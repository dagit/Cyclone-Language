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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);void*Cyc_List_hd(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern char Cyc_List_Nth[8];int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*
l,void*x);struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct
Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}
__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
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
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict
d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2);int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;int Cyc_Position_segment_equals(
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);struct _dynforward_ptr
Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);struct Cyc_Position_Error{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*
t);int Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessSize_struct
LessSize;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqSize_struct
LessEqSize;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
15]="\000\000\000\000HasTagCmps\000";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero\000";
char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ\000";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct _dynforward_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_List_List*,void*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_empty_flowdict();
struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Dict_Dict
Cyc_CfFlowInfo_mt_place_set();int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict
s2);int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict
s2);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict consumed;struct Cyc_List_List*
may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_this;
extern void*Cyc_CfFlowInfo_unknown_all;extern void*Cyc_CfFlowInfo_esc_none;extern
void*Cyc_CfFlowInfo_esc_this;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);void Cyc_CfFlowInfo_print_absrval(
void*rval);void Cyc_CfFlowInfo_print_initlevel(void*il);void Cyc_CfFlowInfo_print_root(
void*root);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place_set(
struct Cyc_Dict_Dict p);void Cyc_CfFlowInfo_print_place_list(struct Cyc_List_List*p);
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo_union f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*);void Cyc_CfFlowInfo_print_relns(struct
Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_join_flow(struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple4{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple4 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Dict_Dict*
all_changed,struct _tuple4 pr1,struct _tuple4 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(union Cyc_CfFlowInfo_FlowInfo_union
f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;
};int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);struct _tuple5{struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple5 Cyc_CfFlowInfo_save_consume_info(union Cyc_CfFlowInfo_FlowInfo_union
f,int clear);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_CfFlowInfo_ConsumeInfo c);struct
_dynforward_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={0,(void*)((
void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_all_v={
0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_none_v={
1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_this_v={
1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_all_v={
1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(void*)& Cyc_CfFlowInfo_unknown_none_v;
void*Cyc_CfFlowInfo_unknown_this=(void*)& Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(
void*)& Cyc_CfFlowInfo_unknown_all_v;void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;
void*Cyc_CfFlowInfo_esc_this=(void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(
void*)& Cyc_CfFlowInfo_esc_all_v;struct Cyc_Dict_Dict Cyc_CfFlowInfo_mt_place_set(){
static struct Cyc_Dict_Dict*mt_place_set_opt=0;if(mt_place_set_opt == 0)
mt_place_set_opt=({struct Cyc_Dict_Dict*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_empty)(Cyc_CfFlowInfo_place_cmp);_tmp6;});return*mt_place_set_opt;}int
Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(*set,place))return 1;else{*set=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k,struct
Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct
_dynforward_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp8;void*_tmp9;struct Cyc_CfFlowInfo_Place*_tmp7=
place;_tmp8=*_tmp7;_tmp9=(void*)_tmp8.root;{void*_tmpA=_tmp9;struct Cyc_Absyn_Vardecl*
_tmpB;_LL1: if(*((int*)_tmpA)!= 0)goto _LL3;_tmpB=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpA)->f1;_LL2: return Cyc_Absynpp_qvar2string(_tmpB->name);_LL3:;_LL4:({void*
_tmpC[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpD="error locations not for VarRoots";_tag_dynforward(_tmpD,sizeof(
char),_get_zero_arr_size(_tmpD,33));}),_tag_dynforward(_tmpC,sizeof(void*),0));});
_LL0:;}}static void Cyc_CfFlowInfo_unique_err(struct Cyc_CfFlowInfo_Place*place,
struct _dynforward_ptr err_msg1,struct _dynforward_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc){struct _dynforward_ptr _tmpE=Cyc_CfFlowInfo_place_err_string(
place);if(!Cyc_Position_segment_equals(consumed_loc,loc)){struct _dynforward_ptr
_tmpF=Cyc_Position_string_of_segment(consumed_loc);({struct Cyc_String_pa_struct
_tmp12;_tmp12.tag=0;_tmp12.f1=(struct _dynforward_ptr)_tmpF;{struct Cyc_String_pa_struct
_tmp11;_tmp11.tag=0;_tmp11.f1=(struct _dynforward_ptr)_tmpE;{void*_tmp10[2]={&
_tmp11,& _tmp12};Cyc_Tcutil_terr(loc,err_msg1,_tag_dynforward(_tmp10,sizeof(void*),
2));}}});}else{({struct Cyc_String_pa_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct
_dynforward_ptr)_tmpE;{void*_tmp13[1]={& _tmp14};Cyc_Tcutil_terr(loc,err_msg2,
_tag_dynforward(_tmp13,sizeof(void*),1));}});}}static struct Cyc_Position_Segment*
Cyc_CfFlowInfo_combine_consume_f(int isErr,struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2){if(isErr)Cyc_CfFlowInfo_unique_err(
place,({const char*_tmp15="May consume unique pointer %s more than once (cf. %s)";
_tag_dynforward(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,54));}),({const
char*_tmp16="May consume unique pointer %s more than once";_tag_dynforward(
_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,45));}),loc1,loc2);return loc1;}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct
Cyc_Dict_Dict s2,int disjoint){struct Cyc_Dict_Dict _tmp17=((struct Cyc_Dict_Dict(*)(
struct Cyc_Position_Segment*(*f)(int,struct Cyc_CfFlowInfo_Place*,struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*),int env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_combine_consume_f,disjoint,s1,s2);return
_tmp17;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);struct
_tuple6{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if(s1.t == s2.t)return 1;{struct
_handler_cons _tmp18;_push_handler(& _tmp18);{int _tmp1A=0;if(setjmp(_tmp18.handler))
_tmp1A=1;if(!_tmp1A){{struct _RegionHandle _tmp1B=_new_region("r");struct
_RegionHandle*r=& _tmp1B;_push_region(r);{struct Cyc_Iter_Iter _tmp1C=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,s1);struct
_tuple6 _tmp1D=*((struct _tuple6*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))
Cyc_Dict_rchoose)(r,s1);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple6*))Cyc_Iter_next)(
_tmp1C,& _tmp1D)){struct Cyc_CfFlowInfo_Place*_tmp1E=_tmp1D.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(s2,_tmp1E)){int
_tmp1F=0;_npop_handler(1);return _tmp1F;}}{int _tmp20=1;_npop_handler(1);return
_tmp20;}};_pop_region(r);};_pop_handler();}else{void*_tmp19=(void*)_exn_thrown;
void*_tmp22=_tmp19;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp22);_LL5:;}}}}
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{
struct _tuple0 _tmp24=({struct _tuple0 _tmp23;_tmp23.f1=r1;_tmp23.f2=r2;_tmp23;});
void*_tmp25;struct Cyc_Absyn_Vardecl*_tmp26;void*_tmp27;struct Cyc_Absyn_Vardecl*
_tmp28;void*_tmp29;void*_tmp2A;void*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;void*
_tmp2D;struct Cyc_Absyn_Exp*_tmp2E;void*_tmp2F;void*_tmp30;void*_tmp31;int _tmp32;
void*_tmp33;int _tmp34;_LLB: _tmp25=_tmp24.f1;if(*((int*)_tmp25)!= 0)goto _LLD;
_tmp26=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp25)->f1;_tmp27=_tmp24.f2;if(*((
int*)_tmp27)!= 0)goto _LLD;_tmp28=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp27)->f1;
_LLC: return(int)_tmp26 - (int)_tmp28;_LLD: _tmp29=_tmp24.f1;if(*((int*)_tmp29)!= 0)
goto _LLF;_LLE: return - 1;_LLF: _tmp2A=_tmp24.f2;if(*((int*)_tmp2A)!= 0)goto _LL11;
_LL10: return 1;_LL11: _tmp2B=_tmp24.f1;if(*((int*)_tmp2B)!= 1)goto _LL13;_tmp2C=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2B)->f1;_tmp2D=_tmp24.f2;if(*((int*)
_tmp2D)!= 1)goto _LL13;_tmp2E=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2D)->f1;
_LL12: return(int)_tmp2C - (int)_tmp2E;_LL13: _tmp2F=_tmp24.f1;if(*((int*)_tmp2F)!= 
1)goto _LL15;_LL14: return - 1;_LL15: _tmp30=_tmp24.f2;if(*((int*)_tmp30)!= 1)goto
_LL17;_LL16: return 1;_LL17: _tmp31=_tmp24.f1;if(*((int*)_tmp31)!= 2)goto _LLA;
_tmp32=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp31)->f1;_tmp33=_tmp24.f2;if(*((
int*)_tmp33)!= 2)goto _LLA;_tmp34=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp33)->f1;
_LL18: return _tmp32 - _tmp34;_LLA:;}}struct Cyc_Dict_Dict Cyc_CfFlowInfo_empty_flowdict(){
static struct Cyc_Dict_Dict*fd=0;if(fd == 0)fd=({struct Cyc_Dict_Dict*_tmp35=
_cycalloc(sizeof(*_tmp35));_tmp35[0]=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);
_tmp35;});return*fd;}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,
struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{int i=Cyc_CfFlowInfo_root_cmp((
void*)p1->root,(void*)p2->root);if(i != 0)return i;return((int(*)(int(*cmp)(int,
int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,
p1->fields,p2->fields);}}static struct _dynforward_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;{void*_tmp36=(void*)p->root;
struct Cyc_Absyn_Vardecl*_tmp37;struct Cyc_Absyn_Exp*_tmp38;int _tmp39;_LL1A: if(*((
int*)_tmp36)!= 0)goto _LL1C;_tmp37=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp36)->f1;
_LL1B: sl=({struct Cyc_List_List*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->hd=({
struct _dynforward_ptr*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_String_pa_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
_tmp37->name).f2);{void*_tmp3C[1]={& _tmp3E};Cyc_aprintf(({const char*_tmp3D="%s";
_tag_dynforward(_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,3));}),
_tag_dynforward(_tmp3C,sizeof(void*),1));}});_tmp3B;});_tmp3A->tl=sl;_tmp3A;});
goto _LL19;_LL1C: if(*((int*)_tmp36)!= 1)goto _LL1E;_tmp38=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp36)->f1;_LL1D: sl=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));
_tmp3F->hd=({struct _dynforward_ptr*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40[0]=({
struct Cyc_Int_pa_struct _tmp43;_tmp43.tag=1;_tmp43.f1=(unsigned long)((int)_tmp38);{
void*_tmp41[1]={& _tmp43};Cyc_aprintf(({const char*_tmp42="mpt%d";_tag_dynforward(
_tmp42,sizeof(char),_get_zero_arr_size(_tmp42,6));}),_tag_dynforward(_tmp41,
sizeof(void*),1));}});_tmp40;});_tmp3F->tl=sl;_tmp3F;});goto _LL19;_LL1E: if(*((
int*)_tmp36)!= 2)goto _LL19;_tmp39=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp36)->f1;_LL1F: sl=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));
_tmp44->hd=({struct _dynforward_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({
struct Cyc_Int_pa_struct _tmp48;_tmp48.tag=1;_tmp48.f1=(unsigned long)_tmp39;{void*
_tmp46[1]={& _tmp48};Cyc_aprintf(({const char*_tmp47="param%d";_tag_dynforward(
_tmp47,sizeof(char),_get_zero_arr_size(_tmp47,8));}),_tag_dynforward(_tmp46,
sizeof(void*),1));}});_tmp45;});_tmp44->tl=sl;_tmp44;});goto _LL19;_LL19:;}{
struct Cyc_List_List*fields=p->fields;for(0;fields != 0;fields=fields->tl){sl=({
struct Cyc_List_List*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->hd=({struct
_dynforward_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Int_pa_struct
_tmp4D;_tmp4D.tag=1;_tmp4D.f1=(unsigned long)((int)fields->hd);{void*_tmp4B[1]={&
_tmp4D};Cyc_aprintf(({const char*_tmp4C="%d";_tag_dynforward(_tmp4C,sizeof(char),
_get_zero_arr_size(_tmp4C,3));}),_tag_dynforward(_tmp4B,sizeof(void*),1));}});
_tmp4A;});_tmp49->tl=sl;_tmp49;});}}{struct _dynforward_ptr*_tmp4E=({struct
_dynforward_ptr*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_String_pa_struct
_tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dynforward_ptr)({const char*_tmp57="";
_tag_dynforward(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,1));});{void*
_tmp54[1]={& _tmp56};Cyc_aprintf(({const char*_tmp55="%s";_tag_dynforward(_tmp55,
sizeof(char),_get_zero_arr_size(_tmp55,3));}),_tag_dynforward(_tmp54,sizeof(void*),
1));}});_tmp53;});for(0;sl != 0;sl=sl->tl){*_tmp4E=({struct Cyc_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp4E);{struct Cyc_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)sl->hd));{void*
_tmp4F[2]={& _tmp51,& _tmp52};Cyc_aprintf(({const char*_tmp50="%s.%s";
_tag_dynforward(_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,6));}),
_tag_dynforward(_tmp4F,sizeof(void*),2));}}});}return _tmp4E;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
int allow_zeroterm,void*t,void*leafval);struct _dynforward_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_List_List*fs,void*leafval){unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(fs);struct _dynforward_ptr d=({
unsigned int _tmp5C=sz;void**_tmp5D=(void**)_cycalloc(_check_times(sizeof(void*),
_tmp5C));struct _dynforward_ptr _tmp5F=_tag_dynforward(_tmp5D,sizeof(void*),_tmp5C);{
unsigned int _tmp5E=_tmp5C;unsigned int i;for(i=0;i < _tmp5E;i ++){_tmp5D[i]=Cyc_CfFlowInfo_unknown_all;}}
_tmp5F;});{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){struct Cyc_Absyn_Aggrfield _tmp59;
struct _dynforward_ptr*_tmp5A;void*_tmp5B;struct Cyc_Absyn_Aggrfield*_tmp58=(
struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmp59=*
_tmp58;_tmp5A=_tmp59.name;_tmp5B=(void*)_tmp59.type;if(_get_dynforward_size(*
_tmp5A,sizeof(char))!= 1)*((void**)_check_dynforward_subscript(d,sizeof(void*),i))=
Cyc_CfFlowInfo_i_typ_to_absrval(0,_tmp5B,leafval);}}return d;}struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(int
allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_union_type(t))return Cyc_CfFlowInfo_unknown_all;{
void*_tmp60=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo _tmp61;union
Cyc_Absyn_TunionFieldInfoU_union _tmp62;struct Cyc_Absyn_Tunionfield*_tmp63;struct
Cyc_List_List*_tmp64;struct Cyc_Absyn_AggrInfo _tmp65;union Cyc_Absyn_AggrInfoU_union
_tmp66;void*_tmp67;struct Cyc_List_List*_tmp68;struct Cyc_Absyn_ArrayInfo _tmp69;
void*_tmp6A;struct Cyc_Absyn_Conref*_tmp6B;void*_tmp6C;struct Cyc_Absyn_PtrInfo
_tmp6D;struct Cyc_Absyn_PtrAtts _tmp6E;struct Cyc_Absyn_Conref*_tmp6F;_LL21: if(
_tmp60 <= (void*)4)goto _LL2F;if(*((int*)_tmp60)!= 3)goto _LL23;_tmp61=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp60)->f1;_tmp62=_tmp61.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp60)->f1).field_info).KnownTunionfield).tag != 1)goto _LL23;_tmp63=(_tmp62.KnownTunionfield).f2;
_LL22: if(_tmp63->typs == 0)return leafval;_tmp64=_tmp63->typs;goto _LL24;_LL23: if(*((
int*)_tmp60)!= 9)goto _LL25;_tmp64=((struct Cyc_Absyn_TupleType_struct*)_tmp60)->f1;
_LL24: {unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp64);struct _dynforward_ptr d=({unsigned int _tmp72=sz;void**_tmp73=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp72));struct _dynforward_ptr _tmp75=
_tag_dynforward(_tmp73,sizeof(void*),_tmp72);{unsigned int _tmp74=_tmp72;
unsigned int i;for(i=0;i < _tmp74;i ++){_tmp73[i]=Cyc_CfFlowInfo_unknown_all;}}
_tmp75;});{int i=0;for(0;i < sz;i ++){*((void**)_check_dynforward_subscript(d,
sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(0,(*((struct _tuple7*)((struct
Cyc_List_List*)_check_null(_tmp64))->hd)).f2,leafval);_tmp64=_tmp64->tl;}}return(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp70=_cycalloc(sizeof(*_tmp70));
_tmp70[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp71;_tmp71.tag=4;_tmp71.f1=d;
_tmp71;});_tmp70;});}_LL25: if(*((int*)_tmp60)!= 10)goto _LL27;_tmp65=((struct Cyc_Absyn_AggrType_struct*)
_tmp60)->f1;_tmp66=_tmp65.aggr_info;_LL26: {struct Cyc_Absyn_Aggrdecl*_tmp76=Cyc_Absyn_get_known_aggrdecl(
_tmp66);if(_tmp76->impl == 0)goto _LL20;_tmp68=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp76->impl))->fields;goto _LL28;}_LL27: if(*((int*)_tmp60)!= 11)goto
_LL29;_tmp67=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp60)->f1;if((int)
_tmp67 != 0)goto _LL29;_tmp68=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp60)->f2;
_LL28: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp77=_cycalloc(
sizeof(*_tmp77));_tmp77[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp78;_tmp78.tag=
4;_tmp78.f1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp68,leafval);_tmp78;});
_tmp77;});_LL29: if(*((int*)_tmp60)!= 7)goto _LL2B;_tmp69=((struct Cyc_Absyn_ArrayType_struct*)
_tmp60)->f1;_tmp6A=(void*)_tmp69.elt_type;_tmp6B=_tmp69.zero_term;if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp6B))goto _LL2B;_LL2A:
return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmp6A)?Cyc_CfFlowInfo_unknown_all:
leafval;_LL2B: if(*((int*)_tmp60)!= 18)goto _LL2D;_tmp6C=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp60)->f1;_LL2C: {void*_tmp79=leafval;void*_tmp7A;void*_tmp7B;_LL32: if(_tmp79
<= (void*)3)goto _LL36;if(*((int*)_tmp79)!= 0)goto _LL34;_tmp7A=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp79)->f1;if((int)_tmp7A != 2)goto _LL34;_LL33: goto _LL35;_LL34: if(*((int*)_tmp79)
!= 1)goto _LL36;_tmp7B=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp79)->f1;if((
int)_tmp7B != 2)goto _LL36;_LL35: goto _LL37;_LL36: if((int)_tmp79 != 0)goto _LL38;
_LL37: goto _LL39;_LL38: if((int)_tmp79 != 1)goto _LL3A;_LL39: return(void*)({struct
Cyc_CfFlowInfo_TagCmps_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({
struct Cyc_CfFlowInfo_TagCmps_struct _tmp7D;_tmp7D.tag=3;_tmp7D.f1=({struct Cyc_CfFlowInfo_TagCmp*
_tmp7E[1];_tmp7E[0]=({struct Cyc_CfFlowInfo_TagCmp*_tmp7F=_cycalloc(sizeof(*
_tmp7F));_tmp7F->cmp=(void*)((void*)5);_tmp7F->bd=(void*)_tmp6C;_tmp7F;});((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp7E,sizeof(struct Cyc_CfFlowInfo_TagCmp*),1));});_tmp7D;});_tmp7C;});_LL3A:;
_LL3B: return leafval;_LL31:;}_LL2D: if(*((int*)_tmp60)!= 4)goto _LL2F;_tmp6D=((
struct Cyc_Absyn_PointerType_struct*)_tmp60)->f1;_tmp6E=_tmp6D.ptr_atts;_tmp6F=
_tmp6E.nullable;if(!(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp6F)))goto _LL2F;_LL2E:{void*_tmp80=leafval;void*_tmp81;void*_tmp82;_LL3D:
if(_tmp80 <= (void*)3)goto _LL41;if(*((int*)_tmp80)!= 0)goto _LL3F;_tmp81=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp80)->f1;if((int)_tmp81 != 1)goto _LL3F;
_LL3E: return(void*)2;_LL3F: if(*((int*)_tmp80)!= 0)goto _LL41;_tmp82=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp80)->f1;if((int)_tmp82 != 2)goto _LL41;
_LL40: return(void*)1;_LL41:;_LL42: goto _LL3C;_LL3C:;}goto _LL20;_LL2F:;_LL30: goto
_LL20;_LL20:;}return Cyc_Tcutil_bits_only(t)?Cyc_CfFlowInfo_unknown_all: leafval;}
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
1,t,leafval);}static int Cyc_CfFlowInfo_prefix_of_member(struct _RegionHandle*r,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _tuple6 _tmp83=({
struct _tuple6 _tmp88;_tmp88.f1=place;_tmp88.f2=0;_tmp88;});struct Cyc_Iter_Iter
_tmp84=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))
Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple6*))
Cyc_Iter_next)(_tmp84,& _tmp83)){struct Cyc_CfFlowInfo_Place*_tmp85=_tmp83.f1;if(
Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp85->root)!= 0)continue;{
struct Cyc_List_List*_tmp86=place->fields;struct Cyc_List_List*_tmp87=_tmp85->fields;
for(0;_tmp86 != 0  && _tmp87 != 0;(_tmp86=_tmp86->tl,_tmp87=_tmp87->tl)){if((int)
_tmp86->hd != (int)_tmp87->hd)break;}if(_tmp86 == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{
struct _RegionHandle*rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,
place))pile->places=({struct Cyc_List_List*_tmp89=_region_malloc(pile->rgn,
sizeof(*_tmp89));_tmp89->hd=place;_tmp89->tl=pile->places;_tmp89;});}static void
Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmp8A=
r;struct Cyc_CfFlowInfo_Place*_tmp8B;struct _dynforward_ptr _tmp8C;_LL44: if(_tmp8A
<= (void*)3)goto _LL48;if(*((int*)_tmp8A)!= 2)goto _LL46;_tmp8B=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp8A)->f1;_LL45: Cyc_CfFlowInfo_add_place(pile,_tmp8B);return;_LL46: if(*((int*)
_tmp8A)!= 4)goto _LL48;_tmp8C=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp8A)->f1;
_LL47:{int i=0;for(0;i < _get_dynforward_size(_tmp8C,sizeof(void*));i ++){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmp8C.curr)[i]);}}return;_LL48:;_LL49: return;_LL43:;}static void*
Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){void*_tmp8D=old_val;
struct _dynforward_ptr _tmp8E;_LL4B: if(_tmp8D <= (void*)3)goto _LL4D;if(*((int*)
_tmp8D)!= 4)goto _LL4D;_tmp8E=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp8D)->f1;
_LL4C: {struct _dynforward_ptr d2=({unsigned int _tmp91=_get_dynforward_size(_tmp8E,
sizeof(void*));void**_tmp92=(void**)_cycalloc(_check_times(sizeof(void*),_tmp91));
struct _dynforward_ptr _tmp94=_tag_dynforward(_tmp92,sizeof(void*),_tmp91);{
unsigned int _tmp93=_tmp91;unsigned int i;for(i=0;i < _tmp93;i ++){_tmp92[i]=Cyc_CfFlowInfo_insert_place_inner(
new_val,((void**)_tmp8E.curr)[(int)i]);}}_tmp94;});return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp90;_tmp90.tag=4;_tmp90.f1=d2;_tmp90;});_tmp8F;});}_LL4D:;_LL4E: return new_val;
_LL4A:;}static struct _dynforward_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_dynforward_ptr d,int n,void*r){void*_tmp95=*((void**)_check_dynforward_subscript(
d,sizeof(void*),n));if(_tmp95 == r)return d;{struct _dynforward_ptr res=({
unsigned int _tmp96=_get_dynforward_size(d,sizeof(void*));void**_tmp97=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp96));struct _dynforward_ptr _tmp99=
_tag_dynforward(_tmp97,sizeof(void*),_tmp96);{unsigned int _tmp98=_tmp96;
unsigned int i;for(i=0;i < _tmp98;i ++){_tmp97[i]=((void**)d.curr)[(int)i];}}_tmp99;});*((
void**)_check_dynforward_subscript(res,sizeof(void*),n))=r;return res;}}struct
_tuple8{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple8 _tmp9B=({struct _tuple8 _tmp9A;_tmp9A.f1=fs;_tmp9A.f2=
old_val;_tmp9A;});struct Cyc_List_List*_tmp9C;struct Cyc_List_List _tmp9D;int _tmp9E;
struct Cyc_List_List*_tmp9F;void*_tmpA0;struct _dynforward_ptr _tmpA1;_LL50: _tmp9C=
_tmp9B.f1;if(_tmp9C == 0)goto _LL52;_tmp9D=*_tmp9C;_tmp9E=(int)_tmp9D.hd;_tmp9F=
_tmp9D.tl;_tmpA0=_tmp9B.f2;if(_tmpA0 <= (void*)3)goto _LL52;if(*((int*)_tmpA0)!= 4)
goto _LL52;_tmpA1=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpA0)->f1;_LL51: {
void*_tmpA2=Cyc_CfFlowInfo_insert_place_outer(_tmp9F,*((void**)
_check_dynforward_subscript(_tmpA1,sizeof(void*),_tmp9E)),new_val);struct
_dynforward_ptr _tmpA3=Cyc_CfFlowInfo_aggr_dict_insert(_tmpA1,_tmp9E,_tmpA2);if(
_tmpA3.curr == _tmpA1.curr)return old_val;else{return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpA5;_tmpA5.tag=4;_tmpA5.f1=_tmpA3;_tmpA5;});_tmpA4;});}}_LL52:;_LL53:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));
_tmpA6[0]=({struct Cyc_Core_Impossible_struct _tmpA7;_tmpA7.tag=Cyc_Core_Impossible;
_tmpA7.f1=({const char*_tmpA8="bad insert place";_tag_dynforward(_tmpA8,sizeof(
char),_get_zero_arr_size(_tmpA8,17));});_tmpA7;});_tmpA6;}));_LL4F:;}}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmpA9=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)
_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)_check_null(
pile->places))->tl;Cyc_CfFlowInfo_update_place_set(all_changed,_tmpA9,0);{void*
oldval;void*newval;{struct _handler_cons _tmpAA;_push_handler(& _tmpAA);{int _tmpAC=
0;if(setjmp(_tmpAA.handler))_tmpAC=1;if(!_tmpAC){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmpA9);;_pop_handler();}else{void*_tmpAB=(void*)_exn_thrown;void*_tmpAE=
_tmpAB;_LL55: if(_tmpAE != Cyc_Dict_Absent)goto _LL57;_LL56: continue;_LL57:;_LL58:(
void)_throw(_tmpAE);_LL54:;}}}{void*_tmpAF=Cyc_CfFlowInfo_initlevel(d,oldval);
_LL5A: if((int)_tmpAF != 2)goto _LL5C;_LL5B: newval=Cyc_CfFlowInfo_esc_all;goto _LL59;
_LL5C: if((int)_tmpAF != 1)goto _LL5E;_LL5D: newval=Cyc_CfFlowInfo_esc_this;goto
_LL59;_LL5E: if((int)_tmpAF != 0)goto _LL59;_LL5F: newval=Cyc_CfFlowInfo_esc_none;
goto _LL59;_LL59:;}Cyc_CfFlowInfo_add_places(pile,oldval);d=Cyc_Dict_insert(d,(
void*)_tmpA9->root,Cyc_CfFlowInfo_insert_place_outer(_tmpA9->fields,Cyc_Dict_lookup(
d,(void*)_tmpA9->root),newval));}}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{
struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static void*Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmpB0=r;void*_tmpB1;void*_tmpB2;_LL61: if(_tmpB0 <= (void*)3)goto
_LL65;if(*((int*)_tmpB0)!= 0)goto _LL63;_tmpB1=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpB0)->f1;_LL62: return _tmpB1;_LL63: if(*((int*)_tmpB0)!= 1)goto _LL65;_tmpB2=(
void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpB0)->f1;_LL64: return _tmpB2;_LL65:
if((int)_tmpB0 != 0)goto _LL67;_LL66: goto _LL68;_LL67: if((int)_tmpB0 != 1)goto _LL69;
_LL68: return(void*)2;_LL69: if((int)_tmpB0 != 2)goto _LL6B;_LL6A: return(void*)1;
_LL6B: if(_tmpB0 <= (void*)3)goto _LL6D;if(*((int*)_tmpB0)!= 3)goto _LL6D;_LL6C:
return(void*)2;_LL6D:;_LL6E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Core_Impossible_struct
_tmpB4;_tmpB4.tag=Cyc_Core_Impossible;_tmpB4.f1=({const char*_tmpB5="initlevel_approx";
_tag_dynforward(_tmpB5,sizeof(char),_get_zero_arr_size(_tmpB5,17));});_tmpB4;});
_tmpB3;}));_LL60:;}static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*_tmpB6=
r;struct _dynforward_ptr _tmpB7;struct Cyc_CfFlowInfo_Place*_tmpB8;_LL70: if(_tmpB6
<= (void*)3)goto _LL74;if(*((int*)_tmpB6)!= 4)goto _LL72;_tmpB7=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpB6)->f1;_LL71: {unsigned int _tmpB9=_get_dynforward_size(_tmpB7,sizeof(void*));
this_ans=(void*)2;{int i=0;for(0;i < _tmpB9;i ++){this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dynforward_subscript(_tmpB7,sizeof(void*),i)),this_ans);}}
goto _LL6F;}_LL72: if(*((int*)_tmpB6)!= 2)goto _LL74;_tmpB8=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpB6)->f1;_LL73: if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(
Cyc_CfFlowInfo_place_cmp,env->seen,_tmpB8))this_ans=(void*)2;else{env->seen=({
struct Cyc_List_List*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->hd=_tmpB8;_tmpBA->tl=
env->seen;_tmpBA;});this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmpB8),(void*)2);env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL6F;_LL74:;_LL75: this_ans=Cyc_CfFlowInfo_initlevel_approx(
r);_LL6F:;}if(this_ans == (void*)0)return(void*)0;if(this_ans == (void*)1  || acc == (
void*)1)return(void*)1;return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict
d,void*r){struct Cyc_CfFlowInfo_InitlevelEnv _tmpBB=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmpBC;_tmpBC.d=d;_tmpBC.seen=0;_tmpBC;});return Cyc_CfFlowInfo_initlevel_rec(&
_tmpBB,r,(void*)2);}struct _tuple9{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){struct Cyc_CfFlowInfo_Place
_tmpBE;void*_tmpBF;struct Cyc_List_List*_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpBD=
place;_tmpBE=*_tmpBD;_tmpBF=(void*)_tmpBE.root;_tmpC0=_tmpBE.fields;{void*_tmpC1=
Cyc_Dict_lookup(d,_tmpBF);for(0;_tmpC0 != 0;_tmpC0=_tmpC0->tl){struct _tuple9
_tmpC3=({struct _tuple9 _tmpC2;_tmpC2.f1=_tmpC1;_tmpC2.f2=(int)_tmpC0->hd;_tmpC2;});
void*_tmpC4;struct _dynforward_ptr _tmpC5;int _tmpC6;_LL77: _tmpC4=_tmpC3.f1;if(
_tmpC4 <= (void*)3)goto _LL79;if(*((int*)_tmpC4)!= 4)goto _LL79;_tmpC5=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC4)->f1;_tmpC6=_tmpC3.f2;_LL78: _tmpC1=*((void**)_check_dynforward_subscript(
_tmpC5,sizeof(void*),_tmpC6));goto _LL76;_LL79:;_LL7A:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct
Cyc_Core_Impossible_struct _tmpC8;_tmpC8.tag=Cyc_Core_Impossible;_tmpC8.f1=({
const char*_tmpC9="bad lookup_place";_tag_dynforward(_tmpC9,sizeof(char),
_get_zero_arr_size(_tmpC9,17));});_tmpC8;});_tmpC7;}));_LL76:;}return _tmpC1;}}
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmpCA=rval;struct
_dynforward_ptr _tmpCB;_LL7C: if(_tmpCA <= (void*)3)goto _LL80;if(*((int*)_tmpCA)!= 
1)goto _LL7E;_LL7D: return 0;_LL7E: if(*((int*)_tmpCA)!= 4)goto _LL80;_tmpCB=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmpCA)->f1;_LL7F: {unsigned int sz=
_get_dynforward_size(_tmpCB,sizeof(void*));{int i=0;for(0;i < sz;i ++){if(!Cyc_CfFlowInfo_is_rval_unescaped(((
void**)_tmpCB.curr)[i]))return 0;}}return 1;}_LL80:;_LL81: return 1;_LL7B:;}int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct _RegionHandle
_tmpCC=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCC;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmpCE=
_region_malloc(rgn,sizeof(*_tmpCE));_tmpCE->rgn=rgn;_tmpCE->places=0;_tmpCE;});
Cyc_CfFlowInfo_add_places(pile,r);{struct Cyc_Dict_Dict _tmpCD=Cyc_CfFlowInfo_escape_these(
pile,all_changed,d);_npop_handler(0);return _tmpCD;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval){struct _tuple0 _tmpD0=({struct _tuple0 _tmpCF;_tmpCF.f1=
oldval;_tmpCF.f2=newval;_tmpCF;});void*_tmpD1;void*_tmpD2;struct Cyc_CfFlowInfo_Place*
_tmpD3;void*_tmpD4;void*_tmpD5;struct _dynforward_ptr _tmpD6;void*_tmpD7;struct
_dynforward_ptr _tmpD8;void*_tmpD9;void*_tmpDA;_LL83: _tmpD1=_tmpD0.f1;if(_tmpD1 <= (
void*)3)goto _LL85;if(*((int*)_tmpD1)!= 1)goto _LL85;_tmpD2=_tmpD0.f2;if(_tmpD2 <= (
void*)3)goto _LL85;if(*((int*)_tmpD2)!= 2)goto _LL85;_tmpD3=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpD2)->f1;_LL84: Cyc_CfFlowInfo_add_place(env->pile,_tmpD3);goto _LL86;_LL85:
_tmpD4=_tmpD0.f1;if(_tmpD4 <= (void*)3)goto _LL87;if(*((int*)_tmpD4)!= 1)goto _LL87;
_LL86: if(Cyc_CfFlowInfo_initlevel(env->d,newval)!= (void*)2)({void*_tmpDB[0]={};
Cyc_Tcutil_terr(env->loc,({const char*_tmpDC="assignment puts possibly-uninitialized data in an escaped location";
_tag_dynforward(_tmpDC,sizeof(char),_get_zero_arr_size(_tmpDC,67));}),
_tag_dynforward(_tmpDB,sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;_LL87:
_tmpD5=_tmpD0.f1;if(_tmpD5 <= (void*)3)goto _LL89;if(*((int*)_tmpD5)!= 4)goto _LL89;
_tmpD6=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpD5)->f1;_tmpD7=_tmpD0.f2;if(
_tmpD7 <= (void*)3)goto _LL89;if(*((int*)_tmpD7)!= 4)goto _LL89;_tmpD8=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD7)->f1;_LL88: {struct _dynforward_ptr new_d=({unsigned int _tmpDF=
_get_dynforward_size(_tmpD6,sizeof(void*));void**_tmpE0=(void**)_cycalloc(
_check_times(sizeof(void*),_tmpDF));struct _dynforward_ptr _tmpE2=_tag_dynforward(
_tmpE0,sizeof(void*),_tmpDF);{unsigned int _tmpE1=_tmpDF;unsigned int i;for(i=0;i < 
_tmpE1;i ++){_tmpE0[i]=Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmpD6.curr)[(
int)i],*((void**)_check_dynforward_subscript(_tmpD8,sizeof(void*),(int)i)));}}
_tmpE2;});int change=0;{int i=0;for(0;i < _get_dynforward_size(_tmpD6,sizeof(void*));
i ++){if(*((void**)_check_dynforward_subscript(new_d,sizeof(void*),i))!= ((void**)
_tmpD6.curr)[i]){change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(
0;i < _get_dynforward_size(_tmpD6,sizeof(void*));i ++){if(*((void**)
_check_dynforward_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dynforward_subscript(_tmpD8,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpDD=
_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpDE;_tmpDE.tag=4;_tmpDE.f1=new_d;_tmpDE;});_tmpDD;});}_LL89: _tmpD9=_tmpD0.f2;
if(_tmpD9 <= (void*)3)goto _LL8B;if(*((int*)_tmpD9)!= 1)goto _LL8B;_tmpDA=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmpD9)->f1;_LL8A: {void*_tmpE3=_tmpDA;_LL8E:
if((int)_tmpE3 != 0)goto _LL90;_LL8F: return Cyc_CfFlowInfo_unknown_none;_LL90: if((
int)_tmpE3 != 1)goto _LL92;_LL91: return Cyc_CfFlowInfo_unknown_this;_LL92: if((int)
_tmpE3 != 2)goto _LL8D;_LL93: return Cyc_CfFlowInfo_unknown_all;_LL8D:;}_LL8B:;_LL8C:
return newval;_LL82:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(
env,oldval,newval);{struct _tuple8 _tmpE5=({struct _tuple8 _tmpE4;_tmpE4.f1=fs;
_tmpE4.f2=oldval;_tmpE4;});struct Cyc_List_List*_tmpE6;struct Cyc_List_List _tmpE7;
int _tmpE8;struct Cyc_List_List*_tmpE9;void*_tmpEA;struct _dynforward_ptr _tmpEB;
_LL95: _tmpE6=_tmpE5.f1;if(_tmpE6 == 0)goto _LL97;_tmpE7=*_tmpE6;_tmpE8=(int)_tmpE7.hd;
_tmpE9=_tmpE7.tl;_tmpEA=_tmpE5.f2;if(_tmpEA <= (void*)3)goto _LL97;if(*((int*)
_tmpEA)!= 4)goto _LL97;_tmpEB=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpEA)->f1;
_LL96: {void*_tmpEC=Cyc_CfFlowInfo_assign_place_outer(env,_tmpE9,*((void**)
_check_dynforward_subscript(_tmpEB,sizeof(void*),_tmpE8)),newval);struct
_dynforward_ptr _tmpED=Cyc_CfFlowInfo_aggr_dict_insert(_tmpEB,_tmpE8,_tmpEC);if(
_tmpED.curr == _tmpEB.curr)return oldval;else{return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpEF;_tmpEF.tag=4;_tmpEF.f1=_tmpED;_tmpEF;});_tmpEE;});}}_LL97:;_LL98:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));
_tmpF0[0]=({struct Cyc_Core_Impossible_struct _tmpF1;_tmpF1.tag=Cyc_Core_Impossible;
_tmpF1.f1=({const char*_tmpF2="bad assign place";_tag_dynforward(_tmpF2,sizeof(
char),_get_zero_arr_size(_tmpF2,17));});_tmpF1;});_tmpF0;}));_LL94:;}}struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,
struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){Cyc_CfFlowInfo_update_place_set(
all_changed,place,0);{struct _RegionHandle _tmpF3=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpF3;_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmpF5;
void*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_CfFlowInfo_Place*_tmpF4=place;
_tmpF5=*_tmpF4;_tmpF6=(void*)_tmpF5.root;_tmpF7=_tmpF5.fields;{struct Cyc_CfFlowInfo_AssignEnv
env=({struct Cyc_CfFlowInfo_AssignEnv _tmpF9;_tmpF9.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmpFA=_region_malloc(rgn,sizeof(*_tmpFA));_tmpFA->rgn=rgn;_tmpFA->places=0;
_tmpFA;});_tmpF9.d=d;_tmpF9.loc=loc;_tmpF9;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmpF7,Cyc_Dict_lookup(d,_tmpF6),r);struct Cyc_Dict_Dict _tmpF8=Cyc_CfFlowInfo_escape_these(
env.pile,all_changed,Cyc_Dict_insert(d,_tmpF6,newval));_npop_handler(0);return
_tmpF8;}};_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*
_tmpFB=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmpFD;void*_tmpFE;
void*_tmpFF;struct Cyc_CfFlowInfo_TagCmp*_tmpFC=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;
_tmpFD=*_tmpFC;_tmpFE=(void*)_tmpFD.cmp;_tmpFF=(void*)_tmpFD.bd;{int found=0;void*
joined_cmp=(void*)10;{struct Cyc_List_List*_tmp100=l1;for(0;_tmp100 != 0;_tmp100=
_tmp100->tl){struct Cyc_CfFlowInfo_TagCmp _tmp102;void*_tmp103;void*_tmp104;struct
Cyc_CfFlowInfo_TagCmp*_tmp101=(struct Cyc_CfFlowInfo_TagCmp*)_tmp100->hd;_tmp102=*
_tmp101;_tmp103=(void*)_tmp102.cmp;_tmp104=(void*)_tmp102.bd;if(Cyc_Tcutil_typecmp(
_tmpFF,_tmp104)== 0){found=1;if(_tmp103 == _tmpFE){joined_cmp=_tmp103;break;}}}}
if(found)_tmpFB=({struct Cyc_List_List*_tmp105=_cycalloc(sizeof(*_tmp105));
_tmp105->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp106=_cycalloc(sizeof(*_tmp106));
_tmp106->cmp=(void*)joined_cmp;_tmp106->bd=(void*)_tmpFF;_tmp106;});_tmp105->tl=
_tmpFB;_tmp105;});}}return _tmpFB;}}static void*Cyc_CfFlowInfo_join_absRval(struct
Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct
_tuple0 _tmp108=({struct _tuple0 _tmp107;_tmp107.f1=r1;_tmp107.f2=r2;_tmp107;});
void*_tmp109;struct Cyc_CfFlowInfo_Place*_tmp10A;void*_tmp10B;struct Cyc_CfFlowInfo_Place*
_tmp10C;void*_tmp10D;struct Cyc_CfFlowInfo_Place*_tmp10E;void*_tmp10F;struct Cyc_CfFlowInfo_Place*
_tmp110;void*_tmp111;void*_tmp112;void*_tmp113;void*_tmp114;void*_tmp115;struct
_dynforward_ptr _tmp116;void*_tmp117;struct _dynforward_ptr _tmp118;void*_tmp119;
struct Cyc_List_List*_tmp11A;void*_tmp11B;struct Cyc_List_List*_tmp11C;void*
_tmp11D;void*_tmp11E;_LL9A: _tmp109=_tmp108.f1;if(_tmp109 <= (void*)3)goto _LL9C;
if(*((int*)_tmp109)!= 2)goto _LL9C;_tmp10A=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp109)->f1;_tmp10B=_tmp108.f2;if(_tmp10B <= (void*)3)goto _LL9C;if(*((int*)
_tmp10B)!= 2)goto _LL9C;_tmp10C=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp10B)->f1;
_LL9B: if(Cyc_CfFlowInfo_place_cmp(_tmp10A,_tmp10C)== 0)return r1;Cyc_CfFlowInfo_add_place(
env->pile,_tmp10A);Cyc_CfFlowInfo_add_place(env->pile,_tmp10C);goto _LL99;_LL9C:
_tmp10D=_tmp108.f1;if(_tmp10D <= (void*)3)goto _LL9E;if(*((int*)_tmp10D)!= 2)goto
_LL9E;_tmp10E=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp10D)->f1;_LL9D: Cyc_CfFlowInfo_add_place(
env->pile,_tmp10E);goto _LL99;_LL9E: _tmp10F=_tmp108.f2;if(_tmp10F <= (void*)3)goto
_LLA0;if(*((int*)_tmp10F)!= 2)goto _LLA0;_tmp110=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp10F)->f1;_LL9F: Cyc_CfFlowInfo_add_place(env->pile,_tmp110);goto _LL99;_LLA0:
_tmp111=_tmp108.f1;if((int)_tmp111 != 1)goto _LLA2;_tmp112=_tmp108.f2;if((int)
_tmp112 != 2)goto _LLA2;_LLA1: goto _LLA3;_LLA2: _tmp113=_tmp108.f1;if((int)_tmp113 != 
2)goto _LLA4;_tmp114=_tmp108.f2;if((int)_tmp114 != 1)goto _LLA4;_LLA3: return(void*)
2;_LLA4: _tmp115=_tmp108.f1;if(_tmp115 <= (void*)3)goto _LLA6;if(*((int*)_tmp115)!= 
4)goto _LLA6;_tmp116=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp115)->f1;
_tmp117=_tmp108.f2;if(_tmp117 <= (void*)3)goto _LLA6;if(*((int*)_tmp117)!= 4)goto
_LLA6;_tmp118=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp117)->f1;_LLA5: {
struct _dynforward_ptr new_d=({unsigned int _tmp121=_get_dynforward_size(_tmp116,
sizeof(void*));void**_tmp122=(void**)_cycalloc(_check_times(sizeof(void*),
_tmp121));struct _dynforward_ptr _tmp124=_tag_dynforward(_tmp122,sizeof(void*),
_tmp121);{unsigned int _tmp123=_tmp121;unsigned int i;for(i=0;i < _tmp123;i ++){
_tmp122[i]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(
env,0,((void**)_tmp116.curr)[(int)i],*((void**)_check_dynforward_subscript(
_tmp118,sizeof(void*),(int)i)));}}_tmp124;});int change=0;{int i=0;for(0;i < 
_get_dynforward_size(_tmp116,sizeof(void*));i ++){if(*((void**)
_check_dynforward_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp116.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dynforward_size(_tmp116,sizeof(void*));i ++){if(*((void**)
_check_dynforward_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dynforward_subscript(_tmp118,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp11F=
_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp120;_tmp120.tag=4;_tmp120.f1=new_d;_tmp120;});_tmp11F;});}_LLA6: _tmp119=
_tmp108.f1;if(_tmp119 <= (void*)3)goto _LLA8;if(*((int*)_tmp119)!= 3)goto _LLA8;
_tmp11A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp119)->f1;_tmp11B=_tmp108.f2;
if(_tmp11B <= (void*)3)goto _LLA8;if(*((int*)_tmp11B)!= 3)goto _LLA8;_tmp11C=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp11B)->f1;_LLA7: {struct Cyc_List_List*
_tmp125=Cyc_CfFlowInfo_join_tag_cmps(_tmp11A,_tmp11C);if(_tmp125 == _tmp11A)
return r1;return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmp126=_cycalloc(
sizeof(*_tmp126));_tmp126[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmp127;
_tmp127.tag=3;_tmp127.f1=_tmp125;_tmp127;});_tmp126;});}_LLA8: _tmp11D=_tmp108.f1;
if(_tmp11D <= (void*)3)goto _LLAA;if(*((int*)_tmp11D)!= 3)goto _LLAA;_LLA9: return r2;
_LLAA: _tmp11E=_tmp108.f2;if(_tmp11E <= (void*)3)goto _LLAC;if(*((int*)_tmp11E)!= 3)
goto _LLAC;_LLAB: return r1;_LLAC:;_LLAD: goto _LL99;_LL99:;}{void*il1=Cyc_CfFlowInfo_initlevel(
env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,r2);struct _tuple0 _tmp129=({
struct _tuple0 _tmp128;_tmp128.f1=r1;_tmp128.f2=r2;_tmp128;});void*_tmp12A;void*
_tmp12B;_LLAF: _tmp12A=_tmp129.f1;if(_tmp12A <= (void*)3)goto _LLB1;if(*((int*)
_tmp12A)!= 1)goto _LLB1;_LLB0: goto _LLB2;_LLB1: _tmp12B=_tmp129.f2;if(_tmp12B <= (
void*)3)goto _LLB3;if(*((int*)_tmp12B)!= 1)goto _LLB3;_LLB2: {struct _tuple0 _tmp12D=({
struct _tuple0 _tmp12C;_tmp12C.f1=il1;_tmp12C.f2=il2;_tmp12C;});void*_tmp12E;void*
_tmp12F;void*_tmp130;void*_tmp131;_LLB6: _tmp12E=_tmp12D.f2;if((int)_tmp12E != 0)
goto _LLB8;_LLB7: goto _LLB9;_LLB8: _tmp12F=_tmp12D.f1;if((int)_tmp12F != 0)goto _LLBA;
_LLB9: return Cyc_CfFlowInfo_esc_none;_LLBA: _tmp130=_tmp12D.f2;if((int)_tmp130 != 1)
goto _LLBC;_LLBB: goto _LLBD;_LLBC: _tmp131=_tmp12D.f1;if((int)_tmp131 != 1)goto _LLBE;
_LLBD: return Cyc_CfFlowInfo_esc_this;_LLBE:;_LLBF: return Cyc_CfFlowInfo_esc_all;
_LLB5:;}_LLB3:;_LLB4: {struct _tuple0 _tmp133=({struct _tuple0 _tmp132;_tmp132.f1=
il1;_tmp132.f2=il2;_tmp132;});void*_tmp134;void*_tmp135;void*_tmp136;void*
_tmp137;_LLC1: _tmp134=_tmp133.f2;if((int)_tmp134 != 0)goto _LLC3;_LLC2: goto _LLC4;
_LLC3: _tmp135=_tmp133.f1;if((int)_tmp135 != 0)goto _LLC5;_LLC4: return Cyc_CfFlowInfo_unknown_none;
_LLC5: _tmp136=_tmp133.f2;if((int)_tmp136 != 1)goto _LLC7;_LLC6: goto _LLC8;_LLC7:
_tmp137=_tmp133.f1;if((int)_tmp137 != 1)goto _LLC9;_LLC8: return Cyc_CfFlowInfo_unknown_this;
_LLC9:;_LLCA: return Cyc_CfFlowInfo_unknown_all;_LLC0:;}_LLAE:;}}struct _tuple10{
union Cyc_CfFlowInfo_RelnOp_union f1;union Cyc_CfFlowInfo_RelnOp_union f2;};static
int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp_union r1,union Cyc_CfFlowInfo_RelnOp_union
r2){struct _tuple10 _tmp139=({struct _tuple10 _tmp138;_tmp138.f1=r1;_tmp138.f2=r2;
_tmp138;});union Cyc_CfFlowInfo_RelnOp_union _tmp13A;unsigned int _tmp13B;union Cyc_CfFlowInfo_RelnOp_union
_tmp13C;unsigned int _tmp13D;union Cyc_CfFlowInfo_RelnOp_union _tmp13E;struct Cyc_Absyn_Vardecl*
_tmp13F;union Cyc_CfFlowInfo_RelnOp_union _tmp140;struct Cyc_Absyn_Vardecl*_tmp141;
union Cyc_CfFlowInfo_RelnOp_union _tmp142;struct Cyc_Absyn_Vardecl*_tmp143;union Cyc_CfFlowInfo_RelnOp_union
_tmp144;struct Cyc_Absyn_Vardecl*_tmp145;union Cyc_CfFlowInfo_RelnOp_union _tmp146;
unsigned int _tmp147;union Cyc_CfFlowInfo_RelnOp_union _tmp148;unsigned int _tmp149;
union Cyc_CfFlowInfo_RelnOp_union _tmp14A;struct Cyc_Absyn_Vardecl*_tmp14B;union Cyc_CfFlowInfo_RelnOp_union
_tmp14C;struct Cyc_Absyn_Vardecl*_tmp14D;_LLCC: _tmp13A=_tmp139.f1;if(((_tmp139.f1).EqualConst).tag
!= 0)goto _LLCE;_tmp13B=(_tmp13A.EqualConst).f1;_tmp13C=_tmp139.f2;if(((_tmp139.f2).EqualConst).tag
!= 0)goto _LLCE;_tmp13D=(_tmp13C.EqualConst).f1;_LLCD: return _tmp13B == _tmp13D;
_LLCE: _tmp13E=_tmp139.f1;if(((_tmp139.f1).LessVar).tag != 1)goto _LLD0;_tmp13F=(
_tmp13E.LessVar).f1;_tmp140=_tmp139.f2;if(((_tmp139.f2).LessVar).tag != 1)goto
_LLD0;_tmp141=(_tmp140.LessVar).f1;_LLCF: return _tmp13F == _tmp141;_LLD0: _tmp142=
_tmp139.f1;if(((_tmp139.f1).LessSize).tag != 2)goto _LLD2;_tmp143=(_tmp142.LessSize).f1;
_tmp144=_tmp139.f2;if(((_tmp139.f2).LessSize).tag != 2)goto _LLD2;_tmp145=(_tmp144.LessSize).f1;
_LLD1: return _tmp143 == _tmp145;_LLD2: _tmp146=_tmp139.f1;if(((_tmp139.f1).LessConst).tag
!= 3)goto _LLD4;_tmp147=(_tmp146.LessConst).f1;_tmp148=_tmp139.f2;if(((_tmp139.f2).LessConst).tag
!= 3)goto _LLD4;_tmp149=(_tmp148.LessConst).f1;_LLD3: return _tmp147 == _tmp149;
_LLD4: _tmp14A=_tmp139.f1;if(((_tmp139.f1).LessEqSize).tag != 4)goto _LLD6;_tmp14B=(
_tmp14A.LessEqSize).f1;_tmp14C=_tmp139.f2;if(((_tmp139.f2).LessEqSize).tag != 4)
goto _LLD6;_tmp14D=(_tmp14C.LessEqSize).f1;_LLD5: return _tmp14B == _tmp14D;_LLD6:;
_LLD7: return 0;_LLCB:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct
Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*
res=0;int diff=0;{struct Cyc_List_List*_tmp14E=r1s;for(0;_tmp14E != 0;_tmp14E=
_tmp14E->tl){struct Cyc_CfFlowInfo_Reln*_tmp14F=(struct Cyc_CfFlowInfo_Reln*)
_tmp14E->hd;int found=0;{struct Cyc_List_List*_tmp150=r2s;for(0;_tmp150 != 0;
_tmp150=_tmp150->tl){struct Cyc_CfFlowInfo_Reln*_tmp151=(struct Cyc_CfFlowInfo_Reln*)
_tmp150->hd;if(_tmp14F == _tmp151  || _tmp14F->vd == _tmp151->vd  && Cyc_CfFlowInfo_same_relop(
_tmp14F->rop,_tmp151->rop)){res=({struct Cyc_List_List*_tmp152=_cycalloc(sizeof(*
_tmp152));_tmp152->hd=_tmp14F;_tmp152->tl=res;_tmp152;});found=1;break;}}}if(!
found)diff=1;}}if(!diff)res=r1s;return res;}}static void Cyc_CfFlowInfo_remove_f(
struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*place){struct _handler_cons
_tmp153;_push_handler(& _tmp153);{int _tmp155=0;if(setjmp(_tmp153.handler))_tmp155=
1;if(!_tmp155){*l=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*l,place);;_pop_handler();}else{
void*_tmp154=(void*)_exn_thrown;void*_tmp157=_tmp154;_LLD9: if(_tmp157 != Cyc_Core_Not_found)
goto _LLDB;_LLDA: goto _LLD8;_LLDB:;_LLDC:(void)_throw(_tmp157);_LLD8:;}}}void Cyc_CfFlowInfo_print_place_list(
struct Cyc_List_List*x){int _tmp158=1;while(x != 0){if(!_tmp158){({void*_tmp159[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp15A=", ";_tag_dynforward(_tmp15A,sizeof(
char),_get_zero_arr_size(_tmp15A,3));}),_tag_dynforward(_tmp159,sizeof(void*),0));});
_tmp158=0;}Cyc_CfFlowInfo_print_place((struct Cyc_CfFlowInfo_Place*)x->hd);x=x->tl;}({
void*_tmp15B[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp15C="\n";
_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,2));}),
_tag_dynforward(_tmp15B,sizeof(void*),0));});}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_or_consume(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;{struct _RegionHandle _tmp15D=
_new_region("r");struct _RegionHandle*r=& _tmp15D;_push_region(r);{struct Cyc_List_List*
_tmp15E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))
Cyc_List_rcopy)(r,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp15E,c2.may_consume);while(_tmp15E != 0){mc=({struct
Cyc_List_List*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp15E))->hd;_tmp15F->tl=mc;_tmp15F;});_tmp15E=((
struct Cyc_List_List*)_check_null(_tmp15E))->tl;}};_pop_region(r);}return({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp160;_tmp160.consumed=Cyc_CfFlowInfo_union_place_set(
c1.consumed,c2.consumed,0);_tmp160.may_consume=mc;_tmp160;});}struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp161=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(c1.may_consume,c2.may_consume);return({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp162;_tmp162.consumed=Cyc_CfFlowInfo_union_place_set(
c1.consumed,c2.consumed,1);_tmp162.may_consume=_tmp161;_tmp162;});}struct
_tuple11{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union
f2,int or_consumed){struct _tuple11 _tmp164=({struct _tuple11 _tmp163;_tmp163.f1=f1;
_tmp163.f2=f2;_tmp163;});union Cyc_CfFlowInfo_FlowInfo_union _tmp165;union Cyc_CfFlowInfo_FlowInfo_union
_tmp166;union Cyc_CfFlowInfo_FlowInfo_union _tmp167;struct Cyc_Dict_Dict _tmp168;
struct Cyc_List_List*_tmp169;struct Cyc_CfFlowInfo_ConsumeInfo _tmp16A;union Cyc_CfFlowInfo_FlowInfo_union
_tmp16B;struct Cyc_Dict_Dict _tmp16C;struct Cyc_List_List*_tmp16D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp16E;_LLDE: _tmp165=_tmp164.f1;if(((_tmp164.f1).BottomFL).tag != 0)goto _LLE0;
_LLDF: return f2;_LLE0: _tmp166=_tmp164.f2;if(((_tmp164.f2).BottomFL).tag != 0)goto
_LLE2;_LLE1: return f1;_LLE2: _tmp167=_tmp164.f1;if(((_tmp164.f1).ReachableFL).tag
!= 1)goto _LLDD;_tmp168=(_tmp167.ReachableFL).f1;_tmp169=(_tmp167.ReachableFL).f2;
_tmp16A=(_tmp167.ReachableFL).f3;_tmp16B=_tmp164.f2;if(((_tmp164.f2).ReachableFL).tag
!= 1)goto _LLDD;_tmp16C=(_tmp16B.ReachableFL).f1;_tmp16D=(_tmp16B.ReachableFL).f2;
_tmp16E=(_tmp16B.ReachableFL).f3;_LLE3: if(((_tmp168.t == _tmp16C.t  && _tmp169 == 
_tmp16D) && _tmp16A.may_consume == _tmp16E.may_consume) && (_tmp16A.consumed).t == (
_tmp16E.consumed).t)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))
return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmp16F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp16F;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp170=({struct Cyc_CfFlowInfo_JoinEnv
_tmp176;_tmp176.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp177=_region_malloc(rgn,
sizeof(*_tmp177));_tmp177->rgn=rgn;_tmp177->places=0;_tmp177;});_tmp176.d1=
_tmp168;_tmp176.d2=_tmp16C;_tmp176;});struct Cyc_Dict_Dict _tmp171=((struct Cyc_Dict_Dict(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmp170,_tmp168,_tmp16C);struct Cyc_List_List*_tmp172=Cyc_CfFlowInfo_join_relns(
_tmp169,_tmp16D);struct Cyc_CfFlowInfo_ConsumeInfo _tmp173=or_consumed?Cyc_CfFlowInfo_or_consume(
_tmp16A,_tmp16E): Cyc_CfFlowInfo_and_consume(_tmp16A,_tmp16E);union Cyc_CfFlowInfo_FlowInfo_union
_tmp175=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp174;(_tmp174.ReachableFL).tag=1;(_tmp174.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(
_tmp170.pile,all_changed,_tmp171);(_tmp174.ReachableFL).f2=_tmp172;(_tmp174.ReachableFL).f3=
_tmp173;_tmp174;});_npop_handler(0);return _tmp175;};_pop_region(rgn);}_LLDD:;}
struct _tuple12{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;union Cyc_CfFlowInfo_FlowInfo_union f3;};struct _tuple4 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_Dict_Dict*all_changed,struct _tuple4 pr1,struct _tuple4 pr2,int or_consumed){
union Cyc_CfFlowInfo_FlowInfo_union _tmp179;void*_tmp17A;struct _tuple4 _tmp178=pr1;
_tmp179=_tmp178.f1;_tmp17A=_tmp178.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp17C;
void*_tmp17D;struct _tuple4 _tmp17B=pr2;_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;{
union Cyc_CfFlowInfo_FlowInfo_union outflow=Cyc_CfFlowInfo_join_flow(all_changed,
_tmp179,_tmp17C,or_consumed);struct _tuple12 _tmp17F=({struct _tuple12 _tmp17E;
_tmp17E.f1=_tmp179;_tmp17E.f2=_tmp17C;_tmp17E.f3=outflow;_tmp17E;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp180;union Cyc_CfFlowInfo_FlowInfo_union _tmp181;union Cyc_CfFlowInfo_FlowInfo_union
_tmp182;struct Cyc_Dict_Dict _tmp183;union Cyc_CfFlowInfo_FlowInfo_union _tmp184;
struct Cyc_Dict_Dict _tmp185;union Cyc_CfFlowInfo_FlowInfo_union _tmp186;struct Cyc_Dict_Dict
_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_CfFlowInfo_ConsumeInfo _tmp189;
_LLE5: _tmp180=_tmp17F.f1;if(((_tmp17F.f1).BottomFL).tag != 0)goto _LLE7;_LLE6:
return({struct _tuple4 _tmp18A;_tmp18A.f1=outflow;_tmp18A.f2=_tmp17D;_tmp18A;});
_LLE7: _tmp181=_tmp17F.f2;if(((_tmp17F.f2).BottomFL).tag != 0)goto _LLE9;_LLE8:
return({struct _tuple4 _tmp18B;_tmp18B.f1=outflow;_tmp18B.f2=_tmp17A;_tmp18B;});
_LLE9: _tmp182=_tmp17F.f1;if(((_tmp17F.f1).ReachableFL).tag != 1)goto _LLEB;_tmp183=(
_tmp182.ReachableFL).f1;_tmp184=_tmp17F.f2;if(((_tmp17F.f2).ReachableFL).tag != 1)
goto _LLEB;_tmp185=(_tmp184.ReachableFL).f1;_tmp186=_tmp17F.f3;if(((_tmp17F.f3).ReachableFL).tag
!= 1)goto _LLEB;_tmp187=(_tmp186.ReachableFL).f1;_tmp188=(_tmp186.ReachableFL).f2;
_tmp189=(_tmp186.ReachableFL).f3;_LLEA: if(((int(*)(int ignore,void*r1,void*r2))
Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp17A,_tmp17D))return({struct _tuple4
_tmp18C;_tmp18C.f1=outflow;_tmp18C.f2=_tmp17D;_tmp18C;});if(((int(*)(int ignore,
void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp17D,_tmp17A))
return({struct _tuple4 _tmp18D;_tmp18D.f1=outflow;_tmp18D.f2=_tmp17A;_tmp18D;});{
struct _RegionHandle _tmp18E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp18E;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp18F=({struct Cyc_CfFlowInfo_JoinEnv
_tmp194;_tmp194.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp195=_region_malloc(rgn,
sizeof(*_tmp195));_tmp195->rgn=rgn;_tmp195->places=0;_tmp195;});_tmp194.d1=
_tmp183;_tmp194.d2=_tmp185;_tmp194;});void*_tmp190=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp18F,0,_tmp17A,
_tmp17D);struct _tuple4 _tmp193=({struct _tuple4 _tmp191;_tmp191.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp192;(_tmp192.ReachableFL).tag=1;(_tmp192.ReachableFL).f1=
Cyc_CfFlowInfo_escape_these(_tmp18F.pile,all_changed,_tmp187);(_tmp192.ReachableFL).f2=
_tmp188;(_tmp192.ReachableFL).f3=_tmp189;_tmp192;});_tmp191.f2=_tmp190;_tmp191;});
_npop_handler(0);return _tmp193;};_pop_region(rgn);}_LLEB:;_LLEC:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp196=_cycalloc(sizeof(*_tmp196));
_tmp196[0]=({struct Cyc_Core_Impossible_struct _tmp197;_tmp197.tag=Cyc_Core_Impossible;
_tmp197.f1=({const char*_tmp198="join_flow_and_rval: BottomFL outflow";
_tag_dynforward(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,37));});_tmp197;});
_tmp196;}));_LLE4:;}}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,
int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*
env,void*r1,void*r2){int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,
env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,
env->curr_place);if(changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(((env->joinenv).pile)->rgn,env->curr_place,
env->chg2))return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(((
env->joinenv).pile)->rgn,env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{
struct _tuple0 _tmp19A=({struct _tuple0 _tmp199;_tmp199.f1=r1;_tmp199.f2=r2;_tmp199;});
void*_tmp19B;struct _dynforward_ptr _tmp19C;void*_tmp19D;struct _dynforward_ptr
_tmp19E;_LLEE: _tmp19B=_tmp19A.f1;if(_tmp19B <= (void*)3)goto _LLF0;if(*((int*)
_tmp19B)!= 4)goto _LLF0;_tmp19C=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp19B)->f1;
_tmp19D=_tmp19A.f2;if(_tmp19D <= (void*)3)goto _LLF0;if(*((int*)_tmp19D)!= 4)goto
_LLF0;_tmp19E=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp19D)->f1;_LLEF: {
struct _dynforward_ptr new_d=({unsigned int _tmp1A1=_get_dynforward_size(_tmp19C,
sizeof(void*));void**_tmp1A2=(void**)_cycalloc(_check_times(sizeof(void*),
_tmp1A1));struct _dynforward_ptr _tmp1A4=_tag_dynforward(_tmp1A2,sizeof(void*),
_tmp1A1);{unsigned int _tmp1A3=_tmp1A1;unsigned int i;for(i=0;i < _tmp1A3;i ++){
_tmp1A2[i]=Cyc_CfFlowInfo_after_absRval_d(env,(int)i,((void**)_tmp19C.curr)[(int)
i],*((void**)_check_dynforward_subscript(_tmp19E,sizeof(void*),(int)i)));}}
_tmp1A4;});int change=0;{int i=0;for(0;i < _get_dynforward_size(_tmp19C,sizeof(void*));
i ++){if(*((void**)_check_dynforward_subscript(new_d,sizeof(void*),i))!= ((void**)
_tmp19C.curr)[i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i
< _get_dynforward_size(_tmp19C,sizeof(void*));i ++){if(*((void**)
_check_dynforward_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dynforward_subscript(_tmp19E,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp19F=
_cycalloc(sizeof(*_tmp19F));_tmp19F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp1A0;_tmp1A0.tag=4;_tmp1A0.f1=new_d;_tmp1A0;});_tmp19F;});}_LLF0:;_LLF1:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1A5=_cycalloc(sizeof(*
_tmp1A5));_tmp1A5[0]=({struct Cyc_Core_Impossible_struct _tmp1A6;_tmp1A6.tag=Cyc_Core_Impossible;
_tmp1A6.f1=({const char*_tmp1A7="after_absRval -- non-aggregates!";
_tag_dynforward(_tmp1A7,sizeof(char),_get_zero_arr_size(_tmp1A7,33));});_tmp1A6;});
_tmp1A5;}));_LLED:;}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*
env,int key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**_tmp1A8=
env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp1A9=env->changed;*env->last_field_cell=({
struct Cyc_List_List*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->hd=(void*)key;
_tmp1AA->tl=0;_tmp1AA;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmp1AB=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp1A8;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp1A9;return _tmp1AB;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp1AC;_tmp1AC.root=(void*)root;
_tmp1AC.fields=0;_tmp1AC;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict
chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Null_c_struct){6})};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((
void*)& dummy_rawexp),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Place
dummy_place={(void*)((void*)& dummy_root),0};struct _tuple11 _tmp1AE=({struct
_tuple11 _tmp1AD;_tmp1AD.f1=f1;_tmp1AD.f2=f2;_tmp1AD;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp1AF;union Cyc_CfFlowInfo_FlowInfo_union _tmp1B0;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B1;struct Cyc_Dict_Dict _tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1B4;union Cyc_CfFlowInfo_FlowInfo_union _tmp1B5;struct Cyc_Dict_Dict _tmp1B6;
struct Cyc_List_List*_tmp1B7;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B8;_LLF3:
_tmp1AF=_tmp1AE.f1;if(((_tmp1AE.f1).BottomFL).tag != 0)goto _LLF5;_LLF4: goto _LLF6;
_LLF5: _tmp1B0=_tmp1AE.f2;if(((_tmp1AE.f2).BottomFL).tag != 0)goto _LLF7;_LLF6:
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B9;(_tmp1B9.BottomFL).tag=0;_tmp1B9;});_LLF7: _tmp1B1=_tmp1AE.f1;if(((
_tmp1AE.f1).ReachableFL).tag != 1)goto _LLF2;_tmp1B2=(_tmp1B1.ReachableFL).f1;
_tmp1B3=(_tmp1B1.ReachableFL).f2;_tmp1B4=(_tmp1B1.ReachableFL).f3;_tmp1B5=
_tmp1AE.f2;if(((_tmp1AE.f2).ReachableFL).tag != 1)goto _LLF2;_tmp1B6=(_tmp1B5.ReachableFL).f1;
_tmp1B7=(_tmp1B5.ReachableFL).f2;_tmp1B8=(_tmp1B5.ReachableFL).f3;_LLF8: if(((
_tmp1B2.t == _tmp1B6.t  && _tmp1B3 == _tmp1B7) && _tmp1B4.may_consume == _tmp1B8.may_consume)
 && (_tmp1B4.consumed).t == (_tmp1B8.consumed).t)return f1;{struct _RegionHandle
_tmp1BA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1BA;_push_region(rgn);{
struct Cyc_CfFlowInfo_Place*_tmp1BB=& dummy_place;struct Cyc_CfFlowInfo_AfterEnv
_tmp1BC=({struct Cyc_CfFlowInfo_AfterEnv _tmp1C2;_tmp1C2.joinenv=({struct Cyc_CfFlowInfo_JoinEnv
_tmp1C3;_tmp1C3.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1C4=_region_malloc(rgn,
sizeof(*_tmp1C4));_tmp1C4->rgn=rgn;_tmp1C4->places=0;_tmp1C4;});_tmp1C3.d1=
_tmp1B2;_tmp1C3.d2=_tmp1B6;_tmp1C3;});_tmp1C2.chg1=chg1;_tmp1C2.chg2=chg2;
_tmp1C2.curr_place=_tmp1BB;_tmp1C2.last_field_cell=& _tmp1BB->fields;_tmp1C2.changed=
Cyc_CfFlowInfo_Neither;_tmp1C2;});struct Cyc_Dict_Dict _tmp1BD=((struct Cyc_Dict_Dict(*)(
void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp1BC,_tmp1B2,_tmp1B6);struct Cyc_List_List*_tmp1BE=Cyc_CfFlowInfo_join_relns(
_tmp1B3,_tmp1B7);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1BF=Cyc_CfFlowInfo_and_consume(
_tmp1B4,_tmp1B8);union Cyc_CfFlowInfo_FlowInfo_union _tmp1C1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C0;(_tmp1C0.ReachableFL).tag=1;(_tmp1C0.ReachableFL).f1=
Cyc_CfFlowInfo_escape_these((_tmp1BC.joinenv).pile,all_changed,_tmp1BD);(_tmp1C0.ReachableFL).f2=
_tmp1BE;(_tmp1C0.ReachableFL).f3=_tmp1BF;_tmp1C0;});_npop_handler(0);return
_tmp1C1;};_pop_region(rgn);}_LLF2:;}static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1C9;void*_tmp1CA;void*_tmp1CB;struct Cyc_CfFlowInfo_TagCmp*_tmp1C8=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp1C9=*_tmp1C8;_tmp1CA=(void*)_tmp1C9.cmp;_tmp1CB=(void*)_tmp1C9.bd;{
struct Cyc_List_List*_tmp1CC=l1;for(0;_tmp1CC != 0;_tmp1CC=_tmp1CC->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1CE;void*_tmp1CF;void*_tmp1D0;struct Cyc_CfFlowInfo_TagCmp*_tmp1CD=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp1CC->hd;_tmp1CE=*_tmp1CD;_tmp1CF=(void*)_tmp1CE.cmp;_tmp1D0=(void*)_tmp1CE.bd;
if(_tmp1CF == _tmp1CA  && Cyc_Tcutil_typecmp(_tmp1D0,_tmp1CB)== 0)break;}if(
_tmp1CC == 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp1D2=({struct
_tuple0 _tmp1D1;_tmp1D1.f1=r1;_tmp1D1.f2=r2;_tmp1D1;});void*_tmp1D3;struct Cyc_CfFlowInfo_Place*
_tmp1D4;void*_tmp1D5;struct Cyc_CfFlowInfo_Place*_tmp1D6;void*_tmp1D7;void*
_tmp1D8;void*_tmp1D9;struct _dynforward_ptr _tmp1DA;void*_tmp1DB;struct
_dynforward_ptr _tmp1DC;void*_tmp1DD;void*_tmp1DE;void*_tmp1DF;void*_tmp1E0;void*
_tmp1E1;void*_tmp1E2;void*_tmp1E3;struct Cyc_List_List*_tmp1E4;void*_tmp1E5;
struct Cyc_List_List*_tmp1E6;void*_tmp1E7;_LLFA: _tmp1D3=_tmp1D2.f1;if(_tmp1D3 <= (
void*)3)goto _LLFC;if(*((int*)_tmp1D3)!= 2)goto _LLFC;_tmp1D4=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp1D3)->f1;_tmp1D5=_tmp1D2.f2;if(_tmp1D5 <= (void*)3)goto _LLFC;if(*((int*)
_tmp1D5)!= 2)goto _LLFC;_tmp1D6=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D5)->f1;
_LLFB: return Cyc_CfFlowInfo_place_cmp(_tmp1D4,_tmp1D6)== 0;_LLFC: _tmp1D7=_tmp1D2.f1;
if(_tmp1D7 <= (void*)3)goto _LLFE;if(*((int*)_tmp1D7)!= 2)goto _LLFE;_LLFD: goto
_LLFF;_LLFE: _tmp1D8=_tmp1D2.f2;if(_tmp1D8 <= (void*)3)goto _LL100;if(*((int*)
_tmp1D8)!= 2)goto _LL100;_LLFF: return 0;_LL100: _tmp1D9=_tmp1D2.f1;if(_tmp1D9 <= (
void*)3)goto _LL102;if(*((int*)_tmp1D9)!= 4)goto _LL102;_tmp1DA=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1D9)->f1;_tmp1DB=_tmp1D2.f2;if(_tmp1DB <= (void*)3)goto _LL102;if(*((int*)
_tmp1DB)!= 4)goto _LL102;_tmp1DC=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1DB)->f1;
_LL101: if(_tmp1DA.curr == _tmp1DC.curr)return 1;{int i=0;for(0;i < 
_get_dynforward_size(_tmp1DA,sizeof(void*));i ++){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp1DA.curr)[i],*((
void**)_check_dynforward_subscript(_tmp1DC,sizeof(void*),i))))return 0;}}return 1;
_LL102: _tmp1DD=_tmp1D2.f2;if((int)_tmp1DD != 2)goto _LL104;_LL103: return r1 == (void*)
1;_LL104: _tmp1DE=_tmp1D2.f2;if((int)_tmp1DE != 0)goto _LL106;_LL105: goto _LL107;
_LL106: _tmp1DF=_tmp1D2.f2;if((int)_tmp1DF != 1)goto _LL108;_LL107: return 0;_LL108:
_tmp1E0=_tmp1D2.f1;if(_tmp1E0 <= (void*)3)goto _LL10A;if(*((int*)_tmp1E0)!= 1)goto
_LL10A;_tmp1E1=_tmp1D2.f2;if(_tmp1E1 <= (void*)3)goto _LL10A;if(*((int*)_tmp1E1)!= 
1)goto _LL10A;_LL109: goto _LLF9;_LL10A: _tmp1E2=_tmp1D2.f1;if(_tmp1E2 <= (void*)3)
goto _LL10C;if(*((int*)_tmp1E2)!= 1)goto _LL10C;_LL10B: return 0;_LL10C: _tmp1E3=
_tmp1D2.f1;if(_tmp1E3 <= (void*)3)goto _LL10E;if(*((int*)_tmp1E3)!= 3)goto _LL10E;
_tmp1E4=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1E3)->f1;_tmp1E5=_tmp1D2.f2;
if(_tmp1E5 <= (void*)3)goto _LL10E;if(*((int*)_tmp1E5)!= 3)goto _LL10E;_tmp1E6=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1E5)->f1;_LL10D: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp1E4,_tmp1E6);_LL10E: _tmp1E7=_tmp1D2.f2;if(_tmp1E7 <= (void*)3)goto _LL110;if(*((
int*)_tmp1E7)!= 3)goto _LL110;_LL10F: return 0;_LL110:;_LL111: goto _LLF9;_LLF9:;}{
struct _tuple0 _tmp1E9=({struct _tuple0 _tmp1E8;_tmp1E8.f1=Cyc_CfFlowInfo_initlevel_approx(
r1);_tmp1E8.f2=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp1E8;});void*_tmp1EA;void*
_tmp1EB;void*_tmp1EC;void*_tmp1ED;void*_tmp1EE;void*_tmp1EF;_LL113: _tmp1EA=
_tmp1E9.f1;if((int)_tmp1EA != 2)goto _LL115;_tmp1EB=_tmp1E9.f2;if((int)_tmp1EB != 2)
goto _LL115;_LL114: return 1;_LL115: _tmp1EC=_tmp1E9.f2;if((int)_tmp1EC != 0)goto
_LL117;_LL116: return 1;_LL117: _tmp1ED=_tmp1E9.f1;if((int)_tmp1ED != 0)goto _LL119;
_LL118: return 0;_LL119: _tmp1EE=_tmp1E9.f2;if((int)_tmp1EE != 1)goto _LL11B;_LL11A:
return 1;_LL11B: _tmp1EF=_tmp1E9.f1;if((int)_tmp1EF != 1)goto _LL112;_LL11C: return 0;
_LL112:;}}static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*
r1s){if(r1s == r2s)return 1;for(0;r1s != 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1F0=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;int found=0;{struct Cyc_List_List*
_tmp1F1=r2s;for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){struct Cyc_CfFlowInfo_Reln*
_tmp1F2=(struct Cyc_CfFlowInfo_Reln*)_tmp1F1->hd;if(_tmp1F0 == _tmp1F2  || _tmp1F0->vd
== _tmp1F2->vd  && Cyc_CfFlowInfo_same_relop(_tmp1F0->rop,_tmp1F2->rop)){found=1;
break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume != c2.may_consume){
struct Cyc_List_List*_tmp1F3=c1.may_consume;struct Cyc_List_List*_tmp1F4=c2.may_consume;
for(0;_tmp1F4 != 0  && _tmp1F3 != 0;_tmp1F4=_tmp1F4->tl){if((unsigned int)_tmp1F3 == (
unsigned int)_tmp1F4)goto compare_consumed_set;{int _tmp1F5=Cyc_CfFlowInfo_place_cmp((
struct Cyc_CfFlowInfo_Place*)_tmp1F3->hd,(struct Cyc_CfFlowInfo_Place*)_tmp1F4->hd);
if(_tmp1F5 == 0)_tmp1F3=_tmp1F3->tl;}}if(_tmp1F3 != 0)return 0;}
compare_consumed_set: return Cyc_CfFlowInfo_place_set_subset(c1.consumed,c2.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union f1,
union Cyc_CfFlowInfo_FlowInfo_union f2){struct _tuple11 _tmp1F7=({struct _tuple11
_tmp1F6;_tmp1F6.f1=f1;_tmp1F6.f2=f2;_tmp1F6;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F8;union Cyc_CfFlowInfo_FlowInfo_union _tmp1F9;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1FA;struct Cyc_Dict_Dict _tmp1FB;struct Cyc_List_List*_tmp1FC;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1FD;union Cyc_CfFlowInfo_FlowInfo_union _tmp1FE;struct Cyc_Dict_Dict _tmp1FF;
struct Cyc_List_List*_tmp200;struct Cyc_CfFlowInfo_ConsumeInfo _tmp201;_LL11E:
_tmp1F8=_tmp1F7.f1;if(((_tmp1F7.f1).BottomFL).tag != 0)goto _LL120;_LL11F: return 1;
_LL120: _tmp1F9=_tmp1F7.f2;if(((_tmp1F7.f2).BottomFL).tag != 0)goto _LL122;_LL121:
return 0;_LL122: _tmp1FA=_tmp1F7.f1;if(((_tmp1F7.f1).ReachableFL).tag != 1)goto
_LL11D;_tmp1FB=(_tmp1FA.ReachableFL).f1;_tmp1FC=(_tmp1FA.ReachableFL).f2;_tmp1FD=(
_tmp1FA.ReachableFL).f3;_tmp1FE=_tmp1F7.f2;if(((_tmp1F7.f2).ReachableFL).tag != 1)
goto _LL11D;_tmp1FF=(_tmp1FE.ReachableFL).f1;_tmp200=(_tmp1FE.ReachableFL).f2;
_tmp201=(_tmp1FE.ReachableFL).f3;_LL123: if(((_tmp1FB.t == _tmp1FF.t  && _tmp1FC == 
_tmp200) && _tmp1FD.may_consume == _tmp201.may_consume) && (_tmp1FD.consumed).t == (
_tmp201.consumed).t)return 1;return(Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp1FB,_tmp1FF) && Cyc_CfFlowInfo_relns_approx(_tmp1FC,_tmp200)) && Cyc_CfFlowInfo_consume_approx(
_tmp1FD,_tmp201);_LL11D:;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=
0;for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp202=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp202->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp_union
_tmp203=_tmp202->rop;struct Cyc_Absyn_Vardecl*_tmp204;struct Cyc_Absyn_Vardecl*
_tmp205;struct Cyc_Absyn_Vardecl*_tmp206;_LL125: if((_tmp203.LessVar).tag != 1)goto
_LL127;_tmp204=(_tmp203.LessVar).f1;_LL126: _tmp205=_tmp204;goto _LL128;_LL127: if((
_tmp203.LessSize).tag != 2)goto _LL129;_tmp205=(_tmp203.LessSize).f1;_LL128:
_tmp206=_tmp205;goto _LL12A;_LL129: if((_tmp203.LessEqSize).tag != 4)goto _LL12B;
_tmp206=(_tmp203.LessEqSize).f1;_LL12A: if(v == _tmp206)found=1;goto _LL124;_LL12B:;
_LL12C: goto _LL124;_LL124:;}}if(!found)return rs;{struct Cyc_List_List*_tmp207=0;
for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp208=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp208->vd != v){{union Cyc_CfFlowInfo_RelnOp_union _tmp209=_tmp208->rop;
struct Cyc_Absyn_Vardecl*_tmp20A;struct Cyc_Absyn_Vardecl*_tmp20B;struct Cyc_Absyn_Vardecl*
_tmp20C;_LL12E: if((_tmp209.LessVar).tag != 1)goto _LL130;_tmp20A=(_tmp209.LessVar).f1;
_LL12F: _tmp20B=_tmp20A;goto _LL131;_LL130: if((_tmp209.LessSize).tag != 2)goto
_LL132;_tmp20B=(_tmp209.LessSize).f1;_LL131: _tmp20C=_tmp20B;goto _LL133;_LL132:
if((_tmp209.LessEqSize).tag != 4)goto _LL134;_tmp20C=(_tmp209.LessEqSize).f1;
_LL133: if(v == _tmp20C)continue;goto _LL12D;_LL134:;_LL135: goto _LL12D;_LL12D:;}
_tmp207=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->hd=
_tmp208;_tmp20D->tl=_tmp207;_tmp20D;});}}return _tmp207;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp20E=(void*)e->r;void*
_tmp20F;struct Cyc_Absyn_Vardecl*_tmp210;void*_tmp211;struct Cyc_Absyn_Vardecl*
_tmp212;void*_tmp213;struct Cyc_Absyn_Vardecl*_tmp214;void*_tmp215;struct Cyc_Absyn_Vardecl*
_tmp216;_LL137: if(*((int*)_tmp20E)!= 1)goto _LL139;_tmp20F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp20E)->f2;if(_tmp20F <= (void*)1)goto _LL139;if(*((int*)_tmp20F)!= 0)goto _LL139;
_tmp210=((struct Cyc_Absyn_Global_b_struct*)_tmp20F)->f1;_LL138: _tmp212=_tmp210;
goto _LL13A;_LL139: if(*((int*)_tmp20E)!= 1)goto _LL13B;_tmp211=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp20E)->f2;if(_tmp211 <= (void*)1)goto _LL13B;if(*((int*)_tmp211)!= 2)goto _LL13B;
_tmp212=((struct Cyc_Absyn_Param_b_struct*)_tmp211)->f1;_LL13A: _tmp214=_tmp212;
goto _LL13C;_LL13B: if(*((int*)_tmp20E)!= 1)goto _LL13D;_tmp213=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp20E)->f2;if(_tmp213 <= (void*)1)goto _LL13D;if(*((int*)_tmp213)!= 3)goto _LL13D;
_tmp214=((struct Cyc_Absyn_Local_b_struct*)_tmp213)->f1;_LL13C: _tmp216=_tmp214;
goto _LL13E;_LL13D: if(*((int*)_tmp20E)!= 1)goto _LL13F;_tmp215=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp20E)->f2;if(_tmp215 <= (void*)1)goto _LL13F;if(*((int*)_tmp215)!= 4)goto _LL13F;
_tmp216=((struct Cyc_Absyn_Pat_b_struct*)_tmp215)->f1;_LL13E: if(!_tmp216->escapes)
return Cyc_CfFlowInfo_reln_kill_var(r,_tmp216);goto _LL136;_LL13F:;_LL140: goto
_LL136;_LL136:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)
return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{void*_tmp217=(void*)e->r;struct Cyc_Absyn_MallocInfo
_tmp218;struct Cyc_Absyn_Exp*_tmp219;int _tmp21A;_LL142: if(*((int*)_tmp217)!= 35)
goto _LL144;_tmp218=((struct Cyc_Absyn_Malloc_e_struct*)_tmp217)->f1;_tmp219=
_tmp218.num_elts;_tmp21A=_tmp218.fat_result;if(_tmp21A != 1)goto _LL144;_LL143:
malloc_loop: {void*_tmp21B=(void*)_tmp219->r;struct Cyc_Absyn_Exp*_tmp21C;void*
_tmp21D;struct Cyc_Absyn_Vardecl*_tmp21E;void*_tmp21F;struct Cyc_Absyn_Vardecl*
_tmp220;void*_tmp221;struct Cyc_Absyn_Vardecl*_tmp222;void*_tmp223;struct Cyc_Absyn_Vardecl*
_tmp224;_LL147: if(*((int*)_tmp21B)!= 15)goto _LL149;_tmp21C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp21B)->f2;_LL148: _tmp219=_tmp21C;goto malloc_loop;_LL149: if(*((int*)_tmp21B)!= 
1)goto _LL14B;_tmp21D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp21B)->f2;if(
_tmp21D <= (void*)1)goto _LL14B;if(*((int*)_tmp21D)!= 4)goto _LL14B;_tmp21E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp21D)->f1;_LL14A: _tmp220=_tmp21E;goto _LL14C;
_LL14B: if(*((int*)_tmp21B)!= 1)goto _LL14D;_tmp21F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp21B)->f2;if(_tmp21F <= (void*)1)goto _LL14D;if(*((int*)_tmp21F)!= 3)goto _LL14D;
_tmp220=((struct Cyc_Absyn_Local_b_struct*)_tmp21F)->f1;_LL14C: _tmp222=_tmp220;
goto _LL14E;_LL14D: if(*((int*)_tmp21B)!= 1)goto _LL14F;_tmp221=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp21B)->f2;if(_tmp221 <= (void*)1)goto _LL14F;if(*((int*)_tmp221)!= 2)goto _LL14F;
_tmp222=((struct Cyc_Absyn_Param_b_struct*)_tmp221)->f1;_LL14E: _tmp224=_tmp222;
goto _LL150;_LL14F: if(*((int*)_tmp21B)!= 1)goto _LL151;_tmp223=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp21B)->f2;if(_tmp223 <= (void*)1)goto _LL151;if(*((int*)_tmp223)!= 0)goto _LL151;
_tmp224=((struct Cyc_Absyn_Global_b_struct*)_tmp223)->f1;_LL150: if(_tmp224->escapes)
return r;return({struct Cyc_List_List*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp226=_cycalloc(sizeof(*_tmp226));_tmp226->vd=
_tmp224;_tmp226->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp227;(_tmp227.LessEqSize).tag=4;(_tmp227.LessEqSize).f1=v;_tmp227;});_tmp226;});
_tmp225->tl=r;_tmp225;});_LL151:;_LL152: return r;_LL146:;}_LL144:;_LL145: goto
_LL141;_LL141:;}{void*_tmp228=Cyc_Tcutil_compress((void*)v->type);_LL154: if(
_tmp228 <= (void*)4)goto _LL156;if(*((int*)_tmp228)!= 5)goto _LL156;_LL155: goto
_LL153;_LL156:;_LL157: return r;_LL153:;}loop: {void*_tmp229=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp22A;union Cyc_Absyn_Cnst_union _tmp22B;int _tmp22C;void*_tmp22D;struct Cyc_List_List*
_tmp22E;struct Cyc_List_List _tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_List_List
_tmp231;struct Cyc_Absyn_Exp*_tmp232;void*_tmp233;struct Cyc_List_List*_tmp234;
struct Cyc_List_List _tmp235;struct Cyc_Absyn_Exp*_tmp236;_LL159: if(*((int*)_tmp229)
!= 15)goto _LL15B;_tmp22A=((struct Cyc_Absyn_Cast_e_struct*)_tmp229)->f2;_LL15A: e=
_tmp22A;goto loop;_LL15B: if(*((int*)_tmp229)!= 0)goto _LL15D;_tmp22B=((struct Cyc_Absyn_Const_e_struct*)
_tmp229)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp229)->f1).Int_c).tag != 2)
goto _LL15D;_tmp22C=(_tmp22B.Int_c).f2;_LL15C: return({struct Cyc_List_List*_tmp237=
_cycalloc(sizeof(*_tmp237));_tmp237->hd=({struct Cyc_CfFlowInfo_Reln*_tmp238=
_cycalloc(sizeof(*_tmp238));_tmp238->vd=v;_tmp238->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp239;(_tmp239.EqualConst).tag=0;(_tmp239.EqualConst).f1=(
unsigned int)_tmp22C;_tmp239;});_tmp238;});_tmp237->tl=r;_tmp237;});_LL15D: if(*((
int*)_tmp229)!= 3)goto _LL15F;_tmp22D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp229)->f1;if((int)_tmp22D != 4)goto _LL15F;_tmp22E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp229)->f2;if(_tmp22E == 0)goto _LL15F;_tmp22F=*_tmp22E;_tmp230=_tmp22F.tl;if(
_tmp230 == 0)goto _LL15F;_tmp231=*_tmp230;_tmp232=(struct Cyc_Absyn_Exp*)_tmp231.hd;
_LL15E:{void*_tmp23A=(void*)_tmp232->r;void*_tmp23B;struct Cyc_List_List*_tmp23C;
struct Cyc_List_List _tmp23D;struct Cyc_Absyn_Exp*_tmp23E;_LL164: if(*((int*)_tmp23A)
!= 3)goto _LL166;_tmp23B=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp23A)->f1;
if((int)_tmp23B != 19)goto _LL166;_tmp23C=((struct Cyc_Absyn_Primop_e_struct*)
_tmp23A)->f2;if(_tmp23C == 0)goto _LL166;_tmp23D=*_tmp23C;_tmp23E=(struct Cyc_Absyn_Exp*)
_tmp23D.hd;_LL165:{void*_tmp23F=(void*)_tmp23E->r;void*_tmp240;struct Cyc_Absyn_Vardecl*
_tmp241;void*_tmp242;struct Cyc_Absyn_Vardecl*_tmp243;void*_tmp244;struct Cyc_Absyn_Vardecl*
_tmp245;void*_tmp246;struct Cyc_Absyn_Vardecl*_tmp247;_LL169: if(*((int*)_tmp23F)
!= 1)goto _LL16B;_tmp240=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp23F)->f2;if(
_tmp240 <= (void*)1)goto _LL16B;if(*((int*)_tmp240)!= 0)goto _LL16B;_tmp241=((
struct Cyc_Absyn_Global_b_struct*)_tmp240)->f1;_LL16A: _tmp243=_tmp241;goto _LL16C;
_LL16B: if(*((int*)_tmp23F)!= 1)goto _LL16D;_tmp242=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23F)->f2;if(_tmp242 <= (void*)1)goto _LL16D;if(*((int*)_tmp242)!= 3)goto _LL16D;
_tmp243=((struct Cyc_Absyn_Local_b_struct*)_tmp242)->f1;_LL16C: _tmp245=_tmp243;
goto _LL16E;_LL16D: if(*((int*)_tmp23F)!= 1)goto _LL16F;_tmp244=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23F)->f2;if(_tmp244 <= (void*)1)goto _LL16F;if(*((int*)_tmp244)!= 2)goto _LL16F;
_tmp245=((struct Cyc_Absyn_Param_b_struct*)_tmp244)->f1;_LL16E: _tmp247=_tmp245;
goto _LL170;_LL16F: if(*((int*)_tmp23F)!= 1)goto _LL171;_tmp246=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23F)->f2;if(_tmp246 <= (void*)1)goto _LL171;if(*((int*)_tmp246)!= 4)goto _LL171;
_tmp247=((struct Cyc_Absyn_Pat_b_struct*)_tmp246)->f1;_LL170: if(_tmp247->escapes)
return r;return({struct Cyc_List_List*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249->vd=v;
_tmp249->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp24A;(_tmp24A.LessSize).tag=2;(_tmp24A.LessSize).f1=_tmp247;_tmp24A;});
_tmp249;});_tmp248->tl=r;_tmp248;});_LL171:;_LL172: goto _LL168;_LL168:;}goto
_LL163;_LL166:;_LL167: goto _LL163;_LL163:;}goto _LL158;_LL15F: if(*((int*)_tmp229)
!= 3)goto _LL161;_tmp233=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp229)->f1;
if((int)_tmp233 != 19)goto _LL161;_tmp234=((struct Cyc_Absyn_Primop_e_struct*)
_tmp229)->f2;if(_tmp234 == 0)goto _LL161;_tmp235=*_tmp234;_tmp236=(struct Cyc_Absyn_Exp*)
_tmp235.hd;_LL160:{void*_tmp24B=(void*)_tmp236->r;void*_tmp24C;struct Cyc_Absyn_Vardecl*
_tmp24D;void*_tmp24E;struct Cyc_Absyn_Vardecl*_tmp24F;void*_tmp250;struct Cyc_Absyn_Vardecl*
_tmp251;void*_tmp252;struct Cyc_Absyn_Vardecl*_tmp253;_LL174: if(*((int*)_tmp24B)
!= 1)goto _LL176;_tmp24C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24B)->f2;if(
_tmp24C <= (void*)1)goto _LL176;if(*((int*)_tmp24C)!= 0)goto _LL176;_tmp24D=((
struct Cyc_Absyn_Global_b_struct*)_tmp24C)->f1;_LL175: _tmp24F=_tmp24D;goto _LL177;
_LL176: if(*((int*)_tmp24B)!= 1)goto _LL178;_tmp24E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24B)->f2;if(_tmp24E <= (void*)1)goto _LL178;if(*((int*)_tmp24E)!= 3)goto _LL178;
_tmp24F=((struct Cyc_Absyn_Local_b_struct*)_tmp24E)->f1;_LL177: _tmp251=_tmp24F;
goto _LL179;_LL178: if(*((int*)_tmp24B)!= 1)goto _LL17A;_tmp250=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24B)->f2;if(_tmp250 <= (void*)1)goto _LL17A;if(*((int*)_tmp250)!= 2)goto _LL17A;
_tmp251=((struct Cyc_Absyn_Param_b_struct*)_tmp250)->f1;_LL179: _tmp253=_tmp251;
goto _LL17B;_LL17A: if(*((int*)_tmp24B)!= 1)goto _LL17C;_tmp252=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24B)->f2;if(_tmp252 <= (void*)1)goto _LL17C;if(*((int*)_tmp252)!= 4)goto _LL17C;
_tmp253=((struct Cyc_Absyn_Pat_b_struct*)_tmp252)->f1;_LL17B: if(_tmp253->escapes)
return r;return({struct Cyc_List_List*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255->vd=v;
_tmp255->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp256;(_tmp256.LessEqSize).tag=4;(_tmp256.LessEqSize).f1=_tmp253;_tmp256;});
_tmp255;});_tmp254->tl=r;_tmp254;});_LL17C:;_LL17D: goto _LL173;_LL173:;}goto
_LL158;_LL161:;_LL162: goto _LL158;_LL158:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*
_tmp257=(void*)e1->r;void*_tmp258;struct Cyc_Absyn_Vardecl*_tmp259;void*_tmp25A;
struct Cyc_Absyn_Vardecl*_tmp25B;void*_tmp25C;struct Cyc_Absyn_Vardecl*_tmp25D;
void*_tmp25E;struct Cyc_Absyn_Vardecl*_tmp25F;_LL17F: if(*((int*)_tmp257)!= 1)goto
_LL181;_tmp258=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp257)->f2;if(_tmp258 <= (
void*)1)goto _LL181;if(*((int*)_tmp258)!= 0)goto _LL181;_tmp259=((struct Cyc_Absyn_Global_b_struct*)
_tmp258)->f1;_LL180: _tmp25B=_tmp259;goto _LL182;_LL181: if(*((int*)_tmp257)!= 1)
goto _LL183;_tmp25A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp257)->f2;if(
_tmp25A <= (void*)1)goto _LL183;if(*((int*)_tmp25A)!= 2)goto _LL183;_tmp25B=((
struct Cyc_Absyn_Param_b_struct*)_tmp25A)->f1;_LL182: _tmp25D=_tmp25B;goto _LL184;
_LL183: if(*((int*)_tmp257)!= 1)goto _LL185;_tmp25C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp257)->f2;if(_tmp25C <= (void*)1)goto _LL185;if(*((int*)_tmp25C)!= 3)goto _LL185;
_tmp25D=((struct Cyc_Absyn_Local_b_struct*)_tmp25C)->f1;_LL184: _tmp25F=_tmp25D;
goto _LL186;_LL185: if(*((int*)_tmp257)!= 1)goto _LL187;_tmp25E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp257)->f2;if(_tmp25E <= (void*)1)goto _LL187;if(*((int*)_tmp25E)!= 4)goto _LL187;
_tmp25F=((struct Cyc_Absyn_Pat_b_struct*)_tmp25E)->f1;_LL186: if(!_tmp25F->escapes)
return Cyc_CfFlowInfo_reln_assign_var(r,_tmp25F,e2);goto _LL17E;_LL187:;_LL188:
goto _LL17E;_LL17E:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){({struct Cyc_String_pa_struct _tmp262;_tmp262.tag=0;_tmp262.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string((r->vd)->name));{
void*_tmp260[1]={& _tmp262};Cyc_fprintf(Cyc_stderr,({const char*_tmp261="%s";
_tag_dynforward(_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,3));}),
_tag_dynforward(_tmp260,sizeof(void*),1));}});{union Cyc_CfFlowInfo_RelnOp_union
_tmp263=r->rop;unsigned int _tmp264;struct Cyc_Absyn_Vardecl*_tmp265;struct Cyc_Absyn_Vardecl*
_tmp266;unsigned int _tmp267;struct Cyc_Absyn_Vardecl*_tmp268;_LL18A: if((_tmp263.EqualConst).tag
!= 0)goto _LL18C;_tmp264=(_tmp263.EqualConst).f1;_LL18B:({struct Cyc_Int_pa_struct
_tmp26B;_tmp26B.tag=1;_tmp26B.f1=(unsigned long)((int)_tmp264);{void*_tmp269[1]={&
_tmp26B};Cyc_fprintf(Cyc_stderr,({const char*_tmp26A="==%d";_tag_dynforward(
_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,5));}),_tag_dynforward(_tmp269,
sizeof(void*),1));}});goto _LL189;_LL18C: if((_tmp263.LessVar).tag != 1)goto _LL18E;
_tmp265=(_tmp263.LessVar).f1;_LL18D:({struct Cyc_String_pa_struct _tmp26E;_tmp26E.tag=
0;_tmp26E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp265->name));{void*_tmp26C[1]={& _tmp26E};Cyc_fprintf(Cyc_stderr,({const char*
_tmp26D="<%s";_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,4));}),
_tag_dynforward(_tmp26C,sizeof(void*),1));}});goto _LL189;_LL18E: if((_tmp263.LessSize).tag
!= 2)goto _LL190;_tmp266=(_tmp263.LessSize).f1;_LL18F:({struct Cyc_String_pa_struct
_tmp271;_tmp271.tag=0;_tmp271.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp266->name));{void*_tmp26F[1]={& _tmp271};Cyc_fprintf(
Cyc_stderr,({const char*_tmp270="<%s.size";_tag_dynforward(_tmp270,sizeof(char),
_get_zero_arr_size(_tmp270,9));}),_tag_dynforward(_tmp26F,sizeof(void*),1));}});
goto _LL189;_LL190: if((_tmp263.LessConst).tag != 3)goto _LL192;_tmp267=(_tmp263.LessConst).f1;
_LL191:({struct Cyc_Int_pa_struct _tmp274;_tmp274.tag=1;_tmp274.f1=(unsigned long)((
int)_tmp267);{void*_tmp272[1]={& _tmp274};Cyc_fprintf(Cyc_stderr,({const char*
_tmp273="<%d";_tag_dynforward(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,4));}),
_tag_dynforward(_tmp272,sizeof(void*),1));}});goto _LL189;_LL192: if((_tmp263.LessEqSize).tag
!= 4)goto _LL189;_tmp268=(_tmp263.LessEqSize).f1;_LL193:({struct Cyc_String_pa_struct
_tmp277;_tmp277.tag=0;_tmp277.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp268->name));{void*_tmp275[1]={& _tmp277};Cyc_fprintf(
Cyc_stderr,({const char*_tmp276="<=%s.size";_tag_dynforward(_tmp276,sizeof(char),
_get_zero_arr_size(_tmp276,10));}),_tag_dynforward(_tmp275,sizeof(void*),1));}});
goto _LL189;_LL189:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;
r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl
!= 0)({void*_tmp278[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp279=",";
_tag_dynforward(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,2));}),
_tag_dynforward(_tmp278,sizeof(void*),0));});}}void Cyc_CfFlowInfo_print_initlevel(
void*il){void*_tmp27A=il;_LL195: if((int)_tmp27A != 0)goto _LL197;_LL196:({void*
_tmp27B[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp27C="uninitialized";
_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,14));}),
_tag_dynforward(_tmp27B,sizeof(void*),0));});goto _LL194;_LL197: if((int)_tmp27A != 
1)goto _LL199;_LL198:({void*_tmp27D[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp27E="this-initialized";_tag_dynforward(_tmp27E,sizeof(char),
_get_zero_arr_size(_tmp27E,17));}),_tag_dynforward(_tmp27D,sizeof(void*),0));});
goto _LL194;_LL199: if((int)_tmp27A != 2)goto _LL194;_LL19A:({void*_tmp27F[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp280="all-initialized";_tag_dynforward(_tmp280,sizeof(
char),_get_zero_arr_size(_tmp280,16));}),_tag_dynforward(_tmp27F,sizeof(void*),0));});
goto _LL194;_LL194:;}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp281=root;
struct Cyc_Absyn_Vardecl*_tmp282;struct Cyc_Absyn_Exp*_tmp283;void*_tmp284;_LL19C:
if(*((int*)_tmp281)!= 0)goto _LL19E;_tmp282=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp281)->f1;_LL19D:({struct Cyc_String_pa_struct _tmp287;_tmp287.tag=0;_tmp287.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp282->name));{
void*_tmp285[1]={& _tmp287};Cyc_fprintf(Cyc_stderr,({const char*_tmp286="Root(%s): ";
_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,11));}),
_tag_dynforward(_tmp285,sizeof(void*),1));}});goto _LL19B;_LL19E: if(*((int*)
_tmp281)!= 1)goto _LL1A0;_tmp283=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp281)->f1;
_tmp284=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp281)->f2;_LL19F:({
struct Cyc_String_pa_struct _tmp28B;_tmp28B.tag=0;_tmp28B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp284));{struct
Cyc_String_pa_struct _tmp28A;_tmp28A.tag=0;_tmp28A.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp283));{void*_tmp288[2]={&
_tmp28A,& _tmp28B};Cyc_fprintf(Cyc_stderr,({const char*_tmp289="MallocPt(%s,%s)";
_tag_dynforward(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,16));}),
_tag_dynforward(_tmp288,sizeof(void*),2));}}});goto _LL19B;_LL1A0: if(*((int*)
_tmp281)!= 2)goto _LL19B;_LL1A1:({void*_tmp28C[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp28D="InitParam(_,_)";_tag_dynforward(_tmp28D,sizeof(char),
_get_zero_arr_size(_tmp28D,15));}),_tag_dynforward(_tmp28C,sizeof(void*),0));});
goto _LL19B;_LL19B:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root((void*)p->root);if(p->fields != 0)({void*_tmp28E[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp28F="+(...)";_tag_dynforward(_tmp28F,
sizeof(char),_get_zero_arr_size(_tmp28F,7));}),_tag_dynforward(_tmp28E,sizeof(
void*),0));});}void Cyc_CfFlowInfo_print_place_set(struct Cyc_Dict_Dict p){struct
_RegionHandle _tmp290=_new_region("r");struct _RegionHandle*r=& _tmp290;
_push_region(r);{struct _handler_cons _tmp291;_push_handler(& _tmp291);{int _tmp293=
0;if(setjmp(_tmp291.handler))_tmp293=1;if(!_tmp293){{struct Cyc_Iter_Iter _tmp294=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,p);struct _tuple6 _tmp295=*((struct _tuple6*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict
d))Cyc_Dict_rchoose)(r,p);int first=1;({void*_tmp296[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp297="{ ";_tag_dynforward(_tmp297,sizeof(char),_get_zero_arr_size(
_tmp297,3));}),_tag_dynforward(_tmp296,sizeof(void*),0));});while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple6*))Cyc_Iter_next)(_tmp294,& _tmp295)){struct Cyc_CfFlowInfo_Place*
_tmp298=_tmp295.f1;if(!first){({void*_tmp299[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp29A=", ";_tag_dynforward(_tmp29A,sizeof(char),_get_zero_arr_size(
_tmp29A,3));}),_tag_dynforward(_tmp299,sizeof(void*),0));});first=0;}Cyc_CfFlowInfo_print_place(
_tmp298);}({void*_tmp29B[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp29C="}\n";
_tag_dynforward(_tmp29C,sizeof(char),_get_zero_arr_size(_tmp29C,3));}),
_tag_dynforward(_tmp29B,sizeof(void*),0));});};_pop_handler();}else{void*_tmp292=(
void*)_exn_thrown;void*_tmp29E=_tmp292;_LL1A3:;_LL1A4:({void*_tmp29F[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2A0="{ }\n";_tag_dynforward(_tmp2A0,sizeof(char),
_get_zero_arr_size(_tmp2A0,5));}),_tag_dynforward(_tmp29F,sizeof(void*),0));});
_npop_handler(0);return;_LL1A5:;_LL1A6:(void)_throw(_tmp29E);_LL1A2:;}}};
_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*_tmp2A1=rval;
void*_tmp2A2;void*_tmp2A3;struct Cyc_CfFlowInfo_Place*_tmp2A4;struct
_dynforward_ptr _tmp2A5;_LL1A8: if((int)_tmp2A1 != 0)goto _LL1AA;_LL1A9:({void*
_tmp2A6[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A7="Zero";_tag_dynforward(
_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,5));}),_tag_dynforward(_tmp2A6,
sizeof(void*),0));});goto _LL1A7;_LL1AA: if((int)_tmp2A1 != 1)goto _LL1AC;_LL1AB:({
void*_tmp2A8[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A9="NotZeroAll";
_tag_dynforward(_tmp2A9,sizeof(char),_get_zero_arr_size(_tmp2A9,11));}),
_tag_dynforward(_tmp2A8,sizeof(void*),0));});goto _LL1A7;_LL1AC: if((int)_tmp2A1 != 
2)goto _LL1AE;_LL1AD:({void*_tmp2AA[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2AB="NotZeroThis";_tag_dynforward(_tmp2AB,sizeof(char),_get_zero_arr_size(
_tmp2AB,12));}),_tag_dynforward(_tmp2AA,sizeof(void*),0));});goto _LL1A7;_LL1AE:
if(_tmp2A1 <= (void*)3)goto _LL1B0;if(*((int*)_tmp2A1)!= 0)goto _LL1B0;_tmp2A2=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2A1)->f1;_LL1AF:({void*_tmp2AC[
0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AD="Unknown(";_tag_dynforward(
_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,9));}),_tag_dynforward(_tmp2AC,
sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(_tmp2A2);({void*_tmp2AE[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp2AF=")";_tag_dynforward(_tmp2AF,sizeof(
char),_get_zero_arr_size(_tmp2AF,2));}),_tag_dynforward(_tmp2AE,sizeof(void*),0));});
goto _LL1A7;_LL1B0: if(_tmp2A1 <= (void*)3)goto _LL1B2;if(*((int*)_tmp2A1)!= 1)goto
_LL1B2;_tmp2A3=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp2A1)->f1;_LL1B1:({
void*_tmp2B0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B1="Esc(";
_tag_dynforward(_tmp2B1,sizeof(char),_get_zero_arr_size(_tmp2B1,5));}),
_tag_dynforward(_tmp2B0,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(
_tmp2A3);({void*_tmp2B2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B3=")";
_tag_dynforward(_tmp2B3,sizeof(char),_get_zero_arr_size(_tmp2B3,2));}),
_tag_dynforward(_tmp2B2,sizeof(void*),0));});goto _LL1A7;_LL1B2: if(_tmp2A1 <= (
void*)3)goto _LL1B4;if(*((int*)_tmp2A1)!= 2)goto _LL1B4;_tmp2A4=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2A1)->f1;_LL1B3:({void*_tmp2B4[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2B5="AddrOf(";_tag_dynforward(_tmp2B5,sizeof(char),_get_zero_arr_size(
_tmp2B5,8));}),_tag_dynforward(_tmp2B4,sizeof(void*),0));});Cyc_CfFlowInfo_print_place(
_tmp2A4);({void*_tmp2B6[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B7=")";
_tag_dynforward(_tmp2B7,sizeof(char),_get_zero_arr_size(_tmp2B7,2));}),
_tag_dynforward(_tmp2B6,sizeof(void*),0));});goto _LL1A7;_LL1B4: if(_tmp2A1 <= (
void*)3)goto _LL1B6;if(*((int*)_tmp2A1)!= 3)goto _LL1B6;_LL1B5:({void*_tmp2B8[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp2B9="TagCmps(?)";_tag_dynforward(_tmp2B9,
sizeof(char),_get_zero_arr_size(_tmp2B9,11));}),_tag_dynforward(_tmp2B8,sizeof(
void*),0));});goto _LL1A7;_LL1B6: if(_tmp2A1 <= (void*)3)goto _LL1A7;if(*((int*)
_tmp2A1)!= 4)goto _LL1A7;_tmp2A5=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2A1)->f1;
_LL1B7:({void*_tmp2BA[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BB="Aggregate{";
_tag_dynforward(_tmp2BB,sizeof(char),_get_zero_arr_size(_tmp2BB,11));}),
_tag_dynforward(_tmp2BA,sizeof(void*),0));});{int i=0;for(0;i < 
_get_dynforward_size(_tmp2A5,sizeof(void*));i ++){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp2A5.curr)[i]);if(i + 1 < _get_dynforward_size(_tmp2A5,sizeof(void*)))({
void*_tmp2BC[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BD=",";
_tag_dynforward(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,2));}),
_tag_dynforward(_tmp2BC,sizeof(void*),0));});}}({void*_tmp2BE[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2BF="}";_tag_dynforward(_tmp2BF,sizeof(char),
_get_zero_arr_size(_tmp2BF,2));}),_tag_dynforward(_tmp2BE,sizeof(void*),0));});
goto _LL1A7;_LL1A7:;}static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*
rval){({void*_tmp2C0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C1="    ";
_tag_dynforward(_tmp2C1,sizeof(char),_get_zero_arr_size(_tmp2C1,5));}),
_tag_dynforward(_tmp2C0,sizeof(void*),0));});Cyc_CfFlowInfo_print_root(root);({
void*_tmp2C2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C3=" --> ";
_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,6));}),
_tag_dynforward(_tmp2C2,sizeof(void*),0));});Cyc_CfFlowInfo_print_absrval(rval);({
void*_tmp2C4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C5="\n";
_tag_dynforward(_tmp2C5,sizeof(char),_get_zero_arr_size(_tmp2C5,2));}),
_tag_dynforward(_tmp2C4,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_flowdict(
struct Cyc_Dict_Dict d){Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}void
Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp2C6=f;struct Cyc_Dict_Dict _tmp2C7;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2C8;
_LL1B9: if((_tmp2C6.BottomFL).tag != 0)goto _LL1BB;_LL1BA:({void*_tmp2C9[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2CA="  BottomFL\n";_tag_dynforward(_tmp2CA,sizeof(
char),_get_zero_arr_size(_tmp2CA,12));}),_tag_dynforward(_tmp2C9,sizeof(void*),0));});
goto _LL1B8;_LL1BB: if((_tmp2C6.ReachableFL).tag != 1)goto _LL1B8;_tmp2C7=(_tmp2C6.ReachableFL).f1;
_tmp2C8=(_tmp2C6.ReachableFL).f3;_LL1BC:({void*_tmp2CB[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp2CC="  ReachableFL:\n";_tag_dynforward(_tmp2CC,sizeof(char),
_get_zero_arr_size(_tmp2CC,16));}),_tag_dynforward(_tmp2CB,sizeof(void*),0));});
Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,_tmp2C7);({void*_tmp2CD[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp2CE="\n    consumed: ";_tag_dynforward(
_tmp2CE,sizeof(char),_get_zero_arr_size(_tmp2CE,16));}),_tag_dynforward(_tmp2CD,
sizeof(void*),0));});Cyc_CfFlowInfo_print_place_set(_tmp2C8.consumed);({void*
_tmp2CF[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D0="\n    may_consume: ";
_tag_dynforward(_tmp2D0,sizeof(char),_get_zero_arr_size(_tmp2D0,19));}),
_tag_dynforward(_tmp2CF,sizeof(void*),0));});Cyc_CfFlowInfo_print_place_list(
_tmp2C8.may_consume);({void*_tmp2D1[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2D2="\n";_tag_dynforward(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,2));}),
_tag_dynforward(_tmp2D1,sizeof(void*),0));});goto _LL1B8;_LL1B8:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t){void*_tmp2D3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp2D4;struct Cyc_Absyn_TunionInfo _tmp2D5;struct Cyc_List_List*_tmp2D6;struct Cyc_Core_Opt*
_tmp2D7;struct Cyc_List_List*_tmp2D8;struct Cyc_Absyn_AggrInfo _tmp2D9;struct Cyc_List_List*
_tmp2DA;struct Cyc_Absyn_TunionFieldInfo _tmp2DB;struct Cyc_List_List*_tmp2DC;
struct Cyc_List_List*_tmp2DD;struct Cyc_Absyn_PtrInfo _tmp2DE;void*_tmp2DF;struct
Cyc_Absyn_PtrAtts _tmp2E0;void*_tmp2E1;struct Cyc_List_List*_tmp2E2;struct Cyc_List_List*
_tmp2E3;struct Cyc_Absyn_ArrayInfo _tmp2E4;void*_tmp2E5;void*_tmp2E6;void*_tmp2E7;
void*_tmp2E8;void*_tmp2E9;void*_tmp2EA;_LL1BE: if((int)_tmp2D3 != 0)goto _LL1C0;
_LL1BF: goto _LL1C1;_LL1C0: if(_tmp2D3 <= (void*)4)goto _LL1C2;if(*((int*)_tmp2D3)!= 
5)goto _LL1C2;_LL1C1: goto _LL1C3;_LL1C2: if((int)_tmp2D3 != 1)goto _LL1C4;_LL1C3: goto
_LL1C5;_LL1C4: if(_tmp2D3 <= (void*)4)goto _LL1D0;if(*((int*)_tmp2D3)!= 6)goto
_LL1C6;_LL1C5: goto _LL1C7;_LL1C6: if(*((int*)_tmp2D3)!= 12)goto _LL1C8;_LL1C7: goto
_LL1C9;_LL1C8: if(*((int*)_tmp2D3)!= 13)goto _LL1CA;_LL1C9: goto _LL1CB;_LL1CA: if(*((
int*)_tmp2D3)!= 14)goto _LL1CC;_LL1CB: goto _LL1CD;_LL1CC: if(*((int*)_tmp2D3)!= 18)
goto _LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(*((int*)_tmp2D3)!= 19)goto _LL1D0;_LL1CF:
goto _LL1D1;_LL1D0: if((int)_tmp2D3 != 2)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2: if(
_tmp2D3 <= (void*)4)goto _LL1D4;if(*((int*)_tmp2D3)!= 0)goto _LL1D4;_LL1D3: return 0;
_LL1D4: if((int)_tmp2D3 != 3)goto _LL1D6;_LL1D5: {void*_tmp2EB=rgn;_LL1F5: if((int)
_tmp2EB != 0)goto _LL1F7;_LL1F6: return 1;_LL1F7:;_LL1F8: return 0;_LL1F4:;}_LL1D6: if(
_tmp2D3 <= (void*)4)goto _LL1D8;if(*((int*)_tmp2D3)!= 1)goto _LL1D8;_tmp2D4=((
struct Cyc_Absyn_VarType_struct*)_tmp2D3)->f1;_LL1D7: {void*_tmp2EC=rgn;struct Cyc_Absyn_Tvar*
_tmp2ED;_LL1FA: if((int)_tmp2EC != 0)goto _LL1FC;_LL1FB: return 0;_LL1FC: if(_tmp2EC <= (
void*)1)goto _LL1F9;if(*((int*)_tmp2EC)!= 0)goto _LL1F9;_tmp2ED=((struct Cyc_CfFlowInfo_Region_k_struct*)
_tmp2EC)->f1;_LL1FD: return Cyc_Absyn_tvar_cmp(_tmp2D4,_tmp2ED)== 0;_LL1F9:;}
_LL1D8: if(_tmp2D3 <= (void*)4)goto _LL1DA;if(*((int*)_tmp2D3)!= 2)goto _LL1DA;
_tmp2D5=((struct Cyc_Absyn_TunionType_struct*)_tmp2D3)->f1;_tmp2D6=_tmp2D5.targs;
_tmp2D7=_tmp2D5.rgn;_LL1D9: if((unsigned int)_tmp2D7  && Cyc_CfFlowInfo_contains_region(
rgn,(void*)_tmp2D7->v))return 1;_tmp2D8=_tmp2D6;goto _LL1DB;_LL1DA: if(_tmp2D3 <= (
void*)4)goto _LL1DC;if(*((int*)_tmp2D3)!= 17)goto _LL1DC;_tmp2D8=((struct Cyc_Absyn_TypedefType_struct*)
_tmp2D3)->f2;_LL1DB: _tmp2DA=_tmp2D8;goto _LL1DD;_LL1DC: if(_tmp2D3 <= (void*)4)goto
_LL1DE;if(*((int*)_tmp2D3)!= 10)goto _LL1DE;_tmp2D9=((struct Cyc_Absyn_AggrType_struct*)
_tmp2D3)->f1;_tmp2DA=_tmp2D9.targs;_LL1DD: _tmp2DC=_tmp2DA;goto _LL1DF;_LL1DE: if(
_tmp2D3 <= (void*)4)goto _LL1E0;if(*((int*)_tmp2D3)!= 3)goto _LL1E0;_tmp2DB=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2D3)->f1;_tmp2DC=_tmp2DB.targs;
_LL1DF: _tmp2DD=_tmp2DC;goto _LL1E1;_LL1E0: if(_tmp2D3 <= (void*)4)goto _LL1E2;if(*((
int*)_tmp2D3)!= 21)goto _LL1E2;_tmp2DD=((struct Cyc_Absyn_JoinEff_struct*)_tmp2D3)->f1;
_LL1E1: return Cyc_List_exists_c(Cyc_CfFlowInfo_contains_region,rgn,_tmp2DD);
_LL1E2: if(_tmp2D3 <= (void*)4)goto _LL1E4;if(*((int*)_tmp2D3)!= 4)goto _LL1E4;
_tmp2DE=((struct Cyc_Absyn_PointerType_struct*)_tmp2D3)->f1;_tmp2DF=(void*)
_tmp2DE.elt_typ;_tmp2E0=_tmp2DE.ptr_atts;_tmp2E1=(void*)_tmp2E0.rgn;_LL1E3:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp2E1) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp2DF);_LL1E4: if(_tmp2D3 <= (void*)4)goto _LL1E6;if(*((int*)_tmp2D3)!= 8)
goto _LL1E6;_LL1E5: return 0;_LL1E6: if(_tmp2D3 <= (void*)4)goto _LL1E8;if(*((int*)
_tmp2D3)!= 9)goto _LL1E8;_tmp2E2=((struct Cyc_Absyn_TupleType_struct*)_tmp2D3)->f1;
_LL1E7: for(0;_tmp2E2 != 0;_tmp2E2=_tmp2E2->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple7*)_tmp2E2->hd)).f2))return 1;}return 0;_LL1E8: if(_tmp2D3 <= (
void*)4)goto _LL1EA;if(*((int*)_tmp2D3)!= 11)goto _LL1EA;_tmp2E3=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2D3)->f2;_LL1E9: for(0;_tmp2E3 != 0;_tmp2E3=_tmp2E3->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp2E3->hd)->type))return 1;}return 0;
_LL1EA: if(_tmp2D3 <= (void*)4)goto _LL1EC;if(*((int*)_tmp2D3)!= 7)goto _LL1EC;
_tmp2E4=((struct Cyc_Absyn_ArrayType_struct*)_tmp2D3)->f1;_tmp2E5=(void*)_tmp2E4.elt_type;
_LL1EB: _tmp2E6=_tmp2E5;goto _LL1ED;_LL1EC: if(_tmp2D3 <= (void*)4)goto _LL1EE;if(*((
int*)_tmp2D3)!= 20)goto _LL1EE;_tmp2E6=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp2D3)->f1;_LL1ED: _tmp2E7=_tmp2E6;goto _LL1EF;_LL1EE: if(_tmp2D3 <= (void*)4)goto
_LL1F0;if(*((int*)_tmp2D3)!= 22)goto _LL1F0;_tmp2E7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp2D3)->f1;_LL1EF: return Cyc_CfFlowInfo_contains_region(rgn,_tmp2E7);_LL1F0: if(
_tmp2D3 <= (void*)4)goto _LL1F2;if(*((int*)_tmp2D3)!= 16)goto _LL1F2;_tmp2E8=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp2D3)->f1;_tmp2E9=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp2D3)->f2;_LL1F1: return Cyc_CfFlowInfo_contains_region(rgn,_tmp2E8) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp2E9);_LL1F2: if(_tmp2D3 <= (void*)4)goto _LL1BD;if(*((int*)_tmp2D3)!= 15)
goto _LL1BD;_tmp2EA=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2D3)->f1;
_LL1F3: return 0;_LL1BD:;}struct _tuple13{struct Cyc_Dict_Dict f1;void*f2;};static
void Cyc_CfFlowInfo_kill_root(struct _tuple13*env,void*root,void*rval){struct
_tuple13 _tmp2EF;struct Cyc_Dict_Dict _tmp2F0;struct Cyc_Dict_Dict*_tmp2F1;void*
_tmp2F2;struct _tuple13*_tmp2EE=env;_tmp2EF=*_tmp2EE;_tmp2F0=_tmp2EF.f1;_tmp2F1=(
struct Cyc_Dict_Dict*)&(*_tmp2EE).f1;_tmp2F2=_tmp2EF.f2;{void*_tmp2F3=root;struct
Cyc_Absyn_Vardecl*_tmp2F4;void*_tmp2F5;_LL1FF: if(*((int*)_tmp2F3)!= 0)goto _LL201;
_tmp2F4=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp2F3)->f1;_LL200: if(Cyc_CfFlowInfo_contains_region(
_tmp2F2,(void*)_tmp2F4->type))rval=Cyc_CfFlowInfo_typ_to_absrval((void*)_tmp2F4->type,
Cyc_CfFlowInfo_unknown_none);*_tmp2F1=Cyc_Dict_insert(*_tmp2F1,root,rval);goto
_LL1FE;_LL201: if(*((int*)_tmp2F3)!= 1)goto _LL203;_tmp2F5=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp2F3)->f2;_LL202: if(!Cyc_CfFlowInfo_contains_region(_tmp2F2,_tmp2F5))*_tmp2F1=
Cyc_Dict_insert(*_tmp2F1,root,rval);goto _LL1FE;_LL203: if(*((int*)_tmp2F3)!= 2)
goto _LL1FE;_LL204: goto _LL1FE;_LL1FE:;}}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_Dict_Dict fd,void*rgn){void*_tmp2F6=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*
_tmp2F7;_LL206: if(_tmp2F6 <= (void*)4)goto _LL208;if(*((int*)_tmp2F6)!= 1)goto
_LL208;_tmp2F7=((struct Cyc_Absyn_VarType_struct*)_tmp2F6)->f1;_LL207: {struct Cyc_CfFlowInfo_Region_k_struct
v=({struct Cyc_CfFlowInfo_Region_k_struct _tmp2F9;_tmp2F9.tag=0;_tmp2F9.f1=_tmp2F7;
_tmp2F9;});void*v2=(void*)& v;struct _tuple13 env=({struct _tuple13 _tmp2F8;_tmp2F8.f1=
Cyc_CfFlowInfo_empty_flowdict();_tmp2F8.f2=v2;_tmp2F8;});((void(*)(void(*f)(
struct _tuple13*,void*,void*),struct _tuple13*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}_LL208: if((int)_tmp2F6 != 3)goto
_LL20A;_LL209: {struct _tuple13 env=({struct _tuple13 _tmp2FA;_tmp2FA.f1=Cyc_CfFlowInfo_empty_flowdict();
_tmp2FA.f2=(void*)0;_tmp2FA;});((void(*)(void(*f)(struct _tuple13*,void*,void*),
struct _tuple13*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f1;}_LL20A:;_LL20B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB[0]=({struct Cyc_Core_Impossible_struct
_tmp2FC;_tmp2FC.tag=Cyc_Core_Impossible;_tmp2FC.f1=({const char*_tmp2FD="kill_flowdict_region";
_tag_dynforward(_tmp2FD,sizeof(char),_get_zero_arr_size(_tmp2FD,21));});_tmp2FC;});
_tmp2FB;}));_LL205:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_kill_flow_region(union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo_union _tmp2FE=f;struct Cyc_Dict_Dict _tmp2FF;struct Cyc_List_List*
_tmp300;struct Cyc_CfFlowInfo_ConsumeInfo _tmp301;_LL20D: if((_tmp2FE.BottomFL).tag
!= 0)goto _LL20F;_LL20E: return f;_LL20F: if((_tmp2FE.ReachableFL).tag != 1)goto
_LL20C;_tmp2FF=(_tmp2FE.ReachableFL).f1;_tmp300=(_tmp2FE.ReachableFL).f2;_tmp301=(
_tmp2FE.ReachableFL).f3;_LL210: {struct Cyc_Dict_Dict _tmp302=Cyc_CfFlowInfo_kill_flowdict_region(
_tmp2FF,rgn);struct Cyc_List_List*_tmp303=Cyc_CfFlowInfo_kill_relns_region(
_tmp300,rgn);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp304;(_tmp304.ReachableFL).tag=1;(_tmp304.ReachableFL).f1=_tmp302;(_tmp304.ReachableFL).f2=
_tmp303;(_tmp304.ReachableFL).f3=_tmp301;_tmp304;});}_LL20C:;}struct _tuple14{
struct Cyc_Dict_Dict*f1;struct Cyc_Position_Segment*f2;};static void Cyc_CfFlowInfo_consume_f(
struct _tuple14*env,struct Cyc_CfFlowInfo_Place*place){struct _tuple14 _tmp306;
struct Cyc_Dict_Dict*_tmp307;struct Cyc_Position_Segment*_tmp308;struct _tuple14*
_tmp305=env;_tmp306=*_tmp305;_tmp307=_tmp306.f1;_tmp308=_tmp306.f2;if(Cyc_CfFlowInfo_update_place_set((
struct Cyc_Dict_Dict*)_tmp307,place,_tmp308)){struct Cyc_Position_Segment*_tmp309=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_lookup)(*_tmp307,place);Cyc_CfFlowInfo_unique_err(place,({const char*
_tmp30A="May consume unique pointer %s more than once (cf. %s)";_tag_dynforward(
_tmp30A,sizeof(char),_get_zero_arr_size(_tmp30A,54));}),({const char*_tmp30B="May consume unique pointer %s more than once";
_tag_dynforward(_tmp30B,sizeof(char),_get_zero_arr_size(_tmp30B,45));}),_tmp309,
_tmp308);}}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp30C=f;struct Cyc_Dict_Dict _tmp30D;struct Cyc_List_List*_tmp30E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp30F;_LL212: if((_tmp30C.BottomFL).tag != 0)goto _LL214;_LL213: return f;_LL214:
if((_tmp30C.ReachableFL).tag != 1)goto _LL211;_tmp30D=(_tmp30C.ReachableFL).f1;
_tmp30E=(_tmp30C.ReachableFL).f2;_tmp30F=(_tmp30C.ReachableFL).f3;_LL215: if(
_tmp30F.may_consume == 0)return f;{struct Cyc_Dict_Dict _tmp310=_tmp30F.consumed;((
void(*)(void(*f)(struct _tuple14*,struct Cyc_CfFlowInfo_Place*),struct _tuple14*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,({struct
_tuple14*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311->f1=& _tmp310;_tmp311->f2=
loc;_tmp311;}),_tmp30F.may_consume);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp312;(_tmp312.ReachableFL).tag=1;(_tmp312.ReachableFL).f1=
_tmp30D;(_tmp312.ReachableFL).f2=_tmp30E;(_tmp312.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp313;_tmp313.consumed=_tmp310;_tmp313.may_consume=0;_tmp313;});_tmp312;});}
_LL211:;}void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,
union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union _tmp314=f;
struct Cyc_Dict_Dict _tmp315;struct Cyc_List_List*_tmp316;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp317;_LL217: if((_tmp314.BottomFL).tag != 0)goto _LL219;_LL218: return;_LL219: if((
_tmp314.ReachableFL).tag != 1)goto _LL216;_tmp315=(_tmp314.ReachableFL).f1;_tmp316=(
_tmp314.ReachableFL).f2;_tmp317=(_tmp314.ReachableFL).f3;_LL21A: {struct Cyc_List_List*
_tmp318=_tmp317.may_consume;while(_tmp318 != 0){if(((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp317.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp318->hd)){struct Cyc_Position_Segment*_tmp319=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp317.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp318->hd);Cyc_CfFlowInfo_unique_err((struct Cyc_CfFlowInfo_Place*)
_tmp318->hd,({const char*_tmp31A="Read through possibly consumed unique pointer %s (cf. %s)";
_tag_dynforward(_tmp31A,sizeof(char),_get_zero_arr_size(_tmp31A,58));}),({const
char*_tmp31B="Read through possibly consumed unique pointer %s";_tag_dynforward(
_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,49));}),_tmp319,loc);break;}
_tmp318=_tmp318->tl;}return;}_LL216:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp31C=f;struct Cyc_Dict_Dict _tmp31D;struct Cyc_List_List*_tmp31E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp31F;_LL21C: if((_tmp31C.BottomFL).tag != 0)goto _LL21E;_LL21D: return f;_LL21E:
if((_tmp31C.ReachableFL).tag != 1)goto _LL21B;_tmp31D=(_tmp31C.ReachableFL).f1;
_tmp31E=(_tmp31C.ReachableFL).f2;_tmp31F=(_tmp31C.ReachableFL).f3;_LL21F: _tmp31F.may_consume=
0;return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp320;(_tmp320.ReachableFL).tag=1;(_tmp320.ReachableFL).f1=_tmp31D;(_tmp320.ReachableFL).f2=
_tmp31E;(_tmp320.ReachableFL).f3=_tmp31F;_tmp320;});_LL21B:;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp321=f;struct Cyc_Dict_Dict _tmp322;struct Cyc_List_List*_tmp323;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp324;_LL221: if((_tmp321.BottomFL).tag != 0)goto _LL223;_LL222: return f;_LL223:
if((_tmp321.ReachableFL).tag != 1)goto _LL220;_tmp322=(_tmp321.ReachableFL).f1;
_tmp323=(_tmp321.ReachableFL).f2;_tmp324=(_tmp321.ReachableFL).f3;_LL224: _tmp324.may_consume=
0;return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp325;(_tmp325.ReachableFL).tag=1;(_tmp325.ReachableFL).f1=_tmp322;(_tmp325.ReachableFL).f2=
_tmp323;(_tmp325.ReachableFL).f3=_tmp324;_tmp325;});_LL220:;}}struct _tuple5 Cyc_CfFlowInfo_save_consume_info(
union Cyc_CfFlowInfo_FlowInfo_union f,int clear){static struct Cyc_Dict_Dict*
empty_info=0;union Cyc_CfFlowInfo_FlowInfo_union _tmp326=f;struct Cyc_Dict_Dict
_tmp327;struct Cyc_List_List*_tmp328;struct Cyc_CfFlowInfo_ConsumeInfo _tmp329;
_LL226: if((_tmp326.BottomFL).tag != 0)goto _LL228;_LL227: if(empty_info == 0)
empty_info=({struct Cyc_Dict_Dict*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A[0]=
Cyc_CfFlowInfo_mt_place_set();_tmp32A;});return({struct _tuple5 _tmp32B;_tmp32B.f1=({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp32C;_tmp32C.consumed=*empty_info;_tmp32C.may_consume=
0;_tmp32C;});_tmp32B.f2=f;_tmp32B;});_LL228: if((_tmp326.ReachableFL).tag != 1)
goto _LL225;_tmp327=(_tmp326.ReachableFL).f1;_tmp328=(_tmp326.ReachableFL).f2;
_tmp329=(_tmp326.ReachableFL).f3;_LL229: if(clear)return({struct _tuple5 _tmp32D;
_tmp32D.f1=_tmp329;_tmp32D.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp32E;(_tmp32E.ReachableFL).tag=1;(_tmp32E.ReachableFL).f1=_tmp327;(_tmp32E.ReachableFL).f2=
_tmp328;(_tmp32E.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp32F;
_tmp32F.consumed=Cyc_CfFlowInfo_mt_place_set();_tmp32F.may_consume=0;_tmp32F;});
_tmp32E;});_tmp32D;});else{return({struct _tuple5 _tmp330;_tmp330.f1=_tmp329;
_tmp330.f2=f;_tmp330;});}_LL225:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_CfFlowInfo_ConsumeInfo c){union Cyc_CfFlowInfo_FlowInfo_union
_tmp331=f;struct Cyc_Dict_Dict _tmp332;struct Cyc_List_List*_tmp333;_LL22B: if((
_tmp331.BottomFL).tag != 0)goto _LL22D;_LL22C: return f;_LL22D: if((_tmp331.ReachableFL).tag
!= 1)goto _LL22A;_tmp332=(_tmp331.ReachableFL).f1;_tmp333=(_tmp331.ReachableFL).f2;
_LL22E: return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp334;(_tmp334.ReachableFL).tag=1;(_tmp334.ReachableFL).f1=_tmp332;(_tmp334.ReachableFL).f2=
_tmp333;(_tmp334.ReachableFL).f3=c;_tmp334;});_LL22A:;}
