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
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*
Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct
Cyc_Position_Segment*,struct _dynforward_ptr);extern char Cyc_Position_Nocontext[14];
extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
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
void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_List_List*
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
e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dynforward_ptr,int*
is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_urk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
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
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dynforward,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dynforward,int*is_dyneither,void**
elt_type);int Cyc_Tcutil_is_unique_pointer(void*t);int Cyc_Tcutil_is_unique_path(
struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,
struct Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;};struct _tuple10 Cyc_Tcutil_addressof_props(
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
void*_tmpEE;void*_tmpEF;void*_tmpF0;void*_tmpF1;void*_tmpF2;void*_tmpF3;void*
_tmpF4;void*_tmpF5;void*_tmpF6;void*_tmpF7;_LL53: _tmpEE=_tmpED.f1;if((int)_tmpEE
!= 2)goto _LL55;_tmpEF=_tmpED.f2;if((int)_tmpEF != 1)goto _LL55;_LL54: goto _LL56;
_LL55: _tmpF0=_tmpED.f1;if((int)_tmpF0 != 2)goto _LL57;_tmpF1=_tmpED.f2;if((int)
_tmpF1 != 0)goto _LL57;_LL56: goto _LL58;_LL57: _tmpF2=_tmpED.f1;if((int)_tmpF2 != 1)
goto _LL59;_tmpF3=_tmpED.f2;if((int)_tmpF3 != 0)goto _LL59;_LL58: goto _LL5A;_LL59:
_tmpF4=_tmpED.f1;if((int)_tmpF4 != 3)goto _LL5B;_tmpF5=_tmpED.f2;if((int)_tmpF5 != 
5)goto _LL5B;_LL5A: goto _LL5C;_LL5B: _tmpF6=_tmpED.f1;if((int)_tmpF6 != 4)goto _LL5D;
_tmpF7=_tmpED.f2;if((int)_tmpF7 != 5)goto _LL5D;_LL5C: return 1;_LL5D:;_LL5E: return 0;
_LL52:;}}static int Cyc_Tcutil_is_region_kind(void*k){void*_tmpF8=k;_LL60: if((int)
_tmpF8 != 3)goto _LL62;_LL61: goto _LL63;_LL62: if((int)_tmpF8 != 5)goto _LL64;_LL63:
goto _LL65;_LL64: if((int)_tmpF8 != 4)goto _LL66;_LL65: return 1;_LL66:;_LL67: return 0;
_LL5F:;}void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpF9=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpFA;void*_tmpFB;_LL69: if(*((int*)_tmpF9)!= 0)goto _LL6B;
_tmpFA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpF9)->f1;_LL6A: return _tmpFA;
_LL6B: if(*((int*)_tmpF9)!= 2)goto _LL6D;_tmpFB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpF9)->f2;_LL6C: return _tmpFB;_LL6D:;_LL6E:({void*_tmpFC[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpFD="kind not suitably constrained!";
_tag_dynforward(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,31));}),
_tag_dynforward(_tmpFC,sizeof(void*),0));});_LL68:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpFE=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpFF;struct Cyc_Absyn_Tvar*
_tmp100;void*_tmp101;struct Cyc_Absyn_TunionInfo _tmp102;union Cyc_Absyn_TunionInfoU_union
_tmp103;struct Cyc_Absyn_UnknownTunionInfo _tmp104;struct Cyc_Absyn_TunionInfo
_tmp105;union Cyc_Absyn_TunionInfoU_union _tmp106;struct Cyc_Absyn_Tuniondecl**
_tmp107;struct Cyc_Absyn_Tuniondecl*_tmp108;struct Cyc_Absyn_TunionFieldInfo
_tmp109;union Cyc_Absyn_TunionFieldInfoU_union _tmp10A;struct Cyc_Absyn_Tuniondecl*
_tmp10B;struct Cyc_Absyn_Tunionfield*_tmp10C;struct Cyc_Absyn_TunionFieldInfo
_tmp10D;union Cyc_Absyn_TunionFieldInfoU_union _tmp10E;struct Cyc_Absyn_Enumdecl*
_tmp10F;struct Cyc_Absyn_AggrInfo _tmp110;union Cyc_Absyn_AggrInfoU_union _tmp111;
struct Cyc_Absyn_AggrInfo _tmp112;union Cyc_Absyn_AggrInfoU_union _tmp113;struct Cyc_Absyn_Aggrdecl**
_tmp114;struct Cyc_Absyn_Aggrdecl*_tmp115;struct Cyc_Absyn_Aggrdecl _tmp116;struct
Cyc_Absyn_AggrdeclImpl*_tmp117;struct Cyc_Absyn_Enumdecl*_tmp118;struct Cyc_Absyn_PtrInfo
_tmp119;struct Cyc_Absyn_Typedefdecl*_tmp11A;_LL70: if(_tmpFE <= (void*)4)goto _LL74;
if(*((int*)_tmpFE)!= 0)goto _LL72;_tmpFF=((struct Cyc_Absyn_Evar_struct*)_tmpFE)->f1;
_LL71: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpFF))->v;_LL72: if(*((int*)
_tmpFE)!= 1)goto _LL74;_tmp100=((struct Cyc_Absyn_VarType_struct*)_tmpFE)->f1;
_LL73: return Cyc_Tcutil_tvar_kind(_tmp100);_LL74: if((int)_tmpFE != 0)goto _LL76;
_LL75: return(void*)1;_LL76: if(_tmpFE <= (void*)4)goto _LL78;if(*((int*)_tmpFE)!= 5)
goto _LL78;_tmp101=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpFE)->f2;_LL77:
return(_tmp101 == (void*)((void*)2) || _tmp101 == (void*)((void*)3))?(void*)2:(
void*)1;_LL78: if((int)_tmpFE != 1)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(_tmpFE <= (
void*)4)goto _LL82;if(*((int*)_tmpFE)!= 6)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(*((
int*)_tmpFE)!= 8)goto _LL7E;_LL7D: return(void*)1;_LL7E: if(*((int*)_tmpFE)!= 16)
goto _LL80;_LL7F: goto _LL81;_LL80: if(*((int*)_tmpFE)!= 15)goto _LL82;_LL81: return(
void*)2;_LL82: if((int)_tmpFE != 3)goto _LL84;_LL83: return(void*)4;_LL84: if((int)
_tmpFE != 2)goto _LL86;_LL85: return(void*)3;_LL86: if(_tmpFE <= (void*)4)goto _LL88;
if(*((int*)_tmpFE)!= 2)goto _LL88;_tmp102=((struct Cyc_Absyn_TunionType_struct*)
_tmpFE)->f1;_tmp103=_tmp102.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmpFE)->f1).tunion_info).UnknownTunion).tag != 0)goto _LL88;_tmp104=(_tmp103.UnknownTunion).f1;
_LL87: if(_tmp104.is_flat)return(void*)1;else{return(void*)2;}_LL88: if(_tmpFE <= (
void*)4)goto _LL8A;if(*((int*)_tmpFE)!= 2)goto _LL8A;_tmp105=((struct Cyc_Absyn_TunionType_struct*)
_tmpFE)->f1;_tmp106=_tmp105.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmpFE)->f1).tunion_info).KnownTunion).tag != 1)goto _LL8A;_tmp107=(_tmp106.KnownTunion).f1;
_tmp108=*_tmp107;_LL89: if(_tmp108->is_flat)return(void*)1;else{return(void*)2;}
_LL8A: if(_tmpFE <= (void*)4)goto _LL8C;if(*((int*)_tmpFE)!= 3)goto _LL8C;_tmp109=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpFE)->f1;_tmp10A=_tmp109.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmpFE)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL8C;_tmp10B=(_tmp10A.KnownTunionfield).f1;_tmp10C=(_tmp10A.KnownTunionfield).f2;
_LL8B: if(_tmp10B->is_flat)return(void*)1;else{if(_tmp10C->typs == 0)return(void*)
2;else{return(void*)1;}}_LL8C: if(_tmpFE <= (void*)4)goto _LL8E;if(*((int*)_tmpFE)
!= 3)goto _LL8E;_tmp10D=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpFE)->f1;
_tmp10E=_tmp10D.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpFE)->f1).field_info).UnknownTunionfield).tag != 0)goto _LL8E;_LL8D:({void*
_tmp11B[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp11C="typ_kind: Unresolved TunionFieldType";_tag_dynforward(_tmp11C,
sizeof(char),_get_zero_arr_size(_tmp11C,37));}),_tag_dynforward(_tmp11B,sizeof(
void*),0));});_LL8E: if(_tmpFE <= (void*)4)goto _LL90;if(*((int*)_tmpFE)!= 12)goto
_LL90;_tmp10F=((struct Cyc_Absyn_EnumType_struct*)_tmpFE)->f2;if(_tmp10F != 0)goto
_LL90;_LL8F: goto _LL91;_LL90: if(_tmpFE <= (void*)4)goto _LL92;if(*((int*)_tmpFE)!= 
10)goto _LL92;_tmp110=((struct Cyc_Absyn_AggrType_struct*)_tmpFE)->f1;_tmp111=
_tmp110.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpFE)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL92;_LL91: return(void*)0;_LL92: if(_tmpFE <= (void*)4)goto _LL94;if(*((
int*)_tmpFE)!= 10)goto _LL94;_tmp112=((struct Cyc_Absyn_AggrType_struct*)_tmpFE)->f1;
_tmp113=_tmp112.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpFE)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL94;_tmp114=(_tmp113.KnownAggr).f1;_tmp115=*_tmp114;_tmp116=*_tmp115;
_tmp117=_tmp116.impl;_LL93: return _tmp117 == 0?(void*)0:(void*)1;_LL94: if(_tmpFE <= (
void*)4)goto _LL96;if(*((int*)_tmpFE)!= 11)goto _LL96;_LL95: goto _LL97;_LL96: if(
_tmpFE <= (void*)4)goto _LL98;if(*((int*)_tmpFE)!= 13)goto _LL98;_LL97: return(void*)
1;_LL98: if(_tmpFE <= (void*)4)goto _LL9A;if(*((int*)_tmpFE)!= 12)goto _LL9A;_tmp118=((
struct Cyc_Absyn_EnumType_struct*)_tmpFE)->f2;_LL99: if(_tmp118->fields == 0)return(
void*)0;else{return(void*)1;}_LL9A: if(_tmpFE <= (void*)4)goto _LL9C;if(*((int*)
_tmpFE)!= 4)goto _LL9C;_tmp119=((struct Cyc_Absyn_PointerType_struct*)_tmpFE)->f1;
_LL9B: {union Cyc_Absyn_Constraint_union _tmp11D=(Cyc_Absyn_compress_conref((
_tmp119.ptr_atts).bounds))->v;void*_tmp11E;void*_tmp11F;void*_tmp120;void*
_tmp121;_LLAF: if((_tmp11D.No_constr).tag != 2)goto _LLB1;_LLB0: goto _LLB2;_LLB1: if((
_tmp11D.Eq_constr).tag != 0)goto _LLB3;_tmp11E=(_tmp11D.Eq_constr).f1;if((int)
_tmp11E != 0)goto _LLB3;_LLB2: return(void*)1;_LLB3: if((_tmp11D.Eq_constr).tag != 0)
goto _LLB5;_tmp11F=(_tmp11D.Eq_constr).f1;if((int)_tmp11F != 1)goto _LLB5;_LLB4:
return(void*)1;_LLB5: if((_tmp11D.Eq_constr).tag != 0)goto _LLB7;_tmp120=(_tmp11D.Eq_constr).f1;
if(_tmp120 <= (void*)2)goto _LLB7;if(*((int*)_tmp120)!= 0)goto _LLB7;_LLB6: goto
_LLB8;_LLB7: if((_tmp11D.Eq_constr).tag != 0)goto _LLB9;_tmp121=(_tmp11D.Eq_constr).f1;
if(_tmp121 <= (void*)2)goto _LLB9;if(*((int*)_tmp121)!= 1)goto _LLB9;_LLB8: return(
void*)2;_LLB9: if((_tmp11D.Forward_constr).tag != 1)goto _LLAE;_LLBA:({void*_tmp122[
0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp123="typ_kind: forward constr in ptr bounds";_tag_dynforward(
_tmp123,sizeof(char),_get_zero_arr_size(_tmp123,39));}),_tag_dynforward(_tmp122,
sizeof(void*),0));});_LLAE:;}_LL9C: if(_tmpFE <= (void*)4)goto _LL9E;if(*((int*)
_tmpFE)!= 14)goto _LL9E;_LL9D: return(void*)2;_LL9E: if(_tmpFE <= (void*)4)goto _LLA0;
if(*((int*)_tmpFE)!= 19)goto _LLA0;_LL9F: return(void*)7;_LLA0: if(_tmpFE <= (void*)
4)goto _LLA2;if(*((int*)_tmpFE)!= 18)goto _LLA2;_LLA1: return(void*)2;_LLA2: if(
_tmpFE <= (void*)4)goto _LLA4;if(*((int*)_tmpFE)!= 7)goto _LLA4;_LLA3: goto _LLA5;
_LLA4: if(_tmpFE <= (void*)4)goto _LLA6;if(*((int*)_tmpFE)!= 9)goto _LLA6;_LLA5:
return(void*)1;_LLA6: if(_tmpFE <= (void*)4)goto _LLA8;if(*((int*)_tmpFE)!= 17)goto
_LLA8;_tmp11A=((struct Cyc_Absyn_TypedefType_struct*)_tmpFE)->f3;_LLA7: if(_tmp11A
== 0  || _tmp11A->kind == 0)({struct Cyc_String_pa_struct _tmp126;_tmp126.tag=0;
_tmp126.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp124[1]={& _tmp126};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp125="typ_kind: typedef found: %s";
_tag_dynforward(_tmp125,sizeof(char),_get_zero_arr_size(_tmp125,28));}),
_tag_dynforward(_tmp124,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp11A->kind))->v;_LLA8: if(_tmpFE <= (void*)4)goto _LLAA;if(*((int*)
_tmpFE)!= 20)goto _LLAA;_LLA9: goto _LLAB;_LLAA: if(_tmpFE <= (void*)4)goto _LLAC;if(*((
int*)_tmpFE)!= 21)goto _LLAC;_LLAB: goto _LLAD;_LLAC: if(_tmpFE <= (void*)4)goto _LL6F;
if(*((int*)_tmpFE)!= 22)goto _LL6F;_LLAD: return(void*)6;_LL6F:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp127;_push_handler(& _tmp127);{int _tmp129=
0;if(setjmp(_tmp127.handler))_tmp129=1;if(!_tmp129){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp12A=1;_npop_handler(0);return _tmp12A;};_pop_handler();}else{void*_tmp128=(
void*)_exn_thrown;void*_tmp12C=_tmp128;_LLBC: if(_tmp12C != Cyc_Tcutil_Unify)goto
_LLBE;_LLBD: return 0;_LLBE:;_LLBF:(void)_throw(_tmp12C);_LLBB:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp12D=t;struct Cyc_Absyn_Tvar*
_tmp12E;struct Cyc_Core_Opt*_tmp12F;struct Cyc_Core_Opt*_tmp130;struct Cyc_Core_Opt**
_tmp131;struct Cyc_Absyn_PtrInfo _tmp132;struct Cyc_Absyn_ArrayInfo _tmp133;void*
_tmp134;struct Cyc_Absyn_FnInfo _tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Core_Opt*
_tmp137;void*_tmp138;struct Cyc_List_List*_tmp139;int _tmp13A;struct Cyc_Absyn_VarargInfo*
_tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*
_tmp13E;struct Cyc_Absyn_TunionInfo _tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_Core_Opt*
_tmp141;struct Cyc_List_List*_tmp142;struct Cyc_Absyn_TunionFieldInfo _tmp143;
struct Cyc_List_List*_tmp144;struct Cyc_Absyn_AggrInfo _tmp145;struct Cyc_List_List*
_tmp146;struct Cyc_List_List*_tmp147;void*_tmp148;void*_tmp149;void*_tmp14A;void*
_tmp14B;void*_tmp14C;struct Cyc_List_List*_tmp14D;_LLC1: if(_tmp12D <= (void*)4)
goto _LLE3;if(*((int*)_tmp12D)!= 1)goto _LLC3;_tmp12E=((struct Cyc_Absyn_VarType_struct*)
_tmp12D)->f1;_LLC2: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp12E)){Cyc_Tcutil_failure_reason=({const char*_tmp14E="(type variable would escape scope)";
_tag_dynforward(_tmp14E,sizeof(char),_get_zero_arr_size(_tmp14E,35));});(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LLC0;_LLC3: if(*((int*)_tmp12D)!= 0)goto
_LLC5;_tmp12F=((struct Cyc_Absyn_Evar_struct*)_tmp12D)->f2;_tmp130=((struct Cyc_Absyn_Evar_struct*)
_tmp12D)->f4;_tmp131=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp12D)->f4;_LLC4: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp14F="(occurs check)";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,15));});(int)
_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmp12F != 0)Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp12F->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp131))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp150=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp131))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp150=({struct Cyc_List_List*_tmp151=
_cycalloc(sizeof(*_tmp151));_tmp151->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp151->tl=
_tmp150;_tmp151;});}}*_tmp131=({struct Cyc_Core_Opt*_tmp152=_cycalloc(sizeof(*
_tmp152));_tmp152->v=_tmp150;_tmp152;});}}}goto _LLC0;_LLC5: if(*((int*)_tmp12D)!= 
4)goto _LLC7;_tmp132=((struct Cyc_Absyn_PointerType_struct*)_tmp12D)->f1;_LLC6: Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp132.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp132.ptr_atts).rgn);{
union Cyc_Absyn_Constraint_union _tmp153=(Cyc_Absyn_compress_conref((_tmp132.ptr_atts).bounds))->v;
void*_tmp154;void*_tmp155;_LLE6: if((_tmp153.Eq_constr).tag != 0)goto _LLE8;_tmp154=(
_tmp153.Eq_constr).f1;if(_tmp154 <= (void*)2)goto _LLE8;if(*((int*)_tmp154)!= 1)
goto _LLE8;_tmp155=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp154)->f1;_LLE7:
Cyc_Tcutil_occurs(evar,r,env,_tmp155);goto _LLE5;_LLE8:;_LLE9: goto _LLE5;_LLE5:;}
goto _LLC0;_LLC7: if(*((int*)_tmp12D)!= 7)goto _LLC9;_tmp133=((struct Cyc_Absyn_ArrayType_struct*)
_tmp12D)->f1;_tmp134=(void*)_tmp133.elt_type;_LLC8: Cyc_Tcutil_occurs(evar,r,env,
_tmp134);goto _LLC0;_LLC9: if(*((int*)_tmp12D)!= 8)goto _LLCB;_tmp135=((struct Cyc_Absyn_FnType_struct*)
_tmp12D)->f1;_tmp136=_tmp135.tvars;_tmp137=_tmp135.effect;_tmp138=(void*)_tmp135.ret_typ;
_tmp139=_tmp135.args;_tmp13A=_tmp135.c_varargs;_tmp13B=_tmp135.cyc_varargs;
_tmp13C=_tmp135.rgn_po;_tmp13D=_tmp135.attributes;_LLCA: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp136,env);if(_tmp137 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp137->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp138);for(0;_tmp139 != 0;_tmp139=_tmp139->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple3*)_tmp139->hd)).f3);}if(_tmp13B != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp13B->type);for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){struct
_tuple0 _tmp157;void*_tmp158;void*_tmp159;struct _tuple0*_tmp156=(struct _tuple0*)
_tmp13C->hd;_tmp157=*_tmp156;_tmp158=_tmp157.f1;_tmp159=_tmp157.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp158);Cyc_Tcutil_occurs(evar,r,env,_tmp159);}goto _LLC0;_LLCB: if(*((
int*)_tmp12D)!= 9)goto _LLCD;_tmp13E=((struct Cyc_Absyn_TupleType_struct*)_tmp12D)->f1;
_LLCC: for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple5*)_tmp13E->hd)).f2);}goto _LLC0;_LLCD: if(*((int*)_tmp12D)!= 2)goto
_LLCF;_tmp13F=((struct Cyc_Absyn_TunionType_struct*)_tmp12D)->f1;_tmp140=_tmp13F.targs;
_tmp141=_tmp13F.rgn;_LLCE: if((unsigned int)_tmp141)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp141->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp140);goto _LLC0;_LLCF: if(*((
int*)_tmp12D)!= 17)goto _LLD1;_tmp142=((struct Cyc_Absyn_TypedefType_struct*)
_tmp12D)->f2;_LLD0: _tmp144=_tmp142;goto _LLD2;_LLD1: if(*((int*)_tmp12D)!= 3)goto
_LLD3;_tmp143=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp12D)->f1;_tmp144=
_tmp143.targs;_LLD2: _tmp146=_tmp144;goto _LLD4;_LLD3: if(*((int*)_tmp12D)!= 10)
goto _LLD5;_tmp145=((struct Cyc_Absyn_AggrType_struct*)_tmp12D)->f1;_tmp146=
_tmp145.targs;_LLD4: Cyc_Tcutil_occurslist(evar,r,env,_tmp146);goto _LLC0;_LLD5:
if(*((int*)_tmp12D)!= 11)goto _LLD7;_tmp147=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp12D)->f2;_LLD6: for(0;_tmp147 != 0;_tmp147=_tmp147->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp147->hd)->type);}goto _LLC0;_LLD7:
if(*((int*)_tmp12D)!= 18)goto _LLD9;_tmp148=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp12D)->f1;_LLD8: _tmp149=_tmp148;goto _LLDA;_LLD9: if(*((int*)_tmp12D)!= 14)goto
_LLDB;_tmp149=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp12D)->f1;_LLDA:
_tmp14A=_tmp149;goto _LLDC;_LLDB: if(*((int*)_tmp12D)!= 20)goto _LLDD;_tmp14A=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp12D)->f1;_LLDC: _tmp14B=_tmp14A;goto _LLDE;
_LLDD: if(*((int*)_tmp12D)!= 15)goto _LLDF;_tmp14B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp12D)->f1;_LLDE: _tmp14C=_tmp14B;goto _LLE0;_LLDF: if(*((int*)_tmp12D)!= 22)goto
_LLE1;_tmp14C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp12D)->f1;_LLE0: Cyc_Tcutil_occurs(
evar,r,env,_tmp14C);goto _LLC0;_LLE1: if(*((int*)_tmp12D)!= 21)goto _LLE3;_tmp14D=((
struct Cyc_Absyn_JoinEff_struct*)_tmp12D)->f1;_LLE2: Cyc_Tcutil_occurslist(evar,r,
env,_tmp14D);goto _LLC0;_LLE3:;_LLE4: goto _LLC0;_LLC0:;}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp15A[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp15B="tq1 real_const not set.";
_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,24));}),
_tag_dynforward(_tmp15A,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp15C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp15D="tq2 real_const not set.";_tag_dynforward(
_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,24));}),_tag_dynforward(_tmp15C,
sizeof(void*),0));});if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({
const char*_tmp15E="(qualifiers don't match)";_tag_dynforward(_tmp15E,sizeof(char),
_get_zero_arr_size(_tmp15E,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dynforward_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint_union _tmp15F=x->v;void*_tmp160;
_LLEB: if((_tmp15F.No_constr).tag != 2)goto _LLED;_LLEC: x->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp161;(_tmp161.Forward_constr).tag=1;(_tmp161.Forward_constr).f1=
y;_tmp161;});return;_LLED: if((_tmp15F.Eq_constr).tag != 0)goto _LLEF;_tmp160=(
_tmp15F.Eq_constr).f1;_LLEE: {union Cyc_Absyn_Constraint_union _tmp162=y->v;void*
_tmp163;_LLF2: if((_tmp162.No_constr).tag != 2)goto _LLF4;_LLF3: y->v=x->v;return;
_LLF4: if((_tmp162.Eq_constr).tag != 0)goto _LLF6;_tmp163=(_tmp162.Eq_constr).f1;
_LLF5: if(cmp(_tmp160,_tmp163)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLF6: if((_tmp162.Forward_constr).tag != 1)goto
_LLF1;_LLF7:({void*_tmp164[0]={};Cyc_Tcutil_impos(({const char*_tmp165="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,40));}),
_tag_dynforward(_tmp164,sizeof(void*),0));});_LLF1:;}_LLEF: if((_tmp15F.Forward_constr).tag
!= 1)goto _LLEA;_LLF0:({void*_tmp166[0]={};Cyc_Tcutil_impos(({const char*_tmp167="unify_conref: forward after compress";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,37));}),
_tag_dynforward(_tmp166,sizeof(void*),0));});_LLEA:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp168;_push_handler(& _tmp168);{int _tmp16A=0;if(setjmp(_tmp168.handler))
_tmp16A=1;if(!_tmp16A){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct
_dynforward_ptr)_tag_dynforward(0,0,0));{int _tmp16B=1;_npop_handler(0);return
_tmp16B;};_pop_handler();}else{void*_tmp169=(void*)_exn_thrown;void*_tmp16D=
_tmp169;_LLF9: if(_tmp16D != Cyc_Tcutil_Unify)goto _LLFB;_LLFA: return 0;_LLFB:;_LLFC:(
void)_throw(_tmp16D);_LLF8:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp16F=({struct _tuple0 _tmp16E;_tmp16E.f1=b1;_tmp16E.f2=b2;_tmp16E;});
void*_tmp170;void*_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;
void*_tmp176;void*_tmp177;void*_tmp178;struct Cyc_Absyn_Exp*_tmp179;void*_tmp17A;
struct Cyc_Absyn_Exp*_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;
void*_tmp180;void*_tmp181;void*_tmp182;void*_tmp183;_LLFE: _tmp170=_tmp16F.f1;if((
int)_tmp170 != 0)goto _LL100;_tmp171=_tmp16F.f2;if((int)_tmp171 != 0)goto _LL100;
_LLFF: return 0;_LL100: _tmp172=_tmp16F.f1;if((int)_tmp172 != 0)goto _LL102;_LL101:
return - 1;_LL102: _tmp173=_tmp16F.f2;if((int)_tmp173 != 0)goto _LL104;_LL103: return 1;
_LL104: _tmp174=_tmp16F.f1;if((int)_tmp174 != 1)goto _LL106;_tmp175=_tmp16F.f2;if((
int)_tmp175 != 1)goto _LL106;_LL105: return 0;_LL106: _tmp176=_tmp16F.f1;if((int)
_tmp176 != 1)goto _LL108;_LL107: return - 1;_LL108: _tmp177=_tmp16F.f2;if((int)_tmp177
!= 1)goto _LL10A;_LL109: return 1;_LL10A: _tmp178=_tmp16F.f1;if(_tmp178 <= (void*)2)
goto _LL10C;if(*((int*)_tmp178)!= 0)goto _LL10C;_tmp179=((struct Cyc_Absyn_Upper_b_struct*)
_tmp178)->f1;_tmp17A=_tmp16F.f2;if(_tmp17A <= (void*)2)goto _LL10C;if(*((int*)
_tmp17A)!= 0)goto _LL10C;_tmp17B=((struct Cyc_Absyn_Upper_b_struct*)_tmp17A)->f1;
_LL10B: return Cyc_Evexp_const_exp_cmp(_tmp179,_tmp17B);_LL10C: _tmp17C=_tmp16F.f1;
if(_tmp17C <= (void*)2)goto _LL10E;if(*((int*)_tmp17C)!= 0)goto _LL10E;_tmp17D=
_tmp16F.f2;if(_tmp17D <= (void*)2)goto _LL10E;if(*((int*)_tmp17D)!= 1)goto _LL10E;
_LL10D: return - 1;_LL10E: _tmp17E=_tmp16F.f1;if(_tmp17E <= (void*)2)goto _LL110;if(*((
int*)_tmp17E)!= 1)goto _LL110;_tmp17F=_tmp16F.f2;if(_tmp17F <= (void*)2)goto _LL110;
if(*((int*)_tmp17F)!= 0)goto _LL110;_LL10F: return 1;_LL110: _tmp180=_tmp16F.f1;if(
_tmp180 <= (void*)2)goto _LLFD;if(*((int*)_tmp180)!= 1)goto _LLFD;_tmp181=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp180)->f1;_tmp182=_tmp16F.f2;if(_tmp182 <= (
void*)2)goto _LLFD;if(*((int*)_tmp182)!= 1)goto _LLFD;_tmp183=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp182)->f1;_LL111: return Cyc_Tcutil_typecmp(_tmp181,_tmp183);_LLFD:;}static int
Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0 _tmp185=({struct _tuple0
_tmp184;_tmp184.f1=b1;_tmp184.f2=b2;_tmp184;});void*_tmp186;void*_tmp187;void*
_tmp188;void*_tmp189;void*_tmp18A;void*_tmp18B;void*_tmp18C;void*_tmp18D;void*
_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;void*_tmp190;struct Cyc_Absyn_Exp*_tmp191;
void*_tmp192;struct Cyc_Absyn_Exp*_tmp193;void*_tmp194;void*_tmp195;void*_tmp196;
void*_tmp197;void*_tmp198;struct Cyc_Absyn_Exp*_tmp199;void*_tmp19A;void*_tmp19B;
void*_tmp19C;void*_tmp19D;_LL113: _tmp186=_tmp185.f1;if((int)_tmp186 != 0)goto
_LL115;_tmp187=_tmp185.f2;if((int)_tmp187 != 0)goto _LL115;_LL114: return 0;_LL115:
_tmp188=_tmp185.f1;if((int)_tmp188 != 0)goto _LL117;_LL116: return - 1;_LL117: _tmp189=
_tmp185.f2;if((int)_tmp189 != 0)goto _LL119;_LL118: return 1;_LL119: _tmp18A=_tmp185.f1;
if((int)_tmp18A != 1)goto _LL11B;_tmp18B=_tmp185.f2;if((int)_tmp18B != 1)goto _LL11B;
_LL11A: return 0;_LL11B: _tmp18C=_tmp185.f1;if((int)_tmp18C != 1)goto _LL11D;_LL11C:
return - 1;_LL11D: _tmp18D=_tmp185.f2;if((int)_tmp18D != 1)goto _LL11F;_LL11E: return 1;
_LL11F: _tmp18E=_tmp185.f1;if(_tmp18E <= (void*)2)goto _LL121;if(*((int*)_tmp18E)!= 
0)goto _LL121;_tmp18F=((struct Cyc_Absyn_Upper_b_struct*)_tmp18E)->f1;_tmp190=
_tmp185.f2;if(_tmp190 <= (void*)2)goto _LL121;if(*((int*)_tmp190)!= 0)goto _LL121;
_tmp191=((struct Cyc_Absyn_Upper_b_struct*)_tmp190)->f1;_LL120: return Cyc_Evexp_const_exp_cmp(
_tmp18F,_tmp191);_LL121: _tmp192=_tmp185.f1;if(_tmp192 <= (void*)2)goto _LL123;if(*((
int*)_tmp192)!= 0)goto _LL123;_tmp193=((struct Cyc_Absyn_Upper_b_struct*)_tmp192)->f1;
_tmp194=_tmp185.f2;if(_tmp194 <= (void*)2)goto _LL123;if(*((int*)_tmp194)!= 1)goto
_LL123;_tmp195=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp194)->f1;_LL122:
_tmp197=_tmp195;_tmp199=_tmp193;goto _LL124;_LL123: _tmp196=_tmp185.f1;if(_tmp196
<= (void*)2)goto _LL125;if(*((int*)_tmp196)!= 1)goto _LL125;_tmp197=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp196)->f1;_tmp198=_tmp185.f2;if(_tmp198 <= (
void*)2)goto _LL125;if(*((int*)_tmp198)!= 0)goto _LL125;_tmp199=((struct Cyc_Absyn_Upper_b_struct*)
_tmp198)->f1;_LL124: {unsigned int _tmp19F;int _tmp1A0;struct _tuple7 _tmp19E=Cyc_Evexp_eval_const_uint_exp(
_tmp199);_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19E.f2;if(!_tmp1A0)return 1;_tmp19B=
_tmp197;_tmp19D=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1A1=_cycalloc_atomic(
sizeof(*_tmp1A1));_tmp1A1[0]=({struct Cyc_Absyn_TypeInt_struct _tmp1A2;_tmp1A2.tag=
19;_tmp1A2.f1=(int)_tmp19F;_tmp1A2;});_tmp1A1;});goto _LL126;}_LL125: _tmp19A=
_tmp185.f1;if(_tmp19A <= (void*)2)goto _LL112;if(*((int*)_tmp19A)!= 1)goto _LL112;
_tmp19B=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp19A)->f1;_tmp19C=_tmp185.f2;
if(_tmp19C <= (void*)2)goto _LL112;if(*((int*)_tmp19C)!= 1)goto _LL112;_tmp19D=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp19C)->f1;_LL126: Cyc_Tcutil_unify_it(
_tmp19B,_tmp19D);return 0;_LL112:;}static int Cyc_Tcutil_attribute_case_number(void*
att){void*_tmp1A3=att;_LL128: if(_tmp1A3 <= (void*)17)goto _LL12A;if(*((int*)
_tmp1A3)!= 0)goto _LL12A;_LL129: return 0;_LL12A: if((int)_tmp1A3 != 0)goto _LL12C;
_LL12B: return 1;_LL12C: if((int)_tmp1A3 != 1)goto _LL12E;_LL12D: return 2;_LL12E: if((
int)_tmp1A3 != 2)goto _LL130;_LL12F: return 3;_LL130: if((int)_tmp1A3 != 3)goto _LL132;
_LL131: return 4;_LL132: if((int)_tmp1A3 != 4)goto _LL134;_LL133: return 5;_LL134: if(
_tmp1A3 <= (void*)17)goto _LL136;if(*((int*)_tmp1A3)!= 1)goto _LL136;_LL135: return 6;
_LL136: if((int)_tmp1A3 != 5)goto _LL138;_LL137: return 7;_LL138: if(_tmp1A3 <= (void*)
17)goto _LL13A;if(*((int*)_tmp1A3)!= 2)goto _LL13A;_LL139: return 8;_LL13A: if((int)
_tmp1A3 != 6)goto _LL13C;_LL13B: return 9;_LL13C: if((int)_tmp1A3 != 7)goto _LL13E;
_LL13D: return 10;_LL13E: if((int)_tmp1A3 != 8)goto _LL140;_LL13F: return 11;_LL140: if((
int)_tmp1A3 != 9)goto _LL142;_LL141: return 12;_LL142: if((int)_tmp1A3 != 10)goto
_LL144;_LL143: return 13;_LL144: if((int)_tmp1A3 != 11)goto _LL146;_LL145: return 14;
_LL146: if((int)_tmp1A3 != 12)goto _LL148;_LL147: return 15;_LL148: if((int)_tmp1A3 != 
13)goto _LL14A;_LL149: return 16;_LL14A: if((int)_tmp1A3 != 14)goto _LL14C;_LL14B:
return 17;_LL14C: if((int)_tmp1A3 != 15)goto _LL14E;_LL14D: return 18;_LL14E: if(
_tmp1A3 <= (void*)17)goto _LL152;if(*((int*)_tmp1A3)!= 3)goto _LL150;_LL14F: return
19;_LL150: if(*((int*)_tmp1A3)!= 4)goto _LL152;_LL151: return 20;_LL152:;_LL153:
return 21;_LL127:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple0 _tmp1A5=({struct _tuple0 _tmp1A4;_tmp1A4.f1=att1;_tmp1A4.f2=att2;_tmp1A4;});
void*_tmp1A6;int _tmp1A7;void*_tmp1A8;int _tmp1A9;void*_tmp1AA;int _tmp1AB;void*
_tmp1AC;int _tmp1AD;void*_tmp1AE;int _tmp1AF;void*_tmp1B0;int _tmp1B1;void*_tmp1B2;
struct _dynforward_ptr _tmp1B3;void*_tmp1B4;struct _dynforward_ptr _tmp1B5;void*
_tmp1B6;void*_tmp1B7;int _tmp1B8;int _tmp1B9;void*_tmp1BA;void*_tmp1BB;int _tmp1BC;
int _tmp1BD;_LL155: _tmp1A6=_tmp1A5.f1;if(_tmp1A6 <= (void*)17)goto _LL157;if(*((int*)
_tmp1A6)!= 0)goto _LL157;_tmp1A7=((struct Cyc_Absyn_Regparm_att_struct*)_tmp1A6)->f1;
_tmp1A8=_tmp1A5.f2;if(_tmp1A8 <= (void*)17)goto _LL157;if(*((int*)_tmp1A8)!= 0)
goto _LL157;_tmp1A9=((struct Cyc_Absyn_Regparm_att_struct*)_tmp1A8)->f1;_LL156:
_tmp1AB=_tmp1A7;_tmp1AD=_tmp1A9;goto _LL158;_LL157: _tmp1AA=_tmp1A5.f1;if(_tmp1AA
<= (void*)17)goto _LL159;if(*((int*)_tmp1AA)!= 4)goto _LL159;_tmp1AB=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1AA)->f1;_tmp1AC=_tmp1A5.f2;if(_tmp1AC <= (void*)17)goto _LL159;if(*((int*)
_tmp1AC)!= 4)goto _LL159;_tmp1AD=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1AC)->f1;_LL158: _tmp1AF=_tmp1AB;_tmp1B1=_tmp1AD;goto _LL15A;_LL159: _tmp1AE=
_tmp1A5.f1;if(_tmp1AE <= (void*)17)goto _LL15B;if(*((int*)_tmp1AE)!= 1)goto _LL15B;
_tmp1AF=((struct Cyc_Absyn_Aligned_att_struct*)_tmp1AE)->f1;_tmp1B0=_tmp1A5.f2;
if(_tmp1B0 <= (void*)17)goto _LL15B;if(*((int*)_tmp1B0)!= 1)goto _LL15B;_tmp1B1=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp1B0)->f1;_LL15A: return Cyc_Core_intcmp(
_tmp1AF,_tmp1B1);_LL15B: _tmp1B2=_tmp1A5.f1;if(_tmp1B2 <= (void*)17)goto _LL15D;if(*((
int*)_tmp1B2)!= 2)goto _LL15D;_tmp1B3=((struct Cyc_Absyn_Section_att_struct*)
_tmp1B2)->f1;_tmp1B4=_tmp1A5.f2;if(_tmp1B4 <= (void*)17)goto _LL15D;if(*((int*)
_tmp1B4)!= 2)goto _LL15D;_tmp1B5=((struct Cyc_Absyn_Section_att_struct*)_tmp1B4)->f1;
_LL15C: return Cyc_strcmp((struct _dynforward_ptr)_tmp1B3,(struct _dynforward_ptr)
_tmp1B5);_LL15D: _tmp1B6=_tmp1A5.f1;if(_tmp1B6 <= (void*)17)goto _LL15F;if(*((int*)
_tmp1B6)!= 3)goto _LL15F;_tmp1B7=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1B6)->f1;_tmp1B8=((struct Cyc_Absyn_Format_att_struct*)_tmp1B6)->f2;_tmp1B9=((
struct Cyc_Absyn_Format_att_struct*)_tmp1B6)->f3;_tmp1BA=_tmp1A5.f2;if(_tmp1BA <= (
void*)17)goto _LL15F;if(*((int*)_tmp1BA)!= 3)goto _LL15F;_tmp1BB=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1BA)->f1;_tmp1BC=((struct Cyc_Absyn_Format_att_struct*)
_tmp1BA)->f2;_tmp1BD=((struct Cyc_Absyn_Format_att_struct*)_tmp1BA)->f3;_LL15E: {
int _tmp1BE=Cyc_Core_intcmp((int)((unsigned int)_tmp1B7),(int)((unsigned int)
_tmp1BB));if(_tmp1BE != 0)return _tmp1BE;{int _tmp1BF=Cyc_Core_intcmp(_tmp1B8,
_tmp1BC);if(_tmp1BF != 0)return _tmp1BF;return Cyc_Core_intcmp(_tmp1B9,_tmp1BD);}}
_LL15F:;_LL160: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL154:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct Cyc_Absyn_TypeInt_struct Cyc_Tcutil_tizero={
19,0};static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;{void*_tmp1C1=Cyc_Tcutil_tvar_kind(tv);_LL162: if((int)_tmp1C1 != 4)goto
_LL164;_LL163: t=(void*)3;goto _LL161;_LL164: if((int)_tmp1C1 != 3)goto _LL166;_LL165:
t=(void*)2;goto _LL161;_LL166: if((int)_tmp1C1 != 6)goto _LL168;_LL167: t=Cyc_Absyn_empty_effect;
goto _LL161;_LL168: if((int)_tmp1C1 != 7)goto _LL16A;_LL169: t=(void*)& Cyc_Tcutil_tizero;
goto _LL161;_LL16A:;_LL16B: t=Cyc_Absyn_sint_typ;goto _LL161;_LL161:;}return({
struct _tuple8*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->f1=tv;_tmp1C2->f2=t;
_tmp1C2;});}static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp1C3=Cyc_Tcutil_compress(
t);void*_tmp1C4;void*_tmp1C5;void*_tmp1C6;struct Cyc_Absyn_TunionInfo _tmp1C7;
struct Cyc_List_List*_tmp1C8;struct Cyc_Core_Opt*_tmp1C9;struct Cyc_Absyn_PtrInfo
_tmp1CA;void*_tmp1CB;struct Cyc_Absyn_PtrAtts _tmp1CC;void*_tmp1CD;struct Cyc_Absyn_ArrayInfo
_tmp1CE;void*_tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_TunionFieldInfo
_tmp1D1;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_AggrInfo _tmp1D3;struct Cyc_List_List*
_tmp1D4;struct Cyc_List_List*_tmp1D5;void*_tmp1D6;struct Cyc_Absyn_FnInfo _tmp1D7;
struct Cyc_List_List*_tmp1D8;struct Cyc_Core_Opt*_tmp1D9;void*_tmp1DA;struct Cyc_List_List*
_tmp1DB;struct Cyc_Absyn_VarargInfo*_tmp1DC;struct Cyc_List_List*_tmp1DD;void*
_tmp1DE;struct Cyc_List_List*_tmp1DF;_LL16D: if((int)_tmp1C3 != 0)goto _LL16F;_LL16E:
goto _LL170;_LL16F: if((int)_tmp1C3 != 1)goto _LL171;_LL170: goto _LL172;_LL171: if(
_tmp1C3 <= (void*)4)goto _LL197;if(*((int*)_tmp1C3)!= 6)goto _LL173;_LL172: goto
_LL174;_LL173: if(*((int*)_tmp1C3)!= 12)goto _LL175;_LL174: goto _LL176;_LL175: if(*((
int*)_tmp1C3)!= 13)goto _LL177;_LL176: goto _LL178;_LL177: if(*((int*)_tmp1C3)!= 19)
goto _LL179;_LL178: goto _LL17A;_LL179: if(*((int*)_tmp1C3)!= 5)goto _LL17B;_LL17A:
return Cyc_Absyn_empty_effect;_LL17B: if(*((int*)_tmp1C3)!= 0)goto _LL17D;_LL17C:
goto _LL17E;_LL17D: if(*((int*)_tmp1C3)!= 1)goto _LL17F;_LL17E: {void*_tmp1E0=Cyc_Tcutil_typ_kind(
t);_LL1A4: if((int)_tmp1E0 != 3)goto _LL1A6;_LL1A5: goto _LL1A7;_LL1A6: if((int)
_tmp1E0 != 4)goto _LL1A8;_LL1A7: goto _LL1A9;_LL1A8: if((int)_tmp1E0 != 5)goto _LL1AA;
_LL1A9: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E1=_cycalloc(sizeof(*
_tmp1E1));_tmp1E1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E2;_tmp1E2.tag=20;
_tmp1E2.f1=(void*)t;_tmp1E2;});_tmp1E1;});_LL1AA: if((int)_tmp1E0 != 6)goto _LL1AC;
_LL1AB: return t;_LL1AC: if((int)_tmp1E0 != 7)goto _LL1AE;_LL1AD: return Cyc_Absyn_empty_effect;
_LL1AE:;_LL1AF: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1E3=_cycalloc(
sizeof(*_tmp1E3));_tmp1E3[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1E4;_tmp1E4.tag=
22;_tmp1E4.f1=(void*)t;_tmp1E4;});_tmp1E3;});_LL1A3:;}_LL17F: if(*((int*)_tmp1C3)
!= 15)goto _LL181;_tmp1C4=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp1C3)->f1;
_LL180: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E5=_cycalloc(sizeof(*
_tmp1E5));_tmp1E5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E6;_tmp1E6.tag=20;
_tmp1E6.f1=(void*)_tmp1C4;_tmp1E6;});_tmp1E5;});_LL181: if(*((int*)_tmp1C3)!= 16)
goto _LL183;_tmp1C5=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp1C3)->f1;
_tmp1C6=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp1C3)->f2;_LL182: return(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));
_tmp1E7[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E8;_tmp1E8.tag=20;_tmp1E8.f1=(
void*)_tmp1C6;_tmp1E8;});_tmp1E7;});_LL183: if(*((int*)_tmp1C3)!= 2)goto _LL185;
_tmp1C7=((struct Cyc_Absyn_TunionType_struct*)_tmp1C3)->f1;_tmp1C8=_tmp1C7.targs;
_tmp1C9=_tmp1C7.rgn;_LL184: {struct Cyc_List_List*ts=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1C8);if((unsigned int)_tmp1C9)ts=({struct Cyc_List_List*_tmp1E9=_cycalloc(
sizeof(*_tmp1E9));_tmp1E9->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1EB;_tmp1EB.tag=20;_tmp1EB.f1=(void*)((void*)_tmp1C9->v);_tmp1EB;});_tmp1EA;}));
_tmp1E9->tl=ts;_tmp1E9;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1ED;_tmp1ED.tag=21;_tmp1ED.f1=ts;_tmp1ED;});_tmp1EC;}));}_LL185: if(*((int*)
_tmp1C3)!= 4)goto _LL187;_tmp1CA=((struct Cyc_Absyn_PointerType_struct*)_tmp1C3)->f1;
_tmp1CB=(void*)_tmp1CA.elt_typ;_tmp1CC=_tmp1CA.ptr_atts;_tmp1CD=(void*)_tmp1CC.rgn;
_LL186: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1EF;_tmp1EF.tag=21;_tmp1EF.f1=({void*_tmp1F0[2];_tmp1F0[1]=Cyc_Tcutil_rgns_of(
_tmp1CB);_tmp1F0[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1F1=_cycalloc(
sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1F2;_tmp1F2.tag=
20;_tmp1F2.f1=(void*)_tmp1CD;_tmp1F2;});_tmp1F1;});Cyc_List_list(_tag_dynforward(
_tmp1F0,sizeof(void*),2));});_tmp1EF;});_tmp1EE;}));_LL187: if(*((int*)_tmp1C3)!= 
7)goto _LL189;_tmp1CE=((struct Cyc_Absyn_ArrayType_struct*)_tmp1C3)->f1;_tmp1CF=(
void*)_tmp1CE.elt_type;_LL188: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp1CF));_LL189: if(*((int*)_tmp1C3)!= 9)goto _LL18B;_tmp1D0=((struct Cyc_Absyn_TupleType_struct*)
_tmp1C3)->f1;_LL18A: {struct Cyc_List_List*_tmp1F3=0;for(0;_tmp1D0 != 0;_tmp1D0=
_tmp1D0->tl){_tmp1F3=({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));
_tmp1F4->hd=(void*)(*((struct _tuple5*)_tmp1D0->hd)).f2;_tmp1F4->tl=_tmp1F3;
_tmp1F4;});}_tmp1D2=_tmp1F3;goto _LL18C;}_LL18B: if(*((int*)_tmp1C3)!= 3)goto
_LL18D;_tmp1D1=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1C3)->f1;_tmp1D2=
_tmp1D1.targs;_LL18C: _tmp1D4=_tmp1D2;goto _LL18E;_LL18D: if(*((int*)_tmp1C3)!= 10)
goto _LL18F;_tmp1D3=((struct Cyc_Absyn_AggrType_struct*)_tmp1C3)->f1;_tmp1D4=
_tmp1D3.targs;_LL18E: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1F6;_tmp1F6.tag=21;_tmp1F6.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1D4);
_tmp1F6;});_tmp1F5;}));_LL18F: if(*((int*)_tmp1C3)!= 11)goto _LL191;_tmp1D5=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C3)->f2;_LL190: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));
_tmp1F7[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F8;_tmp1F8.tag=21;_tmp1F8.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1D5);_tmp1F8;});_tmp1F7;}));_LL191:
if(*((int*)_tmp1C3)!= 14)goto _LL193;_tmp1D6=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1C3)->f1;_LL192: return Cyc_Tcutil_rgns_of(_tmp1D6);_LL193: if(*((int*)_tmp1C3)
!= 18)goto _LL195;_LL194: return Cyc_Absyn_empty_effect;_LL195: if(*((int*)_tmp1C3)
!= 8)goto _LL197;_tmp1D7=((struct Cyc_Absyn_FnType_struct*)_tmp1C3)->f1;_tmp1D8=
_tmp1D7.tvars;_tmp1D9=_tmp1D7.effect;_tmp1DA=(void*)_tmp1D7.ret_typ;_tmp1DB=
_tmp1D7.args;_tmp1DC=_tmp1D7.cyc_varargs;_tmp1DD=_tmp1D7.rgn_po;_LL196: {void*
_tmp1F9=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1D8),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D9))->v);return Cyc_Tcutil_normalize_effect(
_tmp1F9);}_LL197: if((int)_tmp1C3 != 3)goto _LL199;_LL198: goto _LL19A;_LL199: if((int)
_tmp1C3 != 2)goto _LL19B;_LL19A: return Cyc_Absyn_empty_effect;_LL19B: if(_tmp1C3 <= (
void*)4)goto _LL19D;if(*((int*)_tmp1C3)!= 20)goto _LL19D;_LL19C: goto _LL19E;_LL19D:
if(_tmp1C3 <= (void*)4)goto _LL19F;if(*((int*)_tmp1C3)!= 21)goto _LL19F;_LL19E:
return t;_LL19F: if(_tmp1C3 <= (void*)4)goto _LL1A1;if(*((int*)_tmp1C3)!= 22)goto
_LL1A1;_tmp1DE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1C3)->f1;_LL1A0:
return Cyc_Tcutil_rgns_of(_tmp1DE);_LL1A1: if(_tmp1C3 <= (void*)4)goto _LL16C;if(*((
int*)_tmp1C3)!= 17)goto _LL16C;_tmp1DF=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1C3)->f2;_LL1A2: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1FB;_tmp1FB.tag=21;_tmp1FB.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1DF);
_tmp1FB;});_tmp1FA;}));_LL16C:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1FC=e;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List**_tmp1FE;void*
_tmp1FF;_LL1B1: if(_tmp1FC <= (void*)4)goto _LL1B5;if(*((int*)_tmp1FC)!= 21)goto
_LL1B3;_tmp1FD=((struct Cyc_Absyn_JoinEff_struct*)_tmp1FC)->f1;_tmp1FE=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1FC)->f1;_LL1B2: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1FE;for(0;effs != 0;effs=effs->tl){void*_tmp200=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp200)));{void*
_tmp201=(void*)effs->hd;void*_tmp202;_LL1B8: if(_tmp201 <= (void*)4)goto _LL1BC;if(*((
int*)_tmp201)!= 21)goto _LL1BA;_LL1B9: goto _LL1BB;_LL1BA: if(*((int*)_tmp201)!= 20)
goto _LL1BC;_tmp202=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp201)->f1;if((
int)_tmp202 != 2)goto _LL1BC;_LL1BB: redo_join=1;goto _LL1B7;_LL1BC:;_LL1BD: goto
_LL1B7;_LL1B7:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1FE;for(0;effs != 0;effs=effs->tl){void*_tmp203=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp204;void*_tmp205;_LL1BF: if(_tmp203 <= (
void*)4)goto _LL1C3;if(*((int*)_tmp203)!= 21)goto _LL1C1;_tmp204=((struct Cyc_Absyn_JoinEff_struct*)
_tmp203)->f1;_LL1C0: effects=Cyc_List_revappend(_tmp204,effects);goto _LL1BE;
_LL1C1: if(*((int*)_tmp203)!= 20)goto _LL1C3;_tmp205=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp203)->f1;if((int)_tmp205 != 2)goto _LL1C3;_LL1C2: goto _LL1BE;_LL1C3:;_LL1C4:
effects=({struct Cyc_List_List*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->hd=(
void*)_tmp203;_tmp206->tl=effects;_tmp206;});goto _LL1BE;_LL1BE:;}}*_tmp1FE=Cyc_List_imp_rev(
effects);return e;}}_LL1B3: if(*((int*)_tmp1FC)!= 22)goto _LL1B5;_tmp1FF=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1FC)->f1;_LL1B4: {void*_tmp207=Cyc_Tcutil_compress(
_tmp1FF);_LL1C6: if(_tmp207 <= (void*)4)goto _LL1CA;if(*((int*)_tmp207)!= 0)goto
_LL1C8;_LL1C7: goto _LL1C9;_LL1C8: if(*((int*)_tmp207)!= 1)goto _LL1CA;_LL1C9: return
e;_LL1CA:;_LL1CB: return Cyc_Tcutil_rgns_of(_tmp1FF);_LL1C5:;}_LL1B5:;_LL1B6:
return e;_LL1B0:;}}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)6)};
static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp208=({struct Cyc_Absyn_FnType_struct*_tmp209=_cycalloc(sizeof(*_tmp209));
_tmp209[0]=({struct Cyc_Absyn_FnType_struct _tmp20A;_tmp20A.tag=8;_tmp20A.f1=({
struct Cyc_Absyn_FnInfo _tmp20B;_tmp20B.tvars=0;_tmp20B.effect=({struct Cyc_Core_Opt*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->v=(void*)eff;_tmp20C;});_tmp20B.ret_typ=(
void*)((void*)0);_tmp20B.args=0;_tmp20B.c_varargs=0;_tmp20B.cyc_varargs=0;
_tmp20B.rgn_po=0;_tmp20B.attributes=0;_tmp20B;});_tmp20A;});_tmp209;});return Cyc_Absyn_atb_typ((
void*)_tmp208,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2  || r == (void*)3)return 1;{void*_tmp20D=Cyc_Tcutil_compress(e);
void*_tmp20E;struct Cyc_List_List*_tmp20F;void*_tmp210;struct Cyc_Core_Opt*_tmp211;
struct Cyc_Core_Opt*_tmp212;struct Cyc_Core_Opt**_tmp213;struct Cyc_Core_Opt*
_tmp214;_LL1CD: if(_tmp20D <= (void*)4)goto _LL1D5;if(*((int*)_tmp20D)!= 20)goto
_LL1CF;_tmp20E=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp20D)->f1;_LL1CE:
if(constrain)return Cyc_Tcutil_unify(r,_tmp20E);_tmp20E=Cyc_Tcutil_compress(
_tmp20E);if(r == _tmp20E)return 1;{struct _tuple0 _tmp216=({struct _tuple0 _tmp215;
_tmp215.f1=r;_tmp215.f2=_tmp20E;_tmp215;});void*_tmp217;struct Cyc_Absyn_Tvar*
_tmp218;void*_tmp219;struct Cyc_Absyn_Tvar*_tmp21A;_LL1D8: _tmp217=_tmp216.f1;if(
_tmp217 <= (void*)4)goto _LL1DA;if(*((int*)_tmp217)!= 1)goto _LL1DA;_tmp218=((
struct Cyc_Absyn_VarType_struct*)_tmp217)->f1;_tmp219=_tmp216.f2;if(_tmp219 <= (
void*)4)goto _LL1DA;if(*((int*)_tmp219)!= 1)goto _LL1DA;_tmp21A=((struct Cyc_Absyn_VarType_struct*)
_tmp219)->f1;_LL1D9: return Cyc_Absyn_tvar_cmp(_tmp218,_tmp21A)== 0;_LL1DA:;_LL1DB:
return 0;_LL1D7:;}_LL1CF: if(*((int*)_tmp20D)!= 21)goto _LL1D1;_tmp20F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp20D)->f1;_LL1D0: for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp20F->hd))return 1;}return 0;_LL1D1: if(*((int*)_tmp20D)!= 22)
goto _LL1D3;_tmp210=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp20D)->f1;_LL1D2: {
void*_tmp21B=Cyc_Tcutil_rgns_of(_tmp210);void*_tmp21C;_LL1DD: if(_tmp21B <= (void*)
4)goto _LL1DF;if(*((int*)_tmp21B)!= 22)goto _LL1DF;_tmp21C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp21B)->f1;_LL1DE: if(!constrain)return 0;{void*_tmp21D=Cyc_Tcutil_compress(
_tmp21C);struct Cyc_Core_Opt*_tmp21E;struct Cyc_Core_Opt*_tmp21F;struct Cyc_Core_Opt**
_tmp220;struct Cyc_Core_Opt*_tmp221;_LL1E2: if(_tmp21D <= (void*)4)goto _LL1E4;if(*((
int*)_tmp21D)!= 0)goto _LL1E4;_tmp21E=((struct Cyc_Absyn_Evar_struct*)_tmp21D)->f1;
_tmp21F=((struct Cyc_Absyn_Evar_struct*)_tmp21D)->f2;_tmp220=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp21D)->f2;_tmp221=((struct Cyc_Absyn_Evar_struct*)
_tmp21D)->f4;_LL1E3: {void*_tmp222=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp221);Cyc_Tcutil_occurs(_tmp222,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp221))->v,r);{void*_tmp223=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp225=_cycalloc(sizeof(*_tmp225));
_tmp225[0]=({struct Cyc_Absyn_JoinEff_struct _tmp226;_tmp226.tag=21;_tmp226.f1=({
void*_tmp227[2];_tmp227[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp228=
_cycalloc(sizeof(*_tmp228));_tmp228[0]=({struct Cyc_Absyn_AccessEff_struct _tmp229;
_tmp229.tag=20;_tmp229.f1=(void*)r;_tmp229;});_tmp228;});_tmp227[0]=_tmp222;Cyc_List_list(
_tag_dynforward(_tmp227,sizeof(void*),2));});_tmp226;});_tmp225;}));*_tmp220=({
struct Cyc_Core_Opt*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->v=(void*)_tmp223;
_tmp224;});return 1;}}_LL1E4:;_LL1E5: return 0;_LL1E1:;}_LL1DF:;_LL1E0: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp21B);_LL1DC:;}_LL1D3: if(*((int*)_tmp20D)!= 0)goto _LL1D5;_tmp211=((
struct Cyc_Absyn_Evar_struct*)_tmp20D)->f1;_tmp212=((struct Cyc_Absyn_Evar_struct*)
_tmp20D)->f2;_tmp213=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp20D)->f2;_tmp214=((struct Cyc_Absyn_Evar_struct*)_tmp20D)->f4;_LL1D4: if(
_tmp211 == 0  || (void*)_tmp211->v != (void*)6)({void*_tmp22A[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp22B="effect evar has wrong kind";_tag_dynforward(_tmp22B,sizeof(char),
_get_zero_arr_size(_tmp22B,27));}),_tag_dynforward(_tmp22A,sizeof(void*),0));});
if(!constrain)return 0;{void*_tmp22C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp214);Cyc_Tcutil_occurs(_tmp22C,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp214))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp22D=({struct Cyc_Absyn_JoinEff_struct*_tmp22F=_cycalloc(sizeof(*_tmp22F));
_tmp22F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp230;_tmp230.tag=21;_tmp230.f1=({
struct Cyc_List_List*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->hd=(void*)
_tmp22C;_tmp231->tl=({struct Cyc_List_List*_tmp232=_cycalloc(sizeof(*_tmp232));
_tmp232->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp233=_cycalloc(
sizeof(*_tmp233));_tmp233[0]=({struct Cyc_Absyn_AccessEff_struct _tmp234;_tmp234.tag=
20;_tmp234.f1=(void*)r;_tmp234;});_tmp233;}));_tmp232->tl=0;_tmp232;});_tmp231;});
_tmp230;});_tmp22F;});*_tmp213=({struct Cyc_Core_Opt*_tmp22E=_cycalloc(sizeof(*
_tmp22E));_tmp22E->v=(void*)((void*)_tmp22D);_tmp22E;});return 1;}}_LL1D5:;_LL1D6:
return 0;_LL1CC:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp235=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp236;void*_tmp237;struct Cyc_Core_Opt*_tmp238;struct
Cyc_Core_Opt*_tmp239;struct Cyc_Core_Opt**_tmp23A;struct Cyc_Core_Opt*_tmp23B;
_LL1E7: if(_tmp235 <= (void*)4)goto _LL1EF;if(*((int*)_tmp235)!= 20)goto _LL1E9;
_LL1E8: return 0;_LL1E9: if(*((int*)_tmp235)!= 21)goto _LL1EB;_tmp236=((struct Cyc_Absyn_JoinEff_struct*)
_tmp235)->f1;_LL1EA: for(0;_tmp236 != 0;_tmp236=_tmp236->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp236->hd))return 1;}return 0;_LL1EB: if(*((int*)
_tmp235)!= 22)goto _LL1ED;_tmp237=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp235)->f1;_LL1EC: _tmp237=Cyc_Tcutil_compress(_tmp237);if(t == _tmp237)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp237);{void*_tmp23C=Cyc_Tcutil_rgns_of(
t);void*_tmp23D;_LL1F2: if(_tmp23C <= (void*)4)goto _LL1F4;if(*((int*)_tmp23C)!= 22)
goto _LL1F4;_tmp23D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp23C)->f1;_LL1F3: {
struct _tuple0 _tmp23F=({struct _tuple0 _tmp23E;_tmp23E.f1=Cyc_Tcutil_compress(
_tmp23D);_tmp23E.f2=_tmp237;_tmp23E;});void*_tmp240;struct Cyc_Absyn_Tvar*_tmp241;
void*_tmp242;struct Cyc_Absyn_Tvar*_tmp243;_LL1F7: _tmp240=_tmp23F.f1;if(_tmp240 <= (
void*)4)goto _LL1F9;if(*((int*)_tmp240)!= 1)goto _LL1F9;_tmp241=((struct Cyc_Absyn_VarType_struct*)
_tmp240)->f1;_tmp242=_tmp23F.f2;if(_tmp242 <= (void*)4)goto _LL1F9;if(*((int*)
_tmp242)!= 1)goto _LL1F9;_tmp243=((struct Cyc_Absyn_VarType_struct*)_tmp242)->f1;
_LL1F8: return Cyc_Tcutil_unify(t,_tmp237);_LL1F9:;_LL1FA: return _tmp23D == _tmp237;
_LL1F6:;}_LL1F4:;_LL1F5: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp23C);_LL1F1:;}_LL1ED: if(*((int*)_tmp235)!= 0)goto _LL1EF;_tmp238=((struct Cyc_Absyn_Evar_struct*)
_tmp235)->f1;_tmp239=((struct Cyc_Absyn_Evar_struct*)_tmp235)->f2;_tmp23A=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp235)->f2;_tmp23B=((struct Cyc_Absyn_Evar_struct*)
_tmp235)->f4;_LL1EE: if(_tmp238 == 0  || (void*)_tmp238->v != (void*)6)({void*
_tmp244[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp245="effect evar has wrong kind";_tag_dynforward(_tmp245,sizeof(
char),_get_zero_arr_size(_tmp245,27));}),_tag_dynforward(_tmp244,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{void*_tmp246=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp23B);Cyc_Tcutil_occurs(_tmp246,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp23B))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp247=({struct Cyc_Absyn_JoinEff_struct*_tmp249=_cycalloc(sizeof(*_tmp249));
_tmp249[0]=({struct Cyc_Absyn_JoinEff_struct _tmp24A;_tmp24A.tag=21;_tmp24A.f1=({
struct Cyc_List_List*_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B->hd=(void*)
_tmp246;_tmp24B->tl=({struct Cyc_List_List*_tmp24C=_cycalloc(sizeof(*_tmp24C));
_tmp24C->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp24D=_cycalloc(
sizeof(*_tmp24D));_tmp24D[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp24E;_tmp24E.tag=
22;_tmp24E.f1=(void*)t;_tmp24E;});_tmp24D;}));_tmp24C->tl=0;_tmp24C;});_tmp24B;});
_tmp24A;});_tmp249;});*_tmp23A=({struct Cyc_Core_Opt*_tmp248=_cycalloc(sizeof(*
_tmp248));_tmp248->v=(void*)((void*)_tmp247);_tmp248;});return 1;}}_LL1EF:;_LL1F0:
return 0;_LL1E6:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp24F=e;struct
Cyc_Absyn_Tvar*_tmp250;struct Cyc_List_List*_tmp251;void*_tmp252;struct Cyc_Core_Opt*
_tmp253;struct Cyc_Core_Opt*_tmp254;struct Cyc_Core_Opt**_tmp255;struct Cyc_Core_Opt*
_tmp256;_LL1FC: if(_tmp24F <= (void*)4)goto _LL204;if(*((int*)_tmp24F)!= 1)goto
_LL1FE;_tmp250=((struct Cyc_Absyn_VarType_struct*)_tmp24F)->f1;_LL1FD: return Cyc_Absyn_tvar_cmp(
v,_tmp250)== 0;_LL1FE: if(*((int*)_tmp24F)!= 21)goto _LL200;_tmp251=((struct Cyc_Absyn_JoinEff_struct*)
_tmp24F)->f1;_LL1FF: for(0;_tmp251 != 0;_tmp251=_tmp251->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp251->hd))return 1;}return 0;_LL200: if(*((int*)
_tmp24F)!= 22)goto _LL202;_tmp252=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp24F)->f1;_LL201: {void*_tmp257=Cyc_Tcutil_rgns_of(_tmp252);void*_tmp258;
_LL207: if(_tmp257 <= (void*)4)goto _LL209;if(*((int*)_tmp257)!= 22)goto _LL209;
_tmp258=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp257)->f1;_LL208: if(!
may_constrain_evars)return 0;{void*_tmp259=Cyc_Tcutil_compress(_tmp258);struct Cyc_Core_Opt*
_tmp25A;struct Cyc_Core_Opt*_tmp25B;struct Cyc_Core_Opt**_tmp25C;struct Cyc_Core_Opt*
_tmp25D;_LL20C: if(_tmp259 <= (void*)4)goto _LL20E;if(*((int*)_tmp259)!= 0)goto
_LL20E;_tmp25A=((struct Cyc_Absyn_Evar_struct*)_tmp259)->f1;_tmp25B=((struct Cyc_Absyn_Evar_struct*)
_tmp259)->f2;_tmp25C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp259)->f2;_tmp25D=((struct Cyc_Absyn_Evar_struct*)_tmp259)->f4;_LL20D: {void*
_tmp25E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp25D);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp25D))->v,v))return 0;{void*_tmp25F=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp261=_cycalloc(sizeof(*_tmp261));
_tmp261[0]=({struct Cyc_Absyn_JoinEff_struct _tmp262;_tmp262.tag=21;_tmp262.f1=({
void*_tmp263[2];_tmp263[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp264=
_cycalloc(sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Absyn_VarType_struct _tmp265;
_tmp265.tag=1;_tmp265.f1=v;_tmp265;});_tmp264;});_tmp263[0]=_tmp25E;Cyc_List_list(
_tag_dynforward(_tmp263,sizeof(void*),2));});_tmp262;});_tmp261;}));*_tmp25C=({
struct Cyc_Core_Opt*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->v=(void*)_tmp25F;
_tmp260;});return 1;}}_LL20E:;_LL20F: return 0;_LL20B:;}_LL209:;_LL20A: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp257);_LL206:;}_LL202: if(*((int*)_tmp24F)!= 0)goto
_LL204;_tmp253=((struct Cyc_Absyn_Evar_struct*)_tmp24F)->f1;_tmp254=((struct Cyc_Absyn_Evar_struct*)
_tmp24F)->f2;_tmp255=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp24F)->f2;_tmp256=((struct Cyc_Absyn_Evar_struct*)_tmp24F)->f4;_LL203: if(
_tmp253 == 0  || (void*)_tmp253->v != (void*)6)({void*_tmp266[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp267="effect evar has wrong kind";_tag_dynforward(_tmp267,sizeof(char),
_get_zero_arr_size(_tmp267,27));}),_tag_dynforward(_tmp266,sizeof(void*),0));});{
void*_tmp268=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp256);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp256))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp269=({struct Cyc_Absyn_JoinEff_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));
_tmp26B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp26C;_tmp26C.tag=21;_tmp26C.f1=({
struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->hd=(void*)
_tmp268;_tmp26D->tl=({struct Cyc_List_List*_tmp26E=_cycalloc(sizeof(*_tmp26E));
_tmp26E->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp26F=_cycalloc(
sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Absyn_VarType_struct _tmp270;_tmp270.tag=
1;_tmp270.f1=v;_tmp270;});_tmp26F;}));_tmp26E->tl=0;_tmp26E;});_tmp26D;});
_tmp26C;});_tmp26B;});*_tmp255=({struct Cyc_Core_Opt*_tmp26A=_cycalloc(sizeof(*
_tmp26A));_tmp26A->v=(void*)((void*)_tmp269);_tmp26A;});return 1;}}_LL204:;_LL205:
return 0;_LL1FB:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp271=e;struct Cyc_List_List*_tmp272;void*_tmp273;_LL211: if(_tmp271 <= (
void*)4)goto _LL217;if(*((int*)_tmp271)!= 21)goto _LL213;_tmp272=((struct Cyc_Absyn_JoinEff_struct*)
_tmp271)->f1;_LL212: for(0;_tmp272 != 0;_tmp272=_tmp272->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp272->hd))return 1;}return 0;_LL213: if(*((int*)_tmp271)!= 22)goto
_LL215;_tmp273=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp271)->f1;_LL214: {
void*_tmp274=Cyc_Tcutil_rgns_of(_tmp273);void*_tmp275;_LL21A: if(_tmp274 <= (void*)
4)goto _LL21C;if(*((int*)_tmp274)!= 22)goto _LL21C;_tmp275=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp274)->f1;_LL21B: return 0;_LL21C:;_LL21D: return Cyc_Tcutil_evar_in_effect(evar,
_tmp274);_LL219:;}_LL215: if(*((int*)_tmp271)!= 0)goto _LL217;_LL216: return evar == 
e;_LL217:;_LL218: return 0;_LL210:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp276=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp277;void*_tmp278;struct Cyc_Absyn_Tvar*_tmp279;void*_tmp27A;
struct Cyc_Core_Opt*_tmp27B;struct Cyc_Core_Opt**_tmp27C;struct Cyc_Core_Opt*
_tmp27D;_LL21F: if(_tmp276 <= (void*)4)goto _LL229;if(*((int*)_tmp276)!= 21)goto
_LL221;_tmp277=((struct Cyc_Absyn_JoinEff_struct*)_tmp276)->f1;_LL220: for(0;
_tmp277 != 0;_tmp277=_tmp277->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp277->hd,e2))return 0;}return 1;_LL221: if(*((int*)
_tmp276)!= 20)goto _LL223;_tmp278=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp276)->f1;_LL222: return Cyc_Tcutil_region_in_effect(0,_tmp278,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp278,(void*)2);_LL223: if(*((int*)
_tmp276)!= 1)goto _LL225;_tmp279=((struct Cyc_Absyn_VarType_struct*)_tmp276)->f1;
_LL224: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp279,e2);
_LL225: if(*((int*)_tmp276)!= 22)goto _LL227;_tmp27A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp276)->f1;_LL226: {void*_tmp27E=Cyc_Tcutil_rgns_of(_tmp27A);void*_tmp27F;
_LL22C: if(_tmp27E <= (void*)4)goto _LL22E;if(*((int*)_tmp27E)!= 22)goto _LL22E;
_tmp27F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp27E)->f1;_LL22D: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp27F,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp27F,Cyc_Absyn_sint_typ);_LL22E:;_LL22F: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp27E,e2);_LL22B:;}_LL227: if(*((int*)_tmp276)!= 0)goto
_LL229;_tmp27B=((struct Cyc_Absyn_Evar_struct*)_tmp276)->f2;_tmp27C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp276)->f2;_tmp27D=((struct Cyc_Absyn_Evar_struct*)
_tmp276)->f4;_LL228: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp27C=({struct Cyc_Core_Opt*
_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280->v=(void*)Cyc_Absyn_empty_effect;
_tmp280;});return 1;_LL229:;_LL22A:({struct Cyc_String_pa_struct _tmp283;_tmp283.tag=
0;_tmp283.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp281[1]={& _tmp283};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp282="subset_effect: bad effect: %s";
_tag_dynforward(_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,30));}),
_tag_dynforward(_tmp281,sizeof(void*),1));}});_LL21E:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){struct
_tuple0 _tmp285;void*_tmp286;void*_tmp287;struct _tuple0*_tmp284=(struct _tuple0*)
r1->hd;_tmp285=*_tmp284;_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;{int found=_tmp286
== (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0 _tmp289;void*_tmp28A;void*_tmp28B;struct _tuple0*_tmp288=(struct
_tuple0*)r2->hd;_tmp289=*_tmp288;_tmp28A=_tmp289.f1;_tmp28B=_tmp289.f2;if(Cyc_Tcutil_unify(
_tmp286,_tmp28A) && Cyc_Tcutil_unify(_tmp287,_tmp28B)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp28C=t1;struct Cyc_Core_Opt*_tmp28D;struct Cyc_Core_Opt*
_tmp28E;struct Cyc_Core_Opt**_tmp28F;struct Cyc_Core_Opt*_tmp290;_LL231: if(_tmp28C
<= (void*)4)goto _LL233;if(*((int*)_tmp28C)!= 0)goto _LL233;_tmp28D=((struct Cyc_Absyn_Evar_struct*)
_tmp28C)->f1;_tmp28E=((struct Cyc_Absyn_Evar_struct*)_tmp28C)->f2;_tmp28F=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp28C)->f2;_tmp290=((struct Cyc_Absyn_Evar_struct*)
_tmp28C)->f4;_LL232: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp290))->v,t2);{void*_tmp291=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp291,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp28D))->v)){*_tmp28F=({struct Cyc_Core_Opt*_tmp292=_cycalloc(sizeof(*_tmp292));
_tmp292->v=(void*)t2;_tmp292;});return;}else{{void*_tmp293=t2;struct Cyc_Core_Opt*
_tmp294;struct Cyc_Core_Opt**_tmp295;struct Cyc_Core_Opt*_tmp296;struct Cyc_Absyn_PtrInfo
_tmp297;_LL236: if(_tmp293 <= (void*)4)goto _LL23A;if(*((int*)_tmp293)!= 0)goto
_LL238;_tmp294=((struct Cyc_Absyn_Evar_struct*)_tmp293)->f2;_tmp295=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp293)->f2;_tmp296=((struct Cyc_Absyn_Evar_struct*)
_tmp293)->f4;_LL237: {struct Cyc_List_List*_tmp298=(struct Cyc_List_List*)_tmp290->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp296))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp298,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp299="(type variable would escape scope)";_tag_dynforward(_tmp299,
sizeof(char),_get_zero_arr_size(_tmp299,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp28D->v,_tmp291)){*_tmp295=({struct Cyc_Core_Opt*
_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A->v=(void*)t1;_tmp29A;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp29B="(kinds are incompatible)";_tag_dynforward(_tmp29B,sizeof(char),
_get_zero_arr_size(_tmp29B,25));});goto _LL235;}_LL238: if(*((int*)_tmp293)!= 4)
goto _LL23A;_tmp297=((struct Cyc_Absyn_PointerType_struct*)_tmp293)->f1;if(!((void*)
_tmp28D->v == (void*)2))goto _LL23A;_LL239: {struct Cyc_Absyn_Conref*_tmp29C=Cyc_Absyn_compress_conref((
_tmp297.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union _tmp29D=_tmp29C->v;
_LL23D: if((_tmp29D.No_constr).tag != 2)goto _LL23F;_LL23E: _tmp29C->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp29E;(_tmp29E.Eq_constr).tag=0;(_tmp29E.Eq_constr).f1=(
void*)Cyc_Absyn_bounds_one;_tmp29E;});*_tmp28F=({struct Cyc_Core_Opt*_tmp29F=
_cycalloc(sizeof(*_tmp29F));_tmp29F->v=(void*)t2;_tmp29F;});return;_LL23F:;
_LL240: goto _LL23C;_LL23C:;}goto _LL235;}_LL23A:;_LL23B: goto _LL235;_LL235:;}Cyc_Tcutil_failure_reason=({
const char*_tmp2A0="(kinds are incompatible)";_tag_dynforward(_tmp2A0,sizeof(char),
_get_zero_arr_size(_tmp2A0,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}}_LL233:;
_LL234: goto _LL230;_LL230:;}{struct _tuple0 _tmp2A2=({struct _tuple0 _tmp2A1;_tmp2A1.f1=
t2;_tmp2A1.f2=t1;_tmp2A1;});void*_tmp2A3;void*_tmp2A4;void*_tmp2A5;void*_tmp2A6;
struct Cyc_Absyn_Tvar*_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_Tvar*_tmp2A9;void*
_tmp2AA;struct Cyc_Absyn_AggrInfo _tmp2AB;union Cyc_Absyn_AggrInfoU_union _tmp2AC;
struct Cyc_List_List*_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_AggrInfo _tmp2AF;union
Cyc_Absyn_AggrInfoU_union _tmp2B0;struct Cyc_List_List*_tmp2B1;void*_tmp2B2;struct
_tuple2*_tmp2B3;void*_tmp2B4;struct _tuple2*_tmp2B5;void*_tmp2B6;struct Cyc_List_List*
_tmp2B7;void*_tmp2B8;struct Cyc_List_List*_tmp2B9;void*_tmp2BA;struct Cyc_Absyn_TunionInfo
_tmp2BB;union Cyc_Absyn_TunionInfoU_union _tmp2BC;struct Cyc_Absyn_Tuniondecl**
_tmp2BD;struct Cyc_Absyn_Tuniondecl*_tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_Core_Opt*
_tmp2C0;void*_tmp2C1;struct Cyc_Absyn_TunionInfo _tmp2C2;union Cyc_Absyn_TunionInfoU_union
_tmp2C3;struct Cyc_Absyn_Tuniondecl**_tmp2C4;struct Cyc_Absyn_Tuniondecl*_tmp2C5;
struct Cyc_List_List*_tmp2C6;struct Cyc_Core_Opt*_tmp2C7;void*_tmp2C8;struct Cyc_Absyn_TunionFieldInfo
_tmp2C9;union Cyc_Absyn_TunionFieldInfoU_union _tmp2CA;struct Cyc_Absyn_Tuniondecl*
_tmp2CB;struct Cyc_Absyn_Tunionfield*_tmp2CC;struct Cyc_List_List*_tmp2CD;void*
_tmp2CE;struct Cyc_Absyn_TunionFieldInfo _tmp2CF;union Cyc_Absyn_TunionFieldInfoU_union
_tmp2D0;struct Cyc_Absyn_Tuniondecl*_tmp2D1;struct Cyc_Absyn_Tunionfield*_tmp2D2;
struct Cyc_List_List*_tmp2D3;void*_tmp2D4;struct Cyc_Absyn_PtrInfo _tmp2D5;void*
_tmp2D6;struct Cyc_Absyn_Tqual _tmp2D7;struct Cyc_Absyn_PtrAtts _tmp2D8;void*_tmp2D9;
struct Cyc_Absyn_Conref*_tmp2DA;struct Cyc_Absyn_Conref*_tmp2DB;struct Cyc_Absyn_Conref*
_tmp2DC;void*_tmp2DD;struct Cyc_Absyn_PtrInfo _tmp2DE;void*_tmp2DF;struct Cyc_Absyn_Tqual
_tmp2E0;struct Cyc_Absyn_PtrAtts _tmp2E1;void*_tmp2E2;struct Cyc_Absyn_Conref*
_tmp2E3;struct Cyc_Absyn_Conref*_tmp2E4;struct Cyc_Absyn_Conref*_tmp2E5;void*
_tmp2E6;void*_tmp2E7;void*_tmp2E8;void*_tmp2E9;void*_tmp2EA;void*_tmp2EB;void*
_tmp2EC;void*_tmp2ED;void*_tmp2EE;int _tmp2EF;void*_tmp2F0;int _tmp2F1;void*
_tmp2F2;void*_tmp2F3;void*_tmp2F4;void*_tmp2F5;void*_tmp2F6;int _tmp2F7;void*
_tmp2F8;int _tmp2F9;void*_tmp2FA;void*_tmp2FB;void*_tmp2FC;void*_tmp2FD;void*
_tmp2FE;struct Cyc_Absyn_ArrayInfo _tmp2FF;void*_tmp300;struct Cyc_Absyn_Tqual
_tmp301;struct Cyc_Absyn_Exp*_tmp302;struct Cyc_Absyn_Conref*_tmp303;void*_tmp304;
struct Cyc_Absyn_ArrayInfo _tmp305;void*_tmp306;struct Cyc_Absyn_Tqual _tmp307;
struct Cyc_Absyn_Exp*_tmp308;struct Cyc_Absyn_Conref*_tmp309;void*_tmp30A;struct
Cyc_Absyn_FnInfo _tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Core_Opt*_tmp30D;
void*_tmp30E;struct Cyc_List_List*_tmp30F;int _tmp310;struct Cyc_Absyn_VarargInfo*
_tmp311;struct Cyc_List_List*_tmp312;struct Cyc_List_List*_tmp313;void*_tmp314;
struct Cyc_Absyn_FnInfo _tmp315;struct Cyc_List_List*_tmp316;struct Cyc_Core_Opt*
_tmp317;void*_tmp318;struct Cyc_List_List*_tmp319;int _tmp31A;struct Cyc_Absyn_VarargInfo*
_tmp31B;struct Cyc_List_List*_tmp31C;struct Cyc_List_List*_tmp31D;void*_tmp31E;
struct Cyc_List_List*_tmp31F;void*_tmp320;struct Cyc_List_List*_tmp321;void*
_tmp322;void*_tmp323;struct Cyc_List_List*_tmp324;void*_tmp325;void*_tmp326;
struct Cyc_List_List*_tmp327;void*_tmp328;void*_tmp329;void*_tmp32A;void*_tmp32B;
void*_tmp32C;void*_tmp32D;void*_tmp32E;void*_tmp32F;void*_tmp330;void*_tmp331;
void*_tmp332;void*_tmp333;void*_tmp334;void*_tmp335;void*_tmp336;void*_tmp337;
void*_tmp338;void*_tmp339;void*_tmp33A;void*_tmp33B;_LL242: _tmp2A3=_tmp2A2.f1;
if(_tmp2A3 <= (void*)4)goto _LL244;if(*((int*)_tmp2A3)!= 0)goto _LL244;_LL243: Cyc_Tcutil_unify_it(
t2,t1);return;_LL244: _tmp2A4=_tmp2A2.f1;if((int)_tmp2A4 != 0)goto _LL246;_tmp2A5=
_tmp2A2.f2;if((int)_tmp2A5 != 0)goto _LL246;_LL245: return;_LL246: _tmp2A6=_tmp2A2.f1;
if(_tmp2A6 <= (void*)4)goto _LL248;if(*((int*)_tmp2A6)!= 1)goto _LL248;_tmp2A7=((
struct Cyc_Absyn_VarType_struct*)_tmp2A6)->f1;_tmp2A8=_tmp2A2.f2;if(_tmp2A8 <= (
void*)4)goto _LL248;if(*((int*)_tmp2A8)!= 1)goto _LL248;_tmp2A9=((struct Cyc_Absyn_VarType_struct*)
_tmp2A8)->f1;_LL247: {struct _dynforward_ptr*_tmp33C=_tmp2A7->name;struct
_dynforward_ptr*_tmp33D=_tmp2A9->name;int _tmp33E=*((int*)_check_null(_tmp2A7->identity));
int _tmp33F=*((int*)_check_null(_tmp2A9->identity));void*_tmp340=Cyc_Tcutil_tvar_kind(
_tmp2A7);void*_tmp341=Cyc_Tcutil_tvar_kind(_tmp2A9);if(_tmp33F == _tmp33E  && Cyc_strptrcmp(
_tmp33C,_tmp33D)== 0){if(_tmp340 != _tmp341)({struct Cyc_String_pa_struct _tmp346;
_tmp346.tag=0;_tmp346.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp341));{struct Cyc_String_pa_struct _tmp345;_tmp345.tag=0;_tmp345.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp340));{
struct Cyc_String_pa_struct _tmp344;_tmp344.tag=0;_tmp344.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp33C);{void*_tmp342[3]={& _tmp344,&
_tmp345,& _tmp346};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp343="same type variable %s has kinds %s and %s";_tag_dynforward(
_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,42));}),_tag_dynforward(_tmp342,
sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*_tmp347="(variable types are not the same)";
_tag_dynforward(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,34));});goto
_LL241;}_LL248: _tmp2AA=_tmp2A2.f1;if(_tmp2AA <= (void*)4)goto _LL24A;if(*((int*)
_tmp2AA)!= 10)goto _LL24A;_tmp2AB=((struct Cyc_Absyn_AggrType_struct*)_tmp2AA)->f1;
_tmp2AC=_tmp2AB.aggr_info;_tmp2AD=_tmp2AB.targs;_tmp2AE=_tmp2A2.f2;if(_tmp2AE <= (
void*)4)goto _LL24A;if(*((int*)_tmp2AE)!= 10)goto _LL24A;_tmp2AF=((struct Cyc_Absyn_AggrType_struct*)
_tmp2AE)->f1;_tmp2B0=_tmp2AF.aggr_info;_tmp2B1=_tmp2AF.targs;_LL249: {void*
_tmp349;struct _tuple2*_tmp34A;struct _tuple6 _tmp348=Cyc_Absyn_aggr_kinded_name(
_tmp2B0);_tmp349=_tmp348.f1;_tmp34A=_tmp348.f2;{void*_tmp34C;struct _tuple2*
_tmp34D;struct _tuple6 _tmp34B=Cyc_Absyn_aggr_kinded_name(_tmp2AC);_tmp34C=_tmp34B.f1;
_tmp34D=_tmp34B.f2;if(_tmp349 != _tmp34C){Cyc_Tcutil_failure_reason=({const char*
_tmp34E="(struct and union type)";_tag_dynforward(_tmp34E,sizeof(char),
_get_zero_arr_size(_tmp34E,24));});goto _LL241;}if(Cyc_Absyn_qvar_cmp(_tmp34A,
_tmp34D)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp34F="(different type name)";
_tag_dynforward(_tmp34F,sizeof(char),_get_zero_arr_size(_tmp34F,22));});goto
_LL241;}Cyc_Tcutil_unify_list(_tmp2B1,_tmp2AD);return;}}_LL24A: _tmp2B2=_tmp2A2.f1;
if(_tmp2B2 <= (void*)4)goto _LL24C;if(*((int*)_tmp2B2)!= 12)goto _LL24C;_tmp2B3=((
struct Cyc_Absyn_EnumType_struct*)_tmp2B2)->f1;_tmp2B4=_tmp2A2.f2;if(_tmp2B4 <= (
void*)4)goto _LL24C;if(*((int*)_tmp2B4)!= 12)goto _LL24C;_tmp2B5=((struct Cyc_Absyn_EnumType_struct*)
_tmp2B4)->f1;_LL24B: if(Cyc_Absyn_qvar_cmp(_tmp2B3,_tmp2B5)== 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp350="(different enum types)";_tag_dynforward(_tmp350,sizeof(char),
_get_zero_arr_size(_tmp350,23));});goto _LL241;_LL24C: _tmp2B6=_tmp2A2.f1;if(
_tmp2B6 <= (void*)4)goto _LL24E;if(*((int*)_tmp2B6)!= 13)goto _LL24E;_tmp2B7=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp2B6)->f1;_tmp2B8=_tmp2A2.f2;if(_tmp2B8
<= (void*)4)goto _LL24E;if(*((int*)_tmp2B8)!= 13)goto _LL24E;_tmp2B9=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2B8)->f1;_LL24D: {int bad=0;for(0;_tmp2B7 != 0  && _tmp2B9 != 0;(_tmp2B7=_tmp2B7->tl,
_tmp2B9=_tmp2B9->tl)){struct Cyc_Absyn_Enumfield*_tmp351=(struct Cyc_Absyn_Enumfield*)
_tmp2B7->hd;struct Cyc_Absyn_Enumfield*_tmp352=(struct Cyc_Absyn_Enumfield*)
_tmp2B9->hd;if(Cyc_Absyn_qvar_cmp(_tmp351->name,_tmp352->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp353="(different names for enum fields)";_tag_dynforward(_tmp353,
sizeof(char),_get_zero_arr_size(_tmp353,34));});bad=1;break;}if(_tmp351->tag == 
_tmp352->tag)continue;if(_tmp351->tag == 0  || _tmp352->tag == 0){Cyc_Tcutil_failure_reason=({
const char*_tmp354="(different tag values for enum fields)";_tag_dynforward(
_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,39));});bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp351->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp352->tag))){Cyc_Tcutil_failure_reason=({const char*_tmp355="(different tag values for enum fields)";
_tag_dynforward(_tmp355,sizeof(char),_get_zero_arr_size(_tmp355,39));});bad=1;
break;}}if(bad)goto _LL241;if(_tmp2B7 == 0  && _tmp2B9 == 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp356="(different number of fields for enums)";_tag_dynforward(
_tmp356,sizeof(char),_get_zero_arr_size(_tmp356,39));});goto _LL241;}_LL24E:
_tmp2BA=_tmp2A2.f1;if(_tmp2BA <= (void*)4)goto _LL250;if(*((int*)_tmp2BA)!= 2)goto
_LL250;_tmp2BB=((struct Cyc_Absyn_TunionType_struct*)_tmp2BA)->f1;_tmp2BC=_tmp2BB.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp2BA)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL250;_tmp2BD=(_tmp2BC.KnownTunion).f1;_tmp2BE=*_tmp2BD;_tmp2BF=
_tmp2BB.targs;_tmp2C0=_tmp2BB.rgn;_tmp2C1=_tmp2A2.f2;if(_tmp2C1 <= (void*)4)goto
_LL250;if(*((int*)_tmp2C1)!= 2)goto _LL250;_tmp2C2=((struct Cyc_Absyn_TunionType_struct*)
_tmp2C1)->f1;_tmp2C3=_tmp2C2.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp2C1)->f1).tunion_info).KnownTunion).tag != 1)goto _LL250;_tmp2C4=(_tmp2C3.KnownTunion).f1;
_tmp2C5=*_tmp2C4;_tmp2C6=_tmp2C2.targs;_tmp2C7=_tmp2C2.rgn;_LL24F: if(_tmp2BE == 
_tmp2C5  || Cyc_Absyn_qvar_cmp(_tmp2BE->name,_tmp2C5->name)== 0){if(_tmp2C7 != 0
 && _tmp2C0 != 0)Cyc_Tcutil_unify_it((void*)_tmp2C7->v,(void*)_tmp2C0->v);else{
if(_tmp2C7 != 0  || _tmp2C0 != 0)goto Tunion_fail;}Cyc_Tcutil_unify_list(_tmp2C6,
_tmp2BF);return;}Tunion_fail: Cyc_Tcutil_failure_reason=({const char*_tmp357="(different tunion types)";
_tag_dynforward(_tmp357,sizeof(char),_get_zero_arr_size(_tmp357,25));});goto
_LL241;_LL250: _tmp2C8=_tmp2A2.f1;if(_tmp2C8 <= (void*)4)goto _LL252;if(*((int*)
_tmp2C8)!= 3)goto _LL252;_tmp2C9=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2C8)->f1;_tmp2CA=_tmp2C9.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2C8)->f1).field_info).KnownTunionfield).tag != 1)goto _LL252;_tmp2CB=(_tmp2CA.KnownTunionfield).f1;
_tmp2CC=(_tmp2CA.KnownTunionfield).f2;_tmp2CD=_tmp2C9.targs;_tmp2CE=_tmp2A2.f2;
if(_tmp2CE <= (void*)4)goto _LL252;if(*((int*)_tmp2CE)!= 3)goto _LL252;_tmp2CF=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2CE)->f1;_tmp2D0=_tmp2CF.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2CE)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL252;_tmp2D1=(_tmp2D0.KnownTunionfield).f1;_tmp2D2=(_tmp2D0.KnownTunionfield).f2;
_tmp2D3=_tmp2CF.targs;_LL251: if((_tmp2CB == _tmp2D1  || Cyc_Absyn_qvar_cmp(_tmp2CB->name,
_tmp2D1->name)== 0) && (_tmp2CC == _tmp2D2  || Cyc_Absyn_qvar_cmp(_tmp2CC->name,
_tmp2D2->name)== 0)){Cyc_Tcutil_unify_list(_tmp2D3,_tmp2CD);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp358="(different tunion field types)";_tag_dynforward(_tmp358,
sizeof(char),_get_zero_arr_size(_tmp358,31));});goto _LL241;_LL252: _tmp2D4=
_tmp2A2.f1;if(_tmp2D4 <= (void*)4)goto _LL254;if(*((int*)_tmp2D4)!= 4)goto _LL254;
_tmp2D5=((struct Cyc_Absyn_PointerType_struct*)_tmp2D4)->f1;_tmp2D6=(void*)
_tmp2D5.elt_typ;_tmp2D7=_tmp2D5.elt_tq;_tmp2D8=_tmp2D5.ptr_atts;_tmp2D9=(void*)
_tmp2D8.rgn;_tmp2DA=_tmp2D8.nullable;_tmp2DB=_tmp2D8.bounds;_tmp2DC=_tmp2D8.zero_term;
_tmp2DD=_tmp2A2.f2;if(_tmp2DD <= (void*)4)goto _LL254;if(*((int*)_tmp2DD)!= 4)goto
_LL254;_tmp2DE=((struct Cyc_Absyn_PointerType_struct*)_tmp2DD)->f1;_tmp2DF=(void*)
_tmp2DE.elt_typ;_tmp2E0=_tmp2DE.elt_tq;_tmp2E1=_tmp2DE.ptr_atts;_tmp2E2=(void*)
_tmp2E1.rgn;_tmp2E3=_tmp2E1.nullable;_tmp2E4=_tmp2E1.bounds;_tmp2E5=_tmp2E1.zero_term;
_LL253: Cyc_Tcutil_unify_it(_tmp2DF,_tmp2D6);Cyc_Tcutil_unify_it(_tmp2D9,_tmp2E2);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2E5,_tmp2DC,({const char*_tmp359="(not both zero terminated)";
_tag_dynforward(_tmp359,sizeof(char),_get_zero_arr_size(_tmp359,27));}));Cyc_Tcutil_unify_tqual(
_tmp2E0,_tmp2DF,_tmp2D7,_tmp2D6);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2E4,_tmp2DB,({const char*_tmp35A="(different pointer bounds)";_tag_dynforward(
_tmp35A,sizeof(char),_get_zero_arr_size(_tmp35A,27));}));{union Cyc_Absyn_Constraint_union
_tmp35B=(Cyc_Absyn_compress_conref(_tmp2E4))->v;void*_tmp35C;void*_tmp35D;_LL27F:
if((_tmp35B.Eq_constr).tag != 0)goto _LL281;_tmp35C=(_tmp35B.Eq_constr).f1;if((int)
_tmp35C != 0)goto _LL281;_LL280: return;_LL281: if((_tmp35B.Eq_constr).tag != 0)goto
_LL283;_tmp35D=(_tmp35B.Eq_constr).f1;if((int)_tmp35D != 1)goto _LL283;_LL282:
return;_LL283:;_LL284: goto _LL27E;_LL27E:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2E3,_tmp2DA,({const char*_tmp35E="(incompatible pointer types)";
_tag_dynforward(_tmp35E,sizeof(char),_get_zero_arr_size(_tmp35E,29));}));return;
_LL254: _tmp2E6=_tmp2A2.f1;if(_tmp2E6 <= (void*)4)goto _LL256;if(*((int*)_tmp2E6)!= 
5)goto _LL256;_tmp2E7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E6)->f1;
_tmp2E8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E6)->f2;_tmp2E9=_tmp2A2.f2;
if(_tmp2E9 <= (void*)4)goto _LL256;if(*((int*)_tmp2E9)!= 5)goto _LL256;_tmp2EA=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2E9)->f1;_tmp2EB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2E9)->f2;_LL255: if(_tmp2EA == _tmp2E7  && ((_tmp2EB == _tmp2E8  || _tmp2EB == (
void*)2  && _tmp2E8 == (void*)3) || _tmp2EB == (void*)3  && _tmp2E8 == (void*)2))
return;Cyc_Tcutil_failure_reason=({const char*_tmp35F="(different integral types)";
_tag_dynforward(_tmp35F,sizeof(char),_get_zero_arr_size(_tmp35F,27));});goto
_LL241;_LL256: _tmp2EC=_tmp2A2.f1;if((int)_tmp2EC != 1)goto _LL258;_tmp2ED=_tmp2A2.f2;
if((int)_tmp2ED != 1)goto _LL258;_LL257: return;_LL258: _tmp2EE=_tmp2A2.f1;if(
_tmp2EE <= (void*)4)goto _LL25A;if(*((int*)_tmp2EE)!= 6)goto _LL25A;_tmp2EF=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2EE)->f1;_tmp2F0=_tmp2A2.f2;if(_tmp2F0 <= (
void*)4)goto _LL25A;if(*((int*)_tmp2F0)!= 6)goto _LL25A;_tmp2F1=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2F0)->f1;_LL259: if(_tmp2EF == _tmp2F1)return;goto _LL241;_LL25A: _tmp2F2=
_tmp2A2.f1;if(_tmp2F2 <= (void*)4)goto _LL25C;if(*((int*)_tmp2F2)!= 14)goto _LL25C;
_tmp2F3=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2F2)->f1;_tmp2F4=_tmp2A2.f2;
if(_tmp2F4 <= (void*)4)goto _LL25C;if(*((int*)_tmp2F4)!= 14)goto _LL25C;_tmp2F5=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2F4)->f1;_LL25B: Cyc_Tcutil_unify_it(
_tmp2F3,_tmp2F5);return;_LL25C: _tmp2F6=_tmp2A2.f1;if(_tmp2F6 <= (void*)4)goto
_LL25E;if(*((int*)_tmp2F6)!= 19)goto _LL25E;_tmp2F7=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2F6)->f1;_tmp2F8=_tmp2A2.f2;if(_tmp2F8 <= (void*)4)goto _LL25E;if(*((int*)
_tmp2F8)!= 19)goto _LL25E;_tmp2F9=((struct Cyc_Absyn_TypeInt_struct*)_tmp2F8)->f1;
_LL25D: if(_tmp2F7 == _tmp2F9)return;Cyc_Tcutil_failure_reason=({const char*_tmp360="(different type integers)";
_tag_dynforward(_tmp360,sizeof(char),_get_zero_arr_size(_tmp360,26));});goto
_LL241;_LL25E: _tmp2FA=_tmp2A2.f1;if(_tmp2FA <= (void*)4)goto _LL260;if(*((int*)
_tmp2FA)!= 18)goto _LL260;_tmp2FB=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2FA)->f1;_tmp2FC=_tmp2A2.f2;if(_tmp2FC <= (void*)4)goto _LL260;if(*((int*)
_tmp2FC)!= 18)goto _LL260;_tmp2FD=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2FC)->f1;_LL25F: Cyc_Tcutil_unify_it(_tmp2FB,_tmp2FD);return;_LL260: _tmp2FE=
_tmp2A2.f1;if(_tmp2FE <= (void*)4)goto _LL262;if(*((int*)_tmp2FE)!= 7)goto _LL262;
_tmp2FF=((struct Cyc_Absyn_ArrayType_struct*)_tmp2FE)->f1;_tmp300=(void*)_tmp2FF.elt_type;
_tmp301=_tmp2FF.tq;_tmp302=_tmp2FF.num_elts;_tmp303=_tmp2FF.zero_term;_tmp304=
_tmp2A2.f2;if(_tmp304 <= (void*)4)goto _LL262;if(*((int*)_tmp304)!= 7)goto _LL262;
_tmp305=((struct Cyc_Absyn_ArrayType_struct*)_tmp304)->f1;_tmp306=(void*)_tmp305.elt_type;
_tmp307=_tmp305.tq;_tmp308=_tmp305.num_elts;_tmp309=_tmp305.zero_term;_LL261: Cyc_Tcutil_unify_it(
_tmp306,_tmp300);Cyc_Tcutil_unify_tqual(_tmp307,_tmp306,_tmp301,_tmp300);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp303,_tmp309,({const char*_tmp361="(not both zero terminated)";
_tag_dynforward(_tmp361,sizeof(char),_get_zero_arr_size(_tmp361,27));}));if(
_tmp302 == _tmp308)return;if(_tmp302 == 0  || _tmp308 == 0)goto _LL241;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp302,(struct Cyc_Absyn_Exp*)_tmp308))return;Cyc_Tcutil_failure_reason=({
const char*_tmp362="(different array sizes)";_tag_dynforward(_tmp362,sizeof(char),
_get_zero_arr_size(_tmp362,24));});goto _LL241;_LL262: _tmp30A=_tmp2A2.f1;if(
_tmp30A <= (void*)4)goto _LL264;if(*((int*)_tmp30A)!= 8)goto _LL264;_tmp30B=((
struct Cyc_Absyn_FnType_struct*)_tmp30A)->f1;_tmp30C=_tmp30B.tvars;_tmp30D=
_tmp30B.effect;_tmp30E=(void*)_tmp30B.ret_typ;_tmp30F=_tmp30B.args;_tmp310=
_tmp30B.c_varargs;_tmp311=_tmp30B.cyc_varargs;_tmp312=_tmp30B.rgn_po;_tmp313=
_tmp30B.attributes;_tmp314=_tmp2A2.f2;if(_tmp314 <= (void*)4)goto _LL264;if(*((int*)
_tmp314)!= 8)goto _LL264;_tmp315=((struct Cyc_Absyn_FnType_struct*)_tmp314)->f1;
_tmp316=_tmp315.tvars;_tmp317=_tmp315.effect;_tmp318=(void*)_tmp315.ret_typ;
_tmp319=_tmp315.args;_tmp31A=_tmp315.c_varargs;_tmp31B=_tmp315.cyc_varargs;
_tmp31C=_tmp315.rgn_po;_tmp31D=_tmp315.attributes;_LL263: {int done=0;{struct
_RegionHandle _tmp363=_new_region("rgn");struct _RegionHandle*rgn=& _tmp363;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp316 != 0){if(_tmp30C == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp364="(second function type has too few type variables)";
_tag_dynforward(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,50));});(int)
_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp365=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp316->hd);void*_tmp366=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp30C->hd);
if(_tmp365 != _tmp366){Cyc_Tcutil_failure_reason=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp36B;_tmp36B.tag=0;_tmp36B.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp366));{struct Cyc_String_pa_struct
_tmp36A;_tmp36A.tag=0;_tmp36A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(_tmp365));{struct Cyc_String_pa_struct _tmp369;_tmp369.tag=
0;_tmp369.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp316->hd));{void*_tmp367[3]={& _tmp369,& _tmp36A,& _tmp36B};
Cyc_aprintf(({const char*_tmp368="(type var %s has different kinds %s and %s)";
_tag_dynforward(_tmp368,sizeof(char),_get_zero_arr_size(_tmp368,44));}),
_tag_dynforward(_tmp367,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp36C=_region_malloc(rgn,sizeof(*_tmp36C));_tmp36C->hd=({
struct _tuple8*_tmp36D=_region_malloc(rgn,sizeof(*_tmp36D));_tmp36D->f1=(struct
Cyc_Absyn_Tvar*)_tmp30C->hd;_tmp36D->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E[0]=({struct Cyc_Absyn_VarType_struct
_tmp36F;_tmp36F.tag=1;_tmp36F.f1=(struct Cyc_Absyn_Tvar*)_tmp316->hd;_tmp36F;});
_tmp36E;});_tmp36D;});_tmp36C->tl=inst;_tmp36C;});_tmp316=_tmp316->tl;_tmp30C=
_tmp30C->tl;}}if(_tmp30C != 0){Cyc_Tcutil_failure_reason=({const char*_tmp370="(second function type has too many type variables)";
_tag_dynforward(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,51));});
_npop_handler(0);goto _LL241;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371[0]=({struct Cyc_Absyn_FnType_struct
_tmp372;_tmp372.tag=8;_tmp372.f1=({struct Cyc_Absyn_FnInfo _tmp373;_tmp373.tvars=0;
_tmp373.effect=_tmp317;_tmp373.ret_typ=(void*)_tmp318;_tmp373.args=_tmp319;
_tmp373.c_varargs=_tmp31A;_tmp373.cyc_varargs=_tmp31B;_tmp373.rgn_po=_tmp31C;
_tmp373.attributes=_tmp31D;_tmp373;});_tmp372;});_tmp371;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp374=_cycalloc(sizeof(*
_tmp374));_tmp374[0]=({struct Cyc_Absyn_FnType_struct _tmp375;_tmp375.tag=8;
_tmp375.f1=({struct Cyc_Absyn_FnInfo _tmp376;_tmp376.tvars=0;_tmp376.effect=
_tmp30D;_tmp376.ret_typ=(void*)_tmp30E;_tmp376.args=_tmp30F;_tmp376.c_varargs=
_tmp310;_tmp376.cyc_varargs=_tmp311;_tmp376.rgn_po=_tmp312;_tmp376.attributes=
_tmp313;_tmp376;});_tmp375;});_tmp374;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp318,_tmp30E);for(0;_tmp319 != 0  && _tmp30F != 0;(
_tmp319=_tmp319->tl,_tmp30F=_tmp30F->tl)){struct Cyc_Absyn_Tqual _tmp378;void*
_tmp379;struct _tuple3 _tmp377=*((struct _tuple3*)_tmp319->hd);_tmp378=_tmp377.f2;
_tmp379=_tmp377.f3;{struct Cyc_Absyn_Tqual _tmp37B;void*_tmp37C;struct _tuple3
_tmp37A=*((struct _tuple3*)_tmp30F->hd);_tmp37B=_tmp37A.f2;_tmp37C=_tmp37A.f3;Cyc_Tcutil_unify_it(
_tmp379,_tmp37C);Cyc_Tcutil_unify_tqual(_tmp378,_tmp379,_tmp37B,_tmp37C);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp319 != 0  || _tmp30F != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp37D="(function types have different number of arguments)";
_tag_dynforward(_tmp37D,sizeof(char),_get_zero_arr_size(_tmp37D,52));});goto
_LL241;}if(_tmp31A != _tmp310){Cyc_Tcutil_failure_reason=({const char*_tmp37E="(only one function type takes C varargs)";
_tag_dynforward(_tmp37E,sizeof(char),_get_zero_arr_size(_tmp37E,41));});goto
_LL241;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp380=({struct _tuple11 _tmp37F;
_tmp37F.f1=_tmp31B;_tmp37F.f2=_tmp311;_tmp37F;});struct Cyc_Absyn_VarargInfo*
_tmp381;struct Cyc_Absyn_VarargInfo*_tmp382;struct Cyc_Absyn_VarargInfo*_tmp383;
struct Cyc_Absyn_VarargInfo*_tmp384;struct Cyc_Absyn_VarargInfo*_tmp385;struct Cyc_Absyn_VarargInfo
_tmp386;struct Cyc_Core_Opt*_tmp387;struct Cyc_Absyn_Tqual _tmp388;void*_tmp389;int
_tmp38A;struct Cyc_Absyn_VarargInfo*_tmp38B;struct Cyc_Absyn_VarargInfo _tmp38C;
struct Cyc_Core_Opt*_tmp38D;struct Cyc_Absyn_Tqual _tmp38E;void*_tmp38F;int _tmp390;
_LL286: _tmp381=_tmp380.f1;if(_tmp381 != 0)goto _LL288;_tmp382=_tmp380.f2;if(
_tmp382 != 0)goto _LL288;_LL287: goto _LL285;_LL288: _tmp383=_tmp380.f1;if(_tmp383 != 
0)goto _LL28A;_LL289: goto _LL28B;_LL28A: _tmp384=_tmp380.f2;if(_tmp384 != 0)goto
_LL28C;_LL28B: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp391="(only one function type takes varargs)";
_tag_dynforward(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,39));});goto
_LL285;_LL28C: _tmp385=_tmp380.f1;if(_tmp385 == 0)goto _LL285;_tmp386=*_tmp385;
_tmp387=_tmp386.name;_tmp388=_tmp386.tq;_tmp389=(void*)_tmp386.type;_tmp38A=
_tmp386.inject;_tmp38B=_tmp380.f2;if(_tmp38B == 0)goto _LL285;_tmp38C=*_tmp38B;
_tmp38D=_tmp38C.name;_tmp38E=_tmp38C.tq;_tmp38F=(void*)_tmp38C.type;_tmp390=
_tmp38C.inject;_LL28D: Cyc_Tcutil_unify_it(_tmp389,_tmp38F);Cyc_Tcutil_unify_tqual(
_tmp388,_tmp389,_tmp38E,_tmp38F);if(_tmp38A != _tmp390){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({
const char*_tmp392="(only one function type injects varargs)";_tag_dynforward(
_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,41));});}goto _LL285;_LL285:;}if(
bad_cyc_vararg)goto _LL241;{int bad_effect=0;{struct _tuple12 _tmp394=({struct
_tuple12 _tmp393;_tmp393.f1=_tmp317;_tmp393.f2=_tmp30D;_tmp393;});struct Cyc_Core_Opt*
_tmp395;struct Cyc_Core_Opt*_tmp396;struct Cyc_Core_Opt*_tmp397;struct Cyc_Core_Opt*
_tmp398;_LL28F: _tmp395=_tmp394.f1;if(_tmp395 != 0)goto _LL291;_tmp396=_tmp394.f2;
if(_tmp396 != 0)goto _LL291;_LL290: goto _LL28E;_LL291: _tmp397=_tmp394.f1;if(_tmp397
!= 0)goto _LL293;_LL292: goto _LL294;_LL293: _tmp398=_tmp394.f2;if(_tmp398 != 0)goto
_LL295;_LL294: bad_effect=1;goto _LL28E;_LL295:;_LL296: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp317))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp30D))->v);goto _LL28E;_LL28E:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp399="(function type effects do not unify)";
_tag_dynforward(_tmp399,sizeof(char),_get_zero_arr_size(_tmp399,37));});goto
_LL241;}if(!Cyc_Tcutil_same_atts(_tmp313,_tmp31D)){Cyc_Tcutil_failure_reason=({
const char*_tmp39A="(function types have different attributes)";_tag_dynforward(
_tmp39A,sizeof(char),_get_zero_arr_size(_tmp39A,43));});goto _LL241;}if(!Cyc_Tcutil_same_rgn_po(
_tmp312,_tmp31C)){Cyc_Tcutil_failure_reason=({const char*_tmp39B="(function types have different region lifetime orderings)";
_tag_dynforward(_tmp39B,sizeof(char),_get_zero_arr_size(_tmp39B,58));});goto
_LL241;}return;}}}_LL264: _tmp31E=_tmp2A2.f1;if(_tmp31E <= (void*)4)goto _LL266;if(*((
int*)_tmp31E)!= 9)goto _LL266;_tmp31F=((struct Cyc_Absyn_TupleType_struct*)_tmp31E)->f1;
_tmp320=_tmp2A2.f2;if(_tmp320 <= (void*)4)goto _LL266;if(*((int*)_tmp320)!= 9)goto
_LL266;_tmp321=((struct Cyc_Absyn_TupleType_struct*)_tmp320)->f1;_LL265: for(0;
_tmp321 != 0  && _tmp31F != 0;(_tmp321=_tmp321->tl,_tmp31F=_tmp31F->tl)){struct Cyc_Absyn_Tqual
_tmp39D;void*_tmp39E;struct _tuple5 _tmp39C=*((struct _tuple5*)_tmp321->hd);_tmp39D=
_tmp39C.f1;_tmp39E=_tmp39C.f2;{struct Cyc_Absyn_Tqual _tmp3A0;void*_tmp3A1;struct
_tuple5 _tmp39F=*((struct _tuple5*)_tmp31F->hd);_tmp3A0=_tmp39F.f1;_tmp3A1=_tmp39F.f2;
Cyc_Tcutil_unify_it(_tmp39E,_tmp3A1);Cyc_Tcutil_unify_tqual(_tmp39D,_tmp39E,
_tmp3A0,_tmp3A1);}}if(_tmp321 == 0  && _tmp31F == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp3A2="(tuple types have different numbers of components)";
_tag_dynforward(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,51));});goto
_LL241;_LL266: _tmp322=_tmp2A2.f1;if(_tmp322 <= (void*)4)goto _LL268;if(*((int*)
_tmp322)!= 11)goto _LL268;_tmp323=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp322)->f1;_tmp324=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp322)->f2;_tmp325=
_tmp2A2.f2;if(_tmp325 <= (void*)4)goto _LL268;if(*((int*)_tmp325)!= 11)goto _LL268;
_tmp326=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp325)->f1;_tmp327=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp325)->f2;_LL267: if(_tmp326 != _tmp323){
Cyc_Tcutil_failure_reason=({const char*_tmp3A3="(struct and union type)";
_tag_dynforward(_tmp3A3,sizeof(char),_get_zero_arr_size(_tmp3A3,24));});goto
_LL241;}for(0;_tmp327 != 0  && _tmp324 != 0;(_tmp327=_tmp327->tl,_tmp324=_tmp324->tl)){
struct Cyc_Absyn_Aggrfield*_tmp3A4=(struct Cyc_Absyn_Aggrfield*)_tmp327->hd;struct
Cyc_Absyn_Aggrfield*_tmp3A5=(struct Cyc_Absyn_Aggrfield*)_tmp324->hd;if(Cyc_strptrcmp(
_tmp3A4->name,_tmp3A5->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp3A6="(different member names)";
_tag_dynforward(_tmp3A6,sizeof(char),_get_zero_arr_size(_tmp3A6,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp3A4->type,(void*)
_tmp3A5->type);Cyc_Tcutil_unify_tqual(_tmp3A4->tq,(void*)_tmp3A4->type,_tmp3A5->tq,(
void*)_tmp3A5->type);if(!Cyc_Tcutil_same_atts(_tmp3A4->attributes,_tmp3A5->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp3A7="(different attributes on member)";_tag_dynforward(_tmp3A7,
sizeof(char),_get_zero_arr_size(_tmp3A7,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp3A4->width != 0  && _tmp3A5->width == 0  || _tmp3A5->width != 0  && _tmp3A4->width
== 0) || (_tmp3A4->width != 0  && _tmp3A5->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp3A4->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp3A5->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3A8="(different bitfield widths on member)";
_tag_dynforward(_tmp3A8,sizeof(char),_get_zero_arr_size(_tmp3A8,38));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp327 == 0  && _tmp324 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp3A9="(different number of members)";
_tag_dynforward(_tmp3A9,sizeof(char),_get_zero_arr_size(_tmp3A9,30));});goto
_LL241;_LL268: _tmp328=_tmp2A2.f1;if((int)_tmp328 != 2)goto _LL26A;_tmp329=_tmp2A2.f2;
if((int)_tmp329 != 2)goto _LL26A;_LL269: return;_LL26A: _tmp32A=_tmp2A2.f1;if((int)
_tmp32A != 3)goto _LL26C;_tmp32B=_tmp2A2.f2;if((int)_tmp32B != 3)goto _LL26C;_LL26B:
return;_LL26C: _tmp32C=_tmp2A2.f1;if(_tmp32C <= (void*)4)goto _LL26E;if(*((int*)
_tmp32C)!= 15)goto _LL26E;_tmp32D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp32C)->f1;_tmp32E=_tmp2A2.f2;if(_tmp32E <= (void*)4)goto _LL26E;if(*((int*)
_tmp32E)!= 15)goto _LL26E;_tmp32F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp32E)->f1;_LL26D: Cyc_Tcutil_unify_it(_tmp32D,_tmp32F);return;_LL26E: _tmp330=
_tmp2A2.f1;if(_tmp330 <= (void*)4)goto _LL270;if(*((int*)_tmp330)!= 16)goto _LL270;
_tmp331=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp330)->f1;_tmp332=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp330)->f2;_tmp333=_tmp2A2.f2;if(_tmp333 <= (
void*)4)goto _LL270;if(*((int*)_tmp333)!= 16)goto _LL270;_tmp334=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp333)->f1;_tmp335=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp333)->f2;_LL26F: Cyc_Tcutil_unify_it(_tmp331,_tmp334);Cyc_Tcutil_unify_it(
_tmp332,_tmp335);return;_LL270: _tmp336=_tmp2A2.f1;if(_tmp336 <= (void*)4)goto
_LL272;if(*((int*)_tmp336)!= 21)goto _LL272;_LL271: goto _LL273;_LL272: _tmp337=
_tmp2A2.f2;if(_tmp337 <= (void*)4)goto _LL274;if(*((int*)_tmp337)!= 21)goto _LL274;
_LL273: goto _LL275;_LL274: _tmp338=_tmp2A2.f1;if(_tmp338 <= (void*)4)goto _LL276;if(*((
int*)_tmp338)!= 20)goto _LL276;_LL275: goto _LL277;_LL276: _tmp339=_tmp2A2.f1;if(
_tmp339 <= (void*)4)goto _LL278;if(*((int*)_tmp339)!= 22)goto _LL278;_LL277: goto
_LL279;_LL278: _tmp33A=_tmp2A2.f2;if(_tmp33A <= (void*)4)goto _LL27A;if(*((int*)
_tmp33A)!= 22)goto _LL27A;_LL279: goto _LL27B;_LL27A: _tmp33B=_tmp2A2.f2;if(_tmp33B
<= (void*)4)goto _LL27C;if(*((int*)_tmp33B)!= 20)goto _LL27C;_LL27B: if(Cyc_Tcutil_unify_effect(
t1,t2))return;Cyc_Tcutil_failure_reason=({const char*_tmp3AA="(effects don't unify)";
_tag_dynforward(_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,22));});goto
_LL241;_LL27C:;_LL27D: goto _LL241;_LL241:;}(int)_throw((void*)Cyc_Tcutil_Unify);}
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return
0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)
_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){int _tmp3AB=(tq1.real_const + (tq1.q_volatile << 1))+ (
tq1.q_restrict << 2);int _tmp3AC=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict
<< 2);return Cyc_Core_intcmp(_tmp3AB,_tmp3AC);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint_union
_tmp3AD=x->v;void*_tmp3AE;_LL298: if((_tmp3AD.No_constr).tag != 2)goto _LL29A;
_LL299: return - 1;_LL29A: if((_tmp3AD.Eq_constr).tag != 0)goto _LL29C;_tmp3AE=(
_tmp3AD.Eq_constr).f1;_LL29B: {union Cyc_Absyn_Constraint_union _tmp3AF=y->v;void*
_tmp3B0;_LL29F: if((_tmp3AF.No_constr).tag != 2)goto _LL2A1;_LL2A0: return 1;_LL2A1:
if((_tmp3AF.Eq_constr).tag != 0)goto _LL2A3;_tmp3B0=(_tmp3AF.Eq_constr).f1;_LL2A2:
return cmp(_tmp3AE,_tmp3B0);_LL2A3: if((_tmp3AF.Forward_constr).tag != 1)goto _LL29E;
_LL2A4:({void*_tmp3B1[0]={};Cyc_Tcutil_impos(({const char*_tmp3B2="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp3B2,sizeof(char),_get_zero_arr_size(_tmp3B2,40));}),
_tag_dynforward(_tmp3B1,sizeof(void*),0));});_LL29E:;}_LL29C: if((_tmp3AD.Forward_constr).tag
!= 1)goto _LL297;_LL29D:({void*_tmp3B3[0]={};Cyc_Tcutil_impos(({const char*_tmp3B4="unify_conref: forward after compress";
_tag_dynforward(_tmp3B4,sizeof(char),_get_zero_arr_size(_tmp3B4,37));}),
_tag_dynforward(_tmp3B3,sizeof(void*),0));});_LL297:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple5*tqt1,struct _tuple5*tqt2){struct _tuple5 _tmp3B6;struct Cyc_Absyn_Tqual
_tmp3B7;void*_tmp3B8;struct _tuple5*_tmp3B5=tqt1;_tmp3B6=*_tmp3B5;_tmp3B7=_tmp3B6.f1;
_tmp3B8=_tmp3B6.f2;{struct _tuple5 _tmp3BA;struct Cyc_Absyn_Tqual _tmp3BB;void*
_tmp3BC;struct _tuple5*_tmp3B9=tqt2;_tmp3BA=*_tmp3B9;_tmp3BB=_tmp3BA.f1;_tmp3BC=
_tmp3BA.f2;{int _tmp3BD=Cyc_Tcutil_tqual_cmp(_tmp3B7,_tmp3BB);if(_tmp3BD != 0)
return _tmp3BD;return Cyc_Tcutil_typecmp(_tmp3B8,_tmp3BC);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp3BE=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp3BE != 0)return _tmp3BE;{int _tmp3BF=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp3BF != 0)return _tmp3BF;{int _tmp3C0=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp3C0 != 0)return _tmp3C0;{int _tmp3C1=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp3C1 != 0)return
_tmp3C1;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3C2=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3C2 != 0)return _tmp3C2;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3C3=t;_LL2A6: if((int)_tmp3C3 != 0)goto _LL2A8;_LL2A7: return 0;
_LL2A8: if(_tmp3C3 <= (void*)4)goto _LL2B4;if(*((int*)_tmp3C3)!= 0)goto _LL2AA;
_LL2A9: return 1;_LL2AA: if(*((int*)_tmp3C3)!= 1)goto _LL2AC;_LL2AB: return 2;_LL2AC:
if(*((int*)_tmp3C3)!= 2)goto _LL2AE;_LL2AD: return 3;_LL2AE: if(*((int*)_tmp3C3)!= 3)
goto _LL2B0;_LL2AF: return 4;_LL2B0: if(*((int*)_tmp3C3)!= 4)goto _LL2B2;_LL2B1:
return 5;_LL2B2: if(*((int*)_tmp3C3)!= 5)goto _LL2B4;_LL2B3: return 6;_LL2B4: if((int)
_tmp3C3 != 1)goto _LL2B6;_LL2B5: return 7;_LL2B6: if(_tmp3C3 <= (void*)4)goto _LL2CA;
if(*((int*)_tmp3C3)!= 6)goto _LL2B8;_LL2B7: return 8;_LL2B8: if(*((int*)_tmp3C3)!= 7)
goto _LL2BA;_LL2B9: return 9;_LL2BA: if(*((int*)_tmp3C3)!= 8)goto _LL2BC;_LL2BB:
return 10;_LL2BC: if(*((int*)_tmp3C3)!= 9)goto _LL2BE;_LL2BD: return 11;_LL2BE: if(*((
int*)_tmp3C3)!= 10)goto _LL2C0;_LL2BF: return 12;_LL2C0: if(*((int*)_tmp3C3)!= 11)
goto _LL2C2;_LL2C1: return 14;_LL2C2: if(*((int*)_tmp3C3)!= 12)goto _LL2C4;_LL2C3:
return 16;_LL2C4: if(*((int*)_tmp3C3)!= 13)goto _LL2C6;_LL2C5: return 17;_LL2C6: if(*((
int*)_tmp3C3)!= 15)goto _LL2C8;_LL2C7: return 18;_LL2C8: if(*((int*)_tmp3C3)!= 17)
goto _LL2CA;_LL2C9: return 19;_LL2CA: if((int)_tmp3C3 != 3)goto _LL2CC;_LL2CB: return 20;
_LL2CC: if((int)_tmp3C3 != 2)goto _LL2CE;_LL2CD: return 21;_LL2CE: if(_tmp3C3 <= (void*)
4)goto _LL2D0;if(*((int*)_tmp3C3)!= 20)goto _LL2D0;_LL2CF: return 22;_LL2D0: if(
_tmp3C3 <= (void*)4)goto _LL2D2;if(*((int*)_tmp3C3)!= 21)goto _LL2D2;_LL2D1: return
23;_LL2D2: if(_tmp3C3 <= (void*)4)goto _LL2D4;if(*((int*)_tmp3C3)!= 22)goto _LL2D4;
_LL2D3: return 24;_LL2D4: if(_tmp3C3 <= (void*)4)goto _LL2D6;if(*((int*)_tmp3C3)!= 14)
goto _LL2D6;_LL2D5: return 25;_LL2D6: if(_tmp3C3 <= (void*)4)goto _LL2D8;if(*((int*)
_tmp3C3)!= 19)goto _LL2D8;_LL2D7: return 26;_LL2D8: if(_tmp3C3 <= (void*)4)goto _LL2DA;
if(*((int*)_tmp3C3)!= 18)goto _LL2DA;_LL2D9: return 27;_LL2DA: if(_tmp3C3 <= (void*)4)
goto _LL2A5;if(*((int*)_tmp3C3)!= 16)goto _LL2A5;_LL2DB: return 28;_LL2A5:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3C4=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3C4 != 0)return _tmp3C4;{struct _tuple0 _tmp3C6=({struct _tuple0 _tmp3C5;
_tmp3C5.f1=t2;_tmp3C5.f2=t1;_tmp3C5;});void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;
struct Cyc_Absyn_Tvar*_tmp3CA;void*_tmp3CB;struct Cyc_Absyn_Tvar*_tmp3CC;void*
_tmp3CD;struct Cyc_Absyn_AggrInfo _tmp3CE;union Cyc_Absyn_AggrInfoU_union _tmp3CF;
struct Cyc_List_List*_tmp3D0;void*_tmp3D1;struct Cyc_Absyn_AggrInfo _tmp3D2;union
Cyc_Absyn_AggrInfoU_union _tmp3D3;struct Cyc_List_List*_tmp3D4;void*_tmp3D5;struct
_tuple2*_tmp3D6;void*_tmp3D7;struct _tuple2*_tmp3D8;void*_tmp3D9;struct Cyc_List_List*
_tmp3DA;void*_tmp3DB;struct Cyc_List_List*_tmp3DC;void*_tmp3DD;struct Cyc_Absyn_TunionInfo
_tmp3DE;union Cyc_Absyn_TunionInfoU_union _tmp3DF;struct Cyc_Absyn_Tuniondecl**
_tmp3E0;struct Cyc_Absyn_Tuniondecl*_tmp3E1;struct Cyc_List_List*_tmp3E2;struct Cyc_Core_Opt*
_tmp3E3;void*_tmp3E4;struct Cyc_Absyn_TunionInfo _tmp3E5;union Cyc_Absyn_TunionInfoU_union
_tmp3E6;struct Cyc_Absyn_Tuniondecl**_tmp3E7;struct Cyc_Absyn_Tuniondecl*_tmp3E8;
struct Cyc_List_List*_tmp3E9;struct Cyc_Core_Opt*_tmp3EA;void*_tmp3EB;struct Cyc_Absyn_TunionFieldInfo
_tmp3EC;union Cyc_Absyn_TunionFieldInfoU_union _tmp3ED;struct Cyc_Absyn_Tuniondecl*
_tmp3EE;struct Cyc_Absyn_Tunionfield*_tmp3EF;struct Cyc_List_List*_tmp3F0;void*
_tmp3F1;struct Cyc_Absyn_TunionFieldInfo _tmp3F2;union Cyc_Absyn_TunionFieldInfoU_union
_tmp3F3;struct Cyc_Absyn_Tuniondecl*_tmp3F4;struct Cyc_Absyn_Tunionfield*_tmp3F5;
struct Cyc_List_List*_tmp3F6;void*_tmp3F7;struct Cyc_Absyn_PtrInfo _tmp3F8;void*
_tmp3F9;struct Cyc_Absyn_Tqual _tmp3FA;struct Cyc_Absyn_PtrAtts _tmp3FB;void*_tmp3FC;
struct Cyc_Absyn_Conref*_tmp3FD;struct Cyc_Absyn_Conref*_tmp3FE;struct Cyc_Absyn_Conref*
_tmp3FF;void*_tmp400;struct Cyc_Absyn_PtrInfo _tmp401;void*_tmp402;struct Cyc_Absyn_Tqual
_tmp403;struct Cyc_Absyn_PtrAtts _tmp404;void*_tmp405;struct Cyc_Absyn_Conref*
_tmp406;struct Cyc_Absyn_Conref*_tmp407;struct Cyc_Absyn_Conref*_tmp408;void*
_tmp409;void*_tmp40A;void*_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*
_tmp40F;void*_tmp410;void*_tmp411;int _tmp412;void*_tmp413;int _tmp414;void*
_tmp415;struct Cyc_Absyn_ArrayInfo _tmp416;void*_tmp417;struct Cyc_Absyn_Tqual
_tmp418;struct Cyc_Absyn_Exp*_tmp419;struct Cyc_Absyn_Conref*_tmp41A;void*_tmp41B;
struct Cyc_Absyn_ArrayInfo _tmp41C;void*_tmp41D;struct Cyc_Absyn_Tqual _tmp41E;
struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Conref*_tmp420;void*_tmp421;struct
Cyc_Absyn_FnInfo _tmp422;struct Cyc_List_List*_tmp423;struct Cyc_Core_Opt*_tmp424;
void*_tmp425;struct Cyc_List_List*_tmp426;int _tmp427;struct Cyc_Absyn_VarargInfo*
_tmp428;struct Cyc_List_List*_tmp429;struct Cyc_List_List*_tmp42A;void*_tmp42B;
struct Cyc_Absyn_FnInfo _tmp42C;struct Cyc_List_List*_tmp42D;struct Cyc_Core_Opt*
_tmp42E;void*_tmp42F;struct Cyc_List_List*_tmp430;int _tmp431;struct Cyc_Absyn_VarargInfo*
_tmp432;struct Cyc_List_List*_tmp433;struct Cyc_List_List*_tmp434;void*_tmp435;
struct Cyc_List_List*_tmp436;void*_tmp437;struct Cyc_List_List*_tmp438;void*
_tmp439;void*_tmp43A;struct Cyc_List_List*_tmp43B;void*_tmp43C;void*_tmp43D;
struct Cyc_List_List*_tmp43E;void*_tmp43F;void*_tmp440;void*_tmp441;void*_tmp442;
void*_tmp443;void*_tmp444;void*_tmp445;void*_tmp446;void*_tmp447;void*_tmp448;
void*_tmp449;void*_tmp44A;void*_tmp44B;void*_tmp44C;void*_tmp44D;void*_tmp44E;
void*_tmp44F;void*_tmp450;void*_tmp451;int _tmp452;void*_tmp453;int _tmp454;void*
_tmp455;void*_tmp456;void*_tmp457;void*_tmp458;void*_tmp459;void*_tmp45A;_LL2DD:
_tmp3C7=_tmp3C6.f1;if(_tmp3C7 <= (void*)4)goto _LL2DF;if(*((int*)_tmp3C7)!= 0)goto
_LL2DF;_tmp3C8=_tmp3C6.f2;if(_tmp3C8 <= (void*)4)goto _LL2DF;if(*((int*)_tmp3C8)!= 
0)goto _LL2DF;_LL2DE:({void*_tmp45B[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp45C="typecmp: can only compare closed types";
_tag_dynforward(_tmp45C,sizeof(char),_get_zero_arr_size(_tmp45C,39));}),
_tag_dynforward(_tmp45B,sizeof(void*),0));});_LL2DF: _tmp3C9=_tmp3C6.f1;if(
_tmp3C9 <= (void*)4)goto _LL2E1;if(*((int*)_tmp3C9)!= 1)goto _LL2E1;_tmp3CA=((
struct Cyc_Absyn_VarType_struct*)_tmp3C9)->f1;_tmp3CB=_tmp3C6.f2;if(_tmp3CB <= (
void*)4)goto _LL2E1;if(*((int*)_tmp3CB)!= 1)goto _LL2E1;_tmp3CC=((struct Cyc_Absyn_VarType_struct*)
_tmp3CB)->f1;_LL2E0: return Cyc_Core_intcmp(*((int*)_check_null(_tmp3CC->identity)),*((
int*)_check_null(_tmp3CA->identity)));_LL2E1: _tmp3CD=_tmp3C6.f1;if(_tmp3CD <= (
void*)4)goto _LL2E3;if(*((int*)_tmp3CD)!= 10)goto _LL2E3;_tmp3CE=((struct Cyc_Absyn_AggrType_struct*)
_tmp3CD)->f1;_tmp3CF=_tmp3CE.aggr_info;_tmp3D0=_tmp3CE.targs;_tmp3D1=_tmp3C6.f2;
if(_tmp3D1 <= (void*)4)goto _LL2E3;if(*((int*)_tmp3D1)!= 10)goto _LL2E3;_tmp3D2=((
struct Cyc_Absyn_AggrType_struct*)_tmp3D1)->f1;_tmp3D3=_tmp3D2.aggr_info;_tmp3D4=
_tmp3D2.targs;_LL2E2: {struct _tuple2*_tmp45E;struct _tuple6 _tmp45D=Cyc_Absyn_aggr_kinded_name(
_tmp3CF);_tmp45E=_tmp45D.f2;{struct _tuple2*_tmp460;struct _tuple6 _tmp45F=Cyc_Absyn_aggr_kinded_name(
_tmp3D3);_tmp460=_tmp45F.f2;{int _tmp461=Cyc_Absyn_qvar_cmp(_tmp45E,_tmp460);if(
_tmp461 != 0)return _tmp461;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3D0,_tmp3D4);}}}}_LL2E3: _tmp3D5=_tmp3C6.f1;if(_tmp3D5 <= (void*)4)goto _LL2E5;
if(*((int*)_tmp3D5)!= 12)goto _LL2E5;_tmp3D6=((struct Cyc_Absyn_EnumType_struct*)
_tmp3D5)->f1;_tmp3D7=_tmp3C6.f2;if(_tmp3D7 <= (void*)4)goto _LL2E5;if(*((int*)
_tmp3D7)!= 12)goto _LL2E5;_tmp3D8=((struct Cyc_Absyn_EnumType_struct*)_tmp3D7)->f1;
_LL2E4: return Cyc_Absyn_qvar_cmp(_tmp3D6,_tmp3D8);_LL2E5: _tmp3D9=_tmp3C6.f1;if(
_tmp3D9 <= (void*)4)goto _LL2E7;if(*((int*)_tmp3D9)!= 13)goto _LL2E7;_tmp3DA=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3D9)->f1;_tmp3DB=_tmp3C6.f2;if(_tmp3DB
<= (void*)4)goto _LL2E7;if(*((int*)_tmp3DB)!= 13)goto _LL2E7;_tmp3DC=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3DB)->f1;_LL2E6: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3DA,_tmp3DC);_LL2E7: _tmp3DD=_tmp3C6.f1;if(_tmp3DD <= (
void*)4)goto _LL2E9;if(*((int*)_tmp3DD)!= 2)goto _LL2E9;_tmp3DE=((struct Cyc_Absyn_TunionType_struct*)
_tmp3DD)->f1;_tmp3DF=_tmp3DE.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp3DD)->f1).tunion_info).KnownTunion).tag != 1)goto _LL2E9;_tmp3E0=(_tmp3DF.KnownTunion).f1;
_tmp3E1=*_tmp3E0;_tmp3E2=_tmp3DE.targs;_tmp3E3=_tmp3DE.rgn;_tmp3E4=_tmp3C6.f2;
if(_tmp3E4 <= (void*)4)goto _LL2E9;if(*((int*)_tmp3E4)!= 2)goto _LL2E9;_tmp3E5=((
struct Cyc_Absyn_TunionType_struct*)_tmp3E4)->f1;_tmp3E6=_tmp3E5.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3E4)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL2E9;_tmp3E7=(_tmp3E6.KnownTunion).f1;_tmp3E8=*_tmp3E7;_tmp3E9=_tmp3E5.targs;
_tmp3EA=_tmp3E5.rgn;_LL2E8: if(_tmp3E8 == _tmp3E1)return 0;{int _tmp462=Cyc_Absyn_qvar_cmp(
_tmp3E8->name,_tmp3E1->name);if(_tmp462 != 0)return _tmp462;if((unsigned int)
_tmp3EA  && (unsigned int)_tmp3E3){int _tmp463=Cyc_Tcutil_typecmp((void*)_tmp3EA->v,(
void*)_tmp3E3->v);if(_tmp463 != 0)return _tmp463;}else{if((unsigned int)_tmp3EA)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3E9,
_tmp3E2);}_LL2E9: _tmp3EB=_tmp3C6.f1;if(_tmp3EB <= (void*)4)goto _LL2EB;if(*((int*)
_tmp3EB)!= 3)goto _LL2EB;_tmp3EC=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3EB)->f1;_tmp3ED=_tmp3EC.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3EB)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2EB;_tmp3EE=(_tmp3ED.KnownTunionfield).f1;
_tmp3EF=(_tmp3ED.KnownTunionfield).f2;_tmp3F0=_tmp3EC.targs;_tmp3F1=_tmp3C6.f2;
if(_tmp3F1 <= (void*)4)goto _LL2EB;if(*((int*)_tmp3F1)!= 3)goto _LL2EB;_tmp3F2=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3F1)->f1;_tmp3F3=_tmp3F2.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3F1)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL2EB;_tmp3F4=(_tmp3F3.KnownTunionfield).f1;_tmp3F5=(_tmp3F3.KnownTunionfield).f2;
_tmp3F6=_tmp3F2.targs;_LL2EA: if(_tmp3F4 == _tmp3EE)return 0;{int _tmp464=Cyc_Absyn_qvar_cmp(
_tmp3EE->name,_tmp3F4->name);if(_tmp464 != 0)return _tmp464;{int _tmp465=Cyc_Absyn_qvar_cmp(
_tmp3EF->name,_tmp3F5->name);if(_tmp465 != 0)return _tmp465;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3F6,_tmp3F0);}}_LL2EB: _tmp3F7=_tmp3C6.f1;if(_tmp3F7 <= (
void*)4)goto _LL2ED;if(*((int*)_tmp3F7)!= 4)goto _LL2ED;_tmp3F8=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F7)->f1;_tmp3F9=(void*)_tmp3F8.elt_typ;_tmp3FA=_tmp3F8.elt_tq;_tmp3FB=
_tmp3F8.ptr_atts;_tmp3FC=(void*)_tmp3FB.rgn;_tmp3FD=_tmp3FB.nullable;_tmp3FE=
_tmp3FB.bounds;_tmp3FF=_tmp3FB.zero_term;_tmp400=_tmp3C6.f2;if(_tmp400 <= (void*)
4)goto _LL2ED;if(*((int*)_tmp400)!= 4)goto _LL2ED;_tmp401=((struct Cyc_Absyn_PointerType_struct*)
_tmp400)->f1;_tmp402=(void*)_tmp401.elt_typ;_tmp403=_tmp401.elt_tq;_tmp404=
_tmp401.ptr_atts;_tmp405=(void*)_tmp404.rgn;_tmp406=_tmp404.nullable;_tmp407=
_tmp404.bounds;_tmp408=_tmp404.zero_term;_LL2EC: {int _tmp466=Cyc_Tcutil_typecmp(
_tmp402,_tmp3F9);if(_tmp466 != 0)return _tmp466;{int _tmp467=Cyc_Tcutil_typecmp(
_tmp405,_tmp3FC);if(_tmp467 != 0)return _tmp467;{int _tmp468=Cyc_Tcutil_tqual_cmp(
_tmp403,_tmp3FA);if(_tmp468 != 0)return _tmp468;{int _tmp469=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp407,_tmp3FE);if(_tmp469 != 0)return _tmp469;{int _tmp46A=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp408,_tmp3FF);if(_tmp46A != 0)return _tmp46A;{union Cyc_Absyn_Constraint_union
_tmp46B=(Cyc_Absyn_compress_conref(_tmp407))->v;void*_tmp46C;void*_tmp46D;_LL314:
if((_tmp46B.Eq_constr).tag != 0)goto _LL316;_tmp46C=(_tmp46B.Eq_constr).f1;if((int)
_tmp46C != 0)goto _LL316;_LL315: return 0;_LL316: if((_tmp46B.Eq_constr).tag != 0)goto
_LL318;_tmp46D=(_tmp46B.Eq_constr).f1;if((int)_tmp46D != 1)goto _LL318;_LL317:
return 0;_LL318:;_LL319: goto _LL313;_LL313:;}return((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,
_tmp406,_tmp3FD);}}}}}_LL2ED: _tmp409=_tmp3C6.f1;if(_tmp409 <= (void*)4)goto _LL2EF;
if(*((int*)_tmp409)!= 5)goto _LL2EF;_tmp40A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp409)->f1;_tmp40B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp409)->f2;
_tmp40C=_tmp3C6.f2;if(_tmp40C <= (void*)4)goto _LL2EF;if(*((int*)_tmp40C)!= 5)goto
_LL2EF;_tmp40D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp40C)->f1;_tmp40E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp40C)->f2;_LL2EE: if(_tmp40D != _tmp40A)
return Cyc_Core_intcmp((int)((unsigned int)_tmp40D),(int)((unsigned int)_tmp40A));
if(_tmp40E != _tmp40B)return Cyc_Core_intcmp((int)((unsigned int)_tmp40E),(int)((
unsigned int)_tmp40B));return 0;_LL2EF: _tmp40F=_tmp3C6.f1;if((int)_tmp40F != 1)
goto _LL2F1;_tmp410=_tmp3C6.f2;if((int)_tmp410 != 1)goto _LL2F1;_LL2F0: return 0;
_LL2F1: _tmp411=_tmp3C6.f1;if(_tmp411 <= (void*)4)goto _LL2F3;if(*((int*)_tmp411)!= 
6)goto _LL2F3;_tmp412=((struct Cyc_Absyn_DoubleType_struct*)_tmp411)->f1;_tmp413=
_tmp3C6.f2;if(_tmp413 <= (void*)4)goto _LL2F3;if(*((int*)_tmp413)!= 6)goto _LL2F3;
_tmp414=((struct Cyc_Absyn_DoubleType_struct*)_tmp413)->f1;_LL2F2: if(_tmp412 == 
_tmp414)return 0;else{if(_tmp412)return - 1;else{return 1;}}_LL2F3: _tmp415=_tmp3C6.f1;
if(_tmp415 <= (void*)4)goto _LL2F5;if(*((int*)_tmp415)!= 7)goto _LL2F5;_tmp416=((
struct Cyc_Absyn_ArrayType_struct*)_tmp415)->f1;_tmp417=(void*)_tmp416.elt_type;
_tmp418=_tmp416.tq;_tmp419=_tmp416.num_elts;_tmp41A=_tmp416.zero_term;_tmp41B=
_tmp3C6.f2;if(_tmp41B <= (void*)4)goto _LL2F5;if(*((int*)_tmp41B)!= 7)goto _LL2F5;
_tmp41C=((struct Cyc_Absyn_ArrayType_struct*)_tmp41B)->f1;_tmp41D=(void*)_tmp41C.elt_type;
_tmp41E=_tmp41C.tq;_tmp41F=_tmp41C.num_elts;_tmp420=_tmp41C.zero_term;_LL2F4: {
int _tmp46E=Cyc_Tcutil_tqual_cmp(_tmp41E,_tmp418);if(_tmp46E != 0)return _tmp46E;{
int _tmp46F=Cyc_Tcutil_typecmp(_tmp41D,_tmp417);if(_tmp46F != 0)return _tmp46F;{int
_tmp470=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp41A,_tmp420);if(_tmp470 != 0)
return _tmp470;if(_tmp419 == _tmp41F)return 0;if(_tmp419 == 0  || _tmp41F == 0)({void*
_tmp471[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp472="missing expression in array index";_tag_dynforward(_tmp472,
sizeof(char),_get_zero_arr_size(_tmp472,34));}),_tag_dynforward(_tmp471,sizeof(
void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp419,_tmp41F);}}}_LL2F5: _tmp421=_tmp3C6.f1;if(_tmp421 <= (void*)4)goto _LL2F7;
if(*((int*)_tmp421)!= 8)goto _LL2F7;_tmp422=((struct Cyc_Absyn_FnType_struct*)
_tmp421)->f1;_tmp423=_tmp422.tvars;_tmp424=_tmp422.effect;_tmp425=(void*)_tmp422.ret_typ;
_tmp426=_tmp422.args;_tmp427=_tmp422.c_varargs;_tmp428=_tmp422.cyc_varargs;
_tmp429=_tmp422.rgn_po;_tmp42A=_tmp422.attributes;_tmp42B=_tmp3C6.f2;if(_tmp42B
<= (void*)4)goto _LL2F7;if(*((int*)_tmp42B)!= 8)goto _LL2F7;_tmp42C=((struct Cyc_Absyn_FnType_struct*)
_tmp42B)->f1;_tmp42D=_tmp42C.tvars;_tmp42E=_tmp42C.effect;_tmp42F=(void*)_tmp42C.ret_typ;
_tmp430=_tmp42C.args;_tmp431=_tmp42C.c_varargs;_tmp432=_tmp42C.cyc_varargs;
_tmp433=_tmp42C.rgn_po;_tmp434=_tmp42C.attributes;_LL2F6:({void*_tmp473[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp474="typecmp: function types not handled";_tag_dynforward(_tmp474,
sizeof(char),_get_zero_arr_size(_tmp474,36));}),_tag_dynforward(_tmp473,sizeof(
void*),0));});_LL2F7: _tmp435=_tmp3C6.f1;if(_tmp435 <= (void*)4)goto _LL2F9;if(*((
int*)_tmp435)!= 9)goto _LL2F9;_tmp436=((struct Cyc_Absyn_TupleType_struct*)_tmp435)->f1;
_tmp437=_tmp3C6.f2;if(_tmp437 <= (void*)4)goto _LL2F9;if(*((int*)_tmp437)!= 9)goto
_LL2F9;_tmp438=((struct Cyc_Absyn_TupleType_struct*)_tmp437)->f1;_LL2F8: return((
int(*)(int(*cmp)(struct _tuple5*,struct _tuple5*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp438,_tmp436);_LL2F9: _tmp439=
_tmp3C6.f1;if(_tmp439 <= (void*)4)goto _LL2FB;if(*((int*)_tmp439)!= 11)goto _LL2FB;
_tmp43A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp439)->f1;_tmp43B=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp439)->f2;_tmp43C=_tmp3C6.f2;if(_tmp43C
<= (void*)4)goto _LL2FB;if(*((int*)_tmp43C)!= 11)goto _LL2FB;_tmp43D=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp43C)->f1;_tmp43E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp43C)->f2;_LL2FA: if(_tmp43D != _tmp43A){if(_tmp43D == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp43E,_tmp43B);_LL2FB: _tmp43F=_tmp3C6.f1;if(_tmp43F <= (void*)4)goto _LL2FD;if(*((
int*)_tmp43F)!= 15)goto _LL2FD;_tmp440=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp43F)->f1;_tmp441=_tmp3C6.f2;if(_tmp441 <= (void*)4)goto _LL2FD;if(*((int*)
_tmp441)!= 15)goto _LL2FD;_tmp442=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp441)->f1;_LL2FC: return Cyc_Tcutil_typecmp(_tmp440,_tmp442);_LL2FD: _tmp443=
_tmp3C6.f1;if(_tmp443 <= (void*)4)goto _LL2FF;if(*((int*)_tmp443)!= 16)goto _LL2FF;
_tmp444=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp443)->f1;_tmp445=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp443)->f2;_tmp446=_tmp3C6.f2;if(_tmp446 <= (
void*)4)goto _LL2FF;if(*((int*)_tmp446)!= 16)goto _LL2FF;_tmp447=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp446)->f1;_tmp448=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp446)->f2;_LL2FE: {int _tmp475=Cyc_Tcutil_typecmp(_tmp444,_tmp447);if(_tmp475
!= 0)return _tmp475;else{return Cyc_Tcutil_typecmp(_tmp445,_tmp448);}}_LL2FF:
_tmp449=_tmp3C6.f1;if(_tmp449 <= (void*)4)goto _LL301;if(*((int*)_tmp449)!= 14)
goto _LL301;_tmp44A=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp449)->f1;
_tmp44B=_tmp3C6.f2;if(_tmp44B <= (void*)4)goto _LL301;if(*((int*)_tmp44B)!= 14)
goto _LL301;_tmp44C=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp44B)->f1;
_LL300: return Cyc_Tcutil_typecmp(_tmp44A,_tmp44C);_LL301: _tmp44D=_tmp3C6.f1;if(
_tmp44D <= (void*)4)goto _LL303;if(*((int*)_tmp44D)!= 18)goto _LL303;_tmp44E=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp44D)->f1;_tmp44F=_tmp3C6.f2;if(_tmp44F <= (
void*)4)goto _LL303;if(*((int*)_tmp44F)!= 18)goto _LL303;_tmp450=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp44F)->f1;_LL302: return Cyc_Tcutil_typecmp(_tmp44E,
_tmp450);_LL303: _tmp451=_tmp3C6.f1;if(_tmp451 <= (void*)4)goto _LL305;if(*((int*)
_tmp451)!= 19)goto _LL305;_tmp452=((struct Cyc_Absyn_TypeInt_struct*)_tmp451)->f1;
_tmp453=_tmp3C6.f2;if(_tmp453 <= (void*)4)goto _LL305;if(*((int*)_tmp453)!= 19)
goto _LL305;_tmp454=((struct Cyc_Absyn_TypeInt_struct*)_tmp453)->f1;_LL304: return
Cyc_Core_intcmp(_tmp452,_tmp454);_LL305: _tmp455=_tmp3C6.f1;if(_tmp455 <= (void*)4)
goto _LL307;if(*((int*)_tmp455)!= 21)goto _LL307;_LL306: goto _LL308;_LL307: _tmp456=
_tmp3C6.f2;if(_tmp456 <= (void*)4)goto _LL309;if(*((int*)_tmp456)!= 21)goto _LL309;
_LL308: goto _LL30A;_LL309: _tmp457=_tmp3C6.f1;if(_tmp457 <= (void*)4)goto _LL30B;if(*((
int*)_tmp457)!= 20)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp458=_tmp3C6.f1;if(
_tmp458 <= (void*)4)goto _LL30D;if(*((int*)_tmp458)!= 22)goto _LL30D;_LL30C: goto
_LL30E;_LL30D: _tmp459=_tmp3C6.f2;if(_tmp459 <= (void*)4)goto _LL30F;if(*((int*)
_tmp459)!= 22)goto _LL30F;_LL30E: goto _LL310;_LL30F: _tmp45A=_tmp3C6.f2;if(_tmp45A
<= (void*)4)goto _LL311;if(*((int*)_tmp45A)!= 20)goto _LL311;_LL310:({void*_tmp476[
0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp477="typecmp: effects not handled";_tag_dynforward(_tmp477,sizeof(
char),_get_zero_arr_size(_tmp477,29));}),_tag_dynforward(_tmp476,sizeof(void*),0));});
_LL311:;_LL312:({void*_tmp478[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp479="Unmatched case in typecmp";
_tag_dynforward(_tmp479,sizeof(char),_get_zero_arr_size(_tmp479,26));}),
_tag_dynforward(_tmp478,sizeof(void*),0));});_LL2DC:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp47A=Cyc_Tcutil_compress(t);_LL31B: if(_tmp47A <= (void*)4)goto
_LL31D;if(*((int*)_tmp47A)!= 5)goto _LL31D;_LL31C: goto _LL31E;_LL31D: if((int)
_tmp47A != 1)goto _LL31F;_LL31E: goto _LL320;_LL31F: if(_tmp47A <= (void*)4)goto _LL325;
if(*((int*)_tmp47A)!= 6)goto _LL321;_LL320: goto _LL322;_LL321: if(*((int*)_tmp47A)
!= 12)goto _LL323;_LL322: goto _LL324;_LL323: if(*((int*)_tmp47A)!= 13)goto _LL325;
_LL324: return 1;_LL325:;_LL326: return 0;_LL31A:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp47C=({struct _tuple0 _tmp47B;_tmp47B.f1=t1;_tmp47B.f2=t2;_tmp47B;});
void*_tmp47D;int _tmp47E;void*_tmp47F;int _tmp480;void*_tmp481;void*_tmp482;void*
_tmp483;void*_tmp484;void*_tmp485;void*_tmp486;void*_tmp487;void*_tmp488;void*
_tmp489;void*_tmp48A;void*_tmp48B;void*_tmp48C;void*_tmp48D;void*_tmp48E;void*
_tmp48F;void*_tmp490;void*_tmp491;void*_tmp492;void*_tmp493;void*_tmp494;void*
_tmp495;void*_tmp496;void*_tmp497;void*_tmp498;void*_tmp499;void*_tmp49A;void*
_tmp49B;void*_tmp49C;void*_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;void*
_tmp4A1;void*_tmp4A2;void*_tmp4A3;void*_tmp4A4;void*_tmp4A5;void*_tmp4A6;void*
_tmp4A7;void*_tmp4A8;void*_tmp4A9;void*_tmp4AA;void*_tmp4AB;void*_tmp4AC;void*
_tmp4AD;void*_tmp4AE;void*_tmp4AF;void*_tmp4B0;void*_tmp4B1;void*_tmp4B2;void*
_tmp4B3;void*_tmp4B4;void*_tmp4B5;void*_tmp4B6;void*_tmp4B7;void*_tmp4B8;void*
_tmp4B9;void*_tmp4BA;void*_tmp4BB;void*_tmp4BC;void*_tmp4BD;void*_tmp4BE;void*
_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*_tmp4C2;_LL328: _tmp47D=_tmp47C.f1;if(
_tmp47D <= (void*)4)goto _LL32A;if(*((int*)_tmp47D)!= 6)goto _LL32A;_tmp47E=((
struct Cyc_Absyn_DoubleType_struct*)_tmp47D)->f1;_tmp47F=_tmp47C.f2;if(_tmp47F <= (
void*)4)goto _LL32A;if(*((int*)_tmp47F)!= 6)goto _LL32A;_tmp480=((struct Cyc_Absyn_DoubleType_struct*)
_tmp47F)->f1;_LL329: return !_tmp480  && _tmp47E;_LL32A: _tmp481=_tmp47C.f1;if(
_tmp481 <= (void*)4)goto _LL32C;if(*((int*)_tmp481)!= 6)goto _LL32C;_tmp482=_tmp47C.f2;
if((int)_tmp482 != 1)goto _LL32C;_LL32B: goto _LL32D;_LL32C: _tmp483=_tmp47C.f1;if(
_tmp483 <= (void*)4)goto _LL32E;if(*((int*)_tmp483)!= 6)goto _LL32E;_tmp484=_tmp47C.f2;
if(_tmp484 <= (void*)4)goto _LL32E;if(*((int*)_tmp484)!= 5)goto _LL32E;_LL32D: goto
_LL32F;_LL32E: _tmp485=_tmp47C.f1;if(_tmp485 <= (void*)4)goto _LL330;if(*((int*)
_tmp485)!= 6)goto _LL330;_tmp486=_tmp47C.f2;if(_tmp486 <= (void*)4)goto _LL330;if(*((
int*)_tmp486)!= 14)goto _LL330;_LL32F: goto _LL331;_LL330: _tmp487=_tmp47C.f1;if((
int)_tmp487 != 1)goto _LL332;_tmp488=_tmp47C.f2;if(_tmp488 <= (void*)4)goto _LL332;
if(*((int*)_tmp488)!= 14)goto _LL332;_LL331: goto _LL333;_LL332: _tmp489=_tmp47C.f1;
if(_tmp489 <= (void*)4)goto _LL334;if(*((int*)_tmp489)!= 6)goto _LL334;_tmp48A=
_tmp47C.f2;if(_tmp48A <= (void*)4)goto _LL334;if(*((int*)_tmp48A)!= 18)goto _LL334;
_LL333: goto _LL335;_LL334: _tmp48B=_tmp47C.f1;if((int)_tmp48B != 1)goto _LL336;
_tmp48C=_tmp47C.f2;if(_tmp48C <= (void*)4)goto _LL336;if(*((int*)_tmp48C)!= 18)
goto _LL336;_LL335: goto _LL337;_LL336: _tmp48D=_tmp47C.f1;if((int)_tmp48D != 1)goto
_LL338;_tmp48E=_tmp47C.f2;if(_tmp48E <= (void*)4)goto _LL338;if(*((int*)_tmp48E)!= 
5)goto _LL338;_LL337: return 1;_LL338: _tmp48F=_tmp47C.f1;if(_tmp48F <= (void*)4)goto
_LL33A;if(*((int*)_tmp48F)!= 5)goto _LL33A;_tmp490=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48F)->f2;if((int)_tmp490 != 4)goto _LL33A;_tmp491=_tmp47C.f2;if(_tmp491 <= (
void*)4)goto _LL33A;if(*((int*)_tmp491)!= 5)goto _LL33A;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp491)->f2;if((int)_tmp492 != 4)goto _LL33A;_LL339: return 0;_LL33A: _tmp493=
_tmp47C.f1;if(_tmp493 <= (void*)4)goto _LL33C;if(*((int*)_tmp493)!= 5)goto _LL33C;
_tmp494=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp493)->f2;if((int)_tmp494 != 
4)goto _LL33C;_LL33B: return 1;_LL33C: _tmp495=_tmp47C.f1;if(_tmp495 <= (void*)4)goto
_LL33E;if(*((int*)_tmp495)!= 5)goto _LL33E;_tmp496=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp495)->f2;if((int)_tmp496 != 3)goto _LL33E;_tmp497=_tmp47C.f2;if(_tmp497 <= (
void*)4)goto _LL33E;if(*((int*)_tmp497)!= 5)goto _LL33E;_tmp498=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp497)->f2;if((int)_tmp498 != 2)goto _LL33E;_LL33D: goto _LL33F;_LL33E: _tmp499=
_tmp47C.f1;if(_tmp499 <= (void*)4)goto _LL340;if(*((int*)_tmp499)!= 5)goto _LL340;
_tmp49A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp499)->f2;if((int)_tmp49A != 
2)goto _LL340;_tmp49B=_tmp47C.f2;if(_tmp49B <= (void*)4)goto _LL340;if(*((int*)
_tmp49B)!= 5)goto _LL340;_tmp49C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49B)->f2;
if((int)_tmp49C != 3)goto _LL340;_LL33F: return 0;_LL340: _tmp49D=_tmp47C.f1;if(
_tmp49D <= (void*)4)goto _LL342;if(*((int*)_tmp49D)!= 5)goto _LL342;_tmp49E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp49D)->f2;if((int)_tmp49E != 3)goto _LL342;
_tmp49F=_tmp47C.f2;if((int)_tmp49F != 1)goto _LL342;_LL341: goto _LL343;_LL342:
_tmp4A0=_tmp47C.f1;if(_tmp4A0 <= (void*)4)goto _LL344;if(*((int*)_tmp4A0)!= 5)goto
_LL344;_tmp4A1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A0)->f2;if((int)
_tmp4A1 != 2)goto _LL344;_tmp4A2=_tmp47C.f2;if((int)_tmp4A2 != 1)goto _LL344;_LL343:
goto _LL345;_LL344: _tmp4A3=_tmp47C.f1;if(_tmp4A3 <= (void*)4)goto _LL346;if(*((int*)
_tmp4A3)!= 5)goto _LL346;_tmp4A4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A3)->f2;
if((int)_tmp4A4 != 3)goto _LL346;_tmp4A5=_tmp47C.f2;if(_tmp4A5 <= (void*)4)goto
_LL346;if(*((int*)_tmp4A5)!= 5)goto _LL346;_tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A5)->f2;if((int)_tmp4A6 != 1)goto _LL346;_LL345: goto _LL347;_LL346: _tmp4A7=
_tmp47C.f1;if(_tmp4A7 <= (void*)4)goto _LL348;if(*((int*)_tmp4A7)!= 5)goto _LL348;
_tmp4A8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A7)->f2;if((int)_tmp4A8 != 
2)goto _LL348;_tmp4A9=_tmp47C.f2;if(_tmp4A9 <= (void*)4)goto _LL348;if(*((int*)
_tmp4A9)!= 5)goto _LL348;_tmp4AA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A9)->f2;
if((int)_tmp4AA != 1)goto _LL348;_LL347: goto _LL349;_LL348: _tmp4AB=_tmp47C.f1;if(
_tmp4AB <= (void*)4)goto _LL34A;if(*((int*)_tmp4AB)!= 18)goto _LL34A;_tmp4AC=
_tmp47C.f2;if(_tmp4AC <= (void*)4)goto _LL34A;if(*((int*)_tmp4AC)!= 5)goto _LL34A;
_tmp4AD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AC)->f2;if((int)_tmp4AD != 
1)goto _LL34A;_LL349: goto _LL34B;_LL34A: _tmp4AE=_tmp47C.f1;if(_tmp4AE <= (void*)4)
goto _LL34C;if(*((int*)_tmp4AE)!= 14)goto _LL34C;_tmp4AF=_tmp47C.f2;if(_tmp4AF <= (
void*)4)goto _LL34C;if(*((int*)_tmp4AF)!= 5)goto _LL34C;_tmp4B0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AF)->f2;if((int)_tmp4B0 != 1)goto _LL34C;_LL34B: goto _LL34D;_LL34C: _tmp4B1=
_tmp47C.f1;if(_tmp4B1 <= (void*)4)goto _LL34E;if(*((int*)_tmp4B1)!= 5)goto _LL34E;
_tmp4B2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B1)->f2;if((int)_tmp4B2 != 
3)goto _LL34E;_tmp4B3=_tmp47C.f2;if(_tmp4B3 <= (void*)4)goto _LL34E;if(*((int*)
_tmp4B3)!= 5)goto _LL34E;_tmp4B4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B3)->f2;
if((int)_tmp4B4 != 0)goto _LL34E;_LL34D: goto _LL34F;_LL34E: _tmp4B5=_tmp47C.f1;if(
_tmp4B5 <= (void*)4)goto _LL350;if(*((int*)_tmp4B5)!= 5)goto _LL350;_tmp4B6=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4B5)->f2;if((int)_tmp4B6 != 2)goto _LL350;
_tmp4B7=_tmp47C.f2;if(_tmp4B7 <= (void*)4)goto _LL350;if(*((int*)_tmp4B7)!= 5)goto
_LL350;_tmp4B8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B7)->f2;if((int)
_tmp4B8 != 0)goto _LL350;_LL34F: goto _LL351;_LL350: _tmp4B9=_tmp47C.f1;if(_tmp4B9 <= (
void*)4)goto _LL352;if(*((int*)_tmp4B9)!= 5)goto _LL352;_tmp4BA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B9)->f2;if((int)_tmp4BA != 1)goto _LL352;_tmp4BB=_tmp47C.f2;if(_tmp4BB <= (
void*)4)goto _LL352;if(*((int*)_tmp4BB)!= 5)goto _LL352;_tmp4BC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BB)->f2;if((int)_tmp4BC != 0)goto _LL352;_LL351: goto _LL353;_LL352: _tmp4BD=
_tmp47C.f1;if(_tmp4BD <= (void*)4)goto _LL354;if(*((int*)_tmp4BD)!= 18)goto _LL354;
_tmp4BE=_tmp47C.f2;if(_tmp4BE <= (void*)4)goto _LL354;if(*((int*)_tmp4BE)!= 5)goto
_LL354;_tmp4BF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BE)->f2;if((int)
_tmp4BF != 0)goto _LL354;_LL353: goto _LL355;_LL354: _tmp4C0=_tmp47C.f1;if(_tmp4C0 <= (
void*)4)goto _LL356;if(*((int*)_tmp4C0)!= 14)goto _LL356;_tmp4C1=_tmp47C.f2;if(
_tmp4C1 <= (void*)4)goto _LL356;if(*((int*)_tmp4C1)!= 5)goto _LL356;_tmp4C2=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4C1)->f2;if((int)_tmp4C2 != 0)goto _LL356;
_LL355: return 1;_LL356:;_LL357: return 0;_LL327:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){{struct _RegionHandle _tmp4C3=
_new_region("r");struct _RegionHandle*r=& _tmp4C3;_push_region(r);{struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp4C4=
_region_malloc(r,sizeof(*_tmp4C4));_tmp4C4->v=(void*)t1;_tmp4C4;});}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){int _tmp4C5=
0;_npop_handler(0);return _tmp4C5;}}};_pop_region(r);}{struct Cyc_List_List*el=es;
for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,
t)){({struct Cyc_String_pa_struct _tmp4C9;_tmp4C9.tag=0;_tmp4C9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp4C8;_tmp4C8.tag=0;_tmp4C8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp4C6[2]={& _tmp4C8,& _tmp4C9};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp4C7="type mismatch: expecting %s but found %s";
_tag_dynforward(_tmp4C7,sizeof(char),_get_zero_arr_size(_tmp4C7,41));}),
_tag_dynforward(_tmp4C6,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp4CA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL359: if(_tmp4CA <= (void*)4)goto _LL35B;if(*((int*)_tmp4CA)!= 4)goto _LL35B;
_LL35A: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL358;
_LL35B:;_LL35C: return 0;_LL358:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp4CB=Cyc_Tcutil_compress(t);_LL35E: if(_tmp4CB <= (void*)4)goto _LL368;if(*((
int*)_tmp4CB)!= 5)goto _LL360;_LL35F: goto _LL361;_LL360: if(*((int*)_tmp4CB)!= 14)
goto _LL362;_LL361: goto _LL363;_LL362: if(*((int*)_tmp4CB)!= 18)goto _LL364;_LL363:
goto _LL365;_LL364: if(*((int*)_tmp4CB)!= 12)goto _LL366;_LL365: goto _LL367;_LL366:
if(*((int*)_tmp4CB)!= 13)goto _LL368;_LL367: return 1;_LL368:;_LL369: return 0;_LL35D:;}
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*_tmp4CC[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4CD="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4CD,sizeof(char),_get_zero_arr_size(_tmp4CD,44));}),
_tag_dynforward(_tmp4CC,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp4CE[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp4CF="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4CF,sizeof(char),_get_zero_arr_size(_tmp4CF,44));}),
_tag_dynforward(_tmp4CE,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union f1;
union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp4D1=({struct _tuple0 _tmp4D0;_tmp4D0.f1=
t1;_tmp4D0.f2=t2;_tmp4D0;});void*_tmp4D2;struct Cyc_Absyn_PtrInfo _tmp4D3;void*
_tmp4D4;struct Cyc_Absyn_PtrInfo _tmp4D5;void*_tmp4D6;struct Cyc_Absyn_TunionInfo
_tmp4D7;union Cyc_Absyn_TunionInfoU_union _tmp4D8;struct Cyc_Absyn_Tuniondecl**
_tmp4D9;struct Cyc_Absyn_Tuniondecl*_tmp4DA;struct Cyc_List_List*_tmp4DB;struct Cyc_Core_Opt*
_tmp4DC;struct Cyc_Core_Opt _tmp4DD;void*_tmp4DE;void*_tmp4DF;struct Cyc_Absyn_TunionInfo
_tmp4E0;union Cyc_Absyn_TunionInfoU_union _tmp4E1;struct Cyc_Absyn_Tuniondecl**
_tmp4E2;struct Cyc_Absyn_Tuniondecl*_tmp4E3;struct Cyc_List_List*_tmp4E4;struct Cyc_Core_Opt*
_tmp4E5;struct Cyc_Core_Opt _tmp4E6;void*_tmp4E7;void*_tmp4E8;struct Cyc_Absyn_ArrayInfo
_tmp4E9;void*_tmp4EA;struct Cyc_Absyn_Tqual _tmp4EB;struct Cyc_Absyn_Exp*_tmp4EC;
struct Cyc_Absyn_Conref*_tmp4ED;void*_tmp4EE;struct Cyc_Absyn_ArrayInfo _tmp4EF;
void*_tmp4F0;struct Cyc_Absyn_Tqual _tmp4F1;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Conref*
_tmp4F3;void*_tmp4F4;struct Cyc_Absyn_TunionFieldInfo _tmp4F5;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4F6;struct Cyc_Absyn_Tuniondecl*_tmp4F7;struct Cyc_Absyn_Tunionfield*_tmp4F8;
struct Cyc_List_List*_tmp4F9;void*_tmp4FA;struct Cyc_Absyn_TunionInfo _tmp4FB;union
Cyc_Absyn_TunionInfoU_union _tmp4FC;struct Cyc_Absyn_Tuniondecl**_tmp4FD;struct Cyc_Absyn_Tuniondecl*
_tmp4FE;struct Cyc_List_List*_tmp4FF;void*_tmp500;struct Cyc_Absyn_PtrInfo _tmp501;
void*_tmp502;struct Cyc_Absyn_Tqual _tmp503;struct Cyc_Absyn_PtrAtts _tmp504;void*
_tmp505;struct Cyc_Absyn_Conref*_tmp506;struct Cyc_Absyn_Conref*_tmp507;struct Cyc_Absyn_Conref*
_tmp508;void*_tmp509;struct Cyc_Absyn_TunionInfo _tmp50A;union Cyc_Absyn_TunionInfoU_union
_tmp50B;struct Cyc_Absyn_Tuniondecl**_tmp50C;struct Cyc_Absyn_Tuniondecl*_tmp50D;
struct Cyc_List_List*_tmp50E;struct Cyc_Core_Opt*_tmp50F;void*_tmp510;void*_tmp511;
void*_tmp512;void*_tmp513;void*_tmp514;void*_tmp515;void*_tmp516;void*_tmp517;
_LL36B: _tmp4D2=_tmp4D1.f1;if(_tmp4D2 <= (void*)4)goto _LL36D;if(*((int*)_tmp4D2)!= 
4)goto _LL36D;_tmp4D3=((struct Cyc_Absyn_PointerType_struct*)_tmp4D2)->f1;_tmp4D4=
_tmp4D1.f2;if(_tmp4D4 <= (void*)4)goto _LL36D;if(*((int*)_tmp4D4)!= 4)goto _LL36D;
_tmp4D5=((struct Cyc_Absyn_PointerType_struct*)_tmp4D4)->f1;_LL36C: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4D3.ptr_atts).nullable,(
_tmp4D5.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp518=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp4D3.ptr_atts).nullable))->v;
int _tmp519;_LL37E: if((_tmp518.Eq_constr).tag != 0)goto _LL380;_tmp519=(_tmp518.Eq_constr).f1;
_LL37F: okay=!_tmp519;goto _LL37D;_LL380:;_LL381:({void*_tmp51A[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp51B="silent_castable conref not eq";_tag_dynforward(_tmp51B,sizeof(char),
_get_zero_arr_size(_tmp51B,30));}),_tag_dynforward(_tmp51A,sizeof(void*),0));});
_LL37D:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp4D3.ptr_atts).bounds,(
_tmp4D5.ptr_atts).bounds)){struct _tuple13 _tmp51D=({struct _tuple13 _tmp51C;_tmp51C.f1=(
Cyc_Absyn_compress_conref((_tmp4D3.ptr_atts).bounds))->v;_tmp51C.f2=(Cyc_Absyn_compress_conref((
_tmp4D5.ptr_atts).bounds))->v;_tmp51C;});union Cyc_Absyn_Constraint_union _tmp51E;
void*_tmp51F;union Cyc_Absyn_Constraint_union _tmp520;void*_tmp521;union Cyc_Absyn_Constraint_union
_tmp522;_LL383: _tmp51E=_tmp51D.f1;if(((_tmp51D.f1).Eq_constr).tag != 0)goto _LL385;
_tmp51F=(_tmp51E.Eq_constr).f1;_tmp520=_tmp51D.f2;if(((_tmp51D.f2).Eq_constr).tag
!= 0)goto _LL385;_tmp521=(_tmp520.Eq_constr).f1;_LL384:{struct _tuple0 _tmp524=({
struct _tuple0 _tmp523;_tmp523.f1=_tmp51F;_tmp523.f2=_tmp521;_tmp523;});void*
_tmp525;void*_tmp526;void*_tmp527;void*_tmp528;void*_tmp529;void*_tmp52A;void*
_tmp52B;void*_tmp52C;void*_tmp52D;void*_tmp52E;void*_tmp52F;void*_tmp530;void*
_tmp531;struct Cyc_Absyn_Exp*_tmp532;void*_tmp533;struct Cyc_Absyn_Exp*_tmp534;
void*_tmp535;void*_tmp536;struct Cyc_Absyn_Exp*_tmp537;void*_tmp538;void*_tmp539;
struct Cyc_Absyn_Exp*_tmp53A;void*_tmp53B;void*_tmp53C;void*_tmp53D;void*_tmp53E;
void*_tmp53F;struct Cyc_Absyn_Exp*_tmp540;void*_tmp541;void*_tmp542;void*_tmp543;
void*_tmp544;_LL38A: _tmp525=_tmp524.f1;if(_tmp525 <= (void*)2)goto _LL38C;if(*((
int*)_tmp525)!= 0)goto _LL38C;_tmp526=_tmp524.f2;if((int)_tmp526 != 0)goto _LL38C;
_LL38B: goto _LL38D;_LL38C: _tmp527=_tmp524.f1;if(_tmp527 <= (void*)2)goto _LL38E;if(*((
int*)_tmp527)!= 0)goto _LL38E;_tmp528=_tmp524.f2;if((int)_tmp528 != 1)goto _LL38E;
_LL38D: goto _LL38F;_LL38E: _tmp529=_tmp524.f1;if((int)_tmp529 != 1)goto _LL390;
_tmp52A=_tmp524.f2;if((int)_tmp52A != 1)goto _LL390;_LL38F: goto _LL391;_LL390:
_tmp52B=_tmp524.f1;if((int)_tmp52B != 0)goto _LL392;_tmp52C=_tmp524.f2;if((int)
_tmp52C != 1)goto _LL392;_LL391: goto _LL393;_LL392: _tmp52D=_tmp524.f1;if((int)
_tmp52D != 0)goto _LL394;_tmp52E=_tmp524.f2;if((int)_tmp52E != 0)goto _LL394;_LL393:
okay=1;goto _LL389;_LL394: _tmp52F=_tmp524.f1;if((int)_tmp52F != 1)goto _LL396;
_tmp530=_tmp524.f2;if((int)_tmp530 != 0)goto _LL396;_LL395: okay=0;goto _LL389;
_LL396: _tmp531=_tmp524.f1;if(_tmp531 <= (void*)2)goto _LL398;if(*((int*)_tmp531)!= 
0)goto _LL398;_tmp532=((struct Cyc_Absyn_Upper_b_struct*)_tmp531)->f1;_tmp533=
_tmp524.f2;if(_tmp533 <= (void*)2)goto _LL398;if(*((int*)_tmp533)!= 0)goto _LL398;
_tmp534=((struct Cyc_Absyn_Upper_b_struct*)_tmp533)->f1;_LL397: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp534,_tmp532);if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4D5.ptr_atts).zero_term))({void*_tmp545[0]={};Cyc_Tcutil_warn(loc,({const
char*_tmp546="implicit cast to shorter array";_tag_dynforward(_tmp546,sizeof(
char),_get_zero_arr_size(_tmp546,31));}),_tag_dynforward(_tmp545,sizeof(void*),0));});
goto _LL389;_LL398: _tmp535=_tmp524.f1;if((int)_tmp535 != 0)goto _LL39A;_tmp536=
_tmp524.f2;if(_tmp536 <= (void*)2)goto _LL39A;if(*((int*)_tmp536)!= 0)goto _LL39A;
_tmp537=((struct Cyc_Absyn_Upper_b_struct*)_tmp536)->f1;_LL399: _tmp53A=_tmp537;
goto _LL39B;_LL39A: _tmp538=_tmp524.f1;if((int)_tmp538 != 1)goto _LL39C;_tmp539=
_tmp524.f2;if(_tmp539 <= (void*)2)goto _LL39C;if(*((int*)_tmp539)!= 0)goto _LL39C;
_tmp53A=((struct Cyc_Absyn_Upper_b_struct*)_tmp539)->f1;_LL39B: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4D3.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4D5.ptr_atts).bounds))goto _LL389;okay=0;goto
_LL389;_LL39C: _tmp53B=_tmp524.f1;if(_tmp53B <= (void*)2)goto _LL39E;if(*((int*)
_tmp53B)!= 1)goto _LL39E;_tmp53C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp53B)->f1;_tmp53D=_tmp524.f2;if(_tmp53D <= (void*)2)goto _LL39E;if(*((int*)
_tmp53D)!= 1)goto _LL39E;_tmp53E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp53D)->f1;_LL39D: if(!Cyc_Tcutil_unify(_tmp53C,_tmp53E)){struct _tuple0 _tmp548=({
struct _tuple0 _tmp547;_tmp547.f1=Cyc_Tcutil_compress(_tmp53C);_tmp547.f2=Cyc_Tcutil_compress(
_tmp53E);_tmp547;});void*_tmp549;int _tmp54A;void*_tmp54B;int _tmp54C;_LL3A5:
_tmp549=_tmp548.f1;if(_tmp549 <= (void*)4)goto _LL3A7;if(*((int*)_tmp549)!= 19)
goto _LL3A7;_tmp54A=((struct Cyc_Absyn_TypeInt_struct*)_tmp549)->f1;_tmp54B=
_tmp548.f2;if(_tmp54B <= (void*)4)goto _LL3A7;if(*((int*)_tmp54B)!= 19)goto _LL3A7;
_tmp54C=((struct Cyc_Absyn_TypeInt_struct*)_tmp54B)->f1;_LL3A6: if(_tmp54A < 
_tmp54C)okay=0;goto _LL3A4;_LL3A7:;_LL3A8: okay=0;goto _LL3A4;_LL3A4:;}goto _LL389;
_LL39E: _tmp53F=_tmp524.f1;if(_tmp53F <= (void*)2)goto _LL3A0;if(*((int*)_tmp53F)!= 
0)goto _LL3A0;_tmp540=((struct Cyc_Absyn_Upper_b_struct*)_tmp53F)->f1;_tmp541=
_tmp524.f2;if(_tmp541 <= (void*)2)goto _LL3A0;if(*((int*)_tmp541)!= 1)goto _LL3A0;
_tmp542=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp541)->f1;_LL39F: {
unsigned int _tmp54E;int _tmp54F;struct _tuple7 _tmp54D=Cyc_Evexp_eval_const_uint_exp(
_tmp540);_tmp54E=_tmp54D.f1;_tmp54F=_tmp54D.f2;if(!_tmp54F){okay=0;goto _LL389;}{
void*_tmp550=Cyc_Tcutil_compress(_tmp542);int _tmp551;_LL3AA: if(_tmp550 <= (void*)
4)goto _LL3AC;if(*((int*)_tmp550)!= 19)goto _LL3AC;_tmp551=((struct Cyc_Absyn_TypeInt_struct*)
_tmp550)->f1;_LL3AB: if(_tmp54E < _tmp551)okay=0;goto _LL3A9;_LL3AC:;_LL3AD: okay=0;
goto _LL3A9;_LL3A9:;}goto _LL389;}_LL3A0: _tmp543=_tmp524.f1;if(_tmp543 <= (void*)2)
goto _LL3A2;if(*((int*)_tmp543)!= 1)goto _LL3A2;_LL3A1: goto _LL3A3;_LL3A2: _tmp544=
_tmp524.f2;if(_tmp544 <= (void*)2)goto _LL389;if(*((int*)_tmp544)!= 1)goto _LL389;
_LL3A3: okay=0;goto _LL389;_LL389:;}goto _LL382;_LL385: _tmp522=_tmp51D.f1;if(((
_tmp51D.f1).No_constr).tag != 2)goto _LL387;_LL386: okay=0;goto _LL382;_LL387:;
_LL388:({void*_tmp552[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp553="silent_castable conrefs didn't unify";
_tag_dynforward(_tmp553,sizeof(char),_get_zero_arr_size(_tmp553,37));}),
_tag_dynforward(_tmp552,sizeof(void*),0));});_LL382:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp4D3.elt_typ,(void*)_tmp4D5.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp4D3.ptr_atts).rgn,(void*)(_tmp4D5.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp4D3.ptr_atts).rgn,(void*)(_tmp4D5.ptr_atts).rgn));okay=okay  && (!(
_tmp4D3.elt_tq).real_const  || (_tmp4D5.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp4D3.ptr_atts).zero_term,(_tmp4D5.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp4D3.ptr_atts).zero_term)
 && (_tmp4D5.elt_tq).real_const);return okay;}_LL36D: _tmp4D6=_tmp4D1.f1;if(
_tmp4D6 <= (void*)4)goto _LL36F;if(*((int*)_tmp4D6)!= 2)goto _LL36F;_tmp4D7=((
struct Cyc_Absyn_TunionType_struct*)_tmp4D6)->f1;_tmp4D8=_tmp4D7.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp4D6)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL36F;_tmp4D9=(_tmp4D8.KnownTunion).f1;_tmp4DA=*_tmp4D9;_tmp4DB=_tmp4D7.targs;
_tmp4DC=_tmp4D7.rgn;if(_tmp4DC == 0)goto _LL36F;_tmp4DD=*_tmp4DC;_tmp4DE=(void*)
_tmp4DD.v;_tmp4DF=_tmp4D1.f2;if(_tmp4DF <= (void*)4)goto _LL36F;if(*((int*)_tmp4DF)
!= 2)goto _LL36F;_tmp4E0=((struct Cyc_Absyn_TunionType_struct*)_tmp4DF)->f1;
_tmp4E1=_tmp4E0.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp4DF)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL36F;_tmp4E2=(_tmp4E1.KnownTunion).f1;_tmp4E3=*_tmp4E2;_tmp4E4=
_tmp4E0.targs;_tmp4E5=_tmp4E0.rgn;if(_tmp4E5 == 0)goto _LL36F;_tmp4E6=*_tmp4E5;
_tmp4E7=(void*)_tmp4E6.v;_LL36E: if(_tmp4DA != _tmp4E3  || !Cyc_Tcenv_region_outlives(
te,_tmp4DE,_tmp4E7))return 0;for(0;_tmp4DB != 0  && _tmp4E4 != 0;(_tmp4DB=_tmp4DB->tl,
_tmp4E4=_tmp4E4->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4DB->hd,(void*)_tmp4E4->hd))
return 0;}if(_tmp4DB != 0  || _tmp4E4 != 0)return 0;return 1;_LL36F: _tmp4E8=_tmp4D1.f1;
if(_tmp4E8 <= (void*)4)goto _LL371;if(*((int*)_tmp4E8)!= 7)goto _LL371;_tmp4E9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4E8)->f1;_tmp4EA=(void*)_tmp4E9.elt_type;
_tmp4EB=_tmp4E9.tq;_tmp4EC=_tmp4E9.num_elts;_tmp4ED=_tmp4E9.zero_term;_tmp4EE=
_tmp4D1.f2;if(_tmp4EE <= (void*)4)goto _LL371;if(*((int*)_tmp4EE)!= 7)goto _LL371;
_tmp4EF=((struct Cyc_Absyn_ArrayType_struct*)_tmp4EE)->f1;_tmp4F0=(void*)_tmp4EF.elt_type;
_tmp4F1=_tmp4EF.tq;_tmp4F2=_tmp4EF.num_elts;_tmp4F3=_tmp4EF.zero_term;_LL370: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4ED,_tmp4F3) && ((_tmp4EC != 0
 && _tmp4F2 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4EC,(
struct Cyc_Absyn_Exp*)_tmp4F2));return(okay  && Cyc_Tcutil_unify(_tmp4EA,_tmp4F0))
 && (!_tmp4EB.real_const  || _tmp4F1.real_const);}_LL371: _tmp4F4=_tmp4D1.f1;if(
_tmp4F4 <= (void*)4)goto _LL373;if(*((int*)_tmp4F4)!= 3)goto _LL373;_tmp4F5=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4F4)->f1;_tmp4F6=_tmp4F5.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4F4)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL373;_tmp4F7=(_tmp4F6.KnownTunionfield).f1;_tmp4F8=(_tmp4F6.KnownTunionfield).f2;
_tmp4F9=_tmp4F5.targs;_tmp4FA=_tmp4D1.f2;if(_tmp4FA <= (void*)4)goto _LL373;if(*((
int*)_tmp4FA)!= 2)goto _LL373;_tmp4FB=((struct Cyc_Absyn_TunionType_struct*)
_tmp4FA)->f1;_tmp4FC=_tmp4FB.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4FA)->f1).tunion_info).KnownTunion).tag != 1)goto _LL373;_tmp4FD=(_tmp4FC.KnownTunion).f1;
_tmp4FE=*_tmp4FD;_tmp4FF=_tmp4FB.targs;_LL372: if((_tmp4F7 == _tmp4FE  || Cyc_Absyn_qvar_cmp(
_tmp4F7->name,_tmp4FE->name)== 0) && (_tmp4F8->typs == 0  || _tmp4F7->is_flat)){
for(0;_tmp4F9 != 0  && _tmp4FF != 0;(_tmp4F9=_tmp4F9->tl,_tmp4FF=_tmp4FF->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4F9->hd,(void*)_tmp4FF->hd))break;}if(_tmp4F9 == 0
 && _tmp4FF == 0)return 1;}return 0;_LL373: _tmp500=_tmp4D1.f1;if(_tmp500 <= (void*)4)
goto _LL375;if(*((int*)_tmp500)!= 4)goto _LL375;_tmp501=((struct Cyc_Absyn_PointerType_struct*)
_tmp500)->f1;_tmp502=(void*)_tmp501.elt_typ;_tmp503=_tmp501.elt_tq;_tmp504=
_tmp501.ptr_atts;_tmp505=(void*)_tmp504.rgn;_tmp506=_tmp504.nullable;_tmp507=
_tmp504.bounds;_tmp508=_tmp504.zero_term;_tmp509=_tmp4D1.f2;if(_tmp509 <= (void*)
4)goto _LL375;if(*((int*)_tmp509)!= 2)goto _LL375;_tmp50A=((struct Cyc_Absyn_TunionType_struct*)
_tmp509)->f1;_tmp50B=_tmp50A.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp509)->f1).tunion_info).KnownTunion).tag != 1)goto _LL375;_tmp50C=(_tmp50B.KnownTunion).f1;
_tmp50D=*_tmp50C;_tmp50E=_tmp50A.targs;_tmp50F=_tmp50A.rgn;if(!(!_tmp50D->is_flat))
goto _LL375;_LL374:{void*_tmp554=Cyc_Tcutil_compress(_tmp502);struct Cyc_Absyn_TunionFieldInfo
_tmp555;union Cyc_Absyn_TunionFieldInfoU_union _tmp556;struct Cyc_Absyn_Tuniondecl*
_tmp557;struct Cyc_Absyn_Tunionfield*_tmp558;struct Cyc_List_List*_tmp559;_LL3AF:
if(_tmp554 <= (void*)4)goto _LL3B1;if(*((int*)_tmp554)!= 3)goto _LL3B1;_tmp555=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp554)->f1;_tmp556=_tmp555.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp554)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL3B1;_tmp557=(_tmp556.KnownTunionfield).f1;_tmp558=(_tmp556.KnownTunionfield).f2;
_tmp559=_tmp555.targs;_LL3B0: if(!Cyc_Tcutil_unify(_tmp505,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp50F))->v) && !Cyc_Tcenv_region_outlives(te,_tmp505,(void*)
_tmp50F->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp506,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp507,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp508,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp50D->name,_tmp557->name)== 0  && _tmp558->typs
!= 0){int okay=1;for(0;_tmp559 != 0  && _tmp50E != 0;(_tmp559=_tmp559->tl,_tmp50E=
_tmp50E->tl)){if(!Cyc_Tcutil_unify((void*)_tmp559->hd,(void*)_tmp50E->hd)){okay=
0;break;}}if((!okay  || _tmp559 != 0) || _tmp50E != 0)return 0;return 1;}goto _LL3AE;
_LL3B1:;_LL3B2: goto _LL3AE;_LL3AE:;}return 0;_LL375: _tmp510=_tmp4D1.f1;if(_tmp510
<= (void*)4)goto _LL377;if(*((int*)_tmp510)!= 14)goto _LL377;_tmp511=_tmp4D1.f2;
if(_tmp511 <= (void*)4)goto _LL377;if(*((int*)_tmp511)!= 5)goto _LL377;_tmp512=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp511)->f2;if((int)_tmp512 != 2)goto
_LL377;_LL376: goto _LL378;_LL377: _tmp513=_tmp4D1.f1;if(_tmp513 <= (void*)4)goto
_LL379;if(*((int*)_tmp513)!= 14)goto _LL379;_tmp514=_tmp4D1.f2;if(_tmp514 <= (void*)
4)goto _LL379;if(*((int*)_tmp514)!= 5)goto _LL379;_tmp515=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp514)->f2;if((int)_tmp515 != 3)goto _LL379;_LL378: return 1;_LL379: _tmp516=
_tmp4D1.f1;if(_tmp516 <= (void*)4)goto _LL37B;if(*((int*)_tmp516)!= 18)goto _LL37B;
_tmp517=_tmp4D1.f2;if(_tmp517 <= (void*)4)goto _LL37B;if(*((int*)_tmp517)!= 5)goto
_LL37B;_LL37A: return 0;_LL37B:;_LL37C: return Cyc_Tcutil_unify(t1,t2);_LL36A:;}}int
Cyc_Tcutil_is_pointer_type(void*t){void*_tmp55A=Cyc_Tcutil_compress(t);_LL3B4:
if(_tmp55A <= (void*)4)goto _LL3B6;if(*((int*)_tmp55A)!= 4)goto _LL3B6;_LL3B5:
return 1;_LL3B6:;_LL3B7: return 0;_LL3B3:;}int Cyc_Tcutil_is_pointer_or_boxed(void*t,
int*is_dynforward_ptr,int*is_dyneither_ptr){void*_tmp55B=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp55C;struct Cyc_Absyn_PtrAtts _tmp55D;struct Cyc_Absyn_Conref*
_tmp55E;_LL3B9: if(_tmp55B <= (void*)4)goto _LL3BB;if(*((int*)_tmp55B)!= 4)goto
_LL3BB;_tmp55C=((struct Cyc_Absyn_PointerType_struct*)_tmp55B)->f1;_tmp55D=
_tmp55C.ptr_atts;_tmp55E=_tmp55D.bounds;_LL3BA:*is_dynforward_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp55E)== (void*)0;*is_dyneither_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp55E)== (void*)1;return 1;_LL3BB:;_LL3BC: return Cyc_Tcutil_typ_kind(
t)== (void*)2;_LL3B8:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp55F=(
void*)e->r;union Cyc_Absyn_Cnst_union _tmp560;int _tmp561;union Cyc_Absyn_Cnst_union
_tmp562;char _tmp563;union Cyc_Absyn_Cnst_union _tmp564;short _tmp565;union Cyc_Absyn_Cnst_union
_tmp566;long long _tmp567;void*_tmp568;struct Cyc_Absyn_Exp*_tmp569;_LL3BE: if(*((
int*)_tmp55F)!= 0)goto _LL3C0;_tmp560=((struct Cyc_Absyn_Const_e_struct*)_tmp55F)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp55F)->f1).Int_c).tag != 2)goto _LL3C0;
_tmp561=(_tmp560.Int_c).f2;if(_tmp561 != 0)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0:
if(*((int*)_tmp55F)!= 0)goto _LL3C2;_tmp562=((struct Cyc_Absyn_Const_e_struct*)
_tmp55F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp55F)->f1).Char_c).tag != 
0)goto _LL3C2;_tmp563=(_tmp562.Char_c).f2;if(_tmp563 != 0)goto _LL3C2;_LL3C1: goto
_LL3C3;_LL3C2: if(*((int*)_tmp55F)!= 0)goto _LL3C4;_tmp564=((struct Cyc_Absyn_Const_e_struct*)
_tmp55F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp55F)->f1).Short_c).tag != 
1)goto _LL3C4;_tmp565=(_tmp564.Short_c).f2;if(_tmp565 != 0)goto _LL3C4;_LL3C3: goto
_LL3C5;_LL3C4: if(*((int*)_tmp55F)!= 0)goto _LL3C6;_tmp566=((struct Cyc_Absyn_Const_e_struct*)
_tmp55F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp55F)->f1).LongLong_c).tag
!= 3)goto _LL3C6;_tmp567=(_tmp566.LongLong_c).f2;if(_tmp567 != 0)goto _LL3C6;_LL3C5:
return 1;_LL3C6: if(*((int*)_tmp55F)!= 15)goto _LL3C8;_tmp568=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp55F)->f1;_tmp569=((struct Cyc_Absyn_Cast_e_struct*)_tmp55F)->f2;_LL3C7: return
Cyc_Tcutil_is_zero(_tmp569) && Cyc_Tcutil_admits_zero(_tmp568);_LL3C8:;_LL3C9:
return 0;_LL3BD:;}struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((void*)5)};struct Cyc_Core_Opt
Cyc_Tcutil_urk={(void*)((void*)4)};struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp56A=_cycalloc(sizeof(*_tmp56A));
_tmp56A[0]=({struct Cyc_Absyn_Const_e_struct _tmp56B;_tmp56B.tag=0;_tmp56B.f1=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp56C;(_tmp56C.Null_c).tag=
6;_tmp56C;});_tmp56B;});_tmp56A;})));{struct Cyc_List_List*_tmp56D=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp56E=({struct Cyc_Absyn_PointerType_struct*
_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp570;_tmp570.tag=4;_tmp570.f1=({struct Cyc_Absyn_PtrInfo _tmp571;_tmp571.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->v=_tmp56D;_tmp574;}));_tmp571.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp571.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp572;
_tmp572.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({
struct Cyc_Core_Opt*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->v=_tmp56D;
_tmp573;}));_tmp572.nullable=Cyc_Absyn_true_conref;_tmp572.bounds=Cyc_Absyn_empty_conref();
_tmp572.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp572.ptrloc=0;_tmp572;});_tmp571;});_tmp570;});_tmp56F;});(void*)(((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp56E));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dynforward_ptr Cyc_Tcutil_coercion2string(void*c){
void*_tmp575=c;_LL3CB: if((int)_tmp575 != 0)goto _LL3CD;_LL3CC: return({const char*
_tmp576="unknown";_tag_dynforward(_tmp576,sizeof(char),_get_zero_arr_size(
_tmp576,8));});_LL3CD: if((int)_tmp575 != 1)goto _LL3CF;_LL3CE: return({const char*
_tmp577="no coercion";_tag_dynforward(_tmp577,sizeof(char),_get_zero_arr_size(
_tmp577,12));});_LL3CF: if((int)_tmp575 != 2)goto _LL3D1;_LL3D0: return({const char*
_tmp578="null check";_tag_dynforward(_tmp578,sizeof(char),_get_zero_arr_size(
_tmp578,11));});_LL3D1: if((int)_tmp575 != 3)goto _LL3CA;_LL3D2: return({const char*
_tmp579="other coercion";_tag_dynforward(_tmp579,sizeof(char),_get_zero_arr_size(
_tmp579,15));});_LL3CA:;}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(
t2) && Cyc_Tcutil_is_arithmetic_type(t1)){if(Cyc_Tcutil_will_lose_precision(t1,
t2))({struct Cyc_String_pa_struct _tmp57D;_tmp57D.tag=0;_tmp57D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp57C;_tmp57C.tag=0;_tmp57C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp57A[2]={& _tmp57C,& _tmp57D};Cyc_Tcutil_warn(
e->loc,({const char*_tmp57B="integral size mismatch; %s -> %s conversion supplied";
_tag_dynforward(_tmp57B,sizeof(char),_get_zero_arr_size(_tmp57B,53));}),
_tag_dynforward(_tmp57A,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(
void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp581;_tmp581.tag=0;
_tmp581.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp580;_tmp580.tag=0;_tmp580.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp57E[2]={& _tmp580,& _tmp581};Cyc_Tcutil_warn(e->loc,({const char*_tmp57F="implicit cast from %s to %s";
_tag_dynforward(_tmp57F,sizeof(char),_get_zero_arr_size(_tmp57F,28));}),
_tag_dynforward(_tmp57E,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp582=Cyc_Tcutil_compress(t);
_LL3D4: if(_tmp582 <= (void*)4)goto _LL3D6;if(*((int*)_tmp582)!= 5)goto _LL3D6;
_LL3D5: goto _LL3D7;_LL3D6: if((int)_tmp582 != 1)goto _LL3D8;_LL3D7: goto _LL3D9;_LL3D8:
if(_tmp582 <= (void*)4)goto _LL3DA;if(*((int*)_tmp582)!= 6)goto _LL3DA;_LL3D9:
return 1;_LL3DA:;_LL3DB: return 0;_LL3D3:;}static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp584;
struct _RegionHandle*_tmp585;struct _tuple9 _tmp583=*env;_tmp584=_tmp583.f1;_tmp585=
_tmp583.f2;return({struct _tuple5*_tmp586=_region_malloc(_tmp585,sizeof(*_tmp586));
_tmp586->f1=x->tq;_tmp586->f2=Cyc_Tcutil_rsubstitute(_tmp585,_tmp584,(void*)x->type);
_tmp586;});}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(struct _RegionHandle*r,
struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp588;void*_tmp589;struct _tuple5 _tmp587=*
x;_tmp588=_tmp587.f1;_tmp589=_tmp587.f2;return({struct _tuple5*_tmp58A=
_region_malloc(r,sizeof(*_tmp58A));_tmp58A->f1=_tmp588;_tmp58A->f2=_tmp589;
_tmp58A;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp58B=t1;
struct Cyc_List_List*_tmp58C;struct Cyc_Absyn_AggrInfo _tmp58D;union Cyc_Absyn_AggrInfoU_union
_tmp58E;struct Cyc_Absyn_Aggrdecl**_tmp58F;struct Cyc_Absyn_Aggrdecl*_tmp590;
struct Cyc_List_List*_tmp591;void*_tmp592;struct Cyc_List_List*_tmp593;struct Cyc_Absyn_FnInfo
_tmp594;_LL3DD: if((int)_tmp58B != 0)goto _LL3DF;_LL3DE: return 0;_LL3DF: if(_tmp58B <= (
void*)4)goto _LL3E7;if(*((int*)_tmp58B)!= 9)goto _LL3E1;_tmp58C=((struct Cyc_Absyn_TupleType_struct*)
_tmp58B)->f1;_LL3E0: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _RegionHandle*,struct _tuple5*),struct _RegionHandle*env,struct
Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp58C);_LL3E1: if(*((
int*)_tmp58B)!= 10)goto _LL3E3;_tmp58D=((struct Cyc_Absyn_AggrType_struct*)_tmp58B)->f1;
_tmp58E=_tmp58D.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp58B)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL3E3;_tmp58F=(_tmp58E.KnownAggr).f1;_tmp590=*_tmp58F;_tmp591=_tmp58D.targs;
_LL3E2: if((((void*)_tmp590->kind == (void*)1  || _tmp590->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp590->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp590->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp595=
_region_malloc(r,sizeof(*_tmp595));_tmp595->hd=({struct _tuple5*_tmp596=
_region_malloc(r,sizeof(*_tmp596));_tmp596->f1=Cyc_Absyn_empty_tqual(0);_tmp596->f2=
t1;_tmp596;});_tmp595->tl=0;_tmp595;});{struct Cyc_List_List*_tmp597=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp590->tvs,_tmp591);struct _tuple9 env=({struct _tuple9
_tmp598;_tmp598.f1=_tmp597;_tmp598.f2=r;_tmp598;});return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),
struct _tuple9*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp590->impl))->fields);}_LL3E3:
if(*((int*)_tmp58B)!= 11)goto _LL3E5;_tmp592=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp58B)->f1;if((int)_tmp592 != 0)goto _LL3E5;_tmp593=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp58B)->f2;_LL3E4: {struct _tuple9 env=({struct _tuple9 _tmp599;_tmp599.f1=0;
_tmp599.f2=r;_tmp599;});return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp593);}
_LL3E5: if(*((int*)_tmp58B)!= 8)goto _LL3E7;_tmp594=((struct Cyc_Absyn_FnType_struct*)
_tmp58B)->f1;_LL3E6: return({struct Cyc_List_List*_tmp59A=_region_malloc(r,sizeof(*
_tmp59A));_tmp59A->hd=({struct _tuple5*_tmp59B=_region_malloc(r,sizeof(*_tmp59B));
_tmp59B->f1=Cyc_Absyn_const_tqual(0);_tmp59B->f2=t1;_tmp59B;});_tmp59A->tl=0;
_tmp59A;});_LL3E7:;_LL3E8: return({struct Cyc_List_List*_tmp59C=_region_malloc(r,
sizeof(*_tmp59C));_tmp59C->hd=({struct _tuple5*_tmp59D=_region_malloc(r,sizeof(*
_tmp59D));_tmp59D->f1=Cyc_Absyn_empty_tqual(0);_tmp59D->f2=t1;_tmp59D;});_tmp59C->tl=
0;_tmp59C;});_LL3DC:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,
struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*
_tmp59E=(void*)t->hd;_LL3EA: if((int)_tmp59E != 16)goto _LL3EC;_LL3EB: goto _LL3ED;
_LL3EC: if((int)_tmp59E != 3)goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE: if(_tmp59E <= (
void*)17)goto _LL3F0;if(*((int*)_tmp59E)!= 4)goto _LL3F0;_LL3EF: continue;_LL3F0:;
_LL3F1: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL3E9:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp5A0=({struct _tuple0 _tmp59F;_tmp59F.f1=t1;_tmp59F.f2=t2;_tmp59F;});void*
_tmp5A1;struct Cyc_Absyn_PtrInfo _tmp5A2;void*_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A4;
struct Cyc_Absyn_PtrAtts _tmp5A5;void*_tmp5A6;struct Cyc_Absyn_Conref*_tmp5A7;
struct Cyc_Absyn_Conref*_tmp5A8;struct Cyc_Absyn_Conref*_tmp5A9;void*_tmp5AA;
struct Cyc_Absyn_PtrInfo _tmp5AB;void*_tmp5AC;struct Cyc_Absyn_Tqual _tmp5AD;struct
Cyc_Absyn_PtrAtts _tmp5AE;void*_tmp5AF;struct Cyc_Absyn_Conref*_tmp5B0;struct Cyc_Absyn_Conref*
_tmp5B1;struct Cyc_Absyn_Conref*_tmp5B2;void*_tmp5B3;struct Cyc_Absyn_TunionInfo
_tmp5B4;union Cyc_Absyn_TunionInfoU_union _tmp5B5;struct Cyc_Absyn_Tuniondecl**
_tmp5B6;struct Cyc_Absyn_Tuniondecl*_tmp5B7;struct Cyc_List_List*_tmp5B8;struct Cyc_Core_Opt*
_tmp5B9;struct Cyc_Core_Opt _tmp5BA;void*_tmp5BB;void*_tmp5BC;struct Cyc_Absyn_TunionInfo
_tmp5BD;union Cyc_Absyn_TunionInfoU_union _tmp5BE;struct Cyc_Absyn_Tuniondecl**
_tmp5BF;struct Cyc_Absyn_Tuniondecl*_tmp5C0;struct Cyc_List_List*_tmp5C1;struct Cyc_Core_Opt*
_tmp5C2;struct Cyc_Core_Opt _tmp5C3;void*_tmp5C4;void*_tmp5C5;struct Cyc_Absyn_FnInfo
_tmp5C6;void*_tmp5C7;struct Cyc_Absyn_FnInfo _tmp5C8;_LL3F3: _tmp5A1=_tmp5A0.f1;if(
_tmp5A1 <= (void*)4)goto _LL3F5;if(*((int*)_tmp5A1)!= 4)goto _LL3F5;_tmp5A2=((
struct Cyc_Absyn_PointerType_struct*)_tmp5A1)->f1;_tmp5A3=(void*)_tmp5A2.elt_typ;
_tmp5A4=_tmp5A2.elt_tq;_tmp5A5=_tmp5A2.ptr_atts;_tmp5A6=(void*)_tmp5A5.rgn;
_tmp5A7=_tmp5A5.nullable;_tmp5A8=_tmp5A5.bounds;_tmp5A9=_tmp5A5.zero_term;
_tmp5AA=_tmp5A0.f2;if(_tmp5AA <= (void*)4)goto _LL3F5;if(*((int*)_tmp5AA)!= 4)goto
_LL3F5;_tmp5AB=((struct Cyc_Absyn_PointerType_struct*)_tmp5AA)->f1;_tmp5AC=(void*)
_tmp5AB.elt_typ;_tmp5AD=_tmp5AB.elt_tq;_tmp5AE=_tmp5AB.ptr_atts;_tmp5AF=(void*)
_tmp5AE.rgn;_tmp5B0=_tmp5AE.nullable;_tmp5B1=_tmp5AE.bounds;_tmp5B2=_tmp5AE.zero_term;
_LL3F4: if(_tmp5A4.real_const  && !_tmp5AD.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp5A7,_tmp5B0) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp5A7)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5B0))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5A9,
_tmp5B2) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5A9)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5B2))return 0;if(!Cyc_Tcutil_unify(_tmp5A6,_tmp5AF) && !Cyc_Tcenv_region_outlives(
te,_tmp5A6,_tmp5AF))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp5A8,_tmp5B1)){struct _tuple0 _tmp5CA=({struct _tuple0 _tmp5C9;_tmp5C9.f1=Cyc_Absyn_conref_val(
_tmp5A8);_tmp5C9.f2=Cyc_Absyn_conref_val(_tmp5B1);_tmp5C9;});void*_tmp5CB;void*
_tmp5CC;void*_tmp5CD;void*_tmp5CE;void*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;void*
_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D2;_LL3FC: _tmp5CB=_tmp5CA.f1;if(_tmp5CB <= (void*)
2)goto _LL3FE;if(*((int*)_tmp5CB)!= 0)goto _LL3FE;_tmp5CC=_tmp5CA.f2;if((int)
_tmp5CC != 0)goto _LL3FE;_LL3FD: goto _LL3FB;_LL3FE: _tmp5CD=_tmp5CA.f1;if(_tmp5CD <= (
void*)2)goto _LL400;if(*((int*)_tmp5CD)!= 0)goto _LL400;_tmp5CE=_tmp5CA.f2;if((int)
_tmp5CE != 1)goto _LL400;_LL3FF: goto _LL3FB;_LL400: _tmp5CF=_tmp5CA.f1;if(_tmp5CF <= (
void*)2)goto _LL402;if(*((int*)_tmp5CF)!= 0)goto _LL402;_tmp5D0=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5CF)->f1;_tmp5D1=_tmp5CA.f2;if(_tmp5D1 <= (void*)2)goto _LL402;if(*((int*)
_tmp5D1)!= 0)goto _LL402;_tmp5D2=((struct Cyc_Absyn_Upper_b_struct*)_tmp5D1)->f1;
_LL401: if(!Cyc_Evexp_lte_const_exp(_tmp5D2,_tmp5D0))return 0;goto _LL3FB;_LL402:;
_LL403: return 0;_LL3FB:;}return Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*
_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->hd=({struct _tuple0*_tmp5D4=
_cycalloc(sizeof(*_tmp5D4));_tmp5D4->f1=t1;_tmp5D4->f2=t2;_tmp5D4;});_tmp5D3->tl=
assume;_tmp5D3;}),_tmp5A3,_tmp5AC);_LL3F5: _tmp5B3=_tmp5A0.f1;if(_tmp5B3 <= (void*)
4)goto _LL3F7;if(*((int*)_tmp5B3)!= 2)goto _LL3F7;_tmp5B4=((struct Cyc_Absyn_TunionType_struct*)
_tmp5B3)->f1;_tmp5B5=_tmp5B4.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp5B3)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3F7;_tmp5B6=(_tmp5B5.KnownTunion).f1;
_tmp5B7=*_tmp5B6;_tmp5B8=_tmp5B4.targs;_tmp5B9=_tmp5B4.rgn;if(_tmp5B9 == 0)goto
_LL3F7;_tmp5BA=*_tmp5B9;_tmp5BB=(void*)_tmp5BA.v;_tmp5BC=_tmp5A0.f2;if(_tmp5BC <= (
void*)4)goto _LL3F7;if(*((int*)_tmp5BC)!= 2)goto _LL3F7;_tmp5BD=((struct Cyc_Absyn_TunionType_struct*)
_tmp5BC)->f1;_tmp5BE=_tmp5BD.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp5BC)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3F7;_tmp5BF=(_tmp5BE.KnownTunion).f1;
_tmp5C0=*_tmp5BF;_tmp5C1=_tmp5BD.targs;_tmp5C2=_tmp5BD.rgn;if(_tmp5C2 == 0)goto
_LL3F7;_tmp5C3=*_tmp5C2;_tmp5C4=(void*)_tmp5C3.v;_LL3F6: if(_tmp5B7 != _tmp5C0  || 
!Cyc_Tcenv_region_outlives(te,_tmp5BB,_tmp5C4))return 0;for(0;_tmp5B8 != 0  && 
_tmp5C1 != 0;(_tmp5B8=_tmp5B8->tl,_tmp5C1=_tmp5C1->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp5B8->hd,(void*)_tmp5C1->hd))return 0;}if(_tmp5B8 != 0  || _tmp5C1 != 0)
return 0;return 1;_LL3F7: _tmp5C5=_tmp5A0.f1;if(_tmp5C5 <= (void*)4)goto _LL3F9;if(*((
int*)_tmp5C5)!= 8)goto _LL3F9;_tmp5C6=((struct Cyc_Absyn_FnType_struct*)_tmp5C5)->f1;
_tmp5C7=_tmp5A0.f2;if(_tmp5C7 <= (void*)4)goto _LL3F9;if(*((int*)_tmp5C7)!= 8)goto
_LL3F9;_tmp5C8=((struct Cyc_Absyn_FnType_struct*)_tmp5C7)->f1;_LL3F8: if(_tmp5C6.tvars
!= 0  || _tmp5C8.tvars != 0){struct Cyc_List_List*_tmp5D5=_tmp5C6.tvars;struct Cyc_List_List*
_tmp5D6=_tmp5C8.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5D5)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5D6))return 0;{
struct _RegionHandle _tmp5D7=_new_region("r");struct _RegionHandle*r=& _tmp5D7;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp5D5 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp5D5->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp5D6))->hd)){int _tmp5D8=0;_npop_handler(0);
return _tmp5D8;}inst=({struct Cyc_List_List*_tmp5D9=_region_malloc(r,sizeof(*
_tmp5D9));_tmp5D9->hd=({struct _tuple8*_tmp5DA=_region_malloc(r,sizeof(*_tmp5DA));
_tmp5DA->f1=(struct Cyc_Absyn_Tvar*)_tmp5D6->hd;_tmp5DA->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB[0]=({struct Cyc_Absyn_VarType_struct
_tmp5DC;_tmp5DC.tag=1;_tmp5DC.f1=(struct Cyc_Absyn_Tvar*)_tmp5D5->hd;_tmp5DC;});
_tmp5DB;});_tmp5DA;});_tmp5D9->tl=inst;_tmp5D9;});_tmp5D5=_tmp5D5->tl;_tmp5D6=
_tmp5D6->tl;}if(inst != 0){_tmp5C6.tvars=0;_tmp5C8.tvars=0;{int _tmp5E1=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp5DD=_cycalloc(sizeof(*
_tmp5DD));_tmp5DD[0]=({struct Cyc_Absyn_FnType_struct _tmp5DE;_tmp5DE.tag=8;
_tmp5DE.f1=_tmp5C6;_tmp5DE;});_tmp5DD;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_Absyn_FnType_struct
_tmp5E0;_tmp5E0.tag=8;_tmp5E0.f1=_tmp5C8;_tmp5E0;});_tmp5DF;}));_npop_handler(0);
return _tmp5E1;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp5C6.ret_typ,(void*)_tmp5C8.ret_typ))return 0;{struct Cyc_List_List*_tmp5E2=
_tmp5C6.args;struct Cyc_List_List*_tmp5E3=_tmp5C8.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp5E2)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5E3))return 0;for(0;_tmp5E2 != 0;(_tmp5E2=_tmp5E2->tl,_tmp5E3=_tmp5E3->tl)){
struct Cyc_Absyn_Tqual _tmp5E5;void*_tmp5E6;struct _tuple3 _tmp5E4=*((struct _tuple3*)
_tmp5E2->hd);_tmp5E5=_tmp5E4.f2;_tmp5E6=_tmp5E4.f3;{struct Cyc_Absyn_Tqual _tmp5E8;
void*_tmp5E9;struct _tuple3 _tmp5E7=*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp5E3))->hd);_tmp5E8=_tmp5E7.f2;_tmp5E9=_tmp5E7.f3;if(_tmp5E8.real_const
 && !_tmp5E5.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp5E9,_tmp5E6))return
0;}}if(_tmp5C6.c_varargs != _tmp5C8.c_varargs)return 0;if(_tmp5C6.cyc_varargs != 0
 && _tmp5C8.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp5EA=*_tmp5C6.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp5EB=*_tmp5C8.cyc_varargs;if((_tmp5EB.tq).real_const
 && !(_tmp5EA.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp5EB.type,(
void*)_tmp5EA.type))return 0;}else{if(_tmp5C6.cyc_varargs != 0  || _tmp5C8.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5C6.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5C8.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp5C6.rgn_po,_tmp5C8.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp5C6.attributes,_tmp5C8.attributes))return 0;return 1;}_LL3F9:;_LL3FA: return 0;
_LL3F2:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple0 _tmp5ED=({
struct _tuple0 _tmp5EC;_tmp5EC.f1=Cyc_Tcutil_compress(t1);_tmp5EC.f2=Cyc_Tcutil_compress(
t2);_tmp5EC;});void*_tmp5EE;void*_tmp5EF;void*_tmp5F0;void*_tmp5F1;_LL405:
_tmp5EE=_tmp5ED.f1;if(_tmp5EE <= (void*)4)goto _LL407;if(*((int*)_tmp5EE)!= 5)goto
_LL407;_tmp5EF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EE)->f2;_tmp5F0=
_tmp5ED.f2;if(_tmp5F0 <= (void*)4)goto _LL407;if(*((int*)_tmp5F0)!= 5)goto _LL407;
_tmp5F1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F0)->f2;_LL406: return(
_tmp5EF == _tmp5F1  || _tmp5EF == (void*)2  && _tmp5F1 == (void*)3) || _tmp5EF == (
void*)3  && _tmp5F1 == (void*)2;_LL407:;_LL408: return 0;_LL404:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct
_RegionHandle _tmp5F2=_new_region("temp");struct _RegionHandle*temp=& _tmp5F2;
_push_region(temp);{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,te,t2);for(0;tqs2 != 0;(tqs2=
tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){int _tmp5F3=0;_npop_handler(0);return
_tmp5F3;}{struct _tuple5 _tmp5F5;struct Cyc_Absyn_Tqual _tmp5F6;void*_tmp5F7;struct
_tuple5*_tmp5F4=(struct _tuple5*)tqs1->hd;_tmp5F5=*_tmp5F4;_tmp5F6=_tmp5F5.f1;
_tmp5F7=_tmp5F5.f2;{struct _tuple5 _tmp5F9;struct Cyc_Absyn_Tqual _tmp5FA;void*
_tmp5FB;struct _tuple5*_tmp5F8=(struct _tuple5*)tqs2->hd;_tmp5F9=*_tmp5F8;_tmp5FA=
_tmp5F9.f1;_tmp5FB=_tmp5F9.f2;if(_tmp5FA.real_const  && Cyc_Tcutil_subtype(te,
assume,_tmp5F7,_tmp5FB))continue;else{if(Cyc_Tcutil_unify(_tmp5F7,_tmp5FB))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp5F7,_tmp5FB))continue;else{int _tmp5FC=
0;_npop_handler(0);return _tmp5FC;}}}}}}{int _tmp5FD=1;_npop_handler(0);return
_tmp5FD;}};_pop_region(temp);}static int Cyc_Tcutil_is_char_type(void*t){void*
_tmp5FE=Cyc_Tcutil_compress(t);void*_tmp5FF;_LL40A: if(_tmp5FE <= (void*)4)goto
_LL40C;if(*((int*)_tmp5FE)!= 5)goto _LL40C;_tmp5FF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5FE)->f2;if((int)_tmp5FF != 0)goto _LL40C;_LL40B: return 1;_LL40C:;_LL40D: return
0;_LL409:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp600=t2;
void*_tmp601;void*_tmp602;_LL40F: if(_tmp600 <= (void*)4)goto _LL413;if(*((int*)
_tmp600)!= 5)goto _LL411;_tmp601=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp600)->f2;
if((int)_tmp601 != 2)goto _LL411;_LL410: goto _LL412;_LL411: if(*((int*)_tmp600)!= 5)
goto _LL413;_tmp602=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp600)->f2;if((int)
_tmp602 != 3)goto _LL413;_LL412: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL40E;_LL413:;_LL414: goto _LL40E;_LL40E:;}{void*_tmp603=t1;struct Cyc_Absyn_PtrInfo
_tmp604;void*_tmp605;struct Cyc_Absyn_Tqual _tmp606;struct Cyc_Absyn_PtrAtts _tmp607;
void*_tmp608;struct Cyc_Absyn_Conref*_tmp609;struct Cyc_Absyn_Conref*_tmp60A;
struct Cyc_Absyn_Conref*_tmp60B;struct Cyc_Absyn_ArrayInfo _tmp60C;void*_tmp60D;
struct Cyc_Absyn_Tqual _tmp60E;struct Cyc_Absyn_Exp*_tmp60F;struct Cyc_Absyn_Conref*
_tmp610;struct Cyc_Absyn_Enumdecl*_tmp611;_LL416: if(_tmp603 <= (void*)4)goto _LL41E;
if(*((int*)_tmp603)!= 4)goto _LL418;_tmp604=((struct Cyc_Absyn_PointerType_struct*)
_tmp603)->f1;_tmp605=(void*)_tmp604.elt_typ;_tmp606=_tmp604.elt_tq;_tmp607=
_tmp604.ptr_atts;_tmp608=(void*)_tmp607.rgn;_tmp609=_tmp607.nullable;_tmp60A=
_tmp607.bounds;_tmp60B=_tmp607.zero_term;_LL417:{void*_tmp612=t2;struct Cyc_Absyn_PtrInfo
_tmp613;void*_tmp614;struct Cyc_Absyn_Tqual _tmp615;struct Cyc_Absyn_PtrAtts _tmp616;
void*_tmp617;struct Cyc_Absyn_Conref*_tmp618;struct Cyc_Absyn_Conref*_tmp619;
struct Cyc_Absyn_Conref*_tmp61A;_LL425: if(_tmp612 <= (void*)4)goto _LL427;if(*((int*)
_tmp612)!= 4)goto _LL427;_tmp613=((struct Cyc_Absyn_PointerType_struct*)_tmp612)->f1;
_tmp614=(void*)_tmp613.elt_typ;_tmp615=_tmp613.elt_tq;_tmp616=_tmp613.ptr_atts;
_tmp617=(void*)_tmp616.rgn;_tmp618=_tmp616.nullable;_tmp619=_tmp616.bounds;
_tmp61A=_tmp616.zero_term;_LL426: {void*coercion=(void*)3;struct Cyc_List_List*
_tmp61B=({struct Cyc_List_List*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->hd=({
struct _tuple0*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->f1=t1;_tmp62B->f2=t2;
_tmp62B;});_tmp62A->tl=0;_tmp62A;});int _tmp61C=Cyc_Tcutil_ptrsubtype(te,_tmp61B,
_tmp605,_tmp614) && (!_tmp606.real_const  || _tmp615.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp60B,_tmp61A) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp61A);int _tmp61D=_tmp61C?0:((Cyc_Tcutil_bits_only(_tmp605) && Cyc_Tcutil_is_char_type(
_tmp614)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp61A)) && (_tmp615.real_const  || !_tmp606.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp60A,_tmp619);if(!bounds_ok  && !_tmp61D){struct
_tuple0 _tmp61F=({struct _tuple0 _tmp61E;_tmp61E.f1=Cyc_Absyn_conref_val(_tmp60A);
_tmp61E.f2=Cyc_Absyn_conref_val(_tmp619);_tmp61E;});void*_tmp620;struct Cyc_Absyn_Exp*
_tmp621;void*_tmp622;struct Cyc_Absyn_Exp*_tmp623;void*_tmp624;void*_tmp625;void*
_tmp626;void*_tmp627;void*_tmp628;void*_tmp629;_LL42A: _tmp620=_tmp61F.f1;if(
_tmp620 <= (void*)2)goto _LL42C;if(*((int*)_tmp620)!= 0)goto _LL42C;_tmp621=((
struct Cyc_Absyn_Upper_b_struct*)_tmp620)->f1;_tmp622=_tmp61F.f2;if(_tmp622 <= (
void*)2)goto _LL42C;if(*((int*)_tmp622)!= 0)goto _LL42C;_tmp623=((struct Cyc_Absyn_Upper_b_struct*)
_tmp622)->f1;_LL42B: if(Cyc_Evexp_lte_const_exp(_tmp623,_tmp621))bounds_ok=1;goto
_LL429;_LL42C: _tmp624=_tmp61F.f1;if(_tmp624 <= (void*)2)goto _LL42E;if(*((int*)
_tmp624)!= 1)goto _LL42E;_tmp625=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp624)->f1;_tmp626=_tmp61F.f2;if(_tmp626 <= (void*)2)goto _LL42E;if(*((int*)
_tmp626)!= 1)goto _LL42E;_tmp627=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp626)->f1;_LL42D: bounds_ok=Cyc_Tcutil_unify(_tmp625,_tmp627);goto _LL429;
_LL42E: _tmp628=_tmp61F.f1;if(_tmp628 <= (void*)2)goto _LL430;if(*((int*)_tmp628)!= 
1)goto _LL430;_LL42F: goto _LL431;_LL430: _tmp629=_tmp61F.f2;if(_tmp629 <= (void*)2)
goto _LL432;if(*((int*)_tmp629)!= 1)goto _LL432;_LL431: bounds_ok=0;goto _LL429;
_LL432:;_LL433: bounds_ok=1;goto _LL429;_LL429:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp609) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp618))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp61C  || _tmp61D)) && (Cyc_Tcutil_unify(_tmp608,_tmp617) || Cyc_Tcenv_region_outlives(
te,_tmp608,_tmp617)))return coercion;else{return(void*)0;}}}_LL427:;_LL428: goto
_LL424;_LL424:;}return(void*)0;_LL418: if(*((int*)_tmp603)!= 7)goto _LL41A;_tmp60C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp603)->f1;_tmp60D=(void*)_tmp60C.elt_type;
_tmp60E=_tmp60C.tq;_tmp60F=_tmp60C.num_elts;_tmp610=_tmp60C.zero_term;_LL419:{
void*_tmp62C=t2;struct Cyc_Absyn_ArrayInfo _tmp62D;void*_tmp62E;struct Cyc_Absyn_Tqual
_tmp62F;struct Cyc_Absyn_Exp*_tmp630;struct Cyc_Absyn_Conref*_tmp631;_LL435: if(
_tmp62C <= (void*)4)goto _LL437;if(*((int*)_tmp62C)!= 7)goto _LL437;_tmp62D=((
struct Cyc_Absyn_ArrayType_struct*)_tmp62C)->f1;_tmp62E=(void*)_tmp62D.elt_type;
_tmp62F=_tmp62D.tq;_tmp630=_tmp62D.num_elts;_tmp631=_tmp62D.zero_term;_LL436: {
int okay;okay=((_tmp60F != 0  && _tmp630 != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp610,_tmp631)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp630,(
struct Cyc_Absyn_Exp*)_tmp60F);return(okay  && Cyc_Tcutil_unify(_tmp60D,_tmp62E))
 && (!_tmp60E.real_const  || _tmp62F.real_const)?(void*)3:(void*)0;}_LL437:;
_LL438: return(void*)0;_LL434:;}return(void*)0;_LL41A: if(*((int*)_tmp603)!= 12)
goto _LL41C;_tmp611=((struct Cyc_Absyn_EnumType_struct*)_tmp603)->f2;_LL41B:{void*
_tmp632=t2;struct Cyc_Absyn_Enumdecl*_tmp633;_LL43A: if(_tmp632 <= (void*)4)goto
_LL43C;if(*((int*)_tmp632)!= 12)goto _LL43C;_tmp633=((struct Cyc_Absyn_EnumType_struct*)
_tmp632)->f2;_LL43B: if((_tmp611->fields != 0  && _tmp633->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp611->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp633->fields))->v))
return(void*)1;goto _LL439;_LL43C:;_LL43D: goto _LL439;_LL439:;}goto _LL41D;_LL41C:
if(*((int*)_tmp603)!= 5)goto _LL41E;_LL41D: goto _LL41F;_LL41E: if((int)_tmp603 != 1)
goto _LL420;_LL41F: goto _LL421;_LL420: if(_tmp603 <= (void*)4)goto _LL422;if(*((int*)
_tmp603)!= 6)goto _LL422;_LL421: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL422:;_LL423: return(void*)0;_LL415:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp634=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp635=_cycalloc(
sizeof(*_tmp635));_tmp635[0]=({struct Cyc_Absyn_Cast_e_struct _tmp636;_tmp636.tag=
15;_tmp636.f1=(void*)t;_tmp636.f2=_tmp634;_tmp636.f3=0;_tmp636.f4=(void*)c;
_tmp636;});_tmp635;})));e->topt=({struct Cyc_Core_Opt*_tmp637=_cycalloc(sizeof(*
_tmp637));_tmp637->v=(void*)t;_tmp637;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp638=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL43F: if(_tmp638 <= (void*)4)goto _LL44B;if(*((int*)_tmp638)!= 5)goto _LL441;
_LL440: goto _LL442;_LL441: if(*((int*)_tmp638)!= 12)goto _LL443;_LL442: goto _LL444;
_LL443: if(*((int*)_tmp638)!= 13)goto _LL445;_LL444: goto _LL446;_LL445: if(*((int*)
_tmp638)!= 18)goto _LL447;_LL446: goto _LL448;_LL447: if(*((int*)_tmp638)!= 14)goto
_LL449;_LL448: return 1;_LL449: if(*((int*)_tmp638)!= 0)goto _LL44B;_LL44A: return Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL44B:;
_LL44C: return 0;_LL43E:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp639=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL44E: if((int)_tmp639 != 1)goto _LL450;_LL44F: goto
_LL451;_LL450: if(_tmp639 <= (void*)4)goto _LL452;if(*((int*)_tmp639)!= 6)goto
_LL452;_LL451: return 1;_LL452:;_LL453: return 0;_LL44D:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp63A=Cyc_Tcutil_compress(t);_LL455: if(_tmp63A <= (void*)4)goto
_LL457;if(*((int*)_tmp63A)!= 8)goto _LL457;_LL456: return 1;_LL457:;_LL458: return 0;
_LL454:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp63C=({struct _tuple0 _tmp63B;_tmp63B.f1=t1;_tmp63B.f2=t2;_tmp63B;});void*
_tmp63D;int _tmp63E;void*_tmp63F;int _tmp640;void*_tmp641;void*_tmp642;void*
_tmp643;void*_tmp644;void*_tmp645;void*_tmp646;void*_tmp647;void*_tmp648;void*
_tmp649;void*_tmp64A;void*_tmp64B;void*_tmp64C;void*_tmp64D;void*_tmp64E;void*
_tmp64F;void*_tmp650;void*_tmp651;void*_tmp652;void*_tmp653;void*_tmp654;void*
_tmp655;void*_tmp656;void*_tmp657;void*_tmp658;void*_tmp659;void*_tmp65A;void*
_tmp65B;void*_tmp65C;void*_tmp65D;void*_tmp65E;_LL45A: _tmp63D=_tmp63C.f1;if(
_tmp63D <= (void*)4)goto _LL45C;if(*((int*)_tmp63D)!= 6)goto _LL45C;_tmp63E=((
struct Cyc_Absyn_DoubleType_struct*)_tmp63D)->f1;_tmp63F=_tmp63C.f2;if(_tmp63F <= (
void*)4)goto _LL45C;if(*((int*)_tmp63F)!= 6)goto _LL45C;_tmp640=((struct Cyc_Absyn_DoubleType_struct*)
_tmp63F)->f1;_LL45B: if(_tmp63E)return t1;else{return t2;}_LL45C: _tmp641=_tmp63C.f1;
if(_tmp641 <= (void*)4)goto _LL45E;if(*((int*)_tmp641)!= 6)goto _LL45E;_LL45D:
return t1;_LL45E: _tmp642=_tmp63C.f2;if(_tmp642 <= (void*)4)goto _LL460;if(*((int*)
_tmp642)!= 6)goto _LL460;_LL45F: return t2;_LL460: _tmp643=_tmp63C.f1;if((int)
_tmp643 != 1)goto _LL462;_LL461: goto _LL463;_LL462: _tmp644=_tmp63C.f2;if((int)
_tmp644 != 1)goto _LL464;_LL463: return(void*)1;_LL464: _tmp645=_tmp63C.f1;if(
_tmp645 <= (void*)4)goto _LL466;if(*((int*)_tmp645)!= 5)goto _LL466;_tmp646=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp645)->f1;if((int)_tmp646 != 1)goto _LL466;
_tmp647=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp645)->f2;if((int)_tmp647 != 
4)goto _LL466;_LL465: goto _LL467;_LL466: _tmp648=_tmp63C.f2;if(_tmp648 <= (void*)4)
goto _LL468;if(*((int*)_tmp648)!= 5)goto _LL468;_tmp649=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp648)->f1;if((int)_tmp649 != 1)goto _LL468;_tmp64A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp648)->f2;if((int)_tmp64A != 4)goto _LL468;_LL467: return Cyc_Absyn_ulonglong_typ;
_LL468: _tmp64B=_tmp63C.f1;if(_tmp64B <= (void*)4)goto _LL46A;if(*((int*)_tmp64B)!= 
5)goto _LL46A;_tmp64C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64B)->f2;if((
int)_tmp64C != 4)goto _LL46A;_LL469: goto _LL46B;_LL46A: _tmp64D=_tmp63C.f2;if(
_tmp64D <= (void*)4)goto _LL46C;if(*((int*)_tmp64D)!= 5)goto _LL46C;_tmp64E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp64D)->f2;if((int)_tmp64E != 4)goto _LL46C;
_LL46B: return Cyc_Absyn_slonglong_typ;_LL46C: _tmp64F=_tmp63C.f1;if(_tmp64F <= (
void*)4)goto _LL46E;if(*((int*)_tmp64F)!= 5)goto _LL46E;_tmp650=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64F)->f1;if((int)_tmp650 != 1)goto _LL46E;_tmp651=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64F)->f2;if((int)_tmp651 != 3)goto _LL46E;_LL46D: goto _LL46F;_LL46E: _tmp652=
_tmp63C.f2;if(_tmp652 <= (void*)4)goto _LL470;if(*((int*)_tmp652)!= 5)goto _LL470;
_tmp653=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp652)->f1;if((int)_tmp653 != 
1)goto _LL470;_tmp654=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp652)->f2;if((
int)_tmp654 != 3)goto _LL470;_LL46F: return Cyc_Absyn_ulong_typ;_LL470: _tmp655=
_tmp63C.f1;if(_tmp655 <= (void*)4)goto _LL472;if(*((int*)_tmp655)!= 5)goto _LL472;
_tmp656=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp655)->f1;if((int)_tmp656 != 
1)goto _LL472;_tmp657=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp655)->f2;if((
int)_tmp657 != 2)goto _LL472;_LL471: goto _LL473;_LL472: _tmp658=_tmp63C.f2;if(
_tmp658 <= (void*)4)goto _LL474;if(*((int*)_tmp658)!= 5)goto _LL474;_tmp659=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp658)->f1;if((int)_tmp659 != 1)goto _LL474;
_tmp65A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp658)->f2;if((int)_tmp65A != 
2)goto _LL474;_LL473: return Cyc_Absyn_uint_typ;_LL474: _tmp65B=_tmp63C.f1;if(
_tmp65B <= (void*)4)goto _LL476;if(*((int*)_tmp65B)!= 5)goto _LL476;_tmp65C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp65B)->f2;if((int)_tmp65C != 3)goto _LL476;
_LL475: goto _LL477;_LL476: _tmp65D=_tmp63C.f2;if(_tmp65D <= (void*)4)goto _LL478;if(*((
int*)_tmp65D)!= 5)goto _LL478;_tmp65E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp65D)->f2;if((int)_tmp65E != 3)goto _LL478;_LL477: return Cyc_Absyn_slong_typ;
_LL478:;_LL479: return Cyc_Absyn_sint_typ;_LL459:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp65F=(void*)e->r;struct Cyc_Core_Opt*_tmp660;
_LL47B: if(*((int*)_tmp65F)!= 4)goto _LL47D;_tmp660=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp65F)->f2;if(_tmp660 != 0)goto _LL47D;_LL47C:({void*_tmp661[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp662="assignment in test";_tag_dynforward(_tmp662,sizeof(
char),_get_zero_arr_size(_tmp662,19));}),_tag_dynforward(_tmp661,sizeof(void*),0));});
goto _LL47A;_LL47D:;_LL47E: goto _LL47A;_LL47A:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp664=({struct _tuple0 _tmp663;_tmp663.f1=c1;_tmp663.f2=c2;_tmp663;});
void*_tmp665;void*_tmp666;void*_tmp667;void*_tmp668;void*_tmp669;struct Cyc_Core_Opt*
_tmp66A;struct Cyc_Core_Opt**_tmp66B;void*_tmp66C;struct Cyc_Core_Opt*_tmp66D;
struct Cyc_Core_Opt**_tmp66E;void*_tmp66F;struct Cyc_Core_Opt*_tmp670;struct Cyc_Core_Opt**
_tmp671;void*_tmp672;void*_tmp673;void*_tmp674;void*_tmp675;void*_tmp676;void*
_tmp677;struct Cyc_Core_Opt*_tmp678;struct Cyc_Core_Opt**_tmp679;void*_tmp67A;void*
_tmp67B;struct Cyc_Core_Opt*_tmp67C;struct Cyc_Core_Opt**_tmp67D;void*_tmp67E;void*
_tmp67F;struct Cyc_Core_Opt*_tmp680;struct Cyc_Core_Opt**_tmp681;void*_tmp682;
_LL480: _tmp665=_tmp664.f1;if(*((int*)_tmp665)!= 0)goto _LL482;_tmp666=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp665)->f1;_tmp667=_tmp664.f2;if(*((int*)_tmp667)
!= 0)goto _LL482;_tmp668=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp667)->f1;
_LL481: return _tmp666 == _tmp668;_LL482: _tmp669=_tmp664.f2;if(*((int*)_tmp669)!= 1)
goto _LL484;_tmp66A=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp669)->f1;_tmp66B=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp669)->f1;_LL483:*
_tmp66B=({struct Cyc_Core_Opt*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->v=(
void*)c1;_tmp683;});return 1;_LL484: _tmp66C=_tmp664.f1;if(*((int*)_tmp66C)!= 1)
goto _LL486;_tmp66D=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp66C)->f1;_tmp66E=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp66C)->f1;_LL485:*
_tmp66E=({struct Cyc_Core_Opt*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->v=(
void*)c2;_tmp684;});return 1;_LL486: _tmp66F=_tmp664.f1;if(*((int*)_tmp66F)!= 2)
goto _LL488;_tmp670=((struct Cyc_Absyn_Less_kb_struct*)_tmp66F)->f1;_tmp671=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp66F)->f1;_tmp672=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp66F)->f2;_tmp673=_tmp664.f2;if(*((
int*)_tmp673)!= 0)goto _LL488;_tmp674=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp673)->f1;_LL487: if(Cyc_Tcutil_kind_leq(_tmp674,_tmp672)){*_tmp671=({struct
Cyc_Core_Opt*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->v=(void*)c2;_tmp685;});
return 1;}else{return 0;}_LL488: _tmp675=_tmp664.f1;if(*((int*)_tmp675)!= 0)goto
_LL48A;_tmp676=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp675)->f1;_tmp677=
_tmp664.f2;if(*((int*)_tmp677)!= 2)goto _LL48A;_tmp678=((struct Cyc_Absyn_Less_kb_struct*)
_tmp677)->f1;_tmp679=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp677)->f1;_tmp67A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp677)->f2;
_LL489: if(Cyc_Tcutil_kind_leq(_tmp676,_tmp67A)){*_tmp679=({struct Cyc_Core_Opt*
_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->v=(void*)c1;_tmp686;});return 1;}
else{return 0;}_LL48A: _tmp67B=_tmp664.f1;if(*((int*)_tmp67B)!= 2)goto _LL47F;
_tmp67C=((struct Cyc_Absyn_Less_kb_struct*)_tmp67B)->f1;_tmp67D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp67B)->f1;_tmp67E=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp67B)->f2;_tmp67F=_tmp664.f2;if(*((int*)_tmp67F)!= 2)goto _LL47F;_tmp680=((
struct Cyc_Absyn_Less_kb_struct*)_tmp67F)->f1;_tmp681=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp67F)->f1;_tmp682=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp67F)->f2;_LL48B: if(Cyc_Tcutil_kind_leq(_tmp67E,_tmp682)){*_tmp681=({struct
Cyc_Core_Opt*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687->v=(void*)c1;_tmp687;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp682,_tmp67E)){*_tmp67D=({struct Cyc_Core_Opt*
_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->v=(void*)c2;_tmp688;});return 1;}
else{return 0;}}_LL47F:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp689=_cycalloc_atomic(sizeof(*_tmp689));_tmp689[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp689;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dynforward_ptr s=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp68E;_tmp68E.tag=1;_tmp68E.f1=(
unsigned long)i;{void*_tmp68C[1]={& _tmp68E};Cyc_aprintf(({const char*_tmp68D="#%d";
_tag_dynforward(_tmp68D,sizeof(char),_get_zero_arr_size(_tmp68D,4));}),
_tag_dynforward(_tmp68C,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->name=({struct _dynforward_ptr*
_tmp68B=_cycalloc(sizeof(struct _dynforward_ptr)* 1);_tmp68B[0]=s;_tmp68B;});
_tmp68A->identity=0;_tmp68A->kind=(void*)k;_tmp68A;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dynforward_ptr _tmp68F=*t->name;return*((const char*)
_check_dynforward_subscript(_tmp68F,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp692;_tmp692.tag=0;
_tmp692.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*t->name);{void*
_tmp690[1]={& _tmp692};Cyc_printf(({const char*_tmp691="%s";_tag_dynforward(
_tmp691,sizeof(char),_get_zero_arr_size(_tmp691,3));}),_tag_dynforward(_tmp690,
sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(t))return;{struct
_dynforward_ptr _tmp693=Cyc_strconcat(({const char*_tmp698="`";_tag_dynforward(
_tmp698,sizeof(char),_get_zero_arr_size(_tmp698,2));}),(struct _dynforward_ptr)*t->name);({
struct _dynforward_ptr _tmp694=_dynforward_ptr_plus(_tmp693,sizeof(char),1);char
_tmp695=*((char*)_check_dynforward_subscript(_tmp694,sizeof(char),0));char
_tmp696='t';if(_get_dynforward_size(_tmp694,sizeof(char))== 1  && (_tmp695 == '\000'
 && _tmp696 != '\000'))_throw_arraybounds();*((char*)_tmp694.curr)=_tmp696;});t->name=({
struct _dynforward_ptr*_tmp697=_cycalloc(sizeof(struct _dynforward_ptr)* 1);
_tmp697[0]=(struct _dynforward_ptr)_tmp693;_tmp697;});}}struct _tuple14{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(
struct _tuple14*x){return({struct _tuple3*_tmp699=_cycalloc(sizeof(*_tmp699));
_tmp699->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp69A=_cycalloc(sizeof(*
_tmp69A));_tmp69A->v=(*x).f1;_tmp69A;});_tmp699->f2=(*x).f2;_tmp699->f3=(*x).f3;
_tmp699;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp69B=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp69B=({
struct Cyc_List_List*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->hd=(void*)((
void*)atts->hd);_tmp69C->tl=_tmp69B;_tmp69C;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_Absyn_FnType_struct
_tmp69E;_tmp69E.tag=8;_tmp69E.f1=({struct Cyc_Absyn_FnInfo _tmp69F;_tmp69F.tvars=
fd->tvs;_tmp69F.effect=fd->effect;_tmp69F.ret_typ=(void*)((void*)fd->ret_type);
_tmp69F.args=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp69F.c_varargs=
fd->c_varargs;_tmp69F.cyc_varargs=fd->cyc_varargs;_tmp69F.rgn_po=fd->rgn_po;
_tmp69F.attributes=_tmp69B;_tmp69F;});_tmp69E;});_tmp69D;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple15{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple5*t){return(*t).f2;}static
struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t){return({struct _tuple5*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->f1=(*pr).f1;_tmp6A0->f2=t;_tmp6A0;});}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple17{
struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple3*y){return({struct _tuple17*_tmp6A1=_region_malloc(
rgn,sizeof(*_tmp6A1));_tmp6A1->f1=({struct _tuple16*_tmp6A2=_region_malloc(rgn,
sizeof(*_tmp6A2));_tmp6A2->f1=(*y).f1;_tmp6A2->f2=(*y).f2;_tmp6A2;});_tmp6A1->f2=(*
y).f3;_tmp6A1;});}static struct _tuple3*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp6A4;void*_tmp6A5;struct _tuple17 _tmp6A3=*w;_tmp6A4=_tmp6A3.f1;
_tmp6A5=_tmp6A3.f2;{struct Cyc_Core_Opt*_tmp6A7;struct Cyc_Absyn_Tqual _tmp6A8;
struct _tuple16 _tmp6A6=*_tmp6A4;_tmp6A7=_tmp6A6.f1;_tmp6A8=_tmp6A6.f2;return({
struct _tuple3*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->f1=_tmp6A7;_tmp6A9->f2=
_tmp6A8;_tmp6A9->f3=_tmp6A5;_tmp6A9;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp6AA=
_cycalloc(sizeof(*_tmp6AA));_tmp6AA->name=f->name;_tmp6AA->tq=f->tq;_tmp6AA->type=(
void*)t;_tmp6AA->width=f->width;_tmp6AA->attributes=f->attributes;_tmp6AA;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp6AB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp6AC;struct Cyc_Absyn_AggrInfo _tmp6AD;union Cyc_Absyn_AggrInfoU_union _tmp6AE;
struct Cyc_List_List*_tmp6AF;struct Cyc_Absyn_TunionInfo _tmp6B0;union Cyc_Absyn_TunionInfoU_union
_tmp6B1;struct Cyc_List_List*_tmp6B2;struct Cyc_Core_Opt*_tmp6B3;struct Cyc_Absyn_TunionFieldInfo
_tmp6B4;union Cyc_Absyn_TunionFieldInfoU_union _tmp6B5;struct Cyc_List_List*_tmp6B6;
struct _tuple2*_tmp6B7;struct Cyc_List_List*_tmp6B8;struct Cyc_Absyn_Typedefdecl*
_tmp6B9;void**_tmp6BA;struct Cyc_Absyn_ArrayInfo _tmp6BB;void*_tmp6BC;struct Cyc_Absyn_Tqual
_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BE;struct Cyc_Absyn_Conref*_tmp6BF;struct Cyc_Position_Segment*
_tmp6C0;struct Cyc_Absyn_PtrInfo _tmp6C1;void*_tmp6C2;struct Cyc_Absyn_Tqual _tmp6C3;
struct Cyc_Absyn_PtrAtts _tmp6C4;void*_tmp6C5;struct Cyc_Absyn_Conref*_tmp6C6;
struct Cyc_Absyn_Conref*_tmp6C7;struct Cyc_Absyn_Conref*_tmp6C8;struct Cyc_Absyn_FnInfo
_tmp6C9;struct Cyc_List_List*_tmp6CA;struct Cyc_Core_Opt*_tmp6CB;void*_tmp6CC;
struct Cyc_List_List*_tmp6CD;int _tmp6CE;struct Cyc_Absyn_VarargInfo*_tmp6CF;struct
Cyc_List_List*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_List_List*_tmp6D2;
void*_tmp6D3;struct Cyc_List_List*_tmp6D4;struct Cyc_Core_Opt*_tmp6D5;void*_tmp6D6;
void*_tmp6D7;void*_tmp6D8;void*_tmp6D9;void*_tmp6DA;void*_tmp6DB;void*_tmp6DC;
struct Cyc_List_List*_tmp6DD;_LL48D: if(_tmp6AB <= (void*)4)goto _LL4B1;if(*((int*)
_tmp6AB)!= 1)goto _LL48F;_tmp6AC=((struct Cyc_Absyn_VarType_struct*)_tmp6AB)->f1;
_LL48E: {struct _handler_cons _tmp6DE;_push_handler(& _tmp6DE);{int _tmp6E0=0;if(
setjmp(_tmp6DE.handler))_tmp6E0=1;if(!_tmp6E0){{void*_tmp6E1=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6AC);_npop_handler(0);return
_tmp6E1;};_pop_handler();}else{void*_tmp6DF=(void*)_exn_thrown;void*_tmp6E3=
_tmp6DF;_LL4C4: if(_tmp6E3 != Cyc_Core_Not_found)goto _LL4C6;_LL4C5: return t;_LL4C6:;
_LL4C7:(void)_throw(_tmp6E3);_LL4C3:;}}}_LL48F: if(*((int*)_tmp6AB)!= 10)goto
_LL491;_tmp6AD=((struct Cyc_Absyn_AggrType_struct*)_tmp6AB)->f1;_tmp6AE=_tmp6AD.aggr_info;
_tmp6AF=_tmp6AD.targs;_LL490: {struct Cyc_List_List*_tmp6E4=Cyc_Tcutil_substs(rgn,
inst,_tmp6AF);return _tmp6E4 == _tmp6AF?t:(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5[0]=({struct Cyc_Absyn_AggrType_struct
_tmp6E6;_tmp6E6.tag=10;_tmp6E6.f1=({struct Cyc_Absyn_AggrInfo _tmp6E7;_tmp6E7.aggr_info=
_tmp6AE;_tmp6E7.targs=_tmp6E4;_tmp6E7;});_tmp6E6;});_tmp6E5;});}_LL491: if(*((int*)
_tmp6AB)!= 2)goto _LL493;_tmp6B0=((struct Cyc_Absyn_TunionType_struct*)_tmp6AB)->f1;
_tmp6B1=_tmp6B0.tunion_info;_tmp6B2=_tmp6B0.targs;_tmp6B3=_tmp6B0.rgn;_LL492: {
struct Cyc_List_List*_tmp6E8=Cyc_Tcutil_substs(rgn,inst,_tmp6B2);struct Cyc_Core_Opt*
new_r;if((unsigned int)_tmp6B3){void*_tmp6E9=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6B3->v);if(_tmp6E9 == (void*)_tmp6B3->v)new_r=_tmp6B3;else{new_r=({
struct Cyc_Core_Opt*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA->v=(void*)_tmp6E9;
_tmp6EA;});}}else{new_r=_tmp6B3;}return _tmp6E8 == _tmp6B2  && new_r == _tmp6B3?t:(
void*)({struct Cyc_Absyn_TunionType_struct*_tmp6EB=_cycalloc(sizeof(*_tmp6EB));
_tmp6EB[0]=({struct Cyc_Absyn_TunionType_struct _tmp6EC;_tmp6EC.tag=2;_tmp6EC.f1=({
struct Cyc_Absyn_TunionInfo _tmp6ED;_tmp6ED.tunion_info=_tmp6B1;_tmp6ED.targs=
_tmp6E8;_tmp6ED.rgn=new_r;_tmp6ED;});_tmp6EC;});_tmp6EB;});}_LL493: if(*((int*)
_tmp6AB)!= 3)goto _LL495;_tmp6B4=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6AB)->f1;_tmp6B5=_tmp6B4.field_info;_tmp6B6=_tmp6B4.targs;_LL494: {struct Cyc_List_List*
_tmp6EE=Cyc_Tcutil_substs(rgn,inst,_tmp6B6);return _tmp6EE == _tmp6B6?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));
_tmp6EF[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp6F0;_tmp6F0.tag=3;_tmp6F0.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp6F1;_tmp6F1.field_info=_tmp6B5;_tmp6F1.targs=
_tmp6EE;_tmp6F1;});_tmp6F0;});_tmp6EF;});}_LL495: if(*((int*)_tmp6AB)!= 17)goto
_LL497;_tmp6B7=((struct Cyc_Absyn_TypedefType_struct*)_tmp6AB)->f1;_tmp6B8=((
struct Cyc_Absyn_TypedefType_struct*)_tmp6AB)->f2;_tmp6B9=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6AB)->f3;_tmp6BA=((struct Cyc_Absyn_TypedefType_struct*)_tmp6AB)->f4;_LL496: {
struct Cyc_List_List*_tmp6F2=Cyc_Tcutil_substs(rgn,inst,_tmp6B8);return _tmp6F2 == 
_tmp6B8?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp6F3=_cycalloc(sizeof(*
_tmp6F3));_tmp6F3[0]=({struct Cyc_Absyn_TypedefType_struct _tmp6F4;_tmp6F4.tag=17;
_tmp6F4.f1=_tmp6B7;_tmp6F4.f2=_tmp6F2;_tmp6F4.f3=_tmp6B9;_tmp6F4.f4=_tmp6BA;
_tmp6F4;});_tmp6F3;});}_LL497: if(*((int*)_tmp6AB)!= 7)goto _LL499;_tmp6BB=((
struct Cyc_Absyn_ArrayType_struct*)_tmp6AB)->f1;_tmp6BC=(void*)_tmp6BB.elt_type;
_tmp6BD=_tmp6BB.tq;_tmp6BE=_tmp6BB.num_elts;_tmp6BF=_tmp6BB.zero_term;_tmp6C0=
_tmp6BB.zt_loc;_LL498: {void*_tmp6F5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BC);
return _tmp6F5 == _tmp6BC?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp6F6=
_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Absyn_ArrayType_struct _tmp6F7;
_tmp6F7.tag=7;_tmp6F7.f1=({struct Cyc_Absyn_ArrayInfo _tmp6F8;_tmp6F8.elt_type=(
void*)_tmp6F5;_tmp6F8.tq=_tmp6BD;_tmp6F8.num_elts=_tmp6BE;_tmp6F8.zero_term=
_tmp6BF;_tmp6F8.zt_loc=_tmp6C0;_tmp6F8;});_tmp6F7;});_tmp6F6;});}_LL499: if(*((
int*)_tmp6AB)!= 4)goto _LL49B;_tmp6C1=((struct Cyc_Absyn_PointerType_struct*)
_tmp6AB)->f1;_tmp6C2=(void*)_tmp6C1.elt_typ;_tmp6C3=_tmp6C1.elt_tq;_tmp6C4=
_tmp6C1.ptr_atts;_tmp6C5=(void*)_tmp6C4.rgn;_tmp6C6=_tmp6C4.nullable;_tmp6C7=
_tmp6C4.bounds;_tmp6C8=_tmp6C4.zero_term;_LL49A: {void*_tmp6F9=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6C2);void*_tmp6FA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C5);struct
Cyc_Absyn_Conref*_tmp6FB=_tmp6C7;{union Cyc_Absyn_Constraint_union _tmp6FC=(Cyc_Absyn_compress_conref(
_tmp6C7))->v;void*_tmp6FD;void*_tmp6FE;_LL4C9: if((_tmp6FC.Eq_constr).tag != 0)
goto _LL4CB;_tmp6FD=(_tmp6FC.Eq_constr).f1;if(_tmp6FD <= (void*)2)goto _LL4CB;if(*((
int*)_tmp6FD)!= 1)goto _LL4CB;_tmp6FE=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp6FD)->f1;_LL4CA: {void*_tmp6FF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FE);if(
_tmp6FE != _tmp6FF)_tmp6FB=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*
_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp701;_tmp701.tag=1;_tmp701.f1=(void*)_tmp6FF;_tmp701;});_tmp700;}));goto
_LL4C8;}_LL4CB:;_LL4CC: goto _LL4C8;_LL4C8:;}if((_tmp6F9 == _tmp6C2  && _tmp6FA == 
_tmp6C5) && _tmp6FB == _tmp6C7)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702[0]=({struct Cyc_Absyn_PointerType_struct
_tmp703;_tmp703.tag=4;_tmp703.f1=({struct Cyc_Absyn_PtrInfo _tmp704;_tmp704.elt_typ=(
void*)_tmp6F9;_tmp704.elt_tq=_tmp6C3;_tmp704.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp705;_tmp705.rgn=(void*)_tmp6FA;_tmp705.nullable=_tmp6C6;_tmp705.bounds=
_tmp6FB;_tmp705.zero_term=_tmp6C8;_tmp705.ptrloc=0;_tmp705;});_tmp704;});_tmp703;});
_tmp702;});}_LL49B: if(*((int*)_tmp6AB)!= 8)goto _LL49D;_tmp6C9=((struct Cyc_Absyn_FnType_struct*)
_tmp6AB)->f1;_tmp6CA=_tmp6C9.tvars;_tmp6CB=_tmp6C9.effect;_tmp6CC=(void*)_tmp6C9.ret_typ;
_tmp6CD=_tmp6C9.args;_tmp6CE=_tmp6C9.c_varargs;_tmp6CF=_tmp6C9.cyc_varargs;
_tmp6D0=_tmp6C9.rgn_po;_tmp6D1=_tmp6C9.attributes;_LL49C:{struct Cyc_List_List*
_tmp706=_tmp6CA;for(0;_tmp706 != 0;_tmp706=_tmp706->tl){inst=({struct Cyc_List_List*
_tmp707=_region_malloc(rgn,sizeof(*_tmp707));_tmp707->hd=({struct _tuple8*_tmp708=
_region_malloc(rgn,sizeof(*_tmp708));_tmp708->f1=(struct Cyc_Absyn_Tvar*)_tmp706->hd;
_tmp708->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp709=_cycalloc(sizeof(*
_tmp709));_tmp709[0]=({struct Cyc_Absyn_VarType_struct _tmp70A;_tmp70A.tag=1;
_tmp70A.f1=(struct Cyc_Absyn_Tvar*)_tmp706->hd;_tmp70A;});_tmp709;});_tmp708;});
_tmp707->tl=inst;_tmp707;});}}{struct Cyc_List_List*_tmp70C;struct Cyc_List_List*
_tmp70D;struct _tuple1 _tmp70B=((struct _tuple1(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple3*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp6CD));_tmp70C=_tmp70B.f1;_tmp70D=_tmp70B.f2;{struct Cyc_List_List*_tmp70E=
Cyc_Tcutil_substs(rgn,inst,_tmp70D);struct Cyc_List_List*_tmp70F=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp70C,_tmp70E));struct Cyc_Core_Opt*
eff2;if(_tmp6CB == 0)eff2=0;else{void*_tmp710=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6CB->v);if(_tmp710 == (void*)_tmp6CB->v)eff2=_tmp6CB;else{eff2=({struct
Cyc_Core_Opt*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->v=(void*)_tmp710;
_tmp711;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6CF == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp713;struct Cyc_Absyn_Tqual _tmp714;void*
_tmp715;int _tmp716;struct Cyc_Absyn_VarargInfo _tmp712=*_tmp6CF;_tmp713=_tmp712.name;
_tmp714=_tmp712.tq;_tmp715=(void*)_tmp712.type;_tmp716=_tmp712.inject;{void*
_tmp717=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp715);if(_tmp717 == _tmp715)
cyc_varargs2=_tmp6CF;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp718=
_cycalloc(sizeof(*_tmp718));_tmp718->name=_tmp713;_tmp718->tq=_tmp714;_tmp718->type=(
void*)_tmp717;_tmp718->inject=_tmp716;_tmp718;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp71A;struct Cyc_List_List*_tmp71B;struct _tuple1 _tmp719=Cyc_List_rsplit(
rgn,rgn,_tmp6D0);_tmp71A=_tmp719.f1;_tmp71B=_tmp719.f2;{struct Cyc_List_List*
_tmp71C=Cyc_Tcutil_substs(rgn,inst,_tmp71A);struct Cyc_List_List*_tmp71D=Cyc_Tcutil_substs(
rgn,inst,_tmp71B);if(_tmp71C == _tmp71A  && _tmp71D == _tmp71B)rgn_po2=_tmp6D0;
else{rgn_po2=Cyc_List_zip(_tmp71C,_tmp71D);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_Absyn_FnType_struct
_tmp71F;_tmp71F.tag=8;_tmp71F.f1=({struct Cyc_Absyn_FnInfo _tmp720;_tmp720.tvars=
_tmp6CA;_tmp720.effect=eff2;_tmp720.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp6CC);_tmp720.args=_tmp70F;_tmp720.c_varargs=_tmp6CE;_tmp720.cyc_varargs=
cyc_varargs2;_tmp720.rgn_po=rgn_po2;_tmp720.attributes=_tmp6D1;_tmp720;});
_tmp71F;});_tmp71E;});}}}}}_LL49D: if(*((int*)_tmp6AB)!= 9)goto _LL49F;_tmp6D2=((
struct Cyc_Absyn_TupleType_struct*)_tmp6AB)->f1;_LL49E: {struct Cyc_List_List*
_tmp721=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6D2);struct Cyc_List_List*
_tmp722=Cyc_Tcutil_substs(rgn,inst,_tmp721);if(_tmp722 == _tmp721)return t;{struct
Cyc_List_List*_tmp723=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6D2,_tmp722);return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp724=
_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Absyn_TupleType_struct _tmp725;
_tmp725.tag=9;_tmp725.f1=_tmp723;_tmp725;});_tmp724;});}}_LL49F: if(*((int*)
_tmp6AB)!= 11)goto _LL4A1;_tmp6D3=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6AB)->f1;_tmp6D4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6AB)->f2;_LL4A0: {
struct Cyc_List_List*_tmp726=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6D4);struct Cyc_List_List*_tmp727=Cyc_Tcutil_substs(rgn,inst,_tmp726);if(
_tmp727 == _tmp726)return t;{struct Cyc_List_List*_tmp728=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6D4,
_tmp727);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp729=_cycalloc(
sizeof(*_tmp729));_tmp729[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp72A;
_tmp72A.tag=11;_tmp72A.f1=(void*)_tmp6D3;_tmp72A.f2=_tmp728;_tmp72A;});_tmp729;});}}
_LL4A1: if(*((int*)_tmp6AB)!= 0)goto _LL4A3;_tmp6D5=((struct Cyc_Absyn_Evar_struct*)
_tmp6AB)->f2;_LL4A2: if(_tmp6D5 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6D5->v);else{return t;}_LL4A3: if(*((int*)_tmp6AB)!= 15)goto _LL4A5;_tmp6D6=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp6AB)->f1;_LL4A4: {void*_tmp72B=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D6);return _tmp72B == _tmp6D6?t:(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp72D;_tmp72D.tag=15;_tmp72D.f1=(void*)
_tmp72B;_tmp72D;});_tmp72C;});}_LL4A5: if(*((int*)_tmp6AB)!= 16)goto _LL4A7;
_tmp6D7=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp6AB)->f1;_tmp6D8=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp6AB)->f2;_LL4A6: {void*_tmp72E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6D7);void*_tmp72F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D8);return
_tmp72E == _tmp6D7  && _tmp72F == _tmp6D8?t:(void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp731;_tmp731.tag=16;_tmp731.f1=(void*)_tmp72E;_tmp731.f2=(void*)_tmp72F;
_tmp731;});_tmp730;});}_LL4A7: if(*((int*)_tmp6AB)!= 14)goto _LL4A9;_tmp6D9=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp6AB)->f1;_LL4A8: {void*_tmp732=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6D9);return _tmp732 == _tmp6D9?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp734;_tmp734.tag=14;_tmp734.f1=(void*)_tmp732;_tmp734;});_tmp733;});}_LL4A9:
if(*((int*)_tmp6AB)!= 18)goto _LL4AB;_tmp6DA=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6AB)->f1;_LL4AA: {void*_tmp735=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DA);
return _tmp735 == _tmp6DA?t:(void*)({struct Cyc_Absyn_TagType_struct*_tmp736=
_cycalloc(sizeof(*_tmp736));_tmp736[0]=({struct Cyc_Absyn_TagType_struct _tmp737;
_tmp737.tag=18;_tmp737.f1=(void*)_tmp735;_tmp737;});_tmp736;});}_LL4AB: if(*((int*)
_tmp6AB)!= 19)goto _LL4AD;_LL4AC: goto _LL4AE;_LL4AD: if(*((int*)_tmp6AB)!= 12)goto
_LL4AF;_LL4AE: goto _LL4B0;_LL4AF: if(*((int*)_tmp6AB)!= 13)goto _LL4B1;_LL4B0: goto
_LL4B2;_LL4B1: if((int)_tmp6AB != 0)goto _LL4B3;_LL4B2: goto _LL4B4;_LL4B3: if(_tmp6AB
<= (void*)4)goto _LL4B5;if(*((int*)_tmp6AB)!= 5)goto _LL4B5;_LL4B4: goto _LL4B6;
_LL4B5: if((int)_tmp6AB != 1)goto _LL4B7;_LL4B6: goto _LL4B8;_LL4B7: if(_tmp6AB <= (
void*)4)goto _LL4B9;if(*((int*)_tmp6AB)!= 6)goto _LL4B9;_LL4B8: goto _LL4BA;_LL4B9:
if((int)_tmp6AB != 3)goto _LL4BB;_LL4BA: goto _LL4BC;_LL4BB: if((int)_tmp6AB != 2)goto
_LL4BD;_LL4BC: return t;_LL4BD: if(_tmp6AB <= (void*)4)goto _LL4BF;if(*((int*)_tmp6AB)
!= 22)goto _LL4BF;_tmp6DB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6AB)->f1;
_LL4BE: {void*_tmp738=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DB);return _tmp738 == 
_tmp6DB?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp739=_cycalloc(sizeof(*
_tmp739));_tmp739[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp73A;_tmp73A.tag=22;
_tmp73A.f1=(void*)_tmp738;_tmp73A;});_tmp739;});}_LL4BF: if(_tmp6AB <= (void*)4)
goto _LL4C1;if(*((int*)_tmp6AB)!= 20)goto _LL4C1;_tmp6DC=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6AB)->f1;_LL4C0: {void*_tmp73B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DC);
return _tmp73B == _tmp6DC?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp73C=
_cycalloc(sizeof(*_tmp73C));_tmp73C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp73D;
_tmp73D.tag=20;_tmp73D.f1=(void*)_tmp73B;_tmp73D;});_tmp73C;});}_LL4C1: if(
_tmp6AB <= (void*)4)goto _LL48C;if(*((int*)_tmp6AB)!= 21)goto _LL48C;_tmp6DD=((
struct Cyc_Absyn_JoinEff_struct*)_tmp6AB)->f1;_LL4C2: {struct Cyc_List_List*
_tmp73E=Cyc_Tcutil_substs(rgn,inst,_tmp6DD);return _tmp73E == _tmp6DD?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp740;_tmp740.tag=21;_tmp740.f1=_tmp73E;_tmp740;});
_tmp73F;});}_LL48C:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp741=(void*)ts->hd;struct Cyc_List_List*_tmp742=ts->tl;void*
_tmp743=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp741);struct Cyc_List_List*_tmp744=Cyc_Tcutil_substs(
rgn,inst,_tmp742);if(_tmp741 == _tmp743  && _tmp742 == _tmp744)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp745=
_cycalloc(sizeof(*_tmp745));_tmp745->hd=(void*)_tmp743;_tmp745->tl=_tmp744;
_tmp745;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple8*_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->f1=tv;_tmp746->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp747=_cycalloc(sizeof(*_tmp747));
_tmp747->v=(void*)k;_tmp747;}),({struct Cyc_Core_Opt*_tmp748=_cycalloc(sizeof(*
_tmp748));_tmp748->v=s;_tmp748;}));_tmp746;});}struct _tuple8*Cyc_Tcutil_r_make_inst_var(
struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){struct _tuple9 _tmp74A;struct Cyc_List_List*
_tmp74B;struct _RegionHandle*_tmp74C;struct _tuple9*_tmp749=env;_tmp74A=*_tmp749;
_tmp74B=_tmp74A.f1;_tmp74C=_tmp74A.f2;{void*k=Cyc_Tcutil_tvar_kind(tv);return({
struct _tuple8*_tmp74D=_region_malloc(_tmp74C,sizeof(*_tmp74D));_tmp74D->f1=tv;
_tmp74D->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp74E=_cycalloc(sizeof(*
_tmp74E));_tmp74E->v=(void*)k;_tmp74E;}),({struct Cyc_Core_Opt*_tmp74F=_cycalloc(
sizeof(*_tmp74F));_tmp74F->v=_tmp74B;_tmp74F;}));_tmp74D;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp754;_tmp754.tag=0;_tmp754.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k2));{
struct Cyc_String_pa_struct _tmp753;_tmp753.tag=0;_tmp753.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k1));{
struct Cyc_String_pa_struct _tmp752;_tmp752.tag=0;_tmp752.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*tv->name);{void*_tmp750[3]={& _tmp752,&
_tmp753,& _tmp754};Cyc_Tcutil_terr(loc,({const char*_tmp751="type variable %s is used with inconsistent kinds %s and %s";
_tag_dynforward(_tmp751,sizeof(char),_get_zero_arr_size(_tmp751,59));}),
_tag_dynforward(_tmp750,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp755[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp756="same type variable has different identity!";_tag_dynforward(
_tmp756,sizeof(char),_get_zero_arr_size(_tmp756,43));}),_tag_dynforward(_tmp755,
sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({
struct Cyc_List_List*_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757->hd=tv;_tmp757->tl=
tvs;_tmp757;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp758[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp759="fast_add_free_tvar: bad identity in tv";_tag_dynforward(
_tmp759,sizeof(char),_get_zero_arr_size(_tmp759,39));}),_tag_dynforward(_tmp758,
sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp75A=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp75A->identity
== 0)({void*_tmp75B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp75C="fast_add_free_tvar: bad identity in tvs2";
_tag_dynforward(_tmp75C,sizeof(char),_get_zero_arr_size(_tmp75C,41));}),
_tag_dynforward(_tmp75B,sizeof(void*),0));});if(*((int*)_check_null(_tmp75A->identity))
== *((int*)_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*
_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->hd=tv;_tmp75D->tl=tvs;_tmp75D;});}
struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(
tv->identity == 0)({void*_tmp75E[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp75F="fast_add_free_tvar_bool: bad identity in tv";
_tag_dynforward(_tmp75F,sizeof(char),_get_zero_arr_size(_tmp75F,44));}),
_tag_dynforward(_tmp75E,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;
tvs2 != 0;tvs2=tvs2->tl){struct _tuple18 _tmp761;struct Cyc_Absyn_Tvar*_tmp762;int
_tmp763;int*_tmp764;struct _tuple18*_tmp760=(struct _tuple18*)tvs2->hd;_tmp761=*
_tmp760;_tmp762=_tmp761.f1;_tmp763=_tmp761.f2;_tmp764=(int*)&(*_tmp760).f2;if(
_tmp762->identity == 0)({void*_tmp765[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp766="fast_add_free_tvar_bool: bad identity in tvs2";
_tag_dynforward(_tmp766,sizeof(char),_get_zero_arr_size(_tmp766,46));}),
_tag_dynforward(_tmp765,sizeof(void*),0));});if(*((int*)_check_null(_tmp762->identity))
== *((int*)_check_null(tv->identity))){*_tmp764=*_tmp764  || b;return tvs;}}}
return({struct Cyc_List_List*_tmp767=_region_malloc(r,sizeof(*_tmp767));_tmp767->hd=({
struct _tuple18*_tmp768=_region_malloc(r,sizeof(*_tmp768));_tmp768->f1=tv;_tmp768->f2=
b;_tmp768;});_tmp767->tl=tvs;_tmp767;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_String_pa_struct
_tmp76B;_tmp76B.tag=0;_tmp76B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string(tv));{void*_tmp769[1]={& _tmp76B};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp76A="bound tvar id for %s is NULL";_tag_dynforward(_tmp76A,sizeof(char),
_get_zero_arr_size(_tmp76A,29));}),_tag_dynforward(_tmp769,sizeof(void*),1));}});
return({struct Cyc_List_List*_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C->hd=tv;
_tmp76C->tl=tvs;_tmp76C;});}struct _tuple19{void*f1;int f2;};static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp76D=Cyc_Tcutil_compress(e);int _tmp76E;_LL4CE: if(_tmp76D <= (void*)4)
goto _LL4D0;if(*((int*)_tmp76D)!= 0)goto _LL4D0;_tmp76E=((struct Cyc_Absyn_Evar_struct*)
_tmp76D)->f3;_LL4CF:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple19 _tmp770;void*_tmp771;int _tmp772;int*_tmp773;struct _tuple19*_tmp76F=(
struct _tuple19*)es2->hd;_tmp770=*_tmp76F;_tmp771=_tmp770.f1;_tmp772=_tmp770.f2;
_tmp773=(int*)&(*_tmp76F).f2;{void*_tmp774=Cyc_Tcutil_compress(_tmp771);int
_tmp775;_LL4D3: if(_tmp774 <= (void*)4)goto _LL4D5;if(*((int*)_tmp774)!= 0)goto
_LL4D5;_tmp775=((struct Cyc_Absyn_Evar_struct*)_tmp774)->f3;_LL4D4: if(_tmp76E == 
_tmp775){if(b != *_tmp773)*_tmp773=1;return es;}goto _LL4D2;_LL4D5:;_LL4D6: goto
_LL4D2;_LL4D2:;}}}return({struct Cyc_List_List*_tmp776=_region_malloc(r,sizeof(*
_tmp776));_tmp776->hd=({struct _tuple19*_tmp777=_region_malloc(r,sizeof(*_tmp777));
_tmp777->f1=e;_tmp777->f2=b;_tmp777;});_tmp776->tl=es;_tmp776;});_LL4D0:;_LL4D1:
return es;_LL4CD:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct
_RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*
r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b
!= 0;b=b->tl){if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))
== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;
break;}}}if(!present)r=({struct Cyc_List_List*_tmp778=_region_malloc(rgn,sizeof(*
_tmp778));_tmp778->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp778->tl=r;_tmp778;});}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp77A;int _tmp77B;struct
_tuple18 _tmp779=*((struct _tuple18*)tvs->hd);_tmp77A=_tmp779.f1;_tmp77B=_tmp779.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(_tmp77A->identity))== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){
present=1;break;}}}if(!present)res=({struct Cyc_List_List*_tmp77C=_region_malloc(
r,sizeof(*_tmp77C));_tmp77C->hd=(struct _tuple18*)tvs->hd;_tmp77C->tl=res;_tmp77C;});}}
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dynforward_ptr*
fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
width))({struct Cyc_String_pa_struct _tmp77F;_tmp77F.tag=0;_tmp77F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp77D[1]={& _tmp77F};Cyc_Tcutil_terr(
loc,({const char*_tmp77E="bitfield %s does not have constant width";
_tag_dynforward(_tmp77E,sizeof(char),_get_zero_arr_size(_tmp77E,41));}),
_tag_dynforward(_tmp77D,sizeof(void*),1));}});else{unsigned int _tmp781;int
_tmp782;struct _tuple7 _tmp780=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
width);_tmp781=_tmp780.f1;_tmp782=_tmp780.f2;if(!_tmp782)({void*_tmp783[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp784="bitfield width cannot use sizeof or offsetof";
_tag_dynforward(_tmp784,sizeof(char),_get_zero_arr_size(_tmp784,45));}),
_tag_dynforward(_tmp783,sizeof(void*),0));});w=_tmp781;}{void*_tmp785=Cyc_Tcutil_compress(
field_typ);void*_tmp786;_LL4D8: if(_tmp785 <= (void*)4)goto _LL4DA;if(*((int*)
_tmp785)!= 5)goto _LL4DA;_tmp786=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp785)->f2;
_LL4D9:{void*_tmp787=_tmp786;_LL4DD: if((int)_tmp787 != 0)goto _LL4DF;_LL4DE: if(w > 
8)({void*_tmp788[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp789="bitfield larger than type";
_tag_dynforward(_tmp789,sizeof(char),_get_zero_arr_size(_tmp789,26));}),
_tag_dynforward(_tmp788,sizeof(void*),0));});goto _LL4DC;_LL4DF: if((int)_tmp787 != 
1)goto _LL4E1;_LL4E0: if(w > 16)({void*_tmp78A[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp78B="bitfield larger than type";_tag_dynforward(_tmp78B,sizeof(char),
_get_zero_arr_size(_tmp78B,26));}),_tag_dynforward(_tmp78A,sizeof(void*),0));});
goto _LL4DC;_LL4E1: if((int)_tmp787 != 3)goto _LL4E3;_LL4E2: goto _LL4E4;_LL4E3: if((
int)_tmp787 != 2)goto _LL4E5;_LL4E4: if(w > 32)({void*_tmp78C[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp78D="bitfield larger than type";_tag_dynforward(_tmp78D,
sizeof(char),_get_zero_arr_size(_tmp78D,26));}),_tag_dynforward(_tmp78C,sizeof(
void*),0));});goto _LL4DC;_LL4E5: if((int)_tmp787 != 4)goto _LL4DC;_LL4E6: if(w > 64)({
void*_tmp78E[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp78F="bitfield larger than type";
_tag_dynforward(_tmp78F,sizeof(char),_get_zero_arr_size(_tmp78F,26));}),
_tag_dynforward(_tmp78E,sizeof(void*),0));});goto _LL4DC;_LL4DC:;}goto _LL4D7;
_LL4DA:;_LL4DB:({struct Cyc_String_pa_struct _tmp793;_tmp793.tag=0;_tmp793.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(field_typ));{
struct Cyc_String_pa_struct _tmp792;_tmp792.tag=0;_tmp792.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp790[2]={& _tmp792,& _tmp793};
Cyc_Tcutil_terr(loc,({const char*_tmp791="bitfield %s must have integral type but has type %s";
_tag_dynforward(_tmp791,sizeof(char),_get_zero_arr_size(_tmp791,52));}),
_tag_dynforward(_tmp790,sizeof(void*),2));}}});goto _LL4D7;_LL4D7:;}}}static void
Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp794=(void*)
atts->hd;_LL4E8: if((int)_tmp794 != 5)goto _LL4EA;_LL4E9: continue;_LL4EA: if(_tmp794
<= (void*)17)goto _LL4EC;if(*((int*)_tmp794)!= 1)goto _LL4EC;_LL4EB: continue;
_LL4EC:;_LL4ED:({struct Cyc_String_pa_struct _tmp798;_tmp798.tag=0;_tmp798.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_String_pa_struct
_tmp797;_tmp797.tag=0;_tmp797.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp795[2]={& _tmp797,& _tmp798};
Cyc_Tcutil_terr(loc,({const char*_tmp796="bad attribute %s on member %s";
_tag_dynforward(_tmp796,sizeof(char),_get_zero_arr_size(_tmp796,30));}),
_tag_dynforward(_tmp795,sizeof(void*),2));}}});_LL4E7:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp799=t;struct
Cyc_Absyn_Typedefdecl*_tmp79A;void**_tmp79B;_LL4EF: if(_tmp799 <= (void*)4)goto
_LL4F1;if(*((int*)_tmp799)!= 17)goto _LL4F1;_tmp79A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp799)->f3;_tmp79B=((struct Cyc_Absyn_TypedefType_struct*)_tmp799)->f4;_LL4F0:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp79A))->tq).real_const  || (
_tmp79A->tq).print_const){if(declared_const)({void*_tmp79C[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp79D="extra const";_tag_dynforward(_tmp79D,sizeof(char),
_get_zero_arr_size(_tmp79D,12));}),_tag_dynforward(_tmp79C,sizeof(void*),0));});
return 1;}if((unsigned int)_tmp79B)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp79B);else{return declared_const;}_LL4F1:;_LL4F2: return
declared_const;_LL4EE:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t,int put_in_effect){{void*_tmp79E=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp79F;struct Cyc_Core_Opt**_tmp7A0;struct Cyc_Core_Opt*
_tmp7A1;struct Cyc_Core_Opt**_tmp7A2;struct Cyc_Absyn_Tvar*_tmp7A3;struct Cyc_List_List*
_tmp7A4;struct _tuple2*_tmp7A5;struct Cyc_Absyn_Enumdecl*_tmp7A6;struct Cyc_Absyn_Enumdecl**
_tmp7A7;struct Cyc_Absyn_TunionInfo _tmp7A8;union Cyc_Absyn_TunionInfoU_union
_tmp7A9;union Cyc_Absyn_TunionInfoU_union*_tmp7AA;struct Cyc_List_List*_tmp7AB;
struct Cyc_List_List**_tmp7AC;struct Cyc_Core_Opt*_tmp7AD;struct Cyc_Core_Opt**
_tmp7AE;struct Cyc_Absyn_TunionFieldInfo _tmp7AF;union Cyc_Absyn_TunionFieldInfoU_union
_tmp7B0;union Cyc_Absyn_TunionFieldInfoU_union*_tmp7B1;struct Cyc_List_List*
_tmp7B2;struct Cyc_Absyn_PtrInfo _tmp7B3;void*_tmp7B4;struct Cyc_Absyn_Tqual _tmp7B5;
struct Cyc_Absyn_Tqual*_tmp7B6;struct Cyc_Absyn_PtrAtts _tmp7B7;void*_tmp7B8;struct
Cyc_Absyn_Conref*_tmp7B9;struct Cyc_Absyn_Conref*_tmp7BA;struct Cyc_Absyn_Conref*
_tmp7BB;void*_tmp7BC;void*_tmp7BD;struct Cyc_Absyn_ArrayInfo _tmp7BE;void*_tmp7BF;
struct Cyc_Absyn_Tqual _tmp7C0;struct Cyc_Absyn_Tqual*_tmp7C1;struct Cyc_Absyn_Exp*
_tmp7C2;struct Cyc_Absyn_Exp**_tmp7C3;struct Cyc_Absyn_Conref*_tmp7C4;struct Cyc_Position_Segment*
_tmp7C5;struct Cyc_Absyn_FnInfo _tmp7C6;struct Cyc_List_List*_tmp7C7;struct Cyc_List_List**
_tmp7C8;struct Cyc_Core_Opt*_tmp7C9;struct Cyc_Core_Opt**_tmp7CA;void*_tmp7CB;
struct Cyc_List_List*_tmp7CC;int _tmp7CD;struct Cyc_Absyn_VarargInfo*_tmp7CE;struct
Cyc_List_List*_tmp7CF;struct Cyc_List_List*_tmp7D0;struct Cyc_List_List*_tmp7D1;
void*_tmp7D2;struct Cyc_List_List*_tmp7D3;struct Cyc_Absyn_AggrInfo _tmp7D4;union
Cyc_Absyn_AggrInfoU_union _tmp7D5;union Cyc_Absyn_AggrInfoU_union*_tmp7D6;struct
Cyc_List_List*_tmp7D7;struct Cyc_List_List**_tmp7D8;struct _tuple2*_tmp7D9;struct
Cyc_List_List*_tmp7DA;struct Cyc_List_List**_tmp7DB;struct Cyc_Absyn_Typedefdecl*
_tmp7DC;struct Cyc_Absyn_Typedefdecl**_tmp7DD;void**_tmp7DE;void***_tmp7DF;void*
_tmp7E0;void*_tmp7E1;void*_tmp7E2;void*_tmp7E3;void*_tmp7E4;struct Cyc_List_List*
_tmp7E5;_LL4F4: if((int)_tmp79E != 0)goto _LL4F6;_LL4F5: goto _LL4F3;_LL4F6: if(
_tmp79E <= (void*)4)goto _LL50A;if(*((int*)_tmp79E)!= 0)goto _LL4F8;_tmp79F=((
struct Cyc_Absyn_Evar_struct*)_tmp79E)->f1;_tmp7A0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp79E)->f1;_tmp7A1=((struct Cyc_Absyn_Evar_struct*)
_tmp79E)->f2;_tmp7A2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp79E)->f2;_LL4F7: if(*_tmp7A0 == 0)*_tmp7A0=({struct Cyc_Core_Opt*_tmp7E6=
_cycalloc(sizeof(*_tmp7E6));_tmp7E6->v=(void*)expected_kind;_tmp7E6;});if((
cvtenv.fn_result  && cvtenv.generalize_evars) && Cyc_Tcutil_is_region_kind(
expected_kind)){if(expected_kind == (void*)4)*_tmp7A2=({struct Cyc_Core_Opt*
_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7->v=(void*)((void*)3);_tmp7E7;});
else{*_tmp7A2=({struct Cyc_Core_Opt*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8->v=(
void*)((void*)2);_tmp7E8;});}}else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Tvar*
_tmp7E9=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7ED=
_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7EE;
_tmp7EE.tag=2;_tmp7EE.f1=0;_tmp7EE.f2=(void*)expected_kind;_tmp7EE;});_tmp7ED;}));*
_tmp7A2=({struct Cyc_Core_Opt*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA->v=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7EB=_cycalloc(sizeof(*_tmp7EB));
_tmp7EB[0]=({struct Cyc_Absyn_VarType_struct _tmp7EC;_tmp7EC.tag=1;_tmp7EC.f1=
_tmp7E9;_tmp7EC;});_tmp7EB;}));_tmp7EA;});_tmp7A3=_tmp7E9;goto _LL4F9;}else{
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL4F3;_LL4F8: if(*((int*)_tmp79E)!= 1)goto _LL4FA;_tmp7A3=((
struct Cyc_Absyn_VarType_struct*)_tmp79E)->f1;_LL4F9:{void*_tmp7EF=Cyc_Absyn_compress_kb((
void*)_tmp7A3->kind);struct Cyc_Core_Opt*_tmp7F0;struct Cyc_Core_Opt**_tmp7F1;
_LL52B: if(*((int*)_tmp7EF)!= 1)goto _LL52D;_tmp7F0=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7EF)->f1;_tmp7F1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7EF)->f1;_LL52C:*_tmp7F1=({struct Cyc_Core_Opt*_tmp7F2=_cycalloc(sizeof(*
_tmp7F2));_tmp7F2->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7F3=
_cycalloc(sizeof(*_tmp7F3));_tmp7F3[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7F4;
_tmp7F4.tag=2;_tmp7F4.f1=0;_tmp7F4.f2=(void*)expected_kind;_tmp7F4;});_tmp7F3;}));
_tmp7F2;});goto _LL52A;_LL52D:;_LL52E: goto _LL52A;_LL52A:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp7A3);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp7A3,put_in_effect);{void*_tmp7F5=Cyc_Absyn_compress_kb((
void*)_tmp7A3->kind);struct Cyc_Core_Opt*_tmp7F6;struct Cyc_Core_Opt**_tmp7F7;void*
_tmp7F8;_LL530: if(*((int*)_tmp7F5)!= 2)goto _LL532;_tmp7F6=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7F5)->f1;_tmp7F7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7F5)->f1;_tmp7F8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7F5)->f2;
_LL531: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7F8))*_tmp7F7=({struct Cyc_Core_Opt*
_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7FB;_tmp7FB.tag=2;_tmp7FB.f1=0;_tmp7FB.f2=(void*)expected_kind;_tmp7FB;});
_tmp7FA;}));_tmp7F9;});goto _LL52F;_LL532:;_LL533: goto _LL52F;_LL52F:;}goto _LL4F3;
_LL4FA: if(*((int*)_tmp79E)!= 13)goto _LL4FC;_tmp7A4=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp79E)->f1;_LL4FB: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct
_RegionHandle _tmp7FC=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmp7FC;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;for(0;_tmp7A4 != 0;_tmp7A4=_tmp7A4->tl){struct Cyc_Absyn_Enumfield*
_tmp7FD=(struct Cyc_Absyn_Enumfield*)_tmp7A4->hd;if(((int(*)(int(*compare)(struct
_dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l,struct
_dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7FD->name).f2))({
struct Cyc_String_pa_struct _tmp800;_tmp800.tag=0;_tmp800.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*(*_tmp7FD->name).f2);{void*_tmp7FE[1]={&
_tmp800};Cyc_Tcutil_terr(_tmp7FD->loc,({const char*_tmp7FF="duplicate enum field name %s";
_tag_dynforward(_tmp7FF,sizeof(char),_get_zero_arr_size(_tmp7FF,29));}),
_tag_dynforward(_tmp7FE,sizeof(void*),1));}});else{prev_fields=({struct Cyc_List_List*
_tmp801=_region_malloc(uprev_rgn,sizeof(*_tmp801));_tmp801->hd=(*_tmp7FD->name).f2;
_tmp801->tl=prev_fields;_tmp801;});}if(_tmp7FD->tag == 0)_tmp7FD->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp7FD->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp7FD->tag)))({struct Cyc_String_pa_struct
_tmp804;_tmp804.tag=0;_tmp804.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
_tmp7FD->name).f2);{void*_tmp802[1]={& _tmp804};Cyc_Tcutil_terr(loc,({const char*
_tmp803="enum field %s: expression is not constant";_tag_dynforward(_tmp803,
sizeof(char),_get_zero_arr_size(_tmp803,42));}),_tag_dynforward(_tmp802,sizeof(
void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp7FD->tag))).f1;tag_count=t1 + 1;(*_tmp7FD->name).f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp805;(_tmp805.Abs_n).tag=2;(_tmp805.Abs_n).f1=te->ns;
_tmp805;});ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k,struct _tuple19*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7FD->name).f2,(
struct _tuple19*)({struct _tuple19*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->f1=(
void*)({struct Cyc_Tcenv_AnonEnumRes_struct*_tmp807=_cycalloc(sizeof(*_tmp807));
_tmp807[0]=({struct Cyc_Tcenv_AnonEnumRes_struct _tmp808;_tmp808.tag=4;_tmp808.f1=(
void*)t;_tmp808.f2=_tmp7FD;_tmp808;});_tmp807;});_tmp806->f2=1;_tmp806;}));}}};
_pop_region(uprev_rgn);}goto _LL4F3;}_LL4FC: if(*((int*)_tmp79E)!= 12)goto _LL4FE;
_tmp7A5=((struct Cyc_Absyn_EnumType_struct*)_tmp79E)->f1;_tmp7A6=((struct Cyc_Absyn_EnumType_struct*)
_tmp79E)->f2;_tmp7A7=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp79E)->f2;_LL4FD: if(*_tmp7A7 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp7A7))->fields == 0){struct _handler_cons _tmp809;_push_handler(& _tmp809);{int
_tmp80B=0;if(setjmp(_tmp809.handler))_tmp80B=1;if(!_tmp80B){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7A5);*_tmp7A7=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp80A=(void*)_exn_thrown;void*_tmp80D=_tmp80A;
_LL535: if(_tmp80D != Cyc_Dict_Absent)goto _LL537;_LL536: {struct Cyc_Tcenv_Genv*
_tmp80E=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp80F=({struct Cyc_Absyn_Enumdecl*
_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810->sc=(void*)((void*)3);_tmp810->name=
_tmp7A5;_tmp810->fields=0;_tmp810;});Cyc_Tc_tcEnumdecl(te,_tmp80E,loc,_tmp80F);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7A5);*_tmp7A7=(
struct Cyc_Absyn_Enumdecl*)*ed;goto _LL534;}}_LL537:;_LL538:(void)_throw(_tmp80D);
_LL534:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp7A7);*_tmp7A5=(ed->name)[_check_known_subscript_notnull(1,0)];goto _LL4F3;}
_LL4FE: if(*((int*)_tmp79E)!= 2)goto _LL500;_tmp7A8=((struct Cyc_Absyn_TunionType_struct*)
_tmp79E)->f1;_tmp7A9=_tmp7A8.tunion_info;_tmp7AA=(union Cyc_Absyn_TunionInfoU_union*)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp79E)->f1).tunion_info;_tmp7AB=_tmp7A8.targs;
_tmp7AC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp79E)->f1).targs;
_tmp7AD=_tmp7A8.rgn;_tmp7AE=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp79E)->f1).rgn;_LL4FF: {struct Cyc_List_List*_tmp811=*_tmp7AC;{union Cyc_Absyn_TunionInfoU_union
_tmp812=*_tmp7AA;struct Cyc_Absyn_UnknownTunionInfo _tmp813;struct _tuple2*_tmp814;
int _tmp815;int _tmp816;struct Cyc_Absyn_Tuniondecl**_tmp817;struct Cyc_Absyn_Tuniondecl*
_tmp818;_LL53A: if((_tmp812.UnknownTunion).tag != 0)goto _LL53C;_tmp813=(_tmp812.UnknownTunion).f1;
_tmp814=_tmp813.name;_tmp815=_tmp813.is_xtunion;_tmp816=_tmp813.is_flat;_LL53B: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp819;_push_handler(&
_tmp819);{int _tmp81B=0;if(setjmp(_tmp819.handler))_tmp81B=1;if(!_tmp81B){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp814);;_pop_handler();}else{void*_tmp81A=(
void*)_exn_thrown;void*_tmp81D=_tmp81A;_LL53F: if(_tmp81D != Cyc_Dict_Absent)goto
_LL541;_LL540: {struct Cyc_Tcenv_Genv*_tmp81E=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmp81F=({struct Cyc_Absyn_Tuniondecl*_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826->sc=(
void*)((void*)3);_tmp826->name=_tmp814;_tmp826->tvs=0;_tmp826->fields=0;_tmp826->is_xtunion=
_tmp815;_tmp826->is_flat=_tmp816;_tmp826;});Cyc_Tc_tcTuniondecl(te,_tmp81E,loc,
_tmp81F);tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp814);if(_tmp811 != 0){({
struct Cyc_String_pa_struct _tmp823;_tmp823.tag=0;_tmp823.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp814));{
struct Cyc_String_pa_struct _tmp822;_tmp822.tag=0;_tmp822.f1=(struct
_dynforward_ptr)(_tmp815?(struct _dynforward_ptr)((struct _dynforward_ptr)({const
char*_tmp824="xtunion";_tag_dynforward(_tmp824,sizeof(char),_get_zero_arr_size(
_tmp824,8));})):(struct _dynforward_ptr)({const char*_tmp825="tunion";
_tag_dynforward(_tmp825,sizeof(char),_get_zero_arr_size(_tmp825,7));}));{void*
_tmp820[2]={& _tmp822,& _tmp823};Cyc_Tcutil_terr(loc,({const char*_tmp821="declare parameterized %s %s before using";
_tag_dynforward(_tmp821,sizeof(char),_get_zero_arr_size(_tmp821,41));}),
_tag_dynforward(_tmp820,sizeof(void*),2));}}});return cvtenv;}goto _LL53E;}_LL541:;
_LL542:(void)_throw(_tmp81D);_LL53E:;}}}if((*tudp)->is_xtunion != _tmp815)({
struct Cyc_String_pa_struct _tmp829;_tmp829.tag=0;_tmp829.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp814));{void*
_tmp827[1]={& _tmp829};Cyc_Tcutil_terr(loc,({const char*_tmp828="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp828,sizeof(char),_get_zero_arr_size(_tmp828,48));}),
_tag_dynforward(_tmp827,sizeof(void*),1));}});*_tmp7AA=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp82A;(_tmp82A.KnownTunion).tag=1;(_tmp82A.KnownTunion).f1=
tudp;_tmp82A;});_tmp818=*tudp;goto _LL53D;}_LL53C: if((_tmp812.KnownTunion).tag != 
1)goto _LL539;_tmp817=(_tmp812.KnownTunion).f1;_tmp818=*_tmp817;_LL53D: if(_tmp818->is_flat
 && (unsigned int)*_tmp7AE)({struct Cyc_String_pa_struct _tmp82D;_tmp82D.tag=0;
_tmp82D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7AE))->v));{void*_tmp82B[1]={&
_tmp82D};Cyc_Tcutil_terr(loc,({const char*_tmp82C="flat tunion has region %s";
_tag_dynforward(_tmp82C,sizeof(char),_get_zero_arr_size(_tmp82C,26));}),
_tag_dynforward(_tmp82B,sizeof(void*),1));}});if(!_tmp818->is_flat  && !((
unsigned int)*_tmp7AE))*_tmp7AE=({struct Cyc_Core_Opt*_tmp82E=_cycalloc(sizeof(*
_tmp82E));_tmp82E->v=(void*)Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp82F=
_cycalloc(sizeof(*_tmp82F));_tmp82F->v=(void*)((void*)3);_tmp82F;}),0);_tmp82E;});
if((unsigned int)*_tmp7AE){void*_tmp830=(void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp7AE))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp830,
1);}{struct Cyc_List_List*tvs=_tmp818->tvs;for(0;_tmp811 != 0  && tvs != 0;(_tmp811=
_tmp811->tl,tvs=tvs->tl)){void*t1=(void*)_tmp811->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp811 != 0)({struct Cyc_String_pa_struct _tmp833;_tmp833.tag=
0;_tmp833.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp818->name));{void*_tmp831[1]={& _tmp833};Cyc_Tcutil_terr(loc,({const char*
_tmp832="too many type arguments for tunion %s";_tag_dynforward(_tmp832,sizeof(
char),_get_zero_arr_size(_tmp832,38));}),_tag_dynforward(_tmp831,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834->hd=(
void*)e;_tmp834->tl=hidden_ts;_tmp834;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,1);}*_tmp7AC=Cyc_List_imp_append(*_tmp7AC,Cyc_List_imp_rev(
hidden_ts));}goto _LL539;}_LL539:;}goto _LL4F3;}_LL500: if(*((int*)_tmp79E)!= 3)
goto _LL502;_tmp7AF=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp79E)->f1;
_tmp7B0=_tmp7AF.field_info;_tmp7B1=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp79E)->f1).field_info;_tmp7B2=_tmp7AF.targs;
_LL501:{union Cyc_Absyn_TunionFieldInfoU_union _tmp835=*_tmp7B1;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp836;struct _tuple2*_tmp837;struct _tuple2*_tmp838;int _tmp839;struct Cyc_Absyn_Tuniondecl*
_tmp83A;struct Cyc_Absyn_Tunionfield*_tmp83B;_LL544: if((_tmp835.UnknownTunionfield).tag
!= 0)goto _LL546;_tmp836=(_tmp835.UnknownTunionfield).f1;_tmp837=_tmp836.tunion_name;
_tmp838=_tmp836.field_name;_tmp839=_tmp836.is_xtunion;_LL545: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp83C;_push_handler(&
_tmp83C);{int _tmp83E=0;if(setjmp(_tmp83C.handler))_tmp83E=1;if(!_tmp83E){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp837);;_pop_handler();}else{void*_tmp83D=(void*)_exn_thrown;void*
_tmp840=_tmp83D;_LL549: if(_tmp840 != Cyc_Dict_Absent)goto _LL54B;_LL54A:({struct
Cyc_String_pa_struct _tmp843;_tmp843.tag=0;_tmp843.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp837));{void*_tmp841[1]={&
_tmp843};Cyc_Tcutil_terr(loc,({const char*_tmp842="unbound type tunion %s";
_tag_dynforward(_tmp842,sizeof(char),_get_zero_arr_size(_tmp842,23));}),
_tag_dynforward(_tmp841,sizeof(void*),1));}});return cvtenv;_LL54B:;_LL54C:(void)
_throw(_tmp840);_LL548:;}}}{struct _handler_cons _tmp844;_push_handler(& _tmp844);{
int _tmp846=0;if(setjmp(_tmp844.handler))_tmp846=1;if(!_tmp846){{struct
_RegionHandle _tmp847=_new_region("r");struct _RegionHandle*r=& _tmp847;
_push_region(r);{void*_tmp848=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp838);struct
Cyc_Absyn_Tuniondecl*_tmp849;struct Cyc_Absyn_Tunionfield*_tmp84A;_LL54E: if(*((
int*)_tmp848)!= 2)goto _LL550;_tmp849=((struct Cyc_Tcenv_TunionRes_struct*)_tmp848)->f1;
_tmp84A=((struct Cyc_Tcenv_TunionRes_struct*)_tmp848)->f2;_LL54F: tuf=_tmp84A;tud=
_tmp849;if(tud->is_xtunion != _tmp839)({struct Cyc_String_pa_struct _tmp84D;_tmp84D.tag=
0;_tmp84D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp837));{void*_tmp84B[1]={& _tmp84D};Cyc_Tcutil_terr(loc,({const char*_tmp84C="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp84C,sizeof(char),_get_zero_arr_size(_tmp84C,48));}),
_tag_dynforward(_tmp84B,sizeof(void*),1));}});goto _LL54D;_LL550:;_LL551:({struct
Cyc_String_pa_struct _tmp851;_tmp851.tag=0;_tmp851.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp837));{struct Cyc_String_pa_struct
_tmp850;_tmp850.tag=0;_tmp850.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp838));{void*_tmp84E[2]={& _tmp850,& _tmp851};Cyc_Tcutil_terr(
loc,({const char*_tmp84F="unbound field %s in type tunion %s";_tag_dynforward(
_tmp84F,sizeof(char),_get_zero_arr_size(_tmp84F,35));}),_tag_dynforward(_tmp84E,
sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp852=cvtenv;_npop_handler(1);
return _tmp852;}_LL54D:;};_pop_region(r);};_pop_handler();}else{void*_tmp845=(
void*)_exn_thrown;void*_tmp854=_tmp845;_LL553: if(_tmp854 != Cyc_Dict_Absent)goto
_LL555;_LL554:({struct Cyc_String_pa_struct _tmp858;_tmp858.tag=0;_tmp858.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp837));{
struct Cyc_String_pa_struct _tmp857;_tmp857.tag=0;_tmp857.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp838));{void*
_tmp855[2]={& _tmp857,& _tmp858};Cyc_Tcutil_terr(loc,({const char*_tmp856="unbound field %s in type tunion %s";
_tag_dynforward(_tmp856,sizeof(char),_get_zero_arr_size(_tmp856,35));}),
_tag_dynforward(_tmp855,sizeof(void*),2));}}});return cvtenv;_LL555:;_LL556:(void)
_throw(_tmp854);_LL552:;}}}*_tmp7B1=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp859;(_tmp859.KnownTunionfield).tag=1;(
_tmp859.KnownTunionfield).f1=tud;(_tmp859.KnownTunionfield).f2=tuf;_tmp859;});
_tmp83A=tud;_tmp83B=tuf;goto _LL547;}_LL546: if((_tmp835.KnownTunionfield).tag != 1)
goto _LL543;_tmp83A=(_tmp835.KnownTunionfield).f1;_tmp83B=(_tmp835.KnownTunionfield).f2;
_LL547: {struct Cyc_List_List*tvs=_tmp83A->tvs;for(0;_tmp7B2 != 0  && tvs != 0;(
_tmp7B2=_tmp7B2->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7B2->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp7B2 != 0)({struct Cyc_String_pa_struct _tmp85D;_tmp85D.tag=
0;_tmp85D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp83B->name));{struct Cyc_String_pa_struct _tmp85C;_tmp85C.tag=0;_tmp85C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp83A->name));{
void*_tmp85A[2]={& _tmp85C,& _tmp85D};Cyc_Tcutil_terr(loc,({const char*_tmp85B="too many type arguments for tunion %s.%s";
_tag_dynforward(_tmp85B,sizeof(char),_get_zero_arr_size(_tmp85B,41));}),
_tag_dynforward(_tmp85A,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct
_tmp861;_tmp861.tag=0;_tmp861.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp83B->name));{struct Cyc_String_pa_struct _tmp860;
_tmp860.tag=0;_tmp860.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp83A->name));{void*_tmp85E[2]={& _tmp860,& _tmp861};Cyc_Tcutil_terr(loc,({const
char*_tmp85F="too few type arguments for tunion %s.%s";_tag_dynforward(_tmp85F,
sizeof(char),_get_zero_arr_size(_tmp85F,40));}),_tag_dynforward(_tmp85E,sizeof(
void*),2));}}});goto _LL543;}_LL543:;}goto _LL4F3;_LL502: if(*((int*)_tmp79E)!= 4)
goto _LL504;_tmp7B3=((struct Cyc_Absyn_PointerType_struct*)_tmp79E)->f1;_tmp7B4=(
void*)_tmp7B3.elt_typ;_tmp7B5=_tmp7B3.elt_tq;_tmp7B6=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp79E)->f1).elt_tq;_tmp7B7=_tmp7B3.ptr_atts;
_tmp7B8=(void*)_tmp7B7.rgn;_tmp7B9=_tmp7B7.nullable;_tmp7BA=_tmp7B7.bounds;
_tmp7BB=_tmp7B7.zero_term;_LL503: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp7B4,1);_tmp7B6->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp7B6->print_const,_tmp7B4);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)5,_tmp7B8,1);{union Cyc_Absyn_Constraint_union _tmp862=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp7BB))->v;int _tmp863;
_LL558: if((_tmp862.No_constr).tag != 2)goto _LL55A;_LL559:{void*_tmp864=Cyc_Tcutil_compress(
_tmp7B4);void*_tmp865;_LL55F: if(_tmp864 <= (void*)4)goto _LL561;if(*((int*)_tmp864)
!= 5)goto _LL561;_tmp865=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp864)->f2;
if((int)_tmp865 != 0)goto _LL561;_LL560:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7BB,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL55E;_LL561:;_LL562:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7BB,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL55E;
_LL55E:;}goto _LL557;_LL55A: if((_tmp862.Eq_constr).tag != 0)goto _LL55C;_tmp863=(
_tmp862.Eq_constr).f1;if(_tmp863 != 1)goto _LL55C;_LL55B: if(!Cyc_Tcutil_admits_zero(
_tmp7B4))({struct Cyc_String_pa_struct _tmp868;_tmp868.tag=0;_tmp868.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp7B4));{void*
_tmp866[1]={& _tmp868};Cyc_Tcutil_terr(loc,({const char*_tmp867="cannot have a pointer to zero-terminated %s elements";
_tag_dynforward(_tmp867,sizeof(char),_get_zero_arr_size(_tmp867,53));}),
_tag_dynforward(_tmp866,sizeof(void*),1));}});is_zero_terminated=1;goto _LL557;
_LL55C:;_LL55D: is_zero_terminated=0;goto _LL557;_LL557:;}{union Cyc_Absyn_Constraint_union
_tmp869=(Cyc_Absyn_compress_conref(_tmp7BA))->v;void*_tmp86A;void*_tmp86B;void*
_tmp86C;struct Cyc_Absyn_Exp*_tmp86D;void*_tmp86E;void*_tmp86F;_LL564: if((_tmp869.No_constr).tag
!= 2)goto _LL566;_LL565: goto _LL567;_LL566: if((_tmp869.Eq_constr).tag != 0)goto
_LL568;_tmp86A=(_tmp869.Eq_constr).f1;if((int)_tmp86A != 0)goto _LL568;_LL567: goto
_LL563;_LL568: if((_tmp869.Eq_constr).tag != 0)goto _LL56A;_tmp86B=(_tmp869.Eq_constr).f1;
if((int)_tmp86B != 1)goto _LL56A;_LL569: goto _LL563;_LL56A: if((_tmp869.Eq_constr).tag
!= 0)goto _LL56C;_tmp86C=(_tmp869.Eq_constr).f1;if(_tmp86C <= (void*)2)goto _LL56C;
if(*((int*)_tmp86C)!= 0)goto _LL56C;_tmp86D=((struct Cyc_Absyn_Upper_b_struct*)
_tmp86C)->f1;_LL56B: Cyc_Tcexp_tcExp(te,0,_tmp86D);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp86D))({void*_tmp870[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp871="pointer bounds expression is not an unsigned int";
_tag_dynforward(_tmp871,sizeof(char),_get_zero_arr_size(_tmp871,49));}),
_tag_dynforward(_tmp870,sizeof(void*),0));});{unsigned int _tmp873;int _tmp874;
struct _tuple7 _tmp872=Cyc_Evexp_eval_const_uint_exp(_tmp86D);_tmp873=_tmp872.f1;
_tmp874=_tmp872.f2;if(is_zero_terminated  && (!_tmp874  || _tmp873 < 1))({void*
_tmp875[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp876="zero-terminated pointer cannot point to empty sequence";
_tag_dynforward(_tmp876,sizeof(char),_get_zero_arr_size(_tmp876,55));}),
_tag_dynforward(_tmp875,sizeof(void*),0));});goto _LL563;}_LL56C: if((_tmp869.Eq_constr).tag
!= 0)goto _LL56E;_tmp86E=(_tmp869.Eq_constr).f1;if(_tmp86E <= (void*)2)goto _LL56E;
if(*((int*)_tmp86E)!= 1)goto _LL56E;_tmp86F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp86E)->f1;_LL56D: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)7,
_tmp86F,1);goto _LL563;_LL56E: if((_tmp869.Forward_constr).tag != 1)goto _LL563;
_LL56F:({void*_tmp877[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp878="forward constraint";
_tag_dynforward(_tmp878,sizeof(char),_get_zero_arr_size(_tmp878,19));}),
_tag_dynforward(_tmp877,sizeof(void*),0));});_LL563:;}goto _LL4F3;}_LL504: if(*((
int*)_tmp79E)!= 18)goto _LL506;_tmp7BC=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp79E)->f1;_LL505: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)7,
_tmp7BC,1);goto _LL4F3;_LL506: if(*((int*)_tmp79E)!= 14)goto _LL508;_tmp7BD=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp79E)->f1;_LL507: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp7BD,1);goto _LL4F3;_LL508: if(*((int*)_tmp79E)!= 5)goto
_LL50A;_LL509: goto _LL50B;_LL50A: if((int)_tmp79E != 1)goto _LL50C;_LL50B: goto _LL50D;
_LL50C: if(_tmp79E <= (void*)4)goto _LL51C;if(*((int*)_tmp79E)!= 6)goto _LL50E;
_LL50D: goto _LL4F3;_LL50E: if(*((int*)_tmp79E)!= 7)goto _LL510;_tmp7BE=((struct Cyc_Absyn_ArrayType_struct*)
_tmp79E)->f1;_tmp7BF=(void*)_tmp7BE.elt_type;_tmp7C0=_tmp7BE.tq;_tmp7C1=(struct
Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp79E)->f1).tq;_tmp7C2=
_tmp7BE.num_elts;_tmp7C3=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp79E)->f1).num_elts;_tmp7C4=_tmp7BE.zero_term;_tmp7C5=_tmp7BE.zt_loc;_LL50F: {
struct Cyc_Absyn_Exp*_tmp879=*_tmp7C3;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)1,_tmp7BF,1);_tmp7C1->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp7C1->print_const,_tmp7BF);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union
_tmp87A=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7C4))->v;int _tmp87B;_LL571: if((_tmp87A.No_constr).tag != 2)goto _LL573;_LL572:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7C4,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL570;
_LL573: if((_tmp87A.Eq_constr).tag != 0)goto _LL575;_tmp87B=(_tmp87A.Eq_constr).f1;
if(_tmp87B != 1)goto _LL575;_LL574: if(!Cyc_Tcutil_admits_zero(_tmp7BF))({struct Cyc_String_pa_struct
_tmp87E;_tmp87E.tag=0;_tmp87E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp7BF));{void*_tmp87C[1]={& _tmp87E};Cyc_Tcutil_terr(loc,({
const char*_tmp87D="cannot have a zero-terminated array of %s elements";
_tag_dynforward(_tmp87D,sizeof(char),_get_zero_arr_size(_tmp87D,51));}),
_tag_dynforward(_tmp87C,sizeof(void*),1));}});is_zero_terminated=1;goto _LL570;
_LL575:;_LL576: is_zero_terminated=0;goto _LL570;_LL570:;}if(_tmp879 == 0){if(
is_zero_terminated)*_tmp7C3=(_tmp879=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,
0));else{({void*_tmp87F[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp880="array bound defaults to 0 here";
_tag_dynforward(_tmp880,sizeof(char),_get_zero_arr_size(_tmp880,31));}),
_tag_dynforward(_tmp87F,sizeof(void*),0));});*_tmp7C3=(_tmp879=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp879);
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp879))({void*_tmp881[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp882="array bounds expression is not constant";
_tag_dynforward(_tmp882,sizeof(char),_get_zero_arr_size(_tmp882,40));}),
_tag_dynforward(_tmp881,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_tmp879))({void*_tmp883[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp884="array bounds expression is not an unsigned int";_tag_dynforward(
_tmp884,sizeof(char),_get_zero_arr_size(_tmp884,47));}),_tag_dynforward(_tmp883,
sizeof(void*),0));});{unsigned int _tmp886;int _tmp887;struct _tuple7 _tmp885=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp879);_tmp886=_tmp885.f1;_tmp887=_tmp885.f2;if((
is_zero_terminated  && _tmp887) && _tmp886 < 1)({void*_tmp888[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp889="zero terminated array cannot have zero elements";
_tag_dynforward(_tmp889,sizeof(char),_get_zero_arr_size(_tmp889,48));}),
_tag_dynforward(_tmp888,sizeof(void*),0));});goto _LL4F3;}}}_LL510: if(*((int*)
_tmp79E)!= 8)goto _LL512;_tmp7C6=((struct Cyc_Absyn_FnType_struct*)_tmp79E)->f1;
_tmp7C7=_tmp7C6.tvars;_tmp7C8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp79E)->f1).tvars;_tmp7C9=_tmp7C6.effect;_tmp7CA=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp79E)->f1).effect;_tmp7CB=(void*)_tmp7C6.ret_typ;
_tmp7CC=_tmp7C6.args;_tmp7CD=_tmp7C6.c_varargs;_tmp7CE=_tmp7C6.cyc_varargs;
_tmp7CF=_tmp7C6.rgn_po;_tmp7D0=_tmp7C6.attributes;_LL511: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7D0 != 0;_tmp7D0=_tmp7D0->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7D0->hd))({struct Cyc_String_pa_struct _tmp88C;
_tmp88C.tag=0;_tmp88C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)_tmp7D0->hd));{void*_tmp88A[1]={& _tmp88C};Cyc_Tcutil_terr(loc,({const char*
_tmp88B="bad function type attribute %s";_tag_dynforward(_tmp88B,sizeof(char),
_get_zero_arr_size(_tmp88B,31));}),_tag_dynforward(_tmp88A,sizeof(void*),1));}});{
void*_tmp88D=(void*)_tmp7D0->hd;void*_tmp88E;int _tmp88F;int _tmp890;_LL578: if((
int)_tmp88D != 0)goto _LL57A;_LL579: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL577;_LL57A: if((int)_tmp88D != 1)goto _LL57C;_LL57B: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL577;_LL57C: if((int)_tmp88D != 2)goto _LL57E;
_LL57D: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL577;_LL57E: if(
_tmp88D <= (void*)17)goto _LL580;if(*((int*)_tmp88D)!= 3)goto _LL580;_tmp88E=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp88D)->f1;_tmp88F=((struct Cyc_Absyn_Format_att_struct*)
_tmp88D)->f2;_tmp890=((struct Cyc_Absyn_Format_att_struct*)_tmp88D)->f3;_LL57F:
if(!seen_format){seen_format=1;ft=_tmp88E;fmt_desc_arg=_tmp88F;fmt_arg_start=
_tmp890;}else{({void*_tmp891[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp892="function can't have multiple format attributes";
_tag_dynforward(_tmp892,sizeof(char),_get_zero_arr_size(_tmp892,47));}),
_tag_dynforward(_tmp891,sizeof(void*),0));});}goto _LL577;_LL580:;_LL581: goto
_LL577;_LL577:;}}if(num_convs > 1)({void*_tmp893[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp894="function can't have multiple calling conventions";
_tag_dynforward(_tmp894,sizeof(char),_get_zero_arr_size(_tmp894,49));}),
_tag_dynforward(_tmp893,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*
_tmp7C8);{struct Cyc_List_List*b=*_tmp7C8;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp895=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp896;void*_tmp897;_LL583: if(*((
int*)_tmp895)!= 0)goto _LL585;_tmp896=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp895)->f1;if((int)_tmp896 != 1)goto _LL585;_LL584:({struct Cyc_String_pa_struct
_tmp89A;_tmp89A.tag=0;_tmp89A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp898[1]={& _tmp89A};Cyc_Tcutil_terr(
loc,({const char*_tmp899="function attempts to abstract Mem type variable %s";
_tag_dynforward(_tmp899,sizeof(char),_get_zero_arr_size(_tmp899,51));}),
_tag_dynforward(_tmp898,sizeof(void*),1));}});goto _LL582;_LL585: if(*((int*)
_tmp895)!= 0)goto _LL587;_tmp897=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp895)->f1;
if((int)_tmp897 != 5)goto _LL587;_LL586:({struct Cyc_String_pa_struct _tmp89D;
_tmp89D.tag=0;_tmp89D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp89B[1]={& _tmp89D};Cyc_Tcutil_terr(
loc,({const char*_tmp89C="function attempts to abstract TopRegion type variable %s";
_tag_dynforward(_tmp89C,sizeof(char),_get_zero_arr_size(_tmp89C,57));}),
_tag_dynforward(_tmp89B,sizeof(void*),1));}});goto _LL582;_LL587:;_LL588: goto
_LL582;_LL582:;}}}{struct _RegionHandle _tmp89E=_new_region("newr");struct
_RegionHandle*newr=& _tmp89E;_push_region(newr);{struct Cyc_Tcutil_CVTEnv _tmp89F=({
struct Cyc_Tcutil_CVTEnv _tmp939;_tmp939.r=newr;_tmp939.kind_env=cvtenv.kind_env;
_tmp939.free_vars=0;_tmp939.free_evars=0;_tmp939.generalize_evars=cvtenv.generalize_evars;
_tmp939.fn_result=1;_tmp939;});_tmp89F=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp89F,(void*)1,_tmp7CB,1);_tmp89F.fn_result=0;{struct Cyc_List_List*a=_tmp7CC;
for(0;a != 0;a=a->tl){struct _tuple3*_tmp8A0=(struct _tuple3*)a->hd;void*_tmp8A1=(*
_tmp8A0).f3;_tmp89F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp89F,(void*)1,
_tmp8A1,1);((*_tmp8A0).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmp8A0).f2).print_const,_tmp8A1);}}if(_tmp7CE != 0){if(_tmp7CD)({void*_tmp8A2[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp8A3="both c_vararg and cyc_vararg";_tag_dynforward(_tmp8A3,sizeof(
char),_get_zero_arr_size(_tmp8A3,29));}),_tag_dynforward(_tmp8A2,sizeof(void*),0));});{
void*_tmp8A5;int _tmp8A6;struct Cyc_Absyn_VarargInfo _tmp8A4=*_tmp7CE;_tmp8A5=(void*)
_tmp8A4.type;_tmp8A6=_tmp8A4.inject;_tmp89F=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp89F,(void*)1,_tmp8A5,1);(_tmp7CE->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7CE->tq).print_const,_tmp8A5);if(_tmp8A6){void*_tmp8A7=Cyc_Tcutil_compress(
_tmp8A5);struct Cyc_Absyn_TunionInfo _tmp8A8;union Cyc_Absyn_TunionInfoU_union
_tmp8A9;struct Cyc_Absyn_Tuniondecl**_tmp8AA;_LL58A: if(_tmp8A7 <= (void*)4)goto
_LL58C;if(*((int*)_tmp8A7)!= 2)goto _LL58C;_tmp8A8=((struct Cyc_Absyn_TunionType_struct*)
_tmp8A7)->f1;_tmp8A9=_tmp8A8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp8A7)->f1).tunion_info).KnownTunion).tag != 1)goto _LL58C;_tmp8AA=(_tmp8A9.KnownTunion).f1;
_LL58B: if((*_tmp8AA)->is_flat)({void*_tmp8AB[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp8AC="cant inject into a flat tunion";_tag_dynforward(_tmp8AC,sizeof(
char),_get_zero_arr_size(_tmp8AC,31));}),_tag_dynforward(_tmp8AB,sizeof(void*),0));});
goto _LL589;_LL58C:;_LL58D:({void*_tmp8AD[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp8AE="can't inject a non-[x]tunion type";_tag_dynforward(_tmp8AE,sizeof(char),
_get_zero_arr_size(_tmp8AE,34));}),_tag_dynforward(_tmp8AD,sizeof(void*),0));});
goto _LL589;_LL589:;}}}if(seen_format){int _tmp8AF=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp7CC);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp8AF) || 
fmt_arg_start < 0) || _tmp7CE == 0  && fmt_arg_start != 0) || _tmp7CE != 0  && 
fmt_arg_start != _tmp8AF + 1)({void*_tmp8B0[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp8B1="bad format descriptor";_tag_dynforward(_tmp8B1,sizeof(char),
_get_zero_arr_size(_tmp8B1,22));}),_tag_dynforward(_tmp8B0,sizeof(void*),0));});
else{void*_tmp8B3;struct _tuple3 _tmp8B2=*((struct _tuple3*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp7CC,fmt_desc_arg - 1);_tmp8B3=_tmp8B2.f3;{void*_tmp8B4=
Cyc_Tcutil_compress(_tmp8B3);struct Cyc_Absyn_PtrInfo _tmp8B5;void*_tmp8B6;struct
Cyc_Absyn_PtrAtts _tmp8B7;struct Cyc_Absyn_Conref*_tmp8B8;struct Cyc_Absyn_Conref*
_tmp8B9;_LL58F: if(_tmp8B4 <= (void*)4)goto _LL591;if(*((int*)_tmp8B4)!= 4)goto
_LL591;_tmp8B5=((struct Cyc_Absyn_PointerType_struct*)_tmp8B4)->f1;_tmp8B6=(void*)
_tmp8B5.elt_typ;_tmp8B7=_tmp8B5.ptr_atts;_tmp8B8=_tmp8B7.bounds;_tmp8B9=_tmp8B7.zero_term;
_LL590:{struct _tuple0 _tmp8BB=({struct _tuple0 _tmp8BA;_tmp8BA.f1=Cyc_Tcutil_compress(
_tmp8B6);_tmp8BA.f2=Cyc_Absyn_conref_def((void*)((void*)0),_tmp8B8);_tmp8BA;});
void*_tmp8BC;void*_tmp8BD;void*_tmp8BE;void*_tmp8BF;_LL594: _tmp8BC=_tmp8BB.f1;
if(_tmp8BC <= (void*)4)goto _LL596;if(*((int*)_tmp8BC)!= 5)goto _LL596;_tmp8BD=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp8BC)->f1;if((int)_tmp8BD != 2)goto
_LL596;_tmp8BE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp8BC)->f2;if((int)
_tmp8BE != 0)goto _LL596;_tmp8BF=_tmp8BB.f2;if((int)_tmp8BF != 0)goto _LL596;_LL595:
goto _LL593;_LL596:;_LL597:({void*_tmp8C0[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp8C1="format descriptor is not a char ? type";_tag_dynforward(_tmp8C1,sizeof(
char),_get_zero_arr_size(_tmp8C1,39));}),_tag_dynforward(_tmp8C0,sizeof(void*),0));});
goto _LL593;_LL593:;}goto _LL58E;_LL591:;_LL592:({void*_tmp8C2[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8C3="format descriptor is not a char ? type";_tag_dynforward(
_tmp8C3,sizeof(char),_get_zero_arr_size(_tmp8C3,39));}),_tag_dynforward(_tmp8C2,
sizeof(void*),0));});goto _LL58E;_LL58E:;}if(fmt_arg_start != 0){int problem;{
struct _tuple0 _tmp8C5=({struct _tuple0 _tmp8C4;_tmp8C4.f1=ft;_tmp8C4.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7CE))->type);_tmp8C4;});void*
_tmp8C6;void*_tmp8C7;struct Cyc_Absyn_TunionInfo _tmp8C8;union Cyc_Absyn_TunionInfoU_union
_tmp8C9;struct Cyc_Absyn_Tuniondecl**_tmp8CA;struct Cyc_Absyn_Tuniondecl*_tmp8CB;
void*_tmp8CC;void*_tmp8CD;struct Cyc_Absyn_TunionInfo _tmp8CE;union Cyc_Absyn_TunionInfoU_union
_tmp8CF;struct Cyc_Absyn_Tuniondecl**_tmp8D0;struct Cyc_Absyn_Tuniondecl*_tmp8D1;
_LL599: _tmp8C6=_tmp8C5.f1;if((int)_tmp8C6 != 0)goto _LL59B;_tmp8C7=_tmp8C5.f2;if(
_tmp8C7 <= (void*)4)goto _LL59B;if(*((int*)_tmp8C7)!= 2)goto _LL59B;_tmp8C8=((
struct Cyc_Absyn_TunionType_struct*)_tmp8C7)->f1;_tmp8C9=_tmp8C8.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8C7)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL59B;_tmp8CA=(_tmp8C9.KnownTunion).f1;_tmp8CB=*_tmp8CA;_LL59A: problem=
Cyc_Absyn_qvar_cmp(_tmp8CB->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL598;
_LL59B: _tmp8CC=_tmp8C5.f1;if((int)_tmp8CC != 1)goto _LL59D;_tmp8CD=_tmp8C5.f2;if(
_tmp8CD <= (void*)4)goto _LL59D;if(*((int*)_tmp8CD)!= 2)goto _LL59D;_tmp8CE=((
struct Cyc_Absyn_TunionType_struct*)_tmp8CD)->f1;_tmp8CF=_tmp8CE.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8CD)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL59D;_tmp8D0=(_tmp8CF.KnownTunion).f1;_tmp8D1=*_tmp8D0;_LL59C: problem=
Cyc_Absyn_qvar_cmp(_tmp8D1->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL598;
_LL59D:;_LL59E: problem=1;goto _LL598;_LL598:;}if(problem)({void*_tmp8D2[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8D3="format attribute and vararg types don't match";
_tag_dynforward(_tmp8D3,sizeof(char),_get_zero_arr_size(_tmp8D3,46));}),
_tag_dynforward(_tmp8D2,sizeof(void*),0));});}}}{struct Cyc_List_List*rpo=_tmp7CF;
for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8D5;void*_tmp8D6;void*_tmp8D7;struct
_tuple0*_tmp8D4=(struct _tuple0*)rpo->hd;_tmp8D5=*_tmp8D4;_tmp8D6=_tmp8D5.f1;
_tmp8D7=_tmp8D5.f2;_tmp89F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp89F,(void*)6,
_tmp8D6,1);_tmp89F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp89F,(void*)3,_tmp8D7,
1);}}if(*_tmp7CA != 0)_tmp89F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp89F,(void*)
6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7CA))->v,1);else{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp89F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8D9;int _tmp8DA;struct _tuple18 _tmp8D8=*((struct _tuple18*)
tvs->hd);_tmp8D9=_tmp8D8.f1;_tmp8DA=_tmp8D8.f2;if(!_tmp8DA)continue;{void*
_tmp8DB=Cyc_Absyn_compress_kb((void*)_tmp8D9->kind);void*_tmp8DC;struct Cyc_Core_Opt*
_tmp8DD;struct Cyc_Core_Opt**_tmp8DE;void*_tmp8DF;struct Cyc_Core_Opt*_tmp8E0;
struct Cyc_Core_Opt**_tmp8E1;void*_tmp8E2;void*_tmp8E3;void*_tmp8E4;void*_tmp8E5;
struct Cyc_Core_Opt*_tmp8E6;struct Cyc_Core_Opt**_tmp8E7;void*_tmp8E8;void*_tmp8E9;
struct Cyc_Core_Opt*_tmp8EA;struct Cyc_Core_Opt**_tmp8EB;_LL5A0: if(*((int*)_tmp8DB)
!= 0)goto _LL5A2;_tmp8DC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8DB)->f1;if((
int)_tmp8DC != 5)goto _LL5A2;_LL5A1:({void*_tmp8EC[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp8ED="TopRgnKind in toplevel effect!\n";_tag_dynforward(_tmp8ED,sizeof(char),
_get_zero_arr_size(_tmp8ED,32));}),_tag_dynforward(_tmp8EC,sizeof(void*),0));});
_LL5A2: if(*((int*)_tmp8DB)!= 2)goto _LL5A4;_tmp8DD=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8DE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8DF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f2;if((
int)_tmp8DF != 5)goto _LL5A4;_LL5A3: _tmp8E1=_tmp8DE;_tmp8E2=(void*)3;goto _LL5A5;
_LL5A4: if(*((int*)_tmp8DB)!= 2)goto _LL5A6;_tmp8E0=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8E1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8E2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f2;if(!(
_tmp8E2 == (void*)3  || _tmp8E2 == (void*)4))goto _LL5A6;_LL5A5:*_tmp8E1=({struct Cyc_Core_Opt*
_tmp8EE=_cycalloc(sizeof(*_tmp8EE));_tmp8EE->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8EF=_cycalloc(sizeof(*_tmp8EF));_tmp8EF[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8F0;_tmp8F0.tag=0;_tmp8F0.f1=(void*)_tmp8E2;_tmp8F0;});_tmp8EF;}));_tmp8EE;});
_tmp8E3=_tmp8E2;goto _LL5A7;_LL5A6: if(*((int*)_tmp8DB)!= 0)goto _LL5A8;_tmp8E3=(
void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8DB)->f1;if(!(_tmp8E3 == (void*)3  || 
_tmp8E3 == (void*)4))goto _LL5A8;_LL5A7: effect=({struct Cyc_List_List*_tmp8F1=
_cycalloc(sizeof(*_tmp8F1));_tmp8F1->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp8F2=_cycalloc(sizeof(*_tmp8F2));_tmp8F2[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp8F3;_tmp8F3.tag=20;_tmp8F3.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8F4=_cycalloc(sizeof(*_tmp8F4));_tmp8F4[0]=({struct Cyc_Absyn_VarType_struct
_tmp8F5;_tmp8F5.tag=1;_tmp8F5.f1=_tmp8D9;_tmp8F5;});_tmp8F4;}));_tmp8F3;});
_tmp8F2;}));_tmp8F1->tl=effect;_tmp8F1;});goto _LL59F;_LL5A8: if(*((int*)_tmp8DB)
!= 2)goto _LL5AA;_tmp8E4=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f2;
if((int)_tmp8E4 != 7)goto _LL5AA;_LL5A9: goto _LL5AB;_LL5AA: if(*((int*)_tmp8DB)!= 0)
goto _LL5AC;_tmp8E5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8DB)->f1;if((int)
_tmp8E5 != 7)goto _LL5AC;_LL5AB: goto _LL59F;_LL5AC: if(*((int*)_tmp8DB)!= 2)goto
_LL5AE;_tmp8E6=((struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f1;_tmp8E7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f1;_tmp8E8=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f2;if((int)_tmp8E8 != 6)goto _LL5AE;_LL5AD:*_tmp8E7=({struct Cyc_Core_Opt*
_tmp8F6=_cycalloc(sizeof(*_tmp8F6));_tmp8F6->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8F7=_cycalloc(sizeof(*_tmp8F7));_tmp8F7[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8F8;_tmp8F8.tag=0;_tmp8F8.f1=(void*)((void*)6);_tmp8F8;});_tmp8F7;}));
_tmp8F6;});goto _LL5AF;_LL5AE: if(*((int*)_tmp8DB)!= 0)goto _LL5B0;_tmp8E9=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8DB)->f1;if((int)_tmp8E9 != 6)goto _LL5B0;_LL5AF:
effect=({struct Cyc_List_List*_tmp8F9=_cycalloc(sizeof(*_tmp8F9));_tmp8F9->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));
_tmp8FA[0]=({struct Cyc_Absyn_VarType_struct _tmp8FB;_tmp8FB.tag=1;_tmp8FB.f1=
_tmp8D9;_tmp8FB;});_tmp8FA;}));_tmp8F9->tl=effect;_tmp8F9;});goto _LL59F;_LL5B0:
if(*((int*)_tmp8DB)!= 1)goto _LL5B2;_tmp8EA=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8DB)->f1;_tmp8EB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8DB)->f1;_LL5B1:*_tmp8EB=({struct Cyc_Core_Opt*_tmp8FC=_cycalloc(sizeof(*
_tmp8FC));_tmp8FC->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp8FD=
_cycalloc(sizeof(*_tmp8FD));_tmp8FD[0]=({struct Cyc_Absyn_Less_kb_struct _tmp8FE;
_tmp8FE.tag=2;_tmp8FE.f1=0;_tmp8FE.f2=(void*)((void*)0);_tmp8FE;});_tmp8FD;}));
_tmp8FC;});goto _LL5B3;_LL5B2:;_LL5B3: effect=({struct Cyc_List_List*_tmp8FF=
_cycalloc(sizeof(*_tmp8FF));_tmp8FF->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp900=_cycalloc(sizeof(*_tmp900));_tmp900[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp901;_tmp901.tag=22;_tmp901.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp902=_cycalloc(sizeof(*_tmp902));_tmp902[0]=({struct Cyc_Absyn_VarType_struct
_tmp903;_tmp903.tag=1;_tmp903.f1=_tmp8D9;_tmp903;});_tmp902;}));_tmp901;});
_tmp900;}));_tmp8FF->tl=effect;_tmp8FF;});goto _LL59F;_LL59F:;}}}{struct Cyc_List_List*
ts=_tmp89F.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp905;int _tmp906;struct
_tuple19 _tmp904=*((struct _tuple19*)ts->hd);_tmp905=_tmp904.f1;_tmp906=_tmp904.f2;
if(!_tmp906)continue;{void*_tmp907=Cyc_Tcutil_typ_kind(_tmp905);_LL5B5: if((int)
_tmp907 != 5)goto _LL5B7;_LL5B6: goto _LL5B8;_LL5B7: if((int)_tmp907 != 4)goto _LL5B9;
_LL5B8: goto _LL5BA;_LL5B9: if((int)_tmp907 != 3)goto _LL5BB;_LL5BA: effect=({struct
Cyc_List_List*_tmp908=_cycalloc(sizeof(*_tmp908));_tmp908->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp909=_cycalloc(sizeof(*_tmp909));_tmp909[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp90A;_tmp90A.tag=20;_tmp90A.f1=(void*)_tmp905;
_tmp90A;});_tmp909;}));_tmp908->tl=effect;_tmp908;});goto _LL5B4;_LL5BB: if((int)
_tmp907 != 6)goto _LL5BD;_LL5BC: effect=({struct Cyc_List_List*_tmp90B=_cycalloc(
sizeof(*_tmp90B));_tmp90B->hd=(void*)_tmp905;_tmp90B->tl=effect;_tmp90B;});goto
_LL5B4;_LL5BD: if((int)_tmp907 != 7)goto _LL5BF;_LL5BE: goto _LL5B4;_LL5BF:;_LL5C0:
effect=({struct Cyc_List_List*_tmp90C=_cycalloc(sizeof(*_tmp90C));_tmp90C->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp90D=_cycalloc(sizeof(*_tmp90D));
_tmp90D[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp90E;_tmp90E.tag=22;_tmp90E.f1=(
void*)_tmp905;_tmp90E;});_tmp90D;}));_tmp90C->tl=effect;_tmp90C;});goto _LL5B4;
_LL5B4:;}}}*_tmp7CA=({struct Cyc_Core_Opt*_tmp90F=_cycalloc(sizeof(*_tmp90F));
_tmp90F->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp910=_cycalloc(
sizeof(*_tmp910));_tmp910[0]=({struct Cyc_Absyn_JoinEff_struct _tmp911;_tmp911.tag=
21;_tmp911.f1=effect;_tmp911;});_tmp910;}));_tmp90F;});}if(*_tmp7C8 != 0){struct
Cyc_List_List*bs=*_tmp7C8;for(0;bs != 0;bs=bs->tl){void*_tmp912=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp913;struct
Cyc_Core_Opt**_tmp914;struct Cyc_Core_Opt*_tmp915;struct Cyc_Core_Opt**_tmp916;
void*_tmp917;struct Cyc_Core_Opt*_tmp918;struct Cyc_Core_Opt**_tmp919;void*_tmp91A;
struct Cyc_Core_Opt*_tmp91B;struct Cyc_Core_Opt**_tmp91C;void*_tmp91D;struct Cyc_Core_Opt*
_tmp91E;struct Cyc_Core_Opt**_tmp91F;void*_tmp920;void*_tmp921;void*_tmp922;
_LL5C2: if(*((int*)_tmp912)!= 1)goto _LL5C4;_tmp913=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp912)->f1;_tmp914=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp912)->f1;_LL5C3:({struct Cyc_String_pa_struct _tmp925;_tmp925.tag=0;_tmp925.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);{
void*_tmp923[1]={& _tmp925};Cyc_Tcutil_warn(loc,({const char*_tmp924="Type variable %s unconstrained, assuming boxed";
_tag_dynforward(_tmp924,sizeof(char),_get_zero_arr_size(_tmp924,47));}),
_tag_dynforward(_tmp923,sizeof(void*),1));}});_tmp916=_tmp914;goto _LL5C5;_LL5C4:
if(*((int*)_tmp912)!= 2)goto _LL5C6;_tmp915=((struct Cyc_Absyn_Less_kb_struct*)
_tmp912)->f1;_tmp916=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp912)->f1;_tmp917=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp912)->f2;if((
int)_tmp917 != 1)goto _LL5C6;_LL5C5: _tmp919=_tmp916;goto _LL5C7;_LL5C6: if(*((int*)
_tmp912)!= 2)goto _LL5C8;_tmp918=((struct Cyc_Absyn_Less_kb_struct*)_tmp912)->f1;
_tmp919=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp912)->f1;
_tmp91A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp912)->f2;if((int)_tmp91A != 
0)goto _LL5C8;_LL5C7:*_tmp919=({struct Cyc_Core_Opt*_tmp926=_cycalloc(sizeof(*
_tmp926));_tmp926->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp927=
_cycalloc(sizeof(*_tmp927));_tmp927[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp928;
_tmp928.tag=0;_tmp928.f1=(void*)((void*)2);_tmp928;});_tmp927;}));_tmp926;});
goto _LL5C1;_LL5C8: if(*((int*)_tmp912)!= 2)goto _LL5CA;_tmp91B=((struct Cyc_Absyn_Less_kb_struct*)
_tmp912)->f1;_tmp91C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp912)->f1;_tmp91D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp912)->f2;if((
int)_tmp91D != 5)goto _LL5CA;_LL5C9:*_tmp91C=({struct Cyc_Core_Opt*_tmp929=
_cycalloc(sizeof(*_tmp929));_tmp929->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp92A=_cycalloc(sizeof(*_tmp92A));_tmp92A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp92B;_tmp92B.tag=0;_tmp92B.f1=(void*)((void*)3);_tmp92B;});_tmp92A;}));
_tmp929;});goto _LL5C1;_LL5CA: if(*((int*)_tmp912)!= 2)goto _LL5CC;_tmp91E=((struct
Cyc_Absyn_Less_kb_struct*)_tmp912)->f1;_tmp91F=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp912)->f1;_tmp920=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp912)->f2;_LL5CB:*_tmp91F=({struct Cyc_Core_Opt*_tmp92C=_cycalloc(sizeof(*
_tmp92C));_tmp92C->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp92D=
_cycalloc(sizeof(*_tmp92D));_tmp92D[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp92E;
_tmp92E.tag=0;_tmp92E.f1=(void*)_tmp920;_tmp92E;});_tmp92D;}));_tmp92C;});goto
_LL5C1;_LL5CC: if(*((int*)_tmp912)!= 0)goto _LL5CE;_tmp921=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp912)->f1;if((int)_tmp921 != 1)goto _LL5CE;_LL5CD:({void*_tmp92F[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp930="functions can't abstract M types";_tag_dynforward(
_tmp930,sizeof(char),_get_zero_arr_size(_tmp930,33));}),_tag_dynforward(_tmp92F,
sizeof(void*),0));});goto _LL5C1;_LL5CE: if(*((int*)_tmp912)!= 0)goto _LL5D0;
_tmp922=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp912)->f1;if((int)_tmp922 != 5)
goto _LL5D0;_LL5CF:({void*_tmp931[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp932="functions can't abstract TR types";
_tag_dynforward(_tmp932,sizeof(char),_get_zero_arr_size(_tmp932,34));}),
_tag_dynforward(_tmp931,sizeof(void*),0));});goto _LL5C1;_LL5D0:;_LL5D1: goto
_LL5C1;_LL5C1:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,
_tmp89F.kind_env,*_tmp7C8);_tmp89F.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp89F.r,_tmp89F.free_vars,*_tmp7C8);{struct Cyc_List_List*tvs=_tmp89F.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp934;int _tmp935;struct
_tuple18 _tmp933=*((struct _tuple18*)tvs->hd);_tmp934=_tmp933.f1;_tmp935=_tmp933.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp934,_tmp935);}}{struct Cyc_List_List*evs=_tmp89F.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp937;int _tmp938;struct _tuple19 _tmp936=*((struct _tuple19*)evs->hd);
_tmp937=_tmp936.f1;_tmp938=_tmp936.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp937,_tmp938);}}};_pop_region(newr);}goto _LL4F3;}
_LL512: if(*((int*)_tmp79E)!= 9)goto _LL514;_tmp7D1=((struct Cyc_Absyn_TupleType_struct*)
_tmp79E)->f1;_LL513: for(0;_tmp7D1 != 0;_tmp7D1=_tmp7D1->tl){struct _tuple5*_tmp93A=(
struct _tuple5*)_tmp7D1->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)1,(*_tmp93A).f2,1);((*_tmp93A).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp93A).f1).print_const,(*_tmp93A).f2);}goto _LL4F3;_LL514: if(*((int*)
_tmp79E)!= 11)goto _LL516;_tmp7D2=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp79E)->f1;_tmp7D3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp79E)->f2;_LL515:{
struct _RegionHandle _tmp93B=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp93B;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp7D3 != 0;_tmp7D3=_tmp7D3->tl){struct Cyc_Absyn_Aggrfield _tmp93D;struct
_dynforward_ptr*_tmp93E;struct Cyc_Absyn_Tqual _tmp93F;struct Cyc_Absyn_Tqual*
_tmp940;void*_tmp941;struct Cyc_Absyn_Exp*_tmp942;struct Cyc_List_List*_tmp943;
struct Cyc_Absyn_Aggrfield*_tmp93C=(struct Cyc_Absyn_Aggrfield*)_tmp7D3->hd;
_tmp93D=*_tmp93C;_tmp93E=_tmp93D.name;_tmp93F=_tmp93D.tq;_tmp940=(struct Cyc_Absyn_Tqual*)&(*
_tmp93C).tq;_tmp941=(void*)_tmp93D.type;_tmp942=_tmp93D.width;_tmp943=_tmp93D.attributes;
if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct
Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,
_tmp93E))({struct Cyc_String_pa_struct _tmp946;_tmp946.tag=0;_tmp946.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp93E);{void*_tmp944[1]={& _tmp946};
Cyc_Tcutil_terr(loc,({const char*_tmp945="duplicate field %s";_tag_dynforward(
_tmp945,sizeof(char),_get_zero_arr_size(_tmp945,19));}),_tag_dynforward(_tmp944,
sizeof(void*),1));}});if(Cyc_strcmp((struct _dynforward_ptr)*_tmp93E,({const char*
_tmp947="";_tag_dynforward(_tmp947,sizeof(char),_get_zero_arr_size(_tmp947,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp948=_region_malloc(aprev_rgn,sizeof(*
_tmp948));_tmp948->hd=_tmp93E;_tmp948->tl=prev_fields;_tmp948;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp941,1);_tmp940->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp940->print_const,_tmp941);if(_tmp7D2 == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp941))({struct Cyc_String_pa_struct _tmp94B;_tmp94B.tag=0;_tmp94B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp93E);{void*_tmp949[1]={& _tmp94B};
Cyc_Tcutil_warn(loc,({const char*_tmp94A="union member %s is not `bits-only' so it can only be written and not read";
_tag_dynforward(_tmp94A,sizeof(char),_get_zero_arr_size(_tmp94A,74));}),
_tag_dynforward(_tmp949,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,
_tmp941,_tmp942,_tmp93E);Cyc_Tcutil_check_field_atts(loc,_tmp93E,_tmp943);}};
_pop_region(aprev_rgn);}goto _LL4F3;_LL516: if(*((int*)_tmp79E)!= 10)goto _LL518;
_tmp7D4=((struct Cyc_Absyn_AggrType_struct*)_tmp79E)->f1;_tmp7D5=_tmp7D4.aggr_info;
_tmp7D6=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp79E)->f1).aggr_info;_tmp7D7=_tmp7D4.targs;_tmp7D8=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp79E)->f1).targs;_LL517:{union Cyc_Absyn_AggrInfoU_union
_tmp94C=*_tmp7D6;void*_tmp94D;struct _tuple2*_tmp94E;struct Cyc_Absyn_Aggrdecl**
_tmp94F;struct Cyc_Absyn_Aggrdecl*_tmp950;_LL5D3: if((_tmp94C.UnknownAggr).tag != 0)
goto _LL5D5;_tmp94D=(_tmp94C.UnknownAggr).f1;_tmp94E=(_tmp94C.UnknownAggr).f2;
_LL5D4: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp951;_push_handler(&
_tmp951);{int _tmp953=0;if(setjmp(_tmp951.handler))_tmp953=1;if(!_tmp953){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp94E);*_tmp7D6=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp954;(_tmp954.KnownAggr).tag=1;(_tmp954.KnownAggr).f1=adp;_tmp954;});;
_pop_handler();}else{void*_tmp952=(void*)_exn_thrown;void*_tmp956=_tmp952;_LL5D8:
if(_tmp956 != Cyc_Dict_Absent)goto _LL5DA;_LL5D9: {struct Cyc_Tcenv_Genv*_tmp957=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*_tmp958=({struct Cyc_Absyn_Aggrdecl*
_tmp95D=_cycalloc(sizeof(*_tmp95D));_tmp95D->kind=(void*)_tmp94D;_tmp95D->sc=(
void*)((void*)3);_tmp95D->name=_tmp94E;_tmp95D->tvs=0;_tmp95D->impl=0;_tmp95D->attributes=
0;_tmp95D;});Cyc_Tc_tcAggrdecl(te,_tmp957,loc,_tmp958);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp94E);*_tmp7D6=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp959;(_tmp959.KnownAggr).tag=1;(_tmp959.KnownAggr).f1=adp;_tmp959;});if(*
_tmp7D8 != 0){({struct Cyc_String_pa_struct _tmp95C;_tmp95C.tag=0;_tmp95C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp94E));{
void*_tmp95A[1]={& _tmp95C};Cyc_Tcutil_terr(loc,({const char*_tmp95B="declare parameterized type %s before using";
_tag_dynforward(_tmp95B,sizeof(char),_get_zero_arr_size(_tmp95B,43));}),
_tag_dynforward(_tmp95A,sizeof(void*),1));}});return cvtenv;}goto _LL5D7;}_LL5DA:;
_LL5DB:(void)_throw(_tmp956);_LL5D7:;}}}_tmp950=*adp;goto _LL5D6;}_LL5D5: if((
_tmp94C.KnownAggr).tag != 1)goto _LL5D2;_tmp94F=(_tmp94C.KnownAggr).f1;_tmp950=*
_tmp94F;_LL5D6: {struct Cyc_List_List*tvs=_tmp950->tvs;struct Cyc_List_List*ts=*
_tmp7D8;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);}if(ts != 0)({struct Cyc_String_pa_struct _tmp960;_tmp960.tag=
0;_tmp960.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp950->name));{void*_tmp95E[1]={& _tmp960};Cyc_Tcutil_terr(loc,({const char*
_tmp95F="too many parameters for type %s";_tag_dynforward(_tmp95F,sizeof(char),
_get_zero_arr_size(_tmp95F,32));}),_tag_dynforward(_tmp95E,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp961=_cycalloc(sizeof(*_tmp961));_tmp961->hd=(
void*)e;_tmp961->tl=hidden_ts;_tmp961;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmp7D8=Cyc_List_imp_append(*_tmp7D8,Cyc_List_imp_rev(
hidden_ts));}}_LL5D2:;}goto _LL4F3;_LL518: if(*((int*)_tmp79E)!= 17)goto _LL51A;
_tmp7D9=((struct Cyc_Absyn_TypedefType_struct*)_tmp79E)->f1;_tmp7DA=((struct Cyc_Absyn_TypedefType_struct*)
_tmp79E)->f2;_tmp7DB=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp79E)->f2;_tmp7DC=((struct Cyc_Absyn_TypedefType_struct*)_tmp79E)->f3;_tmp7DD=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp79E)->f3;
_tmp7DE=((struct Cyc_Absyn_TypedefType_struct*)_tmp79E)->f4;_tmp7DF=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp79E)->f4;_LL519: {struct Cyc_List_List*
targs=*_tmp7DB;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp962;
_push_handler(& _tmp962);{int _tmp964=0;if(setjmp(_tmp962.handler))_tmp964=1;if(!
_tmp964){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7D9);;_pop_handler();}else{
void*_tmp963=(void*)_exn_thrown;void*_tmp966=_tmp963;_LL5DD: if(_tmp966 != Cyc_Dict_Absent)
goto _LL5DF;_LL5DE:({struct Cyc_String_pa_struct _tmp969;_tmp969.tag=0;_tmp969.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D9));{
void*_tmp967[1]={& _tmp969};Cyc_Tcutil_terr(loc,({const char*_tmp968="unbound typedef name %s";
_tag_dynforward(_tmp968,sizeof(char),_get_zero_arr_size(_tmp968,24));}),
_tag_dynforward(_tmp967,sizeof(void*),1));}});return cvtenv;_LL5DF:;_LL5E0:(void)
_throw(_tmp966);_LL5DC:;}}}*_tmp7DD=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7D9[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(
ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);inst=({
struct Cyc_List_List*_tmp96A=_cycalloc(sizeof(*_tmp96A));_tmp96A->hd=({struct
_tuple8*_tmp96B=_cycalloc(sizeof(*_tmp96B));_tmp96B->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp96B->f2=(void*)ts->hd;_tmp96B;});_tmp96A->tl=inst;_tmp96A;});}if(ts
!= 0)({struct Cyc_String_pa_struct _tmp96E;_tmp96E.tag=0;_tmp96E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D9));{void*
_tmp96C[1]={& _tmp96E};Cyc_Tcutil_terr(loc,({const char*_tmp96D="too many parameters for typedef %s";
_tag_dynforward(_tmp96D,sizeof(char),_get_zero_arr_size(_tmp96D,35));}),
_tag_dynforward(_tmp96C,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp96F=
_cycalloc(sizeof(*_tmp96F));_tmp96F->hd=(void*)e;_tmp96F->tl=hidden_ts;_tmp96F;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);inst=({struct Cyc_List_List*
_tmp970=_cycalloc(sizeof(*_tmp970));_tmp970->hd=({struct _tuple8*_tmp971=
_cycalloc(sizeof(*_tmp971));_tmp971->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp971->f2=
e;_tmp971;});_tmp970->tl=inst;_tmp970;});}*_tmp7DB=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp7DF=({void**_tmp972=
_cycalloc(sizeof(*_tmp972));_tmp972[0]=new_typ;_tmp972;});}goto _LL4F3;}}_LL51A:
if(*((int*)_tmp79E)!= 19)goto _LL51C;_LL51B: goto _LL51D;_LL51C: if((int)_tmp79E != 3)
goto _LL51E;_LL51D: goto _LL51F;_LL51E: if((int)_tmp79E != 2)goto _LL520;_LL51F: goto
_LL4F3;_LL520: if(_tmp79E <= (void*)4)goto _LL522;if(*((int*)_tmp79E)!= 15)goto
_LL522;_tmp7E0=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp79E)->f1;
_LL521: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp7E0,1);
goto _LL4F3;_LL522: if(_tmp79E <= (void*)4)goto _LL524;if(*((int*)_tmp79E)!= 16)goto
_LL524;_tmp7E1=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp79E)->f1;_tmp7E2=(
void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp79E)->f2;_LL523: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7E1,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp7E2,1);goto _LL4F3;_LL524: if(_tmp79E <= (void*)4)goto _LL526;
if(*((int*)_tmp79E)!= 20)goto _LL526;_tmp7E3=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp79E)->f1;_LL525: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp7E3,1);goto _LL4F3;_LL526: if(_tmp79E <= (void*)4)goto _LL528;if(*((int*)_tmp79E)
!= 22)goto _LL528;_tmp7E4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp79E)->f1;
_LL527: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7E4,1);
goto _LL4F3;_LL528: if(_tmp79E <= (void*)4)goto _LL4F3;if(*((int*)_tmp79E)!= 21)goto
_LL4F3;_tmp7E5=((struct Cyc_Absyn_JoinEff_struct*)_tmp79E)->f1;_LL529: for(0;
_tmp7E5 != 0;_tmp7E5=_tmp7E5->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7E5->hd,1);}goto _LL4F3;_LL4F3:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp973=t;struct Cyc_Core_Opt*
_tmp974;struct Cyc_Core_Opt*_tmp975;_LL5E2: if(_tmp973 <= (void*)4)goto _LL5E4;if(*((
int*)_tmp973)!= 0)goto _LL5E4;_tmp974=((struct Cyc_Absyn_Evar_struct*)_tmp973)->f1;
_tmp975=((struct Cyc_Absyn_Evar_struct*)_tmp973)->f2;_LL5E3: {struct
_dynforward_ptr s;{struct Cyc_Core_Opt*_tmp976=_tmp974;struct Cyc_Core_Opt _tmp977;
void*_tmp978;_LL5E7: if(_tmp976 != 0)goto _LL5E9;_LL5E8: s=({const char*_tmp979="kind=NULL ";
_tag_dynforward(_tmp979,sizeof(char),_get_zero_arr_size(_tmp979,11));});goto
_LL5E6;_LL5E9: if(_tmp976 == 0)goto _LL5E6;_tmp977=*_tmp976;_tmp978=(void*)_tmp977.v;
_LL5EA: s=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp97C;_tmp97C.tag=
0;_tmp97C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp978));{void*_tmp97A[1]={& _tmp97C};Cyc_aprintf(({const char*_tmp97B="kind=%s ";
_tag_dynforward(_tmp97B,sizeof(char),_get_zero_arr_size(_tmp97B,9));}),
_tag_dynforward(_tmp97A,sizeof(void*),1));}});goto _LL5E6;_LL5E6:;}{struct Cyc_Core_Opt*
_tmp97D=_tmp975;struct Cyc_Core_Opt _tmp97E;void*_tmp97F;_LL5EC: if(_tmp97D != 0)
goto _LL5EE;_LL5ED: s=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp982;
_tmp982.tag=0;_tmp982.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*
_tmp980[1]={& _tmp982};Cyc_aprintf(({const char*_tmp981="%s ref=NULL";
_tag_dynforward(_tmp981,sizeof(char),_get_zero_arr_size(_tmp981,12));}),
_tag_dynforward(_tmp980,sizeof(void*),1));}});goto _LL5EB;_LL5EE: if(_tmp97D == 0)
goto _LL5EB;_tmp97E=*_tmp97D;_tmp97F=(void*)_tmp97E.v;_LL5EF: s=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp986;_tmp986.tag=0;_tmp986.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp97F));{
struct Cyc_String_pa_struct _tmp985;_tmp985.tag=0;_tmp985.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp983[2]={& _tmp985,& _tmp986};
Cyc_aprintf(({const char*_tmp984="%s ref=%s";_tag_dynforward(_tmp984,sizeof(char),
_get_zero_arr_size(_tmp984,10));}),_tag_dynforward(_tmp983,sizeof(void*),2));}}});
goto _LL5EB;_LL5EB:;}({struct Cyc_String_pa_struct _tmp989;_tmp989.tag=0;_tmp989.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp987[1]={& _tmp989};Cyc_fprintf(
Cyc_stderr,({const char*_tmp988="evar info: %s\n";_tag_dynforward(_tmp988,sizeof(
char),_get_zero_arr_size(_tmp988,15));}),_tag_dynforward(_tmp987,sizeof(void*),1));}});
goto _LL5E1;}_LL5E4:;_LL5E5: goto _LL5E1;_LL5E1:;}({struct Cyc_String_pa_struct
_tmp98E;_tmp98E.tag=0;_tmp98E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(expected_kind));{struct Cyc_String_pa_struct _tmp98D;
_tmp98D.tag=0;_tmp98D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct _tmp98C;_tmp98C.tag=0;
_tmp98C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp98A[3]={& _tmp98C,& _tmp98D,& _tmp98E};Cyc_Tcutil_terr(loc,({const
char*_tmp98B="type %s has kind %s but as used here needs kind %s";
_tag_dynforward(_tmp98B,sizeof(char),_get_zero_arr_size(_tmp98B,51));}),
_tag_dynforward(_tmp98A,sizeof(void*),3));}}}});}return cvtenv;}static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvt,void*expected_kind,void*t){struct Cyc_List_List*
_tmp98F=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,
t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*
_tmp991;struct _tuple18 _tmp990=*((struct _tuple18*)vs->hd);_tmp991=_tmp990.f1;cvt.kind_env=
Cyc_Tcutil_fast_add_free_tvar(_tmp98F,_tmp991);}}{struct Cyc_List_List*evs=cvt.free_evars;
for(0;evs != 0;evs=evs->tl){void*_tmp993;struct _tuple19 _tmp992=*((struct _tuple19*)
evs->hd);_tmp993=_tmp992.f1;{void*_tmp994=Cyc_Tcutil_compress(_tmp993);struct Cyc_Core_Opt*
_tmp995;struct Cyc_Core_Opt**_tmp996;_LL5F1: if(_tmp994 <= (void*)4)goto _LL5F3;if(*((
int*)_tmp994)!= 0)goto _LL5F3;_tmp995=((struct Cyc_Absyn_Evar_struct*)_tmp994)->f4;
_tmp996=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp994)->f4;
_LL5F2: if(*_tmp996 == 0)*_tmp996=({struct Cyc_Core_Opt*_tmp997=_cycalloc(sizeof(*
_tmp997));_tmp997->v=_tmp98F;_tmp997;});else{struct Cyc_List_List*_tmp998=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp996))->v;struct Cyc_List_List*
_tmp999=0;for(0;_tmp998 != 0;_tmp998=_tmp998->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp98F,(struct Cyc_Absyn_Tvar*)_tmp998->hd))
_tmp999=({struct Cyc_List_List*_tmp99A=_cycalloc(sizeof(*_tmp99A));_tmp99A->hd=(
struct Cyc_Absyn_Tvar*)_tmp998->hd;_tmp99A->tl=_tmp999;_tmp99A;});}*_tmp996=({
struct Cyc_Core_Opt*_tmp99B=_cycalloc(sizeof(*_tmp99B));_tmp99B->v=_tmp999;
_tmp99B;});}goto _LL5F0;_LL5F3:;_LL5F4: goto _LL5F0;_LL5F0:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmp99C=Cyc_Tcenv_lookup_type_vars(
te);{struct _RegionHandle _tmp99D=_new_region("temp");struct _RegionHandle*temp=&
_tmp99D;_push_region(temp);{struct Cyc_Tcutil_CVTEnv _tmp99E=Cyc_Tcutil_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9DB;_tmp9DB.r=temp;_tmp9DB.kind_env=_tmp99C;
_tmp9DB.free_vars=0;_tmp9DB.free_evars=0;_tmp9DB.generalize_evars=
generalize_evars;_tmp9DB.fn_result=0;_tmp9DB;}),(void*)1,t);struct Cyc_List_List*
_tmp99F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp99E.free_vars);struct Cyc_List_List*_tmp9A0=((
struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),struct
Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp99E.free_evars);if(_tmp99C != 0){struct Cyc_List_List*_tmp9A1=0;{struct Cyc_List_List*
_tmp9A2=_tmp99F;for(0;(unsigned int)_tmp9A2;_tmp9A2=_tmp9A2->tl){struct Cyc_Absyn_Tvar*
_tmp9A3=(struct Cyc_Absyn_Tvar*)_tmp9A2->hd;int found=0;{struct Cyc_List_List*
_tmp9A4=_tmp99C;for(0;(unsigned int)_tmp9A4;_tmp9A4=_tmp9A4->tl){if(Cyc_Absyn_tvar_cmp(
_tmp9A3,(struct Cyc_Absyn_Tvar*)_tmp9A4->hd)== 0){found=1;break;}}}if(!found)
_tmp9A1=({struct Cyc_List_List*_tmp9A5=_region_malloc(temp,sizeof(*_tmp9A5));
_tmp9A5->hd=(struct Cyc_Absyn_Tvar*)_tmp9A2->hd;_tmp9A5->tl=_tmp9A1;_tmp9A5;});}}
_tmp99F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp9A1);}{struct Cyc_List_List*x=_tmp99F;for(0;x != 0;x=x->tl){void*_tmp9A6=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp9A7;struct Cyc_Core_Opt**
_tmp9A8;struct Cyc_Core_Opt*_tmp9A9;struct Cyc_Core_Opt**_tmp9AA;void*_tmp9AB;
struct Cyc_Core_Opt*_tmp9AC;struct Cyc_Core_Opt**_tmp9AD;void*_tmp9AE;struct Cyc_Core_Opt*
_tmp9AF;struct Cyc_Core_Opt**_tmp9B0;void*_tmp9B1;struct Cyc_Core_Opt*_tmp9B2;
struct Cyc_Core_Opt**_tmp9B3;void*_tmp9B4;void*_tmp9B5;_LL5F6: if(*((int*)_tmp9A6)
!= 1)goto _LL5F8;_tmp9A7=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp9A6)->f1;
_tmp9A8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp9A6)->f1;
_LL5F7: _tmp9AA=_tmp9A8;goto _LL5F9;_LL5F8: if(*((int*)_tmp9A6)!= 2)goto _LL5FA;
_tmp9A9=((struct Cyc_Absyn_Less_kb_struct*)_tmp9A6)->f1;_tmp9AA=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp9A6)->f1;_tmp9AB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp9A6)->f2;if((int)_tmp9AB != 1)goto _LL5FA;_LL5F9: _tmp9AD=_tmp9AA;goto _LL5FB;
_LL5FA: if(*((int*)_tmp9A6)!= 2)goto _LL5FC;_tmp9AC=((struct Cyc_Absyn_Less_kb_struct*)
_tmp9A6)->f1;_tmp9AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp9A6)->f1;_tmp9AE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp9A6)->f2;if((
int)_tmp9AE != 0)goto _LL5FC;_LL5FB:*_tmp9AD=({struct Cyc_Core_Opt*_tmp9B6=
_cycalloc(sizeof(*_tmp9B6));_tmp9B6->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp9B7=_cycalloc(sizeof(*_tmp9B7));_tmp9B7[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp9B8;_tmp9B8.tag=0;_tmp9B8.f1=(void*)((void*)2);_tmp9B8;});_tmp9B7;}));
_tmp9B6;});goto _LL5F5;_LL5FC: if(*((int*)_tmp9A6)!= 2)goto _LL5FE;_tmp9AF=((struct
Cyc_Absyn_Less_kb_struct*)_tmp9A6)->f1;_tmp9B0=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp9A6)->f1;_tmp9B1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp9A6)->f2;if((int)_tmp9B1 != 5)goto _LL5FE;_LL5FD:*_tmp9B0=({struct Cyc_Core_Opt*
_tmp9B9=_cycalloc(sizeof(*_tmp9B9));_tmp9B9->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp9BA=_cycalloc(sizeof(*_tmp9BA));_tmp9BA[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp9BB;_tmp9BB.tag=0;_tmp9BB.f1=(void*)((void*)3);_tmp9BB;});_tmp9BA;}));
_tmp9B9;});goto _LL5F5;_LL5FE: if(*((int*)_tmp9A6)!= 2)goto _LL600;_tmp9B2=((struct
Cyc_Absyn_Less_kb_struct*)_tmp9A6)->f1;_tmp9B3=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp9A6)->f1;_tmp9B4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp9A6)->f2;_LL5FF:*_tmp9B3=({struct Cyc_Core_Opt*_tmp9BC=_cycalloc(sizeof(*
_tmp9BC));_tmp9BC->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp9BD=
_cycalloc(sizeof(*_tmp9BD));_tmp9BD[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp9BE;
_tmp9BE.tag=0;_tmp9BE.f1=(void*)_tmp9B4;_tmp9BE;});_tmp9BD;}));_tmp9BC;});goto
_LL5F5;_LL600: if(*((int*)_tmp9A6)!= 0)goto _LL602;_tmp9B5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp9A6)->f1;if((int)_tmp9B5 != 1)goto _LL602;_LL601:({struct Cyc_String_pa_struct
_tmp9C1;_tmp9C1.tag=0;_tmp9C1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp9BF[1]={& _tmp9C1};
Cyc_Tcutil_terr(loc,({const char*_tmp9C0="type variable %s cannot have kind M";
_tag_dynforward(_tmp9C0,sizeof(char),_get_zero_arr_size(_tmp9C0,36));}),
_tag_dynforward(_tmp9BF,sizeof(void*),1));}});goto _LL5F5;_LL602:;_LL603: goto
_LL5F5;_LL5F5:;}}if(_tmp99F != 0  || _tmp9A0 != 0){{void*_tmp9C2=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp9C3;struct Cyc_List_List*_tmp9C4;struct Cyc_List_List**
_tmp9C5;struct Cyc_Core_Opt*_tmp9C6;void*_tmp9C7;struct Cyc_List_List*_tmp9C8;int
_tmp9C9;struct Cyc_Absyn_VarargInfo*_tmp9CA;struct Cyc_List_List*_tmp9CB;struct Cyc_List_List*
_tmp9CC;_LL605: if(_tmp9C2 <= (void*)4)goto _LL607;if(*((int*)_tmp9C2)!= 8)goto
_LL607;_tmp9C3=((struct Cyc_Absyn_FnType_struct*)_tmp9C2)->f1;_tmp9C4=_tmp9C3.tvars;
_tmp9C5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp9C2)->f1).tvars;
_tmp9C6=_tmp9C3.effect;_tmp9C7=(void*)_tmp9C3.ret_typ;_tmp9C8=_tmp9C3.args;
_tmp9C9=_tmp9C3.c_varargs;_tmp9CA=_tmp9C3.cyc_varargs;_tmp9CB=_tmp9C3.rgn_po;
_tmp9CC=_tmp9C3.attributes;_LL606: if(*_tmp9C5 == 0){*_tmp9C5=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmp99F);_tmp99F=0;}goto _LL604;_LL607:;
_LL608: goto _LL604;_LL604:;}if(_tmp99F != 0)({struct Cyc_String_pa_struct _tmp9CF;
_tmp9CF.tag=0;_tmp9CF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)_tmp99F->hd)->name);{void*_tmp9CD[1]={& _tmp9CF};Cyc_Tcutil_terr(
loc,({const char*_tmp9CE="unbound type variable %s";_tag_dynforward(_tmp9CE,
sizeof(char),_get_zero_arr_size(_tmp9CE,25));}),_tag_dynforward(_tmp9CD,sizeof(
void*),1));}});if(_tmp9A0 != 0)for(0;_tmp9A0 != 0;_tmp9A0=_tmp9A0->tl){void*e=(
void*)_tmp9A0->hd;void*_tmp9D0=Cyc_Tcutil_typ_kind(e);_LL60A: if((int)_tmp9D0 != 4)
goto _LL60C;_LL60B: if(!Cyc_Tcutil_unify(e,(void*)3))({void*_tmp9D1[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9D2="can't unify evar with unique region!";_tag_dynforward(_tmp9D2,sizeof(
char),_get_zero_arr_size(_tmp9D2,37));}),_tag_dynforward(_tmp9D1,sizeof(void*),0));});
goto _LL609;_LL60C: if((int)_tmp9D0 != 5)goto _LL60E;_LL60D: goto _LL60F;_LL60E: if((
int)_tmp9D0 != 3)goto _LL610;_LL60F: if(!Cyc_Tcutil_unify(e,(void*)2))({void*
_tmp9D3[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9D4="can't unify evar with heap!";_tag_dynforward(_tmp9D4,sizeof(
char),_get_zero_arr_size(_tmp9D4,28));}),_tag_dynforward(_tmp9D3,sizeof(void*),0));});
goto _LL609;_LL610: if((int)_tmp9D0 != 6)goto _LL612;_LL611: if(!Cyc_Tcutil_unify(e,
Cyc_Absyn_empty_effect))({void*_tmp9D5[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9D6="can't unify evar with {}!";
_tag_dynforward(_tmp9D6,sizeof(char),_get_zero_arr_size(_tmp9D6,26));}),
_tag_dynforward(_tmp9D5,sizeof(void*),0));});goto _LL609;_LL612:;_LL613:({struct
Cyc_String_pa_struct _tmp9DA;_tmp9DA.tag=0;_tmp9DA.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp9D9;_tmp9D9.tag=0;_tmp9D9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp9D7[2]={& _tmp9D9,& _tmp9DA};Cyc_Tcutil_terr(
loc,({const char*_tmp9D8="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp9D8,sizeof(char),_get_zero_arr_size(_tmp9D8,52));}),
_tag_dynforward(_tmp9D7,sizeof(void*),2));}}});goto _LL609;_LL609:;}}};
_pop_region(temp);}if(Cyc_Tcutil_is_unique_pointer(t))({void*_tmp9DC[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp9DD="unique pointers not permitted at the toplevel";
_tag_dynforward(_tmp9DD,sizeof(char),_get_zero_arr_size(_tmp9DD,46));}),
_tag_dynforward(_tmp9DC,sizeof(void*),0));});}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp9DE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp9DF;struct Cyc_List_List*
_tmp9E0;struct Cyc_Core_Opt*_tmp9E1;void*_tmp9E2;_LL615: if(_tmp9DE <= (void*)4)
goto _LL617;if(*((int*)_tmp9DE)!= 8)goto _LL617;_tmp9DF=((struct Cyc_Absyn_FnType_struct*)
_tmp9DE)->f1;_tmp9E0=_tmp9DF.tvars;_tmp9E1=_tmp9DF.effect;_tmp9E2=(void*)_tmp9DF.ret_typ;
_LL616: fd->tvs=_tmp9E0;fd->effect=_tmp9E1;goto _LL614;_LL617:;_LL618:({void*
_tmp9E3[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9E4="check_fndecl_valid_type: not a FnType";_tag_dynforward(
_tmp9E4,sizeof(char),_get_zero_arr_size(_tmp9E4,38));}),_tag_dynforward(_tmp9E3,
sizeof(void*),0));});_LL614:;}{struct _RegionHandle _tmp9E5=_new_region("r");
struct _RegionHandle*r=& _tmp9E5;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr*(*f)(struct
_tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dynforward_ptr*(*)(
struct _tuple14*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9E6="function declaration has repeated parameter";
_tag_dynforward(_tmp9E6,sizeof(char),_get_zero_arr_size(_tmp9E6,44));}));;
_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp9E7=_cycalloc(sizeof(*
_tmp9E7));_tmp9E7->v=(void*)t;_tmp9E7;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,
int allow_evars,void*t){struct _RegionHandle _tmp9E8=_new_region("r");struct
_RegionHandle*r=& _tmp9E8;_push_region(r);{struct Cyc_Tcutil_CVTEnv _tmp9E9=Cyc_Tcutil_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9FC;_tmp9FC.r=r;_tmp9FC.kind_env=bound_tvars;
_tmp9FC.free_vars=0;_tmp9FC.free_evars=0;_tmp9FC.generalize_evars=0;_tmp9FC.fn_result=
0;_tmp9FC;}),expected_kind,t);struct Cyc_List_List*_tmp9EA=Cyc_Tcutil_remove_bound_tvars(
r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp9E9.free_vars),bound_tvars);struct Cyc_List_List*
_tmp9EB=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp9E9.free_evars);{struct Cyc_List_List*fs=_tmp9EA;for(0;fs != 0;fs=fs->tl){
struct _dynforward_ptr*_tmp9EC=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct
_tmp9F0;_tmp9F0.tag=0;_tmp9F0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp9EF;_tmp9EF.tag=0;
_tmp9EF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp9EC);{void*
_tmp9ED[2]={& _tmp9EF,& _tmp9F0};Cyc_Tcutil_terr(loc,({const char*_tmp9EE="unbound type variable %s in type %s";
_tag_dynforward(_tmp9EE,sizeof(char),_get_zero_arr_size(_tmp9EE,36));}),
_tag_dynforward(_tmp9ED,sizeof(void*),2));}}});}}if(!allow_evars  && _tmp9EB != 0)
for(0;_tmp9EB != 0;_tmp9EB=_tmp9EB->tl){void*e=(void*)_tmp9EB->hd;void*_tmp9F1=
Cyc_Tcutil_typ_kind(e);_LL61A: if((int)_tmp9F1 != 4)goto _LL61C;_LL61B: if(!Cyc_Tcutil_unify(
e,(void*)3))({void*_tmp9F2[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9F3="can't unify evar with unique region!";
_tag_dynforward(_tmp9F3,sizeof(char),_get_zero_arr_size(_tmp9F3,37));}),
_tag_dynforward(_tmp9F2,sizeof(void*),0));});goto _LL619;_LL61C: if((int)_tmp9F1 != 
5)goto _LL61E;_LL61D: goto _LL61F;_LL61E: if((int)_tmp9F1 != 3)goto _LL620;_LL61F: if(!
Cyc_Tcutil_unify(e,(void*)2))({void*_tmp9F4[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9F5="can't unify evar with heap!";
_tag_dynforward(_tmp9F5,sizeof(char),_get_zero_arr_size(_tmp9F5,28));}),
_tag_dynforward(_tmp9F4,sizeof(void*),0));});goto _LL619;_LL620: if((int)_tmp9F1 != 
6)goto _LL622;_LL621: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*
_tmp9F6[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9F7="can't unify evar with {}!";_tag_dynforward(_tmp9F7,sizeof(
char),_get_zero_arr_size(_tmp9F7,26));}),_tag_dynforward(_tmp9F6,sizeof(void*),0));});
goto _LL619;_LL622:;_LL623:({struct Cyc_String_pa_struct _tmp9FB;_tmp9FB.tag=0;
_tmp9FB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp9FA;_tmp9FA.tag=0;_tmp9FA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(e));{void*_tmp9F8[
2]={& _tmp9FA,& _tmp9FB};Cyc_Tcutil_terr(loc,({const char*_tmp9F9="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp9F9,sizeof(char),_get_zero_arr_size(_tmp9F9,52));}),
_tag_dynforward(_tmp9F8,sizeof(void*),2));}}});goto _LL619;_LL619:;}};_pop_region(
r);}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 
0)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dynforward_ptr(*a2string)(void*),struct _dynforward_ptr msg){for(0;vs != 0;
vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((
void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct _tmpA00;_tmpA00.tag=
0;_tmpA00.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)a2string((void*)vs->hd));{
struct Cyc_String_pa_struct _tmp9FF;_tmp9FF.tag=0;_tmp9FF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)msg);{void*_tmp9FD[2]={& _tmp9FF,& _tmpA00};
Cyc_Tcutil_terr(loc,({const char*_tmp9FE="%s: %s";_tag_dynforward(_tmp9FE,sizeof(
char),_get_zero_arr_size(_tmp9FE,7));}),_tag_dynforward(_tmp9FD,sizeof(void*),2));}}});}}}
static struct _dynforward_ptr Cyc_Tcutil_strptr2string(struct _dynforward_ptr*s){
return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr msg){((void(*)(int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr(*a2string)(struct _dynforward_ptr*),struct _dynforward_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct
_dynforward_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,
Cyc_Tcutil_tvar2string,({const char*_tmpA01="duplicate type variable";
_tag_dynforward(_tmpA01,sizeof(char),_get_zero_arr_size(_tmpA01,24));}));}struct
_tuple20{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple21{struct Cyc_List_List*
f1;void*f2;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,({const char*_tmpA02="";_tag_dynforward(_tmpA02,sizeof(char),
_get_zero_arr_size(_tmpA02,1));}))!= 0)fields=({struct Cyc_List_List*_tmpA03=
_cycalloc(sizeof(*_tmpA03));_tmpA03->hd=({struct _tuple20*_tmpA04=_cycalloc(
sizeof(*_tmpA04));_tmpA04->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmpA04->f2=
0;_tmpA04;});_tmpA03->tl=fields;_tmpA03;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple21 _tmpA06;struct Cyc_List_List*_tmpA07;void*
_tmpA08;struct _tuple21*_tmpA05=(struct _tuple21*)des->hd;_tmpA06=*_tmpA05;_tmpA07=
_tmpA06.f1;_tmpA08=_tmpA06.f2;if(_tmpA07 == 0){struct Cyc_List_List*_tmpA09=fields;
for(0;_tmpA09 != 0;_tmpA09=_tmpA09->tl){if(!(*((struct _tuple20*)_tmpA09->hd)).f2){(*((
struct _tuple20*)_tmpA09->hd)).f2=1;(*((struct _tuple21*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmpA0A=_cycalloc(sizeof(*_tmpA0A));_tmpA0A->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmpA0B=_cycalloc(sizeof(*_tmpA0B));
_tmpA0B[0]=({struct Cyc_Absyn_FieldName_struct _tmpA0C;_tmpA0C.tag=1;_tmpA0C.f1=((*((
struct _tuple20*)_tmpA09->hd)).f1)->name;_tmpA0C;});_tmpA0B;}));_tmpA0A->tl=0;
_tmpA0A;});ans=({struct Cyc_List_List*_tmpA0D=_region_malloc(rgn,sizeof(*_tmpA0D));
_tmpA0D->hd=({struct _tuple22*_tmpA0E=_region_malloc(rgn,sizeof(*_tmpA0E));
_tmpA0E->f1=(*((struct _tuple20*)_tmpA09->hd)).f1;_tmpA0E->f2=_tmpA08;_tmpA0E;});
_tmpA0D->tl=ans;_tmpA0D;});break;}}if(_tmpA09 == 0)({void*_tmpA0F[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmpA10="too many arguments to struct";_tag_dynforward(_tmpA10,
sizeof(char),_get_zero_arr_size(_tmpA10,29));}),_tag_dynforward(_tmpA0F,sizeof(
void*),0));});}else{if(_tmpA07->tl != 0)({void*_tmpA11[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmpA12="multiple designators are not supported";_tag_dynforward(
_tmpA12,sizeof(char),_get_zero_arr_size(_tmpA12,39));}),_tag_dynforward(_tmpA11,
sizeof(void*),0));});else{void*_tmpA13=(void*)_tmpA07->hd;struct _dynforward_ptr*
_tmpA14;_LL625: if(*((int*)_tmpA13)!= 0)goto _LL627;_LL626:({void*_tmpA15[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmpA16="array designator used in argument to struct";
_tag_dynforward(_tmpA16,sizeof(char),_get_zero_arr_size(_tmpA16,44));}),
_tag_dynforward(_tmpA15,sizeof(void*),0));});goto _LL624;_LL627: if(*((int*)
_tmpA13)!= 1)goto _LL624;_tmpA14=((struct Cyc_Absyn_FieldName_struct*)_tmpA13)->f1;
_LL628: {struct Cyc_List_List*_tmpA17=fields;for(0;_tmpA17 != 0;_tmpA17=_tmpA17->tl){
if(Cyc_strptrcmp(_tmpA14,((*((struct _tuple20*)_tmpA17->hd)).f1)->name)== 0){if((*((
struct _tuple20*)_tmpA17->hd)).f2)({struct Cyc_String_pa_struct _tmpA1A;_tmpA1A.tag=
0;_tmpA1A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmpA14);{void*
_tmpA18[1]={& _tmpA1A};Cyc_Tcutil_terr(loc,({const char*_tmpA19="field %s has already been used as an argument";
_tag_dynforward(_tmpA19,sizeof(char),_get_zero_arr_size(_tmpA19,46));}),
_tag_dynforward(_tmpA18,sizeof(void*),1));}});(*((struct _tuple20*)_tmpA17->hd)).f2=
1;ans=({struct Cyc_List_List*_tmpA1B=_region_malloc(rgn,sizeof(*_tmpA1B));_tmpA1B->hd=({
struct _tuple22*_tmpA1C=_region_malloc(rgn,sizeof(*_tmpA1C));_tmpA1C->f1=(*((
struct _tuple20*)_tmpA17->hd)).f1;_tmpA1C->f2=_tmpA08;_tmpA1C;});_tmpA1B->tl=ans;
_tmpA1B;});break;}}if(_tmpA17 == 0)({struct Cyc_String_pa_struct _tmpA1F;_tmpA1F.tag=
0;_tmpA1F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmpA14);{void*
_tmpA1D[1]={& _tmpA1F};Cyc_Tcutil_terr(loc,({const char*_tmpA1E="bad field designator %s";
_tag_dynforward(_tmpA1E,sizeof(char),_get_zero_arr_size(_tmpA1E,24));}),
_tag_dynforward(_tmpA1D,sizeof(void*),1));}});goto _LL624;}_LL624:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){({void*
_tmpA20[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpA21="too few arguments to struct";
_tag_dynforward(_tmpA21,sizeof(char),_get_zero_arr_size(_tmpA21,28));}),
_tag_dynforward(_tmpA20,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only){*forward_only=0;{void*_tmpA22=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA23;void*_tmpA24;struct Cyc_Absyn_PtrAtts _tmpA25;
struct Cyc_Absyn_Conref*_tmpA26;_LL62A: if(_tmpA22 <= (void*)4)goto _LL62C;if(*((int*)
_tmpA22)!= 4)goto _LL62C;_tmpA23=((struct Cyc_Absyn_PointerType_struct*)_tmpA22)->f1;
_tmpA24=(void*)_tmpA23.elt_typ;_tmpA25=_tmpA23.ptr_atts;_tmpA26=_tmpA25.bounds;
_LL62B: {struct Cyc_Absyn_Conref*_tmpA27=Cyc_Absyn_compress_conref(_tmpA26);union
Cyc_Absyn_Constraint_union _tmpA28=(Cyc_Absyn_compress_conref(_tmpA27))->v;void*
_tmpA29;void*_tmpA2A;_LL62F: if((_tmpA28.Eq_constr).tag != 0)goto _LL631;_tmpA29=(
_tmpA28.Eq_constr).f1;if((int)_tmpA29 != 0)goto _LL631;_LL630:*forward_only=1;*
elt_typ_dest=_tmpA24;return 1;_LL631: if((_tmpA28.Eq_constr).tag != 0)goto _LL633;
_tmpA2A=(_tmpA28.Eq_constr).f1;if((int)_tmpA2A != 1)goto _LL633;_LL632:*
elt_typ_dest=_tmpA24;return 1;_LL633: if((_tmpA28.No_constr).tag != 2)goto _LL635;
_LL634: _tmpA27->v=(union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union
_tmpA2B;(_tmpA2B.Eq_constr).tag=0;(_tmpA2B.Eq_constr).f1=(void*)((void*)0);
_tmpA2B;});*forward_only=1;*elt_typ_dest=_tmpA24;return 1;_LL635:;_LL636: return 0;
_LL62E:;}_LL62C:;_LL62D: return 0;_LL629:;}}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpA2C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpA2D;void*_tmpA2E;struct Cyc_Absyn_PtrAtts _tmpA2F;struct Cyc_Absyn_Conref*
_tmpA30;_LL638: if(_tmpA2C <= (void*)4)goto _LL63A;if(*((int*)_tmpA2C)!= 4)goto
_LL63A;_tmpA2D=((struct Cyc_Absyn_PointerType_struct*)_tmpA2C)->f1;_tmpA2E=(void*)
_tmpA2D.elt_typ;_tmpA2F=_tmpA2D.ptr_atts;_tmpA30=_tmpA2F.zero_term;_LL639:*
elt_typ_dest=_tmpA2E;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA30);_LL63A:;_LL63B: return 0;_LL637:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmpA31=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA32;void*_tmpA33;struct Cyc_Absyn_PtrAtts
_tmpA34;struct Cyc_Absyn_Conref*_tmpA35;struct Cyc_Absyn_Conref*_tmpA36;struct Cyc_Absyn_ArrayInfo
_tmpA37;void*_tmpA38;struct Cyc_Absyn_Tqual _tmpA39;struct Cyc_Absyn_Exp*_tmpA3A;
struct Cyc_Absyn_Conref*_tmpA3B;_LL63D: if(_tmpA31 <= (void*)4)goto _LL641;if(*((int*)
_tmpA31)!= 4)goto _LL63F;_tmpA32=((struct Cyc_Absyn_PointerType_struct*)_tmpA31)->f1;
_tmpA33=(void*)_tmpA32.elt_typ;_tmpA34=_tmpA32.ptr_atts;_tmpA35=_tmpA34.bounds;
_tmpA36=_tmpA34.zero_term;_LL63E: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA36)){*ptr_type=t;*elt_type=_tmpA33;{void*_tmpA3C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpA35);_LL644: if((int)_tmpA3C != 0)goto _LL646;_LL645:*is_dyneither=0;*
is_dynforward=1;goto _LL643;_LL646: if((int)_tmpA3C != 1)goto _LL648;_LL647:*
is_dynforward=0;*is_dyneither=1;goto _LL643;_LL648:;_LL649:*is_dynforward=(*
is_dyneither=0);goto _LL643;_LL643:;}return 1;}else{return 0;}_LL63F: if(*((int*)
_tmpA31)!= 7)goto _LL641;_tmpA37=((struct Cyc_Absyn_ArrayType_struct*)_tmpA31)->f1;
_tmpA38=(void*)_tmpA37.elt_type;_tmpA39=_tmpA37.tq;_tmpA3A=_tmpA37.num_elts;
_tmpA3B=_tmpA37.zero_term;_LL640: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA3B)){*elt_type=_tmpA38;*is_dynforward=(*is_dyneither=0);*ptr_type=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmpA3D=_cycalloc(sizeof(*_tmpA3D));_tmpA3D[0]=({
struct Cyc_Absyn_PointerType_struct _tmpA3E;_tmpA3E.tag=4;_tmpA3E.f1=({struct Cyc_Absyn_PtrInfo
_tmpA3F;_tmpA3F.elt_typ=(void*)_tmpA38;_tmpA3F.elt_tq=_tmpA39;_tmpA3F.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpA40;_tmpA40.rgn=(void*)((void*)2);_tmpA40.nullable=
Cyc_Absyn_false_conref;_tmpA40.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmpA41=_cycalloc(sizeof(*_tmpA41));_tmpA41[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpA42;_tmpA42.tag=0;_tmpA42.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA3A);
_tmpA42;});_tmpA41;}));_tmpA40.zero_term=_tmpA3B;_tmpA40.ptrloc=0;_tmpA40;});
_tmpA3F;});_tmpA3E;});_tmpA3D;});return 1;}else{return 0;}_LL641:;_LL642: return 0;
_LL63C:;}int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,
int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmpA43=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmpA44;struct Cyc_Absyn_Exp*_tmpA45;struct Cyc_Absyn_Exp*
_tmpA46;struct Cyc_Absyn_Exp*_tmpA47;struct Cyc_Absyn_Exp*_tmpA48;struct Cyc_Absyn_Exp*
_tmpA49;_LL64B: if(*((int*)_tmpA43)!= 15)goto _LL64D;_LL64C:({struct Cyc_String_pa_struct
_tmpA4C;_tmpA4C.tag=0;_tmpA4C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmpA4A[1]={& _tmpA4C};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA4B="we have a cast in a lhs:  %s";_tag_dynforward(_tmpA4B,sizeof(char),
_get_zero_arr_size(_tmpA4B,29));}),_tag_dynforward(_tmpA4A,sizeof(void*),1));}});
_LL64D: if(*((int*)_tmpA43)!= 22)goto _LL64F;_tmpA44=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA43)->f1;_LL64E: _tmpA45=_tmpA44;goto _LL650;_LL64F: if(*((int*)_tmpA43)!= 25)
goto _LL651;_tmpA45=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA43)->f1;_LL650:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA45->topt))->v,ptr_type,is_dynforward,is_dyneither,elt_type);_LL651: if(*((
int*)_tmpA43)!= 24)goto _LL653;_tmpA46=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpA43)->f1;_LL652: _tmpA47=_tmpA46;goto _LL654;_LL653: if(*((int*)_tmpA43)!= 23)
goto _LL655;_tmpA47=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA43)->f1;_LL654:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA47->topt))->v,
ptr_type,is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct
_tmpA4F;_tmpA4F.tag=0;_tmpA4F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmpA4D[1]={& _tmpA4F};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA4E="found zero pointer aggregate member assignment: %s";_tag_dynforward(
_tmpA4E,sizeof(char),_get_zero_arr_size(_tmpA4E,51));}),_tag_dynforward(_tmpA4D,
sizeof(void*),1));}});return 0;_LL655: if(*((int*)_tmpA43)!= 14)goto _LL657;_tmpA48=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmpA43)->f1;_LL656: _tmpA49=_tmpA48;goto
_LL658;_LL657: if(*((int*)_tmpA43)!= 13)goto _LL659;_tmpA49=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpA43)->f1;_LL658: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA49->topt))->v,ptr_type,is_dynforward,is_dyneither,elt_type))({
struct Cyc_String_pa_struct _tmpA52;_tmpA52.tag=0;_tmpA52.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e1));{void*
_tmpA50[1]={& _tmpA52};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA51="found zero pointer instantiate/noinstantiate: %s";
_tag_dynforward(_tmpA51,sizeof(char),_get_zero_arr_size(_tmpA51,49));}),
_tag_dynforward(_tmpA50,sizeof(void*),1));}});return 0;_LL659: if(*((int*)_tmpA43)
!= 1)goto _LL65B;_LL65A: return 0;_LL65B:;_LL65C:({struct Cyc_String_pa_struct
_tmpA55;_tmpA55.tag=0;_tmpA55.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmpA53[1]={& _tmpA55};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA54="found bad lhs in is_zero_ptr_deref: %s";_tag_dynforward(_tmpA54,sizeof(
char),_get_zero_arr_size(_tmpA54,39));}),_tag_dynforward(_tmpA53,sizeof(void*),1));}});
_LL64A:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t,int*is_forward_only){void*
ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore,
is_forward_only);}static int Cyc_Tcutil_is_unique_region(void*r){void*_tmpA56=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpA57;_LL65E: if((int)_tmpA56 != 3)goto _LL660;_LL65F:
return 1;_LL660: if(_tmpA56 <= (void*)4)goto _LL662;if(*((int*)_tmpA56)!= 1)goto
_LL662;_tmpA57=((struct Cyc_Absyn_VarType_struct*)_tmpA56)->f1;_LL661: return Cyc_Tcutil_tvar_kind(
_tmpA57)== (void*)4;_LL662:;_LL663: return 0;_LL65D:;}int Cyc_Tcutil_is_unique_pointer(
void*t){void*_tmpA58=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA59;
struct Cyc_Absyn_PtrAtts _tmpA5A;void*_tmpA5B;_LL665: if(_tmpA58 <= (void*)4)goto
_LL667;if(*((int*)_tmpA58)!= 4)goto _LL667;_tmpA59=((struct Cyc_Absyn_PointerType_struct*)
_tmpA58)->f1;_tmpA5A=_tmpA59.ptr_atts;_tmpA5B=(void*)_tmpA5A.rgn;_LL666: return
Cyc_Tcutil_is_unique_region(_tmpA5B);_LL667:;_LL668: return 0;_LL664:;}static int
Cyc_Tcutil_is_unique_field(void*t,struct _dynforward_ptr*f){void*_tmpA5C=t;struct
Cyc_Absyn_AggrInfo _tmpA5D;union Cyc_Absyn_AggrInfoU_union _tmpA5E;struct Cyc_Absyn_Aggrdecl**
_tmpA5F;struct Cyc_Absyn_Aggrdecl*_tmpA60;struct Cyc_List_List*_tmpA61;_LL66A: if(
_tmpA5C <= (void*)4)goto _LL66C;if(*((int*)_tmpA5C)!= 10)goto _LL66C;_tmpA5D=((
struct Cyc_Absyn_AggrType_struct*)_tmpA5C)->f1;_tmpA5E=_tmpA5D.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpA5C)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL66C;_tmpA5F=(_tmpA5E.KnownAggr).f1;_tmpA60=*_tmpA5F;_tmpA61=_tmpA5D.targs;
_LL66B: {struct Cyc_Absyn_Aggrfield*_tmpA62=Cyc_Absyn_lookup_decl_field(_tmpA60,f);
struct Cyc_Absyn_Aggrfield*_tmpA63=_tmpA62;struct Cyc_Absyn_Aggrfield _tmpA64;void*
_tmpA65;_LL66F: if(_tmpA63 != 0)goto _LL671;_LL670:({void*_tmpA66[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA67="is_unique_field: missing field";_tag_dynforward(_tmpA67,sizeof(char),
_get_zero_arr_size(_tmpA67,31));}),_tag_dynforward(_tmpA66,sizeof(void*),0));});
_LL671: if(_tmpA63 == 0)goto _LL66E;_tmpA64=*_tmpA63;_tmpA65=(void*)_tmpA64.type;
_LL672: return Cyc_Tcutil_is_unique_pointer(_tmpA65);_LL66E:;}_LL66C:;_LL66D:({
void*_tmpA68[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA69="is_unique_field: invalid type";
_tag_dynforward(_tmpA69,sizeof(char),_get_zero_arr_size(_tmpA69,30));}),
_tag_dynforward(_tmpA68,sizeof(void*),0));});_LL669:;}int Cyc_Tcutil_is_unique_path(
struct Cyc_Absyn_Exp*e){void*_tmpA6A=(void*)e->r;struct Cyc_Absyn_Exp*_tmpA6B;
struct Cyc_Absyn_Exp*_tmpA6C;struct Cyc_Absyn_Exp*_tmpA6D;struct Cyc_Absyn_Exp*
_tmpA6E;void*_tmpA6F;struct Cyc_Absyn_Exp*_tmpA70;struct Cyc_Absyn_Exp*_tmpA71;
struct Cyc_Absyn_Stmt*_tmpA72;_LL674: if(*((int*)_tmpA6A)!= 0)goto _LL676;_LL675:
goto _LL677;_LL676: if(*((int*)_tmpA6A)!= 5)goto _LL678;_LL677: goto _LL679;_LL678:
if(*((int*)_tmpA6A)!= 7)goto _LL67A;_LL679: goto _LL67B;_LL67A: if(*((int*)_tmpA6A)
!= 8)goto _LL67C;_LL67B: goto _LL67D;_LL67C: if(*((int*)_tmpA6A)!= 12)goto _LL67E;
_LL67D: goto _LL67F;_LL67E: if(*((int*)_tmpA6A)!= 16)goto _LL680;_LL67F: goto _LL681;
_LL680: if(*((int*)_tmpA6A)!= 18)goto _LL682;_LL681: goto _LL683;_LL682: if(*((int*)
_tmpA6A)!= 19)goto _LL684;_LL683: goto _LL685;_LL684: if(*((int*)_tmpA6A)!= 20)goto
_LL686;_LL685: goto _LL687;_LL686: if(*((int*)_tmpA6A)!= 21)goto _LL688;_LL687: goto
_LL689;_LL688: if(*((int*)_tmpA6A)!= 26)goto _LL68A;_LL689: goto _LL68B;_LL68A: if(*((
int*)_tmpA6A)!= 27)goto _LL68C;_LL68B: goto _LL68D;_LL68C: if(*((int*)_tmpA6A)!= 29)
goto _LL68E;_LL68D: goto _LL68F;_LL68E: if(*((int*)_tmpA6A)!= 30)goto _LL690;_LL68F:
goto _LL691;_LL690: if(*((int*)_tmpA6A)!= 31)goto _LL692;_LL691: goto _LL693;_LL692:
if(*((int*)_tmpA6A)!= 28)goto _LL694;_LL693: goto _LL695;_LL694: if(*((int*)_tmpA6A)
!= 32)goto _LL696;_LL695: goto _LL697;_LL696: if(*((int*)_tmpA6A)!= 33)goto _LL698;
_LL697: goto _LL699;_LL698: if(*((int*)_tmpA6A)!= 34)goto _LL69A;_LL699: goto _LL69B;
_LL69A: if(*((int*)_tmpA6A)!= 36)goto _LL69C;_LL69B: goto _LL69D;_LL69C: if(*((int*)
_tmpA6A)!= 24)goto _LL69E;_LL69D: goto _LL69F;_LL69E: if(*((int*)_tmpA6A)!= 23)goto
_LL6A0;_LL69F: goto _LL6A1;_LL6A0: if(*((int*)_tmpA6A)!= 25)goto _LL6A2;_LL6A1: goto
_LL6A3;_LL6A2: if(*((int*)_tmpA6A)!= 3)goto _LL6A4;_LL6A3: return 0;_LL6A4: if(*((int*)
_tmpA6A)!= 35)goto _LL6A6;_LL6A5: goto _LL6A7;_LL6A6: if(*((int*)_tmpA6A)!= 17)goto
_LL6A8;_LL6A7: goto _LL6A9;_LL6A8: if(*((int*)_tmpA6A)!= 1)goto _LL6AA;_LL6A9: return
Cyc_Tcutil_is_unique_pointer((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL6AA: if(*((int*)_tmpA6A)!= 6)goto _LL6AC;_tmpA6B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA6A)->f2;_LL6AB: _tmpA6C=_tmpA6B;goto _LL6AD;_LL6AC: if(*((int*)_tmpA6A)!= 9)
goto _LL6AE;_tmpA6C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA6A)->f2;_LL6AD:
_tmpA6D=_tmpA6C;goto _LL6AF;_LL6AE: if(*((int*)_tmpA6A)!= 4)goto _LL6B0;_tmpA6D=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpA6A)->f1;_LL6AF: return Cyc_Tcutil_is_unique_path(
_tmpA6D);_LL6B0: if(*((int*)_tmpA6A)!= 11)goto _LL6B2;_tmpA6E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpA6A)->f1;_LL6B1: {void*_tmpA73=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA6E->topt))->v);struct Cyc_Absyn_FnInfo _tmpA74;void*_tmpA75;
struct Cyc_Absyn_PtrInfo _tmpA76;void*_tmpA77;_LL6BB: if(_tmpA73 <= (void*)4)goto
_LL6BF;if(*((int*)_tmpA73)!= 8)goto _LL6BD;_tmpA74=((struct Cyc_Absyn_FnType_struct*)
_tmpA73)->f1;_tmpA75=(void*)_tmpA74.ret_typ;_LL6BC: return Cyc_Tcutil_is_unique_pointer(
_tmpA75);_LL6BD: if(*((int*)_tmpA73)!= 4)goto _LL6BF;_tmpA76=((struct Cyc_Absyn_PointerType_struct*)
_tmpA73)->f1;_tmpA77=(void*)_tmpA76.elt_typ;_LL6BE:{void*_tmpA78=Cyc_Tcutil_compress(
_tmpA77);struct Cyc_Absyn_FnInfo _tmpA79;void*_tmpA7A;_LL6C2: if(_tmpA78 <= (void*)4)
goto _LL6C4;if(*((int*)_tmpA78)!= 8)goto _LL6C4;_tmpA79=((struct Cyc_Absyn_FnType_struct*)
_tmpA78)->f1;_tmpA7A=(void*)_tmpA79.ret_typ;_LL6C3: return Cyc_Tcutil_is_unique_pointer(
_tmpA7A);_LL6C4:;_LL6C5: goto _LL6C1;_LL6C1:;}goto _LL6C0;_LL6BF:;_LL6C0:({struct
Cyc_String_pa_struct _tmpA7D;_tmpA7D.tag=0;_tmpA7D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA6E->topt))->v));{void*_tmpA7B[1]={& _tmpA7D};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA7C="Fncall has non function type %s";_tag_dynforward(_tmpA7C,sizeof(char),
_get_zero_arr_size(_tmpA7C,32));}),_tag_dynforward(_tmpA7B,sizeof(void*),1));}});
_LL6BA:;}_LL6B2: if(*((int*)_tmpA6A)!= 15)goto _LL6B4;_tmpA6F=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpA6A)->f1;_tmpA70=((struct Cyc_Absyn_Cast_e_struct*)_tmpA6A)->f2;_LL6B3: return
Cyc_Tcutil_is_unique_path(_tmpA70) && Cyc_Tcutil_is_unique_pointer(_tmpA6F);
_LL6B4: if(*((int*)_tmpA6A)!= 22)goto _LL6B6;_tmpA71=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA6A)->f1;_LL6B5: return Cyc_Tcutil_is_unique_path(_tmpA71) && Cyc_Tcutil_is_unique_pointer((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6B6: if(*((int*)_tmpA6A)
!= 38)goto _LL6B8;_tmpA72=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpA6A)->f1;_LL6B7:
while(1){void*_tmpA7E=(void*)_tmpA72->r;struct Cyc_Absyn_Stmt*_tmpA7F;struct Cyc_Absyn_Stmt*
_tmpA80;struct Cyc_Absyn_Decl*_tmpA81;struct Cyc_Absyn_Stmt*_tmpA82;struct Cyc_Absyn_Exp*
_tmpA83;_LL6C7: if(_tmpA7E <= (void*)1)goto _LL6CD;if(*((int*)_tmpA7E)!= 1)goto
_LL6C9;_tmpA7F=((struct Cyc_Absyn_Seq_s_struct*)_tmpA7E)->f1;_tmpA80=((struct Cyc_Absyn_Seq_s_struct*)
_tmpA7E)->f2;_LL6C8: _tmpA72=_tmpA80;goto _LL6C6;_LL6C9: if(*((int*)_tmpA7E)!= 11)
goto _LL6CB;_tmpA81=((struct Cyc_Absyn_Decl_s_struct*)_tmpA7E)->f1;_tmpA82=((
struct Cyc_Absyn_Decl_s_struct*)_tmpA7E)->f2;_LL6CA: _tmpA72=_tmpA82;goto _LL6C6;
_LL6CB: if(*((int*)_tmpA7E)!= 0)goto _LL6CD;_tmpA83=((struct Cyc_Absyn_Exp_s_struct*)
_tmpA7E)->f1;_LL6CC: return Cyc_Tcutil_is_unique_path(_tmpA83);_LL6CD:;_LL6CE:({
void*_tmpA84[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA85="is_unique_stmt_exp: ill-formed StmtExp";
_tag_dynforward(_tmpA85,sizeof(char),_get_zero_arr_size(_tmpA85,39));}),
_tag_dynforward(_tmpA84,sizeof(void*),0));});_LL6C6:;}_LL6B8:;_LL6B9:({struct Cyc_String_pa_struct
_tmpA88;_tmpA88.tag=0;_tmpA88.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmpA86[1]={& _tmpA88};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA87="Called is_unique_path with bogus exp |%s|\n";_tag_dynforward(_tmpA87,
sizeof(char),_get_zero_arr_size(_tmpA87,43));}),_tag_dynforward(_tmpA86,sizeof(
void*),1));}});_LL673:;}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmpACC;
_tmpACC.f1=0;_tmpACC.f2=(void*)2;_tmpACC;});void*_tmpA89=(void*)e->r;struct
_tuple2*_tmpA8A;void*_tmpA8B;struct Cyc_Absyn_Exp*_tmpA8C;struct _dynforward_ptr*
_tmpA8D;struct Cyc_Absyn_Exp*_tmpA8E;struct _dynforward_ptr*_tmpA8F;struct Cyc_Absyn_Exp*
_tmpA90;struct Cyc_Absyn_Exp*_tmpA91;struct Cyc_Absyn_Exp*_tmpA92;_LL6D0: if(*((int*)
_tmpA89)!= 1)goto _LL6D2;_tmpA8A=((struct Cyc_Absyn_Var_e_struct*)_tmpA89)->f1;
_tmpA8B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA89)->f2;_LL6D1: {void*
_tmpA93=_tmpA8B;struct Cyc_Absyn_Vardecl*_tmpA94;struct Cyc_Absyn_Vardecl*_tmpA95;
struct Cyc_Absyn_Vardecl*_tmpA96;struct Cyc_Absyn_Vardecl*_tmpA97;_LL6DD: if((int)
_tmpA93 != 0)goto _LL6DF;_LL6DE: goto _LL6E0;_LL6DF: if(_tmpA93 <= (void*)1)goto _LL6E1;
if(*((int*)_tmpA93)!= 1)goto _LL6E1;_LL6E0: return bogus_ans;_LL6E1: if(_tmpA93 <= (
void*)1)goto _LL6E3;if(*((int*)_tmpA93)!= 0)goto _LL6E3;_tmpA94=((struct Cyc_Absyn_Global_b_struct*)
_tmpA93)->f1;_LL6E2: {void*_tmpA98=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL6EA: if(_tmpA98 <= (void*)4)goto _LL6EC;if(*((int*)
_tmpA98)!= 7)goto _LL6EC;_LL6EB: return({struct _tuple10 _tmpA99;_tmpA99.f1=1;
_tmpA99.f2=(void*)2;_tmpA99;});_LL6EC:;_LL6ED: return({struct _tuple10 _tmpA9A;
_tmpA9A.f1=(_tmpA94->tq).real_const;_tmpA9A.f2=(void*)2;_tmpA9A;});_LL6E9:;}
_LL6E3: if(_tmpA93 <= (void*)1)goto _LL6E5;if(*((int*)_tmpA93)!= 3)goto _LL6E5;
_tmpA95=((struct Cyc_Absyn_Local_b_struct*)_tmpA93)->f1;_LL6E4: {void*_tmpA9B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6EF: if(_tmpA9B <= (void*)
4)goto _LL6F1;if(*((int*)_tmpA9B)!= 7)goto _LL6F1;_LL6F0: return({struct _tuple10
_tmpA9C;_tmpA9C.f1=1;_tmpA9C.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpA95->rgn))->v;
_tmpA9C;});_LL6F1:;_LL6F2: _tmpA95->escapes=1;return({struct _tuple10 _tmpA9D;
_tmpA9D.f1=(_tmpA95->tq).real_const;_tmpA9D.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA95->rgn))->v;_tmpA9D;});_LL6EE:;}_LL6E5: if(_tmpA93 <= (void*)1)
goto _LL6E7;if(*((int*)_tmpA93)!= 4)goto _LL6E7;_tmpA96=((struct Cyc_Absyn_Pat_b_struct*)
_tmpA93)->f1;_LL6E6: _tmpA97=_tmpA96;goto _LL6E8;_LL6E7: if(_tmpA93 <= (void*)1)goto
_LL6DC;if(*((int*)_tmpA93)!= 2)goto _LL6DC;_tmpA97=((struct Cyc_Absyn_Param_b_struct*)
_tmpA93)->f1;_LL6E8: _tmpA97->escapes=1;return({struct _tuple10 _tmpA9E;_tmpA9E.f1=(
_tmpA97->tq).real_const;_tmpA9E.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA97->rgn))->v;_tmpA9E;});_LL6DC:;}_LL6D2: if(*((int*)_tmpA89)!= 23)goto _LL6D4;
_tmpA8C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA89)->f1;_tmpA8D=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpA89)->f2;_LL6D3: {void*_tmpA9F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA8C->topt))->v);struct Cyc_List_List*_tmpAA0;struct Cyc_Absyn_AggrInfo
_tmpAA1;union Cyc_Absyn_AggrInfoU_union _tmpAA2;struct Cyc_Absyn_Aggrdecl**_tmpAA3;
struct Cyc_Absyn_Aggrdecl*_tmpAA4;_LL6F4: if(_tmpA9F <= (void*)4)goto _LL6F8;if(*((
int*)_tmpA9F)!= 11)goto _LL6F6;_tmpAA0=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA9F)->f2;_LL6F5: {struct Cyc_Absyn_Aggrfield*_tmpAA5=Cyc_Absyn_lookup_field(
_tmpAA0,_tmpA8D);if(_tmpAA5 != 0  && _tmpAA5->width != 0)return({struct _tuple10
_tmpAA6;_tmpAA6.f1=(_tmpAA5->tq).real_const;_tmpAA6.f2=(Cyc_Tcutil_addressof_props(
te,_tmpA8C)).f2;_tmpAA6;});return bogus_ans;}_LL6F6: if(*((int*)_tmpA9F)!= 10)goto
_LL6F8;_tmpAA1=((struct Cyc_Absyn_AggrType_struct*)_tmpA9F)->f1;_tmpAA2=_tmpAA1.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA9F)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL6F8;_tmpAA3=(_tmpAA2.KnownAggr).f1;_tmpAA4=*_tmpAA3;_LL6F7: {struct
Cyc_Absyn_Aggrfield*_tmpAA7=Cyc_Absyn_lookup_decl_field(_tmpAA4,_tmpA8D);if(
_tmpAA7 != 0  && _tmpAA7->width != 0)return({struct _tuple10 _tmpAA8;_tmpAA8.f1=(
_tmpAA7->tq).real_const;_tmpAA8.f2=(Cyc_Tcutil_addressof_props(te,_tmpA8C)).f2;
_tmpAA8;});return bogus_ans;}_LL6F8:;_LL6F9: return bogus_ans;_LL6F3:;}_LL6D4: if(*((
int*)_tmpA89)!= 24)goto _LL6D6;_tmpA8E=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpA89)->f1;_tmpA8F=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA89)->f2;_LL6D5: {
void*_tmpAA9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA8E->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAAA;void*_tmpAAB;struct Cyc_Absyn_PtrAtts
_tmpAAC;void*_tmpAAD;_LL6FB: if(_tmpAA9 <= (void*)4)goto _LL6FD;if(*((int*)_tmpAA9)
!= 4)goto _LL6FD;_tmpAAA=((struct Cyc_Absyn_PointerType_struct*)_tmpAA9)->f1;
_tmpAAB=(void*)_tmpAAA.elt_typ;_tmpAAC=_tmpAAA.ptr_atts;_tmpAAD=(void*)_tmpAAC.rgn;
_LL6FC: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpAAE=Cyc_Tcutil_compress(
_tmpAAB);struct Cyc_List_List*_tmpAAF;struct Cyc_Absyn_AggrInfo _tmpAB0;union Cyc_Absyn_AggrInfoU_union
_tmpAB1;struct Cyc_Absyn_Aggrdecl**_tmpAB2;struct Cyc_Absyn_Aggrdecl*_tmpAB3;
_LL700: if(_tmpAAE <= (void*)4)goto _LL704;if(*((int*)_tmpAAE)!= 11)goto _LL702;
_tmpAAF=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpAAE)->f2;_LL701: finfo=Cyc_Absyn_lookup_field(
_tmpAAF,_tmpA8F);goto _LL6FF;_LL702: if(*((int*)_tmpAAE)!= 10)goto _LL704;_tmpAB0=((
struct Cyc_Absyn_AggrType_struct*)_tmpAAE)->f1;_tmpAB1=_tmpAB0.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpAAE)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL704;_tmpAB2=(_tmpAB1.KnownAggr).f1;_tmpAB3=*_tmpAB2;_LL703: finfo=Cyc_Absyn_lookup_decl_field(
_tmpAB3,_tmpA8F);goto _LL6FF;_LL704:;_LL705: return bogus_ans;_LL6FF:;}if(finfo != 0
 && finfo->width != 0)return({struct _tuple10 _tmpAB4;_tmpAB4.f1=(finfo->tq).real_const;
_tmpAB4.f2=_tmpAAD;_tmpAB4;});return bogus_ans;}_LL6FD:;_LL6FE: return bogus_ans;
_LL6FA:;}_LL6D6: if(*((int*)_tmpA89)!= 22)goto _LL6D8;_tmpA90=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA89)->f1;_LL6D7: {void*_tmpAB5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA90->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAB6;struct Cyc_Absyn_Tqual
_tmpAB7;struct Cyc_Absyn_PtrAtts _tmpAB8;void*_tmpAB9;_LL707: if(_tmpAB5 <= (void*)4)
goto _LL709;if(*((int*)_tmpAB5)!= 4)goto _LL709;_tmpAB6=((struct Cyc_Absyn_PointerType_struct*)
_tmpAB5)->f1;_tmpAB7=_tmpAB6.elt_tq;_tmpAB8=_tmpAB6.ptr_atts;_tmpAB9=(void*)
_tmpAB8.rgn;_LL708: return({struct _tuple10 _tmpABA;_tmpABA.f1=_tmpAB7.real_const;
_tmpABA.f2=_tmpAB9;_tmpABA;});_LL709:;_LL70A: return bogus_ans;_LL706:;}_LL6D8: if(*((
int*)_tmpA89)!= 25)goto _LL6DA;_tmpA91=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA89)->f1;_tmpA92=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA89)->f2;_LL6D9: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA91->topt))->v);
void*_tmpABB=t;struct Cyc_List_List*_tmpABC;struct Cyc_Absyn_PtrInfo _tmpABD;struct
Cyc_Absyn_Tqual _tmpABE;struct Cyc_Absyn_PtrAtts _tmpABF;void*_tmpAC0;struct Cyc_Absyn_ArrayInfo
_tmpAC1;struct Cyc_Absyn_Tqual _tmpAC2;_LL70C: if(_tmpABB <= (void*)4)goto _LL712;if(*((
int*)_tmpABB)!= 9)goto _LL70E;_tmpABC=((struct Cyc_Absyn_TupleType_struct*)_tmpABB)->f1;
_LL70D: {unsigned int _tmpAC4;int _tmpAC5;struct _tuple7 _tmpAC3=Cyc_Evexp_eval_const_uint_exp(
_tmpA92);_tmpAC4=_tmpAC3.f1;_tmpAC5=_tmpAC3.f2;if(!_tmpAC5)return bogus_ans;{
struct _tuple5*_tmpAC6=Cyc_Absyn_lookup_tuple_field(_tmpABC,(int)_tmpAC4);if(
_tmpAC6 != 0)return({struct _tuple10 _tmpAC7;_tmpAC7.f1=((*_tmpAC6).f1).real_const;
_tmpAC7.f2=(Cyc_Tcutil_addressof_props(te,_tmpA91)).f2;_tmpAC7;});return
bogus_ans;}}_LL70E: if(*((int*)_tmpABB)!= 4)goto _LL710;_tmpABD=((struct Cyc_Absyn_PointerType_struct*)
_tmpABB)->f1;_tmpABE=_tmpABD.elt_tq;_tmpABF=_tmpABD.ptr_atts;_tmpAC0=(void*)
_tmpABF.rgn;_LL70F: return({struct _tuple10 _tmpAC8;_tmpAC8.f1=_tmpABE.real_const;
_tmpAC8.f2=_tmpAC0;_tmpAC8;});_LL710: if(*((int*)_tmpABB)!= 7)goto _LL712;_tmpAC1=((
struct Cyc_Absyn_ArrayType_struct*)_tmpABB)->f1;_tmpAC2=_tmpAC1.tq;_LL711: return({
struct _tuple10 _tmpAC9;_tmpAC9.f1=_tmpAC2.real_const;_tmpAC9.f2=(Cyc_Tcutil_addressof_props(
te,_tmpA91)).f2;_tmpAC9;});_LL712:;_LL713: return bogus_ans;_LL70B:;}_LL6DA:;
_LL6DB:({void*_tmpACA[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmpACB="unary & applied to non-lvalue";
_tag_dynforward(_tmpACB,sizeof(char),_get_zero_arr_size(_tmpACB,30));}),
_tag_dynforward(_tmpACA,sizeof(void*),0));});return bogus_ans;_LL6CF:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpACD=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpACE;void*_tmpACF;struct Cyc_Absyn_Tqual
_tmpAD0;struct Cyc_Absyn_Conref*_tmpAD1;_LL715: if(_tmpACD <= (void*)4)goto _LL717;
if(*((int*)_tmpACD)!= 7)goto _LL717;_tmpACE=((struct Cyc_Absyn_ArrayType_struct*)
_tmpACD)->f1;_tmpACF=(void*)_tmpACE.elt_type;_tmpAD0=_tmpACE.tq;_tmpAD1=_tmpACE.zero_term;
_LL716: {void*_tmpAD3;struct _tuple10 _tmpAD2=Cyc_Tcutil_addressof_props(te,e);
_tmpAD3=_tmpAD2.f2;return Cyc_Absyn_atb_typ(_tmpACF,_tmpAD3,_tmpAD0,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpAD4=_cycalloc(sizeof(*_tmpAD4));_tmpAD4[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpAD5;_tmpAD5.tag=0;_tmpAD5.f1=e;_tmpAD5;});
_tmpAD4;}),_tmpAD1);}_LL717:;_LL718: return e_typ;_LL714:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpAD6=b->v;void*_tmpAD7;void*_tmpAD8;void*
_tmpAD9;struct Cyc_Absyn_Exp*_tmpADA;void*_tmpADB;_LL71A: if((_tmpAD6.Eq_constr).tag
!= 0)goto _LL71C;_tmpAD7=(_tmpAD6.Eq_constr).f1;if((int)_tmpAD7 != 1)goto _LL71C;
_LL71B: return;_LL71C: if((_tmpAD6.Eq_constr).tag != 0)goto _LL71E;_tmpAD8=(_tmpAD6.Eq_constr).f1;
if((int)_tmpAD8 != 0)goto _LL71E;_LL71D: if(i < 0)({void*_tmpADC[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmpADD="dereference is out of bounds";_tag_dynforward(_tmpADD,
sizeof(char),_get_zero_arr_size(_tmpADD,29));}),_tag_dynforward(_tmpADC,sizeof(
void*),0));});return;_LL71E: if((_tmpAD6.Eq_constr).tag != 0)goto _LL720;_tmpAD9=(
_tmpAD6.Eq_constr).f1;if(_tmpAD9 <= (void*)2)goto _LL720;if(*((int*)_tmpAD9)!= 0)
goto _LL720;_tmpADA=((struct Cyc_Absyn_Upper_b_struct*)_tmpAD9)->f1;_LL71F: {
unsigned int _tmpADF;int _tmpAE0;struct _tuple7 _tmpADE=Cyc_Evexp_eval_const_uint_exp(
_tmpADA);_tmpADF=_tmpADE.f1;_tmpAE0=_tmpADE.f2;if(_tmpAE0  && _tmpADF <= i)({
struct Cyc_Int_pa_struct _tmpAE4;_tmpAE4.tag=1;_tmpAE4.f1=(unsigned long)((int)i);{
struct Cyc_Int_pa_struct _tmpAE3;_tmpAE3.tag=1;_tmpAE3.f1=(unsigned long)((int)
_tmpADF);{void*_tmpAE1[2]={& _tmpAE3,& _tmpAE4};Cyc_Tcutil_terr(loc,({const char*
_tmpAE2="dereference is out of bounds: %d <= %d";_tag_dynforward(_tmpAE2,sizeof(
char),_get_zero_arr_size(_tmpAE2,39));}),_tag_dynforward(_tmpAE1,sizeof(void*),2));}}});
return;}_LL720: if((_tmpAD6.Eq_constr).tag != 0)goto _LL722;_tmpADB=(_tmpAD6.Eq_constr).f1;
if(_tmpADB <= (void*)2)goto _LL722;if(*((int*)_tmpADB)!= 1)goto _LL722;_LL721:
return;_LL722: if((_tmpAD6.No_constr).tag != 2)goto _LL724;_LL723: b->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmpAE5;(_tmpAE5.Eq_constr).tag=0;(_tmpAE5.Eq_constr).f1=(
void*)((void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmpAE6=_cycalloc(sizeof(*
_tmpAE6));_tmpAE6[0]=({struct Cyc_Absyn_Upper_b_struct _tmpAE7;_tmpAE7.tag=0;
_tmpAE7.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmpAE7;});_tmpAE6;})));_tmpAE5;});return;
_LL724:;_LL725:({void*_tmpAE8[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpAE9="check_bound -- bad compressed conref";
_tag_dynforward(_tmpAE9,sizeof(char),_get_zero_arr_size(_tmpAE9,37));}),
_tag_dynforward(_tmpAE8,sizeof(void*),0));});_LL719:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){union Cyc_Absyn_Constraint_union
_tmpAEA=(Cyc_Absyn_compress_conref(b))->v;void*_tmpAEB;struct Cyc_Absyn_Exp*
_tmpAEC;_LL727: if((_tmpAEA.Eq_constr).tag != 0)goto _LL729;_tmpAEB=(_tmpAEA.Eq_constr).f1;
if(_tmpAEB <= (void*)2)goto _LL729;if(*((int*)_tmpAEB)!= 0)goto _LL729;_tmpAEC=((
struct Cyc_Absyn_Upper_b_struct*)_tmpAEB)->f1;_LL728: {unsigned int _tmpAEE;int
_tmpAEF;struct _tuple7 _tmpAED=Cyc_Evexp_eval_const_uint_exp(_tmpAEC);_tmpAEE=
_tmpAED.f1;_tmpAEF=_tmpAED.f2;return _tmpAEF  && _tmpAEE == 1;}_LL729:;_LL72A:
return 0;_LL726:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpAF0=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpAF1;void*_tmpAF2;struct Cyc_Absyn_Conref*_tmpAF3;
struct Cyc_List_List*_tmpAF4;struct Cyc_Absyn_AggrInfo _tmpAF5;union Cyc_Absyn_AggrInfoU_union
_tmpAF6;struct Cyc_Absyn_AggrInfo _tmpAF7;union Cyc_Absyn_AggrInfoU_union _tmpAF8;
struct Cyc_Absyn_Aggrdecl**_tmpAF9;struct Cyc_Absyn_Aggrdecl*_tmpAFA;struct Cyc_List_List*
_tmpAFB;struct Cyc_List_List*_tmpAFC;_LL72C: if((int)_tmpAF0 != 0)goto _LL72E;_LL72D:
goto _LL72F;_LL72E: if(_tmpAF0 <= (void*)4)goto _LL730;if(*((int*)_tmpAF0)!= 5)goto
_LL730;_LL72F: goto _LL731;_LL730: if((int)_tmpAF0 != 1)goto _LL732;_LL731: goto _LL733;
_LL732: if(_tmpAF0 <= (void*)4)goto _LL742;if(*((int*)_tmpAF0)!= 6)goto _LL734;
_LL733: return 1;_LL734: if(*((int*)_tmpAF0)!= 12)goto _LL736;_LL735: goto _LL737;
_LL736: if(*((int*)_tmpAF0)!= 13)goto _LL738;_LL737: return 0;_LL738: if(*((int*)
_tmpAF0)!= 7)goto _LL73A;_tmpAF1=((struct Cyc_Absyn_ArrayType_struct*)_tmpAF0)->f1;
_tmpAF2=(void*)_tmpAF1.elt_type;_tmpAF3=_tmpAF1.zero_term;_LL739: return !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpAF3) && Cyc_Tcutil_bits_only(
_tmpAF2);_LL73A: if(*((int*)_tmpAF0)!= 9)goto _LL73C;_tmpAF4=((struct Cyc_Absyn_TupleType_struct*)
_tmpAF0)->f1;_LL73B: for(0;_tmpAF4 != 0;_tmpAF4=_tmpAF4->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpAF4->hd)).f2))return 0;}return 1;_LL73C: if(*((int*)_tmpAF0)!= 
10)goto _LL73E;_tmpAF5=((struct Cyc_Absyn_AggrType_struct*)_tmpAF0)->f1;_tmpAF6=
_tmpAF5.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAF0)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL73E;_LL73D: return 0;_LL73E: if(*((int*)_tmpAF0)!= 10)goto _LL740;
_tmpAF7=((struct Cyc_Absyn_AggrType_struct*)_tmpAF0)->f1;_tmpAF8=_tmpAF7.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAF0)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL740;_tmpAF9=(_tmpAF8.KnownAggr).f1;_tmpAFA=*_tmpAF9;_tmpAFB=_tmpAF7.targs;
_LL73F: if(_tmpAFA->impl == 0)return 0;{struct _RegionHandle _tmpAFD=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpAFD;_push_region(rgn);{struct Cyc_List_List*_tmpAFE=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpAFA->tvs,_tmpAFB);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAFA->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpAFE,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAFF=0;_npop_handler(0);
return _tmpAFF;}}}{int _tmpB00=1;_npop_handler(0);return _tmpB00;}};_pop_region(rgn);}
_LL740: if(*((int*)_tmpAF0)!= 11)goto _LL742;_tmpAFC=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpAF0)->f2;_LL741: for(0;_tmpAFC != 0;_tmpAFC=_tmpAFC->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpAFC->hd)->type))return 0;}return 1;_LL742:;
_LL743: return 0;_LL72B:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpB01=(void*)e->r;struct _tuple2*_tmpB02;void*_tmpB03;struct Cyc_Absyn_Exp*
_tmpB04;struct Cyc_Absyn_Exp*_tmpB05;struct Cyc_Absyn_Exp*_tmpB06;struct Cyc_Absyn_Exp*
_tmpB07;struct Cyc_Absyn_Exp*_tmpB08;struct Cyc_Absyn_Exp*_tmpB09;struct Cyc_Absyn_Exp*
_tmpB0A;void*_tmpB0B;struct Cyc_Absyn_Exp*_tmpB0C;void*_tmpB0D;void*_tmpB0E;
struct Cyc_Absyn_Exp*_tmpB0F;struct Cyc_Absyn_Exp*_tmpB10;struct Cyc_Absyn_Exp*
_tmpB11;struct Cyc_Absyn_Exp*_tmpB12;struct Cyc_List_List*_tmpB13;struct Cyc_List_List*
_tmpB14;struct Cyc_List_List*_tmpB15;void*_tmpB16;struct Cyc_List_List*_tmpB17;
struct Cyc_List_List*_tmpB18;struct Cyc_List_List*_tmpB19;_LL745: if(*((int*)
_tmpB01)!= 0)goto _LL747;_LL746: goto _LL748;_LL747: if(*((int*)_tmpB01)!= 18)goto
_LL749;_LL748: goto _LL74A;_LL749: if(*((int*)_tmpB01)!= 19)goto _LL74B;_LL74A: goto
_LL74C;_LL74B: if(*((int*)_tmpB01)!= 20)goto _LL74D;_LL74C: goto _LL74E;_LL74D: if(*((
int*)_tmpB01)!= 21)goto _LL74F;_LL74E: goto _LL750;_LL74F: if(*((int*)_tmpB01)!= 33)
goto _LL751;_LL750: goto _LL752;_LL751: if(*((int*)_tmpB01)!= 34)goto _LL753;_LL752:
return 1;_LL753: if(*((int*)_tmpB01)!= 1)goto _LL755;_tmpB02=((struct Cyc_Absyn_Var_e_struct*)
_tmpB01)->f1;_tmpB03=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB01)->f2;_LL754: {
void*_tmpB1A=_tmpB03;struct Cyc_Absyn_Vardecl*_tmpB1B;_LL774: if(_tmpB1A <= (void*)
1)goto _LL778;if(*((int*)_tmpB1A)!= 1)goto _LL776;_LL775: return 1;_LL776: if(*((int*)
_tmpB1A)!= 0)goto _LL778;_tmpB1B=((struct Cyc_Absyn_Global_b_struct*)_tmpB1A)->f1;
_LL777: {void*_tmpB1C=Cyc_Tcutil_compress((void*)_tmpB1B->type);_LL77D: if(
_tmpB1C <= (void*)4)goto _LL781;if(*((int*)_tmpB1C)!= 7)goto _LL77F;_LL77E: goto
_LL780;_LL77F: if(*((int*)_tmpB1C)!= 8)goto _LL781;_LL780: return 1;_LL781:;_LL782:
return var_okay;_LL77C:;}_LL778: if((int)_tmpB1A != 0)goto _LL77A;_LL779: return 0;
_LL77A:;_LL77B: return var_okay;_LL773:;}_LL755: if(*((int*)_tmpB01)!= 6)goto _LL757;
_tmpB04=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB01)->f1;_tmpB05=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB01)->f2;_tmpB06=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB01)->f3;_LL756:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB04) && Cyc_Tcutil_cnst_exp(te,0,_tmpB05))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB06);_LL757: if(*((int*)_tmpB01)!= 9)goto _LL759;
_tmpB07=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB01)->f1;_tmpB08=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB01)->f2;_LL758: return Cyc_Tcutil_cnst_exp(te,0,_tmpB07) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB08);_LL759: if(*((int*)_tmpB01)!= 13)goto _LL75B;_tmpB09=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB01)->f1;_LL75A: _tmpB0A=_tmpB09;goto _LL75C;_LL75B: if(*((int*)_tmpB01)!= 14)
goto _LL75D;_tmpB0A=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB01)->f1;_LL75C:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB0A);_LL75D: if(*((int*)_tmpB01)!= 15)
goto _LL75F;_tmpB0B=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB01)->f1;_tmpB0C=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB01)->f2;_tmpB0D=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB01)->f4;if((int)_tmpB0D != 1)goto _LL75F;_LL75E: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB0C);_LL75F: if(*((int*)_tmpB01)!= 15)goto _LL761;_tmpB0E=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB01)->f1;_tmpB0F=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB01)->f2;_LL760: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB0F);_LL761: if(*((
int*)_tmpB01)!= 16)goto _LL763;_tmpB10=((struct Cyc_Absyn_Address_e_struct*)
_tmpB01)->f1;_LL762: return Cyc_Tcutil_cnst_exp(te,1,_tmpB10);_LL763: if(*((int*)
_tmpB01)!= 29)goto _LL765;_tmpB11=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB01)->f2;_tmpB12=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB01)->f3;
_LL764: return Cyc_Tcutil_cnst_exp(te,0,_tmpB11) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB12);_LL765: if(*((int*)_tmpB01)!= 28)goto _LL767;_tmpB13=((struct Cyc_Absyn_Array_e_struct*)
_tmpB01)->f1;_LL766: _tmpB14=_tmpB13;goto _LL768;_LL767: if(*((int*)_tmpB01)!= 31)
goto _LL769;_tmpB14=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB01)->f2;_LL768:
_tmpB15=_tmpB14;goto _LL76A;_LL769: if(*((int*)_tmpB01)!= 30)goto _LL76B;_tmpB15=((
struct Cyc_Absyn_Struct_e_struct*)_tmpB01)->f3;_LL76A: for(0;_tmpB15 != 0;_tmpB15=
_tmpB15->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpB15->hd)).f2))
return 0;}return 1;_LL76B: if(*((int*)_tmpB01)!= 3)goto _LL76D;_tmpB16=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB01)->f1;_tmpB17=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB01)->f2;_LL76C: _tmpB18=_tmpB17;goto _LL76E;_LL76D: if(*((int*)_tmpB01)!= 26)
goto _LL76F;_tmpB18=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB01)->f1;_LL76E: _tmpB19=
_tmpB18;goto _LL770;_LL76F: if(*((int*)_tmpB01)!= 32)goto _LL771;_tmpB19=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpB01)->f1;_LL770: for(0;_tmpB19 != 0;_tmpB19=_tmpB19->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB19->hd))return 0;}return 1;
_LL771:;_LL772: return 0;_LL744:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpB1D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB1E;
struct Cyc_Absyn_PtrAtts _tmpB1F;struct Cyc_Absyn_Conref*_tmpB20;struct Cyc_Absyn_Conref*
_tmpB21;struct Cyc_Absyn_ArrayInfo _tmpB22;void*_tmpB23;struct Cyc_List_List*
_tmpB24;struct Cyc_Absyn_AggrInfo _tmpB25;union Cyc_Absyn_AggrInfoU_union _tmpB26;
struct Cyc_List_List*_tmpB27;struct Cyc_List_List*_tmpB28;_LL784: if((int)_tmpB1D != 
0)goto _LL786;_LL785: goto _LL787;_LL786: if(_tmpB1D <= (void*)4)goto _LL788;if(*((int*)
_tmpB1D)!= 5)goto _LL788;_LL787: goto _LL789;_LL788: if((int)_tmpB1D != 1)goto _LL78A;
_LL789: goto _LL78B;_LL78A: if(_tmpB1D <= (void*)4)goto _LL79A;if(*((int*)_tmpB1D)!= 
6)goto _LL78C;_LL78B: return 1;_LL78C: if(*((int*)_tmpB1D)!= 4)goto _LL78E;_tmpB1E=((
struct Cyc_Absyn_PointerType_struct*)_tmpB1D)->f1;_tmpB1F=_tmpB1E.ptr_atts;
_tmpB20=_tmpB1F.nullable;_tmpB21=_tmpB1F.bounds;_LL78D: {void*_tmpB29=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB21);_LL79D: if((int)_tmpB29 != 0)goto _LL79F;_LL79E: return 1;
_LL79F: if((int)_tmpB29 != 1)goto _LL7A1;_LL7A0: return 1;_LL7A1:;_LL7A2: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpB20);_LL79C:;}_LL78E:
if(*((int*)_tmpB1D)!= 7)goto _LL790;_tmpB22=((struct Cyc_Absyn_ArrayType_struct*)
_tmpB1D)->f1;_tmpB23=(void*)_tmpB22.elt_type;_LL78F: return Cyc_Tcutil_supports_default(
_tmpB23);_LL790: if(*((int*)_tmpB1D)!= 9)goto _LL792;_tmpB24=((struct Cyc_Absyn_TupleType_struct*)
_tmpB1D)->f1;_LL791: for(0;_tmpB24 != 0;_tmpB24=_tmpB24->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple5*)_tmpB24->hd)).f2))return 0;}return 1;_LL792: if(*((int*)_tmpB1D)!= 
10)goto _LL794;_tmpB25=((struct Cyc_Absyn_AggrType_struct*)_tmpB1D)->f1;_tmpB26=
_tmpB25.aggr_info;_tmpB27=_tmpB25.targs;_LL793: {struct Cyc_Absyn_Aggrdecl*
_tmpB2A=Cyc_Absyn_get_known_aggrdecl(_tmpB26);if(_tmpB2A->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB2A->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpB2A->tvs,_tmpB27,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB2A->impl))->fields);}_LL794: if(*((int*)_tmpB1D)!= 11)goto _LL796;
_tmpB28=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpB1D)->f2;_LL795: return Cyc_Tcutil_fields_support_default(
0,0,_tmpB28);_LL796: if(*((int*)_tmpB1D)!= 13)goto _LL798;_LL797: goto _LL799;_LL798:
if(*((int*)_tmpB1D)!= 12)goto _LL79A;_LL799: return 1;_LL79A:;_LL79B: return 0;_LL783:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmpB2B=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpB2B;_push_region(rgn);{struct Cyc_List_List*_tmpB2C=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB2C,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB2D=0;_npop_handler(0);return _tmpB2D;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){void*_tmpB2E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpB2F;struct Cyc_Absyn_PtrAtts _tmpB30;struct Cyc_Absyn_Conref*
_tmpB31;struct Cyc_Absyn_Conref*_tmpB32;_LL7A4: if(_tmpB2E <= (void*)4)goto _LL7A6;
if(*((int*)_tmpB2E)!= 5)goto _LL7A6;_LL7A5: goto _LL7A7;_LL7A6: if((int)_tmpB2E != 1)
goto _LL7A8;_LL7A7: goto _LL7A9;_LL7A8: if(_tmpB2E <= (void*)4)goto _LL7AC;if(*((int*)
_tmpB2E)!= 6)goto _LL7AA;_LL7A9: return 1;_LL7AA: if(*((int*)_tmpB2E)!= 4)goto _LL7AC;
_tmpB2F=((struct Cyc_Absyn_PointerType_struct*)_tmpB2E)->f1;_tmpB30=_tmpB2F.ptr_atts;
_tmpB31=_tmpB30.nullable;_tmpB32=_tmpB30.bounds;_LL7AB: {union Cyc_Absyn_Constraint_union
_tmpB33=(Cyc_Absyn_compress_conref(_tmpB32))->v;void*_tmpB34;void*_tmpB35;_LL7AF:
if((_tmpB33.Eq_constr).tag != 0)goto _LL7B1;_tmpB34=(_tmpB33.Eq_constr).f1;if((int)
_tmpB34 != 0)goto _LL7B1;_LL7B0: return 0;_LL7B1: if((_tmpB33.Eq_constr).tag != 0)goto
_LL7B3;_tmpB35=(_tmpB33.Eq_constr).f1;if((int)_tmpB35 != 1)goto _LL7B3;_LL7B2:
return 0;_LL7B3: if((_tmpB33.Eq_constr).tag != 0)goto _LL7B5;_LL7B4: {union Cyc_Absyn_Constraint_union
_tmpB36=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpB31))->v;int _tmpB37;_LL7B8: if((_tmpB36.Eq_constr).tag != 0)goto _LL7BA;_tmpB37=(
_tmpB36.Eq_constr).f1;_LL7B9: return _tmpB37;_LL7BA:;_LL7BB: return 0;_LL7B7:;}
_LL7B5:;_LL7B6: return 0;_LL7AE:;}_LL7AC:;_LL7AD: return 0;_LL7A3:;}int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpB38=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB39;void*
_tmpB3A;struct Cyc_Absyn_FnInfo _tmpB3B;struct Cyc_List_List*_tmpB3C;_LL7BD: if(
_tmpB38 <= (void*)4)goto _LL7C1;if(*((int*)_tmpB38)!= 4)goto _LL7BF;_tmpB39=((
struct Cyc_Absyn_PointerType_struct*)_tmpB38)->f1;_tmpB3A=(void*)_tmpB39.elt_typ;
_LL7BE: return Cyc_Tcutil_is_noreturn(_tmpB3A);_LL7BF: if(*((int*)_tmpB38)!= 8)goto
_LL7C1;_tmpB3B=((struct Cyc_Absyn_FnType_struct*)_tmpB38)->f1;_tmpB3C=_tmpB3B.attributes;
_LL7C0: for(0;_tmpB3C != 0;_tmpB3C=_tmpB3C->tl){void*_tmpB3D=(void*)_tmpB3C->hd;
_LL7C4: if((int)_tmpB3D != 3)goto _LL7C6;_LL7C5: return 1;_LL7C6:;_LL7C7: continue;
_LL7C3:;}goto _LL7BC;_LL7C1:;_LL7C2: goto _LL7BC;_LL7BC:;}return 0;}struct Cyc_List_List*
Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){void*_tmpB3E=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpB3F;struct Cyc_List_List*_tmpB40;
struct Cyc_List_List**_tmpB41;_LL7C9: if(_tmpB3E <= (void*)4)goto _LL7CB;if(*((int*)
_tmpB3E)!= 8)goto _LL7CB;_tmpB3F=((struct Cyc_Absyn_FnType_struct*)_tmpB3E)->f1;
_tmpB40=_tmpB3F.attributes;_tmpB41=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmpB3E)->f1).attributes;_LL7CA: {struct Cyc_List_List*_tmpB42=0;for(0;atts != 0;
atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmpB41=({struct Cyc_List_List*
_tmpB43=_cycalloc(sizeof(*_tmpB43));_tmpB43->hd=(void*)((void*)atts->hd);_tmpB43->tl=*
_tmpB41;_tmpB43;});else{_tmpB42=({struct Cyc_List_List*_tmpB44=_cycalloc(sizeof(*
_tmpB44));_tmpB44->hd=(void*)((void*)atts->hd);_tmpB44->tl=_tmpB42;_tmpB44;});}}
return _tmpB42;}_LL7CB:;_LL7CC:({void*_tmpB45[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpB46="transfer_fn_type_atts";_tag_dynforward(_tmpB46,sizeof(char),
_get_zero_arr_size(_tmpB46,22));}),_tag_dynforward(_tmpB45,sizeof(void*),0));});
_LL7C8:;}
