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
void*v;};struct _dynforward_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _dynforward_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _dynforward_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _dynforward_ptr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct
_dynforward_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dynforward_ptr);struct Cyc_PP_Doc*Cyc_PP_seq(
struct _dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(
struct Cyc_PP_Doc*(*pp)(void*),struct _dynforward_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dynforward_ptr sep,struct Cyc_List_List*l0);
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct
_dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(struct
_dynforward_ptr start,struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct
Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dynforward_ptr start,
struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
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
_tuple0*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct
_tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
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
_tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;
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
struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union);
struct Cyc_Buffer_t;unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strptrcmp(
struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dynforward_ptr);struct _tuple4{unsigned int f1;int f2;}
;struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void*Cyc_Tcutil_compress(void*t);
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_scopes;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct
_dynforward_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dynforward_ptr Cyc_Absynpp_prim2string(void*p);struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p);struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc);
int Cyc_Absynpp_is_anon_aggrtype(void*t);extern struct _dynforward_ptr Cyc_Absynpp_cyc_string;
extern struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_char_escape(char);struct
_dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr);struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(struct
Cyc_Absyn_Stmt*s);struct _tuple6{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7 Cyc_Absynpp_to_tms(
struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*
fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es);struct _tuple9{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct
Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*
fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);static
int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[
4]="Cyc";struct _dynforward_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0 + 4};struct
_dynforward_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;static int Cyc_Absynpp_add_cyc_prefix;
static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;static int Cyc_Absynpp_rewrite_temp_tvars;
static int Cyc_Absynpp_print_all_tvars;static int Cyc_Absynpp_print_all_kinds;static
int Cyc_Absynpp_print_all_effects;static int Cyc_Absynpp_print_using_stmts;static
int Cyc_Absynpp_print_externC_stmts;static int Cyc_Absynpp_print_full_evars;static
int Cyc_Absynpp_generate_line_directives;static int Cyc_Absynpp_use_curr_namespace;
static int Cyc_Absynpp_print_zeroterm;static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=
fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dynforward_ptr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dynforward_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0:
return({const char*_tmp2="\\a";_tag_dynforward(_tmp2,sizeof(char),
_get_zero_arr_size(_tmp2,3));});case '\b': _LL1: return({const char*_tmp3="\\b";
_tag_dynforward(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,3));});case '\f': _LL2:
return({const char*_tmp4="\\f";_tag_dynforward(_tmp4,sizeof(char),
_get_zero_arr_size(_tmp4,3));});case '\n': _LL3: return({const char*_tmp5="\\n";
_tag_dynforward(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,3));});case '\r': _LL4:
return({const char*_tmp6="\\r";_tag_dynforward(_tmp6,sizeof(char),
_get_zero_arr_size(_tmp6,3));});case '\t': _LL5: return({const char*_tmp7="\\t";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,3));});case '\v': _LL6:
return({const char*_tmp8="\\v";_tag_dynforward(_tmp8,sizeof(char),
_get_zero_arr_size(_tmp8,3));});case '\\': _LL7: return({const char*_tmp9="\\\\";
_tag_dynforward(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,3));});case '"': _LL8:
return({const char*_tmpA="\"";_tag_dynforward(_tmpA,sizeof(char),
_get_zero_arr_size(_tmpA,2));});case '\'': _LL9: return({const char*_tmpB="\\'";
_tag_dynforward(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,3));});default: _LLA:
if(c >= ' '  && c <= '~'){struct _dynforward_ptr _tmpC=Cyc_Core_new_string(2);({
struct _dynforward_ptr _tmpD=_dynforward_ptr_plus(_tmpC,sizeof(char),0);char _tmpE=*((
char*)_check_dynforward_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(
_get_dynforward_size(_tmpD,sizeof(char))== 1  && (_tmpE == '\000'  && _tmpF != '\000'))
_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});return(struct _dynforward_ptr)
_tmpC;}else{struct _dynforward_ptr _tmp10=Cyc_Core_new_string(5);int j=0;({struct
_dynforward_ptr _tmp11=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((
char*)_check_dynforward_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_dynforward_size(_tmp11,sizeof(char))== 1  && (_tmp12 == '\000'  && _tmp13 != '\000'))
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _dynforward_ptr
_tmp14=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_dynforward_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_dynforward_size(_tmp14,sizeof(char))== 1  && (
_tmp15 == '\000'  && _tmp16 != '\000'))_throw_arraybounds();*((char*)_tmp14.curr)=
_tmp16;});({struct _dynforward_ptr _tmp17=_dynforward_ptr_plus(_tmp10,sizeof(char),
j ++);char _tmp18=*((char*)_check_dynforward_subscript(_tmp17,sizeof(char),0));
char _tmp19=(char)('0' + (c >> 3 & 7));if(_get_dynforward_size(_tmp17,sizeof(char))
== 1  && (_tmp18 == '\000'  && _tmp19 != '\000'))_throw_arraybounds();*((char*)
_tmp17.curr)=_tmp19;});({struct _dynforward_ptr _tmp1A=_dynforward_ptr_plus(_tmp10,
sizeof(char),j ++);char _tmp1B=*((char*)_check_dynforward_subscript(_tmp1A,sizeof(
char),0));char _tmp1C=(char)('0' + (c & 7));if(_get_dynforward_size(_tmp1A,sizeof(
char))== 1  && (_tmp1B == '\000'  && _tmp1C != '\000'))_throw_arraybounds();*((char*)
_tmp1A.curr)=_tmp1C;});return(struct _dynforward_ptr)_tmp10;}}}static int Cyc_Absynpp_special(
struct _dynforward_ptr s){int sz=(int)(_get_dynforward_size(s,sizeof(char))- 1);{
int i=0;for(0;i < sz;i ++){char c=*((const char*)_check_dynforward_subscript(s,
sizeof(char),i));if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')return 1;}}
return 0;}struct _dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr s){
if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_dynforward_size(s,sizeof(
char))- 1);if(n > 0  && *((const char*)_check_dynforward_subscript(s,sizeof(char),n))
== '\000')n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp1D=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;
_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')
goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15:
if(_tmp1D != '\r')goto _LL17;_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;
_LL18: goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D
!= '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;
goto _LLC;_LL1F:;_LL20: if(_tmp1D >= ' '  && _tmp1D <= '~')len ++;else{len +=4;}goto
_LLC;_LLC:;}}{struct _dynforward_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;{int i=0;for(0;i <= n;i ++){char _tmp1E=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));_LL22: if(_tmp1E != '\a')goto _LL24;
_LL23:({struct _dynforward_ptr _tmp1F=_dynforward_ptr_plus(t,sizeof(char),j ++);
char _tmp20=*((char*)_check_dynforward_subscript(_tmp1F,sizeof(char),0));char
_tmp21='\\';if(_get_dynforward_size(_tmp1F,sizeof(char))== 1  && (_tmp20 == '\000'
 && _tmp21 != '\000'))_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({
struct _dynforward_ptr _tmp22=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp23=*((
char*)_check_dynforward_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(
_get_dynforward_size(_tmp22,sizeof(char))== 1  && (_tmp23 == '\000'  && _tmp24 != '\000'))
_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')
goto _LL26;_LL25:({struct _dynforward_ptr _tmp25=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp26=*((char*)_check_dynforward_subscript(_tmp25,sizeof(char),0));
char _tmp27='\\';if(_get_dynforward_size(_tmp25,sizeof(char))== 1  && (_tmp26 == '\000'
 && _tmp27 != '\000'))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({
struct _dynforward_ptr _tmp28=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp29=*((
char*)_check_dynforward_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(
_get_dynforward_size(_tmp28,sizeof(char))== 1  && (_tmp29 == '\000'  && _tmp2A != '\000'))
_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')
goto _LL28;_LL27:({struct _dynforward_ptr _tmp2B=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp2C=*((char*)_check_dynforward_subscript(_tmp2B,sizeof(char),0));
char _tmp2D='\\';if(_get_dynforward_size(_tmp2B,sizeof(char))== 1  && (_tmp2C == '\000'
 && _tmp2D != '\000'))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({
struct _dynforward_ptr _tmp2E=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp2F=*((
char*)_check_dynforward_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(
_get_dynforward_size(_tmp2E,sizeof(char))== 1  && (_tmp2F == '\000'  && _tmp30 != '\000'))
_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')
goto _LL2A;_LL29:({struct _dynforward_ptr _tmp31=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp32=*((char*)_check_dynforward_subscript(_tmp31,sizeof(char),0));
char _tmp33='\\';if(_get_dynforward_size(_tmp31,sizeof(char))== 1  && (_tmp32 == '\000'
 && _tmp33 != '\000'))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({
struct _dynforward_ptr _tmp34=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp35=*((
char*)_check_dynforward_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(
_get_dynforward_size(_tmp34,sizeof(char))== 1  && (_tmp35 == '\000'  && _tmp36 != '\000'))
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')
goto _LL2C;_LL2B:({struct _dynforward_ptr _tmp37=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp38=*((char*)_check_dynforward_subscript(_tmp37,sizeof(char),0));
char _tmp39='\\';if(_get_dynforward_size(_tmp37,sizeof(char))== 1  && (_tmp38 == '\000'
 && _tmp39 != '\000'))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({
struct _dynforward_ptr _tmp3A=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp3B=*((
char*)_check_dynforward_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(
_get_dynforward_size(_tmp3A,sizeof(char))== 1  && (_tmp3B == '\000'  && _tmp3C != '\000'))
_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')
goto _LL2E;_LL2D:({struct _dynforward_ptr _tmp3D=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp3E=*((char*)_check_dynforward_subscript(_tmp3D,sizeof(char),0));
char _tmp3F='\\';if(_get_dynforward_size(_tmp3D,sizeof(char))== 1  && (_tmp3E == '\000'
 && _tmp3F != '\000'))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({
struct _dynforward_ptr _tmp40=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp41=*((
char*)_check_dynforward_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(
_get_dynforward_size(_tmp40,sizeof(char))== 1  && (_tmp41 == '\000'  && _tmp42 != '\000'))
_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')
goto _LL30;_LL2F:({struct _dynforward_ptr _tmp43=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp44=*((char*)_check_dynforward_subscript(_tmp43,sizeof(char),0));
char _tmp45='\\';if(_get_dynforward_size(_tmp43,sizeof(char))== 1  && (_tmp44 == '\000'
 && _tmp45 != '\000'))_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({
struct _dynforward_ptr _tmp46=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp47=*((
char*)_check_dynforward_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(
_get_dynforward_size(_tmp46,sizeof(char))== 1  && (_tmp47 == '\000'  && _tmp48 != '\000'))
_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')
goto _LL32;_LL31:({struct _dynforward_ptr _tmp49=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp4A=*((char*)_check_dynforward_subscript(_tmp49,sizeof(char),0));
char _tmp4B='\\';if(_get_dynforward_size(_tmp49,sizeof(char))== 1  && (_tmp4A == '\000'
 && _tmp4B != '\000'))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({
struct _dynforward_ptr _tmp4C=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp4D=*((
char*)_check_dynforward_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_dynforward_size(_tmp4C,sizeof(char))== 1  && (_tmp4D == '\000'  && _tmp4E != '\000'))
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _dynforward_ptr _tmp4F=_dynforward_ptr_plus(t,sizeof(char),
j ++);char _tmp50=*((char*)_check_dynforward_subscript(_tmp4F,sizeof(char),0));
char _tmp51='\\';if(_get_dynforward_size(_tmp4F,sizeof(char))== 1  && (_tmp50 == '\000'
 && _tmp51 != '\000'))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({
struct _dynforward_ptr _tmp52=_dynforward_ptr_plus(t,sizeof(char),j ++);char _tmp53=*((
char*)_check_dynforward_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(
_get_dynforward_size(_tmp52,sizeof(char))== 1  && (_tmp53 == '\000'  && _tmp54 != '\000'))
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(
_tmp1E >= ' '  && _tmp1E <= '~')({struct _dynforward_ptr _tmp55=_dynforward_ptr_plus(
t,sizeof(char),j ++);char _tmp56=*((char*)_check_dynforward_subscript(_tmp55,
sizeof(char),0));char _tmp57=_tmp1E;if(_get_dynforward_size(_tmp55,sizeof(char))
== 1  && (_tmp56 == '\000'  && _tmp57 != '\000'))_throw_arraybounds();*((char*)
_tmp55.curr)=_tmp57;});else{({struct _dynforward_ptr _tmp58=_dynforward_ptr_plus(t,
sizeof(char),j ++);char _tmp59=*((char*)_check_dynforward_subscript(_tmp58,sizeof(
char),0));char _tmp5A='\\';if(_get_dynforward_size(_tmp58,sizeof(char))== 1  && (
_tmp59 == '\000'  && _tmp5A != '\000'))_throw_arraybounds();*((char*)_tmp58.curr)=
_tmp5A;});({struct _dynforward_ptr _tmp5B=_dynforward_ptr_plus(t,sizeof(char),j ++);
char _tmp5C=*((char*)_check_dynforward_subscript(_tmp5B,sizeof(char),0));char
_tmp5D=(char)('0' + (_tmp1E >> 6 & 7));if(_get_dynforward_size(_tmp5B,sizeof(char))
== 1  && (_tmp5C == '\000'  && _tmp5D != '\000'))_throw_arraybounds();*((char*)
_tmp5B.curr)=_tmp5D;});({struct _dynforward_ptr _tmp5E=_dynforward_ptr_plus(t,
sizeof(char),j ++);char _tmp5F=*((char*)_check_dynforward_subscript(_tmp5E,sizeof(
char),0));char _tmp60=(char)('0' + (_tmp1E >> 3 & 7));if(_get_dynforward_size(_tmp5E,
sizeof(char))== 1  && (_tmp5F == '\000'  && _tmp60 != '\000'))_throw_arraybounds();*((
char*)_tmp5E.curr)=_tmp60;});({struct _dynforward_ptr _tmp61=_dynforward_ptr_plus(
t,sizeof(char),j ++);char _tmp62=*((char*)_check_dynforward_subscript(_tmp61,
sizeof(char),0));char _tmp63=(char)('0' + (_tmp1E & 7));if(_get_dynforward_size(
_tmp61,sizeof(char))== 1  && (_tmp62 == '\000'  && _tmp63 != '\000'))
_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto _LL21;_LL21:;}}return(
struct _dynforward_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dynforward_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64 + 9};static char _tmp65[9]="volatile";
static struct _dynforward_ptr Cyc_Absynpp_volatile_string={_tmp65,_tmp65 + 9};static
char _tmp66[6]="const";static struct _dynforward_ptr Cyc_Absynpp_const_str={_tmp66,
_tmp66 + 6};static struct _dynforward_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dynforward_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dynforward_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct
Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=
0;if(tq.q_restrict)l=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->hd=Cyc_Absynpp_restrict_sp;_tmp67->tl=l;_tmp67;});if(tq.q_volatile)l=({
struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=Cyc_Absynpp_volatile_sp;
_tmp68->tl=l;_tmp68;});if(tq.print_const)l=({struct Cyc_List_List*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->hd=Cyc_Absynpp_const_sp;_tmp69->tl=l;_tmp69;});
return Cyc_PP_egroup(({const char*_tmp6A="";_tag_dynforward(_tmp6A,sizeof(char),
_get_zero_arr_size(_tmp6A,1));}),({const char*_tmp6B=" ";_tag_dynforward(_tmp6B,
sizeof(char),_get_zero_arr_size(_tmp6B,2));}),({const char*_tmp6C=" ";
_tag_dynforward(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,l));}struct _dynforward_ptr Cyc_Absynpp_kind2string(void*k){void*
_tmp6D=k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: return({const char*_tmp6E="A";
_tag_dynforward(_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,2));});_LL39: if((
int)_tmp6D != 1)goto _LL3B;_LL3A: return({const char*_tmp6F="M";_tag_dynforward(
_tmp6F,sizeof(char),_get_zero_arr_size(_tmp6F,2));});_LL3B: if((int)_tmp6D != 2)
goto _LL3D;_LL3C: return({const char*_tmp70="B";_tag_dynforward(_tmp70,sizeof(char),
_get_zero_arr_size(_tmp70,2));});_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: return({
const char*_tmp71="R";_tag_dynforward(_tmp71,sizeof(char),_get_zero_arr_size(
_tmp71,2));});_LL3F: if((int)_tmp6D != 4)goto _LL41;_LL40: return({const char*_tmp72="UR";
_tag_dynforward(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,3));});_LL41: if((
int)_tmp6D != 5)goto _LL43;_LL42: return({const char*_tmp73="TR";_tag_dynforward(
_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,3));});_LL43: if((int)_tmp6D != 6)
goto _LL45;_LL44: return({const char*_tmp74="E";_tag_dynforward(_tmp74,sizeof(char),
_get_zero_arr_size(_tmp74,2));});_LL45: if((int)_tmp6D != 7)goto _LL36;_LL46: return({
const char*_tmp75="I";_tag_dynforward(_tmp75,sizeof(char),_get_zero_arr_size(
_tmp75,2));});_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(
Cyc_Absynpp_kind2string(k));}struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*c){void*_tmp76=Cyc_Absyn_compress_kb(c);void*_tmp77;void*_tmp78;_LL48: if(*((
int*)_tmp76)!= 0)goto _LL4A;_tmp77=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp76)->f1;
_LL49: return Cyc_Absynpp_kind2string(_tmp77);_LL4A: if(*((int*)_tmp76)!= 1)goto
_LL4C;_LL4B: if(Cyc_PP_tex_output)return({const char*_tmp79="{?}";_tag_dynforward(
_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,4));});else{return({const char*
_tmp7A="?";_tag_dynforward(_tmp7A,sizeof(char),_get_zero_arr_size(_tmp7A,2));});}
_LL4C: if(*((int*)_tmp76)!= 2)goto _LL47;_tmp78=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp76)->f2;_LL4D: return Cyc_Absynpp_kind2string(_tmp78);_LL47:;}struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c){void*_tmp7B=Cyc_Absyn_compress_kb(c);void*
_tmp7C;void*_tmp7D;_LL4F: if(*((int*)_tmp7B)!= 0)goto _LL51;_tmp7C=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B)->f1;_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(
_tmp7C));_LL51: if(*((int*)_tmp7B)!= 1)goto _LL53;_LL52: if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp7E="{?}";_tag_dynforward(_tmp7E,sizeof(
char),_get_zero_arr_size(_tmp7E,4));}),1);else{return Cyc_PP_text(({const char*
_tmp7F="?";_tag_dynforward(_tmp7F,sizeof(char),_get_zero_arr_size(_tmp7F,2));}));}
_LL53: if(*((int*)_tmp7B)!= 2)goto _LL4E;_tmp7D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7B)->f2;_LL54: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7D));_LL4E:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){return Cyc_PP_egroup(({
const char*_tmp80="<";_tag_dynforward(_tmp80,sizeof(char),_get_zero_arr_size(
_tmp80,2));}),({const char*_tmp81=">";_tag_dynforward(_tmp81,sizeof(char),
_get_zero_arr_size(_tmp81,2));}),({const char*_tmp82=",";_tag_dynforward(_tmp82,
sizeof(char),_get_zero_arr_size(_tmp82,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*
_tmp83=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp84;void*_tmp85;void*
_tmp86;_LL56: if(*((int*)_tmp83)!= 1)goto _LL58;_LL57: goto _LL59;_LL58: if(*((int*)
_tmp83)!= 0)goto _LL5A;_tmp84=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;
if((int)_tmp84 != 2)goto _LL5A;_LL59: return Cyc_PP_textptr(tv->name);_LL5A: if(*((
int*)_tmp83)!= 2)goto _LL5C;_tmp85=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp83)->f2;_LL5B: _tmp86=_tmp85;goto _LL5D;_LL5C: if(*((int*)_tmp83)!= 0)goto _LL55;
_tmp86=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;_LL5D: return({struct
Cyc_PP_Doc*_tmp87[3];_tmp87[2]=Cyc_Absynpp_kind2doc(_tmp86);_tmp87[1]=Cyc_PP_text(({
const char*_tmp88="::";_tag_dynforward(_tmp88,sizeof(char),_get_zero_arr_size(
_tmp88,3));}));_tmp87[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(_tag_dynforward(
_tmp87,sizeof(struct Cyc_PP_Doc*),3));});_LL55:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){return Cyc_PP_egroup(({const char*_tmp89="<";
_tag_dynforward(_tmp89,sizeof(char),_get_zero_arr_size(_tmp89,2));}),({const char*
_tmp8A=">";_tag_dynforward(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,2));}),({
const char*_tmp8B=",";_tag_dynforward(_tmp8B,sizeof(char),_get_zero_arr_size(
_tmp8B,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dynforward_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({const char*_tmp8C="<";
_tag_dynforward(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,2));}),({const char*
_tmp8D=">";_tag_dynforward(_tmp8D,sizeof(char),_get_zero_arr_size(_tmp8D,2));}),({
const char*_tmp8E=",";_tag_dynforward(_tmp8E,sizeof(char),_get_zero_arr_size(
_tmp8E,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct
Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(({const char*_tmp8F="(";_tag_dynforward(_tmp8F,sizeof(char),
_get_zero_arr_size(_tmp8F,2));}),({const char*_tmp90=")";_tag_dynforward(_tmp90,
sizeof(char),_get_zero_arr_size(_tmp90,2));}),({const char*_tmp91=",";
_tag_dynforward(_tmp91,sizeof(char),_get_zero_arr_size(_tmp91,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp92=att;
_LL5F: if((int)_tmp92 != 0)goto _LL61;_LL60: goto _LL62;_LL61: if((int)_tmp92 != 1)goto
_LL63;_LL62: goto _LL64;_LL63: if((int)_tmp92 != 2)goto _LL65;_LL64: return Cyc_PP_nil_doc();
_LL65:;_LL66: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL5E:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp93=(void*)atts->hd;_LL68: if((int)_tmp93 != 0)goto _LL6A;
_LL69: return Cyc_PP_text(({const char*_tmp94=" _stdcall ";_tag_dynforward(_tmp94,
sizeof(char),_get_zero_arr_size(_tmp94,11));}));_LL6A: if((int)_tmp93 != 1)goto
_LL6C;_LL6B: return Cyc_PP_text(({const char*_tmp95=" _cdecl ";_tag_dynforward(
_tmp95,sizeof(char),_get_zero_arr_size(_tmp95,9));}));_LL6C: if((int)_tmp93 != 2)
goto _LL6E;_LL6D: return Cyc_PP_text(({const char*_tmp96=" _fastcall ";
_tag_dynforward(_tmp96,sizeof(char),_get_zero_arr_size(_tmp96,12));}));_LL6E:;
_LL6F: goto _LL67;_LL67:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp97=(void*)atts2->hd;_LL71: if((int)_tmp97 != 0)
goto _LL73;_LL72: goto _LL74;_LL73: if((int)_tmp97 != 1)goto _LL75;_LL74: goto _LL76;
_LL75: if((int)_tmp97 != 2)goto _LL77;_LL76: goto _LL70;_LL77:;_LL78: hasatt=1;goto
_LL70;_LL70:;}}if(!hasatt)return Cyc_PP_nil_doc();return({struct Cyc_PP_Doc*_tmp98[
3];_tmp98[2]=Cyc_PP_text(({const char*_tmp9D=")";_tag_dynforward(_tmp9D,sizeof(
char),_get_zero_arr_size(_tmp9D,2));}));_tmp98[1]=Cyc_PP_group(({const char*
_tmp9A="";_tag_dynforward(_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,1));}),({
const char*_tmp9B="";_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size(
_tmp9B,1));}),({const char*_tmp9C=" ";_tag_dynforward(_tmp9C,sizeof(char),
_get_zero_arr_size(_tmp9C,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));
_tmp98[0]=Cyc_PP_text(({const char*_tmp99="__declspec(";_tag_dynforward(_tmp99,
sizeof(char),_get_zero_arr_size(_tmp99,12));}));Cyc_PP_cat(_tag_dynforward(
_tmp98,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();if(Cyc_Absynpp_to_VC)
return Cyc_Absynpp_noncallconv2doc(atts);return({struct Cyc_PP_Doc*_tmp9E[2];
_tmp9E[1]=Cyc_PP_group(({const char*_tmpA0="((";_tag_dynforward(_tmpA0,sizeof(
char),_get_zero_arr_size(_tmpA0,3));}),({const char*_tmpA1="))";_tag_dynforward(
_tmpA1,sizeof(char),_get_zero_arr_size(_tmpA1,3));}),({const char*_tmpA2=",";
_tag_dynforward(_tmpA2,sizeof(char),_get_zero_arr_size(_tmpA2,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,
atts));_tmp9E[0]=Cyc_PP_text(({const char*_tmp9F=" __attribute__";_tag_dynforward(
_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,15));}));Cyc_PP_cat(
_tag_dynforward(_tmp9E,sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmpA3=(void*)tms->hd;_LL7A:
if(*((int*)_tmpA3)!= 2)goto _LL7C;_LL7B: return 1;_LL7C: if(*((int*)_tmpA3)!= 5)goto
_LL7E;_LL7D: if(!Cyc_Absynpp_to_VC)return 0;return Cyc_Absynpp_next_is_pointer(tms->tl);
_LL7E:;_LL7F: return 0;_LL79:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA4="{?}";_tag_dynforward(
_tmpA4,sizeof(char),_get_zero_arr_size(_tmpA4,4));}),1);else{Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA5="?";_tag_dynforward(_tmpA5,
sizeof(char),_get_zero_arr_size(_tmpA5,2));}));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpA6="{\\lb}";_tag_dynforward(_tmpA6,sizeof(char),_get_zero_arr_size(
_tmpA6,6));}),1);else{Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmpA7="{";_tag_dynforward(_tmpA7,sizeof(char),_get_zero_arr_size(
_tmpA7,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA8="{\\rb}";_tag_dynforward(
_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,6));}),1);else{Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA9="}";_tag_dynforward(_tmpA9,
sizeof(char),_get_zero_arr_size(_tmpA9,2));}));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpAA="\\$";_tag_dynforward(_tmpAA,sizeof(char),_get_zero_arr_size(
_tmpAA,3));}),1);else{Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmpAB="$";_tag_dynforward(_tmpAB,sizeof(char),_get_zero_arr_size(
_tmpAB,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dynforward_ptr sep,struct Cyc_List_List*
ss){return({struct Cyc_PP_Doc*_tmpAC[3];_tmpAC[2]=Cyc_Absynpp_rb();_tmpAC[1]=Cyc_PP_seq(
sep,ss);_tmpAC[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(_tmpAC,sizeof(
struct Cyc_PP_Doc*),3));});}static void Cyc_Absynpp_print_tms(struct Cyc_List_List*
tms){for(0;tms != 0;tms=tms->tl){void*_tmpAD=(void*)tms->hd;void*_tmpAE;struct Cyc_List_List*
_tmpAF;_LL81: if(*((int*)_tmpAD)!= 0)goto _LL83;_LL82:({void*_tmpB0[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpB1="Carray_mod ";_tag_dynforward(_tmpB1,sizeof(char),
_get_zero_arr_size(_tmpB1,12));}),_tag_dynforward(_tmpB0,sizeof(void*),0));});
goto _LL80;_LL83: if(*((int*)_tmpAD)!= 1)goto _LL85;_LL84:({void*_tmpB2[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpB3="ConstArray_mod ";_tag_dynforward(_tmpB3,sizeof(
char),_get_zero_arr_size(_tmpB3,16));}),_tag_dynforward(_tmpB2,sizeof(void*),0));});
goto _LL80;_LL85: if(*((int*)_tmpAD)!= 3)goto _LL87;_tmpAE=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAD)->f1;if(*((int*)_tmpAE)!= 1)goto _LL87;_tmpAF=((struct Cyc_Absyn_WithTypes_struct*)
_tmpAE)->f1;_LL86:({void*_tmpB4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB5="Function_mod(";
_tag_dynforward(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,14));}),
_tag_dynforward(_tmpB4,sizeof(void*),0));});for(0;_tmpAF != 0;_tmpAF=_tmpAF->tl){
struct Cyc_Core_Opt*_tmpB6=(*((struct _tuple1*)_tmpAF->hd)).f1;if(_tmpB6 == 0)({
void*_tmpB7[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB8="?";_tag_dynforward(
_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,2));}),_tag_dynforward(_tmpB7,
sizeof(void*),0));});else{({void*_tmpB9[0]={};Cyc_fprintf(Cyc_stderr,*((struct
_dynforward_ptr*)_tmpB6->v),_tag_dynforward(_tmpB9,sizeof(void*),0));});}if(
_tmpAF->tl != 0)({void*_tmpBA[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpBB=",";
_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,2));}),
_tag_dynforward(_tmpBA,sizeof(void*),0));});}({void*_tmpBC[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpBD=") ";_tag_dynforward(_tmpBD,sizeof(char),_get_zero_arr_size(
_tmpBD,3));}),_tag_dynforward(_tmpBC,sizeof(void*),0));});goto _LL80;_LL87: if(*((
int*)_tmpAD)!= 3)goto _LL89;_LL88:({void*_tmpBE[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpBF="Function_mod()";_tag_dynforward(_tmpBF,sizeof(char),
_get_zero_arr_size(_tmpBF,15));}),_tag_dynforward(_tmpBE,sizeof(void*),0));});
goto _LL80;_LL89: if(*((int*)_tmpAD)!= 5)goto _LL8B;_LL8A:({void*_tmpC0[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC1="Attributes_mod ";_tag_dynforward(_tmpC1,sizeof(
char),_get_zero_arr_size(_tmpC1,16));}),_tag_dynforward(_tmpC0,sizeof(void*),0));});
goto _LL80;_LL8B: if(*((int*)_tmpAD)!= 4)goto _LL8D;_LL8C:({void*_tmpC2[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC3="TypeParams_mod ";_tag_dynforward(_tmpC3,sizeof(
char),_get_zero_arr_size(_tmpC3,16));}),_tag_dynforward(_tmpC2,sizeof(void*),0));});
goto _LL80;_LL8D: if(*((int*)_tmpAD)!= 2)goto _LL80;_LL8E:({void*_tmpC4[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC5="Pointer_mod ";_tag_dynforward(_tmpC5,sizeof(char),
_get_zero_arr_size(_tmpC5,13));}),_tag_dynforward(_tmpC4,sizeof(void*),0));});
goto _LL80;_LL80:;}({void*_tmpC6[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpC7="\n";
_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size(_tmpC7,2));}),
_tag_dynforward(_tmpC6,sizeof(void*),0));});}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);struct Cyc_PP_Doc*p_rest=({
struct Cyc_PP_Doc*_tmp106[3];_tmp106[2]=Cyc_PP_text(({const char*_tmp108=")";
_tag_dynforward(_tmp108,sizeof(char),_get_zero_arr_size(_tmp108,2));}));_tmp106[
1]=rest;_tmp106[0]=Cyc_PP_text(({const char*_tmp107="(";_tag_dynforward(_tmp107,
sizeof(char),_get_zero_arr_size(_tmp107,2));}));Cyc_PP_cat(_tag_dynforward(
_tmp106,sizeof(struct Cyc_PP_Doc*),3));});void*_tmpC8=(void*)tms->hd;struct Cyc_Absyn_Conref*
_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Conref*_tmpCB;void*_tmpCC;
struct Cyc_List_List*_tmpCD;struct Cyc_List_List*_tmpCE;struct Cyc_Position_Segment*
_tmpCF;int _tmpD0;struct Cyc_Absyn_PtrAtts _tmpD1;void*_tmpD2;struct Cyc_Absyn_Conref*
_tmpD3;struct Cyc_Absyn_Conref*_tmpD4;struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Tqual
_tmpD6;_LL90: if(*((int*)_tmpC8)!= 0)goto _LL92;_tmpC9=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpC8)->f1;_LL91: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({
struct Cyc_PP_Doc*_tmpD7[2];_tmpD7[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpC9)?Cyc_PP_text(({const char*_tmpD8="[]ZEROTERM ";_tag_dynforward(_tmpD8,
sizeof(char),_get_zero_arr_size(_tmpD8,12));})): Cyc_PP_text(({const char*_tmpD9="[]";
_tag_dynforward(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,3));}));_tmpD7[0]=
rest;Cyc_PP_cat(_tag_dynforward(_tmpD7,sizeof(struct Cyc_PP_Doc*),2));});_LL92:
if(*((int*)_tmpC8)!= 1)goto _LL94;_tmpCA=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpC8)->f1;_tmpCB=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpC8)->f2;_LL93:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpDA[4];_tmpDA[3]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCB)?Cyc_PP_text(({const char*_tmpDC="]ZEROTERM ";_tag_dynforward(_tmpDC,
sizeof(char),_get_zero_arr_size(_tmpDC,11));})): Cyc_PP_text(({const char*_tmpDD="]";
_tag_dynforward(_tmpDD,sizeof(char),_get_zero_arr_size(_tmpDD,2));}));_tmpDA[2]=
Cyc_Absynpp_exp2doc(_tmpCA);_tmpDA[1]=Cyc_PP_text(({const char*_tmpDB="[";
_tag_dynforward(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,2));}));_tmpDA[0]=
rest;Cyc_PP_cat(_tag_dynforward(_tmpDA,sizeof(struct Cyc_PP_Doc*),4));});_LL94:
if(*((int*)_tmpC8)!= 3)goto _LL96;_tmpCC=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpC8)->f1;_LL95: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpDE=_tmpCC;struct Cyc_List_List*_tmpDF;int _tmpE0;struct Cyc_Absyn_VarargInfo*
_tmpE1;struct Cyc_Core_Opt*_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_List_List*
_tmpE4;struct Cyc_Position_Segment*_tmpE5;_LL9D: if(*((int*)_tmpDE)!= 1)goto _LL9F;
_tmpDF=((struct Cyc_Absyn_WithTypes_struct*)_tmpDE)->f1;_tmpE0=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDE)->f2;_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpDE)->f3;_tmpE2=((
struct Cyc_Absyn_WithTypes_struct*)_tmpDE)->f4;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDE)->f5;_LL9E: return({struct Cyc_PP_Doc*_tmpE6[2];_tmpE6[1]=Cyc_Absynpp_funargs2doc(
_tmpDF,_tmpE0,_tmpE1,_tmpE2,_tmpE3);_tmpE6[0]=rest;Cyc_PP_cat(_tag_dynforward(
_tmpE6,sizeof(struct Cyc_PP_Doc*),2));});_LL9F: if(*((int*)_tmpDE)!= 0)goto _LL9C;
_tmpE4=((struct Cyc_Absyn_NoTypes_struct*)_tmpDE)->f1;_tmpE5=((struct Cyc_Absyn_NoTypes_struct*)
_tmpDE)->f2;_LLA0: return({struct Cyc_PP_Doc*_tmpE7[2];_tmpE7[1]=Cyc_PP_group(({
const char*_tmpE8="(";_tag_dynforward(_tmpE8,sizeof(char),_get_zero_arr_size(
_tmpE8,2));}),({const char*_tmpE9=")";_tag_dynforward(_tmpE9,sizeof(char),
_get_zero_arr_size(_tmpE9,2));}),({const char*_tmpEA=",";_tag_dynforward(_tmpEA,
sizeof(char),_get_zero_arr_size(_tmpEA,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,
_tmpE4));_tmpE7[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpE7,sizeof(struct Cyc_PP_Doc*),
2));});_LL9C:;}_LL96: if(*((int*)_tmpC8)!= 5)goto _LL98;_tmpCD=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC8)->f2;_LL97: if(!Cyc_Absynpp_to_VC){if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;return({struct Cyc_PP_Doc*_tmpEB[2];_tmpEB[1]=Cyc_Absynpp_atts2doc(
_tmpCD);_tmpEB[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpEB,sizeof(struct Cyc_PP_Doc*),
2));});}else{if(Cyc_Absynpp_next_is_pointer(tms->tl))return({struct Cyc_PP_Doc*
_tmpEC[2];_tmpEC[1]=rest;_tmpEC[0]=Cyc_Absynpp_callconv2doc(_tmpCD);Cyc_PP_cat(
_tag_dynforward(_tmpEC,sizeof(struct Cyc_PP_Doc*),2));});return rest;}_LL98: if(*((
int*)_tmpC8)!= 4)goto _LL9A;_tmpCE=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpC8)->f1;_tmpCF=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC8)->f2;_tmpD0=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC8)->f3;_LL99: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpD0)return({struct Cyc_PP_Doc*_tmpED[2];_tmpED[1]=Cyc_Absynpp_ktvars2doc(
_tmpCE);_tmpED[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpED,sizeof(struct Cyc_PP_Doc*),
2));});else{return({struct Cyc_PP_Doc*_tmpEE[2];_tmpEE[1]=Cyc_Absynpp_tvars2doc(
_tmpCE);_tmpEE[0]=rest;Cyc_PP_cat(_tag_dynforward(_tmpEE,sizeof(struct Cyc_PP_Doc*),
2));});}_LL9A: if(*((int*)_tmpC8)!= 2)goto _LL8F;_tmpD1=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC8)->f1;_tmpD2=(void*)_tmpD1.rgn;_tmpD3=_tmpD1.nullable;_tmpD4=_tmpD1.bounds;
_tmpD5=_tmpD1.zero_term;_tmpD6=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpC8)->f2;
_LL9B: {struct Cyc_PP_Doc*ptr;{void*_tmpEF=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpD4);struct Cyc_Absyn_Exp*_tmpF0;void*_tmpF1;_LLA2: if((int)_tmpEF != 0)goto
_LLA4;_LLA3: ptr=Cyc_Absynpp_question();goto _LLA1;_LLA4: if((int)_tmpEF != 1)goto
_LLA6;_LLA5: ptr=({struct Cyc_PP_Doc*_tmpF2[2];_tmpF2[1]=Cyc_PP_text(({const char*
_tmpF3="-";_tag_dynforward(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,2));}));
_tmpF2[0]=Cyc_Absynpp_question();Cyc_PP_cat(_tag_dynforward(_tmpF2,sizeof(struct
Cyc_PP_Doc*),2));});goto _LLA1;_LLA6: if(_tmpEF <= (void*)2)goto _LLA8;if(*((int*)
_tmpEF)!= 0)goto _LLA8;_tmpF0=((struct Cyc_Absyn_Upper_b_struct*)_tmpEF)->f1;_LLA7:
ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,
_tmpD3)?({const char*_tmpF4="*";_tag_dynforward(_tmpF4,sizeof(char),
_get_zero_arr_size(_tmpF4,2));}):({const char*_tmpF5="@";_tag_dynforward(_tmpF5,
sizeof(char),_get_zero_arr_size(_tmpF5,2));}));{unsigned int _tmpF7;int _tmpF8;
struct _tuple4 _tmpF6=Cyc_Evexp_eval_const_uint_exp(_tmpF0);_tmpF7=_tmpF6.f1;
_tmpF8=_tmpF6.f2;if(!_tmpF8  || _tmpF7 != 1)ptr=({struct Cyc_PP_Doc*_tmpF9[4];
_tmpF9[3]=Cyc_Absynpp_rb();_tmpF9[2]=Cyc_Absynpp_exp2doc(_tmpF0);_tmpF9[1]=Cyc_Absynpp_lb();
_tmpF9[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmpF9,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLA1;}_LLA8: if(_tmpEF <= (void*)2)goto _LLA1;if(*((int*)_tmpEF)!= 1)goto _LLA1;
_tmpF1=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmpEF)->f1;_LLA9: ptr=Cyc_PP_text(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpD3)?({const char*
_tmpFA="*";_tag_dynforward(_tmpFA,sizeof(char),_get_zero_arr_size(_tmpFA,2));}):({
const char*_tmpFB="@";_tag_dynforward(_tmpFB,sizeof(char),_get_zero_arr_size(
_tmpFB,2));}));ptr=({struct Cyc_PP_Doc*_tmpFC[4];_tmpFC[3]=Cyc_Absynpp_rb();
_tmpFC[2]=Cyc_Absynpp_typ2doc(_tmpF1);_tmpFC[1]=Cyc_Absynpp_lb();_tmpFC[0]=ptr;
Cyc_PP_cat(_tag_dynforward(_tmpFC,sizeof(struct Cyc_PP_Doc*),4));});goto _LLA1;
_LLA1:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD5))ptr=({struct Cyc_PP_Doc*_tmpFD[2];_tmpFD[1]=
Cyc_PP_text(({const char*_tmpFE="ZEROTERM ";_tag_dynforward(_tmpFE,sizeof(char),
_get_zero_arr_size(_tmpFE,10));}));_tmpFD[0]=ptr;Cyc_PP_cat(_tag_dynforward(
_tmpFD,sizeof(struct Cyc_PP_Doc*),2));});else{if(is_char_ptr  && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD5))ptr=({struct Cyc_PP_Doc*
_tmpFF[2];_tmpFF[1]=Cyc_PP_text(({const char*_tmp100="NOZEROTERM ";
_tag_dynforward(_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,12));}));_tmpFF[
0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmpFF,sizeof(struct Cyc_PP_Doc*),2));});}}{
void*_tmp101=Cyc_Tcutil_compress(_tmpD2);_LLAB: if((int)_tmp101 != 2)goto _LLAD;
_LLAC: goto _LLAA;_LLAD:;_LLAE: ptr=({struct Cyc_PP_Doc*_tmp102[3];_tmp102[2]=Cyc_PP_text(({
const char*_tmp103=" ";_tag_dynforward(_tmp103,sizeof(char),_get_zero_arr_size(
_tmp103,2));}));_tmp102[1]=Cyc_Absynpp_typ2doc(_tmpD2);_tmp102[0]=ptr;Cyc_PP_cat(
_tag_dynforward(_tmp102,sizeof(struct Cyc_PP_Doc*),3));});_LLAA:;}ptr=({struct Cyc_PP_Doc*
_tmp104[2];_tmp104[1]=Cyc_Absynpp_tqual2doc(_tmpD6);_tmp104[0]=ptr;Cyc_PP_cat(
_tag_dynforward(_tmp104,sizeof(struct Cyc_PP_Doc*),2));});return({struct Cyc_PP_Doc*
_tmp105[2];_tmp105[1]=rest;_tmp105[0]=ptr;Cyc_PP_cat(_tag_dynforward(_tmp105,
sizeof(struct Cyc_PP_Doc*),2));});}_LL8F:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp109=Cyc_Tcutil_compress(t);_LLB0: if((int)_tmp109 != 2)goto _LLB2;
_LLB1: return Cyc_PP_text(({const char*_tmp10A="`H";_tag_dynforward(_tmp10A,sizeof(
char),_get_zero_arr_size(_tmp10A,3));}));_LLB2: if((int)_tmp109 != 3)goto _LLB4;
_LLB3: return Cyc_PP_text(({const char*_tmp10B="`U";_tag_dynforward(_tmp10B,sizeof(
char),_get_zero_arr_size(_tmp10B,3));}));_LLB4:;_LLB5: return Cyc_Absynpp_ntyp2doc(
t);_LLAF:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t){void*_tmp10C=Cyc_Tcutil_compress(t);void*_tmp10D;
struct Cyc_List_List*_tmp10E;_LLB7: if(_tmp10C <= (void*)4)goto _LLBB;if(*((int*)
_tmp10C)!= 20)goto _LLB9;_tmp10D=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp10C)->f1;_LLB8:*rgions=({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*
_tmp10F));_tmp10F->hd=Cyc_Absynpp_rgn2doc(_tmp10D);_tmp10F->tl=*rgions;_tmp10F;});
goto _LLB6;_LLB9: if(*((int*)_tmp10C)!= 21)goto _LLBB;_tmp10E=((struct Cyc_Absyn_JoinEff_struct*)
_tmp10C)->f1;_LLBA: for(0;_tmp10E != 0;_tmp10E=_tmp10E->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp10E->hd);}goto _LLB6;_LLBB:;_LLBC:*effects=({struct Cyc_List_List*
_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->hd=Cyc_Absynpp_typ2doc(t);_tmp110->tl=*
effects;_tmp110;});goto _LLB6;_LLB6:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0)return Cyc_PP_group(({const char*_tmp111="";
_tag_dynforward(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,1));}),({const
char*_tmp112="";_tag_dynforward(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,
1));}),({const char*_tmp113="+";_tag_dynforward(_tmp113,sizeof(char),
_get_zero_arr_size(_tmp113,2));}),effects);else{struct Cyc_PP_Doc*_tmp114=Cyc_Absynpp_group_braces(({
const char*_tmp119=",";_tag_dynforward(_tmp119,sizeof(char),_get_zero_arr_size(
_tmp119,2));}),rgions);return Cyc_PP_group(({const char*_tmp115="";_tag_dynforward(
_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,1));}),({const char*_tmp116="";
_tag_dynforward(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,1));}),({const
char*_tmp117="+";_tag_dynforward(_tmp117,sizeof(char),_get_zero_arr_size(_tmp117,
2));}),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(effects,({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*
_tmp118));_tmp118->hd=_tmp114;_tmp118->tl=0;_tmp118;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
void*k){void*_tmp11A=k;_LLBE: if((int)_tmp11A != 0)goto _LLC0;_LLBF: return Cyc_PP_text(({
const char*_tmp11B="struct ";_tag_dynforward(_tmp11B,sizeof(char),
_get_zero_arr_size(_tmp11B,8));}));_LLC0: if((int)_tmp11A != 1)goto _LLBD;_LLC1:
return Cyc_PP_text(({const char*_tmp11C="union ";_tag_dynforward(_tmp11C,sizeof(
char),_get_zero_arr_size(_tmp11C,7));}));_LLBD:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t){struct Cyc_PP_Doc*s;{void*_tmp11D=t;struct Cyc_Core_Opt*_tmp11E;struct Cyc_Core_Opt*
_tmp11F;int _tmp120;struct Cyc_Core_Opt*_tmp121;struct Cyc_Absyn_Tvar*_tmp122;
struct Cyc_Absyn_TunionInfo _tmp123;union Cyc_Absyn_TunionInfoU_union _tmp124;struct
Cyc_List_List*_tmp125;struct Cyc_Core_Opt*_tmp126;struct Cyc_Absyn_TunionFieldInfo
_tmp127;union Cyc_Absyn_TunionFieldInfoU_union _tmp128;struct Cyc_List_List*_tmp129;
void*_tmp12A;void*_tmp12B;int _tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_AggrInfo
_tmp12E;union Cyc_Absyn_AggrInfoU_union _tmp12F;struct Cyc_List_List*_tmp130;void*
_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_List_List*_tmp133;struct _tuple0*
_tmp134;void*_tmp135;struct _tuple0*_tmp136;struct Cyc_List_List*_tmp137;struct Cyc_Absyn_Typedefdecl*
_tmp138;void*_tmp139;void*_tmp13A;void*_tmp13B;int _tmp13C;void*_tmp13D;void*
_tmp13E;_LLC3: if(_tmp11D <= (void*)4)goto _LLC9;if(*((int*)_tmp11D)!= 7)goto _LLC5;
_LLC4: return Cyc_PP_text(({const char*_tmp13F="[[[array]]]";_tag_dynforward(
_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,12));}));_LLC5: if(*((int*)
_tmp11D)!= 8)goto _LLC7;_LLC6: return Cyc_PP_nil_doc();_LLC7: if(*((int*)_tmp11D)!= 
4)goto _LLC9;_LLC8: return Cyc_PP_nil_doc();_LLC9: if((int)_tmp11D != 0)goto _LLCB;
_LLCA: s=Cyc_PP_text(({const char*_tmp140="void";_tag_dynforward(_tmp140,sizeof(
char),_get_zero_arr_size(_tmp140,5));}));goto _LLC2;_LLCB: if(_tmp11D <= (void*)4)
goto _LLD5;if(*((int*)_tmp11D)!= 0)goto _LLCD;_tmp11E=((struct Cyc_Absyn_Evar_struct*)
_tmp11D)->f1;_tmp11F=((struct Cyc_Absyn_Evar_struct*)_tmp11D)->f2;_tmp120=((
struct Cyc_Absyn_Evar_struct*)_tmp11D)->f3;_tmp121=((struct Cyc_Absyn_Evar_struct*)
_tmp11D)->f4;_LLCC: if(_tmp11F != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11F->v);
else{s=({struct Cyc_PP_Doc*_tmp141[6];_tmp141[5]=_tmp11E == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((void*)_tmp11E->v);_tmp141[4]=Cyc_PP_text(({const char*
_tmp147=")::";_tag_dynforward(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,4));}));
_tmp141[3]=(!Cyc_Absynpp_print_full_evars  || _tmp121 == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)_tmp121->v);_tmp141[2]=Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp146;_tmp146.tag=1;_tmp146.f1=(unsigned long)_tmp120;{
void*_tmp144[1]={& _tmp146};Cyc_aprintf(({const char*_tmp145="%d";_tag_dynforward(
_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,3));}),_tag_dynforward(_tmp144,
sizeof(void*),1));}}));_tmp141[1]=Cyc_PP_text(({const char*_tmp143="(";
_tag_dynforward(_tmp143,sizeof(char),_get_zero_arr_size(_tmp143,2));}));_tmp141[
0]=Cyc_PP_text(({const char*_tmp142="%";_tag_dynforward(_tmp142,sizeof(char),
_get_zero_arr_size(_tmp142,2));}));Cyc_PP_cat(_tag_dynforward(_tmp141,sizeof(
struct Cyc_PP_Doc*),6));});}goto _LLC2;_LLCD: if(*((int*)_tmp11D)!= 1)goto _LLCF;
_tmp122=((struct Cyc_Absyn_VarType_struct*)_tmp11D)->f1;_LLCE: s=Cyc_PP_textptr(
_tmp122->name);if(Cyc_Absynpp_print_all_kinds)s=({struct Cyc_PP_Doc*_tmp148[3];
_tmp148[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp122->kind);_tmp148[1]=Cyc_PP_text(({
const char*_tmp149="::";_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size(
_tmp149,3));}));_tmp148[0]=s;Cyc_PP_cat(_tag_dynforward(_tmp148,sizeof(struct Cyc_PP_Doc*),
3));});if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp122))s=({
struct Cyc_PP_Doc*_tmp14A[3];_tmp14A[2]=Cyc_PP_text(({const char*_tmp14C=" */";
_tag_dynforward(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,4));}));_tmp14A[
1]=s;_tmp14A[0]=Cyc_PP_text(({const char*_tmp14B="_ /* ";_tag_dynforward(_tmp14B,
sizeof(char),_get_zero_arr_size(_tmp14B,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp14A,sizeof(struct Cyc_PP_Doc*),3));});goto _LLC2;_LLCF: if(*((int*)_tmp11D)!= 2)
goto _LLD1;_tmp123=((struct Cyc_Absyn_TunionType_struct*)_tmp11D)->f1;_tmp124=
_tmp123.tunion_info;_tmp125=_tmp123.targs;_tmp126=_tmp123.rgn;_LLD0:{union Cyc_Absyn_TunionInfoU_union
_tmp14D=_tmp124;struct Cyc_Absyn_UnknownTunionInfo _tmp14E;struct _tuple0*_tmp14F;
int _tmp150;struct Cyc_Absyn_Tuniondecl**_tmp151;struct Cyc_Absyn_Tuniondecl*
_tmp152;struct Cyc_Absyn_Tuniondecl _tmp153;struct _tuple0*_tmp154;int _tmp155;_LLFA:
if((_tmp14D.UnknownTunion).tag != 0)goto _LLFC;_tmp14E=(_tmp14D.UnknownTunion).f1;
_tmp14F=_tmp14E.name;_tmp150=_tmp14E.is_xtunion;_LLFB: _tmp154=_tmp14F;_tmp155=
_tmp150;goto _LLFD;_LLFC: if((_tmp14D.KnownTunion).tag != 1)goto _LLF9;_tmp151=(
_tmp14D.KnownTunion).f1;_tmp152=*_tmp151;_tmp153=*_tmp152;_tmp154=_tmp153.name;
_tmp155=_tmp153.is_xtunion;_LLFD: {struct Cyc_PP_Doc*_tmp156=Cyc_PP_text(_tmp155?({
const char*_tmp15B="xtunion ";_tag_dynforward(_tmp15B,sizeof(char),
_get_zero_arr_size(_tmp15B,9));}):({const char*_tmp15C="tunion ";_tag_dynforward(
_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,8));}));void*r=(unsigned int)
_tmp126?(void*)_tmp126->v:(void*)2;{void*_tmp157=Cyc_Tcutil_compress(r);_LLFF:
if((int)_tmp157 != 2)goto _LL101;_LL100: s=({struct Cyc_PP_Doc*_tmp158[3];_tmp158[2]=
Cyc_Absynpp_tps2doc(_tmp125);_tmp158[1]=Cyc_Absynpp_qvar2doc(_tmp154);_tmp158[0]=
_tmp156;Cyc_PP_cat(_tag_dynforward(_tmp158,sizeof(struct Cyc_PP_Doc*),3));});goto
_LLFE;_LL101:;_LL102: s=({struct Cyc_PP_Doc*_tmp159[5];_tmp159[4]=Cyc_Absynpp_tps2doc(
_tmp125);_tmp159[3]=Cyc_Absynpp_qvar2doc(_tmp154);_tmp159[2]=Cyc_PP_text(({const
char*_tmp15A=" ";_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,
2));}));_tmp159[1]=Cyc_Absynpp_typ2doc(r);_tmp159[0]=_tmp156;Cyc_PP_cat(
_tag_dynforward(_tmp159,sizeof(struct Cyc_PP_Doc*),5));});goto _LLFE;_LLFE:;}goto
_LLF9;}_LLF9:;}goto _LLC2;_LLD1: if(*((int*)_tmp11D)!= 3)goto _LLD3;_tmp127=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp11D)->f1;_tmp128=_tmp127.field_info;
_tmp129=_tmp127.targs;_LLD2:{union Cyc_Absyn_TunionFieldInfoU_union _tmp15D=
_tmp128;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp15E;struct _tuple0*_tmp15F;
struct _tuple0*_tmp160;int _tmp161;struct Cyc_Absyn_Tuniondecl*_tmp162;struct Cyc_Absyn_Tuniondecl
_tmp163;struct _tuple0*_tmp164;int _tmp165;struct Cyc_Absyn_Tunionfield*_tmp166;
struct Cyc_Absyn_Tunionfield _tmp167;struct _tuple0*_tmp168;_LL104: if((_tmp15D.UnknownTunionfield).tag
!= 0)goto _LL106;_tmp15E=(_tmp15D.UnknownTunionfield).f1;_tmp15F=_tmp15E.tunion_name;
_tmp160=_tmp15E.field_name;_tmp161=_tmp15E.is_xtunion;_LL105: _tmp164=_tmp15F;
_tmp165=_tmp161;_tmp168=_tmp160;goto _LL107;_LL106: if((_tmp15D.KnownTunionfield).tag
!= 1)goto _LL103;_tmp162=(_tmp15D.KnownTunionfield).f1;_tmp163=*_tmp162;_tmp164=
_tmp163.name;_tmp165=_tmp163.is_xtunion;_tmp166=(_tmp15D.KnownTunionfield).f2;
_tmp167=*_tmp166;_tmp168=_tmp167.name;_LL107: {struct Cyc_PP_Doc*_tmp169=Cyc_PP_text(
_tmp165?({const char*_tmp16C="xtunion ";_tag_dynforward(_tmp16C,sizeof(char),
_get_zero_arr_size(_tmp16C,9));}):({const char*_tmp16D="tunion ";_tag_dynforward(
_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,8));}));s=({struct Cyc_PP_Doc*
_tmp16A[4];_tmp16A[3]=Cyc_Absynpp_qvar2doc(_tmp168);_tmp16A[2]=Cyc_PP_text(({
const char*_tmp16B=".";_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(
_tmp16B,2));}));_tmp16A[1]=Cyc_Absynpp_qvar2doc(_tmp164);_tmp16A[0]=_tmp169;Cyc_PP_cat(
_tag_dynforward(_tmp16A,sizeof(struct Cyc_PP_Doc*),4));});goto _LL103;}_LL103:;}
goto _LLC2;_LLD3: if(*((int*)_tmp11D)!= 5)goto _LLD5;_tmp12A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp11D)->f1;_tmp12B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11D)->f2;_LLD4: {
struct _dynforward_ptr sns;struct _dynforward_ptr ts;{void*_tmp16E=_tmp12A;_LL109:
if((int)_tmp16E != 2)goto _LL10B;_LL10A: goto _LL10C;_LL10B: if((int)_tmp16E != 0)goto
_LL10D;_LL10C: sns=({const char*_tmp16F="";_tag_dynforward(_tmp16F,sizeof(char),
_get_zero_arr_size(_tmp16F,1));});goto _LL108;_LL10D: if((int)_tmp16E != 1)goto
_LL108;_LL10E: sns=({const char*_tmp170="unsigned ";_tag_dynforward(_tmp170,
sizeof(char),_get_zero_arr_size(_tmp170,10));});goto _LL108;_LL108:;}{void*
_tmp171=_tmp12B;_LL110: if((int)_tmp171 != 0)goto _LL112;_LL111:{void*_tmp172=
_tmp12A;_LL11B: if((int)_tmp172 != 2)goto _LL11D;_LL11C: sns=({const char*_tmp173="";
_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,1));});goto
_LL11A;_LL11D: if((int)_tmp172 != 0)goto _LL11F;_LL11E: sns=({const char*_tmp174="signed ";
_tag_dynforward(_tmp174,sizeof(char),_get_zero_arr_size(_tmp174,8));});goto
_LL11A;_LL11F: if((int)_tmp172 != 1)goto _LL11A;_LL120: sns=({const char*_tmp175="unsigned ";
_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,10));});goto
_LL11A;_LL11A:;}ts=({const char*_tmp176="char";_tag_dynforward(_tmp176,sizeof(
char),_get_zero_arr_size(_tmp176,5));});goto _LL10F;_LL112: if((int)_tmp171 != 1)
goto _LL114;_LL113: ts=({const char*_tmp177="short";_tag_dynforward(_tmp177,sizeof(
char),_get_zero_arr_size(_tmp177,6));});goto _LL10F;_LL114: if((int)_tmp171 != 2)
goto _LL116;_LL115: ts=({const char*_tmp178="int";_tag_dynforward(_tmp178,sizeof(
char),_get_zero_arr_size(_tmp178,4));});goto _LL10F;_LL116: if((int)_tmp171 != 3)
goto _LL118;_LL117: ts=({const char*_tmp179="long";_tag_dynforward(_tmp179,sizeof(
char),_get_zero_arr_size(_tmp179,5));});goto _LL10F;_LL118: if((int)_tmp171 != 4)
goto _LL10F;_LL119: ts=Cyc_Absynpp_to_VC?({const char*_tmp17A="__int64";
_tag_dynforward(_tmp17A,sizeof(char),_get_zero_arr_size(_tmp17A,8));}):({const
char*_tmp17B="long long";_tag_dynforward(_tmp17B,sizeof(char),_get_zero_arr_size(
_tmp17B,10));});goto _LL10F;_LL10F:;}s=Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp17F;_tmp17F.tag=0;_tmp17F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)ts);{struct Cyc_String_pa_struct _tmp17E;
_tmp17E.tag=0;_tmp17E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)sns);{
void*_tmp17C[2]={& _tmp17E,& _tmp17F};Cyc_aprintf(({const char*_tmp17D="%s%s";
_tag_dynforward(_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,5));}),
_tag_dynforward(_tmp17C,sizeof(void*),2));}}}));goto _LLC2;}_LLD5: if((int)_tmp11D
!= 1)goto _LLD7;_LLD6: s=Cyc_PP_text(({const char*_tmp180="float";_tag_dynforward(
_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,6));}));goto _LLC2;_LLD7: if(
_tmp11D <= (void*)4)goto _LLEF;if(*((int*)_tmp11D)!= 6)goto _LLD9;_tmp12C=((struct
Cyc_Absyn_DoubleType_struct*)_tmp11D)->f1;_LLD8: if(_tmp12C)s=Cyc_PP_text(({const
char*_tmp181="long double";_tag_dynforward(_tmp181,sizeof(char),
_get_zero_arr_size(_tmp181,12));}));else{s=Cyc_PP_text(({const char*_tmp182="double";
_tag_dynforward(_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,7));}));}goto
_LLC2;_LLD9: if(*((int*)_tmp11D)!= 9)goto _LLDB;_tmp12D=((struct Cyc_Absyn_TupleType_struct*)
_tmp11D)->f1;_LLDA: s=({struct Cyc_PP_Doc*_tmp183[2];_tmp183[1]=Cyc_Absynpp_args2doc(
_tmp12D);_tmp183[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp183,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLC2;_LLDB: if(*((int*)_tmp11D)!= 10)goto
_LLDD;_tmp12E=((struct Cyc_Absyn_AggrType_struct*)_tmp11D)->f1;_tmp12F=_tmp12E.aggr_info;
_tmp130=_tmp12E.targs;_LLDC: {void*_tmp185;struct _tuple0*_tmp186;struct _tuple3
_tmp184=Cyc_Absyn_aggr_kinded_name(_tmp12F);_tmp185=_tmp184.f1;_tmp186=_tmp184.f2;
s=({struct Cyc_PP_Doc*_tmp187[3];_tmp187[2]=Cyc_Absynpp_tps2doc(_tmp130);_tmp187[
1]=Cyc_Absynpp_qvar2doc(_tmp186);_tmp187[0]=Cyc_Absynpp_aggr_kind2doc(_tmp185);
Cyc_PP_cat(_tag_dynforward(_tmp187,sizeof(struct Cyc_PP_Doc*),3));});goto _LLC2;}
_LLDD: if(*((int*)_tmp11D)!= 11)goto _LLDF;_tmp131=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11D)->f1;_tmp132=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11D)->f2;_LLDE: s=({
struct Cyc_PP_Doc*_tmp188[4];_tmp188[3]=Cyc_Absynpp_rb();_tmp188[2]=Cyc_PP_nest(2,
Cyc_Absynpp_aggrfields2doc(_tmp132));_tmp188[1]=Cyc_Absynpp_lb();_tmp188[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp131);Cyc_PP_cat(_tag_dynforward(_tmp188,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLC2;_LLDF: if(*((int*)_tmp11D)!= 13)goto _LLE1;_tmp133=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11D)->f1;_LLE0: s=({struct Cyc_PP_Doc*_tmp189[4];_tmp189[3]=Cyc_Absynpp_rb();
_tmp189[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp133));_tmp189[1]=Cyc_Absynpp_lb();
_tmp189[0]=Cyc_PP_text(({const char*_tmp18A="enum ";_tag_dynforward(_tmp18A,
sizeof(char),_get_zero_arr_size(_tmp18A,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp189,sizeof(struct Cyc_PP_Doc*),4));});goto _LLC2;_LLE1: if(*((int*)_tmp11D)!= 
12)goto _LLE3;_tmp134=((struct Cyc_Absyn_EnumType_struct*)_tmp11D)->f1;_LLE2: s=({
struct Cyc_PP_Doc*_tmp18B[2];_tmp18B[1]=Cyc_Absynpp_qvar2doc(_tmp134);_tmp18B[0]=
Cyc_PP_text(({const char*_tmp18C="enum ";_tag_dynforward(_tmp18C,sizeof(char),
_get_zero_arr_size(_tmp18C,6));}));Cyc_PP_cat(_tag_dynforward(_tmp18B,sizeof(
struct Cyc_PP_Doc*),2));});goto _LLC2;_LLE3: if(*((int*)_tmp11D)!= 14)goto _LLE5;
_tmp135=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11D)->f1;_LLE4: s=({
struct Cyc_PP_Doc*_tmp18D[3];_tmp18D[2]=Cyc_PP_text(({const char*_tmp18F=">";
_tag_dynforward(_tmp18F,sizeof(char),_get_zero_arr_size(_tmp18F,2));}));_tmp18D[
1]=Cyc_Absynpp_typ2doc(_tmp135);_tmp18D[0]=Cyc_PP_text(({const char*_tmp18E="sizeof_t<";
_tag_dynforward(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,10));}));Cyc_PP_cat(
_tag_dynforward(_tmp18D,sizeof(struct Cyc_PP_Doc*),3));});goto _LLC2;_LLE5: if(*((
int*)_tmp11D)!= 17)goto _LLE7;_tmp136=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11D)->f1;_tmp137=((struct Cyc_Absyn_TypedefType_struct*)_tmp11D)->f2;_tmp138=((
struct Cyc_Absyn_TypedefType_struct*)_tmp11D)->f3;_LLE6: s=({struct Cyc_PP_Doc*
_tmp190[2];_tmp190[1]=Cyc_Absynpp_tps2doc(_tmp137);_tmp190[0]=Cyc_Absynpp_qvar2doc(
_tmp136);Cyc_PP_cat(_tag_dynforward(_tmp190,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLC2;_LLE7: if(*((int*)_tmp11D)!= 15)goto _LLE9;_tmp139=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp11D)->f1;_LLE8: s=({struct Cyc_PP_Doc*_tmp191[3];_tmp191[2]=Cyc_PP_text(({
const char*_tmp193=">";_tag_dynforward(_tmp193,sizeof(char),_get_zero_arr_size(
_tmp193,2));}));_tmp191[1]=Cyc_Absynpp_rgn2doc(_tmp139);_tmp191[0]=Cyc_PP_text(({
const char*_tmp192="region_t<";_tag_dynforward(_tmp192,sizeof(char),
_get_zero_arr_size(_tmp192,10));}));Cyc_PP_cat(_tag_dynforward(_tmp191,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLC2;_LLE9: if(*((int*)_tmp11D)!= 16)goto _LLEB;
_tmp13A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11D)->f1;_tmp13B=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp11D)->f2;_LLEA: s=({struct Cyc_PP_Doc*
_tmp194[5];_tmp194[4]=Cyc_PP_text(({const char*_tmp197=">";_tag_dynforward(
_tmp197,sizeof(char),_get_zero_arr_size(_tmp197,2));}));_tmp194[3]=Cyc_Absynpp_rgn2doc(
_tmp13B);_tmp194[2]=Cyc_PP_text(({const char*_tmp196=",";_tag_dynforward(_tmp196,
sizeof(char),_get_zero_arr_size(_tmp196,2));}));_tmp194[1]=Cyc_Absynpp_rgn2doc(
_tmp13A);_tmp194[0]=Cyc_PP_text(({const char*_tmp195="dynregion_t<";
_tag_dynforward(_tmp195,sizeof(char),_get_zero_arr_size(_tmp195,13));}));Cyc_PP_cat(
_tag_dynforward(_tmp194,sizeof(struct Cyc_PP_Doc*),5));});goto _LLC2;_LLEB: if(*((
int*)_tmp11D)!= 19)goto _LLED;_tmp13C=((struct Cyc_Absyn_TypeInt_struct*)_tmp11D)->f1;
_LLEC: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp19A;
_tmp19A.tag=1;_tmp19A.f1=(unsigned long)_tmp13C;{void*_tmp198[1]={& _tmp19A};Cyc_aprintf(({
const char*_tmp199="`%d";_tag_dynforward(_tmp199,sizeof(char),_get_zero_arr_size(
_tmp199,4));}),_tag_dynforward(_tmp198,sizeof(void*),1));}}));goto _LLC2;_LLED:
if(*((int*)_tmp11D)!= 18)goto _LLEF;_tmp13D=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp11D)->f1;_LLEE: s=({struct Cyc_PP_Doc*_tmp19B[3];_tmp19B[2]=Cyc_PP_text(({
const char*_tmp19D=">";_tag_dynforward(_tmp19D,sizeof(char),_get_zero_arr_size(
_tmp19D,2));}));_tmp19B[1]=Cyc_Absynpp_typ2doc(_tmp13D);_tmp19B[0]=Cyc_PP_text(({
const char*_tmp19C="tag_t<";_tag_dynforward(_tmp19C,sizeof(char),
_get_zero_arr_size(_tmp19C,7));}));Cyc_PP_cat(_tag_dynforward(_tmp19B,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLC2;_LLEF: if((int)_tmp11D != 3)goto _LLF1;_LLF0:
goto _LLF2;_LLF1: if((int)_tmp11D != 2)goto _LLF3;_LLF2: s=Cyc_Absynpp_rgn2doc(t);
goto _LLC2;_LLF3: if(_tmp11D <= (void*)4)goto _LLF5;if(*((int*)_tmp11D)!= 22)goto
_LLF5;_tmp13E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11D)->f1;_LLF4: s=({
struct Cyc_PP_Doc*_tmp19E[3];_tmp19E[2]=Cyc_PP_text(({const char*_tmp1A0=")";
_tag_dynforward(_tmp1A0,sizeof(char),_get_zero_arr_size(_tmp1A0,2));}));_tmp19E[
1]=Cyc_Absynpp_typ2doc(_tmp13E);_tmp19E[0]=Cyc_PP_text(({const char*_tmp19F="regions(";
_tag_dynforward(_tmp19F,sizeof(char),_get_zero_arr_size(_tmp19F,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp19E,sizeof(struct Cyc_PP_Doc*),3));});goto _LLC2;_LLF5: if(
_tmp11D <= (void*)4)goto _LLF7;if(*((int*)_tmp11D)!= 20)goto _LLF7;_LLF6: goto _LLF8;
_LLF7: if(_tmp11D <= (void*)4)goto _LLC2;if(*((int*)_tmp11D)!= 21)goto _LLC2;_LLF8: s=
Cyc_Absynpp_eff2doc(t);goto _LLC2;_LLC2:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dynforward_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*
cmp){struct _tuple5 _tmp1A2;void*_tmp1A3;void*_tmp1A4;struct _tuple5*_tmp1A1=cmp;
_tmp1A2=*_tmp1A1;_tmp1A3=_tmp1A2.f1;_tmp1A4=_tmp1A2.f2;return({struct Cyc_PP_Doc*
_tmp1A5[3];_tmp1A5[2]=Cyc_Absynpp_rgn2doc(_tmp1A4);_tmp1A5[1]=Cyc_PP_text(({
const char*_tmp1A6=" > ";_tag_dynforward(_tmp1A6,sizeof(char),_get_zero_arr_size(
_tmp1A6,4));}));_tmp1A5[0]=Cyc_Absynpp_rgn2doc(_tmp1A3);Cyc_PP_cat(
_tag_dynforward(_tmp1A5,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){return Cyc_PP_group(({const char*_tmp1A7="";
_tag_dynforward(_tmp1A7,sizeof(char),_get_zero_arr_size(_tmp1A7,1));}),({const
char*_tmp1A8="";_tag_dynforward(_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,
1));}),({const char*_tmp1A9=",";_tag_dynforward(_tmp1A9,sizeof(char),
_get_zero_arr_size(_tmp1A9,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*dopt=(*
t).f1 == 0?0:({struct Cyc_Core_Opt*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->v=
Cyc_PP_text(*((struct _dynforward_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v));
_tmp1AA;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1AB=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs)_tmp1AB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp1AB,({struct Cyc_List_List*_tmp1AC=
_cycalloc(sizeof(*_tmp1AC));_tmp1AC->hd=Cyc_PP_text(({const char*_tmp1AD="...";
_tag_dynforward(_tmp1AD,sizeof(char),_get_zero_arr_size(_tmp1AD,4));}));_tmp1AC->tl=
0;_tmp1AC;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*_tmp1AE=({struct Cyc_PP_Doc*
_tmp1B0[3];_tmp1B0[2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmp1B4=_cycalloc(
sizeof(*_tmp1B4));_tmp1B4->f1=cyc_varargs->name;_tmp1B4->f2=cyc_varargs->tq;
_tmp1B4->f3=(void*)cyc_varargs->type;_tmp1B4;}));_tmp1B0[1]=cyc_varargs->inject?
Cyc_PP_text(({const char*_tmp1B2=" inject ";_tag_dynforward(_tmp1B2,sizeof(char),
_get_zero_arr_size(_tmp1B2,9));})): Cyc_PP_text(({const char*_tmp1B3=" ";
_tag_dynforward(_tmp1B3,sizeof(char),_get_zero_arr_size(_tmp1B3,2));}));_tmp1B0[
0]=Cyc_PP_text(({const char*_tmp1B1="...";_tag_dynforward(_tmp1B1,sizeof(char),
_get_zero_arr_size(_tmp1B1,4));}));Cyc_PP_cat(_tag_dynforward(_tmp1B0,sizeof(
struct Cyc_PP_Doc*),3));});_tmp1AB=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1AB,({struct Cyc_List_List*_tmp1AF=
_cycalloc(sizeof(*_tmp1AF));_tmp1AF->hd=_tmp1AE;_tmp1AF->tl=0;_tmp1AF;}));}}{
struct Cyc_PP_Doc*_tmp1B5=Cyc_PP_group(({const char*_tmp1BD="";_tag_dynforward(
_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,1));}),({const char*_tmp1BE="";
_tag_dynforward(_tmp1BE,sizeof(char),_get_zero_arr_size(_tmp1BE,1));}),({const
char*_tmp1BF=",";_tag_dynforward(_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,
2));}),_tmp1AB);if(effopt != 0  && Cyc_Absynpp_print_all_effects)_tmp1B5=({struct
Cyc_PP_Doc*_tmp1B6[3];_tmp1B6[2]=Cyc_Absynpp_eff2doc((void*)effopt->v);_tmp1B6[1]=
Cyc_PP_text(({const char*_tmp1B7=";";_tag_dynforward(_tmp1B7,sizeof(char),
_get_zero_arr_size(_tmp1B7,2));}));_tmp1B6[0]=_tmp1B5;Cyc_PP_cat(_tag_dynforward(
_tmp1B6,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)_tmp1B5=({struct Cyc_PP_Doc*
_tmp1B8[3];_tmp1B8[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1B8[1]=Cyc_PP_text(({
const char*_tmp1B9=":";_tag_dynforward(_tmp1B9,sizeof(char),_get_zero_arr_size(
_tmp1B9,2));}));_tmp1B8[0]=_tmp1B5;Cyc_PP_cat(_tag_dynforward(_tmp1B8,sizeof(
struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*_tmp1BA[3];_tmp1BA[2]=Cyc_PP_text(({
const char*_tmp1BC=")";_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size(
_tmp1BC,2));}));_tmp1BA[1]=_tmp1B5;_tmp1BA[0]=Cyc_PP_text(({const char*_tmp1BB="(";
_tag_dynforward(_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){return({struct _tuple1*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));
_tmp1C0->f1=({struct Cyc_Core_Opt*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->v=(*
arg).f1;_tmp1C1;});_tmp1C0->f2=(*arg).f2;_tmp1C0->f3=(*arg).f3;_tmp1C0;});}
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dynforward_ptr*v){return Cyc_PP_text(*
v);}struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1C2=0;int match;{union Cyc_Absyn_Nmspace_union _tmp1C3=(*q).f1;struct Cyc_List_List*
_tmp1C4;struct Cyc_List_List*_tmp1C5;_LL122: if((_tmp1C3.Loc_n).tag != 0)goto _LL124;
_LL123: _tmp1C4=0;goto _LL125;_LL124: if((_tmp1C3.Rel_n).tag != 1)goto _LL126;_tmp1C4=(
_tmp1C3.Rel_n).f1;_LL125: match=0;_tmp1C2=_tmp1C4;goto _LL121;_LL126: if((_tmp1C3.Abs_n).tag
!= 2)goto _LL121;_tmp1C5=(_tmp1C3.Abs_n).f1;_LL127: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C5,Cyc_Absynpp_curr_namespace);
_tmp1C2=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*
_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1C6->tl=_tmp1C5;_tmp1C6;}): _tmp1C5;goto _LL121;_LL121:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dynforward_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C2,({struct Cyc_List_List*_tmp1C7=
_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=(*q).f2;_tmp1C7->tl=0;_tmp1C7;})),({
const char*_tmp1C8="_";_tag_dynforward(_tmp1C8,sizeof(char),_get_zero_arr_size(
_tmp1C8,2));}));else{if(match)return*(*q).f2;else{return(struct _dynforward_ptr)
Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1C2,({struct Cyc_List_List*_tmp1C9=_cycalloc(sizeof(*
_tmp1C9));_tmp1C9->hd=(*q).f2;_tmp1C9->tl=0;_tmp1C9;})),({const char*_tmp1CA="::";
_tag_dynforward(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,3));}));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(
q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct
_dynforward_ptr _tmp1CB=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((
struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)Cyc_strconcat(({const
char*_tmp1CC="\\textbf{";_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size(
_tmp1CC,9));}),(struct _dynforward_ptr)_tmp1CB),({const char*_tmp1CD="}";
_tag_dynforward(_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,2));})),(int)Cyc_strlen((
struct _dynforward_ptr)_tmp1CB));else{return Cyc_PP_text(_tmp1CB);}}struct
_dynforward_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace_union
_tmp1CE=(*v).f1;struct Cyc_List_List*_tmp1CF;struct Cyc_List_List*_tmp1D0;_LL129:
if((_tmp1CE.Loc_n).tag != 0)goto _LL12B;_LL12A: goto _LL12C;_LL12B: if((_tmp1CE.Rel_n).tag
!= 1)goto _LL12D;_tmp1CF=(_tmp1CE.Rel_n).f1;if(_tmp1CF != 0)goto _LL12D;_LL12C:
return*(*v).f2;_LL12D: if((_tmp1CE.Abs_n).tag != 2)goto _LL12F;_tmp1D0=(_tmp1CE.Abs_n).f1;
_LL12E: if(((int(*)(int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp1D0,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL130;}_LL12F:;
_LL130: return(struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D1="/* bad namespace : */ ";
_tag_dynforward(_tmp1D1,sizeof(char),_get_zero_arr_size(_tmp1D1,23));}),(struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(v));_LL128:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dynforward_ptr _tmp1D2=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp1D3="\\textbf{";
_tag_dynforward(_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,9));}),(struct
_dynforward_ptr)_tmp1D2),({const char*_tmp1D4="}";_tag_dynforward(_tmp1D4,sizeof(
char),_get_zero_arr_size(_tmp1D4,2));})),(int)Cyc_strlen((struct _dynforward_ptr)
_tmp1D2));else{return Cyc_PP_text(_tmp1D2);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp1D5=(void*)e->r;void*_tmp1D6;struct Cyc_Absyn_Exp*
_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;_LL132: if(*((int*)_tmp1D5)!= 0)goto _LL134;
_LL133: goto _LL135;_LL134: if(*((int*)_tmp1D5)!= 1)goto _LL136;_LL135: goto _LL137;
_LL136: if(*((int*)_tmp1D5)!= 2)goto _LL138;_LL137: return 10000;_LL138: if(*((int*)
_tmp1D5)!= 3)goto _LL13A;_tmp1D6=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D5)->f1;_LL139: {void*_tmp1D9=_tmp1D6;_LL181: if((int)_tmp1D9 != 0)goto _LL183;
_LL182: return 100;_LL183: if((int)_tmp1D9 != 1)goto _LL185;_LL184: return 110;_LL185:
if((int)_tmp1D9 != 2)goto _LL187;_LL186: return 100;_LL187: if((int)_tmp1D9 != 3)goto
_LL189;_LL188: goto _LL18A;_LL189: if((int)_tmp1D9 != 4)goto _LL18B;_LL18A: return 110;
_LL18B: if((int)_tmp1D9 != 5)goto _LL18D;_LL18C: goto _LL18E;_LL18D: if((int)_tmp1D9 != 
6)goto _LL18F;_LL18E: return 70;_LL18F: if((int)_tmp1D9 != 7)goto _LL191;_LL190: goto
_LL192;_LL191: if((int)_tmp1D9 != 8)goto _LL193;_LL192: goto _LL194;_LL193: if((int)
_tmp1D9 != 9)goto _LL195;_LL194: goto _LL196;_LL195: if((int)_tmp1D9 != 10)goto _LL197;
_LL196: return 80;_LL197: if((int)_tmp1D9 != 11)goto _LL199;_LL198: goto _LL19A;_LL199:
if((int)_tmp1D9 != 12)goto _LL19B;_LL19A: return 130;_LL19B: if((int)_tmp1D9 != 13)
goto _LL19D;_LL19C: return 60;_LL19D: if((int)_tmp1D9 != 14)goto _LL19F;_LL19E: return
40;_LL19F: if((int)_tmp1D9 != 15)goto _LL1A1;_LL1A0: return 50;_LL1A1: if((int)_tmp1D9
!= 16)goto _LL1A3;_LL1A2: return 90;_LL1A3: if((int)_tmp1D9 != 17)goto _LL1A5;_LL1A4:
return 80;_LL1A5: if((int)_tmp1D9 != 18)goto _LL1A7;_LL1A6: return 80;_LL1A7: if((int)
_tmp1D9 != 19)goto _LL180;_LL1A8: return 140;_LL180:;}_LL13A: if(*((int*)_tmp1D5)!= 4)
goto _LL13C;_LL13B: return 20;_LL13C: if(*((int*)_tmp1D5)!= 5)goto _LL13E;_LL13D:
return 130;_LL13E: if(*((int*)_tmp1D5)!= 6)goto _LL140;_LL13F: return 30;_LL140: if(*((
int*)_tmp1D5)!= 7)goto _LL142;_LL141: return 35;_LL142: if(*((int*)_tmp1D5)!= 8)goto
_LL144;_LL143: return 30;_LL144: if(*((int*)_tmp1D5)!= 9)goto _LL146;_LL145: return 10;
_LL146: if(*((int*)_tmp1D5)!= 10)goto _LL148;_LL147: goto _LL149;_LL148: if(*((int*)
_tmp1D5)!= 11)goto _LL14A;_LL149: return 140;_LL14A: if(*((int*)_tmp1D5)!= 12)goto
_LL14C;_LL14B: return 130;_LL14C: if(*((int*)_tmp1D5)!= 13)goto _LL14E;_tmp1D7=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D5)->f1;_LL14D: return Cyc_Absynpp_exp_prec(
_tmp1D7);_LL14E: if(*((int*)_tmp1D5)!= 14)goto _LL150;_tmp1D8=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D5)->f1;_LL14F: return Cyc_Absynpp_exp_prec(_tmp1D8);_LL150: if(*((int*)
_tmp1D5)!= 15)goto _LL152;_LL151: return 120;_LL152: if(*((int*)_tmp1D5)!= 17)goto
_LL154;_LL153: return 15;_LL154: if(*((int*)_tmp1D5)!= 16)goto _LL156;_LL155: goto
_LL157;_LL156: if(*((int*)_tmp1D5)!= 18)goto _LL158;_LL157: goto _LL159;_LL158: if(*((
int*)_tmp1D5)!= 19)goto _LL15A;_LL159: goto _LL15B;_LL15A: if(*((int*)_tmp1D5)!= 20)
goto _LL15C;_LL15B: goto _LL15D;_LL15C: if(*((int*)_tmp1D5)!= 21)goto _LL15E;_LL15D:
goto _LL15F;_LL15E: if(*((int*)_tmp1D5)!= 22)goto _LL160;_LL15F: return 130;_LL160:
if(*((int*)_tmp1D5)!= 23)goto _LL162;_LL161: goto _LL163;_LL162: if(*((int*)_tmp1D5)
!= 24)goto _LL164;_LL163: goto _LL165;_LL164: if(*((int*)_tmp1D5)!= 25)goto _LL166;
_LL165: return 140;_LL166: if(*((int*)_tmp1D5)!= 26)goto _LL168;_LL167: return 150;
_LL168: if(*((int*)_tmp1D5)!= 27)goto _LL16A;_LL169: goto _LL16B;_LL16A: if(*((int*)
_tmp1D5)!= 28)goto _LL16C;_LL16B: goto _LL16D;_LL16C: if(*((int*)_tmp1D5)!= 29)goto
_LL16E;_LL16D: goto _LL16F;_LL16E: if(*((int*)_tmp1D5)!= 30)goto _LL170;_LL16F: goto
_LL171;_LL170: if(*((int*)_tmp1D5)!= 31)goto _LL172;_LL171: goto _LL173;_LL172: if(*((
int*)_tmp1D5)!= 32)goto _LL174;_LL173: goto _LL175;_LL174: if(*((int*)_tmp1D5)!= 33)
goto _LL176;_LL175: goto _LL177;_LL176: if(*((int*)_tmp1D5)!= 34)goto _LL178;_LL177:
goto _LL179;_LL178: if(*((int*)_tmp1D5)!= 35)goto _LL17A;_LL179: goto _LL17B;_LL17A:
if(*((int*)_tmp1D5)!= 36)goto _LL17C;_LL17B: goto _LL17D;_LL17C: if(*((int*)_tmp1D5)
!= 37)goto _LL17E;_LL17D: return 140;_LL17E: if(*((int*)_tmp1D5)!= 38)goto _LL131;
_LL17F: return 10000;_LL131:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*
e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp1DA=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp1DB;struct _tuple0*
_tmp1DC;struct _tuple0*_tmp1DD;void*_tmp1DE;struct Cyc_List_List*_tmp1DF;struct Cyc_Absyn_Exp*
_tmp1E0;struct Cyc_Core_Opt*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_Absyn_Exp*
_tmp1E3;void*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;
struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*
_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*
_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_List_List*
_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Exp*
_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;void*_tmp1F5;
struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_Absyn_Exp*
_tmp1F8;struct Cyc_Absyn_Exp*_tmp1F9;void*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FB;
void*_tmp1FC;void*_tmp1FD;struct _dynforward_ptr*_tmp1FE;void*_tmp1FF;void*
_tmp200;unsigned int _tmp201;struct Cyc_List_List*_tmp202;void*_tmp203;struct Cyc_Absyn_Exp*
_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct _dynforward_ptr*_tmp206;struct Cyc_Absyn_Exp*
_tmp207;struct _dynforward_ptr*_tmp208;struct Cyc_Absyn_Exp*_tmp209;struct Cyc_Absyn_Exp*
_tmp20A;struct Cyc_List_List*_tmp20B;struct _tuple1*_tmp20C;struct Cyc_List_List*
_tmp20D;struct Cyc_List_List*_tmp20E;struct Cyc_Absyn_Vardecl*_tmp20F;struct Cyc_Absyn_Exp*
_tmp210;struct Cyc_Absyn_Exp*_tmp211;struct _tuple0*_tmp212;struct Cyc_List_List*
_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp215;struct Cyc_List_List*
_tmp216;struct Cyc_Absyn_Tunionfield*_tmp217;struct _tuple0*_tmp218;struct _tuple0*
_tmp219;struct Cyc_Absyn_MallocInfo _tmp21A;int _tmp21B;struct Cyc_Absyn_Exp*_tmp21C;
void**_tmp21D;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Exp*
_tmp220;struct Cyc_Core_Opt*_tmp221;struct Cyc_List_List*_tmp222;struct Cyc_Absyn_Stmt*
_tmp223;_LL1AA: if(*((int*)_tmp1DA)!= 0)goto _LL1AC;_tmp1DB=((struct Cyc_Absyn_Const_e_struct*)
_tmp1DA)->f1;_LL1AB: s=Cyc_Absynpp_cnst2doc(_tmp1DB);goto _LL1A9;_LL1AC: if(*((int*)
_tmp1DA)!= 1)goto _LL1AE;_tmp1DC=((struct Cyc_Absyn_Var_e_struct*)_tmp1DA)->f1;
_LL1AD: _tmp1DD=_tmp1DC;goto _LL1AF;_LL1AE: if(*((int*)_tmp1DA)!= 2)goto _LL1B0;
_tmp1DD=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1DA)->f1;_LL1AF: s=Cyc_Absynpp_qvar2doc(
_tmp1DD);goto _LL1A9;_LL1B0: if(*((int*)_tmp1DA)!= 3)goto _LL1B2;_tmp1DE=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp1DA)->f1;_tmp1DF=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1DA)->f2;_LL1B1: s=Cyc_Absynpp_primapp2doc(myprec,_tmp1DE,_tmp1DF);goto _LL1A9;
_LL1B2: if(*((int*)_tmp1DA)!= 4)goto _LL1B4;_tmp1E0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1DA)->f1;_tmp1E1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1DA)->f2;_tmp1E2=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1DA)->f3;_LL1B3: s=({struct Cyc_PP_Doc*
_tmp224[5];_tmp224[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E2);_tmp224[3]=Cyc_PP_text(({
const char*_tmp226="= ";_tag_dynforward(_tmp226,sizeof(char),_get_zero_arr_size(
_tmp226,3));}));_tmp224[2]=_tmp1E1 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((
void*)_tmp1E1->v);_tmp224[1]=Cyc_PP_text(({const char*_tmp225=" ";_tag_dynforward(
_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,2));}));_tmp224[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E0);Cyc_PP_cat(_tag_dynforward(_tmp224,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1A9;_LL1B4: if(*((int*)_tmp1DA)!= 5)goto _LL1B6;_tmp1E3=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1DA)->f1;_tmp1E4=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1DA)->f2;
_LL1B5: {struct Cyc_PP_Doc*_tmp227=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E3);{void*
_tmp228=_tmp1E4;_LL1FB: if((int)_tmp228 != 0)goto _LL1FD;_LL1FC: s=({struct Cyc_PP_Doc*
_tmp229[2];_tmp229[1]=_tmp227;_tmp229[0]=Cyc_PP_text(({const char*_tmp22A="++";
_tag_dynforward(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,3));}));Cyc_PP_cat(
_tag_dynforward(_tmp229,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1FA;_LL1FD: if((
int)_tmp228 != 2)goto _LL1FF;_LL1FE: s=({struct Cyc_PP_Doc*_tmp22B[2];_tmp22B[1]=
_tmp227;_tmp22B[0]=Cyc_PP_text(({const char*_tmp22C="--";_tag_dynforward(_tmp22C,
sizeof(char),_get_zero_arr_size(_tmp22C,3));}));Cyc_PP_cat(_tag_dynforward(
_tmp22B,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1FA;_LL1FF: if((int)_tmp228 != 1)
goto _LL201;_LL200: s=({struct Cyc_PP_Doc*_tmp22D[2];_tmp22D[1]=Cyc_PP_text(({const
char*_tmp22E="++";_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size(
_tmp22E,3));}));_tmp22D[0]=_tmp227;Cyc_PP_cat(_tag_dynforward(_tmp22D,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1FA;_LL201: if((int)_tmp228 != 3)goto _LL1FA;
_LL202: s=({struct Cyc_PP_Doc*_tmp22F[2];_tmp22F[1]=Cyc_PP_text(({const char*
_tmp230="--";_tag_dynforward(_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,3));}));
_tmp22F[0]=_tmp227;Cyc_PP_cat(_tag_dynforward(_tmp22F,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1FA;_LL1FA:;}goto _LL1A9;}_LL1B6: if(*((int*)_tmp1DA)!= 6)goto _LL1B8;
_tmp1E5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1DA)->f1;_tmp1E6=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1DA)->f2;_tmp1E7=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1DA)->f3;_LL1B7:
s=({struct Cyc_PP_Doc*_tmp231[5];_tmp231[4]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1E7);_tmp231[3]=Cyc_PP_text(({const char*_tmp233=" : ";_tag_dynforward(
_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,4));}));_tmp231[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1E6);_tmp231[1]=Cyc_PP_text(({const char*_tmp232=" ? ";_tag_dynforward(
_tmp232,sizeof(char),_get_zero_arr_size(_tmp232,4));}));_tmp231[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E5);Cyc_PP_cat(_tag_dynforward(_tmp231,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1A9;_LL1B8: if(*((int*)_tmp1DA)!= 7)goto _LL1BA;_tmp1E8=((struct Cyc_Absyn_And_e_struct*)
_tmp1DA)->f1;_tmp1E9=((struct Cyc_Absyn_And_e_struct*)_tmp1DA)->f2;_LL1B9: s=({
struct Cyc_PP_Doc*_tmp234[3];_tmp234[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E9);
_tmp234[1]=Cyc_PP_text(({const char*_tmp235=" && ";_tag_dynforward(_tmp235,
sizeof(char),_get_zero_arr_size(_tmp235,5));}));_tmp234[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E8);Cyc_PP_cat(_tag_dynforward(_tmp234,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A9;_LL1BA: if(*((int*)_tmp1DA)!= 8)goto _LL1BC;_tmp1EA=((struct Cyc_Absyn_Or_e_struct*)
_tmp1DA)->f1;_tmp1EB=((struct Cyc_Absyn_Or_e_struct*)_tmp1DA)->f2;_LL1BB: s=({
struct Cyc_PP_Doc*_tmp236[3];_tmp236[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EB);
_tmp236[1]=Cyc_PP_text(({const char*_tmp237=" || ";_tag_dynforward(_tmp237,
sizeof(char),_get_zero_arr_size(_tmp237,5));}));_tmp236[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EA);Cyc_PP_cat(_tag_dynforward(_tmp236,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A9;_LL1BC: if(*((int*)_tmp1DA)!= 9)goto _LL1BE;_tmp1EC=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1DA)->f1;_tmp1ED=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1DA)->f2;_LL1BD: s=({
struct Cyc_PP_Doc*_tmp238[5];_tmp238[4]=Cyc_PP_text(({const char*_tmp23B=")";
_tag_dynforward(_tmp23B,sizeof(char),_get_zero_arr_size(_tmp23B,2));}));_tmp238[
3]=Cyc_Absynpp_exp2doc(_tmp1ED);_tmp238[2]=Cyc_PP_text(({const char*_tmp23A=", ";
_tag_dynforward(_tmp23A,sizeof(char),_get_zero_arr_size(_tmp23A,3));}));_tmp238[
1]=Cyc_Absynpp_exp2doc(_tmp1EC);_tmp238[0]=Cyc_PP_text(({const char*_tmp239="(";
_tag_dynforward(_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp238,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1A9;_LL1BE: if(*((
int*)_tmp1DA)!= 10)goto _LL1C0;_tmp1EE=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1DA)->f1;_tmp1EF=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1DA)->f2;_LL1BF:
s=({struct Cyc_PP_Doc*_tmp23C[4];_tmp23C[3]=Cyc_PP_text(({const char*_tmp23E=")";
_tag_dynforward(_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,2));}));_tmp23C[
2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1EF);_tmp23C[1]=Cyc_PP_text(({const char*
_tmp23D="(";_tag_dynforward(_tmp23D,sizeof(char),_get_zero_arr_size(_tmp23D,2));}));
_tmp23C[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EE);Cyc_PP_cat(_tag_dynforward(
_tmp23C,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A9;_LL1C0: if(*((int*)_tmp1DA)!= 
11)goto _LL1C2;_tmp1F0=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1DA)->f1;_tmp1F1=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp1DA)->f2;_LL1C1: s=({struct Cyc_PP_Doc*
_tmp23F[4];_tmp23F[3]=Cyc_PP_text(({const char*_tmp241=")";_tag_dynforward(
_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,2));}));_tmp23F[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp1F1);_tmp23F[1]=Cyc_PP_text(({const char*_tmp240="(";_tag_dynforward(
_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,2));}));_tmp23F[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F0);Cyc_PP_cat(_tag_dynforward(_tmp23F,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1A9;_LL1C2: if(*((int*)_tmp1DA)!= 12)goto _LL1C4;_tmp1F2=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1DA)->f1;_LL1C3: s=({struct Cyc_PP_Doc*_tmp242[2];_tmp242[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F2);_tmp242[0]=Cyc_PP_text(({const char*_tmp243="throw ";
_tag_dynforward(_tmp243,sizeof(char),_get_zero_arr_size(_tmp243,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp242,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A9;_LL1C4: if(*((
int*)_tmp1DA)!= 13)goto _LL1C6;_tmp1F3=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1DA)->f1;_LL1C5: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F3);goto _LL1A9;_LL1C6:
if(*((int*)_tmp1DA)!= 14)goto _LL1C8;_tmp1F4=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1DA)->f1;_LL1C7: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F4);goto _LL1A9;_LL1C8:
if(*((int*)_tmp1DA)!= 15)goto _LL1CA;_tmp1F5=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1DA)->f1;_tmp1F6=((struct Cyc_Absyn_Cast_e_struct*)_tmp1DA)->f2;_LL1C9: s=({
struct Cyc_PP_Doc*_tmp244[4];_tmp244[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F6);
_tmp244[2]=Cyc_PP_text(({const char*_tmp246=")";_tag_dynforward(_tmp246,sizeof(
char),_get_zero_arr_size(_tmp246,2));}));_tmp244[1]=Cyc_Absynpp_typ2doc(_tmp1F5);
_tmp244[0]=Cyc_PP_text(({const char*_tmp245="(";_tag_dynforward(_tmp245,sizeof(
char),_get_zero_arr_size(_tmp245,2));}));Cyc_PP_cat(_tag_dynforward(_tmp244,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A9;_LL1CA: if(*((int*)_tmp1DA)!= 16)goto
_LL1CC;_tmp1F7=((struct Cyc_Absyn_Address_e_struct*)_tmp1DA)->f1;_LL1CB: s=({
struct Cyc_PP_Doc*_tmp247[2];_tmp247[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F7);
_tmp247[0]=Cyc_PP_text(({const char*_tmp248="&";_tag_dynforward(_tmp248,sizeof(
char),_get_zero_arr_size(_tmp248,2));}));Cyc_PP_cat(_tag_dynforward(_tmp247,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A9;_LL1CC: if(*((int*)_tmp1DA)!= 17)goto
_LL1CE;_tmp1F8=((struct Cyc_Absyn_New_e_struct*)_tmp1DA)->f1;_tmp1F9=((struct Cyc_Absyn_New_e_struct*)
_tmp1DA)->f2;_LL1CD: if(_tmp1F8 == 0)s=({struct Cyc_PP_Doc*_tmp249[2];_tmp249[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F9);_tmp249[0]=Cyc_PP_text(({const char*
_tmp24A="new ";_tag_dynforward(_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,5));}));
Cyc_PP_cat(_tag_dynforward(_tmp249,sizeof(struct Cyc_PP_Doc*),2));});else{s=({
struct Cyc_PP_Doc*_tmp24B[4];_tmp24B[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F9);
_tmp24B[2]=Cyc_PP_text(({const char*_tmp24D=") ";_tag_dynforward(_tmp24D,sizeof(
char),_get_zero_arr_size(_tmp24D,3));}));_tmp24B[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_tmp1F8);_tmp24B[0]=Cyc_PP_text(({const char*_tmp24C="rnew(";
_tag_dynforward(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,6));}));Cyc_PP_cat(
_tag_dynforward(_tmp24B,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL1A9;_LL1CE: if(*((
int*)_tmp1DA)!= 18)goto _LL1D0;_tmp1FA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1DA)->f1;_LL1CF: s=({struct Cyc_PP_Doc*_tmp24E[3];_tmp24E[2]=Cyc_PP_text(({
const char*_tmp250=")";_tag_dynforward(_tmp250,sizeof(char),_get_zero_arr_size(
_tmp250,2));}));_tmp24E[1]=Cyc_Absynpp_typ2doc(_tmp1FA);_tmp24E[0]=Cyc_PP_text(({
const char*_tmp24F="sizeof(";_tag_dynforward(_tmp24F,sizeof(char),
_get_zero_arr_size(_tmp24F,8));}));Cyc_PP_cat(_tag_dynforward(_tmp24E,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL1A9;_LL1D0: if(*((int*)_tmp1DA)!= 19)goto _LL1D2;
_tmp1FB=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1DA)->f1;_LL1D1: s=({struct Cyc_PP_Doc*
_tmp251[3];_tmp251[2]=Cyc_PP_text(({const char*_tmp253=")";_tag_dynforward(
_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,2));}));_tmp251[1]=Cyc_Absynpp_exp2doc(
_tmp1FB);_tmp251[0]=Cyc_PP_text(({const char*_tmp252="sizeof(";_tag_dynforward(
_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp251,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A9;_LL1D2: if(*((
int*)_tmp1DA)!= 20)goto _LL1D4;_tmp1FC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1DA)->f1;_tmp1FD=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DA)->f2;
if(*((int*)_tmp1FD)!= 0)goto _LL1D4;_tmp1FE=((struct Cyc_Absyn_StructField_struct*)
_tmp1FD)->f1;_LL1D3: s=({struct Cyc_PP_Doc*_tmp254[5];_tmp254[4]=Cyc_PP_text(({
const char*_tmp257=")";_tag_dynforward(_tmp257,sizeof(char),_get_zero_arr_size(
_tmp257,2));}));_tmp254[3]=Cyc_PP_textptr(_tmp1FE);_tmp254[2]=Cyc_PP_text(({
const char*_tmp256=",";_tag_dynforward(_tmp256,sizeof(char),_get_zero_arr_size(
_tmp256,2));}));_tmp254[1]=Cyc_Absynpp_typ2doc(_tmp1FC);_tmp254[0]=Cyc_PP_text(({
const char*_tmp255="offsetof(";_tag_dynforward(_tmp255,sizeof(char),
_get_zero_arr_size(_tmp255,10));}));Cyc_PP_cat(_tag_dynforward(_tmp254,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1A9;_LL1D4: if(*((int*)_tmp1DA)!= 20)goto _LL1D6;
_tmp1FF=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DA)->f1;_tmp200=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DA)->f2;if(*((int*)_tmp200)!= 1)goto
_LL1D6;_tmp201=((struct Cyc_Absyn_TupleIndex_struct*)_tmp200)->f1;_LL1D5: s=({
struct Cyc_PP_Doc*_tmp258[5];_tmp258[4]=Cyc_PP_text(({const char*_tmp25E=")";
_tag_dynforward(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,2));}));_tmp258[
3]=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp25D;_tmp25D.tag=
1;_tmp25D.f1=(unsigned long)((int)_tmp201);{void*_tmp25B[1]={& _tmp25D};Cyc_aprintf(({
const char*_tmp25C="%d";_tag_dynforward(_tmp25C,sizeof(char),_get_zero_arr_size(
_tmp25C,3));}),_tag_dynforward(_tmp25B,sizeof(void*),1));}}));_tmp258[2]=Cyc_PP_text(({
const char*_tmp25A=",";_tag_dynforward(_tmp25A,sizeof(char),_get_zero_arr_size(
_tmp25A,2));}));_tmp258[1]=Cyc_Absynpp_typ2doc(_tmp1FF);_tmp258[0]=Cyc_PP_text(({
const char*_tmp259="offsetof(";_tag_dynforward(_tmp259,sizeof(char),
_get_zero_arr_size(_tmp259,10));}));Cyc_PP_cat(_tag_dynforward(_tmp258,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1A9;_LL1D6: if(*((int*)_tmp1DA)!= 21)goto _LL1D8;
_tmp202=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1DA)->f1;_tmp203=(void*)((struct
Cyc_Absyn_Gentyp_e_struct*)_tmp1DA)->f2;_LL1D7: s=({struct Cyc_PP_Doc*_tmp25F[4];
_tmp25F[3]=Cyc_PP_text(({const char*_tmp261=")";_tag_dynforward(_tmp261,sizeof(
char),_get_zero_arr_size(_tmp261,2));}));_tmp25F[2]=Cyc_Absynpp_typ2doc(_tmp203);
_tmp25F[1]=Cyc_Absynpp_tvars2doc(_tmp202);_tmp25F[0]=Cyc_PP_text(({const char*
_tmp260="__gen(";_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,
7));}));Cyc_PP_cat(_tag_dynforward(_tmp25F,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL1A9;_LL1D8: if(*((int*)_tmp1DA)!= 22)goto _LL1DA;_tmp204=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1DA)->f1;_LL1D9: s=({struct Cyc_PP_Doc*_tmp262[2];_tmp262[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp204);_tmp262[0]=Cyc_PP_text(({const char*_tmp263="*";_tag_dynforward(
_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp262,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A9;_LL1DA: if(*((
int*)_tmp1DA)!= 23)goto _LL1DC;_tmp205=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1DA)->f1;_tmp206=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1DA)->f2;_LL1DB:
s=({struct Cyc_PP_Doc*_tmp264[3];_tmp264[2]=Cyc_PP_textptr(_tmp206);_tmp264[1]=
Cyc_PP_text(({const char*_tmp265=".";_tag_dynforward(_tmp265,sizeof(char),
_get_zero_arr_size(_tmp265,2));}));_tmp264[0]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp205);Cyc_PP_cat(_tag_dynforward(_tmp264,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1A9;_LL1DC: if(*((int*)_tmp1DA)!= 24)goto _LL1DE;_tmp207=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1DA)->f1;_tmp208=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1DA)->f2;_LL1DD: s=({
struct Cyc_PP_Doc*_tmp266[3];_tmp266[2]=Cyc_PP_textptr(_tmp208);_tmp266[1]=Cyc_PP_text(({
const char*_tmp267="->";_tag_dynforward(_tmp267,sizeof(char),_get_zero_arr_size(
_tmp267,3));}));_tmp266[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp207);Cyc_PP_cat(
_tag_dynforward(_tmp266,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A9;_LL1DE: if(*((
int*)_tmp1DA)!= 25)goto _LL1E0;_tmp209=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1DA)->f1;_tmp20A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1DA)->f2;_LL1DF: s=({
struct Cyc_PP_Doc*_tmp268[4];_tmp268[3]=Cyc_PP_text(({const char*_tmp26A="]";
_tag_dynforward(_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,2));}));_tmp268[
2]=Cyc_Absynpp_exp2doc(_tmp20A);_tmp268[1]=Cyc_PP_text(({const char*_tmp269="[";
_tag_dynforward(_tmp269,sizeof(char),_get_zero_arr_size(_tmp269,2));}));_tmp268[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp209);Cyc_PP_cat(_tag_dynforward(_tmp268,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A9;_LL1E0: if(*((int*)_tmp1DA)!= 26)goto
_LL1E2;_tmp20B=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1DA)->f1;_LL1E1: s=({struct
Cyc_PP_Doc*_tmp26B[4];_tmp26B[3]=Cyc_PP_text(({const char*_tmp26D=")";
_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,2));}));_tmp26B[
2]=Cyc_Absynpp_exps2doc_prec(20,_tmp20B);_tmp26B[1]=Cyc_PP_text(({const char*
_tmp26C="(";_tag_dynforward(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,2));}));
_tmp26B[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp26B,sizeof(struct
Cyc_PP_Doc*),4));});goto _LL1A9;_LL1E2: if(*((int*)_tmp1DA)!= 27)goto _LL1E4;
_tmp20C=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1DA)->f1;_tmp20D=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1DA)->f2;_LL1E3: s=({struct Cyc_PP_Doc*_tmp26E[4];_tmp26E[3]=Cyc_Absynpp_group_braces(({
const char*_tmp271=",";_tag_dynforward(_tmp271,sizeof(char),_get_zero_arr_size(
_tmp271,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp20D));_tmp26E[2]=Cyc_PP_text(({
const char*_tmp270=")";_tag_dynforward(_tmp270,sizeof(char),_get_zero_arr_size(
_tmp270,2));}));_tmp26E[1]=Cyc_Absynpp_typ2doc((*_tmp20C).f3);_tmp26E[0]=Cyc_PP_text(({
const char*_tmp26F="(";_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size(
_tmp26F,2));}));Cyc_PP_cat(_tag_dynforward(_tmp26E,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1A9;_LL1E4: if(*((int*)_tmp1DA)!= 28)goto _LL1E6;_tmp20E=((struct Cyc_Absyn_Array_e_struct*)
_tmp1DA)->f1;_LL1E5: s=Cyc_Absynpp_group_braces(({const char*_tmp272=",";
_tag_dynforward(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp20E));goto _LL1A9;_LL1E6: if(*((int*)_tmp1DA)
!= 29)goto _LL1E8;_tmp20F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DA)->f1;
_tmp210=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DA)->f2;_tmp211=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1DA)->f3;_LL1E7: s=({struct Cyc_PP_Doc*
_tmp273[8];_tmp273[7]=Cyc_Absynpp_rb();_tmp273[6]=Cyc_Absynpp_exp2doc(_tmp211);
_tmp273[5]=Cyc_PP_text(({const char*_tmp276=" : ";_tag_dynforward(_tmp276,sizeof(
char),_get_zero_arr_size(_tmp276,4));}));_tmp273[4]=Cyc_Absynpp_exp2doc(_tmp210);
_tmp273[3]=Cyc_PP_text(({const char*_tmp275=" < ";_tag_dynforward(_tmp275,sizeof(
char),_get_zero_arr_size(_tmp275,4));}));_tmp273[2]=Cyc_PP_text(*(*_tmp20F->name).f2);
_tmp273[1]=Cyc_PP_text(({const char*_tmp274="for ";_tag_dynforward(_tmp274,
sizeof(char),_get_zero_arr_size(_tmp274,5));}));_tmp273[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dynforward(_tmp273,sizeof(struct Cyc_PP_Doc*),8));});goto _LL1A9;_LL1E8: if(*((
int*)_tmp1DA)!= 30)goto _LL1EA;_tmp212=((struct Cyc_Absyn_Struct_e_struct*)_tmp1DA)->f1;
_tmp213=((struct Cyc_Absyn_Struct_e_struct*)_tmp1DA)->f2;_tmp214=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1DA)->f3;_LL1E9: {struct Cyc_List_List*_tmp277=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp214);s=({struct Cyc_PP_Doc*_tmp278[2];_tmp278[1]=Cyc_Absynpp_group_braces(({
const char*_tmp279=",";_tag_dynforward(_tmp279,sizeof(char),_get_zero_arr_size(
_tmp279,2));}),_tmp213 != 0?({struct Cyc_List_List*_tmp27A=_cycalloc(sizeof(*
_tmp27A));_tmp27A->hd=Cyc_Absynpp_tps2doc(_tmp213);_tmp27A->tl=_tmp277;_tmp27A;}):
_tmp277);_tmp278[0]=Cyc_Absynpp_qvar2doc(_tmp212);Cyc_PP_cat(_tag_dynforward(
_tmp278,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A9;}_LL1EA: if(*((int*)_tmp1DA)
!= 31)goto _LL1EC;_tmp215=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1DA)->f2;
_LL1EB: s=Cyc_Absynpp_group_braces(({const char*_tmp27B=",";_tag_dynforward(
_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp215));goto _LL1A9;_LL1EC: if(*((int*)_tmp1DA)!= 32)goto _LL1EE;_tmp216=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp1DA)->f1;_tmp217=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1DA)->f3;_LL1ED: if(_tmp216 == 0)s=Cyc_Absynpp_qvar2doc(_tmp217->name);else{s=({
struct Cyc_PP_Doc*_tmp27C[2];_tmp27C[1]=Cyc_PP_egroup(({const char*_tmp27D="(";
_tag_dynforward(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,2));}),({const
char*_tmp27E=")";_tag_dynforward(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,
2));}),({const char*_tmp27F=",";_tag_dynforward(_tmp27F,sizeof(char),
_get_zero_arr_size(_tmp27F,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp216));_tmp27C[0]=Cyc_Absynpp_qvar2doc(_tmp217->name);Cyc_PP_cat(
_tag_dynforward(_tmp27C,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL1A9;_LL1EE: if(*((
int*)_tmp1DA)!= 33)goto _LL1F0;_tmp218=((struct Cyc_Absyn_Enum_e_struct*)_tmp1DA)->f1;
_LL1EF: s=Cyc_Absynpp_qvar2doc(_tmp218);goto _LL1A9;_LL1F0: if(*((int*)_tmp1DA)!= 
34)goto _LL1F2;_tmp219=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1DA)->f1;_LL1F1: s=
Cyc_Absynpp_qvar2doc(_tmp219);goto _LL1A9;_LL1F2: if(*((int*)_tmp1DA)!= 35)goto
_LL1F4;_tmp21A=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1DA)->f1;_tmp21B=_tmp21A.is_calloc;
_tmp21C=_tmp21A.rgn;_tmp21D=_tmp21A.elt_type;_tmp21E=_tmp21A.num_elts;_LL1F3: if(
_tmp21B){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp21D)),0);if(_tmp21C == 0)s=({struct Cyc_PP_Doc*_tmp280[5];_tmp280[4]=Cyc_PP_text(({
const char*_tmp283=")";_tag_dynforward(_tmp283,sizeof(char),_get_zero_arr_size(
_tmp283,2));}));_tmp280[3]=Cyc_Absynpp_exp2doc(st);_tmp280[2]=Cyc_PP_text(({
const char*_tmp282=",";_tag_dynforward(_tmp282,sizeof(char),_get_zero_arr_size(
_tmp282,2));}));_tmp280[1]=Cyc_Absynpp_exp2doc(_tmp21E);_tmp280[0]=Cyc_PP_text(({
const char*_tmp281="calloc(";_tag_dynforward(_tmp281,sizeof(char),
_get_zero_arr_size(_tmp281,8));}));Cyc_PP_cat(_tag_dynforward(_tmp280,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp284[7];_tmp284[6]=Cyc_PP_text(({
const char*_tmp288=")";_tag_dynforward(_tmp288,sizeof(char),_get_zero_arr_size(
_tmp288,2));}));_tmp284[5]=Cyc_Absynpp_exp2doc(st);_tmp284[4]=Cyc_PP_text(({
const char*_tmp287=",";_tag_dynforward(_tmp287,sizeof(char),_get_zero_arr_size(
_tmp287,2));}));_tmp284[3]=Cyc_Absynpp_exp2doc(_tmp21E);_tmp284[2]=Cyc_PP_text(({
const char*_tmp286=",";_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(
_tmp286,2));}));_tmp284[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp21C);
_tmp284[0]=Cyc_PP_text(({const char*_tmp285="rcalloc(";_tag_dynforward(_tmp285,
sizeof(char),_get_zero_arr_size(_tmp285,9));}));Cyc_PP_cat(_tag_dynforward(
_tmp284,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*new_e;if(
_tmp21D == 0)new_e=_tmp21E;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmp21D,0),_tmp21E,0);}if(_tmp21C == 0)s=({struct Cyc_PP_Doc*_tmp289[3];_tmp289[2]=
Cyc_PP_text(({const char*_tmp28B=")";_tag_dynforward(_tmp28B,sizeof(char),
_get_zero_arr_size(_tmp28B,2));}));_tmp289[1]=Cyc_Absynpp_exp2doc(new_e);_tmp289[
0]=Cyc_PP_text(({const char*_tmp28A="malloc(";_tag_dynforward(_tmp28A,sizeof(char),
_get_zero_arr_size(_tmp28A,8));}));Cyc_PP_cat(_tag_dynforward(_tmp289,sizeof(
struct Cyc_PP_Doc*),3));});else{s=({struct Cyc_PP_Doc*_tmp28C[5];_tmp28C[4]=Cyc_PP_text(({
const char*_tmp28F=")";_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(
_tmp28F,2));}));_tmp28C[3]=Cyc_Absynpp_exp2doc(new_e);_tmp28C[2]=Cyc_PP_text(({
const char*_tmp28E=",";_tag_dynforward(_tmp28E,sizeof(char),_get_zero_arr_size(
_tmp28E,2));}));_tmp28C[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp21C);
_tmp28C[0]=Cyc_PP_text(({const char*_tmp28D="rmalloc(";_tag_dynforward(_tmp28D,
sizeof(char),_get_zero_arr_size(_tmp28D,9));}));Cyc_PP_cat(_tag_dynforward(
_tmp28C,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL1A9;_LL1F4: if(*((int*)_tmp1DA)
!= 36)goto _LL1F6;_tmp21F=((struct Cyc_Absyn_Swap_e_struct*)_tmp1DA)->f1;_tmp220=((
struct Cyc_Absyn_Swap_e_struct*)_tmp1DA)->f2;_LL1F5: s=({struct Cyc_PP_Doc*_tmp290[
5];_tmp290[4]=Cyc_PP_text(({const char*_tmp293=")";_tag_dynforward(_tmp293,
sizeof(char),_get_zero_arr_size(_tmp293,2));}));_tmp290[3]=Cyc_Absynpp_exp2doc(
_tmp220);_tmp290[2]=Cyc_PP_text(({const char*_tmp292=",";_tag_dynforward(_tmp292,
sizeof(char),_get_zero_arr_size(_tmp292,2));}));_tmp290[1]=Cyc_Absynpp_exp2doc(
_tmp21F);_tmp290[0]=Cyc_PP_text(({const char*_tmp291="cycswap(";_tag_dynforward(
_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp290,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1A9;_LL1F6: if(*((
int*)_tmp1DA)!= 37)goto _LL1F8;_tmp221=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1DA)->f1;_tmp222=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1DA)->f2;
_LL1F7: s=Cyc_Absynpp_group_braces(({const char*_tmp294=",";_tag_dynforward(
_tmp294,sizeof(char),_get_zero_arr_size(_tmp294,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp222));goto _LL1A9;_LL1F8: if(*((int*)_tmp1DA)!= 38)goto _LL1A9;_tmp223=((struct
Cyc_Absyn_StmtExp_e_struct*)_tmp1DA)->f1;_LL1F9: s=({struct Cyc_PP_Doc*_tmp295[7];
_tmp295[6]=Cyc_PP_text(({const char*_tmp297=")";_tag_dynforward(_tmp297,sizeof(
char),_get_zero_arr_size(_tmp297,2));}));_tmp295[5]=Cyc_Absynpp_rb();_tmp295[4]=
Cyc_PP_blank_doc();_tmp295[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp223));
_tmp295[2]=Cyc_PP_blank_doc();_tmp295[1]=Cyc_Absynpp_lb();_tmp295[0]=Cyc_PP_text(({
const char*_tmp296="(";_tag_dynforward(_tmp296,sizeof(char),_get_zero_arr_size(
_tmp296,2));}));Cyc_PP_cat(_tag_dynforward(_tmp295,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL1A9;_LL1A9:;}if(inprec >= myprec)s=({struct Cyc_PP_Doc*_tmp298[3];_tmp298[2]=
Cyc_PP_text(({const char*_tmp29A=")";_tag_dynforward(_tmp29A,sizeof(char),
_get_zero_arr_size(_tmp29A,2));}));_tmp298[1]=s;_tmp298[0]=Cyc_PP_text(({const
char*_tmp299="(";_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size(_tmp299,
2));}));Cyc_PP_cat(_tag_dynforward(_tmp298,sizeof(struct Cyc_PP_Doc*),3));});
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp29B=d;
struct Cyc_Absyn_Exp*_tmp29C;struct _dynforward_ptr*_tmp29D;_LL204: if(*((int*)
_tmp29B)!= 0)goto _LL206;_tmp29C=((struct Cyc_Absyn_ArrayElement_struct*)_tmp29B)->f1;
_LL205: return({struct Cyc_PP_Doc*_tmp29E[3];_tmp29E[2]=Cyc_PP_text(({const char*
_tmp2A0="]";_tag_dynforward(_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,2));}));
_tmp29E[1]=Cyc_Absynpp_exp2doc(_tmp29C);_tmp29E[0]=Cyc_PP_text(({const char*
_tmp29F=".[";_tag_dynforward(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,3));}));
Cyc_PP_cat(_tag_dynforward(_tmp29E,sizeof(struct Cyc_PP_Doc*),3));});_LL206: if(*((
int*)_tmp29B)!= 1)goto _LL203;_tmp29D=((struct Cyc_Absyn_FieldName_struct*)_tmp29B)->f1;
_LL207: return({struct Cyc_PP_Doc*_tmp2A1[2];_tmp2A1[1]=Cyc_PP_textptr(_tmp29D);
_tmp2A1[0]=Cyc_PP_text(({const char*_tmp2A2=".";_tag_dynforward(_tmp2A2,sizeof(
char),_get_zero_arr_size(_tmp2A2,2));}));Cyc_PP_cat(_tag_dynforward(_tmp2A1,
sizeof(struct Cyc_PP_Doc*),2));});_LL203:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(
struct _tuple9*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2A3[2];_tmp2A3[1]=Cyc_Absynpp_exp2doc((*de).f2);
_tmp2A3[0]=Cyc_PP_egroup(({const char*_tmp2A4="";_tag_dynforward(_tmp2A4,sizeof(
char),_get_zero_arr_size(_tmp2A4,1));}),({const char*_tmp2A5="=";_tag_dynforward(
_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,2));}),({const char*_tmp2A6="=";
_tag_dynforward(_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_dynforward(_tmp2A3,sizeof(
struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){return Cyc_PP_group(({const char*_tmp2A7="";
_tag_dynforward(_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,1));}),({const
char*_tmp2A8="";_tag_dynforward(_tmp2A8,sizeof(char),_get_zero_arr_size(_tmp2A8,
1));}),({const char*_tmp2A9=",";_tag_dynforward(_tmp2A9,sizeof(char),
_get_zero_arr_size(_tmp2A9,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c){
union Cyc_Absyn_Cnst_union _tmp2AA=c;void*_tmp2AB;char _tmp2AC;void*_tmp2AD;short
_tmp2AE;void*_tmp2AF;int _tmp2B0;void*_tmp2B1;int _tmp2B2;void*_tmp2B3;int _tmp2B4;
void*_tmp2B5;long long _tmp2B6;struct _dynforward_ptr _tmp2B7;struct _dynforward_ptr
_tmp2B8;_LL209: if((_tmp2AA.Char_c).tag != 0)goto _LL20B;_tmp2AB=(_tmp2AA.Char_c).f1;
_tmp2AC=(_tmp2AA.Char_c).f2;_LL20A: return Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp2BB;_tmp2BB.tag=0;_tmp2BB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_char_escape(_tmp2AC));{void*
_tmp2B9[1]={& _tmp2BB};Cyc_aprintf(({const char*_tmp2BA="'%s'";_tag_dynforward(
_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,5));}),_tag_dynforward(_tmp2B9,
sizeof(void*),1));}}));_LL20B: if((_tmp2AA.Short_c).tag != 1)goto _LL20D;_tmp2AD=(
_tmp2AA.Short_c).f1;_tmp2AE=(_tmp2AA.Short_c).f2;_LL20C: return Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2BE;_tmp2BE.tag=1;_tmp2BE.f1=(
unsigned long)((int)_tmp2AE);{void*_tmp2BC[1]={& _tmp2BE};Cyc_aprintf(({const char*
_tmp2BD="%d";_tag_dynforward(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,3));}),
_tag_dynforward(_tmp2BC,sizeof(void*),1));}}));_LL20D: if((_tmp2AA.Int_c).tag != 2)
goto _LL20F;_tmp2AF=(_tmp2AA.Int_c).f1;if((int)_tmp2AF != 2)goto _LL20F;_tmp2B0=(
_tmp2AA.Int_c).f2;_LL20E: _tmp2B2=_tmp2B0;goto _LL210;_LL20F: if((_tmp2AA.Int_c).tag
!= 2)goto _LL211;_tmp2B1=(_tmp2AA.Int_c).f1;if((int)_tmp2B1 != 0)goto _LL211;
_tmp2B2=(_tmp2AA.Int_c).f2;_LL210: return Cyc_PP_text((struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp2C1;_tmp2C1.tag=1;_tmp2C1.f1=(unsigned long)_tmp2B2;{
void*_tmp2BF[1]={& _tmp2C1};Cyc_aprintf(({const char*_tmp2C0="%d";_tag_dynforward(
_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,3));}),_tag_dynforward(_tmp2BF,
sizeof(void*),1));}}));_LL211: if((_tmp2AA.Int_c).tag != 2)goto _LL213;_tmp2B3=(
_tmp2AA.Int_c).f1;if((int)_tmp2B3 != 1)goto _LL213;_tmp2B4=(_tmp2AA.Int_c).f2;
_LL212: return Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2C4;
_tmp2C4.tag=1;_tmp2C4.f1=(unsigned int)_tmp2B4;{void*_tmp2C2[1]={& _tmp2C4};Cyc_aprintf(({
const char*_tmp2C3="%u";_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size(
_tmp2C3,3));}),_tag_dynforward(_tmp2C2,sizeof(void*),1));}}));_LL213: if((_tmp2AA.LongLong_c).tag
!= 3)goto _LL215;_tmp2B5=(_tmp2AA.LongLong_c).f1;_tmp2B6=(_tmp2AA.LongLong_c).f2;
_LL214: return Cyc_PP_text(({const char*_tmp2C5="<<FIX LONG LONG CONSTANT>>";
_tag_dynforward(_tmp2C5,sizeof(char),_get_zero_arr_size(_tmp2C5,27));}));_LL215:
if((_tmp2AA.Float_c).tag != 4)goto _LL217;_tmp2B7=(_tmp2AA.Float_c).f1;_LL216:
return Cyc_PP_text(_tmp2B7);_LL217: if((_tmp2AA.Null_c).tag != 6)goto _LL219;_LL218:
return Cyc_PP_text(({const char*_tmp2C6="NULL";_tag_dynforward(_tmp2C6,sizeof(char),
_get_zero_arr_size(_tmp2C6,5));}));_LL219: if((_tmp2AA.String_c).tag != 5)goto
_LL208;_tmp2B8=(_tmp2AA.String_c).f1;_LL21A: return({struct Cyc_PP_Doc*_tmp2C7[3];
_tmp2C7[2]=Cyc_PP_text(({const char*_tmp2C9="\"";_tag_dynforward(_tmp2C9,sizeof(
char),_get_zero_arr_size(_tmp2C9,2));}));_tmp2C7[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp2B8));_tmp2C7[0]=Cyc_PP_text(({const char*_tmp2C8="\"";_tag_dynforward(
_tmp2C8,sizeof(char),_get_zero_arr_size(_tmp2C8,2));}));Cyc_PP_cat(
_tag_dynforward(_tmp2C7,sizeof(struct Cyc_PP_Doc*),3));});_LL208:;}struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*
ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es == 0  || es->tl != 0)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2CA=_cycalloc(sizeof(*
_tmp2CA));_tmp2CA[0]=({struct Cyc_Core_Failure_struct _tmp2CB;_tmp2CB.tag=Cyc_Core_Failure;
_tmp2CB.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2CE;_tmp2CE.tag=
0;_tmp2CE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2CC[1]={& _tmp2CE};Cyc_aprintf(({const char*_tmp2CD="Absynpp::primapp2doc Size: %s with bad args";
_tag_dynforward(_tmp2CD,sizeof(char),_get_zero_arr_size(_tmp2CD,44));}),
_tag_dynforward(_tmp2CC,sizeof(void*),1));}});_tmp2CB;});_tmp2CA;}));{struct Cyc_PP_Doc*
_tmp2CF=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({
struct Cyc_PP_Doc*_tmp2D0[2];_tmp2D0[1]=Cyc_PP_text(({const char*_tmp2D1=".size";
_tag_dynforward(_tmp2D1,sizeof(char),_get_zero_arr_size(_tmp2D1,6));}));_tmp2D0[
0]=_tmp2CF;Cyc_PP_cat(_tag_dynforward(_tmp2D0,sizeof(struct Cyc_PP_Doc*),2));});}}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2D2=
_cycalloc(sizeof(*_tmp2D2));_tmp2D2[0]=({struct Cyc_Core_Failure_struct _tmp2D3;
_tmp2D3.tag=Cyc_Core_Failure;_tmp2D3.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2D6;_tmp2D6.tag=0;_tmp2D6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_PP_string_of_doc(ps,72));{void*_tmp2D4[1]={& _tmp2D6};Cyc_aprintf(({const char*
_tmp2D5="Absynpp::primapp2doc: %s with no args";_tag_dynforward(_tmp2D5,sizeof(
char),_get_zero_arr_size(_tmp2D5,38));}),_tag_dynforward(_tmp2D4,sizeof(void*),1));}});
_tmp2D3;});_tmp2D2;}));else{if(ds->tl == 0)return({struct Cyc_PP_Doc*_tmp2D7[3];
_tmp2D7[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2D7[1]=Cyc_PP_text(({const char*_tmp2D8=" ";
_tag_dynforward(_tmp2D8,sizeof(char),_get_zero_arr_size(_tmp2D8,2));}));_tmp2D7[
0]=ps;Cyc_PP_cat(_tag_dynforward(_tmp2D7,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));_tmp2D9[0]=({
struct Cyc_Core_Failure_struct _tmp2DA;_tmp2DA.tag=Cyc_Core_Failure;_tmp2DA.f1=(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp2DD;_tmp2DD.tag=0;_tmp2DD.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(ps,72));{void*
_tmp2DB[1]={& _tmp2DD};Cyc_aprintf(({const char*_tmp2DC="Absynpp::primapp2doc: %s with more than 2 args";
_tag_dynforward(_tmp2DC,sizeof(char),_get_zero_arr_size(_tmp2DC,47));}),
_tag_dynforward(_tmp2DB,sizeof(void*),1));}});_tmp2DA;});_tmp2D9;}));else{return({
struct Cyc_PP_Doc*_tmp2DE[5];_tmp2DE[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd;_tmp2DE[3]=Cyc_PP_text(({const char*_tmp2E0=" ";
_tag_dynforward(_tmp2E0,sizeof(char),_get_zero_arr_size(_tmp2E0,2));}));_tmp2DE[
2]=ps;_tmp2DE[1]=Cyc_PP_text(({const char*_tmp2DF=" ";_tag_dynforward(_tmp2DF,
sizeof(char),_get_zero_arr_size(_tmp2DF,2));}));_tmp2DE[0]=(struct Cyc_PP_Doc*)ds->hd;
Cyc_PP_cat(_tag_dynforward(_tmp2DE,sizeof(struct Cyc_PP_Doc*),5));});}}}}}struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p){void*_tmp2E1=p;_LL21C: if((int)
_tmp2E1 != 0)goto _LL21E;_LL21D: return({const char*_tmp2E2="+";_tag_dynforward(
_tmp2E2,sizeof(char),_get_zero_arr_size(_tmp2E2,2));});_LL21E: if((int)_tmp2E1 != 
1)goto _LL220;_LL21F: return({const char*_tmp2E3="*";_tag_dynforward(_tmp2E3,
sizeof(char),_get_zero_arr_size(_tmp2E3,2));});_LL220: if((int)_tmp2E1 != 2)goto
_LL222;_LL221: return({const char*_tmp2E4="-";_tag_dynforward(_tmp2E4,sizeof(char),
_get_zero_arr_size(_tmp2E4,2));});_LL222: if((int)_tmp2E1 != 3)goto _LL224;_LL223:
return({const char*_tmp2E5="/";_tag_dynforward(_tmp2E5,sizeof(char),
_get_zero_arr_size(_tmp2E5,2));});_LL224: if((int)_tmp2E1 != 4)goto _LL226;_LL225:
return({const char*_tmp2E6="%";_tag_dynforward(_tmp2E6,sizeof(char),
_get_zero_arr_size(_tmp2E6,2));});_LL226: if((int)_tmp2E1 != 5)goto _LL228;_LL227:
return({const char*_tmp2E7="==";_tag_dynforward(_tmp2E7,sizeof(char),
_get_zero_arr_size(_tmp2E7,3));});_LL228: if((int)_tmp2E1 != 6)goto _LL22A;_LL229:
return({const char*_tmp2E8="!=";_tag_dynforward(_tmp2E8,sizeof(char),
_get_zero_arr_size(_tmp2E8,3));});_LL22A: if((int)_tmp2E1 != 7)goto _LL22C;_LL22B:
return({const char*_tmp2E9=">";_tag_dynforward(_tmp2E9,sizeof(char),
_get_zero_arr_size(_tmp2E9,2));});_LL22C: if((int)_tmp2E1 != 8)goto _LL22E;_LL22D:
return({const char*_tmp2EA="<";_tag_dynforward(_tmp2EA,sizeof(char),
_get_zero_arr_size(_tmp2EA,2));});_LL22E: if((int)_tmp2E1 != 9)goto _LL230;_LL22F:
return({const char*_tmp2EB=">=";_tag_dynforward(_tmp2EB,sizeof(char),
_get_zero_arr_size(_tmp2EB,3));});_LL230: if((int)_tmp2E1 != 10)goto _LL232;_LL231:
return({const char*_tmp2EC="<=";_tag_dynforward(_tmp2EC,sizeof(char),
_get_zero_arr_size(_tmp2EC,3));});_LL232: if((int)_tmp2E1 != 11)goto _LL234;_LL233:
return({const char*_tmp2ED="!";_tag_dynforward(_tmp2ED,sizeof(char),
_get_zero_arr_size(_tmp2ED,2));});_LL234: if((int)_tmp2E1 != 12)goto _LL236;_LL235:
return({const char*_tmp2EE="~";_tag_dynforward(_tmp2EE,sizeof(char),
_get_zero_arr_size(_tmp2EE,2));});_LL236: if((int)_tmp2E1 != 13)goto _LL238;_LL237:
return({const char*_tmp2EF="&";_tag_dynforward(_tmp2EF,sizeof(char),
_get_zero_arr_size(_tmp2EF,2));});_LL238: if((int)_tmp2E1 != 14)goto _LL23A;_LL239:
return({const char*_tmp2F0="|";_tag_dynforward(_tmp2F0,sizeof(char),
_get_zero_arr_size(_tmp2F0,2));});_LL23A: if((int)_tmp2E1 != 15)goto _LL23C;_LL23B:
return({const char*_tmp2F1="^";_tag_dynforward(_tmp2F1,sizeof(char),
_get_zero_arr_size(_tmp2F1,2));});_LL23C: if((int)_tmp2E1 != 16)goto _LL23E;_LL23D:
return({const char*_tmp2F2="<<";_tag_dynforward(_tmp2F2,sizeof(char),
_get_zero_arr_size(_tmp2F2,3));});_LL23E: if((int)_tmp2E1 != 17)goto _LL240;_LL23F:
return({const char*_tmp2F3=">>";_tag_dynforward(_tmp2F3,sizeof(char),
_get_zero_arr_size(_tmp2F3,3));});_LL240: if((int)_tmp2E1 != 18)goto _LL242;_LL241:
return({const char*_tmp2F4=">>>";_tag_dynforward(_tmp2F4,sizeof(char),
_get_zero_arr_size(_tmp2F4,4));});_LL242: if((int)_tmp2E1 != 19)goto _LL21B;_LL243:
return({const char*_tmp2F5="size";_tag_dynforward(_tmp2F5,sizeof(char),
_get_zero_arr_size(_tmp2F5,5));});_LL21B:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp2F6=(void*)s->r;_LL245: if(_tmp2F6 <= (void*)1)
goto _LL247;if(*((int*)_tmp2F6)!= 11)goto _LL247;_LL246: return 1;_LL247:;_LL248:
return 0;_LL244:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st){
struct Cyc_PP_Doc*s;{void*_tmp2F7=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2F8;struct
Cyc_Absyn_Stmt*_tmp2F9;struct Cyc_Absyn_Stmt*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FB;
struct Cyc_Absyn_Exp*_tmp2FC;struct Cyc_Absyn_Stmt*_tmp2FD;struct Cyc_Absyn_Stmt*
_tmp2FE;struct _tuple2 _tmp2FF;struct Cyc_Absyn_Exp*_tmp300;struct Cyc_Absyn_Stmt*
_tmp301;struct _dynforward_ptr*_tmp302;struct Cyc_Absyn_Exp*_tmp303;struct _tuple2
_tmp304;struct Cyc_Absyn_Exp*_tmp305;struct _tuple2 _tmp306;struct Cyc_Absyn_Exp*
_tmp307;struct Cyc_Absyn_Stmt*_tmp308;struct Cyc_Absyn_Exp*_tmp309;struct Cyc_List_List*
_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Absyn_Decl*
_tmp30D;struct Cyc_Absyn_Stmt*_tmp30E;struct _dynforward_ptr*_tmp30F;struct Cyc_Absyn_Stmt*
_tmp310;struct Cyc_Absyn_Stmt*_tmp311;struct _tuple2 _tmp312;struct Cyc_Absyn_Exp*
_tmp313;struct Cyc_Absyn_Stmt*_tmp314;struct Cyc_List_List*_tmp315;struct Cyc_Absyn_Tvar*
_tmp316;struct Cyc_Absyn_Vardecl*_tmp317;int _tmp318;struct Cyc_Absyn_Exp*_tmp319;
struct Cyc_Absyn_Stmt*_tmp31A;struct Cyc_Absyn_Exp*_tmp31B;struct Cyc_Absyn_Exp*
_tmp31C;struct Cyc_Absyn_Tvar*_tmp31D;struct Cyc_Absyn_Vardecl*_tmp31E;struct Cyc_Absyn_Stmt*
_tmp31F;_LL24A: if((int)_tmp2F7 != 0)goto _LL24C;_LL24B: s=Cyc_PP_text(({const char*
_tmp320=";";_tag_dynforward(_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,2));}));
goto _LL249;_LL24C: if(_tmp2F7 <= (void*)1)goto _LL24E;if(*((int*)_tmp2F7)!= 0)goto
_LL24E;_tmp2F8=((struct Cyc_Absyn_Exp_s_struct*)_tmp2F7)->f1;_LL24D: s=({struct Cyc_PP_Doc*
_tmp321[2];_tmp321[1]=Cyc_PP_text(({const char*_tmp322=";";_tag_dynforward(
_tmp322,sizeof(char),_get_zero_arr_size(_tmp322,2));}));_tmp321[0]=Cyc_Absynpp_exp2doc(
_tmp2F8);Cyc_PP_cat(_tag_dynforward(_tmp321,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL249;_LL24E: if(_tmp2F7 <= (void*)1)goto _LL250;if(*((int*)_tmp2F7)!= 1)goto
_LL250;_tmp2F9=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F7)->f1;_tmp2FA=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2F7)->f2;_LL24F: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp2F9))s=({struct Cyc_PP_Doc*_tmp323[7];_tmp323[6]=Cyc_Absynpp_is_declaration(
_tmp2FA)?({struct Cyc_PP_Doc*_tmp324[5];_tmp324[4]=Cyc_PP_line_doc();_tmp324[3]=
Cyc_Absynpp_rb();_tmp324[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2FA));_tmp324[
1]=Cyc_PP_blank_doc();_tmp324[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(
_tmp324,sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(_tmp2FA);_tmp323[5]=
Cyc_PP_line_doc();_tmp323[4]=Cyc_Absynpp_rb();_tmp323[3]=Cyc_PP_line_doc();
_tmp323[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F9));_tmp323[1]=Cyc_PP_blank_doc();
_tmp323[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dynforward(_tmp323,sizeof(struct Cyc_PP_Doc*),
7));});else{if(Cyc_Absynpp_is_declaration(_tmp2FA))s=({struct Cyc_PP_Doc*_tmp325[
7];_tmp325[6]=Cyc_PP_line_doc();_tmp325[5]=Cyc_Absynpp_rb();_tmp325[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2FA));_tmp325[3]=Cyc_PP_blank_doc();_tmp325[2]=Cyc_Absynpp_lb();
_tmp325[1]=Cyc_PP_line_doc();_tmp325[0]=Cyc_Absynpp_stmt2doc(_tmp2F9);Cyc_PP_cat(
_tag_dynforward(_tmp325,sizeof(struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({const char*_tmp326="";_tag_dynforward(
_tmp326,sizeof(char),_get_zero_arr_size(_tmp326,1));}),({struct Cyc_List_List*
_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->hd=_tmp2F9;_tmp327->tl=({struct Cyc_List_List*
_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328->hd=_tmp2FA;_tmp328->tl=0;_tmp328;});
_tmp327;}));}}}else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp329="";_tag_dynforward(_tmp329,sizeof(char),_get_zero_arr_size(
_tmp329,1));}),({struct Cyc_List_List*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->hd=
_tmp2F9;_tmp32A->tl=({struct Cyc_List_List*_tmp32B=_cycalloc(sizeof(*_tmp32B));
_tmp32B->hd=_tmp2FA;_tmp32B->tl=0;_tmp32B;});_tmp32A;}));}goto _LL249;_LL250: if(
_tmp2F7 <= (void*)1)goto _LL252;if(*((int*)_tmp2F7)!= 2)goto _LL252;_tmp2FB=((
struct Cyc_Absyn_Return_s_struct*)_tmp2F7)->f1;_LL251: if(_tmp2FB == 0)s=Cyc_PP_text(({
const char*_tmp32C="return;";_tag_dynforward(_tmp32C,sizeof(char),
_get_zero_arr_size(_tmp32C,8));}));else{s=({struct Cyc_PP_Doc*_tmp32D[3];_tmp32D[
2]=Cyc_PP_text(({const char*_tmp32F=";";_tag_dynforward(_tmp32F,sizeof(char),
_get_zero_arr_size(_tmp32F,2));}));_tmp32D[1]=_tmp2FB == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2FB);_tmp32D[0]=Cyc_PP_text(({const char*_tmp32E="return ";
_tag_dynforward(_tmp32E,sizeof(char),_get_zero_arr_size(_tmp32E,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp32D,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL249;_LL252: if(
_tmp2F7 <= (void*)1)goto _LL254;if(*((int*)_tmp2F7)!= 3)goto _LL254;_tmp2FC=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F7)->f1;_tmp2FD=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2F7)->f2;_tmp2FE=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F7)->f3;_LL253: {
int print_else;{void*_tmp330=(void*)_tmp2FE->r;_LL273: if((int)_tmp330 != 0)goto
_LL275;_LL274: print_else=0;goto _LL272;_LL275:;_LL276: print_else=1;goto _LL272;
_LL272:;}s=({struct Cyc_PP_Doc*_tmp331[8];_tmp331[7]=print_else?({struct Cyc_PP_Doc*
_tmp335[6];_tmp335[5]=Cyc_Absynpp_rb();_tmp335[4]=Cyc_PP_line_doc();_tmp335[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp337[2];_tmp337[1]=Cyc_Absynpp_stmt2doc(
_tmp2FE);_tmp337[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp337,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp335[2]=Cyc_Absynpp_lb();_tmp335[1]=Cyc_PP_text(({
const char*_tmp336="else ";_tag_dynforward(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,6));}));_tmp335[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dynforward(_tmp335,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();
_tmp331[6]=Cyc_Absynpp_rb();_tmp331[5]=Cyc_PP_line_doc();_tmp331[4]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp334[2];_tmp334[1]=Cyc_Absynpp_stmt2doc(_tmp2FD);_tmp334[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp334,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp331[3]=Cyc_Absynpp_lb();_tmp331[2]=Cyc_PP_text(({const char*_tmp333=") ";
_tag_dynforward(_tmp333,sizeof(char),_get_zero_arr_size(_tmp333,3));}));_tmp331[
1]=Cyc_Absynpp_exp2doc(_tmp2FC);_tmp331[0]=Cyc_PP_text(({const char*_tmp332="if (";
_tag_dynforward(_tmp332,sizeof(char),_get_zero_arr_size(_tmp332,5));}));Cyc_PP_cat(
_tag_dynforward(_tmp331,sizeof(struct Cyc_PP_Doc*),8));});goto _LL249;}_LL254: if(
_tmp2F7 <= (void*)1)goto _LL256;if(*((int*)_tmp2F7)!= 4)goto _LL256;_tmp2FF=((
struct Cyc_Absyn_While_s_struct*)_tmp2F7)->f1;_tmp300=_tmp2FF.f1;_tmp301=((struct
Cyc_Absyn_While_s_struct*)_tmp2F7)->f2;_LL255: s=({struct Cyc_PP_Doc*_tmp338[7];
_tmp338[6]=Cyc_Absynpp_rb();_tmp338[5]=Cyc_PP_line_doc();_tmp338[4]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp33B[2];_tmp33B[1]=Cyc_Absynpp_stmt2doc(_tmp301);_tmp33B[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp33B,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp338[3]=Cyc_Absynpp_lb();_tmp338[2]=Cyc_PP_text(({const char*_tmp33A=") ";
_tag_dynforward(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,3));}));_tmp338[
1]=Cyc_Absynpp_exp2doc(_tmp300);_tmp338[0]=Cyc_PP_text(({const char*_tmp339="while (";
_tag_dynforward(_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,8));}));Cyc_PP_cat(
_tag_dynforward(_tmp338,sizeof(struct Cyc_PP_Doc*),7));});goto _LL249;_LL256: if(
_tmp2F7 <= (void*)1)goto _LL258;if(*((int*)_tmp2F7)!= 5)goto _LL258;_LL257: s=Cyc_PP_text(({
const char*_tmp33C="break;";_tag_dynforward(_tmp33C,sizeof(char),
_get_zero_arr_size(_tmp33C,7));}));goto _LL249;_LL258: if(_tmp2F7 <= (void*)1)goto
_LL25A;if(*((int*)_tmp2F7)!= 6)goto _LL25A;_LL259: s=Cyc_PP_text(({const char*
_tmp33D="continue;";_tag_dynforward(_tmp33D,sizeof(char),_get_zero_arr_size(
_tmp33D,10));}));goto _LL249;_LL25A: if(_tmp2F7 <= (void*)1)goto _LL25C;if(*((int*)
_tmp2F7)!= 7)goto _LL25C;_tmp302=((struct Cyc_Absyn_Goto_s_struct*)_tmp2F7)->f1;
_LL25B: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp340;
_tmp340.tag=0;_tmp340.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp302);{
void*_tmp33E[1]={& _tmp340};Cyc_aprintf(({const char*_tmp33F="goto %s;";
_tag_dynforward(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,9));}),
_tag_dynforward(_tmp33E,sizeof(void*),1));}}));goto _LL249;_LL25C: if(_tmp2F7 <= (
void*)1)goto _LL25E;if(*((int*)_tmp2F7)!= 8)goto _LL25E;_tmp303=((struct Cyc_Absyn_For_s_struct*)
_tmp2F7)->f1;_tmp304=((struct Cyc_Absyn_For_s_struct*)_tmp2F7)->f2;_tmp305=
_tmp304.f1;_tmp306=((struct Cyc_Absyn_For_s_struct*)_tmp2F7)->f3;_tmp307=_tmp306.f1;
_tmp308=((struct Cyc_Absyn_For_s_struct*)_tmp2F7)->f4;_LL25D: s=({struct Cyc_PP_Doc*
_tmp341[11];_tmp341[10]=Cyc_Absynpp_rb();_tmp341[9]=Cyc_PP_line_doc();_tmp341[8]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp346[2];_tmp346[1]=Cyc_Absynpp_stmt2doc(
_tmp308);_tmp346[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp346,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp341[7]=Cyc_Absynpp_lb();_tmp341[6]=Cyc_PP_text(({
const char*_tmp345=") ";_tag_dynforward(_tmp345,sizeof(char),_get_zero_arr_size(
_tmp345,3));}));_tmp341[5]=Cyc_Absynpp_exp2doc(_tmp307);_tmp341[4]=Cyc_PP_text(({
const char*_tmp344="; ";_tag_dynforward(_tmp344,sizeof(char),_get_zero_arr_size(
_tmp344,3));}));_tmp341[3]=Cyc_Absynpp_exp2doc(_tmp305);_tmp341[2]=Cyc_PP_text(({
const char*_tmp343="; ";_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size(
_tmp343,3));}));_tmp341[1]=Cyc_Absynpp_exp2doc(_tmp303);_tmp341[0]=Cyc_PP_text(({
const char*_tmp342="for(";_tag_dynforward(_tmp342,sizeof(char),_get_zero_arr_size(
_tmp342,5));}));Cyc_PP_cat(_tag_dynforward(_tmp341,sizeof(struct Cyc_PP_Doc*),11));});
goto _LL249;_LL25E: if(_tmp2F7 <= (void*)1)goto _LL260;if(*((int*)_tmp2F7)!= 9)goto
_LL260;_tmp309=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F7)->f1;_tmp30A=((struct
Cyc_Absyn_Switch_s_struct*)_tmp2F7)->f2;_LL25F: s=({struct Cyc_PP_Doc*_tmp347[8];
_tmp347[7]=Cyc_Absynpp_rb();_tmp347[6]=Cyc_PP_line_doc();_tmp347[5]=Cyc_Absynpp_switchclauses2doc(
_tmp30A);_tmp347[4]=Cyc_PP_line_doc();_tmp347[3]=Cyc_Absynpp_lb();_tmp347[2]=Cyc_PP_text(({
const char*_tmp349=") ";_tag_dynforward(_tmp349,sizeof(char),_get_zero_arr_size(
_tmp349,3));}));_tmp347[1]=Cyc_Absynpp_exp2doc(_tmp309);_tmp347[0]=Cyc_PP_text(({
const char*_tmp348="switch (";_tag_dynforward(_tmp348,sizeof(char),
_get_zero_arr_size(_tmp348,9));}));Cyc_PP_cat(_tag_dynforward(_tmp347,sizeof(
struct Cyc_PP_Doc*),8));});goto _LL249;_LL260: if(_tmp2F7 <= (void*)1)goto _LL262;if(*((
int*)_tmp2F7)!= 10)goto _LL262;_tmp30B=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2F7)->f1;if(_tmp30B != 0)goto _LL262;_LL261: s=Cyc_PP_text(({const char*_tmp34A="fallthru;";
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,10));}));goto
_LL249;_LL262: if(_tmp2F7 <= (void*)1)goto _LL264;if(*((int*)_tmp2F7)!= 10)goto
_LL264;_tmp30C=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2F7)->f1;_LL263: s=({
struct Cyc_PP_Doc*_tmp34B[3];_tmp34B[2]=Cyc_PP_text(({const char*_tmp34D=");";
_tag_dynforward(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,3));}));_tmp34B[
1]=Cyc_Absynpp_exps2doc_prec(20,_tmp30C);_tmp34B[0]=Cyc_PP_text(({const char*
_tmp34C="fallthru(";_tag_dynforward(_tmp34C,sizeof(char),_get_zero_arr_size(
_tmp34C,10));}));Cyc_PP_cat(_tag_dynforward(_tmp34B,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL249;_LL264: if(_tmp2F7 <= (void*)1)goto _LL266;if(*((int*)_tmp2F7)!= 11)goto
_LL266;_tmp30D=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F7)->f1;_tmp30E=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2F7)->f2;_LL265: s=({struct Cyc_PP_Doc*_tmp34E[3];_tmp34E[2]=Cyc_Absynpp_stmt2doc(
_tmp30E);_tmp34E[1]=Cyc_PP_line_doc();_tmp34E[0]=Cyc_Absynpp_decl2doc(_tmp30D);
Cyc_PP_cat(_tag_dynforward(_tmp34E,sizeof(struct Cyc_PP_Doc*),3));});goto _LL249;
_LL266: if(_tmp2F7 <= (void*)1)goto _LL268;if(*((int*)_tmp2F7)!= 12)goto _LL268;
_tmp30F=((struct Cyc_Absyn_Label_s_struct*)_tmp2F7)->f1;_tmp310=((struct Cyc_Absyn_Label_s_struct*)
_tmp2F7)->f2;_LL267: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp310))s=({struct Cyc_PP_Doc*_tmp34F[7];_tmp34F[6]=Cyc_Absynpp_rb();_tmp34F[5]=
Cyc_PP_line_doc();_tmp34F[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp310));
_tmp34F[3]=Cyc_PP_line_doc();_tmp34F[2]=Cyc_Absynpp_lb();_tmp34F[1]=Cyc_PP_text(({
const char*_tmp350=": ";_tag_dynforward(_tmp350,sizeof(char),_get_zero_arr_size(
_tmp350,3));}));_tmp34F[0]=Cyc_PP_textptr(_tmp30F);Cyc_PP_cat(_tag_dynforward(
_tmp34F,sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*_tmp351[3];
_tmp351[2]=Cyc_Absynpp_stmt2doc(_tmp310);_tmp351[1]=Cyc_PP_text(({const char*
_tmp352=": ";_tag_dynforward(_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,3));}));
_tmp351[0]=Cyc_PP_textptr(_tmp30F);Cyc_PP_cat(_tag_dynforward(_tmp351,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL249;_LL268: if(_tmp2F7 <= (void*)1)goto _LL26A;
if(*((int*)_tmp2F7)!= 13)goto _LL26A;_tmp311=((struct Cyc_Absyn_Do_s_struct*)
_tmp2F7)->f1;_tmp312=((struct Cyc_Absyn_Do_s_struct*)_tmp2F7)->f2;_tmp313=_tmp312.f1;
_LL269: s=({struct Cyc_PP_Doc*_tmp353[9];_tmp353[8]=Cyc_PP_text(({const char*
_tmp356=");";_tag_dynforward(_tmp356,sizeof(char),_get_zero_arr_size(_tmp356,3));}));
_tmp353[7]=Cyc_Absynpp_exp2doc(_tmp313);_tmp353[6]=Cyc_PP_text(({const char*
_tmp355=" while (";_tag_dynforward(_tmp355,sizeof(char),_get_zero_arr_size(
_tmp355,9));}));_tmp353[5]=Cyc_Absynpp_rb();_tmp353[4]=Cyc_PP_line_doc();_tmp353[
3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp311));_tmp353[2]=Cyc_PP_line_doc();
_tmp353[1]=Cyc_Absynpp_lb();_tmp353[0]=Cyc_PP_text(({const char*_tmp354="do ";
_tag_dynforward(_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,4));}));Cyc_PP_cat(
_tag_dynforward(_tmp353,sizeof(struct Cyc_PP_Doc*),9));});goto _LL249;_LL26A: if(
_tmp2F7 <= (void*)1)goto _LL26C;if(*((int*)_tmp2F7)!= 14)goto _LL26C;_tmp314=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F7)->f1;_tmp315=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp2F7)->f2;_LL26B: s=({struct Cyc_PP_Doc*_tmp357[12];_tmp357[11]=Cyc_Absynpp_rb();
_tmp357[10]=Cyc_PP_line_doc();_tmp357[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp315));_tmp357[8]=Cyc_PP_line_doc();_tmp357[7]=Cyc_Absynpp_lb();_tmp357[6]=
Cyc_PP_text(({const char*_tmp359=" catch ";_tag_dynforward(_tmp359,sizeof(char),
_get_zero_arr_size(_tmp359,8));}));_tmp357[5]=Cyc_Absynpp_rb();_tmp357[4]=Cyc_PP_line_doc();
_tmp357[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp314));_tmp357[2]=Cyc_PP_line_doc();
_tmp357[1]=Cyc_Absynpp_lb();_tmp357[0]=Cyc_PP_text(({const char*_tmp358="try ";
_tag_dynforward(_tmp358,sizeof(char),_get_zero_arr_size(_tmp358,5));}));Cyc_PP_cat(
_tag_dynforward(_tmp357,sizeof(struct Cyc_PP_Doc*),12));});goto _LL249;_LL26C: if(
_tmp2F7 <= (void*)1)goto _LL26E;if(*((int*)_tmp2F7)!= 15)goto _LL26E;_tmp316=((
struct Cyc_Absyn_Region_s_struct*)_tmp2F7)->f1;_tmp317=((struct Cyc_Absyn_Region_s_struct*)
_tmp2F7)->f2;_tmp318=((struct Cyc_Absyn_Region_s_struct*)_tmp2F7)->f3;_tmp319=((
struct Cyc_Absyn_Region_s_struct*)_tmp2F7)->f4;_tmp31A=((struct Cyc_Absyn_Region_s_struct*)
_tmp2F7)->f5;_LL26D: s=({struct Cyc_PP_Doc*_tmp35A[12];_tmp35A[11]=Cyc_Absynpp_rb();
_tmp35A[10]=Cyc_PP_line_doc();_tmp35A[9]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp31A));_tmp35A[8]=Cyc_PP_line_doc();_tmp35A[7]=Cyc_Absynpp_lb();_tmp35A[6]=
_tmp319 != 0?({struct Cyc_PP_Doc*_tmp35F[3];_tmp35F[2]=Cyc_PP_text(({const char*
_tmp361=")";_tag_dynforward(_tmp361,sizeof(char),_get_zero_arr_size(_tmp361,2));}));
_tmp35F[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp319);_tmp35F[0]=Cyc_PP_text(({
const char*_tmp360=" = open(";_tag_dynforward(_tmp360,sizeof(char),
_get_zero_arr_size(_tmp360,9));}));Cyc_PP_cat(_tag_dynforward(_tmp35F,sizeof(
struct Cyc_PP_Doc*),3));}): Cyc_PP_nil_doc();_tmp35A[5]=Cyc_Absynpp_qvar2doc(
_tmp317->name);_tmp35A[4]=Cyc_PP_text(({const char*_tmp35E=">";_tag_dynforward(
_tmp35E,sizeof(char),_get_zero_arr_size(_tmp35E,2));}));_tmp35A[3]=Cyc_PP_textptr(
Cyc_Absynpp_get_name(_tmp316));_tmp35A[2]=Cyc_PP_text(({const char*_tmp35D="<";
_tag_dynforward(_tmp35D,sizeof(char),_get_zero_arr_size(_tmp35D,2));}));_tmp35A[
1]=_tmp318?Cyc_PP_nil_doc(): Cyc_PP_text(({const char*_tmp35C="[resetable]";
_tag_dynforward(_tmp35C,sizeof(char),_get_zero_arr_size(_tmp35C,12));}));_tmp35A[
0]=Cyc_PP_text(({const char*_tmp35B="region";_tag_dynforward(_tmp35B,sizeof(char),
_get_zero_arr_size(_tmp35B,7));}));Cyc_PP_cat(_tag_dynforward(_tmp35A,sizeof(
struct Cyc_PP_Doc*),12));});goto _LL249;_LL26E: if(_tmp2F7 <= (void*)1)goto _LL270;
if(*((int*)_tmp2F7)!= 16)goto _LL270;_tmp31B=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp2F7)->f1;_LL26F: s=({struct Cyc_PP_Doc*_tmp362[3];_tmp362[2]=Cyc_PP_text(({
const char*_tmp364=");";_tag_dynforward(_tmp364,sizeof(char),_get_zero_arr_size(
_tmp364,3));}));_tmp362[1]=Cyc_Absynpp_exp2doc(_tmp31B);_tmp362[0]=Cyc_PP_text(({
const char*_tmp363="reset_region(";_tag_dynforward(_tmp363,sizeof(char),
_get_zero_arr_size(_tmp363,14));}));Cyc_PP_cat(_tag_dynforward(_tmp362,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL249;_LL270: if(_tmp2F7 <= (void*)1)goto _LL249;if(*((
int*)_tmp2F7)!= 17)goto _LL249;_tmp31C=((struct Cyc_Absyn_Alias_s_struct*)_tmp2F7)->f1;
_tmp31D=((struct Cyc_Absyn_Alias_s_struct*)_tmp2F7)->f2;_tmp31E=((struct Cyc_Absyn_Alias_s_struct*)
_tmp2F7)->f3;_tmp31F=((struct Cyc_Absyn_Alias_s_struct*)_tmp2F7)->f4;_LL271: s=({
struct Cyc_PP_Doc*_tmp365[12];_tmp365[11]=Cyc_Absynpp_rb();_tmp365[10]=Cyc_PP_line_doc();
_tmp365[9]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp31F));_tmp365[8]=Cyc_PP_line_doc();
_tmp365[7]=Cyc_Absynpp_lb();_tmp365[6]=Cyc_Absynpp_qvar2doc(_tmp31E->name);
_tmp365[5]=Cyc_PP_text(({const char*_tmp369=">";_tag_dynforward(_tmp369,sizeof(
char),_get_zero_arr_size(_tmp369,2));}));_tmp365[4]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp31D));_tmp365[3]=Cyc_PP_text(({const char*_tmp368="<";_tag_dynforward(_tmp368,
sizeof(char),_get_zero_arr_size(_tmp368,2));}));_tmp365[2]=Cyc_PP_text(({const
char*_tmp367=") = ";_tag_dynforward(_tmp367,sizeof(char),_get_zero_arr_size(
_tmp367,5));}));_tmp365[1]=Cyc_Absynpp_exp2doc(_tmp31C);_tmp365[0]=Cyc_PP_text(({
const char*_tmp366="alias(";_tag_dynforward(_tmp366,sizeof(char),
_get_zero_arr_size(_tmp366,7));}));Cyc_PP_cat(_tag_dynforward(_tmp365,sizeof(
struct Cyc_PP_Doc*),12));});goto _LL249;_LL249:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp36A=(void*)p->r;void*_tmp36B;
int _tmp36C;char _tmp36D;struct _dynforward_ptr _tmp36E;struct Cyc_Absyn_Vardecl*
_tmp36F;struct Cyc_Absyn_Pat*_tmp370;struct Cyc_Absyn_Pat _tmp371;void*_tmp372;
struct Cyc_Absyn_Vardecl*_tmp373;struct Cyc_Absyn_Pat*_tmp374;struct Cyc_Absyn_Tvar*
_tmp375;struct Cyc_Absyn_Vardecl*_tmp376;struct Cyc_List_List*_tmp377;int _tmp378;
struct Cyc_Absyn_Pat*_tmp379;struct Cyc_Absyn_Vardecl*_tmp37A;struct Cyc_Absyn_Pat*
_tmp37B;struct Cyc_Absyn_Pat _tmp37C;void*_tmp37D;struct Cyc_Absyn_Vardecl*_tmp37E;
struct Cyc_Absyn_Pat*_tmp37F;struct _tuple0*_tmp380;struct _tuple0*_tmp381;struct
Cyc_List_List*_tmp382;int _tmp383;struct Cyc_Absyn_AggrInfo _tmp384;union Cyc_Absyn_AggrInfoU_union
_tmp385;struct Cyc_List_List*_tmp386;struct Cyc_List_List*_tmp387;int _tmp388;
struct Cyc_Absyn_Enumfield*_tmp389;struct Cyc_Absyn_Enumfield*_tmp38A;struct Cyc_Absyn_Tunionfield*
_tmp38B;struct Cyc_List_List*_tmp38C;struct Cyc_Absyn_Tunionfield*_tmp38D;struct
Cyc_List_List*_tmp38E;int _tmp38F;struct Cyc_Absyn_Exp*_tmp390;_LL278: if((int)
_tmp36A != 0)goto _LL27A;_LL279: s=Cyc_PP_text(({const char*_tmp391="_";
_tag_dynforward(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,2));}));goto
_LL277;_LL27A: if((int)_tmp36A != 1)goto _LL27C;_LL27B: s=Cyc_PP_text(({const char*
_tmp392="NULL";_tag_dynforward(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,5));}));
goto _LL277;_LL27C: if(_tmp36A <= (void*)2)goto _LL27E;if(*((int*)_tmp36A)!= 7)goto
_LL27E;_tmp36B=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp36A)->f1;_tmp36C=((
struct Cyc_Absyn_Int_p_struct*)_tmp36A)->f2;_LL27D: if(_tmp36B != (void*)((void*)1))
s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp395;_tmp395.tag=
1;_tmp395.f1=(unsigned long)_tmp36C;{void*_tmp393[1]={& _tmp395};Cyc_aprintf(({
const char*_tmp394="%d";_tag_dynforward(_tmp394,sizeof(char),_get_zero_arr_size(
_tmp394,3));}),_tag_dynforward(_tmp393,sizeof(void*),1));}}));else{s=Cyc_PP_text((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp398;_tmp398.tag=1;_tmp398.f1=(
unsigned int)_tmp36C;{void*_tmp396[1]={& _tmp398};Cyc_aprintf(({const char*_tmp397="%u";
_tag_dynforward(_tmp397,sizeof(char),_get_zero_arr_size(_tmp397,3));}),
_tag_dynforward(_tmp396,sizeof(void*),1));}}));}goto _LL277;_LL27E: if(_tmp36A <= (
void*)2)goto _LL280;if(*((int*)_tmp36A)!= 8)goto _LL280;_tmp36D=((struct Cyc_Absyn_Char_p_struct*)
_tmp36A)->f1;_LL27F: s=Cyc_PP_text((struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp39B;_tmp39B.tag=0;_tmp39B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_char_escape(_tmp36D));{void*_tmp399[1]={& _tmp39B};Cyc_aprintf(({
const char*_tmp39A="'%s'";_tag_dynforward(_tmp39A,sizeof(char),_get_zero_arr_size(
_tmp39A,5));}),_tag_dynforward(_tmp399,sizeof(void*),1));}}));goto _LL277;_LL280:
if(_tmp36A <= (void*)2)goto _LL282;if(*((int*)_tmp36A)!= 9)goto _LL282;_tmp36E=((
struct Cyc_Absyn_Float_p_struct*)_tmp36A)->f1;_LL281: s=Cyc_PP_text(_tmp36E);goto
_LL277;_LL282: if(_tmp36A <= (void*)2)goto _LL284;if(*((int*)_tmp36A)!= 0)goto
_LL284;_tmp36F=((struct Cyc_Absyn_Var_p_struct*)_tmp36A)->f1;_tmp370=((struct Cyc_Absyn_Var_p_struct*)
_tmp36A)->f2;_tmp371=*_tmp370;_tmp372=(void*)_tmp371.r;if((int)_tmp372 != 0)goto
_LL284;_LL283: s=Cyc_Absynpp_qvar2doc(_tmp36F->name);goto _LL277;_LL284: if(_tmp36A
<= (void*)2)goto _LL286;if(*((int*)_tmp36A)!= 0)goto _LL286;_tmp373=((struct Cyc_Absyn_Var_p_struct*)
_tmp36A)->f1;_tmp374=((struct Cyc_Absyn_Var_p_struct*)_tmp36A)->f2;_LL285: s=({
struct Cyc_PP_Doc*_tmp39C[3];_tmp39C[2]=Cyc_Absynpp_pat2doc(_tmp374);_tmp39C[1]=
Cyc_PP_text(({const char*_tmp39D=" as ";_tag_dynforward(_tmp39D,sizeof(char),
_get_zero_arr_size(_tmp39D,5));}));_tmp39C[0]=Cyc_Absynpp_qvar2doc(_tmp373->name);
Cyc_PP_cat(_tag_dynforward(_tmp39C,sizeof(struct Cyc_PP_Doc*),3));});goto _LL277;
_LL286: if(_tmp36A <= (void*)2)goto _LL288;if(*((int*)_tmp36A)!= 2)goto _LL288;
_tmp375=((struct Cyc_Absyn_TagInt_p_struct*)_tmp36A)->f1;_tmp376=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp36A)->f2;_LL287: s=({struct Cyc_PP_Doc*_tmp39E[4];_tmp39E[3]=Cyc_PP_text(({
const char*_tmp3A0=">";_tag_dynforward(_tmp3A0,sizeof(char),_get_zero_arr_size(
_tmp3A0,2));}));_tmp39E[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp375));_tmp39E[
1]=Cyc_PP_text(({const char*_tmp39F="<";_tag_dynforward(_tmp39F,sizeof(char),
_get_zero_arr_size(_tmp39F,2));}));_tmp39E[0]=Cyc_Absynpp_qvar2doc(_tmp376->name);
Cyc_PP_cat(_tag_dynforward(_tmp39E,sizeof(struct Cyc_PP_Doc*),4));});goto _LL277;
_LL288: if(_tmp36A <= (void*)2)goto _LL28A;if(*((int*)_tmp36A)!= 3)goto _LL28A;
_tmp377=((struct Cyc_Absyn_Tuple_p_struct*)_tmp36A)->f1;_tmp378=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp36A)->f2;_LL289: s=({struct Cyc_PP_Doc*_tmp3A1[4];_tmp3A1[3]=_tmp378?Cyc_PP_text(({
const char*_tmp3A4=", ...)";_tag_dynforward(_tmp3A4,sizeof(char),
_get_zero_arr_size(_tmp3A4,7));})): Cyc_PP_text(({const char*_tmp3A5=")";
_tag_dynforward(_tmp3A5,sizeof(char),_get_zero_arr_size(_tmp3A5,2));}));_tmp3A1[
2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,({
const char*_tmp3A3=",";_tag_dynforward(_tmp3A3,sizeof(char),_get_zero_arr_size(
_tmp3A3,2));}),_tmp377);_tmp3A1[1]=Cyc_PP_text(({const char*_tmp3A2="(";
_tag_dynforward(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,2));}));_tmp3A1[
0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dynforward(_tmp3A1,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL277;_LL28A: if(_tmp36A <= (void*)2)goto _LL28C;if(*((int*)_tmp36A)!= 
4)goto _LL28C;_tmp379=((struct Cyc_Absyn_Pointer_p_struct*)_tmp36A)->f1;_LL28B: s=({
struct Cyc_PP_Doc*_tmp3A6[2];_tmp3A6[1]=Cyc_Absynpp_pat2doc(_tmp379);_tmp3A6[0]=
Cyc_PP_text(({const char*_tmp3A7="&";_tag_dynforward(_tmp3A7,sizeof(char),
_get_zero_arr_size(_tmp3A7,2));}));Cyc_PP_cat(_tag_dynforward(_tmp3A6,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL277;_LL28C: if(_tmp36A <= (void*)2)goto _LL28E;if(*((
int*)_tmp36A)!= 1)goto _LL28E;_tmp37A=((struct Cyc_Absyn_Reference_p_struct*)
_tmp36A)->f1;_tmp37B=((struct Cyc_Absyn_Reference_p_struct*)_tmp36A)->f2;_tmp37C=*
_tmp37B;_tmp37D=(void*)_tmp37C.r;if((int)_tmp37D != 0)goto _LL28E;_LL28D: s=({
struct Cyc_PP_Doc*_tmp3A8[2];_tmp3A8[1]=Cyc_Absynpp_qvar2doc(_tmp37A->name);
_tmp3A8[0]=Cyc_PP_text(({const char*_tmp3A9="*";_tag_dynforward(_tmp3A9,sizeof(
char),_get_zero_arr_size(_tmp3A9,2));}));Cyc_PP_cat(_tag_dynforward(_tmp3A8,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL277;_LL28E: if(_tmp36A <= (void*)2)goto
_LL290;if(*((int*)_tmp36A)!= 1)goto _LL290;_tmp37E=((struct Cyc_Absyn_Reference_p_struct*)
_tmp36A)->f1;_tmp37F=((struct Cyc_Absyn_Reference_p_struct*)_tmp36A)->f2;_LL28F: s=({
struct Cyc_PP_Doc*_tmp3AA[4];_tmp3AA[3]=Cyc_Absynpp_pat2doc(_tmp37F);_tmp3AA[2]=
Cyc_PP_text(({const char*_tmp3AC=" as ";_tag_dynforward(_tmp3AC,sizeof(char),
_get_zero_arr_size(_tmp3AC,5));}));_tmp3AA[1]=Cyc_Absynpp_qvar2doc(_tmp37E->name);
_tmp3AA[0]=Cyc_PP_text(({const char*_tmp3AB="*";_tag_dynforward(_tmp3AB,sizeof(
char),_get_zero_arr_size(_tmp3AB,2));}));Cyc_PP_cat(_tag_dynforward(_tmp3AA,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL277;_LL290: if(_tmp36A <= (void*)2)goto
_LL292;if(*((int*)_tmp36A)!= 12)goto _LL292;_tmp380=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp36A)->f1;_LL291: s=Cyc_Absynpp_qvar2doc(_tmp380);goto _LL277;_LL292: if(_tmp36A
<= (void*)2)goto _LL294;if(*((int*)_tmp36A)!= 13)goto _LL294;_tmp381=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp36A)->f1;_tmp382=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp36A)->f2;
_tmp383=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp36A)->f3;_LL293: {struct
_dynforward_ptr term=_tmp383?({const char*_tmp3B0=", ...)";_tag_dynforward(_tmp3B0,
sizeof(char),_get_zero_arr_size(_tmp3B0,7));}):({const char*_tmp3B1=")";
_tag_dynforward(_tmp3B1,sizeof(char),_get_zero_arr_size(_tmp3B1,2));});s=({
struct Cyc_PP_Doc*_tmp3AD[2];_tmp3AD[1]=Cyc_PP_group(({const char*_tmp3AE="(";
_tag_dynforward(_tmp3AE,sizeof(char),_get_zero_arr_size(_tmp3AE,2));}),term,({
const char*_tmp3AF=",";_tag_dynforward(_tmp3AF,sizeof(char),_get_zero_arr_size(
_tmp3AF,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp382));_tmp3AD[0]=
Cyc_Absynpp_qvar2doc(_tmp381);Cyc_PP_cat(_tag_dynforward(_tmp3AD,sizeof(struct
Cyc_PP_Doc*),2));});goto _LL277;}_LL294: if(_tmp36A <= (void*)2)goto _LL296;if(*((
int*)_tmp36A)!= 5)goto _LL296;_tmp384=((struct Cyc_Absyn_Aggr_p_struct*)_tmp36A)->f1;
_tmp385=_tmp384.aggr_info;_tmp386=((struct Cyc_Absyn_Aggr_p_struct*)_tmp36A)->f2;
_tmp387=((struct Cyc_Absyn_Aggr_p_struct*)_tmp36A)->f3;_tmp388=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp36A)->f4;_LL295: {struct _dynforward_ptr term=_tmp388?({const char*_tmp3BA=", ...}";
_tag_dynforward(_tmp3BA,sizeof(char),_get_zero_arr_size(_tmp3BA,7));}):({const
char*_tmp3BB="}";_tag_dynforward(_tmp3BB,sizeof(char),_get_zero_arr_size(_tmp3BB,
2));});struct _tuple0*_tmp3B3;struct _tuple3 _tmp3B2=Cyc_Absyn_aggr_kinded_name(
_tmp385);_tmp3B3=_tmp3B2.f2;s=({struct Cyc_PP_Doc*_tmp3B4[4];_tmp3B4[3]=Cyc_PP_group(({
const char*_tmp3B8="";_tag_dynforward(_tmp3B8,sizeof(char),_get_zero_arr_size(
_tmp3B8,1));}),term,({const char*_tmp3B9=",";_tag_dynforward(_tmp3B9,sizeof(char),
_get_zero_arr_size(_tmp3B9,2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp387));
_tmp3B4[2]=Cyc_PP_egroup(({const char*_tmp3B5="[";_tag_dynforward(_tmp3B5,sizeof(
char),_get_zero_arr_size(_tmp3B5,2));}),({const char*_tmp3B6="]";_tag_dynforward(
_tmp3B6,sizeof(char),_get_zero_arr_size(_tmp3B6,2));}),({const char*_tmp3B7=",";
_tag_dynforward(_tmp3B7,sizeof(char),_get_zero_arr_size(_tmp3B7,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp386)));_tmp3B4[1]=Cyc_Absynpp_lb();_tmp3B4[0]=Cyc_Absynpp_qvar2doc(_tmp3B3);
Cyc_PP_cat(_tag_dynforward(_tmp3B4,sizeof(struct Cyc_PP_Doc*),4));});goto _LL277;}
_LL296: if(_tmp36A <= (void*)2)goto _LL298;if(*((int*)_tmp36A)!= 10)goto _LL298;
_tmp389=((struct Cyc_Absyn_Enum_p_struct*)_tmp36A)->f2;_LL297: _tmp38A=_tmp389;
goto _LL299;_LL298: if(_tmp36A <= (void*)2)goto _LL29A;if(*((int*)_tmp36A)!= 11)goto
_LL29A;_tmp38A=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp36A)->f2;_LL299: s=Cyc_Absynpp_qvar2doc(
_tmp38A->name);goto _LL277;_LL29A: if(_tmp36A <= (void*)2)goto _LL29C;if(*((int*)
_tmp36A)!= 6)goto _LL29C;_tmp38B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36A)->f2;
_tmp38C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36A)->f3;if(_tmp38C != 0)goto
_LL29C;_LL29B: s=Cyc_Absynpp_qvar2doc(_tmp38B->name);goto _LL277;_LL29C: if(_tmp36A
<= (void*)2)goto _LL29E;if(*((int*)_tmp36A)!= 6)goto _LL29E;_tmp38D=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp36A)->f2;_tmp38E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36A)->f3;_tmp38F=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp36A)->f4;_LL29D: {struct _dynforward_ptr term=
_tmp38F?({const char*_tmp3BF=", ...)";_tag_dynforward(_tmp3BF,sizeof(char),
_get_zero_arr_size(_tmp3BF,7));}):({const char*_tmp3C0=")";_tag_dynforward(
_tmp3C0,sizeof(char),_get_zero_arr_size(_tmp3C0,2));});s=({struct Cyc_PP_Doc*
_tmp3BC[2];_tmp3BC[1]=Cyc_PP_egroup(({const char*_tmp3BD="(";_tag_dynforward(
_tmp3BD,sizeof(char),_get_zero_arr_size(_tmp3BD,2));}),term,({const char*_tmp3BE=",";
_tag_dynforward(_tmp3BE,sizeof(char),_get_zero_arr_size(_tmp3BE,2));}),((struct
Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp38E));_tmp3BC[0]=Cyc_Absynpp_qvar2doc(
_tmp38D->name);Cyc_PP_cat(_tag_dynforward(_tmp3BC,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL277;}_LL29E: if(_tmp36A <= (void*)2)goto _LL277;if(*((int*)_tmp36A)!= 14)
goto _LL277;_tmp390=((struct Cyc_Absyn_Exp_p_struct*)_tmp36A)->f1;_LL29F: s=Cyc_Absynpp_exp2doc(
_tmp390);goto _LL277;_LL277:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple8*dp){return({struct Cyc_PP_Doc*_tmp3C1[2];_tmp3C1[1]=Cyc_Absynpp_pat2doc((*
dp).f2);_tmp3C1[0]=Cyc_PP_egroup(({const char*_tmp3C2="";_tag_dynforward(_tmp3C2,
sizeof(char),_get_zero_arr_size(_tmp3C2,1));}),({const char*_tmp3C3="=";
_tag_dynforward(_tmp3C3,sizeof(char),_get_zero_arr_size(_tmp3C3,2));}),({const
char*_tmp3C4="=";_tag_dynforward(_tmp3C4,sizeof(char),_get_zero_arr_size(_tmp3C4,
2));}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(
_tag_dynforward(_tmp3C1,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (void*)(c->pattern)->r
== (void*)((void*)0))return({struct Cyc_PP_Doc*_tmp3C5[2];_tmp3C5[1]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3C7[2];_tmp3C7[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3C7[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3C7,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3C5[0]=Cyc_PP_text(({const char*_tmp3C6="default: ";_tag_dynforward(
_tmp3C6,sizeof(char),_get_zero_arr_size(_tmp3C6,10));}));Cyc_PP_cat(
_tag_dynforward(_tmp3C5,sizeof(struct Cyc_PP_Doc*),2));});else{if(c->where_clause
== 0)return({struct Cyc_PP_Doc*_tmp3C8[4];_tmp3C8[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp3CB[2];_tmp3CB[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3CB[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_dynforward(_tmp3CB,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3C8[2]=
Cyc_PP_text(({const char*_tmp3CA=": ";_tag_dynforward(_tmp3CA,sizeof(char),
_get_zero_arr_size(_tmp3CA,3));}));_tmp3C8[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3C8[0]=Cyc_PP_text(({const char*_tmp3C9="case ";_tag_dynforward(_tmp3C9,
sizeof(char),_get_zero_arr_size(_tmp3C9,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp3C8,sizeof(struct Cyc_PP_Doc*),4));});else{return({struct Cyc_PP_Doc*_tmp3CC[6];
_tmp3CC[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3D0[2];_tmp3D0[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3D0[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp3D0,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3CC[4]=Cyc_PP_text(({const char*_tmp3CF=": ";
_tag_dynforward(_tmp3CF,sizeof(char),_get_zero_arr_size(_tmp3CF,3));}));_tmp3CC[
3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));
_tmp3CC[2]=Cyc_PP_text(({const char*_tmp3CE=" && ";_tag_dynforward(_tmp3CE,
sizeof(char),_get_zero_arr_size(_tmp3CE,5));}));_tmp3CC[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3CC[0]=Cyc_PP_text(({const char*_tmp3CD="case ";_tag_dynforward(_tmp3CD,
sizeof(char),_get_zero_arr_size(_tmp3CD,6));}));Cyc_PP_cat(_tag_dynforward(
_tmp3CC,sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _dynforward_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({const char*_tmp3D1="";
_tag_dynforward(_tmp3D1,sizeof(char),_get_zero_arr_size(_tmp3D1,1));}),cs);}
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag
== 0)return Cyc_Absynpp_qvar2doc(f->name);else{return({struct Cyc_PP_Doc*_tmp3D2[3];
_tmp3D2[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));
_tmp3D2[1]=Cyc_PP_text(({const char*_tmp3D3=" = ";_tag_dynforward(_tmp3D3,sizeof(
char),_get_zero_arr_size(_tmp3D3,4));}));_tmp3D2[0]=Cyc_Absynpp_qvar2doc(f->name);
Cyc_PP_cat(_tag_dynforward(_tmp3D2,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*
_tmp3D4=",";_tag_dynforward(_tmp3D4,sizeof(char),_get_zero_arr_size(_tmp3D4,2));}),
fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({
const char*_tmp3D5=",";_tag_dynforward(_tmp3D5,sizeof(char),_get_zero_arr_size(
_tmp3D5,2));}),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd){struct Cyc_Absyn_Vardecl _tmp3D7;void*_tmp3D8;struct _tuple0*_tmp3D9;struct Cyc_Absyn_Tqual
_tmp3DA;void*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_List_List*_tmp3DD;
struct Cyc_Absyn_Vardecl*_tmp3D6=vd;_tmp3D7=*_tmp3D6;_tmp3D8=(void*)_tmp3D7.sc;
_tmp3D9=_tmp3D7.name;_tmp3DA=_tmp3D7.tq;_tmp3DB=(void*)_tmp3D7.type;_tmp3DC=
_tmp3D7.initializer;_tmp3DD=_tmp3D7.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3D9);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3DD);struct Cyc_PP_Doc*beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=
attsdoc;else{void*_tmp3DE=Cyc_Tcutil_compress(_tmp3DB);struct Cyc_Absyn_FnInfo
_tmp3DF;struct Cyc_List_List*_tmp3E0;_LL2A1: if(_tmp3DE <= (void*)4)goto _LL2A3;if(*((
int*)_tmp3DE)!= 8)goto _LL2A3;_tmp3DF=((struct Cyc_Absyn_FnType_struct*)_tmp3DE)->f1;
_tmp3E0=_tmp3DF.attributes;_LL2A2: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3E0);
goto _LL2A0;_LL2A3:;_LL2A4: beforenamedoc=Cyc_PP_nil_doc();goto _LL2A0;_LL2A0:;}s=({
struct Cyc_PP_Doc*_tmp3E1[5];_tmp3E1[4]=Cyc_PP_text(({const char*_tmp3E6=";";
_tag_dynforward(_tmp3E6,sizeof(char),_get_zero_arr_size(_tmp3E6,2));}));_tmp3E1[
3]=_tmp3DC == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp3E4[2];_tmp3E4[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3DC);_tmp3E4[0]=Cyc_PP_text(({const char*_tmp3E5=" = ";
_tag_dynforward(_tmp3E5,sizeof(char),_get_zero_arr_size(_tmp3E5,4));}));Cyc_PP_cat(
_tag_dynforward(_tmp3E4,sizeof(struct Cyc_PP_Doc*),2));});_tmp3E1[2]=Cyc_Absynpp_tqtd2doc(
_tmp3DA,_tmp3DB,({struct Cyc_Core_Opt*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->v=({
struct Cyc_PP_Doc*_tmp3E3[2];_tmp3E3[1]=sn;_tmp3E3[0]=beforenamedoc;Cyc_PP_cat(
_tag_dynforward(_tmp3E3,sizeof(struct Cyc_PP_Doc*),2));});_tmp3E2;}));_tmp3E1[1]=
Cyc_Absynpp_scope2doc(_tmp3D8);_tmp3E1[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();
Cyc_PP_cat(_tag_dynforward(_tmp3E1,sizeof(struct Cyc_PP_Doc*),5));});return s;}}
struct _tuple11{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*
Cyc_Absynpp_export2doc(struct _tuple11*x){struct _tuple0*_tmp3E8;struct _tuple11
_tmp3E7=*x;_tmp3E8=_tmp3E7.f2;return Cyc_Absynpp_qvar2doc(_tmp3E8);}struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp3E9=(
void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3EA;struct Cyc_Absyn_Aggrdecl*_tmp3EB;
struct Cyc_Absyn_Vardecl*_tmp3EC;struct Cyc_Absyn_Tuniondecl*_tmp3ED;struct Cyc_Absyn_Tuniondecl
_tmp3EE;void*_tmp3EF;struct _tuple0*_tmp3F0;struct Cyc_List_List*_tmp3F1;struct Cyc_Core_Opt*
_tmp3F2;int _tmp3F3;int _tmp3F4;struct Cyc_Absyn_Pat*_tmp3F5;struct Cyc_Absyn_Exp*
_tmp3F6;struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_Enumdecl*_tmp3F8;struct Cyc_Absyn_Enumdecl
_tmp3F9;void*_tmp3FA;struct _tuple0*_tmp3FB;struct Cyc_Core_Opt*_tmp3FC;struct Cyc_Absyn_Typedefdecl*
_tmp3FD;struct _dynforward_ptr*_tmp3FE;struct Cyc_List_List*_tmp3FF;struct _tuple0*
_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp402;struct Cyc_List_List*
_tmp403;struct Cyc_List_List*_tmp404;_LL2A6: if(_tmp3E9 <= (void*)2)goto _LL2BE;if(*((
int*)_tmp3E9)!= 1)goto _LL2A8;_tmp3EA=((struct Cyc_Absyn_Fn_d_struct*)_tmp3E9)->f1;
_LL2A7: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp40D=_cycalloc(sizeof(*
_tmp40D));_tmp40D[0]=({struct Cyc_Absyn_FnType_struct _tmp40E;_tmp40E.tag=8;
_tmp40E.f1=({struct Cyc_Absyn_FnInfo _tmp40F;_tmp40F.tvars=_tmp3EA->tvs;_tmp40F.effect=
_tmp3EA->effect;_tmp40F.ret_typ=(void*)((void*)_tmp3EA->ret_type);_tmp40F.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3EA->args);_tmp40F.c_varargs=_tmp3EA->c_varargs;
_tmp40F.cyc_varargs=_tmp3EA->cyc_varargs;_tmp40F.rgn_po=_tmp3EA->rgn_po;_tmp40F.attributes=
0;_tmp40F;});_tmp40E;});_tmp40D;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3EA->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3EA->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(({const char*_tmp405="__inline ";_tag_dynforward(_tmp405,
sizeof(char),_get_zero_arr_size(_tmp405,10));}));else{inlinedoc=Cyc_PP_text(({
const char*_tmp406="inline ";_tag_dynforward(_tmp406,sizeof(char),
_get_zero_arr_size(_tmp406,8));}));}}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc((void*)_tmp3EA->sc);struct Cyc_PP_Doc*
beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(_tmp3EA->attributes):
attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp3EA->name);
struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,({
struct Cyc_Core_Opt*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->v=({struct Cyc_PP_Doc*
_tmp40C[2];_tmp40C[1]=namedoc;_tmp40C[0]=beforenamedoc;Cyc_PP_cat(
_tag_dynforward(_tmp40C,sizeof(struct Cyc_PP_Doc*),2));});_tmp40B;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp409[5];_tmp409[4]=Cyc_Absynpp_rb();_tmp409[3]=Cyc_PP_line_doc();
_tmp409[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp40A[2];_tmp40A[1]=Cyc_Absynpp_stmt2doc(
_tmp3EA->body);_tmp40A[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp40A,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp409[1]=Cyc_Absynpp_lb();_tmp409[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_dynforward(_tmp409,sizeof(struct Cyc_PP_Doc*),5));});s=({struct
Cyc_PP_Doc*_tmp407[4];_tmp407[3]=bodydoc;_tmp407[2]=tqtddoc;_tmp407[1]=scopedoc;
_tmp407[0]=inlinedoc;Cyc_PP_cat(_tag_dynforward(_tmp407,sizeof(struct Cyc_PP_Doc*),
4));});if(Cyc_Absynpp_to_VC)s=({struct Cyc_PP_Doc*_tmp408[2];_tmp408[1]=s;_tmp408[
0]=attsdoc;Cyc_PP_cat(_tag_dynforward(_tmp408,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL2A5;}}_LL2A8: if(*((int*)_tmp3E9)!= 4)goto _LL2AA;_tmp3EB=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp3E9)->f1;_LL2A9: if(_tmp3EB->impl == 0)s=({struct Cyc_PP_Doc*_tmp410[5];_tmp410[
4]=Cyc_PP_text(({const char*_tmp411=";";_tag_dynforward(_tmp411,sizeof(char),
_get_zero_arr_size(_tmp411,2));}));_tmp410[3]=Cyc_Absynpp_ktvars2doc(_tmp3EB->tvs);
_tmp410[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3EB->name);_tmp410[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp3EB->kind);_tmp410[0]=Cyc_Absynpp_scope2doc((void*)_tmp3EB->sc);Cyc_PP_cat(
_tag_dynforward(_tmp410,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp412[12];_tmp412[11]=Cyc_PP_text(({const char*_tmp416=";";_tag_dynforward(
_tmp416,sizeof(char),_get_zero_arr_size(_tmp416,2));}));_tmp412[10]=Cyc_Absynpp_atts2doc(
_tmp3EB->attributes);_tmp412[9]=Cyc_Absynpp_rb();_tmp412[8]=Cyc_PP_line_doc();
_tmp412[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp415[2];_tmp415[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3EB->impl))->fields);_tmp415[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp415,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp412[6]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3EB->impl))->rgn_po == 
0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp413[2];_tmp413[1]=Cyc_Absynpp_rgnpo2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3EB->impl))->rgn_po);_tmp413[0]=
Cyc_PP_text(({const char*_tmp414=":";_tag_dynforward(_tmp414,sizeof(char),
_get_zero_arr_size(_tmp414,2));}));Cyc_PP_cat(_tag_dynforward(_tmp413,sizeof(
struct Cyc_PP_Doc*),2));});_tmp412[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3EB->impl))->exist_vars);_tmp412[4]=Cyc_Absynpp_lb();_tmp412[3]=
Cyc_PP_blank_doc();_tmp412[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3EB->name);_tmp412[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3EB->kind);_tmp412[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3EB->sc);Cyc_PP_cat(_tag_dynforward(_tmp412,sizeof(struct Cyc_PP_Doc*),
12));});}goto _LL2A5;_LL2AA: if(*((int*)_tmp3E9)!= 0)goto _LL2AC;_tmp3EC=((struct
Cyc_Absyn_Var_d_struct*)_tmp3E9)->f1;_LL2AB: s=Cyc_Absynpp_vardecl2doc(_tmp3EC);
goto _LL2A5;_LL2AC: if(*((int*)_tmp3E9)!= 5)goto _LL2AE;_tmp3ED=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp3E9)->f1;_tmp3EE=*_tmp3ED;_tmp3EF=(void*)_tmp3EE.sc;_tmp3F0=_tmp3EE.name;
_tmp3F1=_tmp3EE.tvs;_tmp3F2=_tmp3EE.fields;_tmp3F3=_tmp3EE.is_xtunion;_tmp3F4=
_tmp3EE.is_flat;_LL2AD: if(_tmp3F2 == 0)s=({struct Cyc_PP_Doc*_tmp417[6];_tmp417[5]=
Cyc_PP_text(({const char*_tmp41B=";";_tag_dynforward(_tmp41B,sizeof(char),
_get_zero_arr_size(_tmp41B,2));}));_tmp417[4]=Cyc_Absynpp_ktvars2doc(_tmp3F1);
_tmp417[3]=_tmp3F3?Cyc_Absynpp_qvar2bolddoc(_tmp3F0): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3F0);_tmp417[2]=_tmp3F4?Cyc_PP_text(({const char*_tmp41A="__attribute__((flat)) ";
_tag_dynforward(_tmp41A,sizeof(char),_get_zero_arr_size(_tmp41A,23));})): Cyc_PP_blank_doc();
_tmp417[1]=_tmp3F3?Cyc_PP_text(({const char*_tmp418="xtunion ";_tag_dynforward(
_tmp418,sizeof(char),_get_zero_arr_size(_tmp418,9));})): Cyc_PP_text(({const char*
_tmp419="tunion ";_tag_dynforward(_tmp419,sizeof(char),_get_zero_arr_size(
_tmp419,8));}));_tmp417[0]=Cyc_Absynpp_scope2doc(_tmp3EF);Cyc_PP_cat(
_tag_dynforward(_tmp417,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*
_tmp41C[11];_tmp41C[10]=Cyc_PP_text(({const char*_tmp421=";";_tag_dynforward(
_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,2));}));_tmp41C[9]=Cyc_Absynpp_rb();
_tmp41C[8]=Cyc_PP_line_doc();_tmp41C[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp420[
2];_tmp420[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)_tmp3F2->v);
_tmp420[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp420,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp41C[6]=Cyc_Absynpp_lb();_tmp41C[5]=Cyc_PP_blank_doc();_tmp41C[4]=
_tmp3F4?Cyc_PP_text(({const char*_tmp41F="__attribute__((flat)) ";_tag_dynforward(
_tmp41F,sizeof(char),_get_zero_arr_size(_tmp41F,23));})): Cyc_PP_blank_doc();
_tmp41C[3]=Cyc_Absynpp_ktvars2doc(_tmp3F1);_tmp41C[2]=_tmp3F3?Cyc_Absynpp_qvar2bolddoc(
_tmp3F0): Cyc_Absynpp_typedef_name2bolddoc(_tmp3F0);_tmp41C[1]=_tmp3F3?Cyc_PP_text(({
const char*_tmp41D="xtunion ";_tag_dynforward(_tmp41D,sizeof(char),
_get_zero_arr_size(_tmp41D,9));})): Cyc_PP_text(({const char*_tmp41E="tunion ";
_tag_dynforward(_tmp41E,sizeof(char),_get_zero_arr_size(_tmp41E,8));}));_tmp41C[
0]=Cyc_Absynpp_scope2doc(_tmp3EF);Cyc_PP_cat(_tag_dynforward(_tmp41C,sizeof(
struct Cyc_PP_Doc*),11));});}goto _LL2A5;_LL2AE: if(*((int*)_tmp3E9)!= 2)goto _LL2B0;
_tmp3F5=((struct Cyc_Absyn_Let_d_struct*)_tmp3E9)->f1;_tmp3F6=((struct Cyc_Absyn_Let_d_struct*)
_tmp3E9)->f3;_LL2AF: s=({struct Cyc_PP_Doc*_tmp422[5];_tmp422[4]=Cyc_PP_text(({
const char*_tmp425=";";_tag_dynforward(_tmp425,sizeof(char),_get_zero_arr_size(
_tmp425,2));}));_tmp422[3]=Cyc_Absynpp_exp2doc(_tmp3F6);_tmp422[2]=Cyc_PP_text(({
const char*_tmp424=" = ";_tag_dynforward(_tmp424,sizeof(char),_get_zero_arr_size(
_tmp424,4));}));_tmp422[1]=Cyc_Absynpp_pat2doc(_tmp3F5);_tmp422[0]=Cyc_PP_text(({
const char*_tmp423="let ";_tag_dynforward(_tmp423,sizeof(char),_get_zero_arr_size(
_tmp423,5));}));Cyc_PP_cat(_tag_dynforward(_tmp422,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL2A5;_LL2B0: if(*((int*)_tmp3E9)!= 3)goto _LL2B2;_tmp3F7=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3E9)->f1;_LL2B1: s=({struct Cyc_PP_Doc*_tmp426[3];_tmp426[2]=Cyc_PP_text(({
const char*_tmp428=";";_tag_dynforward(_tmp428,sizeof(char),_get_zero_arr_size(
_tmp428,2));}));_tmp426[1]=Cyc_Absynpp_ids2doc(_tmp3F7);_tmp426[0]=Cyc_PP_text(({
const char*_tmp427="let ";_tag_dynforward(_tmp427,sizeof(char),_get_zero_arr_size(
_tmp427,5));}));Cyc_PP_cat(_tag_dynforward(_tmp426,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL2A5;_LL2B2: if(*((int*)_tmp3E9)!= 6)goto _LL2B4;_tmp3F8=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3E9)->f1;_tmp3F9=*_tmp3F8;_tmp3FA=(void*)_tmp3F9.sc;_tmp3FB=_tmp3F9.name;
_tmp3FC=_tmp3F9.fields;_LL2B3: if(_tmp3FC == 0)s=({struct Cyc_PP_Doc*_tmp429[4];
_tmp429[3]=Cyc_PP_text(({const char*_tmp42B=";";_tag_dynforward(_tmp42B,sizeof(
char),_get_zero_arr_size(_tmp42B,2));}));_tmp429[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3FB);_tmp429[1]=Cyc_PP_text(({const char*_tmp42A="enum ";_tag_dynforward(
_tmp42A,sizeof(char),_get_zero_arr_size(_tmp42A,6));}));_tmp429[0]=Cyc_Absynpp_scope2doc(
_tmp3FA);Cyc_PP_cat(_tag_dynforward(_tmp429,sizeof(struct Cyc_PP_Doc*),4));});
else{s=({struct Cyc_PP_Doc*_tmp42C[9];_tmp42C[8]=Cyc_PP_text(({const char*_tmp42F=";";
_tag_dynforward(_tmp42F,sizeof(char),_get_zero_arr_size(_tmp42F,2));}));_tmp42C[
7]=Cyc_Absynpp_rb();_tmp42C[6]=Cyc_PP_line_doc();_tmp42C[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp42E[2];_tmp42E[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp3FC->v);_tmp42E[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dynforward(_tmp42E,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp42C[4]=Cyc_Absynpp_lb();_tmp42C[3]=Cyc_PP_blank_doc();
_tmp42C[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3FB);_tmp42C[1]=Cyc_PP_text(({const char*
_tmp42D="enum ";_tag_dynforward(_tmp42D,sizeof(char),_get_zero_arr_size(_tmp42D,
6));}));_tmp42C[0]=Cyc_Absynpp_scope2doc(_tmp3FA);Cyc_PP_cat(_tag_dynforward(
_tmp42C,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL2A5;_LL2B4: if(*((int*)_tmp3E9)
!= 7)goto _LL2B6;_tmp3FD=((struct Cyc_Absyn_Typedef_d_struct*)_tmp3E9)->f1;_LL2B5: {
void*t;if(_tmp3FD->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3FD->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp3FD->kind,0);}s=({struct Cyc_PP_Doc*_tmp430[4];
_tmp430[3]=Cyc_PP_text(({const char*_tmp434=";";_tag_dynforward(_tmp434,sizeof(
char),_get_zero_arr_size(_tmp434,2));}));_tmp430[2]=Cyc_Absynpp_atts2doc(_tmp3FD->atts);
_tmp430[1]=Cyc_Absynpp_tqtd2doc(_tmp3FD->tq,t,({struct Cyc_Core_Opt*_tmp432=
_cycalloc(sizeof(*_tmp432));_tmp432->v=({struct Cyc_PP_Doc*_tmp433[2];_tmp433[1]=
Cyc_Absynpp_tvars2doc(_tmp3FD->tvs);_tmp433[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3FD->name);Cyc_PP_cat(_tag_dynforward(_tmp433,sizeof(struct Cyc_PP_Doc*),2));});
_tmp432;}));_tmp430[0]=Cyc_PP_text(({const char*_tmp431="typedef ";
_tag_dynforward(_tmp431,sizeof(char),_get_zero_arr_size(_tmp431,9));}));Cyc_PP_cat(
_tag_dynforward(_tmp430,sizeof(struct Cyc_PP_Doc*),4));});goto _LL2A5;}_LL2B6: if(*((
int*)_tmp3E9)!= 8)goto _LL2B8;_tmp3FE=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3E9)->f1;_tmp3FF=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3E9)->f2;_LL2B7:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3FE);s=({
struct Cyc_PP_Doc*_tmp435[8];_tmp435[7]=Cyc_Absynpp_rb();_tmp435[6]=Cyc_PP_line_doc();
_tmp435[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp437="";_tag_dynforward(_tmp437,sizeof(char),_get_zero_arr_size(
_tmp437,1));}),_tmp3FF);_tmp435[4]=Cyc_PP_line_doc();_tmp435[3]=Cyc_Absynpp_lb();
_tmp435[2]=Cyc_PP_blank_doc();_tmp435[1]=Cyc_PP_textptr(_tmp3FE);_tmp435[0]=Cyc_PP_text(({
const char*_tmp436="namespace ";_tag_dynforward(_tmp436,sizeof(char),
_get_zero_arr_size(_tmp436,11));}));Cyc_PP_cat(_tag_dynforward(_tmp435,sizeof(
struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL2A5;_LL2B8: if(*((int*)_tmp3E9)!= 9)goto _LL2BA;_tmp400=((struct Cyc_Absyn_Using_d_struct*)
_tmp3E9)->f1;_tmp401=((struct Cyc_Absyn_Using_d_struct*)_tmp3E9)->f2;_LL2B9: if(
Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp438[8];_tmp438[7]=Cyc_Absynpp_rb();
_tmp438[6]=Cyc_PP_line_doc();_tmp438[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp43A="";_tag_dynforward(_tmp43A,sizeof(char),
_get_zero_arr_size(_tmp43A,1));}),_tmp401);_tmp438[4]=Cyc_PP_line_doc();_tmp438[
3]=Cyc_Absynpp_lb();_tmp438[2]=Cyc_PP_blank_doc();_tmp438[1]=Cyc_Absynpp_qvar2doc(
_tmp400);_tmp438[0]=Cyc_PP_text(({const char*_tmp439="using ";_tag_dynforward(
_tmp439,sizeof(char),_get_zero_arr_size(_tmp439,7));}));Cyc_PP_cat(
_tag_dynforward(_tmp438,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp43B[11];_tmp43B[10]=Cyc_PP_text(({const char*_tmp440=" */";_tag_dynforward(
_tmp440,sizeof(char),_get_zero_arr_size(_tmp440,4));}));_tmp43B[9]=Cyc_Absynpp_rb();
_tmp43B[8]=Cyc_PP_text(({const char*_tmp43F="/* ";_tag_dynforward(_tmp43F,sizeof(
char),_get_zero_arr_size(_tmp43F,4));}));_tmp43B[7]=Cyc_PP_line_doc();_tmp43B[6]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp43E="";_tag_dynforward(_tmp43E,sizeof(char),_get_zero_arr_size(
_tmp43E,1));}),_tmp401);_tmp43B[5]=Cyc_PP_line_doc();_tmp43B[4]=Cyc_PP_text(({
const char*_tmp43D=" */";_tag_dynforward(_tmp43D,sizeof(char),_get_zero_arr_size(
_tmp43D,4));}));_tmp43B[3]=Cyc_Absynpp_lb();_tmp43B[2]=Cyc_PP_blank_doc();
_tmp43B[1]=Cyc_Absynpp_qvar2doc(_tmp400);_tmp43B[0]=Cyc_PP_text(({const char*
_tmp43C="/* using ";_tag_dynforward(_tmp43C,sizeof(char),_get_zero_arr_size(
_tmp43C,10));}));Cyc_PP_cat(_tag_dynforward(_tmp43B,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL2A5;_LL2BA: if(*((int*)_tmp3E9)!= 10)goto _LL2BC;_tmp402=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3E9)->f1;_LL2BB: if(Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*
_tmp441[6];_tmp441[5]=Cyc_Absynpp_rb();_tmp441[4]=Cyc_PP_line_doc();_tmp441[3]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp443="";_tag_dynforward(_tmp443,sizeof(char),_get_zero_arr_size(
_tmp443,1));}),_tmp402);_tmp441[2]=Cyc_PP_line_doc();_tmp441[1]=Cyc_Absynpp_lb();
_tmp441[0]=Cyc_PP_text(({const char*_tmp442="extern \"C\" ";_tag_dynforward(
_tmp442,sizeof(char),_get_zero_arr_size(_tmp442,12));}));Cyc_PP_cat(
_tag_dynforward(_tmp441,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*
_tmp444[9];_tmp444[8]=Cyc_PP_text(({const char*_tmp449=" */";_tag_dynforward(
_tmp449,sizeof(char),_get_zero_arr_size(_tmp449,4));}));_tmp444[7]=Cyc_Absynpp_rb();
_tmp444[6]=Cyc_PP_text(({const char*_tmp448="/* ";_tag_dynforward(_tmp448,sizeof(
char),_get_zero_arr_size(_tmp448,4));}));_tmp444[5]=Cyc_PP_line_doc();_tmp444[4]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp447="";_tag_dynforward(_tmp447,sizeof(char),_get_zero_arr_size(
_tmp447,1));}),_tmp402);_tmp444[3]=Cyc_PP_line_doc();_tmp444[2]=Cyc_PP_text(({
const char*_tmp446=" */";_tag_dynforward(_tmp446,sizeof(char),_get_zero_arr_size(
_tmp446,4));}));_tmp444[1]=Cyc_Absynpp_lb();_tmp444[0]=Cyc_PP_text(({const char*
_tmp445="/* extern \"C\" ";_tag_dynforward(_tmp445,sizeof(char),
_get_zero_arr_size(_tmp445,15));}));Cyc_PP_cat(_tag_dynforward(_tmp444,sizeof(
struct Cyc_PP_Doc*),9));});}goto _LL2A5;_LL2BC: if(*((int*)_tmp3E9)!= 11)goto _LL2BE;
_tmp403=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3E9)->f1;_tmp404=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp3E9)->f2;_LL2BD: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp404 != 0)exs_doc=({struct Cyc_PP_Doc*_tmp44A[7];
_tmp44A[6]=Cyc_Absynpp_rb();_tmp44A[5]=Cyc_PP_line_doc();_tmp44A[4]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct _tuple11*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,({const char*_tmp44C=",";_tag_dynforward(
_tmp44C,sizeof(char),_get_zero_arr_size(_tmp44C,2));}),_tmp404);_tmp44A[3]=Cyc_PP_line_doc();
_tmp44A[2]=Cyc_Absynpp_lb();_tmp44A[1]=Cyc_PP_text(({const char*_tmp44B=" export ";
_tag_dynforward(_tmp44B,sizeof(char),_get_zero_arr_size(_tmp44B,9));}));_tmp44A[
0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_dynforward(_tmp44A,sizeof(struct Cyc_PP_Doc*),
7));});else{exs_doc=Cyc_Absynpp_rb();}s=({struct Cyc_PP_Doc*_tmp44D[6];_tmp44D[5]=
exs_doc;_tmp44D[4]=Cyc_PP_line_doc();_tmp44D[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp44F="";_tag_dynforward(_tmp44F,sizeof(char),
_get_zero_arr_size(_tmp44F,1));}),_tmp403);_tmp44D[2]=Cyc_PP_line_doc();_tmp44D[
1]=Cyc_Absynpp_lb();_tmp44D[0]=Cyc_PP_text(({const char*_tmp44E="extern \"C include\" ";
_tag_dynforward(_tmp44E,sizeof(char),_get_zero_arr_size(_tmp44E,20));}));Cyc_PP_cat(
_tag_dynforward(_tmp44D,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({struct Cyc_PP_Doc*
_tmp450[9];_tmp450[8]=Cyc_PP_text(({const char*_tmp455=" */";_tag_dynforward(
_tmp455,sizeof(char),_get_zero_arr_size(_tmp455,4));}));_tmp450[7]=Cyc_Absynpp_rb();
_tmp450[6]=Cyc_PP_text(({const char*_tmp454="/* ";_tag_dynforward(_tmp454,sizeof(
char),_get_zero_arr_size(_tmp454,4));}));_tmp450[5]=Cyc_PP_line_doc();_tmp450[4]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp453="";_tag_dynforward(_tmp453,sizeof(char),_get_zero_arr_size(
_tmp453,1));}),_tmp403);_tmp450[3]=Cyc_PP_line_doc();_tmp450[2]=Cyc_PP_text(({
const char*_tmp452=" */";_tag_dynforward(_tmp452,sizeof(char),_get_zero_arr_size(
_tmp452,4));}));_tmp450[1]=Cyc_Absynpp_lb();_tmp450[0]=Cyc_PP_text(({const char*
_tmp451="/* extern \"C include\" ";_tag_dynforward(_tmp451,sizeof(char),
_get_zero_arr_size(_tmp451,23));}));Cyc_PP_cat(_tag_dynforward(_tmp450,sizeof(
struct Cyc_PP_Doc*),9));});}goto _LL2A5;_LL2BE: if((int)_tmp3E9 != 0)goto _LL2C0;
_LL2BF: s=({struct Cyc_PP_Doc*_tmp456[2];_tmp456[1]=Cyc_Absynpp_lb();_tmp456[0]=
Cyc_PP_text(({const char*_tmp457="__cyclone_port_on__;";_tag_dynforward(_tmp457,
sizeof(char),_get_zero_arr_size(_tmp457,21));}));Cyc_PP_cat(_tag_dynforward(
_tmp456,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2A5;_LL2C0: if((int)_tmp3E9 != 1)
goto _LL2A5;_LL2C1: s=({struct Cyc_PP_Doc*_tmp458[2];_tmp458[1]=Cyc_Absynpp_lb();
_tmp458[0]=Cyc_PP_text(({const char*_tmp459="__cyclone_port_off__;";
_tag_dynforward(_tmp459,sizeof(char),_get_zero_arr_size(_tmp459,22));}));Cyc_PP_cat(
_tag_dynforward(_tmp458,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2A5;_LL2A5:;}
return s;}int Cyc_Absynpp_print_scopes=1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc){if(!Cyc_Absynpp_print_scopes)return Cyc_PP_nil_doc();{void*_tmp45A=sc;
_LL2C3: if((int)_tmp45A != 0)goto _LL2C5;_LL2C4: return Cyc_PP_text(({const char*
_tmp45B="static ";_tag_dynforward(_tmp45B,sizeof(char),_get_zero_arr_size(
_tmp45B,8));}));_LL2C5: if((int)_tmp45A != 2)goto _LL2C7;_LL2C6: return Cyc_PP_nil_doc();
_LL2C7: if((int)_tmp45A != 3)goto _LL2C9;_LL2C8: return Cyc_PP_text(({const char*
_tmp45C="extern ";_tag_dynforward(_tmp45C,sizeof(char),_get_zero_arr_size(
_tmp45C,8));}));_LL2C9: if((int)_tmp45A != 4)goto _LL2CB;_LL2CA: return Cyc_PP_text(({
const char*_tmp45D="extern \"C\" ";_tag_dynforward(_tmp45D,sizeof(char),
_get_zero_arr_size(_tmp45D,12));}));_LL2CB: if((int)_tmp45A != 1)goto _LL2CD;_LL2CC:
return Cyc_PP_text(({const char*_tmp45E="abstract ";_tag_dynforward(_tmp45E,
sizeof(char),_get_zero_arr_size(_tmp45E,10));}));_LL2CD: if((int)_tmp45A != 5)goto
_LL2C2;_LL2CE: return Cyc_PP_text(({const char*_tmp45F="register ";_tag_dynforward(
_tmp45F,sizeof(char),_get_zero_arr_size(_tmp45F,10));}));_LL2C2:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp460=t;struct Cyc_Absyn_Tvar*_tmp461;struct Cyc_List_List*_tmp462;
_LL2D0: if(_tmp460 <= (void*)4)goto _LL2D4;if(*((int*)_tmp460)!= 1)goto _LL2D2;
_tmp461=((struct Cyc_Absyn_VarType_struct*)_tmp460)->f1;_LL2D1: return Cyc_Tcutil_is_temp_tvar(
_tmp461);_LL2D2: if(*((int*)_tmp460)!= 21)goto _LL2D4;_tmp462=((struct Cyc_Absyn_JoinEff_struct*)
_tmp460)->f1;_LL2D3: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp462);_LL2D4:;_LL2D5: return 0;_LL2CF:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp463=t;void**_tmp464;void*_tmp465;_LL2D7: if(_tmp463 <= (void*)4)goto
_LL2DD;if(*((int*)_tmp463)!= 11)goto _LL2D9;_LL2D8: return 1;_LL2D9: if(*((int*)
_tmp463)!= 13)goto _LL2DB;_LL2DA: return 1;_LL2DB: if(*((int*)_tmp463)!= 17)goto
_LL2DD;_tmp464=((struct Cyc_Absyn_TypedefType_struct*)_tmp463)->f4;if(_tmp464 == 0)
goto _LL2DD;_tmp465=*_tmp464;_LL2DC: return Cyc_Absynpp_is_anon_aggrtype(_tmp465);
_LL2DD:;_LL2DE: return 0;_LL2D6:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple5 _tmp467=({struct _tuple5 _tmp466;_tmp466.f1=(void*)tms->hd;_tmp466.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp466;});void*_tmp468;
void*_tmp469;_LL2E0: _tmp468=_tmp467.f1;if(*((int*)_tmp468)!= 2)goto _LL2E2;
_tmp469=_tmp467.f2;if(*((int*)_tmp469)!= 3)goto _LL2E2;_LL2E1: return({struct Cyc_List_List*
_tmp46A=_region_malloc(r,sizeof(*_tmp46A));_tmp46A->hd=(void*)((void*)tms->hd);
_tmp46A->tl=({struct Cyc_List_List*_tmp46B=_region_malloc(r,sizeof(*_tmp46B));
_tmp46B->hd=(void*)((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);
_tmp46B->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)
_check_null(tms->tl))->tl);_tmp46B;});_tmp46A;});_LL2E2:;_LL2E3: return({struct
Cyc_List_List*_tmp46C=_region_malloc(r,sizeof(*_tmp46C));_tmp46C->hd=(void*)atts;
_tmp46C->tl=tms;_tmp46C;});_LL2DF:;}else{return({struct Cyc_List_List*_tmp46D=
_region_malloc(r,sizeof(*_tmp46D));_tmp46D->hd=(void*)atts;_tmp46D->tl=tms;
_tmp46D;});}}struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp46E=t;struct Cyc_Absyn_ArrayInfo _tmp46F;void*_tmp470;struct
Cyc_Absyn_Tqual _tmp471;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Conref*
_tmp473;struct Cyc_Position_Segment*_tmp474;struct Cyc_Absyn_PtrInfo _tmp475;void*
_tmp476;struct Cyc_Absyn_Tqual _tmp477;struct Cyc_Absyn_PtrAtts _tmp478;struct Cyc_Absyn_FnInfo
_tmp479;struct Cyc_List_List*_tmp47A;struct Cyc_Core_Opt*_tmp47B;void*_tmp47C;
struct Cyc_List_List*_tmp47D;int _tmp47E;struct Cyc_Absyn_VarargInfo*_tmp47F;struct
Cyc_List_List*_tmp480;struct Cyc_List_List*_tmp481;struct Cyc_Core_Opt*_tmp482;
struct Cyc_Core_Opt*_tmp483;int _tmp484;struct _tuple0*_tmp485;struct Cyc_List_List*
_tmp486;void**_tmp487;_LL2E5: if(_tmp46E <= (void*)4)goto _LL2EF;if(*((int*)_tmp46E)
!= 7)goto _LL2E7;_tmp46F=((struct Cyc_Absyn_ArrayType_struct*)_tmp46E)->f1;_tmp470=(
void*)_tmp46F.elt_type;_tmp471=_tmp46F.tq;_tmp472=_tmp46F.num_elts;_tmp473=
_tmp46F.zero_term;_tmp474=_tmp46F.zt_loc;_LL2E6: {struct Cyc_Absyn_Tqual _tmp489;
void*_tmp48A;struct Cyc_List_List*_tmp48B;struct _tuple7 _tmp488=Cyc_Absynpp_to_tms(
r,_tmp471,_tmp470);_tmp489=_tmp488.f1;_tmp48A=_tmp488.f2;_tmp48B=_tmp488.f3;{
void*tm;if(_tmp472 == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp48C=
_region_malloc(r,sizeof(*_tmp48C));_tmp48C[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp48D;_tmp48D.tag=0;_tmp48D.f1=_tmp473;_tmp48D.f2=_tmp474;_tmp48D;});_tmp48C;});
else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp48E=_region_malloc(r,
sizeof(*_tmp48E));_tmp48E[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp48F;
_tmp48F.tag=1;_tmp48F.f1=(struct Cyc_Absyn_Exp*)_tmp472;_tmp48F.f2=_tmp473;
_tmp48F.f3=_tmp474;_tmp48F;});_tmp48E;});}return({struct _tuple7 _tmp490;_tmp490.f1=
_tmp489;_tmp490.f2=_tmp48A;_tmp490.f3=({struct Cyc_List_List*_tmp491=
_region_malloc(r,sizeof(*_tmp491));_tmp491->hd=(void*)tm;_tmp491->tl=_tmp48B;
_tmp491;});_tmp490;});}}_LL2E7: if(*((int*)_tmp46E)!= 4)goto _LL2E9;_tmp475=((
struct Cyc_Absyn_PointerType_struct*)_tmp46E)->f1;_tmp476=(void*)_tmp475.elt_typ;
_tmp477=_tmp475.elt_tq;_tmp478=_tmp475.ptr_atts;_LL2E8: {struct Cyc_Absyn_Tqual
_tmp493;void*_tmp494;struct Cyc_List_List*_tmp495;struct _tuple7 _tmp492=Cyc_Absynpp_to_tms(
r,_tmp477,_tmp476);_tmp493=_tmp492.f1;_tmp494=_tmp492.f2;_tmp495=_tmp492.f3;
_tmp495=({struct Cyc_List_List*_tmp496=_region_malloc(r,sizeof(*_tmp496));_tmp496->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp497=_region_malloc(r,
sizeof(*_tmp497));_tmp497[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp498;_tmp498.tag=
2;_tmp498.f1=_tmp478;_tmp498.f2=tq;_tmp498;});_tmp497;}));_tmp496->tl=_tmp495;
_tmp496;});return({struct _tuple7 _tmp499;_tmp499.f1=_tmp493;_tmp499.f2=_tmp494;
_tmp499.f3=_tmp495;_tmp499;});}_LL2E9: if(*((int*)_tmp46E)!= 8)goto _LL2EB;_tmp479=((
struct Cyc_Absyn_FnType_struct*)_tmp46E)->f1;_tmp47A=_tmp479.tvars;_tmp47B=
_tmp479.effect;_tmp47C=(void*)_tmp479.ret_typ;_tmp47D=_tmp479.args;_tmp47E=
_tmp479.c_varargs;_tmp47F=_tmp479.cyc_varargs;_tmp480=_tmp479.rgn_po;_tmp481=
_tmp479.attributes;_LL2EA: if(!Cyc_Absynpp_print_all_tvars){if(_tmp47B == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp47B->v)){_tmp47B=0;_tmp47A=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp47A);}{struct Cyc_Absyn_Tqual _tmp49B;void*
_tmp49C;struct Cyc_List_List*_tmp49D;struct _tuple7 _tmp49A=Cyc_Absynpp_to_tms(r,
Cyc_Absyn_empty_tqual(0),_tmp47C);_tmp49B=_tmp49A.f1;_tmp49C=_tmp49A.f2;_tmp49D=
_tmp49A.f3;{struct Cyc_List_List*tms=_tmp49D;if(_tmp481 != 0  && !Cyc_Absynpp_to_VC)
tms=Cyc_Absynpp_bubble_attributes(r,(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp49E=_region_malloc(r,sizeof(*_tmp49E));_tmp49E[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp49F;_tmp49F.tag=5;_tmp49F.f1=0;_tmp49F.f2=_tmp481;_tmp49F;});_tmp49E;}),tms);
tms=({struct Cyc_List_List*_tmp4A0=_region_malloc(r,sizeof(*_tmp4A0));_tmp4A0->hd=(
void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4A1=_region_malloc(r,
sizeof(*_tmp4A1));_tmp4A1[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4A2;
_tmp4A2.tag=3;_tmp4A2.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3));_tmp4A3[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4A4;_tmp4A4.tag=1;_tmp4A4.f1=_tmp47D;_tmp4A4.f2=_tmp47E;_tmp4A4.f3=_tmp47F;
_tmp4A4.f4=_tmp47B;_tmp4A4.f5=_tmp480;_tmp4A4;});_tmp4A3;}));_tmp4A2;});_tmp4A1;}));
_tmp4A0->tl=tms;_tmp4A0;});if(Cyc_Absynpp_to_VC)for(0;_tmp481 != 0;_tmp481=
_tmp481->tl){void*_tmp4A5=(void*)_tmp481->hd;_LL2F2: if((int)_tmp4A5 != 0)goto
_LL2F4;_LL2F3: goto _LL2F5;_LL2F4: if((int)_tmp4A5 != 1)goto _LL2F6;_LL2F5: goto _LL2F7;
_LL2F6: if((int)_tmp4A5 != 2)goto _LL2F8;_LL2F7: tms=({struct Cyc_List_List*_tmp4A6=
_region_malloc(r,sizeof(*_tmp4A6));_tmp4A6->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp4A7=_region_malloc(r,sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp4A8;_tmp4A8.tag=5;_tmp4A8.f1=0;_tmp4A8.f2=({struct Cyc_List_List*_tmp4A9=
_cycalloc(sizeof(*_tmp4A9));_tmp4A9->hd=(void*)((void*)_tmp481->hd);_tmp4A9->tl=
0;_tmp4A9;});_tmp4A8;});_tmp4A7;}));_tmp4A6->tl=tms;_tmp4A6;});goto AfterAtts;
_LL2F8:;_LL2F9: goto _LL2F1;_LL2F1:;}AfterAtts: if(_tmp47A != 0)tms=({struct Cyc_List_List*
_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA));_tmp4AA->hd=(void*)((void*)({struct
Cyc_Absyn_TypeParams_mod_struct*_tmp4AB=_region_malloc(r,sizeof(*_tmp4AB));
_tmp4AB[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4AC;_tmp4AC.tag=4;_tmp4AC.f1=
_tmp47A;_tmp4AC.f2=0;_tmp4AC.f3=1;_tmp4AC;});_tmp4AB;}));_tmp4AA->tl=tms;_tmp4AA;});
return({struct _tuple7 _tmp4AD;_tmp4AD.f1=_tmp49B;_tmp4AD.f2=_tmp49C;_tmp4AD.f3=
tms;_tmp4AD;});}}_LL2EB: if(*((int*)_tmp46E)!= 0)goto _LL2ED;_tmp482=((struct Cyc_Absyn_Evar_struct*)
_tmp46E)->f1;_tmp483=((struct Cyc_Absyn_Evar_struct*)_tmp46E)->f2;_tmp484=((
struct Cyc_Absyn_Evar_struct*)_tmp46E)->f3;_LL2EC: if(_tmp483 == 0)return({struct
_tuple7 _tmp4AE;_tmp4AE.f1=tq;_tmp4AE.f2=t;_tmp4AE.f3=0;_tmp4AE;});else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp483->v);}_LL2ED: if(*((int*)_tmp46E)!= 17)goto _LL2EF;_tmp485=((
struct Cyc_Absyn_TypedefType_struct*)_tmp46E)->f1;_tmp486=((struct Cyc_Absyn_TypedefType_struct*)
_tmp46E)->f2;_tmp487=((struct Cyc_Absyn_TypedefType_struct*)_tmp46E)->f4;_LL2EE:
if((_tmp487 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp487))return({struct _tuple7 _tmp4AF;_tmp4AF.f1=tq;_tmp4AF.f2=t;_tmp4AF.f3=0;
_tmp4AF;});else{return Cyc_Absynpp_to_tms(r,tq,*_tmp487);}_LL2EF:;_LL2F0: return({
struct _tuple7 _tmp4B0;_tmp4B0.f1=tq;_tmp4B0.f2=t;_tmp4B0.f3=0;_tmp4B0;});_LL2E4:;}
static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp4B1=t;struct Cyc_Core_Opt*
_tmp4B2;struct Cyc_Core_Opt _tmp4B3;void*_tmp4B4;struct Cyc_Absyn_PtrInfo _tmp4B5;
void*_tmp4B6;_LL2FB: if(_tmp4B1 <= (void*)4)goto _LL2FF;if(*((int*)_tmp4B1)!= 0)
goto _LL2FD;_tmp4B2=((struct Cyc_Absyn_Evar_struct*)_tmp4B1)->f2;if(_tmp4B2 == 0)
goto _LL2FD;_tmp4B3=*_tmp4B2;_tmp4B4=(void*)_tmp4B3.v;_LL2FC: return Cyc_Absynpp_is_char_ptr(
_tmp4B4);_LL2FD: if(*((int*)_tmp4B1)!= 4)goto _LL2FF;_tmp4B5=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B1)->f1;_tmp4B6=(void*)_tmp4B5.elt_typ;_LL2FE: L: {void*_tmp4B7=_tmp4B6;
struct Cyc_Core_Opt*_tmp4B8;struct Cyc_Core_Opt _tmp4B9;void*_tmp4BA;void**_tmp4BB;
void*_tmp4BC;void*_tmp4BD;_LL302: if(_tmp4B7 <= (void*)4)goto _LL308;if(*((int*)
_tmp4B7)!= 0)goto _LL304;_tmp4B8=((struct Cyc_Absyn_Evar_struct*)_tmp4B7)->f2;if(
_tmp4B8 == 0)goto _LL304;_tmp4B9=*_tmp4B8;_tmp4BA=(void*)_tmp4B9.v;_LL303: _tmp4B6=
_tmp4BA;goto L;_LL304: if(*((int*)_tmp4B7)!= 17)goto _LL306;_tmp4BB=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4B7)->f4;if(_tmp4BB == 0)goto _LL306;_tmp4BC=*_tmp4BB;_LL305: _tmp4B6=_tmp4BC;
goto L;_LL306: if(*((int*)_tmp4B7)!= 5)goto _LL308;_tmp4BD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B7)->f2;if((int)_tmp4BD != 0)goto _LL308;_LL307: return 1;_LL308:;_LL309: return
0;_LL301:;}_LL2FF:;_LL300: return 0;_LL2FA:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4BE=_new_region("temp");struct _RegionHandle*temp=& _tmp4BE;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4C0;void*_tmp4C1;struct Cyc_List_List*_tmp4C2;struct
_tuple7 _tmp4BF=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4C0=_tmp4BF.f1;_tmp4C1=
_tmp4BF.f2;_tmp4C2=_tmp4BF.f3;_tmp4C2=Cyc_List_imp_rev(_tmp4C2);if(_tmp4C2 == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp4C4=({struct Cyc_PP_Doc*_tmp4C3[2];_tmp4C3[1]=
Cyc_Absynpp_ntyp2doc(_tmp4C1);_tmp4C3[0]=Cyc_Absynpp_tqual2doc(_tmp4C0);Cyc_PP_cat(
_tag_dynforward(_tmp4C3,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return
_tmp4C4;}else{struct Cyc_PP_Doc*_tmp4C7=({struct Cyc_PP_Doc*_tmp4C5[4];_tmp4C5[3]=
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp4C2);_tmp4C5[2]=Cyc_PP_text(({const char*_tmp4C6=" ";
_tag_dynforward(_tmp4C6,sizeof(char),_get_zero_arr_size(_tmp4C6,2));}));_tmp4C5[
1]=Cyc_Absynpp_ntyp2doc(_tmp4C1);_tmp4C5[0]=Cyc_Absynpp_tqual2doc(_tmp4C0);Cyc_PP_cat(
_tag_dynforward(_tmp4C5,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return
_tmp4C7;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){if(f->width != 0)return({struct Cyc_PP_Doc*_tmp4C8[5];
_tmp4C8[4]=Cyc_PP_text(({const char*_tmp4CB=";";_tag_dynforward(_tmp4CB,sizeof(
char),_get_zero_arr_size(_tmp4CB,2));}));_tmp4C8[3]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4C8[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));
_tmp4C8[1]=Cyc_PP_text(({const char*_tmp4CA=":";_tag_dynforward(_tmp4CA,sizeof(
char),_get_zero_arr_size(_tmp4CA,2));}));_tmp4C8[0]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->v=
Cyc_PP_textptr(f->name);_tmp4C9;}));Cyc_PP_cat(_tag_dynforward(_tmp4C8,sizeof(
struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp4CC[3];_tmp4CC[2]=
Cyc_PP_text(({const char*_tmp4CE=";";_tag_dynforward(_tmp4CE,sizeof(char),
_get_zero_arr_size(_tmp4CE,2));}));_tmp4CC[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4CC[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD->v=Cyc_PP_textptr(f->name);_tmp4CD;}));
Cyc_PP_cat(_tag_dynforward(_tmp4CC,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*
_tmp4CF="";_tag_dynforward(_tmp4CF,sizeof(char),_get_zero_arr_size(_tmp4CF,1));}),
fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f){return({struct Cyc_PP_Doc*_tmp4D0[3];_tmp4D0[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs);_tmp4D0[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp4D0[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_dynforward(
_tmp4D0,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _dynforward_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,({const char*_tmp4D1=",";
_tag_dynforward(_tmp4D1,sizeof(char),_get_zero_arr_size(_tmp4D1,2));}),fields);}
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp4D2[0]={};Cyc_fprintf(f,({const char*_tmp4D3="\n";
_tag_dynforward(_tmp4D3,sizeof(char),_get_zero_arr_size(_tmp4D3,2));}),
_tag_dynforward(_tmp4D2,sizeof(void*),0));});}}struct _dynforward_ptr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*
_tmp4D4="";_tag_dynforward(_tmp4D4,sizeof(char),_get_zero_arr_size(_tmp4D4,1));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc(s),72);}struct _dynforward_ptr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dynforward_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dynforward_ptr Cyc_Absynpp_prim2string(void*p){
return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
