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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
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
_dynforward_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(
void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_strcmp(struct _dynforward_ptr s1,struct _dynforward_ptr s2);typedef
struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};struct _dynforward_ptr Cyc_vrprintf(struct
_RegionHandle*,struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dynforward_ptr,int*is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{
struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_unique_pointer(
void*t);int Cyc_Tcutil_is_unique_path(struct Cyc_Absyn_Exp*e);struct _tuple6{int f1;
void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct _tuple7{unsigned int f1;int
f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(
void*t);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dynforward_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct
Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr msg_part);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
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
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dynforward_ptr msg,struct _dynforward_ptr ap){({void*_tmp0[0]={};
Cyc_Tcutil_terr(loc,(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,
ap),_tag_dynforward(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle _tmp4=_new_region("r");struct
_RegionHandle*r=& _tmp4;_push_region(r);{void*_tmp5=Cyc_Tcenv_lookup_ordinary(r,
te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*_tmp7;struct Cyc_Absyn_Enumfield*
_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;struct Cyc_Absyn_Tuniondecl*
_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((int*)_tmp5)!= 0)goto _LL3;
_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;_LL2:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=({
struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=1;_tmpE.f1=q;_tmpE.f2=(void*)_tmp6;
_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)goto _LL5;_tmp7=((struct
Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Enum_e_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;
_tmp10.tag=33;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7;_tmp10.f3=(
struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;_LL5: if(*((int*)
_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;
_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=34;_tmp12.f1=q;
_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp12;});
_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp14;_tmp14.tag=32;_tmp14.f1=0;
_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});_tmp13;})));goto _LL0;_LL9: if(*((int*)
_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*
_tmp15[1]={& _tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
_tag_dynforward(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,31));}),
_tag_dynforward(_tmp15,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({
struct Cyc_Absyn_Var_e_struct _tmp19;_tmp19.tag=1;_tmp19.f1=q;_tmp19.f2=(void*)((
void*)0);_tmp19;});_tmp18;})));goto _LL0;_LL0:;};_pop_region(r);};_pop_handler();}
else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=_tmp2;_LLC: if(_tmp1B != Cyc_Dict_Absent)
goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp1C=
_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Var_e_struct _tmp1D;_tmp1D.tag=
1;_tmp1D.f1=q;_tmp1D.f2=(void*)((void*)0);_tmp1D;});_tmp1C;})));goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){return({struct _tuple8*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->f1=0;_tmp1E->f2=
e;_tmp1E;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(
void*)e1->r;struct _tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp1F)->f1;_LL12: {struct _handler_cons
_tmp21;_push_handler(& _tmp21);{int _tmp23=0;if(setjmp(_tmp21.handler))_tmp23=1;
if(!_tmp23){{struct _RegionHandle _tmp24=_new_region("r");struct _RegionHandle*r=&
_tmp24;_push_region(r);{void*_tmp25=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,
_tmp20);void*_tmp26;struct Cyc_Absyn_Tuniondecl*_tmp27;struct Cyc_Absyn_Tunionfield*
_tmp28;struct Cyc_Absyn_Aggrdecl*_tmp29;_LL16: if(*((int*)_tmp25)!= 0)goto _LL18;
_tmp26=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp25)->f1;_LL17:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));
_tmp2A[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp2B;_tmp2B.tag=11;_tmp2B.f1=e1;
_tmp2B.f2=es;_tmp2B.f3=0;_tmp2B;});_tmp2A;})));_npop_handler(1);return;_LL18: if(*((
int*)_tmp25)!= 2)goto _LL1A;_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f1;
_tmp28=((struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f2;_LL19: if(_tmp28->typs == 0)({
struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp28->name));{void*_tmp2C[1]={&
_tmp2E};Cyc_Tcutil_terr(e->loc,({const char*_tmp2D="%s is a constant, not a function";
_tag_dynforward(_tmp2D,sizeof(char),_get_zero_arr_size(_tmp2D,33));}),
_tag_dynforward(_tmp2C,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_e_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({
struct Cyc_Absyn_Tunion_e_struct _tmp30;_tmp30.tag=32;_tmp30.f1=es;_tmp30.f2=
_tmp27;_tmp30.f3=_tmp28;_tmp30;});_tmp2F;})));_npop_handler(1);return;_LL1A: if(*((
int*)_tmp25)!= 1)goto _LL1C;_tmp29=((struct Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;
_LL1B: {struct Cyc_List_List*_tmp31=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Struct_e_struct _tmp33;
_tmp33.tag=30;_tmp33.f1=_tmp29->name;_tmp33.f2=0;_tmp33.f3=_tmp31;_tmp33.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp29;_tmp33;});_tmp32;})));_npop_handler(1);return;}
_LL1C: if(*((int*)_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp20));{
void*_tmp34[1]={& _tmp36};Cyc_Tcutil_terr(e->loc,({const char*_tmp35="%s is an enum constructor, not a function";
_tag_dynforward(_tmp35,sizeof(char),_get_zero_arr_size(_tmp35,42));}),
_tag_dynforward(_tmp34,sizeof(void*),1));}});_npop_handler(1);return;_LL15:;};
_pop_region(r);};_pop_handler();}else{void*_tmp22=(void*)_exn_thrown;void*_tmp38=
_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)goto _LL23;_LL22:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3A;_tmp3A.tag=11;_tmp3A.f1=e1;_tmp3A.f2=
es;_tmp3A.f3=0;_tmp3A;});_tmp39;})));return;_LL23:;_LL24:(void)_throw(_tmp38);
_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp3C;_tmp3C.tag=11;_tmp3C.f1=e1;_tmp3C.f2=es;_tmp3C.f3=0;_tmp3C;});_tmp3B;})));
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3E;_tmp3E.tag=28;_tmp3E.f1=des;_tmp3E;});_tmp3D;})));return;}{void*t=*topt;
void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;union Cyc_Absyn_AggrInfoU_union
_tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;struct Cyc_Absyn_Tqual _tmp44;
_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)_tmp3F)!= 10)goto _LL28;_tmp40=((
struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;_tmp41=_tmp40.aggr_info;_LL27:{
union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;struct Cyc_Absyn_Aggrdecl**_tmp46;
struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((_tmp45.UnknownAggr).tag != 0)goto _LL31;
_LL30:({void*_tmp48[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp49="struct type not properly set";
_tag_dynforward(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,29));}),
_tag_dynforward(_tmp48,sizeof(void*),0));});_LL31: if((_tmp45.KnownAggr).tag != 1)
goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;_tmp47=*_tmp46;_LL32:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[
0]=({struct Cyc_Absyn_Struct_e_struct _tmp4B;_tmp4B.tag=30;_tmp4B.f1=_tmp47->name;
_tmp4B.f2=0;_tmp4B.f3=des;_tmp4B.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp47;_tmp4B;});
_tmp4A;})));_LL2E:;}goto _LL25;_LL28: if(*((int*)_tmp3F)!= 7)goto _LL2A;_tmp42=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;_tmp43=(void*)_tmp42.elt_type;
_tmp44=_tmp42.tq;_LL29:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Absyn_Array_e_struct
_tmp4D;_tmp4D.tag=28;_tmp4D.f1=des;_tmp4D;});_tmp4C;})));goto _LL25;_LL2A: if(*((
int*)_tmp3F)!= 11)goto _LL2C;_LL2B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp4F;_tmp4F.tag=31;_tmp4F.f1=(void*)t;_tmp4F.f2=des;_tmp4F;});_tmp4E;})));goto
_LL25;_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_Array_e_struct
_tmp51;_tmp51.tag=28;_tmp51.f1=des;_tmp51;});_tmp50;})));goto _LL25;_LL25:;}}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg_part);{void*_tmp52[2]={& _tmp54,& _tmp55};Cyc_Tcutil_terr(e->loc,({const char*
_tmp53="test of %s has type %s instead of integral or * type";_tag_dynforward(
_tmp53,sizeof(char),_get_zero_arr_size(_tmp53,53));}),_tag_dynforward(_tmp52,
sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union*c,struct Cyc_Absyn_Exp*
e){void*t;{union Cyc_Absyn_Cnst_union _tmp56=*((union Cyc_Absyn_Cnst_union*)
_check_null(c));void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*
_tmp5C;int _tmp5D;struct _dynforward_ptr _tmp5E;_LL34: if((_tmp56.Char_c).tag != 0)
goto _LL36;_tmp57=(_tmp56.Char_c).f1;if((int)_tmp57 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;
goto _LL33;_LL36: if((_tmp56.Char_c).tag != 0)goto _LL38;_tmp58=(_tmp56.Char_c).f1;
if((int)_tmp58 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;_LL38: if((
_tmp56.Char_c).tag != 0)goto _LL3A;_tmp59=(_tmp56.Char_c).f1;if((int)_tmp59 != 2)
goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;_LL3A: if((_tmp56.Short_c).tag != 1)
goto _LL3C;_tmp5A=(_tmp56.Short_c).f1;_LL3B: t=_tmp5A == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if((_tmp56.LongLong_c).tag != 3)goto _LL3E;
_tmp5B=(_tmp56.LongLong_c).f1;_LL3D: t=_tmp5B == (void*)1?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL33;_LL3E: if((_tmp56.Float_c).tag != 4)goto _LL40;
_LL3F: t=Cyc_Absyn_float_typ;goto _LL33;_LL40: if((_tmp56.Int_c).tag != 2)goto _LL42;
_tmp5C=(_tmp56.Int_c).f1;_tmp5D=(_tmp56.Int_c).f2;_LL41: if(topt == 0)t=_tmp5C == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp5F=Cyc_Tcutil_compress(*
topt);void*_tmp60;void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;
void*_tmp66;void*_tmp67;void*_tmp68;_LL47: if(_tmp5F <= (void*)4)goto _LL53;if(*((
int*)_tmp5F)!= 5)goto _LL49;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp61 != 0)goto _LL49;_LL48:{void*_tmp69=_tmp60;_LL56: if((int)_tmp69 != 1)goto
_LL58;_LL57: t=Cyc_Absyn_uchar_typ;goto _LL55;_LL58: if((int)_tmp69 != 0)goto _LL5A;
_LL59: t=Cyc_Absyn_schar_typ;goto _LL55;_LL5A: if((int)_tmp69 != 2)goto _LL55;_LL5B: t=
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6A;(_tmp6A.Char_c).tag=0;(_tmp6A.Char_c).f1=(void*)_tmp60;(_tmp6A.Char_c).f2=(
char)_tmp5D;_tmp6A;});goto _LL46;_LL49: if(*((int*)_tmp5F)!= 5)goto _LL4B;_tmp62=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp63 != 1)goto _LL4B;_LL4A: t=_tmp62 == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6B;(_tmp6B.Short_c).tag=1;(_tmp6B.Short_c).f1=(void*)_tmp62;(_tmp6B.Short_c).f2=(
short)_tmp5D;_tmp6B;});goto _LL46;_LL4B: if(*((int*)_tmp5F)!= 5)goto _LL4D;_tmp64=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp65 != 2)goto _LL4D;_LL4C: t=_tmp64 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(*((int*)_tmp5F)!= 5)goto _LL4F;_tmp66=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp67 != 3)goto _LL4F;_LL4E: t=_tmp66 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4F: if(*((int*)_tmp5F)!= 4)goto _LL51;if(!(_tmp5D
== 0))goto _LL51;_LL50:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_Const_e_struct
_tmp6D;_tmp6D.tag=0;_tmp6D.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6E;(_tmp6E.Null_c).tag=6;_tmp6E;});_tmp6D;});_tmp6C;})));{struct Cyc_List_List*
_tmp6F=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Absyn_PointerType_struct
_tmp71;_tmp71.tag=4;_tmp71.f1=({struct Cyc_Absyn_PtrInfo _tmp72;_tmp72.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->v=_tmp6F;_tmp75;}));_tmp72.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp72.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp73;_tmp73.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp74=_cycalloc(
sizeof(*_tmp74));_tmp74->v=_tmp6F;_tmp74;}));_tmp73.nullable=Cyc_Absyn_true_conref;
_tmp73.bounds=Cyc_Absyn_empty_conref();_tmp73.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp73.ptrloc=0;_tmp73;});_tmp72;});_tmp71;});_tmp70;});
goto _LL46;}_LL51: if(*((int*)_tmp5F)!= 18)goto _LL53;_tmp68=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp5F)->f1;_LL52: {struct Cyc_Absyn_TypeInt_struct*_tmp76=({struct Cyc_Absyn_TypeInt_struct*
_tmp7D=_cycalloc_atomic(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp7E;_tmp7E.tag=19;_tmp7E.f1=_tmp5D;_tmp7E;});_tmp7D;});if(!Cyc_Tcutil_unify(
_tmp68,(void*)_tmp76)){({struct Cyc_String_pa_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)
_tmp76));{struct Cyc_String_pa_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp68));{void*
_tmp77[2]={& _tmp79,& _tmp7A};Cyc_Tcutil_terr(loc,({const char*_tmp78="expecting %s but found %s";
_tag_dynforward(_tmp78,sizeof(char),_get_zero_arr_size(_tmp78,26));}),
_tag_dynforward(_tmp77,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[
0]=({struct Cyc_Absyn_TagType_struct _tmp7C;_tmp7C.tag=18;_tmp7C.f1=(void*)((void*)
_tmp76);_tmp7C;});_tmp7B;});goto _LL46;}_LL53:;_LL54: t=_tmp5C == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if((_tmp56.String_c).tag != 
5)goto _LL44;_tmp5E=(_tmp56.String_c).f1;_LL43: {int len=(int)_get_dynforward_size(
_tmp5E,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp89;(_tmp89.Int_c).tag=2;(_tmp89.Int_c).f1=(void*)((
void*)1);(_tmp89.Int_c).f2=len;_tmp89;}),loc);elen->topt=({struct Cyc_Core_Opt*
_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->v=(void*)Cyc_Absyn_uint_typ;_tmp7F;});
t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp81;_tmp81.tag=0;_tmp81.f1=elen;_tmp81;});
_tmp80;}),Cyc_Absyn_true_conref);if(topt != 0){void*_tmp82=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_ArrayInfo _tmp83;struct Cyc_Absyn_Tqual _tmp84;_LL5D: if(
_tmp82 <= (void*)4)goto _LL61;if(*((int*)_tmp82)!= 7)goto _LL5F;_tmp83=((struct Cyc_Absyn_ArrayType_struct*)
_tmp82)->f1;_tmp84=_tmp83.tq;_LL5E: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
_tmp84,(struct Cyc_Absyn_Exp*)elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),
0);_LL5F: if(*((int*)_tmp82)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t)
 && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->v=(void*)t;_tmp85;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(
0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp86=_cycalloc(sizeof(*_tmp86));
_tmp86[0]=({struct Cyc_Absyn_Upper_b_struct _tmp87;_tmp87.tag=0;_tmp87.f1=elen;
_tmp87;});_tmp86;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp88=_cycalloc(sizeof(*_tmp88));
_tmp88->v=(void*)t;_tmp88;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}_LL44: if((_tmp56.Null_c).tag
!= 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8A=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B[0]=({struct Cyc_Absyn_PointerType_struct _tmp8C;_tmp8C.tag=4;_tmp8C.f1=({
struct Cyc_Absyn_PtrInfo _tmp8D;_tmp8D.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp90=_cycalloc(sizeof(*
_tmp90));_tmp90->v=_tmp8A;_tmp90;}));_tmp8D.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp8D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp8E;_tmp8E.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp8F=_cycalloc(
sizeof(*_tmp8F));_tmp8F->v=_tmp8A;_tmp8F;}));_tmp8E.nullable=Cyc_Absyn_true_conref;
_tmp8E.bounds=Cyc_Absyn_empty_conref();_tmp8E.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp8E.ptrloc=0;_tmp8E;});_tmp8D;});_tmp8C;});_tmp8B;});
goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp91=b;struct Cyc_Absyn_Vardecl*
_tmp92;struct Cyc_Absyn_Fndecl*_tmp93;struct Cyc_Absyn_Vardecl*_tmp94;struct Cyc_Absyn_Vardecl*
_tmp95;struct Cyc_Absyn_Vardecl*_tmp96;_LL64: if((int)_tmp91 != 0)goto _LL66;_LL65:
return({struct Cyc_String_pa_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmp97[
1]={& _tmp99};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp98="undeclared identifier: %s";
_tag_dynforward(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,26));}),
_tag_dynforward(_tmp97,sizeof(void*),1));}});_LL66: if(_tmp91 <= (void*)1)goto
_LL68;if(*((int*)_tmp91)!= 0)goto _LL68;_tmp92=((struct Cyc_Absyn_Global_b_struct*)
_tmp91)->f1;_LL67:*q=*_tmp92->name;return(void*)_tmp92->type;_LL68: if(_tmp91 <= (
void*)1)goto _LL6A;if(*((int*)_tmp91)!= 1)goto _LL6A;_tmp93=((struct Cyc_Absyn_Funname_b_struct*)
_tmp91)->f1;_LL69:*q=*_tmp93->name;return Cyc_Tcutil_fndecl2typ(_tmp93);_LL6A: if(
_tmp91 <= (void*)1)goto _LL6C;if(*((int*)_tmp91)!= 4)goto _LL6C;_tmp94=((struct Cyc_Absyn_Pat_b_struct*)
_tmp91)->f1;_LL6B: _tmp95=_tmp94;goto _LL6D;_LL6C: if(_tmp91 <= (void*)1)goto _LL6E;
if(*((int*)_tmp91)!= 3)goto _LL6E;_tmp95=((struct Cyc_Absyn_Local_b_struct*)_tmp91)->f1;
_LL6D: _tmp96=_tmp95;goto _LL6F;_LL6E: if(_tmp91 <= (void*)1)goto _LL63;if(*((int*)
_tmp91)!= 2)goto _LL63;_tmp96=((struct Cyc_Absyn_Param_b_struct*)_tmp91)->f1;_LL6F:(*
q).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp9A;(_tmp9A.Loc_n).tag=
0;_tmp9A;});return(void*)_tmp96->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,struct Cyc_Position_Segment*)){struct
Cyc_List_List*desc_types;{void*_tmp9B=(void*)fmt->r;union Cyc_Absyn_Cnst_union
_tmp9C;struct _dynforward_ptr _tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp
_tmp9F;void*_tmpA0;union Cyc_Absyn_Cnst_union _tmpA1;struct _dynforward_ptr _tmpA2;
_LL71: if(*((int*)_tmp9B)!= 0)goto _LL73;_tmp9C=((struct Cyc_Absyn_Const_e_struct*)
_tmp9B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp9B)->f1).String_c).tag != 
5)goto _LL73;_tmp9D=(_tmp9C.String_c).f1;_LL72: _tmpA2=_tmp9D;goto _LL74;_LL73: if(*((
int*)_tmp9B)!= 15)goto _LL75;_tmp9E=((struct Cyc_Absyn_Cast_e_struct*)_tmp9B)->f2;
_tmp9F=*_tmp9E;_tmpA0=(void*)_tmp9F.r;if(*((int*)_tmpA0)!= 0)goto _LL75;_tmpA1=((
struct Cyc_Absyn_Const_e_struct*)_tmpA0)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpA0)->f1).String_c).tag != 5)goto _LL75;_tmpA2=(_tmpA1.String_c).f1;_LL74:
desc_types=type_getter(temp,te,(struct _dynforward_ptr)_tmpA2,fmt->loc);goto _LL70;
_LL75:;_LL76: if(opt_args != 0){struct Cyc_List_List*_tmpA3=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpA3 != 0;_tmpA3=_tmpA3->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpA3->hd);if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->topt))->v) && !Cyc_Tcutil_is_unique_path((
struct Cyc_Absyn_Exp*)_tmpA3->hd))({void*_tmpA4[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpA3->hd)->loc,({const char*_tmpA5="Cannot consume paths; do swap instead";
_tag_dynforward(_tmpA5,sizeof(char),_get_zero_arr_size(_tmpA5,38));}),
_tag_dynforward(_tmpA4,sizeof(void*),0));});}}return;_LL70:;}if(opt_args != 0){
struct Cyc_List_List*_tmpA6=(struct Cyc_List_List*)opt_args->v;for(0;desc_types != 
0  && _tmpA6 != 0;(desc_types=desc_types->tl,_tmpA6=_tmpA6->tl)){void*t=(void*)
desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpA6->hd;Cyc_Tcexp_tcExp(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmpA9;_tmpA9.tag=0;_tmpA9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpA7[2]={& _tmpA9,& _tmpAA};Cyc_Tcutil_terr(e->loc,({const char*_tmpA8="descriptor has type \n%s\n but argument has type \n%s";
_tag_dynforward(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,51));}),
_tag_dynforward(_tmpA7,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}if(
Cyc_Tcutil_is_unique_pointer(t) && !Cyc_Tcutil_is_unique_path(e))({void*_tmpAB[0]={};
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA6->hd)->loc,({const char*_tmpAC="Cannot consume paths; do swap instead";
_tag_dynforward(_tmpAC,sizeof(char),_get_zero_arr_size(_tmpAC,38));}),
_tag_dynforward(_tmpAB,sizeof(void*),0));});}if(desc_types != 0)({void*_tmpAD[0]={};
Cyc_Tcutil_terr(fmt->loc,({const char*_tmpAE="too few arguments";_tag_dynforward(
_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,18));}),_tag_dynforward(_tmpAD,
sizeof(void*),0));});if(_tmpA6 != 0)({void*_tmpAF[0]={};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpA6->hd)->loc,({const char*_tmpB0="too many arguments";
_tag_dynforward(_tmpB0,sizeof(char),_get_zero_arr_size(_tmpB0,19));}),
_tag_dynforward(_tmpAF,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpB1=p;_LL78: if((int)_tmpB1 != 0)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if((int)_tmpB1 != 2)goto _LL7C;_LL7B: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpB4;_tmpB4.tag=0;_tmpB4.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB2[1]={& _tmpB4};Cyc_Tcutil_terr(
loc,({const char*_tmpB3="expecting arithmetic type but found %s";_tag_dynforward(
_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,39));}),_tag_dynforward(_tmpB2,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7C: if((int)_tmpB1 != 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpB7;_tmpB7.tag=
0;_tmpB7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB5[1]={& _tmpB7};Cyc_Tcutil_terr(loc,({const char*_tmpB6="expecting integral or * type but found %s";
_tag_dynforward(_tmpB6,sizeof(char),_get_zero_arr_size(_tmpB6,42));}),
_tag_dynforward(_tmpB5,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7E: if((
int)_tmpB1 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct
_tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB8[1]={& _tmpBA};Cyc_Tcutil_terr(loc,({const char*_tmpB9="expecting integral type but found %s";
_tag_dynforward(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,37));}),
_tag_dynforward(_tmpB8,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL80: if((int)_tmpB1 != 19)goto _LL82;_LL81:{void*_tmpBB=
t;struct Cyc_Absyn_PtrInfo _tmpBC;struct Cyc_Absyn_PtrAtts _tmpBD;struct Cyc_Absyn_Conref*
_tmpBE;_LL85: if(_tmpBB <= (void*)4)goto _LL89;if(*((int*)_tmpBB)!= 7)goto _LL87;
_LL86: goto _LL84;_LL87: if(*((int*)_tmpBB)!= 4)goto _LL89;_tmpBC=((struct Cyc_Absyn_PointerType_struct*)
_tmpBB)->f1;_tmpBD=_tmpBC.ptr_atts;_tmpBE=_tmpBD.bounds;_LL88:{union Cyc_Absyn_Constraint_union
_tmpBF=(Cyc_Absyn_compress_conref(_tmpBE))->v;void*_tmpC0;void*_tmpC1;void*
_tmpC2;_LL8C: if((_tmpBF.Eq_constr).tag != 0)goto _LL8E;_tmpC0=(_tmpBF.Eq_constr).f1;
if((int)_tmpC0 != 0)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if((_tmpBF.Eq_constr).tag != 0)
goto _LL90;_tmpC1=(_tmpBF.Eq_constr).f1;if((int)_tmpC1 != 1)goto _LL90;_LL8F: goto
_LL91;_LL90: if((_tmpBF.Eq_constr).tag != 0)goto _LL92;_tmpC2=(_tmpBF.Eq_constr).f1;
if(_tmpC2 <= (void*)2)goto _LL92;if(*((int*)_tmpC2)!= 0)goto _LL92;_LL91: goto _LL8B;
_LL92:;_LL93:({void*_tmpC3[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpC4="can't apply size to pointer/array of abstract length";
_tag_dynforward(_tmpC4,sizeof(char),_get_zero_arr_size(_tmpC4,53));}),
_tag_dynforward(_tmpC3,sizeof(void*),0));});_LL8B:;}goto _LL84;_LL89:;_LL8A:({
struct Cyc_String_pa_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC5[1]={& _tmpC7};Cyc_Tcutil_terr(
loc,({const char*_tmpC6="size requires pointer or array type, not %s";
_tag_dynforward(_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,44));}),
_tag_dynforward(_tmpC5,sizeof(void*),1));}});_LL84:;}return Cyc_Absyn_uint_typ;
_LL82:;_LL83:({void*_tmpC8[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpC9="Non-unary primop";
_tag_dynforward(_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,17));}),
_tag_dynforward(_tmpC8,sizeof(void*),0));});_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_String_pa_struct
_tmpCC;_tmpCC.tag=0;_tmpCC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpCA[1]={& _tmpCC};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpCB="type %s cannot be used here";
_tag_dynforward(_tmpCB,sizeof(char),_get_zero_arr_size(_tmpCB,28));}),
_tag_dynforward(_tmpCA,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpCF;_tmpCF.tag=0;_tmpCF.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpCD[1]={& _tmpCF};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpCE="type %s cannot be used here";_tag_dynforward(_tmpCE,
sizeof(char),_get_zero_arr_size(_tmpCE,28));}),_tag_dynforward(_tmpCD,sizeof(
void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpD0=t1;struct Cyc_Absyn_PtrInfo
_tmpD1;void*_tmpD2;struct Cyc_Absyn_Tqual _tmpD3;struct Cyc_Absyn_PtrAtts _tmpD4;
void*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct Cyc_Absyn_Conref*_tmpD7;struct
Cyc_Absyn_Conref*_tmpD8;_LL95: if(_tmpD0 <= (void*)4)goto _LL97;if(*((int*)_tmpD0)
!= 4)goto _LL97;_tmpD1=((struct Cyc_Absyn_PointerType_struct*)_tmpD0)->f1;_tmpD2=(
void*)_tmpD1.elt_typ;_tmpD3=_tmpD1.elt_tq;_tmpD4=_tmpD1.ptr_atts;_tmpD5=(void*)
_tmpD4.rgn;_tmpD6=_tmpD4.nullable;_tmpD7=_tmpD4.bounds;_tmpD8=_tmpD4.zero_term;
_LL96: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD2),(void*)1))({void*
_tmpD9[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpDA="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,50));}),
_tag_dynforward(_tmpD9,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t1))({
void*_tmpDB[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpDC="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmpDC,sizeof(char),_get_zero_arr_size(_tmpDC,48));}),
_tag_dynforward(_tmpDB,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({
struct Cyc_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmpDD[1]={& _tmpDF};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpDE="expecting int but found %s";_tag_dynforward(_tmpDE,
sizeof(char),_get_zero_arr_size(_tmpDE,27));}),_tag_dynforward(_tmpDD,sizeof(
void*),1));}});_tmpD7=Cyc_Absyn_compress_conref(_tmpD7);{union Cyc_Absyn_Constraint_union
_tmpE0=_tmpD7->v;void*_tmpE1;void*_tmpE2;void*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;
void*_tmpE5;_LL9A: if((_tmpE0.Eq_constr).tag != 0)goto _LL9C;_tmpE1=(_tmpE0.Eq_constr).f1;
if((int)_tmpE1 != 0)goto _LL9C;_LL9B: return t1;_LL9C: if((_tmpE0.Eq_constr).tag != 0)
goto _LL9E;_tmpE2=(_tmpE0.Eq_constr).f1;if((int)_tmpE2 != 1)goto _LL9E;_LL9D: return
t1;_LL9E: if((_tmpE0.Eq_constr).tag != 0)goto _LLA0;_tmpE3=(_tmpE0.Eq_constr).f1;
if(_tmpE3 <= (void*)2)goto _LLA0;if(*((int*)_tmpE3)!= 0)goto _LLA0;_tmpE4=((struct
Cyc_Absyn_Upper_b_struct*)_tmpE3)->f1;_LL9F: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD8))({void*_tmpE6[0]={};Cyc_Tcutil_warn(e1->loc,({
const char*_tmpE7="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dynforward(_tmpE7,sizeof(char),_get_zero_arr_size(_tmpE7,70));}),
_tag_dynforward(_tmpE6,sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*
_tmpE8=({struct Cyc_Absyn_PointerType_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));
_tmpE9[0]=({struct Cyc_Absyn_PointerType_struct _tmpEA;_tmpEA.tag=4;_tmpEA.f1=({
struct Cyc_Absyn_PtrInfo _tmpEB;_tmpEB.elt_typ=(void*)_tmpD2;_tmpEB.elt_tq=_tmpD3;
_tmpEB.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpEC;_tmpEC.rgn=(void*)_tmpD5;_tmpEC.nullable=
Cyc_Absyn_true_conref;_tmpEC.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmpEC.zero_term=
_tmpD8;_tmpEC.ptrloc=0;_tmpEC;});_tmpEB;});_tmpEA;});_tmpE9;});Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpE8,(void*)3);return(void*)_tmpE8;}_LLA0: if((_tmpE0.Eq_constr).tag
!= 0)goto _LLA2;_tmpE5=(_tmpE0.Eq_constr).f1;if(_tmpE5 <= (void*)2)goto _LLA2;if(*((
int*)_tmpE5)!= 1)goto _LLA2;_LLA1:({void*_tmpED[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpEE="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_dynforward(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,64));}),
_tag_dynforward(_tmpED,sizeof(void*),0));});return t1;_LLA2:;_LLA3: _tmpD7->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmpEF;(_tmpEF.Eq_constr).tag=
0;(_tmpEF.Eq_constr).f1=(void*)((void*)0);_tmpEF;});return t1;_LL99:;}_LL97:;
_LL98: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL94:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;int forward_only1=0;int forward_only2=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt,& forward_only1)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF3;
_tmpF3.tag=0;_tmpF3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpF0[2]={& _tmpF2,&
_tmpF3};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF1="pointer arithmetic on values of different types (%s != %s)";
_tag_dynforward(_tmpF1,sizeof(char),_get_zero_arr_size(_tmpF1,59));}),
_tag_dynforward(_tmpF0,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(forward_only1)({void*_tmpF4[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmpF5="can't subtract from forward-only ? pointer";
_tag_dynforward(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,43));}),
_tag_dynforward(_tmpF4,sizeof(void*),0));});if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpF6[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF7="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,50));}),
_tag_dynforward(_tmpF6,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t1))({
void*_tmpF8[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF9="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,48));}),
_tag_dynforward(_tmpF8,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({
struct Cyc_String_pa_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmpFA[2]={& _tmpFC,& _tmpFD};Cyc_Tcutil_terr(e2->loc,({const char*
_tmpFB="expecting either %s or int but found %s";_tag_dynforward(_tmpFB,sizeof(
char),_get_zero_arr_size(_tmpFB,40));}),_tag_dynforward(_tmpFA,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2) && (Cyc_Tcutil_typ_kind(t1)== (
void*)2  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,
Cyc_Tcenv_lookup_opt_type_vars(te)))))return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(
te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,
e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{{struct
_tuple0 _tmpFF=({struct _tuple0 _tmpFE;_tmpFE.f1=Cyc_Tcutil_compress(t1);_tmpFE.f2=
Cyc_Tcutil_compress(t2);_tmpFE;});void*_tmp100;struct Cyc_Absyn_PtrInfo _tmp101;
void*_tmp102;void*_tmp103;struct Cyc_Absyn_PtrInfo _tmp104;void*_tmp105;_LLA5:
_tmp100=_tmpFF.f1;if(_tmp100 <= (void*)4)goto _LLA7;if(*((int*)_tmp100)!= 4)goto
_LLA7;_tmp101=((struct Cyc_Absyn_PointerType_struct*)_tmp100)->f1;_tmp102=(void*)
_tmp101.elt_typ;_tmp103=_tmpFF.f2;if(_tmp103 <= (void*)4)goto _LLA7;if(*((int*)
_tmp103)!= 4)goto _LLA7;_tmp104=((struct Cyc_Absyn_PointerType_struct*)_tmp103)->f1;
_tmp105=(void*)_tmp104.elt_typ;_LLA6: if(Cyc_Tcutil_unify(_tmp102,_tmp105))return
Cyc_Absyn_sint_typ;goto _LLA4;_LLA7:;_LLA8: goto _LLA4;_LLA4:;}({struct Cyc_String_pa_struct
_tmp109;_tmp109.tag=0;_tmp109.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp108;_tmp108.tag=0;
_tmp108.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp106[2]={& _tmp108,& _tmp109};Cyc_Tcutil_terr(loc,({const char*
_tmp107="comparison not allowed between %s and %s";_tag_dynforward(_tmp107,
sizeof(char),_get_zero_arr_size(_tmp107,41));}),_tag_dynforward(_tmp106,sizeof(
void*),2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*
_tmp10A=p;_LLAA: if((int)_tmp10A != 0)goto _LLAC;_LLAB: return Cyc_Tcexp_tcPlus(te,e1,
e2);_LLAC: if((int)_tmp10A != 2)goto _LLAE;_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);
_LLAE: if((int)_tmp10A != 1)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp10A != 3)
goto _LLB2;_LLB1: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LLB2: if((int)_tmp10A != 4)goto _LLB4;_LLB3: goto _LLB5;_LLB4: if((int)_tmp10A != 13)
goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp10A != 14)goto _LLB8;_LLB7: goto _LLB9;
_LLB8: if((int)_tmp10A != 15)goto _LLBA;_LLB9: goto _LLBB;_LLBA: if((int)_tmp10A != 16)
goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp10A != 17)goto _LLBE;_LLBD: goto _LLBF;
_LLBE: if((int)_tmp10A != 18)goto _LLC0;_LLBF: return Cyc_Tcexp_tcArithBinop(te,e1,e2,
Cyc_Tcutil_is_integral);_LLC0: if((int)_tmp10A != 5)goto _LLC2;_LLC1: goto _LLC3;
_LLC2: if((int)_tmp10A != 6)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp10A != 7)
goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)_tmp10A != 8)goto _LLC8;_LLC7: goto _LLC9;
_LLC8: if((int)_tmp10A != 9)goto _LLCA;_LLC9: goto _LLCB;_LLCA: if((int)_tmp10A != 10)
goto _LLCC;_LLCB: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLCC:;_LLCD:({void*
_tmp10B[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp10C="bad binary primop";_tag_dynforward(_tmp10C,sizeof(char),
_get_zero_arr_size(_tmp10C,18));}),_tag_dynforward(_tmp10B,sizeof(void*),0));});
_LLA9:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLCE: return({void*_tmp10D[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp10E="primitive operator has 0 arguments";_tag_dynforward(_tmp10E,sizeof(char),
_get_zero_arr_size(_tmp10E,35));}),_tag_dynforward(_tmp10D,sizeof(void*),0));});
case 1: _LLCF: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLD0: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLD1: return({void*_tmp10F[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp110="primitive operator has > 2 arguments";
_tag_dynforward(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,37));}),
_tag_dynforward(_tmp10F,sizeof(void*),0));});}return t;}}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){{void*_tmp111=(void*)e->r;void*_tmp112;struct Cyc_Absyn_Vardecl*
_tmp113;void*_tmp114;struct Cyc_Absyn_Vardecl*_tmp115;void*_tmp116;struct Cyc_Absyn_Vardecl*
_tmp117;void*_tmp118;struct Cyc_Absyn_Vardecl*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;
struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct _dynforward_ptr*
_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct _dynforward_ptr*_tmp11F;struct Cyc_Absyn_Exp*
_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;_LLD4: if(*((int*)
_tmp111)!= 1)goto _LLD6;_tmp112=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp111)->f2;
if(_tmp112 <= (void*)1)goto _LLD6;if(*((int*)_tmp112)!= 2)goto _LLD6;_tmp113=((
struct Cyc_Absyn_Param_b_struct*)_tmp112)->f1;_LLD5: _tmp115=_tmp113;goto _LLD7;
_LLD6: if(*((int*)_tmp111)!= 1)goto _LLD8;_tmp114=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp111)->f2;if(_tmp114 <= (void*)1)goto _LLD8;if(*((int*)_tmp114)!= 3)goto _LLD8;
_tmp115=((struct Cyc_Absyn_Local_b_struct*)_tmp114)->f1;_LLD7: _tmp117=_tmp115;
goto _LLD9;_LLD8: if(*((int*)_tmp111)!= 1)goto _LLDA;_tmp116=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp111)->f2;if(_tmp116 <= (void*)1)goto _LLDA;if(*((int*)_tmp116)!= 4)goto _LLDA;
_tmp117=((struct Cyc_Absyn_Pat_b_struct*)_tmp116)->f1;_LLD9: _tmp119=_tmp117;goto
_LLDB;_LLDA: if(*((int*)_tmp111)!= 1)goto _LLDC;_tmp118=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp111)->f2;if(_tmp118 <= (void*)1)goto _LLDC;if(*((int*)_tmp118)!= 0)goto _LLDC;
_tmp119=((struct Cyc_Absyn_Global_b_struct*)_tmp118)->f1;_LLDB: if(!(_tmp119->tq).real_const)
return;goto _LLD3;_LLDC: if(*((int*)_tmp111)!= 25)goto _LLDE;_tmp11A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp111)->f1;_tmp11B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp111)->f2;_LLDD:{
void*_tmp123=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp11A->topt))->v);struct Cyc_Absyn_PtrInfo _tmp124;struct Cyc_Absyn_Tqual _tmp125;
struct Cyc_Absyn_ArrayInfo _tmp126;struct Cyc_Absyn_Tqual _tmp127;struct Cyc_List_List*
_tmp128;_LLEB: if(_tmp123 <= (void*)4)goto _LLF1;if(*((int*)_tmp123)!= 4)goto _LLED;
_tmp124=((struct Cyc_Absyn_PointerType_struct*)_tmp123)->f1;_tmp125=_tmp124.elt_tq;
_LLEC: _tmp127=_tmp125;goto _LLEE;_LLED: if(*((int*)_tmp123)!= 7)goto _LLEF;_tmp126=((
struct Cyc_Absyn_ArrayType_struct*)_tmp123)->f1;_tmp127=_tmp126.tq;_LLEE: if(!
_tmp127.real_const)return;goto _LLEA;_LLEF: if(*((int*)_tmp123)!= 9)goto _LLF1;
_tmp128=((struct Cyc_Absyn_TupleType_struct*)_tmp123)->f1;_LLF0: {unsigned int
_tmp12A;int _tmp12B;struct _tuple7 _tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp11B);
_tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;if(!_tmp12B){({void*_tmp12C[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp12D="tuple projection cannot use sizeof or offsetof";
_tag_dynforward(_tmp12D,sizeof(char),_get_zero_arr_size(_tmp12D,47));}),
_tag_dynforward(_tmp12C,sizeof(void*),0));});return;}{struct _handler_cons _tmp12E;
_push_handler(& _tmp12E);{int _tmp130=0;if(setjmp(_tmp12E.handler))_tmp130=1;if(!
_tmp130){{struct _tuple9 _tmp132;struct Cyc_Absyn_Tqual _tmp133;struct _tuple9*
_tmp131=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp128,(
int)_tmp12A);_tmp132=*_tmp131;_tmp133=_tmp132.f1;if(!_tmp133.real_const){
_npop_handler(0);return;}};_pop_handler();}else{void*_tmp12F=(void*)_exn_thrown;
void*_tmp135=_tmp12F;_LLF4: if(_tmp135 != Cyc_List_Nth)goto _LLF6;_LLF5: return;
_LLF6:;_LLF7:(void)_throw(_tmp135);_LLF3:;}}}goto _LLEA;}_LLF1:;_LLF2: goto _LLEA;
_LLEA:;}goto _LLD3;_LLDE: if(*((int*)_tmp111)!= 23)goto _LLE0;_tmp11C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp111)->f1;_tmp11D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp111)->f2;_LLDF:{
void*_tmp136=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp11C->topt))->v);struct Cyc_Absyn_AggrInfo _tmp137;union Cyc_Absyn_AggrInfoU_union
_tmp138;struct Cyc_Absyn_Aggrdecl**_tmp139;struct Cyc_List_List*_tmp13A;_LLF9: if(
_tmp136 <= (void*)4)goto _LLFD;if(*((int*)_tmp136)!= 10)goto _LLFB;_tmp137=((struct
Cyc_Absyn_AggrType_struct*)_tmp136)->f1;_tmp138=_tmp137.aggr_info;if((((((struct
Cyc_Absyn_AggrType_struct*)_tmp136)->f1).aggr_info).KnownAggr).tag != 1)goto _LLFB;
_tmp139=(_tmp138.KnownAggr).f1;_LLFA: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp139 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp139,_tmp11D);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF8;}_LLFB: if(*((int*)_tmp136)!= 11)goto _LLFD;_tmp13A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp136)->f2;_LLFC: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp13A,
_tmp11D);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFD:;_LLFE: goto
_LLF8;_LLF8:;}goto _LLD3;_LLE0: if(*((int*)_tmp111)!= 24)goto _LLE2;_tmp11E=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp111)->f1;_tmp11F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp111)->f2;_LLE1:{void*_tmp13B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp11E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13C;void*_tmp13D;
_LL100: if(_tmp13B <= (void*)4)goto _LL102;if(*((int*)_tmp13B)!= 4)goto _LL102;
_tmp13C=((struct Cyc_Absyn_PointerType_struct*)_tmp13B)->f1;_tmp13D=(void*)
_tmp13C.elt_typ;_LL101:{void*_tmp13E=Cyc_Tcutil_compress(_tmp13D);struct Cyc_Absyn_AggrInfo
_tmp13F;union Cyc_Absyn_AggrInfoU_union _tmp140;struct Cyc_Absyn_Aggrdecl**_tmp141;
struct Cyc_List_List*_tmp142;_LL105: if(_tmp13E <= (void*)4)goto _LL109;if(*((int*)
_tmp13E)!= 10)goto _LL107;_tmp13F=((struct Cyc_Absyn_AggrType_struct*)_tmp13E)->f1;
_tmp140=_tmp13F.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp13E)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL107;_tmp141=(_tmp140.KnownAggr).f1;_LL106: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp141 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp141,_tmp11F);if(sf == 0  || !(sf->tq).real_const)return;goto _LL104;}_LL107: if(*((
int*)_tmp13E)!= 11)goto _LL109;_tmp142=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp13E)->f2;_LL108: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp142,_tmp11F);if(sf == 0  || !(sf->tq).real_const)return;goto _LL104;}_LL109:;
_LL10A: goto _LL104;_LL104:;}goto _LLFF;_LL102:;_LL103: goto _LLFF;_LLFF:;}goto _LLD3;
_LLE2: if(*((int*)_tmp111)!= 22)goto _LLE4;_tmp120=((struct Cyc_Absyn_Deref_e_struct*)
_tmp111)->f1;_LLE3:{void*_tmp143=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp120->topt))->v);struct Cyc_Absyn_PtrInfo _tmp144;struct Cyc_Absyn_Tqual
_tmp145;struct Cyc_Absyn_ArrayInfo _tmp146;struct Cyc_Absyn_Tqual _tmp147;_LL10C: if(
_tmp143 <= (void*)4)goto _LL110;if(*((int*)_tmp143)!= 4)goto _LL10E;_tmp144=((
struct Cyc_Absyn_PointerType_struct*)_tmp143)->f1;_tmp145=_tmp144.elt_tq;_LL10D:
_tmp147=_tmp145;goto _LL10F;_LL10E: if(*((int*)_tmp143)!= 7)goto _LL110;_tmp146=((
struct Cyc_Absyn_ArrayType_struct*)_tmp143)->f1;_tmp147=_tmp146.tq;_LL10F: if(!
_tmp147.real_const)return;goto _LL10B;_LL110:;_LL111: goto _LL10B;_LL10B:;}goto
_LLD3;_LLE4: if(*((int*)_tmp111)!= 13)goto _LLE6;_tmp121=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp111)->f1;_LLE5: _tmp122=_tmp121;goto _LLE7;_LLE6: if(*((int*)_tmp111)!= 14)goto
_LLE8;_tmp122=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp111)->f1;_LLE7: Cyc_Tcexp_check_writable(
te,_tmp122);return;_LLE8:;_LLE9: goto _LLD3;_LLD3:;}({struct Cyc_String_pa_struct
_tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp148[1]={& _tmp14A};Cyc_Tcutil_terr(e->loc,({
const char*_tmp149="attempt to write a const location: %s";_tag_dynforward(
_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,38));}),_tag_dynforward(_tmp148,
sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp14B[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp14C="increment/decrement of non-lvalue";
_tag_dynforward(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,34));}),
_tag_dynforward(_tmp14B,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt,& forward_only) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (
void*)0  || i == (void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(
void*)1))({void*_tmp14D[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp14E="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,50));}),
_tag_dynforward(_tmp14D,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t))({
void*_tmp14F[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp150="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,48));}),
_tag_dynforward(_tmp14F,sizeof(void*),0));});if(forward_only  && (i == (void*)2
 || i == (void*)3))({void*_tmp151[0]={};Cyc_Tcutil_terr(e->loc,({const char*
_tmp152="can't subtract from forward-only ? pointer";_tag_dynforward(_tmp152,
sizeof(char),_get_zero_arr_size(_tmp152,43));}),_tag_dynforward(_tmp151,sizeof(
void*),0));});}else{({struct Cyc_String_pa_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp153[1]={& _tmp155};Cyc_Tcutil_terr(e->loc,({const char*_tmp154="expecting arithmetic or ? type but found %s";
_tag_dynforward(_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,44));}),
_tag_dynforward(_tmp153,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp156="conditional expression";_tag_dynforward(_tmp156,sizeof(char),
_get_zero_arr_size(_tmp156,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_List_List _tmp157=({struct Cyc_List_List _tmp15E;_tmp15E.hd=e3;
_tmp15E.tl=0;_tmp15E;});struct Cyc_List_List _tmp158=({struct Cyc_List_List _tmp15D;
_tmp15D.hd=e2;_tmp15D.tl=(struct Cyc_List_List*)& _tmp157;_tmp15D;});if(!Cyc_Tcutil_coerce_list(
te,t,(struct Cyc_List_List*)& _tmp158)){({struct Cyc_String_pa_struct _tmp15C;
_tmp15C.tag=0;_tmp15C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{
void*_tmp159[2]={& _tmp15B,& _tmp15C};Cyc_Tcutil_terr(loc,({const char*_tmp15A="conditional clause types do not match: %s != %s";
_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,48));}),
_tag_dynforward(_tmp159,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp15F="logical-and expression";_tag_dynforward(_tmp15F,sizeof(char),
_get_zero_arr_size(_tmp15F,23));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp160="logical-and expression";
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,23));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp161="logical-or expression";_tag_dynforward(_tmp161,sizeof(char),
_get_zero_arr_size(_tmp161,22));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp162="logical-or expression";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,22));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){{struct _RegionHandle _tmp163=_new_region("r");struct
_RegionHandle*r=& _tmp163;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(
r,te),0,e1);;_pop_region(r);}Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp164=Cyc_Tcutil_compress(t1);_LL113: if(_tmp164 <= (void*)4)goto _LL115;if(*((
int*)_tmp164)!= 7)goto _LL115;_LL114:({void*_tmp165[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp166="cannot assign to an array";_tag_dynforward(_tmp166,sizeof(
char),_get_zero_arr_size(_tmp166,26));}),_tag_dynforward(_tmp165,sizeof(void*),0));});
goto _LL112;_LL115:;_LL116: goto _LL112;_LL112:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp167[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp168="type is abstract (can't determine size).";
_tag_dynforward(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,41));}),
_tag_dynforward(_tmp167,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp169[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp16A="assignment to non-lvalue";
_tag_dynforward(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,25));}),
_tag_dynforward(_tmp169,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(
po == 0){if(Cyc_Tcutil_is_unique_pointer(t2) && !Cyc_Tcutil_is_unique_path(e2))({
void*_tmp16B[0]={};Cyc_Tcutil_terr(e2->loc,({const char*_tmp16C="Cannot consume paths; do swap instead";
_tag_dynforward(_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,38));}),
_tag_dynforward(_tmp16B,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_assign(te,e2,
t1)){void*_tmp16D=({struct Cyc_String_pa_struct _tmp171;_tmp171.tag=0;_tmp171.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp170;_tmp170.tag=0;_tmp170.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp16E[2]={& _tmp170,& _tmp171};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp16F="type mismatch: %s != %s";_tag_dynforward(_tmp16F,sizeof(char),
_get_zero_arr_size(_tmp16F,24));}),_tag_dynforward(_tmp16E,sizeof(void*),2));}}});
Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();return _tmp16D;}}else{void*
_tmp172=(void*)po->v;void*_tmp173=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp172,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp173,t1) || Cyc_Tcutil_coerceable(_tmp173) && Cyc_Tcutil_coerceable(
t1))){void*_tmp174=({struct Cyc_String_pa_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp175[2]={& _tmp177,& _tmp178};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp176="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dynforward(_tmp176,sizeof(char),_get_zero_arr_size(_tmp176,82));}),
_tag_dynforward(_tmp175,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp173,t1);Cyc_Tcutil_explain_failure();
return _tmp174;}return _tmp173;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp179=Cyc_Tcutil_compress(
t1);void*_tmp17A;void*_tmp17B;_LL118: if((int)_tmp179 != 1)goto _LL11A;_LL119: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL117;_LL11A: if(_tmp179 <= (void*)4)goto
_LL11E;if(*((int*)_tmp179)!= 5)goto _LL11C;_tmp17A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp179)->f2;if((int)_tmp17A != 0)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(*((int*)
_tmp179)!= 5)goto _LL11E;_tmp17B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp179)->f2;
if((int)_tmp17B != 1)goto _LL11E;_LL11D: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL117;_LL11E:;_LL11F: goto _LL117;_LL117:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp17D;struct Cyc_List_List*
_tmp17E;struct Cyc_Position_Segment*_tmp17F;void*_tmp180;struct Cyc_Absyn_Tunionfield
_tmp17C=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp17D=_tmp17C.name;_tmp17E=
_tmp17C.typs;_tmp17F=_tmp17C.loc;_tmp180=(void*)_tmp17C.sc;if(_tmp17E == 0  || 
_tmp17E->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp17E->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_Position_Segment*
_tmp184;void*_tmp185;struct Cyc_Absyn_Tunionfield _tmp181=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp182=_tmp181.name;_tmp183=_tmp181.typs;_tmp184=_tmp181.loc;
_tmp185=(void*)_tmp181.sc;if(_tmp183 == 0  || _tmp183->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp183->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp189;_tmp189.tag=0;_tmp189.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{
struct Cyc_String_pa_struct _tmp188;_tmp188.tag=0;_tmp188.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(tu));{void*
_tmp186[2]={& _tmp188,& _tmp189};Cyc_Tcutil_terr(e->loc,({const char*_tmp187="can't find a field in %s to inject a value of type %s";
_tag_dynforward(_tmp187,sizeof(char),_get_zero_arr_size(_tmp187,54));}),
_tag_dynforward(_tmp186,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp18B=args;struct _RegionHandle _tmp18C=_new_region("ter");
struct _RegionHandle*ter=& _tmp18C;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp18D=
Cyc_Tcenv_new_block(ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp18D,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp18E=t;struct Cyc_Absyn_PtrInfo
_tmp18F;void*_tmp190;struct Cyc_Absyn_Tqual _tmp191;struct Cyc_Absyn_PtrAtts _tmp192;
void*_tmp193;struct Cyc_Absyn_Conref*_tmp194;struct Cyc_Absyn_Conref*_tmp195;
struct Cyc_Absyn_Conref*_tmp196;_LL121: if(_tmp18E <= (void*)4)goto _LL123;if(*((int*)
_tmp18E)!= 4)goto _LL123;_tmp18F=((struct Cyc_Absyn_PointerType_struct*)_tmp18E)->f1;
_tmp190=(void*)_tmp18F.elt_typ;_tmp191=_tmp18F.elt_tq;_tmp192=_tmp18F.ptr_atts;
_tmp193=(void*)_tmp192.rgn;_tmp194=_tmp192.nullable;_tmp195=_tmp192.bounds;
_tmp196=_tmp192.zero_term;_LL122: Cyc_Tcenv_check_rgn_accessible(_tmp18D,loc,
_tmp193);Cyc_Tcutil_check_nonzero_bound(loc,_tmp195);{void*_tmp197=Cyc_Tcutil_compress(
_tmp190);struct Cyc_Absyn_FnInfo _tmp198;struct Cyc_List_List*_tmp199;struct Cyc_Core_Opt*
_tmp19A;void*_tmp19B;struct Cyc_List_List*_tmp19C;int _tmp19D;struct Cyc_Absyn_VarargInfo*
_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp1A0;_LL126: if(
_tmp197 <= (void*)4)goto _LL128;if(*((int*)_tmp197)!= 8)goto _LL128;_tmp198=((
struct Cyc_Absyn_FnType_struct*)_tmp197)->f1;_tmp199=_tmp198.tvars;_tmp19A=
_tmp198.effect;_tmp19B=(void*)_tmp198.ret_typ;_tmp19C=_tmp198.args;_tmp19D=
_tmp198.c_varargs;_tmp19E=_tmp198.cyc_varargs;_tmp19F=_tmp198.rgn_po;_tmp1A0=
_tmp198.attributes;_LL127: if(topt != 0)Cyc_Tcutil_unify(_tmp19B,*topt);while(
_tmp18B != 0  && _tmp19C != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18B->hd;
void*t2=(*((struct _tuple2*)_tmp19C->hd)).f3;Cyc_Tcexp_tcExp(_tmp18D,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp18D,e1,t2)){({struct Cyc_String_pa_struct
_tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1A3;_tmp1A3.tag=0;
_tmp1A3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp1A1[2]={&
_tmp1A3,& _tmp1A4};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1A2="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dynforward(_tmp1A2,sizeof(char),_get_zero_arr_size(_tmp1A2,57));}),
_tag_dynforward(_tmp1A1,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_unique_pointer(
t2) && !Cyc_Tcutil_is_unique_path(e1))({void*_tmp1A5[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp1A6="Cannot consume paths; do swap instead";_tag_dynforward(
_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,38));}),_tag_dynforward(_tmp1A5,
sizeof(void*),0));});_tmp18B=_tmp18B->tl;_tmp19C=_tmp19C->tl;}{int
args_already_checked=0;{struct Cyc_List_List*a=_tmp1A0;for(0;a != 0;a=a->tl){void*
_tmp1A7=(void*)a->hd;void*_tmp1A8;int _tmp1A9;int _tmp1AA;_LL12B: if(_tmp1A7 <= (
void*)17)goto _LL12D;if(*((int*)_tmp1A7)!= 3)goto _LL12D;_tmp1A8=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1A7)->f1;_tmp1A9=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A7)->f2;_tmp1AA=((struct Cyc_Absyn_Format_att_struct*)_tmp1A7)->f3;_LL12C:{
struct _handler_cons _tmp1AB;_push_handler(& _tmp1AB);{int _tmp1AD=0;if(setjmp(
_tmp1AB.handler))_tmp1AD=1;if(!_tmp1AD){{struct Cyc_Absyn_Exp*_tmp1AE=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1A9 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp1AA == 0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp1AF=
_cycalloc(sizeof(*_tmp1AF));_tmp1AF->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp1AA - 1);_tmp1AF;});}args_already_checked=1;{
struct _RegionHandle _tmp1B0=_new_region("temp");struct _RegionHandle*temp=& _tmp1B0;
_push_region(temp);{void*_tmp1B1=_tmp1A8;_LL130: if((int)_tmp1B1 != 0)goto _LL132;
_LL131: Cyc_Tcexp_check_format_args(_tmp18D,_tmp1AE,fmt_args,temp,Cyc_Formatstr_get_format_typs);
goto _LL12F;_LL132: if((int)_tmp1B1 != 1)goto _LL12F;_LL133: Cyc_Tcexp_check_format_args(
_tmp18D,_tmp1AE,fmt_args,temp,Cyc_Formatstr_get_scanf_typs);goto _LL12F;_LL12F:;};
_pop_region(temp);}};_pop_handler();}else{void*_tmp1AC=(void*)_exn_thrown;void*
_tmp1B3=_tmp1AC;_LL135: if(_tmp1B3 != Cyc_List_Nth)goto _LL137;_LL136:({void*
_tmp1B4[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1B5="bad format arguments";
_tag_dynforward(_tmp1B5,sizeof(char),_get_zero_arr_size(_tmp1B5,21));}),
_tag_dynforward(_tmp1B4,sizeof(void*),0));});goto _LL134;_LL137:;_LL138:(void)
_throw(_tmp1B3);_LL134:;}}}goto _LL12A;_LL12D:;_LL12E: goto _LL12A;_LL12A:;}}if(
_tmp19C != 0)({void*_tmp1B6[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1B7="too few arguments for function";
_tag_dynforward(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,31));}),
_tag_dynforward(_tmp1B6,sizeof(void*),0));});else{if((_tmp18B != 0  || _tmp19D)
 || _tmp19E != 0){if(_tmp19D)for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){Cyc_Tcexp_tcExp(
_tmp18D,0,(struct Cyc_Absyn_Exp*)_tmp18B->hd);}else{if(_tmp19E == 0)({void*_tmp1B8[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1B9="too many arguments for function";
_tag_dynforward(_tmp1B9,sizeof(char),_get_zero_arr_size(_tmp1B9,32));}),
_tag_dynforward(_tmp1B8,sizeof(void*),0));});else{void*_tmp1BB;int _tmp1BC;struct
Cyc_Absyn_VarargInfo _tmp1BA=*_tmp19E;_tmp1BB=(void*)_tmp1BA.type;_tmp1BC=_tmp1BA.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp1BD=({struct Cyc_Absyn_VarargCallInfo*_tmp1D9=
_cycalloc(sizeof(*_tmp1D9));_tmp1D9->num_varargs=0;_tmp1D9->injectors=0;_tmp1D9->vai=(
struct Cyc_Absyn_VarargInfo*)_tmp19E;_tmp1D9;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp1BD;if(!_tmp1BC)for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp18B->hd;_tmp1BD->num_varargs ++;Cyc_Tcexp_tcExp(
_tmp18D,(void**)& _tmp1BB,e1);if(!Cyc_Tcutil_coerce_arg(_tmp18D,e1,_tmp1BB)){({
struct Cyc_String_pa_struct _tmp1C1;_tmp1C1.tag=0;_tmp1C1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v));{struct Cyc_String_pa_struct _tmp1C0;
_tmp1C0.tag=0;_tmp1C0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1BB));{void*_tmp1BE[2]={& _tmp1C0,& _tmp1C1};Cyc_Tcutil_terr(loc,({const char*
_tmp1BF="vararg requires type %s but argument has type %s";_tag_dynforward(
_tmp1BF,sizeof(char),_get_zero_arr_size(_tmp1BF,49));}),_tag_dynforward(_tmp1BE,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_unique_pointer(
_tmp1BB) && !Cyc_Tcutil_is_unique_path(e1))({void*_tmp1C2[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmp1C3="Cannot consume paths; do swap instead";
_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,38));}),
_tag_dynforward(_tmp1C2,sizeof(void*),0));});}else{void*_tmp1C4=Cyc_Tcutil_compress(
_tmp1BB);struct Cyc_Absyn_TunionInfo _tmp1C5;union Cyc_Absyn_TunionInfoU_union
_tmp1C6;struct Cyc_Absyn_Tuniondecl**_tmp1C7;struct Cyc_Absyn_Tuniondecl*_tmp1C8;
struct Cyc_List_List*_tmp1C9;struct Cyc_Core_Opt*_tmp1CA;_LL13A: if(_tmp1C4 <= (void*)
4)goto _LL13C;if(*((int*)_tmp1C4)!= 2)goto _LL13C;_tmp1C5=((struct Cyc_Absyn_TunionType_struct*)
_tmp1C4)->f1;_tmp1C6=_tmp1C5.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp1C4)->f1).tunion_info).KnownTunion).tag != 1)goto _LL13C;_tmp1C7=(_tmp1C6.KnownTunion).f1;
_tmp1C8=*_tmp1C7;_tmp1C9=_tmp1C5.targs;_tmp1CA=_tmp1C5.rgn;_LL13B: {struct Cyc_Absyn_Tuniondecl*
_tmp1CB=*Cyc_Tcenv_lookup_tuniondecl(_tmp18D,loc,_tmp1C8->name);struct Cyc_List_List*
fields=0;if(_tmp1CB->fields == 0)({struct Cyc_String_pa_struct _tmp1CE;_tmp1CE.tag=
0;_tmp1CE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1BB));{void*_tmp1CC[1]={& _tmp1CE};Cyc_Tcutil_terr(loc,({const char*_tmp1CD="can't inject into %s";
_tag_dynforward(_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,21));}),
_tag_dynforward(_tmp1CC,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CB->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CA))->v,Cyc_Tcenv_curr_rgn(_tmp18D)))({void*
_tmp1CF[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1D0="bad region for injected varargs";
_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,32));}),
_tag_dynforward(_tmp1CF,sizeof(void*),0));});{struct _RegionHandle _tmp1D1=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp1D1;_push_region(rgn);{struct Cyc_List_List*
_tmp1D2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1CB->tvs,
_tmp1C9);for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){_tmp1BD->num_varargs ++;{struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18B->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(
_tmp18D,0,e1);if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v) && !Cyc_Tcutil_is_unique_path(e1))({void*_tmp1D3[0]={};
Cyc_Tcutil_terr(e1->loc,({const char*_tmp1D4="Cannot consume paths; do swap instead";
_tag_dynforward(_tmp1D4,sizeof(char),_get_zero_arr_size(_tmp1D4,38));}),
_tag_dynforward(_tmp1D3,sizeof(void*),0));});}{struct Cyc_Absyn_Tunionfield*
_tmp1D5=Cyc_Tcexp_tcInjection(_tmp18D,e1,_tmp1BB,rgn,_tmp1D2,fields);if(_tmp1D5
!= 0)_tmp1BD->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp1BD->injectors,({struct Cyc_List_List*
_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->hd=(struct Cyc_Absyn_Tunionfield*)
_tmp1D5;_tmp1D6->tl=0;_tmp1D6;}));}}}};_pop_region(rgn);}goto _LL139;}_LL13C:;
_LL13D:({void*_tmp1D7[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1D8="bad inject vararg type";
_tag_dynforward(_tmp1D8,sizeof(char),_get_zero_arr_size(_tmp1D8,23));}),
_tag_dynforward(_tmp1D7,sizeof(void*),0));});goto _LL139;_LL139:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp18D,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp19A))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp18D,loc,_tmp19F);{void*_tmp1DA=_tmp19B;_npop_handler(0);return _tmp1DA;}}
_LL128:;_LL129: {void*_tmp1DD=({void*_tmp1DB[0]={};Cyc_Tcexp_expr_err(_tmp18D,
loc,topt,({const char*_tmp1DC="expected pointer to function";_tag_dynforward(
_tmp1DC,sizeof(char),_get_zero_arr_size(_tmp1DC,29));}),_tag_dynforward(_tmp1DB,
sizeof(void*),0));});_npop_handler(0);return _tmp1DD;}_LL125:;}_LL123:;_LL124: {
void*_tmp1E0=({void*_tmp1DE[0]={};Cyc_Tcexp_expr_err(_tmp18D,loc,topt,({const
char*_tmp1DF="expected pointer to function";_tag_dynforward(_tmp1DF,sizeof(char),
_get_zero_arr_size(_tmp1DF,29));}),_tag_dynforward(_tmp1DE,sizeof(void*),0));});
_npop_handler(0);return _tmp1E0;}_LL120:;}};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,
e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct _tmp1E3;_tmp1E3.tag=0;_tmp1E3.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1E1[1]={& _tmp1E3};Cyc_Tcutil_terr(
loc,({const char*_tmp1E2="expected xtunion exn but found %s";_tag_dynforward(
_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,34));}),_tag_dynforward(_tmp1E1,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1E4=t1;struct Cyc_Absyn_PtrInfo
_tmp1E5;void*_tmp1E6;struct Cyc_Absyn_Tqual _tmp1E7;struct Cyc_Absyn_PtrAtts _tmp1E8;
void*_tmp1E9;struct Cyc_Absyn_Conref*_tmp1EA;struct Cyc_Absyn_Conref*_tmp1EB;
struct Cyc_Absyn_Conref*_tmp1EC;_LL13F: if(_tmp1E4 <= (void*)4)goto _LL141;if(*((int*)
_tmp1E4)!= 4)goto _LL141;_tmp1E5=((struct Cyc_Absyn_PointerType_struct*)_tmp1E4)->f1;
_tmp1E6=(void*)_tmp1E5.elt_typ;_tmp1E7=_tmp1E5.elt_tq;_tmp1E8=_tmp1E5.ptr_atts;
_tmp1E9=(void*)_tmp1E8.rgn;_tmp1EA=_tmp1E8.nullable;_tmp1EB=_tmp1E8.bounds;
_tmp1EC=_tmp1E8.zero_term;_LL140:{void*_tmp1ED=Cyc_Tcutil_compress(_tmp1E6);
struct Cyc_Absyn_FnInfo _tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_Core_Opt*
_tmp1F0;void*_tmp1F1;struct Cyc_List_List*_tmp1F2;int _tmp1F3;struct Cyc_Absyn_VarargInfo*
_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_List_List*_tmp1F6;_LL144: if(
_tmp1ED <= (void*)4)goto _LL146;if(*((int*)_tmp1ED)!= 8)goto _LL146;_tmp1EE=((
struct Cyc_Absyn_FnType_struct*)_tmp1ED)->f1;_tmp1EF=_tmp1EE.tvars;_tmp1F0=
_tmp1EE.effect;_tmp1F1=(void*)_tmp1EE.ret_typ;_tmp1F2=_tmp1EE.args;_tmp1F3=
_tmp1EE.c_varargs;_tmp1F4=_tmp1EE.cyc_varargs;_tmp1F5=_tmp1EE.rgn_po;_tmp1F6=
_tmp1EE.attributes;_LL145: {struct _RegionHandle _tmp1F7=_new_region("temp");
struct _RegionHandle*temp=& _tmp1F7;_push_region(temp);{struct Cyc_List_List*
instantiation=0;for(0;ts != 0  && _tmp1EF != 0;(ts=ts->tl,_tmp1EF=_tmp1EF->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1EF->hd);Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);instantiation=({struct
Cyc_List_List*_tmp1F8=_region_malloc(temp,sizeof(*_tmp1F8));_tmp1F8->hd=({struct
_tuple5*_tmp1F9=_region_malloc(temp,sizeof(*_tmp1F9));_tmp1F9->f1=(struct Cyc_Absyn_Tvar*)
_tmp1EF->hd;_tmp1F9->f2=(void*)ts->hd;_tmp1F9;});_tmp1F8->tl=instantiation;
_tmp1F8;});}if(ts != 0){void*_tmp1FC=({void*_tmp1FA[0]={};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp1FB="too many type variables in instantiation";
_tag_dynforward(_tmp1FB,sizeof(char),_get_zero_arr_size(_tmp1FB,41));}),
_tag_dynforward(_tmp1FA,sizeof(void*),0));});_npop_handler(0);return _tmp1FC;}{
void*new_fn_typ=Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202[0]=({struct Cyc_Absyn_FnType_struct
_tmp203;_tmp203.tag=8;_tmp203.f1=({struct Cyc_Absyn_FnInfo _tmp204;_tmp204.tvars=
_tmp1EF;_tmp204.effect=_tmp1F0;_tmp204.ret_typ=(void*)_tmp1F1;_tmp204.args=
_tmp1F2;_tmp204.c_varargs=_tmp1F3;_tmp204.cyc_varargs=_tmp1F4;_tmp204.rgn_po=
_tmp1F5;_tmp204.attributes=_tmp1F6;_tmp204;});_tmp203;});_tmp202;}));void*
_tmp201=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp1FD=_cycalloc(sizeof(*
_tmp1FD));_tmp1FD[0]=({struct Cyc_Absyn_PointerType_struct _tmp1FE;_tmp1FE.tag=4;
_tmp1FE.f1=({struct Cyc_Absyn_PtrInfo _tmp1FF;_tmp1FF.elt_typ=(void*)new_fn_typ;
_tmp1FF.elt_tq=_tmp1E7;_tmp1FF.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp200;_tmp200.rgn=(
void*)_tmp1E9;_tmp200.nullable=_tmp1EA;_tmp200.bounds=_tmp1EB;_tmp200.zero_term=
_tmp1EC;_tmp200.ptrloc=0;_tmp200;});_tmp1FF;});_tmp1FE;});_tmp1FD;});
_npop_handler(0);return _tmp201;}};_pop_region(temp);}_LL146:;_LL147: goto _LL143;
_LL143:;}goto _LL13E;_LL141:;_LL142: goto _LL13E;_LL13E:;}return({struct Cyc_String_pa_struct
_tmp207;_tmp207.tag=0;_tmp207.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp205[1]={& _tmp207};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp206="expecting polymorphic type but found %s";
_tag_dynforward(_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,40));}),
_tag_dynforward(_tmp205,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp208=({
struct Cyc_String_pa_struct _tmp20C;_tmp20C.tag=0;_tmp20C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp20B;_tmp20B.tag=0;_tmp20B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp209[2]={& _tmp20B,& _tmp20C};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp20A="cannot cast %s to %s";_tag_dynforward(
_tmp20A,sizeof(char),_get_zero_arr_size(_tmp20A,21));}),_tag_dynforward(_tmp209,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp208;}}}}{struct
_tuple0 _tmp20E=({struct _tuple0 _tmp20D;_tmp20D.f1=(void*)e->r;_tmp20D.f2=Cyc_Tcutil_compress(
t);_tmp20D;});void*_tmp20F;struct Cyc_Absyn_MallocInfo _tmp210;int _tmp211;void*
_tmp212;struct Cyc_Absyn_PtrInfo _tmp213;struct Cyc_Absyn_PtrAtts _tmp214;struct Cyc_Absyn_Conref*
_tmp215;struct Cyc_Absyn_Conref*_tmp216;struct Cyc_Absyn_Conref*_tmp217;_LL149:
_tmp20F=_tmp20E.f1;if(*((int*)_tmp20F)!= 35)goto _LL14B;_tmp210=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp20F)->f1;_tmp211=_tmp210.fat_result;_tmp212=_tmp20E.f2;if(_tmp212 <= (void*)4)
goto _LL14B;if(*((int*)_tmp212)!= 4)goto _LL14B;_tmp213=((struct Cyc_Absyn_PointerType_struct*)
_tmp212)->f1;_tmp214=_tmp213.ptr_atts;_tmp215=_tmp214.nullable;_tmp216=_tmp214.bounds;
_tmp217=_tmp214.zero_term;_LL14A: if((_tmp211  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp217)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp215)){void*_tmp218=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp216);struct Cyc_Absyn_Exp*_tmp219;_LL14E: if(_tmp218 <= (void*)2)goto _LL150;if(*((
int*)_tmp218)!= 0)goto _LL150;_tmp219=((struct Cyc_Absyn_Upper_b_struct*)_tmp218)->f1;
_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp219)).f1 == 1)({void*_tmp21A[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp21B="cast from ? pointer to * pointer will lose size information";
_tag_dynforward(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,60));}),
_tag_dynforward(_tmp21A,sizeof(void*),0));});goto _LL14D;_LL150:;_LL151: goto
_LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto _LL148;_LL148:;}return t;}}static
void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp21C=0;struct
Cyc_Absyn_Tqual _tmp21D=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp21E=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp21F;void*_tmp220;struct Cyc_Absyn_Tqual _tmp221;
struct Cyc_Absyn_PtrAtts _tmp222;struct Cyc_Absyn_Conref*_tmp223;_LL153: if(_tmp21E
<= (void*)4)goto _LL155;if(*((int*)_tmp21E)!= 4)goto _LL155;_tmp21F=((struct Cyc_Absyn_PointerType_struct*)
_tmp21E)->f1;_tmp220=(void*)_tmp21F.elt_typ;_tmp221=_tmp21F.elt_tq;_tmp222=
_tmp21F.ptr_atts;_tmp223=_tmp222.zero_term;_LL154: _tmp21C=({void**_tmp224=
_cycalloc(sizeof(*_tmp224));_tmp224[0]=_tmp220;_tmp224;});_tmp21D=_tmp221;goto
_LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{struct _RegionHandle _tmp225=
_new_region("r");struct _RegionHandle*r=& _tmp225;_push_region(r);Cyc_Tcexp_tcExpNoInst(
Cyc_Tcenv_clear_notreadctxt(r,te),_tmp21C,e);;_pop_region(r);}{void*_tmp226=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp227;struct Cyc_Absyn_Exp*_tmp228;_LL158: if(*((
int*)_tmp226)!= 25)goto _LL15A;_tmp227=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp226)->f1;_tmp228=((struct Cyc_Absyn_Subscript_e_struct*)_tmp226)->f2;_LL159:{
void*_tmp229=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp227->topt))->v);_LL15D: if(_tmp229 <= (void*)4)goto _LL15F;if(*((int*)_tmp229)
!= 9)goto _LL15F;_LL15E: goto _LL15C;_LL15F:;_LL160:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp227,_tmp228,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp227,
_tmp228);_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{int _tmp22B;void*
_tmp22C;struct _tuple6 _tmp22A=Cyc_Tcutil_addressof_props(te,e);_tmp22B=_tmp22A.f1;
_tmp22C=_tmp22A.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp22B){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp22C,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({struct Cyc_String_pa_struct _tmp22F;
_tmp22F.tag=0;_tmp22F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp22D[1]={& _tmp22F};Cyc_Tcutil_terr(loc,({const char*_tmp22E="sizeof applied to type %s, which has unknown size here";
_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,55));}),
_tag_dynforward(_tmp22D,sizeof(void*),1));}});if(topt != 0){void*_tmp230=Cyc_Tcutil_compress(*
topt);_LL162: if(_tmp230 <= (void*)4)goto _LL164;if(*((int*)_tmp230)!= 14)goto
_LL164;_LL163: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp231=_cycalloc(
sizeof(*_tmp231));_tmp231[0]=({struct Cyc_Absyn_SizeofType_struct _tmp232;_tmp232.tag=
14;_tmp232.f1=(void*)t;_tmp232;});_tmp231;});_LL164:;_LL165: goto _LL161;_LL161:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct
_dynforward_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dynforward_ptr)*n,(struct _dynforward_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp233=n;struct _dynforward_ptr*_tmp234;unsigned int _tmp235;_LL167: if(*((int*)
_tmp233)!= 0)goto _LL169;_tmp234=((struct Cyc_Absyn_StructField_struct*)_tmp233)->f1;
_LL168: {int bad_type=1;{void*_tmp236=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp237;union Cyc_Absyn_AggrInfoU_union _tmp238;struct Cyc_Absyn_Aggrdecl**_tmp239;
struct Cyc_List_List*_tmp23A;_LL16C: if(_tmp236 <= (void*)4)goto _LL170;if(*((int*)
_tmp236)!= 10)goto _LL16E;_tmp237=((struct Cyc_Absyn_AggrType_struct*)_tmp236)->f1;
_tmp238=_tmp237.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp236)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL16E;_tmp239=(_tmp238.KnownAggr).f1;_LL16D: if((*_tmp239)->impl == 0)
goto _LL16B;if(!((int(*)(int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp234,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp239)->impl))->fields))({
struct Cyc_String_pa_struct _tmp23D;_tmp23D.tag=0;_tmp23D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp234);{void*_tmp23B[1]={& _tmp23D};
Cyc_Tcutil_terr(loc,({const char*_tmp23C="no field of struct/union has name %s";
_tag_dynforward(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,37));}),
_tag_dynforward(_tmp23B,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL16E: if(*((
int*)_tmp236)!= 11)goto _LL170;_tmp23A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp236)->f2;_LL16F: if(!((int(*)(int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp234,_tmp23A))({struct Cyc_String_pa_struct _tmp240;_tmp240.tag=0;_tmp240.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp234);{void*_tmp23E[1]={&
_tmp240};Cyc_Tcutil_terr(loc,({const char*_tmp23F="no field of struct/union has name %s";
_tag_dynforward(_tmp23F,sizeof(char),_get_zero_arr_size(_tmp23F,37));}),
_tag_dynforward(_tmp23E,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL170:;
_LL171: goto _LL16B;_LL16B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp243;
_tmp243.tag=0;_tmp243.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp241[1]={& _tmp243};Cyc_Tcutil_terr(loc,({const char*_tmp242="%s is not a known struct/union type";
_tag_dynforward(_tmp242,sizeof(char),_get_zero_arr_size(_tmp242,36));}),
_tag_dynforward(_tmp241,sizeof(void*),1));}});goto _LL166;}_LL169: if(*((int*)
_tmp233)!= 1)goto _LL166;_tmp235=((struct Cyc_Absyn_TupleIndex_struct*)_tmp233)->f1;
_LL16A: {int bad_type=1;{void*_tmp244=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp245;union Cyc_Absyn_AggrInfoU_union _tmp246;struct Cyc_Absyn_Aggrdecl**_tmp247;
struct Cyc_List_List*_tmp248;struct Cyc_List_List*_tmp249;struct Cyc_Absyn_TunionFieldInfo
_tmp24A;union Cyc_Absyn_TunionFieldInfoU_union _tmp24B;struct Cyc_Absyn_Tunionfield*
_tmp24C;_LL173: if(_tmp244 <= (void*)4)goto _LL17B;if(*((int*)_tmp244)!= 10)goto
_LL175;_tmp245=((struct Cyc_Absyn_AggrType_struct*)_tmp244)->f1;_tmp246=_tmp245.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp244)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL175;_tmp247=(_tmp246.KnownAggr).f1;_LL174: if((*_tmp247)->impl == 0)
goto _LL172;_tmp248=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp247)->impl))->fields;
goto _LL176;_LL175: if(*((int*)_tmp244)!= 11)goto _LL177;_tmp248=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp244)->f2;_LL176: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp248)
<= _tmp235)({struct Cyc_Int_pa_struct _tmp250;_tmp250.tag=1;_tmp250.f1=(
unsigned long)((int)_tmp235);{struct Cyc_Int_pa_struct _tmp24F;_tmp24F.tag=1;
_tmp24F.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp248);{void*_tmp24D[2]={& _tmp24F,& _tmp250};Cyc_Tcutil_terr(loc,({const char*
_tmp24E="struct/union has too few components: %d <= %d";_tag_dynforward(_tmp24E,
sizeof(char),_get_zero_arr_size(_tmp24E,46));}),_tag_dynforward(_tmp24D,sizeof(
void*),2));}}});bad_type=0;goto _LL172;_LL177: if(*((int*)_tmp244)!= 9)goto _LL179;
_tmp249=((struct Cyc_Absyn_TupleType_struct*)_tmp244)->f1;_LL178: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp249)<= _tmp235)({struct Cyc_Int_pa_struct
_tmp254;_tmp254.tag=1;_tmp254.f1=(unsigned long)((int)_tmp235);{struct Cyc_Int_pa_struct
_tmp253;_tmp253.tag=1;_tmp253.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp249);{void*_tmp251[2]={& _tmp253,& _tmp254};Cyc_Tcutil_terr(
loc,({const char*_tmp252="tuple has too few components: %d <= %d";_tag_dynforward(
_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,39));}),_tag_dynforward(_tmp251,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL179: if(*((int*)_tmp244)!= 3)goto
_LL17B;_tmp24A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp244)->f1;_tmp24B=
_tmp24A.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp244)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL17B;_tmp24C=(_tmp24B.KnownTunionfield).f2;_LL17A: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp24C->typs)< _tmp235)({struct Cyc_Int_pa_struct
_tmp258;_tmp258.tag=1;_tmp258.f1=(unsigned long)((int)_tmp235);{struct Cyc_Int_pa_struct
_tmp257;_tmp257.tag=1;_tmp257.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp24C->typs);{void*_tmp255[2]={& _tmp257,& _tmp258};Cyc_Tcutil_terr(
loc,({const char*_tmp256="tunionfield has too few components: %d < %d";
_tag_dynforward(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,44));}),
_tag_dynforward(_tmp255,sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL17B:;
_LL17C: goto _LL172;_LL172:;}if(bad_type)({struct Cyc_String_pa_struct _tmp25B;
_tmp25B.tag=0;_tmp25B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp259[1]={& _tmp25B};Cyc_Tcutil_terr(loc,({const char*_tmp25A="%s is not a known type";
_tag_dynforward(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,23));}),
_tag_dynforward(_tmp259,sizeof(void*),1));}});goto _LL166;}_LL166:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle _tmp25C=_new_region("r");
struct _RegionHandle*r=& _tmp25C;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp25D=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp25D,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp25E=t;struct Cyc_Absyn_PtrInfo
_tmp25F;void*_tmp260;struct Cyc_Absyn_PtrAtts _tmp261;void*_tmp262;struct Cyc_Absyn_Conref*
_tmp263;struct Cyc_Absyn_Conref*_tmp264;_LL17E: if(_tmp25E <= (void*)4)goto _LL180;
if(*((int*)_tmp25E)!= 4)goto _LL180;_tmp25F=((struct Cyc_Absyn_PointerType_struct*)
_tmp25E)->f1;_tmp260=(void*)_tmp25F.elt_typ;_tmp261=_tmp25F.ptr_atts;_tmp262=(
void*)_tmp261.rgn;_tmp263=_tmp261.bounds;_tmp264=_tmp261.zero_term;_LL17F: Cyc_Tcenv_check_rgn_accessible(
_tmp25D,loc,_tmp262);Cyc_Tcutil_check_nonzero_bound(loc,_tmp263);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp260),(void*)1))({void*_tmp265[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp266="can't dereference abstract pointer type";_tag_dynforward(
_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,40));}),_tag_dynforward(_tmp265,
sizeof(void*),0));});{void*_tmp267=_tmp260;_npop_handler(0);return _tmp267;}
_LL180:;_LL181: {void*_tmp26B=({struct Cyc_String_pa_struct _tmp26A;_tmp26A.tag=0;
_tmp26A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp268[1]={& _tmp26A};Cyc_Tcexp_expr_err(_tmp25D,loc,topt,({const char*
_tmp269="expecting * or @ type but found %s";_tag_dynforward(_tmp269,sizeof(char),
_get_zero_arr_size(_tmp269,35));}),_tag_dynforward(_tmp268,sizeof(void*),1));}});
_npop_handler(0);return _tmp26B;}_LL17D:;}};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*f){Cyc_Tcexp_tcExpNoPromote(
te,0,e);{void*_tmp26C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp26D;union Cyc_Absyn_AggrInfoU_union
_tmp26E;struct Cyc_Absyn_Aggrdecl**_tmp26F;struct Cyc_Absyn_Aggrdecl*_tmp270;
struct Cyc_List_List*_tmp271;void*_tmp272;struct Cyc_List_List*_tmp273;_LL183: if(
_tmp26C <= (void*)4)goto _LL18B;if(*((int*)_tmp26C)!= 10)goto _LL185;_tmp26D=((
struct Cyc_Absyn_AggrType_struct*)_tmp26C)->f1;_tmp26E=_tmp26D.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp26C)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL185;_tmp26F=(_tmp26E.KnownAggr).f1;_tmp270=*_tmp26F;_tmp271=_tmp26D.targs;
_LL184: {struct Cyc_Absyn_Aggrfield*_tmp276=Cyc_Absyn_lookup_decl_field(_tmp270,f);
if(_tmp276 == 0)return({struct Cyc_String_pa_struct _tmp27A;_tmp27A.tag=0;_tmp27A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp279;_tmp279.tag=0;_tmp279.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp270->name));{void*_tmp277[2]={& _tmp279,& _tmp27A};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp278="type %s has no %s field";_tag_dynforward(
_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,24));}),_tag_dynforward(_tmp277,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp270->impl))->exist_vars
!= 0)return({void*_tmp27B[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp27C="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(_tmp27C,64));}),
_tag_dynforward(_tmp27B,sizeof(void*),0));});{void*t2;{struct _RegionHandle
_tmp27D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp27D;_push_region(rgn);{
struct Cyc_List_List*_tmp27E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp270->tvs,_tmp271);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp27E,(void*)
_tmp276->type);if(((void*)_tmp270->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){void*_tmp282=({struct Cyc_String_pa_struct
_tmp281;_tmp281.tag=0;_tmp281.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp27F[1]={& _tmp281};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp280="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,56));}),
_tag_dynforward(_tmp27F,sizeof(void*),1));}});_npop_handler(0);return _tmp282;}};
_pop_region(rgn);}return t2;}}_LL185: if(*((int*)_tmp26C)!= 11)goto _LL187;_tmp272=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp26C)->f1;_tmp273=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp26C)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp283=Cyc_Absyn_lookup_field(
_tmp273,f);if(_tmp283 == 0)return({struct Cyc_String_pa_struct _tmp286;_tmp286.tag=
0;_tmp286.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp284[1]={&
_tmp286};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp285="type has no %s field";
_tag_dynforward(_tmp285,sizeof(char),_get_zero_arr_size(_tmp285,21));}),
_tag_dynforward(_tmp284,sizeof(void*),1));}});if((_tmp272 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only((void*)_tmp283->type))return({struct Cyc_String_pa_struct
_tmp289;_tmp289.tag=0;_tmp289.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp287[1]={& _tmp289};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp288="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,56));}),
_tag_dynforward(_tmp287,sizeof(void*),1));}});return(void*)_tmp283->type;}_LL187:
if(*((int*)_tmp26C)!= 7)goto _LL189;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({
const char*_tmp274="size";_tag_dynforward(_tmp274,sizeof(char),_get_zero_arr_size(
_tmp274,5));}))== 0))goto _LL189;_LL188: goto _LL18A;_LL189: if(*((int*)_tmp26C)!= 4)
goto _LL18B;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({const char*_tmp275="size";
_tag_dynforward(_tmp275,sizeof(char),_get_zero_arr_size(_tmp275,5));}))== 0))
goto _LL18B;_LL18A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp28A=_cycalloc(sizeof(*_tmp28A));_tmp28A[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp28B;_tmp28B.tag=3;_tmp28B.f1=(void*)((void*)19);_tmp28B.f2=({struct Cyc_List_List*
_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->hd=e;_tmp28C->tl=0;_tmp28C;});
_tmp28B;});_tmp28A;})));return Cyc_Absyn_uint_typ;_LL18B:;_LL18C: if(Cyc_strcmp((
struct _dynforward_ptr)*f,({const char*_tmp28D="size";_tag_dynforward(_tmp28D,
sizeof(char),_get_zero_arr_size(_tmp28D,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp290;_tmp290.tag=0;_tmp290.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp28E[1]={& _tmp290};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp28F="expecting struct, union, or array, found %s";
_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,44));}),
_tag_dynforward(_tmp28E,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct
_tmp293;_tmp293.tag=0;_tmp293.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp291[1]={& _tmp293};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp292="expecting struct or union, found %s";
_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,36));}),
_tag_dynforward(_tmp291,sizeof(void*),1));}});}_LL182:;}}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*f){{struct _RegionHandle _tmp294=_new_region("r");struct
_RegionHandle*r=& _tmp294;_push_region(r);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
r,te),0,e);;_pop_region(r);}{void*_tmp295=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp296;void*_tmp297;struct Cyc_Absyn_PtrAtts
_tmp298;void*_tmp299;struct Cyc_Absyn_Conref*_tmp29A;struct Cyc_Absyn_Conref*
_tmp29B;_LL18E: if(_tmp295 <= (void*)4)goto _LL190;if(*((int*)_tmp295)!= 4)goto
_LL190;_tmp296=((struct Cyc_Absyn_PointerType_struct*)_tmp295)->f1;_tmp297=(void*)
_tmp296.elt_typ;_tmp298=_tmp296.ptr_atts;_tmp299=(void*)_tmp298.rgn;_tmp29A=
_tmp298.bounds;_tmp29B=_tmp298.zero_term;_LL18F: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp29A);{void*_tmp29C=Cyc_Tcutil_compress(_tmp297);struct Cyc_Absyn_AggrInfo
_tmp29D;union Cyc_Absyn_AggrInfoU_union _tmp29E;struct Cyc_Absyn_Aggrdecl**_tmp29F;
struct Cyc_Absyn_Aggrdecl*_tmp2A0;struct Cyc_List_List*_tmp2A1;void*_tmp2A2;struct
Cyc_List_List*_tmp2A3;_LL193: if(_tmp29C <= (void*)4)goto _LL197;if(*((int*)_tmp29C)
!= 10)goto _LL195;_tmp29D=((struct Cyc_Absyn_AggrType_struct*)_tmp29C)->f1;_tmp29E=
_tmp29D.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp29C)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL195;_tmp29F=(_tmp29E.KnownAggr).f1;_tmp2A0=*_tmp29F;_tmp2A1=_tmp29D.targs;
_LL194: {struct Cyc_Absyn_Aggrfield*_tmp2A4=Cyc_Absyn_lookup_decl_field(_tmp2A0,f);
if(_tmp2A4 == 0)return({struct Cyc_String_pa_struct _tmp2A8;_tmp2A8.tag=0;_tmp2A8.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp2A7;_tmp2A7.tag=0;_tmp2A7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp2A0->name));{void*_tmp2A5[2]={& _tmp2A7,& _tmp2A8};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2A6="type %s has no %s field";_tag_dynforward(
_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,24));}),_tag_dynforward(_tmp2A5,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A0->impl))->exist_vars
!= 0)return({void*_tmp2A9[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp2AA="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,64));}),
_tag_dynforward(_tmp2A9,sizeof(void*),0));});{void*t3;{struct _RegionHandle
_tmp2AB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2AB;_push_region(rgn);{
struct Cyc_List_List*_tmp2AC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp2A0->tvs,_tmp2A1);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp2AC,(void*)
_tmp2A4->type);};_pop_region(rgn);}if(((void*)_tmp2A0->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t3))return({struct Cyc_String_pa_struct _tmp2AF;
_tmp2AF.tag=0;_tmp2AF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmp2AD[1]={& _tmp2AF};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2AE="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2AE,sizeof(char),_get_zero_arr_size(_tmp2AE,56));}),
_tag_dynforward(_tmp2AD,sizeof(void*),1));}});return t3;}}_LL195: if(*((int*)
_tmp29C)!= 11)goto _LL197;_tmp2A2=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp29C)->f1;_tmp2A3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp29C)->f2;_LL196: {
struct Cyc_Absyn_Aggrfield*_tmp2B0=Cyc_Absyn_lookup_field(_tmp2A3,f);if(_tmp2B0 == 
0)return({struct Cyc_String_pa_struct _tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2B1[1]={& _tmp2B3};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2B2="type has no %s field";_tag_dynforward(_tmp2B2,
sizeof(char),_get_zero_arr_size(_tmp2B2,21));}),_tag_dynforward(_tmp2B1,sizeof(
void*),1));}});if((_tmp2A2 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2B0->type))return({struct Cyc_String_pa_struct _tmp2B6;_tmp2B6.tag=0;
_tmp2B6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2B4[1]={&
_tmp2B6};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2B5="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2B5,sizeof(char),_get_zero_arr_size(_tmp2B5,56));}),
_tag_dynforward(_tmp2B4,sizeof(void*),1));}});return(void*)_tmp2B0->type;}_LL197:;
_LL198: goto _LL192;_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}return({
struct Cyc_String_pa_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2B7[1]={& _tmp2B9};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2B8="expecting struct or union pointer, found %s";
_tag_dynforward(_tmp2B8,sizeof(char),_get_zero_arr_size(_tmp2B8,44));}),
_tag_dynforward(_tmp2B7,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2BB;int _tmp2BC;struct _tuple7 _tmp2BA=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2BB=_tmp2BA.f1;_tmp2BC=_tmp2BA.f2;if(!
_tmp2BC)return({void*_tmp2BD[0]={};Cyc_Tcexp_expr_err(te,loc,0,({const char*
_tmp2BE="tuple projection cannot use sizeof or offsetof";_tag_dynforward(_tmp2BE,
sizeof(char),_get_zero_arr_size(_tmp2BE,47));}),_tag_dynforward(_tmp2BD,sizeof(
void*),0));});{struct _handler_cons _tmp2BF;_push_handler(& _tmp2BF);{int _tmp2C1=0;
if(setjmp(_tmp2BF.handler))_tmp2C1=1;if(!_tmp2C1){{void*_tmp2C2=(*((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2BB)).f2;
_npop_handler(0);return _tmp2C2;};_pop_handler();}else{void*_tmp2C0=(void*)
_exn_thrown;void*_tmp2C4=_tmp2C0;_LL19A: if(_tmp2C4 != Cyc_List_Nth)goto _LL19C;
_LL19B: return({struct Cyc_Int_pa_struct _tmp2C8;_tmp2C8.tag=1;_tmp2C8.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp2C7;_tmp2C7.tag=1;_tmp2C7.f1=(unsigned long)((int)_tmp2BB);{void*_tmp2C5[2]={&
_tmp2C7,& _tmp2C8};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2C6="index is %d but tuple has only %d fields";
_tag_dynforward(_tmp2C6,sizeof(char),_get_zero_arr_size(_tmp2C6,41));}),
_tag_dynforward(_tmp2C5,sizeof(void*),2));}}});_LL19C:;_LL19D:(void)_throw(
_tmp2C4);_LL199:;}}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp2C9=_new_region("r");struct
_RegionHandle*r=& _tmp2C9;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp2CA=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp2CA,0,e1);Cyc_Tcexp_tcExp(_tmp2CA,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2CA,e2)){void*_tmp2CE=({struct Cyc_String_pa_struct
_tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2CB[1]={& _tmp2CD};Cyc_Tcexp_expr_err(
_tmp2CA,e2->loc,topt,({const char*_tmp2CC="expecting int subscript, found %s";
_tag_dynforward(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,34));}),
_tag_dynforward(_tmp2CB,sizeof(void*),1));}});_npop_handler(0);return _tmp2CE;}{
void*_tmp2CF=t1;struct Cyc_Absyn_PtrInfo _tmp2D0;void*_tmp2D1;struct Cyc_Absyn_Tqual
_tmp2D2;struct Cyc_Absyn_PtrAtts _tmp2D3;void*_tmp2D4;struct Cyc_Absyn_Conref*
_tmp2D5;struct Cyc_Absyn_Conref*_tmp2D6;struct Cyc_List_List*_tmp2D7;_LL19F: if(
_tmp2CF <= (void*)4)goto _LL1A3;if(*((int*)_tmp2CF)!= 4)goto _LL1A1;_tmp2D0=((
struct Cyc_Absyn_PointerType_struct*)_tmp2CF)->f1;_tmp2D1=(void*)_tmp2D0.elt_typ;
_tmp2D2=_tmp2D0.elt_tq;_tmp2D3=_tmp2D0.ptr_atts;_tmp2D4=(void*)_tmp2D3.rgn;
_tmp2D5=_tmp2D3.bounds;_tmp2D6=_tmp2D3.zero_term;_LL1A0: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2D6)){int emit_warning=1;{union
Cyc_Absyn_Constraint_union _tmp2D8=(Cyc_Absyn_compress_conref(_tmp2D5))->v;void*
_tmp2D9;struct Cyc_Absyn_Exp*_tmp2DA;void*_tmp2DB;void*_tmp2DC;_LL1A6: if((_tmp2D8.Eq_constr).tag
!= 0)goto _LL1A8;_tmp2D9=(_tmp2D8.Eq_constr).f1;if(_tmp2D9 <= (void*)2)goto _LL1A8;
if(*((int*)_tmp2D9)!= 0)goto _LL1A8;_tmp2DA=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2D9)->f1;_LL1A7: if(Cyc_Tcutil_is_const_exp(_tmp2CA,e2)){unsigned int _tmp2DE;
int _tmp2DF;struct _tuple7 _tmp2DD=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2DE=
_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;if(_tmp2DF){unsigned int _tmp2E1;int _tmp2E2;struct
_tuple7 _tmp2E0=Cyc_Evexp_eval_const_uint_exp(_tmp2DA);_tmp2E1=_tmp2E0.f1;_tmp2E2=
_tmp2E0.f2;if(_tmp2E2  && _tmp2E1 > _tmp2DE)emit_warning=0;}}goto _LL1A5;_LL1A8: if((
_tmp2D8.Eq_constr).tag != 0)goto _LL1AA;_tmp2DB=(_tmp2D8.Eq_constr).f1;if((int)
_tmp2DB != 0)goto _LL1AA;_LL1A9: emit_warning=0;goto _LL1A5;_LL1AA: if((_tmp2D8.Eq_constr).tag
!= 0)goto _LL1AC;_tmp2DC=(_tmp2D8.Eq_constr).f1;if((int)_tmp2DC != 1)goto _LL1AC;
_LL1AB: emit_warning=0;goto _LL1A5;_LL1AC:;_LL1AD: goto _LL1A5;_LL1A5:;}if(
emit_warning)({void*_tmp2E3[0]={};Cyc_Tcutil_warn(e2->loc,({const char*_tmp2E4="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dynforward(_tmp2E4,sizeof(char),_get_zero_arr_size(_tmp2E4,63));}),
_tag_dynforward(_tmp2E3,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(
_tmp2CA,e2)){unsigned int _tmp2E6;int _tmp2E7;struct _tuple7 _tmp2E5=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2E6=_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;if(_tmp2E7)Cyc_Tcutil_check_bound(loc,
_tmp2E6,_tmp2D5);}else{if(Cyc_Tcutil_is_bound_one(_tmp2D5) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2D6))({void*_tmp2E8[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmp2E9="subscript applied to pointer to one object";
_tag_dynforward(_tmp2E9,sizeof(char),_get_zero_arr_size(_tmp2E9,43));}),
_tag_dynforward(_tmp2E8,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,
_tmp2D5);}}Cyc_Tcenv_check_rgn_accessible(_tmp2CA,loc,_tmp2D4);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2D1),(void*)1))({void*_tmp2EA[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp2EB="can't subscript an abstract pointer";_tag_dynforward(_tmp2EB,
sizeof(char),_get_zero_arr_size(_tmp2EB,36));}),_tag_dynforward(_tmp2EA,sizeof(
void*),0));});{void*_tmp2EC=_tmp2D1;_npop_handler(0);return _tmp2EC;}_LL1A1: if(*((
int*)_tmp2CF)!= 9)goto _LL1A3;_tmp2D7=((struct Cyc_Absyn_TupleType_struct*)_tmp2CF)->f1;
_LL1A2: {void*_tmp2ED=Cyc_Tcexp_ithTupleType(_tmp2CA,loc,_tmp2D7,e2);
_npop_handler(0);return _tmp2ED;}_LL1A3:;_LL1A4: {void*_tmp2F1=({struct Cyc_String_pa_struct
_tmp2F0;_tmp2F0.tag=0;_tmp2F0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp2EE[1]={& _tmp2F0};Cyc_Tcexp_expr_err(
_tmp2CA,loc,topt,({const char*_tmp2EF="subscript applied to %s";_tag_dynforward(
_tmp2EF,sizeof(char),_get_zero_arr_size(_tmp2EF,24));}),_tag_dynforward(_tmp2EE,
sizeof(void*),1));}});_npop_handler(0);return _tmp2F1;}_LL19E:;}}};_pop_region(r);}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2F2=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2F3;
_LL1AF: if(_tmp2F2 <= (void*)4)goto _LL1B1;if(*((int*)_tmp2F2)!= 9)goto _LL1B1;
_tmp2F3=((struct Cyc_Absyn_TupleType_struct*)_tmp2F2)->f1;_LL1B0: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2F3)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AE;for(0;es != 0;(es=es->tl,_tmp2F3=_tmp2F3->tl)){
void*_tmp2F4=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2F3))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2F4,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2F3->hd)).f2);fields=({
struct Cyc_List_List*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5->hd=({struct
_tuple9*_tmp2F6=_cycalloc(sizeof(*_tmp2F6));_tmp2F6->f1=(*((struct _tuple9*)
_tmp2F3->hd)).f1;_tmp2F6->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2F6;});_tmp2F5->tl=fields;_tmp2F5;});}done=
1;goto _LL1AE;_LL1B1:;_LL1B2: goto _LL1AE;_LL1AE:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7->hd=({struct _tuple9*_tmp2F8=
_cycalloc(sizeof(*_tmp2F8));_tmp2F8->f1=Cyc_Absyn_empty_tqual(0);_tmp2F8->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2F8;});_tmp2F7->tl=fields;_tmp2F7;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9[0]=({struct Cyc_Absyn_TupleType_struct
_tmp2FA;_tmp2FA.tag=9;_tmp2FA.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp2FA;});_tmp2F9;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp2FB[0]={};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp2FC="tcCompoundLit";_tag_dynforward(_tmp2FC,sizeof(char),
_get_zero_arr_size(_tmp2FC,14));}),_tag_dynforward(_tmp2FB,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des){void*res_t2;{struct
_RegionHandle _tmp2FD=_new_region("r");struct _RegionHandle*r=& _tmp2FD;
_push_region(r);{int _tmp2FE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct*
_tmp2FF=({struct Cyc_Absyn_Const_e_struct*_tmp307=_cycalloc(sizeof(*_tmp307));
_tmp307[0]=({struct Cyc_Absyn_Const_e_struct _tmp308;_tmp308.tag=0;_tmp308.f1=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp309;(_tmp309.Int_c).tag=
2;(_tmp309.Int_c).f1=(void*)((void*)1);(_tmp309.Int_c).f2=_tmp2FE;_tmp309;});
_tmp308;});_tmp307;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2FF,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp300=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2FE - 1);if(!Cyc_Tcutil_is_zero(
_tmp300))({void*_tmp301[0]={};Cyc_Tcutil_terr(_tmp300->loc,({const char*_tmp302="zero-terminated array doesn't end with zero.";
_tag_dynforward(_tmp302,sizeof(char),_get_zero_arr_size(_tmp302,45));}),
_tag_dynforward(_tmp301,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*
_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->v=(void*)Cyc_Absyn_uint_typ;_tmp303;});
res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp306;_tmp306.tag=0;_tmp306.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(res));{void*_tmp304[1]={& _tmp306};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp305="elements of array do not all have the same type (%s)";
_tag_dynforward(_tmp305,sizeof(char),_get_zero_arr_size(_tmp305,53));}),
_tag_dynforward(_tmp304,sizeof(void*),1));}});};_pop_region(r);}{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple8*)
des->hd)).f1;if(ds != 0){void*_tmp30A=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp30B;
_LL1B4: if(*((int*)_tmp30A)!= 1)goto _LL1B6;_LL1B5:({void*_tmp30C[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp30D="only array index designators are supported";
_tag_dynforward(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,43));}),
_tag_dynforward(_tmp30C,sizeof(void*),0));});goto _LL1B3;_LL1B6: if(*((int*)
_tmp30A)!= 0)goto _LL1B3;_tmp30B=((struct Cyc_Absyn_ArrayElement_struct*)_tmp30A)->f1;
_LL1B7: Cyc_Tcexp_tcExpInitializer(te,0,_tmp30B);{unsigned int _tmp30F;int _tmp310;
struct _tuple7 _tmp30E=Cyc_Evexp_eval_const_uint_exp(_tmp30B);_tmp30F=_tmp30E.f1;
_tmp310=_tmp30E.f2;if(!_tmp310)({void*_tmp311[0]={};Cyc_Tcutil_terr(_tmp30B->loc,({
const char*_tmp312="index designator cannot use sizeof or offsetof";
_tag_dynforward(_tmp312,sizeof(char),_get_zero_arr_size(_tmp312,47));}),
_tag_dynforward(_tmp311,sizeof(void*),0));});else{if(_tmp30F != offset)({struct
Cyc_Int_pa_struct _tmp316;_tmp316.tag=1;_tmp316.f1=(unsigned long)((int)_tmp30F);{
struct Cyc_Int_pa_struct _tmp315;_tmp315.tag=1;_tmp315.f1=(unsigned long)offset;{
void*_tmp313[2]={& _tmp315,& _tmp316};Cyc_Tcutil_terr(_tmp30B->loc,({const char*
_tmp314="expecting index designator %d but found %d";_tag_dynforward(_tmp314,
sizeof(char),_get_zero_arr_size(_tmp314,43));}),_tag_dynforward(_tmp313,sizeof(
void*),2));}}});}goto _LL1B3;}_LL1B3:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp317=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1B9: if(_tmp317 <= (void*)4)goto _LL1BB;if(*((int*)
_tmp317)!= 18)goto _LL1BB;_LL1BA: goto _LL1B8;_LL1BB:;_LL1BC: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp31A;_tmp31A.tag=0;_tmp31A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(bound->topt))->v));{void*_tmp318[1]={& _tmp31A};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp319="expecting unsigned int, found %s";
_tag_dynforward(_tmp319,sizeof(char),_get_zero_arr_size(_tmp319,33));}),
_tag_dynforward(_tmp318,sizeof(void*),1));}});_LL1B8:;}if(!(vd->tq).real_const)({
void*_tmp31B[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp31C="comprehension index variable is not declared const!";
_tag_dynforward(_tmp31C,sizeof(char),_get_zero_arr_size(_tmp31C,52));}),
_tag_dynforward(_tmp31B,sizeof(void*),0));});{struct _RegionHandle _tmp31D=
_new_region("r");struct _RegionHandle*r=& _tmp31D;_push_region(r);{struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp31E=te2;
struct Cyc_Absyn_PtrInfo pinfo;void**_tmp31F=0;struct Cyc_Absyn_Tqual*_tmp320=0;
struct Cyc_Absyn_Conref**_tmp321=0;if(topt != 0){void*_tmp322=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp323;struct Cyc_Absyn_ArrayInfo _tmp324;void*
_tmp325;void**_tmp326;struct Cyc_Absyn_Tqual _tmp327;struct Cyc_Absyn_Tqual*_tmp328;
struct Cyc_Absyn_Exp*_tmp329;struct Cyc_Absyn_Conref*_tmp32A;struct Cyc_Absyn_Conref**
_tmp32B;_LL1BE: if(_tmp322 <= (void*)4)goto _LL1C2;if(*((int*)_tmp322)!= 4)goto
_LL1C0;_tmp323=((struct Cyc_Absyn_PointerType_struct*)_tmp322)->f1;_LL1BF: pinfo=
_tmp323;_tmp31F=(void**)((void**)((void*)& pinfo.elt_typ));_tmp320=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp321=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1BD;_LL1C0: if(*((int*)_tmp322)!= 7)goto _LL1C2;_tmp324=((struct Cyc_Absyn_ArrayType_struct*)
_tmp322)->f1;_tmp325=(void*)_tmp324.elt_type;_tmp326=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp322)->f1).elt_type;_tmp327=_tmp324.tq;_tmp328=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp322)->f1).tq;_tmp329=_tmp324.num_elts;
_tmp32A=_tmp324.zero_term;_tmp32B=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp322)->f1).zero_term;_LL1C1: _tmp31F=(void**)_tmp326;_tmp320=(struct Cyc_Absyn_Tqual*)
_tmp328;_tmp321=(struct Cyc_Absyn_Conref**)_tmp32B;goto _LL1BD;_LL1C2:;_LL1C3: goto
_LL1BD;_LL1BD:;}{void*t=Cyc_Tcexp_tcExp(_tmp31E,_tmp31F,body);if(_tmp31E->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp31E,bound))({void*_tmp32C[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp32D="bound is not constant";_tag_dynforward(_tmp32D,
sizeof(char),_get_zero_arr_size(_tmp32D,22));}),_tag_dynforward(_tmp32C,sizeof(
void*),0));});if(!Cyc_Tcutil_is_const_exp(_tmp31E,body))({void*_tmp32E[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp32F="body is not constant";_tag_dynforward(_tmp32F,
sizeof(char),_get_zero_arr_size(_tmp32F,21));}),_tag_dynforward(_tmp32E,sizeof(
void*),0));});}if(_tmp321 != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,*_tmp321)){struct Cyc_Absyn_Exp*_tmp330=Cyc_Absyn_uint_exp(1,0);_tmp330->topt=({
struct Cyc_Core_Opt*_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331->v=(void*)Cyc_Absyn_uint_typ;
_tmp331;});bound=Cyc_Absyn_add_exp(bound,_tmp330,0);bound->topt=({struct Cyc_Core_Opt*
_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332->v=(void*)Cyc_Absyn_uint_typ;_tmp332;});*
is_zero_term=1;}{void*_tmp333=Cyc_Absyn_array_typ(t,_tmp320 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp320,(struct Cyc_Absyn_Exp*)bound,_tmp321 == 0?Cyc_Absyn_false_conref:*
_tmp321,0);_npop_handler(0);return _tmp333;}}}};_pop_region(r);}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp334;_push_handler(& _tmp334);{
int _tmp336=0;if(setjmp(_tmp334.handler))_tmp336=1;if(!_tmp336){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp335=(void*)_exn_thrown;void*_tmp338=
_tmp335;_LL1C5: if(_tmp338 != Cyc_Dict_Absent)goto _LL1C7;_LL1C6:({struct Cyc_String_pa_struct
_tmp33B;_tmp33B.tag=0;_tmp33B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp339[1]={& _tmp33B};Cyc_Tcutil_terr(loc,({
const char*_tmp33A="unbound struct name %s";_tag_dynforward(_tmp33A,sizeof(char),
_get_zero_arr_size(_tmp33A,23));}),_tag_dynforward(_tmp339,sizeof(void*),1));}});
return topt != 0?*topt:(void*)0;_LL1C7:;_LL1C8:(void)_throw(_tmp338);_LL1C4:;}}}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({
void*_tmp33C[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp33D="expecting struct but found union";
_tag_dynforward(_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,33));}),
_tag_dynforward(_tmp33C,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp33E[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp33F="can't build abstract struct";
_tag_dynforward(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,28));}),
_tag_dynforward(_tmp33E,sizeof(void*),0));});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}{struct _RegionHandle _tmp340=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp340;_push_region(rgn);{struct _tuple4 _tmp341=({struct _tuple4 _tmp361;_tmp361.f1=
Cyc_Tcenv_lookup_type_vars(te);_tmp361.f2=rgn;_tmp361;});struct Cyc_List_List*
_tmp342=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp341,ad->tvs);struct Cyc_List_List*_tmp343=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp341,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp342);struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp343);struct Cyc_List_List*_tmp346=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp342,_tmp343);struct Cyc_Absyn_AggrType_struct*_tmp347=({struct Cyc_Absyn_AggrType_struct*
_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C[0]=({struct Cyc_Absyn_AggrType_struct
_tmp35D;_tmp35D.tag=10;_tmp35D.f1=({struct Cyc_Absyn_AggrInfo _tmp35E;_tmp35E.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp35F;(_tmp35F.KnownAggr).tag=
1;(_tmp35F.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp360=_cycalloc(sizeof(*
_tmp360));_tmp360[0]=ad;_tmp360;});_tmp35F;});_tmp35E.targs=_tmp344;_tmp35E;});
_tmp35D;});_tmp35C;});struct Cyc_List_List*_tmp348=*ts;struct Cyc_List_List*
_tmp349=_tmp345;while(_tmp348 != 0  && _tmp349 != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp348->hd);Cyc_Tcutil_unify((
void*)_tmp348->hd,(void*)_tmp349->hd);_tmp348=_tmp348->tl;_tmp349=_tmp349->tl;}
if(_tmp348 != 0)({void*_tmp34A[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp34B="too many explicit witness types";
_tag_dynforward(_tmp34B,sizeof(char),_get_zero_arr_size(_tmp34B,32));}),
_tag_dynforward(_tmp34A,sizeof(void*),0));});*ts=_tmp345;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp347,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp34D;struct Cyc_Absyn_Aggrfield*_tmp34E;
struct Cyc_Absyn_Exp*_tmp34F;struct _tuple10*_tmp34C=(struct _tuple10*)fields->hd;
_tmp34D=*_tmp34C;_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;{void*_tmp350=Cyc_Tcutil_rsubstitute(
rgn,_tmp346,(void*)_tmp34E->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp350,
_tmp34F);if(!Cyc_Tcutil_coerce_arg(te,_tmp34F,_tmp350)){({struct Cyc_String_pa_struct
_tmp356;_tmp356.tag=0;_tmp356.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp34F->topt))->v));{
struct Cyc_String_pa_struct _tmp355;_tmp355.tag=0;_tmp355.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp350));{struct
Cyc_String_pa_struct _tmp354;_tmp354.tag=0;_tmp354.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp353;_tmp353.tag=0;_tmp353.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp34E->name);{void*_tmp351[4]={& _tmp353,& _tmp354,& _tmp355,& _tmp356};Cyc_Tcutil_terr(
_tmp34F->loc,({const char*_tmp352="field %s of struct %s expects type %s != %s";
_tag_dynforward(_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,44));}),
_tag_dynforward(_tmp351,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp357=0;{struct Cyc_List_List*_tmp358=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp358 != 0;_tmp358=_tmp358->tl){_tmp357=({
struct Cyc_List_List*_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359->hd=({struct
_tuple0*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp346,(*((struct _tuple0*)_tmp358->hd)).f1);_tmp35A->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp346,(*((struct _tuple0*)_tmp358->hd)).f2);_tmp35A;});_tmp359->tl=_tmp357;
_tmp359;});}}_tmp357=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp357);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp357);{void*_tmp35B=(void*)
_tmp347;_npop_handler(0);return _tmp35B;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp362=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp362;_push_region(rgn);{void*_tmp363=Cyc_Tcutil_compress(ts);void*_tmp364;
struct Cyc_List_List*_tmp365;_LL1CA: if(_tmp363 <= (void*)4)goto _LL1CC;if(*((int*)
_tmp363)!= 11)goto _LL1CC;_tmp364=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp363)->f1;_tmp365=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp363)->f2;_LL1CB:
if(_tmp364 == (void*)1)({void*_tmp366[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp367="expecting struct but found union";_tag_dynforward(_tmp367,sizeof(char),
_get_zero_arr_size(_tmp367,33));}),_tag_dynforward(_tmp366,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp365);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp369;struct Cyc_Absyn_Aggrfield*
_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;struct _tuple10*_tmp368=(struct _tuple10*)
fields->hd;_tmp369=*_tmp368;_tmp36A=_tmp369.f1;_tmp36B=_tmp369.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp36A->type)),_tmp36B);if(!Cyc_Tcutil_coerce_arg(
te,_tmp36B,(void*)_tmp36A->type)){({struct Cyc_String_pa_struct _tmp370;_tmp370.tag=
0;_tmp370.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36B->topt))->v));{struct Cyc_String_pa_struct
_tmp36F;_tmp36F.tag=0;_tmp36F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)_tmp36A->type));{struct Cyc_String_pa_struct _tmp36E;
_tmp36E.tag=0;_tmp36E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp36A->name);{
void*_tmp36C[3]={& _tmp36E,& _tmp36F,& _tmp370};Cyc_Tcutil_terr(_tmp36B->loc,({
const char*_tmp36D="field %s of struct expects type %s != %s";_tag_dynforward(
_tmp36D,sizeof(char),_get_zero_arr_size(_tmp36D,41));}),_tag_dynforward(_tmp36C,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1C9;}_LL1CC:;
_LL1CD:({void*_tmp371[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp372="tcAnonStruct: wrong type";
_tag_dynforward(_tmp372,sizeof(char),_get_zero_arr_size(_tmp372,25));}),
_tag_dynforward(_tmp371,sizeof(void*),0));});_LL1C9:;};_pop_region(rgn);}return
ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp373=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp373;_push_region(rgn);{struct _tuple4 _tmp374=({
struct _tuple4 _tmp394;_tmp394.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp394.f2=rgn;
_tmp394;});struct Cyc_List_List*_tmp375=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp374,tud->tvs);struct Cyc_List_List*_tmp376=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp375);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp391;_tmp391.tag=3;_tmp391.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp392;
_tmp392.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({union Cyc_Absyn_TunionFieldInfoU_union
_tmp393;(_tmp393.KnownTunionfield).tag=1;(_tmp393.KnownTunionfield).f1=tud;(
_tmp393.KnownTunionfield).f2=tuf;_tmp393;});_tmp392.targs=_tmp376;_tmp392;});
_tmp391;});_tmp390;});if(topt != 0){void*_tmp377=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_TunionInfo _tmp378;struct Cyc_List_List*_tmp379;struct Cyc_Core_Opt*
_tmp37A;_LL1CF: if(_tmp377 <= (void*)4)goto _LL1D3;if(*((int*)_tmp377)!= 3)goto
_LL1D1;_LL1D0: Cyc_Tcutil_unify(*topt,res);goto _LL1CE;_LL1D1: if(*((int*)_tmp377)
!= 2)goto _LL1D3;_tmp378=((struct Cyc_Absyn_TunionType_struct*)_tmp377)->f1;
_tmp379=_tmp378.targs;_tmp37A=_tmp378.rgn;_LL1D2:{struct Cyc_List_List*a=_tmp376;
for(0;a != 0  && _tmp379 != 0;(a=a->tl,_tmp379=_tmp379->tl)){Cyc_Tcutil_unify((void*)
a->hd,(void*)_tmp379->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){e->topt=({
struct Cyc_Core_Opt*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->v=(void*)res;
_tmp37B;});res=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp37C=_cycalloc(
sizeof(*_tmp37C));_tmp37C[0]=({struct Cyc_Absyn_TunionType_struct _tmp37D;_tmp37D.tag=
2;_tmp37D.f1=({struct Cyc_Absyn_TunionInfo _tmp37E;_tmp37E.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp37F;(_tmp37F.KnownTunion).tag=1;(_tmp37F.KnownTunion).f1=({
struct Cyc_Absyn_Tuniondecl**_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380[0]=tud;
_tmp380;});_tmp37F;});_tmp37E.targs=_tmp376;_tmp37E.rgn=_tmp37A;_tmp37E;});
_tmp37D;});_tmp37C;});Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}goto _LL1CE;
_LL1D3:;_LL1D4: goto _LL1CE;_LL1CE:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 
0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp375,(*((struct _tuple9*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp385;_tmp385.tag=0;_tmp385.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)(e->topt == 0?(struct _dynforward_ptr)({
const char*_tmp386="?";_tag_dynforward(_tmp386,sizeof(char),_get_zero_arr_size(
_tmp386,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{
struct Cyc_String_pa_struct _tmp384;_tmp384.tag=0;_tmp384.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp383;_tmp383.tag=0;_tmp383.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp381[3]={& _tmp383,& _tmp384,& _tmp385};
Cyc_Tcutil_terr(e->loc,({const char*_tmp382="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_dynforward(_tmp382,sizeof(char),_get_zero_arr_size(_tmp382,80));}),
_tag_dynforward(_tmp381,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp38A=({struct Cyc_String_pa_struct _tmp389;_tmp389.tag=0;
_tmp389.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp387[1]={& _tmp389};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)
es->hd)->loc,topt,({const char*_tmp388="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp388,sizeof(char),_get_zero_arr_size(_tmp388,45));}),
_tag_dynforward(_tmp387,sizeof(void*),1));}});_npop_handler(0);return _tmp38A;}
if(ts != 0){void*_tmp38E=({struct Cyc_String_pa_struct _tmp38D;_tmp38D.tag=0;
_tmp38D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp38B[1]={& _tmp38D};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp38C="too few arguments for tunion constructor %s";_tag_dynforward(
_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,44));}),_tag_dynforward(_tmp38B,
sizeof(void*),1));}});_npop_handler(0);return _tmp38E;}{void*_tmp38F=res;
_npop_handler(0);return _tmp38F;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp395=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp396;
struct Cyc_Absyn_PtrAtts _tmp397;struct Cyc_Absyn_Conref*_tmp398;struct Cyc_Absyn_ArrayInfo
_tmp399;void*_tmp39A;struct Cyc_List_List*_tmp39B;struct Cyc_Absyn_AggrInfo _tmp39C;
union Cyc_Absyn_AggrInfoU_union _tmp39D;struct Cyc_List_List*_tmp39E;struct Cyc_List_List*
_tmp39F;_LL1D6: if((int)_tmp395 != 0)goto _LL1D8;_LL1D7: return 1;_LL1D8: if(_tmp395 <= (
void*)4)goto _LL1E4;if(*((int*)_tmp395)!= 0)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA:
if(*((int*)_tmp395)!= 1)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(*((int*)_tmp395)
!= 2)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(*((int*)_tmp395)!= 3)goto _LL1E0;
_LL1DF: return 0;_LL1E0: if(*((int*)_tmp395)!= 4)goto _LL1E2;_tmp396=((struct Cyc_Absyn_PointerType_struct*)
_tmp395)->f1;_tmp397=_tmp396.ptr_atts;_tmp398=_tmp397.nullable;_LL1E1: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp398);_LL1E2: if(*((
int*)_tmp395)!= 5)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp395 != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp395 <= (void*)4)goto _LL202;if(*((int*)
_tmp395)!= 6)goto _LL1E8;_LL1E7: return 1;_LL1E8: if(*((int*)_tmp395)!= 7)goto _LL1EA;
_tmp399=((struct Cyc_Absyn_ArrayType_struct*)_tmp395)->f1;_tmp39A=(void*)_tmp399.elt_type;
_LL1E9: return Cyc_Tcexp_zeroable_type(_tmp39A);_LL1EA: if(*((int*)_tmp395)!= 8)
goto _LL1EC;_LL1EB: return 0;_LL1EC: if(*((int*)_tmp395)!= 9)goto _LL1EE;_tmp39B=((
struct Cyc_Absyn_TupleType_struct*)_tmp395)->f1;_LL1ED: for(0;(unsigned int)
_tmp39B;_tmp39B=_tmp39B->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp39B->hd)).f2))return 0;}return 1;_LL1EE: if(*((int*)_tmp395)!= 10)goto _LL1F0;
_tmp39C=((struct Cyc_Absyn_AggrType_struct*)_tmp395)->f1;_tmp39D=_tmp39C.aggr_info;
_tmp39E=_tmp39C.targs;_LL1EF: {struct Cyc_Absyn_Aggrdecl*_tmp3A0=Cyc_Absyn_get_known_aggrdecl(
_tmp39D);if(_tmp3A0->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3A0->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp3A1=_new_region("r");
struct _RegionHandle*r=& _tmp3A1;_push_region(r);{struct Cyc_List_List*_tmp3A2=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp3A0->tvs,_tmp39E);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A0->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3A2,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3A3=0;_npop_handler(0);
return _tmp3A3;}}}{int _tmp3A4=1;_npop_handler(0);return _tmp3A4;}};_pop_region(r);}}
_LL1F0: if(*((int*)_tmp395)!= 12)goto _LL1F2;_LL1F1: return 1;_LL1F2: if(*((int*)
_tmp395)!= 14)goto _LL1F4;_LL1F3: return 1;_LL1F4: if(*((int*)_tmp395)!= 18)goto
_LL1F6;_LL1F5: return 1;_LL1F6: if(*((int*)_tmp395)!= 11)goto _LL1F8;_tmp39F=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp395)->f2;_LL1F7: for(0;_tmp39F != 0;
_tmp39F=_tmp39F->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp39F->hd)->type))return 0;}return 1;_LL1F8: if(*((int*)_tmp395)!= 13)goto _LL1FA;
_LL1F9: return 1;_LL1FA: if(*((int*)_tmp395)!= 17)goto _LL1FC;_LL1FB: return 0;_LL1FC:
if(*((int*)_tmp395)!= 16)goto _LL1FE;_LL1FD: return 0;_LL1FE: if(*((int*)_tmp395)!= 
15)goto _LL200;_LL1FF: return 0;_LL200: if(*((int*)_tmp395)!= 19)goto _LL202;_LL201:
goto _LL203;_LL202: if((int)_tmp395 != 2)goto _LL204;_LL203: goto _LL205;_LL204: if((
int)_tmp395 != 3)goto _LL206;_LL205: goto _LL207;_LL206: if(_tmp395 <= (void*)4)goto
_LL208;if(*((int*)_tmp395)!= 20)goto _LL208;_LL207: goto _LL209;_LL208: if(_tmp395 <= (
void*)4)goto _LL20A;if(*((int*)_tmp395)!= 21)goto _LL20A;_LL209: goto _LL20B;_LL20A:
if(_tmp395 <= (void*)4)goto _LL1D5;if(*((int*)_tmp395)!= 22)goto _LL1D5;_LL20B:({
struct Cyc_String_pa_struct _tmp3A7;_tmp3A7.tag=0;_tmp3A7.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp3A5[
1]={& _tmp3A7};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3A6="bad type `%s' in zeroable type";_tag_dynforward(_tmp3A6,
sizeof(char),_get_zero_arr_size(_tmp3A6,31));}),_tag_dynforward(_tmp3A5,sizeof(
void*),1));}});_LL1D5:;}static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3A8=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3A9;void*_tmp3AA;_LL20D: if(_tmp3A8 <= (void*)4)
goto _LL20F;if(*((int*)_tmp3A8)!= 4)goto _LL20F;_tmp3A9=((struct Cyc_Absyn_PointerType_struct*)
_tmp3A8)->f1;_tmp3AA=(void*)_tmp3A9.elt_typ;_LL20E: Cyc_Tcutil_unify(_tmp3AA,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL20C;_LL20F:;_LL210: goto _LL20C;_LL20C:;}({struct Cyc_String_pa_struct
_tmp3AE;_tmp3AE.tag=0;_tmp3AE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp3AD;_tmp3AD.tag=0;
_tmp3AD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)(allow_zero?(struct
_dynforward_ptr)({const char*_tmp3AF="calloc";_tag_dynforward(_tmp3AF,sizeof(char),
_get_zero_arr_size(_tmp3AF,7));}):(struct _dynforward_ptr)({const char*_tmp3B0="malloc";
_tag_dynforward(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,7));})));{void*
_tmp3AB[2]={& _tmp3AD,& _tmp3AE};Cyc_Tcutil_terr(loc,({const char*_tmp3AC="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dynforward(_tmp3AC,sizeof(char),_get_zero_arr_size(_tmp3AC,60));}),
_tag_dynforward(_tmp3AB,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3B7;_tmp3B7.tag=15;_tmp3B7.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3B7;});_tmp3B6;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3B1=Cyc_Tcutil_compress(handle_type);void*_tmp3B2;_LL212: if(_tmp3B1 <= (
void*)4)goto _LL214;if(*((int*)_tmp3B1)!= 15)goto _LL214;_tmp3B2=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3B1)->f1;_LL213: rgn=_tmp3B2;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL211;_LL214:;_LL215:({struct Cyc_String_pa_struct _tmp3B5;
_tmp3B5.tag=0;_tmp3B5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3B3[1]={& _tmp3B5};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp3B4="expecting region_t type but found %s";_tag_dynforward(_tmp3B4,sizeof(
char),_get_zero_arr_size(_tmp3B4,37));}),_tag_dynforward(_tmp3B3,sizeof(void*),1));}});
goto _LL211;_LL211:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({void*
_tmp3B8[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3B9="calloc with empty type";_tag_dynforward(_tmp3B9,sizeof(char),
_get_zero_arr_size(_tmp3B9,23));}),_tag_dynforward(_tmp3B8,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3BA=(void*)(*e)->r;void*_tmp3BB;void*
_tmp3BC;struct Cyc_List_List*_tmp3BD;struct Cyc_List_List _tmp3BE;struct Cyc_Absyn_Exp*
_tmp3BF;struct Cyc_List_List*_tmp3C0;struct Cyc_List_List _tmp3C1;struct Cyc_Absyn_Exp*
_tmp3C2;struct Cyc_List_List*_tmp3C3;_LL217: if(*((int*)_tmp3BA)!= 18)goto _LL219;
_tmp3BB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3BA)->f1;_LL218:
elt_type=_tmp3BB;*t=(void**)({void**_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4[
0]=elt_type;_tmp3C4;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL216;_LL219: if(*((int*)_tmp3BA)!= 3)
goto _LL21B;_tmp3BC=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3BA)->f1;if((
int)_tmp3BC != 1)goto _LL21B;_tmp3BD=((struct Cyc_Absyn_Primop_e_struct*)_tmp3BA)->f2;
if(_tmp3BD == 0)goto _LL21B;_tmp3BE=*_tmp3BD;_tmp3BF=(struct Cyc_Absyn_Exp*)_tmp3BE.hd;
_tmp3C0=_tmp3BE.tl;if(_tmp3C0 == 0)goto _LL21B;_tmp3C1=*_tmp3C0;_tmp3C2=(struct Cyc_Absyn_Exp*)
_tmp3C1.hd;_tmp3C3=_tmp3C1.tl;if(_tmp3C3 != 0)goto _LL21B;_LL21A:{struct _tuple0
_tmp3C6=({struct _tuple0 _tmp3C5;_tmp3C5.f1=(void*)_tmp3BF->r;_tmp3C5.f2=(void*)
_tmp3C2->r;_tmp3C5;});void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;_LL21E:
_tmp3C7=_tmp3C6.f1;if(*((int*)_tmp3C7)!= 18)goto _LL220;_tmp3C8=(void*)((struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C7)->f1;_LL21F: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp3C8);elt_type=_tmp3C8;*t=(void**)({void**_tmp3CB=_cycalloc(sizeof(*
_tmp3CB));_tmp3CB[0]=elt_type;_tmp3CB;});num_elts=_tmp3C2;one_elt=0;goto _LL21D;
_LL220: _tmp3C9=_tmp3C6.f2;if(*((int*)_tmp3C9)!= 18)goto _LL222;_tmp3CA=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C9)->f1;_LL221: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3CA);elt_type=_tmp3CA;*t=(void**)({void**_tmp3CC=_cycalloc(
sizeof(*_tmp3CC));_tmp3CC[0]=elt_type;_tmp3CC;});num_elts=_tmp3BF;one_elt=0;goto
_LL21D;_LL222:;_LL223: goto No_sizeof;_LL21D:;}goto _LL216;_LL21B:;_LL21C: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3CD=_cycalloc(sizeof(*_tmp3CD));
_tmp3CD[0]=elt_type;_tmp3CD;});num_elts=*e;one_elt=0;goto _LL216;_LL216:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3CE=elt_type;struct Cyc_Absyn_AggrInfo _tmp3CF;
union Cyc_Absyn_AggrInfoU_union _tmp3D0;struct Cyc_Absyn_Aggrdecl**_tmp3D1;struct
Cyc_Absyn_Aggrdecl*_tmp3D2;_LL225: if(_tmp3CE <= (void*)4)goto _LL227;if(*((int*)
_tmp3CE)!= 10)goto _LL227;_tmp3CF=((struct Cyc_Absyn_AggrType_struct*)_tmp3CE)->f1;
_tmp3D0=_tmp3CF.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3CE)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL227;_tmp3D1=(_tmp3D0.KnownAggr).f1;_tmp3D2=*_tmp3D1;_LL226: if(
_tmp3D2->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D2->impl))->exist_vars
!= 0)({void*_tmp3D3[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3D4="malloc with existential types not yet implemented";
_tag_dynforward(_tmp3D4,sizeof(char),_get_zero_arr_size(_tmp3D4,50));}),
_tag_dynforward(_tmp3D3,sizeof(void*),0));});goto _LL224;_LL227:;_LL228: goto
_LL224;_LL224:;}{void*(*_tmp3D5)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp3D6=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp3D7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3D8;struct Cyc_Absyn_PtrAtts _tmp3D9;struct Cyc_Absyn_Conref*_tmp3DA;struct Cyc_Absyn_Conref*
_tmp3DB;_LL22A: if(_tmp3D7 <= (void*)4)goto _LL22C;if(*((int*)_tmp3D7)!= 4)goto
_LL22C;_tmp3D8=((struct Cyc_Absyn_PointerType_struct*)_tmp3D7)->f1;_tmp3D9=
_tmp3D8.ptr_atts;_tmp3DA=_tmp3D9.nullable;_tmp3DB=_tmp3D9.zero_term;_LL22B:
_tmp3D6=_tmp3DB;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3DA))_tmp3D5=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3DB) && !(*is_calloc)){({void*_tmp3DC[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp3DD="converting malloc to calloc to ensure zero-termination";
_tag_dynforward(_tmp3DD,sizeof(char),_get_zero_arr_size(_tmp3DD,55));}),
_tag_dynforward(_tmp3DC,sizeof(void*),0));});*is_calloc=1;}goto _LL229;_LL22C:;
_LL22D: goto _LL229;_LL229:;}if(!one_elt)_tmp3D5=Cyc_Absyn_dynforward_typ;return
_tmp3D5(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3D6);}}}static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3DE=Cyc_Tcutil_compress(t1);_LL22F: if(
_tmp3DE <= (void*)4)goto _LL231;if(*((int*)_tmp3DE)!= 7)goto _LL231;_LL230:({void*
_tmp3DF[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3E0="cannot assign to an array";
_tag_dynforward(_tmp3E0,sizeof(char),_get_zero_arr_size(_tmp3E0,26));}),
_tag_dynforward(_tmp3DF,sizeof(void*),0));});goto _LL22E;_LL231:;_LL232: goto
_LL22E;_LL22E:;}{int ign_1=0;int ign_2=0;if(!Cyc_Tcutil_is_pointer_or_boxed(t1,&
ign_1,& ign_2))({void*_tmp3E1[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3E2="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dynforward(_tmp3E2,sizeof(char),_get_zero_arr_size(_tmp3E2,58));}),
_tag_dynforward(_tmp3E1,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp3E3[0]={};Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp3E4="swap non-lvalue";
_tag_dynforward(_tmp3E4,sizeof(char),_get_zero_arr_size(_tmp3E4,16));}),
_tag_dynforward(_tmp3E3,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e2))return({
void*_tmp3E5[0]={};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp3E6="swap non-lvalue";
_tag_dynforward(_tmp3E6,sizeof(char),_get_zero_arr_size(_tmp3E6,16));}),
_tag_dynforward(_tmp3E5,sizeof(void*),0));});{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;int b_ign2=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,&
b_ign1,& b_ign2,& t_ign2))return({void*_tmp3E7[0]={};Cyc_Tcexp_expr_err(te,e1->loc,
topt,({const char*_tmp3E8="swap value in zeroterm array";_tag_dynforward(_tmp3E8,
sizeof(char),_get_zero_arr_size(_tmp3E8,29));}),_tag_dynforward(_tmp3E7,sizeof(
void*),0));});if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& b_ign2,& t_ign2))
return({void*_tmp3E9[0]={};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp3EA="swap value in zeroterm array";_tag_dynforward(_tmp3EA,sizeof(char),
_get_zero_arr_size(_tmp3EA,29));}),_tag_dynforward(_tmp3E9,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(
t1,t2)){void*_tmp3EB=({struct Cyc_String_pa_struct _tmp3EF;_tmp3EF.tag=0;_tmp3EF.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp3EE;_tmp3EE.tag=0;_tmp3EE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp3EC[2]={& _tmp3EE,& _tmp3EF};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp3ED="type mismatch: %s != %s";_tag_dynforward(_tmp3ED,sizeof(char),
_get_zero_arr_size(_tmp3ED,24));}),_tag_dynforward(_tmp3EC,sizeof(void*),2));}}});
return _tmp3EB;}return(void*)0;}}}}static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*s){{struct
_RegionHandle _tmp3F0=_new_region("r");struct _RegionHandle*r=& _tmp3F0;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);;
_pop_region(r);}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp3F1=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_Absyn_Stmt*
_tmp3F3;struct Cyc_Absyn_Stmt*_tmp3F4;struct Cyc_Absyn_Decl*_tmp3F5;struct Cyc_Absyn_Stmt*
_tmp3F6;_LL234: if(_tmp3F1 <= (void*)1)goto _LL23A;if(*((int*)_tmp3F1)!= 0)goto
_LL236;_tmp3F2=((struct Cyc_Absyn_Exp_s_struct*)_tmp3F1)->f1;_LL235: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v;_LL236: if(*((int*)_tmp3F1)!= 
1)goto _LL238;_tmp3F3=((struct Cyc_Absyn_Seq_s_struct*)_tmp3F1)->f1;_tmp3F4=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3F1)->f2;_LL237: s=_tmp3F4;continue;_LL238: if(*((
int*)_tmp3F1)!= 11)goto _LL23A;_tmp3F5=((struct Cyc_Absyn_Decl_s_struct*)_tmp3F1)->f1;
_tmp3F6=((struct Cyc_Absyn_Decl_s_struct*)_tmp3F1)->f2;_LL239: s=_tmp3F6;continue;
_LL23A:;_LL23B: return({void*_tmp3F7[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp3F8="statement expression must end with expression";_tag_dynforward(
_tmp3F8,sizeof(char),_get_zero_arr_size(_tmp3F8,46));}),_tag_dynforward(_tmp3F7,
sizeof(void*),0));});_LL233:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(rgn_handle != 
0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3FE=
_cycalloc(sizeof(*_tmp3FE));_tmp3FE[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3FF;_tmp3FF.tag=15;_tmp3FF.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3FF;});_tmp3FE;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp3F9=Cyc_Tcutil_compress(handle_type);void*_tmp3FA;_LL23D:
if(_tmp3F9 <= (void*)4)goto _LL23F;if(*((int*)_tmp3F9)!= 15)goto _LL23F;_tmp3FA=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3F9)->f1;_LL23E: rgn=_tmp3FA;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL23C;_LL23F:;_LL240:({struct Cyc_String_pa_struct
_tmp3FD;_tmp3FD.tag=0;_tmp3FD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp3FB[1]={& _tmp3FD};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp3FC="expecting region_t type but found %s";
_tag_dynforward(_tmp3FC,sizeof(char),_get_zero_arr_size(_tmp3FC,37));}),
_tag_dynforward(_tmp3FB,sizeof(void*),1));}});goto _LL23C;_LL23C:;}{void*_tmp400=(
void*)e1->r;struct Cyc_Core_Opt*_tmp401;struct Cyc_List_List*_tmp402;struct Cyc_List_List*
_tmp403;union Cyc_Absyn_Cnst_union _tmp404;struct _dynforward_ptr _tmp405;_LL242: if(*((
int*)_tmp400)!= 29)goto _LL244;_LL243: {void*_tmp406=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp407=Cyc_Tcutil_compress(_tmp406);struct Cyc_Absyn_ArrayInfo
_tmp408;void*_tmp409;struct Cyc_Absyn_Tqual _tmp40A;struct Cyc_Absyn_Exp*_tmp40B;
struct Cyc_Absyn_Conref*_tmp40C;_LL24D: if(_tmp407 <= (void*)4)goto _LL24F;if(*((int*)
_tmp407)!= 7)goto _LL24F;_tmp408=((struct Cyc_Absyn_ArrayType_struct*)_tmp407)->f1;
_tmp409=(void*)_tmp408.elt_type;_tmp40A=_tmp408.tq;_tmp40B=_tmp408.num_elts;
_tmp40C=_tmp408.zero_term;_LL24E: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp40B);void*b;{void*_tmp40D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bnd->topt))->v);void*_tmp40E;_LL252: if(_tmp40D <= (void*)4)goto _LL254;
if(*((int*)_tmp40D)!= 18)goto _LL254;_tmp40E=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp40D)->f1;_LL253: b=(void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp40F=
_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp410;_tmp410.tag=1;_tmp410.f1=(void*)_tmp40E;_tmp410;});_tmp40F;});goto _LL251;
_LL254:;_LL255: if(Cyc_Tcutil_is_const_exp(te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp412;_tmp412.tag=0;_tmp412.f1=bnd;_tmp412;});_tmp411;});else{b=(void*)0;}
_LL251:;}{void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp414=
_cycalloc(sizeof(*_tmp414));_tmp414[0]=({struct Cyc_Absyn_PointerType_struct
_tmp415;_tmp415.tag=4;_tmp415.f1=({struct Cyc_Absyn_PtrInfo _tmp416;_tmp416.elt_typ=(
void*)_tmp409;_tmp416.elt_tq=_tmp40A;_tmp416.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp417;_tmp417.rgn=(void*)rgn;_tmp417.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp417.bounds=Cyc_Absyn_new_conref(b);_tmp417.zero_term=
_tmp40C;_tmp417.ptrloc=0;_tmp417;});_tmp416;});_tmp415;});_tmp414;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->v=(
void*)res_typ;_tmp413;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL24F:;_LL250:({void*_tmp418[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp419="tcNew: comprehension returned non-array type";_tag_dynforward(_tmp419,
sizeof(char),_get_zero_arr_size(_tmp419,45));}),_tag_dynforward(_tmp418,sizeof(
void*),0));});_LL24C:;}_LL244: if(*((int*)_tmp400)!= 37)goto _LL246;_tmp401=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp400)->f1;_tmp402=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp400)->f2;_LL245:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[0]=({struct Cyc_Absyn_Array_e_struct
_tmp41B;_tmp41B.tag=28;_tmp41B.f1=_tmp402;_tmp41B;});_tmp41A;})));_tmp403=
_tmp402;goto _LL247;_LL246: if(*((int*)_tmp400)!= 28)goto _LL248;_tmp403=((struct
Cyc_Absyn_Array_e_struct*)_tmp400)->f1;_LL247: {void**elt_typ_opt=0;int zero_term=
0;if(topt != 0){void*_tmp41C=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp41D;void*_tmp41E;void**_tmp41F;struct Cyc_Absyn_Tqual _tmp420;struct Cyc_Absyn_PtrAtts
_tmp421;struct Cyc_Absyn_Conref*_tmp422;_LL257: if(_tmp41C <= (void*)4)goto _LL259;
if(*((int*)_tmp41C)!= 4)goto _LL259;_tmp41D=((struct Cyc_Absyn_PointerType_struct*)
_tmp41C)->f1;_tmp41E=(void*)_tmp41D.elt_typ;_tmp41F=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp41C)->f1).elt_typ;_tmp420=_tmp41D.elt_tq;_tmp421=_tmp41D.ptr_atts;_tmp422=
_tmp421.zero_term;_LL258: elt_typ_opt=(void**)_tmp41F;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp422);goto _LL256;_LL259:;
_LL25A: goto _LL256;_LL256:;}{void*_tmp423=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp403);e1->topt=({struct Cyc_Core_Opt*_tmp424=_cycalloc(
sizeof(*_tmp424));_tmp424->v=(void*)_tmp423;_tmp424;});{void*res_typ;{void*
_tmp425=Cyc_Tcutil_compress(_tmp423);struct Cyc_Absyn_ArrayInfo _tmp426;void*
_tmp427;struct Cyc_Absyn_Tqual _tmp428;struct Cyc_Absyn_Exp*_tmp429;struct Cyc_Absyn_Conref*
_tmp42A;_LL25C: if(_tmp425 <= (void*)4)goto _LL25E;if(*((int*)_tmp425)!= 7)goto
_LL25E;_tmp426=((struct Cyc_Absyn_ArrayType_struct*)_tmp425)->f1;_tmp427=(void*)
_tmp426.elt_type;_tmp428=_tmp426.tq;_tmp429=_tmp426.num_elts;_tmp42A=_tmp426.zero_term;
_LL25D: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp42B=_cycalloc(
sizeof(*_tmp42B));_tmp42B[0]=({struct Cyc_Absyn_PointerType_struct _tmp42C;_tmp42C.tag=
4;_tmp42C.f1=({struct Cyc_Absyn_PtrInfo _tmp42D;_tmp42D.elt_typ=(void*)_tmp427;
_tmp42D.elt_tq=_tmp428;_tmp42D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp42E;_tmp42E.rgn=(
void*)rgn;_tmp42E.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp42E.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp430;_tmp430.tag=0;_tmp430.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp429);
_tmp430;});_tmp42F;}));_tmp42E.zero_term=_tmp42A;_tmp42E.ptrloc=0;_tmp42E;});
_tmp42D;});_tmp42C;});_tmp42B;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->v=(void*)res_typ;_tmp431;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL25B;_LL25E:;_LL25F:({void*_tmp432[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp433="tcExpNoPromote on Array_e returned non-array type";
_tag_dynforward(_tmp433,sizeof(char),_get_zero_arr_size(_tmp433,50));}),
_tag_dynforward(_tmp432,sizeof(void*),0));});_LL25B:;}return res_typ;}}}_LL248:
if(*((int*)_tmp400)!= 0)goto _LL24A;_tmp404=((struct Cyc_Absyn_Const_e_struct*)
_tmp400)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp400)->f1).String_c).tag
!= 5)goto _LL24A;_tmp405=(_tmp404.String_c).f1;_LL249: {void*_tmp434=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp435=Cyc_Tcexp_tcExp(te,(void**)& _tmp434,e1);return Cyc_Absyn_atb_typ(
_tmp435,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp437;_tmp437.tag=0;_tmp437.f1=Cyc_Absyn_uint_exp(1,0);_tmp437;});_tmp436;}),
Cyc_Absyn_false_conref);}_LL24A:;_LL24B: {void**topt2=0;if(topt != 0){void*
_tmp438=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp439;void*_tmp43A;
void**_tmp43B;struct Cyc_Absyn_Tqual _tmp43C;_LL261: if(_tmp438 <= (void*)4)goto
_LL265;if(*((int*)_tmp438)!= 4)goto _LL263;_tmp439=((struct Cyc_Absyn_PointerType_struct*)
_tmp438)->f1;_tmp43A=(void*)_tmp439.elt_typ;_tmp43B=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp438)->f1).elt_typ;_tmp43C=_tmp439.elt_tq;_LL262: topt2=(void**)_tmp43B;goto
_LL260;_LL263: if(*((int*)_tmp438)!= 2)goto _LL265;_LL264: topt2=({void**_tmp43D=
_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=*topt;_tmp43D;});goto _LL260;_LL265:;
_LL266: goto _LL260;_LL260:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));
_tmp43F[0]=({struct Cyc_Absyn_PointerType_struct _tmp440;_tmp440.tag=4;_tmp440.f1=({
struct Cyc_Absyn_PtrInfo _tmp441;_tmp441.elt_typ=(void*)telt;_tmp441.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp441.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp442;_tmp442.rgn=(void*)rgn;
_tmp442.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp442.bounds=Cyc_Absyn_bounds_one_conref;_tmp442.zero_term=Cyc_Absyn_false_conref;
_tmp442.ptrloc=0;_tmp442;});_tmp441;});_tmp440;});_tmp43F;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E->v=(
void*)res_typ;_tmp43E;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL241:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp443=t;struct Cyc_Absyn_ArrayInfo _tmp444;void*_tmp445;struct
Cyc_Absyn_Tqual _tmp446;struct Cyc_Absyn_Exp*_tmp447;struct Cyc_Absyn_Conref*
_tmp448;_LL268: if(_tmp443 <= (void*)4)goto _LL26A;if(*((int*)_tmp443)!= 7)goto
_LL26A;_tmp444=((struct Cyc_Absyn_ArrayType_struct*)_tmp443)->f1;_tmp445=(void*)
_tmp444.elt_type;_tmp446=_tmp444.tq;_tmp447=_tmp444.num_elts;_tmp448=_tmp444.zero_term;
_LL269: {void*_tmp44A;struct _tuple6 _tmp449=Cyc_Tcutil_addressof_props(te,e);
_tmp44A=_tmp449.f2;{void*_tmp44B=_tmp447 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp44D;_tmp44D.tag=0;_tmp44D.f1=(struct Cyc_Absyn_Exp*)
_tmp447;_tmp44D;});_tmp44C;});t=Cyc_Absyn_atb_typ(_tmp445,_tmp44A,_tmp446,
_tmp44B,_tmp448);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL26A:;_LL26B: return t;_LL267:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);if(Cyc_Tcutil_is_unique_pointer(t) && !Cyc_Tcutil_is_unique_path(e)){({
struct Cyc_String_pa_struct _tmp452;_tmp452.tag=0;_tmp452.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{struct Cyc_String_pa_struct
_tmp451;_tmp451.tag=0;_tmp451.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
struct Cyc_String_pa_struct _tmp450;_tmp450.tag=0;_tmp450.f1=(struct
_dynforward_ptr)(topt == 0?({const char*_tmp453="NULL";_tag_dynforward(_tmp453,
sizeof(char),_get_zero_arr_size(_tmp453,5));}):(struct _dynforward_ptr)Cyc_Absynpp_typ2string(*
topt));{void*_tmp44E[3]={& _tmp450,& _tmp451,& _tmp452};Cyc_fprintf(Cyc_stderr,({
const char*_tmp44F="topt=%s, e->topt->v=%s, e=%s\n";_tag_dynforward(_tmp44F,
sizeof(char),_get_zero_arr_size(_tmp44F,30));}),_tag_dynforward(_tmp44E,sizeof(
void*),3));}}}});({void*_tmp454[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp455="Cannot consume paths; do swap instead";
_tag_dynforward(_tmp455,sizeof(char),_get_zero_arr_size(_tmp455,38));}),
_tag_dynforward(_tmp454,sizeof(void*),0));});}{void*_tmp456=(void*)e->r;union Cyc_Absyn_Cnst_union
_tmp457;_LL26D: if(*((int*)_tmp456)!= 28)goto _LL26F;_LL26E: goto _LL270;_LL26F: if(*((
int*)_tmp456)!= 29)goto _LL271;_LL270: goto _LL272;_LL271: if(*((int*)_tmp456)!= 0)
goto _LL273;_tmp457=((struct Cyc_Absyn_Const_e_struct*)_tmp456)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp456)->f1).String_c).tag != 5)goto _LL273;_LL272:
return t;_LL273:;_LL274: t=Cyc_Tcutil_compress(t);{void*_tmp458=t;struct Cyc_Absyn_ArrayInfo
_tmp459;void*_tmp45A;struct Cyc_Absyn_Tqual _tmp45B;struct Cyc_Absyn_Exp*_tmp45C;
struct Cyc_Absyn_Conref*_tmp45D;_LL276: if(_tmp458 <= (void*)4)goto _LL278;if(*((int*)
_tmp458)!= 7)goto _LL278;_tmp459=((struct Cyc_Absyn_ArrayType_struct*)_tmp458)->f1;
_tmp45A=(void*)_tmp459.elt_type;_tmp45B=_tmp459.tq;_tmp45C=_tmp459.num_elts;
_tmp45D=_tmp459.zero_term;_LL277: {void*_tmp45F;struct _tuple6 _tmp45E=Cyc_Tcutil_addressof_props(
te,e);_tmp45F=_tmp45E.f2;{void*b=_tmp45C == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp461;_tmp461.tag=0;_tmp461.f1=(struct Cyc_Absyn_Exp*)
_tmp45C;_tmp461;});_tmp460;});t=Cyc_Absyn_atb_typ(_tmp45A,_tmp45F,_tmp45B,b,
_tmp45D);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL278:;_LL279:
return t;_LL275:;}_LL26C:;}}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp462=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp463;_LL27B: if(*((int*)_tmp462)!= 13)goto _LL27D;_tmp463=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp462)->f1;_LL27C: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp463);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp463->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp463->topt))->v,0));e->topt=_tmp463->topt;goto
_LL27A;_LL27D:;_LL27E: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp464=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp465;void*
_tmp466;struct Cyc_Absyn_Tqual _tmp467;struct Cyc_Absyn_PtrAtts _tmp468;void*_tmp469;
struct Cyc_Absyn_Conref*_tmp46A;struct Cyc_Absyn_Conref*_tmp46B;struct Cyc_Absyn_Conref*
_tmp46C;_LL280: if(_tmp464 <= (void*)4)goto _LL282;if(*((int*)_tmp464)!= 4)goto
_LL282;_tmp465=((struct Cyc_Absyn_PointerType_struct*)_tmp464)->f1;_tmp466=(void*)
_tmp465.elt_typ;_tmp467=_tmp465.elt_tq;_tmp468=_tmp465.ptr_atts;_tmp469=(void*)
_tmp468.rgn;_tmp46A=_tmp468.nullable;_tmp46B=_tmp468.bounds;_tmp46C=_tmp468.zero_term;
_LL281:{void*_tmp46D=Cyc_Tcutil_compress(_tmp466);struct Cyc_Absyn_FnInfo _tmp46E;
struct Cyc_List_List*_tmp46F;struct Cyc_Core_Opt*_tmp470;void*_tmp471;struct Cyc_List_List*
_tmp472;int _tmp473;struct Cyc_Absyn_VarargInfo*_tmp474;struct Cyc_List_List*
_tmp475;struct Cyc_List_List*_tmp476;_LL285: if(_tmp46D <= (void*)4)goto _LL287;if(*((
int*)_tmp46D)!= 8)goto _LL287;_tmp46E=((struct Cyc_Absyn_FnType_struct*)_tmp46D)->f1;
_tmp46F=_tmp46E.tvars;_tmp470=_tmp46E.effect;_tmp471=(void*)_tmp46E.ret_typ;
_tmp472=_tmp46E.args;_tmp473=_tmp46E.c_varargs;_tmp474=_tmp46E.cyc_varargs;
_tmp475=_tmp46E.rgn_po;_tmp476=_tmp46E.attributes;_LL286: if(_tmp46F != 0){struct
_RegionHandle _tmp477=_new_region("rgn");struct _RegionHandle*rgn=& _tmp477;
_push_region(rgn);{struct _tuple4 _tmp478=({struct _tuple4 _tmp485;_tmp485.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp485.f2=rgn;_tmp485;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp478,_tmp46F);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_Absyn_FnType_struct
_tmp483;_tmp483.tag=8;_tmp483.f1=({struct Cyc_Absyn_FnInfo _tmp484;_tmp484.tvars=0;
_tmp484.effect=_tmp470;_tmp484.ret_typ=(void*)_tmp471;_tmp484.args=_tmp472;
_tmp484.c_varargs=_tmp473;_tmp484.cyc_varargs=_tmp474;_tmp484.rgn_po=_tmp475;
_tmp484.attributes=_tmp476;_tmp484;});_tmp483;});_tmp482;}));struct Cyc_Absyn_PointerType_struct*
_tmp479=({struct Cyc_Absyn_PointerType_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));
_tmp47E[0]=({struct Cyc_Absyn_PointerType_struct _tmp47F;_tmp47F.tag=4;_tmp47F.f1=({
struct Cyc_Absyn_PtrInfo _tmp480;_tmp480.elt_typ=(void*)ftyp;_tmp480.elt_tq=
_tmp467;_tmp480.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp481;_tmp481.rgn=(void*)
_tmp469;_tmp481.nullable=_tmp46A;_tmp481.bounds=_tmp46B;_tmp481.zero_term=
_tmp46C;_tmp481.ptrloc=0;_tmp481;});_tmp480;});_tmp47F;});_tmp47E;});struct Cyc_Absyn_Exp*
_tmp47A=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp47C;_tmp47C.tag=14;_tmp47C.f1=_tmp47A;_tmp47C.f2=ts;_tmp47C;});_tmp47B;})));
e->topt=({struct Cyc_Core_Opt*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->v=(
void*)((void*)_tmp479);_tmp47D;});};_pop_region(rgn);}goto _LL284;_LL287:;_LL288:
goto _LL284;_LL284:;}goto _LL27F;_LL282:;_LL283: goto _LL27F;_LL27F:;}goto _LL27A;
_LL27A:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp486=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp487;struct _tuple1*_tmp488;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_List_List*
_tmp48A;struct Cyc_Core_Opt*_tmp48B;struct Cyc_List_List*_tmp48C;union Cyc_Absyn_Cnst_union
_tmp48D;union Cyc_Absyn_Cnst_union*_tmp48E;struct _tuple1*_tmp48F;void*_tmp490;
void*_tmp491;struct Cyc_List_List*_tmp492;struct Cyc_Absyn_Exp*_tmp493;void*
_tmp494;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Core_Opt*_tmp496;struct Cyc_Absyn_Exp*
_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*
_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*
_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*
_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_List_List*_tmp4A2;struct Cyc_Absyn_VarargCallInfo*
_tmp4A3;struct Cyc_Absyn_VarargCallInfo**_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;
struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_List_List*_tmp4A7;void*_tmp4A8;struct Cyc_Absyn_Exp*
_tmp4A9;void*_tmp4AA;void**_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*
_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;void*_tmp4B0;
void*_tmp4B1;void*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B4;struct _dynforward_ptr*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct
_dynforward_ptr*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;
struct Cyc_List_List*_tmp4BA;struct _tuple2*_tmp4BB;struct Cyc_List_List*_tmp4BC;
struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_Stmt*_tmp4BE;struct Cyc_Absyn_Vardecl*
_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C1;int _tmp4C2;int*
_tmp4C3;struct _tuple1*_tmp4C4;struct _tuple1**_tmp4C5;struct Cyc_List_List*_tmp4C6;
struct Cyc_List_List**_tmp4C7;struct Cyc_List_List*_tmp4C8;struct Cyc_Absyn_Aggrdecl*
_tmp4C9;struct Cyc_Absyn_Aggrdecl**_tmp4CA;void*_tmp4CB;struct Cyc_List_List*
_tmp4CC;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_Tuniondecl*_tmp4CE;struct Cyc_Absyn_Tunionfield*
_tmp4CF;struct _tuple1*_tmp4D0;struct _tuple1**_tmp4D1;struct Cyc_Absyn_Enumdecl*
_tmp4D2;struct Cyc_Absyn_Enumfield*_tmp4D3;struct _tuple1*_tmp4D4;struct _tuple1**
_tmp4D5;void*_tmp4D6;struct Cyc_Absyn_Enumfield*_tmp4D7;struct Cyc_Absyn_MallocInfo
_tmp4D8;int _tmp4D9;int*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;void**_tmp4DC;void***
_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp**_tmp4DF;int _tmp4E0;int*
_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E3;_LL28A: if(*((int*)
_tmp486)!= 13)goto _LL28C;_tmp487=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp486)->f1;_LL28B: Cyc_Tcexp_tcExpNoInst(te,0,_tmp487);return;_LL28C: if(*((int*)
_tmp486)!= 2)goto _LL28E;_tmp488=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp486)->f1;
_LL28D: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp488);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL28E: if(*((int*)_tmp486)!= 10)goto _LL290;_tmp489=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp486)->f1;_tmp48A=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp486)->f2;_LL28F:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp489,_tmp48A);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL290: if(*((int*)_tmp486)!= 37)goto _LL292;_tmp48B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp486)->f1;_tmp48C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp486)->f2;
_LL291: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp48C);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL292: if(*((int*)_tmp486)!= 0)goto _LL294;_tmp48D=((struct Cyc_Absyn_Const_e_struct*)
_tmp486)->f1;_tmp48E=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp486)->f1;_LL293: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp48E,e);goto _LL289;_LL294: if(*((int*)_tmp486)!= 1)goto _LL296;_tmp48F=((struct
Cyc_Absyn_Var_e_struct*)_tmp486)->f1;_tmp490=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp486)->f2;_LL295: t=Cyc_Tcexp_tcVar(te,loc,_tmp48F,_tmp490);goto _LL289;_LL296:
if(*((int*)_tmp486)!= 3)goto _LL298;_tmp491=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp486)->f1;_tmp492=((struct Cyc_Absyn_Primop_e_struct*)_tmp486)->f2;_LL297: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp491,_tmp492);goto _LL289;_LL298: if(*((int*)
_tmp486)!= 5)goto _LL29A;_tmp493=((struct Cyc_Absyn_Increment_e_struct*)_tmp486)->f1;
_tmp494=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp486)->f2;_LL299: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp493,_tmp494);goto _LL289;_LL29A: if(*((int*)_tmp486)!= 4)goto
_LL29C;_tmp495=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp486)->f1;_tmp496=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp486)->f2;_tmp497=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp486)->f3;_LL29B: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp495,_tmp496,_tmp497);
goto _LL289;_LL29C: if(*((int*)_tmp486)!= 6)goto _LL29E;_tmp498=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp486)->f1;_tmp499=((struct Cyc_Absyn_Conditional_e_struct*)_tmp486)->f2;
_tmp49A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp486)->f3;_LL29D: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp498,_tmp499,_tmp49A);goto _LL289;_LL29E: if(*((int*)_tmp486)!= 7)
goto _LL2A0;_tmp49B=((struct Cyc_Absyn_And_e_struct*)_tmp486)->f1;_tmp49C=((struct
Cyc_Absyn_And_e_struct*)_tmp486)->f2;_LL29F: t=Cyc_Tcexp_tcAnd(te,loc,_tmp49B,
_tmp49C);goto _LL289;_LL2A0: if(*((int*)_tmp486)!= 8)goto _LL2A2;_tmp49D=((struct
Cyc_Absyn_Or_e_struct*)_tmp486)->f1;_tmp49E=((struct Cyc_Absyn_Or_e_struct*)
_tmp486)->f2;_LL2A1: t=Cyc_Tcexp_tcOr(te,loc,_tmp49D,_tmp49E);goto _LL289;_LL2A2:
if(*((int*)_tmp486)!= 9)goto _LL2A4;_tmp49F=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp486)->f1;_tmp4A0=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp486)->f2;_LL2A3: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp49F,_tmp4A0);goto _LL289;_LL2A4: if(*((int*)
_tmp486)!= 11)goto _LL2A6;_tmp4A1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp486)->f1;
_tmp4A2=((struct Cyc_Absyn_FnCall_e_struct*)_tmp486)->f2;_tmp4A3=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp486)->f3;_tmp4A4=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp486)->f3;_LL2A5: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4A1,_tmp4A2,_tmp4A4);
goto _LL289;_LL2A6: if(*((int*)_tmp486)!= 12)goto _LL2A8;_tmp4A5=((struct Cyc_Absyn_Throw_e_struct*)
_tmp486)->f1;_LL2A7: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4A5);goto _LL289;_LL2A8:
if(*((int*)_tmp486)!= 14)goto _LL2AA;_tmp4A6=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp486)->f1;_tmp4A7=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp486)->f2;_LL2A9:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4A6,_tmp4A7);goto _LL289;_LL2AA: if(*((
int*)_tmp486)!= 15)goto _LL2AC;_tmp4A8=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp486)->f1;_tmp4A9=((struct Cyc_Absyn_Cast_e_struct*)_tmp486)->f2;_tmp4AA=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp486)->f4;_tmp4AB=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp486)->f4);_LL2AB: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4A8,_tmp4A9,(void**)
_tmp4AB);goto _LL289;_LL2AC: if(*((int*)_tmp486)!= 16)goto _LL2AE;_tmp4AC=((struct
Cyc_Absyn_Address_e_struct*)_tmp486)->f1;_LL2AD: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4AC);goto _LL289;_LL2AE: if(*((int*)_tmp486)!= 17)goto _LL2B0;_tmp4AD=((
struct Cyc_Absyn_New_e_struct*)_tmp486)->f1;_tmp4AE=((struct Cyc_Absyn_New_e_struct*)
_tmp486)->f2;_LL2AF: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4AD,e,_tmp4AE);goto _LL289;
_LL2B0: if(*((int*)_tmp486)!= 19)goto _LL2B2;_tmp4AF=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp486)->f1;_LL2B1: {void*_tmp4E4=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4AF);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4E4);goto _LL289;}_LL2B2: if(*((int*)_tmp486)!= 18)goto _LL2B4;
_tmp4B0=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp486)->f1;_LL2B3: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4B0);goto _LL289;_LL2B4: if(*((int*)_tmp486)!= 20)goto _LL2B6;
_tmp4B1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp486)->f1;_tmp4B2=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp486)->f2;_LL2B5: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4B1,_tmp4B2);goto _LL289;_LL2B6: if(*((int*)_tmp486)!= 21)goto _LL2B8;
_LL2B7:({void*_tmp4E5[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4E6="gen() not in top-level initializer";
_tag_dynforward(_tmp4E6,sizeof(char),_get_zero_arr_size(_tmp4E6,35));}),
_tag_dynforward(_tmp4E5,sizeof(void*),0));});return;_LL2B8: if(*((int*)_tmp486)!= 
22)goto _LL2BA;_tmp4B3=((struct Cyc_Absyn_Deref_e_struct*)_tmp486)->f1;_LL2B9: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4B3);goto _LL289;_LL2BA: if(*((int*)_tmp486)!= 
23)goto _LL2BC;_tmp4B4=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp486)->f1;
_tmp4B5=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp486)->f2;_LL2BB: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp4B4,_tmp4B5);goto _LL289;_LL2BC: if(*((int*)_tmp486)!= 24)goto
_LL2BE;_tmp4B6=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp486)->f1;_tmp4B7=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp486)->f2;_LL2BD: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp4B6,_tmp4B7);goto _LL289;_LL2BE: if(*((int*)_tmp486)!= 25)goto
_LL2C0;_tmp4B8=((struct Cyc_Absyn_Subscript_e_struct*)_tmp486)->f1;_tmp4B9=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp486)->f2;_LL2BF: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4B8,_tmp4B9);goto _LL289;_LL2C0: if(*((int*)_tmp486)!= 26)goto
_LL2C2;_tmp4BA=((struct Cyc_Absyn_Tuple_e_struct*)_tmp486)->f1;_LL2C1: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4BA);goto _LL289;_LL2C2: if(*((int*)_tmp486)!= 27)goto _LL2C4;
_tmp4BB=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp486)->f1;_tmp4BC=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp486)->f2;_LL2C3: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4BB,_tmp4BC);goto
_LL289;_LL2C4: if(*((int*)_tmp486)!= 28)goto _LL2C6;_tmp4BD=((struct Cyc_Absyn_Array_e_struct*)
_tmp486)->f1;_LL2C5: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4E7=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4E8;void*_tmp4E9;void**
_tmp4EA;struct Cyc_Absyn_Conref*_tmp4EB;_LL2D9: if(_tmp4E7 <= (void*)4)goto _LL2DB;
if(*((int*)_tmp4E7)!= 7)goto _LL2DB;_tmp4E8=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4E7)->f1;_tmp4E9=(void*)_tmp4E8.elt_type;_tmp4EA=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4E7)->f1).elt_type;_tmp4EB=_tmp4E8.zero_term;_LL2DA: elt_topt=(void**)_tmp4EA;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4EB);
goto _LL2D8;_LL2DB:;_LL2DC: goto _LL2D8;_LL2D8:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4BD);goto _LL289;}_LL2C6: if(*((int*)_tmp486)!= 38)goto
_LL2C8;_tmp4BE=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp486)->f1;_LL2C7: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4BE);goto _LL289;_LL2C8: if(*((int*)_tmp486)!= 29)goto _LL2CA;
_tmp4BF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp486)->f1;_tmp4C0=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp486)->f2;_tmp4C1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp486)->f3;_tmp4C2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp486)->f4;
_tmp4C3=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp486)->f4;_LL2C9: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4BF,_tmp4C0,_tmp4C1,_tmp4C3);goto
_LL289;_LL2CA: if(*((int*)_tmp486)!= 30)goto _LL2CC;_tmp4C4=((struct Cyc_Absyn_Struct_e_struct*)
_tmp486)->f1;_tmp4C5=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp486)->f1;_tmp4C6=((struct Cyc_Absyn_Struct_e_struct*)_tmp486)->f2;_tmp4C7=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp486)->f2;_tmp4C8=((
struct Cyc_Absyn_Struct_e_struct*)_tmp486)->f3;_tmp4C9=((struct Cyc_Absyn_Struct_e_struct*)
_tmp486)->f4;_tmp4CA=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp486)->f4;_LL2CB: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4C5,_tmp4C7,_tmp4C8,
_tmp4CA);goto _LL289;_LL2CC: if(*((int*)_tmp486)!= 31)goto _LL2CE;_tmp4CB=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp486)->f1;_tmp4CC=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp486)->f2;_LL2CD: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4CB,_tmp4CC);goto _LL289;
_LL2CE: if(*((int*)_tmp486)!= 32)goto _LL2D0;_tmp4CD=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp486)->f1;_tmp4CE=((struct Cyc_Absyn_Tunion_e_struct*)_tmp486)->f2;_tmp4CF=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp486)->f3;_LL2CF: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4CD,_tmp4CE,_tmp4CF);goto _LL289;_LL2D0: if(*((int*)_tmp486)!= 33)goto
_LL2D2;_tmp4D0=((struct Cyc_Absyn_Enum_e_struct*)_tmp486)->f1;_tmp4D1=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp486)->f1;_tmp4D2=((struct Cyc_Absyn_Enum_e_struct*)
_tmp486)->f2;_tmp4D3=((struct Cyc_Absyn_Enum_e_struct*)_tmp486)->f3;_LL2D1:*
_tmp4D1=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4D3))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4ED;_tmp4ED.tag=12;_tmp4ED.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4D2))->name;_tmp4ED.f2=_tmp4D2;_tmp4ED;});_tmp4EC;});goto _LL289;
_LL2D2: if(*((int*)_tmp486)!= 34)goto _LL2D4;_tmp4D4=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp486)->f1;_tmp4D5=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp486)->f1;_tmp4D6=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp486)->f2;
_tmp4D7=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp486)->f3;_LL2D3:*_tmp4D5=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4D7))->name;t=_tmp4D6;goto _LL289;
_LL2D4: if(*((int*)_tmp486)!= 35)goto _LL2D6;_tmp4D8=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp486)->f1;_tmp4D9=_tmp4D8.is_calloc;_tmp4DA=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp486)->f1).is_calloc;_tmp4DB=_tmp4D8.rgn;_tmp4DC=_tmp4D8.elt_type;_tmp4DD=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp486)->f1).elt_type;_tmp4DE=
_tmp4D8.num_elts;_tmp4DF=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp486)->f1).num_elts;_tmp4E0=_tmp4D8.fat_result;_tmp4E1=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp486)->f1).fat_result;_LL2D5: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4DB,_tmp4DD,
_tmp4DF,_tmp4DA,_tmp4E1);goto _LL289;_LL2D6: if(*((int*)_tmp486)!= 36)goto _LL289;
_tmp4E2=((struct Cyc_Absyn_Swap_e_struct*)_tmp486)->f1;_tmp4E3=((struct Cyc_Absyn_Swap_e_struct*)
_tmp486)->f2;_LL2D7: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4E2,_tmp4E3);goto _LL289;
_LL289:;}e->topt=({struct Cyc_Core_Opt*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));
_tmp4EE->v=(void*)t;_tmp4EE;});}
