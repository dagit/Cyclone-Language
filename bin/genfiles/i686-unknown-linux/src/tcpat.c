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
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
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
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];void*Cyc_Set_choose(
struct Cyc_Set_Set*s);int Cyc_strcmp(struct _dynforward_ptr s1,struct _dynforward_ptr
s2);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct
_dynforward_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*
k2);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern
struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern
struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int
Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*
tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Absyn_Pat*
p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;union Cyc_Absyn_AggrInfoU_union _tmp6;
void*_tmp7;struct _tuple1*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*
_tmpA;struct Cyc_List_List**_tmpB;struct Cyc_List_List*_tmpC;int _tmpD;struct Cyc_Absyn_Exp*
_tmpE;_LL1: if(_tmp0 <= (void*)2)goto _LL9;if(*((int*)_tmp0)!= 12)goto _LL3;_tmp1=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{struct _handler_cons _tmpF;
_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){{
struct _RegionHandle _tmp12=_new_region("r");struct _RegionHandle*r=& _tmp12;
_push_region(r);{void*_tmp13=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp1);struct
Cyc_Absyn_Tuniondecl*_tmp14;struct Cyc_Absyn_Tunionfield*_tmp15;struct Cyc_Absyn_Enumdecl*
_tmp16;struct Cyc_Absyn_Enumfield*_tmp17;void*_tmp18;struct Cyc_Absyn_Enumfield*
_tmp19;_LLC: if(*((int*)_tmp13)!= 1)goto _LLE;_LLD:(void*)(p->r=(void*)((void*)0));({
void*_tmp1A[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp1B="struct tag used without arguments in pattern";
_tag_dynforward(_tmp1B,sizeof(char),_get_zero_arr_size(_tmp1B,45));}),
_tag_dynforward(_tmp1A,sizeof(void*),0));});_npop_handler(1);return;_LLE: if(*((
int*)_tmp13)!= 2)goto _LL10;_tmp14=((struct Cyc_Tcenv_TunionRes_struct*)_tmp13)->f1;
_tmp15=((struct Cyc_Tcenv_TunionRes_struct*)_tmp13)->f2;_LLF:(void*)(p->r=(void*)((
void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[
0]=({struct Cyc_Absyn_Tunion_p_struct _tmp1D;_tmp1D.tag=6;_tmp1D.f1=_tmp14;_tmp1D.f2=
_tmp15;_tmp1D.f3=0;_tmp1D.f4=0;_tmp1D;});_tmp1C;})));_npop_handler(1);return;
_LL10: if(*((int*)_tmp13)!= 3)goto _LL12;_tmp16=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp13)->f1;_tmp17=((struct Cyc_Tcenv_EnumRes_struct*)_tmp13)->f2;_LL11:(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Enum_p_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));
_tmp1E[0]=({struct Cyc_Absyn_Enum_p_struct _tmp1F;_tmp1F.tag=10;_tmp1F.f1=_tmp16;
_tmp1F.f2=_tmp17;_tmp1F;});_tmp1E;})));_npop_handler(1);return;_LL12: if(*((int*)
_tmp13)!= 4)goto _LL14;_tmp18=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp13)->f1;
_tmp19=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp13)->f2;_LL13:(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*_tmp20=_cycalloc(sizeof(*
_tmp20));_tmp20[0]=({struct Cyc_Absyn_AnonEnum_p_struct _tmp21;_tmp21.tag=11;
_tmp21.f1=(void*)_tmp18;_tmp21.f2=_tmp19;_tmp21;});_tmp20;})));_npop_handler(1);
return;_LL14: if(*((int*)_tmp13)!= 0)goto _LLB;_LL15: goto _LLB;_LLB:;};_pop_region(
r);};_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp23=_tmp10;
_LL17: if(_tmp23 != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;_LL19:;_LL1A:(void)
_throw(_tmp23);_LL16:;}}}{union Cyc_Absyn_Nmspace_union _tmp24=(*_tmp1).f1;struct
Cyc_List_List*_tmp25;_LL1C: if((_tmp24.Loc_n).tag != 0)goto _LL1E;_LL1D: goto _LL1F;
_LL1E: if((_tmp24.Rel_n).tag != 1)goto _LL20;_tmp25=(_tmp24.Rel_n).f1;if(_tmp25 != 0)
goto _LL20;_LL1F:(*_tmp1).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp26;(_tmp26.Loc_n).tag=0;_tmp26;});(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Absyn_Var_p_struct _tmp28;
_tmp28.tag=0;_tmp28.f1=Cyc_Absyn_new_vardecl(_tmp1,(void*)0,0);_tmp28.f2=Cyc_Absyn_new_pat((
void*)0,0);_tmp28;});_tmp27;})));return;_LL20:;_LL21:({void*_tmp29[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp2A="qualified variable in pattern";_tag_dynforward(_tmp2A,
sizeof(char),_get_zero_arr_size(_tmp2A,30));}),_tag_dynforward(_tmp29,sizeof(
void*),0));});return;_LL1B:;}_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct
Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f2;_tmp4=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct
_handler_cons _tmp2B;_push_handler(& _tmp2B);{int _tmp2D=0;if(setjmp(_tmp2B.handler))
_tmp2D=1;if(!_tmp2D){{struct _RegionHandle _tmp2E=_new_region("r");struct
_RegionHandle*r=& _tmp2E;_push_region(r);{void*_tmp2F=Cyc_Tcenv_lookup_ordinary(r,
te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*_tmp30;struct Cyc_Absyn_Tuniondecl*
_tmp31;struct Cyc_Absyn_Tunionfield*_tmp32;_LL23: if(*((int*)_tmp2F)!= 1)goto _LL25;
_tmp30=((struct Cyc_Tcenv_AggrRes_struct*)_tmp2F)->f1;_LL24: if((void*)_tmp30->kind
== (void*)1){(void*)(p->r=(void*)((void*)0));({void*_tmp33[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp34="cannot pattern-match a union";_tag_dynforward(_tmp34,
sizeof(char),_get_zero_arr_size(_tmp34,29));}),_tag_dynforward(_tmp33,sizeof(
void*),0));});_npop_handler(1);return;}{struct Cyc_List_List*_tmp35=0;for(0;_tmp3
!= 0;_tmp3=_tmp3->tl){_tmp35=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*
_tmp36));_tmp36->hd=({struct _tuple8*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=
0;_tmp37->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd;_tmp37;});_tmp36->tl=_tmp35;_tmp36;});}(
void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp38=_cycalloc(
sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp39;_tmp39.tag=5;
_tmp39.f1=({struct Cyc_Absyn_AggrInfo _tmp3A;_tmp3A.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp3B;(_tmp3B.KnownAggr).tag=1;(_tmp3B.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=_tmp30;
_tmp3C;});_tmp3B;});_tmp3A.targs=0;_tmp3A;});_tmp39.f2=0;_tmp39.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp35);_tmp39.f4=_tmp4;_tmp39;});
_tmp38;})));_npop_handler(1);return;}_LL25: if(*((int*)_tmp2F)!= 2)goto _LL27;
_tmp31=((struct Cyc_Tcenv_TunionRes_struct*)_tmp2F)->f1;_tmp32=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp2F)->f2;_LL26:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp3E;_tmp3E.tag=6;_tmp3E.f1=_tmp31;_tmp3E.f2=_tmp32;_tmp3E.f3=_tmp3;_tmp3E.f4=
_tmp4;_tmp3E;});_tmp3D;})));_npop_handler(1);return;_LL27: if(*((int*)_tmp2F)!= 3)
goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)_tmp2F)!= 4)goto _LL2B;_LL2A:({void*
_tmp3F[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp40="enum tag used with arguments in pattern";
_tag_dynforward(_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,40));}),
_tag_dynforward(_tmp3F,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));
_npop_handler(1);return;_LL2B: if(*((int*)_tmp2F)!= 0)goto _LL22;_LL2C: goto _LL22;
_LL22:;};_pop_region(r);};_pop_handler();}else{void*_tmp2C=(void*)_exn_thrown;
void*_tmp42=_tmp2C;_LL2E: if(_tmp42 != Cyc_Dict_Absent)goto _LL30;_LL2F: goto _LL2D;
_LL30:;_LL31:(void)_throw(_tmp42);_LL2D:;}}}({struct Cyc_String_pa_struct _tmp45;
_tmp45.tag=0;_tmp45.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp2));{void*_tmp43[1]={& _tmp45};Cyc_Tcutil_terr(p->loc,({const char*_tmp44="%s is not a constructor in pattern";
_tag_dynforward(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,35));}),
_tag_dynforward(_tmp43,sizeof(void*),1));}});(void*)(p->r=(void*)((void*)0));
return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;_tmp6=_tmp5.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL7;_tmp7=(_tmp6.UnknownAggr).f1;_tmp8=(_tmp6.UnknownAggr).f2;_tmp9=
_tmp5.targs;_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpB=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;_tmpD=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f4;_LL6:{struct
_handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;if(setjmp(_tmp46.handler))
_tmp48=1;if(!_tmp48){{struct Cyc_Absyn_Aggrdecl**_tmp49=Cyc_Tcenv_lookup_aggrdecl(
te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp4A=*_tmp49;if((void*)_tmp4A->kind
== (void*)1){({void*_tmp4B[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4C="cannot pattern-match a union";
_tag_dynforward(_tmp4C,sizeof(char),_get_zero_arr_size(_tmp4C,29));}),
_tag_dynforward(_tmp4B,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));
_npop_handler(0);return;}if(_tmp4A->impl == 0){({void*_tmp4D[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp4E="can't destructure an abstract struct";_tag_dynforward(
_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,37));}),_tag_dynforward(_tmp4D,
sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}{
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4A->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*
_tmpB);if(more_exists < 0){({void*_tmp4F[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp50="too many existentially bound type variables in pattern";_tag_dynforward(
_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,55));}),_tag_dynforward(_tmp4F,
sizeof(void*),0));});{struct Cyc_List_List**_tmp51=_tmpB;{int n=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4A->impl))->exist_vars);
for(0;n != 0;n --){_tmp51=&((struct Cyc_List_List*)_check_null(*_tmp51))->tl;}}*
_tmp51=0;}}else{if(more_exists > 0){struct Cyc_List_List*_tmp52=0;for(0;
more_exists != 0;more_exists --){_tmp52=({struct Cyc_List_List*_tmp53=_cycalloc(
sizeof(*_tmp53));_tmp53->hd=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp55;_tmp55.tag=1;_tmp55.f1=0;_tmp55;});_tmp54;}));_tmp53->tl=_tmp52;_tmp53;});}*
_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*
_tmpB,_tmp52);}}(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp57;_tmp57.tag=5;_tmp57.f1=({struct Cyc_Absyn_AggrInfo _tmp58;_tmp58.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp59;(_tmp59.KnownAggr).tag=
1;(_tmp59.KnownAggr).f1=_tmp49;_tmp59;});_tmp58.targs=_tmp9;_tmp58;});_tmp57.f2=*
_tmpB;_tmp57.f3=_tmpC;_tmp57.f4=_tmpD;_tmp57;});_tmp56;})));}};_pop_handler();}
else{void*_tmp47=(void*)_exn_thrown;void*_tmp5B=_tmp47;_LL33: if(_tmp5B != Cyc_Dict_Absent)
goto _LL35;_LL34:({void*_tmp5C[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp5D="Non-struct name has designator patterns";
_tag_dynforward(_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,40));}),
_tag_dynforward(_tmp5C,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));goto
_LL32;_LL35:;_LL36:(void)_throw(_tmp5B);_LL32:;}}}return;_LL7: if(*((int*)_tmp0)
!= 14)goto _LL9;_tmpE=((struct Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(
te,0,_tmpE);if(!Cyc_Tcutil_is_const_exp(te,_tmpE)){({void*_tmp5E[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp5F="non-constant expression in case pattern";
_tag_dynforward(_tmp5F,sizeof(char),_get_zero_arr_size(_tmp5F,40));}),
_tag_dynforward(_tmp5E,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));}{
unsigned int _tmp61;int _tmp62;struct _tuple7 _tmp60=Cyc_Evexp_eval_const_uint_exp(
_tmpE);_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_Int_p_struct _tmp64;
_tmp64.tag=7;_tmp64.f1=(void*)((void*)2);_tmp64.f2=(int)_tmp61;_tmp64;});_tmp63;})));
return;}_LL9:;_LLA: return;_LL0:;}static struct _dynforward_ptr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp65=_cycalloc(sizeof(*
_tmp65));_tmp65->v=s;_tmp65;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp66=Cyc_Tcutil_compress(
numt);_LL38: if(_tmp66 <= (void*)4)goto _LL3A;if(*((int*)_tmp66)!= 12)goto _LL3A;
_LL39: if(topt != 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return
numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp67=_cycalloc(
sizeof(*_tmp67));_tmp67->hd=vd;_tmp67->tl=*v_result_ptr;_tmp67;});}static struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,
struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp69;struct Cyc_List_List*_tmp6A;
struct Cyc_Tcpat_TcPatResult _tmp68=res1;_tmp69=_tmp68.tvars_and_bounds_opt;_tmp6A=
_tmp68.patvars;{struct _tuple6*_tmp6C;struct Cyc_List_List*_tmp6D;struct Cyc_Tcpat_TcPatResult
_tmp6B=res2;_tmp6C=_tmp6B.tvars_and_bounds_opt;_tmp6D=_tmp6B.patvars;if(_tmp69 != 
0  || _tmp6C != 0){if(_tmp69 == 0)_tmp69=({struct _tuple6*_tmp6E=_cycalloc(sizeof(*
_tmp6E));_tmp6E->f1=0;_tmp6E->f2=0;_tmp6E;});if(_tmp6C == 0)_tmp6C=({struct
_tuple6*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=0;_tmp6F->f2=0;_tmp6F;});
return({struct Cyc_Tcpat_TcPatResult _tmp70;_tmp70.tvars_and_bounds_opt=({struct
_tuple6*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp69))).f1,(*((struct _tuple6*)_check_null(_tmp6C))).f1);_tmp71->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple6*)_check_null(_tmp69))).f2,(*((struct _tuple6*)_check_null(_tmp6C))).f2);
_tmp71;});_tmp70.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp6A,_tmp6D);_tmp70;});}return({struct Cyc_Tcpat_TcPatResult
_tmp72;_tmp72.tvars_and_bounds_opt=0;_tmp72.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6A,_tmp6D);
_tmp72;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->loc=
loc;_tmp73->topt=0;_tmp73->r=(void*)((void*)0);_tmp73;});}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp145;
_tmp145.tvars_and_bounds_opt=0;_tmp145.patvars=0;_tmp145;});{void*_tmp74=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp75;struct Cyc_Absyn_Pat*_tmp76;struct Cyc_Absyn_Vardecl*
_tmp77;struct Cyc_Absyn_Pat*_tmp78;struct Cyc_Absyn_Tvar*_tmp79;struct Cyc_Absyn_Vardecl*
_tmp7A;void*_tmp7B;void*_tmp7C;void*_tmp7D;struct Cyc_Absyn_Enumdecl*_tmp7E;void*
_tmp7F;struct Cyc_Absyn_Pat*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List**
_tmp82;int _tmp83;struct Cyc_Absyn_AggrInfo _tmp84;union Cyc_Absyn_AggrInfoU_union
_tmp85;struct Cyc_Absyn_Aggrdecl**_tmp86;struct Cyc_Absyn_Aggrdecl*_tmp87;struct
Cyc_List_List*_tmp88;struct Cyc_List_List**_tmp89;struct Cyc_List_List*_tmp8A;
struct Cyc_List_List*_tmp8B;struct Cyc_List_List**_tmp8C;int _tmp8D;struct Cyc_Absyn_Tuniondecl*
_tmp8E;struct Cyc_Absyn_Tunionfield*_tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_List_List**
_tmp91;int _tmp92;struct Cyc_Absyn_AggrInfo _tmp93;union Cyc_Absyn_AggrInfoU_union
_tmp94;_LL3D: if((int)_tmp74 != 0)goto _LL3F;_LL3E: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3C;_LL3F: if(_tmp74 <= (void*)2)goto _LL53;if(*((int*)_tmp74)!= 0)
goto _LL41;_tmp75=((struct Cyc_Absyn_Var_p_struct*)_tmp74)->f1;_tmp76=((struct Cyc_Absyn_Var_p_struct*)
_tmp74)->f2;_LL40: res=Cyc_Tcpat_tcPatRec(te,_tmp76,topt,rgn_opt);t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp76->topt))->v;Cyc_Tcpat_set_vd(_tmp75,& res.patvars,
t);goto _LL3C;_LL41: if(*((int*)_tmp74)!= 1)goto _LL43;_tmp77=((struct Cyc_Absyn_Reference_p_struct*)
_tmp74)->f1;_tmp78=((struct Cyc_Absyn_Reference_p_struct*)_tmp74)->f2;_LL42: res=
Cyc_Tcpat_tcPatRec(te,_tmp78,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp78->topt))->v;if(rgn_opt == 0){({void*_tmp95[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp96="* pattern would point into an unknown/unallowed region";
_tag_dynforward(_tmp96,sizeof(char),_get_zero_arr_size(_tmp96,55));}),
_tag_dynforward(_tmp95,sizeof(void*),0));});goto _LL3C;}Cyc_Tcpat_set_vd(_tmp77,&
res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97[0]=({struct Cyc_Absyn_PointerType_struct _tmp98;_tmp98.tag=4;
_tmp98.f1=({struct Cyc_Absyn_PtrInfo _tmp99;_tmp99.elt_typ=(void*)t;_tmp99.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp99.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9A;_tmp9A.rgn=(
void*)*rgn_opt;_tmp9A.nullable=Cyc_Absyn_false_conref;_tmp9A.bounds=Cyc_Absyn_empty_conref();
_tmp9A.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp9A.ptrloc=
0;_tmp9A;});_tmp99;});_tmp98;});_tmp97;}));goto _LL3C;_LL43: if(*((int*)_tmp74)!= 
2)goto _LL45;_tmp79=((struct Cyc_Absyn_TagInt_p_struct*)_tmp74)->f1;_tmp7A=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp74)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp7A,& res.patvars,(
void*)_tmp7A->type);{struct _RegionHandle _tmp9B=_new_region("r2");struct
_RegionHandle*r2=& _tmp9B;_push_region(r2);Cyc_Tcenv_add_type_vars(r2,p->loc,te,({
struct Cyc_Absyn_Tvar*_tmp9C[1];_tmp9C[0]=_tmp79;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9C,sizeof(struct Cyc_Absyn_Tvar*),
1));}));;_pop_region(r2);}if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->f1=0;_tmp9D->f2=0;_tmp9D;});(*
res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*
_tmp9E));_tmp9E->hd=_tmp79;_tmp9E->tl=(*res.tvars_and_bounds_opt).f1;_tmp9E;});t=
Cyc_Absyn_uint_typ;goto _LL3C;_LL45: if(*((int*)_tmp74)!= 7)goto _LL47;_tmp7B=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)_tmp7B != 1)goto _LL47;_LL46: t=
Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL3C;_LL47: if(*((int*)_tmp74)!= 
7)goto _LL49;_tmp7C=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)
_tmp7C != 2)goto _LL49;_LL48: goto _LL4A;_LL49: if(*((int*)_tmp74)!= 7)goto _LL4B;
_tmp7D=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)_tmp7D != 0)
goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3C;_LL4B:
if(*((int*)_tmp74)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL3C;_LL4D: if(*((int*)_tmp74)!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL3C;_LL4F: if(*((int*)_tmp74)!= 10)goto _LL51;_tmp7E=((
struct Cyc_Absyn_Enum_p_struct*)_tmp74)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({
struct Cyc_Absyn_EnumType_struct _tmpA0;_tmpA0.tag=12;_tmpA0.f1=_tmp7E->name;
_tmpA0.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7E;_tmpA0;});_tmp9F;}));goto _LL3C;_LL51:
if(*((int*)_tmp74)!= 11)goto _LL53;_tmp7F=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp74)->f1;_LL52: t=Cyc_Tcpat_num_type(topt,_tmp7F);goto _LL3C;_LL53: if((int)
_tmp74 != 1)goto _LL55;_LL54: if(topt != 0){void*_tmpA1=Cyc_Tcutil_compress(*topt);
_LL66: if(_tmpA1 <= (void*)4)goto _LL68;if(*((int*)_tmpA1)!= 4)goto _LL68;_LL67: t=*
topt;goto tcpat_end;_LL68:;_LL69: goto _LL65;_LL65:;}{struct Cyc_List_List*_tmpA2=
Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_PointerType_struct
_tmpA4;_tmpA4.tag=4;_tmpA4.f1=({struct Cyc_Absyn_PtrInfo _tmpA5;_tmpA5.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->v=_tmpA2;_tmpA8;}));_tmpA5.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmpA5.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA6;_tmpA6.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpA7=_cycalloc(sizeof(*
_tmpA7));_tmpA7->v=_tmpA2;_tmpA7;}));_tmpA6.nullable=Cyc_Absyn_true_conref;
_tmpA6.bounds=Cyc_Absyn_empty_conref();_tmpA6.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA6.ptrloc=0;_tmpA6;});_tmpA5;});_tmpA4;});_tmpA3;});
goto _LL3C;}_LL55: if(_tmp74 <= (void*)2)goto _LL57;if(*((int*)_tmp74)!= 4)goto _LL57;
_tmp80=((struct Cyc_Absyn_Pointer_p_struct*)_tmp74)->f1;_LL56: {void*inner_typ=(
void*)0;void**_tmpA9=0;if(topt != 0){void*_tmpAA=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpAB;void*_tmpAC;_LL6B: if(_tmpAA <= (void*)4)goto _LL6D;
if(*((int*)_tmpAA)!= 4)goto _LL6D;_tmpAB=((struct Cyc_Absyn_PointerType_struct*)
_tmpAA)->f1;_tmpAC=(void*)_tmpAB.elt_typ;_LL6C: inner_typ=_tmpAC;_tmpA9=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpC1=_cycalloc(sizeof(*
_tmpC1));_tmpC1->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC1;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp80,_tmpA9,(void**)& ptr_rgn));if(_tmpA9 == 0){void*
_tmpAD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmpAE;union Cyc_Absyn_TunionFieldInfoU_union
_tmpAF;struct Cyc_Absyn_Tuniondecl*_tmpB0;struct Cyc_Absyn_Tunionfield*_tmpB1;
struct Cyc_List_List*_tmpB2;_LL70: if(_tmpAD <= (void*)4)goto _LL72;if(*((int*)
_tmpAD)!= 3)goto _LL72;_tmpAE=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpAD)->f1;
_tmpAF=_tmpAE.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmpAD)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL72;_tmpB0=(_tmpAF.KnownTunionfield).f1;_tmpB1=(_tmpAF.KnownTunionfield).f2;
_tmpB2=_tmpAE.targs;if(!(!_tmpB0->is_flat))goto _LL72;_LL71: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_TunionType_struct
_tmpB4;_tmpB4.tag=2;_tmpB4.f1=({struct Cyc_Absyn_TunionInfo _tmpB5;_tmpB5.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union _tmpB7;(
_tmpB7.KnownTunion).tag=1;(_tmpB7.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=_tmpB0;_tmpB8;});_tmpB7;});_tmpB5.targs=
_tmpB2;_tmpB5.rgn=({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->v=(
void*)ptr_rgn;_tmpB6;});_tmpB5;});_tmpB4;});_tmpB3;});goto _LL6F;_LL72:;_LL73: t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));
_tmpB9[0]=({struct Cyc_Absyn_PointerType_struct _tmpBA;_tmpBA.tag=4;_tmpBA.f1=({
struct Cyc_Absyn_PtrInfo _tmpBB;_tmpBB.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp80->topt))->v);_tmpBB.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpBB.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpBC;_tmpBC.rgn=(void*)ptr_rgn;_tmpBC.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBC.bounds=Cyc_Absyn_empty_conref();
_tmpBC.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBC.ptrloc=
0;_tmpBC;});_tmpBB;});_tmpBA;});_tmpB9;});goto _LL6F;_LL6F:;}else{t=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({
struct Cyc_Absyn_PointerType_struct _tmpBE;_tmpBE.tag=4;_tmpBE.f1=({struct Cyc_Absyn_PtrInfo
_tmpBF;_tmpBF.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v);
_tmpBF.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpBF.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpC0;_tmpC0.rgn=(void*)ptr_rgn;_tmpC0.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpC0.bounds=Cyc_Absyn_empty_conref();_tmpC0.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpC0.ptrloc=0;_tmpC0;});
_tmpBF;});_tmpBE;});_tmpBD;});}goto _LL3C;}}_LL57: if(_tmp74 <= (void*)2)goto _LL59;
if(*((int*)_tmp74)!= 3)goto _LL59;_tmp81=((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f1;
_tmp82=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f1;
_tmp83=((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f2;_LL58: {struct Cyc_List_List*
_tmpC2=*_tmp82;struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(
topt != 0){void*_tmpC3=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpC4;
_LL75: if(_tmpC3 <= (void*)4)goto _LL77;if(*((int*)_tmpC3)!= 9)goto _LL77;_tmpC4=((
struct Cyc_Absyn_TupleType_struct*)_tmpC3)->f1;_LL76: topt_ts=_tmpC4;if(_tmp83){
int _tmpC5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC2);int _tmpC6=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC4);if(_tmpC5 < _tmpC6){struct
Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpC6 - _tmpC5;i ++){wild_ps=({struct Cyc_List_List*
_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->hd=Cyc_Tcpat_wild_pat(p->loc);_tmpC7->tl=
wild_ps;_tmpC7;});}}*_tmp82=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC2,wild_ps);_tmpC2=*_tmp82;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC2)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpC4))({void*_tmpC8[0]={};Cyc_Tcutil_warn(
p->loc,({const char*_tmpC9="unnecessary ... in tuple pattern";_tag_dynforward(
_tmpC9,sizeof(char),_get_zero_arr_size(_tmpC9,33));}),_tag_dynforward(_tmpC8,
sizeof(void*),0));});}}goto _LL74;_LL77:;_LL78: goto _LL74;_LL74:;}else{if(_tmp83)({
void*_tmpCA[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmpCB="cannot determine missing fields for ... in tuple pattern";
_tag_dynforward(_tmpCB,sizeof(char),_get_zero_arr_size(_tmpCB,57));}),
_tag_dynforward(_tmpCA,sizeof(void*),0));});}for(0;_tmpC2 != 0;_tmpC2=_tmpC2->tl){
void**_tmpCC=0;if(topt_ts != 0){_tmpCC=(void**)&(*((struct _tuple9*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(
struct Cyc_Absyn_Pat*)_tmpC2->hd,_tmpCC,rgn_opt));pat_ts=({struct Cyc_List_List*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=({struct _tuple9*_tmpCE=_cycalloc(
sizeof(*_tmpCE));_tmpCE->f1=Cyc_Absyn_empty_tqual(0);_tmpCE->f2=(void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)_tmpC2->hd)->topt))->v;_tmpCE;});
_tmpCD->tl=pat_ts;_tmpCD;});}t=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpCF=
_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_TupleType_struct _tmpD0;
_tmpD0.tag=9;_tmpD0.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts);_tmpD0;});_tmpCF;});goto _LL3C;}_LL59: if(_tmp74 <= (void*)2)goto _LL5B;if(*((
int*)_tmp74)!= 5)goto _LL5B;_tmp84=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1;
_tmp85=_tmp84.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL5B;_tmp86=(_tmp85.KnownAggr).f1;_tmp87=*_tmp86;_tmp88=_tmp84.targs;
_tmp89=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).targs;
_tmp8A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f2;_tmp8B=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp74)->f3;_tmp8C=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp74)->f3;_tmp8D=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f4;_LL5A: {struct
Cyc_List_List*_tmpD1=*_tmp8C;if(_tmp87->impl == 0){({void*_tmpD2[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmpD3="can't destructure an abstract struct";_tag_dynforward(
_tmpD3,sizeof(char),_get_zero_arr_size(_tmpD3,37));}),_tag_dynforward(_tmpD2,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpD4=_cycalloc(
sizeof(*_tmpD4));_tmpD4->v=Cyc_Tcenv_lookup_type_vars(te);_tmpD4;}));goto _LL3C;}
if(_tmp8A != 0)rgn_opt=0;{struct _RegionHandle _tmpD5=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpD5;_push_region(rgn);{struct Cyc_List_List*_tmpD6=0;struct
Cyc_List_List*outlives_constraints=0;struct Cyc_List_List*_tmpD7=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp87->impl))->exist_vars;{struct Cyc_List_List*t=_tmp8A;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpD7))->hd;_tmpD7=
_tmpD7->tl;{void*_tmpD8=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpD9=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpDA=_tmpD9;void*_tmpDB;void*_tmpDC;
_LL7A: if(*((int*)_tmpDA)!= 2)goto _LL7C;_tmpDB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDA)->f2;_LL7B: _tmpDC=_tmpDB;goto _LL7D;_LL7C: if(*((int*)_tmpDA)!= 0)goto _LL7E;
_tmpDC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpDA)->f1;_LL7D: k2=_tmpDC;goto
_LL79;_LL7E:;_LL7F:({void*_tmpDD[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpDE="unconstrained existential type variable in struct";
_tag_dynforward(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,50));}),
_tag_dynforward(_tmpDD,sizeof(void*),0));});_LL79:;}{void*_tmpDF=_tmpD8;void*
_tmpE0;struct Cyc_Core_Opt*_tmpE1;struct Cyc_Core_Opt**_tmpE2;void*_tmpE3;struct
Cyc_Core_Opt*_tmpE4;struct Cyc_Core_Opt**_tmpE5;_LL81: if(*((int*)_tmpDF)!= 0)goto
_LL83;_tmpE0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpDF)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(
k2,_tmpE0))error=1;goto _LL80;_LL83: if(*((int*)_tmpDF)!= 2)goto _LL85;_tmpE1=((
struct Cyc_Absyn_Less_kb_struct*)_tmpDF)->f1;_tmpE2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpDF)->f1;_tmpE3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDF)->f2;_LL84: _tmpE5=_tmpE2;goto _LL86;_LL85: if(*((int*)_tmpDF)!= 1)goto _LL80;
_tmpE4=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpDF)->f1;_tmpE5=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmpDF)->f1;_LL86:*_tmpE5=({struct Cyc_Core_Opt*
_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->v=(void*)_tmpD9;_tmpE6;});goto _LL80;
_LL80:;}if(error)({struct Cyc_String_pa_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(k2));{
struct Cyc_String_pa_struct _tmpEA;_tmpEA.tag=0;_tmpEA.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(_tmpD8));{struct Cyc_String_pa_struct
_tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*tv->name);{
void*_tmpE7[3]={& _tmpE9,& _tmpEA,& _tmpEB};Cyc_Tcutil_terr(p->loc,({const char*
_tmpE8="type variable %s has kind %s but must have at least kind %s";
_tag_dynforward(_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,60));}),
_tag_dynforward(_tmpE7,sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_VarType_struct
_tmpF2;_tmpF2.tag=1;_tmpF2.f1=tv;_tmpF2;});_tmpF1;});_tmpD6=({struct Cyc_List_List*
_tmpEC=_region_malloc(rgn,sizeof(*_tmpEC));_tmpEC->hd=(void*)((void*)vartype);
_tmpEC->tl=_tmpD6;_tmpEC;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->hd=({struct _tuple0*_tmpEE=_cycalloc(
sizeof(*_tmpEE));_tmpEE->f1=Cyc_Absyn_empty_effect;_tmpEE->f2=vartype;_tmpEE;});
_tmpED->tl=outlives_constraints;_tmpED;});else{if(k2 == (void*)4  || k2 == (void*)5)({
void*_tmpEF[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpF0="opened existential had unique or top region kind";
_tag_dynforward(_tmpF0,sizeof(char),_get_zero_arr_size(_tmpF0,49));}),
_tag_dynforward(_tmpEF,sizeof(void*),0));});}}}}}_tmpD6=Cyc_List_imp_rev(_tmpD6);{
struct _RegionHandle _tmpF3=_new_region("r2");struct _RegionHandle*r2=& _tmpF3;
_push_region(r2);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,
_tmp8A);struct Cyc_List_List*_tmpF4=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4
_tmpF5=({struct _tuple4 _tmp115;_tmp115.f1=_tmpF4;_tmp115.f2=rgn;_tmp115;});struct
Cyc_List_List*_tmpF6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpF5,_tmp87->tvs);struct
Cyc_List_List*_tmpF7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,
rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->exist_vars,
_tmpD6);struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF6);struct Cyc_List_List*_tmpF9=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF7);struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpF6,_tmpF7);
if(_tmp8A != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=0;_tmpFB->f2=0;_tmpFB;});(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp8A);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpFC=0;{struct Cyc_List_List*_tmpFD=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->rgn_po;for(0;_tmpFD != 
0;_tmpFD=_tmpFD->tl){_tmpFC=({struct Cyc_List_List*_tmpFE=_cycalloc(sizeof(*
_tmpFE));_tmpFE->hd=({struct _tuple0*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpFA,(*((struct _tuple0*)_tmpFD->hd)).f1);_tmpFF->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpFA,(*((struct _tuple0*)_tmpFD->hd)).f2);_tmpFF;});
_tmpFE->tl=_tmpFC;_tmpFE;});}}_tmpFC=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpFC);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpFC);}}*_tmp89=_tmpF8;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp100=
_cycalloc(sizeof(*_tmp100));_tmp100[0]=({struct Cyc_Absyn_AggrType_struct _tmp101;
_tmp101.tag=10;_tmp101.f1=({struct Cyc_Absyn_AggrInfo _tmp102;_tmp102.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp103;(_tmp103.KnownAggr).tag=
1;(_tmp103.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp104=_cycalloc(sizeof(*
_tmp104));_tmp104[0]=_tmp87;_tmp104;});_tmp103;});_tmp102.targs=*_tmp89;_tmp102;});
_tmp101;});_tmp100;});if(_tmp8D){int _tmp105=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpD1);int _tmp106=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp87->impl))->fields);if(_tmp105 < _tmp106){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp106 - _tmp105;i ++){wild_dps=({struct Cyc_List_List*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->hd=({struct _tuple8*_tmp108=
_cycalloc(sizeof(*_tmp108));_tmp108->f1=0;_tmp108->f2=Cyc_Tcpat_wild_pat(p->loc);
_tmp108;});_tmp107->tl=wild_dps;_tmp107;});}}*_tmp8C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD1,
wild_dps);_tmpD1=*_tmp8C;}else{if(_tmp105 == _tmp106)({void*_tmp109[0]={};Cyc_Tcutil_warn(
p->loc,({const char*_tmp10A="unnecessary ... in struct pattern";_tag_dynforward(
_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,34));}),_tag_dynforward(_tmp109,
sizeof(void*),0));});}}{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,
_tmpD1,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->fields);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp10C;struct Cyc_Absyn_Aggrfield*
_tmp10D;struct Cyc_Absyn_Pat*_tmp10E;struct _tuple10*_tmp10B=(struct _tuple10*)
fields->hd;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;{void*_tmp10F=
Cyc_Tcutil_rsubstitute(rgn,_tmpFA,(void*)_tmp10D->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp10E,(void**)& _tmp10F,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10E->topt))->v,_tmp10F))({struct Cyc_String_pa_struct
_tmp114;_tmp114.tag=0;_tmp114.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp10E->topt))->v));{
struct Cyc_String_pa_struct _tmp113;_tmp113.tag=0;_tmp113.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp10F));{struct
Cyc_String_pa_struct _tmp112;_tmp112.tag=0;_tmp112.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp10D->name);{void*_tmp110[3]={& _tmp112,& _tmp113,&
_tmp114};Cyc_Tcutil_terr(p->loc,({const char*_tmp111="field %s of struct pattern expects type %s != %s";
_tag_dynforward(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,49));}),
_tag_dynforward(_tmp110,sizeof(void*),3));}}}});}}}};_pop_region(r2);}};
_pop_region(rgn);}goto _LL3C;}_LL5B: if(_tmp74 <= (void*)2)goto _LL5D;if(*((int*)
_tmp74)!= 6)goto _LL5D;_tmp8E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f1;
_tmp8F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f2;_tmp90=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp74)->f3;_tmp91=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tunion_p_struct*)
_tmp74)->f3;_tmp92=((struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f4;_LL5C: {struct
Cyc_List_List*_tmp116=*_tmp91;if(_tmp8E->is_flat)rgn_opt=0;{struct _RegionHandle
_tmp117=_new_region("rgn");struct _RegionHandle*rgn=& _tmp117;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp8F->typs;struct Cyc_List_List*_tmp118=Cyc_Tcenv_lookup_type_vars(
te);struct _tuple4 _tmp119=({struct _tuple4 _tmp142;_tmp142.f1=_tmp118;_tmp142.f2=
rgn;_tmp142;});struct Cyc_List_List*_tmp11A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp119,_tmp8E->tvs);struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp11A);if(tqts == 0  || _tmp8E->is_flat){struct Cyc_Core_Opt*ropt=0;
if(!_tmp8E->is_flat)ropt=({struct Cyc_Core_Opt*_tmp11C=_cycalloc(sizeof(*_tmp11C));
_tmp11C->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->v=_tmp118;
_tmp11D;}));_tmp11C;});t=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp11E=
_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_TunionType_struct
_tmp11F;_tmp11F.tag=2;_tmp11F.f1=({struct Cyc_Absyn_TunionInfo _tmp120;_tmp120.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union _tmp121;(
_tmp121.KnownTunion).tag=1;(_tmp121.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=_tmp8E;_tmp122;});_tmp121;});
_tmp120.targs=_tmp11B;_tmp120.rgn=ropt;_tmp120;});_tmp11F;});_tmp11E;});}else{t=(
void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp123=_cycalloc(sizeof(*_tmp123));
_tmp123[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp124;_tmp124.tag=3;_tmp124.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp125;_tmp125.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp126;(_tmp126.KnownTunionfield).tag=1;(
_tmp126.KnownTunionfield).f1=_tmp8E;(_tmp126.KnownTunionfield).f2=_tmp8F;_tmp126;});
_tmp125.targs=_tmp11B;_tmp125;});_tmp124;});_tmp123;});}if(topt != 0  && (tqts == 0
 || _tmp8E->is_flat)){void*_tmp127=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo
_tmp128;struct Cyc_List_List*_tmp129;_LL88: if(_tmp127 <= (void*)4)goto _LL8C;if(*((
int*)_tmp127)!= 3)goto _LL8A;_LL89: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp12B;_tmp12B.tag=3;_tmp12B.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp12C;
_tmp12C.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({union Cyc_Absyn_TunionFieldInfoU_union
_tmp12D;(_tmp12D.KnownTunionfield).tag=1;(_tmp12D.KnownTunionfield).f1=_tmp8E;(
_tmp12D.KnownTunionfield).f2=_tmp8F;_tmp12D;});_tmp12C.targs=_tmp11B;_tmp12C;});
_tmp12B;});_tmp12A;});goto _LL87;_LL8A: if(*((int*)_tmp127)!= 2)goto _LL8C;_tmp128=((
struct Cyc_Absyn_TunionType_struct*)_tmp127)->f1;_tmp129=_tmp128.targs;_LL8B: {
struct Cyc_List_List*_tmp12E=_tmp11B;for(0;_tmp12E != 0  && _tmp129 != 0;(_tmp12E=
_tmp12E->tl,_tmp129=_tmp129->tl)){Cyc_Tcutil_unify((void*)_tmp12E->hd,(void*)
_tmp129->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}if(_tmp92){int _tmp12F=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp116);int _tmp130=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp12F < _tmp130){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp130 - _tmp12F;i ++){wild_ps=({struct Cyc_List_List*
_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->hd=Cyc_Tcpat_wild_pat(p->loc);
_tmp131->tl=wild_ps;_tmp131;});}}*_tmp91=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp116,wild_ps);_tmp116=*_tmp91;}
else{if(_tmp12F == _tmp130)({struct Cyc_String_pa_struct _tmp134;_tmp134.tag=0;
_tmp134.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp8E->name));{void*_tmp132[1]={& _tmp134};Cyc_Tcutil_warn(p->loc,({const char*
_tmp133="unnecessary ... in tunion field %s";_tag_dynforward(_tmp133,sizeof(char),
_get_zero_arr_size(_tmp133,35));}),_tag_dynforward(_tmp132,sizeof(void*),1));}});}}
for(0;_tmp116 != 0  && tqts != 0;(_tmp116=_tmp116->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp135=(struct Cyc_Absyn_Pat*)_tmp116->hd;void*_tmp136=Cyc_Tcutil_rsubstitute(
rgn,_tmp11A,(*((struct _tuple9*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,
Cyc_Tcpat_tcPatRec(te,_tmp135,(void**)& _tmp136,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp135->topt))->v,_tmp136))({struct Cyc_String_pa_struct
_tmp13B;_tmp13B.tag=0;_tmp13B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp135->topt))->v));{
struct Cyc_String_pa_struct _tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp136));{struct
Cyc_String_pa_struct _tmp139;_tmp139.tag=0;_tmp139.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp8F->name));{void*_tmp137[3]={&
_tmp139,& _tmp13A,& _tmp13B};Cyc_Tcutil_terr(_tmp135->loc,({const char*_tmp138="%s expects argument type %s, not %s";
_tag_dynforward(_tmp138,sizeof(char),_get_zero_arr_size(_tmp138,36));}),
_tag_dynforward(_tmp137,sizeof(void*),3));}}}});}if(_tmp116 != 0)({struct Cyc_String_pa_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8F->name));{void*_tmp13C[1]={& _tmp13E};Cyc_Tcutil_terr(
p->loc,({const char*_tmp13D="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp13D,sizeof(char),_get_zero_arr_size(_tmp13D,45));}),
_tag_dynforward(_tmp13C,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct
_tmp141;_tmp141.tag=0;_tmp141.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8F->name));{void*_tmp13F[1]={& _tmp141};Cyc_Tcutil_terr(
p->loc,({const char*_tmp140="too few arguments for tunion constructor %s";
_tag_dynforward(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,44));}),
_tag_dynforward(_tmp13F,sizeof(void*),1));}});};_pop_region(rgn);}goto _LL3C;}
_LL5D: if(_tmp74 <= (void*)2)goto _LL5F;if(*((int*)_tmp74)!= 5)goto _LL5F;_tmp93=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1;_tmp94=_tmp93.aggr_info;if((((((
struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).aggr_info).UnknownAggr).tag != 0)goto
_LL5F;_LL5E: goto _LL60;_LL5F: if(_tmp74 <= (void*)2)goto _LL61;if(*((int*)_tmp74)!= 
12)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmp74 <= (void*)2)goto _LL63;if(*((int*)
_tmp74)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmp74 <= (void*)2)goto _LL3C;if(*((
int*)_tmp74)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp143;}));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp144=
_cycalloc(sizeof(*_tmp144));_tmp144->v=(void*)t;_tmp144;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp146=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp147=_new_region("r");struct _RegionHandle*r=& _tmp147;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp146.patvars),p->loc,({const char*
_tmp148="pattern contains a repeated variable";_tag_dynforward(_tmp148,sizeof(
char),_get_zero_arr_size(_tmp148,37));}));;_pop_region(r);}return _tmp146;}void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*
_tmp149=(void*)p->r;struct Cyc_Absyn_Pat*_tmp14A;struct Cyc_List_List*_tmp14B;
struct Cyc_List_List*_tmp14C;struct Cyc_List_List*_tmp14D;_LL8F: if(_tmp149 <= (void*)
2)goto _LL97;if(*((int*)_tmp149)!= 4)goto _LL91;_tmp14A=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp149)->f1;_LL90: Cyc_Tcpat_check_pat_regions(te,_tmp14A);{void*_tmp14E=(void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo _tmp14F;
struct Cyc_Absyn_PtrAtts _tmp150;void*_tmp151;struct Cyc_Absyn_TunionInfo _tmp152;
struct Cyc_Core_Opt*_tmp153;struct Cyc_Core_Opt _tmp154;void*_tmp155;_LL9A: if(
_tmp14E <= (void*)4)goto _LL9E;if(*((int*)_tmp14E)!= 4)goto _LL9C;_tmp14F=((struct
Cyc_Absyn_PointerType_struct*)_tmp14E)->f1;_tmp150=_tmp14F.ptr_atts;_tmp151=(
void*)_tmp150.rgn;_LL9B: _tmp155=_tmp151;goto _LL9D;_LL9C: if(*((int*)_tmp14E)!= 2)
goto _LL9E;_tmp152=((struct Cyc_Absyn_TunionType_struct*)_tmp14E)->f1;_tmp153=
_tmp152.rgn;if(_tmp153 == 0)goto _LL9E;_tmp154=*_tmp153;_tmp155=(void*)_tmp154.v;
_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp155);return;_LL9E:;_LL9F:({
void*_tmp156[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp157="check_pat_regions: bad pointer type";
_tag_dynforward(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,36));}),
_tag_dynforward(_tmp156,sizeof(void*),0));});_LL99:;}_LL91: if(*((int*)_tmp149)!= 
6)goto _LL93;_tmp14B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp149)->f3;_LL92: for(0;
_tmp14B != 0;_tmp14B=_tmp14B->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp14B->hd);}{void*_tmp158=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_TunionInfo _tmp159;struct Cyc_Core_Opt*_tmp15A;struct Cyc_Absyn_TunionInfo
_tmp15B;struct Cyc_Core_Opt*_tmp15C;struct Cyc_Core_Opt _tmp15D;void*_tmp15E;_LLA1:
if(_tmp158 <= (void*)4)goto _LLA7;if(*((int*)_tmp158)!= 2)goto _LLA3;_tmp159=((
struct Cyc_Absyn_TunionType_struct*)_tmp158)->f1;_tmp15A=_tmp159.rgn;if(_tmp15A != 
0)goto _LLA3;_LLA2: return;_LLA3: if(*((int*)_tmp158)!= 2)goto _LLA5;_tmp15B=((
struct Cyc_Absyn_TunionType_struct*)_tmp158)->f1;_tmp15C=_tmp15B.rgn;if(_tmp15C == 
0)goto _LLA5;_tmp15D=*_tmp15C;_tmp15E=(void*)_tmp15D.v;_LLA4: Cyc_Tcenv_check_rgn_accessible(
te,p->loc,_tmp15E);return;_LLA5: if(*((int*)_tmp158)!= 3)goto _LLA7;_LLA6: return;
_LLA7:;_LLA8:({void*_tmp15F[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp160="check_pat_regions: bad tunion type";
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,35));}),
_tag_dynforward(_tmp15F,sizeof(void*),0));});_LLA0:;}_LL93: if(*((int*)_tmp149)!= 
5)goto _LL95;_tmp14C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp149)->f3;_LL94: for(0;
_tmp14C != 0;_tmp14C=_tmp14C->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple8*)_tmp14C->hd)).f2);}return;_LL95: if(*((int*)_tmp149)!= 3)goto _LL97;
_tmp14D=((struct Cyc_Absyn_Tuple_p_struct*)_tmp149)->f1;_LL96: for(0;_tmp14D != 0;
_tmp14D=_tmp14D->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp14D->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};
union Cyc_Tcpat_Name_value_union{struct Cyc_Tcpat_Name_v_struct Name_v;struct Cyc_Tcpat_Int_v_struct
Int_v;};struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value_union name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2){union Cyc_Tcpat_Name_value_union _tmp161=c1->name;
struct _dynforward_ptr _tmp162;int _tmp163;_LLAA: if((_tmp161.Name_v).tag != 0)goto
_LLAC;_tmp162=(_tmp161.Name_v).f1;_LLAB: {union Cyc_Tcpat_Name_value_union _tmp164=
c2->name;struct _dynforward_ptr _tmp165;_LLAF: if((_tmp164.Name_v).tag != 0)goto
_LLB1;_tmp165=(_tmp164.Name_v).f1;_LLB0: return Cyc_strcmp((struct _dynforward_ptr)
_tmp162,(struct _dynforward_ptr)_tmp165);_LLB1: if((_tmp164.Int_v).tag != 1)goto
_LLAE;_LLB2: return - 1;_LLAE:;}_LLAC: if((_tmp161.Int_v).tag != 1)goto _LLA9;_tmp163=(
_tmp161.Int_v).f1;_LLAD: {union Cyc_Tcpat_Name_value_union _tmp166=c2->name;int
_tmp167;_LLB4: if((_tmp166.Name_v).tag != 0)goto _LLB6;_LLB5: return 1;_LLB6: if((
_tmp166.Int_v).tag != 1)goto _LLB3;_tmp167=(_tmp166.Int_v).f1;_LLB7: return _tmp163 - 
_tmp167;_LLB3:;}_LLA9:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct
_RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static int Cyc_Tcpat_one_opt=1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=
256;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp168=_region_malloc(r,sizeof(*
_tmp168));_tmp168->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp169;(_tmp169.Name_v).tag=0;(_tmp169.Name_v).f1=({const char*_tmp16A="NULL";
_tag_dynforward(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,5));});_tmp169;});
_tmp168->arity=0;_tmp168->span=(int*)& Cyc_Tcpat_two_opt;_tmp168->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp168;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp16B=_region_malloc(r,sizeof(*_tmp16B));_tmp16B->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp16C;(_tmp16C.Name_v).tag=0;(_tmp16C.Name_v).f1=({
const char*_tmp16D="&";_tag_dynforward(_tmp16D,sizeof(char),_get_zero_arr_size(
_tmp16D,2));});_tmp16C;});_tmp16B->arity=1;_tmp16B->span=(int*)& Cyc_Tcpat_two_opt;
_tmp16B->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp16B;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp16E=_region_malloc(r,sizeof(*_tmp16E));_tmp16E->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp16F;(_tmp16F.Name_v).tag=0;(_tmp16F.Name_v).f1=({
const char*_tmp170="&";_tag_dynforward(_tmp170,sizeof(char),_get_zero_arr_size(
_tmp170,2));});_tmp16F;});_tmp16E->arity=1;_tmp16E->span=(int*)& Cyc_Tcpat_one_opt;
_tmp16E->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp16E;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp171=_region_malloc(r,sizeof(*_tmp171));_tmp171->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp172;(
_tmp172.Int_v).tag=1;(_tmp172.Int_v).f1=i;_tmp172;});_tmp171->arity=0;_tmp171->span=
0;_tmp171->orig_pat=p;_tmp171;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(
struct _RegionHandle*r,struct _dynforward_ptr f,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp173=_region_malloc(r,sizeof(*_tmp173));_tmp173->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp174;(
_tmp174.Name_v).tag=0;(_tmp174.Name_v).f1=f;_tmp174;});_tmp173->arity=0;_tmp173->span=
0;_tmp173->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp173;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp175=_region_malloc(r,sizeof(*_tmp175));_tmp175->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp176;(
_tmp176.Int_v).tag=1;(_tmp176.Int_v).f1=(int)c;_tmp176;});_tmp175->arity=0;
_tmp175->span=(int*)& Cyc_Tcpat_twofiftysix_opt;_tmp175->orig_pat=(struct Cyc_Absyn_Pat*)
p;_tmp175;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp177=_region_malloc(r,sizeof(*_tmp177));_tmp177->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp178;(_tmp178.Name_v).tag=0;(_tmp178.Name_v).f1=({
const char*_tmp179="$";_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(
_tmp179,2));});_tmp178;});_tmp177->arity=i;_tmp177->span=(int*)& Cyc_Tcpat_one_opt;
_tmp177->orig_pat=p;_tmp177;});}static void*Cyc_Tcpat_null_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp17A=_region_malloc(r,sizeof(*_tmp17A));_tmp17A[0]=({struct Cyc_Tcpat_Con_struct
_tmp17B;_tmp17B.tag=0;_tmp17B.f1=Cyc_Tcpat_null_con(r,p);_tmp17B.f2=0;_tmp17B;});
_tmp17A;});}static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*
p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp17C=_region_malloc(r,sizeof(*
_tmp17C));_tmp17C[0]=({struct Cyc_Tcpat_Con_struct _tmp17D;_tmp17D.tag=0;_tmp17D.f1=
Cyc_Tcpat_int_con(r,i,p);_tmp17D.f2=0;_tmp17D;});_tmp17C;});}static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp17E=_region_malloc(r,sizeof(*_tmp17E));_tmp17E[0]=({struct Cyc_Tcpat_Con_struct
_tmp17F;_tmp17F.tag=0;_tmp17F.f1=Cyc_Tcpat_char_con(r,c,p);_tmp17F.f2=0;_tmp17F;});
_tmp17E;});}static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct
_dynforward_ptr f,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp180=_region_malloc(r,sizeof(*_tmp180));_tmp180[0]=({struct Cyc_Tcpat_Con_struct
_tmp181;_tmp181.tag=0;_tmp181.f1=Cyc_Tcpat_float_con(r,f,p);_tmp181.f2=0;_tmp181;});
_tmp180;});}static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,
struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp182=
_region_malloc(r,sizeof(*_tmp182));_tmp182[0]=({struct Cyc_Tcpat_Con_struct
_tmp183;_tmp183.tag=0;_tmp183.f1=Cyc_Tcpat_null_ptr_con(r,p0);_tmp183.f2=({
struct Cyc_List_List*_tmp184=_region_malloc(r,sizeof(*_tmp184));_tmp184->hd=(void*)
p;_tmp184->tl=0;_tmp184;});_tmp183;});_tmp182;});}static void*Cyc_Tcpat_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp185=_region_malloc(r,sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Tcpat_Con_struct
_tmp186;_tmp186.tag=0;_tmp186.f1=Cyc_Tcpat_ptr_con(r,p0);_tmp186.f2=({struct Cyc_List_List*
_tmp187=_region_malloc(r,sizeof(*_tmp187));_tmp187->hd=(void*)p;_tmp187->tl=0;
_tmp187;});_tmp186;});_tmp185;});}static void*Cyc_Tcpat_tuple_pat(struct
_RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp188=_region_malloc(r,sizeof(*_tmp188));_tmp188[0]=({
struct Cyc_Tcpat_Con_struct _tmp189;_tmp189.tag=0;_tmp189.f1=Cyc_Tcpat_tuple_con(r,
Cyc_List_length(ss),p);_tmp189.f2=ss;_tmp189;});_tmp188;});}static void*Cyc_Tcpat_con_pat(
struct _RegionHandle*r,struct _dynforward_ptr con_name,int*span,struct Cyc_List_List*
ps,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*
_tmp18C=_region_malloc(r,sizeof(*_tmp18C));_tmp18C->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp18D;(_tmp18D.Name_v).tag=0;(_tmp18D.Name_v).f1=
con_name;_tmp18D;});_tmp18C->arity=Cyc_List_length(ps);_tmp18C->span=span;
_tmp18C->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp18C;});return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp18A=_region_malloc(r,sizeof(*_tmp18A));_tmp18A[0]=({struct Cyc_Tcpat_Con_struct
_tmp18B;_tmp18B.tag=0;_tmp18B.f1=c;_tmp18B.f2=ps;_tmp18B;});_tmp18A;});}static
void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){void*s;{
void*_tmp18E=(void*)p->r;void*_tmp18F;int _tmp190;char _tmp191;struct
_dynforward_ptr _tmp192;struct Cyc_Absyn_Pat*_tmp193;struct Cyc_Absyn_Pat*_tmp194;
struct Cyc_Absyn_Pat*_tmp195;struct Cyc_Absyn_Tuniondecl*_tmp196;struct Cyc_Absyn_Tunionfield*
_tmp197;struct Cyc_List_List*_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_Absyn_AggrInfo
_tmp19A;union Cyc_Absyn_AggrInfoU_union _tmp19B;struct Cyc_Absyn_Aggrdecl**_tmp19C;
struct Cyc_Absyn_Aggrdecl*_tmp19D;struct Cyc_List_List*_tmp19E;struct Cyc_Absyn_Enumdecl*
_tmp19F;struct Cyc_Absyn_Enumfield*_tmp1A0;void*_tmp1A1;struct Cyc_Absyn_Enumfield*
_tmp1A2;_LLB9: if((int)_tmp18E != 0)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(_tmp18E <= (
void*)2)goto _LLBD;if(*((int*)_tmp18E)!= 2)goto _LLBD;_LLBC: s=(void*)0;goto _LLB8;
_LLBD: if((int)_tmp18E != 1)goto _LLBF;_LLBE: s=Cyc_Tcpat_null_pat(r,p);goto _LLB8;
_LLBF: if(_tmp18E <= (void*)2)goto _LLC1;if(*((int*)_tmp18E)!= 7)goto _LLC1;_tmp18F=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp18E)->f1;_tmp190=((struct Cyc_Absyn_Int_p_struct*)
_tmp18E)->f2;_LLC0: s=Cyc_Tcpat_int_pat(r,_tmp190,(struct Cyc_Absyn_Pat*)p);goto
_LLB8;_LLC1: if(_tmp18E <= (void*)2)goto _LLC3;if(*((int*)_tmp18E)!= 8)goto _LLC3;
_tmp191=((struct Cyc_Absyn_Char_p_struct*)_tmp18E)->f1;_LLC2: s=Cyc_Tcpat_char_pat(
r,_tmp191,p);goto _LLB8;_LLC3: if(_tmp18E <= (void*)2)goto _LLC5;if(*((int*)_tmp18E)
!= 9)goto _LLC5;_tmp192=((struct Cyc_Absyn_Float_p_struct*)_tmp18E)->f1;_LLC4: s=
Cyc_Tcpat_float_pat(r,_tmp192,p);goto _LLB8;_LLC5: if(_tmp18E <= (void*)2)goto _LLC7;
if(*((int*)_tmp18E)!= 0)goto _LLC7;_tmp193=((struct Cyc_Absyn_Var_p_struct*)
_tmp18E)->f2;_LLC6: s=Cyc_Tcpat_compile_pat(r,_tmp193);goto _LLB8;_LLC7: if(_tmp18E
<= (void*)2)goto _LLC9;if(*((int*)_tmp18E)!= 1)goto _LLC9;_tmp194=((struct Cyc_Absyn_Reference_p_struct*)
_tmp18E)->f2;_LLC8: s=Cyc_Tcpat_compile_pat(r,_tmp194);goto _LLB8;_LLC9: if(_tmp18E
<= (void*)2)goto _LLCB;if(*((int*)_tmp18E)!= 4)goto _LLCB;_tmp195=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp18E)->f1;_LLCA:{void*_tmp1A3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1A4;struct Cyc_Absyn_PtrAtts
_tmp1A5;struct Cyc_Absyn_Conref*_tmp1A6;struct Cyc_Absyn_TunionInfo _tmp1A7;struct
Cyc_Core_Opt*_tmp1A8;struct Cyc_Core_Opt _tmp1A9;_LLDE: if(_tmp1A3 <= (void*)4)goto
_LLE2;if(*((int*)_tmp1A3)!= 4)goto _LLE0;_tmp1A4=((struct Cyc_Absyn_PointerType_struct*)
_tmp1A3)->f1;_tmp1A5=_tmp1A4.ptr_atts;_tmp1A6=_tmp1A5.nullable;_LLDF: {int
is_nullable=0;int still_working=1;while(still_working){union Cyc_Absyn_Constraint_union
_tmp1AA=_tmp1A6->v;struct Cyc_Absyn_Conref*_tmp1AB;int _tmp1AC;_LLE5: if((_tmp1AA.Forward_constr).tag
!= 1)goto _LLE7;_tmp1AB=(_tmp1AA.Forward_constr).f1;_LLE6: _tmp1A6->v=_tmp1AB->v;
continue;_LLE7: if((_tmp1AA.No_constr).tag != 2)goto _LLE9;_LLE8: _tmp1A6->v=(union
Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp1AD;(_tmp1AD.Eq_constr).tag=
0;(_tmp1AD.Eq_constr).f1=(void*)0;_tmp1AD;});is_nullable=0;still_working=0;goto
_LLE4;_LLE9: if((_tmp1AA.Eq_constr).tag != 0)goto _LLE4;_tmp1AC=(_tmp1AA.Eq_constr).f1;
_LLEA: is_nullable=(int)_tmp1AC;still_working=0;goto _LLE4;_LLE4:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp195);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLDD;}}_LLE0: if(*((int*)_tmp1A3)!= 2)goto _LLE2;_tmp1A7=((struct Cyc_Absyn_TunionType_struct*)
_tmp1A3)->f1;_tmp1A8=_tmp1A7.rgn;if(_tmp1A8 == 0)goto _LLE2;_tmp1A9=*_tmp1A8;_LLE1:{
void*_tmp1AE=(void*)_tmp195->r;struct Cyc_Absyn_Tuniondecl*_tmp1AF;struct Cyc_Absyn_Tunionfield*
_tmp1B0;struct Cyc_List_List*_tmp1B1;_LLEC: if(_tmp1AE <= (void*)2)goto _LLEE;if(*((
int*)_tmp1AE)!= 6)goto _LLEE;_tmp1AF=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AE)->f1;
_tmp1B0=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AE)->f2;_tmp1B1=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1AE)->f3;_LLED: {int*span;if(_tmp1AF->is_xtunion)span=0;else{span=({int*
_tmp1B2=_region_malloc(r,sizeof(*_tmp1B2));_tmp1B2[0]=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1AF->fields))->v);_tmp1B2;});}s=Cyc_Tcpat_con_pat(r,*(*_tmp1B0->name).f2,
span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1B1),p);goto _LLEB;}_LLEE:;
_LLEF:({void*_tmp1B3[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B4="non-[x]tunion pattern has tunion type";
_tag_dynforward(_tmp1B4,sizeof(char),_get_zero_arr_size(_tmp1B4,38));}),
_tag_dynforward(_tmp1B3,sizeof(void*),0));});_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({
void*_tmp1B5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1B6="pointer pattern does not have pointer type";
_tag_dynforward(_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,43));}),
_tag_dynforward(_tmp1B5,sizeof(void*),0));});_LLDD:;}goto _LLB8;_LLCB: if(_tmp18E
<= (void*)2)goto _LLCD;if(*((int*)_tmp18E)!= 6)goto _LLCD;_tmp196=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp18E)->f1;_tmp197=((struct Cyc_Absyn_Tunion_p_struct*)_tmp18E)->f2;_tmp198=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp18E)->f3;_LLCC: {int*span;{void*_tmp1B7=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LLF1: if(_tmp1B7 <= (void*)
4)goto _LLF5;if(*((int*)_tmp1B7)!= 2)goto _LLF3;_LLF2: if(_tmp196->is_xtunion)span=
0;else{span=({int*_tmp1B8=_region_malloc(r,sizeof(*_tmp1B8));_tmp1B8[0]=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp196->fields))->v);_tmp1B8;});}goto _LLF0;_LLF3: if(*((int*)_tmp1B7)
!= 3)goto _LLF5;_LLF4: span=(int*)& Cyc_Tcpat_one_opt;goto _LLF0;_LLF5:;_LLF6: span=({
void*_tmp1B9[0]={};((int*(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1BA="void tunion pattern has bad type";
_tag_dynforward(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,33));}),
_tag_dynforward(_tmp1B9,sizeof(void*),0));});goto _LLF0;_LLF0:;}s=Cyc_Tcpat_con_pat(
r,*(*_tmp197->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp198),p);goto _LLB8;}_LLCD: if(
_tmp18E <= (void*)2)goto _LLCF;if(*((int*)_tmp18E)!= 3)goto _LLCF;_tmp199=((struct
Cyc_Absyn_Tuple_p_struct*)_tmp18E)->f1;_LLCE: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp199),(struct Cyc_Absyn_Pat*)p);goto _LLB8;_LLCF: if(_tmp18E <= (void*)2)goto
_LLD1;if(*((int*)_tmp18E)!= 5)goto _LLD1;_tmp19A=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp18E)->f1;_tmp19B=_tmp19A.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)
_tmp18E)->f1).aggr_info).KnownAggr).tag != 1)goto _LLD1;_tmp19C=(_tmp19B.KnownAggr).f1;
_tmp19D=*_tmp19C;_tmp19E=((struct Cyc_Absyn_Aggr_p_struct*)_tmp18E)->f3;_LLD0: {
struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp19D->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=
Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({
const char*_tmp1C9="";_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(
_tmp1C9,1));}))== 0;{struct Cyc_List_List*dlps0=_tmp19E;for(0;!found  && dlps0 != 0;
dlps0=dlps0->tl){struct _tuple8 _tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Pat*
_tmp1BE;struct _tuple8*_tmp1BB=(struct _tuple8*)dlps0->hd;_tmp1BC=*_tmp1BB;_tmp1BD=
_tmp1BC.f1;_tmp1BE=_tmp1BC.f2;{struct Cyc_List_List*_tmp1BF=_tmp1BD;struct Cyc_List_List
_tmp1C0;void*_tmp1C1;struct _dynforward_ptr*_tmp1C2;struct Cyc_List_List*_tmp1C3;
_LLF8: if(_tmp1BF == 0)goto _LLFA;_tmp1C0=*_tmp1BF;_tmp1C1=(void*)_tmp1C0.hd;if(*((
int*)_tmp1C1)!= 1)goto _LLFA;_tmp1C2=((struct Cyc_Absyn_FieldName_struct*)_tmp1C1)->f1;
_tmp1C3=_tmp1C0.tl;if(_tmp1C3 != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1C2,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp1C4=_region_malloc(r,sizeof(*_tmp1C4));_tmp1C4->hd=(void*)Cyc_Tcpat_compile_pat(
r,_tmp1BE);_tmp1C4->tl=ps;_tmp1C4;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*
_tmp1C5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1C6="bad designator(s)";_tag_dynforward(_tmp1C6,sizeof(char),
_get_zero_arr_size(_tmp1C6,18));}),_tag_dynforward(_tmp1C5,sizeof(void*),0));});
_LLF7:;}}}if(!found)({void*_tmp1C7[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C8="bad designator";
_tag_dynforward(_tmp1C8,sizeof(char),_get_zero_arr_size(_tmp1C8,15));}),
_tag_dynforward(_tmp1C7,sizeof(void*),0));});}}s=Cyc_Tcpat_tuple_pat(r,ps,(
struct Cyc_Absyn_Pat*)p);goto _LLB8;}_LLD1: if(_tmp18E <= (void*)2)goto _LLD3;if(*((
int*)_tmp18E)!= 10)goto _LLD3;_tmp19F=((struct Cyc_Absyn_Enum_p_struct*)_tmp18E)->f1;
_tmp1A0=((struct Cyc_Absyn_Enum_p_struct*)_tmp18E)->f2;_LLD2: {int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp19F->fields))->v);s=Cyc_Tcpat_con_pat(r,*(*_tmp1A0->name).f2,({
int*_tmp1CA=_region_malloc(r,sizeof(*_tmp1CA));_tmp1CA[0]=span;_tmp1CA;}),0,p);
goto _LLB8;}_LLD3: if(_tmp18E <= (void*)2)goto _LLD5;if(*((int*)_tmp18E)!= 11)goto
_LLD5;_tmp1A1=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp18E)->f1;_tmp1A2=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp18E)->f2;_LLD4: {struct Cyc_List_List*
fields;{void*_tmp1CB=Cyc_Tcutil_compress(_tmp1A1);struct Cyc_List_List*_tmp1CC;
_LLFD: if(_tmp1CB <= (void*)4)goto _LLFF;if(*((int*)_tmp1CB)!= 13)goto _LLFF;_tmp1CC=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp1CB)->f1;_LLFE: fields=_tmp1CC;goto _LLFC;
_LLFF:;_LL100:({void*_tmp1CD[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1CE="bad type in AnonEnum_p";
_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,23));}),
_tag_dynforward(_tmp1CD,sizeof(void*),0));});_LLFC:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(r,*(*_tmp1A2->name).f2,({int*
_tmp1CF=_region_malloc(r,sizeof(*_tmp1CF));_tmp1CF[0]=span;_tmp1CF;}),0,p);goto
_LLB8;}}_LLD5: if(_tmp18E <= (void*)2)goto _LLD7;if(*((int*)_tmp18E)!= 12)goto _LLD7;
_LLD6: goto _LLD8;_LLD7: if(_tmp18E <= (void*)2)goto _LLD9;if(*((int*)_tmp18E)!= 13)
goto _LLD9;_LLD8: goto _LLDA;_LLD9: if(_tmp18E <= (void*)2)goto _LLDB;if(*((int*)
_tmp18E)!= 5)goto _LLDB;_LLDA: goto _LLDC;_LLDB: if(_tmp18E <= (void*)2)goto _LLB8;if(*((
int*)_tmp18E)!= 14)goto _LLB8;_LLDC: s=(void*)0;_LLB8:;}return s;}struct Cyc_Tcpat_Pos_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};
struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp1D0=td;struct Cyc_Set_Set*_tmp1D1;_LL102: if(*((int*)_tmp1D0)!= 1)goto
_LL104;_tmp1D1=((struct Cyc_Tcpat_Neg_struct*)_tmp1D0)->f1;_LL103: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1D1,c))({void*
_tmp1D2[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1D3="add_neg called when constructor already in set";
_tag_dynforward(_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,47));}),
_tag_dynforward(_tmp1D2,sizeof(void*),0));});if(c->span != 0  && ((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1D1)+ 1 >= *((int*)_check_null(c->span)))({
void*_tmp1D4[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1D5="add_neg called when |cs U {c}| >= span(c)";
_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,42));}),
_tag_dynforward(_tmp1D4,sizeof(void*),0));});return(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp1D6=_region_malloc(r,sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1D7;_tmp1D7.tag=1;_tmp1D7.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp1D1,c);
_tmp1D7;});_tmp1D6;});_LL104: if(*((int*)_tmp1D0)!= 0)goto _LL101;_LL105:({void*
_tmp1D8[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1D9="add_neg called when td is Positive";_tag_dynforward(_tmp1D9,
sizeof(char),_get_zero_arr_size(_tmp1D9,35));}),_tag_dynforward(_tmp1D8,sizeof(
void*),0));});_LL101:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,
void*td){void*_tmp1DA=td;struct Cyc_Tcpat_Con_s*_tmp1DB;struct Cyc_Set_Set*_tmp1DC;
_LL107: if(*((int*)_tmp1DA)!= 0)goto _LL109;_tmp1DB=((struct Cyc_Tcpat_Pos_struct*)
_tmp1DA)->f1;_LL108: if(Cyc_Tcpat_compare_con(c,_tmp1DB)== 0)return(void*)0;else{
return(void*)1;}_LL109: if(*((int*)_tmp1DA)!= 1)goto _LL106;_tmp1DC=((struct Cyc_Tcpat_Neg_struct*)
_tmp1DA)->f1;_LL10A: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1DC,c))return(void*)1;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1DC)+ 1)return(void*)0;else{return(void*)2;}}_LL106:;}struct _tuple12{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp1DD=ctxt;struct Cyc_List_List _tmp1DE;struct _tuple12*_tmp1DF;struct _tuple12
_tmp1E0;struct Cyc_Tcpat_Con_s*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List*
_tmp1E3;_LL10C: if(_tmp1DD != 0)goto _LL10E;_LL10D: return 0;_LL10E: if(_tmp1DD == 0)
goto _LL10B;_tmp1DE=*_tmp1DD;_tmp1DF=(struct _tuple12*)_tmp1DE.hd;_tmp1E0=*_tmp1DF;
_tmp1E1=_tmp1E0.f1;_tmp1E2=_tmp1E0.f2;_tmp1E3=_tmp1DE.tl;_LL10F: return({struct
Cyc_List_List*_tmp1E4=_region_malloc(r,sizeof(*_tmp1E4));_tmp1E4->hd=({struct
_tuple12*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->f1=_tmp1E1;_tmp1E5->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1E6=_region_malloc(r,sizeof(*
_tmp1E6));_tmp1E6->hd=(void*)dsc;_tmp1E6->tl=_tmp1E2;_tmp1E6;});_tmp1E5;});
_tmp1E4->tl=_tmp1E3;_tmp1E4;});_LL10B:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1E7=ctxt;
struct Cyc_List_List _tmp1E8;struct _tuple12*_tmp1E9;struct _tuple12 _tmp1EA;struct
Cyc_Tcpat_Con_s*_tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_List_List*_tmp1ED;
_LL111: if(_tmp1E7 != 0)goto _LL113;_LL112:({void*_tmp1EE[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp1EF="norm_context: empty context";_tag_dynforward(_tmp1EF,sizeof(char),
_get_zero_arr_size(_tmp1EF,28));}),_tag_dynforward(_tmp1EE,sizeof(void*),0));});
_LL113: if(_tmp1E7 == 0)goto _LL110;_tmp1E8=*_tmp1E7;_tmp1E9=(struct _tuple12*)
_tmp1E8.hd;_tmp1EA=*_tmp1E9;_tmp1EB=_tmp1EA.f1;_tmp1EC=_tmp1EA.f2;_tmp1ED=
_tmp1E8.tl;_LL114: return Cyc_Tcpat_augment(r,_tmp1ED,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1F1;_tmp1F1.tag=0;_tmp1F1.f1=_tmp1EB;_tmp1F1.f2=Cyc_List_rrev(r,_tmp1EC);
_tmp1F1;});_tmp1F0;}));_LL110:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1F3=({struct _tuple6 _tmp1F2;_tmp1F2.f1=ctxt;_tmp1F2.f2=work;
_tmp1F2;});struct Cyc_List_List*_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_List_List*
_tmp1F6;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*_tmp1F8;struct Cyc_List_List
_tmp1F9;struct _tuple12*_tmp1FA;struct _tuple12 _tmp1FB;struct Cyc_Tcpat_Con_s*
_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*
_tmp1FF;struct Cyc_List_List _tmp200;struct _tuple11*_tmp201;struct _tuple11 _tmp202;
struct Cyc_List_List*_tmp203;struct Cyc_List_List*_tmp204;_LL116: _tmp1F4=_tmp1F3.f1;
if(_tmp1F4 != 0)goto _LL118;_tmp1F5=_tmp1F3.f2;if(_tmp1F5 != 0)goto _LL118;_LL117:
return dsc;_LL118: _tmp1F6=_tmp1F3.f1;if(_tmp1F6 != 0)goto _LL11A;_LL119: goto _LL11B;
_LL11A: _tmp1F7=_tmp1F3.f2;if(_tmp1F7 != 0)goto _LL11C;_LL11B:({void*_tmp205[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp206="build_desc: ctxt and work don't match";_tag_dynforward(
_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,38));}),_tag_dynforward(_tmp205,
sizeof(void*),0));});_LL11C: _tmp1F8=_tmp1F3.f1;if(_tmp1F8 == 0)goto _LL115;_tmp1F9=*
_tmp1F8;_tmp1FA=(struct _tuple12*)_tmp1F9.hd;_tmp1FB=*_tmp1FA;_tmp1FC=_tmp1FB.f1;
_tmp1FD=_tmp1FB.f2;_tmp1FE=_tmp1F9.tl;_tmp1FF=_tmp1F3.f2;if(_tmp1FF == 0)goto
_LL115;_tmp200=*_tmp1FF;_tmp201=(struct _tuple11*)_tmp200.hd;_tmp202=*_tmp201;
_tmp203=_tmp202.f3;_tmp204=_tmp200.tl;_LL11D: {struct Cyc_Tcpat_Pos_struct*
_tmp207=({struct Cyc_Tcpat_Pos_struct*_tmp208=_region_malloc(r,sizeof(*_tmp208));
_tmp208[0]=({struct Cyc_Tcpat_Pos_struct _tmp209;_tmp209.tag=0;_tmp209.f1=_tmp1FC;
_tmp209.f2=Cyc_List_rappend(r,Cyc_List_rrev(r,_tmp1FD),({struct Cyc_List_List*
_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->hd=(void*)dsc;_tmp20A->tl=
_tmp203;_tmp20A;}));_tmp209;});_tmp208;});return Cyc_Tcpat_build_desc(r,_tmp1FE,(
void*)_tmp207,_tmp204);}_LL115:;}static void*Cyc_Tcpat_match(struct _RegionHandle*
r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(
struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*
_tmp20B=allmrules;struct Cyc_List_List _tmp20C;struct _tuple0*_tmp20D;struct _tuple0
_tmp20E;void*_tmp20F;void*_tmp210;struct Cyc_List_List*_tmp211;_LL11F: if(_tmp20B
!= 0)goto _LL121;_LL120: return(void*)({struct Cyc_Tcpat_Failure_struct*_tmp212=
_cycalloc(sizeof(*_tmp212));_tmp212[0]=({struct Cyc_Tcpat_Failure_struct _tmp213;
_tmp213.tag=0;_tmp213.f1=(void*)dsc;_tmp213;});_tmp212;});_LL121: if(_tmp20B == 0)
goto _LL11E;_tmp20C=*_tmp20B;_tmp20D=(struct _tuple0*)_tmp20C.hd;_tmp20E=*_tmp20D;
_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;_tmp211=_tmp20C.tl;_LL122: return Cyc_Tcpat_match(
r,_tmp20F,0,dsc,0,0,_tmp210,_tmp211);_LL11E:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(
void*)({struct Cyc_Tcpat_Neg_struct*_tmp214=_region_malloc(r,sizeof(*_tmp214));
_tmp214[0]=({struct Cyc_Tcpat_Neg_struct _tmp215;_tmp215.tag=1;_tmp215.f1=Cyc_Tcpat_empty_con_set(
r);_tmp215;});_tmp214;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp216=work;
struct Cyc_List_List _tmp217;struct _tuple11*_tmp218;struct _tuple11 _tmp219;struct
Cyc_List_List*_tmp21A;struct Cyc_List_List*_tmp21B;struct Cyc_List_List*_tmp21C;
struct Cyc_List_List*_tmp21D;struct Cyc_List_List _tmp21E;struct _tuple11*_tmp21F;
struct _tuple11 _tmp220;struct Cyc_List_List*_tmp221;struct Cyc_List_List*_tmp222;
struct Cyc_List_List*_tmp223;struct Cyc_List_List*_tmp224;_LL124: if(_tmp216 != 0)
goto _LL126;_LL125: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp225=
_region_malloc(r,sizeof(*_tmp225));_tmp225[0]=({struct Cyc_Tcpat_Success_struct
_tmp226;_tmp226.tag=1;_tmp226.f1=(void*)right_hand_side;_tmp226;});_tmp225;});
_LL126: if(_tmp216 == 0)goto _LL128;_tmp217=*_tmp216;_tmp218=(struct _tuple11*)
_tmp217.hd;_tmp219=*_tmp218;_tmp21A=_tmp219.f1;if(_tmp21A != 0)goto _LL128;_tmp21B=
_tmp219.f2;if(_tmp21B != 0)goto _LL128;_tmp21C=_tmp219.f3;if(_tmp21C != 0)goto
_LL128;_tmp21D=_tmp217.tl;_LL127: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp21D,right_hand_side,rules);_LL128: if(_tmp216 == 0)goto _LL123;_tmp21E=*
_tmp216;_tmp21F=(struct _tuple11*)_tmp21E.hd;_tmp220=*_tmp21F;_tmp221=_tmp220.f1;
_tmp222=_tmp220.f2;_tmp223=_tmp220.f3;_tmp224=_tmp21E.tl;_LL129: if((_tmp221 == 0
 || _tmp222 == 0) || _tmp223 == 0)({void*_tmp227[0]={};Cyc_Tcutil_impos(({const
char*_tmp228="tcpat:and_match: malformed work frame";_tag_dynforward(_tmp228,
sizeof(char),_get_zero_arr_size(_tmp228,38));}),_tag_dynforward(_tmp227,sizeof(
void*),0));});{struct Cyc_List_List _tmp22A;void*_tmp22B;struct Cyc_List_List*
_tmp22C;struct Cyc_List_List*_tmp229=(struct Cyc_List_List*)_tmp221;_tmp22A=*
_tmp229;_tmp22B=(void*)_tmp22A.hd;_tmp22C=_tmp22A.tl;{struct Cyc_List_List _tmp22E;
struct Cyc_List_List*_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_List_List*
_tmp22D=(struct Cyc_List_List*)_tmp222;_tmp22E=*_tmp22D;_tmp22F=(struct Cyc_List_List*)
_tmp22E.hd;_tmp230=_tmp22E.tl;{struct Cyc_List_List _tmp232;void*_tmp233;struct Cyc_List_List*
_tmp234;struct Cyc_List_List*_tmp231=(struct Cyc_List_List*)_tmp223;_tmp232=*
_tmp231;_tmp233=(void*)_tmp232.hd;_tmp234=_tmp232.tl;{struct _tuple11*_tmp235=({
struct _tuple11*_tmp237=_region_malloc(r,sizeof(*_tmp237));_tmp237->f1=_tmp22C;
_tmp237->f2=_tmp230;_tmp237->f3=_tmp234;_tmp237;});return Cyc_Tcpat_match(r,
_tmp22B,_tmp22F,_tmp233,ctx,({struct Cyc_List_List*_tmp236=_region_malloc(r,
sizeof(*_tmp236));_tmp236->hd=_tmp235;_tmp236->tl=_tmp224;_tmp236;}),
right_hand_side,rules);}}}}_LL123:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp238=dsc;
struct Cyc_Set_Set*_tmp239;struct Cyc_List_List*_tmp23A;_LL12B: if(*((int*)_tmp238)
!= 1)goto _LL12D;_tmp239=((struct Cyc_Tcpat_Neg_struct*)_tmp238)->f1;_LL12C: {void*
any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp23D=_region_malloc(r,sizeof(*_tmp23D));
_tmp23D[0]=({struct Cyc_Tcpat_Neg_struct _tmp23E;_tmp23E.tag=1;_tmp23E.f1=Cyc_Tcpat_empty_con_set(
r);_tmp23E;});_tmp23D;});struct Cyc_List_List*_tmp23B=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp23B=({struct Cyc_List_List*_tmp23C=_region_malloc(r,sizeof(*_tmp23C));
_tmp23C->hd=(void*)any;_tmp23C->tl=_tmp23B;_tmp23C;});}}return _tmp23B;}_LL12D:
if(*((int*)_tmp238)!= 0)goto _LL12A;_tmp23A=((struct Cyc_Tcpat_Pos_struct*)_tmp238)->f2;
_LL12E: return _tmp23A;_LL12A:;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct
_tuple13 _tmp240;struct _RegionHandle*_tmp241;struct Cyc_List_List*_tmp242;struct
_tuple13*_tmp23F=env;_tmp240=*_tmp23F;_tmp241=_tmp240.f1;_tmp242=_tmp240.f2;
return({struct Cyc_List_List*_tmp243=_region_malloc(_tmp241,sizeof(*_tmp243));
_tmp243->hd=(void*)(i + 1);_tmp243->tl=_tmp242;_tmp243;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple13 _tmp244=({struct _tuple13 _tmp245;_tmp245.f1=r;_tmp245.f2=obj;
_tmp245;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp244);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp246=p;struct Cyc_Tcpat_Con_s*
_tmp247;struct Cyc_List_List*_tmp248;_LL130: if((int)_tmp246 != 0)goto _LL132;_LL131:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL132: if(_tmp246 <= (void*)1)goto _LL12F;if(*((int*)_tmp246)!= 0)goto
_LL12F;_tmp247=((struct Cyc_Tcpat_Con_struct*)_tmp246)->f1;_tmp248=((struct Cyc_Tcpat_Con_struct*)
_tmp246)->f2;_LL133: {void*_tmp249=Cyc_Tcpat_static_match(_tmp247,dsc);_LL135:
if((int)_tmp249 != 0)goto _LL137;_LL136: {struct Cyc_List_List*ctx2=({struct Cyc_List_List*
_tmp24C=_region_malloc(r,sizeof(*_tmp24C));_tmp24C->hd=({struct _tuple12*_tmp24D=
_region_malloc(r,sizeof(*_tmp24D));_tmp24D->f1=_tmp247;_tmp24D->f2=0;_tmp24D;});
_tmp24C->tl=ctx;_tmp24C;});struct _tuple11*work_frame=({struct _tuple11*_tmp24B=
_region_malloc(r,sizeof(*_tmp24B));_tmp24B->f1=_tmp248;_tmp24B->f2=Cyc_Tcpat_getoargs(
r,_tmp247,obj);_tmp24B->f3=Cyc_Tcpat_getdargs(r,_tmp247,dsc);_tmp24B;});struct
Cyc_List_List*work2=({struct Cyc_List_List*_tmp24A=_region_malloc(r,sizeof(*
_tmp24A));_tmp24A->hd=work_frame;_tmp24A->tl=work;_tmp24A;});return Cyc_Tcpat_and_match(
r,ctx2,work2,right_hand_side,rules);}_LL137: if((int)_tmp249 != 1)goto _LL139;
_LL138: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL139: if((int)_tmp249 != 2)goto _LL134;_LL13A: {struct Cyc_List_List*ctx2=({struct
Cyc_List_List*_tmp254=_region_malloc(r,sizeof(*_tmp254));_tmp254->hd=({struct
_tuple12*_tmp255=_region_malloc(r,sizeof(*_tmp255));_tmp255->f1=_tmp247;_tmp255->f2=
0;_tmp255;});_tmp254->tl=ctx;_tmp254;});struct _tuple11*work_frame=({struct
_tuple11*_tmp253=_region_malloc(r,sizeof(*_tmp253));_tmp253->f1=_tmp248;_tmp253->f2=
Cyc_Tcpat_getoargs(r,_tmp247,obj);_tmp253->f3=Cyc_Tcpat_getdargs(r,_tmp247,dsc);
_tmp253;});struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp252=
_region_malloc(r,sizeof(*_tmp252));_tmp252->hd=work_frame;_tmp252->tl=work;
_tmp252;});void*_tmp24E=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp24F=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,
dsc,_tmp247),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp250=
_region_malloc(r,sizeof(*_tmp250));_tmp250[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp251;_tmp251.tag=2;_tmp251.f1=obj;_tmp251.f2=_tmp247;_tmp251.f3=(void*)
_tmp24E;_tmp251.f4=(void*)_tmp24F;_tmp251;});_tmp250;});}_LL134:;}_LL12F:;}
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,
void*),void*env2,int*exhaust_done){void*_tmp256=d;void*_tmp257;void*_tmp258;void*
_tmp259;void*_tmp25A;_LL13C: if(*((int*)_tmp256)!= 0)goto _LL13E;_tmp257=(void*)((
struct Cyc_Tcpat_Failure_struct*)_tmp256)->f1;_LL13D: if(!(*exhaust_done)){
not_exhaust(r,env1,_tmp257);*exhaust_done=1;}goto _LL13B;_LL13E: if(*((int*)
_tmp256)!= 1)goto _LL140;_tmp258=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp256)->f1;
_LL13F: rhs_appears(env2,_tmp258);goto _LL13B;_LL140: if(*((int*)_tmp256)!= 2)goto
_LL13B;_tmp259=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp256)->f3;_tmp25A=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp256)->f4;_LL141: Cyc_Tcpat_check_exhaust_overlap(
r,_tmp259,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
r,_tmp25A,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13B;_LL13B:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp25C;struct _RegionHandle*_tmp25D;int*_tmp25E;struct
_tuple16*_tmp25B=env;_tmp25C=*_tmp25B;_tmp25D=_tmp25C.f1;_tmp25E=_tmp25C.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp25D,swc->pattern);struct _tuple14*rhs=({struct
_tuple14*_tmp264=_region_malloc(_tmp25D,sizeof(*_tmp264));_tmp264->f1=0;_tmp264->f2=(
swc->pattern)->loc;_tmp264;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(
_tmp25D,({struct Cyc_List_List*_tmp25F=_region_malloc(_tmp25D,sizeof(*_tmp25F));
_tmp25F->hd=(void*)sp0;_tmp25F->tl=({struct Cyc_List_List*_tmp260=_region_malloc(
_tmp25D,sizeof(*_tmp260));_tmp260->hd=(void*)Cyc_Tcpat_int_pat(_tmp25D,*_tmp25E,
0);_tmp260->tl=0;_tmp260;});_tmp25F;}),0);*_tmp25E=*_tmp25E + 1;}else{sp=Cyc_Tcpat_tuple_pat(
_tmp25D,({struct Cyc_List_List*_tmp261=_region_malloc(_tmp25D,sizeof(*_tmp261));
_tmp261->hd=(void*)sp0;_tmp261->tl=({struct Cyc_List_List*_tmp262=_region_malloc(
_tmp25D,sizeof(*_tmp262));_tmp262->hd=(void*)((void*)0);_tmp262->tl=0;_tmp262;});
_tmp261;}),0);}return({struct _tuple15*_tmp263=_region_malloc(_tmp25D,sizeof(*
_tmp263));_tmp263->f1=sp;_tmp263->f2=rhs;_tmp263;});}}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _dynforward_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dynforward_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp265=d;struct Cyc_Tcpat_Con_s*_tmp266;
struct Cyc_List_List*_tmp267;struct Cyc_Set_Set*_tmp268;_LL143: if(*((int*)_tmp265)
!= 0)goto _LL145;_tmp266=((struct Cyc_Tcpat_Pos_struct*)_tmp265)->f1;_tmp267=((
struct Cyc_Tcpat_Pos_struct*)_tmp265)->f2;_LL144: {union Cyc_Tcpat_Name_value_union
_tmp269=_tmp266->name;struct Cyc_Absyn_Pat*_tmp26A=_tmp266->orig_pat;if(_tmp26A == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp267))->hd);{void*_tmp26B=(void*)_tmp26A->r;struct Cyc_Absyn_Vardecl*_tmp26C;
struct Cyc_Absyn_Vardecl*_tmp26D;struct Cyc_Absyn_Tvar*_tmp26E;struct Cyc_Absyn_Vardecl*
_tmp26F;struct Cyc_Absyn_Pat*_tmp270;struct Cyc_Absyn_Pat _tmp271;void*_tmp272;
struct Cyc_Absyn_Tunionfield*_tmp273;struct Cyc_Absyn_AggrInfo _tmp274;union Cyc_Absyn_AggrInfoU_union
_tmp275;struct Cyc_Absyn_Aggrdecl**_tmp276;struct Cyc_Absyn_Aggrdecl*_tmp277;
struct Cyc_Absyn_Tunionfield*_tmp278;int _tmp279;char _tmp27A;struct _dynforward_ptr
_tmp27B;struct Cyc_Absyn_Enumfield*_tmp27C;struct Cyc_Absyn_Enumfield*_tmp27D;
struct Cyc_Absyn_Exp*_tmp27E;_LL148: if((int)_tmp26B != 0)goto _LL14A;_LL149: return({
const char*_tmp27F="_";_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size(
_tmp27F,2));});_LL14A: if(_tmp26B <= (void*)2)goto _LL15A;if(*((int*)_tmp26B)!= 0)
goto _LL14C;_tmp26C=((struct Cyc_Absyn_Var_p_struct*)_tmp26B)->f1;_LL14B: return Cyc_Absynpp_qvar2string(
_tmp26C->name);_LL14C: if(*((int*)_tmp26B)!= 1)goto _LL14E;_tmp26D=((struct Cyc_Absyn_Reference_p_struct*)
_tmp26B)->f1;_LL14D: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp282;_tmp282.tag=0;_tmp282.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp26D->name));{void*_tmp280[1]={& _tmp282};Cyc_aprintf(({
const char*_tmp281="*%s";_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(
_tmp281,4));}),_tag_dynforward(_tmp280,sizeof(void*),1));}});_LL14E: if(*((int*)
_tmp26B)!= 2)goto _LL150;_tmp26E=((struct Cyc_Absyn_TagInt_p_struct*)_tmp26B)->f1;
_tmp26F=((struct Cyc_Absyn_TagInt_p_struct*)_tmp26B)->f2;_LL14F: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp286;_tmp286.tag=0;_tmp286.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp26E->name);{struct Cyc_String_pa_struct
_tmp285;_tmp285.tag=0;_tmp285.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp26F->name));{void*_tmp283[2]={& _tmp285,& _tmp286};Cyc_aprintf(({
const char*_tmp284="%s<`%s>";_tag_dynforward(_tmp284,sizeof(char),
_get_zero_arr_size(_tmp284,8));}),_tag_dynforward(_tmp283,sizeof(void*),2));}}});
_LL150: if(*((int*)_tmp26B)!= 3)goto _LL152;_LL151: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp289;_tmp289.tag=0;_tmp289.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(r,_tmp267));{void*
_tmp287[1]={& _tmp289};Cyc_aprintf(({const char*_tmp288="$(%s)";_tag_dynforward(
_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,6));}),_tag_dynforward(_tmp287,
sizeof(void*),1));}});_LL152: if(*((int*)_tmp26B)!= 4)goto _LL154;_tmp270=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp26B)->f1;_tmp271=*_tmp270;_tmp272=(void*)_tmp271.r;
if(_tmp272 <= (void*)2)goto _LL154;if(*((int*)_tmp272)!= 6)goto _LL154;_tmp273=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp272)->f2;_LL153: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp28D;_tmp28D.tag=0;_tmp28D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(r,_tmp267));{
struct Cyc_String_pa_struct _tmp28C;_tmp28C.tag=0;_tmp28C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp273->name));{
void*_tmp28A[2]={& _tmp28C,& _tmp28D};Cyc_aprintf(({const char*_tmp28B="&%s(%s)";
_tag_dynforward(_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,8));}),
_tag_dynforward(_tmp28A,sizeof(void*),2));}}});_LL154: if(*((int*)_tmp26B)!= 4)
goto _LL156;_LL155: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp290;_tmp290.tag=0;_tmp290.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcpat_descs2string(r,_tmp267));{void*_tmp28E[1]={& _tmp290};Cyc_aprintf(({
const char*_tmp28F="&%s";_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(
_tmp28F,4));}),_tag_dynforward(_tmp28E,sizeof(void*),1));}});_LL156: if(*((int*)
_tmp26B)!= 5)goto _LL158;_tmp274=((struct Cyc_Absyn_Aggr_p_struct*)_tmp26B)->f1;
_tmp275=_tmp274.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp26B)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL158;_tmp276=(_tmp275.KnownAggr).f1;_tmp277=*_tmp276;_LL157: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp294;_tmp294.tag=0;_tmp294.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(r,_tmp267));{
struct Cyc_String_pa_struct _tmp293;_tmp293.tag=0;_tmp293.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp277->name));{
void*_tmp291[2]={& _tmp293,& _tmp294};Cyc_aprintf(({const char*_tmp292="%s{%s}";
_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,7));}),
_tag_dynforward(_tmp291,sizeof(void*),2));}}});_LL158: if(*((int*)_tmp26B)!= 6)
goto _LL15A;_tmp278=((struct Cyc_Absyn_Tunion_p_struct*)_tmp26B)->f2;_LL159: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp297;_tmp297.tag=0;_tmp297.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp278->name));{
void*_tmp295[1]={& _tmp297};Cyc_aprintf(({const char*_tmp296="%s";_tag_dynforward(
_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,3));}),_tag_dynforward(_tmp295,
sizeof(void*),1));}});_LL15A: if((int)_tmp26B != 1)goto _LL15C;_LL15B: return({const
char*_tmp298="NULL";_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(
_tmp298,5));});_LL15C: if(_tmp26B <= (void*)2)goto _LL168;if(*((int*)_tmp26B)!= 7)
goto _LL15E;_tmp279=((struct Cyc_Absyn_Int_p_struct*)_tmp26B)->f2;_LL15D: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp29B;_tmp29B.tag=1;_tmp29B.f1=(
unsigned long)_tmp279;{void*_tmp299[1]={& _tmp29B};Cyc_aprintf(({const char*
_tmp29A="%d";_tag_dynforward(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,3));}),
_tag_dynforward(_tmp299,sizeof(void*),1));}});_LL15E: if(*((int*)_tmp26B)!= 8)
goto _LL160;_tmp27A=((struct Cyc_Absyn_Char_p_struct*)_tmp26B)->f1;_LL15F: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp29E;_tmp29E.tag=1;_tmp29E.f1=(
unsigned long)((int)_tmp27A);{void*_tmp29C[1]={& _tmp29E};Cyc_aprintf(({const char*
_tmp29D="%d";_tag_dynforward(_tmp29D,sizeof(char),_get_zero_arr_size(_tmp29D,3));}),
_tag_dynforward(_tmp29C,sizeof(void*),1));}});_LL160: if(*((int*)_tmp26B)!= 9)
goto _LL162;_tmp27B=((struct Cyc_Absyn_Float_p_struct*)_tmp26B)->f1;_LL161: return
_tmp27B;_LL162: if(*((int*)_tmp26B)!= 10)goto _LL164;_tmp27C=((struct Cyc_Absyn_Enum_p_struct*)
_tmp26B)->f2;_LL163: _tmp27D=_tmp27C;goto _LL165;_LL164: if(*((int*)_tmp26B)!= 11)
goto _LL166;_tmp27D=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp26B)->f2;_LL165:
return Cyc_Absynpp_qvar2string(_tmp27D->name);_LL166: if(*((int*)_tmp26B)!= 14)
goto _LL168;_tmp27E=((struct Cyc_Absyn_Exp_p_struct*)_tmp26B)->f1;_LL167: return Cyc_Absynpp_exp2string(
_tmp27E);_LL168:;_LL169:(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL147:;}}
_LL145: if(*((int*)_tmp265)!= 1)goto _LL142;_tmp268=((struct Cyc_Tcpat_Neg_struct*)
_tmp265)->f1;_LL146: if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp268))
return({const char*_tmp29F="_";_tag_dynforward(_tmp29F,sizeof(char),
_get_zero_arr_size(_tmp29F,2));});{struct Cyc_Tcpat_Con_s*_tmp2A0=((struct Cyc_Tcpat_Con_s*(*)(
struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp268);struct Cyc_Absyn_Pat*_tmp2A1=
_tmp2A0->orig_pat;if(_tmp2A1 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{void*
_tmp2A2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2A1->topt))->v);
void*_tmp2A3;struct Cyc_Absyn_PtrInfo _tmp2A4;struct Cyc_Absyn_PtrAtts _tmp2A5;
struct Cyc_Absyn_TunionInfo _tmp2A6;union Cyc_Absyn_TunionInfoU_union _tmp2A7;struct
Cyc_Absyn_Tuniondecl**_tmp2A8;struct Cyc_Absyn_Tuniondecl*_tmp2A9;_LL16B: if(
_tmp2A2 <= (void*)4)goto _LL173;if(*((int*)_tmp2A2)!= 5)goto _LL16D;_tmp2A3=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2A2)->f2;if((int)_tmp2A3 != 0)goto _LL16D;
_LL16C:{int i=0;for(0;i < 256;i ++){struct Cyc_Tcpat_Con_s*_tmp2AA=Cyc_Tcpat_char_con(
r,(char)i,(struct Cyc_Absyn_Pat*)_tmp2A1);if(!((int(*)(struct Cyc_Set_Set*s,struct
Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp268,_tmp2AA))return(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2AD;_tmp2AD.tag=1;_tmp2AD.f1=(
unsigned long)i;{void*_tmp2AB[1]={& _tmp2AD};Cyc_aprintf(({const char*_tmp2AC="%d";
_tag_dynforward(_tmp2AC,sizeof(char),_get_zero_arr_size(_tmp2AC,3));}),
_tag_dynforward(_tmp2AB,sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16D: if(*((int*)_tmp2A2)!= 5)goto _LL16F;_LL16E:{unsigned int i=0;for(0;i < 0 - 1;
i ++){struct Cyc_Tcpat_Con_s*_tmp2AE=Cyc_Tcpat_int_con(r,(int)i,_tmp2A1);if(!((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp268,_tmp2AE))
return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2B1;_tmp2B1.tag=1;
_tmp2B1.f1=(unsigned long)((int)i);{void*_tmp2AF[1]={& _tmp2B1};Cyc_aprintf(({
const char*_tmp2B0="%d";_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(
_tmp2B0,3));}),_tag_dynforward(_tmp2AF,sizeof(void*),1));}});}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL16F: if(*((int*)_tmp2A2)!= 4)goto _LL171;_tmp2A4=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A2)->f1;_tmp2A5=_tmp2A4.ptr_atts;_LL170: {
struct Cyc_Absyn_Conref*_tmp2B2=_tmp2A5.nullable;int is_nullable=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2B2);if(is_nullable){if(!((
int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp268,
Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp2A1)))return({const char*_tmp2B3="NULL";
_tag_dynforward(_tmp2B3,sizeof(char),_get_zero_arr_size(_tmp2B3,5));});}return({
const char*_tmp2B4="&_";_tag_dynforward(_tmp2B4,sizeof(char),_get_zero_arr_size(
_tmp2B4,3));});}_LL171: if(*((int*)_tmp2A2)!= 2)goto _LL173;_tmp2A6=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A2)->f1;_tmp2A7=_tmp2A6.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp2A2)->f1).tunion_info).KnownTunion).tag != 1)goto _LL173;_tmp2A8=(_tmp2A7.KnownTunion).f1;
_tmp2A9=*_tmp2A8;_LL172: if(_tmp2A9->is_xtunion)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2B5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2A9->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2B5);for(0;(unsigned int)_tmp2B5;_tmp2B5=_tmp2B5->tl){struct _dynforward_ptr n=*(*((
struct Cyc_Absyn_Tunionfield*)_tmp2B5->hd)->name).f2;struct Cyc_List_List*_tmp2B6=((
struct Cyc_Absyn_Tunionfield*)_tmp2B5->hd)->typs;if(!((int(*)(struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp268,(struct Cyc_Tcpat_Con_s*)({
struct Cyc_Tcpat_Con_s*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->name=(union
Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp2B8;(_tmp2B8.Name_v).tag=
0;(_tmp2B8.Name_v).f1=n;_tmp2B8;});_tmp2B7->arity=0;_tmp2B7->span=0;_tmp2B7->orig_pat=
_tmp2A1;_tmp2B7;}))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2B6)
== 0)return n;else{if(_tmp2A9->is_flat)return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2BB;_tmp2BB.tag=0;_tmp2BB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
n);{void*_tmp2B9[1]={& _tmp2BB};Cyc_aprintf(({const char*_tmp2BA="%s(...)";
_tag_dynforward(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,8));}),
_tag_dynforward(_tmp2B9,sizeof(void*),1));}});else{return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp2BE;_tmp2BE.tag=0;_tmp2BE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)n);{void*_tmp2BC[1]={& _tmp2BE};Cyc_aprintf(({
const char*_tmp2BD="&%s(...)";_tag_dynforward(_tmp2BD,sizeof(char),
_get_zero_arr_size(_tmp2BD,9));}),_tag_dynforward(_tmp2BC,sizeof(void*),1));}});}}}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);}_LL173:;_LL174:(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16A:;}}_LL142:;}static struct _dynforward_ptr*Cyc_Tcpat_desc2stringptr(struct
_RegionHandle*r,void*d){return({struct _dynforward_ptr*_tmp2BF=_cycalloc(sizeof(*
_tmp2BF));_tmp2BF[0]=Cyc_Tcpat_desc2string(r,d);_tmp2BF;});}static struct
_dynforward_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp2C0=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);struct
_dynforward_ptr*comma=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),",",sizeof(char),2);{struct Cyc_List_List*_tmp2C1=_tmp2C0;for(0;
_tmp2C1 != 0;_tmp2C1=((struct Cyc_List_List*)_check_null(_tmp2C1))->tl){if(_tmp2C1->tl
!= 0){_tmp2C1->tl=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp2C2=_cycalloc(
sizeof(*_tmp2C2));_tmp2C2->hd=comma;_tmp2C2->tl=_tmp2C1->tl;_tmp2C2;});_tmp2C1=
_tmp2C1->tl;}}}return(struct _dynforward_ptr)Cyc_strconcat_l(_tmp2C0);}static void
Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
void*desc){struct _handler_cons _tmp2C4;_push_handler(& _tmp2C4);{int _tmp2C6=0;if(
setjmp(_tmp2C4.handler))_tmp2C6=1;if(!_tmp2C6){{struct _dynforward_ptr _tmp2C7=Cyc_Tcpat_desc2string(
r,desc);({struct Cyc_String_pa_struct _tmp2CA;_tmp2CA.tag=0;_tmp2CA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2C7);{void*_tmp2C8[1]={& _tmp2CA};Cyc_Tcutil_terr(
loc,({const char*_tmp2C9="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2C9,sizeof(char),_get_zero_arr_size(_tmp2C9,53));}),
_tag_dynforward(_tmp2C8,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2C5=(void*)_exn_thrown;void*_tmp2CC=_tmp2C5;_LL176: if(_tmp2CC != Cyc_Tcpat_Desc2string)
goto _LL178;_LL177:({void*_tmp2CD[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2CE="patterns may not be exhaustive.";
_tag_dynforward(_tmp2CE,sizeof(char),_get_zero_arr_size(_tmp2CE,32));}),
_tag_dynforward(_tmp2CD,sizeof(void*),0));});goto _LL175;_LL178:;_LL179:(void)
_throw(_tmp2CC);_LL175:;}}}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct _RegionHandle _tmp2CF=_new_region("r");struct
_RegionHandle*r=& _tmp2CF;_push_region(r);{int _tmp2D0=0;struct _tuple16 _tmp2D1=({
struct _tuple16 _tmp2DD;_tmp2DD.f1=r;_tmp2DD.f2=& _tmp2D0;_tmp2DD;});struct Cyc_List_List*
_tmp2D2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(
struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct _tuple16*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp2D1,swcs);void*_tmp2D3=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2D2);int _tmp2D4=0;((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(
struct _RegionHandle*,struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple14*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
r,_tmp2D3,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp2D4);for(0;
_tmp2D2 != 0;_tmp2D2=_tmp2D2->tl){struct _tuple15 _tmp2D6;struct _tuple14*_tmp2D7;
struct _tuple14 _tmp2D8;int _tmp2D9;struct Cyc_Position_Segment*_tmp2DA;struct
_tuple15*_tmp2D5=(struct _tuple15*)_tmp2D2->hd;_tmp2D6=*_tmp2D5;_tmp2D7=_tmp2D6.f2;
_tmp2D8=*_tmp2D7;_tmp2D9=_tmp2D8.f1;_tmp2DA=_tmp2D8.f2;if(!_tmp2D9)({void*
_tmp2DB[0]={};Cyc_Tcutil_terr(_tmp2DA,({const char*_tmp2DC="redundant pattern";
_tag_dynforward(_tmp2DC,sizeof(char),_get_zero_arr_size(_tmp2DC,18));}),
_tag_dynforward(_tmp2DB,sizeof(void*),0));});}};_pop_region(r);}struct _tuple17{
struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _RegionHandle*r,struct _tuple17*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp2DE;_push_handler(& _tmp2DE);{int _tmp2E0=0;if(setjmp(_tmp2DE.handler))
_tmp2E0=1;if(!_tmp2E0){{struct _dynforward_ptr _tmp2E1=Cyc_Tcpat_desc2string(r,
desc);({struct Cyc_String_pa_struct _tmp2E4;_tmp2E4.tag=0;_tmp2E4.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2E1);{void*_tmp2E2[1]={& _tmp2E4};Cyc_Tcutil_warn((*
pr).f1,({const char*_tmp2E3="pattern not exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2E3,sizeof(char),_get_zero_arr_size(_tmp2E3,45));}),
_tag_dynforward(_tmp2E2,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2DF=(void*)_exn_thrown;void*_tmp2E6=_tmp2DF;_LL17B: if(_tmp2E6 != Cyc_Tcpat_Desc2string)
goto _LL17D;_LL17C:({void*_tmp2E7[0]={};Cyc_Tcutil_warn((*pr).f1,({const char*
_tmp2E8="pattern not exhaustive.";_tag_dynforward(_tmp2E8,sizeof(char),
_get_zero_arr_size(_tmp2E8,24));}),_tag_dynforward(_tmp2E7,sizeof(void*),0));});
goto _LL17A;_LL17D:;_LL17E:(void)_throw(_tmp2E6);_LL17A:;}}}}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple18{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp2E9=
_new_region("r");struct _RegionHandle*r=& _tmp2E9;_push_region(r);{struct Cyc_List_List*
_tmp2EA=({struct Cyc_List_List*_tmp2F0=_region_malloc(r,sizeof(*_tmp2F0));_tmp2F0->hd=({
struct _tuple18*_tmp2F1=_region_malloc(r,sizeof(*_tmp2F1));_tmp2F1->f1=Cyc_Tcpat_compile_pat(
r,p);_tmp2F1->f2=0;_tmp2F1;});_tmp2F0->tl=0;_tmp2F0;});void*_tmp2EB=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2EA);struct _tuple17 _tmp2EC=({struct _tuple17 _tmp2EF;_tmp2EF.f1=loc;_tmp2EF.f2=
1;_tmp2EF;});int _tmp2ED=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct _tuple17*,void*),struct _tuple17*env1,
void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
r,_tmp2EB,Cyc_Tcpat_not_exhaust_warn,& _tmp2EC,Cyc_Tcpat_dummy_fn,0,& _tmp2ED);{
int _tmp2EE=_tmp2EC.f2;_npop_handler(0);return _tmp2EE;}};_pop_region(r);}static
struct _tuple15*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);struct _tuple14*rhs=({struct
_tuple14*_tmp2F3=_region_malloc(r,sizeof(*_tmp2F3));_tmp2F3->f1=0;_tmp2F3->f2=(
swc->pattern)->loc;_tmp2F3;});return({struct _tuple15*_tmp2F2=_region_malloc(r,
sizeof(*_tmp2F2));_tmp2F2->f1=sp0;_tmp2F2->f2=rhs;_tmp2F2;});}static void Cyc_Tcpat_not_exhaust_err2(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*swcs){struct _RegionHandle
_tmp2F4=_new_region("r");struct _RegionHandle*r=& _tmp2F4;_push_region(r);{struct
Cyc_List_List*_tmp2F5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple15*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,
r,swcs);void*_tmp2F6=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(r,_tmp2F5);int _tmp2F7=0;((void(*)(struct
_RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp2F6,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2F7);for(0;_tmp2F5 != 0;_tmp2F5=_tmp2F5->tl){
struct _tuple15 _tmp2F9;struct _tuple14*_tmp2FA;struct _tuple14 _tmp2FB;int _tmp2FC;
struct Cyc_Position_Segment*_tmp2FD;struct _tuple15*_tmp2F8=(struct _tuple15*)
_tmp2F5->hd;_tmp2F9=*_tmp2F8;_tmp2FA=_tmp2F9.f2;_tmp2FB=*_tmp2FA;_tmp2FC=_tmp2FB.f1;
_tmp2FD=_tmp2FB.f2;if(!_tmp2FC)({void*_tmp2FE[0]={};Cyc_Tcutil_terr(_tmp2FD,({
const char*_tmp2FF="redundant pattern";_tag_dynforward(_tmp2FF,sizeof(char),
_get_zero_arr_size(_tmp2FF,18));}),_tag_dynforward(_tmp2FE,sizeof(void*),0));});}};
_pop_region(r);}
