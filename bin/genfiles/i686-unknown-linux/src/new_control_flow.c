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
struct Cyc_Dict_Dict d,void*k);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*
r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,
struct Cyc_Dict_Dict,void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dynforward_ptr*
s1,struct _dynforward_ptr*s2);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*
hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*
val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct
Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
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
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_unique_pointer(void*t);int Cyc_Tcutil_is_noreturn(
void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
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
_LL39:;}_LL34:;}static struct _tuple5 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,
struct Cyc_Absyn_Exp*e,void*r){void*_tmp8E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp8F;void*_tmp90;struct Cyc_Absyn_PtrAtts
_tmp91;struct Cyc_Absyn_Conref*_tmp92;struct Cyc_Absyn_Conref*_tmp93;_LL58: if(
_tmp8E <= (void*)4)goto _LL5A;if(*((int*)_tmp8E)!= 4)goto _LL5A;_tmp8F=((struct Cyc_Absyn_PointerType_struct*)
_tmp8E)->f1;_tmp90=(void*)_tmp8F.elt_typ;_tmp91=_tmp8F.ptr_atts;_tmp92=_tmp91.bounds;
_tmp93=_tmp91.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp94=f;
struct Cyc_Dict_Dict _tmp95;struct Cyc_List_List*_tmp96;_LL5D: if((_tmp94.BottomFL).tag
!= 0)goto _LL5F;_LL5E: return({struct _tuple5 _tmp97;_tmp97.f1=f;_tmp97.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp90,(void*)(env->fenv)->unknown_all);_tmp97;});_LL5F: if((_tmp94.ReachableFL).tag
!= 1)goto _LL5C;_tmp95=(_tmp94.ReachableFL).f1;_tmp96=(_tmp94.ReachableFL).f2;
_LL60: if(Cyc_Tcutil_is_bound_one(_tmp92)){void*_tmp98=r;struct Cyc_CfFlowInfo_Place*
_tmp99;void*_tmp9A;_LL62: if((int)_tmp98 != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmp98 != 2)goto _LL66;_LL65:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp9C;_tmp9C.tag=Cyc_CfFlowInfo_NotZero;_tmp9C.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp96);_tmp9C;});_tmp9B;})));goto _LL61;_LL66: if(_tmp98 <= (void*)3)goto _LL68;if(*((
int*)_tmp98)!= 2)goto _LL68;_tmp99=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp98)->f1;_LL67:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp9E;_tmp9E.tag=Cyc_CfFlowInfo_NotZero;_tmp9E.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp96);_tmp9E;});_tmp9D;})));return({struct _tuple5 _tmp9F;_tmp9F.f1=f;_tmp9F.f2=
Cyc_CfFlowInfo_lookup_place(_tmp95,_tmp99);_tmp9F;});_LL68: if((int)_tmp98 != 0)
goto _LL6A;_LL69:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple5 _tmpA0;_tmpA0.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmpA1;(_tmpA1.BottomFL).tag=0;_tmpA1;});_tmpA0.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp90,(void*)(env->fenv)->unknown_all);_tmpA0;});_LL6A: if(_tmp98 <= (
void*)3)goto _LL6C;if(*((int*)_tmp98)!= 0)goto _LL6C;_tmp9A=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp98)->f1;_LL6B: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp9A);goto _LL6D;
_LL6C:;_LL6D:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpA3;_tmpA3.tag=Cyc_CfFlowInfo_UnknownZ;_tmpA3.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp96);_tmpA3;});_tmpA2;})));_LL61:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpA4=_cycalloc(sizeof(*
_tmpA4));_tmpA4[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmpA5;_tmpA5.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpA5.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp96);_tmpA5;});
_tmpA4;})));}{void*_tmpA6=Cyc_CfFlowInfo_initlevel(_tmp95,r);_LL6F: if((int)
_tmpA6 != 0)goto _LL71;_LL70:({void*_tmpA7[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmpA8="dereference of possibly uninitialized pointer";_tag_dynforward(
_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,46));}),_tag_dynforward(_tmpA7,
sizeof(void*),0));});goto _LL72;_LL71: if((int)_tmpA6 != 2)goto _LL73;_LL72: return({
struct _tuple5 _tmpA9;_tmpA9.f1=f;_tmpA9.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmp90,(void*)(env->fenv)->unknown_all);_tmpA9;});_LL73: if((int)_tmpA6 != 1)goto
_LL6E;_LL74: return({struct _tuple5 _tmpAA;_tmpAA.f1=f;_tmpAA.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp90,(void*)(env->fenv)->unknown_none);_tmpAA;});_LL6E:;}_LL5C:;}
_LL5A:;_LL5B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Core_Impossible_struct _tmpAC;
_tmpAC.tag=Cyc_Core_Impossible;_tmpAC.f1=({const char*_tmpAD="right deref of non-pointer-type";
_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size(_tmpAD,32));});_tmpAC;});
_tmpAB;}));_LL57:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpAE=(void*)e1->r;void*_tmpAF;struct Cyc_Absyn_Vardecl*
_tmpB0;void*_tmpB1;struct Cyc_Absyn_Vardecl*_tmpB2;void*_tmpB3;struct Cyc_Absyn_Vardecl*
_tmpB4;void*_tmpB5;struct Cyc_Absyn_Vardecl*_tmpB6;_LL76: if(*((int*)_tmpAE)!= 1)
goto _LL78;_tmpAF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpAE)->f2;if(_tmpAF <= (
void*)1)goto _LL78;if(*((int*)_tmpAF)!= 4)goto _LL78;_tmpB0=((struct Cyc_Absyn_Pat_b_struct*)
_tmpAF)->f1;_LL77: _tmpB2=_tmpB0;goto _LL79;_LL78: if(*((int*)_tmpAE)!= 1)goto _LL7A;
_tmpB1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpAE)->f2;if(_tmpB1 <= (void*)1)
goto _LL7A;if(*((int*)_tmpB1)!= 3)goto _LL7A;_tmpB2=((struct Cyc_Absyn_Local_b_struct*)
_tmpB1)->f1;_LL79: _tmpB4=_tmpB2;goto _LL7B;_LL7A: if(*((int*)_tmpAE)!= 1)goto _LL7C;
_tmpB3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpAE)->f2;if(_tmpB3 <= (void*)1)
goto _LL7C;if(*((int*)_tmpB3)!= 2)goto _LL7C;_tmpB4=((struct Cyc_Absyn_Param_b_struct*)
_tmpB3)->f1;_LL7B: _tmpB6=_tmpB4;goto _LL7D;_LL7C: if(*((int*)_tmpAE)!= 1)goto _LL7E;
_tmpB5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpAE)->f2;if(_tmpB5 <= (void*)1)
goto _LL7E;if(*((int*)_tmpB5)!= 0)goto _LL7E;_tmpB6=((struct Cyc_Absyn_Global_b_struct*)
_tmpB5)->f1;_LL7D: if(!_tmpB6->escapes){void*_tmpB7=(void*)e2->r;void*_tmpB8;
struct Cyc_Absyn_Vardecl*_tmpB9;void*_tmpBA;struct Cyc_Absyn_Vardecl*_tmpBB;void*
_tmpBC;struct Cyc_Absyn_Vardecl*_tmpBD;void*_tmpBE;struct Cyc_Absyn_Vardecl*_tmpBF;
_LL81: if(*((int*)_tmpB7)!= 1)goto _LL83;_tmpB8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB7)->f2;if(_tmpB8 <= (void*)1)goto _LL83;if(*((int*)_tmpB8)!= 4)goto _LL83;
_tmpB9=((struct Cyc_Absyn_Pat_b_struct*)_tmpB8)->f1;_LL82: _tmpBB=_tmpB9;goto _LL84;
_LL83: if(*((int*)_tmpB7)!= 1)goto _LL85;_tmpBA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB7)->f2;if(_tmpBA <= (void*)1)goto _LL85;if(*((int*)_tmpBA)!= 3)goto _LL85;
_tmpBB=((struct Cyc_Absyn_Local_b_struct*)_tmpBA)->f1;_LL84: _tmpBD=_tmpBB;goto
_LL86;_LL85: if(*((int*)_tmpB7)!= 1)goto _LL87;_tmpBC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB7)->f2;if(_tmpBC <= (void*)1)goto _LL87;if(*((int*)_tmpBC)!= 2)goto _LL87;
_tmpBD=((struct Cyc_Absyn_Param_b_struct*)_tmpBC)->f1;_LL86: _tmpBF=_tmpBD;goto
_LL88;_LL87: if(*((int*)_tmpB7)!= 1)goto _LL89;_tmpBE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB7)->f2;if(_tmpBE <= (void*)1)goto _LL89;if(*((int*)_tmpBE)!= 0)goto _LL89;
_tmpBF=((struct Cyc_Absyn_Global_b_struct*)_tmpBE)->f1;_LL88: if(!_tmpBF->escapes){
int found=0;{struct Cyc_List_List*_tmpC0=relns;for(0;_tmpC0 != 0;_tmpC0=_tmpC0->tl){
struct Cyc_CfFlowInfo_Reln*_tmpC1=(struct Cyc_CfFlowInfo_Reln*)_tmpC0->hd;if(
_tmpC1->vd == _tmpBF){union Cyc_CfFlowInfo_RelnOp_union _tmpC2=_tmpC1->rop;struct
Cyc_Absyn_Vardecl*_tmpC3;_LL8C: if((_tmpC2.LessSize).tag != 2)goto _LL8E;_tmpC3=(
_tmpC2.LessSize).f1;if(!(_tmpC3 == _tmpB6))goto _LL8E;_LL8D: return relns;_LL8E:;
_LL8F: continue;_LL8B:;}}}if(!found)return({struct Cyc_List_List*_tmpC4=
_region_malloc(rgn,sizeof(*_tmpC4));_tmpC4->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpC5=_region_malloc(rgn,sizeof(*_tmpC5));_tmpC5->vd=_tmpBF;_tmpC5->rop=(union
Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmpC6;(_tmpC6.LessSize).tag=
2;(_tmpC6.LessSize).f1=_tmpB6;_tmpC6;});_tmpC5;});_tmpC4->tl=relns;_tmpC4;});}
return relns;_LL89:;_LL8A: return relns;_LL80:;}else{return relns;}_LL7E:;_LL7F:
return relns;_LL75:;}struct _tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmpC7=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo
cinfo;{union Cyc_CfFlowInfo_FlowInfo_union _tmpC8=inflow;struct Cyc_Dict_Dict _tmpC9;
struct Cyc_List_List*_tmpCA;struct Cyc_CfFlowInfo_ConsumeInfo _tmpCB;_LL91: if((
_tmpC8.BottomFL).tag != 0)goto _LL93;_LL92: return({struct _tuple5 _tmpCC;_tmpCC.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpCD;(
_tmpCD.BottomFL).tag=0;_tmpCD;});_tmpCC.f2=(void*)_tmpC7->unknown_all;_tmpCC;});
_LL93: if((_tmpC8.ReachableFL).tag != 1)goto _LL90;_tmpC9=(_tmpC8.ReachableFL).f1;
_tmpCA=(_tmpC8.ReachableFL).f2;_tmpCB=(_tmpC8.ReachableFL).f3;_LL94: d=_tmpC9;
relns=_tmpCA;cinfo=_tmpCB;_LL90:;}{void*_tmpCE=(void*)e->r;struct Cyc_Absyn_Exp*
_tmpCF;void*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct
Cyc_Absyn_Exp*_tmpD3;union Cyc_Absyn_Cnst_union _tmpD4;union Cyc_Absyn_Cnst_union
_tmpD5;int _tmpD6;union Cyc_Absyn_Cnst_union _tmpD7;void*_tmpD8;struct Cyc_List_List*
_tmpD9;void*_tmpDA;void*_tmpDB;struct Cyc_Absyn_Vardecl*_tmpDC;void*_tmpDD;struct
Cyc_Absyn_Vardecl*_tmpDE;void*_tmpDF;struct Cyc_Absyn_Vardecl*_tmpE0;void*_tmpE1;
struct Cyc_List_List*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;
struct Cyc_Core_Opt*_tmpE5;struct Cyc_Core_Opt _tmpE6;struct Cyc_Absyn_Exp*_tmpE7;
struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Core_Opt*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;
struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;
struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_MallocInfo
_tmpF0;int _tmpF1;struct Cyc_Absyn_Exp*_tmpF2;void**_tmpF3;struct Cyc_Absyn_Exp*
_tmpF4;int _tmpF5;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*
_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*
_tmpFB;struct Cyc_Absyn_Exp*_tmpFC;struct _dynforward_ptr*_tmpFD;struct Cyc_Absyn_Exp*
_tmpFE;struct _dynforward_ptr*_tmpFF;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*
_tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*
_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*
_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_Tuniondecl*
_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_List_List*
_tmp10D;struct Cyc_Absyn_Aggrdecl*_tmp10E;struct Cyc_Absyn_Aggrdecl _tmp10F;struct
Cyc_Absyn_AggrdeclImpl*_tmp110;struct Cyc_Absyn_AggrdeclImpl _tmp111;struct Cyc_List_List*
_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_Absyn_Vardecl*_tmp114;struct Cyc_Absyn_Exp*
_tmp115;struct Cyc_Absyn_Exp*_tmp116;int _tmp117;struct Cyc_Absyn_Stmt*_tmp118;void*
_tmp119;_LL96: if(*((int*)_tmpCE)!= 15)goto _LL98;_tmpCF=((struct Cyc_Absyn_Cast_e_struct*)
_tmpCE)->f2;_tmpD0=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpCE)->f4;if((int)
_tmpD0 != 2)goto _LL98;_LL97: {union Cyc_CfFlowInfo_FlowInfo_union _tmp11B;void*
_tmp11C;struct _tuple5 _tmp11A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpCF);
_tmp11B=_tmp11A.f1;_tmp11C=_tmp11A.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp11E;
void*_tmp11F;struct _tuple5 _tmp11D=Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp11B,_tmpCF,_tmp11C);_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;return({struct
_tuple5 _tmp120;_tmp120.f1=_tmp11E;_tmp120.f2=_tmp11C;_tmp120;});}}_LL98: if(*((
int*)_tmpCE)!= 15)goto _LL9A;_tmpD1=((struct Cyc_Absyn_Cast_e_struct*)_tmpCE)->f2;
_LL99: _tmpD2=_tmpD1;goto _LL9B;_LL9A: if(*((int*)_tmpCE)!= 13)goto _LL9C;_tmpD2=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpCE)->f1;_LL9B: _tmpD3=_tmpD2;goto
_LL9D;_LL9C: if(*((int*)_tmpCE)!= 14)goto _LL9E;_tmpD3=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpCE)->f1;_LL9D: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpD3);_LL9E:
if(*((int*)_tmpCE)!= 0)goto _LLA0;_tmpD4=((struct Cyc_Absyn_Const_e_struct*)_tmpCE)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpCE)->f1).Null_c).tag != 6)goto _LLA0;
_LL9F: goto _LLA1;_LLA0: if(*((int*)_tmpCE)!= 0)goto _LLA2;_tmpD5=((struct Cyc_Absyn_Const_e_struct*)
_tmpCE)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpCE)->f1).Int_c).tag != 2)
goto _LLA2;_tmpD6=(_tmpD5.Int_c).f2;if(_tmpD6 != 0)goto _LLA2;_LLA1: return({struct
_tuple5 _tmp121;_tmp121.f1=inflow;_tmp121.f2=(void*)0;_tmp121;});_LLA2: if(*((int*)
_tmpCE)!= 0)goto _LLA4;_tmpD7=((struct Cyc_Absyn_Const_e_struct*)_tmpCE)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpCE)->f1).Int_c).tag != 2)goto _LLA4;_LLA3: goto
_LLA5;_LLA4: if(*((int*)_tmpCE)!= 1)goto _LLA6;_tmpD8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpCE)->f2;if(_tmpD8 <= (void*)1)goto _LLA6;if(*((int*)_tmpD8)!= 1)goto _LLA6;
_LLA5: return({struct _tuple5 _tmp122;_tmp122.f1=inflow;_tmp122.f2=(void*)1;_tmp122;});
_LLA6: if(*((int*)_tmpCE)!= 32)goto _LLA8;_tmpD9=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpCE)->f1;if(_tmpD9 != 0)goto _LLA8;_LLA7: goto _LLA9;_LLA8: if(*((int*)_tmpCE)!= 0)
goto _LLAA;_LLA9: goto _LLAB;_LLAA: if(*((int*)_tmpCE)!= 19)goto _LLAC;_LLAB: goto
_LLAD;_LLAC: if(*((int*)_tmpCE)!= 18)goto _LLAE;_LLAD: goto _LLAF;_LLAE: if(*((int*)
_tmpCE)!= 20)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if(*((int*)_tmpCE)!= 21)goto _LLB2;
_LLB1: goto _LLB3;_LLB2: if(*((int*)_tmpCE)!= 34)goto _LLB4;_LLB3: goto _LLB5;_LLB4:
if(*((int*)_tmpCE)!= 33)goto _LLB6;_LLB5: return({struct _tuple5 _tmp123;_tmp123.f1=
inflow;_tmp123.f2=(void*)_tmpC7->unknown_all;_tmp123;});_LLB6: if(*((int*)_tmpCE)
!= 1)goto _LLB8;_tmpDA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpCE)->f2;if(
_tmpDA <= (void*)1)goto _LLB8;if(*((int*)_tmpDA)!= 0)goto _LLB8;_LLB7: return({
struct _tuple5 _tmp124;_tmp124.f1=inflow;_tmp124.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpC7,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpC7->unknown_all);
_tmp124;});_LLB8: if(*((int*)_tmpCE)!= 1)goto _LLBA;_tmpDB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpCE)->f2;if(_tmpDB <= (void*)1)goto _LLBA;if(*((int*)_tmpDB)!= 2)goto _LLBA;
_tmpDC=((struct Cyc_Absyn_Param_b_struct*)_tmpDB)->f1;_LLB9: _tmpDE=_tmpDC;goto
_LLBB;_LLBA: if(*((int*)_tmpCE)!= 1)goto _LLBC;_tmpDD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpCE)->f2;if(_tmpDD <= (void*)1)goto _LLBC;if(*((int*)_tmpDD)!= 3)goto _LLBC;
_tmpDE=((struct Cyc_Absyn_Local_b_struct*)_tmpDD)->f1;_LLBB: _tmpE0=_tmpDE;goto
_LLBD;_LLBC: if(*((int*)_tmpCE)!= 1)goto _LLBE;_tmpDF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpCE)->f2;if(_tmpDF <= (void*)1)goto _LLBE;if(*((int*)_tmpDF)!= 4)goto _LLBE;
_tmpE0=((struct Cyc_Absyn_Pat_b_struct*)_tmpDF)->f1;_LLBD: if(Cyc_Tcutil_is_unique_pointer((
void*)_tmpE0->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp125=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp12A=_region_malloc(_tmpC7->r,sizeof(*_tmp12A));_tmp12A[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp12B;_tmp12B.tag=0;_tmp12B.f1=_tmpE0;_tmp12B;});_tmp12A;});struct Cyc_CfFlowInfo_Place*
_tmp126=({struct Cyc_CfFlowInfo_Place*_tmp129=_region_malloc(_tmpC7->r,sizeof(*
_tmp129));_tmp129->root=(void*)((void*)_tmp125);_tmp129->fields=0;_tmp129;});
cinfo.may_consume=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,
struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpC7->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp127=_region_malloc(_tmpC7->r,sizeof(*_tmp127));_tmp127->hd=
_tmp126;_tmp127->tl=0;_tmp127;}),cinfo.may_consume);inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp128;(_tmp128.ReachableFL).tag=1;(_tmp128.ReachableFL).f1=
d;(_tmp128.ReachableFL).f2=relns;(_tmp128.ReachableFL).f3=cinfo;_tmp128;});}
return({struct _tuple5 _tmp12C;_tmp12C.f1=inflow;_tmp12C.f2=Cyc_Dict_lookup(d,(
void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp12D=_region_malloc(env->r,
sizeof(*_tmp12D));_tmp12D[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp12E;
_tmp12E.tag=0;_tmp12E.f1=_tmpE0;_tmp12E;});_tmp12D;}));_tmp12C;});_LLBE: if(*((
int*)_tmpCE)!= 3)goto _LLC0;_tmpE1=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpCE)->f1;_tmpE2=((struct Cyc_Absyn_Primop_e_struct*)_tmpCE)->f2;_LLBF: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp130;void*_tmp131;struct _tuple5 _tmp12F=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpE2,0);_tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;{void*_tmp132=_tmpE1;
_LLFD: if((int)_tmp132 != 0)goto _LLFF;_LLFE: goto _LL100;_LLFF: if((int)_tmp132 != 2)
goto _LL101;_LL100: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpE2))->hd)->loc,_tmp130);goto _LLFC;_LL101:;
_LL102: _tmp130=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpE2))->hd)->loc,_tmp130);goto _LLFC;_LLFC:;}
return({struct _tuple5 _tmp133;_tmp133.f1=_tmp130;_tmp133.f2=_tmp131;_tmp133;});}
_LLC0: if(*((int*)_tmpCE)!= 5)goto _LLC2;_tmpE3=((struct Cyc_Absyn_Increment_e_struct*)
_tmpCE)->f1;_LLC1: {struct Cyc_List_List _tmp134=({struct Cyc_List_List _tmp143;
_tmp143.hd=_tmpE3;_tmp143.tl=0;_tmp143;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp136;struct _tuple5 _tmp135=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp134,0);_tmp136=_tmp135.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmpE3->loc,_tmp136);{union Cyc_CfFlowInfo_AbsLVal_union _tmp138;struct _tuple7
_tmp137=Cyc_NewControlFlow_anal_Lexp(env,_tmp136,_tmpE3);_tmp138=_tmp137.f2;{
struct _tuple12 _tmp13A=({struct _tuple12 _tmp139;_tmp139.f1=_tmp138;_tmp139.f2=
_tmp136;_tmp139;});union Cyc_CfFlowInfo_AbsLVal_union _tmp13B;struct Cyc_CfFlowInfo_Place*
_tmp13C;union Cyc_CfFlowInfo_FlowInfo_union _tmp13D;struct Cyc_Dict_Dict _tmp13E;
struct Cyc_List_List*_tmp13F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp140;_LL104:
_tmp13B=_tmp13A.f1;if(((_tmp13A.f1).PlaceL).tag != 0)goto _LL106;_tmp13C=(_tmp13B.PlaceL).f1;
_tmp13D=_tmp13A.f2;if(((_tmp13A.f2).ReachableFL).tag != 1)goto _LL106;_tmp13E=(
_tmp13D.ReachableFL).f1;_tmp13F=(_tmp13D.ReachableFL).f2;_tmp140=(_tmp13D.ReachableFL).f3;
_LL105: _tmp13F=Cyc_CfFlowInfo_reln_kill_exp(_tmpC7->r,_tmp13F,_tmpE3);_tmp136=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp141;(
_tmp141.ReachableFL).tag=1;(_tmp141.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmpC7,_tmpE3->loc,_tmp13E,env->all_changed,_tmp13C,(void*)_tmpC7->unknown_all);(
_tmp141.ReachableFL).f2=_tmp13F;(_tmp141.ReachableFL).f3=_tmp140;_tmp141;});goto
_LL103;_LL106:;_LL107: goto _LL103;_LL103:;}return({struct _tuple5 _tmp142;_tmp142.f1=
_tmp136;_tmp142.f2=(void*)_tmpC7->unknown_all;_tmp142;});}}_LLC2: if(*((int*)
_tmpCE)!= 4)goto _LLC4;_tmpE4=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpCE)->f1;
_tmpE5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpCE)->f2;if(_tmpE5 == 0)goto _LLC4;
_tmpE6=*_tmpE5;_tmpE7=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpCE)->f3;_LLC3: {
struct Cyc_List_List _tmp144=({struct Cyc_List_List _tmp162;_tmp162.hd=_tmpE7;
_tmp162.tl=0;_tmp162;});struct Cyc_List_List _tmp145=({struct Cyc_List_List _tmp161;
_tmp161.hd=_tmpE4;_tmp161.tl=(struct Cyc_List_List*)& _tmp144;_tmp161;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp147;struct _tuple5 _tmp146=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp145,1);_tmp147=_tmp146.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp149;struct _tuple7 _tmp148=Cyc_NewControlFlow_anal_Lexp(env,_tmp147,e);_tmp149=
_tmp148.f2;_tmp147=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp147);{struct
_tuple12 _tmp14B=({struct _tuple12 _tmp14A;_tmp14A.f1=_tmp149;_tmp14A.f2=_tmp147;
_tmp14A;});union Cyc_CfFlowInfo_AbsLVal_union _tmp14C;struct Cyc_CfFlowInfo_Place*
_tmp14D;union Cyc_CfFlowInfo_FlowInfo_union _tmp14E;struct Cyc_Dict_Dict _tmp14F;
struct Cyc_List_List*_tmp150;struct Cyc_CfFlowInfo_ConsumeInfo _tmp151;_LL109:
_tmp14C=_tmp14B.f1;if(((_tmp14B.f1).PlaceL).tag != 0)goto _LL10B;_tmp14D=(_tmp14C.PlaceL).f1;
_tmp14E=_tmp14B.f2;if(((_tmp14B.f2).ReachableFL).tag != 1)goto _LL10B;_tmp14F=(
_tmp14E.ReachableFL).f1;_tmp150=(_tmp14E.ReachableFL).f2;_tmp151=(_tmp14E.ReachableFL).f3;
_LL10A: _tmp150=Cyc_CfFlowInfo_reln_kill_exp(_tmpC7->r,_tmp150,_tmpE4);_tmp14F=
Cyc_CfFlowInfo_assign_place(_tmpC7,_tmpE4->loc,_tmp14F,env->all_changed,_tmp14D,(
void*)_tmpC7->unknown_all);if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpE4->topt))->v)){_tmp151.may_consume=((struct Cyc_List_List*(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpC7->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp152=_region_malloc(_tmpC7->r,sizeof(*_tmp152));_tmp152->hd=
_tmp14D;_tmp152->tl=0;_tmp152;}),_tmp151.may_consume);if(!((int(*)(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp151.consumed,_tmp14D)){
struct Cyc_CfFlowInfo_Place _tmp154;void*_tmp155;struct Cyc_CfFlowInfo_Place*
_tmp153=_tmp14D;_tmp154=*_tmp153;_tmp155=(void*)_tmp154.root;{void*_tmp156=
_tmp155;struct Cyc_Absyn_Vardecl*_tmp157;_LL10E: if(*((int*)_tmp156)!= 0)goto
_LL110;_tmp157=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp156)->f1;_LL10F: {
struct _dynforward_ptr _tmp158=Cyc_Absynpp_qvar2string(_tmp157->name);({struct Cyc_String_pa_struct
_tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp158);{void*_tmp159[1]={& _tmp15B};Cyc_Tcutil_warn(_tmpE4->loc,({const char*
_tmp15A="may clobber unique pointer %s";_tag_dynforward(_tmp15A,sizeof(char),
_get_zero_arr_size(_tmp15A,30));}),_tag_dynforward(_tmp159,sizeof(void*),1));}});
goto _LL10D;}_LL110:;_LL111:({void*_tmp15C[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp15D="warning locations not for VarRoots";
_tag_dynforward(_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,35));}),
_tag_dynforward(_tmp15C,sizeof(void*),0));});_LL10D:;}}if(env->all_changed == 0){
struct Cyc_Dict_Dict _tmp15E=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct
Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)(_tmpC7->r,_tmp151.consumed,
_tmp14D);if((_tmp151.consumed).t != _tmp15E.t);_tmp151.consumed=_tmp15E;}}_tmp147=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp15F;(
_tmp15F.ReachableFL).tag=1;(_tmp15F.ReachableFL).f1=_tmp14F;(_tmp15F.ReachableFL).f2=
_tmp150;(_tmp15F.ReachableFL).f3=_tmp151;_tmp15F;});goto _LL108;_LL10B:;_LL10C:
goto _LL108;_LL108:;}return({struct _tuple5 _tmp160;_tmp160.f1=_tmp147;_tmp160.f2=(
void*)_tmpC7->unknown_all;_tmp160;});}}_LLC4: if(*((int*)_tmpCE)!= 4)goto _LLC6;
_tmpE8=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpCE)->f1;_tmpE9=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpCE)->f2;if(_tmpE9 != 0)goto _LLC6;_tmpEA=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpCE)->f3;_LLC5: {struct Cyc_Dict_Dict*_tmp163=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp165;union Cyc_CfFlowInfo_FlowInfo_union _tmp166;struct _tuple6 _tmp164=Cyc_CfFlowInfo_save_consume_info(
_tmpC7,inflow,1);_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp18F;_tmp18F.consumed=_tmpC7->mt_place_set;
_tmp18F.may_consume=0;_tmp18F;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp167=_region_malloc(env->r,sizeof(*_tmp167));_tmp167[0]=_tmpC7->mt_place_set;
_tmp167;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp169;union Cyc_CfFlowInfo_AbsLVal_union
_tmp16A;struct _tuple7 _tmp168=Cyc_NewControlFlow_anal_Lexp(env,_tmp166,_tmpE8);
_tmp169=_tmp168.f1;_tmp16A=_tmp168.f2;{struct Cyc_Dict_Dict _tmp16B=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp16C=
_region_malloc(env->r,sizeof(*_tmp16C));_tmp16C[0]=_tmpC7->mt_place_set;_tmp16C;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp16E;void*_tmp16F;struct _tuple5 _tmp16D=Cyc_NewControlFlow_anal_Rexp(
env,_tmp166,_tmpEA);_tmp16E=_tmp16D.f1;_tmp16F=_tmp16D.f2;{struct Cyc_Dict_Dict
_tmp170=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp171=Cyc_CfFlowInfo_after_flow(_tmpC7,(struct Cyc_Dict_Dict*)& _tmp16B,_tmp169,
_tmp16E,_tmp16B,_tmp170);union Cyc_CfFlowInfo_FlowInfo_union _tmp172=Cyc_CfFlowInfo_join_flow(
_tmpC7,_tmp163,_tmp166,_tmp171,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp174;struct
_tuple6 _tmp173=Cyc_CfFlowInfo_save_consume_info(_tmpC7,_tmp172,0);_tmp174=
_tmp173.f1;if(init_consume)_tmp166=Cyc_CfFlowInfo_restore_consume_info(_tmp166,
outflow_consume);outflow_consume=_tmp174;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp172,_tmp166)){if(_tmp163 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp163,Cyc_CfFlowInfo_union_place_set(
_tmp16B,_tmp170,0),0);}_tmp171=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp171);{union Cyc_CfFlowInfo_FlowInfo_union _tmp175=_tmp171;struct Cyc_Dict_Dict
_tmp176;struct Cyc_List_List*_tmp177;struct Cyc_CfFlowInfo_ConsumeInfo _tmp178;
_LL113: if((_tmp175.BottomFL).tag != 0)goto _LL115;_LL114: return({struct _tuple5
_tmp179;_tmp179.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp17A;(_tmp17A.BottomFL).tag=0;_tmp17A;});_tmp179.f2=_tmp16F;_tmp179;});_LL115:
if((_tmp175.ReachableFL).tag != 1)goto _LL112;_tmp176=(_tmp175.ReachableFL).f1;
_tmp177=(_tmp175.ReachableFL).f2;_tmp178=(_tmp175.ReachableFL).f3;_LL116: _tmp178=
Cyc_CfFlowInfo_and_consume(_tmpC7->r,_tmp165,_tmp178);{union Cyc_CfFlowInfo_AbsLVal_union
_tmp17B=_tmp16A;struct Cyc_CfFlowInfo_Place*_tmp17C;_LL118: if((_tmp17B.PlaceL).tag
!= 0)goto _LL11A;_tmp17C=(_tmp17B.PlaceL).f1;_LL119: _tmp176=Cyc_CfFlowInfo_assign_place(
_tmpC7,e->loc,_tmp176,env->all_changed,_tmp17C,_tmp16F);if(Cyc_Tcutil_is_unique_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE8->topt))->v)){_tmp178.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpC7->r,Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*_tmp17D=_region_malloc(
_tmpC7->r,sizeof(*_tmp17D));_tmp17D->hd=_tmp17C;_tmp17D->tl=0;_tmp17D;}),_tmp178.may_consume);
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
_tmp178.consumed,_tmp17C)){struct Cyc_CfFlowInfo_Place _tmp17F;void*_tmp180;struct
Cyc_CfFlowInfo_Place*_tmp17E=_tmp17C;_tmp17F=*_tmp17E;_tmp180=(void*)_tmp17F.root;{
void*_tmp181=_tmp180;struct Cyc_Absyn_Vardecl*_tmp182;_LL11D: if(*((int*)_tmp181)
!= 0)goto _LL11F;_tmp182=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp181)->f1;
_LL11E: {struct _dynforward_ptr _tmp183=Cyc_Absynpp_qvar2string(_tmp182->name);({
struct Cyc_String_pa_struct _tmp186;_tmp186.tag=0;_tmp186.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp183);{void*_tmp184[1]={& _tmp186};Cyc_Tcutil_warn(
_tmpE8->loc,({const char*_tmp185="may clobber unique pointer %s";_tag_dynforward(
_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,30));}),_tag_dynforward(_tmp184,
sizeof(void*),1));}});goto _LL11C;}_LL11F:;_LL120:({void*_tmp187[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp188="warning locations not for VarRoots";_tag_dynforward(_tmp188,sizeof(char),
_get_zero_arr_size(_tmp188,35));}),_tag_dynforward(_tmp187,sizeof(void*),0));});
_LL11C:;}}if(env->all_changed == 0){struct Cyc_Dict_Dict _tmp189=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)(
_tmpC7->r,_tmp178.consumed,_tmp17C);if((_tmp178.consumed).t != _tmp189.t);_tmp178.consumed=
_tmp189;}}_tmp177=Cyc_CfFlowInfo_reln_assign_exp(_tmpC7->r,_tmp177,_tmpE8,_tmpEA);
_tmp171=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp18A;(_tmp18A.ReachableFL).tag=1;(_tmp18A.ReachableFL).f1=_tmp176;(_tmp18A.ReachableFL).f2=
_tmp177;(_tmp18A.ReachableFL).f3=_tmp178;_tmp18A;});Cyc_NewControlFlow_update_tryflow(
env,_tmp171);return({struct _tuple5 _tmp18B;_tmp18B.f1=_tmp171;_tmp18B.f2=_tmp16F;
_tmp18B;});_LL11A: if((_tmp17B.UnknownL).tag != 1)goto _LL117;_LL11B: if(Cyc_Tcutil_is_unique_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE8->topt))->v))({void*_tmp18C[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp18D="dest of unique assignment is unknown!";_tag_dynforward(
_tmp18D,sizeof(char),_get_zero_arr_size(_tmp18D,38));}),_tag_dynforward(_tmp18C,
sizeof(void*),0));});return({struct _tuple5 _tmp18E;_tmp18E.f1=Cyc_NewControlFlow_use_Rval(
env,_tmpEA->loc,_tmp171,_tmp16F);_tmp18E.f2=_tmp16F;_tmp18E;});_LL117:;}_LL112:;}}
_tmp166=Cyc_CfFlowInfo_restore_consume_info(_tmp172,empty_consume);}}}}}}}_LLC6:
if(*((int*)_tmpCE)!= 9)goto _LLC8;_tmpEB=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpCE)->f1;_tmpEC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpCE)->f2;_LLC7: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp191;void*_tmp192;struct _tuple5 _tmp190=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpEB);_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_tmp191=Cyc_CfFlowInfo_drop_unique_rvals(
_tmpEB->loc,_tmp191);return Cyc_NewControlFlow_anal_Rexp(env,_tmp191,_tmpEC);}
_LLC8: if(*((int*)_tmpCE)!= 12)goto _LLCA;_tmpED=((struct Cyc_Absyn_Throw_e_struct*)
_tmpCE)->f1;_LLC9: {union Cyc_CfFlowInfo_FlowInfo_union _tmp194;void*_tmp195;
struct _tuple5 _tmp193=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpED);_tmp194=
_tmp193.f1;_tmp195=_tmp193.f2;_tmp194=Cyc_CfFlowInfo_consume_unique_rvals(_tmpED->loc,
_tmp194);Cyc_NewControlFlow_use_Rval(env,_tmpED->loc,_tmp194,_tmp195);return({
struct _tuple5 _tmp196;_tmp196.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp197;(_tmp197.BottomFL).tag=0;_tmp197;});_tmp196.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpC7,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpC7->unknown_all);
_tmp196;});}_LLCA: if(*((int*)_tmpCE)!= 11)goto _LLCC;_tmpEE=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpCE)->f1;_tmpEF=((struct Cyc_Absyn_FnCall_e_struct*)_tmpCE)->f2;_LLCB: {struct
_RegionHandle _tmp198=_new_region("temp");struct _RegionHandle*temp=& _tmp198;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp19B;struct Cyc_List_List*
_tmp19C;struct _tuple11 _tmp19A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp199=_region_malloc(temp,sizeof(*_tmp199));
_tmp199->hd=_tmpEE;_tmp199->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmpEF);_tmp199;}),1);_tmp19B=
_tmp19A.f1;_tmp19C=_tmp19A.f2;_tmp19B=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp19B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp19D=Cyc_NewControlFlow_use_Rval(
env,_tmpEE->loc,_tmp19B,(void*)((struct Cyc_List_List*)_check_null(_tmp19C))->hd);
_tmp19C=_tmp19C->tl;{struct Cyc_List_List*init_params=0;{void*_tmp19E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEE->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp19F;void*_tmp1A0;_LL122: if(_tmp19E <= (void*)4)goto _LL124;if(*((int*)_tmp19E)
!= 4)goto _LL124;_tmp19F=((struct Cyc_Absyn_PointerType_struct*)_tmp19E)->f1;
_tmp1A0=(void*)_tmp19F.elt_typ;_LL123:{void*_tmp1A1=Cyc_Tcutil_compress(_tmp1A0);
struct Cyc_Absyn_FnInfo _tmp1A2;struct Cyc_List_List*_tmp1A3;_LL127: if(_tmp1A1 <= (
void*)4)goto _LL129;if(*((int*)_tmp1A1)!= 8)goto _LL129;_tmp1A2=((struct Cyc_Absyn_FnType_struct*)
_tmp1A1)->f1;_tmp1A3=_tmp1A2.attributes;_LL128: for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){
void*_tmp1A4=(void*)_tmp1A3->hd;int _tmp1A5;_LL12C: if(_tmp1A4 <= (void*)17)goto
_LL12E;if(*((int*)_tmp1A4)!= 4)goto _LL12E;_tmp1A5=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A4)->f1;_LL12D: init_params=({struct Cyc_List_List*_tmp1A6=_region_malloc(
temp,sizeof(*_tmp1A6));_tmp1A6->hd=(void*)_tmp1A5;_tmp1A6->tl=init_params;
_tmp1A6;});goto _LL12B;_LL12E:;_LL12F: goto _LL12B;_LL12B:;}goto _LL126;_LL129:;
_LL12A:({void*_tmp1A7[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A8="anal_Rexp: bad function type";
_tag_dynforward(_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,29));}),
_tag_dynforward(_tmp1A7,sizeof(void*),0));});_LL126:;}goto _LL121;_LL124:;_LL125:({
void*_tmp1A9[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1AA="anal_Rexp: bad function type";
_tag_dynforward(_tmp1AA,sizeof(char),_get_zero_arr_size(_tmp1AA,29));}),
_tag_dynforward(_tmp1A9,sizeof(void*),0));});_LL121:;}{int i=1;for(0;_tmp19C != 0;(((
_tmp19C=_tmp19C->tl,_tmpEF=((struct Cyc_List_List*)_check_null(_tmpEF))->tl)),++ i)){
if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){_tmp19D=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpEF))->hd)->loc,_tmp19D,(void*)_tmp19C->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1AB=_tmp19B;struct Cyc_Dict_Dict _tmp1AC;_LL131: if((_tmp1AB.BottomFL).tag != 0)
goto _LL133;_LL132: goto _LL130;_LL133: if((_tmp1AB.ReachableFL).tag != 1)goto _LL130;
_tmp1AC=(_tmp1AB.ReachableFL).f1;_LL134: if(Cyc_CfFlowInfo_initlevel(_tmp1AC,(
void*)_tmp19C->hd)== (void*)0)({void*_tmp1AD[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpEF))->hd)->loc,({const char*_tmp1AE="expression may not be initialized";
_tag_dynforward(_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,34));}),
_tag_dynforward(_tmp1AD,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1AF=_tmp19D;struct Cyc_Dict_Dict _tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1B2;_LL136: if((_tmp1AF.BottomFL).tag != 0)goto _LL138;_LL137: goto _LL135;_LL138:
if((_tmp1AF.ReachableFL).tag != 1)goto _LL135;_tmp1B0=(_tmp1AF.ReachableFL).f1;
_tmp1B1=(_tmp1AF.ReachableFL).f2;_tmp1B2=(_tmp1AF.ReachableFL).f3;_LL139: {
struct Cyc_Dict_Dict _tmp1B3=Cyc_CfFlowInfo_escape_deref(_tmpC7,_tmp1B0,env->all_changed,(
void*)_tmp19C->hd);{void*_tmp1B4=(void*)_tmp19C->hd;struct Cyc_CfFlowInfo_Place*
_tmp1B5;_LL13B: if(_tmp1B4 <= (void*)3)goto _LL13D;if(*((int*)_tmp1B4)!= 2)goto
_LL13D;_tmp1B5=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1B4)->f1;_LL13C:{
void*_tmp1B6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpEF))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1B7;void*_tmp1B8;_LL140: if(_tmp1B6 <= (void*)4)goto
_LL142;if(*((int*)_tmp1B6)!= 4)goto _LL142;_tmp1B7=((struct Cyc_Absyn_PointerType_struct*)
_tmp1B6)->f1;_tmp1B8=(void*)_tmp1B7.elt_typ;_LL141: _tmp1B3=Cyc_CfFlowInfo_assign_place(
_tmpC7,((struct Cyc_Absyn_Exp*)_tmpEF->hd)->loc,_tmp1B3,env->all_changed,_tmp1B5,
Cyc_CfFlowInfo_typ_to_absrval(_tmpC7,_tmp1B8,(void*)_tmpC7->esc_all));goto _LL13F;
_LL142:;_LL143:({void*_tmp1B9[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1BA="anal_Rexp:bad type for initialized arg";
_tag_dynforward(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,39));}),
_tag_dynforward(_tmp1B9,sizeof(void*),0));});_LL13F:;}goto _LL13A;_LL13D:;_LL13E:
goto _LL13A;_LL13A:;}_tmp19D=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BB;(_tmp1BB.ReachableFL).tag=1;(_tmp1BB.ReachableFL).f1=_tmp1B3;(_tmp1BB.ReachableFL).f2=
_tmp1B1;(_tmp1BB.ReachableFL).f3=_tmp1B2;_tmp1BB;});goto _LL135;}_LL135:;}goto
_LL130;_LL130:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE->topt))->v)){struct _tuple5 _tmp1BE=({struct _tuple5 _tmp1BC;
_tmp1BC.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BD;(_tmp1BD.BottomFL).tag=0;_tmp1BD;});_tmp1BC.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpC7,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpC7->unknown_all);
_tmp1BC;});_npop_handler(0);return _tmp1BE;}else{struct _tuple5 _tmp1C0=({struct
_tuple5 _tmp1BF;_tmp1BF.f1=_tmp19D;_tmp1BF.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpC7,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpC7->unknown_all);
_tmp1BF;});_npop_handler(0);return _tmp1C0;}}}};_pop_region(temp);}_LLCC: if(*((
int*)_tmpCE)!= 35)goto _LLCE;_tmpF0=((struct Cyc_Absyn_Malloc_e_struct*)_tmpCE)->f1;
_tmpF1=_tmpF0.is_calloc;_tmpF2=_tmpF0.rgn;_tmpF3=_tmpF0.elt_type;_tmpF4=_tmpF0.num_elts;
_tmpF5=_tmpF0.fat_result;_LLCD: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1D2=_region_malloc(_tmpC7->r,sizeof(*_tmp1D2));_tmp1D2[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1D3;_tmp1D3.tag=1;_tmp1D3.f1=_tmpF4;_tmp1D3.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1D3;});_tmp1D2;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1D1=_region_malloc(_tmpC7->r,sizeof(*_tmp1D1));
_tmp1D1->root=(void*)root;_tmp1D1->fields=0;_tmp1D1;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp1CF=_region_malloc(_tmpC7->r,sizeof(*_tmp1CF));
_tmp1CF[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1D0;_tmp1D0.tag=2;_tmp1D0.f1=
place;_tmp1D0;});_tmp1CF;});void*place_val=_tmpF5?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(
_tmpC7,*((void**)_check_null(_tmpF3)),(void*)_tmpC7->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmpF2 != 0){
struct _RegionHandle _tmp1C1=_new_region("temp");struct _RegionHandle*temp=& _tmp1C1;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C4;struct Cyc_List_List*
_tmp1C5;struct _tuple11 _tmp1C3=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1C2[2];_tmp1C2[1]=_tmpF4;_tmp1C2[0]=(struct Cyc_Absyn_Exp*)
_tmpF2;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
temp,_tag_dynforward(_tmp1C2,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1C4=
_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;outflow=_tmp1C4;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C7;struct _tuple5 _tmp1C6=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpF4);
_tmp1C7=_tmp1C6.f1;outflow=_tmp1C7;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF4->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C8=outflow;struct
Cyc_Dict_Dict _tmp1C9;struct Cyc_List_List*_tmp1CA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1CB;_LL145: if((_tmp1C8.BottomFL).tag != 0)goto _LL147;_LL146: return({struct
_tuple5 _tmp1CC;_tmp1CC.f1=outflow;_tmp1CC.f2=rval;_tmp1CC;});_LL147: if((_tmp1C8.ReachableFL).tag
!= 1)goto _LL144;_tmp1C9=(_tmp1C8.ReachableFL).f1;_tmp1CA=(_tmp1C8.ReachableFL).f2;
_tmp1CB=(_tmp1C8.ReachableFL).f3;_LL148: return({struct _tuple5 _tmp1CD;_tmp1CD.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1CE;(
_tmp1CE.ReachableFL).tag=1;(_tmp1CE.ReachableFL).f1=Cyc_Dict_insert(_tmp1C9,root,
place_val);(_tmp1CE.ReachableFL).f2=_tmp1CA;(_tmp1CE.ReachableFL).f3=_tmp1CB;
_tmp1CE;});_tmp1CD.f2=rval;_tmp1CD;});_LL144:;}}_LLCE: if(*((int*)_tmpCE)!= 36)
goto _LLD0;_tmpF6=((struct Cyc_Absyn_Swap_e_struct*)_tmpCE)->f1;_tmpF7=((struct Cyc_Absyn_Swap_e_struct*)
_tmpCE)->f2;_LLCF: {void*left_rval;void*right_rval;union Cyc_CfFlowInfo_FlowInfo_union
outflow;{struct _RegionHandle _tmp1D4=_new_region("temp");struct _RegionHandle*temp=&
_tmp1D4;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D7;struct Cyc_List_List*
_tmp1D8;struct _tuple11 _tmp1D6=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1D5[2];_tmp1D5[1]=_tmpF7;_tmp1D5[0]=_tmpF6;((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
temp,_tag_dynforward(_tmp1D5,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1D7=
_tmp1D6.f1;_tmp1D8=_tmp1D6.f2;left_rval=(void*)((struct Cyc_List_List*)
_check_null(_tmp1D8))->hd;right_rval=(void*)((struct Cyc_List_List*)_check_null(
_tmp1D8->tl))->hd;outflow=_tmp1D7;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp1DA;struct _tuple7 _tmp1D9=
Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmpF6);_tmp1DA=_tmp1D9.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1DC;struct _tuple7 _tmp1DB=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmpF7);
_tmp1DC=_tmp1DB.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp1DD=outflow;struct Cyc_Dict_Dict
_tmp1DE;struct Cyc_List_List*_tmp1DF;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1E0;
_LL14A: if((_tmp1DD.ReachableFL).tag != 1)goto _LL14C;_tmp1DE=(_tmp1DD.ReachableFL).f1;
_tmp1DF=(_tmp1DD.ReachableFL).f2;_tmp1E0=(_tmp1DD.ReachableFL).f3;_LL14B:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp1E1=_tmp1DA;struct Cyc_CfFlowInfo_Place*_tmp1E2;
_LL14F: if((_tmp1E1.PlaceL).tag != 0)goto _LL151;_tmp1E2=(_tmp1E1.PlaceL).f1;_LL150:
_tmp1DE=Cyc_CfFlowInfo_assign_place(_tmpC7,_tmpF6->loc,_tmp1DE,env->all_changed,
_tmp1E2,right_rval);goto _LL14E;_LL151: if((_tmp1E1.UnknownL).tag != 1)goto _LL14E;
_LL152: goto _LL14E;_LL14E:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp1E3=_tmp1DC;
struct Cyc_CfFlowInfo_Place*_tmp1E4;_LL154: if((_tmp1E3.PlaceL).tag != 0)goto _LL156;
_tmp1E4=(_tmp1E3.PlaceL).f1;_LL155: _tmp1DE=Cyc_CfFlowInfo_assign_place(_tmpC7,
_tmpF7->loc,_tmp1DE,env->all_changed,_tmp1E4,left_rval);goto _LL153;_LL156: if((
_tmp1E3.UnknownL).tag != 1)goto _LL153;_LL157: goto _LL153;_LL153:;}_tmp1DF=Cyc_CfFlowInfo_reln_kill_exp(
_tmpC7->r,_tmp1DF,_tmpF6);_tmp1DF=Cyc_CfFlowInfo_reln_kill_exp(_tmpC7->r,_tmp1DF,
_tmpF7);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E5;(_tmp1E5.ReachableFL).tag=1;(_tmp1E5.ReachableFL).f1=_tmp1DE;(_tmp1E5.ReachableFL).f2=
_tmp1DF;(_tmp1E5.ReachableFL).f3=_tmp1E0;_tmp1E5;});goto _LL149;_LL14C:;_LL14D:
goto _LL149;_LL149:;}return({struct _tuple5 _tmp1E6;_tmp1E6.f1=outflow;_tmp1E6.f2=(
void*)_tmpC7->unknown_all;_tmp1E6;});}}}_LLD0: if(*((int*)_tmpCE)!= 17)goto _LLD2;
_tmpF8=((struct Cyc_Absyn_New_e_struct*)_tmpCE)->f1;_tmpF9=((struct Cyc_Absyn_New_e_struct*)
_tmpCE)->f2;_LLD1: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1F9=_region_malloc(_tmpC7->r,sizeof(*_tmp1F9));_tmp1F9[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1FA;_tmp1FA.tag=1;_tmp1FA.f1=_tmpF9;_tmp1FA.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1FA;});_tmp1F9;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1F8=_region_malloc(_tmpC7->r,sizeof(*_tmp1F8));
_tmp1F8->root=(void*)root;_tmp1F8->fields=0;_tmp1F8;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp1F6=_region_malloc(_tmpC7->r,sizeof(*_tmp1F6));
_tmp1F6[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1F7;_tmp1F7.tag=2;_tmp1F7.f1=
place;_tmp1F7;});_tmp1F6;});Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmpF8 != 0){
struct _RegionHandle _tmp1E7=_new_region("temp");struct _RegionHandle*temp=& _tmp1E7;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1EA;struct Cyc_List_List*
_tmp1EB;struct _tuple11 _tmp1E9=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1E8[2];_tmp1E8[1]=_tmpF9;_tmp1E8[0]=(struct Cyc_Absyn_Exp*)
_tmpF8;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
temp,_tag_dynforward(_tmp1E8,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1EA=
_tmp1E9.f1;_tmp1EB=_tmp1E9.f2;outflow=_tmp1EA;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp1EB))->tl))->hd;};_pop_region(
temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp1ED;void*_tmp1EE;struct _tuple5
_tmp1EC=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpF9);_tmp1ED=_tmp1EC.f1;
_tmp1EE=_tmp1EC.f2;outflow=_tmp1ED;place_val=_tmp1EE;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF9->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1EF=outflow;struct
Cyc_Dict_Dict _tmp1F0;struct Cyc_List_List*_tmp1F1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1F2;_LL159: if((_tmp1EF.BottomFL).tag != 0)goto _LL15B;_LL15A: return({struct
_tuple5 _tmp1F3;_tmp1F3.f1=outflow;_tmp1F3.f2=rval;_tmp1F3;});_LL15B: if((_tmp1EF.ReachableFL).tag
!= 1)goto _LL158;_tmp1F0=(_tmp1EF.ReachableFL).f1;_tmp1F1=(_tmp1EF.ReachableFL).f2;
_tmp1F2=(_tmp1EF.ReachableFL).f3;_LL15C: return({struct _tuple5 _tmp1F4;_tmp1F4.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1F5;(
_tmp1F5.ReachableFL).tag=1;(_tmp1F5.ReachableFL).f1=Cyc_Dict_insert(_tmp1F0,root,
place_val);(_tmp1F5.ReachableFL).f2=_tmp1F1;(_tmp1F5.ReachableFL).f3=_tmp1F2;
_tmp1F5;});_tmp1F4.f2=rval;_tmp1F4;});_LL158:;}}}_LLD2: if(*((int*)_tmpCE)!= 16)
goto _LLD4;_tmpFA=((struct Cyc_Absyn_Address_e_struct*)_tmpCE)->f1;_LLD3: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp1FC;union Cyc_CfFlowInfo_AbsLVal_union _tmp1FD;
struct _tuple7 _tmp1FB=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmpFA);_tmp1FC=
_tmp1FB.f1;_tmp1FD=_tmp1FB.f2;{union Cyc_CfFlowInfo_AbsLVal_union _tmp1FE=_tmp1FD;
struct Cyc_CfFlowInfo_Place*_tmp1FF;_LL15E: if((_tmp1FE.UnknownL).tag != 1)goto
_LL160;_LL15F: return({struct _tuple5 _tmp200;_tmp200.f1=_tmp1FC;_tmp200.f2=(void*)
1;_tmp200;});_LL160: if((_tmp1FE.PlaceL).tag != 0)goto _LL15D;_tmp1FF=(_tmp1FE.PlaceL).f1;
_LL161: return({struct _tuple5 _tmp201;_tmp201.f1=_tmp1FC;_tmp201.f2=(void*)({
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp202=_region_malloc(env->r,sizeof(*
_tmp202));_tmp202[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp203;_tmp203.tag=
2;_tmp203.f1=_tmp1FF;_tmp203;});_tmp202;});_tmp201;});_LL15D:;}}_LLD4: if(*((int*)
_tmpCE)!= 22)goto _LLD6;_tmpFB=((struct Cyc_Absyn_Deref_e_struct*)_tmpCE)->f1;
_LLD5: {union Cyc_CfFlowInfo_FlowInfo_union _tmp205;void*_tmp206;struct _tuple5
_tmp204=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpFB);_tmp205=_tmp204.f1;
_tmp206=_tmp204.f2;_tmp205=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp205);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp205,_tmpFB,_tmp206);}
_LLD6: if(*((int*)_tmpCE)!= 23)goto _LLD8;_tmpFC=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpCE)->f1;_tmpFD=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpCE)->f2;_LLD7: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp208;void*_tmp209;struct _tuple5 _tmp207=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpFC);_tmp208=_tmp207.f1;_tmp209=_tmp207.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpFC->topt))->v))return({struct _tuple5
_tmp20A;_tmp20A.f1=_tmp208;_tmp20A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmpC7,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpC7->unknown_all);_tmp20A;});{
void*_tmp20B=_tmp209;struct _dynforward_ptr _tmp20C;_LL163: if(_tmp20B <= (void*)3)
goto _LL165;if(*((int*)_tmp20B)!= 4)goto _LL165;_tmp20C=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp20B)->f1;_LL164: {int _tmp20D=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpFC->topt))->v,_tmpFD);return({struct _tuple5
_tmp20E;_tmp20E.f1=_tmp208;_tmp20E.f2=*((void**)_check_dynforward_subscript(
_tmp20C,sizeof(void*),_tmp20D));_tmp20E;});}_LL165:;_LL166:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F[0]=({
struct Cyc_Core_Impossible_struct _tmp210;_tmp210.tag=Cyc_Core_Impossible;_tmp210.f1=(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp213;_tmp213.tag=0;_tmp213.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp211[1]={& _tmp213};Cyc_aprintf(({const char*_tmp212="anal_Rexp: AggrMember: %s";
_tag_dynforward(_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,26));}),
_tag_dynforward(_tmp211,sizeof(void*),1));}});_tmp210;});_tmp20F;}));_LL162:;}}
_LLD8: if(*((int*)_tmpCE)!= 24)goto _LLDA;_tmpFE=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpCE)->f1;_tmpFF=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpCE)->f2;_LLD9: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp215;void*_tmp216;struct _tuple5 _tmp214=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpFE);_tmp215=_tmp214.f1;_tmp216=_tmp214.f2;_tmp215=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp215);{union Cyc_CfFlowInfo_FlowInfo_union _tmp218;void*_tmp219;struct
_tuple5 _tmp217=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp215,_tmpFE,_tmp216);
_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;{void*_tmp21A=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpFE->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp21B;void*_tmp21C;_LL168: if(_tmp21A <= (void*)4)goto _LL16A;if(*((int*)_tmp21A)
!= 4)goto _LL16A;_tmp21B=((struct Cyc_Absyn_PointerType_struct*)_tmp21A)->f1;
_tmp21C=(void*)_tmp21B.elt_typ;_LL169: if(Cyc_Absyn_is_union_type(_tmp21C))return({
struct _tuple5 _tmp21D;_tmp21D.f1=_tmp218;_tmp21D.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpC7,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpC7->unknown_all);
_tmp21D;});{void*_tmp21E=_tmp219;struct _dynforward_ptr _tmp21F;_LL16D: if(_tmp21E
<= (void*)3)goto _LL16F;if(*((int*)_tmp21E)!= 4)goto _LL16F;_tmp21F=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp21E)->f1;_LL16E: {int _tmp220=Cyc_NewControlFlow_get_field_index(_tmp21C,
_tmpFF);return({struct _tuple5 _tmp221;_tmp221.f1=_tmp218;_tmp221.f2=*((void**)
_check_dynforward_subscript(_tmp21F,sizeof(void*),_tmp220));_tmp221;});}_LL16F:;
_LL170:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp222=_cycalloc(
sizeof(*_tmp222));_tmp222[0]=({struct Cyc_Core_Impossible_struct _tmp223;_tmp223.tag=
Cyc_Core_Impossible;_tmp223.f1=({const char*_tmp224="anal_Rexp: AggrArrow";
_tag_dynforward(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,21));});_tmp223;});
_tmp222;}));_LL16C:;}_LL16A:;_LL16B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225[0]=({struct Cyc_Core_Impossible_struct
_tmp226;_tmp226.tag=Cyc_Core_Impossible;_tmp226.f1=({const char*_tmp227="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,25));});_tmp226;});
_tmp225;}));_LL167:;}}}_LLDA: if(*((int*)_tmpCE)!= 6)goto _LLDC;_tmp100=((struct
Cyc_Absyn_Conditional_e_struct*)_tmpCE)->f1;_tmp101=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpCE)->f2;_tmp102=((struct Cyc_Absyn_Conditional_e_struct*)_tmpCE)->f3;_LLDB: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp229;union Cyc_CfFlowInfo_FlowInfo_union
_tmp22A;struct _tuple8 _tmp228=Cyc_NewControlFlow_anal_test(env,inflow,_tmp100);
_tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;_tmp229=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp100->loc,_tmp229);_tmp22A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp100->loc,
_tmp22A);{struct _tuple5 _tmp22B=Cyc_NewControlFlow_anal_Rexp(env,_tmp229,_tmp101);
struct _tuple5 _tmp22C=Cyc_NewControlFlow_anal_Rexp(env,_tmp22A,_tmp102);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpC7,env->all_changed,_tmp22B,_tmp22C,1);}}_LLDC: if(*((int*)_tmpCE)!= 7)goto
_LLDE;_tmp103=((struct Cyc_Absyn_And_e_struct*)_tmpCE)->f1;_tmp104=((struct Cyc_Absyn_And_e_struct*)
_tmpCE)->f2;_LLDD: {union Cyc_CfFlowInfo_FlowInfo_union _tmp22E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp22F;struct _tuple8 _tmp22D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp103);
_tmp22E=_tmp22D.f1;_tmp22F=_tmp22D.f2;_tmp22E=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp103->loc,_tmp22E);_tmp22F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp103->loc,
_tmp22F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp231;void*_tmp232;struct _tuple5
_tmp230=Cyc_NewControlFlow_anal_Rexp(env,_tmp22E,_tmp104);_tmp231=_tmp230.f1;
_tmp232=_tmp230.f2;_tmp231=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp104->loc,
_tmp231);{struct _tuple5 _tmp233=({struct _tuple5 _tmp236;_tmp236.f1=_tmp231;_tmp236.f2=
_tmp232;_tmp236;});struct _tuple5 _tmp234=({struct _tuple5 _tmp235;_tmp235.f1=
_tmp22F;_tmp235.f2=(void*)((void*)0);_tmp235;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpC7,env->all_changed,_tmp233,_tmp234,0);}}}_LLDE: if(*((int*)_tmpCE)!= 8)goto
_LLE0;_tmp105=((struct Cyc_Absyn_Or_e_struct*)_tmpCE)->f1;_tmp106=((struct Cyc_Absyn_Or_e_struct*)
_tmpCE)->f2;_LLDF: {union Cyc_CfFlowInfo_FlowInfo_union _tmp238;union Cyc_CfFlowInfo_FlowInfo_union
_tmp239;struct _tuple8 _tmp237=Cyc_NewControlFlow_anal_test(env,inflow,_tmp105);
_tmp238=_tmp237.f1;_tmp239=_tmp237.f2;_tmp238=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp105->loc,_tmp238);_tmp239=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp105->loc,
_tmp239);{union Cyc_CfFlowInfo_FlowInfo_union _tmp23B;void*_tmp23C;struct _tuple5
_tmp23A=Cyc_NewControlFlow_anal_Rexp(env,_tmp239,_tmp106);_tmp23B=_tmp23A.f1;
_tmp23C=_tmp23A.f2;_tmp23B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp106->loc,
_tmp23B);{struct _tuple5 _tmp23D=({struct _tuple5 _tmp240;_tmp240.f1=_tmp23B;_tmp240.f2=
_tmp23C;_tmp240;});struct _tuple5 _tmp23E=({struct _tuple5 _tmp23F;_tmp23F.f1=
_tmp238;_tmp23F.f2=(void*)((void*)1);_tmp23F;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpC7,env->all_changed,_tmp23D,_tmp23E,0);}}}_LLE0: if(*((int*)_tmpCE)!= 25)goto
_LLE2;_tmp107=((struct Cyc_Absyn_Subscript_e_struct*)_tmpCE)->f1;_tmp108=((struct
Cyc_Absyn_Subscript_e_struct*)_tmpCE)->f2;_LLE1: {struct _RegionHandle _tmp241=
_new_region("temp");struct _RegionHandle*temp=& _tmp241;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp244;struct Cyc_List_List*_tmp245;struct _tuple11
_tmp243=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp242[2];_tmp242[1]=_tmp108;_tmp242[0]=_tmp107;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp242,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp244=_tmp243.f1;_tmp245=
_tmp243.f2;_tmp244=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp108->loc,_tmp244);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp246=_tmp244;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp247=_tmp244;struct Cyc_Dict_Dict _tmp248;struct Cyc_List_List*_tmp249;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp24A;_LL172: if((_tmp247.ReachableFL).tag != 1)goto _LL174;_tmp248=(_tmp247.ReachableFL).f1;
_tmp249=(_tmp247.ReachableFL).f2;_tmp24A=(_tmp247.ReachableFL).f3;_LL173: if(Cyc_CfFlowInfo_initlevel(
_tmp248,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp245))->tl))->hd)== (void*)0)({void*_tmp24B[0]={};Cyc_Tcutil_terr(
_tmp108->loc,({const char*_tmp24C="expression may not be initialized";
_tag_dynforward(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,34));}),
_tag_dynforward(_tmp24B,sizeof(void*),0));});{struct Cyc_List_List*_tmp24D=Cyc_NewControlFlow_add_subscript_reln(
_tmpC7->r,_tmp249,_tmp107,_tmp108);if(_tmp249 != _tmp24D)_tmp246=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp24E;(_tmp24E.ReachableFL).tag=1;(_tmp24E.ReachableFL).f1=
_tmp248;(_tmp24E.ReachableFL).f2=_tmp24D;(_tmp24E.ReachableFL).f3=_tmp24A;
_tmp24E;});goto _LL171;}_LL174:;_LL175: goto _LL171;_LL171:;}{void*_tmp24F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp107->topt))->v);struct Cyc_List_List*
_tmp250;struct Cyc_Absyn_PtrInfo _tmp251;struct Cyc_Absyn_PtrAtts _tmp252;struct Cyc_Absyn_Conref*
_tmp253;_LL177: if(_tmp24F <= (void*)4)goto _LL17B;if(*((int*)_tmp24F)!= 9)goto
_LL179;_tmp250=((struct Cyc_Absyn_TupleType_struct*)_tmp24F)->f1;_LL178: {void*
_tmp254=(void*)((struct Cyc_List_List*)_check_null(_tmp245))->hd;struct
_dynforward_ptr _tmp255;_LL17E: if(_tmp254 <= (void*)3)goto _LL180;if(*((int*)
_tmp254)!= 4)goto _LL180;_tmp255=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp254)->f1;
_LL17F: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp108)).f1;struct _tuple5
_tmp257=({struct _tuple5 _tmp256;_tmp256.f1=_tmp246;_tmp256.f2=*((void**)
_check_dynforward_subscript(_tmp255,sizeof(void*),(int)i));_tmp256;});
_npop_handler(0);return _tmp257;}_LL180:;_LL181:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258[0]=({struct Cyc_Core_Impossible_struct
_tmp259;_tmp259.tag=Cyc_Core_Impossible;_tmp259.f1=({const char*_tmp25A="anal_Rexp: Subscript";
_tag_dynforward(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,21));});_tmp259;});
_tmp258;}));_LL17D:;}_LL179: if(*((int*)_tmp24F)!= 4)goto _LL17B;_tmp251=((struct
Cyc_Absyn_PointerType_struct*)_tmp24F)->f1;_tmp252=_tmp251.ptr_atts;_tmp253=
_tmp252.bounds;_LL17A:{union Cyc_Absyn_Constraint_union _tmp25B=(Cyc_Absyn_compress_conref(
_tmp253))->v;void*_tmp25C;_LL183: if((_tmp25B.Eq_constr).tag != 0)goto _LL185;
_tmp25C=(_tmp25B.Eq_constr).f1;if(_tmp25C <= (void*)2)goto _LL185;if(*((int*)
_tmp25C)!= 1)goto _LL185;_LL184:{void*_tmp25D=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp245))->tl))->hd;struct Cyc_List_List*
_tmp25E;_LL188: if(_tmp25D <= (void*)3)goto _LL18A;if(*((int*)_tmp25D)!= 3)goto
_LL18A;_tmp25E=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp25D)->f1;_LL189:(void*)(
_tmp108->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp25F=
_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp260;_tmp260.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp260.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp25E);_tmp260;});_tmp25F;})));goto _LL187;_LL18A:;_LL18B:
goto _LL187;_LL187:;}goto _LL182;_LL185:;_LL186: goto _LL182;_LL182:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp262;void*_tmp263;struct _tuple5 _tmp261=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp244,_tmp107,(void*)((struct Cyc_List_List*)_check_null(_tmp245))->hd);
_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp264=
_tmp262;_LL18D: if((_tmp264.BottomFL).tag != 0)goto _LL18F;_LL18E: {struct _tuple5
_tmp266=({struct _tuple5 _tmp265;_tmp265.f1=_tmp262;_tmp265.f2=_tmp263;_tmp265;});
_npop_handler(0);return _tmp266;}_LL18F:;_LL190: {struct _tuple5 _tmp268=({struct
_tuple5 _tmp267;_tmp267.f1=_tmp246;_tmp267.f2=_tmp263;_tmp267;});_npop_handler(0);
return _tmp268;}_LL18C:;}}_LL17B:;_LL17C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269[0]=({struct Cyc_Core_Impossible_struct
_tmp26A;_tmp26A.tag=Cyc_Core_Impossible;_tmp26A.f1=({const char*_tmp26B="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,33));});_tmp26A;});
_tmp269;}));_LL176:;}}};_pop_region(temp);}_LLE2: if(*((int*)_tmpCE)!= 32)goto
_LLE4;_tmp109=((struct Cyc_Absyn_Tunion_e_struct*)_tmpCE)->f1;_tmp10A=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpCE)->f2;_LLE3: if(_tmp10A->is_flat){struct _RegionHandle _tmp26C=_new_region("temp");
struct _RegionHandle*temp=& _tmp26C;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp26E;struct Cyc_List_List*_tmp26F;struct _tuple11 _tmp26D=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp109,0);_tmp26E=_tmp26D.f1;_tmp26F=_tmp26D.f2;_tmp26E=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp26E);for(0;(unsigned int)_tmp109;(_tmp109=_tmp109->tl,_tmp26F=_tmp26F->tl)){
_tmp26E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp109->hd)->loc,
_tmp26E,(void*)((struct Cyc_List_List*)_check_null(_tmp26F))->hd);}{struct _tuple5
_tmp271=({struct _tuple5 _tmp270;_tmp270.f1=_tmp26E;_tmp270.f2=(void*)_tmpC7->unknown_all;
_tmp270;});_npop_handler(0);return _tmp271;}};_pop_region(temp);}_tmp10B=_tmp109;
goto _LLE5;_LLE4: if(*((int*)_tmpCE)!= 26)goto _LLE6;_tmp10B=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpCE)->f1;_LLE5: {struct _RegionHandle _tmp272=_new_region("temp");struct
_RegionHandle*temp=& _tmp272;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp274;struct Cyc_List_List*_tmp275;struct _tuple11 _tmp273=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp10B,0);_tmp274=_tmp273.f1;_tmp275=_tmp273.f2;_tmp274=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp274);{struct _dynforward_ptr aggrdict=({unsigned int _tmp27A=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10B);void**
_tmp27B=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp27A));
struct _dynforward_ptr _tmp27E=_tag_dynforward(_tmp27B,sizeof(void*),_tmp27A);{
unsigned int _tmp27C=_tmp27A;unsigned int i;for(i=0;i < _tmp27C;i ++){_tmp27B[i]=({
void*_tmp27D=(void*)((struct Cyc_List_List*)_check_null(_tmp275))->hd;_tmp275=
_tmp275->tl;_tmp27D;});}}_tmp27E;});struct _tuple5 _tmp279=({struct _tuple5 _tmp276;
_tmp276.f1=_tmp274;_tmp276.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp277=_region_malloc(env->r,sizeof(*_tmp277));_tmp277[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp278;_tmp278.tag=4;_tmp278.f1=aggrdict;_tmp278;});_tmp277;});_tmp276;});
_npop_handler(0);return _tmp279;}};_pop_region(temp);}_LLE6: if(*((int*)_tmpCE)!= 
31)goto _LLE8;_tmp10C=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpCE)->f2;_LLE7: {
struct Cyc_List_List*fs;{void*_tmp27F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_List_List*_tmp280;_LL192: if(_tmp27F <= (void*)
4)goto _LL194;if(*((int*)_tmp27F)!= 11)goto _LL194;_tmp280=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp27F)->f2;_LL193: fs=_tmp280;goto _LL191;_LL194:;_LL195:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({
struct Cyc_Core_Impossible_struct _tmp282;_tmp282.tag=Cyc_Core_Impossible;_tmp282.f1=({
const char*_tmp283="anal_Rexp:anon struct has bad type";_tag_dynforward(_tmp283,
sizeof(char),_get_zero_arr_size(_tmp283,35));});_tmp282;});_tmp281;}));_LL191:;}
_tmp10D=_tmp10C;_tmp112=fs;goto _LLE9;}_LLE8: if(*((int*)_tmpCE)!= 30)goto _LLEA;
_tmp10D=((struct Cyc_Absyn_Struct_e_struct*)_tmpCE)->f3;_tmp10E=((struct Cyc_Absyn_Struct_e_struct*)
_tmpCE)->f4;if(_tmp10E == 0)goto _LLEA;_tmp10F=*_tmp10E;_tmp110=_tmp10F.impl;if(
_tmp110 == 0)goto _LLEA;_tmp111=*_tmp110;_tmp112=_tmp111.fields;_LLE9: {struct
_RegionHandle _tmp284=_new_region("temp");struct _RegionHandle*temp=& _tmp284;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp286;struct Cyc_List_List*
_tmp287;struct _tuple11 _tmp285=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple13*))Cyc_Core_snd,_tmp10D),0);_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;
_tmp286=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp286);{struct
_dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmpC7,_tmp112,(
void*)_tmpC7->unknown_all);{int i=0;for(0;_tmp287 != 0;(((_tmp287=_tmp287->tl,
_tmp10D=_tmp10D->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple13*)((struct
Cyc_List_List*)_check_null(_tmp10D))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp288=(void*)ds->hd;struct _dynforward_ptr*_tmp289;_LL197: if(*((int*)_tmp288)!= 
0)goto _LL199;_LL198:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp28A=_cycalloc(sizeof(*_tmp28A));_tmp28A[0]=({struct Cyc_Core_Impossible_struct
_tmp28B;_tmp28B.tag=Cyc_Core_Impossible;_tmp28B.f1=({const char*_tmp28C="anal_Rexp:Struct_e";
_tag_dynforward(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,19));});_tmp28B;});
_tmp28A;}));_LL199: if(*((int*)_tmp288)!= 1)goto _LL196;_tmp289=((struct Cyc_Absyn_FieldName_struct*)
_tmp288)->f1;_LL19A: {int _tmp28D=Cyc_NewControlFlow_get_field_index_fs(_tmp112,
_tmp289);*((void**)_check_dynforward_subscript(aggrdict,sizeof(void*),_tmp28D))=(
void*)_tmp287->hd;}_LL196:;}}}{struct _tuple5 _tmp291=({struct _tuple5 _tmp28E;
_tmp28E.f1=_tmp286;_tmp28E.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp28F=_region_malloc(env->r,sizeof(*_tmp28F));_tmp28F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp290;_tmp290.tag=4;_tmp290.f1=aggrdict;_tmp290;});_tmp28F;});_tmp28E;});
_npop_handler(0);return _tmp291;}}};_pop_region(temp);}_LLEA: if(*((int*)_tmpCE)!= 
30)goto _LLEC;_LLEB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp292=
_cycalloc(sizeof(*_tmp292));_tmp292[0]=({struct Cyc_Core_Impossible_struct _tmp293;
_tmp293.tag=Cyc_Core_Impossible;_tmp293.f1=({const char*_tmp294="anal_Rexp:missing aggrdeclimpl";
_tag_dynforward(_tmp294,sizeof(char),_get_zero_arr_size(_tmp294,31));});_tmp293;});
_tmp292;}));_LLEC: if(*((int*)_tmpCE)!= 28)goto _LLEE;_tmp113=((struct Cyc_Absyn_Array_e_struct*)
_tmpCE)->f1;_LLED: {struct _RegionHandle _tmp295=_new_region("temp");struct
_RegionHandle*temp=& _tmp295;_push_region(temp);{struct Cyc_List_List*_tmp296=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple13*))Cyc_Core_snd,_tmp113);union Cyc_CfFlowInfo_FlowInfo_union _tmp298;
struct Cyc_List_List*_tmp299;struct _tuple11 _tmp297=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp296,0);_tmp298=_tmp297.f1;_tmp299=_tmp297.f2;_tmp298=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp298);for(0;_tmp299 != 0;(_tmp299=_tmp299->tl,_tmp296=_tmp296->tl)){
_tmp298=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp296))->hd)->loc,_tmp298,(void*)_tmp299->hd);}{struct _tuple5
_tmp29B=({struct _tuple5 _tmp29A;_tmp29A.f1=_tmp298;_tmp29A.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpC7,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpC7->unknown_all);
_tmp29A;});_npop_handler(0);return _tmp29B;}};_pop_region(temp);}_LLEE: if(*((int*)
_tmpCE)!= 29)goto _LLF0;_tmp114=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpCE)->f1;
_tmp115=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpCE)->f2;_tmp116=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmpCE)->f3;_tmp117=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpCE)->f4;_LLEF: {union Cyc_CfFlowInfo_FlowInfo_union _tmp29D;void*_tmp29E;
struct _tuple5 _tmp29C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp115);_tmp29D=
_tmp29C.f1;_tmp29E=_tmp29C.f2;_tmp29D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp115->loc,_tmp29D);{union Cyc_CfFlowInfo_FlowInfo_union _tmp29F=_tmp29D;struct
Cyc_Dict_Dict _tmp2A0;struct Cyc_List_List*_tmp2A1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2A2;_LL19C: if((_tmp29F.BottomFL).tag != 0)goto _LL19E;_LL19D: return({struct
_tuple5 _tmp2A3;_tmp2A3.f1=_tmp29D;_tmp2A3.f2=(void*)_tmpC7->unknown_all;_tmp2A3;});
_LL19E: if((_tmp29F.ReachableFL).tag != 1)goto _LL19B;_tmp2A0=(_tmp29F.ReachableFL).f1;
_tmp2A1=(_tmp29F.ReachableFL).f2;_tmp2A2=(_tmp29F.ReachableFL).f3;_LL19F: if(Cyc_CfFlowInfo_initlevel(
_tmp2A0,_tmp29E)== (void*)0)({void*_tmp2A4[0]={};Cyc_Tcutil_terr(_tmp115->loc,({
const char*_tmp2A5="expression may not be initialized";_tag_dynforward(_tmp2A5,
sizeof(char),_get_zero_arr_size(_tmp2A5,34));}),_tag_dynforward(_tmp2A4,sizeof(
void*),0));});{struct Cyc_List_List*new_relns=_tmp2A1;comp_loop: {void*_tmp2A6=(
void*)_tmp115->r;struct Cyc_Absyn_Exp*_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_Vardecl*
_tmp2A9;void*_tmp2AA;struct Cyc_Absyn_Vardecl*_tmp2AB;void*_tmp2AC;struct Cyc_Absyn_Vardecl*
_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_Vardecl*_tmp2AF;union Cyc_Absyn_Cnst_union
_tmp2B0;int _tmp2B1;void*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List
_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B5;_LL1A1: if(*((int*)_tmp2A6)!= 15)goto _LL1A3;
_tmp2A7=((struct Cyc_Absyn_Cast_e_struct*)_tmp2A6)->f2;_LL1A2: _tmp115=_tmp2A7;
goto comp_loop;_LL1A3: if(*((int*)_tmp2A6)!= 1)goto _LL1A5;_tmp2A8=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp2A6)->f2;if(_tmp2A8 <= (void*)1)goto _LL1A5;if(*((int*)
_tmp2A8)!= 0)goto _LL1A5;_tmp2A9=((struct Cyc_Absyn_Global_b_struct*)_tmp2A8)->f1;
if(!(!_tmp2A9->escapes))goto _LL1A5;_LL1A4: _tmp2AB=_tmp2A9;goto _LL1A6;_LL1A5: if(*((
int*)_tmp2A6)!= 1)goto _LL1A7;_tmp2AA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A6)->f2;if(_tmp2AA <= (void*)1)goto _LL1A7;if(*((int*)_tmp2AA)!= 3)goto _LL1A7;
_tmp2AB=((struct Cyc_Absyn_Local_b_struct*)_tmp2AA)->f1;if(!(!_tmp2AB->escapes))
goto _LL1A7;_LL1A6: _tmp2AD=_tmp2AB;goto _LL1A8;_LL1A7: if(*((int*)_tmp2A6)!= 1)goto
_LL1A9;_tmp2AC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A6)->f2;if(_tmp2AC <= (
void*)1)goto _LL1A9;if(*((int*)_tmp2AC)!= 4)goto _LL1A9;_tmp2AD=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2AC)->f1;if(!(!_tmp2AD->escapes))goto _LL1A9;_LL1A8: _tmp2AF=_tmp2AD;goto
_LL1AA;_LL1A9: if(*((int*)_tmp2A6)!= 1)goto _LL1AB;_tmp2AE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A6)->f2;if(_tmp2AE <= (void*)1)goto _LL1AB;if(*((int*)_tmp2AE)!= 2)goto _LL1AB;
_tmp2AF=((struct Cyc_Absyn_Param_b_struct*)_tmp2AE)->f1;if(!(!_tmp2AF->escapes))
goto _LL1AB;_LL1AA: new_relns=({struct Cyc_List_List*_tmp2B6=_region_malloc(env->r,
sizeof(*_tmp2B6));_tmp2B6->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2B7=_region_malloc(
env->r,sizeof(*_tmp2B7));_tmp2B7->vd=_tmp114;_tmp2B7->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2B8;(_tmp2B8.LessVar).tag=1;(_tmp2B8.LessVar).f1=
_tmp2AF;_tmp2B8;});_tmp2B7;});_tmp2B6->tl=_tmp2A1;_tmp2B6;});goto _LL1A0;_LL1AB:
if(*((int*)_tmp2A6)!= 0)goto _LL1AD;_tmp2B0=((struct Cyc_Absyn_Const_e_struct*)
_tmp2A6)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2A6)->f1).Int_c).tag != 2)
goto _LL1AD;_tmp2B1=(_tmp2B0.Int_c).f2;_LL1AC: new_relns=({struct Cyc_List_List*
_tmp2B9=_region_malloc(env->r,sizeof(*_tmp2B9));_tmp2B9->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2BA=_region_malloc(env->r,sizeof(*_tmp2BA));_tmp2BA->vd=_tmp114;_tmp2BA->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2BB;(
_tmp2BB.LessConst).tag=3;(_tmp2BB.LessConst).f1=(unsigned int)_tmp2B1;_tmp2BB;});
_tmp2BA;});_tmp2B9->tl=_tmp2A1;_tmp2B9;});goto _LL1A0;_LL1AD: if(*((int*)_tmp2A6)
!= 3)goto _LL1AF;_tmp2B2=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2A6)->f1;
if((int)_tmp2B2 != 19)goto _LL1AF;_tmp2B3=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2A6)->f2;if(_tmp2B3 == 0)goto _LL1AF;_tmp2B4=*_tmp2B3;_tmp2B5=(struct Cyc_Absyn_Exp*)
_tmp2B4.hd;_LL1AE:{void*_tmp2BC=(void*)_tmp2B5->r;void*_tmp2BD;struct Cyc_Absyn_Vardecl*
_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C0;void*_tmp2C1;struct Cyc_Absyn_Vardecl*
_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_Vardecl*_tmp2C4;_LL1B2: if(*((int*)_tmp2BC)
!= 1)goto _LL1B4;_tmp2BD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BC)->f2;if(
_tmp2BD <= (void*)1)goto _LL1B4;if(*((int*)_tmp2BD)!= 0)goto _LL1B4;_tmp2BE=((
struct Cyc_Absyn_Global_b_struct*)_tmp2BD)->f1;if(!(!_tmp2BE->escapes))goto _LL1B4;
_LL1B3: _tmp2C0=_tmp2BE;goto _LL1B5;_LL1B4: if(*((int*)_tmp2BC)!= 1)goto _LL1B6;
_tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BC)->f2;if(_tmp2BF <= (void*)
1)goto _LL1B6;if(*((int*)_tmp2BF)!= 3)goto _LL1B6;_tmp2C0=((struct Cyc_Absyn_Local_b_struct*)
_tmp2BF)->f1;if(!(!_tmp2C0->escapes))goto _LL1B6;_LL1B5: _tmp2C2=_tmp2C0;goto
_LL1B7;_LL1B6: if(*((int*)_tmp2BC)!= 1)goto _LL1B8;_tmp2C1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BC)->f2;if(_tmp2C1 <= (void*)1)goto _LL1B8;if(*((int*)_tmp2C1)!= 4)goto _LL1B8;
_tmp2C2=((struct Cyc_Absyn_Pat_b_struct*)_tmp2C1)->f1;if(!(!_tmp2C2->escapes))
goto _LL1B8;_LL1B7: _tmp2C4=_tmp2C2;goto _LL1B9;_LL1B8: if(*((int*)_tmp2BC)!= 1)goto
_LL1BA;_tmp2C3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BC)->f2;if(_tmp2C3 <= (
void*)1)goto _LL1BA;if(*((int*)_tmp2C3)!= 2)goto _LL1BA;_tmp2C4=((struct Cyc_Absyn_Param_b_struct*)
_tmp2C3)->f1;if(!(!_tmp2C4->escapes))goto _LL1BA;_LL1B9: new_relns=({struct Cyc_List_List*
_tmp2C5=_region_malloc(env->r,sizeof(*_tmp2C5));_tmp2C5->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2C6=_region_malloc(env->r,sizeof(*_tmp2C6));_tmp2C6->vd=_tmp114;_tmp2C6->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2C7;(
_tmp2C7.LessSize).tag=2;(_tmp2C7.LessSize).f1=_tmp2C4;_tmp2C7;});_tmp2C6;});
_tmp2C5->tl=_tmp2A1;_tmp2C5;});goto _LL1B1;_LL1BA:;_LL1BB: goto _LL1B1;_LL1B1:;}
goto _LL1A0;_LL1AF:;_LL1B0: goto _LL1A0;_LL1A0:;}if(_tmp2A1 != new_relns)_tmp29D=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp2C8;(
_tmp2C8.ReachableFL).tag=1;(_tmp2C8.ReachableFL).f1=_tmp2A0;(_tmp2C8.ReachableFL).f2=
new_relns;(_tmp2C8.ReachableFL).f3=_tmp2A2;_tmp2C8;});{void*_tmp2C9=_tmp29E;
_LL1BD: if((int)_tmp2C9 != 0)goto _LL1BF;_LL1BE: return({struct _tuple5 _tmp2CA;
_tmp2CA.f1=_tmp29D;_tmp2CA.f2=(void*)_tmpC7->unknown_all;_tmp2CA;});_LL1BF: if((
int)_tmp2C9 != 2)goto _LL1C1;_LL1C0: goto _LL1C2;_LL1C1: if((int)_tmp2C9 != 1)goto
_LL1C3;_LL1C2: goto _LL1C4;_LL1C3: if(_tmp2C9 <= (void*)3)goto _LL1C5;if(*((int*)
_tmp2C9)!= 2)goto _LL1C5;_LL1C4: {struct Cyc_List_List _tmp2CB=({struct Cyc_List_List
_tmp2D7;_tmp2D7.hd=_tmp114;_tmp2D7.tl=0;_tmp2D7;});_tmp29D=Cyc_NewControlFlow_add_vars(
_tmpC7,_tmp29D,(struct Cyc_List_List*)& _tmp2CB,(void*)_tmpC7->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp2CD;void*_tmp2CE;struct _tuple5 _tmp2CC=Cyc_NewControlFlow_anal_Rexp(
env,_tmp29D,_tmp116);_tmp2CD=_tmp2CC.f1;_tmp2CE=_tmp2CC.f2;_tmp2CD=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp116->loc,_tmp2CD);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2CF=_tmp2CD;struct
Cyc_Dict_Dict _tmp2D0;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2D1;_LL1C8: if((_tmp2CF.BottomFL).tag
!= 0)goto _LL1CA;_LL1C9: return({struct _tuple5 _tmp2D2;_tmp2D2.f1=_tmp2CD;_tmp2D2.f2=(
void*)_tmpC7->unknown_all;_tmp2D2;});_LL1CA: if((_tmp2CF.ReachableFL).tag != 1)
goto _LL1C7;_tmp2D0=(_tmp2CF.ReachableFL).f1;_tmp2D1=(_tmp2CF.ReachableFL).f3;
_LL1CB: if(Cyc_CfFlowInfo_initlevel(_tmp2D0,_tmp2CE)!= (void*)2){({void*_tmp2D3[0]={};
Cyc_Tcutil_terr(_tmp115->loc,({const char*_tmp2D4="expression may not be initialized";
_tag_dynforward(_tmp2D4,sizeof(char),_get_zero_arr_size(_tmp2D4,34));}),
_tag_dynforward(_tmp2D3,sizeof(void*),0));});return({struct _tuple5 _tmp2D5;
_tmp2D5.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2D6;(_tmp2D6.BottomFL).tag=0;_tmp2D6;});_tmp2D5.f2=(void*)_tmpC7->unknown_all;
_tmp2D5;});}_LL1C7:;}_tmp29D=_tmp2CD;goto _LL1C6;}}_LL1C5:;_LL1C6: while(1){struct
Cyc_List_List _tmp2D8=({struct Cyc_List_List _tmp2E4;_tmp2E4.hd=_tmp114;_tmp2E4.tl=
0;_tmp2E4;});_tmp29D=Cyc_NewControlFlow_add_vars(_tmpC7,_tmp29D,(struct Cyc_List_List*)&
_tmp2D8,(void*)_tmpC7->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DA;void*_tmp2DB;struct _tuple5 _tmp2D9=Cyc_NewControlFlow_anal_Rexp(env,
_tmp29D,_tmp116);_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;_tmp2DA=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp116->loc,_tmp2DA);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DC=_tmp2DA;struct
Cyc_Dict_Dict _tmp2DD;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2DE;_LL1CD: if((_tmp2DC.BottomFL).tag
!= 0)goto _LL1CF;_LL1CE: goto _LL1CC;_LL1CF: if((_tmp2DC.ReachableFL).tag != 1)goto
_LL1CC;_tmp2DD=(_tmp2DC.ReachableFL).f1;_tmp2DE=(_tmp2DC.ReachableFL).f3;_LL1D0:
if(Cyc_CfFlowInfo_initlevel(_tmp2DD,_tmp2DB)!= (void*)2){({void*_tmp2DF[0]={};
Cyc_Tcutil_terr(_tmp115->loc,({const char*_tmp2E0="expression may not be initialized";
_tag_dynforward(_tmp2E0,sizeof(char),_get_zero_arr_size(_tmp2E0,34));}),
_tag_dynforward(_tmp2DF,sizeof(void*),0));});return({struct _tuple5 _tmp2E1;
_tmp2E1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2E2;(_tmp2E2.BottomFL).tag=0;_tmp2E2;});_tmp2E1.f2=(void*)_tmpC7->unknown_all;
_tmp2E1;});}_LL1CC:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp2E3=Cyc_CfFlowInfo_join_flow(
_tmpC7,env->all_changed,_tmp29D,_tmp2DA,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp2E3,_tmp29D))break;_tmp29D=_tmp2E3;}}}return({struct _tuple5 _tmp2E5;_tmp2E5.f1=
_tmp29D;_tmp2E5.f2=(void*)_tmpC7->unknown_all;_tmp2E5;});_LL1BC:;}}_LL19B:;}}
_LLF0: if(*((int*)_tmpCE)!= 38)goto _LLF2;_tmp118=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpCE)->f1;_LLF1: while(1){union Cyc_CfFlowInfo_FlowInfo_union*_tmp2E7;struct
_tuple9 _tmp2E6=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp118);_tmp2E7=
_tmp2E6.f2;inflow=*_tmp2E7;{void*_tmp2E8=(void*)_tmp118->r;struct Cyc_Absyn_Stmt*
_tmp2E9;struct Cyc_Absyn_Stmt*_tmp2EA;struct Cyc_Absyn_Decl*_tmp2EB;struct Cyc_Absyn_Stmt*
_tmp2EC;struct Cyc_Absyn_Exp*_tmp2ED;_LL1D2: if(_tmp2E8 <= (void*)1)goto _LL1D8;if(*((
int*)_tmp2E8)!= 1)goto _LL1D4;_tmp2E9=((struct Cyc_Absyn_Seq_s_struct*)_tmp2E8)->f1;
_tmp2EA=((struct Cyc_Absyn_Seq_s_struct*)_tmp2E8)->f2;_LL1D3: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp2E9);_tmp118=_tmp2EA;goto _LL1D1;_LL1D4: if(*((int*)_tmp2E8)!= 11)
goto _LL1D6;_tmp2EB=((struct Cyc_Absyn_Decl_s_struct*)_tmp2E8)->f1;_tmp2EC=((
struct Cyc_Absyn_Decl_s_struct*)_tmp2E8)->f2;_LL1D5: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp2EB);_tmp118=_tmp2EC;goto _LL1D1;_LL1D6: if(*((int*)_tmp2E8)!= 0)
goto _LL1D8;_tmp2ED=((struct Cyc_Absyn_Exp_s_struct*)_tmp2E8)->f1;_LL1D7: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp2ED);_LL1D8:;_LL1D9:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE[0]=({struct Cyc_Core_Impossible_struct
_tmp2EF;_tmp2EF.tag=Cyc_Core_Impossible;_tmp2EF.f1=({const char*_tmp2F0="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp2F0,sizeof(char),_get_zero_arr_size(_tmp2F0,33));});_tmp2EF;});
_tmp2EE;}));_LL1D1:;}}_LLF2: if(*((int*)_tmpCE)!= 1)goto _LLF4;_tmp119=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpCE)->f2;if((int)_tmp119 != 0)goto _LLF4;_LLF3:
goto _LLF5;_LLF4: if(*((int*)_tmpCE)!= 2)goto _LLF6;_LLF5: goto _LLF7;_LLF6: if(*((int*)
_tmpCE)!= 10)goto _LLF8;_LLF7: goto _LLF9;_LLF8: if(*((int*)_tmpCE)!= 37)goto _LLFA;
_LLF9: goto _LLFB;_LLFA: if(*((int*)_tmpCE)!= 27)goto _LL95;_LLFB:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1[0]=({
struct Cyc_Core_Impossible_struct _tmp2F2;_tmp2F2.tag=Cyc_Core_Impossible;_tmp2F2.f1=({
const char*_tmp2F3="anal_Rexp, unexpected exp form";_tag_dynforward(_tmp2F3,
sizeof(char),_get_zero_arr_size(_tmp2F3,31));});_tmp2F2;});_tmp2F1;}));_LL95:;}}
static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,
struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*
_tmp2F4=env->fenv;void*_tmp2F5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp2F6;void*_tmp2F7;struct Cyc_Absyn_PtrAtts
_tmp2F8;struct Cyc_Absyn_Conref*_tmp2F9;struct Cyc_Absyn_Conref*_tmp2FA;_LL1DB: if(
_tmp2F5 <= (void*)4)goto _LL1DD;if(*((int*)_tmp2F5)!= 4)goto _LL1DD;_tmp2F6=((
struct Cyc_Absyn_PointerType_struct*)_tmp2F5)->f1;_tmp2F7=(void*)_tmp2F6.elt_typ;
_tmp2F8=_tmp2F6.ptr_atts;_tmp2F9=_tmp2F8.bounds;_tmp2FA=_tmp2F8.zero_term;_LL1DC: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp2FB=f;struct Cyc_Dict_Dict _tmp2FC;struct Cyc_List_List*
_tmp2FD;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2FE;_LL1E0: if((_tmp2FB.BottomFL).tag
!= 0)goto _LL1E2;_LL1E1: return({struct _tuple7 _tmp2FF;_tmp2FF.f1=f;_tmp2FF.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp300;(
_tmp300.UnknownL).tag=1;_tmp300;});_tmp2FF;});_LL1E2: if((_tmp2FB.ReachableFL).tag
!= 1)goto _LL1DF;_tmp2FC=(_tmp2FB.ReachableFL).f1;_tmp2FD=(_tmp2FB.ReachableFL).f2;
_tmp2FE=(_tmp2FB.ReachableFL).f3;_LL1E3: if(Cyc_Tcutil_is_bound_one(_tmp2F9)){
void*_tmp301=r;struct Cyc_CfFlowInfo_Place*_tmp302;struct Cyc_CfFlowInfo_Place
_tmp303;void*_tmp304;struct Cyc_List_List*_tmp305;void*_tmp306;_LL1E5: if((int)
_tmp301 != 1)goto _LL1E7;_LL1E6: goto _LL1E8;_LL1E7: if((int)_tmp301 != 2)goto _LL1E9;
_LL1E8:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp308;_tmp308.tag=Cyc_CfFlowInfo_NotZero;_tmp308.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp2FD);_tmp308;});_tmp307;})));goto _LL1E4;_LL1E9: if(
_tmp301 <= (void*)3)goto _LL1EB;if(*((int*)_tmp301)!= 2)goto _LL1EB;_tmp302=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp301)->f1;_tmp303=*_tmp302;_tmp304=(
void*)_tmp303.root;_tmp305=_tmp303.fields;_LL1EA:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp30A;_tmp30A.tag=Cyc_CfFlowInfo_NotZero;
_tmp30A.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp2FD);_tmp30A;});
_tmp309;})));return({struct _tuple7 _tmp30B;_tmp30B.f1=f;_tmp30B.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp30C;(_tmp30C.PlaceL).tag=0;(_tmp30C.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp30D=_region_malloc(_tmp2F4->r,sizeof(*_tmp30D));
_tmp30D->root=(void*)_tmp304;_tmp30D->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp2F4->r,_tmp305,flds);_tmp30D;});_tmp30C;});_tmp30B;});_LL1EB: if((int)_tmp301
!= 0)goto _LL1ED;_LL1EC:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp30E;_tmp30E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp310;(_tmp310.BottomFL).tag=0;_tmp310;});
_tmp30E.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp30F;(_tmp30F.UnknownL).tag=1;_tmp30F;});_tmp30E;});_LL1ED: if(_tmp301 <= (void*)
3)goto _LL1EF;if(*((int*)_tmp301)!= 0)goto _LL1EF;_tmp306=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp301)->f1;_LL1EE: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp306);goto
_LL1F0;_LL1EF:;_LL1F0:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp312;_tmp312.tag=Cyc_CfFlowInfo_UnknownZ;_tmp312.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp2FD);_tmp312;});_tmp311;})));_LL1E4:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp313=_cycalloc(sizeof(*
_tmp313));_tmp313[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp314;_tmp314.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp314.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp2FD);_tmp314;});_tmp313;})));}if(Cyc_CfFlowInfo_initlevel(_tmp2FC,r)== (void*)
0)({void*_tmp315[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp316="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp316,sizeof(char),_get_zero_arr_size(_tmp316,46));}),
_tag_dynforward(_tmp315,sizeof(void*),0));});return({struct _tuple7 _tmp317;
_tmp317.f1=f;_tmp317.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp318;(_tmp318.UnknownL).tag=1;_tmp318;});_tmp317;});_LL1DF:;}_LL1DD:;_LL1DE:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp319=_cycalloc(sizeof(*
_tmp319));_tmp319[0]=({struct Cyc_Core_Impossible_struct _tmp31A;_tmp31A.tag=Cyc_Core_Impossible;
_tmp31A.f1=({const char*_tmp31B="left deref of non-pointer-type";_tag_dynforward(
_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,31));});_tmp31A;});_tmp319;}));
_LL1DA:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp31C=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp31D=inflow;struct Cyc_Dict_Dict _tmp31E;struct Cyc_List_List*
_tmp31F;_LL1F2: if((_tmp31D.BottomFL).tag != 0)goto _LL1F4;_LL1F3: return({struct
_tuple7 _tmp320;_tmp320.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp322;(_tmp322.BottomFL).tag=0;_tmp322;});_tmp320.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp321;(_tmp321.UnknownL).tag=1;_tmp321;});
_tmp320;});_LL1F4: if((_tmp31D.ReachableFL).tag != 1)goto _LL1F1;_tmp31E=(_tmp31D.ReachableFL).f1;
_tmp31F=(_tmp31D.ReachableFL).f2;_LL1F5: d=_tmp31E;_LL1F1:;}{void*_tmp323=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_Absyn_Exp*_tmp325;struct Cyc_Absyn_Exp*
_tmp326;void*_tmp327;struct Cyc_Absyn_Vardecl*_tmp328;void*_tmp329;struct Cyc_Absyn_Vardecl*
_tmp32A;void*_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32C;void*_tmp32D;struct Cyc_Absyn_Vardecl*
_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;struct _dynforward_ptr*_tmp330;struct Cyc_Absyn_Exp*
_tmp331;struct Cyc_Absyn_Exp*_tmp332;struct Cyc_Absyn_Exp*_tmp333;struct Cyc_Absyn_Exp*
_tmp334;struct _dynforward_ptr*_tmp335;_LL1F7: if(*((int*)_tmp323)!= 15)goto _LL1F9;
_tmp324=((struct Cyc_Absyn_Cast_e_struct*)_tmp323)->f2;_LL1F8: _tmp325=_tmp324;
goto _LL1FA;_LL1F9: if(*((int*)_tmp323)!= 13)goto _LL1FB;_tmp325=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp323)->f1;_LL1FA: _tmp326=_tmp325;goto _LL1FC;_LL1FB: if(*((int*)_tmp323)!= 14)
goto _LL1FD;_tmp326=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp323)->f1;_LL1FC:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp326,flds);_LL1FD: if(*((int*)
_tmp323)!= 1)goto _LL1FF;_tmp327=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp323)->f2;
if(_tmp327 <= (void*)1)goto _LL1FF;if(*((int*)_tmp327)!= 2)goto _LL1FF;_tmp328=((
struct Cyc_Absyn_Param_b_struct*)_tmp327)->f1;_LL1FE: _tmp32A=_tmp328;goto _LL200;
_LL1FF: if(*((int*)_tmp323)!= 1)goto _LL201;_tmp329=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp323)->f2;if(_tmp329 <= (void*)1)goto _LL201;if(*((int*)_tmp329)!= 3)goto _LL201;
_tmp32A=((struct Cyc_Absyn_Local_b_struct*)_tmp329)->f1;_LL200: _tmp32C=_tmp32A;
goto _LL202;_LL201: if(*((int*)_tmp323)!= 1)goto _LL203;_tmp32B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp323)->f2;if(_tmp32B <= (void*)1)goto _LL203;if(*((int*)_tmp32B)!= 4)goto _LL203;
_tmp32C=((struct Cyc_Absyn_Pat_b_struct*)_tmp32B)->f1;_LL202: return({struct
_tuple7 _tmp336;_tmp336.f1=inflow;_tmp336.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp337;(_tmp337.PlaceL).tag=0;(_tmp337.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp338=_region_malloc(env->r,sizeof(*_tmp338));
_tmp338->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp339=
_region_malloc(env->r,sizeof(*_tmp339));_tmp339[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp33A;_tmp33A.tag=0;_tmp33A.f1=_tmp32C;_tmp33A;});_tmp339;}));_tmp338->fields=
flds;_tmp338;});_tmp337;});_tmp336;});_LL203: if(*((int*)_tmp323)!= 1)goto _LL205;
_tmp32D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp323)->f2;if(_tmp32D <= (void*)
1)goto _LL205;if(*((int*)_tmp32D)!= 0)goto _LL205;_tmp32E=((struct Cyc_Absyn_Global_b_struct*)
_tmp32D)->f1;_LL204: return({struct _tuple7 _tmp33B;_tmp33B.f1=inflow;_tmp33B.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp33C;(
_tmp33C.UnknownL).tag=1;_tmp33C;});_tmp33B;});_LL205: if(*((int*)_tmp323)!= 24)
goto _LL207;_tmp32F=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp323)->f1;_tmp330=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp323)->f2;_LL206:{void*_tmp33D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp32F->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp33E;void*_tmp33F;_LL210: if(_tmp33D <= (void*)4)goto _LL212;if(*((int*)_tmp33D)
!= 4)goto _LL212;_tmp33E=((struct Cyc_Absyn_PointerType_struct*)_tmp33D)->f1;
_tmp33F=(void*)_tmp33E.elt_typ;_LL211: if(!Cyc_Absyn_is_union_type(_tmp33F))flds=({
struct Cyc_List_List*_tmp340=_region_malloc(env->r,sizeof(*_tmp340));_tmp340->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp33F,_tmp330);_tmp340->tl=flds;
_tmp340;});goto _LL20F;_LL212:;_LL213:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341[0]=({struct Cyc_Core_Impossible_struct
_tmp342;_tmp342.tag=Cyc_Core_Impossible;_tmp342.f1=({const char*_tmp343="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,25));});_tmp342;});
_tmp341;}));_LL20F:;}_tmp331=_tmp32F;goto _LL208;_LL207: if(*((int*)_tmp323)!= 22)
goto _LL209;_tmp331=((struct Cyc_Absyn_Deref_e_struct*)_tmp323)->f1;_LL208: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp345;void*_tmp346;struct _tuple5 _tmp344=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp331);_tmp345=_tmp344.f1;_tmp346=_tmp344.f2;_tmp345=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp345);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp345,_tmp331,
_tmp346,flds);}_LL209: if(*((int*)_tmp323)!= 25)goto _LL20B;_tmp332=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp323)->f1;_tmp333=((struct Cyc_Absyn_Subscript_e_struct*)_tmp323)->f2;_LL20A: {
void*_tmp347=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp332->topt))->v);struct Cyc_Absyn_PtrInfo _tmp348;struct Cyc_Absyn_PtrAtts
_tmp349;struct Cyc_Absyn_Conref*_tmp34A;_LL215: if(_tmp347 <= (void*)4)goto _LL219;
if(*((int*)_tmp347)!= 9)goto _LL217;_LL216: {unsigned int _tmp34B=(Cyc_Evexp_eval_const_uint_exp(
_tmp333)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp332,({struct
Cyc_List_List*_tmp34C=_region_malloc(env->r,sizeof(*_tmp34C));_tmp34C->hd=(void*)((
int)_tmp34B);_tmp34C->tl=flds;_tmp34C;}));}_LL217: if(*((int*)_tmp347)!= 4)goto
_LL219;_tmp348=((struct Cyc_Absyn_PointerType_struct*)_tmp347)->f1;_tmp349=
_tmp348.ptr_atts;_tmp34A=_tmp349.bounds;_LL218: {struct _RegionHandle _tmp34D=
_new_region("temp");struct _RegionHandle*temp=& _tmp34D;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp350;struct Cyc_List_List*_tmp351;struct _tuple11
_tmp34F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp34E[2];_tmp34E[1]=_tmp333;_tmp34E[0]=_tmp332;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp34E,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp350=_tmp34F.f1;_tmp351=
_tmp34F.f2;_tmp350=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp333->loc,_tmp350);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp352=_tmp350;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp353=_tmp350;struct Cyc_Dict_Dict _tmp354;struct Cyc_List_List*_tmp355;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp356;_LL21C: if((_tmp353.ReachableFL).tag != 1)goto _LL21E;_tmp354=(_tmp353.ReachableFL).f1;
_tmp355=(_tmp353.ReachableFL).f2;_tmp356=(_tmp353.ReachableFL).f3;_LL21D: if(Cyc_CfFlowInfo_initlevel(
_tmp354,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp351))->tl))->hd)== (void*)0)({void*_tmp357[0]={};Cyc_Tcutil_terr(
_tmp333->loc,({const char*_tmp358="expression may not be initialized";
_tag_dynforward(_tmp358,sizeof(char),_get_zero_arr_size(_tmp358,34));}),
_tag_dynforward(_tmp357,sizeof(void*),0));});{struct Cyc_List_List*_tmp359=Cyc_NewControlFlow_add_subscript_reln(
_tmp31C->r,_tmp355,_tmp332,_tmp333);if(_tmp355 != _tmp359)_tmp352=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp35A;(_tmp35A.ReachableFL).tag=1;(_tmp35A.ReachableFL).f1=
_tmp354;(_tmp35A.ReachableFL).f2=_tmp359;(_tmp35A.ReachableFL).f3=_tmp356;
_tmp35A;});goto _LL21B;}_LL21E:;_LL21F: goto _LL21B;_LL21B:;}{union Cyc_Absyn_Constraint_union
_tmp35B=(Cyc_Absyn_compress_conref(_tmp34A))->v;void*_tmp35C;_LL221: if((_tmp35B.Eq_constr).tag
!= 0)goto _LL223;_tmp35C=(_tmp35B.Eq_constr).f1;if(_tmp35C <= (void*)2)goto _LL223;
if(*((int*)_tmp35C)!= 1)goto _LL223;_LL222:{void*_tmp35D=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp351))->tl))->hd;struct Cyc_List_List*
_tmp35E;_LL226: if(_tmp35D <= (void*)3)goto _LL228;if(*((int*)_tmp35D)!= 3)goto
_LL228;_tmp35E=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp35D)->f1;_LL227:(void*)(
_tmp333->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp35F=
_cycalloc(sizeof(*_tmp35F));_tmp35F[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp360;_tmp360.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp360.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp35E);_tmp360;});_tmp35F;})));goto _LL225;_LL228:;_LL229:
goto _LL225;_LL225:;}goto _LL220;_LL223:;_LL224: goto _LL220;_LL220:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp362;union Cyc_CfFlowInfo_AbsLVal_union _tmp363;struct _tuple7 _tmp361=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp350,_tmp332,(void*)((struct Cyc_List_List*)_check_null(_tmp351))->hd,
flds);_tmp362=_tmp361.f1;_tmp363=_tmp361.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp364=_tmp362;_LL22B: if((_tmp364.BottomFL).tag != 0)goto _LL22D;_LL22C: {struct
_tuple7 _tmp366=({struct _tuple7 _tmp365;_tmp365.f1=_tmp362;_tmp365.f2=_tmp363;
_tmp365;});_npop_handler(0);return _tmp366;}_LL22D:;_LL22E: {struct _tuple7 _tmp368=({
struct _tuple7 _tmp367;_tmp367.f1=_tmp352;_tmp367.f2=_tmp363;_tmp367;});
_npop_handler(0);return _tmp368;}_LL22A:;}}}};_pop_region(temp);}_LL219:;_LL21A:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp369=_cycalloc(sizeof(*
_tmp369));_tmp369[0]=({struct Cyc_Core_Impossible_struct _tmp36A;_tmp36A.tag=Cyc_Core_Impossible;
_tmp36A.f1=({const char*_tmp36B="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp36B,sizeof(char),_get_zero_arr_size(_tmp36B,33));});_tmp36A;});
_tmp369;}));_LL214:;}_LL20B: if(*((int*)_tmp323)!= 23)goto _LL20D;_tmp334=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp323)->f1;_tmp335=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp323)->f2;_LL20C: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp334->topt))->v))return({struct _tuple7 _tmp36C;_tmp36C.f1=inflow;
_tmp36C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp36D;(_tmp36D.UnknownL).tag=1;_tmp36D;});_tmp36C;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp334,({struct Cyc_List_List*_tmp36E=_region_malloc(env->r,sizeof(*
_tmp36E));_tmp36E->hd=(void*)Cyc_NewControlFlow_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp334->topt))->v,_tmp335);_tmp36E->tl=flds;_tmp36E;}));
_LL20D:;_LL20E: return({struct _tuple7 _tmp36F;_tmp36F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp371;(_tmp371.BottomFL).tag=0;_tmp371;});
_tmp36F.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp370;(_tmp370.UnknownL).tag=1;_tmp370;});_tmp36F;});_LL1F6:;}}static struct
_tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp373;union Cyc_CfFlowInfo_AbsLVal_union _tmp374;struct _tuple7 _tmp372=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp373=_tmp372.f1;_tmp374=_tmp372.f2;return({struct _tuple7
_tmp375;_tmp375.f1=_tmp373;_tmp375.f2=_tmp374;_tmp375;});}static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp376=env->fenv;{void*
_tmp377=(void*)e->r;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*_tmp379;
struct Cyc_Absyn_Exp*_tmp37A;struct Cyc_Absyn_Exp*_tmp37B;struct Cyc_Absyn_Exp*
_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Exp*
_tmp37F;struct Cyc_Absyn_Exp*_tmp380;void*_tmp381;struct Cyc_List_List*_tmp382;
struct Cyc_List_List _tmp383;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_List_List*
_tmp385;void*_tmp386;struct Cyc_List_List*_tmp387;_LL230: if(*((int*)_tmp377)!= 6)
goto _LL232;_tmp378=((struct Cyc_Absyn_Conditional_e_struct*)_tmp377)->f1;_tmp379=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp377)->f2;_tmp37A=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp377)->f3;_LL231: {union Cyc_CfFlowInfo_FlowInfo_union _tmp389;union Cyc_CfFlowInfo_FlowInfo_union
_tmp38A;struct _tuple8 _tmp388=Cyc_NewControlFlow_anal_test(env,inflow,_tmp378);
_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;_tmp389=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp378->loc,_tmp389);_tmp38A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp378->loc,
_tmp38A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp38C;union Cyc_CfFlowInfo_FlowInfo_union
_tmp38D;struct _tuple8 _tmp38B=Cyc_NewControlFlow_anal_test(env,_tmp389,_tmp379);
_tmp38C=_tmp38B.f1;_tmp38D=_tmp38B.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp38F;
union Cyc_CfFlowInfo_FlowInfo_union _tmp390;struct _tuple8 _tmp38E=Cyc_NewControlFlow_anal_test(
env,_tmp38A,_tmp37A);_tmp38F=_tmp38E.f1;_tmp390=_tmp38E.f2;return({struct _tuple8
_tmp391;_tmp391.f1=Cyc_CfFlowInfo_join_flow(_tmp376,env->all_changed,_tmp38C,
_tmp38F,1);_tmp391.f2=Cyc_CfFlowInfo_join_flow(_tmp376,env->all_changed,_tmp38D,
_tmp390,1);_tmp391;});}}}_LL232: if(*((int*)_tmp377)!= 7)goto _LL234;_tmp37B=((
struct Cyc_Absyn_And_e_struct*)_tmp377)->f1;_tmp37C=((struct Cyc_Absyn_And_e_struct*)
_tmp377)->f2;_LL233: {union Cyc_CfFlowInfo_FlowInfo_union _tmp393;union Cyc_CfFlowInfo_FlowInfo_union
_tmp394;struct _tuple8 _tmp392=Cyc_NewControlFlow_anal_test(env,inflow,_tmp37B);
_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;_tmp393=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp37B->loc,_tmp393);_tmp394=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp37B->loc,
_tmp394);{union Cyc_CfFlowInfo_FlowInfo_union _tmp396;union Cyc_CfFlowInfo_FlowInfo_union
_tmp397;struct _tuple8 _tmp395=Cyc_NewControlFlow_anal_test(env,_tmp393,_tmp37C);
_tmp396=_tmp395.f1;_tmp397=_tmp395.f2;_tmp396=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp37C->loc,_tmp396);_tmp397=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp37C->loc,
_tmp397);return({struct _tuple8 _tmp398;_tmp398.f1=_tmp396;_tmp398.f2=Cyc_CfFlowInfo_join_flow(
_tmp376,env->all_changed,_tmp394,_tmp397,0);_tmp398;});}}_LL234: if(*((int*)
_tmp377)!= 8)goto _LL236;_tmp37D=((struct Cyc_Absyn_Or_e_struct*)_tmp377)->f1;
_tmp37E=((struct Cyc_Absyn_Or_e_struct*)_tmp377)->f2;_LL235: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp39A;union Cyc_CfFlowInfo_FlowInfo_union _tmp39B;struct _tuple8 _tmp399=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp37D);_tmp39A=_tmp399.f1;_tmp39B=_tmp399.f2;_tmp39A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp37D->loc,_tmp39A);_tmp39B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp37D->loc,
_tmp39B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp39D;union Cyc_CfFlowInfo_FlowInfo_union
_tmp39E;struct _tuple8 _tmp39C=Cyc_NewControlFlow_anal_test(env,_tmp39B,_tmp37E);
_tmp39D=_tmp39C.f1;_tmp39E=_tmp39C.f2;_tmp39D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp37E->loc,_tmp39D);_tmp39E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp37E->loc,
_tmp39E);return({struct _tuple8 _tmp39F;_tmp39F.f1=Cyc_CfFlowInfo_join_flow(
_tmp376,env->all_changed,_tmp39A,_tmp39D,0);_tmp39F.f2=_tmp39E;_tmp39F;});}}
_LL236: if(*((int*)_tmp377)!= 9)goto _LL238;_tmp37F=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp377)->f1;_tmp380=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp377)->f2;_LL237: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3A1;void*_tmp3A2;struct _tuple5 _tmp3A0=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp37F);_tmp3A1=_tmp3A0.f1;_tmp3A2=_tmp3A0.f2;_tmp3A1=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp37F->loc,_tmp3A1);return Cyc_NewControlFlow_anal_test(env,_tmp3A1,_tmp380);}
_LL238: if(*((int*)_tmp377)!= 3)goto _LL23A;_tmp381=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp377)->f1;if((int)_tmp381 != 11)goto _LL23A;_tmp382=((struct Cyc_Absyn_Primop_e_struct*)
_tmp377)->f2;if(_tmp382 == 0)goto _LL23A;_tmp383=*_tmp382;_tmp384=(struct Cyc_Absyn_Exp*)
_tmp383.hd;_tmp385=_tmp383.tl;if(_tmp385 != 0)goto _LL23A;_LL239: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A4;union Cyc_CfFlowInfo_FlowInfo_union _tmp3A5;struct _tuple8 _tmp3A3=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp384);_tmp3A4=_tmp3A3.f1;_tmp3A5=_tmp3A3.f2;return({struct _tuple8
_tmp3A6;_tmp3A6.f1=_tmp3A5;_tmp3A6.f2=_tmp3A4;_tmp3A6;});}_LL23A: if(*((int*)
_tmp377)!= 3)goto _LL23C;_tmp386=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp377)->f1;_tmp387=((struct Cyc_Absyn_Primop_e_struct*)_tmp377)->f2;_LL23B: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3A7=
_new_region("temp");struct _RegionHandle*temp=& _tmp3A7;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3A9;struct Cyc_List_List*_tmp3AA;struct _tuple11
_tmp3A8=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp387,0);
_tmp3A9=_tmp3A8.f1;_tmp3AA=_tmp3A8.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3AA))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3AA->tl))->hd;
f=_tmp3A9;};_pop_region(temp);}{void*_tmp3AB=_tmp386;_LL23F: if((int)_tmp3AB != 0)
goto _LL241;_LL240: goto _LL242;_LL241: if((int)_tmp3AB != 2)goto _LL243;_LL242: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp387))->hd)->loc,f);
goto _LL23E;_LL243:;_LL244: f=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp387))->hd)->loc,f);goto _LL23E;_LL23E:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp3AC=f;struct Cyc_Dict_Dict _tmp3AD;struct Cyc_List_List*
_tmp3AE;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3AF;_LL246: if((_tmp3AC.BottomFL).tag
!= 0)goto _LL248;_LL247: return({struct _tuple8 _tmp3B0;_tmp3B0.f1=f;_tmp3B0.f2=f;
_tmp3B0;});_LL248: if((_tmp3AC.ReachableFL).tag != 1)goto _LL245;_tmp3AD=(_tmp3AC.ReachableFL).f1;
_tmp3AE=(_tmp3AC.ReachableFL).f2;_tmp3AF=(_tmp3AC.ReachableFL).f3;_LL249: {
struct Cyc_Absyn_Exp*_tmp3B1=(struct Cyc_Absyn_Exp*)_tmp387->hd;struct Cyc_Absyn_Exp*
_tmp3B2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp387->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp3AD,r1)== (void*)0)({void*_tmp3B3[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp387->hd)->loc,({const char*_tmp3B4="expression may not be initialized";
_tag_dynforward(_tmp3B4,sizeof(char),_get_zero_arr_size(_tmp3B4,34));}),
_tag_dynforward(_tmp3B3,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp3AD,
r2)== (void*)0)({void*_tmp3B5[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp387->tl))->hd)->loc,({const char*_tmp3B6="expression may not be initialized";
_tag_dynforward(_tmp3B6,sizeof(char),_get_zero_arr_size(_tmp3B6,34));}),
_tag_dynforward(_tmp3B5,sizeof(void*),0));});if(_tmp386 == (void*)5  || _tmp386 == (
void*)6){struct _tuple0 _tmp3B8=({struct _tuple0 _tmp3B7;_tmp3B7.f1=r1;_tmp3B7.f2=r2;
_tmp3B7;});void*_tmp3B9;void*_tmp3BA;void*_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*
_tmp3BE;void*_tmp3BF;void*_tmp3C0;void*_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*
_tmp3C4;void*_tmp3C5;void*_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*
_tmp3CA;void*_tmp3CB;void*_tmp3CC;_LL24B: _tmp3B9=_tmp3B8.f1;if(_tmp3B9 <= (void*)
3)goto _LL24D;if(*((int*)_tmp3B9)!= 0)goto _LL24D;_tmp3BA=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3B9)->f1;_tmp3BB=_tmp3B8.f2;if((int)_tmp3BB != 0)goto _LL24D;_LL24C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CE;union Cyc_CfFlowInfo_FlowInfo_union _tmp3CF;struct _tuple8 _tmp3CD=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3B1,_tmp3BA);_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;{void*_tmp3D0=
_tmp386;_LL260: if((int)_tmp3D0 != 5)goto _LL262;_LL261: return({struct _tuple8
_tmp3D1;_tmp3D1.f1=_tmp3CF;_tmp3D1.f2=_tmp3CE;_tmp3D1;});_LL262: if((int)_tmp3D0
!= 6)goto _LL264;_LL263: return({struct _tuple8 _tmp3D2;_tmp3D2.f1=_tmp3CE;_tmp3D2.f2=
_tmp3CF;_tmp3D2;});_LL264:;_LL265:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3[0]=({struct Cyc_Core_Impossible_struct
_tmp3D4;_tmp3D4.tag=Cyc_Core_Impossible;_tmp3D4.f1=({const char*_tmp3D5="anal_test, zero-split";
_tag_dynforward(_tmp3D5,sizeof(char),_get_zero_arr_size(_tmp3D5,22));});_tmp3D4;});
_tmp3D3;}));_LL25F:;}}_LL24D: _tmp3BC=_tmp3B8.f1;if((int)_tmp3BC != 0)goto _LL24F;
_tmp3BD=_tmp3B8.f2;if(_tmp3BD <= (void*)3)goto _LL24F;if(*((int*)_tmp3BD)!= 0)goto
_LL24F;_tmp3BE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3BD)->f1;
_LL24E: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3D7;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D8;struct _tuple8 _tmp3D6=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3B2,
_tmp3BE);_tmp3D7=_tmp3D6.f1;_tmp3D8=_tmp3D6.f2;{void*_tmp3D9=_tmp386;_LL267: if((
int)_tmp3D9 != 5)goto _LL269;_LL268: return({struct _tuple8 _tmp3DA;_tmp3DA.f1=
_tmp3D8;_tmp3DA.f2=_tmp3D7;_tmp3DA;});_LL269: if((int)_tmp3D9 != 6)goto _LL26B;
_LL26A: return({struct _tuple8 _tmp3DB;_tmp3DB.f1=_tmp3D7;_tmp3DB.f2=_tmp3D8;
_tmp3DB;});_LL26B:;_LL26C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC[0]=({struct Cyc_Core_Impossible_struct
_tmp3DD;_tmp3DD.tag=Cyc_Core_Impossible;_tmp3DD.f1=({const char*_tmp3DE="anal_test, zero-split";
_tag_dynforward(_tmp3DE,sizeof(char),_get_zero_arr_size(_tmp3DE,22));});_tmp3DD;});
_tmp3DC;}));_LL266:;}}_LL24F: _tmp3BF=_tmp3B8.f1;if((int)_tmp3BF != 0)goto _LL251;
_tmp3C0=_tmp3B8.f2;if((int)_tmp3C0 != 0)goto _LL251;_LL250: if(_tmp386 == (void*)5)
return({struct _tuple8 _tmp3DF;_tmp3DF.f1=f;_tmp3DF.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E0;(_tmp3E0.BottomFL).tag=0;_tmp3E0;});
_tmp3DF;});else{return({struct _tuple8 _tmp3E1;_tmp3E1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E2;(_tmp3E2.BottomFL).tag=0;_tmp3E2;});
_tmp3E1.f2=f;_tmp3E1;});}_LL251: _tmp3C1=_tmp3B8.f1;if((int)_tmp3C1 != 0)goto
_LL253;_tmp3C2=_tmp3B8.f2;if((int)_tmp3C2 != 1)goto _LL253;_LL252: goto _LL254;
_LL253: _tmp3C3=_tmp3B8.f1;if((int)_tmp3C3 != 0)goto _LL255;_tmp3C4=_tmp3B8.f2;if((
int)_tmp3C4 != 2)goto _LL255;_LL254: goto _LL256;_LL255: _tmp3C5=_tmp3B8.f1;if((int)
_tmp3C5 != 0)goto _LL257;_tmp3C6=_tmp3B8.f2;if(_tmp3C6 <= (void*)3)goto _LL257;if(*((
int*)_tmp3C6)!= 2)goto _LL257;_LL256: goto _LL258;_LL257: _tmp3C7=_tmp3B8.f1;if((int)
_tmp3C7 != 1)goto _LL259;_tmp3C8=_tmp3B8.f2;if((int)_tmp3C8 != 0)goto _LL259;_LL258:
goto _LL25A;_LL259: _tmp3C9=_tmp3B8.f1;if((int)_tmp3C9 != 2)goto _LL25B;_tmp3CA=
_tmp3B8.f2;if((int)_tmp3CA != 0)goto _LL25B;_LL25A: goto _LL25C;_LL25B: _tmp3CB=
_tmp3B8.f1;if(_tmp3CB <= (void*)3)goto _LL25D;if(*((int*)_tmp3CB)!= 2)goto _LL25D;
_tmp3CC=_tmp3B8.f2;if((int)_tmp3CC != 0)goto _LL25D;_LL25C: if(_tmp386 == (void*)6)
return({struct _tuple8 _tmp3E3;_tmp3E3.f1=f;_tmp3E3.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E4;(_tmp3E4.BottomFL).tag=0;_tmp3E4;});
_tmp3E3;});else{return({struct _tuple8 _tmp3E5;_tmp3E5.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E6;(_tmp3E6.BottomFL).tag=0;_tmp3E6;});
_tmp3E5.f2=f;_tmp3E5;});}_LL25D:;_LL25E: goto _LL24A;_LL24A:;}{struct _tuple0
_tmp3E8=({struct _tuple0 _tmp3E7;_tmp3E7.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3B1->topt))->v);_tmp3E7.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3B2->topt))->v);_tmp3E7;});void*_tmp3E9;void*
_tmp3EA;void*_tmp3EB;void*_tmp3EC;void*_tmp3ED;void*_tmp3EE;_LL26E: _tmp3E9=
_tmp3E8.f1;if(_tmp3E9 <= (void*)4)goto _LL270;if(*((int*)_tmp3E9)!= 5)goto _LL270;
_tmp3EA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E9)->f1;if((int)_tmp3EA != 
1)goto _LL270;_LL26F: goto _LL271;_LL270: _tmp3EB=_tmp3E8.f2;if(_tmp3EB <= (void*)4)
goto _LL272;if(*((int*)_tmp3EB)!= 5)goto _LL272;_tmp3EC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3EB)->f1;if((int)_tmp3EC != 1)goto _LL272;_LL271: goto _LL273;_LL272: _tmp3ED=
_tmp3E8.f1;if(_tmp3ED <= (void*)4)goto _LL274;if(*((int*)_tmp3ED)!= 18)goto _LL274;
_LL273: goto _LL275;_LL274: _tmp3EE=_tmp3E8.f2;if(_tmp3EE <= (void*)4)goto _LL276;if(*((
int*)_tmp3EE)!= 18)goto _LL276;_LL275: goto _LL26D;_LL276:;_LL277: return({struct
_tuple8 _tmp3EF;_tmp3EF.f1=f;_tmp3EF.f2=f;_tmp3EF;});_LL26D:;}{void*_tmp3F0=
_tmp386;_LL279: if((int)_tmp3F0 != 5)goto _LL27B;_LL27A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F1=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3B1,r1,(void*)5,r2);_tmp3F1=Cyc_NewControlFlow_if_tagcmp(
env,_tmp3F1,_tmp3B2,r2,(void*)5,r1);return({struct _tuple8 _tmp3F2;_tmp3F2.f1=
_tmp3F1;_tmp3F2.f2=f;_tmp3F2;});}_LL27B: if((int)_tmp3F0 != 6)goto _LL27D;_LL27C: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3F3=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3B1,r1,(void*)5,r2);_tmp3F3=Cyc_NewControlFlow_if_tagcmp(env,_tmp3F3,_tmp3B2,
r2,(void*)5,r1);return({struct _tuple8 _tmp3F4;_tmp3F4.f1=f;_tmp3F4.f2=_tmp3F3;
_tmp3F4;});}_LL27D: if((int)_tmp3F0 != 7)goto _LL27F;_LL27E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F5=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3B2,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F6=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3B1,r1,(void*)10,r2);return({
struct _tuple8 _tmp3F7;_tmp3F7.f1=_tmp3F5;_tmp3F7.f2=_tmp3F6;_tmp3F7;});}_LL27F:
if((int)_tmp3F0 != 9)goto _LL281;_LL280: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F8=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3B2,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3F9=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3B1,r1,(void*)8,r2);return({
struct _tuple8 _tmp3FA;_tmp3FA.f1=_tmp3F8;_tmp3FA.f2=_tmp3F9;_tmp3FA;});}_LL281:
if((int)_tmp3F0 != 8)goto _LL283;_LL282: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3FB=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3B1,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp3FC=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3B2,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3FD=_tmp3FB;struct Cyc_Dict_Dict _tmp3FE;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3FF;_LL288: if((_tmp3FD.BottomFL).tag != 0)goto _LL28A;_LL289:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400[0]=({
struct Cyc_Core_Impossible_struct _tmp401;_tmp401.tag=Cyc_Core_Impossible;_tmp401.f1=({
const char*_tmp402="anal_test, Lt";_tag_dynforward(_tmp402,sizeof(char),
_get_zero_arr_size(_tmp402,14));});_tmp401;});_tmp400;}));_LL28A: if((_tmp3FD.ReachableFL).tag
!= 1)goto _LL287;_tmp3FE=(_tmp3FD.ReachableFL).f1;_tmp3FF=(_tmp3FD.ReachableFL).f3;
_LL28B: _tmp3AD=_tmp3FE;_tmp3AF=_tmp3FF;_LL287:;}{void*_tmp403=(void*)_tmp3B1->r;
void*_tmp404;struct Cyc_Absyn_Vardecl*_tmp405;void*_tmp406;struct Cyc_Absyn_Vardecl*
_tmp407;void*_tmp408;struct Cyc_Absyn_Vardecl*_tmp409;void*_tmp40A;struct Cyc_Absyn_Vardecl*
_tmp40B;_LL28D: if(*((int*)_tmp403)!= 1)goto _LL28F;_tmp404=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp403)->f2;if(_tmp404 <= (void*)1)goto _LL28F;if(*((int*)_tmp404)!= 0)goto _LL28F;
_tmp405=((struct Cyc_Absyn_Global_b_struct*)_tmp404)->f1;if(!(!_tmp405->escapes))
goto _LL28F;_LL28E: _tmp407=_tmp405;goto _LL290;_LL28F: if(*((int*)_tmp403)!= 1)goto
_LL291;_tmp406=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp403)->f2;if(_tmp406 <= (
void*)1)goto _LL291;if(*((int*)_tmp406)!= 3)goto _LL291;_tmp407=((struct Cyc_Absyn_Local_b_struct*)
_tmp406)->f1;if(!(!_tmp407->escapes))goto _LL291;_LL290: _tmp409=_tmp407;goto
_LL292;_LL291: if(*((int*)_tmp403)!= 1)goto _LL293;_tmp408=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp403)->f2;if(_tmp408 <= (void*)1)goto _LL293;if(*((int*)_tmp408)!= 4)goto _LL293;
_tmp409=((struct Cyc_Absyn_Pat_b_struct*)_tmp408)->f1;if(!(!_tmp409->escapes))
goto _LL293;_LL292: _tmp40B=_tmp409;goto _LL294;_LL293: if(*((int*)_tmp403)!= 1)goto
_LL295;_tmp40A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp403)->f2;if(_tmp40A <= (
void*)1)goto _LL295;if(*((int*)_tmp40A)!= 2)goto _LL295;_tmp40B=((struct Cyc_Absyn_Param_b_struct*)
_tmp40A)->f1;if(!(!_tmp40B->escapes))goto _LL295;_LL294: {void*_tmp40C=(void*)
_tmp3B2->r;void*_tmp40D;struct Cyc_Absyn_Vardecl*_tmp40E;void*_tmp40F;struct Cyc_Absyn_Vardecl*
_tmp410;void*_tmp411;struct Cyc_Absyn_Vardecl*_tmp412;void*_tmp413;struct Cyc_Absyn_Vardecl*
_tmp414;union Cyc_Absyn_Cnst_union _tmp415;int _tmp416;void*_tmp417;struct Cyc_List_List*
_tmp418;struct Cyc_List_List _tmp419;struct Cyc_Absyn_Exp*_tmp41A;_LL298: if(*((int*)
_tmp40C)!= 1)goto _LL29A;_tmp40D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp40C)->f2;
if(_tmp40D <= (void*)1)goto _LL29A;if(*((int*)_tmp40D)!= 0)goto _LL29A;_tmp40E=((
struct Cyc_Absyn_Global_b_struct*)_tmp40D)->f1;if(!(!_tmp40E->escapes))goto _LL29A;
_LL299: _tmp410=_tmp40E;goto _LL29B;_LL29A: if(*((int*)_tmp40C)!= 1)goto _LL29C;
_tmp40F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp40C)->f2;if(_tmp40F <= (void*)
1)goto _LL29C;if(*((int*)_tmp40F)!= 3)goto _LL29C;_tmp410=((struct Cyc_Absyn_Local_b_struct*)
_tmp40F)->f1;if(!(!_tmp410->escapes))goto _LL29C;_LL29B: _tmp412=_tmp410;goto
_LL29D;_LL29C: if(*((int*)_tmp40C)!= 1)goto _LL29E;_tmp411=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp40C)->f2;if(_tmp411 <= (void*)1)goto _LL29E;if(*((int*)_tmp411)!= 4)goto _LL29E;
_tmp412=((struct Cyc_Absyn_Pat_b_struct*)_tmp411)->f1;if(!(!_tmp412->escapes))
goto _LL29E;_LL29D: _tmp414=_tmp412;goto _LL29F;_LL29E: if(*((int*)_tmp40C)!= 1)goto
_LL2A0;_tmp413=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp40C)->f2;if(_tmp413 <= (
void*)1)goto _LL2A0;if(*((int*)_tmp413)!= 2)goto _LL2A0;_tmp414=((struct Cyc_Absyn_Param_b_struct*)
_tmp413)->f1;if(!(!_tmp414->escapes))goto _LL2A0;_LL29F: {struct _RegionHandle*
_tmp41B=(env->fenv)->r;struct Cyc_List_List*_tmp41C=({struct Cyc_List_List*_tmp41F=
_region_malloc(_tmp41B,sizeof(*_tmp41F));_tmp41F->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp420=_region_malloc(_tmp41B,sizeof(*_tmp420));_tmp420->vd=_tmp40B;_tmp420->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp421;(
_tmp421.LessVar).tag=1;(_tmp421.LessVar).f1=_tmp414;_tmp421;});_tmp420;});
_tmp41F->tl=_tmp3AE;_tmp41F;});return({struct _tuple8 _tmp41D;_tmp41D.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp41E;(_tmp41E.ReachableFL).tag=1;(_tmp41E.ReachableFL).f1=
_tmp3AD;(_tmp41E.ReachableFL).f2=_tmp41C;(_tmp41E.ReachableFL).f3=_tmp3AF;
_tmp41E;});_tmp41D.f2=_tmp3FC;_tmp41D;});}_LL2A0: if(*((int*)_tmp40C)!= 0)goto
_LL2A2;_tmp415=((struct Cyc_Absyn_Const_e_struct*)_tmp40C)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp40C)->f1).Int_c).tag != 2)goto _LL2A2;_tmp416=(_tmp415.Int_c).f2;_LL2A1: {
struct _RegionHandle*_tmp422=(env->fenv)->r;struct Cyc_List_List*_tmp423=({struct
Cyc_List_List*_tmp426=_region_malloc(_tmp422,sizeof(*_tmp426));_tmp426->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp427=_region_malloc(_tmp422,sizeof(*_tmp427));
_tmp427->vd=_tmp40B;_tmp427->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp428;(_tmp428.LessConst).tag=3;(_tmp428.LessConst).f1=(unsigned int)_tmp416;
_tmp428;});_tmp427;});_tmp426->tl=_tmp3AE;_tmp426;});return({struct _tuple8
_tmp424;_tmp424.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp425;(_tmp425.ReachableFL).tag=1;(_tmp425.ReachableFL).f1=_tmp3AD;(_tmp425.ReachableFL).f2=
_tmp423;(_tmp425.ReachableFL).f3=_tmp3AF;_tmp425;});_tmp424.f2=_tmp3FC;_tmp424;});}
_LL2A2: if(*((int*)_tmp40C)!= 3)goto _LL2A4;_tmp417=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp40C)->f1;if((int)_tmp417 != 19)goto _LL2A4;_tmp418=((struct Cyc_Absyn_Primop_e_struct*)
_tmp40C)->f2;if(_tmp418 == 0)goto _LL2A4;_tmp419=*_tmp418;_tmp41A=(struct Cyc_Absyn_Exp*)
_tmp419.hd;_LL2A3: {void*_tmp429=(void*)_tmp41A->r;void*_tmp42A;struct Cyc_Absyn_Vardecl*
_tmp42B;void*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42D;void*_tmp42E;struct Cyc_Absyn_Vardecl*
_tmp42F;void*_tmp430;struct Cyc_Absyn_Vardecl*_tmp431;_LL2A7: if(*((int*)_tmp429)
!= 1)goto _LL2A9;_tmp42A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp429)->f2;if(
_tmp42A <= (void*)1)goto _LL2A9;if(*((int*)_tmp42A)!= 0)goto _LL2A9;_tmp42B=((
struct Cyc_Absyn_Global_b_struct*)_tmp42A)->f1;if(!(!_tmp42B->escapes))goto _LL2A9;
_LL2A8: _tmp42D=_tmp42B;goto _LL2AA;_LL2A9: if(*((int*)_tmp429)!= 1)goto _LL2AB;
_tmp42C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp429)->f2;if(_tmp42C <= (void*)
1)goto _LL2AB;if(*((int*)_tmp42C)!= 3)goto _LL2AB;_tmp42D=((struct Cyc_Absyn_Local_b_struct*)
_tmp42C)->f1;if(!(!_tmp42D->escapes))goto _LL2AB;_LL2AA: _tmp42F=_tmp42D;goto
_LL2AC;_LL2AB: if(*((int*)_tmp429)!= 1)goto _LL2AD;_tmp42E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp429)->f2;if(_tmp42E <= (void*)1)goto _LL2AD;if(*((int*)_tmp42E)!= 4)goto _LL2AD;
_tmp42F=((struct Cyc_Absyn_Pat_b_struct*)_tmp42E)->f1;if(!(!_tmp42F->escapes))
goto _LL2AD;_LL2AC: _tmp431=_tmp42F;goto _LL2AE;_LL2AD: if(*((int*)_tmp429)!= 1)goto
_LL2AF;_tmp430=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp429)->f2;if(_tmp430 <= (
void*)1)goto _LL2AF;if(*((int*)_tmp430)!= 2)goto _LL2AF;_tmp431=((struct Cyc_Absyn_Param_b_struct*)
_tmp430)->f1;if(!(!_tmp431->escapes))goto _LL2AF;_LL2AE: {struct _RegionHandle*
_tmp432=(env->fenv)->r;struct Cyc_List_List*_tmp433=({struct Cyc_List_List*_tmp436=
_region_malloc(_tmp432,sizeof(*_tmp436));_tmp436->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp437=_region_malloc(_tmp432,sizeof(*_tmp437));_tmp437->vd=_tmp40B;_tmp437->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp438;(
_tmp438.LessSize).tag=2;(_tmp438.LessSize).f1=_tmp431;_tmp438;});_tmp437;});
_tmp436->tl=_tmp3AE;_tmp436;});return({struct _tuple8 _tmp434;_tmp434.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp435;(_tmp435.ReachableFL).tag=1;(_tmp435.ReachableFL).f1=
_tmp3AD;(_tmp435.ReachableFL).f2=_tmp433;(_tmp435.ReachableFL).f3=_tmp3AF;
_tmp435;});_tmp434.f2=_tmp3FC;_tmp434;});}_LL2AF:;_LL2B0: return({struct _tuple8
_tmp439;_tmp439.f1=_tmp3FB;_tmp439.f2=_tmp3FC;_tmp439;});_LL2A6:;}_LL2A4:;_LL2A5:
return({struct _tuple8 _tmp43A;_tmp43A.f1=_tmp3FB;_tmp43A.f2=_tmp3FC;_tmp43A;});
_LL297:;}_LL295:;_LL296: return({struct _tuple8 _tmp43B;_tmp43B.f1=_tmp3FB;_tmp43B.f2=
_tmp3FC;_tmp43B;});_LL28C:;}}_LL283: if((int)_tmp3F0 != 10)goto _LL285;_LL284: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp43C=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3B1,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp43D=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3B2,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp43E=_tmp43C;
struct Cyc_Dict_Dict _tmp43F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp440;_LL2B2: if((
_tmp43E.BottomFL).tag != 0)goto _LL2B4;_LL2B3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441[0]=({struct Cyc_Core_Impossible_struct
_tmp442;_tmp442.tag=Cyc_Core_Impossible;_tmp442.f1=({const char*_tmp443="anal_test, Lte";
_tag_dynforward(_tmp443,sizeof(char),_get_zero_arr_size(_tmp443,15));});_tmp442;});
_tmp441;}));_LL2B4: if((_tmp43E.ReachableFL).tag != 1)goto _LL2B1;_tmp43F=(_tmp43E.ReachableFL).f1;
_tmp440=(_tmp43E.ReachableFL).f3;_LL2B5: _tmp3AD=_tmp43F;_tmp3AF=_tmp440;_LL2B1:;}{
void*_tmp444=(void*)_tmp3B1->r;void*_tmp445;struct Cyc_Absyn_Vardecl*_tmp446;void*
_tmp447;struct Cyc_Absyn_Vardecl*_tmp448;void*_tmp449;struct Cyc_Absyn_Vardecl*
_tmp44A;void*_tmp44B;struct Cyc_Absyn_Vardecl*_tmp44C;_LL2B7: if(*((int*)_tmp444)
!= 1)goto _LL2B9;_tmp445=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp444)->f2;if(
_tmp445 <= (void*)1)goto _LL2B9;if(*((int*)_tmp445)!= 0)goto _LL2B9;_tmp446=((
struct Cyc_Absyn_Global_b_struct*)_tmp445)->f1;if(!(!_tmp446->escapes))goto _LL2B9;
_LL2B8: _tmp448=_tmp446;goto _LL2BA;_LL2B9: if(*((int*)_tmp444)!= 1)goto _LL2BB;
_tmp447=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp444)->f2;if(_tmp447 <= (void*)
1)goto _LL2BB;if(*((int*)_tmp447)!= 3)goto _LL2BB;_tmp448=((struct Cyc_Absyn_Local_b_struct*)
_tmp447)->f1;if(!(!_tmp448->escapes))goto _LL2BB;_LL2BA: _tmp44A=_tmp448;goto
_LL2BC;_LL2BB: if(*((int*)_tmp444)!= 1)goto _LL2BD;_tmp449=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp444)->f2;if(_tmp449 <= (void*)1)goto _LL2BD;if(*((int*)_tmp449)!= 4)goto _LL2BD;
_tmp44A=((struct Cyc_Absyn_Pat_b_struct*)_tmp449)->f1;if(!(!_tmp44A->escapes))
goto _LL2BD;_LL2BC: _tmp44C=_tmp44A;goto _LL2BE;_LL2BD: if(*((int*)_tmp444)!= 1)goto
_LL2BF;_tmp44B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp444)->f2;if(_tmp44B <= (
void*)1)goto _LL2BF;if(*((int*)_tmp44B)!= 2)goto _LL2BF;_tmp44C=((struct Cyc_Absyn_Param_b_struct*)
_tmp44B)->f1;if(!(!_tmp44C->escapes))goto _LL2BF;_LL2BE: {void*_tmp44D=(void*)
_tmp3B2->r;void*_tmp44E;struct Cyc_List_List*_tmp44F;struct Cyc_List_List _tmp450;
struct Cyc_Absyn_Exp*_tmp451;_LL2C2: if(*((int*)_tmp44D)!= 3)goto _LL2C4;_tmp44E=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp44D)->f1;if((int)_tmp44E != 19)goto
_LL2C4;_tmp44F=((struct Cyc_Absyn_Primop_e_struct*)_tmp44D)->f2;if(_tmp44F == 0)
goto _LL2C4;_tmp450=*_tmp44F;_tmp451=(struct Cyc_Absyn_Exp*)_tmp450.hd;_LL2C3: {
void*_tmp452=(void*)_tmp451->r;void*_tmp453;struct Cyc_Absyn_Vardecl*_tmp454;void*
_tmp455;struct Cyc_Absyn_Vardecl*_tmp456;void*_tmp457;struct Cyc_Absyn_Vardecl*
_tmp458;void*_tmp459;struct Cyc_Absyn_Vardecl*_tmp45A;_LL2C7: if(*((int*)_tmp452)
!= 1)goto _LL2C9;_tmp453=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp452)->f2;if(
_tmp453 <= (void*)1)goto _LL2C9;if(*((int*)_tmp453)!= 0)goto _LL2C9;_tmp454=((
struct Cyc_Absyn_Global_b_struct*)_tmp453)->f1;if(!(!_tmp454->escapes))goto _LL2C9;
_LL2C8: _tmp456=_tmp454;goto _LL2CA;_LL2C9: if(*((int*)_tmp452)!= 1)goto _LL2CB;
_tmp455=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp452)->f2;if(_tmp455 <= (void*)
1)goto _LL2CB;if(*((int*)_tmp455)!= 3)goto _LL2CB;_tmp456=((struct Cyc_Absyn_Local_b_struct*)
_tmp455)->f1;if(!(!_tmp456->escapes))goto _LL2CB;_LL2CA: _tmp458=_tmp456;goto
_LL2CC;_LL2CB: if(*((int*)_tmp452)!= 1)goto _LL2CD;_tmp457=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp452)->f2;if(_tmp457 <= (void*)1)goto _LL2CD;if(*((int*)_tmp457)!= 4)goto _LL2CD;
_tmp458=((struct Cyc_Absyn_Pat_b_struct*)_tmp457)->f1;if(!(!_tmp458->escapes))
goto _LL2CD;_LL2CC: _tmp45A=_tmp458;goto _LL2CE;_LL2CD: if(*((int*)_tmp452)!= 1)goto
_LL2CF;_tmp459=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp452)->f2;if(_tmp459 <= (
void*)1)goto _LL2CF;if(*((int*)_tmp459)!= 2)goto _LL2CF;_tmp45A=((struct Cyc_Absyn_Param_b_struct*)
_tmp459)->f1;if(!(!_tmp45A->escapes))goto _LL2CF;_LL2CE: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp45B=env->fenv;struct Cyc_List_List*_tmp45C=({struct Cyc_List_List*_tmp45F=
_region_malloc(_tmp45B->r,sizeof(*_tmp45F));_tmp45F->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp460=_region_malloc(_tmp45B->r,sizeof(*_tmp460));_tmp460->vd=_tmp44C;_tmp460->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp461;(
_tmp461.LessEqSize).tag=4;(_tmp461.LessEqSize).f1=_tmp45A;_tmp461;});_tmp460;});
_tmp45F->tl=_tmp3AE;_tmp45F;});return({struct _tuple8 _tmp45D;_tmp45D.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp45E;(_tmp45E.ReachableFL).tag=1;(_tmp45E.ReachableFL).f1=
_tmp3AD;(_tmp45E.ReachableFL).f2=_tmp45C;(_tmp45E.ReachableFL).f3=_tmp3AF;
_tmp45E;});_tmp45D.f2=_tmp43D;_tmp45D;});}_LL2CF:;_LL2D0: return({struct _tuple8
_tmp462;_tmp462.f1=_tmp43C;_tmp462.f2=_tmp43D;_tmp462;});_LL2C6:;}_LL2C4:;_LL2C5:
return({struct _tuple8 _tmp463;_tmp463.f1=_tmp43C;_tmp463.f2=_tmp43D;_tmp463;});
_LL2C1:;}_LL2BF:;_LL2C0: return({struct _tuple8 _tmp464;_tmp464.f1=_tmp43C;_tmp464.f2=
_tmp43D;_tmp464;});_LL2B6:;}}_LL285:;_LL286: return({struct _tuple8 _tmp465;_tmp465.f1=
f;_tmp465.f2=f;_tmp465;});_LL278:;}}_LL245:;}}_LL23C:;_LL23D: goto _LL22F;_LL22F:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp467;void*_tmp468;struct _tuple5 _tmp466=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp467=_tmp466.f1;_tmp468=_tmp466.f2;_tmp467=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp467);{union Cyc_CfFlowInfo_FlowInfo_union _tmp469=_tmp467;struct Cyc_Dict_Dict
_tmp46A;_LL2D2: if((_tmp469.BottomFL).tag != 0)goto _LL2D4;_LL2D3: return({struct
_tuple8 _tmp46B;_tmp46B.f1=_tmp467;_tmp46B.f2=_tmp467;_tmp46B;});_LL2D4: if((
_tmp469.ReachableFL).tag != 1)goto _LL2D1;_tmp46A=(_tmp469.ReachableFL).f1;_LL2D5: {
void*_tmp46C=_tmp468;void*_tmp46D;void*_tmp46E;void*_tmp46F;_LL2D7: if((int)
_tmp46C != 0)goto _LL2D9;_LL2D8: return({struct _tuple8 _tmp470;_tmp470.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp471;(_tmp471.BottomFL).tag=0;_tmp471;});
_tmp470.f2=_tmp467;_tmp470;});_LL2D9: if((int)_tmp46C != 2)goto _LL2DB;_LL2DA: goto
_LL2DC;_LL2DB: if((int)_tmp46C != 1)goto _LL2DD;_LL2DC: goto _LL2DE;_LL2DD: if(_tmp46C
<= (void*)3)goto _LL2DF;if(*((int*)_tmp46C)!= 2)goto _LL2DF;_LL2DE: return({struct
_tuple8 _tmp472;_tmp472.f1=_tmp467;_tmp472.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp473;(_tmp473.BottomFL).tag=0;_tmp473;});
_tmp472;});_LL2DF: if(_tmp46C <= (void*)3)goto _LL2E1;if(*((int*)_tmp46C)!= 0)goto
_LL2E1;_tmp46D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp46C)->f1;if((
int)_tmp46D != 0)goto _LL2E1;_LL2E0: goto _LL2E2;_LL2E1: if(_tmp46C <= (void*)3)goto
_LL2E3;if(*((int*)_tmp46C)!= 1)goto _LL2E3;_tmp46E=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp46C)->f1;if((int)_tmp46E != 0)goto _LL2E3;_LL2E2:({void*_tmp474[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp475="expression may not be initialized";_tag_dynforward(
_tmp475,sizeof(char),_get_zero_arr_size(_tmp475,34));}),_tag_dynforward(_tmp474,
sizeof(void*),0));});return({struct _tuple8 _tmp476;_tmp476.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp478;(_tmp478.BottomFL).tag=0;_tmp478;});
_tmp476.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp477;(_tmp477.BottomFL).tag=0;_tmp477;});_tmp476;});_LL2E3: if(_tmp46C <= (void*)
3)goto _LL2E5;if(*((int*)_tmp46C)!= 0)goto _LL2E5;_tmp46F=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp46C)->f1;_LL2E4: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp467,e,
_tmp46F);_LL2E5: if(_tmp46C <= (void*)3)goto _LL2E7;if(*((int*)_tmp46C)!= 1)goto
_LL2E7;_LL2E6: return({struct _tuple8 _tmp479;_tmp479.f1=_tmp467;_tmp479.f2=_tmp467;
_tmp479;});_LL2E7: if(_tmp46C <= (void*)3)goto _LL2E9;if(*((int*)_tmp46C)!= 3)goto
_LL2E9;_LL2E8: return({struct _tuple8 _tmp47A;_tmp47A.f1=_tmp467;_tmp47A.f2=_tmp467;
_tmp47A;});_LL2E9: if(_tmp46C <= (void*)3)goto _LL2D6;if(*((int*)_tmp46C)!= 4)goto
_LL2D6;_LL2EA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp47B=
_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_Core_Impossible_struct _tmp47C;
_tmp47C.tag=Cyc_Core_Impossible;_tmp47C.f1=({const char*_tmp47D="anal_test";
_tag_dynforward(_tmp47D,sizeof(char),_get_zero_arr_size(_tmp47D,10));});_tmp47C;});
_tmp47B;}));_LL2D6:;}_LL2D1:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp47E=flow;struct Cyc_Dict_Dict _tmp47F;
_LL2EC: if((_tmp47E.BottomFL).tag != 0)goto _LL2EE;_LL2ED: return;_LL2EE: if((_tmp47E.ReachableFL).tag
!= 1)goto _LL2EB;_tmp47F=(_tmp47E.ReachableFL).f1;_LL2EF:{struct Cyc_List_List*
_tmp480=env->param_roots;for(0;_tmp480 != 0;_tmp480=_tmp480->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp47F,Cyc_CfFlowInfo_lookup_place(_tmp47F,(struct Cyc_CfFlowInfo_Place*)_tmp480->hd))
!= (void*)2)({void*_tmp481[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp482="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp482,sizeof(char),_get_zero_arr_size(_tmp482,76));}),
_tag_dynforward(_tmp481,sizeof(void*),0));});}}return;_LL2EB:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp483=env->fenv;
for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp485;struct Cyc_Core_Opt*
_tmp486;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Stmt*_tmp488;struct Cyc_Position_Segment*
_tmp489;struct Cyc_Absyn_Switch_clause*_tmp484=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp485=*_tmp484;_tmp486=_tmp485.pat_vars;_tmp487=_tmp485.where_clause;
_tmp488=_tmp485.body;_tmp489=_tmp485.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp483,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp486))->v,(void*)_tmp483->unknown_all,_tmp489);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp487 != 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp48B;union Cyc_CfFlowInfo_FlowInfo_union _tmp48C;struct _tuple8 _tmp48A=Cyc_NewControlFlow_anal_test(
env,clause_inflow,(struct Cyc_Absyn_Exp*)_tmp487);_tmp48B=_tmp48A.f1;_tmp48C=
_tmp48A.f2;inflow=_tmp48C;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
_tmp48B,_tmp488);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp488);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp48D=
clause_outflow;_LL2F1: if((_tmp48D.BottomFL).tag != 0)goto _LL2F3;_LL2F2: goto _LL2F0;
_LL2F3:;_LL2F4: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp48E[0]={};Cyc_Tcutil_terr(
_tmp488->loc,({const char*_tmp48F="switch clause may implicitly fallthru";
_tag_dynforward(_tmp48F,sizeof(char),_get_zero_arr_size(_tmp48F,38));}),
_tag_dynforward(_tmp48E,sizeof(void*),0));});else{({void*_tmp490[0]={};Cyc_Tcutil_warn(
_tmp488->loc,({const char*_tmp491="switch clause may implicitly fallthru";
_tag_dynforward(_tmp491,sizeof(char),_get_zero_arr_size(_tmp491,38));}),
_tag_dynforward(_tmp490,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL2F0;_LL2F0:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp492;(_tmp492.BottomFL).tag=0;_tmp492;});}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp494;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp495;struct _tuple9 _tmp493=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp494=_tmp493.f1;_tmp495=_tmp493.f2;inflow=*_tmp495;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp496=env->fenv;{void*_tmp497=(void*)s->r;struct Cyc_Absyn_Exp*_tmp498;struct
Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Stmt*_tmp49B;
struct Cyc_Absyn_Stmt*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Stmt*
_tmp49E;struct Cyc_Absyn_Stmt*_tmp49F;struct _tuple3 _tmp4A0;struct Cyc_Absyn_Exp*
_tmp4A1;struct Cyc_Absyn_Stmt*_tmp4A2;struct Cyc_Absyn_Stmt*_tmp4A3;struct Cyc_Absyn_Stmt*
_tmp4A4;struct _tuple3 _tmp4A5;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Stmt*
_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;struct _tuple3 _tmp4A9;struct Cyc_Absyn_Exp*
_tmp4AA;struct Cyc_Absyn_Stmt*_tmp4AB;struct _tuple3 _tmp4AC;struct Cyc_Absyn_Exp*
_tmp4AD;struct Cyc_Absyn_Stmt*_tmp4AE;struct Cyc_Absyn_Stmt*_tmp4AF;struct Cyc_Absyn_Stmt*
_tmp4B0;struct Cyc_List_List*_tmp4B1;struct Cyc_Absyn_Switch_clause**_tmp4B2;
struct Cyc_Absyn_Switch_clause*_tmp4B3;struct Cyc_Absyn_Stmt*_tmp4B4;struct Cyc_Absyn_Stmt*
_tmp4B5;struct Cyc_Absyn_Stmt*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_List_List*
_tmp4B8;struct Cyc_Absyn_Stmt*_tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_Absyn_Decl*
_tmp4BB;struct Cyc_Absyn_Stmt*_tmp4BC;struct Cyc_Absyn_Stmt*_tmp4BD;struct Cyc_Absyn_Tvar*
_tmp4BE;struct Cyc_Absyn_Vardecl*_tmp4BF;int _tmp4C0;struct Cyc_Absyn_Exp*_tmp4C1;
struct Cyc_Absyn_Stmt*_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C3;_LL2F6: if((int)_tmp497
!= 0)goto _LL2F8;_LL2F7: return inflow;_LL2F8: if(_tmp497 <= (void*)1)goto _LL31E;if(*((
int*)_tmp497)!= 2)goto _LL2FA;_tmp498=((struct Cyc_Absyn_Return_s_struct*)_tmp497)->f1;
if(_tmp498 != 0)goto _LL2FA;_LL2F9: if(env->noreturn)({void*_tmp4C4[0]={};Cyc_Tcutil_terr(
s->loc,({const char*_tmp4C5="`noreturn' function might return";_tag_dynforward(
_tmp4C5,sizeof(char),_get_zero_arr_size(_tmp4C5,33));}),_tag_dynforward(_tmp4C4,
sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C6;(_tmp4C6.BottomFL).tag=0;_tmp4C6;});_LL2FA: if(*((int*)_tmp497)!= 2)goto
_LL2FC;_tmp499=((struct Cyc_Absyn_Return_s_struct*)_tmp497)->f1;_LL2FB: if(env->noreturn)({
void*_tmp4C7[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4C8="`noreturn' function might return";
_tag_dynforward(_tmp4C8,sizeof(char),_get_zero_arr_size(_tmp4C8,33));}),
_tag_dynforward(_tmp4C7,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CA;void*_tmp4CB;struct _tuple5 _tmp4C9=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp499));_tmp4CA=_tmp4C9.f1;_tmp4CB=
_tmp4C9.f2;_tmp4CA=Cyc_CfFlowInfo_consume_unique_rvals(_tmp499->loc,_tmp4CA);
_tmp4CA=Cyc_NewControlFlow_use_Rval(env,_tmp499->loc,_tmp4CA,_tmp4CB);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp4CA);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CC;(_tmp4CC.BottomFL).tag=0;_tmp4CC;});}_LL2FC: if(*((int*)_tmp497)!= 0)goto
_LL2FE;_tmp49A=((struct Cyc_Absyn_Exp_s_struct*)_tmp497)->f1;_LL2FD: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp49A)).f1;goto _LL2F5;_LL2FE: if(*((int*)_tmp497)!= 1)goto _LL300;
_tmp49B=((struct Cyc_Absyn_Seq_s_struct*)_tmp497)->f1;_tmp49C=((struct Cyc_Absyn_Seq_s_struct*)
_tmp497)->f2;_LL2FF: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp49B),_tmp49C);goto _LL2F5;_LL300: if(*((int*)_tmp497)!= 3)goto
_LL302;_tmp49D=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp497)->f1;_tmp49E=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp497)->f2;_tmp49F=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp497)->f3;_LL301: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4CE;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CF;struct _tuple8 _tmp4CD=Cyc_NewControlFlow_anal_test(env,inflow,_tmp49D);
_tmp4CE=_tmp4CD.f1;_tmp4CF=_tmp4CD.f2;_tmp4CE=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp49D->loc,_tmp4CE);_tmp4CF=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp49D->loc,
_tmp4CF);outflow=Cyc_CfFlowInfo_join_flow(_tmp496,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp4CE,_tmp49E),Cyc_NewControlFlow_anal_stmt(env,_tmp4CF,_tmp49F),1);goto
_LL2F5;}_LL302: if(*((int*)_tmp497)!= 4)goto _LL304;_tmp4A0=((struct Cyc_Absyn_While_s_struct*)
_tmp497)->f1;_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;_tmp4A3=((struct Cyc_Absyn_While_s_struct*)
_tmp497)->f2;_LL303: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp4D1;struct _tuple9
_tmp4D0=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4A2);_tmp4D1=_tmp4D0.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4D2=*_tmp4D1;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4D4;union Cyc_CfFlowInfo_FlowInfo_union _tmp4D5;struct _tuple8 _tmp4D3=Cyc_NewControlFlow_anal_test(
env,_tmp4D2,_tmp4A1);_tmp4D4=_tmp4D3.f1;_tmp4D5=_tmp4D3.f2;_tmp4D4=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4A1->loc,_tmp4D4);_tmp4D5=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4A1->loc,
_tmp4D5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4D6=Cyc_NewControlFlow_anal_stmt(
env,_tmp4D4,_tmp4A3);Cyc_NewControlFlow_update_flow(env,_tmp4A2,_tmp4D6);outflow=
_tmp4D5;goto _LL2F5;}}}_LL304: if(*((int*)_tmp497)!= 13)goto _LL306;_tmp4A4=((
struct Cyc_Absyn_Do_s_struct*)_tmp497)->f1;_tmp4A5=((struct Cyc_Absyn_Do_s_struct*)
_tmp497)->f2;_tmp4A6=_tmp4A5.f1;_tmp4A7=_tmp4A5.f2;_LL305: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp4D7=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4A4);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4D9;struct _tuple9 _tmp4D8=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4D7,
_tmp4A7);_tmp4D9=_tmp4D8.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4DA=*_tmp4D9;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4DC;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4DD;struct _tuple8 _tmp4DB=Cyc_NewControlFlow_anal_test(env,_tmp4DA,_tmp4A6);
_tmp4DC=_tmp4DB.f1;_tmp4DD=_tmp4DB.f2;Cyc_NewControlFlow_update_flow(env,_tmp4A4,
_tmp4DC);outflow=_tmp4DD;goto _LL2F5;}}_LL306: if(*((int*)_tmp497)!= 8)goto _LL308;
_tmp4A8=((struct Cyc_Absyn_For_s_struct*)_tmp497)->f1;_tmp4A9=((struct Cyc_Absyn_For_s_struct*)
_tmp497)->f2;_tmp4AA=_tmp4A9.f1;_tmp4AB=_tmp4A9.f2;_tmp4AC=((struct Cyc_Absyn_For_s_struct*)
_tmp497)->f3;_tmp4AD=_tmp4AC.f1;_tmp4AE=_tmp4AC.f2;_tmp4AF=((struct Cyc_Absyn_For_s_struct*)
_tmp497)->f4;_LL307: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4DE=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4A8)).f1;_tmp4DE=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4A8->loc,
_tmp4DE);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp4E0;struct _tuple9 _tmp4DF=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp4DE,_tmp4AB);_tmp4E0=_tmp4DF.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E1=*_tmp4E0;union Cyc_CfFlowInfo_FlowInfo_union _tmp4E3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E4;struct _tuple8 _tmp4E2=Cyc_NewControlFlow_anal_test(env,_tmp4E1,_tmp4AA);
_tmp4E3=_tmp4E2.f1;_tmp4E4=_tmp4E2.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E5=
Cyc_NewControlFlow_anal_stmt(env,_tmp4E3,_tmp4AF);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4E7;struct _tuple9 _tmp4E6=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4E5,
_tmp4AE);_tmp4E7=_tmp4E6.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E8=*_tmp4E7;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4E9=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp4E8,_tmp4AD)).f1;_tmp4E9=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4AD->loc,
_tmp4E9);Cyc_NewControlFlow_update_flow(env,_tmp4AB,_tmp4E9);outflow=_tmp4E4;
goto _LL2F5;}}}}}_LL308: if(*((int*)_tmp497)!= 5)goto _LL30A;_tmp4B0=((struct Cyc_Absyn_Break_s_struct*)
_tmp497)->f1;if(_tmp4B0 != 0)goto _LL30A;_LL309: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4EA;(_tmp4EA.BottomFL).tag=0;_tmp4EA;});
_LL30A: if(*((int*)_tmp497)!= 10)goto _LL30C;_tmp4B1=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp497)->f1;_tmp4B2=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp497)->f2;if(
_tmp4B2 == 0)goto _LL30C;_tmp4B3=*_tmp4B2;_LL30B: {struct _RegionHandle _tmp4EB=
_new_region("temp");struct _RegionHandle*temp=& _tmp4EB;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp4ED;struct Cyc_List_List*_tmp4EE;struct _tuple11
_tmp4EC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4B1,0);
_tmp4ED=_tmp4EC.f1;_tmp4EE=_tmp4EC.f2;for(0;_tmp4EE != 0;(_tmp4EE=_tmp4EE->tl,
_tmp4B1=_tmp4B1->tl)){_tmp4ED=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4B1))->hd)->loc,_tmp4ED,(void*)_tmp4EE->hd);}
_tmp4ED=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp4ED);_tmp4ED=Cyc_NewControlFlow_add_vars(
_tmp496,_tmp4ED,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4B3->pat_vars))->v,(
void*)_tmp496->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4B3->body,_tmp4ED);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4F0=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4EF;(_tmp4EF.BottomFL).tag=0;_tmp4EF;});
_npop_handler(0);return _tmp4F0;}};_pop_region(temp);}_LL30C: if(*((int*)_tmp497)
!= 5)goto _LL30E;_tmp4B4=((struct Cyc_Absyn_Break_s_struct*)_tmp497)->f1;_LL30D:
_tmp4B5=_tmp4B4;goto _LL30F;_LL30E: if(*((int*)_tmp497)!= 6)goto _LL310;_tmp4B5=((
struct Cyc_Absyn_Continue_s_struct*)_tmp497)->f1;_LL30F: _tmp4B6=_tmp4B5;goto
_LL311;_LL310: if(*((int*)_tmp497)!= 7)goto _LL312;_tmp4B6=((struct Cyc_Absyn_Goto_s_struct*)
_tmp497)->f2;_LL311: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp4F1=
_tmp494->encloser;struct Cyc_Absyn_Stmt*_tmp4F2=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4B6)))->encloser;while(_tmp4F2 != _tmp4F1){
struct Cyc_Absyn_Stmt*_tmp4F3=(Cyc_NewControlFlow_get_stmt_annot(_tmp4F1))->encloser;
if(_tmp4F3 == _tmp4F1){({void*_tmp4F4[0]={};Cyc_Tcutil_terr(s->loc,({const char*
_tmp4F5="goto enters local scope or exception handler";_tag_dynforward(_tmp4F5,
sizeof(char),_get_zero_arr_size(_tmp4F5,45));}),_tag_dynforward(_tmp4F4,sizeof(
void*),0));});break;}_tmp4F1=_tmp4F3;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp4B6),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4F6;(_tmp4F6.BottomFL).tag=0;_tmp4F6;});
_LL312: if(*((int*)_tmp497)!= 9)goto _LL314;_tmp4B7=((struct Cyc_Absyn_Switch_s_struct*)
_tmp497)->f1;_tmp4B8=((struct Cyc_Absyn_Switch_s_struct*)_tmp497)->f2;_LL313: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp4F8;void*_tmp4F9;struct _tuple5 _tmp4F7=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4B7);_tmp4F8=_tmp4F7.f1;_tmp4F9=_tmp4F7.f2;_tmp4F8=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4B7->loc,_tmp4F8);_tmp4F8=Cyc_NewControlFlow_use_Rval(env,_tmp4B7->loc,
_tmp4F8,_tmp4F9);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp4F8,_tmp4B8);goto
_LL2F5;}_LL314: if(*((int*)_tmp497)!= 14)goto _LL316;_tmp4B9=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp497)->f1;_tmp4BA=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp497)->f2;_LL315: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4B9);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4BA);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4FA=scs_outflow;
_LL321: if((_tmp4FA.BottomFL).tag != 0)goto _LL323;_LL322: goto _LL320;_LL323:;_LL324:({
void*_tmp4FB[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4FC="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp4FC,sizeof(char),_get_zero_arr_size(_tmp4FC,42));}),
_tag_dynforward(_tmp4FB,sizeof(void*),0));});_LL320:;}outflow=s1_outflow;goto
_LL2F5;}}}_LL316: if(*((int*)_tmp497)!= 11)goto _LL318;_tmp4BB=((struct Cyc_Absyn_Decl_s_struct*)
_tmp497)->f1;_tmp4BC=((struct Cyc_Absyn_Decl_s_struct*)_tmp497)->f2;_LL317:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4BB),_tmp4BC);goto _LL2F5;_LL318: if(*((int*)_tmp497)!= 12)goto _LL31A;_tmp4BD=((
struct Cyc_Absyn_Label_s_struct*)_tmp497)->f2;_LL319: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4BD);goto _LL2F5;_LL31A: if(*((int*)_tmp497)!= 15)goto _LL31C;
_tmp4BE=((struct Cyc_Absyn_Region_s_struct*)_tmp497)->f1;_tmp4BF=((struct Cyc_Absyn_Region_s_struct*)
_tmp497)->f2;_tmp4C0=((struct Cyc_Absyn_Region_s_struct*)_tmp497)->f3;_tmp4C1=((
struct Cyc_Absyn_Region_s_struct*)_tmp497)->f4;_tmp4C2=((struct Cyc_Absyn_Region_s_struct*)
_tmp497)->f5;_LL31B: if(_tmp4C1 != 0){struct Cyc_Absyn_Exp*_tmp4FD=(struct Cyc_Absyn_Exp*)
_tmp4C1;union Cyc_CfFlowInfo_FlowInfo_union _tmp4FF;void*_tmp500;struct _tuple5
_tmp4FE=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4FD);_tmp4FF=_tmp4FE.f1;
_tmp500=_tmp4FE.f2;_tmp4FF=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4FD->loc,
_tmp4FF);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp4FD->loc,_tmp4FF,_tmp500);}{
struct Cyc_List_List _tmp501=({struct Cyc_List_List _tmp502;_tmp502.hd=_tmp4BF;
_tmp502.tl=0;_tmp502;});inflow=Cyc_NewControlFlow_add_vars(_tmp496,inflow,(
struct Cyc_List_List*)& _tmp501,(void*)_tmp496->unknown_all,_tmp4C2->loc);outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4C2);goto _LL2F5;}_LL31C: if(*((int*)
_tmp497)!= 16)goto _LL31E;_tmp4C3=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp497)->f1;
_LL31D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp504;void*_tmp505;struct _tuple5
_tmp503=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4C3);_tmp504=_tmp503.f1;
_tmp505=_tmp503.f2;_tmp504=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4C3->loc,
_tmp504);{union Cyc_CfFlowInfo_FlowInfo_union _tmp506=Cyc_NewControlFlow_use_Rval(
env,_tmp4C3->loc,_tmp504,_tmp505);{void*_tmp507=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C3->topt))->v);void*_tmp508;_LL326: if(
_tmp507 <= (void*)4)goto _LL328;if(*((int*)_tmp507)!= 15)goto _LL328;_tmp508=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp507)->f1;_LL327: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp496,_tmp504,_tmp508);goto _LL325;_LL328:;_LL329:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509[0]=({
struct Cyc_Core_Impossible_struct _tmp50A;_tmp50A.tag=Cyc_Core_Impossible;_tmp50A.f1=({
const char*_tmp50B="anal_stmt -- reset_region";_tag_dynforward(_tmp50B,sizeof(
char),_get_zero_arr_size(_tmp50B,26));});_tmp50A;});_tmp509;}));_LL325:;}goto
_LL2F5;}}_LL31E:;_LL31F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C[0]=({struct Cyc_Core_Impossible_struct
_tmp50D;_tmp50D.tag=Cyc_Core_Impossible;_tmp50D.f1=({const char*_tmp50E="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp50E,sizeof(char),_get_zero_arr_size(_tmp50E,56));});_tmp50D;});
_tmp50C;}));_LL2F5:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp50F=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp510;_LL32B: if((_tmp50F.ReachableFL).tag != 1)goto _LL32D;_tmp510=(_tmp50F.ReachableFL).f3;
_LL32C: goto _LL32A;_LL32D:;_LL32E: goto _LL32A;_LL32A:;}return outflow;}}static void
Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp511=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp512;struct Cyc_Core_Opt*_tmp513;struct Cyc_Core_Opt _tmp514;struct Cyc_List_List*
_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_List_List*_tmp517;struct Cyc_Absyn_Fndecl*
_tmp518;_LL330: if(_tmp511 <= (void*)2)goto _LL338;if(*((int*)_tmp511)!= 0)goto
_LL332;_tmp512=((struct Cyc_Absyn_Var_d_struct*)_tmp511)->f1;_LL331: {struct Cyc_List_List
_tmp519=({struct Cyc_List_List _tmp528;_tmp528.hd=_tmp512;_tmp528.tl=0;_tmp528;});
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp519,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp51A=
_tmp512->initializer;if(_tmp51A == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp51C;void*_tmp51D;struct _tuple5 _tmp51B=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp51A);_tmp51C=_tmp51B.f1;_tmp51D=_tmp51B.f2;
_tmp51C=Cyc_CfFlowInfo_consume_unique_rvals(_tmp51A->loc,_tmp51C);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp51E=_tmp51C;struct Cyc_Dict_Dict _tmp51F;struct Cyc_List_List*_tmp520;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp521;_LL33B: if((_tmp51E.BottomFL).tag != 0)goto _LL33D;_LL33C: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp522;(_tmp522.BottomFL).tag=0;_tmp522;});
_LL33D: if((_tmp51E.ReachableFL).tag != 1)goto _LL33A;_tmp51F=(_tmp51E.ReachableFL).f1;
_tmp520=(_tmp51E.ReachableFL).f2;_tmp521=(_tmp51E.ReachableFL).f3;_LL33E: _tmp51F=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp51F,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp523=_region_malloc(env->r,sizeof(*_tmp523));
_tmp523->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp524=
_region_malloc(env->r,sizeof(*_tmp524));_tmp524[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp525;_tmp525.tag=0;_tmp525.f1=_tmp512;_tmp525;});_tmp524;}));_tmp523->fields=
0;_tmp523;}),_tmp51D);_tmp520=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp520,
_tmp512,(struct Cyc_Absyn_Exp*)_check_null(_tmp512->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp526=({union Cyc_CfFlowInfo_FlowInfo_union _tmp527;(_tmp527.ReachableFL).tag=1;(
_tmp527.ReachableFL).f1=_tmp51F;(_tmp527.ReachableFL).f2=_tmp520;(_tmp527.ReachableFL).f3=
_tmp521;_tmp527;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp526);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp526;}_LL33A:;}}}}_LL332:
if(*((int*)_tmp511)!= 2)goto _LL334;_tmp513=((struct Cyc_Absyn_Let_d_struct*)
_tmp511)->f2;if(_tmp513 == 0)goto _LL334;_tmp514=*_tmp513;_tmp515=(struct Cyc_List_List*)
_tmp514.v;_tmp516=((struct Cyc_Absyn_Let_d_struct*)_tmp511)->f3;_LL333: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp52A;void*_tmp52B;struct _tuple5 _tmp529=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp516);_tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;_tmp52A=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp516->loc,_tmp52A);_tmp52A=Cyc_NewControlFlow_use_Rval(env,_tmp516->loc,
_tmp52A,_tmp52B);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp52A,_tmp515,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL334: if(*((int*)_tmp511)!= 3)goto
_LL336;_tmp517=((struct Cyc_Absyn_Letv_d_struct*)_tmp511)->f1;_LL335: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp517,(void*)(env->fenv)->unknown_none,decl->loc);_LL336: if(*((
int*)_tmp511)!= 1)goto _LL338;_tmp518=((struct Cyc_Absyn_Fn_d_struct*)_tmp511)->f1;
_LL337: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp518);{void*
_tmp52C=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp518->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp52D=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp518->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp52E=
_region_malloc(env->r,sizeof(*_tmp52E));_tmp52E->hd=_tmp52D;_tmp52E->tl=0;
_tmp52E;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL338:;_LL339:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp52F=_cycalloc(sizeof(*
_tmp52F));_tmp52F[0]=({struct Cyc_Core_Impossible_struct _tmp530;_tmp530.tag=Cyc_Core_Impossible;
_tmp530.f1=({const char*_tmp531="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp531,sizeof(char),_get_zero_arr_size(_tmp531,35));});_tmp530;});
_tmp52F;}));_LL32F:;}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp532=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp532;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp533;(_tmp533.ReachableFL).tag=1;(_tmp533.ReachableFL).f1=
fenv->mt_flowdict;(_tmp533.ReachableFL).f2=0;(_tmp533.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp534;_tmp534.consumed=fenv->mt_place_set;_tmp534.may_consume=
0;_tmp534;});_tmp533;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp535=fenv->r;struct Cyc_Position_Segment*_tmp536=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp536);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp537=inflow;struct Cyc_Dict_Dict
_tmp538;struct Cyc_List_List*_tmp539;struct Cyc_CfFlowInfo_ConsumeInfo _tmp53A;
_LL340: if((_tmp537.BottomFL).tag != 0)goto _LL342;_LL341:({void*_tmp53B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp53C="check_fun";_tag_dynforward(_tmp53C,sizeof(char),
_get_zero_arr_size(_tmp53C,10));}),_tag_dynforward(_tmp53B,sizeof(void*),0));});
_LL342: if((_tmp537.ReachableFL).tag != 1)goto _LL33F;_tmp538=(_tmp537.ReachableFL).f1;
_tmp539=(_tmp537.ReachableFL).f2;_tmp53A=(_tmp537.ReachableFL).f3;_LL343: {
struct Cyc_List_List*atts;{void*_tmp53D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp53E;struct Cyc_List_List*
_tmp53F;_LL345: if(_tmp53D <= (void*)4)goto _LL347;if(*((int*)_tmp53D)!= 8)goto
_LL347;_tmp53E=((struct Cyc_Absyn_FnType_struct*)_tmp53D)->f1;_tmp53F=_tmp53E.attributes;
_LL346: atts=_tmp53F;goto _LL344;_LL347:;_LL348:({void*_tmp540[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp541="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp541,sizeof(char),_get_zero_arr_size(_tmp541,50));}),_tag_dynforward(_tmp540,
sizeof(void*),0));});_LL344:;}for(0;atts != 0;atts=atts->tl){void*_tmp542=(void*)
atts->hd;int _tmp543;_LL34A: if(_tmp542 <= (void*)17)goto _LL34C;if(*((int*)_tmp542)
!= 4)goto _LL34C;_tmp543=((struct Cyc_Absyn_Initializes_att_struct*)_tmp542)->f1;
_LL34B: {unsigned int j=(unsigned int)_tmp543;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp544[0]={};Cyc_Tcutil_terr(_tmp536,({const char*_tmp545="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp545,sizeof(char),_get_zero_arr_size(_tmp545,51));}),
_tag_dynforward(_tmp544,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp546=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp547=Cyc_Tcutil_compress((void*)_tmp546->type);struct Cyc_Absyn_PtrInfo
_tmp548;void*_tmp549;struct Cyc_Absyn_PtrAtts _tmp54A;struct Cyc_Absyn_Conref*
_tmp54B;struct Cyc_Absyn_Conref*_tmp54C;struct Cyc_Absyn_Conref*_tmp54D;_LL34F: if(
_tmp547 <= (void*)4)goto _LL351;if(*((int*)_tmp547)!= 4)goto _LL351;_tmp548=((
struct Cyc_Absyn_PointerType_struct*)_tmp547)->f1;_tmp549=(void*)_tmp548.elt_typ;
_tmp54A=_tmp548.ptr_atts;_tmp54B=_tmp54A.nullable;_tmp54C=_tmp54A.bounds;_tmp54D=
_tmp54A.zero_term;_LL350: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp54B))({void*_tmp54E[0]={};Cyc_Tcutil_terr(_tmp536,({const char*_tmp54F="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp54F,sizeof(char),_get_zero_arr_size(_tmp54F,55));}),
_tag_dynforward(_tmp54E,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp54C))({
void*_tmp550[0]={};Cyc_Tcutil_terr(_tmp536,({const char*_tmp551="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp551,sizeof(char),_get_zero_arr_size(_tmp551,57));}),
_tag_dynforward(_tmp550,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp54D))({void*_tmp552[0]={};Cyc_Tcutil_terr(_tmp536,({
const char*_tmp553="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp553,sizeof(char),_get_zero_arr_size(_tmp553,77));}),
_tag_dynforward(_tmp552,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp554=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp55C=_region_malloc(_tmp535,
sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp55D;
_tmp55D.tag=2;_tmp55D.f1=(int)j;_tmp55D.f2=(void*)_tmp549;_tmp55D;});_tmp55C;});
struct Cyc_CfFlowInfo_Place*_tmp555=({struct Cyc_CfFlowInfo_Place*_tmp55B=
_region_malloc(_tmp535,sizeof(*_tmp55B));_tmp55B->root=(void*)((void*)_tmp554);
_tmp55B->fields=0;_tmp55B;});_tmp538=Cyc_Dict_insert(_tmp538,(void*)_tmp554,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp549,(void*)fenv->esc_none));_tmp538=Cyc_Dict_insert(_tmp538,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp556=_region_malloc(_tmp535,sizeof(*
_tmp556));_tmp556[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp557;_tmp557.tag=0;
_tmp557.f1=_tmp546;_tmp557;});_tmp556;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp558=_region_malloc(_tmp535,sizeof(*_tmp558));_tmp558[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp559;_tmp559.tag=2;_tmp559.f1=_tmp555;_tmp559;});_tmp558;}));param_roots=({
struct Cyc_List_List*_tmp55A=_region_malloc(_tmp535,sizeof(*_tmp55A));_tmp55A->hd=
_tmp555;_tmp55A->tl=param_roots;_tmp55A;});goto _LL34E;}_LL351:;_LL352:({void*
_tmp55E[0]={};Cyc_Tcutil_terr(_tmp536,({const char*_tmp55F="initializes attribute on non-pointer";
_tag_dynforward(_tmp55F,sizeof(char),_get_zero_arr_size(_tmp55F,37));}),
_tag_dynforward(_tmp55E,sizeof(void*),0));});_LL34E:;}goto _LL349;}}_LL34C:;
_LL34D: goto _LL349;_LL349:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp560;(_tmp560.ReachableFL).tag=1;(_tmp560.ReachableFL).f1=_tmp538;(_tmp560.ReachableFL).f2=
_tmp539;(_tmp560.ReachableFL).f3=_tmp53A;_tmp560;});}_LL33F:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(
struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),
int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp535,33,(int(*)(
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp569=_region_malloc(_tmp535,sizeof(*_tmp569));_tmp569->r=_tmp535;_tmp569->fenv=
fenv;_tmp569->iterate_again=1;_tmp569->iteration_num=0;_tmp569->in_try=0;_tmp569->tryflow=
inflow;_tmp569->all_changed=0;_tmp569->noreturn=noreturn;_tmp569->param_roots=
param_roots;_tmp569->flow_table=flow_table;_tmp569;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp561=outflow;_LL354: if((_tmp561.BottomFL).tag != 0)goto _LL356;_LL355: goto
_LL353;_LL356:;_LL357: Cyc_NewControlFlow_check_init_params(_tmp536,env,outflow);
if(noreturn)({void*_tmp562[0]={};Cyc_Tcutil_terr(_tmp536,({const char*_tmp563="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp563,sizeof(char),_get_zero_arr_size(_tmp563,46));}),
_tag_dynforward(_tmp562,sizeof(void*),0));});else{void*_tmp564=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL359: if((int)_tmp564 != 0)goto _LL35B;_LL35A: goto _LL358;
_LL35B: if(_tmp564 <= (void*)4)goto _LL35D;if(*((int*)_tmp564)!= 6)goto _LL35D;
_LL35C: goto _LL35E;_LL35D: if((int)_tmp564 != 1)goto _LL35F;_LL35E: goto _LL360;_LL35F:
if(_tmp564 <= (void*)4)goto _LL361;if(*((int*)_tmp564)!= 5)goto _LL361;_LL360:({
void*_tmp565[0]={};Cyc_Tcutil_warn(_tmp536,({const char*_tmp566="function may not return a value";
_tag_dynforward(_tmp566,sizeof(char),_get_zero_arr_size(_tmp566,32));}),
_tag_dynforward(_tmp565,sizeof(void*),0));});goto _LL358;_LL361:;_LL362:({void*
_tmp567[0]={};Cyc_Tcutil_terr(_tmp536,({const char*_tmp568="function may not return a value";
_tag_dynforward(_tmp568,sizeof(char),_get_zero_arr_size(_tmp568,32));}),
_tag_dynforward(_tmp567,sizeof(void*),0));});goto _LL358;_LL358:;}goto _LL353;
_LL353:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;
ds=ds->tl){void*_tmp56A=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*
_tmp56B;struct Cyc_List_List*_tmp56C;struct Cyc_List_List*_tmp56D;struct Cyc_List_List*
_tmp56E;_LL364: if(_tmp56A <= (void*)2)goto _LL36E;if(*((int*)_tmp56A)!= 1)goto
_LL366;_tmp56B=((struct Cyc_Absyn_Fn_d_struct*)_tmp56A)->f1;_LL365: Cyc_NewControlFlow_check_fun(
_tmp56B);goto _LL363;_LL366: if(*((int*)_tmp56A)!= 10)goto _LL368;_tmp56C=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp56A)->f1;_LL367: _tmp56D=_tmp56C;goto _LL369;
_LL368: if(*((int*)_tmp56A)!= 9)goto _LL36A;_tmp56D=((struct Cyc_Absyn_Using_d_struct*)
_tmp56A)->f2;_LL369: _tmp56E=_tmp56D;goto _LL36B;_LL36A: if(*((int*)_tmp56A)!= 8)
goto _LL36C;_tmp56E=((struct Cyc_Absyn_Namespace_d_struct*)_tmp56A)->f2;_LL36B: Cyc_NewControlFlow_cf_check(
_tmp56E);goto _LL363;_LL36C: if(*((int*)_tmp56A)!= 11)goto _LL36E;_LL36D: goto _LL363;
_LL36E:;_LL36F: goto _LL363;_LL363:;}}
