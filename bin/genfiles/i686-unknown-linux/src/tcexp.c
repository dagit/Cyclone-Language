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
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
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
Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple4{struct
Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;
void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
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
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,
struct _dynforward_ptr,struct Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,struct Cyc_Position_Segment*);void*
Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
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
ap),_tag_dynforward(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp1;}));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons
_tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler))_tmp4=1;if(!
_tmp4){{struct _RegionHandle _tmp5=_new_region("r");struct _RegionHandle*r=& _tmp5;
_push_region(r);{void*_tmp6=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q);void*_tmp7;
struct Cyc_Absyn_Enumdecl*_tmp8;struct Cyc_Absyn_Enumfield*_tmp9;void*_tmpA;struct
Cyc_Absyn_Enumfield*_tmpB;struct Cyc_Absyn_Tuniondecl*_tmpC;struct Cyc_Absyn_Tunionfield*
_tmpD;_LL1: if(*((int*)_tmp6)!= 0)goto _LL3;_tmp7=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp6)->f1;_LL2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmpE=
_cycalloc(sizeof(*_tmpE));_tmpE[0]=({struct Cyc_Absyn_Var_e_struct _tmpF;_tmpF.tag=
1;_tmpF.f1=q;_tmpF.f2=(void*)_tmp7;_tmpF;});_tmpE;})));goto _LL0;_LL3: if(*((int*)
_tmp6)!= 3)goto _LL5;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)_tmp6)->f1;_tmp9=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp6)->f2;_LL4:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Enum_e_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=({
struct Cyc_Absyn_Enum_e_struct _tmp11;_tmp11.tag=33;_tmp11.f1=q;_tmp11.f2=(struct
Cyc_Absyn_Enumdecl*)_tmp8;_tmp11.f3=(struct Cyc_Absyn_Enumfield*)_tmp9;_tmp11;});
_tmp10;})));goto _LL0;_LL5: if(*((int*)_tmp6)!= 4)goto _LL7;_tmpA=(void*)((struct
Cyc_Tcenv_AnonEnumRes_struct*)_tmp6)->f1;_tmpB=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp6)->f2;_LL6:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_e_struct*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Absyn_AnonEnum_e_struct
_tmp13;_tmp13.tag=34;_tmp13.f1=q;_tmp13.f2=(void*)_tmpA;_tmp13.f3=(struct Cyc_Absyn_Enumfield*)
_tmpB;_tmp13;});_tmp12;})));goto _LL0;_LL7: if(*((int*)_tmp6)!= 2)goto _LL9;_tmpC=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp6)->f1;_tmpD=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp6)->f2;_LL8:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp15;_tmp15.tag=32;_tmp15.f1=0;_tmp15.f2=_tmpC;_tmp15.f3=_tmpD;_tmp15;});
_tmp14;})));goto _LL0;_LL9: if(*((int*)_tmp6)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct
_tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
q));{void*_tmp16[1]={& _tmp18};Cyc_Tcutil_terr(e->loc,({const char*_tmp17="bad occurrence of type name %s";
_tag_dynforward(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,31));}),
_tag_dynforward(_tmp16,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({
struct Cyc_Absyn_Var_e_struct _tmp1A;_tmp1A.tag=1;_tmp1A.f1=q;_tmp1A.f2=(void*)((
void*)0);_tmp1A;});_tmp19;})));goto _LL0;_LL0:;};_pop_region(r);};_pop_handler();}
else{void*_tmp3=(void*)_exn_thrown;void*_tmp1C=_tmp3;_LLC: if(_tmp1C != Cyc_Dict_Absent)
goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp1D=
_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_Absyn_Var_e_struct _tmp1E;_tmp1E.tag=
1;_tmp1E.f1=q;_tmp1E.f2=(void*)((void*)0);_tmp1E;});_tmp1D;})));goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp1C);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){return({struct _tuple8*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->f1=0;_tmp1F->f2=
e;_tmp1F;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp20=(
void*)e1->r;struct _tuple1*_tmp21;_LL11: if(*((int*)_tmp20)!= 2)goto _LL13;_tmp21=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp20)->f1;_LL12: {struct _handler_cons
_tmp22;_push_handler(& _tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;
if(!_tmp24){{struct _RegionHandle _tmp25=_new_region("r");struct _RegionHandle*r=&
_tmp25;_push_region(r);{void*_tmp26=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,
_tmp21);void*_tmp27;struct Cyc_Absyn_Tuniondecl*_tmp28;struct Cyc_Absyn_Tunionfield*
_tmp29;struct Cyc_Absyn_Aggrdecl*_tmp2A;_LL16: if(*((int*)_tmp26)!= 0)goto _LL18;
_tmp27=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp26)->f1;_LL17:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));
_tmp2B[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp2C;_tmp2C.tag=11;_tmp2C.f1=e1;
_tmp2C.f2=es;_tmp2C.f3=0;_tmp2C;});_tmp2B;})));_npop_handler(1);return;_LL18: if(*((
int*)_tmp26)!= 2)goto _LL1A;_tmp28=((struct Cyc_Tcenv_TunionRes_struct*)_tmp26)->f1;
_tmp29=((struct Cyc_Tcenv_TunionRes_struct*)_tmp26)->f2;_LL19: if(_tmp29->typs == 0)({
struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp29->name));{void*_tmp2D[1]={&
_tmp2F};Cyc_Tcutil_terr(e->loc,({const char*_tmp2E="%s is a constant, not a function";
_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,33));}),
_tag_dynforward(_tmp2D,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_e_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30[0]=({
struct Cyc_Absyn_Tunion_e_struct _tmp31;_tmp31.tag=32;_tmp31.f1=es;_tmp31.f2=
_tmp28;_tmp31.f3=_tmp29;_tmp31;});_tmp30;})));_npop_handler(1);return;_LL1A: if(*((
int*)_tmp26)!= 1)goto _LL1C;_tmp2A=((struct Cyc_Tcenv_AggrRes_struct*)_tmp26)->f1;
_LL1B: {struct Cyc_List_List*_tmp32=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp33=
_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_Struct_e_struct _tmp34;
_tmp34.tag=30;_tmp34.f1=_tmp2A->name;_tmp34.f2=0;_tmp34.f3=_tmp32;_tmp34.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp2A;_tmp34;});_tmp33;})));_npop_handler(1);return;}
_LL1C: if(*((int*)_tmp26)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp26)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp21));{
void*_tmp35[1]={& _tmp37};Cyc_Tcutil_terr(e->loc,({const char*_tmp36="%s is an enum constructor, not a function";
_tag_dynforward(_tmp36,sizeof(char),_get_zero_arr_size(_tmp36,42));}),
_tag_dynforward(_tmp35,sizeof(void*),1));}});_npop_handler(1);return;_LL15:;};
_pop_region(r);};_pop_handler();}else{void*_tmp23=(void*)_exn_thrown;void*_tmp39=
_tmp23;_LL21: if(_tmp39 != Cyc_Dict_Absent)goto _LL23;_LL22:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3B;_tmp3B.tag=11;_tmp3B.f1=e1;_tmp3B.f2=
es;_tmp3B.f3=0;_tmp3B;});_tmp3A;})));return;_LL23:;_LL24:(void)_throw(_tmp39);
_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp3D;_tmp3D.tag=11;_tmp3D.f1=e1;_tmp3D.f2=es;_tmp3D.f3=0;_tmp3D;});_tmp3C;})));
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3F;_tmp3F.tag=28;_tmp3F.f1=des;_tmp3F;});_tmp3E;})));return;}{void*t=*topt;
void*_tmp40=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp41;union Cyc_Absyn_AggrInfoU_union
_tmp42;struct Cyc_Absyn_ArrayInfo _tmp43;void*_tmp44;struct Cyc_Absyn_Tqual _tmp45;
_LL26: if(_tmp40 <= (void*)4)goto _LL2C;if(*((int*)_tmp40)!= 10)goto _LL28;_tmp41=((
struct Cyc_Absyn_AggrType_struct*)_tmp40)->f1;_tmp42=_tmp41.aggr_info;_LL27:{
union Cyc_Absyn_AggrInfoU_union _tmp46=_tmp42;struct Cyc_Absyn_Aggrdecl**_tmp47;
struct Cyc_Absyn_Aggrdecl*_tmp48;_LL2F: if((_tmp46.UnknownAggr).tag != 0)goto _LL31;
_LL30:({void*_tmp49[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A="struct type not properly set";
_tag_dynforward(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,29));}),
_tag_dynforward(_tmp49,sizeof(void*),0));});_LL31: if((_tmp46.KnownAggr).tag != 1)
goto _LL2E;_tmp47=(_tmp46.KnownAggr).f1;_tmp48=*_tmp47;_LL32:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[
0]=({struct Cyc_Absyn_Struct_e_struct _tmp4C;_tmp4C.tag=30;_tmp4C.f1=_tmp48->name;
_tmp4C.f2=0;_tmp4C.f3=des;_tmp4C.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp48;_tmp4C;});
_tmp4B;})));_LL2E:;}goto _LL25;_LL28: if(*((int*)_tmp40)!= 7)goto _LL2A;_tmp43=((
struct Cyc_Absyn_ArrayType_struct*)_tmp40)->f1;_tmp44=(void*)_tmp43.elt_type;
_tmp45=_tmp43.tq;_LL29:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_Array_e_struct
_tmp4E;_tmp4E.tag=28;_tmp4E.f1=des;_tmp4E;});_tmp4D;})));goto _LL25;_LL2A: if(*((
int*)_tmp40)!= 11)goto _LL2C;_LL2B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp50;_tmp50.tag=31;_tmp50.f1=(void*)t;_tmp50.f2=des;_tmp50;});_tmp4F;})));goto
_LL25;_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Absyn_Array_e_struct
_tmp52;_tmp52.tag=28;_tmp52.f1=des;_tmp52;});_tmp51;})));goto _LL25;_LL25:;}}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct
_tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg_part);{void*_tmp53[2]={& _tmp55,& _tmp56};Cyc_Tcutil_terr(e->loc,({const char*
_tmp54="test of %s has type %s instead of integral or * type";_tag_dynforward(
_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,53));}),_tag_dynforward(_tmp53,
sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union*c,struct Cyc_Absyn_Exp*
e){void*t;{union Cyc_Absyn_Cnst_union _tmp57=*((union Cyc_Absyn_Cnst_union*)
_check_null(c));void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*_tmp5C;void*
_tmp5D;int _tmp5E;struct _dynforward_ptr _tmp5F;_LL34: if((_tmp57.Char_c).tag != 0)
goto _LL36;_tmp58=(_tmp57.Char_c).f1;if((int)_tmp58 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;
goto _LL33;_LL36: if((_tmp57.Char_c).tag != 0)goto _LL38;_tmp59=(_tmp57.Char_c).f1;
if((int)_tmp59 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;_LL38: if((
_tmp57.Char_c).tag != 0)goto _LL3A;_tmp5A=(_tmp57.Char_c).f1;if((int)_tmp5A != 2)
goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;_LL3A: if((_tmp57.Short_c).tag != 1)
goto _LL3C;_tmp5B=(_tmp57.Short_c).f1;_LL3B: t=_tmp5B == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if((_tmp57.LongLong_c).tag != 3)goto _LL3E;
_tmp5C=(_tmp57.LongLong_c).f1;_LL3D: t=_tmp5C == (void*)1?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL33;_LL3E: if((_tmp57.Float_c).tag != 4)goto _LL40;
_LL3F: t=Cyc_Absyn_float_typ;goto _LL33;_LL40: if((_tmp57.Int_c).tag != 2)goto _LL42;
_tmp5D=(_tmp57.Int_c).f1;_tmp5E=(_tmp57.Int_c).f2;_LL41: if(topt == 0)t=_tmp5D == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp60=Cyc_Tcutil_compress(*
topt);void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;
void*_tmp67;void*_tmp68;void*_tmp69;_LL47: if(_tmp60 <= (void*)4)goto _LL53;if(*((
int*)_tmp60)!= 5)goto _LL49;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f1;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f2;if((int)
_tmp62 != 0)goto _LL49;_LL48:{void*_tmp6A=_tmp61;_LL56: if((int)_tmp6A != 1)goto
_LL58;_LL57: t=Cyc_Absyn_uchar_typ;goto _LL55;_LL58: if((int)_tmp6A != 0)goto _LL5A;
_LL59: t=Cyc_Absyn_schar_typ;goto _LL55;_LL5A: if((int)_tmp6A != 2)goto _LL55;_LL5B: t=
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6B;(_tmp6B.Char_c).tag=0;(_tmp6B.Char_c).f1=(void*)_tmp61;(_tmp6B.Char_c).f2=(
char)_tmp5E;_tmp6B;});goto _LL46;_LL49: if(*((int*)_tmp60)!= 5)goto _LL4B;_tmp63=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f2;if((int)_tmp64 != 1)goto _LL4B;_LL4A: t=_tmp63 == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6C;(_tmp6C.Short_c).tag=1;(_tmp6C.Short_c).f1=(void*)_tmp63;(_tmp6C.Short_c).f2=(
short)_tmp5E;_tmp6C;});goto _LL46;_LL4B: if(*((int*)_tmp60)!= 5)goto _LL4D;_tmp65=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp66=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f2;if((int)_tmp66 != 2)goto _LL4D;_LL4C: t=_tmp65 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(*((int*)_tmp60)!= 5)goto _LL4F;_tmp67=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f2;if((int)_tmp68 != 3)goto _LL4F;_LL4E: t=_tmp67 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4F: if(*((int*)_tmp60)!= 4)goto _LL51;if(!(_tmp5E
== 0))goto _LL51;_LL50:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Absyn_Const_e_struct
_tmp6E;_tmp6E.tag=0;_tmp6E.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6F;(_tmp6F.Null_c).tag=6;_tmp6F;});_tmp6E;});_tmp6D;})));{struct Cyc_List_List*
_tmp70=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_PointerType_struct
_tmp72;_tmp72.tag=4;_tmp72.f1=({struct Cyc_Absyn_PtrInfo _tmp73;_tmp73.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->v=_tmp70;_tmp76;}));_tmp73.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp73.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp74;_tmp74.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp75=_cycalloc(
sizeof(*_tmp75));_tmp75->v=_tmp70;_tmp75;}));_tmp74.nullable=Cyc_Absyn_true_conref;
_tmp74.bounds=Cyc_Absyn_empty_conref();_tmp74.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp74.ptrloc=0;_tmp74;});_tmp73;});_tmp72;});_tmp71;});
goto _LL46;}_LL51: if(*((int*)_tmp60)!= 18)goto _LL53;_tmp69=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp60)->f1;_LL52: {struct Cyc_Absyn_TypeInt_struct*_tmp77=({struct Cyc_Absyn_TypeInt_struct*
_tmp7E=_cycalloc_atomic(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp7F;_tmp7F.tag=19;_tmp7F.f1=_tmp5E;_tmp7F;});_tmp7E;});if(!Cyc_Tcutil_unify(
_tmp69,(void*)_tmp77)){({struct Cyc_String_pa_struct _tmp7B;_tmp7B.tag=0;_tmp7B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)
_tmp77));{struct Cyc_String_pa_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp69));{void*
_tmp78[2]={& _tmp7A,& _tmp7B};Cyc_Tcutil_terr(loc,({const char*_tmp79="expecting %s but found %s";
_tag_dynforward(_tmp79,sizeof(char),_get_zero_arr_size(_tmp79,26));}),
_tag_dynforward(_tmp78,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[
0]=({struct Cyc_Absyn_TagType_struct _tmp7D;_tmp7D.tag=18;_tmp7D.f1=(void*)((void*)
_tmp77);_tmp7D;});_tmp7C;});goto _LL46;}_LL53:;_LL54: t=_tmp5D == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if((_tmp57.String_c).tag != 
5)goto _LL44;_tmp5F=(_tmp57.String_c).f1;_LL43: {int len=(int)_get_dynforward_size(
_tmp5F,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp8B;(_tmp8B.Int_c).tag=2;(_tmp8B.Int_c).f1=(void*)((
void*)1);(_tmp8B.Int_c).f2=len;_tmp8B;}),loc);elen->topt=({struct Cyc_Core_Opt*
_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->v=(void*)Cyc_Absyn_uint_typ;_tmp80;});
t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp82;_tmp82.tag=0;_tmp82.f1=elen;_tmp82;});
_tmp81;}),Cyc_Absyn_true_conref);if(topt != 0){void*_tmp83=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_ArrayInfo _tmp84;struct Cyc_Absyn_Tqual _tmp85;_LL5D: if(
_tmp83 <= (void*)4)goto _LL61;if(*((int*)_tmp83)!= 7)goto _LL5F;_tmp84=((struct Cyc_Absyn_ArrayType_struct*)
_tmp83)->f1;_tmp85=_tmp84.tq;_LL5E: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
_tmp85,(struct Cyc_Absyn_Exp*)elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),
0);_LL5F: if(*((int*)_tmp83)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t)
 && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->v=(void*)t;_tmp86;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87->v=Cyc_Tcenv_lookup_type_vars(te);_tmp87;})),Cyc_Absyn_const_tqual(
0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp88=_cycalloc(sizeof(*_tmp88));
_tmp88[0]=({struct Cyc_Absyn_Upper_b_struct _tmp89;_tmp89.tag=0;_tmp89.f1=elen;
_tmp89;});_tmp88;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp8A=_cycalloc(sizeof(*_tmp8A));
_tmp8A->v=(void*)t;_tmp8A;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}_LL44: if((_tmp57.Null_c).tag
!= 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8C=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D[0]=({struct Cyc_Absyn_PointerType_struct _tmp8E;_tmp8E.tag=4;_tmp8E.f1=({
struct Cyc_Absyn_PtrInfo _tmp8F;_tmp8F.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp92=_cycalloc(sizeof(*
_tmp92));_tmp92->v=_tmp8C;_tmp92;}));_tmp8F.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp8F.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp90;_tmp90.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp91=_cycalloc(
sizeof(*_tmp91));_tmp91->v=_tmp8C;_tmp91;}));_tmp90.nullable=Cyc_Absyn_true_conref;
_tmp90.bounds=Cyc_Absyn_empty_conref();_tmp90.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp90.ptrloc=0;_tmp90;});_tmp8F;});_tmp8E;});_tmp8D;});
goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp93=b;struct Cyc_Absyn_Vardecl*
_tmp94;struct Cyc_Absyn_Fndecl*_tmp95;struct Cyc_Absyn_Vardecl*_tmp96;struct Cyc_Absyn_Vardecl*
_tmp97;struct Cyc_Absyn_Vardecl*_tmp98;_LL64: if((int)_tmp93 != 0)goto _LL66;_LL65:
return({struct Cyc_String_pa_struct _tmp9B;_tmp9B.tag=0;_tmp9B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmp99[
1]={& _tmp9B};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp9A="undeclared identifier: %s";
_tag_dynforward(_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,26));}),
_tag_dynforward(_tmp99,sizeof(void*),1));}});_LL66: if(_tmp93 <= (void*)1)goto
_LL68;if(*((int*)_tmp93)!= 0)goto _LL68;_tmp94=((struct Cyc_Absyn_Global_b_struct*)
_tmp93)->f1;_LL67:*q=*_tmp94->name;return(void*)_tmp94->type;_LL68: if(_tmp93 <= (
void*)1)goto _LL6A;if(*((int*)_tmp93)!= 1)goto _LL6A;_tmp95=((struct Cyc_Absyn_Funname_b_struct*)
_tmp93)->f1;_LL69:*q=*_tmp95->name;return Cyc_Tcutil_fndecl2typ(_tmp95);_LL6A: if(
_tmp93 <= (void*)1)goto _LL6C;if(*((int*)_tmp93)!= 4)goto _LL6C;_tmp96=((struct Cyc_Absyn_Pat_b_struct*)
_tmp93)->f1;_LL6B: _tmp97=_tmp96;goto _LL6D;_LL6C: if(_tmp93 <= (void*)1)goto _LL6E;
if(*((int*)_tmp93)!= 3)goto _LL6E;_tmp97=((struct Cyc_Absyn_Local_b_struct*)_tmp93)->f1;
_LL6D: _tmp98=_tmp97;goto _LL6F;_LL6E: if(_tmp93 <= (void*)1)goto _LL63;if(*((int*)
_tmp93)!= 2)goto _LL63;_tmp98=((struct Cyc_Absyn_Param_b_struct*)_tmp93)->f1;_LL6F:(*
q).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp9C;(_tmp9C.Loc_n).tag=
0;_tmp9C;});return(void*)_tmp98->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmp9D=(void*)
fmt->r;union Cyc_Absyn_Cnst_union _tmp9E;struct _dynforward_ptr _tmp9F;struct Cyc_Absyn_Exp*
_tmpA0;struct Cyc_Absyn_Exp _tmpA1;void*_tmpA2;union Cyc_Absyn_Cnst_union _tmpA3;
struct _dynforward_ptr _tmpA4;_LL71: if(*((int*)_tmp9D)!= 0)goto _LL73;_tmp9E=((
struct Cyc_Absyn_Const_e_struct*)_tmp9D)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp9D)->f1).String_c).tag != 5)goto _LL73;_tmp9F=(_tmp9E.String_c).f1;_LL72:
_tmpA4=_tmp9F;goto _LL74;_LL73: if(*((int*)_tmp9D)!= 15)goto _LL75;_tmpA0=((struct
Cyc_Absyn_Cast_e_struct*)_tmp9D)->f2;_tmpA1=*_tmpA0;_tmpA2=(void*)_tmpA1.r;if(*((
int*)_tmpA2)!= 0)goto _LL75;_tmpA3=((struct Cyc_Absyn_Const_e_struct*)_tmpA2)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpA2)->f1).String_c).tag != 5)goto _LL75;
_tmpA4=(_tmpA3.String_c).f1;_LL74: desc_types=type_getter(te,(struct
_dynforward_ptr)_tmpA4,fmt->loc);goto _LL70;_LL75:;_LL76: if(opt_args != 0){struct
Cyc_List_List*_tmpA5=(struct Cyc_List_List*)opt_args->v;for(0;_tmpA5 != 0;_tmpA5=
_tmpA5->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpA5->hd);if(Cyc_Tcutil_is_unique_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpA5->hd)->topt))->v)
 && !Cyc_Tcutil_is_unique_path((struct Cyc_Absyn_Exp*)_tmpA5->hd))({struct Cyc_Int_pa_struct
_tmpA8;_tmpA8.tag=1;_tmpA8.f1=(unsigned long)332;{void*_tmpA6[1]={& _tmpA8};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmpA5->hd)->loc,({const char*_tmpA7="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmpA7,sizeof(char),_get_zero_arr_size(_tmpA7,41));}),
_tag_dynforward(_tmpA6,sizeof(void*),1));}});}}return;_LL70:;}if(opt_args != 0){
struct Cyc_List_List*_tmpA9=(struct Cyc_List_List*)opt_args->v;for(0;desc_types != 
0  && _tmpA9 != 0;(desc_types=desc_types->tl,_tmpA9=_tmpA9->tl)){void*t=(void*)
desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpA9->hd;Cyc_Tcexp_tcExp(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpAD;_tmpAD.tag=0;_tmpAD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmpAC;_tmpAC.tag=0;_tmpAC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpAA[2]={& _tmpAC,& _tmpAD};Cyc_Tcutil_terr(e->loc,({const char*_tmpAB="descriptor has type \n%s\n but argument has type \n%s";
_tag_dynforward(_tmpAB,sizeof(char),_get_zero_arr_size(_tmpAB,51));}),
_tag_dynforward(_tmpAA,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}if(
Cyc_Tcutil_is_unique_pointer(t) && !Cyc_Tcutil_is_unique_path(e))({struct Cyc_Int_pa_struct
_tmpB0;_tmpB0.tag=1;_tmpB0.f1=(unsigned long)351;{void*_tmpAE[1]={& _tmpB0};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmpA9->hd)->loc,({const char*_tmpAF="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmpAF,sizeof(char),_get_zero_arr_size(_tmpAF,41));}),
_tag_dynforward(_tmpAE,sizeof(void*),1));}});}if(desc_types != 0)({void*_tmpB1[0]={};
Cyc_Tcutil_terr(fmt->loc,({const char*_tmpB2="too few arguments";_tag_dynforward(
_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,18));}),_tag_dynforward(_tmpB1,
sizeof(void*),0));});if(_tmpA9 != 0)({void*_tmpB3[0]={};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpA9->hd)->loc,({const char*_tmpB4="too many arguments";
_tag_dynforward(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,19));}),
_tag_dynforward(_tmpB3,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpB5=p;_LL78: if((int)_tmpB5 != 0)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if((int)_tmpB5 != 2)goto _LL7C;_LL7B: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB6[1]={& _tmpB8};Cyc_Tcutil_terr(
loc,({const char*_tmpB7="expecting arithmetic type but found %s";_tag_dynforward(
_tmpB7,sizeof(char),_get_zero_arr_size(_tmpB7,39));}),_tag_dynforward(_tmpB6,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7C: if((int)_tmpB5 != 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpBB;_tmpBB.tag=
0;_tmpBB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB9[1]={& _tmpBB};Cyc_Tcutil_terr(loc,({const char*_tmpBA="expecting integral or * type but found %s";
_tag_dynforward(_tmpBA,sizeof(char),_get_zero_arr_size(_tmpBA,42));}),
_tag_dynforward(_tmpB9,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7E: if((
int)_tmpB5 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct
_tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpBC[1]={& _tmpBE};Cyc_Tcutil_terr(loc,({const char*_tmpBD="expecting integral type but found %s";
_tag_dynforward(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,37));}),
_tag_dynforward(_tmpBC,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL80: if((int)_tmpB5 != 19)goto _LL82;_LL81:{void*_tmpBF=
t;struct Cyc_Absyn_PtrInfo _tmpC0;struct Cyc_Absyn_PtrAtts _tmpC1;struct Cyc_Absyn_Conref*
_tmpC2;_LL85: if(_tmpBF <= (void*)4)goto _LL89;if(*((int*)_tmpBF)!= 7)goto _LL87;
_LL86: goto _LL84;_LL87: if(*((int*)_tmpBF)!= 4)goto _LL89;_tmpC0=((struct Cyc_Absyn_PointerType_struct*)
_tmpBF)->f1;_tmpC1=_tmpC0.ptr_atts;_tmpC2=_tmpC1.bounds;_LL88:{union Cyc_Absyn_Constraint_union
_tmpC3=(Cyc_Absyn_compress_conref(_tmpC2))->v;void*_tmpC4;void*_tmpC5;void*
_tmpC6;_LL8C: if((_tmpC3.Eq_constr).tag != 0)goto _LL8E;_tmpC4=(_tmpC3.Eq_constr).f1;
if((int)_tmpC4 != 0)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if((_tmpC3.Eq_constr).tag != 0)
goto _LL90;_tmpC5=(_tmpC3.Eq_constr).f1;if((int)_tmpC5 != 1)goto _LL90;_LL8F: goto
_LL91;_LL90: if((_tmpC3.Eq_constr).tag != 0)goto _LL92;_tmpC6=(_tmpC3.Eq_constr).f1;
if(_tmpC6 <= (void*)2)goto _LL92;if(*((int*)_tmpC6)!= 0)goto _LL92;_LL91: goto _LL8B;
_LL92:;_LL93:({void*_tmpC7[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpC8="can't apply size to pointer/array of abstract length";
_tag_dynforward(_tmpC8,sizeof(char),_get_zero_arr_size(_tmpC8,53));}),
_tag_dynforward(_tmpC7,sizeof(void*),0));});_LL8B:;}goto _LL84;_LL89:;_LL8A:({
struct Cyc_String_pa_struct _tmpCB;_tmpCB.tag=0;_tmpCB.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC9[1]={& _tmpCB};Cyc_Tcutil_terr(
loc,({const char*_tmpCA="size requires pointer or array type, not %s";
_tag_dynforward(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,44));}),
_tag_dynforward(_tmpC9,sizeof(void*),1));}});_LL84:;}return Cyc_Absyn_uint_typ;
_LL82:;_LL83:({void*_tmpCC[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpCD="Non-unary primop";
_tag_dynforward(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,17));}),
_tag_dynforward(_tmpCC,sizeof(void*),0));});_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_String_pa_struct
_tmpD0;_tmpD0.tag=0;_tmpD0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpCE[1]={& _tmpD0};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpCF="type %s cannot be used here";
_tag_dynforward(_tmpCF,sizeof(char),_get_zero_arr_size(_tmpCF,28));}),
_tag_dynforward(_tmpCE,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD1;}));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpD4;_tmpD4.tag=0;
_tmpD4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpD2[1]={& _tmpD4};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpD3="type %s cannot be used here";
_tag_dynforward(_tmpD3,sizeof(char),_get_zero_arr_size(_tmpD3,28));}),
_tag_dynforward(_tmpD2,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD5;}));}{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static
void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
void*_tmpD6=t1;struct Cyc_Absyn_PtrInfo _tmpD7;void*_tmpD8;struct Cyc_Absyn_Tqual
_tmpD9;struct Cyc_Absyn_PtrAtts _tmpDA;void*_tmpDB;struct Cyc_Absyn_Conref*_tmpDC;
struct Cyc_Absyn_Conref*_tmpDD;struct Cyc_Absyn_Conref*_tmpDE;_LL95: if(_tmpD6 <= (
void*)4)goto _LL97;if(*((int*)_tmpD6)!= 4)goto _LL97;_tmpD7=((struct Cyc_Absyn_PointerType_struct*)
_tmpD6)->f1;_tmpD8=(void*)_tmpD7.elt_typ;_tmpD9=_tmpD7.elt_tq;_tmpDA=_tmpD7.ptr_atts;
_tmpDB=(void*)_tmpDA.rgn;_tmpDC=_tmpDA.nullable;_tmpDD=_tmpDA.bounds;_tmpDE=
_tmpDA.zero_term;_LL96: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD8),(void*)
1))({void*_tmpDF[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpE0="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,50));}),
_tag_dynforward(_tmpDF,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t1))({
void*_tmpE1[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpE2="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,48));}),
_tag_dynforward(_tmpE1,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({
struct Cyc_String_pa_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmpE3[1]={& _tmpE5};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpE4="expecting int but found %s";_tag_dynforward(_tmpE4,
sizeof(char),_get_zero_arr_size(_tmpE4,27));}),_tag_dynforward(_tmpE3,sizeof(
void*),1));}});_tmpDD=Cyc_Absyn_compress_conref(_tmpDD);{union Cyc_Absyn_Constraint_union
_tmpE6=_tmpDD->v;void*_tmpE7;void*_tmpE8;void*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;
void*_tmpEB;_LL9A: if((_tmpE6.Eq_constr).tag != 0)goto _LL9C;_tmpE7=(_tmpE6.Eq_constr).f1;
if((int)_tmpE7 != 0)goto _LL9C;_LL9B: return t1;_LL9C: if((_tmpE6.Eq_constr).tag != 0)
goto _LL9E;_tmpE8=(_tmpE6.Eq_constr).f1;if((int)_tmpE8 != 1)goto _LL9E;_LL9D: return
t1;_LL9E: if((_tmpE6.Eq_constr).tag != 0)goto _LLA0;_tmpE9=(_tmpE6.Eq_constr).f1;
if(_tmpE9 <= (void*)2)goto _LLA0;if(*((int*)_tmpE9)!= 0)goto _LLA0;_tmpEA=((struct
Cyc_Absyn_Upper_b_struct*)_tmpE9)->f1;_LL9F: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpDE))({void*_tmpEC[0]={};Cyc_Tcutil_warn(e1->loc,({
const char*_tmpED="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dynforward(_tmpED,sizeof(char),_get_zero_arr_size(_tmpED,70));}),
_tag_dynforward(_tmpEC,sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*
_tmpEE=({struct Cyc_Absyn_PointerType_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));
_tmpEF[0]=({struct Cyc_Absyn_PointerType_struct _tmpF0;_tmpF0.tag=4;_tmpF0.f1=({
struct Cyc_Absyn_PtrInfo _tmpF1;_tmpF1.elt_typ=(void*)_tmpD8;_tmpF1.elt_tq=_tmpD9;
_tmpF1.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpF2;_tmpF2.rgn=(void*)_tmpDB;_tmpF2.nullable=
Cyc_Absyn_true_conref;_tmpF2.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmpF2.zero_term=
_tmpDE;_tmpF2.ptrloc=0;_tmpF2;});_tmpF1;});_tmpF0;});_tmpEF;});Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpEE,(void*)3);return(void*)_tmpEE;}_LLA0: if((_tmpE6.Eq_constr).tag
!= 0)goto _LLA2;_tmpEB=(_tmpE6.Eq_constr).f1;if(_tmpEB <= (void*)2)goto _LLA2;if(*((
int*)_tmpEB)!= 1)goto _LLA2;_LLA1:({void*_tmpF3[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpF4="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_dynforward(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,64));}),
_tag_dynforward(_tmpF3,sizeof(void*),0));});return t1;_LLA2:;_LLA3: _tmpDD->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmpF5;(_tmpF5.Eq_constr).tag=
0;(_tmpF5.Eq_constr).f1=(void*)((void*)0);_tmpF5;});return t1;_LL99:;}_LL97:;
_LL98: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL94:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;int forward_only1=0;int forward_only2=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt,& forward_only1)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF9;
_tmpF9.tag=0;_tmpF9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpF8;_tmpF8.tag=0;_tmpF8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpF6[2]={& _tmpF8,&
_tmpF9};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF7="pointer arithmetic on values of different types (%s != %s)";
_tag_dynforward(_tmpF7,sizeof(char),_get_zero_arr_size(_tmpF7,59));}),
_tag_dynforward(_tmpF6,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(forward_only1)({void*_tmpFA[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmpFB="can't subtract from forward-only ? pointer";
_tag_dynforward(_tmpFB,sizeof(char),_get_zero_arr_size(_tmpFB,43));}),
_tag_dynforward(_tmpFA,sizeof(void*),0));});if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpFC[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpFD="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,50));}),
_tag_dynforward(_tmpFC,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t1))({
void*_tmpFE[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpFF="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,48));}),
_tag_dynforward(_tmpFE,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({
struct Cyc_String_pa_struct _tmp103;_tmp103.tag=0;_tmp103.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp102;_tmp102.tag=0;_tmp102.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp100[2]={& _tmp102,& _tmp103};Cyc_Tcutil_terr(
e2->loc,({const char*_tmp101="expecting either %s or int but found %s";
_tag_dynforward(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,40));}),
_tag_dynforward(_tmp100,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,(
void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,(
void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}static
void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2) && Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmp104=_cycalloc(
sizeof(*_tmp104));_tmp104->v=Cyc_Tcenv_lookup_type_vars(te);_tmp104;}))))return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmp106=({struct _tuple0 _tmp105;
_tmp105.f1=Cyc_Tcutil_compress(t1);_tmp105.f2=Cyc_Tcutil_compress(t2);_tmp105;});
void*_tmp107;struct Cyc_Absyn_PtrInfo _tmp108;void*_tmp109;void*_tmp10A;struct Cyc_Absyn_PtrInfo
_tmp10B;void*_tmp10C;_LLA5: _tmp107=_tmp106.f1;if(_tmp107 <= (void*)4)goto _LLA7;
if(*((int*)_tmp107)!= 4)goto _LLA7;_tmp108=((struct Cyc_Absyn_PointerType_struct*)
_tmp107)->f1;_tmp109=(void*)_tmp108.elt_typ;_tmp10A=_tmp106.f2;if(_tmp10A <= (
void*)4)goto _LLA7;if(*((int*)_tmp10A)!= 4)goto _LLA7;_tmp10B=((struct Cyc_Absyn_PointerType_struct*)
_tmp10A)->f1;_tmp10C=(void*)_tmp10B.elt_typ;_LLA6: if(Cyc_Tcutil_unify(_tmp109,
_tmp10C))return Cyc_Absyn_sint_typ;goto _LLA4;_LLA7:;_LLA8: goto _LLA4;_LLA4:;}({
struct Cyc_String_pa_struct _tmp110;_tmp110.tag=0;_tmp110.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp10F;_tmp10F.tag=0;_tmp10F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp10D[2]={& _tmp10F,& _tmp110};Cyc_Tcutil_terr(
loc,({const char*_tmp10E="comparison not allowed between %s and %s";
_tag_dynforward(_tmp10E,sizeof(char),_get_zero_arr_size(_tmp10E,41));}),
_tag_dynforward(_tmp10D,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp111=_cycalloc(sizeof(*_tmp111));
_tmp111->v=Cyc_Tcenv_lookup_type_vars(te);_tmp111;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp112=p;_LLAA: if((int)_tmp112 != 
0)goto _LLAC;_LLAB: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLAC: if((int)_tmp112 != 2)
goto _LLAE;_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLAE: if((int)_tmp112 != 1)goto
_LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp112 != 3)goto _LLB2;_LLB1: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LLB2: if((int)_tmp112 != 4)goto _LLB4;_LLB3: goto
_LLB5;_LLB4: if((int)_tmp112 != 13)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp112
!= 14)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp112 != 15)goto _LLBA;_LLB9: goto
_LLBB;_LLBA: if((int)_tmp112 != 16)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp112
!= 17)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp112 != 18)goto _LLC0;_LLBF:
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);_LLC0: if((int)
_tmp112 != 5)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if((int)_tmp112 != 6)goto _LLC4;_LLC3:
goto _LLC5;_LLC4: if((int)_tmp112 != 7)goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)
_tmp112 != 8)goto _LLC8;_LLC7: goto _LLC9;_LLC8: if((int)_tmp112 != 9)goto _LLCA;_LLC9:
goto _LLCB;_LLCA: if((int)_tmp112 != 10)goto _LLCC;_LLCB: return Cyc_Tcexp_tcAnyBinop(
te,loc,e1,e2);_LLCC:;_LLCD:({void*_tmp113[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp114="bad binary primop";
_tag_dynforward(_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,18));}),
_tag_dynforward(_tmp113,sizeof(void*),0));});_LLA9:;}static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCE: return({void*_tmp115[0]={};
Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp116="primitive operator has 0 arguments";
_tag_dynforward(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,35));}),
_tag_dynforward(_tmp115,sizeof(void*),0));});case 1: _LLCF: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLD0: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLD1: return({void*_tmp117[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp118="primitive operator has > 2 arguments";_tag_dynforward(_tmp118,sizeof(
char),_get_zero_arr_size(_tmp118,37));}),_tag_dynforward(_tmp117,sizeof(void*),0));});}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp119=(void*)e->r;void*
_tmp11A;struct Cyc_Absyn_Vardecl*_tmp11B;void*_tmp11C;struct Cyc_Absyn_Vardecl*
_tmp11D;void*_tmp11E;struct Cyc_Absyn_Vardecl*_tmp11F;void*_tmp120;struct Cyc_Absyn_Vardecl*
_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*
_tmp124;struct _dynforward_ptr*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct
_dynforward_ptr*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;
struct Cyc_Absyn_Exp*_tmp12A;_LLD4: if(*((int*)_tmp119)!= 1)goto _LLD6;_tmp11A=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp119)->f2;if(_tmp11A <= (void*)1)goto
_LLD6;if(*((int*)_tmp11A)!= 2)goto _LLD6;_tmp11B=((struct Cyc_Absyn_Param_b_struct*)
_tmp11A)->f1;_LLD5: _tmp11D=_tmp11B;goto _LLD7;_LLD6: if(*((int*)_tmp119)!= 1)goto
_LLD8;_tmp11C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp119)->f2;if(_tmp11C <= (
void*)1)goto _LLD8;if(*((int*)_tmp11C)!= 3)goto _LLD8;_tmp11D=((struct Cyc_Absyn_Local_b_struct*)
_tmp11C)->f1;_LLD7: _tmp11F=_tmp11D;goto _LLD9;_LLD8: if(*((int*)_tmp119)!= 1)goto
_LLDA;_tmp11E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp119)->f2;if(_tmp11E <= (
void*)1)goto _LLDA;if(*((int*)_tmp11E)!= 4)goto _LLDA;_tmp11F=((struct Cyc_Absyn_Pat_b_struct*)
_tmp11E)->f1;_LLD9: _tmp121=_tmp11F;goto _LLDB;_LLDA: if(*((int*)_tmp119)!= 1)goto
_LLDC;_tmp120=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp119)->f2;if(_tmp120 <= (
void*)1)goto _LLDC;if(*((int*)_tmp120)!= 0)goto _LLDC;_tmp121=((struct Cyc_Absyn_Global_b_struct*)
_tmp120)->f1;_LLDB: if(!(_tmp121->tq).real_const)return;goto _LLD3;_LLDC: if(*((int*)
_tmp119)!= 25)goto _LLDE;_tmp122=((struct Cyc_Absyn_Subscript_e_struct*)_tmp119)->f1;
_tmp123=((struct Cyc_Absyn_Subscript_e_struct*)_tmp119)->f2;_LLDD:{void*_tmp12B=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp122->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp12C;struct Cyc_Absyn_Tqual _tmp12D;struct Cyc_Absyn_ArrayInfo
_tmp12E;struct Cyc_Absyn_Tqual _tmp12F;struct Cyc_List_List*_tmp130;_LLEB: if(
_tmp12B <= (void*)4)goto _LLF1;if(*((int*)_tmp12B)!= 4)goto _LLED;_tmp12C=((struct
Cyc_Absyn_PointerType_struct*)_tmp12B)->f1;_tmp12D=_tmp12C.elt_tq;_LLEC: _tmp12F=
_tmp12D;goto _LLEE;_LLED: if(*((int*)_tmp12B)!= 7)goto _LLEF;_tmp12E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp12B)->f1;_tmp12F=_tmp12E.tq;_LLEE: if(!_tmp12F.real_const)return;goto _LLEA;
_LLEF: if(*((int*)_tmp12B)!= 9)goto _LLF1;_tmp130=((struct Cyc_Absyn_TupleType_struct*)
_tmp12B)->f1;_LLF0: {unsigned int _tmp132;int _tmp133;struct _tuple7 _tmp131=Cyc_Evexp_eval_const_uint_exp(
_tmp123);_tmp132=_tmp131.f1;_tmp133=_tmp131.f2;if(!_tmp133){({void*_tmp134[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp135="tuple projection cannot use sizeof or offsetof";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,47));}),
_tag_dynforward(_tmp134,sizeof(void*),0));});return;}{struct _handler_cons _tmp136;
_push_handler(& _tmp136);{int _tmp138=0;if(setjmp(_tmp136.handler))_tmp138=1;if(!
_tmp138){{struct _tuple9 _tmp13A;struct Cyc_Absyn_Tqual _tmp13B;struct _tuple9*
_tmp139=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp130,(
int)_tmp132);_tmp13A=*_tmp139;_tmp13B=_tmp13A.f1;if(!_tmp13B.real_const){
_npop_handler(0);return;}};_pop_handler();}else{void*_tmp137=(void*)_exn_thrown;
void*_tmp13D=_tmp137;_LLF4: if(_tmp13D != Cyc_List_Nth)goto _LLF6;_LLF5: return;
_LLF6:;_LLF7:(void)_throw(_tmp13D);_LLF3:;}}}goto _LLEA;}_LLF1:;_LLF2: goto _LLEA;
_LLEA:;}goto _LLD3;_LLDE: if(*((int*)_tmp119)!= 23)goto _LLE0;_tmp124=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp119)->f1;_tmp125=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp119)->f2;_LLDF:{
void*_tmp13E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp124->topt))->v);struct Cyc_Absyn_AggrInfo _tmp13F;union Cyc_Absyn_AggrInfoU_union
_tmp140;struct Cyc_Absyn_Aggrdecl**_tmp141;struct Cyc_List_List*_tmp142;_LLF9: if(
_tmp13E <= (void*)4)goto _LLFD;if(*((int*)_tmp13E)!= 10)goto _LLFB;_tmp13F=((struct
Cyc_Absyn_AggrType_struct*)_tmp13E)->f1;_tmp140=_tmp13F.aggr_info;if((((((struct
Cyc_Absyn_AggrType_struct*)_tmp13E)->f1).aggr_info).KnownAggr).tag != 1)goto _LLFB;
_tmp141=(_tmp140.KnownAggr).f1;_LLFA: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp141 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp141,_tmp125);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF8;}_LLFB: if(*((int*)_tmp13E)!= 11)goto _LLFD;_tmp142=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp13E)->f2;_LLFC: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp142,
_tmp125);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFD:;_LLFE: goto
_LLF8;_LLF8:;}goto _LLD3;_LLE0: if(*((int*)_tmp119)!= 24)goto _LLE2;_tmp126=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp119)->f1;_tmp127=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp119)->f2;_LLE1:{void*_tmp143=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp126->topt))->v);struct Cyc_Absyn_PtrInfo _tmp144;void*_tmp145;
_LL100: if(_tmp143 <= (void*)4)goto _LL102;if(*((int*)_tmp143)!= 4)goto _LL102;
_tmp144=((struct Cyc_Absyn_PointerType_struct*)_tmp143)->f1;_tmp145=(void*)
_tmp144.elt_typ;_LL101:{void*_tmp146=Cyc_Tcutil_compress(_tmp145);struct Cyc_Absyn_AggrInfo
_tmp147;union Cyc_Absyn_AggrInfoU_union _tmp148;struct Cyc_Absyn_Aggrdecl**_tmp149;
struct Cyc_List_List*_tmp14A;_LL105: if(_tmp146 <= (void*)4)goto _LL109;if(*((int*)
_tmp146)!= 10)goto _LL107;_tmp147=((struct Cyc_Absyn_AggrType_struct*)_tmp146)->f1;
_tmp148=_tmp147.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp146)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL107;_tmp149=(_tmp148.KnownAggr).f1;_LL106: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp149 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp149,_tmp127);if(sf == 0  || !(sf->tq).real_const)return;goto _LL104;}_LL107: if(*((
int*)_tmp146)!= 11)goto _LL109;_tmp14A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp146)->f2;_LL108: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp14A,_tmp127);if(sf == 0  || !(sf->tq).real_const)return;goto _LL104;}_LL109:;
_LL10A: goto _LL104;_LL104:;}goto _LLFF;_LL102:;_LL103: goto _LLFF;_LLFF:;}goto _LLD3;
_LLE2: if(*((int*)_tmp119)!= 22)goto _LLE4;_tmp128=((struct Cyc_Absyn_Deref_e_struct*)
_tmp119)->f1;_LLE3:{void*_tmp14B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp128->topt))->v);struct Cyc_Absyn_PtrInfo _tmp14C;struct Cyc_Absyn_Tqual
_tmp14D;struct Cyc_Absyn_ArrayInfo _tmp14E;struct Cyc_Absyn_Tqual _tmp14F;_LL10C: if(
_tmp14B <= (void*)4)goto _LL110;if(*((int*)_tmp14B)!= 4)goto _LL10E;_tmp14C=((
struct Cyc_Absyn_PointerType_struct*)_tmp14B)->f1;_tmp14D=_tmp14C.elt_tq;_LL10D:
_tmp14F=_tmp14D;goto _LL10F;_LL10E: if(*((int*)_tmp14B)!= 7)goto _LL110;_tmp14E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp14B)->f1;_tmp14F=_tmp14E.tq;_LL10F: if(!
_tmp14F.real_const)return;goto _LL10B;_LL110:;_LL111: goto _LL10B;_LL10B:;}goto
_LLD3;_LLE4: if(*((int*)_tmp119)!= 13)goto _LLE6;_tmp129=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp119)->f1;_LLE5: _tmp12A=_tmp129;goto _LLE7;_LLE6: if(*((int*)_tmp119)!= 14)goto
_LLE8;_tmp12A=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp119)->f1;_LLE7: Cyc_Tcexp_check_writable(
te,_tmp12A);return;_LLE8:;_LLE9: goto _LLD3;_LLD3:;}({struct Cyc_String_pa_struct
_tmp152;_tmp152.tag=0;_tmp152.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp150[1]={& _tmp152};Cyc_Tcutil_terr(e->loc,({
const char*_tmp151="attempt to write a const location: %s";_tag_dynforward(
_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,38));}),_tag_dynforward(_tmp150,
sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp153[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp154="increment/decrement of non-lvalue";
_tag_dynforward(_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,34));}),
_tag_dynforward(_tmp153,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt,& forward_only) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (
void*)0  || i == (void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(
void*)1))({void*_tmp155[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp156="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,50));}),
_tag_dynforward(_tmp155,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t))({
void*_tmp157[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp158="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,48));}),
_tag_dynforward(_tmp157,sizeof(void*),0));});if(forward_only  && (i == (void*)2
 || i == (void*)3))({void*_tmp159[0]={};Cyc_Tcutil_terr(e->loc,({const char*
_tmp15A="can't subtract from forward-only ? pointer";_tag_dynforward(_tmp15A,
sizeof(char),_get_zero_arr_size(_tmp15A,43));}),_tag_dynforward(_tmp159,sizeof(
void*),0));});}else{({struct Cyc_String_pa_struct _tmp15D;_tmp15D.tag=0;_tmp15D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp15B[1]={& _tmp15D};Cyc_Tcutil_terr(e->loc,({const char*_tmp15C="expecting arithmetic or ? type but found %s";
_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,44));}),
_tag_dynforward(_tmp15B,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp15E="conditional expression";_tag_dynforward(_tmp15E,sizeof(char),
_get_zero_arr_size(_tmp15E,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp167;}));struct Cyc_List_List _tmp15F=({struct Cyc_List_List _tmp166;_tmp166.hd=
e3;_tmp166.tl=0;_tmp166;});struct Cyc_List_List _tmp160=({struct Cyc_List_List
_tmp165;_tmp165.hd=e2;_tmp165.tl=(struct Cyc_List_List*)& _tmp15F;_tmp165;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp160)){({struct Cyc_String_pa_struct
_tmp164;_tmp164.tag=0;_tmp164.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{
struct Cyc_String_pa_struct _tmp163;_tmp163.tag=0;_tmp163.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmp161[2]={& _tmp163,& _tmp164};
Cyc_Tcutil_terr(loc,({const char*_tmp162="conditional clause types do not match: %s != %s";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,48));}),
_tag_dynforward(_tmp161,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp168="logical-and expression";_tag_dynforward(_tmp168,sizeof(char),
_get_zero_arr_size(_tmp168,23));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp169="logical-and expression";
_tag_dynforward(_tmp169,sizeof(char),_get_zero_arr_size(_tmp169,23));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp16A="logical-or expression";_tag_dynforward(_tmp16A,sizeof(char),
_get_zero_arr_size(_tmp16A,22));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp16B="logical-or expression";
_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,22));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){{struct _RegionHandle _tmp16C=_new_region("r");struct
_RegionHandle*r=& _tmp16C;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(
r,te),0,e1);;_pop_region(r);}Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp16D=Cyc_Tcutil_compress(t1);_LL113: if(_tmp16D <= (void*)4)goto _LL115;if(*((
int*)_tmp16D)!= 7)goto _LL115;_LL114:({void*_tmp16E[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp16F="cannot assign to an array";_tag_dynforward(_tmp16F,sizeof(
char),_get_zero_arr_size(_tmp16F,26));}),_tag_dynforward(_tmp16E,sizeof(void*),0));});
goto _LL112;_LL115:;_LL116: goto _LL112;_LL112:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp170[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp171="type is abstract (can't determine size).";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,41));}),
_tag_dynforward(_tmp170,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp172[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp173="assignment to non-lvalue";
_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,25));}),
_tag_dynforward(_tmp172,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(
po == 0){if(Cyc_Tcutil_is_unique_pointer(t2) && !Cyc_Tcutil_is_unique_path(e2))({
struct Cyc_Int_pa_struct _tmp176;_tmp176.tag=1;_tmp176.f1=(unsigned long)744;{void*
_tmp174[1]={& _tmp176};Cyc_Tcutil_terr(e2->loc,({const char*_tmp175="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,41));}),
_tag_dynforward(_tmp174,sizeof(void*),1));}});if(!Cyc_Tcutil_coerce_assign(te,e2,
t1)){void*_tmp177=({struct Cyc_String_pa_struct _tmp17B;_tmp17B.tag=0;_tmp17B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp178[2]={& _tmp17A,& _tmp17B};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp179="type mismatch: %s != %s";_tag_dynforward(_tmp179,sizeof(char),
_get_zero_arr_size(_tmp179,24));}),_tag_dynforward(_tmp178,sizeof(void*),2));}}});
Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();return _tmp177;}}else{void*
_tmp17C=(void*)po->v;void*_tmp17D=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp17C,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp17D,t1) || Cyc_Tcutil_coerceable(_tmp17D) && Cyc_Tcutil_coerceable(
t1))){void*_tmp17E=({struct Cyc_String_pa_struct _tmp182;_tmp182.tag=0;_tmp182.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp181;_tmp181.tag=0;_tmp181.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp17F[2]={& _tmp181,& _tmp182};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp180="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,82));}),
_tag_dynforward(_tmp17F,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp17D,t1);Cyc_Tcutil_explain_failure();
return _tmp17E;}return _tmp17D;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp183=Cyc_Tcutil_compress(
t1);void*_tmp184;void*_tmp185;_LL118: if((int)_tmp183 != 1)goto _LL11A;_LL119: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL117;_LL11A: if(_tmp183 <= (void*)4)goto
_LL11E;if(*((int*)_tmp183)!= 5)goto _LL11C;_tmp184=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp183)->f2;if((int)_tmp184 != 0)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(*((int*)
_tmp183)!= 5)goto _LL11E;_tmp185=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp183)->f2;
if((int)_tmp185 != 1)goto _LL11E;_LL11D: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL117;_LL11E:;_LL11F: goto _LL117;_LL117:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp187;struct Cyc_List_List*
_tmp188;struct Cyc_Position_Segment*_tmp189;void*_tmp18A;struct Cyc_Absyn_Tunionfield
_tmp186=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp187=_tmp186.name;_tmp188=
_tmp186.typs;_tmp189=_tmp186.loc;_tmp18A=(void*)_tmp186.sc;if(_tmp188 == 0  || 
_tmp188->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp188->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_Position_Segment*
_tmp18E;void*_tmp18F;struct Cyc_Absyn_Tunionfield _tmp18B=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp18C=_tmp18B.name;_tmp18D=_tmp18B.typs;_tmp18E=_tmp18B.loc;
_tmp18F=(void*)_tmp18B.sc;if(_tmp18D == 0  || _tmp18D->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp18D->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp193;_tmp193.tag=0;_tmp193.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{
struct Cyc_String_pa_struct _tmp192;_tmp192.tag=0;_tmp192.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(tu));{void*
_tmp190[2]={& _tmp192,& _tmp193};Cyc_Tcutil_terr(e->loc,({const char*_tmp191="can't find a field in %s to inject a value of type %s";
_tag_dynforward(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,54));}),
_tag_dynforward(_tmp190,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp195=args;struct _RegionHandle _tmp196=_new_region("ter");
struct _RegionHandle*ter=& _tmp196;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp197=
Cyc_Tcenv_new_block(ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp197,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp198=t;struct Cyc_Absyn_PtrInfo
_tmp199;void*_tmp19A;struct Cyc_Absyn_Tqual _tmp19B;struct Cyc_Absyn_PtrAtts _tmp19C;
void*_tmp19D;struct Cyc_Absyn_Conref*_tmp19E;struct Cyc_Absyn_Conref*_tmp19F;
struct Cyc_Absyn_Conref*_tmp1A0;_LL121: if(_tmp198 <= (void*)4)goto _LL123;if(*((int*)
_tmp198)!= 4)goto _LL123;_tmp199=((struct Cyc_Absyn_PointerType_struct*)_tmp198)->f1;
_tmp19A=(void*)_tmp199.elt_typ;_tmp19B=_tmp199.elt_tq;_tmp19C=_tmp199.ptr_atts;
_tmp19D=(void*)_tmp19C.rgn;_tmp19E=_tmp19C.nullable;_tmp19F=_tmp19C.bounds;
_tmp1A0=_tmp19C.zero_term;_LL122: Cyc_Tcenv_check_rgn_accessible(_tmp197,loc,
_tmp19D);Cyc_Tcutil_check_nonzero_bound(loc,_tmp19F);{void*_tmp1A1=Cyc_Tcutil_compress(
_tmp19A);struct Cyc_Absyn_FnInfo _tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_Core_Opt*
_tmp1A4;void*_tmp1A5;struct Cyc_List_List*_tmp1A6;int _tmp1A7;struct Cyc_Absyn_VarargInfo*
_tmp1A8;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AA;_LL126: if(
_tmp1A1 <= (void*)4)goto _LL128;if(*((int*)_tmp1A1)!= 8)goto _LL128;_tmp1A2=((
struct Cyc_Absyn_FnType_struct*)_tmp1A1)->f1;_tmp1A3=_tmp1A2.tvars;_tmp1A4=
_tmp1A2.effect;_tmp1A5=(void*)_tmp1A2.ret_typ;_tmp1A6=_tmp1A2.args;_tmp1A7=
_tmp1A2.c_varargs;_tmp1A8=_tmp1A2.cyc_varargs;_tmp1A9=_tmp1A2.rgn_po;_tmp1AA=
_tmp1A2.attributes;_LL127: if(topt != 0)Cyc_Tcutil_unify(_tmp1A5,*topt);while(
_tmp195 != 0  && _tmp1A6 != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp195->hd;
void*t2=(*((struct _tuple2*)_tmp1A6->hd)).f3;Cyc_Tcexp_tcExp(_tmp197,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp197,e1,t2)){({struct Cyc_String_pa_struct
_tmp1AE;_tmp1AE.tag=0;_tmp1AE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1AD;_tmp1AD.tag=0;
_tmp1AD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp1AB[2]={&
_tmp1AD,& _tmp1AE};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1AC="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,57));}),
_tag_dynforward(_tmp1AB,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_unique_pointer(
t2) && !Cyc_Tcutil_is_unique_path(e1))({struct Cyc_Int_pa_struct _tmp1B1;_tmp1B1.tag=
1;_tmp1B1.f1=(unsigned long)861;{void*_tmp1AF[1]={& _tmp1B1};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp1B0="Cannot consume paths; do swap instead %d";_tag_dynforward(
_tmp1B0,sizeof(char),_get_zero_arr_size(_tmp1B0,41));}),_tag_dynforward(_tmp1AF,
sizeof(void*),1));}});_tmp195=_tmp195->tl;_tmp1A6=_tmp1A6->tl;}{int
args_already_checked=0;{struct Cyc_List_List*a=_tmp1AA;for(0;a != 0;a=a->tl){void*
_tmp1B2=(void*)a->hd;void*_tmp1B3;int _tmp1B4;int _tmp1B5;_LL12B: if(_tmp1B2 <= (
void*)17)goto _LL12D;if(*((int*)_tmp1B2)!= 3)goto _LL12D;_tmp1B3=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1B2)->f1;_tmp1B4=((struct Cyc_Absyn_Format_att_struct*)
_tmp1B2)->f2;_tmp1B5=((struct Cyc_Absyn_Format_att_struct*)_tmp1B2)->f3;_LL12C:{
struct _handler_cons _tmp1B6;_push_handler(& _tmp1B6);{int _tmp1B8=0;if(setjmp(
_tmp1B6.handler))_tmp1B8=1;if(!_tmp1B8){{struct Cyc_Absyn_Exp*_tmp1B9=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1B4 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp1B5 == 0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp1BA=
_cycalloc(sizeof(*_tmp1BA));_tmp1BA->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp1B5 - 1);_tmp1BA;});}args_already_checked=1;{
void*_tmp1BB=_tmp1B3;_LL130: if((int)_tmp1BB != 0)goto _LL132;_LL131: Cyc_Tcexp_check_format_args(
_tmp197,_tmp1B9,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL12F;_LL132: if((
int)_tmp1BB != 1)goto _LL12F;_LL133: Cyc_Tcexp_check_format_args(_tmp197,_tmp1B9,
fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL12F;_LL12F:;}};_pop_handler();}
else{void*_tmp1B7=(void*)_exn_thrown;void*_tmp1BD=_tmp1B7;_LL135: if(_tmp1BD != 
Cyc_List_Nth)goto _LL137;_LL136:({void*_tmp1BE[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp1BF="bad format arguments";_tag_dynforward(_tmp1BF,sizeof(char),
_get_zero_arr_size(_tmp1BF,21));}),_tag_dynforward(_tmp1BE,sizeof(void*),0));});
goto _LL134;_LL137:;_LL138:(void)_throw(_tmp1BD);_LL134:;}}}goto _LL12A;_LL12D:;
_LL12E: goto _LL12A;_LL12A:;}}if(_tmp1A6 != 0)({void*_tmp1C0[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1C1="too few arguments for function";_tag_dynforward(_tmp1C1,
sizeof(char),_get_zero_arr_size(_tmp1C1,31));}),_tag_dynforward(_tmp1C0,sizeof(
void*),0));});else{if((_tmp195 != 0  || _tmp1A7) || _tmp1A8 != 0){if(_tmp1A7)for(0;
_tmp195 != 0;_tmp195=_tmp195->tl){Cyc_Tcexp_tcExp(_tmp197,0,(struct Cyc_Absyn_Exp*)
_tmp195->hd);}else{if(_tmp1A8 == 0)({void*_tmp1C2[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp1C3="too many arguments for function";_tag_dynforward(_tmp1C3,
sizeof(char),_get_zero_arr_size(_tmp1C3,32));}),_tag_dynforward(_tmp1C2,sizeof(
void*),0));});else{void*_tmp1C5;int _tmp1C6;struct Cyc_Absyn_VarargInfo _tmp1C4=*
_tmp1A8;_tmp1C5=(void*)_tmp1C4.type;_tmp1C6=_tmp1C4.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp1C7=({struct Cyc_Absyn_VarargCallInfo*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));
_tmp1E5->num_varargs=0;_tmp1E5->injectors=0;_tmp1E5->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp1A8;_tmp1E5;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1C7;
if(!_tmp1C6)for(0;_tmp195 != 0;_tmp195=_tmp195->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp195->hd;_tmp1C7->num_varargs ++;Cyc_Tcexp_tcExp(_tmp197,(
void**)& _tmp1C5,e1);if(!Cyc_Tcutil_coerce_arg(_tmp197,e1,_tmp1C5)){({struct Cyc_String_pa_struct
_tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp1C5));{void*
_tmp1C8[2]={& _tmp1CA,& _tmp1CB};Cyc_Tcutil_terr(loc,({const char*_tmp1C9="vararg requires type %s but argument has type %s";
_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,49));}),
_tag_dynforward(_tmp1C8,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}if(
Cyc_Tcutil_is_unique_pointer(_tmp1C5) && !Cyc_Tcutil_is_unique_path(e1))({struct
Cyc_Int_pa_struct _tmp1CE;_tmp1CE.tag=1;_tmp1CE.f1=(unsigned long)924;{void*
_tmp1CC[1]={& _tmp1CE};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1CD="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,41));}),
_tag_dynforward(_tmp1CC,sizeof(void*),1));}});}else{void*_tmp1CF=Cyc_Tcutil_compress(
_tmp1C5);struct Cyc_Absyn_TunionInfo _tmp1D0;union Cyc_Absyn_TunionInfoU_union
_tmp1D1;struct Cyc_Absyn_Tuniondecl**_tmp1D2;struct Cyc_Absyn_Tuniondecl*_tmp1D3;
struct Cyc_List_List*_tmp1D4;struct Cyc_Core_Opt*_tmp1D5;_LL13A: if(_tmp1CF <= (void*)
4)goto _LL13C;if(*((int*)_tmp1CF)!= 2)goto _LL13C;_tmp1D0=((struct Cyc_Absyn_TunionType_struct*)
_tmp1CF)->f1;_tmp1D1=_tmp1D0.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp1CF)->f1).tunion_info).KnownTunion).tag != 1)goto _LL13C;_tmp1D2=(_tmp1D1.KnownTunion).f1;
_tmp1D3=*_tmp1D2;_tmp1D4=_tmp1D0.targs;_tmp1D5=_tmp1D0.rgn;_LL13B: {struct Cyc_Absyn_Tuniondecl*
_tmp1D6=*Cyc_Tcenv_lookup_tuniondecl(_tmp197,loc,_tmp1D3->name);struct Cyc_List_List*
fields=0;if(_tmp1D6->fields == 0)({struct Cyc_String_pa_struct _tmp1D9;_tmp1D9.tag=
0;_tmp1D9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1C5));{void*_tmp1D7[1]={& _tmp1D9};Cyc_Tcutil_terr(loc,({const char*_tmp1D8="can't inject into %s";
_tag_dynforward(_tmp1D8,sizeof(char),_get_zero_arr_size(_tmp1D8,21));}),
_tag_dynforward(_tmp1D7,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D6->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D5))->v,Cyc_Tcenv_curr_rgn(_tmp197)))({void*
_tmp1DA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1DB="bad region for injected varargs";
_tag_dynforward(_tmp1DB,sizeof(char),_get_zero_arr_size(_tmp1DB,32));}),
_tag_dynforward(_tmp1DA,sizeof(void*),0));});{struct _RegionHandle _tmp1DC=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp1DC;_push_region(rgn);{struct Cyc_List_List*
_tmp1DD=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1D6->tvs,
_tmp1D4);for(0;_tmp195 != 0;_tmp195=_tmp195->tl){_tmp1C7->num_varargs ++;{struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp195->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(
_tmp197,0,e1);if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v) && !Cyc_Tcutil_is_unique_path(e1))({struct Cyc_Int_pa_struct
_tmp1E0;_tmp1E0.tag=1;_tmp1E0.f1=(unsigned long)950;{void*_tmp1DE[1]={& _tmp1E0};
Cyc_Tcutil_terr(e1->loc,({const char*_tmp1DF="Cannot consume paths; do swap instead %d";
_tag_dynforward(_tmp1DF,sizeof(char),_get_zero_arr_size(_tmp1DF,41));}),
_tag_dynforward(_tmp1DE,sizeof(void*),1));}});}{struct Cyc_Absyn_Tunionfield*
_tmp1E1=Cyc_Tcexp_tcInjection(_tmp197,e1,_tmp1C5,rgn,_tmp1DD,fields);if(_tmp1E1
!= 0)_tmp1C7->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp1C7->injectors,({struct Cyc_List_List*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2->hd=(struct Cyc_Absyn_Tunionfield*)
_tmp1E1;_tmp1E2->tl=0;_tmp1E2;}));}}}};_pop_region(rgn);}goto _LL139;}_LL13C:;
_LL13D:({void*_tmp1E3[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1E4="bad inject vararg type";
_tag_dynforward(_tmp1E4,sizeof(char),_get_zero_arr_size(_tmp1E4,23));}),
_tag_dynforward(_tmp1E3,sizeof(void*),0));});goto _LL139;_LL139:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp197,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1A4))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp197,loc,_tmp1A9);{void*_tmp1E6=_tmp1A5;_npop_handler(0);return _tmp1E6;}}
_LL128:;_LL129: {void*_tmp1E9=({void*_tmp1E7[0]={};Cyc_Tcexp_expr_err(_tmp197,
loc,topt,({const char*_tmp1E8="expected pointer to function";_tag_dynforward(
_tmp1E8,sizeof(char),_get_zero_arr_size(_tmp1E8,29));}),_tag_dynforward(_tmp1E7,
sizeof(void*),0));});_npop_handler(0);return _tmp1E9;}_LL125:;}_LL123:;_LL124: {
void*_tmp1EC=({void*_tmp1EA[0]={};Cyc_Tcexp_expr_err(_tmp197,loc,topt,({const
char*_tmp1EB="expected pointer to function";_tag_dynforward(_tmp1EB,sizeof(char),
_get_zero_arr_size(_tmp1EB,29));}),_tag_dynforward(_tmp1EA,sizeof(void*),0));});
_npop_handler(0);return _tmp1EC;}_LL120:;}};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,
e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct _tmp1EF;_tmp1EF.tag=0;_tmp1EF.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1ED[1]={& _tmp1EF};Cyc_Tcutil_terr(
loc,({const char*_tmp1EE="expected xtunion exn but found %s";_tag_dynforward(
_tmp1EE,sizeof(char),_get_zero_arr_size(_tmp1EE,34));}),_tag_dynforward(_tmp1ED,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1F0=
_cycalloc(sizeof(*_tmp1F0));_tmp1F0->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1F0;}));}
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F1=t1;struct Cyc_Absyn_PtrInfo
_tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Tqual _tmp1F4;struct Cyc_Absyn_PtrAtts _tmp1F5;
void*_tmp1F6;struct Cyc_Absyn_Conref*_tmp1F7;struct Cyc_Absyn_Conref*_tmp1F8;
struct Cyc_Absyn_Conref*_tmp1F9;_LL13F: if(_tmp1F1 <= (void*)4)goto _LL141;if(*((int*)
_tmp1F1)!= 4)goto _LL141;_tmp1F2=((struct Cyc_Absyn_PointerType_struct*)_tmp1F1)->f1;
_tmp1F3=(void*)_tmp1F2.elt_typ;_tmp1F4=_tmp1F2.elt_tq;_tmp1F5=_tmp1F2.ptr_atts;
_tmp1F6=(void*)_tmp1F5.rgn;_tmp1F7=_tmp1F5.nullable;_tmp1F8=_tmp1F5.bounds;
_tmp1F9=_tmp1F5.zero_term;_LL140:{void*_tmp1FA=Cyc_Tcutil_compress(_tmp1F3);
struct Cyc_Absyn_FnInfo _tmp1FB;struct Cyc_List_List*_tmp1FC;struct Cyc_Core_Opt*
_tmp1FD;void*_tmp1FE;struct Cyc_List_List*_tmp1FF;int _tmp200;struct Cyc_Absyn_VarargInfo*
_tmp201;struct Cyc_List_List*_tmp202;struct Cyc_List_List*_tmp203;_LL144: if(
_tmp1FA <= (void*)4)goto _LL146;if(*((int*)_tmp1FA)!= 8)goto _LL146;_tmp1FB=((
struct Cyc_Absyn_FnType_struct*)_tmp1FA)->f1;_tmp1FC=_tmp1FB.tvars;_tmp1FD=
_tmp1FB.effect;_tmp1FE=(void*)_tmp1FB.ret_typ;_tmp1FF=_tmp1FB.args;_tmp200=
_tmp1FB.c_varargs;_tmp201=_tmp1FB.cyc_varargs;_tmp202=_tmp1FB.rgn_po;_tmp203=
_tmp1FB.attributes;_LL145: {struct Cyc_List_List*instantiation=0;for(0;ts != 0  && 
_tmp1FC != 0;(ts=ts->tl,_tmp1FC=_tmp1FC->tl)){void*k=Cyc_Tcutil_tvar_kind((struct
Cyc_Absyn_Tvar*)_tmp1FC->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp204=_cycalloc(
sizeof(*_tmp204));_tmp204->hd=({struct _tuple5*_tmp205=_cycalloc(sizeof(*_tmp205));
_tmp205->f1=(struct Cyc_Absyn_Tvar*)_tmp1FC->hd;_tmp205->f2=(void*)ts->hd;_tmp205;});
_tmp204->tl=instantiation;_tmp204;});}if(ts != 0)return({void*_tmp206[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp207="too many type variables in instantiation";
_tag_dynforward(_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,41));}),
_tag_dynforward(_tmp206,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp20C=_cycalloc(sizeof(*
_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_FnType_struct _tmp20D;_tmp20D.tag=8;
_tmp20D.f1=({struct Cyc_Absyn_FnInfo _tmp20E;_tmp20E.tvars=_tmp1FC;_tmp20E.effect=
_tmp1FD;_tmp20E.ret_typ=(void*)_tmp1FE;_tmp20E.args=_tmp1FF;_tmp20E.c_varargs=
_tmp200;_tmp20E.cyc_varargs=_tmp201;_tmp20E.rgn_po=_tmp202;_tmp20E.attributes=
_tmp203;_tmp20E;});_tmp20D;});_tmp20C;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208[0]=({struct Cyc_Absyn_PointerType_struct
_tmp209;_tmp209.tag=4;_tmp209.f1=({struct Cyc_Absyn_PtrInfo _tmp20A;_tmp20A.elt_typ=(
void*)new_fn_typ;_tmp20A.elt_tq=_tmp1F4;_tmp20A.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp20B;_tmp20B.rgn=(void*)_tmp1F6;_tmp20B.nullable=_tmp1F7;_tmp20B.bounds=
_tmp1F8;_tmp20B.zero_term=_tmp1F9;_tmp20B.ptrloc=0;_tmp20B;});_tmp20A;});_tmp209;});
_tmp208;});}}_LL146:;_LL147: goto _LL143;_LL143:;}goto _LL13E;_LL141:;_LL142: goto
_LL13E;_LL13E:;}return({struct Cyc_String_pa_struct _tmp211;_tmp211.tag=0;_tmp211.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp20F[1]={& _tmp211};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp210="expecting polymorphic type but found %s";
_tag_dynforward(_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,40));}),
_tag_dynforward(_tmp20F,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp212=({
struct Cyc_String_pa_struct _tmp216;_tmp216.tag=0;_tmp216.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp215;_tmp215.tag=0;_tmp215.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp213[2]={& _tmp215,& _tmp216};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp214="cannot cast %s to %s";_tag_dynforward(
_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,21));}),_tag_dynforward(_tmp213,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp212;}}}}{struct
_tuple0 _tmp218=({struct _tuple0 _tmp217;_tmp217.f1=(void*)e->r;_tmp217.f2=Cyc_Tcutil_compress(
t);_tmp217;});void*_tmp219;struct Cyc_Absyn_MallocInfo _tmp21A;int _tmp21B;void*
_tmp21C;struct Cyc_Absyn_PtrInfo _tmp21D;struct Cyc_Absyn_PtrAtts _tmp21E;struct Cyc_Absyn_Conref*
_tmp21F;struct Cyc_Absyn_Conref*_tmp220;struct Cyc_Absyn_Conref*_tmp221;_LL149:
_tmp219=_tmp218.f1;if(*((int*)_tmp219)!= 35)goto _LL14B;_tmp21A=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp219)->f1;_tmp21B=_tmp21A.fat_result;_tmp21C=_tmp218.f2;if(_tmp21C <= (void*)4)
goto _LL14B;if(*((int*)_tmp21C)!= 4)goto _LL14B;_tmp21D=((struct Cyc_Absyn_PointerType_struct*)
_tmp21C)->f1;_tmp21E=_tmp21D.ptr_atts;_tmp21F=_tmp21E.nullable;_tmp220=_tmp21E.bounds;
_tmp221=_tmp21E.zero_term;_LL14A: if((_tmp21B  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp221)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp21F)){void*_tmp222=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp220);struct Cyc_Absyn_Exp*_tmp223;_LL14E: if(_tmp222 <= (void*)2)goto _LL150;if(*((
int*)_tmp222)!= 0)goto _LL150;_tmp223=((struct Cyc_Absyn_Upper_b_struct*)_tmp222)->f1;
_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp223)).f1 == 1)({void*_tmp224[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp225="cast from ? pointer to * pointer will lose size information";
_tag_dynforward(_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,60));}),
_tag_dynforward(_tmp224,sizeof(void*),0));});goto _LL14D;_LL150:;_LL151: goto
_LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto _LL148;_LL148:;}return t;}}static
void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp226=0;struct
Cyc_Absyn_Tqual _tmp227=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp228=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp229;void*_tmp22A;struct Cyc_Absyn_Tqual _tmp22B;
struct Cyc_Absyn_PtrAtts _tmp22C;struct Cyc_Absyn_Conref*_tmp22D;_LL153: if(_tmp228
<= (void*)4)goto _LL155;if(*((int*)_tmp228)!= 4)goto _LL155;_tmp229=((struct Cyc_Absyn_PointerType_struct*)
_tmp228)->f1;_tmp22A=(void*)_tmp229.elt_typ;_tmp22B=_tmp229.elt_tq;_tmp22C=
_tmp229.ptr_atts;_tmp22D=_tmp22C.zero_term;_LL154: _tmp226=({void**_tmp22E=
_cycalloc(sizeof(*_tmp22E));_tmp22E[0]=_tmp22A;_tmp22E;});_tmp227=_tmp22B;goto
_LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{struct _RegionHandle _tmp22F=
_new_region("r");struct _RegionHandle*r=& _tmp22F;_push_region(r);Cyc_Tcexp_tcExpNoInst(
Cyc_Tcenv_clear_notreadctxt(r,te),_tmp226,e);;_pop_region(r);}{void*_tmp230=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp231;struct Cyc_Absyn_Exp*_tmp232;_LL158: if(*((
int*)_tmp230)!= 25)goto _LL15A;_tmp231=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp230)->f1;_tmp232=((struct Cyc_Absyn_Subscript_e_struct*)_tmp230)->f2;_LL159:{
void*_tmp233=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp231->topt))->v);_LL15D: if(_tmp233 <= (void*)4)goto _LL15F;if(*((int*)_tmp233)
!= 9)goto _LL15F;_LL15E: goto _LL15C;_LL15F:;_LL160:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp231,_tmp232,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp231,
_tmp232);_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{int _tmp235;void*
_tmp236;struct _tuple6 _tmp234=Cyc_Tcutil_addressof_props(te,e);_tmp235=_tmp234.f1;
_tmp236=_tmp234.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp235){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp236,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({struct Cyc_String_pa_struct _tmp239;
_tmp239.tag=0;_tmp239.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp237[1]={& _tmp239};Cyc_Tcutil_terr(loc,({const char*_tmp238="sizeof applied to type %s, which has unknown size here";
_tag_dynforward(_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,55));}),
_tag_dynforward(_tmp237,sizeof(void*),1));}});if(topt != 0){void*_tmp23A=Cyc_Tcutil_compress(*
topt);_LL162: if(_tmp23A <= (void*)4)goto _LL164;if(*((int*)_tmp23A)!= 14)goto
_LL164;_LL163: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp23B=_cycalloc(
sizeof(*_tmp23B));_tmp23B[0]=({struct Cyc_Absyn_SizeofType_struct _tmp23C;_tmp23C.tag=
14;_tmp23C.f1=(void*)t;_tmp23C;});_tmp23B;});_LL164:;_LL165: goto _LL161;_LL161:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct
_dynforward_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dynforward_ptr)*n,(struct _dynforward_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp23D=n;struct _dynforward_ptr*_tmp23E;unsigned int _tmp23F;_LL167: if(*((int*)
_tmp23D)!= 0)goto _LL169;_tmp23E=((struct Cyc_Absyn_StructField_struct*)_tmp23D)->f1;
_LL168: {int bad_type=1;{void*_tmp240=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp241;union Cyc_Absyn_AggrInfoU_union _tmp242;struct Cyc_Absyn_Aggrdecl**_tmp243;
struct Cyc_List_List*_tmp244;_LL16C: if(_tmp240 <= (void*)4)goto _LL170;if(*((int*)
_tmp240)!= 10)goto _LL16E;_tmp241=((struct Cyc_Absyn_AggrType_struct*)_tmp240)->f1;
_tmp242=_tmp241.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp240)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL16E;_tmp243=(_tmp242.KnownAggr).f1;_LL16D: if((*_tmp243)->impl == 0)
goto _LL16B;if(!((int(*)(int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp23E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp243)->impl))->fields))({
struct Cyc_String_pa_struct _tmp247;_tmp247.tag=0;_tmp247.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp23E);{void*_tmp245[1]={& _tmp247};
Cyc_Tcutil_terr(loc,({const char*_tmp246="no field of struct/union has name %s";
_tag_dynforward(_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,37));}),
_tag_dynforward(_tmp245,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL16E: if(*((
int*)_tmp240)!= 11)goto _LL170;_tmp244=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp240)->f2;_LL16F: if(!((int(*)(int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp23E,_tmp244))({struct Cyc_String_pa_struct _tmp24A;_tmp24A.tag=0;_tmp24A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp23E);{void*_tmp248[1]={&
_tmp24A};Cyc_Tcutil_terr(loc,({const char*_tmp249="no field of struct/union has name %s";
_tag_dynforward(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,37));}),
_tag_dynforward(_tmp248,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL170:;
_LL171: goto _LL16B;_LL16B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp24D;
_tmp24D.tag=0;_tmp24D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp24B[1]={& _tmp24D};Cyc_Tcutil_terr(loc,({const char*_tmp24C="%s is not a known struct/union type";
_tag_dynforward(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,36));}),
_tag_dynforward(_tmp24B,sizeof(void*),1));}});goto _LL166;}_LL169: if(*((int*)
_tmp23D)!= 1)goto _LL166;_tmp23F=((struct Cyc_Absyn_TupleIndex_struct*)_tmp23D)->f1;
_LL16A: {int bad_type=1;{void*_tmp24E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp24F;union Cyc_Absyn_AggrInfoU_union _tmp250;struct Cyc_Absyn_Aggrdecl**_tmp251;
struct Cyc_List_List*_tmp252;struct Cyc_List_List*_tmp253;struct Cyc_Absyn_TunionFieldInfo
_tmp254;union Cyc_Absyn_TunionFieldInfoU_union _tmp255;struct Cyc_Absyn_Tunionfield*
_tmp256;_LL173: if(_tmp24E <= (void*)4)goto _LL17B;if(*((int*)_tmp24E)!= 10)goto
_LL175;_tmp24F=((struct Cyc_Absyn_AggrType_struct*)_tmp24E)->f1;_tmp250=_tmp24F.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp24E)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL175;_tmp251=(_tmp250.KnownAggr).f1;_LL174: if((*_tmp251)->impl == 0)
goto _LL172;_tmp252=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp251)->impl))->fields;
goto _LL176;_LL175: if(*((int*)_tmp24E)!= 11)goto _LL177;_tmp252=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp24E)->f2;_LL176: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp252)
<= _tmp23F)({struct Cyc_Int_pa_struct _tmp25A;_tmp25A.tag=1;_tmp25A.f1=(
unsigned long)((int)_tmp23F);{struct Cyc_Int_pa_struct _tmp259;_tmp259.tag=1;
_tmp259.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp252);{void*_tmp257[2]={& _tmp259,& _tmp25A};Cyc_Tcutil_terr(loc,({const char*
_tmp258="struct/union has too few components: %d <= %d";_tag_dynforward(_tmp258,
sizeof(char),_get_zero_arr_size(_tmp258,46));}),_tag_dynforward(_tmp257,sizeof(
void*),2));}}});bad_type=0;goto _LL172;_LL177: if(*((int*)_tmp24E)!= 9)goto _LL179;
_tmp253=((struct Cyc_Absyn_TupleType_struct*)_tmp24E)->f1;_LL178: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp253)<= _tmp23F)({struct Cyc_Int_pa_struct
_tmp25E;_tmp25E.tag=1;_tmp25E.f1=(unsigned long)((int)_tmp23F);{struct Cyc_Int_pa_struct
_tmp25D;_tmp25D.tag=1;_tmp25D.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp253);{void*_tmp25B[2]={& _tmp25D,& _tmp25E};Cyc_Tcutil_terr(
loc,({const char*_tmp25C="tuple has too few components: %d <= %d";_tag_dynforward(
_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,39));}),_tag_dynforward(_tmp25B,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL179: if(*((int*)_tmp24E)!= 3)goto
_LL17B;_tmp254=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp24E)->f1;_tmp255=
_tmp254.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp24E)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL17B;_tmp256=(_tmp255.KnownTunionfield).f2;_LL17A: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp256->typs)< _tmp23F)({struct Cyc_Int_pa_struct
_tmp262;_tmp262.tag=1;_tmp262.f1=(unsigned long)((int)_tmp23F);{struct Cyc_Int_pa_struct
_tmp261;_tmp261.tag=1;_tmp261.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp256->typs);{void*_tmp25F[2]={& _tmp261,& _tmp262};Cyc_Tcutil_terr(
loc,({const char*_tmp260="tunionfield has too few components: %d < %d";
_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,44));}),
_tag_dynforward(_tmp25F,sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL17B:;
_LL17C: goto _LL172;_LL172:;}if(bad_type)({struct Cyc_String_pa_struct _tmp265;
_tmp265.tag=0;_tmp265.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp263[1]={& _tmp265};Cyc_Tcutil_terr(loc,({const char*_tmp264="%s is not a known type";
_tag_dynforward(_tmp264,sizeof(char),_get_zero_arr_size(_tmp264,23));}),
_tag_dynforward(_tmp263,sizeof(void*),1));}});goto _LL166;}_LL166:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle _tmp266=_new_region("r");
struct _RegionHandle*r=& _tmp266;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp267=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp267,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp268=t;struct Cyc_Absyn_PtrInfo
_tmp269;void*_tmp26A;struct Cyc_Absyn_PtrAtts _tmp26B;void*_tmp26C;struct Cyc_Absyn_Conref*
_tmp26D;struct Cyc_Absyn_Conref*_tmp26E;_LL17E: if(_tmp268 <= (void*)4)goto _LL180;
if(*((int*)_tmp268)!= 4)goto _LL180;_tmp269=((struct Cyc_Absyn_PointerType_struct*)
_tmp268)->f1;_tmp26A=(void*)_tmp269.elt_typ;_tmp26B=_tmp269.ptr_atts;_tmp26C=(
void*)_tmp26B.rgn;_tmp26D=_tmp26B.bounds;_tmp26E=_tmp26B.zero_term;_LL17F: Cyc_Tcenv_check_rgn_accessible(
_tmp267,loc,_tmp26C);Cyc_Tcutil_check_nonzero_bound(loc,_tmp26D);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp26A),(void*)1))({void*_tmp26F[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp270="can't dereference abstract pointer type";_tag_dynforward(
_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,40));}),_tag_dynforward(_tmp26F,
sizeof(void*),0));});{void*_tmp271=_tmp26A;_npop_handler(0);return _tmp271;}
_LL180:;_LL181: {void*_tmp275=({struct Cyc_String_pa_struct _tmp274;_tmp274.tag=0;
_tmp274.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp272[1]={& _tmp274};Cyc_Tcexp_expr_err(_tmp267,loc,topt,({const char*
_tmp273="expecting * or @ type but found %s";_tag_dynforward(_tmp273,sizeof(char),
_get_zero_arr_size(_tmp273,35));}),_tag_dynforward(_tmp272,sizeof(void*),1));}});
_npop_handler(0);return _tmp275;}_LL17D:;}};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*f){Cyc_Tcexp_tcExpNoPromote(
te,0,e);{void*_tmp276=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp277;union Cyc_Absyn_AggrInfoU_union
_tmp278;struct Cyc_Absyn_Aggrdecl**_tmp279;struct Cyc_Absyn_Aggrdecl*_tmp27A;
struct Cyc_List_List*_tmp27B;void*_tmp27C;struct Cyc_List_List*_tmp27D;_LL183: if(
_tmp276 <= (void*)4)goto _LL18B;if(*((int*)_tmp276)!= 10)goto _LL185;_tmp277=((
struct Cyc_Absyn_AggrType_struct*)_tmp276)->f1;_tmp278=_tmp277.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp276)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL185;_tmp279=(_tmp278.KnownAggr).f1;_tmp27A=*_tmp279;_tmp27B=_tmp277.targs;
_LL184: {struct Cyc_Absyn_Aggrfield*_tmp280=Cyc_Absyn_lookup_decl_field(_tmp27A,f);
if(_tmp280 == 0)return({struct Cyc_String_pa_struct _tmp284;_tmp284.tag=0;_tmp284.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp283;_tmp283.tag=0;_tmp283.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp27A->name));{void*_tmp281[2]={& _tmp283,& _tmp284};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp282="type %s has no %s field";_tag_dynforward(
_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,24));}),_tag_dynforward(_tmp281,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp27A->impl))->exist_vars
!= 0)return({void*_tmp285[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp286="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,64));}),
_tag_dynforward(_tmp285,sizeof(void*),0));});{void*t2;{struct _RegionHandle
_tmp287=_new_region("rgn");struct _RegionHandle*rgn=& _tmp287;_push_region(rgn);{
struct Cyc_List_List*_tmp288=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp27A->tvs,_tmp27B);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp288,(void*)
_tmp280->type);if(((void*)_tmp27A->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){void*_tmp28C=({struct Cyc_String_pa_struct
_tmp28B;_tmp28B.tag=0;_tmp28B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp289[1]={& _tmp28B};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp28A="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,56));}),
_tag_dynforward(_tmp289,sizeof(void*),1));}});_npop_handler(0);return _tmp28C;}};
_pop_region(rgn);}return t2;}}_LL185: if(*((int*)_tmp276)!= 11)goto _LL187;_tmp27C=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp276)->f1;_tmp27D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp276)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp28D=Cyc_Absyn_lookup_field(
_tmp27D,f);if(_tmp28D == 0)return({struct Cyc_String_pa_struct _tmp290;_tmp290.tag=
0;_tmp290.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp28E[1]={&
_tmp290};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp28F="type has no %s field";
_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,21));}),
_tag_dynforward(_tmp28E,sizeof(void*),1));}});if((_tmp27C == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only((void*)_tmp28D->type))return({struct Cyc_String_pa_struct
_tmp293;_tmp293.tag=0;_tmp293.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp291[1]={& _tmp293};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp292="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,56));}),
_tag_dynforward(_tmp291,sizeof(void*),1));}});return(void*)_tmp28D->type;}_LL187:
if(*((int*)_tmp276)!= 7)goto _LL189;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({
const char*_tmp27E="size";_tag_dynforward(_tmp27E,sizeof(char),_get_zero_arr_size(
_tmp27E,5));}))== 0))goto _LL189;_LL188: goto _LL18A;_LL189: if(*((int*)_tmp276)!= 4)
goto _LL18B;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({const char*_tmp27F="size";
_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,5));}))== 0))
goto _LL18B;_LL18A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp295;_tmp295.tag=3;_tmp295.f1=(void*)((void*)19);_tmp295.f2=({struct Cyc_List_List*
_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->hd=e;_tmp296->tl=0;_tmp296;});
_tmp295;});_tmp294;})));return Cyc_Absyn_uint_typ;_LL18B:;_LL18C: if(Cyc_strcmp((
struct _dynforward_ptr)*f,({const char*_tmp297="size";_tag_dynforward(_tmp297,
sizeof(char),_get_zero_arr_size(_tmp297,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp29A;_tmp29A.tag=0;_tmp29A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp298[1]={& _tmp29A};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp299="expecting struct, union, or array, found %s";
_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size(_tmp299,44));}),
_tag_dynforward(_tmp298,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct
_tmp29D;_tmp29D.tag=0;_tmp29D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp29B[1]={& _tmp29D};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp29C="expecting struct or union, found %s";
_tag_dynforward(_tmp29C,sizeof(char),_get_zero_arr_size(_tmp29C,36));}),
_tag_dynforward(_tmp29B,sizeof(void*),1));}});}_LL182:;}}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*f){{struct _RegionHandle _tmp29E=_new_region("r");struct
_RegionHandle*r=& _tmp29E;_push_region(r);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
r,te),0,e);;_pop_region(r);}{void*_tmp29F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp2A0;void*_tmp2A1;struct Cyc_Absyn_PtrAtts
_tmp2A2;void*_tmp2A3;struct Cyc_Absyn_Conref*_tmp2A4;struct Cyc_Absyn_Conref*
_tmp2A5;_LL18E: if(_tmp29F <= (void*)4)goto _LL190;if(*((int*)_tmp29F)!= 4)goto
_LL190;_tmp2A0=((struct Cyc_Absyn_PointerType_struct*)_tmp29F)->f1;_tmp2A1=(void*)
_tmp2A0.elt_typ;_tmp2A2=_tmp2A0.ptr_atts;_tmp2A3=(void*)_tmp2A2.rgn;_tmp2A4=
_tmp2A2.bounds;_tmp2A5=_tmp2A2.zero_term;_LL18F: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2A4);{void*_tmp2A6=Cyc_Tcutil_compress(_tmp2A1);struct Cyc_Absyn_AggrInfo
_tmp2A7;union Cyc_Absyn_AggrInfoU_union _tmp2A8;struct Cyc_Absyn_Aggrdecl**_tmp2A9;
struct Cyc_Absyn_Aggrdecl*_tmp2AA;struct Cyc_List_List*_tmp2AB;void*_tmp2AC;struct
Cyc_List_List*_tmp2AD;_LL193: if(_tmp2A6 <= (void*)4)goto _LL197;if(*((int*)_tmp2A6)
!= 10)goto _LL195;_tmp2A7=((struct Cyc_Absyn_AggrType_struct*)_tmp2A6)->f1;_tmp2A8=
_tmp2A7.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp2A6)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL195;_tmp2A9=(_tmp2A8.KnownAggr).f1;_tmp2AA=*_tmp2A9;_tmp2AB=_tmp2A7.targs;
_LL194: {struct Cyc_Absyn_Aggrfield*_tmp2AE=Cyc_Absyn_lookup_decl_field(_tmp2AA,f);
if(_tmp2AE == 0)return({struct Cyc_String_pa_struct _tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp2AA->name));{void*_tmp2AF[2]={& _tmp2B1,& _tmp2B2};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2B0="type %s has no %s field";_tag_dynforward(
_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,24));}),_tag_dynforward(_tmp2AF,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AA->impl))->exist_vars
!= 0)return({void*_tmp2B3[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp2B4="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,64));}),
_tag_dynforward(_tmp2B3,sizeof(void*),0));});{void*t3;{struct _RegionHandle
_tmp2B5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2B5;_push_region(rgn);{
struct Cyc_List_List*_tmp2B6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp2AA->tvs,_tmp2AB);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp2B6,(void*)
_tmp2AE->type);};_pop_region(rgn);}if(((void*)_tmp2AA->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t3))return({struct Cyc_String_pa_struct _tmp2B9;
_tmp2B9.tag=0;_tmp2B9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmp2B7[1]={& _tmp2B9};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2B8="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2B8,sizeof(char),_get_zero_arr_size(_tmp2B8,56));}),
_tag_dynforward(_tmp2B7,sizeof(void*),1));}});return t3;}}_LL195: if(*((int*)
_tmp2A6)!= 11)goto _LL197;_tmp2AC=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2A6)->f1;_tmp2AD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2A6)->f2;_LL196: {
struct Cyc_Absyn_Aggrfield*_tmp2BA=Cyc_Absyn_lookup_field(_tmp2AD,f);if(_tmp2BA == 
0)return({struct Cyc_String_pa_struct _tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2BB[1]={& _tmp2BD};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2BC="type has no %s field";_tag_dynforward(_tmp2BC,
sizeof(char),_get_zero_arr_size(_tmp2BC,21));}),_tag_dynforward(_tmp2BB,sizeof(
void*),1));}});if((_tmp2AC == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2BA->type))return({struct Cyc_String_pa_struct _tmp2C0;_tmp2C0.tag=0;
_tmp2C0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2BE[1]={&
_tmp2C0};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2BF="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,56));}),
_tag_dynforward(_tmp2BE,sizeof(void*),1));}});return(void*)_tmp2BA->type;}_LL197:;
_LL198: goto _LL192;_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}return({
struct Cyc_String_pa_struct _tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2C1[1]={& _tmp2C3};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2C2="expecting struct or union pointer, found %s";
_tag_dynforward(_tmp2C2,sizeof(char),_get_zero_arr_size(_tmp2C2,44));}),
_tag_dynforward(_tmp2C1,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2C5;int _tmp2C6;struct _tuple7 _tmp2C4=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;if(!
_tmp2C6)return({void*_tmp2C7[0]={};Cyc_Tcexp_expr_err(te,loc,0,({const char*
_tmp2C8="tuple projection cannot use sizeof or offsetof";_tag_dynforward(_tmp2C8,
sizeof(char),_get_zero_arr_size(_tmp2C8,47));}),_tag_dynforward(_tmp2C7,sizeof(
void*),0));});{struct _handler_cons _tmp2C9;_push_handler(& _tmp2C9);{int _tmp2CB=0;
if(setjmp(_tmp2C9.handler))_tmp2CB=1;if(!_tmp2CB){{void*_tmp2CC=(*((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2C5)).f2;
_npop_handler(0);return _tmp2CC;};_pop_handler();}else{void*_tmp2CA=(void*)
_exn_thrown;void*_tmp2CE=_tmp2CA;_LL19A: if(_tmp2CE != Cyc_List_Nth)goto _LL19C;
_LL19B: return({struct Cyc_Int_pa_struct _tmp2D2;_tmp2D2.tag=1;_tmp2D2.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp2D1;_tmp2D1.tag=1;_tmp2D1.f1=(unsigned long)((int)_tmp2C5);{void*_tmp2CF[2]={&
_tmp2D1,& _tmp2D2};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2D0="index is %d but tuple has only %d fields";
_tag_dynforward(_tmp2D0,sizeof(char),_get_zero_arr_size(_tmp2D0,41));}),
_tag_dynforward(_tmp2CF,sizeof(void*),2));}}});_LL19C:;_LL19D:(void)_throw(
_tmp2CE);_LL199:;}}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp2D3=_new_region("r");struct
_RegionHandle*r=& _tmp2D3;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp2D4=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp2D4,0,e1);Cyc_Tcexp_tcExp(_tmp2D4,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2D4,e2)){void*_tmp2D8=({struct Cyc_String_pa_struct
_tmp2D7;_tmp2D7.tag=0;_tmp2D7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2D5[1]={& _tmp2D7};Cyc_Tcexp_expr_err(
_tmp2D4,e2->loc,topt,({const char*_tmp2D6="expecting int subscript, found %s";
_tag_dynforward(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,34));}),
_tag_dynforward(_tmp2D5,sizeof(void*),1));}});_npop_handler(0);return _tmp2D8;}{
void*_tmp2D9=t1;struct Cyc_Absyn_PtrInfo _tmp2DA;void*_tmp2DB;struct Cyc_Absyn_Tqual
_tmp2DC;struct Cyc_Absyn_PtrAtts _tmp2DD;void*_tmp2DE;struct Cyc_Absyn_Conref*
_tmp2DF;struct Cyc_Absyn_Conref*_tmp2E0;struct Cyc_List_List*_tmp2E1;_LL19F: if(
_tmp2D9 <= (void*)4)goto _LL1A3;if(*((int*)_tmp2D9)!= 4)goto _LL1A1;_tmp2DA=((
struct Cyc_Absyn_PointerType_struct*)_tmp2D9)->f1;_tmp2DB=(void*)_tmp2DA.elt_typ;
_tmp2DC=_tmp2DA.elt_tq;_tmp2DD=_tmp2DA.ptr_atts;_tmp2DE=(void*)_tmp2DD.rgn;
_tmp2DF=_tmp2DD.bounds;_tmp2E0=_tmp2DD.zero_term;_LL1A0: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2E0)){int emit_warning=1;{union
Cyc_Absyn_Constraint_union _tmp2E2=(Cyc_Absyn_compress_conref(_tmp2DF))->v;void*
_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E4;void*_tmp2E5;void*_tmp2E6;_LL1A6: if((_tmp2E2.Eq_constr).tag
!= 0)goto _LL1A8;_tmp2E3=(_tmp2E2.Eq_constr).f1;if(_tmp2E3 <= (void*)2)goto _LL1A8;
if(*((int*)_tmp2E3)!= 0)goto _LL1A8;_tmp2E4=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2E3)->f1;_LL1A7: if(Cyc_Tcutil_is_const_exp(_tmp2D4,e2)){unsigned int _tmp2E8;
int _tmp2E9;struct _tuple7 _tmp2E7=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2E8=
_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;if(_tmp2E9){unsigned int _tmp2EB;int _tmp2EC;struct
_tuple7 _tmp2EA=Cyc_Evexp_eval_const_uint_exp(_tmp2E4);_tmp2EB=_tmp2EA.f1;_tmp2EC=
_tmp2EA.f2;if(_tmp2EC  && _tmp2EB > _tmp2E8)emit_warning=0;}}goto _LL1A5;_LL1A8: if((
_tmp2E2.Eq_constr).tag != 0)goto _LL1AA;_tmp2E5=(_tmp2E2.Eq_constr).f1;if((int)
_tmp2E5 != 0)goto _LL1AA;_LL1A9: emit_warning=0;goto _LL1A5;_LL1AA: if((_tmp2E2.Eq_constr).tag
!= 0)goto _LL1AC;_tmp2E6=(_tmp2E2.Eq_constr).f1;if((int)_tmp2E6 != 1)goto _LL1AC;
_LL1AB: emit_warning=0;goto _LL1A5;_LL1AC:;_LL1AD: goto _LL1A5;_LL1A5:;}if(
emit_warning)({void*_tmp2ED[0]={};Cyc_Tcutil_warn(e2->loc,({const char*_tmp2EE="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dynforward(_tmp2EE,sizeof(char),_get_zero_arr_size(_tmp2EE,63));}),
_tag_dynforward(_tmp2ED,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(
_tmp2D4,e2)){unsigned int _tmp2F0;int _tmp2F1;struct _tuple7 _tmp2EF=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2F0=_tmp2EF.f1;_tmp2F1=_tmp2EF.f2;if(_tmp2F1)Cyc_Tcutil_check_bound(loc,
_tmp2F0,_tmp2DF);}else{if(Cyc_Tcutil_is_bound_one(_tmp2DF) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2E0))({void*_tmp2F2[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmp2F3="subscript applied to pointer to one object";
_tag_dynforward(_tmp2F3,sizeof(char),_get_zero_arr_size(_tmp2F3,43));}),
_tag_dynforward(_tmp2F2,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,
_tmp2DF);}}Cyc_Tcenv_check_rgn_accessible(_tmp2D4,loc,_tmp2DE);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2DB),(void*)1))({void*_tmp2F4[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp2F5="can't subscript an abstract pointer";_tag_dynforward(_tmp2F5,
sizeof(char),_get_zero_arr_size(_tmp2F5,36));}),_tag_dynforward(_tmp2F4,sizeof(
void*),0));});{void*_tmp2F6=_tmp2DB;_npop_handler(0);return _tmp2F6;}_LL1A1: if(*((
int*)_tmp2D9)!= 9)goto _LL1A3;_tmp2E1=((struct Cyc_Absyn_TupleType_struct*)_tmp2D9)->f1;
_LL1A2: {void*_tmp2F7=Cyc_Tcexp_ithTupleType(_tmp2D4,loc,_tmp2E1,e2);
_npop_handler(0);return _tmp2F7;}_LL1A3:;_LL1A4: {void*_tmp2FB=({struct Cyc_String_pa_struct
_tmp2FA;_tmp2FA.tag=0;_tmp2FA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp2F8[1]={& _tmp2FA};Cyc_Tcexp_expr_err(
_tmp2D4,loc,topt,({const char*_tmp2F9="subscript applied to %s";_tag_dynforward(
_tmp2F9,sizeof(char),_get_zero_arr_size(_tmp2F9,24));}),_tag_dynforward(_tmp2F8,
sizeof(void*),1));}});_npop_handler(0);return _tmp2FB;}_LL19E:;}}};_pop_region(r);}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2FC=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2FD;
_LL1AF: if(_tmp2FC <= (void*)4)goto _LL1B1;if(*((int*)_tmp2FC)!= 9)goto _LL1B1;
_tmp2FD=((struct Cyc_Absyn_TupleType_struct*)_tmp2FC)->f1;_LL1B0: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2FD)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AE;for(0;es != 0;(es=es->tl,_tmp2FD=_tmp2FD->tl)){
void*_tmp2FE=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2FD))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2FE,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2FD->hd)).f2);fields=({
struct Cyc_List_List*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->hd=({struct
_tuple9*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->f1=(*((struct _tuple9*)
_tmp2FD->hd)).f1;_tmp300->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp300;});_tmp2FF->tl=fields;_tmp2FF;});}done=
1;goto _LL1AE;_LL1B1:;_LL1B2: goto _LL1AE;_LL1AE:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->hd=({struct _tuple9*_tmp302=
_cycalloc(sizeof(*_tmp302));_tmp302->f1=Cyc_Absyn_empty_tqual(0);_tmp302->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp302;});_tmp301->tl=fields;_tmp301;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_Absyn_TupleType_struct
_tmp304;_tmp304.tag=9;_tmp304.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp304;});_tmp303;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp305[0]={};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp306="tcCompoundLit";_tag_dynforward(_tmp306,sizeof(char),
_get_zero_arr_size(_tmp306,14));}),_tag_dynforward(_tmp305,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des){void*res_t2;{struct
_RegionHandle _tmp307=_new_region("r");struct _RegionHandle*r=& _tmp307;
_push_region(r);{int _tmp308=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp314;}));struct Cyc_Absyn_Const_e_struct*
_tmp309=({struct Cyc_Absyn_Const_e_struct*_tmp311=_cycalloc(sizeof(*_tmp311));
_tmp311[0]=({struct Cyc_Absyn_Const_e_struct _tmp312;_tmp312.tag=0;_tmp312.f1=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp313;(_tmp313.Int_c).tag=
2;(_tmp313.Int_c).f1=(void*)((void*)1);(_tmp313.Int_c).f2=_tmp308;_tmp313;});
_tmp312;});_tmp311;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp309,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp30A=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp308 - 1);if(!Cyc_Tcutil_is_zero(
_tmp30A))({void*_tmp30B[0]={};Cyc_Tcutil_terr(_tmp30A->loc,({const char*_tmp30C="zero-terminated array doesn't end with zero.";
_tag_dynforward(_tmp30C,sizeof(char),_get_zero_arr_size(_tmp30C,45));}),
_tag_dynforward(_tmp30B,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*
_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D->v=(void*)Cyc_Absyn_uint_typ;_tmp30D;});
res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp310;_tmp310.tag=0;_tmp310.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(res));{void*_tmp30E[1]={& _tmp310};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp30F="elements of array do not all have the same type (%s)";
_tag_dynforward(_tmp30F,sizeof(char),_get_zero_arr_size(_tmp30F,53));}),
_tag_dynforward(_tmp30E,sizeof(void*),1));}});};_pop_region(r);}{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple8*)
des->hd)).f1;if(ds != 0){void*_tmp315=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp316;
_LL1B4: if(*((int*)_tmp315)!= 1)goto _LL1B6;_LL1B5:({void*_tmp317[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp318="only array index designators are supported";
_tag_dynforward(_tmp318,sizeof(char),_get_zero_arr_size(_tmp318,43));}),
_tag_dynforward(_tmp317,sizeof(void*),0));});goto _LL1B3;_LL1B6: if(*((int*)
_tmp315)!= 0)goto _LL1B3;_tmp316=((struct Cyc_Absyn_ArrayElement_struct*)_tmp315)->f1;
_LL1B7: Cyc_Tcexp_tcExpInitializer(te,0,_tmp316);{unsigned int _tmp31A;int _tmp31B;
struct _tuple7 _tmp319=Cyc_Evexp_eval_const_uint_exp(_tmp316);_tmp31A=_tmp319.f1;
_tmp31B=_tmp319.f2;if(!_tmp31B)({void*_tmp31C[0]={};Cyc_Tcutil_terr(_tmp316->loc,({
const char*_tmp31D="index designator cannot use sizeof or offsetof";
_tag_dynforward(_tmp31D,sizeof(char),_get_zero_arr_size(_tmp31D,47));}),
_tag_dynforward(_tmp31C,sizeof(void*),0));});else{if(_tmp31A != offset)({struct
Cyc_Int_pa_struct _tmp321;_tmp321.tag=1;_tmp321.f1=(unsigned long)((int)_tmp31A);{
struct Cyc_Int_pa_struct _tmp320;_tmp320.tag=1;_tmp320.f1=(unsigned long)offset;{
void*_tmp31E[2]={& _tmp320,& _tmp321};Cyc_Tcutil_terr(_tmp316->loc,({const char*
_tmp31F="expecting index designator %d but found %d";_tag_dynforward(_tmp31F,
sizeof(char),_get_zero_arr_size(_tmp31F,43));}),_tag_dynforward(_tmp31E,sizeof(
void*),2));}}});}goto _LL1B3;}_LL1B3:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp322=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1B9: if(_tmp322 <= (void*)4)goto _LL1BB;if(*((int*)
_tmp322)!= 18)goto _LL1BB;_LL1BA: goto _LL1B8;_LL1BB:;_LL1BC: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp325;_tmp325.tag=0;_tmp325.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(bound->topt))->v));{void*_tmp323[1]={& _tmp325};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp324="expecting unsigned int, found %s";
_tag_dynforward(_tmp324,sizeof(char),_get_zero_arr_size(_tmp324,33));}),
_tag_dynforward(_tmp323,sizeof(void*),1));}});_LL1B8:;}if(!(vd->tq).real_const)({
void*_tmp326[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp327="comprehension index variable is not declared const!";
_tag_dynforward(_tmp327,sizeof(char),_get_zero_arr_size(_tmp327,52));}),
_tag_dynforward(_tmp326,sizeof(void*),0));});{struct _RegionHandle _tmp328=
_new_region("r");struct _RegionHandle*r=& _tmp328;_push_region(r);{struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp329=te2;
struct Cyc_Absyn_PtrInfo pinfo;void**_tmp32A=0;struct Cyc_Absyn_Tqual*_tmp32B=0;
struct Cyc_Absyn_Conref**_tmp32C=0;if(topt != 0){void*_tmp32D=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp32E;struct Cyc_Absyn_ArrayInfo _tmp32F;void*
_tmp330;void**_tmp331;struct Cyc_Absyn_Tqual _tmp332;struct Cyc_Absyn_Tqual*_tmp333;
struct Cyc_Absyn_Exp*_tmp334;struct Cyc_Absyn_Conref*_tmp335;struct Cyc_Absyn_Conref**
_tmp336;_LL1BE: if(_tmp32D <= (void*)4)goto _LL1C2;if(*((int*)_tmp32D)!= 4)goto
_LL1C0;_tmp32E=((struct Cyc_Absyn_PointerType_struct*)_tmp32D)->f1;_LL1BF: pinfo=
_tmp32E;_tmp32A=(void**)((void**)((void*)& pinfo.elt_typ));_tmp32B=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp32C=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1BD;_LL1C0: if(*((int*)_tmp32D)!= 7)goto _LL1C2;_tmp32F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp32D)->f1;_tmp330=(void*)_tmp32F.elt_type;_tmp331=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp32D)->f1).elt_type;_tmp332=_tmp32F.tq;_tmp333=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp32D)->f1).tq;_tmp334=_tmp32F.num_elts;
_tmp335=_tmp32F.zero_term;_tmp336=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp32D)->f1).zero_term;_LL1C1: _tmp32A=(void**)_tmp331;_tmp32B=(struct Cyc_Absyn_Tqual*)
_tmp333;_tmp32C=(struct Cyc_Absyn_Conref**)_tmp336;goto _LL1BD;_LL1C2:;_LL1C3: goto
_LL1BD;_LL1BD:;}{void*t=Cyc_Tcexp_tcExp(_tmp329,_tmp32A,body);if(_tmp329->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp329,bound))({void*_tmp337[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp338="bound is not constant";_tag_dynforward(_tmp338,
sizeof(char),_get_zero_arr_size(_tmp338,22));}),_tag_dynforward(_tmp337,sizeof(
void*),0));});if(!Cyc_Tcutil_is_const_exp(_tmp329,body))({void*_tmp339[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp33A="body is not constant";_tag_dynforward(_tmp33A,
sizeof(char),_get_zero_arr_size(_tmp33A,21));}),_tag_dynforward(_tmp339,sizeof(
void*),0));});}if(_tmp32C != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,*_tmp32C)){struct Cyc_Absyn_Exp*_tmp33B=Cyc_Absyn_uint_exp(1,0);_tmp33B->topt=({
struct Cyc_Core_Opt*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->v=(void*)Cyc_Absyn_uint_typ;
_tmp33C;});bound=Cyc_Absyn_add_exp(bound,_tmp33B,0);bound->topt=({struct Cyc_Core_Opt*
_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->v=(void*)Cyc_Absyn_uint_typ;_tmp33D;});*
is_zero_term=1;}{void*_tmp33E=Cyc_Absyn_array_typ(t,_tmp32B == 0?Cyc_Absyn_empty_tqual(
0):*_tmp32B,(struct Cyc_Absyn_Exp*)bound,_tmp32C == 0?Cyc_Absyn_false_conref:*
_tmp32C,0);_npop_handler(0);return _tmp33E;}}}};_pop_region(r);}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp33F;_push_handler(& _tmp33F);{
int _tmp341=0;if(setjmp(_tmp33F.handler))_tmp341=1;if(!_tmp341){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp340=(void*)_exn_thrown;void*_tmp343=
_tmp340;_LL1C5: if(_tmp343 != Cyc_Dict_Absent)goto _LL1C7;_LL1C6:({struct Cyc_String_pa_struct
_tmp346;_tmp346.tag=0;_tmp346.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp344[1]={& _tmp346};Cyc_Tcutil_terr(loc,({
const char*_tmp345="unbound struct name %s";_tag_dynforward(_tmp345,sizeof(char),
_get_zero_arr_size(_tmp345,23));}),_tag_dynforward(_tmp344,sizeof(void*),1));}});
return topt != 0?*topt:(void*)0;_LL1C7:;_LL1C8:(void)_throw(_tmp343);_LL1C4:;}}}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({
void*_tmp347[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp348="expecting struct but found union";
_tag_dynforward(_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,33));}),
_tag_dynforward(_tmp347,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp349[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp34A="can't build abstract struct";
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,28));}),
_tag_dynforward(_tmp349,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp34B;}));}{struct _RegionHandle _tmp34C=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp34C;_push_region(rgn);{struct _tuple4 _tmp34D=({struct _tuple4 _tmp36D;
_tmp36D.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp36D.f2=rgn;_tmp36D;});struct Cyc_List_List*
_tmp34E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp34D,ad->tvs);struct Cyc_List_List*_tmp34F=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp34D,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp350=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp34E);struct Cyc_List_List*_tmp351=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp34F);struct Cyc_List_List*_tmp352=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp34E,_tmp34F);struct Cyc_Absyn_AggrType_struct*_tmp353=({struct Cyc_Absyn_AggrType_struct*
_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368[0]=({struct Cyc_Absyn_AggrType_struct
_tmp369;_tmp369.tag=10;_tmp369.f1=({struct Cyc_Absyn_AggrInfo _tmp36A;_tmp36A.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp36B;(_tmp36B.KnownAggr).tag=
1;(_tmp36B.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp36C=_cycalloc(sizeof(*
_tmp36C));_tmp36C[0]=ad;_tmp36C;});_tmp36B;});_tmp36A.targs=_tmp350;_tmp36A;});
_tmp369;});_tmp368;});struct Cyc_List_List*_tmp354=*ts;struct Cyc_List_List*
_tmp355=_tmp351;while(_tmp354 != 0  && _tmp355 != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp354->hd);Cyc_Tcutil_unify((
void*)_tmp354->hd,(void*)_tmp355->hd);_tmp354=_tmp354->tl;_tmp355=_tmp355->tl;}
if(_tmp354 != 0)({void*_tmp356[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp357="too many explicit witness types";
_tag_dynforward(_tmp357,sizeof(char),_get_zero_arr_size(_tmp357,32));}),
_tag_dynforward(_tmp356,sizeof(void*),0));});*ts=_tmp351;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp353,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp359;struct Cyc_Absyn_Aggrfield*_tmp35A;
struct Cyc_Absyn_Exp*_tmp35B;struct _tuple10*_tmp358=(struct _tuple10*)fields->hd;
_tmp359=*_tmp358;_tmp35A=_tmp359.f1;_tmp35B=_tmp359.f2;{void*_tmp35C=Cyc_Tcutil_rsubstitute(
rgn,_tmp352,(void*)_tmp35A->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp35C,
_tmp35B);if(!Cyc_Tcutil_coerce_arg(te,_tmp35B,_tmp35C)){({struct Cyc_String_pa_struct
_tmp362;_tmp362.tag=0;_tmp362.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp35B->topt))->v));{
struct Cyc_String_pa_struct _tmp361;_tmp361.tag=0;_tmp361.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp35C));{struct
Cyc_String_pa_struct _tmp360;_tmp360.tag=0;_tmp360.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp35F;_tmp35F.tag=0;_tmp35F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp35A->name);{void*_tmp35D[4]={& _tmp35F,& _tmp360,& _tmp361,& _tmp362};Cyc_Tcutil_terr(
_tmp35B->loc,({const char*_tmp35E="field %s of struct %s expects type %s != %s";
_tag_dynforward(_tmp35E,sizeof(char),_get_zero_arr_size(_tmp35E,44));}),
_tag_dynforward(_tmp35D,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp363=0;{struct Cyc_List_List*_tmp364=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp364 != 0;_tmp364=_tmp364->tl){_tmp363=({
struct Cyc_List_List*_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365->hd=({struct
_tuple0*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp352,(*((struct _tuple0*)_tmp364->hd)).f1);_tmp366->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp352,(*((struct _tuple0*)_tmp364->hd)).f2);_tmp366;});_tmp365->tl=_tmp363;
_tmp365;});}}_tmp363=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp363);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp363);{void*_tmp367=(void*)
_tmp353;_npop_handler(0);return _tmp367;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp36E=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp36E;_push_region(rgn);{void*_tmp36F=Cyc_Tcutil_compress(ts);void*_tmp370;
struct Cyc_List_List*_tmp371;_LL1CA: if(_tmp36F <= (void*)4)goto _LL1CC;if(*((int*)
_tmp36F)!= 11)goto _LL1CC;_tmp370=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp36F)->f1;_tmp371=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp36F)->f2;_LL1CB:
if(_tmp370 == (void*)1)({void*_tmp372[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp373="expecting struct but found union";_tag_dynforward(_tmp373,sizeof(char),
_get_zero_arr_size(_tmp373,33));}),_tag_dynforward(_tmp372,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp371);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp375;struct Cyc_Absyn_Aggrfield*
_tmp376;struct Cyc_Absyn_Exp*_tmp377;struct _tuple10*_tmp374=(struct _tuple10*)
fields->hd;_tmp375=*_tmp374;_tmp376=_tmp375.f1;_tmp377=_tmp375.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp376->type)),_tmp377);if(!Cyc_Tcutil_coerce_arg(
te,_tmp377,(void*)_tmp376->type)){({struct Cyc_String_pa_struct _tmp37C;_tmp37C.tag=
0;_tmp37C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp377->topt))->v));{struct Cyc_String_pa_struct
_tmp37B;_tmp37B.tag=0;_tmp37B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)_tmp376->type));{struct Cyc_String_pa_struct _tmp37A;
_tmp37A.tag=0;_tmp37A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp376->name);{
void*_tmp378[3]={& _tmp37A,& _tmp37B,& _tmp37C};Cyc_Tcutil_terr(_tmp377->loc,({
const char*_tmp379="field %s of struct expects type %s != %s";_tag_dynforward(
_tmp379,sizeof(char),_get_zero_arr_size(_tmp379,41));}),_tag_dynforward(_tmp378,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1C9;}_LL1CC:;
_LL1CD:({void*_tmp37D[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp37E="tcAnonStruct: wrong type";
_tag_dynforward(_tmp37E,sizeof(char),_get_zero_arr_size(_tmp37E,25));}),
_tag_dynforward(_tmp37D,sizeof(void*),0));});_LL1C9:;};_pop_region(rgn);}return
ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp37F=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp37F;_push_region(rgn);{struct _tuple4 _tmp380=({
struct _tuple4 _tmp3A0;_tmp3A0.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp3A0.f2=rgn;
_tmp3A0;});struct Cyc_List_List*_tmp381=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp380,tud->tvs);struct Cyc_List_List*_tmp382=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp381);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp39D;_tmp39D.tag=3;_tmp39D.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp39E;
_tmp39E.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({union Cyc_Absyn_TunionFieldInfoU_union
_tmp39F;(_tmp39F.KnownTunionfield).tag=1;(_tmp39F.KnownTunionfield).f1=tud;(
_tmp39F.KnownTunionfield).f2=tuf;_tmp39F;});_tmp39E.targs=_tmp382;_tmp39E;});
_tmp39D;});_tmp39C;});if(topt != 0){void*_tmp383=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_TunionInfo _tmp384;struct Cyc_List_List*_tmp385;struct Cyc_Core_Opt*
_tmp386;_LL1CF: if(_tmp383 <= (void*)4)goto _LL1D3;if(*((int*)_tmp383)!= 3)goto
_LL1D1;_LL1D0: Cyc_Tcutil_unify(*topt,res);goto _LL1CE;_LL1D1: if(*((int*)_tmp383)
!= 2)goto _LL1D3;_tmp384=((struct Cyc_Absyn_TunionType_struct*)_tmp383)->f1;
_tmp385=_tmp384.targs;_tmp386=_tmp384.rgn;_LL1D2:{struct Cyc_List_List*a=_tmp382;
for(0;a != 0  && _tmp385 != 0;(a=a->tl,_tmp385=_tmp385->tl)){Cyc_Tcutil_unify((void*)
a->hd,(void*)_tmp385->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){e->topt=({
struct Cyc_Core_Opt*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->v=(void*)res;
_tmp387;});res=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp388=_cycalloc(
sizeof(*_tmp388));_tmp388[0]=({struct Cyc_Absyn_TunionType_struct _tmp389;_tmp389.tag=
2;_tmp389.f1=({struct Cyc_Absyn_TunionInfo _tmp38A;_tmp38A.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp38B;(_tmp38B.KnownTunion).tag=1;(_tmp38B.KnownTunion).f1=({
struct Cyc_Absyn_Tuniondecl**_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=tud;
_tmp38C;});_tmp38B;});_tmp38A.targs=_tmp382;_tmp38A.rgn=_tmp386;_tmp38A;});
_tmp389;});_tmp388;});Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}goto _LL1CE;
_LL1D3:;_LL1D4: goto _LL1CE;_LL1CE:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 
0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp381,(*((struct _tuple9*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp391;_tmp391.tag=0;_tmp391.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)(e->topt == 0?(struct _dynforward_ptr)({
const char*_tmp392="?";_tag_dynforward(_tmp392,sizeof(char),_get_zero_arr_size(
_tmp392,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{
struct Cyc_String_pa_struct _tmp390;_tmp390.tag=0;_tmp390.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp38F;_tmp38F.tag=0;_tmp38F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp38D[3]={& _tmp38F,& _tmp390,& _tmp391};
Cyc_Tcutil_terr(e->loc,({const char*_tmp38E="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_dynforward(_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,80));}),
_tag_dynforward(_tmp38D,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp396=({struct Cyc_String_pa_struct _tmp395;_tmp395.tag=0;
_tmp395.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp393[1]={& _tmp395};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)
es->hd)->loc,topt,({const char*_tmp394="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp394,sizeof(char),_get_zero_arr_size(_tmp394,45));}),
_tag_dynforward(_tmp393,sizeof(void*),1));}});_npop_handler(0);return _tmp396;}
if(ts != 0){void*_tmp39A=({struct Cyc_String_pa_struct _tmp399;_tmp399.tag=0;
_tmp399.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp397[1]={& _tmp399};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp398="too few arguments for tunion constructor %s";_tag_dynforward(
_tmp398,sizeof(char),_get_zero_arr_size(_tmp398,44));}),_tag_dynforward(_tmp397,
sizeof(void*),1));}});_npop_handler(0);return _tmp39A;}{void*_tmp39B=res;
_npop_handler(0);return _tmp39B;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp3A1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3A2;
struct Cyc_Absyn_PtrAtts _tmp3A3;struct Cyc_Absyn_Conref*_tmp3A4;struct Cyc_Absyn_ArrayInfo
_tmp3A5;void*_tmp3A6;struct Cyc_List_List*_tmp3A7;struct Cyc_Absyn_AggrInfo _tmp3A8;
union Cyc_Absyn_AggrInfoU_union _tmp3A9;struct Cyc_List_List*_tmp3AA;struct Cyc_List_List*
_tmp3AB;_LL1D6: if((int)_tmp3A1 != 0)goto _LL1D8;_LL1D7: return 1;_LL1D8: if(_tmp3A1 <= (
void*)4)goto _LL1E4;if(*((int*)_tmp3A1)!= 0)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA:
if(*((int*)_tmp3A1)!= 1)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(*((int*)_tmp3A1)
!= 2)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(*((int*)_tmp3A1)!= 3)goto _LL1E0;
_LL1DF: return 0;_LL1E0: if(*((int*)_tmp3A1)!= 4)goto _LL1E2;_tmp3A2=((struct Cyc_Absyn_PointerType_struct*)
_tmp3A1)->f1;_tmp3A3=_tmp3A2.ptr_atts;_tmp3A4=_tmp3A3.nullable;_LL1E1: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp3A4);_LL1E2: if(*((
int*)_tmp3A1)!= 5)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp3A1 != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp3A1 <= (void*)4)goto _LL202;if(*((int*)
_tmp3A1)!= 6)goto _LL1E8;_LL1E7: return 1;_LL1E8: if(*((int*)_tmp3A1)!= 7)goto _LL1EA;
_tmp3A5=((struct Cyc_Absyn_ArrayType_struct*)_tmp3A1)->f1;_tmp3A6=(void*)_tmp3A5.elt_type;
_LL1E9: return Cyc_Tcexp_zeroable_type(_tmp3A6);_LL1EA: if(*((int*)_tmp3A1)!= 8)
goto _LL1EC;_LL1EB: return 0;_LL1EC: if(*((int*)_tmp3A1)!= 9)goto _LL1EE;_tmp3A7=((
struct Cyc_Absyn_TupleType_struct*)_tmp3A1)->f1;_LL1ED: for(0;(unsigned int)
_tmp3A7;_tmp3A7=_tmp3A7->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp3A7->hd)).f2))return 0;}return 1;_LL1EE: if(*((int*)_tmp3A1)!= 10)goto _LL1F0;
_tmp3A8=((struct Cyc_Absyn_AggrType_struct*)_tmp3A1)->f1;_tmp3A9=_tmp3A8.aggr_info;
_tmp3AA=_tmp3A8.targs;_LL1EF: {struct Cyc_Absyn_Aggrdecl*_tmp3AC=Cyc_Absyn_get_known_aggrdecl(
_tmp3A9);if(_tmp3AC->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3AC->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp3AD=_new_region("r");
struct _RegionHandle*r=& _tmp3AD;_push_region(r);{struct Cyc_List_List*_tmp3AE=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp3AC->tvs,_tmp3AA);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AC->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3AE,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3AF=0;_npop_handler(0);
return _tmp3AF;}}}{int _tmp3B0=1;_npop_handler(0);return _tmp3B0;}};_pop_region(r);}}
_LL1F0: if(*((int*)_tmp3A1)!= 12)goto _LL1F2;_LL1F1: return 1;_LL1F2: if(*((int*)
_tmp3A1)!= 14)goto _LL1F4;_LL1F3: return 1;_LL1F4: if(*((int*)_tmp3A1)!= 18)goto
_LL1F6;_LL1F5: return 1;_LL1F6: if(*((int*)_tmp3A1)!= 11)goto _LL1F8;_tmp3AB=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp3A1)->f2;_LL1F7: for(0;_tmp3AB != 0;
_tmp3AB=_tmp3AB->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp3AB->hd)->type))return 0;}return 1;_LL1F8: if(*((int*)_tmp3A1)!= 13)goto _LL1FA;
_LL1F9: return 1;_LL1FA: if(*((int*)_tmp3A1)!= 17)goto _LL1FC;_LL1FB: return 0;_LL1FC:
if(*((int*)_tmp3A1)!= 16)goto _LL1FE;_LL1FD: return 0;_LL1FE: if(*((int*)_tmp3A1)!= 
15)goto _LL200;_LL1FF: return 0;_LL200: if(*((int*)_tmp3A1)!= 19)goto _LL202;_LL201:
goto _LL203;_LL202: if((int)_tmp3A1 != 2)goto _LL204;_LL203: goto _LL205;_LL204: if((
int)_tmp3A1 != 3)goto _LL206;_LL205: goto _LL207;_LL206: if(_tmp3A1 <= (void*)4)goto
_LL208;if(*((int*)_tmp3A1)!= 20)goto _LL208;_LL207: goto _LL209;_LL208: if(_tmp3A1 <= (
void*)4)goto _LL20A;if(*((int*)_tmp3A1)!= 21)goto _LL20A;_LL209: goto _LL20B;_LL20A:
if(_tmp3A1 <= (void*)4)goto _LL1D5;if(*((int*)_tmp3A1)!= 22)goto _LL1D5;_LL20B:({
struct Cyc_String_pa_struct _tmp3B3;_tmp3B3.tag=0;_tmp3B3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp3B1[
1]={& _tmp3B3};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3B2="bad type `%s' in zeroable type";_tag_dynforward(_tmp3B2,
sizeof(char),_get_zero_arr_size(_tmp3B2,31));}),_tag_dynforward(_tmp3B1,sizeof(
void*),1));}});_LL1D5:;}static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3B4=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3B5;void*_tmp3B6;_LL20D: if(_tmp3B4 <= (void*)4)
goto _LL20F;if(*((int*)_tmp3B4)!= 4)goto _LL20F;_tmp3B5=((struct Cyc_Absyn_PointerType_struct*)
_tmp3B4)->f1;_tmp3B6=(void*)_tmp3B5.elt_typ;_LL20E: Cyc_Tcutil_unify(_tmp3B6,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL20C;_LL20F:;_LL210: goto _LL20C;_LL20C:;}({struct Cyc_String_pa_struct
_tmp3BA;_tmp3BA.tag=0;_tmp3BA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp3B9;_tmp3B9.tag=0;
_tmp3B9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)(allow_zero?(struct
_dynforward_ptr)({const char*_tmp3BB="calloc";_tag_dynforward(_tmp3BB,sizeof(char),
_get_zero_arr_size(_tmp3BB,7));}):(struct _dynforward_ptr)({const char*_tmp3BC="malloc";
_tag_dynforward(_tmp3BC,sizeof(char),_get_zero_arr_size(_tmp3BC,7));})));{void*
_tmp3B7[2]={& _tmp3B9,& _tmp3BA};Cyc_Tcutil_terr(loc,({const char*_tmp3B8="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dynforward(_tmp3B8,sizeof(char),_get_zero_arr_size(_tmp3B8,60));}),
_tag_dynforward(_tmp3B7,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3C3;_tmp3C3.tag=15;_tmp3C3.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp3C4;}));_tmp3C3;});_tmp3C2;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3BD=Cyc_Tcutil_compress(handle_type);void*_tmp3BE;_LL212: if(_tmp3BD <= (
void*)4)goto _LL214;if(*((int*)_tmp3BD)!= 15)goto _LL214;_tmp3BE=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3BD)->f1;_LL213: rgn=_tmp3BE;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL211;_LL214:;_LL215:({struct Cyc_String_pa_struct _tmp3C1;
_tmp3C1.tag=0;_tmp3C1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3BF[1]={& _tmp3C1};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp3C0="expecting region_t type but found %s";_tag_dynforward(_tmp3C0,sizeof(
char),_get_zero_arr_size(_tmp3C0,37));}),_tag_dynforward(_tmp3BF,sizeof(void*),1));}});
goto _LL211;_LL211:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({void*
_tmp3C5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3C6="calloc with empty type";_tag_dynforward(_tmp3C6,sizeof(char),
_get_zero_arr_size(_tmp3C6,23));}),_tag_dynforward(_tmp3C5,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3C7=(void*)(*e)->r;void*_tmp3C8;void*
_tmp3C9;struct Cyc_List_List*_tmp3CA;struct Cyc_List_List _tmp3CB;struct Cyc_Absyn_Exp*
_tmp3CC;struct Cyc_List_List*_tmp3CD;struct Cyc_List_List _tmp3CE;struct Cyc_Absyn_Exp*
_tmp3CF;struct Cyc_List_List*_tmp3D0;_LL217: if(*((int*)_tmp3C7)!= 18)goto _LL219;
_tmp3C8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C7)->f1;_LL218:
elt_type=_tmp3C8;*t=(void**)({void**_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[
0]=elt_type;_tmp3D1;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL216;_LL219: if(*((int*)_tmp3C7)!= 3)
goto _LL21B;_tmp3C9=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3C7)->f1;if((
int)_tmp3C9 != 1)goto _LL21B;_tmp3CA=((struct Cyc_Absyn_Primop_e_struct*)_tmp3C7)->f2;
if(_tmp3CA == 0)goto _LL21B;_tmp3CB=*_tmp3CA;_tmp3CC=(struct Cyc_Absyn_Exp*)_tmp3CB.hd;
_tmp3CD=_tmp3CB.tl;if(_tmp3CD == 0)goto _LL21B;_tmp3CE=*_tmp3CD;_tmp3CF=(struct Cyc_Absyn_Exp*)
_tmp3CE.hd;_tmp3D0=_tmp3CE.tl;if(_tmp3D0 != 0)goto _LL21B;_LL21A:{struct _tuple0
_tmp3D3=({struct _tuple0 _tmp3D2;_tmp3D2.f1=(void*)_tmp3CC->r;_tmp3D2.f2=(void*)
_tmp3CF->r;_tmp3D2;});void*_tmp3D4;void*_tmp3D5;void*_tmp3D6;void*_tmp3D7;_LL21E:
_tmp3D4=_tmp3D3.f1;if(*((int*)_tmp3D4)!= 18)goto _LL220;_tmp3D5=(void*)((struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3D4)->f1;_LL21F: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp3D5);elt_type=_tmp3D5;*t=(void**)({void**_tmp3D8=_cycalloc(sizeof(*
_tmp3D8));_tmp3D8[0]=elt_type;_tmp3D8;});num_elts=_tmp3CF;one_elt=0;goto _LL21D;
_LL220: _tmp3D6=_tmp3D3.f2;if(*((int*)_tmp3D6)!= 18)goto _LL222;_tmp3D7=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3D6)->f1;_LL221: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3D7);elt_type=_tmp3D7;*t=(void**)({void**_tmp3D9=_cycalloc(
sizeof(*_tmp3D9));_tmp3D9[0]=elt_type;_tmp3D9;});num_elts=_tmp3CC;one_elt=0;goto
_LL21D;_LL222:;_LL223: goto No_sizeof;_LL21D:;}goto _LL216;_LL21B:;_LL21C: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3DA=_cycalloc(sizeof(*_tmp3DA));
_tmp3DA[0]=elt_type;_tmp3DA;});num_elts=*e;one_elt=0;goto _LL216;_LL216:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3DB=elt_type;struct Cyc_Absyn_AggrInfo _tmp3DC;
union Cyc_Absyn_AggrInfoU_union _tmp3DD;struct Cyc_Absyn_Aggrdecl**_tmp3DE;struct
Cyc_Absyn_Aggrdecl*_tmp3DF;_LL225: if(_tmp3DB <= (void*)4)goto _LL227;if(*((int*)
_tmp3DB)!= 10)goto _LL227;_tmp3DC=((struct Cyc_Absyn_AggrType_struct*)_tmp3DB)->f1;
_tmp3DD=_tmp3DC.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3DB)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL227;_tmp3DE=(_tmp3DD.KnownAggr).f1;_tmp3DF=*_tmp3DE;_LL226: if(
_tmp3DF->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3DF->impl))->exist_vars
!= 0)({void*_tmp3E0[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3E1="malloc with existential types not yet implemented";
_tag_dynforward(_tmp3E1,sizeof(char),_get_zero_arr_size(_tmp3E1,50));}),
_tag_dynforward(_tmp3E0,sizeof(void*),0));});goto _LL224;_LL227:;_LL228: goto
_LL224;_LL224:;}{void*(*_tmp3E2)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp3E3=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp3E4=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3E5;struct Cyc_Absyn_PtrAtts _tmp3E6;struct Cyc_Absyn_Conref*_tmp3E7;struct Cyc_Absyn_Conref*
_tmp3E8;_LL22A: if(_tmp3E4 <= (void*)4)goto _LL22C;if(*((int*)_tmp3E4)!= 4)goto
_LL22C;_tmp3E5=((struct Cyc_Absyn_PointerType_struct*)_tmp3E4)->f1;_tmp3E6=
_tmp3E5.ptr_atts;_tmp3E7=_tmp3E6.nullable;_tmp3E8=_tmp3E6.zero_term;_LL22B:
_tmp3E3=_tmp3E8;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3E7))_tmp3E2=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3E8) && !(*is_calloc)){({void*_tmp3E9[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp3EA="converting malloc to calloc to ensure zero-termination";
_tag_dynforward(_tmp3EA,sizeof(char),_get_zero_arr_size(_tmp3EA,55));}),
_tag_dynforward(_tmp3E9,sizeof(void*),0));});*is_calloc=1;}goto _LL229;_LL22C:;
_LL22D: goto _LL229;_LL229:;}if(!one_elt)_tmp3E2=Cyc_Absyn_dynforward_typ;return
_tmp3E2(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3E3);}}}static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3EB=Cyc_Tcutil_compress(t1);_LL22F: if(
_tmp3EB <= (void*)4)goto _LL231;if(*((int*)_tmp3EB)!= 7)goto _LL231;_LL230:({void*
_tmp3EC[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3ED="cannot assign to an array";
_tag_dynforward(_tmp3ED,sizeof(char),_get_zero_arr_size(_tmp3ED,26));}),
_tag_dynforward(_tmp3EC,sizeof(void*),0));});goto _LL22E;_LL231:;_LL232: goto
_LL22E;_LL22E:;}{int ign_1=0;int ign_2=0;if(!Cyc_Tcutil_is_pointer_or_boxed(t1,&
ign_1,& ign_2))({void*_tmp3EE[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3EF="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dynforward(_tmp3EF,sizeof(char),_get_zero_arr_size(_tmp3EF,58));}),
_tag_dynforward(_tmp3EE,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp3F0[0]={};Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp3F1="swap non-lvalue";
_tag_dynforward(_tmp3F1,sizeof(char),_get_zero_arr_size(_tmp3F1,16));}),
_tag_dynforward(_tmp3F0,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e2))return({
void*_tmp3F2[0]={};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp3F3="swap non-lvalue";
_tag_dynforward(_tmp3F3,sizeof(char),_get_zero_arr_size(_tmp3F3,16));}),
_tag_dynforward(_tmp3F2,sizeof(void*),0));});{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;int b_ign2=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,&
b_ign1,& b_ign2,& t_ign2))return({void*_tmp3F4[0]={};Cyc_Tcexp_expr_err(te,e1->loc,
topt,({const char*_tmp3F5="swap value in zeroterm array";_tag_dynforward(_tmp3F5,
sizeof(char),_get_zero_arr_size(_tmp3F5,29));}),_tag_dynforward(_tmp3F4,sizeof(
void*),0));});if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& b_ign2,& t_ign2))
return({void*_tmp3F6[0]={};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp3F7="swap value in zeroterm array";_tag_dynforward(_tmp3F7,sizeof(char),
_get_zero_arr_size(_tmp3F7,29));}),_tag_dynforward(_tmp3F6,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(
t1,t2)){void*_tmp3F8=({struct Cyc_String_pa_struct _tmp3FC;_tmp3FC.tag=0;_tmp3FC.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp3FB;_tmp3FB.tag=0;_tmp3FB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp3F9[2]={& _tmp3FB,& _tmp3FC};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp3FA="type mismatch: %s != %s";_tag_dynforward(_tmp3FA,sizeof(char),
_get_zero_arr_size(_tmp3FA,24));}),_tag_dynforward(_tmp3F9,sizeof(void*),2));}}});
return _tmp3F8;}return(void*)0;}}}}static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*s){{struct
_RegionHandle _tmp3FD=_new_region("r");struct _RegionHandle*r=& _tmp3FD;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);;
_pop_region(r);}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp3FE=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Stmt*
_tmp400;struct Cyc_Absyn_Stmt*_tmp401;struct Cyc_Absyn_Decl*_tmp402;struct Cyc_Absyn_Stmt*
_tmp403;_LL234: if(_tmp3FE <= (void*)1)goto _LL23A;if(*((int*)_tmp3FE)!= 0)goto
_LL236;_tmp3FF=((struct Cyc_Absyn_Exp_s_struct*)_tmp3FE)->f1;_LL235: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3FF->topt))->v;_LL236: if(*((int*)_tmp3FE)!= 
1)goto _LL238;_tmp400=((struct Cyc_Absyn_Seq_s_struct*)_tmp3FE)->f1;_tmp401=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3FE)->f2;_LL237: s=_tmp401;continue;_LL238: if(*((
int*)_tmp3FE)!= 11)goto _LL23A;_tmp402=((struct Cyc_Absyn_Decl_s_struct*)_tmp3FE)->f1;
_tmp403=((struct Cyc_Absyn_Decl_s_struct*)_tmp3FE)->f2;_LL239: s=_tmp403;continue;
_LL23A:;_LL23B: return({void*_tmp404[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp405="statement expression must end with expression";_tag_dynforward(
_tmp405,sizeof(char),_get_zero_arr_size(_tmp405,46));}),_tag_dynforward(_tmp404,
sizeof(void*),0));});_LL233:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(rgn_handle != 
0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp40B=
_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp40C;_tmp40C.tag=15;_tmp40C.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp40D;}));_tmp40C;});_tmp40B;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp406=Cyc_Tcutil_compress(handle_type);void*_tmp407;_LL23D:
if(_tmp406 <= (void*)4)goto _LL23F;if(*((int*)_tmp406)!= 15)goto _LL23F;_tmp407=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp406)->f1;_LL23E: rgn=_tmp407;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL23C;_LL23F:;_LL240:({struct Cyc_String_pa_struct
_tmp40A;_tmp40A.tag=0;_tmp40A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp408[1]={& _tmp40A};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp409="expecting region_t type but found %s";
_tag_dynforward(_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,37));}),
_tag_dynforward(_tmp408,sizeof(void*),1));}});goto _LL23C;_LL23C:;}{void*_tmp40E=(
void*)e1->r;struct Cyc_Core_Opt*_tmp40F;struct Cyc_List_List*_tmp410;struct Cyc_List_List*
_tmp411;union Cyc_Absyn_Cnst_union _tmp412;struct _dynforward_ptr _tmp413;_LL242: if(*((
int*)_tmp40E)!= 29)goto _LL244;_LL243: {void*_tmp414=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp415=Cyc_Tcutil_compress(_tmp414);struct Cyc_Absyn_ArrayInfo
_tmp416;void*_tmp417;struct Cyc_Absyn_Tqual _tmp418;struct Cyc_Absyn_Exp*_tmp419;
struct Cyc_Absyn_Conref*_tmp41A;_LL24D: if(_tmp415 <= (void*)4)goto _LL24F;if(*((int*)
_tmp415)!= 7)goto _LL24F;_tmp416=((struct Cyc_Absyn_ArrayType_struct*)_tmp415)->f1;
_tmp417=(void*)_tmp416.elt_type;_tmp418=_tmp416.tq;_tmp419=_tmp416.num_elts;
_tmp41A=_tmp416.zero_term;_LL24E: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp419);void*b;{void*_tmp41B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bnd->topt))->v);void*_tmp41C;_LL252: if(_tmp41B <= (void*)4)goto _LL254;
if(*((int*)_tmp41B)!= 18)goto _LL254;_tmp41C=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp41B)->f1;_LL253: b=(void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp41D=
_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp41E;_tmp41E.tag=1;_tmp41E.f1=(void*)_tmp41C;_tmp41E;});_tmp41D;});goto _LL251;
_LL254:;_LL255: if(Cyc_Tcutil_is_const_exp(te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp420;_tmp420.tag=0;_tmp420.f1=bnd;_tmp420;});_tmp41F;});else{b=(void*)0;}
_LL251:;}{void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp422=
_cycalloc(sizeof(*_tmp422));_tmp422[0]=({struct Cyc_Absyn_PointerType_struct
_tmp423;_tmp423.tag=4;_tmp423.f1=({struct Cyc_Absyn_PtrInfo _tmp424;_tmp424.elt_typ=(
void*)_tmp417;_tmp424.elt_tq=_tmp418;_tmp424.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp425;_tmp425.rgn=(void*)rgn;_tmp425.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp425.bounds=Cyc_Absyn_new_conref(b);_tmp425.zero_term=
_tmp41A;_tmp425.ptrloc=0;_tmp425;});_tmp424;});_tmp423;});_tmp422;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->v=(
void*)res_typ;_tmp421;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL24F:;_LL250:({void*_tmp426[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp427="tcNew: comprehension returned non-array type";_tag_dynforward(_tmp427,
sizeof(char),_get_zero_arr_size(_tmp427,45));}),_tag_dynforward(_tmp426,sizeof(
void*),0));});_LL24C:;}_LL244: if(*((int*)_tmp40E)!= 37)goto _LL246;_tmp40F=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp40E)->f1;_tmp410=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp40E)->f2;_LL245:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428[0]=({struct Cyc_Absyn_Array_e_struct
_tmp429;_tmp429.tag=28;_tmp429.f1=_tmp410;_tmp429;});_tmp428;})));_tmp411=
_tmp410;goto _LL247;_LL246: if(*((int*)_tmp40E)!= 28)goto _LL248;_tmp411=((struct
Cyc_Absyn_Array_e_struct*)_tmp40E)->f1;_LL247: {void**elt_typ_opt=0;int zero_term=
0;if(topt != 0){void*_tmp42A=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp42B;void*_tmp42C;void**_tmp42D;struct Cyc_Absyn_Tqual _tmp42E;struct Cyc_Absyn_PtrAtts
_tmp42F;struct Cyc_Absyn_Conref*_tmp430;_LL257: if(_tmp42A <= (void*)4)goto _LL259;
if(*((int*)_tmp42A)!= 4)goto _LL259;_tmp42B=((struct Cyc_Absyn_PointerType_struct*)
_tmp42A)->f1;_tmp42C=(void*)_tmp42B.elt_typ;_tmp42D=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp42A)->f1).elt_typ;_tmp42E=_tmp42B.elt_tq;_tmp42F=_tmp42B.ptr_atts;_tmp430=
_tmp42F.zero_term;_LL258: elt_typ_opt=(void**)_tmp42D;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp430);goto _LL256;_LL259:;
_LL25A: goto _LL256;_LL256:;}{void*_tmp431=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp411);e1->topt=({struct Cyc_Core_Opt*_tmp432=_cycalloc(
sizeof(*_tmp432));_tmp432->v=(void*)_tmp431;_tmp432;});{void*res_typ;{void*
_tmp433=Cyc_Tcutil_compress(_tmp431);struct Cyc_Absyn_ArrayInfo _tmp434;void*
_tmp435;struct Cyc_Absyn_Tqual _tmp436;struct Cyc_Absyn_Exp*_tmp437;struct Cyc_Absyn_Conref*
_tmp438;_LL25C: if(_tmp433 <= (void*)4)goto _LL25E;if(*((int*)_tmp433)!= 7)goto
_LL25E;_tmp434=((struct Cyc_Absyn_ArrayType_struct*)_tmp433)->f1;_tmp435=(void*)
_tmp434.elt_type;_tmp436=_tmp434.tq;_tmp437=_tmp434.num_elts;_tmp438=_tmp434.zero_term;
_LL25D: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp439=_cycalloc(
sizeof(*_tmp439));_tmp439[0]=({struct Cyc_Absyn_PointerType_struct _tmp43A;_tmp43A.tag=
4;_tmp43A.f1=({struct Cyc_Absyn_PtrInfo _tmp43B;_tmp43B.elt_typ=(void*)_tmp435;
_tmp43B.elt_tq=_tmp436;_tmp43B.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp43C;_tmp43C.rgn=(
void*)rgn;_tmp43C.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp43C.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp43E;_tmp43E.tag=0;_tmp43E.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp437);
_tmp43E;});_tmp43D;}));_tmp43C.zero_term=_tmp438;_tmp43C.ptrloc=0;_tmp43C;});
_tmp43B;});_tmp43A;});_tmp439;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->v=(void*)res_typ;_tmp43F;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL25B;_LL25E:;_LL25F:({void*_tmp440[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp441="tcExpNoPromote on Array_e returned non-array type";
_tag_dynforward(_tmp441,sizeof(char),_get_zero_arr_size(_tmp441,50));}),
_tag_dynforward(_tmp440,sizeof(void*),0));});_LL25B:;}return res_typ;}}}_LL248:
if(*((int*)_tmp40E)!= 0)goto _LL24A;_tmp412=((struct Cyc_Absyn_Const_e_struct*)
_tmp40E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp40E)->f1).String_c).tag
!= 5)goto _LL24A;_tmp413=(_tmp412.String_c).f1;_LL249: {void*_tmp442=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp443=Cyc_Tcexp_tcExp(te,(void**)& _tmp442,e1);return Cyc_Absyn_atb_typ(
_tmp443,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp445;_tmp445.tag=0;_tmp445.f1=Cyc_Absyn_uint_exp(1,0);_tmp445;});_tmp444;}),
Cyc_Absyn_false_conref);}_LL24A:;_LL24B: {void**topt2=0;if(topt != 0){void*
_tmp446=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp447;void*_tmp448;
void**_tmp449;struct Cyc_Absyn_Tqual _tmp44A;_LL261: if(_tmp446 <= (void*)4)goto
_LL265;if(*((int*)_tmp446)!= 4)goto _LL263;_tmp447=((struct Cyc_Absyn_PointerType_struct*)
_tmp446)->f1;_tmp448=(void*)_tmp447.elt_typ;_tmp449=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp446)->f1).elt_typ;_tmp44A=_tmp447.elt_tq;_LL262: topt2=(void**)_tmp449;goto
_LL260;_LL263: if(*((int*)_tmp446)!= 2)goto _LL265;_LL264: topt2=({void**_tmp44B=
_cycalloc(sizeof(*_tmp44B));_tmp44B[0]=*topt;_tmp44B;});goto _LL260;_LL265:;
_LL266: goto _LL260;_LL260:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp44D=_cycalloc(sizeof(*_tmp44D));
_tmp44D[0]=({struct Cyc_Absyn_PointerType_struct _tmp44E;_tmp44E.tag=4;_tmp44E.f1=({
struct Cyc_Absyn_PtrInfo _tmp44F;_tmp44F.elt_typ=(void*)telt;_tmp44F.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp44F.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp450;_tmp450.rgn=(void*)rgn;
_tmp450.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp450.bounds=Cyc_Absyn_bounds_one_conref;_tmp450.zero_term=Cyc_Absyn_false_conref;
_tmp450.ptrloc=0;_tmp450;});_tmp44F;});_tmp44E;});_tmp44D;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->v=(
void*)res_typ;_tmp44C;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL241:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp451=t;struct Cyc_Absyn_ArrayInfo _tmp452;void*_tmp453;struct
Cyc_Absyn_Tqual _tmp454;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Conref*
_tmp456;_LL268: if(_tmp451 <= (void*)4)goto _LL26A;if(*((int*)_tmp451)!= 7)goto
_LL26A;_tmp452=((struct Cyc_Absyn_ArrayType_struct*)_tmp451)->f1;_tmp453=(void*)
_tmp452.elt_type;_tmp454=_tmp452.tq;_tmp455=_tmp452.num_elts;_tmp456=_tmp452.zero_term;
_LL269: {void*_tmp458;struct _tuple6 _tmp457=Cyc_Tcutil_addressof_props(te,e);
_tmp458=_tmp457.f2;{void*_tmp459=_tmp455 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp45B;_tmp45B.tag=0;_tmp45B.f1=(struct Cyc_Absyn_Exp*)
_tmp455;_tmp45B;});_tmp45A;});t=Cyc_Absyn_atb_typ(_tmp453,_tmp458,_tmp454,
_tmp459,_tmp456);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL26A:;_LL26B: return t;_LL267:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);if(Cyc_Tcutil_is_unique_pointer(t) && !Cyc_Tcutil_is_unique_path(e)){({
struct Cyc_String_pa_struct _tmp460;_tmp460.tag=0;_tmp460.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{struct Cyc_String_pa_struct
_tmp45F;_tmp45F.tag=0;_tmp45F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
struct Cyc_String_pa_struct _tmp45E;_tmp45E.tag=0;_tmp45E.f1=(struct
_dynforward_ptr)(topt == 0?({const char*_tmp461="NULL";_tag_dynforward(_tmp461,
sizeof(char),_get_zero_arr_size(_tmp461,5));}):(struct _dynforward_ptr)Cyc_Absynpp_typ2string(*
topt));{void*_tmp45C[3]={& _tmp45E,& _tmp45F,& _tmp460};Cyc_fprintf(Cyc_stderr,({
const char*_tmp45D="topt=%s, e->topt->v=%s, e=%s\n";_tag_dynforward(_tmp45D,
sizeof(char),_get_zero_arr_size(_tmp45D,30));}),_tag_dynforward(_tmp45C,sizeof(
void*),3));}}}});({struct Cyc_Int_pa_struct _tmp464;_tmp464.tag=1;_tmp464.f1=(
unsigned long)2117;{void*_tmp462[1]={& _tmp464};Cyc_Tcutil_terr(e->loc,({const
char*_tmp463="Cannot consume paths; do swap instead %d";_tag_dynforward(_tmp463,
sizeof(char),_get_zero_arr_size(_tmp463,41));}),_tag_dynforward(_tmp462,sizeof(
void*),1));}});}{void*_tmp465=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp466;
_LL26D: if(*((int*)_tmp465)!= 28)goto _LL26F;_LL26E: goto _LL270;_LL26F: if(*((int*)
_tmp465)!= 29)goto _LL271;_LL270: goto _LL272;_LL271: if(*((int*)_tmp465)!= 0)goto
_LL273;_tmp466=((struct Cyc_Absyn_Const_e_struct*)_tmp465)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp465)->f1).String_c).tag != 5)goto _LL273;_LL272: return t;_LL273:;_LL274: t=Cyc_Tcutil_compress(
t);{void*_tmp467=t;struct Cyc_Absyn_ArrayInfo _tmp468;void*_tmp469;struct Cyc_Absyn_Tqual
_tmp46A;struct Cyc_Absyn_Exp*_tmp46B;struct Cyc_Absyn_Conref*_tmp46C;_LL276: if(
_tmp467 <= (void*)4)goto _LL278;if(*((int*)_tmp467)!= 7)goto _LL278;_tmp468=((
struct Cyc_Absyn_ArrayType_struct*)_tmp467)->f1;_tmp469=(void*)_tmp468.elt_type;
_tmp46A=_tmp468.tq;_tmp46B=_tmp468.num_elts;_tmp46C=_tmp468.zero_term;_LL277: {
void*_tmp46E;struct _tuple6 _tmp46D=Cyc_Tcutil_addressof_props(te,e);_tmp46E=
_tmp46D.f2;{void*b=_tmp46B == 0?(void*)((void*)0):(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp470;_tmp470.tag=0;_tmp470.f1=(struct Cyc_Absyn_Exp*)_tmp46B;_tmp470;});
_tmp46F;});t=Cyc_Absyn_atb_typ(_tmp469,_tmp46E,_tmp46A,b,_tmp46C);Cyc_Tcutil_unchecked_cast(
te,e,t,(void*)3);return t;}}_LL278:;_LL279: return t;_LL275:;}_LL26C:;}}static void*
Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){{void*_tmp471=(void*)e->r;struct Cyc_Absyn_Exp*_tmp472;_LL27B: if(*((int*)
_tmp471)!= 13)goto _LL27D;_tmp472=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp471)->f1;_LL27C: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp472);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp472->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp472->topt))->v,0));e->topt=_tmp472->topt;goto
_LL27A;_LL27D:;_LL27E: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp473=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp474;void*
_tmp475;struct Cyc_Absyn_Tqual _tmp476;struct Cyc_Absyn_PtrAtts _tmp477;void*_tmp478;
struct Cyc_Absyn_Conref*_tmp479;struct Cyc_Absyn_Conref*_tmp47A;struct Cyc_Absyn_Conref*
_tmp47B;_LL280: if(_tmp473 <= (void*)4)goto _LL282;if(*((int*)_tmp473)!= 4)goto
_LL282;_tmp474=((struct Cyc_Absyn_PointerType_struct*)_tmp473)->f1;_tmp475=(void*)
_tmp474.elt_typ;_tmp476=_tmp474.elt_tq;_tmp477=_tmp474.ptr_atts;_tmp478=(void*)
_tmp477.rgn;_tmp479=_tmp477.nullable;_tmp47A=_tmp477.bounds;_tmp47B=_tmp477.zero_term;
_LL281:{void*_tmp47C=Cyc_Tcutil_compress(_tmp475);struct Cyc_Absyn_FnInfo _tmp47D;
struct Cyc_List_List*_tmp47E;struct Cyc_Core_Opt*_tmp47F;void*_tmp480;struct Cyc_List_List*
_tmp481;int _tmp482;struct Cyc_Absyn_VarargInfo*_tmp483;struct Cyc_List_List*
_tmp484;struct Cyc_List_List*_tmp485;_LL285: if(_tmp47C <= (void*)4)goto _LL287;if(*((
int*)_tmp47C)!= 8)goto _LL287;_tmp47D=((struct Cyc_Absyn_FnType_struct*)_tmp47C)->f1;
_tmp47E=_tmp47D.tvars;_tmp47F=_tmp47D.effect;_tmp480=(void*)_tmp47D.ret_typ;
_tmp481=_tmp47D.args;_tmp482=_tmp47D.c_varargs;_tmp483=_tmp47D.cyc_varargs;
_tmp484=_tmp47D.rgn_po;_tmp485=_tmp47D.attributes;_LL286: if(_tmp47E != 0){struct
_RegionHandle _tmp486=_new_region("rgn");struct _RegionHandle*rgn=& _tmp486;
_push_region(rgn);{struct _tuple4 _tmp487=({struct _tuple4 _tmp494;_tmp494.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp494.f2=rgn;_tmp494;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp487,_tmp47E);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491[0]=({struct Cyc_Absyn_FnType_struct
_tmp492;_tmp492.tag=8;_tmp492.f1=({struct Cyc_Absyn_FnInfo _tmp493;_tmp493.tvars=0;
_tmp493.effect=_tmp47F;_tmp493.ret_typ=(void*)_tmp480;_tmp493.args=_tmp481;
_tmp493.c_varargs=_tmp482;_tmp493.cyc_varargs=_tmp483;_tmp493.rgn_po=_tmp484;
_tmp493.attributes=_tmp485;_tmp493;});_tmp492;});_tmp491;}));struct Cyc_Absyn_PointerType_struct*
_tmp488=({struct Cyc_Absyn_PointerType_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));
_tmp48D[0]=({struct Cyc_Absyn_PointerType_struct _tmp48E;_tmp48E.tag=4;_tmp48E.f1=({
struct Cyc_Absyn_PtrInfo _tmp48F;_tmp48F.elt_typ=(void*)ftyp;_tmp48F.elt_tq=
_tmp476;_tmp48F.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp490;_tmp490.rgn=(void*)
_tmp478;_tmp490.nullable=_tmp479;_tmp490.bounds=_tmp47A;_tmp490.zero_term=
_tmp47B;_tmp490.ptrloc=0;_tmp490;});_tmp48F;});_tmp48E;});_tmp48D;});struct Cyc_Absyn_Exp*
_tmp489=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp48B;_tmp48B.tag=14;_tmp48B.f1=_tmp489;_tmp48B.f2=ts;_tmp48B;});_tmp48A;})));
e->topt=({struct Cyc_Core_Opt*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->v=(
void*)((void*)_tmp488);_tmp48C;});};_pop_region(rgn);}goto _LL284;_LL287:;_LL288:
goto _LL284;_LL284:;}goto _LL27F;_LL282:;_LL283: goto _LL27F;_LL27F:;}goto _LL27A;
_LL27A:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp495=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp496;struct _tuple1*_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_List_List*
_tmp499;struct Cyc_Core_Opt*_tmp49A;struct Cyc_List_List*_tmp49B;union Cyc_Absyn_Cnst_union
_tmp49C;union Cyc_Absyn_Cnst_union*_tmp49D;struct _tuple1*_tmp49E;void*_tmp49F;
void*_tmp4A0;struct Cyc_List_List*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;void*
_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Core_Opt*_tmp4A5;struct Cyc_Absyn_Exp*
_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*
_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*
_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*
_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_List_List*_tmp4B1;struct Cyc_Absyn_VarargCallInfo*
_tmp4B2;struct Cyc_Absyn_VarargCallInfo**_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;
struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_List_List*_tmp4B6;void*_tmp4B7;struct Cyc_Absyn_Exp*
_tmp4B8;void*_tmp4B9;void**_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*
_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;void*_tmp4BF;
void*_tmp4C0;void*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*
_tmp4C3;struct _dynforward_ptr*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;struct
_dynforward_ptr*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;
struct Cyc_List_List*_tmp4C9;struct _tuple2*_tmp4CA;struct Cyc_List_List*_tmp4CB;
struct Cyc_List_List*_tmp4CC;struct Cyc_Absyn_Stmt*_tmp4CD;struct Cyc_Absyn_Vardecl*
_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D0;int _tmp4D1;int*
_tmp4D2;struct _tuple1*_tmp4D3;struct _tuple1**_tmp4D4;struct Cyc_List_List*_tmp4D5;
struct Cyc_List_List**_tmp4D6;struct Cyc_List_List*_tmp4D7;struct Cyc_Absyn_Aggrdecl*
_tmp4D8;struct Cyc_Absyn_Aggrdecl**_tmp4D9;void*_tmp4DA;struct Cyc_List_List*
_tmp4DB;struct Cyc_List_List*_tmp4DC;struct Cyc_Absyn_Tuniondecl*_tmp4DD;struct Cyc_Absyn_Tunionfield*
_tmp4DE;struct _tuple1*_tmp4DF;struct _tuple1**_tmp4E0;struct Cyc_Absyn_Enumdecl*
_tmp4E1;struct Cyc_Absyn_Enumfield*_tmp4E2;struct _tuple1*_tmp4E3;struct _tuple1**
_tmp4E4;void*_tmp4E5;struct Cyc_Absyn_Enumfield*_tmp4E6;struct Cyc_Absyn_MallocInfo
_tmp4E7;int _tmp4E8;int*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EA;void**_tmp4EB;void***
_tmp4EC;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp**_tmp4EE;int _tmp4EF;int*
_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F2;_LL28A: if(*((int*)
_tmp495)!= 13)goto _LL28C;_tmp496=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp495)->f1;_LL28B: Cyc_Tcexp_tcExpNoInst(te,0,_tmp496);return;_LL28C: if(*((int*)
_tmp495)!= 2)goto _LL28E;_tmp497=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp495)->f1;
_LL28D: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp497);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL28E: if(*((int*)_tmp495)!= 10)goto _LL290;_tmp498=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp495)->f1;_tmp499=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp495)->f2;_LL28F:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp498,_tmp499);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL290: if(*((int*)_tmp495)!= 37)goto _LL292;_tmp49A=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp495)->f1;_tmp49B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp495)->f2;
_LL291: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp49B);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL292: if(*((int*)_tmp495)!= 0)goto _LL294;_tmp49C=((struct Cyc_Absyn_Const_e_struct*)
_tmp495)->f1;_tmp49D=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp495)->f1;_LL293: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp49D,e);goto _LL289;_LL294: if(*((int*)_tmp495)!= 1)goto _LL296;_tmp49E=((struct
Cyc_Absyn_Var_e_struct*)_tmp495)->f1;_tmp49F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp495)->f2;_LL295: t=Cyc_Tcexp_tcVar(te,loc,_tmp49E,_tmp49F);goto _LL289;_LL296:
if(*((int*)_tmp495)!= 3)goto _LL298;_tmp4A0=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp495)->f1;_tmp4A1=((struct Cyc_Absyn_Primop_e_struct*)_tmp495)->f2;_LL297: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4A0,_tmp4A1);goto _LL289;_LL298: if(*((int*)
_tmp495)!= 5)goto _LL29A;_tmp4A2=((struct Cyc_Absyn_Increment_e_struct*)_tmp495)->f1;
_tmp4A3=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp495)->f2;_LL299: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp4A2,_tmp4A3);goto _LL289;_LL29A: if(*((int*)_tmp495)!= 4)goto
_LL29C;_tmp4A4=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp495)->f1;_tmp4A5=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp495)->f2;_tmp4A6=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp495)->f3;_LL29B: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4A4,_tmp4A5,_tmp4A6);
goto _LL289;_LL29C: if(*((int*)_tmp495)!= 6)goto _LL29E;_tmp4A7=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp495)->f1;_tmp4A8=((struct Cyc_Absyn_Conditional_e_struct*)_tmp495)->f2;
_tmp4A9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp495)->f3;_LL29D: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp4A7,_tmp4A8,_tmp4A9);goto _LL289;_LL29E: if(*((int*)_tmp495)!= 7)
goto _LL2A0;_tmp4AA=((struct Cyc_Absyn_And_e_struct*)_tmp495)->f1;_tmp4AB=((struct
Cyc_Absyn_And_e_struct*)_tmp495)->f2;_LL29F: t=Cyc_Tcexp_tcAnd(te,loc,_tmp4AA,
_tmp4AB);goto _LL289;_LL2A0: if(*((int*)_tmp495)!= 8)goto _LL2A2;_tmp4AC=((struct
Cyc_Absyn_Or_e_struct*)_tmp495)->f1;_tmp4AD=((struct Cyc_Absyn_Or_e_struct*)
_tmp495)->f2;_LL2A1: t=Cyc_Tcexp_tcOr(te,loc,_tmp4AC,_tmp4AD);goto _LL289;_LL2A2:
if(*((int*)_tmp495)!= 9)goto _LL2A4;_tmp4AE=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp495)->f1;_tmp4AF=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp495)->f2;_LL2A3: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4AE,_tmp4AF);goto _LL289;_LL2A4: if(*((int*)
_tmp495)!= 11)goto _LL2A6;_tmp4B0=((struct Cyc_Absyn_FnCall_e_struct*)_tmp495)->f1;
_tmp4B1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp495)->f2;_tmp4B2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp495)->f3;_tmp4B3=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp495)->f3;_LL2A5: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4B0,_tmp4B1,_tmp4B3);
goto _LL289;_LL2A6: if(*((int*)_tmp495)!= 12)goto _LL2A8;_tmp4B4=((struct Cyc_Absyn_Throw_e_struct*)
_tmp495)->f1;_LL2A7: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4B4);goto _LL289;_LL2A8:
if(*((int*)_tmp495)!= 14)goto _LL2AA;_tmp4B5=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp495)->f1;_tmp4B6=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp495)->f2;_LL2A9:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4B5,_tmp4B6);goto _LL289;_LL2AA: if(*((
int*)_tmp495)!= 15)goto _LL2AC;_tmp4B7=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp495)->f1;_tmp4B8=((struct Cyc_Absyn_Cast_e_struct*)_tmp495)->f2;_tmp4B9=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp495)->f4;_tmp4BA=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp495)->f4);_LL2AB: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4B7,_tmp4B8,(void**)
_tmp4BA);goto _LL289;_LL2AC: if(*((int*)_tmp495)!= 16)goto _LL2AE;_tmp4BB=((struct
Cyc_Absyn_Address_e_struct*)_tmp495)->f1;_LL2AD: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4BB);goto _LL289;_LL2AE: if(*((int*)_tmp495)!= 17)goto _LL2B0;_tmp4BC=((
struct Cyc_Absyn_New_e_struct*)_tmp495)->f1;_tmp4BD=((struct Cyc_Absyn_New_e_struct*)
_tmp495)->f2;_LL2AF: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4BC,e,_tmp4BD);goto _LL289;
_LL2B0: if(*((int*)_tmp495)!= 19)goto _LL2B2;_tmp4BE=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp495)->f1;_LL2B1: {void*_tmp4F3=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4BE);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4F3);goto _LL289;}_LL2B2: if(*((int*)_tmp495)!= 18)goto _LL2B4;
_tmp4BF=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp495)->f1;_LL2B3: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4BF);goto _LL289;_LL2B4: if(*((int*)_tmp495)!= 20)goto _LL2B6;
_tmp4C0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp495)->f1;_tmp4C1=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp495)->f2;_LL2B5: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4C0,_tmp4C1);goto _LL289;_LL2B6: if(*((int*)_tmp495)!= 21)goto _LL2B8;
_LL2B7:({void*_tmp4F4[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4F5="gen() not in top-level initializer";
_tag_dynforward(_tmp4F5,sizeof(char),_get_zero_arr_size(_tmp4F5,35));}),
_tag_dynforward(_tmp4F4,sizeof(void*),0));});return;_LL2B8: if(*((int*)_tmp495)!= 
22)goto _LL2BA;_tmp4C2=((struct Cyc_Absyn_Deref_e_struct*)_tmp495)->f1;_LL2B9: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4C2);goto _LL289;_LL2BA: if(*((int*)_tmp495)!= 
23)goto _LL2BC;_tmp4C3=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp495)->f1;
_tmp4C4=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp495)->f2;_LL2BB: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp4C3,_tmp4C4);goto _LL289;_LL2BC: if(*((int*)_tmp495)!= 24)goto
_LL2BE;_tmp4C5=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp495)->f1;_tmp4C6=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp495)->f2;_LL2BD: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp4C5,_tmp4C6);goto _LL289;_LL2BE: if(*((int*)_tmp495)!= 25)goto
_LL2C0;_tmp4C7=((struct Cyc_Absyn_Subscript_e_struct*)_tmp495)->f1;_tmp4C8=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp495)->f2;_LL2BF: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4C7,_tmp4C8);goto _LL289;_LL2C0: if(*((int*)_tmp495)!= 26)goto
_LL2C2;_tmp4C9=((struct Cyc_Absyn_Tuple_e_struct*)_tmp495)->f1;_LL2C1: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4C9);goto _LL289;_LL2C2: if(*((int*)_tmp495)!= 27)goto _LL2C4;
_tmp4CA=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp495)->f1;_tmp4CB=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp495)->f2;_LL2C3: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4CA,_tmp4CB);goto
_LL289;_LL2C4: if(*((int*)_tmp495)!= 28)goto _LL2C6;_tmp4CC=((struct Cyc_Absyn_Array_e_struct*)
_tmp495)->f1;_LL2C5: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4F6=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4F7;void*_tmp4F8;void**
_tmp4F9;struct Cyc_Absyn_Conref*_tmp4FA;_LL2D9: if(_tmp4F6 <= (void*)4)goto _LL2DB;
if(*((int*)_tmp4F6)!= 7)goto _LL2DB;_tmp4F7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4F6)->f1;_tmp4F8=(void*)_tmp4F7.elt_type;_tmp4F9=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4F6)->f1).elt_type;_tmp4FA=_tmp4F7.zero_term;_LL2DA: elt_topt=(void**)_tmp4F9;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4FA);
goto _LL2D8;_LL2DB:;_LL2DC: goto _LL2D8;_LL2D8:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4CC);goto _LL289;}_LL2C6: if(*((int*)_tmp495)!= 38)goto
_LL2C8;_tmp4CD=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp495)->f1;_LL2C7: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4CD);goto _LL289;_LL2C8: if(*((int*)_tmp495)!= 29)goto _LL2CA;
_tmp4CE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp495)->f1;_tmp4CF=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp495)->f2;_tmp4D0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp495)->f3;_tmp4D1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp495)->f4;
_tmp4D2=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp495)->f4;_LL2C9: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4CE,_tmp4CF,_tmp4D0,_tmp4D2);goto
_LL289;_LL2CA: if(*((int*)_tmp495)!= 30)goto _LL2CC;_tmp4D3=((struct Cyc_Absyn_Struct_e_struct*)
_tmp495)->f1;_tmp4D4=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp495)->f1;_tmp4D5=((struct Cyc_Absyn_Struct_e_struct*)_tmp495)->f2;_tmp4D6=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp495)->f2;_tmp4D7=((
struct Cyc_Absyn_Struct_e_struct*)_tmp495)->f3;_tmp4D8=((struct Cyc_Absyn_Struct_e_struct*)
_tmp495)->f4;_tmp4D9=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp495)->f4;_LL2CB: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4D4,_tmp4D6,_tmp4D7,
_tmp4D9);goto _LL289;_LL2CC: if(*((int*)_tmp495)!= 31)goto _LL2CE;_tmp4DA=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp495)->f1;_tmp4DB=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp495)->f2;_LL2CD: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4DA,_tmp4DB);goto _LL289;
_LL2CE: if(*((int*)_tmp495)!= 32)goto _LL2D0;_tmp4DC=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp495)->f1;_tmp4DD=((struct Cyc_Absyn_Tunion_e_struct*)_tmp495)->f2;_tmp4DE=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp495)->f3;_LL2CF: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4DC,_tmp4DD,_tmp4DE);goto _LL289;_LL2D0: if(*((int*)_tmp495)!= 33)goto
_LL2D2;_tmp4DF=((struct Cyc_Absyn_Enum_e_struct*)_tmp495)->f1;_tmp4E0=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp495)->f1;_tmp4E1=((struct Cyc_Absyn_Enum_e_struct*)
_tmp495)->f2;_tmp4E2=((struct Cyc_Absyn_Enum_e_struct*)_tmp495)->f3;_LL2D1:*
_tmp4E0=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4E2))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4FC;_tmp4FC.tag=12;_tmp4FC.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4E1))->name;_tmp4FC.f2=_tmp4E1;_tmp4FC;});_tmp4FB;});goto _LL289;
_LL2D2: if(*((int*)_tmp495)!= 34)goto _LL2D4;_tmp4E3=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp495)->f1;_tmp4E4=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp495)->f1;_tmp4E5=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp495)->f2;
_tmp4E6=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp495)->f3;_LL2D3:*_tmp4E4=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4E6))->name;t=_tmp4E5;goto _LL289;
_LL2D4: if(*((int*)_tmp495)!= 35)goto _LL2D6;_tmp4E7=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp495)->f1;_tmp4E8=_tmp4E7.is_calloc;_tmp4E9=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp495)->f1).is_calloc;_tmp4EA=_tmp4E7.rgn;_tmp4EB=_tmp4E7.elt_type;_tmp4EC=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp495)->f1).elt_type;_tmp4ED=
_tmp4E7.num_elts;_tmp4EE=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp495)->f1).num_elts;_tmp4EF=_tmp4E7.fat_result;_tmp4F0=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp495)->f1).fat_result;_LL2D5: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4EA,_tmp4EC,
_tmp4EE,_tmp4E9,_tmp4F0);goto _LL289;_LL2D6: if(*((int*)_tmp495)!= 36)goto _LL289;
_tmp4F1=((struct Cyc_Absyn_Swap_e_struct*)_tmp495)->f1;_tmp4F2=((struct Cyc_Absyn_Swap_e_struct*)
_tmp495)->f2;_LL2D7: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4F1,_tmp4F2);goto _LL289;
_LL289:;}e->topt=({struct Cyc_Core_Opt*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));
_tmp4FD->v=(void*)t;_tmp4FD;});}
