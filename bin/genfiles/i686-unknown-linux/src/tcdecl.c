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
 typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
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
struct _dynforward_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,
struct _dynforward_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dynforward_ptr);extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);void*Cyc_Absyn_force_kb(void*kb);struct _dynforward_ptr Cyc_Absyn_attribute2string(
void*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg1,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap);struct _tuple3{void*f1;int f2;};struct _tuple3 Cyc_Tcdecl_merge_scope(
void*s0,void*s1,struct _dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg,int allow_externC_extern_merge);struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct
Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*
d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(struct
Cyc_List_List*f,int*res,struct _dynforward_ptr*v,struct Cyc_Position_Segment*loc,
struct _dynforward_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct
_dynforward_ptr Cyc_Absynpp_scope2string(void*sc);char Cyc_Tcdecl_Incompatible[17]="\000\000\000\000Incompatible\000";
void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg1,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){if(msg1 == 0)(int)_throw((void*)
Cyc_Tcdecl_Incompatible);{struct _dynforward_ptr fmt2=(struct _dynforward_ptr)Cyc_strconcat(({
const char*_tmp6="%s ";_tag_dynforward(_tmp6,sizeof(char),_get_zero_arr_size(
_tmp6,4));}),(struct _dynforward_ptr)fmt);struct _dynforward_ptr ap2=({unsigned int
_tmp0=_get_dynforward_size(ap,sizeof(void*))+ 1;void**_tmp1=(void**)_cycalloc(
_check_times(sizeof(void*),_tmp0));struct _dynforward_ptr _tmp5=_tag_dynforward(
_tmp1,sizeof(void*),_tmp0);{unsigned int _tmp2=_tmp0;unsigned int i;for(i=0;i < 
_tmp2;i ++){_tmp1[i]=i == 0?(void*)({struct Cyc_String_pa_struct*_tmp3=_cycalloc(
sizeof(*_tmp3));_tmp3[0]=({struct Cyc_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*msg1);_tmp4;});_tmp3;}):*((void**)
_check_dynforward_subscript(ap,sizeof(void*),(int)(i - 1)));}}_tmp5;});Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt2,ap2)));}}static void Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct
_dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*msg){({struct Cyc_String_pa_struct _tmpC;_tmpC.tag=0;_tmpC.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_scope2string(s0));{
struct Cyc_String_pa_struct _tmpB;_tmpB.tag=0;_tmpB.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_scope2string(s1));{struct Cyc_String_pa_struct
_tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v);{
struct Cyc_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)t);{void*_tmp7[4]={& _tmp9,& _tmpA,& _tmpB,& _tmpC};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp8="%s %s is %s whereas expected scope is %s";
_tag_dynforward(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,41));}),
_tag_dynforward(_tmp7,sizeof(void*),4));}}}}});}struct _tuple3 Cyc_Tcdecl_merge_scope(
void*s0,void*s1,struct _dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg,int externCmerge){{struct _tuple4 _tmpE=({struct
_tuple4 _tmpD;_tmpD.f1=s0;_tmpD.f2=s1;_tmpD;});void*_tmpF;void*_tmp10;void*_tmp11;
void*_tmp12;void*_tmp13;void*_tmp14;void*_tmp15;void*_tmp16;void*_tmp17;void*
_tmp18;void*_tmp19;void*_tmp1A;void*_tmp1B;void*_tmp1C;void*_tmp1D;void*_tmp1E;
void*_tmp1F;void*_tmp20;_LL1: _tmpF=_tmpE.f1;if((int)_tmpF != 4)goto _LL3;_tmp10=
_tmpE.f2;if((int)_tmp10 != 4)goto _LL3;_LL2: goto _LL0;_LL3: _tmp11=_tmpE.f1;if((int)
_tmp11 != 4)goto _LL5;_tmp12=_tmpE.f2;if((int)_tmp12 != 3)goto _LL5;_LL4: goto _LL6;
_LL5: _tmp13=_tmpE.f1;if((int)_tmp13 != 3)goto _LL7;_tmp14=_tmpE.f2;if((int)_tmp14
!= 4)goto _LL7;_LL6: if(externCmerge)goto _LL0;goto _LL8;_LL7: _tmp15=_tmpE.f1;if((
int)_tmp15 != 4)goto _LL9;_LL8: goto _LLA;_LL9: _tmp16=_tmpE.f2;if((int)_tmp16 != 4)
goto _LLB;_LLA: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct
_tuple3 _tmp21;_tmp21.f1=s1;_tmp21.f2=0;_tmp21;});_LLB: _tmp17=_tmpE.f2;if((int)
_tmp17 != 3)goto _LLD;_LLC: s1=s0;goto _LL0;_LLD: _tmp18=_tmpE.f1;if((int)_tmp18 != 3)
goto _LLF;_LLE: goto _LL0;_LLF: _tmp19=_tmpE.f1;if((int)_tmp19 != 0)goto _LL11;_tmp1A=
_tmpE.f2;if((int)_tmp1A != 0)goto _LL11;_LL10: goto _LL12;_LL11: _tmp1B=_tmpE.f1;if((
int)_tmp1B != 2)goto _LL13;_tmp1C=_tmpE.f2;if((int)_tmp1C != 2)goto _LL13;_LL12: goto
_LL14;_LL13: _tmp1D=_tmpE.f1;if((int)_tmp1D != 1)goto _LL15;_tmp1E=_tmpE.f2;if((int)
_tmp1E != 1)goto _LL15;_LL14: goto _LL0;_LL15: _tmp1F=_tmpE.f1;if((int)_tmp1F != 5)
goto _LL17;_tmp20=_tmpE.f2;if((int)_tmp20 != 5)goto _LL17;_LL16: goto _LL0;_LL17:;
_LL18: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple3 _tmp22;
_tmp22.f1=s1;_tmp22.f2=0;_tmp22;});_LL0:;}return({struct _tuple3 _tmp23;_tmp23.f1=
s1;_tmp23.f2=1;_tmp23;});}static int Cyc_Tcdecl_check_type(void*t0,void*t1){return
Cyc_Tcutil_unify(t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp*e){void*_tmp24=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp25;
int _tmp26;_LL1A: if(*((int*)_tmp24)!= 0)goto _LL1C;_tmp25=((struct Cyc_Absyn_Const_e_struct*)
_tmp24)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp24)->f1).Int_c).tag != 2)
goto _LL1C;_tmp26=(_tmp25.Int_c).f2;_LL1B: return(unsigned int)_tmp26;_LL1C:;_LL1D:(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp27=_cycalloc(
sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Core_Invalid_argument_struct _tmp28;_tmp28.tag=
Cyc_Core_Invalid_argument;_tmp28.f1=({const char*_tmp29="Tcdecl::get_uint_const_value";
_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,29));});_tmp28;});
_tmp27;}));_LL19:;}inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,
struct Cyc_List_List*tvs1,struct _dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){({struct Cyc_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v);{
struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)t);{void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp2B="%s %s has a different number of type parameters";
_tag_dynforward(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,48));}),
_tag_dynforward(_tmp2A,sizeof(void*),2));}}});return 0;}{struct Cyc_List_List*
_tmp2E=tvs0;struct Cyc_List_List*_tmp2F=tvs1;for(0;_tmp2E != 0;(_tmp2E=_tmp2E->tl,
_tmp2F=_tmp2F->tl)){void*_tmp30=Cyc_Absyn_force_kb((void*)((struct Cyc_Absyn_Tvar*)
_tmp2E->hd)->kind);void*_tmp31=Cyc_Absyn_force_kb((void*)((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp2F))->hd)->kind);if(_tmp30 == _tmp31)
continue;({struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp31));{struct
Cyc_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct Cyc_Absyn_Tvar*)_tmp2E->hd)->name);{struct Cyc_String_pa_struct
_tmp36;_tmp36.tag=0;_tmp36.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp30));{struct Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)v);{struct Cyc_String_pa_struct _tmp34;
_tmp34.tag=0;_tmp34.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t);{void*
_tmp32[5]={& _tmp34,& _tmp35,& _tmp36,& _tmp37,& _tmp38};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp33="%s %s has a different kind (%s) for type parameter %s (%s)";
_tag_dynforward(_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,59));}),
_tag_dynforward(_tmp32,sizeof(void*),5));}}}}}});return 0;}return 1;}}static int Cyc_Tcdecl_check_atts(
struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dynforward_ptr t,
struct _dynforward_ptr v,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){
if(!Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_String_pa_struct _tmp3C;_tmp3C.tag=
0;_tmp3C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v);{struct Cyc_String_pa_struct
_tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t);{
void*_tmp39[2]={& _tmp3B,& _tmp3C};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp3A="%s %s has different attributes";
_tag_dynforward(_tmp3A,sizeof(char),_get_zero_arr_size(_tmp3A,31));}),
_tag_dynforward(_tmp39,sizeof(void*),2));}}});return 0;}return 1;}struct _tuple5{
struct Cyc_Absyn_Tvar*f1;void*f2;};static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){inst=({struct Cyc_List_List*_tmp3D=
_cycalloc(sizeof(*_tmp3D));_tmp3D->hd=({struct _tuple5*_tmp3E=_cycalloc(sizeof(*
_tmp3E));_tmp3E->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
tvs1))->hd;_tmp3E->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp3F=_cycalloc(
sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Absyn_VarType_struct _tmp40;_tmp40.tag=1;
_tmp40.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp40;});_tmp3F;});_tmp3E;});_tmp3D->tl=
inst;_tmp3D;});}return inst;}struct _tuple6{struct Cyc_Absyn_AggrdeclImpl*f1;struct
Cyc_Absyn_AggrdeclImpl*f2;};struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct _dynforward_ptr _tmp41=Cyc_Absynpp_qvar2string(
d0->name);int _tmp42=1;if(!((void*)d0->kind == (void*)d1->kind))return 0;if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,({const char*_tmp43="type";_tag_dynforward(_tmp43,sizeof(char),
_get_zero_arr_size(_tmp43,5));}),_tmp41,loc,msg))return 0;{void*_tmp46;int _tmp47;
struct _tuple3 _tmp45=Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,({const
char*_tmp44="type";_tag_dynforward(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,
5));}),_tmp41,loc,msg,1);_tmp46=_tmp45.f1;_tmp47=_tmp45.f2;if(!_tmp47)_tmp42=0;
if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,({const char*_tmp48="type";
_tag_dynforward(_tmp48,sizeof(char),_get_zero_arr_size(_tmp48,5));}),_tmp41,loc,
msg))_tmp42=0;{struct Cyc_Absyn_Aggrdecl*d2;{struct _tuple6 _tmp4A=({struct _tuple6
_tmp49;_tmp49.f1=d0->impl;_tmp49.f2=d1->impl;_tmp49;});struct Cyc_Absyn_AggrdeclImpl*
_tmp4B;struct Cyc_Absyn_AggrdeclImpl*_tmp4C;struct Cyc_Absyn_AggrdeclImpl*_tmp4D;
struct Cyc_Absyn_AggrdeclImpl _tmp4E;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*
_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_Absyn_AggrdeclImpl*_tmp52;struct Cyc_Absyn_AggrdeclImpl
_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp55;struct Cyc_List_List*
_tmp56;_LL1F: _tmp4B=_tmp4A.f2;if(_tmp4B != 0)goto _LL21;_LL20: d2=d0;goto _LL1E;
_LL21: _tmp4C=_tmp4A.f1;if(_tmp4C != 0)goto _LL23;_LL22: d2=d1;goto _LL1E;_LL23:
_tmp4D=_tmp4A.f1;if(_tmp4D == 0)goto _LL1E;_tmp4E=*_tmp4D;_tmp4F=_tmp4E.exist_vars;
_tmp50=_tmp4E.rgn_po;_tmp51=_tmp4E.fields;_tmp52=_tmp4A.f2;if(_tmp52 == 0)goto
_LL1E;_tmp53=*_tmp52;_tmp54=_tmp53.exist_vars;_tmp55=_tmp53.rgn_po;_tmp56=_tmp53.fields;
_LL24: if(!Cyc_Tcdecl_check_tvs(_tmp4F,_tmp54,({const char*_tmp57="type";
_tag_dynforward(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,5));}),_tmp41,loc,
msg))return 0;{struct Cyc_List_List*_tmp58=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp4F),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
d1->tvs,_tmp54));for(0;_tmp50 != 0  && _tmp55 != 0;(_tmp50=_tmp50->tl,_tmp55=_tmp55->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple4*)_tmp50->hd)).f1,(*((struct _tuple4*)
_tmp55->hd)).f1);Cyc_Tcdecl_check_type((*((struct _tuple4*)_tmp50->hd)).f2,(*((
struct _tuple4*)_tmp55->hd)).f2);}for(0;_tmp51 != 0  && _tmp56 != 0;(_tmp51=_tmp51->tl,
_tmp56=_tmp56->tl)){struct Cyc_Absyn_Aggrfield _tmp5A;struct _dynforward_ptr*_tmp5B;
struct Cyc_Absyn_Tqual _tmp5C;void*_tmp5D;struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_List_List*
_tmp5F;struct Cyc_Absyn_Aggrfield*_tmp59=(struct Cyc_Absyn_Aggrfield*)_tmp51->hd;
_tmp5A=*_tmp59;_tmp5B=_tmp5A.name;_tmp5C=_tmp5A.tq;_tmp5D=(void*)_tmp5A.type;
_tmp5E=_tmp5A.width;_tmp5F=_tmp5A.attributes;{struct Cyc_Absyn_Aggrfield _tmp61;
struct _dynforward_ptr*_tmp62;struct Cyc_Absyn_Tqual _tmp63;void*_tmp64;struct Cyc_Absyn_Exp*
_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_Absyn_Aggrfield*_tmp60=(struct Cyc_Absyn_Aggrfield*)
_tmp56->hd;_tmp61=*_tmp60;_tmp62=_tmp61.name;_tmp63=_tmp61.tq;_tmp64=(void*)
_tmp61.type;_tmp65=_tmp61.width;_tmp66=_tmp61.attributes;if(Cyc_strptrcmp(_tmp5B,
_tmp62)!= 0){({struct Cyc_String_pa_struct _tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp62);{struct Cyc_String_pa_struct
_tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp5B);{struct Cyc_String_pa_struct _tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp41);{struct Cyc_String_pa_struct
_tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)({
const char*_tmp6D="type";_tag_dynforward(_tmp6D,sizeof(char),_get_zero_arr_size(
_tmp6D,5));}));{void*_tmp67[4]={& _tmp69,& _tmp6A,& _tmp6B,& _tmp6C};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp68="%s %s : field name mismatch %s != %s";
_tag_dynforward(_tmp68,sizeof(char),_get_zero_arr_size(_tmp68,37));}),
_tag_dynforward(_tmp67,sizeof(void*),4));}}}}});return 0;}if(!Cyc_Tcutil_same_atts(
_tmp5F,_tmp66)){({struct Cyc_String_pa_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp5B);{struct Cyc_String_pa_struct
_tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp41);{struct Cyc_String_pa_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)({const char*_tmp73="type";
_tag_dynforward(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,5));}));{void*
_tmp6E[3]={& _tmp70,& _tmp71,& _tmp72};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp6F="%s %s : attribute mismatch on field %s";
_tag_dynforward(_tmp6F,sizeof(char),_get_zero_arr_size(_tmp6F,39));}),
_tag_dynforward(_tmp6E,sizeof(void*),3));}}}});_tmp42=0;}if(!Cyc_Tcutil_equal_tqual(
_tmp5C,_tmp63)){({struct Cyc_String_pa_struct _tmp78;_tmp78.tag=0;_tmp78.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp5B);{struct Cyc_String_pa_struct
_tmp77;_tmp77.tag=0;_tmp77.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp41);{struct Cyc_String_pa_struct _tmp76;_tmp76.tag=0;_tmp76.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)({const char*_tmp79="type";
_tag_dynforward(_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,5));}));{void*
_tmp74[3]={& _tmp76,& _tmp77,& _tmp78};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp75="%s %s : qualifier mismatch on field %s";
_tag_dynforward(_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,39));}),
_tag_dynforward(_tmp74,sizeof(void*),3));}}}});_tmp42=0;}if(((_tmp5E != 0  && 
_tmp65 != 0) && Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_tmp5E)!= 
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_tmp65) || _tmp5E == 0  && 
_tmp65 != 0) || _tmp5E != 0  && _tmp65 == 0){({struct Cyc_String_pa_struct _tmp7E;
_tmp7E.tag=0;_tmp7E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp5B);{
struct Cyc_String_pa_struct _tmp7D;_tmp7D.tag=0;_tmp7D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp41);{struct Cyc_String_pa_struct _tmp7C;_tmp7C.tag=0;
_tmp7C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)({const char*_tmp7F="type";
_tag_dynforward(_tmp7F,sizeof(char),_get_zero_arr_size(_tmp7F,5));}));{void*
_tmp7A[3]={& _tmp7C,& _tmp7D,& _tmp7E};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp7B="%s %s : bitfield mismatch on field %s";
_tag_dynforward(_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,38));}),
_tag_dynforward(_tmp7A,sizeof(void*),3));}}}});_tmp42=0;}{void*subst_t1=Cyc_Tcutil_substitute(
_tmp58,_tmp64);if(!Cyc_Tcdecl_check_type(_tmp5D,subst_t1)){({struct Cyc_String_pa_struct
_tmp85;_tmp85.tag=0;_tmp85.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
subst_t1));{struct Cyc_String_pa_struct _tmp84;_tmp84.tag=0;_tmp84.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp5D));{struct
Cyc_String_pa_struct _tmp83;_tmp83.tag=0;_tmp83.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp5B);{struct Cyc_String_pa_struct _tmp82;_tmp82.tag=0;
_tmp82.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp41);{void*_tmp80[4]={&
_tmp82,& _tmp83,& _tmp84,& _tmp85};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp81="type %s : type mismatch on field %s: %s != %s";
_tag_dynforward(_tmp81,sizeof(char),_get_zero_arr_size(_tmp81,46));}),
_tag_dynforward(_tmp80,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();
_tmp42=0;}}}}if(_tmp51 != 0){({struct Cyc_String_pa_struct _tmp89;_tmp89.tag=0;
_tmp89.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)
_tmp51->hd)->name);{struct Cyc_String_pa_struct _tmp88;_tmp88.tag=0;_tmp88.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp41);{void*_tmp86[2]={& _tmp88,&
_tmp89};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp87="type %s is missing field %s";
_tag_dynforward(_tmp87,sizeof(char),_get_zero_arr_size(_tmp87,28));}),
_tag_dynforward(_tmp86,sizeof(void*),2));}}});_tmp42=0;}if(_tmp56 != 0){({struct
Cyc_String_pa_struct _tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp56->hd)->name);{struct
Cyc_String_pa_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp41);{void*_tmp8A[2]={& _tmp8C,& _tmp8D};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp8B="type %s has extra field %s";_tag_dynforward(_tmp8B,
sizeof(char),_get_zero_arr_size(_tmp8B,27));}),_tag_dynforward(_tmp8A,sizeof(
void*),2));}}});_tmp42=0;}d2=d0;goto _LL1E;}_LL1E:;}if(!_tmp42)return 0;if(_tmp46
== (void*)d2->sc)return(struct Cyc_Absyn_Aggrdecl*)d2;else{d2=({struct Cyc_Absyn_Aggrdecl*
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=*d2;_tmp8E;});(void*)(d2->sc=(void*)
_tmp46);return(struct Cyc_Absyn_Aggrdecl*)d2;}}}}inline static struct
_dynforward_ptr Cyc_Tcdecl_is_x2string(int is_x){return is_x?({const char*_tmp8F="xtunion";
_tag_dynforward(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,8));}):({const char*
_tmp90="tunion";_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,7));});}
struct _tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_Absyn_Tunionfield*
Cyc_Tcdecl_merge_tunionfield(struct Cyc_Absyn_Tunionfield*f0,struct Cyc_Absyn_Tunionfield*
f1,struct Cyc_List_List*inst,struct _dynforward_ptr t,struct _dynforward_ptr v,struct
_dynforward_ptr*msg){struct Cyc_Position_Segment*loc=f1->loc;if(Cyc_strptrcmp((*
f0->name).f2,(*f1->name).f2)!= 0){({struct Cyc_String_pa_struct _tmp96;_tmp96.tag=
0;_tmp96.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*f0->name).f2);{
struct Cyc_String_pa_struct _tmp95;_tmp95.tag=0;_tmp95.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*(*f1->name).f2);{struct Cyc_String_pa_struct _tmp94;_tmp94.tag=
0;_tmp94.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v);{struct Cyc_String_pa_struct
_tmp93;_tmp93.tag=0;_tmp93.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t);{
void*_tmp91[4]={& _tmp93,& _tmp94,& _tmp95,& _tmp96};Cyc_Tcdecl_merr(loc,msg,({const
char*_tmp92="%s %s: field name mismatch %s != %s";_tag_dynforward(_tmp92,sizeof(
char),_get_zero_arr_size(_tmp92,36));}),_tag_dynforward(_tmp91,sizeof(void*),4));}}}}});
return 0;}{struct _dynforward_ptr _tmp97=*(*f0->name).f2;void*_tmp9D;int _tmp9E;
struct _tuple3 _tmp9C=Cyc_Tcdecl_merge_scope((void*)f0->sc,(void*)f1->sc,(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp9B;_tmp9B.tag=0;_tmp9B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)v);{struct Cyc_String_pa_struct
_tmp9A;_tmp9A.tag=0;_tmp9A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t);{
void*_tmp98[2]={& _tmp9A,& _tmp9B};Cyc_aprintf(({const char*_tmp99="in %s %s, field";
_tag_dynforward(_tmp99,sizeof(char),_get_zero_arr_size(_tmp99,16));}),
_tag_dynforward(_tmp98,sizeof(void*),2));}}}),_tmp97,loc,msg,0);_tmp9D=_tmp9C.f1;
_tmp9E=_tmp9C.f2;{struct Cyc_List_List*_tmp9F=f0->typs;struct Cyc_List_List*_tmpA0=
f1->typs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F)!= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpA0)){({struct Cyc_String_pa_struct
_tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp97);{struct Cyc_String_pa_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)v);{struct Cyc_String_pa_struct _tmpA3;
_tmpA3.tag=0;_tmpA3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t);{void*
_tmpA1[3]={& _tmpA3,& _tmpA4,& _tmpA5};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpA2="%s %s, field %s: parameter number mismatch";
_tag_dynforward(_tmpA2,sizeof(char),_get_zero_arr_size(_tmpA2,43));}),
_tag_dynforward(_tmpA1,sizeof(void*),3));}}}});_tmp9E=0;}for(0;_tmp9F != 0;(
_tmp9F=_tmp9F->tl,_tmpA0=_tmpA0->tl)){if(!Cyc_Tcutil_equal_tqual((*((struct
_tuple7*)_tmp9F->hd)).f1,(*((struct _tuple7*)((struct Cyc_List_List*)_check_null(
_tmpA0))->hd)).f1)){({struct Cyc_String_pa_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp97);{struct Cyc_String_pa_struct
_tmpA9;_tmpA9.tag=0;_tmpA9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v);{
struct Cyc_String_pa_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)t);{void*_tmpA6[3]={& _tmpA8,& _tmpA9,& _tmpAA};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmpA7="%s %s, field %s: parameter qualifier";
_tag_dynforward(_tmpA7,sizeof(char),_get_zero_arr_size(_tmpA7,37));}),
_tag_dynforward(_tmpA6,sizeof(void*),3));}}}});_tmp9E=0;}{void*subst_t1=Cyc_Tcutil_substitute(
inst,(*((struct _tuple7*)_tmpA0->hd)).f2);if(!Cyc_Tcdecl_check_type((*((struct
_tuple7*)_tmp9F->hd)).f2,subst_t1)){({struct Cyc_String_pa_struct _tmpB1;_tmpB1.tag=
0;_tmpB1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
subst_t1));{struct Cyc_String_pa_struct _tmpB0;_tmpB0.tag=0;_tmpB0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((*((struct _tuple7*)
_tmp9F->hd)).f2));{struct Cyc_String_pa_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp97);{struct Cyc_String_pa_struct
_tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v);{
struct Cyc_String_pa_struct _tmpAD;_tmpAD.tag=0;_tmpAD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)t);{void*_tmpAB[5]={& _tmpAD,& _tmpAE,& _tmpAF,& _tmpB0,&
_tmpB1};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpAC="%s %s, field %s: parameter type mismatch %s != %s";
_tag_dynforward(_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,50));}),
_tag_dynforward(_tmpAB,sizeof(void*),5));}}}}}});Cyc_Tcutil_explain_failure();
_tmp9E=0;}}}if(!_tmp9E)return 0;if((void*)f0->sc != _tmp9D){struct Cyc_Absyn_Tunionfield*
_tmpB2=({struct Cyc_Absyn_Tunionfield*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=*
f0;_tmpB3;});(void*)(((struct Cyc_Absyn_Tunionfield*)_check_null(_tmpB2))->sc=(
void*)_tmp9D);return _tmpB2;}else{return(struct Cyc_Absyn_Tunionfield*)f0;}}}}
static struct _tuple7*Cyc_Tcdecl_substitute_tunionfield_f2(struct Cyc_List_List*
inst,struct _tuple7*x){struct _tuple7 _tmpB5;struct Cyc_Absyn_Tqual _tmpB6;void*
_tmpB7;struct _tuple7*_tmpB4=x;_tmpB5=*_tmpB4;_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;
return({struct _tuple7*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->f1=_tmpB6;_tmpB8->f2=
Cyc_Tcutil_substitute(inst,_tmpB7);_tmpB8;});}static struct Cyc_Absyn_Tunionfield*
Cyc_Tcdecl_substitute_tunionfield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Tunionfield*
f1){struct Cyc_Absyn_Tunionfield*_tmpB9=({struct Cyc_Absyn_Tunionfield*_tmpBA=
_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=*f1;_tmpBA;});_tmpB9->typs=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_tunionfield_f2,
inst1,f1->typs);return _tmpB9;}static struct Cyc_List_List*Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct
Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct
_dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*msg){struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpC3=
_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=0;_tmpC3;});struct Cyc_List_List**_tmpBB=
f2sp;int cmp=- 1;for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)f0s->hd)->name,((struct Cyc_Absyn_Tunionfield*)f1s->hd)->name))
< 0){struct Cyc_List_List*_tmpBC=({struct Cyc_List_List*_tmpBD=_cycalloc(sizeof(*
_tmpBD));_tmpBD->hd=(struct Cyc_Absyn_Tunionfield*)f0s->hd;_tmpBD->tl=0;_tmpBD;});*
_tmpBB=_tmpBC;_tmpBB=&((struct Cyc_List_List*)_check_null(_tmpBC))->tl;f0s=f0s->tl;}
if(f0s == 0  || cmp > 0){*incl=0;{struct Cyc_List_List*_tmpBE=({struct Cyc_List_List*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->hd=Cyc_Tcdecl_substitute_tunionfield(
inst,(struct Cyc_Absyn_Tunionfield*)f1s->hd);_tmpBF->tl=0;_tmpBF;});*_tmpBB=
_tmpBE;_tmpBB=&((struct Cyc_List_List*)_check_null(_tmpBE))->tl;}}else{struct Cyc_Absyn_Tunionfield*
_tmpC0=Cyc_Tcdecl_merge_tunionfield((struct Cyc_Absyn_Tunionfield*)f0s->hd,(
struct Cyc_Absyn_Tunionfield*)f1s->hd,inst,t,v,msg);if(_tmpC0 != 0){if(_tmpC0 != (
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)f0s->hd))*incl=0;{
struct Cyc_List_List*_tmpC1=({struct Cyc_List_List*_tmpC2=_cycalloc(sizeof(*_tmpC2));
_tmpC2->hd=(struct Cyc_Absyn_Tunionfield*)_tmpC0;_tmpC2->tl=0;_tmpC2;});*_tmpBB=
_tmpC1;_tmpBB=&((struct Cyc_List_List*)_check_null(_tmpC1))->tl;}}else{*res=0;}
f0s=f0s->tl;}}if(f1s != 0){*incl=0;*_tmpBB=f1s;}else{*_tmpBB=f0s;}return*f2sp;}
struct _tuple8{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct
_tuple8 Cyc_Tcdecl_split(struct Cyc_List_List*f){if(f == 0)return({struct _tuple8
_tmpC4;_tmpC4.f1=0;_tmpC4.f2=0;_tmpC4;});if(f->tl == 0)return({struct _tuple8
_tmpC5;_tmpC5.f1=f;_tmpC5.f2=0;_tmpC5;});{struct Cyc_List_List*_tmpC7;struct Cyc_List_List*
_tmpC8;struct _tuple8 _tmpC6=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);
_tmpC7=_tmpC6.f1;_tmpC8=_tmpC6.f2;return({struct _tuple8 _tmpC9;_tmpC9.f1=({struct
Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->hd=(void*)((void*)f->hd);
_tmpCB->tl=_tmpC7;_tmpCB;});_tmpC9.f2=({struct Cyc_List_List*_tmpCA=_cycalloc(
sizeof(*_tmpCA));_tmpCA->hd=(void*)((void*)((struct Cyc_List_List*)_check_null(f->tl))->hd);
_tmpCA->tl=_tmpC8;_tmpCA;});_tmpC9;});}}struct Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List*f,int*res,struct _dynforward_ptr*v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct Cyc_List_List*_tmpCD;struct Cyc_List_List*
_tmpCE;struct _tuple8 _tmpCC=((struct _tuple8(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(
f);_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;if(_tmpCD != 0  && _tmpCD->tl != 0)_tmpCD=Cyc_Tcdecl_sort_xtunion_fields(
_tmpCD,res,v,loc,msg);if(_tmpCE != 0  && _tmpCE->tl != 0)_tmpCE=Cyc_Tcdecl_sort_xtunion_fields(
_tmpCE,res,v,loc,msg);return Cyc_Tcdecl_merge_xtunion_fields(_tmpCD,_tmpCE,0,0,0,
res,({int*_tmpCF=_cycalloc_atomic(sizeof(*_tmpCF));_tmpCF[0]=1;_tmpCF;}),({const
char*_tmpD0="xtunion";_tag_dynforward(_tmpD0,sizeof(char),_get_zero_arr_size(
_tmpD0,8));}),*v,loc,msg);}struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;};struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*
d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*msg){struct _dynforward_ptr _tmpD1=Cyc_Absynpp_qvar2string(d0->name);
struct _dynforward_ptr t=({const char*_tmpF9="[x]tunion";_tag_dynforward(_tmpF9,
sizeof(char),_get_zero_arr_size(_tmpF9,10));});int _tmpD2=1;if(d0->is_xtunion != 
d1->is_xtunion){({struct Cyc_String_pa_struct _tmpD7;_tmpD7.tag=0;_tmpD7.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcdecl_is_x2string(d1->is_xtunion));{
struct Cyc_String_pa_struct _tmpD6;_tmpD6.tag=0;_tmpD6.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Tcdecl_is_x2string(d0->is_xtunion));{struct Cyc_String_pa_struct
_tmpD5;_tmpD5.tag=0;_tmpD5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpD1);{void*_tmpD3[3]={& _tmpD5,& _tmpD6,& _tmpD7};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmpD4="expected %s to be a %s instead of a %s";_tag_dynforward(_tmpD4,
sizeof(char),_get_zero_arr_size(_tmpD4,39));}),_tag_dynforward(_tmpD3,sizeof(
void*),3));}}}});_tmpD2=0;}else{t=Cyc_Tcdecl_is_x2string(d0->is_xtunion);}if(d0->is_flat
!= d1->is_flat){if(d0->is_flat)({void*_tmpD8[0]={};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmpD9="expected __attribute__((flat))";_tag_dynforward(_tmpD9,sizeof(
char),_get_zero_arr_size(_tmpD9,31));}),_tag_dynforward(_tmpD8,sizeof(void*),0));});
else{({void*_tmpDA[0]={};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpDB="did not expect __attribute__((flat))";
_tag_dynforward(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,37));}),
_tag_dynforward(_tmpDA,sizeof(void*),0));});}}if(!Cyc_Tcdecl_check_tvs(d0->tvs,
d1->tvs,t,_tmpD1,loc,msg))return 0;{void*_tmpDD;int _tmpDE;struct _tuple3 _tmpDC=Cyc_Tcdecl_merge_scope((
void*)d0->sc,(void*)d1->sc,t,_tmpD1,loc,msg,0);_tmpDD=_tmpDC.f1;_tmpDE=_tmpDC.f2;
if(!_tmpDE)_tmpD2=0;{struct Cyc_Absyn_Tuniondecl*d2;{struct _tuple9 _tmpE0=({struct
_tuple9 _tmpDF;_tmpDF.f1=d0->fields;_tmpDF.f2=d1->fields;_tmpDF;});struct Cyc_Core_Opt*
_tmpE1;struct Cyc_Core_Opt*_tmpE2;struct Cyc_Core_Opt*_tmpE3;struct Cyc_Core_Opt
_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_Core_Opt*_tmpE6;struct Cyc_Core_Opt
_tmpE7;struct Cyc_List_List*_tmpE8;_LL26: _tmpE1=_tmpE0.f2;if(_tmpE1 != 0)goto _LL28;
_LL27: d2=d0;goto _LL25;_LL28: _tmpE2=_tmpE0.f1;if(_tmpE2 != 0)goto _LL2A;_LL29: d2=d1;
goto _LL25;_LL2A: _tmpE3=_tmpE0.f1;if(_tmpE3 == 0)goto _LL25;_tmpE4=*_tmpE3;_tmpE5=(
struct Cyc_List_List*)_tmpE4.v;_tmpE6=_tmpE0.f2;if(_tmpE6 == 0)goto _LL25;_tmpE7=*
_tmpE6;_tmpE8=(struct Cyc_List_List*)_tmpE7.v;_LL2B: {struct Cyc_List_List*_tmpE9=
Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->is_xtunion){int _tmpEA=1;struct
Cyc_List_List*_tmpEB=Cyc_Tcdecl_merge_xtunion_fields(_tmpE5,_tmpE8,_tmpE9,d0->tvs,
d1->tvs,& _tmpD2,& _tmpEA,t,_tmpD1,loc,msg);if(_tmpEA)d2=d0;else{d2=({struct Cyc_Absyn_Tuniondecl*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=*d0;_tmpEC;});(void*)(d2->sc=(void*)
_tmpDD);d2->fields=({struct Cyc_Core_Opt*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->v=
_tmpEB;_tmpED;});}}else{for(0;_tmpE5 != 0  && _tmpE8 != 0;(_tmpE5=_tmpE5->tl,_tmpE8=
_tmpE8->tl)){Cyc_Tcdecl_merge_tunionfield((struct Cyc_Absyn_Tunionfield*)_tmpE5->hd,(
struct Cyc_Absyn_Tunionfield*)_tmpE8->hd,_tmpE9,t,_tmpD1,msg);}if(_tmpE5 != 0){({
struct Cyc_String_pa_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*(*((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)
_check_null(_tmpE8))->hd)->name).f2);{struct Cyc_String_pa_struct _tmpF1;_tmpF1.tag=
0;_tmpF1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmpD1);{struct Cyc_String_pa_struct
_tmpF0;_tmpF0.tag=0;_tmpF0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t);{
void*_tmpEE[3]={& _tmpF0,& _tmpF1,& _tmpF2};Cyc_Tcdecl_merr(loc,msg,({const char*
_tmpEF="%s %s has extra field %s";_tag_dynforward(_tmpEF,sizeof(char),
_get_zero_arr_size(_tmpEF,25));}),_tag_dynforward(_tmpEE,sizeof(void*),3));}}}});
_tmpD2=0;}if(_tmpE8 != 0){({struct Cyc_String_pa_struct _tmpF7;_tmpF7.tag=0;_tmpF7.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*(*((struct Cyc_Absyn_Tunionfield*)
_tmpE8->hd)->name).f2);{struct Cyc_String_pa_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpD1);{struct Cyc_String_pa_struct
_tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t);{
void*_tmpF3[3]={& _tmpF5,& _tmpF6,& _tmpF7};Cyc_Tcdecl_merr(loc,msg,({const char*
_tmpF4="%s %s is missing field %s";_tag_dynforward(_tmpF4,sizeof(char),
_get_zero_arr_size(_tmpF4,26));}),_tag_dynforward(_tmpF3,sizeof(void*),3));}}}});
_tmpD2=0;}d2=d0;}goto _LL25;}_LL25:;}if(!_tmpD2)return 0;if(_tmpDD == (void*)d2->sc)
return(struct Cyc_Absyn_Tuniondecl*)d2;else{d2=({struct Cyc_Absyn_Tuniondecl*
_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=*d2;_tmpF8;});(void*)(d2->sc=(void*)
_tmpDD);return(struct Cyc_Absyn_Tuniondecl*)d2;}}}}struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct _dynforward_ptr _tmpFA=Cyc_Absynpp_qvar2string(
d0->name);int _tmpFB=1;void*_tmpFE;int _tmpFF;struct _tuple3 _tmpFD=Cyc_Tcdecl_merge_scope((
void*)d0->sc,(void*)d1->sc,({const char*_tmpFC="enum";_tag_dynforward(_tmpFC,
sizeof(char),_get_zero_arr_size(_tmpFC,5));}),_tmpFA,loc,msg,1);_tmpFE=_tmpFD.f1;
_tmpFF=_tmpFD.f2;if(!_tmpFF)_tmpFB=0;{struct Cyc_Absyn_Enumdecl*d2;{struct _tuple9
_tmp101=({struct _tuple9 _tmp100;_tmp100.f1=d0->fields;_tmp100.f2=d1->fields;
_tmp100;});struct Cyc_Core_Opt*_tmp102;struct Cyc_Core_Opt*_tmp103;struct Cyc_Core_Opt*
_tmp104;struct Cyc_Core_Opt _tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Core_Opt*
_tmp107;struct Cyc_Core_Opt _tmp108;struct Cyc_List_List*_tmp109;_LL2D: _tmp102=
_tmp101.f2;if(_tmp102 != 0)goto _LL2F;_LL2E: d2=d0;goto _LL2C;_LL2F: _tmp103=_tmp101.f1;
if(_tmp103 != 0)goto _LL31;_LL30: d2=d1;goto _LL2C;_LL31: _tmp104=_tmp101.f1;if(
_tmp104 == 0)goto _LL2C;_tmp105=*_tmp104;_tmp106=(struct Cyc_List_List*)_tmp105.v;
_tmp107=_tmp101.f2;if(_tmp107 == 0)goto _LL2C;_tmp108=*_tmp107;_tmp109=(struct Cyc_List_List*)
_tmp108.v;_LL32: for(0;_tmp106 != 0  && _tmp109 != 0;(_tmp106=_tmp106->tl,_tmp109=
_tmp109->tl)){struct Cyc_Absyn_Enumfield _tmp10B;struct _tuple0*_tmp10C;struct
_tuple0 _tmp10D;struct _dynforward_ptr*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct
Cyc_Position_Segment*_tmp110;struct Cyc_Absyn_Enumfield*_tmp10A=(struct Cyc_Absyn_Enumfield*)
_tmp106->hd;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.name;_tmp10D=*_tmp10C;_tmp10E=
_tmp10D.f2;_tmp10F=_tmp10B.tag;_tmp110=_tmp10B.loc;{struct Cyc_Absyn_Enumfield
_tmp112;struct _tuple0*_tmp113;struct _tuple0 _tmp114;struct _dynforward_ptr*_tmp115;
struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Position_Segment*_tmp117;struct Cyc_Absyn_Enumfield*
_tmp111=(struct Cyc_Absyn_Enumfield*)_tmp109->hd;_tmp112=*_tmp111;_tmp113=_tmp112.name;
_tmp114=*_tmp113;_tmp115=_tmp114.f2;_tmp116=_tmp112.tag;_tmp117=_tmp112.loc;if(
Cyc_strptrcmp(_tmp10E,_tmp115)!= 0){({struct Cyc_String_pa_struct _tmp11C;_tmp11C.tag=
0;_tmp11C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp115);{struct Cyc_String_pa_struct
_tmp11B;_tmp11B.tag=0;_tmp11B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp10E);{struct Cyc_String_pa_struct _tmp11A;_tmp11A.tag=0;_tmp11A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpFA);{void*_tmp118[3]={& _tmp11A,&
_tmp11B,& _tmp11C};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp119="enum %s: field name mismatch %s != %s";
_tag_dynforward(_tmp119,sizeof(char),_get_zero_arr_size(_tmp119,38));}),
_tag_dynforward(_tmp118,sizeof(void*),3));}}}});_tmpFB=0;}if(Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_check_null(_tmp10F))!= Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_check_null(_tmp116))){({struct Cyc_String_pa_struct _tmp120;
_tmp120.tag=0;_tmp120.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp115);{
struct Cyc_String_pa_struct _tmp11F;_tmp11F.tag=0;_tmp11F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpFA);{void*_tmp11D[2]={& _tmp11F,&
_tmp120};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp11E="enum %s, field %s, value mismatch";
_tag_dynforward(_tmp11E,sizeof(char),_get_zero_arr_size(_tmp11E,34));}),
_tag_dynforward(_tmp11D,sizeof(void*),2));}}});_tmpFB=0;}}}d2=d0;goto _LL2C;_LL2C:;}
if(!_tmpFB)return 0;if((void*)d2->sc == _tmpFE)return(struct Cyc_Absyn_Enumdecl*)d2;
else{d2=({struct Cyc_Absyn_Enumdecl*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121[0]=*
d2;_tmp121;});(void*)(d2->sc=(void*)_tmpFE);return(struct Cyc_Absyn_Enumdecl*)d2;}}}
static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(void*sc0,void*t0,struct Cyc_Absyn_Tqual
tq0,struct Cyc_List_List*atts0,void*sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct
Cyc_List_List*atts1,struct _dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){int _tmp122=1;void*_tmp124;int _tmp125;struct
_tuple3 _tmp123=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);_tmp124=_tmp123.f1;
_tmp125=_tmp123.f2;if(!_tmp125)_tmp122=0;if(!Cyc_Tcdecl_check_type(t0,t1)){({
struct Cyc_String_pa_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t0));{struct Cyc_String_pa_struct
_tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{struct Cyc_String_pa_struct _tmp129;_tmp129.tag=0;
_tmp129.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v);{struct Cyc_String_pa_struct
_tmp128;_tmp128.tag=0;_tmp128.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
t);{void*_tmp126[4]={& _tmp128,& _tmp129,& _tmp12A,& _tmp12B};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp127="%s %s has type \n%s\n instead of \n%s\n";
_tag_dynforward(_tmp127,sizeof(char),_get_zero_arr_size(_tmp127,36));}),
_tag_dynforward(_tmp126,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();
_tmp122=0;}if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){({struct Cyc_String_pa_struct
_tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
v);{struct Cyc_String_pa_struct _tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)t);{void*_tmp12C[2]={& _tmp12E,& _tmp12F};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp12D="%s %s has different type qualifiers";
_tag_dynforward(_tmp12D,sizeof(char),_get_zero_arr_size(_tmp12D,36));}),
_tag_dynforward(_tmp12C,sizeof(void*),2));}}});_tmp122=0;}if(!Cyc_Tcutil_same_atts(
atts0,atts1)){({struct Cyc_String_pa_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)v);{struct Cyc_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
t);{void*_tmp130[2]={& _tmp132,& _tmp133};Cyc_Tcdecl_merr(loc,msg,({const char*
_tmp131="%s %s has different attributes";_tag_dynforward(_tmp131,sizeof(char),
_get_zero_arr_size(_tmp131,31));}),_tag_dynforward(_tmp130,sizeof(void*),2));}}});({
void*_tmp134[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp135="previous attributes: ";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,22));}),
_tag_dynforward(_tmp134,sizeof(void*),0));});for(0;atts0 != 0;atts0=atts0->tl){({
struct Cyc_String_pa_struct _tmp138;_tmp138.tag=0;_tmp138.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd));{
void*_tmp136[1]={& _tmp138};Cyc_fprintf(Cyc_stderr,({const char*_tmp137="%s ";
_tag_dynforward(_tmp137,sizeof(char),_get_zero_arr_size(_tmp137,4));}),
_tag_dynforward(_tmp136,sizeof(void*),1));}});}({void*_tmp139[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp13A="\ncurrent attributes: ";_tag_dynforward(_tmp13A,
sizeof(char),_get_zero_arr_size(_tmp13A,22));}),_tag_dynforward(_tmp139,sizeof(
void*),0));});for(0;atts1 != 0;atts1=atts1->tl){({struct Cyc_String_pa_struct
_tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absyn_attribute2string((void*)atts1->hd));{void*_tmp13B[1]={& _tmp13D};Cyc_fprintf(
Cyc_stderr,({const char*_tmp13C="%s ";_tag_dynforward(_tmp13C,sizeof(char),
_get_zero_arr_size(_tmp13C,4));}),_tag_dynforward(_tmp13B,sizeof(void*),1));}});}({
void*_tmp13E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp13F="\n";
_tag_dynforward(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,2));}),
_tag_dynforward(_tmp13E,sizeof(void*),0));});_tmp122=0;}return({struct _tuple3
_tmp140;_tmp140.f1=_tmp124;_tmp140.f2=_tmp122;_tmp140;});}struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){struct _dynforward_ptr
_tmp141=Cyc_Absynpp_qvar2string(d0->name);void*_tmp144;int _tmp145;struct _tuple3
_tmp143=Cyc_Tcdecl_check_var_or_fn_decl((void*)d0->sc,(void*)d0->type,d0->tq,d0->attributes,(
void*)d1->sc,(void*)d1->type,d1->tq,d1->attributes,({const char*_tmp142="variable";
_tag_dynforward(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,9));}),_tmp141,
loc,msg);_tmp144=_tmp143.f1;_tmp145=_tmp143.f2;if(!_tmp145)return 0;if((void*)d0->sc
== _tmp144)return(struct Cyc_Absyn_Vardecl*)d0;else{struct Cyc_Absyn_Vardecl*
_tmp146=({struct Cyc_Absyn_Vardecl*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=*
d0;_tmp147;});(void*)(((struct Cyc_Absyn_Vardecl*)_check_null(_tmp146))->sc=(void*)
_tmp144);return _tmp146;}}struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct _dynforward_ptr _tmp148=Cyc_Absynpp_qvar2string(
d0->name);if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_tmp149="typedef";
_tag_dynforward(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,8));}),_tmp148,
loc,msg))return 0;{struct Cyc_List_List*_tmp14A=Cyc_Tcdecl_build_tvs_map(d0->tvs,
d1->tvs);if(d0->defn != 0  && d1->defn != 0){void*subst_defn1=Cyc_Tcutil_substitute(
_tmp14A,(void*)((struct Cyc_Core_Opt*)_check_null(d1->defn))->v);if(!Cyc_Tcdecl_check_type((
void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v,subst_defn1)){({struct Cyc_String_pa_struct
_tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v));{
struct Cyc_String_pa_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(subst_defn1));{
struct Cyc_String_pa_struct _tmp14D;_tmp14D.tag=0;_tmp14D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp148);{void*_tmp14B[3]={& _tmp14D,&
_tmp14E,& _tmp14F};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp14C="typedef %s does not refer to the same type: %s != %s";
_tag_dynforward(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,53));}),
_tag_dynforward(_tmp14B,sizeof(void*),3));}}}});return 0;}}return(struct Cyc_Absyn_Typedefdecl*)
d0;}}void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct _tuple4 _tmp151=({struct _tuple4 _tmp150;
_tmp150.f1=b0;_tmp150.f2=b1;_tmp150;});void*_tmp152;void*_tmp153;void*_tmp154;
struct Cyc_Absyn_Vardecl*_tmp155;void*_tmp156;struct Cyc_Absyn_Vardecl*_tmp157;
void*_tmp158;struct Cyc_Absyn_Vardecl*_tmp159;void*_tmp15A;struct Cyc_Absyn_Fndecl*
_tmp15B;void*_tmp15C;void*_tmp15D;struct Cyc_Absyn_Fndecl*_tmp15E;void*_tmp15F;
struct Cyc_Absyn_Fndecl*_tmp160;void*_tmp161;struct Cyc_Absyn_Vardecl*_tmp162;
_LL34: _tmp152=_tmp151.f1;if((int)_tmp152 != 0)goto _LL36;_tmp153=_tmp151.f2;if((
int)_tmp153 != 0)goto _LL36;_LL35: return(void*)0;_LL36: _tmp154=_tmp151.f1;if(
_tmp154 <= (void*)1)goto _LL38;if(*((int*)_tmp154)!= 0)goto _LL38;_tmp155=((struct
Cyc_Absyn_Global_b_struct*)_tmp154)->f1;_tmp156=_tmp151.f2;if(_tmp156 <= (void*)1)
goto _LL38;if(*((int*)_tmp156)!= 0)goto _LL38;_tmp157=((struct Cyc_Absyn_Global_b_struct*)
_tmp156)->f1;_LL37: {struct Cyc_Absyn_Vardecl*_tmp163=Cyc_Tcdecl_merge_vardecl(
_tmp155,_tmp157,loc,msg);if(_tmp163 == 0)return(void*)0;if(_tmp163 == (struct Cyc_Absyn_Vardecl*)
_tmp155)return b0;if(_tmp163 == (struct Cyc_Absyn_Vardecl*)_tmp157)return b1;return(
void*)({struct Cyc_Absyn_Global_b_struct*_tmp164=_cycalloc(sizeof(*_tmp164));
_tmp164[0]=({struct Cyc_Absyn_Global_b_struct _tmp165;_tmp165.tag=0;_tmp165.f1=(
struct Cyc_Absyn_Vardecl*)_tmp163;_tmp165;});_tmp164;});}_LL38: _tmp158=_tmp151.f1;
if(_tmp158 <= (void*)1)goto _LL3A;if(*((int*)_tmp158)!= 0)goto _LL3A;_tmp159=((
struct Cyc_Absyn_Global_b_struct*)_tmp158)->f1;_tmp15A=_tmp151.f2;if(_tmp15A <= (
void*)1)goto _LL3A;if(*((int*)_tmp15A)!= 1)goto _LL3A;_tmp15B=((struct Cyc_Absyn_Funname_b_struct*)
_tmp15A)->f1;_LL39: {int _tmp168;struct _tuple3 _tmp167=Cyc_Tcdecl_check_var_or_fn_decl((
void*)_tmp159->sc,(void*)_tmp159->type,_tmp159->tq,_tmp159->attributes,(void*)
_tmp15B->sc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp15B->cached_typ))->v,
Cyc_Absyn_empty_tqual(0),_tmp15B->attributes,({const char*_tmp166="function";
_tag_dynforward(_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,9));}),Cyc_Absynpp_qvar2string(
_tmp159->name),loc,msg);_tmp168=_tmp167.f2;return _tmp168?b1:(void*)0;}_LL3A:
_tmp15C=_tmp151.f1;if(_tmp15C <= (void*)1)goto _LL3C;if(*((int*)_tmp15C)!= 1)goto
_LL3C;_tmp15D=_tmp151.f2;if(_tmp15D <= (void*)1)goto _LL3C;if(*((int*)_tmp15D)!= 1)
goto _LL3C;_tmp15E=((struct Cyc_Absyn_Funname_b_struct*)_tmp15D)->f1;_LL3B:({
struct Cyc_String_pa_struct _tmp16B;_tmp16B.tag=0;_tmp16B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));{
void*_tmp169[1]={& _tmp16B};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp16A="redefinition of function %s";
_tag_dynforward(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,28));}),
_tag_dynforward(_tmp169,sizeof(void*),1));}});return(void*)0;_LL3C: _tmp15F=
_tmp151.f1;if(_tmp15F <= (void*)1)goto _LL3E;if(*((int*)_tmp15F)!= 1)goto _LL3E;
_tmp160=((struct Cyc_Absyn_Funname_b_struct*)_tmp15F)->f1;_tmp161=_tmp151.f2;if(
_tmp161 <= (void*)1)goto _LL3E;if(*((int*)_tmp161)!= 0)goto _LL3E;_tmp162=((struct
Cyc_Absyn_Global_b_struct*)_tmp161)->f1;_LL3D: {int _tmp16E;struct _tuple3 _tmp16D=
Cyc_Tcdecl_check_var_or_fn_decl((void*)_tmp160->sc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp160->cached_typ))->v,Cyc_Absyn_empty_tqual(0),_tmp160->attributes,(
void*)_tmp162->sc,(void*)_tmp162->type,_tmp162->tq,_tmp162->attributes,({const
char*_tmp16C="variable";_tag_dynforward(_tmp16C,sizeof(char),_get_zero_arr_size(
_tmp16C,9));}),Cyc_Absynpp_qvar2string(_tmp160->name),loc,msg);_tmp16E=_tmp16D.f2;
return _tmp16E?b0:(void*)0;}_LL3E:;_LL3F:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp170;_tmp170.tag=Cyc_Core_Invalid_argument;_tmp170.f1=({const char*_tmp171="Tcdecl::merge_binding";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,22));});_tmp170;});
_tmp16F;}));_LL33:;}struct Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct
Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){struct Cyc_Tcdecl_Xtunionfielddecl
_tmp173;struct Cyc_Absyn_Tuniondecl*_tmp174;struct Cyc_Absyn_Tunionfield*_tmp175;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp172=d0;_tmp173=*_tmp172;_tmp174=_tmp173.base;
_tmp175=_tmp173.field;{struct Cyc_Tcdecl_Xtunionfielddecl _tmp177;struct Cyc_Absyn_Tuniondecl*
_tmp178;struct Cyc_Absyn_Tunionfield*_tmp179;struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp176=d1;_tmp177=*_tmp176;_tmp178=_tmp177.base;_tmp179=_tmp177.field;{struct
_dynforward_ptr _tmp17A=Cyc_Absynpp_qvar2string(_tmp175->name);if(Cyc_Absyn_qvar_cmp(
_tmp174->name,_tmp178->name)!= 0){({struct Cyc_String_pa_struct _tmp17F;_tmp17F.tag=
0;_tmp17F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp17A);{struct Cyc_String_pa_struct
_tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp178->name));{struct Cyc_String_pa_struct _tmp17D;
_tmp17D.tag=0;_tmp17D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp174->name));{void*_tmp17B[3]={& _tmp17D,& _tmp17E,& _tmp17F};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp17C="xtunions %s and %s have both a field named %s";
_tag_dynforward(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,46));}),
_tag_dynforward(_tmp17B,sizeof(void*),3));}}}});return 0;}if(!Cyc_Tcdecl_check_tvs(
_tmp174->tvs,_tmp178->tvs,({const char*_tmp180="xtunion";_tag_dynforward(_tmp180,
sizeof(char),_get_zero_arr_size(_tmp180,8));}),Cyc_Absynpp_qvar2string(_tmp174->name),
loc,msg))return 0;{struct Cyc_List_List*_tmp181=Cyc_Tcdecl_build_tvs_map(_tmp174->tvs,
_tmp178->tvs);struct Cyc_Absyn_Tunionfield*_tmp182=Cyc_Tcdecl_merge_tunionfield(
_tmp175,_tmp179,_tmp181,({const char*_tmp184="xtunionfield";_tag_dynforward(
_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,13));}),_tmp17A,msg);if(_tmp182
== 0)return 0;if(_tmp182 == (struct Cyc_Absyn_Tunionfield*)_tmp175)return(struct Cyc_Tcdecl_Xtunionfielddecl*)
d0;return({struct Cyc_Tcdecl_Xtunionfielddecl*_tmp183=_cycalloc(sizeof(*_tmp183));
_tmp183->base=_tmp174;_tmp183->field=(struct Cyc_Absyn_Tunionfield*)_tmp182;
_tmp183;});}}}}
