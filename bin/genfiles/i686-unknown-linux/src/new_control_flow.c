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
return relns;_LL98:;}struct _tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD3=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo
cinfo;{union Cyc_CfFlowInfo_FlowInfo_union _tmpD4=inflow;struct Cyc_Dict_Dict _tmpD5;
struct Cyc_List_List*_tmpD6;struct Cyc_CfFlowInfo_ConsumeInfo _tmpD7;_LLB4: if((
_tmpD4.BottomFL).tag != 0)goto _LLB6;_LLB5: return({struct _tuple5 _tmpD8;_tmpD8.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpD9;(
_tmpD9.BottomFL).tag=0;_tmpD9;});_tmpD8.f2=(void*)_tmpD3->unknown_all;_tmpD8;});
_LLB6: if((_tmpD4.ReachableFL).tag != 1)goto _LLB3;_tmpD5=(_tmpD4.ReachableFL).f1;
_tmpD6=(_tmpD4.ReachableFL).f2;_tmpD7=(_tmpD4.ReachableFL).f3;_LLB7: d=_tmpD5;
relns=_tmpD6;cinfo=_tmpD7;_LLB3:;}{void*_tmpDA=(void*)e->r;struct Cyc_Absyn_Exp*
_tmpDB;void*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct
Cyc_Absyn_Exp*_tmpDF;union Cyc_Absyn_Cnst_union _tmpE0;union Cyc_Absyn_Cnst_union
_tmpE1;int _tmpE2;union Cyc_Absyn_Cnst_union _tmpE3;void*_tmpE4;struct Cyc_List_List*
_tmpE5;void*_tmpE6;void*_tmpE7;struct Cyc_Absyn_Vardecl*_tmpE8;void*_tmpE9;struct
Cyc_Absyn_Vardecl*_tmpEA;void*_tmpEB;struct Cyc_Absyn_Vardecl*_tmpEC;void*_tmpED;
struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;
struct Cyc_Core_Opt*_tmpF1;struct Cyc_Core_Opt _tmpF2;struct Cyc_Absyn_Exp*_tmpF3;
struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Core_Opt*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;
struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;
struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_MallocInfo
_tmpFC;int _tmpFD;struct Cyc_Absyn_Exp*_tmpFE;void**_tmpFF;struct Cyc_Absyn_Exp*
_tmp100;int _tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp103;
struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*
_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct
_dynforward_ptr*_tmp109;struct Cyc_Absyn_Exp*_tmp10A;struct _dynforward_ptr*
_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*
_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*
_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*
_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Tuniondecl*_tmp116;struct Cyc_List_List*
_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_Absyn_Aggrdecl*
_tmp11A;struct Cyc_Absyn_Aggrdecl _tmp11B;struct Cyc_Absyn_AggrdeclImpl*_tmp11C;
struct Cyc_Absyn_AggrdeclImpl _tmp11D;struct Cyc_List_List*_tmp11E;struct Cyc_List_List*
_tmp11F;struct Cyc_Absyn_Vardecl*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*
_tmp122;int _tmp123;struct Cyc_Absyn_Stmt*_tmp124;void*_tmp125;_LLB9: if(*((int*)
_tmpDA)!= 15)goto _LLBB;_tmpDB=((struct Cyc_Absyn_Cast_e_struct*)_tmpDA)->f2;
_tmpDC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpDA)->f4;if((int)_tmpDC != 2)
goto _LLBB;_LLBA: {union Cyc_CfFlowInfo_FlowInfo_union _tmp127;void*_tmp128;struct
_tuple5 _tmp126=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpDB);_tmp127=_tmp126.f1;
_tmp128=_tmp126.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp12A;void*_tmp12B;
struct _tuple5 _tmp129=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp127,_tmpDB,
_tmp128);_tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;return({struct _tuple5 _tmp12C;
_tmp12C.f1=_tmp12A;_tmp12C.f2=_tmp128;_tmp12C;});}}_LLBB: if(*((int*)_tmpDA)!= 15)
goto _LLBD;_tmpDD=((struct Cyc_Absyn_Cast_e_struct*)_tmpDA)->f2;_LLBC: _tmpDE=
_tmpDD;goto _LLBE;_LLBD: if(*((int*)_tmpDA)!= 13)goto _LLBF;_tmpDE=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpDA)->f1;_LLBE: _tmpDF=_tmpDE;goto _LLC0;_LLBF: if(*((int*)_tmpDA)!= 14)goto
_LLC1;_tmpDF=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpDA)->f1;_LLC0: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpDF);_LLC1: if(*((int*)_tmpDA)!= 0)goto _LLC3;_tmpE0=((struct Cyc_Absyn_Const_e_struct*)
_tmpDA)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpDA)->f1).Null_c).tag != 6)
goto _LLC3;_LLC2: goto _LLC4;_LLC3: if(*((int*)_tmpDA)!= 0)goto _LLC5;_tmpE1=((struct
Cyc_Absyn_Const_e_struct*)_tmpDA)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpDA)->f1).Int_c).tag != 2)goto _LLC5;_tmpE2=(_tmpE1.Int_c).f2;if(_tmpE2 != 0)
goto _LLC5;_LLC4: return({struct _tuple5 _tmp12D;_tmp12D.f1=inflow;_tmp12D.f2=(void*)
0;_tmp12D;});_LLC5: if(*((int*)_tmpDA)!= 0)goto _LLC7;_tmpE3=((struct Cyc_Absyn_Const_e_struct*)
_tmpDA)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpDA)->f1).Int_c).tag != 2)
goto _LLC7;_LLC6: goto _LLC8;_LLC7: if(*((int*)_tmpDA)!= 1)goto _LLC9;_tmpE4=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpDA)->f2;if(_tmpE4 <= (void*)1)goto _LLC9;if(*((
int*)_tmpE4)!= 1)goto _LLC9;_LLC8: return({struct _tuple5 _tmp12E;_tmp12E.f1=inflow;
_tmp12E.f2=(void*)1;_tmp12E;});_LLC9: if(*((int*)_tmpDA)!= 32)goto _LLCB;_tmpE5=((
struct Cyc_Absyn_Tunion_e_struct*)_tmpDA)->f1;if(_tmpE5 != 0)goto _LLCB;_LLCA: goto
_LLCC;_LLCB: if(*((int*)_tmpDA)!= 0)goto _LLCD;_LLCC: goto _LLCE;_LLCD: if(*((int*)
_tmpDA)!= 19)goto _LLCF;_LLCE: goto _LLD0;_LLCF: if(*((int*)_tmpDA)!= 18)goto _LLD1;
_LLD0: goto _LLD2;_LLD1: if(*((int*)_tmpDA)!= 20)goto _LLD3;_LLD2: goto _LLD4;_LLD3:
if(*((int*)_tmpDA)!= 21)goto _LLD5;_LLD4: goto _LLD6;_LLD5: if(*((int*)_tmpDA)!= 34)
goto _LLD7;_LLD6: goto _LLD8;_LLD7: if(*((int*)_tmpDA)!= 33)goto _LLD9;_LLD8: return({
struct _tuple5 _tmp12F;_tmp12F.f1=inflow;_tmp12F.f2=(void*)_tmpD3->unknown_all;
_tmp12F;});_LLD9: if(*((int*)_tmpDA)!= 1)goto _LLDB;_tmpE6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpDA)->f2;if(_tmpE6 <= (void*)1)goto _LLDB;if(*((int*)_tmpE6)!= 0)goto _LLDB;
_LLDA: return({struct _tuple5 _tmp130;_tmp130.f1=inflow;_tmp130.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpD3,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpD3->unknown_all);
_tmp130;});_LLDB: if(*((int*)_tmpDA)!= 1)goto _LLDD;_tmpE7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpDA)->f2;if(_tmpE7 <= (void*)1)goto _LLDD;if(*((int*)_tmpE7)!= 2)goto _LLDD;
_tmpE8=((struct Cyc_Absyn_Param_b_struct*)_tmpE7)->f1;_LLDC: _tmpEA=_tmpE8;goto
_LLDE;_LLDD: if(*((int*)_tmpDA)!= 1)goto _LLDF;_tmpE9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpDA)->f2;if(_tmpE9 <= (void*)1)goto _LLDF;if(*((int*)_tmpE9)!= 3)goto _LLDF;
_tmpEA=((struct Cyc_Absyn_Local_b_struct*)_tmpE9)->f1;_LLDE: _tmpEC=_tmpEA;goto
_LLE0;_LLDF: if(*((int*)_tmpDA)!= 1)goto _LLE1;_tmpEB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpDA)->f2;if(_tmpEB <= (void*)1)goto _LLE1;if(*((int*)_tmpEB)!= 4)goto _LLE1;
_tmpEC=((struct Cyc_Absyn_Pat_b_struct*)_tmpEB)->f1;_LLE0: if(Cyc_Tcutil_is_unique_pointer((
void*)_tmpEC->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp131=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp136=_region_malloc(_tmpD3->r,sizeof(*_tmp136));_tmp136[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp137;_tmp137.tag=0;_tmp137.f1=_tmpEC;_tmp137;});_tmp136;});struct Cyc_CfFlowInfo_Place*
_tmp132=({struct Cyc_CfFlowInfo_Place*_tmp135=_region_malloc(_tmpD3->r,sizeof(*
_tmp135));_tmp135->root=(void*)((void*)_tmp131);_tmp135->fields=0;_tmp135;});
cinfo.may_consume=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,
struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpD3->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp133=_region_malloc(_tmpD3->r,sizeof(*_tmp133));_tmp133->hd=
_tmp132;_tmp133->tl=0;_tmp133;}),cinfo.may_consume);inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp134;(_tmp134.ReachableFL).tag=1;(_tmp134.ReachableFL).f1=
d;(_tmp134.ReachableFL).f2=relns;(_tmp134.ReachableFL).f3=cinfo;_tmp134;});}
return({struct _tuple5 _tmp138;_tmp138.f1=inflow;_tmp138.f2=Cyc_Dict_lookup(d,(
void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp139=_region_malloc(env->r,
sizeof(*_tmp139));_tmp139[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp13A;
_tmp13A.tag=0;_tmp13A.f1=_tmpEC;_tmp13A;});_tmp139;}));_tmp138;});_LLE1: if(*((
int*)_tmpDA)!= 3)goto _LLE3;_tmpED=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpDA)->f1;_tmpEE=((struct Cyc_Absyn_Primop_e_struct*)_tmpDA)->f2;_LLE2: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp13C;void*_tmp13D;struct _tuple5 _tmp13B=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpEE,0);_tmp13C=_tmp13B.f1;_tmp13D=_tmp13B.f2;{void*_tmp13E=_tmpED;
_LL120: if((int)_tmp13E != 0)goto _LL122;_LL121: goto _LL123;_LL122: if((int)_tmp13E != 
2)goto _LL124;_LL123: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpEE))->hd)->loc,_tmp13C);goto _LL11F;_LL124:;
_LL125: _tmp13C=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpEE))->hd)->loc,_tmp13C);goto _LL11F;_LL11F:;}
return({struct _tuple5 _tmp13F;_tmp13F.f1=_tmp13C;_tmp13F.f2=_tmp13D;_tmp13F;});}
_LLE3: if(*((int*)_tmpDA)!= 5)goto _LLE5;_tmpEF=((struct Cyc_Absyn_Increment_e_struct*)
_tmpDA)->f1;_LLE4: {struct Cyc_List_List _tmp140=({struct Cyc_List_List _tmp14F;
_tmp14F.hd=_tmpEF;_tmp14F.tl=0;_tmp14F;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp142;struct _tuple5 _tmp141=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp140,0);_tmp142=_tmp141.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmpEF->loc,_tmp142);{union Cyc_CfFlowInfo_AbsLVal_union _tmp144;struct _tuple7
_tmp143=Cyc_NewControlFlow_anal_Lexp(env,_tmp142,_tmpEF);_tmp144=_tmp143.f2;{
struct _tuple12 _tmp146=({struct _tuple12 _tmp145;_tmp145.f1=_tmp144;_tmp145.f2=
_tmp142;_tmp145;});union Cyc_CfFlowInfo_AbsLVal_union _tmp147;struct Cyc_CfFlowInfo_Place*
_tmp148;union Cyc_CfFlowInfo_FlowInfo_union _tmp149;struct Cyc_Dict_Dict _tmp14A;
struct Cyc_List_List*_tmp14B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp14C;_LL127:
_tmp147=_tmp146.f1;if(((_tmp146.f1).PlaceL).tag != 0)goto _LL129;_tmp148=(_tmp147.PlaceL).f1;
_tmp149=_tmp146.f2;if(((_tmp146.f2).ReachableFL).tag != 1)goto _LL129;_tmp14A=(
_tmp149.ReachableFL).f1;_tmp14B=(_tmp149.ReachableFL).f2;_tmp14C=(_tmp149.ReachableFL).f3;
_LL128: _tmp14B=Cyc_CfFlowInfo_reln_kill_exp(_tmpD3->r,_tmp14B,_tmpEF);_tmp142=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp14D;(
_tmp14D.ReachableFL).tag=1;(_tmp14D.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmpD3,_tmpEF->loc,_tmp14A,env->all_changed,_tmp148,(void*)_tmpD3->unknown_all);(
_tmp14D.ReachableFL).f2=_tmp14B;(_tmp14D.ReachableFL).f3=_tmp14C;_tmp14D;});goto
_LL126;_LL129:;_LL12A: goto _LL126;_LL126:;}return({struct _tuple5 _tmp14E;_tmp14E.f1=
_tmp142;_tmp14E.f2=(void*)_tmpD3->unknown_all;_tmp14E;});}}_LLE5: if(*((int*)
_tmpDA)!= 4)goto _LLE7;_tmpF0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpDA)->f1;
_tmpF1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpDA)->f2;if(_tmpF1 == 0)goto _LLE7;
_tmpF2=*_tmpF1;_tmpF3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpDA)->f3;_LLE6: {
struct Cyc_List_List _tmp150=({struct Cyc_List_List _tmp16E;_tmp16E.hd=_tmpF3;
_tmp16E.tl=0;_tmp16E;});struct Cyc_List_List _tmp151=({struct Cyc_List_List _tmp16D;
_tmp16D.hd=_tmpF0;_tmp16D.tl=(struct Cyc_List_List*)& _tmp150;_tmp16D;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp153;struct _tuple5 _tmp152=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp151,1);_tmp153=_tmp152.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp155;struct _tuple7 _tmp154=Cyc_NewControlFlow_anal_Lexp(env,_tmp153,e);_tmp155=
_tmp154.f2;_tmp153=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp153);{struct
_tuple12 _tmp157=({struct _tuple12 _tmp156;_tmp156.f1=_tmp155;_tmp156.f2=_tmp153;
_tmp156;});union Cyc_CfFlowInfo_AbsLVal_union _tmp158;struct Cyc_CfFlowInfo_Place*
_tmp159;union Cyc_CfFlowInfo_FlowInfo_union _tmp15A;struct Cyc_Dict_Dict _tmp15B;
struct Cyc_List_List*_tmp15C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp15D;_LL12C:
_tmp158=_tmp157.f1;if(((_tmp157.f1).PlaceL).tag != 0)goto _LL12E;_tmp159=(_tmp158.PlaceL).f1;
_tmp15A=_tmp157.f2;if(((_tmp157.f2).ReachableFL).tag != 1)goto _LL12E;_tmp15B=(
_tmp15A.ReachableFL).f1;_tmp15C=(_tmp15A.ReachableFL).f2;_tmp15D=(_tmp15A.ReachableFL).f3;
_LL12D: _tmp15C=Cyc_CfFlowInfo_reln_kill_exp(_tmpD3->r,_tmp15C,_tmpF0);_tmp15B=
Cyc_CfFlowInfo_assign_place(_tmpD3,_tmpF0->loc,_tmp15B,env->all_changed,_tmp159,(
void*)_tmpD3->unknown_all);if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF0->topt))->v) && Cyc_Tcutil_is_unique_path(_tmpF0)){_tmp15D.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD3->r,Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*_tmp15E=_region_malloc(
_tmpD3->r,sizeof(*_tmp15E));_tmp15E->hd=_tmp159;_tmp15E->tl=0;_tmp15E;}),_tmp15D.may_consume);
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
_tmp15D.consumed,_tmp159)){struct Cyc_CfFlowInfo_Place _tmp160;void*_tmp161;struct
Cyc_CfFlowInfo_Place*_tmp15F=_tmp159;_tmp160=*_tmp15F;_tmp161=(void*)_tmp160.root;{
void*_tmp162=_tmp161;struct Cyc_Absyn_Vardecl*_tmp163;_LL131: if(*((int*)_tmp162)
!= 0)goto _LL133;_tmp163=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp162)->f1;
_LL132: {struct _dynforward_ptr _tmp164=Cyc_Absynpp_qvar2string(_tmp163->name);({
struct Cyc_String_pa_struct _tmp167;_tmp167.tag=0;_tmp167.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp164);{void*_tmp165[1]={& _tmp167};Cyc_Tcutil_warn(
_tmpF0->loc,({const char*_tmp166="may clobber unique pointer %s";_tag_dynforward(
_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,30));}),_tag_dynforward(_tmp165,
sizeof(void*),1));}});goto _LL130;}_LL133:;_LL134:({void*_tmp168[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp169="warning locations not for VarRoots";_tag_dynforward(_tmp169,sizeof(char),
_get_zero_arr_size(_tmp169,35));}),_tag_dynforward(_tmp168,sizeof(void*),0));});
_LL130:;}}if(env->all_changed == 0){struct Cyc_Dict_Dict _tmp16A=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)(
_tmpD3->r,_tmp15D.consumed,_tmp159);if((_tmp15D.consumed).t != _tmp16A.t);_tmp15D.consumed=
_tmp16A;}}_tmp153=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp16B;(_tmp16B.ReachableFL).tag=1;(_tmp16B.ReachableFL).f1=_tmp15B;(_tmp16B.ReachableFL).f2=
_tmp15C;(_tmp16B.ReachableFL).f3=_tmp15D;_tmp16B;});goto _LL12B;_LL12E:;_LL12F:
goto _LL12B;_LL12B:;}return({struct _tuple5 _tmp16C;_tmp16C.f1=_tmp153;_tmp16C.f2=(
void*)_tmpD3->unknown_all;_tmp16C;});}}_LLE7: if(*((int*)_tmpDA)!= 4)goto _LLE9;
_tmpF4=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpDA)->f1;_tmpF5=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpDA)->f2;if(_tmpF5 != 0)goto _LLE9;_tmpF6=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpDA)->f3;_LLE8: {struct Cyc_Dict_Dict*_tmp16F=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp171;union Cyc_CfFlowInfo_FlowInfo_union _tmp172;struct _tuple6 _tmp170=Cyc_CfFlowInfo_save_consume_info(
_tmpD3,inflow,1);_tmp171=_tmp170.f1;_tmp172=_tmp170.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp19B;_tmp19B.consumed=_tmpD3->mt_place_set;
_tmp19B.may_consume=0;_tmp19B;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp173=_region_malloc(env->r,sizeof(*_tmp173));_tmp173[0]=_tmpD3->mt_place_set;
_tmp173;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp175;union Cyc_CfFlowInfo_AbsLVal_union
_tmp176;struct _tuple7 _tmp174=Cyc_NewControlFlow_anal_Lexp(env,_tmp172,_tmpF4);
_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{struct Cyc_Dict_Dict _tmp177=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp178=
_region_malloc(env->r,sizeof(*_tmp178));_tmp178[0]=_tmpD3->mt_place_set;_tmp178;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp17A;void*_tmp17B;struct _tuple5 _tmp179=Cyc_NewControlFlow_anal_Rexp(
env,_tmp172,_tmpF6);_tmp17A=_tmp179.f1;_tmp17B=_tmp179.f2;{struct Cyc_Dict_Dict
_tmp17C=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp17D=Cyc_CfFlowInfo_after_flow(_tmpD3,(struct Cyc_Dict_Dict*)& _tmp177,_tmp175,
_tmp17A,_tmp177,_tmp17C);union Cyc_CfFlowInfo_FlowInfo_union _tmp17E=Cyc_CfFlowInfo_join_flow(
_tmpD3,_tmp16F,_tmp172,_tmp17D,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp180;struct
_tuple6 _tmp17F=Cyc_CfFlowInfo_save_consume_info(_tmpD3,_tmp17E,0);_tmp180=
_tmp17F.f1;if(init_consume)_tmp172=Cyc_CfFlowInfo_restore_consume_info(_tmp172,
outflow_consume);outflow_consume=_tmp180;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp17E,_tmp172)){if(_tmp16F == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp16F,Cyc_CfFlowInfo_union_place_set(
_tmp177,_tmp17C,0),0);}_tmp17D=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp17D);{union Cyc_CfFlowInfo_FlowInfo_union _tmp181=_tmp17D;struct Cyc_Dict_Dict
_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_CfFlowInfo_ConsumeInfo _tmp184;
_LL136: if((_tmp181.BottomFL).tag != 0)goto _LL138;_LL137: return({struct _tuple5
_tmp185;_tmp185.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp186;(_tmp186.BottomFL).tag=0;_tmp186;});_tmp185.f2=_tmp17B;_tmp185;});_LL138:
if((_tmp181.ReachableFL).tag != 1)goto _LL135;_tmp182=(_tmp181.ReachableFL).f1;
_tmp183=(_tmp181.ReachableFL).f2;_tmp184=(_tmp181.ReachableFL).f3;_LL139: _tmp184=
Cyc_CfFlowInfo_and_consume(_tmpD3->r,_tmp171,_tmp184);{union Cyc_CfFlowInfo_AbsLVal_union
_tmp187=_tmp176;struct Cyc_CfFlowInfo_Place*_tmp188;_LL13B: if((_tmp187.PlaceL).tag
!= 0)goto _LL13D;_tmp188=(_tmp187.PlaceL).f1;_LL13C: _tmp182=Cyc_CfFlowInfo_assign_place(
_tmpD3,e->loc,_tmp182,env->all_changed,_tmp188,_tmp17B);if(Cyc_Tcutil_is_unique_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF4->topt))->v) && Cyc_Tcutil_is_unique_path(
_tmpF4)){_tmp184.may_consume=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpD3->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp189=_region_malloc(_tmpD3->r,sizeof(*_tmp189));_tmp189->hd=
_tmp188;_tmp189->tl=0;_tmp189;}),_tmp184.may_consume);if(!((int(*)(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp184.consumed,_tmp188)){
struct Cyc_CfFlowInfo_Place _tmp18B;void*_tmp18C;struct Cyc_CfFlowInfo_Place*
_tmp18A=_tmp188;_tmp18B=*_tmp18A;_tmp18C=(void*)_tmp18B.root;{void*_tmp18D=
_tmp18C;struct Cyc_Absyn_Vardecl*_tmp18E;_LL140: if(*((int*)_tmp18D)!= 0)goto
_LL142;_tmp18E=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp18D)->f1;_LL141: {
struct _dynforward_ptr _tmp18F=Cyc_Absynpp_qvar2string(_tmp18E->name);({struct Cyc_String_pa_struct
_tmp192;_tmp192.tag=0;_tmp192.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp18F);{void*_tmp190[1]={& _tmp192};Cyc_Tcutil_warn(_tmpF4->loc,({const char*
_tmp191="may clobber unique pointer %s";_tag_dynforward(_tmp191,sizeof(char),
_get_zero_arr_size(_tmp191,30));}),_tag_dynforward(_tmp190,sizeof(void*),1));}});
goto _LL13F;}_LL142:;_LL143:({void*_tmp193[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp194="warning locations not for VarRoots";_tag_dynforward(_tmp194,
sizeof(char),_get_zero_arr_size(_tmp194,35));}),_tag_dynforward(_tmp193,sizeof(
void*),0));});_LL13F:;}}if(env->all_changed == 0){struct Cyc_Dict_Dict _tmp195=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmpD3->r,_tmp184.consumed,_tmp188);if((_tmp184.consumed).t != 
_tmp195.t);_tmp184.consumed=_tmp195;}}_tmp183=Cyc_CfFlowInfo_reln_assign_exp(
_tmpD3->r,_tmp183,_tmpF4,_tmpF6);_tmp17D=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp196;(_tmp196.ReachableFL).tag=1;(_tmp196.ReachableFL).f1=
_tmp182;(_tmp196.ReachableFL).f2=_tmp183;(_tmp196.ReachableFL).f3=_tmp184;
_tmp196;});Cyc_NewControlFlow_update_tryflow(env,_tmp17D);return({struct _tuple5
_tmp197;_tmp197.f1=_tmp17D;_tmp197.f2=_tmp17B;_tmp197;});_LL13D: if((_tmp187.UnknownL).tag
!= 1)goto _LL13A;_LL13E: if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF4->topt))->v))({void*_tmp198[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp199="dest of unique assignment is unknown!";_tag_dynforward(_tmp199,sizeof(
char),_get_zero_arr_size(_tmp199,38));}),_tag_dynforward(_tmp198,sizeof(void*),0));});
return({struct _tuple5 _tmp19A;_tmp19A.f1=Cyc_NewControlFlow_use_Rval(env,_tmpF6->loc,
_tmp17D,_tmp17B);_tmp19A.f2=_tmp17B;_tmp19A;});_LL13A:;}_LL135:;}}_tmp172=Cyc_CfFlowInfo_restore_consume_info(
_tmp17E,empty_consume);}}}}}}}_LLE9: if(*((int*)_tmpDA)!= 9)goto _LLEB;_tmpF7=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmpDA)->f1;_tmpF8=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpDA)->f2;_LLEA: {union Cyc_CfFlowInfo_FlowInfo_union _tmp19D;void*_tmp19E;
struct _tuple5 _tmp19C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpF7);_tmp19D=
_tmp19C.f1;_tmp19E=_tmp19C.f2;_tmp19D=Cyc_CfFlowInfo_drop_unique_rvals(_tmpF7->loc,
_tmp19D);return Cyc_NewControlFlow_anal_Rexp(env,_tmp19D,_tmpF8);}_LLEB: if(*((int*)
_tmpDA)!= 12)goto _LLED;_tmpF9=((struct Cyc_Absyn_Throw_e_struct*)_tmpDA)->f1;
_LLEC: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1A0;void*_tmp1A1;struct _tuple5
_tmp19F=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpF9);_tmp1A0=_tmp19F.f1;
_tmp1A1=_tmp19F.f2;_tmp1A0=Cyc_CfFlowInfo_consume_unique_rvals(_tmpF9->loc,
_tmp1A0);Cyc_NewControlFlow_use_Rval(env,_tmpF9->loc,_tmp1A0,_tmp1A1);return({
struct _tuple5 _tmp1A2;_tmp1A2.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A3;(_tmp1A3.BottomFL).tag=0;_tmp1A3;});_tmp1A2.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpD3,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpD3->unknown_all);
_tmp1A2;});}_LLED: if(*((int*)_tmpDA)!= 11)goto _LLEF;_tmpFA=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpDA)->f1;_tmpFB=((struct Cyc_Absyn_FnCall_e_struct*)_tmpDA)->f2;_LLEE: {struct
_RegionHandle _tmp1A4=_new_region("temp");struct _RegionHandle*temp=& _tmp1A4;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1A7;struct Cyc_List_List*
_tmp1A8;struct _tuple11 _tmp1A6=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp1A5=_region_malloc(temp,sizeof(*_tmp1A5));
_tmp1A5->hd=_tmpFA;_tmp1A5->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmpFB);_tmp1A5;}),1);_tmp1A7=
_tmp1A6.f1;_tmp1A8=_tmp1A6.f2;_tmp1A7=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1A7);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1A9=Cyc_NewControlFlow_use_Rval(
env,_tmpFA->loc,_tmp1A7,(void*)((struct Cyc_List_List*)_check_null(_tmp1A8))->hd);
_tmp1A8=_tmp1A8->tl;{struct Cyc_List_List*init_params=0;{void*_tmp1AA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpFA->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1AB;void*_tmp1AC;_LL145: if(_tmp1AA <= (void*)4)goto _LL147;if(*((int*)_tmp1AA)
!= 4)goto _LL147;_tmp1AB=((struct Cyc_Absyn_PointerType_struct*)_tmp1AA)->f1;
_tmp1AC=(void*)_tmp1AB.elt_typ;_LL146:{void*_tmp1AD=Cyc_Tcutil_compress(_tmp1AC);
struct Cyc_Absyn_FnInfo _tmp1AE;struct Cyc_List_List*_tmp1AF;_LL14A: if(_tmp1AD <= (
void*)4)goto _LL14C;if(*((int*)_tmp1AD)!= 8)goto _LL14C;_tmp1AE=((struct Cyc_Absyn_FnType_struct*)
_tmp1AD)->f1;_tmp1AF=_tmp1AE.attributes;_LL14B: for(0;_tmp1AF != 0;_tmp1AF=_tmp1AF->tl){
void*_tmp1B0=(void*)_tmp1AF->hd;int _tmp1B1;_LL14F: if(_tmp1B0 <= (void*)17)goto
_LL151;if(*((int*)_tmp1B0)!= 4)goto _LL151;_tmp1B1=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1B0)->f1;_LL150: init_params=({struct Cyc_List_List*_tmp1B2=_region_malloc(
temp,sizeof(*_tmp1B2));_tmp1B2->hd=(void*)_tmp1B1;_tmp1B2->tl=init_params;
_tmp1B2;});goto _LL14E;_LL151:;_LL152: goto _LL14E;_LL14E:;}goto _LL149;_LL14C:;
_LL14D:({void*_tmp1B3[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B4="anal_Rexp: bad function type";
_tag_dynforward(_tmp1B4,sizeof(char),_get_zero_arr_size(_tmp1B4,29));}),
_tag_dynforward(_tmp1B3,sizeof(void*),0));});_LL149:;}goto _LL144;_LL147:;_LL148:({
void*_tmp1B5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1B6="anal_Rexp: bad function type";
_tag_dynforward(_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,29));}),
_tag_dynforward(_tmp1B5,sizeof(void*),0));});_LL144:;}{int i=1;for(0;_tmp1A8 != 0;(((
_tmp1A8=_tmp1A8->tl,_tmpFB=((struct Cyc_List_List*)_check_null(_tmpFB))->tl)),++ i)){
if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){_tmp1A9=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpFB))->hd)->loc,_tmp1A9,(void*)_tmp1A8->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B7=_tmp1A7;struct Cyc_Dict_Dict _tmp1B8;_LL154: if((_tmp1B7.BottomFL).tag != 0)
goto _LL156;_LL155: goto _LL153;_LL156: if((_tmp1B7.ReachableFL).tag != 1)goto _LL153;
_tmp1B8=(_tmp1B7.ReachableFL).f1;_LL157: if(Cyc_CfFlowInfo_initlevel(_tmp1B8,(
void*)_tmp1A8->hd)== (void*)0)({void*_tmp1B9[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpFB))->hd)->loc,({const char*_tmp1BA="expression may not be initialized";
_tag_dynforward(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,34));}),
_tag_dynforward(_tmp1B9,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BB=_tmp1A9;struct Cyc_Dict_Dict _tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1BE;_LL159: if((_tmp1BB.BottomFL).tag != 0)goto _LL15B;_LL15A: goto _LL158;_LL15B:
if((_tmp1BB.ReachableFL).tag != 1)goto _LL158;_tmp1BC=(_tmp1BB.ReachableFL).f1;
_tmp1BD=(_tmp1BB.ReachableFL).f2;_tmp1BE=(_tmp1BB.ReachableFL).f3;_LL15C: {
struct Cyc_Dict_Dict _tmp1BF=Cyc_CfFlowInfo_escape_deref(_tmpD3,_tmp1BC,env->all_changed,(
void*)_tmp1A8->hd);{void*_tmp1C0=(void*)_tmp1A8->hd;struct Cyc_CfFlowInfo_Place*
_tmp1C1;_LL15E: if(_tmp1C0 <= (void*)3)goto _LL160;if(*((int*)_tmp1C0)!= 2)goto
_LL160;_tmp1C1=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1C0)->f1;_LL15F:{
void*_tmp1C2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmpFB))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1C3;void*_tmp1C4;_LL163: if(_tmp1C2 <= (void*)4)goto
_LL165;if(*((int*)_tmp1C2)!= 4)goto _LL165;_tmp1C3=((struct Cyc_Absyn_PointerType_struct*)
_tmp1C2)->f1;_tmp1C4=(void*)_tmp1C3.elt_typ;_LL164: _tmp1BF=Cyc_CfFlowInfo_assign_place(
_tmpD3,((struct Cyc_Absyn_Exp*)_tmpFB->hd)->loc,_tmp1BF,env->all_changed,_tmp1C1,
Cyc_CfFlowInfo_typ_to_absrval(_tmpD3,_tmp1C4,(void*)_tmpD3->esc_all));goto _LL162;
_LL165:;_LL166:({void*_tmp1C5[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C6="anal_Rexp:bad type for initialized arg";
_tag_dynforward(_tmp1C6,sizeof(char),_get_zero_arr_size(_tmp1C6,39));}),
_tag_dynforward(_tmp1C5,sizeof(void*),0));});_LL162:;}goto _LL15D;_LL160:;_LL161:
goto _LL15D;_LL15D:;}_tmp1A9=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C7;(_tmp1C7.ReachableFL).tag=1;(_tmp1C7.ReachableFL).f1=_tmp1BF;(_tmp1C7.ReachableFL).f2=
_tmp1BD;(_tmp1C7.ReachableFL).f3=_tmp1BE;_tmp1C7;});goto _LL158;}_LL158:;}goto
_LL153;_LL153:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpFA->topt))->v)){struct _tuple5 _tmp1CA=({struct _tuple5 _tmp1C8;
_tmp1C8.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C9;(_tmp1C9.BottomFL).tag=0;_tmp1C9;});_tmp1C8.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpD3,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpD3->unknown_all);
_tmp1C8;});_npop_handler(0);return _tmp1CA;}else{struct _tuple5 _tmp1CC=({struct
_tuple5 _tmp1CB;_tmp1CB.f1=_tmp1A9;_tmp1CB.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpD3,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpD3->unknown_all);
_tmp1CB;});_npop_handler(0);return _tmp1CC;}}}};_pop_region(temp);}_LLEF: if(*((
int*)_tmpDA)!= 35)goto _LLF1;_tmpFC=((struct Cyc_Absyn_Malloc_e_struct*)_tmpDA)->f1;
_tmpFD=_tmpFC.is_calloc;_tmpFE=_tmpFC.rgn;_tmpFF=_tmpFC.elt_type;_tmp100=_tmpFC.num_elts;
_tmp101=_tmpFC.fat_result;_LLF0: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp1DE=_region_malloc(_tmpD3->r,sizeof(*_tmp1DE));_tmp1DE[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp1DF;_tmp1DF.tag=1;_tmp1DF.f1=_tmp100;_tmp1DF.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp1DF;});_tmp1DE;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp1DD=_region_malloc(_tmpD3->r,sizeof(*_tmp1DD));
_tmp1DD->root=(void*)root;_tmp1DD->fields=0;_tmp1DD;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp1DB=_region_malloc(_tmpD3->r,sizeof(*_tmp1DB));
_tmp1DB[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp1DC;_tmp1DC.tag=2;_tmp1DC.f1=
place;_tmp1DC;});_tmp1DB;});void*place_val=_tmp101?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(
_tmpD3,*((void**)_check_null(_tmpFF)),(void*)_tmpD3->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmpFE != 0){
struct _RegionHandle _tmp1CD=_new_region("temp");struct _RegionHandle*temp=& _tmp1CD;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D0;struct Cyc_List_List*
_tmp1D1;struct _tuple11 _tmp1CF=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1CE[2];_tmp1CE[1]=_tmp100;_tmp1CE[0]=(struct Cyc_Absyn_Exp*)
_tmpFE;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
temp,_tag_dynforward(_tmp1CE,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1D0=
_tmp1CF.f1;_tmp1D1=_tmp1CF.f2;outflow=_tmp1D0;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D3;struct _tuple5 _tmp1D2=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp100);
_tmp1D3=_tmp1D2.f1;outflow=_tmp1D3;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp100->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D4=outflow;struct
Cyc_Dict_Dict _tmp1D5;struct Cyc_List_List*_tmp1D6;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1D7;_LL168: if((_tmp1D4.BottomFL).tag != 0)goto _LL16A;_LL169: return({struct
_tuple5 _tmp1D8;_tmp1D8.f1=outflow;_tmp1D8.f2=rval;_tmp1D8;});_LL16A: if((_tmp1D4.ReachableFL).tag
!= 1)goto _LL167;_tmp1D5=(_tmp1D4.ReachableFL).f1;_tmp1D6=(_tmp1D4.ReachableFL).f2;
_tmp1D7=(_tmp1D4.ReachableFL).f3;_LL16B: return({struct _tuple5 _tmp1D9;_tmp1D9.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1DA;(
_tmp1DA.ReachableFL).tag=1;(_tmp1DA.ReachableFL).f1=Cyc_Dict_insert(_tmp1D5,root,
place_val);(_tmp1DA.ReachableFL).f2=_tmp1D6;(_tmp1DA.ReachableFL).f3=_tmp1D7;
_tmp1DA;});_tmp1D9.f2=rval;_tmp1D9;});_LL167:;}}_LLF1: if(*((int*)_tmpDA)!= 36)
goto _LLF3;_tmp102=((struct Cyc_Absyn_Swap_e_struct*)_tmpDA)->f1;_tmp103=((struct
Cyc_Absyn_Swap_e_struct*)_tmpDA)->f2;_LLF2: {void*left_rval;void*right_rval;
union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle _tmp1E0=
_new_region("temp");struct _RegionHandle*temp=& _tmp1E0;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp1E3;struct Cyc_List_List*_tmp1E4;struct _tuple11
_tmp1E2=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp1E1[2];_tmp1E1[1]=_tmp103;_tmp1E1[0]=_tmp102;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp1E1,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1E3=_tmp1E2.f1;_tmp1E4=
_tmp1E2.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1E4))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1E4->tl))->hd;outflow=
_tmp1E3;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp1E6;struct _tuple7 _tmp1E5=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp102);_tmp1E6=_tmp1E5.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1E8;struct _tuple7 _tmp1E7=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp103);
_tmp1E8=_tmp1E7.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E9=outflow;struct Cyc_Dict_Dict
_tmp1EA;struct Cyc_List_List*_tmp1EB;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1EC;
_LL16D: if((_tmp1E9.ReachableFL).tag != 1)goto _LL16F;_tmp1EA=(_tmp1E9.ReachableFL).f1;
_tmp1EB=(_tmp1E9.ReachableFL).f2;_tmp1EC=(_tmp1E9.ReachableFL).f3;_LL16E:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp1ED=_tmp1E6;struct Cyc_CfFlowInfo_Place*_tmp1EE;
_LL172: if((_tmp1ED.PlaceL).tag != 0)goto _LL174;_tmp1EE=(_tmp1ED.PlaceL).f1;_LL173:
_tmp1EA=Cyc_CfFlowInfo_assign_place(_tmpD3,_tmp102->loc,_tmp1EA,env->all_changed,
_tmp1EE,right_rval);goto _LL171;_LL174: if((_tmp1ED.UnknownL).tag != 1)goto _LL171;
_LL175: goto _LL171;_LL171:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp1EF=_tmp1E8;
struct Cyc_CfFlowInfo_Place*_tmp1F0;_LL177: if((_tmp1EF.PlaceL).tag != 0)goto _LL179;
_tmp1F0=(_tmp1EF.PlaceL).f1;_LL178: _tmp1EA=Cyc_CfFlowInfo_assign_place(_tmpD3,
_tmp103->loc,_tmp1EA,env->all_changed,_tmp1F0,left_rval);goto _LL176;_LL179: if((
_tmp1EF.UnknownL).tag != 1)goto _LL176;_LL17A: goto _LL176;_LL176:;}_tmp1EB=Cyc_CfFlowInfo_reln_kill_exp(
_tmpD3->r,_tmp1EB,_tmp102);_tmp1EB=Cyc_CfFlowInfo_reln_kill_exp(_tmpD3->r,
_tmp1EB,_tmp103);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F1;(_tmp1F1.ReachableFL).tag=1;(_tmp1F1.ReachableFL).f1=_tmp1EA;(_tmp1F1.ReachableFL).f2=
_tmp1EB;(_tmp1F1.ReachableFL).f3=_tmp1EC;_tmp1F1;});goto _LL16C;_LL16F:;_LL170:
goto _LL16C;_LL16C:;}return({struct _tuple5 _tmp1F2;_tmp1F2.f1=outflow;_tmp1F2.f2=(
void*)_tmpD3->unknown_all;_tmp1F2;});}}}_LLF3: if(*((int*)_tmpDA)!= 17)goto _LLF5;
_tmp104=((struct Cyc_Absyn_New_e_struct*)_tmpDA)->f1;_tmp105=((struct Cyc_Absyn_New_e_struct*)
_tmpDA)->f2;_LLF4: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp205=_region_malloc(_tmpD3->r,sizeof(*_tmp205));_tmp205[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp206;_tmp206.tag=1;_tmp206.f1=_tmp105;_tmp206.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp206;});_tmp205;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp204=_region_malloc(_tmpD3->r,sizeof(*_tmp204));
_tmp204->root=(void*)root;_tmp204->fields=0;_tmp204;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp202=_region_malloc(_tmpD3->r,sizeof(*_tmp202));
_tmp202[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp203;_tmp203.tag=2;_tmp203.f1=
place;_tmp203;});_tmp202;});Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmp104 != 
0){struct _RegionHandle _tmp1F3=_new_region("temp");struct _RegionHandle*temp=&
_tmp1F3;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1F6;struct Cyc_List_List*
_tmp1F7;struct _tuple11 _tmp1F5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1F4[2];_tmp1F4[1]=_tmp105;_tmp1F4[0]=(struct Cyc_Absyn_Exp*)
_tmp104;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp1F4,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp1F6=_tmp1F5.f1;_tmp1F7=_tmp1F5.f2;outflow=_tmp1F6;place_val=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp1F7))->tl))->hd;};
_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp1F9;void*_tmp1FA;
struct _tuple5 _tmp1F8=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp105);_tmp1F9=
_tmp1F8.f1;_tmp1FA=_tmp1F8.f2;outflow=_tmp1F9;place_val=_tmp1FA;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp105->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1FB=outflow;struct
Cyc_Dict_Dict _tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1FE;_LL17C: if((_tmp1FB.BottomFL).tag != 0)goto _LL17E;_LL17D: return({struct
_tuple5 _tmp1FF;_tmp1FF.f1=outflow;_tmp1FF.f2=rval;_tmp1FF;});_LL17E: if((_tmp1FB.ReachableFL).tag
!= 1)goto _LL17B;_tmp1FC=(_tmp1FB.ReachableFL).f1;_tmp1FD=(_tmp1FB.ReachableFL).f2;
_tmp1FE=(_tmp1FB.ReachableFL).f3;_LL17F: return({struct _tuple5 _tmp200;_tmp200.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp201;(
_tmp201.ReachableFL).tag=1;(_tmp201.ReachableFL).f1=Cyc_Dict_insert(_tmp1FC,root,
place_val);(_tmp201.ReachableFL).f2=_tmp1FD;(_tmp201.ReachableFL).f3=_tmp1FE;
_tmp201;});_tmp200.f2=rval;_tmp200;});_LL17B:;}}}_LLF5: if(*((int*)_tmpDA)!= 16)
goto _LLF7;_tmp106=((struct Cyc_Absyn_Address_e_struct*)_tmpDA)->f1;_LLF6: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp208;union Cyc_CfFlowInfo_AbsLVal_union _tmp209;
struct _tuple7 _tmp207=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp106);_tmp208=
_tmp207.f1;_tmp209=_tmp207.f2;{union Cyc_CfFlowInfo_AbsLVal_union _tmp20A=_tmp209;
struct Cyc_CfFlowInfo_Place*_tmp20B;_LL181: if((_tmp20A.UnknownL).tag != 1)goto
_LL183;_LL182: return({struct _tuple5 _tmp20C;_tmp20C.f1=_tmp208;_tmp20C.f2=(void*)
1;_tmp20C;});_LL183: if((_tmp20A.PlaceL).tag != 0)goto _LL180;_tmp20B=(_tmp20A.PlaceL).f1;
_LL184: return({struct _tuple5 _tmp20D;_tmp20D.f1=_tmp208;_tmp20D.f2=(void*)({
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp20E=_region_malloc(env->r,sizeof(*
_tmp20E));_tmp20E[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp20F;_tmp20F.tag=
2;_tmp20F.f1=_tmp20B;_tmp20F;});_tmp20E;});_tmp20D;});_LL180:;}}_LLF7: if(*((int*)
_tmpDA)!= 22)goto _LLF9;_tmp107=((struct Cyc_Absyn_Deref_e_struct*)_tmpDA)->f1;
_LLF8: {union Cyc_CfFlowInfo_FlowInfo_union _tmp211;void*_tmp212;struct _tuple5
_tmp210=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp107);_tmp211=_tmp210.f1;
_tmp212=_tmp210.f2;_tmp211=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp211);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp211,_tmp107,_tmp212);}
_LLF9: if(*((int*)_tmpDA)!= 23)goto _LLFB;_tmp108=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpDA)->f1;_tmp109=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpDA)->f2;_LLFA: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp214;void*_tmp215;struct _tuple5 _tmp213=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp108);_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp108->topt))->v))return({struct
_tuple5 _tmp216;_tmp216.f1=_tmp214;_tmp216.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpD3,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpD3->unknown_all);
_tmp216;});{void*_tmp217=_tmp215;struct _dynforward_ptr _tmp218;_LL186: if(_tmp217
<= (void*)3)goto _LL188;if(*((int*)_tmp217)!= 4)goto _LL188;_tmp218=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp217)->f1;_LL187: {int _tmp219=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp108->topt))->v,_tmp109);return({struct
_tuple5 _tmp21A;_tmp21A.f1=_tmp214;_tmp21A.f2=*((void**)
_check_dynforward_subscript(_tmp218,sizeof(void*),_tmp219));_tmp21A;});}_LL188:;
_LL189:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp21B=_cycalloc(
sizeof(*_tmp21B));_tmp21B[0]=({struct Cyc_Core_Impossible_struct _tmp21C;_tmp21C.tag=
Cyc_Core_Impossible;_tmp21C.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp21F;_tmp21F.tag=0;_tmp21F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp21D[1]={& _tmp21F};Cyc_aprintf(({const char*
_tmp21E="anal_Rexp: AggrMember: %s";_tag_dynforward(_tmp21E,sizeof(char),
_get_zero_arr_size(_tmp21E,26));}),_tag_dynforward(_tmp21D,sizeof(void*),1));}});
_tmp21C;});_tmp21B;}));_LL185:;}}_LLFB: if(*((int*)_tmpDA)!= 24)goto _LLFD;_tmp10A=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpDA)->f1;_tmp10B=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpDA)->f2;_LLFC: {union Cyc_CfFlowInfo_FlowInfo_union _tmp221;void*_tmp222;
struct _tuple5 _tmp220=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp10A);_tmp221=
_tmp220.f1;_tmp222=_tmp220.f2;_tmp221=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp221);{union Cyc_CfFlowInfo_FlowInfo_union _tmp224;void*_tmp225;struct _tuple5
_tmp223=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp221,_tmp10A,_tmp222);
_tmp224=_tmp223.f1;_tmp225=_tmp223.f2;{void*_tmp226=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp10A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp227;void*_tmp228;_LL18B: if(_tmp226 <= (void*)4)goto _LL18D;if(*((int*)_tmp226)
!= 4)goto _LL18D;_tmp227=((struct Cyc_Absyn_PointerType_struct*)_tmp226)->f1;
_tmp228=(void*)_tmp227.elt_typ;_LL18C: if(Cyc_Absyn_is_union_type(_tmp228))return({
struct _tuple5 _tmp229;_tmp229.f1=_tmp224;_tmp229.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpD3,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpD3->unknown_all);
_tmp229;});{void*_tmp22A=_tmp225;struct _dynforward_ptr _tmp22B;_LL190: if(_tmp22A
<= (void*)3)goto _LL192;if(*((int*)_tmp22A)!= 4)goto _LL192;_tmp22B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp22A)->f1;_LL191: {int _tmp22C=Cyc_NewControlFlow_get_field_index(_tmp228,
_tmp10B);return({struct _tuple5 _tmp22D;_tmp22D.f1=_tmp224;_tmp22D.f2=*((void**)
_check_dynforward_subscript(_tmp22B,sizeof(void*),_tmp22C));_tmp22D;});}_LL192:;
_LL193:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp22E=_cycalloc(
sizeof(*_tmp22E));_tmp22E[0]=({struct Cyc_Core_Impossible_struct _tmp22F;_tmp22F.tag=
Cyc_Core_Impossible;_tmp22F.f1=({const char*_tmp230="anal_Rexp: AggrArrow";
_tag_dynforward(_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,21));});_tmp22F;});
_tmp22E;}));_LL18F:;}_LL18D:;_LL18E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231[0]=({struct Cyc_Core_Impossible_struct
_tmp232;_tmp232.tag=Cyc_Core_Impossible;_tmp232.f1=({const char*_tmp233="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,25));});_tmp232;});
_tmp231;}));_LL18A:;}}}_LLFD: if(*((int*)_tmpDA)!= 6)goto _LLFF;_tmp10C=((struct
Cyc_Absyn_Conditional_e_struct*)_tmpDA)->f1;_tmp10D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpDA)->f2;_tmp10E=((struct Cyc_Absyn_Conditional_e_struct*)_tmpDA)->f3;_LLFE: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp235;union Cyc_CfFlowInfo_FlowInfo_union
_tmp236;struct _tuple8 _tmp234=Cyc_NewControlFlow_anal_test(env,inflow,_tmp10C);
_tmp235=_tmp234.f1;_tmp236=_tmp234.f2;_tmp235=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp10C->loc,_tmp235);_tmp236=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp10C->loc,
_tmp236);{struct _tuple5 _tmp237=Cyc_NewControlFlow_anal_Rexp(env,_tmp235,_tmp10D);
struct _tuple5 _tmp238=Cyc_NewControlFlow_anal_Rexp(env,_tmp236,_tmp10E);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpD3,env->all_changed,_tmp237,_tmp238,1);}}_LLFF: if(*((int*)_tmpDA)!= 7)goto
_LL101;_tmp10F=((struct Cyc_Absyn_And_e_struct*)_tmpDA)->f1;_tmp110=((struct Cyc_Absyn_And_e_struct*)
_tmpDA)->f2;_LL100: {union Cyc_CfFlowInfo_FlowInfo_union _tmp23A;union Cyc_CfFlowInfo_FlowInfo_union
_tmp23B;struct _tuple8 _tmp239=Cyc_NewControlFlow_anal_test(env,inflow,_tmp10F);
_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;_tmp23A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp10F->loc,_tmp23A);_tmp23B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp10F->loc,
_tmp23B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp23D;void*_tmp23E;struct _tuple5
_tmp23C=Cyc_NewControlFlow_anal_Rexp(env,_tmp23A,_tmp110);_tmp23D=_tmp23C.f1;
_tmp23E=_tmp23C.f2;_tmp23D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp110->loc,
_tmp23D);{struct _tuple5 _tmp23F=({struct _tuple5 _tmp242;_tmp242.f1=_tmp23D;_tmp242.f2=
_tmp23E;_tmp242;});struct _tuple5 _tmp240=({struct _tuple5 _tmp241;_tmp241.f1=
_tmp23B;_tmp241.f2=(void*)((void*)0);_tmp241;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpD3,env->all_changed,_tmp23F,_tmp240,0);}}}_LL101: if(*((int*)_tmpDA)!= 8)goto
_LL103;_tmp111=((struct Cyc_Absyn_Or_e_struct*)_tmpDA)->f1;_tmp112=((struct Cyc_Absyn_Or_e_struct*)
_tmpDA)->f2;_LL102: {union Cyc_CfFlowInfo_FlowInfo_union _tmp244;union Cyc_CfFlowInfo_FlowInfo_union
_tmp245;struct _tuple8 _tmp243=Cyc_NewControlFlow_anal_test(env,inflow,_tmp111);
_tmp244=_tmp243.f1;_tmp245=_tmp243.f2;_tmp244=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp111->loc,_tmp244);_tmp245=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp111->loc,
_tmp245);{union Cyc_CfFlowInfo_FlowInfo_union _tmp247;void*_tmp248;struct _tuple5
_tmp246=Cyc_NewControlFlow_anal_Rexp(env,_tmp245,_tmp112);_tmp247=_tmp246.f1;
_tmp248=_tmp246.f2;_tmp247=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp112->loc,
_tmp247);{struct _tuple5 _tmp249=({struct _tuple5 _tmp24C;_tmp24C.f1=_tmp247;_tmp24C.f2=
_tmp248;_tmp24C;});struct _tuple5 _tmp24A=({struct _tuple5 _tmp24B;_tmp24B.f1=
_tmp244;_tmp24B.f2=(void*)((void*)1);_tmp24B;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmpD3,env->all_changed,_tmp249,_tmp24A,0);}}}_LL103: if(*((int*)_tmpDA)!= 25)
goto _LL105;_tmp113=((struct Cyc_Absyn_Subscript_e_struct*)_tmpDA)->f1;_tmp114=((
struct Cyc_Absyn_Subscript_e_struct*)_tmpDA)->f2;_LL104: {struct _RegionHandle
_tmp24D=_new_region("temp");struct _RegionHandle*temp=& _tmp24D;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp250;struct Cyc_List_List*_tmp251;struct
_tuple11 _tmp24F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct
Cyc_Absyn_Exp*_tmp24E[2];_tmp24E[1]=_tmp114;_tmp24E[0]=_tmp113;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp24E,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp250=_tmp24F.f1;_tmp251=
_tmp24F.f2;_tmp250=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp114->loc,_tmp250);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp252=_tmp250;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp253=_tmp250;struct Cyc_Dict_Dict _tmp254;struct Cyc_List_List*_tmp255;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp256;_LL195: if((_tmp253.ReachableFL).tag != 1)goto _LL197;_tmp254=(_tmp253.ReachableFL).f1;
_tmp255=(_tmp253.ReachableFL).f2;_tmp256=(_tmp253.ReachableFL).f3;_LL196: if(Cyc_CfFlowInfo_initlevel(
_tmp254,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp251))->tl))->hd)== (void*)0)({void*_tmp257[0]={};Cyc_Tcutil_terr(
_tmp114->loc,({const char*_tmp258="expression may not be initialized";
_tag_dynforward(_tmp258,sizeof(char),_get_zero_arr_size(_tmp258,34));}),
_tag_dynforward(_tmp257,sizeof(void*),0));});{struct Cyc_List_List*_tmp259=Cyc_NewControlFlow_add_subscript_reln(
_tmpD3->r,_tmp255,_tmp113,_tmp114);if(_tmp255 != _tmp259)_tmp252=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp25A;(_tmp25A.ReachableFL).tag=1;(_tmp25A.ReachableFL).f1=
_tmp254;(_tmp25A.ReachableFL).f2=_tmp259;(_tmp25A.ReachableFL).f3=_tmp256;
_tmp25A;});goto _LL194;}_LL197:;_LL198: goto _LL194;_LL194:;}{void*_tmp25B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp113->topt))->v);struct Cyc_List_List*
_tmp25C;struct Cyc_Absyn_PtrInfo _tmp25D;struct Cyc_Absyn_PtrAtts _tmp25E;struct Cyc_Absyn_Conref*
_tmp25F;_LL19A: if(_tmp25B <= (void*)4)goto _LL19E;if(*((int*)_tmp25B)!= 9)goto
_LL19C;_tmp25C=((struct Cyc_Absyn_TupleType_struct*)_tmp25B)->f1;_LL19B: {void*
_tmp260=(void*)((struct Cyc_List_List*)_check_null(_tmp251))->hd;struct
_dynforward_ptr _tmp261;_LL1A1: if(_tmp260 <= (void*)3)goto _LL1A3;if(*((int*)
_tmp260)!= 4)goto _LL1A3;_tmp261=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp260)->f1;
_LL1A2: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp114)).f1;struct _tuple5
_tmp263=({struct _tuple5 _tmp262;_tmp262.f1=_tmp252;_tmp262.f2=*((void**)
_check_dynforward_subscript(_tmp261,sizeof(void*),(int)i));_tmp262;});
_npop_handler(0);return _tmp263;}_LL1A3:;_LL1A4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Core_Impossible_struct
_tmp265;_tmp265.tag=Cyc_Core_Impossible;_tmp265.f1=({const char*_tmp266="anal_Rexp: Subscript";
_tag_dynforward(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,21));});_tmp265;});
_tmp264;}));_LL1A0:;}_LL19C: if(*((int*)_tmp25B)!= 4)goto _LL19E;_tmp25D=((struct
Cyc_Absyn_PointerType_struct*)_tmp25B)->f1;_tmp25E=_tmp25D.ptr_atts;_tmp25F=
_tmp25E.bounds;_LL19D:{union Cyc_Absyn_Constraint_union _tmp267=(Cyc_Absyn_compress_conref(
_tmp25F))->v;void*_tmp268;_LL1A6: if((_tmp267.Eq_constr).tag != 0)goto _LL1A8;
_tmp268=(_tmp267.Eq_constr).f1;if(_tmp268 <= (void*)2)goto _LL1A8;if(*((int*)
_tmp268)!= 1)goto _LL1A8;_LL1A7:{void*_tmp269=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp251))->tl))->hd;struct Cyc_List_List*
_tmp26A;_LL1AB: if(_tmp269 <= (void*)3)goto _LL1AD;if(*((int*)_tmp269)!= 3)goto
_LL1AD;_tmp26A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp269)->f1;_LL1AC:(void*)(
_tmp114->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp26B=
_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp26C;_tmp26C.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp26C.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp26A);_tmp26C;});_tmp26B;})));goto _LL1AA;_LL1AD:;_LL1AE:
goto _LL1AA;_LL1AA:;}goto _LL1A5;_LL1A8:;_LL1A9: goto _LL1A5;_LL1A5:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp26E;void*_tmp26F;struct _tuple5 _tmp26D=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp250,_tmp113,(void*)((struct Cyc_List_List*)_check_null(_tmp251))->hd);
_tmp26E=_tmp26D.f1;_tmp26F=_tmp26D.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp270=
_tmp26E;_LL1B0: if((_tmp270.BottomFL).tag != 0)goto _LL1B2;_LL1B1: {struct _tuple5
_tmp272=({struct _tuple5 _tmp271;_tmp271.f1=_tmp26E;_tmp271.f2=_tmp26F;_tmp271;});
_npop_handler(0);return _tmp272;}_LL1B2:;_LL1B3: {struct _tuple5 _tmp274=({struct
_tuple5 _tmp273;_tmp273.f1=_tmp252;_tmp273.f2=_tmp26F;_tmp273;});_npop_handler(0);
return _tmp274;}_LL1AF:;}}_LL19E:;_LL19F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Core_Impossible_struct
_tmp276;_tmp276.tag=Cyc_Core_Impossible;_tmp276.f1=({const char*_tmp277="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,33));});_tmp276;});
_tmp275;}));_LL199:;}}};_pop_region(temp);}_LL105: if(*((int*)_tmpDA)!= 32)goto
_LL107;_tmp115=((struct Cyc_Absyn_Tunion_e_struct*)_tmpDA)->f1;_tmp116=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpDA)->f2;_LL106: if(_tmp116->is_flat){struct
_RegionHandle _tmp278=_new_region("temp");struct _RegionHandle*temp=& _tmp278;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp27A;struct Cyc_List_List*
_tmp27B;struct _tuple11 _tmp279=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,_tmp115,0);_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;_tmp27A=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp27A);for(0;(unsigned int)_tmp115;(_tmp115=_tmp115->tl,_tmp27B=_tmp27B->tl)){
_tmp27A=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp115->hd)->loc,
_tmp27A,(void*)((struct Cyc_List_List*)_check_null(_tmp27B))->hd);}{struct _tuple5
_tmp27D=({struct _tuple5 _tmp27C;_tmp27C.f1=_tmp27A;_tmp27C.f2=(void*)_tmpD3->unknown_all;
_tmp27C;});_npop_handler(0);return _tmp27D;}};_pop_region(temp);}_tmp117=_tmp115;
goto _LL108;_LL107: if(*((int*)_tmpDA)!= 26)goto _LL109;_tmp117=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpDA)->f1;_LL108: {struct _RegionHandle _tmp27E=_new_region("temp");struct
_RegionHandle*temp=& _tmp27E;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp280;struct Cyc_List_List*_tmp281;struct _tuple11 _tmp27F=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp117,0);_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;_tmp280=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp280);{struct _dynforward_ptr aggrdict=({unsigned int _tmp286=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp117);void**
_tmp287=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp286));
struct _dynforward_ptr _tmp28A=_tag_dynforward(_tmp287,sizeof(void*),_tmp286);{
unsigned int _tmp288=_tmp286;unsigned int i;for(i=0;i < _tmp288;i ++){_tmp287[i]=({
void*_tmp289=(void*)((struct Cyc_List_List*)_check_null(_tmp281))->hd;_tmp281=
_tmp281->tl;_tmp289;});}}_tmp28A;});struct _tuple5 _tmp285=({struct _tuple5 _tmp282;
_tmp282.f1=_tmp280;_tmp282.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp283=_region_malloc(env->r,sizeof(*_tmp283));_tmp283[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp284;_tmp284.tag=4;_tmp284.f1=aggrdict;_tmp284;});_tmp283;});_tmp282;});
_npop_handler(0);return _tmp285;}};_pop_region(temp);}_LL109: if(*((int*)_tmpDA)!= 
31)goto _LL10B;_tmp118=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpDA)->f2;_LL10A: {
struct Cyc_List_List*fs;{void*_tmp28B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_List_List*_tmp28C;_LL1B5: if(_tmp28B <= (void*)
4)goto _LL1B7;if(*((int*)_tmp28B)!= 11)goto _LL1B7;_tmp28C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp28B)->f2;_LL1B6: fs=_tmp28C;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=({
struct Cyc_Core_Impossible_struct _tmp28E;_tmp28E.tag=Cyc_Core_Impossible;_tmp28E.f1=({
const char*_tmp28F="anal_Rexp:anon struct has bad type";_tag_dynforward(_tmp28F,
sizeof(char),_get_zero_arr_size(_tmp28F,35));});_tmp28E;});_tmp28D;}));_LL1B4:;}
_tmp119=_tmp118;_tmp11E=fs;goto _LL10C;}_LL10B: if(*((int*)_tmpDA)!= 30)goto _LL10D;
_tmp119=((struct Cyc_Absyn_Struct_e_struct*)_tmpDA)->f3;_tmp11A=((struct Cyc_Absyn_Struct_e_struct*)
_tmpDA)->f4;if(_tmp11A == 0)goto _LL10D;_tmp11B=*_tmp11A;_tmp11C=_tmp11B.impl;if(
_tmp11C == 0)goto _LL10D;_tmp11D=*_tmp11C;_tmp11E=_tmp11D.fields;_LL10C: {struct
_RegionHandle _tmp290=_new_region("temp");struct _RegionHandle*temp=& _tmp290;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp292;struct Cyc_List_List*
_tmp293;struct _tuple11 _tmp291=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple13*))Cyc_Core_snd,_tmp119),0);_tmp292=_tmp291.f1;_tmp293=_tmp291.f2;
_tmp292=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp292);{struct
_dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmpD3,_tmp11E,(
void*)_tmpD3->unknown_all);{int i=0;for(0;_tmp293 != 0;(((_tmp293=_tmp293->tl,
_tmp119=_tmp119->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple13*)((struct
Cyc_List_List*)_check_null(_tmp119))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp294=(void*)ds->hd;struct _dynforward_ptr*_tmp295;_LL1BA: if(*((int*)_tmp294)!= 
0)goto _LL1BC;_LL1BB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296[0]=({struct Cyc_Core_Impossible_struct
_tmp297;_tmp297.tag=Cyc_Core_Impossible;_tmp297.f1=({const char*_tmp298="anal_Rexp:Struct_e";
_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,19));});_tmp297;});
_tmp296;}));_LL1BC: if(*((int*)_tmp294)!= 1)goto _LL1B9;_tmp295=((struct Cyc_Absyn_FieldName_struct*)
_tmp294)->f1;_LL1BD: {int _tmp299=Cyc_NewControlFlow_get_field_index_fs(_tmp11E,
_tmp295);*((void**)_check_dynforward_subscript(aggrdict,sizeof(void*),_tmp299))=(
void*)_tmp293->hd;}_LL1B9:;}}}{struct _tuple5 _tmp29D=({struct _tuple5 _tmp29A;
_tmp29A.f1=_tmp292;_tmp29A.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp29B=_region_malloc(env->r,sizeof(*_tmp29B));_tmp29B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp29C;_tmp29C.tag=4;_tmp29C.f1=aggrdict;_tmp29C;});_tmp29B;});_tmp29A;});
_npop_handler(0);return _tmp29D;}}};_pop_region(temp);}_LL10D: if(*((int*)_tmpDA)
!= 30)goto _LL10F;_LL10E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E[0]=({struct Cyc_Core_Impossible_struct
_tmp29F;_tmp29F.tag=Cyc_Core_Impossible;_tmp29F.f1=({const char*_tmp2A0="anal_Rexp:missing aggrdeclimpl";
_tag_dynforward(_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,31));});_tmp29F;});
_tmp29E;}));_LL10F: if(*((int*)_tmpDA)!= 28)goto _LL111;_tmp11F=((struct Cyc_Absyn_Array_e_struct*)
_tmpDA)->f1;_LL110: {struct _RegionHandle _tmp2A1=_new_region("temp");struct
_RegionHandle*temp=& _tmp2A1;_push_region(temp);{struct Cyc_List_List*_tmp2A2=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple13*))Cyc_Core_snd,_tmp11F);union Cyc_CfFlowInfo_FlowInfo_union _tmp2A4;
struct Cyc_List_List*_tmp2A5;struct _tuple11 _tmp2A3=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp2A2,0);_tmp2A4=_tmp2A3.f1;_tmp2A5=_tmp2A3.f2;_tmp2A4=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2A4);for(0;_tmp2A5 != 0;(_tmp2A5=_tmp2A5->tl,_tmp2A2=_tmp2A2->tl)){
_tmp2A4=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2A2))->hd)->loc,_tmp2A4,(void*)_tmp2A5->hd);}{struct _tuple5
_tmp2A7=({struct _tuple5 _tmp2A6;_tmp2A6.f1=_tmp2A4;_tmp2A6.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmpD3,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmpD3->unknown_all);
_tmp2A6;});_npop_handler(0);return _tmp2A7;}};_pop_region(temp);}_LL111: if(*((int*)
_tmpDA)!= 29)goto _LL113;_tmp120=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpDA)->f1;
_tmp121=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpDA)->f2;_tmp122=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmpDA)->f3;_tmp123=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpDA)->f4;_LL112: {union Cyc_CfFlowInfo_FlowInfo_union _tmp2A9;void*_tmp2AA;
struct _tuple5 _tmp2A8=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp121);_tmp2A9=
_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;_tmp2A9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp121->loc,_tmp2A9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2AB=_tmp2A9;struct
Cyc_Dict_Dict _tmp2AC;struct Cyc_List_List*_tmp2AD;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2AE;_LL1BF: if((_tmp2AB.BottomFL).tag != 0)goto _LL1C1;_LL1C0: return({struct
_tuple5 _tmp2AF;_tmp2AF.f1=_tmp2A9;_tmp2AF.f2=(void*)_tmpD3->unknown_all;_tmp2AF;});
_LL1C1: if((_tmp2AB.ReachableFL).tag != 1)goto _LL1BE;_tmp2AC=(_tmp2AB.ReachableFL).f1;
_tmp2AD=(_tmp2AB.ReachableFL).f2;_tmp2AE=(_tmp2AB.ReachableFL).f3;_LL1C2: if(Cyc_CfFlowInfo_initlevel(
_tmp2AC,_tmp2AA)== (void*)0)({void*_tmp2B0[0]={};Cyc_Tcutil_terr(_tmp121->loc,({
const char*_tmp2B1="expression may not be initialized";_tag_dynforward(_tmp2B1,
sizeof(char),_get_zero_arr_size(_tmp2B1,34));}),_tag_dynforward(_tmp2B0,sizeof(
void*),0));});{struct Cyc_List_List*new_relns=_tmp2AD;comp_loop: {void*_tmp2B2=(
void*)_tmp121->r;struct Cyc_Absyn_Exp*_tmp2B3;void*_tmp2B4;struct Cyc_Absyn_Vardecl*
_tmp2B5;void*_tmp2B6;struct Cyc_Absyn_Vardecl*_tmp2B7;void*_tmp2B8;struct Cyc_Absyn_Vardecl*
_tmp2B9;void*_tmp2BA;struct Cyc_Absyn_Vardecl*_tmp2BB;union Cyc_Absyn_Cnst_union
_tmp2BC;int _tmp2BD;void*_tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List
_tmp2C0;struct Cyc_Absyn_Exp*_tmp2C1;_LL1C4: if(*((int*)_tmp2B2)!= 15)goto _LL1C6;
_tmp2B3=((struct Cyc_Absyn_Cast_e_struct*)_tmp2B2)->f2;_LL1C5: _tmp121=_tmp2B3;
goto comp_loop;_LL1C6: if(*((int*)_tmp2B2)!= 1)goto _LL1C8;_tmp2B4=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp2B2)->f2;if(_tmp2B4 <= (void*)1)goto _LL1C8;if(*((int*)
_tmp2B4)!= 0)goto _LL1C8;_tmp2B5=((struct Cyc_Absyn_Global_b_struct*)_tmp2B4)->f1;
if(!(!_tmp2B5->escapes))goto _LL1C8;_LL1C7: _tmp2B7=_tmp2B5;goto _LL1C9;_LL1C8: if(*((
int*)_tmp2B2)!= 1)goto _LL1CA;_tmp2B6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B2)->f2;if(_tmp2B6 <= (void*)1)goto _LL1CA;if(*((int*)_tmp2B6)!= 3)goto _LL1CA;
_tmp2B7=((struct Cyc_Absyn_Local_b_struct*)_tmp2B6)->f1;if(!(!_tmp2B7->escapes))
goto _LL1CA;_LL1C9: _tmp2B9=_tmp2B7;goto _LL1CB;_LL1CA: if(*((int*)_tmp2B2)!= 1)goto
_LL1CC;_tmp2B8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B2)->f2;if(_tmp2B8 <= (
void*)1)goto _LL1CC;if(*((int*)_tmp2B8)!= 4)goto _LL1CC;_tmp2B9=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2B8)->f1;if(!(!_tmp2B9->escapes))goto _LL1CC;_LL1CB: _tmp2BB=_tmp2B9;goto
_LL1CD;_LL1CC: if(*((int*)_tmp2B2)!= 1)goto _LL1CE;_tmp2BA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B2)->f2;if(_tmp2BA <= (void*)1)goto _LL1CE;if(*((int*)_tmp2BA)!= 2)goto _LL1CE;
_tmp2BB=((struct Cyc_Absyn_Param_b_struct*)_tmp2BA)->f1;if(!(!_tmp2BB->escapes))
goto _LL1CE;_LL1CD: new_relns=({struct Cyc_List_List*_tmp2C2=_region_malloc(env->r,
sizeof(*_tmp2C2));_tmp2C2->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2C3=_region_malloc(
env->r,sizeof(*_tmp2C3));_tmp2C3->vd=_tmp120;_tmp2C3->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2C4;(_tmp2C4.LessVar).tag=1;(_tmp2C4.LessVar).f1=
_tmp2BB;_tmp2C4;});_tmp2C3;});_tmp2C2->tl=_tmp2AD;_tmp2C2;});goto _LL1C3;_LL1CE:
if(*((int*)_tmp2B2)!= 0)goto _LL1D0;_tmp2BC=((struct Cyc_Absyn_Const_e_struct*)
_tmp2B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2B2)->f1).Int_c).tag != 2)
goto _LL1D0;_tmp2BD=(_tmp2BC.Int_c).f2;_LL1CF: new_relns=({struct Cyc_List_List*
_tmp2C5=_region_malloc(env->r,sizeof(*_tmp2C5));_tmp2C5->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2C6=_region_malloc(env->r,sizeof(*_tmp2C6));_tmp2C6->vd=_tmp120;_tmp2C6->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2C7;(
_tmp2C7.LessConst).tag=3;(_tmp2C7.LessConst).f1=(unsigned int)_tmp2BD;_tmp2C7;});
_tmp2C6;});_tmp2C5->tl=_tmp2AD;_tmp2C5;});goto _LL1C3;_LL1D0: if(*((int*)_tmp2B2)
!= 3)goto _LL1D2;_tmp2BE=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2B2)->f1;
if((int)_tmp2BE != 19)goto _LL1D2;_tmp2BF=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B2)->f2;if(_tmp2BF == 0)goto _LL1D2;_tmp2C0=*_tmp2BF;_tmp2C1=(struct Cyc_Absyn_Exp*)
_tmp2C0.hd;_LL1D1:{void*_tmp2C8=(void*)_tmp2C1->r;void*_tmp2C9;struct Cyc_Absyn_Vardecl*
_tmp2CA;void*_tmp2CB;struct Cyc_Absyn_Vardecl*_tmp2CC;void*_tmp2CD;struct Cyc_Absyn_Vardecl*
_tmp2CE;void*_tmp2CF;struct Cyc_Absyn_Vardecl*_tmp2D0;_LL1D5: if(*((int*)_tmp2C8)
!= 1)goto _LL1D7;_tmp2C9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C8)->f2;if(
_tmp2C9 <= (void*)1)goto _LL1D7;if(*((int*)_tmp2C9)!= 0)goto _LL1D7;_tmp2CA=((
struct Cyc_Absyn_Global_b_struct*)_tmp2C9)->f1;if(!(!_tmp2CA->escapes))goto _LL1D7;
_LL1D6: _tmp2CC=_tmp2CA;goto _LL1D8;_LL1D7: if(*((int*)_tmp2C8)!= 1)goto _LL1D9;
_tmp2CB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C8)->f2;if(_tmp2CB <= (void*)
1)goto _LL1D9;if(*((int*)_tmp2CB)!= 3)goto _LL1D9;_tmp2CC=((struct Cyc_Absyn_Local_b_struct*)
_tmp2CB)->f1;if(!(!_tmp2CC->escapes))goto _LL1D9;_LL1D8: _tmp2CE=_tmp2CC;goto
_LL1DA;_LL1D9: if(*((int*)_tmp2C8)!= 1)goto _LL1DB;_tmp2CD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C8)->f2;if(_tmp2CD <= (void*)1)goto _LL1DB;if(*((int*)_tmp2CD)!= 4)goto _LL1DB;
_tmp2CE=((struct Cyc_Absyn_Pat_b_struct*)_tmp2CD)->f1;if(!(!_tmp2CE->escapes))
goto _LL1DB;_LL1DA: _tmp2D0=_tmp2CE;goto _LL1DC;_LL1DB: if(*((int*)_tmp2C8)!= 1)goto
_LL1DD;_tmp2CF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C8)->f2;if(_tmp2CF <= (
void*)1)goto _LL1DD;if(*((int*)_tmp2CF)!= 2)goto _LL1DD;_tmp2D0=((struct Cyc_Absyn_Param_b_struct*)
_tmp2CF)->f1;if(!(!_tmp2D0->escapes))goto _LL1DD;_LL1DC: new_relns=({struct Cyc_List_List*
_tmp2D1=_region_malloc(env->r,sizeof(*_tmp2D1));_tmp2D1->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2D2=_region_malloc(env->r,sizeof(*_tmp2D2));_tmp2D2->vd=_tmp120;_tmp2D2->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2D3;(
_tmp2D3.LessSize).tag=2;(_tmp2D3.LessSize).f1=_tmp2D0;_tmp2D3;});_tmp2D2;});
_tmp2D1->tl=_tmp2AD;_tmp2D1;});goto _LL1D4;_LL1DD:;_LL1DE: goto _LL1D4;_LL1D4:;}
goto _LL1C3;_LL1D2:;_LL1D3: goto _LL1C3;_LL1C3:;}if(_tmp2AD != new_relns)_tmp2A9=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp2D4;(
_tmp2D4.ReachableFL).tag=1;(_tmp2D4.ReachableFL).f1=_tmp2AC;(_tmp2D4.ReachableFL).f2=
new_relns;(_tmp2D4.ReachableFL).f3=_tmp2AE;_tmp2D4;});{void*_tmp2D5=_tmp2AA;
_LL1E0: if((int)_tmp2D5 != 0)goto _LL1E2;_LL1E1: return({struct _tuple5 _tmp2D6;
_tmp2D6.f1=_tmp2A9;_tmp2D6.f2=(void*)_tmpD3->unknown_all;_tmp2D6;});_LL1E2: if((
int)_tmp2D5 != 2)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp2D5 != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp2D5 <= (void*)3)goto _LL1E8;if(*((int*)
_tmp2D5)!= 2)goto _LL1E8;_LL1E7: {struct Cyc_List_List _tmp2D7=({struct Cyc_List_List
_tmp2E3;_tmp2E3.hd=_tmp120;_tmp2E3.tl=0;_tmp2E3;});_tmp2A9=Cyc_NewControlFlow_add_vars(
_tmpD3,_tmp2A9,(struct Cyc_List_List*)& _tmp2D7,(void*)_tmpD3->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp2D9;void*_tmp2DA;struct _tuple5 _tmp2D8=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2A9,_tmp122);_tmp2D9=_tmp2D8.f1;_tmp2DA=_tmp2D8.f2;_tmp2D9=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp122->loc,_tmp2D9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DB=_tmp2D9;struct
Cyc_Dict_Dict _tmp2DC;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2DD;_LL1EB: if((_tmp2DB.BottomFL).tag
!= 0)goto _LL1ED;_LL1EC: return({struct _tuple5 _tmp2DE;_tmp2DE.f1=_tmp2D9;_tmp2DE.f2=(
void*)_tmpD3->unknown_all;_tmp2DE;});_LL1ED: if((_tmp2DB.ReachableFL).tag != 1)
goto _LL1EA;_tmp2DC=(_tmp2DB.ReachableFL).f1;_tmp2DD=(_tmp2DB.ReachableFL).f3;
_LL1EE: if(Cyc_CfFlowInfo_initlevel(_tmp2DC,_tmp2DA)!= (void*)2){({void*_tmp2DF[0]={};
Cyc_Tcutil_terr(_tmp121->loc,({const char*_tmp2E0="expression may not be initialized";
_tag_dynforward(_tmp2E0,sizeof(char),_get_zero_arr_size(_tmp2E0,34));}),
_tag_dynforward(_tmp2DF,sizeof(void*),0));});return({struct _tuple5 _tmp2E1;
_tmp2E1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2E2;(_tmp2E2.BottomFL).tag=0;_tmp2E2;});_tmp2E1.f2=(void*)_tmpD3->unknown_all;
_tmp2E1;});}_LL1EA:;}_tmp2A9=_tmp2D9;goto _LL1E9;}}_LL1E8:;_LL1E9: while(1){struct
Cyc_List_List _tmp2E4=({struct Cyc_List_List _tmp2F0;_tmp2F0.hd=_tmp120;_tmp2F0.tl=
0;_tmp2F0;});_tmp2A9=Cyc_NewControlFlow_add_vars(_tmpD3,_tmp2A9,(struct Cyc_List_List*)&
_tmp2E4,(void*)_tmpD3->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2E6;void*_tmp2E7;struct _tuple5 _tmp2E5=Cyc_NewControlFlow_anal_Rexp(env,
_tmp2A9,_tmp122);_tmp2E6=_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;_tmp2E6=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp122->loc,_tmp2E6);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2E8=_tmp2E6;struct
Cyc_Dict_Dict _tmp2E9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2EA;_LL1F0: if((_tmp2E8.BottomFL).tag
!= 0)goto _LL1F2;_LL1F1: goto _LL1EF;_LL1F2: if((_tmp2E8.ReachableFL).tag != 1)goto
_LL1EF;_tmp2E9=(_tmp2E8.ReachableFL).f1;_tmp2EA=(_tmp2E8.ReachableFL).f3;_LL1F3:
if(Cyc_CfFlowInfo_initlevel(_tmp2E9,_tmp2E7)!= (void*)2){({void*_tmp2EB[0]={};
Cyc_Tcutil_terr(_tmp121->loc,({const char*_tmp2EC="expression may not be initialized";
_tag_dynforward(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,34));}),
_tag_dynforward(_tmp2EB,sizeof(void*),0));});return({struct _tuple5 _tmp2ED;
_tmp2ED.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp2EE;(_tmp2EE.BottomFL).tag=0;_tmp2EE;});_tmp2ED.f2=(void*)_tmpD3->unknown_all;
_tmp2ED;});}_LL1EF:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp2EF=Cyc_CfFlowInfo_join_flow(
_tmpD3,env->all_changed,_tmp2A9,_tmp2E6,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp2EF,_tmp2A9))break;_tmp2A9=_tmp2EF;}}}return({struct _tuple5 _tmp2F1;_tmp2F1.f1=
_tmp2A9;_tmp2F1.f2=(void*)_tmpD3->unknown_all;_tmp2F1;});_LL1DF:;}}_LL1BE:;}}
_LL113: if(*((int*)_tmpDA)!= 38)goto _LL115;_tmp124=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpDA)->f1;_LL114: while(1){union Cyc_CfFlowInfo_FlowInfo_union*_tmp2F3;struct
_tuple9 _tmp2F2=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp124);_tmp2F3=
_tmp2F2.f2;inflow=*_tmp2F3;{void*_tmp2F4=(void*)_tmp124->r;struct Cyc_Absyn_Stmt*
_tmp2F5;struct Cyc_Absyn_Stmt*_tmp2F6;struct Cyc_Absyn_Decl*_tmp2F7;struct Cyc_Absyn_Stmt*
_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;_LL1F5: if(_tmp2F4 <= (void*)1)goto _LL1FB;if(*((
int*)_tmp2F4)!= 1)goto _LL1F7;_tmp2F5=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F4)->f1;
_tmp2F6=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F4)->f2;_LL1F6: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp2F5);_tmp124=_tmp2F6;goto _LL1F4;_LL1F7: if(*((int*)_tmp2F4)!= 11)
goto _LL1F9;_tmp2F7=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F4)->f1;_tmp2F8=((
struct Cyc_Absyn_Decl_s_struct*)_tmp2F4)->f2;_LL1F8: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp2F7);_tmp124=_tmp2F8;goto _LL1F4;_LL1F9: if(*((int*)_tmp2F4)!= 0)
goto _LL1FB;_tmp2F9=((struct Cyc_Absyn_Exp_s_struct*)_tmp2F4)->f1;_LL1FA: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp2F9);_LL1FB:;_LL1FC:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA[0]=({struct Cyc_Core_Impossible_struct
_tmp2FB;_tmp2FB.tag=Cyc_Core_Impossible;_tmp2FB.f1=({const char*_tmp2FC="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp2FC,sizeof(char),_get_zero_arr_size(_tmp2FC,33));});_tmp2FB;});
_tmp2FA;}));_LL1F4:;}}_LL115: if(*((int*)_tmpDA)!= 1)goto _LL117;_tmp125=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpDA)->f2;if((int)_tmp125 != 0)goto _LL117;_LL116:
goto _LL118;_LL117: if(*((int*)_tmpDA)!= 2)goto _LL119;_LL118: goto _LL11A;_LL119: if(*((
int*)_tmpDA)!= 10)goto _LL11B;_LL11A: goto _LL11C;_LL11B: if(*((int*)_tmpDA)!= 37)
goto _LL11D;_LL11C: goto _LL11E;_LL11D: if(*((int*)_tmpDA)!= 27)goto _LLB8;_LL11E:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2FD=_cycalloc(sizeof(*
_tmp2FD));_tmp2FD[0]=({struct Cyc_Core_Impossible_struct _tmp2FE;_tmp2FE.tag=Cyc_Core_Impossible;
_tmp2FE.f1=({const char*_tmp2FF="anal_Rexp, unexpected exp form";_tag_dynforward(
_tmp2FF,sizeof(char),_get_zero_arr_size(_tmp2FF,31));});_tmp2FE;});_tmp2FD;}));
_LLB8:;}}static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,
struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*
_tmp300=env->fenv;void*_tmp301=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp302;void*_tmp303;struct Cyc_Absyn_PtrAtts
_tmp304;struct Cyc_Absyn_Conref*_tmp305;struct Cyc_Absyn_Conref*_tmp306;_LL1FE: if(
_tmp301 <= (void*)4)goto _LL200;if(*((int*)_tmp301)!= 4)goto _LL200;_tmp302=((
struct Cyc_Absyn_PointerType_struct*)_tmp301)->f1;_tmp303=(void*)_tmp302.elt_typ;
_tmp304=_tmp302.ptr_atts;_tmp305=_tmp304.bounds;_tmp306=_tmp304.zero_term;_LL1FF: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp307=f;struct Cyc_Dict_Dict _tmp308;struct Cyc_List_List*
_tmp309;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30A;_LL203: if((_tmp307.BottomFL).tag
!= 0)goto _LL205;_LL204: return({struct _tuple7 _tmp30B;_tmp30B.f1=f;_tmp30B.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp30C;(
_tmp30C.UnknownL).tag=1;_tmp30C;});_tmp30B;});_LL205: if((_tmp307.ReachableFL).tag
!= 1)goto _LL202;_tmp308=(_tmp307.ReachableFL).f1;_tmp309=(_tmp307.ReachableFL).f2;
_tmp30A=(_tmp307.ReachableFL).f3;_LL206: if(Cyc_Tcutil_is_bound_one(_tmp305)){
void*_tmp30D=r;struct Cyc_CfFlowInfo_Place*_tmp30E;struct Cyc_CfFlowInfo_Place
_tmp30F;void*_tmp310;struct Cyc_List_List*_tmp311;void*_tmp312;_LL208: if((int)
_tmp30D != 1)goto _LL20A;_LL209: goto _LL20B;_LL20A: if((int)_tmp30D != 2)goto _LL20C;
_LL20B:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp314;_tmp314.tag=Cyc_CfFlowInfo_NotZero;_tmp314.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp309);_tmp314;});_tmp313;})));goto _LL207;_LL20C: if(
_tmp30D <= (void*)3)goto _LL20E;if(*((int*)_tmp30D)!= 2)goto _LL20E;_tmp30E=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp30D)->f1;_tmp30F=*_tmp30E;_tmp310=(
void*)_tmp30F.root;_tmp311=_tmp30F.fields;_LL20D:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp316;_tmp316.tag=Cyc_CfFlowInfo_NotZero;
_tmp316.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp309);_tmp316;});
_tmp315;})));return({struct _tuple7 _tmp317;_tmp317.f1=f;_tmp317.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp318;(_tmp318.PlaceL).tag=0;(_tmp318.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp319=_region_malloc(_tmp300->r,sizeof(*_tmp319));
_tmp319->root=(void*)_tmp310;_tmp319->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp300->r,_tmp311,flds);_tmp319;});_tmp318;});_tmp317;});_LL20E: if((int)_tmp30D
!= 0)goto _LL210;_LL20F:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp31A;_tmp31A.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp31C;(_tmp31C.BottomFL).tag=0;_tmp31C;});
_tmp31A.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp31B;(_tmp31B.UnknownL).tag=1;_tmp31B;});_tmp31A;});_LL210: if(_tmp30D <= (void*)
3)goto _LL212;if(*((int*)_tmp30D)!= 0)goto _LL212;_tmp312=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp30D)->f1;_LL211: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp312);goto
_LL213;_LL212:;_LL213:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp31E;_tmp31E.tag=Cyc_CfFlowInfo_UnknownZ;_tmp31E.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp309);_tmp31E;});_tmp31D;})));_LL207:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp31F=_cycalloc(sizeof(*
_tmp31F));_tmp31F[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp320;_tmp320.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp320.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp309);_tmp320;});_tmp31F;})));}if(Cyc_CfFlowInfo_initlevel(_tmp308,r)== (void*)
0)({void*_tmp321[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp322="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp322,sizeof(char),_get_zero_arr_size(_tmp322,46));}),
_tag_dynforward(_tmp321,sizeof(void*),0));});return({struct _tuple7 _tmp323;
_tmp323.f1=f;_tmp323.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp324;(_tmp324.UnknownL).tag=1;_tmp324;});_tmp323;});_LL202:;}_LL200:;_LL201:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp325=_cycalloc(sizeof(*
_tmp325));_tmp325[0]=({struct Cyc_Core_Impossible_struct _tmp326;_tmp326.tag=Cyc_Core_Impossible;
_tmp326.f1=({const char*_tmp327="left deref of non-pointer-type";_tag_dynforward(
_tmp327,sizeof(char),_get_zero_arr_size(_tmp327,31));});_tmp326;});_tmp325;}));
_LL1FD:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp328=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp329=inflow;struct Cyc_Dict_Dict _tmp32A;struct Cyc_List_List*
_tmp32B;_LL215: if((_tmp329.BottomFL).tag != 0)goto _LL217;_LL216: return({struct
_tuple7 _tmp32C;_tmp32C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp32E;(_tmp32E.BottomFL).tag=0;_tmp32E;});_tmp32C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp32D;(_tmp32D.UnknownL).tag=1;_tmp32D;});
_tmp32C;});_LL217: if((_tmp329.ReachableFL).tag != 1)goto _LL214;_tmp32A=(_tmp329.ReachableFL).f1;
_tmp32B=(_tmp329.ReachableFL).f2;_LL218: d=_tmp32A;_LL214:;}{void*_tmp32F=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp330;struct Cyc_Absyn_Exp*_tmp331;struct Cyc_Absyn_Exp*
_tmp332;void*_tmp333;struct Cyc_Absyn_Vardecl*_tmp334;void*_tmp335;struct Cyc_Absyn_Vardecl*
_tmp336;void*_tmp337;struct Cyc_Absyn_Vardecl*_tmp338;void*_tmp339;struct Cyc_Absyn_Vardecl*
_tmp33A;struct Cyc_Absyn_Exp*_tmp33B;struct _dynforward_ptr*_tmp33C;struct Cyc_Absyn_Exp*
_tmp33D;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*
_tmp340;struct _dynforward_ptr*_tmp341;_LL21A: if(*((int*)_tmp32F)!= 15)goto _LL21C;
_tmp330=((struct Cyc_Absyn_Cast_e_struct*)_tmp32F)->f2;_LL21B: _tmp331=_tmp330;
goto _LL21D;_LL21C: if(*((int*)_tmp32F)!= 13)goto _LL21E;_tmp331=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp32F)->f1;_LL21D: _tmp332=_tmp331;goto _LL21F;_LL21E: if(*((int*)_tmp32F)!= 14)
goto _LL220;_tmp332=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp32F)->f1;_LL21F:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp332,flds);_LL220: if(*((int*)
_tmp32F)!= 1)goto _LL222;_tmp333=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp32F)->f2;
if(_tmp333 <= (void*)1)goto _LL222;if(*((int*)_tmp333)!= 2)goto _LL222;_tmp334=((
struct Cyc_Absyn_Param_b_struct*)_tmp333)->f1;_LL221: _tmp336=_tmp334;goto _LL223;
_LL222: if(*((int*)_tmp32F)!= 1)goto _LL224;_tmp335=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp32F)->f2;if(_tmp335 <= (void*)1)goto _LL224;if(*((int*)_tmp335)!= 3)goto _LL224;
_tmp336=((struct Cyc_Absyn_Local_b_struct*)_tmp335)->f1;_LL223: _tmp338=_tmp336;
goto _LL225;_LL224: if(*((int*)_tmp32F)!= 1)goto _LL226;_tmp337=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp32F)->f2;if(_tmp337 <= (void*)1)goto _LL226;if(*((int*)_tmp337)!= 4)goto _LL226;
_tmp338=((struct Cyc_Absyn_Pat_b_struct*)_tmp337)->f1;_LL225: return({struct
_tuple7 _tmp342;_tmp342.f1=inflow;_tmp342.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp343;(_tmp343.PlaceL).tag=0;(_tmp343.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp344=_region_malloc(env->r,sizeof(*_tmp344));
_tmp344->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp345=
_region_malloc(env->r,sizeof(*_tmp345));_tmp345[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp346;_tmp346.tag=0;_tmp346.f1=_tmp338;_tmp346;});_tmp345;}));_tmp344->fields=
flds;_tmp344;});_tmp343;});_tmp342;});_LL226: if(*((int*)_tmp32F)!= 1)goto _LL228;
_tmp339=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp32F)->f2;if(_tmp339 <= (void*)
1)goto _LL228;if(*((int*)_tmp339)!= 0)goto _LL228;_tmp33A=((struct Cyc_Absyn_Global_b_struct*)
_tmp339)->f1;_LL227: return({struct _tuple7 _tmp347;_tmp347.f1=inflow;_tmp347.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp348;(
_tmp348.UnknownL).tag=1;_tmp348;});_tmp347;});_LL228: if(*((int*)_tmp32F)!= 24)
goto _LL22A;_tmp33B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp32F)->f1;_tmp33C=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp32F)->f2;_LL229:{void*_tmp349=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp33B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp34A;void*_tmp34B;_LL233: if(_tmp349 <= (void*)4)goto _LL235;if(*((int*)_tmp349)
!= 4)goto _LL235;_tmp34A=((struct Cyc_Absyn_PointerType_struct*)_tmp349)->f1;
_tmp34B=(void*)_tmp34A.elt_typ;_LL234: if(!Cyc_Absyn_is_union_type(_tmp34B))flds=({
struct Cyc_List_List*_tmp34C=_region_malloc(env->r,sizeof(*_tmp34C));_tmp34C->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp34B,_tmp33C);_tmp34C->tl=flds;
_tmp34C;});goto _LL232;_LL235:;_LL236:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D[0]=({struct Cyc_Core_Impossible_struct
_tmp34E;_tmp34E.tag=Cyc_Core_Impossible;_tmp34E.f1=({const char*_tmp34F="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp34F,sizeof(char),_get_zero_arr_size(_tmp34F,25));});_tmp34E;});
_tmp34D;}));_LL232:;}_tmp33D=_tmp33B;goto _LL22B;_LL22A: if(*((int*)_tmp32F)!= 22)
goto _LL22C;_tmp33D=((struct Cyc_Absyn_Deref_e_struct*)_tmp32F)->f1;_LL22B: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp351;void*_tmp352;struct _tuple5 _tmp350=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp33D);_tmp351=_tmp350.f1;_tmp352=_tmp350.f2;_tmp351=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp351);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp351,_tmp33D,
_tmp352,flds);}_LL22C: if(*((int*)_tmp32F)!= 25)goto _LL22E;_tmp33E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp32F)->f1;_tmp33F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp32F)->f2;_LL22D: {
void*_tmp353=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp33E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp354;struct Cyc_Absyn_PtrAtts
_tmp355;struct Cyc_Absyn_Conref*_tmp356;_LL238: if(_tmp353 <= (void*)4)goto _LL23C;
if(*((int*)_tmp353)!= 9)goto _LL23A;_LL239: {unsigned int _tmp357=(Cyc_Evexp_eval_const_uint_exp(
_tmp33F)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp33E,({struct
Cyc_List_List*_tmp358=_region_malloc(env->r,sizeof(*_tmp358));_tmp358->hd=(void*)((
int)_tmp357);_tmp358->tl=flds;_tmp358;}));}_LL23A: if(*((int*)_tmp353)!= 4)goto
_LL23C;_tmp354=((struct Cyc_Absyn_PointerType_struct*)_tmp353)->f1;_tmp355=
_tmp354.ptr_atts;_tmp356=_tmp355.bounds;_LL23B: {struct _RegionHandle _tmp359=
_new_region("temp");struct _RegionHandle*temp=& _tmp359;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp35C;struct Cyc_List_List*_tmp35D;struct _tuple11
_tmp35B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp35A[2];_tmp35A[1]=_tmp33F;_tmp35A[0]=_tmp33E;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp35A,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp35C=_tmp35B.f1;_tmp35D=
_tmp35B.f2;_tmp35C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp33F->loc,_tmp35C);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp35E=_tmp35C;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp35F=_tmp35C;struct Cyc_Dict_Dict _tmp360;struct Cyc_List_List*_tmp361;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp362;_LL23F: if((_tmp35F.ReachableFL).tag != 1)goto _LL241;_tmp360=(_tmp35F.ReachableFL).f1;
_tmp361=(_tmp35F.ReachableFL).f2;_tmp362=(_tmp35F.ReachableFL).f3;_LL240: if(Cyc_CfFlowInfo_initlevel(
_tmp360,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp35D))->tl))->hd)== (void*)0)({void*_tmp363[0]={};Cyc_Tcutil_terr(
_tmp33F->loc,({const char*_tmp364="expression may not be initialized";
_tag_dynforward(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,34));}),
_tag_dynforward(_tmp363,sizeof(void*),0));});{struct Cyc_List_List*_tmp365=Cyc_NewControlFlow_add_subscript_reln(
_tmp328->r,_tmp361,_tmp33E,_tmp33F);if(_tmp361 != _tmp365)_tmp35E=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp366;(_tmp366.ReachableFL).tag=1;(_tmp366.ReachableFL).f1=
_tmp360;(_tmp366.ReachableFL).f2=_tmp365;(_tmp366.ReachableFL).f3=_tmp362;
_tmp366;});goto _LL23E;}_LL241:;_LL242: goto _LL23E;_LL23E:;}{union Cyc_Absyn_Constraint_union
_tmp367=(Cyc_Absyn_compress_conref(_tmp356))->v;void*_tmp368;_LL244: if((_tmp367.Eq_constr).tag
!= 0)goto _LL246;_tmp368=(_tmp367.Eq_constr).f1;if(_tmp368 <= (void*)2)goto _LL246;
if(*((int*)_tmp368)!= 1)goto _LL246;_LL245:{void*_tmp369=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp35D))->tl))->hd;struct Cyc_List_List*
_tmp36A;_LL249: if(_tmp369 <= (void*)3)goto _LL24B;if(*((int*)_tmp369)!= 3)goto
_LL24B;_tmp36A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp369)->f1;_LL24A:(void*)(
_tmp33F->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp36B=
_cycalloc(sizeof(*_tmp36B));_tmp36B[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp36C;_tmp36C.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp36C.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp36A);_tmp36C;});_tmp36B;})));goto _LL248;_LL24B:;_LL24C:
goto _LL248;_LL248:;}goto _LL243;_LL246:;_LL247: goto _LL243;_LL243:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp36E;union Cyc_CfFlowInfo_AbsLVal_union _tmp36F;struct _tuple7 _tmp36D=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp35C,_tmp33E,(void*)((struct Cyc_List_List*)_check_null(_tmp35D))->hd,
flds);_tmp36E=_tmp36D.f1;_tmp36F=_tmp36D.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp370=_tmp36E;_LL24E: if((_tmp370.BottomFL).tag != 0)goto _LL250;_LL24F: {struct
_tuple7 _tmp372=({struct _tuple7 _tmp371;_tmp371.f1=_tmp36E;_tmp371.f2=_tmp36F;
_tmp371;});_npop_handler(0);return _tmp372;}_LL250:;_LL251: {struct _tuple7 _tmp374=({
struct _tuple7 _tmp373;_tmp373.f1=_tmp35E;_tmp373.f2=_tmp36F;_tmp373;});
_npop_handler(0);return _tmp374;}_LL24D:;}}}};_pop_region(temp);}_LL23C:;_LL23D:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp375=_cycalloc(sizeof(*
_tmp375));_tmp375[0]=({struct Cyc_Core_Impossible_struct _tmp376;_tmp376.tag=Cyc_Core_Impossible;
_tmp376.f1=({const char*_tmp377="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp377,sizeof(char),_get_zero_arr_size(_tmp377,33));});_tmp376;});
_tmp375;}));_LL237:;}_LL22E: if(*((int*)_tmp32F)!= 23)goto _LL230;_tmp340=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp32F)->f1;_tmp341=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp32F)->f2;_LL22F: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp340->topt))->v))return({struct _tuple7 _tmp378;_tmp378.f1=inflow;
_tmp378.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp379;(_tmp379.UnknownL).tag=1;_tmp379;});_tmp378;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp340,({struct Cyc_List_List*_tmp37A=_region_malloc(env->r,sizeof(*
_tmp37A));_tmp37A->hd=(void*)Cyc_NewControlFlow_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp340->topt))->v,_tmp341);_tmp37A->tl=flds;_tmp37A;}));
_LL230:;_LL231: return({struct _tuple7 _tmp37B;_tmp37B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp37D;(_tmp37D.BottomFL).tag=0;_tmp37D;});
_tmp37B.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp37C;(_tmp37C.UnknownL).tag=1;_tmp37C;});_tmp37B;});_LL219:;}}static struct
_tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp37F;union Cyc_CfFlowInfo_AbsLVal_union _tmp380;struct _tuple7 _tmp37E=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp37F=_tmp37E.f1;_tmp380=_tmp37E.f2;return({struct _tuple7
_tmp381;_tmp381.f1=_tmp37F;_tmp381.f2=_tmp380;_tmp381;});}static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp382=env->fenv;{void*
_tmp383=(void*)e->r;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_Absyn_Exp*_tmp385;
struct Cyc_Absyn_Exp*_tmp386;struct Cyc_Absyn_Exp*_tmp387;struct Cyc_Absyn_Exp*
_tmp388;struct Cyc_Absyn_Exp*_tmp389;struct Cyc_Absyn_Exp*_tmp38A;struct Cyc_Absyn_Exp*
_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;void*_tmp38D;struct Cyc_List_List*_tmp38E;
struct Cyc_List_List _tmp38F;struct Cyc_Absyn_Exp*_tmp390;struct Cyc_List_List*
_tmp391;void*_tmp392;struct Cyc_List_List*_tmp393;_LL253: if(*((int*)_tmp383)!= 6)
goto _LL255;_tmp384=((struct Cyc_Absyn_Conditional_e_struct*)_tmp383)->f1;_tmp385=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp383)->f2;_tmp386=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp383)->f3;_LL254: {union Cyc_CfFlowInfo_FlowInfo_union _tmp395;union Cyc_CfFlowInfo_FlowInfo_union
_tmp396;struct _tuple8 _tmp394=Cyc_NewControlFlow_anal_test(env,inflow,_tmp384);
_tmp395=_tmp394.f1;_tmp396=_tmp394.f2;_tmp395=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp384->loc,_tmp395);_tmp396=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp384->loc,
_tmp396);{union Cyc_CfFlowInfo_FlowInfo_union _tmp398;union Cyc_CfFlowInfo_FlowInfo_union
_tmp399;struct _tuple8 _tmp397=Cyc_NewControlFlow_anal_test(env,_tmp395,_tmp385);
_tmp398=_tmp397.f1;_tmp399=_tmp397.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp39B;
union Cyc_CfFlowInfo_FlowInfo_union _tmp39C;struct _tuple8 _tmp39A=Cyc_NewControlFlow_anal_test(
env,_tmp396,_tmp386);_tmp39B=_tmp39A.f1;_tmp39C=_tmp39A.f2;return({struct _tuple8
_tmp39D;_tmp39D.f1=Cyc_CfFlowInfo_join_flow(_tmp382,env->all_changed,_tmp398,
_tmp39B,1);_tmp39D.f2=Cyc_CfFlowInfo_join_flow(_tmp382,env->all_changed,_tmp399,
_tmp39C,1);_tmp39D;});}}}_LL255: if(*((int*)_tmp383)!= 7)goto _LL257;_tmp387=((
struct Cyc_Absyn_And_e_struct*)_tmp383)->f1;_tmp388=((struct Cyc_Absyn_And_e_struct*)
_tmp383)->f2;_LL256: {union Cyc_CfFlowInfo_FlowInfo_union _tmp39F;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A0;struct _tuple8 _tmp39E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp387);
_tmp39F=_tmp39E.f1;_tmp3A0=_tmp39E.f2;_tmp39F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp387->loc,_tmp39F);_tmp3A0=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp387->loc,
_tmp3A0);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3A2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A3;struct _tuple8 _tmp3A1=Cyc_NewControlFlow_anal_test(env,_tmp39F,_tmp388);
_tmp3A2=_tmp3A1.f1;_tmp3A3=_tmp3A1.f2;_tmp3A2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp388->loc,_tmp3A2);_tmp3A3=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp388->loc,
_tmp3A3);return({struct _tuple8 _tmp3A4;_tmp3A4.f1=_tmp3A2;_tmp3A4.f2=Cyc_CfFlowInfo_join_flow(
_tmp382,env->all_changed,_tmp3A0,_tmp3A3,0);_tmp3A4;});}}_LL257: if(*((int*)
_tmp383)!= 8)goto _LL259;_tmp389=((struct Cyc_Absyn_Or_e_struct*)_tmp383)->f1;
_tmp38A=((struct Cyc_Absyn_Or_e_struct*)_tmp383)->f2;_LL258: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A6;union Cyc_CfFlowInfo_FlowInfo_union _tmp3A7;struct _tuple8 _tmp3A5=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp389);_tmp3A6=_tmp3A5.f1;_tmp3A7=_tmp3A5.f2;_tmp3A6=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp389->loc,_tmp3A6);_tmp3A7=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp389->loc,
_tmp3A7);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3A9;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AA;struct _tuple8 _tmp3A8=Cyc_NewControlFlow_anal_test(env,_tmp3A7,_tmp38A);
_tmp3A9=_tmp3A8.f1;_tmp3AA=_tmp3A8.f2;_tmp3A9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp38A->loc,_tmp3A9);_tmp3AA=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp38A->loc,
_tmp3AA);return({struct _tuple8 _tmp3AB;_tmp3AB.f1=Cyc_CfFlowInfo_join_flow(
_tmp382,env->all_changed,_tmp3A6,_tmp3A9,0);_tmp3AB.f2=_tmp3AA;_tmp3AB;});}}
_LL259: if(*((int*)_tmp383)!= 9)goto _LL25B;_tmp38B=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp383)->f1;_tmp38C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp383)->f2;_LL25A: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3AD;void*_tmp3AE;struct _tuple5 _tmp3AC=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp38B);_tmp3AD=_tmp3AC.f1;_tmp3AE=_tmp3AC.f2;_tmp3AD=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp38B->loc,_tmp3AD);return Cyc_NewControlFlow_anal_test(env,_tmp3AD,_tmp38C);}
_LL25B: if(*((int*)_tmp383)!= 3)goto _LL25D;_tmp38D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp383)->f1;if((int)_tmp38D != 11)goto _LL25D;_tmp38E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp383)->f2;if(_tmp38E == 0)goto _LL25D;_tmp38F=*_tmp38E;_tmp390=(struct Cyc_Absyn_Exp*)
_tmp38F.hd;_tmp391=_tmp38F.tl;if(_tmp391 != 0)goto _LL25D;_LL25C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3B0;union Cyc_CfFlowInfo_FlowInfo_union _tmp3B1;struct _tuple8 _tmp3AF=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp390);_tmp3B0=_tmp3AF.f1;_tmp3B1=_tmp3AF.f2;return({struct _tuple8
_tmp3B2;_tmp3B2.f1=_tmp3B1;_tmp3B2.f2=_tmp3B0;_tmp3B2;});}_LL25D: if(*((int*)
_tmp383)!= 3)goto _LL25F;_tmp392=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp383)->f1;_tmp393=((struct Cyc_Absyn_Primop_e_struct*)_tmp383)->f2;_LL25E: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3B3=
_new_region("temp");struct _RegionHandle*temp=& _tmp3B3;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3B5;struct Cyc_List_List*_tmp3B6;struct _tuple11
_tmp3B4=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp393,0);
_tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3B6))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3B6->tl))->hd;
f=_tmp3B5;};_pop_region(temp);}{void*_tmp3B7=_tmp392;_LL262: if((int)_tmp3B7 != 0)
goto _LL264;_LL263: goto _LL265;_LL264: if((int)_tmp3B7 != 2)goto _LL266;_LL265: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp393))->hd)->loc,f);
goto _LL261;_LL266:;_LL267: f=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp393))->hd)->loc,f);goto _LL261;_LL261:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp3B8=f;struct Cyc_Dict_Dict _tmp3B9;struct Cyc_List_List*
_tmp3BA;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3BB;_LL269: if((_tmp3B8.BottomFL).tag
!= 0)goto _LL26B;_LL26A: return({struct _tuple8 _tmp3BC;_tmp3BC.f1=f;_tmp3BC.f2=f;
_tmp3BC;});_LL26B: if((_tmp3B8.ReachableFL).tag != 1)goto _LL268;_tmp3B9=(_tmp3B8.ReachableFL).f1;
_tmp3BA=(_tmp3B8.ReachableFL).f2;_tmp3BB=(_tmp3B8.ReachableFL).f3;_LL26C: {
struct Cyc_Absyn_Exp*_tmp3BD=(struct Cyc_Absyn_Exp*)_tmp393->hd;struct Cyc_Absyn_Exp*
_tmp3BE=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp393->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp3B9,r1)== (void*)0)({void*_tmp3BF[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp393->hd)->loc,({const char*_tmp3C0="expression may not be initialized";
_tag_dynforward(_tmp3C0,sizeof(char),_get_zero_arr_size(_tmp3C0,34));}),
_tag_dynforward(_tmp3BF,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp3B9,
r2)== (void*)0)({void*_tmp3C1[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp393->tl))->hd)->loc,({const char*_tmp3C2="expression may not be initialized";
_tag_dynforward(_tmp3C2,sizeof(char),_get_zero_arr_size(_tmp3C2,34));}),
_tag_dynforward(_tmp3C1,sizeof(void*),0));});if(_tmp392 == (void*)5  || _tmp392 == (
void*)6){struct _tuple0 _tmp3C4=({struct _tuple0 _tmp3C3;_tmp3C3.f1=r1;_tmp3C3.f2=r2;
_tmp3C3;});void*_tmp3C5;void*_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*
_tmp3CA;void*_tmp3CB;void*_tmp3CC;void*_tmp3CD;void*_tmp3CE;void*_tmp3CF;void*
_tmp3D0;void*_tmp3D1;void*_tmp3D2;void*_tmp3D3;void*_tmp3D4;void*_tmp3D5;void*
_tmp3D6;void*_tmp3D7;void*_tmp3D8;_LL26E: _tmp3C5=_tmp3C4.f1;if(_tmp3C5 <= (void*)
3)goto _LL270;if(*((int*)_tmp3C5)!= 0)goto _LL270;_tmp3C6=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3C5)->f1;_tmp3C7=_tmp3C4.f2;if((int)_tmp3C7 != 0)goto _LL270;_LL26F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DA;union Cyc_CfFlowInfo_FlowInfo_union _tmp3DB;struct _tuple8 _tmp3D9=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3BD,_tmp3C6);_tmp3DA=_tmp3D9.f1;_tmp3DB=_tmp3D9.f2;{void*_tmp3DC=
_tmp392;_LL283: if((int)_tmp3DC != 5)goto _LL285;_LL284: return({struct _tuple8
_tmp3DD;_tmp3DD.f1=_tmp3DB;_tmp3DD.f2=_tmp3DA;_tmp3DD;});_LL285: if((int)_tmp3DC
!= 6)goto _LL287;_LL286: return({struct _tuple8 _tmp3DE;_tmp3DE.f1=_tmp3DA;_tmp3DE.f2=
_tmp3DB;_tmp3DE;});_LL287:;_LL288:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_Core_Impossible_struct
_tmp3E0;_tmp3E0.tag=Cyc_Core_Impossible;_tmp3E0.f1=({const char*_tmp3E1="anal_test, zero-split";
_tag_dynforward(_tmp3E1,sizeof(char),_get_zero_arr_size(_tmp3E1,22));});_tmp3E0;});
_tmp3DF;}));_LL282:;}}_LL270: _tmp3C8=_tmp3C4.f1;if((int)_tmp3C8 != 0)goto _LL272;
_tmp3C9=_tmp3C4.f2;if(_tmp3C9 <= (void*)3)goto _LL272;if(*((int*)_tmp3C9)!= 0)goto
_LL272;_tmp3CA=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3C9)->f1;
_LL271: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3E3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E4;struct _tuple8 _tmp3E2=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3BE,
_tmp3CA);_tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;{void*_tmp3E5=_tmp392;_LL28A: if((
int)_tmp3E5 != 5)goto _LL28C;_LL28B: return({struct _tuple8 _tmp3E6;_tmp3E6.f1=
_tmp3E4;_tmp3E6.f2=_tmp3E3;_tmp3E6;});_LL28C: if((int)_tmp3E5 != 6)goto _LL28E;
_LL28D: return({struct _tuple8 _tmp3E7;_tmp3E7.f1=_tmp3E3;_tmp3E7.f2=_tmp3E4;
_tmp3E7;});_LL28E:;_LL28F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8[0]=({struct Cyc_Core_Impossible_struct
_tmp3E9;_tmp3E9.tag=Cyc_Core_Impossible;_tmp3E9.f1=({const char*_tmp3EA="anal_test, zero-split";
_tag_dynforward(_tmp3EA,sizeof(char),_get_zero_arr_size(_tmp3EA,22));});_tmp3E9;});
_tmp3E8;}));_LL289:;}}_LL272: _tmp3CB=_tmp3C4.f1;if((int)_tmp3CB != 0)goto _LL274;
_tmp3CC=_tmp3C4.f2;if((int)_tmp3CC != 0)goto _LL274;_LL273: if(_tmp392 == (void*)5)
return({struct _tuple8 _tmp3EB;_tmp3EB.f1=f;_tmp3EB.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3EC;(_tmp3EC.BottomFL).tag=0;_tmp3EC;});
_tmp3EB;});else{return({struct _tuple8 _tmp3ED;_tmp3ED.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3EE;(_tmp3EE.BottomFL).tag=0;_tmp3EE;});
_tmp3ED.f2=f;_tmp3ED;});}_LL274: _tmp3CD=_tmp3C4.f1;if((int)_tmp3CD != 0)goto
_LL276;_tmp3CE=_tmp3C4.f2;if((int)_tmp3CE != 1)goto _LL276;_LL275: goto _LL277;
_LL276: _tmp3CF=_tmp3C4.f1;if((int)_tmp3CF != 0)goto _LL278;_tmp3D0=_tmp3C4.f2;if((
int)_tmp3D0 != 2)goto _LL278;_LL277: goto _LL279;_LL278: _tmp3D1=_tmp3C4.f1;if((int)
_tmp3D1 != 0)goto _LL27A;_tmp3D2=_tmp3C4.f2;if(_tmp3D2 <= (void*)3)goto _LL27A;if(*((
int*)_tmp3D2)!= 2)goto _LL27A;_LL279: goto _LL27B;_LL27A: _tmp3D3=_tmp3C4.f1;if((int)
_tmp3D3 != 1)goto _LL27C;_tmp3D4=_tmp3C4.f2;if((int)_tmp3D4 != 0)goto _LL27C;_LL27B:
goto _LL27D;_LL27C: _tmp3D5=_tmp3C4.f1;if((int)_tmp3D5 != 2)goto _LL27E;_tmp3D6=
_tmp3C4.f2;if((int)_tmp3D6 != 0)goto _LL27E;_LL27D: goto _LL27F;_LL27E: _tmp3D7=
_tmp3C4.f1;if(_tmp3D7 <= (void*)3)goto _LL280;if(*((int*)_tmp3D7)!= 2)goto _LL280;
_tmp3D8=_tmp3C4.f2;if((int)_tmp3D8 != 0)goto _LL280;_LL27F: if(_tmp392 == (void*)6)
return({struct _tuple8 _tmp3EF;_tmp3EF.f1=f;_tmp3EF.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3F0;(_tmp3F0.BottomFL).tag=0;_tmp3F0;});
_tmp3EF;});else{return({struct _tuple8 _tmp3F1;_tmp3F1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3F2;(_tmp3F2.BottomFL).tag=0;_tmp3F2;});
_tmp3F1.f2=f;_tmp3F1;});}_LL280:;_LL281: goto _LL26D;_LL26D:;}{struct _tuple0
_tmp3F4=({struct _tuple0 _tmp3F3;_tmp3F3.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3BD->topt))->v);_tmp3F3.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3BE->topt))->v);_tmp3F3;});void*_tmp3F5;void*
_tmp3F6;void*_tmp3F7;void*_tmp3F8;void*_tmp3F9;void*_tmp3FA;_LL291: _tmp3F5=
_tmp3F4.f1;if(_tmp3F5 <= (void*)4)goto _LL293;if(*((int*)_tmp3F5)!= 5)goto _LL293;
_tmp3F6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F5)->f1;if((int)_tmp3F6 != 
1)goto _LL293;_LL292: goto _LL294;_LL293: _tmp3F7=_tmp3F4.f2;if(_tmp3F7 <= (void*)4)
goto _LL295;if(*((int*)_tmp3F7)!= 5)goto _LL295;_tmp3F8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3F7)->f1;if((int)_tmp3F8 != 1)goto _LL295;_LL294: goto _LL296;_LL295: _tmp3F9=
_tmp3F4.f1;if(_tmp3F9 <= (void*)4)goto _LL297;if(*((int*)_tmp3F9)!= 18)goto _LL297;
_LL296: goto _LL298;_LL297: _tmp3FA=_tmp3F4.f2;if(_tmp3FA <= (void*)4)goto _LL299;if(*((
int*)_tmp3FA)!= 18)goto _LL299;_LL298: goto _LL290;_LL299:;_LL29A: return({struct
_tuple8 _tmp3FB;_tmp3FB.f1=f;_tmp3FB.f2=f;_tmp3FB;});_LL290:;}{void*_tmp3FC=
_tmp392;_LL29C: if((int)_tmp3FC != 5)goto _LL29E;_LL29D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3FD=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3BD,r1,(void*)5,r2);_tmp3FD=Cyc_NewControlFlow_if_tagcmp(
env,_tmp3FD,_tmp3BE,r2,(void*)5,r1);return({struct _tuple8 _tmp3FE;_tmp3FE.f1=
_tmp3FD;_tmp3FE.f2=f;_tmp3FE;});}_LL29E: if((int)_tmp3FC != 6)goto _LL2A0;_LL29F: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3FF=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3BD,r1,(void*)5,r2);_tmp3FF=Cyc_NewControlFlow_if_tagcmp(env,_tmp3FF,_tmp3BE,
r2,(void*)5,r1);return({struct _tuple8 _tmp400;_tmp400.f1=f;_tmp400.f2=_tmp3FF;
_tmp400;});}_LL2A0: if((int)_tmp3FC != 7)goto _LL2A2;_LL2A1: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp401=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3BE,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp402=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3BD,r1,(void*)10,r2);return({
struct _tuple8 _tmp403;_tmp403.f1=_tmp401;_tmp403.f2=_tmp402;_tmp403;});}_LL2A2:
if((int)_tmp3FC != 9)goto _LL2A4;_LL2A3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp404=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3BE,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp405=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3BD,r1,(void*)8,r2);return({
struct _tuple8 _tmp406;_tmp406.f1=_tmp404;_tmp406.f2=_tmp405;_tmp406;});}_LL2A4:
if((int)_tmp3FC != 8)goto _LL2A6;_LL2A5: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp407=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3BD,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp408=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3BE,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp409=_tmp407;struct Cyc_Dict_Dict _tmp40A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp40B;_LL2AB: if((_tmp409.BottomFL).tag != 0)goto _LL2AD;_LL2AC:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({
struct Cyc_Core_Impossible_struct _tmp40D;_tmp40D.tag=Cyc_Core_Impossible;_tmp40D.f1=({
const char*_tmp40E="anal_test, Lt";_tag_dynforward(_tmp40E,sizeof(char),
_get_zero_arr_size(_tmp40E,14));});_tmp40D;});_tmp40C;}));_LL2AD: if((_tmp409.ReachableFL).tag
!= 1)goto _LL2AA;_tmp40A=(_tmp409.ReachableFL).f1;_tmp40B=(_tmp409.ReachableFL).f3;
_LL2AE: _tmp3B9=_tmp40A;_tmp3BB=_tmp40B;_LL2AA:;}{void*_tmp40F=(void*)_tmp3BD->r;
void*_tmp410;struct Cyc_Absyn_Vardecl*_tmp411;void*_tmp412;struct Cyc_Absyn_Vardecl*
_tmp413;void*_tmp414;struct Cyc_Absyn_Vardecl*_tmp415;void*_tmp416;struct Cyc_Absyn_Vardecl*
_tmp417;_LL2B0: if(*((int*)_tmp40F)!= 1)goto _LL2B2;_tmp410=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp40F)->f2;if(_tmp410 <= (void*)1)goto _LL2B2;if(*((int*)_tmp410)!= 0)goto _LL2B2;
_tmp411=((struct Cyc_Absyn_Global_b_struct*)_tmp410)->f1;if(!(!_tmp411->escapes))
goto _LL2B2;_LL2B1: _tmp413=_tmp411;goto _LL2B3;_LL2B2: if(*((int*)_tmp40F)!= 1)goto
_LL2B4;_tmp412=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp40F)->f2;if(_tmp412 <= (
void*)1)goto _LL2B4;if(*((int*)_tmp412)!= 3)goto _LL2B4;_tmp413=((struct Cyc_Absyn_Local_b_struct*)
_tmp412)->f1;if(!(!_tmp413->escapes))goto _LL2B4;_LL2B3: _tmp415=_tmp413;goto
_LL2B5;_LL2B4: if(*((int*)_tmp40F)!= 1)goto _LL2B6;_tmp414=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp40F)->f2;if(_tmp414 <= (void*)1)goto _LL2B6;if(*((int*)_tmp414)!= 4)goto _LL2B6;
_tmp415=((struct Cyc_Absyn_Pat_b_struct*)_tmp414)->f1;if(!(!_tmp415->escapes))
goto _LL2B6;_LL2B5: _tmp417=_tmp415;goto _LL2B7;_LL2B6: if(*((int*)_tmp40F)!= 1)goto
_LL2B8;_tmp416=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp40F)->f2;if(_tmp416 <= (
void*)1)goto _LL2B8;if(*((int*)_tmp416)!= 2)goto _LL2B8;_tmp417=((struct Cyc_Absyn_Param_b_struct*)
_tmp416)->f1;if(!(!_tmp417->escapes))goto _LL2B8;_LL2B7: {void*_tmp418=(void*)
_tmp3BE->r;void*_tmp419;struct Cyc_Absyn_Vardecl*_tmp41A;void*_tmp41B;struct Cyc_Absyn_Vardecl*
_tmp41C;void*_tmp41D;struct Cyc_Absyn_Vardecl*_tmp41E;void*_tmp41F;struct Cyc_Absyn_Vardecl*
_tmp420;union Cyc_Absyn_Cnst_union _tmp421;int _tmp422;void*_tmp423;struct Cyc_List_List*
_tmp424;struct Cyc_List_List _tmp425;struct Cyc_Absyn_Exp*_tmp426;_LL2BB: if(*((int*)
_tmp418)!= 1)goto _LL2BD;_tmp419=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp418)->f2;
if(_tmp419 <= (void*)1)goto _LL2BD;if(*((int*)_tmp419)!= 0)goto _LL2BD;_tmp41A=((
struct Cyc_Absyn_Global_b_struct*)_tmp419)->f1;if(!(!_tmp41A->escapes))goto _LL2BD;
_LL2BC: _tmp41C=_tmp41A;goto _LL2BE;_LL2BD: if(*((int*)_tmp418)!= 1)goto _LL2BF;
_tmp41B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp418)->f2;if(_tmp41B <= (void*)
1)goto _LL2BF;if(*((int*)_tmp41B)!= 3)goto _LL2BF;_tmp41C=((struct Cyc_Absyn_Local_b_struct*)
_tmp41B)->f1;if(!(!_tmp41C->escapes))goto _LL2BF;_LL2BE: _tmp41E=_tmp41C;goto
_LL2C0;_LL2BF: if(*((int*)_tmp418)!= 1)goto _LL2C1;_tmp41D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp418)->f2;if(_tmp41D <= (void*)1)goto _LL2C1;if(*((int*)_tmp41D)!= 4)goto _LL2C1;
_tmp41E=((struct Cyc_Absyn_Pat_b_struct*)_tmp41D)->f1;if(!(!_tmp41E->escapes))
goto _LL2C1;_LL2C0: _tmp420=_tmp41E;goto _LL2C2;_LL2C1: if(*((int*)_tmp418)!= 1)goto
_LL2C3;_tmp41F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp418)->f2;if(_tmp41F <= (
void*)1)goto _LL2C3;if(*((int*)_tmp41F)!= 2)goto _LL2C3;_tmp420=((struct Cyc_Absyn_Param_b_struct*)
_tmp41F)->f1;if(!(!_tmp420->escapes))goto _LL2C3;_LL2C2: {struct _RegionHandle*
_tmp427=(env->fenv)->r;struct Cyc_List_List*_tmp428=({struct Cyc_List_List*_tmp42B=
_region_malloc(_tmp427,sizeof(*_tmp42B));_tmp42B->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp42C=_region_malloc(_tmp427,sizeof(*_tmp42C));_tmp42C->vd=_tmp417;_tmp42C->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp42D;(
_tmp42D.LessVar).tag=1;(_tmp42D.LessVar).f1=_tmp420;_tmp42D;});_tmp42C;});
_tmp42B->tl=_tmp3BA;_tmp42B;});return({struct _tuple8 _tmp429;_tmp429.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp42A;(_tmp42A.ReachableFL).tag=1;(_tmp42A.ReachableFL).f1=
_tmp3B9;(_tmp42A.ReachableFL).f2=_tmp428;(_tmp42A.ReachableFL).f3=_tmp3BB;
_tmp42A;});_tmp429.f2=_tmp408;_tmp429;});}_LL2C3: if(*((int*)_tmp418)!= 0)goto
_LL2C5;_tmp421=((struct Cyc_Absyn_Const_e_struct*)_tmp418)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp418)->f1).Int_c).tag != 2)goto _LL2C5;_tmp422=(_tmp421.Int_c).f2;_LL2C4: {
struct _RegionHandle*_tmp42E=(env->fenv)->r;struct Cyc_List_List*_tmp42F=({struct
Cyc_List_List*_tmp432=_region_malloc(_tmp42E,sizeof(*_tmp432));_tmp432->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp433=_region_malloc(_tmp42E,sizeof(*_tmp433));
_tmp433->vd=_tmp417;_tmp433->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp434;(_tmp434.LessConst).tag=3;(_tmp434.LessConst).f1=(unsigned int)_tmp422;
_tmp434;});_tmp433;});_tmp432->tl=_tmp3BA;_tmp432;});return({struct _tuple8
_tmp430;_tmp430.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp431;(_tmp431.ReachableFL).tag=1;(_tmp431.ReachableFL).f1=_tmp3B9;(_tmp431.ReachableFL).f2=
_tmp42F;(_tmp431.ReachableFL).f3=_tmp3BB;_tmp431;});_tmp430.f2=_tmp408;_tmp430;});}
_LL2C5: if(*((int*)_tmp418)!= 3)goto _LL2C7;_tmp423=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp418)->f1;if((int)_tmp423 != 19)goto _LL2C7;_tmp424=((struct Cyc_Absyn_Primop_e_struct*)
_tmp418)->f2;if(_tmp424 == 0)goto _LL2C7;_tmp425=*_tmp424;_tmp426=(struct Cyc_Absyn_Exp*)
_tmp425.hd;_LL2C6: {void*_tmp435=(void*)_tmp426->r;void*_tmp436;struct Cyc_Absyn_Vardecl*
_tmp437;void*_tmp438;struct Cyc_Absyn_Vardecl*_tmp439;void*_tmp43A;struct Cyc_Absyn_Vardecl*
_tmp43B;void*_tmp43C;struct Cyc_Absyn_Vardecl*_tmp43D;_LL2CA: if(*((int*)_tmp435)
!= 1)goto _LL2CC;_tmp436=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp435)->f2;if(
_tmp436 <= (void*)1)goto _LL2CC;if(*((int*)_tmp436)!= 0)goto _LL2CC;_tmp437=((
struct Cyc_Absyn_Global_b_struct*)_tmp436)->f1;if(!(!_tmp437->escapes))goto _LL2CC;
_LL2CB: _tmp439=_tmp437;goto _LL2CD;_LL2CC: if(*((int*)_tmp435)!= 1)goto _LL2CE;
_tmp438=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp435)->f2;if(_tmp438 <= (void*)
1)goto _LL2CE;if(*((int*)_tmp438)!= 3)goto _LL2CE;_tmp439=((struct Cyc_Absyn_Local_b_struct*)
_tmp438)->f1;if(!(!_tmp439->escapes))goto _LL2CE;_LL2CD: _tmp43B=_tmp439;goto
_LL2CF;_LL2CE: if(*((int*)_tmp435)!= 1)goto _LL2D0;_tmp43A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp435)->f2;if(_tmp43A <= (void*)1)goto _LL2D0;if(*((int*)_tmp43A)!= 4)goto _LL2D0;
_tmp43B=((struct Cyc_Absyn_Pat_b_struct*)_tmp43A)->f1;if(!(!_tmp43B->escapes))
goto _LL2D0;_LL2CF: _tmp43D=_tmp43B;goto _LL2D1;_LL2D0: if(*((int*)_tmp435)!= 1)goto
_LL2D2;_tmp43C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp435)->f2;if(_tmp43C <= (
void*)1)goto _LL2D2;if(*((int*)_tmp43C)!= 2)goto _LL2D2;_tmp43D=((struct Cyc_Absyn_Param_b_struct*)
_tmp43C)->f1;if(!(!_tmp43D->escapes))goto _LL2D2;_LL2D1: {struct _RegionHandle*
_tmp43E=(env->fenv)->r;struct Cyc_List_List*_tmp43F=({struct Cyc_List_List*_tmp442=
_region_malloc(_tmp43E,sizeof(*_tmp442));_tmp442->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp443=_region_malloc(_tmp43E,sizeof(*_tmp443));_tmp443->vd=_tmp417;_tmp443->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp444;(
_tmp444.LessSize).tag=2;(_tmp444.LessSize).f1=_tmp43D;_tmp444;});_tmp443;});
_tmp442->tl=_tmp3BA;_tmp442;});return({struct _tuple8 _tmp440;_tmp440.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp441;(_tmp441.ReachableFL).tag=1;(_tmp441.ReachableFL).f1=
_tmp3B9;(_tmp441.ReachableFL).f2=_tmp43F;(_tmp441.ReachableFL).f3=_tmp3BB;
_tmp441;});_tmp440.f2=_tmp408;_tmp440;});}_LL2D2:;_LL2D3: return({struct _tuple8
_tmp445;_tmp445.f1=_tmp407;_tmp445.f2=_tmp408;_tmp445;});_LL2C9:;}_LL2C7:;_LL2C8:
return({struct _tuple8 _tmp446;_tmp446.f1=_tmp407;_tmp446.f2=_tmp408;_tmp446;});
_LL2BA:;}_LL2B8:;_LL2B9: return({struct _tuple8 _tmp447;_tmp447.f1=_tmp407;_tmp447.f2=
_tmp408;_tmp447;});_LL2AF:;}}_LL2A6: if((int)_tmp3FC != 10)goto _LL2A8;_LL2A7: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp448=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3BD,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp449=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3BE,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp44A=_tmp448;
struct Cyc_Dict_Dict _tmp44B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp44C;_LL2D5: if((
_tmp44A.BottomFL).tag != 0)goto _LL2D7;_LL2D6:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D[0]=({struct Cyc_Core_Impossible_struct
_tmp44E;_tmp44E.tag=Cyc_Core_Impossible;_tmp44E.f1=({const char*_tmp44F="anal_test, Lte";
_tag_dynforward(_tmp44F,sizeof(char),_get_zero_arr_size(_tmp44F,15));});_tmp44E;});
_tmp44D;}));_LL2D7: if((_tmp44A.ReachableFL).tag != 1)goto _LL2D4;_tmp44B=(_tmp44A.ReachableFL).f1;
_tmp44C=(_tmp44A.ReachableFL).f3;_LL2D8: _tmp3B9=_tmp44B;_tmp3BB=_tmp44C;_LL2D4:;}{
void*_tmp450=(void*)_tmp3BD->r;void*_tmp451;struct Cyc_Absyn_Vardecl*_tmp452;void*
_tmp453;struct Cyc_Absyn_Vardecl*_tmp454;void*_tmp455;struct Cyc_Absyn_Vardecl*
_tmp456;void*_tmp457;struct Cyc_Absyn_Vardecl*_tmp458;_LL2DA: if(*((int*)_tmp450)
!= 1)goto _LL2DC;_tmp451=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp450)->f2;if(
_tmp451 <= (void*)1)goto _LL2DC;if(*((int*)_tmp451)!= 0)goto _LL2DC;_tmp452=((
struct Cyc_Absyn_Global_b_struct*)_tmp451)->f1;if(!(!_tmp452->escapes))goto _LL2DC;
_LL2DB: _tmp454=_tmp452;goto _LL2DD;_LL2DC: if(*((int*)_tmp450)!= 1)goto _LL2DE;
_tmp453=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp450)->f2;if(_tmp453 <= (void*)
1)goto _LL2DE;if(*((int*)_tmp453)!= 3)goto _LL2DE;_tmp454=((struct Cyc_Absyn_Local_b_struct*)
_tmp453)->f1;if(!(!_tmp454->escapes))goto _LL2DE;_LL2DD: _tmp456=_tmp454;goto
_LL2DF;_LL2DE: if(*((int*)_tmp450)!= 1)goto _LL2E0;_tmp455=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp450)->f2;if(_tmp455 <= (void*)1)goto _LL2E0;if(*((int*)_tmp455)!= 4)goto _LL2E0;
_tmp456=((struct Cyc_Absyn_Pat_b_struct*)_tmp455)->f1;if(!(!_tmp456->escapes))
goto _LL2E0;_LL2DF: _tmp458=_tmp456;goto _LL2E1;_LL2E0: if(*((int*)_tmp450)!= 1)goto
_LL2E2;_tmp457=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp450)->f2;if(_tmp457 <= (
void*)1)goto _LL2E2;if(*((int*)_tmp457)!= 2)goto _LL2E2;_tmp458=((struct Cyc_Absyn_Param_b_struct*)
_tmp457)->f1;if(!(!_tmp458->escapes))goto _LL2E2;_LL2E1: {void*_tmp459=(void*)
_tmp3BE->r;void*_tmp45A;struct Cyc_List_List*_tmp45B;struct Cyc_List_List _tmp45C;
struct Cyc_Absyn_Exp*_tmp45D;_LL2E5: if(*((int*)_tmp459)!= 3)goto _LL2E7;_tmp45A=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp459)->f1;if((int)_tmp45A != 19)goto
_LL2E7;_tmp45B=((struct Cyc_Absyn_Primop_e_struct*)_tmp459)->f2;if(_tmp45B == 0)
goto _LL2E7;_tmp45C=*_tmp45B;_tmp45D=(struct Cyc_Absyn_Exp*)_tmp45C.hd;_LL2E6: {
void*_tmp45E=(void*)_tmp45D->r;void*_tmp45F;struct Cyc_Absyn_Vardecl*_tmp460;void*
_tmp461;struct Cyc_Absyn_Vardecl*_tmp462;void*_tmp463;struct Cyc_Absyn_Vardecl*
_tmp464;void*_tmp465;struct Cyc_Absyn_Vardecl*_tmp466;_LL2EA: if(*((int*)_tmp45E)
!= 1)goto _LL2EC;_tmp45F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45E)->f2;if(
_tmp45F <= (void*)1)goto _LL2EC;if(*((int*)_tmp45F)!= 0)goto _LL2EC;_tmp460=((
struct Cyc_Absyn_Global_b_struct*)_tmp45F)->f1;if(!(!_tmp460->escapes))goto _LL2EC;
_LL2EB: _tmp462=_tmp460;goto _LL2ED;_LL2EC: if(*((int*)_tmp45E)!= 1)goto _LL2EE;
_tmp461=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45E)->f2;if(_tmp461 <= (void*)
1)goto _LL2EE;if(*((int*)_tmp461)!= 3)goto _LL2EE;_tmp462=((struct Cyc_Absyn_Local_b_struct*)
_tmp461)->f1;if(!(!_tmp462->escapes))goto _LL2EE;_LL2ED: _tmp464=_tmp462;goto
_LL2EF;_LL2EE: if(*((int*)_tmp45E)!= 1)goto _LL2F0;_tmp463=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp45E)->f2;if(_tmp463 <= (void*)1)goto _LL2F0;if(*((int*)_tmp463)!= 4)goto _LL2F0;
_tmp464=((struct Cyc_Absyn_Pat_b_struct*)_tmp463)->f1;if(!(!_tmp464->escapes))
goto _LL2F0;_LL2EF: _tmp466=_tmp464;goto _LL2F1;_LL2F0: if(*((int*)_tmp45E)!= 1)goto
_LL2F2;_tmp465=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45E)->f2;if(_tmp465 <= (
void*)1)goto _LL2F2;if(*((int*)_tmp465)!= 2)goto _LL2F2;_tmp466=((struct Cyc_Absyn_Param_b_struct*)
_tmp465)->f1;if(!(!_tmp466->escapes))goto _LL2F2;_LL2F1: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp467=env->fenv;struct Cyc_List_List*_tmp468=({struct Cyc_List_List*_tmp46B=
_region_malloc(_tmp467->r,sizeof(*_tmp46B));_tmp46B->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp46C=_region_malloc(_tmp467->r,sizeof(*_tmp46C));_tmp46C->vd=_tmp458;_tmp46C->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp46D;(
_tmp46D.LessEqSize).tag=4;(_tmp46D.LessEqSize).f1=_tmp466;_tmp46D;});_tmp46C;});
_tmp46B->tl=_tmp3BA;_tmp46B;});return({struct _tuple8 _tmp469;_tmp469.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp46A;(_tmp46A.ReachableFL).tag=1;(_tmp46A.ReachableFL).f1=
_tmp3B9;(_tmp46A.ReachableFL).f2=_tmp468;(_tmp46A.ReachableFL).f3=_tmp3BB;
_tmp46A;});_tmp469.f2=_tmp449;_tmp469;});}_LL2F2:;_LL2F3: return({struct _tuple8
_tmp46E;_tmp46E.f1=_tmp448;_tmp46E.f2=_tmp449;_tmp46E;});_LL2E9:;}_LL2E7:;_LL2E8:
return({struct _tuple8 _tmp46F;_tmp46F.f1=_tmp448;_tmp46F.f2=_tmp449;_tmp46F;});
_LL2E4:;}_LL2E2:;_LL2E3: return({struct _tuple8 _tmp470;_tmp470.f1=_tmp448;_tmp470.f2=
_tmp449;_tmp470;});_LL2D9:;}}_LL2A8:;_LL2A9: return({struct _tuple8 _tmp471;_tmp471.f1=
f;_tmp471.f2=f;_tmp471;});_LL29B:;}}_LL268:;}}_LL25F:;_LL260: goto _LL252;_LL252:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp473;void*_tmp474;struct _tuple5 _tmp472=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;_tmp473=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp473);{union Cyc_CfFlowInfo_FlowInfo_union _tmp475=_tmp473;struct Cyc_Dict_Dict
_tmp476;_LL2F5: if((_tmp475.BottomFL).tag != 0)goto _LL2F7;_LL2F6: return({struct
_tuple8 _tmp477;_tmp477.f1=_tmp473;_tmp477.f2=_tmp473;_tmp477;});_LL2F7: if((
_tmp475.ReachableFL).tag != 1)goto _LL2F4;_tmp476=(_tmp475.ReachableFL).f1;_LL2F8: {
void*_tmp478=_tmp474;void*_tmp479;void*_tmp47A;void*_tmp47B;_LL2FA: if((int)
_tmp478 != 0)goto _LL2FC;_LL2FB: return({struct _tuple8 _tmp47C;_tmp47C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp47D;(_tmp47D.BottomFL).tag=0;_tmp47D;});
_tmp47C.f2=_tmp473;_tmp47C;});_LL2FC: if((int)_tmp478 != 2)goto _LL2FE;_LL2FD: goto
_LL2FF;_LL2FE: if((int)_tmp478 != 1)goto _LL300;_LL2FF: goto _LL301;_LL300: if(_tmp478
<= (void*)3)goto _LL302;if(*((int*)_tmp478)!= 2)goto _LL302;_LL301: return({struct
_tuple8 _tmp47E;_tmp47E.f1=_tmp473;_tmp47E.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp47F;(_tmp47F.BottomFL).tag=0;_tmp47F;});
_tmp47E;});_LL302: if(_tmp478 <= (void*)3)goto _LL304;if(*((int*)_tmp478)!= 0)goto
_LL304;_tmp479=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp478)->f1;if((
int)_tmp479 != 0)goto _LL304;_LL303: goto _LL305;_LL304: if(_tmp478 <= (void*)3)goto
_LL306;if(*((int*)_tmp478)!= 1)goto _LL306;_tmp47A=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp478)->f1;if((int)_tmp47A != 0)goto _LL306;_LL305:({void*_tmp480[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp481="expression may not be initialized";_tag_dynforward(
_tmp481,sizeof(char),_get_zero_arr_size(_tmp481,34));}),_tag_dynforward(_tmp480,
sizeof(void*),0));});return({struct _tuple8 _tmp482;_tmp482.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp484;(_tmp484.BottomFL).tag=0;_tmp484;});
_tmp482.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp483;(_tmp483.BottomFL).tag=0;_tmp483;});_tmp482;});_LL306: if(_tmp478 <= (void*)
3)goto _LL308;if(*((int*)_tmp478)!= 0)goto _LL308;_tmp47B=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp478)->f1;_LL307: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp473,e,
_tmp47B);_LL308: if(_tmp478 <= (void*)3)goto _LL30A;if(*((int*)_tmp478)!= 1)goto
_LL30A;_LL309: return({struct _tuple8 _tmp485;_tmp485.f1=_tmp473;_tmp485.f2=_tmp473;
_tmp485;});_LL30A: if(_tmp478 <= (void*)3)goto _LL30C;if(*((int*)_tmp478)!= 3)goto
_LL30C;_LL30B: return({struct _tuple8 _tmp486;_tmp486.f1=_tmp473;_tmp486.f2=_tmp473;
_tmp486;});_LL30C: if(_tmp478 <= (void*)3)goto _LL2F9;if(*((int*)_tmp478)!= 4)goto
_LL2F9;_LL30D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp487=
_cycalloc(sizeof(*_tmp487));_tmp487[0]=({struct Cyc_Core_Impossible_struct _tmp488;
_tmp488.tag=Cyc_Core_Impossible;_tmp488.f1=({const char*_tmp489="anal_test";
_tag_dynforward(_tmp489,sizeof(char),_get_zero_arr_size(_tmp489,10));});_tmp488;});
_tmp487;}));_LL2F9:;}_LL2F4:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp48A=flow;struct Cyc_Dict_Dict _tmp48B;
_LL30F: if((_tmp48A.BottomFL).tag != 0)goto _LL311;_LL310: return;_LL311: if((_tmp48A.ReachableFL).tag
!= 1)goto _LL30E;_tmp48B=(_tmp48A.ReachableFL).f1;_LL312:{struct Cyc_List_List*
_tmp48C=env->param_roots;for(0;_tmp48C != 0;_tmp48C=_tmp48C->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp48B,Cyc_CfFlowInfo_lookup_place(_tmp48B,(struct Cyc_CfFlowInfo_Place*)_tmp48C->hd))
!= (void*)2)({void*_tmp48D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp48E="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp48E,sizeof(char),_get_zero_arr_size(_tmp48E,76));}),
_tag_dynforward(_tmp48D,sizeof(void*),0));});}}return;_LL30E:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp48F=env->fenv;
for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp491;struct Cyc_Core_Opt*
_tmp492;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Stmt*_tmp494;struct Cyc_Position_Segment*
_tmp495;struct Cyc_Absyn_Switch_clause*_tmp490=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp491=*_tmp490;_tmp492=_tmp491.pat_vars;_tmp493=_tmp491.where_clause;
_tmp494=_tmp491.body;_tmp495=_tmp491.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp48F,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp492))->v,(void*)_tmp48F->unknown_all,_tmp495);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp493 != 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp497;union Cyc_CfFlowInfo_FlowInfo_union _tmp498;struct _tuple8 _tmp496=Cyc_NewControlFlow_anal_test(
env,clause_inflow,(struct Cyc_Absyn_Exp*)_tmp493);_tmp497=_tmp496.f1;_tmp498=
_tmp496.f2;inflow=_tmp498;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
_tmp497,_tmp494);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp494);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp499=
clause_outflow;_LL314: if((_tmp499.BottomFL).tag != 0)goto _LL316;_LL315: goto _LL313;
_LL316:;_LL317: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp49A[0]={};Cyc_Tcutil_terr(
_tmp494->loc,({const char*_tmp49B="switch clause may implicitly fallthru";
_tag_dynforward(_tmp49B,sizeof(char),_get_zero_arr_size(_tmp49B,38));}),
_tag_dynforward(_tmp49A,sizeof(void*),0));});else{({void*_tmp49C[0]={};Cyc_Tcutil_warn(
_tmp494->loc,({const char*_tmp49D="switch clause may implicitly fallthru";
_tag_dynforward(_tmp49D,sizeof(char),_get_zero_arr_size(_tmp49D,38));}),
_tag_dynforward(_tmp49C,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL313;_LL313:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp49E;(_tmp49E.BottomFL).tag=0;_tmp49E;});}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp4A0;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4A1;struct _tuple9 _tmp49F=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp4A0=_tmp49F.f1;_tmp4A1=_tmp49F.f2;inflow=*_tmp4A1;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4A2=env->fenv;{void*_tmp4A3=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4A4;struct
Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Stmt*_tmp4A7;
struct Cyc_Absyn_Stmt*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Stmt*
_tmp4AA;struct Cyc_Absyn_Stmt*_tmp4AB;struct _tuple3 _tmp4AC;struct Cyc_Absyn_Exp*
_tmp4AD;struct Cyc_Absyn_Stmt*_tmp4AE;struct Cyc_Absyn_Stmt*_tmp4AF;struct Cyc_Absyn_Stmt*
_tmp4B0;struct _tuple3 _tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Stmt*
_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct _tuple3 _tmp4B5;struct Cyc_Absyn_Exp*
_tmp4B6;struct Cyc_Absyn_Stmt*_tmp4B7;struct _tuple3 _tmp4B8;struct Cyc_Absyn_Exp*
_tmp4B9;struct Cyc_Absyn_Stmt*_tmp4BA;struct Cyc_Absyn_Stmt*_tmp4BB;struct Cyc_Absyn_Stmt*
_tmp4BC;struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_Switch_clause**_tmp4BE;
struct Cyc_Absyn_Switch_clause*_tmp4BF;struct Cyc_Absyn_Stmt*_tmp4C0;struct Cyc_Absyn_Stmt*
_tmp4C1;struct Cyc_Absyn_Stmt*_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C3;struct Cyc_List_List*
_tmp4C4;struct Cyc_Absyn_Stmt*_tmp4C5;struct Cyc_List_List*_tmp4C6;struct Cyc_Absyn_Decl*
_tmp4C7;struct Cyc_Absyn_Stmt*_tmp4C8;struct Cyc_Absyn_Stmt*_tmp4C9;struct Cyc_Absyn_Tvar*
_tmp4CA;struct Cyc_Absyn_Vardecl*_tmp4CB;int _tmp4CC;struct Cyc_Absyn_Exp*_tmp4CD;
struct Cyc_Absyn_Stmt*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Exp*
_tmp4D0;struct Cyc_Absyn_Tvar*_tmp4D1;struct Cyc_Absyn_Vardecl*_tmp4D2;struct Cyc_Absyn_Stmt*
_tmp4D3;_LL319: if((int)_tmp4A3 != 0)goto _LL31B;_LL31A: return inflow;_LL31B: if(
_tmp4A3 <= (void*)1)goto _LL343;if(*((int*)_tmp4A3)!= 2)goto _LL31D;_tmp4A4=((
struct Cyc_Absyn_Return_s_struct*)_tmp4A3)->f1;if(_tmp4A4 != 0)goto _LL31D;_LL31C:
if(env->noreturn)({void*_tmp4D4[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4D5="`noreturn' function might return";
_tag_dynforward(_tmp4D5,sizeof(char),_get_zero_arr_size(_tmp4D5,33));}),
_tag_dynforward(_tmp4D4,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4D6;(_tmp4D6.BottomFL).tag=0;_tmp4D6;});_LL31D: if(*((int*)_tmp4A3)!= 2)goto
_LL31F;_tmp4A5=((struct Cyc_Absyn_Return_s_struct*)_tmp4A3)->f1;_LL31E: if(env->noreturn)({
void*_tmp4D7[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4D8="`noreturn' function might return";
_tag_dynforward(_tmp4D8,sizeof(char),_get_zero_arr_size(_tmp4D8,33));}),
_tag_dynforward(_tmp4D7,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4DA;void*_tmp4DB;struct _tuple5 _tmp4D9=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4A5));_tmp4DA=_tmp4D9.f1;_tmp4DB=
_tmp4D9.f2;_tmp4DA=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4A5->loc,_tmp4DA);
_tmp4DA=Cyc_NewControlFlow_use_Rval(env,_tmp4A5->loc,_tmp4DA,_tmp4DB);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp4DA);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4DC;(_tmp4DC.BottomFL).tag=0;_tmp4DC;});}_LL31F: if(*((int*)_tmp4A3)!= 0)goto
_LL321;_tmp4A6=((struct Cyc_Absyn_Exp_s_struct*)_tmp4A3)->f1;_LL320: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4A6)).f1;goto _LL318;_LL321: if(*((int*)_tmp4A3)!= 1)goto _LL323;
_tmp4A7=((struct Cyc_Absyn_Seq_s_struct*)_tmp4A3)->f1;_tmp4A8=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4A3)->f2;_LL322: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4A7),_tmp4A8);goto _LL318;_LL323: if(*((int*)_tmp4A3)!= 3)goto
_LL325;_tmp4A9=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4A3)->f1;_tmp4AA=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4A3)->f2;_tmp4AB=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4A3)->f3;_LL324: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4DE;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4DF;struct _tuple8 _tmp4DD=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A9);
_tmp4DE=_tmp4DD.f1;_tmp4DF=_tmp4DD.f2;_tmp4DE=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4A9->loc,_tmp4DE);_tmp4DF=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4A9->loc,
_tmp4DF);outflow=Cyc_CfFlowInfo_join_flow(_tmp4A2,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp4DE,_tmp4AA),Cyc_NewControlFlow_anal_stmt(env,_tmp4DF,_tmp4AB),1);goto
_LL318;}_LL325: if(*((int*)_tmp4A3)!= 4)goto _LL327;_tmp4AC=((struct Cyc_Absyn_While_s_struct*)
_tmp4A3)->f1;_tmp4AD=_tmp4AC.f1;_tmp4AE=_tmp4AC.f2;_tmp4AF=((struct Cyc_Absyn_While_s_struct*)
_tmp4A3)->f2;_LL326: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp4E1;struct _tuple9
_tmp4E0=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4AE);_tmp4E1=_tmp4E0.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4E2=*_tmp4E1;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E4;union Cyc_CfFlowInfo_FlowInfo_union _tmp4E5;struct _tuple8 _tmp4E3=Cyc_NewControlFlow_anal_test(
env,_tmp4E2,_tmp4AD);_tmp4E4=_tmp4E3.f1;_tmp4E5=_tmp4E3.f2;_tmp4E4=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4AD->loc,_tmp4E4);_tmp4E5=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4AD->loc,
_tmp4E5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E6=Cyc_NewControlFlow_anal_stmt(
env,_tmp4E4,_tmp4AF);Cyc_NewControlFlow_update_flow(env,_tmp4AE,_tmp4E6);outflow=
_tmp4E5;goto _LL318;}}}_LL327: if(*((int*)_tmp4A3)!= 13)goto _LL329;_tmp4B0=((
struct Cyc_Absyn_Do_s_struct*)_tmp4A3)->f1;_tmp4B1=((struct Cyc_Absyn_Do_s_struct*)
_tmp4A3)->f2;_tmp4B2=_tmp4B1.f1;_tmp4B3=_tmp4B1.f2;_LL328: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E7=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4B0);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4E9;struct _tuple9 _tmp4E8=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4E7,
_tmp4B3);_tmp4E9=_tmp4E8.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4EA=*_tmp4E9;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4EC;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4ED;struct _tuple8 _tmp4EB=Cyc_NewControlFlow_anal_test(env,_tmp4EA,_tmp4B2);
_tmp4EC=_tmp4EB.f1;_tmp4ED=_tmp4EB.f2;Cyc_NewControlFlow_update_flow(env,_tmp4B0,
_tmp4EC);outflow=_tmp4ED;goto _LL318;}}_LL329: if(*((int*)_tmp4A3)!= 8)goto _LL32B;
_tmp4B4=((struct Cyc_Absyn_For_s_struct*)_tmp4A3)->f1;_tmp4B5=((struct Cyc_Absyn_For_s_struct*)
_tmp4A3)->f2;_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;_tmp4B8=((struct Cyc_Absyn_For_s_struct*)
_tmp4A3)->f3;_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;_tmp4BB=((struct Cyc_Absyn_For_s_struct*)
_tmp4A3)->f4;_LL32A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4EE=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4B4)).f1;_tmp4EE=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4B4->loc,
_tmp4EE);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp4F0;struct _tuple9 _tmp4EF=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp4EE,_tmp4B7);_tmp4F0=_tmp4EF.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4F1=*_tmp4F0;union Cyc_CfFlowInfo_FlowInfo_union _tmp4F3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp4F4;struct _tuple8 _tmp4F2=Cyc_NewControlFlow_anal_test(env,_tmp4F1,_tmp4B6);
_tmp4F3=_tmp4F2.f1;_tmp4F4=_tmp4F2.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4F5=
Cyc_NewControlFlow_anal_stmt(env,_tmp4F3,_tmp4BB);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4F7;struct _tuple9 _tmp4F6=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4F5,
_tmp4BA);_tmp4F7=_tmp4F6.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp4F8=*_tmp4F7;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4F9=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp4F8,_tmp4B9)).f1;_tmp4F9=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4B9->loc,
_tmp4F9);Cyc_NewControlFlow_update_flow(env,_tmp4B7,_tmp4F9);outflow=_tmp4F4;
goto _LL318;}}}}}_LL32B: if(*((int*)_tmp4A3)!= 5)goto _LL32D;_tmp4BC=((struct Cyc_Absyn_Break_s_struct*)
_tmp4A3)->f1;if(_tmp4BC != 0)goto _LL32D;_LL32C: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4FA;(_tmp4FA.BottomFL).tag=0;_tmp4FA;});
_LL32D: if(*((int*)_tmp4A3)!= 10)goto _LL32F;_tmp4BD=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4A3)->f1;_tmp4BE=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4A3)->f2;if(
_tmp4BE == 0)goto _LL32F;_tmp4BF=*_tmp4BE;_LL32E: {struct _RegionHandle _tmp4FB=
_new_region("temp");struct _RegionHandle*temp=& _tmp4FB;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp4FD;struct Cyc_List_List*_tmp4FE;struct _tuple11
_tmp4FC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4BD,0);
_tmp4FD=_tmp4FC.f1;_tmp4FE=_tmp4FC.f2;for(0;_tmp4FE != 0;(_tmp4FE=_tmp4FE->tl,
_tmp4BD=_tmp4BD->tl)){_tmp4FD=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4BD))->hd)->loc,_tmp4FD,(void*)_tmp4FE->hd);}
_tmp4FD=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp4FD);_tmp4FD=Cyc_NewControlFlow_add_vars(
_tmp4A2,_tmp4FD,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4BF->pat_vars))->v,(
void*)_tmp4A2->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4BF->body,_tmp4FD);{union Cyc_CfFlowInfo_FlowInfo_union _tmp500=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4FF;(_tmp4FF.BottomFL).tag=0;_tmp4FF;});
_npop_handler(0);return _tmp500;}};_pop_region(temp);}_LL32F: if(*((int*)_tmp4A3)
!= 5)goto _LL331;_tmp4C0=((struct Cyc_Absyn_Break_s_struct*)_tmp4A3)->f1;_LL330:
_tmp4C1=_tmp4C0;goto _LL332;_LL331: if(*((int*)_tmp4A3)!= 6)goto _LL333;_tmp4C1=((
struct Cyc_Absyn_Continue_s_struct*)_tmp4A3)->f1;_LL332: _tmp4C2=_tmp4C1;goto
_LL334;_LL333: if(*((int*)_tmp4A3)!= 7)goto _LL335;_tmp4C2=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4A3)->f2;_LL334: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp501=
_tmp4A0->encloser;struct Cyc_Absyn_Stmt*_tmp502=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4C2)))->encloser;while(_tmp502 != _tmp501){
struct Cyc_Absyn_Stmt*_tmp503=(Cyc_NewControlFlow_get_stmt_annot(_tmp501))->encloser;
if(_tmp503 == _tmp501){({void*_tmp504[0]={};Cyc_Tcutil_terr(s->loc,({const char*
_tmp505="goto enters local scope or exception handler";_tag_dynforward(_tmp505,
sizeof(char),_get_zero_arr_size(_tmp505,45));}),_tag_dynforward(_tmp504,sizeof(
void*),0));});break;}_tmp501=_tmp503;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp4C2),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp506;(_tmp506.BottomFL).tag=0;_tmp506;});
_LL335: if(*((int*)_tmp4A3)!= 9)goto _LL337;_tmp4C3=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4A3)->f1;_tmp4C4=((struct Cyc_Absyn_Switch_s_struct*)_tmp4A3)->f2;_LL336: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp508;void*_tmp509;struct _tuple5 _tmp507=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4C3);_tmp508=_tmp507.f1;_tmp509=_tmp507.f2;_tmp508=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4C3->loc,_tmp508);_tmp508=Cyc_NewControlFlow_use_Rval(env,_tmp4C3->loc,
_tmp508,_tmp509);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp508,_tmp4C4);goto
_LL318;}_LL337: if(*((int*)_tmp4A3)!= 14)goto _LL339;_tmp4C5=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4A3)->f1;_tmp4C6=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4A3)->f2;_LL338: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4C5);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4C6);{union Cyc_CfFlowInfo_FlowInfo_union _tmp50A=scs_outflow;
_LL346: if((_tmp50A.BottomFL).tag != 0)goto _LL348;_LL347: goto _LL345;_LL348:;_LL349:({
void*_tmp50B[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp50C="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp50C,sizeof(char),_get_zero_arr_size(_tmp50C,42));}),
_tag_dynforward(_tmp50B,sizeof(void*),0));});_LL345:;}outflow=s1_outflow;goto
_LL318;}}}_LL339: if(*((int*)_tmp4A3)!= 11)goto _LL33B;_tmp4C7=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4A3)->f1;_tmp4C8=((struct Cyc_Absyn_Decl_s_struct*)_tmp4A3)->f2;_LL33A:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4C7),_tmp4C8);goto _LL318;_LL33B: if(*((int*)_tmp4A3)!= 12)goto _LL33D;_tmp4C9=((
struct Cyc_Absyn_Label_s_struct*)_tmp4A3)->f2;_LL33C: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4C9);goto _LL318;_LL33D: if(*((int*)_tmp4A3)!= 15)goto _LL33F;
_tmp4CA=((struct Cyc_Absyn_Region_s_struct*)_tmp4A3)->f1;_tmp4CB=((struct Cyc_Absyn_Region_s_struct*)
_tmp4A3)->f2;_tmp4CC=((struct Cyc_Absyn_Region_s_struct*)_tmp4A3)->f3;_tmp4CD=((
struct Cyc_Absyn_Region_s_struct*)_tmp4A3)->f4;_tmp4CE=((struct Cyc_Absyn_Region_s_struct*)
_tmp4A3)->f5;_LL33E: if(_tmp4CD != 0){struct Cyc_Absyn_Exp*_tmp50D=(struct Cyc_Absyn_Exp*)
_tmp4CD;union Cyc_CfFlowInfo_FlowInfo_union _tmp50F;void*_tmp510;struct _tuple5
_tmp50E=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp50D);_tmp50F=_tmp50E.f1;
_tmp510=_tmp50E.f2;_tmp50F=Cyc_CfFlowInfo_consume_unique_rvals(_tmp50D->loc,
_tmp50F);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp50D->loc,_tmp50F,_tmp510);}{
struct Cyc_List_List _tmp511=({struct Cyc_List_List _tmp512;_tmp512.hd=_tmp4CB;
_tmp512.tl=0;_tmp512;});inflow=Cyc_NewControlFlow_add_vars(_tmp4A2,inflow,(
struct Cyc_List_List*)& _tmp511,(void*)_tmp4A2->unknown_all,_tmp4CE->loc);outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4CE);goto _LL318;}_LL33F: if(*((int*)
_tmp4A3)!= 16)goto _LL341;_tmp4CF=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4A3)->f1;
_LL340: {union Cyc_CfFlowInfo_FlowInfo_union _tmp514;void*_tmp515;struct _tuple5
_tmp513=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4CF);_tmp514=_tmp513.f1;
_tmp515=_tmp513.f2;_tmp514=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4CF->loc,
_tmp514);{union Cyc_CfFlowInfo_FlowInfo_union _tmp516=Cyc_NewControlFlow_use_Rval(
env,_tmp4CF->loc,_tmp514,_tmp515);{void*_tmp517=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4CF->topt))->v);void*_tmp518;_LL34B: if(
_tmp517 <= (void*)4)goto _LL34D;if(*((int*)_tmp517)!= 15)goto _LL34D;_tmp518=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp517)->f1;_LL34C: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4A2,_tmp514,_tmp518);goto _LL34A;_LL34D:;_LL34E:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519[0]=({
struct Cyc_Core_Impossible_struct _tmp51A;_tmp51A.tag=Cyc_Core_Impossible;_tmp51A.f1=({
const char*_tmp51B="anal_stmt -- reset_region";_tag_dynforward(_tmp51B,sizeof(
char),_get_zero_arr_size(_tmp51B,26));});_tmp51A;});_tmp519;}));_LL34A:;}goto
_LL318;}}_LL341: if(*((int*)_tmp4A3)!= 17)goto _LL343;_tmp4D0=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4A3)->f1;_tmp4D1=((struct Cyc_Absyn_Alias_s_struct*)_tmp4A3)->f2;_tmp4D2=((
struct Cyc_Absyn_Alias_s_struct*)_tmp4A3)->f3;_tmp4D3=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4A3)->f4;_LL342: {union Cyc_CfFlowInfo_FlowInfo_union _tmp51D;void*_tmp51E;
struct _tuple5 _tmp51C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4D0);_tmp51D=
_tmp51C.f1;_tmp51E=_tmp51C.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp520;struct Cyc_List_List*
_tmp521;struct _tuple6 _tmp51F=Cyc_CfFlowInfo_save_consume_info(_tmp4A2,_tmp51D,0);
_tmp520=_tmp51F.f1;_tmp521=_tmp520.may_consume;_tmp51D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4D0->loc,_tmp51D);_tmp51D=Cyc_NewControlFlow_use_Rval(env,_tmp4D0->loc,
_tmp51D,_tmp51E);{struct Cyc_List_List _tmp522=({struct Cyc_List_List _tmp529;
_tmp529.hd=_tmp4D2;_tmp529.tl=0;_tmp529;});_tmp51D=Cyc_NewControlFlow_add_vars(
_tmp4A2,_tmp51D,(struct Cyc_List_List*)& _tmp522,(void*)_tmp4A2->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp51D,_tmp4D3);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp523=outflow;struct Cyc_Dict_Dict _tmp524;struct Cyc_List_List*_tmp525;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp526;_LL350: if((_tmp523.BottomFL).tag != 0)goto _LL352;_LL351: goto _LL34F;_LL352:
if((_tmp523.ReachableFL).tag != 1)goto _LL34F;_tmp524=(_tmp523.ReachableFL).f1;
_tmp525=(_tmp523.ReachableFL).f2;_tmp526=(_tmp523.ReachableFL).f3;_LL353: while(
_tmp521 != 0){struct Cyc_Dict_Dict _tmp527=_tmp526.consumed;_tmp526.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4A2->r,_tmp526.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp521->hd);if((_tmp526.consumed).t != _tmp527.t);_tmp521=_tmp521->tl;}outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp528;(
_tmp528.ReachableFL).tag=1;(_tmp528.ReachableFL).f1=_tmp524;(_tmp528.ReachableFL).f2=
_tmp525;(_tmp528.ReachableFL).f3=_tmp526;_tmp528;});goto _LL34F;_LL34F:;}goto
_LL318;}}}_LL343:;_LL344:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A[0]=({struct Cyc_Core_Impossible_struct
_tmp52B;_tmp52B.tag=Cyc_Core_Impossible;_tmp52B.f1=({const char*_tmp52C="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp52C,sizeof(char),_get_zero_arr_size(_tmp52C,56));});_tmp52B;});
_tmp52A;}));_LL318:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp52D=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp52E;_LL355: if((_tmp52D.ReachableFL).tag != 1)goto _LL357;_tmp52E=(_tmp52D.ReachableFL).f3;
_LL356: goto _LL354;_LL357:;_LL358: goto _LL354;_LL354:;}return outflow;}}static void
Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp52F=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp530;struct Cyc_Core_Opt*_tmp531;struct Cyc_Core_Opt _tmp532;struct Cyc_List_List*
_tmp533;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_List_List*_tmp535;struct Cyc_Absyn_Fndecl*
_tmp536;_LL35A: if(_tmp52F <= (void*)2)goto _LL362;if(*((int*)_tmp52F)!= 0)goto
_LL35C;_tmp530=((struct Cyc_Absyn_Var_d_struct*)_tmp52F)->f1;_LL35B: {struct Cyc_List_List
_tmp537=({struct Cyc_List_List _tmp546;_tmp546.hd=_tmp530;_tmp546.tl=0;_tmp546;});
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp537,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp538=
_tmp530->initializer;if(_tmp538 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp53A;void*_tmp53B;struct _tuple5 _tmp539=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp538);_tmp53A=_tmp539.f1;_tmp53B=_tmp539.f2;
_tmp53A=Cyc_CfFlowInfo_consume_unique_rvals(_tmp538->loc,_tmp53A);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp53C=_tmp53A;struct Cyc_Dict_Dict _tmp53D;struct Cyc_List_List*_tmp53E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp53F;_LL365: if((_tmp53C.BottomFL).tag != 0)goto _LL367;_LL366: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp540;(_tmp540.BottomFL).tag=0;_tmp540;});
_LL367: if((_tmp53C.ReachableFL).tag != 1)goto _LL364;_tmp53D=(_tmp53C.ReachableFL).f1;
_tmp53E=(_tmp53C.ReachableFL).f2;_tmp53F=(_tmp53C.ReachableFL).f3;_LL368: _tmp53D=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp53D,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp541=_region_malloc(env->r,sizeof(*_tmp541));
_tmp541->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp542=
_region_malloc(env->r,sizeof(*_tmp542));_tmp542[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp543;_tmp543.tag=0;_tmp543.f1=_tmp530;_tmp543;});_tmp542;}));_tmp541->fields=
0;_tmp541;}),_tmp53B);_tmp53E=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp53E,
_tmp530,(struct Cyc_Absyn_Exp*)_check_null(_tmp530->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp544=({union Cyc_CfFlowInfo_FlowInfo_union _tmp545;(_tmp545.ReachableFL).tag=1;(
_tmp545.ReachableFL).f1=_tmp53D;(_tmp545.ReachableFL).f2=_tmp53E;(_tmp545.ReachableFL).f3=
_tmp53F;_tmp545;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp544);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp544;}_LL364:;}}}}_LL35C:
if(*((int*)_tmp52F)!= 2)goto _LL35E;_tmp531=((struct Cyc_Absyn_Let_d_struct*)
_tmp52F)->f2;if(_tmp531 == 0)goto _LL35E;_tmp532=*_tmp531;_tmp533=(struct Cyc_List_List*)
_tmp532.v;_tmp534=((struct Cyc_Absyn_Let_d_struct*)_tmp52F)->f3;_LL35D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp548;void*_tmp549;struct _tuple5 _tmp547=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp534);_tmp548=_tmp547.f1;_tmp549=_tmp547.f2;_tmp548=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp534->loc,_tmp548);_tmp548=Cyc_NewControlFlow_use_Rval(env,_tmp534->loc,
_tmp548,_tmp549);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp548,_tmp533,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL35E: if(*((int*)_tmp52F)!= 3)goto
_LL360;_tmp535=((struct Cyc_Absyn_Letv_d_struct*)_tmp52F)->f1;_LL35F: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp535,(void*)(env->fenv)->unknown_none,decl->loc);_LL360: if(*((
int*)_tmp52F)!= 1)goto _LL362;_tmp536=((struct Cyc_Absyn_Fn_d_struct*)_tmp52F)->f1;
_LL361: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp536);{void*
_tmp54A=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp536->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp54B=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp536->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp54C=
_region_malloc(env->r,sizeof(*_tmp54C));_tmp54C->hd=_tmp54B;_tmp54C->tl=0;
_tmp54C;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL362:;_LL363:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp54D=_cycalloc(sizeof(*
_tmp54D));_tmp54D[0]=({struct Cyc_Core_Impossible_struct _tmp54E;_tmp54E.tag=Cyc_Core_Impossible;
_tmp54E.f1=({const char*_tmp54F="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp54F,sizeof(char),_get_zero_arr_size(_tmp54F,35));});_tmp54E;});
_tmp54D;}));_LL359:;}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp550=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp550;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp551;(_tmp551.ReachableFL).tag=1;(_tmp551.ReachableFL).f1=
fenv->mt_flowdict;(_tmp551.ReachableFL).f2=0;(_tmp551.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp552;_tmp552.consumed=fenv->mt_place_set;_tmp552.may_consume=
0;_tmp552;});_tmp551;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp553=fenv->r;struct Cyc_Position_Segment*_tmp554=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp554);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp555=inflow;struct Cyc_Dict_Dict
_tmp556;struct Cyc_List_List*_tmp557;struct Cyc_CfFlowInfo_ConsumeInfo _tmp558;
_LL36A: if((_tmp555.BottomFL).tag != 0)goto _LL36C;_LL36B:({void*_tmp559[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp55A="check_fun";_tag_dynforward(_tmp55A,sizeof(char),
_get_zero_arr_size(_tmp55A,10));}),_tag_dynforward(_tmp559,sizeof(void*),0));});
_LL36C: if((_tmp555.ReachableFL).tag != 1)goto _LL369;_tmp556=(_tmp555.ReachableFL).f1;
_tmp557=(_tmp555.ReachableFL).f2;_tmp558=(_tmp555.ReachableFL).f3;_LL36D: {
struct Cyc_List_List*atts;{void*_tmp55B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp55C;struct Cyc_List_List*
_tmp55D;_LL36F: if(_tmp55B <= (void*)4)goto _LL371;if(*((int*)_tmp55B)!= 8)goto
_LL371;_tmp55C=((struct Cyc_Absyn_FnType_struct*)_tmp55B)->f1;_tmp55D=_tmp55C.attributes;
_LL370: atts=_tmp55D;goto _LL36E;_LL371:;_LL372:({void*_tmp55E[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp55F="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp55F,sizeof(char),_get_zero_arr_size(_tmp55F,50));}),_tag_dynforward(_tmp55E,
sizeof(void*),0));});_LL36E:;}for(0;atts != 0;atts=atts->tl){void*_tmp560=(void*)
atts->hd;int _tmp561;_LL374: if(_tmp560 <= (void*)17)goto _LL376;if(*((int*)_tmp560)
!= 4)goto _LL376;_tmp561=((struct Cyc_Absyn_Initializes_att_struct*)_tmp560)->f1;
_LL375: {unsigned int j=(unsigned int)_tmp561;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp562[0]={};Cyc_Tcutil_terr(_tmp554,({const char*_tmp563="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp563,sizeof(char),_get_zero_arr_size(_tmp563,51));}),
_tag_dynforward(_tmp562,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp564=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp565=Cyc_Tcutil_compress((void*)_tmp564->type);struct Cyc_Absyn_PtrInfo
_tmp566;void*_tmp567;struct Cyc_Absyn_PtrAtts _tmp568;struct Cyc_Absyn_Conref*
_tmp569;struct Cyc_Absyn_Conref*_tmp56A;struct Cyc_Absyn_Conref*_tmp56B;_LL379: if(
_tmp565 <= (void*)4)goto _LL37B;if(*((int*)_tmp565)!= 4)goto _LL37B;_tmp566=((
struct Cyc_Absyn_PointerType_struct*)_tmp565)->f1;_tmp567=(void*)_tmp566.elt_typ;
_tmp568=_tmp566.ptr_atts;_tmp569=_tmp568.nullable;_tmp56A=_tmp568.bounds;_tmp56B=
_tmp568.zero_term;_LL37A: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp569))({void*_tmp56C[0]={};Cyc_Tcutil_terr(_tmp554,({const char*_tmp56D="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp56D,sizeof(char),_get_zero_arr_size(_tmp56D,55));}),
_tag_dynforward(_tmp56C,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp56A))({
void*_tmp56E[0]={};Cyc_Tcutil_terr(_tmp554,({const char*_tmp56F="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp56F,sizeof(char),_get_zero_arr_size(_tmp56F,57));}),
_tag_dynforward(_tmp56E,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp56B))({void*_tmp570[0]={};Cyc_Tcutil_terr(_tmp554,({
const char*_tmp571="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp571,sizeof(char),_get_zero_arr_size(_tmp571,77));}),
_tag_dynforward(_tmp570,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp572=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp57A=_region_malloc(_tmp553,
sizeof(*_tmp57A));_tmp57A[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp57B;
_tmp57B.tag=2;_tmp57B.f1=(int)j;_tmp57B.f2=(void*)_tmp567;_tmp57B;});_tmp57A;});
struct Cyc_CfFlowInfo_Place*_tmp573=({struct Cyc_CfFlowInfo_Place*_tmp579=
_region_malloc(_tmp553,sizeof(*_tmp579));_tmp579->root=(void*)((void*)_tmp572);
_tmp579->fields=0;_tmp579;});_tmp556=Cyc_Dict_insert(_tmp556,(void*)_tmp572,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp567,(void*)fenv->esc_none));_tmp556=Cyc_Dict_insert(_tmp556,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp574=_region_malloc(_tmp553,sizeof(*
_tmp574));_tmp574[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp575;_tmp575.tag=0;
_tmp575.f1=_tmp564;_tmp575;});_tmp574;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp576=_region_malloc(_tmp553,sizeof(*_tmp576));_tmp576[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp577;_tmp577.tag=2;_tmp577.f1=_tmp573;_tmp577;});_tmp576;}));param_roots=({
struct Cyc_List_List*_tmp578=_region_malloc(_tmp553,sizeof(*_tmp578));_tmp578->hd=
_tmp573;_tmp578->tl=param_roots;_tmp578;});goto _LL378;}_LL37B:;_LL37C:({void*
_tmp57C[0]={};Cyc_Tcutil_terr(_tmp554,({const char*_tmp57D="initializes attribute on non-pointer";
_tag_dynforward(_tmp57D,sizeof(char),_get_zero_arr_size(_tmp57D,37));}),
_tag_dynforward(_tmp57C,sizeof(void*),0));});_LL378:;}goto _LL373;}}_LL376:;
_LL377: goto _LL373;_LL373:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp57E;(_tmp57E.ReachableFL).tag=1;(_tmp57E.ReachableFL).f1=_tmp556;(_tmp57E.ReachableFL).f2=
_tmp557;(_tmp57E.ReachableFL).f3=_tmp558;_tmp57E;});}_LL369:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(
struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),
int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp553,33,(int(*)(
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp587=_region_malloc(_tmp553,sizeof(*_tmp587));_tmp587->r=_tmp553;_tmp587->fenv=
fenv;_tmp587->iterate_again=1;_tmp587->iteration_num=0;_tmp587->in_try=0;_tmp587->tryflow=
inflow;_tmp587->all_changed=0;_tmp587->noreturn=noreturn;_tmp587->param_roots=
param_roots;_tmp587->flow_table=flow_table;_tmp587;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp57F=outflow;_LL37E: if((_tmp57F.BottomFL).tag != 0)goto _LL380;_LL37F: goto
_LL37D;_LL380:;_LL381: Cyc_NewControlFlow_check_init_params(_tmp554,env,outflow);
if(noreturn)({void*_tmp580[0]={};Cyc_Tcutil_terr(_tmp554,({const char*_tmp581="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp581,sizeof(char),_get_zero_arr_size(_tmp581,46));}),
_tag_dynforward(_tmp580,sizeof(void*),0));});else{void*_tmp582=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL383: if((int)_tmp582 != 0)goto _LL385;_LL384: goto _LL382;
_LL385: if(_tmp582 <= (void*)4)goto _LL387;if(*((int*)_tmp582)!= 6)goto _LL387;
_LL386: goto _LL388;_LL387: if((int)_tmp582 != 1)goto _LL389;_LL388: goto _LL38A;_LL389:
if(_tmp582 <= (void*)4)goto _LL38B;if(*((int*)_tmp582)!= 5)goto _LL38B;_LL38A:({
void*_tmp583[0]={};Cyc_Tcutil_warn(_tmp554,({const char*_tmp584="function may not return a value";
_tag_dynforward(_tmp584,sizeof(char),_get_zero_arr_size(_tmp584,32));}),
_tag_dynforward(_tmp583,sizeof(void*),0));});goto _LL382;_LL38B:;_LL38C:({void*
_tmp585[0]={};Cyc_Tcutil_terr(_tmp554,({const char*_tmp586="function may not return a value";
_tag_dynforward(_tmp586,sizeof(char),_get_zero_arr_size(_tmp586,32));}),
_tag_dynforward(_tmp585,sizeof(void*),0));});goto _LL382;_LL382:;}goto _LL37D;
_LL37D:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;
ds=ds->tl){void*_tmp588=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*
_tmp589;struct Cyc_List_List*_tmp58A;struct Cyc_List_List*_tmp58B;struct Cyc_List_List*
_tmp58C;_LL38E: if(_tmp588 <= (void*)2)goto _LL398;if(*((int*)_tmp588)!= 1)goto
_LL390;_tmp589=((struct Cyc_Absyn_Fn_d_struct*)_tmp588)->f1;_LL38F: Cyc_NewControlFlow_check_fun(
_tmp589);goto _LL38D;_LL390: if(*((int*)_tmp588)!= 10)goto _LL392;_tmp58A=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp588)->f1;_LL391: _tmp58B=_tmp58A;goto _LL393;
_LL392: if(*((int*)_tmp588)!= 9)goto _LL394;_tmp58B=((struct Cyc_Absyn_Using_d_struct*)
_tmp588)->f2;_LL393: _tmp58C=_tmp58B;goto _LL395;_LL394: if(*((int*)_tmp588)!= 8)
goto _LL396;_tmp58C=((struct Cyc_Absyn_Namespace_d_struct*)_tmp588)->f2;_LL395: Cyc_NewControlFlow_cf_check(
_tmp58C);goto _LL38D;_LL396: if(*((int*)_tmp588)!= 11)goto _LL398;_LL397: goto _LL38D;
_LL398:;_LL399: goto _LL38D;_LL38D:;}}
