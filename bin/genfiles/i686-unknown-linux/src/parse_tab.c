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
16];extern char Cyc_Core_Free_Region[16];typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long
__pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
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
struct _dynforward_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,
struct _dynforward_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};extern char
Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dynforward_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*
x,void*accum);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(
struct Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0
Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_zstrcmp(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_zstrptrcmp(struct _dynforward_ptr*,struct
_dynforward_ptr*);struct _dynforward_ptr Cyc_strcat(struct _dynforward_ptr dest,
struct _dynforward_ptr src);struct _dynforward_ptr Cyc_strcpy(struct _dynforward_ptr
dest,struct _dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct
_dynforward_ptr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct _dynforward_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _dynforward_ptr);extern char Cyc_Position_Nocontext[
14];void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_copy_type(void*
t);void*Cyc_Tcutil_compress(void*t);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Lex_register_typedef(
struct _tuple1*s);void Cyc_Lex_enter_namespace(struct _dynforward_ptr*);void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dynforward_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_Parse_err((struct
_dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)
Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct _dynforward_ptr msg,struct Cyc_Position_Segment*
sg){({struct Cyc_String_pa_struct _tmp3;_tmp3.tag=0;_tmp3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)msg);{struct Cyc_String_pa_struct _tmp2;
_tmp2.tag=0;_tmp2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
sg));{void*_tmp0[2]={& _tmp2,& _tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,46));}),
_tag_dynforward(_tmp0,sizeof(void*),2));}}});return;}static int Cyc_Parse_enum_counter=
0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmp4=_cycalloc(
sizeof(*_tmp4));_tmp4->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp9;(_tmp9.Rel_n).tag=1;(_tmp9.Rel_n).f1=0;_tmp9;});_tmp4->f2=({struct
_dynforward_ptr*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=(struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp8;_tmp8.tag=1;_tmp8.f1=(unsigned long)Cyc_Parse_enum_counter
++;{void*_tmp6[1]={& _tmp8};Cyc_aprintf(({const char*_tmp7="__anonymous_enum_%d__";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,22));}),
_tag_dynforward(_tmp6,sizeof(void*),1));}});_tmp5;});_tmp4;});}struct _tuple7{
struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct
Cyc_List_List*f5;};struct _tuple8{struct _tuple7*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple8*field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct
_tuple7 _tmpD;struct _tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct
Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;
struct _tuple8*_tmpA=field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=
_tmpD.f1;_tmpF=_tmpD.f2;_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=
_tmpB.f2;if(_tmp11 != 0)Cyc_Parse_err(({const char*_tmp14="bad type params in struct field";
_tag_dynforward(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,32));}),loc);if(
Cyc_Absyn_is_qvar_qualified(_tmpE))Cyc_Parse_err(({const char*_tmp15="struct or union field cannot be qualified with a namespace";
_tag_dynforward(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,59));}),loc);
return({struct Cyc_Absyn_Aggrfield*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->name=(*
_tmpE).f2;_tmp16->tq=_tmpF;_tmp16->type=(void*)_tmp10;_tmp16->width=_tmp13;
_tmp16->attributes=_tmp12;_tmp16;});}static void*Cyc_Parse_type_spec(void*t,
struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Parse_Type_spec_struct
_tmp18;_tmp18.tag=4;_tmp18.f1=(void*)t;_tmp18.f2=loc;_tmp18;});_tmp17;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo
_tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual _tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct
Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*_tmp1F;_LL1: if(_tmp19 <= (void*)
4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;
_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;_LL2: return Cyc_Absyn_starb_typ(
_tmp1B,argposn?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp20=_cycalloc(sizeof(*
_tmp20));_tmp20->v=(void*)((void*)3);_tmp20;}),0):(void*)2,_tmp1C,_tmp1D == 0?(
void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp21=_cycalloc(sizeof(*_tmp21));
_tmp21[0]=({struct Cyc_Absyn_Upper_b_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct
Cyc_Absyn_Exp*)_tmp1D;_tmp22;});_tmp21;}),_tmp1E);_LL3:;_LL4: return t;_LL0:;}
static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple9*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t){return({
struct _tuple9*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->f1=(*t).f2;_tmp23->f2=(*
t).f3;_tmp23;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmp24=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp25;_LL6: if(_tmp24 <= (void*)
2)goto _LL8;if(*((int*)_tmp24)!= 0)goto _LL8;_tmp25=((struct Cyc_Absyn_Var_d_struct*)
_tmp24)->f1;_LL7: return _tmp25;_LL8:;_LL9:({void*_tmp26[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(d->loc,({
const char*_tmp27="bad declaration in `forarray' statement";_tag_dynforward(
_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,40));}),_tag_dynforward(_tmp26,
sizeof(void*),0));});_LL5:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmp28=
tm;_LLB: if(*((int*)_tmp28)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}
static void*Cyc_Parse_id2type(struct _dynforward_ptr s,void*k){if(Cyc_zstrcmp((
struct _dynforward_ptr)s,({const char*_tmp29="`H";_tag_dynforward(_tmp29,sizeof(
char),_get_zero_arr_size(_tmp29,3));}))== 0)return(void*)2;else{if(Cyc_zstrcmp((
struct _dynforward_ptr)s,({const char*_tmp2A="`U";_tag_dynforward(_tmp2A,sizeof(
char),_get_zero_arr_size(_tmp2A,3));}))== 0)return(void*)3;else{return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=({
struct Cyc_Absyn_VarType_struct _tmp2C;_tmp2C.tag=1;_tmp2C.f1=({struct Cyc_Absyn_Tvar*
_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->name=({struct _dynforward_ptr*_tmp2E=
_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=s;_tmp2E;});_tmp2D->identity=0;_tmp2D->kind=(
void*)k;_tmp2D;});_tmp2C;});_tmp2B;});}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp2F=Cyc_Absyn_compress_kb((void*)t->kind);
void*_tmp30;void*_tmp31;_LL10: if(*((int*)_tmp2F)!= 0)goto _LL12;_tmp30=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp2F)->f1;_LL11: k=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp33;
_tmp33.tag=0;_tmp33.f1=(void*)_tmp30;_tmp33;});_tmp32;});goto _LLF;_LL12: if(*((
int*)_tmp2F)!= 1)goto _LL14;_LL13: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp35;_tmp35.tag=1;_tmp35.f1=0;_tmp35;});_tmp34;});goto _LLF;_LL14: if(*((int*)
_tmp2F)!= 2)goto _LLF;_tmp31=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2F)->f2;
_LL15: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp36=_cycalloc(sizeof(*_tmp36));
_tmp36[0]=({struct Cyc_Absyn_Less_kb_struct _tmp37;_tmp37.tag=2;_tmp37.f1=0;_tmp37.f2=(
void*)_tmp31;_tmp37;});_tmp36;});goto _LLF;_LLF:;}return({struct Cyc_Absyn_Tvar*
_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->name=t->name;_tmp38->identity=0;_tmp38->kind=(
void*)k;_tmp38;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp39=t;struct Cyc_Absyn_Tvar*_tmp3A;_LL17: if(_tmp39 <= (void*)4)
goto _LL19;if(*((int*)_tmp39)!= 1)goto _LL19;_tmp3A=((struct Cyc_Absyn_VarType_struct*)
_tmp39)->f1;_LL18: return _tmp3A;_LL19:;_LL1A:({void*_tmp3B[0]={};((int(*)(struct
Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(
loc,({const char*_tmp3C="expecting a list of type variables, not types";
_tag_dynforward(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,46));}),
_tag_dynforward(_tmp3B,sizeof(void*),0));});_LL16:;}static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*_tmp3D=
_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_VarType_struct _tmp3E;
_tmp3E.tag=1;_tmp3E.f1=pr;_tmp3E;});_tmp3D;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k,int leq){void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp40;struct Cyc_Absyn_Tvar _tmp41;void*_tmp42;void**_tmp43;_LL1C: if(_tmp3F <= (
void*)4)goto _LL1E;if(*((int*)_tmp3F)!= 1)goto _LL1E;_tmp40=((struct Cyc_Absyn_VarType_struct*)
_tmp3F)->f1;_tmp41=*_tmp40;_tmp42=(void*)_tmp41.kind;_tmp43=(void**)&(*((struct
Cyc_Absyn_VarType_struct*)_tmp3F)->f1).kind;_LL1D: {void*_tmp44=Cyc_Absyn_compress_kb(*
_tmp43);_LL21: if(*((int*)_tmp44)!= 1)goto _LL23;_LL22: if(!leq)*_tmp43=(void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp46;_tmp46.tag=0;_tmp46.f1=(void*)k;_tmp46;});
_tmp45;});else{*_tmp43=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp47=_cycalloc(
sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Absyn_Less_kb_struct _tmp48;_tmp48.tag=2;
_tmp48.f1=0;_tmp48.f2=(void*)k;_tmp48;});_tmp47;});}return;_LL23:;_LL24: return;
_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp49=(void*)tms->hd;void*_tmp4A;
_LL26: if(*((int*)_tmp49)!= 3)goto _LL28;_tmp4A=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp49)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp4B=_tmp4A;struct Cyc_List_List*_tmp4C;_LL2B: if(*((int*)_tmp4B)!= 1)
goto _LL2D;_LL2C:({void*_tmp4D[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp4E="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dynforward(_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,93));}),
_tag_dynforward(_tmp4D,sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp4B)!= 
0)goto _LL2A;_tmp4C=((struct Cyc_Absyn_NoTypes_struct*)_tmp4B)->f1;_LL2E: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp4C)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds))({void*_tmp4F[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp50="wrong number of parameter declarations in old-style function declaration";
_tag_dynforward(_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,73));}),
_tag_dynforward(_tmp4F,sizeof(void*),0));});{struct Cyc_List_List*_tmp51=0;for(0;
_tmp4C != 0;_tmp4C=_tmp4C->tl){struct Cyc_List_List*_tmp52=tds;for(0;_tmp52 != 0;
_tmp52=_tmp52->tl){struct Cyc_Absyn_Decl*_tmp53=(struct Cyc_Absyn_Decl*)_tmp52->hd;
void*_tmp54=(void*)_tmp53->r;struct Cyc_Absyn_Vardecl*_tmp55;_LL30: if(_tmp54 <= (
void*)2)goto _LL32;if(*((int*)_tmp54)!= 0)goto _LL32;_tmp55=((struct Cyc_Absyn_Var_d_struct*)
_tmp54)->f1;_LL31: if(Cyc_zstrptrcmp((*_tmp55->name).f2,(struct _dynforward_ptr*)
_tmp4C->hd)!= 0)continue;if(_tmp55->initializer != 0)({void*_tmp56[0]={};((int(*)(
struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Parse_abort)(_tmp53->loc,({const char*_tmp57="initializer found in parameter declaration";
_tag_dynforward(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,43));}),
_tag_dynforward(_tmp56,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(
_tmp55->name))({void*_tmp58[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp53->loc,({const
char*_tmp59="namespaces forbidden in parameter declarations";_tag_dynforward(
_tmp59,sizeof(char),_get_zero_arr_size(_tmp59,47));}),_tag_dynforward(_tmp58,
sizeof(void*),0));});_tmp51=({struct Cyc_List_List*_tmp5A=_cycalloc(sizeof(*
_tmp5A));_tmp5A->hd=({struct _tuple2*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->f1=({
struct Cyc_Core_Opt*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->v=(*_tmp55->name).f2;
_tmp5C;});_tmp5B->f2=_tmp55->tq;_tmp5B->f3=(void*)_tmp55->type;_tmp5B;});_tmp5A->tl=
_tmp51;_tmp5A;});goto L;_LL32:;_LL33:({void*_tmp5D[0]={};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp53->loc,({
const char*_tmp5E="nonvariable declaration in parameter type";_tag_dynforward(
_tmp5E,sizeof(char),_get_zero_arr_size(_tmp5E,42));}),_tag_dynforward(_tmp5D,
sizeof(void*),0));});_LL2F:;}L: if(_tmp52 == 0)({struct Cyc_String_pa_struct _tmp61;
_tmp61.tag=0;_tmp61.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_tmp4C->hd));{void*_tmp5F[1]={& _tmp61};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({
const char*_tmp60="%s is not given a type";_tag_dynforward(_tmp60,sizeof(char),
_get_zero_arr_size(_tmp60,23));}),_tag_dynforward(_tmp5F,sizeof(void*),1));}});}
return({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp63=_cycalloc(sizeof(*_tmp63));
_tmp63[0]=({struct Cyc_Absyn_Function_mod_struct _tmp64;_tmp64.tag=3;_tmp64.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp65=_cycalloc(sizeof(*_tmp65));
_tmp65[0]=({struct Cyc_Absyn_WithTypes_struct _tmp66;_tmp66.tag=1;_tmp66.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp51);_tmp66.f2=
0;_tmp66.f3=0;_tmp66.f4=0;_tmp66.f5=0;_tmp66;});_tmp65;}));_tmp64;});_tmp63;}));
_tmp62->tl=0;_tmp62;});}_LL2A:;}goto _LL29;_LL28:;_LL29: return({struct Cyc_List_List*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=(void*)((void*)tms->hd);_tmp67->tl=
Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp67;});_LL25:;}}struct _tuple10{
struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68->id=d->id;_tmp68->tms=Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc);_tmp68;});{
void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0){void*
_tmp69=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc))->v;_LL35: if((int)_tmp69 != 1)goto _LL37;_LL36: sc=(void*)3;goto _LL34;
_LL37: if((int)_tmp69 != 3)goto _LL39;_LL38: sc=(void*)0;goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({
const char*_tmp6A="bad storage class on function";_tag_dynforward(_tmp6A,sizeof(
char),_get_zero_arr_size(_tmp6A,30));}),loc);goto _LL34;_LL34:;}}{void*_tmp6C;
struct Cyc_Core_Opt*_tmp6D;struct _tuple5 _tmp6B=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;{struct Cyc_Absyn_Tqual _tmp6F;void*
_tmp70;struct Cyc_List_List*_tmp71;struct Cyc_List_List*_tmp72;struct _tuple6 _tmp6E=
Cyc_Parse_apply_tms(tq,_tmp6C,atts,d->tms);_tmp6F=_tmp6E.f1;_tmp70=_tmp6E.f2;
_tmp71=_tmp6E.f3;_tmp72=_tmp6E.f4;if(_tmp6D != 0)({void*_tmp73[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp74="nested type declaration within function prototype";
_tag_dynforward(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,50));}),
_tag_dynforward(_tmp73,sizeof(void*),0));});if(_tmp71 != 0)({void*_tmp75[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp76="bad type params, ignoring";
_tag_dynforward(_tmp76,sizeof(char),_get_zero_arr_size(_tmp76,26));}),
_tag_dynforward(_tmp75,sizeof(void*),0));});{void*_tmp77=_tmp70;struct Cyc_Absyn_FnInfo
_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_Core_Opt*_tmp7A;void*_tmp7B;struct
Cyc_List_List*_tmp7C;int _tmp7D;struct Cyc_Absyn_VarargInfo*_tmp7E;struct Cyc_List_List*
_tmp7F;struct Cyc_List_List*_tmp80;_LL3C: if(_tmp77 <= (void*)4)goto _LL3E;if(*((int*)
_tmp77)!= 8)goto _LL3E;_tmp78=((struct Cyc_Absyn_FnType_struct*)_tmp77)->f1;_tmp79=
_tmp78.tvars;_tmp7A=_tmp78.effect;_tmp7B=(void*)_tmp78.ret_typ;_tmp7C=_tmp78.args;
_tmp7D=_tmp78.c_varargs;_tmp7E=_tmp78.cyc_varargs;_tmp7F=_tmp78.rgn_po;_tmp80=
_tmp78.attributes;_LL3D: {struct Cyc_List_List*_tmp81=0;{struct Cyc_List_List*
_tmp82=_tmp7C;for(0;_tmp82 != 0;_tmp82=_tmp82->tl){struct _tuple2 _tmp84;struct Cyc_Core_Opt*
_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct _tuple2*_tmp83=(struct
_tuple2*)_tmp82->hd;_tmp84=*_tmp83;_tmp85=_tmp84.f1;_tmp86=_tmp84.f2;_tmp87=
_tmp84.f3;if(_tmp85 == 0){Cyc_Parse_err(({const char*_tmp88="missing argument variable in function prototype";
_tag_dynforward(_tmp88,sizeof(char),_get_zero_arr_size(_tmp88,48));}),loc);
_tmp81=({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->hd=({
struct _tuple10*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=_init_dynforward_ptr(
_cycalloc(sizeof(struct _dynforward_ptr)),"?",sizeof(char),2);_tmp8A->f2=_tmp86;
_tmp8A->f3=_tmp87;_tmp8A;});_tmp89->tl=_tmp81;_tmp89;});}else{_tmp81=({struct Cyc_List_List*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->hd=({struct _tuple10*_tmp8D=_cycalloc(
sizeof(*_tmp8D));_tmp8D->f1=(struct _dynforward_ptr*)_tmp85->v;_tmp8D->f2=_tmp86;
_tmp8D->f3=_tmp87;_tmp8D;});_tmp8C->tl=_tmp81;_tmp8C;});}}}return({struct Cyc_Absyn_Fndecl*
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->sc=(void*)sc;_tmp8E->name=d->id;_tmp8E->tvs=
_tmp79;_tmp8E->is_inline=is_inline;_tmp8E->effect=_tmp7A;_tmp8E->ret_type=(void*)
_tmp7B;_tmp8E->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp81);_tmp8E->c_varargs=_tmp7D;_tmp8E->cyc_varargs=_tmp7E;_tmp8E->rgn_po=
_tmp7F;_tmp8E->body=body;_tmp8E->cached_typ=0;_tmp8E->param_vardecls=0;_tmp8E->fn_vardecl=
0;_tmp8E->attributes=Cyc_List_append(_tmp80,_tmp72);_tmp8E;});}_LL3E:;_LL3F:({
void*_tmp8F[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp90="declarator is not a function prototype";
_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,39));}),
_tag_dynforward(_tmp8F,sizeof(void*),0));});_LL3B:;}}}}}static char _tmp91[52]="at most one type may appear within a type specifier";
static struct _dynforward_ptr Cyc_Parse_msg1={_tmp91,_tmp91 + 52};static char _tmp92[
63]="const or volatile may appear only once within a type specifier";static
struct _dynforward_ptr Cyc_Parse_msg2={_tmp92,_tmp92 + 63};static char _tmp93[50]="type specifier includes more than one declaration";
static struct _dynforward_ptr Cyc_Parse_msg3={_tmp93,_tmp93 + 50};static char _tmp94[
60]="sign specifier may appear only once within a type specifier";static struct
_dynforward_ptr Cyc_Parse_msg4={_tmp94,_tmp94 + 60};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)0;void*sz=(
void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;
ts=ts->tl){void*_tmp95=(void*)ts->hd;void*_tmp96;struct Cyc_Position_Segment*
_tmp97;struct Cyc_Position_Segment*_tmp98;struct Cyc_Position_Segment*_tmp99;
struct Cyc_Position_Segment*_tmp9A;struct Cyc_Position_Segment*_tmp9B;struct Cyc_Absyn_Decl*
_tmp9C;_LL41: if(*((int*)_tmp95)!= 4)goto _LL43;_tmp96=(void*)((struct Cyc_Parse_Type_spec_struct*)
_tmp95)->f1;_tmp97=((struct Cyc_Parse_Type_spec_struct*)_tmp95)->f2;_LL42: if(
seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp97);last_loc=_tmp97;seen_type=1;t=
_tmp96;goto _LL40;_LL43: if(*((int*)_tmp95)!= 0)goto _LL45;_tmp98=((struct Cyc_Parse_Signed_spec_struct*)
_tmp95)->f1;_LL44: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp98);if(seen_type)
Cyc_Parse_err(({const char*_tmp9D="signed qualifier must come before type";
_tag_dynforward(_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,39));}),_tmp98);
last_loc=_tmp98;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp95)!= 1)
goto _LL47;_tmp99=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp95)->f1;_LL46: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp99);if(seen_type)Cyc_Parse_err(({
const char*_tmp9E="signed qualifier must come before type";_tag_dynforward(_tmp9E,
sizeof(char),_get_zero_arr_size(_tmp9E,39));}),_tmp99);last_loc=_tmp99;seen_sign=
1;sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp95)!= 2)goto _LL49;_tmp9A=((struct
Cyc_Parse_Short_spec_struct*)_tmp95)->f1;_LL48: if(seen_size)Cyc_Parse_err(({
const char*_tmp9F="integral size may appear only once within a type specifier";
_tag_dynforward(_tmp9F,sizeof(char),_get_zero_arr_size(_tmp9F,59));}),_tmp9A);
if(seen_type)Cyc_Parse_err(({const char*_tmpA0="short modifier must come before base type";
_tag_dynforward(_tmpA0,sizeof(char),_get_zero_arr_size(_tmpA0,42));}),_tmp9A);
last_loc=_tmp9A;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp95)!= 3)
goto _LL4B;_tmp9B=((struct Cyc_Parse_Long_spec_struct*)_tmp95)->f1;_LL4A: if(
seen_type)Cyc_Parse_err(({const char*_tmpA1="long modifier must come before base type";
_tag_dynforward(_tmpA1,sizeof(char),_get_zero_arr_size(_tmpA1,41));}),_tmp9B);
if(seen_size){void*_tmpA2=sz;_LL4E: if((int)_tmpA2 != 3)goto _LL50;_LL4F: sz=(void*)
4;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(({const char*_tmpA3="extra long in type specifier";
_tag_dynforward(_tmpA3,sizeof(char),_get_zero_arr_size(_tmpA3,29));}),_tmp9B);
goto _LL4D;_LL4D:;}else{sz=(void*)3;}last_loc=_tmp9B;seen_size=1;goto _LL40;_LL4B:
if(*((int*)_tmp95)!= 5)goto _LL40;_tmp9C=((struct Cyc_Parse_Decl_spec_struct*)
_tmp95)->f1;_LL4C: last_loc=_tmp9C->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmpA4=(void*)_tmp9C->r;struct Cyc_Absyn_Aggrdecl*_tmpA5;struct Cyc_Absyn_Tuniondecl*
_tmpA6;struct Cyc_Absyn_Enumdecl*_tmpA7;_LL53: if(_tmpA4 <= (void*)2)goto _LL59;if(*((
int*)_tmpA4)!= 4)goto _LL55;_tmpA5=((struct Cyc_Absyn_Aggr_d_struct*)_tmpA4)->f1;
_LL54: {struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA5->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_AggrType_struct
_tmpAA;_tmpAA.tag=10;_tmpAA.f1=({struct Cyc_Absyn_AggrInfo _tmpAB;_tmpAB.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpAC;(_tmpAC.UnknownAggr).tag=
0;(_tmpAC.UnknownAggr).f1=(void*)((void*)_tmpA5->kind);(_tmpAC.UnknownAggr).f2=
_tmpA5->name;_tmpAC;});_tmpAB.targs=_tmpA8;_tmpAB;});_tmpAA;});_tmpA9;});if(
_tmpA5->impl != 0)declopt=({struct Cyc_Core_Opt*_tmpAD=_cycalloc(sizeof(*_tmpAD));
_tmpAD->v=_tmp9C;_tmpAD;});goto _LL52;}_LL55: if(*((int*)_tmpA4)!= 5)goto _LL57;
_tmpA6=((struct Cyc_Absyn_Tunion_d_struct*)_tmpA4)->f1;_LL56: {struct Cyc_List_List*
_tmpAE=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA6->tvs));struct Cyc_Core_Opt*ropt=_tmpA6->is_flat?0:({struct Cyc_Core_Opt*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->v=(void*)((void*)2);_tmpB5;});t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_TunionType_struct _tmpB0;_tmpB0.tag=2;_tmpB0.f1=({struct Cyc_Absyn_TunionInfo
_tmpB1;_tmpB1.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union
_tmpB2;(_tmpB2.KnownTunion).tag=1;(_tmpB2.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=_tmpA6;_tmpB3;});_tmpB2;});_tmpB1.targs=
_tmpAE;_tmpB1.rgn=ropt;_tmpB1;});_tmpB0;});_tmpAF;});if(_tmpA6->fields != 0)
declopt=({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->v=_tmp9C;
_tmpB4;});goto _LL52;}_LL57: if(*((int*)_tmpA4)!= 6)goto _LL59;_tmpA7=((struct Cyc_Absyn_Enum_d_struct*)
_tmpA4)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmpB6=_cycalloc(
sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_EnumType_struct _tmpB7;_tmpB7.tag=12;
_tmpB7.f1=_tmpA7->name;_tmpB7.f2=0;_tmpB7;});_tmpB6;});declopt=({struct Cyc_Core_Opt*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->v=_tmp9C;_tmpB8;});goto _LL52;_LL59:;
_LL5A:({void*_tmpB9[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp9C->loc,({const
char*_tmpBA="bad declaration within type specifier";_tag_dynforward(_tmpBA,
sizeof(char),_get_zero_arr_size(_tmpBA,38));}),_tag_dynforward(_tmpB9,sizeof(
void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp9C->loc);}goto _LL40;
_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({void*_tmpBB[0]={};Cyc_Tcutil_warn(
last_loc,({const char*_tmpBC="missing type within specifier";_tag_dynforward(
_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,30));}),_tag_dynforward(_tmpBB,
sizeof(void*),0));});t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpBD=_cycalloc(
sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_IntType_struct _tmpBE;_tmpBE.tag=5;
_tmpBE.f1=(void*)sgn;_tmpBE.f2=(void*)sz;_tmpBE;});_tmpBD;});}else{if(seen_sign){
void*_tmpBF=t;void*_tmpC0;void*_tmpC1;_LL5C: if(_tmpBF <= (void*)4)goto _LL5E;if(*((
int*)_tmpBF)!= 5)goto _LL5E;_tmpC0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBF)->f1;_tmpC1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBF)->f2;_LL5D:
if(_tmpC0 != sgn)t=(void*)({struct Cyc_Absyn_IntType_struct*_tmpC2=_cycalloc(
sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_IntType_struct _tmpC3;_tmpC3.tag=5;
_tmpC3.f1=(void*)sgn;_tmpC3.f2=(void*)_tmpC1;_tmpC3;});_tmpC2;});goto _LL5B;_LL5E:;
_LL5F: Cyc_Parse_err(({const char*_tmpC4="sign specification on non-integral type";
_tag_dynforward(_tmpC4,sizeof(char),_get_zero_arr_size(_tmpC4,40));}),last_loc);
goto _LL5B;_LL5B:;}if(seen_size){void*_tmpC5=t;void*_tmpC6;void*_tmpC7;_LL61: if(
_tmpC5 <= (void*)4)goto _LL65;if(*((int*)_tmpC5)!= 5)goto _LL63;_tmpC6=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpC5)->f1;_tmpC7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC5)->f2;_LL62: if(_tmpC7 != sz)t=(void*)({struct Cyc_Absyn_IntType_struct*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_IntType_struct
_tmpC9;_tmpC9.tag=5;_tmpC9.f1=(void*)_tmpC6;_tmpC9.f2=(void*)sz;_tmpC9;});_tmpC8;});
goto _LL60;_LL63: if(*((int*)_tmpC5)!= 6)goto _LL65;_LL64: t=(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmpCA=_cycalloc_atomic(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpCB;_tmpCB.tag=6;_tmpCB.f1=1;_tmpCB;});_tmpCA;});goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpCC="size qualifier on non-integral type";_tag_dynforward(_tmpCC,
sizeof(char),_get_zero_arr_size(_tmpCC,36));}),last_loc);goto _LL60;_LL60:;}}
return({struct _tuple5 _tmpCD;_tmpCD.f1=t;_tmpCD.f2=declopt;_tmpCD;});}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmpCE=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmpCF=_tmpCE->id;struct Cyc_Absyn_Tqual _tmpD1;void*_tmpD2;struct
Cyc_List_List*_tmpD3;struct Cyc_List_List*_tmpD4;struct _tuple6 _tmpD0=Cyc_Parse_apply_tms(
tq,t,shared_atts,_tmpCE->tms);_tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;_tmpD3=_tmpD0.f3;
_tmpD4=_tmpD0.f4;if(ds->tl == 0)return({struct Cyc_List_List*_tmpD5=_region_malloc(
r,sizeof(*_tmpD5));_tmpD5->hd=({struct _tuple7*_tmpD6=_region_malloc(r,sizeof(*
_tmpD6));_tmpD6->f1=_tmpCF;_tmpD6->f2=_tmpD1;_tmpD6->f3=_tmpD2;_tmpD6->f4=_tmpD3;
_tmpD6->f5=_tmpD4;_tmpD6;});_tmpD5->tl=0;_tmpD5;});else{return({struct Cyc_List_List*
_tmpD7=_region_malloc(r,sizeof(*_tmpD7));_tmpD7->hd=({struct _tuple7*_tmpD8=
_region_malloc(r,sizeof(*_tmpD8));_tmpD8->f1=_tmpCF;_tmpD8->f2=_tmpD1;_tmpD8->f3=
_tmpD2;_tmpD8->f4=_tmpD3;_tmpD8->f5=_tmpD4;_tmpD8;});_tmpD7->tl=Cyc_Parse_apply_tmss(
r,_tmpD1,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpD7;});}}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpD9;_tmpD9.f1=
tq;_tmpD9.f2=t;_tmpD9.f3=0;_tmpD9.f4=atts;_tmpD9;});{void*_tmpDA=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpDB;struct Cyc_Position_Segment*_tmpDC;struct Cyc_Absyn_Exp*
_tmpDD;struct Cyc_Absyn_Conref*_tmpDE;struct Cyc_Position_Segment*_tmpDF;void*
_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_Position_Segment*_tmpE2;struct Cyc_Absyn_PtrAtts
_tmpE3;struct Cyc_Absyn_Tqual _tmpE4;struct Cyc_Position_Segment*_tmpE5;struct Cyc_List_List*
_tmpE6;_LL68: if(*((int*)_tmpDA)!= 0)goto _LL6A;_tmpDB=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_Carray_mod_struct*)_tmpDA)->f2;_LL69: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpDB,
_tmpDC),atts,tms->tl);_LL6A: if(*((int*)_tmpDA)!= 1)goto _LL6C;_tmpDD=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpDA)->f1;_tmpDE=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpDA)->f2;_tmpDF=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpDA)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpDD,
_tmpDE,_tmpDF),atts,tms->tl);_LL6C: if(*((int*)_tmpDA)!= 3)goto _LL6E;_tmpE0=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpDA)->f1;_LL6D: {void*_tmpE7=_tmpE0;
struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_Core_Opt*
_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Position_Segment*_tmpED;_LL75: if(*((
int*)_tmpE7)!= 1)goto _LL77;_tmpE8=((struct Cyc_Absyn_WithTypes_struct*)_tmpE7)->f1;
_tmpE9=((struct Cyc_Absyn_WithTypes_struct*)_tmpE7)->f2;_tmpEA=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE7)->f3;_tmpEB=((struct Cyc_Absyn_WithTypes_struct*)_tmpE7)->f4;_tmpEC=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE7)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->hd=(void*)((
void*)as->hd);_tmpEE->tl=fn_atts;_tmpEE;});else{new_atts=({struct Cyc_List_List*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->hd=(void*)((void*)as->hd);_tmpEF->tl=
new_atts;_tmpEF;});}}}if(tms->tl != 0){void*_tmpF0=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpF1;_LL7A: if(*((int*)_tmpF0)!= 
4)goto _LL7C;_tmpF1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpF0)->f1;_LL7B:
typvars=_tmpF1;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpE9  && _tmpEA == 0) && _tmpE8 != 0) && _tmpE8->tl == 0) && (*((struct _tuple2*)
_tmpE8->hd)).f1 == 0) && (*((struct _tuple2*)_tmpE8->hd)).f3 == (void*)0)_tmpE8=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpE8);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpEB,t,_tmpE8,_tmpE9,_tmpEA,_tmpEC,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpE7)!= 0)goto _LL74;_tmpED=((struct Cyc_Absyn_NoTypes_struct*)_tmpE7)->f2;_LL78:({
void*_tmpF2[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpED,({const char*_tmpF3="function declaration without parameter types";
_tag_dynforward(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,45));}),
_tag_dynforward(_tmpF2,sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpDA)!= 4)
goto _LL70;_tmpE1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpDA)->f1;_tmpE2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpDA)->f2;_LL6F: if(tms->tl == 0)return({
struct _tuple6 _tmpF4;_tmpF4.f1=tq;_tmpF4.f2=t;_tmpF4.f3=_tmpE1;_tmpF4.f4=atts;
_tmpF4;});({void*_tmpF5[0]={};((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpE2,({const char*
_tmpF6="type parameters must appear before function arguments in declarator";
_tag_dynforward(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,68));}),
_tag_dynforward(_tmpF5,sizeof(void*),0));});_LL70: if(*((int*)_tmpDA)!= 2)goto
_LL72;_tmpE3=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpDA)->f1;_tmpE4=((struct
Cyc_Absyn_Pointer_mod_struct*)_tmpDA)->f2;_LL71: return Cyc_Parse_apply_tms(_tmpE4,(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));
_tmpF7[0]=({struct Cyc_Absyn_PointerType_struct _tmpF8;_tmpF8.tag=4;_tmpF8.f1=({
struct Cyc_Absyn_PtrInfo _tmpF9;_tmpF9.elt_typ=(void*)t;_tmpF9.elt_tq=tq;_tmpF9.ptr_atts=
_tmpE3;_tmpF9;});_tmpF8;});_tmpF7;}),atts,tms->tl);_LL72: if(*((int*)_tmpDA)!= 5)
goto _LL67;_tmpE5=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpDA)->f1;_tmpE6=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpDA)->f2;_LL73: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpE6),tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){void*_tmpFB;struct Cyc_Core_Opt*
_tmpFC;struct _tuple5 _tmpFA=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpFB=
_tmpFA.f1;_tmpFC=_tmpFA.f2;if(_tmpFC != 0)({void*_tmpFD[0]={};Cyc_Tcutil_warn(loc,({
const char*_tmpFE="ignoring nested type declaration(s) in specifier list";
_tag_dynforward(_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,54));}),
_tag_dynforward(_tmpFD,sizeof(void*),0));});return _tmpFB;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmp100;struct _tuple1*_tmp101;struct Cyc_Absyn_Tqual _tmp102;void*
_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp105;struct _tuple7*
_tmpFF=t;_tmp100=*_tmpFF;_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;_tmp103=_tmp100.f3;
_tmp104=_tmp100.f4;_tmp105=_tmp100.f5;Cyc_Lex_register_typedef(_tmp101);{struct
Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{void*_tmp106=_tmp103;struct Cyc_Core_Opt*
_tmp107;_LL7F: if(_tmp106 <= (void*)4)goto _LL81;if(*((int*)_tmp106)!= 0)goto _LL81;
_tmp107=((struct Cyc_Absyn_Evar_struct*)_tmp106)->f1;_LL80: type=0;if(_tmp107 == 0)
kind=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->v=(void*)((
void*)2);_tmp108;});else{kind=_tmp107;}goto _LL7E;_LL81:;_LL82: kind=0;type=({
struct Cyc_Core_Opt*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->v=(void*)_tmp103;
_tmp109;});goto _LL7E;_LL7E:;}return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Typedef_d_struct
_tmp10B;_tmp10B.tag=7;_tmp10B.f1=({struct Cyc_Absyn_Typedefdecl*_tmp10C=_cycalloc(
sizeof(*_tmp10C));_tmp10C->name=_tmp101;_tmp10C->tvs=_tmp104;_tmp10C->kind=kind;
_tmp10C->defn=type;_tmp10C->atts=_tmp105;_tmp10C->tq=_tmp102;_tmp10C;});_tmp10B;});
_tmp10A;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp10E;_tmp10E.tag=11;_tmp10E.f1=d;_tmp10E.f2=s;_tmp10E;});_tmp10D;}),d->loc);}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp10F=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp10F;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp111;struct Cyc_Absyn_Tqual
_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_Parse_Declaration_spec*
_tmp110=ds;_tmp111=*_tmp110;_tmp112=_tmp111.tq;_tmp113=_tmp111.type_specs;
_tmp114=_tmp111.attributes;if(_tmp112.loc == 0)_tmp112.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp115="inline is allowed only on function definitions";
_tag_dynforward(_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,47));}),loc);if(
_tmp113 == 0){Cyc_Parse_err(({const char*_tmp116="missing type specifiers in declaration";
_tag_dynforward(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,39));}),loc);{
struct Cyc_List_List*_tmp117=0;_npop_handler(0);return _tmp117;}}{void*s=(void*)2;
int istypedef=0;if(ds->sc != 0){void*_tmp118=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL84: if((int)_tmp118 != 0)goto _LL86;_LL85: istypedef=1;
goto _LL83;_LL86: if((int)_tmp118 != 1)goto _LL88;_LL87: s=(void*)3;goto _LL83;_LL88:
if((int)_tmp118 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A: if((int)_tmp118 != 
3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp118 != 4)goto _LL8E;_LL8D:
s=(void*)2;goto _LL83;_LL8E: if((int)_tmp118 != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)_tmp118 != 6)goto _LL83;_LL91:
s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp11A;struct Cyc_List_List*
_tmp11B;struct _tuple0 _tmp119=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);
_tmp11A=_tmp119.f1;_tmp11B=_tmp119.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmp11B;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=
0;break;}}}{struct _tuple5 _tmp11C=Cyc_Parse_collapse_type_specifiers(_tmp113,loc);
if(_tmp11A == 0){void*_tmp11E;struct Cyc_Core_Opt*_tmp11F;struct _tuple5 _tmp11D=
_tmp11C;_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;if(_tmp11F != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp11F->v;{void*_tmp120=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmp121;struct Cyc_Absyn_Aggrdecl*_tmp122;struct Cyc_Absyn_Tuniondecl*_tmp123;
_LL93: if(_tmp120 <= (void*)2)goto _LL99;if(*((int*)_tmp120)!= 6)goto _LL95;_tmp121=((
struct Cyc_Absyn_Enum_d_struct*)_tmp120)->f1;_LL94:(void*)(_tmp121->sc=(void*)s);
if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp124="bad attributes on enum";
_tag_dynforward(_tmp124,sizeof(char),_get_zero_arr_size(_tmp124,23));}),loc);
goto _LL92;_LL95: if(*((int*)_tmp120)!= 4)goto _LL97;_tmp122=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp120)->f1;_LL96:(void*)(_tmp122->sc=(void*)s);_tmp122->attributes=_tmp114;
goto _LL92;_LL97: if(*((int*)_tmp120)!= 5)goto _LL99;_tmp123=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp120)->f1;_LL98:(void*)(_tmp123->sc=(void*)s);if(_tmp114 != 0)Cyc_Parse_err(({
const char*_tmp125="bad attributes on tunion";_tag_dynforward(_tmp125,sizeof(char),
_get_zero_arr_size(_tmp125,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp126="bad declaration";_tag_dynforward(_tmp126,sizeof(char),
_get_zero_arr_size(_tmp126,16));}),loc);{struct Cyc_List_List*_tmp127=0;
_npop_handler(0);return _tmp127;}_LL92:;}{struct Cyc_List_List*_tmp129=({struct Cyc_List_List*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->hd=d;_tmp128->tl=0;_tmp128;});
_npop_handler(0);return _tmp129;}}else{void*_tmp12A=_tmp11E;struct Cyc_Absyn_AggrInfo
_tmp12B;union Cyc_Absyn_AggrInfoU_union _tmp12C;void*_tmp12D;struct _tuple1*_tmp12E;
struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_TunionInfo _tmp130;union Cyc_Absyn_TunionInfoU_union
_tmp131;struct Cyc_Absyn_Tuniondecl**_tmp132;struct Cyc_Absyn_TunionInfo _tmp133;
union Cyc_Absyn_TunionInfoU_union _tmp134;struct Cyc_Absyn_UnknownTunionInfo _tmp135;
struct _tuple1*_tmp136;int _tmp137;int _tmp138;struct Cyc_List_List*_tmp139;struct
_tuple1*_tmp13A;struct Cyc_List_List*_tmp13B;_LL9C: if(_tmp12A <= (void*)4)goto
_LLA6;if(*((int*)_tmp12A)!= 10)goto _LL9E;_tmp12B=((struct Cyc_Absyn_AggrType_struct*)
_tmp12A)->f1;_tmp12C=_tmp12B.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp12A)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL9E;_tmp12D=(_tmp12C.UnknownAggr).f1;
_tmp12E=(_tmp12C.UnknownAggr).f2;_tmp12F=_tmp12B.targs;_LL9D: {struct Cyc_List_List*
_tmp13C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp12F);struct Cyc_Absyn_Aggrdecl*_tmp13D=({struct Cyc_Absyn_Aggrdecl*_tmp143=
_cycalloc(sizeof(*_tmp143));_tmp143->kind=(void*)_tmp12D;_tmp143->sc=(void*)s;
_tmp143->name=_tmp12E;_tmp143->tvs=_tmp13C;_tmp143->impl=0;_tmp143->attributes=0;
_tmp143;});if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp13E="bad attributes on type declaration";
_tag_dynforward(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,35));}),loc);{
struct Cyc_List_List*_tmp142=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*
_tmp13F));_tmp13F->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp141;_tmp141.tag=4;_tmp141.f1=_tmp13D;_tmp141;});_tmp140;}),loc);_tmp13F->tl=
0;_tmp13F;});_npop_handler(0);return _tmp142;}}_LL9E: if(*((int*)_tmp12A)!= 2)goto
_LLA0;_tmp130=((struct Cyc_Absyn_TunionType_struct*)_tmp12A)->f1;_tmp131=_tmp130.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp12A)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LLA0;_tmp132=(_tmp131.KnownTunion).f1;_LL9F: if(_tmp114 != 0)Cyc_Parse_err(({
const char*_tmp144="bad attributes on tunion";_tag_dynforward(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,25));}),loc);{struct Cyc_List_List*_tmp148=({struct Cyc_List_List*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Tunion_d_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146[0]=({
struct Cyc_Absyn_Tunion_d_struct _tmp147;_tmp147.tag=5;_tmp147.f1=*_tmp132;_tmp147;});
_tmp146;}),loc);_tmp145->tl=0;_tmp145;});_npop_handler(0);return _tmp148;}_LLA0:
if(*((int*)_tmp12A)!= 2)goto _LLA2;_tmp133=((struct Cyc_Absyn_TunionType_struct*)
_tmp12A)->f1;_tmp134=_tmp133.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp12A)->f1).tunion_info).UnknownTunion).tag != 0)goto _LLA2;_tmp135=(_tmp134.UnknownTunion).f1;
_tmp136=_tmp135.name;_tmp137=_tmp135.is_xtunion;_tmp138=_tmp135.is_flat;_tmp139=
_tmp133.targs;_LLA1: {struct Cyc_List_List*_tmp149=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp139);
struct Cyc_Absyn_Decl*_tmp14A=Cyc_Absyn_tunion_decl(s,_tmp136,_tmp149,0,_tmp137,
_tmp138,loc);if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp14B="bad attributes on tunion";
_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,25));}),loc);{
struct Cyc_List_List*_tmp14D=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*
_tmp14C));_tmp14C->hd=_tmp14A;_tmp14C->tl=0;_tmp14C;});_npop_handler(0);return
_tmp14D;}}_LLA2: if(*((int*)_tmp12A)!= 12)goto _LLA4;_tmp13A=((struct Cyc_Absyn_EnumType_struct*)
_tmp12A)->f1;_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp14E=({struct Cyc_Absyn_Enumdecl*
_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->sc=(void*)s;_tmp155->name=_tmp13A;
_tmp155->fields=0;_tmp155;});if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp14F="bad attributes on enum";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,23));}),loc);{
struct Cyc_List_List*_tmp154=({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*
_tmp150));_tmp150->hd=({struct Cyc_Absyn_Decl*_tmp151=_cycalloc(sizeof(*_tmp151));
_tmp151->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp152=_cycalloc(
sizeof(*_tmp152));_tmp152[0]=({struct Cyc_Absyn_Enum_d_struct _tmp153;_tmp153.tag=
6;_tmp153.f1=_tmp14E;_tmp153;});_tmp152;}));_tmp151->loc=loc;_tmp151;});_tmp150->tl=
0;_tmp150;});_npop_handler(0);return _tmp154;}}_LLA4: if(*((int*)_tmp12A)!= 13)
goto _LLA6;_tmp13B=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp12A)->f1;_LLA5: {
struct Cyc_Absyn_Enumdecl*_tmp156=({struct Cyc_Absyn_Enumdecl*_tmp15D=_cycalloc(
sizeof(*_tmp15D));_tmp15D->sc=(void*)s;_tmp15D->name=Cyc_Parse_gensym_enum();
_tmp15D->fields=({struct Cyc_Core_Opt*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->v=
_tmp13B;_tmp15E;});_tmp15D;});if(_tmp114 != 0)Cyc_Parse_err(({const char*_tmp157="bad attributes on enum";
_tag_dynforward(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,23));}),loc);{
struct Cyc_List_List*_tmp15C=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*
_tmp158));_tmp158->hd=({struct Cyc_Absyn_Decl*_tmp159=_cycalloc(sizeof(*_tmp159));
_tmp159->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp15A=_cycalloc(
sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_Absyn_Enum_d_struct _tmp15B;_tmp15B.tag=
6;_tmp15B.f1=_tmp156;_tmp15B;});_tmp15A;}));_tmp159->loc=loc;_tmp159;});_tmp158->tl=
0;_tmp158;});_npop_handler(0);return _tmp15C;}}_LLA6:;_LLA7: Cyc_Parse_err(({const
char*_tmp15F="missing declarator";_tag_dynforward(_tmp15F,sizeof(char),
_get_zero_arr_size(_tmp15F,19));}),loc);{struct Cyc_List_List*_tmp160=0;
_npop_handler(0);return _tmp160;}_LL9B:;}}else{void*t=_tmp11C.f1;struct Cyc_List_List*
_tmp161=Cyc_Parse_apply_tmss(mkrgn,_tmp112,t,_tmp11A,_tmp114);if(istypedef){if(!
exps_empty)Cyc_Parse_err(({const char*_tmp162="initializer in typedef declaration";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,35));}),loc);{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(
struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp161);
if(_tmp11C.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmp11C.f2)->v;{
void*_tmp163=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp164;struct Cyc_Absyn_Tuniondecl*
_tmp165;struct Cyc_Absyn_Enumdecl*_tmp166;_LLA9: if(_tmp163 <= (void*)2)goto _LLAF;
if(*((int*)_tmp163)!= 4)goto _LLAB;_tmp164=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp163)->f1;_LLAA:(void*)(_tmp164->sc=(void*)s);_tmp164->attributes=_tmp114;
_tmp114=0;goto _LLA8;_LLAB: if(*((int*)_tmp163)!= 5)goto _LLAD;_tmp165=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp163)->f1;_LLAC:(void*)(_tmp165->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp163)!= 6)goto _LLAF;_tmp166=((struct Cyc_Absyn_Enum_d_struct*)_tmp163)->f1;
_LLAE:(void*)(_tmp166->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp167="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,70));}),loc);
goto _LLA8;_LLA8:;}decls=({struct Cyc_List_List*_tmp168=_cycalloc(sizeof(*_tmp168));
_tmp168->hd=d;_tmp168->tl=decls;_tmp168;});}{struct Cyc_List_List*_tmp169=decls;
_npop_handler(0);return _tmp169;}}}else{if(_tmp11C.f2 != 0)Cyc_Parse_unimp(({const
char*_tmp16A="nested type declaration within declarator";_tag_dynforward(_tmp16A,
sizeof(char),_get_zero_arr_size(_tmp16A,42));}),loc);{struct Cyc_List_List*decls=
0;{struct Cyc_List_List*_tmp16B=_tmp161;for(0;_tmp16B != 0;(_tmp16B=_tmp16B->tl,
_tmp11B=_tmp11B->tl)){struct _tuple7 _tmp16D;struct _tuple1*_tmp16E;struct Cyc_Absyn_Tqual
_tmp16F;void*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List*_tmp172;
struct _tuple7*_tmp16C=(struct _tuple7*)_tmp16B->hd;_tmp16D=*_tmp16C;_tmp16E=
_tmp16D.f1;_tmp16F=_tmp16D.f2;_tmp170=_tmp16D.f3;_tmp171=_tmp16D.f4;_tmp172=
_tmp16D.f5;if(_tmp171 != 0)({void*_tmp173[0]={};Cyc_Tcutil_warn(loc,({const char*
_tmp174="bad type params, ignoring";_tag_dynforward(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,26));}),_tag_dynforward(_tmp173,sizeof(void*),0));});
if(_tmp11B == 0)({void*_tmp175[0]={};((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp176="unexpected NULL in parse!";_tag_dynforward(_tmp176,sizeof(char),
_get_zero_arr_size(_tmp176,26));}),_tag_dynforward(_tmp175,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp177=(struct Cyc_Absyn_Exp*)_tmp11B->hd;struct Cyc_Absyn_Vardecl*
_tmp178=Cyc_Absyn_new_vardecl(_tmp16E,_tmp170,_tmp177);_tmp178->tq=_tmp16F;(void*)(
_tmp178->sc=(void*)s);_tmp178->attributes=_tmp172;{struct Cyc_Absyn_Decl*_tmp179=({
struct Cyc_Absyn_Decl*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[
0]=({struct Cyc_Absyn_Var_d_struct _tmp17D;_tmp17D.tag=0;_tmp17D.f1=_tmp178;
_tmp17D;});_tmp17C;}));_tmp17B->loc=loc;_tmp17B;});decls=({struct Cyc_List_List*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->hd=_tmp179;_tmp17A->tl=decls;
_tmp17A;});}}}}{struct Cyc_List_List*_tmp17E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp17E;}}}}}}}}};_pop_region(
mkrgn);}static void*Cyc_Parse_id_to_kind(struct _dynforward_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dynforward_ptr)s)== 1  || Cyc_strlen((struct
_dynforward_ptr)s)== 2)switch(*((const char*)_check_dynforward_subscript(s,
sizeof(char),0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case
'B': _LLB3: return(void*)2;case 'R': _LLB4: return(void*)3;case 'U': _LLB5: if(*((const
char*)_check_dynforward_subscript(s,sizeof(char),1))== 'R')return(void*)4;else{
break;}case 'T': _LLB6: if(*((const char*)_check_dynforward_subscript(s,sizeof(char),
1))== 'R')return(void*)5;else{break;}case 'E': _LLB7: return(void*)6;case 'I': _LLB8:
return(void*)7;default: _LLB9: break;}Cyc_Parse_err((struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp182;_tmp182.tag=1;_tmp182.f1=(unsigned long)((int)Cyc_strlen((
struct _dynforward_ptr)s));{struct Cyc_String_pa_struct _tmp181;_tmp181.tag=0;
_tmp181.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp17F[2]={&
_tmp181,& _tmp182};Cyc_aprintf(({const char*_tmp180="bad kind: %s; strlen=%d";
_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,24));}),
_tag_dynforward(_tmp17F,sizeof(void*),2));}}}),loc);return(void*)2;}static struct
Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp183=(void*)p->r;
struct _tuple1*_tmp184;struct Cyc_Absyn_Vardecl*_tmp185;struct Cyc_Absyn_Pat*
_tmp186;struct Cyc_Absyn_Pat _tmp187;void*_tmp188;struct Cyc_Absyn_Pat*_tmp189;void*
_tmp18A;int _tmp18B;char _tmp18C;struct _dynforward_ptr _tmp18D;struct _tuple1*
_tmp18E;struct Cyc_List_List*_tmp18F;int _tmp190;struct Cyc_Absyn_Exp*_tmp191;_LLBC:
if(_tmp183 <= (void*)2)goto _LLC2;if(*((int*)_tmp183)!= 12)goto _LLBE;_tmp184=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp183)->f1;_LLBD: return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp192=_cycalloc(sizeof(*_tmp192));
_tmp192[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp193;_tmp193.tag=2;_tmp193.f1=
_tmp184;_tmp193;});_tmp192;}),p->loc);_LLBE: if(*((int*)_tmp183)!= 1)goto _LLC0;
_tmp185=((struct Cyc_Absyn_Reference_p_struct*)_tmp183)->f1;_tmp186=((struct Cyc_Absyn_Reference_p_struct*)
_tmp183)->f2;_tmp187=*_tmp186;_tmp188=(void*)_tmp187.r;if((int)_tmp188 != 0)goto
_LLC0;_LLBF: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp195;_tmp195.tag=2;_tmp195.f1=_tmp185->name;_tmp195;});_tmp194;}),p->loc),p->loc);
_LLC0: if(*((int*)_tmp183)!= 4)goto _LLC2;_tmp189=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp183)->f1;_LLC1: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp189),p->loc);
_LLC2: if((int)_tmp183 != 1)goto _LLC4;_LLC3: return Cyc_Absyn_null_exp(p->loc);_LLC4:
if(_tmp183 <= (void*)2)goto _LLCE;if(*((int*)_tmp183)!= 7)goto _LLC6;_tmp18A=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp183)->f1;_tmp18B=((struct Cyc_Absyn_Int_p_struct*)
_tmp183)->f2;_LLC5: return Cyc_Absyn_int_exp(_tmp18A,_tmp18B,p->loc);_LLC6: if(*((
int*)_tmp183)!= 8)goto _LLC8;_tmp18C=((struct Cyc_Absyn_Char_p_struct*)_tmp183)->f1;
_LLC7: return Cyc_Absyn_char_exp(_tmp18C,p->loc);_LLC8: if(*((int*)_tmp183)!= 9)
goto _LLCA;_tmp18D=((struct Cyc_Absyn_Float_p_struct*)_tmp183)->f1;_LLC9: return Cyc_Absyn_float_exp(
_tmp18D,p->loc);_LLCA: if(*((int*)_tmp183)!= 13)goto _LLCC;_tmp18E=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp183)->f1;_tmp18F=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp183)->f2;
_tmp190=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp183)->f3;if(_tmp190 != 0)goto
_LLCC;_LLCB: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp197;_tmp197.tag=2;_tmp197.f1=_tmp18E;_tmp197;});_tmp196;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp18F);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCC: if(*((int*)_tmp183)!= 14)goto _LLCE;_tmp191=((struct Cyc_Absyn_Exp_p_struct*)
_tmp183)->f1;_LLCD: return _tmp191;_LLCE:;_LLCF: Cyc_Parse_err(({const char*_tmp198="cannot mix patterns and expressions in case";
_tag_dynforward(_tmp198,sizeof(char),_get_zero_arr_size(_tmp198,44));}),p->loc);
return Cyc_Absyn_null_exp(p->loc);_LLBB:;}struct _tuple11{void*f1;int f2;};struct
Cyc_Int_tok_struct{int tag;struct _tuple11 f1;};struct Cyc_Char_tok_struct{int tag;
char f1;};struct Cyc_String_tok_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*
f1;};struct _tuple12{struct Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;
struct Cyc_Absyn_Conref*f3;};struct Cyc_YY1_struct{int tag;struct _tuple12*f1;};
struct Cyc_YY2_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_YY5_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_YY10_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct _tuple13{struct Cyc_List_List*f1;int f2;};
struct Cyc_YY12_struct{int tag;struct _tuple13*f1;};struct Cyc_YY13_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_YY14_struct{int tag;struct _tuple14*f1;};struct Cyc_YY15_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY16_struct{int tag;struct _tuple13*f1;};
struct Cyc_YY17_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY19_struct{int tag;struct Cyc_Parse_Declaration_spec*
f1;};struct _tuple15{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_YY20_struct{int tag;struct _tuple15*f1;};struct Cyc_YY21_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{
int tag;void*f1;};struct Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{
int tag;struct Cyc_Absyn_Tqual f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY27_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Parse_Declarator*
f1;};struct Cyc_YY30_struct{int tag;struct Cyc_Parse_Abstractdeclarator*f1;};struct
Cyc_YY31_struct{int tag;int f1;};struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{
int tag;struct Cyc_Absyn_Tunionfield*f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_YY35_struct{int tag;struct _tuple16*f1;};struct Cyc_YY36_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};
struct Cyc_YY38_struct{int tag;struct Cyc_List_List*f1;};struct _tuple17{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_YY39_struct{int tag;struct _tuple17*f1;};struct Cyc_YY40_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};
struct Cyc_YY44_struct{int tag;void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY46_struct{int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct
Cyc_Absyn_Enumfield*f1;};struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY49_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_YY52_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{
int tag;};union Cyc_YYSTYPE_union{struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct
Char_tok;struct Cyc_String_tok_struct String_tok;struct Cyc_Stringopt_tok_struct
Stringopt_tok;struct Cyc_QualId_tok_struct QualId_tok;struct Cyc_YY1_struct YY1;
struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct YY3;struct Cyc_YY4_struct YY4;struct
Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct
YY8;struct Cyc_YY9_struct YY9;struct Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;
struct Cyc_YY12_struct YY12;struct Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;
struct Cyc_YY15_struct YY15;struct Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;
struct Cyc_YY18_struct YY18;struct Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;
struct Cyc_YY21_struct YY21;struct Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;
struct Cyc_YY24_struct YY24;struct Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;
struct Cyc_YY27_struct YY27;struct Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;
struct Cyc_YY30_struct YY30;struct Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;
struct Cyc_YY33_struct YY33;struct Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;
struct Cyc_YY36_struct YY36;struct Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;
struct Cyc_YY39_struct YY39;struct Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;
struct Cyc_YY42_struct YY42;struct Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;
struct Cyc_YY45_struct YY45;struct Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;
struct Cyc_YY48_struct YY48;struct Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;
struct Cyc_YY51_struct YY51;struct Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct
YYINITIALSVAL;};static char _tmp19A[14]="$(sign_t,int)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp19A,_tmp19A + 14}};struct _tuple11 Cyc_yyget_Int_tok(
union Cyc_YYSTYPE_union yy1){struct _tuple11 yyzzz;{union Cyc_YYSTYPE_union _tmp19B=
yy1;struct _tuple11 _tmp19C;_LLD1: if((_tmp19B.Int_tok).tag != 0)goto _LLD3;_tmp19C=(
_tmp19B.Int_tok).f1;_LLD2: yyzzz=_tmp19C;goto _LLD0;_LLD3:;_LLD4:(int)_throw((void*)&
Cyc_yyfail_Int_tok);_LLD0:;}return yyzzz;}static char _tmp19E[5]="char";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp19E,
_tmp19E + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1){char yyzzz;{union
Cyc_YYSTYPE_union _tmp19F=yy1;char _tmp1A0;_LLD6: if((_tmp19F.Char_tok).tag != 1)
goto _LLD8;_tmp1A0=(_tmp19F.Char_tok).f1;_LLD7: yyzzz=_tmp1A0;goto _LLD5;_LLD8:;
_LLD9:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD5:;}return yyzzz;}static char
_tmp1A2[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp1A2,_tmp1A2 + 9}};struct _dynforward_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE_union yy1){struct _dynforward_ptr yyzzz;{union Cyc_YYSTYPE_union
_tmp1A3=yy1;struct _dynforward_ptr _tmp1A4;_LLDB: if((_tmp1A3.String_tok).tag != 2)
goto _LLDD;_tmp1A4=(_tmp1A3.String_tok).f1;_LLDC: yyzzz=_tmp1A4;goto _LLDA;_LLDD:;
_LLDE:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLDA:;}return yyzzz;}static char
_tmp1A6[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1A6,_tmp1A6 + 54}};
struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){struct _tuple12*yyzzz;{
union Cyc_YYSTYPE_union _tmp1A7=yy1;struct _tuple12*_tmp1A8;_LLE0: if((_tmp1A7.YY1).tag
!= 5)goto _LLE2;_tmp1A8=(_tmp1A7.YY1).f1;_LLE1: yyzzz=_tmp1A8;goto _LLDF;_LLE2:;
_LLE3:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDF:;}return yyzzz;}static char _tmp1AA[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp1AA,_tmp1AA + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union _tmp1AB=yy1;struct Cyc_Absyn_Conref*
_tmp1AC;_LLE5: if((_tmp1AB.YY2).tag != 6)goto _LLE7;_tmp1AC=(_tmp1AB.YY2).f1;_LLE6:
yyzzz=_tmp1AC;goto _LLE4;_LLE7:;_LLE8:(int)_throw((void*)& Cyc_yyfail_YY2);_LLE4:;}
return yyzzz;}static char _tmp1AE[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp1AE,_tmp1AE + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union
Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1AF=
yy1;struct Cyc_Absyn_Exp*_tmp1B0;_LLEA: if((_tmp1AF.YY3).tag != 7)goto _LLEC;_tmp1B0=(
_tmp1AF.YY3).f1;_LLEB: yyzzz=_tmp1B0;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)&
Cyc_yyfail_YY3);_LLE9:;}return yyzzz;}static char _tmp1B2[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1B2,_tmp1B2 + 
10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE_union _tmp1B3=yy1;struct Cyc_Absyn_Exp*_tmp1B4;_LLEF: if((
_tmp1B3.YY4).tag != 8)goto _LLF1;_tmp1B4=(_tmp1B3.YY4).f1;_LLF0: yyzzz=_tmp1B4;goto
_LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_YY4);_LLEE:;}return yyzzz;}
static char _tmp1B6[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp1B6,_tmp1B6 + 14}};struct Cyc_List_List*Cyc_yyget_YY5(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1B7=
yy1;struct Cyc_List_List*_tmp1B8;_LLF4: if((_tmp1B7.YY5).tag != 9)goto _LLF6;_tmp1B8=(
_tmp1B7.YY5).f1;_LLF5: yyzzz=_tmp1B8;goto _LLF3;_LLF6:;_LLF7:(int)_throw((void*)&
Cyc_yyfail_YY5);_LLF3:;}return yyzzz;}static char _tmp1BA[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1BA,
_tmp1BA + 39}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1BB=yy1;struct Cyc_List_List*
_tmp1BC;_LLF9: if((_tmp1BB.YY6).tag != 10)goto _LLFB;_tmp1BC=(_tmp1BB.YY6).f1;_LLFA:
yyzzz=_tmp1BC;goto _LLF8;_LLFB:;_LLFC:(int)_throw((void*)& Cyc_yyfail_YY6);_LLF8:;}
return yyzzz;}static char _tmp1BE[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1BE,_tmp1BE + 9}};void*Cyc_yyget_YY7(union
Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1BF=yy1;void*_tmp1C0;
_LLFE: if((_tmp1BF.YY7).tag != 11)goto _LL100;_tmp1C0=(_tmp1BF.YY7).f1;_LLFF: yyzzz=
_tmp1C0;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY7);_LLFD:;}
return yyzzz;}static char _tmp1C2[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1C2,_tmp1C2 + 16}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C3=yy1;struct Cyc_Core_Opt*_tmp1C4;_LL103: if((_tmp1C3.YY8).tag != 12)goto
_LL105;_tmp1C4=(_tmp1C3.YY8).f1;_LL104: yyzzz=_tmp1C4;goto _LL102;_LL105:;_LL106:(
int)_throw((void*)& Cyc_yyfail_YY8);_LL102:;}return yyzzz;}static char _tmp1C6[7]="qvar_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{
_tmp1C6,_tmp1C6 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union
yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE_union _tmp1C7=yy1;struct _tuple1*
_tmp1C8;_LL108: if((_tmp1C7.QualId_tok).tag != 4)goto _LL10A;_tmp1C8=(_tmp1C7.QualId_tok).f1;
_LL109: yyzzz=_tmp1C8;goto _LL107;_LL10A:;_LL10B:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL107:;}return yyzzz;}static char _tmp1CA[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1CA,_tmp1CA + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1CB=yy1;struct Cyc_Absyn_Stmt*_tmp1CC;_LL10D: if((_tmp1CB.YY9).tag != 13)goto
_LL10F;_tmp1CC=(_tmp1CB.YY9).f1;_LL10E: yyzzz=_tmp1CC;goto _LL10C;_LL10F:;_LL110:(
int)_throw((void*)& Cyc_yyfail_YY9);_LL10C:;}return yyzzz;}static char _tmp1CE[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1CE,
_tmp1CE + 24}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1CF=yy1;struct Cyc_List_List*
_tmp1D0;_LL112: if((_tmp1CF.YY10).tag != 14)goto _LL114;_tmp1D0=(_tmp1CF.YY10).f1;
_LL113: yyzzz=_tmp1D0;goto _LL111;_LL114:;_LL115:(int)_throw((void*)& Cyc_yyfail_YY10);
_LL111:;}return yyzzz;}static char _tmp1D2[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1D2,_tmp1D2 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE_union
_tmp1D3=yy1;struct Cyc_Absyn_Pat*_tmp1D4;_LL117: if((_tmp1D3.YY11).tag != 15)goto
_LL119;_tmp1D4=(_tmp1D3.YY11).f1;_LL118: yyzzz=_tmp1D4;goto _LL116;_LL119:;_LL11A:(
int)_throw((void*)& Cyc_yyfail_YY11);_LL116:;}return yyzzz;}static char _tmp1D6[23]="$(list_t<pat_t>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1D6,
_tmp1D6 + 23}};struct _tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1D7=yy1;struct _tuple13*_tmp1D8;_LL11C:
if((_tmp1D7.YY12).tag != 16)goto _LL11E;_tmp1D8=(_tmp1D7.YY12).f1;_LL11D: yyzzz=
_tmp1D8;goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY12);_LL11B:;}
return yyzzz;}static char _tmp1DA[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1DA,_tmp1DA + 14}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1DB=yy1;struct Cyc_List_List*_tmp1DC;_LL121: if((_tmp1DB.YY13).tag != 17)goto
_LL123;_tmp1DC=(_tmp1DB.YY13).f1;_LL122: yyzzz=_tmp1DC;goto _LL120;_LL123:;_LL124:(
int)_throw((void*)& Cyc_yyfail_YY13);_LL120:;}return yyzzz;}static char _tmp1DE[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1DE,
_tmp1DE + 31}};struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){struct
_tuple14*yyzzz;{union Cyc_YYSTYPE_union _tmp1DF=yy1;struct _tuple14*_tmp1E0;_LL126:
if((_tmp1DF.YY14).tag != 18)goto _LL128;_tmp1E0=(_tmp1DF.YY14).f1;_LL127: yyzzz=
_tmp1E0;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_YY14);_LL125:;}
return yyzzz;}static char _tmp1E2[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1E2,
_tmp1E2 + 39}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1E3=yy1;struct Cyc_List_List*
_tmp1E4;_LL12B: if((_tmp1E3.YY15).tag != 19)goto _LL12D;_tmp1E4=(_tmp1E3.YY15).f1;
_LL12C: yyzzz=_tmp1E4;goto _LL12A;_LL12D:;_LL12E:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL12A:;}return yyzzz;}static char _tmp1E6[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1E6,
_tmp1E6 + 48}};struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1E7=yy1;struct _tuple13*_tmp1E8;_LL130:
if((_tmp1E7.YY16).tag != 20)goto _LL132;_tmp1E8=(_tmp1E7.YY16).f1;_LL131: yyzzz=
_tmp1E8;goto _LL12F;_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY16);_LL12F:;}
return yyzzz;}static char _tmp1EA[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1EA,_tmp1EA + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union
_tmp1EB=yy1;struct Cyc_Absyn_Fndecl*_tmp1EC;_LL135: if((_tmp1EB.YY17).tag != 21)
goto _LL137;_tmp1EC=(_tmp1EB.YY17).f1;_LL136: yyzzz=_tmp1EC;goto _LL134;_LL137:;
_LL138:(int)_throw((void*)& Cyc_yyfail_YY17);_LL134:;}return yyzzz;}static char
_tmp1EE[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp1EE,_tmp1EE + 15}};struct Cyc_List_List*Cyc_yyget_YY18(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1EF=
yy1;struct Cyc_List_List*_tmp1F0;_LL13A: if((_tmp1EF.YY18).tag != 22)goto _LL13C;
_tmp1F0=(_tmp1EF.YY18).f1;_LL13B: yyzzz=_tmp1F0;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY18);_LL139:;}return yyzzz;}static char _tmp1F2[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1F2,
_tmp1F2 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union _tmp1F3=yy1;
struct Cyc_Parse_Declaration_spec*_tmp1F4;_LL13F: if((_tmp1F3.YY19).tag != 23)goto
_LL141;_tmp1F4=(_tmp1F3.YY19).f1;_LL140: yyzzz=_tmp1F4;goto _LL13E;_LL141:;_LL142:(
int)_throw((void*)& Cyc_yyfail_YY19);_LL13E:;}return yyzzz;}static char _tmp1F6[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1F6,
_tmp1F6 + 27}};struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1){struct
_tuple15*yyzzz;{union Cyc_YYSTYPE_union _tmp1F7=yy1;struct _tuple15*_tmp1F8;_LL144:
if((_tmp1F7.YY20).tag != 24)goto _LL146;_tmp1F8=(_tmp1F7.YY20).f1;_LL145: yyzzz=
_tmp1F8;goto _LL143;_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY20);_LL143:;}
return yyzzz;}static char _tmp1FA[35]="list_t<$(declarator_t,exp_opt_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1FA,_tmp1FA + 
35}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1FB=yy1;struct Cyc_List_List*_tmp1FC;_LL149: if((
_tmp1FB.YY21).tag != 25)goto _LL14B;_tmp1FC=(_tmp1FB.YY21).f1;_LL14A: yyzzz=_tmp1FC;
goto _LL148;_LL14B:;_LL14C:(int)_throw((void*)& Cyc_yyfail_YY21);_LL148:;}return
yyzzz;}static char _tmp1FE[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1FE,_tmp1FE + 16}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1FF=yy1;void*
_tmp200;_LL14E: if((_tmp1FF.YY22).tag != 26)goto _LL150;_tmp200=(_tmp1FF.YY22).f1;
_LL14F: yyzzz=_tmp200;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL14D:;}return yyzzz;}static char _tmp202[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp202,_tmp202 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp203=yy1;void*
_tmp204;_LL153: if((_tmp203.YY23).tag != 27)goto _LL155;_tmp204=(_tmp203.YY23).f1;
_LL154: yyzzz=_tmp204;goto _LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL152:;}return yyzzz;}static char _tmp206[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp206,_tmp206 + 12}};void*Cyc_yyget_YY24(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp207=yy1;void*
_tmp208;_LL158: if((_tmp207.YY24).tag != 28)goto _LL15A;_tmp208=(_tmp207.YY24).f1;
_LL159: yyzzz=_tmp208;goto _LL157;_LL15A:;_LL15B:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL157:;}return yyzzz;}static char _tmp20A[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp20A,_tmp20A + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE_union
_tmp20B=yy1;struct Cyc_Absyn_Tqual _tmp20C;_LL15D: if((_tmp20B.YY25).tag != 29)goto
_LL15F;_tmp20C=(_tmp20B.YY25).f1;_LL15E: yyzzz=_tmp20C;goto _LL15C;_LL15F:;_LL160:(
int)_throw((void*)& Cyc_yyfail_YY25);_LL15C:;}return yyzzz;}static char _tmp20E[20]="list_t<aggrfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp20E,
_tmp20E + 20}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp20F=yy1;struct Cyc_List_List*
_tmp210;_LL162: if((_tmp20F.YY26).tag != 30)goto _LL164;_tmp210=(_tmp20F.YY26).f1;
_LL163: yyzzz=_tmp210;goto _LL161;_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL161:;}return yyzzz;}static char _tmp212[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp212,_tmp212 + 
28}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp213=yy1;struct Cyc_List_List*_tmp214;_LL167: if((
_tmp213.YY27).tag != 31)goto _LL169;_tmp214=(_tmp213.YY27).f1;_LL168: yyzzz=_tmp214;
goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY27);_LL166:;}return
yyzzz;}static char _tmp216[24]="list_t<type_modifier_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp216,_tmp216 + 24}};struct Cyc_List_List*Cyc_yyget_YY28(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp217=yy1;struct Cyc_List_List*_tmp218;_LL16C: if((_tmp217.YY28).tag != 32)goto
_LL16E;_tmp218=(_tmp217.YY28).f1;_LL16D: yyzzz=_tmp218;goto _LL16B;_LL16E:;_LL16F:(
int)_throw((void*)& Cyc_yyfail_YY28);_LL16B:;}return yyzzz;}static char _tmp21A[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp21A,
_tmp21A + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE_union _tmp21B=yy1;struct
Cyc_Parse_Declarator*_tmp21C;_LL171: if((_tmp21B.YY29).tag != 33)goto _LL173;
_tmp21C=(_tmp21B.YY29).f1;_LL172: yyzzz=_tmp21C;goto _LL170;_LL173:;_LL174:(int)
_throw((void*)& Cyc_yyfail_YY29);_LL170:;}return yyzzz;}static char _tmp21E[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp21E,
_tmp21E + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE_union _tmp21F=yy1;
struct Cyc_Parse_Abstractdeclarator*_tmp220;_LL176: if((_tmp21F.YY30).tag != 34)
goto _LL178;_tmp220=(_tmp21F.YY30).f1;_LL177: yyzzz=_tmp220;goto _LL175;_LL178:;
_LL179:(int)_throw((void*)& Cyc_yyfail_YY30);_LL175:;}return yyzzz;}static char
_tmp222[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp222,_tmp222 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE_union yy1){int yyzzz;{
union Cyc_YYSTYPE_union _tmp223=yy1;int _tmp224;_LL17B: if((_tmp223.YY31).tag != 35)
goto _LL17D;_tmp224=(_tmp223.YY31).f1;_LL17C: yyzzz=_tmp224;goto _LL17A;_LL17D:;
_LL17E:(int)_throw((void*)& Cyc_yyfail_YY31);_LL17A:;}return yyzzz;}static char
_tmp226[8]="scope_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{
_tmp226,_tmp226 + 8}};void*Cyc_yyget_YY32(union Cyc_YYSTYPE_union yy1){void*yyzzz;{
union Cyc_YYSTYPE_union _tmp227=yy1;void*_tmp228;_LL180: if((_tmp227.YY32).tag != 36)
goto _LL182;_tmp228=(_tmp227.YY32).f1;_LL181: yyzzz=_tmp228;goto _LL17F;_LL182:;
_LL183:(int)_throw((void*)& Cyc_yyfail_YY32);_LL17F:;}return yyzzz;}static char
_tmp22A[14]="tunionfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp22A,_tmp22A + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY33(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{union Cyc_YYSTYPE_union
_tmp22B=yy1;struct Cyc_Absyn_Tunionfield*_tmp22C;_LL185: if((_tmp22B.YY33).tag != 
37)goto _LL187;_tmp22C=(_tmp22B.YY33).f1;_LL186: yyzzz=_tmp22C;goto _LL184;_LL187:;
_LL188:(int)_throw((void*)& Cyc_yyfail_YY33);_LL184:;}return yyzzz;}static char
_tmp22E[22]="list_t<tunionfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY34={
Cyc_Core_Failure,{_tmp22E,_tmp22E + 22}};struct Cyc_List_List*Cyc_yyget_YY34(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp22F=
yy1;struct Cyc_List_List*_tmp230;_LL18A: if((_tmp22F.YY34).tag != 38)goto _LL18C;
_tmp230=(_tmp22F.YY34).f1;_LL18B: yyzzz=_tmp230;goto _LL189;_LL18C:;_LL18D:(int)
_throw((void*)& Cyc_yyfail_YY34);_LL189:;}return yyzzz;}static char _tmp232[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp232,
_tmp232 + 50}};struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1){struct
_tuple16*yyzzz;{union Cyc_YYSTYPE_union _tmp233=yy1;struct _tuple16*_tmp234;_LL18F:
if((_tmp233.YY35).tag != 39)goto _LL191;_tmp234=(_tmp233.YY35).f1;_LL190: yyzzz=
_tmp234;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY35);_LL18E:;}
return yyzzz;}static char _tmp236[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp236,_tmp236 + 14}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp237=yy1;struct Cyc_List_List*_tmp238;_LL194: if((_tmp237.YY36).tag != 40)goto
_LL196;_tmp238=(_tmp237.YY36).f1;_LL195: yyzzz=_tmp238;goto _LL193;_LL196:;_LL197:(
int)_throw((void*)& Cyc_yyfail_YY36);_LL193:;}return yyzzz;}static char _tmp23A[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp23A,
_tmp23A + 32}};struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){struct
_tuple2*yyzzz;{union Cyc_YYSTYPE_union _tmp23B=yy1;struct _tuple2*_tmp23C;_LL199:
if((_tmp23B.YY37).tag != 41)goto _LL19B;_tmp23C=(_tmp23B.YY37).f1;_LL19A: yyzzz=
_tmp23C;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY37);_LL198:;}
return yyzzz;}static char _tmp23E[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp23E,
_tmp23E + 40}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp23F=yy1;struct Cyc_List_List*
_tmp240;_LL19E: if((_tmp23F.YY38).tag != 42)goto _LL1A0;_tmp240=(_tmp23F.YY38).f1;
_LL19F: yyzzz=_tmp240;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL19D:;}return yyzzz;}static char _tmp242[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp242,
_tmp242 + 107}};struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1){struct
_tuple17*yyzzz;{union Cyc_YYSTYPE_union _tmp243=yy1;struct _tuple17*_tmp244;_LL1A3:
if((_tmp243.YY39).tag != 43)goto _LL1A5;_tmp244=(_tmp243.YY39).f1;_LL1A4: yyzzz=
_tmp244;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1A2:;}
return yyzzz;}static char _tmp246[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp246,_tmp246 + 15}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp247=yy1;struct Cyc_List_List*_tmp248;_LL1A8: if((_tmp247.YY40).tag != 44)goto
_LL1AA;_tmp248=(_tmp247.YY40).f1;_LL1A9: yyzzz=_tmp248;goto _LL1A7;_LL1AA:;_LL1AB:(
int)_throw((void*)& Cyc_yyfail_YY40);_LL1A7:;}return yyzzz;}static char _tmp24A[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp24A,
_tmp24A + 21}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp24B=yy1;struct Cyc_List_List*
_tmp24C;_LL1AD: if((_tmp24B.YY41).tag != 45)goto _LL1AF;_tmp24C=(_tmp24B.YY41).f1;
_LL1AE: yyzzz=_tmp24C;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY41);
_LL1AC:;}return yyzzz;}static char _tmp24E[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp24E,_tmp24E + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp24F=yy1;void*
_tmp250;_LL1B2: if((_tmp24F.YY42).tag != 46)goto _LL1B4;_tmp250=(_tmp24F.YY42).f1;
_LL1B3: yyzzz=_tmp250;goto _LL1B1;_LL1B4:;_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1B1:;}return yyzzz;}static char _tmp252[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp252,_tmp252 + 7}};void*Cyc_yyget_YY43(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp253=yy1;void*
_tmp254;_LL1B7: if((_tmp253.YY43).tag != 47)goto _LL1B9;_tmp254=(_tmp253.YY43).f1;
_LL1B8: yyzzz=_tmp254;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1B6:;}return yyzzz;}static char _tmp256[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp256,_tmp256 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp257=yy1;void*
_tmp258;_LL1BC: if((_tmp257.YY44).tag != 48)goto _LL1BE;_tmp258=(_tmp257.YY44).f1;
_LL1BD: yyzzz=_tmp258;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1BB:;}return yyzzz;}static char _tmp25A[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp25A,_tmp25A + 20}};
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp25B=yy1;struct Cyc_List_List*_tmp25C;_LL1C1: if((
_tmp25B.YY45).tag != 49)goto _LL1C3;_tmp25C=(_tmp25B.YY45).f1;_LL1C2: yyzzz=_tmp25C;
goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C0:;}return
yyzzz;}static char _tmp25E[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp25E,_tmp25E + 12}};void*Cyc_yyget_YY46(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp25F=yy1;void*
_tmp260;_LL1C6: if((_tmp25F.YY46).tag != 50)goto _LL1C8;_tmp260=(_tmp25F.YY46).f1;
_LL1C7: yyzzz=_tmp260;goto _LL1C5;_LL1C8:;_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY46);
_LL1C5:;}return yyzzz;}static char _tmp262[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp262,_tmp262 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union
Cyc_YYSTYPE_union _tmp263=yy1;struct Cyc_Absyn_Enumfield*_tmp264;_LL1CB: if((
_tmp263.YY47).tag != 51)goto _LL1CD;_tmp264=(_tmp263.YY47).f1;_LL1CC: yyzzz=_tmp264;
goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1CA:;}return
yyzzz;}static char _tmp266[20]="list_t<enumfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp266,_tmp266 + 20}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp267=yy1;struct Cyc_List_List*_tmp268;_LL1D0: if((_tmp267.YY48).tag != 52)goto
_LL1D2;_tmp268=(_tmp267.YY48).f1;_LL1D1: yyzzz=_tmp268;goto _LL1CF;_LL1D2:;_LL1D3:(
int)_throw((void*)& Cyc_yyfail_YY48);_LL1CF:;}return yyzzz;}static char _tmp26A[14]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp26A,
_tmp26A + 14}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE_union yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp26B=yy1;struct Cyc_Core_Opt*
_tmp26C;_LL1D5: if((_tmp26B.YY49).tag != 53)goto _LL1D7;_tmp26C=(_tmp26B.YY49).f1;
_LL1D6: yyzzz=_tmp26C;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((void*)& Cyc_yyfail_YY49);
_LL1D4:;}return yyzzz;}static char _tmp26E[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp26E,_tmp26E + 
26}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp26F=yy1;struct Cyc_List_List*_tmp270;_LL1DA: if((
_tmp26F.YY50).tag != 54)goto _LL1DC;_tmp270=(_tmp26F.YY50).f1;_LL1DB: yyzzz=_tmp270;
goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY50);_LL1D9:;}return
yyzzz;}static char _tmp272[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp272,_tmp272 + 15}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp273=yy1;struct Cyc_Absyn_Conref*_tmp274;_LL1DF: if((_tmp273.YY51).tag != 55)
goto _LL1E1;_tmp274=(_tmp273.YY51).f1;_LL1E0: yyzzz=_tmp274;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1DE:;}return yyzzz;}static char
_tmp276[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp276,_tmp276 + 40}};struct Cyc_List_List*Cyc_yyget_YY52(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp277=yy1;struct Cyc_List_List*_tmp278;_LL1E4: if((_tmp277.YY52).tag != 56)goto
_LL1E6;_tmp278=(_tmp277.YY52).f1;_LL1E5: yyzzz=_tmp278;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY52);_LL1E3:;}return yyzzz;}struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype
Cyc_yynewloc(){return({struct Cyc_Yyltype _tmp279;_tmp279.timestamp=0;_tmp279.first_line=
0;_tmp279.first_column=0;_tmp279.last_line=0;_tmp279.last_column=0;_tmp279;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[361]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,2,114,130,128,2,
111,112,120,125,109,123,117,129,2,2,2,2,2,2,2,2,2,2,116,106,115,110,124,122,121,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,118,2,119,127,113,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,107,126,108,131,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105};static char
_tmp27A[2]="$";static char _tmp27B[6]="error";static char _tmp27C[12]="$undefined.";
static char _tmp27D[5]="AUTO";static char _tmp27E[9]="REGISTER";static char _tmp27F[7]="STATIC";
static char _tmp280[7]="EXTERN";static char _tmp281[8]="TYPEDEF";static char _tmp282[5]="VOID";
static char _tmp283[5]="CHAR";static char _tmp284[6]="SHORT";static char _tmp285[4]="INT";
static char _tmp286[5]="LONG";static char _tmp287[6]="FLOAT";static char _tmp288[7]="DOUBLE";
static char _tmp289[7]="SIGNED";static char _tmp28A[9]="UNSIGNED";static char _tmp28B[
6]="CONST";static char _tmp28C[9]="VOLATILE";static char _tmp28D[9]="RESTRICT";
static char _tmp28E[7]="STRUCT";static char _tmp28F[6]="UNION";static char _tmp290[5]="CASE";
static char _tmp291[8]="DEFAULT";static char _tmp292[7]="INLINE";static char _tmp293[7]="SIZEOF";
static char _tmp294[9]="OFFSETOF";static char _tmp295[3]="IF";static char _tmp296[5]="ELSE";
static char _tmp297[7]="SWITCH";static char _tmp298[6]="WHILE";static char _tmp299[3]="DO";
static char _tmp29A[4]="FOR";static char _tmp29B[5]="GOTO";static char _tmp29C[9]="CONTINUE";
static char _tmp29D[6]="BREAK";static char _tmp29E[7]="RETURN";static char _tmp29F[5]="ENUM";
static char _tmp2A0[8]="NULL_kw";static char _tmp2A1[4]="LET";static char _tmp2A2[6]="THROW";
static char _tmp2A3[4]="TRY";static char _tmp2A4[6]="CATCH";static char _tmp2A5[7]="EXPORT";
static char _tmp2A6[4]="NEW";static char _tmp2A7[9]="ABSTRACT";static char _tmp2A8[9]="FALLTHRU";
static char _tmp2A9[6]="USING";static char _tmp2AA[10]="NAMESPACE";static char _tmp2AB[
7]="TUNION";static char _tmp2AC[8]="XTUNION";static char _tmp2AD[7]="MALLOC";static
char _tmp2AE[8]="RMALLOC";static char _tmp2AF[7]="CALLOC";static char _tmp2B0[8]="RCALLOC";
static char _tmp2B1[5]="SWAP";static char _tmp2B2[9]="REGION_T";static char _tmp2B3[9]="SIZEOF_T";
static char _tmp2B4[6]="TAG_T";static char _tmp2B5[7]="REGION";static char _tmp2B6[5]="RNEW";
static char _tmp2B7[8]="REGIONS";static char _tmp2B8[13]="RESET_REGION";static char
_tmp2B9[4]="GEN";static char _tmp2BA[14]="NOZEROTERM_kw";static char _tmp2BB[12]="ZEROTERM_kw";
static char _tmp2BC[7]="PORTON";static char _tmp2BD[8]="PORTOFF";static char _tmp2BE[8]="FLAT_kw";
static char _tmp2BF[12]="DYNREGION_T";static char _tmp2C0[6]="ALIAS";static char
_tmp2C1[7]="PTR_OP";static char _tmp2C2[7]="INC_OP";static char _tmp2C3[7]="DEC_OP";
static char _tmp2C4[8]="LEFT_OP";static char _tmp2C5[9]="RIGHT_OP";static char _tmp2C6[
6]="LE_OP";static char _tmp2C7[6]="GE_OP";static char _tmp2C8[6]="EQ_OP";static char
_tmp2C9[6]="NE_OP";static char _tmp2CA[7]="AND_OP";static char _tmp2CB[6]="OR_OP";
static char _tmp2CC[11]="MUL_ASSIGN";static char _tmp2CD[11]="DIV_ASSIGN";static char
_tmp2CE[11]="MOD_ASSIGN";static char _tmp2CF[11]="ADD_ASSIGN";static char _tmp2D0[11]="SUB_ASSIGN";
static char _tmp2D1[12]="LEFT_ASSIGN";static char _tmp2D2[13]="RIGHT_ASSIGN";static
char _tmp2D3[11]="AND_ASSIGN";static char _tmp2D4[11]="XOR_ASSIGN";static char
_tmp2D5[10]="OR_ASSIGN";static char _tmp2D6[9]="ELLIPSIS";static char _tmp2D7[11]="LEFT_RIGHT";
static char _tmp2D8[12]="COLON_COLON";static char _tmp2D9[11]="IDENTIFIER";static
char _tmp2DA[17]="INTEGER_CONSTANT";static char _tmp2DB[7]="STRING";static char
_tmp2DC[19]="CHARACTER_CONSTANT";static char _tmp2DD[18]="FLOATING_CONSTANT";
static char _tmp2DE[9]="TYPE_VAR";static char _tmp2DF[13]="TYPEDEF_NAME";static char
_tmp2E0[16]="QUAL_IDENTIFIER";static char _tmp2E1[18]="QUAL_TYPEDEF_NAME";static
char _tmp2E2[13]="TYPE_INTEGER";static char _tmp2E3[10]="ATTRIBUTE";static char
_tmp2E4[4]="';'";static char _tmp2E5[4]="'{'";static char _tmp2E6[4]="'}'";static
char _tmp2E7[4]="','";static char _tmp2E8[4]="'='";static char _tmp2E9[4]="'('";
static char _tmp2EA[4]="')'";static char _tmp2EB[4]="'_'";static char _tmp2EC[4]="'$'";
static char _tmp2ED[4]="'<'";static char _tmp2EE[4]="':'";static char _tmp2EF[4]="'.'";
static char _tmp2F0[4]="'['";static char _tmp2F1[4]="']'";static char _tmp2F2[4]="'*'";
static char _tmp2F3[4]="'@'";static char _tmp2F4[4]="'?'";static char _tmp2F5[4]="'-'";
static char _tmp2F6[4]="'>'";static char _tmp2F7[4]="'+'";static char _tmp2F8[4]="'|'";
static char _tmp2F9[4]="'^'";static char _tmp2FA[4]="'&'";static char _tmp2FB[4]="'/'";
static char _tmp2FC[4]="'%'";static char _tmp2FD[4]="'~'";static char _tmp2FE[4]="'!'";
static char _tmp2FF[5]="prog";static char _tmp300[17]="translation_unit";static char
_tmp301[12]="export_list";static char _tmp302[19]="export_list_values";static char
_tmp303[21]="external_declaration";static char _tmp304[20]="function_definition";
static char _tmp305[21]="function_definition2";static char _tmp306[13]="using_action";
static char _tmp307[15]="unusing_action";static char _tmp308[17]="namespace_action";
static char _tmp309[19]="unnamespace_action";static char _tmp30A[12]="declaration";
static char _tmp30B[17]="declaration_list";static char _tmp30C[23]="declaration_specifiers";
static char _tmp30D[24]="storage_class_specifier";static char _tmp30E[15]="attributes_opt";
static char _tmp30F[11]="attributes";static char _tmp310[15]="attribute_list";static
char _tmp311[10]="attribute";static char _tmp312[15]="type_specifier";static char
_tmp313[25]="type_specifier_notypedef";static char _tmp314[5]="kind";static char
_tmp315[15]="type_qualifier";static char _tmp316[15]="enum_specifier";static char
_tmp317[11]="enum_field";static char _tmp318[22]="enum_declaration_list";static
char _tmp319[26]="struct_or_union_specifier";static char _tmp31A[16]="type_params_opt";
static char _tmp31B[16]="struct_or_union";static char _tmp31C[24]="struct_declaration_list";
static char _tmp31D[25]="struct_declaration_list0";static char _tmp31E[21]="init_declarator_list";
static char _tmp31F[22]="init_declarator_list0";static char _tmp320[16]="init_declarator";
static char _tmp321[19]="struct_declaration";static char _tmp322[25]="specifier_qualifier_list";
static char _tmp323[35]="notypedef_specifier_qualifier_list";static char _tmp324[23]="struct_declarator_list";
static char _tmp325[24]="struct_declarator_list0";static char _tmp326[18]="struct_declarator";
static char _tmp327[17]="tunion_specifier";static char _tmp328[9]="flat_opt";static
char _tmp329[18]="tunion_or_xtunion";static char _tmp32A[17]="tunionfield_list";
static char _tmp32B[18]="tunionfield_scope";static char _tmp32C[12]="tunionfield";
static char _tmp32D[11]="declarator";static char _tmp32E[23]="declarator_withtypedef";
static char _tmp32F[18]="direct_declarator";static char _tmp330[30]="direct_declarator_withtypedef";
static char _tmp331[8]="pointer";static char _tmp332[12]="one_pointer";static char
_tmp333[23]="pointer_null_and_bound";static char _tmp334[14]="pointer_bound";
static char _tmp335[18]="zeroterm_qual_opt";static char _tmp336[12]="opt_rgn_opt";
static char _tmp337[8]="rgn_opt";static char _tmp338[11]="tqual_list";static char
_tmp339[20]="parameter_type_list";static char _tmp33A[9]="type_var";static char
_tmp33B[16]="optional_effect";static char _tmp33C[19]="optional_rgn_order";static
char _tmp33D[10]="rgn_order";static char _tmp33E[16]="optional_inject";static char
_tmp33F[11]="effect_set";static char _tmp340[14]="atomic_effect";static char _tmp341[
11]="region_set";static char _tmp342[15]="parameter_list";static char _tmp343[22]="parameter_declaration";
static char _tmp344[16]="identifier_list";static char _tmp345[17]="identifier_list0";
static char _tmp346[12]="initializer";static char _tmp347[18]="array_initializer";
static char _tmp348[17]="initializer_list";static char _tmp349[12]="designation";
static char _tmp34A[16]="designator_list";static char _tmp34B[11]="designator";
static char _tmp34C[10]="type_name";static char _tmp34D[14]="any_type_name";static
char _tmp34E[15]="type_name_list";static char _tmp34F[20]="abstract_declarator";
static char _tmp350[27]="direct_abstract_declarator";static char _tmp351[10]="statement";
static char _tmp352[13]="open_exp_opt";static char _tmp353[18]="labeled_statement";
static char _tmp354[21]="expression_statement";static char _tmp355[19]="compound_statement";
static char _tmp356[16]="block_item_list";static char _tmp357[20]="selection_statement";
static char _tmp358[15]="switch_clauses";static char _tmp359[20]="iteration_statement";
static char _tmp35A[15]="jump_statement";static char _tmp35B[12]="exp_pattern";
static char _tmp35C[20]="conditional_pattern";static char _tmp35D[19]="logical_or_pattern";
static char _tmp35E[20]="logical_and_pattern";static char _tmp35F[21]="inclusive_or_pattern";
static char _tmp360[21]="exclusive_or_pattern";static char _tmp361[12]="and_pattern";
static char _tmp362[17]="equality_pattern";static char _tmp363[19]="relational_pattern";
static char _tmp364[14]="shift_pattern";static char _tmp365[17]="additive_pattern";
static char _tmp366[23]="multiplicative_pattern";static char _tmp367[13]="cast_pattern";
static char _tmp368[14]="unary_pattern";static char _tmp369[16]="postfix_pattern";
static char _tmp36A[16]="primary_pattern";static char _tmp36B[8]="pattern";static
char _tmp36C[19]="tuple_pattern_list";static char _tmp36D[20]="tuple_pattern_list0";
static char _tmp36E[14]="field_pattern";static char _tmp36F[19]="field_pattern_list";
static char _tmp370[20]="field_pattern_list0";static char _tmp371[11]="expression";
static char _tmp372[22]="assignment_expression";static char _tmp373[20]="assignment_operator";
static char _tmp374[23]="conditional_expression";static char _tmp375[20]="constant_expression";
static char _tmp376[22]="logical_or_expression";static char _tmp377[23]="logical_and_expression";
static char _tmp378[24]="inclusive_or_expression";static char _tmp379[24]="exclusive_or_expression";
static char _tmp37A[15]="and_expression";static char _tmp37B[20]="equality_expression";
static char _tmp37C[22]="relational_expression";static char _tmp37D[17]="shift_expression";
static char _tmp37E[20]="additive_expression";static char _tmp37F[26]="multiplicative_expression";
static char _tmp380[16]="cast_expression";static char _tmp381[17]="unary_expression";
static char _tmp382[15]="unary_operator";static char _tmp383[19]="postfix_expression";
static char _tmp384[19]="primary_expression";static char _tmp385[25]="argument_expression_list";
static char _tmp386[26]="argument_expression_list0";static char _tmp387[9]="constant";
static char _tmp388[20]="qual_opt_identifier";static char _tmp389[17]="qual_opt_typedef";
static char _tmp38A[18]="struct_union_name";static char _tmp38B[11]="field_name";
static char _tmp38C[12]="right_angle";static struct _dynforward_ptr Cyc_yytname[275]={{
_tmp27A,_tmp27A + 2},{_tmp27B,_tmp27B + 6},{_tmp27C,_tmp27C + 12},{_tmp27D,_tmp27D + 
5},{_tmp27E,_tmp27E + 9},{_tmp27F,_tmp27F + 7},{_tmp280,_tmp280 + 7},{_tmp281,
_tmp281 + 8},{_tmp282,_tmp282 + 5},{_tmp283,_tmp283 + 5},{_tmp284,_tmp284 + 6},{
_tmp285,_tmp285 + 4},{_tmp286,_tmp286 + 5},{_tmp287,_tmp287 + 6},{_tmp288,_tmp288 + 
7},{_tmp289,_tmp289 + 7},{_tmp28A,_tmp28A + 9},{_tmp28B,_tmp28B + 6},{_tmp28C,
_tmp28C + 9},{_tmp28D,_tmp28D + 9},{_tmp28E,_tmp28E + 7},{_tmp28F,_tmp28F + 6},{
_tmp290,_tmp290 + 5},{_tmp291,_tmp291 + 8},{_tmp292,_tmp292 + 7},{_tmp293,_tmp293 + 
7},{_tmp294,_tmp294 + 9},{_tmp295,_tmp295 + 3},{_tmp296,_tmp296 + 5},{_tmp297,
_tmp297 + 7},{_tmp298,_tmp298 + 6},{_tmp299,_tmp299 + 3},{_tmp29A,_tmp29A + 4},{
_tmp29B,_tmp29B + 5},{_tmp29C,_tmp29C + 9},{_tmp29D,_tmp29D + 6},{_tmp29E,_tmp29E + 
7},{_tmp29F,_tmp29F + 5},{_tmp2A0,_tmp2A0 + 8},{_tmp2A1,_tmp2A1 + 4},{_tmp2A2,
_tmp2A2 + 6},{_tmp2A3,_tmp2A3 + 4},{_tmp2A4,_tmp2A4 + 6},{_tmp2A5,_tmp2A5 + 7},{
_tmp2A6,_tmp2A6 + 4},{_tmp2A7,_tmp2A7 + 9},{_tmp2A8,_tmp2A8 + 9},{_tmp2A9,_tmp2A9 + 
6},{_tmp2AA,_tmp2AA + 10},{_tmp2AB,_tmp2AB + 7},{_tmp2AC,_tmp2AC + 8},{_tmp2AD,
_tmp2AD + 7},{_tmp2AE,_tmp2AE + 8},{_tmp2AF,_tmp2AF + 7},{_tmp2B0,_tmp2B0 + 8},{
_tmp2B1,_tmp2B1 + 5},{_tmp2B2,_tmp2B2 + 9},{_tmp2B3,_tmp2B3 + 9},{_tmp2B4,_tmp2B4 + 
6},{_tmp2B5,_tmp2B5 + 7},{_tmp2B6,_tmp2B6 + 5},{_tmp2B7,_tmp2B7 + 8},{_tmp2B8,
_tmp2B8 + 13},{_tmp2B9,_tmp2B9 + 4},{_tmp2BA,_tmp2BA + 14},{_tmp2BB,_tmp2BB + 12},{
_tmp2BC,_tmp2BC + 7},{_tmp2BD,_tmp2BD + 8},{_tmp2BE,_tmp2BE + 8},{_tmp2BF,_tmp2BF + 
12},{_tmp2C0,_tmp2C0 + 6},{_tmp2C1,_tmp2C1 + 7},{_tmp2C2,_tmp2C2 + 7},{_tmp2C3,
_tmp2C3 + 7},{_tmp2C4,_tmp2C4 + 8},{_tmp2C5,_tmp2C5 + 9},{_tmp2C6,_tmp2C6 + 6},{
_tmp2C7,_tmp2C7 + 6},{_tmp2C8,_tmp2C8 + 6},{_tmp2C9,_tmp2C9 + 6},{_tmp2CA,_tmp2CA + 
7},{_tmp2CB,_tmp2CB + 6},{_tmp2CC,_tmp2CC + 11},{_tmp2CD,_tmp2CD + 11},{_tmp2CE,
_tmp2CE + 11},{_tmp2CF,_tmp2CF + 11},{_tmp2D0,_tmp2D0 + 11},{_tmp2D1,_tmp2D1 + 12},{
_tmp2D2,_tmp2D2 + 13},{_tmp2D3,_tmp2D3 + 11},{_tmp2D4,_tmp2D4 + 11},{_tmp2D5,
_tmp2D5 + 10},{_tmp2D6,_tmp2D6 + 9},{_tmp2D7,_tmp2D7 + 11},{_tmp2D8,_tmp2D8 + 12},{
_tmp2D9,_tmp2D9 + 11},{_tmp2DA,_tmp2DA + 17},{_tmp2DB,_tmp2DB + 7},{_tmp2DC,_tmp2DC
+ 19},{_tmp2DD,_tmp2DD + 18},{_tmp2DE,_tmp2DE + 9},{_tmp2DF,_tmp2DF + 13},{_tmp2E0,
_tmp2E0 + 16},{_tmp2E1,_tmp2E1 + 18},{_tmp2E2,_tmp2E2 + 13},{_tmp2E3,_tmp2E3 + 10},{
_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5 + 4},{_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7 + 
4},{_tmp2E8,_tmp2E8 + 4},{_tmp2E9,_tmp2E9 + 4},{_tmp2EA,_tmp2EA + 4},{_tmp2EB,
_tmp2EB + 4},{_tmp2EC,_tmp2EC + 4},{_tmp2ED,_tmp2ED + 4},{_tmp2EE,_tmp2EE + 4},{
_tmp2EF,_tmp2EF + 4},{_tmp2F0,_tmp2F0 + 4},{_tmp2F1,_tmp2F1 + 4},{_tmp2F2,_tmp2F2 + 
4},{_tmp2F3,_tmp2F3 + 4},{_tmp2F4,_tmp2F4 + 4},{_tmp2F5,_tmp2F5 + 4},{_tmp2F6,
_tmp2F6 + 4},{_tmp2F7,_tmp2F7 + 4},{_tmp2F8,_tmp2F8 + 4},{_tmp2F9,_tmp2F9 + 4},{
_tmp2FA,_tmp2FA + 4},{_tmp2FB,_tmp2FB + 4},{_tmp2FC,_tmp2FC + 4},{_tmp2FD,_tmp2FD + 
4},{_tmp2FE,_tmp2FE + 4},{_tmp2FF,_tmp2FF + 5},{_tmp300,_tmp300 + 17},{_tmp301,
_tmp301 + 12},{_tmp302,_tmp302 + 19},{_tmp303,_tmp303 + 21},{_tmp304,_tmp304 + 20},{
_tmp305,_tmp305 + 21},{_tmp306,_tmp306 + 13},{_tmp307,_tmp307 + 15},{_tmp308,
_tmp308 + 17},{_tmp309,_tmp309 + 19},{_tmp30A,_tmp30A + 12},{_tmp30B,_tmp30B + 17},{
_tmp30C,_tmp30C + 23},{_tmp30D,_tmp30D + 24},{_tmp30E,_tmp30E + 15},{_tmp30F,
_tmp30F + 11},{_tmp310,_tmp310 + 15},{_tmp311,_tmp311 + 10},{_tmp312,_tmp312 + 15},{
_tmp313,_tmp313 + 25},{_tmp314,_tmp314 + 5},{_tmp315,_tmp315 + 15},{_tmp316,_tmp316
+ 15},{_tmp317,_tmp317 + 11},{_tmp318,_tmp318 + 22},{_tmp319,_tmp319 + 26},{_tmp31A,
_tmp31A + 16},{_tmp31B,_tmp31B + 16},{_tmp31C,_tmp31C + 24},{_tmp31D,_tmp31D + 25},{
_tmp31E,_tmp31E + 21},{_tmp31F,_tmp31F + 22},{_tmp320,_tmp320 + 16},{_tmp321,
_tmp321 + 19},{_tmp322,_tmp322 + 25},{_tmp323,_tmp323 + 35},{_tmp324,_tmp324 + 23},{
_tmp325,_tmp325 + 24},{_tmp326,_tmp326 + 18},{_tmp327,_tmp327 + 17},{_tmp328,
_tmp328 + 9},{_tmp329,_tmp329 + 18},{_tmp32A,_tmp32A + 17},{_tmp32B,_tmp32B + 18},{
_tmp32C,_tmp32C + 12},{_tmp32D,_tmp32D + 11},{_tmp32E,_tmp32E + 23},{_tmp32F,
_tmp32F + 18},{_tmp330,_tmp330 + 30},{_tmp331,_tmp331 + 8},{_tmp332,_tmp332 + 12},{
_tmp333,_tmp333 + 23},{_tmp334,_tmp334 + 14},{_tmp335,_tmp335 + 18},{_tmp336,
_tmp336 + 12},{_tmp337,_tmp337 + 8},{_tmp338,_tmp338 + 11},{_tmp339,_tmp339 + 20},{
_tmp33A,_tmp33A + 9},{_tmp33B,_tmp33B + 16},{_tmp33C,_tmp33C + 19},{_tmp33D,_tmp33D
+ 10},{_tmp33E,_tmp33E + 16},{_tmp33F,_tmp33F + 11},{_tmp340,_tmp340 + 14},{_tmp341,
_tmp341 + 11},{_tmp342,_tmp342 + 15},{_tmp343,_tmp343 + 22},{_tmp344,_tmp344 + 16},{
_tmp345,_tmp345 + 17},{_tmp346,_tmp346 + 12},{_tmp347,_tmp347 + 18},{_tmp348,
_tmp348 + 17},{_tmp349,_tmp349 + 12},{_tmp34A,_tmp34A + 16},{_tmp34B,_tmp34B + 11},{
_tmp34C,_tmp34C + 10},{_tmp34D,_tmp34D + 14},{_tmp34E,_tmp34E + 15},{_tmp34F,
_tmp34F + 20},{_tmp350,_tmp350 + 27},{_tmp351,_tmp351 + 10},{_tmp352,_tmp352 + 13},{
_tmp353,_tmp353 + 18},{_tmp354,_tmp354 + 21},{_tmp355,_tmp355 + 19},{_tmp356,
_tmp356 + 16},{_tmp357,_tmp357 + 20},{_tmp358,_tmp358 + 15},{_tmp359,_tmp359 + 20},{
_tmp35A,_tmp35A + 15},{_tmp35B,_tmp35B + 12},{_tmp35C,_tmp35C + 20},{_tmp35D,
_tmp35D + 19},{_tmp35E,_tmp35E + 20},{_tmp35F,_tmp35F + 21},{_tmp360,_tmp360 + 21},{
_tmp361,_tmp361 + 12},{_tmp362,_tmp362 + 17},{_tmp363,_tmp363 + 19},{_tmp364,
_tmp364 + 14},{_tmp365,_tmp365 + 17},{_tmp366,_tmp366 + 23},{_tmp367,_tmp367 + 13},{
_tmp368,_tmp368 + 14},{_tmp369,_tmp369 + 16},{_tmp36A,_tmp36A + 16},{_tmp36B,
_tmp36B + 8},{_tmp36C,_tmp36C + 19},{_tmp36D,_tmp36D + 20},{_tmp36E,_tmp36E + 14},{
_tmp36F,_tmp36F + 19},{_tmp370,_tmp370 + 20},{_tmp371,_tmp371 + 11},{_tmp372,
_tmp372 + 22},{_tmp373,_tmp373 + 20},{_tmp374,_tmp374 + 23},{_tmp375,_tmp375 + 20},{
_tmp376,_tmp376 + 22},{_tmp377,_tmp377 + 23},{_tmp378,_tmp378 + 24},{_tmp379,
_tmp379 + 24},{_tmp37A,_tmp37A + 15},{_tmp37B,_tmp37B + 20},{_tmp37C,_tmp37C + 22},{
_tmp37D,_tmp37D + 17},{_tmp37E,_tmp37E + 20},{_tmp37F,_tmp37F + 26},{_tmp380,
_tmp380 + 16},{_tmp381,_tmp381 + 17},{_tmp382,_tmp382 + 15},{_tmp383,_tmp383 + 19},{
_tmp384,_tmp384 + 19},{_tmp385,_tmp385 + 25},{_tmp386,_tmp386 + 26},{_tmp387,
_tmp387 + 9},{_tmp388,_tmp388 + 20},{_tmp389,_tmp389 + 17},{_tmp38A,_tmp38A + 18},{
_tmp38B,_tmp38B + 11},{_tmp38C,_tmp38C + 12}};static short Cyc_yyr1[501]={0,133,134,
134,134,134,134,134,134,134,134,134,135,135,136,136,137,137,137,138,138,138,138,
139,139,140,141,142,143,144,144,144,144,145,145,146,146,146,146,146,146,146,146,
146,146,147,147,147,147,147,147,147,148,148,149,150,150,151,151,151,151,151,151,
152,152,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,154,155,155,155,156,156,156,157,157,158,158,158,159,159,159,
160,160,161,161,162,162,163,163,164,165,165,166,166,167,168,168,168,168,168,168,
169,169,169,169,169,169,170,171,171,172,172,172,173,173,173,174,174,175,175,176,
176,176,176,177,177,177,178,178,179,179,180,180,181,181,181,181,181,181,181,181,
181,181,182,182,182,182,182,182,182,182,182,182,182,183,183,184,185,185,185,185,
186,186,186,187,187,187,188,188,188,189,189,189,190,190,191,191,191,191,192,192,
193,193,194,194,195,195,196,196,197,197,198,198,198,198,199,199,200,200,201,201,
201,202,203,203,204,204,205,205,205,205,206,206,206,206,207,208,208,209,209,210,
210,211,211,211,211,211,212,212,213,213,213,214,214,214,214,214,214,214,214,214,
214,214,215,215,215,215,215,215,215,215,215,215,215,215,216,216,217,218,218,219,
219,220,220,220,220,220,220,221,221,221,221,221,221,222,222,222,222,222,222,223,
223,223,223,223,223,223,223,223,223,223,223,223,223,224,224,224,224,224,224,224,
224,225,226,226,227,227,228,228,229,229,230,230,231,231,232,232,232,233,233,233,
233,233,234,234,234,235,235,235,236,236,236,236,237,237,238,238,238,238,238,238,
238,239,240,241,241,241,241,241,241,241,241,241,241,241,241,241,241,242,242,242,
243,243,244,244,245,245,245,246,246,247,247,248,248,249,249,249,249,249,249,249,
249,249,249,249,250,250,250,250,250,250,250,251,252,252,253,253,254,254,255,255,
256,256,257,257,257,258,258,258,258,258,259,259,259,260,260,260,261,261,261,261,
262,262,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,264,
264,264,265,265,265,265,265,265,265,265,265,265,266,266,266,266,266,266,266,266,
266,267,268,268,269,269,269,269,270,270,271,271,272,272,273,273,274,274};static
short Cyc_yyr2[501]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,
3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,
1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,
2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,
5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,2,0,3,3,0,1,1,0,
1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,
3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,
1,6,4,9,6,5,8,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,
8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,
3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,
1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,
3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,6,1,1,1,1,4,3,4,3,
3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[
1006]={0,18,45,46,47,48,50,65,68,69,70,71,72,73,74,75,89,90,91,105,106,41,0,0,51,
0,0,0,0,86,0,0,138,0,491,201,493,492,494,87,0,0,66,0,183,183,181,1,0,16,0,0,17,0,
35,43,37,63,39,76,77,0,78,0,139,151,0,176,186,79,155,103,49,48,42,0,93,490,0,491,
487,488,489,0,366,0,0,0,0,224,0,368,369,25,27,139,139,139,0,0,139,0,0,0,0,0,139,
139,179,180,182,2,0,0,0,0,29,0,111,112,114,36,44,38,40,139,495,496,103,140,141,
189,139,33,139,0,19,139,139,0,164,152,177,188,187,192,139,64,0,49,97,0,95,0,491,
375,0,0,0,0,0,0,0,0,0,0,0,0,103,0,0,477,139,0,0,464,0,0,462,463,0,392,394,407,
415,417,419,421,423,425,428,433,436,439,443,0,445,465,476,475,0,376,374,32,0,0,
103,0,0,0,121,117,119,242,244,0,0,0,9,10,0,497,498,202,88,0,0,156,67,222,0,219,0,
0,0,3,0,5,0,30,0,0,139,20,0,139,109,0,102,191,0,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
491,280,282,288,284,0,286,265,266,267,0,268,269,270,0,34,21,114,209,225,0,0,205,
203,0,249,0,186,0,194,52,193,0,0,98,94,0,0,370,0,0,139,453,139,409,443,0,410,411,
0,0,0,0,0,0,0,139,446,447,139,0,0,0,449,450,448,0,367,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,397,398,399,400,401,402,403,404,405,406,396,0,451,0,471,472,0,0,0,
479,0,103,382,383,0,380,0,226,0,0,0,139,245,217,0,122,127,123,125,118,120,139,0,
251,243,252,500,499,0,81,84,85,139,82,58,57,0,55,157,139,221,153,251,223,165,166,
139,80,139,185,184,26,0,28,0,113,115,228,227,22,100,110,0,0,0,129,130,132,0,103,
103,0,0,0,0,0,0,139,0,317,318,319,0,0,321,0,277,0,0,0,0,0,289,285,114,287,283,
281,210,139,0,0,216,204,211,160,0,0,139,205,162,139,163,158,186,195,53,104,0,99,
414,96,92,378,379,0,0,0,229,0,0,233,0,0,0,238,0,0,0,0,0,0,139,0,0,0,478,485,0,
484,393,416,0,418,420,422,424,426,427,431,432,429,430,434,435,438,437,440,441,
442,395,470,467,0,469,0,139,0,371,0,377,31,389,0,385,390,0,387,372,0,0,246,128,
124,126,0,205,0,186,0,253,139,139,0,264,248,0,0,0,0,121,0,103,139,139,0,175,154,
220,4,6,0,133,116,0,0,205,0,136,0,0,0,296,0,0,0,0,0,316,320,0,0,0,0,0,0,0,0,0,
279,139,23,203,139,213,0,0,206,0,161,209,197,250,159,195,178,0,7,0,452,0,0,241,0,
230,0,234,237,239,457,0,0,0,0,0,0,0,0,0,444,481,0,0,468,466,0,0,381,384,386,373,
0,247,218,260,0,254,255,186,0,205,0,186,0,83,0,0,0,54,56,0,167,0,205,0,0,186,0,
131,134,139,103,146,0,0,0,0,0,0,0,0,0,0,0,0,0,296,322,0,0,272,0,0,0,0,24,205,0,
214,212,0,205,139,196,0,8,0,0,0,240,231,235,0,0,0,0,0,412,413,456,483,0,486,408,
480,482,0,388,391,259,257,262,0,263,256,186,0,61,59,60,168,171,0,173,174,169,186,
0,137,148,147,0,0,142,290,296,0,0,0,139,0,0,324,325,327,329,331,333,335,337,340,
345,348,351,355,357,364,365,0,139,293,302,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,275,0,
199,215,207,198,203,13,0,0,455,454,0,236,458,139,0,461,473,0,261,258,0,172,170,
101,135,149,146,146,0,0,296,139,361,139,0,358,139,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,359,297,0,304,0,0,0,312,0,0,0,0,0,0,295,0,271,274,0,0,0,205,12,14,0,0,
0,0,474,0,139,145,144,291,292,0,0,0,0,296,298,328,0,330,332,334,336,338,339,343,
344,341,342,346,347,350,349,352,353,354,0,303,305,306,0,314,313,0,308,0,0,0,278,
0,0,208,200,15,0,0,139,0,0,294,360,0,356,299,0,139,307,315,309,310,0,0,276,232,
459,0,62,150,0,0,326,296,300,311,273,0,363,362,301,460,0,0,0};static short Cyc_yydefgoto[
142]={1003,47,641,838,48,49,269,50,425,51,427,52,134,53,54,488,210,409,410,211,
57,223,212,59,150,151,60,147,61,244,245,117,118,119,246,213,392,437,438,439,62,
63,131,784,785,786,64,440,65,414,66,67,68,108,145,250,296,638,568,69,569,480,630,
472,476,477,387,289,231,88,89,504,430,505,506,507,508,214,291,292,570,398,272,
617,273,274,275,276,277,711,278,279,794,795,796,797,798,799,800,801,802,803,804,
805,806,807,808,809,376,377,378,558,559,560,280,181,364,182,494,183,184,185,186,
187,188,189,190,191,192,193,194,195,196,197,521,522,198,199,71,839,224,402};
static short Cyc_yypact[1006]={2667,- -32768,- -32768,- -32768,- -32768,- 63,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,1227,105,563,- -32768,204,- 43,21,62,83,51,112,-
-32768,143,- -32768,- 27,- -32768,- -32768,- -32768,- -32768,- 8,951,215,181,222,222,
140,- -32768,2544,- -32768,322,377,- -32768,496,1227,1227,1227,- -32768,1227,- -32768,
- -32768,604,- -32768,384,3235,361,212,388,530,- -32768,- -32768,230,258,272,- -32768,
204,268,- -32768,727,130,- -32768,- -32768,- -32768,5568,- -32768,289,238,727,307,347,
314,- -32768,213,- -32768,- -32768,5992,5992,5992,2544,2544,5992,- 19,378,252,383,-
19,6159,2038,- -32768,- -32768,- -32768,- -32768,2544,2789,2544,2789,- -32768,394,416,
- -32768,3163,- -32768,- -32768,- -32768,- -32768,6022,- -32768,- -32768,230,- -32768,-
-32768,65,1653,- -32768,3235,496,- -32768,3297,5992,3936,- -32768,361,- -32768,-
-32768,- -32768,104,5992,- -32768,2789,- -32768,455,462,464,204,- 58,- -32768,727,198,
5705,472,5568,982,478,511,517,533,555,575,230,5759,5759,- -32768,2163,611,5813,-
-32768,5813,5813,- -32768,- -32768,381,- -32768,- -32768,- 15,646,605,529,609,561,- 14,
534,337,148,- -32768,1170,5813,149,50,- -32768,626,821,671,- -32768,- -32768,673,
5568,230,821,669,218,6007,6266,6007,327,- -32768,25,25,25,- -32768,- -32768,- 3,-
-32768,- -32768,- -32768,- -32768,42,680,- -32768,- -32768,509,420,- -32768,675,81,633,
- -32768,689,- -32768,691,- -32768,252,4032,3235,- -32768,713,6129,- -32768,607,709,-
-32768,204,- -32768,717,151,740,3451,745,750,752,754,4128,3451,124,77,753,5759,
751,- -32768,- -32768,1523,1523,496,1523,- -32768,- -32768,- -32768,767,- -32768,-
-32768,- -32768,447,- -32768,- -32768,776,782,- -32768,17,791,788,454,794,787,- 39,
530,790,- -32768,809,- -32768,- 39,810,204,- -32768,5568,813,- -32768,798,801,2163,-
-32768,6159,- -32768,- -32768,3552,- -32768,847,5568,5568,5568,5568,5568,5568,818,
2163,- -32768,- -32768,1783,819,463,5568,- -32768,- -32768,- -32768,5568,- -32768,5813,
5568,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,
5813,5813,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,5568,- -32768,- 19,- -32768,- -32768,4224,- 19,5568,- -32768,
815,230,- -32768,- -32768,824,829,727,- -32768,494,3518,828,5992,- -32768,833,825,-
-32768,6266,6266,6266,- -32768,- -32768,2928,4320,231,- -32768,370,- -32768,- -32768,
17,- -32768,- -32768,- -32768,5992,- -32768,- -32768,832,835,836,- -32768,2858,- -32768,
481,283,- -32768,- -32768,- -32768,6159,- -32768,2288,- -32768,- -32768,- -32768,2544,-
-32768,2544,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,771,5568,838,
839,- -32768,834,304,230,535,5568,5568,840,846,5568,924,1913,850,- -32768,- -32768,
- -32768,526,915,- -32768,4416,849,877,885,5568,871,3451,- -32768,- -32768,3163,-
-32768,- -32768,- -32768,- -32768,6159,872,251,- -32768,- -32768,857,- -32768,17,873,
6144,788,- -32768,5992,- -32768,- -32768,530,498,- -32768,- -32768,2421,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,875,879,889,- -32768,- 19,5568,- -32768,571,
4032,116,- -32768,536,880,886,887,890,537,6159,882,892,5622,- -32768,- -32768,891,
895,- -32768,646,171,605,529,609,561,- 14,- 14,534,534,534,534,337,337,148,148,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,893,- -32768,118,5992,3840,- -32768,
1020,- -32768,- -32768,- -32768,727,- -32768,- -32768,901,902,- -32768,185,898,- -32768,
- -32768,- -32768,- -32768,906,788,907,530,904,370,3359,5992,4512,- -32768,- -32768,
25,657,909,42,3043,912,230,3297,5992,4608,- -32768,481,- -32768,- -32768,- -32768,
1044,- -32768,- -32768,607,5568,788,204,894,545,551,5568,693,576,914,4704,4800,566,
- -32768,- -32768,920,922,917,935,3451,908,925,588,916,- -32768,3235,- -32768,933,
5992,- -32768,939,17,- -32768,926,- -32768,169,- -32768,787,- -32768,498,- -32768,934,
- -32768,2544,941,260,936,- -32768,938,- -32768,3648,- -32768,- -32768,- -32768,-
-32768,5568,1027,5568,5568,982,947,941,949,3840,- -32768,- -32768,5568,5568,-
-32768,- -32768,- 39,651,- -32768,- -32768,- -32768,- -32768,3614,- -32768,- -32768,-
-32768,952,- -32768,- -32768,530,953,788,- 39,530,944,- -32768,592,955,956,- -32768,-
-32768,958,- -32768,962,788,963,- 39,530,957,- -32768,- -32768,6022,230,803,3451,975,
971,1061,969,980,3451,5568,4896,615,4992,629,5088,693,- -32768,986,983,- -32768,
1002,- 11,992,1001,- -32768,788,196,- -32768,- -32768,1003,788,6159,- -32768,655,-
-32768,994,996,5568,- -32768,- -32768,- -32768,4032,630,993,1000,636,- -32768,847,-
-32768,- -32768,728,- -32768,- -32768,- -32768,- -32768,3840,- -32768,- -32768,- -32768,
- -32768,- -32768,999,- -32768,- -32768,530,1028,- -32768,- -32768,- -32768,- -32768,-
-32768,1011,- -32768,- -32768,- -32768,530,1022,- -32768,- -32768,- -32768,1035,204,
658,1100,693,1029,5867,1024,2288,5813,1033,- -32768,- 13,- -32768,1064,1025,726,784,
99,804,490,153,- -32768,- -32768,- -32768,- -32768,1070,5813,1783,- -32768,- -32768,
661,3451,663,5184,3451,667,5280,5376,681,1045,- -32768,5568,3451,3451,1052,-
-32768,1030,- -32768,- -32768,1049,- -32768,933,- -32768,1053,682,- -32768,- -32768,
261,- -32768,- -32768,6159,1142,- -32768,- -32768,3744,- -32768,- -32768,1059,- -32768,
- -32768,- -32768,- -32768,1060,55,803,3451,1065,693,2163,- -32768,6159,1066,- -32768,
1343,5813,5568,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,5813,
5813,5813,5813,5813,5568,- -32768,- -32768,1071,- -32768,3451,3451,694,- -32768,3451,
3451,706,3451,708,5472,- -32768,721,- -32768,- -32768,1055,1081,17,788,- -32768,-
-32768,236,5568,1073,1072,- -32768,1086,6159,- -32768,- -32768,- -32768,- -32768,1080,
1078,1082,5813,693,- -32768,646,279,605,529,529,561,- 14,- 14,534,534,534,534,337,
337,148,148,- -32768,- -32768,- -32768,323,- -32768,- -32768,- -32768,3451,- -32768,-
-32768,3451,- -32768,3451,3451,723,- -32768,1099,3451,- -32768,- -32768,- -32768,772,
1083,6159,1084,734,- -32768,941,315,- -32768,- -32768,5568,1343,- -32768,- -32768,-
-32768,- -32768,3451,3451,- -32768,- -32768,- -32768,1085,- -32768,- -32768,1087,1088,
- -32768,693,- -32768,- -32768,- -32768,1090,- -32768,- -32768,- -32768,- -32768,1198,
1205,- -32768};static short Cyc_yypgoto[142]={- -32768,142,- -32768,293,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 41,- 117,19,- -32768,- -32768,0,627,
- -32768,71,- 181,1106,32,- -32768,- -32768,- 126,- -32768,- 12,- -32768,512,- -32768,-
-32768,- -32768,972,974,107,459,- -32768,- -32768,617,- -32768,- -32768,- -32768,38,-
-32768,- -32768,197,- 33,1150,- 346,301,- -32768,- -32768,1172,- 17,- -32768,- -32768,
583,- 123,- 98,- 122,156,312,590,596,- 382,- 435,- 105,- 257,- 113,- -32768,- 212,- 151,-
538,- 362,- -32768,719,- 167,152,- 95,- 124,- 228,310,- -32768,- -32768,- -32768,- 26,- 244,
- -32768,- 275,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
95,1021,- -32768,553,- -32768,- -32768,299,674,- -32768,- 143,- 401,- 150,- 332,- 320,-
333,911,- 308,- 298,- 336,- 301,- 299,- 16,624,520,- -32768,- -32768,- 325,- -32768,115,
305,- 48,1204,- 348,226};static short Cyc_yytable[6381]={55,230,524,242,527,326,532,
533,534,535,313,314,669,127,287,288,526,310,544,578,556,55,547,133,290,465,466,
303,468,429,390,529,58,251,72,595,399,156,136,628,74,103,530,531,546,536,537,297,
55,538,539,298,94,58,55,55,55,157,55,407,782,783,342,343,55,140,334,101,869,590,
484,56,399,121,122,123,221,124,473,133,58,517,222,135,828,400,58,58,58,397,58,
270,56,281,243,590,58,631,55,55,399,344,646,102,829,416,405,335,282,870,345,386,
55,55,55,55,248,35,90,56,55,400,401,754,474,56,56,56,676,56,58,58,55,615,55,56,
95,408,91,135,498,140,499,372,58,58,58,58,55,400,401,271,58,135,645,516,321,98,
329,493,330,331,591,- 143,58,35,58,482,573,56,56,373,459,155,492,877,878,96,249,
365,58,418,202,56,56,56,56,573,475,422,111,56,460,91,382,461,413,702,97,418,34,
281,91,56,35,56,401,37,390,390,390,389,75,229,879,625,432,295,99,56,366,367,368,
880,591,156,650,332,270,270,457,270,247,502,503,458,- 225,667,104,- 225,218,219,55,
391,229,157,34,215,216,217,120,304,220,37,235,236,237,238,100,233,369,135,445,
110,471,446,370,371,350,55,55,91,55,885,58,286,486,351,352,708,332,- 203,886,887,
270,- 203,745,665,271,271,299,271,106,493,649,740,489,675,305,34,226,58,58,475,58,
70,37,34,833,105,401,306,556,56,37,91,230,388,35,393,206,401,91,41,207,55,385,76,
92,107,93,34,283,201,540,541,542,36,37,38,56,56,394,56,271,146,70,34,658,395,623,
35,247,70,37,221,739,58,70,627,493,222,550,41,585,148,126,418,142,149,332,70,44,
45,46,153,386,914,34,584,152,475,180,92,36,37,38,418,332,389,389,389,92,418,412,
977,56,745,577,34,200,395,584,70,70,36,37,38,70,609,221,991,583,203,589,435,222,
70,70,70,70,391,391,391,205,55,133,55,112,113,599,601,332,129,130,594,744,283,
394,978,70,624,403,404,824,395,406,44,45,46,55,682,683,70,668,493,204,58,152,58,
348,92,349,695,696,386,40,55,467,135,636,327,137,697,553,40,138,557,836,139,684,
574,58,114,115,575,135,745,576,225,332,55,698,333,91,227,56,91,56,58,239,229,662,
729,381,92,750,751,44,45,46,734,92,861,396,16,17,18,485,229,637,56,756,58,490,
240,229,631,229,419,415,475,420,843,417,585,562,930,56,933,934,938,939,940,941,
70,418,744,441,418,693,932,417,470,680,443,332,579,447,455,286,693,56,481,300,
449,935,592,890,593,301,456,332,577,302,519,70,936,937,229,942,943,281,309,944,
945,40,924,229,315,589,34,586,929,143,144,587,728,37,588,554,77,116,332,34,152,
327,41,346,347,36,37,38,883,509,884,44,45,46,514,412,327,316,55,928,866,992,395,
317,44,45,46,612,640,525,332,635,744,634,340,341,55,135,78,318,332,332,671,652,
657,146,672,600,976,332,58,338,706,79,80,332,81,82,707,763,37,319,91,767,637,548,
91,718,58,83,332,84,85,915,647,648,229,778,86,92,332,320,92,712,127,388,87,781,
229,56,396,925,332,926,34,726,769,34,995,770,36,37,38,36,37,38,125,56,415,910,
709,710,417,435,1001,417,818,328,436,332,678,336,44,45,46,70,337,70,374,994,821,
441,339,332,332,417,423,844,602,603,332,417,606,847,610,34,851,688,689,690,703,
36,37,38,758,759,620,854,837,858,77,379,859,380,557,332,270,332,891,622,893,332,
867,730,897,384,234,308,738,311,311,421,902,912,91,332,913,411,323,324,889,70,
424,311,426,311,311,988,332,810,687,953,78,782,783,247,475,55,294,971,332,442,
332,956,311,958,433,154,80,91,81,82,270,444,37,332,271,332,961,993,983,848,849,
83,765,84,85,229,419,58,451,990,86,565,566,567,448,775,873,874,87,450,92,452,77,
453,92,875,876,462,127,34,464,55,946,947,948,36,37,38,469,40,471,881,882,986,332,
435,56,441,832,241,271,417,463,835,44,45,46,757,441,920,921,441,417,58,78,417,
478,479,704,483,715,717,487,766,975,401,375,40,431,154,80,491,81,82,495,496,37,
777,497,311,723,334,515,549,518,83,564,84,85,551,270,552,56,561,86,563,580,596,
582,70,581,597,87,598,604,746,605,607,749,611,613,311,616,311,311,311,311,311,
311,311,311,311,311,311,311,311,311,311,311,311,618,55,92,619,621,629,626,644,
632,431,642,643,653,510,511,512,513,659,654,655,271,35,656,660,705,520,663,664,
666,523,158,159,673,58,674,815,817,92,820,787,823,677,679,77,691,814,681,694,713,
229,719,720,721,722,727,724,162,163,164,165,166,543,286,842,737,126,520,725,168,
34,731,661,56,733,741,747,37,169,170,40,742,77,752,311,753,41,768,762,764,965,
771,772,572,773,44,45,46,774,776,779,34,80,171,81,82,788,789,37,812,790,791,813,
312,857,180,825,172,826,520,173,827,830,77,78,831,174,834,845,175,840,176,841,
846,177,850,670,178,179,154,80,895,81,82,899,901,37,853,852,904,892,311,860,896,
855,83,520,84,85,865,862,905,906,34,86,78,311,856,871,36,37,38,87,868,888,872,
907,903,908,435,154,80,909,81,82,911,327,37,44,45,46,916,918,931,922,919,792,923,
84,85,963,950,927,962,431,86,970,969,175,968,793,949,972,87,973,974,178,179,984,
987,989,998,1004,999,1000,960,1002,951,952,1005,966,954,955,692,957,228,428,967,
701,780,141,109,126,434,736,964,311,735,431,732,651,761,383,811,2,3,4,73,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,528,686,21,353,354,355,356,357,358,359,360,361,
362,700,979,22,128,980,0,981,982,0,0,24,985,0,0,- 139,- 139,520,0,363,311,0,27,28,
29,0,0,0,311,0,0,0,996,997,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,431,0,0,0,0,35,36,748,38,39,40,0,0,431,0,0,755,0,42,43,0,0,0,0,2,3,4,73,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,709,710,21,158,159,252,0,253,254,255,256,257,
258,259,260,22,77,23,160,261,0,0,161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,
263,167,0,264,168,0,0,0,0,32,33,265,864,169,170,311,0,431,0,0,0,0,0,0,0,0,0,0,0,
0,0,431,0,311,0,0,266,80,171,81,82,35,36,37,38,39,40,267,132,- 296,0,0,172,0,42,
232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,311,0,311,311,311,311,311,311,311,311,311,311,311,311,311,311,311,311,311,
0,0,0,0,0,0,0,0,0,0,0,431,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,158,159,252,311,253,254,255,256,257,258,259,260,22,77,23,160,261,0,0,161,
24,262,0,0,- 139,- 139,162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,
33,265,0,169,170,0,0,0,0,311,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,266,80,171,81,82,35,
36,37,38,39,40,267,132,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,
0,178,179,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,252,0,
253,254,255,256,257,258,259,260,22,77,23,160,261,0,0,161,24,262,0,0,0,0,162,163,
164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,33,265,0,169,170,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,266,80,171,81,82,35,36,37,38,39,40,267,132,268,0,0,
172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,2,3,4,73,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,252,0,253,254,255,256,257,258,259,
260,22,77,23,160,261,0,0,161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,263,167,
0,264,168,0,0,0,0,32,33,265,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
266,80,171,81,82,35,36,37,38,39,40,267,132,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,
175,0,176,0,0,177,0,0,178,179,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,158,159,0,0,0,0,0,0,0,0,0,0,22,77,23,160,0,0,0,161,24,0,0,0,0,0,162,163,
164,165,166,27,28,29,0,167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,608,0,0,0,0,172,0,42,232,
0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,162,
163,164,165,166,27,28,29,0,167,208,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,209,0,0,0,172,0,
42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,
0,162,163,164,165,166,27,28,29,0,167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,325,0,0,0,
172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,166,27,28,29,0,167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,0,0,0,
0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,- 11,1,0,2,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,
0,0,639,0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,-
11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,
- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,0,0,
0,44,45,46,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,
0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,0,0,
0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,
0,35,36,37,38,39,40,0,0,0,0,0,41,0,42,43,0,0,0,0,0,44,45,46,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,
0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,
0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,22,
0,- 11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,
0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,284,0,0,34,0,0,0,0,35,36,37,
38,39,40,286,22,0,0,0,412,- 203,42,43,0,- 203,0,395,0,44,45,46,0,0,0,27,28,29,0,0,
0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,284,0,0,0,0,0,0,
0,35,36,0,38,39,40,286,0,0,0,0,394,- 203,42,43,0,- 203,0,395,0,44,45,46,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,
0,0,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,0,435,0,42,43,0,0,
0,0,0,44,45,46,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,
0,0,0,0,0,32,33,0,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,35,36,0,38,39,40,0,132,0,22,241,23,0,42,43,0,0,24,0,0,0,0,0,0,0,0,0,0,
27,28,29,0,0,0,0,0,0,0,0,0,32,33,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,22,35,36,0,38,39,40,0,132,0,0,0,0,0,42,43,0,0,0,27,28,29,0,
0,0,0,0,0,0,0,0,32,33,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,284,
0,0,285,0,0,0,22,35,36,0,38,39,40,286,0,0,0,0,0,- 203,42,43,0,- 203,0,27,28,29,0,0,
0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,284,0,0,0,0,0,0,
0,35,36,0,38,39,40,286,0,0,0,0,0,- 203,42,43,0,- 203,158,159,252,0,253,254,255,256,
257,258,259,260,0,77,0,160,261,0,0,161,0,262,0,0,0,0,162,163,164,165,166,0,0,0,
263,167,0,264,168,0,0,0,0,0,0,265,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,266,80,171,81,82,0,0,37,0,0,77,267,132,0,0,0,172,0,0,173,0,0,0,0,0,174,0,0,
175,0,176,158,159,177,0,0,178,179,500,0,0,0,0,0,77,0,160,0,0,0,161,0,78,0,0,0,0,
162,163,164,165,166,0,0,555,0,167,154,80,168,81,82,0,0,37,0,0,0,169,170,0,0,0,83,
0,84,85,0,0,502,503,0,86,0,0,0,0,0,0,0,87,34,80,171,81,82,77,0,37,0,0,0,0,312,
501,0,0,172,0,0,173,0,0,502,503,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,
0,160,0,0,0,161,0,78,0,0,0,0,162,163,164,165,166,0,0,760,0,167,154,80,168,81,82,
0,0,37,0,0,0,169,170,0,0,0,83,0,84,85,0,0,502,503,0,86,0,0,0,0,0,0,0,87,34,80,
171,81,82,0,0,37,0,0,0,0,312,743,0,0,172,0,0,173,0,0,502,503,0,174,158,159,175,0,
176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,
0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,34,80,171,81,82,0,0,37,0,0,0,0,312,917,0,0,172,0,0,173,0,0,502,503,0,174,158,
159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,
165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,312,0,0,0,172,0,0,173,0,0,502,503,0,
174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,
163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,
293,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,312,0,0,0,172,0,0,173,0,
0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,
0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,454,0,0,0,0,172,0,0,173,
0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,
0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,545,0,
173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,
173,0,0,0,0,571,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,
0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,
614,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,
161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,
172,0,0,173,0,0,0,0,685,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,
0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,
170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,
0,0,172,0,0,173,0,0,0,0,699,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,
0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,
169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,
714,0,0,0,0,172,0,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,
0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,
0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,
0,716,0,0,0,0,172,0,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,
77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,
0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,
0,0,0,0,0,0,0,172,816,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,
0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,
0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,
0,0,0,0,0,0,0,0,172,819,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,
179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,
0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,
0,37,0,0,0,822,0,0,0,0,172,0,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,
178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,
0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,
82,0,0,37,0,0,0,0,0,0,0,0,172,894,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,
0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,
168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,
81,82,0,0,37,0,0,0,0,0,0,0,0,172,898,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,
177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,
0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,
171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,900,0,173,0,0,0,0,0,174,158,159,175,0,176,0,
0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,
167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,959,0,173,0,0,0,0,0,174,158,159,175,0,
176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,
0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,0,
77,0,0,34,80,171,81,82,0,0,37,0,0,162,163,164,165,166,0,172,0,0,173,0,0,168,0,0,
174,0,0,175,0,176,169,170,177,0,0,178,179,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,
171,81,82,0,0,37,0,0,0,0,661,158,159,0,172,0,0,173,0,0,0,0,0,174,77,0,175,0,176,
0,0,177,0,0,178,179,0,162,163,164,165,166,0,0,0,0,0,0,0,168,0,0,0,0,0,0,0,0,169,
170,0,0,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,0,0,
162,163,164,165,166,0,307,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,0,0,
178,179,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,0,0,162,163,
164,165,166,0,322,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,0,0,178,179,
158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,0,0,162,163,164,165,
166,0,172,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,0,0,178,179,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,863,0,0,173,0,0,0,
0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,22,0,0,0,27,28,29,0,0,208,0,0,- 139,- 139,0,22,32,33,0,27,28,29,0,0,0,0,0,0,0,0,
0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,35,36,0,38,39,40,0,209,0,0,0,0,0,42,
43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,35,36,0,38,39,40,0,0,- 107,0,0,0,0,42,43,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
22,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,27,28,29,0,0,0,0,0,0,0,0,22,
32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,35,
36,0,38,39,40,0,633,- 108,0,0,0,0,42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,35,
36,0,38,39,40,0,0,0,0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,- 139,- 139,0,0,0,0,0,27,28,29,0,0,
0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
35,0,0,0,39,40,0,0,0,0,0,0,0,42,43};static short Cyc_yycheck[6381]={0,106,334,120,
337,172,342,343,344,345,161,161,550,61,137,137,336,160,366,401,382,21,370,64,137,
269,270,153,272,241,211,339,0,131,97,436,75,95,64,474,21,41,340,341,369,346,347,
145,48,348,349,146,95,21,54,55,56,115,58,17,5,6,76,77,64,65,81,94,81,415,109,0,
75,54,55,56,95,58,61,120,48,325,101,64,95,124,54,55,56,213,58,132,21,134,120,441,
64,479,98,99,75,115,503,111,115,229,109,122,134,122,124,209,112,113,114,115,128,
100,23,48,120,124,125,661,107,54,55,56,563,58,98,99,132,458,134,64,115,95,23,120,
307,141,309,93,112,113,114,115,148,124,125,132,120,134,502,322,168,106,174,302,
176,177,419,108,132,100,134,289,396,98,99,121,95,78,300,76,77,115,113,195,148,
229,87,112,113,114,115,415,286,108,48,120,115,78,206,118,229,598,115,247,95,242,
87,132,100,134,125,102,389,390,391,211,107,106,115,472,242,113,106,148,71,72,73,
124,481,95,110,109,269,270,106,272,125,117,118,111,106,119,41,109,98,99,242,211,
137,115,95,95,96,97,53,156,100,102,112,113,114,115,115,107,111,242,111,123,95,
114,117,118,120,269,270,156,272,120,242,106,293,129,130,604,109,112,129,130,325,
116,648,116,269,270,148,272,111,436,506,643,296,112,100,95,103,269,270,401,272,0,
102,95,112,94,125,113,674,242,102,200,421,210,100,212,107,125,207,111,111,325,
108,22,23,107,25,95,135,95,350,351,352,101,102,103,269,270,111,272,325,115,41,95,
515,118,467,100,245,48,102,95,96,325,53,108,503,101,374,111,412,107,61,415,67,97,
109,66,120,121,122,107,474,116,95,412,75,479,83,78,101,102,103,435,109,389,390,
391,87,441,111,116,325,759,398,95,111,118,435,98,99,101,102,103,103,450,95,96,
412,106,414,111,101,112,113,114,115,389,390,391,110,425,467,427,106,107,442,443,
109,49,50,435,648,240,111,116,135,467,216,217,719,118,220,120,121,122,450,574,
574,148,549,598,109,425,153,427,123,156,125,586,586,563,105,467,271,450,487,172,
111,586,379,105,115,382,735,118,575,111,450,106,107,115,467,849,118,111,109,491,
587,112,379,112,425,382,427,467,106,394,518,625,205,200,657,657,120,121,122,633,
207,788,213,17,18,19,292,412,488,450,665,491,298,109,419,909,421,109,229,629,112,
745,229,583,384,869,467,872,873,877,878,879,880,240,594,759,247,597,583,871,247,
106,571,250,109,405,253,260,106,594,491,109,109,255,874,425,812,427,108,261,109,
573,110,112,271,875,876,472,881,882,623,111,883,884,105,862,481,111,590,95,111,
868,64,65,115,623,102,118,106,38,106,109,95,300,307,111,74,75,101,102,103,123,
315,125,120,121,122,320,111,322,111,623,868,792,974,118,111,120,121,122,106,491,
335,109,484,849,482,78,79,641,623,80,111,109,109,552,112,112,115,556,117,928,109,
623,127,112,95,96,109,98,99,112,681,102,111,552,685,637,371,556,106,641,111,109,
113,114,845,108,109,574,699,120,379,109,111,382,112,737,583,128,704,586,623,394,
863,109,865,95,112,109,95,978,112,101,102,103,101,102,103,107,641,412,836,22,23,
412,111,994,415,106,111,116,109,569,80,120,121,122,425,126,427,107,978,106,435,
128,109,109,435,108,112,444,445,109,441,448,112,450,95,768,95,96,97,599,101,102,
103,108,109,462,779,108,106,38,95,109,95,674,109,812,109,112,464,112,109,793,626,
112,111,107,158,641,160,161,111,106,106,674,109,109,112,169,170,811,491,108,174,
108,176,177,969,109,709,579,112,80,5,6,703,909,812,139,919,109,107,109,112,195,
112,108,95,96,709,98,99,868,111,102,109,812,109,112,977,112,108,109,111,683,113,
114,735,109,812,95,112,120,389,390,391,111,696,127,128,128,111,552,106,38,106,
556,78,79,111,913,95,116,868,885,886,887,101,102,103,108,105,95,74,75,108,109,
111,812,583,729,110,868,583,265,734,120,121,122,668,594,858,859,597,594,868,80,
597,112,116,600,112,608,609,119,684,927,125,92,105,241,95,96,108,98,99,108,124,
102,698,124,302,617,81,111,115,112,111,108,113,114,112,978,109,868,112,120,109,
111,106,109,641,112,109,128,116,111,653,107,30,656,106,42,334,110,336,337,338,
339,340,341,342,343,344,345,346,347,348,349,350,351,352,100,978,674,95,110,125,
111,95,112,312,112,109,109,316,317,318,319,112,109,109,978,100,109,108,107,328,
112,109,112,332,25,26,108,978,109,713,714,709,716,706,718,112,112,38,112,712,119,
112,111,919,107,106,112,95,115,124,51,52,53,54,55,364,106,741,107,737,369,119,63,
95,108,107,978,124,115,25,102,72,73,105,119,38,112,436,112,111,119,112,112,910,
112,112,395,112,120,121,122,112,112,119,95,96,97,98,99,107,112,102,116,25,26,108,
107,785,792,106,111,111,421,114,95,106,38,80,100,120,100,111,123,112,125,112,109,
128,112,92,131,132,95,96,818,98,99,821,822,102,112,96,826,816,503,28,819,108,111,
458,113,114,111,107,827,828,95,120,80,518,108,80,101,102,103,128,116,80,126,100,
108,124,111,95,96,109,98,99,108,863,102,120,121,122,25,109,870,860,111,111,108,
113,114,95,106,112,124,506,120,96,111,123,112,125,888,108,128,112,109,131,132,95,
112,112,112,0,112,112,902,112,893,894,0,913,897,898,582,900,105,240,914,597,703,
66,45,913,245,637,909,598,633,550,629,507,674,207,709,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,338,576,24,82,83,84,85,86,87,88,89,90,91,588,953,37,61,
956,- 1,958,959,- 1,- 1,45,963,- 1,- 1,49,50,604,- 1,110,657,- 1,56,57,58,- 1,- 1,- 1,665,
- 1,- 1,- 1,983,984,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,648,- 1,- 1,- 1,- 1,100,101,655,103,104,105,- 1,- 1,661,- 1,- 1,664,- 1,
113,114,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,
52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,790,72,73,793,- 1,745,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,759,- 1,811,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,107,108,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,
- 1,- 1,128,- 1,- 1,131,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,869,-
1,871,872,873,874,875,876,877,878,879,880,881,882,883,884,885,886,887,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,849,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,25,26,27,927,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,-
1,- 1,49,50,51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,
- 1,- 1,- 1,- 1,977,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
100,101,102,103,104,105,106,107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,
123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,
- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
100,101,102,103,104,105,106,107,108,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,-
1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,
- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
100,101,102,103,104,105,106,107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,
123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
100,101,102,103,104,105,106,- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,
123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,53,54,55,56,57,58,- 1,60,61,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,- 1,107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,-
1,- 1,128,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,
55,56,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,103,104,105,- 1,
107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,- 1,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,103,104,105,- 1,- 1,- 1,- 1,- 1,
111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,0,1,-
1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,
58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,- 1,108,-
1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,121,122,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,
68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,-
1,- 1,120,121,122,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,
- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,
103,104,105,- 1,- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,121,122,1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,37,- 1,108,- 1,- 1,111,
- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,121,122,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,92,- 1,- 1,
95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,106,37,- 1,- 1,- 1,111,112,113,114,- 1,116,- 1,
118,- 1,120,121,122,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
100,101,- 1,103,104,105,106,- 1,- 1,- 1,- 1,111,112,113,114,- 1,116,- 1,118,- 1,120,121,
122,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,- 1,- 1,- 1,- 1,111,-
1,113,114,- 1,- 1,- 1,- 1,- 1,120,121,122,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,
- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,
- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,100,101,-
1,103,104,105,- 1,107,- 1,37,110,39,- 1,113,114,- 1,- 1,45,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,100,101,- 1,103,104,105,- 1,
107,- 1,- 1,- 1,- 1,- 1,113,114,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,95,- 1,- 1,
- 1,37,100,101,- 1,103,104,105,106,- 1,- 1,- 1,- 1,- 1,112,113,114,- 1,116,- 1,56,57,58,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,- 1,103,104,105,106,- 1,- 1,- 1,- 1,
- 1,112,113,114,- 1,116,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,
44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,70,
- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,
98,99,- 1,- 1,102,- 1,- 1,38,106,107,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,
123,- 1,125,25,26,128,- 1,- 1,131,132,32,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,80,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,92,- 1,60,95,96,63,98,99,- 1,- 1,102,- 1,- 1,- 1,72,
73,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,95,96,
97,98,99,38,- 1,102,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,- 1,- 1,114,- 1,- 1,117,118,- 1,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,80,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,92,- 1,60,95,96,63,98,99,- 1,- 1,102,- 1,- 1,- 1,72,73,- 1,- 1,-
1,111,- 1,113,114,- 1,- 1,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,95,96,97,98,99,- 1,
- 1,102,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,- 1,- 1,114,- 1,- 1,117,118,- 1,120,25,26,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,
107,108,- 1,- 1,111,- 1,- 1,114,- 1,- 1,117,118,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,-
1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,111,- 1,
- 1,114,- 1,- 1,117,118,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,
- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,
120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,
- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,
102,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,-
1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,
- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,
- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,-
1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,
- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,
99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,
- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,
114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,
- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,-
1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,
128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,
- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,
111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,-
1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,
- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,
- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,
114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,-
1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,
128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,
- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,
- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,52,53,54,55,- 1,111,- 1,- 1,114,- 1,- 1,63,- 1,
- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,131,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,25,26,- 1,111,- 1,- 1,114,-
1,- 1,- 1,- 1,- 1,120,38,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,51,52,53,54,55,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,52,53,54,55,-
1,111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,131,132,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,
52,53,54,55,- 1,111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,
131,132,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,
- 1,- 1,51,52,53,54,55,- 1,111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,
128,- 1,- 1,131,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,37,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,-
1,- 1,56,57,58,- 1,- 1,61,- 1,- 1,49,50,- 1,37,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,100,101,- 1,103,104,105,
- 1,107,- 1,- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,
114,100,101,- 1,103,104,105,- 1,- 1,108,- 1,- 1,- 1,- 1,113,114,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,68,69,- 1,
56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
68,69,100,101,- 1,103,104,105,- 1,92,108,- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,
105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,
114,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
113,114};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(struct _dynforward_ptr);
int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE_union
Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){57});static
int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*f1;struct _tuple1*f2;
int f3;};struct _tuple19{struct _dynforward_ptr f1;void*f2;};static char _tmp42E[8]="stdcall";
static char _tmp42F[6]="cdecl";static char _tmp430[9]="fastcall";static char _tmp431[9]="noreturn";
static char _tmp432[6]="const";static char _tmp433[8]="aligned";static char _tmp434[7]="packed";
static char _tmp435[7]="shared";static char _tmp436[7]="unused";static char _tmp437[5]="weak";
static char _tmp438[10]="dllimport";static char _tmp439[10]="dllexport";static char
_tmp43A[23]="no_instrument_function";static char _tmp43B[12]="constructor";static
char _tmp43C[11]="destructor";static char _tmp43D[22]="no_check_memory_usage";
static char _tmp43E[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp38E=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp38E;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8EE=10000;short*_tmp8EF=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp8EE));{unsigned int
_tmp8F0=_tmp8EE;unsigned int i;for(i=0;i < _tmp8F0;i ++){_tmp8EF[i]=(short)0;}}
_tmp8EF;});int yyvsp_offset;union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)({
unsigned int _tmp8EB=10000;union Cyc_YYSTYPE_union*_tmp8EC=(union Cyc_YYSTYPE_union*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),_tmp8EB));{
unsigned int _tmp8ED=_tmp8EB;unsigned int i;for(i=0;i < _tmp8ED;i ++){_tmp8EC[i]=Cyc_yylval;}}
_tmp8EC;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp8E8=10000;struct Cyc_Yyltype*_tmp8E9=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp8E8));{
unsigned int _tmp8EA=_tmp8E8;unsigned int i;for(i=0;i < _tmp8EA;i ++){_tmp8E9[i]=Cyc_yynewloc();}}
_tmp8E9;});int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp38F="parser stack overflow";_tag_dynforward(
_tmp38F,sizeof(char),_get_zero_arr_size(_tmp38F,22));}));(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1006,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 360?(int)Cyc_yytranslate[_check_known_subscript_notnull(361,Cyc_yychar)]:
275;}yyn +=yychar1;if((yyn < 0  || yyn > 6380) || Cyc_yycheck[
_check_known_subscript_notnull(6381,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6381,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1005){int
_tmp390=0;_npop_handler(0);return _tmp390;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)yyerrstatus --;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1006,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(501,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1E9: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp391;_push_handler(& _tmp391);{int _tmp393=0;if(setjmp(_tmp391.handler))_tmp393=
1;if(!_tmp393){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp392=(void*)_exn_thrown;void*
_tmp395=_tmp392;_LL1EC: if(*((void**)_tmp395)!= Cyc_Core_Failure)goto _LL1EE;
_LL1ED: x=0;goto _LL1EB;_LL1EE:;_LL1EF:(void)_throw(_tmp395);_LL1EB:;}}}{struct
_handler_cons _tmp396;_push_handler(& _tmp396);{int _tmp398=0;if(setjmp(_tmp396.handler))
_tmp398=1;if(!_tmp398){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp397=(void*)_exn_thrown;void*
_tmp39A=_tmp397;_LL1F1: if(*((void**)_tmp39A)!= Cyc_Core_Failure)goto _LL1F3;
_LL1F2: y=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(_tmp39A);_LL1F0:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp39B;(_tmp39B.YY18).tag=22;(
_tmp39B.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y);_tmp39B;});break;}case 3: _LL1EA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp39C;(_tmp39C.YY18).tag=22;(_tmp39C.YY18).f1=({struct
Cyc_List_List*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->hd=({struct Cyc_Absyn_Decl*
_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F[0]=({struct Cyc_Absyn_Using_d_struct
_tmp3A0;_tmp3A0.tag=9;_tmp3A0.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A0.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A0;});_tmp39F;}));
_tmp39E->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp39E;});_tmp39D->tl=0;_tmp39D;});_tmp39C;});Cyc_Lex_leave_using();
break;case 4: _LL1F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3A1;(
_tmp3A1.YY18).tag=22;(_tmp3A1.YY18).f1=({struct Cyc_List_List*_tmp3A2=_cycalloc(
sizeof(*_tmp3A2));_tmp3A2->hd=({struct Cyc_Absyn_Decl*_tmp3A3=_cycalloc(sizeof(*
_tmp3A3));_tmp3A3->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp3A4=
_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=({struct Cyc_Absyn_Using_d_struct _tmp3A5;
_tmp3A5.tag=9;_tmp3A5.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp3A5.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A5;});_tmp3A4;}));
_tmp3A3->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3A3;});_tmp3A2->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A2;});_tmp3A1;});
break;case 5: _LL1F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3A6;(
_tmp3A6.YY18).tag=22;(_tmp3A6.YY18).f1=({struct Cyc_List_List*_tmp3A7=_cycalloc(
sizeof(*_tmp3A7));_tmp3A7->hd=({struct Cyc_Absyn_Decl*_tmp3A8=_cycalloc(sizeof(*
_tmp3A8));_tmp3A8->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A9=
_cycalloc(sizeof(*_tmp3A9));_tmp3A9[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3AA;_tmp3AA.tag=8;_tmp3AA.f1=({struct _dynforward_ptr*_tmp3AB=_cycalloc(
sizeof(*_tmp3AB));_tmp3AB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3AB;});_tmp3AA.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3AA;});
_tmp3A9;}));_tmp3A8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp3A8;});
_tmp3A7->tl=0;_tmp3A7;});_tmp3A6;});Cyc_Lex_leave_namespace();break;case 6: _LL1F7: {
struct _dynforward_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp3AC;_push_handler(& _tmp3AC);{int _tmp3AE=0;if(setjmp(_tmp3AC.handler))
_tmp3AE=1;if(!_tmp3AE){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp3AD=(void*)_exn_thrown;void*_tmp3B0=_tmp3AD;_LL1FA: if(*((void**)_tmp3B0)
!= Cyc_Core_Failure)goto _LL1FC;_LL1FB: nspace=({const char*_tmp3B1="";
_tag_dynforward(_tmp3B1,sizeof(char),_get_zero_arr_size(_tmp3B1,1));});goto
_LL1F9;_LL1FC:;_LL1FD:(void)_throw(_tmp3B0);_LL1F9:;}}}{struct _handler_cons
_tmp3B2;_push_handler(& _tmp3B2);{int _tmp3B4=0;if(setjmp(_tmp3B2.handler))_tmp3B4=
1;if(!_tmp3B4){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp3B3=(void*)_exn_thrown;void*
_tmp3B6=_tmp3B3;_LL1FF: if(*((void**)_tmp3B6)!= Cyc_Core_Failure)goto _LL201;
_LL200: x=0;goto _LL1FE;_LL201:;_LL202:(void)_throw(_tmp3B6);_LL1FE:;}}}{struct
_handler_cons _tmp3B7;_push_handler(& _tmp3B7);{int _tmp3B9=0;if(setjmp(_tmp3B7.handler))
_tmp3B9=1;if(!_tmp3B9){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp3B8=(void*)_exn_thrown;void*
_tmp3BB=_tmp3B8;_LL204: if(*((void**)_tmp3BB)!= Cyc_Core_Failure)goto _LL206;
_LL205: y=0;goto _LL203;_LL206:;_LL207:(void)_throw(_tmp3BB);_LL203:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3BC;(_tmp3BC.YY18).tag=22;(
_tmp3BC.YY18).f1=({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));
_tmp3BD->hd=({struct Cyc_Absyn_Decl*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->r=(
void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3BF=_cycalloc(sizeof(*
_tmp3BF));_tmp3BF[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp3C0;_tmp3C0.tag=8;
_tmp3C0.f1=({struct _dynforward_ptr*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=
nspace;_tmp3C1;});_tmp3C0.f2=x;_tmp3C0;});_tmp3BF;}));_tmp3BE->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3BE;});
_tmp3BD->tl=y;_tmp3BD;});_tmp3BC;});break;}case 7: _LL1F8: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),({const char*_tmp3C2="C";_tag_dynforward(_tmp3C2,sizeof(char),
_get_zero_arr_size(_tmp3C2,2));}))== 0)yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3C3;(_tmp3C3.YY18).tag=22;(_tmp3C3.YY18).f1=({struct Cyc_List_List*_tmp3C4=
_cycalloc(sizeof(*_tmp3C4));_tmp3C4->hd=({struct Cyc_Absyn_Decl*_tmp3C5=_cycalloc(
sizeof(*_tmp3C5));_tmp3C5->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3C7;_tmp3C7.tag=10;_tmp3C7.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C7;});_tmp3C6;}));
_tmp3C5->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3C5;});_tmp3C4->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C4;});_tmp3C3;});else{
if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3C8="C include";
_tag_dynforward(_tmp3C8,sizeof(char),_get_zero_arr_size(_tmp3C8,10));}))!= 0)Cyc_Parse_err(({
const char*_tmp3C9="expecting \"C\" or \"C include\"";_tag_dynforward(_tmp3C9,
sizeof(char),_get_zero_arr_size(_tmp3C9,29));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3CA;(_tmp3CA.YY18).tag=22;(_tmp3CA.YY18).f1=({
struct Cyc_List_List*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->hd=({struct Cyc_Absyn_Decl*
_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3CE;_tmp3CE.tag=11;_tmp3CE.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3CE.f2=0;_tmp3CE;});
_tmp3CD;}));_tmp3CC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3CC;});
_tmp3CB->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3CB;});_tmp3CA;});}break;case 8: _LL208: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3CF="C include";_tag_dynforward(_tmp3CF,
sizeof(char),_get_zero_arr_size(_tmp3CF,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp3D0="expecting \"C include\"";_tag_dynforward(_tmp3D0,sizeof(char),
_get_zero_arr_size(_tmp3D0,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D1;(_tmp3D1.YY18).tag=
22;(_tmp3D1.YY18).f1=({struct Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));
_tmp3D2->hd=({struct Cyc_Absyn_Decl*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->r=(
void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3D4=_cycalloc(
sizeof(*_tmp3D4));_tmp3D4[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3D5;
_tmp3D5.tag=11;_tmp3D5.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3D5.f2=exs;_tmp3D5;});_tmp3D4;}));_tmp3D3->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3D3;});
_tmp3D2->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3D2;});_tmp3D1;});break;}case 9: _LL209: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3D6;(_tmp3D6.YY18).tag=22;(_tmp3D6.YY18).f1=({struct
Cyc_List_List*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->hd=({struct Cyc_Absyn_Decl*
_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->r=(void*)((void*)0);_tmp3D8->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3D8;});_tmp3D7->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D7;});_tmp3D6;});
break;case 10: _LL20A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D9;(_tmp3D9.YY18).tag=22;(_tmp3D9.YY18).f1=({struct Cyc_List_List*_tmp3DA=
_cycalloc(sizeof(*_tmp3DA));_tmp3DA->hd=({struct Cyc_Absyn_Decl*_tmp3DB=_cycalloc(
sizeof(*_tmp3DB));_tmp3DB->r=(void*)((void*)1);_tmp3DB->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3DB;});
_tmp3DA->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3DA;});_tmp3D9;});break;case 11: _LL20B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3DC;(_tmp3DC.YY18).tag=22;(_tmp3DC.YY18).f1=0;_tmp3DC;});
break;case 12: _LL20C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)];break;case 13: _LL20D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DD;(_tmp3DD.YY52).tag=56;(_tmp3DD.YY52).f1=0;_tmp3DD;});break;case 14: _LL20E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3DE;(_tmp3DE.YY52).tag=
56;(_tmp3DE.YY52).f1=({struct Cyc_List_List*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));
_tmp3DF->hd=({struct _tuple18*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3E0->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3E0->f3=0;_tmp3E0;});_tmp3DF->tl=0;_tmp3DF;});_tmp3DE;});break;case 15: _LL20F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E1;(_tmp3E1.YY52).tag=
56;(_tmp3E1.YY52).f1=({struct Cyc_List_List*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));
_tmp3E2->hd=({struct _tuple18*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3E3->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3E3->f3=0;_tmp3E3;});_tmp3E2->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E2;});_tmp3E1;});
break;case 16: _LL210: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E4;(_tmp3E4.YY18).tag=22;(_tmp3E4.YY18).f1=({struct Cyc_List_List*_tmp3E5=
_cycalloc(sizeof(*_tmp3E5));_tmp3E5->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3E7;_tmp3E7.tag=1;_tmp3E7.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E7;});_tmp3E6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E5->tl=0;
_tmp3E5;});_tmp3E4;});break;case 17: _LL211: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL212: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E8;(_tmp3E8.YY18).tag=22;(
_tmp3E8.YY18).f1=0;_tmp3E8;});break;case 19: _LL213: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3E9;(_tmp3E9.YY17).tag=21;(_tmp3E9.YY17).f1=Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E9;});
break;case 20: _LL214: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EA;(_tmp3EA.YY17).tag=21;(_tmp3EA.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3EB;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EA;});
break;case 21: _LL215: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EC;(_tmp3EC.YY17).tag=21;(_tmp3EC.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EC;});
break;case 22: _LL216: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3ED;(_tmp3ED.YY17).tag=21;(_tmp3ED.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3EE;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3ED;});
break;case 23: _LL217: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EF;(_tmp3EF.YY17).tag=21;(_tmp3EF.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3F0;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EF;});
break;case 24: _LL218: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F1;(_tmp3F1.YY17).tag=21;(_tmp3F1.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3F2;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F1;});
break;case 25: _LL219: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21A: Cyc_Lex_leave_using();
break;case 27: _LL21B: Cyc_Lex_enter_namespace(({struct _dynforward_ptr*_tmp3F3=
_cycalloc(sizeof(*_tmp3F3));_tmp3F3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3F3;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21C: Cyc_Lex_leave_namespace();
break;case 29: _LL21D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F4;(_tmp3F4.YY18).tag=22;(_tmp3F4.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3F4;});
break;case 30: _LL21E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F5;(_tmp3F5.YY18).tag=22;(_tmp3F5.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F5;});
break;case 31: _LL21F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F6;(_tmp3F6.YY18).tag=22;(_tmp3F6.YY18).f1=({struct Cyc_List_List*_tmp3F7=
_cycalloc(sizeof(*_tmp3F7));_tmp3F7->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F7->tl=0;
_tmp3F7;});_tmp3F6;});break;case 32: _LL220: {struct Cyc_List_List*_tmp3F8=0;{
struct Cyc_List_List*_tmp3F9=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3F9 != 0;_tmp3F9=_tmp3F9->tl){struct
_dynforward_ptr*_tmp3FA=(struct _dynforward_ptr*)_tmp3F9->hd;struct _tuple1*qv=({
struct _tuple1*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp3FE;(_tmp3FE.Rel_n).tag=1;(_tmp3FE.Rel_n).f1=0;
_tmp3FE;});_tmp3FD->f2=_tmp3FA;_tmp3FD;});struct Cyc_Absyn_Vardecl*_tmp3FB=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);_tmp3F8=({struct Cyc_List_List*_tmp3FC=_cycalloc(
sizeof(*_tmp3FC));_tmp3FC->hd=_tmp3FB;_tmp3FC->tl=_tmp3F8;_tmp3FC;});}}_tmp3F8=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3F8);yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3FF;(_tmp3FF.YY18).tag=22;(
_tmp3FF.YY18).f1=({struct Cyc_List_List*_tmp400=_cycalloc(sizeof(*_tmp400));
_tmp400->hd=Cyc_Absyn_letv_decl(_tmp3F8,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp400->tl=0;
_tmp400;});_tmp3FF;});break;}case 33: _LL221: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL222: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp401;(_tmp401.YY18).tag=22;(
_tmp401.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp401;});break;case 35: _LL223: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp402;(_tmp402.YY19).tag=23;(_tmp402.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->sc=({
struct Cyc_Core_Opt*_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp404;});_tmp403->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp403->type_specs=
0;_tmp403->is_inline=0;_tmp403->attributes=0;_tmp403;});_tmp402;});break;case 36:
_LL224: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc
!= 0)({void*_tmp405[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp406="Only one storage class is allowed in a declaration";_tag_dynforward(
_tmp406,sizeof(char),_get_zero_arr_size(_tmp406,51));}),_tag_dynforward(_tmp405,
sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp407;(_tmp407.YY19).tag=23;(_tmp407.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->sc=({struct Cyc_Core_Opt*_tmp409=
_cycalloc(sizeof(*_tmp409));_tmp409->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp409;});_tmp408->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp408->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs;_tmp408->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp408->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp408;});_tmp407;});break;case 37: _LL225: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp40A;(_tmp40A.YY19).tag=23;(_tmp40A.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->sc=0;_tmp40B->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp40B->type_specs=({struct Cyc_List_List*_tmp40C=
_cycalloc(sizeof(*_tmp40C));_tmp40C->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40C->tl=0;_tmp40C;});
_tmp40B->is_inline=0;_tmp40B->attributes=0;_tmp40B;});_tmp40A;});break;case 38:
_LL226: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp40D;(_tmp40D.YY19).tag=
23;(_tmp40D.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp40E=_cycalloc(
sizeof(*_tmp40E));_tmp40E->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp40E->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp40E->type_specs=({
struct Cyc_List_List*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp40F->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp40F;});
_tmp40E->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp40E->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp40E;});
_tmp40D;});break;case 39: _LL227: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp410;(_tmp410.YY19).tag=23;(_tmp410.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->sc=0;_tmp411->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp411->type_specs=0;
_tmp411->is_inline=0;_tmp411->attributes=0;_tmp411;});_tmp410;});break;case 40:
_LL228: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp412;(_tmp412.YY19).tag=
23;(_tmp412.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp413=_cycalloc(
sizeof(*_tmp413));_tmp413->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp413->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp413->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp413->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp413->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp413;});
_tmp412;});break;case 41: _LL229: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp414;(_tmp414.YY19).tag=23;(_tmp414.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->sc=0;_tmp415->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp415->type_specs=0;_tmp415->is_inline=1;_tmp415->attributes=
0;_tmp415;});_tmp414;});break;case 42: _LL22A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp416;(_tmp416.YY19).tag=23;(_tmp416.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp417->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp417->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp417->is_inline=1;_tmp417->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp417;});
_tmp416;});break;case 43: _LL22B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp418;(_tmp418.YY19).tag=23;(_tmp418.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->sc=0;_tmp419->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp419->type_specs=0;_tmp419->is_inline=0;_tmp419->attributes=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp419;});
_tmp418;});break;case 44: _LL22C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41A;(_tmp41A.YY19).tag=23;(_tmp41A.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp41B->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp41B->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp41B->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp41B->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp41B;});
_tmp41A;});break;case 45: _LL22D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41C;(_tmp41C.YY22).tag=26;(_tmp41C.YY22).f1=(void*)((void*)4);_tmp41C;});
break;case 46: _LL22E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41D;(_tmp41D.YY22).tag=26;(_tmp41D.YY22).f1=(void*)((void*)5);_tmp41D;});
break;case 47: _LL22F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41E;(_tmp41E.YY22).tag=26;(_tmp41E.YY22).f1=(void*)((void*)3);_tmp41E;});
break;case 48: _LL230: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41F;(_tmp41F.YY22).tag=26;(_tmp41F.YY22).f1=(void*)((void*)1);_tmp41F;});
break;case 49: _LL231: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp420="C";
_tag_dynforward(_tmp420,sizeof(char),_get_zero_arr_size(_tmp420,2));}))!= 0)Cyc_Parse_err(({
const char*_tmp421="only extern or extern \"C\" is allowed";_tag_dynforward(
_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp422;(_tmp422.YY22).tag=26;(_tmp422.YY22).f1=(void*)((
void*)2);_tmp422;});break;case 50: _LL232: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp423;(_tmp423.YY22).tag=26;(_tmp423.YY22).f1=(void*)((void*)0);_tmp423;});
break;case 51: _LL233: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp424;(_tmp424.YY22).tag=26;(_tmp424.YY22).f1=(void*)((void*)6);_tmp424;});
break;case 52: _LL234: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp425;(_tmp425.YY45).tag=49;(_tmp425.YY45).f1=0;_tmp425;});break;case 53: _LL235:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54:
_LL236: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 55: _LL237: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp426;(
_tmp426.YY45).tag=49;(_tmp426.YY45).f1=({struct Cyc_List_List*_tmp427=_cycalloc(
sizeof(*_tmp427));_tmp427->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp427->tl=0;_tmp427;});
_tmp426;});break;case 56: _LL238: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp428;(_tmp428.YY45).tag=49;(_tmp428.YY45).f1=({struct Cyc_List_List*_tmp429=
_cycalloc(sizeof(*_tmp429));_tmp429->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp429->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp429;});_tmp428;});
break;case 57: _LL239: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp42E,_tmp42E + 8},(void*)0},{{_tmp42F,
_tmp42F + 6},(void*)1},{{_tmp430,_tmp430 + 9},(void*)2},{{_tmp431,_tmp431 + 9},(
void*)3},{{_tmp432,_tmp432 + 6},(void*)4},{{_tmp433,_tmp433 + 8},(void*)&
att_aligned},{{_tmp434,_tmp434 + 7},(void*)5},{{_tmp435,_tmp435 + 7},(void*)7},{{
_tmp436,_tmp436 + 7},(void*)8},{{_tmp437,_tmp437 + 5},(void*)9},{{_tmp438,_tmp438 + 
10},(void*)10},{{_tmp439,_tmp439 + 10},(void*)11},{{_tmp43A,_tmp43A + 23},(void*)
12},{{_tmp43B,_tmp43B + 12},(void*)13},{{_tmp43C,_tmp43C + 11},(void*)14},{{
_tmp43D,_tmp43D + 22},(void*)15},{{_tmp43E,_tmp43E + 5},(void*)16}};struct
_dynforward_ptr _tmp42A=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);if((((_get_dynforward_size(_tmp42A,sizeof(char))> 4  && *((
const char*)_check_dynforward_subscript(_tmp42A,sizeof(char),0))== '_') && *((
const char*)_check_dynforward_subscript(_tmp42A,sizeof(char),1))== '_') && *((
const char*)_check_dynforward_subscript(_tmp42A,sizeof(char),(int)(
_get_dynforward_size(_tmp42A,sizeof(char))- 2)))== '_') && *((const char*)
_check_dynforward_subscript(_tmp42A,sizeof(char),(int)(_get_dynforward_size(
_tmp42A,sizeof(char))- 3)))== '_')_tmp42A=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)_tmp42A,2,_get_dynforward_size(_tmp42A,sizeof(char))- 5);{
int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((struct _dynforward_ptr)_tmp42A,(struct
_dynforward_ptr)(att_map[i]).f1)== 0){yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp42B;(_tmp42B.YY46).tag=50;(_tmp42B.YY46).f1=(void*)(att_map[i]).f2;_tmp42B;});
break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp42C="unrecognized attribute";
_tag_dynforward(_tmp42C,sizeof(char),_get_zero_arr_size(_tmp42C,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp42D;(_tmp42D.YY46).tag=50;(_tmp42D.YY46).f1=(void*)((
void*)1);_tmp42D;});}break;}}case 58: _LL23A: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp440;(_tmp440.YY46).tag=50;(_tmp440.YY46).f1=(void*)((void*)
4);_tmp440;});break;case 59: _LL23B: {struct _dynforward_ptr _tmp441=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp443;struct
_tuple11 _tmp442=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp443=_tmp442.f2;{void*a;if(Cyc_zstrcmp((struct
_dynforward_ptr)_tmp441,({const char*_tmp444="regparm";_tag_dynforward(_tmp444,
sizeof(char),_get_zero_arr_size(_tmp444,8));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)_tmp441,({const char*_tmp445="__regparm__";_tag_dynforward(
_tmp445,sizeof(char),_get_zero_arr_size(_tmp445,12));}))== 0){if(_tmp443 < 0  || 
_tmp443 > 3)Cyc_Parse_err(({const char*_tmp446="regparm requires value between 0 and 3";
_tag_dynforward(_tmp446,sizeof(char),_get_zero_arr_size(_tmp446,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp447=_cycalloc_atomic(sizeof(*_tmp447));
_tmp447[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp448;_tmp448.tag=0;_tmp448.f1=
_tmp443;_tmp448;});_tmp447;});}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp441,({const char*_tmp449="aligned";_tag_dynforward(_tmp449,sizeof(char),
_get_zero_arr_size(_tmp449,8));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp441,({const char*_tmp44A="__aligned__";_tag_dynforward(_tmp44A,sizeof(char),
_get_zero_arr_size(_tmp44A,12));}))== 0){if(_tmp443 < 0)Cyc_Parse_err(({const char*
_tmp44B="aligned requires positive power of two";_tag_dynforward(_tmp44B,sizeof(
char),_get_zero_arr_size(_tmp44B,39));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp443;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp44C="aligned requires positive power of two";
_tag_dynforward(_tmp44C,sizeof(char),_get_zero_arr_size(_tmp44C,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp44D=_cycalloc_atomic(sizeof(*_tmp44D));
_tmp44D[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp44E;_tmp44E.tag=1;_tmp44E.f1=
_tmp443;_tmp44E;});_tmp44D;});}}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp441,({const char*_tmp44F="initializes";_tag_dynforward(_tmp44F,sizeof(char),
_get_zero_arr_size(_tmp44F,12));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp441,({const char*_tmp450="__initializes__";_tag_dynforward(_tmp450,sizeof(
char),_get_zero_arr_size(_tmp450,16));}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp451=_cycalloc_atomic(sizeof(*_tmp451));_tmp451[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp452;_tmp452.tag=4;_tmp452.f1=_tmp443;_tmp452;});_tmp451;});else{Cyc_Parse_err(({
const char*_tmp453="unrecognized attribute";_tag_dynforward(_tmp453,sizeof(char),
_get_zero_arr_size(_tmp453,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp454;(_tmp454.YY46).tag=
50;(_tmp454.YY46).f1=(void*)a;_tmp454;});break;}}case 60: _LL23C: {struct
_dynforward_ptr _tmp455=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp456=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp455,({const char*_tmp457="section";_tag_dynforward(
_tmp457,sizeof(char),_get_zero_arr_size(_tmp457,8));}))== 0  || Cyc_zstrcmp((
struct _dynforward_ptr)_tmp455,({const char*_tmp458="__section__";_tag_dynforward(
_tmp458,sizeof(char),_get_zero_arr_size(_tmp458,12));}))== 0)a=(void*)({struct
Cyc_Absyn_Section_att_struct*_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({
struct Cyc_Absyn_Section_att_struct _tmp45A;_tmp45A.tag=2;_tmp45A.f1=_tmp456;
_tmp45A;});_tmp459;});else{Cyc_Parse_err(({const char*_tmp45B="unrecognized attribute";
_tag_dynforward(_tmp45B,sizeof(char),_get_zero_arr_size(_tmp45B,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp45C;(_tmp45C.YY46).tag=
50;(_tmp45C.YY46).f1=(void*)a;_tmp45C;});break;}case 61: _LL23D: {struct
_dynforward_ptr _tmp45D=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp45E=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp45D,({const char*_tmp45F="__mode__";_tag_dynforward(
_tmp45F,sizeof(char),_get_zero_arr_size(_tmp45F,9));}))== 0)a=(void*)({struct Cyc_Absyn_Mode_att_struct*
_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460[0]=({struct Cyc_Absyn_Mode_att_struct
_tmp461;_tmp461.tag=5;_tmp461.f1=_tmp45E;_tmp461;});_tmp460;});else{Cyc_Parse_err(({
const char*_tmp462="unrecognized attribute";_tag_dynforward(_tmp462,sizeof(char),
_get_zero_arr_size(_tmp462,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp463;(_tmp463.YY46).tag=
50;(_tmp463.YY46).f1=(void*)a;_tmp463;});break;}case 62: _LL23E: {struct
_dynforward_ptr _tmp464=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dynforward_ptr _tmp465=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp467;struct
_tuple11 _tmp466=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp467=_tmp466.f2;{int _tmp469;struct _tuple11 _tmp468=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp469=_tmp468.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp464,({const char*
_tmp46A="format";_tag_dynforward(_tmp46A,sizeof(char),_get_zero_arr_size(_tmp46A,
7));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp464,({const char*_tmp46B="__format__";
_tag_dynforward(_tmp46B,sizeof(char),_get_zero_arr_size(_tmp46B,11));}))== 0){
if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp465,({const char*_tmp46C="printf";
_tag_dynforward(_tmp46C,sizeof(char),_get_zero_arr_size(_tmp46C,7));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));
_tmp46D[0]=({struct Cyc_Absyn_Format_att_struct _tmp46E;_tmp46E.tag=3;_tmp46E.f1=(
void*)((void*)0);_tmp46E.f2=_tmp467;_tmp46E.f3=_tmp469;_tmp46E;});_tmp46D;});
else{if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp465,({const char*_tmp46F="scanf";
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size(_tmp46F,6));}))== 0)a=(
void*)({struct Cyc_Absyn_Format_att_struct*_tmp470=_cycalloc(sizeof(*_tmp470));
_tmp470[0]=({struct Cyc_Absyn_Format_att_struct _tmp471;_tmp471.tag=3;_tmp471.f1=(
void*)((void*)1);_tmp471.f2=_tmp467;_tmp471.f3=_tmp469;_tmp471;});_tmp470;});
else{Cyc_Parse_err(({const char*_tmp472="unrecognized format type";
_tag_dynforward(_tmp472,sizeof(char),_get_zero_arr_size(_tmp472,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp473="unrecognized attribute";_tag_dynforward(_tmp473,sizeof(char),
_get_zero_arr_size(_tmp473,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp474;(_tmp474.YY46).tag=50;(_tmp474.YY46).f1=(
void*)a;_tmp474;});break;}}}case 63: _LL23F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL240: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp475;(_tmp475.YY23).tag=27;(
_tmp475.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp477;_tmp477.tag=17;_tmp477.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp477.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp477.f3=0;_tmp477.f4=
0;_tmp477;});_tmp476;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp475;});
break;case 65: _LL241: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp478;(_tmp478.YY23).tag=27;(_tmp478.YY23).f1=(void*)Cyc_Parse_type_spec((void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp478;});break;case 66: _LL242: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp479;(_tmp479.YY23).tag=27;(_tmp479.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp479;});
break;case 67: _LL243: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp47A;(_tmp47A.YY23).tag=27;(_tmp47A.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->v=(void*)Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp47B;}),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47A;});
break;case 68: _LL244: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp47C;(_tmp47C.YY23).tag=27;(_tmp47C.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp47C;});break;case 69: _LL245: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp47D;(_tmp47D.YY23).tag=27;(_tmp47D.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Short_spec_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));
_tmp47E[0]=({struct Cyc_Parse_Short_spec_struct _tmp47F;_tmp47F.tag=2;_tmp47F.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp47F;});_tmp47E;}));_tmp47D;});break;case 70: _LL246:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp480;(_tmp480.YY23).tag=
27;(_tmp480.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp480;});
break;case 71: _LL247: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp481;(_tmp481.YY23).tag=27;(_tmp481.YY23).f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_Parse_Long_spec_struct
_tmp483;_tmp483.tag=3;_tmp483.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp483;});
_tmp482;}));_tmp481;});break;case 72: _LL248: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp484;(_tmp484.YY23).tag=27;(_tmp484.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp484;});
break;case 73: _LL249: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp485;(_tmp485.YY23).tag=27;(_tmp485.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp485;});break;case 74: _LL24A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp486;(_tmp486.YY23).tag=27;(_tmp486.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Signed_spec_struct*_tmp487=_cycalloc(sizeof(*_tmp487));
_tmp487[0]=({struct Cyc_Parse_Signed_spec_struct _tmp488;_tmp488.tag=0;_tmp488.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp488;});_tmp487;}));_tmp486;});break;case 75: _LL24B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp489;(_tmp489.YY23).tag=
27;(_tmp489.YY23).f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp48B;_tmp48B.tag=1;_tmp48B.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp48B;});
_tmp48A;}));_tmp489;});break;case 76: _LL24C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL24D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL24E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL24F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp48C;(_tmp48C.YY23).tag=
27;(_tmp48C.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48C;});
break;case 80: _LL250: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp48D;(_tmp48D.YY23).tag=27;(_tmp48D.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TupleType_struct*_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E[0]=({
struct Cyc_Absyn_TupleType_struct _tmp48F;_tmp48F.tag=9;_tmp48F.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp48F;});_tmp48E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp48D;});break;case 81: _LL251: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp490;(_tmp490.YY23).tag=27;(_tmp490.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp491=_cycalloc(sizeof(*_tmp491));
_tmp491[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp492;_tmp492.tag=15;_tmp492.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp492;});_tmp491;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp490;});
break;case 82: _LL252: {void*_tmp493=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->v=(void*)((void*)3);_tmp497;}),0);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp494;(_tmp494.YY23).tag=
27;(_tmp494.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp496;_tmp496.tag=16;_tmp496.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp496.f2=(void*)
_tmp493;_tmp496;});_tmp495;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp494;});
break;}case 83: _LL253: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp498;(_tmp498.YY23).tag=27;(_tmp498.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DynRgnType_struct*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499[0]=({
struct Cyc_Absyn_DynRgnType_struct _tmp49A;_tmp49A.tag=16;_tmp49A.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp49A.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp49A;});_tmp499;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp498;});
break;case 84: _LL254: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp49B;(_tmp49B.YY23).tag=27;(_tmp49B.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp49D;_tmp49D.tag=14;_tmp49D.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49D;});_tmp49C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp49B;});break;case 85: _LL255: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp49E;(_tmp49E.YY23).tag=27;(_tmp49E.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp49F=_cycalloc(sizeof(*_tmp49F));
_tmp49F[0]=({struct Cyc_Absyn_TagType_struct _tmp4A0;_tmp4A0.tag=18;_tmp4A0.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4A0;});_tmp49F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49E;});
break;case 86: _LL256: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A1;(_tmp4A1.YY23).tag=27;(_tmp4A1.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TagType_struct*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2[0]=({
struct Cyc_Absyn_TagType_struct _tmp4A3;_tmp4A3.tag=18;_tmp4A3.f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->v=(void*)((void*)
7);_tmp4A4;}),0);_tmp4A3;});_tmp4A2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A1;});
break;case 87: _LL257: {int _tmp4A6;struct _tuple11 _tmp4A5=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A6=_tmp4A5.f2;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4A7;(_tmp4A7.YY23).tag=27;(
_tmp4A7.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeInt_struct*
_tmp4A8=_cycalloc_atomic(sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp4A9;_tmp4A9.tag=19;_tmp4A9.f1=_tmp4A6;_tmp4A9;});_tmp4A8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A7;});
break;}case 88: _LL258: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AA;(_tmp4AA.YY43).tag=47;(_tmp4AA.YY43).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4AA;});
break;case 89: _LL259: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AB;(_tmp4AB.YY25).tag=29;(_tmp4AB.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4AC;
_tmp4AC.print_const=1;_tmp4AC.q_volatile=0;_tmp4AC.q_restrict=0;_tmp4AC.real_const=
1;_tmp4AC.loc=loc;_tmp4AC;});_tmp4AB;});break;}case 90: _LL25A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4AD;(_tmp4AD.YY25).tag=29;(_tmp4AD.YY25).f1=({struct
Cyc_Absyn_Tqual _tmp4AE;_tmp4AE.print_const=0;_tmp4AE.q_volatile=1;_tmp4AE.q_restrict=
0;_tmp4AE.real_const=0;_tmp4AE.loc=0;_tmp4AE;});_tmp4AD;});break;case 91: _LL25B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4AF;(_tmp4AF.YY25).tag=
29;(_tmp4AF.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4B0;_tmp4B0.print_const=0;
_tmp4B0.q_volatile=0;_tmp4B0.q_restrict=1;_tmp4B0.real_const=0;_tmp4B0.loc=0;
_tmp4B0;});_tmp4AF;});break;case 92: _LL25C: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4B1;(_tmp4B1.YY23).tag=27;(_tmp4B1.YY23).f1=(void*)((void*)({
struct Cyc_Parse_Decl_spec_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2[0]=({
struct Cyc_Parse_Decl_spec_struct _tmp4B3;_tmp4B3.tag=5;_tmp4B3.f1=({struct Cyc_Absyn_Decl*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp4B6;_tmp4B6.tag=6;_tmp4B6.f1=({struct Cyc_Absyn_Enumdecl*_tmp4B7=_cycalloc(
sizeof(*_tmp4B7));_tmp4B7->sc=(void*)((void*)2);_tmp4B7->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4B7->fields=({
struct Cyc_Core_Opt*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B8;});_tmp4B7;});
_tmp4B6;});_tmp4B5;}));_tmp4B4->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4B4;});
_tmp4B3;});_tmp4B2;}));_tmp4B1;});break;case 93: _LL25D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4B9;(_tmp4B9.YY23).tag=27;(_tmp4B9.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));
_tmp4BA[0]=({struct Cyc_Absyn_EnumType_struct _tmp4BB;_tmp4BB.tag=12;_tmp4BB.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4BB.f2=0;_tmp4BB;});_tmp4BA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B9;});
break;case 94: _LL25E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4BC;(_tmp4BC.YY23).tag=27;(_tmp4BC.YY23).f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_Parse_Type_spec_struct
_tmp4BE;_tmp4BE.tag=4;_tmp4BE.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4C0;_tmp4C0.tag=13;_tmp4C0.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C0;});_tmp4BF;}));
_tmp4BE.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4BE;});_tmp4BD;}));_tmp4BC;});break;case 95: _LL25F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C1;(_tmp4C1.YY47).tag=
51;(_tmp4C1.YY47).f1=({struct Cyc_Absyn_Enumfield*_tmp4C2=_cycalloc(sizeof(*
_tmp4C2));_tmp4C2->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4C2->tag=0;_tmp4C2->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4C2;});
_tmp4C1;});break;case 96: _LL260: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4C3;(_tmp4C3.YY47).tag=51;(_tmp4C3.YY47).f1=({struct Cyc_Absyn_Enumfield*
_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4C4->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C4->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4C4;});_tmp4C3;});break;case 97: _LL261: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C5;(_tmp4C5.YY48).tag=52;(
_tmp4C5.YY48).f1=({struct Cyc_List_List*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));
_tmp4C6->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C6->tl=0;_tmp4C6;});_tmp4C5;});break;case 98: _LL262: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C7;(_tmp4C7.YY48).tag=52;(
_tmp4C7.YY48).f1=({struct Cyc_List_List*_tmp4C8=_cycalloc(sizeof(*_tmp4C8));
_tmp4C8->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4C8->tl=0;_tmp4C8;});_tmp4C7;});break;case 99: _LL263: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C9;(_tmp4C9.YY48).tag=52;(
_tmp4C9.YY48).f1=({struct Cyc_List_List*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));
_tmp4CA->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4CA->tl=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4CA;});_tmp4C9;});
break;case 100: _LL264: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CB;(_tmp4CB.YY23).tag=27;(_tmp4CB.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AnonAggrType_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC[
0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4CD;_tmp4CD.tag=11;_tmp4CD.f1=(void*)
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4CD.f2=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4CD;});_tmp4CC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CB;});
break;case 101: _LL265: {struct Cyc_List_List*_tmp4CE=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4CF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D0;(_tmp4D0.YY23).tag=27;(_tmp4D0.YY23).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4D2;_tmp4D2.tag=5;_tmp4D2.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4CE,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4CF,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D2;});
_tmp4D1;}));_tmp4D0;});break;}case 102: _LL266: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D3;(_tmp4D3.YY23).tag=27;(_tmp4D3.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));
_tmp4D4[0]=({struct Cyc_Absyn_AggrType_struct _tmp4D5;_tmp4D5.tag=10;_tmp4D5.f1=({
struct Cyc_Absyn_AggrInfo _tmp4D6;_tmp4D6.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp4D7;(_tmp4D7.UnknownAggr).tag=0;(_tmp4D7.UnknownAggr).f1=(
void*)Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);(
_tmp4D7.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4D7;});_tmp4D6.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D6;});_tmp4D5;});
_tmp4D4;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4D3;});break;case 103: _LL267: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D8;(_tmp4D8.YY40).tag=44;(_tmp4D8.YY40).f1=0;_tmp4D8;});
break;case 104: _LL268: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D9;(_tmp4D9.YY40).tag=44;(_tmp4D9.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4D9;});break;
case 105: _LL269: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DA;(
_tmp4DA.YY24).tag=28;(_tmp4DA.YY24).f1=(void*)((void*)0);_tmp4DA;});break;case
106: _LL26A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DB;(
_tmp4DB.YY24).tag=28;(_tmp4DB.YY24).f1=(void*)((void*)1);_tmp4DB;});break;case
107: _LL26B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DC;(
_tmp4DC.YY26).tag=30;(_tmp4DC.YY26).f1=0;_tmp4DC;});break;case 108: _LL26C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DD;(_tmp4DD.YY26).tag=30;(
_tmp4DD.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4DD;});break;
case 109: _LL26D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DE;(
_tmp4DE.YY27).tag=31;(_tmp4DE.YY27).f1=({struct Cyc_List_List*_tmp4DF=_cycalloc(
sizeof(*_tmp4DF));_tmp4DF->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4DF->tl=0;_tmp4DF;});_tmp4DE;});break;case 110: _LL26E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E0;(_tmp4E0.YY27).tag=
31;(_tmp4E0.YY27).f1=({struct Cyc_List_List*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));
_tmp4E1->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4E1->tl=Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4E1;});_tmp4E0;});break;case 111: _LL26F: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E2;(_tmp4E2.YY21).tag=25;(_tmp4E2.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4E2;});break;case
112: _LL270: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E3;(
_tmp4E3.YY21).tag=25;(_tmp4E3.YY21).f1=({struct Cyc_List_List*_tmp4E4=_cycalloc(
sizeof(*_tmp4E4));_tmp4E4->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4E4->tl=0;_tmp4E4;});_tmp4E3;});break;case 113: _LL271:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E5;(_tmp4E5.YY21).tag=
25;(_tmp4E5.YY21).f1=({struct Cyc_List_List*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));
_tmp4E6->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4E6->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp4E6;});_tmp4E5;});break;case 114: _LL272: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E7;(_tmp4E7.YY20).tag=24;(_tmp4E7.YY20).f1=({
struct _tuple15*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E8->f2=0;_tmp4E8;});
_tmp4E7;});break;case 115: _LL273: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4E9;(_tmp4E9.YY20).tag=24;(_tmp4E9.YY20).f1=({struct _tuple15*_tmp4EA=
_cycalloc(sizeof(*_tmp4EA));_tmp4EA->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4EA->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EA;});
_tmp4E9;});break;case 116: _LL274:{struct _RegionHandle _tmp4EB=_new_region("temp");
struct _RegionHandle*temp=& _tmp4EB;_push_region(temp);{struct _tuple16 _tmp4ED;
struct Cyc_Absyn_Tqual _tmp4EE;struct Cyc_List_List*_tmp4EF;struct Cyc_List_List*
_tmp4F0;struct _tuple16*_tmp4EC=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4ED=*_tmp4EC;_tmp4EE=
_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;_tmp4F0=_tmp4ED.f3;if(_tmp4EE.loc == 0)_tmp4EE.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp4F2;struct Cyc_List_List*
_tmp4F3;struct _tuple0 _tmp4F1=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4F2=_tmp4F1.f1;
_tmp4F3=_tmp4F1.f2;{void*_tmp4F4=Cyc_Parse_speclist2typ(_tmp4EF,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4F5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp4EE,_tmp4F4,_tmp4F2,_tmp4F0),_tmp4F3);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F6;(_tmp4F6.YY26).tag=30;(_tmp4F6.YY26).f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4F5);
_tmp4F6;});}}};_pop_region(temp);}break;case 117: _LL275: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F7;(_tmp4F7.YY35).tag=39;(_tmp4F7.YY35).f1=({struct
_tuple16*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4F8->f2=({struct Cyc_List_List*_tmp4F9=_cycalloc(
sizeof(*_tmp4F9));_tmp4F9->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F9->tl=0;_tmp4F9;});
_tmp4F8->f3=0;_tmp4F8;});_tmp4F7;});break;case 118: _LL276: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4FA;(_tmp4FA.YY35).tag=39;(_tmp4FA.YY35).f1=({struct
_tuple16*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4FB->f2=({struct Cyc_List_List*
_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FC->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4FC;});_tmp4FB->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp4FB;});_tmp4FA;});break;case 119: _LL277: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4FD;(_tmp4FD.YY35).tag=39;(_tmp4FD.YY35).f1=({struct
_tuple16*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FE->f2=0;_tmp4FE->f3=0;
_tmp4FE;});_tmp4FD;});break;case 120: _LL278: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4FF;(_tmp4FF.YY35).tag=39;(_tmp4FF.YY35).f1=({struct
_tuple16*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp500->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp500->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp500;});_tmp4FF;});
break;case 121: _LL279: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp501;(_tmp501.YY35).tag=39;(_tmp501.YY35).f1=({struct _tuple16*_tmp502=
_cycalloc(sizeof(*_tmp502));_tmp502->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp502->f2=0;
_tmp502->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp502;});_tmp501;});break;case 122: _LL27A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp503;(_tmp503.YY35).tag=39;(_tmp503.YY35).f1=({struct
_tuple16*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp504->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp504->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp504;});_tmp503;});break;case 123: _LL27B: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp505;(_tmp505.YY35).tag=39;(_tmp505.YY35).f1=({struct
_tuple16*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp506->f2=({struct Cyc_List_List*_tmp507=_cycalloc(
sizeof(*_tmp507));_tmp507->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp507->tl=0;_tmp507;});
_tmp506->f3=0;_tmp506;});_tmp505;});break;case 124: _LL27C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp508;(_tmp508.YY35).tag=39;(_tmp508.YY35).f1=({struct
_tuple16*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp509->f2=({struct Cyc_List_List*
_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50A->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp50A;});_tmp509->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp509;});_tmp508;});break;case 125: _LL27D: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp50B;(_tmp50B.YY35).tag=39;(_tmp50B.YY35).f1=({struct
_tuple16*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50C->f2=0;_tmp50C->f3=0;
_tmp50C;});_tmp50B;});break;case 126: _LL27E: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp50D;(_tmp50D.YY35).tag=39;(_tmp50D.YY35).f1=({struct
_tuple16*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp50E->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp50E->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp50E;});_tmp50D;});
break;case 127: _LL27F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp50F;(_tmp50F.YY35).tag=39;(_tmp50F.YY35).f1=({struct _tuple16*_tmp510=
_cycalloc(sizeof(*_tmp510));_tmp510->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp510->f2=0;
_tmp510->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp510;});_tmp50F;});break;case 128: _LL280: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp511;(_tmp511.YY35).tag=39;(_tmp511.YY35).f1=({struct
_tuple16*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp512->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp512->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp512;});_tmp511;});break;case 129: _LL281: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp513;(_tmp513.YY21).tag=25;(_tmp513.YY21).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp513;});break;case 130:
_LL282: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp514;(_tmp514.YY21).tag=
25;(_tmp514.YY21).f1=({struct Cyc_List_List*_tmp515=_cycalloc(sizeof(*_tmp515));
_tmp515->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp515->tl=0;_tmp515;});_tmp514;});break;case 131: _LL283: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp516;(_tmp516.YY21).tag=25;(
_tmp516.YY21).f1=({struct Cyc_List_List*_tmp517=_cycalloc(sizeof(*_tmp517));
_tmp517->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp517->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp517;});_tmp516;});break;case 132: _LL284: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp518;(_tmp518.YY20).tag=24;(_tmp518.YY20).f1=({
struct _tuple15*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp519->f2=0;_tmp519;});
_tmp518;});break;case 133: _LL285: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp51A;(_tmp51A.YY20).tag=24;(_tmp51A.YY20).f1=({struct _tuple15*_tmp51B=
_cycalloc(sizeof(*_tmp51B));_tmp51B->f1=({struct Cyc_Parse_Declarator*_tmp51C=
_cycalloc(sizeof(*_tmp51C));_tmp51C->id=({struct _tuple1*_tmp51D=_cycalloc(
sizeof(*_tmp51D));_tmp51D->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp51F;(_tmp51F.Rel_n).tag=1;(_tmp51F.Rel_n).f1=0;_tmp51F;});_tmp51D->f2=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp51D;});_tmp51C->tms=0;_tmp51C;});_tmp51B->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51B;});_tmp51A;});
break;case 134: _LL286: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp520;(_tmp520.YY20).tag=24;(_tmp520.YY20).f1=({struct _tuple15*_tmp521=
_cycalloc(sizeof(*_tmp521));_tmp521->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp521->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp521;});
_tmp520;});break;case 135: _LL287: {struct Cyc_List_List*_tmp522=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp523;(_tmp523.YY23).tag=27;(_tmp523.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp524=_cycalloc(sizeof(*_tmp524));
_tmp524[0]=({struct Cyc_Parse_Decl_spec_struct _tmp525;_tmp525.tag=5;_tmp525.f1=
Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp522,({struct Cyc_Core_Opt*
_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp526;}),Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp525;});
_tmp524;}));_tmp523;});break;}case 136: _LL288: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp527;(_tmp527.YY23).tag=27;(_tmp527.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp528=_cycalloc(sizeof(*_tmp528));
_tmp528[0]=({struct Cyc_Absyn_TunionType_struct _tmp529;_tmp529.tag=2;_tmp529.f1=({
struct Cyc_Absyn_TunionInfo _tmp52A;_tmp52A.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp52B;(_tmp52B.UnknownTunion).tag=0;(_tmp52B.UnknownTunion).f1=({
struct Cyc_Absyn_UnknownTunionInfo _tmp52C;_tmp52C.name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52C.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp52C.is_flat=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp52C;});_tmp52B;});
_tmp52A.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp52A.rgn=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp52A;});_tmp529;});_tmp528;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp527;});
break;case 137: _LL289: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52D;(_tmp52D.YY23).tag=27;(_tmp52D.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));
_tmp52E[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp52F;_tmp52F.tag=3;_tmp52F.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp530;_tmp530.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp531;(_tmp531.UnknownTunionfield).tag=0;(
_tmp531.UnknownTunionfield).f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp532;
_tmp532.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp532.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp532.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp532;});_tmp531;});
_tmp530.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp530;});_tmp52F;});_tmp52E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp52D;});
break;case 138: _LL28A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp533;(_tmp533.YY31).tag=35;(_tmp533.YY31).f1=1;_tmp533;});break;case 139:
_LL28B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp534;(_tmp534.YY31).tag=
35;(_tmp534.YY31).f1=0;_tmp534;});break;case 140: _LL28C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp535;(_tmp535.YY31).tag=35;(_tmp535.YY31).f1=0;_tmp535;});
break;case 141: _LL28D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp536;(_tmp536.YY31).tag=35;(_tmp536.YY31).f1=1;_tmp536;});break;case 142:
_LL28E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp537;(_tmp537.YY34).tag=
38;(_tmp537.YY34).f1=({struct Cyc_List_List*_tmp538=_cycalloc(sizeof(*_tmp538));
_tmp538->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp538->tl=0;_tmp538;});_tmp537;});break;case 143: _LL28F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp539;(_tmp539.YY34).tag=38;(
_tmp539.YY34).f1=({struct Cyc_List_List*_tmp53A=_cycalloc(sizeof(*_tmp53A));
_tmp53A->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp53A->tl=0;_tmp53A;});_tmp539;});break;case 144: _LL290:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp53B;(_tmp53B.YY34).tag=
38;(_tmp53B.YY34).f1=({struct Cyc_List_List*_tmp53C=_cycalloc(sizeof(*_tmp53C));
_tmp53C->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp53C->tl=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53C;});_tmp53B;});
break;case 145: _LL291: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53D;(_tmp53D.YY34).tag=38;(_tmp53D.YY34).f1=({struct Cyc_List_List*_tmp53E=
_cycalloc(sizeof(*_tmp53E));_tmp53E->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp53E->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53E;});_tmp53D;});
break;case 146: _LL292: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53F;(_tmp53F.YY32).tag=36;(_tmp53F.YY32).f1=(void*)((void*)2);_tmp53F;});
break;case 147: _LL293: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp540;(_tmp540.YY32).tag=36;(_tmp540.YY32).f1=(void*)((void*)3);_tmp540;});
break;case 148: _LL294: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp541;(_tmp541.YY32).tag=36;(_tmp541.YY32).f1=(void*)((void*)0);_tmp541;});
break;case 149: _LL295: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp542;(_tmp542.YY33).tag=37;(_tmp542.YY33).f1=({struct Cyc_Absyn_Tunionfield*
_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp543->typs=0;_tmp543->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp543->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp543;});_tmp542;});
break;case 150: _LL296: {struct Cyc_List_List*_tmp544=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp545;(_tmp545.YY33).tag=37;(_tmp545.YY33).f1=({struct
Cyc_Absyn_Tunionfield*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp546->typs=
_tmp544;_tmp546->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp546->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp546;});_tmp545;});break;}case 151: _LL297: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 152: _LL298: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp547;(_tmp547.YY29).tag=33;(
_tmp547.YY29).f1=({struct Cyc_Parse_Declarator*_tmp548=_cycalloc(sizeof(*_tmp548));
_tmp548->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp548->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp548;});_tmp547;});
break;case 153: _LL299: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 154: _LL29A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp549;(_tmp549.YY29).tag=33;(_tmp549.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp54A->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp54A;});
_tmp549;});break;case 155: _LL29B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp54B;(_tmp54B.YY29).tag=33;(_tmp54B.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54C->tms=0;_tmp54C;});
_tmp54B;});break;case 156: _LL29C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 157: _LL29D: {struct Cyc_Parse_Declarator*_tmp54D=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54D->tms=({
struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));
_tmp54F[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp550;_tmp550.tag=5;_tmp550.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp550.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp550;});_tmp54F;}));
_tmp54E->tl=_tmp54D->tms;_tmp54E;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 158: _LL29E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp551;(_tmp551.YY29).tag=33;(_tmp551.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp552->tms=({
struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp554=_cycalloc(sizeof(*_tmp554));
_tmp554[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp555;_tmp555.tag=0;_tmp555.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp555.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp555;});_tmp554;}));_tmp553->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp553;});
_tmp552;});_tmp551;});break;case 159: _LL29F: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp556;(_tmp556.YY29).tag=33;(_tmp556.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp557->tms=({
struct Cyc_List_List*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp559=_cycalloc(sizeof(*_tmp559));
_tmp559[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp55A;_tmp55A.tag=1;_tmp55A.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp55A.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp55A.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp55A;});_tmp559;}));_tmp558->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp558;});
_tmp557;});_tmp556;});break;case 160: _LL2A0: {struct _tuple17 _tmp55C;struct Cyc_List_List*
_tmp55D;int _tmp55E;struct Cyc_Absyn_VarargInfo*_tmp55F;struct Cyc_Core_Opt*_tmp560;
struct Cyc_List_List*_tmp561;struct _tuple17*_tmp55B=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55C=*_tmp55B;_tmp55D=
_tmp55C.f1;_tmp55E=_tmp55C.f2;_tmp55F=_tmp55C.f3;_tmp560=_tmp55C.f4;_tmp561=
_tmp55C.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp562;(
_tmp562.YY29).tag=33;(_tmp562.YY29).f1=({struct Cyc_Parse_Declarator*_tmp563=
_cycalloc(sizeof(*_tmp563));_tmp563->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp563->tms=({
struct Cyc_List_List*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp565=_cycalloc(sizeof(*_tmp565));
_tmp565[0]=({struct Cyc_Absyn_Function_mod_struct _tmp566;_tmp566.tag=3;_tmp566.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp567=_cycalloc(sizeof(*
_tmp567));_tmp567[0]=({struct Cyc_Absyn_WithTypes_struct _tmp568;_tmp568.tag=1;
_tmp568.f1=_tmp55D;_tmp568.f2=_tmp55E;_tmp568.f3=_tmp55F;_tmp568.f4=_tmp560;
_tmp568.f5=_tmp561;_tmp568;});_tmp567;}));_tmp566;});_tmp565;}));_tmp564->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp564;});_tmp563;});_tmp562;});break;}case 161: _LL2A1: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp569;(_tmp569.YY29).tag=33;(_tmp569.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp56A->tms=({
struct Cyc_List_List*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));
_tmp56C[0]=({struct Cyc_Absyn_Function_mod_struct _tmp56D;_tmp56D.tag=3;_tmp56D.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp56E=_cycalloc(sizeof(*
_tmp56E));_tmp56E[0]=({struct Cyc_Absyn_WithTypes_struct _tmp56F;_tmp56F.tag=1;
_tmp56F.f1=0;_tmp56F.f2=0;_tmp56F.f3=0;_tmp56F.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56F.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56F;});_tmp56E;}));
_tmp56D;});_tmp56C;}));_tmp56B->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp56B;});_tmp56A;});
_tmp569;});break;case 162: _LL2A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp570;(_tmp570.YY29).tag=33;(_tmp570.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp571->tms=({
struct Cyc_List_List*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp573=_cycalloc(sizeof(*_tmp573));
_tmp573[0]=({struct Cyc_Absyn_Function_mod_struct _tmp574;_tmp574.tag=3;_tmp574.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp575=_cycalloc(sizeof(*_tmp575));
_tmp575[0]=({struct Cyc_Absyn_NoTypes_struct _tmp576;_tmp576.tag=0;_tmp576.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp576.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp576;});
_tmp575;}));_tmp574;});_tmp573;}));_tmp572->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp572;});_tmp571;});
_tmp570;});break;case 163: _LL2A3: {struct Cyc_List_List*_tmp577=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp578;(_tmp578.YY29).tag=
33;(_tmp578.YY29).f1=({struct Cyc_Parse_Declarator*_tmp579=_cycalloc(sizeof(*
_tmp579));_tmp579->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp579->tms=({struct Cyc_List_List*_tmp57A=_cycalloc(
sizeof(*_tmp57A));_tmp57A->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp57C;_tmp57C.tag=4;_tmp57C.f1=_tmp577;_tmp57C.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp57C.f3=0;
_tmp57C;});_tmp57B;}));_tmp57A->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp57A;});_tmp579;});
_tmp578;});break;}case 164: _LL2A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp57D;(_tmp57D.YY29).tag=33;(_tmp57D.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp57E->tms=({
struct Cyc_List_List*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp580=_cycalloc(sizeof(*_tmp580));
_tmp580[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp581;_tmp581.tag=5;_tmp581.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp581.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp581;});_tmp580;}));
_tmp57F->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp57F;});_tmp57E;});_tmp57D;});break;case 165: _LL2A5:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp582;(_tmp582.YY29).tag=
33;(_tmp582.YY29).f1=({struct Cyc_Parse_Declarator*_tmp583=_cycalloc(sizeof(*
_tmp583));_tmp583->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp583->tms=0;_tmp583;});_tmp582;});break;case 166: _LL2A6:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp584;(_tmp584.YY29).tag=
33;(_tmp584.YY29).f1=({struct Cyc_Parse_Declarator*_tmp585=_cycalloc(sizeof(*
_tmp585));_tmp585->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp585->tms=0;_tmp585;});_tmp584;});break;case 167: _LL2A7:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 168:
_LL2A8: {struct Cyc_Parse_Declarator*_tmp586=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp586->tms=({struct Cyc_List_List*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp589;_tmp589.tag=5;_tmp589.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp589.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp589;});_tmp588;}));_tmp587->tl=_tmp586->tms;_tmp587;});yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 169: _LL2A9:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp58A;(_tmp58A.YY29).tag=
33;(_tmp58A.YY29).f1=({struct Cyc_Parse_Declarator*_tmp58B=_cycalloc(sizeof(*
_tmp58B));_tmp58B->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp58B->tms=({struct Cyc_List_List*_tmp58C=_cycalloc(
sizeof(*_tmp58C));_tmp58C->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp58E;_tmp58E.tag=0;_tmp58E.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp58E;});
_tmp58D;}));_tmp58C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp58C;});_tmp58B;});_tmp58A;});break;case 170:
_LL2AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp58F;(_tmp58F.YY29).tag=
33;(_tmp58F.YY29).f1=({struct Cyc_Parse_Declarator*_tmp590=_cycalloc(sizeof(*
_tmp590));_tmp590->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp590->tms=({struct Cyc_List_List*_tmp591=_cycalloc(
sizeof(*_tmp591));_tmp591->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp593;_tmp593.tag=1;_tmp593.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp593.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp593.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp593;});
_tmp592;}));_tmp591->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp591;});_tmp590;});_tmp58F;});break;case 171:
_LL2AB: {struct _tuple17 _tmp595;struct Cyc_List_List*_tmp596;int _tmp597;struct Cyc_Absyn_VarargInfo*
_tmp598;struct Cyc_Core_Opt*_tmp599;struct Cyc_List_List*_tmp59A;struct _tuple17*
_tmp594=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp595=*_tmp594;_tmp596=_tmp595.f1;_tmp597=_tmp595.f2;_tmp598=_tmp595.f3;
_tmp599=_tmp595.f4;_tmp59A=_tmp595.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp59B;(_tmp59B.YY29).tag=33;(_tmp59B.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp59C->tms=({
struct Cyc_List_List*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp59E=_cycalloc(sizeof(*_tmp59E));
_tmp59E[0]=({struct Cyc_Absyn_Function_mod_struct _tmp59F;_tmp59F.tag=3;_tmp59F.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5A0=_cycalloc(sizeof(*
_tmp5A0));_tmp5A0[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5A1;_tmp5A1.tag=1;
_tmp5A1.f1=_tmp596;_tmp5A1.f2=_tmp597;_tmp5A1.f3=_tmp598;_tmp5A1.f4=_tmp599;
_tmp5A1.f5=_tmp59A;_tmp5A1;});_tmp5A0;}));_tmp59F;});_tmp59E;}));_tmp59D->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp59D;});_tmp59C;});_tmp59B;});break;}case 172: _LL2AC: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5A2;(_tmp5A2.YY29).tag=33;(_tmp5A2.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5A3->tms=({
struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));
_tmp5A5[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5A6;_tmp5A6.tag=3;_tmp5A6.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5A7=_cycalloc(sizeof(*
_tmp5A7));_tmp5A7[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5A8;_tmp5A8.tag=1;
_tmp5A8.f1=0;_tmp5A8.f2=0;_tmp5A8.f3=0;_tmp5A8.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A8.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A8;});_tmp5A7;}));
_tmp5A6;});_tmp5A5;}));_tmp5A4->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5A4;});_tmp5A3;});
_tmp5A2;});break;case 173: _LL2AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A9;(_tmp5A9.YY29).tag=33;(_tmp5A9.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5AA->tms=({
struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));
_tmp5AC[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5AD;_tmp5AD.tag=3;_tmp5AD.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));
_tmp5AE[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5AF;_tmp5AF.tag=0;_tmp5AF.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5AF;});
_tmp5AE;}));_tmp5AD;});_tmp5AC;}));_tmp5AB->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5AB;});_tmp5AA;});
_tmp5A9;});break;case 174: _LL2AE: {struct Cyc_List_List*_tmp5B0=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B1;(_tmp5B1.YY29).tag=
33;(_tmp5B1.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5B2=_cycalloc(sizeof(*
_tmp5B2));_tmp5B2->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5B2->tms=({struct Cyc_List_List*_tmp5B3=_cycalloc(
sizeof(*_tmp5B3));_tmp5B3->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5B5;_tmp5B5.tag=4;_tmp5B5.f1=_tmp5B0;_tmp5B5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5B5.f3=0;
_tmp5B5;});_tmp5B4;}));_tmp5B3->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5B3;});_tmp5B2;});
_tmp5B1;});break;}case 175: _LL2AF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B6;(_tmp5B6.YY29).tag=33;(_tmp5B6.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5B7->tms=({
struct Cyc_List_List*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));
_tmp5B9[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5BA;_tmp5BA.tag=5;_tmp5BA.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5BA.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BA;});_tmp5B9;}));
_tmp5B8->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5B8;});_tmp5B7;});_tmp5B6;});break;case 176: _LL2B0:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 177:
_LL2B1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5BB;(_tmp5BB.YY28).tag=
32;(_tmp5BB.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5BB;});break;case 178:
_LL2B2: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5BE;_tmp5BE.tag=5;_tmp5BE.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5BE.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5BE;});_tmp5BD;}));_tmp5BC->tl=ans;_tmp5BC;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp5BF=_cycalloc(
sizeof(*_tmp5BF));_tmp5BF->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp5BF->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5BF->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5BF;});ans=({struct Cyc_List_List*_tmp5C0=
_cycalloc(sizeof(*_tmp5C0));_tmp5C0->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5C2;_tmp5C2.tag=2;_tmp5C2.f1=({struct Cyc_Absyn_PtrAtts _tmp5C3;_tmp5C3.rgn=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5C3.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp5C3.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp5C3.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5C3.ptrloc=ptrloc;_tmp5C3;});_tmp5C2.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C2;});_tmp5C1;}));
_tmp5C0->tl=ans;_tmp5C0;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C4;(_tmp5C4.YY28).tag=32;(_tmp5C4.YY28).f1=ans;_tmp5C4;});break;}}case 179:
_LL2B3: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C5;(_tmp5C5.YY1).tag=5;(
_tmp5C5.YY1).f1=({struct _tuple12*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->f1=
loc;_tmp5C6->f2=Cyc_Absyn_true_conref;_tmp5C6->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C6;});_tmp5C5;});
break;}case 180: _LL2B4: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C7;(_tmp5C7.YY1).tag=5;(_tmp5C7.YY1).f1=({struct _tuple12*_tmp5C8=_cycalloc(
sizeof(*_tmp5C8));_tmp5C8->f1=loc;_tmp5C8->f2=Cyc_Absyn_false_conref;_tmp5C8->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C8;});
_tmp5C7;});break;}case 181: _LL2B5: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C9;(_tmp5C9.YY1).tag=5;(_tmp5C9.YY1).f1=({struct _tuple12*_tmp5CA=_cycalloc(
sizeof(*_tmp5CA));_tmp5CA->f1=loc;_tmp5CA->f2=Cyc_Absyn_true_conref;_tmp5CA->f3=
Cyc_Absyn_bounds_dynforward_conref;_tmp5CA;});_tmp5C9;});break;}case 182: _LL2B6: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5CB;(_tmp5CB.YY1).tag=5;(
_tmp5CB.YY1).f1=({struct _tuple12*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->f1=
loc;_tmp5CC->f2=Cyc_Absyn_true_conref;_tmp5CC->f3=Cyc_Absyn_bounds_dyneither_conref;
_tmp5CC;});_tmp5CB;});break;}case 183: _LL2B7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5CD;(_tmp5CD.YY2).tag=6;(_tmp5CD.YY2).f1=Cyc_Absyn_bounds_one_conref;
_tmp5CD;});break;case 184: _LL2B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5CE;(_tmp5CE.YY2).tag=6;(_tmp5CE.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp5D0;_tmp5D0.tag=0;_tmp5D0.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D0;});_tmp5CF;}));
_tmp5CE;});break;case 185: _LL2B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D1;(_tmp5D1.YY2).tag=6;(_tmp5D1.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_AbsUpper_b_struct*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2[0]=({
struct Cyc_Absyn_AbsUpper_b_struct _tmp5D3;_tmp5D3.tag=1;_tmp5D3.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D3;});_tmp5D2;}));
_tmp5D1;});break;case 186: _LL2BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D4;(_tmp5D4.YY51).tag=55;(_tmp5D4.YY51).f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5D4;});break;case 187: _LL2BB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D5;(_tmp5D5.YY51).tag=55;(_tmp5D5.YY51).f1=Cyc_Absyn_true_conref;
_tmp5D5;});break;case 188: _LL2BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D6;(_tmp5D6.YY51).tag=55;(_tmp5D6.YY51).f1=Cyc_Absyn_false_conref;_tmp5D6;});
break;case 189: _LL2BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D7;(_tmp5D7.YY49).tag=53;(_tmp5D7.YY49).f1=0;_tmp5D7;});break;case 190:
_LL2BE: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D8;(_tmp5D8.YY49).tag=53;(_tmp5D8.YY49).f1=({struct
Cyc_Core_Opt*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->v=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D9;});_tmp5D8;});
break;case 191: _LL2BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5DA;(_tmp5DA.YY49).tag=53;(_tmp5DA.YY49).f1=({struct Cyc_Core_Opt*_tmp5DB=
_cycalloc(sizeof(*_tmp5DB));_tmp5DB->v=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->v=(void*)((void*)3);_tmp5DC;}),0);
_tmp5DB;});_tmp5DA;});break;case 192: _LL2C0: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp5DD;(_tmp5DD.YY44).tag=48;(_tmp5DD.YY44).f1=(void*)Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->v=(void*)((void*)
5);_tmp5DE;}),0);_tmp5DD;});break;case 193: _LL2C1: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 194: _LL2C2: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5DF;(_tmp5DF.YY44).tag=48;(
_tmp5DF.YY44).f1=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp5E0=
_cycalloc(sizeof(*_tmp5E0));_tmp5E0->v=(void*)((void*)5);_tmp5E0;}),0);_tmp5DF;});
break;case 195: _LL2C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E1;(_tmp5E1.YY25).tag=29;(_tmp5E1.YY25).f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line));_tmp5E1;});
break;case 196: _LL2C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E2;(_tmp5E2.YY25).tag=29;(_tmp5E2.YY25).f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5E2;});break;case 197:
_LL2C5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E3;(_tmp5E3.YY39).tag=
43;(_tmp5E3.YY39).f1=({struct _tuple17*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));
_tmp5E4->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp5E4->f2=0;_tmp5E4->f3=0;_tmp5E4->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E4->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E4;});_tmp5E3;});
break;case 198: _LL2C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E5;(_tmp5E5.YY39).tag=43;(_tmp5E5.YY39).f1=({struct _tuple17*_tmp5E6=
_cycalloc(sizeof(*_tmp5E6));_tmp5E6->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp5E6->f2=1;_tmp5E6->f3=0;_tmp5E6->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E6->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E6;});_tmp5E5;});
break;case 199: _LL2C7: {struct _tuple2 _tmp5E8;struct Cyc_Core_Opt*_tmp5E9;struct Cyc_Absyn_Tqual
_tmp5EA;void*_tmp5EB;struct _tuple2*_tmp5E7=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E8=*_tmp5E7;_tmp5E9=
_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;_tmp5EB=_tmp5E8.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5EC=({struct Cyc_Absyn_VarargInfo*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->name=
_tmp5E9;_tmp5EF->tq=_tmp5EA;_tmp5EF->type=(void*)_tmp5EB;_tmp5EF->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5EF;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5ED;(_tmp5ED.YY39).tag=43;(
_tmp5ED.YY39).f1=({struct _tuple17*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->f1=
0;_tmp5EE->f2=0;_tmp5EE->f3=_tmp5EC;_tmp5EE->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5EE->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EE;});_tmp5ED;});
break;}}case 200: _LL2C8: {struct _tuple2 _tmp5F1;struct Cyc_Core_Opt*_tmp5F2;struct
Cyc_Absyn_Tqual _tmp5F3;void*_tmp5F4;struct _tuple2*_tmp5F0=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5F1=*_tmp5F0;_tmp5F2=
_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;_tmp5F4=_tmp5F1.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5F5=({struct Cyc_Absyn_VarargInfo*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->name=
_tmp5F2;_tmp5F8->tq=_tmp5F3;_tmp5F8->type=(void*)_tmp5F4;_tmp5F8->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5F8;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5F6;(_tmp5F6.YY39).tag=43;(
_tmp5F6.YY39).f1=({struct _tuple17*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5F7->f2=0;
_tmp5F7->f3=_tmp5F5;_tmp5F7->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F7->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F7;});_tmp5F6;});
break;}}case 201: _LL2C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F9;(_tmp5F9.YY44).tag=48;(_tmp5F9.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5FB;_tmp5FB.tag=1;_tmp5FB.f1=0;_tmp5FB;});_tmp5FA;}));_tmp5F9;});break;case
202: _LL2CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FC;(
_tmp5FC.YY44).tag=48;(_tmp5FC.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5FE;_tmp5FE.tag=0;_tmp5FE.f1=(void*)Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FE;});_tmp5FD;}));
_tmp5FC;});break;case 203: _LL2CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5FF;(_tmp5FF.YY49).tag=53;(_tmp5FF.YY49).f1=0;_tmp5FF;});break;case 204:
_LL2CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp600;(_tmp600.YY49).tag=
53;(_tmp600.YY49).f1=({struct Cyc_Core_Opt*_tmp601=_cycalloc(sizeof(*_tmp601));
_tmp601->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp602=_cycalloc(
sizeof(*_tmp602));_tmp602[0]=({struct Cyc_Absyn_JoinEff_struct _tmp603;_tmp603.tag=
21;_tmp603.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp603;});_tmp602;}));_tmp601;});_tmp600;});break;case 205:
_LL2CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp604;(_tmp604.YY50).tag=
54;(_tmp604.YY50).f1=0;_tmp604;});break;case 206: _LL2CE: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 207: _LL2CF: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp605;(_tmp605.YY50).tag=54;(
_tmp605.YY50).f1=({struct Cyc_List_List*_tmp606=_cycalloc(sizeof(*_tmp606));
_tmp606->hd=({struct _tuple4*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->f1=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp60A=_cycalloc(sizeof(*_tmp60A));
_tmp60A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp60B;_tmp60B.tag=21;_tmp60B.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp60B;});_tmp60A;});
_tmp607->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp609;_tmp609.tag=0;_tmp609.f1=(void*)((void*)3);_tmp609;});_tmp608;}));
_tmp607;});_tmp606->tl=0;_tmp606;});_tmp605;});break;case 208: _LL2D0: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp60C;(_tmp60C.YY50).tag=54;(_tmp60C.YY50).f1=({
struct Cyc_List_List*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->hd=({struct
_tuple4*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp612;_tmp612.tag=21;_tmp612.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp612;});_tmp611;});
_tmp60E->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp610;_tmp610.tag=0;_tmp610.f1=(void*)((void*)3);_tmp610;});_tmp60F;}));
_tmp60E;});_tmp60D->tl=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp60D;});_tmp60C;});break;case 209: _LL2D1: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp613;(_tmp613.YY31).tag=35;(_tmp613.YY31).f1=0;_tmp613;});
break;case 210: _LL2D2: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp614="inject";
_tag_dynforward(_tmp614,sizeof(char),_get_zero_arr_size(_tmp614,7));}))!= 0)Cyc_Parse_err(({
const char*_tmp615="missing type in function declaration";_tag_dynforward(_tmp615,
sizeof(char),_get_zero_arr_size(_tmp615,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp616;(_tmp616.YY31).tag=35;(_tmp616.YY31).f1=1;_tmp616;});
break;case 211: _LL2D3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 212: _LL2D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp617;(_tmp617.YY40).tag=44;(_tmp617.YY40).f1=Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp617;});break;case 213:
_LL2D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp618;(_tmp618.YY40).tag=
44;(_tmp618.YY40).f1=0;_tmp618;});break;case 214: _LL2D6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 215: _LL2D7:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp619;(_tmp619.YY40).tag=
44;(_tmp619.YY40).f1=({struct Cyc_List_List*_tmp61A=_cycalloc(sizeof(*_tmp61A));
_tmp61A->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp61B=_cycalloc(
sizeof(*_tmp61B));_tmp61B[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp61C;_tmp61C.tag=
22;_tmp61C.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp61C;});_tmp61B;}));_tmp61A->tl=0;_tmp61A;});_tmp619;});
break;case 216: _LL2D8: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)6,0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp61D;(_tmp61D.YY40).tag=44;(_tmp61D.YY40).f1=({struct
Cyc_List_List*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61E->tl=0;_tmp61E;});
_tmp61D;});break;case 217: _LL2D9: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp61F;(_tmp61F.YY40).tag=44;(_tmp61F.YY40).f1=({struct
Cyc_List_List*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp622;_tmp622.tag=20;_tmp622.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp622;});_tmp621;}));
_tmp620->tl=0;_tmp620;});_tmp61F;});break;case 218: _LL2DA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)3,0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp623;(
_tmp623.YY40).tag=44;(_tmp623.YY40).f1=({struct Cyc_List_List*_tmp624=_cycalloc(
sizeof(*_tmp624));_tmp624->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp626;_tmp626.tag=20;_tmp626.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp626;});_tmp625;}));
_tmp624->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp624;});_tmp623;});break;case 219: _LL2DB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp627;(_tmp627.YY38).tag=42;(_tmp627.YY38).f1=({struct
Cyc_List_List*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp628->tl=0;_tmp628;});
_tmp627;});break;case 220: _LL2DC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp629;(_tmp629.YY38).tag=42;(_tmp629.YY38).f1=({struct Cyc_List_List*_tmp62A=
_cycalloc(sizeof(*_tmp62A));_tmp62A->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62A->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp62A;});_tmp629;});
break;case 221: _LL2DD: {struct _tuple16 _tmp62C;struct Cyc_Absyn_Tqual _tmp62D;struct
Cyc_List_List*_tmp62E;struct Cyc_List_List*_tmp62F;struct _tuple16*_tmp62B=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62C=*_tmp62B;
_tmp62D=_tmp62C.f1;_tmp62E=_tmp62C.f2;_tmp62F=_tmp62C.f3;if(_tmp62D.loc == 0)
_tmp62D.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp631;struct _tuple1*
_tmp632;struct Cyc_List_List*_tmp633;struct Cyc_Parse_Declarator*_tmp630=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp631=*_tmp630;
_tmp632=_tmp631.id;_tmp633=_tmp631.tms;{void*_tmp634=Cyc_Parse_speclist2typ(
_tmp62E,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp636;void*_tmp637;struct
Cyc_List_List*_tmp638;struct Cyc_List_List*_tmp639;struct _tuple6 _tmp635=Cyc_Parse_apply_tms(
_tmp62D,_tmp634,_tmp62F,_tmp633);_tmp636=_tmp635.f1;_tmp637=_tmp635.f2;_tmp638=
_tmp635.f3;_tmp639=_tmp635.f4;if(_tmp638 != 0)Cyc_Parse_err(({const char*_tmp63A="parameter with bad type params";
_tag_dynforward(_tmp63A,sizeof(char),_get_zero_arr_size(_tmp63A,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp632))Cyc_Parse_err(({const char*_tmp63B="parameter cannot be qualified with a namespace";
_tag_dynforward(_tmp63B,sizeof(char),_get_zero_arr_size(_tmp63B,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp63C=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp641=_cycalloc(sizeof(*
_tmp641));_tmp641->v=(*_tmp632).f2;_tmp641;});if(_tmp639 != 0)({void*_tmp63D[0]={};
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp63E="extra attributes on parameter, ignoring";
_tag_dynforward(_tmp63E,sizeof(char),_get_zero_arr_size(_tmp63E,40));}),
_tag_dynforward(_tmp63D,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp63F;(_tmp63F.YY37).tag=41;(_tmp63F.YY37).f1=({struct
_tuple2*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->f1=_tmp63C;_tmp640->f2=
_tmp636;_tmp640->f3=_tmp637;_tmp640;});_tmp63F;});break;}}}}case 222: _LL2DE: {
struct _tuple16 _tmp643;struct Cyc_Absyn_Tqual _tmp644;struct Cyc_List_List*_tmp645;
struct Cyc_List_List*_tmp646;struct _tuple16*_tmp642=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp643=*_tmp642;_tmp644=
_tmp643.f1;_tmp645=_tmp643.f2;_tmp646=_tmp643.f3;if(_tmp644.loc == 0)_tmp644.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp647=Cyc_Parse_speclist2typ(_tmp645,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp646 != 0)({
void*_tmp648[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp649="bad attributes on parameter, ignoring";_tag_dynforward(_tmp649,sizeof(
char),_get_zero_arr_size(_tmp649,38));}),_tag_dynforward(_tmp648,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp64A;(_tmp64A.YY37).tag=
41;(_tmp64A.YY37).f1=({struct _tuple2*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->f1=
0;_tmp64B->f2=_tmp644;_tmp64B->f3=_tmp647;_tmp64B;});_tmp64A;});break;}}case 223:
_LL2DF: {struct _tuple16 _tmp64D;struct Cyc_Absyn_Tqual _tmp64E;struct Cyc_List_List*
_tmp64F;struct Cyc_List_List*_tmp650;struct _tuple16*_tmp64C=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64D=*_tmp64C;_tmp64E=
_tmp64D.f1;_tmp64F=_tmp64D.f2;_tmp650=_tmp64D.f3;if(_tmp64E.loc == 0)_tmp64E.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp651=Cyc_Parse_speclist2typ(_tmp64F,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp652=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp654;void*_tmp655;struct Cyc_List_List*_tmp656;struct Cyc_List_List*
_tmp657;struct _tuple6 _tmp653=Cyc_Parse_apply_tms(_tmp64E,_tmp651,_tmp650,_tmp652);
_tmp654=_tmp653.f1;_tmp655=_tmp653.f2;_tmp656=_tmp653.f3;_tmp657=_tmp653.f4;if(
_tmp656 != 0)({void*_tmp658[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp659="bad type parameters on formal argument, ignoring";_tag_dynforward(
_tmp659,sizeof(char),_get_zero_arr_size(_tmp659,49));}),_tag_dynforward(_tmp658,
sizeof(void*),0));});if(_tmp657 != 0)({void*_tmp65A[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp65B="bad attributes on parameter, ignoring";_tag_dynforward(_tmp65B,sizeof(
char),_get_zero_arr_size(_tmp65B,38));}),_tag_dynforward(_tmp65A,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp65C;(_tmp65C.YY37).tag=
41;(_tmp65C.YY37).f1=({struct _tuple2*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->f1=
0;_tmp65D->f2=_tmp654;_tmp65D->f3=_tmp655;_tmp65D;});_tmp65C;});break;}}case 224:
_LL2E0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp65E;(_tmp65E.YY36).tag=
40;(_tmp65E.YY36).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));
_tmp65E;});break;case 225: _LL2E1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp65F;(_tmp65F.YY36).tag=40;(_tmp65F.YY36).f1=({struct Cyc_List_List*_tmp660=
_cycalloc(sizeof(*_tmp660));_tmp660->hd=({struct _dynforward_ptr*_tmp661=
_cycalloc(sizeof(*_tmp661));_tmp661[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp661;});_tmp660->tl=0;
_tmp660;});_tmp65F;});break;case 226: _LL2E2: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp662;(_tmp662.YY36).tag=40;(_tmp662.YY36).f1=({struct Cyc_List_List*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->hd=({struct _dynforward_ptr*_tmp664=
_cycalloc(sizeof(*_tmp664));_tmp664[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp664;});_tmp663->tl=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp663;});_tmp662;});
break;case 227: _LL2E3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 228: _LL2E4: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 229: _LL2E5: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp665;(_tmp665.YY3).tag=7;(_tmp665.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp666=_cycalloc(sizeof(*_tmp666));
_tmp666[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp667;_tmp667.tag=37;
_tmp667.f1=0;_tmp667.f2=0;_tmp667;});_tmp666;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp665;});
break;case 230: _LL2E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp668;(_tmp668.YY3).tag=7;(_tmp668.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp66A;_tmp66A.tag=37;_tmp66A.f1=0;
_tmp66A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp66A;});_tmp669;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp668;});
break;case 231: _LL2E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp66B;(_tmp66B.YY3).tag=7;(_tmp66B.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp66D;_tmp66D.tag=37;_tmp66D.f1=0;
_tmp66D.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp66D;});_tmp66C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp66B;});
break;case 232: _LL2E8: {struct Cyc_Absyn_Vardecl*_tmp66E=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp674;(_tmp674.Loc_n).tag=0;_tmp674;});_tmp672->f2=({
struct _dynforward_ptr*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp673;});_tmp672;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp66E->tq).real_const=
1;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp66F;(_tmp66F.YY3).tag=
7;(_tmp66F.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp671;_tmp671.tag=29;_tmp671.f1=_tmp66E;_tmp671.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp671.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp671.f4=0;
_tmp671;});_tmp670;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp66F;});
break;}case 233: _LL2E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp675;(_tmp675.YY6).tag=10;(_tmp675.YY6).f1=({struct Cyc_List_List*_tmp676=
_cycalloc(sizeof(*_tmp676));_tmp676->hd=({struct _tuple20*_tmp677=_cycalloc(
sizeof(*_tmp677));_tmp677->f1=0;_tmp677->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp677;});_tmp676->tl=0;
_tmp676;});_tmp675;});break;case 234: _LL2EA: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp678;(_tmp678.YY6).tag=10;(_tmp678.YY6).f1=({struct Cyc_List_List*
_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->hd=({struct _tuple20*_tmp67A=
_cycalloc(sizeof(*_tmp67A));_tmp67A->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp67A->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67A;});_tmp679->tl=
0;_tmp679;});_tmp678;});break;case 235: _LL2EB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp67B;(_tmp67B.YY6).tag=10;(_tmp67B.YY6).f1=({struct Cyc_List_List*
_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->hd=({struct _tuple20*_tmp67D=
_cycalloc(sizeof(*_tmp67D));_tmp67D->f1=0;_tmp67D->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67D;});_tmp67C->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp67C;});_tmp67B;});
break;case 236: _LL2EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp67E;(_tmp67E.YY6).tag=10;(_tmp67E.YY6).f1=({struct Cyc_List_List*_tmp67F=
_cycalloc(sizeof(*_tmp67F));_tmp67F->hd=({struct _tuple20*_tmp680=_cycalloc(
sizeof(*_tmp680));_tmp680->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp680->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp680;});_tmp67F->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp67F;});_tmp67E;});
break;case 237: _LL2ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp681;(_tmp681.YY41).tag=45;(_tmp681.YY41).f1=Cyc_List_imp_rev(Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp681;});break;
case 238: _LL2EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp682;(
_tmp682.YY41).tag=45;(_tmp682.YY41).f1=({struct Cyc_List_List*_tmp683=_cycalloc(
sizeof(*_tmp683));_tmp683->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp683->tl=0;_tmp683;});
_tmp682;});break;case 239: _LL2EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp684;(_tmp684.YY41).tag=45;(_tmp684.YY41).f1=({struct Cyc_List_List*_tmp685=
_cycalloc(sizeof(*_tmp685));_tmp685->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp685->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp685;});_tmp684;});
break;case 240: _LL2F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp686;(_tmp686.YY42).tag=46;(_tmp686.YY42).f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp688;_tmp688.tag=0;_tmp688.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp688;});_tmp687;}));
_tmp686;});break;case 241: _LL2F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp689;(_tmp689.YY42).tag=46;(_tmp689.YY42).f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A[0]=({struct Cyc_Absyn_FieldName_struct
_tmp68B;_tmp68B.tag=1;_tmp68B.f1=({struct _dynforward_ptr*_tmp68C=_cycalloc(
sizeof(*_tmp68C));_tmp68C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp68C;});_tmp68B;});
_tmp68A;}));_tmp689;});break;case 242: _LL2F2: {struct _tuple16 _tmp68E;struct Cyc_Absyn_Tqual
_tmp68F;struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp691;struct _tuple16*
_tmp68D=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp68E=*_tmp68D;_tmp68F=_tmp68E.f1;_tmp690=_tmp68E.f2;_tmp691=_tmp68E.f3;{void*
_tmp692=Cyc_Parse_speclist2typ(_tmp690,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp691 != 0)({
void*_tmp693[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp694="ignoring attributes in type";_tag_dynforward(_tmp694,sizeof(char),
_get_zero_arr_size(_tmp694,28));}),_tag_dynforward(_tmp693,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp695;(_tmp695.YY37).tag=
41;(_tmp695.YY37).f1=({struct _tuple2*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->f1=
0;_tmp696->f2=_tmp68F;_tmp696->f3=_tmp692;_tmp696;});_tmp695;});break;}}case 243:
_LL2F3: {struct _tuple16 _tmp698;struct Cyc_Absyn_Tqual _tmp699;struct Cyc_List_List*
_tmp69A;struct Cyc_List_List*_tmp69B;struct _tuple16*_tmp697=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp698=*_tmp697;_tmp699=
_tmp698.f1;_tmp69A=_tmp698.f2;_tmp69B=_tmp698.f3;{void*_tmp69C=Cyc_Parse_speclist2typ(
_tmp69A,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp69D=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp69E=
Cyc_Parse_apply_tms(_tmp699,_tmp69C,_tmp69B,_tmp69D);if(_tmp69E.f3 != 0)({void*
_tmp69F[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6A0="bad type params, ignoring";_tag_dynforward(_tmp6A0,sizeof(char),
_get_zero_arr_size(_tmp6A0,26));}),_tag_dynforward(_tmp69F,sizeof(void*),0));});
if(_tmp69E.f4 != 0)({void*_tmp6A1[0]={};Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6A2="bad specifiers, ignoring";_tag_dynforward(_tmp6A2,sizeof(char),
_get_zero_arr_size(_tmp6A2,25));}),_tag_dynforward(_tmp6A1,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A3;(_tmp6A3.YY37).tag=
41;(_tmp6A3.YY37).f1=({struct _tuple2*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->f1=
0;_tmp6A4->f2=_tmp69E.f1;_tmp6A4->f3=_tmp69E.f2;_tmp6A4;});_tmp6A3;});break;}}
case 244: _LL2F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A5;(
_tmp6A5.YY44).tag=48;(_tmp6A5.YY44).f1=(void*)(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp6A5;});break;case
245: _LL2F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A6;(
_tmp6A6.YY44).tag=48;(_tmp6A6.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6A8;_tmp6A8.tag=21;_tmp6A8.f1=0;_tmp6A8;});_tmp6A7;}));_tmp6A6;});break;case
246: _LL2F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A9;(
_tmp6A9.YY44).tag=48;(_tmp6A9.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6AB;_tmp6AB.tag=21;_tmp6AB.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AB;});_tmp6AA;}));
_tmp6A9;});break;case 247: _LL2F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AC;(_tmp6AC.YY44).tag=48;(_tmp6AC.YY44).f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6AE;_tmp6AE.tag=22;_tmp6AE.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AE;});_tmp6AD;}));
_tmp6AC;});break;case 248: _LL2F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AF;(_tmp6AF.YY44).tag=48;(_tmp6AF.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6B1;_tmp6B1.tag=21;_tmp6B1.f1=({struct Cyc_List_List*_tmp6B2=_cycalloc(
sizeof(*_tmp6B2));_tmp6B2->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B2->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B2;});_tmp6B1;});
_tmp6B0;}));_tmp6AF;});break;case 249: _LL2F9: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6B3;(_tmp6B3.YY40).tag=44;(_tmp6B3.YY40).f1=({struct
Cyc_List_List*_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B4->tl=0;_tmp6B4;});
_tmp6B3;});break;case 250: _LL2FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B5;(_tmp6B5.YY40).tag=44;(_tmp6B5.YY40).f1=({struct Cyc_List_List*_tmp6B6=
_cycalloc(sizeof(*_tmp6B6));_tmp6B6->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B6->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B6;});_tmp6B5;});
break;case 251: _LL2FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B7;(_tmp6B7.YY30).tag=34;(_tmp6B7.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B8;});_tmp6B7;});
break;case 252: _LL2FC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 253: _LL2FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B9;(_tmp6B9.YY30).tag=34;(_tmp6B9.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->tms=Cyc_List_imp_append(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6BA;});
_tmp6B9;});break;case 254: _LL2FE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 255: _LL2FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6BB;(_tmp6BB.YY30).tag=34;(_tmp6BB.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->tms=({struct Cyc_List_List*_tmp6BD=
_cycalloc(sizeof(*_tmp6BD));_tmp6BD->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6BF;_tmp6BF.tag=0;_tmp6BF.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6BF;});
_tmp6BE;}));_tmp6BD->tl=0;_tmp6BD;});_tmp6BC;});_tmp6BB;});break;case 256: _LL300:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6C0;(_tmp6C0.YY30).tag=
34;(_tmp6C0.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6C1=_cycalloc(
sizeof(*_tmp6C1));_tmp6C1->tms=({struct Cyc_List_List*_tmp6C2=_cycalloc(sizeof(*
_tmp6C2));_tmp6C2->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp6C3=
_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6C4;_tmp6C4.tag=0;_tmp6C4.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C4.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6C4;});
_tmp6C3;}));_tmp6C2->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp6C2;});_tmp6C1;});_tmp6C0;});break;case 257:
_LL301: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6C5;(_tmp6C5.YY30).tag=
34;(_tmp6C5.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6C6=_cycalloc(
sizeof(*_tmp6C6));_tmp6C6->tms=({struct Cyc_List_List*_tmp6C7=_cycalloc(sizeof(*
_tmp6C7));_tmp6C7->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6C9;_tmp6C9.tag=1;_tmp6C9.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C9.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C9.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6C9;});
_tmp6C8;}));_tmp6C7->tl=0;_tmp6C7;});_tmp6C6;});_tmp6C5;});break;case 258: _LL302:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6CA;(_tmp6CA.YY30).tag=
34;(_tmp6CA.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6CB=_cycalloc(
sizeof(*_tmp6CB));_tmp6CB->tms=({struct Cyc_List_List*_tmp6CC=_cycalloc(sizeof(*
_tmp6CC));_tmp6CC->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6CE;_tmp6CE.tag=1;_tmp6CE.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CE.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CE.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6CE;});
_tmp6CD;}));_tmp6CC->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp6CC;});_tmp6CB;});_tmp6CA;});break;case 259:
_LL303: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6CF;(_tmp6CF.YY30).tag=
34;(_tmp6CF.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6D0=_cycalloc(
sizeof(*_tmp6D0));_tmp6D0->tms=({struct Cyc_List_List*_tmp6D1=_cycalloc(sizeof(*
_tmp6D1));_tmp6D1->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D3;_tmp6D3.tag=3;_tmp6D3.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6D5;_tmp6D5.tag=1;_tmp6D5.f1=0;_tmp6D5.f2=0;_tmp6D5.f3=0;_tmp6D5.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D5.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D5;});_tmp6D4;}));
_tmp6D3;});_tmp6D2;}));_tmp6D1->tl=0;_tmp6D1;});_tmp6D0;});_tmp6CF;});break;case
260: _LL304: {struct _tuple17 _tmp6D7;struct Cyc_List_List*_tmp6D8;int _tmp6D9;struct
Cyc_Absyn_VarargInfo*_tmp6DA;struct Cyc_Core_Opt*_tmp6DB;struct Cyc_List_List*
_tmp6DC;struct _tuple17*_tmp6D6=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D7=*_tmp6D6;_tmp6D8=
_tmp6D7.f1;_tmp6D9=_tmp6D7.f2;_tmp6DA=_tmp6D7.f3;_tmp6DB=_tmp6D7.f4;_tmp6DC=
_tmp6D7.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6DD;(
_tmp6DD.YY30).tag=34;(_tmp6DD.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->tms=({struct Cyc_List_List*_tmp6DF=
_cycalloc(sizeof(*_tmp6DF));_tmp6DF->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6E1;_tmp6E1.tag=3;_tmp6E1.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6E3;_tmp6E3.tag=1;_tmp6E3.f1=_tmp6D8;_tmp6E3.f2=_tmp6D9;_tmp6E3.f3=_tmp6DA;
_tmp6E3.f4=_tmp6DB;_tmp6E3.f5=_tmp6DC;_tmp6E3;});_tmp6E2;}));_tmp6E1;});_tmp6E0;}));
_tmp6DF->tl=0;_tmp6DF;});_tmp6DE;});_tmp6DD;});break;}case 261: _LL305: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6E4;(_tmp6E4.YY30).tag=34;(
_tmp6E4.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6E5=_cycalloc(sizeof(*
_tmp6E5));_tmp6E5->tms=({struct Cyc_List_List*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));
_tmp6E6->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6E7=
_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6E8;_tmp6E8.tag=3;_tmp6E8.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6EA;_tmp6EA.tag=1;_tmp6EA.f1=0;_tmp6EA.f2=0;_tmp6EA.f3=0;_tmp6EA.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6EA.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EA;});_tmp6E9;}));
_tmp6E8;});_tmp6E7;}));_tmp6E6->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6E6;});_tmp6E5;});
_tmp6E4;});break;case 262: _LL306: {struct _tuple17 _tmp6EC;struct Cyc_List_List*
_tmp6ED;int _tmp6EE;struct Cyc_Absyn_VarargInfo*_tmp6EF;struct Cyc_Core_Opt*_tmp6F0;
struct Cyc_List_List*_tmp6F1;struct _tuple17*_tmp6EB=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EC=*_tmp6EB;_tmp6ED=
_tmp6EC.f1;_tmp6EE=_tmp6EC.f2;_tmp6EF=_tmp6EC.f3;_tmp6F0=_tmp6EC.f4;_tmp6F1=
_tmp6EC.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F2;(
_tmp6F2.YY30).tag=34;(_tmp6F2.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->tms=({struct Cyc_List_List*_tmp6F4=
_cycalloc(sizeof(*_tmp6F4));_tmp6F4->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6F6;_tmp6F6.tag=3;_tmp6F6.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6F8;_tmp6F8.tag=1;_tmp6F8.f1=_tmp6ED;_tmp6F8.f2=_tmp6EE;_tmp6F8.f3=_tmp6EF;
_tmp6F8.f4=_tmp6F0;_tmp6F8.f5=_tmp6F1;_tmp6F8;});_tmp6F7;}));_tmp6F6;});_tmp6F5;}));
_tmp6F4->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp6F4;});_tmp6F3;});_tmp6F2;});break;}case 263: _LL307: {
struct Cyc_List_List*_tmp6F9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6FA;(_tmp6FA.YY30).tag=
34;(_tmp6FA.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6FB=_cycalloc(
sizeof(*_tmp6FB));_tmp6FB->tms=({struct Cyc_List_List*_tmp6FC=_cycalloc(sizeof(*
_tmp6FC));_tmp6FC->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp6FE;_tmp6FE.tag=4;_tmp6FE.f1=_tmp6F9;_tmp6FE.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6FE.f3=0;
_tmp6FE;});_tmp6FD;}));_tmp6FC->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6FC;});_tmp6FB;});
_tmp6FA;});break;}case 264: _LL308: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6FF;(_tmp6FF.YY30).tag=34;(_tmp6FF.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700->tms=({struct Cyc_List_List*_tmp701=
_cycalloc(sizeof(*_tmp701));_tmp701->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp703;_tmp703.tag=5;_tmp703.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp703.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp703;});_tmp702;}));
_tmp701->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp701;});_tmp700;});_tmp6FF;});break;case 265: _LL309:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 266:
_LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
267: _LL30B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 268: _LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 269: _LL30D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 270: _LL30E: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 271: _LL30F: if(Cyc_zstrcmp((struct _dynforward_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({
const char*_tmp704="`H";_tag_dynforward(_tmp704,sizeof(char),_get_zero_arr_size(
_tmp704,3));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp705="`U";
_tag_dynforward(_tmp705,sizeof(char),_get_zero_arr_size(_tmp705,3));}))== 0)Cyc_Parse_err((
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp708;_tmp708.tag=0;_tmp708.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{void*_tmp706[1]={&
_tmp708};Cyc_aprintf(({const char*_tmp707="bad occurrence of heap region %s";
_tag_dynforward(_tmp707,sizeof(char),_get_zero_arr_size(_tmp707,33));}),
_tag_dynforward(_tmp706,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->name=({
struct _dynforward_ptr*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp716;});_tmp713->identity=
0;_tmp713->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp714=_cycalloc(
sizeof(*_tmp714));_tmp714[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp715;_tmp715.tag=0;
_tmp715.f1=(void*)((void*)3);_tmp715;});_tmp714;}));_tmp713;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711[0]=({
struct Cyc_Absyn_VarType_struct _tmp712;_tmp712.tag=1;_tmp712.f1=tv;_tmp712;});
_tmp711;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp709;(
_tmp709.YY9).tag=13;(_tmp709.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*
_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_Absyn_Region_s_struct
_tmp70B;_tmp70B.tag=15;_tmp70B.f1=tv;_tmp70B.f2=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp70E;(_tmp70E.Loc_n).tag=0;_tmp70E;});_tmp70C->f2=({
struct _dynforward_ptr*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70D;});_tmp70C;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp70F=_cycalloc(sizeof(*_tmp70F));
_tmp70F[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp710;_tmp710.tag=15;_tmp710.f1=(
void*)t;_tmp710;});_tmp70F;}),0);_tmp70B.f3=0;_tmp70B.f4=0;_tmp70B.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70B;});_tmp70A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp709;});break;}case 272: _LL310: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp717="H";_tag_dynforward(_tmp717,sizeof(
char),_get_zero_arr_size(_tmp717,2));}))== 0)Cyc_Parse_err(({const char*_tmp718="bad occurrence of heap region `H";
_tag_dynforward(_tmp718,sizeof(char),_get_zero_arr_size(_tmp718,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723->name=({
struct _dynforward_ptr*_tmp726=_cycalloc(sizeof(*_tmp726));_tmp726[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp729;_tmp729.tag=0;_tmp729.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp727[1]={&
_tmp729};Cyc_aprintf(({const char*_tmp728="`%s";_tag_dynforward(_tmp728,sizeof(
char),_get_zero_arr_size(_tmp728,4));}),_tag_dynforward(_tmp727,sizeof(void*),1));}});
_tmp726;});_tmp723->identity=0;_tmp723->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp725;_tmp725.tag=0;_tmp725.f1=(void*)((void*)3);_tmp725;});_tmp724;}));
_tmp723;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp721=_cycalloc(
sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Absyn_VarType_struct _tmp722;_tmp722.tag=
1;_tmp722.f1=tv;_tmp722;});_tmp721;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp719;(_tmp719.YY9).tag=13;(_tmp719.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=({
struct Cyc_Absyn_Region_s_struct _tmp71B;_tmp71B.tag=15;_tmp71B.f1=tv;_tmp71B.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp71C=_cycalloc(sizeof(*_tmp71C));
_tmp71C->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp71E;(
_tmp71E.Loc_n).tag=0;_tmp71E;});_tmp71C->f2=({struct _dynforward_ptr*_tmp71D=
_cycalloc(sizeof(*_tmp71D));_tmp71D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71D;});_tmp71C;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp71F=_cycalloc(sizeof(*_tmp71F));
_tmp71F[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp720;_tmp720.tag=15;_tmp720.f1=(
void*)t;_tmp720;});_tmp71F;}),0);_tmp71B.f3=0;_tmp71B.f4=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71B.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71B;});_tmp71A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp719;});break;}case 273: _LL311: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 6)]),({const char*_tmp72A="resetable";_tag_dynforward(_tmp72A,
sizeof(char),_get_zero_arr_size(_tmp72A,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp72B="expecting [resetable]";_tag_dynforward(_tmp72B,sizeof(char),
_get_zero_arr_size(_tmp72B,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp72C="`H";_tag_dynforward(_tmp72C,
sizeof(char),_get_zero_arr_size(_tmp72C,3));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp72D="`U";_tag_dynforward(_tmp72D,sizeof(char),
_get_zero_arr_size(_tmp72D,3));})))Cyc_Parse_err((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp730;_tmp730.tag=0;_tmp730.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{void*_tmp72E[1]={& _tmp730};Cyc_aprintf(({const char*
_tmp72F="bad occurrence of heap region %s";_tag_dynforward(_tmp72F,sizeof(char),
_get_zero_arr_size(_tmp72F,33));}),_tag_dynforward(_tmp72E,sizeof(void*),1));}}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B->name=({struct _dynforward_ptr*
_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp73E;});_tmp73B->identity=
0;_tmp73B->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp73C=_cycalloc(
sizeof(*_tmp73C));_tmp73C[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp73D;_tmp73D.tag=0;
_tmp73D.f1=(void*)((void*)3);_tmp73D;});_tmp73C;}));_tmp73B;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739[0]=({
struct Cyc_Absyn_VarType_struct _tmp73A;_tmp73A.tag=1;_tmp73A.f1=tv;_tmp73A;});
_tmp739;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp731;(
_tmp731.YY9).tag=13;(_tmp731.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Region_s_struct*
_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({struct Cyc_Absyn_Region_s_struct
_tmp733;_tmp733.tag=15;_tmp733.f1=tv;_tmp733.f2=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp736;(_tmp736.Loc_n).tag=0;_tmp736;});_tmp734->f2=({
struct _dynforward_ptr*_tmp735=_cycalloc(sizeof(*_tmp735));_tmp735[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp735;});_tmp734;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp737=_cycalloc(sizeof(*_tmp737));
_tmp737[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp738;_tmp738.tag=15;_tmp738.f1=(
void*)t;_tmp738;});_tmp737;}),0);_tmp733.f3=1;_tmp733.f4=0;_tmp733.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp733;});_tmp732;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp731;});break;}case 274: _LL312: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp73F="resetable";_tag_dynforward(_tmp73F,
sizeof(char),_get_zero_arr_size(_tmp73F,10));}))!= 0)Cyc_Parse_err(({const char*
_tmp740="expecting `resetable'";_tag_dynforward(_tmp740,sizeof(char),
_get_zero_arr_size(_tmp740,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp741="H";_tag_dynforward(_tmp741,sizeof(
char),_get_zero_arr_size(_tmp741,2));}))== 0)Cyc_Parse_err(({const char*_tmp742="bad occurrence of heap region `H";
_tag_dynforward(_tmp742,sizeof(char),_get_zero_arr_size(_tmp742,33));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->name=({
struct _dynforward_ptr*_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp753;_tmp753.tag=0;_tmp753.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp751[1]={&
_tmp753};Cyc_aprintf(({const char*_tmp752="`%s";_tag_dynforward(_tmp752,sizeof(
char),_get_zero_arr_size(_tmp752,4));}),_tag_dynforward(_tmp751,sizeof(void*),1));}});
_tmp750;});_tmp74D->identity=0;_tmp74D->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp74E=_cycalloc(sizeof(*_tmp74E));_tmp74E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp74F;_tmp74F.tag=0;_tmp74F.f1=(void*)((void*)3);_tmp74F;});_tmp74E;}));
_tmp74D;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp74B=_cycalloc(
sizeof(*_tmp74B));_tmp74B[0]=({struct Cyc_Absyn_VarType_struct _tmp74C;_tmp74C.tag=
1;_tmp74C.f1=tv;_tmp74C;});_tmp74B;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp743;(_tmp743.YY9).tag=13;(_tmp743.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744[0]=({
struct Cyc_Absyn_Region_s_struct _tmp745;_tmp745.tag=15;_tmp745.f1=tv;_tmp745.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp746=_cycalloc(sizeof(*_tmp746));
_tmp746->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp748;(
_tmp748.Loc_n).tag=0;_tmp748;});_tmp746->f2=({struct _dynforward_ptr*_tmp747=
_cycalloc(sizeof(*_tmp747));_tmp747[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp747;});_tmp746;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp749=_cycalloc(sizeof(*_tmp749));
_tmp749[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp74A;_tmp74A.tag=15;_tmp74A.f1=(
void*)t;_tmp74A;});_tmp749;}),0);_tmp745.f3=1;_tmp745.f4=0;_tmp745.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp745;});_tmp744;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp743;});break;}case 275: _LL313: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp754;(_tmp754.YY9).tag=13;(_tmp754.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp755=_cycalloc(sizeof(*_tmp755));
_tmp755[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp756;_tmp756.tag=16;_tmp756.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp756;});_tmp755;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp754;});
break;case 276: _LL314: {struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp75D=
_cycalloc(sizeof(*_tmp75D));_tmp75D->name=({struct _dynforward_ptr*_tmp760=
_cycalloc(sizeof(*_tmp760));_tmp760[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp760;});_tmp75D->identity=
0;_tmp75D->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp75E=_cycalloc(
sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp75F;_tmp75F.tag=0;
_tmp75F.f1=(void*)((void*)3);_tmp75F;});_tmp75E;}));_tmp75D;});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp757;(_tmp757.YY9).tag=13;(_tmp757.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Alias_s_struct*_tmp758=_cycalloc(sizeof(*_tmp758));
_tmp758[0]=({struct Cyc_Absyn_Alias_s_struct _tmp759;_tmp759.tag=17;_tmp759.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp759.f2=tv;
_tmp759.f3=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp75A=_cycalloc(sizeof(*
_tmp75A));_tmp75A->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp75C;(_tmp75C.Loc_n).tag=0;_tmp75C;});_tmp75A->f2=({struct _dynforward_ptr*
_tmp75B=_cycalloc(sizeof(*_tmp75B));_tmp75B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp75B;});_tmp75A;}),(
void*)0,0);_tmp759.f4=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp759;});_tmp758;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp757;});
break;}case 277: _LL315: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp761;(_tmp761.YY4).tag=8;(_tmp761.YY4).f1=0;_tmp761;});break;case 278: _LL316:
if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp762="open";
_tag_dynforward(_tmp762,sizeof(char),_get_zero_arr_size(_tmp762,5));}))!= 0)Cyc_Parse_err(({
const char*_tmp763="expecting `open'";_tag_dynforward(_tmp763,sizeof(char),
_get_zero_arr_size(_tmp763,17));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp764;(_tmp764.YY4).tag=8;(_tmp764.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp764;});break;case 279: _LL317: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp765;(_tmp765.YY9).tag=13;(_tmp765.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp766=_cycalloc(sizeof(*_tmp766));
_tmp766[0]=({struct Cyc_Absyn_Label_s_struct _tmp767;_tmp767.tag=12;_tmp767.f1=({
struct _dynforward_ptr*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp768;});_tmp767.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp767;});
_tmp766;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp765;});break;case 280: _LL318: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp769;(_tmp769.YY9).tag=13;(_tmp769.YY9).f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp769;});break;case 281: _LL319: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76A;(_tmp76A.YY9).tag=13;(_tmp76A.YY9).f1=Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76A;});
break;case 282: _LL31A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76B;(_tmp76B.YY9).tag=13;(_tmp76B.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76B;});
break;case 283: _LL31B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 284: _LL31C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76C;(_tmp76C.YY9).tag=13;(_tmp76C.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp76C;});break;case 285: _LL31D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76D;(_tmp76D.YY9).tag=13;(_tmp76D.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp76D;});break;case 286:
_LL31E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
287: _LL31F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp76E;(
_tmp76E.YY9).tag=13;(_tmp76E.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76E;});
break;case 288: _LL320: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76F;(_tmp76F.YY9).tag=13;(_tmp76F.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp771;_tmp771.tag=1;_tmp771.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp771;});_tmp770;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp76F;});break;case 289:
_LL321: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp772;(_tmp772.YY9).tag=
13;(_tmp772.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp774;_tmp774.tag=1;_tmp774.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp774;});_tmp773;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]));_tmp772;});break;case 290: _LL322: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp775;(_tmp775.YY9).tag=13;(_tmp775.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp775;});break;case 291: _LL323: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp776;(_tmp776.YY9).tag=13;(_tmp776.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp776;});
break;case 292: _LL324: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp777;(_tmp777.YY9).tag=13;(_tmp777.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp777;});
break;case 293: _LL325: {struct Cyc_Absyn_Exp*_tmp778=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp77B;_tmp77B.tag=2;_tmp77B.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp77B;});_tmp77A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp779;(_tmp779.YY9).tag=13;(_tmp779.YY9).f1=Cyc_Absyn_switch_stmt(_tmp778,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp779;});
break;}case 294: _LL326: {struct Cyc_Absyn_Exp*_tmp77C=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp77D;(_tmp77D.YY9).tag=13;(_tmp77D.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp77C,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp77D;});
break;}case 295: _LL327: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77E;(_tmp77E.YY9).tag=13;(_tmp77E.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp77E;});
break;case 296: _LL328: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77F;(_tmp77F.YY10).tag=14;(_tmp77F.YY10).f1=0;_tmp77F;});break;case 297:
_LL329: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp780;(_tmp780.YY10).tag=
14;(_tmp780.YY10).f1=({struct Cyc_List_List*_tmp781=_cycalloc(sizeof(*_tmp781));
_tmp781->hd=({struct Cyc_Absyn_Switch_clause*_tmp782=_cycalloc(sizeof(*_tmp782));
_tmp782->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp782->pat_vars=
0;_tmp782->where_clause=0;_tmp782->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp782->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp782;});
_tmp781->tl=0;_tmp781;});_tmp780;});break;case 298: _LL32A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp783;(_tmp783.YY10).tag=14;(_tmp783.YY10).f1=({struct
Cyc_List_List*_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784->hd=({struct Cyc_Absyn_Switch_clause*
_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp785->pat_vars=0;
_tmp785->where_clause=0;_tmp785->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp785->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp785;});_tmp784->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp784;});_tmp783;});
break;case 299: _LL32B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp786;(_tmp786.YY10).tag=14;(_tmp786.YY10).f1=({struct Cyc_List_List*_tmp787=
_cycalloc(sizeof(*_tmp787));_tmp787->hd=({struct Cyc_Absyn_Switch_clause*_tmp788=
_cycalloc(sizeof(*_tmp788));_tmp788->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp788->pat_vars=0;
_tmp788->where_clause=0;_tmp788->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp788->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp788;});
_tmp787->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp787;});_tmp786;});break;case 300: _LL32C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp789;(_tmp789.YY10).tag=14;(_tmp789.YY10).f1=({struct
Cyc_List_List*_tmp78A=_cycalloc(sizeof(*_tmp78A));_tmp78A->hd=({struct Cyc_Absyn_Switch_clause*
_tmp78B=_cycalloc(sizeof(*_tmp78B));_tmp78B->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp78B->pat_vars=0;
_tmp78B->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp78B->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp78B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp78B;});
_tmp78A->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp78A;});_tmp789;});break;case 301: _LL32D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp78C;(_tmp78C.YY10).tag=14;(_tmp78C.YY10).f1=({struct
Cyc_List_List*_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D->hd=({struct Cyc_Absyn_Switch_clause*
_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp78E->pat_vars=0;
_tmp78E->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp78E->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp78E->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp78E;});
_tmp78D->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp78D;});_tmp78C;});break;case 302: _LL32E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp78F;(_tmp78F.YY9).tag=13;(_tmp78F.YY9).f1=Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp78F;});
break;case 303: _LL32F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp790;(_tmp790.YY9).tag=13;(_tmp790.YY9).f1=Cyc_Absyn_do_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp790;});
break;case 304: _LL330: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp791;(_tmp791.YY9).tag=13;(_tmp791.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp791;});
break;case 305: _LL331: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp792;(_tmp792.YY9).tag=13;(_tmp792.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp792;});break;case 306: _LL332: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp793;(_tmp793.YY9).tag=13;(_tmp793.YY9).f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp793;});
break;case 307: _LL333: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp794;(_tmp794.YY9).tag=13;(_tmp794.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp794;});
break;case 308: _LL334: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp795;(_tmp795.YY9).tag=13;(_tmp795.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp795;});
break;case 309: _LL335: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp796;(_tmp796.YY9).tag=13;(_tmp796.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp796;});
break;case 310: _LL336: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp797;(_tmp797.YY9).tag=13;(_tmp797.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp797;});
break;case 311: _LL337: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp798;(_tmp798.YY9).tag=13;(_tmp798.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp798;});
break;case 312: _LL338: {struct Cyc_List_List*_tmp799=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp79A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79B;(_tmp79B.YY9).tag=13;(_tmp79B.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp799,_tmp79A);_tmp79B;});break;}case 313: _LL339: {struct Cyc_List_List*_tmp79C=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp79D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79E;(_tmp79E.YY9).tag=13;(_tmp79E.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp79C,_tmp79D);_tmp79E;});break;}case 314: _LL33A: {struct Cyc_List_List*_tmp79F=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp7A0=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7A1;(_tmp7A1.YY9).tag=13;(_tmp7A1.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp79F,_tmp7A0);_tmp7A1;});break;}case 315: _LL33B: {struct Cyc_List_List*_tmp7A2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
struct Cyc_Absyn_Stmt*_tmp7A3=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7A4;(_tmp7A4.YY9).tag=13;(_tmp7A4.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp7A2,_tmp7A3);_tmp7A4;});break;}case 316: _LL33C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7A5;(_tmp7A5.YY9).tag=13;(_tmp7A5.YY9).f1=Cyc_Absyn_goto_stmt(({
struct _dynforward_ptr*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7A5;});
break;case 317: _LL33D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A7;(_tmp7A7.YY9).tag=13;(_tmp7A7.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7A7;});
break;case 318: _LL33E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A8;(_tmp7A8.YY9).tag=13;(_tmp7A8.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7A8;});
break;case 319: _LL33F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A9;(_tmp7A9.YY9).tag=13;(_tmp7A9.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7A9;});
break;case 320: _LL340: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AA;(_tmp7AA.YY9).tag=13;(_tmp7AA.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7AA;});
break;case 321: _LL341: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AB;(_tmp7AB.YY9).tag=13;(_tmp7AB.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp7AB;});
break;case 322: _LL342: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AC;(_tmp7AC.YY9).tag=13;(_tmp7AC.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp7AC;});
break;case 323: _LL343: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AD;(_tmp7AD.YY9).tag=13;(_tmp7AD.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp7AD;});
break;case 324: _LL344: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL345: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 326: _LL346: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7AE;(_tmp7AE.YY11).tag=15;(_tmp7AE.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7AE;});
break;case 327: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 328: _LL348: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7AF;(_tmp7AF.YY11).tag=15;(_tmp7AF.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7AF;});break;case 329: _LL349: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 330: _LL34A: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B0;(_tmp7B0.YY11).tag=15;(
_tmp7B0.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B0;});
break;case 331: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 332: _LL34C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B1;(_tmp7B1.YY11).tag=15;(_tmp7B1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B1;});
break;case 333: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 334: _LL34E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B2;(_tmp7B2.YY11).tag=15;(_tmp7B2.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B2;});
break;case 335: _LL34F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 336: _LL350: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B3;(_tmp7B3.YY11).tag=15;(_tmp7B3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B3;});
break;case 337: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 338: _LL352: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B4;(_tmp7B4.YY11).tag=15;(_tmp7B4.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B4;});break;case 339: _LL353: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7B5;(_tmp7B5.YY11).tag=15;(_tmp7B5.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B5;});
break;case 340: _LL354: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 341: _LL355: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B6;(_tmp7B6.YY11).tag=15;(_tmp7B6.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B6;});break;case 342: _LL356: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7B7;(_tmp7B7.YY11).tag=15;(_tmp7B7.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B7;});
break;case 343: _LL357: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B8;(_tmp7B8.YY11).tag=15;(_tmp7B8.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7B8;});break;case 344: _LL358: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7B9;(_tmp7B9.YY11).tag=15;(_tmp7B9.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7B9;});
break;case 345: _LL359: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL35A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7BA;(_tmp7BA.YY11).tag=15;(_tmp7BA.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7BA;});
break;case 347: _LL35B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7BB;(_tmp7BB.YY11).tag=15;(_tmp7BB.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7BB;});
break;case 348: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL35D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7BC;(_tmp7BC.YY11).tag=15;(_tmp7BC.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7BC;});
break;case 350: _LL35E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7BD;(_tmp7BD.YY11).tag=15;(_tmp7BD.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7BD;});
break;case 351: _LL35F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 352: _LL360: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7BE;(_tmp7BE.YY11).tag=15;(_tmp7BE.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7BE;});
break;case 353: _LL361: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7BF;(_tmp7BF.YY11).tag=15;(_tmp7BF.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7BF;});
break;case 354: _LL362: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7C0;(_tmp7C0.YY11).tag=15;(_tmp7C0.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7C0;});
break;case 355: _LL363: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 356: _LL364: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7C1;(_tmp7C1.YY11).tag=15;(_tmp7C1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7C1;});break;case 357: _LL365: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 358: _LL366: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C2;(_tmp7C2.YY11).tag=15;(
_tmp7C2.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp7C2;});break;case 359:
_LL367: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C3;(_tmp7C3.YY11).tag=
15;(_tmp7C3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7C3;});
break;case 360: _LL368: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7C4;(_tmp7C4.YY11).tag=15;(_tmp7C4.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7C4;});break;case 361: _LL369: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7C5;(_tmp7C5.YY11).tag=15;(_tmp7C5.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7C5;});break;case 362: _LL36A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7C6;(_tmp7C6.YY11).tag=15;(_tmp7C6.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp7C7=
_cycalloc(sizeof(*_tmp7C7));_tmp7C7[0]=({struct Cyc_Absyn_StructField_struct
_tmp7C8;_tmp7C8.tag=0;_tmp7C8.f1=({struct _dynforward_ptr*_tmp7C9=_cycalloc(
sizeof(*_tmp7C9));_tmp7C9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7C9;});_tmp7C8;});
_tmp7C7;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7C6;});break;case 363: _LL36B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7CA;(_tmp7CA.YY11).tag=15;(_tmp7CA.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7CB=
_cycalloc_atomic(sizeof(*_tmp7CB));_tmp7CB[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp7CC;_tmp7CC.tag=1;_tmp7CC.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7CC;});_tmp7CB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7CA;});break;case 364: _LL36C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 365: _LL36D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 366: _LL36E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7CD;(_tmp7CD.YY11).tag=
15;(_tmp7CD.YY11).f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CD;});
break;case 367: _LL36F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7CE;(_tmp7CE.YY11).tag=15;(_tmp7CE.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7CE;});break;
case 368: _LL370: {struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp7CF=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp7D0;void*_tmp7D1;char _tmp7D2;union Cyc_Absyn_Cnst_union
_tmp7D3;void*_tmp7D4;short _tmp7D5;union Cyc_Absyn_Cnst_union _tmp7D6;void*_tmp7D7;
int _tmp7D8;union Cyc_Absyn_Cnst_union _tmp7D9;struct _dynforward_ptr _tmp7DA;union
Cyc_Absyn_Cnst_union _tmp7DB;union Cyc_Absyn_Cnst_union _tmp7DC;union Cyc_Absyn_Cnst_union
_tmp7DD;_LL373: if(*((int*)_tmp7CF)!= 0)goto _LL375;_tmp7D0=((struct Cyc_Absyn_Const_e_struct*)
_tmp7CF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7CF)->f1).Char_c).tag != 
0)goto _LL375;_tmp7D1=(_tmp7D0.Char_c).f1;_tmp7D2=(_tmp7D0.Char_c).f2;_LL374:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7DE;(_tmp7DE.YY11).tag=
15;(_tmp7DE.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp7DF=_cycalloc_atomic(sizeof(*_tmp7DF));_tmp7DF[0]=({struct Cyc_Absyn_Char_p_struct
_tmp7E0;_tmp7E0.tag=8;_tmp7E0.f1=_tmp7D2;_tmp7E0;});_tmp7DF;}),e->loc);_tmp7DE;});
goto _LL372;_LL375: if(*((int*)_tmp7CF)!= 0)goto _LL377;_tmp7D3=((struct Cyc_Absyn_Const_e_struct*)
_tmp7CF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7CF)->f1).Short_c).tag != 
1)goto _LL377;_tmp7D4=(_tmp7D3.Short_c).f1;_tmp7D5=(_tmp7D3.Short_c).f2;_LL376:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7E1;(_tmp7E1.YY11).tag=
15;(_tmp7E1.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=({struct Cyc_Absyn_Int_p_struct
_tmp7E3;_tmp7E3.tag=7;_tmp7E3.f1=(void*)_tmp7D4;_tmp7E3.f2=(int)_tmp7D5;_tmp7E3;});
_tmp7E2;}),e->loc);_tmp7E1;});goto _LL372;_LL377: if(*((int*)_tmp7CF)!= 0)goto
_LL379;_tmp7D6=((struct Cyc_Absyn_Const_e_struct*)_tmp7CF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7CF)->f1).Int_c).tag != 2)goto _LL379;_tmp7D7=(_tmp7D6.Int_c).f1;_tmp7D8=(
_tmp7D6.Int_c).f2;_LL378: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7E4;(_tmp7E4.YY11).tag=15;(_tmp7E4.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5[0]=({
struct Cyc_Absyn_Int_p_struct _tmp7E6;_tmp7E6.tag=7;_tmp7E6.f1=(void*)_tmp7D7;
_tmp7E6.f2=_tmp7D8;_tmp7E6;});_tmp7E5;}),e->loc);_tmp7E4;});goto _LL372;_LL379:
if(*((int*)_tmp7CF)!= 0)goto _LL37B;_tmp7D9=((struct Cyc_Absyn_Const_e_struct*)
_tmp7CF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7CF)->f1).Float_c).tag != 
4)goto _LL37B;_tmp7DA=(_tmp7D9.Float_c).f1;_LL37A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7E7;(_tmp7E7.YY11).tag=15;(_tmp7E7.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Float_p_struct*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));
_tmp7E8[0]=({struct Cyc_Absyn_Float_p_struct _tmp7E9;_tmp7E9.tag=9;_tmp7E9.f1=
_tmp7DA;_tmp7E9;});_tmp7E8;}),e->loc);_tmp7E7;});goto _LL372;_LL37B: if(*((int*)
_tmp7CF)!= 0)goto _LL37D;_tmp7DB=((struct Cyc_Absyn_Const_e_struct*)_tmp7CF)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7CF)->f1).Null_c).tag != 6)goto _LL37D;
_LL37C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7EA;(_tmp7EA.YY11).tag=
15;(_tmp7EA.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp7EA;});goto _LL372;
_LL37D: if(*((int*)_tmp7CF)!= 0)goto _LL37F;_tmp7DC=((struct Cyc_Absyn_Const_e_struct*)
_tmp7CF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7CF)->f1).String_c).tag
!= 5)goto _LL37F;_LL37E: Cyc_Parse_err(({const char*_tmp7EB="strings cannot occur within patterns";
_tag_dynforward(_tmp7EB,sizeof(char),_get_zero_arr_size(_tmp7EB,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL372;
_LL37F: if(*((int*)_tmp7CF)!= 0)goto _LL381;_tmp7DD=((struct Cyc_Absyn_Const_e_struct*)
_tmp7CF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7CF)->f1).LongLong_c).tag
!= 3)goto _LL381;_LL380: Cyc_Parse_unimp(({const char*_tmp7EC="long long's in patterns";
_tag_dynforward(_tmp7EC,sizeof(char),_get_zero_arr_size(_tmp7EC,24));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL372;
_LL381:;_LL382: Cyc_Parse_err(({const char*_tmp7ED="bad constant in case";
_tag_dynforward(_tmp7ED,sizeof(char),_get_zero_arr_size(_tmp7ED,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL372:;}break;}
case 369: _LL371: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7EE;(
_tmp7EE.YY11).tag=15;(_tmp7EE.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp7F0;_tmp7F0.tag=12;_tmp7F0.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7F0;});_tmp7EF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EE;});
break;case 370: _LL383: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7F1="as";
_tag_dynforward(_tmp7F1,sizeof(char),_get_zero_arr_size(_tmp7F1,3));}))!= 0)Cyc_Parse_err(({
const char*_tmp7F2="expecting `as'";_tag_dynforward(_tmp7F2,sizeof(char),
_get_zero_arr_size(_tmp7F2,15));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7F3;(_tmp7F3.YY11).tag=15;(_tmp7F3.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp7F4=_cycalloc(
sizeof(*_tmp7F4));_tmp7F4[0]=({struct Cyc_Absyn_Var_p_struct _tmp7F5;_tmp7F5.tag=0;
_tmp7F5.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7F6=_cycalloc(sizeof(*
_tmp7F6));_tmp7F6->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7F8;(_tmp7F8.Loc_n).tag=0;_tmp7F8;});_tmp7F6->f2=({struct _dynforward_ptr*
_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7F7;});_tmp7F6;}),(
void*)0,0);_tmp7F5.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7F5;});_tmp7F4;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7F3;});
break;case 371: _LL384: {struct Cyc_List_List*_tmp7FA;int _tmp7FB;struct _tuple13
_tmp7F9=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7FA=_tmp7F9.f1;_tmp7FB=_tmp7F9.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7FC;(_tmp7FC.YY11).tag=15;(_tmp7FC.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));
_tmp7FD[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7FE;_tmp7FE.tag=3;_tmp7FE.f1=
_tmp7FA;_tmp7FE.f2=_tmp7FB;_tmp7FE;});_tmp7FD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FC;});
break;}case 372: _LL385: {struct Cyc_List_List*_tmp800;int _tmp801;struct _tuple13
_tmp7FF=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp800=_tmp7FF.f1;_tmp801=_tmp7FF.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp802;(_tmp802.YY11).tag=15;(_tmp802.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp803=_cycalloc(sizeof(*_tmp803));
_tmp803[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp804;_tmp804.tag=13;_tmp804.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp804.f2=_tmp800;_tmp804.f3=_tmp801;_tmp804;});_tmp803;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp802;});
break;}case 373: _LL386: {struct Cyc_List_List*_tmp806;int _tmp807;struct _tuple13
_tmp805=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp806=_tmp805.f1;_tmp807=_tmp805.f2;{struct Cyc_List_List*_tmp808=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp809;(_tmp809.YY11).tag=15;(_tmp809.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp80B;_tmp80B.tag=5;_tmp80B.f1=({struct Cyc_Absyn_AggrInfo
_tmp80C;_tmp80C.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp80D;(_tmp80D.UnknownAggr).tag=0;(_tmp80D.UnknownAggr).f1=(void*)((void*)0);(
_tmp80D.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp80D;});_tmp80C.targs=0;_tmp80C;});_tmp80B.f2=
_tmp808;_tmp80B.f3=_tmp806;_tmp80B.f4=_tmp807;_tmp80B;});_tmp80A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp809;});
break;}}case 374: _LL387: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp80E;(_tmp80E.YY11).tag=15;(_tmp80E.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Pointer_p_struct*_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp810;_tmp810.tag=4;_tmp810.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp810;});_tmp80F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp80E;});break;case 375: _LL388: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp811;(_tmp811.YY11).tag=15;(_tmp811.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp812=_cycalloc(sizeof(*_tmp812));
_tmp812[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp813;_tmp813.tag=4;_tmp813.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp814=_cycalloc(
sizeof(*_tmp814));_tmp814[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp815;_tmp815.tag=
4;_tmp815.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp815;});_tmp814;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp813;});
_tmp812;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp811;});break;case 376: _LL389: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp816;(_tmp816.YY11).tag=15;(_tmp816.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp817=_cycalloc(sizeof(*_tmp817));
_tmp817[0]=({struct Cyc_Absyn_Reference_p_struct _tmp818;_tmp818.tag=1;_tmp818.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp819=_cycalloc(sizeof(*_tmp819));
_tmp819->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp81B;(
_tmp81B.Loc_n).tag=0;_tmp81B;});_tmp819->f2=({struct _dynforward_ptr*_tmp81A=
_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81A;});_tmp819;}),(void*)
0,0);_tmp818.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp818;});
_tmp817;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp816;});break;case 377: _LL38A: if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp81C="as";_tag_dynforward(_tmp81C,
sizeof(char),_get_zero_arr_size(_tmp81C,3));}))!= 0)Cyc_Parse_err(({const char*
_tmp81D="expecting `as'";_tag_dynforward(_tmp81D,sizeof(char),_get_zero_arr_size(
_tmp81D,15));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81E;(_tmp81E.YY11).tag=15;(_tmp81E.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Reference_p_struct*_tmp81F=_cycalloc(sizeof(*_tmp81F));_tmp81F[0]=({
struct Cyc_Absyn_Reference_p_struct _tmp820;_tmp820.tag=1;_tmp820.f1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp821=_cycalloc(sizeof(*_tmp821));_tmp821->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp823;(_tmp823.Loc_n).tag=0;_tmp823;});_tmp821->f2=({
struct _dynforward_ptr*_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp822;});_tmp821;}),(
void*)0,0);_tmp820.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp820;});_tmp81F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp81E;});
break;case 378: _LL38B: {void*_tmp824=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp82E;_tmp82E.tag=0;_tmp82E.f1=(void*)((void*)7);_tmp82E;});_tmp82D;}));yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp825;(_tmp825.YY11).tag=15;(
_tmp825.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp827;_tmp827.tag=2;_tmp827.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp824);
_tmp827.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp828=_cycalloc(sizeof(*
_tmp828));_tmp828->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp82A;(_tmp82A.Loc_n).tag=0;_tmp82A;});_tmp828->f2=({struct _dynforward_ptr*
_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp829;});_tmp828;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp82B=_cycalloc(sizeof(*_tmp82B));
_tmp82B[0]=({struct Cyc_Absyn_TagType_struct _tmp82C;_tmp82C.tag=18;_tmp82C.f1=(
void*)_tmp824;_tmp82C;});_tmp82B;}),0);_tmp827;});_tmp826;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp825;});
break;}case 379: _LL38C: {struct Cyc_Absyn_Tvar*_tmp82F=Cyc_Tcutil_new_tvar((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp83B;_tmp83B.tag=0;_tmp83B.f1=(void*)((void*)7);
_tmp83B;});_tmp83A;}));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp830;(_tmp830.YY11).tag=15;(_tmp830.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_TagInt_p_struct*_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831[0]=({
struct Cyc_Absyn_TagInt_p_struct _tmp832;_tmp832.tag=2;_tmp832.f1=_tmp82F;_tmp832.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp833=_cycalloc(sizeof(*_tmp833));
_tmp833->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp835;(
_tmp835.Loc_n).tag=0;_tmp835;});_tmp833->f2=({struct _dynforward_ptr*_tmp834=
_cycalloc(sizeof(*_tmp834));_tmp834[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp834;});_tmp833;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp836=_cycalloc(sizeof(*_tmp836));
_tmp836[0]=({struct Cyc_Absyn_TagType_struct _tmp837;_tmp837.tag=18;_tmp837.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp838=_cycalloc(sizeof(*_tmp838));
_tmp838[0]=({struct Cyc_Absyn_VarType_struct _tmp839;_tmp839.tag=1;_tmp839.f1=
_tmp82F;_tmp839;});_tmp838;}));_tmp837;});_tmp836;}),0);_tmp832;});_tmp831;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp830;});break;}case 380: _LL38D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp83C;(_tmp83C.YY12).tag=16;(_tmp83C.YY12).f1=({struct
_tuple13*_tmp83D=_cycalloc(sizeof(*_tmp83D));_tmp83D->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp83D->f2=0;_tmp83D;});
_tmp83C;});break;case 381: _LL38E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83E;(_tmp83E.YY12).tag=16;(_tmp83E.YY12).f1=({struct _tuple13*_tmp83F=
_cycalloc(sizeof(*_tmp83F));_tmp83F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp83F->f2=1;_tmp83F;});_tmp83E;});break;case 382: _LL38F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp840;(_tmp840.YY12).tag=
16;(_tmp840.YY12).f1=({struct _tuple13*_tmp841=_cycalloc(sizeof(*_tmp841));
_tmp841->f1=0;_tmp841->f2=1;_tmp841;});_tmp840;});break;case 383: _LL390: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp842;(_tmp842.YY13).tag=17;(
_tmp842.YY13).f1=({struct Cyc_List_List*_tmp843=_cycalloc(sizeof(*_tmp843));
_tmp843->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp843->tl=0;_tmp843;});_tmp842;});break;case 384: _LL391: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp844;(_tmp844.YY13).tag=17;(
_tmp844.YY13).f1=({struct Cyc_List_List*_tmp845=_cycalloc(sizeof(*_tmp845));
_tmp845->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp845->tl=Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp845;});_tmp844;});break;case 385: _LL392: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp846;(_tmp846.YY14).tag=18;(_tmp846.YY14).f1=({
struct _tuple14*_tmp847=_cycalloc(sizeof(*_tmp847));_tmp847->f1=0;_tmp847->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp847;});_tmp846;});
break;case 386: _LL393: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp848;(_tmp848.YY14).tag=18;(_tmp848.YY14).f1=({struct _tuple14*_tmp849=
_cycalloc(sizeof(*_tmp849));_tmp849->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp849->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp849;});_tmp848;});
break;case 387: _LL394: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84A;(_tmp84A.YY16).tag=20;(_tmp84A.YY16).f1=({struct _tuple13*_tmp84B=
_cycalloc(sizeof(*_tmp84B));_tmp84B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp84B->f2=0;_tmp84B;});_tmp84A;});break;case 388: _LL395: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp84C;(_tmp84C.YY16).tag=20;(
_tmp84C.YY16).f1=({struct _tuple13*_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp84D->f2=1;_tmp84D;});
_tmp84C;});break;case 389: _LL396: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84E;(_tmp84E.YY16).tag=20;(_tmp84E.YY16).f1=({struct _tuple13*_tmp84F=
_cycalloc(sizeof(*_tmp84F));_tmp84F->f1=0;_tmp84F->f2=1;_tmp84F;});_tmp84E;});
break;case 390: _LL397: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp850;(_tmp850.YY15).tag=19;(_tmp850.YY15).f1=({struct Cyc_List_List*_tmp851=
_cycalloc(sizeof(*_tmp851));_tmp851->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp851->tl=0;_tmp851;});
_tmp850;});break;case 391: _LL398: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp852;(_tmp852.YY15).tag=19;(_tmp852.YY15).f1=({struct Cyc_List_List*_tmp853=
_cycalloc(sizeof(*_tmp853));_tmp853->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp853->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp853;});_tmp852;});
break;case 392: _LL399: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 393: _LL39A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp854;(_tmp854.YY3).tag=7;(_tmp854.YY3).f1=Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp854;});
break;case 394: _LL39B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 395: _LL39C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp855;(_tmp855.YY3).tag=7;(_tmp855.YY3).f1=Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
break;case 396: _LL39D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp856;(_tmp856.YY8).tag=12;(_tmp856.YY8).f1=0;_tmp856;});break;case 397: _LL39E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp857;(_tmp857.YY8).tag=
12;(_tmp857.YY8).f1=({struct Cyc_Core_Opt*_tmp858=_cycalloc(sizeof(*_tmp858));
_tmp858->v=(void*)((void*)1);_tmp858;});_tmp857;});break;case 398: _LL39F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp859;(_tmp859.YY8).tag=12;(
_tmp859.YY8).f1=({struct Cyc_Core_Opt*_tmp85A=_cycalloc(sizeof(*_tmp85A));_tmp85A->v=(
void*)((void*)3);_tmp85A;});_tmp859;});break;case 399: _LL3A0: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp85B;(_tmp85B.YY8).tag=12;(_tmp85B.YY8).f1=({struct Cyc_Core_Opt*
_tmp85C=_cycalloc(sizeof(*_tmp85C));_tmp85C->v=(void*)((void*)4);_tmp85C;});
_tmp85B;});break;case 400: _LL3A1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85D;(_tmp85D.YY8).tag=12;(_tmp85D.YY8).f1=({struct Cyc_Core_Opt*_tmp85E=
_cycalloc(sizeof(*_tmp85E));_tmp85E->v=(void*)((void*)0);_tmp85E;});_tmp85D;});
break;case 401: _LL3A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85F;(_tmp85F.YY8).tag=12;(_tmp85F.YY8).f1=({struct Cyc_Core_Opt*_tmp860=
_cycalloc(sizeof(*_tmp860));_tmp860->v=(void*)((void*)2);_tmp860;});_tmp85F;});
break;case 402: _LL3A3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp861;(_tmp861.YY8).tag=12;(_tmp861.YY8).f1=({struct Cyc_Core_Opt*_tmp862=
_cycalloc(sizeof(*_tmp862));_tmp862->v=(void*)((void*)16);_tmp862;});_tmp861;});
break;case 403: _LL3A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp863;(_tmp863.YY8).tag=12;(_tmp863.YY8).f1=({struct Cyc_Core_Opt*_tmp864=
_cycalloc(sizeof(*_tmp864));_tmp864->v=(void*)((void*)17);_tmp864;});_tmp863;});
break;case 404: _LL3A5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp865;(_tmp865.YY8).tag=12;(_tmp865.YY8).f1=({struct Cyc_Core_Opt*_tmp866=
_cycalloc(sizeof(*_tmp866));_tmp866->v=(void*)((void*)13);_tmp866;});_tmp865;});
break;case 405: _LL3A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp867;(_tmp867.YY8).tag=12;(_tmp867.YY8).f1=({struct Cyc_Core_Opt*_tmp868=
_cycalloc(sizeof(*_tmp868));_tmp868->v=(void*)((void*)15);_tmp868;});_tmp867;});
break;case 406: _LL3A7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp869;(_tmp869.YY8).tag=12;(_tmp869.YY8).f1=({struct Cyc_Core_Opt*_tmp86A=
_cycalloc(sizeof(*_tmp86A));_tmp86A->v=(void*)((void*)14);_tmp86A;});_tmp869;});
break;case 407: _LL3A8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 408: _LL3A9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86B;(_tmp86B.YY3).tag=7;(_tmp86B.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86B;});
break;case 409: _LL3AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86C;(_tmp86C.YY3).tag=7;(_tmp86C.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86C;});
break;case 410: _LL3AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86D;(_tmp86D.YY3).tag=7;(_tmp86D.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86D;});
break;case 411: _LL3AC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86E;(_tmp86E.YY3).tag=7;(_tmp86E.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86E;});
break;case 412: _LL3AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86F;(_tmp86F.YY3).tag=7;(_tmp86F.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86F;});
break;case 413: _LL3AE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp870;(_tmp870.YY3).tag=7;(_tmp870.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp870;});
break;case 414: _LL3AF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 415: _LL3B0: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 416: _LL3B1: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp871;(_tmp871.YY3).tag=7;(_tmp871.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp871;});
break;case 417: _LL3B2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 418: _LL3B3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp872;(_tmp872.YY3).tag=7;(_tmp872.YY3).f1=Cyc_Absyn_and_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp872;});
break;case 419: _LL3B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 420: _LL3B5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp873;(_tmp873.YY3).tag=7;(_tmp873.YY3).f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp873;});
break;case 421: _LL3B6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 422: _LL3B7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp874;(_tmp874.YY3).tag=7;(_tmp874.YY3).f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp874;});
break;case 423: _LL3B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp875;(_tmp875.YY3).tag=7;(_tmp875.YY3).f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp875;});
break;case 425: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 426: _LL3BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp876;(_tmp876.YY3).tag=7;(_tmp876.YY3).f1=Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp876;});
break;case 427: _LL3BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp877;(_tmp877.YY3).tag=7;(_tmp877.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp877;});
break;case 428: _LL3BD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 429: _LL3BE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp878;(_tmp878.YY3).tag=7;(_tmp878.YY3).f1=Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp878;});
break;case 430: _LL3BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp879;(_tmp879.YY3).tag=7;(_tmp879.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp879;});
break;case 431: _LL3C0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87A;(_tmp87A.YY3).tag=7;(_tmp87A.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87A;});
break;case 432: _LL3C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87B;(_tmp87B.YY3).tag=7;(_tmp87B.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87B;});
break;case 433: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 434: _LL3C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87C;(_tmp87C.YY3).tag=7;(_tmp87C.YY3).f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87C;});
break;case 435: _LL3C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87D;(_tmp87D.YY3).tag=7;(_tmp87D.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87D;});
break;case 436: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 437: _LL3C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87E;(_tmp87E.YY3).tag=7;(_tmp87E.YY3).f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87E;});
break;case 438: _LL3C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87F;(_tmp87F.YY3).tag=7;(_tmp87F.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87F;});
break;case 439: _LL3C8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 440: _LL3C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp880;(_tmp880.YY3).tag=7;(_tmp880.YY3).f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp880;});
break;case 441: _LL3CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp881;(_tmp881.YY3).tag=7;(_tmp881.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp881;});
break;case 442: _LL3CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp882;(_tmp882.YY3).tag=7;(_tmp882.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp882;});
break;case 443: _LL3CC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 444: _LL3CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp883;(_tmp883.YY3).tag=7;(_tmp883.YY3).f1=Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp883;});
break;case 445: _LL3CE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 446: _LL3CF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp884;(_tmp884.YY3).tag=7;(_tmp884.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp884;});
break;case 447: _LL3D0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp885;(_tmp885.YY3).tag=7;(_tmp885.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp885;});
break;case 448: _LL3D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp886;(_tmp886.YY3).tag=7;(_tmp886.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp886;});
break;case 449: _LL3D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp887;(_tmp887.YY3).tag=7;(_tmp887.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp887;});
break;case 450: _LL3D3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 451: _LL3D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp888;(_tmp888.YY3).tag=7;(_tmp888.YY3).f1=Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp888;});
break;case 452: _LL3D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp889;(_tmp889.YY3).tag=7;(_tmp889.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp889;});
break;case 453: _LL3D6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88A;(_tmp88A.YY3).tag=7;(_tmp88A.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88A;});
break;case 454: _LL3D7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88B;(_tmp88B.YY3).tag=7;(_tmp88B.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp88C=_cycalloc(sizeof(*_tmp88C));_tmp88C[0]=({
struct Cyc_Absyn_StructField_struct _tmp88D;_tmp88D.tag=0;_tmp88D.f1=({struct
_dynforward_ptr*_tmp88E=_cycalloc(sizeof(*_tmp88E));_tmp88E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp88E;});_tmp88D;});
_tmp88C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp88B;});break;case 455: _LL3D8: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp88F;(_tmp88F.YY3).tag=7;(_tmp88F.YY3).f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp890=_cycalloc_atomic(sizeof(*
_tmp890));_tmp890[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp891;_tmp891.tag=1;
_tmp891.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp891;});_tmp890;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88F;});
break;case 456: _LL3D9: {struct Cyc_Position_Segment*_tmp892=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp893=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp892,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp894;(_tmp894.YY3).tag=
7;(_tmp894.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp893,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp894;});
break;}case 457: _LL3DA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp895;(_tmp895.YY3).tag=7;(_tmp895.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp897;_tmp897.tag=35;_tmp897.f1=({struct Cyc_Absyn_MallocInfo
_tmp898;_tmp898.is_calloc=0;_tmp898.rgn=0;_tmp898.elt_type=0;_tmp898.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp898.fat_result=0;_tmp898;});_tmp897;});_tmp896;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp895;});
break;case 458: _LL3DB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp899;(_tmp899.YY3).tag=7;(_tmp899.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp89B;_tmp89B.tag=35;_tmp89B.f1=({struct Cyc_Absyn_MallocInfo
_tmp89C;_tmp89C.is_calloc=0;_tmp89C.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp89C.elt_type=0;
_tmp89C.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp89C.fat_result=0;_tmp89C;});_tmp89B;});_tmp89A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp899;});
break;case 459: _LL3DC: {void*_tmp89E;struct _tuple2 _tmp89D=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp89E=_tmp89D.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp89F;(_tmp89F.YY3).tag=7;(
_tmp89F.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp8A1;_tmp8A1.tag=35;_tmp8A1.f1=({struct Cyc_Absyn_MallocInfo _tmp8A2;_tmp8A2.is_calloc=
1;_tmp8A2.rgn=0;_tmp8A2.elt_type=({void**_tmp8A3=_cycalloc(sizeof(*_tmp8A3));
_tmp8A3[0]=_tmp89E;_tmp8A3;});_tmp8A2.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp8A2.fat_result=0;
_tmp8A2;});_tmp8A1;});_tmp8A0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89F;});
break;}case 460: _LL3DD: {void*_tmp8A5;struct _tuple2 _tmp8A4=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8A5=_tmp8A4.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8A6;(_tmp8A6.YY3).tag=7;(
_tmp8A6.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp8A8;_tmp8A8.tag=35;_tmp8A8.f1=({struct Cyc_Absyn_MallocInfo _tmp8A9;_tmp8A9.is_calloc=
1;_tmp8A9.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp8A9.elt_type=({void**
_tmp8AA=_cycalloc(sizeof(*_tmp8AA));_tmp8AA[0]=_tmp8A5;_tmp8AA;});_tmp8A9.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp8A9.fat_result=0;_tmp8A9;});_tmp8A8;});_tmp8A7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A6;});
break;}case 461: _LL3DE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AB;(_tmp8AB.YY3).tag=7;(_tmp8AB.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Swap_e_struct*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC[0]=({struct
Cyc_Absyn_Swap_e_struct _tmp8AD;_tmp8AD.tag=36;_tmp8AD.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp8AD.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp8AD;});_tmp8AC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8AB;});break;case 462: _LL3DF: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8AE;(_tmp8AE.YY7).tag=11;(_tmp8AE.YY7).f1=(void*)((
void*)12);_tmp8AE;});break;case 463: _LL3E0: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp8AF;(_tmp8AF.YY7).tag=11;(_tmp8AF.YY7).f1=(void*)((void*)11);
_tmp8AF;});break;case 464: _LL3E1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B0;(_tmp8B0.YY7).tag=11;(_tmp8B0.YY7).f1=(void*)((void*)2);_tmp8B0;});
break;case 465: _LL3E2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 466: _LL3E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B1;(_tmp8B1.YY3).tag=7;(_tmp8B1.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B1;});
break;case 467: _LL3E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B2;(_tmp8B2.YY3).tag=7;(_tmp8B2.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B2;});
break;case 468: _LL3E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B3;(_tmp8B3.YY3).tag=7;(_tmp8B3.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B3;});
break;case 469: _LL3E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B4;(_tmp8B4.YY3).tag=7;(_tmp8B4.YY3).f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp8B5=_cycalloc(sizeof(*_tmp8B5));_tmp8B5[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8B5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B4;});
break;case 470: _LL3E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B6;(_tmp8B6.YY3).tag=7;(_tmp8B6.YY3).f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp8B7=_cycalloc(sizeof(*_tmp8B7));_tmp8B7[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8B7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B6;});
break;case 471: _LL3E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B8;(_tmp8B8.YY3).tag=7;(_tmp8B8.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B8;});
break;case 472: _LL3E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B9;(_tmp8B9.YY3).tag=7;(_tmp8B9.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B9;});
break;case 473: _LL3EA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8BA;(_tmp8BA.YY3).tag=7;(_tmp8BA.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_CompoundLit_e_struct*_tmp8BB=_cycalloc(sizeof(*_tmp8BB));_tmp8BB[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp8BC;_tmp8BC.tag=27;_tmp8BC.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8BC.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8BC;});_tmp8BB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8BA;});break;case 474: _LL3EB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8BD;(_tmp8BD.YY3).tag=7;(_tmp8BD.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp8BE=_cycalloc(sizeof(*_tmp8BE));
_tmp8BE[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp8BF;_tmp8BF.tag=27;_tmp8BF.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp8BF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp8BF;});_tmp8BE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8BD;});
break;case 475: _LL3EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C0;(_tmp8C0.YY3).tag=7;(_tmp8C0.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp8C1=_cycalloc(sizeof(*_tmp8C1));_tmp8C1[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp8C2;_tmp8C2.tag=2;_tmp8C2.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8C2;});_tmp8C1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8C0;});break;case 476: _LL3ED: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 477: _LL3EE: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8C3;(_tmp8C3.YY3).tag=7;(
_tmp8C3.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C3;});
break;case 478: _LL3EF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 479: _LL3F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C4;(_tmp8C4.YY3).tag=7;(_tmp8C4.YY3).f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C4;});
break;case 480: _LL3F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C5;(_tmp8C5.YY3).tag=7;(_tmp8C5.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C5;});
break;case 481: _LL3F2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C6;(_tmp8C6.YY3).tag=7;(_tmp8C6.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8C6;});
break;case 482: _LL3F3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8C7;(_tmp8C7.YY3).tag=7;(_tmp8C7.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Struct_e_struct*_tmp8C8=_cycalloc(sizeof(*_tmp8C8));_tmp8C8[0]=({
struct Cyc_Absyn_Struct_e_struct _tmp8C9;_tmp8C9.tag=30;_tmp8C9.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8C9.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8C9.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8C9.f4=0;_tmp8C9;});
_tmp8C8;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8C7;});break;case 483: _LL3F4: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8CA;(_tmp8CA.YY3).tag=7;(_tmp8CA.YY3).f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8CA;});
break;case 484: _LL3F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8CB;(_tmp8CB.YY5).tag=9;(_tmp8CB.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp8CB;});break;case 485: _LL3F6: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8CC;(_tmp8CC.YY5).tag=9;(_tmp8CC.YY5).f1=({struct Cyc_List_List*
_tmp8CD=_cycalloc(sizeof(*_tmp8CD));_tmp8CD->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8CD->tl=0;_tmp8CD;});
_tmp8CC;});break;case 486: _LL3F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8CE;(_tmp8CE.YY5).tag=9;(_tmp8CE.YY5).f1=({struct Cyc_List_List*_tmp8CF=
_cycalloc(sizeof(*_tmp8CF));_tmp8CF->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8CF->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8CF;});_tmp8CE;});
break;case 487: _LL3F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8D0;(_tmp8D0.YY3).tag=7;(_tmp8D0.YY3).f1=Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D0;});
break;case 488: _LL3F9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8D1;(_tmp8D1.YY3).tag=7;(_tmp8D1.YY3).f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D1;});
break;case 489: _LL3FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8D2;(_tmp8D2.YY3).tag=7;(_tmp8D2.YY3).f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D2;});
break;case 490: _LL3FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8D3;(_tmp8D3.YY3).tag=7;(_tmp8D3.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8D3;});
break;case 491: _LL3FC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8D4;(_tmp8D4.QualId_tok).tag=4;(_tmp8D4.QualId_tok).f1=({struct _tuple1*
_tmp8D5=_cycalloc(sizeof(*_tmp8D5));_tmp8D5->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp8D7;(_tmp8D7.Rel_n).tag=1;(_tmp8D7.Rel_n).f1=0;
_tmp8D7;});_tmp8D5->f2=({struct _dynforward_ptr*_tmp8D6=_cycalloc(sizeof(*_tmp8D6));
_tmp8D6[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp8D6;});_tmp8D5;});_tmp8D4;});break;case 492: _LL3FD: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 493: _LL3FE: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8D8;(_tmp8D8.QualId_tok).tag=
4;(_tmp8D8.QualId_tok).f1=({struct _tuple1*_tmp8D9=_cycalloc(sizeof(*_tmp8D9));
_tmp8D9->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8DB;(
_tmp8DB.Rel_n).tag=1;(_tmp8DB.Rel_n).f1=0;_tmp8DB;});_tmp8D9->f2=({struct
_dynforward_ptr*_tmp8DA=_cycalloc(sizeof(*_tmp8DA));_tmp8DA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8DA;});_tmp8D9;});
_tmp8D8;});break;case 494: _LL3FF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 495: _LL400: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 496: _LL401: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497: _LL402: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 498: _LL403:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 499:
_LL404: break;case 500: _LL405: yylex_buf->lex_curr_pos -=1;break;default: _LL406:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(501,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 133)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6380) && Cyc_yycheck[_check_known_subscript_notnull(6381,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6381,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 133)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1006,yystate)];if(yyn > - 32768  && yyn < 6380){int
sze=0;struct _dynforward_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 275 / 
sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6381,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(275,x)])+ 15,count ++);}msg=({unsigned int _tmp8DC=(
unsigned int)(sze + 15);char*_tmp8DD=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp8DC + 1));struct _dynforward_ptr _tmp8DF=_tag_dynforward(_tmp8DD,
sizeof(char),_tmp8DC + 1);{unsigned int _tmp8DE=_tmp8DC;unsigned int i;for(i=0;i < 
_tmp8DE;i ++){_tmp8DD[i]='\000';}_tmp8DD[_tmp8DE]=(char)0;}_tmp8DF;});Cyc_strcpy(
msg,({const char*_tmp8E0="parse error";_tag_dynforward(_tmp8E0,sizeof(char),
_get_zero_arr_size(_tmp8E0,12));}));if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 
275 / sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6381,x + 
yyn)]== x){Cyc_strcat(msg,(struct _dynforward_ptr)(count == 0?(struct
_dynforward_ptr)({const char*_tmp8E1=", expecting `";_tag_dynforward(_tmp8E1,
sizeof(char),_get_zero_arr_size(_tmp8E1,14));}):(struct _dynforward_ptr)({const
char*_tmp8E2=" or `";_tag_dynforward(_tmp8E2,sizeof(char),_get_zero_arr_size(
_tmp8E2,6));})));Cyc_strcat(msg,(struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(275,x)]);Cyc_strcat(msg,({const char*_tmp8E3="'";
_tag_dynforward(_tmp8E3,sizeof(char),_get_zero_arr_size(_tmp8E3,2));}));count ++;}}}
Cyc_yyerror((struct _dynforward_ptr)msg);}else{Cyc_yyerror(({const char*_tmp8E4="parse error";
_tag_dynforward(_tmp8E4,sizeof(char),_get_zero_arr_size(_tmp8E4,12));}));}}goto
yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8E5=1;
_npop_handler(0);return _tmp8E5;}Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;
yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp8E6=1;_npop_handler(0);return
_tmp8E6;}yyvsp_offset --;yystate=(int)yyss[_check_known_subscript_notnull(10000,
-- yyssp_offset)];yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1006,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 6380) || Cyc_yycheck[_check_known_subscript_notnull(
6381,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6381,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1005){int
_tmp8E7=0;_npop_handler(0);return _tmp8E7;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union _tmp8F1=v;struct _tuple11
_tmp8F2;int _tmp8F3;char _tmp8F4;struct _dynforward_ptr _tmp8F5;struct _tuple1*
_tmp8F6;struct _tuple1 _tmp8F7;union Cyc_Absyn_Nmspace_union _tmp8F8;struct
_dynforward_ptr*_tmp8F9;_LL409: if((_tmp8F1.Int_tok).tag != 0)goto _LL40B;_tmp8F2=(
_tmp8F1.Int_tok).f1;_tmp8F3=_tmp8F2.f2;_LL40A:({struct Cyc_Int_pa_struct _tmp8FC;
_tmp8FC.tag=1;_tmp8FC.f1=(unsigned long)_tmp8F3;{void*_tmp8FA[1]={& _tmp8FC};Cyc_fprintf(
Cyc_stderr,({const char*_tmp8FB="%d";_tag_dynforward(_tmp8FB,sizeof(char),
_get_zero_arr_size(_tmp8FB,3));}),_tag_dynforward(_tmp8FA,sizeof(void*),1));}});
goto _LL408;_LL40B: if((_tmp8F1.Char_tok).tag != 1)goto _LL40D;_tmp8F4=(_tmp8F1.Char_tok).f1;
_LL40C:({struct Cyc_Int_pa_struct _tmp8FF;_tmp8FF.tag=1;_tmp8FF.f1=(unsigned long)((
int)_tmp8F4);{void*_tmp8FD[1]={& _tmp8FF};Cyc_fprintf(Cyc_stderr,({const char*
_tmp8FE="%c";_tag_dynforward(_tmp8FE,sizeof(char),_get_zero_arr_size(_tmp8FE,3));}),
_tag_dynforward(_tmp8FD,sizeof(void*),1));}});goto _LL408;_LL40D: if((_tmp8F1.String_tok).tag
!= 2)goto _LL40F;_tmp8F5=(_tmp8F1.String_tok).f1;_LL40E:({struct Cyc_String_pa_struct
_tmp902;_tmp902.tag=0;_tmp902.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp8F5);{void*_tmp900[1]={& _tmp902};Cyc_fprintf(Cyc_stderr,({const char*_tmp901="\"%s\"";
_tag_dynforward(_tmp901,sizeof(char),_get_zero_arr_size(_tmp901,5));}),
_tag_dynforward(_tmp900,sizeof(void*),1));}});goto _LL408;_LL40F: if((_tmp8F1.QualId_tok).tag
!= 4)goto _LL411;_tmp8F6=(_tmp8F1.QualId_tok).f1;_tmp8F7=*_tmp8F6;_tmp8F8=_tmp8F7.f1;
_tmp8F9=_tmp8F7.f2;_LL410: {struct Cyc_List_List*_tmp903=0;{union Cyc_Absyn_Nmspace_union
_tmp904=_tmp8F8;struct Cyc_List_List*_tmp905;struct Cyc_List_List*_tmp906;_LL414:
if((_tmp904.Rel_n).tag != 1)goto _LL416;_tmp905=(_tmp904.Rel_n).f1;_LL415: _tmp903=
_tmp905;goto _LL413;_LL416: if((_tmp904.Abs_n).tag != 2)goto _LL418;_tmp906=(_tmp904.Abs_n).f1;
_LL417: _tmp903=_tmp906;goto _LL413;_LL418: if((_tmp904.Loc_n).tag != 0)goto _LL413;
_LL419: goto _LL413;_LL413:;}for(0;_tmp903 != 0;_tmp903=_tmp903->tl){({struct Cyc_String_pa_struct
_tmp909;_tmp909.tag=0;_tmp909.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)_tmp903->hd));{void*_tmp907[1]={& _tmp909};Cyc_fprintf(Cyc_stderr,({
const char*_tmp908="%s::";_tag_dynforward(_tmp908,sizeof(char),_get_zero_arr_size(
_tmp908,5));}),_tag_dynforward(_tmp907,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp90C;_tmp90C.tag=0;_tmp90C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp8F9);{void*_tmp90A[1]={& _tmp90C};Cyc_fprintf(Cyc_stderr,({const char*_tmp90B="%s::";
_tag_dynforward(_tmp90B,sizeof(char),_get_zero_arr_size(_tmp90B,5));}),
_tag_dynforward(_tmp90A,sizeof(void*),1));}});goto _LL408;}_LL411:;_LL412:({void*
_tmp90D[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp90E="?";_tag_dynforward(
_tmp90E,sizeof(char),_get_zero_arr_size(_tmp90E,2));}),_tag_dynforward(_tmp90D,
sizeof(void*),0));});goto _LL408;_LL408:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
