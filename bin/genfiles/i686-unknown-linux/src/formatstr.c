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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int isdigit(int);
unsigned int Cyc_strlen(struct _dynforward_ptr s);struct _dynforward_ptr Cyc_implode(
struct Cyc_List_List*c);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;
struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};extern char
Cyc_Position_Nocontext[14];typedef struct{int __count;union{unsigned int __wch;char
__wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t
__state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;struct
Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_Loc_n_struct{int tag;};struct
Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple0{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
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
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dynforward_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple3{void*f1;void*f2;};struct _tuple3*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,
int opened);int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);struct Cyc_Tcenv_CList{void*hd;
struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct
Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,
struct _dynforward_ptr,struct Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,struct Cyc_Position_Segment*);struct
Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _dynforward_ptr s,
int i);static struct Cyc_Core_Opt Cyc_Formatstr_rk={(void*)((void*)3)};struct _tuple4{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;char f5;int f6;};struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(
struct _dynforward_ptr s,int i){unsigned int _tmp0=Cyc_strlen((struct _dynforward_ptr)
s);if(i < 0  || i >= _tmp0)return 0;{struct Cyc_List_List*_tmp1=0;char c=' ';for(0;i < 
_tmp0;i ++){c=*((const char*)_check_dynforward_subscript(s,sizeof(char),i));
switch(c){case '+': _LL0: goto _LL1;case '-': _LL1: goto _LL2;case ' ': _LL2: goto _LL3;case
'#': _LL3: goto _LL4;case '0': _LL4: _tmp1=({struct Cyc_List_List*_tmp2=_cycalloc(
sizeof(*_tmp2));_tmp2->hd=(void*)((int)c);_tmp2->tl=_tmp1;_tmp2;});continue;
default: _LL5: break;}break;}if(i >= _tmp0)return 0;_tmp1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1);{struct Cyc_List_List*_tmp3=0;c=*((
const char*)_check_dynforward_subscript(s,sizeof(char),i));if(c == '*'){_tmp3=({
struct Cyc_List_List*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->hd=(void*)((int)c);
_tmp4->tl=_tmp3;_tmp4;});i ++;}else{for(0;i < _tmp0;i ++){c=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(isdigit((int)c))_tmp3=({struct
Cyc_List_List*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->hd=(void*)((int)c);_tmp5->tl=
_tmp3;_tmp5;});else{break;}}}if(i >= _tmp0)return 0;_tmp3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3);{struct Cyc_List_List*_tmp6=0;c=*((
const char*)_check_dynforward_subscript(s,sizeof(char),i));if(c == '.'){_tmp6=({
struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->hd=(void*)((int)c);
_tmp7->tl=_tmp6;_tmp7;});i ++;if(i >= _tmp0)return 0;c=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(c == '*'){_tmp6=({struct Cyc_List_List*
_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->hd=(void*)((int)c);_tmp8->tl=_tmp6;_tmp8;});
i ++;}else{for(0;i < _tmp0;i ++){c=*((const char*)_check_dynforward_subscript(s,
sizeof(char),i));if(isdigit((int)c))_tmp6=({struct Cyc_List_List*_tmp9=_cycalloc(
sizeof(*_tmp9));_tmp9->hd=(void*)((int)c);_tmp9->tl=_tmp6;_tmp9;});else{break;}}}}
if(i >= _tmp0)return 0;_tmp6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp6);{struct Cyc_List_List*_tmpA=0;c=*((const char*)_check_dynforward_subscript(
s,sizeof(char),i));switch(c){case 'h': _LL7: _tmpA=({struct Cyc_List_List*_tmpB=
_cycalloc(sizeof(*_tmpB));_tmpB->hd=(void*)((int)c);_tmpB->tl=_tmpA;_tmpB;});i ++;
if(i >= _tmp0)return 0;c=*((const char*)_check_dynforward_subscript(s,sizeof(char),
i));if(c == 'h'){_tmpA=({struct Cyc_List_List*_tmpC=_cycalloc(sizeof(*_tmpC));
_tmpC->hd=(void*)((int)c);_tmpC->tl=_tmpA;_tmpC;});i ++;}break;case 'l': _LL8: _tmpA=({
struct Cyc_List_List*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->hd=(void*)((int)c);
_tmpD->tl=_tmpA;_tmpD;});i ++;if(i >= _tmp0)return 0;c=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(c == 'l'){_tmpA=({struct Cyc_List_List*
_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=(void*)((int)c);_tmpE->tl=_tmpA;_tmpE;});
i ++;}break;case 'j': _LL9: goto _LLA;case 'z': _LLA: goto _LLB;case 't': _LLB: goto _LLC;
case 'L': _LLC: _tmpA=({struct Cyc_List_List*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->hd=(
void*)((int)c);_tmpF->tl=_tmpA;_tmpF;});i ++;break;default: _LLD: break;}if(i >= 
_tmp0)return 0;_tmpA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpA);c=*((const char*)_check_dynforward_subscript(s,sizeof(char),i));switch(c){
case 'd': _LLF: goto _LL10;case 'i': _LL10: goto _LL11;case 'o': _LL11: goto _LL12;case 'u':
_LL12: goto _LL13;case 'x': _LL13: goto _LL14;case 'X': _LL14: goto _LL15;case 'f': _LL15:
goto _LL16;case 'F': _LL16: goto _LL17;case 'e': _LL17: goto _LL18;case 'E': _LL18: goto
_LL19;case 'g': _LL19: goto _LL1A;case 'G': _LL1A: goto _LL1B;case 'a': _LL1B: goto _LL1C;
case 'A': _LL1C: goto _LL1D;case 'c': _LL1D: goto _LL1E;case 's': _LL1E: goto _LL1F;case 'p':
_LL1F: goto _LL20;case 'n': _LL20: goto _LL21;case '%': _LL21: break;default: _LL22: return
0;}return({struct Cyc_Core_Opt*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->v=({
struct _tuple4*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->f1=_tmp1;_tmp11->f2=
_tmp3;_tmp11->f3=_tmp6;_tmp11->f4=_tmpA;_tmp11->f5=c;_tmp11->f6=i + 1;_tmp11;});
_tmp10;});}}}}}struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*
te,struct _dynforward_ptr s,struct Cyc_Position_Segment*loc){unsigned int _tmp12=Cyc_strlen((
struct _dynforward_ptr)s);struct Cyc_List_List*_tmp13=0;int i;for(i=0;i < _tmp12;i ++){
if(*((const char*)_check_dynforward_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmp14=Cyc_Formatstr_parse_conversionspecification(s,i + 1);
if(_tmp14 == 0){({void*_tmp15[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp16="bad format string";
_tag_dynforward(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,18));}),
_tag_dynforward(_tmp15,sizeof(void*),0));});return 0;}{struct _tuple4 _tmp18;struct
Cyc_List_List*_tmp19;struct Cyc_List_List*_tmp1A;struct Cyc_List_List*_tmp1B;
struct Cyc_List_List*_tmp1C;char _tmp1D;int _tmp1E;struct _tuple4*_tmp17=(struct
_tuple4*)_tmp14->v;_tmp18=*_tmp17;_tmp19=_tmp18.f1;_tmp1A=_tmp18.f2;_tmp1B=
_tmp18.f3;_tmp1C=_tmp18.f4;_tmp1D=_tmp18.f5;_tmp1E=_tmp18.f6;i=_tmp1E - 1;{struct
Cyc_List_List*_tmp1F=_tmp1C;struct Cyc_List_List _tmp20;int _tmp21;struct Cyc_List_List*
_tmp22;_LL25: if(_tmp1F == 0)goto _LL27;_tmp20=*_tmp1F;_tmp21=(int)_tmp20.hd;_tmp22=
_tmp20.tl;if(_tmp22 != 0)goto _LL27;if(!((_tmp21 == 'j'  || _tmp21 == 'z') || _tmp21
== 't'))goto _LL27;_LL26:({struct Cyc_Int_pa_struct _tmp25;_tmp25.tag=1;_tmp25.f1=(
unsigned long)_tmp21;{void*_tmp23[1]={& _tmp25};Cyc_Tcutil_terr(loc,({const char*
_tmp24="length modifier '%c' is not supported";_tag_dynforward(_tmp24,sizeof(
char),_get_zero_arr_size(_tmp24,38));}),_tag_dynforward(_tmp23,sizeof(void*),1));}});
return 0;_LL27:;_LL28: goto _LL24;_LL24:;}{struct Cyc_List_List*_tmp26=_tmp1A;struct
Cyc_List_List _tmp27;int _tmp28;struct Cyc_List_List*_tmp29;_LL2A: if(_tmp26 == 0)
goto _LL2C;_tmp27=*_tmp26;_tmp28=(int)_tmp27.hd;_tmp29=_tmp27.tl;if(_tmp29 != 0)
goto _LL2C;if(!(_tmp28 == '*'))goto _LL2C;_LL2B: _tmp13=({struct Cyc_List_List*_tmp2A=
_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=(void*)Cyc_Absyn_sint_typ;_tmp2A->tl=
_tmp13;_tmp2A;});goto _LL29;_LL2C:;_LL2D: goto _LL29;_LL29:;}{struct Cyc_List_List*
_tmp2B=_tmp1B;struct Cyc_List_List _tmp2C;int _tmp2D;struct Cyc_List_List*_tmp2E;
struct Cyc_List_List _tmp2F;int _tmp30;struct Cyc_List_List*_tmp31;_LL2F: if(_tmp2B == 
0)goto _LL31;_tmp2C=*_tmp2B;_tmp2D=(int)_tmp2C.hd;_tmp2E=_tmp2C.tl;if(_tmp2E == 0)
goto _LL31;_tmp2F=*_tmp2E;_tmp30=(int)_tmp2F.hd;_tmp31=_tmp2F.tl;if(_tmp31 != 0)
goto _LL31;if(!(_tmp2D == '.'  && _tmp30 == '*'))goto _LL31;_LL30: _tmp13=({struct Cyc_List_List*
_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->hd=(void*)Cyc_Absyn_sint_typ;_tmp32->tl=
_tmp13;_tmp32;});goto _LL2E;_LL31:;_LL32: goto _LL2E;_LL2E:;}{void*t;switch(_tmp1D){
case 'd': _LL33: goto _LL34;case 'i': _LL34:{struct Cyc_List_List*f=_tmp19;for(0;f != 0;
f=f->tl){if((int)f->hd == '#'){({struct Cyc_Int_pa_struct _tmp35;_tmp35.tag=1;
_tmp35.f1=(unsigned long)((int)_tmp1D);{void*_tmp33[1]={& _tmp35};Cyc_Tcutil_terr(
loc,({const char*_tmp34="flag '#' is not valid with %%%c";_tag_dynforward(_tmp34,
sizeof(char),_get_zero_arr_size(_tmp34,32));}),_tag_dynforward(_tmp33,sizeof(
void*),1));}});return 0;}}}{struct Cyc_List_List*_tmp36=_tmp1C;struct Cyc_List_List
_tmp37;int _tmp38;struct Cyc_List_List*_tmp39;struct Cyc_List_List _tmp3A;int _tmp3B;
struct Cyc_List_List*_tmp3C;struct Cyc_List_List _tmp3D;int _tmp3E;struct Cyc_List_List*
_tmp3F;struct Cyc_List_List _tmp40;int _tmp41;struct Cyc_List_List*_tmp42;_LL37: if(
_tmp36 != 0)goto _LL39;_LL38: t=Cyc_Absyn_sint_typ;goto _LL36;_LL39: if(_tmp36 == 0)
goto _LL3B;_tmp37=*_tmp36;_tmp38=(int)_tmp37.hd;_tmp39=_tmp37.tl;if(_tmp39 != 0)
goto _LL3B;if(!(_tmp38 == 'l'))goto _LL3B;_LL3A: t=Cyc_Absyn_slong_typ;goto _LL36;
_LL3B: if(_tmp36 == 0)goto _LL3D;_tmp3A=*_tmp36;_tmp3B=(int)_tmp3A.hd;_tmp3C=_tmp3A.tl;
if(_tmp3C != 0)goto _LL3D;if(!(_tmp3B == 'h'))goto _LL3D;_LL3C: t=Cyc_Absyn_sshort_typ;
goto _LL36;_LL3D: if(_tmp36 == 0)goto _LL3F;_tmp3D=*_tmp36;_tmp3E=(int)_tmp3D.hd;
_tmp3F=_tmp3D.tl;if(_tmp3F == 0)goto _LL3F;_tmp40=*_tmp3F;_tmp41=(int)_tmp40.hd;
_tmp42=_tmp40.tl;if(_tmp42 != 0)goto _LL3F;if(!(_tmp3E == 'h'  && _tmp41 == 'h'))goto
_LL3F;_LL3E: t=Cyc_Absyn_schar_typ;goto _LL36;_LL3F:;_LL40:({struct Cyc_Int_pa_struct
_tmp46;_tmp46.tag=1;_tmp46.f1=(unsigned long)((int)_tmp1D);{struct Cyc_String_pa_struct
_tmp45;_tmp45.tag=0;_tmp45.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(
_tmp1C));{void*_tmp43[2]={& _tmp45,& _tmp46};Cyc_Tcutil_terr(loc,({const char*
_tmp44="length modifier '%s' is not allowed with %%%c";_tag_dynforward(_tmp44,
sizeof(char),_get_zero_arr_size(_tmp44,46));}),_tag_dynforward(_tmp43,sizeof(
void*),2));}}});return 0;_LL36:;}_tmp13=({struct Cyc_List_List*_tmp47=_cycalloc(
sizeof(*_tmp47));_tmp47->hd=(void*)t;_tmp47->tl=_tmp13;_tmp47;});break;case 'u':
_LL35:{struct Cyc_List_List*f=_tmp19;for(0;f != 0;f=f->tl){if((int)f->hd == '#'){({
void*_tmp48[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp49="Flag '#' not valid with %%u";
_tag_dynforward(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,28));}),
_tag_dynforward(_tmp48,sizeof(void*),0));});return 0;}}}goto _LL41;case 'o': _LL41:
goto _LL42;case 'x': _LL42: goto _LL43;case 'X': _LL43:{struct Cyc_List_List*_tmp4A=
_tmp1C;struct Cyc_List_List _tmp4B;int _tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_List_List
_tmp4E;int _tmp4F;struct Cyc_List_List*_tmp50;struct Cyc_List_List _tmp51;int _tmp52;
struct Cyc_List_List*_tmp53;struct Cyc_List_List _tmp54;int _tmp55;struct Cyc_List_List*
_tmp56;_LL46: if(_tmp4A != 0)goto _LL48;_LL47: t=Cyc_Absyn_uint_typ;goto _LL45;_LL48:
if(_tmp4A == 0)goto _LL4A;_tmp4B=*_tmp4A;_tmp4C=(int)_tmp4B.hd;_tmp4D=_tmp4B.tl;
if(_tmp4D != 0)goto _LL4A;if(!(_tmp4C == 'l'))goto _LL4A;_LL49: t=Cyc_Absyn_ulong_typ;
goto _LL45;_LL4A: if(_tmp4A == 0)goto _LL4C;_tmp4E=*_tmp4A;_tmp4F=(int)_tmp4E.hd;
_tmp50=_tmp4E.tl;if(_tmp50 != 0)goto _LL4C;if(!(_tmp4F == 'h'))goto _LL4C;_LL4B: t=
Cyc_Absyn_ushort_typ;goto _LL45;_LL4C: if(_tmp4A == 0)goto _LL4E;_tmp51=*_tmp4A;
_tmp52=(int)_tmp51.hd;_tmp53=_tmp51.tl;if(_tmp53 == 0)goto _LL4E;_tmp54=*_tmp53;
_tmp55=(int)_tmp54.hd;_tmp56=_tmp54.tl;if(_tmp56 != 0)goto _LL4E;if(!(_tmp52 == 'h'
 && _tmp55 == 'h'))goto _LL4E;_LL4D: t=Cyc_Absyn_uchar_typ;goto _LL45;_LL4E:;_LL4F:({
struct Cyc_Int_pa_struct _tmp5A;_tmp5A.tag=1;_tmp5A.f1=(unsigned long)((int)_tmp1D);{
struct Cyc_String_pa_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_implode(_tmp1C));{void*_tmp57[2]={& _tmp59,& _tmp5A};Cyc_Tcutil_terr(
loc,({const char*_tmp58="length modifier '%s' is not allowed with %%%c";
_tag_dynforward(_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,46));}),
_tag_dynforward(_tmp57,sizeof(void*),2));}}});return 0;_LL45:;}_tmp13=({struct Cyc_List_List*
_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->hd=(void*)t;_tmp5B->tl=_tmp13;_tmp5B;});
break;case 'f': _LL44: goto _LL50;case 'F': _LL50: goto _LL51;case 'e': _LL51: goto _LL52;
case 'E': _LL52: goto _LL53;case 'g': _LL53: goto _LL54;case 'G': _LL54: goto _LL55;case 'a':
_LL55: goto _LL56;case 'A': _LL56:{struct Cyc_List_List*_tmp5C=_tmp1C;struct Cyc_List_List
_tmp5D;int _tmp5E;struct Cyc_List_List*_tmp5F;_LL59: if(_tmp5C != 0)goto _LL5B;_LL5A:
_tmp13=({struct Cyc_List_List*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->hd=(void*)
Cyc_Absyn_double_typ(0);_tmp60->tl=_tmp13;_tmp60;});goto _LL58;_LL5B: if(_tmp5C == 
0)goto _LL5D;_tmp5D=*_tmp5C;_tmp5E=(int)_tmp5D.hd;_tmp5F=_tmp5D.tl;if(_tmp5F != 0)
goto _LL5D;if(!(_tmp5E == 'l'))goto _LL5D;_LL5C: _tmp13=({struct Cyc_List_List*_tmp61=
_cycalloc(sizeof(*_tmp61));_tmp61->hd=(void*)Cyc_Absyn_double_typ(1);_tmp61->tl=
_tmp13;_tmp61;});goto _LL58;_LL5D:;_LL5E:({struct Cyc_Int_pa_struct _tmp65;_tmp65.tag=
1;_tmp65.f1=(unsigned long)((int)_tmp1D);{struct Cyc_String_pa_struct _tmp64;
_tmp64.tag=0;_tmp64.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(
_tmp1C));{void*_tmp62[2]={& _tmp64,& _tmp65};Cyc_Tcutil_terr(loc,({const char*
_tmp63="length modifier '%s' is not allowed with %%%c";_tag_dynforward(_tmp63,
sizeof(char),_get_zero_arr_size(_tmp63,46));}),_tag_dynforward(_tmp62,sizeof(
void*),2));}}});return 0;_LL58:;}break;case 'c': _LL57:{struct Cyc_List_List*f=
_tmp19;for(0;f != 0;f=f->tl){if((int)f->hd == '#'  || (int)f->hd == '0'){({struct Cyc_Int_pa_struct
_tmp68;_tmp68.tag=1;_tmp68.f1=(unsigned long)((int)f->hd);{void*_tmp66[1]={&
_tmp68};Cyc_Tcutil_terr(loc,({const char*_tmp67="flag '%c' not allowed with %%c";
_tag_dynforward(_tmp67,sizeof(char),_get_zero_arr_size(_tmp67,31));}),
_tag_dynforward(_tmp66,sizeof(void*),1));}});return 0;}}}if(_tmp1C != 0){({struct
Cyc_String_pa_struct _tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_implode(_tmp1C));{void*_tmp69[1]={& _tmp6B};Cyc_Tcutil_terr(
loc,({const char*_tmp6A="length modifier '%s' not allowed with %%c";
_tag_dynforward(_tmp6A,sizeof(char),_get_zero_arr_size(_tmp6A,42));}),
_tag_dynforward(_tmp69,sizeof(void*),1));}});return 0;}if(_tmp1B != 0){({struct Cyc_String_pa_struct
_tmp6E;_tmp6E.tag=0;_tmp6E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(
_tmp1B));{void*_tmp6C[1]={& _tmp6E};Cyc_Tcutil_terr(loc,({const char*_tmp6D="precision '%s' not allowed with %%c";
_tag_dynforward(_tmp6D,sizeof(char),_get_zero_arr_size(_tmp6D,36));}),
_tag_dynforward(_tmp6C,sizeof(void*),1));}});return 0;}_tmp13=({struct Cyc_List_List*
_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->hd=(void*)Cyc_Absyn_sint_typ;_tmp6F->tl=
_tmp13;_tmp6F;});break;case 's': _LL5F:{struct Cyc_List_List*f=_tmp19;for(0;f != 0;f=
f->tl){if((int)f->hd != '-'){({void*_tmp70[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp71="a flag not allowed with %%s";_tag_dynforward(_tmp71,sizeof(char),
_get_zero_arr_size(_tmp71,28));}),_tag_dynforward(_tmp70,sizeof(void*),0));});
return 0;}}}if(_tmp1C != 0){({void*_tmp72[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp73="length modifiers not allowed with %%s";_tag_dynforward(_tmp73,sizeof(
char),_get_zero_arr_size(_tmp73,38));}),_tag_dynforward(_tmp72,sizeof(void*),0));});
return 0;}{void*ptr=Cyc_Absyn_dynforward_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({struct Cyc_Core_Opt*_tmp75=_cycalloc(
sizeof(*_tmp75));_tmp75->v=Cyc_Tcenv_lookup_type_vars(te);_tmp75;})),Cyc_Absyn_const_tqual(
0),Cyc_Absyn_false_conref);_tmp13=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*
_tmp74));_tmp74->hd=(void*)ptr;_tmp74->tl=_tmp13;_tmp74;});break;}case 'p': _LL60:
_tmp13=({struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->hd=(void*)
Cyc_Absyn_uint_typ;_tmp76->tl=_tmp13;_tmp76;});break;case 'n': _LL61:{struct Cyc_List_List*
f=_tmp19;for(0;f != 0;f=f->tl){if((int)f->hd == '#'  || (int)f->hd == '0'){({struct
Cyc_Int_pa_struct _tmp79;_tmp79.tag=1;_tmp79.f1=(unsigned long)((int)f->hd);{void*
_tmp77[1]={& _tmp79};Cyc_Tcutil_terr(loc,({const char*_tmp78="flag '%c' not allowed with %%n";
_tag_dynforward(_tmp78,sizeof(char),_get_zero_arr_size(_tmp78,31));}),
_tag_dynforward(_tmp77,sizeof(void*),1));}});return 0;}}}if(_tmp1B != 0){({struct
Cyc_String_pa_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_implode(_tmp1B));{void*_tmp7A[1]={& _tmp7C};Cyc_Tcutil_terr(
loc,({const char*_tmp7B="precision '%s' not allowed with %%n";_tag_dynforward(
_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,36));}),_tag_dynforward(_tmp7A,
sizeof(void*),1));}});return 0;}{struct Cyc_Core_Opt*_tmp7D=({struct Cyc_Core_Opt*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp90;});{struct Cyc_List_List*_tmp7E=_tmp1C;struct Cyc_List_List _tmp7F;int _tmp80;
struct Cyc_List_List*_tmp81;struct Cyc_List_List _tmp82;int _tmp83;struct Cyc_List_List*
_tmp84;struct Cyc_List_List _tmp85;int _tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List
_tmp88;int _tmp89;struct Cyc_List_List*_tmp8A;_LL64: if(_tmp7E != 0)goto _LL66;_LL65:
t=Cyc_Absyn_sint_typ;goto _LL63;_LL66: if(_tmp7E == 0)goto _LL68;_tmp7F=*_tmp7E;
_tmp80=(int)_tmp7F.hd;_tmp81=_tmp7F.tl;if(_tmp81 != 0)goto _LL68;if(!(_tmp80 == 'l'))
goto _LL68;_LL67: t=Cyc_Absyn_ulong_typ;goto _LL63;_LL68: if(_tmp7E == 0)goto _LL6A;
_tmp82=*_tmp7E;_tmp83=(int)_tmp82.hd;_tmp84=_tmp82.tl;if(_tmp84 != 0)goto _LL6A;
if(!(_tmp83 == 'h'))goto _LL6A;_LL69: t=Cyc_Absyn_sshort_typ;goto _LL63;_LL6A: if(
_tmp7E == 0)goto _LL6C;_tmp85=*_tmp7E;_tmp86=(int)_tmp85.hd;_tmp87=_tmp85.tl;if(
_tmp87 == 0)goto _LL6C;_tmp88=*_tmp87;_tmp89=(int)_tmp88.hd;_tmp8A=_tmp88.tl;if(
_tmp8A != 0)goto _LL6C;if(!(_tmp86 == 'h'  && _tmp89 == 'h'))goto _LL6C;_LL6B: t=Cyc_Absyn_schar_typ;
goto _LL63;_LL6C:;_LL6D:({struct Cyc_Int_pa_struct _tmp8E;_tmp8E.tag=1;_tmp8E.f1=(
unsigned long)((int)_tmp1D);{struct Cyc_String_pa_struct _tmp8D;_tmp8D.tag=0;
_tmp8D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(_tmp1C));{
void*_tmp8B[2]={& _tmp8D,& _tmp8E};Cyc_Tcutil_terr(loc,({const char*_tmp8C="length modifier '%s' is not allowed with %%%c";
_tag_dynforward(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,46));}),
_tag_dynforward(_tmp8B,sizeof(void*),2));}}});return 0;_LL63:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp7D),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmp13=({struct Cyc_List_List*_tmp8F=_cycalloc(sizeof(*
_tmp8F));_tmp8F->hd=(void*)t;_tmp8F->tl=_tmp13;_tmp8F;});break;}case '%': _LL62:
if(_tmp19 != 0){({struct Cyc_String_pa_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(_tmp19));{void*_tmp91[1]={&
_tmp93};Cyc_Tcutil_terr(loc,({const char*_tmp92="flags '%s' not allowed with %%%%";
_tag_dynforward(_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,33));}),
_tag_dynforward(_tmp91,sizeof(void*),1));}});return 0;}if(_tmp1A != 0){({struct Cyc_String_pa_struct
_tmp96;_tmp96.tag=0;_tmp96.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(
_tmp1A));{void*_tmp94[1]={& _tmp96};Cyc_Tcutil_terr(loc,({const char*_tmp95="width '%s' not allowed with %%%%";
_tag_dynforward(_tmp95,sizeof(char),_get_zero_arr_size(_tmp95,33));}),
_tag_dynforward(_tmp94,sizeof(void*),1));}});return 0;}if(_tmp1B != 0){({struct Cyc_String_pa_struct
_tmp99;_tmp99.tag=0;_tmp99.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(
_tmp1B));{void*_tmp97[1]={& _tmp99};Cyc_Tcutil_terr(loc,({const char*_tmp98="precision '%s' not allowed with %%%%";
_tag_dynforward(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,37));}),
_tag_dynforward(_tmp97,sizeof(void*),1));}});return 0;}if(_tmp1C != 0){({struct Cyc_String_pa_struct
_tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(
_tmp1C));{void*_tmp9A[1]={& _tmp9C};Cyc_Tcutil_terr(loc,({const char*_tmp9B="length modifier '%s' not allowed with %%%%";
_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,43));}),
_tag_dynforward(_tmp9A,sizeof(void*),1));}});return 0;}break;default: _LL6E: return
0;}}}}}return Cyc_List_imp_rev(_tmp13);}struct _tuple5{int f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;char f4;int f5;};struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(
struct _dynforward_ptr s,int i){unsigned int _tmp9D=Cyc_strlen((struct
_dynforward_ptr)s);if(i < 0  || i >= _tmp9D)return 0;{int _tmp9E=0;char _tmp9F=*((
const char*)_check_dynforward_subscript(s,sizeof(char),i));if(_tmp9F == '*'){
_tmp9E=1;i ++;if(i >= _tmp9D)return 0;}{struct Cyc_List_List*_tmpA0=0;for(0;i < 
_tmp9D;i ++){_tmp9F=*((const char*)_check_dynforward_subscript(s,sizeof(char),i));
if(isdigit((int)_tmp9F))_tmpA0=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*
_tmpA1));_tmpA1->hd=(void*)((int)_tmp9F);_tmpA1->tl=_tmpA0;_tmpA1;});else{break;}}
if(i >= _tmp9D)return 0;_tmpA0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpA0);{struct Cyc_List_List*_tmpA2=0;_tmp9F=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));switch(_tmp9F){case 'h': _LL70:
_tmpA2=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=(void*)((
int)_tmp9F);_tmpA3->tl=_tmpA2;_tmpA3;});i ++;if(i >= _tmp9D)return 0;_tmp9F=*((
const char*)_check_dynforward_subscript(s,sizeof(char),i));if(_tmp9F == 'h'){
_tmpA2=({struct Cyc_List_List*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->hd=(void*)((
int)_tmp9F);_tmpA4->tl=_tmpA2;_tmpA4;});i ++;}break;case 'l': _LL71: _tmpA2=({struct
Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=(void*)((int)_tmp9F);
_tmpA5->tl=_tmpA2;_tmpA5;});i ++;if(i >= _tmp9D)return 0;_tmp9F=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(_tmp9F == 'l'){_tmpA2=({struct
Cyc_List_List*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->hd=(void*)((int)_tmp9F);
_tmpA6->tl=_tmpA2;_tmpA6;});i ++;}break;case 'j': _LL72: goto _LL73;case 'z': _LL73:
goto _LL74;case 't': _LL74: goto _LL75;case 'L': _LL75: _tmpA2=({struct Cyc_List_List*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->hd=(void*)((int)_tmp9F);_tmpA7->tl=
_tmpA2;_tmpA7;});i ++;break;default: _LL76: break;}if(i >= _tmp9D)return 0;_tmpA2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA2);_tmp9F=*((
const char*)_check_dynforward_subscript(s,sizeof(char),i));switch(_tmp9F){case 'd':
_LL78: goto _LL79;case 'i': _LL79: goto _LL7A;case 'o': _LL7A: goto _LL7B;case 'u': _LL7B:
goto _LL7C;case 'x': _LL7C: goto _LL7D;case 'X': _LL7D: goto _LL7E;case 'f': _LL7E: goto
_LL7F;case 'F': _LL7F: goto _LL80;case 'e': _LL80: goto _LL81;case 'E': _LL81: goto _LL82;
case 'g': _LL82: goto _LL83;case 'G': _LL83: goto _LL84;case 'a': _LL84: goto _LL85;case 'A':
_LL85: goto _LL86;case 'c': _LL86: goto _LL87;case 's': _LL87: goto _LL88;case 'p': _LL88:
goto _LL89;case 'n': _LL89: goto _LL8A;case '%': _LL8A: break;default: _LL8B: return 0;}
return({struct Cyc_Core_Opt*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->v=({struct
_tuple5*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->f1=_tmp9E;_tmpA9->f2=_tmpA0;
_tmpA9->f3=_tmpA2;_tmpA9->f4=_tmp9F;_tmpA9->f5=i + 1;_tmpA9;});_tmpA8;});}}}}
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*te,struct
_dynforward_ptr s,struct Cyc_Position_Segment*loc){unsigned int _tmpAA=Cyc_strlen((
struct _dynforward_ptr)s);struct Cyc_List_List*_tmpAB=0;int i;for(i=0;i < _tmpAA;i ++){
if(*((const char*)_check_dynforward_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmpAC=Cyc_Formatstr_parse_inputformat(s,i + 1);if(_tmpAC == 0){({
void*_tmpAD[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpAE="bad format string";
_tag_dynforward(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,18));}),
_tag_dynforward(_tmpAD,sizeof(void*),0));});return 0;}{struct _tuple5 _tmpB0;int
_tmpB1;struct Cyc_List_List*_tmpB2;struct Cyc_List_List*_tmpB3;char _tmpB4;int
_tmpB5;struct _tuple5*_tmpAF=(struct _tuple5*)_tmpAC->v;_tmpB0=*_tmpAF;_tmpB1=
_tmpB0.f1;_tmpB2=_tmpB0.f2;_tmpB3=_tmpB0.f3;_tmpB4=_tmpB0.f4;_tmpB5=_tmpB0.f5;i=
_tmpB5 - 1;{struct Cyc_List_List*_tmpB6=_tmpB3;struct Cyc_List_List _tmpB7;int _tmpB8;
struct Cyc_List_List*_tmpB9;_LL8E: if(_tmpB6 == 0)goto _LL90;_tmpB7=*_tmpB6;_tmpB8=(
int)_tmpB7.hd;_tmpB9=_tmpB7.tl;if(_tmpB9 != 0)goto _LL90;if(!((_tmpB8 == 'j'  || 
_tmpB8 == 'z') || _tmpB8 == 't'))goto _LL90;_LL8F:({struct Cyc_Int_pa_struct _tmpBC;
_tmpBC.tag=1;_tmpBC.f1=(unsigned long)_tmpB8;{void*_tmpBA[1]={& _tmpBC};Cyc_Tcutil_terr(
loc,({const char*_tmpBB="length modifier '%c' is not supported";_tag_dynforward(
_tmpBB,sizeof(char),_get_zero_arr_size(_tmpBB,38));}),_tag_dynforward(_tmpBA,
sizeof(void*),1));}});return 0;_LL90:;_LL91: goto _LL8D;_LL8D:;}if(_tmpB1)continue;{
void*t;switch(_tmpB4){case 'd': _LL92: goto _LL93;case 'i': _LL93: {struct Cyc_Core_Opt*
_tmpBD=({struct Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpD0;});{struct Cyc_List_List*_tmpBE=_tmpB3;struct Cyc_List_List _tmpBF;int
_tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_List_List _tmpC2;int _tmpC3;struct Cyc_List_List*
_tmpC4;struct Cyc_List_List _tmpC5;int _tmpC6;struct Cyc_List_List*_tmpC7;struct Cyc_List_List
_tmpC8;int _tmpC9;struct Cyc_List_List*_tmpCA;_LL96: if(_tmpBE != 0)goto _LL98;_LL97:
t=Cyc_Absyn_sint_typ;goto _LL95;_LL98: if(_tmpBE == 0)goto _LL9A;_tmpBF=*_tmpBE;
_tmpC0=(int)_tmpBF.hd;_tmpC1=_tmpBF.tl;if(_tmpC1 != 0)goto _LL9A;if(!(_tmpC0 == 'l'))
goto _LL9A;_LL99: t=Cyc_Absyn_slong_typ;goto _LL95;_LL9A: if(_tmpBE == 0)goto _LL9C;
_tmpC2=*_tmpBE;_tmpC3=(int)_tmpC2.hd;_tmpC4=_tmpC2.tl;if(_tmpC4 != 0)goto _LL9C;
if(!(_tmpC3 == 'h'))goto _LL9C;_LL9B: t=Cyc_Absyn_sshort_typ;goto _LL95;_LL9C: if(
_tmpBE == 0)goto _LL9E;_tmpC5=*_tmpBE;_tmpC6=(int)_tmpC5.hd;_tmpC7=_tmpC5.tl;if(
_tmpC7 == 0)goto _LL9E;_tmpC8=*_tmpC7;_tmpC9=(int)_tmpC8.hd;_tmpCA=_tmpC8.tl;if(
_tmpCA != 0)goto _LL9E;if(!(_tmpC6 == 'h'  && _tmpC9 == 'h'))goto _LL9E;_LL9D: t=Cyc_Absyn_schar_typ;
goto _LL95;_LL9E:;_LL9F:({struct Cyc_Int_pa_struct _tmpCE;_tmpCE.tag=1;_tmpCE.f1=(
unsigned long)((int)_tmpB4);{struct Cyc_String_pa_struct _tmpCD;_tmpCD.tag=0;
_tmpCD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(_tmpB3));{
void*_tmpCB[2]={& _tmpCD,& _tmpCE};Cyc_Tcutil_terr(loc,({const char*_tmpCC="length modifier '%s' is not allowed with %%%c";
_tag_dynforward(_tmpCC,sizeof(char),_get_zero_arr_size(_tmpCC,46));}),
_tag_dynforward(_tmpCB,sizeof(void*),2));}}});return 0;_LL95:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpBD),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmpAB=({struct Cyc_List_List*_tmpCF=_cycalloc(sizeof(*
_tmpCF));_tmpCF->hd=(void*)t;_tmpCF->tl=_tmpAB;_tmpCF;});break;}case 'u': _LL94:
goto _LLA0;case 'o': _LLA0: goto _LLA1;case 'x': _LLA1: goto _LLA2;case 'X': _LLA2: {struct
Cyc_Core_Opt*_tmpD1=({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));
_tmpE4->v=Cyc_Tcenv_lookup_type_vars(te);_tmpE4;});{struct Cyc_List_List*_tmpD2=
_tmpB3;struct Cyc_List_List _tmpD3;int _tmpD4;struct Cyc_List_List*_tmpD5;struct Cyc_List_List
_tmpD6;int _tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_List_List _tmpD9;int _tmpDA;
struct Cyc_List_List*_tmpDB;struct Cyc_List_List _tmpDC;int _tmpDD;struct Cyc_List_List*
_tmpDE;_LLA5: if(_tmpD2 != 0)goto _LLA7;_LLA6: t=Cyc_Absyn_uint_typ;goto _LLA4;_LLA7:
if(_tmpD2 == 0)goto _LLA9;_tmpD3=*_tmpD2;_tmpD4=(int)_tmpD3.hd;_tmpD5=_tmpD3.tl;
if(_tmpD5 != 0)goto _LLA9;if(!(_tmpD4 == 'l'))goto _LLA9;_LLA8: t=Cyc_Absyn_ulong_typ;
goto _LLA4;_LLA9: if(_tmpD2 == 0)goto _LLAB;_tmpD6=*_tmpD2;_tmpD7=(int)_tmpD6.hd;
_tmpD8=_tmpD6.tl;if(_tmpD8 != 0)goto _LLAB;if(!(_tmpD7 == 'h'))goto _LLAB;_LLAA: t=
Cyc_Absyn_ushort_typ;goto _LLA4;_LLAB: if(_tmpD2 == 0)goto _LLAD;_tmpD9=*_tmpD2;
_tmpDA=(int)_tmpD9.hd;_tmpDB=_tmpD9.tl;if(_tmpDB == 0)goto _LLAD;_tmpDC=*_tmpDB;
_tmpDD=(int)_tmpDC.hd;_tmpDE=_tmpDC.tl;if(_tmpDE != 0)goto _LLAD;if(!(_tmpDA == 'h'
 && _tmpDD == 'h'))goto _LLAD;_LLAC: t=Cyc_Absyn_uchar_typ;goto _LLA4;_LLAD:;_LLAE:({
struct Cyc_Int_pa_struct _tmpE2;_tmpE2.tag=1;_tmpE2.f1=(unsigned long)((int)_tmpB4);{
struct Cyc_String_pa_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_implode(_tmpB3));{void*_tmpDF[2]={& _tmpE1,& _tmpE2};Cyc_Tcutil_terr(
loc,({const char*_tmpE0="length modifier '%s' is not allowed with %%%c";
_tag_dynforward(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,46));}),
_tag_dynforward(_tmpDF,sizeof(void*),2));}}});return 0;_LLA4:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpD1),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmpAB=({struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(*
_tmpE3));_tmpE3->hd=(void*)t;_tmpE3->tl=_tmpAB;_tmpE3;});break;}case 'f': _LLA3:
goto _LLAF;case 'F': _LLAF: goto _LLB0;case 'e': _LLB0: goto _LLB1;case 'E': _LLB1: goto
_LLB2;case 'g': _LLB2: goto _LLB3;case 'G': _LLB3: goto _LLB4;case 'a': _LLB4: goto _LLB5;
case 'A': _LLB5: {struct Cyc_Core_Opt*_tmpE5=({struct Cyc_Core_Opt*_tmpEF=_cycalloc(
sizeof(*_tmpEF));_tmpEF->v=Cyc_Tcenv_lookup_type_vars(te);_tmpEF;});{struct Cyc_List_List*
_tmpE6=_tmpB3;struct Cyc_List_List _tmpE7;int _tmpE8;struct Cyc_List_List*_tmpE9;
_LLB8: if(_tmpE6 != 0)goto _LLBA;_LLB9: t=Cyc_Absyn_float_typ;goto _LLB7;_LLBA: if(
_tmpE6 == 0)goto _LLBC;_tmpE7=*_tmpE6;_tmpE8=(int)_tmpE7.hd;_tmpE9=_tmpE7.tl;if(
_tmpE9 != 0)goto _LLBC;if(!(_tmpE8 == 'l'))goto _LLBC;_LLBB: t=Cyc_Absyn_double_typ(0);
goto _LLB7;_LLBC:;_LLBD:({struct Cyc_Int_pa_struct _tmpED;_tmpED.tag=1;_tmpED.f1=(
unsigned long)((int)_tmpB4);{struct Cyc_String_pa_struct _tmpEC;_tmpEC.tag=0;
_tmpEC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(_tmpB3));{
void*_tmpEA[2]={& _tmpEC,& _tmpED};Cyc_Tcutil_terr(loc,({const char*_tmpEB="length modifier '%s' is not allowed with %%%c";
_tag_dynforward(_tmpEB,sizeof(char),_get_zero_arr_size(_tmpEB,46));}),
_tag_dynforward(_tmpEA,sizeof(void*),2));}}});return 0;_LLB7:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpE5),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmpAB=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*
_tmpEE));_tmpEE->hd=(void*)t;_tmpEE->tl=_tmpAB;_tmpEE;});break;}case 'c': _LLB6: {
struct Cyc_Core_Opt*_tmpF0=({struct Cyc_Core_Opt*_tmpF2=_cycalloc(sizeof(*_tmpF2));
_tmpF2->v=Cyc_Tcenv_lookup_type_vars(te);_tmpF2;});_tmpAB=({struct Cyc_List_List*
_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->hd=(void*)Cyc_Absyn_dynforward_typ(Cyc_Absyn_char_typ,
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpF0),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmpF1->tl=_tmpAB;_tmpF1;});break;}case 's': _LLBE: {
struct Cyc_Core_Opt*_tmpF3=({struct Cyc_Core_Opt*_tmpF6=_cycalloc(sizeof(*_tmpF6));
_tmpF6->v=Cyc_Tcenv_lookup_type_vars(te);_tmpF6;});void*ptr=Cyc_Absyn_dynforward_typ(
Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({
struct Cyc_Core_Opt*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpF5;})),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmpAB=({struct
Cyc_List_List*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->hd=(void*)ptr;_tmpF4->tl=
_tmpAB;_tmpF4;});break;}case '[': _LLBF: goto _LLC0;case 'p': _LLC0:({struct Cyc_Int_pa_struct
_tmpF9;_tmpF9.tag=1;_tmpF9.f1=(unsigned long)((int)_tmpB4);{void*_tmpF7[1]={&
_tmpF9};Cyc_Tcutil_terr(loc,({const char*_tmpF8="%%%c is not supported";
_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,22));}),
_tag_dynforward(_tmpF7,sizeof(void*),1));}});return 0;case 'n': _LLC1: {struct Cyc_Core_Opt*
_tmpFA=({struct Cyc_Core_Opt*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp10D;});{struct Cyc_List_List*_tmpFB=_tmpB3;struct Cyc_List_List _tmpFC;int
_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_List_List _tmpFF;int _tmp100;struct Cyc_List_List*
_tmp101;struct Cyc_List_List _tmp102;int _tmp103;struct Cyc_List_List*_tmp104;struct
Cyc_List_List _tmp105;int _tmp106;struct Cyc_List_List*_tmp107;_LLC4: if(_tmpFB != 0)
goto _LLC6;_LLC5: t=Cyc_Absyn_sint_typ;goto _LLC3;_LLC6: if(_tmpFB == 0)goto _LLC8;
_tmpFC=*_tmpFB;_tmpFD=(int)_tmpFC.hd;_tmpFE=_tmpFC.tl;if(_tmpFE != 0)goto _LLC8;
if(!(_tmpFD == 'l'))goto _LLC8;_LLC7: t=Cyc_Absyn_ulong_typ;goto _LLC3;_LLC8: if(
_tmpFB == 0)goto _LLCA;_tmpFF=*_tmpFB;_tmp100=(int)_tmpFF.hd;_tmp101=_tmpFF.tl;if(
_tmp101 != 0)goto _LLCA;if(!(_tmp100 == 'h'))goto _LLCA;_LLC9: t=Cyc_Absyn_sshort_typ;
goto _LLC3;_LLCA: if(_tmpFB == 0)goto _LLCC;_tmp102=*_tmpFB;_tmp103=(int)_tmp102.hd;
_tmp104=_tmp102.tl;if(_tmp104 == 0)goto _LLCC;_tmp105=*_tmp104;_tmp106=(int)
_tmp105.hd;_tmp107=_tmp105.tl;if(_tmp107 != 0)goto _LLCC;if(!(_tmp103 == 'h'  && 
_tmp106 == 'h'))goto _LLCC;_LLCB: t=Cyc_Absyn_schar_typ;goto _LLC3;_LLCC:;_LLCD:({
struct Cyc_Int_pa_struct _tmp10B;_tmp10B.tag=1;_tmp10B.f1=(unsigned long)((int)
_tmpB4);{struct Cyc_String_pa_struct _tmp10A;_tmp10A.tag=0;_tmp10A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(_tmpB3));{void*_tmp108[2]={&
_tmp10A,& _tmp10B};Cyc_Tcutil_terr(loc,({const char*_tmp109="length modifier '%s' is not allowed with %%%c";
_tag_dynforward(_tmp109,sizeof(char),_get_zero_arr_size(_tmp109,46));}),
_tag_dynforward(_tmp108,sizeof(void*),2));}}});return 0;_LLC3:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpFA),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmpAB=({struct Cyc_List_List*_tmp10C=_cycalloc(
sizeof(*_tmp10C));_tmp10C->hd=(void*)t;_tmp10C->tl=_tmpAB;_tmp10C;});break;}case
'%': _LLC2: if(_tmpB1){({void*_tmp10E[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp10F="Assignment suppression (*) is not allowed with %%%%";_tag_dynforward(
_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,52));}),_tag_dynforward(_tmp10E,
sizeof(void*),0));});return 0;}if(_tmpB2 != 0){({struct Cyc_String_pa_struct _tmp112;
_tmp112.tag=0;_tmp112.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_implode(
_tmpB2));{void*_tmp110[1]={& _tmp112};Cyc_Tcutil_terr(loc,({const char*_tmp111="width '%s' not allowed with %%%%";
_tag_dynforward(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,33));}),
_tag_dynforward(_tmp110,sizeof(void*),1));}});return 0;}if(_tmpB3 != 0){({struct
Cyc_String_pa_struct _tmp115;_tmp115.tag=0;_tmp115.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_implode(_tmpB3));{void*_tmp113[1]={& _tmp115};Cyc_Tcutil_terr(
loc,({const char*_tmp114="length modifier '%s' not allowed with %%%%";
_tag_dynforward(_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,43));}),
_tag_dynforward(_tmp113,sizeof(void*),1));}});return 0;}break;default: _LLCE:
return 0;}}}}}return Cyc_List_imp_rev(_tmpAB);}
