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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct _dynforward_ptr Cstring_to_string(char*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
struct _dynforward_ptr Cyc_List_to_array(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[8];
struct Cyc_Arg_Bad_struct{char*tag;struct _dynforward_ptr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr,struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dynforward_ptr),struct _dynforward_ptr errmsg,struct
_dynforward_ptr args);typedef struct{int __count;union{unsigned int __wch;char __wchb[
4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);unsigned int Cyc_fread(struct
_dynforward_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
unsigned int Cyc_fwrite(struct _dynforward_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);int remove(const char*);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dynforward_ptr,struct _dynforward_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;
void*abort();void exit(int);char*getenv(const char*);int system(const char*);
unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);int Cyc_strncmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned int len);struct _dynforward_ptr
Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr
Cyc_strconcat_l(struct Cyc_List_List*);struct _dynforward_ptr Cyc_str_sepstr(struct
Cyc_List_List*,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strdup(struct
_dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr,int
ofs,unsigned int n);struct _dynforward_ptr Cyc_strchr(struct _dynforward_ptr s,char c);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct _dynforward_ptr Cyc_Filename_concat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_chop_extension(
struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_dirname(struct
_dynforward_ptr);struct _dynforward_ptr Cyc_Filename_basename(struct
_dynforward_ptr);int Cyc_Filename_check_suffix(struct _dynforward_ptr,struct
_dynforward_ptr);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dynforward_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();int isspace(int);
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dynforward_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple4 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple5{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple6*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple7*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY16_struct{int tag;struct _tuple6*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*f1;};struct _tuple8{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{int tag;
struct _tuple8*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};struct
Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};struct Cyc_YY38_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY39_struct{int tag;
struct _tuple10*f1;};struct Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY42_struct{int tag;
void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{int tag;
void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY46_struct{
int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY49_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY52_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
extern int Cyc_Tovc_elim_array_initializers;struct Cyc_List_List*Cyc_Tovc_tovc(
struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
f1;};void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple11{struct _dynforward_ptr f1;struct _dynforward_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);void Cyc_Port_port(
struct Cyc_List_List*);void Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;
int Cyc_Toc_warn_all_null_deref;static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int
Cyc_noshake_r=0;static int Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;static int Cyc_cf_r=0;static int Cyc_noprint_r=0;static int Cyc_ic_r=
0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=0;static int Cyc_stop_after_asmfile_r=
0;static int Cyc_elim_se_r=0;static int Cyc_tovc_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=
0;static int Cyc_save_c_r=0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=
0;static int Cyc_nocheck_r=0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=
0;static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int
Cyc_print_all_kinds_r=0;static int Cyc_print_all_effects_r=0;void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=
1;}static struct _dynforward_ptr*Cyc_output_file=0;static void Cyc_set_output_file(
struct _dynforward_ptr s){Cyc_output_file=({struct _dynforward_ptr*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0[0]=s;_tmp0;});}extern char*Cdef_inc_path;extern char*
Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;static char
_tmp1[1]="";static struct _dynforward_ptr Cyc_cpp={_tmp1,_tmp1 + 1};static void Cyc_set_cpp(
struct _dynforward_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dynforward_ptr s){Cyc_cppargs=({struct Cyc_List_List*
_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=({struct _dynforward_ptr*_tmp3=
_cycalloc(sizeof(*_tmp3));_tmp3[0]=s;_tmp3;});_tmp2->tl=Cyc_cppargs;_tmp2;});}
static struct _dynforward_ptr Cyc_target_arch=(struct _dynforward_ptr){(void*)0,(
void*)(0 + 0)};static void Cyc_set_target_arch(struct _dynforward_ptr s){Cyc_toc_r=1;
Cyc_target_arch=s;}static void Cyc_print_version(){({struct Cyc_String_pa_struct
_tmp6;_tmp6.tag=0;_tmp6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Cversion));{void*_tmp4[1]={& _tmp6};Cyc_printf(({const char*
_tmp5="The Cyclone compiler, version %s\n";_tag_dynforward(_tmp5,sizeof(char),
_get_zero_arr_size(_tmp5,34));}),_tag_dynforward(_tmp4,sizeof(void*),1));}});({
struct Cyc_String_pa_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cstring_to_string(Ccomp));{struct Cyc_String_pa_struct _tmp9;
_tmp9.tag=0;_tmp9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Carch));{void*_tmp7[2]={& _tmp9,& _tmpA};Cyc_printf(({const char*
_tmp8="Compiled for architecture: %s CC=\"%s\"\n";_tag_dynforward(_tmp8,sizeof(
char),_get_zero_arr_size(_tmp8,39));}),_tag_dynforward(_tmp7,sizeof(void*),2));}}});({
struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cstring_to_string(Cdef_lib_path));{void*_tmpB[1]={& _tmpD};
Cyc_printf(({const char*_tmpC="Standard library directory: %s\n";_tag_dynforward(
_tmpC,sizeof(char),_get_zero_arr_size(_tmpC,32));}),_tag_dynforward(_tmpB,
sizeof(void*),1));}});({struct Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path));{
void*_tmpE[1]={& _tmp10};Cyc_printf(({const char*_tmpF="Standard include directory: %s\n";
_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,32));}),
_tag_dynforward(_tmpE,sizeof(void*),1));}});exit(0);}static int Cyc_is_cyclone_sourcefile(
struct _dynforward_ptr s){unsigned int _tmp11=Cyc_strlen((struct _dynforward_ptr)s);
if(_tmp11 <= 4)return 0;else{return Cyc_strcmp((struct _dynforward_ptr)
_dynforward_ptr_plus(s,sizeof(char),(int)(_tmp11 - 4)),({const char*_tmp12=".cyc";
_tag_dynforward(_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,5));}))== 0;}}
static struct Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct
_dynforward_ptr s){Cyc_ccargs=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*
_tmp13));_tmp13->hd=({struct _dynforward_ptr*_tmp14=_cycalloc(sizeof(*_tmp14));
_tmp14[0]=s;_tmp14;});_tmp13->tl=Cyc_ccargs;_tmp13;});}static struct Cyc_List_List*
Cyc_cyclone_exec_path=0;static void Cyc_add_cyclone_exec_path(struct
_dynforward_ptr s){unsigned int _tmp15=Cyc_strlen((struct _dynforward_ptr)s);if(
_tmp15 <= 2)return;{struct _dynforward_ptr _tmp16=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)s,2,_tmp15 - 2);Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=({struct _dynforward_ptr*_tmp18=
_cycalloc(sizeof(*_tmp18));_tmp18[0]=_tmp16;_tmp18;});_tmp17->tl=Cyc_cyclone_exec_path;
_tmp17;});}}static struct Cyc_List_List*Cyc_libargs=0;static void Cyc_add_libarg(
struct _dynforward_ptr s){if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp19="-lxml";_tag_dynforward(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,6));}))
== 0)Cyc_add_ccarg(s);else{Cyc_libargs=({struct Cyc_List_List*_tmp1A=_cycalloc(
sizeof(*_tmp1A));_tmp1A->hd=({struct _dynforward_ptr*_tmp1B=_cycalloc(sizeof(*
_tmp1B));_tmp1B[0]=s;_tmp1B;});_tmp1A->tl=Cyc_libargs;_tmp1A;});}}static void Cyc_add_marg(
struct _dynforward_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps(){Cyc_save_temps_r=
1;Cyc_add_ccarg(({const char*_tmp1C="-save-temps";_tag_dynforward(_tmp1C,sizeof(
char),_get_zero_arr_size(_tmp1C,12));}));}static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=
1;Cyc_add_cpparg(({const char*_tmp1D="-M";_tag_dynforward(_tmp1D,sizeof(char),
_get_zero_arr_size(_tmp1D,3));}));}static struct _dynforward_ptr*Cyc_dependencies_target=
0;static void Cyc_set_dependencies_target(struct _dynforward_ptr s){Cyc_dependencies_target=({
struct _dynforward_ptr*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=s;_tmp1E;});}
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(({
const char*_tmp1F="-c";_tag_dynforward(_tmp1F,sizeof(char),_get_zero_arr_size(
_tmp1F,3));}));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(({const char*
_tmp20="-no-cpp-precomp";_tag_dynforward(_tmp20,sizeof(char),_get_zero_arr_size(
_tmp20,16));}));Cyc_add_ccarg(({const char*_tmp21="-no-cpp-precomp";
_tag_dynforward(_tmp21,sizeof(char),_get_zero_arr_size(_tmp21,16));}));}static
void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions(){Cyc_add_ccarg(({const char*_tmp22="-D_INLINE_FUNCTIONS";
_tag_dynforward(_tmp22,sizeof(char),_get_zero_arr_size(_tmp22,20));}));}static
void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}static void Cyc_set_tovc(){
Cyc_tovc_r=1;Cyc_set_elim_se();}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(({
const char*_tmp23="-DNO_CYC_BOUNDS_CHECKS";_tag_dynforward(_tmp23,sizeof(char),
_get_zero_arr_size(_tmp23,23));}));}static void Cyc_set_nonullchecks(){Cyc_add_ccarg(({
const char*_tmp24="-DNO_CYC_NULL_CHECKS";_tag_dynforward(_tmp24,sizeof(char),
_get_zero_arr_size(_tmp24,21));}));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=
0;Cyc_add_ccarg(({const char*_tmp25="-DNO_CYC_PREFIX";_tag_dynforward(_tmp25,
sizeof(char),_get_zero_arr_size(_tmp25,16));}));}static void Cyc_set_pa(){Cyc_pa_r=
1;Cyc_add_ccarg(({const char*_tmp26="-DCYC_REGION_PROFILE";_tag_dynforward(_tmp26,
sizeof(char),_get_zero_arr_size(_tmp26,21));}));}static void Cyc_set_pg(){Cyc_pg_r=
1;Cyc_add_ccarg(({const char*_tmp27="-pg";_tag_dynforward(_tmp27,sizeof(char),
_get_zero_arr_size(_tmp27,4));}));}static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=
1;Cyc_add_ccarg(({const char*_tmp28="-S";_tag_dynforward(_tmp28,sizeof(char),
_get_zero_arr_size(_tmp28,3));}));}static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}static void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(
struct _dynforward_ptr s){if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp29="cyc";_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,4));}))
== 0)Cyc_intype=(void*)1;else{if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp2A="none";_tag_dynforward(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,5));}))
== 0)Cyc_intype=(void*)0;else{({struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;
_tmp2D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp2B[1]={&
_tmp2D};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C="Input type '%s' not supported\n";
_tag_dynforward(_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,31));}),
_tag_dynforward(_tmp2B,sizeof(void*),1));}});}}}struct _dynforward_ptr Cyc_make_base_filename(
struct _dynforward_ptr s,struct _dynforward_ptr*output_file){struct _dynforward_ptr
_tmp2E=Cyc_Filename_dirname(output_file == 0?s:*output_file);struct
_dynforward_ptr _tmp2F=Cyc_Filename_chop_extension((struct _dynforward_ptr)Cyc_Filename_basename(
s));struct _dynforward_ptr _tmp30=Cyc_strlen((struct _dynforward_ptr)_tmp2E)> 0?Cyc_Filename_concat((
struct _dynforward_ptr)_tmp2E,(struct _dynforward_ptr)_tmp2F): _tmp2F;return(struct
_dynforward_ptr)_tmp30;}static struct Cyc_List_List*Cyc_cyclone_files=0;static void
Cyc_add_other(struct _dynforward_ptr s){if(Cyc_intype == (void*)1  || Cyc_is_cyclone_sourcefile(
s)){Cyc_cyclone_files=({struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));
_tmp31->hd=({struct _dynforward_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=s;
_tmp32;});_tmp31->tl=Cyc_cyclone_files;_tmp31;});{struct _dynforward_ptr _tmp33=
Cyc_make_base_filename(s,Cyc_output_file);struct _dynforward_ptr _tmp34=Cyc_strconcat((
struct _dynforward_ptr)_tmp33,({const char*_tmp35=".c";_tag_dynforward(_tmp35,
sizeof(char),_get_zero_arr_size(_tmp35,3));}));Cyc_add_ccarg((struct
_dynforward_ptr)_tmp34);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _dynforward_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dynforward_ptr(s,sizeof(char),1));}}struct _tuple12{struct _dynforward_ptr*
f1;struct _dynforward_ptr*f2;};struct Cyc_List_List*Cyc_read_specs(struct
_dynforward_ptr file){struct Cyc_List_List*_tmp36=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dynforward_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){({
struct Cyc_String_pa_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmp37[1]={& _tmp39};Cyc_fprintf(Cyc_stderr,({
const char*_tmp38="Error opening spec file %s\n";_tag_dynforward(_tmp38,sizeof(
char),_get_zero_arr_size(_tmp38,28));}),_tag_dynforward(_tmp37,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){({struct Cyc_Int_pa_struct _tmp3D;_tmp3D.tag=1;_tmp3D.f1=(unsigned long)c;{
struct Cyc_String_pa_struct _tmp3C;_tmp3C.tag=0;_tmp3C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmp3A[2]={& _tmp3C,& _tmp3D};Cyc_fprintf(Cyc_stderr,({
const char*_tmp3B="Error reading spec file %s: unexpected character '%c'\n";
_tag_dynforward(_tmp3B,sizeof(char),_get_zero_arr_size(_tmp3B,55));}),
_tag_dynforward(_tmp3A,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);}goto CLEANUP_AND_RETURN;}JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(c == - 1){({struct Cyc_String_pa_struct _tmp40;
_tmp40.tag=0;_tmp40.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file);{
void*_tmp3E[1]={& _tmp40};Cyc_fprintf(Cyc_stderr,({const char*_tmp3F="Error reading spec file %s: unexpected EOF\n";
_tag_dynforward(_tmp3F,sizeof(char),_get_zero_arr_size(_tmp3F,44));}),
_tag_dynforward(_tmp3E,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){strname[
_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;i ++;if(i >= 256){({struct Cyc_String_pa_struct
_tmp43;_tmp43.tag=0;_tmp43.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
file);{void*_tmp41[1]={& _tmp43};Cyc_fprintf(Cyc_stderr,({const char*_tmp42="Error reading spec file %s: string name too long\n";
_tag_dynforward(_tmp42,sizeof(char),_get_zero_arr_size(_tmp42,50));}),
_tag_dynforward(_tmp41,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)
spec_file);if(isspace(c))continue;break;}if(c == '*'){_tmp36=({struct Cyc_List_List*
_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->hd=({struct _tuple12*_tmp45=_cycalloc(
sizeof(*_tmp45));_tmp45->f1=({struct _dynforward_ptr*_tmp48=_cycalloc(sizeof(*
_tmp48));_tmp48[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strname,
sizeof(char),256));_tmp48;});_tmp45->f2=({struct _dynforward_ptr*_tmp46=_cycalloc(
sizeof(*_tmp46));_tmp46[0]=(struct _dynforward_ptr)Cyc_strdup(({const char*_tmp47="";
_tag_dynforward(_tmp47,sizeof(char),_get_zero_arr_size(_tmp47,1));}));_tmp46;});
_tmp45;});_tmp44->tl=_tmp36;_tmp44;});goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=
1;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;i ++;if(i >= 4096){({
struct Cyc_String_pa_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _dynforward_ptr)
_tag_dynforward(strname,sizeof(char),256);{struct Cyc_String_pa_struct _tmp4B;
_tmp4B.tag=0;_tmp4B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file);{
void*_tmp49[2]={& _tmp4B,& _tmp4C};Cyc_fprintf(Cyc_stderr,({const char*_tmp4A="Error reading spec file %s: value of %s too long\n";
_tag_dynforward(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,50));}),
_tag_dynforward(_tmp49,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}}_tmp36=({struct Cyc_List_List*_tmp4D=
_cycalloc(sizeof(*_tmp4D));_tmp4D->hd=({struct _tuple12*_tmp4E=_cycalloc(sizeof(*
_tmp4E));_tmp4E->f1=({struct _dynforward_ptr*_tmp50=_cycalloc(sizeof(*_tmp50));
_tmp50[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strname,sizeof(char),
256));_tmp50;});_tmp4E->f2=({struct _dynforward_ptr*_tmp4F=_cycalloc(sizeof(*
_tmp4F));_tmp4F[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strvalue,
sizeof(char),4096));_tmp4F;});_tmp4E;});_tmp4D->tl=_tmp36;_tmp4D;});if(c == - 1)
goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((struct Cyc___cycFILE*)
spec_file);return _tmp36;}struct _dynforward_ptr Cyc_split_specs(struct
_dynforward_ptr cmdline){if(cmdline.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return _tag_dynforward(0,0,0);{unsigned int _tmp51=
Cyc_strlen((struct _dynforward_ptr)cmdline);struct Cyc_List_List*_tmp52=0;char buf[
4096];int i=0;int j=0;if(_tmp51 > 4096)goto DONE;while(1){while(1){if(i >= _tmp51)
goto DONE;if(*((const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))== 
0)goto DONE;if(!isspace((int)*((const char*)_check_dynforward_subscript(cmdline,
sizeof(char),i))))break;i ++;}j=0;while(1){if(i >= _tmp51)break;if(*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))== '\\'){i ++;if(i
>= _tmp51)break;if(*((const char*)_check_dynforward_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i));j ++;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i));j ++;}i ++;}if(j < 4096)buf[
_check_known_subscript_notnull(4096,j)]='\000';_tmp52=({struct Cyc_List_List*
_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=({struct _dynforward_ptr*_tmp54=
_cycalloc(sizeof(*_tmp54));_tmp54[0]=(struct _dynforward_ptr)Cyc_strdup(
_tag_dynforward(buf,sizeof(char),4096));_tmp54;});_tmp53->tl=_tmp52;_tmp53;});}
DONE: _tmp52=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp52);_tmp52=({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp55->tl=_tmp52;_tmp55;});{struct _dynforward_ptr _tmp57=((struct _dynforward_ptr(*)(
struct Cyc_List_List*x))Cyc_List_to_array)(_tmp52);struct _dynforward_ptr _tmp58=({
unsigned int _tmp59=_get_dynforward_size(_tmp57,sizeof(struct _dynforward_ptr*));
struct _dynforward_ptr*_tmp5A=(struct _dynforward_ptr*)_cycalloc(_check_times(
sizeof(struct _dynforward_ptr),_tmp59));struct _dynforward_ptr _tmp5C=
_tag_dynforward(_tmp5A,sizeof(struct _dynforward_ptr),_tmp59);{unsigned int _tmp5B=
_tmp59;unsigned int k;for(k=0;k < _tmp5B;k ++){_tmp5A[k]=*(*((struct _dynforward_ptr**)
_check_dynforward_subscript(_tmp57,sizeof(struct _dynforward_ptr*),(int)k)));}}
_tmp5C;});return _tmp58;}}}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dynforward_ptr filename,struct _dynforward_ptr mode,struct _dynforward_ptr
msg_part){struct _handler_cons _tmp5D;_push_handler(& _tmp5D);{int _tmp5F=0;if(
setjmp(_tmp5D.handler))_tmp5F=1;if(!_tmp5F){{struct Cyc___cycFILE*_tmp60=(struct
Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);return _tmp60;};
_pop_handler();}else{void*_tmp5E=(void*)_exn_thrown;void*_tmp62=_tmp5E;_LL1:;
_LL2: Cyc_compile_failure=1;({struct Cyc_String_pa_struct _tmp66;_tmp66.tag=0;
_tmp66.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)filename);{struct Cyc_String_pa_struct
_tmp65;_tmp65.tag=0;_tmp65.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg_part);{void*_tmp63[2]={& _tmp65,& _tmp66};Cyc_fprintf(Cyc_stderr,({const char*
_tmp64="\nError: couldn't open %s %s\n";_tag_dynforward(_tmp64,sizeof(char),
_get_zero_arr_size(_tmp64,29));}),_tag_dynforward(_tmp63,sizeof(void*),2));}}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(
_tmp62);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct
Cyc_List_List*tds){struct _dynforward_ptr exn_string=({const char*_tmp84="";
_tag_dynforward(_tmp84,sizeof(char),_get_zero_arr_size(_tmp84,1));});struct
_dynforward_ptr explain_string=({const char*_tmp83="";_tag_dynforward(_tmp83,
sizeof(char),_get_zero_arr_size(_tmp83,1));});int other_exn=0;void*ex=(void*)({
struct Cyc_Core_Impossible_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({
struct Cyc_Core_Impossible_struct _tmp81;_tmp81.tag=Cyc_Core_Impossible;_tmp81.f1=({
const char*_tmp82="";_tag_dynforward(_tmp82,sizeof(char),_get_zero_arr_size(
_tmp82,1));});_tmp81;});_tmp80;});struct Cyc_List_List*_tmp67=0;{struct
_handler_cons _tmp68;_push_handler(& _tmp68);{int _tmp6A=0;if(setjmp(_tmp68.handler))
_tmp6A=1;if(!_tmp6A){_tmp67=f(env,tds);;_pop_handler();}else{void*_tmp69=(void*)
_exn_thrown;void*_tmp6C=_tmp69;struct _dynforward_ptr _tmp6D;struct _dynforward_ptr
_tmp6E;struct _dynforward_ptr _tmp6F;_LL6: if(*((void**)_tmp6C)!= Cyc_Core_Impossible)
goto _LL8;_tmp6D=((struct Cyc_Core_Impossible_struct*)_tmp6C)->f1;_LL7: exn_string=({
const char*_tmp70="Exception Core::Impossible";_tag_dynforward(_tmp70,sizeof(char),
_get_zero_arr_size(_tmp70,27));});explain_string=_tmp6D;goto _LL5;_LL8: if(_tmp6C
!= Cyc_Dict_Absent)goto _LLA;_LL9: exn_string=({const char*_tmp71="Exception Dict::Absent";
_tag_dynforward(_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,23));});goto _LL5;
_LLA: if(*((void**)_tmp6C)!= Cyc_Core_Invalid_argument)goto _LLC;_tmp6E=((struct
Cyc_Core_Invalid_argument_struct*)_tmp6C)->f1;_LLB: exn_string=({const char*_tmp72="Exception Core::Invalid_argument";
_tag_dynforward(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,33));});
explain_string=_tmp6E;goto _LL5;_LLC: if(*((void**)_tmp6C)!= Cyc_Core_Failure)goto
_LLE;_tmp6F=((struct Cyc_Core_Failure_struct*)_tmp6C)->f1;_LLD: exn_string=({const
char*_tmp73="Exception Core::Failure";_tag_dynforward(_tmp73,sizeof(char),
_get_zero_arr_size(_tmp73,24));});explain_string=_tmp6F;goto _LL5;_LLE:;_LLF: ex=
_tmp6C;other_exn=1;exn_string=({const char*_tmp74="Uncaught exception";
_tag_dynforward(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,19));});goto _LL5;
_LL10:;_LL11:(void)_throw(_tmp6C);_LL5:;}}}if(Cyc_Position_error_p())Cyc_compile_failure=
1;if(Cyc_strcmp((struct _dynforward_ptr)exn_string,({const char*_tmp75="";
_tag_dynforward(_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,1));}))!= 0){Cyc_compile_failure=
1;({struct Cyc_String_pa_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)explain_string);{struct Cyc_String_pa_struct
_tmp79;_tmp79.tag=0;_tmp79.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
stage_name);{struct Cyc_String_pa_struct _tmp78;_tmp78.tag=0;_tmp78.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)exn_string);{void*_tmp76[3]={& _tmp78,&
_tmp79,& _tmp7A};Cyc_fprintf(Cyc_stderr,({const char*_tmp77="\n%s thrown during %s: %s";
_tag_dynforward(_tmp77,sizeof(char),_get_zero_arr_size(_tmp77,25));}),
_tag_dynforward(_tmp76,sizeof(void*),3));}}}});}if(Cyc_compile_failure){({void*
_tmp7B[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp7C="\nCOMPILATION FAILED!\n";
_tag_dynforward(_tmp7C,sizeof(char),_get_zero_arr_size(_tmp7C,22));}),
_tag_dynforward(_tmp7B,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
if(other_exn)(int)_throw(ex);return _tmp67;}else{if(Cyc_v_r){({struct Cyc_String_pa_struct
_tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
stage_name);{void*_tmp7D[1]={& _tmp7F};Cyc_fprintf(Cyc_stderr,({const char*_tmp7E="%s completed.\n";
_tag_dynforward(_tmp7E,sizeof(char),_get_zero_arr_size(_tmp7E,15));}),
_tag_dynforward(_tmp7D,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return _tmp67;}}return _tmp67;}struct Cyc_List_List*Cyc_do_parse(struct
Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*
_tmp85=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);return _tmp85;}}struct _tuple13{
struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(
struct _tuple13*tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp87;struct
Cyc_Tcenv_Tenv*_tmp88;struct _tuple13 _tmp86=*tcenv;_tmp87=_tmp86.f1;_tmp88=_tmp86.f2;
Cyc_Tc_tc(_tmp87,_tmp88,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp88,tds);
return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);return tds;}struct _tuple14{struct Cyc_Tcenv_Tenv*
f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(
struct _tuple14*params,struct Cyc_List_List*tds){struct _tuple14 _tmp8A;struct Cyc_Tcenv_Tenv*
_tmp8B;struct Cyc___cycFILE*_tmp8C;struct Cyc___cycFILE*_tmp8D;struct _tuple14*
_tmp89=params;_tmp8A=*_tmp89;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;_tmp8D=_tmp8A.f3;{
struct Cyc_Interface_I*_tmp8E=Cyc_Interface_extract(_tmp8B->ae);if(_tmp8C == 0)Cyc_Interface_save(
_tmp8E,_tmp8D);else{struct Cyc_Interface_I*_tmp8F=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmp8C);if(!Cyc_Interface_is_subinterface(_tmp8F,_tmp8E,({struct _tuple11*_tmp90=
_cycalloc(sizeof(*_tmp90));_tmp90->f1=({const char*_tmp92="written interface";
_tag_dynforward(_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,18));});_tmp90->f2=({
const char*_tmp91="maximal interface";_tag_dynforward(_tmp91,sizeof(char),
_get_zero_arr_size(_tmp91,18));});_tmp90;})))Cyc_compile_failure=1;else{Cyc_Interface_save(
_tmp8F,_tmp8D);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,
struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=Cyc_tovc_r;
return Cyc_Tovc_tovc(tds);}static char _tmp93[1]="";static struct _dynforward_ptr Cyc_cyc_include={
_tmp93,_tmp93 + 1};static void Cyc_set_cyc_include(struct _dynforward_ptr f){Cyc_cyc_include=
f;}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*
tds){struct Cyc_Absynpp_Params _tmp94=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmp94.expand_typedefs=!Cyc_noexpand_r;_tmp94.to_VC=Cyc_tovc_r;_tmp94.add_cyc_prefix=
Cyc_add_cyc_namespace_r;_tmp94.generate_line_directives=Cyc_generate_line_directives_r;
_tmp94.print_full_evars=Cyc_print_full_evars_r;_tmp94.print_all_tvars=Cyc_print_all_tvars_r;
_tmp94.print_all_kinds=Cyc_print_all_kinds_r;_tmp94.print_all_effects=Cyc_print_all_effects_r;{
struct Cyc___cycFILE*_tmp95=Cyc_try_file_open(Cyc_cyc_include,({const char*_tmp9B="r";
_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,2));}),({const char*
_tmp9C="internal compiler file";_tag_dynforward(_tmp9C,sizeof(char),
_get_zero_arr_size(_tmp9C,23));}));if(_tmp95 == 0)return tds;{unsigned int n_read=
1024;unsigned int n_written;char buf[1024];{unsigned int _tmp9A=1024;unsigned int i;
for(i=0;i < _tmp9A;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=Cyc_fread(
_tag_dynforward(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)_tmp95);if(
n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp95)){Cyc_compile_failure=1;({
void*_tmp96[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp97="\nError: problem copying cyc_include.h\n";
_tag_dynforward(_tmp97,sizeof(char),_get_zero_arr_size(_tmp97,39));}),
_tag_dynforward(_tmp96,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
return tds;}n_written=Cyc_fwrite(_tag_dynforward(buf,sizeof(char),1024),1,n_read,
out_file);if(n_read != n_written){Cyc_compile_failure=1;({void*_tmp98[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp99="\nError: problem copying cyc_include.h\n";
_tag_dynforward(_tmp99,sizeof(char),_get_zero_arr_size(_tmp99,39));}),
_tag_dynforward(_tmp98,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
return tds;}}Cyc_fclose((struct Cyc___cycFILE*)_tmp95);if(Cyc_pp_r){Cyc_Absynpp_set_params(&
_tmp94);Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(&
_tmp94);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}}}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dynforward_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dynforward_ptr s,char c){if(s.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmp9D=0;unsigned int
_tmp9E=Cyc_strlen((struct _dynforward_ptr)s);while(_tmp9E > 0){struct
_dynforward_ptr _tmp9F=Cyc_strchr(s,c);if(_tmp9F.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr){_tmp9D=({struct Cyc_List_List*_tmpA0=_cycalloc(
sizeof(*_tmpA0));_tmpA0->hd=({struct _dynforward_ptr*_tmpA1=_cycalloc(sizeof(*
_tmpA1));_tmpA1[0]=s;_tmpA1;});_tmpA0->tl=_tmp9D;_tmpA0;});break;}else{_tmp9D=({
struct Cyc_List_List*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->hd=({struct
_dynforward_ptr*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=(struct
_dynforward_ptr)Cyc_substring((struct _dynforward_ptr)s,0,(unsigned int)((((
struct _dynforward_ptr)_tmp9F).curr - s.curr)/ sizeof(char)));_tmpA3;});_tmpA2->tl=
_tmp9D;_tmpA2;});_tmp9E -=(((struct _dynforward_ptr)_tmp9F).curr - s.curr)/ sizeof(
char);s=_dynforward_ptr_plus(_tmp9F,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9D);}}static int Cyc_file_exists(
struct _dynforward_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmpA4;
_push_handler(& _tmpA4);{int _tmpA6=0;if(setjmp(_tmpA4.handler))_tmpA6=1;if(!
_tmpA6){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({const char*_tmpA7="r";
_tag_dynforward(_tmpA7,sizeof(char),_get_zero_arr_size(_tmpA7,2));}));;
_pop_handler();}else{void*_tmpA5=(void*)_exn_thrown;void*_tmpA9=_tmpA5;_LL13:;
_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmpA9);_LL12:;}}}if(f == 0)return 0;
else{Cyc_fclose((struct Cyc___cycFILE*)f);return 1;}}static struct _dynforward_ptr
Cyc_sprint_list(struct Cyc_List_List*dirs){struct _dynforward_ptr tmp=({const char*
_tmpAC="";_tag_dynforward(_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,1));});
for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmpAA=*((struct
_dynforward_ptr*)dirs->hd);if(_tmpAA.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmpAA)== 0)
continue;_tmpAA=(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)
_tmpAA,({const char*_tmpAB=":";_tag_dynforward(_tmpAB,sizeof(char),
_get_zero_arr_size(_tmpAB,2));}));tmp=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)_tmpAA,(struct _dynforward_ptr)tmp);}return tmp;}static
struct _dynforward_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dynforward_ptr
file){if(file.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)return
0;for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmpAD=*((struct
_dynforward_ptr*)dirs->hd);if(_tmpAD.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmpAD)== 0)
continue;{struct _dynforward_ptr s=(struct _dynforward_ptr)Cyc_Filename_concat(
_tmpAD,file);if(Cyc_file_exists(s))return({struct _dynforward_ptr*_tmpAE=
_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=s;_tmpAE;});}}return 0;}static struct
_dynforward_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dynforward_ptr file){
struct _dynforward_ptr*_tmpAF=Cyc_find(dirs,file);if(_tmpAF == 0){Cyc_compile_failure=
1;Cyc_remove_cfiles();({struct Cyc_String_pa_struct _tmpB3;_tmpB3.tag=0;_tmpB3.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{struct Cyc_String_pa_struct
_tmpB2;_tmpB2.tag=0;_tmpB2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
file);{void*_tmpB0[2]={& _tmpB2,& _tmpB3};Cyc_fprintf(Cyc_stderr,({const char*
_tmpB1="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmpB1,sizeof(char),_get_zero_arr_size(_tmpB1,56));}),
_tag_dynforward(_tmpB0,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpB4=_cycalloc(
sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Core_Failure_struct _tmpB5;_tmpB5.tag=Cyc_Core_Failure;
_tmpB5.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmpB9;_tmpB9.tag=0;
_tmpB9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{
struct Cyc_String_pa_struct _tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmpB6[2]={& _tmpB8,& _tmpB9};Cyc_aprintf(({
const char*_tmpB7="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,56));}),
_tag_dynforward(_tmpB6,sizeof(void*),2));}}});_tmpB5;});_tmpB4;}));}return*
_tmpAF;}static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct
_dynforward_ptr subdir){struct Cyc_List_List*_tmpBA=0;for(0;dirs != 0;dirs=dirs->tl){
_tmpBA=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->hd=({
struct _dynforward_ptr*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=(struct
_dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),subdir);
_tmpBC;});_tmpBB->tl=_tmpBA;_tmpBB;});}_tmpBA=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBA);return _tmpBA;}static struct Cyc_List_List*
Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dynforward_ptr subdir){struct Cyc_List_List*
_tmpBD=0;for(0;dirs != 0;dirs=dirs->tl){_tmpBD=({struct Cyc_List_List*_tmpBE=
_cycalloc(sizeof(*_tmpBE));_tmpBE->hd=(struct _dynforward_ptr*)dirs->hd;_tmpBE->tl=
_tmpBD;_tmpBE;});_tmpBD=({struct Cyc_List_List*_tmpBF=_cycalloc(sizeof(*_tmpBF));
_tmpBF->hd=({struct _dynforward_ptr*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=(
struct _dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),
subdir);_tmpC0;});_tmpBF->tl=_tmpBD;_tmpBF;});}_tmpBD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBD);return _tmpBD;}static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dynforward_ptr Cyc_sh_escape_string(
struct _dynforward_ptr s){unsigned int _tmpC1=Cyc_strlen((struct _dynforward_ptr)s);
int _tmpC2=0;int _tmpC3=0;{int i=0;for(0;i < _tmpC1;i ++){char _tmpC4=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(_tmpC4 == '\'')_tmpC2 ++;else{
if(Cyc_is_other_special(_tmpC4))_tmpC3 ++;}}}if(_tmpC2 == 0  && _tmpC3 == 0)return s;
if(_tmpC2 == 0)return(struct _dynforward_ptr)Cyc_strconcat_l(({struct
_dynforward_ptr*_tmpC5[3];_tmpC5[2]=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"'",sizeof(char),2);_tmpC5[1]=({struct _dynforward_ptr*_tmpC7=
_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=(struct _dynforward_ptr)s;_tmpC7;});_tmpC5[0]=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"'",sizeof(char),2);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmpC5,sizeof(struct _dynforward_ptr*),3));}));{unsigned int _tmpC9=(_tmpC1 + 
_tmpC2)+ _tmpC3;struct _dynforward_ptr s2=({unsigned int _tmpD3=_tmpC9 + 1;char*
_tmpD4=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpD3 + 1));struct
_dynforward_ptr _tmpD6=_tag_dynforward(_tmpD4,sizeof(char),_tmpD3 + 1);{
unsigned int _tmpD5=_tmpD3;unsigned int i;for(i=0;i < _tmpD5;i ++){_tmpD4[i]='\000';}
_tmpD4[_tmpD5]=(char)0;}_tmpD6;});int _tmpCA=0;int _tmpCB=0;for(0;_tmpCA < _tmpC1;
_tmpCA ++){char _tmpCC=*((const char*)_check_dynforward_subscript(s,sizeof(char),
_tmpCA));if(_tmpCC == '\''  || Cyc_is_other_special(_tmpCC))({struct
_dynforward_ptr _tmpCD=_dynforward_ptr_plus(s2,sizeof(char),_tmpCB ++);char _tmpCE=*((
char*)_check_dynforward_subscript(_tmpCD,sizeof(char),0));char _tmpCF='\\';if(
_get_dynforward_size(_tmpCD,sizeof(char))== 1  && (_tmpCE == '\000'  && _tmpCF != '\000'))
_throw_arraybounds();*((char*)_tmpCD.curr)=_tmpCF;});({struct _dynforward_ptr
_tmpD0=_dynforward_ptr_plus(s2,sizeof(char),_tmpCB ++);char _tmpD1=*((char*)
_check_dynforward_subscript(_tmpD0,sizeof(char),0));char _tmpD2=_tmpCC;if(
_get_dynforward_size(_tmpD0,sizeof(char))== 1  && (_tmpD1 == '\000'  && _tmpD2 != '\000'))
_throw_arraybounds();*((char*)_tmpD0.curr)=_tmpD2;});}return(struct
_dynforward_ptr)s2;}}static struct _dynforward_ptr*Cyc_sh_escape_stringptr(struct
_dynforward_ptr*sp){return({struct _dynforward_ptr*_tmpD7=_cycalloc(sizeof(*
_tmpD7));_tmpD7[0]=Cyc_sh_escape_string(*sp);_tmpD7;});}static void Cyc_process_file(
struct _dynforward_ptr filename){struct _dynforward_ptr _tmpD8=Cyc_make_base_filename(
filename,Cyc_output_file);struct _dynforward_ptr _tmpD9=Cyc_strconcat((struct
_dynforward_ptr)_tmpD8,({const char*_tmp133=".cyp";_tag_dynforward(_tmp133,
sizeof(char),_get_zero_arr_size(_tmp133,5));}));struct _dynforward_ptr _tmpDA=Cyc_strconcat((
struct _dynforward_ptr)_tmpD8,({const char*_tmp132=".cyci";_tag_dynforward(_tmp132,
sizeof(char),_get_zero_arr_size(_tmp132,6));}));struct _dynforward_ptr _tmpDB=Cyc_strconcat((
struct _dynforward_ptr)_tmpD8,({const char*_tmp131=".cycio";_tag_dynforward(
_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,7));}));struct _dynforward_ptr
_tmpDC=Cyc_strconcat((struct _dynforward_ptr)_tmpD8,({const char*_tmp130=".c";
_tag_dynforward(_tmp130,sizeof(char),_get_zero_arr_size(_tmp130,3));}));if(Cyc_v_r)({
struct Cyc_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)filename);{void*_tmpDD[1]={& _tmpDF};Cyc_fprintf(Cyc_stderr,({
const char*_tmpDE="Compiling %s\n";_tag_dynforward(_tmpDE,sizeof(char),
_get_zero_arr_size(_tmpDE,14));}),_tag_dynforward(_tmpDD,sizeof(void*),1));}});{
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmp12E="r";
_tag_dynforward(_tmp12E,sizeof(char),_get_zero_arr_size(_tmp12E,2));}),({const
char*_tmp12F="input file";_tag_dynforward(_tmp12F,sizeof(char),
_get_zero_arr_size(_tmp12F,11));}));if(Cyc_compile_failure  || !((unsigned int)f0))
return;Cyc_fclose((struct Cyc___cycFILE*)f0);{struct _dynforward_ptr _tmpE0=Cyc_str_sepstr(({
struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->hd=({struct
_dynforward_ptr*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=(struct
_dynforward_ptr)({const char*_tmp12C="";_tag_dynforward(_tmp12C,sizeof(char),
_get_zero_arr_size(_tmp12C,1));});_tmp12B;});_tmp12A->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmp12A;}),({const char*_tmp12D=" ";_tag_dynforward(_tmp12D,sizeof(
char),_get_zero_arr_size(_tmp12D,2));}));struct _dynforward_ptr def_inc_path=(
struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*
_tmpE1=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);_tmpE1=Cyc_add_subdir(
_tmpE1,({const char*_tmpE2="include";_tag_dynforward(_tmpE2,sizeof(char),
_get_zero_arr_size(_tmpE2,8));}));if(Cyc_strlen((struct _dynforward_ptr)
def_inc_path)> 0)_tmpE1=({struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(*_tmpE3));
_tmpE3->hd=({struct _dynforward_ptr*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=
def_inc_path;_tmpE4;});_tmpE3->tl=_tmpE1;_tmpE3;});{char*_tmpE5=getenv((const
char*)"CYCLONE_INCLUDE_PATH");if((char*)_tmpE5 != 0)_tmpE1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({
char*_tmpE6=_tmpE5;_tag_dynforward(_tmpE6,sizeof(char),_get_zero_arr_size(_tmpE6,
1));}),':'),_tmpE1);{struct _dynforward_ptr stdinc_string=(struct _dynforward_ptr)
Cyc_str_sepstr(({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->hd=({
struct _dynforward_ptr*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=(struct
_dynforward_ptr)({const char*_tmp128="";_tag_dynforward(_tmp128,sizeof(char),
_get_zero_arr_size(_tmp128,1));});_tmp127;});_tmp126->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpE1);_tmp126;}),({const char*_tmp129=" -I";
_tag_dynforward(_tmp129,sizeof(char),_get_zero_arr_size(_tmp129,4));}));struct
_dynforward_ptr ofile_string;if(Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)
ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmpE9;_tmpE9.tag=
0;_tmpE9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)));{void*_tmpE7[1]={& _tmpE9};Cyc_aprintf(({
const char*_tmpE8=" > %s";_tag_dynforward(_tmpE8,sizeof(char),_get_zero_arr_size(
_tmpE8,6));}),_tag_dynforward(_tmpE7,sizeof(void*),1));}});else{ofile_string=({
const char*_tmpEA="";_tag_dynforward(_tmpEA,sizeof(char),_get_zero_arr_size(
_tmpEA,1));});}}else{ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmpED;_tmpED.tag=0;_tmpED.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string((
struct _dynforward_ptr)_tmpD9));{void*_tmpEB[1]={& _tmpED};Cyc_aprintf(({const char*
_tmpEC=" > %s";_tag_dynforward(_tmpEC,sizeof(char),_get_zero_arr_size(_tmpEC,6));}),
_tag_dynforward(_tmpEB,sizeof(void*),1));}});}{struct _dynforward_ptr fixup_string;
if(Cyc_produce_dependencies){if(Cyc_dependencies_target == 0)fixup_string=({const
char*_tmpEE=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_dynforward(_tmpEE,
sizeof(char),_get_zero_arr_size(_tmpEE,35));});else{fixup_string=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmpF1;_tmpF1.tag=0;_tmpF1.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_null(Cyc_dependencies_target)));{void*_tmpEF[1]={& _tmpF1};Cyc_aprintf(({
const char*_tmpF0=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dynforward(_tmpF0,sizeof(
char),_get_zero_arr_size(_tmpF0,29));}),_tag_dynforward(_tmpEF,sizeof(void*),1));}});}}
else{fixup_string=({const char*_tmpF2="";_tag_dynforward(_tmpF2,sizeof(char),
_get_zero_arr_size(_tmpF2,1));});}{struct _dynforward_ptr _tmpF3=({struct Cyc_String_pa_struct
_tmp125;_tmp125.tag=0;_tmp125.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
ofile_string);{struct Cyc_String_pa_struct _tmp124;_tmp124.tag=0;_tmp124.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)fixup_string);{struct Cyc_String_pa_struct
_tmp123;_tmp123.tag=0;_tmp123.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_sh_escape_string(filename));{struct Cyc_String_pa_struct _tmp122;_tmp122.tag=0;
_tmp122.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)stdinc_string);{struct
Cyc_String_pa_struct _tmp121;_tmp121.tag=0;_tmp121.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmpE0);{struct Cyc_String_pa_struct _tmp120;_tmp120.tag=0;
_tmp120.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_cpp);{void*_tmp11E[
6]={& _tmp120,& _tmp121,& _tmp122,& _tmp123,& _tmp124,& _tmp125};Cyc_aprintf(({const
char*_tmp11F="%s %s%s %s%s%s";_tag_dynforward(_tmp11F,sizeof(char),
_get_zero_arr_size(_tmp11F,15));}),_tag_dynforward(_tmp11E,sizeof(void*),6));}}}}}}});
if(Cyc_v_r)({struct Cyc_String_pa_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpF3);{void*_tmpF4[1]={& _tmpF6};Cyc_fprintf(
Cyc_stderr,({const char*_tmpF5="%s\n";_tag_dynforward(_tmpF5,sizeof(char),
_get_zero_arr_size(_tmpF5,4));}),_tag_dynforward(_tmpF4,sizeof(void*),1));}});
if(system((const char*)_untag_dynforward_ptr(_tmpF3,sizeof(char),1))!= 0){Cyc_compile_failure=
1;({void*_tmpF7[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF8="\nError: preprocessing\n";
_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,23));}),
_tag_dynforward(_tmpF7,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)
return;Cyc_Position_reset_position((struct _dynforward_ptr)_tmpD9);{struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dynforward_ptr)_tmpD9,({const char*_tmp11C="r";
_tag_dynforward(_tmp11C,sizeof(char),_get_zero_arr_size(_tmp11C,2));}),({const
char*_tmp11D="file";_tag_dynforward(_tmp11D,sizeof(char),_get_zero_arr_size(
_tmp11D,5));}));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{struct
_handler_cons _tmpF9;_push_handler(& _tmpF9);{int _tmpFB=0;if(setjmp(_tmpF9.handler))
_tmpFB=1;if(!_tmpFB){tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),
struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*
_tmpFC="parsing";_tag_dynforward(_tmpFC,sizeof(char),_get_zero_arr_size(_tmpFC,8));}),
Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);;_pop_handler();}
else{void*_tmpFA=(void*)_exn_thrown;void*_tmpFE=_tmpFA;_LL27:;_LL28: Cyc_file_close((
struct Cyc___cycFILE*)_check_null(in_file));Cyc_remove_file((struct
_dynforward_ptr)_tmpD9);(int)_throw(_tmpFE);_LL29:;_LL2A:(void)_throw(_tmpFE);
_LL26:;}}}Cyc_file_close((struct Cyc___cycFILE*)in_file);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpD9);return;}if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);return;}{struct _RegionHandle _tmpFF=_new_region("tc_rgn");
struct _RegionHandle*tc_rgn=& _tmpFF;_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*
_tmp100=Cyc_Tcenv_tc_init(tc_rgn);if(Cyc_parseonly_r){_npop_handler(0);goto
PRINTC;}{struct _tuple13 _tmp101=({struct _tuple13 _tmp109;_tmp109.f1=tc_rgn;_tmp109.f2=
_tmp100;_tmp109;});tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,
struct Cyc_List_List*(*f)(struct _tuple13*,struct Cyc_List_List*),struct _tuple13*
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp102="type checking";
_tag_dynforward(_tmp102,sizeof(char),_get_zero_arr_size(_tmp102,14));}),Cyc_do_typecheck,&
_tmp101,tds);if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)
_tmpD9);_npop_handler(0);return;}if(Cyc_tc_r){_npop_handler(0);goto PRINTC;}tds=((
struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const
char*_tmp103="control-flow checking";_tag_dynforward(_tmp103,sizeof(char),
_get_zero_arr_size(_tmp103,22));}),Cyc_do_cfcheck,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpD9);_npop_handler(0);return;}if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)((char*)
_untag_dynforward_ptr(_tmpDA,sizeof(char),1)),(const char*)"r");struct Cyc___cycFILE*
inter_objfile=Cyc_try_file_open((struct _dynforward_ptr)_tmpDB,({const char*
_tmp107="w";_tag_dynforward(_tmp107,sizeof(char),_get_zero_arr_size(_tmp107,2));}),({
const char*_tmp108="interface object file";_tag_dynforward(_tmp108,sizeof(char),
_get_zero_arr_size(_tmp108,22));}));if(inter_objfile == 0){Cyc_compile_failure=1;
_npop_handler(0);return;}Cyc_Position_reset_position((struct _dynforward_ptr)
_tmpDA);{struct _tuple14 _tmp104=({struct _tuple14 _tmp106;_tmp106.f1=_tmp100;
_tmp106.f2=inter_file;_tmp106.f3=(struct Cyc___cycFILE*)inter_objfile;_tmp106;});
tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*
f)(struct _tuple14*,struct Cyc_List_List*),struct _tuple14*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp105="interface checking";_tag_dynforward(
_tmp105,sizeof(char),_get_zero_arr_size(_tmp105,19));}),Cyc_do_interface,&
_tmp104,tds);if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);}}}};_pop_region(tc_rgn);}
if(Cyc_cf_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp10A="translation to C";_tag_dynforward(
_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,17));}),Cyc_do_translate,1,tds);
if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpD9);return;}
if(!Cyc_compile_failure)Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct
_dynforward_ptr)_tmpD9);if(Cyc_compile_failure)return;if(Cyc_tovc_r  || Cyc_elim_se_r)
tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp10B="post-pass to VC";_tag_dynforward(_tmp10B,sizeof(char),
_get_zero_arr_size(_tmp10B,16));}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure)
return;PRINTC: {struct Cyc___cycFILE*out_file;if((Cyc_parseonly_r  || Cyc_tc_r)
 || Cyc_cf_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp10C="w";
_tag_dynforward(_tmp10C,sizeof(char),_get_zero_arr_size(_tmp10C,2));}),({const
char*_tmp10D="output file";_tag_dynforward(_tmp10D,sizeof(char),
_get_zero_arr_size(_tmp10D,12));}));else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}
else{if(Cyc_toc_r  && Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp10E="w";
_tag_dynforward(_tmp10E,sizeof(char),_get_zero_arr_size(_tmp10E,2));}),({const
char*_tmp10F="output file";_tag_dynforward(_tmp10F,sizeof(char),
_get_zero_arr_size(_tmp10F,12));}));else{out_file=Cyc_try_file_open((struct
_dynforward_ptr)_tmpDC,({const char*_tmp110="w";_tag_dynforward(_tmp110,sizeof(
char),_get_zero_arr_size(_tmp110,2));}),({const char*_tmp111="output file";
_tag_dynforward(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,12));}));}}if(
Cyc_compile_failure  || !((unsigned int)out_file))return;{struct _handler_cons
_tmp112;_push_handler(& _tmp112);{int _tmp114=0;if(setjmp(_tmp112.handler))_tmp114=
1;if(!_tmp114){if(!Cyc_noprint_r)tds=((struct Cyc_List_List*(*)(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct
Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp115="printing";_tag_dynforward(_tmp115,sizeof(char),
_get_zero_arr_size(_tmp115,9));}),Cyc_do_print,(struct Cyc___cycFILE*)out_file,
tds);;_pop_handler();}else{void*_tmp113=(void*)_exn_thrown;void*_tmp117=_tmp113;
_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((struct Cyc___cycFILE*)out_file);
Cyc_cfiles=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->hd=({
struct _dynforward_ptr*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=(struct
_dynforward_ptr)_tmpDC;_tmp119;});_tmp118->tl=Cyc_cfiles;_tmp118;});(int)_throw(
_tmp117);_LL2E:;_LL2F:(void)_throw(_tmp117);_LL2B:;}}}Cyc_file_close((struct Cyc___cycFILE*)
out_file);Cyc_cfiles=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));
_tmp11A->hd=({struct _dynforward_ptr*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B[
0]=(struct _dynforward_ptr)_tmpDC;_tmp11B;});_tmp11A->tl=Cyc_cfiles;_tmp11A;});}}}}}}}}}}
static char _tmp134[8]="<final>";static struct _dynforward_ptr Cyc_final_str={_tmp134,
_tmp134 + 8};static struct _dynforward_ptr*Cyc_final_strptr=& Cyc_final_str;static
struct Cyc_Interface_I*Cyc_read_cycio(struct _dynforward_ptr*n){if(n == (struct
_dynforward_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{struct
_dynforward_ptr basename;{struct _handler_cons _tmp135;_push_handler(& _tmp135);{int
_tmp137=0;if(setjmp(_tmp135.handler))_tmp137=1;if(!_tmp137){basename=(struct
_dynforward_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp136=(void*)_exn_thrown;void*_tmp139=_tmp136;_LL31: if(*((void**)_tmp139)!= 
Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(
void)_throw(_tmp139);_LL30:;}}}{struct _dynforward_ptr _tmp13A=Cyc_strconcat((
struct _dynforward_ptr)basename,({const char*_tmp13F=".cycio";_tag_dynforward(
_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,7));}));struct Cyc___cycFILE*
_tmp13B=Cyc_try_file_open((struct _dynforward_ptr)_tmp13A,({const char*_tmp13D="rb";
_tag_dynforward(_tmp13D,sizeof(char),_get_zero_arr_size(_tmp13D,3));}),({const
char*_tmp13E="interface object file";_tag_dynforward(_tmp13E,sizeof(char),
_get_zero_arr_size(_tmp13E,22));}));if(_tmp13B == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();
exit(1);}Cyc_Position_reset_position((struct _dynforward_ptr)_tmp13A);{struct Cyc_Interface_I*
_tmp13C=Cyc_Interface_load((struct Cyc___cycFILE*)_tmp13B);Cyc_file_close((struct
Cyc___cycFILE*)_tmp13B);return _tmp13C;}}}}static int Cyc_is_cfile(struct
_dynforward_ptr*n){return*((const char*)_check_dynforward_subscript(*n,sizeof(
char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple15{struct
_dynforward_ptr f1;int f2;struct _dynforward_ptr f3;void*f4;struct _dynforward_ptr f5;
};int Cyc_main(int argc,struct _dynforward_ptr argv){GC_blacklist_warn_clear();{
struct Cyc_List_List*cyclone_arch_path;struct _dynforward_ptr def_lib_path;struct
_dynforward_ptr comp=(struct _dynforward_ptr)Cstring_to_string(Ccomp);{struct
_RegionHandle _tmp140=_new_region("r");struct _RegionHandle*r=& _tmp140;
_push_region(r);{struct _dynforward_ptr _tmp141=({unsigned int _tmp2DE=(
unsigned int)argc;int*_tmp2DF=(int*)_cycalloc_atomic(_check_times(sizeof(int),
_tmp2DE));struct _dynforward_ptr _tmp2E1=_tag_dynforward(_tmp2DF,sizeof(int),
_tmp2DE);{unsigned int _tmp2E0=_tmp2DE;unsigned int i;for(i=0;i < _tmp2E0;i ++){
_tmp2DF[i]=0;}}_tmp2E1;});int _tmp142=0;int i;int j;int k;for(i=1;i < argc;i ++){if(
Cyc_strncmp(({const char*_tmp143="-B";_tag_dynforward(_tmp143,sizeof(char),
_get_zero_arr_size(_tmp143,3));}),(struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
i)),2)== 0){*((int*)_check_dynforward_subscript(_tmp141,sizeof(int),i))=1;
_tmp142 ++;}else{if(Cyc_strcmp(({const char*_tmp144="-b";_tag_dynforward(_tmp144,
sizeof(char),_get_zero_arr_size(_tmp144,3));}),(struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
i)))== 0){*((int*)_check_dynforward_subscript(_tmp141,sizeof(int),i))=1;_tmp142
++;i ++;if(i >= argc)break;*((int*)_check_dynforward_subscript(_tmp141,sizeof(int),
i))=1;_tmp142 ++;}}}{struct _dynforward_ptr _tmp145=({unsigned int _tmp2DA=(
unsigned int)(_tmp142 + 1);struct _dynforward_ptr*_tmp2DB=(struct _dynforward_ptr*)
_cycalloc(_check_times(sizeof(struct _dynforward_ptr),_tmp2DA));struct
_dynforward_ptr _tmp2DD=_tag_dynforward(_tmp2DB,sizeof(struct _dynforward_ptr),
_tmp2DA);{unsigned int _tmp2DC=_tmp2DA;unsigned int n;for(n=0;n < _tmp2DC;n ++){
_tmp2DB[n]=(struct _dynforward_ptr)_tag_dynforward(0,0,0);}}_tmp2DD;});struct
_dynforward_ptr _tmp146=({unsigned int _tmp2D6=(unsigned int)(argc - _tmp142);
struct _dynforward_ptr*_tmp2D7=(struct _dynforward_ptr*)_cycalloc(_check_times(
sizeof(struct _dynforward_ptr),_tmp2D6));struct _dynforward_ptr _tmp2D9=
_tag_dynforward(_tmp2D7,sizeof(struct _dynforward_ptr),_tmp2D6);{unsigned int
_tmp2D8=_tmp2D6;unsigned int n;for(n=0;n < _tmp2D8;n ++){_tmp2D7[n]=(struct
_dynforward_ptr)_tag_dynforward(0,0,0);}}_tmp2D9;});*((struct _dynforward_ptr*)
_check_dynforward_subscript(_tmp145,sizeof(struct _dynforward_ptr),0))=(*((struct
_dynforward_ptr*)_check_dynforward_subscript(_tmp146,sizeof(struct
_dynforward_ptr),0))=*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),0)));for(i=(j=(k=1));i < argc;i ++){if(*((int*)
_check_dynforward_subscript(_tmp141,sizeof(int),i)))*((struct _dynforward_ptr*)
_check_dynforward_subscript(_tmp145,sizeof(struct _dynforward_ptr),j ++))=*((
struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct
_dynforward_ptr),i));else{*((struct _dynforward_ptr*)_check_dynforward_subscript(
_tmp146,sizeof(struct _dynforward_ptr),k ++))=*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),i));}}{struct Cyc_List_List*
options=({struct _tuple15*_tmp161[62];_tmp161[61]=({struct _tuple15*_tmp2D0=
_region_malloc(r,sizeof(*_tmp2D0));_tmp2D0->f1=({const char*_tmp2D5="-noregions";
_tag_dynforward(_tmp2D5,sizeof(char),_get_zero_arr_size(_tmp2D5,11));});_tmp2D0->f2=
0;_tmp2D0->f3=({const char*_tmp2D4="";_tag_dynforward(_tmp2D4,sizeof(char),
_get_zero_arr_size(_tmp2D4,1));});_tmp2D0->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2D2=_region_malloc(r,sizeof(*_tmp2D2));_tmp2D2[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2D3;_tmp2D3.tag=3;_tmp2D3.f1=& Cyc_Absyn_no_regions;_tmp2D3;});_tmp2D2;});
_tmp2D0->f5=({const char*_tmp2D1="Generate code that doesn't use regions";
_tag_dynforward(_tmp2D1,sizeof(char),_get_zero_arr_size(_tmp2D1,39));});_tmp2D0;});
_tmp161[60]=({struct _tuple15*_tmp2CA=_region_malloc(r,sizeof(*_tmp2CA));_tmp2CA->f1=({
const char*_tmp2CF="-port";_tag_dynforward(_tmp2CF,sizeof(char),
_get_zero_arr_size(_tmp2CF,6));});_tmp2CA->f2=0;_tmp2CA->f3=({const char*_tmp2CE="";
_tag_dynforward(_tmp2CE,sizeof(char),_get_zero_arr_size(_tmp2CE,1));});_tmp2CA->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp2CC=_region_malloc(r,sizeof(*_tmp2CC));
_tmp2CC[0]=({struct Cyc_Arg_Unit_spec_struct _tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=Cyc_set_port_c_code;
_tmp2CD;});_tmp2CC;});_tmp2CA->f5=({const char*_tmp2CB="Suggest how to port C code to Cyclone";
_tag_dynforward(_tmp2CB,sizeof(char),_get_zero_arr_size(_tmp2CB,38));});_tmp2CA;});
_tmp161[59]=({struct _tuple15*_tmp2C4=_region_malloc(r,sizeof(*_tmp2C4));_tmp2C4->f1=({
const char*_tmp2C9="-detailedlocation";_tag_dynforward(_tmp2C9,sizeof(char),
_get_zero_arr_size(_tmp2C9,18));});_tmp2C4->f2=0;_tmp2C4->f3=({const char*_tmp2C8="";
_tag_dynforward(_tmp2C8,sizeof(char),_get_zero_arr_size(_tmp2C8,1));});_tmp2C4->f4=(
void*)({struct Cyc_Arg_Clear_spec_struct*_tmp2C6=_region_malloc(r,sizeof(*_tmp2C6));
_tmp2C6[0]=({struct Cyc_Arg_Clear_spec_struct _tmp2C7;_tmp2C7.tag=4;_tmp2C7.f1=&
Cyc_Position_use_gcc_style_location;_tmp2C7;});_tmp2C6;});_tmp2C4->f5=({const
char*_tmp2C5="Try to give more detailed location information for errors";
_tag_dynforward(_tmp2C5,sizeof(char),_get_zero_arr_size(_tmp2C5,58));});_tmp2C4;});
_tmp161[58]=({struct _tuple15*_tmp2BE=_region_malloc(r,sizeof(*_tmp2BE));_tmp2BE->f1=({
const char*_tmp2C3="-noregister";_tag_dynforward(_tmp2C3,sizeof(char),
_get_zero_arr_size(_tmp2C3,12));});_tmp2BE->f2=0;_tmp2BE->f3=({const char*_tmp2C2="";
_tag_dynforward(_tmp2C2,sizeof(char),_get_zero_arr_size(_tmp2C2,1));});_tmp2BE->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2C0=_region_malloc(r,sizeof(*_tmp2C0));
_tmp2C0[0]=({struct Cyc_Arg_Set_spec_struct _tmp2C1;_tmp2C1.tag=3;_tmp2C1.f1=& Cyc_Parse_no_register;
_tmp2C1;});_tmp2C0;});_tmp2BE->f5=({const char*_tmp2BF="Treat register storage class as public";
_tag_dynforward(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,39));});_tmp2BE;});
_tmp161[57]=({struct _tuple15*_tmp2B8=_region_malloc(r,sizeof(*_tmp2B8));_tmp2B8->f1=({
const char*_tmp2BD="-warnnullchecks";_tag_dynforward(_tmp2BD,sizeof(char),
_get_zero_arr_size(_tmp2BD,16));});_tmp2B8->f2=0;_tmp2B8->f3=({const char*_tmp2BC="";
_tag_dynforward(_tmp2BC,sizeof(char),_get_zero_arr_size(_tmp2BC,1));});_tmp2B8->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2BA=_region_malloc(r,sizeof(*_tmp2BA));
_tmp2BA[0]=({struct Cyc_Arg_Set_spec_struct _tmp2BB;_tmp2BB.tag=3;_tmp2BB.f1=& Cyc_Toc_warn_all_null_deref;
_tmp2BB;});_tmp2BA;});_tmp2B8->f5=({const char*_tmp2B9="Warn when any null check can't be eliminated";
_tag_dynforward(_tmp2B9,sizeof(char),_get_zero_arr_size(_tmp2B9,45));});_tmp2B8;});
_tmp161[56]=({struct _tuple15*_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));_tmp2B2->f1=({
const char*_tmp2B7="-warnboundschecks";_tag_dynforward(_tmp2B7,sizeof(char),
_get_zero_arr_size(_tmp2B7,18));});_tmp2B2->f2=0;_tmp2B2->f3=({const char*_tmp2B6="";
_tag_dynforward(_tmp2B6,sizeof(char),_get_zero_arr_size(_tmp2B6,1));});_tmp2B2->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2B4=_region_malloc(r,sizeof(*_tmp2B4));
_tmp2B4[0]=({struct Cyc_Arg_Set_spec_struct _tmp2B5;_tmp2B5.tag=3;_tmp2B5.f1=& Cyc_Toc_warn_bounds_checks;
_tmp2B5;});_tmp2B4;});_tmp2B2->f5=({const char*_tmp2B3="Warn when bounds checks can't be eliminated";
_tag_dynforward(_tmp2B3,sizeof(char),_get_zero_arr_size(_tmp2B3,44));});_tmp2B2;});
_tmp161[55]=({struct _tuple15*_tmp2AC=_region_malloc(r,sizeof(*_tmp2AC));_tmp2AC->f1=({
const char*_tmp2B1="-CI";_tag_dynforward(_tmp2B1,sizeof(char),_get_zero_arr_size(
_tmp2B1,4));});_tmp2AC->f2=0;_tmp2AC->f3=({const char*_tmp2B0=" <file>";
_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,8));});_tmp2AC->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp2AE=_region_malloc(r,sizeof(*
_tmp2AE));_tmp2AE[0]=({struct Cyc_Arg_String_spec_struct _tmp2AF;_tmp2AF.tag=5;
_tmp2AF.f1=Cyc_set_cyc_include;_tmp2AF;});_tmp2AE;});_tmp2AC->f5=({const char*
_tmp2AD="Set cyc_include.h to be <file>";_tag_dynforward(_tmp2AD,sizeof(char),
_get_zero_arr_size(_tmp2AD,31));});_tmp2AC;});_tmp161[54]=({struct _tuple15*
_tmp2A6=_region_malloc(r,sizeof(*_tmp2A6));_tmp2A6->f1=({const char*_tmp2AB="-printalleffects";
_tag_dynforward(_tmp2AB,sizeof(char),_get_zero_arr_size(_tmp2AB,17));});_tmp2A6->f2=
0;_tmp2A6->f3=({const char*_tmp2AA="";_tag_dynforward(_tmp2AA,sizeof(char),
_get_zero_arr_size(_tmp2AA,1));});_tmp2A6->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2A8=_region_malloc(r,sizeof(*_tmp2A8));_tmp2A8[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2A9;_tmp2A9.tag=3;_tmp2A9.f1=& Cyc_print_all_effects_r;_tmp2A9;});_tmp2A8;});
_tmp2A6->f5=({const char*_tmp2A7="Print effects for functions (type debugging)";
_tag_dynforward(_tmp2A7,sizeof(char),_get_zero_arr_size(_tmp2A7,45));});_tmp2A6;});
_tmp161[53]=({struct _tuple15*_tmp2A0=_region_malloc(r,sizeof(*_tmp2A0));_tmp2A0->f1=({
const char*_tmp2A5="-printfullevars";_tag_dynforward(_tmp2A5,sizeof(char),
_get_zero_arr_size(_tmp2A5,16));});_tmp2A0->f2=0;_tmp2A0->f3=({const char*_tmp2A4="";
_tag_dynforward(_tmp2A4,sizeof(char),_get_zero_arr_size(_tmp2A4,1));});_tmp2A0->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2A2=_region_malloc(r,sizeof(*_tmp2A2));
_tmp2A2[0]=({struct Cyc_Arg_Set_spec_struct _tmp2A3;_tmp2A3.tag=3;_tmp2A3.f1=& Cyc_print_full_evars_r;
_tmp2A3;});_tmp2A2;});_tmp2A0->f5=({const char*_tmp2A1="Print full information for evars (type debugging)";
_tag_dynforward(_tmp2A1,sizeof(char),_get_zero_arr_size(_tmp2A1,50));});_tmp2A0;});
_tmp161[52]=({struct _tuple15*_tmp29A=_region_malloc(r,sizeof(*_tmp29A));_tmp29A->f1=({
const char*_tmp29F="-printallkinds";_tag_dynforward(_tmp29F,sizeof(char),
_get_zero_arr_size(_tmp29F,15));});_tmp29A->f2=0;_tmp29A->f3=({const char*_tmp29E="";
_tag_dynforward(_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,1));});_tmp29A->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp29C=_region_malloc(r,sizeof(*_tmp29C));
_tmp29C[0]=({struct Cyc_Arg_Set_spec_struct _tmp29D;_tmp29D.tag=3;_tmp29D.f1=& Cyc_print_all_kinds_r;
_tmp29D;});_tmp29C;});_tmp29A->f5=({const char*_tmp29B="Always print kinds of type variables";
_tag_dynforward(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,37));});_tmp29A;});
_tmp161[51]=({struct _tuple15*_tmp294=_region_malloc(r,sizeof(*_tmp294));_tmp294->f1=({
const char*_tmp299="-printalltvars";_tag_dynforward(_tmp299,sizeof(char),
_get_zero_arr_size(_tmp299,15));});_tmp294->f2=0;_tmp294->f3=({const char*_tmp298="";
_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,1));});_tmp294->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp296=_region_malloc(r,sizeof(*_tmp296));
_tmp296[0]=({struct Cyc_Arg_Set_spec_struct _tmp297;_tmp297.tag=3;_tmp297.f1=& Cyc_print_all_tvars_r;
_tmp297;});_tmp296;});_tmp294->f5=({const char*_tmp295="Print all type variables (even implicit default effects)";
_tag_dynforward(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,57));});_tmp294;});
_tmp161[50]=({struct _tuple15*_tmp28E=_region_malloc(r,sizeof(*_tmp28E));_tmp28E->f1=({
const char*_tmp293="-noexpandtypedefs";_tag_dynforward(_tmp293,sizeof(char),
_get_zero_arr_size(_tmp293,18));});_tmp28E->f2=0;_tmp28E->f3=({const char*_tmp292="";
_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,1));});_tmp28E->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp290=_region_malloc(r,sizeof(*_tmp290));
_tmp290[0]=({struct Cyc_Arg_Set_spec_struct _tmp291;_tmp291.tag=3;_tmp291.f1=& Cyc_noexpand_r;
_tmp291;});_tmp290;});_tmp28E->f5=({const char*_tmp28F="Don't expand typedefs in pretty printing";
_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,41));});_tmp28E;});
_tmp161[49]=({struct _tuple15*_tmp288=_region_malloc(r,sizeof(*_tmp288));_tmp288->f1=({
const char*_tmp28D="-noremoveunused";_tag_dynforward(_tmp28D,sizeof(char),
_get_zero_arr_size(_tmp28D,16));});_tmp288->f2=0;_tmp288->f3=({const char*_tmp28C="";
_tag_dynforward(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,1));});_tmp288->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp28A=_region_malloc(r,sizeof(*_tmp28A));
_tmp28A[0]=({struct Cyc_Arg_Set_spec_struct _tmp28B;_tmp28B.tag=3;_tmp28B.f1=& Cyc_noshake_r;
_tmp28B;});_tmp28A;});_tmp288->f5=({const char*_tmp289="Don't remove externed variables that aren't used";
_tag_dynforward(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,49));});_tmp288;});
_tmp161[48]=({struct _tuple15*_tmp282=_region_malloc(r,sizeof(*_tmp282));_tmp282->f1=({
const char*_tmp287="-nogc";_tag_dynforward(_tmp287,sizeof(char),
_get_zero_arr_size(_tmp287,6));});_tmp282->f2=0;_tmp282->f3=({const char*_tmp286="";
_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,1));});_tmp282->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp284=_region_malloc(r,sizeof(*_tmp284));
_tmp284[0]=({struct Cyc_Arg_Set_spec_struct _tmp285;_tmp285.tag=3;_tmp285.f1=& Cyc_nogc_r;
_tmp285;});_tmp284;});_tmp282->f5=({const char*_tmp283="Don't link in the garbage collector";
_tag_dynforward(_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,36));});_tmp282;});
_tmp161[47]=({struct _tuple15*_tmp27C=_region_malloc(r,sizeof(*_tmp27C));_tmp27C->f1=({
const char*_tmp281="-nocyc";_tag_dynforward(_tmp281,sizeof(char),
_get_zero_arr_size(_tmp281,7));});_tmp27C->f2=0;_tmp27C->f3=({const char*_tmp280="";
_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,1));});_tmp27C->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp27E=_region_malloc(r,sizeof(*_tmp27E));
_tmp27E[0]=({struct Cyc_Arg_Unit_spec_struct _tmp27F;_tmp27F.tag=0;_tmp27F.f1=Cyc_set_nocyc;
_tmp27F;});_tmp27E;});_tmp27C->f5=({const char*_tmp27D="Don't add implicit namespace Cyc";
_tag_dynforward(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,33));});_tmp27C;});
_tmp161[46]=({struct _tuple15*_tmp276=_region_malloc(r,sizeof(*_tmp276));_tmp276->f1=({
const char*_tmp27B="-no-cpp-precomp";_tag_dynforward(_tmp27B,sizeof(char),
_get_zero_arr_size(_tmp27B,16));});_tmp276->f2=0;_tmp276->f3=({const char*_tmp27A="";
_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size(_tmp27A,1));});_tmp276->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp278=_region_malloc(r,sizeof(*_tmp278));
_tmp278[0]=({struct Cyc_Arg_Unit_spec_struct _tmp279;_tmp279.tag=0;_tmp279.f1=Cyc_set_nocppprecomp;
_tmp279;});_tmp278;});_tmp276->f5=({const char*_tmp277="Don't do smart preprocessing (mac only)";
_tag_dynforward(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,40));});_tmp276;});
_tmp161[45]=({struct _tuple15*_tmp270=_region_malloc(r,sizeof(*_tmp270));_tmp270->f1=({
const char*_tmp275="-use-cpp";_tag_dynforward(_tmp275,sizeof(char),
_get_zero_arr_size(_tmp275,9));});_tmp270->f2=0;_tmp270->f3=({const char*_tmp274="<path>";
_tag_dynforward(_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,7));});_tmp270->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp272=_region_malloc(r,sizeof(*
_tmp272));_tmp272[0]=({struct Cyc_Arg_String_spec_struct _tmp273;_tmp273.tag=5;
_tmp273.f1=Cyc_set_cpp;_tmp273;});_tmp272;});_tmp270->f5=({const char*_tmp271="Indicate which preprocessor to use";
_tag_dynforward(_tmp271,sizeof(char),_get_zero_arr_size(_tmp271,35));});_tmp270;});
_tmp161[44]=({struct _tuple15*_tmp26A=_region_malloc(r,sizeof(*_tmp26A));_tmp26A->f1=({
const char*_tmp26F="--inline-checks";_tag_dynforward(_tmp26F,sizeof(char),
_get_zero_arr_size(_tmp26F,16));});_tmp26A->f2=0;_tmp26A->f3=({const char*_tmp26E="";
_tag_dynforward(_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,1));});_tmp26A->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp26C=_region_malloc(r,sizeof(*_tmp26C));
_tmp26C[0]=({struct Cyc_Arg_Unit_spec_struct _tmp26D;_tmp26D.tag=0;_tmp26D.f1=Cyc_set_inline_functions;
_tmp26D;});_tmp26C;});_tmp26A->f5=({const char*_tmp26B="Inline bounds checks instead of #define";
_tag_dynforward(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,40));});_tmp26A;});
_tmp161[43]=({struct _tuple15*_tmp264=_region_malloc(r,sizeof(*_tmp264));_tmp264->f1=({
const char*_tmp269="--noboundschecks";_tag_dynforward(_tmp269,sizeof(char),
_get_zero_arr_size(_tmp269,17));});_tmp264->f2=0;_tmp264->f3=({const char*_tmp268="";
_tag_dynforward(_tmp268,sizeof(char),_get_zero_arr_size(_tmp268,1));});_tmp264->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp266=_region_malloc(r,sizeof(*_tmp266));
_tmp266[0]=({struct Cyc_Arg_Unit_spec_struct _tmp267;_tmp267.tag=0;_tmp267.f1=Cyc_set_noboundschecks;
_tmp267;});_tmp266;});_tmp264->f5=({const char*_tmp265="Disable bounds checks";
_tag_dynforward(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,22));});_tmp264;});
_tmp161[42]=({struct _tuple15*_tmp25E=_region_malloc(r,sizeof(*_tmp25E));_tmp25E->f1=({
const char*_tmp263="--nonullchecks";_tag_dynforward(_tmp263,sizeof(char),
_get_zero_arr_size(_tmp263,15));});_tmp25E->f2=0;_tmp25E->f3=({const char*_tmp262="";
_tag_dynforward(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,1));});_tmp25E->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp260=_region_malloc(r,sizeof(*_tmp260));
_tmp260[0]=({struct Cyc_Arg_Unit_spec_struct _tmp261;_tmp261.tag=0;_tmp261.f1=Cyc_set_nonullchecks;
_tmp261;});_tmp260;});_tmp25E->f5=({const char*_tmp25F="Disable null checks";
_tag_dynforward(_tmp25F,sizeof(char),_get_zero_arr_size(_tmp25F,20));});_tmp25E;});
_tmp161[41]=({struct _tuple15*_tmp258=_region_malloc(r,sizeof(*_tmp258));_tmp258->f1=({
const char*_tmp25D="--nochecks";_tag_dynforward(_tmp25D,sizeof(char),
_get_zero_arr_size(_tmp25D,11));});_tmp258->f2=0;_tmp258->f3=({const char*_tmp25C="";
_tag_dynforward(_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,1));});_tmp258->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp25A=_region_malloc(r,sizeof(*_tmp25A));
_tmp25A[0]=({struct Cyc_Arg_Unit_spec_struct _tmp25B;_tmp25B.tag=0;_tmp25B.f1=Cyc_set_nochecks;
_tmp25B;});_tmp25A;});_tmp258->f5=({const char*_tmp259="Disable bounds/null checks";
_tag_dynforward(_tmp259,sizeof(char),_get_zero_arr_size(_tmp259,27));});_tmp258;});
_tmp161[40]=({struct _tuple15*_tmp252=_region_malloc(r,sizeof(*_tmp252));_tmp252->f1=({
const char*_tmp257="--lineno";_tag_dynforward(_tmp257,sizeof(char),
_get_zero_arr_size(_tmp257,9));});_tmp252->f2=0;_tmp252->f3=({const char*_tmp256="";
_tag_dynforward(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,1));});_tmp252->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp254=_region_malloc(r,sizeof(*_tmp254));
_tmp254[0]=({struct Cyc_Arg_Unit_spec_struct _tmp255;_tmp255.tag=0;_tmp255.f1=Cyc_set_lineno;
_tmp255;});_tmp254;});_tmp252->f5=({const char*_tmp253="Generate line numbers for debugging";
_tag_dynforward(_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,36));});_tmp252;});
_tmp161[39]=({struct _tuple15*_tmp24C=_region_malloc(r,sizeof(*_tmp24C));_tmp24C->f1=({
const char*_tmp251="-save-c";_tag_dynforward(_tmp251,sizeof(char),
_get_zero_arr_size(_tmp251,8));});_tmp24C->f2=0;_tmp24C->f3=({const char*_tmp250="";
_tag_dynforward(_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,1));});_tmp24C->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp24E=_region_malloc(r,sizeof(*_tmp24E));
_tmp24E[0]=({struct Cyc_Arg_Set_spec_struct _tmp24F;_tmp24F.tag=3;_tmp24F.f1=& Cyc_save_c_r;
_tmp24F;});_tmp24E;});_tmp24C->f5=({const char*_tmp24D="Don't delete temporary C files";
_tag_dynforward(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,31));});_tmp24C;});
_tmp161[38]=({struct _tuple15*_tmp246=_region_malloc(r,sizeof(*_tmp246));_tmp246->f1=({
const char*_tmp24B="-save-temps";_tag_dynforward(_tmp24B,sizeof(char),
_get_zero_arr_size(_tmp24B,12));});_tmp246->f2=0;_tmp246->f3=({const char*_tmp24A="";
_tag_dynforward(_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,1));});_tmp246->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp248=_region_malloc(r,sizeof(*_tmp248));
_tmp248[0]=({struct Cyc_Arg_Unit_spec_struct _tmp249;_tmp249.tag=0;_tmp249.f1=Cyc_set_save_temps;
_tmp249;});_tmp248;});_tmp246->f5=({const char*_tmp247="Don't delete temporary files";
_tag_dynforward(_tmp247,sizeof(char),_get_zero_arr_size(_tmp247,29));});_tmp246;});
_tmp161[37]=({struct _tuple15*_tmp240=_region_malloc(r,sizeof(*_tmp240));_tmp240->f1=({
const char*_tmp245="-tovc";_tag_dynforward(_tmp245,sizeof(char),
_get_zero_arr_size(_tmp245,6));});_tmp240->f2=0;_tmp240->f3=({const char*_tmp244="";
_tag_dynforward(_tmp244,sizeof(char),_get_zero_arr_size(_tmp244,1));});_tmp240->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp242=_region_malloc(r,sizeof(*_tmp242));
_tmp242[0]=({struct Cyc_Arg_Unit_spec_struct _tmp243;_tmp243.tag=0;_tmp243.f1=Cyc_set_tovc;
_tmp243;});_tmp242;});_tmp240->f5=({const char*_tmp241="Avoid gcc extensions in C output";
_tag_dynforward(_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,33));});_tmp240;});
_tmp161[36]=({struct _tuple15*_tmp23A=_region_malloc(r,sizeof(*_tmp23A));_tmp23A->f1=({
const char*_tmp23F="-elim-statement-expressions";_tag_dynforward(_tmp23F,sizeof(
char),_get_zero_arr_size(_tmp23F,28));});_tmp23A->f2=0;_tmp23A->f3=({const char*
_tmp23E="";_tag_dynforward(_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,1));});
_tmp23A->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp23C=_region_malloc(r,
sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Arg_Unit_spec_struct _tmp23D;_tmp23D.tag=
0;_tmp23D.f1=Cyc_set_elim_se;_tmp23D;});_tmp23C;});_tmp23A->f5=({const char*
_tmp23B="Avoid statement expressions in C output (implies --inline-checks)";
_tag_dynforward(_tmp23B,sizeof(char),_get_zero_arr_size(_tmp23B,66));});_tmp23A;});
_tmp161[35]=({struct _tuple15*_tmp234=_region_malloc(r,sizeof(*_tmp234));_tmp234->f1=({
const char*_tmp239="-up";_tag_dynforward(_tmp239,sizeof(char),_get_zero_arr_size(
_tmp239,4));});_tmp234->f2=0;_tmp234->f3=({const char*_tmp238="";_tag_dynforward(
_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,1));});_tmp234->f4=(void*)({
struct Cyc_Arg_Clear_spec_struct*_tmp236=_region_malloc(r,sizeof(*_tmp236));
_tmp236[0]=({struct Cyc_Arg_Clear_spec_struct _tmp237;_tmp237.tag=4;_tmp237.f1=&
Cyc_pp_r;_tmp237;});_tmp236;});_tmp234->f5=({const char*_tmp235="Ugly print";
_tag_dynforward(_tmp235,sizeof(char),_get_zero_arr_size(_tmp235,11));});_tmp234;});
_tmp161[34]=({struct _tuple15*_tmp22E=_region_malloc(r,sizeof(*_tmp22E));_tmp22E->f1=({
const char*_tmp233="-pp";_tag_dynforward(_tmp233,sizeof(char),_get_zero_arr_size(
_tmp233,4));});_tmp22E->f2=0;_tmp22E->f3=({const char*_tmp232="";_tag_dynforward(
_tmp232,sizeof(char),_get_zero_arr_size(_tmp232,1));});_tmp22E->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp230=_region_malloc(r,sizeof(*_tmp230));_tmp230[
0]=({struct Cyc_Arg_Set_spec_struct _tmp231;_tmp231.tag=3;_tmp231.f1=& Cyc_pp_r;
_tmp231;});_tmp230;});_tmp22E->f5=({const char*_tmp22F="Pretty print";
_tag_dynforward(_tmp22F,sizeof(char),_get_zero_arr_size(_tmp22F,13));});_tmp22E;});
_tmp161[33]=({struct _tuple15*_tmp228=_region_malloc(r,sizeof(*_tmp228));_tmp228->f1=({
const char*_tmp22D="-ic";_tag_dynforward(_tmp22D,sizeof(char),_get_zero_arr_size(
_tmp22D,4));});_tmp228->f2=0;_tmp228->f3=({const char*_tmp22C="";_tag_dynforward(
_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,1));});_tmp228->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp22A=_region_malloc(r,sizeof(*_tmp22A));_tmp22A[
0]=({struct Cyc_Arg_Set_spec_struct _tmp22B;_tmp22B.tag=3;_tmp22B.f1=& Cyc_ic_r;
_tmp22B;});_tmp22A;});_tmp228->f5=({const char*_tmp229="Activate the link-checker";
_tag_dynforward(_tmp229,sizeof(char),_get_zero_arr_size(_tmp229,26));});_tmp228;});
_tmp161[32]=({struct _tuple15*_tmp222=_region_malloc(r,sizeof(*_tmp222));_tmp222->f1=({
const char*_tmp227="-stopafter-toc";_tag_dynforward(_tmp227,sizeof(char),
_get_zero_arr_size(_tmp227,15));});_tmp222->f2=0;_tmp222->f3=({const char*_tmp226="";
_tag_dynforward(_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,1));});_tmp222->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp224=_region_malloc(r,sizeof(*_tmp224));
_tmp224[0]=({struct Cyc_Arg_Set_spec_struct _tmp225;_tmp225.tag=3;_tmp225.f1=& Cyc_toc_r;
_tmp225;});_tmp224;});_tmp222->f5=({const char*_tmp223="Stop after translation to C";
_tag_dynforward(_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,28));});_tmp222;});
_tmp161[31]=({struct _tuple15*_tmp21C=_region_malloc(r,sizeof(*_tmp21C));_tmp21C->f1=({
const char*_tmp221="-stopafter-cf";_tag_dynforward(_tmp221,sizeof(char),
_get_zero_arr_size(_tmp221,14));});_tmp21C->f2=0;_tmp21C->f3=({const char*_tmp220="";
_tag_dynforward(_tmp220,sizeof(char),_get_zero_arr_size(_tmp220,1));});_tmp21C->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp21E=_region_malloc(r,sizeof(*_tmp21E));
_tmp21E[0]=({struct Cyc_Arg_Set_spec_struct _tmp21F;_tmp21F.tag=3;_tmp21F.f1=& Cyc_cf_r;
_tmp21F;});_tmp21E;});_tmp21C->f5=({const char*_tmp21D="Stop after control-flow checking";
_tag_dynforward(_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,33));});_tmp21C;});
_tmp161[30]=({struct _tuple15*_tmp216=_region_malloc(r,sizeof(*_tmp216));_tmp216->f1=({
const char*_tmp21B="-noprint";_tag_dynforward(_tmp21B,sizeof(char),
_get_zero_arr_size(_tmp21B,9));});_tmp216->f2=0;_tmp216->f3=({const char*_tmp21A="";
_tag_dynforward(_tmp21A,sizeof(char),_get_zero_arr_size(_tmp21A,1));});_tmp216->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp218=_region_malloc(r,sizeof(*_tmp218));
_tmp218[0]=({struct Cyc_Arg_Set_spec_struct _tmp219;_tmp219.tag=3;_tmp219.f1=& Cyc_noprint_r;
_tmp219;});_tmp218;});_tmp216->f5=({const char*_tmp217="Do not print output (when stopping early)";
_tag_dynforward(_tmp217,sizeof(char),_get_zero_arr_size(_tmp217,42));});_tmp216;});
_tmp161[29]=({struct _tuple15*_tmp210=_region_malloc(r,sizeof(*_tmp210));_tmp210->f1=({
const char*_tmp215="-stopafter-tc";_tag_dynforward(_tmp215,sizeof(char),
_get_zero_arr_size(_tmp215,14));});_tmp210->f2=0;_tmp210->f3=({const char*_tmp214="";
_tag_dynforward(_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,1));});_tmp210->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp212=_region_malloc(r,sizeof(*_tmp212));
_tmp212[0]=({struct Cyc_Arg_Set_spec_struct _tmp213;_tmp213.tag=3;_tmp213.f1=& Cyc_tc_r;
_tmp213;});_tmp212;});_tmp210->f5=({const char*_tmp211="Stop after type checking";
_tag_dynforward(_tmp211,sizeof(char),_get_zero_arr_size(_tmp211,25));});_tmp210;});
_tmp161[28]=({struct _tuple15*_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->f1=({
const char*_tmp20F="-stopafter-parse";_tag_dynforward(_tmp20F,sizeof(char),
_get_zero_arr_size(_tmp20F,17));});_tmp20A->f2=0;_tmp20A->f3=({const char*_tmp20E="";
_tag_dynforward(_tmp20E,sizeof(char),_get_zero_arr_size(_tmp20E,1));});_tmp20A->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp20C=_region_malloc(r,sizeof(*_tmp20C));
_tmp20C[0]=({struct Cyc_Arg_Set_spec_struct _tmp20D;_tmp20D.tag=3;_tmp20D.f1=& Cyc_parseonly_r;
_tmp20D;});_tmp20C;});_tmp20A->f5=({const char*_tmp20B="Stop after parsing";
_tag_dynforward(_tmp20B,sizeof(char),_get_zero_arr_size(_tmp20B,19));});_tmp20A;});
_tmp161[27]=({struct _tuple15*_tmp204=_region_malloc(r,sizeof(*_tmp204));_tmp204->f1=({
const char*_tmp209="-E";_tag_dynforward(_tmp209,sizeof(char),_get_zero_arr_size(
_tmp209,3));});_tmp204->f2=0;_tmp204->f3=({const char*_tmp208="";_tag_dynforward(
_tmp208,sizeof(char),_get_zero_arr_size(_tmp208,1));});_tmp204->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp206=_region_malloc(r,sizeof(*_tmp206));_tmp206[
0]=({struct Cyc_Arg_Set_spec_struct _tmp207;_tmp207.tag=3;_tmp207.f1=& Cyc_stop_after_cpp_r;
_tmp207;});_tmp206;});_tmp204->f5=({const char*_tmp205="Stop after preprocessing";
_tag_dynforward(_tmp205,sizeof(char),_get_zero_arr_size(_tmp205,25));});_tmp204;});
_tmp161[26]=({struct _tuple15*_tmp1FE=_region_malloc(r,sizeof(*_tmp1FE));_tmp1FE->f1=({
const char*_tmp203="-Wall";_tag_dynforward(_tmp203,sizeof(char),
_get_zero_arr_size(_tmp203,6));});_tmp1FE->f2=0;_tmp1FE->f3=({const char*_tmp202="";
_tag_dynforward(_tmp202,sizeof(char),_get_zero_arr_size(_tmp202,1));});_tmp1FE->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp200=_region_malloc(r,sizeof(*_tmp200));
_tmp200[0]=({struct Cyc_Arg_Unit_spec_struct _tmp201;_tmp201.tag=0;_tmp201.f1=Cyc_set_all_warnings;
_tmp201;});_tmp200;});_tmp1FE->f5=({const char*_tmp1FF="Turn on all warnings";
_tag_dynforward(_tmp1FF,sizeof(char),_get_zero_arr_size(_tmp1FF,21));});_tmp1FE;});
_tmp161[25]=({struct _tuple15*_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));_tmp1F8->f1=({
const char*_tmp1FD="-b";_tag_dynforward(_tmp1FD,sizeof(char),_get_zero_arr_size(
_tmp1FD,3));});_tmp1F8->f2=0;_tmp1F8->f3=({const char*_tmp1FC="<arch>";
_tag_dynforward(_tmp1FC,sizeof(char),_get_zero_arr_size(_tmp1FC,7));});_tmp1F8->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1FA=_region_malloc(r,sizeof(*
_tmp1FA));_tmp1FA[0]=({struct Cyc_Arg_String_spec_struct _tmp1FB;_tmp1FB.tag=5;
_tmp1FB.f1=Cyc_set_target_arch;_tmp1FB;});_tmp1FA;});_tmp1F8->f5=({const char*
_tmp1F9="Set target architecture; implies -stopafter-toc";_tag_dynforward(
_tmp1F9,sizeof(char),_get_zero_arr_size(_tmp1F9,48));});_tmp1F8;});_tmp161[24]=({
struct _tuple15*_tmp1F2=_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->f1=({const
char*_tmp1F7="-MT";_tag_dynforward(_tmp1F7,sizeof(char),_get_zero_arr_size(
_tmp1F7,4));});_tmp1F2->f2=0;_tmp1F2->f3=({const char*_tmp1F6=" <target>";
_tag_dynforward(_tmp1F6,sizeof(char),_get_zero_arr_size(_tmp1F6,10));});_tmp1F2->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1F4=_region_malloc(r,sizeof(*
_tmp1F4));_tmp1F4[0]=({struct Cyc_Arg_String_spec_struct _tmp1F5;_tmp1F5.tag=5;
_tmp1F5.f1=Cyc_set_dependencies_target;_tmp1F5;});_tmp1F4;});_tmp1F2->f5=({const
char*_tmp1F3="Give target for dependencies";_tag_dynforward(_tmp1F3,sizeof(char),
_get_zero_arr_size(_tmp1F3,29));});_tmp1F2;});_tmp161[23]=({struct _tuple15*
_tmp1EC=_region_malloc(r,sizeof(*_tmp1EC));_tmp1EC->f1=({const char*_tmp1F1="-MG";
_tag_dynforward(_tmp1F1,sizeof(char),_get_zero_arr_size(_tmp1F1,4));});_tmp1EC->f2=
0;_tmp1EC->f3=({const char*_tmp1F0="";_tag_dynforward(_tmp1F0,sizeof(char),
_get_zero_arr_size(_tmp1F0,1));});_tmp1EC->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1EE=_region_malloc(r,sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1EF;_tmp1EF.tag=1;_tmp1EF.f1=Cyc_add_cpparg;_tmp1EF;});_tmp1EE;});_tmp1EC->f5=({
const char*_tmp1ED="When producing dependencies assume that missing files are generated";
_tag_dynforward(_tmp1ED,sizeof(char),_get_zero_arr_size(_tmp1ED,68));});_tmp1EC;});
_tmp161[22]=({struct _tuple15*_tmp1E6=_region_malloc(r,sizeof(*_tmp1E6));_tmp1E6->f1=({
const char*_tmp1EB="-M";_tag_dynforward(_tmp1EB,sizeof(char),_get_zero_arr_size(
_tmp1EB,3));});_tmp1E6->f2=0;_tmp1E6->f3=({const char*_tmp1EA="";_tag_dynforward(
_tmp1EA,sizeof(char),_get_zero_arr_size(_tmp1EA,1));});_tmp1E6->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1E8=_region_malloc(r,sizeof(*_tmp1E8));
_tmp1E8[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1E9;_tmp1E9.tag=0;_tmp1E9.f1=Cyc_set_produce_dependencies;
_tmp1E9;});_tmp1E8;});_tmp1E6->f5=({const char*_tmp1E7="Produce dependencies";
_tag_dynforward(_tmp1E7,sizeof(char),_get_zero_arr_size(_tmp1E7,21));});_tmp1E6;});
_tmp161[21]=({struct _tuple15*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->f1=({
const char*_tmp1E5="-S";_tag_dynforward(_tmp1E5,sizeof(char),_get_zero_arr_size(
_tmp1E5,3));});_tmp1E0->f2=0;_tmp1E0->f3=({const char*_tmp1E4="";_tag_dynforward(
_tmp1E4,sizeof(char),_get_zero_arr_size(_tmp1E4,1));});_tmp1E0->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1E2=_region_malloc(r,sizeof(*_tmp1E2));
_tmp1E2[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1E3;_tmp1E3.tag=0;_tmp1E3.f1=Cyc_set_stop_after_asmfile;
_tmp1E3;});_tmp1E2;});_tmp1E0->f5=({const char*_tmp1E1="Stop after producing assembly code";
_tag_dynforward(_tmp1E1,sizeof(char),_get_zero_arr_size(_tmp1E1,35));});_tmp1E0;});
_tmp161[20]=({struct _tuple15*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));_tmp1DA->f1=({
const char*_tmp1DF="-pa";_tag_dynforward(_tmp1DF,sizeof(char),_get_zero_arr_size(
_tmp1DF,4));});_tmp1DA->f2=0;_tmp1DA->f3=({const char*_tmp1DE="";_tag_dynforward(
_tmp1DE,sizeof(char),_get_zero_arr_size(_tmp1DE,1));});_tmp1DA->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1DC=_region_malloc(r,sizeof(*_tmp1DC));
_tmp1DC[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1DD;_tmp1DD.tag=0;_tmp1DD.f1=Cyc_set_pa;
_tmp1DD;});_tmp1DC;});_tmp1DA->f5=({const char*_tmp1DB="Compile for profiling with aprof";
_tag_dynforward(_tmp1DB,sizeof(char),_get_zero_arr_size(_tmp1DB,33));});_tmp1DA;});
_tmp161[19]=({struct _tuple15*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));_tmp1D4->f1=({
const char*_tmp1D9="-pg";_tag_dynforward(_tmp1D9,sizeof(char),_get_zero_arr_size(
_tmp1D9,4));});_tmp1D4->f2=0;_tmp1D4->f3=({const char*_tmp1D8="";_tag_dynforward(
_tmp1D8,sizeof(char),_get_zero_arr_size(_tmp1D8,1));});_tmp1D4->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1D6=_region_malloc(r,sizeof(*_tmp1D6));
_tmp1D6[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1D7;_tmp1D7.tag=0;_tmp1D7.f1=Cyc_set_pg;
_tmp1D7;});_tmp1D6;});_tmp1D4->f5=({const char*_tmp1D5="Compile for profiling with gprof";
_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,33));});_tmp1D4;});
_tmp161[18]=({struct _tuple15*_tmp1CE=_region_malloc(r,sizeof(*_tmp1CE));_tmp1CE->f1=({
const char*_tmp1D3="-p";_tag_dynforward(_tmp1D3,sizeof(char),_get_zero_arr_size(
_tmp1D3,3));});_tmp1CE->f2=0;_tmp1CE->f3=({const char*_tmp1D2="";_tag_dynforward(
_tmp1D2,sizeof(char),_get_zero_arr_size(_tmp1D2,1));});_tmp1CE->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));
_tmp1D0[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=Cyc_add_ccarg;
_tmp1D1;});_tmp1D0;});_tmp1CE->f5=({const char*_tmp1CF="Compile for profiling with prof";
_tag_dynforward(_tmp1CF,sizeof(char),_get_zero_arr_size(_tmp1CF,32));});_tmp1CE;});
_tmp161[17]=({struct _tuple15*_tmp1C8=_region_malloc(r,sizeof(*_tmp1C8));_tmp1C8->f1=({
const char*_tmp1CD="-g";_tag_dynforward(_tmp1CD,sizeof(char),_get_zero_arr_size(
_tmp1CD,3));});_tmp1C8->f2=0;_tmp1C8->f3=({const char*_tmp1CC="";_tag_dynforward(
_tmp1CC,sizeof(char),_get_zero_arr_size(_tmp1CC,1));});_tmp1C8->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1CA=_region_malloc(r,sizeof(*_tmp1CA));
_tmp1CA[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1CB;_tmp1CB.tag=1;_tmp1CB.f1=Cyc_add_ccarg;
_tmp1CB;});_tmp1CA;});_tmp1C8->f5=({const char*_tmp1C9="Compile for debugging";
_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,22));});_tmp1C8;});
_tmp161[16]=({struct _tuple15*_tmp1C2=_region_malloc(r,sizeof(*_tmp1C2));_tmp1C2->f1=({
const char*_tmp1C7="-fomit-frame-pointer";_tag_dynforward(_tmp1C7,sizeof(char),
_get_zero_arr_size(_tmp1C7,21));});_tmp1C2->f2=0;_tmp1C2->f3=({const char*_tmp1C6="";
_tag_dynforward(_tmp1C6,sizeof(char),_get_zero_arr_size(_tmp1C6,1));});_tmp1C2->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1C4=_region_malloc(r,sizeof(*_tmp1C4));
_tmp1C4[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1C5;_tmp1C5.tag=1;_tmp1C5.f1=Cyc_add_ccarg;
_tmp1C5;});_tmp1C4;});_tmp1C2->f5=({const char*_tmp1C3="Omit frame pointer";
_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,19));});_tmp1C2;});
_tmp161[15]=({struct _tuple15*_tmp1BC=_region_malloc(r,sizeof(*_tmp1BC));_tmp1BC->f1=({
const char*_tmp1C1="-O3";_tag_dynforward(_tmp1C1,sizeof(char),_get_zero_arr_size(
_tmp1C1,4));});_tmp1BC->f2=0;_tmp1BC->f3=({const char*_tmp1C0="";_tag_dynforward(
_tmp1C0,sizeof(char),_get_zero_arr_size(_tmp1C0,1));});_tmp1BC->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1BE=_region_malloc(r,sizeof(*_tmp1BE));
_tmp1BE[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1BF;_tmp1BF.tag=1;_tmp1BF.f1=Cyc_add_ccarg;
_tmp1BF;});_tmp1BE;});_tmp1BC->f5=({const char*_tmp1BD="Even more optimization";
_tag_dynforward(_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,23));});_tmp1BC;});
_tmp161[14]=({struct _tuple15*_tmp1B6=_region_malloc(r,sizeof(*_tmp1B6));_tmp1B6->f1=({
const char*_tmp1BB="-O2";_tag_dynforward(_tmp1BB,sizeof(char),_get_zero_arr_size(
_tmp1BB,4));});_tmp1B6->f2=0;_tmp1B6->f3=({const char*_tmp1BA="";_tag_dynforward(
_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,1));});_tmp1B6->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1B8=_region_malloc(r,sizeof(*_tmp1B8));
_tmp1B8[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1B9;_tmp1B9.tag=1;_tmp1B9.f1=Cyc_add_ccarg;
_tmp1B9;});_tmp1B8;});_tmp1B6->f5=({const char*_tmp1B7="A higher level of optimization";
_tag_dynforward(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,31));});_tmp1B6;});
_tmp161[13]=({struct _tuple15*_tmp1B0=_region_malloc(r,sizeof(*_tmp1B0));_tmp1B0->f1=({
const char*_tmp1B5="-O";_tag_dynforward(_tmp1B5,sizeof(char),_get_zero_arr_size(
_tmp1B5,3));});_tmp1B0->f2=0;_tmp1B0->f3=({const char*_tmp1B4="";_tag_dynforward(
_tmp1B4,sizeof(char),_get_zero_arr_size(_tmp1B4,1));});_tmp1B0->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1B2=_region_malloc(r,sizeof(*_tmp1B2));
_tmp1B2[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1B3;_tmp1B3.tag=1;_tmp1B3.f1=Cyc_add_ccarg;
_tmp1B3;});_tmp1B2;});_tmp1B0->f5=({const char*_tmp1B1="Optimize";_tag_dynforward(
_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,9));});_tmp1B0;});_tmp161[12]=({
struct _tuple15*_tmp1AA=_region_malloc(r,sizeof(*_tmp1AA));_tmp1AA->f1=({const
char*_tmp1AF="-O0";_tag_dynforward(_tmp1AF,sizeof(char),_get_zero_arr_size(
_tmp1AF,4));});_tmp1AA->f2=0;_tmp1AA->f3=({const char*_tmp1AE="";_tag_dynforward(
_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,1));});_tmp1AA->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1AC=_region_malloc(r,sizeof(*_tmp1AC));
_tmp1AC[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1AD;_tmp1AD.tag=1;_tmp1AD.f1=Cyc_add_ccarg;
_tmp1AD;});_tmp1AC;});_tmp1AA->f5=({const char*_tmp1AB="Don't optimize";
_tag_dynforward(_tmp1AB,sizeof(char),_get_zero_arr_size(_tmp1AB,15));});_tmp1AA;});
_tmp161[11]=({struct _tuple15*_tmp1A4=_region_malloc(r,sizeof(*_tmp1A4));_tmp1A4->f1=({
const char*_tmp1A9="-s";_tag_dynforward(_tmp1A9,sizeof(char),_get_zero_arr_size(
_tmp1A9,3));});_tmp1A4->f2=0;_tmp1A4->f3=({const char*_tmp1A8="";_tag_dynforward(
_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,1));});_tmp1A4->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1A6=_region_malloc(r,sizeof(*_tmp1A6));
_tmp1A6[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1A7;_tmp1A7.tag=1;_tmp1A7.f1=Cyc_add_ccarg;
_tmp1A7;});_tmp1A6;});_tmp1A4->f5=({const char*_tmp1A5="Remove all symbol table and relocation info from executable";
_tag_dynforward(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,60));});_tmp1A4;});
_tmp161[10]=({struct _tuple15*_tmp19E=_region_malloc(r,sizeof(*_tmp19E));_tmp19E->f1=({
const char*_tmp1A3="-x";_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size(
_tmp1A3,3));});_tmp19E->f2=0;_tmp19E->f3=({const char*_tmp1A2=" <language>";
_tag_dynforward(_tmp1A2,sizeof(char),_get_zero_arr_size(_tmp1A2,12));});_tmp19E->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1A0=_region_malloc(r,sizeof(*
_tmp1A0));_tmp1A0[0]=({struct Cyc_Arg_String_spec_struct _tmp1A1;_tmp1A1.tag=5;
_tmp1A1.f1=Cyc_set_inputtype;_tmp1A1;});_tmp1A0;});_tmp19E->f5=({const char*
_tmp19F="Specify <language> for the following input files";_tag_dynforward(
_tmp19F,sizeof(char),_get_zero_arr_size(_tmp19F,49));});_tmp19E;});_tmp161[9]=({
struct _tuple15*_tmp198=_region_malloc(r,sizeof(*_tmp198));_tmp198->f1=({const
char*_tmp19D="-c";_tag_dynforward(_tmp19D,sizeof(char),_get_zero_arr_size(
_tmp19D,3));});_tmp198->f2=0;_tmp198->f3=({const char*_tmp19C="";_tag_dynforward(
_tmp19C,sizeof(char),_get_zero_arr_size(_tmp19C,1));});_tmp198->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp19A=_region_malloc(r,sizeof(*_tmp19A));
_tmp19A[0]=({struct Cyc_Arg_Unit_spec_struct _tmp19B;_tmp19B.tag=0;_tmp19B.f1=Cyc_set_stop_after_objectfile;
_tmp19B;});_tmp19A;});_tmp198->f5=({const char*_tmp199="Produce an object file instead of an executable; do not link";
_tag_dynforward(_tmp199,sizeof(char),_get_zero_arr_size(_tmp199,61));});_tmp198;});
_tmp161[8]=({struct _tuple15*_tmp192=_region_malloc(r,sizeof(*_tmp192));_tmp192->f1=({
const char*_tmp197="-m";_tag_dynforward(_tmp197,sizeof(char),_get_zero_arr_size(
_tmp197,3));});_tmp192->f2=1;_tmp192->f3=({const char*_tmp196="<option>";
_tag_dynforward(_tmp196,sizeof(char),_get_zero_arr_size(_tmp196,9));});_tmp192->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp194=_region_malloc(r,sizeof(*_tmp194));
_tmp194[0]=({struct Cyc_Arg_Flag_spec_struct _tmp195;_tmp195.tag=1;_tmp195.f1=Cyc_add_marg;
_tmp195;});_tmp194;});_tmp192->f5=({const char*_tmp193="GCC specific: pass machine-dependent flag on to gcc";
_tag_dynforward(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,52));});_tmp192;});
_tmp161[7]=({struct _tuple15*_tmp18C=_region_malloc(r,sizeof(*_tmp18C));_tmp18C->f1=({
const char*_tmp191="-l";_tag_dynforward(_tmp191,sizeof(char),_get_zero_arr_size(
_tmp191,3));});_tmp18C->f2=1;_tmp18C->f3=({const char*_tmp190="<libname>";
_tag_dynforward(_tmp190,sizeof(char),_get_zero_arr_size(_tmp190,10));});_tmp18C->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp18E=_region_malloc(r,sizeof(*_tmp18E));
_tmp18E[0]=({struct Cyc_Arg_Flag_spec_struct _tmp18F;_tmp18F.tag=1;_tmp18F.f1=Cyc_add_libarg;
_tmp18F;});_tmp18E;});_tmp18C->f5=({const char*_tmp18D="Library file";
_tag_dynforward(_tmp18D,sizeof(char),_get_zero_arr_size(_tmp18D,13));});_tmp18C;});
_tmp161[6]=({struct _tuple15*_tmp186=_region_malloc(r,sizeof(*_tmp186));_tmp186->f1=({
const char*_tmp18B="-L";_tag_dynforward(_tmp18B,sizeof(char),_get_zero_arr_size(
_tmp18B,3));});_tmp186->f2=1;_tmp186->f3=({const char*_tmp18A="<dir>";
_tag_dynforward(_tmp18A,sizeof(char),_get_zero_arr_size(_tmp18A,6));});_tmp186->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp188=_region_malloc(r,sizeof(*_tmp188));
_tmp188[0]=({struct Cyc_Arg_Flag_spec_struct _tmp189;_tmp189.tag=1;_tmp189.f1=Cyc_add_ccarg;
_tmp189;});_tmp188;});_tmp186->f5=({const char*_tmp187="Add to the list of directories for -l";
_tag_dynforward(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,38));});_tmp186;});
_tmp161[5]=({struct _tuple15*_tmp180=_region_malloc(r,sizeof(*_tmp180));_tmp180->f1=({
const char*_tmp185="-I";_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size(
_tmp185,3));});_tmp180->f2=1;_tmp180->f3=({const char*_tmp184="<dir>";
_tag_dynforward(_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,6));});_tmp180->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp182=_region_malloc(r,sizeof(*_tmp182));
_tmp182[0]=({struct Cyc_Arg_Flag_spec_struct _tmp183;_tmp183.tag=1;_tmp183.f1=Cyc_add_cpparg;
_tmp183;});_tmp182;});_tmp180->f5=({const char*_tmp181="Add to the list of directories to search for include files";
_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,59));});_tmp180;});
_tmp161[4]=({struct _tuple15*_tmp17A=_region_malloc(r,sizeof(*_tmp17A));_tmp17A->f1=({
const char*_tmp17F="-B";_tag_dynforward(_tmp17F,sizeof(char),_get_zero_arr_size(
_tmp17F,3));});_tmp17A->f2=1;_tmp17A->f3=({const char*_tmp17E="<file>";
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,7));});_tmp17A->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp17C=_region_malloc(r,sizeof(*_tmp17C));
_tmp17C[0]=({struct Cyc_Arg_Flag_spec_struct _tmp17D;_tmp17D.tag=1;_tmp17D.f1=Cyc_add_cyclone_exec_path;
_tmp17D;});_tmp17C;});_tmp17A->f5=({const char*_tmp17B="Add to the list of directories to search for compiler files";
_tag_dynforward(_tmp17B,sizeof(char),_get_zero_arr_size(_tmp17B,60));});_tmp17A;});
_tmp161[3]=({struct _tuple15*_tmp174=_region_malloc(r,sizeof(*_tmp174));_tmp174->f1=({
const char*_tmp179="-D";_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(
_tmp179,3));});_tmp174->f2=1;_tmp174->f3=({const char*_tmp178="<name>[=<value>]";
_tag_dynforward(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,17));});_tmp174->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp176=_region_malloc(r,sizeof(*_tmp176));
_tmp176[0]=({struct Cyc_Arg_Flag_spec_struct _tmp177;_tmp177.tag=1;_tmp177.f1=Cyc_add_cpparg;
_tmp177;});_tmp176;});_tmp174->f5=({const char*_tmp175="Pass definition to preprocessor";
_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,32));});_tmp174;});
_tmp161[2]=({struct _tuple15*_tmp16E=_region_malloc(r,sizeof(*_tmp16E));_tmp16E->f1=({
const char*_tmp173="-o";_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size(
_tmp173,3));});_tmp16E->f2=0;_tmp16E->f3=({const char*_tmp172=" <file>";
_tag_dynforward(_tmp172,sizeof(char),_get_zero_arr_size(_tmp172,8));});_tmp16E->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp170=_region_malloc(r,sizeof(*
_tmp170));_tmp170[0]=({struct Cyc_Arg_String_spec_struct _tmp171;_tmp171.tag=5;
_tmp171.f1=Cyc_set_output_file;_tmp171;});_tmp170;});_tmp16E->f5=({const char*
_tmp16F="Set the output file name to <file>";_tag_dynforward(_tmp16F,sizeof(char),
_get_zero_arr_size(_tmp16F,35));});_tmp16E;});_tmp161[1]=({struct _tuple15*
_tmp168=_region_malloc(r,sizeof(*_tmp168));_tmp168->f1=({const char*_tmp16D="--version";
_tag_dynforward(_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,10));});_tmp168->f2=
0;_tmp168->f3=({const char*_tmp16C="";_tag_dynforward(_tmp16C,sizeof(char),
_get_zero_arr_size(_tmp16C,1));});_tmp168->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp16A=_region_malloc(r,sizeof(*_tmp16A));_tmp16A[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp16B;_tmp16B.tag=0;_tmp16B.f1=Cyc_print_version;_tmp16B;});_tmp16A;});_tmp168->f5=({
const char*_tmp169="Print version information and exit";_tag_dynforward(_tmp169,
sizeof(char),_get_zero_arr_size(_tmp169,35));});_tmp168;});_tmp161[0]=({struct
_tuple15*_tmp162=_region_malloc(r,sizeof(*_tmp162));_tmp162->f1=({const char*
_tmp167="-v";_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,3));});
_tmp162->f2=0;_tmp162->f3=({const char*_tmp166="";_tag_dynforward(_tmp166,sizeof(
char),_get_zero_arr_size(_tmp166,1));});_tmp162->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp164=_region_malloc(r,sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Arg_Set_spec_struct
_tmp165;_tmp165.tag=3;_tmp165.f1=& Cyc_v_r;_tmp165;});_tmp164;});_tmp162->f5=({
const char*_tmp163="Print compilation stages verbosely";_tag_dynforward(_tmp163,
sizeof(char),_get_zero_arr_size(_tmp163,35));});_tmp162;});((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(r,_tag_dynforward(
_tmp161,sizeof(struct _tuple15*),62));});struct _dynforward_ptr optstring=({const
char*_tmp160="Options:";_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(
_tmp160,9));});Cyc_Arg_current=0;Cyc_Arg_parse(options,Cyc_add_other,optstring,
_tmp145);if(Cyc_target_arch.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)
Cyc_target_arch=(struct _dynforward_ptr)Cstring_to_string(Carch);{struct
_dynforward_ptr cyclone_exec_prefix=({char*_tmp15F=getenv((const char*)"CYCLONE_EXEC_PREFIX");
_tag_dynforward(_tmp15F,sizeof(char),_get_zero_arr_size(_tmp15F,1));});if(
cyclone_exec_prefix.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));
_tmp147->hd=({struct _dynforward_ptr*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[
0]=cyclone_exec_prefix;_tmp148;});_tmp147->tl=Cyc_cyclone_exec_path;_tmp147;});
def_lib_path=(struct _dynforward_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((
struct _dynforward_ptr)def_lib_path)> 0)Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=({struct _dynforward_ptr*_tmp14A=
_cycalloc(sizeof(*_tmp14A));_tmp14A[0]=(struct _dynforward_ptr)Cyc_Filename_concat(
def_lib_path,({const char*_tmp14B="cyc-lib";_tag_dynforward(_tmp14B,sizeof(char),
_get_zero_arr_size(_tmp14B,8));}));_tmp14A;});_tmp149->tl=Cyc_cyclone_exec_path;
_tmp149;});Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);{struct _dynforward_ptr _tmp14C=Cyc_do_find(cyclone_arch_path,({
const char*_tmp15E="cycspecs";_tag_dynforward(_tmp15E,sizeof(char),
_get_zero_arr_size(_tmp15E,9));}));if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp14C);{void*_tmp14D[1]={& _tmp14F};Cyc_fprintf(Cyc_stderr,({const char*_tmp14E="Reading from specs file %s\n";
_tag_dynforward(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,28));}),
_tag_dynforward(_tmp14D,sizeof(void*),1));}});{struct Cyc_List_List*_tmp150=Cyc_read_specs(
_tmp14C);struct _dynforward_ptr _tmp151=_tag_dynforward(0,0,0);{struct
_handler_cons _tmp152;_push_handler(& _tmp152);{int _tmp154=0;if(setjmp(_tmp152.handler))
_tmp154=1;if(!_tmp154){{struct _dynforward_ptr _tmp155=*((struct _dynforward_ptr*(*)(
int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l,
struct _dynforward_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp150,
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"cyclone",sizeof(
char),8));_tmp151=Cyc_split_specs(_tmp155);};_pop_handler();}else{void*_tmp153=(
void*)_exn_thrown;void*_tmp158=_tmp153;_LL36: if(_tmp158 != Cyc_Core_Not_found)
goto _LL38;_LL37: goto _LL35;_LL38:;_LL39:(void)_throw(_tmp158);_LL35:;}}}if(
_tmp151.curr != (_tag_dynforward(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp151);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp146);if(Cyc_strcmp((struct _dynforward_ptr)
Cyc_cpp,({const char*_tmp159="";_tag_dynforward(_tmp159,sizeof(char),
_get_zero_arr_size(_tmp159,1));}))== 0)Cyc_set_cpp((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp14C);{struct Cyc_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
comp);{void*_tmp15A[2]={& _tmp15C,& _tmp15D};Cyc_aprintf(({const char*_tmp15B="%s -x c -E -specs %s";
_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,21));}),
_tag_dynforward(_tmp15A,sizeof(void*),2));}}}));}}}}}};_pop_region(r);}if(Cyc_cyclone_files
== 0  && Cyc_ccargs == 0){({void*_tmp2E2[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2E3="missing file\n";_tag_dynforward(_tmp2E3,sizeof(char),_get_zero_arr_size(
_tmp2E3,14));}),_tag_dynforward(_tmp2E2,sizeof(void*),0));});exit(1);}if(!Cyc_stop_after_cpp_r
 && Cyc_strlen((struct _dynforward_ptr)Cyc_cyc_include)== 0)Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,({const char*_tmp2E4="cyc_include.h";_tag_dynforward(
_tmp2E4,sizeof(char),_get_zero_arr_size(_tmp2E4,14));}));{struct Cyc_List_List*
_tmp2E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);
for(0;_tmp2E5 != 0;_tmp2E5=_tmp2E5->tl){Cyc_process_file(*((struct _dynforward_ptr*)
_tmp2E5->hd));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp2E6="-L";_tag_dynforward(
_tmp2E6,sizeof(char),_get_zero_arr_size(_tmp2E6,3));}),(struct _dynforward_ptr)
def_lib_path));Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_ccargs);{struct _dynforward_ptr _tmp2E7=Cyc_str_sepstr(((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),({const char*_tmp33F=" ";_tag_dynforward(
_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _dynforward_ptr _tmp2E8=
Cyc_str_sepstr(({struct Cyc_List_List*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->hd=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp33C->tl=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct
_dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs);_tmp33C;}),({const char*_tmp33E=" ";_tag_dynforward(_tmp33E,sizeof(
char),_get_zero_arr_size(_tmp33E,2));}));struct Cyc_List_List*stdlib;struct
_dynforward_ptr stdlib_string;int _tmp2E9=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dynforward_ptr*)
_check_null(Cyc_output_file)),({const char*_tmp33A=".a";_tag_dynforward(_tmp33A,
sizeof(char),_get_zero_arr_size(_tmp33A,3));}))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp33B=".lib";
_tag_dynforward(_tmp33B,sizeof(char),_get_zero_arr_size(_tmp33B,5));}));if(
_tmp2E9){stdlib=0;stdlib_string=(struct _dynforward_ptr)({const char*_tmp2EA="";
_tag_dynforward(_tmp2EA,sizeof(char),_get_zero_arr_size(_tmp2EA,1));});}else{
struct _dynforward_ptr libcyc_flag;struct _dynforward_ptr nogc_filename;struct
_dynforward_ptr runtime_filename;struct _dynforward_ptr gc_filename;if(Cyc_pa_r){
libcyc_flag=({const char*_tmp2EB="-lcyc_a";_tag_dynforward(_tmp2EB,sizeof(char),
_get_zero_arr_size(_tmp2EB,8));});nogc_filename=({const char*_tmp2EC="nogc_a.a";
_tag_dynforward(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,9));});
runtime_filename=({const char*_tmp2ED="runtime_cyc_a.o";_tag_dynforward(_tmp2ED,
sizeof(char),_get_zero_arr_size(_tmp2ED,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp2EE="-lcyc_nocheck";_tag_dynforward(_tmp2EE,sizeof(
char),_get_zero_arr_size(_tmp2EE,14));});nogc_filename=({const char*_tmp2EF="nogc_nocheck.a";
_tag_dynforward(_tmp2EF,sizeof(char),_get_zero_arr_size(_tmp2EF,15));});
runtime_filename=({const char*_tmp2F0="runtime_cyc.o";_tag_dynforward(_tmp2F0,
sizeof(char),_get_zero_arr_size(_tmp2F0,14));});}else{if(Cyc_pg_r){libcyc_flag=({
const char*_tmp2F1="-lcyc_pg";_tag_dynforward(_tmp2F1,sizeof(char),
_get_zero_arr_size(_tmp2F1,9));});runtime_filename=({const char*_tmp2F2="runtime_cyc_pg.o";
_tag_dynforward(_tmp2F2,sizeof(char),_get_zero_arr_size(_tmp2F2,17));});
nogc_filename=({const char*_tmp2F3="nogc_pg.a";_tag_dynforward(_tmp2F3,sizeof(
char),_get_zero_arr_size(_tmp2F3,10));});}else{libcyc_flag=({const char*_tmp2F4="-lcyc";
_tag_dynforward(_tmp2F4,sizeof(char),_get_zero_arr_size(_tmp2F4,6));});
nogc_filename=({const char*_tmp2F5="nogc.a";_tag_dynforward(_tmp2F5,sizeof(char),
_get_zero_arr_size(_tmp2F5,7));});runtime_filename=({const char*_tmp2F6="runtime_cyc.o";
_tag_dynforward(_tmp2F6,sizeof(char),_get_zero_arr_size(_tmp2F6,14));});}}}
gc_filename=({const char*_tmp2F7="gc.a";_tag_dynforward(_tmp2F7,sizeof(char),
_get_zero_arr_size(_tmp2F7,5));});{struct _dynforward_ptr _tmp2F8=Cyc_nogc_r?Cyc_do_find(
cyclone_arch_path,nogc_filename): Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dynforward_ptr _tmp2F9=Cyc_do_find(cyclone_arch_path,runtime_filename);
stdlib=0;stdlib_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2FE;_tmp2FE.tag=0;_tmp2FE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2F8);{struct Cyc_String_pa_struct _tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)libcyc_flag);{struct Cyc_String_pa_struct
_tmp2FC;_tmp2FC.tag=0;_tmp2FC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2F9);{void*_tmp2FA[3]={& _tmp2FC,& _tmp2FD,& _tmp2FE};Cyc_aprintf(({const char*
_tmp2FB=" %s %s %s";_tag_dynforward(_tmp2FB,sizeof(char),_get_zero_arr_size(
_tmp2FB,10));}),_tag_dynforward(_tmp2FA,sizeof(void*),3));}}}});}}if(Cyc_ic_r){
struct _handler_cons _tmp2FF;_push_handler(& _tmp2FF);{int _tmp301=0;if(setjmp(
_tmp2FF.handler))_tmp301=1;if(!_tmp301){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*
f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dynforward_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp302=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp2E9)_tmp302=({
struct Cyc_List_List*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->hd=Cyc_final_strptr;
_tmp303->tl=_tmp302;_tmp303;});{struct Cyc_Interface_I*_tmp304=((struct Cyc_Interface_I*(*)(
struct Cyc_Interface_I*(*get)(struct _dynforward_ptr*),struct Cyc_List_List*la,
struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,
_tmp302,_tmp302);if(_tmp304 == 0){({void*_tmp305[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp306="Error: interfaces incompatible\n";_tag_dynforward(_tmp306,
sizeof(char),_get_zero_arr_size(_tmp306,32));}),_tag_dynforward(_tmp305,sizeof(
void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp307=1;
_npop_handler(0);return _tmp307;}}if(_tmp2E9){if(Cyc_output_file != 0){struct
_dynforward_ptr _tmp308=({struct Cyc_String_pa_struct _tmp30F;_tmp30F.tag=0;_tmp30F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Filename_chop_extension(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp30D[1]={&
_tmp30F};Cyc_aprintf(({const char*_tmp30E="%s.cycio";_tag_dynforward(_tmp30E,
sizeof(char),_get_zero_arr_size(_tmp30E,9));}),_tag_dynforward(_tmp30D,sizeof(
void*),1));}});struct Cyc___cycFILE*_tmp309=Cyc_try_file_open((struct
_dynforward_ptr)_tmp308,({const char*_tmp30B="wb";_tag_dynforward(_tmp30B,sizeof(
char),_get_zero_arr_size(_tmp30B,3));}),({const char*_tmp30C="interface object file";
_tag_dynforward(_tmp30C,sizeof(char),_get_zero_arr_size(_tmp30C,22));}));if(
_tmp309 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp30A=1;
_npop_handler(0);return _tmp30A;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_tmp304,(struct Cyc___cycFILE*)_tmp309);Cyc_file_close((struct Cyc___cycFILE*)
_tmp309);}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp304,({struct _tuple11*_tmp310=_cycalloc(sizeof(*_tmp310));
_tmp310->f1=({const char*_tmp312="empty interface";_tag_dynforward(_tmp312,
sizeof(char),_get_zero_arr_size(_tmp312,16));});_tmp310->f2=({const char*_tmp311="global interface";
_tag_dynforward(_tmp311,sizeof(char),_get_zero_arr_size(_tmp311,17));});_tmp310;}))){({
void*_tmp313[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp314="Error: some objects are still undefined\n";
_tag_dynforward(_tmp314,sizeof(char),_get_zero_arr_size(_tmp314,41));}),
_tag_dynforward(_tmp313,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp315=1;_npop_handler(0);return _tmp315;}}}}};_pop_handler();}else{void*
_tmp300=(void*)_exn_thrown;void*_tmp317=_tmp300;_LL3B:;_LL3C:{void*_tmp318=
_tmp317;struct _dynforward_ptr _tmp319;struct _dynforward_ptr _tmp31A;struct
_dynforward_ptr _tmp31B;_LL40: if(*((void**)_tmp318)!= Cyc_Core_Failure)goto _LL42;
_tmp319=((struct Cyc_Core_Failure_struct*)_tmp318)->f1;_LL41:({struct Cyc_String_pa_struct
_tmp31E;_tmp31E.tag=0;_tmp31E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp319);{void*_tmp31C[1]={& _tmp31E};Cyc_fprintf(Cyc_stderr,({const char*_tmp31D="Exception Core::Failure %s\n";
_tag_dynforward(_tmp31D,sizeof(char),_get_zero_arr_size(_tmp31D,28));}),
_tag_dynforward(_tmp31C,sizeof(void*),1));}});goto _LL3F;_LL42: if(*((void**)
_tmp318)!= Cyc_Core_Impossible)goto _LL44;_tmp31A=((struct Cyc_Core_Impossible_struct*)
_tmp318)->f1;_LL43:({struct Cyc_String_pa_struct _tmp321;_tmp321.tag=0;_tmp321.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp31A);{void*_tmp31F[1]={&
_tmp321};Cyc_fprintf(Cyc_stderr,({const char*_tmp320="Exception Core::Impossible %s\n";
_tag_dynforward(_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,31));}),
_tag_dynforward(_tmp31F,sizeof(void*),1));}});goto _LL3F;_LL44: if(_tmp318 != Cyc_Dict_Absent)
goto _LL46;_LL45:({void*_tmp322[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp323="Exception Dict::Absent\n";
_tag_dynforward(_tmp323,sizeof(char),_get_zero_arr_size(_tmp323,24));}),
_tag_dynforward(_tmp322,sizeof(void*),0));});goto _LL3F;_LL46: if(*((void**)
_tmp318)!= Cyc_Core_Invalid_argument)goto _LL48;_tmp31B=((struct Cyc_Core_Invalid_argument_struct*)
_tmp318)->f1;_LL47:({struct Cyc_String_pa_struct _tmp326;_tmp326.tag=0;_tmp326.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp31B);{void*_tmp324[1]={&
_tmp326};Cyc_fprintf(Cyc_stderr,({const char*_tmp325="Exception Core::Invalid_argument %s\n";
_tag_dynforward(_tmp325,sizeof(char),_get_zero_arr_size(_tmp325,37));}),
_tag_dynforward(_tmp324,sizeof(void*),1));}});goto _LL3F;_LL48:;_LL49:({void*
_tmp327[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp328="Uncaught exception\n";
_tag_dynforward(_tmp328,sizeof(char),_get_zero_arr_size(_tmp328,20));}),
_tag_dynforward(_tmp327,sizeof(void*),0));});goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL3D:;_LL3E:(void)_throw(_tmp317);_LL3A:;}}}{
struct _dynforward_ptr outfile_str=({const char*_tmp339="";_tag_dynforward(_tmp339,
sizeof(char),_get_zero_arr_size(_tmp339,1));});if(Cyc_output_file != 0)
outfile_str=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp32B;_tmp32B.tag=
0;_tmp32B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp329[1]={&
_tmp32B};Cyc_aprintf(({const char*_tmp32A=" -o %s";_tag_dynforward(_tmp32A,
sizeof(char),_get_zero_arr_size(_tmp32A,7));}),_tag_dynforward(_tmp329,sizeof(
void*),1));}});{struct _dynforward_ptr _tmp32C=({struct Cyc_String_pa_struct _tmp338;
_tmp338.tag=0;_tmp338.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp2E8);{
struct Cyc_String_pa_struct _tmp337;_tmp337.tag=0;_tmp337.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdlib_string);{struct Cyc_String_pa_struct
_tmp336;_tmp336.tag=0;_tmp336.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2E7);{struct Cyc_String_pa_struct _tmp335;_tmp335.tag=0;_tmp335.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)outfile_str);{struct Cyc_String_pa_struct
_tmp334;_tmp334.tag=0;_tmp334.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
comp);{void*_tmp332[5]={& _tmp334,& _tmp335,& _tmp336,& _tmp337,& _tmp338};Cyc_aprintf(({
const char*_tmp333="%s %s %s%s%s";_tag_dynforward(_tmp333,sizeof(char),
_get_zero_arr_size(_tmp333,13));}),_tag_dynforward(_tmp332,sizeof(void*),5));}}}}}});
if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp32F;_tmp32F.tag=0;_tmp32F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp32C);{void*_tmp32D[1]={& _tmp32F};Cyc_fprintf(
Cyc_stderr,({const char*_tmp32E="%s\n";_tag_dynforward(_tmp32E,sizeof(char),
_get_zero_arr_size(_tmp32E,4));}),_tag_dynforward(_tmp32D,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)
_untag_dynforward_ptr(_tmp32C,sizeof(char),1))!= 0){({void*_tmp330[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp331="Error: C compiler failed\n";_tag_dynforward(
_tmp331,sizeof(char),_get_zero_arr_size(_tmp331,26));}),_tag_dynforward(_tmp330,
sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();
return Cyc_compile_failure?1: 0;}}}}}}
