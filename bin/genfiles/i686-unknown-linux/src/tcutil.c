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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(
struct _tuple0*);int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};extern char Cyc_Core_Open_Region[16];extern char
Cyc_Core_Free_Region[16];typedef struct{int __count;union{unsigned int __wch;char
__wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t
__state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern
struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr
Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _dynforward_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,struct _dynforward_ptr);
extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_num_errors;extern int
Cyc_Position_max_errors;void Cyc_Position_post_error(struct Cyc_Position_Error*);
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple2{union Cyc_Absyn_Nmspace_union
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
_tuple2*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple2*tunion_name;struct
_tuple2*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple2*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple2*
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
_tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple2*f1;
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
struct Cyc_List_List*f1;};struct _tuple3{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple3*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple4{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple4 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple4 f2;struct _tuple4 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple4 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple2*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple2*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _tuple5{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple5*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple6{void*f1;struct _tuple2*f2;};struct _tuple6 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
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
le;};void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);unsigned int Cyc_strlen(struct
_dynforward_ptr s);int Cyc_strcmp(struct _dynforward_ptr s1,struct _dynforward_ptr s2);
int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct
_dynforward_ptr Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);void*
Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_flush_warnings();extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
void*Cyc_Tcutil_copy_type(void*t);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*
Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);
void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct
Cyc_Absyn_Tvar*);struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dynforward_ptr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*,int allow_unique);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void
Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr
err_msg);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,struct Cyc_List_List*);
void Cyc_Tcutil_check_no_unique_region(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*t);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct
Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,
unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;};struct _tuple10 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple5*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int
Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=
0;struct _dynforward_ptr Cyc_Tcutil_failure_reason=(struct _dynforward_ptr){(void*)
0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= 
Cyc_Position_max_errors)return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct _dynforward_ptr s1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure));{struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(
struct _dynforward_ptr)(Cyc_Tcutil_tq1_const?({const char*_tmp23="const ";
_tag_dynforward(_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,7));}):({const char*
_tmp24="";_tag_dynforward(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,1));}));{
void*_tmp1F[2]={& _tmp21,& _tmp22};Cyc_aprintf(({const char*_tmp20="%s%s";
_tag_dynforward(_tmp20,sizeof(char),_get_zero_arr_size(_tmp20,5));}),
_tag_dynforward(_tmp1F,sizeof(void*),2));}}});struct _dynforward_ptr s2=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure));{
struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dynforward_ptr)(
Cyc_Tcutil_tq2_const?({const char*_tmp1D="const ";_tag_dynforward(_tmp1D,sizeof(
char),_get_zero_arr_size(_tmp1D,7));}):({const char*_tmp1E="";_tag_dynforward(
_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,1));}));{void*_tmp19[2]={& _tmp1B,&
_tmp1C};Cyc_aprintf(({const char*_tmp1A="%s%s";_tag_dynforward(_tmp1A,sizeof(char),
_get_zero_arr_size(_tmp1A,5));}),_tag_dynforward(_tmp19,sizeof(void*),2));}}});
int pos=2;({struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)s1);{void*_tmp0[1]={& _tmp2};Cyc_fprintf(
Cyc_stderr,({const char*_tmp1="  %s";_tag_dynforward(_tmp1,sizeof(char),
_get_zero_arr_size(_tmp1,5));}),_tag_dynforward(_tmp0,sizeof(void*),1));}});pos +=
_get_dynforward_size(s1,sizeof(char));if(pos + 5 >= 80){({void*_tmp3[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp4="\n\t";_tag_dynforward(_tmp4,sizeof(char),
_get_zero_arr_size(_tmp4,3));}),_tag_dynforward(_tmp3,sizeof(void*),0));});pos=8;}
else{({void*_tmp5[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp6=" ";
_tag_dynforward(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,2));}),
_tag_dynforward(_tmp5,sizeof(void*),0));});pos ++;}({void*_tmp7[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp8="and ";_tag_dynforward(_tmp8,sizeof(char),
_get_zero_arr_size(_tmp8,5));}),_tag_dynforward(_tmp7,sizeof(void*),0));});pos +=
4;if(pos + _get_dynforward_size(s2,sizeof(char))>= 80){({void*_tmp9[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpA="\n\t";_tag_dynforward(_tmpA,sizeof(char),
_get_zero_arr_size(_tmpA,3));}),_tag_dynforward(_tmp9,sizeof(void*),0));});pos=8;}({
struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)s2);{void*_tmpB[1]={& _tmpD};Cyc_fprintf(Cyc_stderr,({const
char*_tmpC="%s ";_tag_dynforward(_tmpC,sizeof(char),_get_zero_arr_size(_tmpC,4));}),
_tag_dynforward(_tmpB,sizeof(void*),1));}});pos +=_get_dynforward_size(s2,sizeof(
char))+ 1;if(pos + 17 >= 80){({void*_tmpE[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmpF="\n\t";_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,3));}),
_tag_dynforward(_tmpE,sizeof(void*),0));});pos=8;}({void*_tmp10[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp11="are not compatible. ";_tag_dynforward(_tmp11,
sizeof(char),_get_zero_arr_size(_tmp11,21));}),_tag_dynforward(_tmp10,sizeof(
void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr){if(pos + Cyc_strlen((struct
_dynforward_ptr)Cyc_Tcutil_failure_reason)>= 80)({void*_tmp12[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp13="\n\t";_tag_dynforward(_tmp13,sizeof(char),
_get_zero_arr_size(_tmp13,3));}),_tag_dynforward(_tmp12,sizeof(void*),0));});({
struct Cyc_String_pa_struct _tmp16;_tmp16.tag=0;_tmp16.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Tcutil_failure_reason);{void*_tmp14[1]={& _tmp16};Cyc_fprintf(
Cyc_stderr,({const char*_tmp15="%s";_tag_dynforward(_tmp15,sizeof(char),
_get_zero_arr_size(_tmp15,3));}),_tag_dynforward(_tmp14,sizeof(void*),1));}});}({
void*_tmp17[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp18="\n";_tag_dynforward(
_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,2));}),_tag_dynforward(_tmp17,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*loc,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dynforward_ptr)Cyc_vrprintf(
Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap){struct _dynforward_ptr msg=(struct _dynforward_ptr)Cyc_vrprintf(
Cyc_Core_heap_region,fmt,ap);({struct Cyc_String_pa_struct _tmp27;_tmp27.tag=0;
_tmp27.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)msg);{void*_tmp25[1]={&
_tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="Compiler Error (Tcutil::impos): %s\n";
_tag_dynforward(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,36));}),
_tag_dynforward(_tmp25,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp28=
_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Core_Impossible_struct _tmp29;
_tmp29.tag=Cyc_Core_Impossible;_tmp29.f1=msg;_tmp29;});_tmp28;}));}static struct
_dynforward_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){return*tv->name;}
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){({
struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind));{struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd));{void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2B="%s::%s ";_tag_dynforward(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,8));}),
_tag_dynforward(_tmp2A,sizeof(void*),2));}}});}({void*_tmp2E[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2F="\n";_tag_dynforward(_tmp2F,sizeof(char),
_get_zero_arr_size(_tmp2F,2));}),_tag_dynforward(_tmp2E,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){struct
_dynforward_ptr msg=(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,
ap);Cyc_Tcutil_warning_segs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*
_tmp30));_tmp30->hd=sg;_tmp30->tl=Cyc_Tcutil_warning_segs;_tmp30;});Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=({struct
_dynforward_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=msg;_tmp32;});_tmp31->tl=
Cyc_Tcutil_warning_msgs;_tmp31;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs
== 0)return;({void*_tmp33[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp34="***Warnings***\n";
_tag_dynforward(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,16));}),
_tag_dynforward(_tmp33,sizeof(void*),0));});{struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=
0;_tmp39.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));{
struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct _dynforward_ptr*)((struct Cyc_List_List*)
_check_null(_tmp35))->hd));{void*_tmp36[2]={& _tmp38,& _tmp39};Cyc_fprintf(Cyc_stderr,({
const char*_tmp37="%s: %s\n";_tag_dynforward(_tmp37,sizeof(char),
_get_zero_arr_size(_tmp37,8));}),_tag_dynforward(_tmp36,sizeof(void*),2));}}});
_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}({
void*_tmp3A[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp3B="**************\n";
_tag_dynforward(_tmp3B,sizeof(char),_get_zero_arr_size(_tmp3B,16));}),
_tag_dynforward(_tmp3A,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;static int Cyc_Tcutil_fast_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){return*((int*)_check_null(tv1->identity))
- *((int*)_check_null(tv2->identity));}void*Cyc_Tcutil_compress(void*t){void*
_tmp3C=t;struct Cyc_Core_Opt*_tmp3D;void**_tmp3E;void**_tmp3F;void***_tmp40;
struct Cyc_Core_Opt*_tmp41;struct Cyc_Core_Opt**_tmp42;_LL1: if(_tmp3C <= (void*)4)
goto _LL9;if(*((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((struct Cyc_Absyn_Evar_struct*)
_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(*((int*)_tmp3C)!= 17)
goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;if(_tmp3E != 0)
goto _LL5;_LL4: return t;_LL5: if(*((int*)_tmp3C)!= 17)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;_tmp40=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;
_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp40)));if(t2 != *((
void**)_check_null(*_tmp40)))*_tmp40=({void**_tmp43=_cycalloc(sizeof(*_tmp43));
_tmp43[0]=t2;_tmp43;});return t2;}_LL7: if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp42))->v)*_tmp42=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44->v=(void*)t2;_tmp44;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){union Cyc_Absyn_Constraint_union
_tmp45=c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((_tmp45.No_constr).tag
!= 2)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if((_tmp45.Eq_constr).tag
!= 0)goto _LL10;_tmp46=(_tmp45.Eq_constr).f1;_LLF: return Cyc_Absyn_new_conref(
_tmp46);_LL10: if((_tmp45.Forward_constr).tag != 1)goto _LLB;_tmp47=(_tmp45.Forward_constr).f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp48=Cyc_Absyn_compress_kb(kb);void*_tmp49;void*_tmp4A;_LL13: if(*((
int*)_tmp48)!= 0)goto _LL15;_tmp49=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp48)->f1;
_LL14: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp4B=_cycalloc(sizeof(*
_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(
void*)_tmp49;_tmp4C;});_tmp4B;});_LL15: if(*((int*)_tmp48)!= 1)goto _LL17;_LL16:
return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));
_tmp4D[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp4E;_tmp4E.tag=1;_tmp4E.f1=0;
_tmp4E;});_tmp4D;});_LL17: if(*((int*)_tmp48)!= 2)goto _LL12;_tmp4A=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp48)->f2;_LL18: return(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp50;_tmp50.tag=2;_tmp50.f1=0;_tmp50.f2=(void*)_tmp4A;_tmp50;});_tmp4F;});
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->name=tv->name;
_tmp51->identity=0;_tmp51->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);
_tmp51;});}static struct _tuple3*Cyc_Tcutil_copy_arg(struct _tuple3*arg){struct
_tuple3 _tmp53;struct Cyc_Core_Opt*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;
struct _tuple3*_tmp52=arg;_tmp53=*_tmp52;_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;_tmp56=
_tmp53.f3;return({struct _tuple3*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=
_tmp54;_tmp57->f2=_tmp55;_tmp57->f3=Cyc_Tcutil_copy_type(_tmp56);_tmp57;});}
static struct _tuple5*Cyc_Tcutil_copy_tqt(struct _tuple5*arg){struct _tuple5 _tmp59;
struct Cyc_Absyn_Tqual _tmp5A;void*_tmp5B;struct _tuple5*_tmp58=arg;_tmp59=*_tmp58;
_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;return({struct _tuple5*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C->f1=_tmp5A;_tmp5C->f2=Cyc_Tcutil_copy_type(_tmp5B);_tmp5C;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->name=
f->name;_tmp5D->tq=f->tq;_tmp5D->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp5D->width=f->width;_tmp5D->attributes=f->attributes;_tmp5D;});}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp5F;void*_tmp60;
void*_tmp61;struct _tuple0*_tmp5E=x;_tmp5F=*_tmp5E;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;
return({struct _tuple0*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=Cyc_Tcutil_copy_type(
_tmp60);_tmp62->f2=Cyc_Tcutil_copy_type(_tmp61);_tmp62;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->name=f->name;_tmp63->tag=f->tag;_tmp63->loc=
f->loc;_tmp63;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp64=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp65;struct Cyc_Absyn_TunionInfo _tmp66;union Cyc_Absyn_TunionInfoU_union
_tmp67;struct Cyc_List_List*_tmp68;struct Cyc_Core_Opt*_tmp69;struct Cyc_Absyn_TunionFieldInfo
_tmp6A;union Cyc_Absyn_TunionFieldInfoU_union _tmp6B;struct Cyc_List_List*_tmp6C;
struct Cyc_Absyn_PtrInfo _tmp6D;void*_tmp6E;struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_PtrAtts
_tmp70;void*_tmp71;struct Cyc_Absyn_Conref*_tmp72;struct Cyc_Absyn_Conref*_tmp73;
struct Cyc_Absyn_Conref*_tmp74;struct Cyc_Absyn_PtrLoc*_tmp75;void*_tmp76;void*
_tmp77;int _tmp78;struct Cyc_Absyn_ArrayInfo _tmp79;void*_tmp7A;struct Cyc_Absyn_Tqual
_tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Conref*_tmp7D;struct Cyc_Position_Segment*
_tmp7E;struct Cyc_Absyn_FnInfo _tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_Core_Opt*
_tmp81;void*_tmp82;struct Cyc_List_List*_tmp83;int _tmp84;struct Cyc_Absyn_VarargInfo*
_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List*
_tmp88;struct Cyc_Absyn_AggrInfo _tmp89;union Cyc_Absyn_AggrInfoU_union _tmp8A;void*
_tmp8B;struct _tuple2*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_AggrInfo
_tmp8E;union Cyc_Absyn_AggrInfoU_union _tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;
struct Cyc_List_List*_tmp91;void*_tmp92;struct Cyc_List_List*_tmp93;struct _tuple2*
_tmp94;struct Cyc_Absyn_Enumdecl*_tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;
int _tmp98;void*_tmp99;void*_tmp9A;void*_tmp9B;void*_tmp9C;struct _tuple2*_tmp9D;
struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Typedefdecl*_tmp9F;void*_tmpA0;struct
Cyc_List_List*_tmpA1;void*_tmpA2;_LL1A: if((int)_tmp64 != 0)goto _LL1C;_LL1B: goto
_LL1D;_LL1C: if(_tmp64 <= (void*)4)goto _LL28;if(*((int*)_tmp64)!= 0)goto _LL1E;
_LL1D: return t;_LL1E: if(*((int*)_tmp64)!= 1)goto _LL20;_tmp65=((struct Cyc_Absyn_VarType_struct*)
_tmp64)->f1;_LL1F: return(void*)({struct Cyc_Absyn_VarType_struct*_tmpA3=_cycalloc(
sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_VarType_struct _tmpA4;_tmpA4.tag=1;
_tmpA4.f1=Cyc_Tcutil_copy_tvar(_tmp65);_tmpA4;});_tmpA3;});_LL20: if(*((int*)
_tmp64)!= 2)goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)_tmp64)->f1;
_tmp67=_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=_tmp66.rgn;_LL21: {struct
Cyc_Core_Opt*_tmpA5=(unsigned int)_tmp69?({struct Cyc_Core_Opt*_tmpA9=_cycalloc(
sizeof(*_tmpA9));_tmpA9->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp69->v);_tmpA9;}):
0;return(void*)({struct Cyc_Absyn_TunionType_struct*_tmpA6=_cycalloc(sizeof(*
_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_TunionType_struct _tmpA7;_tmpA7.tag=2;_tmpA7.f1=({
struct Cyc_Absyn_TunionInfo _tmpA8;_tmpA8.tunion_info=_tmp67;_tmpA8.targs=Cyc_Tcutil_copy_types(
_tmp68);_tmpA8.rgn=_tmpA5;_tmpA8;});_tmpA7;});_tmpA6;});}_LL22: if(*((int*)_tmp64)
!= 3)goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;
_tmp6B=_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpAB;_tmpAB.tag=3;_tmpAB.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpAC;_tmpAC.field_info=
_tmp6B;_tmpAC.targs=Cyc_Tcutil_copy_types(_tmp6C);_tmpAC;});_tmpAB;});_tmpAA;});
_LL24: if(*((int*)_tmp64)!= 4)goto _LL26;_tmp6D=((struct Cyc_Absyn_PointerType_struct*)
_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;
_tmp71=(void*)_tmp70.rgn;_tmp72=_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=
_tmp70.zero_term;_tmp75=_tmp70.ptrloc;_LL25: {void*_tmpAD=Cyc_Tcutil_copy_type(
_tmp6E);void*_tmpAE=Cyc_Tcutil_copy_type(_tmp71);struct Cyc_Absyn_Conref*_tmpAF=((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp72);struct Cyc_Absyn_Tqual _tmpB0=_tmp6F;struct Cyc_Absyn_Conref*_tmpB1=Cyc_Tcutil_copy_conref(
_tmp73);struct Cyc_Absyn_Conref*_tmpB2=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB4;_tmpB4.tag=4;_tmpB4.f1=({struct Cyc_Absyn_PtrInfo _tmpB5;_tmpB5.elt_typ=(
void*)_tmpAD;_tmpB5.elt_tq=_tmpB0;_tmpB5.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpB6;_tmpB6.rgn=(void*)_tmpAE;_tmpB6.nullable=_tmpAF;_tmpB6.bounds=_tmpB1;
_tmpB6.zero_term=_tmpB2;_tmpB6.ptrloc=_tmp75;_tmpB6;});_tmpB5;});_tmpB4;});
_tmpB3;});}_LL26: if(*((int*)_tmp64)!= 5)goto _LL28;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64)->f1;_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f2;_LL27:
return(void*)({struct Cyc_Absyn_IntType_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));
_tmpB7[0]=({struct Cyc_Absyn_IntType_struct _tmpB8;_tmpB8.tag=5;_tmpB8.f1=(void*)
_tmp76;_tmpB8.f2=(void*)_tmp77;_tmpB8;});_tmpB7;});_LL28: if((int)_tmp64 != 1)goto
_LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (void*)4)goto _LL48;if(*((int*)_tmp64)!= 6)
goto _LL2C;_tmp78=((struct Cyc_Absyn_DoubleType_struct*)_tmp64)->f1;_LL2B: return(
void*)({struct Cyc_Absyn_DoubleType_struct*_tmpB9=_cycalloc_atomic(sizeof(*_tmpB9));
_tmpB9[0]=({struct Cyc_Absyn_DoubleType_struct _tmpBA;_tmpBA.tag=6;_tmpBA.f1=
_tmp78;_tmpBA;});_tmpB9;});_LL2C: if(*((int*)_tmp64)!= 7)goto _LL2E;_tmp79=((
struct Cyc_Absyn_ArrayType_struct*)_tmp64)->f1;_tmp7A=(void*)_tmp79.elt_type;
_tmp7B=_tmp79.tq;_tmp7C=_tmp79.num_elts;_tmp7D=_tmp79.zero_term;_tmp7E=_tmp79.zt_loc;
_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpBB=_cycalloc(sizeof(*
_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_ArrayType_struct _tmpBC;_tmpBC.tag=7;_tmpBC.f1=({
struct Cyc_Absyn_ArrayInfo _tmpBD;_tmpBD.elt_type=(void*)Cyc_Tcutil_copy_type(
_tmp7A);_tmpBD.tq=_tmp7B;_tmpBD.num_elts=_tmp7C;_tmpBD.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp7D);_tmpBD.zt_loc=_tmp7E;
_tmpBD;});_tmpBC;});_tmpBB;});_LL2E: if(*((int*)_tmp64)!= 8)goto _LL30;_tmp7F=((
struct Cyc_Absyn_FnType_struct*)_tmp64)->f1;_tmp80=_tmp7F.tvars;_tmp81=_tmp7F.effect;
_tmp82=(void*)_tmp7F.ret_typ;_tmp83=_tmp7F.args;_tmp84=_tmp7F.c_varargs;_tmp85=
_tmp7F.cyc_varargs;_tmp86=_tmp7F.rgn_po;_tmp87=_tmp7F.attributes;_LL2F: {struct
Cyc_List_List*_tmpBE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp80);struct Cyc_Core_Opt*_tmpBF=_tmp81 == 0?0:({struct Cyc_Core_Opt*_tmpC9=
_cycalloc(sizeof(*_tmpC9));_tmpC9->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp81->v);
_tmpC9;});void*_tmpC0=Cyc_Tcutil_copy_type(_tmp82);struct Cyc_List_List*_tmpC1=((
struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp83);int _tmpC2=_tmp84;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp85 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp85;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpC3=_cycalloc(sizeof(*_tmpC3));
_tmpC3->name=cv->name;_tmpC3->tq=cv->tq;_tmpC3->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpC3->inject=cv->inject;_tmpC3;});}{struct Cyc_List_List*_tmpC4=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp86);struct Cyc_List_List*_tmpC5=_tmp87;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));
_tmpC6[0]=({struct Cyc_Absyn_FnType_struct _tmpC7;_tmpC7.tag=8;_tmpC7.f1=({struct
Cyc_Absyn_FnInfo _tmpC8;_tmpC8.tvars=_tmpBE;_tmpC8.effect=_tmpBF;_tmpC8.ret_typ=(
void*)_tmpC0;_tmpC8.args=_tmpC1;_tmpC8.c_varargs=_tmpC2;_tmpC8.cyc_varargs=
cyc_varargs2;_tmpC8.rgn_po=_tmpC4;_tmpC8.attributes=_tmpC5;_tmpC8;});_tmpC7;});
_tmpC6;});}}_LL30: if(*((int*)_tmp64)!= 9)goto _LL32;_tmp88=((struct Cyc_Absyn_TupleType_struct*)
_tmp64)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpCA=
_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_TupleType_struct _tmpCB;
_tmpCB.tag=9;_tmpCB.f1=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp88);
_tmpCB;});_tmpCA;});_LL32: if(*((int*)_tmp64)!= 10)goto _LL34;_tmp89=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8A=_tmp89.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp8B=(_tmp8A.UnknownAggr).f1;
_tmp8C=(_tmp8A.UnknownAggr).f2;_tmp8D=_tmp89.targs;_LL33: return(void*)({struct
Cyc_Absyn_AggrType_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({struct
Cyc_Absyn_AggrType_struct _tmpCD;_tmpCD.tag=10;_tmpCD.f1=({struct Cyc_Absyn_AggrInfo
_tmpCE;_tmpCE.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmpCF;(_tmpCF.UnknownAggr).tag=0;(_tmpCF.UnknownAggr).f1=(void*)_tmp8B;(_tmpCF.UnknownAggr).f2=
_tmp8C;_tmpCF;});_tmpCE.targs=Cyc_Tcutil_copy_types(_tmp8D);_tmpCE;});_tmpCD;});
_tmpCC;});_LL34: if(*((int*)_tmp64)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8F=_tmp8E.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1).aggr_info).KnownAggr).tag != 1)goto _LL36;_tmp90=(_tmp8F.KnownAggr).f1;
_tmp91=_tmp8E.targs;_LL35: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpD0=
_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_AggrType_struct _tmpD1;
_tmpD1.tag=10;_tmpD1.f1=({struct Cyc_Absyn_AggrInfo _tmpD2;_tmpD2.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpD3;(_tmpD3.KnownAggr).tag=
1;(_tmpD3.KnownAggr).f1=_tmp90;_tmpD3;});_tmpD2.targs=Cyc_Tcutil_copy_types(
_tmp91);_tmpD2;});_tmpD1;});_tmpD0;});_LL36: if(*((int*)_tmp64)!= 11)goto _LL38;
_tmp92=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f1;_tmp93=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmpD5;_tmpD5.tag=11;_tmpD5.f1=(void*)_tmp92;_tmpD5.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp93);_tmpD5;});_tmpD4;});_LL38: if(*((int*)
_tmp64)!= 12)goto _LL3A;_tmp94=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;
_tmp95=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({
struct Cyc_Absyn_EnumType_struct _tmpD7;_tmpD7.tag=12;_tmpD7.f1=_tmp94;_tmpD7.f2=
_tmp95;_tmpD7;});_tmpD6;});_LL3A: if(*((int*)_tmp64)!= 13)goto _LL3C;_tmp96=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmpD9;_tmpD9.tag=13;_tmpD9.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*
f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp96);_tmpD9;});_tmpD8;});_LL3C: if(*((int*)_tmp64)!= 14)goto _LL3E;_tmp97=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D: return(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_SizeofType_struct
_tmpDB;_tmpDB.tag=14;_tmpDB.f1=(void*)Cyc_Tcutil_copy_type(_tmp97);_tmpDB;});
_tmpDA;});_LL3E: if(*((int*)_tmp64)!= 19)goto _LL40;_tmp98=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpDC=
_cycalloc_atomic(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpDD;_tmpDD.tag=19;_tmpDD.f1=_tmp98;_tmpDD;});_tmpDC;});_LL40: if(*((int*)
_tmp64)!= 18)goto _LL42;_tmp99=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp64)->f1;
_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDE=_cycalloc(sizeof(*
_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_TagType_struct _tmpDF;_tmpDF.tag=18;_tmpDF.f1=(
void*)Cyc_Tcutil_copy_type(_tmp99);_tmpDF;});_tmpDE;});_LL42: if(*((int*)_tmp64)
!= 15)goto _LL44;_tmp9A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp64)->f1;
_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpE0=_cycalloc(
sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpE1;_tmpE1.tag=
15;_tmpE1.f1=(void*)Cyc_Tcutil_copy_type(_tmp9A);_tmpE1;});_tmpE0;});_LL44: if(*((
int*)_tmp64)!= 16)goto _LL46;_tmp9B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp64)->f1;_tmp9C=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp64)->f2;_LL45:
return(void*)({struct Cyc_Absyn_DynRgnType_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));
_tmpE2[0]=({struct Cyc_Absyn_DynRgnType_struct _tmpE3;_tmpE3.tag=16;_tmpE3.f1=(
void*)Cyc_Tcutil_copy_type(_tmp9B);_tmpE3.f2=(void*)Cyc_Tcutil_copy_type(_tmp9C);
_tmpE3;});_tmpE2;});_LL46: if(*((int*)_tmp64)!= 17)goto _LL48;_tmp9D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f1;_tmp9E=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f2;_tmp9F=((
struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f3;_LL47: return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpE5;_tmpE5.tag=17;_tmpE5.f1=_tmp9D;_tmpE5.f2=Cyc_Tcutil_copy_types(_tmp9E);
_tmpE5.f3=_tmp9F;_tmpE5.f4=0;_tmpE5;});_tmpE4;});_LL48: if((int)_tmp64 != 3)goto
_LL4A;_LL49: goto _LL4B;_LL4A: if((int)_tmp64 != 2)goto _LL4C;_LL4B: return t;_LL4C: if(
_tmp64 <= (void*)4)goto _LL4E;if(*((int*)_tmp64)!= 20)goto _LL4E;_tmpA0=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_AccessEff_struct
_tmpE7;_tmpE7.tag=20;_tmpE7.f1=(void*)Cyc_Tcutil_copy_type(_tmpA0);_tmpE7;});
_tmpE6;});_LL4E: if(_tmp64 <= (void*)4)goto _LL50;if(*((int*)_tmp64)!= 21)goto _LL50;
_tmpA1=((struct Cyc_Absyn_JoinEff_struct*)_tmp64)->f1;_LL4F: return(void*)({struct
Cyc_Absyn_JoinEff_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpE9;_tmpE9.tag=21;_tmpE9.f1=Cyc_Tcutil_copy_types(_tmpA1);_tmpE9;});_tmpE8;});
_LL50: if(_tmp64 <= (void*)4)goto _LL19;if(*((int*)_tmp64)!= 22)goto _LL19;_tmpA2=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp64)->f1;_LL51: return(void*)({struct
Cyc_Absyn_RgnsEff_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmpEB;_tmpEB.tag=22;_tmpEB.f1=(void*)Cyc_Tcutil_copy_type(_tmpA2);_tmpEB;});
_tmpEA;});_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{
struct _tuple0 _tmpED=({struct _tuple0 _tmpEC;_tmpEC.f1=k1;_tmpEC.f2=k2;_tmpEC;});
void*_tmpEE;void*_tmpEF;void*_tmpF0;void*_tmpF1;void*_tmpF2;void*_tmpF3;_LL53:
_tmpEE=_tmpED.f1;if((int)_tmpEE != 2)goto _LL55;_tmpEF=_tmpED.f2;if((int)_tmpEF != 
1)goto _LL55;_LL54: goto _LL56;_LL55: _tmpF0=_tmpED.f1;if((int)_tmpF0 != 2)goto _LL57;
_tmpF1=_tmpED.f2;if((int)_tmpF1 != 0)goto _LL57;_LL56: goto _LL58;_LL57: _tmpF2=
_tmpED.f1;if((int)_tmpF2 != 1)goto _LL59;_tmpF3=_tmpED.f2;if((int)_tmpF3 != 0)goto
_LL59;_LL58: return 1;_LL59:;_LL5A: return 0;_LL52:;}}void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv){void*_tmpF4=Cyc_Absyn_compress_kb((void*)tv->kind);void*
_tmpF5;void*_tmpF6;_LL5C: if(*((int*)_tmpF4)!= 0)goto _LL5E;_tmpF5=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmpF4)->f1;_LL5D: return _tmpF5;_LL5E: if(*((int*)_tmpF4)
!= 2)goto _LL60;_tmpF6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpF4)->f2;_LL5F:
return _tmpF6;_LL60:;_LL61:({void*_tmpF7[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF8="kind not suitably constrained!";
_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,31));}),
_tag_dynforward(_tmpF7,sizeof(void*),0));});_LL5B:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpF9=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpFA;struct Cyc_Core_Opt*
_tmpFB;struct Cyc_Absyn_Tvar*_tmpFC;void*_tmpFD;struct Cyc_Absyn_TunionInfo _tmpFE;
union Cyc_Absyn_TunionInfoU_union _tmpFF;struct Cyc_Absyn_UnknownTunionInfo _tmp100;
struct Cyc_Absyn_TunionInfo _tmp101;union Cyc_Absyn_TunionInfoU_union _tmp102;struct
Cyc_Absyn_Tuniondecl**_tmp103;struct Cyc_Absyn_Tuniondecl*_tmp104;struct Cyc_Absyn_TunionFieldInfo
_tmp105;union Cyc_Absyn_TunionFieldInfoU_union _tmp106;struct Cyc_Absyn_Tuniondecl*
_tmp107;struct Cyc_Absyn_Tunionfield*_tmp108;struct Cyc_Absyn_TunionFieldInfo
_tmp109;union Cyc_Absyn_TunionFieldInfoU_union _tmp10A;struct Cyc_Absyn_Enumdecl*
_tmp10B;struct Cyc_Absyn_AggrInfo _tmp10C;union Cyc_Absyn_AggrInfoU_union _tmp10D;
struct Cyc_Absyn_AggrInfo _tmp10E;union Cyc_Absyn_AggrInfoU_union _tmp10F;struct Cyc_Absyn_Aggrdecl**
_tmp110;struct Cyc_Absyn_Aggrdecl*_tmp111;struct Cyc_Absyn_Aggrdecl _tmp112;struct
Cyc_Absyn_AggrdeclImpl*_tmp113;struct Cyc_Absyn_Enumdecl*_tmp114;struct Cyc_Absyn_PtrInfo
_tmp115;struct Cyc_Absyn_Typedefdecl*_tmp116;_LL63: if(_tmpF9 <= (void*)4)goto _LL67;
if(*((int*)_tmpF9)!= 0)goto _LL65;_tmpFA=((struct Cyc_Absyn_Evar_struct*)_tmpF9)->f1;
_tmpFB=((struct Cyc_Absyn_Evar_struct*)_tmpF9)->f2;_LL64: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpFA))->v;_LL65: if(*((int*)_tmpF9)!= 1)goto _LL67;_tmpFC=((struct
Cyc_Absyn_VarType_struct*)_tmpF9)->f1;_LL66: return Cyc_Tcutil_tvar_kind(_tmpFC);
_LL67: if((int)_tmpF9 != 0)goto _LL69;_LL68: return(void*)1;_LL69: if(_tmpF9 <= (void*)
4)goto _LL6B;if(*((int*)_tmpF9)!= 5)goto _LL6B;_tmpFD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpF9)->f2;_LL6A: return(_tmpFD == (void*)((void*)2) || _tmpFD == (void*)((void*)3))?(
void*)2:(void*)1;_LL6B: if((int)_tmpF9 != 1)goto _LL6D;_LL6C: goto _LL6E;_LL6D: if(
_tmpF9 <= (void*)4)goto _LL75;if(*((int*)_tmpF9)!= 6)goto _LL6F;_LL6E: goto _LL70;
_LL6F: if(*((int*)_tmpF9)!= 8)goto _LL71;_LL70: return(void*)1;_LL71: if(*((int*)
_tmpF9)!= 16)goto _LL73;_LL72: goto _LL74;_LL73: if(*((int*)_tmpF9)!= 15)goto _LL75;
_LL74: return(void*)2;_LL75: if((int)_tmpF9 != 3)goto _LL77;_LL76: goto _LL78;_LL77:
if((int)_tmpF9 != 2)goto _LL79;_LL78: return(void*)3;_LL79: if(_tmpF9 <= (void*)4)
goto _LL7B;if(*((int*)_tmpF9)!= 2)goto _LL7B;_tmpFE=((struct Cyc_Absyn_TunionType_struct*)
_tmpF9)->f1;_tmpFF=_tmpFE.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmpF9)->f1).tunion_info).UnknownTunion).tag != 0)goto _LL7B;_tmp100=(_tmpFF.UnknownTunion).f1;
_LL7A: if(_tmp100.is_flat)return(void*)1;else{return(void*)2;}_LL7B: if(_tmpF9 <= (
void*)4)goto _LL7D;if(*((int*)_tmpF9)!= 2)goto _LL7D;_tmp101=((struct Cyc_Absyn_TunionType_struct*)
_tmpF9)->f1;_tmp102=_tmp101.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmpF9)->f1).tunion_info).KnownTunion).tag != 1)goto _LL7D;_tmp103=(_tmp102.KnownTunion).f1;
_tmp104=*_tmp103;_LL7C: if(_tmp104->is_flat)return(void*)1;else{return(void*)2;}
_LL7D: if(_tmpF9 <= (void*)4)goto _LL7F;if(*((int*)_tmpF9)!= 3)goto _LL7F;_tmp105=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpF9)->f1;_tmp106=_tmp105.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF9)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL7F;_tmp107=(_tmp106.KnownTunionfield).f1;_tmp108=(_tmp106.KnownTunionfield).f2;
_LL7E: if(_tmp107->is_flat)return(void*)1;else{if(_tmp108->typs == 0)return(void*)
2;else{return(void*)1;}}_LL7F: if(_tmpF9 <= (void*)4)goto _LL81;if(*((int*)_tmpF9)
!= 3)goto _LL81;_tmp109=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF9)->f1;
_tmp10A=_tmp109.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF9)->f1).field_info).UnknownTunionfield).tag != 0)goto _LL81;_LL80:({void*
_tmp117[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp118="typ_kind: Unresolved TunionFieldType";_tag_dynforward(_tmp118,
sizeof(char),_get_zero_arr_size(_tmp118,37));}),_tag_dynforward(_tmp117,sizeof(
void*),0));});_LL81: if(_tmpF9 <= (void*)4)goto _LL83;if(*((int*)_tmpF9)!= 12)goto
_LL83;_tmp10B=((struct Cyc_Absyn_EnumType_struct*)_tmpF9)->f2;if(_tmp10B != 0)goto
_LL83;_LL82: goto _LL84;_LL83: if(_tmpF9 <= (void*)4)goto _LL85;if(*((int*)_tmpF9)!= 
10)goto _LL85;_tmp10C=((struct Cyc_Absyn_AggrType_struct*)_tmpF9)->f1;_tmp10D=
_tmp10C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF9)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL85;_LL84: return(void*)0;_LL85: if(_tmpF9 <= (void*)4)goto _LL87;if(*((
int*)_tmpF9)!= 10)goto _LL87;_tmp10E=((struct Cyc_Absyn_AggrType_struct*)_tmpF9)->f1;
_tmp10F=_tmp10E.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF9)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL87;_tmp110=(_tmp10F.KnownAggr).f1;_tmp111=*_tmp110;_tmp112=*_tmp111;
_tmp113=_tmp112.impl;_LL86: return _tmp113 == 0?(void*)0:(void*)1;_LL87: if(_tmpF9 <= (
void*)4)goto _LL89;if(*((int*)_tmpF9)!= 11)goto _LL89;_LL88: goto _LL8A;_LL89: if(
_tmpF9 <= (void*)4)goto _LL8B;if(*((int*)_tmpF9)!= 13)goto _LL8B;_LL8A: return(void*)
1;_LL8B: if(_tmpF9 <= (void*)4)goto _LL8D;if(*((int*)_tmpF9)!= 12)goto _LL8D;_tmp114=((
struct Cyc_Absyn_EnumType_struct*)_tmpF9)->f2;_LL8C: if(_tmp114->fields == 0)return(
void*)0;else{return(void*)1;}_LL8D: if(_tmpF9 <= (void*)4)goto _LL8F;if(*((int*)
_tmpF9)!= 4)goto _LL8F;_tmp115=((struct Cyc_Absyn_PointerType_struct*)_tmpF9)->f1;
_LL8E: {union Cyc_Absyn_Constraint_union _tmp119=(Cyc_Absyn_compress_conref((
_tmp115.ptr_atts).bounds))->v;void*_tmp11A;void*_tmp11B;void*_tmp11C;void*
_tmp11D;_LLA2: if((_tmp119.No_constr).tag != 2)goto _LLA4;_LLA3: goto _LLA5;_LLA4: if((
_tmp119.Eq_constr).tag != 0)goto _LLA6;_tmp11A=(_tmp119.Eq_constr).f1;if((int)
_tmp11A != 0)goto _LLA6;_LLA5: return(void*)1;_LLA6: if((_tmp119.Eq_constr).tag != 0)
goto _LLA8;_tmp11B=(_tmp119.Eq_constr).f1;if((int)_tmp11B != 1)goto _LLA8;_LLA7:
return(void*)1;_LLA8: if((_tmp119.Eq_constr).tag != 0)goto _LLAA;_tmp11C=(_tmp119.Eq_constr).f1;
if(_tmp11C <= (void*)2)goto _LLAA;if(*((int*)_tmp11C)!= 0)goto _LLAA;_LLA9: goto
_LLAB;_LLAA: if((_tmp119.Eq_constr).tag != 0)goto _LLAC;_tmp11D=(_tmp119.Eq_constr).f1;
if(_tmp11D <= (void*)2)goto _LLAC;if(*((int*)_tmp11D)!= 1)goto _LLAC;_LLAB: return(
void*)2;_LLAC: if((_tmp119.Forward_constr).tag != 1)goto _LLA1;_LLAD:({void*_tmp11E[
0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp11F="typ_kind: forward constr in ptr bounds";_tag_dynforward(
_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,39));}),_tag_dynforward(_tmp11E,
sizeof(void*),0));});_LLA1:;}_LL8F: if(_tmpF9 <= (void*)4)goto _LL91;if(*((int*)
_tmpF9)!= 14)goto _LL91;_LL90: return(void*)2;_LL91: if(_tmpF9 <= (void*)4)goto _LL93;
if(*((int*)_tmpF9)!= 19)goto _LL93;_LL92: return(void*)5;_LL93: if(_tmpF9 <= (void*)
4)goto _LL95;if(*((int*)_tmpF9)!= 18)goto _LL95;_LL94: return(void*)2;_LL95: if(
_tmpF9 <= (void*)4)goto _LL97;if(*((int*)_tmpF9)!= 7)goto _LL97;_LL96: goto _LL98;
_LL97: if(_tmpF9 <= (void*)4)goto _LL99;if(*((int*)_tmpF9)!= 9)goto _LL99;_LL98:
return(void*)1;_LL99: if(_tmpF9 <= (void*)4)goto _LL9B;if(*((int*)_tmpF9)!= 17)goto
_LL9B;_tmp116=((struct Cyc_Absyn_TypedefType_struct*)_tmpF9)->f3;_LL9A: if(_tmp116
== 0  || _tmp116->kind == 0)({struct Cyc_String_pa_struct _tmp122;_tmp122.tag=0;
_tmp122.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp120[1]={& _tmp122};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp121="typ_kind: typedef found: %s";
_tag_dynforward(_tmp121,sizeof(char),_get_zero_arr_size(_tmp121,28));}),
_tag_dynforward(_tmp120,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp116->kind))->v;_LL9B: if(_tmpF9 <= (void*)4)goto _LL9D;if(*((int*)
_tmpF9)!= 20)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if(_tmpF9 <= (void*)4)goto _LL9F;if(*((
int*)_tmpF9)!= 21)goto _LL9F;_LL9E: goto _LLA0;_LL9F: if(_tmpF9 <= (void*)4)goto _LL62;
if(*((int*)_tmpF9)!= 22)goto _LL62;_LLA0: return(void*)4;_LL62:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp123;_push_handler(& _tmp123);{int _tmp125=
0;if(setjmp(_tmp123.handler))_tmp125=1;if(!_tmp125){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp126=1;_npop_handler(0);return _tmp126;};_pop_handler();}else{void*_tmp124=(
void*)_exn_thrown;void*_tmp128=_tmp124;_LLAF: if(_tmp128 != Cyc_Tcutil_Unify)goto
_LLB1;_LLB0: return 0;_LLB1:;_LLB2:(void)_throw(_tmp128);_LLAE:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp129=t;struct Cyc_Absyn_Tvar*
_tmp12A;struct Cyc_Core_Opt*_tmp12B;struct Cyc_Core_Opt*_tmp12C;struct Cyc_Core_Opt**
_tmp12D;struct Cyc_Absyn_PtrInfo _tmp12E;struct Cyc_Absyn_ArrayInfo _tmp12F;void*
_tmp130;struct Cyc_Absyn_FnInfo _tmp131;struct Cyc_List_List*_tmp132;struct Cyc_Core_Opt*
_tmp133;void*_tmp134;struct Cyc_List_List*_tmp135;int _tmp136;struct Cyc_Absyn_VarargInfo*
_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_List_List*_tmp139;struct Cyc_List_List*
_tmp13A;struct Cyc_Absyn_TunionInfo _tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_Core_Opt*
_tmp13D;struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_TunionFieldInfo _tmp13F;
struct Cyc_List_List*_tmp140;struct Cyc_Absyn_AggrInfo _tmp141;struct Cyc_List_List*
_tmp142;struct Cyc_List_List*_tmp143;void*_tmp144;void*_tmp145;void*_tmp146;void*
_tmp147;void*_tmp148;struct Cyc_List_List*_tmp149;_LLB4: if(_tmp129 <= (void*)4)
goto _LLD6;if(*((int*)_tmp129)!= 1)goto _LLB6;_tmp12A=((struct Cyc_Absyn_VarType_struct*)
_tmp129)->f1;_LLB5: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp12A)){Cyc_Tcutil_failure_reason=({const char*_tmp14A="(type variable would escape scope)";
_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,35));});(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LLB3;_LLB6: if(*((int*)_tmp129)!= 0)goto
_LLB8;_tmp12B=((struct Cyc_Absyn_Evar_struct*)_tmp129)->f2;_tmp12C=((struct Cyc_Absyn_Evar_struct*)
_tmp129)->f4;_tmp12D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp129)->f4;_LLB7: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp14B="(occurs check)";
_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,15));});(int)
_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmp12B != 0)Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp12B->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp12D))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp14C=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp12D))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp14C=({struct Cyc_List_List*_tmp14D=
_cycalloc(sizeof(*_tmp14D));_tmp14D->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp14D->tl=
_tmp14C;_tmp14D;});}}*_tmp12D=({struct Cyc_Core_Opt*_tmp14E=_cycalloc(sizeof(*
_tmp14E));_tmp14E->v=_tmp14C;_tmp14E;});}}}goto _LLB3;_LLB8: if(*((int*)_tmp129)!= 
4)goto _LLBA;_tmp12E=((struct Cyc_Absyn_PointerType_struct*)_tmp129)->f1;_LLB9: Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12E.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp12E.ptr_atts).rgn);{
union Cyc_Absyn_Constraint_union _tmp14F=(Cyc_Absyn_compress_conref((_tmp12E.ptr_atts).bounds))->v;
void*_tmp150;void*_tmp151;_LLD9: if((_tmp14F.Eq_constr).tag != 0)goto _LLDB;_tmp150=(
_tmp14F.Eq_constr).f1;if(_tmp150 <= (void*)2)goto _LLDB;if(*((int*)_tmp150)!= 1)
goto _LLDB;_tmp151=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp150)->f1;_LLDA:
Cyc_Tcutil_occurs(evar,r,env,_tmp151);goto _LLD8;_LLDB:;_LLDC: goto _LLD8;_LLD8:;}
goto _LLB3;_LLBA: if(*((int*)_tmp129)!= 7)goto _LLBC;_tmp12F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp129)->f1;_tmp130=(void*)_tmp12F.elt_type;_LLBB: Cyc_Tcutil_occurs(evar,r,env,
_tmp130);goto _LLB3;_LLBC: if(*((int*)_tmp129)!= 8)goto _LLBE;_tmp131=((struct Cyc_Absyn_FnType_struct*)
_tmp129)->f1;_tmp132=_tmp131.tvars;_tmp133=_tmp131.effect;_tmp134=(void*)_tmp131.ret_typ;
_tmp135=_tmp131.args;_tmp136=_tmp131.c_varargs;_tmp137=_tmp131.cyc_varargs;
_tmp138=_tmp131.rgn_po;_tmp139=_tmp131.attributes;_LLBD: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp132,env);if(_tmp133 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp133->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp134);for(0;_tmp135 != 0;_tmp135=_tmp135->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple3*)_tmp135->hd)).f3);}if(_tmp137 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp137->type);for(0;_tmp138 != 0;_tmp138=_tmp138->tl){struct
_tuple0 _tmp153;void*_tmp154;void*_tmp155;struct _tuple0*_tmp152=(struct _tuple0*)
_tmp138->hd;_tmp153=*_tmp152;_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp154);Cyc_Tcutil_occurs(evar,r,env,_tmp155);}goto _LLB3;_LLBE: if(*((
int*)_tmp129)!= 9)goto _LLC0;_tmp13A=((struct Cyc_Absyn_TupleType_struct*)_tmp129)->f1;
_LLBF: for(0;_tmp13A != 0;_tmp13A=_tmp13A->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple5*)_tmp13A->hd)).f2);}goto _LLB3;_LLC0: if(*((int*)_tmp129)!= 2)goto
_LLC2;_tmp13B=((struct Cyc_Absyn_TunionType_struct*)_tmp129)->f1;_tmp13C=_tmp13B.targs;
_tmp13D=_tmp13B.rgn;_LLC1: if((unsigned int)_tmp13D)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp13D->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp13C);goto _LLB3;_LLC2: if(*((
int*)_tmp129)!= 17)goto _LLC4;_tmp13E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp129)->f2;_LLC3: _tmp140=_tmp13E;goto _LLC5;_LLC4: if(*((int*)_tmp129)!= 3)goto
_LLC6;_tmp13F=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp129)->f1;_tmp140=
_tmp13F.targs;_LLC5: _tmp142=_tmp140;goto _LLC7;_LLC6: if(*((int*)_tmp129)!= 10)
goto _LLC8;_tmp141=((struct Cyc_Absyn_AggrType_struct*)_tmp129)->f1;_tmp142=
_tmp141.targs;_LLC7: Cyc_Tcutil_occurslist(evar,r,env,_tmp142);goto _LLB3;_LLC8:
if(*((int*)_tmp129)!= 11)goto _LLCA;_tmp143=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp129)->f2;_LLC9: for(0;_tmp143 != 0;_tmp143=_tmp143->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp143->hd)->type);}goto _LLB3;_LLCA:
if(*((int*)_tmp129)!= 18)goto _LLCC;_tmp144=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp129)->f1;_LLCB: _tmp145=_tmp144;goto _LLCD;_LLCC: if(*((int*)_tmp129)!= 14)goto
_LLCE;_tmp145=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp129)->f1;_LLCD:
_tmp146=_tmp145;goto _LLCF;_LLCE: if(*((int*)_tmp129)!= 20)goto _LLD0;_tmp146=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp129)->f1;_LLCF: _tmp147=_tmp146;goto _LLD1;
_LLD0: if(*((int*)_tmp129)!= 15)goto _LLD2;_tmp147=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp129)->f1;_LLD1: _tmp148=_tmp147;goto _LLD3;_LLD2: if(*((int*)_tmp129)!= 22)goto
_LLD4;_tmp148=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp129)->f1;_LLD3: Cyc_Tcutil_occurs(
evar,r,env,_tmp148);goto _LLB3;_LLD4: if(*((int*)_tmp129)!= 21)goto _LLD6;_tmp149=((
struct Cyc_Absyn_JoinEff_struct*)_tmp129)->f1;_LLD5: Cyc_Tcutil_occurslist(evar,r,
env,_tmp149);goto _LLB3;_LLD6:;_LLD7: goto _LLB3;_LLB3:;}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp156[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp157="tq1 real_const not set.";
_tag_dynforward(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,24));}),
_tag_dynforward(_tmp156,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp158[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp159="tq2 real_const not set.";_tag_dynforward(
_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,24));}),_tag_dynforward(_tmp158,
sizeof(void*),0));});if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({
const char*_tmp15A="(qualifiers don't match)";_tag_dynforward(_tmp15A,sizeof(char),
_get_zero_arr_size(_tmp15A,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dynforward_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint_union _tmp15B=x->v;void*_tmp15C;
_LLDE: if((_tmp15B.No_constr).tag != 2)goto _LLE0;_LLDF: x->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp15D;(_tmp15D.Forward_constr).tag=1;(_tmp15D.Forward_constr).f1=
y;_tmp15D;});return;_LLE0: if((_tmp15B.Eq_constr).tag != 0)goto _LLE2;_tmp15C=(
_tmp15B.Eq_constr).f1;_LLE1: {union Cyc_Absyn_Constraint_union _tmp15E=y->v;void*
_tmp15F;_LLE5: if((_tmp15E.No_constr).tag != 2)goto _LLE7;_LLE6: y->v=x->v;return;
_LLE7: if((_tmp15E.Eq_constr).tag != 0)goto _LLE9;_tmp15F=(_tmp15E.Eq_constr).f1;
_LLE8: if(cmp(_tmp15C,_tmp15F)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLE9: if((_tmp15E.Forward_constr).tag != 1)goto
_LLE4;_LLEA:({void*_tmp160[0]={};Cyc_Tcutil_impos(({const char*_tmp161="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp161,sizeof(char),_get_zero_arr_size(_tmp161,40));}),
_tag_dynforward(_tmp160,sizeof(void*),0));});_LLE4:;}_LLE2: if((_tmp15B.Forward_constr).tag
!= 1)goto _LLDD;_LLE3:({void*_tmp162[0]={};Cyc_Tcutil_impos(({const char*_tmp163="unify_conref: forward after compress";
_tag_dynforward(_tmp163,sizeof(char),_get_zero_arr_size(_tmp163,37));}),
_tag_dynforward(_tmp162,sizeof(void*),0));});_LLDD:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp164;_push_handler(& _tmp164);{int _tmp166=0;if(setjmp(_tmp164.handler))
_tmp166=1;if(!_tmp166){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct
_dynforward_ptr)_tag_dynforward(0,0,0));{int _tmp167=1;_npop_handler(0);return
_tmp167;};_pop_handler();}else{void*_tmp165=(void*)_exn_thrown;void*_tmp169=
_tmp165;_LLEC: if(_tmp169 != Cyc_Tcutil_Unify)goto _LLEE;_LLED: return 0;_LLEE:;_LLEF:(
void)_throw(_tmp169);_LLEB:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp16B=({struct _tuple0 _tmp16A;_tmp16A.f1=b1;_tmp16A.f2=b2;_tmp16A;});
void*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;void*_tmp170;void*_tmp171;
void*_tmp172;void*_tmp173;void*_tmp174;struct Cyc_Absyn_Exp*_tmp175;void*_tmp176;
struct Cyc_Absyn_Exp*_tmp177;void*_tmp178;void*_tmp179;void*_tmp17A;void*_tmp17B;
void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;_LLF1: _tmp16C=_tmp16B.f1;if((
int)_tmp16C != 0)goto _LLF3;_tmp16D=_tmp16B.f2;if((int)_tmp16D != 0)goto _LLF3;_LLF2:
return 0;_LLF3: _tmp16E=_tmp16B.f1;if((int)_tmp16E != 0)goto _LLF5;_LLF4: return - 1;
_LLF5: _tmp16F=_tmp16B.f2;if((int)_tmp16F != 0)goto _LLF7;_LLF6: return 1;_LLF7:
_tmp170=_tmp16B.f1;if((int)_tmp170 != 1)goto _LLF9;_tmp171=_tmp16B.f2;if((int)
_tmp171 != 1)goto _LLF9;_LLF8: return 0;_LLF9: _tmp172=_tmp16B.f1;if((int)_tmp172 != 1)
goto _LLFB;_LLFA: return - 1;_LLFB: _tmp173=_tmp16B.f2;if((int)_tmp173 != 1)goto _LLFD;
_LLFC: return 1;_LLFD: _tmp174=_tmp16B.f1;if(_tmp174 <= (void*)2)goto _LLFF;if(*((int*)
_tmp174)!= 0)goto _LLFF;_tmp175=((struct Cyc_Absyn_Upper_b_struct*)_tmp174)->f1;
_tmp176=_tmp16B.f2;if(_tmp176 <= (void*)2)goto _LLFF;if(*((int*)_tmp176)!= 0)goto
_LLFF;_tmp177=((struct Cyc_Absyn_Upper_b_struct*)_tmp176)->f1;_LLFE: return Cyc_Evexp_const_exp_cmp(
_tmp175,_tmp177);_LLFF: _tmp178=_tmp16B.f1;if(_tmp178 <= (void*)2)goto _LL101;if(*((
int*)_tmp178)!= 0)goto _LL101;_tmp179=_tmp16B.f2;if(_tmp179 <= (void*)2)goto _LL101;
if(*((int*)_tmp179)!= 1)goto _LL101;_LL100: return - 1;_LL101: _tmp17A=_tmp16B.f1;if(
_tmp17A <= (void*)2)goto _LL103;if(*((int*)_tmp17A)!= 1)goto _LL103;_tmp17B=_tmp16B.f2;
if(_tmp17B <= (void*)2)goto _LL103;if(*((int*)_tmp17B)!= 0)goto _LL103;_LL102:
return 1;_LL103: _tmp17C=_tmp16B.f1;if(_tmp17C <= (void*)2)goto _LLF0;if(*((int*)
_tmp17C)!= 1)goto _LLF0;_tmp17D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp17C)->f1;_tmp17E=_tmp16B.f2;if(_tmp17E <= (void*)2)goto _LLF0;if(*((int*)
_tmp17E)!= 1)goto _LLF0;_tmp17F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp17E)->f1;_LL104: return Cyc_Tcutil_typecmp(_tmp17D,_tmp17F);_LLF0:;}static int
Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0 _tmp181=({struct _tuple0
_tmp180;_tmp180.f1=b1;_tmp180.f2=b2;_tmp180;});void*_tmp182;void*_tmp183;void*
_tmp184;void*_tmp185;void*_tmp186;void*_tmp187;void*_tmp188;void*_tmp189;void*
_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;void*_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;
void*_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;void*_tmp190;void*_tmp191;void*_tmp192;
void*_tmp193;void*_tmp194;struct Cyc_Absyn_Exp*_tmp195;void*_tmp196;void*_tmp197;
void*_tmp198;void*_tmp199;_LL106: _tmp182=_tmp181.f1;if((int)_tmp182 != 0)goto
_LL108;_tmp183=_tmp181.f2;if((int)_tmp183 != 0)goto _LL108;_LL107: return 0;_LL108:
_tmp184=_tmp181.f1;if((int)_tmp184 != 0)goto _LL10A;_LL109: return - 1;_LL10A: _tmp185=
_tmp181.f2;if((int)_tmp185 != 0)goto _LL10C;_LL10B: return 1;_LL10C: _tmp186=_tmp181.f1;
if((int)_tmp186 != 1)goto _LL10E;_tmp187=_tmp181.f2;if((int)_tmp187 != 1)goto _LL10E;
_LL10D: return 0;_LL10E: _tmp188=_tmp181.f1;if((int)_tmp188 != 1)goto _LL110;_LL10F:
return - 1;_LL110: _tmp189=_tmp181.f2;if((int)_tmp189 != 1)goto _LL112;_LL111: return 1;
_LL112: _tmp18A=_tmp181.f1;if(_tmp18A <= (void*)2)goto _LL114;if(*((int*)_tmp18A)!= 
0)goto _LL114;_tmp18B=((struct Cyc_Absyn_Upper_b_struct*)_tmp18A)->f1;_tmp18C=
_tmp181.f2;if(_tmp18C <= (void*)2)goto _LL114;if(*((int*)_tmp18C)!= 0)goto _LL114;
_tmp18D=((struct Cyc_Absyn_Upper_b_struct*)_tmp18C)->f1;_LL113: return Cyc_Evexp_const_exp_cmp(
_tmp18B,_tmp18D);_LL114: _tmp18E=_tmp181.f1;if(_tmp18E <= (void*)2)goto _LL116;if(*((
int*)_tmp18E)!= 0)goto _LL116;_tmp18F=((struct Cyc_Absyn_Upper_b_struct*)_tmp18E)->f1;
_tmp190=_tmp181.f2;if(_tmp190 <= (void*)2)goto _LL116;if(*((int*)_tmp190)!= 1)goto
_LL116;_tmp191=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp190)->f1;_LL115:
_tmp193=_tmp191;_tmp195=_tmp18F;goto _LL117;_LL116: _tmp192=_tmp181.f1;if(_tmp192
<= (void*)2)goto _LL118;if(*((int*)_tmp192)!= 1)goto _LL118;_tmp193=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp192)->f1;_tmp194=_tmp181.f2;if(_tmp194 <= (
void*)2)goto _LL118;if(*((int*)_tmp194)!= 0)goto _LL118;_tmp195=((struct Cyc_Absyn_Upper_b_struct*)
_tmp194)->f1;_LL117: {unsigned int _tmp19B;int _tmp19C;struct _tuple7 _tmp19A=Cyc_Evexp_eval_const_uint_exp(
_tmp195);_tmp19B=_tmp19A.f1;_tmp19C=_tmp19A.f2;if(!_tmp19C)return 1;_tmp197=
_tmp193;_tmp199=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp19D=_cycalloc_atomic(
sizeof(*_tmp19D));_tmp19D[0]=({struct Cyc_Absyn_TypeInt_struct _tmp19E;_tmp19E.tag=
19;_tmp19E.f1=(int)_tmp19B;_tmp19E;});_tmp19D;});goto _LL119;}_LL118: _tmp196=
_tmp181.f1;if(_tmp196 <= (void*)2)goto _LL105;if(*((int*)_tmp196)!= 1)goto _LL105;
_tmp197=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp196)->f1;_tmp198=_tmp181.f2;
if(_tmp198 <= (void*)2)goto _LL105;if(*((int*)_tmp198)!= 1)goto _LL105;_tmp199=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp198)->f1;_LL119: Cyc_Tcutil_unify_it(
_tmp197,_tmp199);return 0;_LL105:;}static int Cyc_Tcutil_attribute_case_number(void*
att){void*_tmp19F=att;_LL11B: if(_tmp19F <= (void*)17)goto _LL11D;if(*((int*)
_tmp19F)!= 0)goto _LL11D;_LL11C: return 0;_LL11D: if((int)_tmp19F != 0)goto _LL11F;
_LL11E: return 1;_LL11F: if((int)_tmp19F != 1)goto _LL121;_LL120: return 2;_LL121: if((
int)_tmp19F != 2)goto _LL123;_LL122: return 3;_LL123: if((int)_tmp19F != 3)goto _LL125;
_LL124: return 4;_LL125: if((int)_tmp19F != 4)goto _LL127;_LL126: return 5;_LL127: if(
_tmp19F <= (void*)17)goto _LL129;if(*((int*)_tmp19F)!= 1)goto _LL129;_LL128: return 6;
_LL129: if((int)_tmp19F != 5)goto _LL12B;_LL12A: return 7;_LL12B: if(_tmp19F <= (void*)
17)goto _LL12D;if(*((int*)_tmp19F)!= 2)goto _LL12D;_LL12C: return 8;_LL12D: if((int)
_tmp19F != 6)goto _LL12F;_LL12E: return 9;_LL12F: if((int)_tmp19F != 7)goto _LL131;
_LL130: return 10;_LL131: if((int)_tmp19F != 8)goto _LL133;_LL132: return 11;_LL133: if((
int)_tmp19F != 9)goto _LL135;_LL134: return 12;_LL135: if((int)_tmp19F != 10)goto
_LL137;_LL136: return 13;_LL137: if((int)_tmp19F != 11)goto _LL139;_LL138: return 14;
_LL139: if((int)_tmp19F != 12)goto _LL13B;_LL13A: return 15;_LL13B: if((int)_tmp19F != 
13)goto _LL13D;_LL13C: return 16;_LL13D: if((int)_tmp19F != 14)goto _LL13F;_LL13E:
return 17;_LL13F: if((int)_tmp19F != 15)goto _LL141;_LL140: return 18;_LL141: if(
_tmp19F <= (void*)17)goto _LL145;if(*((int*)_tmp19F)!= 3)goto _LL143;_LL142: return
19;_LL143: if(*((int*)_tmp19F)!= 4)goto _LL145;_LL144: return 20;_LL145:;_LL146:
return 21;_LL11A:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple0 _tmp1A1=({struct _tuple0 _tmp1A0;_tmp1A0.f1=att1;_tmp1A0.f2=att2;_tmp1A0;});
void*_tmp1A2;int _tmp1A3;void*_tmp1A4;int _tmp1A5;void*_tmp1A6;int _tmp1A7;void*
_tmp1A8;int _tmp1A9;void*_tmp1AA;int _tmp1AB;void*_tmp1AC;int _tmp1AD;void*_tmp1AE;
struct _dynforward_ptr _tmp1AF;void*_tmp1B0;struct _dynforward_ptr _tmp1B1;void*
_tmp1B2;void*_tmp1B3;int _tmp1B4;int _tmp1B5;void*_tmp1B6;void*_tmp1B7;int _tmp1B8;
int _tmp1B9;_LL148: _tmp1A2=_tmp1A1.f1;if(_tmp1A2 <= (void*)17)goto _LL14A;if(*((int*)
_tmp1A2)!= 0)goto _LL14A;_tmp1A3=((struct Cyc_Absyn_Regparm_att_struct*)_tmp1A2)->f1;
_tmp1A4=_tmp1A1.f2;if(_tmp1A4 <= (void*)17)goto _LL14A;if(*((int*)_tmp1A4)!= 0)
goto _LL14A;_tmp1A5=((struct Cyc_Absyn_Regparm_att_struct*)_tmp1A4)->f1;_LL149:
_tmp1A7=_tmp1A3;_tmp1A9=_tmp1A5;goto _LL14B;_LL14A: _tmp1A6=_tmp1A1.f1;if(_tmp1A6
<= (void*)17)goto _LL14C;if(*((int*)_tmp1A6)!= 4)goto _LL14C;_tmp1A7=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A6)->f1;_tmp1A8=_tmp1A1.f2;if(_tmp1A8 <= (void*)17)goto _LL14C;if(*((int*)
_tmp1A8)!= 4)goto _LL14C;_tmp1A9=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A8)->f1;_LL14B: _tmp1AB=_tmp1A7;_tmp1AD=_tmp1A9;goto _LL14D;_LL14C: _tmp1AA=
_tmp1A1.f1;if(_tmp1AA <= (void*)17)goto _LL14E;if(*((int*)_tmp1AA)!= 1)goto _LL14E;
_tmp1AB=((struct Cyc_Absyn_Aligned_att_struct*)_tmp1AA)->f1;_tmp1AC=_tmp1A1.f2;
if(_tmp1AC <= (void*)17)goto _LL14E;if(*((int*)_tmp1AC)!= 1)goto _LL14E;_tmp1AD=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp1AC)->f1;_LL14D: return Cyc_Core_intcmp(
_tmp1AB,_tmp1AD);_LL14E: _tmp1AE=_tmp1A1.f1;if(_tmp1AE <= (void*)17)goto _LL150;if(*((
int*)_tmp1AE)!= 2)goto _LL150;_tmp1AF=((struct Cyc_Absyn_Section_att_struct*)
_tmp1AE)->f1;_tmp1B0=_tmp1A1.f2;if(_tmp1B0 <= (void*)17)goto _LL150;if(*((int*)
_tmp1B0)!= 2)goto _LL150;_tmp1B1=((struct Cyc_Absyn_Section_att_struct*)_tmp1B0)->f1;
_LL14F: return Cyc_strcmp((struct _dynforward_ptr)_tmp1AF,(struct _dynforward_ptr)
_tmp1B1);_LL150: _tmp1B2=_tmp1A1.f1;if(_tmp1B2 <= (void*)17)goto _LL152;if(*((int*)
_tmp1B2)!= 3)goto _LL152;_tmp1B3=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1B2)->f1;_tmp1B4=((struct Cyc_Absyn_Format_att_struct*)_tmp1B2)->f2;_tmp1B5=((
struct Cyc_Absyn_Format_att_struct*)_tmp1B2)->f3;_tmp1B6=_tmp1A1.f2;if(_tmp1B6 <= (
void*)17)goto _LL152;if(*((int*)_tmp1B6)!= 3)goto _LL152;_tmp1B7=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1B6)->f1;_tmp1B8=((struct Cyc_Absyn_Format_att_struct*)
_tmp1B6)->f2;_tmp1B9=((struct Cyc_Absyn_Format_att_struct*)_tmp1B6)->f3;_LL151: {
int _tmp1BA=Cyc_Core_intcmp((int)((unsigned int)_tmp1B3),(int)((unsigned int)
_tmp1B7));if(_tmp1BA != 0)return _tmp1BA;{int _tmp1BB=Cyc_Core_intcmp(_tmp1B4,
_tmp1B8);if(_tmp1BB != 0)return _tmp1BB;return Cyc_Core_intcmp(_tmp1B5,_tmp1B9);}}
_LL152:;_LL153: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL147:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct Cyc_Absyn_TypeInt_struct Cyc_Tcutil_tizero={
19,0};static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;{void*_tmp1BD=Cyc_Tcutil_tvar_kind(tv);_LL155: if((int)_tmp1BD != 3)goto
_LL157;_LL156: t=(void*)2;goto _LL154;_LL157: if((int)_tmp1BD != 4)goto _LL159;_LL158:
t=Cyc_Absyn_empty_effect;goto _LL154;_LL159: if((int)_tmp1BD != 5)goto _LL15B;_LL15A:
t=(void*)& Cyc_Tcutil_tizero;goto _LL154;_LL15B:;_LL15C: t=Cyc_Absyn_sint_typ;goto
_LL154;_LL154:;}return({struct _tuple8*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));
_tmp1BE->f1=tv;_tmp1BE->f2=t;_tmp1BE;});}static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1BF=Cyc_Tcutil_compress(t);void*_tmp1C0;void*_tmp1C1;void*_tmp1C2;
struct Cyc_Absyn_TunionInfo _tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_Core_Opt*
_tmp1C5;struct Cyc_Absyn_PtrInfo _tmp1C6;void*_tmp1C7;struct Cyc_Absyn_PtrAtts
_tmp1C8;void*_tmp1C9;struct Cyc_Absyn_ArrayInfo _tmp1CA;void*_tmp1CB;struct Cyc_List_List*
_tmp1CC;struct Cyc_Absyn_TunionFieldInfo _tmp1CD;struct Cyc_List_List*_tmp1CE;
struct Cyc_Absyn_AggrInfo _tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_List_List*
_tmp1D1;void*_tmp1D2;struct Cyc_Absyn_FnInfo _tmp1D3;struct Cyc_List_List*_tmp1D4;
struct Cyc_Core_Opt*_tmp1D5;void*_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_Absyn_VarargInfo*
_tmp1D8;struct Cyc_List_List*_tmp1D9;void*_tmp1DA;struct Cyc_List_List*_tmp1DB;
_LL15E: if((int)_tmp1BF != 0)goto _LL160;_LL15F: goto _LL161;_LL160: if((int)_tmp1BF != 
1)goto _LL162;_LL161: goto _LL163;_LL162: if(_tmp1BF <= (void*)4)goto _LL188;if(*((int*)
_tmp1BF)!= 6)goto _LL164;_LL163: goto _LL165;_LL164: if(*((int*)_tmp1BF)!= 12)goto
_LL166;_LL165: goto _LL167;_LL166: if(*((int*)_tmp1BF)!= 13)goto _LL168;_LL167: goto
_LL169;_LL168: if(*((int*)_tmp1BF)!= 19)goto _LL16A;_LL169: goto _LL16B;_LL16A: if(*((
int*)_tmp1BF)!= 5)goto _LL16C;_LL16B: return Cyc_Absyn_empty_effect;_LL16C: if(*((
int*)_tmp1BF)!= 0)goto _LL16E;_LL16D: goto _LL16F;_LL16E: if(*((int*)_tmp1BF)!= 1)
goto _LL170;_LL16F: {void*_tmp1DC=Cyc_Tcutil_typ_kind(t);_LL195: if((int)_tmp1DC != 
3)goto _LL197;_LL196: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DD=
_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1DE;
_tmp1DE.tag=20;_tmp1DE.f1=(void*)t;_tmp1DE;});_tmp1DD;});_LL197: if((int)_tmp1DC
!= 4)goto _LL199;_LL198: return t;_LL199: if((int)_tmp1DC != 5)goto _LL19B;_LL19A:
return Cyc_Absyn_empty_effect;_LL19B:;_LL19C: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1E0;_tmp1E0.tag=22;_tmp1E0.f1=(void*)t;_tmp1E0;});_tmp1DF;});_LL194:;}_LL170:
if(*((int*)_tmp1BF)!= 15)goto _LL172;_tmp1C0=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1BF)->f1;_LL171: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E1=
_cycalloc(sizeof(*_tmp1E1));_tmp1E1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E2;
_tmp1E2.tag=20;_tmp1E2.f1=(void*)_tmp1C0;_tmp1E2;});_tmp1E1;});_LL172: if(*((int*)
_tmp1BF)!= 16)goto _LL174;_tmp1C1=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp1BF)->f1;_tmp1C2=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp1BF)->f2;
_LL173: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E3=_cycalloc(sizeof(*
_tmp1E3));_tmp1E3[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E4;_tmp1E4.tag=20;
_tmp1E4.f1=(void*)_tmp1C2;_tmp1E4;});_tmp1E3;});_LL174: if(*((int*)_tmp1BF)!= 2)
goto _LL176;_tmp1C3=((struct Cyc_Absyn_TunionType_struct*)_tmp1BF)->f1;_tmp1C4=
_tmp1C3.targs;_tmp1C5=_tmp1C3.rgn;_LL175: {struct Cyc_List_List*ts=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp1C4);if((unsigned int)_tmp1C5)ts=({struct Cyc_List_List*
_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1E7;_tmp1E7.tag=20;_tmp1E7.f1=(void*)((void*)_tmp1C5->v);_tmp1E7;});_tmp1E6;}));
_tmp1E5->tl=ts;_tmp1E5;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E9;_tmp1E9.tag=21;_tmp1E9.f1=ts;_tmp1E9;});_tmp1E8;}));}_LL176: if(*((int*)
_tmp1BF)!= 4)goto _LL178;_tmp1C6=((struct Cyc_Absyn_PointerType_struct*)_tmp1BF)->f1;
_tmp1C7=(void*)_tmp1C6.elt_typ;_tmp1C8=_tmp1C6.ptr_atts;_tmp1C9=(void*)_tmp1C8.rgn;
_LL177: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1EB;_tmp1EB.tag=21;_tmp1EB.f1=({void*_tmp1EC[2];_tmp1EC[1]=Cyc_Tcutil_rgns_of(
_tmp1C7);_tmp1EC[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1ED=_cycalloc(
sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1EE;_tmp1EE.tag=
20;_tmp1EE.f1=(void*)_tmp1C9;_tmp1EE;});_tmp1ED;});Cyc_List_list(_tag_dynforward(
_tmp1EC,sizeof(void*),2));});_tmp1EB;});_tmp1EA;}));_LL178: if(*((int*)_tmp1BF)!= 
7)goto _LL17A;_tmp1CA=((struct Cyc_Absyn_ArrayType_struct*)_tmp1BF)->f1;_tmp1CB=(
void*)_tmp1CA.elt_type;_LL179: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp1CB));_LL17A: if(*((int*)_tmp1BF)!= 9)goto _LL17C;_tmp1CC=((struct Cyc_Absyn_TupleType_struct*)
_tmp1BF)->f1;_LL17B: {struct Cyc_List_List*_tmp1EF=0;for(0;_tmp1CC != 0;_tmp1CC=
_tmp1CC->tl){_tmp1EF=({struct Cyc_List_List*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));
_tmp1F0->hd=(void*)(*((struct _tuple5*)_tmp1CC->hd)).f2;_tmp1F0->tl=_tmp1EF;
_tmp1F0;});}_tmp1CE=_tmp1EF;goto _LL17D;}_LL17C: if(*((int*)_tmp1BF)!= 3)goto
_LL17E;_tmp1CD=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1BF)->f1;_tmp1CE=
_tmp1CD.targs;_LL17D: _tmp1D0=_tmp1CE;goto _LL17F;_LL17E: if(*((int*)_tmp1BF)!= 10)
goto _LL180;_tmp1CF=((struct Cyc_Absyn_AggrType_struct*)_tmp1BF)->f1;_tmp1D0=
_tmp1CF.targs;_LL17F: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1F2;_tmp1F2.tag=21;_tmp1F2.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1D0);
_tmp1F2;});_tmp1F1;}));_LL180: if(*((int*)_tmp1BF)!= 11)goto _LL182;_tmp1D1=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1BF)->f2;_LL181: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F4;_tmp1F4.tag=21;_tmp1F4.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1D1);_tmp1F4;});_tmp1F3;}));_LL182:
if(*((int*)_tmp1BF)!= 14)goto _LL184;_tmp1D2=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1BF)->f1;_LL183: return Cyc_Tcutil_rgns_of(_tmp1D2);_LL184: if(*((int*)_tmp1BF)
!= 18)goto _LL186;_LL185: return Cyc_Absyn_empty_effect;_LL186: if(*((int*)_tmp1BF)
!= 8)goto _LL188;_tmp1D3=((struct Cyc_Absyn_FnType_struct*)_tmp1BF)->f1;_tmp1D4=
_tmp1D3.tvars;_tmp1D5=_tmp1D3.effect;_tmp1D6=(void*)_tmp1D3.ret_typ;_tmp1D7=
_tmp1D3.args;_tmp1D8=_tmp1D3.cyc_varargs;_tmp1D9=_tmp1D3.rgn_po;_LL187: {void*
_tmp1F5=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1D4),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D5))->v);return Cyc_Tcutil_normalize_effect(
_tmp1F5);}_LL188: if((int)_tmp1BF != 3)goto _LL18A;_LL189: goto _LL18B;_LL18A: if((int)
_tmp1BF != 2)goto _LL18C;_LL18B: return Cyc_Absyn_empty_effect;_LL18C: if(_tmp1BF <= (
void*)4)goto _LL18E;if(*((int*)_tmp1BF)!= 20)goto _LL18E;_LL18D: goto _LL18F;_LL18E:
if(_tmp1BF <= (void*)4)goto _LL190;if(*((int*)_tmp1BF)!= 21)goto _LL190;_LL18F:
return t;_LL190: if(_tmp1BF <= (void*)4)goto _LL192;if(*((int*)_tmp1BF)!= 22)goto
_LL192;_tmp1DA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1BF)->f1;_LL191:
return Cyc_Tcutil_rgns_of(_tmp1DA);_LL192: if(_tmp1BF <= (void*)4)goto _LL15D;if(*((
int*)_tmp1BF)!= 17)goto _LL15D;_tmp1DB=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1BF)->f2;_LL193: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1F7;_tmp1F7.tag=21;_tmp1F7.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1DB);
_tmp1F7;});_tmp1F6;}));_LL15D:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1F8=e;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List**_tmp1FA;void*
_tmp1FB;_LL19E: if(_tmp1F8 <= (void*)4)goto _LL1A2;if(*((int*)_tmp1F8)!= 21)goto
_LL1A0;_tmp1F9=((struct Cyc_Absyn_JoinEff_struct*)_tmp1F8)->f1;_tmp1FA=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1F8)->f1;_LL19F: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1FA;for(0;effs != 0;effs=effs->tl){void*_tmp1FC=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1FC)));{void*
_tmp1FD=(void*)effs->hd;void*_tmp1FE;_LL1A5: if(_tmp1FD <= (void*)4)goto _LL1A9;if(*((
int*)_tmp1FD)!= 21)goto _LL1A7;_LL1A6: goto _LL1A8;_LL1A7: if(*((int*)_tmp1FD)!= 20)
goto _LL1A9;_tmp1FE=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1FD)->f1;if((
int)_tmp1FE != 2)goto _LL1A9;_LL1A8: redo_join=1;goto _LL1A4;_LL1A9:;_LL1AA: goto
_LL1A4;_LL1A4:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1FA;for(0;effs != 0;effs=effs->tl){void*_tmp1FF=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp200;void*_tmp201;_LL1AC: if(_tmp1FF <= (
void*)4)goto _LL1B0;if(*((int*)_tmp1FF)!= 21)goto _LL1AE;_tmp200=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1FF)->f1;_LL1AD: effects=Cyc_List_revappend(_tmp200,effects);goto _LL1AB;
_LL1AE: if(*((int*)_tmp1FF)!= 20)goto _LL1B0;_tmp201=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1FF)->f1;if((int)_tmp201 != 2)goto _LL1B0;_LL1AF: goto _LL1AB;_LL1B0:;_LL1B1:
effects=({struct Cyc_List_List*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->hd=(
void*)_tmp1FF;_tmp202->tl=effects;_tmp202;});goto _LL1AB;_LL1AB:;}}*_tmp1FA=Cyc_List_imp_rev(
effects);return e;}}_LL1A0: if(*((int*)_tmp1F8)!= 22)goto _LL1A2;_tmp1FB=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1F8)->f1;_LL1A1: {void*_tmp203=Cyc_Tcutil_compress(
_tmp1FB);_LL1B3: if(_tmp203 <= (void*)4)goto _LL1B7;if(*((int*)_tmp203)!= 0)goto
_LL1B5;_LL1B4: goto _LL1B6;_LL1B5: if(*((int*)_tmp203)!= 1)goto _LL1B7;_LL1B6: return
e;_LL1B7:;_LL1B8: return Cyc_Tcutil_rgns_of(_tmp1FB);_LL1B2:;}_LL1A2:;_LL1A3:
return e;_LL19D:;}}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};
static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp204=({struct Cyc_Absyn_FnType_struct*_tmp205=_cycalloc(sizeof(*_tmp205));
_tmp205[0]=({struct Cyc_Absyn_FnType_struct _tmp206;_tmp206.tag=8;_tmp206.f1=({
struct Cyc_Absyn_FnInfo _tmp207;_tmp207.tvars=0;_tmp207.effect=({struct Cyc_Core_Opt*
_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->v=(void*)eff;_tmp208;});_tmp207.ret_typ=(
void*)((void*)0);_tmp207.args=0;_tmp207.c_varargs=0;_tmp207.cyc_varargs=0;
_tmp207.rgn_po=0;_tmp207.attributes=0;_tmp207;});_tmp206;});_tmp205;});return Cyc_Absyn_atb_typ((
void*)_tmp204,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2)return 1;{void*_tmp209=Cyc_Tcutil_compress(e);void*_tmp20A;
struct Cyc_List_List*_tmp20B;void*_tmp20C;struct Cyc_Core_Opt*_tmp20D;struct Cyc_Core_Opt*
_tmp20E;struct Cyc_Core_Opt**_tmp20F;struct Cyc_Core_Opt*_tmp210;_LL1BA: if(_tmp209
<= (void*)4)goto _LL1C2;if(*((int*)_tmp209)!= 20)goto _LL1BC;_tmp20A=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp209)->f1;_LL1BB: if(constrain)return Cyc_Tcutil_unify(
r,_tmp20A);_tmp20A=Cyc_Tcutil_compress(_tmp20A);if(r == _tmp20A)return 1;{struct
_tuple0 _tmp212=({struct _tuple0 _tmp211;_tmp211.f1=r;_tmp211.f2=_tmp20A;_tmp211;});
void*_tmp213;struct Cyc_Absyn_Tvar*_tmp214;void*_tmp215;struct Cyc_Absyn_Tvar*
_tmp216;_LL1C5: _tmp213=_tmp212.f1;if(_tmp213 <= (void*)4)goto _LL1C7;if(*((int*)
_tmp213)!= 1)goto _LL1C7;_tmp214=((struct Cyc_Absyn_VarType_struct*)_tmp213)->f1;
_tmp215=_tmp212.f2;if(_tmp215 <= (void*)4)goto _LL1C7;if(*((int*)_tmp215)!= 1)goto
_LL1C7;_tmp216=((struct Cyc_Absyn_VarType_struct*)_tmp215)->f1;_LL1C6: return Cyc_Absyn_tvar_cmp(
_tmp214,_tmp216)== 0;_LL1C7:;_LL1C8: return 0;_LL1C4:;}_LL1BC: if(*((int*)_tmp209)
!= 21)goto _LL1BE;_tmp20B=((struct Cyc_Absyn_JoinEff_struct*)_tmp209)->f1;_LL1BD:
for(0;_tmp20B != 0;_tmp20B=_tmp20B->tl){if(Cyc_Tcutil_region_in_effect(constrain,
r,(void*)_tmp20B->hd))return 1;}return 0;_LL1BE: if(*((int*)_tmp209)!= 22)goto
_LL1C0;_tmp20C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp209)->f1;_LL1BF: {
void*_tmp217=Cyc_Tcutil_rgns_of(_tmp20C);void*_tmp218;_LL1CA: if(_tmp217 <= (void*)
4)goto _LL1CC;if(*((int*)_tmp217)!= 22)goto _LL1CC;_tmp218=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp217)->f1;_LL1CB: if(!constrain)return 0;{void*_tmp219=Cyc_Tcutil_compress(
_tmp218);struct Cyc_Core_Opt*_tmp21A;struct Cyc_Core_Opt*_tmp21B;struct Cyc_Core_Opt**
_tmp21C;struct Cyc_Core_Opt*_tmp21D;_LL1CF: if(_tmp219 <= (void*)4)goto _LL1D1;if(*((
int*)_tmp219)!= 0)goto _LL1D1;_tmp21A=((struct Cyc_Absyn_Evar_struct*)_tmp219)->f1;
_tmp21B=((struct Cyc_Absyn_Evar_struct*)_tmp219)->f2;_tmp21C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp219)->f2;_tmp21D=((struct Cyc_Absyn_Evar_struct*)
_tmp219)->f4;_LL1D0: {void*_tmp21E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp21D);Cyc_Tcutil_occurs(_tmp21E,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp21D))->v,r);{void*_tmp21F=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp221=_cycalloc(sizeof(*_tmp221));
_tmp221[0]=({struct Cyc_Absyn_JoinEff_struct _tmp222;_tmp222.tag=21;_tmp222.f1=({
void*_tmp223[2];_tmp223[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp224=
_cycalloc(sizeof(*_tmp224));_tmp224[0]=({struct Cyc_Absyn_AccessEff_struct _tmp225;
_tmp225.tag=20;_tmp225.f1=(void*)r;_tmp225;});_tmp224;});_tmp223[0]=_tmp21E;Cyc_List_list(
_tag_dynforward(_tmp223,sizeof(void*),2));});_tmp222;});_tmp221;}));*_tmp21C=({
struct Cyc_Core_Opt*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220->v=(void*)_tmp21F;
_tmp220;});return 1;}}_LL1D1:;_LL1D2: return 0;_LL1CE:;}_LL1CC:;_LL1CD: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp217);_LL1C9:;}_LL1C0: if(*((int*)_tmp209)!= 0)goto _LL1C2;_tmp20D=((
struct Cyc_Absyn_Evar_struct*)_tmp209)->f1;_tmp20E=((struct Cyc_Absyn_Evar_struct*)
_tmp209)->f2;_tmp20F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp209)->f2;_tmp210=((struct Cyc_Absyn_Evar_struct*)_tmp209)->f4;_LL1C1: if(
_tmp20D == 0  || (void*)_tmp20D->v != (void*)4)({void*_tmp226[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp227="effect evar has wrong kind";_tag_dynforward(_tmp227,sizeof(char),
_get_zero_arr_size(_tmp227,27));}),_tag_dynforward(_tmp226,sizeof(void*),0));});
if(!constrain)return 0;{void*_tmp228=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp210);Cyc_Tcutil_occurs(_tmp228,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp210))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp229=({struct Cyc_Absyn_JoinEff_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));
_tmp22B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp22C;_tmp22C.tag=21;_tmp22C.f1=({
struct Cyc_List_List*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->hd=(void*)
_tmp228;_tmp22D->tl=({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(*_tmp22E));
_tmp22E->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp22F=_cycalloc(
sizeof(*_tmp22F));_tmp22F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp230;_tmp230.tag=
20;_tmp230.f1=(void*)r;_tmp230;});_tmp22F;}));_tmp22E->tl=0;_tmp22E;});_tmp22D;});
_tmp22C;});_tmp22B;});*_tmp20F=({struct Cyc_Core_Opt*_tmp22A=_cycalloc(sizeof(*
_tmp22A));_tmp22A->v=(void*)((void*)_tmp229);_tmp22A;});return 1;}}_LL1C2:;_LL1C3:
return 0;_LL1B9:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp231=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp232;void*_tmp233;struct Cyc_Core_Opt*_tmp234;struct
Cyc_Core_Opt*_tmp235;struct Cyc_Core_Opt**_tmp236;struct Cyc_Core_Opt*_tmp237;
_LL1D4: if(_tmp231 <= (void*)4)goto _LL1DC;if(*((int*)_tmp231)!= 20)goto _LL1D6;
_LL1D5: return 0;_LL1D6: if(*((int*)_tmp231)!= 21)goto _LL1D8;_tmp232=((struct Cyc_Absyn_JoinEff_struct*)
_tmp231)->f1;_LL1D7: for(0;_tmp232 != 0;_tmp232=_tmp232->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp232->hd))return 1;}return 0;_LL1D8: if(*((int*)
_tmp231)!= 22)goto _LL1DA;_tmp233=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp231)->f1;_LL1D9: _tmp233=Cyc_Tcutil_compress(_tmp233);if(t == _tmp233)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp233);{void*_tmp238=Cyc_Tcutil_rgns_of(
t);void*_tmp239;_LL1DF: if(_tmp238 <= (void*)4)goto _LL1E1;if(*((int*)_tmp238)!= 22)
goto _LL1E1;_tmp239=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp238)->f1;_LL1E0: {
struct _tuple0 _tmp23B=({struct _tuple0 _tmp23A;_tmp23A.f1=Cyc_Tcutil_compress(
_tmp239);_tmp23A.f2=_tmp233;_tmp23A;});void*_tmp23C;struct Cyc_Absyn_Tvar*_tmp23D;
void*_tmp23E;struct Cyc_Absyn_Tvar*_tmp23F;_LL1E4: _tmp23C=_tmp23B.f1;if(_tmp23C <= (
void*)4)goto _LL1E6;if(*((int*)_tmp23C)!= 1)goto _LL1E6;_tmp23D=((struct Cyc_Absyn_VarType_struct*)
_tmp23C)->f1;_tmp23E=_tmp23B.f2;if(_tmp23E <= (void*)4)goto _LL1E6;if(*((int*)
_tmp23E)!= 1)goto _LL1E6;_tmp23F=((struct Cyc_Absyn_VarType_struct*)_tmp23E)->f1;
_LL1E5: return Cyc_Tcutil_unify(t,_tmp233);_LL1E6:;_LL1E7: return _tmp239 == _tmp233;
_LL1E3:;}_LL1E1:;_LL1E2: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp238);_LL1DE:;}_LL1DA: if(*((int*)_tmp231)!= 0)goto _LL1DC;_tmp234=((struct Cyc_Absyn_Evar_struct*)
_tmp231)->f1;_tmp235=((struct Cyc_Absyn_Evar_struct*)_tmp231)->f2;_tmp236=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp231)->f2;_tmp237=((struct Cyc_Absyn_Evar_struct*)
_tmp231)->f4;_LL1DB: if(_tmp234 == 0  || (void*)_tmp234->v != (void*)4)({void*
_tmp240[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp241="effect evar has wrong kind";_tag_dynforward(_tmp241,sizeof(
char),_get_zero_arr_size(_tmp241,27));}),_tag_dynforward(_tmp240,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{void*_tmp242=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp237);Cyc_Tcutil_occurs(_tmp242,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp237))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp243=({struct Cyc_Absyn_JoinEff_struct*_tmp245=_cycalloc(sizeof(*_tmp245));
_tmp245[0]=({struct Cyc_Absyn_JoinEff_struct _tmp246;_tmp246.tag=21;_tmp246.f1=({
struct Cyc_List_List*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247->hd=(void*)
_tmp242;_tmp247->tl=({struct Cyc_List_List*_tmp248=_cycalloc(sizeof(*_tmp248));
_tmp248->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp249=_cycalloc(
sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp24A;_tmp24A.tag=
22;_tmp24A.f1=(void*)t;_tmp24A;});_tmp249;}));_tmp248->tl=0;_tmp248;});_tmp247;});
_tmp246;});_tmp245;});*_tmp236=({struct Cyc_Core_Opt*_tmp244=_cycalloc(sizeof(*
_tmp244));_tmp244->v=(void*)((void*)_tmp243);_tmp244;});return 1;}}_LL1DC:;_LL1DD:
return 0;_LL1D3:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp24B=e;struct
Cyc_Absyn_Tvar*_tmp24C;struct Cyc_List_List*_tmp24D;void*_tmp24E;struct Cyc_Core_Opt*
_tmp24F;struct Cyc_Core_Opt*_tmp250;struct Cyc_Core_Opt**_tmp251;struct Cyc_Core_Opt*
_tmp252;_LL1E9: if(_tmp24B <= (void*)4)goto _LL1F1;if(*((int*)_tmp24B)!= 1)goto
_LL1EB;_tmp24C=((struct Cyc_Absyn_VarType_struct*)_tmp24B)->f1;_LL1EA: return Cyc_Absyn_tvar_cmp(
v,_tmp24C)== 0;_LL1EB: if(*((int*)_tmp24B)!= 21)goto _LL1ED;_tmp24D=((struct Cyc_Absyn_JoinEff_struct*)
_tmp24B)->f1;_LL1EC: for(0;_tmp24D != 0;_tmp24D=_tmp24D->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp24D->hd))return 1;}return 0;_LL1ED: if(*((int*)
_tmp24B)!= 22)goto _LL1EF;_tmp24E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp24B)->f1;_LL1EE: {void*_tmp253=Cyc_Tcutil_rgns_of(_tmp24E);void*_tmp254;
_LL1F4: if(_tmp253 <= (void*)4)goto _LL1F6;if(*((int*)_tmp253)!= 22)goto _LL1F6;
_tmp254=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp253)->f1;_LL1F5: if(!
may_constrain_evars)return 0;{void*_tmp255=Cyc_Tcutil_compress(_tmp254);struct Cyc_Core_Opt*
_tmp256;struct Cyc_Core_Opt*_tmp257;struct Cyc_Core_Opt**_tmp258;struct Cyc_Core_Opt*
_tmp259;_LL1F9: if(_tmp255 <= (void*)4)goto _LL1FB;if(*((int*)_tmp255)!= 0)goto
_LL1FB;_tmp256=((struct Cyc_Absyn_Evar_struct*)_tmp255)->f1;_tmp257=((struct Cyc_Absyn_Evar_struct*)
_tmp255)->f2;_tmp258=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp255)->f2;_tmp259=((struct Cyc_Absyn_Evar_struct*)_tmp255)->f4;_LL1FA: {void*
_tmp25A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp259);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp259))->v,v))return 0;{void*_tmp25B=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));
_tmp25D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp25E;_tmp25E.tag=21;_tmp25E.f1=({
void*_tmp25F[2];_tmp25F[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp260=
_cycalloc(sizeof(*_tmp260));_tmp260[0]=({struct Cyc_Absyn_VarType_struct _tmp261;
_tmp261.tag=1;_tmp261.f1=v;_tmp261;});_tmp260;});_tmp25F[0]=_tmp25A;Cyc_List_list(
_tag_dynforward(_tmp25F,sizeof(void*),2));});_tmp25E;});_tmp25D;}));*_tmp258=({
struct Cyc_Core_Opt*_tmp25C=_cycalloc(sizeof(*_tmp25C));_tmp25C->v=(void*)_tmp25B;
_tmp25C;});return 1;}}_LL1FB:;_LL1FC: return 0;_LL1F8:;}_LL1F6:;_LL1F7: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp253);_LL1F3:;}_LL1EF: if(*((int*)_tmp24B)!= 0)goto
_LL1F1;_tmp24F=((struct Cyc_Absyn_Evar_struct*)_tmp24B)->f1;_tmp250=((struct Cyc_Absyn_Evar_struct*)
_tmp24B)->f2;_tmp251=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp24B)->f2;_tmp252=((struct Cyc_Absyn_Evar_struct*)_tmp24B)->f4;_LL1F0: if(
_tmp24F == 0  || (void*)_tmp24F->v != (void*)4)({void*_tmp262[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp263="effect evar has wrong kind";_tag_dynforward(_tmp263,sizeof(char),
_get_zero_arr_size(_tmp263,27));}),_tag_dynforward(_tmp262,sizeof(void*),0));});{
void*_tmp264=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp252);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp252))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp265=({struct Cyc_Absyn_JoinEff_struct*_tmp267=_cycalloc(sizeof(*_tmp267));
_tmp267[0]=({struct Cyc_Absyn_JoinEff_struct _tmp268;_tmp268.tag=21;_tmp268.f1=({
struct Cyc_List_List*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->hd=(void*)
_tmp264;_tmp269->tl=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));
_tmp26A->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp26B=_cycalloc(
sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Absyn_VarType_struct _tmp26C;_tmp26C.tag=
1;_tmp26C.f1=v;_tmp26C;});_tmp26B;}));_tmp26A->tl=0;_tmp26A;});_tmp269;});
_tmp268;});_tmp267;});*_tmp251=({struct Cyc_Core_Opt*_tmp266=_cycalloc(sizeof(*
_tmp266));_tmp266->v=(void*)((void*)_tmp265);_tmp266;});return 1;}}_LL1F1:;_LL1F2:
return 0;_LL1E8:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp26D=e;struct Cyc_List_List*_tmp26E;void*_tmp26F;_LL1FE: if(_tmp26D <= (
void*)4)goto _LL204;if(*((int*)_tmp26D)!= 21)goto _LL200;_tmp26E=((struct Cyc_Absyn_JoinEff_struct*)
_tmp26D)->f1;_LL1FF: for(0;_tmp26E != 0;_tmp26E=_tmp26E->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp26E->hd))return 1;}return 0;_LL200: if(*((int*)_tmp26D)!= 22)goto
_LL202;_tmp26F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp26D)->f1;_LL201: {
void*_tmp270=Cyc_Tcutil_rgns_of(_tmp26F);void*_tmp271;_LL207: if(_tmp270 <= (void*)
4)goto _LL209;if(*((int*)_tmp270)!= 22)goto _LL209;_tmp271=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp270)->f1;_LL208: return 0;_LL209:;_LL20A: return Cyc_Tcutil_evar_in_effect(evar,
_tmp270);_LL206:;}_LL202: if(*((int*)_tmp26D)!= 0)goto _LL204;_LL203: return evar == 
e;_LL204:;_LL205: return 0;_LL1FD:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp272=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp273;void*_tmp274;struct Cyc_Absyn_Tvar*_tmp275;void*_tmp276;
struct Cyc_Core_Opt*_tmp277;struct Cyc_Core_Opt**_tmp278;struct Cyc_Core_Opt*
_tmp279;_LL20C: if(_tmp272 <= (void*)4)goto _LL216;if(*((int*)_tmp272)!= 21)goto
_LL20E;_tmp273=((struct Cyc_Absyn_JoinEff_struct*)_tmp272)->f1;_LL20D: for(0;
_tmp273 != 0;_tmp273=_tmp273->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp273->hd,e2))return 0;}return 1;_LL20E: if(*((int*)
_tmp272)!= 20)goto _LL210;_tmp274=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp272)->f1;_LL20F: return Cyc_Tcutil_region_in_effect(0,_tmp274,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp274,(void*)2);_LL210: if(*((int*)
_tmp272)!= 1)goto _LL212;_tmp275=((struct Cyc_Absyn_VarType_struct*)_tmp272)->f1;
_LL211: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp275,e2);
_LL212: if(*((int*)_tmp272)!= 22)goto _LL214;_tmp276=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp272)->f1;_LL213: {void*_tmp27A=Cyc_Tcutil_rgns_of(_tmp276);void*_tmp27B;
_LL219: if(_tmp27A <= (void*)4)goto _LL21B;if(*((int*)_tmp27A)!= 22)goto _LL21B;
_tmp27B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp27A)->f1;_LL21A: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp27B,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp27B,Cyc_Absyn_sint_typ);_LL21B:;_LL21C: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp27A,e2);_LL218:;}_LL214: if(*((int*)_tmp272)!= 0)goto
_LL216;_tmp277=((struct Cyc_Absyn_Evar_struct*)_tmp272)->f2;_tmp278=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp272)->f2;_tmp279=((struct Cyc_Absyn_Evar_struct*)
_tmp272)->f4;_LL215: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp278=({struct Cyc_Core_Opt*
_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->v=(void*)Cyc_Absyn_empty_effect;
_tmp27C;});return 1;_LL216:;_LL217:({struct Cyc_String_pa_struct _tmp27F;_tmp27F.tag=
0;_tmp27F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp27D[1]={& _tmp27F};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp27E="subset_effect: bad effect: %s";
_tag_dynforward(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,30));}),
_tag_dynforward(_tmp27D,sizeof(void*),1));}});_LL20B:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){struct
_tuple0 _tmp281;void*_tmp282;void*_tmp283;struct _tuple0*_tmp280=(struct _tuple0*)
r1->hd;_tmp281=*_tmp280;_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;{int found=_tmp282
== (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0 _tmp285;void*_tmp286;void*_tmp287;struct _tuple0*_tmp284=(struct
_tuple0*)r2->hd;_tmp285=*_tmp284;_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;if(Cyc_Tcutil_unify(
_tmp282,_tmp286) && Cyc_Tcutil_unify(_tmp283,_tmp287)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp288=t1;struct Cyc_Core_Opt*_tmp289;struct Cyc_Core_Opt*
_tmp28A;struct Cyc_Core_Opt**_tmp28B;struct Cyc_Core_Opt*_tmp28C;_LL21E: if(_tmp288
<= (void*)4)goto _LL220;if(*((int*)_tmp288)!= 0)goto _LL220;_tmp289=((struct Cyc_Absyn_Evar_struct*)
_tmp288)->f1;_tmp28A=((struct Cyc_Absyn_Evar_struct*)_tmp288)->f2;_tmp28B=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp288)->f2;_tmp28C=((struct Cyc_Absyn_Evar_struct*)
_tmp288)->f4;_LL21F: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp28C))->v,t2);{void*_tmp28D=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp28D,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp289))->v)){*_tmp28B=({struct Cyc_Core_Opt*_tmp28E=_cycalloc(sizeof(*_tmp28E));
_tmp28E->v=(void*)t2;_tmp28E;});return;}else{{void*_tmp28F=t2;struct Cyc_Core_Opt*
_tmp290;struct Cyc_Core_Opt**_tmp291;struct Cyc_Core_Opt*_tmp292;struct Cyc_Absyn_PtrInfo
_tmp293;_LL223: if(_tmp28F <= (void*)4)goto _LL227;if(*((int*)_tmp28F)!= 0)goto
_LL225;_tmp290=((struct Cyc_Absyn_Evar_struct*)_tmp28F)->f2;_tmp291=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp28F)->f2;_tmp292=((struct Cyc_Absyn_Evar_struct*)
_tmp28F)->f4;_LL224: {struct Cyc_List_List*_tmp294=(struct Cyc_List_List*)_tmp28C->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp292))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp294,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp295="(type variable would escape scope)";_tag_dynforward(_tmp295,
sizeof(char),_get_zero_arr_size(_tmp295,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp289->v,_tmp28D)){*_tmp291=({struct Cyc_Core_Opt*
_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->v=(void*)t1;_tmp296;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp297="(kinds are incompatible)";_tag_dynforward(_tmp297,sizeof(char),
_get_zero_arr_size(_tmp297,25));});goto _LL222;}_LL225: if(*((int*)_tmp28F)!= 4)
goto _LL227;_tmp293=((struct Cyc_Absyn_PointerType_struct*)_tmp28F)->f1;if(!((void*)
_tmp289->v == (void*)2))goto _LL227;_LL226: {struct Cyc_Absyn_Conref*_tmp298=Cyc_Absyn_compress_conref((
_tmp293.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union _tmp299=_tmp298->v;
_LL22A: if((_tmp299.No_constr).tag != 2)goto _LL22C;_LL22B: _tmp298->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp29A;(_tmp29A.Eq_constr).tag=0;(_tmp29A.Eq_constr).f1=(
void*)Cyc_Absyn_bounds_one;_tmp29A;});*_tmp28B=({struct Cyc_Core_Opt*_tmp29B=
_cycalloc(sizeof(*_tmp29B));_tmp29B->v=(void*)t2;_tmp29B;});return;_LL22C:;
_LL22D: goto _LL229;_LL229:;}goto _LL222;}_LL227:;_LL228: goto _LL222;_LL222:;}Cyc_Tcutil_failure_reason=({
const char*_tmp29C="(kinds are incompatible)";_tag_dynforward(_tmp29C,sizeof(char),
_get_zero_arr_size(_tmp29C,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}}_LL220:;
_LL221: goto _LL21D;_LL21D:;}{struct _tuple0 _tmp29E=({struct _tuple0 _tmp29D;_tmp29D.f1=
t2;_tmp29D.f2=t1;_tmp29D;});void*_tmp29F;void*_tmp2A0;void*_tmp2A1;void*_tmp2A2;
struct Cyc_Absyn_Tvar*_tmp2A3;void*_tmp2A4;struct Cyc_Absyn_Tvar*_tmp2A5;void*
_tmp2A6;struct Cyc_Absyn_AggrInfo _tmp2A7;union Cyc_Absyn_AggrInfoU_union _tmp2A8;
struct Cyc_List_List*_tmp2A9;void*_tmp2AA;struct Cyc_Absyn_AggrInfo _tmp2AB;union
Cyc_Absyn_AggrInfoU_union _tmp2AC;struct Cyc_List_List*_tmp2AD;void*_tmp2AE;struct
_tuple2*_tmp2AF;void*_tmp2B0;struct _tuple2*_tmp2B1;void*_tmp2B2;struct Cyc_List_List*
_tmp2B3;void*_tmp2B4;struct Cyc_List_List*_tmp2B5;void*_tmp2B6;struct Cyc_Absyn_TunionInfo
_tmp2B7;union Cyc_Absyn_TunionInfoU_union _tmp2B8;struct Cyc_Absyn_Tuniondecl**
_tmp2B9;struct Cyc_Absyn_Tuniondecl*_tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_Core_Opt*
_tmp2BC;void*_tmp2BD;struct Cyc_Absyn_TunionInfo _tmp2BE;union Cyc_Absyn_TunionInfoU_union
_tmp2BF;struct Cyc_Absyn_Tuniondecl**_tmp2C0;struct Cyc_Absyn_Tuniondecl*_tmp2C1;
struct Cyc_List_List*_tmp2C2;struct Cyc_Core_Opt*_tmp2C3;void*_tmp2C4;struct Cyc_Absyn_TunionFieldInfo
_tmp2C5;union Cyc_Absyn_TunionFieldInfoU_union _tmp2C6;struct Cyc_Absyn_Tuniondecl*
_tmp2C7;struct Cyc_Absyn_Tunionfield*_tmp2C8;struct Cyc_List_List*_tmp2C9;void*
_tmp2CA;struct Cyc_Absyn_TunionFieldInfo _tmp2CB;union Cyc_Absyn_TunionFieldInfoU_union
_tmp2CC;struct Cyc_Absyn_Tuniondecl*_tmp2CD;struct Cyc_Absyn_Tunionfield*_tmp2CE;
struct Cyc_List_List*_tmp2CF;void*_tmp2D0;struct Cyc_Absyn_PtrInfo _tmp2D1;void*
_tmp2D2;struct Cyc_Absyn_Tqual _tmp2D3;struct Cyc_Absyn_PtrAtts _tmp2D4;void*_tmp2D5;
struct Cyc_Absyn_Conref*_tmp2D6;struct Cyc_Absyn_Conref*_tmp2D7;struct Cyc_Absyn_Conref*
_tmp2D8;void*_tmp2D9;struct Cyc_Absyn_PtrInfo _tmp2DA;void*_tmp2DB;struct Cyc_Absyn_Tqual
_tmp2DC;struct Cyc_Absyn_PtrAtts _tmp2DD;void*_tmp2DE;struct Cyc_Absyn_Conref*
_tmp2DF;struct Cyc_Absyn_Conref*_tmp2E0;struct Cyc_Absyn_Conref*_tmp2E1;void*
_tmp2E2;void*_tmp2E3;void*_tmp2E4;void*_tmp2E5;void*_tmp2E6;void*_tmp2E7;void*
_tmp2E8;void*_tmp2E9;void*_tmp2EA;int _tmp2EB;void*_tmp2EC;int _tmp2ED;void*
_tmp2EE;void*_tmp2EF;void*_tmp2F0;void*_tmp2F1;void*_tmp2F2;int _tmp2F3;void*
_tmp2F4;int _tmp2F5;void*_tmp2F6;void*_tmp2F7;void*_tmp2F8;void*_tmp2F9;void*
_tmp2FA;struct Cyc_Absyn_ArrayInfo _tmp2FB;void*_tmp2FC;struct Cyc_Absyn_Tqual
_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Conref*_tmp2FF;void*_tmp300;
struct Cyc_Absyn_ArrayInfo _tmp301;void*_tmp302;struct Cyc_Absyn_Tqual _tmp303;
struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Conref*_tmp305;void*_tmp306;struct
Cyc_Absyn_FnInfo _tmp307;struct Cyc_List_List*_tmp308;struct Cyc_Core_Opt*_tmp309;
void*_tmp30A;struct Cyc_List_List*_tmp30B;int _tmp30C;struct Cyc_Absyn_VarargInfo*
_tmp30D;struct Cyc_List_List*_tmp30E;struct Cyc_List_List*_tmp30F;void*_tmp310;
struct Cyc_Absyn_FnInfo _tmp311;struct Cyc_List_List*_tmp312;struct Cyc_Core_Opt*
_tmp313;void*_tmp314;struct Cyc_List_List*_tmp315;int _tmp316;struct Cyc_Absyn_VarargInfo*
_tmp317;struct Cyc_List_List*_tmp318;struct Cyc_List_List*_tmp319;void*_tmp31A;
struct Cyc_List_List*_tmp31B;void*_tmp31C;struct Cyc_List_List*_tmp31D;void*
_tmp31E;void*_tmp31F;struct Cyc_List_List*_tmp320;void*_tmp321;void*_tmp322;
struct Cyc_List_List*_tmp323;void*_tmp324;void*_tmp325;void*_tmp326;void*_tmp327;
void*_tmp328;void*_tmp329;void*_tmp32A;void*_tmp32B;void*_tmp32C;void*_tmp32D;
void*_tmp32E;void*_tmp32F;void*_tmp330;void*_tmp331;void*_tmp332;void*_tmp333;
void*_tmp334;void*_tmp335;void*_tmp336;void*_tmp337;_LL22F: _tmp29F=_tmp29E.f1;
if(_tmp29F <= (void*)4)goto _LL231;if(*((int*)_tmp29F)!= 0)goto _LL231;_LL230: Cyc_Tcutil_unify_it(
t2,t1);return;_LL231: _tmp2A0=_tmp29E.f1;if((int)_tmp2A0 != 0)goto _LL233;_tmp2A1=
_tmp29E.f2;if((int)_tmp2A1 != 0)goto _LL233;_LL232: return;_LL233: _tmp2A2=_tmp29E.f1;
if(_tmp2A2 <= (void*)4)goto _LL235;if(*((int*)_tmp2A2)!= 1)goto _LL235;_tmp2A3=((
struct Cyc_Absyn_VarType_struct*)_tmp2A2)->f1;_tmp2A4=_tmp29E.f2;if(_tmp2A4 <= (
void*)4)goto _LL235;if(*((int*)_tmp2A4)!= 1)goto _LL235;_tmp2A5=((struct Cyc_Absyn_VarType_struct*)
_tmp2A4)->f1;_LL234: {struct _dynforward_ptr*_tmp338=_tmp2A3->name;struct
_dynforward_ptr*_tmp339=_tmp2A5->name;int _tmp33A=*((int*)_check_null(_tmp2A3->identity));
int _tmp33B=*((int*)_check_null(_tmp2A5->identity));void*_tmp33C=Cyc_Tcutil_tvar_kind(
_tmp2A3);void*_tmp33D=Cyc_Tcutil_tvar_kind(_tmp2A5);if(_tmp33B == _tmp33A  && Cyc_strptrcmp(
_tmp338,_tmp339)== 0){if(_tmp33C != _tmp33D)({struct Cyc_String_pa_struct _tmp342;
_tmp342.tag=0;_tmp342.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp33D));{struct Cyc_String_pa_struct _tmp341;_tmp341.tag=0;_tmp341.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp33C));{
struct Cyc_String_pa_struct _tmp340;_tmp340.tag=0;_tmp340.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp338);{void*_tmp33E[3]={& _tmp340,&
_tmp341,& _tmp342};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp33F="same type variable %s has kinds %s and %s";_tag_dynforward(
_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,42));}),_tag_dynforward(_tmp33E,
sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*_tmp343="(variable types are not the same)";
_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,34));});goto
_LL22E;}_LL235: _tmp2A6=_tmp29E.f1;if(_tmp2A6 <= (void*)4)goto _LL237;if(*((int*)
_tmp2A6)!= 10)goto _LL237;_tmp2A7=((struct Cyc_Absyn_AggrType_struct*)_tmp2A6)->f1;
_tmp2A8=_tmp2A7.aggr_info;_tmp2A9=_tmp2A7.targs;_tmp2AA=_tmp29E.f2;if(_tmp2AA <= (
void*)4)goto _LL237;if(*((int*)_tmp2AA)!= 10)goto _LL237;_tmp2AB=((struct Cyc_Absyn_AggrType_struct*)
_tmp2AA)->f1;_tmp2AC=_tmp2AB.aggr_info;_tmp2AD=_tmp2AB.targs;_LL236: {void*
_tmp345;struct _tuple2*_tmp346;struct _tuple6 _tmp344=Cyc_Absyn_aggr_kinded_name(
_tmp2AC);_tmp345=_tmp344.f1;_tmp346=_tmp344.f2;{void*_tmp348;struct _tuple2*
_tmp349;struct _tuple6 _tmp347=Cyc_Absyn_aggr_kinded_name(_tmp2A8);_tmp348=_tmp347.f1;
_tmp349=_tmp347.f2;if(_tmp345 != _tmp348){Cyc_Tcutil_failure_reason=({const char*
_tmp34A="(struct and union type)";_tag_dynforward(_tmp34A,sizeof(char),
_get_zero_arr_size(_tmp34A,24));});goto _LL22E;}if(Cyc_Absyn_qvar_cmp(_tmp346,
_tmp349)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp34B="(different type name)";
_tag_dynforward(_tmp34B,sizeof(char),_get_zero_arr_size(_tmp34B,22));});goto
_LL22E;}Cyc_Tcutil_unify_list(_tmp2AD,_tmp2A9);return;}}_LL237: _tmp2AE=_tmp29E.f1;
if(_tmp2AE <= (void*)4)goto _LL239;if(*((int*)_tmp2AE)!= 12)goto _LL239;_tmp2AF=((
struct Cyc_Absyn_EnumType_struct*)_tmp2AE)->f1;_tmp2B0=_tmp29E.f2;if(_tmp2B0 <= (
void*)4)goto _LL239;if(*((int*)_tmp2B0)!= 12)goto _LL239;_tmp2B1=((struct Cyc_Absyn_EnumType_struct*)
_tmp2B0)->f1;_LL238: if(Cyc_Absyn_qvar_cmp(_tmp2AF,_tmp2B1)== 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp34C="(different enum types)";_tag_dynforward(_tmp34C,sizeof(char),
_get_zero_arr_size(_tmp34C,23));});goto _LL22E;_LL239: _tmp2B2=_tmp29E.f1;if(
_tmp2B2 <= (void*)4)goto _LL23B;if(*((int*)_tmp2B2)!= 13)goto _LL23B;_tmp2B3=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp2B2)->f1;_tmp2B4=_tmp29E.f2;if(_tmp2B4
<= (void*)4)goto _LL23B;if(*((int*)_tmp2B4)!= 13)goto _LL23B;_tmp2B5=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2B4)->f1;_LL23A: {int bad=0;for(0;_tmp2B3 != 0  && _tmp2B5 != 0;(_tmp2B3=_tmp2B3->tl,
_tmp2B5=_tmp2B5->tl)){struct Cyc_Absyn_Enumfield*_tmp34D=(struct Cyc_Absyn_Enumfield*)
_tmp2B3->hd;struct Cyc_Absyn_Enumfield*_tmp34E=(struct Cyc_Absyn_Enumfield*)
_tmp2B5->hd;if(Cyc_Absyn_qvar_cmp(_tmp34D->name,_tmp34E->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp34F="(different names for enum fields)";_tag_dynforward(_tmp34F,
sizeof(char),_get_zero_arr_size(_tmp34F,34));});bad=1;break;}if(_tmp34D->tag == 
_tmp34E->tag)continue;if(_tmp34D->tag == 0  || _tmp34E->tag == 0){Cyc_Tcutil_failure_reason=({
const char*_tmp350="(different tag values for enum fields)";_tag_dynforward(
_tmp350,sizeof(char),_get_zero_arr_size(_tmp350,39));});bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp34D->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp34E->tag))){Cyc_Tcutil_failure_reason=({const char*_tmp351="(different tag values for enum fields)";
_tag_dynforward(_tmp351,sizeof(char),_get_zero_arr_size(_tmp351,39));});bad=1;
break;}}if(bad)goto _LL22E;if(_tmp2B3 == 0  && _tmp2B5 == 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp352="(different number of fields for enums)";_tag_dynforward(
_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,39));});goto _LL22E;}_LL23B:
_tmp2B6=_tmp29E.f1;if(_tmp2B6 <= (void*)4)goto _LL23D;if(*((int*)_tmp2B6)!= 2)goto
_LL23D;_tmp2B7=((struct Cyc_Absyn_TunionType_struct*)_tmp2B6)->f1;_tmp2B8=_tmp2B7.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp2B6)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL23D;_tmp2B9=(_tmp2B8.KnownTunion).f1;_tmp2BA=*_tmp2B9;_tmp2BB=
_tmp2B7.targs;_tmp2BC=_tmp2B7.rgn;_tmp2BD=_tmp29E.f2;if(_tmp2BD <= (void*)4)goto
_LL23D;if(*((int*)_tmp2BD)!= 2)goto _LL23D;_tmp2BE=((struct Cyc_Absyn_TunionType_struct*)
_tmp2BD)->f1;_tmp2BF=_tmp2BE.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp2BD)->f1).tunion_info).KnownTunion).tag != 1)goto _LL23D;_tmp2C0=(_tmp2BF.KnownTunion).f1;
_tmp2C1=*_tmp2C0;_tmp2C2=_tmp2BE.targs;_tmp2C3=_tmp2BE.rgn;_LL23C: if(_tmp2BA == 
_tmp2C1  || Cyc_Absyn_qvar_cmp(_tmp2BA->name,_tmp2C1->name)== 0){if(_tmp2C3 != 0
 && _tmp2BC != 0)Cyc_Tcutil_unify_it((void*)_tmp2C3->v,(void*)_tmp2BC->v);else{
if(_tmp2C3 != 0  || _tmp2BC != 0)goto Tunion_fail;}Cyc_Tcutil_unify_list(_tmp2C2,
_tmp2BB);return;}Tunion_fail: Cyc_Tcutil_failure_reason=({const char*_tmp353="(different tunion types)";
_tag_dynforward(_tmp353,sizeof(char),_get_zero_arr_size(_tmp353,25));});goto
_LL22E;_LL23D: _tmp2C4=_tmp29E.f1;if(_tmp2C4 <= (void*)4)goto _LL23F;if(*((int*)
_tmp2C4)!= 3)goto _LL23F;_tmp2C5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2C4)->f1;_tmp2C6=_tmp2C5.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2C4)->f1).field_info).KnownTunionfield).tag != 1)goto _LL23F;_tmp2C7=(_tmp2C6.KnownTunionfield).f1;
_tmp2C8=(_tmp2C6.KnownTunionfield).f2;_tmp2C9=_tmp2C5.targs;_tmp2CA=_tmp29E.f2;
if(_tmp2CA <= (void*)4)goto _LL23F;if(*((int*)_tmp2CA)!= 3)goto _LL23F;_tmp2CB=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2CA)->f1;_tmp2CC=_tmp2CB.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2CA)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL23F;_tmp2CD=(_tmp2CC.KnownTunionfield).f1;_tmp2CE=(_tmp2CC.KnownTunionfield).f2;
_tmp2CF=_tmp2CB.targs;_LL23E: if((_tmp2C7 == _tmp2CD  || Cyc_Absyn_qvar_cmp(_tmp2C7->name,
_tmp2CD->name)== 0) && (_tmp2C8 == _tmp2CE  || Cyc_Absyn_qvar_cmp(_tmp2C8->name,
_tmp2CE->name)== 0)){Cyc_Tcutil_unify_list(_tmp2CF,_tmp2C9);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp354="(different tunion field types)";_tag_dynforward(_tmp354,
sizeof(char),_get_zero_arr_size(_tmp354,31));});goto _LL22E;_LL23F: _tmp2D0=
_tmp29E.f1;if(_tmp2D0 <= (void*)4)goto _LL241;if(*((int*)_tmp2D0)!= 4)goto _LL241;
_tmp2D1=((struct Cyc_Absyn_PointerType_struct*)_tmp2D0)->f1;_tmp2D2=(void*)
_tmp2D1.elt_typ;_tmp2D3=_tmp2D1.elt_tq;_tmp2D4=_tmp2D1.ptr_atts;_tmp2D5=(void*)
_tmp2D4.rgn;_tmp2D6=_tmp2D4.nullable;_tmp2D7=_tmp2D4.bounds;_tmp2D8=_tmp2D4.zero_term;
_tmp2D9=_tmp29E.f2;if(_tmp2D9 <= (void*)4)goto _LL241;if(*((int*)_tmp2D9)!= 4)goto
_LL241;_tmp2DA=((struct Cyc_Absyn_PointerType_struct*)_tmp2D9)->f1;_tmp2DB=(void*)
_tmp2DA.elt_typ;_tmp2DC=_tmp2DA.elt_tq;_tmp2DD=_tmp2DA.ptr_atts;_tmp2DE=(void*)
_tmp2DD.rgn;_tmp2DF=_tmp2DD.nullable;_tmp2E0=_tmp2DD.bounds;_tmp2E1=_tmp2DD.zero_term;
_LL240: Cyc_Tcutil_unify_it(_tmp2DB,_tmp2D2);Cyc_Tcutil_unify_it(_tmp2D5,_tmp2DE);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2E1,_tmp2D8,({const char*_tmp355="(not both zero terminated)";
_tag_dynforward(_tmp355,sizeof(char),_get_zero_arr_size(_tmp355,27));}));Cyc_Tcutil_unify_tqual(
_tmp2DC,_tmp2DB,_tmp2D3,_tmp2D2);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2E0,_tmp2D7,({const char*_tmp356="(different pointer bounds)";_tag_dynforward(
_tmp356,sizeof(char),_get_zero_arr_size(_tmp356,27));}));{union Cyc_Absyn_Constraint_union
_tmp357=(Cyc_Absyn_compress_conref(_tmp2E0))->v;void*_tmp358;void*_tmp359;_LL26C:
if((_tmp357.Eq_constr).tag != 0)goto _LL26E;_tmp358=(_tmp357.Eq_constr).f1;if((int)
_tmp358 != 0)goto _LL26E;_LL26D: return;_LL26E: if((_tmp357.Eq_constr).tag != 0)goto
_LL270;_tmp359=(_tmp357.Eq_constr).f1;if((int)_tmp359 != 1)goto _LL270;_LL26F:
return;_LL270:;_LL271: goto _LL26B;_LL26B:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2DF,_tmp2D6,({const char*_tmp35A="(incompatible pointer types)";
_tag_dynforward(_tmp35A,sizeof(char),_get_zero_arr_size(_tmp35A,29));}));return;
_LL241: _tmp2E2=_tmp29E.f1;if(_tmp2E2 <= (void*)4)goto _LL243;if(*((int*)_tmp2E2)!= 
5)goto _LL243;_tmp2E3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E2)->f1;
_tmp2E4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E2)->f2;_tmp2E5=_tmp29E.f2;
if(_tmp2E5 <= (void*)4)goto _LL243;if(*((int*)_tmp2E5)!= 5)goto _LL243;_tmp2E6=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E5)->f1;_tmp2E7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2E5)->f2;_LL242: if(_tmp2E6 == _tmp2E3  && ((_tmp2E7 == _tmp2E4  || _tmp2E7 == (
void*)2  && _tmp2E4 == (void*)3) || _tmp2E7 == (void*)3  && _tmp2E4 == (void*)2))
return;Cyc_Tcutil_failure_reason=({const char*_tmp35B="(different integral types)";
_tag_dynforward(_tmp35B,sizeof(char),_get_zero_arr_size(_tmp35B,27));});goto
_LL22E;_LL243: _tmp2E8=_tmp29E.f1;if((int)_tmp2E8 != 1)goto _LL245;_tmp2E9=_tmp29E.f2;
if((int)_tmp2E9 != 1)goto _LL245;_LL244: return;_LL245: _tmp2EA=_tmp29E.f1;if(
_tmp2EA <= (void*)4)goto _LL247;if(*((int*)_tmp2EA)!= 6)goto _LL247;_tmp2EB=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2EA)->f1;_tmp2EC=_tmp29E.f2;if(_tmp2EC <= (
void*)4)goto _LL247;if(*((int*)_tmp2EC)!= 6)goto _LL247;_tmp2ED=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2EC)->f1;_LL246: if(_tmp2EB == _tmp2ED)return;goto _LL22E;_LL247: _tmp2EE=
_tmp29E.f1;if(_tmp2EE <= (void*)4)goto _LL249;if(*((int*)_tmp2EE)!= 14)goto _LL249;
_tmp2EF=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2EE)->f1;_tmp2F0=_tmp29E.f2;
if(_tmp2F0 <= (void*)4)goto _LL249;if(*((int*)_tmp2F0)!= 14)goto _LL249;_tmp2F1=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2F0)->f1;_LL248: Cyc_Tcutil_unify_it(
_tmp2EF,_tmp2F1);return;_LL249: _tmp2F2=_tmp29E.f1;if(_tmp2F2 <= (void*)4)goto
_LL24B;if(*((int*)_tmp2F2)!= 19)goto _LL24B;_tmp2F3=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2F2)->f1;_tmp2F4=_tmp29E.f2;if(_tmp2F4 <= (void*)4)goto _LL24B;if(*((int*)
_tmp2F4)!= 19)goto _LL24B;_tmp2F5=((struct Cyc_Absyn_TypeInt_struct*)_tmp2F4)->f1;
_LL24A: if(_tmp2F3 == _tmp2F5)return;Cyc_Tcutil_failure_reason=({const char*_tmp35C="(different type integers)";
_tag_dynforward(_tmp35C,sizeof(char),_get_zero_arr_size(_tmp35C,26));});goto
_LL22E;_LL24B: _tmp2F6=_tmp29E.f1;if(_tmp2F6 <= (void*)4)goto _LL24D;if(*((int*)
_tmp2F6)!= 18)goto _LL24D;_tmp2F7=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2F6)->f1;_tmp2F8=_tmp29E.f2;if(_tmp2F8 <= (void*)4)goto _LL24D;if(*((int*)
_tmp2F8)!= 18)goto _LL24D;_tmp2F9=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2F8)->f1;_LL24C: Cyc_Tcutil_unify_it(_tmp2F7,_tmp2F9);return;_LL24D: _tmp2FA=
_tmp29E.f1;if(_tmp2FA <= (void*)4)goto _LL24F;if(*((int*)_tmp2FA)!= 7)goto _LL24F;
_tmp2FB=((struct Cyc_Absyn_ArrayType_struct*)_tmp2FA)->f1;_tmp2FC=(void*)_tmp2FB.elt_type;
_tmp2FD=_tmp2FB.tq;_tmp2FE=_tmp2FB.num_elts;_tmp2FF=_tmp2FB.zero_term;_tmp300=
_tmp29E.f2;if(_tmp300 <= (void*)4)goto _LL24F;if(*((int*)_tmp300)!= 7)goto _LL24F;
_tmp301=((struct Cyc_Absyn_ArrayType_struct*)_tmp300)->f1;_tmp302=(void*)_tmp301.elt_type;
_tmp303=_tmp301.tq;_tmp304=_tmp301.num_elts;_tmp305=_tmp301.zero_term;_LL24E: Cyc_Tcutil_unify_it(
_tmp302,_tmp2FC);Cyc_Tcutil_unify_tqual(_tmp303,_tmp302,_tmp2FD,_tmp2FC);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2FF,_tmp305,({const char*_tmp35D="(not both zero terminated)";
_tag_dynforward(_tmp35D,sizeof(char),_get_zero_arr_size(_tmp35D,27));}));if(
_tmp2FE == _tmp304)return;if(_tmp2FE == 0  || _tmp304 == 0)goto _LL22E;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp2FE,(struct Cyc_Absyn_Exp*)_tmp304))return;Cyc_Tcutil_failure_reason=({
const char*_tmp35E="(different array sizes)";_tag_dynforward(_tmp35E,sizeof(char),
_get_zero_arr_size(_tmp35E,24));});goto _LL22E;_LL24F: _tmp306=_tmp29E.f1;if(
_tmp306 <= (void*)4)goto _LL251;if(*((int*)_tmp306)!= 8)goto _LL251;_tmp307=((
struct Cyc_Absyn_FnType_struct*)_tmp306)->f1;_tmp308=_tmp307.tvars;_tmp309=
_tmp307.effect;_tmp30A=(void*)_tmp307.ret_typ;_tmp30B=_tmp307.args;_tmp30C=
_tmp307.c_varargs;_tmp30D=_tmp307.cyc_varargs;_tmp30E=_tmp307.rgn_po;_tmp30F=
_tmp307.attributes;_tmp310=_tmp29E.f2;if(_tmp310 <= (void*)4)goto _LL251;if(*((int*)
_tmp310)!= 8)goto _LL251;_tmp311=((struct Cyc_Absyn_FnType_struct*)_tmp310)->f1;
_tmp312=_tmp311.tvars;_tmp313=_tmp311.effect;_tmp314=(void*)_tmp311.ret_typ;
_tmp315=_tmp311.args;_tmp316=_tmp311.c_varargs;_tmp317=_tmp311.cyc_varargs;
_tmp318=_tmp311.rgn_po;_tmp319=_tmp311.attributes;_LL250: {int done=0;{struct
_RegionHandle _tmp35F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp35F;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp312 != 0){if(_tmp308 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp360="(second function type has too few type variables)";
_tag_dynforward(_tmp360,sizeof(char),_get_zero_arr_size(_tmp360,50));});(int)
_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp361=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp312->hd);void*_tmp362=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp308->hd);
if(_tmp361 != _tmp362){Cyc_Tcutil_failure_reason=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp367;_tmp367.tag=0;_tmp367.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp362));{struct Cyc_String_pa_struct
_tmp366;_tmp366.tag=0;_tmp366.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(_tmp361));{struct Cyc_String_pa_struct _tmp365;_tmp365.tag=
0;_tmp365.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp312->hd));{void*_tmp363[3]={& _tmp365,& _tmp366,& _tmp367};
Cyc_aprintf(({const char*_tmp364="(type var %s has different kinds %s and %s)";
_tag_dynforward(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,44));}),
_tag_dynforward(_tmp363,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp368=_region_malloc(rgn,sizeof(*_tmp368));_tmp368->hd=({
struct _tuple8*_tmp369=_region_malloc(rgn,sizeof(*_tmp369));_tmp369->f1=(struct
Cyc_Absyn_Tvar*)_tmp308->hd;_tmp369->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A[0]=({struct Cyc_Absyn_VarType_struct
_tmp36B;_tmp36B.tag=1;_tmp36B.f1=(struct Cyc_Absyn_Tvar*)_tmp312->hd;_tmp36B;});
_tmp36A;});_tmp369;});_tmp368->tl=inst;_tmp368;});_tmp312=_tmp312->tl;_tmp308=
_tmp308->tl;}}if(_tmp308 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp36C="(second function type has too many type variables)";
_tag_dynforward(_tmp36C,sizeof(char),_get_zero_arr_size(_tmp36C,51));});
_npop_handler(0);goto _LL22E;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=({struct Cyc_Absyn_FnType_struct
_tmp36E;_tmp36E.tag=8;_tmp36E.f1=({struct Cyc_Absyn_FnInfo _tmp36F;_tmp36F.tvars=0;
_tmp36F.effect=_tmp313;_tmp36F.ret_typ=(void*)_tmp314;_tmp36F.args=_tmp315;
_tmp36F.c_varargs=_tmp316;_tmp36F.cyc_varargs=_tmp317;_tmp36F.rgn_po=_tmp318;
_tmp36F.attributes=_tmp319;_tmp36F;});_tmp36E;});_tmp36D;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp370=_cycalloc(sizeof(*
_tmp370));_tmp370[0]=({struct Cyc_Absyn_FnType_struct _tmp371;_tmp371.tag=8;
_tmp371.f1=({struct Cyc_Absyn_FnInfo _tmp372;_tmp372.tvars=0;_tmp372.effect=
_tmp309;_tmp372.ret_typ=(void*)_tmp30A;_tmp372.args=_tmp30B;_tmp372.c_varargs=
_tmp30C;_tmp372.cyc_varargs=_tmp30D;_tmp372.rgn_po=_tmp30E;_tmp372.attributes=
_tmp30F;_tmp372;});_tmp371;});_tmp370;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp314,_tmp30A);for(0;_tmp315 != 0  && _tmp30B != 0;(
_tmp315=_tmp315->tl,_tmp30B=_tmp30B->tl)){struct Cyc_Absyn_Tqual _tmp374;void*
_tmp375;struct _tuple3 _tmp373=*((struct _tuple3*)_tmp315->hd);_tmp374=_tmp373.f2;
_tmp375=_tmp373.f3;{struct Cyc_Absyn_Tqual _tmp377;void*_tmp378;struct _tuple3
_tmp376=*((struct _tuple3*)_tmp30B->hd);_tmp377=_tmp376.f2;_tmp378=_tmp376.f3;Cyc_Tcutil_unify_it(
_tmp375,_tmp378);Cyc_Tcutil_unify_tqual(_tmp374,_tmp375,_tmp377,_tmp378);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp315 != 0  || _tmp30B != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp379="(function types have different number of arguments)";
_tag_dynforward(_tmp379,sizeof(char),_get_zero_arr_size(_tmp379,52));});goto
_LL22E;}if(_tmp316 != _tmp30C){Cyc_Tcutil_failure_reason=({const char*_tmp37A="(only one function type takes C varargs)";
_tag_dynforward(_tmp37A,sizeof(char),_get_zero_arr_size(_tmp37A,41));});goto
_LL22E;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp37C=({struct _tuple11 _tmp37B;
_tmp37B.f1=_tmp317;_tmp37B.f2=_tmp30D;_tmp37B;});struct Cyc_Absyn_VarargInfo*
_tmp37D;struct Cyc_Absyn_VarargInfo*_tmp37E;struct Cyc_Absyn_VarargInfo*_tmp37F;
struct Cyc_Absyn_VarargInfo*_tmp380;struct Cyc_Absyn_VarargInfo*_tmp381;struct Cyc_Absyn_VarargInfo
_tmp382;struct Cyc_Core_Opt*_tmp383;struct Cyc_Absyn_Tqual _tmp384;void*_tmp385;int
_tmp386;struct Cyc_Absyn_VarargInfo*_tmp387;struct Cyc_Absyn_VarargInfo _tmp388;
struct Cyc_Core_Opt*_tmp389;struct Cyc_Absyn_Tqual _tmp38A;void*_tmp38B;int _tmp38C;
_LL273: _tmp37D=_tmp37C.f1;if(_tmp37D != 0)goto _LL275;_tmp37E=_tmp37C.f2;if(
_tmp37E != 0)goto _LL275;_LL274: goto _LL272;_LL275: _tmp37F=_tmp37C.f1;if(_tmp37F != 
0)goto _LL277;_LL276: goto _LL278;_LL277: _tmp380=_tmp37C.f2;if(_tmp380 != 0)goto
_LL279;_LL278: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp38D="(only one function type takes varargs)";
_tag_dynforward(_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,39));});goto
_LL272;_LL279: _tmp381=_tmp37C.f1;if(_tmp381 == 0)goto _LL272;_tmp382=*_tmp381;
_tmp383=_tmp382.name;_tmp384=_tmp382.tq;_tmp385=(void*)_tmp382.type;_tmp386=
_tmp382.inject;_tmp387=_tmp37C.f2;if(_tmp387 == 0)goto _LL272;_tmp388=*_tmp387;
_tmp389=_tmp388.name;_tmp38A=_tmp388.tq;_tmp38B=(void*)_tmp388.type;_tmp38C=
_tmp388.inject;_LL27A: Cyc_Tcutil_unify_it(_tmp385,_tmp38B);Cyc_Tcutil_unify_tqual(
_tmp384,_tmp385,_tmp38A,_tmp38B);if(_tmp386 != _tmp38C){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({
const char*_tmp38E="(only one function type injects varargs)";_tag_dynforward(
_tmp38E,sizeof(char),_get_zero_arr_size(_tmp38E,41));});}goto _LL272;_LL272:;}if(
bad_cyc_vararg)goto _LL22E;{int bad_effect=0;{struct _tuple12 _tmp390=({struct
_tuple12 _tmp38F;_tmp38F.f1=_tmp313;_tmp38F.f2=_tmp309;_tmp38F;});struct Cyc_Core_Opt*
_tmp391;struct Cyc_Core_Opt*_tmp392;struct Cyc_Core_Opt*_tmp393;struct Cyc_Core_Opt*
_tmp394;_LL27C: _tmp391=_tmp390.f1;if(_tmp391 != 0)goto _LL27E;_tmp392=_tmp390.f2;
if(_tmp392 != 0)goto _LL27E;_LL27D: goto _LL27B;_LL27E: _tmp393=_tmp390.f1;if(_tmp393
!= 0)goto _LL280;_LL27F: goto _LL281;_LL280: _tmp394=_tmp390.f2;if(_tmp394 != 0)goto
_LL282;_LL281: bad_effect=1;goto _LL27B;_LL282:;_LL283: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp313))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp309))->v);goto _LL27B;_LL27B:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp395="(function type effects do not unify)";
_tag_dynforward(_tmp395,sizeof(char),_get_zero_arr_size(_tmp395,37));});goto
_LL22E;}if(!Cyc_Tcutil_same_atts(_tmp30F,_tmp319)){Cyc_Tcutil_failure_reason=({
const char*_tmp396="(function types have different attributes)";_tag_dynforward(
_tmp396,sizeof(char),_get_zero_arr_size(_tmp396,43));});goto _LL22E;}if(!Cyc_Tcutil_same_rgn_po(
_tmp30E,_tmp318)){Cyc_Tcutil_failure_reason=({const char*_tmp397="(function types have different region lifetime orderings)";
_tag_dynforward(_tmp397,sizeof(char),_get_zero_arr_size(_tmp397,58));});goto
_LL22E;}return;}}}_LL251: _tmp31A=_tmp29E.f1;if(_tmp31A <= (void*)4)goto _LL253;if(*((
int*)_tmp31A)!= 9)goto _LL253;_tmp31B=((struct Cyc_Absyn_TupleType_struct*)_tmp31A)->f1;
_tmp31C=_tmp29E.f2;if(_tmp31C <= (void*)4)goto _LL253;if(*((int*)_tmp31C)!= 9)goto
_LL253;_tmp31D=((struct Cyc_Absyn_TupleType_struct*)_tmp31C)->f1;_LL252: for(0;
_tmp31D != 0  && _tmp31B != 0;(_tmp31D=_tmp31D->tl,_tmp31B=_tmp31B->tl)){struct Cyc_Absyn_Tqual
_tmp399;void*_tmp39A;struct _tuple5 _tmp398=*((struct _tuple5*)_tmp31D->hd);_tmp399=
_tmp398.f1;_tmp39A=_tmp398.f2;{struct Cyc_Absyn_Tqual _tmp39C;void*_tmp39D;struct
_tuple5 _tmp39B=*((struct _tuple5*)_tmp31B->hd);_tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;
Cyc_Tcutil_unify_it(_tmp39A,_tmp39D);Cyc_Tcutil_unify_tqual(_tmp399,_tmp39A,
_tmp39C,_tmp39D);}}if(_tmp31D == 0  && _tmp31B == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp39E="(tuple types have different numbers of components)";
_tag_dynforward(_tmp39E,sizeof(char),_get_zero_arr_size(_tmp39E,51));});goto
_LL22E;_LL253: _tmp31E=_tmp29E.f1;if(_tmp31E <= (void*)4)goto _LL255;if(*((int*)
_tmp31E)!= 11)goto _LL255;_tmp31F=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp31E)->f1;_tmp320=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp31E)->f2;_tmp321=
_tmp29E.f2;if(_tmp321 <= (void*)4)goto _LL255;if(*((int*)_tmp321)!= 11)goto _LL255;
_tmp322=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp321)->f1;_tmp323=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp321)->f2;_LL254: if(_tmp322 != _tmp31F){
Cyc_Tcutil_failure_reason=({const char*_tmp39F="(struct and union type)";
_tag_dynforward(_tmp39F,sizeof(char),_get_zero_arr_size(_tmp39F,24));});goto
_LL22E;}for(0;_tmp323 != 0  && _tmp320 != 0;(_tmp323=_tmp323->tl,_tmp320=_tmp320->tl)){
struct Cyc_Absyn_Aggrfield*_tmp3A0=(struct Cyc_Absyn_Aggrfield*)_tmp323->hd;struct
Cyc_Absyn_Aggrfield*_tmp3A1=(struct Cyc_Absyn_Aggrfield*)_tmp320->hd;if(Cyc_strptrcmp(
_tmp3A0->name,_tmp3A1->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp3A2="(different member names)";
_tag_dynforward(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp3A0->type,(void*)
_tmp3A1->type);Cyc_Tcutil_unify_tqual(_tmp3A0->tq,(void*)_tmp3A0->type,_tmp3A1->tq,(
void*)_tmp3A1->type);if(!Cyc_Tcutil_same_atts(_tmp3A0->attributes,_tmp3A1->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp3A3="(different attributes on member)";_tag_dynforward(_tmp3A3,
sizeof(char),_get_zero_arr_size(_tmp3A3,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp3A0->width != 0  && _tmp3A1->width == 0  || _tmp3A1->width != 0  && _tmp3A0->width
== 0) || (_tmp3A0->width != 0  && _tmp3A1->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp3A0->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp3A1->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3A4="(different bitfield widths on member)";
_tag_dynforward(_tmp3A4,sizeof(char),_get_zero_arr_size(_tmp3A4,38));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp323 == 0  && _tmp320 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp3A5="(different number of members)";
_tag_dynforward(_tmp3A5,sizeof(char),_get_zero_arr_size(_tmp3A5,30));});goto
_LL22E;_LL255: _tmp324=_tmp29E.f1;if((int)_tmp324 != 2)goto _LL257;_tmp325=_tmp29E.f2;
if((int)_tmp325 != 2)goto _LL257;_LL256: return;_LL257: _tmp326=_tmp29E.f1;if((int)
_tmp326 != 3)goto _LL259;_tmp327=_tmp29E.f2;if((int)_tmp327 != 3)goto _LL259;_LL258:
return;_LL259: _tmp328=_tmp29E.f1;if(_tmp328 <= (void*)4)goto _LL25B;if(*((int*)
_tmp328)!= 15)goto _LL25B;_tmp329=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp328)->f1;_tmp32A=_tmp29E.f2;if(_tmp32A <= (void*)4)goto _LL25B;if(*((int*)
_tmp32A)!= 15)goto _LL25B;_tmp32B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp32A)->f1;_LL25A: Cyc_Tcutil_unify_it(_tmp329,_tmp32B);return;_LL25B: _tmp32C=
_tmp29E.f1;if(_tmp32C <= (void*)4)goto _LL25D;if(*((int*)_tmp32C)!= 16)goto _LL25D;
_tmp32D=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp32C)->f1;_tmp32E=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp32C)->f2;_tmp32F=_tmp29E.f2;if(_tmp32F <= (
void*)4)goto _LL25D;if(*((int*)_tmp32F)!= 16)goto _LL25D;_tmp330=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp32F)->f1;_tmp331=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp32F)->f2;_LL25C: Cyc_Tcutil_unify_it(_tmp32D,_tmp330);Cyc_Tcutil_unify_it(
_tmp32E,_tmp331);return;_LL25D: _tmp332=_tmp29E.f1;if(_tmp332 <= (void*)4)goto
_LL25F;if(*((int*)_tmp332)!= 21)goto _LL25F;_LL25E: goto _LL260;_LL25F: _tmp333=
_tmp29E.f2;if(_tmp333 <= (void*)4)goto _LL261;if(*((int*)_tmp333)!= 21)goto _LL261;
_LL260: goto _LL262;_LL261: _tmp334=_tmp29E.f1;if(_tmp334 <= (void*)4)goto _LL263;if(*((
int*)_tmp334)!= 20)goto _LL263;_LL262: goto _LL264;_LL263: _tmp335=_tmp29E.f1;if(
_tmp335 <= (void*)4)goto _LL265;if(*((int*)_tmp335)!= 22)goto _LL265;_LL264: goto
_LL266;_LL265: _tmp336=_tmp29E.f2;if(_tmp336 <= (void*)4)goto _LL267;if(*((int*)
_tmp336)!= 22)goto _LL267;_LL266: goto _LL268;_LL267: _tmp337=_tmp29E.f2;if(_tmp337
<= (void*)4)goto _LL269;if(*((int*)_tmp337)!= 20)goto _LL269;_LL268: if(Cyc_Tcutil_unify_effect(
t1,t2))return;Cyc_Tcutil_failure_reason=({const char*_tmp3A6="(effects don't unify)";
_tag_dynforward(_tmp3A6,sizeof(char),_get_zero_arr_size(_tmp3A6,22));});goto
_LL22E;_LL269:;_LL26A: goto _LL22E;_LL22E:;}(int)_throw((void*)Cyc_Tcutil_Unify);}
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return
0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)
_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){int _tmp3A7=(tq1.real_const + (tq1.q_volatile << 1))+ (
tq1.q_restrict << 2);int _tmp3A8=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict
<< 2);return Cyc_Core_intcmp(_tmp3A7,_tmp3A8);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint_union
_tmp3A9=x->v;void*_tmp3AA;_LL285: if((_tmp3A9.No_constr).tag != 2)goto _LL287;
_LL286: return - 1;_LL287: if((_tmp3A9.Eq_constr).tag != 0)goto _LL289;_tmp3AA=(
_tmp3A9.Eq_constr).f1;_LL288: {union Cyc_Absyn_Constraint_union _tmp3AB=y->v;void*
_tmp3AC;_LL28C: if((_tmp3AB.No_constr).tag != 2)goto _LL28E;_LL28D: return 1;_LL28E:
if((_tmp3AB.Eq_constr).tag != 0)goto _LL290;_tmp3AC=(_tmp3AB.Eq_constr).f1;_LL28F:
return cmp(_tmp3AA,_tmp3AC);_LL290: if((_tmp3AB.Forward_constr).tag != 1)goto _LL28B;
_LL291:({void*_tmp3AD[0]={};Cyc_Tcutil_impos(({const char*_tmp3AE="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp3AE,sizeof(char),_get_zero_arr_size(_tmp3AE,40));}),
_tag_dynforward(_tmp3AD,sizeof(void*),0));});_LL28B:;}_LL289: if((_tmp3A9.Forward_constr).tag
!= 1)goto _LL284;_LL28A:({void*_tmp3AF[0]={};Cyc_Tcutil_impos(({const char*_tmp3B0="unify_conref: forward after compress";
_tag_dynforward(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,37));}),
_tag_dynforward(_tmp3AF,sizeof(void*),0));});_LL284:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple5*tqt1,struct _tuple5*tqt2){struct _tuple5 _tmp3B2;struct Cyc_Absyn_Tqual
_tmp3B3;void*_tmp3B4;struct _tuple5*_tmp3B1=tqt1;_tmp3B2=*_tmp3B1;_tmp3B3=_tmp3B2.f1;
_tmp3B4=_tmp3B2.f2;{struct _tuple5 _tmp3B6;struct Cyc_Absyn_Tqual _tmp3B7;void*
_tmp3B8;struct _tuple5*_tmp3B5=tqt2;_tmp3B6=*_tmp3B5;_tmp3B7=_tmp3B6.f1;_tmp3B8=
_tmp3B6.f2;{int _tmp3B9=Cyc_Tcutil_tqual_cmp(_tmp3B3,_tmp3B7);if(_tmp3B9 != 0)
return _tmp3B9;return Cyc_Tcutil_typecmp(_tmp3B4,_tmp3B8);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp3BA=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp3BA != 0)return _tmp3BA;{int _tmp3BB=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp3BB != 0)return _tmp3BB;{int _tmp3BC=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp3BC != 0)return _tmp3BC;{int _tmp3BD=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp3BD != 0)return
_tmp3BD;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3BE=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3BE != 0)return _tmp3BE;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3BF=t;_LL293: if((int)_tmp3BF != 0)goto _LL295;_LL294: return 0;
_LL295: if(_tmp3BF <= (void*)4)goto _LL2A1;if(*((int*)_tmp3BF)!= 0)goto _LL297;
_LL296: return 1;_LL297: if(*((int*)_tmp3BF)!= 1)goto _LL299;_LL298: return 2;_LL299:
if(*((int*)_tmp3BF)!= 2)goto _LL29B;_LL29A: return 3;_LL29B: if(*((int*)_tmp3BF)!= 3)
goto _LL29D;_LL29C: return 4;_LL29D: if(*((int*)_tmp3BF)!= 4)goto _LL29F;_LL29E:
return 5;_LL29F: if(*((int*)_tmp3BF)!= 5)goto _LL2A1;_LL2A0: return 6;_LL2A1: if((int)
_tmp3BF != 1)goto _LL2A3;_LL2A2: return 7;_LL2A3: if(_tmp3BF <= (void*)4)goto _LL2B7;
if(*((int*)_tmp3BF)!= 6)goto _LL2A5;_LL2A4: return 8;_LL2A5: if(*((int*)_tmp3BF)!= 7)
goto _LL2A7;_LL2A6: return 9;_LL2A7: if(*((int*)_tmp3BF)!= 8)goto _LL2A9;_LL2A8:
return 10;_LL2A9: if(*((int*)_tmp3BF)!= 9)goto _LL2AB;_LL2AA: return 11;_LL2AB: if(*((
int*)_tmp3BF)!= 10)goto _LL2AD;_LL2AC: return 12;_LL2AD: if(*((int*)_tmp3BF)!= 11)
goto _LL2AF;_LL2AE: return 14;_LL2AF: if(*((int*)_tmp3BF)!= 12)goto _LL2B1;_LL2B0:
return 16;_LL2B1: if(*((int*)_tmp3BF)!= 13)goto _LL2B3;_LL2B2: return 17;_LL2B3: if(*((
int*)_tmp3BF)!= 15)goto _LL2B5;_LL2B4: return 18;_LL2B5: if(*((int*)_tmp3BF)!= 17)
goto _LL2B7;_LL2B6: return 19;_LL2B7: if((int)_tmp3BF != 3)goto _LL2B9;_LL2B8: return 20;
_LL2B9: if((int)_tmp3BF != 2)goto _LL2BB;_LL2BA: return 21;_LL2BB: if(_tmp3BF <= (void*)
4)goto _LL2BD;if(*((int*)_tmp3BF)!= 20)goto _LL2BD;_LL2BC: return 22;_LL2BD: if(
_tmp3BF <= (void*)4)goto _LL2BF;if(*((int*)_tmp3BF)!= 21)goto _LL2BF;_LL2BE: return
23;_LL2BF: if(_tmp3BF <= (void*)4)goto _LL2C1;if(*((int*)_tmp3BF)!= 22)goto _LL2C1;
_LL2C0: return 24;_LL2C1: if(_tmp3BF <= (void*)4)goto _LL2C3;if(*((int*)_tmp3BF)!= 14)
goto _LL2C3;_LL2C2: return 25;_LL2C3: if(_tmp3BF <= (void*)4)goto _LL2C5;if(*((int*)
_tmp3BF)!= 19)goto _LL2C5;_LL2C4: return 26;_LL2C5: if(_tmp3BF <= (void*)4)goto _LL2C7;
if(*((int*)_tmp3BF)!= 18)goto _LL2C7;_LL2C6: return 27;_LL2C7: if(_tmp3BF <= (void*)4)
goto _LL292;if(*((int*)_tmp3BF)!= 16)goto _LL292;_LL2C8: return 28;_LL292:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3C0=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3C0 != 0)return _tmp3C0;{struct _tuple0 _tmp3C2=({struct _tuple0 _tmp3C1;
_tmp3C1.f1=t2;_tmp3C1.f2=t1;_tmp3C1;});void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;
struct Cyc_Absyn_Tvar*_tmp3C6;void*_tmp3C7;struct Cyc_Absyn_Tvar*_tmp3C8;void*
_tmp3C9;struct Cyc_Absyn_AggrInfo _tmp3CA;union Cyc_Absyn_AggrInfoU_union _tmp3CB;
struct Cyc_List_List*_tmp3CC;void*_tmp3CD;struct Cyc_Absyn_AggrInfo _tmp3CE;union
Cyc_Absyn_AggrInfoU_union _tmp3CF;struct Cyc_List_List*_tmp3D0;void*_tmp3D1;struct
_tuple2*_tmp3D2;void*_tmp3D3;struct _tuple2*_tmp3D4;void*_tmp3D5;struct Cyc_List_List*
_tmp3D6;void*_tmp3D7;struct Cyc_List_List*_tmp3D8;void*_tmp3D9;struct Cyc_Absyn_TunionInfo
_tmp3DA;union Cyc_Absyn_TunionInfoU_union _tmp3DB;struct Cyc_Absyn_Tuniondecl**
_tmp3DC;struct Cyc_Absyn_Tuniondecl*_tmp3DD;struct Cyc_List_List*_tmp3DE;struct Cyc_Core_Opt*
_tmp3DF;void*_tmp3E0;struct Cyc_Absyn_TunionInfo _tmp3E1;union Cyc_Absyn_TunionInfoU_union
_tmp3E2;struct Cyc_Absyn_Tuniondecl**_tmp3E3;struct Cyc_Absyn_Tuniondecl*_tmp3E4;
struct Cyc_List_List*_tmp3E5;struct Cyc_Core_Opt*_tmp3E6;void*_tmp3E7;struct Cyc_Absyn_TunionFieldInfo
_tmp3E8;union Cyc_Absyn_TunionFieldInfoU_union _tmp3E9;struct Cyc_Absyn_Tuniondecl*
_tmp3EA;struct Cyc_Absyn_Tunionfield*_tmp3EB;struct Cyc_List_List*_tmp3EC;void*
_tmp3ED;struct Cyc_Absyn_TunionFieldInfo _tmp3EE;union Cyc_Absyn_TunionFieldInfoU_union
_tmp3EF;struct Cyc_Absyn_Tuniondecl*_tmp3F0;struct Cyc_Absyn_Tunionfield*_tmp3F1;
struct Cyc_List_List*_tmp3F2;void*_tmp3F3;struct Cyc_Absyn_PtrInfo _tmp3F4;void*
_tmp3F5;struct Cyc_Absyn_Tqual _tmp3F6;struct Cyc_Absyn_PtrAtts _tmp3F7;void*_tmp3F8;
struct Cyc_Absyn_Conref*_tmp3F9;struct Cyc_Absyn_Conref*_tmp3FA;struct Cyc_Absyn_Conref*
_tmp3FB;void*_tmp3FC;struct Cyc_Absyn_PtrInfo _tmp3FD;void*_tmp3FE;struct Cyc_Absyn_Tqual
_tmp3FF;struct Cyc_Absyn_PtrAtts _tmp400;void*_tmp401;struct Cyc_Absyn_Conref*
_tmp402;struct Cyc_Absyn_Conref*_tmp403;struct Cyc_Absyn_Conref*_tmp404;void*
_tmp405;void*_tmp406;void*_tmp407;void*_tmp408;void*_tmp409;void*_tmp40A;void*
_tmp40B;void*_tmp40C;void*_tmp40D;int _tmp40E;void*_tmp40F;int _tmp410;void*
_tmp411;struct Cyc_Absyn_ArrayInfo _tmp412;void*_tmp413;struct Cyc_Absyn_Tqual
_tmp414;struct Cyc_Absyn_Exp*_tmp415;struct Cyc_Absyn_Conref*_tmp416;void*_tmp417;
struct Cyc_Absyn_ArrayInfo _tmp418;void*_tmp419;struct Cyc_Absyn_Tqual _tmp41A;
struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_Absyn_Conref*_tmp41C;void*_tmp41D;struct
Cyc_Absyn_FnInfo _tmp41E;struct Cyc_List_List*_tmp41F;struct Cyc_Core_Opt*_tmp420;
void*_tmp421;struct Cyc_List_List*_tmp422;int _tmp423;struct Cyc_Absyn_VarargInfo*
_tmp424;struct Cyc_List_List*_tmp425;struct Cyc_List_List*_tmp426;void*_tmp427;
struct Cyc_Absyn_FnInfo _tmp428;struct Cyc_List_List*_tmp429;struct Cyc_Core_Opt*
_tmp42A;void*_tmp42B;struct Cyc_List_List*_tmp42C;int _tmp42D;struct Cyc_Absyn_VarargInfo*
_tmp42E;struct Cyc_List_List*_tmp42F;struct Cyc_List_List*_tmp430;void*_tmp431;
struct Cyc_List_List*_tmp432;void*_tmp433;struct Cyc_List_List*_tmp434;void*
_tmp435;void*_tmp436;struct Cyc_List_List*_tmp437;void*_tmp438;void*_tmp439;
struct Cyc_List_List*_tmp43A;void*_tmp43B;void*_tmp43C;void*_tmp43D;void*_tmp43E;
void*_tmp43F;void*_tmp440;void*_tmp441;void*_tmp442;void*_tmp443;void*_tmp444;
void*_tmp445;void*_tmp446;void*_tmp447;void*_tmp448;void*_tmp449;void*_tmp44A;
void*_tmp44B;void*_tmp44C;void*_tmp44D;int _tmp44E;void*_tmp44F;int _tmp450;void*
_tmp451;void*_tmp452;void*_tmp453;void*_tmp454;void*_tmp455;void*_tmp456;_LL2CA:
_tmp3C3=_tmp3C2.f1;if(_tmp3C3 <= (void*)4)goto _LL2CC;if(*((int*)_tmp3C3)!= 0)goto
_LL2CC;_tmp3C4=_tmp3C2.f2;if(_tmp3C4 <= (void*)4)goto _LL2CC;if(*((int*)_tmp3C4)!= 
0)goto _LL2CC;_LL2CB:({void*_tmp457[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp458="typecmp: can only compare closed types";
_tag_dynforward(_tmp458,sizeof(char),_get_zero_arr_size(_tmp458,39));}),
_tag_dynforward(_tmp457,sizeof(void*),0));});_LL2CC: _tmp3C5=_tmp3C2.f1;if(
_tmp3C5 <= (void*)4)goto _LL2CE;if(*((int*)_tmp3C5)!= 1)goto _LL2CE;_tmp3C6=((
struct Cyc_Absyn_VarType_struct*)_tmp3C5)->f1;_tmp3C7=_tmp3C2.f2;if(_tmp3C7 <= (
void*)4)goto _LL2CE;if(*((int*)_tmp3C7)!= 1)goto _LL2CE;_tmp3C8=((struct Cyc_Absyn_VarType_struct*)
_tmp3C7)->f1;_LL2CD: return Cyc_Core_intcmp(*((int*)_check_null(_tmp3C8->identity)),*((
int*)_check_null(_tmp3C6->identity)));_LL2CE: _tmp3C9=_tmp3C2.f1;if(_tmp3C9 <= (
void*)4)goto _LL2D0;if(*((int*)_tmp3C9)!= 10)goto _LL2D0;_tmp3CA=((struct Cyc_Absyn_AggrType_struct*)
_tmp3C9)->f1;_tmp3CB=_tmp3CA.aggr_info;_tmp3CC=_tmp3CA.targs;_tmp3CD=_tmp3C2.f2;
if(_tmp3CD <= (void*)4)goto _LL2D0;if(*((int*)_tmp3CD)!= 10)goto _LL2D0;_tmp3CE=((
struct Cyc_Absyn_AggrType_struct*)_tmp3CD)->f1;_tmp3CF=_tmp3CE.aggr_info;_tmp3D0=
_tmp3CE.targs;_LL2CF: {struct _tuple2*_tmp45A;struct _tuple6 _tmp459=Cyc_Absyn_aggr_kinded_name(
_tmp3CB);_tmp45A=_tmp459.f2;{struct _tuple2*_tmp45C;struct _tuple6 _tmp45B=Cyc_Absyn_aggr_kinded_name(
_tmp3CF);_tmp45C=_tmp45B.f2;{int _tmp45D=Cyc_Absyn_qvar_cmp(_tmp45A,_tmp45C);if(
_tmp45D != 0)return _tmp45D;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3CC,_tmp3D0);}}}}_LL2D0: _tmp3D1=_tmp3C2.f1;if(_tmp3D1 <= (void*)4)goto _LL2D2;
if(*((int*)_tmp3D1)!= 12)goto _LL2D2;_tmp3D2=((struct Cyc_Absyn_EnumType_struct*)
_tmp3D1)->f1;_tmp3D3=_tmp3C2.f2;if(_tmp3D3 <= (void*)4)goto _LL2D2;if(*((int*)
_tmp3D3)!= 12)goto _LL2D2;_tmp3D4=((struct Cyc_Absyn_EnumType_struct*)_tmp3D3)->f1;
_LL2D1: return Cyc_Absyn_qvar_cmp(_tmp3D2,_tmp3D4);_LL2D2: _tmp3D5=_tmp3C2.f1;if(
_tmp3D5 <= (void*)4)goto _LL2D4;if(*((int*)_tmp3D5)!= 13)goto _LL2D4;_tmp3D6=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3D5)->f1;_tmp3D7=_tmp3C2.f2;if(_tmp3D7
<= (void*)4)goto _LL2D4;if(*((int*)_tmp3D7)!= 13)goto _LL2D4;_tmp3D8=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3D7)->f1;_LL2D3: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3D6,_tmp3D8);_LL2D4: _tmp3D9=_tmp3C2.f1;if(_tmp3D9 <= (
void*)4)goto _LL2D6;if(*((int*)_tmp3D9)!= 2)goto _LL2D6;_tmp3DA=((struct Cyc_Absyn_TunionType_struct*)
_tmp3D9)->f1;_tmp3DB=_tmp3DA.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp3D9)->f1).tunion_info).KnownTunion).tag != 1)goto _LL2D6;_tmp3DC=(_tmp3DB.KnownTunion).f1;
_tmp3DD=*_tmp3DC;_tmp3DE=_tmp3DA.targs;_tmp3DF=_tmp3DA.rgn;_tmp3E0=_tmp3C2.f2;
if(_tmp3E0 <= (void*)4)goto _LL2D6;if(*((int*)_tmp3E0)!= 2)goto _LL2D6;_tmp3E1=((
struct Cyc_Absyn_TunionType_struct*)_tmp3E0)->f1;_tmp3E2=_tmp3E1.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3E0)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL2D6;_tmp3E3=(_tmp3E2.KnownTunion).f1;_tmp3E4=*_tmp3E3;_tmp3E5=_tmp3E1.targs;
_tmp3E6=_tmp3E1.rgn;_LL2D5: if(_tmp3E4 == _tmp3DD)return 0;{int _tmp45E=Cyc_Absyn_qvar_cmp(
_tmp3E4->name,_tmp3DD->name);if(_tmp45E != 0)return _tmp45E;if((unsigned int)
_tmp3E6  && (unsigned int)_tmp3DF){int _tmp45F=Cyc_Tcutil_typecmp((void*)_tmp3E6->v,(
void*)_tmp3DF->v);if(_tmp45F != 0)return _tmp45F;}else{if((unsigned int)_tmp3E6)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3E5,
_tmp3DE);}_LL2D6: _tmp3E7=_tmp3C2.f1;if(_tmp3E7 <= (void*)4)goto _LL2D8;if(*((int*)
_tmp3E7)!= 3)goto _LL2D8;_tmp3E8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3E7)->f1;_tmp3E9=_tmp3E8.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3E7)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2D8;_tmp3EA=(_tmp3E9.KnownTunionfield).f1;
_tmp3EB=(_tmp3E9.KnownTunionfield).f2;_tmp3EC=_tmp3E8.targs;_tmp3ED=_tmp3C2.f2;
if(_tmp3ED <= (void*)4)goto _LL2D8;if(*((int*)_tmp3ED)!= 3)goto _LL2D8;_tmp3EE=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3ED)->f1;_tmp3EF=_tmp3EE.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3ED)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL2D8;_tmp3F0=(_tmp3EF.KnownTunionfield).f1;_tmp3F1=(_tmp3EF.KnownTunionfield).f2;
_tmp3F2=_tmp3EE.targs;_LL2D7: if(_tmp3F0 == _tmp3EA)return 0;{int _tmp460=Cyc_Absyn_qvar_cmp(
_tmp3EA->name,_tmp3F0->name);if(_tmp460 != 0)return _tmp460;{int _tmp461=Cyc_Absyn_qvar_cmp(
_tmp3EB->name,_tmp3F1->name);if(_tmp461 != 0)return _tmp461;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3F2,_tmp3EC);}}_LL2D8: _tmp3F3=_tmp3C2.f1;if(_tmp3F3 <= (
void*)4)goto _LL2DA;if(*((int*)_tmp3F3)!= 4)goto _LL2DA;_tmp3F4=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F3)->f1;_tmp3F5=(void*)_tmp3F4.elt_typ;_tmp3F6=_tmp3F4.elt_tq;_tmp3F7=
_tmp3F4.ptr_atts;_tmp3F8=(void*)_tmp3F7.rgn;_tmp3F9=_tmp3F7.nullable;_tmp3FA=
_tmp3F7.bounds;_tmp3FB=_tmp3F7.zero_term;_tmp3FC=_tmp3C2.f2;if(_tmp3FC <= (void*)
4)goto _LL2DA;if(*((int*)_tmp3FC)!= 4)goto _LL2DA;_tmp3FD=((struct Cyc_Absyn_PointerType_struct*)
_tmp3FC)->f1;_tmp3FE=(void*)_tmp3FD.elt_typ;_tmp3FF=_tmp3FD.elt_tq;_tmp400=
_tmp3FD.ptr_atts;_tmp401=(void*)_tmp400.rgn;_tmp402=_tmp400.nullable;_tmp403=
_tmp400.bounds;_tmp404=_tmp400.zero_term;_LL2D9: {int _tmp462=Cyc_Tcutil_typecmp(
_tmp3FE,_tmp3F5);if(_tmp462 != 0)return _tmp462;{int _tmp463=Cyc_Tcutil_typecmp(
_tmp401,_tmp3F8);if(_tmp463 != 0)return _tmp463;{int _tmp464=Cyc_Tcutil_tqual_cmp(
_tmp3FF,_tmp3F6);if(_tmp464 != 0)return _tmp464;{int _tmp465=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp403,_tmp3FA);if(_tmp465 != 0)return _tmp465;{int _tmp466=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp404,_tmp3FB);if(_tmp466 != 0)return _tmp466;{union Cyc_Absyn_Constraint_union
_tmp467=(Cyc_Absyn_compress_conref(_tmp403))->v;void*_tmp468;void*_tmp469;_LL301:
if((_tmp467.Eq_constr).tag != 0)goto _LL303;_tmp468=(_tmp467.Eq_constr).f1;if((int)
_tmp468 != 0)goto _LL303;_LL302: return 0;_LL303: if((_tmp467.Eq_constr).tag != 0)goto
_LL305;_tmp469=(_tmp467.Eq_constr).f1;if((int)_tmp469 != 1)goto _LL305;_LL304:
return 0;_LL305:;_LL306: goto _LL300;_LL300:;}return((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,
_tmp402,_tmp3F9);}}}}}_LL2DA: _tmp405=_tmp3C2.f1;if(_tmp405 <= (void*)4)goto _LL2DC;
if(*((int*)_tmp405)!= 5)goto _LL2DC;_tmp406=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp405)->f1;_tmp407=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp405)->f2;
_tmp408=_tmp3C2.f2;if(_tmp408 <= (void*)4)goto _LL2DC;if(*((int*)_tmp408)!= 5)goto
_LL2DC;_tmp409=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp408)->f1;_tmp40A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp408)->f2;_LL2DB: if(_tmp409 != _tmp406)
return Cyc_Core_intcmp((int)((unsigned int)_tmp409),(int)((unsigned int)_tmp406));
if(_tmp40A != _tmp407)return Cyc_Core_intcmp((int)((unsigned int)_tmp40A),(int)((
unsigned int)_tmp407));return 0;_LL2DC: _tmp40B=_tmp3C2.f1;if((int)_tmp40B != 1)
goto _LL2DE;_tmp40C=_tmp3C2.f2;if((int)_tmp40C != 1)goto _LL2DE;_LL2DD: return 0;
_LL2DE: _tmp40D=_tmp3C2.f1;if(_tmp40D <= (void*)4)goto _LL2E0;if(*((int*)_tmp40D)!= 
6)goto _LL2E0;_tmp40E=((struct Cyc_Absyn_DoubleType_struct*)_tmp40D)->f1;_tmp40F=
_tmp3C2.f2;if(_tmp40F <= (void*)4)goto _LL2E0;if(*((int*)_tmp40F)!= 6)goto _LL2E0;
_tmp410=((struct Cyc_Absyn_DoubleType_struct*)_tmp40F)->f1;_LL2DF: if(_tmp40E == 
_tmp410)return 0;else{if(_tmp40E)return - 1;else{return 1;}}_LL2E0: _tmp411=_tmp3C2.f1;
if(_tmp411 <= (void*)4)goto _LL2E2;if(*((int*)_tmp411)!= 7)goto _LL2E2;_tmp412=((
struct Cyc_Absyn_ArrayType_struct*)_tmp411)->f1;_tmp413=(void*)_tmp412.elt_type;
_tmp414=_tmp412.tq;_tmp415=_tmp412.num_elts;_tmp416=_tmp412.zero_term;_tmp417=
_tmp3C2.f2;if(_tmp417 <= (void*)4)goto _LL2E2;if(*((int*)_tmp417)!= 7)goto _LL2E2;
_tmp418=((struct Cyc_Absyn_ArrayType_struct*)_tmp417)->f1;_tmp419=(void*)_tmp418.elt_type;
_tmp41A=_tmp418.tq;_tmp41B=_tmp418.num_elts;_tmp41C=_tmp418.zero_term;_LL2E1: {
int _tmp46A=Cyc_Tcutil_tqual_cmp(_tmp41A,_tmp414);if(_tmp46A != 0)return _tmp46A;{
int _tmp46B=Cyc_Tcutil_typecmp(_tmp419,_tmp413);if(_tmp46B != 0)return _tmp46B;{int
_tmp46C=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp416,_tmp41C);if(_tmp46C != 0)
return _tmp46C;if(_tmp415 == _tmp41B)return 0;if(_tmp415 == 0  || _tmp41B == 0)({void*
_tmp46D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp46E="missing expression in array index";_tag_dynforward(_tmp46E,
sizeof(char),_get_zero_arr_size(_tmp46E,34));}),_tag_dynforward(_tmp46D,sizeof(
void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp415,_tmp41B);}}}_LL2E2: _tmp41D=_tmp3C2.f1;if(_tmp41D <= (void*)4)goto _LL2E4;
if(*((int*)_tmp41D)!= 8)goto _LL2E4;_tmp41E=((struct Cyc_Absyn_FnType_struct*)
_tmp41D)->f1;_tmp41F=_tmp41E.tvars;_tmp420=_tmp41E.effect;_tmp421=(void*)_tmp41E.ret_typ;
_tmp422=_tmp41E.args;_tmp423=_tmp41E.c_varargs;_tmp424=_tmp41E.cyc_varargs;
_tmp425=_tmp41E.rgn_po;_tmp426=_tmp41E.attributes;_tmp427=_tmp3C2.f2;if(_tmp427
<= (void*)4)goto _LL2E4;if(*((int*)_tmp427)!= 8)goto _LL2E4;_tmp428=((struct Cyc_Absyn_FnType_struct*)
_tmp427)->f1;_tmp429=_tmp428.tvars;_tmp42A=_tmp428.effect;_tmp42B=(void*)_tmp428.ret_typ;
_tmp42C=_tmp428.args;_tmp42D=_tmp428.c_varargs;_tmp42E=_tmp428.cyc_varargs;
_tmp42F=_tmp428.rgn_po;_tmp430=_tmp428.attributes;_LL2E3:({void*_tmp46F[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp470="typecmp: function types not handled";_tag_dynforward(_tmp470,
sizeof(char),_get_zero_arr_size(_tmp470,36));}),_tag_dynforward(_tmp46F,sizeof(
void*),0));});_LL2E4: _tmp431=_tmp3C2.f1;if(_tmp431 <= (void*)4)goto _LL2E6;if(*((
int*)_tmp431)!= 9)goto _LL2E6;_tmp432=((struct Cyc_Absyn_TupleType_struct*)_tmp431)->f1;
_tmp433=_tmp3C2.f2;if(_tmp433 <= (void*)4)goto _LL2E6;if(*((int*)_tmp433)!= 9)goto
_LL2E6;_tmp434=((struct Cyc_Absyn_TupleType_struct*)_tmp433)->f1;_LL2E5: return((
int(*)(int(*cmp)(struct _tuple5*,struct _tuple5*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp434,_tmp432);_LL2E6: _tmp435=
_tmp3C2.f1;if(_tmp435 <= (void*)4)goto _LL2E8;if(*((int*)_tmp435)!= 11)goto _LL2E8;
_tmp436=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp435)->f1;_tmp437=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp435)->f2;_tmp438=_tmp3C2.f2;if(_tmp438
<= (void*)4)goto _LL2E8;if(*((int*)_tmp438)!= 11)goto _LL2E8;_tmp439=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp438)->f1;_tmp43A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp438)->f2;_LL2E7: if(_tmp439 != _tmp436){if(_tmp439 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp43A,_tmp437);_LL2E8: _tmp43B=_tmp3C2.f1;if(_tmp43B <= (void*)4)goto _LL2EA;if(*((
int*)_tmp43B)!= 15)goto _LL2EA;_tmp43C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp43B)->f1;_tmp43D=_tmp3C2.f2;if(_tmp43D <= (void*)4)goto _LL2EA;if(*((int*)
_tmp43D)!= 15)goto _LL2EA;_tmp43E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp43D)->f1;_LL2E9: return Cyc_Tcutil_typecmp(_tmp43C,_tmp43E);_LL2EA: _tmp43F=
_tmp3C2.f1;if(_tmp43F <= (void*)4)goto _LL2EC;if(*((int*)_tmp43F)!= 16)goto _LL2EC;
_tmp440=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp43F)->f1;_tmp441=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp43F)->f2;_tmp442=_tmp3C2.f2;if(_tmp442 <= (
void*)4)goto _LL2EC;if(*((int*)_tmp442)!= 16)goto _LL2EC;_tmp443=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp442)->f1;_tmp444=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp442)->f2;_LL2EB: {int _tmp471=Cyc_Tcutil_typecmp(_tmp440,_tmp443);if(_tmp471
!= 0)return _tmp471;else{return Cyc_Tcutil_typecmp(_tmp441,_tmp444);}}_LL2EC:
_tmp445=_tmp3C2.f1;if(_tmp445 <= (void*)4)goto _LL2EE;if(*((int*)_tmp445)!= 14)
goto _LL2EE;_tmp446=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp445)->f1;
_tmp447=_tmp3C2.f2;if(_tmp447 <= (void*)4)goto _LL2EE;if(*((int*)_tmp447)!= 14)
goto _LL2EE;_tmp448=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp447)->f1;
_LL2ED: return Cyc_Tcutil_typecmp(_tmp446,_tmp448);_LL2EE: _tmp449=_tmp3C2.f1;if(
_tmp449 <= (void*)4)goto _LL2F0;if(*((int*)_tmp449)!= 18)goto _LL2F0;_tmp44A=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp449)->f1;_tmp44B=_tmp3C2.f2;if(_tmp44B <= (
void*)4)goto _LL2F0;if(*((int*)_tmp44B)!= 18)goto _LL2F0;_tmp44C=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp44B)->f1;_LL2EF: return Cyc_Tcutil_typecmp(_tmp44A,
_tmp44C);_LL2F0: _tmp44D=_tmp3C2.f1;if(_tmp44D <= (void*)4)goto _LL2F2;if(*((int*)
_tmp44D)!= 19)goto _LL2F2;_tmp44E=((struct Cyc_Absyn_TypeInt_struct*)_tmp44D)->f1;
_tmp44F=_tmp3C2.f2;if(_tmp44F <= (void*)4)goto _LL2F2;if(*((int*)_tmp44F)!= 19)
goto _LL2F2;_tmp450=((struct Cyc_Absyn_TypeInt_struct*)_tmp44F)->f1;_LL2F1: return
Cyc_Core_intcmp(_tmp44E,_tmp450);_LL2F2: _tmp451=_tmp3C2.f1;if(_tmp451 <= (void*)4)
goto _LL2F4;if(*((int*)_tmp451)!= 21)goto _LL2F4;_LL2F3: goto _LL2F5;_LL2F4: _tmp452=
_tmp3C2.f2;if(_tmp452 <= (void*)4)goto _LL2F6;if(*((int*)_tmp452)!= 21)goto _LL2F6;
_LL2F5: goto _LL2F7;_LL2F6: _tmp453=_tmp3C2.f1;if(_tmp453 <= (void*)4)goto _LL2F8;if(*((
int*)_tmp453)!= 20)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: _tmp454=_tmp3C2.f1;if(
_tmp454 <= (void*)4)goto _LL2FA;if(*((int*)_tmp454)!= 22)goto _LL2FA;_LL2F9: goto
_LL2FB;_LL2FA: _tmp455=_tmp3C2.f2;if(_tmp455 <= (void*)4)goto _LL2FC;if(*((int*)
_tmp455)!= 22)goto _LL2FC;_LL2FB: goto _LL2FD;_LL2FC: _tmp456=_tmp3C2.f2;if(_tmp456
<= (void*)4)goto _LL2FE;if(*((int*)_tmp456)!= 20)goto _LL2FE;_LL2FD:({void*_tmp472[
0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp473="typecmp: effects not handled";_tag_dynforward(_tmp473,sizeof(
char),_get_zero_arr_size(_tmp473,29));}),_tag_dynforward(_tmp472,sizeof(void*),0));});
_LL2FE:;_LL2FF:({void*_tmp474[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp475="Unmatched case in typecmp";
_tag_dynforward(_tmp475,sizeof(char),_get_zero_arr_size(_tmp475,26));}),
_tag_dynforward(_tmp474,sizeof(void*),0));});_LL2C9:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp476=Cyc_Tcutil_compress(t);_LL308: if(_tmp476 <= (void*)4)goto
_LL30A;if(*((int*)_tmp476)!= 5)goto _LL30A;_LL309: goto _LL30B;_LL30A: if((int)
_tmp476 != 1)goto _LL30C;_LL30B: goto _LL30D;_LL30C: if(_tmp476 <= (void*)4)goto _LL312;
if(*((int*)_tmp476)!= 6)goto _LL30E;_LL30D: goto _LL30F;_LL30E: if(*((int*)_tmp476)
!= 12)goto _LL310;_LL30F: goto _LL311;_LL310: if(*((int*)_tmp476)!= 13)goto _LL312;
_LL311: return 1;_LL312:;_LL313: return 0;_LL307:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp478=({struct _tuple0 _tmp477;_tmp477.f1=t1;_tmp477.f2=t2;_tmp477;});
void*_tmp479;int _tmp47A;void*_tmp47B;int _tmp47C;void*_tmp47D;void*_tmp47E;void*
_tmp47F;void*_tmp480;void*_tmp481;void*_tmp482;void*_tmp483;void*_tmp484;void*
_tmp485;void*_tmp486;void*_tmp487;void*_tmp488;void*_tmp489;void*_tmp48A;void*
_tmp48B;void*_tmp48C;void*_tmp48D;void*_tmp48E;void*_tmp48F;void*_tmp490;void*
_tmp491;void*_tmp492;void*_tmp493;void*_tmp494;void*_tmp495;void*_tmp496;void*
_tmp497;void*_tmp498;void*_tmp499;void*_tmp49A;void*_tmp49B;void*_tmp49C;void*
_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;void*_tmp4A1;void*_tmp4A2;void*
_tmp4A3;void*_tmp4A4;void*_tmp4A5;void*_tmp4A6;void*_tmp4A7;void*_tmp4A8;void*
_tmp4A9;void*_tmp4AA;void*_tmp4AB;void*_tmp4AC;void*_tmp4AD;void*_tmp4AE;void*
_tmp4AF;void*_tmp4B0;void*_tmp4B1;void*_tmp4B2;void*_tmp4B3;void*_tmp4B4;void*
_tmp4B5;void*_tmp4B6;void*_tmp4B7;void*_tmp4B8;void*_tmp4B9;void*_tmp4BA;void*
_tmp4BB;void*_tmp4BC;void*_tmp4BD;void*_tmp4BE;_LL315: _tmp479=_tmp478.f1;if(
_tmp479 <= (void*)4)goto _LL317;if(*((int*)_tmp479)!= 6)goto _LL317;_tmp47A=((
struct Cyc_Absyn_DoubleType_struct*)_tmp479)->f1;_tmp47B=_tmp478.f2;if(_tmp47B <= (
void*)4)goto _LL317;if(*((int*)_tmp47B)!= 6)goto _LL317;_tmp47C=((struct Cyc_Absyn_DoubleType_struct*)
_tmp47B)->f1;_LL316: return !_tmp47C  && _tmp47A;_LL317: _tmp47D=_tmp478.f1;if(
_tmp47D <= (void*)4)goto _LL319;if(*((int*)_tmp47D)!= 6)goto _LL319;_tmp47E=_tmp478.f2;
if((int)_tmp47E != 1)goto _LL319;_LL318: goto _LL31A;_LL319: _tmp47F=_tmp478.f1;if(
_tmp47F <= (void*)4)goto _LL31B;if(*((int*)_tmp47F)!= 6)goto _LL31B;_tmp480=_tmp478.f2;
if(_tmp480 <= (void*)4)goto _LL31B;if(*((int*)_tmp480)!= 5)goto _LL31B;_LL31A: goto
_LL31C;_LL31B: _tmp481=_tmp478.f1;if(_tmp481 <= (void*)4)goto _LL31D;if(*((int*)
_tmp481)!= 6)goto _LL31D;_tmp482=_tmp478.f2;if(_tmp482 <= (void*)4)goto _LL31D;if(*((
int*)_tmp482)!= 14)goto _LL31D;_LL31C: goto _LL31E;_LL31D: _tmp483=_tmp478.f1;if((
int)_tmp483 != 1)goto _LL31F;_tmp484=_tmp478.f2;if(_tmp484 <= (void*)4)goto _LL31F;
if(*((int*)_tmp484)!= 14)goto _LL31F;_LL31E: goto _LL320;_LL31F: _tmp485=_tmp478.f1;
if(_tmp485 <= (void*)4)goto _LL321;if(*((int*)_tmp485)!= 6)goto _LL321;_tmp486=
_tmp478.f2;if(_tmp486 <= (void*)4)goto _LL321;if(*((int*)_tmp486)!= 18)goto _LL321;
_LL320: goto _LL322;_LL321: _tmp487=_tmp478.f1;if((int)_tmp487 != 1)goto _LL323;
_tmp488=_tmp478.f2;if(_tmp488 <= (void*)4)goto _LL323;if(*((int*)_tmp488)!= 18)
goto _LL323;_LL322: goto _LL324;_LL323: _tmp489=_tmp478.f1;if((int)_tmp489 != 1)goto
_LL325;_tmp48A=_tmp478.f2;if(_tmp48A <= (void*)4)goto _LL325;if(*((int*)_tmp48A)!= 
5)goto _LL325;_LL324: return 1;_LL325: _tmp48B=_tmp478.f1;if(_tmp48B <= (void*)4)goto
_LL327;if(*((int*)_tmp48B)!= 5)goto _LL327;_tmp48C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48B)->f2;if((int)_tmp48C != 4)goto _LL327;_tmp48D=_tmp478.f2;if(_tmp48D <= (
void*)4)goto _LL327;if(*((int*)_tmp48D)!= 5)goto _LL327;_tmp48E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48D)->f2;if((int)_tmp48E != 4)goto _LL327;_LL326: return 0;_LL327: _tmp48F=
_tmp478.f1;if(_tmp48F <= (void*)4)goto _LL329;if(*((int*)_tmp48F)!= 5)goto _LL329;
_tmp490=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48F)->f2;if((int)_tmp490 != 
4)goto _LL329;_LL328: return 1;_LL329: _tmp491=_tmp478.f1;if(_tmp491 <= (void*)4)goto
_LL32B;if(*((int*)_tmp491)!= 5)goto _LL32B;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp491)->f2;if((int)_tmp492 != 3)goto _LL32B;_tmp493=_tmp478.f2;if(_tmp493 <= (
void*)4)goto _LL32B;if(*((int*)_tmp493)!= 5)goto _LL32B;_tmp494=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp493)->f2;if((int)_tmp494 != 2)goto _LL32B;_LL32A: goto _LL32C;_LL32B: _tmp495=
_tmp478.f1;if(_tmp495 <= (void*)4)goto _LL32D;if(*((int*)_tmp495)!= 5)goto _LL32D;
_tmp496=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp495)->f2;if((int)_tmp496 != 
2)goto _LL32D;_tmp497=_tmp478.f2;if(_tmp497 <= (void*)4)goto _LL32D;if(*((int*)
_tmp497)!= 5)goto _LL32D;_tmp498=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp497)->f2;
if((int)_tmp498 != 3)goto _LL32D;_LL32C: return 0;_LL32D: _tmp499=_tmp478.f1;if(
_tmp499 <= (void*)4)goto _LL32F;if(*((int*)_tmp499)!= 5)goto _LL32F;_tmp49A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp499)->f2;if((int)_tmp49A != 3)goto _LL32F;
_tmp49B=_tmp478.f2;if((int)_tmp49B != 1)goto _LL32F;_LL32E: goto _LL330;_LL32F:
_tmp49C=_tmp478.f1;if(_tmp49C <= (void*)4)goto _LL331;if(*((int*)_tmp49C)!= 5)goto
_LL331;_tmp49D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49C)->f2;if((int)
_tmp49D != 2)goto _LL331;_tmp49E=_tmp478.f2;if((int)_tmp49E != 1)goto _LL331;_LL330:
goto _LL332;_LL331: _tmp49F=_tmp478.f1;if(_tmp49F <= (void*)4)goto _LL333;if(*((int*)
_tmp49F)!= 5)goto _LL333;_tmp4A0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49F)->f2;
if((int)_tmp4A0 != 3)goto _LL333;_tmp4A1=_tmp478.f2;if(_tmp4A1 <= (void*)4)goto
_LL333;if(*((int*)_tmp4A1)!= 5)goto _LL333;_tmp4A2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A1)->f2;if((int)_tmp4A2 != 1)goto _LL333;_LL332: goto _LL334;_LL333: _tmp4A3=
_tmp478.f1;if(_tmp4A3 <= (void*)4)goto _LL335;if(*((int*)_tmp4A3)!= 5)goto _LL335;
_tmp4A4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A3)->f2;if((int)_tmp4A4 != 
2)goto _LL335;_tmp4A5=_tmp478.f2;if(_tmp4A5 <= (void*)4)goto _LL335;if(*((int*)
_tmp4A5)!= 5)goto _LL335;_tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A5)->f2;
if((int)_tmp4A6 != 1)goto _LL335;_LL334: goto _LL336;_LL335: _tmp4A7=_tmp478.f1;if(
_tmp4A7 <= (void*)4)goto _LL337;if(*((int*)_tmp4A7)!= 18)goto _LL337;_tmp4A8=
_tmp478.f2;if(_tmp4A8 <= (void*)4)goto _LL337;if(*((int*)_tmp4A8)!= 5)goto _LL337;
_tmp4A9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A8)->f2;if((int)_tmp4A9 != 
1)goto _LL337;_LL336: goto _LL338;_LL337: _tmp4AA=_tmp478.f1;if(_tmp4AA <= (void*)4)
goto _LL339;if(*((int*)_tmp4AA)!= 14)goto _LL339;_tmp4AB=_tmp478.f2;if(_tmp4AB <= (
void*)4)goto _LL339;if(*((int*)_tmp4AB)!= 5)goto _LL339;_tmp4AC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AB)->f2;if((int)_tmp4AC != 1)goto _LL339;_LL338: goto _LL33A;_LL339: _tmp4AD=
_tmp478.f1;if(_tmp4AD <= (void*)4)goto _LL33B;if(*((int*)_tmp4AD)!= 5)goto _LL33B;
_tmp4AE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AD)->f2;if((int)_tmp4AE != 
3)goto _LL33B;_tmp4AF=_tmp478.f2;if(_tmp4AF <= (void*)4)goto _LL33B;if(*((int*)
_tmp4AF)!= 5)goto _LL33B;_tmp4B0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AF)->f2;
if((int)_tmp4B0 != 0)goto _LL33B;_LL33A: goto _LL33C;_LL33B: _tmp4B1=_tmp478.f1;if(
_tmp4B1 <= (void*)4)goto _LL33D;if(*((int*)_tmp4B1)!= 5)goto _LL33D;_tmp4B2=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4B1)->f2;if((int)_tmp4B2 != 2)goto _LL33D;
_tmp4B3=_tmp478.f2;if(_tmp4B3 <= (void*)4)goto _LL33D;if(*((int*)_tmp4B3)!= 5)goto
_LL33D;_tmp4B4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B3)->f2;if((int)
_tmp4B4 != 0)goto _LL33D;_LL33C: goto _LL33E;_LL33D: _tmp4B5=_tmp478.f1;if(_tmp4B5 <= (
void*)4)goto _LL33F;if(*((int*)_tmp4B5)!= 5)goto _LL33F;_tmp4B6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B5)->f2;if((int)_tmp4B6 != 1)goto _LL33F;_tmp4B7=_tmp478.f2;if(_tmp4B7 <= (
void*)4)goto _LL33F;if(*((int*)_tmp4B7)!= 5)goto _LL33F;_tmp4B8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B7)->f2;if((int)_tmp4B8 != 0)goto _LL33F;_LL33E: goto _LL340;_LL33F: _tmp4B9=
_tmp478.f1;if(_tmp4B9 <= (void*)4)goto _LL341;if(*((int*)_tmp4B9)!= 18)goto _LL341;
_tmp4BA=_tmp478.f2;if(_tmp4BA <= (void*)4)goto _LL341;if(*((int*)_tmp4BA)!= 5)goto
_LL341;_tmp4BB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BA)->f2;if((int)
_tmp4BB != 0)goto _LL341;_LL340: goto _LL342;_LL341: _tmp4BC=_tmp478.f1;if(_tmp4BC <= (
void*)4)goto _LL343;if(*((int*)_tmp4BC)!= 14)goto _LL343;_tmp4BD=_tmp478.f2;if(
_tmp4BD <= (void*)4)goto _LL343;if(*((int*)_tmp4BD)!= 5)goto _LL343;_tmp4BE=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4BD)->f2;if((int)_tmp4BE != 0)goto _LL343;
_LL342: return 1;_LL343:;_LL344: return 0;_LL314:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){{struct _RegionHandle _tmp4BF=
_new_region("r");struct _RegionHandle*r=& _tmp4BF;_push_region(r);{struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp4C0=
_region_malloc(r,sizeof(*_tmp4C0));_tmp4C0->v=(void*)t1;_tmp4C0;});}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){int _tmp4C1=
0;_npop_handler(0);return _tmp4C1;}}};_pop_region(r);}{struct Cyc_List_List*el=es;
for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,
t)){({struct Cyc_String_pa_struct _tmp4C5;_tmp4C5.tag=0;_tmp4C5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp4C4;_tmp4C4.tag=0;_tmp4C4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp4C2[2]={& _tmp4C4,& _tmp4C5};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp4C3="type mismatch: expecting %s but found %s";
_tag_dynforward(_tmp4C3,sizeof(char),_get_zero_arr_size(_tmp4C3,41));}),
_tag_dynforward(_tmp4C2,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp4C6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL346: if(_tmp4C6 <= (void*)4)goto _LL348;if(*((int*)_tmp4C6)!= 4)goto _LL348;
_LL347: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL345;
_LL348:;_LL349: return 0;_LL345:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp4C7=Cyc_Tcutil_compress(t);_LL34B: if(_tmp4C7 <= (void*)4)goto _LL355;if(*((
int*)_tmp4C7)!= 5)goto _LL34D;_LL34C: goto _LL34E;_LL34D: if(*((int*)_tmp4C7)!= 14)
goto _LL34F;_LL34E: goto _LL350;_LL34F: if(*((int*)_tmp4C7)!= 18)goto _LL351;_LL350:
goto _LL352;_LL351: if(*((int*)_tmp4C7)!= 12)goto _LL353;_LL352: goto _LL354;_LL353:
if(*((int*)_tmp4C7)!= 13)goto _LL355;_LL354: return 1;_LL355:;_LL356: return 0;_LL34A:;}
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*_tmp4C8[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4C9="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4C9,sizeof(char),_get_zero_arr_size(_tmp4C9,44));}),
_tag_dynforward(_tmp4C8,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp4CA[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp4CB="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4CB,sizeof(char),_get_zero_arr_size(_tmp4CB,44));}),
_tag_dynforward(_tmp4CA,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union f1;
union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp4CD=({struct _tuple0 _tmp4CC;_tmp4CC.f1=
t1;_tmp4CC.f2=t2;_tmp4CC;});void*_tmp4CE;struct Cyc_Absyn_PtrInfo _tmp4CF;void*
_tmp4D0;struct Cyc_Absyn_PtrInfo _tmp4D1;void*_tmp4D2;struct Cyc_Absyn_TunionInfo
_tmp4D3;union Cyc_Absyn_TunionInfoU_union _tmp4D4;struct Cyc_Absyn_Tuniondecl**
_tmp4D5;struct Cyc_Absyn_Tuniondecl*_tmp4D6;struct Cyc_List_List*_tmp4D7;struct Cyc_Core_Opt*
_tmp4D8;struct Cyc_Core_Opt _tmp4D9;void*_tmp4DA;void*_tmp4DB;struct Cyc_Absyn_TunionInfo
_tmp4DC;union Cyc_Absyn_TunionInfoU_union _tmp4DD;struct Cyc_Absyn_Tuniondecl**
_tmp4DE;struct Cyc_Absyn_Tuniondecl*_tmp4DF;struct Cyc_List_List*_tmp4E0;struct Cyc_Core_Opt*
_tmp4E1;struct Cyc_Core_Opt _tmp4E2;void*_tmp4E3;void*_tmp4E4;struct Cyc_Absyn_ArrayInfo
_tmp4E5;void*_tmp4E6;struct Cyc_Absyn_Tqual _tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;
struct Cyc_Absyn_Conref*_tmp4E9;void*_tmp4EA;struct Cyc_Absyn_ArrayInfo _tmp4EB;
void*_tmp4EC;struct Cyc_Absyn_Tqual _tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Conref*
_tmp4EF;void*_tmp4F0;struct Cyc_Absyn_TunionFieldInfo _tmp4F1;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4F2;struct Cyc_Absyn_Tuniondecl*_tmp4F3;struct Cyc_Absyn_Tunionfield*_tmp4F4;
struct Cyc_List_List*_tmp4F5;void*_tmp4F6;struct Cyc_Absyn_TunionInfo _tmp4F7;union
Cyc_Absyn_TunionInfoU_union _tmp4F8;struct Cyc_Absyn_Tuniondecl**_tmp4F9;struct Cyc_Absyn_Tuniondecl*
_tmp4FA;struct Cyc_List_List*_tmp4FB;void*_tmp4FC;struct Cyc_Absyn_PtrInfo _tmp4FD;
void*_tmp4FE;struct Cyc_Absyn_Tqual _tmp4FF;struct Cyc_Absyn_PtrAtts _tmp500;void*
_tmp501;struct Cyc_Absyn_Conref*_tmp502;struct Cyc_Absyn_Conref*_tmp503;struct Cyc_Absyn_Conref*
_tmp504;void*_tmp505;struct Cyc_Absyn_TunionInfo _tmp506;union Cyc_Absyn_TunionInfoU_union
_tmp507;struct Cyc_Absyn_Tuniondecl**_tmp508;struct Cyc_Absyn_Tuniondecl*_tmp509;
struct Cyc_List_List*_tmp50A;struct Cyc_Core_Opt*_tmp50B;void*_tmp50C;void*_tmp50D;
void*_tmp50E;void*_tmp50F;void*_tmp510;void*_tmp511;void*_tmp512;void*_tmp513;
_LL358: _tmp4CE=_tmp4CD.f1;if(_tmp4CE <= (void*)4)goto _LL35A;if(*((int*)_tmp4CE)!= 
4)goto _LL35A;_tmp4CF=((struct Cyc_Absyn_PointerType_struct*)_tmp4CE)->f1;_tmp4D0=
_tmp4CD.f2;if(_tmp4D0 <= (void*)4)goto _LL35A;if(*((int*)_tmp4D0)!= 4)goto _LL35A;
_tmp4D1=((struct Cyc_Absyn_PointerType_struct*)_tmp4D0)->f1;_LL359: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4CF.ptr_atts).nullable,(
_tmp4D1.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp514=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp4CF.ptr_atts).nullable))->v;
int _tmp515;_LL36B: if((_tmp514.Eq_constr).tag != 0)goto _LL36D;_tmp515=(_tmp514.Eq_constr).f1;
_LL36C: okay=!_tmp515;goto _LL36A;_LL36D:;_LL36E:({void*_tmp516[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp517="silent_castable conref not eq";_tag_dynforward(_tmp517,sizeof(char),
_get_zero_arr_size(_tmp517,30));}),_tag_dynforward(_tmp516,sizeof(void*),0));});
_LL36A:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp4CF.ptr_atts).bounds,(
_tmp4D1.ptr_atts).bounds)){struct _tuple13 _tmp519=({struct _tuple13 _tmp518;_tmp518.f1=(
Cyc_Absyn_compress_conref((_tmp4CF.ptr_atts).bounds))->v;_tmp518.f2=(Cyc_Absyn_compress_conref((
_tmp4D1.ptr_atts).bounds))->v;_tmp518;});union Cyc_Absyn_Constraint_union _tmp51A;
void*_tmp51B;union Cyc_Absyn_Constraint_union _tmp51C;void*_tmp51D;union Cyc_Absyn_Constraint_union
_tmp51E;_LL370: _tmp51A=_tmp519.f1;if(((_tmp519.f1).Eq_constr).tag != 0)goto _LL372;
_tmp51B=(_tmp51A.Eq_constr).f1;_tmp51C=_tmp519.f2;if(((_tmp519.f2).Eq_constr).tag
!= 0)goto _LL372;_tmp51D=(_tmp51C.Eq_constr).f1;_LL371:{struct _tuple0 _tmp520=({
struct _tuple0 _tmp51F;_tmp51F.f1=_tmp51B;_tmp51F.f2=_tmp51D;_tmp51F;});void*
_tmp521;void*_tmp522;void*_tmp523;void*_tmp524;void*_tmp525;void*_tmp526;void*
_tmp527;void*_tmp528;void*_tmp529;void*_tmp52A;void*_tmp52B;void*_tmp52C;void*
_tmp52D;struct Cyc_Absyn_Exp*_tmp52E;void*_tmp52F;struct Cyc_Absyn_Exp*_tmp530;
void*_tmp531;void*_tmp532;struct Cyc_Absyn_Exp*_tmp533;void*_tmp534;void*_tmp535;
struct Cyc_Absyn_Exp*_tmp536;void*_tmp537;void*_tmp538;void*_tmp539;void*_tmp53A;
void*_tmp53B;struct Cyc_Absyn_Exp*_tmp53C;void*_tmp53D;void*_tmp53E;void*_tmp53F;
void*_tmp540;_LL377: _tmp521=_tmp520.f1;if(_tmp521 <= (void*)2)goto _LL379;if(*((
int*)_tmp521)!= 0)goto _LL379;_tmp522=_tmp520.f2;if((int)_tmp522 != 0)goto _LL379;
_LL378: goto _LL37A;_LL379: _tmp523=_tmp520.f1;if(_tmp523 <= (void*)2)goto _LL37B;if(*((
int*)_tmp523)!= 0)goto _LL37B;_tmp524=_tmp520.f2;if((int)_tmp524 != 1)goto _LL37B;
_LL37A: goto _LL37C;_LL37B: _tmp525=_tmp520.f1;if((int)_tmp525 != 1)goto _LL37D;
_tmp526=_tmp520.f2;if((int)_tmp526 != 1)goto _LL37D;_LL37C: goto _LL37E;_LL37D:
_tmp527=_tmp520.f1;if((int)_tmp527 != 0)goto _LL37F;_tmp528=_tmp520.f2;if((int)
_tmp528 != 1)goto _LL37F;_LL37E: goto _LL380;_LL37F: _tmp529=_tmp520.f1;if((int)
_tmp529 != 0)goto _LL381;_tmp52A=_tmp520.f2;if((int)_tmp52A != 0)goto _LL381;_LL380:
okay=1;goto _LL376;_LL381: _tmp52B=_tmp520.f1;if((int)_tmp52B != 1)goto _LL383;
_tmp52C=_tmp520.f2;if((int)_tmp52C != 0)goto _LL383;_LL382: okay=0;goto _LL376;
_LL383: _tmp52D=_tmp520.f1;if(_tmp52D <= (void*)2)goto _LL385;if(*((int*)_tmp52D)!= 
0)goto _LL385;_tmp52E=((struct Cyc_Absyn_Upper_b_struct*)_tmp52D)->f1;_tmp52F=
_tmp520.f2;if(_tmp52F <= (void*)2)goto _LL385;if(*((int*)_tmp52F)!= 0)goto _LL385;
_tmp530=((struct Cyc_Absyn_Upper_b_struct*)_tmp52F)->f1;_LL384: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp530,_tmp52E);if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4D1.ptr_atts).zero_term))({void*_tmp541[0]={};Cyc_Tcutil_warn(loc,({const
char*_tmp542="implicit cast to shorter array";_tag_dynforward(_tmp542,sizeof(
char),_get_zero_arr_size(_tmp542,31));}),_tag_dynforward(_tmp541,sizeof(void*),0));});
goto _LL376;_LL385: _tmp531=_tmp520.f1;if((int)_tmp531 != 0)goto _LL387;_tmp532=
_tmp520.f2;if(_tmp532 <= (void*)2)goto _LL387;if(*((int*)_tmp532)!= 0)goto _LL387;
_tmp533=((struct Cyc_Absyn_Upper_b_struct*)_tmp532)->f1;_LL386: _tmp536=_tmp533;
goto _LL388;_LL387: _tmp534=_tmp520.f1;if((int)_tmp534 != 1)goto _LL389;_tmp535=
_tmp520.f2;if(_tmp535 <= (void*)2)goto _LL389;if(*((int*)_tmp535)!= 0)goto _LL389;
_tmp536=((struct Cyc_Absyn_Upper_b_struct*)_tmp535)->f1;_LL388: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4CF.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4D1.ptr_atts).bounds))goto _LL376;okay=0;goto
_LL376;_LL389: _tmp537=_tmp520.f1;if(_tmp537 <= (void*)2)goto _LL38B;if(*((int*)
_tmp537)!= 1)goto _LL38B;_tmp538=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp537)->f1;_tmp539=_tmp520.f2;if(_tmp539 <= (void*)2)goto _LL38B;if(*((int*)
_tmp539)!= 1)goto _LL38B;_tmp53A=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp539)->f1;_LL38A: if(!Cyc_Tcutil_unify(_tmp538,_tmp53A)){struct _tuple0 _tmp544=({
struct _tuple0 _tmp543;_tmp543.f1=Cyc_Tcutil_compress(_tmp538);_tmp543.f2=Cyc_Tcutil_compress(
_tmp53A);_tmp543;});void*_tmp545;int _tmp546;void*_tmp547;int _tmp548;_LL392:
_tmp545=_tmp544.f1;if(_tmp545 <= (void*)4)goto _LL394;if(*((int*)_tmp545)!= 19)
goto _LL394;_tmp546=((struct Cyc_Absyn_TypeInt_struct*)_tmp545)->f1;_tmp547=
_tmp544.f2;if(_tmp547 <= (void*)4)goto _LL394;if(*((int*)_tmp547)!= 19)goto _LL394;
_tmp548=((struct Cyc_Absyn_TypeInt_struct*)_tmp547)->f1;_LL393: if(_tmp546 < 
_tmp548)okay=0;goto _LL391;_LL394:;_LL395: okay=0;goto _LL391;_LL391:;}goto _LL376;
_LL38B: _tmp53B=_tmp520.f1;if(_tmp53B <= (void*)2)goto _LL38D;if(*((int*)_tmp53B)!= 
0)goto _LL38D;_tmp53C=((struct Cyc_Absyn_Upper_b_struct*)_tmp53B)->f1;_tmp53D=
_tmp520.f2;if(_tmp53D <= (void*)2)goto _LL38D;if(*((int*)_tmp53D)!= 1)goto _LL38D;
_tmp53E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp53D)->f1;_LL38C: {
unsigned int _tmp54A;int _tmp54B;struct _tuple7 _tmp549=Cyc_Evexp_eval_const_uint_exp(
_tmp53C);_tmp54A=_tmp549.f1;_tmp54B=_tmp549.f2;if(!_tmp54B){okay=0;goto _LL376;}{
void*_tmp54C=Cyc_Tcutil_compress(_tmp53E);int _tmp54D;_LL397: if(_tmp54C <= (void*)
4)goto _LL399;if(*((int*)_tmp54C)!= 19)goto _LL399;_tmp54D=((struct Cyc_Absyn_TypeInt_struct*)
_tmp54C)->f1;_LL398: if(_tmp54A < _tmp54D)okay=0;goto _LL396;_LL399:;_LL39A: okay=0;
goto _LL396;_LL396:;}goto _LL376;}_LL38D: _tmp53F=_tmp520.f1;if(_tmp53F <= (void*)2)
goto _LL38F;if(*((int*)_tmp53F)!= 1)goto _LL38F;_LL38E: goto _LL390;_LL38F: _tmp540=
_tmp520.f2;if(_tmp540 <= (void*)2)goto _LL376;if(*((int*)_tmp540)!= 1)goto _LL376;
_LL390: okay=0;goto _LL376;_LL376:;}goto _LL36F;_LL372: _tmp51E=_tmp519.f1;if(((
_tmp519.f1).No_constr).tag != 2)goto _LL374;_LL373: okay=0;goto _LL36F;_LL374:;
_LL375:({void*_tmp54E[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp54F="silent_castable conrefs didn't unify";
_tag_dynforward(_tmp54F,sizeof(char),_get_zero_arr_size(_tmp54F,37));}),
_tag_dynforward(_tmp54E,sizeof(void*),0));});_LL36F:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp4CF.elt_typ,(void*)_tmp4D1.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp4CF.ptr_atts).rgn,(void*)(_tmp4D1.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp4CF.ptr_atts).rgn,(void*)(_tmp4D1.ptr_atts).rgn));okay=okay  && (!(
_tmp4CF.elt_tq).real_const  || (_tmp4D1.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp4CF.ptr_atts).zero_term,(_tmp4D1.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp4CF.ptr_atts).zero_term)
 && (_tmp4D1.elt_tq).real_const);return okay;}_LL35A: _tmp4D2=_tmp4CD.f1;if(
_tmp4D2 <= (void*)4)goto _LL35C;if(*((int*)_tmp4D2)!= 2)goto _LL35C;_tmp4D3=((
struct Cyc_Absyn_TunionType_struct*)_tmp4D2)->f1;_tmp4D4=_tmp4D3.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp4D2)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL35C;_tmp4D5=(_tmp4D4.KnownTunion).f1;_tmp4D6=*_tmp4D5;_tmp4D7=_tmp4D3.targs;
_tmp4D8=_tmp4D3.rgn;if(_tmp4D8 == 0)goto _LL35C;_tmp4D9=*_tmp4D8;_tmp4DA=(void*)
_tmp4D9.v;_tmp4DB=_tmp4CD.f2;if(_tmp4DB <= (void*)4)goto _LL35C;if(*((int*)_tmp4DB)
!= 2)goto _LL35C;_tmp4DC=((struct Cyc_Absyn_TunionType_struct*)_tmp4DB)->f1;
_tmp4DD=_tmp4DC.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp4DB)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL35C;_tmp4DE=(_tmp4DD.KnownTunion).f1;_tmp4DF=*_tmp4DE;_tmp4E0=
_tmp4DC.targs;_tmp4E1=_tmp4DC.rgn;if(_tmp4E1 == 0)goto _LL35C;_tmp4E2=*_tmp4E1;
_tmp4E3=(void*)_tmp4E2.v;_LL35B: if(_tmp4D6 != _tmp4DF  || !Cyc_Tcenv_region_outlives(
te,_tmp4DA,_tmp4E3))return 0;for(0;_tmp4D7 != 0  && _tmp4E0 != 0;(_tmp4D7=_tmp4D7->tl,
_tmp4E0=_tmp4E0->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4D7->hd,(void*)_tmp4E0->hd))
return 0;}if(_tmp4D7 != 0  || _tmp4E0 != 0)return 0;return 1;_LL35C: _tmp4E4=_tmp4CD.f1;
if(_tmp4E4 <= (void*)4)goto _LL35E;if(*((int*)_tmp4E4)!= 7)goto _LL35E;_tmp4E5=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4E4)->f1;_tmp4E6=(void*)_tmp4E5.elt_type;
_tmp4E7=_tmp4E5.tq;_tmp4E8=_tmp4E5.num_elts;_tmp4E9=_tmp4E5.zero_term;_tmp4EA=
_tmp4CD.f2;if(_tmp4EA <= (void*)4)goto _LL35E;if(*((int*)_tmp4EA)!= 7)goto _LL35E;
_tmp4EB=((struct Cyc_Absyn_ArrayType_struct*)_tmp4EA)->f1;_tmp4EC=(void*)_tmp4EB.elt_type;
_tmp4ED=_tmp4EB.tq;_tmp4EE=_tmp4EB.num_elts;_tmp4EF=_tmp4EB.zero_term;_LL35D: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4E9,_tmp4EF) && ((_tmp4E8 != 0
 && _tmp4EE != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4E8,(
struct Cyc_Absyn_Exp*)_tmp4EE));return(okay  && Cyc_Tcutil_unify(_tmp4E6,_tmp4EC))
 && (!_tmp4E7.real_const  || _tmp4ED.real_const);}_LL35E: _tmp4F0=_tmp4CD.f1;if(
_tmp4F0 <= (void*)4)goto _LL360;if(*((int*)_tmp4F0)!= 3)goto _LL360;_tmp4F1=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4F0)->f1;_tmp4F2=_tmp4F1.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4F0)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL360;_tmp4F3=(_tmp4F2.KnownTunionfield).f1;_tmp4F4=(_tmp4F2.KnownTunionfield).f2;
_tmp4F5=_tmp4F1.targs;_tmp4F6=_tmp4CD.f2;if(_tmp4F6 <= (void*)4)goto _LL360;if(*((
int*)_tmp4F6)!= 2)goto _LL360;_tmp4F7=((struct Cyc_Absyn_TunionType_struct*)
_tmp4F6)->f1;_tmp4F8=_tmp4F7.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4F6)->f1).tunion_info).KnownTunion).tag != 1)goto _LL360;_tmp4F9=(_tmp4F8.KnownTunion).f1;
_tmp4FA=*_tmp4F9;_tmp4FB=_tmp4F7.targs;_LL35F: if((_tmp4F3 == _tmp4FA  || Cyc_Absyn_qvar_cmp(
_tmp4F3->name,_tmp4FA->name)== 0) && (_tmp4F4->typs == 0  || _tmp4F3->is_flat)){
for(0;_tmp4F5 != 0  && _tmp4FB != 0;(_tmp4F5=_tmp4F5->tl,_tmp4FB=_tmp4FB->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4F5->hd,(void*)_tmp4FB->hd))break;}if(_tmp4F5 == 0
 && _tmp4FB == 0)return 1;}return 0;_LL360: _tmp4FC=_tmp4CD.f1;if(_tmp4FC <= (void*)4)
goto _LL362;if(*((int*)_tmp4FC)!= 4)goto _LL362;_tmp4FD=((struct Cyc_Absyn_PointerType_struct*)
_tmp4FC)->f1;_tmp4FE=(void*)_tmp4FD.elt_typ;_tmp4FF=_tmp4FD.elt_tq;_tmp500=
_tmp4FD.ptr_atts;_tmp501=(void*)_tmp500.rgn;_tmp502=_tmp500.nullable;_tmp503=
_tmp500.bounds;_tmp504=_tmp500.zero_term;_tmp505=_tmp4CD.f2;if(_tmp505 <= (void*)
4)goto _LL362;if(*((int*)_tmp505)!= 2)goto _LL362;_tmp506=((struct Cyc_Absyn_TunionType_struct*)
_tmp505)->f1;_tmp507=_tmp506.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp505)->f1).tunion_info).KnownTunion).tag != 1)goto _LL362;_tmp508=(_tmp507.KnownTunion).f1;
_tmp509=*_tmp508;_tmp50A=_tmp506.targs;_tmp50B=_tmp506.rgn;if(!(!_tmp509->is_flat))
goto _LL362;_LL361:{void*_tmp550=Cyc_Tcutil_compress(_tmp4FE);struct Cyc_Absyn_TunionFieldInfo
_tmp551;union Cyc_Absyn_TunionFieldInfoU_union _tmp552;struct Cyc_Absyn_Tuniondecl*
_tmp553;struct Cyc_Absyn_Tunionfield*_tmp554;struct Cyc_List_List*_tmp555;_LL39C:
if(_tmp550 <= (void*)4)goto _LL39E;if(*((int*)_tmp550)!= 3)goto _LL39E;_tmp551=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp550)->f1;_tmp552=_tmp551.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp550)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL39E;_tmp553=(_tmp552.KnownTunionfield).f1;_tmp554=(_tmp552.KnownTunionfield).f2;
_tmp555=_tmp551.targs;_LL39D: if(!Cyc_Tcutil_unify(_tmp501,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp50B))->v) && !Cyc_Tcenv_region_outlives(te,_tmp501,(void*)
_tmp50B->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp502,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp503,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp504,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp509->name,_tmp553->name)== 0  && _tmp554->typs
!= 0){int okay=1;for(0;_tmp555 != 0  && _tmp50A != 0;(_tmp555=_tmp555->tl,_tmp50A=
_tmp50A->tl)){if(!Cyc_Tcutil_unify((void*)_tmp555->hd,(void*)_tmp50A->hd)){okay=
0;break;}}if((!okay  || _tmp555 != 0) || _tmp50A != 0)return 0;return 1;}goto _LL39B;
_LL39E:;_LL39F: goto _LL39B;_LL39B:;}return 0;_LL362: _tmp50C=_tmp4CD.f1;if(_tmp50C
<= (void*)4)goto _LL364;if(*((int*)_tmp50C)!= 14)goto _LL364;_tmp50D=_tmp4CD.f2;
if(_tmp50D <= (void*)4)goto _LL364;if(*((int*)_tmp50D)!= 5)goto _LL364;_tmp50E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp50D)->f2;if((int)_tmp50E != 2)goto
_LL364;_LL363: goto _LL365;_LL364: _tmp50F=_tmp4CD.f1;if(_tmp50F <= (void*)4)goto
_LL366;if(*((int*)_tmp50F)!= 14)goto _LL366;_tmp510=_tmp4CD.f2;if(_tmp510 <= (void*)
4)goto _LL366;if(*((int*)_tmp510)!= 5)goto _LL366;_tmp511=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp510)->f2;if((int)_tmp511 != 3)goto _LL366;_LL365: return 1;_LL366: _tmp512=
_tmp4CD.f1;if(_tmp512 <= (void*)4)goto _LL368;if(*((int*)_tmp512)!= 18)goto _LL368;
_tmp513=_tmp4CD.f2;if(_tmp513 <= (void*)4)goto _LL368;if(*((int*)_tmp513)!= 5)goto
_LL368;_LL367: return 0;_LL368:;_LL369: return Cyc_Tcutil_unify(t1,t2);_LL357:;}}int
Cyc_Tcutil_is_pointer_type(void*t){void*_tmp556=Cyc_Tcutil_compress(t);_LL3A1:
if(_tmp556 <= (void*)4)goto _LL3A3;if(*((int*)_tmp556)!= 4)goto _LL3A3;_LL3A2:
return 1;_LL3A3:;_LL3A4: return 0;_LL3A0:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e){void*_tmp557=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp558;int _tmp559;union Cyc_Absyn_Cnst_union
_tmp55A;char _tmp55B;union Cyc_Absyn_Cnst_union _tmp55C;short _tmp55D;union Cyc_Absyn_Cnst_union
_tmp55E;long long _tmp55F;void*_tmp560;struct Cyc_Absyn_Exp*_tmp561;_LL3A6: if(*((
int*)_tmp557)!= 0)goto _LL3A8;_tmp558=((struct Cyc_Absyn_Const_e_struct*)_tmp557)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp557)->f1).Int_c).tag != 2)goto _LL3A8;
_tmp559=(_tmp558.Int_c).f2;if(_tmp559 != 0)goto _LL3A8;_LL3A7: goto _LL3A9;_LL3A8:
if(*((int*)_tmp557)!= 0)goto _LL3AA;_tmp55A=((struct Cyc_Absyn_Const_e_struct*)
_tmp557)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp557)->f1).Char_c).tag != 
0)goto _LL3AA;_tmp55B=(_tmp55A.Char_c).f2;if(_tmp55B != 0)goto _LL3AA;_LL3A9: goto
_LL3AB;_LL3AA: if(*((int*)_tmp557)!= 0)goto _LL3AC;_tmp55C=((struct Cyc_Absyn_Const_e_struct*)
_tmp557)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp557)->f1).Short_c).tag != 
1)goto _LL3AC;_tmp55D=(_tmp55C.Short_c).f2;if(_tmp55D != 0)goto _LL3AC;_LL3AB: goto
_LL3AD;_LL3AC: if(*((int*)_tmp557)!= 0)goto _LL3AE;_tmp55E=((struct Cyc_Absyn_Const_e_struct*)
_tmp557)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp557)->f1).LongLong_c).tag
!= 3)goto _LL3AE;_tmp55F=(_tmp55E.LongLong_c).f2;if(_tmp55F != 0)goto _LL3AE;_LL3AD:
return 1;_LL3AE: if(*((int*)_tmp557)!= 15)goto _LL3B0;_tmp560=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp557)->f1;_tmp561=((struct Cyc_Absyn_Cast_e_struct*)_tmp557)->f2;_LL3AF: return
Cyc_Tcutil_is_zero(_tmp561) && Cyc_Tcutil_admits_zero(_tmp560);_LL3B0:;_LL3B1:
return 0;_LL3A5:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562[0]=({struct Cyc_Absyn_Const_e_struct
_tmp563;_tmp563.tag=0;_tmp563.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp564;(_tmp564.Null_c).tag=6;_tmp564;});_tmp563;});_tmp562;})));{struct Cyc_List_List*
_tmp565=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp566=({struct Cyc_Absyn_PointerType_struct*_tmp567=_cycalloc(sizeof(*_tmp567));
_tmp567[0]=({struct Cyc_Absyn_PointerType_struct _tmp568;_tmp568.tag=4;_tmp568.f1=({
struct Cyc_Absyn_PtrInfo _tmp569;_tmp569.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp56C=_cycalloc(sizeof(*
_tmp56C));_tmp56C->v=_tmp565;_tmp56C;}));_tmp569.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp569.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp56A;_tmp56A.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp56B=_cycalloc(
sizeof(*_tmp56B));_tmp56B->v=_tmp565;_tmp56B;}));_tmp56A.nullable=Cyc_Absyn_true_conref;
_tmp56A.bounds=Cyc_Absyn_empty_conref();_tmp56A.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp56A.ptrloc=0;_tmp56A;});_tmp569;});_tmp568;});
_tmp567;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)
_tmp566));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}struct _dynforward_ptr
Cyc_Tcutil_coercion2string(void*c){void*_tmp56D=c;_LL3B3: if((int)_tmp56D != 0)
goto _LL3B5;_LL3B4: return({const char*_tmp56E="unknown";_tag_dynforward(_tmp56E,
sizeof(char),_get_zero_arr_size(_tmp56E,8));});_LL3B5: if((int)_tmp56D != 1)goto
_LL3B7;_LL3B6: return({const char*_tmp56F="no coercion";_tag_dynforward(_tmp56F,
sizeof(char),_get_zero_arr_size(_tmp56F,12));});_LL3B7: if((int)_tmp56D != 2)goto
_LL3B9;_LL3B8: return({const char*_tmp570="null check";_tag_dynforward(_tmp570,
sizeof(char),_get_zero_arr_size(_tmp570,11));});_LL3B9: if((int)_tmp56D != 3)goto
_LL3B2;_LL3BA: return({const char*_tmp571="other coercion";_tag_dynforward(_tmp571,
sizeof(char),_get_zero_arr_size(_tmp571,15));});_LL3B2:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp575;_tmp575.tag=0;_tmp575.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp574;_tmp574.tag=0;
_tmp574.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp572[2]={& _tmp574,& _tmp575};Cyc_Tcutil_warn(e->loc,({const char*
_tmp573="integral size mismatch; %s -> %s conversion supplied";_tag_dynforward(
_tmp573,sizeof(char),_get_zero_arr_size(_tmp573,53));}),_tag_dynforward(_tmp572,
sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp579;_tmp579.tag=0;
_tmp579.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp578;_tmp578.tag=0;_tmp578.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp576[2]={& _tmp578,& _tmp579};Cyc_Tcutil_warn(e->loc,({const char*_tmp577="implicit cast from %s to %s";
_tag_dynforward(_tmp577,sizeof(char),_get_zero_arr_size(_tmp577,28));}),
_tag_dynforward(_tmp576,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp57A=Cyc_Tcutil_compress(t);
_LL3BC: if(_tmp57A <= (void*)4)goto _LL3BE;if(*((int*)_tmp57A)!= 5)goto _LL3BE;
_LL3BD: goto _LL3BF;_LL3BE: if((int)_tmp57A != 1)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0:
if(_tmp57A <= (void*)4)goto _LL3C2;if(*((int*)_tmp57A)!= 6)goto _LL3C2;_LL3C1:
return 1;_LL3C2:;_LL3C3: return 0;_LL3BB:;}static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp57C;
struct _RegionHandle*_tmp57D;struct _tuple9 _tmp57B=*env;_tmp57C=_tmp57B.f1;_tmp57D=
_tmp57B.f2;return({struct _tuple5*_tmp57E=_region_malloc(_tmp57D,sizeof(*_tmp57E));
_tmp57E->f1=x->tq;_tmp57E->f2=Cyc_Tcutil_rsubstitute(_tmp57D,_tmp57C,(void*)x->type);
_tmp57E;});}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(struct _RegionHandle*r,
struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp580;void*_tmp581;struct _tuple5 _tmp57F=*
x;_tmp580=_tmp57F.f1;_tmp581=_tmp57F.f2;return({struct _tuple5*_tmp582=
_region_malloc(r,sizeof(*_tmp582));_tmp582->f1=_tmp580;_tmp582->f2=_tmp581;
_tmp582;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp583=t1;
struct Cyc_List_List*_tmp584;struct Cyc_Absyn_AggrInfo _tmp585;union Cyc_Absyn_AggrInfoU_union
_tmp586;struct Cyc_Absyn_Aggrdecl**_tmp587;struct Cyc_Absyn_Aggrdecl*_tmp588;
struct Cyc_List_List*_tmp589;void*_tmp58A;struct Cyc_List_List*_tmp58B;struct Cyc_Absyn_FnInfo
_tmp58C;_LL3C5: if((int)_tmp583 != 0)goto _LL3C7;_LL3C6: return 0;_LL3C7: if(_tmp583 <= (
void*)4)goto _LL3CF;if(*((int*)_tmp583)!= 9)goto _LL3C9;_tmp584=((struct Cyc_Absyn_TupleType_struct*)
_tmp583)->f1;_LL3C8: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _RegionHandle*,struct _tuple5*),struct _RegionHandle*env,struct
Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp584);_LL3C9: if(*((
int*)_tmp583)!= 10)goto _LL3CB;_tmp585=((struct Cyc_Absyn_AggrType_struct*)_tmp583)->f1;
_tmp586=_tmp585.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp583)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL3CB;_tmp587=(_tmp586.KnownAggr).f1;_tmp588=*_tmp587;_tmp589=_tmp585.targs;
_LL3CA: if((((void*)_tmp588->kind == (void*)1  || _tmp588->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp588->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp588->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp58D=
_region_malloc(r,sizeof(*_tmp58D));_tmp58D->hd=({struct _tuple5*_tmp58E=
_region_malloc(r,sizeof(*_tmp58E));_tmp58E->f1=Cyc_Absyn_empty_tqual(0);_tmp58E->f2=
t1;_tmp58E;});_tmp58D->tl=0;_tmp58D;});{struct Cyc_List_List*_tmp58F=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp588->tvs,_tmp589);struct _tuple9 env=({struct _tuple9
_tmp590;_tmp590.f1=_tmp58F;_tmp590.f2=r;_tmp590;});return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),
struct _tuple9*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp588->impl))->fields);}_LL3CB:
if(*((int*)_tmp583)!= 11)goto _LL3CD;_tmp58A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp583)->f1;if((int)_tmp58A != 0)goto _LL3CD;_tmp58B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp583)->f2;_LL3CC: {struct _tuple9 env=({struct _tuple9 _tmp591;_tmp591.f1=0;
_tmp591.f2=r;_tmp591;});return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp58B);}
_LL3CD: if(*((int*)_tmp583)!= 8)goto _LL3CF;_tmp58C=((struct Cyc_Absyn_FnType_struct*)
_tmp583)->f1;_LL3CE: return({struct Cyc_List_List*_tmp592=_region_malloc(r,sizeof(*
_tmp592));_tmp592->hd=({struct _tuple5*_tmp593=_region_malloc(r,sizeof(*_tmp593));
_tmp593->f1=Cyc_Absyn_const_tqual(0);_tmp593->f2=t1;_tmp593;});_tmp592->tl=0;
_tmp592;});_LL3CF:;_LL3D0: return({struct Cyc_List_List*_tmp594=_region_malloc(r,
sizeof(*_tmp594));_tmp594->hd=({struct _tuple5*_tmp595=_region_malloc(r,sizeof(*
_tmp595));_tmp595->f1=Cyc_Absyn_empty_tqual(0);_tmp595->f2=t1;_tmp595;});_tmp594->tl=
0;_tmp594;});_LL3C4:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,
struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*
_tmp596=(void*)t->hd;_LL3D2: if((int)_tmp596 != 16)goto _LL3D4;_LL3D3: goto _LL3D5;
_LL3D4: if((int)_tmp596 != 3)goto _LL3D6;_LL3D5: goto _LL3D7;_LL3D6: if(_tmp596 <= (
void*)17)goto _LL3D8;if(*((int*)_tmp596)!= 4)goto _LL3D8;_LL3D7: continue;_LL3D8:;
_LL3D9: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL3D1:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp598=({struct _tuple0 _tmp597;_tmp597.f1=t1;_tmp597.f2=t2;_tmp597;});void*
_tmp599;struct Cyc_Absyn_PtrInfo _tmp59A;void*_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;
struct Cyc_Absyn_PtrAtts _tmp59D;void*_tmp59E;struct Cyc_Absyn_Conref*_tmp59F;
struct Cyc_Absyn_Conref*_tmp5A0;struct Cyc_Absyn_Conref*_tmp5A1;void*_tmp5A2;
struct Cyc_Absyn_PtrInfo _tmp5A3;void*_tmp5A4;struct Cyc_Absyn_Tqual _tmp5A5;struct
Cyc_Absyn_PtrAtts _tmp5A6;void*_tmp5A7;struct Cyc_Absyn_Conref*_tmp5A8;struct Cyc_Absyn_Conref*
_tmp5A9;struct Cyc_Absyn_Conref*_tmp5AA;void*_tmp5AB;struct Cyc_Absyn_TunionInfo
_tmp5AC;union Cyc_Absyn_TunionInfoU_union _tmp5AD;struct Cyc_Absyn_Tuniondecl**
_tmp5AE;struct Cyc_Absyn_Tuniondecl*_tmp5AF;struct Cyc_List_List*_tmp5B0;struct Cyc_Core_Opt*
_tmp5B1;struct Cyc_Core_Opt _tmp5B2;void*_tmp5B3;void*_tmp5B4;struct Cyc_Absyn_TunionInfo
_tmp5B5;union Cyc_Absyn_TunionInfoU_union _tmp5B6;struct Cyc_Absyn_Tuniondecl**
_tmp5B7;struct Cyc_Absyn_Tuniondecl*_tmp5B8;struct Cyc_List_List*_tmp5B9;struct Cyc_Core_Opt*
_tmp5BA;struct Cyc_Core_Opt _tmp5BB;void*_tmp5BC;void*_tmp5BD;struct Cyc_Absyn_FnInfo
_tmp5BE;void*_tmp5BF;struct Cyc_Absyn_FnInfo _tmp5C0;_LL3DB: _tmp599=_tmp598.f1;if(
_tmp599 <= (void*)4)goto _LL3DD;if(*((int*)_tmp599)!= 4)goto _LL3DD;_tmp59A=((
struct Cyc_Absyn_PointerType_struct*)_tmp599)->f1;_tmp59B=(void*)_tmp59A.elt_typ;
_tmp59C=_tmp59A.elt_tq;_tmp59D=_tmp59A.ptr_atts;_tmp59E=(void*)_tmp59D.rgn;
_tmp59F=_tmp59D.nullable;_tmp5A0=_tmp59D.bounds;_tmp5A1=_tmp59D.zero_term;
_tmp5A2=_tmp598.f2;if(_tmp5A2 <= (void*)4)goto _LL3DD;if(*((int*)_tmp5A2)!= 4)goto
_LL3DD;_tmp5A3=((struct Cyc_Absyn_PointerType_struct*)_tmp5A2)->f1;_tmp5A4=(void*)
_tmp5A3.elt_typ;_tmp5A5=_tmp5A3.elt_tq;_tmp5A6=_tmp5A3.ptr_atts;_tmp5A7=(void*)
_tmp5A6.rgn;_tmp5A8=_tmp5A6.nullable;_tmp5A9=_tmp5A6.bounds;_tmp5AA=_tmp5A6.zero_term;
_LL3DC: if(_tmp59C.real_const  && !_tmp5A5.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp59F,_tmp5A8) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp59F)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5A8))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5A1,
_tmp5AA) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5A1)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5AA))return 0;if(!Cyc_Tcutil_unify(_tmp59E,_tmp5A7) && !Cyc_Tcenv_region_outlives(
te,_tmp59E,_tmp5A7))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp5A0,_tmp5A9)){struct _tuple0 _tmp5C2=({struct _tuple0 _tmp5C1;_tmp5C1.f1=Cyc_Absyn_conref_val(
_tmp5A0);_tmp5C1.f2=Cyc_Absyn_conref_val(_tmp5A9);_tmp5C1;});void*_tmp5C3;void*
_tmp5C4;void*_tmp5C5;void*_tmp5C6;void*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C8;void*
_tmp5C9;struct Cyc_Absyn_Exp*_tmp5CA;_LL3E4: _tmp5C3=_tmp5C2.f1;if(_tmp5C3 <= (void*)
2)goto _LL3E6;if(*((int*)_tmp5C3)!= 0)goto _LL3E6;_tmp5C4=_tmp5C2.f2;if((int)
_tmp5C4 != 0)goto _LL3E6;_LL3E5: goto _LL3E3;_LL3E6: _tmp5C5=_tmp5C2.f1;if(_tmp5C5 <= (
void*)2)goto _LL3E8;if(*((int*)_tmp5C5)!= 0)goto _LL3E8;_tmp5C6=_tmp5C2.f2;if((int)
_tmp5C6 != 1)goto _LL3E8;_LL3E7: goto _LL3E3;_LL3E8: _tmp5C7=_tmp5C2.f1;if(_tmp5C7 <= (
void*)2)goto _LL3EA;if(*((int*)_tmp5C7)!= 0)goto _LL3EA;_tmp5C8=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5C7)->f1;_tmp5C9=_tmp5C2.f2;if(_tmp5C9 <= (void*)2)goto _LL3EA;if(*((int*)
_tmp5C9)!= 0)goto _LL3EA;_tmp5CA=((struct Cyc_Absyn_Upper_b_struct*)_tmp5C9)->f1;
_LL3E9: if(!Cyc_Evexp_lte_const_exp(_tmp5CA,_tmp5C8))return 0;goto _LL3E3;_LL3EA:;
_LL3EB: return 0;_LL3E3:;}return Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*
_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->hd=({struct _tuple0*_tmp5CC=
_cycalloc(sizeof(*_tmp5CC));_tmp5CC->f1=t1;_tmp5CC->f2=t2;_tmp5CC;});_tmp5CB->tl=
assume;_tmp5CB;}),_tmp59B,_tmp5A4);_LL3DD: _tmp5AB=_tmp598.f1;if(_tmp5AB <= (void*)
4)goto _LL3DF;if(*((int*)_tmp5AB)!= 2)goto _LL3DF;_tmp5AC=((struct Cyc_Absyn_TunionType_struct*)
_tmp5AB)->f1;_tmp5AD=_tmp5AC.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp5AB)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3DF;_tmp5AE=(_tmp5AD.KnownTunion).f1;
_tmp5AF=*_tmp5AE;_tmp5B0=_tmp5AC.targs;_tmp5B1=_tmp5AC.rgn;if(_tmp5B1 == 0)goto
_LL3DF;_tmp5B2=*_tmp5B1;_tmp5B3=(void*)_tmp5B2.v;_tmp5B4=_tmp598.f2;if(_tmp5B4 <= (
void*)4)goto _LL3DF;if(*((int*)_tmp5B4)!= 2)goto _LL3DF;_tmp5B5=((struct Cyc_Absyn_TunionType_struct*)
_tmp5B4)->f1;_tmp5B6=_tmp5B5.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp5B4)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3DF;_tmp5B7=(_tmp5B6.KnownTunion).f1;
_tmp5B8=*_tmp5B7;_tmp5B9=_tmp5B5.targs;_tmp5BA=_tmp5B5.rgn;if(_tmp5BA == 0)goto
_LL3DF;_tmp5BB=*_tmp5BA;_tmp5BC=(void*)_tmp5BB.v;_LL3DE: if(_tmp5AF != _tmp5B8  || 
!Cyc_Tcenv_region_outlives(te,_tmp5B3,_tmp5BC))return 0;for(0;_tmp5B0 != 0  && 
_tmp5B9 != 0;(_tmp5B0=_tmp5B0->tl,_tmp5B9=_tmp5B9->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp5B0->hd,(void*)_tmp5B9->hd))return 0;}if(_tmp5B0 != 0  || _tmp5B9 != 0)
return 0;return 1;_LL3DF: _tmp5BD=_tmp598.f1;if(_tmp5BD <= (void*)4)goto _LL3E1;if(*((
int*)_tmp5BD)!= 8)goto _LL3E1;_tmp5BE=((struct Cyc_Absyn_FnType_struct*)_tmp5BD)->f1;
_tmp5BF=_tmp598.f2;if(_tmp5BF <= (void*)4)goto _LL3E1;if(*((int*)_tmp5BF)!= 8)goto
_LL3E1;_tmp5C0=((struct Cyc_Absyn_FnType_struct*)_tmp5BF)->f1;_LL3E0: if(_tmp5BE.tvars
!= 0  || _tmp5C0.tvars != 0){struct Cyc_List_List*_tmp5CD=_tmp5BE.tvars;struct Cyc_List_List*
_tmp5CE=_tmp5C0.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5CD)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5CE))return 0;{
struct _RegionHandle _tmp5CF=_new_region("r");struct _RegionHandle*r=& _tmp5CF;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp5CD != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp5CD->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp5CE))->hd)){int _tmp5D0=0;_npop_handler(0);
return _tmp5D0;}inst=({struct Cyc_List_List*_tmp5D1=_region_malloc(r,sizeof(*
_tmp5D1));_tmp5D1->hd=({struct _tuple8*_tmp5D2=_region_malloc(r,sizeof(*_tmp5D2));
_tmp5D2->f1=(struct Cyc_Absyn_Tvar*)_tmp5CE->hd;_tmp5D2->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3[0]=({struct Cyc_Absyn_VarType_struct
_tmp5D4;_tmp5D4.tag=1;_tmp5D4.f1=(struct Cyc_Absyn_Tvar*)_tmp5CD->hd;_tmp5D4;});
_tmp5D3;});_tmp5D2;});_tmp5D1->tl=inst;_tmp5D1;});_tmp5CD=_tmp5CD->tl;_tmp5CE=
_tmp5CE->tl;}if(inst != 0){_tmp5BE.tvars=0;_tmp5C0.tvars=0;{int _tmp5D9=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp5D5=_cycalloc(sizeof(*
_tmp5D5));_tmp5D5[0]=({struct Cyc_Absyn_FnType_struct _tmp5D6;_tmp5D6.tag=8;
_tmp5D6.f1=_tmp5BE;_tmp5D6;});_tmp5D5;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_Absyn_FnType_struct
_tmp5D8;_tmp5D8.tag=8;_tmp5D8.f1=_tmp5C0;_tmp5D8;});_tmp5D7;}));_npop_handler(0);
return _tmp5D9;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp5BE.ret_typ,(void*)_tmp5C0.ret_typ))return 0;{struct Cyc_List_List*_tmp5DA=
_tmp5BE.args;struct Cyc_List_List*_tmp5DB=_tmp5C0.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp5DA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5DB))return 0;for(0;_tmp5DA != 0;(_tmp5DA=_tmp5DA->tl,_tmp5DB=_tmp5DB->tl)){
struct Cyc_Absyn_Tqual _tmp5DD;void*_tmp5DE;struct _tuple3 _tmp5DC=*((struct _tuple3*)
_tmp5DA->hd);_tmp5DD=_tmp5DC.f2;_tmp5DE=_tmp5DC.f3;{struct Cyc_Absyn_Tqual _tmp5E0;
void*_tmp5E1;struct _tuple3 _tmp5DF=*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp5DB))->hd);_tmp5E0=_tmp5DF.f2;_tmp5E1=_tmp5DF.f3;if(_tmp5E0.real_const
 && !_tmp5DD.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp5E1,_tmp5DE))return
0;}}if(_tmp5BE.c_varargs != _tmp5C0.c_varargs)return 0;if(_tmp5BE.cyc_varargs != 0
 && _tmp5C0.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp5E2=*_tmp5BE.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp5E3=*_tmp5C0.cyc_varargs;if((_tmp5E3.tq).real_const
 && !(_tmp5E2.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp5E3.type,(
void*)_tmp5E2.type))return 0;}else{if(_tmp5BE.cyc_varargs != 0  || _tmp5C0.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5BE.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5C0.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp5BE.rgn_po,_tmp5C0.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp5BE.attributes,_tmp5C0.attributes))return 0;return 1;}_LL3E1:;_LL3E2: return 0;
_LL3DA:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple0 _tmp5E5=({
struct _tuple0 _tmp5E4;_tmp5E4.f1=Cyc_Tcutil_compress(t1);_tmp5E4.f2=Cyc_Tcutil_compress(
t2);_tmp5E4;});void*_tmp5E6;void*_tmp5E7;void*_tmp5E8;void*_tmp5E9;_LL3ED:
_tmp5E6=_tmp5E5.f1;if(_tmp5E6 <= (void*)4)goto _LL3EF;if(*((int*)_tmp5E6)!= 5)goto
_LL3EF;_tmp5E7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E6)->f2;_tmp5E8=
_tmp5E5.f2;if(_tmp5E8 <= (void*)4)goto _LL3EF;if(*((int*)_tmp5E8)!= 5)goto _LL3EF;
_tmp5E9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E8)->f2;_LL3EE: return(
_tmp5E7 == _tmp5E9  || _tmp5E7 == (void*)2  && _tmp5E9 == (void*)3) || _tmp5E7 == (
void*)3  && _tmp5E9 == (void*)2;_LL3EF:;_LL3F0: return 0;_LL3EC:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct
_RegionHandle _tmp5EA=_new_region("temp");struct _RegionHandle*temp=& _tmp5EA;
_push_region(temp);{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,te,t2);for(0;tqs2 != 0;(tqs2=
tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){int _tmp5EB=0;_npop_handler(0);return
_tmp5EB;}{struct _tuple5 _tmp5ED;struct Cyc_Absyn_Tqual _tmp5EE;void*_tmp5EF;struct
_tuple5*_tmp5EC=(struct _tuple5*)tqs1->hd;_tmp5ED=*_tmp5EC;_tmp5EE=_tmp5ED.f1;
_tmp5EF=_tmp5ED.f2;{struct _tuple5 _tmp5F1;struct Cyc_Absyn_Tqual _tmp5F2;void*
_tmp5F3;struct _tuple5*_tmp5F0=(struct _tuple5*)tqs2->hd;_tmp5F1=*_tmp5F0;_tmp5F2=
_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;if(_tmp5F2.real_const  && Cyc_Tcutil_subtype(te,
assume,_tmp5EF,_tmp5F3))continue;else{if(Cyc_Tcutil_unify(_tmp5EF,_tmp5F3))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp5EF,_tmp5F3))continue;else{int _tmp5F4=
0;_npop_handler(0);return _tmp5F4;}}}}}}{int _tmp5F5=1;_npop_handler(0);return
_tmp5F5;}};_pop_region(temp);}static int Cyc_Tcutil_is_char_type(void*t){void*
_tmp5F6=Cyc_Tcutil_compress(t);void*_tmp5F7;_LL3F2: if(_tmp5F6 <= (void*)4)goto
_LL3F4;if(*((int*)_tmp5F6)!= 5)goto _LL3F4;_tmp5F7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F6)->f2;if((int)_tmp5F7 != 0)goto _LL3F4;_LL3F3: return 1;_LL3F4:;_LL3F5: return
0;_LL3F1:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp5F8=t2;
void*_tmp5F9;void*_tmp5FA;_LL3F7: if(_tmp5F8 <= (void*)4)goto _LL3FB;if(*((int*)
_tmp5F8)!= 5)goto _LL3F9;_tmp5F9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F8)->f2;
if((int)_tmp5F9 != 2)goto _LL3F9;_LL3F8: goto _LL3FA;_LL3F9: if(*((int*)_tmp5F8)!= 5)
goto _LL3FB;_tmp5FA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F8)->f2;if((int)
_tmp5FA != 3)goto _LL3FB;_LL3FA: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3F6;_LL3FB:;_LL3FC: goto _LL3F6;_LL3F6:;}{void*_tmp5FB=t1;struct Cyc_Absyn_PtrInfo
_tmp5FC;void*_tmp5FD;struct Cyc_Absyn_Tqual _tmp5FE;struct Cyc_Absyn_PtrAtts _tmp5FF;
void*_tmp600;struct Cyc_Absyn_Conref*_tmp601;struct Cyc_Absyn_Conref*_tmp602;
struct Cyc_Absyn_Conref*_tmp603;struct Cyc_Absyn_ArrayInfo _tmp604;void*_tmp605;
struct Cyc_Absyn_Tqual _tmp606;struct Cyc_Absyn_Exp*_tmp607;struct Cyc_Absyn_Conref*
_tmp608;struct Cyc_Absyn_Enumdecl*_tmp609;_LL3FE: if(_tmp5FB <= (void*)4)goto _LL406;
if(*((int*)_tmp5FB)!= 4)goto _LL400;_tmp5FC=((struct Cyc_Absyn_PointerType_struct*)
_tmp5FB)->f1;_tmp5FD=(void*)_tmp5FC.elt_typ;_tmp5FE=_tmp5FC.elt_tq;_tmp5FF=
_tmp5FC.ptr_atts;_tmp600=(void*)_tmp5FF.rgn;_tmp601=_tmp5FF.nullable;_tmp602=
_tmp5FF.bounds;_tmp603=_tmp5FF.zero_term;_LL3FF:{void*_tmp60A=t2;struct Cyc_Absyn_PtrInfo
_tmp60B;void*_tmp60C;struct Cyc_Absyn_Tqual _tmp60D;struct Cyc_Absyn_PtrAtts _tmp60E;
void*_tmp60F;struct Cyc_Absyn_Conref*_tmp610;struct Cyc_Absyn_Conref*_tmp611;
struct Cyc_Absyn_Conref*_tmp612;_LL40D: if(_tmp60A <= (void*)4)goto _LL40F;if(*((int*)
_tmp60A)!= 4)goto _LL40F;_tmp60B=((struct Cyc_Absyn_PointerType_struct*)_tmp60A)->f1;
_tmp60C=(void*)_tmp60B.elt_typ;_tmp60D=_tmp60B.elt_tq;_tmp60E=_tmp60B.ptr_atts;
_tmp60F=(void*)_tmp60E.rgn;_tmp610=_tmp60E.nullable;_tmp611=_tmp60E.bounds;
_tmp612=_tmp60E.zero_term;_LL40E: {void*coercion=(void*)3;struct Cyc_List_List*
_tmp613=({struct Cyc_List_List*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->hd=({
struct _tuple0*_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623->f1=t1;_tmp623->f2=t2;
_tmp623;});_tmp622->tl=0;_tmp622;});int _tmp614=Cyc_Tcutil_ptrsubtype(te,_tmp613,
_tmp5FD,_tmp60C) && (!_tmp5FE.real_const  || _tmp60D.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp603,_tmp612) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp612);int _tmp615=_tmp614?0:((Cyc_Tcutil_bits_only(_tmp5FD) && Cyc_Tcutil_is_char_type(
_tmp60C)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp612)) && (_tmp60D.real_const  || !_tmp5FE.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp602,_tmp611);if(!bounds_ok  && !_tmp615){struct
_tuple0 _tmp617=({struct _tuple0 _tmp616;_tmp616.f1=Cyc_Absyn_conref_val(_tmp602);
_tmp616.f2=Cyc_Absyn_conref_val(_tmp611);_tmp616;});void*_tmp618;struct Cyc_Absyn_Exp*
_tmp619;void*_tmp61A;struct Cyc_Absyn_Exp*_tmp61B;void*_tmp61C;void*_tmp61D;void*
_tmp61E;void*_tmp61F;void*_tmp620;void*_tmp621;_LL412: _tmp618=_tmp617.f1;if(
_tmp618 <= (void*)2)goto _LL414;if(*((int*)_tmp618)!= 0)goto _LL414;_tmp619=((
struct Cyc_Absyn_Upper_b_struct*)_tmp618)->f1;_tmp61A=_tmp617.f2;if(_tmp61A <= (
void*)2)goto _LL414;if(*((int*)_tmp61A)!= 0)goto _LL414;_tmp61B=((struct Cyc_Absyn_Upper_b_struct*)
_tmp61A)->f1;_LL413: if(Cyc_Evexp_lte_const_exp(_tmp61B,_tmp619))bounds_ok=1;goto
_LL411;_LL414: _tmp61C=_tmp617.f1;if(_tmp61C <= (void*)2)goto _LL416;if(*((int*)
_tmp61C)!= 1)goto _LL416;_tmp61D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp61C)->f1;_tmp61E=_tmp617.f2;if(_tmp61E <= (void*)2)goto _LL416;if(*((int*)
_tmp61E)!= 1)goto _LL416;_tmp61F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp61E)->f1;_LL415: bounds_ok=Cyc_Tcutil_unify(_tmp61D,_tmp61F);goto _LL411;
_LL416: _tmp620=_tmp617.f1;if(_tmp620 <= (void*)2)goto _LL418;if(*((int*)_tmp620)!= 
1)goto _LL418;_LL417: goto _LL419;_LL418: _tmp621=_tmp617.f2;if(_tmp621 <= (void*)2)
goto _LL41A;if(*((int*)_tmp621)!= 1)goto _LL41A;_LL419: bounds_ok=0;goto _LL411;
_LL41A:;_LL41B: bounds_ok=1;goto _LL411;_LL411:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp601) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp610))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp614  || _tmp615)) && (Cyc_Tcutil_unify(_tmp600,_tmp60F) || Cyc_Tcenv_region_outlives(
te,_tmp600,_tmp60F)))return coercion;else{return(void*)0;}}}_LL40F:;_LL410: goto
_LL40C;_LL40C:;}return(void*)0;_LL400: if(*((int*)_tmp5FB)!= 7)goto _LL402;_tmp604=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5FB)->f1;_tmp605=(void*)_tmp604.elt_type;
_tmp606=_tmp604.tq;_tmp607=_tmp604.num_elts;_tmp608=_tmp604.zero_term;_LL401:{
void*_tmp624=t2;struct Cyc_Absyn_ArrayInfo _tmp625;void*_tmp626;struct Cyc_Absyn_Tqual
_tmp627;struct Cyc_Absyn_Exp*_tmp628;struct Cyc_Absyn_Conref*_tmp629;_LL41D: if(
_tmp624 <= (void*)4)goto _LL41F;if(*((int*)_tmp624)!= 7)goto _LL41F;_tmp625=((
struct Cyc_Absyn_ArrayType_struct*)_tmp624)->f1;_tmp626=(void*)_tmp625.elt_type;
_tmp627=_tmp625.tq;_tmp628=_tmp625.num_elts;_tmp629=_tmp625.zero_term;_LL41E: {
int okay;okay=((_tmp607 != 0  && _tmp628 != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp608,_tmp629)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp628,(
struct Cyc_Absyn_Exp*)_tmp607);return(okay  && Cyc_Tcutil_unify(_tmp605,_tmp626))
 && (!_tmp606.real_const  || _tmp627.real_const)?(void*)3:(void*)0;}_LL41F:;
_LL420: return(void*)0;_LL41C:;}return(void*)0;_LL402: if(*((int*)_tmp5FB)!= 12)
goto _LL404;_tmp609=((struct Cyc_Absyn_EnumType_struct*)_tmp5FB)->f2;_LL403:{void*
_tmp62A=t2;struct Cyc_Absyn_Enumdecl*_tmp62B;_LL422: if(_tmp62A <= (void*)4)goto
_LL424;if(*((int*)_tmp62A)!= 12)goto _LL424;_tmp62B=((struct Cyc_Absyn_EnumType_struct*)
_tmp62A)->f2;_LL423: if((_tmp609->fields != 0  && _tmp62B->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp609->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp62B->fields))->v))
return(void*)1;goto _LL421;_LL424:;_LL425: goto _LL421;_LL421:;}goto _LL405;_LL404:
if(*((int*)_tmp5FB)!= 5)goto _LL406;_LL405: goto _LL407;_LL406: if((int)_tmp5FB != 1)
goto _LL408;_LL407: goto _LL409;_LL408: if(_tmp5FB <= (void*)4)goto _LL40A;if(*((int*)
_tmp5FB)!= 6)goto _LL40A;_LL409: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL40A:;_LL40B: return(void*)0;_LL3FD:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp62C=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp62D=_cycalloc(
sizeof(*_tmp62D));_tmp62D[0]=({struct Cyc_Absyn_Cast_e_struct _tmp62E;_tmp62E.tag=
15;_tmp62E.f1=(void*)t;_tmp62E.f2=_tmp62C;_tmp62E.f3=0;_tmp62E.f4=(void*)c;
_tmp62E;});_tmp62D;})));e->topt=({struct Cyc_Core_Opt*_tmp62F=_cycalloc(sizeof(*
_tmp62F));_tmp62F->v=(void*)t;_tmp62F;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp630=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL427: if(_tmp630 <= (void*)4)goto _LL433;if(*((int*)_tmp630)!= 5)goto _LL429;
_LL428: goto _LL42A;_LL429: if(*((int*)_tmp630)!= 12)goto _LL42B;_LL42A: goto _LL42C;
_LL42B: if(*((int*)_tmp630)!= 13)goto _LL42D;_LL42C: goto _LL42E;_LL42D: if(*((int*)
_tmp630)!= 18)goto _LL42F;_LL42E: goto _LL430;_LL42F: if(*((int*)_tmp630)!= 14)goto
_LL431;_LL430: return 1;_LL431: if(*((int*)_tmp630)!= 0)goto _LL433;_LL432: return Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL433:;
_LL434: return 0;_LL426:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp631=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL436: if((int)_tmp631 != 1)goto _LL438;_LL437: goto
_LL439;_LL438: if(_tmp631 <= (void*)4)goto _LL43A;if(*((int*)_tmp631)!= 6)goto
_LL43A;_LL439: return 1;_LL43A:;_LL43B: return 0;_LL435:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp632=Cyc_Tcutil_compress(t);_LL43D: if(_tmp632 <= (void*)4)goto
_LL43F;if(*((int*)_tmp632)!= 8)goto _LL43F;_LL43E: return 1;_LL43F:;_LL440: return 0;
_LL43C:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp634=({struct _tuple0 _tmp633;_tmp633.f1=t1;_tmp633.f2=t2;_tmp633;});void*
_tmp635;int _tmp636;void*_tmp637;int _tmp638;void*_tmp639;void*_tmp63A;void*
_tmp63B;void*_tmp63C;void*_tmp63D;void*_tmp63E;void*_tmp63F;void*_tmp640;void*
_tmp641;void*_tmp642;void*_tmp643;void*_tmp644;void*_tmp645;void*_tmp646;void*
_tmp647;void*_tmp648;void*_tmp649;void*_tmp64A;void*_tmp64B;void*_tmp64C;void*
_tmp64D;void*_tmp64E;void*_tmp64F;void*_tmp650;void*_tmp651;void*_tmp652;void*
_tmp653;void*_tmp654;void*_tmp655;void*_tmp656;_LL442: _tmp635=_tmp634.f1;if(
_tmp635 <= (void*)4)goto _LL444;if(*((int*)_tmp635)!= 6)goto _LL444;_tmp636=((
struct Cyc_Absyn_DoubleType_struct*)_tmp635)->f1;_tmp637=_tmp634.f2;if(_tmp637 <= (
void*)4)goto _LL444;if(*((int*)_tmp637)!= 6)goto _LL444;_tmp638=((struct Cyc_Absyn_DoubleType_struct*)
_tmp637)->f1;_LL443: if(_tmp636)return t1;else{return t2;}_LL444: _tmp639=_tmp634.f1;
if(_tmp639 <= (void*)4)goto _LL446;if(*((int*)_tmp639)!= 6)goto _LL446;_LL445:
return t1;_LL446: _tmp63A=_tmp634.f2;if(_tmp63A <= (void*)4)goto _LL448;if(*((int*)
_tmp63A)!= 6)goto _LL448;_LL447: return t2;_LL448: _tmp63B=_tmp634.f1;if((int)
_tmp63B != 1)goto _LL44A;_LL449: goto _LL44B;_LL44A: _tmp63C=_tmp634.f2;if((int)
_tmp63C != 1)goto _LL44C;_LL44B: return(void*)1;_LL44C: _tmp63D=_tmp634.f1;if(
_tmp63D <= (void*)4)goto _LL44E;if(*((int*)_tmp63D)!= 5)goto _LL44E;_tmp63E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp63D)->f1;if((int)_tmp63E != 1)goto _LL44E;
_tmp63F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp63D)->f2;if((int)_tmp63F != 
4)goto _LL44E;_LL44D: goto _LL44F;_LL44E: _tmp640=_tmp634.f2;if(_tmp640 <= (void*)4)
goto _LL450;if(*((int*)_tmp640)!= 5)goto _LL450;_tmp641=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp640)->f1;if((int)_tmp641 != 1)goto _LL450;_tmp642=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp640)->f2;if((int)_tmp642 != 4)goto _LL450;_LL44F: return Cyc_Absyn_ulonglong_typ;
_LL450: _tmp643=_tmp634.f1;if(_tmp643 <= (void*)4)goto _LL452;if(*((int*)_tmp643)!= 
5)goto _LL452;_tmp644=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp643)->f2;if((
int)_tmp644 != 4)goto _LL452;_LL451: goto _LL453;_LL452: _tmp645=_tmp634.f2;if(
_tmp645 <= (void*)4)goto _LL454;if(*((int*)_tmp645)!= 5)goto _LL454;_tmp646=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp645)->f2;if((int)_tmp646 != 4)goto _LL454;
_LL453: return Cyc_Absyn_slonglong_typ;_LL454: _tmp647=_tmp634.f1;if(_tmp647 <= (
void*)4)goto _LL456;if(*((int*)_tmp647)!= 5)goto _LL456;_tmp648=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp647)->f1;if((int)_tmp648 != 1)goto _LL456;_tmp649=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp647)->f2;if((int)_tmp649 != 3)goto _LL456;_LL455: goto _LL457;_LL456: _tmp64A=
_tmp634.f2;if(_tmp64A <= (void*)4)goto _LL458;if(*((int*)_tmp64A)!= 5)goto _LL458;
_tmp64B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64A)->f1;if((int)_tmp64B != 
1)goto _LL458;_tmp64C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64A)->f2;if((
int)_tmp64C != 3)goto _LL458;_LL457: return Cyc_Absyn_ulong_typ;_LL458: _tmp64D=
_tmp634.f1;if(_tmp64D <= (void*)4)goto _LL45A;if(*((int*)_tmp64D)!= 5)goto _LL45A;
_tmp64E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64D)->f1;if((int)_tmp64E != 
1)goto _LL45A;_tmp64F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64D)->f2;if((
int)_tmp64F != 2)goto _LL45A;_LL459: goto _LL45B;_LL45A: _tmp650=_tmp634.f2;if(
_tmp650 <= (void*)4)goto _LL45C;if(*((int*)_tmp650)!= 5)goto _LL45C;_tmp651=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp650)->f1;if((int)_tmp651 != 1)goto _LL45C;
_tmp652=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp650)->f2;if((int)_tmp652 != 
2)goto _LL45C;_LL45B: return Cyc_Absyn_uint_typ;_LL45C: _tmp653=_tmp634.f1;if(
_tmp653 <= (void*)4)goto _LL45E;if(*((int*)_tmp653)!= 5)goto _LL45E;_tmp654=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp653)->f2;if((int)_tmp654 != 3)goto _LL45E;
_LL45D: goto _LL45F;_LL45E: _tmp655=_tmp634.f2;if(_tmp655 <= (void*)4)goto _LL460;if(*((
int*)_tmp655)!= 5)goto _LL460;_tmp656=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp655)->f2;if((int)_tmp656 != 3)goto _LL460;_LL45F: return Cyc_Absyn_slong_typ;
_LL460:;_LL461: return Cyc_Absyn_sint_typ;_LL441:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp657=(void*)e->r;struct Cyc_Core_Opt*_tmp658;
_LL463: if(*((int*)_tmp657)!= 4)goto _LL465;_tmp658=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp657)->f2;if(_tmp658 != 0)goto _LL465;_LL464:({void*_tmp659[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp65A="assignment in test";_tag_dynforward(_tmp65A,sizeof(
char),_get_zero_arr_size(_tmp65A,19));}),_tag_dynforward(_tmp659,sizeof(void*),0));});
goto _LL462;_LL465:;_LL466: goto _LL462;_LL462:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp65C=({struct _tuple0 _tmp65B;_tmp65B.f1=c1;_tmp65B.f2=c2;_tmp65B;});
void*_tmp65D;void*_tmp65E;void*_tmp65F;void*_tmp660;void*_tmp661;struct Cyc_Core_Opt*
_tmp662;struct Cyc_Core_Opt**_tmp663;void*_tmp664;struct Cyc_Core_Opt*_tmp665;
struct Cyc_Core_Opt**_tmp666;void*_tmp667;struct Cyc_Core_Opt*_tmp668;struct Cyc_Core_Opt**
_tmp669;void*_tmp66A;void*_tmp66B;void*_tmp66C;void*_tmp66D;void*_tmp66E;void*
_tmp66F;struct Cyc_Core_Opt*_tmp670;struct Cyc_Core_Opt**_tmp671;void*_tmp672;void*
_tmp673;struct Cyc_Core_Opt*_tmp674;struct Cyc_Core_Opt**_tmp675;void*_tmp676;void*
_tmp677;struct Cyc_Core_Opt*_tmp678;struct Cyc_Core_Opt**_tmp679;void*_tmp67A;
_LL468: _tmp65D=_tmp65C.f1;if(*((int*)_tmp65D)!= 0)goto _LL46A;_tmp65E=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp65D)->f1;_tmp65F=_tmp65C.f2;if(*((int*)_tmp65F)
!= 0)goto _LL46A;_tmp660=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp65F)->f1;
_LL469: return _tmp65E == _tmp660;_LL46A: _tmp661=_tmp65C.f2;if(*((int*)_tmp661)!= 1)
goto _LL46C;_tmp662=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp661)->f1;_tmp663=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp661)->f1;_LL46B:*
_tmp663=({struct Cyc_Core_Opt*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->v=(
void*)c1;_tmp67B;});return 1;_LL46C: _tmp664=_tmp65C.f1;if(*((int*)_tmp664)!= 1)
goto _LL46E;_tmp665=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp664)->f1;_tmp666=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp664)->f1;_LL46D:*
_tmp666=({struct Cyc_Core_Opt*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->v=(
void*)c2;_tmp67C;});return 1;_LL46E: _tmp667=_tmp65C.f1;if(*((int*)_tmp667)!= 2)
goto _LL470;_tmp668=((struct Cyc_Absyn_Less_kb_struct*)_tmp667)->f1;_tmp669=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp667)->f1;_tmp66A=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp667)->f2;_tmp66B=_tmp65C.f2;if(*((
int*)_tmp66B)!= 0)goto _LL470;_tmp66C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp66B)->f1;_LL46F: if(Cyc_Tcutil_kind_leq(_tmp66C,_tmp66A)){*_tmp669=({struct
Cyc_Core_Opt*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->v=(void*)c2;_tmp67D;});
return 1;}else{return 0;}_LL470: _tmp66D=_tmp65C.f1;if(*((int*)_tmp66D)!= 0)goto
_LL472;_tmp66E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp66D)->f1;_tmp66F=
_tmp65C.f2;if(*((int*)_tmp66F)!= 2)goto _LL472;_tmp670=((struct Cyc_Absyn_Less_kb_struct*)
_tmp66F)->f1;_tmp671=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp66F)->f1;_tmp672=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp66F)->f2;
_LL471: if(Cyc_Tcutil_kind_leq(_tmp66E,_tmp672)){*_tmp671=({struct Cyc_Core_Opt*
_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->v=(void*)c1;_tmp67E;});return 1;}
else{return 0;}_LL472: _tmp673=_tmp65C.f1;if(*((int*)_tmp673)!= 2)goto _LL467;
_tmp674=((struct Cyc_Absyn_Less_kb_struct*)_tmp673)->f1;_tmp675=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp673)->f1;_tmp676=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp673)->f2;_tmp677=_tmp65C.f2;if(*((int*)_tmp677)!= 2)goto _LL467;_tmp678=((
struct Cyc_Absyn_Less_kb_struct*)_tmp677)->f1;_tmp679=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp677)->f1;_tmp67A=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp677)->f2;_LL473: if(Cyc_Tcutil_kind_leq(_tmp676,_tmp67A)){*_tmp679=({struct
Cyc_Core_Opt*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->v=(void*)c1;_tmp67F;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp67A,_tmp676)){*_tmp675=({struct Cyc_Core_Opt*
_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680->v=(void*)c2;_tmp680;});return 1;}
else{return 0;}}_LL467:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp681=_cycalloc_atomic(sizeof(*_tmp681));_tmp681[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp681;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dynforward_ptr s=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp686;_tmp686.tag=1;_tmp686.f1=(
unsigned long)i;{void*_tmp684[1]={& _tmp686};Cyc_aprintf(({const char*_tmp685="#%d";
_tag_dynforward(_tmp685,sizeof(char),_get_zero_arr_size(_tmp685,4));}),
_tag_dynforward(_tmp684,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->name=({struct _dynforward_ptr*
_tmp683=_cycalloc(sizeof(struct _dynforward_ptr)* 1);_tmp683[0]=s;_tmp683;});
_tmp682->identity=0;_tmp682->kind=(void*)k;_tmp682;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dynforward_ptr _tmp687=*t->name;return*((const char*)
_check_dynforward_subscript(_tmp687,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp68A;_tmp68A.tag=0;
_tmp68A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*t->name);{void*
_tmp688[1]={& _tmp68A};Cyc_printf(({const char*_tmp689="%s";_tag_dynforward(
_tmp689,sizeof(char),_get_zero_arr_size(_tmp689,3));}),_tag_dynforward(_tmp688,
sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(t))return;{struct
_dynforward_ptr _tmp68B=Cyc_strconcat(({const char*_tmp690="`";_tag_dynforward(
_tmp690,sizeof(char),_get_zero_arr_size(_tmp690,2));}),(struct _dynforward_ptr)*t->name);({
struct _dynforward_ptr _tmp68C=_dynforward_ptr_plus(_tmp68B,sizeof(char),1);char
_tmp68D=*((char*)_check_dynforward_subscript(_tmp68C,sizeof(char),0));char
_tmp68E='t';if(_get_dynforward_size(_tmp68C,sizeof(char))== 1  && (_tmp68D == '\000'
 && _tmp68E != '\000'))_throw_arraybounds();*((char*)_tmp68C.curr)=_tmp68E;});t->name=({
struct _dynforward_ptr*_tmp68F=_cycalloc(sizeof(struct _dynforward_ptr)* 1);
_tmp68F[0]=(struct _dynforward_ptr)_tmp68B;_tmp68F;});}}struct _tuple14{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(
struct _tuple14*x){return({struct _tuple3*_tmp691=_cycalloc(sizeof(*_tmp691));
_tmp691->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp692=_cycalloc(sizeof(*
_tmp692));_tmp692->v=(*x).f1;_tmp692;});_tmp691->f2=(*x).f2;_tmp691->f3=(*x).f3;
_tmp691;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp693=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp693=({
struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->hd=(void*)((
void*)atts->hd);_tmp694->tl=_tmp693;_tmp694;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({struct Cyc_Absyn_FnType_struct
_tmp696;_tmp696.tag=8;_tmp696.f1=({struct Cyc_Absyn_FnInfo _tmp697;_tmp697.tvars=
fd->tvs;_tmp697.effect=fd->effect;_tmp697.ret_typ=(void*)((void*)fd->ret_type);
_tmp697.args=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp697.c_varargs=
fd->c_varargs;_tmp697.cyc_varargs=fd->cyc_varargs;_tmp697.rgn_po=fd->rgn_po;
_tmp697.attributes=_tmp693;_tmp697;});_tmp696;});_tmp695;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple15{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple5*t){return(*t).f2;}static
struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t){return({struct _tuple5*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->f1=(*pr).f1;_tmp698->f2=t;_tmp698;});}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple17{
struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple3*y){return({struct _tuple17*_tmp699=_region_malloc(
rgn,sizeof(*_tmp699));_tmp699->f1=({struct _tuple16*_tmp69A=_region_malloc(rgn,
sizeof(*_tmp69A));_tmp69A->f1=(*y).f1;_tmp69A->f2=(*y).f2;_tmp69A;});_tmp699->f2=(*
y).f3;_tmp699;});}static struct _tuple3*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp69C;void*_tmp69D;struct _tuple17 _tmp69B=*w;_tmp69C=_tmp69B.f1;
_tmp69D=_tmp69B.f2;{struct Cyc_Core_Opt*_tmp69F;struct Cyc_Absyn_Tqual _tmp6A0;
struct _tuple16 _tmp69E=*_tmp69C;_tmp69F=_tmp69E.f1;_tmp6A0=_tmp69E.f2;return({
struct _tuple3*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->f1=_tmp69F;_tmp6A1->f2=
_tmp6A0;_tmp6A1->f3=_tmp69D;_tmp6A1;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp6A2=
_cycalloc(sizeof(*_tmp6A2));_tmp6A2->name=f->name;_tmp6A2->tq=f->tq;_tmp6A2->type=(
void*)t;_tmp6A2->width=f->width;_tmp6A2->attributes=f->attributes;_tmp6A2;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp6A3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp6A4;struct Cyc_Absyn_AggrInfo _tmp6A5;union Cyc_Absyn_AggrInfoU_union _tmp6A6;
struct Cyc_List_List*_tmp6A7;struct Cyc_Absyn_TunionInfo _tmp6A8;union Cyc_Absyn_TunionInfoU_union
_tmp6A9;struct Cyc_List_List*_tmp6AA;struct Cyc_Core_Opt*_tmp6AB;struct Cyc_Absyn_TunionFieldInfo
_tmp6AC;union Cyc_Absyn_TunionFieldInfoU_union _tmp6AD;struct Cyc_List_List*_tmp6AE;
struct _tuple2*_tmp6AF;struct Cyc_List_List*_tmp6B0;struct Cyc_Absyn_Typedefdecl*
_tmp6B1;void**_tmp6B2;struct Cyc_Absyn_ArrayInfo _tmp6B3;void*_tmp6B4;struct Cyc_Absyn_Tqual
_tmp6B5;struct Cyc_Absyn_Exp*_tmp6B6;struct Cyc_Absyn_Conref*_tmp6B7;struct Cyc_Position_Segment*
_tmp6B8;struct Cyc_Absyn_PtrInfo _tmp6B9;void*_tmp6BA;struct Cyc_Absyn_Tqual _tmp6BB;
struct Cyc_Absyn_PtrAtts _tmp6BC;void*_tmp6BD;struct Cyc_Absyn_Conref*_tmp6BE;
struct Cyc_Absyn_Conref*_tmp6BF;struct Cyc_Absyn_Conref*_tmp6C0;struct Cyc_Absyn_FnInfo
_tmp6C1;struct Cyc_List_List*_tmp6C2;struct Cyc_Core_Opt*_tmp6C3;void*_tmp6C4;
struct Cyc_List_List*_tmp6C5;int _tmp6C6;struct Cyc_Absyn_VarargInfo*_tmp6C7;struct
Cyc_List_List*_tmp6C8;struct Cyc_List_List*_tmp6C9;struct Cyc_List_List*_tmp6CA;
void*_tmp6CB;struct Cyc_List_List*_tmp6CC;struct Cyc_Core_Opt*_tmp6CD;void*_tmp6CE;
void*_tmp6CF;void*_tmp6D0;void*_tmp6D1;void*_tmp6D2;void*_tmp6D3;void*_tmp6D4;
struct Cyc_List_List*_tmp6D5;_LL475: if(_tmp6A3 <= (void*)4)goto _LL499;if(*((int*)
_tmp6A3)!= 1)goto _LL477;_tmp6A4=((struct Cyc_Absyn_VarType_struct*)_tmp6A3)->f1;
_LL476: {struct _handler_cons _tmp6D6;_push_handler(& _tmp6D6);{int _tmp6D8=0;if(
setjmp(_tmp6D6.handler))_tmp6D8=1;if(!_tmp6D8){{void*_tmp6D9=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6A4);_npop_handler(0);return
_tmp6D9;};_pop_handler();}else{void*_tmp6D7=(void*)_exn_thrown;void*_tmp6DB=
_tmp6D7;_LL4AC: if(_tmp6DB != Cyc_Core_Not_found)goto _LL4AE;_LL4AD: return t;_LL4AE:;
_LL4AF:(void)_throw(_tmp6DB);_LL4AB:;}}}_LL477: if(*((int*)_tmp6A3)!= 10)goto
_LL479;_tmp6A5=((struct Cyc_Absyn_AggrType_struct*)_tmp6A3)->f1;_tmp6A6=_tmp6A5.aggr_info;
_tmp6A7=_tmp6A5.targs;_LL478: {struct Cyc_List_List*_tmp6DC=Cyc_Tcutil_substs(rgn,
inst,_tmp6A7);return _tmp6DC == _tmp6A7?t:(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_Absyn_AggrType_struct
_tmp6DE;_tmp6DE.tag=10;_tmp6DE.f1=({struct Cyc_Absyn_AggrInfo _tmp6DF;_tmp6DF.aggr_info=
_tmp6A6;_tmp6DF.targs=_tmp6DC;_tmp6DF;});_tmp6DE;});_tmp6DD;});}_LL479: if(*((int*)
_tmp6A3)!= 2)goto _LL47B;_tmp6A8=((struct Cyc_Absyn_TunionType_struct*)_tmp6A3)->f1;
_tmp6A9=_tmp6A8.tunion_info;_tmp6AA=_tmp6A8.targs;_tmp6AB=_tmp6A8.rgn;_LL47A: {
struct Cyc_List_List*_tmp6E0=Cyc_Tcutil_substs(rgn,inst,_tmp6AA);struct Cyc_Core_Opt*
new_r;if((unsigned int)_tmp6AB){void*_tmp6E1=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6AB->v);if(_tmp6E1 == (void*)_tmp6AB->v)new_r=_tmp6AB;else{new_r=({
struct Cyc_Core_Opt*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->v=(void*)_tmp6E1;
_tmp6E2;});}}else{new_r=_tmp6AB;}return _tmp6E0 == _tmp6AA  && new_r == _tmp6AB?t:(
void*)({struct Cyc_Absyn_TunionType_struct*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));
_tmp6E3[0]=({struct Cyc_Absyn_TunionType_struct _tmp6E4;_tmp6E4.tag=2;_tmp6E4.f1=({
struct Cyc_Absyn_TunionInfo _tmp6E5;_tmp6E5.tunion_info=_tmp6A9;_tmp6E5.targs=
_tmp6E0;_tmp6E5.rgn=new_r;_tmp6E5;});_tmp6E4;});_tmp6E3;});}_LL47B: if(*((int*)
_tmp6A3)!= 3)goto _LL47D;_tmp6AC=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6A3)->f1;_tmp6AD=_tmp6AC.field_info;_tmp6AE=_tmp6AC.targs;_LL47C: {struct Cyc_List_List*
_tmp6E6=Cyc_Tcutil_substs(rgn,inst,_tmp6AE);return _tmp6E6 == _tmp6AE?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));
_tmp6E7[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp6E8;_tmp6E8.tag=3;_tmp6E8.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp6E9;_tmp6E9.field_info=_tmp6AD;_tmp6E9.targs=
_tmp6E6;_tmp6E9;});_tmp6E8;});_tmp6E7;});}_LL47D: if(*((int*)_tmp6A3)!= 17)goto
_LL47F;_tmp6AF=((struct Cyc_Absyn_TypedefType_struct*)_tmp6A3)->f1;_tmp6B0=((
struct Cyc_Absyn_TypedefType_struct*)_tmp6A3)->f2;_tmp6B1=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6A3)->f3;_tmp6B2=((struct Cyc_Absyn_TypedefType_struct*)_tmp6A3)->f4;_LL47E: {
struct Cyc_List_List*_tmp6EA=Cyc_Tcutil_substs(rgn,inst,_tmp6B0);return _tmp6EA == 
_tmp6B0?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp6EB=_cycalloc(sizeof(*
_tmp6EB));_tmp6EB[0]=({struct Cyc_Absyn_TypedefType_struct _tmp6EC;_tmp6EC.tag=17;
_tmp6EC.f1=_tmp6AF;_tmp6EC.f2=_tmp6EA;_tmp6EC.f3=_tmp6B1;_tmp6EC.f4=_tmp6B2;
_tmp6EC;});_tmp6EB;});}_LL47F: if(*((int*)_tmp6A3)!= 7)goto _LL481;_tmp6B3=((
struct Cyc_Absyn_ArrayType_struct*)_tmp6A3)->f1;_tmp6B4=(void*)_tmp6B3.elt_type;
_tmp6B5=_tmp6B3.tq;_tmp6B6=_tmp6B3.num_elts;_tmp6B7=_tmp6B3.zero_term;_tmp6B8=
_tmp6B3.zt_loc;_LL480: {void*_tmp6ED=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B4);
return _tmp6ED == _tmp6B4?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp6EE=
_cycalloc(sizeof(*_tmp6EE));_tmp6EE[0]=({struct Cyc_Absyn_ArrayType_struct _tmp6EF;
_tmp6EF.tag=7;_tmp6EF.f1=({struct Cyc_Absyn_ArrayInfo _tmp6F0;_tmp6F0.elt_type=(
void*)_tmp6ED;_tmp6F0.tq=_tmp6B5;_tmp6F0.num_elts=_tmp6B6;_tmp6F0.zero_term=
_tmp6B7;_tmp6F0.zt_loc=_tmp6B8;_tmp6F0;});_tmp6EF;});_tmp6EE;});}_LL481: if(*((
int*)_tmp6A3)!= 4)goto _LL483;_tmp6B9=((struct Cyc_Absyn_PointerType_struct*)
_tmp6A3)->f1;_tmp6BA=(void*)_tmp6B9.elt_typ;_tmp6BB=_tmp6B9.elt_tq;_tmp6BC=
_tmp6B9.ptr_atts;_tmp6BD=(void*)_tmp6BC.rgn;_tmp6BE=_tmp6BC.nullable;_tmp6BF=
_tmp6BC.bounds;_tmp6C0=_tmp6BC.zero_term;_LL482: {void*_tmp6F1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6BA);void*_tmp6F2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BD);struct
Cyc_Absyn_Conref*_tmp6F3=_tmp6BF;{union Cyc_Absyn_Constraint_union _tmp6F4=(Cyc_Absyn_compress_conref(
_tmp6BF))->v;void*_tmp6F5;void*_tmp6F6;_LL4B1: if((_tmp6F4.Eq_constr).tag != 0)
goto _LL4B3;_tmp6F5=(_tmp6F4.Eq_constr).f1;if(_tmp6F5 <= (void*)2)goto _LL4B3;if(*((
int*)_tmp6F5)!= 1)goto _LL4B3;_tmp6F6=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp6F5)->f1;_LL4B2: {void*_tmp6F7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F6);if(
_tmp6F6 != _tmp6F7)_tmp6F3=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*
_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp6F9;_tmp6F9.tag=1;_tmp6F9.f1=(void*)_tmp6F7;_tmp6F9;});_tmp6F8;}));goto
_LL4B0;}_LL4B3:;_LL4B4: goto _LL4B0;_LL4B0:;}if((_tmp6F1 == _tmp6BA  && _tmp6F2 == 
_tmp6BD) && _tmp6F3 == _tmp6BF)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6FB;_tmp6FB.tag=4;_tmp6FB.f1=({struct Cyc_Absyn_PtrInfo _tmp6FC;_tmp6FC.elt_typ=(
void*)_tmp6F1;_tmp6FC.elt_tq=_tmp6BB;_tmp6FC.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6FD;_tmp6FD.rgn=(void*)_tmp6F2;_tmp6FD.nullable=_tmp6BE;_tmp6FD.bounds=
_tmp6F3;_tmp6FD.zero_term=_tmp6C0;_tmp6FD.ptrloc=0;_tmp6FD;});_tmp6FC;});_tmp6FB;});
_tmp6FA;});}_LL483: if(*((int*)_tmp6A3)!= 8)goto _LL485;_tmp6C1=((struct Cyc_Absyn_FnType_struct*)
_tmp6A3)->f1;_tmp6C2=_tmp6C1.tvars;_tmp6C3=_tmp6C1.effect;_tmp6C4=(void*)_tmp6C1.ret_typ;
_tmp6C5=_tmp6C1.args;_tmp6C6=_tmp6C1.c_varargs;_tmp6C7=_tmp6C1.cyc_varargs;
_tmp6C8=_tmp6C1.rgn_po;_tmp6C9=_tmp6C1.attributes;_LL484:{struct Cyc_List_List*
_tmp6FE=_tmp6C2;for(0;_tmp6FE != 0;_tmp6FE=_tmp6FE->tl){inst=({struct Cyc_List_List*
_tmp6FF=_region_malloc(rgn,sizeof(*_tmp6FF));_tmp6FF->hd=({struct _tuple8*_tmp700=
_region_malloc(rgn,sizeof(*_tmp700));_tmp700->f1=(struct Cyc_Absyn_Tvar*)_tmp6FE->hd;
_tmp700->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp701=_cycalloc(sizeof(*
_tmp701));_tmp701[0]=({struct Cyc_Absyn_VarType_struct _tmp702;_tmp702.tag=1;
_tmp702.f1=(struct Cyc_Absyn_Tvar*)_tmp6FE->hd;_tmp702;});_tmp701;});_tmp700;});
_tmp6FF->tl=inst;_tmp6FF;});}}{struct Cyc_List_List*_tmp704;struct Cyc_List_List*
_tmp705;struct _tuple1 _tmp703=((struct _tuple1(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple3*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp6C5));_tmp704=_tmp703.f1;_tmp705=_tmp703.f2;{struct Cyc_List_List*_tmp706=
Cyc_Tcutil_substs(rgn,inst,_tmp705);struct Cyc_List_List*_tmp707=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp704,_tmp706));struct Cyc_Core_Opt*
eff2;if(_tmp6C3 == 0)eff2=0;else{void*_tmp708=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6C3->v);if(_tmp708 == (void*)_tmp6C3->v)eff2=_tmp6C3;else{eff2=({struct
Cyc_Core_Opt*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709->v=(void*)_tmp708;
_tmp709;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6C7 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp70B;struct Cyc_Absyn_Tqual _tmp70C;void*
_tmp70D;int _tmp70E;struct Cyc_Absyn_VarargInfo _tmp70A=*_tmp6C7;_tmp70B=_tmp70A.name;
_tmp70C=_tmp70A.tq;_tmp70D=(void*)_tmp70A.type;_tmp70E=_tmp70A.inject;{void*
_tmp70F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70D);if(_tmp70F == _tmp70D)
cyc_varargs2=_tmp6C7;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp710=
_cycalloc(sizeof(*_tmp710));_tmp710->name=_tmp70B;_tmp710->tq=_tmp70C;_tmp710->type=(
void*)_tmp70F;_tmp710->inject=_tmp70E;_tmp710;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp712;struct Cyc_List_List*_tmp713;struct _tuple1 _tmp711=Cyc_List_rsplit(
rgn,rgn,_tmp6C8);_tmp712=_tmp711.f1;_tmp713=_tmp711.f2;{struct Cyc_List_List*
_tmp714=Cyc_Tcutil_substs(rgn,inst,_tmp712);struct Cyc_List_List*_tmp715=Cyc_Tcutil_substs(
rgn,inst,_tmp713);if(_tmp714 == _tmp712  && _tmp715 == _tmp713)rgn_po2=_tmp6C8;
else{rgn_po2=Cyc_List_zip(_tmp714,_tmp715);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=({struct Cyc_Absyn_FnType_struct
_tmp717;_tmp717.tag=8;_tmp717.f1=({struct Cyc_Absyn_FnInfo _tmp718;_tmp718.tvars=
_tmp6C2;_tmp718.effect=eff2;_tmp718.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp6C4);_tmp718.args=_tmp707;_tmp718.c_varargs=_tmp6C6;_tmp718.cyc_varargs=
cyc_varargs2;_tmp718.rgn_po=rgn_po2;_tmp718.attributes=_tmp6C9;_tmp718;});
_tmp717;});_tmp716;});}}}}}_LL485: if(*((int*)_tmp6A3)!= 9)goto _LL487;_tmp6CA=((
struct Cyc_Absyn_TupleType_struct*)_tmp6A3)->f1;_LL486: {struct Cyc_List_List*
_tmp719=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6CA);struct Cyc_List_List*
_tmp71A=Cyc_Tcutil_substs(rgn,inst,_tmp719);if(_tmp71A == _tmp719)return t;{struct
Cyc_List_List*_tmp71B=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6CA,_tmp71A);return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp71C=
_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_TupleType_struct _tmp71D;
_tmp71D.tag=9;_tmp71D.f1=_tmp71B;_tmp71D;});_tmp71C;});}}_LL487: if(*((int*)
_tmp6A3)!= 11)goto _LL489;_tmp6CB=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6A3)->f1;_tmp6CC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6A3)->f2;_LL488: {
struct Cyc_List_List*_tmp71E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6CC);struct Cyc_List_List*_tmp71F=Cyc_Tcutil_substs(rgn,inst,_tmp71E);if(
_tmp71F == _tmp71E)return t;{struct Cyc_List_List*_tmp720=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6CC,
_tmp71F);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp721=_cycalloc(
sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp722;
_tmp722.tag=11;_tmp722.f1=(void*)_tmp6CB;_tmp722.f2=_tmp720;_tmp722;});_tmp721;});}}
_LL489: if(*((int*)_tmp6A3)!= 0)goto _LL48B;_tmp6CD=((struct Cyc_Absyn_Evar_struct*)
_tmp6A3)->f2;_LL48A: if(_tmp6CD != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6CD->v);else{return t;}_LL48B: if(*((int*)_tmp6A3)!= 15)goto _LL48D;_tmp6CE=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp6A3)->f1;_LL48C: {void*_tmp723=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6CE);return _tmp723 == _tmp6CE?t:(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp725;_tmp725.tag=15;_tmp725.f1=(void*)
_tmp723;_tmp725;});_tmp724;});}_LL48D: if(*((int*)_tmp6A3)!= 16)goto _LL48F;
_tmp6CF=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp6A3)->f1;_tmp6D0=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp6A3)->f2;_LL48E: {void*_tmp726=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6CF);void*_tmp727=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D0);return
_tmp726 == _tmp6CF  && _tmp727 == _tmp6D0?t:(void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp729;_tmp729.tag=16;_tmp729.f1=(void*)_tmp726;_tmp729.f2=(void*)_tmp727;
_tmp729;});_tmp728;});}_LL48F: if(*((int*)_tmp6A3)!= 14)goto _LL491;_tmp6D1=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp6A3)->f1;_LL490: {void*_tmp72A=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6D1);return _tmp72A == _tmp6D1?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp72C;_tmp72C.tag=14;_tmp72C.f1=(void*)_tmp72A;_tmp72C;});_tmp72B;});}_LL491:
if(*((int*)_tmp6A3)!= 18)goto _LL493;_tmp6D2=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6A3)->f1;_LL492: {void*_tmp72D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D2);
return _tmp72D == _tmp6D2?t:(void*)({struct Cyc_Absyn_TagType_struct*_tmp72E=
_cycalloc(sizeof(*_tmp72E));_tmp72E[0]=({struct Cyc_Absyn_TagType_struct _tmp72F;
_tmp72F.tag=18;_tmp72F.f1=(void*)_tmp72D;_tmp72F;});_tmp72E;});}_LL493: if(*((int*)
_tmp6A3)!= 19)goto _LL495;_LL494: goto _LL496;_LL495: if(*((int*)_tmp6A3)!= 12)goto
_LL497;_LL496: goto _LL498;_LL497: if(*((int*)_tmp6A3)!= 13)goto _LL499;_LL498: goto
_LL49A;_LL499: if((int)_tmp6A3 != 0)goto _LL49B;_LL49A: goto _LL49C;_LL49B: if(_tmp6A3
<= (void*)4)goto _LL49D;if(*((int*)_tmp6A3)!= 5)goto _LL49D;_LL49C: goto _LL49E;
_LL49D: if((int)_tmp6A3 != 1)goto _LL49F;_LL49E: goto _LL4A0;_LL49F: if(_tmp6A3 <= (
void*)4)goto _LL4A1;if(*((int*)_tmp6A3)!= 6)goto _LL4A1;_LL4A0: goto _LL4A2;_LL4A1:
if((int)_tmp6A3 != 3)goto _LL4A3;_LL4A2: goto _LL4A4;_LL4A3: if((int)_tmp6A3 != 2)goto
_LL4A5;_LL4A4: return t;_LL4A5: if(_tmp6A3 <= (void*)4)goto _LL4A7;if(*((int*)_tmp6A3)
!= 22)goto _LL4A7;_tmp6D3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6A3)->f1;
_LL4A6: {void*_tmp730=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D3);return _tmp730 == 
_tmp6D3?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp731=_cycalloc(sizeof(*
_tmp731));_tmp731[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp732;_tmp732.tag=22;
_tmp732.f1=(void*)_tmp730;_tmp732;});_tmp731;});}_LL4A7: if(_tmp6A3 <= (void*)4)
goto _LL4A9;if(*((int*)_tmp6A3)!= 20)goto _LL4A9;_tmp6D4=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6A3)->f1;_LL4A8: {void*_tmp733=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D4);
return _tmp733 == _tmp6D4?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp734=
_cycalloc(sizeof(*_tmp734));_tmp734[0]=({struct Cyc_Absyn_AccessEff_struct _tmp735;
_tmp735.tag=20;_tmp735.f1=(void*)_tmp733;_tmp735;});_tmp734;});}_LL4A9: if(
_tmp6A3 <= (void*)4)goto _LL474;if(*((int*)_tmp6A3)!= 21)goto _LL474;_tmp6D5=((
struct Cyc_Absyn_JoinEff_struct*)_tmp6A3)->f1;_LL4AA: {struct Cyc_List_List*
_tmp736=Cyc_Tcutil_substs(rgn,inst,_tmp6D5);return _tmp736 == _tmp6D5?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp738;_tmp738.tag=21;_tmp738.f1=_tmp736;_tmp738;});
_tmp737;});}_LL474:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp739=(void*)ts->hd;struct Cyc_List_List*_tmp73A=ts->tl;void*
_tmp73B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp739);struct Cyc_List_List*_tmp73C=Cyc_Tcutil_substs(
rgn,inst,_tmp73A);if(_tmp739 == _tmp73B  && _tmp73A == _tmp73C)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp73D=
_cycalloc(sizeof(*_tmp73D));_tmp73D->hd=(void*)_tmp73B;_tmp73D->tl=_tmp73C;
_tmp73D;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple8*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E->f1=tv;_tmp73E->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp73F=_cycalloc(sizeof(*_tmp73F));
_tmp73F->v=(void*)k;_tmp73F;}),({struct Cyc_Core_Opt*_tmp740=_cycalloc(sizeof(*
_tmp740));_tmp740->v=s;_tmp740;}));_tmp73E;});}struct _tuple8*Cyc_Tcutil_r_make_inst_var(
struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){struct _tuple9 _tmp742;struct Cyc_List_List*
_tmp743;struct _RegionHandle*_tmp744;struct _tuple9*_tmp741=env;_tmp742=*_tmp741;
_tmp743=_tmp742.f1;_tmp744=_tmp742.f2;{void*k=Cyc_Tcutil_tvar_kind(tv);return({
struct _tuple8*_tmp745=_region_malloc(_tmp744,sizeof(*_tmp745));_tmp745->f1=tv;
_tmp745->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp746=_cycalloc(sizeof(*
_tmp746));_tmp746->v=(void*)k;_tmp746;}),({struct Cyc_Core_Opt*_tmp747=_cycalloc(
sizeof(*_tmp747));_tmp747->v=_tmp743;_tmp747;}));_tmp745;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp74C;_tmp74C.tag=0;_tmp74C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k2));{
struct Cyc_String_pa_struct _tmp74B;_tmp74B.tag=0;_tmp74B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k1));{
struct Cyc_String_pa_struct _tmp74A;_tmp74A.tag=0;_tmp74A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*tv->name);{void*_tmp748[3]={& _tmp74A,&
_tmp74B,& _tmp74C};Cyc_Tcutil_terr(loc,({const char*_tmp749="type variable %s is used with inconsistent kinds %s and %s";
_tag_dynforward(_tmp749,sizeof(char),_get_zero_arr_size(_tmp749,59));}),
_tag_dynforward(_tmp748,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp74D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp74E="same type variable has different identity!";_tag_dynforward(
_tmp74E,sizeof(char),_get_zero_arr_size(_tmp74E,43));}),_tag_dynforward(_tmp74D,
sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({
struct Cyc_List_List*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=tv;_tmp74F->tl=
tvs;_tmp74F;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp750[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp751="fast_add_free_tvar: bad identity in tv";_tag_dynforward(
_tmp751,sizeof(char),_get_zero_arr_size(_tmp751,39));}),_tag_dynforward(_tmp750,
sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp752=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp752->identity
== 0)({void*_tmp753[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp754="fast_add_free_tvar: bad identity in tvs2";
_tag_dynforward(_tmp754,sizeof(char),_get_zero_arr_size(_tmp754,41));}),
_tag_dynforward(_tmp753,sizeof(void*),0));});if(*((int*)_check_null(_tmp752->identity))
== *((int*)_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*
_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->hd=tv;_tmp755->tl=tvs;_tmp755;});}
struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == 0)({void*
_tmp756[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp757="fast_add_free_tvar_bool: bad identity in tv";_tag_dynforward(
_tmp757,sizeof(char),_get_zero_arr_size(_tmp757,44));}),_tag_dynforward(_tmp756,
sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct _tuple18 _tmp759;struct Cyc_Absyn_Tvar*_tmp75A;int _tmp75B;int*_tmp75C;struct
_tuple18*_tmp758=(struct _tuple18*)tvs2->hd;_tmp759=*_tmp758;_tmp75A=_tmp759.f1;
_tmp75B=_tmp759.f2;_tmp75C=(int*)&(*_tmp758).f2;if(_tmp75A->identity == 0)({void*
_tmp75D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp75E="fast_add_free_tvar_bool: bad identity in tvs2";
_tag_dynforward(_tmp75E,sizeof(char),_get_zero_arr_size(_tmp75E,46));}),
_tag_dynforward(_tmp75D,sizeof(void*),0));});if(*((int*)_check_null(_tmp75A->identity))
== *((int*)_check_null(tv->identity))){*_tmp75C=*_tmp75C  || b;return tvs;}}}
return({struct Cyc_List_List*_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F->hd=({
struct _tuple18*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->f1=tv;_tmp760->f2=b;
_tmp760;});_tmp75F->tl=tvs;_tmp75F;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_String_pa_struct
_tmp763;_tmp763.tag=0;_tmp763.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string(tv));{void*_tmp761[1]={& _tmp763};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp762="bound tvar id for %s is NULL";_tag_dynforward(_tmp762,sizeof(char),
_get_zero_arr_size(_tmp762,29));}),_tag_dynforward(_tmp761,sizeof(void*),1));}});
return({struct Cyc_List_List*_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764->hd=tv;
_tmp764->tl=tvs;_tmp764;});}struct _tuple19{void*f1;int f2;};static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e,int b){void*_tmp765=Cyc_Tcutil_compress(
e);int _tmp766;_LL4B6: if(_tmp765 <= (void*)4)goto _LL4B8;if(*((int*)_tmp765)!= 0)
goto _LL4B8;_tmp766=((struct Cyc_Absyn_Evar_struct*)_tmp765)->f3;_LL4B7:{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple19 _tmp768;void*_tmp769;int _tmp76A;
int*_tmp76B;struct _tuple19*_tmp767=(struct _tuple19*)es2->hd;_tmp768=*_tmp767;
_tmp769=_tmp768.f1;_tmp76A=_tmp768.f2;_tmp76B=(int*)&(*_tmp767).f2;{void*_tmp76C=
Cyc_Tcutil_compress(_tmp769);int _tmp76D;_LL4BB: if(_tmp76C <= (void*)4)goto _LL4BD;
if(*((int*)_tmp76C)!= 0)goto _LL4BD;_tmp76D=((struct Cyc_Absyn_Evar_struct*)
_tmp76C)->f3;_LL4BC: if(_tmp766 == _tmp76D){if(b != *_tmp76B)*_tmp76B=1;return es;}
goto _LL4BA;_LL4BD:;_LL4BE: goto _LL4BA;_LL4BA:;}}}return({struct Cyc_List_List*
_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E->hd=({struct _tuple19*_tmp76F=
_cycalloc(sizeof(*_tmp76F));_tmp76F->f1=e;_tmp76F->f2=b;_tmp76F;});_tmp76E->tl=
es;_tmp76E;});_LL4B8:;_LL4B9: return es;_LL4B5:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;
tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!
present)r=({struct Cyc_List_List*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp770->tl=r;_tmp770;});}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct Cyc_List_List*tvs,struct Cyc_List_List*
btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*
_tmp772;int _tmp773;struct _tuple18 _tmp771=*((struct _tuple18*)tvs->hd);_tmp772=
_tmp771.f1;_tmp773=_tmp771.f2;{int present=0;{struct Cyc_List_List*b=btvs;for(0;b
!= 0;b=b->tl){if(*((int*)_check_null(_tmp772->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!present)r=({
struct Cyc_List_List*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->hd=(struct
_tuple18*)tvs->hd;_tmp774->tl=r;_tmp774;});}}r=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _dynforward_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp777;_tmp777.tag=
0;_tmp777.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp775[1]={&
_tmp777};Cyc_Tcutil_terr(loc,({const char*_tmp776="bitfield %s does not have constant width";
_tag_dynforward(_tmp776,sizeof(char),_get_zero_arr_size(_tmp776,41));}),
_tag_dynforward(_tmp775,sizeof(void*),1));}});else{unsigned int _tmp779;int
_tmp77A;struct _tuple7 _tmp778=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
width);_tmp779=_tmp778.f1;_tmp77A=_tmp778.f2;if(!_tmp77A)({void*_tmp77B[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp77C="bitfield width cannot use sizeof or offsetof";
_tag_dynforward(_tmp77C,sizeof(char),_get_zero_arr_size(_tmp77C,45));}),
_tag_dynforward(_tmp77B,sizeof(void*),0));});w=_tmp779;}{void*_tmp77D=Cyc_Tcutil_compress(
field_typ);void*_tmp77E;_LL4C0: if(_tmp77D <= (void*)4)goto _LL4C2;if(*((int*)
_tmp77D)!= 5)goto _LL4C2;_tmp77E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp77D)->f2;
_LL4C1:{void*_tmp77F=_tmp77E;_LL4C5: if((int)_tmp77F != 0)goto _LL4C7;_LL4C6: if(w > 
8)({void*_tmp780[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp781="bitfield larger than type";
_tag_dynforward(_tmp781,sizeof(char),_get_zero_arr_size(_tmp781,26));}),
_tag_dynforward(_tmp780,sizeof(void*),0));});goto _LL4C4;_LL4C7: if((int)_tmp77F != 
1)goto _LL4C9;_LL4C8: if(w > 16)({void*_tmp782[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp783="bitfield larger than type";_tag_dynforward(_tmp783,sizeof(char),
_get_zero_arr_size(_tmp783,26));}),_tag_dynforward(_tmp782,sizeof(void*),0));});
goto _LL4C4;_LL4C9: if((int)_tmp77F != 3)goto _LL4CB;_LL4CA: goto _LL4CC;_LL4CB: if((
int)_tmp77F != 2)goto _LL4CD;_LL4CC: if(w > 32)({void*_tmp784[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp785="bitfield larger than type";_tag_dynforward(_tmp785,
sizeof(char),_get_zero_arr_size(_tmp785,26));}),_tag_dynforward(_tmp784,sizeof(
void*),0));});goto _LL4C4;_LL4CD: if((int)_tmp77F != 4)goto _LL4C4;_LL4CE: if(w > 64)({
void*_tmp786[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp787="bitfield larger than type";
_tag_dynforward(_tmp787,sizeof(char),_get_zero_arr_size(_tmp787,26));}),
_tag_dynforward(_tmp786,sizeof(void*),0));});goto _LL4C4;_LL4C4:;}goto _LL4BF;
_LL4C2:;_LL4C3:({struct Cyc_String_pa_struct _tmp78B;_tmp78B.tag=0;_tmp78B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(field_typ));{
struct Cyc_String_pa_struct _tmp78A;_tmp78A.tag=0;_tmp78A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp788[2]={& _tmp78A,& _tmp78B};
Cyc_Tcutil_terr(loc,({const char*_tmp789="bitfield %s must have integral type but has type %s";
_tag_dynforward(_tmp789,sizeof(char),_get_zero_arr_size(_tmp789,52));}),
_tag_dynforward(_tmp788,sizeof(void*),2));}}});goto _LL4BF;_LL4BF:;}}}static void
Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp78C=(void*)
atts->hd;_LL4D0: if((int)_tmp78C != 5)goto _LL4D2;_LL4D1: continue;_LL4D2: if(_tmp78C
<= (void*)17)goto _LL4D4;if(*((int*)_tmp78C)!= 1)goto _LL4D4;_LL4D3: continue;
_LL4D4:;_LL4D5:({struct Cyc_String_pa_struct _tmp790;_tmp790.tag=0;_tmp790.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_String_pa_struct
_tmp78F;_tmp78F.tag=0;_tmp78F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp78D[2]={& _tmp78F,& _tmp790};
Cyc_Tcutil_terr(loc,({const char*_tmp78E="bad attribute %s on member %s";
_tag_dynforward(_tmp78E,sizeof(char),_get_zero_arr_size(_tmp78E,30));}),
_tag_dynforward(_tmp78D,sizeof(void*),2));}}});_LL4CF:;}}struct Cyc_Tcutil_CVTEnv{
struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp791=t;struct
Cyc_Absyn_Typedefdecl*_tmp792;void**_tmp793;_LL4D7: if(_tmp791 <= (void*)4)goto
_LL4D9;if(*((int*)_tmp791)!= 17)goto _LL4D9;_tmp792=((struct Cyc_Absyn_TypedefType_struct*)
_tmp791)->f3;_tmp793=((struct Cyc_Absyn_TypedefType_struct*)_tmp791)->f4;_LL4D8:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp792))->tq).real_const  || (
_tmp792->tq).print_const){if(declared_const)({void*_tmp794[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp795="extra const";_tag_dynforward(_tmp795,sizeof(char),
_get_zero_arr_size(_tmp795,12));}),_tag_dynforward(_tmp794,sizeof(void*),0));});
return 1;}if((unsigned int)_tmp793)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp793);else{return declared_const;}_LL4D9:;_LL4DA: return
declared_const;_LL4D6:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t,int allow_unique,int put_in_effect){{void*_tmp796=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp797;struct Cyc_Core_Opt**_tmp798;
struct Cyc_Core_Opt*_tmp799;struct Cyc_Core_Opt**_tmp79A;struct Cyc_Absyn_Tvar*
_tmp79B;struct Cyc_List_List*_tmp79C;struct _tuple2*_tmp79D;struct Cyc_Absyn_Enumdecl*
_tmp79E;struct Cyc_Absyn_Enumdecl**_tmp79F;struct Cyc_Absyn_TunionInfo _tmp7A0;
union Cyc_Absyn_TunionInfoU_union _tmp7A1;union Cyc_Absyn_TunionInfoU_union*_tmp7A2;
struct Cyc_List_List*_tmp7A3;struct Cyc_List_List**_tmp7A4;struct Cyc_Core_Opt*
_tmp7A5;struct Cyc_Core_Opt**_tmp7A6;struct Cyc_Absyn_TunionFieldInfo _tmp7A7;union
Cyc_Absyn_TunionFieldInfoU_union _tmp7A8;union Cyc_Absyn_TunionFieldInfoU_union*
_tmp7A9;struct Cyc_List_List*_tmp7AA;struct Cyc_Absyn_PtrInfo _tmp7AB;void*_tmp7AC;
struct Cyc_Absyn_Tqual _tmp7AD;struct Cyc_Absyn_Tqual*_tmp7AE;struct Cyc_Absyn_PtrAtts
_tmp7AF;void*_tmp7B0;struct Cyc_Absyn_Conref*_tmp7B1;struct Cyc_Absyn_Conref*
_tmp7B2;struct Cyc_Absyn_Conref*_tmp7B3;void*_tmp7B4;void*_tmp7B5;struct Cyc_Absyn_ArrayInfo
_tmp7B6;void*_tmp7B7;struct Cyc_Absyn_Tqual _tmp7B8;struct Cyc_Absyn_Tqual*_tmp7B9;
struct Cyc_Absyn_Exp*_tmp7BA;struct Cyc_Absyn_Exp**_tmp7BB;struct Cyc_Absyn_Conref*
_tmp7BC;struct Cyc_Position_Segment*_tmp7BD;struct Cyc_Absyn_FnInfo _tmp7BE;struct
Cyc_List_List*_tmp7BF;struct Cyc_List_List**_tmp7C0;struct Cyc_Core_Opt*_tmp7C1;
struct Cyc_Core_Opt**_tmp7C2;void*_tmp7C3;struct Cyc_List_List*_tmp7C4;int _tmp7C5;
struct Cyc_Absyn_VarargInfo*_tmp7C6;struct Cyc_List_List*_tmp7C7;struct Cyc_List_List*
_tmp7C8;struct Cyc_List_List*_tmp7C9;void*_tmp7CA;struct Cyc_List_List*_tmp7CB;
struct Cyc_Absyn_AggrInfo _tmp7CC;union Cyc_Absyn_AggrInfoU_union _tmp7CD;union Cyc_Absyn_AggrInfoU_union*
_tmp7CE;struct Cyc_List_List*_tmp7CF;struct Cyc_List_List**_tmp7D0;struct _tuple2*
_tmp7D1;struct Cyc_List_List*_tmp7D2;struct Cyc_List_List**_tmp7D3;struct Cyc_Absyn_Typedefdecl*
_tmp7D4;struct Cyc_Absyn_Typedefdecl**_tmp7D5;void**_tmp7D6;void***_tmp7D7;void*
_tmp7D8;void*_tmp7D9;void*_tmp7DA;void*_tmp7DB;void*_tmp7DC;struct Cyc_List_List*
_tmp7DD;_LL4DC: if((int)_tmp796 != 0)goto _LL4DE;_LL4DD: goto _LL4DB;_LL4DE: if(
_tmp796 <= (void*)4)goto _LL4F2;if(*((int*)_tmp796)!= 0)goto _LL4E0;_tmp797=((
struct Cyc_Absyn_Evar_struct*)_tmp796)->f1;_tmp798=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp796)->f1;_tmp799=((struct Cyc_Absyn_Evar_struct*)
_tmp796)->f2;_tmp79A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp796)->f2;_LL4DF: if(*_tmp798 == 0)*_tmp798=({struct Cyc_Core_Opt*_tmp7DE=
_cycalloc(sizeof(*_tmp7DE));_tmp7DE->v=(void*)expected_kind;_tmp7DE;});if((
cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind == (void*)3)*
_tmp79A=({struct Cyc_Core_Opt*_tmp7DF=_cycalloc(sizeof(*_tmp7DF));_tmp7DF->v=(
void*)((void*)2);_tmp7DF;});else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Tvar*
_tmp7E0=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7E4=
_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7E5;
_tmp7E5.tag=2;_tmp7E5.f1=0;_tmp7E5.f2=(void*)expected_kind;_tmp7E5;});_tmp7E4;}));*
_tmp79A=({struct Cyc_Core_Opt*_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1->v=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7E2=_cycalloc(sizeof(*_tmp7E2));
_tmp7E2[0]=({struct Cyc_Absyn_VarType_struct _tmp7E3;_tmp7E3.tag=1;_tmp7E3.f1=
_tmp7E0;_tmp7E3;});_tmp7E2;}));_tmp7E1;});_tmp79B=_tmp7E0;goto _LL4E1;}else{
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t,put_in_effect);}}
goto _LL4DB;_LL4E0: if(*((int*)_tmp796)!= 1)goto _LL4E2;_tmp79B=((struct Cyc_Absyn_VarType_struct*)
_tmp796)->f1;_LL4E1:{void*_tmp7E6=Cyc_Absyn_compress_kb((void*)_tmp79B->kind);
struct Cyc_Core_Opt*_tmp7E7;struct Cyc_Core_Opt**_tmp7E8;_LL513: if(*((int*)_tmp7E6)
!= 1)goto _LL515;_tmp7E7=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7E6)->f1;
_tmp7E8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7E6)->f1;
_LL514:*_tmp7E8=({struct Cyc_Core_Opt*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));_tmp7E9->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));
_tmp7EA[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7EB;_tmp7EB.tag=2;_tmp7EB.f1=0;
_tmp7EB.f2=(void*)expected_kind;_tmp7EB;});_tmp7EA;}));_tmp7E9;});goto _LL512;
_LL515:;_LL516: goto _LL512;_LL512:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmp79B);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.free_vars,_tmp79B,put_in_effect);goto _LL4DB;_LL4E2: if(*((int*)_tmp796)!= 
13)goto _LL4E4;_tmp79C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp796)->f1;_LL4E3: {
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle _tmp7EC=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp7EC;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=0;for(0;
_tmp79C != 0;_tmp79C=_tmp79C->tl){struct Cyc_Absyn_Enumfield*_tmp7ED=(struct Cyc_Absyn_Enumfield*)
_tmp79C->hd;if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct
_dynforward_ptr*),struct Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp7ED->name).f2))({struct Cyc_String_pa_struct
_tmp7F0;_tmp7F0.tag=0;_tmp7F0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
_tmp7ED->name).f2);{void*_tmp7EE[1]={& _tmp7F0};Cyc_Tcutil_terr(_tmp7ED->loc,({
const char*_tmp7EF="duplicate enum field name %s";_tag_dynforward(_tmp7EF,sizeof(
char),_get_zero_arr_size(_tmp7EF,29));}),_tag_dynforward(_tmp7EE,sizeof(void*),1));}});
else{prev_fields=({struct Cyc_List_List*_tmp7F1=_region_malloc(uprev_rgn,sizeof(*
_tmp7F1));_tmp7F1->hd=(*_tmp7ED->name).f2;_tmp7F1->tl=prev_fields;_tmp7F1;});}
if(_tmp7ED->tag == 0)_tmp7ED->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp7ED->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp7ED->tag)))({struct Cyc_String_pa_struct _tmp7F4;_tmp7F4.tag=0;
_tmp7F4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*_tmp7ED->name).f2);{
void*_tmp7F2[1]={& _tmp7F4};Cyc_Tcutil_terr(loc,({const char*_tmp7F3="enum field %s: expression is not constant";
_tag_dynforward(_tmp7F3,sizeof(char),_get_zero_arr_size(_tmp7F3,42));}),
_tag_dynforward(_tmp7F2,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp7ED->tag))).f1;tag_count=t1 + 1;(*_tmp7ED->name).f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7F5;(_tmp7F5.Abs_n).tag=
2;(_tmp7F5.Abs_n).f1=te->ns;_tmp7F5;});ge->ordinaries=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k,struct _tuple19*v))Cyc_Dict_insert)(
ge->ordinaries,(*_tmp7ED->name).f2,(struct _tuple19*)({struct _tuple19*_tmp7F6=
_cycalloc(sizeof(*_tmp7F6));_tmp7F6->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7[0]=({struct Cyc_Tcenv_AnonEnumRes_struct
_tmp7F8;_tmp7F8.tag=4;_tmp7F8.f1=(void*)t;_tmp7F8.f2=_tmp7ED;_tmp7F8;});_tmp7F7;});
_tmp7F6->f2=1;_tmp7F6;}));}}};_pop_region(uprev_rgn);}goto _LL4DB;}_LL4E4: if(*((
int*)_tmp796)!= 12)goto _LL4E6;_tmp79D=((struct Cyc_Absyn_EnumType_struct*)_tmp796)->f1;
_tmp79E=((struct Cyc_Absyn_EnumType_struct*)_tmp796)->f2;_tmp79F=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp796)->f2;_LL4E5: if(*_tmp79F == 0  || ((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp79F))->fields == 0){struct _handler_cons
_tmp7F9;_push_handler(& _tmp7F9);{int _tmp7FB=0;if(setjmp(_tmp7F9.handler))_tmp7FB=
1;if(!_tmp7FB){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp79D);*_tmp79F=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp7FA=(void*)_exn_thrown;void*_tmp7FD=_tmp7FA;_LL518: if(_tmp7FD != Cyc_Dict_Absent)
goto _LL51A;_LL519: {struct Cyc_Tcenv_Genv*_tmp7FE=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp7FF=({struct Cyc_Absyn_Enumdecl*_tmp800=_cycalloc(
sizeof(*_tmp800));_tmp800->sc=(void*)((void*)3);_tmp800->name=_tmp79D;_tmp800->fields=
0;_tmp800;});Cyc_Tc_tcEnumdecl(te,_tmp7FE,loc,_tmp7FF);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp79D);*_tmp79F=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL517;}}_LL51A:;_LL51B:(void)_throw(_tmp7FD);_LL517:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp79F);*_tmp79D=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL4DB;}_LL4E6: if(*((int*)_tmp796)!= 2)
goto _LL4E8;_tmp7A0=((struct Cyc_Absyn_TunionType_struct*)_tmp796)->f1;_tmp7A1=
_tmp7A0.tunion_info;_tmp7A2=(union Cyc_Absyn_TunionInfoU_union*)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp796)->f1).tunion_info;_tmp7A3=_tmp7A0.targs;_tmp7A4=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp796)->f1).targs;_tmp7A5=_tmp7A0.rgn;
_tmp7A6=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp796)->f1).rgn;
_LL4E7: {struct Cyc_List_List*_tmp801=*_tmp7A4;{union Cyc_Absyn_TunionInfoU_union
_tmp802=*_tmp7A2;struct Cyc_Absyn_UnknownTunionInfo _tmp803;struct _tuple2*_tmp804;
int _tmp805;int _tmp806;struct Cyc_Absyn_Tuniondecl**_tmp807;struct Cyc_Absyn_Tuniondecl*
_tmp808;_LL51D: if((_tmp802.UnknownTunion).tag != 0)goto _LL51F;_tmp803=(_tmp802.UnknownTunion).f1;
_tmp804=_tmp803.name;_tmp805=_tmp803.is_xtunion;_tmp806=_tmp803.is_flat;_LL51E: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp809;_push_handler(&
_tmp809);{int _tmp80B=0;if(setjmp(_tmp809.handler))_tmp80B=1;if(!_tmp80B){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp804);;_pop_handler();}else{void*_tmp80A=(
void*)_exn_thrown;void*_tmp80D=_tmp80A;_LL522: if(_tmp80D != Cyc_Dict_Absent)goto
_LL524;_LL523: {struct Cyc_Tcenv_Genv*_tmp80E=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmp80F=({struct Cyc_Absyn_Tuniondecl*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816->sc=(
void*)((void*)3);_tmp816->name=_tmp804;_tmp816->tvs=0;_tmp816->fields=0;_tmp816->is_xtunion=
_tmp805;_tmp816->is_flat=_tmp806;_tmp816;});Cyc_Tc_tcTuniondecl(te,_tmp80E,loc,
_tmp80F);tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp804);if(_tmp801 != 0){({
struct Cyc_String_pa_struct _tmp813;_tmp813.tag=0;_tmp813.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp804));{
struct Cyc_String_pa_struct _tmp812;_tmp812.tag=0;_tmp812.f1=(struct
_dynforward_ptr)(_tmp805?(struct _dynforward_ptr)((struct _dynforward_ptr)({const
char*_tmp814="xtunion";_tag_dynforward(_tmp814,sizeof(char),_get_zero_arr_size(
_tmp814,8));})):(struct _dynforward_ptr)({const char*_tmp815="tunion";
_tag_dynforward(_tmp815,sizeof(char),_get_zero_arr_size(_tmp815,7));}));{void*
_tmp810[2]={& _tmp812,& _tmp813};Cyc_Tcutil_terr(loc,({const char*_tmp811="declare parameterized %s %s before using";
_tag_dynforward(_tmp811,sizeof(char),_get_zero_arr_size(_tmp811,41));}),
_tag_dynforward(_tmp810,sizeof(void*),2));}}});return cvtenv;}goto _LL521;}_LL524:;
_LL525:(void)_throw(_tmp80D);_LL521:;}}}if((*tudp)->is_xtunion != _tmp805)({
struct Cyc_String_pa_struct _tmp819;_tmp819.tag=0;_tmp819.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp804));{void*
_tmp817[1]={& _tmp819};Cyc_Tcutil_terr(loc,({const char*_tmp818="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp818,sizeof(char),_get_zero_arr_size(_tmp818,48));}),
_tag_dynforward(_tmp817,sizeof(void*),1));}});*_tmp7A2=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp81A;(_tmp81A.KnownTunion).tag=1;(_tmp81A.KnownTunion).f1=
tudp;_tmp81A;});_tmp808=*tudp;goto _LL520;}_LL51F: if((_tmp802.KnownTunion).tag != 
1)goto _LL51C;_tmp807=(_tmp802.KnownTunion).f1;_tmp808=*_tmp807;_LL520: if(_tmp808->is_flat
 && (unsigned int)*_tmp7A6)({struct Cyc_String_pa_struct _tmp81D;_tmp81D.tag=0;
_tmp81D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7A6))->v));{void*_tmp81B[1]={&
_tmp81D};Cyc_Tcutil_terr(loc,({const char*_tmp81C="flat tunion has region %s";
_tag_dynforward(_tmp81C,sizeof(char),_get_zero_arr_size(_tmp81C,26));}),
_tag_dynforward(_tmp81B,sizeof(void*),1));}});if(!_tmp808->is_flat  && !((
unsigned int)*_tmp7A6))*_tmp7A6=({struct Cyc_Core_Opt*_tmp81E=_cycalloc(sizeof(*
_tmp81E));_tmp81E->v=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp81F=
_cycalloc(sizeof(*_tmp81F));_tmp81F->v=(void*)((void*)3);_tmp81F;}),0);_tmp81E;});
if((unsigned int)*_tmp7A6){void*_tmp820=(void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp7A6))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp820,
allow_unique,1);}{struct Cyc_List_List*tvs=_tmp808->tvs;for(0;_tmp801 != 0  && tvs
!= 0;(_tmp801=_tmp801->tl,tvs=tvs->tl)){void*t1=(void*)_tmp801->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,0,1);}if(_tmp801 != 0)({struct Cyc_String_pa_struct _tmp823;_tmp823.tag=
0;_tmp823.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp808->name));{void*_tmp821[1]={& _tmp823};Cyc_Tcutil_terr(loc,({const char*
_tmp822="too many type arguments for tunion %s";_tag_dynforward(_tmp822,sizeof(
char),_get_zero_arr_size(_tmp822,38));}),_tag_dynforward(_tmp821,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824->hd=(
void*)e;_tmp824->tl=hidden_ts;_tmp824;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,0,1);}*_tmp7A4=Cyc_List_imp_append(*_tmp7A4,Cyc_List_imp_rev(
hidden_ts));}goto _LL51C;}_LL51C:;}goto _LL4DB;}_LL4E8: if(*((int*)_tmp796)!= 3)
goto _LL4EA;_tmp7A7=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp796)->f1;
_tmp7A8=_tmp7A7.field_info;_tmp7A9=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp796)->f1).field_info;_tmp7AA=_tmp7A7.targs;
_LL4E9:{union Cyc_Absyn_TunionFieldInfoU_union _tmp825=*_tmp7A9;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp826;struct _tuple2*_tmp827;struct _tuple2*_tmp828;int _tmp829;struct Cyc_Absyn_Tuniondecl*
_tmp82A;struct Cyc_Absyn_Tunionfield*_tmp82B;_LL527: if((_tmp825.UnknownTunionfield).tag
!= 0)goto _LL529;_tmp826=(_tmp825.UnknownTunionfield).f1;_tmp827=_tmp826.tunion_name;
_tmp828=_tmp826.field_name;_tmp829=_tmp826.is_xtunion;_LL528: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp82C;_push_handler(&
_tmp82C);{int _tmp82E=0;if(setjmp(_tmp82C.handler))_tmp82E=1;if(!_tmp82E){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp827);;_pop_handler();}else{void*_tmp82D=(void*)_exn_thrown;void*
_tmp830=_tmp82D;_LL52C: if(_tmp830 != Cyc_Dict_Absent)goto _LL52E;_LL52D:({struct
Cyc_String_pa_struct _tmp833;_tmp833.tag=0;_tmp833.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp827));{void*_tmp831[1]={&
_tmp833};Cyc_Tcutil_terr(loc,({const char*_tmp832="unbound type tunion %s";
_tag_dynforward(_tmp832,sizeof(char),_get_zero_arr_size(_tmp832,23));}),
_tag_dynforward(_tmp831,sizeof(void*),1));}});return cvtenv;_LL52E:;_LL52F:(void)
_throw(_tmp830);_LL52B:;}}}{struct _handler_cons _tmp834;_push_handler(& _tmp834);{
int _tmp836=0;if(setjmp(_tmp834.handler))_tmp836=1;if(!_tmp836){{struct
_RegionHandle _tmp837=_new_region("r");struct _RegionHandle*r=& _tmp837;
_push_region(r);{void*_tmp838=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp828);struct
Cyc_Absyn_Tuniondecl*_tmp839;struct Cyc_Absyn_Tunionfield*_tmp83A;_LL531: if(*((
int*)_tmp838)!= 2)goto _LL533;_tmp839=((struct Cyc_Tcenv_TunionRes_struct*)_tmp838)->f1;
_tmp83A=((struct Cyc_Tcenv_TunionRes_struct*)_tmp838)->f2;_LL532: tuf=_tmp83A;tud=
_tmp839;if(tud->is_xtunion != _tmp829)({struct Cyc_String_pa_struct _tmp83D;_tmp83D.tag=
0;_tmp83D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp827));{void*_tmp83B[1]={& _tmp83D};Cyc_Tcutil_terr(loc,({const char*_tmp83C="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp83C,sizeof(char),_get_zero_arr_size(_tmp83C,48));}),
_tag_dynforward(_tmp83B,sizeof(void*),1));}});goto _LL530;_LL533:;_LL534:({struct
Cyc_String_pa_struct _tmp841;_tmp841.tag=0;_tmp841.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp827));{struct Cyc_String_pa_struct
_tmp840;_tmp840.tag=0;_tmp840.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp828));{void*_tmp83E[2]={& _tmp840,& _tmp841};Cyc_Tcutil_terr(
loc,({const char*_tmp83F="unbound field %s in type tunion %s";_tag_dynforward(
_tmp83F,sizeof(char),_get_zero_arr_size(_tmp83F,35));}),_tag_dynforward(_tmp83E,
sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp842=cvtenv;_npop_handler(1);
return _tmp842;}_LL530:;};_pop_region(r);};_pop_handler();}else{void*_tmp835=(
void*)_exn_thrown;void*_tmp844=_tmp835;_LL536: if(_tmp844 != Cyc_Dict_Absent)goto
_LL538;_LL537:({struct Cyc_String_pa_struct _tmp848;_tmp848.tag=0;_tmp848.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp827));{
struct Cyc_String_pa_struct _tmp847;_tmp847.tag=0;_tmp847.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp828));{void*
_tmp845[2]={& _tmp847,& _tmp848};Cyc_Tcutil_terr(loc,({const char*_tmp846="unbound field %s in type tunion %s";
_tag_dynforward(_tmp846,sizeof(char),_get_zero_arr_size(_tmp846,35));}),
_tag_dynforward(_tmp845,sizeof(void*),2));}}});return cvtenv;_LL538:;_LL539:(void)
_throw(_tmp844);_LL535:;}}}*_tmp7A9=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp849;(_tmp849.KnownTunionfield).tag=1;(
_tmp849.KnownTunionfield).f1=tud;(_tmp849.KnownTunionfield).f2=tuf;_tmp849;});
_tmp82A=tud;_tmp82B=tuf;goto _LL52A;}_LL529: if((_tmp825.KnownTunionfield).tag != 1)
goto _LL526;_tmp82A=(_tmp825.KnownTunionfield).f1;_tmp82B=(_tmp825.KnownTunionfield).f2;
_LL52A: {struct Cyc_List_List*tvs=_tmp82A->tvs;for(0;_tmp7AA != 0  && tvs != 0;(
_tmp7AA=_tmp7AA->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7AA->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,0,1);}if(_tmp7AA != 0)({struct Cyc_String_pa_struct _tmp84D;_tmp84D.tag=
0;_tmp84D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp82B->name));{struct Cyc_String_pa_struct _tmp84C;_tmp84C.tag=0;_tmp84C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp82A->name));{
void*_tmp84A[2]={& _tmp84C,& _tmp84D};Cyc_Tcutil_terr(loc,({const char*_tmp84B="too many type arguments for tunion %s.%s";
_tag_dynforward(_tmp84B,sizeof(char),_get_zero_arr_size(_tmp84B,41));}),
_tag_dynforward(_tmp84A,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct
_tmp851;_tmp851.tag=0;_tmp851.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp82B->name));{struct Cyc_String_pa_struct _tmp850;
_tmp850.tag=0;_tmp850.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp82A->name));{void*_tmp84E[2]={& _tmp850,& _tmp851};Cyc_Tcutil_terr(loc,({const
char*_tmp84F="too few type arguments for tunion %s.%s";_tag_dynforward(_tmp84F,
sizeof(char),_get_zero_arr_size(_tmp84F,40));}),_tag_dynforward(_tmp84E,sizeof(
void*),2));}}});goto _LL526;}_LL526:;}goto _LL4DB;_LL4EA: if(*((int*)_tmp796)!= 4)
goto _LL4EC;_tmp7AB=((struct Cyc_Absyn_PointerType_struct*)_tmp796)->f1;_tmp7AC=(
void*)_tmp7AB.elt_typ;_tmp7AD=_tmp7AB.elt_tq;_tmp7AE=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp796)->f1).elt_tq;_tmp7AF=_tmp7AB.ptr_atts;
_tmp7B0=(void*)_tmp7AF.rgn;_tmp7B1=_tmp7AF.nullable;_tmp7B2=_tmp7AF.bounds;
_tmp7B3=_tmp7AF.zero_term;_LL4EB: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp7AC,0,1);_tmp7AE->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp7AE->print_const,_tmp7AC);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp7B0,allow_unique,1);{union Cyc_Absyn_Constraint_union _tmp852=(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7B3))->v;int _tmp853;_LL53B: if((_tmp852.No_constr).tag != 2)goto _LL53D;_LL53C:{
void*_tmp854=Cyc_Tcutil_compress(_tmp7AC);void*_tmp855;_LL542: if(_tmp854 <= (void*)
4)goto _LL544;if(*((int*)_tmp854)!= 5)goto _LL544;_tmp855=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp854)->f2;if((int)_tmp855 != 0)goto _LL544;_LL543:((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp7B3,Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL541;_LL544:;_LL545:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7B3,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL541;
_LL541:;}goto _LL53A;_LL53D: if((_tmp852.Eq_constr).tag != 0)goto _LL53F;_tmp853=(
_tmp852.Eq_constr).f1;if(_tmp853 != 1)goto _LL53F;_LL53E: if(!Cyc_Tcutil_admits_zero(
_tmp7AC))({struct Cyc_String_pa_struct _tmp858;_tmp858.tag=0;_tmp858.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp7AC));{void*
_tmp856[1]={& _tmp858};Cyc_Tcutil_terr(loc,({const char*_tmp857="cannot have a pointer to zero-terminated %s elements";
_tag_dynforward(_tmp857,sizeof(char),_get_zero_arr_size(_tmp857,53));}),
_tag_dynforward(_tmp856,sizeof(void*),1));}});is_zero_terminated=1;goto _LL53A;
_LL53F:;_LL540: is_zero_terminated=0;goto _LL53A;_LL53A:;}{union Cyc_Absyn_Constraint_union
_tmp859=(Cyc_Absyn_compress_conref(_tmp7B2))->v;void*_tmp85A;void*_tmp85B;void*
_tmp85C;struct Cyc_Absyn_Exp*_tmp85D;void*_tmp85E;void*_tmp85F;_LL547: if((_tmp859.No_constr).tag
!= 2)goto _LL549;_LL548: goto _LL54A;_LL549: if((_tmp859.Eq_constr).tag != 0)goto
_LL54B;_tmp85A=(_tmp859.Eq_constr).f1;if((int)_tmp85A != 0)goto _LL54B;_LL54A: goto
_LL546;_LL54B: if((_tmp859.Eq_constr).tag != 0)goto _LL54D;_tmp85B=(_tmp859.Eq_constr).f1;
if((int)_tmp85B != 1)goto _LL54D;_LL54C: goto _LL546;_LL54D: if((_tmp859.Eq_constr).tag
!= 0)goto _LL54F;_tmp85C=(_tmp859.Eq_constr).f1;if(_tmp85C <= (void*)2)goto _LL54F;
if(*((int*)_tmp85C)!= 0)goto _LL54F;_tmp85D=((struct Cyc_Absyn_Upper_b_struct*)
_tmp85C)->f1;_LL54E: Cyc_Tcexp_tcExp(te,0,_tmp85D);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp85D))({void*_tmp860[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp861="pointer bounds expression is not an unsigned int";
_tag_dynforward(_tmp861,sizeof(char),_get_zero_arr_size(_tmp861,49));}),
_tag_dynforward(_tmp860,sizeof(void*),0));});{unsigned int _tmp863;int _tmp864;
struct _tuple7 _tmp862=Cyc_Evexp_eval_const_uint_exp(_tmp85D);_tmp863=_tmp862.f1;
_tmp864=_tmp862.f2;if(is_zero_terminated  && (!_tmp864  || _tmp863 < 1))({void*
_tmp865[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp866="zero-terminated pointer cannot point to empty sequence";
_tag_dynforward(_tmp866,sizeof(char),_get_zero_arr_size(_tmp866,55));}),
_tag_dynforward(_tmp865,sizeof(void*),0));});goto _LL546;}_LL54F: if((_tmp859.Eq_constr).tag
!= 0)goto _LL551;_tmp85E=(_tmp859.Eq_constr).f1;if(_tmp85E <= (void*)2)goto _LL551;
if(*((int*)_tmp85E)!= 1)goto _LL551;_tmp85F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp85E)->f1;_LL550: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp85F,0,1);goto _LL546;_LL551: if((_tmp859.Forward_constr).tag != 1)goto _LL546;
_LL552:({void*_tmp867[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp868="forward constraint";
_tag_dynforward(_tmp868,sizeof(char),_get_zero_arr_size(_tmp868,19));}),
_tag_dynforward(_tmp867,sizeof(void*),0));});_LL546:;}goto _LL4DB;}_LL4EC: if(*((
int*)_tmp796)!= 18)goto _LL4EE;_tmp7B4=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp796)->f1;_LL4ED: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7B4,allow_unique,1);goto _LL4DB;_LL4EE: if(*((int*)_tmp796)!= 14)goto _LL4F0;
_tmp7B5=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp796)->f1;_LL4EF: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7B5,allow_unique,1);
goto _LL4DB;_LL4F0: if(*((int*)_tmp796)!= 5)goto _LL4F2;_LL4F1: goto _LL4F3;_LL4F2:
if((int)_tmp796 != 1)goto _LL4F4;_LL4F3: goto _LL4F5;_LL4F4: if(_tmp796 <= (void*)4)
goto _LL502;if(*((int*)_tmp796)!= 6)goto _LL4F6;_LL4F5: goto _LL4DB;_LL4F6: if(*((int*)
_tmp796)!= 7)goto _LL4F8;_tmp7B6=((struct Cyc_Absyn_ArrayType_struct*)_tmp796)->f1;
_tmp7B7=(void*)_tmp7B6.elt_type;_tmp7B8=_tmp7B6.tq;_tmp7B9=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp796)->f1).tq;_tmp7BA=_tmp7B6.num_elts;
_tmp7BB=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp796)->f1).num_elts;
_tmp7BC=_tmp7B6.zero_term;_tmp7BD=_tmp7B6.zt_loc;_LL4F7: {struct Cyc_Absyn_Exp*
_tmp869=*_tmp7BB;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp7B7,0,1);_tmp7B9->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,
_tmp7B9->print_const,_tmp7B7);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union
_tmp86A=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7BC))->v;int _tmp86B;_LL554: if((_tmp86A.No_constr).tag != 2)goto _LL556;_LL555:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7BC,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL553;
_LL556: if((_tmp86A.Eq_constr).tag != 0)goto _LL558;_tmp86B=(_tmp86A.Eq_constr).f1;
if(_tmp86B != 1)goto _LL558;_LL557: if(!Cyc_Tcutil_admits_zero(_tmp7B7))({struct Cyc_String_pa_struct
_tmp86E;_tmp86E.tag=0;_tmp86E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp7B7));{void*_tmp86C[1]={& _tmp86E};Cyc_Tcutil_terr(loc,({
const char*_tmp86D="cannot have a zero-terminated array of %s elements";
_tag_dynforward(_tmp86D,sizeof(char),_get_zero_arr_size(_tmp86D,51));}),
_tag_dynforward(_tmp86C,sizeof(void*),1));}});is_zero_terminated=1;goto _LL553;
_LL558:;_LL559: is_zero_terminated=0;goto _LL553;_LL553:;}if(_tmp869 == 0){if(
is_zero_terminated)*_tmp7BB=(_tmp869=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,
0));else{({void*_tmp86F[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp870="array bound defaults to 0 here";
_tag_dynforward(_tmp870,sizeof(char),_get_zero_arr_size(_tmp870,31));}),
_tag_dynforward(_tmp86F,sizeof(void*),0));});*_tmp7BB=(_tmp869=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp869);
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp869))({void*_tmp871[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp872="array bounds expression is not constant";
_tag_dynforward(_tmp872,sizeof(char),_get_zero_arr_size(_tmp872,40));}),
_tag_dynforward(_tmp871,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_tmp869))({void*_tmp873[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp874="array bounds expression is not an unsigned int";_tag_dynforward(
_tmp874,sizeof(char),_get_zero_arr_size(_tmp874,47));}),_tag_dynforward(_tmp873,
sizeof(void*),0));});{unsigned int _tmp876;int _tmp877;struct _tuple7 _tmp875=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp869);_tmp876=_tmp875.f1;_tmp877=_tmp875.f2;if((
is_zero_terminated  && _tmp877) && _tmp876 < 1)({void*_tmp878[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp879="zero terminated array cannot have zero elements";
_tag_dynforward(_tmp879,sizeof(char),_get_zero_arr_size(_tmp879,48));}),
_tag_dynforward(_tmp878,sizeof(void*),0));});goto _LL4DB;}}}_LL4F8: if(*((int*)
_tmp796)!= 8)goto _LL4FA;_tmp7BE=((struct Cyc_Absyn_FnType_struct*)_tmp796)->f1;
_tmp7BF=_tmp7BE.tvars;_tmp7C0=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp796)->f1).tvars;_tmp7C1=_tmp7BE.effect;_tmp7C2=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp796)->f1).effect;_tmp7C3=(void*)_tmp7BE.ret_typ;
_tmp7C4=_tmp7BE.args;_tmp7C5=_tmp7BE.c_varargs;_tmp7C6=_tmp7BE.cyc_varargs;
_tmp7C7=_tmp7BE.rgn_po;_tmp7C8=_tmp7BE.attributes;_LL4F9: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7C8 != 0;_tmp7C8=_tmp7C8->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7C8->hd))({struct Cyc_String_pa_struct _tmp87C;
_tmp87C.tag=0;_tmp87C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)_tmp7C8->hd));{void*_tmp87A[1]={& _tmp87C};Cyc_Tcutil_terr(loc,({const char*
_tmp87B="bad function type attribute %s";_tag_dynforward(_tmp87B,sizeof(char),
_get_zero_arr_size(_tmp87B,31));}),_tag_dynforward(_tmp87A,sizeof(void*),1));}});{
void*_tmp87D=(void*)_tmp7C8->hd;void*_tmp87E;int _tmp87F;int _tmp880;_LL55B: if((
int)_tmp87D != 0)goto _LL55D;_LL55C: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL55A;_LL55D: if((int)_tmp87D != 1)goto _LL55F;_LL55E: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL55A;_LL55F: if((int)_tmp87D != 2)goto _LL561;
_LL560: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL55A;_LL561: if(
_tmp87D <= (void*)17)goto _LL563;if(*((int*)_tmp87D)!= 3)goto _LL563;_tmp87E=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp87D)->f1;_tmp87F=((struct Cyc_Absyn_Format_att_struct*)
_tmp87D)->f2;_tmp880=((struct Cyc_Absyn_Format_att_struct*)_tmp87D)->f3;_LL562:
if(!seen_format){seen_format=1;ft=_tmp87E;fmt_desc_arg=_tmp87F;fmt_arg_start=
_tmp880;}else{({void*_tmp881[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp882="function can't have multiple format attributes";
_tag_dynforward(_tmp882,sizeof(char),_get_zero_arr_size(_tmp882,47));}),
_tag_dynforward(_tmp881,sizeof(void*),0));});}goto _LL55A;_LL563:;_LL564: goto
_LL55A;_LL55A:;}}if(num_convs > 1)({void*_tmp883[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp884="function can't have multiple calling conventions";
_tag_dynforward(_tmp884,sizeof(char),_get_zero_arr_size(_tmp884,49));}),
_tag_dynforward(_tmp883,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*
_tmp7C0);{struct Cyc_List_List*b=*_tmp7C0;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp885=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp886;_LL566: if(*((int*)
_tmp885)!= 0)goto _LL568;_tmp886=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp885)->f1;
if((int)_tmp886 != 1)goto _LL568;_LL567:({struct Cyc_String_pa_struct _tmp889;
_tmp889.tag=0;_tmp889.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp887[1]={& _tmp889};Cyc_Tcutil_terr(
loc,({const char*_tmp888="function attempts to abstract Mem type variable %s";
_tag_dynforward(_tmp888,sizeof(char),_get_zero_arr_size(_tmp888,51));}),
_tag_dynforward(_tmp887,sizeof(void*),1));}});goto _LL565;_LL568:;_LL569: goto
_LL565;_LL565:;}}}{struct Cyc_Tcutil_CVTEnv _tmp88A=({struct Cyc_Tcutil_CVTEnv
_tmp915;_tmp915.kind_env=cvtenv.kind_env;_tmp915.free_vars=0;_tmp915.free_evars=
0;_tmp915.generalize_evars=cvtenv.generalize_evars;_tmp915.fn_result=1;_tmp915;});
_tmp88A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp88A,(void*)1,_tmp7C3,1,1);
_tmp88A.fn_result=0;{struct Cyc_List_List*a=_tmp7C4;for(0;a != 0;a=a->tl){struct
_tuple3*_tmp88B=(struct _tuple3*)a->hd;void*_tmp88C=(*_tmp88B).f3;_tmp88A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp88A,(void*)1,_tmp88C,1,1);((*_tmp88B).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp88B).f2).print_const,_tmp88C);}}if(_tmp7C6 != 0){if(_tmp7C5)({void*
_tmp88D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp88E="both c_vararg and cyc_vararg";_tag_dynforward(_tmp88E,sizeof(
char),_get_zero_arr_size(_tmp88E,29));}),_tag_dynforward(_tmp88D,sizeof(void*),0));});{
void*_tmp890;int _tmp891;struct Cyc_Absyn_VarargInfo _tmp88F=*_tmp7C6;_tmp890=(void*)
_tmp88F.type;_tmp891=_tmp88F.inject;_tmp88A=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp88A,(void*)1,_tmp890,1,1);(_tmp7C6->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7C6->tq).print_const,_tmp890);if(_tmp891){void*_tmp892=Cyc_Tcutil_compress(
_tmp890);struct Cyc_Absyn_TunionInfo _tmp893;union Cyc_Absyn_TunionInfoU_union
_tmp894;struct Cyc_Absyn_Tuniondecl**_tmp895;_LL56B: if(_tmp892 <= (void*)4)goto
_LL56D;if(*((int*)_tmp892)!= 2)goto _LL56D;_tmp893=((struct Cyc_Absyn_TunionType_struct*)
_tmp892)->f1;_tmp894=_tmp893.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp892)->f1).tunion_info).KnownTunion).tag != 1)goto _LL56D;_tmp895=(_tmp894.KnownTunion).f1;
_LL56C: if((*_tmp895)->is_flat)({void*_tmp896[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp897="cant inject into a flat tunion";_tag_dynforward(_tmp897,sizeof(
char),_get_zero_arr_size(_tmp897,31));}),_tag_dynforward(_tmp896,sizeof(void*),0));});
goto _LL56A;_LL56D:;_LL56E:({void*_tmp898[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp899="can't inject a non-[x]tunion type";_tag_dynforward(_tmp899,sizeof(char),
_get_zero_arr_size(_tmp899,34));}),_tag_dynforward(_tmp898,sizeof(void*),0));});
goto _LL56A;_LL56A:;}}}if(seen_format){int _tmp89A=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp7C4);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp89A) || 
fmt_arg_start < 0) || _tmp7C6 == 0  && fmt_arg_start != 0) || _tmp7C6 != 0  && 
fmt_arg_start != _tmp89A + 1)({void*_tmp89B[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp89C="bad format descriptor";_tag_dynforward(_tmp89C,sizeof(char),
_get_zero_arr_size(_tmp89C,22));}),_tag_dynforward(_tmp89B,sizeof(void*),0));});
else{void*_tmp89E;struct _tuple3 _tmp89D=*((struct _tuple3*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp7C4,fmt_desc_arg - 1);_tmp89E=_tmp89D.f3;{void*_tmp89F=
Cyc_Tcutil_compress(_tmp89E);struct Cyc_Absyn_PtrInfo _tmp8A0;void*_tmp8A1;struct
Cyc_Absyn_PtrAtts _tmp8A2;struct Cyc_Absyn_Conref*_tmp8A3;struct Cyc_Absyn_Conref*
_tmp8A4;_LL570: if(_tmp89F <= (void*)4)goto _LL572;if(*((int*)_tmp89F)!= 4)goto
_LL572;_tmp8A0=((struct Cyc_Absyn_PointerType_struct*)_tmp89F)->f1;_tmp8A1=(void*)
_tmp8A0.elt_typ;_tmp8A2=_tmp8A0.ptr_atts;_tmp8A3=_tmp8A2.bounds;_tmp8A4=_tmp8A2.zero_term;
_LL571:{struct _tuple0 _tmp8A6=({struct _tuple0 _tmp8A5;_tmp8A5.f1=Cyc_Tcutil_compress(
_tmp8A1);_tmp8A5.f2=Cyc_Absyn_conref_def((void*)((void*)0),_tmp8A3);_tmp8A5;});
void*_tmp8A7;void*_tmp8A8;void*_tmp8A9;void*_tmp8AA;_LL575: _tmp8A7=_tmp8A6.f1;
if(_tmp8A7 <= (void*)4)goto _LL577;if(*((int*)_tmp8A7)!= 5)goto _LL577;_tmp8A8=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp8A7)->f1;if((int)_tmp8A8 != 2)goto
_LL577;_tmp8A9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp8A7)->f2;if((int)
_tmp8A9 != 0)goto _LL577;_tmp8AA=_tmp8A6.f2;if((int)_tmp8AA != 0)goto _LL577;_LL576:
goto _LL574;_LL577:;_LL578:({void*_tmp8AB[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp8AC="format descriptor is not a char ? type";_tag_dynforward(_tmp8AC,sizeof(
char),_get_zero_arr_size(_tmp8AC,39));}),_tag_dynforward(_tmp8AB,sizeof(void*),0));});
goto _LL574;_LL574:;}goto _LL56F;_LL572:;_LL573:({void*_tmp8AD[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8AE="format descriptor is not a char ? type";_tag_dynforward(
_tmp8AE,sizeof(char),_get_zero_arr_size(_tmp8AE,39));}),_tag_dynforward(_tmp8AD,
sizeof(void*),0));});goto _LL56F;_LL56F:;}if(fmt_arg_start != 0){int problem;{
struct _tuple0 _tmp8B0=({struct _tuple0 _tmp8AF;_tmp8AF.f1=ft;_tmp8AF.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7C6))->type);_tmp8AF;});void*
_tmp8B1;void*_tmp8B2;struct Cyc_Absyn_TunionInfo _tmp8B3;union Cyc_Absyn_TunionInfoU_union
_tmp8B4;struct Cyc_Absyn_Tuniondecl**_tmp8B5;struct Cyc_Absyn_Tuniondecl*_tmp8B6;
void*_tmp8B7;void*_tmp8B8;struct Cyc_Absyn_TunionInfo _tmp8B9;union Cyc_Absyn_TunionInfoU_union
_tmp8BA;struct Cyc_Absyn_Tuniondecl**_tmp8BB;struct Cyc_Absyn_Tuniondecl*_tmp8BC;
_LL57A: _tmp8B1=_tmp8B0.f1;if((int)_tmp8B1 != 0)goto _LL57C;_tmp8B2=_tmp8B0.f2;if(
_tmp8B2 <= (void*)4)goto _LL57C;if(*((int*)_tmp8B2)!= 2)goto _LL57C;_tmp8B3=((
struct Cyc_Absyn_TunionType_struct*)_tmp8B2)->f1;_tmp8B4=_tmp8B3.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8B2)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL57C;_tmp8B5=(_tmp8B4.KnownTunion).f1;_tmp8B6=*_tmp8B5;_LL57B: problem=
Cyc_Absyn_qvar_cmp(_tmp8B6->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL579;
_LL57C: _tmp8B7=_tmp8B0.f1;if((int)_tmp8B7 != 1)goto _LL57E;_tmp8B8=_tmp8B0.f2;if(
_tmp8B8 <= (void*)4)goto _LL57E;if(*((int*)_tmp8B8)!= 2)goto _LL57E;_tmp8B9=((
struct Cyc_Absyn_TunionType_struct*)_tmp8B8)->f1;_tmp8BA=_tmp8B9.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8B8)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL57E;_tmp8BB=(_tmp8BA.KnownTunion).f1;_tmp8BC=*_tmp8BB;_LL57D: problem=
Cyc_Absyn_qvar_cmp(_tmp8BC->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL579;
_LL57E:;_LL57F: problem=1;goto _LL579;_LL579:;}if(problem)({void*_tmp8BD[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8BE="format attribute and vararg types don't match";
_tag_dynforward(_tmp8BE,sizeof(char),_get_zero_arr_size(_tmp8BE,46));}),
_tag_dynforward(_tmp8BD,sizeof(void*),0));});}}}{struct Cyc_List_List*rpo=_tmp7C7;
for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8C0;void*_tmp8C1;void*_tmp8C2;struct
_tuple0*_tmp8BF=(struct _tuple0*)rpo->hd;_tmp8C0=*_tmp8BF;_tmp8C1=_tmp8C0.f1;
_tmp8C2=_tmp8C0.f2;_tmp88A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp88A,(void*)4,
_tmp8C1,0,1);_tmp88A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp88A,(void*)3,
_tmp8C2,0,1);}}if(*_tmp7C2 != 0)_tmp88A=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp88A,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7C2))->v,0,1);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp88A.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8C4;int _tmp8C5;struct
_tuple18 _tmp8C3=*((struct _tuple18*)tvs->hd);_tmp8C4=_tmp8C3.f1;_tmp8C5=_tmp8C3.f2;
if(!_tmp8C5)continue;{void*_tmp8C6=Cyc_Absyn_compress_kb((void*)_tmp8C4->kind);
struct Cyc_Core_Opt*_tmp8C7;struct Cyc_Core_Opt**_tmp8C8;void*_tmp8C9;void*_tmp8CA;
void*_tmp8CB;void*_tmp8CC;struct Cyc_Core_Opt*_tmp8CD;struct Cyc_Core_Opt**_tmp8CE;
void*_tmp8CF;void*_tmp8D0;struct Cyc_Core_Opt*_tmp8D1;struct Cyc_Core_Opt**_tmp8D2;
_LL581: if(*((int*)_tmp8C6)!= 2)goto _LL583;_tmp8C7=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8C6)->f1;_tmp8C8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8C6)->f1;_tmp8C9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8C6)->f2;if((
int)_tmp8C9 != 3)goto _LL583;_LL582:*_tmp8C8=({struct Cyc_Core_Opt*_tmp8D3=
_cycalloc(sizeof(*_tmp8D3));_tmp8D3->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8D4=_cycalloc(sizeof(*_tmp8D4));_tmp8D4[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8D5;_tmp8D5.tag=0;_tmp8D5.f1=(void*)((void*)3);_tmp8D5;});_tmp8D4;}));
_tmp8D3;});goto _LL584;_LL583: if(*((int*)_tmp8C6)!= 0)goto _LL585;_tmp8CA=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8C6)->f1;if((int)_tmp8CA != 3)goto _LL585;_LL584:
effect=({struct Cyc_List_List*_tmp8D6=_cycalloc(sizeof(*_tmp8D6));_tmp8D6->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp8D7=_cycalloc(sizeof(*
_tmp8D7));_tmp8D7[0]=({struct Cyc_Absyn_AccessEff_struct _tmp8D8;_tmp8D8.tag=20;
_tmp8D8.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8D9=_cycalloc(
sizeof(*_tmp8D9));_tmp8D9[0]=({struct Cyc_Absyn_VarType_struct _tmp8DA;_tmp8DA.tag=
1;_tmp8DA.f1=_tmp8C4;_tmp8DA;});_tmp8D9;}));_tmp8D8;});_tmp8D7;}));_tmp8D6->tl=
effect;_tmp8D6;});goto _LL580;_LL585: if(*((int*)_tmp8C6)!= 2)goto _LL587;_tmp8CB=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8C6)->f2;if((int)_tmp8CB != 5)goto
_LL587;_LL586: goto _LL588;_LL587: if(*((int*)_tmp8C6)!= 0)goto _LL589;_tmp8CC=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8C6)->f1;if((int)_tmp8CC != 5)goto _LL589;_LL588:
goto _LL580;_LL589: if(*((int*)_tmp8C6)!= 2)goto _LL58B;_tmp8CD=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8C6)->f1;_tmp8CE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8C6)->f1;_tmp8CF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8C6)->f2;if((
int)_tmp8CF != 4)goto _LL58B;_LL58A:*_tmp8CE=({struct Cyc_Core_Opt*_tmp8DB=
_cycalloc(sizeof(*_tmp8DB));_tmp8DB->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8DC=_cycalloc(sizeof(*_tmp8DC));_tmp8DC[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8DD;_tmp8DD.tag=0;_tmp8DD.f1=(void*)((void*)4);_tmp8DD;});_tmp8DC;}));
_tmp8DB;});goto _LL58C;_LL58B: if(*((int*)_tmp8C6)!= 0)goto _LL58D;_tmp8D0=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8C6)->f1;if((int)_tmp8D0 != 4)goto _LL58D;_LL58C:
effect=({struct Cyc_List_List*_tmp8DE=_cycalloc(sizeof(*_tmp8DE));_tmp8DE->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8DF=_cycalloc(sizeof(*_tmp8DF));
_tmp8DF[0]=({struct Cyc_Absyn_VarType_struct _tmp8E0;_tmp8E0.tag=1;_tmp8E0.f1=
_tmp8C4;_tmp8E0;});_tmp8DF;}));_tmp8DE->tl=effect;_tmp8DE;});goto _LL580;_LL58D:
if(*((int*)_tmp8C6)!= 1)goto _LL58F;_tmp8D1=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8C6)->f1;_tmp8D2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8C6)->f1;_LL58E:*_tmp8D2=({struct Cyc_Core_Opt*_tmp8E1=_cycalloc(sizeof(*
_tmp8E1));_tmp8E1->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp8E2=
_cycalloc(sizeof(*_tmp8E2));_tmp8E2[0]=({struct Cyc_Absyn_Less_kb_struct _tmp8E3;
_tmp8E3.tag=2;_tmp8E3.f1=0;_tmp8E3.f2=(void*)((void*)0);_tmp8E3;});_tmp8E2;}));
_tmp8E1;});goto _LL590;_LL58F:;_LL590: effect=({struct Cyc_List_List*_tmp8E4=
_cycalloc(sizeof(*_tmp8E4));_tmp8E4->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8E5=_cycalloc(sizeof(*_tmp8E5));_tmp8E5[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8E6;_tmp8E6.tag=22;_tmp8E6.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8E7=_cycalloc(sizeof(*_tmp8E7));_tmp8E7[0]=({struct Cyc_Absyn_VarType_struct
_tmp8E8;_tmp8E8.tag=1;_tmp8E8.f1=_tmp8C4;_tmp8E8;});_tmp8E7;}));_tmp8E6;});
_tmp8E5;}));_tmp8E4->tl=effect;_tmp8E4;});goto _LL580;_LL580:;}}}{struct Cyc_List_List*
ts=_tmp88A.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8EA;int _tmp8EB;struct
_tuple19 _tmp8E9=*((struct _tuple19*)ts->hd);_tmp8EA=_tmp8E9.f1;_tmp8EB=_tmp8E9.f2;
if(!_tmp8EB)continue;{void*_tmp8EC=Cyc_Tcutil_typ_kind(_tmp8EA);_LL592: if((int)
_tmp8EC != 3)goto _LL594;_LL593: effect=({struct Cyc_List_List*_tmp8ED=_cycalloc(
sizeof(*_tmp8ED));_tmp8ED->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp8EE=_cycalloc(sizeof(*_tmp8EE));_tmp8EE[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp8EF;_tmp8EF.tag=20;_tmp8EF.f1=(void*)_tmp8EA;_tmp8EF;});_tmp8EE;}));_tmp8ED->tl=
effect;_tmp8ED;});goto _LL591;_LL594: if((int)_tmp8EC != 4)goto _LL596;_LL595: effect=({
struct Cyc_List_List*_tmp8F0=_cycalloc(sizeof(*_tmp8F0));_tmp8F0->hd=(void*)
_tmp8EA;_tmp8F0->tl=effect;_tmp8F0;});goto _LL591;_LL596: if((int)_tmp8EC != 5)goto
_LL598;_LL597: goto _LL591;_LL598:;_LL599: effect=({struct Cyc_List_List*_tmp8F1=
_cycalloc(sizeof(*_tmp8F1));_tmp8F1->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8F2=_cycalloc(sizeof(*_tmp8F2));_tmp8F2[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8F3;_tmp8F3.tag=22;_tmp8F3.f1=(void*)_tmp8EA;_tmp8F3;});_tmp8F2;}));_tmp8F1->tl=
effect;_tmp8F1;});goto _LL591;_LL591:;}}}*_tmp7C2=({struct Cyc_Core_Opt*_tmp8F4=
_cycalloc(sizeof(*_tmp8F4));_tmp8F4->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp8F6;_tmp8F6.tag=21;_tmp8F6.f1=effect;_tmp8F6;});_tmp8F5;}));_tmp8F4;});}if(*
_tmp7C0 != 0){struct Cyc_List_List*bs=*_tmp7C0;for(0;bs != 0;bs=bs->tl){void*
_tmp8F7=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp8F8;struct Cyc_Core_Opt**_tmp8F9;struct Cyc_Core_Opt*
_tmp8FA;struct Cyc_Core_Opt**_tmp8FB;void*_tmp8FC;struct Cyc_Core_Opt*_tmp8FD;
struct Cyc_Core_Opt**_tmp8FE;void*_tmp8FF;struct Cyc_Core_Opt*_tmp900;struct Cyc_Core_Opt**
_tmp901;void*_tmp902;void*_tmp903;_LL59B: if(*((int*)_tmp8F7)!= 1)goto _LL59D;
_tmp8F8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8F7)->f1;_tmp8F9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8F7)->f1;_LL59C:({struct Cyc_String_pa_struct
_tmp906;_tmp906.tag=0;_tmp906.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp904[1]={& _tmp906};Cyc_Tcutil_warn(
loc,({const char*_tmp905="Type variable %s unconstrained, assuming boxed";
_tag_dynforward(_tmp905,sizeof(char),_get_zero_arr_size(_tmp905,47));}),
_tag_dynforward(_tmp904,sizeof(void*),1));}});_tmp8FB=_tmp8F9;goto _LL59E;_LL59D:
if(*((int*)_tmp8F7)!= 2)goto _LL59F;_tmp8FA=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8F7)->f1;_tmp8FB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8F7)->f1;_tmp8FC=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8F7)->f2;if((
int)_tmp8FC != 1)goto _LL59F;_LL59E: _tmp8FE=_tmp8FB;goto _LL5A0;_LL59F: if(*((int*)
_tmp8F7)!= 2)goto _LL5A1;_tmp8FD=((struct Cyc_Absyn_Less_kb_struct*)_tmp8F7)->f1;
_tmp8FE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8F7)->f1;
_tmp8FF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8F7)->f2;if((int)_tmp8FF != 
0)goto _LL5A1;_LL5A0:*_tmp8FE=({struct Cyc_Core_Opt*_tmp907=_cycalloc(sizeof(*
_tmp907));_tmp907->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp908=
_cycalloc(sizeof(*_tmp908));_tmp908[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp909;
_tmp909.tag=0;_tmp909.f1=(void*)((void*)2);_tmp909;});_tmp908;}));_tmp907;});
goto _LL59A;_LL5A1: if(*((int*)_tmp8F7)!= 2)goto _LL5A3;_tmp900=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8F7)->f1;_tmp901=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8F7)->f1;_tmp902=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8F7)->f2;
_LL5A2:*_tmp901=({struct Cyc_Core_Opt*_tmp90A=_cycalloc(sizeof(*_tmp90A));_tmp90A->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp90B=_cycalloc(sizeof(*_tmp90B));
_tmp90B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp90C;_tmp90C.tag=0;_tmp90C.f1=(void*)
_tmp902;_tmp90C;});_tmp90B;}));_tmp90A;});goto _LL59A;_LL5A3: if(*((int*)_tmp8F7)
!= 0)goto _LL5A5;_tmp903=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8F7)->f1;if((
int)_tmp903 != 1)goto _LL5A5;_LL5A4:({void*_tmp90D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp90E="functions can't abstract M types";_tag_dynforward(_tmp90E,
sizeof(char),_get_zero_arr_size(_tmp90E,33));}),_tag_dynforward(_tmp90D,sizeof(
void*),0));});goto _LL59A;_LL5A5:;_LL5A6: goto _LL59A;_LL59A:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp88A.kind_env,*_tmp7C0);_tmp88A.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp88A.free_vars,*_tmp7C0);{struct Cyc_List_List*tvs=_tmp88A.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp910;int _tmp911;struct _tuple18 _tmp90F=*((
struct _tuple18*)tvs->hd);_tmp910=_tmp90F.f1;_tmp911=_tmp90F.f2;cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.free_vars,_tmp910,_tmp911);}}{struct
Cyc_List_List*evs=_tmp88A.free_evars;for(0;evs != 0;evs=evs->tl){void*_tmp913;int
_tmp914;struct _tuple19 _tmp912=*((struct _tuple19*)evs->hd);_tmp913=_tmp912.f1;
_tmp914=_tmp912.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,
_tmp913,_tmp914);}}goto _LL4DB;}}_LL4FA: if(*((int*)_tmp796)!= 9)goto _LL4FC;
_tmp7C9=((struct Cyc_Absyn_TupleType_struct*)_tmp796)->f1;_LL4FB: for(0;_tmp7C9 != 
0;_tmp7C9=_tmp7C9->tl){struct _tuple5*_tmp916=(struct _tuple5*)_tmp7C9->hd;cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*_tmp916).f2,0,1);((*
_tmp916).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp916).f1).print_const,(*
_tmp916).f2);}goto _LL4DB;_LL4FC: if(*((int*)_tmp796)!= 11)goto _LL4FE;_tmp7CA=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp796)->f1;_tmp7CB=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp796)->f2;_LL4FD:{struct _RegionHandle _tmp917=_new_region("aprev_rgn");struct
_RegionHandle*aprev_rgn=& _tmp917;_push_region(aprev_rgn);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp7CB != 0;_tmp7CB=_tmp7CB->tl){struct Cyc_Absyn_Aggrfield
_tmp919;struct _dynforward_ptr*_tmp91A;struct Cyc_Absyn_Tqual _tmp91B;struct Cyc_Absyn_Tqual*
_tmp91C;void*_tmp91D;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_List_List*_tmp91F;
struct Cyc_Absyn_Aggrfield*_tmp918=(struct Cyc_Absyn_Aggrfield*)_tmp7CB->hd;
_tmp919=*_tmp918;_tmp91A=_tmp919.name;_tmp91B=_tmp919.tq;_tmp91C=(struct Cyc_Absyn_Tqual*)&(*
_tmp918).tq;_tmp91D=(void*)_tmp919.type;_tmp91E=_tmp919.width;_tmp91F=_tmp919.attributes;
if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct
Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,
_tmp91A))({struct Cyc_String_pa_struct _tmp922;_tmp922.tag=0;_tmp922.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp91A);{void*_tmp920[1]={& _tmp922};
Cyc_Tcutil_terr(loc,({const char*_tmp921="duplicate field %s";_tag_dynforward(
_tmp921,sizeof(char),_get_zero_arr_size(_tmp921,19));}),_tag_dynforward(_tmp920,
sizeof(void*),1));}});if(Cyc_strcmp((struct _dynforward_ptr)*_tmp91A,({const char*
_tmp923="";_tag_dynforward(_tmp923,sizeof(char),_get_zero_arr_size(_tmp923,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp924=_region_malloc(aprev_rgn,sizeof(*
_tmp924));_tmp924->hd=_tmp91A;_tmp924->tl=prev_fields;_tmp924;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp91D,0,1);_tmp91C->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp91C->print_const,_tmp91D);if(_tmp7CA == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp91D))({struct Cyc_String_pa_struct _tmp927;_tmp927.tag=0;_tmp927.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp91A);{void*_tmp925[1]={& _tmp927};
Cyc_Tcutil_warn(loc,({const char*_tmp926="union member %s is not `bits-only' so it can only be written and not read";
_tag_dynforward(_tmp926,sizeof(char),_get_zero_arr_size(_tmp926,74));}),
_tag_dynforward(_tmp925,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,
_tmp91D,_tmp91E,_tmp91A);Cyc_Tcutil_check_field_atts(loc,_tmp91A,_tmp91F);}};
_pop_region(aprev_rgn);}goto _LL4DB;_LL4FE: if(*((int*)_tmp796)!= 10)goto _LL500;
_tmp7CC=((struct Cyc_Absyn_AggrType_struct*)_tmp796)->f1;_tmp7CD=_tmp7CC.aggr_info;
_tmp7CE=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp796)->f1).aggr_info;_tmp7CF=_tmp7CC.targs;_tmp7D0=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp796)->f1).targs;_LL4FF:{union Cyc_Absyn_AggrInfoU_union
_tmp928=*_tmp7CE;void*_tmp929;struct _tuple2*_tmp92A;struct Cyc_Absyn_Aggrdecl**
_tmp92B;struct Cyc_Absyn_Aggrdecl*_tmp92C;_LL5A8: if((_tmp928.UnknownAggr).tag != 0)
goto _LL5AA;_tmp929=(_tmp928.UnknownAggr).f1;_tmp92A=(_tmp928.UnknownAggr).f2;
_LL5A9: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp92D;_push_handler(&
_tmp92D);{int _tmp92F=0;if(setjmp(_tmp92D.handler))_tmp92F=1;if(!_tmp92F){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp92A);*_tmp7CE=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp930;(_tmp930.KnownAggr).tag=1;(_tmp930.KnownAggr).f1=adp;_tmp930;});;
_pop_handler();}else{void*_tmp92E=(void*)_exn_thrown;void*_tmp932=_tmp92E;_LL5AD:
if(_tmp932 != Cyc_Dict_Absent)goto _LL5AF;_LL5AE: {struct Cyc_Tcenv_Genv*_tmp933=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*_tmp934=({struct Cyc_Absyn_Aggrdecl*
_tmp939=_cycalloc(sizeof(*_tmp939));_tmp939->kind=(void*)_tmp929;_tmp939->sc=(
void*)((void*)3);_tmp939->name=_tmp92A;_tmp939->tvs=0;_tmp939->impl=0;_tmp939->attributes=
0;_tmp939;});Cyc_Tc_tcAggrdecl(te,_tmp933,loc,_tmp934);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp92A);*_tmp7CE=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp935;(_tmp935.KnownAggr).tag=1;(_tmp935.KnownAggr).f1=adp;_tmp935;});if(*
_tmp7D0 != 0){({struct Cyc_String_pa_struct _tmp938;_tmp938.tag=0;_tmp938.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp92A));{
void*_tmp936[1]={& _tmp938};Cyc_Tcutil_terr(loc,({const char*_tmp937="declare parameterized type %s before using";
_tag_dynforward(_tmp937,sizeof(char),_get_zero_arr_size(_tmp937,43));}),
_tag_dynforward(_tmp936,sizeof(void*),1));}});return cvtenv;}goto _LL5AC;}_LL5AF:;
_LL5B0:(void)_throw(_tmp932);_LL5AC:;}}}_tmp92C=*adp;goto _LL5AB;}_LL5AA: if((
_tmp928.KnownAggr).tag != 1)goto _LL5A7;_tmp92B=(_tmp928.KnownAggr).f1;_tmp92C=*
_tmp92B;_LL5AB: {struct Cyc_List_List*tvs=_tmp92C->tvs;struct Cyc_List_List*ts=*
_tmp7D0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,0,1);}if(ts != 0)({struct Cyc_String_pa_struct _tmp93C;
_tmp93C.tag=0;_tmp93C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp92C->name));{void*_tmp93A[1]={& _tmp93C};Cyc_Tcutil_terr(loc,({const char*
_tmp93B="too many parameters for type %s";_tag_dynforward(_tmp93B,sizeof(char),
_get_zero_arr_size(_tmp93B,32));}),_tag_dynforward(_tmp93A,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp93D=_cycalloc(sizeof(*_tmp93D));_tmp93D->hd=(
void*)e;_tmp93D->tl=hidden_ts;_tmp93D;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,0,1);}*_tmp7D0=Cyc_List_imp_append(*_tmp7D0,Cyc_List_imp_rev(
hidden_ts));}}_LL5A7:;}goto _LL4DB;_LL500: if(*((int*)_tmp796)!= 17)goto _LL502;
_tmp7D1=((struct Cyc_Absyn_TypedefType_struct*)_tmp796)->f1;_tmp7D2=((struct Cyc_Absyn_TypedefType_struct*)
_tmp796)->f2;_tmp7D3=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp796)->f2;_tmp7D4=((struct Cyc_Absyn_TypedefType_struct*)_tmp796)->f3;_tmp7D5=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp796)->f3;
_tmp7D6=((struct Cyc_Absyn_TypedefType_struct*)_tmp796)->f4;_tmp7D7=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp796)->f4;_LL501: {struct Cyc_List_List*
targs=*_tmp7D3;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp93E;
_push_handler(& _tmp93E);{int _tmp940=0;if(setjmp(_tmp93E.handler))_tmp940=1;if(!
_tmp940){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7D1);;_pop_handler();}else{
void*_tmp93F=(void*)_exn_thrown;void*_tmp942=_tmp93F;_LL5B2: if(_tmp942 != Cyc_Dict_Absent)
goto _LL5B4;_LL5B3:({struct Cyc_String_pa_struct _tmp945;_tmp945.tag=0;_tmp945.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D1));{
void*_tmp943[1]={& _tmp945};Cyc_Tcutil_terr(loc,({const char*_tmp944="unbound typedef name %s";
_tag_dynforward(_tmp944,sizeof(char),_get_zero_arr_size(_tmp944,24));}),
_tag_dynforward(_tmp943,sizeof(void*),1));}});return cvtenv;_LL5B4:;_LL5B5:(void)
_throw(_tmp942);_LL5B1:;}}}*_tmp7D5=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7D1[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(
ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,0,1);inst=({
struct Cyc_List_List*_tmp946=_cycalloc(sizeof(*_tmp946));_tmp946->hd=({struct
_tuple8*_tmp947=_cycalloc(sizeof(*_tmp947));_tmp947->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp947->f2=(void*)ts->hd;_tmp947;});_tmp946->tl=inst;_tmp946;});}if(ts
!= 0)({struct Cyc_String_pa_struct _tmp94A;_tmp94A.tag=0;_tmp94A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D1));{void*
_tmp948[1]={& _tmp94A};Cyc_Tcutil_terr(loc,({const char*_tmp949="too many parameters for typedef %s";
_tag_dynforward(_tmp949,sizeof(char),_get_zero_arr_size(_tmp949,35));}),
_tag_dynforward(_tmp948,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp94B=
_cycalloc(sizeof(*_tmp94B));_tmp94B->hd=(void*)e;_tmp94B->tl=hidden_ts;_tmp94B;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,0,1);inst=({struct Cyc_List_List*
_tmp94C=_cycalloc(sizeof(*_tmp94C));_tmp94C->hd=({struct _tuple8*_tmp94D=
_cycalloc(sizeof(*_tmp94D));_tmp94D->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp94D->f2=
e;_tmp94D;});_tmp94C->tl=inst;_tmp94C;});}*_tmp7D3=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp7D7=({void**_tmp94E=
_cycalloc(sizeof(*_tmp94E));_tmp94E[0]=new_typ;_tmp94E;});}goto _LL4DB;}}_LL502:
if((int)_tmp796 != 3)goto _LL504;_LL503: if(!allow_unique)({void*_tmp94F[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp950="`U only allowed in outermost pointers";_tag_dynforward(
_tmp950,sizeof(char),_get_zero_arr_size(_tmp950,38));}),_tag_dynforward(_tmp94F,
sizeof(void*),0));});goto _LL4DB;_LL504: if(_tmp796 <= (void*)4)goto _LL506;if(*((
int*)_tmp796)!= 19)goto _LL506;_LL505: goto _LL507;_LL506: if((int)_tmp796 != 2)goto
_LL508;_LL507: goto _LL4DB;_LL508: if(_tmp796 <= (void*)4)goto _LL50A;if(*((int*)
_tmp796)!= 15)goto _LL50A;_tmp7D8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp796)->f1;_LL509: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp7D8,1,1);goto _LL4DB;_LL50A: if(_tmp796 <= (void*)4)goto _LL50C;if(*((int*)
_tmp796)!= 16)goto _LL50C;_tmp7D9=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp796)->f1;_tmp7DA=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp796)->f2;
_LL50B: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7D9,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7DA,0,1);goto
_LL4DB;_LL50C: if(_tmp796 <= (void*)4)goto _LL50E;if(*((int*)_tmp796)!= 20)goto
_LL50E;_tmp7DB=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp796)->f1;_LL50D:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7DB,0,1);goto
_LL4DB;_LL50E: if(_tmp796 <= (void*)4)goto _LL510;if(*((int*)_tmp796)!= 22)goto
_LL510;_tmp7DC=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp796)->f1;_LL50F:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7DC,0,1);goto
_LL4DB;_LL510: if(_tmp796 <= (void*)4)goto _LL4DB;if(*((int*)_tmp796)!= 21)goto
_LL4DB;_tmp7DD=((struct Cyc_Absyn_JoinEff_struct*)_tmp796)->f1;_LL511: for(0;
_tmp7DD != 0;_tmp7DD=_tmp7DD->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)4,(void*)_tmp7DD->hd,0,1);}goto _LL4DB;_LL4DB:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind))({struct Cyc_String_pa_struct _tmp955;
_tmp955.tag=0;_tmp955.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
expected_kind));{struct Cyc_String_pa_struct _tmp954;_tmp954.tag=0;_tmp954.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t)));{struct Cyc_String_pa_struct _tmp953;_tmp953.tag=0;_tmp953.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp951[
3]={& _tmp953,& _tmp954,& _tmp955};Cyc_Tcutil_terr(loc,({const char*_tmp952="type %s has kind %s but as used here needs kind %s";
_tag_dynforward(_tmp952,sizeof(char),_get_zero_arr_size(_tmp952,51));}),
_tag_dynforward(_tmp951,sizeof(void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*kind_env,void*expected_kind,int generalize_evars,void*t,
int allow_unique){struct Cyc_Tcutil_CVTEnv _tmp956=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp963;_tmp963.kind_env=kind_env;_tmp963.free_vars=
0;_tmp963.free_evars=0;_tmp963.generalize_evars=generalize_evars;_tmp963.fn_result=
0;_tmp963;}),expected_kind,t,allow_unique,1);{struct Cyc_List_List*vs=_tmp956.free_vars;
for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmp958;struct _tuple18 _tmp957=*((
struct _tuple18*)vs->hd);_tmp958=_tmp957.f1;_tmp956.kind_env=Cyc_Tcutil_fast_add_free_tvar(
kind_env,_tmp958);}}{struct Cyc_List_List*evs=_tmp956.free_evars;for(0;evs != 0;
evs=evs->tl){void*_tmp95A;struct _tuple19 _tmp959=*((struct _tuple19*)evs->hd);
_tmp95A=_tmp959.f1;{void*_tmp95B=Cyc_Tcutil_compress(_tmp95A);struct Cyc_Core_Opt*
_tmp95C;struct Cyc_Core_Opt**_tmp95D;_LL5B7: if(_tmp95B <= (void*)4)goto _LL5B9;if(*((
int*)_tmp95B)!= 0)goto _LL5B9;_tmp95C=((struct Cyc_Absyn_Evar_struct*)_tmp95B)->f4;
_tmp95D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp95B)->f4;
_LL5B8: if(*_tmp95D == 0)*_tmp95D=({struct Cyc_Core_Opt*_tmp95E=_cycalloc(sizeof(*
_tmp95E));_tmp95E->v=kind_env;_tmp95E;});else{struct Cyc_List_List*_tmp95F=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp95D))->v;struct Cyc_List_List*
_tmp960=0;for(0;_tmp95F != 0;_tmp95F=_tmp95F->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp95F->hd))_tmp960=({struct Cyc_List_List*_tmp961=_cycalloc(sizeof(*_tmp961));
_tmp961->hd=(struct Cyc_Absyn_Tvar*)_tmp95F->hd;_tmp961->tl=_tmp960;_tmp961;});}*
_tmp95D=({struct Cyc_Core_Opt*_tmp962=_cycalloc(sizeof(*_tmp962));_tmp962->v=
_tmp960;_tmp962;});}goto _LL5B6;_LL5B9:;_LL5BA: goto _LL5B6;_LL5B6:;}}}return
_tmp956;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*t,int allow_unique){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmp964=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Tcutil_CVTEnv
_tmp965=Cyc_Tcutil_check_valid_type(loc,te,_tmp964,(void*)1,generalize_evars,t,
allow_unique);struct Cyc_List_List*_tmp966=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp965.free_vars);struct Cyc_List_List*_tmp967=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple19*))Cyc_Core_fst,_tmp965.free_evars);if(_tmp964 != 0){
struct Cyc_List_List*_tmp968=0;{struct Cyc_List_List*_tmp969=_tmp966;for(0;(
unsigned int)_tmp969;_tmp969=_tmp969->tl){struct Cyc_Absyn_Tvar*_tmp96A=(struct
Cyc_Absyn_Tvar*)_tmp969->hd;int found=0;{struct Cyc_List_List*_tmp96B=_tmp964;for(
0;(unsigned int)_tmp96B;_tmp96B=_tmp96B->tl){if(Cyc_Absyn_tvar_cmp(_tmp96A,(
struct Cyc_Absyn_Tvar*)_tmp96B->hd)== 0){found=1;break;}}}if(!found)_tmp968=({
struct Cyc_List_List*_tmp96C=_cycalloc(sizeof(*_tmp96C));_tmp96C->hd=(struct Cyc_Absyn_Tvar*)
_tmp969->hd;_tmp96C->tl=_tmp968;_tmp96C;});}}_tmp966=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp968);}{struct Cyc_List_List*x=
_tmp966;for(0;x != 0;x=x->tl){void*_tmp96D=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp96E;struct Cyc_Core_Opt**
_tmp96F;struct Cyc_Core_Opt*_tmp970;struct Cyc_Core_Opt**_tmp971;void*_tmp972;
struct Cyc_Core_Opt*_tmp973;struct Cyc_Core_Opt**_tmp974;void*_tmp975;struct Cyc_Core_Opt*
_tmp976;struct Cyc_Core_Opt**_tmp977;void*_tmp978;void*_tmp979;_LL5BC: if(*((int*)
_tmp96D)!= 1)goto _LL5BE;_tmp96E=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp96D)->f1;
_tmp96F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp96D)->f1;
_LL5BD: _tmp971=_tmp96F;goto _LL5BF;_LL5BE: if(*((int*)_tmp96D)!= 2)goto _LL5C0;
_tmp970=((struct Cyc_Absyn_Less_kb_struct*)_tmp96D)->f1;_tmp971=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp96D)->f1;_tmp972=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp96D)->f2;if((int)_tmp972 != 1)goto _LL5C0;_LL5BF: _tmp974=_tmp971;goto _LL5C1;
_LL5C0: if(*((int*)_tmp96D)!= 2)goto _LL5C2;_tmp973=((struct Cyc_Absyn_Less_kb_struct*)
_tmp96D)->f1;_tmp974=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp96D)->f1;_tmp975=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp96D)->f2;if((
int)_tmp975 != 0)goto _LL5C2;_LL5C1:*_tmp974=({struct Cyc_Core_Opt*_tmp97A=
_cycalloc(sizeof(*_tmp97A));_tmp97A->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp97B=_cycalloc(sizeof(*_tmp97B));_tmp97B[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp97C;_tmp97C.tag=0;_tmp97C.f1=(void*)((void*)2);_tmp97C;});_tmp97B;}));
_tmp97A;});goto _LL5BB;_LL5C2: if(*((int*)_tmp96D)!= 2)goto _LL5C4;_tmp976=((struct
Cyc_Absyn_Less_kb_struct*)_tmp96D)->f1;_tmp977=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp96D)->f1;_tmp978=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp96D)->f2;_LL5C3:*_tmp977=({struct Cyc_Core_Opt*_tmp97D=_cycalloc(sizeof(*
_tmp97D));_tmp97D->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp97E=
_cycalloc(sizeof(*_tmp97E));_tmp97E[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp97F;
_tmp97F.tag=0;_tmp97F.f1=(void*)_tmp978;_tmp97F;});_tmp97E;}));_tmp97D;});goto
_LL5BB;_LL5C4: if(*((int*)_tmp96D)!= 0)goto _LL5C6;_tmp979=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp96D)->f1;if((int)_tmp979 != 1)goto _LL5C6;_LL5C5:({struct Cyc_String_pa_struct
_tmp982;_tmp982.tag=0;_tmp982.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp980[1]={& _tmp982};
Cyc_Tcutil_terr(loc,({const char*_tmp981="type variable %s cannot have kind M";
_tag_dynforward(_tmp981,sizeof(char),_get_zero_arr_size(_tmp981,36));}),
_tag_dynforward(_tmp980,sizeof(void*),1));}});goto _LL5BB;_LL5C6:;_LL5C7: goto
_LL5BB;_LL5BB:;}}if(_tmp966 != 0  || _tmp967 != 0){{void*_tmp983=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp984;struct Cyc_List_List*_tmp985;struct Cyc_List_List**
_tmp986;struct Cyc_Core_Opt*_tmp987;void*_tmp988;struct Cyc_List_List*_tmp989;int
_tmp98A;struct Cyc_Absyn_VarargInfo*_tmp98B;struct Cyc_List_List*_tmp98C;struct Cyc_List_List*
_tmp98D;_LL5C9: if(_tmp983 <= (void*)4)goto _LL5CB;if(*((int*)_tmp983)!= 8)goto
_LL5CB;_tmp984=((struct Cyc_Absyn_FnType_struct*)_tmp983)->f1;_tmp985=_tmp984.tvars;
_tmp986=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp983)->f1).tvars;
_tmp987=_tmp984.effect;_tmp988=(void*)_tmp984.ret_typ;_tmp989=_tmp984.args;
_tmp98A=_tmp984.c_varargs;_tmp98B=_tmp984.cyc_varargs;_tmp98C=_tmp984.rgn_po;
_tmp98D=_tmp984.attributes;_LL5CA: if(*_tmp986 == 0){*_tmp986=_tmp966;_tmp966=0;}
goto _LL5C8;_LL5CB:;_LL5CC: goto _LL5C8;_LL5C8:;}if(_tmp966 != 0)({struct Cyc_String_pa_struct
_tmp990;_tmp990.tag=0;_tmp990.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)_tmp966->hd)->name);{void*_tmp98E[1]={& _tmp990};Cyc_Tcutil_terr(
loc,({const char*_tmp98F="unbound type variable %s";_tag_dynforward(_tmp98F,
sizeof(char),_get_zero_arr_size(_tmp98F,25));}),_tag_dynforward(_tmp98E,sizeof(
void*),1));}});if(_tmp967 != 0)for(0;_tmp967 != 0;_tmp967=_tmp967->tl){void*e=(
void*)_tmp967->hd;void*_tmp991=Cyc_Tcutil_typ_kind(e);_LL5CE: if((int)_tmp991 != 3)
goto _LL5D0;_LL5CF: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp992[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp993="can't unify evar with heap!";_tag_dynforward(_tmp993,sizeof(char),
_get_zero_arr_size(_tmp993,28));}),_tag_dynforward(_tmp992,sizeof(void*),0));});
goto _LL5CD;_LL5D0: if((int)_tmp991 != 4)goto _LL5D2;_LL5D1: if(!Cyc_Tcutil_unify(e,
Cyc_Absyn_empty_effect))({void*_tmp994[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp995="can't unify evar with {}!";
_tag_dynforward(_tmp995,sizeof(char),_get_zero_arr_size(_tmp995,26));}),
_tag_dynforward(_tmp994,sizeof(void*),0));});goto _LL5CD;_LL5D2:;_LL5D3:({struct
Cyc_String_pa_struct _tmp999;_tmp999.tag=0;_tmp999.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp998;_tmp998.tag=0;_tmp998.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp996[2]={& _tmp998,& _tmp999};Cyc_Tcutil_terr(
loc,({const char*_tmp997="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp997,sizeof(char),_get_zero_arr_size(_tmp997,52));}),
_tag_dynforward(_tmp996,sizeof(void*),2));}}});goto _LL5CD;_LL5CD:;}}}void Cyc_Tcutil_check_no_unique_region(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp99A;_tmp99A.kind_env=0;_tmp99A.free_vars=0;
_tmp99A.free_evars=0;_tmp99A.generalize_evars=0;_tmp99A.fn_result=0;_tmp99A;}),(
void*)1,t,0,1);}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t,1);{void*_tmp99B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp99C;struct Cyc_List_List*_tmp99D;struct Cyc_Core_Opt*
_tmp99E;void*_tmp99F;_LL5D5: if(_tmp99B <= (void*)4)goto _LL5D7;if(*((int*)_tmp99B)
!= 8)goto _LL5D7;_tmp99C=((struct Cyc_Absyn_FnType_struct*)_tmp99B)->f1;_tmp99D=
_tmp99C.tvars;_tmp99E=_tmp99C.effect;_tmp99F=(void*)_tmp99C.ret_typ;_LL5D6: fd->tvs=
_tmp99D;fd->effect=_tmp99E;goto _LL5D4;_LL5D7:;_LL5D8:({void*_tmp9A0[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9A1="check_fndecl_valid_type: not a FnType";_tag_dynforward(_tmp9A1,sizeof(
char),_get_zero_arr_size(_tmp9A1,38));}),_tag_dynforward(_tmp9A0,sizeof(void*),0));});
_LL5D4:;}{struct _RegionHandle _tmp9A2=_new_region("r");struct _RegionHandle*r=&
_tmp9A2;_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,(struct _dynforward_ptr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,({const char*_tmp9A3="function declaration has repeated parameter";
_tag_dynforward(_tmp9A3,sizeof(char),_get_zero_arr_size(_tmp9A3,44));}));;
_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp9A4=_cycalloc(sizeof(*
_tmp9A4));_tmp9A4->v=(void*)t;_tmp9A4;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,
int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp9A5=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t,1);struct Cyc_List_List*_tmp9A6=Cyc_Tcutil_remove_bound_tvars(((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple18*))Cyc_Core_fst,_tmp9A5.free_vars),
bound_tvars);struct Cyc_List_List*_tmp9A7=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple19*))
Cyc_Core_fst,_tmp9A5.free_evars);{struct Cyc_List_List*fs=_tmp9A6;for(0;fs != 0;fs=
fs->tl){struct _dynforward_ptr*_tmp9A8=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({
struct Cyc_String_pa_struct _tmp9AC;_tmp9AC.tag=0;_tmp9AC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp9AB;_tmp9AB.tag=0;_tmp9AB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp9A8);{void*_tmp9A9[2]={& _tmp9AB,& _tmp9AC};Cyc_Tcutil_terr(loc,({const char*
_tmp9AA="unbound type variable %s in type %s";_tag_dynforward(_tmp9AA,sizeof(
char),_get_zero_arr_size(_tmp9AA,36));}),_tag_dynforward(_tmp9A9,sizeof(void*),2));}}});}}
if(!allow_evars  && _tmp9A7 != 0)for(0;_tmp9A7 != 0;_tmp9A7=_tmp9A7->tl){void*e=(
void*)_tmp9A7->hd;void*_tmp9AD=Cyc_Tcutil_typ_kind(e);_LL5DA: if((int)_tmp9AD != 3)
goto _LL5DC;_LL5DB: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp9AE[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9AF="can't unify evar with heap!";_tag_dynforward(_tmp9AF,sizeof(char),
_get_zero_arr_size(_tmp9AF,28));}),_tag_dynforward(_tmp9AE,sizeof(void*),0));});
goto _LL5D9;_LL5DC: if((int)_tmp9AD != 4)goto _LL5DE;_LL5DD: if(!Cyc_Tcutil_unify(e,
Cyc_Absyn_empty_effect))({void*_tmp9B0[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9B1="can't unify evar with {}!";
_tag_dynforward(_tmp9B1,sizeof(char),_get_zero_arr_size(_tmp9B1,26));}),
_tag_dynforward(_tmp9B0,sizeof(void*),0));});goto _LL5D9;_LL5DE:;_LL5DF:({struct
Cyc_String_pa_struct _tmp9B5;_tmp9B5.tag=0;_tmp9B5.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp9B4;_tmp9B4.tag=0;_tmp9B4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp9B2[2]={& _tmp9B4,& _tmp9B5};Cyc_Tcutil_terr(
loc,({const char*_tmp9B3="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp9B3,sizeof(char),_get_zero_arr_size(_tmp9B3,52));}),
_tag_dynforward(_tmp9B2,sizeof(void*),2));}}});goto _LL5D9;_LL5D9:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr(*a2string)(
void*),struct _dynforward_ptr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=
vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({
struct Cyc_String_pa_struct _tmp9B9;_tmp9B9.tag=0;_tmp9B9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)a2string((void*)vs->hd));{struct Cyc_String_pa_struct
_tmp9B8;_tmp9B8.tag=0;_tmp9B8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg);{void*_tmp9B6[2]={& _tmp9B8,& _tmp9B9};Cyc_Tcutil_terr(loc,({const char*
_tmp9B7="%s: %s";_tag_dynforward(_tmp9B7,sizeof(char),_get_zero_arr_size(_tmp9B7,
7));}),_tag_dynforward(_tmp9B6,sizeof(void*),2));}}});}}}static struct
_dynforward_ptr Cyc_Tcutil_strptr2string(struct _dynforward_ptr*s){return*s;}void
Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr msg){((void(*)(int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr(*a2string)(struct _dynforward_ptr*),struct _dynforward_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct
_dynforward_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,
Cyc_Tcutil_tvar2string,({const char*_tmp9BA="duplicate type variable";
_tag_dynforward(_tmp9BA,sizeof(char),_get_zero_arr_size(_tmp9BA,24));}));}struct
_tuple20{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple21{struct Cyc_List_List*
f1;void*f2;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,({const char*_tmp9BB="";_tag_dynforward(_tmp9BB,sizeof(char),
_get_zero_arr_size(_tmp9BB,1));}))!= 0)fields=({struct Cyc_List_List*_tmp9BC=
_cycalloc(sizeof(*_tmp9BC));_tmp9BC->hd=({struct _tuple20*_tmp9BD=_cycalloc(
sizeof(*_tmp9BD));_tmp9BD->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp9BD->f2=
0;_tmp9BD;});_tmp9BC->tl=fields;_tmp9BC;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple21 _tmp9BF;struct Cyc_List_List*_tmp9C0;void*
_tmp9C1;struct _tuple21*_tmp9BE=(struct _tuple21*)des->hd;_tmp9BF=*_tmp9BE;_tmp9C0=
_tmp9BF.f1;_tmp9C1=_tmp9BF.f2;if(_tmp9C0 == 0){struct Cyc_List_List*_tmp9C2=fields;
for(0;_tmp9C2 != 0;_tmp9C2=_tmp9C2->tl){if(!(*((struct _tuple20*)_tmp9C2->hd)).f2){(*((
struct _tuple20*)_tmp9C2->hd)).f2=1;(*((struct _tuple21*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp9C3=_cycalloc(sizeof(*_tmp9C3));_tmp9C3->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp9C4=_cycalloc(sizeof(*_tmp9C4));
_tmp9C4[0]=({struct Cyc_Absyn_FieldName_struct _tmp9C5;_tmp9C5.tag=1;_tmp9C5.f1=((*((
struct _tuple20*)_tmp9C2->hd)).f1)->name;_tmp9C5;});_tmp9C4;}));_tmp9C3->tl=0;
_tmp9C3;});ans=({struct Cyc_List_List*_tmp9C6=_region_malloc(rgn,sizeof(*_tmp9C6));
_tmp9C6->hd=({struct _tuple22*_tmp9C7=_region_malloc(rgn,sizeof(*_tmp9C7));
_tmp9C7->f1=(*((struct _tuple20*)_tmp9C2->hd)).f1;_tmp9C7->f2=_tmp9C1;_tmp9C7;});
_tmp9C6->tl=ans;_tmp9C6;});break;}}if(_tmp9C2 == 0)({void*_tmp9C8[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp9C9="too many arguments to struct";_tag_dynforward(_tmp9C9,
sizeof(char),_get_zero_arr_size(_tmp9C9,29));}),_tag_dynforward(_tmp9C8,sizeof(
void*),0));});}else{if(_tmp9C0->tl != 0)({void*_tmp9CA[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp9CB="multiple designators are not supported";_tag_dynforward(
_tmp9CB,sizeof(char),_get_zero_arr_size(_tmp9CB,39));}),_tag_dynforward(_tmp9CA,
sizeof(void*),0));});else{void*_tmp9CC=(void*)_tmp9C0->hd;struct _dynforward_ptr*
_tmp9CD;_LL5E1: if(*((int*)_tmp9CC)!= 0)goto _LL5E3;_LL5E2:({void*_tmp9CE[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp9CF="array designator used in argument to struct";
_tag_dynforward(_tmp9CF,sizeof(char),_get_zero_arr_size(_tmp9CF,44));}),
_tag_dynforward(_tmp9CE,sizeof(void*),0));});goto _LL5E0;_LL5E3: if(*((int*)
_tmp9CC)!= 1)goto _LL5E0;_tmp9CD=((struct Cyc_Absyn_FieldName_struct*)_tmp9CC)->f1;
_LL5E4: {struct Cyc_List_List*_tmp9D0=fields;for(0;_tmp9D0 != 0;_tmp9D0=_tmp9D0->tl){
if(Cyc_strptrcmp(_tmp9CD,((*((struct _tuple20*)_tmp9D0->hd)).f1)->name)== 0){if((*((
struct _tuple20*)_tmp9D0->hd)).f2)({struct Cyc_String_pa_struct _tmp9D3;_tmp9D3.tag=
0;_tmp9D3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp9CD);{void*
_tmp9D1[1]={& _tmp9D3};Cyc_Tcutil_terr(loc,({const char*_tmp9D2="field %s has already been used as an argument";
_tag_dynforward(_tmp9D2,sizeof(char),_get_zero_arr_size(_tmp9D2,46));}),
_tag_dynforward(_tmp9D1,sizeof(void*),1));}});(*((struct _tuple20*)_tmp9D0->hd)).f2=
1;ans=({struct Cyc_List_List*_tmp9D4=_region_malloc(rgn,sizeof(*_tmp9D4));_tmp9D4->hd=({
struct _tuple22*_tmp9D5=_region_malloc(rgn,sizeof(*_tmp9D5));_tmp9D5->f1=(*((
struct _tuple20*)_tmp9D0->hd)).f1;_tmp9D5->f2=_tmp9C1;_tmp9D5;});_tmp9D4->tl=ans;
_tmp9D4;});break;}}if(_tmp9D0 == 0)({struct Cyc_String_pa_struct _tmp9D8;_tmp9D8.tag=
0;_tmp9D8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp9CD);{void*
_tmp9D6[1]={& _tmp9D8};Cyc_Tcutil_terr(loc,({const char*_tmp9D7="bad field designator %s";
_tag_dynforward(_tmp9D7,sizeof(char),_get_zero_arr_size(_tmp9D7,24));}),
_tag_dynforward(_tmp9D6,sizeof(void*),1));}});goto _LL5E0;}_LL5E0:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){({void*
_tmp9D9[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp9DA="too few arguments to struct";
_tag_dynforward(_tmp9DA,sizeof(char),_get_zero_arr_size(_tmp9DA,28));}),
_tag_dynforward(_tmp9D9,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only){*forward_only=0;{void*_tmp9DB=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp9DC;void*_tmp9DD;struct Cyc_Absyn_PtrAtts _tmp9DE;
struct Cyc_Absyn_Conref*_tmp9DF;_LL5E6: if(_tmp9DB <= (void*)4)goto _LL5E8;if(*((int*)
_tmp9DB)!= 4)goto _LL5E8;_tmp9DC=((struct Cyc_Absyn_PointerType_struct*)_tmp9DB)->f1;
_tmp9DD=(void*)_tmp9DC.elt_typ;_tmp9DE=_tmp9DC.ptr_atts;_tmp9DF=_tmp9DE.bounds;
_LL5E7: {struct Cyc_Absyn_Conref*_tmp9E0=Cyc_Absyn_compress_conref(_tmp9DF);union
Cyc_Absyn_Constraint_union _tmp9E1=(Cyc_Absyn_compress_conref(_tmp9E0))->v;void*
_tmp9E2;void*_tmp9E3;_LL5EB: if((_tmp9E1.Eq_constr).tag != 0)goto _LL5ED;_tmp9E2=(
_tmp9E1.Eq_constr).f1;if((int)_tmp9E2 != 0)goto _LL5ED;_LL5EC:*forward_only=1;*
elt_typ_dest=_tmp9DD;return 1;_LL5ED: if((_tmp9E1.Eq_constr).tag != 0)goto _LL5EF;
_tmp9E3=(_tmp9E1.Eq_constr).f1;if((int)_tmp9E3 != 1)goto _LL5EF;_LL5EE:*
elt_typ_dest=_tmp9DD;return 1;_LL5EF: if((_tmp9E1.No_constr).tag != 2)goto _LL5F1;
_LL5F0: _tmp9E0->v=(union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union
_tmp9E4;(_tmp9E4.Eq_constr).tag=0;(_tmp9E4.Eq_constr).f1=(void*)((void*)0);
_tmp9E4;});*forward_only=1;*elt_typ_dest=_tmp9DD;return 1;_LL5F1:;_LL5F2: return 0;
_LL5EA:;}_LL5E8:;_LL5E9: return 0;_LL5E5:;}}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp9E5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp9E6;void*_tmp9E7;struct Cyc_Absyn_PtrAtts _tmp9E8;struct Cyc_Absyn_Conref*
_tmp9E9;_LL5F4: if(_tmp9E5 <= (void*)4)goto _LL5F6;if(*((int*)_tmp9E5)!= 4)goto
_LL5F6;_tmp9E6=((struct Cyc_Absyn_PointerType_struct*)_tmp9E5)->f1;_tmp9E7=(void*)
_tmp9E6.elt_typ;_tmp9E8=_tmp9E6.ptr_atts;_tmp9E9=_tmp9E8.zero_term;_LL5F5:*
elt_typ_dest=_tmp9E7;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9E9);_LL5F6:;_LL5F7: return 0;_LL5F3:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t,int*is_forward_only){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore,is_forward_only);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmpA2D;
_tmpA2D.f1=0;_tmpA2D.f2=(void*)2;_tmpA2D;});void*_tmp9EA=(void*)e->r;struct
_tuple2*_tmp9EB;void*_tmp9EC;struct Cyc_Absyn_Exp*_tmp9ED;struct _dynforward_ptr*
_tmp9EE;struct Cyc_Absyn_Exp*_tmp9EF;struct _dynforward_ptr*_tmp9F0;struct Cyc_Absyn_Exp*
_tmp9F1;struct Cyc_Absyn_Exp*_tmp9F2;struct Cyc_Absyn_Exp*_tmp9F3;_LL5F9: if(*((int*)
_tmp9EA)!= 1)goto _LL5FB;_tmp9EB=((struct Cyc_Absyn_Var_e_struct*)_tmp9EA)->f1;
_tmp9EC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9EA)->f2;_LL5FA: {void*
_tmp9F4=_tmp9EC;struct Cyc_Absyn_Vardecl*_tmp9F5;struct Cyc_Absyn_Vardecl*_tmp9F6;
struct Cyc_Absyn_Vardecl*_tmp9F7;struct Cyc_Absyn_Vardecl*_tmp9F8;_LL606: if((int)
_tmp9F4 != 0)goto _LL608;_LL607: goto _LL609;_LL608: if(_tmp9F4 <= (void*)1)goto _LL60A;
if(*((int*)_tmp9F4)!= 1)goto _LL60A;_LL609: return bogus_ans;_LL60A: if(_tmp9F4 <= (
void*)1)goto _LL60C;if(*((int*)_tmp9F4)!= 0)goto _LL60C;_tmp9F5=((struct Cyc_Absyn_Global_b_struct*)
_tmp9F4)->f1;_LL60B: {void*_tmp9F9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL613: if(_tmp9F9 <= (void*)4)goto _LL615;if(*((int*)
_tmp9F9)!= 7)goto _LL615;_LL614: return({struct _tuple10 _tmp9FA;_tmp9FA.f1=1;
_tmp9FA.f2=(void*)2;_tmp9FA;});_LL615:;_LL616: return({struct _tuple10 _tmp9FB;
_tmp9FB.f1=(_tmp9F5->tq).real_const;_tmp9FB.f2=(void*)2;_tmp9FB;});_LL612:;}
_LL60C: if(_tmp9F4 <= (void*)1)goto _LL60E;if(*((int*)_tmp9F4)!= 3)goto _LL60E;
_tmp9F6=((struct Cyc_Absyn_Local_b_struct*)_tmp9F4)->f1;_LL60D: {void*_tmp9FC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL618: if(_tmp9FC <= (void*)
4)goto _LL61A;if(*((int*)_tmp9FC)!= 7)goto _LL61A;_LL619: return({struct _tuple10
_tmp9FD;_tmp9FD.f1=1;_tmp9FD.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp9F6->rgn))->v;
_tmp9FD;});_LL61A:;_LL61B: _tmp9F6->escapes=1;return({struct _tuple10 _tmp9FE;
_tmp9FE.f1=(_tmp9F6->tq).real_const;_tmp9FE.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F6->rgn))->v;_tmp9FE;});_LL617:;}_LL60E: if(_tmp9F4 <= (void*)1)
goto _LL610;if(*((int*)_tmp9F4)!= 4)goto _LL610;_tmp9F7=((struct Cyc_Absyn_Pat_b_struct*)
_tmp9F4)->f1;_LL60F: _tmp9F8=_tmp9F7;goto _LL611;_LL610: if(_tmp9F4 <= (void*)1)goto
_LL605;if(*((int*)_tmp9F4)!= 2)goto _LL605;_tmp9F8=((struct Cyc_Absyn_Param_b_struct*)
_tmp9F4)->f1;_LL611: _tmp9F8->escapes=1;return({struct _tuple10 _tmp9FF;_tmp9FF.f1=(
_tmp9F8->tq).real_const;_tmp9FF.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9F8->rgn))->v;_tmp9FF;});_LL605:;}_LL5FB: if(*((int*)_tmp9EA)!= 23)goto _LL5FD;
_tmp9ED=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp9EA)->f1;_tmp9EE=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp9EA)->f2;_LL5FC: {void*_tmpA00=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9ED->topt))->v);struct Cyc_List_List*_tmpA01;struct Cyc_Absyn_AggrInfo
_tmpA02;union Cyc_Absyn_AggrInfoU_union _tmpA03;struct Cyc_Absyn_Aggrdecl**_tmpA04;
struct Cyc_Absyn_Aggrdecl*_tmpA05;_LL61D: if(_tmpA00 <= (void*)4)goto _LL621;if(*((
int*)_tmpA00)!= 11)goto _LL61F;_tmpA01=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA00)->f2;_LL61E: {struct Cyc_Absyn_Aggrfield*_tmpA06=Cyc_Absyn_lookup_field(
_tmpA01,_tmp9EE);if(_tmpA06 != 0  && _tmpA06->width != 0)return({struct _tuple10
_tmpA07;_tmpA07.f1=(_tmpA06->tq).real_const;_tmpA07.f2=(Cyc_Tcutil_addressof_props(
te,_tmp9ED)).f2;_tmpA07;});return bogus_ans;}_LL61F: if(*((int*)_tmpA00)!= 10)goto
_LL621;_tmpA02=((struct Cyc_Absyn_AggrType_struct*)_tmpA00)->f1;_tmpA03=_tmpA02.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA00)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL621;_tmpA04=(_tmpA03.KnownAggr).f1;_tmpA05=*_tmpA04;_LL620: {struct
Cyc_Absyn_Aggrfield*_tmpA08=Cyc_Absyn_lookup_decl_field(_tmpA05,_tmp9EE);if(
_tmpA08 != 0  && _tmpA08->width != 0)return({struct _tuple10 _tmpA09;_tmpA09.f1=(
_tmpA08->tq).real_const;_tmpA09.f2=(Cyc_Tcutil_addressof_props(te,_tmp9ED)).f2;
_tmpA09;});return bogus_ans;}_LL621:;_LL622: return bogus_ans;_LL61C:;}_LL5FD: if(*((
int*)_tmp9EA)!= 24)goto _LL5FF;_tmp9EF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp9EA)->f1;_tmp9F0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp9EA)->f2;_LL5FE: {
void*_tmpA0A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9EF->topt))->v);struct Cyc_Absyn_PtrInfo _tmpA0B;void*_tmpA0C;struct Cyc_Absyn_PtrAtts
_tmpA0D;void*_tmpA0E;_LL624: if(_tmpA0A <= (void*)4)goto _LL626;if(*((int*)_tmpA0A)
!= 4)goto _LL626;_tmpA0B=((struct Cyc_Absyn_PointerType_struct*)_tmpA0A)->f1;
_tmpA0C=(void*)_tmpA0B.elt_typ;_tmpA0D=_tmpA0B.ptr_atts;_tmpA0E=(void*)_tmpA0D.rgn;
_LL625: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpA0F=Cyc_Tcutil_compress(
_tmpA0C);struct Cyc_List_List*_tmpA10;struct Cyc_Absyn_AggrInfo _tmpA11;union Cyc_Absyn_AggrInfoU_union
_tmpA12;struct Cyc_Absyn_Aggrdecl**_tmpA13;struct Cyc_Absyn_Aggrdecl*_tmpA14;
_LL629: if(_tmpA0F <= (void*)4)goto _LL62D;if(*((int*)_tmpA0F)!= 11)goto _LL62B;
_tmpA10=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA0F)->f2;_LL62A: finfo=Cyc_Absyn_lookup_field(
_tmpA10,_tmp9F0);goto _LL628;_LL62B: if(*((int*)_tmpA0F)!= 10)goto _LL62D;_tmpA11=((
struct Cyc_Absyn_AggrType_struct*)_tmpA0F)->f1;_tmpA12=_tmpA11.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpA0F)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL62D;_tmpA13=(_tmpA12.KnownAggr).f1;_tmpA14=*_tmpA13;_LL62C: finfo=Cyc_Absyn_lookup_decl_field(
_tmpA14,_tmp9F0);goto _LL628;_LL62D:;_LL62E: return bogus_ans;_LL628:;}if(finfo != 0
 && finfo->width != 0)return({struct _tuple10 _tmpA15;_tmpA15.f1=(finfo->tq).real_const;
_tmpA15.f2=_tmpA0E;_tmpA15;});return bogus_ans;}_LL626:;_LL627: return bogus_ans;
_LL623:;}_LL5FF: if(*((int*)_tmp9EA)!= 22)goto _LL601;_tmp9F1=((struct Cyc_Absyn_Deref_e_struct*)
_tmp9EA)->f1;_LL600: {void*_tmpA16=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F1->topt))->v);struct Cyc_Absyn_PtrInfo _tmpA17;struct Cyc_Absyn_Tqual
_tmpA18;struct Cyc_Absyn_PtrAtts _tmpA19;void*_tmpA1A;_LL630: if(_tmpA16 <= (void*)4)
goto _LL632;if(*((int*)_tmpA16)!= 4)goto _LL632;_tmpA17=((struct Cyc_Absyn_PointerType_struct*)
_tmpA16)->f1;_tmpA18=_tmpA17.elt_tq;_tmpA19=_tmpA17.ptr_atts;_tmpA1A=(void*)
_tmpA19.rgn;_LL631: return({struct _tuple10 _tmpA1B;_tmpA1B.f1=_tmpA18.real_const;
_tmpA1B.f2=_tmpA1A;_tmpA1B;});_LL632:;_LL633: return bogus_ans;_LL62F:;}_LL601: if(*((
int*)_tmp9EA)!= 25)goto _LL603;_tmp9F2=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp9EA)->f1;_tmp9F3=((struct Cyc_Absyn_Subscript_e_struct*)_tmp9EA)->f2;_LL602: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp9F2->topt))->v);
void*_tmpA1C=t;struct Cyc_List_List*_tmpA1D;struct Cyc_Absyn_PtrInfo _tmpA1E;struct
Cyc_Absyn_Tqual _tmpA1F;struct Cyc_Absyn_PtrAtts _tmpA20;void*_tmpA21;struct Cyc_Absyn_ArrayInfo
_tmpA22;struct Cyc_Absyn_Tqual _tmpA23;_LL635: if(_tmpA1C <= (void*)4)goto _LL63B;if(*((
int*)_tmpA1C)!= 9)goto _LL637;_tmpA1D=((struct Cyc_Absyn_TupleType_struct*)_tmpA1C)->f1;
_LL636: {unsigned int _tmpA25;int _tmpA26;struct _tuple7 _tmpA24=Cyc_Evexp_eval_const_uint_exp(
_tmp9F3);_tmpA25=_tmpA24.f1;_tmpA26=_tmpA24.f2;if(!_tmpA26)return bogus_ans;{
struct _tuple5*_tmpA27=Cyc_Absyn_lookup_tuple_field(_tmpA1D,(int)_tmpA25);if(
_tmpA27 != 0)return({struct _tuple10 _tmpA28;_tmpA28.f1=((*_tmpA27).f1).real_const;
_tmpA28.f2=(Cyc_Tcutil_addressof_props(te,_tmp9F2)).f2;_tmpA28;});return
bogus_ans;}}_LL637: if(*((int*)_tmpA1C)!= 4)goto _LL639;_tmpA1E=((struct Cyc_Absyn_PointerType_struct*)
_tmpA1C)->f1;_tmpA1F=_tmpA1E.elt_tq;_tmpA20=_tmpA1E.ptr_atts;_tmpA21=(void*)
_tmpA20.rgn;_LL638: return({struct _tuple10 _tmpA29;_tmpA29.f1=_tmpA1F.real_const;
_tmpA29.f2=_tmpA21;_tmpA29;});_LL639: if(*((int*)_tmpA1C)!= 7)goto _LL63B;_tmpA22=((
struct Cyc_Absyn_ArrayType_struct*)_tmpA1C)->f1;_tmpA23=_tmpA22.tq;_LL63A: return({
struct _tuple10 _tmpA2A;_tmpA2A.f1=_tmpA23.real_const;_tmpA2A.f2=(Cyc_Tcutil_addressof_props(
te,_tmp9F2)).f2;_tmpA2A;});_LL63B:;_LL63C: return bogus_ans;_LL634:;}_LL603:;
_LL604:({void*_tmpA2B[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmpA2C="unary & applied to non-lvalue";
_tag_dynforward(_tmpA2C,sizeof(char),_get_zero_arr_size(_tmpA2C,30));}),
_tag_dynforward(_tmpA2B,sizeof(void*),0));});return bogus_ans;_LL5F8:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpA2E=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpA2F;void*_tmpA30;struct Cyc_Absyn_Tqual
_tmpA31;struct Cyc_Absyn_Conref*_tmpA32;_LL63E: if(_tmpA2E <= (void*)4)goto _LL640;
if(*((int*)_tmpA2E)!= 7)goto _LL640;_tmpA2F=((struct Cyc_Absyn_ArrayType_struct*)
_tmpA2E)->f1;_tmpA30=(void*)_tmpA2F.elt_type;_tmpA31=_tmpA2F.tq;_tmpA32=_tmpA2F.zero_term;
_LL63F: {void*_tmpA34;struct _tuple10 _tmpA33=Cyc_Tcutil_addressof_props(te,e);
_tmpA34=_tmpA33.f2;return Cyc_Absyn_atb_typ(_tmpA30,_tmpA34,_tmpA31,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpA35=_cycalloc(sizeof(*_tmpA35));_tmpA35[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpA36;_tmpA36.tag=0;_tmpA36.f1=e;_tmpA36;});
_tmpA35;}),_tmpA32);}_LL640:;_LL641: return e_typ;_LL63D:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpA37=b->v;void*_tmpA38;void*_tmpA39;void*
_tmpA3A;struct Cyc_Absyn_Exp*_tmpA3B;void*_tmpA3C;_LL643: if((_tmpA37.Eq_constr).tag
!= 0)goto _LL645;_tmpA38=(_tmpA37.Eq_constr).f1;if((int)_tmpA38 != 1)goto _LL645;
_LL644: return;_LL645: if((_tmpA37.Eq_constr).tag != 0)goto _LL647;_tmpA39=(_tmpA37.Eq_constr).f1;
if((int)_tmpA39 != 0)goto _LL647;_LL646: if(i < 0)({void*_tmpA3D[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmpA3E="dereference is out of bounds";_tag_dynforward(_tmpA3E,
sizeof(char),_get_zero_arr_size(_tmpA3E,29));}),_tag_dynforward(_tmpA3D,sizeof(
void*),0));});return;_LL647: if((_tmpA37.Eq_constr).tag != 0)goto _LL649;_tmpA3A=(
_tmpA37.Eq_constr).f1;if(_tmpA3A <= (void*)2)goto _LL649;if(*((int*)_tmpA3A)!= 0)
goto _LL649;_tmpA3B=((struct Cyc_Absyn_Upper_b_struct*)_tmpA3A)->f1;_LL648: {
unsigned int _tmpA40;int _tmpA41;struct _tuple7 _tmpA3F=Cyc_Evexp_eval_const_uint_exp(
_tmpA3B);_tmpA40=_tmpA3F.f1;_tmpA41=_tmpA3F.f2;if(_tmpA41  && _tmpA40 <= i)({
struct Cyc_Int_pa_struct _tmpA45;_tmpA45.tag=1;_tmpA45.f1=(unsigned long)((int)i);{
struct Cyc_Int_pa_struct _tmpA44;_tmpA44.tag=1;_tmpA44.f1=(unsigned long)((int)
_tmpA40);{void*_tmpA42[2]={& _tmpA44,& _tmpA45};Cyc_Tcutil_terr(loc,({const char*
_tmpA43="dereference is out of bounds: %d <= %d";_tag_dynforward(_tmpA43,sizeof(
char),_get_zero_arr_size(_tmpA43,39));}),_tag_dynforward(_tmpA42,sizeof(void*),2));}}});
return;}_LL649: if((_tmpA37.Eq_constr).tag != 0)goto _LL64B;_tmpA3C=(_tmpA37.Eq_constr).f1;
if(_tmpA3C <= (void*)2)goto _LL64B;if(*((int*)_tmpA3C)!= 1)goto _LL64B;_LL64A:
return;_LL64B: if((_tmpA37.No_constr).tag != 2)goto _LL64D;_LL64C: b->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmpA46;(_tmpA46.Eq_constr).tag=0;(_tmpA46.Eq_constr).f1=(
void*)((void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmpA47=_cycalloc(sizeof(*
_tmpA47));_tmpA47[0]=({struct Cyc_Absyn_Upper_b_struct _tmpA48;_tmpA48.tag=0;
_tmpA48.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmpA48;});_tmpA47;})));_tmpA46;});return;
_LL64D:;_LL64E:({void*_tmpA49[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA4A="check_bound -- bad compressed conref";
_tag_dynforward(_tmpA4A,sizeof(char),_get_zero_arr_size(_tmpA4A,37));}),
_tag_dynforward(_tmpA49,sizeof(void*),0));});_LL642:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){union Cyc_Absyn_Constraint_union
_tmpA4B=(Cyc_Absyn_compress_conref(b))->v;void*_tmpA4C;struct Cyc_Absyn_Exp*
_tmpA4D;_LL650: if((_tmpA4B.Eq_constr).tag != 0)goto _LL652;_tmpA4C=(_tmpA4B.Eq_constr).f1;
if(_tmpA4C <= (void*)2)goto _LL652;if(*((int*)_tmpA4C)!= 0)goto _LL652;_tmpA4D=((
struct Cyc_Absyn_Upper_b_struct*)_tmpA4C)->f1;_LL651: {unsigned int _tmpA4F;int
_tmpA50;struct _tuple7 _tmpA4E=Cyc_Evexp_eval_const_uint_exp(_tmpA4D);_tmpA4F=
_tmpA4E.f1;_tmpA50=_tmpA4E.f2;return _tmpA50  && _tmpA4F == 1;}_LL652:;_LL653:
return 0;_LL64F:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpA51=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpA52;void*_tmpA53;struct Cyc_Absyn_Conref*_tmpA54;
struct Cyc_List_List*_tmpA55;struct Cyc_Absyn_AggrInfo _tmpA56;union Cyc_Absyn_AggrInfoU_union
_tmpA57;struct Cyc_Absyn_AggrInfo _tmpA58;union Cyc_Absyn_AggrInfoU_union _tmpA59;
struct Cyc_Absyn_Aggrdecl**_tmpA5A;struct Cyc_Absyn_Aggrdecl*_tmpA5B;struct Cyc_List_List*
_tmpA5C;struct Cyc_List_List*_tmpA5D;_LL655: if((int)_tmpA51 != 0)goto _LL657;_LL656:
goto _LL658;_LL657: if(_tmpA51 <= (void*)4)goto _LL659;if(*((int*)_tmpA51)!= 5)goto
_LL659;_LL658: goto _LL65A;_LL659: if((int)_tmpA51 != 1)goto _LL65B;_LL65A: goto _LL65C;
_LL65B: if(_tmpA51 <= (void*)4)goto _LL66B;if(*((int*)_tmpA51)!= 6)goto _LL65D;
_LL65C: return 1;_LL65D: if(*((int*)_tmpA51)!= 12)goto _LL65F;_LL65E: goto _LL660;
_LL65F: if(*((int*)_tmpA51)!= 13)goto _LL661;_LL660: return 0;_LL661: if(*((int*)
_tmpA51)!= 7)goto _LL663;_tmpA52=((struct Cyc_Absyn_ArrayType_struct*)_tmpA51)->f1;
_tmpA53=(void*)_tmpA52.elt_type;_tmpA54=_tmpA52.zero_term;_LL662: return !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpA54) && Cyc_Tcutil_bits_only(
_tmpA53);_LL663: if(*((int*)_tmpA51)!= 9)goto _LL665;_tmpA55=((struct Cyc_Absyn_TupleType_struct*)
_tmpA51)->f1;_LL664: for(0;_tmpA55 != 0;_tmpA55=_tmpA55->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpA55->hd)).f2))return 0;}return 1;_LL665: if(*((int*)_tmpA51)!= 
10)goto _LL667;_tmpA56=((struct Cyc_Absyn_AggrType_struct*)_tmpA51)->f1;_tmpA57=
_tmpA56.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA51)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL667;_LL666: return 0;_LL667: if(*((int*)_tmpA51)!= 10)goto _LL669;
_tmpA58=((struct Cyc_Absyn_AggrType_struct*)_tmpA51)->f1;_tmpA59=_tmpA58.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA51)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL669;_tmpA5A=(_tmpA59.KnownAggr).f1;_tmpA5B=*_tmpA5A;_tmpA5C=_tmpA58.targs;
_LL668: if(_tmpA5B->impl == 0)return 0;{struct _RegionHandle _tmpA5E=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpA5E;_push_region(rgn);{struct Cyc_List_List*_tmpA5F=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA5B->tvs,_tmpA5C);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA5B->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpA5F,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA60=0;_npop_handler(0);
return _tmpA60;}}}{int _tmpA61=1;_npop_handler(0);return _tmpA61;}};_pop_region(rgn);}
_LL669: if(*((int*)_tmpA51)!= 11)goto _LL66B;_tmpA5D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA51)->f2;_LL66A: for(0;_tmpA5D != 0;_tmpA5D=_tmpA5D->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpA5D->hd)->type))return 0;}return 1;_LL66B:;
_LL66C: return 0;_LL654:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpA62=(void*)e->r;struct _tuple2*_tmpA63;void*_tmpA64;struct Cyc_Absyn_Exp*
_tmpA65;struct Cyc_Absyn_Exp*_tmpA66;struct Cyc_Absyn_Exp*_tmpA67;struct Cyc_Absyn_Exp*
_tmpA68;struct Cyc_Absyn_Exp*_tmpA69;struct Cyc_Absyn_Exp*_tmpA6A;struct Cyc_Absyn_Exp*
_tmpA6B;void*_tmpA6C;struct Cyc_Absyn_Exp*_tmpA6D;void*_tmpA6E;void*_tmpA6F;
struct Cyc_Absyn_Exp*_tmpA70;struct Cyc_Absyn_Exp*_tmpA71;struct Cyc_Absyn_Exp*
_tmpA72;struct Cyc_Absyn_Exp*_tmpA73;struct Cyc_List_List*_tmpA74;struct Cyc_List_List*
_tmpA75;struct Cyc_List_List*_tmpA76;void*_tmpA77;struct Cyc_List_List*_tmpA78;
struct Cyc_List_List*_tmpA79;struct Cyc_List_List*_tmpA7A;_LL66E: if(*((int*)
_tmpA62)!= 0)goto _LL670;_LL66F: goto _LL671;_LL670: if(*((int*)_tmpA62)!= 18)goto
_LL672;_LL671: goto _LL673;_LL672: if(*((int*)_tmpA62)!= 19)goto _LL674;_LL673: goto
_LL675;_LL674: if(*((int*)_tmpA62)!= 20)goto _LL676;_LL675: goto _LL677;_LL676: if(*((
int*)_tmpA62)!= 21)goto _LL678;_LL677: goto _LL679;_LL678: if(*((int*)_tmpA62)!= 33)
goto _LL67A;_LL679: goto _LL67B;_LL67A: if(*((int*)_tmpA62)!= 34)goto _LL67C;_LL67B:
return 1;_LL67C: if(*((int*)_tmpA62)!= 1)goto _LL67E;_tmpA63=((struct Cyc_Absyn_Var_e_struct*)
_tmpA62)->f1;_tmpA64=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA62)->f2;_LL67D: {
void*_tmpA7B=_tmpA64;struct Cyc_Absyn_Vardecl*_tmpA7C;_LL69D: if(_tmpA7B <= (void*)
1)goto _LL6A1;if(*((int*)_tmpA7B)!= 1)goto _LL69F;_LL69E: return 1;_LL69F: if(*((int*)
_tmpA7B)!= 0)goto _LL6A1;_tmpA7C=((struct Cyc_Absyn_Global_b_struct*)_tmpA7B)->f1;
_LL6A0: {void*_tmpA7D=Cyc_Tcutil_compress((void*)_tmpA7C->type);_LL6A6: if(
_tmpA7D <= (void*)4)goto _LL6AA;if(*((int*)_tmpA7D)!= 7)goto _LL6A8;_LL6A7: goto
_LL6A9;_LL6A8: if(*((int*)_tmpA7D)!= 8)goto _LL6AA;_LL6A9: return 1;_LL6AA:;_LL6AB:
return var_okay;_LL6A5:;}_LL6A1: if((int)_tmpA7B != 0)goto _LL6A3;_LL6A2: return 0;
_LL6A3:;_LL6A4: return var_okay;_LL69C:;}_LL67E: if(*((int*)_tmpA62)!= 6)goto _LL680;
_tmpA65=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA62)->f1;_tmpA66=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA62)->f2;_tmpA67=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA62)->f3;_LL67F:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpA65) && Cyc_Tcutil_cnst_exp(te,0,_tmpA66))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpA67);_LL680: if(*((int*)_tmpA62)!= 9)goto _LL682;
_tmpA68=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA62)->f1;_tmpA69=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpA62)->f2;_LL681: return Cyc_Tcutil_cnst_exp(te,0,_tmpA68) && Cyc_Tcutil_cnst_exp(
te,0,_tmpA69);_LL682: if(*((int*)_tmpA62)!= 13)goto _LL684;_tmpA6A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpA62)->f1;_LL683: _tmpA6B=_tmpA6A;goto _LL685;_LL684: if(*((int*)_tmpA62)!= 14)
goto _LL686;_tmpA6B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpA62)->f1;_LL685:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpA6B);_LL686: if(*((int*)_tmpA62)!= 15)
goto _LL688;_tmpA6C=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpA62)->f1;_tmpA6D=((
struct Cyc_Absyn_Cast_e_struct*)_tmpA62)->f2;_tmpA6E=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpA62)->f4;if((int)_tmpA6E != 1)goto _LL688;_LL687: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpA6D);_LL688: if(*((int*)_tmpA62)!= 15)goto _LL68A;_tmpA6F=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpA62)->f1;_tmpA70=((struct Cyc_Absyn_Cast_e_struct*)
_tmpA62)->f2;_LL689: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpA70);_LL68A: if(*((
int*)_tmpA62)!= 16)goto _LL68C;_tmpA71=((struct Cyc_Absyn_Address_e_struct*)
_tmpA62)->f1;_LL68B: return Cyc_Tcutil_cnst_exp(te,1,_tmpA71);_LL68C: if(*((int*)
_tmpA62)!= 29)goto _LL68E;_tmpA72=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpA62)->f2;_tmpA73=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpA62)->f3;
_LL68D: return Cyc_Tcutil_cnst_exp(te,0,_tmpA72) && Cyc_Tcutil_cnst_exp(te,0,
_tmpA73);_LL68E: if(*((int*)_tmpA62)!= 28)goto _LL690;_tmpA74=((struct Cyc_Absyn_Array_e_struct*)
_tmpA62)->f1;_LL68F: _tmpA75=_tmpA74;goto _LL691;_LL690: if(*((int*)_tmpA62)!= 31)
goto _LL692;_tmpA75=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpA62)->f2;_LL691:
_tmpA76=_tmpA75;goto _LL693;_LL692: if(*((int*)_tmpA62)!= 30)goto _LL694;_tmpA76=((
struct Cyc_Absyn_Struct_e_struct*)_tmpA62)->f3;_LL693: for(0;_tmpA76 != 0;_tmpA76=
_tmpA76->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpA76->hd)).f2))
return 0;}return 1;_LL694: if(*((int*)_tmpA62)!= 3)goto _LL696;_tmpA77=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpA62)->f1;_tmpA78=((struct Cyc_Absyn_Primop_e_struct*)
_tmpA62)->f2;_LL695: _tmpA79=_tmpA78;goto _LL697;_LL696: if(*((int*)_tmpA62)!= 26)
goto _LL698;_tmpA79=((struct Cyc_Absyn_Tuple_e_struct*)_tmpA62)->f1;_LL697: _tmpA7A=
_tmpA79;goto _LL699;_LL698: if(*((int*)_tmpA62)!= 32)goto _LL69A;_tmpA7A=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpA62)->f1;_LL699: for(0;_tmpA7A != 0;_tmpA7A=_tmpA7A->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpA7A->hd))return 0;}return 1;
_LL69A:;_LL69B: return 0;_LL66D:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpA7E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA7F;
struct Cyc_Absyn_PtrAtts _tmpA80;struct Cyc_Absyn_Conref*_tmpA81;struct Cyc_Absyn_Conref*
_tmpA82;struct Cyc_Absyn_ArrayInfo _tmpA83;void*_tmpA84;struct Cyc_List_List*
_tmpA85;struct Cyc_Absyn_AggrInfo _tmpA86;union Cyc_Absyn_AggrInfoU_union _tmpA87;
struct Cyc_List_List*_tmpA88;struct Cyc_List_List*_tmpA89;_LL6AD: if((int)_tmpA7E != 
0)goto _LL6AF;_LL6AE: goto _LL6B0;_LL6AF: if(_tmpA7E <= (void*)4)goto _LL6B1;if(*((int*)
_tmpA7E)!= 5)goto _LL6B1;_LL6B0: goto _LL6B2;_LL6B1: if((int)_tmpA7E != 1)goto _LL6B3;
_LL6B2: goto _LL6B4;_LL6B3: if(_tmpA7E <= (void*)4)goto _LL6C3;if(*((int*)_tmpA7E)!= 
6)goto _LL6B5;_LL6B4: return 1;_LL6B5: if(*((int*)_tmpA7E)!= 4)goto _LL6B7;_tmpA7F=((
struct Cyc_Absyn_PointerType_struct*)_tmpA7E)->f1;_tmpA80=_tmpA7F.ptr_atts;
_tmpA81=_tmpA80.nullable;_tmpA82=_tmpA80.bounds;_LL6B6: {void*_tmpA8A=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpA82);_LL6C6: if((int)_tmpA8A != 0)goto _LL6C8;_LL6C7: return 1;
_LL6C8: if((int)_tmpA8A != 1)goto _LL6CA;_LL6C9: return 1;_LL6CA:;_LL6CB: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpA81);_LL6C5:;}_LL6B7:
if(*((int*)_tmpA7E)!= 7)goto _LL6B9;_tmpA83=((struct Cyc_Absyn_ArrayType_struct*)
_tmpA7E)->f1;_tmpA84=(void*)_tmpA83.elt_type;_LL6B8: return Cyc_Tcutil_supports_default(
_tmpA84);_LL6B9: if(*((int*)_tmpA7E)!= 9)goto _LL6BB;_tmpA85=((struct Cyc_Absyn_TupleType_struct*)
_tmpA7E)->f1;_LL6BA: for(0;_tmpA85 != 0;_tmpA85=_tmpA85->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple5*)_tmpA85->hd)).f2))return 0;}return 1;_LL6BB: if(*((int*)_tmpA7E)!= 
10)goto _LL6BD;_tmpA86=((struct Cyc_Absyn_AggrType_struct*)_tmpA7E)->f1;_tmpA87=
_tmpA86.aggr_info;_tmpA88=_tmpA86.targs;_LL6BC: {struct Cyc_Absyn_Aggrdecl*
_tmpA8B=Cyc_Absyn_get_known_aggrdecl(_tmpA87);if(_tmpA8B->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA8B->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpA8B->tvs,_tmpA88,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpA8B->impl))->fields);}_LL6BD: if(*((int*)_tmpA7E)!= 11)goto _LL6BF;
_tmpA89=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA7E)->f2;_LL6BE: return Cyc_Tcutil_fields_support_default(
0,0,_tmpA89);_LL6BF: if(*((int*)_tmpA7E)!= 13)goto _LL6C1;_LL6C0: goto _LL6C2;_LL6C1:
if(*((int*)_tmpA7E)!= 12)goto _LL6C3;_LL6C2: return 1;_LL6C3:;_LL6C4: return 0;_LL6AC:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmpA8C=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpA8C;_push_region(rgn);{struct Cyc_List_List*_tmpA8D=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpA8D,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpA8E=0;_npop_handler(0);return _tmpA8E;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){void*_tmpA8F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA90;struct Cyc_Absyn_PtrAtts _tmpA91;struct Cyc_Absyn_Conref*
_tmpA92;struct Cyc_Absyn_Conref*_tmpA93;_LL6CD: if(_tmpA8F <= (void*)4)goto _LL6CF;
if(*((int*)_tmpA8F)!= 5)goto _LL6CF;_LL6CE: goto _LL6D0;_LL6CF: if((int)_tmpA8F != 1)
goto _LL6D1;_LL6D0: goto _LL6D2;_LL6D1: if(_tmpA8F <= (void*)4)goto _LL6D5;if(*((int*)
_tmpA8F)!= 6)goto _LL6D3;_LL6D2: return 1;_LL6D3: if(*((int*)_tmpA8F)!= 4)goto _LL6D5;
_tmpA90=((struct Cyc_Absyn_PointerType_struct*)_tmpA8F)->f1;_tmpA91=_tmpA90.ptr_atts;
_tmpA92=_tmpA91.nullable;_tmpA93=_tmpA91.bounds;_LL6D4: {union Cyc_Absyn_Constraint_union
_tmpA94=(Cyc_Absyn_compress_conref(_tmpA93))->v;void*_tmpA95;void*_tmpA96;_LL6D8:
if((_tmpA94.Eq_constr).tag != 0)goto _LL6DA;_tmpA95=(_tmpA94.Eq_constr).f1;if((int)
_tmpA95 != 0)goto _LL6DA;_LL6D9: return 0;_LL6DA: if((_tmpA94.Eq_constr).tag != 0)goto
_LL6DC;_tmpA96=(_tmpA94.Eq_constr).f1;if((int)_tmpA96 != 1)goto _LL6DC;_LL6DB:
return 0;_LL6DC: if((_tmpA94.Eq_constr).tag != 0)goto _LL6DE;_LL6DD: {union Cyc_Absyn_Constraint_union
_tmpA97=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpA92))->v;int _tmpA98;_LL6E1: if((_tmpA97.Eq_constr).tag != 0)goto _LL6E3;_tmpA98=(
_tmpA97.Eq_constr).f1;_LL6E2: return _tmpA98;_LL6E3:;_LL6E4: return 0;_LL6E0:;}
_LL6DE:;_LL6DF: return 0;_LL6D7:;}_LL6D5:;_LL6D6: return 0;_LL6CC:;}int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpA99=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA9A;void*
_tmpA9B;struct Cyc_Absyn_FnInfo _tmpA9C;struct Cyc_List_List*_tmpA9D;_LL6E6: if(
_tmpA99 <= (void*)4)goto _LL6EA;if(*((int*)_tmpA99)!= 4)goto _LL6E8;_tmpA9A=((
struct Cyc_Absyn_PointerType_struct*)_tmpA99)->f1;_tmpA9B=(void*)_tmpA9A.elt_typ;
_LL6E7: return Cyc_Tcutil_is_noreturn(_tmpA9B);_LL6E8: if(*((int*)_tmpA99)!= 8)goto
_LL6EA;_tmpA9C=((struct Cyc_Absyn_FnType_struct*)_tmpA99)->f1;_tmpA9D=_tmpA9C.attributes;
_LL6E9: for(0;_tmpA9D != 0;_tmpA9D=_tmpA9D->tl){void*_tmpA9E=(void*)_tmpA9D->hd;
_LL6ED: if((int)_tmpA9E != 3)goto _LL6EF;_LL6EE: return 1;_LL6EF:;_LL6F0: continue;
_LL6EC:;}goto _LL6E5;_LL6EA:;_LL6EB: goto _LL6E5;_LL6E5:;}return 0;}struct Cyc_List_List*
Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){void*_tmpA9F=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpAA0;struct Cyc_List_List*_tmpAA1;
struct Cyc_List_List**_tmpAA2;_LL6F2: if(_tmpA9F <= (void*)4)goto _LL6F4;if(*((int*)
_tmpA9F)!= 8)goto _LL6F4;_tmpAA0=((struct Cyc_Absyn_FnType_struct*)_tmpA9F)->f1;
_tmpAA1=_tmpAA0.attributes;_tmpAA2=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmpA9F)->f1).attributes;_LL6F3: {struct Cyc_List_List*_tmpAA3=0;for(0;atts != 0;
atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmpAA2=({struct Cyc_List_List*
_tmpAA4=_cycalloc(sizeof(*_tmpAA4));_tmpAA4->hd=(void*)((void*)atts->hd);_tmpAA4->tl=*
_tmpAA2;_tmpAA4;});else{_tmpAA3=({struct Cyc_List_List*_tmpAA5=_cycalloc(sizeof(*
_tmpAA5));_tmpAA5->hd=(void*)((void*)atts->hd);_tmpAA5->tl=_tmpAA3;_tmpAA5;});}}
return _tmpAA3;}_LL6F4:;_LL6F5:({void*_tmpAA6[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpAA7="transfer_fn_type_atts";_tag_dynforward(_tmpAA7,sizeof(char),
_get_zero_arr_size(_tmpAA7,22));}),_tag_dynforward(_tmpAA6,sizeof(void*),0));});
_LL6F1:;}
