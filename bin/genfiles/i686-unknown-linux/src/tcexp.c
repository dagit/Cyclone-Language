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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(
struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};extern char Cyc_Core_Open_Region[16];extern char
Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*
f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
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
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
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
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
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
struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union c,struct Cyc_Absyn_Exp*
e){void*t;{union Cyc_Absyn_Cnst_union _tmp57=c;void*_tmp58;void*_tmp59;void*_tmp5A;
void*_tmp5B;void*_tmp5C;void*_tmp5D;int _tmp5E;struct _dynforward_ptr _tmp5F;_LL34:
if((_tmp57.Char_c).tag != 0)goto _LL36;_tmp58=(_tmp57.Char_c).f1;if((int)_tmp58 != 
0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;goto _LL33;_LL36: if((_tmp57.Char_c).tag != 
0)goto _LL38;_tmp59=(_tmp57.Char_c).f1;if((int)_tmp59 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;
goto _LL33;_LL38: if((_tmp57.Char_c).tag != 0)goto _LL3A;_tmp5A=(_tmp57.Char_c).f1;
if((int)_tmp5A != 2)goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;_LL3A: if((
_tmp57.Short_c).tag != 1)goto _LL3C;_tmp5B=(_tmp57.Short_c).f1;_LL3B: t=_tmp5B == (
void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if((_tmp57.LongLong_c).tag
!= 3)goto _LL3E;_tmp5C=(_tmp57.LongLong_c).f1;_LL3D: t=_tmp5C == (void*)1?Cyc_Absyn_ulonglong_typ:
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
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=({struct Cyc_Absyn_Const_e_struct
_tmp6C;_tmp6C.tag=0;_tmp6C.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6D;(_tmp6D.Char_c).tag=0;(_tmp6D.Char_c).f1=(void*)_tmp61;(_tmp6D.Char_c).f2=(
char)_tmp5E;_tmp6D;});_tmp6C;});_tmp6B;})));goto _LL46;_LL49: if(*((int*)_tmp60)!= 
5)goto _LL4B;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp64=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f2;if((int)_tmp64 != 1)goto _LL4B;
_LL4A: t=_tmp63 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));
_tmp6E[0]=({struct Cyc_Absyn_Const_e_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(union
Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp70;(_tmp70.Short_c).tag=1;(
_tmp70.Short_c).f1=(void*)_tmp63;(_tmp70.Short_c).f2=(short)_tmp5E;_tmp70;});
_tmp6F;});_tmp6E;})));goto _LL46;_LL4B: if(*((int*)_tmp60)!= 5)goto _LL4D;_tmp65=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp66=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f2;if((int)_tmp66 != 2)goto _LL4D;_LL4C: t=_tmp65 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(*((int*)_tmp60)!= 5)goto _LL4F;_tmp67=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp60)->f1;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp60)->f2;if((int)_tmp68 != 3)goto _LL4F;_LL4E: t=_tmp67 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4F: if(*((int*)_tmp60)!= 4)goto _LL51;if(!(_tmp5E
== 0))goto _LL51;_LL50:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_Const_e_struct
_tmp72;_tmp72.tag=0;_tmp72.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp73;(_tmp73.Null_c).tag=6;_tmp73;});_tmp72;});_tmp71;})));{struct Cyc_List_List*
_tmp74=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({struct Cyc_Absyn_PointerType_struct
_tmp76;_tmp76.tag=4;_tmp76.f1=({struct Cyc_Absyn_PtrInfo _tmp77;_tmp77.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->v=_tmp74;_tmp7A;}));_tmp77.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp77.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp78;_tmp78.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp79=_cycalloc(
sizeof(*_tmp79));_tmp79->v=_tmp74;_tmp79;}));_tmp78.nullable=Cyc_Absyn_true_conref;
_tmp78.bounds=Cyc_Absyn_empty_conref();_tmp78.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp78.ptrloc=0;_tmp78;});_tmp77;});_tmp76;});_tmp75;});
goto _LL46;}_LL51: if(*((int*)_tmp60)!= 18)goto _LL53;_tmp69=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp60)->f1;_LL52: {struct Cyc_Absyn_TypeInt_struct*_tmp7B=({struct Cyc_Absyn_TypeInt_struct*
_tmp82=_cycalloc_atomic(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp83;_tmp83.tag=19;_tmp83.f1=_tmp5E;_tmp83;});_tmp82;});if(!Cyc_Tcutil_unify(
_tmp69,(void*)_tmp7B)){({struct Cyc_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)
_tmp7B));{struct Cyc_String_pa_struct _tmp7E;_tmp7E.tag=0;_tmp7E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp69));{void*
_tmp7C[2]={& _tmp7E,& _tmp7F};Cyc_Tcutil_terr(loc,({const char*_tmp7D="expecting %s but found %s";
_tag_dynforward(_tmp7D,sizeof(char),_get_zero_arr_size(_tmp7D,26));}),
_tag_dynforward(_tmp7C,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}t=(
void*)({struct Cyc_Absyn_TagType_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[
0]=({struct Cyc_Absyn_TagType_struct _tmp81;_tmp81.tag=18;_tmp81.f1=(void*)((void*)
_tmp7B);_tmp81;});_tmp80;});goto _LL46;}_LL53:;_LL54: t=_tmp5D == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}goto _LL33;_LL42: if((_tmp57.String_c).tag != 
5)goto _LL44;_tmp5F=(_tmp57.String_c).f1;_LL43: {int len=(int)_get_dynforward_size(
_tmp5F,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp8F;(_tmp8F.Int_c).tag=2;(_tmp8F.Int_c).f1=(void*)((
void*)1);(_tmp8F.Int_c).f2=len;_tmp8F;}),loc);elen->topt=({struct Cyc_Core_Opt*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->v=(void*)Cyc_Absyn_uint_typ;_tmp84;});
t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp86;_tmp86.tag=0;_tmp86.f1=elen;_tmp86;});
_tmp85;}),Cyc_Absyn_true_conref);if(topt != 0){void*_tmp87=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_ArrayInfo _tmp88;struct Cyc_Absyn_Tqual _tmp89;_LL5D: if(
_tmp87 <= (void*)4)goto _LL61;if(*((int*)_tmp87)!= 7)goto _LL5F;_tmp88=((struct Cyc_Absyn_ArrayType_struct*)
_tmp87)->f1;_tmp89=_tmp88.tq;_LL5E: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
_tmp89,(struct Cyc_Absyn_Exp*)elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),
0);_LL5F: if(*((int*)_tmp87)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t)
 && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->v=(void*)t;_tmp8A;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp8B=_cycalloc(sizeof(*
_tmp8B));_tmp8B->v=Cyc_Tcenv_lookup_type_vars(te);_tmp8B;})),Cyc_Absyn_const_tqual(
0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));
_tmp8C[0]=({struct Cyc_Absyn_Upper_b_struct _tmp8D;_tmp8D.tag=0;_tmp8D.f1=elen;
_tmp8D;});_tmp8C;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp8E=_cycalloc(sizeof(*_tmp8E));
_tmp8E->v=(void*)t;_tmp8E;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}_LL44: if((_tmp57.Null_c).tag
!= 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp90=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91[0]=({struct Cyc_Absyn_PointerType_struct _tmp92;_tmp92.tag=4;_tmp92.f1=({
struct Cyc_Absyn_PtrInfo _tmp93;_tmp93.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp96=_cycalloc(sizeof(*
_tmp96));_tmp96->v=_tmp90;_tmp96;}));_tmp93.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp93.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp94;_tmp94.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp95=_cycalloc(
sizeof(*_tmp95));_tmp95->v=_tmp90;_tmp95;}));_tmp94.nullable=Cyc_Absyn_true_conref;
_tmp94.bounds=Cyc_Absyn_empty_conref();_tmp94.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp94.ptrloc=0;_tmp94;});_tmp93;});_tmp92;});_tmp91;});
goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp97=b;struct Cyc_Absyn_Vardecl*
_tmp98;struct Cyc_Absyn_Fndecl*_tmp99;struct Cyc_Absyn_Vardecl*_tmp9A;struct Cyc_Absyn_Vardecl*
_tmp9B;struct Cyc_Absyn_Vardecl*_tmp9C;_LL64: if((int)_tmp97 != 0)goto _LL66;_LL65:
return({struct Cyc_String_pa_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmp9D[
1]={& _tmp9F};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp9E="undeclared identifier: %s";
_tag_dynforward(_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,26));}),
_tag_dynforward(_tmp9D,sizeof(void*),1));}});_LL66: if(_tmp97 <= (void*)1)goto
_LL68;if(*((int*)_tmp97)!= 0)goto _LL68;_tmp98=((struct Cyc_Absyn_Global_b_struct*)
_tmp97)->f1;_LL67:*q=*_tmp98->name;return(void*)_tmp98->type;_LL68: if(_tmp97 <= (
void*)1)goto _LL6A;if(*((int*)_tmp97)!= 1)goto _LL6A;_tmp99=((struct Cyc_Absyn_Funname_b_struct*)
_tmp97)->f1;_LL69:*q=*_tmp99->name;return Cyc_Tcutil_fndecl2typ(_tmp99);_LL6A: if(
_tmp97 <= (void*)1)goto _LL6C;if(*((int*)_tmp97)!= 4)goto _LL6C;_tmp9A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp97)->f1;_LL6B: _tmp9B=_tmp9A;goto _LL6D;_LL6C: if(_tmp97 <= (void*)1)goto _LL6E;
if(*((int*)_tmp97)!= 3)goto _LL6E;_tmp9B=((struct Cyc_Absyn_Local_b_struct*)_tmp97)->f1;
_LL6D: _tmp9C=_tmp9B;goto _LL6F;_LL6E: if(_tmp97 <= (void*)1)goto _LL63;if(*((int*)
_tmp97)!= 2)goto _LL63;_tmp9C=((struct Cyc_Absyn_Param_b_struct*)_tmp97)->f1;_LL6F:(*
q).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmpA0;(_tmpA0.Loc_n).tag=
0;_tmpA0;});return(void*)_tmp9C->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpA1=(void*)
fmt->r;union Cyc_Absyn_Cnst_union _tmpA2;struct _dynforward_ptr _tmpA3;struct Cyc_Absyn_Exp*
_tmpA4;struct Cyc_Absyn_Exp _tmpA5;void*_tmpA6;union Cyc_Absyn_Cnst_union _tmpA7;
struct _dynforward_ptr _tmpA8;_LL71: if(*((int*)_tmpA1)!= 0)goto _LL73;_tmpA2=((
struct Cyc_Absyn_Const_e_struct*)_tmpA1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpA1)->f1).String_c).tag != 5)goto _LL73;_tmpA3=(_tmpA2.String_c).f1;_LL72:
_tmpA8=_tmpA3;goto _LL74;_LL73: if(*((int*)_tmpA1)!= 15)goto _LL75;_tmpA4=((struct
Cyc_Absyn_Cast_e_struct*)_tmpA1)->f2;_tmpA5=*_tmpA4;_tmpA6=(void*)_tmpA5.r;if(*((
int*)_tmpA6)!= 0)goto _LL75;_tmpA7=((struct Cyc_Absyn_Const_e_struct*)_tmpA6)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpA6)->f1).String_c).tag != 5)goto _LL75;
_tmpA8=(_tmpA7.String_c).f1;_LL74: desc_types=type_getter(te,(struct
_dynforward_ptr)_tmpA8,fmt->loc);goto _LL70;_LL75:;_LL76: if(opt_args != 0){struct
Cyc_List_List*_tmpA9=(struct Cyc_List_List*)opt_args->v;for(0;_tmpA9 != 0;_tmpA9=
_tmpA9->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpA9->hd);if(Cyc_Tcutil_is_unique_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpA9->hd)->topt))->v)
 && !Cyc_Tcutil_is_unique_path((struct Cyc_Absyn_Exp*)_tmpA9->hd))({void*_tmpAA[0]={};
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA9->hd)->loc,({const char*_tmpAB="Cannot consume paths; do swap instead";
_tag_dynforward(_tmpAB,sizeof(char),_get_zero_arr_size(_tmpAB,38));}),
_tag_dynforward(_tmpAA,sizeof(void*),0));});}}return;_LL70:;}if(opt_args != 0){
struct Cyc_List_List*_tmpAC=(struct Cyc_List_List*)opt_args->v;for(0;desc_types != 
0  && _tmpAC != 0;(desc_types=desc_types->tl,_tmpAC=_tmpAC->tl)){void*t=(void*)
desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpAC->hd;Cyc_Tcexp_tcExp(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpB0;_tmpB0.tag=0;_tmpB0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmpAF;_tmpAF.tag=0;_tmpAF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpAD[2]={& _tmpAF,& _tmpB0};Cyc_Tcutil_terr(e->loc,({const char*_tmpAE="descriptor has type \n%s\n but argument has type \n%s";
_tag_dynforward(_tmpAE,sizeof(char),_get_zero_arr_size(_tmpAE,51));}),
_tag_dynforward(_tmpAD,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}if(
Cyc_Tcutil_is_unique_pointer(t) && !Cyc_Tcutil_is_unique_path(e))({void*_tmpB1[0]={};
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpAC->hd)->loc,({const char*_tmpB2="Cannot consume paths; do swap instead";
_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,38));}),
_tag_dynforward(_tmpB1,sizeof(void*),0));});}if(desc_types != 0)({void*_tmpB3[0]={};
Cyc_Tcutil_terr(fmt->loc,({const char*_tmpB4="too few arguments";_tag_dynforward(
_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,18));}),_tag_dynforward(_tmpB3,
sizeof(void*),0));});if(_tmpAC != 0)({void*_tmpB5[0]={};Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmpAC->hd)->loc,({const char*_tmpB6="too many arguments";
_tag_dynforward(_tmpB6,sizeof(char),_get_zero_arr_size(_tmpB6,19));}),
_tag_dynforward(_tmpB5,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpB7=p;_LL78: if((int)_tmpB7 != 0)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if((int)_tmpB7 != 2)goto _LL7C;_LL7B: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB8[1]={& _tmpBA};Cyc_Tcutil_terr(
loc,({const char*_tmpB9="expecting arithmetic type but found %s";_tag_dynforward(
_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,39));}),_tag_dynforward(_tmpB8,
sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
_LL7C: if((int)_tmpB7 != 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct _tmpBD;_tmpBD.tag=
0;_tmpBD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpBB[1]={& _tmpBD};Cyc_Tcutil_terr(loc,({const char*_tmpBC="expecting integral or * type but found %s";
_tag_dynforward(_tmpBC,sizeof(char),_get_zero_arr_size(_tmpBC,42));}),
_tag_dynforward(_tmpBB,sizeof(void*),1));}});return Cyc_Absyn_sint_typ;_LL7E: if((
int)_tmpB7 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(e))({struct Cyc_String_pa_struct
_tmpC0;_tmpC0.tag=0;_tmpC0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpBE[1]={& _tmpC0};Cyc_Tcutil_terr(loc,({const char*_tmpBF="expecting integral type but found %s";
_tag_dynforward(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,37));}),
_tag_dynforward(_tmpBE,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL80: if((int)_tmpB7 != 19)goto _LL82;_LL81:{void*_tmpC1=
t;struct Cyc_Absyn_PtrInfo _tmpC2;struct Cyc_Absyn_PtrAtts _tmpC3;struct Cyc_Absyn_Conref*
_tmpC4;_LL85: if(_tmpC1 <= (void*)4)goto _LL89;if(*((int*)_tmpC1)!= 7)goto _LL87;
_LL86: goto _LL84;_LL87: if(*((int*)_tmpC1)!= 4)goto _LL89;_tmpC2=((struct Cyc_Absyn_PointerType_struct*)
_tmpC1)->f1;_tmpC3=_tmpC2.ptr_atts;_tmpC4=_tmpC3.bounds;_LL88:{union Cyc_Absyn_Constraint_union
_tmpC5=(Cyc_Absyn_compress_conref(_tmpC4))->v;void*_tmpC6;void*_tmpC7;void*
_tmpC8;_LL8C: if((_tmpC5.Eq_constr).tag != 0)goto _LL8E;_tmpC6=(_tmpC5.Eq_constr).f1;
if((int)_tmpC6 != 0)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if((_tmpC5.Eq_constr).tag != 0)
goto _LL90;_tmpC7=(_tmpC5.Eq_constr).f1;if((int)_tmpC7 != 1)goto _LL90;_LL8F: goto
_LL91;_LL90: if((_tmpC5.Eq_constr).tag != 0)goto _LL92;_tmpC8=(_tmpC5.Eq_constr).f1;
if(_tmpC8 <= (void*)2)goto _LL92;if(*((int*)_tmpC8)!= 0)goto _LL92;_LL91: goto _LL8B;
_LL92:;_LL93:({void*_tmpC9[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpCA="can't apply size to pointer/array of abstract length";
_tag_dynforward(_tmpCA,sizeof(char),_get_zero_arr_size(_tmpCA,53));}),
_tag_dynforward(_tmpC9,sizeof(void*),0));});_LL8B:;}goto _LL84;_LL89:;_LL8A:({
struct Cyc_String_pa_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpCB[1]={& _tmpCD};Cyc_Tcutil_terr(
loc,({const char*_tmpCC="size requires pointer or array type, not %s";
_tag_dynforward(_tmpCC,sizeof(char),_get_zero_arr_size(_tmpCC,44));}),
_tag_dynforward(_tmpCB,sizeof(void*),1));}});_LL84:;}return Cyc_Absyn_uint_typ;
_LL82:;_LL83:({void*_tmpCE[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpCF="Non-unary primop";
_tag_dynforward(_tmpCF,sizeof(char),_get_zero_arr_size(_tmpCF,17));}),
_tag_dynforward(_tmpCE,sizeof(void*),0));});_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_String_pa_struct
_tmpD2;_tmpD2.tag=0;_tmpD2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpD0[1]={& _tmpD2};
Cyc_Tcutil_terr(e1->loc,({const char*_tmpD1="type %s cannot be used here";
_tag_dynforward(_tmpD1,sizeof(char),_get_zero_arr_size(_tmpD1,28));}),
_tag_dynforward(_tmpD0,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD3;}));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpD6;_tmpD6.tag=0;
_tmpD6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpD4[1]={& _tmpD6};
Cyc_Tcutil_terr(e2->loc,({const char*_tmpD5="type %s cannot be used here";
_tag_dynforward(_tmpD5,sizeof(char),_get_zero_arr_size(_tmpD5,28));}),
_tag_dynforward(_tmpD4,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD7;}));}{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static
void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
void*_tmpD8=t1;struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;struct Cyc_Absyn_Tqual
_tmpDB;struct Cyc_Absyn_PtrAtts _tmpDC;void*_tmpDD;struct Cyc_Absyn_Conref*_tmpDE;
struct Cyc_Absyn_Conref*_tmpDF;struct Cyc_Absyn_Conref*_tmpE0;_LL95: if(_tmpD8 <= (
void*)4)goto _LL97;if(*((int*)_tmpD8)!= 4)goto _LL97;_tmpD9=((struct Cyc_Absyn_PointerType_struct*)
_tmpD8)->f1;_tmpDA=(void*)_tmpD9.elt_typ;_tmpDB=_tmpD9.elt_tq;_tmpDC=_tmpD9.ptr_atts;
_tmpDD=(void*)_tmpDC.rgn;_tmpDE=_tmpDC.nullable;_tmpDF=_tmpDC.bounds;_tmpE0=
_tmpDC.zero_term;_LL96: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpDA),(void*)
1))({void*_tmpE1[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpE2="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpE2,sizeof(char),_get_zero_arr_size(_tmpE2,50));}),
_tag_dynforward(_tmpE1,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t1))({
void*_tmpE3[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpE4="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,48));}),
_tag_dynforward(_tmpE3,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({
struct Cyc_String_pa_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmpE5[1]={& _tmpE7};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpE6="expecting int but found %s";_tag_dynforward(_tmpE6,
sizeof(char),_get_zero_arr_size(_tmpE6,27));}),_tag_dynforward(_tmpE5,sizeof(
void*),1));}});_tmpDF=Cyc_Absyn_compress_conref(_tmpDF);{union Cyc_Absyn_Constraint_union
_tmpE8=_tmpDF->v;void*_tmpE9;void*_tmpEA;void*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;
void*_tmpED;_LL9A: if((_tmpE8.Eq_constr).tag != 0)goto _LL9C;_tmpE9=(_tmpE8.Eq_constr).f1;
if((int)_tmpE9 != 0)goto _LL9C;_LL9B: return t1;_LL9C: if((_tmpE8.Eq_constr).tag != 0)
goto _LL9E;_tmpEA=(_tmpE8.Eq_constr).f1;if((int)_tmpEA != 1)goto _LL9E;_LL9D: return
t1;_LL9E: if((_tmpE8.Eq_constr).tag != 0)goto _LLA0;_tmpEB=(_tmpE8.Eq_constr).f1;
if(_tmpEB <= (void*)2)goto _LLA0;if(*((int*)_tmpEB)!= 0)goto _LLA0;_tmpEC=((struct
Cyc_Absyn_Upper_b_struct*)_tmpEB)->f1;_LL9F: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpE0))({void*_tmpEE[0]={};Cyc_Tcutil_warn(e1->loc,({
const char*_tmpEF="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dynforward(_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,70));}),
_tag_dynforward(_tmpEE,sizeof(void*),0));});{struct Cyc_Absyn_PointerType_struct*
_tmpF0=({struct Cyc_Absyn_PointerType_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));
_tmpF1[0]=({struct Cyc_Absyn_PointerType_struct _tmpF2;_tmpF2.tag=4;_tmpF2.f1=({
struct Cyc_Absyn_PtrInfo _tmpF3;_tmpF3.elt_typ=(void*)_tmpDA;_tmpF3.elt_tq=_tmpDB;
_tmpF3.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpF4;_tmpF4.rgn=(void*)_tmpDD;_tmpF4.nullable=
Cyc_Absyn_true_conref;_tmpF4.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmpF4.zero_term=
_tmpE0;_tmpF4.ptrloc=0;_tmpF4;});_tmpF3;});_tmpF2;});_tmpF1;});Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpF0,(void*)3);return(void*)_tmpF0;}_LLA0: if((_tmpE8.Eq_constr).tag
!= 0)goto _LLA2;_tmpED=(_tmpE8.Eq_constr).f1;if(_tmpED <= (void*)2)goto _LLA2;if(*((
int*)_tmpED)!= 1)goto _LLA2;_LLA1:({void*_tmpF5[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpF6="pointer arithmetic not allowed on pointers with abstract bounds";
_tag_dynforward(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,64));}),
_tag_dynforward(_tmpF5,sizeof(void*),0));});return t1;_LLA2:;_LLA3: _tmpDF->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmpF7;(_tmpF7.Eq_constr).tag=
0;(_tmpF7.Eq_constr).f1=(void*)((void*)0);_tmpF7;});return t1;_LL99:;}_LL97:;
_LL98: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL94:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;int forward_only1=0;int forward_only2=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt,& forward_only1)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpFB;
_tmpFB.tag=0;_tmpFB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpF8[2]={& _tmpFA,&
_tmpFB};Cyc_Tcutil_terr(e1->loc,({const char*_tmpF9="pointer arithmetic on values of different types (%s != %s)";
_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,59));}),
_tag_dynforward(_tmpF8,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_typ;}else{if(forward_only1)({void*_tmpFC[0]={};Cyc_Tcutil_terr(
e1->loc,({const char*_tmpFD="can't subtract from forward-only ? pointer";
_tag_dynforward(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,43));}),
_tag_dynforward(_tmpFC,sizeof(void*),0));});if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1_elt),(void*)1))({void*_tmpFE[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmpFF="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,50));}),
_tag_dynforward(_tmpFE,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t1))({
void*_tmp100[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp101="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,48));}),
_tag_dynforward(_tmp100,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(te,
e2)){({struct Cyc_String_pa_struct _tmp105;_tmp105.tag=0;_tmp105.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp104;_tmp104.tag=0;_tmp104.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp102[2]={& _tmp104,& _tmp105};Cyc_Tcutil_terr(
e2->loc,({const char*_tmp103="expecting either %s or int but found %s";
_tag_dynforward(_tmp103,sizeof(char),_get_zero_arr_size(_tmp103,40));}),
_tag_dynforward(_tmp102,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,(
void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,(
void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}static
void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,t2) && Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmp106=_cycalloc(
sizeof(*_tmp106));_tmp106->v=Cyc_Tcenv_lookup_type_vars(te);_tmp106;}))))return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmp108=({struct _tuple0 _tmp107;
_tmp107.f1=Cyc_Tcutil_compress(t1);_tmp107.f2=Cyc_Tcutil_compress(t2);_tmp107;});
void*_tmp109;struct Cyc_Absyn_PtrInfo _tmp10A;void*_tmp10B;void*_tmp10C;struct Cyc_Absyn_PtrInfo
_tmp10D;void*_tmp10E;_LLA5: _tmp109=_tmp108.f1;if(_tmp109 <= (void*)4)goto _LLA7;
if(*((int*)_tmp109)!= 4)goto _LLA7;_tmp10A=((struct Cyc_Absyn_PointerType_struct*)
_tmp109)->f1;_tmp10B=(void*)_tmp10A.elt_typ;_tmp10C=_tmp108.f2;if(_tmp10C <= (
void*)4)goto _LLA7;if(*((int*)_tmp10C)!= 4)goto _LLA7;_tmp10D=((struct Cyc_Absyn_PointerType_struct*)
_tmp10C)->f1;_tmp10E=(void*)_tmp10D.elt_typ;_LLA6: if(Cyc_Tcutil_unify(_tmp10B,
_tmp10E))return Cyc_Absyn_sint_typ;goto _LLA4;_LLA7:;_LLA8: goto _LLA4;_LLA4:;}({
struct Cyc_String_pa_struct _tmp112;_tmp112.tag=0;_tmp112.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp111;_tmp111.tag=0;_tmp111.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp10F[2]={& _tmp111,& _tmp112};Cyc_Tcutil_terr(
loc,({const char*_tmp110="comparison not allowed between %s and %s";
_tag_dynforward(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,41));}),
_tag_dynforward(_tmp10F,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp113=_cycalloc(sizeof(*_tmp113));
_tmp113->v=Cyc_Tcenv_lookup_type_vars(te);_tmp113;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp114=p;_LLAA: if((int)_tmp114 != 
0)goto _LLAC;_LLAB: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLAC: if((int)_tmp114 != 2)
goto _LLAE;_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLAE: if((int)_tmp114 != 1)goto
_LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp114 != 3)goto _LLB2;_LLB1: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LLB2: if((int)_tmp114 != 4)goto _LLB4;_LLB3: goto
_LLB5;_LLB4: if((int)_tmp114 != 13)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp114
!= 14)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp114 != 15)goto _LLBA;_LLB9: goto
_LLBB;_LLBA: if((int)_tmp114 != 16)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp114
!= 17)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp114 != 18)goto _LLC0;_LLBF:
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);_LLC0: if((int)
_tmp114 != 5)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if((int)_tmp114 != 6)goto _LLC4;_LLC3:
goto _LLC5;_LLC4: if((int)_tmp114 != 7)goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)
_tmp114 != 8)goto _LLC8;_LLC7: goto _LLC9;_LLC8: if((int)_tmp114 != 9)goto _LLCA;_LLC9:
goto _LLCB;_LLCA: if((int)_tmp114 != 10)goto _LLCC;_LLCB: return Cyc_Tcexp_tcAnyBinop(
te,loc,e1,e2);_LLCC:;_LLCD:({void*_tmp115[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp116="bad binary primop";
_tag_dynforward(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,18));}),
_tag_dynforward(_tmp115,sizeof(void*),0));});_LLA9:;}static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCE: return({void*_tmp117[0]={};
Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp118="primitive operator has 0 arguments";
_tag_dynforward(_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,35));}),
_tag_dynforward(_tmp117,sizeof(void*),0));});case 1: _LLCF: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLD0: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLD1: return({void*_tmp119[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp11A="primitive operator has > 2 arguments";_tag_dynforward(_tmp11A,sizeof(
char),_get_zero_arr_size(_tmp11A,37));}),_tag_dynforward(_tmp119,sizeof(void*),0));});}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp11B=(void*)e->r;void*
_tmp11C;struct Cyc_Absyn_Vardecl*_tmp11D;void*_tmp11E;struct Cyc_Absyn_Vardecl*
_tmp11F;void*_tmp120;struct Cyc_Absyn_Vardecl*_tmp121;void*_tmp122;struct Cyc_Absyn_Vardecl*
_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*
_tmp126;struct _dynforward_ptr*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct
_dynforward_ptr*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12B;
struct Cyc_Absyn_Exp*_tmp12C;_LLD4: if(*((int*)_tmp11B)!= 1)goto _LLD6;_tmp11C=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11B)->f2;if(_tmp11C <= (void*)1)goto
_LLD6;if(*((int*)_tmp11C)!= 2)goto _LLD6;_tmp11D=((struct Cyc_Absyn_Param_b_struct*)
_tmp11C)->f1;_LLD5: _tmp11F=_tmp11D;goto _LLD7;_LLD6: if(*((int*)_tmp11B)!= 1)goto
_LLD8;_tmp11E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11B)->f2;if(_tmp11E <= (
void*)1)goto _LLD8;if(*((int*)_tmp11E)!= 3)goto _LLD8;_tmp11F=((struct Cyc_Absyn_Local_b_struct*)
_tmp11E)->f1;_LLD7: _tmp121=_tmp11F;goto _LLD9;_LLD8: if(*((int*)_tmp11B)!= 1)goto
_LLDA;_tmp120=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11B)->f2;if(_tmp120 <= (
void*)1)goto _LLDA;if(*((int*)_tmp120)!= 4)goto _LLDA;_tmp121=((struct Cyc_Absyn_Pat_b_struct*)
_tmp120)->f1;_LLD9: _tmp123=_tmp121;goto _LLDB;_LLDA: if(*((int*)_tmp11B)!= 1)goto
_LLDC;_tmp122=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11B)->f2;if(_tmp122 <= (
void*)1)goto _LLDC;if(*((int*)_tmp122)!= 0)goto _LLDC;_tmp123=((struct Cyc_Absyn_Global_b_struct*)
_tmp122)->f1;_LLDB: if(!(_tmp123->tq).real_const)return;goto _LLD3;_LLDC: if(*((int*)
_tmp11B)!= 25)goto _LLDE;_tmp124=((struct Cyc_Absyn_Subscript_e_struct*)_tmp11B)->f1;
_tmp125=((struct Cyc_Absyn_Subscript_e_struct*)_tmp11B)->f2;_LLDD:{void*_tmp12D=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp124->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp12E;struct Cyc_Absyn_Tqual _tmp12F;struct Cyc_Absyn_ArrayInfo
_tmp130;struct Cyc_Absyn_Tqual _tmp131;struct Cyc_List_List*_tmp132;_LLEB: if(
_tmp12D <= (void*)4)goto _LLF1;if(*((int*)_tmp12D)!= 4)goto _LLED;_tmp12E=((struct
Cyc_Absyn_PointerType_struct*)_tmp12D)->f1;_tmp12F=_tmp12E.elt_tq;_LLEC: _tmp131=
_tmp12F;goto _LLEE;_LLED: if(*((int*)_tmp12D)!= 7)goto _LLEF;_tmp130=((struct Cyc_Absyn_ArrayType_struct*)
_tmp12D)->f1;_tmp131=_tmp130.tq;_LLEE: if(!_tmp131.real_const)return;goto _LLEA;
_LLEF: if(*((int*)_tmp12D)!= 9)goto _LLF1;_tmp132=((struct Cyc_Absyn_TupleType_struct*)
_tmp12D)->f1;_LLF0: {unsigned int _tmp134;int _tmp135;struct _tuple7 _tmp133=Cyc_Evexp_eval_const_uint_exp(
_tmp125);_tmp134=_tmp133.f1;_tmp135=_tmp133.f2;if(!_tmp135){({void*_tmp136[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp137="tuple projection cannot use sizeof or offsetof";
_tag_dynforward(_tmp137,sizeof(char),_get_zero_arr_size(_tmp137,47));}),
_tag_dynforward(_tmp136,sizeof(void*),0));});return;}{struct _handler_cons _tmp138;
_push_handler(& _tmp138);{int _tmp13A=0;if(setjmp(_tmp138.handler))_tmp13A=1;if(!
_tmp13A){{struct _tuple9 _tmp13C;struct Cyc_Absyn_Tqual _tmp13D;struct _tuple9*
_tmp13B=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp132,(
int)_tmp134);_tmp13C=*_tmp13B;_tmp13D=_tmp13C.f1;if(!_tmp13D.real_const){
_npop_handler(0);return;}};_pop_handler();}else{void*_tmp139=(void*)_exn_thrown;
void*_tmp13F=_tmp139;_LLF4: if(_tmp13F != Cyc_List_Nth)goto _LLF6;_LLF5: return;
_LLF6:;_LLF7:(void)_throw(_tmp13F);_LLF3:;}}}goto _LLEA;}_LLF1:;_LLF2: goto _LLEA;
_LLEA:;}goto _LLD3;_LLDE: if(*((int*)_tmp11B)!= 23)goto _LLE0;_tmp126=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp11B)->f1;_tmp127=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp11B)->f2;_LLDF:{
void*_tmp140=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp126->topt))->v);struct Cyc_Absyn_AggrInfo _tmp141;union Cyc_Absyn_AggrInfoU_union
_tmp142;struct Cyc_Absyn_Aggrdecl**_tmp143;struct Cyc_List_List*_tmp144;_LLF9: if(
_tmp140 <= (void*)4)goto _LLFD;if(*((int*)_tmp140)!= 10)goto _LLFB;_tmp141=((struct
Cyc_Absyn_AggrType_struct*)_tmp140)->f1;_tmp142=_tmp141.aggr_info;if((((((struct
Cyc_Absyn_AggrType_struct*)_tmp140)->f1).aggr_info).KnownAggr).tag != 1)goto _LLFB;
_tmp143=(_tmp142.KnownAggr).f1;_LLFA: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp143 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp143,_tmp127);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF8;}_LLFB: if(*((int*)_tmp140)!= 11)goto _LLFD;_tmp144=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp140)->f2;_LLFC: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp144,
_tmp127);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF8;}_LLFD:;_LLFE: goto
_LLF8;_LLF8:;}goto _LLD3;_LLE0: if(*((int*)_tmp11B)!= 24)goto _LLE2;_tmp128=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp11B)->f1;_tmp129=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp11B)->f2;_LLE1:{void*_tmp145=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp128->topt))->v);struct Cyc_Absyn_PtrInfo _tmp146;void*_tmp147;
_LL100: if(_tmp145 <= (void*)4)goto _LL102;if(*((int*)_tmp145)!= 4)goto _LL102;
_tmp146=((struct Cyc_Absyn_PointerType_struct*)_tmp145)->f1;_tmp147=(void*)
_tmp146.elt_typ;_LL101:{void*_tmp148=Cyc_Tcutil_compress(_tmp147);struct Cyc_Absyn_AggrInfo
_tmp149;union Cyc_Absyn_AggrInfoU_union _tmp14A;struct Cyc_Absyn_Aggrdecl**_tmp14B;
struct Cyc_List_List*_tmp14C;_LL105: if(_tmp148 <= (void*)4)goto _LL109;if(*((int*)
_tmp148)!= 10)goto _LL107;_tmp149=((struct Cyc_Absyn_AggrType_struct*)_tmp148)->f1;
_tmp14A=_tmp149.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp148)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL107;_tmp14B=(_tmp14A.KnownAggr).f1;_LL106: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp14B == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp14B,_tmp129);if(sf == 0  || !(sf->tq).real_const)return;goto _LL104;}_LL107: if(*((
int*)_tmp148)!= 11)goto _LL109;_tmp14C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp148)->f2;_LL108: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp14C,_tmp129);if(sf == 0  || !(sf->tq).real_const)return;goto _LL104;}_LL109:;
_LL10A: goto _LL104;_LL104:;}goto _LLFF;_LL102:;_LL103: goto _LLFF;_LLFF:;}goto _LLD3;
_LLE2: if(*((int*)_tmp11B)!= 22)goto _LLE4;_tmp12A=((struct Cyc_Absyn_Deref_e_struct*)
_tmp11B)->f1;_LLE3:{void*_tmp14D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp12A->topt))->v);struct Cyc_Absyn_PtrInfo _tmp14E;struct Cyc_Absyn_Tqual
_tmp14F;struct Cyc_Absyn_ArrayInfo _tmp150;struct Cyc_Absyn_Tqual _tmp151;_LL10C: if(
_tmp14D <= (void*)4)goto _LL110;if(*((int*)_tmp14D)!= 4)goto _LL10E;_tmp14E=((
struct Cyc_Absyn_PointerType_struct*)_tmp14D)->f1;_tmp14F=_tmp14E.elt_tq;_LL10D:
_tmp151=_tmp14F;goto _LL10F;_LL10E: if(*((int*)_tmp14D)!= 7)goto _LL110;_tmp150=((
struct Cyc_Absyn_ArrayType_struct*)_tmp14D)->f1;_tmp151=_tmp150.tq;_LL10F: if(!
_tmp151.real_const)return;goto _LL10B;_LL110:;_LL111: goto _LL10B;_LL10B:;}goto
_LLD3;_LLE4: if(*((int*)_tmp11B)!= 13)goto _LLE6;_tmp12B=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp11B)->f1;_LLE5: _tmp12C=_tmp12B;goto _LLE7;_LLE6: if(*((int*)_tmp11B)!= 14)goto
_LLE8;_tmp12C=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp11B)->f1;_LLE7: Cyc_Tcexp_check_writable(
te,_tmp12C);return;_LLE8:;_LLE9: goto _LLD3;_LLD3:;}({struct Cyc_String_pa_struct
_tmp154;_tmp154.tag=0;_tmp154.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp152[1]={& _tmp154};Cyc_Tcutil_terr(e->loc,({
const char*_tmp153="attempt to write a const location: %s";_tag_dynforward(
_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,38));}),_tag_dynforward(_tmp152,
sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp155[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp156="increment/decrement of non-lvalue";
_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,34));}),
_tag_dynforward(_tmp155,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt,& forward_only) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (
void*)0  || i == (void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(
void*)1))({void*_tmp157[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp158="can't perform arithmetic on abstract pointer type";
_tag_dynforward(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,50));}),
_tag_dynforward(_tmp157,sizeof(void*),0));});if(Cyc_Tcutil_is_unique_pointer(t))({
void*_tmp159[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp15A="can't perform arithmetic on unique pointer type";
_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,48));}),
_tag_dynforward(_tmp159,sizeof(void*),0));});if(forward_only  && (i == (void*)2
 || i == (void*)3))({void*_tmp15B[0]={};Cyc_Tcutil_terr(e->loc,({const char*
_tmp15C="can't subtract from forward-only ? pointer";_tag_dynforward(_tmp15C,
sizeof(char),_get_zero_arr_size(_tmp15C,43));}),_tag_dynforward(_tmp15B,sizeof(
void*),0));});}else{({struct Cyc_String_pa_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp15D[1]={& _tmp15F};Cyc_Tcutil_terr(e->loc,({const char*_tmp15E="expecting arithmetic or ? type but found %s";
_tag_dynforward(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,44));}),
_tag_dynforward(_tmp15D,sizeof(void*),1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const
char*_tmp160="conditional expression";_tag_dynforward(_tmp160,sizeof(char),
_get_zero_arr_size(_tmp160,23));}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp169;}));struct Cyc_List_List _tmp161=({struct Cyc_List_List _tmp168;_tmp168.hd=
e3;_tmp168.tl=0;_tmp168;});struct Cyc_List_List _tmp162=({struct Cyc_List_List
_tmp167;_tmp167.hd=e2;_tmp167.tl=(struct Cyc_List_List*)& _tmp161;_tmp167;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp162)){({struct Cyc_String_pa_struct
_tmp166;_tmp166.tag=0;_tmp166.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{
struct Cyc_String_pa_struct _tmp165;_tmp165.tag=0;_tmp165.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmp163[2]={& _tmp165,& _tmp166};
Cyc_Tcutil_terr(loc,({const char*_tmp164="conditional clause types do not match: %s != %s";
_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size(_tmp164,48));}),
_tag_dynforward(_tmp163,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp16A="logical-and expression";_tag_dynforward(_tmp16A,sizeof(char),
_get_zero_arr_size(_tmp16A,23));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp16B="logical-and expression";
_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,23));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({
const char*_tmp16C="logical-or expression";_tag_dynforward(_tmp16C,sizeof(char),
_get_zero_arr_size(_tmp16C,22));}));Cyc_Tcexp_tcTest(te,e2,({const char*_tmp16D="logical-or expression";
_tag_dynforward(_tmp16D,sizeof(char),_get_zero_arr_size(_tmp16D,22));}));return
Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){{struct _RegionHandle _tmp16E=_new_region("r");struct
_RegionHandle*r=& _tmp16E;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(
r,te),0,e1);;_pop_region(r);}Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp16F=Cyc_Tcutil_compress(t1);_LL113: if(_tmp16F <= (void*)4)goto _LL115;if(*((
int*)_tmp16F)!= 7)goto _LL115;_LL114:({void*_tmp170[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp171="cannot assign to an array";_tag_dynforward(_tmp171,sizeof(
char),_get_zero_arr_size(_tmp171,26));}),_tag_dynforward(_tmp170,sizeof(void*),0));});
goto _LL112;_LL115:;_LL116: goto _LL112;_LL112:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp172[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp173="type is abstract (can't determine size).";
_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,41));}),
_tag_dynforward(_tmp172,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp174[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp175="assignment to non-lvalue";
_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,25));}),
_tag_dynforward(_tmp174,sizeof(void*),0));});Cyc_Tcexp_check_writable(te,e1);if(
po == 0){if(Cyc_Tcutil_is_unique_pointer(t2) && !Cyc_Tcutil_is_unique_path(e2))({
void*_tmp176[0]={};Cyc_Tcutil_terr(e2->loc,({const char*_tmp177="Cannot consume paths; do swap instead";
_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,38));}),
_tag_dynforward(_tmp176,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_assign(te,e2,
t1)){void*_tmp178=({struct Cyc_String_pa_struct _tmp17C;_tmp17C.tag=0;_tmp17C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp17B;_tmp17B.tag=0;_tmp17B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp179[2]={& _tmp17B,& _tmp17C};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp17A="type mismatch: %s != %s";_tag_dynforward(_tmp17A,sizeof(char),
_get_zero_arr_size(_tmp17A,24));}),_tag_dynforward(_tmp179,sizeof(void*),2));}}});
Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();return _tmp178;}}else{void*
_tmp17D=(void*)po->v;void*_tmp17E=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp17D,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp17E,t1) || Cyc_Tcutil_coerceable(_tmp17E) && Cyc_Tcutil_coerceable(
t1))){void*_tmp17F=({struct Cyc_String_pa_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp182;_tmp182.tag=0;_tmp182.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp180[2]={& _tmp182,& _tmp183};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp181="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,82));}),
_tag_dynforward(_tmp180,sizeof(void*),2));}}});Cyc_Tcutil_unify(_tmp17E,t1);Cyc_Tcutil_explain_failure();
return _tmp17F;}return _tmp17E;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp184=Cyc_Tcutil_compress(
t1);void*_tmp185;void*_tmp186;_LL118: if((int)_tmp184 != 1)goto _LL11A;_LL119: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL117;_LL11A: if(_tmp184 <= (void*)4)goto
_LL11E;if(*((int*)_tmp184)!= 5)goto _LL11C;_tmp185=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp184)->f2;if((int)_tmp185 != 0)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(*((int*)
_tmp184)!= 5)goto _LL11E;_tmp186=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp184)->f2;
if((int)_tmp186 != 1)goto _LL11E;_LL11D: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL117;_LL11E:;_LL11F: goto _LL117;_LL117:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp188;struct Cyc_List_List*
_tmp189;struct Cyc_Position_Segment*_tmp18A;void*_tmp18B;struct Cyc_Absyn_Tunionfield
_tmp187=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp188=_tmp187.name;_tmp189=
_tmp187.typs;_tmp18A=_tmp187.loc;_tmp18B=(void*)_tmp187.sc;if(_tmp189 == 0  || 
_tmp189->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp189->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_Position_Segment*
_tmp18F;void*_tmp190;struct Cyc_Absyn_Tunionfield _tmp18C=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp18D=_tmp18C.name;_tmp18E=_tmp18C.typs;_tmp18F=_tmp18C.loc;
_tmp190=(void*)_tmp18C.sc;if(_tmp18E == 0  || _tmp18E->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp18E->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp194;_tmp194.tag=0;_tmp194.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{
struct Cyc_String_pa_struct _tmp193;_tmp193.tag=0;_tmp193.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(tu));{void*
_tmp191[2]={& _tmp193,& _tmp194};Cyc_Tcutil_terr(e->loc,({const char*_tmp192="can't find a field in %s to inject a value of type %s";
_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,54));}),
_tag_dynforward(_tmp191,sizeof(void*),2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){
struct Cyc_List_List*_tmp196=args;struct _RegionHandle _tmp197=_new_region("ter");
struct _RegionHandle*ter=& _tmp197;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp198=
Cyc_Tcenv_new_block(ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp198,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp199=t;struct Cyc_Absyn_PtrInfo
_tmp19A;void*_tmp19B;struct Cyc_Absyn_Tqual _tmp19C;struct Cyc_Absyn_PtrAtts _tmp19D;
void*_tmp19E;struct Cyc_Absyn_Conref*_tmp19F;struct Cyc_Absyn_Conref*_tmp1A0;
struct Cyc_Absyn_Conref*_tmp1A1;_LL121: if(_tmp199 <= (void*)4)goto _LL123;if(*((int*)
_tmp199)!= 4)goto _LL123;_tmp19A=((struct Cyc_Absyn_PointerType_struct*)_tmp199)->f1;
_tmp19B=(void*)_tmp19A.elt_typ;_tmp19C=_tmp19A.elt_tq;_tmp19D=_tmp19A.ptr_atts;
_tmp19E=(void*)_tmp19D.rgn;_tmp19F=_tmp19D.nullable;_tmp1A0=_tmp19D.bounds;
_tmp1A1=_tmp19D.zero_term;_LL122: Cyc_Tcenv_check_rgn_accessible(_tmp198,loc,
_tmp19E);Cyc_Tcutil_check_nonzero_bound(loc,_tmp1A0);{void*_tmp1A2=Cyc_Tcutil_compress(
_tmp19B);struct Cyc_Absyn_FnInfo _tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_Core_Opt*
_tmp1A5;void*_tmp1A6;struct Cyc_List_List*_tmp1A7;int _tmp1A8;struct Cyc_Absyn_VarargInfo*
_tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1AB;_LL126: if(
_tmp1A2 <= (void*)4)goto _LL128;if(*((int*)_tmp1A2)!= 8)goto _LL128;_tmp1A3=((
struct Cyc_Absyn_FnType_struct*)_tmp1A2)->f1;_tmp1A4=_tmp1A3.tvars;_tmp1A5=
_tmp1A3.effect;_tmp1A6=(void*)_tmp1A3.ret_typ;_tmp1A7=_tmp1A3.args;_tmp1A8=
_tmp1A3.c_varargs;_tmp1A9=_tmp1A3.cyc_varargs;_tmp1AA=_tmp1A3.rgn_po;_tmp1AB=
_tmp1A3.attributes;_LL127: if(topt != 0)Cyc_Tcutil_unify(_tmp1A6,*topt);while(
_tmp196 != 0  && _tmp1A7 != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp196->hd;
void*t2=(*((struct _tuple2*)_tmp1A7->hd)).f3;Cyc_Tcexp_tcExp(_tmp198,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp198,e1,t2)){({struct Cyc_String_pa_struct
_tmp1AF;_tmp1AF.tag=0;_tmp1AF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1AE;_tmp1AE.tag=0;
_tmp1AE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp1AC[2]={&
_tmp1AE,& _tmp1AF};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1AD="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dynforward(_tmp1AD,sizeof(char),_get_zero_arr_size(_tmp1AD,57));}),
_tag_dynforward(_tmp1AC,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_unique_pointer(
t2) && !Cyc_Tcutil_is_unique_path(e1))({void*_tmp1B0[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp1B1="Cannot consume paths; do swap instead";_tag_dynforward(
_tmp1B1,sizeof(char),_get_zero_arr_size(_tmp1B1,38));}),_tag_dynforward(_tmp1B0,
sizeof(void*),0));});_tmp196=_tmp196->tl;_tmp1A7=_tmp1A7->tl;}{int
args_already_checked=0;{struct Cyc_List_List*a=_tmp1AB;for(0;a != 0;a=a->tl){void*
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
_tmp198,_tmp1B9,fmt_args,Cyc_Formatstr_get_format_typs);goto _LL12F;_LL132: if((
int)_tmp1BB != 1)goto _LL12F;_LL133: Cyc_Tcexp_check_format_args(_tmp198,_tmp1B9,
fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL12F;_LL12F:;}};_pop_handler();}
else{void*_tmp1B7=(void*)_exn_thrown;void*_tmp1BD=_tmp1B7;_LL135: if(_tmp1BD != 
Cyc_List_Nth)goto _LL137;_LL136:({void*_tmp1BE[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp1BF="bad format arguments";_tag_dynforward(_tmp1BF,sizeof(char),
_get_zero_arr_size(_tmp1BF,21));}),_tag_dynforward(_tmp1BE,sizeof(void*),0));});
goto _LL134;_LL137:;_LL138:(void)_throw(_tmp1BD);_LL134:;}}}goto _LL12A;_LL12D:;
_LL12E: goto _LL12A;_LL12A:;}}if(_tmp1A7 != 0)({void*_tmp1C0[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp1C1="too few arguments for function";_tag_dynforward(_tmp1C1,
sizeof(char),_get_zero_arr_size(_tmp1C1,31));}),_tag_dynforward(_tmp1C0,sizeof(
void*),0));});else{if((_tmp196 != 0  || _tmp1A8) || _tmp1A9 != 0){if(_tmp1A8)for(0;
_tmp196 != 0;_tmp196=_tmp196->tl){Cyc_Tcexp_tcExp(_tmp198,0,(struct Cyc_Absyn_Exp*)
_tmp196->hd);}else{if(_tmp1A9 == 0)({void*_tmp1C2[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp1C3="too many arguments for function";_tag_dynforward(_tmp1C3,
sizeof(char),_get_zero_arr_size(_tmp1C3,32));}),_tag_dynforward(_tmp1C2,sizeof(
void*),0));});else{void*_tmp1C5;int _tmp1C6;struct Cyc_Absyn_VarargInfo _tmp1C4=*
_tmp1A9;_tmp1C5=(void*)_tmp1C4.type;_tmp1C6=_tmp1C4.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp1C7=({struct Cyc_Absyn_VarargCallInfo*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));
_tmp1E3->num_varargs=0;_tmp1E3->injectors=0;_tmp1E3->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp1A9;_tmp1E3;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1C7;
if(!_tmp1C6)for(0;_tmp196 != 0;_tmp196=_tmp196->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp196->hd;_tmp1C7->num_varargs ++;Cyc_Tcexp_tcExp(_tmp198,(
void**)& _tmp1C5,e1);if(!Cyc_Tcutil_coerce_arg(_tmp198,e1,_tmp1C5)){({struct Cyc_String_pa_struct
_tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp1C5));{void*
_tmp1C8[2]={& _tmp1CA,& _tmp1CB};Cyc_Tcutil_terr(loc,({const char*_tmp1C9="vararg requires type %s but argument has type %s";
_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,49));}),
_tag_dynforward(_tmp1C8,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}if(
Cyc_Tcutil_is_unique_pointer(_tmp1C5) && !Cyc_Tcutil_is_unique_path(e1))({void*
_tmp1CC[0]={};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1CD="Cannot consume paths; do swap instead";
_tag_dynforward(_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,38));}),
_tag_dynforward(_tmp1CC,sizeof(void*),0));});}else{void*_tmp1CE=Cyc_Tcutil_compress(
_tmp1C5);struct Cyc_Absyn_TunionInfo _tmp1CF;union Cyc_Absyn_TunionInfoU_union
_tmp1D0;struct Cyc_Absyn_Tuniondecl**_tmp1D1;struct Cyc_Absyn_Tuniondecl*_tmp1D2;
struct Cyc_List_List*_tmp1D3;struct Cyc_Core_Opt*_tmp1D4;_LL13A: if(_tmp1CE <= (void*)
4)goto _LL13C;if(*((int*)_tmp1CE)!= 2)goto _LL13C;_tmp1CF=((struct Cyc_Absyn_TunionType_struct*)
_tmp1CE)->f1;_tmp1D0=_tmp1CF.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp1CE)->f1).tunion_info).KnownTunion).tag != 1)goto _LL13C;_tmp1D1=(_tmp1D0.KnownTunion).f1;
_tmp1D2=*_tmp1D1;_tmp1D3=_tmp1CF.targs;_tmp1D4=_tmp1CF.rgn;_LL13B: {struct Cyc_Absyn_Tuniondecl*
_tmp1D5=*Cyc_Tcenv_lookup_tuniondecl(_tmp198,loc,_tmp1D2->name);struct Cyc_List_List*
fields=0;if(_tmp1D5->fields == 0)({struct Cyc_String_pa_struct _tmp1D8;_tmp1D8.tag=
0;_tmp1D8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp1C5));{void*_tmp1D6[1]={& _tmp1D8};Cyc_Tcutil_terr(loc,({const char*_tmp1D7="can't inject into %s";
_tag_dynforward(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,21));}),
_tag_dynforward(_tmp1D6,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D5->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D4))->v,Cyc_Tcenv_curr_rgn(_tmp198)))({void*
_tmp1D9[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1DA="bad region for injected varargs";
_tag_dynforward(_tmp1DA,sizeof(char),_get_zero_arr_size(_tmp1DA,32));}),
_tag_dynforward(_tmp1D9,sizeof(void*),0));});{struct _RegionHandle _tmp1DB=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp1DB;_push_region(rgn);{struct Cyc_List_List*
_tmp1DC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1D5->tvs,
_tmp1D3);for(0;_tmp196 != 0;_tmp196=_tmp196->tl){_tmp1C7->num_varargs ++;{struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp196->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(
_tmp198,0,e1);if(Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v) && !Cyc_Tcutil_is_unique_path(e1))({void*_tmp1DD[0]={};
Cyc_Tcutil_terr(e1->loc,({const char*_tmp1DE="Cannot consume paths; do swap instead";
_tag_dynforward(_tmp1DE,sizeof(char),_get_zero_arr_size(_tmp1DE,38));}),
_tag_dynforward(_tmp1DD,sizeof(void*),0));});}{struct Cyc_Absyn_Tunionfield*
_tmp1DF=Cyc_Tcexp_tcInjection(_tmp198,e1,_tmp1C5,rgn,_tmp1DC,fields);if(_tmp1DF
!= 0)_tmp1C7->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp1C7->injectors,({struct Cyc_List_List*
_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->hd=(struct Cyc_Absyn_Tunionfield*)
_tmp1DF;_tmp1E0->tl=0;_tmp1E0;}));}}}};_pop_region(rgn);}goto _LL139;}_LL13C:;
_LL13D:({void*_tmp1E1[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1E2="bad inject vararg type";
_tag_dynforward(_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,23));}),
_tag_dynforward(_tmp1E1,sizeof(void*),0));});goto _LL139;_LL139:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp198,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1A5))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp198,loc,_tmp1AA);{void*_tmp1E4=_tmp1A6;_npop_handler(0);return _tmp1E4;}}
_LL128:;_LL129: {void*_tmp1E7=({void*_tmp1E5[0]={};Cyc_Tcexp_expr_err(_tmp198,
loc,topt,({const char*_tmp1E6="expected pointer to function";_tag_dynforward(
_tmp1E6,sizeof(char),_get_zero_arr_size(_tmp1E6,29));}),_tag_dynforward(_tmp1E5,
sizeof(void*),0));});_npop_handler(0);return _tmp1E7;}_LL125:;}_LL123:;_LL124: {
void*_tmp1EA=({void*_tmp1E8[0]={};Cyc_Tcexp_expr_err(_tmp198,loc,topt,({const
char*_tmp1E9="expected pointer to function";_tag_dynforward(_tmp1E9,sizeof(char),
_get_zero_arr_size(_tmp1E9,29));}),_tag_dynforward(_tmp1E8,sizeof(void*),0));});
_npop_handler(0);return _tmp1EA;}_LL120:;}};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,
e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct _tmp1ED;_tmp1ED.tag=0;_tmp1ED.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp1EB[1]={& _tmp1ED};Cyc_Tcutil_terr(
loc,({const char*_tmp1EC="expected xtunion exn but found %s";_tag_dynforward(
_tmp1EC,sizeof(char),_get_zero_arr_size(_tmp1EC,34));}),_tag_dynforward(_tmp1EB,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1EE=
_cycalloc(sizeof(*_tmp1EE));_tmp1EE->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1EE;}));}
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1EF=t1;struct Cyc_Absyn_PtrInfo
_tmp1F0;void*_tmp1F1;struct Cyc_Absyn_Tqual _tmp1F2;struct Cyc_Absyn_PtrAtts _tmp1F3;
void*_tmp1F4;struct Cyc_Absyn_Conref*_tmp1F5;struct Cyc_Absyn_Conref*_tmp1F6;
struct Cyc_Absyn_Conref*_tmp1F7;_LL13F: if(_tmp1EF <= (void*)4)goto _LL141;if(*((int*)
_tmp1EF)!= 4)goto _LL141;_tmp1F0=((struct Cyc_Absyn_PointerType_struct*)_tmp1EF)->f1;
_tmp1F1=(void*)_tmp1F0.elt_typ;_tmp1F2=_tmp1F0.elt_tq;_tmp1F3=_tmp1F0.ptr_atts;
_tmp1F4=(void*)_tmp1F3.rgn;_tmp1F5=_tmp1F3.nullable;_tmp1F6=_tmp1F3.bounds;
_tmp1F7=_tmp1F3.zero_term;_LL140:{void*_tmp1F8=Cyc_Tcutil_compress(_tmp1F1);
struct Cyc_Absyn_FnInfo _tmp1F9;struct Cyc_List_List*_tmp1FA;struct Cyc_Core_Opt*
_tmp1FB;void*_tmp1FC;struct Cyc_List_List*_tmp1FD;int _tmp1FE;struct Cyc_Absyn_VarargInfo*
_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp201;_LL144: if(
_tmp1F8 <= (void*)4)goto _LL146;if(*((int*)_tmp1F8)!= 8)goto _LL146;_tmp1F9=((
struct Cyc_Absyn_FnType_struct*)_tmp1F8)->f1;_tmp1FA=_tmp1F9.tvars;_tmp1FB=
_tmp1F9.effect;_tmp1FC=(void*)_tmp1F9.ret_typ;_tmp1FD=_tmp1F9.args;_tmp1FE=
_tmp1F9.c_varargs;_tmp1FF=_tmp1F9.cyc_varargs;_tmp200=_tmp1F9.rgn_po;_tmp201=
_tmp1F9.attributes;_LL145: {struct Cyc_List_List*instantiation=0;for(0;ts != 0  && 
_tmp1FA != 0;(ts=ts->tl,_tmp1FA=_tmp1FA->tl)){void*k=Cyc_Tcutil_tvar_kind((struct
Cyc_Absyn_Tvar*)_tmp1FA->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp202=_cycalloc(
sizeof(*_tmp202));_tmp202->hd=({struct _tuple5*_tmp203=_cycalloc(sizeof(*_tmp203));
_tmp203->f1=(struct Cyc_Absyn_Tvar*)_tmp1FA->hd;_tmp203->f2=(void*)ts->hd;_tmp203;});
_tmp202->tl=instantiation;_tmp202;});}if(ts != 0)return({void*_tmp204[0]={};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp205="too many type variables in instantiation";
_tag_dynforward(_tmp205,sizeof(char),_get_zero_arr_size(_tmp205,41));}),
_tag_dynforward(_tmp204,sizeof(void*),0));});{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp20A=_cycalloc(sizeof(*
_tmp20A));_tmp20A[0]=({struct Cyc_Absyn_FnType_struct _tmp20B;_tmp20B.tag=8;
_tmp20B.f1=({struct Cyc_Absyn_FnInfo _tmp20C;_tmp20C.tvars=_tmp1FA;_tmp20C.effect=
_tmp1FB;_tmp20C.ret_typ=(void*)_tmp1FC;_tmp20C.args=_tmp1FD;_tmp20C.c_varargs=
_tmp1FE;_tmp20C.cyc_varargs=_tmp1FF;_tmp20C.rgn_po=_tmp200;_tmp20C.attributes=
_tmp201;_tmp20C;});_tmp20B;});_tmp20A;}));return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206[0]=({struct Cyc_Absyn_PointerType_struct
_tmp207;_tmp207.tag=4;_tmp207.f1=({struct Cyc_Absyn_PtrInfo _tmp208;_tmp208.elt_typ=(
void*)new_fn_typ;_tmp208.elt_tq=_tmp1F2;_tmp208.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp209;_tmp209.rgn=(void*)_tmp1F4;_tmp209.nullable=_tmp1F5;_tmp209.bounds=
_tmp1F6;_tmp209.zero_term=_tmp1F7;_tmp209.ptrloc=0;_tmp209;});_tmp208;});_tmp207;});
_tmp206;});}}_LL146:;_LL147: goto _LL143;_LL143:;}goto _LL13E;_LL141:;_LL142: goto
_LL13E;_LL13E:;}return({struct Cyc_String_pa_struct _tmp20F;_tmp20F.tag=0;_tmp20F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp20D[1]={& _tmp20F};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp20E="expecting polymorphic type but found %s";
_tag_dynforward(_tmp20E,sizeof(char),_get_zero_arr_size(_tmp20E,40));}),
_tag_dynforward(_tmp20D,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp210=({
struct Cyc_String_pa_struct _tmp214;_tmp214.tag=0;_tmp214.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp213;_tmp213.tag=0;_tmp213.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp211[2]={& _tmp213,& _tmp214};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp212="cannot cast %s to %s";_tag_dynforward(
_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,21));}),_tag_dynforward(_tmp211,
sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp210;}}}}{struct
_tuple0 _tmp216=({struct _tuple0 _tmp215;_tmp215.f1=(void*)e->r;_tmp215.f2=Cyc_Tcutil_compress(
t);_tmp215;});void*_tmp217;struct Cyc_Absyn_MallocInfo _tmp218;int _tmp219;void*
_tmp21A;struct Cyc_Absyn_PtrInfo _tmp21B;struct Cyc_Absyn_PtrAtts _tmp21C;struct Cyc_Absyn_Conref*
_tmp21D;struct Cyc_Absyn_Conref*_tmp21E;struct Cyc_Absyn_Conref*_tmp21F;_LL149:
_tmp217=_tmp216.f1;if(*((int*)_tmp217)!= 35)goto _LL14B;_tmp218=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp217)->f1;_tmp219=_tmp218.fat_result;_tmp21A=_tmp216.f2;if(_tmp21A <= (void*)4)
goto _LL14B;if(*((int*)_tmp21A)!= 4)goto _LL14B;_tmp21B=((struct Cyc_Absyn_PointerType_struct*)
_tmp21A)->f1;_tmp21C=_tmp21B.ptr_atts;_tmp21D=_tmp21C.nullable;_tmp21E=_tmp21C.bounds;
_tmp21F=_tmp21C.zero_term;_LL14A: if((_tmp219  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp21F)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp21D)){void*_tmp220=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp21E);struct Cyc_Absyn_Exp*_tmp221;_LL14E: if(_tmp220 <= (void*)2)goto _LL150;if(*((
int*)_tmp220)!= 0)goto _LL150;_tmp221=((struct Cyc_Absyn_Upper_b_struct*)_tmp220)->f1;
_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp221)).f1 == 1)({void*_tmp222[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp223="cast from ? pointer to * pointer will lose size information";
_tag_dynforward(_tmp223,sizeof(char),_get_zero_arr_size(_tmp223,60));}),
_tag_dynforward(_tmp222,sizeof(void*),0));});goto _LL14D;_LL150:;_LL151: goto
_LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto _LL148;_LL148:;}return t;}}static
void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp224=0;struct
Cyc_Absyn_Tqual _tmp225=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp226=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp227;void*_tmp228;struct Cyc_Absyn_Tqual _tmp229;
struct Cyc_Absyn_PtrAtts _tmp22A;struct Cyc_Absyn_Conref*_tmp22B;_LL153: if(_tmp226
<= (void*)4)goto _LL155;if(*((int*)_tmp226)!= 4)goto _LL155;_tmp227=((struct Cyc_Absyn_PointerType_struct*)
_tmp226)->f1;_tmp228=(void*)_tmp227.elt_typ;_tmp229=_tmp227.elt_tq;_tmp22A=
_tmp227.ptr_atts;_tmp22B=_tmp22A.zero_term;_LL154: _tmp224=({void**_tmp22C=
_cycalloc(sizeof(*_tmp22C));_tmp22C[0]=_tmp228;_tmp22C;});_tmp225=_tmp229;goto
_LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{struct _RegionHandle _tmp22D=
_new_region("r");struct _RegionHandle*r=& _tmp22D;_push_region(r);Cyc_Tcexp_tcExpNoInst(
Cyc_Tcenv_clear_notreadctxt(r,te),_tmp224,e);;_pop_region(r);}{void*_tmp22E=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp22F;struct Cyc_Absyn_Exp*_tmp230;_LL158: if(*((
int*)_tmp22E)!= 25)goto _LL15A;_tmp22F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp22E)->f1;_tmp230=((struct Cyc_Absyn_Subscript_e_struct*)_tmp22E)->f2;_LL159:{
void*_tmp231=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp22F->topt))->v);_LL15D: if(_tmp231 <= (void*)4)goto _LL15F;if(*((int*)_tmp231)
!= 9)goto _LL15F;_LL15E: goto _LL15C;_LL15F:;_LL160:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp22F,_tmp230,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp22F,
_tmp230);_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{int _tmp233;void*
_tmp234;struct _tuple6 _tmp232=Cyc_Tcutil_addressof_props(te,e);_tmp233=_tmp232.f1;
_tmp234=_tmp232.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp233){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp234,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({struct Cyc_String_pa_struct _tmp237;
_tmp237.tag=0;_tmp237.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp235[1]={& _tmp237};Cyc_Tcutil_terr(loc,({const char*_tmp236="sizeof applied to type %s, which has unknown size here";
_tag_dynforward(_tmp236,sizeof(char),_get_zero_arr_size(_tmp236,55));}),
_tag_dynforward(_tmp235,sizeof(void*),1));}});if(topt != 0){void*_tmp238=Cyc_Tcutil_compress(*
topt);_LL162: if(_tmp238 <= (void*)4)goto _LL164;if(*((int*)_tmp238)!= 14)goto
_LL164;_LL163: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp239=_cycalloc(
sizeof(*_tmp239));_tmp239[0]=({struct Cyc_Absyn_SizeofType_struct _tmp23A;_tmp23A.tag=
14;_tmp23A.f1=(void*)t;_tmp23A;});_tmp239;});_LL164:;_LL165: goto _LL161;_LL161:;}
return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(struct
_dynforward_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dynforward_ptr)*n,(struct _dynforward_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp23B=n;struct _dynforward_ptr*_tmp23C;unsigned int _tmp23D;_LL167: if(*((int*)
_tmp23B)!= 0)goto _LL169;_tmp23C=((struct Cyc_Absyn_StructField_struct*)_tmp23B)->f1;
_LL168: {int bad_type=1;{void*_tmp23E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp23F;union Cyc_Absyn_AggrInfoU_union _tmp240;struct Cyc_Absyn_Aggrdecl**_tmp241;
struct Cyc_List_List*_tmp242;_LL16C: if(_tmp23E <= (void*)4)goto _LL170;if(*((int*)
_tmp23E)!= 10)goto _LL16E;_tmp23F=((struct Cyc_Absyn_AggrType_struct*)_tmp23E)->f1;
_tmp240=_tmp23F.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp23E)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL16E;_tmp241=(_tmp240.KnownAggr).f1;_LL16D: if((*_tmp241)->impl == 0)
goto _LL16B;if(!((int(*)(int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp23C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp241)->impl))->fields))({
struct Cyc_String_pa_struct _tmp245;_tmp245.tag=0;_tmp245.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp23C);{void*_tmp243[1]={& _tmp245};
Cyc_Tcutil_terr(loc,({const char*_tmp244="no field of struct/union has name %s";
_tag_dynforward(_tmp244,sizeof(char),_get_zero_arr_size(_tmp244,37));}),
_tag_dynforward(_tmp243,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL16E: if(*((
int*)_tmp23E)!= 11)goto _LL170;_tmp242=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23E)->f2;_LL16F: if(!((int(*)(int(*pred)(struct _dynforward_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dynforward_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp23C,_tmp242))({struct Cyc_String_pa_struct _tmp248;_tmp248.tag=0;_tmp248.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp23C);{void*_tmp246[1]={&
_tmp248};Cyc_Tcutil_terr(loc,({const char*_tmp247="no field of struct/union has name %s";
_tag_dynforward(_tmp247,sizeof(char),_get_zero_arr_size(_tmp247,37));}),
_tag_dynforward(_tmp246,sizeof(void*),1));}});bad_type=0;goto _LL16B;_LL170:;
_LL171: goto _LL16B;_LL16B:;}if(bad_type)({struct Cyc_String_pa_struct _tmp24B;
_tmp24B.tag=0;_tmp24B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp249[1]={& _tmp24B};Cyc_Tcutil_terr(loc,({const char*_tmp24A="%s is not a known struct/union type";
_tag_dynforward(_tmp24A,sizeof(char),_get_zero_arr_size(_tmp24A,36));}),
_tag_dynforward(_tmp249,sizeof(void*),1));}});goto _LL166;}_LL169: if(*((int*)
_tmp23B)!= 1)goto _LL166;_tmp23D=((struct Cyc_Absyn_TupleIndex_struct*)_tmp23B)->f1;
_LL16A: {int bad_type=1;{void*_tmp24C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp24D;union Cyc_Absyn_AggrInfoU_union _tmp24E;struct Cyc_Absyn_Aggrdecl**_tmp24F;
struct Cyc_List_List*_tmp250;struct Cyc_List_List*_tmp251;struct Cyc_Absyn_TunionFieldInfo
_tmp252;union Cyc_Absyn_TunionFieldInfoU_union _tmp253;struct Cyc_Absyn_Tunionfield*
_tmp254;_LL173: if(_tmp24C <= (void*)4)goto _LL17B;if(*((int*)_tmp24C)!= 10)goto
_LL175;_tmp24D=((struct Cyc_Absyn_AggrType_struct*)_tmp24C)->f1;_tmp24E=_tmp24D.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp24C)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL175;_tmp24F=(_tmp24E.KnownAggr).f1;_LL174: if((*_tmp24F)->impl == 0)
goto _LL172;_tmp250=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp24F)->impl))->fields;
goto _LL176;_LL175: if(*((int*)_tmp24C)!= 11)goto _LL177;_tmp250=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp24C)->f2;_LL176: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp250)
<= _tmp23D)({struct Cyc_Int_pa_struct _tmp258;_tmp258.tag=1;_tmp258.f1=(
unsigned long)((int)_tmp23D);{struct Cyc_Int_pa_struct _tmp257;_tmp257.tag=1;
_tmp257.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp250);{void*_tmp255[2]={& _tmp257,& _tmp258};Cyc_Tcutil_terr(loc,({const char*
_tmp256="struct/union has too few components: %d <= %d";_tag_dynforward(_tmp256,
sizeof(char),_get_zero_arr_size(_tmp256,46));}),_tag_dynforward(_tmp255,sizeof(
void*),2));}}});bad_type=0;goto _LL172;_LL177: if(*((int*)_tmp24C)!= 9)goto _LL179;
_tmp251=((struct Cyc_Absyn_TupleType_struct*)_tmp24C)->f1;_LL178: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp251)<= _tmp23D)({struct Cyc_Int_pa_struct
_tmp25C;_tmp25C.tag=1;_tmp25C.f1=(unsigned long)((int)_tmp23D);{struct Cyc_Int_pa_struct
_tmp25B;_tmp25B.tag=1;_tmp25B.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp251);{void*_tmp259[2]={& _tmp25B,& _tmp25C};Cyc_Tcutil_terr(
loc,({const char*_tmp25A="tuple has too few components: %d <= %d";_tag_dynforward(
_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,39));}),_tag_dynforward(_tmp259,
sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL179: if(*((int*)_tmp24C)!= 3)goto
_LL17B;_tmp252=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp24C)->f1;_tmp253=
_tmp252.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp24C)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL17B;_tmp254=(_tmp253.KnownTunionfield).f2;_LL17A: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp254->typs)< _tmp23D)({struct Cyc_Int_pa_struct
_tmp260;_tmp260.tag=1;_tmp260.f1=(unsigned long)((int)_tmp23D);{struct Cyc_Int_pa_struct
_tmp25F;_tmp25F.tag=1;_tmp25F.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp254->typs);{void*_tmp25D[2]={& _tmp25F,& _tmp260};Cyc_Tcutil_terr(
loc,({const char*_tmp25E="tunionfield has too few components: %d < %d";
_tag_dynforward(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,44));}),
_tag_dynforward(_tmp25D,sizeof(void*),2));}}});bad_type=0;goto _LL172;_LL17B:;
_LL17C: goto _LL172;_LL172:;}if(bad_type)({struct Cyc_String_pa_struct _tmp263;
_tmp263.tag=0;_tmp263.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp261[1]={& _tmp263};Cyc_Tcutil_terr(loc,({const char*_tmp262="%s is not a known type";
_tag_dynforward(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,23));}),
_tag_dynforward(_tmp261,sizeof(void*),1));}});goto _LL166;}_LL166:;}return Cyc_Absyn_uint_typ;}
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle _tmp264=_new_region("r");
struct _RegionHandle*r=& _tmp264;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp265=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp265,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp266=t;struct Cyc_Absyn_PtrInfo
_tmp267;void*_tmp268;struct Cyc_Absyn_PtrAtts _tmp269;void*_tmp26A;struct Cyc_Absyn_Conref*
_tmp26B;struct Cyc_Absyn_Conref*_tmp26C;_LL17E: if(_tmp266 <= (void*)4)goto _LL180;
if(*((int*)_tmp266)!= 4)goto _LL180;_tmp267=((struct Cyc_Absyn_PointerType_struct*)
_tmp266)->f1;_tmp268=(void*)_tmp267.elt_typ;_tmp269=_tmp267.ptr_atts;_tmp26A=(
void*)_tmp269.rgn;_tmp26B=_tmp269.bounds;_tmp26C=_tmp269.zero_term;_LL17F: Cyc_Tcenv_check_rgn_accessible(
_tmp265,loc,_tmp26A);Cyc_Tcutil_check_nonzero_bound(loc,_tmp26B);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp268),(void*)1))({void*_tmp26D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp26E="can't dereference abstract pointer type";_tag_dynforward(
_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,40));}),_tag_dynforward(_tmp26D,
sizeof(void*),0));});{void*_tmp26F=_tmp268;_npop_handler(0);return _tmp26F;}
_LL180:;_LL181: {void*_tmp273=({struct Cyc_String_pa_struct _tmp272;_tmp272.tag=0;
_tmp272.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp270[1]={& _tmp272};Cyc_Tcexp_expr_err(_tmp265,loc,topt,({const char*
_tmp271="expecting * or @ type but found %s";_tag_dynforward(_tmp271,sizeof(char),
_get_zero_arr_size(_tmp271,35));}),_tag_dynforward(_tmp270,sizeof(void*),1));}});
_npop_handler(0);return _tmp273;}_LL17D:;}};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*f){Cyc_Tcexp_tcExpNoPromote(
te,0,e);{void*_tmp274=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp275;union Cyc_Absyn_AggrInfoU_union
_tmp276;struct Cyc_Absyn_Aggrdecl**_tmp277;struct Cyc_Absyn_Aggrdecl*_tmp278;
struct Cyc_List_List*_tmp279;void*_tmp27A;struct Cyc_List_List*_tmp27B;_LL183: if(
_tmp274 <= (void*)4)goto _LL18B;if(*((int*)_tmp274)!= 10)goto _LL185;_tmp275=((
struct Cyc_Absyn_AggrType_struct*)_tmp274)->f1;_tmp276=_tmp275.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp274)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL185;_tmp277=(_tmp276.KnownAggr).f1;_tmp278=*_tmp277;_tmp279=_tmp275.targs;
_LL184: {struct Cyc_Absyn_Aggrfield*_tmp27E=Cyc_Absyn_lookup_decl_field(_tmp278,f);
if(_tmp27E == 0)return({struct Cyc_String_pa_struct _tmp282;_tmp282.tag=0;_tmp282.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp281;_tmp281.tag=0;_tmp281.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp278->name));{void*_tmp27F[2]={& _tmp281,& _tmp282};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp280="type %s has no %s field";_tag_dynforward(
_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,24));}),_tag_dynforward(_tmp27F,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp278->impl))->exist_vars
!= 0)return({void*_tmp283[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp284="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,64));}),
_tag_dynforward(_tmp283,sizeof(void*),0));});{void*t2;{struct _RegionHandle
_tmp285=_new_region("rgn");struct _RegionHandle*rgn=& _tmp285;_push_region(rgn);{
struct Cyc_List_List*_tmp286=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp278->tvs,_tmp279);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp286,(void*)
_tmp27E->type);if(((void*)_tmp278->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){void*_tmp28A=({struct Cyc_String_pa_struct
_tmp289;_tmp289.tag=0;_tmp289.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp287[1]={& _tmp289};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp288="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,56));}),
_tag_dynforward(_tmp287,sizeof(void*),1));}});_npop_handler(0);return _tmp28A;}};
_pop_region(rgn);}return t2;}}_LL185: if(*((int*)_tmp274)!= 11)goto _LL187;_tmp27A=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp274)->f1;_tmp27B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp274)->f2;_LL186: {struct Cyc_Absyn_Aggrfield*_tmp28B=Cyc_Absyn_lookup_field(
_tmp27B,f);if(_tmp28B == 0)return({struct Cyc_String_pa_struct _tmp28E;_tmp28E.tag=
0;_tmp28E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp28C[1]={&
_tmp28E};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp28D="type has no %s field";
_tag_dynforward(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,21));}),
_tag_dynforward(_tmp28C,sizeof(void*),1));}});if((_tmp27A == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only((void*)_tmp28B->type))return({struct Cyc_String_pa_struct
_tmp291;_tmp291.tag=0;_tmp291.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
f);{void*_tmp28F[1]={& _tmp291};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp290="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,56));}),
_tag_dynforward(_tmp28F,sizeof(void*),1));}});return(void*)_tmp28B->type;}_LL187:
if(*((int*)_tmp274)!= 7)goto _LL189;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({
const char*_tmp27C="size";_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size(
_tmp27C,5));}))== 0))goto _LL189;_LL188: goto _LL18A;_LL189: if(*((int*)_tmp274)!= 4)
goto _LL18B;if(!(Cyc_strcmp((struct _dynforward_ptr)*f,({const char*_tmp27D="size";
_tag_dynforward(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,5));}))== 0))
goto _LL18B;_LL18A:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp293;_tmp293.tag=3;_tmp293.f1=(void*)((void*)19);_tmp293.f2=({struct Cyc_List_List*
_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294->hd=e;_tmp294->tl=0;_tmp294;});
_tmp293;});_tmp292;})));return Cyc_Absyn_uint_typ;_LL18B:;_LL18C: if(Cyc_strcmp((
struct _dynforward_ptr)*f,({const char*_tmp295="size";_tag_dynforward(_tmp295,
sizeof(char),_get_zero_arr_size(_tmp295,5));}))== 0)return({struct Cyc_String_pa_struct
_tmp298;_tmp298.tag=0;_tmp298.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp296[1]={& _tmp298};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp297="expecting struct, union, or array, found %s";
_tag_dynforward(_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,44));}),
_tag_dynforward(_tmp296,sizeof(void*),1));}});else{return({struct Cyc_String_pa_struct
_tmp29B;_tmp29B.tag=0;_tmp29B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp299[1]={& _tmp29B};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp29A="expecting struct or union, found %s";
_tag_dynforward(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,36));}),
_tag_dynforward(_tmp299,sizeof(void*),1));}});}_LL182:;}}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*f){{struct _RegionHandle _tmp29C=_new_region("r");struct
_RegionHandle*r=& _tmp29C;_push_region(r);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
r,te),0,e);;_pop_region(r);}{void*_tmp29D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp29E;void*_tmp29F;struct Cyc_Absyn_PtrAtts
_tmp2A0;void*_tmp2A1;struct Cyc_Absyn_Conref*_tmp2A2;struct Cyc_Absyn_Conref*
_tmp2A3;_LL18E: if(_tmp29D <= (void*)4)goto _LL190;if(*((int*)_tmp29D)!= 4)goto
_LL190;_tmp29E=((struct Cyc_Absyn_PointerType_struct*)_tmp29D)->f1;_tmp29F=(void*)
_tmp29E.elt_typ;_tmp2A0=_tmp29E.ptr_atts;_tmp2A1=(void*)_tmp2A0.rgn;_tmp2A2=
_tmp2A0.bounds;_tmp2A3=_tmp2A0.zero_term;_LL18F: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2A2);{void*_tmp2A4=Cyc_Tcutil_compress(_tmp29F);struct Cyc_Absyn_AggrInfo
_tmp2A5;union Cyc_Absyn_AggrInfoU_union _tmp2A6;struct Cyc_Absyn_Aggrdecl**_tmp2A7;
struct Cyc_Absyn_Aggrdecl*_tmp2A8;struct Cyc_List_List*_tmp2A9;void*_tmp2AA;struct
Cyc_List_List*_tmp2AB;_LL193: if(_tmp2A4 <= (void*)4)goto _LL197;if(*((int*)_tmp2A4)
!= 10)goto _LL195;_tmp2A5=((struct Cyc_Absyn_AggrType_struct*)_tmp2A4)->f1;_tmp2A6=
_tmp2A5.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp2A4)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL195;_tmp2A7=(_tmp2A6.KnownAggr).f1;_tmp2A8=*_tmp2A7;_tmp2A9=_tmp2A5.targs;
_LL194: {struct Cyc_Absyn_Aggrfield*_tmp2AC=Cyc_Absyn_lookup_decl_field(_tmp2A8,f);
if(_tmp2AC == 0)return({struct Cyc_String_pa_struct _tmp2B0;_tmp2B0.tag=0;_tmp2B0.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*f);{struct Cyc_String_pa_struct
_tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp2A8->name));{void*_tmp2AD[2]={& _tmp2AF,& _tmp2B0};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2AE="type %s has no %s field";_tag_dynforward(
_tmp2AE,sizeof(char),_get_zero_arr_size(_tmp2AE,24));}),_tag_dynforward(_tmp2AD,
sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A8->impl))->exist_vars
!= 0)return({void*_tmp2B1[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp2B2="must use pattern-matching to access fields of existential types";
_tag_dynforward(_tmp2B2,sizeof(char),_get_zero_arr_size(_tmp2B2,64));}),
_tag_dynforward(_tmp2B1,sizeof(void*),0));});{void*t3;{struct _RegionHandle
_tmp2B3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2B3;_push_region(rgn);{
struct Cyc_List_List*_tmp2B4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp2A8->tvs,_tmp2A9);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp2B4,(void*)
_tmp2AC->type);};_pop_region(rgn);}if(((void*)_tmp2A8->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t3))return({struct Cyc_String_pa_struct _tmp2B7;
_tmp2B7.tag=0;_tmp2B7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{
void*_tmp2B5[1]={& _tmp2B7};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2B6="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2B6,sizeof(char),_get_zero_arr_size(_tmp2B6,56));}),
_tag_dynforward(_tmp2B5,sizeof(void*),1));}});return t3;}}_LL195: if(*((int*)
_tmp2A4)!= 11)goto _LL197;_tmp2AA=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2A4)->f1;_tmp2AB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2A4)->f2;_LL196: {
struct Cyc_Absyn_Aggrfield*_tmp2B8=Cyc_Absyn_lookup_field(_tmp2AB,f);if(_tmp2B8 == 
0)return({struct Cyc_String_pa_struct _tmp2BB;_tmp2BB.tag=0;_tmp2BB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2B9[1]={& _tmp2BB};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2BA="type has no %s field";_tag_dynforward(_tmp2BA,
sizeof(char),_get_zero_arr_size(_tmp2BA,21));}),_tag_dynforward(_tmp2B9,sizeof(
void*),1));}});if((_tmp2AA == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2B8->type))return({struct Cyc_String_pa_struct _tmp2BE;_tmp2BE.tag=0;
_tmp2BE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp2BC[1]={&
_tmp2BE};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2BD="cannot read union member %s since it is not `bits-only'";
_tag_dynforward(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,56));}),
_tag_dynforward(_tmp2BC,sizeof(void*),1));}});return(void*)_tmp2B8->type;}_LL197:;
_LL198: goto _LL192;_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}return({
struct Cyc_String_pa_struct _tmp2C1;_tmp2C1.tag=0;_tmp2C1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2BF[1]={& _tmp2C1};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2C0="expecting struct or union pointer, found %s";
_tag_dynforward(_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,44));}),
_tag_dynforward(_tmp2BF,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2C3;int _tmp2C4;struct _tuple7 _tmp2C2=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2C3=_tmp2C2.f1;_tmp2C4=_tmp2C2.f2;if(!
_tmp2C4)return({void*_tmp2C5[0]={};Cyc_Tcexp_expr_err(te,loc,0,({const char*
_tmp2C6="tuple projection cannot use sizeof or offsetof";_tag_dynforward(_tmp2C6,
sizeof(char),_get_zero_arr_size(_tmp2C6,47));}),_tag_dynforward(_tmp2C5,sizeof(
void*),0));});{struct _handler_cons _tmp2C7;_push_handler(& _tmp2C7);{int _tmp2C9=0;
if(setjmp(_tmp2C7.handler))_tmp2C9=1;if(!_tmp2C9){{void*_tmp2CA=(*((struct
_tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2C3)).f2;
_npop_handler(0);return _tmp2CA;};_pop_handler();}else{void*_tmp2C8=(void*)
_exn_thrown;void*_tmp2CC=_tmp2C8;_LL19A: if(_tmp2CC != Cyc_List_Nth)goto _LL19C;
_LL19B: return({struct Cyc_Int_pa_struct _tmp2D0;_tmp2D0.tag=1;_tmp2D0.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct
_tmp2CF;_tmp2CF.tag=1;_tmp2CF.f1=(unsigned long)((int)_tmp2C3);{void*_tmp2CD[2]={&
_tmp2CF,& _tmp2D0};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2CE="index is %d but tuple has only %d fields";
_tag_dynforward(_tmp2CE,sizeof(char),_get_zero_arr_size(_tmp2CE,41));}),
_tag_dynforward(_tmp2CD,sizeof(void*),2));}}});_LL19C:;_LL19D:(void)_throw(
_tmp2CC);_LL199:;}}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp2D1=_new_region("r");struct
_RegionHandle*r=& _tmp2D1;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp2D2=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp2D2,0,e1);Cyc_Tcexp_tcExp(_tmp2D2,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2D2,e2)){void*_tmp2D6=({struct Cyc_String_pa_struct
_tmp2D5;_tmp2D5.tag=0;_tmp2D5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2D3[1]={& _tmp2D5};Cyc_Tcexp_expr_err(
_tmp2D2,e2->loc,topt,({const char*_tmp2D4="expecting int subscript, found %s";
_tag_dynforward(_tmp2D4,sizeof(char),_get_zero_arr_size(_tmp2D4,34));}),
_tag_dynforward(_tmp2D3,sizeof(void*),1));}});_npop_handler(0);return _tmp2D6;}{
void*_tmp2D7=t1;struct Cyc_Absyn_PtrInfo _tmp2D8;void*_tmp2D9;struct Cyc_Absyn_Tqual
_tmp2DA;struct Cyc_Absyn_PtrAtts _tmp2DB;void*_tmp2DC;struct Cyc_Absyn_Conref*
_tmp2DD;struct Cyc_Absyn_Conref*_tmp2DE;struct Cyc_List_List*_tmp2DF;_LL19F: if(
_tmp2D7 <= (void*)4)goto _LL1A3;if(*((int*)_tmp2D7)!= 4)goto _LL1A1;_tmp2D8=((
struct Cyc_Absyn_PointerType_struct*)_tmp2D7)->f1;_tmp2D9=(void*)_tmp2D8.elt_typ;
_tmp2DA=_tmp2D8.elt_tq;_tmp2DB=_tmp2D8.ptr_atts;_tmp2DC=(void*)_tmp2DB.rgn;
_tmp2DD=_tmp2DB.bounds;_tmp2DE=_tmp2DB.zero_term;_LL1A0: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2DE)){int emit_warning=1;{union
Cyc_Absyn_Constraint_union _tmp2E0=(Cyc_Absyn_compress_conref(_tmp2DD))->v;void*
_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;void*_tmp2E3;void*_tmp2E4;_LL1A6: if((_tmp2E0.Eq_constr).tag
!= 0)goto _LL1A8;_tmp2E1=(_tmp2E0.Eq_constr).f1;if(_tmp2E1 <= (void*)2)goto _LL1A8;
if(*((int*)_tmp2E1)!= 0)goto _LL1A8;_tmp2E2=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2E1)->f1;_LL1A7: if(Cyc_Tcutil_is_const_exp(_tmp2D2,e2)){unsigned int _tmp2E6;
int _tmp2E7;struct _tuple7 _tmp2E5=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2E6=
_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;if(_tmp2E7){unsigned int _tmp2E9;int _tmp2EA;struct
_tuple7 _tmp2E8=Cyc_Evexp_eval_const_uint_exp(_tmp2E2);_tmp2E9=_tmp2E8.f1;_tmp2EA=
_tmp2E8.f2;if(_tmp2EA  && _tmp2E9 > _tmp2E6)emit_warning=0;}}goto _LL1A5;_LL1A8: if((
_tmp2E0.Eq_constr).tag != 0)goto _LL1AA;_tmp2E3=(_tmp2E0.Eq_constr).f1;if((int)
_tmp2E3 != 0)goto _LL1AA;_LL1A9: emit_warning=0;goto _LL1A5;_LL1AA: if((_tmp2E0.Eq_constr).tag
!= 0)goto _LL1AC;_tmp2E4=(_tmp2E0.Eq_constr).f1;if((int)_tmp2E4 != 1)goto _LL1AC;
_LL1AB: emit_warning=0;goto _LL1A5;_LL1AC:;_LL1AD: goto _LL1A5;_LL1A5:;}if(
emit_warning)({void*_tmp2EB[0]={};Cyc_Tcutil_warn(e2->loc,({const char*_tmp2EC="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dynforward(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,63));}),
_tag_dynforward(_tmp2EB,sizeof(void*),0));});}else{if(Cyc_Tcutil_is_const_exp(
_tmp2D2,e2)){unsigned int _tmp2EE;int _tmp2EF;struct _tuple7 _tmp2ED=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp2EE=_tmp2ED.f1;_tmp2EF=_tmp2ED.f2;if(_tmp2EF)Cyc_Tcutil_check_bound(loc,
_tmp2EE,_tmp2DD);}else{if(Cyc_Tcutil_is_bound_one(_tmp2DD) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2DE))({void*_tmp2F0[0]={};
Cyc_Tcutil_warn(e1->loc,({const char*_tmp2F1="subscript applied to pointer to one object";
_tag_dynforward(_tmp2F1,sizeof(char),_get_zero_arr_size(_tmp2F1,43));}),
_tag_dynforward(_tmp2F0,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(loc,
_tmp2DD);}}Cyc_Tcenv_check_rgn_accessible(_tmp2D2,loc,_tmp2DC);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2D9),(void*)1))({void*_tmp2F2[0]={};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp2F3="can't subscript an abstract pointer";_tag_dynforward(_tmp2F3,
sizeof(char),_get_zero_arr_size(_tmp2F3,36));}),_tag_dynforward(_tmp2F2,sizeof(
void*),0));});{void*_tmp2F4=_tmp2D9;_npop_handler(0);return _tmp2F4;}_LL1A1: if(*((
int*)_tmp2D7)!= 9)goto _LL1A3;_tmp2DF=((struct Cyc_Absyn_TupleType_struct*)_tmp2D7)->f1;
_LL1A2: {void*_tmp2F5=Cyc_Tcexp_ithTupleType(_tmp2D2,loc,_tmp2DF,e2);
_npop_handler(0);return _tmp2F5;}_LL1A3:;_LL1A4: {void*_tmp2F9=({struct Cyc_String_pa_struct
_tmp2F8;_tmp2F8.tag=0;_tmp2F8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp2F6[1]={& _tmp2F8};Cyc_Tcexp_expr_err(
_tmp2D2,loc,topt,({const char*_tmp2F7="subscript applied to %s";_tag_dynforward(
_tmp2F7,sizeof(char),_get_zero_arr_size(_tmp2F7,24));}),_tag_dynforward(_tmp2F6,
sizeof(void*),1));}});_npop_handler(0);return _tmp2F9;}_LL19E:;}}};_pop_region(r);}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2FA=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2FB;
_LL1AF: if(_tmp2FA <= (void*)4)goto _LL1B1;if(*((int*)_tmp2FA)!= 9)goto _LL1B1;
_tmp2FB=((struct Cyc_Absyn_TupleType_struct*)_tmp2FA)->f1;_LL1B0: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2FB)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AE;for(0;es != 0;(es=es->tl,_tmp2FB=_tmp2FB->tl)){
void*_tmp2FC=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2FC,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2FB->hd)).f2);fields=({
struct Cyc_List_List*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->hd=({struct
_tuple9*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE->f1=(*((struct _tuple9*)
_tmp2FB->hd)).f1;_tmp2FE->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2FE;});_tmp2FD->tl=fields;_tmp2FD;});}done=
1;goto _LL1AE;_LL1B1:;_LL1B2: goto _LL1AE;_LL1AE:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->hd=({struct _tuple9*_tmp300=
_cycalloc(sizeof(*_tmp300));_tmp300->f1=Cyc_Absyn_empty_tqual(0);_tmp300->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp300;});_tmp2FF->tl=fields;_tmp2FF;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301[0]=({struct Cyc_Absyn_TupleType_struct
_tmp302;_tmp302.tag=9;_tmp302.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp302;});_tmp301;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp303[0]={};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp304="tcCompoundLit";_tag_dynforward(_tmp304,sizeof(char),
_get_zero_arr_size(_tmp304,14));}),_tag_dynforward(_tmp303,sizeof(void*),0));});}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des){void*res_t2;{struct
_RegionHandle _tmp305=_new_region("r");struct _RegionHandle*r=& _tmp305;
_push_region(r);{int _tmp306=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp312;}));struct Cyc_Absyn_Const_e_struct*
_tmp307=({struct Cyc_Absyn_Const_e_struct*_tmp30F=_cycalloc(sizeof(*_tmp30F));
_tmp30F[0]=({struct Cyc_Absyn_Const_e_struct _tmp310;_tmp310.tag=0;_tmp310.f1=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp311;(_tmp311.Int_c).tag=
2;(_tmp311.Int_c).f1=(void*)((void*)1);(_tmp311.Int_c).f2=_tmp306;_tmp311;});
_tmp310;});_tmp30F;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp307,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp308=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp306 - 1);if(!Cyc_Tcutil_is_zero(
_tmp308))({void*_tmp309[0]={};Cyc_Tcutil_terr(_tmp308->loc,({const char*_tmp30A="zero-terminated array doesn't end with zero.";
_tag_dynforward(_tmp30A,sizeof(char),_get_zero_arr_size(_tmp30A,45));}),
_tag_dynforward(_tmp309,sizeof(void*),0));});}sz_exp->topt=({struct Cyc_Core_Opt*
_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B->v=(void*)Cyc_Absyn_uint_typ;_tmp30B;});
res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es))({struct Cyc_String_pa_struct
_tmp30E;_tmp30E.tag=0;_tmp30E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(res));{void*_tmp30C[1]={& _tmp30E};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp30D="elements of array do not all have the same type (%s)";
_tag_dynforward(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,53));}),
_tag_dynforward(_tmp30C,sizeof(void*),1));}});};_pop_region(r);}{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple8*)
des->hd)).f1;if(ds != 0){void*_tmp313=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp314;
_LL1B4: if(*((int*)_tmp313)!= 1)goto _LL1B6;_LL1B5:({void*_tmp315[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp316="only array index designators are supported";
_tag_dynforward(_tmp316,sizeof(char),_get_zero_arr_size(_tmp316,43));}),
_tag_dynforward(_tmp315,sizeof(void*),0));});goto _LL1B3;_LL1B6: if(*((int*)
_tmp313)!= 0)goto _LL1B3;_tmp314=((struct Cyc_Absyn_ArrayElement_struct*)_tmp313)->f1;
_LL1B7: Cyc_Tcexp_tcExpInitializer(te,0,_tmp314);{unsigned int _tmp318;int _tmp319;
struct _tuple7 _tmp317=Cyc_Evexp_eval_const_uint_exp(_tmp314);_tmp318=_tmp317.f1;
_tmp319=_tmp317.f2;if(!_tmp319)({void*_tmp31A[0]={};Cyc_Tcutil_terr(_tmp314->loc,({
const char*_tmp31B="index designator cannot use sizeof or offsetof";
_tag_dynforward(_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,47));}),
_tag_dynforward(_tmp31A,sizeof(void*),0));});else{if(_tmp318 != offset)({struct
Cyc_Int_pa_struct _tmp31F;_tmp31F.tag=1;_tmp31F.f1=(unsigned long)((int)_tmp318);{
struct Cyc_Int_pa_struct _tmp31E;_tmp31E.tag=1;_tmp31E.f1=(unsigned long)offset;{
void*_tmp31C[2]={& _tmp31E,& _tmp31F};Cyc_Tcutil_terr(_tmp314->loc,({const char*
_tmp31D="expecting index designator %d but found %d";_tag_dynforward(_tmp31D,
sizeof(char),_get_zero_arr_size(_tmp31D,43));}),_tag_dynforward(_tmp31C,sizeof(
void*),2));}}});}goto _LL1B3;}_LL1B3:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp320=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1B9: if(_tmp320 <= (void*)4)goto _LL1BB;if(*((int*)
_tmp320)!= 18)goto _LL1BB;_LL1BA: goto _LL1B8;_LL1BB:;_LL1BC: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp323;_tmp323.tag=0;_tmp323.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(bound->topt))->v));{void*_tmp321[1]={& _tmp323};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp322="expecting unsigned int, found %s";
_tag_dynforward(_tmp322,sizeof(char),_get_zero_arr_size(_tmp322,33));}),
_tag_dynforward(_tmp321,sizeof(void*),1));}});_LL1B8:;}if(!(vd->tq).real_const)({
void*_tmp324[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp325="comprehension index variable is not declared const!";
_tag_dynforward(_tmp325,sizeof(char),_get_zero_arr_size(_tmp325,52));}),
_tag_dynforward(_tmp324,sizeof(void*),0));});{struct _RegionHandle _tmp326=
_new_region("r");struct _RegionHandle*r=& _tmp326;_push_region(r);{struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp327=te2;
struct Cyc_Absyn_PtrInfo pinfo;void**_tmp328=0;struct Cyc_Absyn_Tqual*_tmp329=0;
struct Cyc_Absyn_Conref**_tmp32A=0;if(topt != 0){void*_tmp32B=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp32C;struct Cyc_Absyn_ArrayInfo _tmp32D;void*
_tmp32E;void**_tmp32F;struct Cyc_Absyn_Tqual _tmp330;struct Cyc_Absyn_Tqual*_tmp331;
struct Cyc_Absyn_Exp*_tmp332;struct Cyc_Absyn_Conref*_tmp333;struct Cyc_Absyn_Conref**
_tmp334;_LL1BE: if(_tmp32B <= (void*)4)goto _LL1C2;if(*((int*)_tmp32B)!= 4)goto
_LL1C0;_tmp32C=((struct Cyc_Absyn_PointerType_struct*)_tmp32B)->f1;_LL1BF: pinfo=
_tmp32C;_tmp328=(void**)((void**)((void*)& pinfo.elt_typ));_tmp329=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp32A=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1BD;_LL1C0: if(*((int*)_tmp32B)!= 7)goto _LL1C2;_tmp32D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp32B)->f1;_tmp32E=(void*)_tmp32D.elt_type;_tmp32F=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp32B)->f1).elt_type;_tmp330=_tmp32D.tq;_tmp331=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp32B)->f1).tq;_tmp332=_tmp32D.num_elts;
_tmp333=_tmp32D.zero_term;_tmp334=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp32B)->f1).zero_term;_LL1C1: _tmp328=(void**)_tmp32F;_tmp329=(struct Cyc_Absyn_Tqual*)
_tmp331;_tmp32A=(struct Cyc_Absyn_Conref**)_tmp334;goto _LL1BD;_LL1C2:;_LL1C3: goto
_LL1BD;_LL1BD:;}{void*t=Cyc_Tcexp_tcExp(_tmp327,_tmp328,body);if(_tmp327->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp327,bound))({void*_tmp335[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp336="bound is not constant";_tag_dynforward(_tmp336,
sizeof(char),_get_zero_arr_size(_tmp336,22));}),_tag_dynforward(_tmp335,sizeof(
void*),0));});if(!Cyc_Tcutil_is_const_exp(_tmp327,body))({void*_tmp337[0]={};Cyc_Tcutil_terr(
bound->loc,({const char*_tmp338="body is not constant";_tag_dynforward(_tmp338,
sizeof(char),_get_zero_arr_size(_tmp338,21));}),_tag_dynforward(_tmp337,sizeof(
void*),0));});}if(_tmp32A != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,*_tmp32A)){struct Cyc_Absyn_Exp*_tmp339=Cyc_Absyn_uint_exp(1,0);_tmp339->topt=({
struct Cyc_Core_Opt*_tmp33A=_cycalloc(sizeof(*_tmp33A));_tmp33A->v=(void*)Cyc_Absyn_uint_typ;
_tmp33A;});bound=Cyc_Absyn_add_exp(bound,_tmp339,0);bound->topt=({struct Cyc_Core_Opt*
_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->v=(void*)Cyc_Absyn_uint_typ;_tmp33B;});*
is_zero_term=1;}{void*_tmp33C=Cyc_Absyn_array_typ(t,_tmp329 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp329,(struct Cyc_Absyn_Exp*)bound,_tmp32A == 0?Cyc_Absyn_false_conref:*
_tmp32A,0);_npop_handler(0);return _tmp33C;}}}};_pop_region(r);}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp33D;_push_handler(& _tmp33D);{
int _tmp33F=0;if(setjmp(_tmp33D.handler))_tmp33F=1;if(!_tmp33F){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp33E=(void*)_exn_thrown;void*_tmp341=
_tmp33E;_LL1C5: if(_tmp341 != Cyc_Dict_Absent)goto _LL1C7;_LL1C6:({struct Cyc_String_pa_struct
_tmp344;_tmp344.tag=0;_tmp344.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp342[1]={& _tmp344};Cyc_Tcutil_terr(loc,({
const char*_tmp343="unbound struct name %s";_tag_dynforward(_tmp343,sizeof(char),
_get_zero_arr_size(_tmp343,23));}),_tag_dynforward(_tmp342,sizeof(void*),1));}});
return topt != 0?*topt:(void*)0;_LL1C7:;_LL1C8:(void)_throw(_tmp341);_LL1C4:;}}}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({
void*_tmp345[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp346="expecting struct but found union";
_tag_dynforward(_tmp346,sizeof(char),_get_zero_arr_size(_tmp346,33));}),
_tag_dynforward(_tmp345,sizeof(void*),0));});if(ad->impl == 0){({void*_tmp347[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp348="can't build abstract struct";
_tag_dynforward(_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,28));}),
_tag_dynforward(_tmp347,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp349;}));}{struct _RegionHandle _tmp34A=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp34A;_push_region(rgn);{struct _tuple4 _tmp34B=({struct _tuple4 _tmp36B;
_tmp36B.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp36B.f2=rgn;_tmp36B;});struct Cyc_List_List*
_tmp34C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp34B,ad->tvs);struct Cyc_List_List*_tmp34D=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp34B,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp34E=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp34C);struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp34D);struct Cyc_List_List*_tmp350=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp34C,_tmp34D);struct Cyc_Absyn_AggrType_struct*_tmp351=({struct Cyc_Absyn_AggrType_struct*
_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366[0]=({struct Cyc_Absyn_AggrType_struct
_tmp367;_tmp367.tag=10;_tmp367.f1=({struct Cyc_Absyn_AggrInfo _tmp368;_tmp368.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp369;(_tmp369.KnownAggr).tag=
1;(_tmp369.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp36A=_cycalloc(sizeof(*
_tmp36A));_tmp36A[0]=ad;_tmp36A;});_tmp369;});_tmp368.targs=_tmp34E;_tmp368;});
_tmp367;});_tmp366;});struct Cyc_List_List*_tmp352=*ts;struct Cyc_List_List*
_tmp353=_tmp34F;while(_tmp352 != 0  && _tmp353 != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp352->hd);Cyc_Tcutil_unify((
void*)_tmp352->hd,(void*)_tmp353->hd);_tmp352=_tmp352->tl;_tmp353=_tmp353->tl;}
if(_tmp352 != 0)({void*_tmp354[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp355="too many explicit witness types";
_tag_dynforward(_tmp355,sizeof(char),_get_zero_arr_size(_tmp355,32));}),
_tag_dynforward(_tmp354,sizeof(void*),0));});*ts=_tmp34F;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp351,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);for(0;fields != 0;
fields=fields->tl){struct _tuple10 _tmp357;struct Cyc_Absyn_Aggrfield*_tmp358;
struct Cyc_Absyn_Exp*_tmp359;struct _tuple10*_tmp356=(struct _tuple10*)fields->hd;
_tmp357=*_tmp356;_tmp358=_tmp357.f1;_tmp359=_tmp357.f2;{void*_tmp35A=Cyc_Tcutil_rsubstitute(
rgn,_tmp350,(void*)_tmp358->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp35A,
_tmp359);if(!Cyc_Tcutil_coerce_arg(te,_tmp359,_tmp35A)){({struct Cyc_String_pa_struct
_tmp360;_tmp360.tag=0;_tmp360.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp359->topt))->v));{
struct Cyc_String_pa_struct _tmp35F;_tmp35F.tag=0;_tmp35F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp35A));{struct
Cyc_String_pa_struct _tmp35E;_tmp35E.tag=0;_tmp35E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp35D;_tmp35D.tag=0;_tmp35D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp358->name);{void*_tmp35B[4]={& _tmp35D,& _tmp35E,& _tmp35F,& _tmp360};Cyc_Tcutil_terr(
_tmp359->loc,({const char*_tmp35C="field %s of struct %s expects type %s != %s";
_tag_dynforward(_tmp35C,sizeof(char),_get_zero_arr_size(_tmp35C,44));}),
_tag_dynforward(_tmp35B,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
struct Cyc_List_List*_tmp361=0;{struct Cyc_List_List*_tmp362=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp362 != 0;_tmp362=_tmp362->tl){_tmp361=({
struct Cyc_List_List*_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363->hd=({struct
_tuple0*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364->f1=Cyc_Tcutil_rsubstitute(
rgn,_tmp350,(*((struct _tuple0*)_tmp362->hd)).f1);_tmp364->f2=Cyc_Tcutil_rsubstitute(
rgn,_tmp350,(*((struct _tuple0*)_tmp362->hd)).f2);_tmp364;});_tmp363->tl=_tmp361;
_tmp363;});}}_tmp361=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp361);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp361);{void*_tmp365=(void*)
_tmp351;_npop_handler(0);return _tmp365;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){{struct _RegionHandle _tmp36C=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp36C;_push_region(rgn);{void*_tmp36D=Cyc_Tcutil_compress(ts);void*_tmp36E;
struct Cyc_List_List*_tmp36F;_LL1CA: if(_tmp36D <= (void*)4)goto _LL1CC;if(*((int*)
_tmp36D)!= 11)goto _LL1CC;_tmp36E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp36D)->f1;_tmp36F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp36D)->f2;_LL1CB:
if(_tmp36E == (void*)1)({void*_tmp370[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp371="expecting struct but found union";_tag_dynforward(_tmp371,sizeof(char),
_get_zero_arr_size(_tmp371,33));}),_tag_dynforward(_tmp370,sizeof(void*),0));});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,_tmp36F);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp373;struct Cyc_Absyn_Aggrfield*
_tmp374;struct Cyc_Absyn_Exp*_tmp375;struct _tuple10*_tmp372=(struct _tuple10*)
fields->hd;_tmp373=*_tmp372;_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp374->type)),_tmp375);if(!Cyc_Tcutil_coerce_arg(
te,_tmp375,(void*)_tmp374->type)){({struct Cyc_String_pa_struct _tmp37A;_tmp37A.tag=
0;_tmp37A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp375->topt))->v));{struct Cyc_String_pa_struct
_tmp379;_tmp379.tag=0;_tmp379.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)_tmp374->type));{struct Cyc_String_pa_struct _tmp378;
_tmp378.tag=0;_tmp378.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp374->name);{
void*_tmp376[3]={& _tmp378,& _tmp379,& _tmp37A};Cyc_Tcutil_terr(_tmp375->loc,({
const char*_tmp377="field %s of struct expects type %s != %s";_tag_dynforward(
_tmp377,sizeof(char),_get_zero_arr_size(_tmp377,41));}),_tag_dynforward(_tmp376,
sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1C9;}_LL1CC:;
_LL1CD:({void*_tmp37B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp37C="tcAnonStruct: wrong type";
_tag_dynforward(_tmp37C,sizeof(char),_get_zero_arr_size(_tmp37C,25));}),
_tag_dynforward(_tmp37B,sizeof(void*),0));});_LL1C9:;};_pop_region(rgn);}return
ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp37D=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp37D;_push_region(rgn);{struct _tuple4 _tmp37E=({
struct _tuple4 _tmp39E;_tmp39E.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp39E.f2=rgn;
_tmp39E;});struct Cyc_List_List*_tmp37F=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp37E,tud->tvs);struct Cyc_List_List*_tmp380=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp37F);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp39B;_tmp39B.tag=3;_tmp39B.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp39C;
_tmp39C.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({union Cyc_Absyn_TunionFieldInfoU_union
_tmp39D;(_tmp39D.KnownTunionfield).tag=1;(_tmp39D.KnownTunionfield).f1=tud;(
_tmp39D.KnownTunionfield).f2=tuf;_tmp39D;});_tmp39C.targs=_tmp380;_tmp39C;});
_tmp39B;});_tmp39A;});if(topt != 0){void*_tmp381=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_TunionInfo _tmp382;struct Cyc_List_List*_tmp383;struct Cyc_Core_Opt*
_tmp384;_LL1CF: if(_tmp381 <= (void*)4)goto _LL1D3;if(*((int*)_tmp381)!= 3)goto
_LL1D1;_LL1D0: Cyc_Tcutil_unify(*topt,res);goto _LL1CE;_LL1D1: if(*((int*)_tmp381)
!= 2)goto _LL1D3;_tmp382=((struct Cyc_Absyn_TunionType_struct*)_tmp381)->f1;
_tmp383=_tmp382.targs;_tmp384=_tmp382.rgn;_LL1D2:{struct Cyc_List_List*a=_tmp380;
for(0;a != 0  && _tmp383 != 0;(a=a->tl,_tmp383=_tmp383->tl)){Cyc_Tcutil_unify((void*)
a->hd,(void*)_tmp383->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){e->topt=({
struct Cyc_Core_Opt*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->v=(void*)res;
_tmp385;});res=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp386=_cycalloc(
sizeof(*_tmp386));_tmp386[0]=({struct Cyc_Absyn_TunionType_struct _tmp387;_tmp387.tag=
2;_tmp387.f1=({struct Cyc_Absyn_TunionInfo _tmp388;_tmp388.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp389;(_tmp389.KnownTunion).tag=1;(_tmp389.KnownTunion).f1=({
struct Cyc_Absyn_Tuniondecl**_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=tud;
_tmp38A;});_tmp389;});_tmp388.targs=_tmp380;_tmp388.rgn=_tmp384;_tmp388;});
_tmp387;});_tmp386;});Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}goto _LL1CE;
_LL1D3:;_LL1D4: goto _LL1CE;_LL1CE:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 
0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp37F,(*((struct _tuple9*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp38F;_tmp38F.tag=0;_tmp38F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)(e->topt == 0?(struct _dynforward_ptr)({
const char*_tmp390="?";_tag_dynforward(_tmp390,sizeof(char),_get_zero_arr_size(
_tmp390,2));}): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{
struct Cyc_String_pa_struct _tmp38E;_tmp38E.tag=0;_tmp38E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp38D;_tmp38D.tag=0;_tmp38D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp38B[3]={& _tmp38D,& _tmp38E,& _tmp38F};
Cyc_Tcutil_terr(e->loc,({const char*_tmp38C="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_dynforward(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,80));}),
_tag_dynforward(_tmp38B,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp394=({struct Cyc_String_pa_struct _tmp393;_tmp393.tag=0;
_tmp393.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp391[1]={& _tmp393};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)
es->hd)->loc,topt,({const char*_tmp392="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,45));}),
_tag_dynforward(_tmp391,sizeof(void*),1));}});_npop_handler(0);return _tmp394;}
if(ts != 0){void*_tmp398=({struct Cyc_String_pa_struct _tmp397;_tmp397.tag=0;
_tmp397.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp395[1]={& _tmp397};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp396="too few arguments for tunion constructor %s";_tag_dynforward(
_tmp396,sizeof(char),_get_zero_arr_size(_tmp396,44));}),_tag_dynforward(_tmp395,
sizeof(void*),1));}});_npop_handler(0);return _tmp398;}{void*_tmp399=res;
_npop_handler(0);return _tmp399;}}};_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp39F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3A0;
struct Cyc_Absyn_PtrAtts _tmp3A1;struct Cyc_Absyn_Conref*_tmp3A2;struct Cyc_Absyn_ArrayInfo
_tmp3A3;void*_tmp3A4;struct Cyc_List_List*_tmp3A5;struct Cyc_Absyn_AggrInfo _tmp3A6;
union Cyc_Absyn_AggrInfoU_union _tmp3A7;struct Cyc_List_List*_tmp3A8;struct Cyc_List_List*
_tmp3A9;_LL1D6: if((int)_tmp39F != 0)goto _LL1D8;_LL1D7: return 1;_LL1D8: if(_tmp39F <= (
void*)4)goto _LL1E4;if(*((int*)_tmp39F)!= 0)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA:
if(*((int*)_tmp39F)!= 1)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(*((int*)_tmp39F)
!= 2)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(*((int*)_tmp39F)!= 3)goto _LL1E0;
_LL1DF: return 0;_LL1E0: if(*((int*)_tmp39F)!= 4)goto _LL1E2;_tmp3A0=((struct Cyc_Absyn_PointerType_struct*)
_tmp39F)->f1;_tmp3A1=_tmp3A0.ptr_atts;_tmp3A2=_tmp3A1.nullable;_LL1E1: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp3A2);_LL1E2: if(*((
int*)_tmp39F)!= 5)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)_tmp39F != 1)goto
_LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp39F <= (void*)4)goto _LL202;if(*((int*)
_tmp39F)!= 6)goto _LL1E8;_LL1E7: return 1;_LL1E8: if(*((int*)_tmp39F)!= 7)goto _LL1EA;
_tmp3A3=((struct Cyc_Absyn_ArrayType_struct*)_tmp39F)->f1;_tmp3A4=(void*)_tmp3A3.elt_type;
_LL1E9: return Cyc_Tcexp_zeroable_type(_tmp3A4);_LL1EA: if(*((int*)_tmp39F)!= 8)
goto _LL1EC;_LL1EB: return 0;_LL1EC: if(*((int*)_tmp39F)!= 9)goto _LL1EE;_tmp3A5=((
struct Cyc_Absyn_TupleType_struct*)_tmp39F)->f1;_LL1ED: for(0;(unsigned int)
_tmp3A5;_tmp3A5=_tmp3A5->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp3A5->hd)).f2))return 0;}return 1;_LL1EE: if(*((int*)_tmp39F)!= 10)goto _LL1F0;
_tmp3A6=((struct Cyc_Absyn_AggrType_struct*)_tmp39F)->f1;_tmp3A7=_tmp3A6.aggr_info;
_tmp3A8=_tmp3A6.targs;_LL1EF: {struct Cyc_Absyn_Aggrdecl*_tmp3AA=Cyc_Absyn_get_known_aggrdecl(
_tmp3A7);if(_tmp3AA->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3AA->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp3AB=_new_region("r");
struct _RegionHandle*r=& _tmp3AB;_push_region(r);{struct Cyc_List_List*_tmp3AC=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp3AA->tvs,_tmp3A8);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AA->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3AC,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3AD=0;_npop_handler(0);
return _tmp3AD;}}}{int _tmp3AE=1;_npop_handler(0);return _tmp3AE;}};_pop_region(r);}}
_LL1F0: if(*((int*)_tmp39F)!= 12)goto _LL1F2;_LL1F1: return 1;_LL1F2: if(*((int*)
_tmp39F)!= 14)goto _LL1F4;_LL1F3: return 1;_LL1F4: if(*((int*)_tmp39F)!= 18)goto
_LL1F6;_LL1F5: return 1;_LL1F6: if(*((int*)_tmp39F)!= 11)goto _LL1F8;_tmp3A9=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp39F)->f2;_LL1F7: for(0;_tmp3A9 != 0;
_tmp3A9=_tmp3A9->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp3A9->hd)->type))return 0;}return 1;_LL1F8: if(*((int*)_tmp39F)!= 13)goto _LL1FA;
_LL1F9: return 1;_LL1FA: if(*((int*)_tmp39F)!= 17)goto _LL1FC;_LL1FB: return 0;_LL1FC:
if(*((int*)_tmp39F)!= 16)goto _LL1FE;_LL1FD: return 0;_LL1FE: if(*((int*)_tmp39F)!= 
15)goto _LL200;_LL1FF: return 0;_LL200: if(*((int*)_tmp39F)!= 19)goto _LL202;_LL201:
goto _LL203;_LL202: if((int)_tmp39F != 2)goto _LL204;_LL203: goto _LL205;_LL204: if((
int)_tmp39F != 3)goto _LL206;_LL205: goto _LL207;_LL206: if(_tmp39F <= (void*)4)goto
_LL208;if(*((int*)_tmp39F)!= 20)goto _LL208;_LL207: goto _LL209;_LL208: if(_tmp39F <= (
void*)4)goto _LL20A;if(*((int*)_tmp39F)!= 21)goto _LL20A;_LL209: goto _LL20B;_LL20A:
if(_tmp39F <= (void*)4)goto _LL1D5;if(*((int*)_tmp39F)!= 22)goto _LL1D5;_LL20B:({
struct Cyc_String_pa_struct _tmp3B1;_tmp3B1.tag=0;_tmp3B1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp3AF[
1]={& _tmp3B1};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3B0="bad type `%s' in zeroable type";_tag_dynforward(_tmp3B0,
sizeof(char),_get_zero_arr_size(_tmp3B0,31));}),_tag_dynforward(_tmp3AF,sizeof(
void*),1));}});_LL1D5:;}static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3B2=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3B3;void*_tmp3B4;_LL20D: if(_tmp3B2 <= (void*)4)
goto _LL20F;if(*((int*)_tmp3B2)!= 4)goto _LL20F;_tmp3B3=((struct Cyc_Absyn_PointerType_struct*)
_tmp3B2)->f1;_tmp3B4=(void*)_tmp3B3.elt_typ;_LL20E: Cyc_Tcutil_unify(_tmp3B4,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL20C;_LL20F:;_LL210: goto _LL20C;_LL20C:;}({struct Cyc_String_pa_struct
_tmp3B8;_tmp3B8.tag=0;_tmp3B8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp3B7;_tmp3B7.tag=0;
_tmp3B7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)(allow_zero?(struct
_dynforward_ptr)({const char*_tmp3B9="calloc";_tag_dynforward(_tmp3B9,sizeof(char),
_get_zero_arr_size(_tmp3B9,7));}):(struct _dynforward_ptr)({const char*_tmp3BA="malloc";
_tag_dynforward(_tmp3BA,sizeof(char),_get_zero_arr_size(_tmp3BA,7));})));{void*
_tmp3B5[2]={& _tmp3B7,& _tmp3B8};Cyc_Tcutil_terr(loc,({const char*_tmp3B6="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dynforward(_tmp3B6,sizeof(char),_get_zero_arr_size(_tmp3B6,60));}),
_tag_dynforward(_tmp3B5,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3C1;_tmp3C1.tag=15;_tmp3C1.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp3C2;}));_tmp3C1;});_tmp3C0;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3BB=Cyc_Tcutil_compress(handle_type);void*_tmp3BC;_LL212: if(_tmp3BB <= (
void*)4)goto _LL214;if(*((int*)_tmp3BB)!= 15)goto _LL214;_tmp3BC=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3BB)->f1;_LL213: rgn=_tmp3BC;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL211;_LL214:;_LL215:({struct Cyc_String_pa_struct _tmp3BF;
_tmp3BF.tag=0;_tmp3BF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3BD[1]={& _tmp3BF};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp3BE="expecting region_t type but found %s";_tag_dynforward(_tmp3BE,sizeof(
char),_get_zero_arr_size(_tmp3BE,37));}),_tag_dynforward(_tmp3BD,sizeof(void*),1));}});
goto _LL211;_LL211:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({void*
_tmp3C3[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3C4="calloc with empty type";_tag_dynforward(_tmp3C4,sizeof(char),
_get_zero_arr_size(_tmp3C4,23));}),_tag_dynforward(_tmp3C3,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3C5=(void*)(*e)->r;void*_tmp3C6;void*
_tmp3C7;struct Cyc_List_List*_tmp3C8;struct Cyc_List_List _tmp3C9;struct Cyc_Absyn_Exp*
_tmp3CA;struct Cyc_List_List*_tmp3CB;struct Cyc_List_List _tmp3CC;struct Cyc_Absyn_Exp*
_tmp3CD;struct Cyc_List_List*_tmp3CE;_LL217: if(*((int*)_tmp3C5)!= 18)goto _LL219;
_tmp3C6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C5)->f1;_LL218:
elt_type=_tmp3C6;*t=(void**)({void**_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[
0]=elt_type;_tmp3CF;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL216;_LL219: if(*((int*)_tmp3C5)!= 3)
goto _LL21B;_tmp3C7=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3C5)->f1;if((
int)_tmp3C7 != 1)goto _LL21B;_tmp3C8=((struct Cyc_Absyn_Primop_e_struct*)_tmp3C5)->f2;
if(_tmp3C8 == 0)goto _LL21B;_tmp3C9=*_tmp3C8;_tmp3CA=(struct Cyc_Absyn_Exp*)_tmp3C9.hd;
_tmp3CB=_tmp3C9.tl;if(_tmp3CB == 0)goto _LL21B;_tmp3CC=*_tmp3CB;_tmp3CD=(struct Cyc_Absyn_Exp*)
_tmp3CC.hd;_tmp3CE=_tmp3CC.tl;if(_tmp3CE != 0)goto _LL21B;_LL21A:{struct _tuple0
_tmp3D1=({struct _tuple0 _tmp3D0;_tmp3D0.f1=(void*)_tmp3CA->r;_tmp3D0.f2=(void*)
_tmp3CD->r;_tmp3D0;});void*_tmp3D2;void*_tmp3D3;void*_tmp3D4;void*_tmp3D5;_LL21E:
_tmp3D2=_tmp3D1.f1;if(*((int*)_tmp3D2)!= 18)goto _LL220;_tmp3D3=(void*)((struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3D2)->f1;_LL21F: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp3D3);elt_type=_tmp3D3;*t=(void**)({void**_tmp3D6=_cycalloc(sizeof(*
_tmp3D6));_tmp3D6[0]=elt_type;_tmp3D6;});num_elts=_tmp3CD;one_elt=0;goto _LL21D;
_LL220: _tmp3D4=_tmp3D1.f2;if(*((int*)_tmp3D4)!= 18)goto _LL222;_tmp3D5=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3D4)->f1;_LL221: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3D5);elt_type=_tmp3D5;*t=(void**)({void**_tmp3D7=_cycalloc(
sizeof(*_tmp3D7));_tmp3D7[0]=elt_type;_tmp3D7;});num_elts=_tmp3CA;one_elt=0;goto
_LL21D;_LL222:;_LL223: goto No_sizeof;_LL21D:;}goto _LL216;_LL21B:;_LL21C: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3D8=_cycalloc(sizeof(*_tmp3D8));
_tmp3D8[0]=elt_type;_tmp3D8;});num_elts=*e;one_elt=0;goto _LL216;_LL216:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3D9=elt_type;struct Cyc_Absyn_AggrInfo _tmp3DA;
union Cyc_Absyn_AggrInfoU_union _tmp3DB;struct Cyc_Absyn_Aggrdecl**_tmp3DC;struct
Cyc_Absyn_Aggrdecl*_tmp3DD;_LL225: if(_tmp3D9 <= (void*)4)goto _LL227;if(*((int*)
_tmp3D9)!= 10)goto _LL227;_tmp3DA=((struct Cyc_Absyn_AggrType_struct*)_tmp3D9)->f1;
_tmp3DB=_tmp3DA.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3D9)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL227;_tmp3DC=(_tmp3DB.KnownAggr).f1;_tmp3DD=*_tmp3DC;_LL226: if(
_tmp3DD->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3DD->impl))->exist_vars
!= 0)({void*_tmp3DE[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3DF="malloc with existential types not yet implemented";
_tag_dynforward(_tmp3DF,sizeof(char),_get_zero_arr_size(_tmp3DF,50));}),
_tag_dynforward(_tmp3DE,sizeof(void*),0));});goto _LL224;_LL227:;_LL228: goto
_LL224;_LL224:;}{void*(*_tmp3E0)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;struct Cyc_Absyn_Conref*_tmp3E1=Cyc_Absyn_false_conref;
if(topt != 0){void*_tmp3E2=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3E3;struct Cyc_Absyn_PtrAtts _tmp3E4;struct Cyc_Absyn_Conref*_tmp3E5;struct Cyc_Absyn_Conref*
_tmp3E6;_LL22A: if(_tmp3E2 <= (void*)4)goto _LL22C;if(*((int*)_tmp3E2)!= 4)goto
_LL22C;_tmp3E3=((struct Cyc_Absyn_PointerType_struct*)_tmp3E2)->f1;_tmp3E4=
_tmp3E3.ptr_atts;_tmp3E5=_tmp3E4.nullable;_tmp3E6=_tmp3E4.zero_term;_LL22B:
_tmp3E1=_tmp3E6;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3E5))_tmp3E0=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3E6) && !(*is_calloc)){({void*_tmp3E7[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp3E8="converting malloc to calloc to ensure zero-termination";
_tag_dynforward(_tmp3E8,sizeof(char),_get_zero_arr_size(_tmp3E8,55));}),
_tag_dynforward(_tmp3E7,sizeof(void*),0));});*is_calloc=1;}goto _LL229;_LL22C:;
_LL22D: goto _LL229;_LL229:;}if(!one_elt)_tmp3E0=Cyc_Absyn_dynforward_typ;return
_tmp3E0(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3E1);}}}static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3E9=Cyc_Tcutil_compress(t1);_LL22F: if(
_tmp3E9 <= (void*)4)goto _LL231;if(*((int*)_tmp3E9)!= 7)goto _LL231;_LL230:({void*
_tmp3EA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3EB="cannot assign to an array";
_tag_dynforward(_tmp3EB,sizeof(char),_get_zero_arr_size(_tmp3EB,26));}),
_tag_dynforward(_tmp3EA,sizeof(void*),0));});goto _LL22E;_LL231:;_LL232: goto
_LL22E;_LL22E:;}{int ign_1=0;int ign_2=0;if(!Cyc_Tcutil_is_pointer_or_boxed(t1,&
ign_1,& ign_2))({void*_tmp3EC[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3ED="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dynforward(_tmp3ED,sizeof(char),_get_zero_arr_size(_tmp3ED,58));}),
_tag_dynforward(_tmp3EC,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e1))return({
void*_tmp3EE[0]={};Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp3EF="swap non-lvalue";
_tag_dynforward(_tmp3EF,sizeof(char),_get_zero_arr_size(_tmp3EF,16));}),
_tag_dynforward(_tmp3EE,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(e2))return({
void*_tmp3F0[0]={};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp3F1="swap non-lvalue";
_tag_dynforward(_tmp3F1,sizeof(char),_get_zero_arr_size(_tmp3F1,16));}),
_tag_dynforward(_tmp3F0,sizeof(void*),0));});{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;int b_ign2=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,&
b_ign1,& b_ign2,& t_ign2))return({void*_tmp3F2[0]={};Cyc_Tcexp_expr_err(te,e1->loc,
topt,({const char*_tmp3F3="swap value in zeroterm array";_tag_dynforward(_tmp3F3,
sizeof(char),_get_zero_arr_size(_tmp3F3,29));}),_tag_dynforward(_tmp3F2,sizeof(
void*),0));});if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& b_ign2,& t_ign2))
return({void*_tmp3F4[0]={};Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp3F5="swap value in zeroterm array";_tag_dynforward(_tmp3F5,sizeof(char),
_get_zero_arr_size(_tmp3F5,29));}),_tag_dynforward(_tmp3F4,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(
t1,t2)){void*_tmp3F6=({struct Cyc_String_pa_struct _tmp3FA;_tmp3FA.tag=0;_tmp3FA.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp3F9;_tmp3F9.tag=0;_tmp3F9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp3F7[2]={& _tmp3F9,& _tmp3FA};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp3F8="type mismatch: %s != %s";_tag_dynforward(_tmp3F8,sizeof(char),
_get_zero_arr_size(_tmp3F8,24));}),_tag_dynforward(_tmp3F7,sizeof(void*),2));}}});
return _tmp3F6;}return(void*)0;}}}}static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*s){{struct
_RegionHandle _tmp3FB=_new_region("r");struct _RegionHandle*r=& _tmp3FB;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);;
_pop_region(r);}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp3FC=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Stmt*
_tmp3FE;struct Cyc_Absyn_Stmt*_tmp3FF;struct Cyc_Absyn_Decl*_tmp400;struct Cyc_Absyn_Stmt*
_tmp401;_LL234: if(_tmp3FC <= (void*)1)goto _LL23A;if(*((int*)_tmp3FC)!= 0)goto
_LL236;_tmp3FD=((struct Cyc_Absyn_Exp_s_struct*)_tmp3FC)->f1;_LL235: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3FD->topt))->v;_LL236: if(*((int*)_tmp3FC)!= 
1)goto _LL238;_tmp3FE=((struct Cyc_Absyn_Seq_s_struct*)_tmp3FC)->f1;_tmp3FF=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3FC)->f2;_LL237: s=_tmp3FF;continue;_LL238: if(*((
int*)_tmp3FC)!= 11)goto _LL23A;_tmp400=((struct Cyc_Absyn_Decl_s_struct*)_tmp3FC)->f1;
_tmp401=((struct Cyc_Absyn_Decl_s_struct*)_tmp3FC)->f2;_LL239: s=_tmp401;continue;
_LL23A:;_LL23B: return({void*_tmp402[0]={};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp403="statement expression must end with expression";_tag_dynforward(
_tmp403,sizeof(char),_get_zero_arr_size(_tmp403,46));}),_tag_dynforward(_tmp402,
sizeof(void*),0));});_LL233:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(rgn_handle != 
0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp409=
_cycalloc(sizeof(*_tmp409));_tmp409[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp40A;_tmp40A.tag=15;_tmp40A.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp40B;}));_tmp40A;});_tmp409;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp404=Cyc_Tcutil_compress(handle_type);void*_tmp405;_LL23D:
if(_tmp404 <= (void*)4)goto _LL23F;if(*((int*)_tmp404)!= 15)goto _LL23F;_tmp405=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp404)->f1;_LL23E: rgn=_tmp405;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL23C;_LL23F:;_LL240:({struct Cyc_String_pa_struct
_tmp408;_tmp408.tag=0;_tmp408.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp406[1]={& _tmp408};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp407="expecting region_t type but found %s";
_tag_dynforward(_tmp407,sizeof(char),_get_zero_arr_size(_tmp407,37));}),
_tag_dynforward(_tmp406,sizeof(void*),1));}});goto _LL23C;_LL23C:;}{void*_tmp40C=(
void*)e1->r;struct Cyc_Core_Opt*_tmp40D;struct Cyc_List_List*_tmp40E;struct Cyc_List_List*
_tmp40F;union Cyc_Absyn_Cnst_union _tmp410;struct _dynforward_ptr _tmp411;_LL242: if(*((
int*)_tmp40C)!= 29)goto _LL244;_LL243: {void*_tmp412=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp413=Cyc_Tcutil_compress(_tmp412);struct Cyc_Absyn_ArrayInfo
_tmp414;void*_tmp415;struct Cyc_Absyn_Tqual _tmp416;struct Cyc_Absyn_Exp*_tmp417;
struct Cyc_Absyn_Conref*_tmp418;_LL24D: if(_tmp413 <= (void*)4)goto _LL24F;if(*((int*)
_tmp413)!= 7)goto _LL24F;_tmp414=((struct Cyc_Absyn_ArrayType_struct*)_tmp413)->f1;
_tmp415=(void*)_tmp414.elt_type;_tmp416=_tmp414.tq;_tmp417=_tmp414.num_elts;
_tmp418=_tmp414.zero_term;_LL24E: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp417);void*b;{void*_tmp419=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bnd->topt))->v);void*_tmp41A;_LL252: if(_tmp419 <= (void*)4)goto _LL254;
if(*((int*)_tmp419)!= 18)goto _LL254;_tmp41A=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp419)->f1;_LL253: b=(void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp41B=
_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp41C;_tmp41C.tag=1;_tmp41C.f1=(void*)_tmp41A;_tmp41C;});_tmp41B;});goto _LL251;
_LL254:;_LL255: if(Cyc_Tcutil_is_const_exp(te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp41E;_tmp41E.tag=0;_tmp41E.f1=bnd;_tmp41E;});_tmp41D;});else{b=(void*)0;}
_LL251:;}{void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp420=
_cycalloc(sizeof(*_tmp420));_tmp420[0]=({struct Cyc_Absyn_PointerType_struct
_tmp421;_tmp421.tag=4;_tmp421.f1=({struct Cyc_Absyn_PtrInfo _tmp422;_tmp422.elt_typ=(
void*)_tmp415;_tmp422.elt_tq=_tmp416;_tmp422.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp423;_tmp423.rgn=(void*)rgn;_tmp423.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp423.bounds=Cyc_Absyn_new_conref(b);_tmp423.zero_term=
_tmp418;_tmp423.ptrloc=0;_tmp423;});_tmp422;});_tmp421;});_tmp420;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->v=(
void*)res_typ;_tmp41F;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL24F:;_LL250:({void*_tmp424[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp425="tcNew: comprehension returned non-array type";_tag_dynforward(_tmp425,
sizeof(char),_get_zero_arr_size(_tmp425,45));}),_tag_dynforward(_tmp424,sizeof(
void*),0));});_LL24C:;}_LL244: if(*((int*)_tmp40C)!= 37)goto _LL246;_tmp40D=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp40C)->f1;_tmp40E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp40C)->f2;_LL245:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426[0]=({struct Cyc_Absyn_Array_e_struct
_tmp427;_tmp427.tag=28;_tmp427.f1=_tmp40E;_tmp427;});_tmp426;})));_tmp40F=
_tmp40E;goto _LL247;_LL246: if(*((int*)_tmp40C)!= 28)goto _LL248;_tmp40F=((struct
Cyc_Absyn_Array_e_struct*)_tmp40C)->f1;_LL247: {void**elt_typ_opt=0;int zero_term=
0;if(topt != 0){void*_tmp428=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp429;void*_tmp42A;void**_tmp42B;struct Cyc_Absyn_Tqual _tmp42C;struct Cyc_Absyn_PtrAtts
_tmp42D;struct Cyc_Absyn_Conref*_tmp42E;_LL257: if(_tmp428 <= (void*)4)goto _LL259;
if(*((int*)_tmp428)!= 4)goto _LL259;_tmp429=((struct Cyc_Absyn_PointerType_struct*)
_tmp428)->f1;_tmp42A=(void*)_tmp429.elt_typ;_tmp42B=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp428)->f1).elt_typ;_tmp42C=_tmp429.elt_tq;_tmp42D=_tmp429.ptr_atts;_tmp42E=
_tmp42D.zero_term;_LL258: elt_typ_opt=(void**)_tmp42B;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp42E);goto _LL256;_LL259:;
_LL25A: goto _LL256;_LL256:;}{void*_tmp42F=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp40F);e1->topt=({struct Cyc_Core_Opt*_tmp430=_cycalloc(
sizeof(*_tmp430));_tmp430->v=(void*)_tmp42F;_tmp430;});{void*res_typ;{void*
_tmp431=Cyc_Tcutil_compress(_tmp42F);struct Cyc_Absyn_ArrayInfo _tmp432;void*
_tmp433;struct Cyc_Absyn_Tqual _tmp434;struct Cyc_Absyn_Exp*_tmp435;struct Cyc_Absyn_Conref*
_tmp436;_LL25C: if(_tmp431 <= (void*)4)goto _LL25E;if(*((int*)_tmp431)!= 7)goto
_LL25E;_tmp432=((struct Cyc_Absyn_ArrayType_struct*)_tmp431)->f1;_tmp433=(void*)
_tmp432.elt_type;_tmp434=_tmp432.tq;_tmp435=_tmp432.num_elts;_tmp436=_tmp432.zero_term;
_LL25D: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp437=_cycalloc(
sizeof(*_tmp437));_tmp437[0]=({struct Cyc_Absyn_PointerType_struct _tmp438;_tmp438.tag=
4;_tmp438.f1=({struct Cyc_Absyn_PtrInfo _tmp439;_tmp439.elt_typ=(void*)_tmp433;
_tmp439.elt_tq=_tmp434;_tmp439.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp43A;_tmp43A.rgn=(
void*)rgn;_tmp43A.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp43A.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp43C;_tmp43C.tag=0;_tmp43C.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp435);
_tmp43C;});_tmp43B;}));_tmp43A.zero_term=_tmp436;_tmp43A.ptrloc=0;_tmp43A;});
_tmp439;});_tmp438;});_tmp437;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->v=(void*)res_typ;_tmp43D;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL25B;_LL25E:;_LL25F:({void*_tmp43E[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp43F="tcExpNoPromote on Array_e returned non-array type";
_tag_dynforward(_tmp43F,sizeof(char),_get_zero_arr_size(_tmp43F,50));}),
_tag_dynforward(_tmp43E,sizeof(void*),0));});_LL25B:;}return res_typ;}}}_LL248:
if(*((int*)_tmp40C)!= 0)goto _LL24A;_tmp410=((struct Cyc_Absyn_Const_e_struct*)
_tmp40C)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp40C)->f1).String_c).tag
!= 5)goto _LL24A;_tmp411=(_tmp410.String_c).f1;_LL249: {void*_tmp440=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp441=Cyc_Tcexp_tcExp(te,(void**)& _tmp440,e1);return Cyc_Absyn_atb_typ(
_tmp441,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp443;_tmp443.tag=0;_tmp443.f1=Cyc_Absyn_uint_exp(1,0);_tmp443;});_tmp442;}),
Cyc_Absyn_false_conref);}_LL24A:;_LL24B: {void**topt2=0;if(topt != 0){void*
_tmp444=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp445;void*_tmp446;
void**_tmp447;struct Cyc_Absyn_Tqual _tmp448;_LL261: if(_tmp444 <= (void*)4)goto
_LL265;if(*((int*)_tmp444)!= 4)goto _LL263;_tmp445=((struct Cyc_Absyn_PointerType_struct*)
_tmp444)->f1;_tmp446=(void*)_tmp445.elt_typ;_tmp447=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp444)->f1).elt_typ;_tmp448=_tmp445.elt_tq;_LL262: topt2=(void**)_tmp447;goto
_LL260;_LL263: if(*((int*)_tmp444)!= 2)goto _LL265;_LL264: topt2=({void**_tmp449=
_cycalloc(sizeof(*_tmp449));_tmp449[0]=*topt;_tmp449;});goto _LL260;_LL265:;
_LL266: goto _LL260;_LL260:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));
_tmp44B[0]=({struct Cyc_Absyn_PointerType_struct _tmp44C;_tmp44C.tag=4;_tmp44C.f1=({
struct Cyc_Absyn_PtrInfo _tmp44D;_tmp44D.elt_typ=(void*)telt;_tmp44D.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp44D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp44E;_tmp44E.rgn=(void*)rgn;
_tmp44E.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp44E.bounds=Cyc_Absyn_bounds_one_conref;_tmp44E.zero_term=Cyc_Absyn_false_conref;
_tmp44E.ptrloc=0;_tmp44E;});_tmp44D;});_tmp44C;});_tmp44B;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A->v=(
void*)res_typ;_tmp44A;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL241:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp44F=t;struct Cyc_Absyn_ArrayInfo _tmp450;void*_tmp451;struct
Cyc_Absyn_Tqual _tmp452;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_Absyn_Conref*
_tmp454;_LL268: if(_tmp44F <= (void*)4)goto _LL26A;if(*((int*)_tmp44F)!= 7)goto
_LL26A;_tmp450=((struct Cyc_Absyn_ArrayType_struct*)_tmp44F)->f1;_tmp451=(void*)
_tmp450.elt_type;_tmp452=_tmp450.tq;_tmp453=_tmp450.num_elts;_tmp454=_tmp450.zero_term;
_LL269: {void*_tmp456;struct _tuple6 _tmp455=Cyc_Tcutil_addressof_props(te,e);
_tmp456=_tmp455.f2;{void*_tmp457=_tmp453 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp459;_tmp459.tag=0;_tmp459.f1=(struct Cyc_Absyn_Exp*)
_tmp453;_tmp459;});_tmp458;});t=Cyc_Absyn_atb_typ(_tmp451,_tmp456,_tmp452,
_tmp457,_tmp454);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL26A:;_LL26B: return t;_LL267:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);if(Cyc_Tcutil_is_unique_pointer(t) && !Cyc_Tcutil_is_unique_path(e))({
void*_tmp45A[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp45B="Cannot consume paths; do swap instead";
_tag_dynforward(_tmp45B,sizeof(char),_get_zero_arr_size(_tmp45B,38));}),
_tag_dynforward(_tmp45A,sizeof(void*),0));});{void*_tmp45C=(void*)e->r;union Cyc_Absyn_Cnst_union
_tmp45D;_LL26D: if(*((int*)_tmp45C)!= 28)goto _LL26F;_LL26E: goto _LL270;_LL26F: if(*((
int*)_tmp45C)!= 29)goto _LL271;_LL270: goto _LL272;_LL271: if(*((int*)_tmp45C)!= 0)
goto _LL273;_tmp45D=((struct Cyc_Absyn_Const_e_struct*)_tmp45C)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp45C)->f1).String_c).tag != 5)goto _LL273;_LL272:
return t;_LL273:;_LL274: t=Cyc_Tcutil_compress(t);{void*_tmp45E=t;struct Cyc_Absyn_ArrayInfo
_tmp45F;void*_tmp460;struct Cyc_Absyn_Tqual _tmp461;struct Cyc_Absyn_Exp*_tmp462;
struct Cyc_Absyn_Conref*_tmp463;_LL276: if(_tmp45E <= (void*)4)goto _LL278;if(*((int*)
_tmp45E)!= 7)goto _LL278;_tmp45F=((struct Cyc_Absyn_ArrayType_struct*)_tmp45E)->f1;
_tmp460=(void*)_tmp45F.elt_type;_tmp461=_tmp45F.tq;_tmp462=_tmp45F.num_elts;
_tmp463=_tmp45F.zero_term;_LL277: {void*_tmp465;struct _tuple6 _tmp464=Cyc_Tcutil_addressof_props(
te,e);_tmp465=_tmp464.f2;{void*b=_tmp462 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp467;_tmp467.tag=0;_tmp467.f1=(struct Cyc_Absyn_Exp*)
_tmp462;_tmp467;});_tmp466;});t=Cyc_Absyn_atb_typ(_tmp460,_tmp465,_tmp461,b,
_tmp463);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL278:;_LL279:
return t;_LL275:;}_LL26C:;}}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp468=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp469;_LL27B: if(*((int*)_tmp468)!= 13)goto _LL27D;_tmp469=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp468)->f1;_LL27C: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp469);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp469->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp469->topt))->v,0));e->topt=_tmp469->topt;goto
_LL27A;_LL27D:;_LL27E: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp46A=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp46B;void*
_tmp46C;struct Cyc_Absyn_Tqual _tmp46D;struct Cyc_Absyn_PtrAtts _tmp46E;void*_tmp46F;
struct Cyc_Absyn_Conref*_tmp470;struct Cyc_Absyn_Conref*_tmp471;struct Cyc_Absyn_Conref*
_tmp472;_LL280: if(_tmp46A <= (void*)4)goto _LL282;if(*((int*)_tmp46A)!= 4)goto
_LL282;_tmp46B=((struct Cyc_Absyn_PointerType_struct*)_tmp46A)->f1;_tmp46C=(void*)
_tmp46B.elt_typ;_tmp46D=_tmp46B.elt_tq;_tmp46E=_tmp46B.ptr_atts;_tmp46F=(void*)
_tmp46E.rgn;_tmp470=_tmp46E.nullable;_tmp471=_tmp46E.bounds;_tmp472=_tmp46E.zero_term;
_LL281:{void*_tmp473=Cyc_Tcutil_compress(_tmp46C);struct Cyc_Absyn_FnInfo _tmp474;
struct Cyc_List_List*_tmp475;struct Cyc_Core_Opt*_tmp476;void*_tmp477;struct Cyc_List_List*
_tmp478;int _tmp479;struct Cyc_Absyn_VarargInfo*_tmp47A;struct Cyc_List_List*
_tmp47B;struct Cyc_List_List*_tmp47C;_LL285: if(_tmp473 <= (void*)4)goto _LL287;if(*((
int*)_tmp473)!= 8)goto _LL287;_tmp474=((struct Cyc_Absyn_FnType_struct*)_tmp473)->f1;
_tmp475=_tmp474.tvars;_tmp476=_tmp474.effect;_tmp477=(void*)_tmp474.ret_typ;
_tmp478=_tmp474.args;_tmp479=_tmp474.c_varargs;_tmp47A=_tmp474.cyc_varargs;
_tmp47B=_tmp474.rgn_po;_tmp47C=_tmp474.attributes;_LL286: if(_tmp475 != 0){struct
_RegionHandle _tmp47D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp47D;
_push_region(rgn);{struct _tuple4 _tmp47E=({struct _tuple4 _tmp48B;_tmp48B.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp48B.f2=rgn;_tmp48B;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp47E,_tmp475);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488[0]=({struct Cyc_Absyn_FnType_struct
_tmp489;_tmp489.tag=8;_tmp489.f1=({struct Cyc_Absyn_FnInfo _tmp48A;_tmp48A.tvars=0;
_tmp48A.effect=_tmp476;_tmp48A.ret_typ=(void*)_tmp477;_tmp48A.args=_tmp478;
_tmp48A.c_varargs=_tmp479;_tmp48A.cyc_varargs=_tmp47A;_tmp48A.rgn_po=_tmp47B;
_tmp48A.attributes=_tmp47C;_tmp48A;});_tmp489;});_tmp488;}));struct Cyc_Absyn_PointerType_struct*
_tmp47F=({struct Cyc_Absyn_PointerType_struct*_tmp484=_cycalloc(sizeof(*_tmp484));
_tmp484[0]=({struct Cyc_Absyn_PointerType_struct _tmp485;_tmp485.tag=4;_tmp485.f1=({
struct Cyc_Absyn_PtrInfo _tmp486;_tmp486.elt_typ=(void*)ftyp;_tmp486.elt_tq=
_tmp46D;_tmp486.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp487;_tmp487.rgn=(void*)
_tmp46F;_tmp487.nullable=_tmp470;_tmp487.bounds=_tmp471;_tmp487.zero_term=
_tmp472;_tmp487.ptrloc=0;_tmp487;});_tmp486;});_tmp485;});_tmp484;});struct Cyc_Absyn_Exp*
_tmp480=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmp482;_tmp482.tag=14;_tmp482.f1=_tmp480;_tmp482.f2=ts;_tmp482;});_tmp481;})));
e->topt=({struct Cyc_Core_Opt*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->v=(
void*)((void*)_tmp47F);_tmp483;});};_pop_region(rgn);}goto _LL284;_LL287:;_LL288:
goto _LL284;_LL284:;}goto _LL27F;_LL282:;_LL283: goto _LL27F;_LL27F:;}goto _LL27A;
_LL27A:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp48C=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp48D;struct _tuple1*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_List_List*
_tmp490;struct Cyc_Core_Opt*_tmp491;struct Cyc_List_List*_tmp492;union Cyc_Absyn_Cnst_union
_tmp493;struct _tuple1*_tmp494;void*_tmp495;void*_tmp496;struct Cyc_List_List*
_tmp497;struct Cyc_Absyn_Exp*_tmp498;void*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;
struct Cyc_Core_Opt*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*
_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*
_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*
_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*
_tmp4A6;struct Cyc_List_List*_tmp4A7;struct Cyc_Absyn_VarargCallInfo*_tmp4A8;
struct Cyc_Absyn_VarargCallInfo**_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*
_tmp4AB;struct Cyc_List_List*_tmp4AC;void*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;
void*_tmp4AF;void**_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*
_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;void*_tmp4B5;
void*_tmp4B6;void*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Absyn_Exp*
_tmp4B9;struct _dynforward_ptr*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BB;struct
_dynforward_ptr*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;
struct Cyc_List_List*_tmp4BF;struct _tuple2*_tmp4C0;struct Cyc_List_List*_tmp4C1;
struct Cyc_List_List*_tmp4C2;struct Cyc_Absyn_Stmt*_tmp4C3;struct Cyc_Absyn_Vardecl*
_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C6;int _tmp4C7;int*
_tmp4C8;struct _tuple1*_tmp4C9;struct _tuple1**_tmp4CA;struct Cyc_List_List*_tmp4CB;
struct Cyc_List_List**_tmp4CC;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_Aggrdecl*
_tmp4CE;struct Cyc_Absyn_Aggrdecl**_tmp4CF;void*_tmp4D0;struct Cyc_List_List*
_tmp4D1;struct Cyc_List_List*_tmp4D2;struct Cyc_Absyn_Tuniondecl*_tmp4D3;struct Cyc_Absyn_Tunionfield*
_tmp4D4;struct _tuple1*_tmp4D5;struct _tuple1**_tmp4D6;struct Cyc_Absyn_Enumdecl*
_tmp4D7;struct Cyc_Absyn_Enumfield*_tmp4D8;struct _tuple1*_tmp4D9;struct _tuple1**
_tmp4DA;void*_tmp4DB;struct Cyc_Absyn_Enumfield*_tmp4DC;struct Cyc_Absyn_MallocInfo
_tmp4DD;int _tmp4DE;int*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E0;void**_tmp4E1;void***
_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp**_tmp4E4;int _tmp4E5;int*
_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;_LL28A: if(*((int*)
_tmp48C)!= 13)goto _LL28C;_tmp48D=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp48C)->f1;_LL28B: Cyc_Tcexp_tcExpNoInst(te,0,_tmp48D);return;_LL28C: if(*((int*)
_tmp48C)!= 2)goto _LL28E;_tmp48E=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp48C)->f1;
_LL28D: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp48E);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL28E: if(*((int*)_tmp48C)!= 10)goto _LL290;_tmp48F=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp48C)->f1;_tmp490=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp48C)->f2;_LL28F:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp48F,_tmp490);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL290: if(*((int*)_tmp48C)!= 37)goto _LL292;_tmp491=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp48C)->f1;_tmp492=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp48C)->f2;
_LL291: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp492);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL292: if(*((int*)_tmp48C)!= 0)goto _LL294;_tmp493=((struct Cyc_Absyn_Const_e_struct*)
_tmp48C)->f1;_LL293: t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp493,e);goto _LL289;_LL294:
if(*((int*)_tmp48C)!= 1)goto _LL296;_tmp494=((struct Cyc_Absyn_Var_e_struct*)
_tmp48C)->f1;_tmp495=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp48C)->f2;_LL295:
t=Cyc_Tcexp_tcVar(te,loc,_tmp494,_tmp495);goto _LL289;_LL296: if(*((int*)_tmp48C)
!= 3)goto _LL298;_tmp496=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp48C)->f1;
_tmp497=((struct Cyc_Absyn_Primop_e_struct*)_tmp48C)->f2;_LL297: t=Cyc_Tcexp_tcPrimop(
te,loc,topt,_tmp496,_tmp497);goto _LL289;_LL298: if(*((int*)_tmp48C)!= 5)goto
_LL29A;_tmp498=((struct Cyc_Absyn_Increment_e_struct*)_tmp48C)->f1;_tmp499=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmp48C)->f2;_LL299: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp498,_tmp499);goto _LL289;_LL29A: if(*((int*)_tmp48C)!= 4)goto
_LL29C;_tmp49A=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp48C)->f1;_tmp49B=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp48C)->f2;_tmp49C=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp48C)->f3;_LL29B: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp49A,_tmp49B,_tmp49C);
goto _LL289;_LL29C: if(*((int*)_tmp48C)!= 6)goto _LL29E;_tmp49D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp48C)->f1;_tmp49E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp48C)->f2;
_tmp49F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp48C)->f3;_LL29D: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp49D,_tmp49E,_tmp49F);goto _LL289;_LL29E: if(*((int*)_tmp48C)!= 7)
goto _LL2A0;_tmp4A0=((struct Cyc_Absyn_And_e_struct*)_tmp48C)->f1;_tmp4A1=((struct
Cyc_Absyn_And_e_struct*)_tmp48C)->f2;_LL29F: t=Cyc_Tcexp_tcAnd(te,loc,_tmp4A0,
_tmp4A1);goto _LL289;_LL2A0: if(*((int*)_tmp48C)!= 8)goto _LL2A2;_tmp4A2=((struct
Cyc_Absyn_Or_e_struct*)_tmp48C)->f1;_tmp4A3=((struct Cyc_Absyn_Or_e_struct*)
_tmp48C)->f2;_LL2A1: t=Cyc_Tcexp_tcOr(te,loc,_tmp4A2,_tmp4A3);goto _LL289;_LL2A2:
if(*((int*)_tmp48C)!= 9)goto _LL2A4;_tmp4A4=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp48C)->f1;_tmp4A5=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp48C)->f2;_LL2A3: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4A4,_tmp4A5);goto _LL289;_LL2A4: if(*((int*)
_tmp48C)!= 11)goto _LL2A6;_tmp4A6=((struct Cyc_Absyn_FnCall_e_struct*)_tmp48C)->f1;
_tmp4A7=((struct Cyc_Absyn_FnCall_e_struct*)_tmp48C)->f2;_tmp4A8=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp48C)->f3;_tmp4A9=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp48C)->f3;_LL2A5: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4A6,_tmp4A7,_tmp4A9);
goto _LL289;_LL2A6: if(*((int*)_tmp48C)!= 12)goto _LL2A8;_tmp4AA=((struct Cyc_Absyn_Throw_e_struct*)
_tmp48C)->f1;_LL2A7: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4AA);goto _LL289;_LL2A8:
if(*((int*)_tmp48C)!= 14)goto _LL2AA;_tmp4AB=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp48C)->f1;_tmp4AC=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp48C)->f2;_LL2A9:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4AB,_tmp4AC);goto _LL289;_LL2AA: if(*((
int*)_tmp48C)!= 15)goto _LL2AC;_tmp4AD=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp48C)->f1;_tmp4AE=((struct Cyc_Absyn_Cast_e_struct*)_tmp48C)->f2;_tmp4AF=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp48C)->f4;_tmp4B0=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp48C)->f4);_LL2AB: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4AD,_tmp4AE,(void**)
_tmp4B0);goto _LL289;_LL2AC: if(*((int*)_tmp48C)!= 16)goto _LL2AE;_tmp4B1=((struct
Cyc_Absyn_Address_e_struct*)_tmp48C)->f1;_LL2AD: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4B1);goto _LL289;_LL2AE: if(*((int*)_tmp48C)!= 17)goto _LL2B0;_tmp4B2=((
struct Cyc_Absyn_New_e_struct*)_tmp48C)->f1;_tmp4B3=((struct Cyc_Absyn_New_e_struct*)
_tmp48C)->f2;_LL2AF: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4B2,e,_tmp4B3);goto _LL289;
_LL2B0: if(*((int*)_tmp48C)!= 19)goto _LL2B2;_tmp4B4=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp48C)->f1;_LL2B1: {void*_tmp4E9=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4B4);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4E9);goto _LL289;}_LL2B2: if(*((int*)_tmp48C)!= 18)goto _LL2B4;
_tmp4B5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp48C)->f1;_LL2B3: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4B5);goto _LL289;_LL2B4: if(*((int*)_tmp48C)!= 20)goto _LL2B6;
_tmp4B6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp48C)->f1;_tmp4B7=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp48C)->f2;_LL2B5: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4B6,_tmp4B7);goto _LL289;_LL2B6: if(*((int*)_tmp48C)!= 21)goto _LL2B8;
_LL2B7:({void*_tmp4EA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4EB="gen() not in top-level initializer";
_tag_dynforward(_tmp4EB,sizeof(char),_get_zero_arr_size(_tmp4EB,35));}),
_tag_dynforward(_tmp4EA,sizeof(void*),0));});return;_LL2B8: if(*((int*)_tmp48C)!= 
22)goto _LL2BA;_tmp4B8=((struct Cyc_Absyn_Deref_e_struct*)_tmp48C)->f1;_LL2B9: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4B8);goto _LL289;_LL2BA: if(*((int*)_tmp48C)!= 
23)goto _LL2BC;_tmp4B9=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp48C)->f1;
_tmp4BA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp48C)->f2;_LL2BB: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp4B9,_tmp4BA);goto _LL289;_LL2BC: if(*((int*)_tmp48C)!= 24)goto
_LL2BE;_tmp4BB=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp48C)->f1;_tmp4BC=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp48C)->f2;_LL2BD: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp4BB,_tmp4BC);goto _LL289;_LL2BE: if(*((int*)_tmp48C)!= 25)goto
_LL2C0;_tmp4BD=((struct Cyc_Absyn_Subscript_e_struct*)_tmp48C)->f1;_tmp4BE=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp48C)->f2;_LL2BF: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4BD,_tmp4BE);goto _LL289;_LL2C0: if(*((int*)_tmp48C)!= 26)goto
_LL2C2;_tmp4BF=((struct Cyc_Absyn_Tuple_e_struct*)_tmp48C)->f1;_LL2C1: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4BF);goto _LL289;_LL2C2: if(*((int*)_tmp48C)!= 27)goto _LL2C4;
_tmp4C0=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp48C)->f1;_tmp4C1=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp48C)->f2;_LL2C3: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4C0,_tmp4C1);goto
_LL289;_LL2C4: if(*((int*)_tmp48C)!= 28)goto _LL2C6;_tmp4C2=((struct Cyc_Absyn_Array_e_struct*)
_tmp48C)->f1;_LL2C5: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4EC=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4ED;void*_tmp4EE;void**
_tmp4EF;struct Cyc_Absyn_Conref*_tmp4F0;_LL2D9: if(_tmp4EC <= (void*)4)goto _LL2DB;
if(*((int*)_tmp4EC)!= 7)goto _LL2DB;_tmp4ED=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4EC)->f1;_tmp4EE=(void*)_tmp4ED.elt_type;_tmp4EF=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4EC)->f1).elt_type;_tmp4F0=_tmp4ED.zero_term;_LL2DA: elt_topt=(void**)_tmp4EF;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4F0);
goto _LL2D8;_LL2DB:;_LL2DC: goto _LL2D8;_LL2D8:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4C2);goto _LL289;}_LL2C6: if(*((int*)_tmp48C)!= 38)goto
_LL2C8;_tmp4C3=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp48C)->f1;_LL2C7: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4C3);goto _LL289;_LL2C8: if(*((int*)_tmp48C)!= 29)goto _LL2CA;
_tmp4C4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f1;_tmp4C5=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f2;_tmp4C6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp48C)->f3;_tmp4C7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f4;
_tmp4C8=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f4;_LL2C9: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4C4,_tmp4C5,_tmp4C6,_tmp4C8);goto
_LL289;_LL2CA: if(*((int*)_tmp48C)!= 30)goto _LL2CC;_tmp4C9=((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f1;_tmp4CA=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f1;_tmp4CB=((struct Cyc_Absyn_Struct_e_struct*)_tmp48C)->f2;_tmp4CC=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp48C)->f2;_tmp4CD=((
struct Cyc_Absyn_Struct_e_struct*)_tmp48C)->f3;_tmp4CE=((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f4;_tmp4CF=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f4;_LL2CB: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4CA,_tmp4CC,_tmp4CD,
_tmp4CF);goto _LL289;_LL2CC: if(*((int*)_tmp48C)!= 31)goto _LL2CE;_tmp4D0=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp48C)->f1;_tmp4D1=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp48C)->f2;_LL2CD: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4D0,_tmp4D1);goto _LL289;
_LL2CE: if(*((int*)_tmp48C)!= 32)goto _LL2D0;_tmp4D2=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp48C)->f1;_tmp4D3=((struct Cyc_Absyn_Tunion_e_struct*)_tmp48C)->f2;_tmp4D4=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp48C)->f3;_LL2CF: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4D2,_tmp4D3,_tmp4D4);goto _LL289;_LL2D0: if(*((int*)_tmp48C)!= 33)goto
_LL2D2;_tmp4D5=((struct Cyc_Absyn_Enum_e_struct*)_tmp48C)->f1;_tmp4D6=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp48C)->f1;_tmp4D7=((struct Cyc_Absyn_Enum_e_struct*)
_tmp48C)->f2;_tmp4D8=((struct Cyc_Absyn_Enum_e_struct*)_tmp48C)->f3;_LL2D1:*
_tmp4D6=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4D8))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4F2;_tmp4F2.tag=12;_tmp4F2.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4D7))->name;_tmp4F2.f2=_tmp4D7;_tmp4F2;});_tmp4F1;});goto _LL289;
_LL2D2: if(*((int*)_tmp48C)!= 34)goto _LL2D4;_tmp4D9=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp48C)->f1;_tmp4DA=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp48C)->f1;_tmp4DB=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp48C)->f2;
_tmp4DC=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp48C)->f3;_LL2D3:*_tmp4DA=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4DC))->name;t=_tmp4DB;goto _LL289;
_LL2D4: if(*((int*)_tmp48C)!= 35)goto _LL2D6;_tmp4DD=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48C)->f1;_tmp4DE=_tmp4DD.is_calloc;_tmp4DF=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48C)->f1).is_calloc;_tmp4E0=_tmp4DD.rgn;_tmp4E1=_tmp4DD.elt_type;_tmp4E2=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp48C)->f1).elt_type;_tmp4E3=
_tmp4DD.num_elts;_tmp4E4=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48C)->f1).num_elts;_tmp4E5=_tmp4DD.fat_result;_tmp4E6=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48C)->f1).fat_result;_LL2D5: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4E0,_tmp4E2,
_tmp4E4,_tmp4DF,_tmp4E6);goto _LL289;_LL2D6: if(*((int*)_tmp48C)!= 36)goto _LL289;
_tmp4E7=((struct Cyc_Absyn_Swap_e_struct*)_tmp48C)->f1;_tmp4E8=((struct Cyc_Absyn_Swap_e_struct*)
_tmp48C)->f2;_LL2D7: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4E7,_tmp4E8);goto _LL289;
_LL289:;}e->topt=({struct Cyc_Core_Opt*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));
_tmp4F3->v=(void*)t;_tmp4F3;});}
