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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_memq(struct Cyc_List_List*l,void*x);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];
typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
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
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_Hashtable_Table;
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*
cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*
t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,
void*key);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void*Cyc_Tcutil_impos(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*
t);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_unique_pointer(void*t);int Cyc_Tcutil_is_unique_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noreturn(void*);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
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
struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);struct Cyc_CfFlowInfo_BottomFL_struct{int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};union Cyc_CfFlowInfo_FlowInfo_union{struct Cyc_CfFlowInfo_BottomFL_struct
BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{
struct _RegionHandle*r;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);struct _dynforward_ptr
Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,
void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,
void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r);void*
Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);struct
Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,
struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,
union Cyc_CfFlowInfo_FlowInfo_union f,int clear);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c);struct _dynforward_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;})));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_AbsLVal_union f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;
_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";
_tag_dynforward(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,37));});_tmp7;});
_tmp6;}));_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*Cyc_NewControlFlow_get_stmt_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union**
sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);if(sflow
== 0){union Cyc_CfFlowInfo_FlowInfo_union*res=({union Cyc_CfFlowInfo_FlowInfo_union*
_tmp9=_region_malloc(env->r,sizeof(*_tmp9));_tmp9[0]=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmpA;(_tmpA.BottomFL).tag=0;_tmpA;});_tmp9;});((
void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo_union*
val))Cyc_Hashtable_insert)(env->flow_table,s,res);return res;}return*sflow;}
struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo_union*
f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo_union*
_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*_tmpC=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,inflow,*_tmpC,1);_tmpB->visited=env->iteration_num;
return({struct _tuple9 _tmpD;_tmpD.f1=_tmpB;_tmpD.f2=_tmpC;_tmpD;});}static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dynforward_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dynforward_ptr*_tmpE=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmpE,f)== 0)return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Core_Impossible_struct
_tmp10;_tmp10.tag=Cyc_Core_Impossible;_tmp10.f1=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*f);{void*_tmp11[1]={& _tmp13};Cyc_aprintf(({const char*
_tmp12="get_field_index_fs failed: %s";_tag_dynforward(_tmp12,sizeof(char),
_get_zero_arr_size(_tmp12,30));}),_tag_dynforward(_tmp11,sizeof(void*),1));}});
_tmp10;});_tmpF;}));}static int Cyc_NewControlFlow_get_field_index(void*t,struct
_dynforward_ptr*f){void*_tmp14=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp15;union Cyc_Absyn_AggrInfoU_union _tmp16;struct Cyc_List_List*_tmp17;_LL6: if(
_tmp14 <= (void*)4)goto _LLA;if(*((int*)_tmp14)!= 10)goto _LL8;_tmp15=((struct Cyc_Absyn_AggrType_struct*)
_tmp14)->f1;_tmp16=_tmp15.aggr_info;_LL7: {struct Cyc_List_List*_tmp18=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp16))->impl))->fields;
_tmp17=_tmp18;goto _LL9;}_LL8: if(*((int*)_tmp14)!= 11)goto _LLA;_tmp17=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp17,f);_LLA:;
_LLB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp19=_cycalloc(
sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Core_Impossible_struct _tmp1A;_tmp1A.tag=
Cyc_Core_Impossible;_tmp1A.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1B[1]={& _tmp1D};Cyc_aprintf(({const char*_tmp1C="get_field_index failed: %s";
_tag_dynforward(_tmp1C,sizeof(char),_get_zero_arr_size(_tmp1C,27));}),
_tag_dynforward(_tmp1B,sizeof(void*),1));}});_tmp1A;});_tmp19;}));_LL5:;}static
void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow,1);}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(
env,s);union Cyc_CfFlowInfo_FlowInfo_union _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,flow,*_tmp1F,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*
_tmp1F)){*_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo_union
_tmp21=inflow;struct Cyc_Dict_Dict _tmp22;struct Cyc_List_List*_tmp23;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp24;_LLD: if((_tmp21.BottomFL).tag != 0)goto _LLF;_LLE: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp25;(_tmp25.BottomFL).tag=0;_tmp25;});_LLF:
if((_tmp21.ReachableFL).tag != 1)goto _LLC;_tmp22=(_tmp21.ReachableFL).f1;_tmp23=(
_tmp21.ReachableFL).f2;_tmp24=(_tmp21.ReachableFL).f3;_LL10: for(0;vds != 0;vds=
vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp26=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp27=_region_malloc(fenv->r,sizeof(*_tmp27));_tmp27[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp28;_tmp28.tag=0;_tmp28.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp28;});_tmp27;});
_tmp22=Cyc_Dict_insert(_tmp22,(void*)_tmp26,Cyc_CfFlowInfo_typ_to_absrval(fenv,(
void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp29;(_tmp29.ReachableFL).tag=1;(_tmp29.ReachableFL).f1=
_tmp22;(_tmp29.ReachableFL).f2=_tmp23;(_tmp29.ReachableFL).f3=_tmp24;_tmp29;});
_LLC:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp2B;
struct Cyc_Dict_Dict _tmp2C;struct _tuple6 _tmp2A=Cyc_CfFlowInfo_save_consume_info(
fenv,outflow,0);_tmp2B=_tmp2A.f1;_tmp2C=_tmp2B.consumed;{struct _RegionHandle
_tmp2D=_new_region("tmp");struct _RegionHandle*tmp=& _tmp2D;_push_region(tmp);{
struct Cyc_Iter_Iter _tmp2E=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(tmp,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple10 _tmp2F=*((struct _tuple10*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(tmp,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp2E,& _tmp2F)){struct Cyc_CfFlowInfo_Place*_tmp30=_tmp2F.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp2C,_tmp30))({
struct Cyc_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(_tmp30));{void*_tmp31[1]={&
_tmp33};Cyc_Tcutil_terr(_tmp2F.f2,({const char*_tmp32="Failed to consume unique variable %s";
_tag_dynforward(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,37));}),
_tag_dynforward(_tmp31,sizeof(void*),1));}});}};_pop_region(tmp);}*
out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union _tmp34=inflow;struct Cyc_Dict_Dict
_tmp35;struct Cyc_List_List*_tmp36;struct Cyc_CfFlowInfo_ConsumeInfo _tmp37;_LL12:
if((_tmp34.BottomFL).tag != 0)goto _LL14;_LL13: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp38;(_tmp38.BottomFL).tag=0;_tmp38;});_LL14:
if((_tmp34.ReachableFL).tag != 1)goto _LL11;_tmp35=(_tmp34.ReachableFL).f1;_tmp36=(
_tmp34.ReachableFL).f2;_tmp37=(_tmp34.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp35,r)!= (void*)2)({void*_tmp39[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3A="expression may not be fully initialized";
_tag_dynforward(_tmp3A,sizeof(char),_get_zero_arr_size(_tmp3A,40));}),
_tag_dynforward(_tmp39,sizeof(void*),0));});{struct Cyc_Dict_Dict _tmp3B=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp35,env->all_changed,r);if(_tmp35.t == _tmp3B.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3C=({union Cyc_CfFlowInfo_FlowInfo_union _tmp3D;(
_tmp3D.ReachableFL).tag=1;(_tmp3D.ReachableFL).f1=_tmp3B;(_tmp3D.ReachableFL).f2=
_tmp36;(_tmp3D.ReachableFL).f3=_tmp37;_tmp3D;});Cyc_NewControlFlow_update_tryflow(
env,(union Cyc_CfFlowInfo_FlowInfo_union)_tmp3C);return(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp3C;}}_LL11:;}struct _tuple11{union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*
f2;};static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(struct
_RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct
_tuple11 _tmp3E;_tmp3E.f1=inflow;_tmp3E.f2=0;_tmp3E;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp40;void*_tmp41;struct _tuple5 _tmp3F=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp40=_tmp3F.f1;_tmp41=_tmp3F.f2;return({struct
_tuple11 _tmp42;_tmp42.f1=_tmp40;_tmp42.f2=({struct Cyc_List_List*_tmp43=
_region_malloc(rgn,sizeof(*_tmp43));_tmp43->hd=(void*)_tmp41;_tmp43->tl=0;_tmp43;});
_tmp42;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union old_inflow;union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp45;union
Cyc_CfFlowInfo_FlowInfo_union _tmp46;struct _tuple6 _tmp44=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp45=_tmp44.f1;_tmp46=_tmp44.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp5B;_tmp5B.consumed=(env->fenv)->mt_place_set;
_tmp5B.may_consume=0;_tmp5B;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp46=Cyc_CfFlowInfo_restore_consume_info(_tmp46,_tmp45);env->all_changed=({
struct Cyc_Dict_Dict*_tmp47=_region_malloc(env->r,sizeof(*_tmp47));_tmp47[0]=(env->fenv)->mt_place_set;
_tmp47;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp49;void*_tmp4A;struct _tuple5
_tmp48=Cyc_NewControlFlow_anal_Rexp(env,_tmp46,(struct Cyc_Absyn_Exp*)es->hd);
_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;outflow=_tmp49;rvals=({struct Cyc_List_List*
_tmp4B=_region_malloc(rgn,sizeof(*_tmp4B));_tmp4B->hd=(void*)_tmp4A;_tmp4B->tl=0;
_tmp4B;});this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((
struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(arg1_unconsumed){union
Cyc_CfFlowInfo_FlowInfo_union _tmp4C=outflow;struct Cyc_Dict_Dict _tmp4D;struct Cyc_List_List*
_tmp4E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4F;_LL17: if((_tmp4C.BottomFL).tag != 0)
goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp4C.ReachableFL).tag != 1)goto _LL16;_tmp4D=(
_tmp4C.ReachableFL).f1;_tmp4E=(_tmp4C.ReachableFL).f2;_tmp4F=(_tmp4C.ReachableFL).f3;
_LL1A: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp50;(_tmp50.ReachableFL).tag=1;(_tmp50.ReachableFL).f1=_tmp4D;(_tmp50.ReachableFL).f2=
_tmp4E;(_tmp50.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp51;_tmp51.consumed=
_tmp4F.consumed;_tmp51.may_consume=_tmp45.may_consume;_tmp51;});_tmp50;});_LL16:;}{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Dict_Dict*_tmp52=_region_malloc(env->r,sizeof(*_tmp52));_tmp52[0]=(env->fenv)->mt_place_set;
_tmp52;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp54;void*_tmp55;struct _tuple5
_tmp53=Cyc_NewControlFlow_anal_Rexp(env,_tmp46,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;rvals=({struct Cyc_List_List*_tmp56=
_region_malloc(rgn,sizeof(*_tmp56));_tmp56->hd=(void*)_tmp55;_tmp56->tl=rvals;
_tmp56;});outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp54,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp58;struct _tuple6 _tmp57=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp58=_tmp57.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp46,outflow_consume);else{old_inflow=_tmp46;}init_consume=1;outflow_consume=
_tmp58;_tmp46=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,_tmp46,
outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp46,old_inflow));if(
outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict*
_tmp59=_region_malloc(env->r,sizeof(*_tmp59));_tmp59[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp59;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple11 _tmp5A;_tmp5A.f1=outflow;_tmp5A.f2=Cyc_List_imp_rev(
rvals);_tmp5A;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp5C=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp5C;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple11 _tmp5D=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp60=_tmp5E;struct Cyc_Dict_Dict _tmp61;_LL1C: if((_tmp60.ReachableFL).tag != 1)
goto _LL1E;_tmp61=(_tmp60.ReachableFL).f1;_LL1D: for(0;_tmp5F != 0;(_tmp5F=_tmp5F->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp61,(void*)_tmp5F->hd)== (void*)0)({void*_tmp62[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp63="expression may not be initialized";_tag_dynforward(_tmp63,sizeof(
char),_get_zero_arr_size(_tmp63,34));}),_tag_dynforward(_tmp62,sizeof(void*),0));});}
goto _LL1B;_LL1E: if((_tmp60.BottomFL).tag != 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{
struct _tuple5 _tmp65=({struct _tuple5 _tmp64;_tmp64.f1=_tmp5E;_tmp64.f2=(void*)(env->fenv)->unknown_all;
_tmp64;});_npop_handler(0);return _tmp65;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp66=outflow;struct Cyc_Dict_Dict _tmp67;
struct Cyc_List_List*_tmp68;struct Cyc_CfFlowInfo_ConsumeInfo _tmp69;_LL21: if((
_tmp66.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23: if((_tmp66.ReachableFL).tag
!= 1)goto _LL20;_tmp67=(_tmp66.ReachableFL).f1;_tmp68=(_tmp66.ReachableFL).f2;
_tmp69=(_tmp66.ReachableFL).f3;_LL24: {union Cyc_CfFlowInfo_AbsLVal_union _tmp6A=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp6B;
_LL26: if((_tmp6A.UnknownL).tag != 1)goto _LL28;_LL27: return outflow;_LL28: if((
_tmp6A.PlaceL).tag != 0)goto _LL25;_tmp6B=(_tmp6A.PlaceL).f1;_LL29: {void*nzval=il
== (void*)2?(void*)1:(void*)2;return(union Cyc_CfFlowInfo_FlowInfo_union)({union
Cyc_CfFlowInfo_FlowInfo_union _tmp6C;(_tmp6C.ReachableFL).tag=1;(_tmp6C.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp67,env->all_changed,_tmp6B,
nzval);(_tmp6C.ReachableFL).f2=_tmp68;(_tmp6C.ReachableFL).f3=_tmp69;_tmp6C;});}
_LL25:;}_LL20:;}static struct _tuple8 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp6D=
outflow;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp70;_LL2B: if((_tmp6D.BottomFL).tag != 0)goto _LL2D;_LL2C: return({struct _tuple8
_tmp71;_tmp71.f1=outflow;_tmp71.f2=outflow;_tmp71;});_LL2D: if((_tmp6D.ReachableFL).tag
!= 1)goto _LL2A;_tmp6E=(_tmp6D.ReachableFL).f1;_tmp6F=(_tmp6D.ReachableFL).f2;
_tmp70=(_tmp6D.ReachableFL).f3;_LL2E: {union Cyc_CfFlowInfo_AbsLVal_union _tmp72=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp73;
_LL30: if((_tmp72.UnknownL).tag != 1)goto _LL32;_LL31: return({struct _tuple8 _tmp74;
_tmp74.f1=outflow;_tmp74.f2=outflow;_tmp74;});_LL32: if((_tmp72.PlaceL).tag != 0)
goto _LL2F;_tmp73=(_tmp72.PlaceL).f1;_LL33: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple8 _tmp75;_tmp75.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp77;(_tmp77.ReachableFL).tag=1;(_tmp77.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6E,env->all_changed,_tmp73,
nzval);(_tmp77.ReachableFL).f2=_tmp6F;(_tmp77.ReachableFL).f3=_tmp70;_tmp77;});
_tmp75.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp76;(_tmp76.ReachableFL).tag=1;(_tmp76.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp6E,env->all_changed,_tmp73,(void*)0);(_tmp76.ReachableFL).f2=
_tmp6F;(_tmp76.ReachableFL).f3=_tmp70;_tmp76;});_tmp75;});}_LL2F:;}_LL2A:;}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,
void*r2){union Cyc_CfFlowInfo_FlowInfo_union _tmp78=flow;struct Cyc_Dict_Dict _tmp79;
struct Cyc_List_List*_tmp7A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp7B;_LL35: if((
_tmp78.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((_tmp78.ReachableFL).tag
!= 1)goto _LL34;_tmp79=(_tmp78.ReachableFL).f1;_tmp7A=(_tmp78.ReachableFL).f2;
_tmp7B=(_tmp78.ReachableFL).f3;_LL38: {void*_tmp7C=r2;struct Cyc_List_List*_tmp7D;
_LL3A: if(_tmp7C <= (void*)3)goto _LL3C;if(*((int*)_tmp7C)!= 3)goto _LL3C;_tmp7D=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7C)->f1;_LL3B: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp7E=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp7F;_LL3F: if((_tmp7E.UnknownL).tag != 1)goto _LL41;_LL40: return flow;_LL41: if((
_tmp7E.PlaceL).tag != 0)goto _LL3E;_tmp7F=(_tmp7E.PlaceL).f1;_LL42: {struct Cyc_List_List*
new_cl;{void*_tmp80=r1;struct Cyc_List_List*_tmp81;void*_tmp82;_LL44: if(_tmp80 <= (
void*)3)goto _LL48;if(*((int*)_tmp80)!= 3)goto _LL46;_tmp81=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp80)->f1;_LL45: new_cl=_tmp81;goto _LL43;_LL46: if(*((int*)_tmp80)!= 0)goto _LL48;
_tmp82=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp80)->f1;if((int)_tmp82
!= 2)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp80 != 0)goto _LL4A;_LL49: goto
_LL4B;_LL4A: if((int)_tmp80 != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D:
return flow;_LL43:;}for(0;_tmp7D != 0;_tmp7D=_tmp7D->tl){void*new_cmp;{struct
_tuple0 _tmp84=({struct _tuple0 _tmp83;_tmp83.f1=p;_tmp83.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp7D->hd)->cmp;_tmp83;});void*_tmp85;void*_tmp86;void*_tmp87;void*_tmp88;_LL4F:
_tmp85=_tmp84.f1;if((int)_tmp85 != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp86=
_tmp84.f2;if((int)_tmp86 != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53:
_tmp87=_tmp84.f1;if((int)_tmp87 != 5)goto _LL55;_tmp88=_tmp84.f2;if((int)_tmp88 != 
5)goto _LL55;_LL54: new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto
_LL4E;_LL4E:;}new_cl=({struct Cyc_List_List*_tmp89=_region_malloc(env->r,sizeof(*
_tmp89));_tmp89->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp8A=_region_malloc(env->r,
sizeof(*_tmp8A));_tmp8A->cmp=(void*)new_cmp;_tmp8A->bd=(void*)((void*)((struct
Cyc_CfFlowInfo_TagCmp*)_tmp7D->hd)->bd);_tmp8A;});_tmp89->tl=new_cl;_tmp89;});}
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp8B;(_tmp8B.ReachableFL).tag=1;(_tmp8B.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp79,env->all_changed,_tmp7F,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp8C=_region_malloc(env->r,sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp8D;_tmp8D.tag=3;_tmp8D.f1=new_cl;_tmp8D;});_tmp8C;}));(_tmp8B.ReachableFL).f2=
_tmp7A;(_tmp8B.ReachableFL).f3=_tmp7B;_tmp8B;});}_LL3E:;}_LL3C:;_LL3D: return flow;
_LL39:;}_LL34:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp90=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp91;void*_tmp92;struct Cyc_Absyn_PtrAtts _tmp93;struct
Cyc_Absyn_Conref*_tmp94;struct Cyc_Absyn_Conref*_tmp95;_LL58: if(_tmp90 <= (void*)4)
goto _LL5A;if(*((int*)_tmp90)!= 4)goto _LL5A;_tmp91=((struct Cyc_Absyn_PointerType_struct*)
_tmp90)->f1;_tmp92=(void*)_tmp91.elt_typ;_tmp93=_tmp91.ptr_atts;_tmp94=_tmp93.bounds;
_tmp95=_tmp93.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp96=f;
struct Cyc_Dict_Dict _tmp97;struct Cyc_List_List*_tmp98;_LL5D: if((_tmp96.BottomFL).tag
!= 0)goto _LL5F;_LL5E: return({struct _tuple5 _tmp99;_tmp99.f1=f;_tmp99.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_all);_tmp99;});_LL5F: if((_tmp96.ReachableFL).tag
!= 1)goto _LL5C;_tmp97=(_tmp96.ReachableFL).f1;_tmp98=(_tmp96.ReachableFL).f2;
_LL60: if(Cyc_Tcutil_is_bound_one(_tmp94)){void*_tmp9A=r;struct Cyc_CfFlowInfo_Place*
_tmp9B;void*_tmp9C;_LL62: if((int)_tmp9A != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmp9A != 2)goto _LL66;_LL65:{void*_tmp9D=(void*)e->annot;struct Cyc_List_List*
_tmp9E;_LL6F: if(*((void**)_tmp9D)!= Cyc_CfFlowInfo_NotZero)goto _LL71;_tmp9E=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp9D)->f1;_LL70: if(!Cyc_CfFlowInfo_same_relns(
_tmp98,_tmp9E))goto _LL72;goto _LL6E;_LL71:;_LL72:{void*_tmp9F=(void*)e->r;_LL74:
if(*((int*)_tmp9F)!= 25)goto _LL76;_LL75:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_NotZero_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmpA1;_tmpA1.tag=Cyc_CfFlowInfo_NotZero;
_tmpA1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp98);_tmpA1;});
_tmpA0;})));goto _LL73;_LL76:;_LL77:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL73;_LL73:;}goto _LL6E;_LL6E:;}goto _LL61;_LL66: if(_tmp9A <= (void*)3)goto
_LL68;if(*((int*)_tmp9A)!= 2)goto _LL68;_tmp9B=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9A)->f1;_LL67:{void*_tmpA2=(void*)e->annot;struct Cyc_List_List*_tmpA3;_LL79:
if(*((void**)_tmpA2)!= Cyc_CfFlowInfo_NotZero)goto _LL7B;_tmpA3=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA2)->f1;_LL7A: if(!Cyc_CfFlowInfo_same_relns(_tmp98,_tmpA3))goto _LL7C;goto
_LL78;_LL7B:;_LL7C:{void*_tmpA4=(void*)e->r;_LL7E: if(*((int*)_tmpA4)!= 25)goto
_LL80;_LL7F:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmpA6;_tmpA6.tag=Cyc_CfFlowInfo_NotZero;_tmpA6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp98);_tmpA6;});_tmpA5;})));goto _LL7D;_LL80:;_LL81:(void*)(e->annot=(void*)((
void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL7D;_LL7D:;}goto _LL78;_LL78:;}
return({struct _tuple5 _tmpA7;_tmpA7.f1=f;_tmpA7.f2=Cyc_CfFlowInfo_lookup_place(
_tmp97,_tmp9B);_tmpA7;});_LL68: if((int)_tmp9A != 0)goto _LL6A;_LL69:(void*)(e->annot=(
void*)((void*)Cyc_CfFlowInfo_IsZero));return({struct _tuple5 _tmpA8;_tmpA8.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpA9;(
_tmpA9.BottomFL).tag=0;_tmpA9;});_tmpA8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmp92,(void*)(env->fenv)->unknown_all);_tmpA8;});_LL6A: if(_tmp9A <= (void*)3)
goto _LL6C;if(*((int*)_tmp9A)!= 0)goto _LL6C;_tmp9C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp9A)->f1;_LL6B: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp9C);goto _LL6D;
_LL6C:;_LL6D:{void*_tmpAA=(void*)e->r;_LL83: if(*((int*)_tmpAA)!= 25)goto _LL85;
_LL84:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpAC;_tmpAC.tag=Cyc_CfFlowInfo_UnknownZ;_tmpAC.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp98);_tmpAC;});_tmpAB;})));goto _LL82;_LL85:;_LL86:(void*)(
e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));goto _LL82;_LL82:;}
goto _LL61;_LL61:;}else{void*_tmpAD=(void*)e->annot;struct Cyc_List_List*_tmpAE;
_LL88: if(*((void**)_tmpAD)!= Cyc_CfFlowInfo_UnknownZ)goto _LL8A;_tmpAE=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmpAD)->f1;_LL89: if(!Cyc_CfFlowInfo_same_relns(
_tmp98,_tmpAE))goto _LL8B;goto _LL87;_LL8A:;_LL8B:{void*_tmpAF=(void*)e->r;_LL8D:
if(*((int*)_tmpAF)!= 25)goto _LL8F;_LL8E:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownZ_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmpB1;_tmpB1.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpB1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp98);_tmpB1;});
_tmpB0;})));goto _LL8C;_LL8F:;_LL90:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL8C;_LL8C:;}goto _LL87;_LL87:;}{void*_tmpB2=Cyc_CfFlowInfo_initlevel(_tmp97,
r);_LL92: if((int)_tmpB2 != 0)goto _LL94;_LL93:({void*_tmpB3[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmpB4="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,46));}),
_tag_dynforward(_tmpB3,sizeof(void*),0));});goto _LL95;_LL94: if((int)_tmpB2 != 2)
goto _LL96;_LL95: return({struct _tuple5 _tmpB5;_tmpB5.f1=f;_tmpB5.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_all);_tmpB5;});_LL96: if((int)_tmpB2
!= 1)goto _LL91;_LL97: return({struct _tuple5 _tmpB6;_tmpB6.f1=f;_tmpB6.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_none);_tmpB6;});_LL91:;}_LL5C:;}
_LL5A:;_LL5B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpB7=
_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Core_Impossible_struct _tmpB8;
_tmpB8.tag=Cyc_Core_Impossible;_tmpB8.f1=({const char*_tmpB9="right deref of non-pointer-type";
_tag_dynforward(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,32));});_tmpB8;});
_tmpB7;}));_LL57:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpBA=(void*)e1->r;void*_tmpBB;struct Cyc_Absyn_Vardecl*
_tmpBC;void*_tmpBD;struct Cyc_Absyn_Vardecl*_tmpBE;void*_tmpBF;struct Cyc_Absyn_Vardecl*
_tmpC0;void*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC2;_LL99: if(*((int*)_tmpBA)!= 1)
goto _LL9B;_tmpBB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBB <= (
void*)1)goto _LL9B;if(*((int*)_tmpBB)!= 4)goto _LL9B;_tmpBC=((struct Cyc_Absyn_Pat_b_struct*)
_tmpBB)->f1;_LL9A: _tmpBE=_tmpBC;goto _LL9C;_LL9B: if(*((int*)_tmpBA)!= 1)goto _LL9D;
_tmpBD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBD <= (void*)1)
goto _LL9D;if(*((int*)_tmpBD)!= 3)goto _LL9D;_tmpBE=((struct Cyc_Absyn_Local_b_struct*)
_tmpBD)->f1;_LL9C: _tmpC0=_tmpBE;goto _LL9E;_LL9D: if(*((int*)_tmpBA)!= 1)goto _LL9F;
_tmpBF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBF <= (void*)1)
goto _LL9F;if(*((int*)_tmpBF)!= 2)goto _LL9F;_tmpC0=((struct Cyc_Absyn_Param_b_struct*)
_tmpBF)->f1;_LL9E: _tmpC2=_tmpC0;goto _LLA0;_LL9F: if(*((int*)_tmpBA)!= 1)goto _LLA1;
_tmpC1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpC1 <= (void*)1)
goto _LLA1;if(*((int*)_tmpC1)!= 0)goto _LLA1;_tmpC2=((struct Cyc_Absyn_Global_b_struct*)
_tmpC1)->f1;_LLA0: if(!_tmpC2->escapes){void*_tmpC3=(void*)e2->r;void*_tmpC4;
struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;void*
_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_Absyn_Vardecl*_tmpCB;
_LLA4: if(*((int*)_tmpC3)!= 1)goto _LLA6;_tmpC4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC4 <= (void*)1)goto _LLA6;if(*((int*)_tmpC4)!= 4)goto _LLA6;
_tmpC5=((struct Cyc_Absyn_Pat_b_struct*)_tmpC4)->f1;_LLA5: _tmpC7=_tmpC5;goto _LLA7;
_LLA6: if(*((int*)_tmpC3)!= 1)goto _LLA8;_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC6 <= (void*)1)goto _LLA8;if(*((int*)_tmpC6)!= 3)goto _LLA8;
_tmpC7=((struct Cyc_Absyn_Local_b_struct*)_tmpC6)->f1;_LLA7: _tmpC9=_tmpC7;goto
_LLA9;_LLA8: if(*((int*)_tmpC3)!= 1)goto _LLAA;_tmpC8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC8 <= (void*)1)goto _LLAA;if(*((int*)_tmpC8)!= 2)goto _LLAA;
_tmpC9=((struct Cyc_Absyn_Param_b_struct*)_tmpC8)->f1;_LLA9: _tmpCB=_tmpC9;goto
_LLAB;_LLAA: if(*((int*)_tmpC3)!= 1)goto _LLAC;_tmpCA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpCA <= (void*)1)goto _LLAC;if(*((int*)_tmpCA)!= 0)goto _LLAC;
_tmpCB=((struct Cyc_Absyn_Global_b_struct*)_tmpCA)->f1;_LLAB: if(!_tmpCB->escapes){
int found=0;{struct Cyc_List_List*_tmpCC=relns;for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
struct Cyc_CfFlowInfo_Reln*_tmpCD=(struct Cyc_CfFlowInfo_Reln*)_tmpCC->hd;if(
_tmpCD->vd == _tmpCB){union Cyc_CfFlowInfo_RelnOp_union _tmpCE=_tmpCD->rop;struct
Cyc_Absyn_Vardecl*_tmpCF;_LLAF: if((_tmpCE.LessSize).tag != 2)goto _LLB1;_tmpCF=(
_tmpCE.LessSize).f1;if(!(_tmpCF == _tmpC2))goto _LLB1;_LLB0: return relns;_LLB1:;
_LLB2: continue;_LLAE:;}}}if(!found)return({struct Cyc_List_List*_tmpD0=
_region_malloc(rgn,sizeof(*_tmpD0));_tmpD0->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpD1=_region_malloc(rgn,sizeof(*_tmpD1));_tmpD1->vd=_tmpCB;_tmpD1->rop=(union
Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmpD2;(_tmpD2.LessSize).tag=
2;(_tmpD2.LessSize).f1=_tmpC2;_tmpD2;});_tmpD1;});_tmpD0->tl=relns;_tmpD0;});}
return relns;_LLAC:;_LLAD: return relns;_LLA3:;}else{return relns;}_LLA1:;_LLA2:
return relns;_LL98:;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_assignment(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD3=env->fenv;if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v) && Cyc_Tcutil_is_unique_path(e)){cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD3->r,Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*_tmpD4=_region_malloc(
_tmpD3->r,sizeof(*_tmpD4));_tmpD4->hd=p;_tmpD4->tl=0;_tmpD4;}),cinfo.may_consume);
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place _tmpD6;void*_tmpD7;struct Cyc_CfFlowInfo_Place*
_tmpD5=p;_tmpD6=*_tmpD5;_tmpD7=(void*)_tmpD6.root;{void*rval=(void*)0;if((Cyc_Dict_lookup_bool(
outdict,_tmpD7,& rval) && Cyc_CfFlowInfo_initlevel(outdict,rval)!= (void*)0) && 
rval != (void*)0){void*_tmpD8=_tmpD7;struct Cyc_Absyn_Vardecl*_tmpD9;_LLB4: if(*((
int*)_tmpD8)!= 0)goto _LLB6;_tmpD9=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpD8)->f1;
_LLB5: {struct _dynforward_ptr _tmpDA=Cyc_Absynpp_qvar2string(_tmpD9->name);({
struct Cyc_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmpDA);{void*_tmpDB[1]={& _tmpDD};Cyc_Tcutil_warn(e->loc,({
const char*_tmpDC="may clobber unique pointer %s";_tag_dynforward(_tmpDC,sizeof(
char),_get_zero_arr_size(_tmpDC,30));}),_tag_dynforward(_tmpDB,sizeof(void*),1));}});
goto _LLB3;}_LLB6:;_LLB7:({void*_tmpDE[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpDF="warning locations not for VarRoots";
_tag_dynforward(_tmpDF,sizeof(char),_get_zero_arr_size(_tmpDF,35));}),
_tag_dynforward(_tmpDE,sizeof(void*),0));});_LLB3:;}}}if(env->all_changed == 0){
struct Cyc_Dict_Dict _tmpE0=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct
Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)(_tmpD3->r,cinfo.consumed,
p);if((cinfo.consumed).t != _tmpE0.t);cinfo.consumed=_tmpE0;}}return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmpE1=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo
cinfo;{union Cyc_CfFlowInfo_FlowInfo_union _tmpE2=inflow;struct Cyc_Dict_Dict _tmpE3;
struct Cyc_List_List*_tmpE4;struct Cyc_CfFlowInfo_ConsumeInfo _tmpE5;_LLB9: if((
_tmpE2.BottomFL).tag != 0)goto _LLBB;_LLBA: return({struct _tuple5 _tmpE6;_tmpE6.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpE7;(
_tmpE7.BottomFL).tag=0;_tmpE7;});_tmpE6.f2=(void*)_tmpE1->unknown_all;_tmpE6;});
_LLBB: if((_tmpE2.ReachableFL).tag != 1)goto _LLB8;_tmpE3=(_tmpE2.ReachableFL).f1;
_tmpE4=(_tmpE2.ReachableFL).f2;_tmpE5=(_tmpE2.ReachableFL).f3;_LLBC: d=_tmpE3;
relns=_tmpE4;cinfo=_tmpE5;_LLB8:;}{void*_tmpE8=(void*)e->r;struct Cyc_Absyn_Exp*
_tmpE9;void*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct
Cyc_Absyn_Exp*_tmpED;union Cyc_Absyn_Cnst_union _tmpEE;union Cyc_Absyn_Cnst_union
_tmpEF;int _tmpF0;union Cyc_Absyn_Cnst_union _tmpF1;void*_tmpF2;struct Cyc_List_List*
_tmpF3;void*_tmpF4;void*_tmpF5;struct Cyc_Absyn_Vardecl*_tmpF6;void*_tmpF7;struct
Cyc_Absyn_Vardecl*_tmpF8;void*_tmpF9;struct Cyc_Absyn_Vardecl*_tmpFA;void*_tmpFB;
struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;
struct Cyc_Core_Opt*_tmpFF;struct Cyc_Core_Opt _tmp100;struct Cyc_Absyn_Exp*_tmp101;
struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Core_Opt*_tmp103;struct Cyc_Absyn_Exp*
_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*
_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_MallocInfo
_tmp10A;int _tmp10B;struct Cyc_Absyn_Exp*_tmp10C;void**_tmp10D;struct Cyc_Absyn_Exp*
_tmp10E;int _tmp10F;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;
struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*
_tmp114;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct
_dynforward_ptr*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct _dynforward_ptr*
_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*
_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*
_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*
_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_Tuniondecl*_tmp124;struct Cyc_List_List*
_tmp125;struct Cyc_List_List*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Aggrdecl*
_tmp128;struct Cyc_Absyn_Aggrdecl _tmp129;struct Cyc_Absyn_AggrdeclImpl*_tmp12A;
struct Cyc_Absyn_AggrdeclImpl _tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*
_tmp12D;struct Cyc_Absyn_Vardecl*_tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*
_tmp130;int _tmp131;struct Cyc_Absyn_Stmt*_tmp132;void*_tmp133;_LLBE: if(*((int*)
_tmpE8)!= 15)goto _LLC0;_tmpE9=((struct Cyc_Absyn_Cast_e_struct*)_tmpE8)->f2;
_tmpEA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpE8)->f4;if((int)_tmpEA != 2)
goto _LLC0;_LLBF: {union Cyc_CfFlowInfo_FlowInfo_union _tmp135;void*_tmp136;struct
_tuple5 _tmp134=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpE9);_tmp135=_tmp134.f1;
_tmp136=_tmp134.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp138;void*_tmp139;
struct _tuple5 _tmp137=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp135,_tmpE9,
_tmp136);_tmp138=_tmp137.f1;_tmp139=_tmp137.f2;return({struct _tuple5 _tmp13A;
_tmp13A.f1=_tmp138;_tmp13A.f2=_tmp136;_tmp13A;});}}_LLC0: if(*((int*)_tmpE8)!= 15)
goto _LLC2;_tmpEB=((struct Cyc_Absyn_Cast_e_struct*)_tmpE8)->f2;_LLC1: _tmpEC=
_tmpEB;goto _LLC3;_LLC2: if(*((int*)_tmpE8)!= 13)goto _LLC4;_tmpEC=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpE8)->f1;_LLC3: _tmpED=_tmpEC;goto _LLC5;_LLC4: if(*((int*)_tmpE8)!= 14)goto
_LLC6;_tmpED=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpE8)->f1;_LLC5: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpED);_LLC6: if(*((int*)_tmpE8)!= 0)goto _LLC8;_tmpEE=((struct Cyc_Absyn_Const_e_struct*)
_tmpE8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).Null_c).tag != 6)
goto _LLC8;_LLC7: goto _LLC9;_LLC8: if(*((int*)_tmpE8)!= 0)goto _LLCA;_tmpEF=((struct
Cyc_Absyn_Const_e_struct*)_tmpE8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpE8)->f1).Int_c).tag != 2)goto _LLCA;_tmpF0=(_tmpEF.Int_c).f2;if(_tmpF0 != 0)
goto _LLCA;_LLC9: return({struct _tuple5 _tmp13B;_tmp13B.f1=inflow;_tmp13B.f2=(void*)
0;_tmp13B;});_LLCA: if(*((int*)_tmpE8)!= 0)goto _LLCC;_tmpF1=((struct Cyc_Absyn_Const_e_struct*)
_tmpE8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE8)->f1).Int_c).tag != 2)
goto _LLCC;_LLCB: goto _LLCD;_LLCC: if(*((int*)_tmpE8)!= 1)goto _LLCE;_tmpF2=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpE8)->f2;if(_tmpF2 <= (void*)1)goto _LLCE;if(*((
int*)_tmpF2)!= 1)goto _LLCE;_LLCD: return({struct _tuple5 _tmp13C;_tmp13C.f1=inflow;
_tmp13C.f2=(void*)1;_tmp13C;});_LLCE: if(*((int*)_tmpE8)!= 32)goto _LLD0;_tmpF3=((
struct Cyc_Absyn_Tunion_e_struct*)_tmpE8)->f1;if(_tmpF3 != 0)goto _LLD0;_LLCF: goto
_LLD1;_LLD0: if(*((int*)_tmpE8)!= 0)goto _LLD2;_LLD1: goto _LLD3;_LLD2: if(*((int*)
_tmpE8)!= 19)goto _LLD4;_LLD3: goto _LLD5;_LLD4: if(*((int*)_tmpE8)!= 18)goto _LLD6;
_LLD5: goto _LLD7;_LLD6: if(*((int*)_tmpE8)!= 20)goto _LLD8;_LLD7: goto _LLD9;_LLD8:
if(*((int*)_tmpE8)!= 21)goto _LLDA;_LLD9: goto _LLDB;_LLDA: if(*((int*)_tmpE8)!= 34)
goto _LLDC;_LLDB: goto _LLDD;_LLDC: if(*((int*)_tmpE8)!= 33)goto _LLDE;_LLDD: return({
struct _tuple5 _tmp13D;_tmp13D.f1=inflow;_tmp13D.f2=(void*)_tmpE1->unknown_all;
_tmp13D;});_LLDE: if(*((int*)_tmpE8)!= 1)goto _LLE0;_tmpF4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE8)->f2;if(_tmpF4 <= (void*)1)goto _LLE0;if(*((int*)_tmpF4)!= 0)goto _LLE0;
_LLDF: return({struct _tuple5 _tmp13E;_tmp13E.f1=inflow;_tmp13E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpE1,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpE1->unknown_all);
_tmp13E;});_LLE0: if(*((int*)_tmpE8)!= 1)goto _LLE2;_tmpF5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE8)->f2;if(_tmpF5 <= (void*)1)goto _LLE2;if(*((int*)_tmpF5)!= 2)goto _LLE2;
_tmpF6=((struct Cyc_Absyn_Param_b_struct*)_tmpF5)->f1;_LLE1: _tmpF8=_tmpF6;goto
_LLE3;_LLE2: if(*((int*)_tmpE8)!= 1)goto _LLE4;_tmpF7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE8)->f2;if(_tmpF7 <= (void*)1)goto _LLE4;if(*((int*)_tmpF7)!= 3)goto _LLE4;
_tmpF8=((struct Cyc_Absyn_Local_b_struct*)_tmpF7)->f1;_LLE3: _tmpFA=_tmpF8;goto
_LLE5;_LLE4: if(*((int*)_tmpE8)!= 1)goto _LLE6;_tmpF9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE8)->f2;if(_tmpF9 <= (void*)1)goto _LLE6;if(*((int*)_tmpF9)!= 4)goto _LLE6;
_tmpFA=((struct Cyc_Absyn_Pat_b_struct*)_tmpF9)->f1;_LLE5: if(Cyc_Tcutil_is_unique_pointer((
void*)_tmpFA->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp13F=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp144=_region_malloc(_tmpE1->r,sizeof(*_tmp144));_tmp144[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=_tmpFA;_tmp145;});_tmp144;});struct Cyc_CfFlowInfo_Place*
_tmp140=({struct Cyc_CfFlowInfo_Place*_tmp143=_region_malloc(_tmpE1->r,sizeof(*
_tmp143));_tmp143->root=(void*)((void*)_tmp13F);_tmp143->fields=0;_tmp143;});
cinfo.may_consume=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,
struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpE1->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp141=_region_malloc(_tmpE1->r,sizeof(*_tmp141));_tmp141->hd=
_tmp140;_tmp141->tl=0;_tmp141;}),cinfo.may_consume);inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp142;(_tmp142.ReachableFL).tag=1;(_tmp142.ReachableFL).f1=
d;(_tmp142.ReachableFL).f2=relns;(_tmp142.ReachableFL).f3=cinfo;_tmp142;});}
return({struct _tuple5 _tmp146;_tmp146.f1=inflow;_tmp146.f2=Cyc_Dict_lookup(d,(
void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp147=_region_malloc(env->r,
sizeof(*_tmp147));_tmp147[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp148;
_tmp148.tag=0;_tmp148.f1=_tmpFA;_tmp148;});_tmp147;}));_tmp146;});_LLE6: if(*((
int*)_tmpE8)!= 3)goto _LLE8;_tmpFB=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpE8)->f1;_tmpFC=((struct Cyc_Absyn_Primop_e_struct*)_tmpE8)->f2;_LLE7: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp14A;void*_tmp14B;struct _tuple5 _tmp149=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpFC,0);_tmp14A=_tmp149.f1;_tmp14B=_tmp149.f2;{void*_tmp14C=_tmpFB;
_LL125: if((int)_tmp14C != 0)goto _LL127;_LL126: goto _LL128;_LL127: if((int)_tmp14C != 
2)goto _LL129;_LL128: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpFC))->hd)->loc,_tmp14A);goto _LL124;_LL129:;
_LL12A: _tmp14A=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpFC))->hd)->loc,_tmp14A);goto _LL124;_LL124:;}
return({struct _tuple5 _tmp14D;_tmp14D.f1=_tmp14A;_tmp14D.f2=_tmp14B;_tmp14D;});}
_LLE8: if(*((int*)_tmpE8)!= 5)goto _LLEA;_tmpFD=((struct Cyc_Absyn_Increment_e_struct*)
_tmpE8)->f1;_LLE9: {struct Cyc_List_List _tmp14E=({struct Cyc_List_List _tmp15D;
_tmp15D.hd=_tmpFD;_tmp15D.tl=0;_tmp15D;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp150;struct _tuple5 _tmp14F=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp14E,0);_tmp150=_tmp14F.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmpFD->loc,_tmp150);{union Cyc_CfFlowInfo_AbsLVal_union _tmp152;struct _tuple7
_tmp151=Cyc_NewControlFlow_anal_Lexp(env,_tmp150,_tmpFD);_tmp152=_tmp151.f2;{
struct _tuple12 _tmp154=({struct _tuple12 _tmp153;_tmp153.f1=_tmp152;_tmp153.f2=
_tmp150;_tmp153;});union Cyc_CfFlowInfo_AbsLVal_union _tmp155;struct Cyc_CfFlowInfo_Place*
_tmp156;union Cyc_CfFlowInfo_FlowInfo_union _tmp157;struct Cyc_Dict_Dict _tmp158;
struct Cyc_List_List*_tmp159;struct Cyc_CfFlowInfo_ConsumeInfo _tmp15A;_LL12C:
_tmp155=_tmp154.f1;if(((_tmp154.f1).PlaceL).tag != 0)goto _LL12E;_tmp156=(_tmp155.PlaceL).f1;
_tmp157=_tmp154.f2;if(((_tmp154.f2).ReachableFL).tag != 1)goto _LL12E;_tmp158=(
_tmp157.ReachableFL).f1;_tmp159=(_tmp157.ReachableFL).f2;_tmp15A=(_tmp157.ReachableFL).f3;
_LL12D: _tmp159=Cyc_CfFlowInfo_reln_kill_exp(_tmpE1->r,_tmp159,_tmpFD);_tmp150=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp15B;(
_tmp15B.ReachableFL).tag=1;(_tmp15B.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmpE1,_tmpFD->loc,_tmp158,env->all_changed,_tmp156,(void*)_tmpE1->unknown_all);(
_tmp15B.ReachableFL).f2=_tmp159;(_tmp15B.ReachableFL).f3=_tmp15A;_tmp15B;});goto
_LL12B;_LL12E:;_LL12F: goto _LL12B;_LL12B:;}return({struct _tuple5 _tmp15C;_tmp15C.f1=
_tmp150;_tmp15C.f2=(void*)_tmpE1->unknown_all;_tmp15C;});}}_LLEA: if(*((int*)
_tmpE8)!= 4)goto _LLEC;_tmpFE=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE8)->f1;
_tmpFF=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE8)->f2;if(_tmpFF == 0)goto _LLEC;
_tmp100=*_tmpFF;_tmp101=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE8)->f3;_LLEB: {
struct Cyc_List_List _tmp15E=({struct Cyc_List_List _tmp16D;_tmp16D.hd=_tmp101;
_tmp16D.tl=0;_tmp16D;});struct Cyc_List_List _tmp15F=({struct Cyc_List_List _tmp16C;
_tmp16C.hd=_tmpFE;_tmp16C.tl=(struct Cyc_List_List*)& _tmp15E;_tmp16C;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp161;struct _tuple5 _tmp160=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp15F,1);_tmp161=_tmp160.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp163;struct _tuple7 _tmp162=Cyc_NewControlFlow_anal_Lexp(env,_tmp161,e);_tmp163=
_tmp162.f2;_tmp161=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp161);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp164=_tmp161;struct Cyc_Dict_Dict _tmp165;struct Cyc_List_List*_tmp166;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp167;_LL131: if((_tmp164.ReachableFL).tag != 1)goto _LL133;_tmp165=(_tmp164.ReachableFL).f1;
_tmp166=(_tmp164.ReachableFL).f2;_tmp167=(_tmp164.ReachableFL).f3;_LL132:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp168=_tmp163;struct Cyc_CfFlowInfo_Place*_tmp169;
_LL136: if((_tmp168.PlaceL).tag != 0)goto _LL138;_tmp169=(_tmp168.PlaceL).f1;_LL137:
_tmp167=Cyc_NewControlFlow_consume_assignment(env,_tmp169,_tmp167,_tmp165,_tmpFE);
_tmp166=Cyc_CfFlowInfo_reln_kill_exp(_tmpE1->r,_tmp166,_tmpFE);_tmp165=Cyc_CfFlowInfo_assign_place(
_tmpE1,_tmpFE->loc,_tmp165,env->all_changed,_tmp169,(void*)_tmpE1->unknown_all);
_tmp161=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp16A;(_tmp16A.ReachableFL).tag=1;(_tmp16A.ReachableFL).f1=_tmp165;(_tmp16A.ReachableFL).f2=
_tmp166;(_tmp16A.ReachableFL).f3=_tmp167;_tmp16A;});goto _LL135;_LL138: if((
_tmp168.UnknownL).tag != 1)goto _LL135;_LL139: goto _LL135;_LL135:;}goto _LL130;
_LL133:;_LL134: goto _LL130;_LL130:;}return({struct _tuple5 _tmp16B;_tmp16B.f1=
_tmp161;_tmp16B.f2=(void*)_tmpE1->unknown_all;_tmp16B;});}}_LLEC: if(*((int*)
_tmpE8)!= 4)goto _LLEE;_tmp102=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE8)->f1;
_tmp103=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE8)->f2;if(_tmp103 != 0)goto
_LLEE;_tmp104=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE8)->f3;_LLED: {struct Cyc_Dict_Dict*
_tmp16E=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo _tmp170;union Cyc_CfFlowInfo_FlowInfo_union
_tmp171;struct _tuple6 _tmp16F=Cyc_CfFlowInfo_save_consume_info(_tmpE1,inflow,1);
_tmp170=_tmp16F.f1;_tmp171=_tmp16F.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp18B;_tmp18B.consumed=_tmpE1->mt_place_set;
_tmp18B.may_consume=0;_tmp18B;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp172=_region_malloc(env->r,sizeof(*_tmp172));_tmp172[0]=_tmpE1->mt_place_set;
_tmp172;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp174;union Cyc_CfFlowInfo_AbsLVal_union
_tmp175;struct _tuple7 _tmp173=Cyc_NewControlFlow_anal_Lexp(env,_tmp171,_tmp102);
_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;{struct Cyc_Dict_Dict _tmp176=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp177=
_region_malloc(env->r,sizeof(*_tmp177));_tmp177[0]=_tmpE1->mt_place_set;_tmp177;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp179;void*_tmp17A;struct _tuple5 _tmp178=Cyc_NewControlFlow_anal_Rexp(
env,_tmp171,_tmp104);_tmp179=_tmp178.f1;_tmp17A=_tmp178.f2;{struct Cyc_Dict_Dict
_tmp17B=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp17C=Cyc_CfFlowInfo_after_flow(_tmpE1,(struct Cyc_Dict_Dict*)& _tmp176,_tmp174,
_tmp179,_tmp176,_tmp17B);union Cyc_CfFlowInfo_FlowInfo_union _tmp17D=Cyc_CfFlowInfo_join_flow(
_tmpE1,_tmp16E,_tmp171,_tmp17C,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp17F;struct
_tuple6 _tmp17E=Cyc_CfFlowInfo_save_consume_info(_tmpE1,_tmp17D,0);_tmp17F=
_tmp17E.f1;if(init_consume)_tmp171=Cyc_CfFlowInfo_restore_consume_info(_tmp171,
outflow_consume);outflow_consume=_tmp17F;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp17D,_tmp171)){if(_tmp16E == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp16E,Cyc_CfFlowInfo_union_place_set(
_tmp176,_tmp17B,0),0);}_tmp17C=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp17C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp180=_tmp17C;struct Cyc_Dict_Dict
_tmp181;struct Cyc_List_List*_tmp182;struct Cyc_CfFlowInfo_ConsumeInfo _tmp183;
_LL13B: if((_tmp180.BottomFL).tag != 0)goto _LL13D;_LL13C: return({struct _tuple5
_tmp184;_tmp184.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp185;(_tmp185.BottomFL).tag=0;_tmp185;});_tmp184.f2=_tmp17A;_tmp184;});_LL13D:
if((_tmp180.ReachableFL).tag != 1)goto _LL13A;_tmp181=(_tmp180.ReachableFL).f1;
_tmp182=(_tmp180.ReachableFL).f2;_tmp183=(_tmp180.ReachableFL).f3;_LL13E: _tmp183=
Cyc_CfFlowInfo_and_consume(_tmpE1->r,_tmp170,_tmp183);{union Cyc_CfFlowInfo_AbsLVal_union
_tmp186=_tmp175;struct Cyc_CfFlowInfo_Place*_tmp187;_LL140: if((_tmp186.PlaceL).tag
!= 0)goto _LL142;_tmp187=(_tmp186.PlaceL).f1;_LL141: _tmp183=Cyc_NewControlFlow_consume_assignment(
env,_tmp187,_tmp183,_tmp181,_tmp102);_tmp181=Cyc_CfFlowInfo_assign_place(_tmpE1,
e->loc,_tmp181,env->all_changed,_tmp187,_tmp17A);_tmp182=Cyc_CfFlowInfo_reln_assign_exp(
_tmpE1->r,_tmp182,_tmp102,_tmp104);_tmp17C=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp188;(_tmp188.ReachableFL).tag=1;(_tmp188.ReachableFL).f1=
_tmp181;(_tmp188.ReachableFL).f2=_tmp182;(_tmp188.ReachableFL).f3=_tmp183;
_tmp188;});Cyc_NewControlFlow_update_tryflow(env,_tmp17C);return({struct _tuple5
_tmp189;_tmp189.f1=_tmp17C;_tmp189.f2=_tmp17A;_tmp189;});_LL142: if((_tmp186.UnknownL).tag
!= 1)goto _LL13F;_LL143: return({struct _tuple5 _tmp18A;_tmp18A.f1=Cyc_NewControlFlow_use_Rval(
env,_tmp104->loc,_tmp17C,_tmp17A);_tmp18A.f2=_tmp17A;_tmp18A;});_LL13F:;}_LL13A:;}}
_tmp171=Cyc_CfFlowInfo_restore_consume_info(_tmp17D,empty_consume);}}}}}}}_LLEE:
if(*((int*)_tmpE8)!= 9)goto _LLF0;_tmp105=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpE8)->f1;_tmp106=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpE8)->f2;_LLEF: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp18D;void*_tmp18E;struct _tuple5 _tmp18C=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp105);_tmp18D=_tmp18C.f1;_tmp18E=_tmp18C.f2;_tmp18D=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp105->loc,_tmp18D);return Cyc_NewControlFlow_anal_Rexp(env,_tmp18D,_tmp106);}
_LLF0: if(*((int*)_tmpE8)!= 12)goto _LLF2;_tmp107=((struct Cyc_Absyn_Throw_e_struct*)
_tmpE8)->f1;_LLF1: {union Cyc_CfFlowInfo_FlowInfo_union _tmp190;void*_tmp191;
struct _tuple5 _tmp18F=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp107);_tmp190=
_tmp18F.f1;_tmp191=_tmp18F.f2;_tmp190=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp107->loc,_tmp190);Cyc_NewControlFlow_use_Rval(env,_tmp107->loc,_tmp190,
_tmp191);return({struct _tuple5 _tmp192;_tmp192.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp193;(_tmp193.BottomFL).tag=0;_tmp193;});
_tmp192.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmpE1,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)_tmpE1->unknown_all);_tmp192;});}_LLF2: if(*((int*)
_tmpE8)!= 11)goto _LLF4;_tmp108=((struct Cyc_Absyn_FnCall_e_struct*)_tmpE8)->f1;
_tmp109=((struct Cyc_Absyn_FnCall_e_struct*)_tmpE8)->f2;_LLF3: {struct
_RegionHandle _tmp194=_new_region("temp");struct _RegionHandle*temp=& _tmp194;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp197;struct Cyc_List_List*
_tmp198;struct _tuple11 _tmp196=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp195=_region_malloc(temp,sizeof(*_tmp195));
_tmp195->hd=_tmp108;_tmp195->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp109);_tmp195;}),1);_tmp197=
_tmp196.f1;_tmp198=_tmp196.f2;_tmp197=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp197);{union Cyc_CfFlowInfo_FlowInfo_union _tmp199=Cyc_NewControlFlow_use_Rval(
env,_tmp108->loc,_tmp197,(void*)((struct Cyc_List_List*)_check_null(_tmp198))->hd);
_tmp198=_tmp198->tl;{struct Cyc_List_List*init_params=0;{void*_tmp19A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp108->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp19B;void*_tmp19C;_LL145: if(_tmp19A <= (void*)4)goto _LL147;if(*((int*)_tmp19A)
!= 4)goto _LL147;_tmp19B=((struct Cyc_Absyn_PointerType_struct*)_tmp19A)->f1;
_tmp19C=(void*)_tmp19B.elt_typ;_LL146:{void*_tmp19D=Cyc_Tcutil_compress(_tmp19C);
struct Cyc_Absyn_FnInfo _tmp19E;struct Cyc_List_List*_tmp19F;_LL14A: if(_tmp19D <= (
void*)4)goto _LL14C;if(*((int*)_tmp19D)!= 8)goto _LL14C;_tmp19E=((struct Cyc_Absyn_FnType_struct*)
_tmp19D)->f1;_tmp19F=_tmp19E.attributes;_LL14B: for(0;_tmp19F != 0;_tmp19F=_tmp19F->tl){
void*_tmp1A0=(void*)_tmp19F->hd;int _tmp1A1;_LL14F: if(_tmp1A0 <= (void*)17)goto
_LL151;if(*((int*)_tmp1A0)!= 4)goto _LL151;_tmp1A1=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A0)->f1;_LL150: init_params=({struct Cyc_List_List*_tmp1A2=_region_malloc(
temp,sizeof(*_tmp1A2));_tmp1A2->hd=(void*)_tmp1A1;_tmp1A2->tl=init_params;
_tmp1A2;});goto _LL14E;_LL151:;_LL152: goto _LL14E;_LL14E:;}goto _LL149;_LL14C:;
_LL14D:({void*_tmp1A3[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A4="anal_Rexp: bad function type";
_tag_dynforward(_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,29));}),
_tag_dynforward(_tmp1A3,sizeof(void*),0));});_LL149:;}goto _LL144;_LL147:;_LL148:({
void*_tmp1A5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1A6="anal_Rexp: bad function type";
_tag_dynforward(_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,29));}),
_tag_dynforward(_tmp1A5,sizeof(void*),0));});_LL144:;}{int i=1;for(0;_tmp198 != 0;(((
_tmp198=_tmp198->tl,_tmp109=((struct Cyc_List_List*)_check_null(_tmp109))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
_tmp199=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp109))->hd)->loc,_tmp199,(void*)_tmp198->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A7=_tmp197;struct Cyc_Dict_Dict _tmp1A8;_LL154: if((_tmp1A7.BottomFL).tag != 0)
goto _LL156;_LL155: goto _LL153;_LL156: if((_tmp1A7.ReachableFL).tag != 1)goto _LL153;
_tmp1A8=(_tmp1A7.ReachableFL).f1;_LL157: if(Cyc_CfFlowInfo_initlevel(_tmp1A8,(
void*)_tmp198->hd)== (void*)0)({void*_tmp1A9[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp109))->hd)->loc,({const char*_tmp1AA="expression may not be initialized";
_tag_dynforward(_tmp1AA,sizeof(char),_get_zero_arr_size(_tmp1AA,34));}),
_tag_dynforward(_tmp1A9,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1AB=_tmp199;struct Cyc_Dict_Dict _tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1AE;_LL159: if((_tmp1AB.BottomFL).tag != 0)goto _LL15B;_LL15A: goto _LL158;_LL15B:
if((_tmp1AB.ReachableFL).tag != 1)goto _LL158;_tmp1AC=(_tmp1AB.ReachableFL).f1;
_tmp1AD=(_tmp1AB.ReachableFL).f2;_tmp1AE=(_tmp1AB.ReachableFL).f3;_LL15C: {
struct Cyc_Dict_Dict _tmp1AF=Cyc_CfFlowInfo_escape_deref(_tmpE1,_tmp1AC,env->all_changed,(
void*)_tmp198->hd);{void*_tmp1B0=(void*)_tmp198->hd;struct Cyc_CfFlowInfo_Place*
_tmp1B1;_LL15E: if(_tmp1B0 <= (void*)3)goto _LL160;if(*((int*)_tmp1B0)!= 2)goto
_LL160;_tmp1B1=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1B0)->f1;_LL15F:{
void*_tmp1B2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp109))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1B3;void*_tmp1B4;_LL163: if(_tmp1B2 <= (void*)4)goto
_LL165;if(*((int*)_tmp1B2)!= 4)goto _LL165;_tmp1B3=((struct Cyc_Absyn_PointerType_struct*)
_tmp1B2)->f1;_tmp1B4=(void*)_tmp1B3.elt_typ;_LL164: _tmp1AF=Cyc_CfFlowInfo_assign_place(
_tmpE1,((struct Cyc_Absyn_Exp*)_tmp109->hd)->loc,_tmp1AF,env->all_changed,_tmp1B1,
Cyc_CfFlowInfo_typ_to_absrval(_tmpE1,_tmp1B4,(void*)_tmpE1->esc_all));goto _LL162;
_LL165:;_LL166:({void*_tmp1B5[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B6="anal_Rexp:bad type for initialized arg";
_tag_dynforward(_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,39));}),
_tag_dynforward(_tmp1B5,sizeof(void*),0));});_LL162:;}goto _LL15D;_LL160:;_LL161:
goto _LL15D;_LL15D:;}_tmp199=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B7;(_tmp1B7.ReachableFL).tag=1;(_tmp1B7.ReachableFL).f1=_tmp1AF;(_tmp1B7.ReachableFL).f2=
_tmp1AD;(_tmp1B7.ReachableFL).f3=_tmp1AE;_tmp1B7;});goto _LL158;}_LL158:;}goto
_LL153;_LL153:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp108->topt))->v)){struct _tuple5 _tmp1BA=({struct _tuple5 _tmp1B8;
_tmp1B8.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B9;(_tmp1B9.BottomFL).tag=0;_tmp1B9;});_tmp1B8.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpE1,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpE1->unknown_all);
_tmp1B8;});_npop_handler(0);return _tmp1BA;}else{struct _tuple5 _tmp1BC=({struct
_tuple5 _tmp1BB;_tmp1BB.f1=_tmp199;_tmp1BB.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpE1,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpE1->unknown_all);
_tmp1BB;});_npop_handler(0);return _tmp1BC;}}}};_pop_region(temp);}_LLF4: if(*((
int*)_tmpE8)!= 35)goto _LLF6;_tmp10A=((struct Cyc_Absyn_Malloc_e_struct*)_tmpE8)->f1;
_tmp10B=_tmp10A.is_calloc;_tmp10C=_tmp10A.rgn;_tmp10D=_tmp10A.elt_type;_tmp10E=
_tmp10A.num_elts;_tmp10F=_tmp10A.fat_result;_LLF5: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1CE=_region_malloc(_tmpE1->r,sizeof(*_tmp1CE));_tmp1CE[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1CF;_tmp1CF.tag=1;_tmp1CF.f1=_tmp10E;_tmp1CF.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1CF;});_tmp1CE;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1CD=_region_malloc(_tmpE1->r,sizeof(*_tmp1CD));
_tmp1CD->root=(void*)root;_tmp1CD->fields=0;_tmp1CD;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp1CB=_region_malloc(_tmpE1->r,sizeof(*_tmp1CB));
_tmp1CB[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1CC;_tmp1CC.tag=2;_tmp1CC.f1=
place;_tmp1CC;});_tmp1CB;});void*place_val=_tmp10F?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(
_tmpE1,*((void**)_check_null(_tmp10D)),(void*)_tmpE1->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmp10C != 0){
struct _RegionHandle _tmp1BD=_new_region("temp");struct _RegionHandle*temp=& _tmp1BD;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C0;struct Cyc_List_List*
_tmp1C1;struct _tuple11 _tmp1BF=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1BE[2];_tmp1BE[1]=_tmp10E;_tmp1BE[0]=(struct Cyc_Absyn_Exp*)
_tmp10C;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp1BE,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp1C0=_tmp1BF.f1;_tmp1C1=_tmp1BF.f2;outflow=_tmp1C0;};_pop_region(temp);}
else{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C3;struct _tuple5 _tmp1C2=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp10E);_tmp1C3=_tmp1C2.f1;outflow=_tmp1C3;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp10E->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C4=outflow;struct
Cyc_Dict_Dict _tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1C7;_LL168: if((_tmp1C4.BottomFL).tag != 0)goto _LL16A;_LL169: return({struct
_tuple5 _tmp1C8;_tmp1C8.f1=outflow;_tmp1C8.f2=rval;_tmp1C8;});_LL16A: if((_tmp1C4.ReachableFL).tag
!= 1)goto _LL167;_tmp1C5=(_tmp1C4.ReachableFL).f1;_tmp1C6=(_tmp1C4.ReachableFL).f2;
_tmp1C7=(_tmp1C4.ReachableFL).f3;_LL16B: return({struct _tuple5 _tmp1C9;_tmp1C9.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1CA;(
_tmp1CA.ReachableFL).tag=1;(_tmp1CA.ReachableFL).f1=Cyc_Dict_insert(_tmp1C5,root,
place_val);(_tmp1CA.ReachableFL).f2=_tmp1C6;(_tmp1CA.ReachableFL).f3=_tmp1C7;
_tmp1CA;});_tmp1C9.f2=rval;_tmp1C9;});_LL167:;}}_LLF6: if(*((int*)_tmpE8)!= 36)
goto _LLF8;_tmp110=((struct Cyc_Absyn_Swap_e_struct*)_tmpE8)->f1;_tmp111=((struct
Cyc_Absyn_Swap_e_struct*)_tmpE8)->f2;_LLF7: {void*left_rval;void*right_rval;
union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle _tmp1D0=
_new_region("temp");struct _RegionHandle*temp=& _tmp1D0;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp1D3;struct Cyc_List_List*_tmp1D4;struct _tuple11
_tmp1D2=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp1D1[2];_tmp1D1[1]=_tmp111;_tmp1D1[0]=_tmp110;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp1D1,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1D3=_tmp1D2.f1;_tmp1D4=
_tmp1D2.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1D4))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1D4->tl))->hd;outflow=
_tmp1D3;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp1D6;struct _tuple7 _tmp1D5=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp110);_tmp1D6=_tmp1D5.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1D8;struct _tuple7 _tmp1D7=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp111);
_tmp1D8=_tmp1D7.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D9=outflow;struct Cyc_Dict_Dict
_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1DC;
_LL16D: if((_tmp1D9.ReachableFL).tag != 1)goto _LL16F;_tmp1DA=(_tmp1D9.ReachableFL).f1;
_tmp1DB=(_tmp1D9.ReachableFL).f2;_tmp1DC=(_tmp1D9.ReachableFL).f3;_LL16E:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp1DD=_tmp1D6;struct Cyc_CfFlowInfo_Place*_tmp1DE;
_LL172: if((_tmp1DD.PlaceL).tag != 0)goto _LL174;_tmp1DE=(_tmp1DD.PlaceL).f1;_LL173:
_tmp1DA=Cyc_CfFlowInfo_assign_place(_tmpE1,_tmp110->loc,_tmp1DA,env->all_changed,
_tmp1DE,right_rval);goto _LL171;_LL174: if((_tmp1DD.UnknownL).tag != 1)goto _LL171;
_LL175: goto _LL171;_LL171:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp1DF=_tmp1D8;
struct Cyc_CfFlowInfo_Place*_tmp1E0;_LL177: if((_tmp1DF.PlaceL).tag != 0)goto _LL179;
_tmp1E0=(_tmp1DF.PlaceL).f1;_LL178: _tmp1DA=Cyc_CfFlowInfo_assign_place(_tmpE1,
_tmp111->loc,_tmp1DA,env->all_changed,_tmp1E0,left_rval);goto _LL176;_LL179: if((
_tmp1DF.UnknownL).tag != 1)goto _LL176;_LL17A: goto _LL176;_LL176:;}_tmp1DB=Cyc_CfFlowInfo_reln_kill_exp(
_tmpE1->r,_tmp1DB,_tmp110);_tmp1DB=Cyc_CfFlowInfo_reln_kill_exp(_tmpE1->r,
_tmp1DB,_tmp111);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E1;(_tmp1E1.ReachableFL).tag=1;(_tmp1E1.ReachableFL).f1=_tmp1DA;(_tmp1E1.ReachableFL).f2=
_tmp1DB;(_tmp1E1.ReachableFL).f3=_tmp1DC;_tmp1E1;});goto _LL16C;_LL16F:;_LL170:
goto _LL16C;_LL16C:;}return({struct _tuple5 _tmp1E2;_tmp1E2.f1=outflow;_tmp1E2.f2=(
void*)_tmpE1->unknown_all;_tmp1E2;});}}}_LLF8: if(*((int*)_tmpE8)!= 17)goto _LLFA;
_tmp112=((struct Cyc_Absyn_New_e_struct*)_tmpE8)->f1;_tmp113=((struct Cyc_Absyn_New_e_struct*)
_tmpE8)->f2;_LLF9: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1F5=_region_malloc(_tmpE1->r,sizeof(*_tmp1F5));_tmp1F5[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1F6;_tmp1F6.tag=1;_tmp1F6.f1=_tmp113;_tmp1F6.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1F6;});_tmp1F5;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1F4=_region_malloc(_tmpE1->r,sizeof(*_tmp1F4));
_tmp1F4->root=(void*)root;_tmp1F4->fields=0;_tmp1F4;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp1F2=_region_malloc(_tmpE1->r,sizeof(*_tmp1F2));
_tmp1F2[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1F3;_tmp1F3.tag=2;_tmp1F3.f1=
place;_tmp1F3;});_tmp1F2;});Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmp112 != 
0){struct _RegionHandle _tmp1E3=_new_region("temp");struct _RegionHandle*temp=&
_tmp1E3;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E6;struct Cyc_List_List*
_tmp1E7;struct _tuple11 _tmp1E5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1E4[2];_tmp1E4[1]=_tmp113;_tmp1E4[0]=(struct Cyc_Absyn_Exp*)
_tmp112;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp1E4,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp1E6=_tmp1E5.f1;_tmp1E7=_tmp1E5.f2;outflow=_tmp1E6;place_val=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp1E7))->tl))->hd;};
_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E9;void*_tmp1EA;
struct _tuple5 _tmp1E8=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp113);_tmp1E9=
_tmp1E8.f1;_tmp1EA=_tmp1E8.f2;outflow=_tmp1E9;place_val=_tmp1EA;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp113->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1EB=outflow;struct
Cyc_Dict_Dict _tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1EE;_LL17C: if((_tmp1EB.BottomFL).tag != 0)goto _LL17E;_LL17D: return({struct
_tuple5 _tmp1EF;_tmp1EF.f1=outflow;_tmp1EF.f2=rval;_tmp1EF;});_LL17E: if((_tmp1EB.ReachableFL).tag
!= 1)goto _LL17B;_tmp1EC=(_tmp1EB.ReachableFL).f1;_tmp1ED=(_tmp1EB.ReachableFL).f2;
_tmp1EE=(_tmp1EB.ReachableFL).f3;_LL17F: return({struct _tuple5 _tmp1F0;_tmp1F0.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1F1;(
_tmp1F1.ReachableFL).tag=1;(_tmp1F1.ReachableFL).f1=Cyc_Dict_insert(_tmp1EC,root,
place_val);(_tmp1F1.ReachableFL).f2=_tmp1ED;(_tmp1F1.ReachableFL).f3=_tmp1EE;
_tmp1F1;});_tmp1F0.f2=rval;_tmp1F0;});_LL17B:;}}}_LLFA: if(*((int*)_tmpE8)!= 16)
goto _LLFC;_tmp114=((struct Cyc_Absyn_Address_e_struct*)_tmpE8)->f1;_LLFB: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp1F8;union Cyc_CfFlowInfo_AbsLVal_union _tmp1F9;
struct _tuple7 _tmp1F7=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp114);_tmp1F8=
_tmp1F7.f1;_tmp1F9=_tmp1F7.f2;{union Cyc_CfFlowInfo_AbsLVal_union _tmp1FA=_tmp1F9;
struct Cyc_CfFlowInfo_Place*_tmp1FB;_LL181: if((_tmp1FA.UnknownL).tag != 1)goto
_LL183;_LL182: return({struct _tuple5 _tmp1FC;_tmp1FC.f1=_tmp1F8;_tmp1FC.f2=(void*)
1;_tmp1FC;});_LL183: if((_tmp1FA.PlaceL).tag != 0)goto _LL180;_tmp1FB=(_tmp1FA.PlaceL).f1;
_LL184: return({struct _tuple5 _tmp1FD;_tmp1FD.f1=_tmp1F8;_tmp1FD.f2=(void*)({
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1FE=_region_malloc(env->r,sizeof(*
_tmp1FE));_tmp1FE[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1FF;_tmp1FF.tag=
2;_tmp1FF.f1=_tmp1FB;_tmp1FF;});_tmp1FE;});_tmp1FD;});_LL180:;}}_LLFC: if(*((int*)
_tmpE8)!= 22)goto _LLFE;_tmp115=((struct Cyc_Absyn_Deref_e_struct*)_tmpE8)->f1;
_LLFD: {union Cyc_CfFlowInfo_FlowInfo_union _tmp201;void*_tmp202;struct _tuple5
_tmp200=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp115);_tmp201=_tmp200.f1;
_tmp202=_tmp200.f2;_tmp201=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp201);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp201,_tmp115,_tmp202);}
_LLFE: if(*((int*)_tmpE8)!= 23)goto _LL100;_tmp116=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpE8)->f1;_tmp117=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpE8)->f2;_LLFF: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp204;void*_tmp205;struct _tuple5 _tmp203=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp116);_tmp204=_tmp203.f1;_tmp205=_tmp203.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp116->topt))->v))return({struct
_tuple5 _tmp206;_tmp206.f1=_tmp204;_tmp206.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpE1,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpE1->unknown_all);
_tmp206;});{void*_tmp207=_tmp205;struct _dynforward_ptr _tmp208;_LL186: if(_tmp207
<= (void*)3)goto _LL188;if(*((int*)_tmp207)!= 4)goto _LL188;_tmp208=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp207)->f1;_LL187: {int _tmp209=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp116->topt))->v,_tmp117);return({struct
_tuple5 _tmp20A;_tmp20A.f1=_tmp204;_tmp20A.f2=*((void**)
_check_dynforward_subscript(_tmp208,sizeof(void*),_tmp209));_tmp20A;});}_LL188:;
_LL189:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp20B=_cycalloc(
sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_Core_Impossible_struct _tmp20C;_tmp20C.tag=
Cyc_Core_Impossible;_tmp20C.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp20F;_tmp20F.tag=0;_tmp20F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp20D[1]={& _tmp20F};Cyc_aprintf(({const char*
_tmp20E="anal_Rexp: AggrMember: %s";_tag_dynforward(_tmp20E,sizeof(char),
_get_zero_arr_size(_tmp20E,26));}),_tag_dynforward(_tmp20D,sizeof(void*),1));}});
_tmp20C;});_tmp20B;}));_LL185:;}}_LL100: if(*((int*)_tmpE8)!= 24)goto _LL102;
_tmp118=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpE8)->f1;_tmp119=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE8)->f2;_LL101: {union Cyc_CfFlowInfo_FlowInfo_union _tmp211;void*_tmp212;
struct _tuple5 _tmp210=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp118);_tmp211=
_tmp210.f1;_tmp212=_tmp210.f2;_tmp211=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp211);{union Cyc_CfFlowInfo_FlowInfo_union _tmp214;void*_tmp215;struct _tuple5
_tmp213=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp211,_tmp118,_tmp212);
_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;{void*_tmp216=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp118->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp217;void*_tmp218;_LL18B: if(_tmp216 <= (void*)4)goto _LL18D;if(*((int*)_tmp216)
!= 4)goto _LL18D;_tmp217=((struct Cyc_Absyn_PointerType_struct*)_tmp216)->f1;
_tmp218=(void*)_tmp217.elt_typ;_LL18C: if(Cyc_Absyn_is_union_type(_tmp218))return({
struct _tuple5 _tmp219;_tmp219.f1=_tmp214;_tmp219.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpE1,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpE1->unknown_all);
_tmp219;});{void*_tmp21A=_tmp215;struct _dynforward_ptr _tmp21B;_LL190: if(_tmp21A
<= (void*)3)goto _LL192;if(*((int*)_tmp21A)!= 4)goto _LL192;_tmp21B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp21A)->f1;_LL191: {int _tmp21C=Cyc_NewControlFlow_get_field_index(_tmp218,
_tmp119);return({struct _tuple5 _tmp21D;_tmp21D.f1=_tmp214;_tmp21D.f2=*((void**)
_check_dynforward_subscript(_tmp21B,sizeof(void*),_tmp21C));_tmp21D;});}_LL192:;
_LL193:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp21E=_cycalloc(
sizeof(*_tmp21E));_tmp21E[0]=({struct Cyc_Core_Impossible_struct _tmp21F;_tmp21F.tag=
Cyc_Core_Impossible;_tmp21F.f1=({const char*_tmp220="anal_Rexp: AggrArrow";
_tag_dynforward(_tmp220,sizeof(char),_get_zero_arr_size(_tmp220,21));});_tmp21F;});
_tmp21E;}));_LL18F:;}_LL18D:;_LL18E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221[0]=({struct Cyc_Core_Impossible_struct
_tmp222;_tmp222.tag=Cyc_Core_Impossible;_tmp222.f1=({const char*_tmp223="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,25));});_tmp222;});
_tmp221;}));_LL18A:;}}}_LL102: if(*((int*)_tmpE8)!= 6)goto _LL104;_tmp11A=((struct
Cyc_Absyn_Conditional_e_struct*)_tmpE8)->f1;_tmp11B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpE8)->f2;_tmp11C=((struct Cyc_Absyn_Conditional_e_struct*)_tmpE8)->f3;_LL103: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp225;union Cyc_CfFlowInfo_FlowInfo_union
_tmp226;struct _tuple8 _tmp224=Cyc_NewControlFlow_anal_test(env,inflow,_tmp11A);
_tmp225=_tmp224.f1;_tmp226=_tmp224.f2;_tmp225=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp11A->loc,_tmp225);_tmp226=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp11A->loc,
_tmp226);{struct _tuple5 _tmp227=Cyc_NewControlFlow_anal_Rexp(env,_tmp225,_tmp11B);
struct _tuple5 _tmp228=Cyc_NewControlFlow_anal_Rexp(env,_tmp226,_tmp11C);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpE1,env->all_changed,_tmp227,_tmp228,1);}}_LL104: if(*((int*)_tmpE8)!= 7)goto
_LL106;_tmp11D=((struct Cyc_Absyn_And_e_struct*)_tmpE8)->f1;_tmp11E=((struct Cyc_Absyn_And_e_struct*)
_tmpE8)->f2;_LL105: {union Cyc_CfFlowInfo_FlowInfo_union _tmp22A;union Cyc_CfFlowInfo_FlowInfo_union
_tmp22B;struct _tuple8 _tmp229=Cyc_NewControlFlow_anal_test(env,inflow,_tmp11D);
_tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;_tmp22A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp11D->loc,_tmp22A);_tmp22B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp11D->loc,
_tmp22B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp22D;void*_tmp22E;struct _tuple5
_tmp22C=Cyc_NewControlFlow_anal_Rexp(env,_tmp22A,_tmp11E);_tmp22D=_tmp22C.f1;
_tmp22E=_tmp22C.f2;_tmp22D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp11E->loc,
_tmp22D);{struct _tuple5 _tmp22F=({struct _tuple5 _tmp232;_tmp232.f1=_tmp22D;_tmp232.f2=
_tmp22E;_tmp232;});struct _tuple5 _tmp230=({struct _tuple5 _tmp231;_tmp231.f1=
_tmp22B;_tmp231.f2=(void*)((void*)0);_tmp231;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpE1,env->all_changed,_tmp22F,_tmp230,0);}}}_LL106: if(*((int*)_tmpE8)!= 8)goto
_LL108;_tmp11F=((struct Cyc_Absyn_Or_e_struct*)_tmpE8)->f1;_tmp120=((struct Cyc_Absyn_Or_e_struct*)
_tmpE8)->f2;_LL107: {union Cyc_CfFlowInfo_FlowInfo_union _tmp234;union Cyc_CfFlowInfo_FlowInfo_union
_tmp235;struct _tuple8 _tmp233=Cyc_NewControlFlow_anal_test(env,inflow,_tmp11F);
_tmp234=_tmp233.f1;_tmp235=_tmp233.f2;_tmp234=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp11F->loc,_tmp234);_tmp235=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp11F->loc,
_tmp235);{union Cyc_CfFlowInfo_FlowInfo_union _tmp237;void*_tmp238;struct _tuple5
_tmp236=Cyc_NewControlFlow_anal_Rexp(env,_tmp235,_tmp120);_tmp237=_tmp236.f1;
_tmp238=_tmp236.f2;_tmp237=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp120->loc,
_tmp237);{struct _tuple5 _tmp239=({struct _tuple5 _tmp23C;_tmp23C.f1=_tmp237;_tmp23C.f2=
_tmp238;_tmp23C;});struct _tuple5 _tmp23A=({struct _tuple5 _tmp23B;_tmp23B.f1=
_tmp234;_tmp23B.f2=(void*)((void*)1);_tmp23B;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpE1,env->all_changed,_tmp239,_tmp23A,0);}}}_LL108: if(*((int*)_tmpE8)!= 25)
goto _LL10A;_tmp121=((struct Cyc_Absyn_Subscript_e_struct*)_tmpE8)->f1;_tmp122=((
struct Cyc_Absyn_Subscript_e_struct*)_tmpE8)->f2;_LL109: {struct _RegionHandle
_tmp23D=_new_region("temp");struct _RegionHandle*temp=& _tmp23D;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp240;struct Cyc_List_List*_tmp241;struct
_tuple11 _tmp23F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct
Cyc_Absyn_Exp*_tmp23E[2];_tmp23E[1]=_tmp122;_tmp23E[0]=_tmp121;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp23E,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp240=_tmp23F.f1;_tmp241=
_tmp23F.f2;_tmp240=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp122->loc,_tmp240);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp242=_tmp240;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp243=_tmp240;struct Cyc_Dict_Dict _tmp244;struct Cyc_List_List*_tmp245;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp246;_LL195: if((_tmp243.ReachableFL).tag != 1)goto _LL197;_tmp244=(_tmp243.ReachableFL).f1;
_tmp245=(_tmp243.ReachableFL).f2;_tmp246=(_tmp243.ReachableFL).f3;_LL196: if(Cyc_CfFlowInfo_initlevel(
_tmp244,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp241))->tl))->hd)== (void*)0)({void*_tmp247[0]={};Cyc_Tcutil_terr(
_tmp122->loc,({const char*_tmp248="expression may not be initialized";
_tag_dynforward(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,34));}),
_tag_dynforward(_tmp247,sizeof(void*),0));});{struct Cyc_List_List*_tmp249=Cyc_NewControlFlow_add_subscript_reln(
_tmpE1->r,_tmp245,_tmp121,_tmp122);if(_tmp245 != _tmp249)_tmp242=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp24A;(_tmp24A.ReachableFL).tag=1;(_tmp24A.ReachableFL).f1=
_tmp244;(_tmp24A.ReachableFL).f2=_tmp249;(_tmp24A.ReachableFL).f3=_tmp246;
_tmp24A;});goto _LL194;}_LL197:;_LL198: goto _LL194;_LL194:;}{void*_tmp24B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp121->topt))->v);struct Cyc_List_List*
_tmp24C;struct Cyc_Absyn_PtrInfo _tmp24D;struct Cyc_Absyn_PtrAtts _tmp24E;struct Cyc_Absyn_Conref*
_tmp24F;_LL19A: if(_tmp24B <= (void*)4)goto _LL19E;if(*((int*)_tmp24B)!= 9)goto
_LL19C;_tmp24C=((struct Cyc_Absyn_TupleType_struct*)_tmp24B)->f1;_LL19B: {void*
_tmp250=(void*)((struct Cyc_List_List*)_check_null(_tmp241))->hd;struct
_dynforward_ptr _tmp251;_LL1A1: if(_tmp250 <= (void*)3)goto _LL1A3;if(*((int*)
_tmp250)!= 4)goto _LL1A3;_tmp251=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp250)->f1;
_LL1A2: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp122)).f1;struct _tuple5
_tmp253=({struct _tuple5 _tmp252;_tmp252.f1=_tmp242;_tmp252.f2=*((void**)
_check_dynforward_subscript(_tmp251,sizeof(void*),(int)i));_tmp252;});
_npop_handler(0);return _tmp253;}_LL1A3:;_LL1A4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254[0]=({struct Cyc_Core_Impossible_struct
_tmp255;_tmp255.tag=Cyc_Core_Impossible;_tmp255.f1=({const char*_tmp256="anal_Rexp: Subscript";
_tag_dynforward(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,21));});_tmp255;});
_tmp254;}));_LL1A0:;}_LL19C: if(*((int*)_tmp24B)!= 4)goto _LL19E;_tmp24D=((struct
Cyc_Absyn_PointerType_struct*)_tmp24B)->f1;_tmp24E=_tmp24D.ptr_atts;_tmp24F=
_tmp24E.bounds;_LL19D:{union Cyc_Absyn_Constraint_union _tmp257=(Cyc_Absyn_compress_conref(
_tmp24F))->v;void*_tmp258;_LL1A6: if((_tmp257.Eq_constr).tag != 0)goto _LL1A8;
_tmp258=(_tmp257.Eq_constr).f1;if(_tmp258 <= (void*)2)goto _LL1A8;if(*((int*)
_tmp258)!= 1)goto _LL1A8;_LL1A7:{void*_tmp259=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp241))->tl))->hd;struct Cyc_List_List*
_tmp25A;_LL1AB: if(_tmp259 <= (void*)3)goto _LL1AD;if(*((int*)_tmp259)!= 3)goto
_LL1AD;_tmp25A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp259)->f1;_LL1AC:(void*)(
_tmp122->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp25B=
_cycalloc(sizeof(*_tmp25B));_tmp25B[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp25C;_tmp25C.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp25C.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp25A);_tmp25C;});_tmp25B;})));goto _LL1AA;_LL1AD:;_LL1AE:
goto _LL1AA;_LL1AA:;}goto _LL1A5;_LL1A8:;_LL1A9: goto _LL1A5;_LL1A5:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp25E;void*_tmp25F;struct _tuple5 _tmp25D=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp240,_tmp121,(void*)((struct Cyc_List_List*)_check_null(_tmp241))->hd);
_tmp25E=_tmp25D.f1;_tmp25F=_tmp25D.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp260=
_tmp25E;_LL1B0: if((_tmp260.BottomFL).tag != 0)goto _LL1B2;_LL1B1: {struct _tuple5
_tmp262=({struct _tuple5 _tmp261;_tmp261.f1=_tmp25E;_tmp261.f2=_tmp25F;_tmp261;});
_npop_handler(0);return _tmp262;}_LL1B2:;_LL1B3: {struct _tuple5 _tmp264=({struct
_tuple5 _tmp263;_tmp263.f1=_tmp242;_tmp263.f2=_tmp25F;_tmp263;});_npop_handler(0);
return _tmp264;}_LL1AF:;}}_LL19E:;_LL19F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Core_Impossible_struct
_tmp266;_tmp266.tag=Cyc_Core_Impossible;_tmp266.f1=({const char*_tmp267="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,33));});_tmp266;});
_tmp265;}));_LL199:;}}};_pop_region(temp);}_LL10A: if(*((int*)_tmpE8)!= 32)goto
_LL10C;_tmp123=((struct Cyc_Absyn_Tunion_e_struct*)_tmpE8)->f1;_tmp124=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpE8)->f2;_LL10B: if(_tmp124->is_flat){struct
_RegionHandle _tmp268=_new_region("temp");struct _RegionHandle*temp=& _tmp268;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp26A;struct Cyc_List_List*
_tmp26B;struct _tuple11 _tmp269=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,_tmp123,0);_tmp26A=_tmp269.f1;_tmp26B=_tmp269.f2;_tmp26A=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp26A);for(0;(unsigned int)_tmp123;(_tmp123=_tmp123->tl,_tmp26B=_tmp26B->tl)){
_tmp26A=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp123->hd)->loc,
_tmp26A,(void*)((struct Cyc_List_List*)_check_null(_tmp26B))->hd);}{struct _tuple5
_tmp26D=({struct _tuple5 _tmp26C;_tmp26C.f1=_tmp26A;_tmp26C.f2=(void*)_tmpE1->unknown_all;
_tmp26C;});_npop_handler(0);return _tmp26D;}};_pop_region(temp);}_tmp125=_tmp123;
goto _LL10D;_LL10C: if(*((int*)_tmpE8)!= 26)goto _LL10E;_tmp125=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpE8)->f1;_LL10D: {struct _RegionHandle _tmp26E=_new_region("temp");struct
_RegionHandle*temp=& _tmp26E;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp270;struct Cyc_List_List*_tmp271;struct _tuple11 _tmp26F=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp125,0);_tmp270=_tmp26F.f1;_tmp271=_tmp26F.f2;_tmp270=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp270);{struct _dynforward_ptr aggrdict=({unsigned int _tmp276=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp125);void**
_tmp277=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp276));
struct _dynforward_ptr _tmp27A=_tag_dynforward(_tmp277,sizeof(void*),_tmp276);{
unsigned int _tmp278=_tmp276;unsigned int i;for(i=0;i < _tmp278;i ++){_tmp277[i]=({
void*_tmp279=(void*)((struct Cyc_List_List*)_check_null(_tmp271))->hd;_tmp271=
_tmp271->tl;_tmp279;});}}_tmp27A;});struct _tuple5 _tmp275=({struct _tuple5 _tmp272;
_tmp272.f1=_tmp270;_tmp272.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp273=_region_malloc(env->r,sizeof(*_tmp273));_tmp273[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp274;_tmp274.tag=4;_tmp274.f1=aggrdict;_tmp274;});_tmp273;});_tmp272;});
_npop_handler(0);return _tmp275;}};_pop_region(temp);}_LL10E: if(*((int*)_tmpE8)!= 
31)goto _LL110;_tmp126=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpE8)->f2;_LL10F: {
struct Cyc_List_List*fs;{void*_tmp27B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_List_List*_tmp27C;_LL1B5: if(_tmp27B <= (void*)
4)goto _LL1B7;if(*((int*)_tmp27B)!= 11)goto _LL1B7;_tmp27C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp27B)->f2;_LL1B6: fs=_tmp27C;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D[0]=({
struct Cyc_Core_Impossible_struct _tmp27E;_tmp27E.tag=Cyc_Core_Impossible;_tmp27E.f1=({
const char*_tmp27F="anal_Rexp:anon struct has bad type";_tag_dynforward(_tmp27F,
sizeof(char),_get_zero_arr_size(_tmp27F,35));});_tmp27E;});_tmp27D;}));_LL1B4:;}
_tmp127=_tmp126;_tmp12C=fs;goto _LL111;}_LL110: if(*((int*)_tmpE8)!= 30)goto _LL112;
_tmp127=((struct Cyc_Absyn_Struct_e_struct*)_tmpE8)->f3;_tmp128=((struct Cyc_Absyn_Struct_e_struct*)
_tmpE8)->f4;if(_tmp128 == 0)goto _LL112;_tmp129=*_tmp128;_tmp12A=_tmp129.impl;if(
_tmp12A == 0)goto _LL112;_tmp12B=*_tmp12A;_tmp12C=_tmp12B.fields;_LL111: {struct
_RegionHandle _tmp280=_new_region("temp");struct _RegionHandle*temp=& _tmp280;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp282;struct Cyc_List_List*
_tmp283;struct _tuple11 _tmp281=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple13*))Cyc_Core_snd,_tmp127),0);_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;
_tmp282=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp282);{struct
_dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmpE1,_tmp12C,(
void*)_tmpE1->unknown_all);{int i=0;for(0;_tmp283 != 0;(((_tmp283=_tmp283->tl,
_tmp127=_tmp127->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple13*)((struct
Cyc_List_List*)_check_null(_tmp127))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp284=(void*)ds->hd;struct _dynforward_ptr*_tmp285;_LL1BA: if(*((int*)_tmp284)!= 
0)goto _LL1BC;_LL1BB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286[0]=({struct Cyc_Core_Impossible_struct
_tmp287;_tmp287.tag=Cyc_Core_Impossible;_tmp287.f1=({const char*_tmp288="anal_Rexp:Struct_e";
_tag_dynforward(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,19));});_tmp287;});
_tmp286;}));_LL1BC: if(*((int*)_tmp284)!= 1)goto _LL1B9;_tmp285=((struct Cyc_Absyn_FieldName_struct*)
_tmp284)->f1;_LL1BD: {int _tmp289=Cyc_NewControlFlow_get_field_index_fs(_tmp12C,
_tmp285);*((void**)_check_dynforward_subscript(aggrdict,sizeof(void*),_tmp289))=(
void*)_tmp283->hd;}_LL1B9:;}}}{struct _tuple5 _tmp28D=({struct _tuple5 _tmp28A;
_tmp28A.f1=_tmp282;_tmp28A.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp28B=_region_malloc(env->r,sizeof(*_tmp28B));_tmp28B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp28C;_tmp28C.tag=4;_tmp28C.f1=aggrdict;_tmp28C;});_tmp28B;});_tmp28A;});
_npop_handler(0);return _tmp28D;}}};_pop_region(temp);}_LL112: if(*((int*)_tmpE8)
!= 30)goto _LL114;_LL113:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E[0]=({struct Cyc_Core_Impossible_struct
_tmp28F;_tmp28F.tag=Cyc_Core_Impossible;_tmp28F.f1=({const char*_tmp290="anal_Rexp:missing aggrdeclimpl";
_tag_dynforward(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,31));});_tmp28F;});
_tmp28E;}));_LL114: if(*((int*)_tmpE8)!= 28)goto _LL116;_tmp12D=((struct Cyc_Absyn_Array_e_struct*)
_tmpE8)->f1;_LL115: {struct _RegionHandle _tmp291=_new_region("temp");struct
_RegionHandle*temp=& _tmp291;_push_region(temp);{struct Cyc_List_List*_tmp292=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple13*))Cyc_Core_snd,_tmp12D);union Cyc_CfFlowInfo_FlowInfo_union _tmp294;
struct Cyc_List_List*_tmp295;struct _tuple11 _tmp293=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp292,0);_tmp294=_tmp293.f1;_tmp295=_tmp293.f2;_tmp294=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp294);for(0;_tmp295 != 0;(_tmp295=_tmp295->tl,_tmp292=_tmp292->tl)){
_tmp294=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp292))->hd)->loc,_tmp294,(void*)_tmp295->hd);}{struct _tuple5
_tmp297=({struct _tuple5 _tmp296;_tmp296.f1=_tmp294;_tmp296.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpE1,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpE1->unknown_all);
_tmp296;});_npop_handler(0);return _tmp297;}};_pop_region(temp);}_LL116: if(*((int*)
_tmpE8)!= 29)goto _LL118;_tmp12E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpE8)->f1;
_tmp12F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpE8)->f2;_tmp130=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmpE8)->f3;_tmp131=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpE8)->f4;_LL117: {union Cyc_CfFlowInfo_FlowInfo_union _tmp299;void*_tmp29A;
struct _tuple5 _tmp298=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp12F);_tmp299=
_tmp298.f1;_tmp29A=_tmp298.f2;_tmp299=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp12F->loc,_tmp299);{union Cyc_CfFlowInfo_FlowInfo_union _tmp29B=_tmp299;struct
Cyc_Dict_Dict _tmp29C;struct Cyc_List_List*_tmp29D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp29E;_LL1BF: if((_tmp29B.BottomFL).tag != 0)goto _LL1C1;_LL1C0: return({struct
_tuple5 _tmp29F;_tmp29F.f1=_tmp299;_tmp29F.f2=(void*)_tmpE1->unknown_all;_tmp29F;});
_LL1C1: if((_tmp29B.ReachableFL).tag != 1)goto _LL1BE;_tmp29C=(_tmp29B.ReachableFL).f1;
_tmp29D=(_tmp29B.ReachableFL).f2;_tmp29E=(_tmp29B.ReachableFL).f3;_LL1C2: if(Cyc_CfFlowInfo_initlevel(
_tmp29C,_tmp29A)== (void*)0)({void*_tmp2A0[0]={};Cyc_Tcutil_terr(_tmp12F->loc,({
const char*_tmp2A1="expression may not be initialized";_tag_dynforward(_tmp2A1,
sizeof(char),_get_zero_arr_size(_tmp2A1,34));}),_tag_dynforward(_tmp2A0,sizeof(
void*),0));});{struct Cyc_List_List*new_relns=_tmp29D;comp_loop: {void*_tmp2A2=(
void*)_tmp12F->r;struct Cyc_Absyn_Exp*_tmp2A3;void*_tmp2A4;struct Cyc_Absyn_Vardecl*
_tmp2A5;void*_tmp2A6;struct Cyc_Absyn_Vardecl*_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_Vardecl*
_tmp2A9;void*_tmp2AA;struct Cyc_Absyn_Vardecl*_tmp2AB;union Cyc_Absyn_Cnst_union
_tmp2AC;int _tmp2AD;void*_tmp2AE;struct Cyc_List_List*_tmp2AF;struct Cyc_List_List
_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B1;_LL1C4: if(*((int*)_tmp2A2)!= 15)goto _LL1C6;
_tmp2A3=((struct Cyc_Absyn_Cast_e_struct*)_tmp2A2)->f2;_LL1C5: _tmp12F=_tmp2A3;
goto comp_loop;_LL1C6: if(*((int*)_tmp2A2)!= 1)goto _LL1C8;_tmp2A4=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp2A2)->f2;if(_tmp2A4 <= (void*)1)goto _LL1C8;if(*((int*)
_tmp2A4)!= 0)goto _LL1C8;_tmp2A5=((struct Cyc_Absyn_Global_b_struct*)_tmp2A4)->f1;
if(!(!_tmp2A5->escapes))goto _LL1C8;_LL1C7: _tmp2A7=_tmp2A5;goto _LL1C9;_LL1C8: if(*((
int*)_tmp2A2)!= 1)goto _LL1CA;_tmp2A6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A2)->f2;if(_tmp2A6 <= (void*)1)goto _LL1CA;if(*((int*)_tmp2A6)!= 3)goto _LL1CA;
_tmp2A7=((struct Cyc_Absyn_Local_b_struct*)_tmp2A6)->f1;if(!(!_tmp2A7->escapes))
goto _LL1CA;_LL1C9: _tmp2A9=_tmp2A7;goto _LL1CB;_LL1CA: if(*((int*)_tmp2A2)!= 1)goto
_LL1CC;_tmp2A8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A2)->f2;if(_tmp2A8 <= (
void*)1)goto _LL1CC;if(*((int*)_tmp2A8)!= 4)goto _LL1CC;_tmp2A9=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2A8)->f1;if(!(!_tmp2A9->escapes))goto _LL1CC;_LL1CB: _tmp2AB=_tmp2A9;goto
_LL1CD;_LL1CC: if(*((int*)_tmp2A2)!= 1)goto _LL1CE;_tmp2AA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A2)->f2;if(_tmp2AA <= (void*)1)goto _LL1CE;if(*((int*)_tmp2AA)!= 2)goto _LL1CE;
_tmp2AB=((struct Cyc_Absyn_Param_b_struct*)_tmp2AA)->f1;if(!(!_tmp2AB->escapes))
goto _LL1CE;_LL1CD: new_relns=({struct Cyc_List_List*_tmp2B2=_region_malloc(env->r,
sizeof(*_tmp2B2));_tmp2B2->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2B3=_region_malloc(
env->r,sizeof(*_tmp2B3));_tmp2B3->vd=_tmp12E;_tmp2B3->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2B4;(_tmp2B4.LessVar).tag=1;(_tmp2B4.LessVar).f1=
_tmp2AB;_tmp2B4;});_tmp2B3;});_tmp2B2->tl=_tmp29D;_tmp2B2;});goto _LL1C3;_LL1CE:
if(*((int*)_tmp2A2)!= 0)goto _LL1D0;_tmp2AC=((struct Cyc_Absyn_Const_e_struct*)
_tmp2A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2A2)->f1).Int_c).tag != 2)
goto _LL1D0;_tmp2AD=(_tmp2AC.Int_c).f2;_LL1CF: new_relns=({struct Cyc_List_List*
_tmp2B5=_region_malloc(env->r,sizeof(*_tmp2B5));_tmp2B5->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2B6=_region_malloc(env->r,sizeof(*_tmp2B6));_tmp2B6->vd=_tmp12E;_tmp2B6->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2B7;(
_tmp2B7.LessConst).tag=3;(_tmp2B7.LessConst).f1=(unsigned int)_tmp2AD;_tmp2B7;});
_tmp2B6;});_tmp2B5->tl=_tmp29D;_tmp2B5;});goto _LL1C3;_LL1D0: if(*((int*)_tmp2A2)
!= 3)goto _LL1D2;_tmp2AE=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2A2)->f1;
if((int)_tmp2AE != 19)goto _LL1D2;_tmp2AF=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2A2)->f2;if(_tmp2AF == 0)goto _LL1D2;_tmp2B0=*_tmp2AF;_tmp2B1=(struct Cyc_Absyn_Exp*)
_tmp2B0.hd;_LL1D1:{void*_tmp2B8=(void*)_tmp2B1->r;void*_tmp2B9;struct Cyc_Absyn_Vardecl*
_tmp2BA;void*_tmp2BB;struct Cyc_Absyn_Vardecl*_tmp2BC;void*_tmp2BD;struct Cyc_Absyn_Vardecl*
_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C0;_LL1D5: if(*((int*)_tmp2B8)
!= 1)goto _LL1D7;_tmp2B9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B8)->f2;if(
_tmp2B9 <= (void*)1)goto _LL1D7;if(*((int*)_tmp2B9)!= 0)goto _LL1D7;_tmp2BA=((
struct Cyc_Absyn_Global_b_struct*)_tmp2B9)->f1;if(!(!_tmp2BA->escapes))goto _LL1D7;
_LL1D6: _tmp2BC=_tmp2BA;goto _LL1D8;_LL1D7: if(*((int*)_tmp2B8)!= 1)goto _LL1D9;
_tmp2BB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B8)->f2;if(_tmp2BB <= (void*)
1)goto _LL1D9;if(*((int*)_tmp2BB)!= 3)goto _LL1D9;_tmp2BC=((struct Cyc_Absyn_Local_b_struct*)
_tmp2BB)->f1;if(!(!_tmp2BC->escapes))goto _LL1D9;_LL1D8: _tmp2BE=_tmp2BC;goto
_LL1DA;_LL1D9: if(*((int*)_tmp2B8)!= 1)goto _LL1DB;_tmp2BD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B8)->f2;if(_tmp2BD <= (void*)1)goto _LL1DB;if(*((int*)_tmp2BD)!= 4)goto _LL1DB;
_tmp2BE=((struct Cyc_Absyn_Pat_b_struct*)_tmp2BD)->f1;if(!(!_tmp2BE->escapes))
goto _LL1DB;_LL1DA: _tmp2C0=_tmp2BE;goto _LL1DC;_LL1DB: if(*((int*)_tmp2B8)!= 1)goto
_LL1DD;_tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B8)->f2;if(_tmp2BF <= (
void*)1)goto _LL1DD;if(*((int*)_tmp2BF)!= 2)goto _LL1DD;_tmp2C0=((struct Cyc_Absyn_Param_b_struct*)
_tmp2BF)->f1;if(!(!_tmp2C0->escapes))goto _LL1DD;_LL1DC: new_relns=({struct Cyc_List_List*
_tmp2C1=_region_malloc(env->r,sizeof(*_tmp2C1));_tmp2C1->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2C2=_region_malloc(env->r,sizeof(*_tmp2C2));_tmp2C2->vd=_tmp12E;_tmp2C2->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2C3;(
_tmp2C3.LessSize).tag=2;(_tmp2C3.LessSize).f1=_tmp2C0;_tmp2C3;});_tmp2C2;});
_tmp2C1->tl=_tmp29D;_tmp2C1;});goto _LL1D4;_LL1DD:;_LL1DE: goto _LL1D4;_LL1D4:;}
goto _LL1C3;_LL1D2:;_LL1D3: goto _LL1C3;_LL1C3:;}if(_tmp29D != new_relns)_tmp299=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp2C4;(
_tmp2C4.ReachableFL).tag=1;(_tmp2C4.ReachableFL).f1=_tmp29C;(_tmp2C4.ReachableFL).f2=
new_relns;(_tmp2C4.ReachableFL).f3=_tmp29E;_tmp2C4;});{void*_tmp2C5=_tmp29A;
_LL1E0: if((int)_tmp2C5 != 0)goto _LL1E2;_LL1E1: return({struct _tuple5 _tmp2C6;
_tmp2C6.f1=_tmp299;_tmp2C6.f2=(void*)_tmpE1->unknown_all;_tmp2C6;});_LL1E2: if((
int)_tmp2C5 != 2)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp2C5 != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp2C5 <= (void*)3)goto _LL1E8;if(*((int*)
_tmp2C5)!= 2)goto _LL1E8;_LL1E7: {struct Cyc_List_List _tmp2C7=({struct Cyc_List_List
_tmp2D3;_tmp2D3.hd=_tmp12E;_tmp2D3.tl=0;_tmp2D3;});_tmp299=Cyc_NewControlFlow_add_vars(
_tmpE1,_tmp299,(struct Cyc_List_List*)& _tmp2C7,(void*)_tmpE1->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp2C9;void*_tmp2CA;struct _tuple5 _tmp2C8=Cyc_NewControlFlow_anal_Rexp(
env,_tmp299,_tmp130);_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;_tmp2C9=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp130->loc,_tmp2C9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2CB=_tmp2C9;struct
Cyc_Dict_Dict _tmp2CC;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2CD;_LL1EB: if((_tmp2CB.BottomFL).tag
!= 0)goto _LL1ED;_LL1EC: return({struct _tuple5 _tmp2CE;_tmp2CE.f1=_tmp2C9;_tmp2CE.f2=(
void*)_tmpE1->unknown_all;_tmp2CE;});_LL1ED: if((_tmp2CB.ReachableFL).tag != 1)
goto _LL1EA;_tmp2CC=(_tmp2CB.ReachableFL).f1;_tmp2CD=(_tmp2CB.ReachableFL).f3;
_LL1EE: if(Cyc_CfFlowInfo_initlevel(_tmp2CC,_tmp2CA)!= (void*)2){({void*_tmp2CF[0]={};
Cyc_Tcutil_terr(_tmp12F->loc,({const char*_tmp2D0="expression may not be initialized";
_tag_dynforward(_tmp2D0,sizeof(char),_get_zero_arr_size(_tmp2D0,34));}),
_tag_dynforward(_tmp2CF,sizeof(void*),0));});return({struct _tuple5 _tmp2D1;
_tmp2D1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2D2;(_tmp2D2.BottomFL).tag=0;_tmp2D2;});_tmp2D1.f2=(void*)_tmpE1->unknown_all;
_tmp2D1;});}_LL1EA:;}_tmp299=_tmp2C9;goto _LL1E9;}}_LL1E8:;_LL1E9: while(1){struct
Cyc_List_List _tmp2D4=({struct Cyc_List_List _tmp2E0;_tmp2E0.hd=_tmp12E;_tmp2E0.tl=
0;_tmp2E0;});_tmp299=Cyc_NewControlFlow_add_vars(_tmpE1,_tmp299,(struct Cyc_List_List*)&
_tmp2D4,(void*)_tmpE1->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2D6;void*_tmp2D7;struct _tuple5 _tmp2D5=Cyc_NewControlFlow_anal_Rexp(env,
_tmp299,_tmp130);_tmp2D6=_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;_tmp2D6=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp130->loc,_tmp2D6);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2D8=_tmp2D6;struct
Cyc_Dict_Dict _tmp2D9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2DA;_LL1F0: if((_tmp2D8.BottomFL).tag
!= 0)goto _LL1F2;_LL1F1: goto _LL1EF;_LL1F2: if((_tmp2D8.ReachableFL).tag != 1)goto
_LL1EF;_tmp2D9=(_tmp2D8.ReachableFL).f1;_tmp2DA=(_tmp2D8.ReachableFL).f3;_LL1F3:
if(Cyc_CfFlowInfo_initlevel(_tmp2D9,_tmp2D7)!= (void*)2){({void*_tmp2DB[0]={};
Cyc_Tcutil_terr(_tmp12F->loc,({const char*_tmp2DC="expression may not be initialized";
_tag_dynforward(_tmp2DC,sizeof(char),_get_zero_arr_size(_tmp2DC,34));}),
_tag_dynforward(_tmp2DB,sizeof(void*),0));});return({struct _tuple5 _tmp2DD;
_tmp2DD.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DE;(_tmp2DE.BottomFL).tag=0;_tmp2DE;});_tmp2DD.f2=(void*)_tmpE1->unknown_all;
_tmp2DD;});}_LL1EF:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DF=Cyc_CfFlowInfo_join_flow(
_tmpE1,env->all_changed,_tmp299,_tmp2D6,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp2DF,_tmp299))break;_tmp299=_tmp2DF;}}}return({struct _tuple5 _tmp2E1;_tmp2E1.f1=
_tmp299;_tmp2E1.f2=(void*)_tmpE1->unknown_all;_tmp2E1;});_LL1DF:;}}_LL1BE:;}}
_LL118: if(*((int*)_tmpE8)!= 38)goto _LL11A;_tmp132=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpE8)->f1;_LL119: while(1){union Cyc_CfFlowInfo_FlowInfo_union*_tmp2E3;struct
_tuple9 _tmp2E2=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp132);_tmp2E3=
_tmp2E2.f2;inflow=*_tmp2E3;{void*_tmp2E4=(void*)_tmp132->r;struct Cyc_Absyn_Stmt*
_tmp2E5;struct Cyc_Absyn_Stmt*_tmp2E6;struct Cyc_Absyn_Decl*_tmp2E7;struct Cyc_Absyn_Stmt*
_tmp2E8;struct Cyc_Absyn_Exp*_tmp2E9;_LL1F5: if(_tmp2E4 <= (void*)1)goto _LL1FB;if(*((
int*)_tmp2E4)!= 1)goto _LL1F7;_tmp2E5=((struct Cyc_Absyn_Seq_s_struct*)_tmp2E4)->f1;
_tmp2E6=((struct Cyc_Absyn_Seq_s_struct*)_tmp2E4)->f2;_LL1F6: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp2E5);_tmp132=_tmp2E6;goto _LL1F4;_LL1F7: if(*((int*)_tmp2E4)!= 11)
goto _LL1F9;_tmp2E7=((struct Cyc_Absyn_Decl_s_struct*)_tmp2E4)->f1;_tmp2E8=((
struct Cyc_Absyn_Decl_s_struct*)_tmp2E4)->f2;_LL1F8: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp2E7);_tmp132=_tmp2E8;goto _LL1F4;_LL1F9: if(*((int*)_tmp2E4)!= 0)
goto _LL1FB;_tmp2E9=((struct Cyc_Absyn_Exp_s_struct*)_tmp2E4)->f1;_LL1FA: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp2E9);_LL1FB:;_LL1FC:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA[0]=({struct Cyc_Core_Impossible_struct
_tmp2EB;_tmp2EB.tag=Cyc_Core_Impossible;_tmp2EB.f1=({const char*_tmp2EC="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,33));});_tmp2EB;});
_tmp2EA;}));_LL1F4:;}}_LL11A: if(*((int*)_tmpE8)!= 1)goto _LL11C;_tmp133=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpE8)->f2;if((int)_tmp133 != 0)goto _LL11C;_LL11B:
goto _LL11D;_LL11C: if(*((int*)_tmpE8)!= 2)goto _LL11E;_LL11D: goto _LL11F;_LL11E: if(*((
int*)_tmpE8)!= 10)goto _LL120;_LL11F: goto _LL121;_LL120: if(*((int*)_tmpE8)!= 37)
goto _LL122;_LL121: goto _LL123;_LL122: if(*((int*)_tmpE8)!= 27)goto _LLBD;_LL123:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2ED=_cycalloc(sizeof(*
_tmp2ED));_tmp2ED[0]=({struct Cyc_Core_Impossible_struct _tmp2EE;_tmp2EE.tag=Cyc_Core_Impossible;
_tmp2EE.f1=({const char*_tmp2EF="anal_Rexp, unexpected exp form";_tag_dynforward(
_tmp2EF,sizeof(char),_get_zero_arr_size(_tmp2EF,31));});_tmp2EE;});_tmp2ED;}));
_LLBD:;}}static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,
struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*
_tmp2F0=env->fenv;void*_tmp2F1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp2F2;void*_tmp2F3;struct Cyc_Absyn_PtrAtts
_tmp2F4;struct Cyc_Absyn_Conref*_tmp2F5;struct Cyc_Absyn_Conref*_tmp2F6;_LL1FE: if(
_tmp2F1 <= (void*)4)goto _LL200;if(*((int*)_tmp2F1)!= 4)goto _LL200;_tmp2F2=((
struct Cyc_Absyn_PointerType_struct*)_tmp2F1)->f1;_tmp2F3=(void*)_tmp2F2.elt_typ;
_tmp2F4=_tmp2F2.ptr_atts;_tmp2F5=_tmp2F4.bounds;_tmp2F6=_tmp2F4.zero_term;_LL1FF: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp2F7=f;struct Cyc_Dict_Dict _tmp2F8;struct Cyc_List_List*
_tmp2F9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2FA;_LL203: if((_tmp2F7.BottomFL).tag
!= 0)goto _LL205;_LL204: return({struct _tuple7 _tmp2FB;_tmp2FB.f1=f;_tmp2FB.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp2FC;(
_tmp2FC.UnknownL).tag=1;_tmp2FC;});_tmp2FB;});_LL205: if((_tmp2F7.ReachableFL).tag
!= 1)goto _LL202;_tmp2F8=(_tmp2F7.ReachableFL).f1;_tmp2F9=(_tmp2F7.ReachableFL).f2;
_tmp2FA=(_tmp2F7.ReachableFL).f3;_LL206: if(Cyc_Tcutil_is_bound_one(_tmp2F5)){
void*_tmp2FD=r;struct Cyc_CfFlowInfo_Place*_tmp2FE;struct Cyc_CfFlowInfo_Place
_tmp2FF;void*_tmp300;struct Cyc_List_List*_tmp301;void*_tmp302;_LL208: if((int)
_tmp2FD != 1)goto _LL20A;_LL209: goto _LL20B;_LL20A: if((int)_tmp2FD != 2)goto _LL20C;
_LL20B:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp304;_tmp304.tag=Cyc_CfFlowInfo_NotZero;_tmp304.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp2F9);_tmp304;});_tmp303;})));goto _LL207;_LL20C: if(
_tmp2FD <= (void*)3)goto _LL20E;if(*((int*)_tmp2FD)!= 2)goto _LL20E;_tmp2FE=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2FD)->f1;_tmp2FF=*_tmp2FE;_tmp300=(
void*)_tmp2FF.root;_tmp301=_tmp2FF.fields;_LL20D:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp306;_tmp306.tag=Cyc_CfFlowInfo_NotZero;
_tmp306.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp2F9);_tmp306;});
_tmp305;})));return({struct _tuple7 _tmp307;_tmp307.f1=f;_tmp307.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp308;(_tmp308.PlaceL).tag=0;(_tmp308.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp309=_region_malloc(_tmp2F0->r,sizeof(*_tmp309));
_tmp309->root=(void*)_tmp300;_tmp309->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp2F0->r,_tmp301,flds);_tmp309;});_tmp308;});_tmp307;});_LL20E: if((int)_tmp2FD
!= 0)goto _LL210;_LL20F:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp30A;_tmp30A.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp30C;(_tmp30C.BottomFL).tag=0;_tmp30C;});
_tmp30A.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp30B;(_tmp30B.UnknownL).tag=1;_tmp30B;});_tmp30A;});_LL210: if(_tmp2FD <= (void*)
3)goto _LL212;if(*((int*)_tmp2FD)!= 0)goto _LL212;_tmp302=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp2FD)->f1;_LL211: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp302);goto
_LL213;_LL212:;_LL213:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp30E;_tmp30E.tag=Cyc_CfFlowInfo_UnknownZ;_tmp30E.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp2F9);_tmp30E;});_tmp30D;})));_LL207:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp30F=_cycalloc(sizeof(*
_tmp30F));_tmp30F[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp310;_tmp310.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp310.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp2F9);_tmp310;});_tmp30F;})));}if(Cyc_CfFlowInfo_initlevel(_tmp2F8,r)== (void*)
0)({void*_tmp311[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp312="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp312,sizeof(char),_get_zero_arr_size(_tmp312,46));}),
_tag_dynforward(_tmp311,sizeof(void*),0));});return({struct _tuple7 _tmp313;
_tmp313.f1=f;_tmp313.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp314;(_tmp314.UnknownL).tag=1;_tmp314;});_tmp313;});_LL202:;}_LL200:;_LL201:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp315=_cycalloc(sizeof(*
_tmp315));_tmp315[0]=({struct Cyc_Core_Impossible_struct _tmp316;_tmp316.tag=Cyc_Core_Impossible;
_tmp316.f1=({const char*_tmp317="left deref of non-pointer-type";_tag_dynforward(
_tmp317,sizeof(char),_get_zero_arr_size(_tmp317,31));});_tmp316;});_tmp315;}));
_LL1FD:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp318=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp319=inflow;struct Cyc_Dict_Dict _tmp31A;struct Cyc_List_List*
_tmp31B;_LL215: if((_tmp319.BottomFL).tag != 0)goto _LL217;_LL216: return({struct
_tuple7 _tmp31C;_tmp31C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp31E;(_tmp31E.BottomFL).tag=0;_tmp31E;});_tmp31C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp31D;(_tmp31D.UnknownL).tag=1;_tmp31D;});
_tmp31C;});_LL217: if((_tmp319.ReachableFL).tag != 1)goto _LL214;_tmp31A=(_tmp319.ReachableFL).f1;
_tmp31B=(_tmp319.ReachableFL).f2;_LL218: d=_tmp31A;_LL214:;}{void*_tmp31F=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp320;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*
_tmp322;void*_tmp323;struct Cyc_Absyn_Vardecl*_tmp324;void*_tmp325;struct Cyc_Absyn_Vardecl*
_tmp326;void*_tmp327;struct Cyc_Absyn_Vardecl*_tmp328;void*_tmp329;struct Cyc_Absyn_Vardecl*
_tmp32A;struct Cyc_Absyn_Exp*_tmp32B;struct _dynforward_ptr*_tmp32C;struct Cyc_Absyn_Exp*
_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Exp*
_tmp330;struct _dynforward_ptr*_tmp331;_LL21A: if(*((int*)_tmp31F)!= 15)goto _LL21C;
_tmp320=((struct Cyc_Absyn_Cast_e_struct*)_tmp31F)->f2;_LL21B: _tmp321=_tmp320;
goto _LL21D;_LL21C: if(*((int*)_tmp31F)!= 13)goto _LL21E;_tmp321=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp31F)->f1;_LL21D: _tmp322=_tmp321;goto _LL21F;_LL21E: if(*((int*)_tmp31F)!= 14)
goto _LL220;_tmp322=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp31F)->f1;_LL21F:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp322,flds);_LL220: if(*((int*)
_tmp31F)!= 1)goto _LL222;_tmp323=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp31F)->f2;
if(_tmp323 <= (void*)1)goto _LL222;if(*((int*)_tmp323)!= 2)goto _LL222;_tmp324=((
struct Cyc_Absyn_Param_b_struct*)_tmp323)->f1;_LL221: _tmp326=_tmp324;goto _LL223;
_LL222: if(*((int*)_tmp31F)!= 1)goto _LL224;_tmp325=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp31F)->f2;if(_tmp325 <= (void*)1)goto _LL224;if(*((int*)_tmp325)!= 3)goto _LL224;
_tmp326=((struct Cyc_Absyn_Local_b_struct*)_tmp325)->f1;_LL223: _tmp328=_tmp326;
goto _LL225;_LL224: if(*((int*)_tmp31F)!= 1)goto _LL226;_tmp327=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp31F)->f2;if(_tmp327 <= (void*)1)goto _LL226;if(*((int*)_tmp327)!= 4)goto _LL226;
_tmp328=((struct Cyc_Absyn_Pat_b_struct*)_tmp327)->f1;_LL225: return({struct
_tuple7 _tmp332;_tmp332.f1=inflow;_tmp332.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp333;(_tmp333.PlaceL).tag=0;(_tmp333.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp334=_region_malloc(env->r,sizeof(*_tmp334));
_tmp334->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp335=
_region_malloc(env->r,sizeof(*_tmp335));_tmp335[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp336;_tmp336.tag=0;_tmp336.f1=_tmp328;_tmp336;});_tmp335;}));_tmp334->fields=
flds;_tmp334;});_tmp333;});_tmp332;});_LL226: if(*((int*)_tmp31F)!= 1)goto _LL228;
_tmp329=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp31F)->f2;if(_tmp329 <= (void*)
1)goto _LL228;if(*((int*)_tmp329)!= 0)goto _LL228;_tmp32A=((struct Cyc_Absyn_Global_b_struct*)
_tmp329)->f1;_LL227: return({struct _tuple7 _tmp337;_tmp337.f1=inflow;_tmp337.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp338;(
_tmp338.UnknownL).tag=1;_tmp338;});_tmp337;});_LL228: if(*((int*)_tmp31F)!= 24)
goto _LL22A;_tmp32B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp31F)->f1;_tmp32C=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp31F)->f2;_LL229:{void*_tmp339=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp32B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp33A;void*_tmp33B;_LL233: if(_tmp339 <= (void*)4)goto _LL235;if(*((int*)_tmp339)
!= 4)goto _LL235;_tmp33A=((struct Cyc_Absyn_PointerType_struct*)_tmp339)->f1;
_tmp33B=(void*)_tmp33A.elt_typ;_LL234: if(!Cyc_Absyn_is_union_type(_tmp33B))flds=({
struct Cyc_List_List*_tmp33C=_region_malloc(env->r,sizeof(*_tmp33C));_tmp33C->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp33B,_tmp32C);_tmp33C->tl=flds;
_tmp33C;});goto _LL232;_LL235:;_LL236:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D[0]=({struct Cyc_Core_Impossible_struct
_tmp33E;_tmp33E.tag=Cyc_Core_Impossible;_tmp33E.f1=({const char*_tmp33F="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,25));});_tmp33E;});
_tmp33D;}));_LL232:;}_tmp32D=_tmp32B;goto _LL22B;_LL22A: if(*((int*)_tmp31F)!= 22)
goto _LL22C;_tmp32D=((struct Cyc_Absyn_Deref_e_struct*)_tmp31F)->f1;_LL22B: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp341;void*_tmp342;struct _tuple5 _tmp340=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp32D);_tmp341=_tmp340.f1;_tmp342=_tmp340.f2;_tmp341=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp341);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp341,_tmp32D,
_tmp342,flds);}_LL22C: if(*((int*)_tmp31F)!= 25)goto _LL22E;_tmp32E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp31F)->f1;_tmp32F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp31F)->f2;_LL22D: {
void*_tmp343=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp32E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp344;struct Cyc_Absyn_PtrAtts
_tmp345;struct Cyc_Absyn_Conref*_tmp346;_LL238: if(_tmp343 <= (void*)4)goto _LL23C;
if(*((int*)_tmp343)!= 9)goto _LL23A;_LL239: {unsigned int _tmp347=(Cyc_Evexp_eval_const_uint_exp(
_tmp32F)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp32E,({struct
Cyc_List_List*_tmp348=_region_malloc(env->r,sizeof(*_tmp348));_tmp348->hd=(void*)((
int)_tmp347);_tmp348->tl=flds;_tmp348;}));}_LL23A: if(*((int*)_tmp343)!= 4)goto
_LL23C;_tmp344=((struct Cyc_Absyn_PointerType_struct*)_tmp343)->f1;_tmp345=
_tmp344.ptr_atts;_tmp346=_tmp345.bounds;_LL23B: {struct _RegionHandle _tmp349=
_new_region("temp");struct _RegionHandle*temp=& _tmp349;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp34C;struct Cyc_List_List*_tmp34D;struct _tuple11
_tmp34B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp34A[2];_tmp34A[1]=_tmp32F;_tmp34A[0]=_tmp32E;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp34A,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp34C=_tmp34B.f1;_tmp34D=
_tmp34B.f2;_tmp34C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp32F->loc,_tmp34C);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp34E=_tmp34C;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp34F=_tmp34C;struct Cyc_Dict_Dict _tmp350;struct Cyc_List_List*_tmp351;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp352;_LL23F: if((_tmp34F.ReachableFL).tag != 1)goto _LL241;_tmp350=(_tmp34F.ReachableFL).f1;
_tmp351=(_tmp34F.ReachableFL).f2;_tmp352=(_tmp34F.ReachableFL).f3;_LL240: if(Cyc_CfFlowInfo_initlevel(
_tmp350,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp34D))->tl))->hd)== (void*)0)({void*_tmp353[0]={};Cyc_Tcutil_terr(
_tmp32F->loc,({const char*_tmp354="expression may not be initialized";
_tag_dynforward(_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,34));}),
_tag_dynforward(_tmp353,sizeof(void*),0));});{struct Cyc_List_List*_tmp355=Cyc_NewControlFlow_add_subscript_reln(
_tmp318->r,_tmp351,_tmp32E,_tmp32F);if(_tmp351 != _tmp355)_tmp34E=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp356;(_tmp356.ReachableFL).tag=1;(_tmp356.ReachableFL).f1=
_tmp350;(_tmp356.ReachableFL).f2=_tmp355;(_tmp356.ReachableFL).f3=_tmp352;
_tmp356;});goto _LL23E;}_LL241:;_LL242: goto _LL23E;_LL23E:;}{union Cyc_Absyn_Constraint_union
_tmp357=(Cyc_Absyn_compress_conref(_tmp346))->v;void*_tmp358;_LL244: if((_tmp357.Eq_constr).tag
!= 0)goto _LL246;_tmp358=(_tmp357.Eq_constr).f1;if(_tmp358 <= (void*)2)goto _LL246;
if(*((int*)_tmp358)!= 1)goto _LL246;_LL245:{void*_tmp359=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp34D))->tl))->hd;struct Cyc_List_List*
_tmp35A;_LL249: if(_tmp359 <= (void*)3)goto _LL24B;if(*((int*)_tmp359)!= 3)goto
_LL24B;_tmp35A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp359)->f1;_LL24A:(void*)(
_tmp32F->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp35B=
_cycalloc(sizeof(*_tmp35B));_tmp35B[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp35C;_tmp35C.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp35C.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp35A);_tmp35C;});_tmp35B;})));goto _LL248;_LL24B:;_LL24C:
goto _LL248;_LL248:;}goto _LL243;_LL246:;_LL247: goto _LL243;_LL243:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp35E;union Cyc_CfFlowInfo_AbsLVal_union _tmp35F;struct _tuple7 _tmp35D=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp34C,_tmp32E,(void*)((struct Cyc_List_List*)_check_null(_tmp34D))->hd,
flds);_tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp360=_tmp35E;_LL24E: if((_tmp360.BottomFL).tag != 0)goto _LL250;_LL24F: {struct
_tuple7 _tmp362=({struct _tuple7 _tmp361;_tmp361.f1=_tmp35E;_tmp361.f2=_tmp35F;
_tmp361;});_npop_handler(0);return _tmp362;}_LL250:;_LL251: {struct _tuple7 _tmp364=({
struct _tuple7 _tmp363;_tmp363.f1=_tmp34E;_tmp363.f2=_tmp35F;_tmp363;});
_npop_handler(0);return _tmp364;}_LL24D:;}}}};_pop_region(temp);}_LL23C:;_LL23D:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp365=_cycalloc(sizeof(*
_tmp365));_tmp365[0]=({struct Cyc_Core_Impossible_struct _tmp366;_tmp366.tag=Cyc_Core_Impossible;
_tmp366.f1=({const char*_tmp367="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp367,sizeof(char),_get_zero_arr_size(_tmp367,33));});_tmp366;});
_tmp365;}));_LL237:;}_LL22E: if(*((int*)_tmp31F)!= 23)goto _LL230;_tmp330=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp31F)->f1;_tmp331=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp31F)->f2;_LL22F: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp330->topt))->v))return({struct _tuple7 _tmp368;_tmp368.f1=inflow;
_tmp368.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp369;(_tmp369.UnknownL).tag=1;_tmp369;});_tmp368;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp330,({struct Cyc_List_List*_tmp36A=_region_malloc(env->r,sizeof(*
_tmp36A));_tmp36A->hd=(void*)Cyc_NewControlFlow_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp330->topt))->v,_tmp331);_tmp36A->tl=flds;_tmp36A;}));
_LL230:;_LL231: return({struct _tuple7 _tmp36B;_tmp36B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp36D;(_tmp36D.BottomFL).tag=0;_tmp36D;});
_tmp36B.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp36C;(_tmp36C.UnknownL).tag=1;_tmp36C;});_tmp36B;});_LL219:;}}static struct
_tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp36F;union Cyc_CfFlowInfo_AbsLVal_union _tmp370;struct _tuple7 _tmp36E=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp36F=_tmp36E.f1;_tmp370=_tmp36E.f2;return({struct _tuple7
_tmp371;_tmp371.f1=_tmp36F;_tmp371.f2=_tmp370;_tmp371;});}static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp372=env->fenv;{void*
_tmp373=(void*)e->r;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp375;
struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*
_tmp378;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Exp*_tmp37A;struct Cyc_Absyn_Exp*
_tmp37B;struct Cyc_Absyn_Exp*_tmp37C;void*_tmp37D;struct Cyc_List_List*_tmp37E;
struct Cyc_List_List _tmp37F;struct Cyc_Absyn_Exp*_tmp380;struct Cyc_List_List*
_tmp381;void*_tmp382;struct Cyc_List_List*_tmp383;_LL253: if(*((int*)_tmp373)!= 6)
goto _LL255;_tmp374=((struct Cyc_Absyn_Conditional_e_struct*)_tmp373)->f1;_tmp375=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp373)->f2;_tmp376=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp373)->f3;_LL254: {union Cyc_CfFlowInfo_FlowInfo_union _tmp385;union Cyc_CfFlowInfo_FlowInfo_union
_tmp386;struct _tuple8 _tmp384=Cyc_NewControlFlow_anal_test(env,inflow,_tmp374);
_tmp385=_tmp384.f1;_tmp386=_tmp384.f2;_tmp385=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp374->loc,_tmp385);_tmp386=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp374->loc,
_tmp386);{union Cyc_CfFlowInfo_FlowInfo_union _tmp388;union Cyc_CfFlowInfo_FlowInfo_union
_tmp389;struct _tuple8 _tmp387=Cyc_NewControlFlow_anal_test(env,_tmp385,_tmp375);
_tmp388=_tmp387.f1;_tmp389=_tmp387.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp38B;
union Cyc_CfFlowInfo_FlowInfo_union _tmp38C;struct _tuple8 _tmp38A=Cyc_NewControlFlow_anal_test(
env,_tmp386,_tmp376);_tmp38B=_tmp38A.f1;_tmp38C=_tmp38A.f2;return({struct _tuple8
_tmp38D;_tmp38D.f1=Cyc_CfFlowInfo_join_flow(_tmp372,env->all_changed,_tmp388,
_tmp38B,1);_tmp38D.f2=Cyc_CfFlowInfo_join_flow(_tmp372,env->all_changed,_tmp389,
_tmp38C,1);_tmp38D;});}}}_LL255: if(*((int*)_tmp373)!= 7)goto _LL257;_tmp377=((
struct Cyc_Absyn_And_e_struct*)_tmp373)->f1;_tmp378=((struct Cyc_Absyn_And_e_struct*)
_tmp373)->f2;_LL256: {union Cyc_CfFlowInfo_FlowInfo_union _tmp38F;union Cyc_CfFlowInfo_FlowInfo_union
_tmp390;struct _tuple8 _tmp38E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp377);
_tmp38F=_tmp38E.f1;_tmp390=_tmp38E.f2;_tmp38F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp377->loc,_tmp38F);_tmp390=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp377->loc,
_tmp390);{union Cyc_CfFlowInfo_FlowInfo_union _tmp392;union Cyc_CfFlowInfo_FlowInfo_union
_tmp393;struct _tuple8 _tmp391=Cyc_NewControlFlow_anal_test(env,_tmp38F,_tmp378);
_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;_tmp392=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp378->loc,_tmp392);_tmp393=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp378->loc,
_tmp393);return({struct _tuple8 _tmp394;_tmp394.f1=_tmp392;_tmp394.f2=Cyc_CfFlowInfo_join_flow(
_tmp372,env->all_changed,_tmp390,_tmp393,0);_tmp394;});}}_LL257: if(*((int*)
_tmp373)!= 8)goto _LL259;_tmp379=((struct Cyc_Absyn_Or_e_struct*)_tmp373)->f1;
_tmp37A=((struct Cyc_Absyn_Or_e_struct*)_tmp373)->f2;_LL258: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp396;union Cyc_CfFlowInfo_FlowInfo_union _tmp397;struct _tuple8 _tmp395=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp379);_tmp396=_tmp395.f1;_tmp397=_tmp395.f2;_tmp396=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp379->loc,_tmp396);_tmp397=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp379->loc,
_tmp397);{union Cyc_CfFlowInfo_FlowInfo_union _tmp399;union Cyc_CfFlowInfo_FlowInfo_union
_tmp39A;struct _tuple8 _tmp398=Cyc_NewControlFlow_anal_test(env,_tmp397,_tmp37A);
_tmp399=_tmp398.f1;_tmp39A=_tmp398.f2;_tmp399=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp37A->loc,_tmp399);_tmp39A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp37A->loc,
_tmp39A);return({struct _tuple8 _tmp39B;_tmp39B.f1=Cyc_CfFlowInfo_join_flow(
_tmp372,env->all_changed,_tmp396,_tmp399,0);_tmp39B.f2=_tmp39A;_tmp39B;});}}
_LL259: if(*((int*)_tmp373)!= 9)goto _LL25B;_tmp37B=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp373)->f1;_tmp37C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp373)->f2;_LL25A: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp39D;void*_tmp39E;struct _tuple5 _tmp39C=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp37B);_tmp39D=_tmp39C.f1;_tmp39E=_tmp39C.f2;_tmp39D=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp37B->loc,_tmp39D);return Cyc_NewControlFlow_anal_test(env,_tmp39D,_tmp37C);}
_LL25B: if(*((int*)_tmp373)!= 3)goto _LL25D;_tmp37D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp373)->f1;if((int)_tmp37D != 11)goto _LL25D;_tmp37E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp373)->f2;if(_tmp37E == 0)goto _LL25D;_tmp37F=*_tmp37E;_tmp380=(struct Cyc_Absyn_Exp*)
_tmp37F.hd;_tmp381=_tmp37F.tl;if(_tmp381 != 0)goto _LL25D;_LL25C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A0;union Cyc_CfFlowInfo_FlowInfo_union _tmp3A1;struct _tuple8 _tmp39F=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp380);_tmp3A0=_tmp39F.f1;_tmp3A1=_tmp39F.f2;return({struct _tuple8
_tmp3A2;_tmp3A2.f1=_tmp3A1;_tmp3A2.f2=_tmp3A0;_tmp3A2;});}_LL25D: if(*((int*)
_tmp373)!= 3)goto _LL25F;_tmp382=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp373)->f1;_tmp383=((struct Cyc_Absyn_Primop_e_struct*)_tmp373)->f2;_LL25E: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3A3=
_new_region("temp");struct _RegionHandle*temp=& _tmp3A3;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3A5;struct Cyc_List_List*_tmp3A6;struct _tuple11
_tmp3A4=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp383,0);
_tmp3A5=_tmp3A4.f1;_tmp3A6=_tmp3A4.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3A6))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3A6->tl))->hd;
f=_tmp3A5;};_pop_region(temp);}{void*_tmp3A7=_tmp382;_LL262: if((int)_tmp3A7 != 0)
goto _LL264;_LL263: goto _LL265;_LL264: if((int)_tmp3A7 != 2)goto _LL266;_LL265: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp383))->hd)->loc,f);
goto _LL261;_LL266:;_LL267: f=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp383))->hd)->loc,f);goto _LL261;_LL261:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp3A8=f;struct Cyc_Dict_Dict _tmp3A9;struct Cyc_List_List*
_tmp3AA;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3AB;_LL269: if((_tmp3A8.BottomFL).tag
!= 0)goto _LL26B;_LL26A: return({struct _tuple8 _tmp3AC;_tmp3AC.f1=f;_tmp3AC.f2=f;
_tmp3AC;});_LL26B: if((_tmp3A8.ReachableFL).tag != 1)goto _LL268;_tmp3A9=(_tmp3A8.ReachableFL).f1;
_tmp3AA=(_tmp3A8.ReachableFL).f2;_tmp3AB=(_tmp3A8.ReachableFL).f3;_LL26C: {
struct Cyc_Absyn_Exp*_tmp3AD=(struct Cyc_Absyn_Exp*)_tmp383->hd;struct Cyc_Absyn_Exp*
_tmp3AE=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp383->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp3A9,r1)== (void*)0)({void*_tmp3AF[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp383->hd)->loc,({const char*_tmp3B0="expression may not be initialized";
_tag_dynforward(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,34));}),
_tag_dynforward(_tmp3AF,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp3A9,
r2)== (void*)0)({void*_tmp3B1[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp383->tl))->hd)->loc,({const char*_tmp3B2="expression may not be initialized";
_tag_dynforward(_tmp3B2,sizeof(char),_get_zero_arr_size(_tmp3B2,34));}),
_tag_dynforward(_tmp3B1,sizeof(void*),0));});if(_tmp382 == (void*)5  || _tmp382 == (
void*)6){struct _tuple0 _tmp3B4=({struct _tuple0 _tmp3B3;_tmp3B3.f1=r1;_tmp3B3.f2=r2;
_tmp3B3;});void*_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;void*_tmp3B9;void*
_tmp3BA;void*_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;void*
_tmp3C0;void*_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;void*
_tmp3C6;void*_tmp3C7;void*_tmp3C8;_LL26E: _tmp3B5=_tmp3B4.f1;if(_tmp3B5 <= (void*)
3)goto _LL270;if(*((int*)_tmp3B5)!= 0)goto _LL270;_tmp3B6=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3B5)->f1;_tmp3B7=_tmp3B4.f2;if((int)_tmp3B7 != 0)goto _LL270;_LL26F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CA;union Cyc_CfFlowInfo_FlowInfo_union _tmp3CB;struct _tuple8 _tmp3C9=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3AD,_tmp3B6);_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;{void*_tmp3CC=
_tmp382;_LL283: if((int)_tmp3CC != 5)goto _LL285;_LL284: return({struct _tuple8
_tmp3CD;_tmp3CD.f1=_tmp3CB;_tmp3CD.f2=_tmp3CA;_tmp3CD;});_LL285: if((int)_tmp3CC
!= 6)goto _LL287;_LL286: return({struct _tuple8 _tmp3CE;_tmp3CE.f1=_tmp3CA;_tmp3CE.f2=
_tmp3CB;_tmp3CE;});_LL287:;_LL288:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_Core_Impossible_struct
_tmp3D0;_tmp3D0.tag=Cyc_Core_Impossible;_tmp3D0.f1=({const char*_tmp3D1="anal_test, zero-split";
_tag_dynforward(_tmp3D1,sizeof(char),_get_zero_arr_size(_tmp3D1,22));});_tmp3D0;});
_tmp3CF;}));_LL282:;}}_LL270: _tmp3B8=_tmp3B4.f1;if((int)_tmp3B8 != 0)goto _LL272;
_tmp3B9=_tmp3B4.f2;if(_tmp3B9 <= (void*)3)goto _LL272;if(*((int*)_tmp3B9)!= 0)goto
_LL272;_tmp3BA=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3B9)->f1;
_LL271: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3D3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D4;struct _tuple8 _tmp3D2=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3AE,
_tmp3BA);_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;{void*_tmp3D5=_tmp382;_LL28A: if((
int)_tmp3D5 != 5)goto _LL28C;_LL28B: return({struct _tuple8 _tmp3D6;_tmp3D6.f1=
_tmp3D4;_tmp3D6.f2=_tmp3D3;_tmp3D6;});_LL28C: if((int)_tmp3D5 != 6)goto _LL28E;
_LL28D: return({struct _tuple8 _tmp3D7;_tmp3D7.f1=_tmp3D3;_tmp3D7.f2=_tmp3D4;
_tmp3D7;});_LL28E:;_LL28F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8[0]=({struct Cyc_Core_Impossible_struct
_tmp3D9;_tmp3D9.tag=Cyc_Core_Impossible;_tmp3D9.f1=({const char*_tmp3DA="anal_test, zero-split";
_tag_dynforward(_tmp3DA,sizeof(char),_get_zero_arr_size(_tmp3DA,22));});_tmp3D9;});
_tmp3D8;}));_LL289:;}}_LL272: _tmp3BB=_tmp3B4.f1;if((int)_tmp3BB != 0)goto _LL274;
_tmp3BC=_tmp3B4.f2;if((int)_tmp3BC != 0)goto _LL274;_LL273: if(_tmp382 == (void*)5)
return({struct _tuple8 _tmp3DB;_tmp3DB.f1=f;_tmp3DB.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3DC;(_tmp3DC.BottomFL).tag=0;_tmp3DC;});
_tmp3DB;});else{return({struct _tuple8 _tmp3DD;_tmp3DD.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3DE;(_tmp3DE.BottomFL).tag=0;_tmp3DE;});
_tmp3DD.f2=f;_tmp3DD;});}_LL274: _tmp3BD=_tmp3B4.f1;if((int)_tmp3BD != 0)goto
_LL276;_tmp3BE=_tmp3B4.f2;if((int)_tmp3BE != 1)goto _LL276;_LL275: goto _LL277;
_LL276: _tmp3BF=_tmp3B4.f1;if((int)_tmp3BF != 0)goto _LL278;_tmp3C0=_tmp3B4.f2;if((
int)_tmp3C0 != 2)goto _LL278;_LL277: goto _LL279;_LL278: _tmp3C1=_tmp3B4.f1;if((int)
_tmp3C1 != 0)goto _LL27A;_tmp3C2=_tmp3B4.f2;if(_tmp3C2 <= (void*)3)goto _LL27A;if(*((
int*)_tmp3C2)!= 2)goto _LL27A;_LL279: goto _LL27B;_LL27A: _tmp3C3=_tmp3B4.f1;if((int)
_tmp3C3 != 1)goto _LL27C;_tmp3C4=_tmp3B4.f2;if((int)_tmp3C4 != 0)goto _LL27C;_LL27B:
goto _LL27D;_LL27C: _tmp3C5=_tmp3B4.f1;if((int)_tmp3C5 != 2)goto _LL27E;_tmp3C6=
_tmp3B4.f2;if((int)_tmp3C6 != 0)goto _LL27E;_LL27D: goto _LL27F;_LL27E: _tmp3C7=
_tmp3B4.f1;if(_tmp3C7 <= (void*)3)goto _LL280;if(*((int*)_tmp3C7)!= 2)goto _LL280;
_tmp3C8=_tmp3B4.f2;if((int)_tmp3C8 != 0)goto _LL280;_LL27F: if(_tmp382 == (void*)6)
return({struct _tuple8 _tmp3DF;_tmp3DF.f1=f;_tmp3DF.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E0;(_tmp3E0.BottomFL).tag=0;_tmp3E0;});
_tmp3DF;});else{return({struct _tuple8 _tmp3E1;_tmp3E1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E2;(_tmp3E2.BottomFL).tag=0;_tmp3E2;});
_tmp3E1.f2=f;_tmp3E1;});}_LL280:;_LL281: goto _LL26D;_LL26D:;}{struct _tuple0
_tmp3E4=({struct _tuple0 _tmp3E3;_tmp3E3.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3AD->topt))->v);_tmp3E3.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3AE->topt))->v);_tmp3E3;});void*_tmp3E5;void*
_tmp3E6;void*_tmp3E7;void*_tmp3E8;void*_tmp3E9;void*_tmp3EA;_LL291: _tmp3E5=
_tmp3E4.f1;if(_tmp3E5 <= (void*)4)goto _LL293;if(*((int*)_tmp3E5)!= 5)goto _LL293;
_tmp3E6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E5)->f1;if((int)_tmp3E6 != 
1)goto _LL293;_LL292: goto _LL294;_LL293: _tmp3E7=_tmp3E4.f2;if(_tmp3E7 <= (void*)4)
goto _LL295;if(*((int*)_tmp3E7)!= 5)goto _LL295;_tmp3E8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3E7)->f1;if((int)_tmp3E8 != 1)goto _LL295;_LL294: goto _LL296;_LL295: _tmp3E9=
_tmp3E4.f1;if(_tmp3E9 <= (void*)4)goto _LL297;if(*((int*)_tmp3E9)!= 18)goto _LL297;
_LL296: goto _LL298;_LL297: _tmp3EA=_tmp3E4.f2;if(_tmp3EA <= (void*)4)goto _LL299;if(*((
int*)_tmp3EA)!= 18)goto _LL299;_LL298: goto _LL290;_LL299:;_LL29A: return({struct
_tuple8 _tmp3EB;_tmp3EB.f1=f;_tmp3EB.f2=f;_tmp3EB;});_LL290:;}{void*_tmp3EC=
_tmp382;_LL29C: if((int)_tmp3EC != 5)goto _LL29E;_LL29D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3ED=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3AD,r1,(void*)5,r2);_tmp3ED=Cyc_NewControlFlow_if_tagcmp(
env,_tmp3ED,_tmp3AE,r2,(void*)5,r1);return({struct _tuple8 _tmp3EE;_tmp3EE.f1=
_tmp3ED;_tmp3EE.f2=f;_tmp3EE;});}_LL29E: if((int)_tmp3EC != 6)goto _LL2A0;_LL29F: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3EF=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3AD,r1,(void*)5,r2);_tmp3EF=Cyc_NewControlFlow_if_tagcmp(env,_tmp3EF,_tmp3AE,
r2,(void*)5,r1);return({struct _tuple8 _tmp3F0;_tmp3F0.f1=f;_tmp3F0.f2=_tmp3EF;
_tmp3F0;});}_LL2A0: if((int)_tmp3EC != 7)goto _LL2A2;_LL2A1: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F1=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3AE,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F2=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3AD,r1,(void*)10,r2);return({
struct _tuple8 _tmp3F3;_tmp3F3.f1=_tmp3F1;_tmp3F3.f2=_tmp3F2;_tmp3F3;});}_LL2A2:
if((int)_tmp3EC != 9)goto _LL2A4;_LL2A3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F4=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3AE,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F5=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3AD,r1,(void*)8,r2);return({
struct _tuple8 _tmp3F6;_tmp3F6.f1=_tmp3F4;_tmp3F6.f2=_tmp3F5;_tmp3F6;});}_LL2A4:
if((int)_tmp3EC != 8)goto _LL2A6;_LL2A5: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F7=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3AD,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F8=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3AE,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F9=_tmp3F7;struct Cyc_Dict_Dict _tmp3FA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3FB;_LL2AB: if((_tmp3F9.BottomFL).tag != 0)goto _LL2AD;_LL2AC:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC[0]=({
struct Cyc_Core_Impossible_struct _tmp3FD;_tmp3FD.tag=Cyc_Core_Impossible;_tmp3FD.f1=({
const char*_tmp3FE="anal_test, Lt";_tag_dynforward(_tmp3FE,sizeof(char),
_get_zero_arr_size(_tmp3FE,14));});_tmp3FD;});_tmp3FC;}));_LL2AD: if((_tmp3F9.ReachableFL).tag
!= 1)goto _LL2AA;_tmp3FA=(_tmp3F9.ReachableFL).f1;_tmp3FB=(_tmp3F9.ReachableFL).f3;
_LL2AE: _tmp3A9=_tmp3FA;_tmp3AB=_tmp3FB;_LL2AA:;}{void*_tmp3FF=(void*)_tmp3AD->r;
void*_tmp400;struct Cyc_Absyn_Vardecl*_tmp401;void*_tmp402;struct Cyc_Absyn_Vardecl*
_tmp403;void*_tmp404;struct Cyc_Absyn_Vardecl*_tmp405;void*_tmp406;struct Cyc_Absyn_Vardecl*
_tmp407;_LL2B0: if(*((int*)_tmp3FF)!= 1)goto _LL2B2;_tmp400=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3FF)->f2;if(_tmp400 <= (void*)1)goto _LL2B2;if(*((int*)_tmp400)!= 0)goto _LL2B2;
_tmp401=((struct Cyc_Absyn_Global_b_struct*)_tmp400)->f1;if(!(!_tmp401->escapes))
goto _LL2B2;_LL2B1: _tmp403=_tmp401;goto _LL2B3;_LL2B2: if(*((int*)_tmp3FF)!= 1)goto
_LL2B4;_tmp402=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3FF)->f2;if(_tmp402 <= (
void*)1)goto _LL2B4;if(*((int*)_tmp402)!= 3)goto _LL2B4;_tmp403=((struct Cyc_Absyn_Local_b_struct*)
_tmp402)->f1;if(!(!_tmp403->escapes))goto _LL2B4;_LL2B3: _tmp405=_tmp403;goto
_LL2B5;_LL2B4: if(*((int*)_tmp3FF)!= 1)goto _LL2B6;_tmp404=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3FF)->f2;if(_tmp404 <= (void*)1)goto _LL2B6;if(*((int*)_tmp404)!= 4)goto _LL2B6;
_tmp405=((struct Cyc_Absyn_Pat_b_struct*)_tmp404)->f1;if(!(!_tmp405->escapes))
goto _LL2B6;_LL2B5: _tmp407=_tmp405;goto _LL2B7;_LL2B6: if(*((int*)_tmp3FF)!= 1)goto
_LL2B8;_tmp406=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3FF)->f2;if(_tmp406 <= (
void*)1)goto _LL2B8;if(*((int*)_tmp406)!= 2)goto _LL2B8;_tmp407=((struct Cyc_Absyn_Param_b_struct*)
_tmp406)->f1;if(!(!_tmp407->escapes))goto _LL2B8;_LL2B7: {void*_tmp408=(void*)
_tmp3AE->r;void*_tmp409;struct Cyc_Absyn_Vardecl*_tmp40A;void*_tmp40B;struct Cyc_Absyn_Vardecl*
_tmp40C;void*_tmp40D;struct Cyc_Absyn_Vardecl*_tmp40E;void*_tmp40F;struct Cyc_Absyn_Vardecl*
_tmp410;union Cyc_Absyn_Cnst_union _tmp411;int _tmp412;void*_tmp413;struct Cyc_List_List*
_tmp414;struct Cyc_List_List _tmp415;struct Cyc_Absyn_Exp*_tmp416;_LL2BB: if(*((int*)
_tmp408)!= 1)goto _LL2BD;_tmp409=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp408)->f2;
if(_tmp409 <= (void*)1)goto _LL2BD;if(*((int*)_tmp409)!= 0)goto _LL2BD;_tmp40A=((
struct Cyc_Absyn_Global_b_struct*)_tmp409)->f1;if(!(!_tmp40A->escapes))goto _LL2BD;
_LL2BC: _tmp40C=_tmp40A;goto _LL2BE;_LL2BD: if(*((int*)_tmp408)!= 1)goto _LL2BF;
_tmp40B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp408)->f2;if(_tmp40B <= (void*)
1)goto _LL2BF;if(*((int*)_tmp40B)!= 3)goto _LL2BF;_tmp40C=((struct Cyc_Absyn_Local_b_struct*)
_tmp40B)->f1;if(!(!_tmp40C->escapes))goto _LL2BF;_LL2BE: _tmp40E=_tmp40C;goto
_LL2C0;_LL2BF: if(*((int*)_tmp408)!= 1)goto _LL2C1;_tmp40D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp408)->f2;if(_tmp40D <= (void*)1)goto _LL2C1;if(*((int*)_tmp40D)!= 4)goto _LL2C1;
_tmp40E=((struct Cyc_Absyn_Pat_b_struct*)_tmp40D)->f1;if(!(!_tmp40E->escapes))
goto _LL2C1;_LL2C0: _tmp410=_tmp40E;goto _LL2C2;_LL2C1: if(*((int*)_tmp408)!= 1)goto
_LL2C3;_tmp40F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp408)->f2;if(_tmp40F <= (
void*)1)goto _LL2C3;if(*((int*)_tmp40F)!= 2)goto _LL2C3;_tmp410=((struct Cyc_Absyn_Param_b_struct*)
_tmp40F)->f1;if(!(!_tmp410->escapes))goto _LL2C3;_LL2C2: {struct _RegionHandle*
_tmp417=(env->fenv)->r;struct Cyc_List_List*_tmp418=({struct Cyc_List_List*_tmp41B=
_region_malloc(_tmp417,sizeof(*_tmp41B));_tmp41B->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp41C=_region_malloc(_tmp417,sizeof(*_tmp41C));_tmp41C->vd=_tmp407;_tmp41C->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp41D;(
_tmp41D.LessVar).tag=1;(_tmp41D.LessVar).f1=_tmp410;_tmp41D;});_tmp41C;});
_tmp41B->tl=_tmp3AA;_tmp41B;});return({struct _tuple8 _tmp419;_tmp419.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp41A;(_tmp41A.ReachableFL).tag=1;(_tmp41A.ReachableFL).f1=
_tmp3A9;(_tmp41A.ReachableFL).f2=_tmp418;(_tmp41A.ReachableFL).f3=_tmp3AB;
_tmp41A;});_tmp419.f2=_tmp3F8;_tmp419;});}_LL2C3: if(*((int*)_tmp408)!= 0)goto
_LL2C5;_tmp411=((struct Cyc_Absyn_Const_e_struct*)_tmp408)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp408)->f1).Int_c).tag != 2)goto _LL2C5;_tmp412=(_tmp411.Int_c).f2;_LL2C4: {
struct _RegionHandle*_tmp41E=(env->fenv)->r;struct Cyc_List_List*_tmp41F=({struct
Cyc_List_List*_tmp422=_region_malloc(_tmp41E,sizeof(*_tmp422));_tmp422->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp423=_region_malloc(_tmp41E,sizeof(*_tmp423));
_tmp423->vd=_tmp407;_tmp423->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp424;(_tmp424.LessConst).tag=3;(_tmp424.LessConst).f1=(unsigned int)_tmp412;
_tmp424;});_tmp423;});_tmp422->tl=_tmp3AA;_tmp422;});return({struct _tuple8
_tmp420;_tmp420.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp421;(_tmp421.ReachableFL).tag=1;(_tmp421.ReachableFL).f1=_tmp3A9;(_tmp421.ReachableFL).f2=
_tmp41F;(_tmp421.ReachableFL).f3=_tmp3AB;_tmp421;});_tmp420.f2=_tmp3F8;_tmp420;});}
_LL2C5: if(*((int*)_tmp408)!= 3)goto _LL2C7;_tmp413=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp408)->f1;if((int)_tmp413 != 19)goto _LL2C7;_tmp414=((struct Cyc_Absyn_Primop_e_struct*)
_tmp408)->f2;if(_tmp414 == 0)goto _LL2C7;_tmp415=*_tmp414;_tmp416=(struct Cyc_Absyn_Exp*)
_tmp415.hd;_LL2C6: {void*_tmp425=(void*)_tmp416->r;void*_tmp426;struct Cyc_Absyn_Vardecl*
_tmp427;void*_tmp428;struct Cyc_Absyn_Vardecl*_tmp429;void*_tmp42A;struct Cyc_Absyn_Vardecl*
_tmp42B;void*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42D;_LL2CA: if(*((int*)_tmp425)
!= 1)goto _LL2CC;_tmp426=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp425)->f2;if(
_tmp426 <= (void*)1)goto _LL2CC;if(*((int*)_tmp426)!= 0)goto _LL2CC;_tmp427=((
struct Cyc_Absyn_Global_b_struct*)_tmp426)->f1;if(!(!_tmp427->escapes))goto _LL2CC;
_LL2CB: _tmp429=_tmp427;goto _LL2CD;_LL2CC: if(*((int*)_tmp425)!= 1)goto _LL2CE;
_tmp428=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp425)->f2;if(_tmp428 <= (void*)
1)goto _LL2CE;if(*((int*)_tmp428)!= 3)goto _LL2CE;_tmp429=((struct Cyc_Absyn_Local_b_struct*)
_tmp428)->f1;if(!(!_tmp429->escapes))goto _LL2CE;_LL2CD: _tmp42B=_tmp429;goto
_LL2CF;_LL2CE: if(*((int*)_tmp425)!= 1)goto _LL2D0;_tmp42A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp425)->f2;if(_tmp42A <= (void*)1)goto _LL2D0;if(*((int*)_tmp42A)!= 4)goto _LL2D0;
_tmp42B=((struct Cyc_Absyn_Pat_b_struct*)_tmp42A)->f1;if(!(!_tmp42B->escapes))
goto _LL2D0;_LL2CF: _tmp42D=_tmp42B;goto _LL2D1;_LL2D0: if(*((int*)_tmp425)!= 1)goto
_LL2D2;_tmp42C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp425)->f2;if(_tmp42C <= (
void*)1)goto _LL2D2;if(*((int*)_tmp42C)!= 2)goto _LL2D2;_tmp42D=((struct Cyc_Absyn_Param_b_struct*)
_tmp42C)->f1;if(!(!_tmp42D->escapes))goto _LL2D2;_LL2D1: {struct _RegionHandle*
_tmp42E=(env->fenv)->r;struct Cyc_List_List*_tmp42F=({struct Cyc_List_List*_tmp432=
_region_malloc(_tmp42E,sizeof(*_tmp432));_tmp432->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp433=_region_malloc(_tmp42E,sizeof(*_tmp433));_tmp433->vd=_tmp407;_tmp433->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp434;(
_tmp434.LessSize).tag=2;(_tmp434.LessSize).f1=_tmp42D;_tmp434;});_tmp433;});
_tmp432->tl=_tmp3AA;_tmp432;});return({struct _tuple8 _tmp430;_tmp430.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp431;(_tmp431.ReachableFL).tag=1;(_tmp431.ReachableFL).f1=
_tmp3A9;(_tmp431.ReachableFL).f2=_tmp42F;(_tmp431.ReachableFL).f3=_tmp3AB;
_tmp431;});_tmp430.f2=_tmp3F8;_tmp430;});}_LL2D2:;_LL2D3: return({struct _tuple8
_tmp435;_tmp435.f1=_tmp3F7;_tmp435.f2=_tmp3F8;_tmp435;});_LL2C9:;}_LL2C7:;_LL2C8:
return({struct _tuple8 _tmp436;_tmp436.f1=_tmp3F7;_tmp436.f2=_tmp3F8;_tmp436;});
_LL2BA:;}_LL2B8:;_LL2B9: return({struct _tuple8 _tmp437;_tmp437.f1=_tmp3F7;_tmp437.f2=
_tmp3F8;_tmp437;});_LL2AF:;}}_LL2A6: if((int)_tmp3EC != 10)goto _LL2A8;_LL2A7: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp438=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3AD,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp439=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3AE,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp43A=_tmp438;
struct Cyc_Dict_Dict _tmp43B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp43C;_LL2D5: if((
_tmp43A.BottomFL).tag != 0)goto _LL2D7;_LL2D6:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_Core_Impossible_struct
_tmp43E;_tmp43E.tag=Cyc_Core_Impossible;_tmp43E.f1=({const char*_tmp43F="anal_test, Lte";
_tag_dynforward(_tmp43F,sizeof(char),_get_zero_arr_size(_tmp43F,15));});_tmp43E;});
_tmp43D;}));_LL2D7: if((_tmp43A.ReachableFL).tag != 1)goto _LL2D4;_tmp43B=(_tmp43A.ReachableFL).f1;
_tmp43C=(_tmp43A.ReachableFL).f3;_LL2D8: _tmp3A9=_tmp43B;_tmp3AB=_tmp43C;_LL2D4:;}{
void*_tmp440=(void*)_tmp3AD->r;void*_tmp441;struct Cyc_Absyn_Vardecl*_tmp442;void*
_tmp443;struct Cyc_Absyn_Vardecl*_tmp444;void*_tmp445;struct Cyc_Absyn_Vardecl*
_tmp446;void*_tmp447;struct Cyc_Absyn_Vardecl*_tmp448;_LL2DA: if(*((int*)_tmp440)
!= 1)goto _LL2DC;_tmp441=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;if(
_tmp441 <= (void*)1)goto _LL2DC;if(*((int*)_tmp441)!= 0)goto _LL2DC;_tmp442=((
struct Cyc_Absyn_Global_b_struct*)_tmp441)->f1;if(!(!_tmp442->escapes))goto _LL2DC;
_LL2DB: _tmp444=_tmp442;goto _LL2DD;_LL2DC: if(*((int*)_tmp440)!= 1)goto _LL2DE;
_tmp443=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;if(_tmp443 <= (void*)
1)goto _LL2DE;if(*((int*)_tmp443)!= 3)goto _LL2DE;_tmp444=((struct Cyc_Absyn_Local_b_struct*)
_tmp443)->f1;if(!(!_tmp444->escapes))goto _LL2DE;_LL2DD: _tmp446=_tmp444;goto
_LL2DF;_LL2DE: if(*((int*)_tmp440)!= 1)goto _LL2E0;_tmp445=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp440)->f2;if(_tmp445 <= (void*)1)goto _LL2E0;if(*((int*)_tmp445)!= 4)goto _LL2E0;
_tmp446=((struct Cyc_Absyn_Pat_b_struct*)_tmp445)->f1;if(!(!_tmp446->escapes))
goto _LL2E0;_LL2DF: _tmp448=_tmp446;goto _LL2E1;_LL2E0: if(*((int*)_tmp440)!= 1)goto
_LL2E2;_tmp447=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;if(_tmp447 <= (
void*)1)goto _LL2E2;if(*((int*)_tmp447)!= 2)goto _LL2E2;_tmp448=((struct Cyc_Absyn_Param_b_struct*)
_tmp447)->f1;if(!(!_tmp448->escapes))goto _LL2E2;_LL2E1: {void*_tmp449=(void*)
_tmp3AE->r;void*_tmp44A;struct Cyc_List_List*_tmp44B;struct Cyc_List_List _tmp44C;
struct Cyc_Absyn_Exp*_tmp44D;_LL2E5: if(*((int*)_tmp449)!= 3)goto _LL2E7;_tmp44A=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp449)->f1;if((int)_tmp44A != 19)goto
_LL2E7;_tmp44B=((struct Cyc_Absyn_Primop_e_struct*)_tmp449)->f2;if(_tmp44B == 0)
goto _LL2E7;_tmp44C=*_tmp44B;_tmp44D=(struct Cyc_Absyn_Exp*)_tmp44C.hd;_LL2E6: {
void*_tmp44E=(void*)_tmp44D->r;void*_tmp44F;struct Cyc_Absyn_Vardecl*_tmp450;void*
_tmp451;struct Cyc_Absyn_Vardecl*_tmp452;void*_tmp453;struct Cyc_Absyn_Vardecl*
_tmp454;void*_tmp455;struct Cyc_Absyn_Vardecl*_tmp456;_LL2EA: if(*((int*)_tmp44E)
!= 1)goto _LL2EC;_tmp44F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44E)->f2;if(
_tmp44F <= (void*)1)goto _LL2EC;if(*((int*)_tmp44F)!= 0)goto _LL2EC;_tmp450=((
struct Cyc_Absyn_Global_b_struct*)_tmp44F)->f1;if(!(!_tmp450->escapes))goto _LL2EC;
_LL2EB: _tmp452=_tmp450;goto _LL2ED;_LL2EC: if(*((int*)_tmp44E)!= 1)goto _LL2EE;
_tmp451=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44E)->f2;if(_tmp451 <= (void*)
1)goto _LL2EE;if(*((int*)_tmp451)!= 3)goto _LL2EE;_tmp452=((struct Cyc_Absyn_Local_b_struct*)
_tmp451)->f1;if(!(!_tmp452->escapes))goto _LL2EE;_LL2ED: _tmp454=_tmp452;goto
_LL2EF;_LL2EE: if(*((int*)_tmp44E)!= 1)goto _LL2F0;_tmp453=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp44E)->f2;if(_tmp453 <= (void*)1)goto _LL2F0;if(*((int*)_tmp453)!= 4)goto _LL2F0;
_tmp454=((struct Cyc_Absyn_Pat_b_struct*)_tmp453)->f1;if(!(!_tmp454->escapes))
goto _LL2F0;_LL2EF: _tmp456=_tmp454;goto _LL2F1;_LL2F0: if(*((int*)_tmp44E)!= 1)goto
_LL2F2;_tmp455=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44E)->f2;if(_tmp455 <= (
void*)1)goto _LL2F2;if(*((int*)_tmp455)!= 2)goto _LL2F2;_tmp456=((struct Cyc_Absyn_Param_b_struct*)
_tmp455)->f1;if(!(!_tmp456->escapes))goto _LL2F2;_LL2F1: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp457=env->fenv;struct Cyc_List_List*_tmp458=({struct Cyc_List_List*_tmp45B=
_region_malloc(_tmp457->r,sizeof(*_tmp45B));_tmp45B->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp45C=_region_malloc(_tmp457->r,sizeof(*_tmp45C));_tmp45C->vd=_tmp448;_tmp45C->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp45D;(
_tmp45D.LessEqSize).tag=4;(_tmp45D.LessEqSize).f1=_tmp456;_tmp45D;});_tmp45C;});
_tmp45B->tl=_tmp3AA;_tmp45B;});return({struct _tuple8 _tmp459;_tmp459.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp45A;(_tmp45A.ReachableFL).tag=1;(_tmp45A.ReachableFL).f1=
_tmp3A9;(_tmp45A.ReachableFL).f2=_tmp458;(_tmp45A.ReachableFL).f3=_tmp3AB;
_tmp45A;});_tmp459.f2=_tmp439;_tmp459;});}_LL2F2:;_LL2F3: return({struct _tuple8
_tmp45E;_tmp45E.f1=_tmp438;_tmp45E.f2=_tmp439;_tmp45E;});_LL2E9:;}_LL2E7:;_LL2E8:
return({struct _tuple8 _tmp45F;_tmp45F.f1=_tmp438;_tmp45F.f2=_tmp439;_tmp45F;});
_LL2E4:;}_LL2E2:;_LL2E3: return({struct _tuple8 _tmp460;_tmp460.f1=_tmp438;_tmp460.f2=
_tmp439;_tmp460;});_LL2D9:;}}_LL2A8:;_LL2A9: return({struct _tuple8 _tmp461;_tmp461.f1=
f;_tmp461.f2=f;_tmp461;});_LL29B:;}}_LL268:;}}_LL25F:;_LL260: goto _LL252;_LL252:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp463;void*_tmp464;struct _tuple5 _tmp462=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;_tmp463=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp463);{union Cyc_CfFlowInfo_FlowInfo_union _tmp465=_tmp463;struct Cyc_Dict_Dict
_tmp466;_LL2F5: if((_tmp465.BottomFL).tag != 0)goto _LL2F7;_LL2F6: return({struct
_tuple8 _tmp467;_tmp467.f1=_tmp463;_tmp467.f2=_tmp463;_tmp467;});_LL2F7: if((
_tmp465.ReachableFL).tag != 1)goto _LL2F4;_tmp466=(_tmp465.ReachableFL).f1;_LL2F8: {
void*_tmp468=_tmp464;void*_tmp469;void*_tmp46A;void*_tmp46B;_LL2FA: if((int)
_tmp468 != 0)goto _LL2FC;_LL2FB: return({struct _tuple8 _tmp46C;_tmp46C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp46D;(_tmp46D.BottomFL).tag=0;_tmp46D;});
_tmp46C.f2=_tmp463;_tmp46C;});_LL2FC: if((int)_tmp468 != 2)goto _LL2FE;_LL2FD: goto
_LL2FF;_LL2FE: if((int)_tmp468 != 1)goto _LL300;_LL2FF: goto _LL301;_LL300: if(_tmp468
<= (void*)3)goto _LL302;if(*((int*)_tmp468)!= 2)goto _LL302;_LL301: return({struct
_tuple8 _tmp46E;_tmp46E.f1=_tmp463;_tmp46E.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp46F;(_tmp46F.BottomFL).tag=0;_tmp46F;});
_tmp46E;});_LL302: if(_tmp468 <= (void*)3)goto _LL304;if(*((int*)_tmp468)!= 0)goto
_LL304;_tmp469=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp468)->f1;if((
int)_tmp469 != 0)goto _LL304;_LL303: goto _LL305;_LL304: if(_tmp468 <= (void*)3)goto
_LL306;if(*((int*)_tmp468)!= 1)goto _LL306;_tmp46A=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp468)->f1;if((int)_tmp46A != 0)goto _LL306;_LL305:({void*_tmp470[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp471="expression may not be initialized";_tag_dynforward(
_tmp471,sizeof(char),_get_zero_arr_size(_tmp471,34));}),_tag_dynforward(_tmp470,
sizeof(void*),0));});return({struct _tuple8 _tmp472;_tmp472.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp474;(_tmp474.BottomFL).tag=0;_tmp474;});
_tmp472.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp473;(_tmp473.BottomFL).tag=0;_tmp473;});_tmp472;});_LL306: if(_tmp468 <= (void*)
3)goto _LL308;if(*((int*)_tmp468)!= 0)goto _LL308;_tmp46B=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp468)->f1;_LL307: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp463,e,
_tmp46B);_LL308: if(_tmp468 <= (void*)3)goto _LL30A;if(*((int*)_tmp468)!= 1)goto
_LL30A;_LL309: return({struct _tuple8 _tmp475;_tmp475.f1=_tmp463;_tmp475.f2=_tmp463;
_tmp475;});_LL30A: if(_tmp468 <= (void*)3)goto _LL30C;if(*((int*)_tmp468)!= 3)goto
_LL30C;_LL30B: return({struct _tuple8 _tmp476;_tmp476.f1=_tmp463;_tmp476.f2=_tmp463;
_tmp476;});_LL30C: if(_tmp468 <= (void*)3)goto _LL2F9;if(*((int*)_tmp468)!= 4)goto
_LL2F9;_LL30D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp477=
_cycalloc(sizeof(*_tmp477));_tmp477[0]=({struct Cyc_Core_Impossible_struct _tmp478;
_tmp478.tag=Cyc_Core_Impossible;_tmp478.f1=({const char*_tmp479="anal_test";
_tag_dynforward(_tmp479,sizeof(char),_get_zero_arr_size(_tmp479,10));});_tmp478;});
_tmp477;}));_LL2F9:;}_LL2F4:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp47A=flow;struct Cyc_Dict_Dict _tmp47B;
_LL30F: if((_tmp47A.BottomFL).tag != 0)goto _LL311;_LL310: return;_LL311: if((_tmp47A.ReachableFL).tag
!= 1)goto _LL30E;_tmp47B=(_tmp47A.ReachableFL).f1;_LL312:{struct Cyc_List_List*
_tmp47C=env->param_roots;for(0;_tmp47C != 0;_tmp47C=_tmp47C->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp47B,Cyc_CfFlowInfo_lookup_place(_tmp47B,(struct Cyc_CfFlowInfo_Place*)_tmp47C->hd))
!= (void*)2)({void*_tmp47D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp47E="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp47E,sizeof(char),_get_zero_arr_size(_tmp47E,76));}),
_tag_dynforward(_tmp47D,sizeof(void*),0));});}}return;_LL30E:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp47F=env->fenv;
for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp481;struct Cyc_Core_Opt*
_tmp482;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Stmt*_tmp484;struct Cyc_Position_Segment*
_tmp485;struct Cyc_Absyn_Switch_clause*_tmp480=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp481=*_tmp480;_tmp482=_tmp481.pat_vars;_tmp483=_tmp481.where_clause;
_tmp484=_tmp481.body;_tmp485=_tmp481.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp47F,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp482))->v,(void*)_tmp47F->unknown_all,_tmp485);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp483 != 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp487;union Cyc_CfFlowInfo_FlowInfo_union _tmp488;struct _tuple8 _tmp486=Cyc_NewControlFlow_anal_test(
env,clause_inflow,(struct Cyc_Absyn_Exp*)_tmp483);_tmp487=_tmp486.f1;_tmp488=
_tmp486.f2;inflow=_tmp488;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
_tmp487,_tmp484);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp484);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp489=
clause_outflow;_LL314: if((_tmp489.BottomFL).tag != 0)goto _LL316;_LL315: goto _LL313;
_LL316:;_LL317: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp48A[0]={};Cyc_Tcutil_terr(
_tmp484->loc,({const char*_tmp48B="switch clause may implicitly fallthru";
_tag_dynforward(_tmp48B,sizeof(char),_get_zero_arr_size(_tmp48B,38));}),
_tag_dynforward(_tmp48A,sizeof(void*),0));});else{({void*_tmp48C[0]={};Cyc_Tcutil_warn(
_tmp484->loc,({const char*_tmp48D="switch clause may implicitly fallthru";
_tag_dynforward(_tmp48D,sizeof(char),_get_zero_arr_size(_tmp48D,38));}),
_tag_dynforward(_tmp48C,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL313;_LL313:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp48E;(_tmp48E.BottomFL).tag=0;_tmp48E;});}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp490;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp491;struct _tuple9 _tmp48F=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp490=_tmp48F.f1;_tmp491=_tmp48F.f2;inflow=*_tmp491;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp492=env->fenv;{void*_tmp493=(void*)s->r;struct Cyc_Absyn_Exp*_tmp494;struct
Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Stmt*_tmp497;
struct Cyc_Absyn_Stmt*_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Stmt*
_tmp49A;struct Cyc_Absyn_Stmt*_tmp49B;struct _tuple3 _tmp49C;struct Cyc_Absyn_Exp*
_tmp49D;struct Cyc_Absyn_Stmt*_tmp49E;struct Cyc_Absyn_Stmt*_tmp49F;struct Cyc_Absyn_Stmt*
_tmp4A0;struct _tuple3 _tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Stmt*
_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct _tuple3 _tmp4A5;struct Cyc_Absyn_Exp*
_tmp4A6;struct Cyc_Absyn_Stmt*_tmp4A7;struct _tuple3 _tmp4A8;struct Cyc_Absyn_Exp*
_tmp4A9;struct Cyc_Absyn_Stmt*_tmp4AA;struct Cyc_Absyn_Stmt*_tmp4AB;struct Cyc_Absyn_Stmt*
_tmp4AC;struct Cyc_List_List*_tmp4AD;struct Cyc_Absyn_Switch_clause**_tmp4AE;
struct Cyc_Absyn_Switch_clause*_tmp4AF;struct Cyc_Absyn_Stmt*_tmp4B0;struct Cyc_Absyn_Stmt*
_tmp4B1;struct Cyc_Absyn_Stmt*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_List_List*
_tmp4B4;struct Cyc_Absyn_Stmt*_tmp4B5;struct Cyc_List_List*_tmp4B6;struct Cyc_Absyn_Decl*
_tmp4B7;struct Cyc_Absyn_Stmt*_tmp4B8;struct Cyc_Absyn_Stmt*_tmp4B9;struct Cyc_Absyn_Tvar*
_tmp4BA;struct Cyc_Absyn_Vardecl*_tmp4BB;int _tmp4BC;struct Cyc_Absyn_Exp*_tmp4BD;
struct Cyc_Absyn_Stmt*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*
_tmp4C0;struct Cyc_Absyn_Tvar*_tmp4C1;struct Cyc_Absyn_Vardecl*_tmp4C2;struct Cyc_Absyn_Stmt*
_tmp4C3;_LL319: if((int)_tmp493 != 0)goto _LL31B;_LL31A: return inflow;_LL31B: if(
_tmp493 <= (void*)1)goto _LL343;if(*((int*)_tmp493)!= 2)goto _LL31D;_tmp494=((
struct Cyc_Absyn_Return_s_struct*)_tmp493)->f1;if(_tmp494 != 0)goto _LL31D;_LL31C:
if(env->noreturn)({void*_tmp4C4[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4C5="`noreturn' function might return";
_tag_dynforward(_tmp4C5,sizeof(char),_get_zero_arr_size(_tmp4C5,33));}),
_tag_dynforward(_tmp4C4,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C6;(_tmp4C6.BottomFL).tag=0;_tmp4C6;});_LL31D: if(*((int*)_tmp493)!= 2)goto
_LL31F;_tmp495=((struct Cyc_Absyn_Return_s_struct*)_tmp493)->f1;_LL31E: if(env->noreturn)({
void*_tmp4C7[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4C8="`noreturn' function might return";
_tag_dynforward(_tmp4C8,sizeof(char),_get_zero_arr_size(_tmp4C8,33));}),
_tag_dynforward(_tmp4C7,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CA;void*_tmp4CB;struct _tuple5 _tmp4C9=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp495));_tmp4CA=_tmp4C9.f1;_tmp4CB=
_tmp4C9.f2;_tmp4CA=Cyc_CfFlowInfo_consume_unique_rvals(_tmp495->loc,_tmp4CA);
_tmp4CA=Cyc_NewControlFlow_use_Rval(env,_tmp495->loc,_tmp4CA,_tmp4CB);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp4CA);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CC;(_tmp4CC.BottomFL).tag=0;_tmp4CC;});}_LL31F: if(*((int*)_tmp493)!= 0)goto
_LL321;_tmp496=((struct Cyc_Absyn_Exp_s_struct*)_tmp493)->f1;_LL320: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp496)).f1;goto _LL318;_LL321: if(*((int*)_tmp493)!= 1)goto _LL323;
_tmp497=((struct Cyc_Absyn_Seq_s_struct*)_tmp493)->f1;_tmp498=((struct Cyc_Absyn_Seq_s_struct*)
_tmp493)->f2;_LL322: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp497),_tmp498);goto _LL318;_LL323: if(*((int*)_tmp493)!= 3)goto
_LL325;_tmp499=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp493)->f1;_tmp49A=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp493)->f2;_tmp49B=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp493)->f3;_LL324: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4CE;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CF;struct _tuple8 _tmp4CD=Cyc_NewControlFlow_anal_test(env,inflow,_tmp499);
_tmp4CE=_tmp4CD.f1;_tmp4CF=_tmp4CD.f2;_tmp4CE=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp499->loc,_tmp4CE);_tmp4CF=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp499->loc,
_tmp4CF);outflow=Cyc_CfFlowInfo_join_flow(_tmp492,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp4CE,_tmp49A),Cyc_NewControlFlow_anal_stmt(env,_tmp4CF,_tmp49B),1);goto
_LL318;}_LL325: if(*((int*)_tmp493)!= 4)goto _LL327;_tmp49C=((struct Cyc_Absyn_While_s_struct*)
_tmp493)->f1;_tmp49D=_tmp49C.f1;_tmp49E=_tmp49C.f2;_tmp49F=((struct Cyc_Absyn_While_s_struct*)
_tmp493)->f2;_LL326: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp4D1;struct _tuple9
_tmp4D0=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp49E);_tmp4D1=_tmp4D0.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4D2=*_tmp4D1;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4D4;union Cyc_CfFlowInfo_FlowInfo_union _tmp4D5;struct _tuple8 _tmp4D3=Cyc_NewControlFlow_anal_test(
env,_tmp4D2,_tmp49D);_tmp4D4=_tmp4D3.f1;_tmp4D5=_tmp4D3.f2;_tmp4D4=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp49D->loc,_tmp4D4);_tmp4D5=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp49D->loc,
_tmp4D5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4D6=Cyc_NewControlFlow_anal_stmt(
env,_tmp4D4,_tmp49F);Cyc_NewControlFlow_update_flow(env,_tmp49E,_tmp4D6);outflow=
_tmp4D5;goto _LL318;}}}_LL327: if(*((int*)_tmp493)!= 13)goto _LL329;_tmp4A0=((
struct Cyc_Absyn_Do_s_struct*)_tmp493)->f1;_tmp4A1=((struct Cyc_Absyn_Do_s_struct*)
_tmp493)->f2;_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;_LL328: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp4D7=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4A0);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4D9;struct _tuple9 _tmp4D8=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4D7,
_tmp4A3);_tmp4D9=_tmp4D8.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4DA=*_tmp4D9;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4DC;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4DD;struct _tuple8 _tmp4DB=Cyc_NewControlFlow_anal_test(env,_tmp4DA,_tmp4A2);
_tmp4DC=_tmp4DB.f1;_tmp4DD=_tmp4DB.f2;Cyc_NewControlFlow_update_flow(env,_tmp4A0,
_tmp4DC);outflow=_tmp4DD;goto _LL318;}}_LL329: if(*((int*)_tmp493)!= 8)goto _LL32B;
_tmp4A4=((struct Cyc_Absyn_For_s_struct*)_tmp493)->f1;_tmp4A5=((struct Cyc_Absyn_For_s_struct*)
_tmp493)->f2;_tmp4A6=_tmp4A5.f1;_tmp4A7=_tmp4A5.f2;_tmp4A8=((struct Cyc_Absyn_For_s_struct*)
_tmp493)->f3;_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;_tmp4AB=((struct Cyc_Absyn_For_s_struct*)
_tmp493)->f4;_LL32A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4DE=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4A4)).f1;_tmp4DE=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4A4->loc,
_tmp4DE);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp4E0;struct _tuple9 _tmp4DF=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp4DE,_tmp4A7);_tmp4E0=_tmp4DF.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E1=*_tmp4E0;union Cyc_CfFlowInfo_FlowInfo_union _tmp4E3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E4;struct _tuple8 _tmp4E2=Cyc_NewControlFlow_anal_test(env,_tmp4E1,_tmp4A6);
_tmp4E3=_tmp4E2.f1;_tmp4E4=_tmp4E2.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E5=
Cyc_NewControlFlow_anal_stmt(env,_tmp4E3,_tmp4AB);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4E7;struct _tuple9 _tmp4E6=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4E5,
_tmp4AA);_tmp4E7=_tmp4E6.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E8=*_tmp4E7;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4E9=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp4E8,_tmp4A9)).f1;_tmp4E9=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4A9->loc,
_tmp4E9);Cyc_NewControlFlow_update_flow(env,_tmp4A7,_tmp4E9);outflow=_tmp4E4;
goto _LL318;}}}}}_LL32B: if(*((int*)_tmp493)!= 5)goto _LL32D;_tmp4AC=((struct Cyc_Absyn_Break_s_struct*)
_tmp493)->f1;if(_tmp4AC != 0)goto _LL32D;_LL32C: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4EA;(_tmp4EA.BottomFL).tag=0;_tmp4EA;});
_LL32D: if(*((int*)_tmp493)!= 10)goto _LL32F;_tmp4AD=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp493)->f1;_tmp4AE=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp493)->f2;if(
_tmp4AE == 0)goto _LL32F;_tmp4AF=*_tmp4AE;_LL32E: {struct _RegionHandle _tmp4EB=
_new_region("temp");struct _RegionHandle*temp=& _tmp4EB;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp4ED;struct Cyc_List_List*_tmp4EE;struct _tuple11
_tmp4EC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4AD,0);
_tmp4ED=_tmp4EC.f1;_tmp4EE=_tmp4EC.f2;for(0;_tmp4EE != 0;(_tmp4EE=_tmp4EE->tl,
_tmp4AD=_tmp4AD->tl)){_tmp4ED=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4AD))->hd)->loc,_tmp4ED,(void*)_tmp4EE->hd);}
_tmp4ED=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp4ED);_tmp4ED=Cyc_NewControlFlow_add_vars(
_tmp492,_tmp4ED,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4AF->pat_vars))->v,(
void*)_tmp492->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4AF->body,_tmp4ED);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4F0=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4EF;(_tmp4EF.BottomFL).tag=0;_tmp4EF;});
_npop_handler(0);return _tmp4F0;}};_pop_region(temp);}_LL32F: if(*((int*)_tmp493)
!= 5)goto _LL331;_tmp4B0=((struct Cyc_Absyn_Break_s_struct*)_tmp493)->f1;_LL330:
_tmp4B1=_tmp4B0;goto _LL332;_LL331: if(*((int*)_tmp493)!= 6)goto _LL333;_tmp4B1=((
struct Cyc_Absyn_Continue_s_struct*)_tmp493)->f1;_LL332: _tmp4B2=_tmp4B1;goto
_LL334;_LL333: if(*((int*)_tmp493)!= 7)goto _LL335;_tmp4B2=((struct Cyc_Absyn_Goto_s_struct*)
_tmp493)->f2;_LL334: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp4F1=
_tmp490->encloser;struct Cyc_Absyn_Stmt*_tmp4F2=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4B2)))->encloser;while(_tmp4F2 != _tmp4F1){
struct Cyc_Absyn_Stmt*_tmp4F3=(Cyc_NewControlFlow_get_stmt_annot(_tmp4F1))->encloser;
if(_tmp4F3 == _tmp4F1){({void*_tmp4F4[0]={};Cyc_Tcutil_terr(s->loc,({const char*
_tmp4F5="goto enters local scope or exception handler";_tag_dynforward(_tmp4F5,
sizeof(char),_get_zero_arr_size(_tmp4F5,45));}),_tag_dynforward(_tmp4F4,sizeof(
void*),0));});break;}_tmp4F1=_tmp4F3;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp4B2),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4F6;(_tmp4F6.BottomFL).tag=0;_tmp4F6;});
_LL335: if(*((int*)_tmp493)!= 9)goto _LL337;_tmp4B3=((struct Cyc_Absyn_Switch_s_struct*)
_tmp493)->f1;_tmp4B4=((struct Cyc_Absyn_Switch_s_struct*)_tmp493)->f2;_LL336: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp4F8;void*_tmp4F9;struct _tuple5 _tmp4F7=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4B3);_tmp4F8=_tmp4F7.f1;_tmp4F9=_tmp4F7.f2;_tmp4F8=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4B3->loc,_tmp4F8);_tmp4F8=Cyc_NewControlFlow_use_Rval(env,_tmp4B3->loc,
_tmp4F8,_tmp4F9);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp4F8,_tmp4B4);goto
_LL318;}_LL337: if(*((int*)_tmp493)!= 14)goto _LL339;_tmp4B5=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp493)->f1;_tmp4B6=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp493)->f2;_LL338: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4B5);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4B6);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4FA=scs_outflow;
_LL346: if((_tmp4FA.BottomFL).tag != 0)goto _LL348;_LL347: goto _LL345;_LL348:;_LL349:({
void*_tmp4FB[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4FC="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp4FC,sizeof(char),_get_zero_arr_size(_tmp4FC,42));}),
_tag_dynforward(_tmp4FB,sizeof(void*),0));});_LL345:;}outflow=s1_outflow;goto
_LL318;}}}_LL339: if(*((int*)_tmp493)!= 11)goto _LL33B;_tmp4B7=((struct Cyc_Absyn_Decl_s_struct*)
_tmp493)->f1;_tmp4B8=((struct Cyc_Absyn_Decl_s_struct*)_tmp493)->f2;_LL33A:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4B7),_tmp4B8);goto _LL318;_LL33B: if(*((int*)_tmp493)!= 12)goto _LL33D;_tmp4B9=((
struct Cyc_Absyn_Label_s_struct*)_tmp493)->f2;_LL33C: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4B9);goto _LL318;_LL33D: if(*((int*)_tmp493)!= 15)goto _LL33F;
_tmp4BA=((struct Cyc_Absyn_Region_s_struct*)_tmp493)->f1;_tmp4BB=((struct Cyc_Absyn_Region_s_struct*)
_tmp493)->f2;_tmp4BC=((struct Cyc_Absyn_Region_s_struct*)_tmp493)->f3;_tmp4BD=((
struct Cyc_Absyn_Region_s_struct*)_tmp493)->f4;_tmp4BE=((struct Cyc_Absyn_Region_s_struct*)
_tmp493)->f5;_LL33E: if(_tmp4BD != 0){struct Cyc_Absyn_Exp*_tmp4FD=(struct Cyc_Absyn_Exp*)
_tmp4BD;union Cyc_CfFlowInfo_FlowInfo_union _tmp4FF;void*_tmp500;struct _tuple5
_tmp4FE=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4FD);_tmp4FF=_tmp4FE.f1;
_tmp500=_tmp4FE.f2;_tmp4FF=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4FD->loc,
_tmp4FF);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp4FD->loc,_tmp4FF,_tmp500);}{
struct Cyc_List_List _tmp501=({struct Cyc_List_List _tmp502;_tmp502.hd=_tmp4BB;
_tmp502.tl=0;_tmp502;});inflow=Cyc_NewControlFlow_add_vars(_tmp492,inflow,(
struct Cyc_List_List*)& _tmp501,(void*)_tmp492->unknown_all,_tmp4BE->loc);outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4BE);goto _LL318;}_LL33F: if(*((int*)
_tmp493)!= 16)goto _LL341;_tmp4BF=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp493)->f1;
_LL340: {union Cyc_CfFlowInfo_FlowInfo_union _tmp504;void*_tmp505;struct _tuple5
_tmp503=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4BF);_tmp504=_tmp503.f1;
_tmp505=_tmp503.f2;_tmp504=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4BF->loc,
_tmp504);{union Cyc_CfFlowInfo_FlowInfo_union _tmp506=Cyc_NewControlFlow_use_Rval(
env,_tmp4BF->loc,_tmp504,_tmp505);{void*_tmp507=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4BF->topt))->v);void*_tmp508;_LL34B: if(
_tmp507 <= (void*)4)goto _LL34D;if(*((int*)_tmp507)!= 15)goto _LL34D;_tmp508=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp507)->f1;_LL34C: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp492,_tmp504,_tmp508);goto _LL34A;_LL34D:;_LL34E:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509[0]=({
struct Cyc_Core_Impossible_struct _tmp50A;_tmp50A.tag=Cyc_Core_Impossible;_tmp50A.f1=({
const char*_tmp50B="anal_stmt -- reset_region";_tag_dynforward(_tmp50B,sizeof(
char),_get_zero_arr_size(_tmp50B,26));});_tmp50A;});_tmp509;}));_LL34A:;}goto
_LL318;}}_LL341: if(*((int*)_tmp493)!= 17)goto _LL343;_tmp4C0=((struct Cyc_Absyn_Alias_s_struct*)
_tmp493)->f1;_tmp4C1=((struct Cyc_Absyn_Alias_s_struct*)_tmp493)->f2;_tmp4C2=((
struct Cyc_Absyn_Alias_s_struct*)_tmp493)->f3;_tmp4C3=((struct Cyc_Absyn_Alias_s_struct*)
_tmp493)->f4;_LL342: {union Cyc_CfFlowInfo_FlowInfo_union _tmp50D;void*_tmp50E;
struct _tuple5 _tmp50C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4C0);_tmp50D=
_tmp50C.f1;_tmp50E=_tmp50C.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp510;struct Cyc_List_List*
_tmp511;struct _tuple6 _tmp50F=Cyc_CfFlowInfo_save_consume_info(_tmp492,_tmp50D,0);
_tmp510=_tmp50F.f1;_tmp511=_tmp510.may_consume;_tmp50D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4C0->loc,_tmp50D);_tmp50D=Cyc_NewControlFlow_use_Rval(env,_tmp4C0->loc,
_tmp50D,_tmp50E);{struct Cyc_List_List _tmp512=({struct Cyc_List_List _tmp519;
_tmp519.hd=_tmp4C2;_tmp519.tl=0;_tmp519;});_tmp50D=Cyc_NewControlFlow_add_vars(
_tmp492,_tmp50D,(struct Cyc_List_List*)& _tmp512,(void*)_tmp492->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp50D,_tmp4C3);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp513=outflow;struct Cyc_Dict_Dict _tmp514;struct Cyc_List_List*_tmp515;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp516;_LL350: if((_tmp513.BottomFL).tag != 0)goto _LL352;_LL351: goto _LL34F;_LL352:
if((_tmp513.ReachableFL).tag != 1)goto _LL34F;_tmp514=(_tmp513.ReachableFL).f1;
_tmp515=(_tmp513.ReachableFL).f2;_tmp516=(_tmp513.ReachableFL).f3;_LL353: while(
_tmp511 != 0){struct Cyc_Dict_Dict _tmp517=_tmp516.consumed;_tmp516.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp492->r,_tmp516.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp511->hd);if((_tmp516.consumed).t != _tmp517.t);_tmp511=_tmp511->tl;}outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp518;(
_tmp518.ReachableFL).tag=1;(_tmp518.ReachableFL).f1=_tmp514;(_tmp518.ReachableFL).f2=
_tmp515;(_tmp518.ReachableFL).f3=_tmp516;_tmp518;});goto _LL34F;_LL34F:;}goto
_LL318;}}}_LL343:;_LL344:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A[0]=({struct Cyc_Core_Impossible_struct
_tmp51B;_tmp51B.tag=Cyc_Core_Impossible;_tmp51B.f1=({const char*_tmp51C="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp51C,sizeof(char),_get_zero_arr_size(_tmp51C,56));});_tmp51B;});
_tmp51A;}));_LL318:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp51D=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp51E;_LL355: if((_tmp51D.ReachableFL).tag != 1)goto _LL357;_tmp51E=(_tmp51D.ReachableFL).f3;
_LL356: goto _LL354;_LL357:;_LL358: goto _LL354;_LL354:;}return outflow;}}static void
Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp51F=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp520;struct Cyc_Core_Opt*_tmp521;struct Cyc_Core_Opt _tmp522;struct Cyc_List_List*
_tmp523;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_List_List*_tmp525;struct Cyc_Absyn_Fndecl*
_tmp526;_LL35A: if(_tmp51F <= (void*)2)goto _LL362;if(*((int*)_tmp51F)!= 0)goto
_LL35C;_tmp520=((struct Cyc_Absyn_Var_d_struct*)_tmp51F)->f1;_LL35B: {struct Cyc_List_List
_tmp527=({struct Cyc_List_List _tmp536;_tmp536.hd=_tmp520;_tmp536.tl=0;_tmp536;});
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp527,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp528=
_tmp520->initializer;if(_tmp528 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp52A;void*_tmp52B;struct _tuple5 _tmp529=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp528);_tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;
_tmp52A=Cyc_CfFlowInfo_consume_unique_rvals(_tmp528->loc,_tmp52A);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp52C=_tmp52A;struct Cyc_Dict_Dict _tmp52D;struct Cyc_List_List*_tmp52E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp52F;_LL365: if((_tmp52C.BottomFL).tag != 0)goto _LL367;_LL366: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp530;(_tmp530.BottomFL).tag=0;_tmp530;});
_LL367: if((_tmp52C.ReachableFL).tag != 1)goto _LL364;_tmp52D=(_tmp52C.ReachableFL).f1;
_tmp52E=(_tmp52C.ReachableFL).f2;_tmp52F=(_tmp52C.ReachableFL).f3;_LL368: _tmp52D=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp52D,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp531=_region_malloc(env->r,sizeof(*_tmp531));
_tmp531->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp532=
_region_malloc(env->r,sizeof(*_tmp532));_tmp532[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp533;_tmp533.tag=0;_tmp533.f1=_tmp520;_tmp533;});_tmp532;}));_tmp531->fields=
0;_tmp531;}),_tmp52B);_tmp52E=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp52E,
_tmp520,(struct Cyc_Absyn_Exp*)_check_null(_tmp520->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp534=({union Cyc_CfFlowInfo_FlowInfo_union _tmp535;(_tmp535.ReachableFL).tag=1;(
_tmp535.ReachableFL).f1=_tmp52D;(_tmp535.ReachableFL).f2=_tmp52E;(_tmp535.ReachableFL).f3=
_tmp52F;_tmp535;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp534);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp534;}_LL364:;}}}}_LL35C:
if(*((int*)_tmp51F)!= 2)goto _LL35E;_tmp521=((struct Cyc_Absyn_Let_d_struct*)
_tmp51F)->f2;if(_tmp521 == 0)goto _LL35E;_tmp522=*_tmp521;_tmp523=(struct Cyc_List_List*)
_tmp522.v;_tmp524=((struct Cyc_Absyn_Let_d_struct*)_tmp51F)->f3;_LL35D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp538;void*_tmp539;struct _tuple5 _tmp537=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp524);_tmp538=_tmp537.f1;_tmp539=_tmp537.f2;_tmp538=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp524->loc,_tmp538);_tmp538=Cyc_NewControlFlow_use_Rval(env,_tmp524->loc,
_tmp538,_tmp539);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp538,_tmp523,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL35E: if(*((int*)_tmp51F)!= 3)goto
_LL360;_tmp525=((struct Cyc_Absyn_Letv_d_struct*)_tmp51F)->f1;_LL35F: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp525,(void*)(env->fenv)->unknown_none,decl->loc);_LL360: if(*((
int*)_tmp51F)!= 1)goto _LL362;_tmp526=((struct Cyc_Absyn_Fn_d_struct*)_tmp51F)->f1;
_LL361: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp526);{void*
_tmp53A=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp526->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp53B=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp526->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp53C=
_region_malloc(env->r,sizeof(*_tmp53C));_tmp53C->hd=_tmp53B;_tmp53C->tl=0;
_tmp53C;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL362:;_LL363:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp53D=_cycalloc(sizeof(*
_tmp53D));_tmp53D[0]=({struct Cyc_Core_Impossible_struct _tmp53E;_tmp53E.tag=Cyc_Core_Impossible;
_tmp53E.f1=({const char*_tmp53F="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp53F,sizeof(char),_get_zero_arr_size(_tmp53F,35));});_tmp53E;});
_tmp53D;}));_LL359:;}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp540=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp540;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp541;(_tmp541.ReachableFL).tag=1;(_tmp541.ReachableFL).f1=
fenv->mt_flowdict;(_tmp541.ReachableFL).f2=0;(_tmp541.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp542;_tmp542.consumed=fenv->mt_place_set;_tmp542.may_consume=
0;_tmp542;});_tmp541;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp543=fenv->r;struct Cyc_Position_Segment*_tmp544=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp544);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp545=inflow;struct Cyc_Dict_Dict
_tmp546;struct Cyc_List_List*_tmp547;struct Cyc_CfFlowInfo_ConsumeInfo _tmp548;
_LL36A: if((_tmp545.BottomFL).tag != 0)goto _LL36C;_LL36B:({void*_tmp549[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp54A="check_fun";_tag_dynforward(_tmp54A,sizeof(char),
_get_zero_arr_size(_tmp54A,10));}),_tag_dynforward(_tmp549,sizeof(void*),0));});
_LL36C: if((_tmp545.ReachableFL).tag != 1)goto _LL369;_tmp546=(_tmp545.ReachableFL).f1;
_tmp547=(_tmp545.ReachableFL).f2;_tmp548=(_tmp545.ReachableFL).f3;_LL36D: {
struct Cyc_List_List*atts;{void*_tmp54B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp54C;struct Cyc_List_List*
_tmp54D;_LL36F: if(_tmp54B <= (void*)4)goto _LL371;if(*((int*)_tmp54B)!= 8)goto
_LL371;_tmp54C=((struct Cyc_Absyn_FnType_struct*)_tmp54B)->f1;_tmp54D=_tmp54C.attributes;
_LL370: atts=_tmp54D;goto _LL36E;_LL371:;_LL372:({void*_tmp54E[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp54F="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp54F,sizeof(char),_get_zero_arr_size(_tmp54F,50));}),_tag_dynforward(_tmp54E,
sizeof(void*),0));});_LL36E:;}for(0;atts != 0;atts=atts->tl){void*_tmp550=(void*)
atts->hd;int _tmp551;_LL374: if(_tmp550 <= (void*)17)goto _LL376;if(*((int*)_tmp550)
!= 4)goto _LL376;_tmp551=((struct Cyc_Absyn_Initializes_att_struct*)_tmp550)->f1;
_LL375: {unsigned int j=(unsigned int)_tmp551;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp552[0]={};Cyc_Tcutil_terr(_tmp544,({const char*_tmp553="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp553,sizeof(char),_get_zero_arr_size(_tmp553,51));}),
_tag_dynforward(_tmp552,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp554=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp555=Cyc_Tcutil_compress((void*)_tmp554->type);struct Cyc_Absyn_PtrInfo
_tmp556;void*_tmp557;struct Cyc_Absyn_PtrAtts _tmp558;struct Cyc_Absyn_Conref*
_tmp559;struct Cyc_Absyn_Conref*_tmp55A;struct Cyc_Absyn_Conref*_tmp55B;_LL379: if(
_tmp555 <= (void*)4)goto _LL37B;if(*((int*)_tmp555)!= 4)goto _LL37B;_tmp556=((
struct Cyc_Absyn_PointerType_struct*)_tmp555)->f1;_tmp557=(void*)_tmp556.elt_typ;
_tmp558=_tmp556.ptr_atts;_tmp559=_tmp558.nullable;_tmp55A=_tmp558.bounds;_tmp55B=
_tmp558.zero_term;_LL37A: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp559))({void*_tmp55C[0]={};Cyc_Tcutil_terr(_tmp544,({const char*_tmp55D="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp55D,sizeof(char),_get_zero_arr_size(_tmp55D,55));}),
_tag_dynforward(_tmp55C,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp55A))({
void*_tmp55E[0]={};Cyc_Tcutil_terr(_tmp544,({const char*_tmp55F="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp55F,sizeof(char),_get_zero_arr_size(_tmp55F,57));}),
_tag_dynforward(_tmp55E,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp55B))({void*_tmp560[0]={};Cyc_Tcutil_terr(_tmp544,({
const char*_tmp561="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp561,sizeof(char),_get_zero_arr_size(_tmp561,77));}),
_tag_dynforward(_tmp560,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp562=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp56A=_region_malloc(_tmp543,
sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp56B;
_tmp56B.tag=2;_tmp56B.f1=(int)j;_tmp56B.f2=(void*)_tmp557;_tmp56B;});_tmp56A;});
struct Cyc_CfFlowInfo_Place*_tmp563=({struct Cyc_CfFlowInfo_Place*_tmp569=
_region_malloc(_tmp543,sizeof(*_tmp569));_tmp569->root=(void*)((void*)_tmp562);
_tmp569->fields=0;_tmp569;});_tmp546=Cyc_Dict_insert(_tmp546,(void*)_tmp562,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp557,(void*)fenv->esc_none));_tmp546=Cyc_Dict_insert(_tmp546,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp564=_region_malloc(_tmp543,sizeof(*
_tmp564));_tmp564[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp565;_tmp565.tag=0;
_tmp565.f1=_tmp554;_tmp565;});_tmp564;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp566=_region_malloc(_tmp543,sizeof(*_tmp566));_tmp566[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp567;_tmp567.tag=2;_tmp567.f1=_tmp563;_tmp567;});_tmp566;}));param_roots=({
struct Cyc_List_List*_tmp568=_region_malloc(_tmp543,sizeof(*_tmp568));_tmp568->hd=
_tmp563;_tmp568->tl=param_roots;_tmp568;});goto _LL378;}_LL37B:;_LL37C:({void*
_tmp56C[0]={};Cyc_Tcutil_terr(_tmp544,({const char*_tmp56D="initializes attribute on non-pointer";
_tag_dynforward(_tmp56D,sizeof(char),_get_zero_arr_size(_tmp56D,37));}),
_tag_dynforward(_tmp56C,sizeof(void*),0));});_LL378:;}goto _LL373;}}_LL376:;
_LL377: goto _LL373;_LL373:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp56E;(_tmp56E.ReachableFL).tag=1;(_tmp56E.ReachableFL).f1=_tmp546;(_tmp56E.ReachableFL).f2=
_tmp547;(_tmp56E.ReachableFL).f3=_tmp548;_tmp56E;});}_LL369:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(
struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),
int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp543,33,(int(*)(
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp577=_region_malloc(_tmp543,sizeof(*_tmp577));_tmp577->r=_tmp543;_tmp577->fenv=
fenv;_tmp577->iterate_again=1;_tmp577->iteration_num=0;_tmp577->in_try=0;_tmp577->tryflow=
inflow;_tmp577->all_changed=0;_tmp577->noreturn=noreturn;_tmp577->param_roots=
param_roots;_tmp577->flow_table=flow_table;_tmp577;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56F=outflow;_LL37E: if((_tmp56F.BottomFL).tag != 0)goto _LL380;_LL37F: goto
_LL37D;_LL380:;_LL381: Cyc_NewControlFlow_check_init_params(_tmp544,env,outflow);
if(noreturn)({void*_tmp570[0]={};Cyc_Tcutil_terr(_tmp544,({const char*_tmp571="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp571,sizeof(char),_get_zero_arr_size(_tmp571,46));}),
_tag_dynforward(_tmp570,sizeof(void*),0));});else{void*_tmp572=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL383: if((int)_tmp572 != 0)goto _LL385;_LL384: goto _LL382;
_LL385: if(_tmp572 <= (void*)4)goto _LL387;if(*((int*)_tmp572)!= 6)goto _LL387;
_LL386: goto _LL388;_LL387: if((int)_tmp572 != 1)goto _LL389;_LL388: goto _LL38A;_LL389:
if(_tmp572 <= (void*)4)goto _LL38B;if(*((int*)_tmp572)!= 5)goto _LL38B;_LL38A:({
void*_tmp573[0]={};Cyc_Tcutil_warn(_tmp544,({const char*_tmp574="function may not return a value";
_tag_dynforward(_tmp574,sizeof(char),_get_zero_arr_size(_tmp574,32));}),
_tag_dynforward(_tmp573,sizeof(void*),0));});goto _LL382;_LL38B:;_LL38C:({void*
_tmp575[0]={};Cyc_Tcutil_terr(_tmp544,({const char*_tmp576="function may not return a value";
_tag_dynforward(_tmp576,sizeof(char),_get_zero_arr_size(_tmp576,32));}),
_tag_dynforward(_tmp575,sizeof(void*),0));});goto _LL382;_LL382:;}goto _LL37D;
_LL37D:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;
ds=ds->tl){void*_tmp578=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*
_tmp579;struct Cyc_List_List*_tmp57A;struct Cyc_List_List*_tmp57B;struct Cyc_List_List*
_tmp57C;_LL38E: if(_tmp578 <= (void*)2)goto _LL398;if(*((int*)_tmp578)!= 1)goto
_LL390;_tmp579=((struct Cyc_Absyn_Fn_d_struct*)_tmp578)->f1;_LL38F: Cyc_NewControlFlow_check_fun(
_tmp579);goto _LL38D;_LL390: if(*((int*)_tmp578)!= 10)goto _LL392;_tmp57A=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp578)->f1;_LL391: _tmp57B=_tmp57A;goto _LL393;
_LL392: if(*((int*)_tmp578)!= 9)goto _LL394;_tmp57B=((struct Cyc_Absyn_Using_d_struct*)
_tmp578)->f2;_LL393: _tmp57C=_tmp57B;goto _LL395;_LL394: if(*((int*)_tmp578)!= 8)
goto _LL396;_tmp57C=((struct Cyc_Absyn_Namespace_d_struct*)_tmp578)->f2;_LL395: Cyc_NewControlFlow_cf_check(
_tmp57C);goto _LL38D;_LL396: if(*((int*)_tmp578)!= 11)goto _LL398;_LL397: goto _LL38D;
_LL398:;_LL399: goto _LL38D;_LL38D:;}}
