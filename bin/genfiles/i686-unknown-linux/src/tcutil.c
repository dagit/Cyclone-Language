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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);int
Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
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
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _tuple7{unsigned int f1;int f2;
};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dynforward_ptr,int*
is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_urk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ek;extern struct Cyc_Core_Opt Cyc_Tcutil_ik;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k);void*Cyc_Tcutil_kind_to_bound(
void*k);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
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
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
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
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){return tv1->identity - tv2->identity;}
void*Cyc_Tcutil_compress(void*t){void*_tmp3C=t;struct Cyc_Core_Opt*_tmp3D;void**
_tmp3E;void**_tmp3F;void***_tmp40;struct Cyc_Core_Opt*_tmp41;struct Cyc_Core_Opt**
_tmp42;_LL1: if(_tmp3C <= (void*)4)goto _LL9;if(*((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;
_LL3: if(*((int*)_tmp3C)!= 17)goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;if(_tmp3E != 0)goto _LL5;_LL4: return t;_LL5: if(*((int*)_tmp3C)!= 17)
goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;_tmp40=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;_LL6: {void*t2=Cyc_Tcutil_compress(*((
void**)_check_null(*_tmp40)));if(t2 != *((void**)_check_null(*_tmp40)))*_tmp40=({
void**_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=t2;_tmp43;});return t2;}_LL7:
if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;
_tmp42=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);
if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v)*_tmp42=({struct
Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->v=(void*)t2;_tmp44;});
return t2;}_LL9:;_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(void*t);static
struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return Cyc_List_map(
Cyc_Tcutil_copy_type,ts);}static struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(
struct Cyc_Absyn_Conref*c){union Cyc_Absyn_Constraint_union _tmp45=c->v;void*_tmp46;
struct Cyc_Absyn_Conref*_tmp47;_LLC: if((_tmp45.No_constr).tag != 2)goto _LLE;_LLD:
return Cyc_Absyn_empty_conref();_LLE: if((_tmp45.Eq_constr).tag != 0)goto _LL10;
_tmp46=(_tmp45.Eq_constr).f1;_LLF: return Cyc_Absyn_new_conref(_tmp46);_LL10: if((
_tmp45.Forward_constr).tag != 1)goto _LLB;_tmp47=(_tmp45.Forward_constr).f1;_LL11:
return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp48=Cyc_Absyn_compress_kb(kb);void*_tmp49;_LL13: if(*((int*)
_tmp48)!= 1)goto _LL15;_LL14: return(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp4B;_tmp4B.tag=1;_tmp4B.f1=0;_tmp4B;});_tmp4A;});_LL15: if(*((int*)_tmp48)!= 2)
goto _LL17;_tmp49=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp48)->f2;_LL16:
return(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C[0]=({struct Cyc_Absyn_Less_kb_struct _tmp4D;_tmp4D.tag=2;_tmp4D.f1=0;_tmp4D.f2=(
void*)_tmp49;_tmp4D;});_tmp4C;});_LL17:;_LL18: return kb;_LL12:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){return({struct Cyc_Absyn_Tvar*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->name=tv->name;_tmp4E->identity=- 1;
_tmp4E->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);_tmp4E;});}static
struct _tuple3*Cyc_Tcutil_copy_arg(struct _tuple3*arg){struct _tuple3 _tmp50;struct
Cyc_Core_Opt*_tmp51;struct Cyc_Absyn_Tqual _tmp52;void*_tmp53;struct _tuple3*_tmp4F=
arg;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;_tmp52=_tmp50.f2;_tmp53=_tmp50.f3;return({
struct _tuple3*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->f1=_tmp51;_tmp54->f2=
_tmp52;_tmp54->f3=Cyc_Tcutil_copy_type(_tmp53);_tmp54;});}static struct _tuple5*
Cyc_Tcutil_copy_tqt(struct _tuple5*arg){struct _tuple5 _tmp56;struct Cyc_Absyn_Tqual
_tmp57;void*_tmp58;struct _tuple5*_tmp55=arg;_tmp56=*_tmp55;_tmp57=_tmp56.f1;
_tmp58=_tmp56.f2;return({struct _tuple5*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=
_tmp57;_tmp59->f2=Cyc_Tcutil_copy_type(_tmp58);_tmp59;});}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->name=f->name;_tmp5A->tq=f->tq;_tmp5A->type=(
void*)Cyc_Tcutil_copy_type((void*)f->type);_tmp5A->width=f->width;_tmp5A->attributes=
f->attributes;_tmp5A;});}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct
_tuple0*x){struct _tuple0 _tmp5C;void*_tmp5D;void*_tmp5E;struct _tuple0*_tmp5B=x;
_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;return({struct _tuple0*_tmp5F=
_cycalloc(sizeof(*_tmp5F));_tmp5F->f1=Cyc_Tcutil_copy_type(_tmp5D);_tmp5F->f2=
Cyc_Tcutil_copy_type(_tmp5E);_tmp5F;});}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*_tmp60=_cycalloc(
sizeof(*_tmp60));_tmp60->name=f->name;_tmp60->tag=f->tag;_tmp60->loc=f->loc;
_tmp60;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp61=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp62;struct Cyc_Absyn_TunionInfo _tmp63;union Cyc_Absyn_TunionInfoU_union
_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_Core_Opt*_tmp66;struct Cyc_Absyn_TunionFieldInfo
_tmp67;union Cyc_Absyn_TunionFieldInfoU_union _tmp68;struct Cyc_List_List*_tmp69;
struct Cyc_Absyn_PtrInfo _tmp6A;void*_tmp6B;struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_PtrAtts
_tmp6D;void*_tmp6E;struct Cyc_Absyn_Conref*_tmp6F;struct Cyc_Absyn_Conref*_tmp70;
struct Cyc_Absyn_Conref*_tmp71;struct Cyc_Absyn_PtrLoc*_tmp72;struct Cyc_Absyn_ArrayInfo
_tmp73;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;struct Cyc_Absyn_Exp*_tmp76;struct
Cyc_Absyn_Conref*_tmp77;struct Cyc_Position_Segment*_tmp78;struct Cyc_Absyn_FnInfo
_tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_Core_Opt*_tmp7B;void*_tmp7C;struct
Cyc_List_List*_tmp7D;int _tmp7E;struct Cyc_Absyn_VarargInfo*_tmp7F;struct Cyc_List_List*
_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_AggrInfo
_tmp83;union Cyc_Absyn_AggrInfoU_union _tmp84;void*_tmp85;struct _tuple2*_tmp86;
struct Cyc_List_List*_tmp87;struct Cyc_Absyn_AggrInfo _tmp88;union Cyc_Absyn_AggrInfoU_union
_tmp89;struct Cyc_Absyn_Aggrdecl**_tmp8A;struct Cyc_List_List*_tmp8B;void*_tmp8C;
struct Cyc_List_List*_tmp8D;struct _tuple2*_tmp8E;struct Cyc_Absyn_Enumdecl*_tmp8F;
struct Cyc_List_List*_tmp90;void*_tmp91;int _tmp92;void*_tmp93;void*_tmp94;void*
_tmp95;void*_tmp96;struct _tuple2*_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Typedefdecl*
_tmp99;void*_tmp9A;struct Cyc_List_List*_tmp9B;void*_tmp9C;_LL1A: if((int)_tmp61 != 
0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp61 <= (void*)4)goto _LL28;if(*((int*)
_tmp61)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(*((int*)_tmp61)!= 1)goto _LL20;
_tmp62=((struct Cyc_Absyn_VarType_struct*)_tmp61)->f1;_LL1F: return(void*)({struct
Cyc_Absyn_VarType_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_VarType_struct
_tmp9E;_tmp9E.tag=1;_tmp9E.f1=Cyc_Tcutil_copy_tvar(_tmp62);_tmp9E;});_tmp9D;});
_LL20: if(*((int*)_tmp61)!= 2)goto _LL22;_tmp63=((struct Cyc_Absyn_TunionType_struct*)
_tmp61)->f1;_tmp64=_tmp63.tunion_info;_tmp65=_tmp63.targs;_tmp66=_tmp63.rgn;
_LL21: {struct Cyc_Core_Opt*_tmp9F=(unsigned int)_tmp66?({struct Cyc_Core_Opt*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp66->v);_tmpA3;}): 0;return(void*)({struct Cyc_Absyn_TunionType_struct*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_TunionType_struct _tmpA1;
_tmpA1.tag=2;_tmpA1.f1=({struct Cyc_Absyn_TunionInfo _tmpA2;_tmpA2.tunion_info=
_tmp64;_tmpA2.targs=Cyc_Tcutil_copy_types(_tmp65);_tmpA2.rgn=_tmp9F;_tmpA2;});
_tmpA1;});_tmpA0;});}_LL22: if(*((int*)_tmp61)!= 3)goto _LL24;_tmp67=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp61)->f1;_tmp68=_tmp67.field_info;_tmp69=_tmp67.targs;_LL23: return(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[
0]=({struct Cyc_Absyn_TunionFieldType_struct _tmpA5;_tmpA5.tag=3;_tmpA5.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmpA6;_tmpA6.field_info=_tmp68;_tmpA6.targs=Cyc_Tcutil_copy_types(
_tmp69);_tmpA6;});_tmpA5;});_tmpA4;});_LL24: if(*((int*)_tmp61)!= 4)goto _LL26;
_tmp6A=((struct Cyc_Absyn_PointerType_struct*)_tmp61)->f1;_tmp6B=(void*)_tmp6A.elt_typ;
_tmp6C=_tmp6A.elt_tq;_tmp6D=_tmp6A.ptr_atts;_tmp6E=(void*)_tmp6D.rgn;_tmp6F=
_tmp6D.nullable;_tmp70=_tmp6D.bounds;_tmp71=_tmp6D.zero_term;_tmp72=_tmp6D.ptrloc;
_LL25: {void*_tmpA7=Cyc_Tcutil_copy_type(_tmp6B);void*_tmpA8=Cyc_Tcutil_copy_type(
_tmp6E);struct Cyc_Absyn_Conref*_tmpA9=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp6F);struct Cyc_Absyn_Tqual _tmpAA=_tmp6C;struct Cyc_Absyn_Conref*
_tmpAB=Cyc_Tcutil_copy_conref(_tmp70);struct Cyc_Absyn_Conref*_tmpAC=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp71);return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({
struct Cyc_Absyn_PointerType_struct _tmpAE;_tmpAE.tag=4;_tmpAE.f1=({struct Cyc_Absyn_PtrInfo
_tmpAF;_tmpAF.elt_typ=(void*)_tmpA7;_tmpAF.elt_tq=_tmpAA;_tmpAF.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpB0;_tmpB0.rgn=(void*)_tmpA8;_tmpB0.nullable=_tmpA9;
_tmpB0.bounds=_tmpAB;_tmpB0.zero_term=_tmpAC;_tmpB0.ptrloc=_tmp72;_tmpB0;});
_tmpAF;});_tmpAE;});_tmpAD;});}_LL26: if(*((int*)_tmp61)!= 5)goto _LL28;_LL27: goto
_LL29;_LL28: if((int)_tmp61 != 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(_tmp61 <= (void*)
4)goto _LL48;if(*((int*)_tmp61)!= 6)goto _LL2C;_LL2B: return t;_LL2C: if(*((int*)
_tmp61)!= 7)goto _LL2E;_tmp73=((struct Cyc_Absyn_ArrayType_struct*)_tmp61)->f1;
_tmp74=(void*)_tmp73.elt_type;_tmp75=_tmp73.tq;_tmp76=_tmp73.num_elts;_tmp77=
_tmp73.zero_term;_tmp78=_tmp73.zt_loc;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB2;_tmpB2.tag=7;_tmpB2.f1=({struct Cyc_Absyn_ArrayInfo _tmpB3;_tmpB3.elt_type=(
void*)Cyc_Tcutil_copy_type(_tmp74);_tmpB3.tq=_tmp75;_tmpB3.num_elts=_tmp76;
_tmpB3.zero_term=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp77);_tmpB3.zt_loc=_tmp78;_tmpB3;});_tmpB2;});_tmpB1;});_LL2E: if(*((int*)
_tmp61)!= 8)goto _LL30;_tmp79=((struct Cyc_Absyn_FnType_struct*)_tmp61)->f1;_tmp7A=
_tmp79.tvars;_tmp7B=_tmp79.effect;_tmp7C=(void*)_tmp79.ret_typ;_tmp7D=_tmp79.args;
_tmp7E=_tmp79.c_varargs;_tmp7F=_tmp79.cyc_varargs;_tmp80=_tmp79.rgn_po;_tmp81=
_tmp79.attributes;_LL2F: {struct Cyc_List_List*_tmpB4=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp7A);struct Cyc_Core_Opt*_tmpB5=_tmp7B == 0?0:({struct Cyc_Core_Opt*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp7B->v);_tmpBF;});void*_tmpB6=Cyc_Tcutil_copy_type(_tmp7C);struct Cyc_List_List*
_tmpB7=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7D);int _tmpB8=_tmp7E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp7F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp7F;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpB9=_cycalloc(sizeof(*_tmpB9));
_tmpB9->name=cv->name;_tmpB9->tq=cv->tq;_tmpB9->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpB9->inject=cv->inject;_tmpB9;});}{struct Cyc_List_List*_tmpBA=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp80);struct Cyc_List_List*_tmpBB=_tmp81;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));
_tmpBC[0]=({struct Cyc_Absyn_FnType_struct _tmpBD;_tmpBD.tag=8;_tmpBD.f1=({struct
Cyc_Absyn_FnInfo _tmpBE;_tmpBE.tvars=_tmpB4;_tmpBE.effect=_tmpB5;_tmpBE.ret_typ=(
void*)_tmpB6;_tmpBE.args=_tmpB7;_tmpBE.c_varargs=_tmpB8;_tmpBE.cyc_varargs=
cyc_varargs2;_tmpBE.rgn_po=_tmpBA;_tmpBE.attributes=_tmpBB;_tmpBE;});_tmpBD;});
_tmpBC;});}}_LL30: if(*((int*)_tmp61)!= 9)goto _LL32;_tmp82=((struct Cyc_Absyn_TupleType_struct*)
_tmp61)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpC0=
_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_TupleType_struct _tmpC1;
_tmpC1.tag=9;_tmpC1.f1=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp82);
_tmpC1;});_tmpC0;});_LL32: if(*((int*)_tmp61)!= 10)goto _LL34;_tmp83=((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1;_tmp84=_tmp83.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp85=(_tmp84.UnknownAggr).f1;
_tmp86=(_tmp84.UnknownAggr).f2;_tmp87=_tmp83.targs;_LL33: return(void*)({struct
Cyc_Absyn_AggrType_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct
Cyc_Absyn_AggrType_struct _tmpC3;_tmpC3.tag=10;_tmpC3.f1=({struct Cyc_Absyn_AggrInfo
_tmpC4;_tmpC4.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmpC5;(_tmpC5.UnknownAggr).tag=0;(_tmpC5.UnknownAggr).f1=(void*)_tmp85;(_tmpC5.UnknownAggr).f2=
_tmp86;_tmpC5;});_tmpC4.targs=Cyc_Tcutil_copy_types(_tmp87);_tmpC4;});_tmpC3;});
_tmpC2;});_LL34: if(*((int*)_tmp61)!= 10)goto _LL36;_tmp88=((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1;_tmp89=_tmp88.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).KnownAggr).tag != 1)goto _LL36;_tmp8A=(_tmp89.KnownAggr).f1;
_tmp8B=_tmp88.targs;_LL35: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_AggrType_struct _tmpC7;
_tmpC7.tag=10;_tmpC7.f1=({struct Cyc_Absyn_AggrInfo _tmpC8;_tmpC8.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpC9;(_tmpC9.KnownAggr).tag=
1;(_tmpC9.KnownAggr).f1=_tmp8A;_tmpC9;});_tmpC8.targs=Cyc_Tcutil_copy_types(
_tmp8B);_tmpC8;});_tmpC7;});_tmpC6;});_LL36: if(*((int*)_tmp61)!= 11)goto _LL38;
_tmp8C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f1;_tmp8D=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f2;_LL37: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmpCB;_tmpCB.tag=11;_tmpCB.f1=(void*)_tmp8C;_tmpCB.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp8D);_tmpCB;});_tmpCA;});_LL38: if(*((int*)
_tmp61)!= 12)goto _LL3A;_tmp8E=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f1;
_tmp8F=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f2;_LL39: return(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({
struct Cyc_Absyn_EnumType_struct _tmpCD;_tmpCD.tag=12;_tmpCD.f1=_tmp8E;_tmpCD.f2=
_tmp8F;_tmpCD;});_tmpCC;});_LL3A: if(*((int*)_tmp61)!= 13)goto _LL3C;_tmp90=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp61)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmpCF;_tmpCF.tag=13;_tmpCF.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*
f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp90);_tmpCF;});_tmpCE;});_LL3C: if(*((int*)_tmp61)!= 14)goto _LL3E;_tmp91=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp61)->f1;_LL3D: return(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_SizeofType_struct
_tmpD1;_tmpD1.tag=14;_tmpD1.f1=(void*)Cyc_Tcutil_copy_type(_tmp91);_tmpD1;});
_tmpD0;});_LL3E: if(*((int*)_tmp61)!= 19)goto _LL40;_tmp92=((struct Cyc_Absyn_TypeInt_struct*)
_tmp61)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpD2=
_cycalloc_atomic(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpD3;_tmpD3.tag=19;_tmpD3.f1=_tmp92;_tmpD3;});_tmpD2;});_LL40: if(*((int*)
_tmp61)!= 18)goto _LL42;_tmp93=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp61)->f1;
_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpD4=_cycalloc(sizeof(*
_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_TagType_struct _tmpD5;_tmpD5.tag=18;_tmpD5.f1=(
void*)Cyc_Tcutil_copy_type(_tmp93);_tmpD5;});_tmpD4;});_LL42: if(*((int*)_tmp61)
!= 15)goto _LL44;_tmp94=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp61)->f1;
_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpD6=_cycalloc(
sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpD7;_tmpD7.tag=
15;_tmpD7.f1=(void*)Cyc_Tcutil_copy_type(_tmp94);_tmpD7;});_tmpD6;});_LL44: if(*((
int*)_tmp61)!= 16)goto _LL46;_tmp95=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp61)->f1;_tmp96=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp61)->f2;_LL45:
return(void*)({struct Cyc_Absyn_DynRgnType_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));
_tmpD8[0]=({struct Cyc_Absyn_DynRgnType_struct _tmpD9;_tmpD9.tag=16;_tmpD9.f1=(
void*)Cyc_Tcutil_copy_type(_tmp95);_tmpD9.f2=(void*)Cyc_Tcutil_copy_type(_tmp96);
_tmpD9;});_tmpD8;});_LL46: if(*((int*)_tmp61)!= 17)goto _LL48;_tmp97=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61)->f1;_tmp98=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f2;_tmp99=((
struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f3;_LL47: return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpDB;_tmpDB.tag=17;_tmpDB.f1=_tmp97;_tmpDB.f2=Cyc_Tcutil_copy_types(_tmp98);
_tmpDB.f3=_tmp99;_tmpDB.f4=0;_tmpDB;});_tmpDA;});_LL48: if((int)_tmp61 != 3)goto
_LL4A;_LL49: goto _LL4B;_LL4A: if((int)_tmp61 != 2)goto _LL4C;_LL4B: return t;_LL4C: if(
_tmp61 <= (void*)4)goto _LL4E;if(*((int*)_tmp61)!= 20)goto _LL4E;_tmp9A=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp61)->f1;_LL4D: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_AccessEff_struct
_tmpDD;_tmpDD.tag=20;_tmpDD.f1=(void*)Cyc_Tcutil_copy_type(_tmp9A);_tmpDD;});
_tmpDC;});_LL4E: if(_tmp61 <= (void*)4)goto _LL50;if(*((int*)_tmp61)!= 21)goto _LL50;
_tmp9B=((struct Cyc_Absyn_JoinEff_struct*)_tmp61)->f1;_LL4F: return(void*)({struct
Cyc_Absyn_JoinEff_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpDF;_tmpDF.tag=21;_tmpDF.f1=Cyc_Tcutil_copy_types(_tmp9B);_tmpDF;});_tmpDE;});
_LL50: if(_tmp61 <= (void*)4)goto _LL19;if(*((int*)_tmp61)!= 22)goto _LL19;_tmp9C=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp61)->f1;_LL51: return(void*)({struct
Cyc_Absyn_RgnsEff_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmpE1;_tmpE1.tag=22;_tmpE1.f1=(void*)Cyc_Tcutil_copy_type(_tmp9C);_tmpE1;});
_tmpE0;});_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{
struct _tuple0 _tmpE3=({struct _tuple0 _tmpE2;_tmpE2.f1=k1;_tmpE2.f2=k2;_tmpE2;});
void*_tmpE4;void*_tmpE5;void*_tmpE6;void*_tmpE7;void*_tmpE8;void*_tmpE9;void*
_tmpEA;void*_tmpEB;void*_tmpEC;void*_tmpED;_LL53: _tmpE4=_tmpE3.f1;if((int)_tmpE4
!= 2)goto _LL55;_tmpE5=_tmpE3.f2;if((int)_tmpE5 != 1)goto _LL55;_LL54: goto _LL56;
_LL55: _tmpE6=_tmpE3.f1;if((int)_tmpE6 != 2)goto _LL57;_tmpE7=_tmpE3.f2;if((int)
_tmpE7 != 0)goto _LL57;_LL56: goto _LL58;_LL57: _tmpE8=_tmpE3.f1;if((int)_tmpE8 != 1)
goto _LL59;_tmpE9=_tmpE3.f2;if((int)_tmpE9 != 0)goto _LL59;_LL58: goto _LL5A;_LL59:
_tmpEA=_tmpE3.f1;if((int)_tmpEA != 3)goto _LL5B;_tmpEB=_tmpE3.f2;if((int)_tmpEB != 
5)goto _LL5B;_LL5A: goto _LL5C;_LL5B: _tmpEC=_tmpE3.f1;if((int)_tmpEC != 4)goto _LL5D;
_tmpED=_tmpE3.f2;if((int)_tmpED != 5)goto _LL5D;_LL5C: return 1;_LL5D:;_LL5E: return 0;
_LL52:;}}static int Cyc_Tcutil_is_region_kind(void*k){void*_tmpEE=k;_LL60: if((int)
_tmpEE != 3)goto _LL62;_LL61: goto _LL63;_LL62: if((int)_tmpEE != 5)goto _LL64;_LL63:
goto _LL65;_LL64: if((int)_tmpEE != 4)goto _LL66;_LL65: return 1;_LL66:;_LL67: return 0;
_LL5F:;}void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpEF=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpF0;void*_tmpF1;_LL69: if(*((int*)_tmpEF)!= 0)goto _LL6B;
_tmpF0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpEF)->f1;_LL6A: return _tmpF0;
_LL6B: if(*((int*)_tmpEF)!= 2)goto _LL6D;_tmpF1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpEF)->f2;_LL6C: return _tmpF1;_LL6D:;_LL6E:({void*_tmpF2[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF3="kind not suitably constrained!";
_tag_dynforward(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,31));}),
_tag_dynforward(_tmpF2,sizeof(void*),0));});_LL68:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpF4=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpF5;struct Cyc_Absyn_Tvar*
_tmpF6;void*_tmpF7;struct Cyc_Absyn_TunionInfo _tmpF8;union Cyc_Absyn_TunionInfoU_union
_tmpF9;struct Cyc_Absyn_UnknownTunionInfo _tmpFA;struct Cyc_Absyn_TunionInfo _tmpFB;
union Cyc_Absyn_TunionInfoU_union _tmpFC;struct Cyc_Absyn_Tuniondecl**_tmpFD;struct
Cyc_Absyn_Tuniondecl*_tmpFE;struct Cyc_Absyn_TunionFieldInfo _tmpFF;union Cyc_Absyn_TunionFieldInfoU_union
_tmp100;struct Cyc_Absyn_Tuniondecl*_tmp101;struct Cyc_Absyn_Tunionfield*_tmp102;
struct Cyc_Absyn_TunionFieldInfo _tmp103;union Cyc_Absyn_TunionFieldInfoU_union
_tmp104;struct Cyc_Absyn_Enumdecl*_tmp105;struct Cyc_Absyn_AggrInfo _tmp106;union
Cyc_Absyn_AggrInfoU_union _tmp107;struct Cyc_Absyn_AggrInfo _tmp108;union Cyc_Absyn_AggrInfoU_union
_tmp109;struct Cyc_Absyn_Aggrdecl**_tmp10A;struct Cyc_Absyn_Aggrdecl*_tmp10B;
struct Cyc_Absyn_Aggrdecl _tmp10C;struct Cyc_Absyn_AggrdeclImpl*_tmp10D;struct Cyc_Absyn_Enumdecl*
_tmp10E;struct Cyc_Absyn_PtrInfo _tmp10F;struct Cyc_Absyn_Typedefdecl*_tmp110;_LL70:
if(_tmpF4 <= (void*)4)goto _LL74;if(*((int*)_tmpF4)!= 0)goto _LL72;_tmpF5=((struct
Cyc_Absyn_Evar_struct*)_tmpF4)->f1;_LL71: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF5))->v;_LL72: if(*((int*)_tmpF4)!= 1)goto _LL74;_tmpF6=((struct
Cyc_Absyn_VarType_struct*)_tmpF4)->f1;_LL73: return Cyc_Tcutil_tvar_kind(_tmpF6);
_LL74: if((int)_tmpF4 != 0)goto _LL76;_LL75: return(void*)1;_LL76: if(_tmpF4 <= (void*)
4)goto _LL78;if(*((int*)_tmpF4)!= 5)goto _LL78;_tmpF7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpF4)->f2;_LL77: return(_tmpF7 == (void*)((void*)2) || _tmpF7 == (void*)((void*)3))?(
void*)2:(void*)1;_LL78: if((int)_tmpF4 != 1)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(
_tmpF4 <= (void*)4)goto _LL82;if(*((int*)_tmpF4)!= 6)goto _LL7C;_LL7B: goto _LL7D;
_LL7C: if(*((int*)_tmpF4)!= 8)goto _LL7E;_LL7D: return(void*)1;_LL7E: if(*((int*)
_tmpF4)!= 16)goto _LL80;_LL7F: goto _LL81;_LL80: if(*((int*)_tmpF4)!= 15)goto _LL82;
_LL81: return(void*)2;_LL82: if((int)_tmpF4 != 3)goto _LL84;_LL83: return(void*)4;
_LL84: if((int)_tmpF4 != 2)goto _LL86;_LL85: return(void*)3;_LL86: if(_tmpF4 <= (void*)
4)goto _LL88;if(*((int*)_tmpF4)!= 2)goto _LL88;_tmpF8=((struct Cyc_Absyn_TunionType_struct*)
_tmpF4)->f1;_tmpF9=_tmpF8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmpF4)->f1).tunion_info).UnknownTunion).tag != 0)goto _LL88;_tmpFA=(_tmpF9.UnknownTunion).f1;
_LL87: if(_tmpFA.is_flat)return(void*)1;else{return(void*)2;}_LL88: if(_tmpF4 <= (
void*)4)goto _LL8A;if(*((int*)_tmpF4)!= 2)goto _LL8A;_tmpFB=((struct Cyc_Absyn_TunionType_struct*)
_tmpF4)->f1;_tmpFC=_tmpFB.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmpF4)->f1).tunion_info).KnownTunion).tag != 1)goto _LL8A;_tmpFD=(_tmpFC.KnownTunion).f1;
_tmpFE=*_tmpFD;_LL89: if(_tmpFE->is_flat)return(void*)1;else{return(void*)2;}
_LL8A: if(_tmpF4 <= (void*)4)goto _LL8C;if(*((int*)_tmpF4)!= 3)goto _LL8C;_tmpFF=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpF4)->f1;_tmp100=_tmpFF.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF4)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL8C;_tmp101=(_tmp100.KnownTunionfield).f1;_tmp102=(_tmp100.KnownTunionfield).f2;
_LL8B: if(_tmp101->is_flat)return(void*)1;else{if(_tmp102->typs == 0)return(void*)
2;else{return(void*)1;}}_LL8C: if(_tmpF4 <= (void*)4)goto _LL8E;if(*((int*)_tmpF4)
!= 3)goto _LL8E;_tmp103=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF4)->f1;
_tmp104=_tmp103.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF4)->f1).field_info).UnknownTunionfield).tag != 0)goto _LL8E;_LL8D:({void*
_tmp111[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp112="typ_kind: Unresolved TunionFieldType";_tag_dynforward(_tmp112,
sizeof(char),_get_zero_arr_size(_tmp112,37));}),_tag_dynforward(_tmp111,sizeof(
void*),0));});_LL8E: if(_tmpF4 <= (void*)4)goto _LL90;if(*((int*)_tmpF4)!= 12)goto
_LL90;_tmp105=((struct Cyc_Absyn_EnumType_struct*)_tmpF4)->f2;if(_tmp105 != 0)goto
_LL90;_LL8F: goto _LL91;_LL90: if(_tmpF4 <= (void*)4)goto _LL92;if(*((int*)_tmpF4)!= 
10)goto _LL92;_tmp106=((struct Cyc_Absyn_AggrType_struct*)_tmpF4)->f1;_tmp107=
_tmp106.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF4)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL92;_LL91: return(void*)0;_LL92: if(_tmpF4 <= (void*)4)goto _LL94;if(*((
int*)_tmpF4)!= 10)goto _LL94;_tmp108=((struct Cyc_Absyn_AggrType_struct*)_tmpF4)->f1;
_tmp109=_tmp108.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF4)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL94;_tmp10A=(_tmp109.KnownAggr).f1;_tmp10B=*_tmp10A;_tmp10C=*_tmp10B;
_tmp10D=_tmp10C.impl;_LL93: return _tmp10D == 0?(void*)0:(void*)1;_LL94: if(_tmpF4 <= (
void*)4)goto _LL96;if(*((int*)_tmpF4)!= 11)goto _LL96;_LL95: goto _LL97;_LL96: if(
_tmpF4 <= (void*)4)goto _LL98;if(*((int*)_tmpF4)!= 13)goto _LL98;_LL97: return(void*)
1;_LL98: if(_tmpF4 <= (void*)4)goto _LL9A;if(*((int*)_tmpF4)!= 12)goto _LL9A;_tmp10E=((
struct Cyc_Absyn_EnumType_struct*)_tmpF4)->f2;_LL99: if(_tmp10E->fields == 0)return(
void*)0;else{return(void*)1;}_LL9A: if(_tmpF4 <= (void*)4)goto _LL9C;if(*((int*)
_tmpF4)!= 4)goto _LL9C;_tmp10F=((struct Cyc_Absyn_PointerType_struct*)_tmpF4)->f1;
_LL9B: {union Cyc_Absyn_Constraint_union _tmp113=(Cyc_Absyn_compress_conref((
_tmp10F.ptr_atts).bounds))->v;void*_tmp114;void*_tmp115;void*_tmp116;void*
_tmp117;_LLAF: if((_tmp113.No_constr).tag != 2)goto _LLB1;_LLB0: goto _LLB2;_LLB1: if((
_tmp113.Eq_constr).tag != 0)goto _LLB3;_tmp114=(_tmp113.Eq_constr).f1;if((int)
_tmp114 != 0)goto _LLB3;_LLB2: return(void*)1;_LLB3: if((_tmp113.Eq_constr).tag != 0)
goto _LLB5;_tmp115=(_tmp113.Eq_constr).f1;if((int)_tmp115 != 1)goto _LLB5;_LLB4:
return(void*)1;_LLB5: if((_tmp113.Eq_constr).tag != 0)goto _LLB7;_tmp116=(_tmp113.Eq_constr).f1;
if(_tmp116 <= (void*)2)goto _LLB7;if(*((int*)_tmp116)!= 0)goto _LLB7;_LLB6: goto
_LLB8;_LLB7: if((_tmp113.Eq_constr).tag != 0)goto _LLB9;_tmp117=(_tmp113.Eq_constr).f1;
if(_tmp117 <= (void*)2)goto _LLB9;if(*((int*)_tmp117)!= 1)goto _LLB9;_LLB8: return(
void*)2;_LLB9: if((_tmp113.Forward_constr).tag != 1)goto _LLAE;_LLBA:({void*_tmp118[
0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp119="typ_kind: forward constr in ptr bounds";_tag_dynforward(
_tmp119,sizeof(char),_get_zero_arr_size(_tmp119,39));}),_tag_dynforward(_tmp118,
sizeof(void*),0));});_LLAE:;}_LL9C: if(_tmpF4 <= (void*)4)goto _LL9E;if(*((int*)
_tmpF4)!= 14)goto _LL9E;_LL9D: return(void*)2;_LL9E: if(_tmpF4 <= (void*)4)goto _LLA0;
if(*((int*)_tmpF4)!= 19)goto _LLA0;_LL9F: return(void*)7;_LLA0: if(_tmpF4 <= (void*)
4)goto _LLA2;if(*((int*)_tmpF4)!= 18)goto _LLA2;_LLA1: return(void*)2;_LLA2: if(
_tmpF4 <= (void*)4)goto _LLA4;if(*((int*)_tmpF4)!= 7)goto _LLA4;_LLA3: goto _LLA5;
_LLA4: if(_tmpF4 <= (void*)4)goto _LLA6;if(*((int*)_tmpF4)!= 9)goto _LLA6;_LLA5:
return(void*)1;_LLA6: if(_tmpF4 <= (void*)4)goto _LLA8;if(*((int*)_tmpF4)!= 17)goto
_LLA8;_tmp110=((struct Cyc_Absyn_TypedefType_struct*)_tmpF4)->f3;_LLA7: if(_tmp110
== 0  || _tmp110->kind == 0)({struct Cyc_String_pa_struct _tmp11C;_tmp11C.tag=0;
_tmp11C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp11A[1]={& _tmp11C};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp11B="typ_kind: typedef found: %s";
_tag_dynforward(_tmp11B,sizeof(char),_get_zero_arr_size(_tmp11B,28));}),
_tag_dynforward(_tmp11A,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp110->kind))->v;_LLA8: if(_tmpF4 <= (void*)4)goto _LLAA;if(*((int*)
_tmpF4)!= 20)goto _LLAA;_LLA9: goto _LLAB;_LLAA: if(_tmpF4 <= (void*)4)goto _LLAC;if(*((
int*)_tmpF4)!= 21)goto _LLAC;_LLAB: goto _LLAD;_LLAC: if(_tmpF4 <= (void*)4)goto _LL6F;
if(*((int*)_tmpF4)!= 22)goto _LL6F;_LLAD: return(void*)6;_LL6F:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp11D;_push_handler(& _tmp11D);{int _tmp11F=
0;if(setjmp(_tmp11D.handler))_tmp11F=1;if(!_tmp11F){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp120=1;_npop_handler(0);return _tmp120;};_pop_handler();}else{void*_tmp11E=(
void*)_exn_thrown;void*_tmp122=_tmp11E;_LLBC: if(_tmp122 != Cyc_Tcutil_Unify)goto
_LLBE;_LLBD: return 0;_LLBE:;_LLBF:(void)_throw(_tmp122);_LLBB:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp123=t;struct Cyc_Absyn_Tvar*
_tmp124;struct Cyc_Core_Opt*_tmp125;struct Cyc_Core_Opt*_tmp126;struct Cyc_Core_Opt**
_tmp127;struct Cyc_Absyn_PtrInfo _tmp128;struct Cyc_Absyn_ArrayInfo _tmp129;void*
_tmp12A;struct Cyc_Absyn_FnInfo _tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_Core_Opt*
_tmp12D;void*_tmp12E;struct Cyc_List_List*_tmp12F;int _tmp130;struct Cyc_Absyn_VarargInfo*
_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_List_List*_tmp133;struct Cyc_List_List*
_tmp134;struct Cyc_Absyn_TunionInfo _tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Core_Opt*
_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_TunionFieldInfo _tmp139;
struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_AggrInfo _tmp13B;struct Cyc_List_List*
_tmp13C;struct Cyc_List_List*_tmp13D;void*_tmp13E;void*_tmp13F;void*_tmp140;void*
_tmp141;void*_tmp142;struct Cyc_List_List*_tmp143;_LLC1: if(_tmp123 <= (void*)4)
goto _LLE3;if(*((int*)_tmp123)!= 1)goto _LLC3;_tmp124=((struct Cyc_Absyn_VarType_struct*)
_tmp123)->f1;_LLC2: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp124)){Cyc_Tcutil_failure_reason=({const char*_tmp144="(type variable would escape scope)";
_tag_dynforward(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,35));});(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LLC0;_LLC3: if(*((int*)_tmp123)!= 0)goto
_LLC5;_tmp125=((struct Cyc_Absyn_Evar_struct*)_tmp123)->f2;_tmp126=((struct Cyc_Absyn_Evar_struct*)
_tmp123)->f4;_tmp127=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp123)->f4;_LLC4: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp145="(occurs check)";
_tag_dynforward(_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,15));});(int)
_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmp125 != 0)Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp125->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp127))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp146=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp127))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp146=({struct Cyc_List_List*_tmp147=
_cycalloc(sizeof(*_tmp147));_tmp147->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp147->tl=
_tmp146;_tmp147;});}}*_tmp127=({struct Cyc_Core_Opt*_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148->v=_tmp146;_tmp148;});}}}goto _LLC0;_LLC5: if(*((int*)_tmp123)!= 
4)goto _LLC7;_tmp128=((struct Cyc_Absyn_PointerType_struct*)_tmp123)->f1;_LLC6: Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp128.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp128.ptr_atts).rgn);{
union Cyc_Absyn_Constraint_union _tmp149=(Cyc_Absyn_compress_conref((_tmp128.ptr_atts).bounds))->v;
void*_tmp14A;void*_tmp14B;_LLE6: if((_tmp149.Eq_constr).tag != 0)goto _LLE8;_tmp14A=(
_tmp149.Eq_constr).f1;if(_tmp14A <= (void*)2)goto _LLE8;if(*((int*)_tmp14A)!= 1)
goto _LLE8;_tmp14B=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp14A)->f1;_LLE7:
Cyc_Tcutil_occurs(evar,r,env,_tmp14B);goto _LLE5;_LLE8:;_LLE9: goto _LLE5;_LLE5:;}
goto _LLC0;_LLC7: if(*((int*)_tmp123)!= 7)goto _LLC9;_tmp129=((struct Cyc_Absyn_ArrayType_struct*)
_tmp123)->f1;_tmp12A=(void*)_tmp129.elt_type;_LLC8: Cyc_Tcutil_occurs(evar,r,env,
_tmp12A);goto _LLC0;_LLC9: if(*((int*)_tmp123)!= 8)goto _LLCB;_tmp12B=((struct Cyc_Absyn_FnType_struct*)
_tmp123)->f1;_tmp12C=_tmp12B.tvars;_tmp12D=_tmp12B.effect;_tmp12E=(void*)_tmp12B.ret_typ;
_tmp12F=_tmp12B.args;_tmp130=_tmp12B.c_varargs;_tmp131=_tmp12B.cyc_varargs;
_tmp132=_tmp12B.rgn_po;_tmp133=_tmp12B.attributes;_LLCA: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp12C,env);if(_tmp12D != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp12D->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp12E);for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple3*)_tmp12F->hd)).f3);}if(_tmp131 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp131->type);for(0;_tmp132 != 0;_tmp132=_tmp132->tl){struct
_tuple0 _tmp14D;void*_tmp14E;void*_tmp14F;struct _tuple0*_tmp14C=(struct _tuple0*)
_tmp132->hd;_tmp14D=*_tmp14C;_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp14E);Cyc_Tcutil_occurs(evar,r,env,_tmp14F);}goto _LLC0;_LLCB: if(*((
int*)_tmp123)!= 9)goto _LLCD;_tmp134=((struct Cyc_Absyn_TupleType_struct*)_tmp123)->f1;
_LLCC: for(0;_tmp134 != 0;_tmp134=_tmp134->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple5*)_tmp134->hd)).f2);}goto _LLC0;_LLCD: if(*((int*)_tmp123)!= 2)goto
_LLCF;_tmp135=((struct Cyc_Absyn_TunionType_struct*)_tmp123)->f1;_tmp136=_tmp135.targs;
_tmp137=_tmp135.rgn;_LLCE: if((unsigned int)_tmp137)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp137->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp136);goto _LLC0;_LLCF: if(*((
int*)_tmp123)!= 17)goto _LLD1;_tmp138=((struct Cyc_Absyn_TypedefType_struct*)
_tmp123)->f2;_LLD0: _tmp13A=_tmp138;goto _LLD2;_LLD1: if(*((int*)_tmp123)!= 3)goto
_LLD3;_tmp139=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp123)->f1;_tmp13A=
_tmp139.targs;_LLD2: _tmp13C=_tmp13A;goto _LLD4;_LLD3: if(*((int*)_tmp123)!= 10)
goto _LLD5;_tmp13B=((struct Cyc_Absyn_AggrType_struct*)_tmp123)->f1;_tmp13C=
_tmp13B.targs;_LLD4: Cyc_Tcutil_occurslist(evar,r,env,_tmp13C);goto _LLC0;_LLD5:
if(*((int*)_tmp123)!= 11)goto _LLD7;_tmp13D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp123)->f2;_LLD6: for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp13D->hd)->type);}goto _LLC0;_LLD7:
if(*((int*)_tmp123)!= 18)goto _LLD9;_tmp13E=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp123)->f1;_LLD8: _tmp13F=_tmp13E;goto _LLDA;_LLD9: if(*((int*)_tmp123)!= 14)goto
_LLDB;_tmp13F=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp123)->f1;_LLDA:
_tmp140=_tmp13F;goto _LLDC;_LLDB: if(*((int*)_tmp123)!= 20)goto _LLDD;_tmp140=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp123)->f1;_LLDC: _tmp141=_tmp140;goto _LLDE;
_LLDD: if(*((int*)_tmp123)!= 15)goto _LLDF;_tmp141=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp123)->f1;_LLDE: _tmp142=_tmp141;goto _LLE0;_LLDF: if(*((int*)_tmp123)!= 22)goto
_LLE1;_tmp142=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp123)->f1;_LLE0: Cyc_Tcutil_occurs(
evar,r,env,_tmp142);goto _LLC0;_LLE1: if(*((int*)_tmp123)!= 21)goto _LLE3;_tmp143=((
struct Cyc_Absyn_JoinEff_struct*)_tmp123)->f1;_LLE2: Cyc_Tcutil_occurslist(evar,r,
env,_tmp143);goto _LLC0;_LLE3:;_LLE4: goto _LLC0;_LLC0:;}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp150[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp151="tq1 real_const not set.";
_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,24));}),
_tag_dynforward(_tmp150,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp152[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp153="tq2 real_const not set.";_tag_dynforward(
_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,24));}),_tag_dynforward(_tmp152,
sizeof(void*),0));});if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({
const char*_tmp154="(qualifiers don't match)";_tag_dynforward(_tmp154,sizeof(char),
_get_zero_arr_size(_tmp154,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dynforward_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint_union _tmp155=x->v;void*_tmp156;
_LLEB: if((_tmp155.No_constr).tag != 2)goto _LLED;_LLEC: x->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp157;(_tmp157.Forward_constr).tag=1;(_tmp157.Forward_constr).f1=
y;_tmp157;});return;_LLED: if((_tmp155.Eq_constr).tag != 0)goto _LLEF;_tmp156=(
_tmp155.Eq_constr).f1;_LLEE: {union Cyc_Absyn_Constraint_union _tmp158=y->v;void*
_tmp159;_LLF2: if((_tmp158.No_constr).tag != 2)goto _LLF4;_LLF3: y->v=x->v;return;
_LLF4: if((_tmp158.Eq_constr).tag != 0)goto _LLF6;_tmp159=(_tmp158.Eq_constr).f1;
_LLF5: if(cmp(_tmp156,_tmp159)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLF6: if((_tmp158.Forward_constr).tag != 1)goto
_LLF1;_LLF7:({void*_tmp15A[0]={};Cyc_Tcutil_impos(({const char*_tmp15B="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,40));}),
_tag_dynforward(_tmp15A,sizeof(void*),0));});_LLF1:;}_LLEF: if((_tmp155.Forward_constr).tag
!= 1)goto _LLEA;_LLF0:({void*_tmp15C[0]={};Cyc_Tcutil_impos(({const char*_tmp15D="unify_conref: forward after compress";
_tag_dynforward(_tmp15D,sizeof(char),_get_zero_arr_size(_tmp15D,37));}),
_tag_dynforward(_tmp15C,sizeof(void*),0));});_LLEA:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp15E;_push_handler(& _tmp15E);{int _tmp160=0;if(setjmp(_tmp15E.handler))
_tmp160=1;if(!_tmp160){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct
_dynforward_ptr)_tag_dynforward(0,0,0));{int _tmp161=1;_npop_handler(0);return
_tmp161;};_pop_handler();}else{void*_tmp15F=(void*)_exn_thrown;void*_tmp163=
_tmp15F;_LLF9: if(_tmp163 != Cyc_Tcutil_Unify)goto _LLFB;_LLFA: return 0;_LLFB:;_LLFC:(
void)_throw(_tmp163);_LLF8:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp165=({struct _tuple0 _tmp164;_tmp164.f1=b1;_tmp164.f2=b2;_tmp164;});
void*_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;void*_tmp16B;
void*_tmp16C;void*_tmp16D;void*_tmp16E;struct Cyc_Absyn_Exp*_tmp16F;void*_tmp170;
struct Cyc_Absyn_Exp*_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;
void*_tmp176;void*_tmp177;void*_tmp178;void*_tmp179;_LLFE: _tmp166=_tmp165.f1;if((
int)_tmp166 != 0)goto _LL100;_tmp167=_tmp165.f2;if((int)_tmp167 != 0)goto _LL100;
_LLFF: return 0;_LL100: _tmp168=_tmp165.f1;if((int)_tmp168 != 0)goto _LL102;_LL101:
return - 1;_LL102: _tmp169=_tmp165.f2;if((int)_tmp169 != 0)goto _LL104;_LL103: return 1;
_LL104: _tmp16A=_tmp165.f1;if((int)_tmp16A != 1)goto _LL106;_tmp16B=_tmp165.f2;if((
int)_tmp16B != 1)goto _LL106;_LL105: return 0;_LL106: _tmp16C=_tmp165.f1;if((int)
_tmp16C != 1)goto _LL108;_LL107: return - 1;_LL108: _tmp16D=_tmp165.f2;if((int)_tmp16D
!= 1)goto _LL10A;_LL109: return 1;_LL10A: _tmp16E=_tmp165.f1;if(_tmp16E <= (void*)2)
goto _LL10C;if(*((int*)_tmp16E)!= 0)goto _LL10C;_tmp16F=((struct Cyc_Absyn_Upper_b_struct*)
_tmp16E)->f1;_tmp170=_tmp165.f2;if(_tmp170 <= (void*)2)goto _LL10C;if(*((int*)
_tmp170)!= 0)goto _LL10C;_tmp171=((struct Cyc_Absyn_Upper_b_struct*)_tmp170)->f1;
_LL10B: return Cyc_Evexp_const_exp_cmp(_tmp16F,_tmp171);_LL10C: _tmp172=_tmp165.f1;
if(_tmp172 <= (void*)2)goto _LL10E;if(*((int*)_tmp172)!= 0)goto _LL10E;_tmp173=
_tmp165.f2;if(_tmp173 <= (void*)2)goto _LL10E;if(*((int*)_tmp173)!= 1)goto _LL10E;
_LL10D: return - 1;_LL10E: _tmp174=_tmp165.f1;if(_tmp174 <= (void*)2)goto _LL110;if(*((
int*)_tmp174)!= 1)goto _LL110;_tmp175=_tmp165.f2;if(_tmp175 <= (void*)2)goto _LL110;
if(*((int*)_tmp175)!= 0)goto _LL110;_LL10F: return 1;_LL110: _tmp176=_tmp165.f1;if(
_tmp176 <= (void*)2)goto _LLFD;if(*((int*)_tmp176)!= 1)goto _LLFD;_tmp177=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp176)->f1;_tmp178=_tmp165.f2;if(_tmp178 <= (
void*)2)goto _LLFD;if(*((int*)_tmp178)!= 1)goto _LLFD;_tmp179=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp178)->f1;_LL111: return Cyc_Tcutil_typecmp(_tmp177,_tmp179);_LLFD:;}static int
Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0 _tmp17B=({struct _tuple0
_tmp17A;_tmp17A.f1=b1;_tmp17A.f2=b2;_tmp17A;});void*_tmp17C;void*_tmp17D;void*
_tmp17E;void*_tmp17F;void*_tmp180;void*_tmp181;void*_tmp182;void*_tmp183;void*
_tmp184;struct Cyc_Absyn_Exp*_tmp185;void*_tmp186;struct Cyc_Absyn_Exp*_tmp187;
void*_tmp188;struct Cyc_Absyn_Exp*_tmp189;void*_tmp18A;void*_tmp18B;void*_tmp18C;
void*_tmp18D;void*_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;void*_tmp190;void*_tmp191;
void*_tmp192;void*_tmp193;_LL113: _tmp17C=_tmp17B.f1;if((int)_tmp17C != 0)goto
_LL115;_tmp17D=_tmp17B.f2;if((int)_tmp17D != 0)goto _LL115;_LL114: return 0;_LL115:
_tmp17E=_tmp17B.f1;if((int)_tmp17E != 0)goto _LL117;_LL116: return - 1;_LL117: _tmp17F=
_tmp17B.f2;if((int)_tmp17F != 0)goto _LL119;_LL118: return 1;_LL119: _tmp180=_tmp17B.f1;
if((int)_tmp180 != 1)goto _LL11B;_tmp181=_tmp17B.f2;if((int)_tmp181 != 1)goto _LL11B;
_LL11A: return 0;_LL11B: _tmp182=_tmp17B.f1;if((int)_tmp182 != 1)goto _LL11D;_LL11C:
return - 1;_LL11D: _tmp183=_tmp17B.f2;if((int)_tmp183 != 1)goto _LL11F;_LL11E: return 1;
_LL11F: _tmp184=_tmp17B.f1;if(_tmp184 <= (void*)2)goto _LL121;if(*((int*)_tmp184)!= 
0)goto _LL121;_tmp185=((struct Cyc_Absyn_Upper_b_struct*)_tmp184)->f1;_tmp186=
_tmp17B.f2;if(_tmp186 <= (void*)2)goto _LL121;if(*((int*)_tmp186)!= 0)goto _LL121;
_tmp187=((struct Cyc_Absyn_Upper_b_struct*)_tmp186)->f1;_LL120: return Cyc_Evexp_const_exp_cmp(
_tmp185,_tmp187);_LL121: _tmp188=_tmp17B.f1;if(_tmp188 <= (void*)2)goto _LL123;if(*((
int*)_tmp188)!= 0)goto _LL123;_tmp189=((struct Cyc_Absyn_Upper_b_struct*)_tmp188)->f1;
_tmp18A=_tmp17B.f2;if(_tmp18A <= (void*)2)goto _LL123;if(*((int*)_tmp18A)!= 1)goto
_LL123;_tmp18B=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18A)->f1;_LL122:
_tmp18D=_tmp18B;_tmp18F=_tmp189;goto _LL124;_LL123: _tmp18C=_tmp17B.f1;if(_tmp18C
<= (void*)2)goto _LL125;if(*((int*)_tmp18C)!= 1)goto _LL125;_tmp18D=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18C)->f1;_tmp18E=_tmp17B.f2;if(_tmp18E <= (
void*)2)goto _LL125;if(*((int*)_tmp18E)!= 0)goto _LL125;_tmp18F=((struct Cyc_Absyn_Upper_b_struct*)
_tmp18E)->f1;_LL124: {unsigned int _tmp195;int _tmp196;struct _tuple7 _tmp194=Cyc_Evexp_eval_const_uint_exp(
_tmp18F);_tmp195=_tmp194.f1;_tmp196=_tmp194.f2;if(!_tmp196)return 1;_tmp191=
_tmp18D;_tmp193=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp197=_cycalloc_atomic(
sizeof(*_tmp197));_tmp197[0]=({struct Cyc_Absyn_TypeInt_struct _tmp198;_tmp198.tag=
19;_tmp198.f1=(int)_tmp195;_tmp198;});_tmp197;});goto _LL126;}_LL125: _tmp190=
_tmp17B.f1;if(_tmp190 <= (void*)2)goto _LL112;if(*((int*)_tmp190)!= 1)goto _LL112;
_tmp191=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp190)->f1;_tmp192=_tmp17B.f2;
if(_tmp192 <= (void*)2)goto _LL112;if(*((int*)_tmp192)!= 1)goto _LL112;_tmp193=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp192)->f1;_LL126: Cyc_Tcutil_unify_it(
_tmp191,_tmp193);return 0;_LL112:;}static int Cyc_Tcutil_attribute_case_number(void*
att){void*_tmp199=att;_LL128: if(_tmp199 <= (void*)17)goto _LL12A;if(*((int*)
_tmp199)!= 0)goto _LL12A;_LL129: return 0;_LL12A: if((int)_tmp199 != 0)goto _LL12C;
_LL12B: return 1;_LL12C: if((int)_tmp199 != 1)goto _LL12E;_LL12D: return 2;_LL12E: if((
int)_tmp199 != 2)goto _LL130;_LL12F: return 3;_LL130: if((int)_tmp199 != 3)goto _LL132;
_LL131: return 4;_LL132: if((int)_tmp199 != 4)goto _LL134;_LL133: return 5;_LL134: if(
_tmp199 <= (void*)17)goto _LL136;if(*((int*)_tmp199)!= 1)goto _LL136;_LL135: return 6;
_LL136: if((int)_tmp199 != 5)goto _LL138;_LL137: return 7;_LL138: if(_tmp199 <= (void*)
17)goto _LL13A;if(*((int*)_tmp199)!= 2)goto _LL13A;_LL139: return 8;_LL13A: if((int)
_tmp199 != 6)goto _LL13C;_LL13B: return 9;_LL13C: if((int)_tmp199 != 7)goto _LL13E;
_LL13D: return 10;_LL13E: if((int)_tmp199 != 8)goto _LL140;_LL13F: return 11;_LL140: if((
int)_tmp199 != 9)goto _LL142;_LL141: return 12;_LL142: if((int)_tmp199 != 10)goto
_LL144;_LL143: return 13;_LL144: if((int)_tmp199 != 11)goto _LL146;_LL145: return 14;
_LL146: if((int)_tmp199 != 12)goto _LL148;_LL147: return 15;_LL148: if((int)_tmp199 != 
13)goto _LL14A;_LL149: return 16;_LL14A: if((int)_tmp199 != 14)goto _LL14C;_LL14B:
return 17;_LL14C: if((int)_tmp199 != 15)goto _LL14E;_LL14D: return 18;_LL14E: if(
_tmp199 <= (void*)17)goto _LL152;if(*((int*)_tmp199)!= 3)goto _LL150;_LL14F: return
19;_LL150: if(*((int*)_tmp199)!= 4)goto _LL152;_LL151: return 20;_LL152:;_LL153:
return 21;_LL127:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple0 _tmp19B=({struct _tuple0 _tmp19A;_tmp19A.f1=att1;_tmp19A.f2=att2;_tmp19A;});
void*_tmp19C;int _tmp19D;void*_tmp19E;int _tmp19F;void*_tmp1A0;int _tmp1A1;void*
_tmp1A2;int _tmp1A3;void*_tmp1A4;int _tmp1A5;void*_tmp1A6;int _tmp1A7;void*_tmp1A8;
struct _dynforward_ptr _tmp1A9;void*_tmp1AA;struct _dynforward_ptr _tmp1AB;void*
_tmp1AC;void*_tmp1AD;int _tmp1AE;int _tmp1AF;void*_tmp1B0;void*_tmp1B1;int _tmp1B2;
int _tmp1B3;_LL155: _tmp19C=_tmp19B.f1;if(_tmp19C <= (void*)17)goto _LL157;if(*((int*)
_tmp19C)!= 0)goto _LL157;_tmp19D=((struct Cyc_Absyn_Regparm_att_struct*)_tmp19C)->f1;
_tmp19E=_tmp19B.f2;if(_tmp19E <= (void*)17)goto _LL157;if(*((int*)_tmp19E)!= 0)
goto _LL157;_tmp19F=((struct Cyc_Absyn_Regparm_att_struct*)_tmp19E)->f1;_LL156:
_tmp1A1=_tmp19D;_tmp1A3=_tmp19F;goto _LL158;_LL157: _tmp1A0=_tmp19B.f1;if(_tmp1A0
<= (void*)17)goto _LL159;if(*((int*)_tmp1A0)!= 4)goto _LL159;_tmp1A1=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A0)->f1;_tmp1A2=_tmp19B.f2;if(_tmp1A2 <= (void*)17)goto _LL159;if(*((int*)
_tmp1A2)!= 4)goto _LL159;_tmp1A3=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1A2)->f1;_LL158: _tmp1A5=_tmp1A1;_tmp1A7=_tmp1A3;goto _LL15A;_LL159: _tmp1A4=
_tmp19B.f1;if(_tmp1A4 <= (void*)17)goto _LL15B;if(*((int*)_tmp1A4)!= 1)goto _LL15B;
_tmp1A5=((struct Cyc_Absyn_Aligned_att_struct*)_tmp1A4)->f1;_tmp1A6=_tmp19B.f2;
if(_tmp1A6 <= (void*)17)goto _LL15B;if(*((int*)_tmp1A6)!= 1)goto _LL15B;_tmp1A7=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp1A6)->f1;_LL15A: return Cyc_Core_intcmp(
_tmp1A5,_tmp1A7);_LL15B: _tmp1A8=_tmp19B.f1;if(_tmp1A8 <= (void*)17)goto _LL15D;if(*((
int*)_tmp1A8)!= 2)goto _LL15D;_tmp1A9=((struct Cyc_Absyn_Section_att_struct*)
_tmp1A8)->f1;_tmp1AA=_tmp19B.f2;if(_tmp1AA <= (void*)17)goto _LL15D;if(*((int*)
_tmp1AA)!= 2)goto _LL15D;_tmp1AB=((struct Cyc_Absyn_Section_att_struct*)_tmp1AA)->f1;
_LL15C: return Cyc_strcmp((struct _dynforward_ptr)_tmp1A9,(struct _dynforward_ptr)
_tmp1AB);_LL15D: _tmp1AC=_tmp19B.f1;if(_tmp1AC <= (void*)17)goto _LL15F;if(*((int*)
_tmp1AC)!= 3)goto _LL15F;_tmp1AD=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1AC)->f1;_tmp1AE=((struct Cyc_Absyn_Format_att_struct*)_tmp1AC)->f2;_tmp1AF=((
struct Cyc_Absyn_Format_att_struct*)_tmp1AC)->f3;_tmp1B0=_tmp19B.f2;if(_tmp1B0 <= (
void*)17)goto _LL15F;if(*((int*)_tmp1B0)!= 3)goto _LL15F;_tmp1B1=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1B0)->f1;_tmp1B2=((struct Cyc_Absyn_Format_att_struct*)
_tmp1B0)->f2;_tmp1B3=((struct Cyc_Absyn_Format_att_struct*)_tmp1B0)->f3;_LL15E: {
int _tmp1B4=Cyc_Core_intcmp((int)((unsigned int)_tmp1AD),(int)((unsigned int)
_tmp1B1));if(_tmp1B4 != 0)return _tmp1B4;{int _tmp1B5=Cyc_Core_intcmp(_tmp1AE,
_tmp1B2);if(_tmp1B5 != 0)return _tmp1B5;return Cyc_Core_intcmp(_tmp1AF,_tmp1B3);}}
_LL15F:;_LL160: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL154:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct Cyc_Absyn_TypeInt_struct Cyc_Tcutil_tizero={
19,0};static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;{void*_tmp1B7=Cyc_Tcutil_tvar_kind(tv);_LL162: if((int)_tmp1B7 != 4)goto
_LL164;_LL163: t=(void*)3;goto _LL161;_LL164: if((int)_tmp1B7 != 3)goto _LL166;_LL165:
t=(void*)2;goto _LL161;_LL166: if((int)_tmp1B7 != 6)goto _LL168;_LL167: t=Cyc_Absyn_empty_effect;
goto _LL161;_LL168: if((int)_tmp1B7 != 7)goto _LL16A;_LL169: t=(void*)& Cyc_Tcutil_tizero;
goto _LL161;_LL16A:;_LL16B: t=Cyc_Absyn_sint_typ;goto _LL161;_LL161:;}return({
struct _tuple8*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->f1=tv;_tmp1B8->f2=t;
_tmp1B8;});}static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp1B9=Cyc_Tcutil_compress(
t);void*_tmp1BA;void*_tmp1BB;void*_tmp1BC;struct Cyc_Absyn_TunionInfo _tmp1BD;
struct Cyc_List_List*_tmp1BE;struct Cyc_Core_Opt*_tmp1BF;struct Cyc_Absyn_PtrInfo
_tmp1C0;void*_tmp1C1;struct Cyc_Absyn_PtrAtts _tmp1C2;void*_tmp1C3;struct Cyc_Absyn_ArrayInfo
_tmp1C4;void*_tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_TunionFieldInfo
_tmp1C7;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_AggrInfo _tmp1C9;struct Cyc_List_List*
_tmp1CA;struct Cyc_List_List*_tmp1CB;void*_tmp1CC;struct Cyc_Absyn_FnInfo _tmp1CD;
struct Cyc_List_List*_tmp1CE;struct Cyc_Core_Opt*_tmp1CF;void*_tmp1D0;struct Cyc_List_List*
_tmp1D1;struct Cyc_Absyn_VarargInfo*_tmp1D2;struct Cyc_List_List*_tmp1D3;void*
_tmp1D4;struct Cyc_List_List*_tmp1D5;_LL16D: if((int)_tmp1B9 != 0)goto _LL16F;_LL16E:
goto _LL170;_LL16F: if((int)_tmp1B9 != 1)goto _LL171;_LL170: goto _LL172;_LL171: if(
_tmp1B9 <= (void*)4)goto _LL197;if(*((int*)_tmp1B9)!= 6)goto _LL173;_LL172: goto
_LL174;_LL173: if(*((int*)_tmp1B9)!= 12)goto _LL175;_LL174: goto _LL176;_LL175: if(*((
int*)_tmp1B9)!= 13)goto _LL177;_LL176: goto _LL178;_LL177: if(*((int*)_tmp1B9)!= 19)
goto _LL179;_LL178: goto _LL17A;_LL179: if(*((int*)_tmp1B9)!= 5)goto _LL17B;_LL17A:
return Cyc_Absyn_empty_effect;_LL17B: if(*((int*)_tmp1B9)!= 0)goto _LL17D;_LL17C:
goto _LL17E;_LL17D: if(*((int*)_tmp1B9)!= 1)goto _LL17F;_LL17E: {void*_tmp1D6=Cyc_Tcutil_typ_kind(
t);_LL1A4: if((int)_tmp1D6 != 3)goto _LL1A6;_LL1A5: goto _LL1A7;_LL1A6: if((int)
_tmp1D6 != 4)goto _LL1A8;_LL1A7: goto _LL1A9;_LL1A8: if((int)_tmp1D6 != 5)goto _LL1AA;
_LL1A9: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D7=_cycalloc(sizeof(*
_tmp1D7));_tmp1D7[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D8;_tmp1D8.tag=20;
_tmp1D8.f1=(void*)t;_tmp1D8;});_tmp1D7;});_LL1AA: if((int)_tmp1D6 != 6)goto _LL1AC;
_LL1AB: return t;_LL1AC: if((int)_tmp1D6 != 7)goto _LL1AE;_LL1AD: return Cyc_Absyn_empty_effect;
_LL1AE:;_LL1AF: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1D9=_cycalloc(
sizeof(*_tmp1D9));_tmp1D9[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1DA;_tmp1DA.tag=
22;_tmp1DA.f1=(void*)t;_tmp1DA;});_tmp1D9;});_LL1A3:;}_LL17F: if(*((int*)_tmp1B9)
!= 15)goto _LL181;_tmp1BA=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp1B9)->f1;
_LL180: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DB=_cycalloc(sizeof(*
_tmp1DB));_tmp1DB[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1DC;_tmp1DC.tag=20;
_tmp1DC.f1=(void*)_tmp1BA;_tmp1DC;});_tmp1DB;});_LL181: if(*((int*)_tmp1B9)!= 16)
goto _LL183;_tmp1BB=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp1B9)->f1;
_tmp1BC=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp1B9)->f2;_LL182: return(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));
_tmp1DD[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1DE;_tmp1DE.tag=20;_tmp1DE.f1=(
void*)_tmp1BC;_tmp1DE;});_tmp1DD;});_LL183: if(*((int*)_tmp1B9)!= 2)goto _LL185;
_tmp1BD=((struct Cyc_Absyn_TunionType_struct*)_tmp1B9)->f1;_tmp1BE=_tmp1BD.targs;
_tmp1BF=_tmp1BD.rgn;_LL184: {struct Cyc_List_List*ts=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1BE);if((unsigned int)_tmp1BF)ts=({struct Cyc_List_List*_tmp1DF=_cycalloc(
sizeof(*_tmp1DF));_tmp1DF->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1E1;_tmp1E1.tag=20;_tmp1E1.f1=(void*)((void*)_tmp1BF->v);_tmp1E1;});_tmp1E0;}));
_tmp1DF->tl=ts;_tmp1DF;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E3;_tmp1E3.tag=21;_tmp1E3.f1=ts;_tmp1E3;});_tmp1E2;}));}_LL185: if(*((int*)
_tmp1B9)!= 4)goto _LL187;_tmp1C0=((struct Cyc_Absyn_PointerType_struct*)_tmp1B9)->f1;
_tmp1C1=(void*)_tmp1C0.elt_typ;_tmp1C2=_tmp1C0.ptr_atts;_tmp1C3=(void*)_tmp1C2.rgn;
_LL186: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E5;_tmp1E5.tag=21;_tmp1E5.f1=({void*_tmp1E6[2];_tmp1E6[1]=Cyc_Tcutil_rgns_of(
_tmp1C1);_tmp1E6[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1E7=_cycalloc(
sizeof(*_tmp1E7));_tmp1E7[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E8;_tmp1E8.tag=
20;_tmp1E8.f1=(void*)_tmp1C3;_tmp1E8;});_tmp1E7;});Cyc_List_list(_tag_dynforward(
_tmp1E6,sizeof(void*),2));});_tmp1E5;});_tmp1E4;}));_LL187: if(*((int*)_tmp1B9)!= 
7)goto _LL189;_tmp1C4=((struct Cyc_Absyn_ArrayType_struct*)_tmp1B9)->f1;_tmp1C5=(
void*)_tmp1C4.elt_type;_LL188: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp1C5));_LL189: if(*((int*)_tmp1B9)!= 9)goto _LL18B;_tmp1C6=((struct Cyc_Absyn_TupleType_struct*)
_tmp1B9)->f1;_LL18A: {struct Cyc_List_List*_tmp1E9=0;for(0;_tmp1C6 != 0;_tmp1C6=
_tmp1C6->tl){_tmp1E9=({struct Cyc_List_List*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));
_tmp1EA->hd=(void*)(*((struct _tuple5*)_tmp1C6->hd)).f2;_tmp1EA->tl=_tmp1E9;
_tmp1EA;});}_tmp1C8=_tmp1E9;goto _LL18C;}_LL18B: if(*((int*)_tmp1B9)!= 3)goto
_LL18D;_tmp1C7=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1B9)->f1;_tmp1C8=
_tmp1C7.targs;_LL18C: _tmp1CA=_tmp1C8;goto _LL18E;_LL18D: if(*((int*)_tmp1B9)!= 10)
goto _LL18F;_tmp1C9=((struct Cyc_Absyn_AggrType_struct*)_tmp1B9)->f1;_tmp1CA=
_tmp1C9.targs;_LL18E: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1EC;_tmp1EC.tag=21;_tmp1EC.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1CA);
_tmp1EC;});_tmp1EB;}));_LL18F: if(*((int*)_tmp1B9)!= 11)goto _LL191;_tmp1CB=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1B9)->f2;_LL190: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));
_tmp1ED[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1EE;_tmp1EE.tag=21;_tmp1EE.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1CB);_tmp1EE;});_tmp1ED;}));_LL191:
if(*((int*)_tmp1B9)!= 14)goto _LL193;_tmp1CC=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1B9)->f1;_LL192: return Cyc_Tcutil_rgns_of(_tmp1CC);_LL193: if(*((int*)_tmp1B9)
!= 18)goto _LL195;_LL194: return Cyc_Absyn_empty_effect;_LL195: if(*((int*)_tmp1B9)
!= 8)goto _LL197;_tmp1CD=((struct Cyc_Absyn_FnType_struct*)_tmp1B9)->f1;_tmp1CE=
_tmp1CD.tvars;_tmp1CF=_tmp1CD.effect;_tmp1D0=(void*)_tmp1CD.ret_typ;_tmp1D1=
_tmp1CD.args;_tmp1D2=_tmp1CD.cyc_varargs;_tmp1D3=_tmp1CD.rgn_po;_LL196: {void*
_tmp1EF=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1CE),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1CF))->v);return Cyc_Tcutil_normalize_effect(
_tmp1EF);}_LL197: if((int)_tmp1B9 != 3)goto _LL199;_LL198: goto _LL19A;_LL199: if((int)
_tmp1B9 != 2)goto _LL19B;_LL19A: return Cyc_Absyn_empty_effect;_LL19B: if(_tmp1B9 <= (
void*)4)goto _LL19D;if(*((int*)_tmp1B9)!= 20)goto _LL19D;_LL19C: goto _LL19E;_LL19D:
if(_tmp1B9 <= (void*)4)goto _LL19F;if(*((int*)_tmp1B9)!= 21)goto _LL19F;_LL19E:
return t;_LL19F: if(_tmp1B9 <= (void*)4)goto _LL1A1;if(*((int*)_tmp1B9)!= 22)goto
_LL1A1;_tmp1D4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1B9)->f1;_LL1A0:
return Cyc_Tcutil_rgns_of(_tmp1D4);_LL1A1: if(_tmp1B9 <= (void*)4)goto _LL16C;if(*((
int*)_tmp1B9)!= 17)goto _LL16C;_tmp1D5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1B9)->f2;_LL1A2: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1F1;_tmp1F1.tag=21;_tmp1F1.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1D5);
_tmp1F1;});_tmp1F0;}));_LL16C:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1F2=e;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List**_tmp1F4;void*
_tmp1F5;_LL1B1: if(_tmp1F2 <= (void*)4)goto _LL1B5;if(*((int*)_tmp1F2)!= 21)goto
_LL1B3;_tmp1F3=((struct Cyc_Absyn_JoinEff_struct*)_tmp1F2)->f1;_tmp1F4=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1F2)->f1;_LL1B2: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1F4;for(0;effs != 0;effs=effs->tl){void*_tmp1F6=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1F6)));{void*
_tmp1F7=(void*)effs->hd;void*_tmp1F8;_LL1B8: if(_tmp1F7 <= (void*)4)goto _LL1BC;if(*((
int*)_tmp1F7)!= 21)goto _LL1BA;_LL1B9: goto _LL1BB;_LL1BA: if(*((int*)_tmp1F7)!= 20)
goto _LL1BC;_tmp1F8=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1F7)->f1;if((
int)_tmp1F8 != 2)goto _LL1BC;_LL1BB: redo_join=1;goto _LL1B7;_LL1BC:;_LL1BD: goto
_LL1B7;_LL1B7:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1F4;for(0;effs != 0;effs=effs->tl){void*_tmp1F9=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1FA;void*_tmp1FB;_LL1BF: if(_tmp1F9 <= (
void*)4)goto _LL1C3;if(*((int*)_tmp1F9)!= 21)goto _LL1C1;_tmp1FA=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F9)->f1;_LL1C0: effects=Cyc_List_revappend(_tmp1FA,effects);goto _LL1BE;
_LL1C1: if(*((int*)_tmp1F9)!= 20)goto _LL1C3;_tmp1FB=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F9)->f1;if((int)_tmp1FB != 2)goto _LL1C3;_LL1C2: goto _LL1BE;_LL1C3:;_LL1C4:
effects=({struct Cyc_List_List*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->hd=(
void*)_tmp1F9;_tmp1FC->tl=effects;_tmp1FC;});goto _LL1BE;_LL1BE:;}}*_tmp1F4=Cyc_List_imp_rev(
effects);return e;}}_LL1B3: if(*((int*)_tmp1F2)!= 22)goto _LL1B5;_tmp1F5=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1F2)->f1;_LL1B4: {void*_tmp1FD=Cyc_Tcutil_compress(
_tmp1F5);_LL1C6: if(_tmp1FD <= (void*)4)goto _LL1CA;if(*((int*)_tmp1FD)!= 0)goto
_LL1C8;_LL1C7: goto _LL1C9;_LL1C8: if(*((int*)_tmp1FD)!= 1)goto _LL1CA;_LL1C9: return
e;_LL1CA:;_LL1CB: return Cyc_Tcutil_rgns_of(_tmp1F5);_LL1C5:;}_LL1B5:;_LL1B6:
return e;_LL1B0:;}}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp1FE=({struct Cyc_Absyn_FnType_struct*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));
_tmp1FF[0]=({struct Cyc_Absyn_FnType_struct _tmp200;_tmp200.tag=8;_tmp200.f1=({
struct Cyc_Absyn_FnInfo _tmp201;_tmp201.tvars=0;_tmp201.effect=({struct Cyc_Core_Opt*
_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->v=(void*)eff;_tmp202;});_tmp201.ret_typ=(
void*)((void*)0);_tmp201.args=0;_tmp201.c_varargs=0;_tmp201.cyc_varargs=0;
_tmp201.rgn_po=0;_tmp201.attributes=0;_tmp201;});_tmp200;});_tmp1FF;});return Cyc_Absyn_atb_typ((
void*)_tmp1FE,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2  || r == (void*)3)return 1;{void*_tmp203=Cyc_Tcutil_compress(e);
void*_tmp204;struct Cyc_List_List*_tmp205;void*_tmp206;struct Cyc_Core_Opt*_tmp207;
struct Cyc_Core_Opt*_tmp208;struct Cyc_Core_Opt**_tmp209;struct Cyc_Core_Opt*
_tmp20A;_LL1CD: if(_tmp203 <= (void*)4)goto _LL1D5;if(*((int*)_tmp203)!= 20)goto
_LL1CF;_tmp204=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp203)->f1;_LL1CE:
if(constrain)return Cyc_Tcutil_unify(r,_tmp204);_tmp204=Cyc_Tcutil_compress(
_tmp204);if(r == _tmp204)return 1;{struct _tuple0 _tmp20C=({struct _tuple0 _tmp20B;
_tmp20B.f1=r;_tmp20B.f2=_tmp204;_tmp20B;});void*_tmp20D;struct Cyc_Absyn_Tvar*
_tmp20E;void*_tmp20F;struct Cyc_Absyn_Tvar*_tmp210;_LL1D8: _tmp20D=_tmp20C.f1;if(
_tmp20D <= (void*)4)goto _LL1DA;if(*((int*)_tmp20D)!= 1)goto _LL1DA;_tmp20E=((
struct Cyc_Absyn_VarType_struct*)_tmp20D)->f1;_tmp20F=_tmp20C.f2;if(_tmp20F <= (
void*)4)goto _LL1DA;if(*((int*)_tmp20F)!= 1)goto _LL1DA;_tmp210=((struct Cyc_Absyn_VarType_struct*)
_tmp20F)->f1;_LL1D9: return Cyc_Absyn_tvar_cmp(_tmp20E,_tmp210)== 0;_LL1DA:;_LL1DB:
return 0;_LL1D7:;}_LL1CF: if(*((int*)_tmp203)!= 21)goto _LL1D1;_tmp205=((struct Cyc_Absyn_JoinEff_struct*)
_tmp203)->f1;_LL1D0: for(0;_tmp205 != 0;_tmp205=_tmp205->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp205->hd))return 1;}return 0;_LL1D1: if(*((int*)_tmp203)!= 22)
goto _LL1D3;_tmp206=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp203)->f1;_LL1D2: {
void*_tmp211=Cyc_Tcutil_rgns_of(_tmp206);void*_tmp212;_LL1DD: if(_tmp211 <= (void*)
4)goto _LL1DF;if(*((int*)_tmp211)!= 22)goto _LL1DF;_tmp212=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp211)->f1;_LL1DE: if(!constrain)return 0;{void*_tmp213=Cyc_Tcutil_compress(
_tmp212);struct Cyc_Core_Opt*_tmp214;struct Cyc_Core_Opt*_tmp215;struct Cyc_Core_Opt**
_tmp216;struct Cyc_Core_Opt*_tmp217;_LL1E2: if(_tmp213 <= (void*)4)goto _LL1E4;if(*((
int*)_tmp213)!= 0)goto _LL1E4;_tmp214=((struct Cyc_Absyn_Evar_struct*)_tmp213)->f1;
_tmp215=((struct Cyc_Absyn_Evar_struct*)_tmp213)->f2;_tmp216=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp213)->f2;_tmp217=((struct Cyc_Absyn_Evar_struct*)
_tmp213)->f4;_LL1E3: {void*_tmp218=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp217);Cyc_Tcutil_occurs(_tmp218,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp217))->v,r);{void*_tmp219=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));
_tmp21B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp21C;_tmp21C.tag=21;_tmp21C.f1=({
void*_tmp21D[2];_tmp21D[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp21E=
_cycalloc(sizeof(*_tmp21E));_tmp21E[0]=({struct Cyc_Absyn_AccessEff_struct _tmp21F;
_tmp21F.tag=20;_tmp21F.f1=(void*)r;_tmp21F;});_tmp21E;});_tmp21D[0]=_tmp218;Cyc_List_list(
_tag_dynforward(_tmp21D,sizeof(void*),2));});_tmp21C;});_tmp21B;}));*_tmp216=({
struct Cyc_Core_Opt*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A->v=(void*)_tmp219;
_tmp21A;});return 1;}}_LL1E4:;_LL1E5: return 0;_LL1E1:;}_LL1DF:;_LL1E0: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp211);_LL1DC:;}_LL1D3: if(*((int*)_tmp203)!= 0)goto _LL1D5;_tmp207=((
struct Cyc_Absyn_Evar_struct*)_tmp203)->f1;_tmp208=((struct Cyc_Absyn_Evar_struct*)
_tmp203)->f2;_tmp209=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp203)->f2;_tmp20A=((struct Cyc_Absyn_Evar_struct*)_tmp203)->f4;_LL1D4: if(
_tmp207 == 0  || (void*)_tmp207->v != (void*)6)({void*_tmp220[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp221="effect evar has wrong kind";_tag_dynforward(_tmp221,sizeof(char),
_get_zero_arr_size(_tmp221,27));}),_tag_dynforward(_tmp220,sizeof(void*),0));});
if(!constrain)return 0;{void*_tmp222=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp20A);Cyc_Tcutil_occurs(_tmp222,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20A))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp223=({struct Cyc_Absyn_JoinEff_struct*_tmp225=_cycalloc(sizeof(*_tmp225));
_tmp225[0]=({struct Cyc_Absyn_JoinEff_struct _tmp226;_tmp226.tag=21;_tmp226.f1=({
struct Cyc_List_List*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->hd=(void*)
_tmp222;_tmp227->tl=({struct Cyc_List_List*_tmp228=_cycalloc(sizeof(*_tmp228));
_tmp228->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp229=_cycalloc(
sizeof(*_tmp229));_tmp229[0]=({struct Cyc_Absyn_AccessEff_struct _tmp22A;_tmp22A.tag=
20;_tmp22A.f1=(void*)r;_tmp22A;});_tmp229;}));_tmp228->tl=0;_tmp228;});_tmp227;});
_tmp226;});_tmp225;});*_tmp209=({struct Cyc_Core_Opt*_tmp224=_cycalloc(sizeof(*
_tmp224));_tmp224->v=(void*)((void*)_tmp223);_tmp224;});return 1;}}_LL1D5:;_LL1D6:
return 0;_LL1CC:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp22B=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp22C;void*_tmp22D;struct Cyc_Core_Opt*_tmp22E;struct
Cyc_Core_Opt*_tmp22F;struct Cyc_Core_Opt**_tmp230;struct Cyc_Core_Opt*_tmp231;
_LL1E7: if(_tmp22B <= (void*)4)goto _LL1EF;if(*((int*)_tmp22B)!= 20)goto _LL1E9;
_LL1E8: return 0;_LL1E9: if(*((int*)_tmp22B)!= 21)goto _LL1EB;_tmp22C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp22B)->f1;_LL1EA: for(0;_tmp22C != 0;_tmp22C=_tmp22C->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp22C->hd))return 1;}return 0;_LL1EB: if(*((int*)
_tmp22B)!= 22)goto _LL1ED;_tmp22D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp22B)->f1;_LL1EC: _tmp22D=Cyc_Tcutil_compress(_tmp22D);if(t == _tmp22D)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp22D);{void*_tmp232=Cyc_Tcutil_rgns_of(
t);void*_tmp233;_LL1F2: if(_tmp232 <= (void*)4)goto _LL1F4;if(*((int*)_tmp232)!= 22)
goto _LL1F4;_tmp233=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp232)->f1;_LL1F3: {
struct _tuple0 _tmp235=({struct _tuple0 _tmp234;_tmp234.f1=Cyc_Tcutil_compress(
_tmp233);_tmp234.f2=_tmp22D;_tmp234;});void*_tmp236;struct Cyc_Absyn_Tvar*_tmp237;
void*_tmp238;struct Cyc_Absyn_Tvar*_tmp239;_LL1F7: _tmp236=_tmp235.f1;if(_tmp236 <= (
void*)4)goto _LL1F9;if(*((int*)_tmp236)!= 1)goto _LL1F9;_tmp237=((struct Cyc_Absyn_VarType_struct*)
_tmp236)->f1;_tmp238=_tmp235.f2;if(_tmp238 <= (void*)4)goto _LL1F9;if(*((int*)
_tmp238)!= 1)goto _LL1F9;_tmp239=((struct Cyc_Absyn_VarType_struct*)_tmp238)->f1;
_LL1F8: return Cyc_Tcutil_unify(t,_tmp22D);_LL1F9:;_LL1FA: return _tmp233 == _tmp22D;
_LL1F6:;}_LL1F4:;_LL1F5: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp232);_LL1F1:;}_LL1ED: if(*((int*)_tmp22B)!= 0)goto _LL1EF;_tmp22E=((struct Cyc_Absyn_Evar_struct*)
_tmp22B)->f1;_tmp22F=((struct Cyc_Absyn_Evar_struct*)_tmp22B)->f2;_tmp230=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp22B)->f2;_tmp231=((struct Cyc_Absyn_Evar_struct*)
_tmp22B)->f4;_LL1EE: if(_tmp22E == 0  || (void*)_tmp22E->v != (void*)6)({void*
_tmp23A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp23B="effect evar has wrong kind";_tag_dynforward(_tmp23B,sizeof(
char),_get_zero_arr_size(_tmp23B,27));}),_tag_dynforward(_tmp23A,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{void*_tmp23C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp231);Cyc_Tcutil_occurs(_tmp23C,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp231))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp23D=({struct Cyc_Absyn_JoinEff_struct*_tmp23F=_cycalloc(sizeof(*_tmp23F));
_tmp23F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp240;_tmp240.tag=21;_tmp240.f1=({
struct Cyc_List_List*_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241->hd=(void*)
_tmp23C;_tmp241->tl=({struct Cyc_List_List*_tmp242=_cycalloc(sizeof(*_tmp242));
_tmp242->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp243=_cycalloc(
sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp244;_tmp244.tag=
22;_tmp244.f1=(void*)t;_tmp244;});_tmp243;}));_tmp242->tl=0;_tmp242;});_tmp241;});
_tmp240;});_tmp23F;});*_tmp230=({struct Cyc_Core_Opt*_tmp23E=_cycalloc(sizeof(*
_tmp23E));_tmp23E->v=(void*)((void*)_tmp23D);_tmp23E;});return 1;}}_LL1EF:;_LL1F0:
return 0;_LL1E6:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp245=e;struct
Cyc_Absyn_Tvar*_tmp246;struct Cyc_List_List*_tmp247;void*_tmp248;struct Cyc_Core_Opt*
_tmp249;struct Cyc_Core_Opt*_tmp24A;struct Cyc_Core_Opt**_tmp24B;struct Cyc_Core_Opt*
_tmp24C;_LL1FC: if(_tmp245 <= (void*)4)goto _LL204;if(*((int*)_tmp245)!= 1)goto
_LL1FE;_tmp246=((struct Cyc_Absyn_VarType_struct*)_tmp245)->f1;_LL1FD: return Cyc_Absyn_tvar_cmp(
v,_tmp246)== 0;_LL1FE: if(*((int*)_tmp245)!= 21)goto _LL200;_tmp247=((struct Cyc_Absyn_JoinEff_struct*)
_tmp245)->f1;_LL1FF: for(0;_tmp247 != 0;_tmp247=_tmp247->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp247->hd))return 1;}return 0;_LL200: if(*((int*)
_tmp245)!= 22)goto _LL202;_tmp248=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp245)->f1;_LL201: {void*_tmp24D=Cyc_Tcutil_rgns_of(_tmp248);void*_tmp24E;
_LL207: if(_tmp24D <= (void*)4)goto _LL209;if(*((int*)_tmp24D)!= 22)goto _LL209;
_tmp24E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp24D)->f1;_LL208: if(!
may_constrain_evars)return 0;{void*_tmp24F=Cyc_Tcutil_compress(_tmp24E);struct Cyc_Core_Opt*
_tmp250;struct Cyc_Core_Opt*_tmp251;struct Cyc_Core_Opt**_tmp252;struct Cyc_Core_Opt*
_tmp253;_LL20C: if(_tmp24F <= (void*)4)goto _LL20E;if(*((int*)_tmp24F)!= 0)goto
_LL20E;_tmp250=((struct Cyc_Absyn_Evar_struct*)_tmp24F)->f1;_tmp251=((struct Cyc_Absyn_Evar_struct*)
_tmp24F)->f2;_tmp252=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp24F)->f2;_tmp253=((struct Cyc_Absyn_Evar_struct*)_tmp24F)->f4;_LL20D: {void*
_tmp254=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp253);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp253))->v,v))return 0;{void*_tmp255=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp257=_cycalloc(sizeof(*_tmp257));
_tmp257[0]=({struct Cyc_Absyn_JoinEff_struct _tmp258;_tmp258.tag=21;_tmp258.f1=({
void*_tmp259[2];_tmp259[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp25A=
_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Absyn_VarType_struct _tmp25B;
_tmp25B.tag=1;_tmp25B.f1=v;_tmp25B;});_tmp25A;});_tmp259[0]=_tmp254;Cyc_List_list(
_tag_dynforward(_tmp259,sizeof(void*),2));});_tmp258;});_tmp257;}));*_tmp252=({
struct Cyc_Core_Opt*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->v=(void*)_tmp255;
_tmp256;});return 1;}}_LL20E:;_LL20F: return 0;_LL20B:;}_LL209:;_LL20A: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp24D);_LL206:;}_LL202: if(*((int*)_tmp245)!= 0)goto
_LL204;_tmp249=((struct Cyc_Absyn_Evar_struct*)_tmp245)->f1;_tmp24A=((struct Cyc_Absyn_Evar_struct*)
_tmp245)->f2;_tmp24B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp245)->f2;_tmp24C=((struct Cyc_Absyn_Evar_struct*)_tmp245)->f4;_LL203: if(
_tmp249 == 0  || (void*)_tmp249->v != (void*)6)({void*_tmp25C[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp25D="effect evar has wrong kind";_tag_dynforward(_tmp25D,sizeof(char),
_get_zero_arr_size(_tmp25D,27));}),_tag_dynforward(_tmp25C,sizeof(void*),0));});{
void*_tmp25E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp24C);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp24C))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp25F=({struct Cyc_Absyn_JoinEff_struct*_tmp261=_cycalloc(sizeof(*_tmp261));
_tmp261[0]=({struct Cyc_Absyn_JoinEff_struct _tmp262;_tmp262.tag=21;_tmp262.f1=({
struct Cyc_List_List*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->hd=(void*)
_tmp25E;_tmp263->tl=({struct Cyc_List_List*_tmp264=_cycalloc(sizeof(*_tmp264));
_tmp264->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp265=_cycalloc(
sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Absyn_VarType_struct _tmp266;_tmp266.tag=
1;_tmp266.f1=v;_tmp266;});_tmp265;}));_tmp264->tl=0;_tmp264;});_tmp263;});
_tmp262;});_tmp261;});*_tmp24B=({struct Cyc_Core_Opt*_tmp260=_cycalloc(sizeof(*
_tmp260));_tmp260->v=(void*)((void*)_tmp25F);_tmp260;});return 1;}}_LL204:;_LL205:
return 0;_LL1FB:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp267=e;struct Cyc_List_List*_tmp268;void*_tmp269;_LL211: if(_tmp267 <= (
void*)4)goto _LL217;if(*((int*)_tmp267)!= 21)goto _LL213;_tmp268=((struct Cyc_Absyn_JoinEff_struct*)
_tmp267)->f1;_LL212: for(0;_tmp268 != 0;_tmp268=_tmp268->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp268->hd))return 1;}return 0;_LL213: if(*((int*)_tmp267)!= 22)goto
_LL215;_tmp269=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp267)->f1;_LL214: {
void*_tmp26A=Cyc_Tcutil_rgns_of(_tmp269);void*_tmp26B;_LL21A: if(_tmp26A <= (void*)
4)goto _LL21C;if(*((int*)_tmp26A)!= 22)goto _LL21C;_tmp26B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp26A)->f1;_LL21B: return 0;_LL21C:;_LL21D: return Cyc_Tcutil_evar_in_effect(evar,
_tmp26A);_LL219:;}_LL215: if(*((int*)_tmp267)!= 0)goto _LL217;_LL216: return evar == 
e;_LL217:;_LL218: return 0;_LL210:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp26C=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp26D;void*_tmp26E;struct Cyc_Absyn_Tvar*_tmp26F;void*_tmp270;
struct Cyc_Core_Opt*_tmp271;struct Cyc_Core_Opt**_tmp272;struct Cyc_Core_Opt*
_tmp273;_LL21F: if(_tmp26C <= (void*)4)goto _LL229;if(*((int*)_tmp26C)!= 21)goto
_LL221;_tmp26D=((struct Cyc_Absyn_JoinEff_struct*)_tmp26C)->f1;_LL220: for(0;
_tmp26D != 0;_tmp26D=_tmp26D->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp26D->hd,e2))return 0;}return 1;_LL221: if(*((int*)
_tmp26C)!= 20)goto _LL223;_tmp26E=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp26C)->f1;_LL222: return Cyc_Tcutil_region_in_effect(0,_tmp26E,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp26E,(void*)2);_LL223: if(*((int*)
_tmp26C)!= 1)goto _LL225;_tmp26F=((struct Cyc_Absyn_VarType_struct*)_tmp26C)->f1;
_LL224: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp26F,e2);
_LL225: if(*((int*)_tmp26C)!= 22)goto _LL227;_tmp270=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp26C)->f1;_LL226: {void*_tmp274=Cyc_Tcutil_rgns_of(_tmp270);void*_tmp275;
_LL22C: if(_tmp274 <= (void*)4)goto _LL22E;if(*((int*)_tmp274)!= 22)goto _LL22E;
_tmp275=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp274)->f1;_LL22D: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp275,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp275,Cyc_Absyn_sint_typ);_LL22E:;_LL22F: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp274,e2);_LL22B:;}_LL227: if(*((int*)_tmp26C)!= 0)goto
_LL229;_tmp271=((struct Cyc_Absyn_Evar_struct*)_tmp26C)->f2;_tmp272=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp26C)->f2;_tmp273=((struct Cyc_Absyn_Evar_struct*)
_tmp26C)->f4;_LL228: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp272=({struct Cyc_Core_Opt*
_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276->v=(void*)Cyc_Absyn_empty_effect;
_tmp276;});return 1;_LL229:;_LL22A:({struct Cyc_String_pa_struct _tmp279;_tmp279.tag=
0;_tmp279.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp277[1]={& _tmp279};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp278="subset_effect: bad effect: %s";
_tag_dynforward(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,30));}),
_tag_dynforward(_tmp277,sizeof(void*),1));}});_LL21E:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){struct
_tuple0 _tmp27B;void*_tmp27C;void*_tmp27D;struct _tuple0*_tmp27A=(struct _tuple0*)
r1->hd;_tmp27B=*_tmp27A;_tmp27C=_tmp27B.f1;_tmp27D=_tmp27B.f2;{int found=_tmp27C
== (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0 _tmp27F;void*_tmp280;void*_tmp281;struct _tuple0*_tmp27E=(struct
_tuple0*)r2->hd;_tmp27F=*_tmp27E;_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;if(Cyc_Tcutil_unify(
_tmp27C,_tmp280) && Cyc_Tcutil_unify(_tmp27D,_tmp281)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp282=t1;struct Cyc_Core_Opt*_tmp283;struct Cyc_Core_Opt*
_tmp284;struct Cyc_Core_Opt**_tmp285;struct Cyc_Core_Opt*_tmp286;_LL231: if(_tmp282
<= (void*)4)goto _LL233;if(*((int*)_tmp282)!= 0)goto _LL233;_tmp283=((struct Cyc_Absyn_Evar_struct*)
_tmp282)->f1;_tmp284=((struct Cyc_Absyn_Evar_struct*)_tmp282)->f2;_tmp285=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp282)->f2;_tmp286=((struct Cyc_Absyn_Evar_struct*)
_tmp282)->f4;_LL232: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp286))->v,t2);{void*_tmp287=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp287,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp283))->v)){*_tmp285=({struct Cyc_Core_Opt*_tmp288=_cycalloc(sizeof(*_tmp288));
_tmp288->v=(void*)t2;_tmp288;});return;}else{{void*_tmp289=t2;struct Cyc_Core_Opt*
_tmp28A;struct Cyc_Core_Opt**_tmp28B;struct Cyc_Core_Opt*_tmp28C;struct Cyc_Absyn_PtrInfo
_tmp28D;_LL236: if(_tmp289 <= (void*)4)goto _LL23A;if(*((int*)_tmp289)!= 0)goto
_LL238;_tmp28A=((struct Cyc_Absyn_Evar_struct*)_tmp289)->f2;_tmp28B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp289)->f2;_tmp28C=((struct Cyc_Absyn_Evar_struct*)
_tmp289)->f4;_LL237: {struct Cyc_List_List*_tmp28E=(struct Cyc_List_List*)_tmp286->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp28C))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp28E,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp28F="(type variable would escape scope)";_tag_dynforward(_tmp28F,
sizeof(char),_get_zero_arr_size(_tmp28F,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp283->v,_tmp287)){*_tmp28B=({struct Cyc_Core_Opt*
_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290->v=(void*)t1;_tmp290;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp291="(kinds are incompatible)";_tag_dynforward(_tmp291,sizeof(char),
_get_zero_arr_size(_tmp291,25));});goto _LL235;}_LL238: if(*((int*)_tmp289)!= 4)
goto _LL23A;_tmp28D=((struct Cyc_Absyn_PointerType_struct*)_tmp289)->f1;if(!((void*)
_tmp283->v == (void*)2))goto _LL23A;_LL239: {struct Cyc_Absyn_Conref*_tmp292=Cyc_Absyn_compress_conref((
_tmp28D.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union _tmp293=_tmp292->v;
_LL23D: if((_tmp293.No_constr).tag != 2)goto _LL23F;_LL23E: _tmp292->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp294;(_tmp294.Eq_constr).tag=0;(_tmp294.Eq_constr).f1=(
void*)Cyc_Absyn_bounds_one;_tmp294;});*_tmp285=({struct Cyc_Core_Opt*_tmp295=
_cycalloc(sizeof(*_tmp295));_tmp295->v=(void*)t2;_tmp295;});return;_LL23F:;
_LL240: goto _LL23C;_LL23C:;}goto _LL235;}_LL23A:;_LL23B: goto _LL235;_LL235:;}Cyc_Tcutil_failure_reason=({
const char*_tmp296="(kinds are incompatible)";_tag_dynforward(_tmp296,sizeof(char),
_get_zero_arr_size(_tmp296,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}}_LL233:;
_LL234: goto _LL230;_LL230:;}{struct _tuple0 _tmp298=({struct _tuple0 _tmp297;_tmp297.f1=
t2;_tmp297.f2=t1;_tmp297;});void*_tmp299;void*_tmp29A;void*_tmp29B;void*_tmp29C;
struct Cyc_Absyn_Tvar*_tmp29D;void*_tmp29E;struct Cyc_Absyn_Tvar*_tmp29F;void*
_tmp2A0;struct Cyc_Absyn_AggrInfo _tmp2A1;union Cyc_Absyn_AggrInfoU_union _tmp2A2;
struct Cyc_List_List*_tmp2A3;void*_tmp2A4;struct Cyc_Absyn_AggrInfo _tmp2A5;union
Cyc_Absyn_AggrInfoU_union _tmp2A6;struct Cyc_List_List*_tmp2A7;void*_tmp2A8;struct
_tuple2*_tmp2A9;void*_tmp2AA;struct _tuple2*_tmp2AB;void*_tmp2AC;struct Cyc_List_List*
_tmp2AD;void*_tmp2AE;struct Cyc_List_List*_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_TunionInfo
_tmp2B1;union Cyc_Absyn_TunionInfoU_union _tmp2B2;struct Cyc_Absyn_Tuniondecl**
_tmp2B3;struct Cyc_Absyn_Tuniondecl*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Core_Opt*
_tmp2B6;void*_tmp2B7;struct Cyc_Absyn_TunionInfo _tmp2B8;union Cyc_Absyn_TunionInfoU_union
_tmp2B9;struct Cyc_Absyn_Tuniondecl**_tmp2BA;struct Cyc_Absyn_Tuniondecl*_tmp2BB;
struct Cyc_List_List*_tmp2BC;struct Cyc_Core_Opt*_tmp2BD;void*_tmp2BE;struct Cyc_Absyn_TunionFieldInfo
_tmp2BF;union Cyc_Absyn_TunionFieldInfoU_union _tmp2C0;struct Cyc_Absyn_Tuniondecl*
_tmp2C1;struct Cyc_Absyn_Tunionfield*_tmp2C2;struct Cyc_List_List*_tmp2C3;void*
_tmp2C4;struct Cyc_Absyn_TunionFieldInfo _tmp2C5;union Cyc_Absyn_TunionFieldInfoU_union
_tmp2C6;struct Cyc_Absyn_Tuniondecl*_tmp2C7;struct Cyc_Absyn_Tunionfield*_tmp2C8;
struct Cyc_List_List*_tmp2C9;void*_tmp2CA;struct Cyc_Absyn_PtrInfo _tmp2CB;void*
_tmp2CC;struct Cyc_Absyn_Tqual _tmp2CD;struct Cyc_Absyn_PtrAtts _tmp2CE;void*_tmp2CF;
struct Cyc_Absyn_Conref*_tmp2D0;struct Cyc_Absyn_Conref*_tmp2D1;struct Cyc_Absyn_Conref*
_tmp2D2;void*_tmp2D3;struct Cyc_Absyn_PtrInfo _tmp2D4;void*_tmp2D5;struct Cyc_Absyn_Tqual
_tmp2D6;struct Cyc_Absyn_PtrAtts _tmp2D7;void*_tmp2D8;struct Cyc_Absyn_Conref*
_tmp2D9;struct Cyc_Absyn_Conref*_tmp2DA;struct Cyc_Absyn_Conref*_tmp2DB;void*
_tmp2DC;void*_tmp2DD;void*_tmp2DE;void*_tmp2DF;void*_tmp2E0;void*_tmp2E1;void*
_tmp2E2;void*_tmp2E3;void*_tmp2E4;int _tmp2E5;void*_tmp2E6;int _tmp2E7;void*
_tmp2E8;void*_tmp2E9;void*_tmp2EA;void*_tmp2EB;void*_tmp2EC;int _tmp2ED;void*
_tmp2EE;int _tmp2EF;void*_tmp2F0;void*_tmp2F1;void*_tmp2F2;void*_tmp2F3;void*
_tmp2F4;struct Cyc_Absyn_ArrayInfo _tmp2F5;void*_tmp2F6;struct Cyc_Absyn_Tqual
_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F8;struct Cyc_Absyn_Conref*_tmp2F9;void*_tmp2FA;
struct Cyc_Absyn_ArrayInfo _tmp2FB;void*_tmp2FC;struct Cyc_Absyn_Tqual _tmp2FD;
struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Conref*_tmp2FF;void*_tmp300;struct
Cyc_Absyn_FnInfo _tmp301;struct Cyc_List_List*_tmp302;struct Cyc_Core_Opt*_tmp303;
void*_tmp304;struct Cyc_List_List*_tmp305;int _tmp306;struct Cyc_Absyn_VarargInfo*
_tmp307;struct Cyc_List_List*_tmp308;struct Cyc_List_List*_tmp309;void*_tmp30A;
struct Cyc_Absyn_FnInfo _tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Core_Opt*
_tmp30D;void*_tmp30E;struct Cyc_List_List*_tmp30F;int _tmp310;struct Cyc_Absyn_VarargInfo*
_tmp311;struct Cyc_List_List*_tmp312;struct Cyc_List_List*_tmp313;void*_tmp314;
struct Cyc_List_List*_tmp315;void*_tmp316;struct Cyc_List_List*_tmp317;void*
_tmp318;void*_tmp319;struct Cyc_List_List*_tmp31A;void*_tmp31B;void*_tmp31C;
struct Cyc_List_List*_tmp31D;void*_tmp31E;void*_tmp31F;void*_tmp320;void*_tmp321;
void*_tmp322;void*_tmp323;void*_tmp324;void*_tmp325;void*_tmp326;void*_tmp327;
void*_tmp328;void*_tmp329;void*_tmp32A;void*_tmp32B;void*_tmp32C;void*_tmp32D;
void*_tmp32E;void*_tmp32F;void*_tmp330;void*_tmp331;_LL242: _tmp299=_tmp298.f1;
if(_tmp299 <= (void*)4)goto _LL244;if(*((int*)_tmp299)!= 0)goto _LL244;_LL243: Cyc_Tcutil_unify_it(
t2,t1);return;_LL244: _tmp29A=_tmp298.f1;if((int)_tmp29A != 0)goto _LL246;_tmp29B=
_tmp298.f2;if((int)_tmp29B != 0)goto _LL246;_LL245: return;_LL246: _tmp29C=_tmp298.f1;
if(_tmp29C <= (void*)4)goto _LL248;if(*((int*)_tmp29C)!= 1)goto _LL248;_tmp29D=((
struct Cyc_Absyn_VarType_struct*)_tmp29C)->f1;_tmp29E=_tmp298.f2;if(_tmp29E <= (
void*)4)goto _LL248;if(*((int*)_tmp29E)!= 1)goto _LL248;_tmp29F=((struct Cyc_Absyn_VarType_struct*)
_tmp29E)->f1;_LL247: {struct _dynforward_ptr*_tmp332=_tmp29D->name;struct
_dynforward_ptr*_tmp333=_tmp29F->name;int _tmp334=_tmp29D->identity;int _tmp335=
_tmp29F->identity;void*_tmp336=Cyc_Tcutil_tvar_kind(_tmp29D);void*_tmp337=Cyc_Tcutil_tvar_kind(
_tmp29F);if(_tmp335 == _tmp334  && Cyc_strptrcmp(_tmp332,_tmp333)== 0){if(_tmp336
!= _tmp337)({struct Cyc_String_pa_struct _tmp33C;_tmp33C.tag=0;_tmp33C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp337));{
struct Cyc_String_pa_struct _tmp33B;_tmp33B.tag=0;_tmp33B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp336));{
struct Cyc_String_pa_struct _tmp33A;_tmp33A.tag=0;_tmp33A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp332);{void*_tmp338[3]={& _tmp33A,&
_tmp33B,& _tmp33C};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp339="same type variable %s has kinds %s and %s";_tag_dynforward(
_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,42));}),_tag_dynforward(_tmp338,
sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*_tmp33D="(variable types are not the same)";
_tag_dynforward(_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,34));});goto
_LL241;}_LL248: _tmp2A0=_tmp298.f1;if(_tmp2A0 <= (void*)4)goto _LL24A;if(*((int*)
_tmp2A0)!= 10)goto _LL24A;_tmp2A1=((struct Cyc_Absyn_AggrType_struct*)_tmp2A0)->f1;
_tmp2A2=_tmp2A1.aggr_info;_tmp2A3=_tmp2A1.targs;_tmp2A4=_tmp298.f2;if(_tmp2A4 <= (
void*)4)goto _LL24A;if(*((int*)_tmp2A4)!= 10)goto _LL24A;_tmp2A5=((struct Cyc_Absyn_AggrType_struct*)
_tmp2A4)->f1;_tmp2A6=_tmp2A5.aggr_info;_tmp2A7=_tmp2A5.targs;_LL249: {void*
_tmp33F;struct _tuple2*_tmp340;struct _tuple6 _tmp33E=Cyc_Absyn_aggr_kinded_name(
_tmp2A6);_tmp33F=_tmp33E.f1;_tmp340=_tmp33E.f2;{void*_tmp342;struct _tuple2*
_tmp343;struct _tuple6 _tmp341=Cyc_Absyn_aggr_kinded_name(_tmp2A2);_tmp342=_tmp341.f1;
_tmp343=_tmp341.f2;if(_tmp33F != _tmp342){Cyc_Tcutil_failure_reason=({const char*
_tmp344="(struct and union type)";_tag_dynforward(_tmp344,sizeof(char),
_get_zero_arr_size(_tmp344,24));});goto _LL241;}if(Cyc_Absyn_qvar_cmp(_tmp340,
_tmp343)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp345="(different type name)";
_tag_dynforward(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,22));});goto
_LL241;}Cyc_Tcutil_unify_list(_tmp2A7,_tmp2A3);return;}}_LL24A: _tmp2A8=_tmp298.f1;
if(_tmp2A8 <= (void*)4)goto _LL24C;if(*((int*)_tmp2A8)!= 12)goto _LL24C;_tmp2A9=((
struct Cyc_Absyn_EnumType_struct*)_tmp2A8)->f1;_tmp2AA=_tmp298.f2;if(_tmp2AA <= (
void*)4)goto _LL24C;if(*((int*)_tmp2AA)!= 12)goto _LL24C;_tmp2AB=((struct Cyc_Absyn_EnumType_struct*)
_tmp2AA)->f1;_LL24B: if(Cyc_Absyn_qvar_cmp(_tmp2A9,_tmp2AB)== 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp346="(different enum types)";_tag_dynforward(_tmp346,sizeof(char),
_get_zero_arr_size(_tmp346,23));});goto _LL241;_LL24C: _tmp2AC=_tmp298.f1;if(
_tmp2AC <= (void*)4)goto _LL24E;if(*((int*)_tmp2AC)!= 13)goto _LL24E;_tmp2AD=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp2AC)->f1;_tmp2AE=_tmp298.f2;if(_tmp2AE
<= (void*)4)goto _LL24E;if(*((int*)_tmp2AE)!= 13)goto _LL24E;_tmp2AF=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2AE)->f1;_LL24D: {int bad=0;for(0;_tmp2AD != 0  && _tmp2AF != 0;(_tmp2AD=_tmp2AD->tl,
_tmp2AF=_tmp2AF->tl)){struct Cyc_Absyn_Enumfield*_tmp347=(struct Cyc_Absyn_Enumfield*)
_tmp2AD->hd;struct Cyc_Absyn_Enumfield*_tmp348=(struct Cyc_Absyn_Enumfield*)
_tmp2AF->hd;if(Cyc_Absyn_qvar_cmp(_tmp347->name,_tmp348->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp349="(different names for enum fields)";_tag_dynforward(_tmp349,
sizeof(char),_get_zero_arr_size(_tmp349,34));});bad=1;break;}if(_tmp347->tag == 
_tmp348->tag)continue;if(_tmp347->tag == 0  || _tmp348->tag == 0){Cyc_Tcutil_failure_reason=({
const char*_tmp34A="(different tag values for enum fields)";_tag_dynforward(
_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,39));});bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp347->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp348->tag))){Cyc_Tcutil_failure_reason=({const char*_tmp34B="(different tag values for enum fields)";
_tag_dynforward(_tmp34B,sizeof(char),_get_zero_arr_size(_tmp34B,39));});bad=1;
break;}}if(bad)goto _LL241;if(_tmp2AD == 0  && _tmp2AF == 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp34C="(different number of fields for enums)";_tag_dynforward(
_tmp34C,sizeof(char),_get_zero_arr_size(_tmp34C,39));});goto _LL241;}_LL24E:
_tmp2B0=_tmp298.f1;if(_tmp2B0 <= (void*)4)goto _LL250;if(*((int*)_tmp2B0)!= 2)goto
_LL250;_tmp2B1=((struct Cyc_Absyn_TunionType_struct*)_tmp2B0)->f1;_tmp2B2=_tmp2B1.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp2B0)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL250;_tmp2B3=(_tmp2B2.KnownTunion).f1;_tmp2B4=*_tmp2B3;_tmp2B5=
_tmp2B1.targs;_tmp2B6=_tmp2B1.rgn;_tmp2B7=_tmp298.f2;if(_tmp2B7 <= (void*)4)goto
_LL250;if(*((int*)_tmp2B7)!= 2)goto _LL250;_tmp2B8=((struct Cyc_Absyn_TunionType_struct*)
_tmp2B7)->f1;_tmp2B9=_tmp2B8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp2B7)->f1).tunion_info).KnownTunion).tag != 1)goto _LL250;_tmp2BA=(_tmp2B9.KnownTunion).f1;
_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2B8.targs;_tmp2BD=_tmp2B8.rgn;_LL24F: if(_tmp2B4 == 
_tmp2BB  || Cyc_Absyn_qvar_cmp(_tmp2B4->name,_tmp2BB->name)== 0){if(_tmp2BD != 0
 && _tmp2B6 != 0)Cyc_Tcutil_unify_it((void*)_tmp2BD->v,(void*)_tmp2B6->v);else{
if(_tmp2BD != 0  || _tmp2B6 != 0)goto Tunion_fail;}Cyc_Tcutil_unify_list(_tmp2BC,
_tmp2B5);return;}Tunion_fail: Cyc_Tcutil_failure_reason=({const char*_tmp34D="(different tunion types)";
_tag_dynforward(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,25));});goto
_LL241;_LL250: _tmp2BE=_tmp298.f1;if(_tmp2BE <= (void*)4)goto _LL252;if(*((int*)
_tmp2BE)!= 3)goto _LL252;_tmp2BF=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2BE)->f1;_tmp2C0=_tmp2BF.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2BE)->f1).field_info).KnownTunionfield).tag != 1)goto _LL252;_tmp2C1=(_tmp2C0.KnownTunionfield).f1;
_tmp2C2=(_tmp2C0.KnownTunionfield).f2;_tmp2C3=_tmp2BF.targs;_tmp2C4=_tmp298.f2;
if(_tmp2C4 <= (void*)4)goto _LL252;if(*((int*)_tmp2C4)!= 3)goto _LL252;_tmp2C5=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2C4)->f1;_tmp2C6=_tmp2C5.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2C4)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL252;_tmp2C7=(_tmp2C6.KnownTunionfield).f1;_tmp2C8=(_tmp2C6.KnownTunionfield).f2;
_tmp2C9=_tmp2C5.targs;_LL251: if((_tmp2C1 == _tmp2C7  || Cyc_Absyn_qvar_cmp(_tmp2C1->name,
_tmp2C7->name)== 0) && (_tmp2C2 == _tmp2C8  || Cyc_Absyn_qvar_cmp(_tmp2C2->name,
_tmp2C8->name)== 0)){Cyc_Tcutil_unify_list(_tmp2C9,_tmp2C3);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp34E="(different tunion field types)";_tag_dynforward(_tmp34E,
sizeof(char),_get_zero_arr_size(_tmp34E,31));});goto _LL241;_LL252: _tmp2CA=
_tmp298.f1;if(_tmp2CA <= (void*)4)goto _LL254;if(*((int*)_tmp2CA)!= 4)goto _LL254;
_tmp2CB=((struct Cyc_Absyn_PointerType_struct*)_tmp2CA)->f1;_tmp2CC=(void*)
_tmp2CB.elt_typ;_tmp2CD=_tmp2CB.elt_tq;_tmp2CE=_tmp2CB.ptr_atts;_tmp2CF=(void*)
_tmp2CE.rgn;_tmp2D0=_tmp2CE.nullable;_tmp2D1=_tmp2CE.bounds;_tmp2D2=_tmp2CE.zero_term;
_tmp2D3=_tmp298.f2;if(_tmp2D3 <= (void*)4)goto _LL254;if(*((int*)_tmp2D3)!= 4)goto
_LL254;_tmp2D4=((struct Cyc_Absyn_PointerType_struct*)_tmp2D3)->f1;_tmp2D5=(void*)
_tmp2D4.elt_typ;_tmp2D6=_tmp2D4.elt_tq;_tmp2D7=_tmp2D4.ptr_atts;_tmp2D8=(void*)
_tmp2D7.rgn;_tmp2D9=_tmp2D7.nullable;_tmp2DA=_tmp2D7.bounds;_tmp2DB=_tmp2D7.zero_term;
_LL253: Cyc_Tcutil_unify_it(_tmp2D5,_tmp2CC);Cyc_Tcutil_unify_it(_tmp2CF,_tmp2D8);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2DB,_tmp2D2,({const char*_tmp34F="(not both zero terminated)";
_tag_dynforward(_tmp34F,sizeof(char),_get_zero_arr_size(_tmp34F,27));}));Cyc_Tcutil_unify_tqual(
_tmp2D6,_tmp2D5,_tmp2CD,_tmp2CC);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2DA,_tmp2D1,({const char*_tmp350="(different pointer bounds)";_tag_dynforward(
_tmp350,sizeof(char),_get_zero_arr_size(_tmp350,27));}));{union Cyc_Absyn_Constraint_union
_tmp351=(Cyc_Absyn_compress_conref(_tmp2DA))->v;void*_tmp352;void*_tmp353;_LL27F:
if((_tmp351.Eq_constr).tag != 0)goto _LL281;_tmp352=(_tmp351.Eq_constr).f1;if((int)
_tmp352 != 0)goto _LL281;_LL280: return;_LL281: if((_tmp351.Eq_constr).tag != 0)goto
_LL283;_tmp353=(_tmp351.Eq_constr).f1;if((int)_tmp353 != 1)goto _LL283;_LL282:
return;_LL283:;_LL284: goto _LL27E;_LL27E:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2D9,_tmp2D0,({const char*_tmp354="(incompatible pointer types)";
_tag_dynforward(_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,29));}));return;
_LL254: _tmp2DC=_tmp298.f1;if(_tmp2DC <= (void*)4)goto _LL256;if(*((int*)_tmp2DC)!= 
5)goto _LL256;_tmp2DD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DC)->f1;
_tmp2DE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DC)->f2;_tmp2DF=_tmp298.f2;
if(_tmp2DF <= (void*)4)goto _LL256;if(*((int*)_tmp2DF)!= 5)goto _LL256;_tmp2E0=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DF)->f1;_tmp2E1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2DF)->f2;_LL255: if(_tmp2E0 == _tmp2DD  && ((_tmp2E1 == _tmp2DE  || _tmp2E1 == (
void*)2  && _tmp2DE == (void*)3) || _tmp2E1 == (void*)3  && _tmp2DE == (void*)2))
return;Cyc_Tcutil_failure_reason=({const char*_tmp355="(different integral types)";
_tag_dynforward(_tmp355,sizeof(char),_get_zero_arr_size(_tmp355,27));});goto
_LL241;_LL256: _tmp2E2=_tmp298.f1;if((int)_tmp2E2 != 1)goto _LL258;_tmp2E3=_tmp298.f2;
if((int)_tmp2E3 != 1)goto _LL258;_LL257: return;_LL258: _tmp2E4=_tmp298.f1;if(
_tmp2E4 <= (void*)4)goto _LL25A;if(*((int*)_tmp2E4)!= 6)goto _LL25A;_tmp2E5=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2E4)->f1;_tmp2E6=_tmp298.f2;if(_tmp2E6 <= (
void*)4)goto _LL25A;if(*((int*)_tmp2E6)!= 6)goto _LL25A;_tmp2E7=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2E6)->f1;_LL259: if(_tmp2E5 == _tmp2E7)return;goto _LL241;_LL25A: _tmp2E8=
_tmp298.f1;if(_tmp2E8 <= (void*)4)goto _LL25C;if(*((int*)_tmp2E8)!= 14)goto _LL25C;
_tmp2E9=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2E8)->f1;_tmp2EA=_tmp298.f2;
if(_tmp2EA <= (void*)4)goto _LL25C;if(*((int*)_tmp2EA)!= 14)goto _LL25C;_tmp2EB=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2EA)->f1;_LL25B: Cyc_Tcutil_unify_it(
_tmp2E9,_tmp2EB);return;_LL25C: _tmp2EC=_tmp298.f1;if(_tmp2EC <= (void*)4)goto
_LL25E;if(*((int*)_tmp2EC)!= 19)goto _LL25E;_tmp2ED=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2EC)->f1;_tmp2EE=_tmp298.f2;if(_tmp2EE <= (void*)4)goto _LL25E;if(*((int*)
_tmp2EE)!= 19)goto _LL25E;_tmp2EF=((struct Cyc_Absyn_TypeInt_struct*)_tmp2EE)->f1;
_LL25D: if(_tmp2ED == _tmp2EF)return;Cyc_Tcutil_failure_reason=({const char*_tmp356="(different type integers)";
_tag_dynforward(_tmp356,sizeof(char),_get_zero_arr_size(_tmp356,26));});goto
_LL241;_LL25E: _tmp2F0=_tmp298.f1;if(_tmp2F0 <= (void*)4)goto _LL260;if(*((int*)
_tmp2F0)!= 18)goto _LL260;_tmp2F1=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2F0)->f1;_tmp2F2=_tmp298.f2;if(_tmp2F2 <= (void*)4)goto _LL260;if(*((int*)
_tmp2F2)!= 18)goto _LL260;_tmp2F3=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2F2)->f1;_LL25F: Cyc_Tcutil_unify_it(_tmp2F1,_tmp2F3);return;_LL260: _tmp2F4=
_tmp298.f1;if(_tmp2F4 <= (void*)4)goto _LL262;if(*((int*)_tmp2F4)!= 7)goto _LL262;
_tmp2F5=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F4)->f1;_tmp2F6=(void*)_tmp2F5.elt_type;
_tmp2F7=_tmp2F5.tq;_tmp2F8=_tmp2F5.num_elts;_tmp2F9=_tmp2F5.zero_term;_tmp2FA=
_tmp298.f2;if(_tmp2FA <= (void*)4)goto _LL262;if(*((int*)_tmp2FA)!= 7)goto _LL262;
_tmp2FB=((struct Cyc_Absyn_ArrayType_struct*)_tmp2FA)->f1;_tmp2FC=(void*)_tmp2FB.elt_type;
_tmp2FD=_tmp2FB.tq;_tmp2FE=_tmp2FB.num_elts;_tmp2FF=_tmp2FB.zero_term;_LL261: Cyc_Tcutil_unify_it(
_tmp2FC,_tmp2F6);Cyc_Tcutil_unify_tqual(_tmp2FD,_tmp2FC,_tmp2F7,_tmp2F6);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2F9,_tmp2FF,({const char*_tmp357="(not both zero terminated)";
_tag_dynforward(_tmp357,sizeof(char),_get_zero_arr_size(_tmp357,27));}));if(
_tmp2F8 == _tmp2FE)return;if(_tmp2F8 == 0  || _tmp2FE == 0)goto _LL241;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp2F8,(struct Cyc_Absyn_Exp*)_tmp2FE))return;Cyc_Tcutil_failure_reason=({
const char*_tmp358="(different array sizes)";_tag_dynforward(_tmp358,sizeof(char),
_get_zero_arr_size(_tmp358,24));});goto _LL241;_LL262: _tmp300=_tmp298.f1;if(
_tmp300 <= (void*)4)goto _LL264;if(*((int*)_tmp300)!= 8)goto _LL264;_tmp301=((
struct Cyc_Absyn_FnType_struct*)_tmp300)->f1;_tmp302=_tmp301.tvars;_tmp303=
_tmp301.effect;_tmp304=(void*)_tmp301.ret_typ;_tmp305=_tmp301.args;_tmp306=
_tmp301.c_varargs;_tmp307=_tmp301.cyc_varargs;_tmp308=_tmp301.rgn_po;_tmp309=
_tmp301.attributes;_tmp30A=_tmp298.f2;if(_tmp30A <= (void*)4)goto _LL264;if(*((int*)
_tmp30A)!= 8)goto _LL264;_tmp30B=((struct Cyc_Absyn_FnType_struct*)_tmp30A)->f1;
_tmp30C=_tmp30B.tvars;_tmp30D=_tmp30B.effect;_tmp30E=(void*)_tmp30B.ret_typ;
_tmp30F=_tmp30B.args;_tmp310=_tmp30B.c_varargs;_tmp311=_tmp30B.cyc_varargs;
_tmp312=_tmp30B.rgn_po;_tmp313=_tmp30B.attributes;_LL263: {int done=0;{struct
_RegionHandle _tmp359=_new_region("rgn");struct _RegionHandle*rgn=& _tmp359;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp30C != 0){if(_tmp302 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp35A="(second function type has too few type variables)";
_tag_dynforward(_tmp35A,sizeof(char),_get_zero_arr_size(_tmp35A,50));});(int)
_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp35B=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp30C->hd);void*_tmp35C=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp302->hd);
if(_tmp35B != _tmp35C){Cyc_Tcutil_failure_reason=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp361;_tmp361.tag=0;_tmp361.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp35C));{struct Cyc_String_pa_struct
_tmp360;_tmp360.tag=0;_tmp360.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(_tmp35B));{struct Cyc_String_pa_struct _tmp35F;_tmp35F.tag=
0;_tmp35F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp30C->hd));{void*_tmp35D[3]={& _tmp35F,& _tmp360,& _tmp361};
Cyc_aprintf(({const char*_tmp35E="(type var %s has different kinds %s and %s)";
_tag_dynforward(_tmp35E,sizeof(char),_get_zero_arr_size(_tmp35E,44));}),
_tag_dynforward(_tmp35D,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp362=_region_malloc(rgn,sizeof(*_tmp362));_tmp362->hd=({
struct _tuple8*_tmp363=_region_malloc(rgn,sizeof(*_tmp363));_tmp363->f1=(struct
Cyc_Absyn_Tvar*)_tmp302->hd;_tmp363->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364[0]=({struct Cyc_Absyn_VarType_struct
_tmp365;_tmp365.tag=1;_tmp365.f1=(struct Cyc_Absyn_Tvar*)_tmp30C->hd;_tmp365;});
_tmp364;});_tmp363;});_tmp362->tl=inst;_tmp362;});_tmp30C=_tmp30C->tl;_tmp302=
_tmp302->tl;}}if(_tmp302 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp366="(second function type has too many type variables)";
_tag_dynforward(_tmp366,sizeof(char),_get_zero_arr_size(_tmp366,51));});
_npop_handler(0);goto _LL241;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367[0]=({struct Cyc_Absyn_FnType_struct
_tmp368;_tmp368.tag=8;_tmp368.f1=({struct Cyc_Absyn_FnInfo _tmp369;_tmp369.tvars=0;
_tmp369.effect=_tmp30D;_tmp369.ret_typ=(void*)_tmp30E;_tmp369.args=_tmp30F;
_tmp369.c_varargs=_tmp310;_tmp369.cyc_varargs=_tmp311;_tmp369.rgn_po=_tmp312;
_tmp369.attributes=_tmp313;_tmp369;});_tmp368;});_tmp367;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp36A=_cycalloc(sizeof(*
_tmp36A));_tmp36A[0]=({struct Cyc_Absyn_FnType_struct _tmp36B;_tmp36B.tag=8;
_tmp36B.f1=({struct Cyc_Absyn_FnInfo _tmp36C;_tmp36C.tvars=0;_tmp36C.effect=
_tmp303;_tmp36C.ret_typ=(void*)_tmp304;_tmp36C.args=_tmp305;_tmp36C.c_varargs=
_tmp306;_tmp36C.cyc_varargs=_tmp307;_tmp36C.rgn_po=_tmp308;_tmp36C.attributes=
_tmp309;_tmp36C;});_tmp36B;});_tmp36A;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp30E,_tmp304);for(0;_tmp30F != 0  && _tmp305 != 0;(
_tmp30F=_tmp30F->tl,_tmp305=_tmp305->tl)){struct Cyc_Absyn_Tqual _tmp36E;void*
_tmp36F;struct _tuple3 _tmp36D=*((struct _tuple3*)_tmp30F->hd);_tmp36E=_tmp36D.f2;
_tmp36F=_tmp36D.f3;{struct Cyc_Absyn_Tqual _tmp371;void*_tmp372;struct _tuple3
_tmp370=*((struct _tuple3*)_tmp305->hd);_tmp371=_tmp370.f2;_tmp372=_tmp370.f3;Cyc_Tcutil_unify_it(
_tmp36F,_tmp372);Cyc_Tcutil_unify_tqual(_tmp36E,_tmp36F,_tmp371,_tmp372);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp30F != 0  || _tmp305 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp373="(function types have different number of arguments)";
_tag_dynforward(_tmp373,sizeof(char),_get_zero_arr_size(_tmp373,52));});goto
_LL241;}if(_tmp310 != _tmp306){Cyc_Tcutil_failure_reason=({const char*_tmp374="(only one function type takes C varargs)";
_tag_dynforward(_tmp374,sizeof(char),_get_zero_arr_size(_tmp374,41));});goto
_LL241;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp376=({struct _tuple11 _tmp375;
_tmp375.f1=_tmp311;_tmp375.f2=_tmp307;_tmp375;});struct Cyc_Absyn_VarargInfo*
_tmp377;struct Cyc_Absyn_VarargInfo*_tmp378;struct Cyc_Absyn_VarargInfo*_tmp379;
struct Cyc_Absyn_VarargInfo*_tmp37A;struct Cyc_Absyn_VarargInfo*_tmp37B;struct Cyc_Absyn_VarargInfo
_tmp37C;struct Cyc_Core_Opt*_tmp37D;struct Cyc_Absyn_Tqual _tmp37E;void*_tmp37F;int
_tmp380;struct Cyc_Absyn_VarargInfo*_tmp381;struct Cyc_Absyn_VarargInfo _tmp382;
struct Cyc_Core_Opt*_tmp383;struct Cyc_Absyn_Tqual _tmp384;void*_tmp385;int _tmp386;
_LL286: _tmp377=_tmp376.f1;if(_tmp377 != 0)goto _LL288;_tmp378=_tmp376.f2;if(
_tmp378 != 0)goto _LL288;_LL287: goto _LL285;_LL288: _tmp379=_tmp376.f1;if(_tmp379 != 
0)goto _LL28A;_LL289: goto _LL28B;_LL28A: _tmp37A=_tmp376.f2;if(_tmp37A != 0)goto
_LL28C;_LL28B: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp387="(only one function type takes varargs)";
_tag_dynforward(_tmp387,sizeof(char),_get_zero_arr_size(_tmp387,39));});goto
_LL285;_LL28C: _tmp37B=_tmp376.f1;if(_tmp37B == 0)goto _LL285;_tmp37C=*_tmp37B;
_tmp37D=_tmp37C.name;_tmp37E=_tmp37C.tq;_tmp37F=(void*)_tmp37C.type;_tmp380=
_tmp37C.inject;_tmp381=_tmp376.f2;if(_tmp381 == 0)goto _LL285;_tmp382=*_tmp381;
_tmp383=_tmp382.name;_tmp384=_tmp382.tq;_tmp385=(void*)_tmp382.type;_tmp386=
_tmp382.inject;_LL28D: Cyc_Tcutil_unify_it(_tmp37F,_tmp385);Cyc_Tcutil_unify_tqual(
_tmp37E,_tmp37F,_tmp384,_tmp385);if(_tmp380 != _tmp386){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({
const char*_tmp388="(only one function type injects varargs)";_tag_dynforward(
_tmp388,sizeof(char),_get_zero_arr_size(_tmp388,41));});}goto _LL285;_LL285:;}if(
bad_cyc_vararg)goto _LL241;{int bad_effect=0;{struct _tuple12 _tmp38A=({struct
_tuple12 _tmp389;_tmp389.f1=_tmp30D;_tmp389.f2=_tmp303;_tmp389;});struct Cyc_Core_Opt*
_tmp38B;struct Cyc_Core_Opt*_tmp38C;struct Cyc_Core_Opt*_tmp38D;struct Cyc_Core_Opt*
_tmp38E;_LL28F: _tmp38B=_tmp38A.f1;if(_tmp38B != 0)goto _LL291;_tmp38C=_tmp38A.f2;
if(_tmp38C != 0)goto _LL291;_LL290: goto _LL28E;_LL291: _tmp38D=_tmp38A.f1;if(_tmp38D
!= 0)goto _LL293;_LL292: goto _LL294;_LL293: _tmp38E=_tmp38A.f2;if(_tmp38E != 0)goto
_LL295;_LL294: bad_effect=1;goto _LL28E;_LL295:;_LL296: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp30D))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp303))->v);goto _LL28E;_LL28E:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp38F="(function type effects do not unify)";
_tag_dynforward(_tmp38F,sizeof(char),_get_zero_arr_size(_tmp38F,37));});goto
_LL241;}if(!Cyc_Tcutil_same_atts(_tmp309,_tmp313)){Cyc_Tcutil_failure_reason=({
const char*_tmp390="(function types have different attributes)";_tag_dynforward(
_tmp390,sizeof(char),_get_zero_arr_size(_tmp390,43));});goto _LL241;}if(!Cyc_Tcutil_same_rgn_po(
_tmp308,_tmp312)){Cyc_Tcutil_failure_reason=({const char*_tmp391="(function types have different region lifetime orderings)";
_tag_dynforward(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,58));});goto
_LL241;}return;}}}_LL264: _tmp314=_tmp298.f1;if(_tmp314 <= (void*)4)goto _LL266;if(*((
int*)_tmp314)!= 9)goto _LL266;_tmp315=((struct Cyc_Absyn_TupleType_struct*)_tmp314)->f1;
_tmp316=_tmp298.f2;if(_tmp316 <= (void*)4)goto _LL266;if(*((int*)_tmp316)!= 9)goto
_LL266;_tmp317=((struct Cyc_Absyn_TupleType_struct*)_tmp316)->f1;_LL265: for(0;
_tmp317 != 0  && _tmp315 != 0;(_tmp317=_tmp317->tl,_tmp315=_tmp315->tl)){struct Cyc_Absyn_Tqual
_tmp393;void*_tmp394;struct _tuple5 _tmp392=*((struct _tuple5*)_tmp317->hd);_tmp393=
_tmp392.f1;_tmp394=_tmp392.f2;{struct Cyc_Absyn_Tqual _tmp396;void*_tmp397;struct
_tuple5 _tmp395=*((struct _tuple5*)_tmp315->hd);_tmp396=_tmp395.f1;_tmp397=_tmp395.f2;
Cyc_Tcutil_unify_it(_tmp394,_tmp397);Cyc_Tcutil_unify_tqual(_tmp393,_tmp394,
_tmp396,_tmp397);}}if(_tmp317 == 0  && _tmp315 == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp398="(tuple types have different numbers of components)";
_tag_dynforward(_tmp398,sizeof(char),_get_zero_arr_size(_tmp398,51));});goto
_LL241;_LL266: _tmp318=_tmp298.f1;if(_tmp318 <= (void*)4)goto _LL268;if(*((int*)
_tmp318)!= 11)goto _LL268;_tmp319=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp318)->f1;_tmp31A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp318)->f2;_tmp31B=
_tmp298.f2;if(_tmp31B <= (void*)4)goto _LL268;if(*((int*)_tmp31B)!= 11)goto _LL268;
_tmp31C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp31B)->f1;_tmp31D=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp31B)->f2;_LL267: if(_tmp31C != _tmp319){
Cyc_Tcutil_failure_reason=({const char*_tmp399="(struct and union type)";
_tag_dynforward(_tmp399,sizeof(char),_get_zero_arr_size(_tmp399,24));});goto
_LL241;}for(0;_tmp31D != 0  && _tmp31A != 0;(_tmp31D=_tmp31D->tl,_tmp31A=_tmp31A->tl)){
struct Cyc_Absyn_Aggrfield*_tmp39A=(struct Cyc_Absyn_Aggrfield*)_tmp31D->hd;struct
Cyc_Absyn_Aggrfield*_tmp39B=(struct Cyc_Absyn_Aggrfield*)_tmp31A->hd;if(Cyc_strptrcmp(
_tmp39A->name,_tmp39B->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp39C="(different member names)";
_tag_dynforward(_tmp39C,sizeof(char),_get_zero_arr_size(_tmp39C,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp39A->type,(void*)
_tmp39B->type);Cyc_Tcutil_unify_tqual(_tmp39A->tq,(void*)_tmp39A->type,_tmp39B->tq,(
void*)_tmp39B->type);if(!Cyc_Tcutil_same_atts(_tmp39A->attributes,_tmp39B->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp39D="(different attributes on member)";_tag_dynforward(_tmp39D,
sizeof(char),_get_zero_arr_size(_tmp39D,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp39A->width != 0  && _tmp39B->width == 0  || _tmp39B->width != 0  && _tmp39A->width
== 0) || (_tmp39A->width != 0  && _tmp39B->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp39A->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp39B->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp39E="(different bitfield widths on member)";
_tag_dynforward(_tmp39E,sizeof(char),_get_zero_arr_size(_tmp39E,38));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp31D == 0  && _tmp31A == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp39F="(different number of members)";
_tag_dynforward(_tmp39F,sizeof(char),_get_zero_arr_size(_tmp39F,30));});goto
_LL241;_LL268: _tmp31E=_tmp298.f1;if((int)_tmp31E != 2)goto _LL26A;_tmp31F=_tmp298.f2;
if((int)_tmp31F != 2)goto _LL26A;_LL269: return;_LL26A: _tmp320=_tmp298.f1;if((int)
_tmp320 != 3)goto _LL26C;_tmp321=_tmp298.f2;if((int)_tmp321 != 3)goto _LL26C;_LL26B:
return;_LL26C: _tmp322=_tmp298.f1;if(_tmp322 <= (void*)4)goto _LL26E;if(*((int*)
_tmp322)!= 15)goto _LL26E;_tmp323=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp322)->f1;_tmp324=_tmp298.f2;if(_tmp324 <= (void*)4)goto _LL26E;if(*((int*)
_tmp324)!= 15)goto _LL26E;_tmp325=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp324)->f1;_LL26D: Cyc_Tcutil_unify_it(_tmp323,_tmp325);return;_LL26E: _tmp326=
_tmp298.f1;if(_tmp326 <= (void*)4)goto _LL270;if(*((int*)_tmp326)!= 16)goto _LL270;
_tmp327=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp326)->f1;_tmp328=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp326)->f2;_tmp329=_tmp298.f2;if(_tmp329 <= (
void*)4)goto _LL270;if(*((int*)_tmp329)!= 16)goto _LL270;_tmp32A=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp329)->f1;_tmp32B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp329)->f2;_LL26F: Cyc_Tcutil_unify_it(_tmp327,_tmp32A);Cyc_Tcutil_unify_it(
_tmp328,_tmp32B);return;_LL270: _tmp32C=_tmp298.f1;if(_tmp32C <= (void*)4)goto
_LL272;if(*((int*)_tmp32C)!= 21)goto _LL272;_LL271: goto _LL273;_LL272: _tmp32D=
_tmp298.f2;if(_tmp32D <= (void*)4)goto _LL274;if(*((int*)_tmp32D)!= 21)goto _LL274;
_LL273: goto _LL275;_LL274: _tmp32E=_tmp298.f1;if(_tmp32E <= (void*)4)goto _LL276;if(*((
int*)_tmp32E)!= 20)goto _LL276;_LL275: goto _LL277;_LL276: _tmp32F=_tmp298.f1;if(
_tmp32F <= (void*)4)goto _LL278;if(*((int*)_tmp32F)!= 22)goto _LL278;_LL277: goto
_LL279;_LL278: _tmp330=_tmp298.f2;if(_tmp330 <= (void*)4)goto _LL27A;if(*((int*)
_tmp330)!= 22)goto _LL27A;_LL279: goto _LL27B;_LL27A: _tmp331=_tmp298.f2;if(_tmp331
<= (void*)4)goto _LL27C;if(*((int*)_tmp331)!= 20)goto _LL27C;_LL27B: if(Cyc_Tcutil_unify_effect(
t1,t2))return;Cyc_Tcutil_failure_reason=({const char*_tmp3A0="(effects don't unify)";
_tag_dynforward(_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,22));});goto
_LL241;_LL27C:;_LL27D: goto _LL241;_LL241:;}(int)_throw((void*)Cyc_Tcutil_Unify);}
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return
0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)
_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){int _tmp3A1=(tq1.real_const + (tq1.q_volatile << 1))+ (
tq1.q_restrict << 2);int _tmp3A2=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict
<< 2);return Cyc_Core_intcmp(_tmp3A1,_tmp3A2);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint_union
_tmp3A3=x->v;void*_tmp3A4;_LL298: if((_tmp3A3.No_constr).tag != 2)goto _LL29A;
_LL299: return - 1;_LL29A: if((_tmp3A3.Eq_constr).tag != 0)goto _LL29C;_tmp3A4=(
_tmp3A3.Eq_constr).f1;_LL29B: {union Cyc_Absyn_Constraint_union _tmp3A5=y->v;void*
_tmp3A6;_LL29F: if((_tmp3A5.No_constr).tag != 2)goto _LL2A1;_LL2A0: return 1;_LL2A1:
if((_tmp3A5.Eq_constr).tag != 0)goto _LL2A3;_tmp3A6=(_tmp3A5.Eq_constr).f1;_LL2A2:
return cmp(_tmp3A4,_tmp3A6);_LL2A3: if((_tmp3A5.Forward_constr).tag != 1)goto _LL29E;
_LL2A4:({void*_tmp3A7[0]={};Cyc_Tcutil_impos(({const char*_tmp3A8="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp3A8,sizeof(char),_get_zero_arr_size(_tmp3A8,40));}),
_tag_dynforward(_tmp3A7,sizeof(void*),0));});_LL29E:;}_LL29C: if((_tmp3A3.Forward_constr).tag
!= 1)goto _LL297;_LL29D:({void*_tmp3A9[0]={};Cyc_Tcutil_impos(({const char*_tmp3AA="unify_conref: forward after compress";
_tag_dynforward(_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,37));}),
_tag_dynforward(_tmp3A9,sizeof(void*),0));});_LL297:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple5*tqt1,struct _tuple5*tqt2){struct _tuple5 _tmp3AC;struct Cyc_Absyn_Tqual
_tmp3AD;void*_tmp3AE;struct _tuple5*_tmp3AB=tqt1;_tmp3AC=*_tmp3AB;_tmp3AD=_tmp3AC.f1;
_tmp3AE=_tmp3AC.f2;{struct _tuple5 _tmp3B0;struct Cyc_Absyn_Tqual _tmp3B1;void*
_tmp3B2;struct _tuple5*_tmp3AF=tqt2;_tmp3B0=*_tmp3AF;_tmp3B1=_tmp3B0.f1;_tmp3B2=
_tmp3B0.f2;{int _tmp3B3=Cyc_Tcutil_tqual_cmp(_tmp3AD,_tmp3B1);if(_tmp3B3 != 0)
return _tmp3B3;return Cyc_Tcutil_typecmp(_tmp3AE,_tmp3B2);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp3B4=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp3B4 != 0)return _tmp3B4;{int _tmp3B5=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp3B5 != 0)return _tmp3B5;{int _tmp3B6=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp3B6 != 0)return _tmp3B6;{int _tmp3B7=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp3B7 != 0)return
_tmp3B7;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3B8=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3B8 != 0)return _tmp3B8;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3B9=t;_LL2A6: if((int)_tmp3B9 != 0)goto _LL2A8;_LL2A7: return 0;
_LL2A8: if(_tmp3B9 <= (void*)4)goto _LL2B4;if(*((int*)_tmp3B9)!= 0)goto _LL2AA;
_LL2A9: return 1;_LL2AA: if(*((int*)_tmp3B9)!= 1)goto _LL2AC;_LL2AB: return 2;_LL2AC:
if(*((int*)_tmp3B9)!= 2)goto _LL2AE;_LL2AD: return 3;_LL2AE: if(*((int*)_tmp3B9)!= 3)
goto _LL2B0;_LL2AF: return 4;_LL2B0: if(*((int*)_tmp3B9)!= 4)goto _LL2B2;_LL2B1:
return 5;_LL2B2: if(*((int*)_tmp3B9)!= 5)goto _LL2B4;_LL2B3: return 6;_LL2B4: if((int)
_tmp3B9 != 1)goto _LL2B6;_LL2B5: return 7;_LL2B6: if(_tmp3B9 <= (void*)4)goto _LL2CA;
if(*((int*)_tmp3B9)!= 6)goto _LL2B8;_LL2B7: return 8;_LL2B8: if(*((int*)_tmp3B9)!= 7)
goto _LL2BA;_LL2B9: return 9;_LL2BA: if(*((int*)_tmp3B9)!= 8)goto _LL2BC;_LL2BB:
return 10;_LL2BC: if(*((int*)_tmp3B9)!= 9)goto _LL2BE;_LL2BD: return 11;_LL2BE: if(*((
int*)_tmp3B9)!= 10)goto _LL2C0;_LL2BF: return 12;_LL2C0: if(*((int*)_tmp3B9)!= 11)
goto _LL2C2;_LL2C1: return 14;_LL2C2: if(*((int*)_tmp3B9)!= 12)goto _LL2C4;_LL2C3:
return 16;_LL2C4: if(*((int*)_tmp3B9)!= 13)goto _LL2C6;_LL2C5: return 17;_LL2C6: if(*((
int*)_tmp3B9)!= 15)goto _LL2C8;_LL2C7: return 18;_LL2C8: if(*((int*)_tmp3B9)!= 17)
goto _LL2CA;_LL2C9: return 19;_LL2CA: if((int)_tmp3B9 != 3)goto _LL2CC;_LL2CB: return 20;
_LL2CC: if((int)_tmp3B9 != 2)goto _LL2CE;_LL2CD: return 21;_LL2CE: if(_tmp3B9 <= (void*)
4)goto _LL2D0;if(*((int*)_tmp3B9)!= 20)goto _LL2D0;_LL2CF: return 22;_LL2D0: if(
_tmp3B9 <= (void*)4)goto _LL2D2;if(*((int*)_tmp3B9)!= 21)goto _LL2D2;_LL2D1: return
23;_LL2D2: if(_tmp3B9 <= (void*)4)goto _LL2D4;if(*((int*)_tmp3B9)!= 22)goto _LL2D4;
_LL2D3: return 24;_LL2D4: if(_tmp3B9 <= (void*)4)goto _LL2D6;if(*((int*)_tmp3B9)!= 14)
goto _LL2D6;_LL2D5: return 25;_LL2D6: if(_tmp3B9 <= (void*)4)goto _LL2D8;if(*((int*)
_tmp3B9)!= 19)goto _LL2D8;_LL2D7: return 26;_LL2D8: if(_tmp3B9 <= (void*)4)goto _LL2DA;
if(*((int*)_tmp3B9)!= 18)goto _LL2DA;_LL2D9: return 27;_LL2DA: if(_tmp3B9 <= (void*)4)
goto _LL2A5;if(*((int*)_tmp3B9)!= 16)goto _LL2A5;_LL2DB: return 28;_LL2A5:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3BA=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3BA != 0)return _tmp3BA;{struct _tuple0 _tmp3BC=({struct _tuple0 _tmp3BB;
_tmp3BB.f1=t2;_tmp3BB.f2=t1;_tmp3BB;});void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;
struct Cyc_Absyn_Tvar*_tmp3C0;void*_tmp3C1;struct Cyc_Absyn_Tvar*_tmp3C2;void*
_tmp3C3;struct Cyc_Absyn_AggrInfo _tmp3C4;union Cyc_Absyn_AggrInfoU_union _tmp3C5;
struct Cyc_List_List*_tmp3C6;void*_tmp3C7;struct Cyc_Absyn_AggrInfo _tmp3C8;union
Cyc_Absyn_AggrInfoU_union _tmp3C9;struct Cyc_List_List*_tmp3CA;void*_tmp3CB;struct
_tuple2*_tmp3CC;void*_tmp3CD;struct _tuple2*_tmp3CE;void*_tmp3CF;struct Cyc_List_List*
_tmp3D0;void*_tmp3D1;struct Cyc_List_List*_tmp3D2;void*_tmp3D3;struct Cyc_Absyn_TunionInfo
_tmp3D4;union Cyc_Absyn_TunionInfoU_union _tmp3D5;struct Cyc_Absyn_Tuniondecl**
_tmp3D6;struct Cyc_Absyn_Tuniondecl*_tmp3D7;struct Cyc_List_List*_tmp3D8;struct Cyc_Core_Opt*
_tmp3D9;void*_tmp3DA;struct Cyc_Absyn_TunionInfo _tmp3DB;union Cyc_Absyn_TunionInfoU_union
_tmp3DC;struct Cyc_Absyn_Tuniondecl**_tmp3DD;struct Cyc_Absyn_Tuniondecl*_tmp3DE;
struct Cyc_List_List*_tmp3DF;struct Cyc_Core_Opt*_tmp3E0;void*_tmp3E1;struct Cyc_Absyn_TunionFieldInfo
_tmp3E2;union Cyc_Absyn_TunionFieldInfoU_union _tmp3E3;struct Cyc_Absyn_Tuniondecl*
_tmp3E4;struct Cyc_Absyn_Tunionfield*_tmp3E5;struct Cyc_List_List*_tmp3E6;void*
_tmp3E7;struct Cyc_Absyn_TunionFieldInfo _tmp3E8;union Cyc_Absyn_TunionFieldInfoU_union
_tmp3E9;struct Cyc_Absyn_Tuniondecl*_tmp3EA;struct Cyc_Absyn_Tunionfield*_tmp3EB;
struct Cyc_List_List*_tmp3EC;void*_tmp3ED;struct Cyc_Absyn_PtrInfo _tmp3EE;void*
_tmp3EF;struct Cyc_Absyn_Tqual _tmp3F0;struct Cyc_Absyn_PtrAtts _tmp3F1;void*_tmp3F2;
struct Cyc_Absyn_Conref*_tmp3F3;struct Cyc_Absyn_Conref*_tmp3F4;struct Cyc_Absyn_Conref*
_tmp3F5;void*_tmp3F6;struct Cyc_Absyn_PtrInfo _tmp3F7;void*_tmp3F8;struct Cyc_Absyn_Tqual
_tmp3F9;struct Cyc_Absyn_PtrAtts _tmp3FA;void*_tmp3FB;struct Cyc_Absyn_Conref*
_tmp3FC;struct Cyc_Absyn_Conref*_tmp3FD;struct Cyc_Absyn_Conref*_tmp3FE;void*
_tmp3FF;void*_tmp400;void*_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;void*
_tmp405;void*_tmp406;void*_tmp407;int _tmp408;void*_tmp409;int _tmp40A;void*
_tmp40B;struct Cyc_Absyn_ArrayInfo _tmp40C;void*_tmp40D;struct Cyc_Absyn_Tqual
_tmp40E;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Conref*_tmp410;void*_tmp411;
struct Cyc_Absyn_ArrayInfo _tmp412;void*_tmp413;struct Cyc_Absyn_Tqual _tmp414;
struct Cyc_Absyn_Exp*_tmp415;struct Cyc_Absyn_Conref*_tmp416;void*_tmp417;struct
Cyc_Absyn_FnInfo _tmp418;struct Cyc_List_List*_tmp419;struct Cyc_Core_Opt*_tmp41A;
void*_tmp41B;struct Cyc_List_List*_tmp41C;int _tmp41D;struct Cyc_Absyn_VarargInfo*
_tmp41E;struct Cyc_List_List*_tmp41F;struct Cyc_List_List*_tmp420;void*_tmp421;
struct Cyc_Absyn_FnInfo _tmp422;struct Cyc_List_List*_tmp423;struct Cyc_Core_Opt*
_tmp424;void*_tmp425;struct Cyc_List_List*_tmp426;int _tmp427;struct Cyc_Absyn_VarargInfo*
_tmp428;struct Cyc_List_List*_tmp429;struct Cyc_List_List*_tmp42A;void*_tmp42B;
struct Cyc_List_List*_tmp42C;void*_tmp42D;struct Cyc_List_List*_tmp42E;void*
_tmp42F;void*_tmp430;struct Cyc_List_List*_tmp431;void*_tmp432;void*_tmp433;
struct Cyc_List_List*_tmp434;void*_tmp435;void*_tmp436;void*_tmp437;void*_tmp438;
void*_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;void*_tmp43D;void*_tmp43E;
void*_tmp43F;void*_tmp440;void*_tmp441;void*_tmp442;void*_tmp443;void*_tmp444;
void*_tmp445;void*_tmp446;void*_tmp447;int _tmp448;void*_tmp449;int _tmp44A;void*
_tmp44B;void*_tmp44C;void*_tmp44D;void*_tmp44E;void*_tmp44F;void*_tmp450;_LL2DD:
_tmp3BD=_tmp3BC.f1;if(_tmp3BD <= (void*)4)goto _LL2DF;if(*((int*)_tmp3BD)!= 0)goto
_LL2DF;_tmp3BE=_tmp3BC.f2;if(_tmp3BE <= (void*)4)goto _LL2DF;if(*((int*)_tmp3BE)!= 
0)goto _LL2DF;_LL2DE:({void*_tmp451[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp452="typecmp: can only compare closed types";
_tag_dynforward(_tmp452,sizeof(char),_get_zero_arr_size(_tmp452,39));}),
_tag_dynforward(_tmp451,sizeof(void*),0));});_LL2DF: _tmp3BF=_tmp3BC.f1;if(
_tmp3BF <= (void*)4)goto _LL2E1;if(*((int*)_tmp3BF)!= 1)goto _LL2E1;_tmp3C0=((
struct Cyc_Absyn_VarType_struct*)_tmp3BF)->f1;_tmp3C1=_tmp3BC.f2;if(_tmp3C1 <= (
void*)4)goto _LL2E1;if(*((int*)_tmp3C1)!= 1)goto _LL2E1;_tmp3C2=((struct Cyc_Absyn_VarType_struct*)
_tmp3C1)->f1;_LL2E0: return Cyc_Core_intcmp(_tmp3C2->identity,_tmp3C0->identity);
_LL2E1: _tmp3C3=_tmp3BC.f1;if(_tmp3C3 <= (void*)4)goto _LL2E3;if(*((int*)_tmp3C3)!= 
10)goto _LL2E3;_tmp3C4=((struct Cyc_Absyn_AggrType_struct*)_tmp3C3)->f1;_tmp3C5=
_tmp3C4.aggr_info;_tmp3C6=_tmp3C4.targs;_tmp3C7=_tmp3BC.f2;if(_tmp3C7 <= (void*)4)
goto _LL2E3;if(*((int*)_tmp3C7)!= 10)goto _LL2E3;_tmp3C8=((struct Cyc_Absyn_AggrType_struct*)
_tmp3C7)->f1;_tmp3C9=_tmp3C8.aggr_info;_tmp3CA=_tmp3C8.targs;_LL2E2: {struct
_tuple2*_tmp454;struct _tuple6 _tmp453=Cyc_Absyn_aggr_kinded_name(_tmp3C5);_tmp454=
_tmp453.f2;{struct _tuple2*_tmp456;struct _tuple6 _tmp455=Cyc_Absyn_aggr_kinded_name(
_tmp3C9);_tmp456=_tmp455.f2;{int _tmp457=Cyc_Absyn_qvar_cmp(_tmp454,_tmp456);if(
_tmp457 != 0)return _tmp457;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3C6,_tmp3CA);}}}}_LL2E3: _tmp3CB=_tmp3BC.f1;if(_tmp3CB <= (void*)4)goto _LL2E5;
if(*((int*)_tmp3CB)!= 12)goto _LL2E5;_tmp3CC=((struct Cyc_Absyn_EnumType_struct*)
_tmp3CB)->f1;_tmp3CD=_tmp3BC.f2;if(_tmp3CD <= (void*)4)goto _LL2E5;if(*((int*)
_tmp3CD)!= 12)goto _LL2E5;_tmp3CE=((struct Cyc_Absyn_EnumType_struct*)_tmp3CD)->f1;
_LL2E4: return Cyc_Absyn_qvar_cmp(_tmp3CC,_tmp3CE);_LL2E5: _tmp3CF=_tmp3BC.f1;if(
_tmp3CF <= (void*)4)goto _LL2E7;if(*((int*)_tmp3CF)!= 13)goto _LL2E7;_tmp3D0=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3CF)->f1;_tmp3D1=_tmp3BC.f2;if(_tmp3D1
<= (void*)4)goto _LL2E7;if(*((int*)_tmp3D1)!= 13)goto _LL2E7;_tmp3D2=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3D1)->f1;_LL2E6: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3D0,_tmp3D2);_LL2E7: _tmp3D3=_tmp3BC.f1;if(_tmp3D3 <= (
void*)4)goto _LL2E9;if(*((int*)_tmp3D3)!= 2)goto _LL2E9;_tmp3D4=((struct Cyc_Absyn_TunionType_struct*)
_tmp3D3)->f1;_tmp3D5=_tmp3D4.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp3D3)->f1).tunion_info).KnownTunion).tag != 1)goto _LL2E9;_tmp3D6=(_tmp3D5.KnownTunion).f1;
_tmp3D7=*_tmp3D6;_tmp3D8=_tmp3D4.targs;_tmp3D9=_tmp3D4.rgn;_tmp3DA=_tmp3BC.f2;
if(_tmp3DA <= (void*)4)goto _LL2E9;if(*((int*)_tmp3DA)!= 2)goto _LL2E9;_tmp3DB=((
struct Cyc_Absyn_TunionType_struct*)_tmp3DA)->f1;_tmp3DC=_tmp3DB.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3DA)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL2E9;_tmp3DD=(_tmp3DC.KnownTunion).f1;_tmp3DE=*_tmp3DD;_tmp3DF=_tmp3DB.targs;
_tmp3E0=_tmp3DB.rgn;_LL2E8: if(_tmp3DE == _tmp3D7)return 0;{int _tmp458=Cyc_Absyn_qvar_cmp(
_tmp3DE->name,_tmp3D7->name);if(_tmp458 != 0)return _tmp458;if((unsigned int)
_tmp3E0  && (unsigned int)_tmp3D9){int _tmp459=Cyc_Tcutil_typecmp((void*)_tmp3E0->v,(
void*)_tmp3D9->v);if(_tmp459 != 0)return _tmp459;}else{if((unsigned int)_tmp3E0)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3DF,
_tmp3D8);}_LL2E9: _tmp3E1=_tmp3BC.f1;if(_tmp3E1 <= (void*)4)goto _LL2EB;if(*((int*)
_tmp3E1)!= 3)goto _LL2EB;_tmp3E2=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3E1)->f1;_tmp3E3=_tmp3E2.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3E1)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2EB;_tmp3E4=(_tmp3E3.KnownTunionfield).f1;
_tmp3E5=(_tmp3E3.KnownTunionfield).f2;_tmp3E6=_tmp3E2.targs;_tmp3E7=_tmp3BC.f2;
if(_tmp3E7 <= (void*)4)goto _LL2EB;if(*((int*)_tmp3E7)!= 3)goto _LL2EB;_tmp3E8=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3E7)->f1;_tmp3E9=_tmp3E8.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3E7)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL2EB;_tmp3EA=(_tmp3E9.KnownTunionfield).f1;_tmp3EB=(_tmp3E9.KnownTunionfield).f2;
_tmp3EC=_tmp3E8.targs;_LL2EA: if(_tmp3EA == _tmp3E4)return 0;{int _tmp45A=Cyc_Absyn_qvar_cmp(
_tmp3E4->name,_tmp3EA->name);if(_tmp45A != 0)return _tmp45A;{int _tmp45B=Cyc_Absyn_qvar_cmp(
_tmp3E5->name,_tmp3EB->name);if(_tmp45B != 0)return _tmp45B;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3EC,_tmp3E6);}}_LL2EB: _tmp3ED=_tmp3BC.f1;if(_tmp3ED <= (
void*)4)goto _LL2ED;if(*((int*)_tmp3ED)!= 4)goto _LL2ED;_tmp3EE=((struct Cyc_Absyn_PointerType_struct*)
_tmp3ED)->f1;_tmp3EF=(void*)_tmp3EE.elt_typ;_tmp3F0=_tmp3EE.elt_tq;_tmp3F1=
_tmp3EE.ptr_atts;_tmp3F2=(void*)_tmp3F1.rgn;_tmp3F3=_tmp3F1.nullable;_tmp3F4=
_tmp3F1.bounds;_tmp3F5=_tmp3F1.zero_term;_tmp3F6=_tmp3BC.f2;if(_tmp3F6 <= (void*)
4)goto _LL2ED;if(*((int*)_tmp3F6)!= 4)goto _LL2ED;_tmp3F7=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F6)->f1;_tmp3F8=(void*)_tmp3F7.elt_typ;_tmp3F9=_tmp3F7.elt_tq;_tmp3FA=
_tmp3F7.ptr_atts;_tmp3FB=(void*)_tmp3FA.rgn;_tmp3FC=_tmp3FA.nullable;_tmp3FD=
_tmp3FA.bounds;_tmp3FE=_tmp3FA.zero_term;_LL2EC: {int _tmp45C=Cyc_Tcutil_typecmp(
_tmp3F8,_tmp3EF);if(_tmp45C != 0)return _tmp45C;{int _tmp45D=Cyc_Tcutil_typecmp(
_tmp3FB,_tmp3F2);if(_tmp45D != 0)return _tmp45D;{int _tmp45E=Cyc_Tcutil_tqual_cmp(
_tmp3F9,_tmp3F0);if(_tmp45E != 0)return _tmp45E;{int _tmp45F=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3FD,_tmp3F4);if(_tmp45F != 0)return _tmp45F;{int _tmp460=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3FE,_tmp3F5);if(_tmp460 != 0)return _tmp460;{union Cyc_Absyn_Constraint_union
_tmp461=(Cyc_Absyn_compress_conref(_tmp3FD))->v;void*_tmp462;void*_tmp463;_LL314:
if((_tmp461.Eq_constr).tag != 0)goto _LL316;_tmp462=(_tmp461.Eq_constr).f1;if((int)
_tmp462 != 0)goto _LL316;_LL315: return 0;_LL316: if((_tmp461.Eq_constr).tag != 0)goto
_LL318;_tmp463=(_tmp461.Eq_constr).f1;if((int)_tmp463 != 1)goto _LL318;_LL317:
return 0;_LL318:;_LL319: goto _LL313;_LL313:;}return((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,
_tmp3FC,_tmp3F3);}}}}}_LL2ED: _tmp3FF=_tmp3BC.f1;if(_tmp3FF <= (void*)4)goto _LL2EF;
if(*((int*)_tmp3FF)!= 5)goto _LL2EF;_tmp400=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3FF)->f1;_tmp401=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3FF)->f2;
_tmp402=_tmp3BC.f2;if(_tmp402 <= (void*)4)goto _LL2EF;if(*((int*)_tmp402)!= 5)goto
_LL2EF;_tmp403=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp402)->f1;_tmp404=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp402)->f2;_LL2EE: if(_tmp403 != _tmp400)
return Cyc_Core_intcmp((int)((unsigned int)_tmp403),(int)((unsigned int)_tmp400));
if(_tmp404 != _tmp401)return Cyc_Core_intcmp((int)((unsigned int)_tmp404),(int)((
unsigned int)_tmp401));return 0;_LL2EF: _tmp405=_tmp3BC.f1;if((int)_tmp405 != 1)
goto _LL2F1;_tmp406=_tmp3BC.f2;if((int)_tmp406 != 1)goto _LL2F1;_LL2F0: return 0;
_LL2F1: _tmp407=_tmp3BC.f1;if(_tmp407 <= (void*)4)goto _LL2F3;if(*((int*)_tmp407)!= 
6)goto _LL2F3;_tmp408=((struct Cyc_Absyn_DoubleType_struct*)_tmp407)->f1;_tmp409=
_tmp3BC.f2;if(_tmp409 <= (void*)4)goto _LL2F3;if(*((int*)_tmp409)!= 6)goto _LL2F3;
_tmp40A=((struct Cyc_Absyn_DoubleType_struct*)_tmp409)->f1;_LL2F2: if(_tmp408 == 
_tmp40A)return 0;else{if(_tmp408)return - 1;else{return 1;}}_LL2F3: _tmp40B=_tmp3BC.f1;
if(_tmp40B <= (void*)4)goto _LL2F5;if(*((int*)_tmp40B)!= 7)goto _LL2F5;_tmp40C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp40B)->f1;_tmp40D=(void*)_tmp40C.elt_type;
_tmp40E=_tmp40C.tq;_tmp40F=_tmp40C.num_elts;_tmp410=_tmp40C.zero_term;_tmp411=
_tmp3BC.f2;if(_tmp411 <= (void*)4)goto _LL2F5;if(*((int*)_tmp411)!= 7)goto _LL2F5;
_tmp412=((struct Cyc_Absyn_ArrayType_struct*)_tmp411)->f1;_tmp413=(void*)_tmp412.elt_type;
_tmp414=_tmp412.tq;_tmp415=_tmp412.num_elts;_tmp416=_tmp412.zero_term;_LL2F4: {
int _tmp464=Cyc_Tcutil_tqual_cmp(_tmp414,_tmp40E);if(_tmp464 != 0)return _tmp464;{
int _tmp465=Cyc_Tcutil_typecmp(_tmp413,_tmp40D);if(_tmp465 != 0)return _tmp465;{int
_tmp466=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp410,_tmp416);if(_tmp466 != 0)
return _tmp466;if(_tmp40F == _tmp415)return 0;if(_tmp40F == 0  || _tmp415 == 0)({void*
_tmp467[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp468="missing expression in array index";_tag_dynforward(_tmp468,
sizeof(char),_get_zero_arr_size(_tmp468,34));}),_tag_dynforward(_tmp467,sizeof(
void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp40F,_tmp415);}}}_LL2F5: _tmp417=_tmp3BC.f1;if(_tmp417 <= (void*)4)goto _LL2F7;
if(*((int*)_tmp417)!= 8)goto _LL2F7;_tmp418=((struct Cyc_Absyn_FnType_struct*)
_tmp417)->f1;_tmp419=_tmp418.tvars;_tmp41A=_tmp418.effect;_tmp41B=(void*)_tmp418.ret_typ;
_tmp41C=_tmp418.args;_tmp41D=_tmp418.c_varargs;_tmp41E=_tmp418.cyc_varargs;
_tmp41F=_tmp418.rgn_po;_tmp420=_tmp418.attributes;_tmp421=_tmp3BC.f2;if(_tmp421
<= (void*)4)goto _LL2F7;if(*((int*)_tmp421)!= 8)goto _LL2F7;_tmp422=((struct Cyc_Absyn_FnType_struct*)
_tmp421)->f1;_tmp423=_tmp422.tvars;_tmp424=_tmp422.effect;_tmp425=(void*)_tmp422.ret_typ;
_tmp426=_tmp422.args;_tmp427=_tmp422.c_varargs;_tmp428=_tmp422.cyc_varargs;
_tmp429=_tmp422.rgn_po;_tmp42A=_tmp422.attributes;_LL2F6:({void*_tmp469[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp46A="typecmp: function types not handled";_tag_dynforward(_tmp46A,
sizeof(char),_get_zero_arr_size(_tmp46A,36));}),_tag_dynforward(_tmp469,sizeof(
void*),0));});_LL2F7: _tmp42B=_tmp3BC.f1;if(_tmp42B <= (void*)4)goto _LL2F9;if(*((
int*)_tmp42B)!= 9)goto _LL2F9;_tmp42C=((struct Cyc_Absyn_TupleType_struct*)_tmp42B)->f1;
_tmp42D=_tmp3BC.f2;if(_tmp42D <= (void*)4)goto _LL2F9;if(*((int*)_tmp42D)!= 9)goto
_LL2F9;_tmp42E=((struct Cyc_Absyn_TupleType_struct*)_tmp42D)->f1;_LL2F8: return((
int(*)(int(*cmp)(struct _tuple5*,struct _tuple5*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp42E,_tmp42C);_LL2F9: _tmp42F=
_tmp3BC.f1;if(_tmp42F <= (void*)4)goto _LL2FB;if(*((int*)_tmp42F)!= 11)goto _LL2FB;
_tmp430=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp42F)->f1;_tmp431=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp42F)->f2;_tmp432=_tmp3BC.f2;if(_tmp432
<= (void*)4)goto _LL2FB;if(*((int*)_tmp432)!= 11)goto _LL2FB;_tmp433=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp432)->f1;_tmp434=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp432)->f2;_LL2FA: if(_tmp433 != _tmp430){if(_tmp433 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp434,_tmp431);_LL2FB: _tmp435=_tmp3BC.f1;if(_tmp435 <= (void*)4)goto _LL2FD;if(*((
int*)_tmp435)!= 15)goto _LL2FD;_tmp436=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp435)->f1;_tmp437=_tmp3BC.f2;if(_tmp437 <= (void*)4)goto _LL2FD;if(*((int*)
_tmp437)!= 15)goto _LL2FD;_tmp438=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp437)->f1;_LL2FC: return Cyc_Tcutil_typecmp(_tmp436,_tmp438);_LL2FD: _tmp439=
_tmp3BC.f1;if(_tmp439 <= (void*)4)goto _LL2FF;if(*((int*)_tmp439)!= 16)goto _LL2FF;
_tmp43A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp439)->f1;_tmp43B=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp439)->f2;_tmp43C=_tmp3BC.f2;if(_tmp43C <= (
void*)4)goto _LL2FF;if(*((int*)_tmp43C)!= 16)goto _LL2FF;_tmp43D=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp43C)->f1;_tmp43E=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp43C)->f2;_LL2FE: {int _tmp46B=Cyc_Tcutil_typecmp(_tmp43A,_tmp43D);if(_tmp46B
!= 0)return _tmp46B;else{return Cyc_Tcutil_typecmp(_tmp43B,_tmp43E);}}_LL2FF:
_tmp43F=_tmp3BC.f1;if(_tmp43F <= (void*)4)goto _LL301;if(*((int*)_tmp43F)!= 14)
goto _LL301;_tmp440=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp43F)->f1;
_tmp441=_tmp3BC.f2;if(_tmp441 <= (void*)4)goto _LL301;if(*((int*)_tmp441)!= 14)
goto _LL301;_tmp442=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp441)->f1;
_LL300: return Cyc_Tcutil_typecmp(_tmp440,_tmp442);_LL301: _tmp443=_tmp3BC.f1;if(
_tmp443 <= (void*)4)goto _LL303;if(*((int*)_tmp443)!= 18)goto _LL303;_tmp444=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp443)->f1;_tmp445=_tmp3BC.f2;if(_tmp445 <= (
void*)4)goto _LL303;if(*((int*)_tmp445)!= 18)goto _LL303;_tmp446=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp445)->f1;_LL302: return Cyc_Tcutil_typecmp(_tmp444,
_tmp446);_LL303: _tmp447=_tmp3BC.f1;if(_tmp447 <= (void*)4)goto _LL305;if(*((int*)
_tmp447)!= 19)goto _LL305;_tmp448=((struct Cyc_Absyn_TypeInt_struct*)_tmp447)->f1;
_tmp449=_tmp3BC.f2;if(_tmp449 <= (void*)4)goto _LL305;if(*((int*)_tmp449)!= 19)
goto _LL305;_tmp44A=((struct Cyc_Absyn_TypeInt_struct*)_tmp449)->f1;_LL304: return
Cyc_Core_intcmp(_tmp448,_tmp44A);_LL305: _tmp44B=_tmp3BC.f1;if(_tmp44B <= (void*)4)
goto _LL307;if(*((int*)_tmp44B)!= 21)goto _LL307;_LL306: goto _LL308;_LL307: _tmp44C=
_tmp3BC.f2;if(_tmp44C <= (void*)4)goto _LL309;if(*((int*)_tmp44C)!= 21)goto _LL309;
_LL308: goto _LL30A;_LL309: _tmp44D=_tmp3BC.f1;if(_tmp44D <= (void*)4)goto _LL30B;if(*((
int*)_tmp44D)!= 20)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp44E=_tmp3BC.f1;if(
_tmp44E <= (void*)4)goto _LL30D;if(*((int*)_tmp44E)!= 22)goto _LL30D;_LL30C: goto
_LL30E;_LL30D: _tmp44F=_tmp3BC.f2;if(_tmp44F <= (void*)4)goto _LL30F;if(*((int*)
_tmp44F)!= 22)goto _LL30F;_LL30E: goto _LL310;_LL30F: _tmp450=_tmp3BC.f2;if(_tmp450
<= (void*)4)goto _LL311;if(*((int*)_tmp450)!= 20)goto _LL311;_LL310:({void*_tmp46C[
0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp46D="typecmp: effects not handled";_tag_dynforward(_tmp46D,sizeof(
char),_get_zero_arr_size(_tmp46D,29));}),_tag_dynforward(_tmp46C,sizeof(void*),0));});
_LL311:;_LL312:({void*_tmp46E[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp46F="Unmatched case in typecmp";
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size(_tmp46F,26));}),
_tag_dynforward(_tmp46E,sizeof(void*),0));});_LL2DC:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp470=Cyc_Tcutil_compress(t);_LL31B: if(_tmp470 <= (void*)4)goto
_LL31D;if(*((int*)_tmp470)!= 5)goto _LL31D;_LL31C: goto _LL31E;_LL31D: if((int)
_tmp470 != 1)goto _LL31F;_LL31E: goto _LL320;_LL31F: if(_tmp470 <= (void*)4)goto _LL325;
if(*((int*)_tmp470)!= 6)goto _LL321;_LL320: goto _LL322;_LL321: if(*((int*)_tmp470)
!= 12)goto _LL323;_LL322: goto _LL324;_LL323: if(*((int*)_tmp470)!= 13)goto _LL325;
_LL324: return 1;_LL325:;_LL326: return 0;_LL31A:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp472=({struct _tuple0 _tmp471;_tmp471.f1=t1;_tmp471.f2=t2;_tmp471;});
void*_tmp473;int _tmp474;void*_tmp475;int _tmp476;void*_tmp477;void*_tmp478;void*
_tmp479;void*_tmp47A;void*_tmp47B;void*_tmp47C;void*_tmp47D;void*_tmp47E;void*
_tmp47F;void*_tmp480;void*_tmp481;void*_tmp482;void*_tmp483;void*_tmp484;void*
_tmp485;void*_tmp486;void*_tmp487;void*_tmp488;void*_tmp489;void*_tmp48A;void*
_tmp48B;void*_tmp48C;void*_tmp48D;void*_tmp48E;void*_tmp48F;void*_tmp490;void*
_tmp491;void*_tmp492;void*_tmp493;void*_tmp494;void*_tmp495;void*_tmp496;void*
_tmp497;void*_tmp498;void*_tmp499;void*_tmp49A;void*_tmp49B;void*_tmp49C;void*
_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;void*_tmp4A1;void*_tmp4A2;void*
_tmp4A3;void*_tmp4A4;void*_tmp4A5;void*_tmp4A6;void*_tmp4A7;void*_tmp4A8;void*
_tmp4A9;void*_tmp4AA;void*_tmp4AB;void*_tmp4AC;void*_tmp4AD;void*_tmp4AE;void*
_tmp4AF;void*_tmp4B0;void*_tmp4B1;void*_tmp4B2;void*_tmp4B3;void*_tmp4B4;void*
_tmp4B5;void*_tmp4B6;void*_tmp4B7;void*_tmp4B8;_LL328: _tmp473=_tmp472.f1;if(
_tmp473 <= (void*)4)goto _LL32A;if(*((int*)_tmp473)!= 6)goto _LL32A;_tmp474=((
struct Cyc_Absyn_DoubleType_struct*)_tmp473)->f1;_tmp475=_tmp472.f2;if(_tmp475 <= (
void*)4)goto _LL32A;if(*((int*)_tmp475)!= 6)goto _LL32A;_tmp476=((struct Cyc_Absyn_DoubleType_struct*)
_tmp475)->f1;_LL329: return !_tmp476  && _tmp474;_LL32A: _tmp477=_tmp472.f1;if(
_tmp477 <= (void*)4)goto _LL32C;if(*((int*)_tmp477)!= 6)goto _LL32C;_tmp478=_tmp472.f2;
if((int)_tmp478 != 1)goto _LL32C;_LL32B: goto _LL32D;_LL32C: _tmp479=_tmp472.f1;if(
_tmp479 <= (void*)4)goto _LL32E;if(*((int*)_tmp479)!= 6)goto _LL32E;_tmp47A=_tmp472.f2;
if(_tmp47A <= (void*)4)goto _LL32E;if(*((int*)_tmp47A)!= 5)goto _LL32E;_LL32D: goto
_LL32F;_LL32E: _tmp47B=_tmp472.f1;if(_tmp47B <= (void*)4)goto _LL330;if(*((int*)
_tmp47B)!= 6)goto _LL330;_tmp47C=_tmp472.f2;if(_tmp47C <= (void*)4)goto _LL330;if(*((
int*)_tmp47C)!= 14)goto _LL330;_LL32F: goto _LL331;_LL330: _tmp47D=_tmp472.f1;if((
int)_tmp47D != 1)goto _LL332;_tmp47E=_tmp472.f2;if(_tmp47E <= (void*)4)goto _LL332;
if(*((int*)_tmp47E)!= 14)goto _LL332;_LL331: goto _LL333;_LL332: _tmp47F=_tmp472.f1;
if(_tmp47F <= (void*)4)goto _LL334;if(*((int*)_tmp47F)!= 6)goto _LL334;_tmp480=
_tmp472.f2;if(_tmp480 <= (void*)4)goto _LL334;if(*((int*)_tmp480)!= 18)goto _LL334;
_LL333: goto _LL335;_LL334: _tmp481=_tmp472.f1;if((int)_tmp481 != 1)goto _LL336;
_tmp482=_tmp472.f2;if(_tmp482 <= (void*)4)goto _LL336;if(*((int*)_tmp482)!= 18)
goto _LL336;_LL335: goto _LL337;_LL336: _tmp483=_tmp472.f1;if((int)_tmp483 != 1)goto
_LL338;_tmp484=_tmp472.f2;if(_tmp484 <= (void*)4)goto _LL338;if(*((int*)_tmp484)!= 
5)goto _LL338;_LL337: return 1;_LL338: _tmp485=_tmp472.f1;if(_tmp485 <= (void*)4)goto
_LL33A;if(*((int*)_tmp485)!= 5)goto _LL33A;_tmp486=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp485)->f2;if((int)_tmp486 != 4)goto _LL33A;_tmp487=_tmp472.f2;if(_tmp487 <= (
void*)4)goto _LL33A;if(*((int*)_tmp487)!= 5)goto _LL33A;_tmp488=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp487)->f2;if((int)_tmp488 != 4)goto _LL33A;_LL339: return 0;_LL33A: _tmp489=
_tmp472.f1;if(_tmp489 <= (void*)4)goto _LL33C;if(*((int*)_tmp489)!= 5)goto _LL33C;
_tmp48A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp489)->f2;if((int)_tmp48A != 
4)goto _LL33C;_LL33B: return 1;_LL33C: _tmp48B=_tmp472.f1;if(_tmp48B <= (void*)4)goto
_LL33E;if(*((int*)_tmp48B)!= 5)goto _LL33E;_tmp48C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48B)->f2;if((int)_tmp48C != 3)goto _LL33E;_tmp48D=_tmp472.f2;if(_tmp48D <= (
void*)4)goto _LL33E;if(*((int*)_tmp48D)!= 5)goto _LL33E;_tmp48E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48D)->f2;if((int)_tmp48E != 2)goto _LL33E;_LL33D: goto _LL33F;_LL33E: _tmp48F=
_tmp472.f1;if(_tmp48F <= (void*)4)goto _LL340;if(*((int*)_tmp48F)!= 5)goto _LL340;
_tmp490=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48F)->f2;if((int)_tmp490 != 
2)goto _LL340;_tmp491=_tmp472.f2;if(_tmp491 <= (void*)4)goto _LL340;if(*((int*)
_tmp491)!= 5)goto _LL340;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp491)->f2;
if((int)_tmp492 != 3)goto _LL340;_LL33F: return 0;_LL340: _tmp493=_tmp472.f1;if(
_tmp493 <= (void*)4)goto _LL342;if(*((int*)_tmp493)!= 5)goto _LL342;_tmp494=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp493)->f2;if((int)_tmp494 != 3)goto _LL342;
_tmp495=_tmp472.f2;if((int)_tmp495 != 1)goto _LL342;_LL341: goto _LL343;_LL342:
_tmp496=_tmp472.f1;if(_tmp496 <= (void*)4)goto _LL344;if(*((int*)_tmp496)!= 5)goto
_LL344;_tmp497=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp496)->f2;if((int)
_tmp497 != 2)goto _LL344;_tmp498=_tmp472.f2;if((int)_tmp498 != 1)goto _LL344;_LL343:
goto _LL345;_LL344: _tmp499=_tmp472.f1;if(_tmp499 <= (void*)4)goto _LL346;if(*((int*)
_tmp499)!= 5)goto _LL346;_tmp49A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp499)->f2;
if((int)_tmp49A != 3)goto _LL346;_tmp49B=_tmp472.f2;if(_tmp49B <= (void*)4)goto
_LL346;if(*((int*)_tmp49B)!= 5)goto _LL346;_tmp49C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49B)->f2;if((int)_tmp49C != 1)goto _LL346;_LL345: goto _LL347;_LL346: _tmp49D=
_tmp472.f1;if(_tmp49D <= (void*)4)goto _LL348;if(*((int*)_tmp49D)!= 5)goto _LL348;
_tmp49E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49D)->f2;if((int)_tmp49E != 
2)goto _LL348;_tmp49F=_tmp472.f2;if(_tmp49F <= (void*)4)goto _LL348;if(*((int*)
_tmp49F)!= 5)goto _LL348;_tmp4A0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49F)->f2;
if((int)_tmp4A0 != 1)goto _LL348;_LL347: goto _LL349;_LL348: _tmp4A1=_tmp472.f1;if(
_tmp4A1 <= (void*)4)goto _LL34A;if(*((int*)_tmp4A1)!= 18)goto _LL34A;_tmp4A2=
_tmp472.f2;if(_tmp4A2 <= (void*)4)goto _LL34A;if(*((int*)_tmp4A2)!= 5)goto _LL34A;
_tmp4A3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A2)->f2;if((int)_tmp4A3 != 
1)goto _LL34A;_LL349: goto _LL34B;_LL34A: _tmp4A4=_tmp472.f1;if(_tmp4A4 <= (void*)4)
goto _LL34C;if(*((int*)_tmp4A4)!= 14)goto _LL34C;_tmp4A5=_tmp472.f2;if(_tmp4A5 <= (
void*)4)goto _LL34C;if(*((int*)_tmp4A5)!= 5)goto _LL34C;_tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A5)->f2;if((int)_tmp4A6 != 1)goto _LL34C;_LL34B: goto _LL34D;_LL34C: _tmp4A7=
_tmp472.f1;if(_tmp4A7 <= (void*)4)goto _LL34E;if(*((int*)_tmp4A7)!= 5)goto _LL34E;
_tmp4A8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A7)->f2;if((int)_tmp4A8 != 
3)goto _LL34E;_tmp4A9=_tmp472.f2;if(_tmp4A9 <= (void*)4)goto _LL34E;if(*((int*)
_tmp4A9)!= 5)goto _LL34E;_tmp4AA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A9)->f2;
if((int)_tmp4AA != 0)goto _LL34E;_LL34D: goto _LL34F;_LL34E: _tmp4AB=_tmp472.f1;if(
_tmp4AB <= (void*)4)goto _LL350;if(*((int*)_tmp4AB)!= 5)goto _LL350;_tmp4AC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4AB)->f2;if((int)_tmp4AC != 2)goto _LL350;
_tmp4AD=_tmp472.f2;if(_tmp4AD <= (void*)4)goto _LL350;if(*((int*)_tmp4AD)!= 5)goto
_LL350;_tmp4AE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AD)->f2;if((int)
_tmp4AE != 0)goto _LL350;_LL34F: goto _LL351;_LL350: _tmp4AF=_tmp472.f1;if(_tmp4AF <= (
void*)4)goto _LL352;if(*((int*)_tmp4AF)!= 5)goto _LL352;_tmp4B0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AF)->f2;if((int)_tmp4B0 != 1)goto _LL352;_tmp4B1=_tmp472.f2;if(_tmp4B1 <= (
void*)4)goto _LL352;if(*((int*)_tmp4B1)!= 5)goto _LL352;_tmp4B2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B1)->f2;if((int)_tmp4B2 != 0)goto _LL352;_LL351: goto _LL353;_LL352: _tmp4B3=
_tmp472.f1;if(_tmp4B3 <= (void*)4)goto _LL354;if(*((int*)_tmp4B3)!= 18)goto _LL354;
_tmp4B4=_tmp472.f2;if(_tmp4B4 <= (void*)4)goto _LL354;if(*((int*)_tmp4B4)!= 5)goto
_LL354;_tmp4B5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B4)->f2;if((int)
_tmp4B5 != 0)goto _LL354;_LL353: goto _LL355;_LL354: _tmp4B6=_tmp472.f1;if(_tmp4B6 <= (
void*)4)goto _LL356;if(*((int*)_tmp4B6)!= 14)goto _LL356;_tmp4B7=_tmp472.f2;if(
_tmp4B7 <= (void*)4)goto _LL356;if(*((int*)_tmp4B7)!= 5)goto _LL356;_tmp4B8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4B7)->f2;if((int)_tmp4B8 != 0)goto _LL356;
_LL355: return 1;_LL356:;_LL357: return 0;_LL327:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){{struct _RegionHandle _tmp4B9=
_new_region("r");struct _RegionHandle*r=& _tmp4B9;_push_region(r);{struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp4BA=
_region_malloc(r,sizeof(*_tmp4BA));_tmp4BA->v=(void*)t1;_tmp4BA;});}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){int _tmp4BB=
0;_npop_handler(0);return _tmp4BB;}}};_pop_region(r);}{struct Cyc_List_List*el=es;
for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,
t)){({struct Cyc_String_pa_struct _tmp4BF;_tmp4BF.tag=0;_tmp4BF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp4BE;_tmp4BE.tag=0;_tmp4BE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp4BC[2]={& _tmp4BE,& _tmp4BF};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp4BD="type mismatch: expecting %s but found %s";
_tag_dynforward(_tmp4BD,sizeof(char),_get_zero_arr_size(_tmp4BD,41));}),
_tag_dynforward(_tmp4BC,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp4C0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL359: if(_tmp4C0 <= (void*)4)goto _LL35B;if(*((int*)_tmp4C0)!= 4)goto _LL35B;
_LL35A: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL358;
_LL35B:;_LL35C: return 0;_LL358:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp4C1=Cyc_Tcutil_compress(t);_LL35E: if(_tmp4C1 <= (void*)4)goto _LL368;if(*((
int*)_tmp4C1)!= 5)goto _LL360;_LL35F: goto _LL361;_LL360: if(*((int*)_tmp4C1)!= 14)
goto _LL362;_LL361: goto _LL363;_LL362: if(*((int*)_tmp4C1)!= 18)goto _LL364;_LL363:
goto _LL365;_LL364: if(*((int*)_tmp4C1)!= 12)goto _LL366;_LL365: goto _LL367;_LL366:
if(*((int*)_tmp4C1)!= 13)goto _LL368;_LL367: return 1;_LL368:;_LL369: return 0;_LL35D:;}
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*_tmp4C2[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4C3="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4C3,sizeof(char),_get_zero_arr_size(_tmp4C3,44));}),
_tag_dynforward(_tmp4C2,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp4C4[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp4C5="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4C5,sizeof(char),_get_zero_arr_size(_tmp4C5,44));}),
_tag_dynforward(_tmp4C4,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union f1;
union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp4C7=({struct _tuple0 _tmp4C6;_tmp4C6.f1=
t1;_tmp4C6.f2=t2;_tmp4C6;});void*_tmp4C8;struct Cyc_Absyn_PtrInfo _tmp4C9;void*
_tmp4CA;struct Cyc_Absyn_PtrInfo _tmp4CB;void*_tmp4CC;struct Cyc_Absyn_TunionInfo
_tmp4CD;union Cyc_Absyn_TunionInfoU_union _tmp4CE;struct Cyc_Absyn_Tuniondecl**
_tmp4CF;struct Cyc_Absyn_Tuniondecl*_tmp4D0;struct Cyc_List_List*_tmp4D1;struct Cyc_Core_Opt*
_tmp4D2;struct Cyc_Core_Opt _tmp4D3;void*_tmp4D4;void*_tmp4D5;struct Cyc_Absyn_TunionInfo
_tmp4D6;union Cyc_Absyn_TunionInfoU_union _tmp4D7;struct Cyc_Absyn_Tuniondecl**
_tmp4D8;struct Cyc_Absyn_Tuniondecl*_tmp4D9;struct Cyc_List_List*_tmp4DA;struct Cyc_Core_Opt*
_tmp4DB;struct Cyc_Core_Opt _tmp4DC;void*_tmp4DD;void*_tmp4DE;struct Cyc_Absyn_ArrayInfo
_tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Tqual _tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;
struct Cyc_Absyn_Conref*_tmp4E3;void*_tmp4E4;struct Cyc_Absyn_ArrayInfo _tmp4E5;
void*_tmp4E6;struct Cyc_Absyn_Tqual _tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Conref*
_tmp4E9;void*_tmp4EA;struct Cyc_Absyn_TunionFieldInfo _tmp4EB;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4EC;struct Cyc_Absyn_Tuniondecl*_tmp4ED;struct Cyc_Absyn_Tunionfield*_tmp4EE;
struct Cyc_List_List*_tmp4EF;void*_tmp4F0;struct Cyc_Absyn_TunionInfo _tmp4F1;union
Cyc_Absyn_TunionInfoU_union _tmp4F2;struct Cyc_Absyn_Tuniondecl**_tmp4F3;struct Cyc_Absyn_Tuniondecl*
_tmp4F4;struct Cyc_List_List*_tmp4F5;void*_tmp4F6;struct Cyc_Absyn_PtrInfo _tmp4F7;
void*_tmp4F8;struct Cyc_Absyn_Tqual _tmp4F9;struct Cyc_Absyn_PtrAtts _tmp4FA;void*
_tmp4FB;struct Cyc_Absyn_Conref*_tmp4FC;struct Cyc_Absyn_Conref*_tmp4FD;struct Cyc_Absyn_Conref*
_tmp4FE;void*_tmp4FF;struct Cyc_Absyn_TunionInfo _tmp500;union Cyc_Absyn_TunionInfoU_union
_tmp501;struct Cyc_Absyn_Tuniondecl**_tmp502;struct Cyc_Absyn_Tuniondecl*_tmp503;
struct Cyc_List_List*_tmp504;struct Cyc_Core_Opt*_tmp505;void*_tmp506;void*_tmp507;
void*_tmp508;void*_tmp509;void*_tmp50A;void*_tmp50B;void*_tmp50C;void*_tmp50D;
_LL36B: _tmp4C8=_tmp4C7.f1;if(_tmp4C8 <= (void*)4)goto _LL36D;if(*((int*)_tmp4C8)!= 
4)goto _LL36D;_tmp4C9=((struct Cyc_Absyn_PointerType_struct*)_tmp4C8)->f1;_tmp4CA=
_tmp4C7.f2;if(_tmp4CA <= (void*)4)goto _LL36D;if(*((int*)_tmp4CA)!= 4)goto _LL36D;
_tmp4CB=((struct Cyc_Absyn_PointerType_struct*)_tmp4CA)->f1;_LL36C: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4C9.ptr_atts).nullable,(
_tmp4CB.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp50E=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp4C9.ptr_atts).nullable))->v;
int _tmp50F;_LL37E: if((_tmp50E.Eq_constr).tag != 0)goto _LL380;_tmp50F=(_tmp50E.Eq_constr).f1;
_LL37F: okay=!_tmp50F;goto _LL37D;_LL380:;_LL381:({void*_tmp510[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp511="silent_castable conref not eq";_tag_dynforward(_tmp511,sizeof(char),
_get_zero_arr_size(_tmp511,30));}),_tag_dynforward(_tmp510,sizeof(void*),0));});
_LL37D:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp4C9.ptr_atts).bounds,(
_tmp4CB.ptr_atts).bounds)){struct _tuple13 _tmp513=({struct _tuple13 _tmp512;_tmp512.f1=(
Cyc_Absyn_compress_conref((_tmp4C9.ptr_atts).bounds))->v;_tmp512.f2=(Cyc_Absyn_compress_conref((
_tmp4CB.ptr_atts).bounds))->v;_tmp512;});union Cyc_Absyn_Constraint_union _tmp514;
void*_tmp515;union Cyc_Absyn_Constraint_union _tmp516;void*_tmp517;union Cyc_Absyn_Constraint_union
_tmp518;_LL383: _tmp514=_tmp513.f1;if(((_tmp513.f1).Eq_constr).tag != 0)goto _LL385;
_tmp515=(_tmp514.Eq_constr).f1;_tmp516=_tmp513.f2;if(((_tmp513.f2).Eq_constr).tag
!= 0)goto _LL385;_tmp517=(_tmp516.Eq_constr).f1;_LL384:{struct _tuple0 _tmp51A=({
struct _tuple0 _tmp519;_tmp519.f1=_tmp515;_tmp519.f2=_tmp517;_tmp519;});void*
_tmp51B;void*_tmp51C;void*_tmp51D;void*_tmp51E;void*_tmp51F;void*_tmp520;void*
_tmp521;void*_tmp522;void*_tmp523;void*_tmp524;void*_tmp525;void*_tmp526;void*
_tmp527;struct Cyc_Absyn_Exp*_tmp528;void*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;
void*_tmp52B;void*_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;void*_tmp52E;void*_tmp52F;
struct Cyc_Absyn_Exp*_tmp530;void*_tmp531;void*_tmp532;void*_tmp533;void*_tmp534;
void*_tmp535;struct Cyc_Absyn_Exp*_tmp536;void*_tmp537;void*_tmp538;void*_tmp539;
void*_tmp53A;_LL38A: _tmp51B=_tmp51A.f1;if(_tmp51B <= (void*)2)goto _LL38C;if(*((
int*)_tmp51B)!= 0)goto _LL38C;_tmp51C=_tmp51A.f2;if((int)_tmp51C != 0)goto _LL38C;
_LL38B: goto _LL38D;_LL38C: _tmp51D=_tmp51A.f1;if(_tmp51D <= (void*)2)goto _LL38E;if(*((
int*)_tmp51D)!= 0)goto _LL38E;_tmp51E=_tmp51A.f2;if((int)_tmp51E != 1)goto _LL38E;
_LL38D: goto _LL38F;_LL38E: _tmp51F=_tmp51A.f1;if((int)_tmp51F != 1)goto _LL390;
_tmp520=_tmp51A.f2;if((int)_tmp520 != 1)goto _LL390;_LL38F: goto _LL391;_LL390:
_tmp521=_tmp51A.f1;if((int)_tmp521 != 0)goto _LL392;_tmp522=_tmp51A.f2;if((int)
_tmp522 != 1)goto _LL392;_LL391: goto _LL393;_LL392: _tmp523=_tmp51A.f1;if((int)
_tmp523 != 0)goto _LL394;_tmp524=_tmp51A.f2;if((int)_tmp524 != 0)goto _LL394;_LL393:
okay=1;goto _LL389;_LL394: _tmp525=_tmp51A.f1;if((int)_tmp525 != 1)goto _LL396;
_tmp526=_tmp51A.f2;if((int)_tmp526 != 0)goto _LL396;_LL395: okay=0;goto _LL389;
_LL396: _tmp527=_tmp51A.f1;if(_tmp527 <= (void*)2)goto _LL398;if(*((int*)_tmp527)!= 
0)goto _LL398;_tmp528=((struct Cyc_Absyn_Upper_b_struct*)_tmp527)->f1;_tmp529=
_tmp51A.f2;if(_tmp529 <= (void*)2)goto _LL398;if(*((int*)_tmp529)!= 0)goto _LL398;
_tmp52A=((struct Cyc_Absyn_Upper_b_struct*)_tmp529)->f1;_LL397: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp52A,_tmp528);if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4CB.ptr_atts).zero_term))({void*_tmp53B[0]={};Cyc_Tcutil_warn(loc,({const
char*_tmp53C="implicit cast to shorter array";_tag_dynforward(_tmp53C,sizeof(
char),_get_zero_arr_size(_tmp53C,31));}),_tag_dynforward(_tmp53B,sizeof(void*),0));});
goto _LL389;_LL398: _tmp52B=_tmp51A.f1;if((int)_tmp52B != 0)goto _LL39A;_tmp52C=
_tmp51A.f2;if(_tmp52C <= (void*)2)goto _LL39A;if(*((int*)_tmp52C)!= 0)goto _LL39A;
_tmp52D=((struct Cyc_Absyn_Upper_b_struct*)_tmp52C)->f1;_LL399: _tmp530=_tmp52D;
goto _LL39B;_LL39A: _tmp52E=_tmp51A.f1;if((int)_tmp52E != 1)goto _LL39C;_tmp52F=
_tmp51A.f2;if(_tmp52F <= (void*)2)goto _LL39C;if(*((int*)_tmp52F)!= 0)goto _LL39C;
_tmp530=((struct Cyc_Absyn_Upper_b_struct*)_tmp52F)->f1;_LL39B: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4C9.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4CB.ptr_atts).bounds))goto _LL389;okay=0;goto
_LL389;_LL39C: _tmp531=_tmp51A.f1;if(_tmp531 <= (void*)2)goto _LL39E;if(*((int*)
_tmp531)!= 1)goto _LL39E;_tmp532=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp531)->f1;_tmp533=_tmp51A.f2;if(_tmp533 <= (void*)2)goto _LL39E;if(*((int*)
_tmp533)!= 1)goto _LL39E;_tmp534=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp533)->f1;_LL39D: if(!Cyc_Tcutil_unify(_tmp532,_tmp534)){struct _tuple0 _tmp53E=({
struct _tuple0 _tmp53D;_tmp53D.f1=Cyc_Tcutil_compress(_tmp532);_tmp53D.f2=Cyc_Tcutil_compress(
_tmp534);_tmp53D;});void*_tmp53F;int _tmp540;void*_tmp541;int _tmp542;_LL3A5:
_tmp53F=_tmp53E.f1;if(_tmp53F <= (void*)4)goto _LL3A7;if(*((int*)_tmp53F)!= 19)
goto _LL3A7;_tmp540=((struct Cyc_Absyn_TypeInt_struct*)_tmp53F)->f1;_tmp541=
_tmp53E.f2;if(_tmp541 <= (void*)4)goto _LL3A7;if(*((int*)_tmp541)!= 19)goto _LL3A7;
_tmp542=((struct Cyc_Absyn_TypeInt_struct*)_tmp541)->f1;_LL3A6: if(_tmp540 < 
_tmp542)okay=0;goto _LL3A4;_LL3A7:;_LL3A8: okay=0;goto _LL3A4;_LL3A4:;}goto _LL389;
_LL39E: _tmp535=_tmp51A.f1;if(_tmp535 <= (void*)2)goto _LL3A0;if(*((int*)_tmp535)!= 
0)goto _LL3A0;_tmp536=((struct Cyc_Absyn_Upper_b_struct*)_tmp535)->f1;_tmp537=
_tmp51A.f2;if(_tmp537 <= (void*)2)goto _LL3A0;if(*((int*)_tmp537)!= 1)goto _LL3A0;
_tmp538=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp537)->f1;_LL39F: {
unsigned int _tmp544;int _tmp545;struct _tuple7 _tmp543=Cyc_Evexp_eval_const_uint_exp(
_tmp536);_tmp544=_tmp543.f1;_tmp545=_tmp543.f2;if(!_tmp545){okay=0;goto _LL389;}{
void*_tmp546=Cyc_Tcutil_compress(_tmp538);int _tmp547;_LL3AA: if(_tmp546 <= (void*)
4)goto _LL3AC;if(*((int*)_tmp546)!= 19)goto _LL3AC;_tmp547=((struct Cyc_Absyn_TypeInt_struct*)
_tmp546)->f1;_LL3AB: if(_tmp544 < _tmp547)okay=0;goto _LL3A9;_LL3AC:;_LL3AD: okay=0;
goto _LL3A9;_LL3A9:;}goto _LL389;}_LL3A0: _tmp539=_tmp51A.f1;if(_tmp539 <= (void*)2)
goto _LL3A2;if(*((int*)_tmp539)!= 1)goto _LL3A2;_LL3A1: goto _LL3A3;_LL3A2: _tmp53A=
_tmp51A.f2;if(_tmp53A <= (void*)2)goto _LL389;if(*((int*)_tmp53A)!= 1)goto _LL389;
_LL3A3: okay=0;goto _LL389;_LL389:;}goto _LL382;_LL385: _tmp518=_tmp513.f1;if(((
_tmp513.f1).No_constr).tag != 2)goto _LL387;_LL386: okay=0;goto _LL382;_LL387:;
_LL388:({void*_tmp548[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp549="silent_castable conrefs didn't unify";
_tag_dynforward(_tmp549,sizeof(char),_get_zero_arr_size(_tmp549,37));}),
_tag_dynforward(_tmp548,sizeof(void*),0));});_LL382:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp4C9.elt_typ,(void*)_tmp4CB.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp4C9.ptr_atts).rgn,(void*)(_tmp4CB.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp4C9.ptr_atts).rgn,(void*)(_tmp4CB.ptr_atts).rgn));okay=okay  && (!(
_tmp4C9.elt_tq).real_const  || (_tmp4CB.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp4C9.ptr_atts).zero_term,(_tmp4CB.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp4C9.ptr_atts).zero_term)
 && (_tmp4CB.elt_tq).real_const);return okay;}_LL36D: _tmp4CC=_tmp4C7.f1;if(
_tmp4CC <= (void*)4)goto _LL36F;if(*((int*)_tmp4CC)!= 2)goto _LL36F;_tmp4CD=((
struct Cyc_Absyn_TunionType_struct*)_tmp4CC)->f1;_tmp4CE=_tmp4CD.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp4CC)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL36F;_tmp4CF=(_tmp4CE.KnownTunion).f1;_tmp4D0=*_tmp4CF;_tmp4D1=_tmp4CD.targs;
_tmp4D2=_tmp4CD.rgn;if(_tmp4D2 == 0)goto _LL36F;_tmp4D3=*_tmp4D2;_tmp4D4=(void*)
_tmp4D3.v;_tmp4D5=_tmp4C7.f2;if(_tmp4D5 <= (void*)4)goto _LL36F;if(*((int*)_tmp4D5)
!= 2)goto _LL36F;_tmp4D6=((struct Cyc_Absyn_TunionType_struct*)_tmp4D5)->f1;
_tmp4D7=_tmp4D6.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp4D5)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL36F;_tmp4D8=(_tmp4D7.KnownTunion).f1;_tmp4D9=*_tmp4D8;_tmp4DA=
_tmp4D6.targs;_tmp4DB=_tmp4D6.rgn;if(_tmp4DB == 0)goto _LL36F;_tmp4DC=*_tmp4DB;
_tmp4DD=(void*)_tmp4DC.v;_LL36E: if(_tmp4D0 != _tmp4D9  || !Cyc_Tcenv_region_outlives(
te,_tmp4D4,_tmp4DD))return 0;for(0;_tmp4D1 != 0  && _tmp4DA != 0;(_tmp4D1=_tmp4D1->tl,
_tmp4DA=_tmp4DA->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4D1->hd,(void*)_tmp4DA->hd))
return 0;}if(_tmp4D1 != 0  || _tmp4DA != 0)return 0;return 1;_LL36F: _tmp4DE=_tmp4C7.f1;
if(_tmp4DE <= (void*)4)goto _LL371;if(*((int*)_tmp4DE)!= 7)goto _LL371;_tmp4DF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4DE)->f1;_tmp4E0=(void*)_tmp4DF.elt_type;
_tmp4E1=_tmp4DF.tq;_tmp4E2=_tmp4DF.num_elts;_tmp4E3=_tmp4DF.zero_term;_tmp4E4=
_tmp4C7.f2;if(_tmp4E4 <= (void*)4)goto _LL371;if(*((int*)_tmp4E4)!= 7)goto _LL371;
_tmp4E5=((struct Cyc_Absyn_ArrayType_struct*)_tmp4E4)->f1;_tmp4E6=(void*)_tmp4E5.elt_type;
_tmp4E7=_tmp4E5.tq;_tmp4E8=_tmp4E5.num_elts;_tmp4E9=_tmp4E5.zero_term;_LL370: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4E3,_tmp4E9) && ((_tmp4E2 != 0
 && _tmp4E8 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4E2,(
struct Cyc_Absyn_Exp*)_tmp4E8));return(okay  && Cyc_Tcutil_unify(_tmp4E0,_tmp4E6))
 && (!_tmp4E1.real_const  || _tmp4E7.real_const);}_LL371: _tmp4EA=_tmp4C7.f1;if(
_tmp4EA <= (void*)4)goto _LL373;if(*((int*)_tmp4EA)!= 3)goto _LL373;_tmp4EB=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4EA)->f1;_tmp4EC=_tmp4EB.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4EA)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL373;_tmp4ED=(_tmp4EC.KnownTunionfield).f1;_tmp4EE=(_tmp4EC.KnownTunionfield).f2;
_tmp4EF=_tmp4EB.targs;_tmp4F0=_tmp4C7.f2;if(_tmp4F0 <= (void*)4)goto _LL373;if(*((
int*)_tmp4F0)!= 2)goto _LL373;_tmp4F1=((struct Cyc_Absyn_TunionType_struct*)
_tmp4F0)->f1;_tmp4F2=_tmp4F1.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4F0)->f1).tunion_info).KnownTunion).tag != 1)goto _LL373;_tmp4F3=(_tmp4F2.KnownTunion).f1;
_tmp4F4=*_tmp4F3;_tmp4F5=_tmp4F1.targs;_LL372: if((_tmp4ED == _tmp4F4  || Cyc_Absyn_qvar_cmp(
_tmp4ED->name,_tmp4F4->name)== 0) && (_tmp4EE->typs == 0  || _tmp4ED->is_flat)){
for(0;_tmp4EF != 0  && _tmp4F5 != 0;(_tmp4EF=_tmp4EF->tl,_tmp4F5=_tmp4F5->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4EF->hd,(void*)_tmp4F5->hd))break;}if(_tmp4EF == 0
 && _tmp4F5 == 0)return 1;}return 0;_LL373: _tmp4F6=_tmp4C7.f1;if(_tmp4F6 <= (void*)4)
goto _LL375;if(*((int*)_tmp4F6)!= 4)goto _LL375;_tmp4F7=((struct Cyc_Absyn_PointerType_struct*)
_tmp4F6)->f1;_tmp4F8=(void*)_tmp4F7.elt_typ;_tmp4F9=_tmp4F7.elt_tq;_tmp4FA=
_tmp4F7.ptr_atts;_tmp4FB=(void*)_tmp4FA.rgn;_tmp4FC=_tmp4FA.nullable;_tmp4FD=
_tmp4FA.bounds;_tmp4FE=_tmp4FA.zero_term;_tmp4FF=_tmp4C7.f2;if(_tmp4FF <= (void*)
4)goto _LL375;if(*((int*)_tmp4FF)!= 2)goto _LL375;_tmp500=((struct Cyc_Absyn_TunionType_struct*)
_tmp4FF)->f1;_tmp501=_tmp500.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4FF)->f1).tunion_info).KnownTunion).tag != 1)goto _LL375;_tmp502=(_tmp501.KnownTunion).f1;
_tmp503=*_tmp502;_tmp504=_tmp500.targs;_tmp505=_tmp500.rgn;if(!(!_tmp503->is_flat))
goto _LL375;_LL374:{void*_tmp54A=Cyc_Tcutil_compress(_tmp4F8);struct Cyc_Absyn_TunionFieldInfo
_tmp54B;union Cyc_Absyn_TunionFieldInfoU_union _tmp54C;struct Cyc_Absyn_Tuniondecl*
_tmp54D;struct Cyc_Absyn_Tunionfield*_tmp54E;struct Cyc_List_List*_tmp54F;_LL3AF:
if(_tmp54A <= (void*)4)goto _LL3B1;if(*((int*)_tmp54A)!= 3)goto _LL3B1;_tmp54B=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp54A)->f1;_tmp54C=_tmp54B.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp54A)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL3B1;_tmp54D=(_tmp54C.KnownTunionfield).f1;_tmp54E=(_tmp54C.KnownTunionfield).f2;
_tmp54F=_tmp54B.targs;_LL3B0: if(!Cyc_Tcutil_unify(_tmp4FB,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp505))->v) && !Cyc_Tcenv_region_outlives(te,_tmp4FB,(void*)
_tmp505->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4FC,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp4FD,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4FE,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp503->name,_tmp54D->name)== 0  && _tmp54E->typs
!= 0){int okay=1;for(0;_tmp54F != 0  && _tmp504 != 0;(_tmp54F=_tmp54F->tl,_tmp504=
_tmp504->tl)){if(!Cyc_Tcutil_unify((void*)_tmp54F->hd,(void*)_tmp504->hd)){okay=
0;break;}}if((!okay  || _tmp54F != 0) || _tmp504 != 0)return 0;return 1;}goto _LL3AE;
_LL3B1:;_LL3B2: goto _LL3AE;_LL3AE:;}return 0;_LL375: _tmp506=_tmp4C7.f1;if(_tmp506
<= (void*)4)goto _LL377;if(*((int*)_tmp506)!= 14)goto _LL377;_tmp507=_tmp4C7.f2;
if(_tmp507 <= (void*)4)goto _LL377;if(*((int*)_tmp507)!= 5)goto _LL377;_tmp508=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp507)->f2;if((int)_tmp508 != 2)goto
_LL377;_LL376: goto _LL378;_LL377: _tmp509=_tmp4C7.f1;if(_tmp509 <= (void*)4)goto
_LL379;if(*((int*)_tmp509)!= 14)goto _LL379;_tmp50A=_tmp4C7.f2;if(_tmp50A <= (void*)
4)goto _LL379;if(*((int*)_tmp50A)!= 5)goto _LL379;_tmp50B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp50A)->f2;if((int)_tmp50B != 3)goto _LL379;_LL378: return 1;_LL379: _tmp50C=
_tmp4C7.f1;if(_tmp50C <= (void*)4)goto _LL37B;if(*((int*)_tmp50C)!= 18)goto _LL37B;
_tmp50D=_tmp4C7.f2;if(_tmp50D <= (void*)4)goto _LL37B;if(*((int*)_tmp50D)!= 5)goto
_LL37B;_LL37A: return 0;_LL37B:;_LL37C: return Cyc_Tcutil_unify(t1,t2);_LL36A:;}}int
Cyc_Tcutil_is_pointer_type(void*t){void*_tmp550=Cyc_Tcutil_compress(t);_LL3B4:
if(_tmp550 <= (void*)4)goto _LL3B6;if(*((int*)_tmp550)!= 4)goto _LL3B6;_LL3B5:
return 1;_LL3B6:;_LL3B7: return 0;_LL3B3:;}int Cyc_Tcutil_is_pointer_or_boxed(void*t,
int*is_dynforward_ptr,int*is_dyneither_ptr){void*_tmp551=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp552;struct Cyc_Absyn_PtrAtts _tmp553;struct Cyc_Absyn_Conref*
_tmp554;_LL3B9: if(_tmp551 <= (void*)4)goto _LL3BB;if(*((int*)_tmp551)!= 4)goto
_LL3BB;_tmp552=((struct Cyc_Absyn_PointerType_struct*)_tmp551)->f1;_tmp553=
_tmp552.ptr_atts;_tmp554=_tmp553.bounds;_LL3BA:*is_dynforward_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp554)== (void*)0;*is_dyneither_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp554)== (void*)1;return 1;_LL3BB:;_LL3BC: return Cyc_Tcutil_typ_kind(
t)== (void*)2;_LL3B8:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp555=(
void*)e->r;union Cyc_Absyn_Cnst_union _tmp556;int _tmp557;union Cyc_Absyn_Cnst_union
_tmp558;char _tmp559;union Cyc_Absyn_Cnst_union _tmp55A;short _tmp55B;union Cyc_Absyn_Cnst_union
_tmp55C;long long _tmp55D;void*_tmp55E;struct Cyc_Absyn_Exp*_tmp55F;_LL3BE: if(*((
int*)_tmp555)!= 0)goto _LL3C0;_tmp556=((struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1).Int_c).tag != 2)goto _LL3C0;
_tmp557=(_tmp556.Int_c).f2;if(_tmp557 != 0)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0:
if(*((int*)_tmp555)!= 0)goto _LL3C2;_tmp558=((struct Cyc_Absyn_Const_e_struct*)
_tmp555)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1).Char_c).tag != 
0)goto _LL3C2;_tmp559=(_tmp558.Char_c).f2;if(_tmp559 != 0)goto _LL3C2;_LL3C1: goto
_LL3C3;_LL3C2: if(*((int*)_tmp555)!= 0)goto _LL3C4;_tmp55A=((struct Cyc_Absyn_Const_e_struct*)
_tmp555)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1).Short_c).tag != 
1)goto _LL3C4;_tmp55B=(_tmp55A.Short_c).f2;if(_tmp55B != 0)goto _LL3C4;_LL3C3: goto
_LL3C5;_LL3C4: if(*((int*)_tmp555)!= 0)goto _LL3C6;_tmp55C=((struct Cyc_Absyn_Const_e_struct*)
_tmp555)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1).LongLong_c).tag
!= 3)goto _LL3C6;_tmp55D=(_tmp55C.LongLong_c).f2;if(_tmp55D != 0)goto _LL3C6;_LL3C5:
return 1;_LL3C6: if(*((int*)_tmp555)!= 15)goto _LL3C8;_tmp55E=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp555)->f1;_tmp55F=((struct Cyc_Absyn_Cast_e_struct*)_tmp555)->f2;_LL3C7: return
Cyc_Tcutil_is_zero(_tmp55F) && Cyc_Tcutil_admits_zero(_tmp55E);_LL3C8:;_LL3C9:
return 0;_LL3BD:;}struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((void*)5)};struct Cyc_Core_Opt
Cyc_Tcutil_urk={(void*)((void*)4)};struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)((void*)7)};struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)((void*)6)};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k){void*_tmp560=
k;_LL3CB: if((int)_tmp560 != 0)goto _LL3CD;_LL3CC: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;
_LL3CD: if((int)_tmp560 != 1)goto _LL3CF;_LL3CE: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;
_LL3CF: if((int)_tmp560 != 2)goto _LL3D1;_LL3D0: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;
_LL3D1: if((int)_tmp560 != 3)goto _LL3D3;_LL3D2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;
_LL3D3: if((int)_tmp560 != 4)goto _LL3D5;_LL3D4: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;
_LL3D5: if((int)_tmp560 != 5)goto _LL3D7;_LL3D6: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;
_LL3D7: if((int)_tmp560 != 6)goto _LL3D9;_LL3D8: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ek;
_LL3D9: if((int)_tmp560 != 7)goto _LL3CA;_LL3DA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ik;
_LL3CA:;}static void**Cyc_Tcutil_kind_to_b(void*k){static struct Cyc_Absyn_Eq_kb_struct
ab_v={0,(void*)((void*)0)};static struct Cyc_Absyn_Eq_kb_struct mb_v={0,(void*)((
void*)1)};static struct Cyc_Absyn_Eq_kb_struct bb_v={0,(void*)((void*)2)};static
struct Cyc_Absyn_Eq_kb_struct rb_v={0,(void*)((void*)3)};static struct Cyc_Absyn_Eq_kb_struct
ub_v={0,(void*)((void*)4)};static struct Cyc_Absyn_Eq_kb_struct tb_v={0,(void*)((
void*)5)};static struct Cyc_Absyn_Eq_kb_struct eb_v={0,(void*)((void*)6)};static
struct Cyc_Absyn_Eq_kb_struct ib_v={0,(void*)((void*)7)};static void*ab=(void*)&
ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;static void*rb=(void*)&
rb_v;static void*ub=(void*)& ub_v;static void*tb=(void*)& tb_v;static void*eb=(void*)&
eb_v;static void*ib=(void*)& ib_v;void*_tmp561=k;_LL3DC: if((int)_tmp561 != 0)goto
_LL3DE;_LL3DD: return& ab;_LL3DE: if((int)_tmp561 != 1)goto _LL3E0;_LL3DF: return& mb;
_LL3E0: if((int)_tmp561 != 2)goto _LL3E2;_LL3E1: return& bb;_LL3E2: if((int)_tmp561 != 
3)goto _LL3E4;_LL3E3: return& rb;_LL3E4: if((int)_tmp561 != 4)goto _LL3E6;_LL3E5:
return& ub;_LL3E6: if((int)_tmp561 != 5)goto _LL3E8;_LL3E7: return& tb;_LL3E8: if((int)
_tmp561 != 6)goto _LL3EA;_LL3E9: return& eb;_LL3EA: if((int)_tmp561 != 7)goto _LL3DB;
_LL3EB: return& ib;_LL3DB:;}void*Cyc_Tcutil_kind_to_bound(void*k){return*Cyc_Tcutil_kind_to_b(
k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(void*k){return(struct Cyc_Core_Opt*)
Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*
t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(
e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp56A=
_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_Absyn_Const_e_struct _tmp56B;
_tmp56B.tag=0;_tmp56B.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp56C;(_tmp56C.Null_c).tag=6;_tmp56C;});_tmp56B;});_tmp56A;})));{struct Cyc_Core_Opt*
_tmp56D=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp56E=({struct Cyc_Absyn_PointerType_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));
_tmp56F[0]=({struct Cyc_Absyn_PointerType_struct _tmp570;_tmp570.tag=4;_tmp570.f1=({
struct Cyc_Absyn_PtrInfo _tmp571;_tmp571.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp56D);_tmp571.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp571.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp572;_tmp572.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,_tmp56D);_tmp572.nullable=Cyc_Absyn_true_conref;
_tmp572.bounds=Cyc_Absyn_empty_conref();_tmp572.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp572.ptrloc=0;_tmp572;});_tmp571;});_tmp570;});
_tmp56F;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)
_tmp56E));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}struct _dynforward_ptr
Cyc_Tcutil_coercion2string(void*c){void*_tmp573=c;_LL3ED: if((int)_tmp573 != 0)
goto _LL3EF;_LL3EE: return({const char*_tmp574="unknown";_tag_dynforward(_tmp574,
sizeof(char),_get_zero_arr_size(_tmp574,8));});_LL3EF: if((int)_tmp573 != 1)goto
_LL3F1;_LL3F0: return({const char*_tmp575="no coercion";_tag_dynforward(_tmp575,
sizeof(char),_get_zero_arr_size(_tmp575,12));});_LL3F1: if((int)_tmp573 != 2)goto
_LL3F3;_LL3F2: return({const char*_tmp576="null check";_tag_dynforward(_tmp576,
sizeof(char),_get_zero_arr_size(_tmp576,11));});_LL3F3: if((int)_tmp573 != 3)goto
_LL3EC;_LL3F4: return({const char*_tmp577="other coercion";_tag_dynforward(_tmp577,
sizeof(char),_get_zero_arr_size(_tmp577,15));});_LL3EC:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp57B;_tmp57B.tag=0;_tmp57B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp57A;_tmp57A.tag=0;
_tmp57A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp578[2]={& _tmp57A,& _tmp57B};Cyc_Tcutil_warn(e->loc,({const char*
_tmp579="integral size mismatch; %s -> %s conversion supplied";_tag_dynforward(
_tmp579,sizeof(char),_get_zero_arr_size(_tmp579,53));}),_tag_dynforward(_tmp578,
sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp57F;_tmp57F.tag=0;
_tmp57F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp57E;_tmp57E.tag=0;_tmp57E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp57C[2]={& _tmp57E,& _tmp57F};Cyc_Tcutil_warn(e->loc,({const char*_tmp57D="implicit cast from %s to %s";
_tag_dynforward(_tmp57D,sizeof(char),_get_zero_arr_size(_tmp57D,28));}),
_tag_dynforward(_tmp57C,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp580=Cyc_Tcutil_compress(t);
_LL3F6: if(_tmp580 <= (void*)4)goto _LL3F8;if(*((int*)_tmp580)!= 5)goto _LL3F8;
_LL3F7: goto _LL3F9;_LL3F8: if((int)_tmp580 != 1)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA:
if(_tmp580 <= (void*)4)goto _LL3FC;if(*((int*)_tmp580)!= 6)goto _LL3FC;_LL3FB:
return 1;_LL3FC:;_LL3FD: return 0;_LL3F5:;}static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp582;
struct _RegionHandle*_tmp583;struct _tuple9 _tmp581=*env;_tmp582=_tmp581.f1;_tmp583=
_tmp581.f2;return({struct _tuple5*_tmp584=_region_malloc(_tmp583,sizeof(*_tmp584));
_tmp584->f1=x->tq;_tmp584->f2=Cyc_Tcutil_rsubstitute(_tmp583,_tmp582,(void*)x->type);
_tmp584;});}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(struct _RegionHandle*r,
struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp586;void*_tmp587;struct _tuple5 _tmp585=*
x;_tmp586=_tmp585.f1;_tmp587=_tmp585.f2;return({struct _tuple5*_tmp588=
_region_malloc(r,sizeof(*_tmp588));_tmp588->f1=_tmp586;_tmp588->f2=_tmp587;
_tmp588;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp589=t1;
struct Cyc_List_List*_tmp58A;struct Cyc_Absyn_AggrInfo _tmp58B;union Cyc_Absyn_AggrInfoU_union
_tmp58C;struct Cyc_Absyn_Aggrdecl**_tmp58D;struct Cyc_Absyn_Aggrdecl*_tmp58E;
struct Cyc_List_List*_tmp58F;void*_tmp590;struct Cyc_List_List*_tmp591;struct Cyc_Absyn_FnInfo
_tmp592;_LL3FF: if((int)_tmp589 != 0)goto _LL401;_LL400: return 0;_LL401: if(_tmp589 <= (
void*)4)goto _LL409;if(*((int*)_tmp589)!= 9)goto _LL403;_tmp58A=((struct Cyc_Absyn_TupleType_struct*)
_tmp589)->f1;_LL402: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _RegionHandle*,struct _tuple5*),struct _RegionHandle*env,struct
Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp58A);_LL403: if(*((
int*)_tmp589)!= 10)goto _LL405;_tmp58B=((struct Cyc_Absyn_AggrType_struct*)_tmp589)->f1;
_tmp58C=_tmp58B.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp589)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL405;_tmp58D=(_tmp58C.KnownAggr).f1;_tmp58E=*_tmp58D;_tmp58F=_tmp58B.targs;
_LL404: if((((void*)_tmp58E->kind == (void*)1  || _tmp58E->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp58E->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp58E->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp593=
_region_malloc(r,sizeof(*_tmp593));_tmp593->hd=({struct _tuple5*_tmp594=
_region_malloc(r,sizeof(*_tmp594));_tmp594->f1=Cyc_Absyn_empty_tqual(0);_tmp594->f2=
t1;_tmp594;});_tmp593->tl=0;_tmp593;});{struct Cyc_List_List*_tmp595=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp58E->tvs,_tmp58F);struct _tuple9 env=({struct _tuple9
_tmp596;_tmp596.f1=_tmp595;_tmp596.f2=r;_tmp596;});return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),
struct _tuple9*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp58E->impl))->fields);}_LL405:
if(*((int*)_tmp589)!= 11)goto _LL407;_tmp590=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp589)->f1;if((int)_tmp590 != 0)goto _LL407;_tmp591=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp589)->f2;_LL406: {struct _tuple9 env=({struct _tuple9 _tmp597;_tmp597.f1=0;
_tmp597.f2=r;_tmp597;});return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp591);}
_LL407: if(*((int*)_tmp589)!= 8)goto _LL409;_tmp592=((struct Cyc_Absyn_FnType_struct*)
_tmp589)->f1;_LL408: return({struct Cyc_List_List*_tmp598=_region_malloc(r,sizeof(*
_tmp598));_tmp598->hd=({struct _tuple5*_tmp599=_region_malloc(r,sizeof(*_tmp599));
_tmp599->f1=Cyc_Absyn_const_tqual(0);_tmp599->f2=t1;_tmp599;});_tmp598->tl=0;
_tmp598;});_LL409:;_LL40A: return({struct Cyc_List_List*_tmp59A=_region_malloc(r,
sizeof(*_tmp59A));_tmp59A->hd=({struct _tuple5*_tmp59B=_region_malloc(r,sizeof(*
_tmp59B));_tmp59B->f1=Cyc_Absyn_empty_tqual(0);_tmp59B->f2=t1;_tmp59B;});_tmp59A->tl=
0;_tmp59A;});_LL3FE:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,
struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*
_tmp59C=(void*)t->hd;_LL40C: if((int)_tmp59C != 16)goto _LL40E;_LL40D: goto _LL40F;
_LL40E: if((int)_tmp59C != 3)goto _LL410;_LL40F: goto _LL411;_LL410: if(_tmp59C <= (
void*)17)goto _LL412;if(*((int*)_tmp59C)!= 4)goto _LL412;_LL411: continue;_LL412:;
_LL413: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL40B:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp59E=({struct _tuple0 _tmp59D;_tmp59D.f1=t1;_tmp59D.f2=t2;_tmp59D;});void*
_tmp59F;struct Cyc_Absyn_PtrInfo _tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;
struct Cyc_Absyn_PtrAtts _tmp5A3;void*_tmp5A4;struct Cyc_Absyn_Conref*_tmp5A5;
struct Cyc_Absyn_Conref*_tmp5A6;struct Cyc_Absyn_Conref*_tmp5A7;void*_tmp5A8;
struct Cyc_Absyn_PtrInfo _tmp5A9;void*_tmp5AA;struct Cyc_Absyn_Tqual _tmp5AB;struct
Cyc_Absyn_PtrAtts _tmp5AC;void*_tmp5AD;struct Cyc_Absyn_Conref*_tmp5AE;struct Cyc_Absyn_Conref*
_tmp5AF;struct Cyc_Absyn_Conref*_tmp5B0;void*_tmp5B1;struct Cyc_Absyn_TunionInfo
_tmp5B2;union Cyc_Absyn_TunionInfoU_union _tmp5B3;struct Cyc_Absyn_Tuniondecl**
_tmp5B4;struct Cyc_Absyn_Tuniondecl*_tmp5B5;struct Cyc_List_List*_tmp5B6;struct Cyc_Core_Opt*
_tmp5B7;struct Cyc_Core_Opt _tmp5B8;void*_tmp5B9;void*_tmp5BA;struct Cyc_Absyn_TunionInfo
_tmp5BB;union Cyc_Absyn_TunionInfoU_union _tmp5BC;struct Cyc_Absyn_Tuniondecl**
_tmp5BD;struct Cyc_Absyn_Tuniondecl*_tmp5BE;struct Cyc_List_List*_tmp5BF;struct Cyc_Core_Opt*
_tmp5C0;struct Cyc_Core_Opt _tmp5C1;void*_tmp5C2;void*_tmp5C3;struct Cyc_Absyn_FnInfo
_tmp5C4;void*_tmp5C5;struct Cyc_Absyn_FnInfo _tmp5C6;_LL415: _tmp59F=_tmp59E.f1;if(
_tmp59F <= (void*)4)goto _LL417;if(*((int*)_tmp59F)!= 4)goto _LL417;_tmp5A0=((
struct Cyc_Absyn_PointerType_struct*)_tmp59F)->f1;_tmp5A1=(void*)_tmp5A0.elt_typ;
_tmp5A2=_tmp5A0.elt_tq;_tmp5A3=_tmp5A0.ptr_atts;_tmp5A4=(void*)_tmp5A3.rgn;
_tmp5A5=_tmp5A3.nullable;_tmp5A6=_tmp5A3.bounds;_tmp5A7=_tmp5A3.zero_term;
_tmp5A8=_tmp59E.f2;if(_tmp5A8 <= (void*)4)goto _LL417;if(*((int*)_tmp5A8)!= 4)goto
_LL417;_tmp5A9=((struct Cyc_Absyn_PointerType_struct*)_tmp5A8)->f1;_tmp5AA=(void*)
_tmp5A9.elt_typ;_tmp5AB=_tmp5A9.elt_tq;_tmp5AC=_tmp5A9.ptr_atts;_tmp5AD=(void*)
_tmp5AC.rgn;_tmp5AE=_tmp5AC.nullable;_tmp5AF=_tmp5AC.bounds;_tmp5B0=_tmp5AC.zero_term;
_LL416: if(_tmp5A2.real_const  && !_tmp5AB.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp5A5,_tmp5AE) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp5A5)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5AE))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5A7,
_tmp5B0) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5A7)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5B0))return 0;if(!Cyc_Tcutil_unify(_tmp5A4,_tmp5AD) && !Cyc_Tcenv_region_outlives(
te,_tmp5A4,_tmp5AD))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp5A6,_tmp5AF)){struct _tuple0 _tmp5C8=({struct _tuple0 _tmp5C7;_tmp5C7.f1=Cyc_Absyn_conref_val(
_tmp5A6);_tmp5C7.f2=Cyc_Absyn_conref_val(_tmp5AF);_tmp5C7;});void*_tmp5C9;void*
_tmp5CA;void*_tmp5CB;void*_tmp5CC;void*_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CE;void*
_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;_LL41E: _tmp5C9=_tmp5C8.f1;if(_tmp5C9 <= (void*)
2)goto _LL420;if(*((int*)_tmp5C9)!= 0)goto _LL420;_tmp5CA=_tmp5C8.f2;if((int)
_tmp5CA != 0)goto _LL420;_LL41F: goto _LL41D;_LL420: _tmp5CB=_tmp5C8.f1;if(_tmp5CB <= (
void*)2)goto _LL422;if(*((int*)_tmp5CB)!= 0)goto _LL422;_tmp5CC=_tmp5C8.f2;if((int)
_tmp5CC != 1)goto _LL422;_LL421: goto _LL41D;_LL422: _tmp5CD=_tmp5C8.f1;if(_tmp5CD <= (
void*)2)goto _LL424;if(*((int*)_tmp5CD)!= 0)goto _LL424;_tmp5CE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5CD)->f1;_tmp5CF=_tmp5C8.f2;if(_tmp5CF <= (void*)2)goto _LL424;if(*((int*)
_tmp5CF)!= 0)goto _LL424;_tmp5D0=((struct Cyc_Absyn_Upper_b_struct*)_tmp5CF)->f1;
_LL423: if(!Cyc_Evexp_lte_const_exp(_tmp5D0,_tmp5CE))return 0;goto _LL41D;_LL424:;
_LL425: return 0;_LL41D:;}return Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*
_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->hd=({struct _tuple0*_tmp5D2=
_cycalloc(sizeof(*_tmp5D2));_tmp5D2->f1=t1;_tmp5D2->f2=t2;_tmp5D2;});_tmp5D1->tl=
assume;_tmp5D1;}),_tmp5A1,_tmp5AA);_LL417: _tmp5B1=_tmp59E.f1;if(_tmp5B1 <= (void*)
4)goto _LL419;if(*((int*)_tmp5B1)!= 2)goto _LL419;_tmp5B2=((struct Cyc_Absyn_TunionType_struct*)
_tmp5B1)->f1;_tmp5B3=_tmp5B2.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp5B1)->f1).tunion_info).KnownTunion).tag != 1)goto _LL419;_tmp5B4=(_tmp5B3.KnownTunion).f1;
_tmp5B5=*_tmp5B4;_tmp5B6=_tmp5B2.targs;_tmp5B7=_tmp5B2.rgn;if(_tmp5B7 == 0)goto
_LL419;_tmp5B8=*_tmp5B7;_tmp5B9=(void*)_tmp5B8.v;_tmp5BA=_tmp59E.f2;if(_tmp5BA <= (
void*)4)goto _LL419;if(*((int*)_tmp5BA)!= 2)goto _LL419;_tmp5BB=((struct Cyc_Absyn_TunionType_struct*)
_tmp5BA)->f1;_tmp5BC=_tmp5BB.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp5BA)->f1).tunion_info).KnownTunion).tag != 1)goto _LL419;_tmp5BD=(_tmp5BC.KnownTunion).f1;
_tmp5BE=*_tmp5BD;_tmp5BF=_tmp5BB.targs;_tmp5C0=_tmp5BB.rgn;if(_tmp5C0 == 0)goto
_LL419;_tmp5C1=*_tmp5C0;_tmp5C2=(void*)_tmp5C1.v;_LL418: if(_tmp5B5 != _tmp5BE  || 
!Cyc_Tcenv_region_outlives(te,_tmp5B9,_tmp5C2))return 0;for(0;_tmp5B6 != 0  && 
_tmp5BF != 0;(_tmp5B6=_tmp5B6->tl,_tmp5BF=_tmp5BF->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp5B6->hd,(void*)_tmp5BF->hd))return 0;}if(_tmp5B6 != 0  || _tmp5BF != 0)
return 0;return 1;_LL419: _tmp5C3=_tmp59E.f1;if(_tmp5C3 <= (void*)4)goto _LL41B;if(*((
int*)_tmp5C3)!= 8)goto _LL41B;_tmp5C4=((struct Cyc_Absyn_FnType_struct*)_tmp5C3)->f1;
_tmp5C5=_tmp59E.f2;if(_tmp5C5 <= (void*)4)goto _LL41B;if(*((int*)_tmp5C5)!= 8)goto
_LL41B;_tmp5C6=((struct Cyc_Absyn_FnType_struct*)_tmp5C5)->f1;_LL41A: if(_tmp5C4.tvars
!= 0  || _tmp5C6.tvars != 0){struct Cyc_List_List*_tmp5D3=_tmp5C4.tvars;struct Cyc_List_List*
_tmp5D4=_tmp5C6.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5D3)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5D4))return 0;{
struct _RegionHandle _tmp5D5=_new_region("r");struct _RegionHandle*r=& _tmp5D5;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp5D3 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp5D3->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp5D4))->hd)){int _tmp5D6=0;_npop_handler(0);
return _tmp5D6;}inst=({struct Cyc_List_List*_tmp5D7=_region_malloc(r,sizeof(*
_tmp5D7));_tmp5D7->hd=({struct _tuple8*_tmp5D8=_region_malloc(r,sizeof(*_tmp5D8));
_tmp5D8->f1=(struct Cyc_Absyn_Tvar*)_tmp5D4->hd;_tmp5D8->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9[0]=({struct Cyc_Absyn_VarType_struct
_tmp5DA;_tmp5DA.tag=1;_tmp5DA.f1=(struct Cyc_Absyn_Tvar*)_tmp5D3->hd;_tmp5DA;});
_tmp5D9;});_tmp5D8;});_tmp5D7->tl=inst;_tmp5D7;});_tmp5D3=_tmp5D3->tl;_tmp5D4=
_tmp5D4->tl;}if(inst != 0){_tmp5C4.tvars=0;_tmp5C6.tvars=0;{int _tmp5DF=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp5DB=_cycalloc(sizeof(*
_tmp5DB));_tmp5DB[0]=({struct Cyc_Absyn_FnType_struct _tmp5DC;_tmp5DC.tag=8;
_tmp5DC.f1=_tmp5C4;_tmp5DC;});_tmp5DB;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD[0]=({struct Cyc_Absyn_FnType_struct
_tmp5DE;_tmp5DE.tag=8;_tmp5DE.f1=_tmp5C6;_tmp5DE;});_tmp5DD;}));_npop_handler(0);
return _tmp5DF;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp5C4.ret_typ,(void*)_tmp5C6.ret_typ))return 0;{struct Cyc_List_List*_tmp5E0=
_tmp5C4.args;struct Cyc_List_List*_tmp5E1=_tmp5C6.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp5E0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5E1))return 0;for(0;_tmp5E0 != 0;(_tmp5E0=_tmp5E0->tl,_tmp5E1=_tmp5E1->tl)){
struct Cyc_Absyn_Tqual _tmp5E3;void*_tmp5E4;struct _tuple3 _tmp5E2=*((struct _tuple3*)
_tmp5E0->hd);_tmp5E3=_tmp5E2.f2;_tmp5E4=_tmp5E2.f3;{struct Cyc_Absyn_Tqual _tmp5E6;
void*_tmp5E7;struct _tuple3 _tmp5E5=*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp5E1))->hd);_tmp5E6=_tmp5E5.f2;_tmp5E7=_tmp5E5.f3;if(_tmp5E6.real_const
 && !_tmp5E3.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp5E7,_tmp5E4))return
0;}}if(_tmp5C4.c_varargs != _tmp5C6.c_varargs)return 0;if(_tmp5C4.cyc_varargs != 0
 && _tmp5C6.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp5E8=*_tmp5C4.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp5E9=*_tmp5C6.cyc_varargs;if((_tmp5E9.tq).real_const
 && !(_tmp5E8.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp5E9.type,(
void*)_tmp5E8.type))return 0;}else{if(_tmp5C4.cyc_varargs != 0  || _tmp5C6.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5C4.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5C6.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp5C4.rgn_po,_tmp5C6.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp5C4.attributes,_tmp5C6.attributes))return 0;return 1;}_LL41B:;_LL41C: return 0;
_LL414:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple0 _tmp5EB=({
struct _tuple0 _tmp5EA;_tmp5EA.f1=Cyc_Tcutil_compress(t1);_tmp5EA.f2=Cyc_Tcutil_compress(
t2);_tmp5EA;});void*_tmp5EC;void*_tmp5ED;void*_tmp5EE;void*_tmp5EF;_LL427:
_tmp5EC=_tmp5EB.f1;if(_tmp5EC <= (void*)4)goto _LL429;if(*((int*)_tmp5EC)!= 5)goto
_LL429;_tmp5ED=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EC)->f2;_tmp5EE=
_tmp5EB.f2;if(_tmp5EE <= (void*)4)goto _LL429;if(*((int*)_tmp5EE)!= 5)goto _LL429;
_tmp5EF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EE)->f2;_LL428: return(
_tmp5ED == _tmp5EF  || _tmp5ED == (void*)2  && _tmp5EF == (void*)3) || _tmp5ED == (
void*)3  && _tmp5EF == (void*)2;_LL429:;_LL42A: return 0;_LL426:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct
_RegionHandle _tmp5F0=_new_region("temp");struct _RegionHandle*temp=& _tmp5F0;
_push_region(temp);{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,te,t2);for(0;tqs2 != 0;(tqs2=
tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){int _tmp5F1=0;_npop_handler(0);return
_tmp5F1;}{struct _tuple5 _tmp5F3;struct Cyc_Absyn_Tqual _tmp5F4;void*_tmp5F5;struct
_tuple5*_tmp5F2=(struct _tuple5*)tqs1->hd;_tmp5F3=*_tmp5F2;_tmp5F4=_tmp5F3.f1;
_tmp5F5=_tmp5F3.f2;{struct _tuple5 _tmp5F7;struct Cyc_Absyn_Tqual _tmp5F8;void*
_tmp5F9;struct _tuple5*_tmp5F6=(struct _tuple5*)tqs2->hd;_tmp5F7=*_tmp5F6;_tmp5F8=
_tmp5F7.f1;_tmp5F9=_tmp5F7.f2;if(_tmp5F8.real_const  && Cyc_Tcutil_subtype(te,
assume,_tmp5F5,_tmp5F9))continue;else{if(Cyc_Tcutil_unify(_tmp5F5,_tmp5F9))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp5F5,_tmp5F9))continue;else{int _tmp5FA=
0;_npop_handler(0);return _tmp5FA;}}}}}}{int _tmp5FB=1;_npop_handler(0);return
_tmp5FB;}};_pop_region(temp);}static int Cyc_Tcutil_is_char_type(void*t){void*
_tmp5FC=Cyc_Tcutil_compress(t);void*_tmp5FD;_LL42C: if(_tmp5FC <= (void*)4)goto
_LL42E;if(*((int*)_tmp5FC)!= 5)goto _LL42E;_tmp5FD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5FC)->f2;if((int)_tmp5FD != 0)goto _LL42E;_LL42D: return 1;_LL42E:;_LL42F: return
0;_LL42B:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp5FE=t2;
void*_tmp5FF;void*_tmp600;_LL431: if(_tmp5FE <= (void*)4)goto _LL435;if(*((int*)
_tmp5FE)!= 5)goto _LL433;_tmp5FF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FE)->f2;
if((int)_tmp5FF != 2)goto _LL433;_LL432: goto _LL434;_LL433: if(*((int*)_tmp5FE)!= 5)
goto _LL435;_tmp600=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FE)->f2;if((int)
_tmp600 != 3)goto _LL435;_LL434: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL430;_LL435:;_LL436: goto _LL430;_LL430:;}{void*_tmp601=t1;struct Cyc_Absyn_PtrInfo
_tmp602;void*_tmp603;struct Cyc_Absyn_Tqual _tmp604;struct Cyc_Absyn_PtrAtts _tmp605;
void*_tmp606;struct Cyc_Absyn_Conref*_tmp607;struct Cyc_Absyn_Conref*_tmp608;
struct Cyc_Absyn_Conref*_tmp609;struct Cyc_Absyn_ArrayInfo _tmp60A;void*_tmp60B;
struct Cyc_Absyn_Tqual _tmp60C;struct Cyc_Absyn_Exp*_tmp60D;struct Cyc_Absyn_Conref*
_tmp60E;struct Cyc_Absyn_Enumdecl*_tmp60F;_LL438: if(_tmp601 <= (void*)4)goto _LL440;
if(*((int*)_tmp601)!= 4)goto _LL43A;_tmp602=((struct Cyc_Absyn_PointerType_struct*)
_tmp601)->f1;_tmp603=(void*)_tmp602.elt_typ;_tmp604=_tmp602.elt_tq;_tmp605=
_tmp602.ptr_atts;_tmp606=(void*)_tmp605.rgn;_tmp607=_tmp605.nullable;_tmp608=
_tmp605.bounds;_tmp609=_tmp605.zero_term;_LL439:{void*_tmp610=t2;struct Cyc_Absyn_PtrInfo
_tmp611;void*_tmp612;struct Cyc_Absyn_Tqual _tmp613;struct Cyc_Absyn_PtrAtts _tmp614;
void*_tmp615;struct Cyc_Absyn_Conref*_tmp616;struct Cyc_Absyn_Conref*_tmp617;
struct Cyc_Absyn_Conref*_tmp618;_LL447: if(_tmp610 <= (void*)4)goto _LL449;if(*((int*)
_tmp610)!= 4)goto _LL449;_tmp611=((struct Cyc_Absyn_PointerType_struct*)_tmp610)->f1;
_tmp612=(void*)_tmp611.elt_typ;_tmp613=_tmp611.elt_tq;_tmp614=_tmp611.ptr_atts;
_tmp615=(void*)_tmp614.rgn;_tmp616=_tmp614.nullable;_tmp617=_tmp614.bounds;
_tmp618=_tmp614.zero_term;_LL448: {void*coercion=(void*)3;struct Cyc_List_List*
_tmp619=({struct Cyc_List_List*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->hd=({
struct _tuple0*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->f1=t1;_tmp629->f2=t2;
_tmp629;});_tmp628->tl=0;_tmp628;});int _tmp61A=Cyc_Tcutil_ptrsubtype(te,_tmp619,
_tmp603,_tmp612) && (!_tmp604.real_const  || _tmp613.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp609,_tmp618) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp618);int _tmp61B=_tmp61A?0:((Cyc_Tcutil_bits_only(_tmp603) && Cyc_Tcutil_is_char_type(
_tmp612)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp618)) && (_tmp613.real_const  || !_tmp604.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp608,_tmp617);if(!bounds_ok  && !_tmp61B){struct
_tuple0 _tmp61D=({struct _tuple0 _tmp61C;_tmp61C.f1=Cyc_Absyn_conref_val(_tmp608);
_tmp61C.f2=Cyc_Absyn_conref_val(_tmp617);_tmp61C;});void*_tmp61E;struct Cyc_Absyn_Exp*
_tmp61F;void*_tmp620;struct Cyc_Absyn_Exp*_tmp621;void*_tmp622;void*_tmp623;void*
_tmp624;void*_tmp625;void*_tmp626;void*_tmp627;_LL44C: _tmp61E=_tmp61D.f1;if(
_tmp61E <= (void*)2)goto _LL44E;if(*((int*)_tmp61E)!= 0)goto _LL44E;_tmp61F=((
struct Cyc_Absyn_Upper_b_struct*)_tmp61E)->f1;_tmp620=_tmp61D.f2;if(_tmp620 <= (
void*)2)goto _LL44E;if(*((int*)_tmp620)!= 0)goto _LL44E;_tmp621=((struct Cyc_Absyn_Upper_b_struct*)
_tmp620)->f1;_LL44D: if(Cyc_Evexp_lte_const_exp(_tmp621,_tmp61F))bounds_ok=1;goto
_LL44B;_LL44E: _tmp622=_tmp61D.f1;if(_tmp622 <= (void*)2)goto _LL450;if(*((int*)
_tmp622)!= 1)goto _LL450;_tmp623=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp622)->f1;_tmp624=_tmp61D.f2;if(_tmp624 <= (void*)2)goto _LL450;if(*((int*)
_tmp624)!= 1)goto _LL450;_tmp625=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp624)->f1;_LL44F: bounds_ok=Cyc_Tcutil_unify(_tmp623,_tmp625);goto _LL44B;
_LL450: _tmp626=_tmp61D.f1;if(_tmp626 <= (void*)2)goto _LL452;if(*((int*)_tmp626)!= 
1)goto _LL452;_LL451: goto _LL453;_LL452: _tmp627=_tmp61D.f2;if(_tmp627 <= (void*)2)
goto _LL454;if(*((int*)_tmp627)!= 1)goto _LL454;_LL453: bounds_ok=0;goto _LL44B;
_LL454:;_LL455: bounds_ok=1;goto _LL44B;_LL44B:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp607) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp616))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp61A  || _tmp61B)) && (Cyc_Tcutil_unify(_tmp606,_tmp615) || Cyc_Tcenv_region_outlives(
te,_tmp606,_tmp615)))return coercion;else{return(void*)0;}}}_LL449:;_LL44A: goto
_LL446;_LL446:;}return(void*)0;_LL43A: if(*((int*)_tmp601)!= 7)goto _LL43C;_tmp60A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp601)->f1;_tmp60B=(void*)_tmp60A.elt_type;
_tmp60C=_tmp60A.tq;_tmp60D=_tmp60A.num_elts;_tmp60E=_tmp60A.zero_term;_LL43B:{
void*_tmp62A=t2;struct Cyc_Absyn_ArrayInfo _tmp62B;void*_tmp62C;struct Cyc_Absyn_Tqual
_tmp62D;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Conref*_tmp62F;_LL457: if(
_tmp62A <= (void*)4)goto _LL459;if(*((int*)_tmp62A)!= 7)goto _LL459;_tmp62B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp62A)->f1;_tmp62C=(void*)_tmp62B.elt_type;
_tmp62D=_tmp62B.tq;_tmp62E=_tmp62B.num_elts;_tmp62F=_tmp62B.zero_term;_LL458: {
int okay;okay=((_tmp60D != 0  && _tmp62E != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp60E,_tmp62F)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp62E,(
struct Cyc_Absyn_Exp*)_tmp60D);return(okay  && Cyc_Tcutil_unify(_tmp60B,_tmp62C))
 && (!_tmp60C.real_const  || _tmp62D.real_const)?(void*)3:(void*)0;}_LL459:;
_LL45A: return(void*)0;_LL456:;}return(void*)0;_LL43C: if(*((int*)_tmp601)!= 12)
goto _LL43E;_tmp60F=((struct Cyc_Absyn_EnumType_struct*)_tmp601)->f2;_LL43D:{void*
_tmp630=t2;struct Cyc_Absyn_Enumdecl*_tmp631;_LL45C: if(_tmp630 <= (void*)4)goto
_LL45E;if(*((int*)_tmp630)!= 12)goto _LL45E;_tmp631=((struct Cyc_Absyn_EnumType_struct*)
_tmp630)->f2;_LL45D: if((_tmp60F->fields != 0  && _tmp631->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp60F->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp631->fields))->v))
return(void*)1;goto _LL45B;_LL45E:;_LL45F: goto _LL45B;_LL45B:;}goto _LL43F;_LL43E:
if(*((int*)_tmp601)!= 5)goto _LL440;_LL43F: goto _LL441;_LL440: if((int)_tmp601 != 1)
goto _LL442;_LL441: goto _LL443;_LL442: if(_tmp601 <= (void*)4)goto _LL444;if(*((int*)
_tmp601)!= 6)goto _LL444;_LL443: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL444:;_LL445: return(void*)0;_LL437:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp632=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp633=_cycalloc(
sizeof(*_tmp633));_tmp633[0]=({struct Cyc_Absyn_Cast_e_struct _tmp634;_tmp634.tag=
15;_tmp634.f1=(void*)t;_tmp634.f2=_tmp632;_tmp634.f3=0;_tmp634.f4=(void*)c;
_tmp634;});_tmp633;})));e->topt=({struct Cyc_Core_Opt*_tmp635=_cycalloc(sizeof(*
_tmp635));_tmp635->v=(void*)t;_tmp635;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp636=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL461: if(_tmp636 <= (void*)4)goto _LL46D;if(*((int*)_tmp636)!= 5)goto _LL463;
_LL462: goto _LL464;_LL463: if(*((int*)_tmp636)!= 12)goto _LL465;_LL464: goto _LL466;
_LL465: if(*((int*)_tmp636)!= 13)goto _LL467;_LL466: goto _LL468;_LL467: if(*((int*)
_tmp636)!= 18)goto _LL469;_LL468: goto _LL46A;_LL469: if(*((int*)_tmp636)!= 14)goto
_LL46B;_LL46A: return 1;_LL46B: if(*((int*)_tmp636)!= 0)goto _LL46D;_LL46C: return Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL46D:;
_LL46E: return 0;_LL460:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp637=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL470: if((int)_tmp637 != 1)goto _LL472;_LL471: goto
_LL473;_LL472: if(_tmp637 <= (void*)4)goto _LL474;if(*((int*)_tmp637)!= 6)goto
_LL474;_LL473: return 1;_LL474:;_LL475: return 0;_LL46F:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp638=Cyc_Tcutil_compress(t);_LL477: if(_tmp638 <= (void*)4)goto
_LL479;if(*((int*)_tmp638)!= 8)goto _LL479;_LL478: return 1;_LL479:;_LL47A: return 0;
_LL476:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp63A=({struct _tuple0 _tmp639;_tmp639.f1=t1;_tmp639.f2=t2;_tmp639;});void*
_tmp63B;int _tmp63C;void*_tmp63D;int _tmp63E;void*_tmp63F;void*_tmp640;void*
_tmp641;void*_tmp642;void*_tmp643;void*_tmp644;void*_tmp645;void*_tmp646;void*
_tmp647;void*_tmp648;void*_tmp649;void*_tmp64A;void*_tmp64B;void*_tmp64C;void*
_tmp64D;void*_tmp64E;void*_tmp64F;void*_tmp650;void*_tmp651;void*_tmp652;void*
_tmp653;void*_tmp654;void*_tmp655;void*_tmp656;void*_tmp657;void*_tmp658;void*
_tmp659;void*_tmp65A;void*_tmp65B;void*_tmp65C;_LL47C: _tmp63B=_tmp63A.f1;if(
_tmp63B <= (void*)4)goto _LL47E;if(*((int*)_tmp63B)!= 6)goto _LL47E;_tmp63C=((
struct Cyc_Absyn_DoubleType_struct*)_tmp63B)->f1;_tmp63D=_tmp63A.f2;if(_tmp63D <= (
void*)4)goto _LL47E;if(*((int*)_tmp63D)!= 6)goto _LL47E;_tmp63E=((struct Cyc_Absyn_DoubleType_struct*)
_tmp63D)->f1;_LL47D: if(_tmp63C)return t1;else{return t2;}_LL47E: _tmp63F=_tmp63A.f1;
if(_tmp63F <= (void*)4)goto _LL480;if(*((int*)_tmp63F)!= 6)goto _LL480;_LL47F:
return t1;_LL480: _tmp640=_tmp63A.f2;if(_tmp640 <= (void*)4)goto _LL482;if(*((int*)
_tmp640)!= 6)goto _LL482;_LL481: return t2;_LL482: _tmp641=_tmp63A.f1;if((int)
_tmp641 != 1)goto _LL484;_LL483: goto _LL485;_LL484: _tmp642=_tmp63A.f2;if((int)
_tmp642 != 1)goto _LL486;_LL485: return(void*)1;_LL486: _tmp643=_tmp63A.f1;if(
_tmp643 <= (void*)4)goto _LL488;if(*((int*)_tmp643)!= 5)goto _LL488;_tmp644=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp643)->f1;if((int)_tmp644 != 1)goto _LL488;
_tmp645=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp643)->f2;if((int)_tmp645 != 
4)goto _LL488;_LL487: goto _LL489;_LL488: _tmp646=_tmp63A.f2;if(_tmp646 <= (void*)4)
goto _LL48A;if(*((int*)_tmp646)!= 5)goto _LL48A;_tmp647=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp646)->f1;if((int)_tmp647 != 1)goto _LL48A;_tmp648=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp646)->f2;if((int)_tmp648 != 4)goto _LL48A;_LL489: return Cyc_Absyn_ulonglong_typ;
_LL48A: _tmp649=_tmp63A.f1;if(_tmp649 <= (void*)4)goto _LL48C;if(*((int*)_tmp649)!= 
5)goto _LL48C;_tmp64A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp649)->f2;if((
int)_tmp64A != 4)goto _LL48C;_LL48B: goto _LL48D;_LL48C: _tmp64B=_tmp63A.f2;if(
_tmp64B <= (void*)4)goto _LL48E;if(*((int*)_tmp64B)!= 5)goto _LL48E;_tmp64C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp64B)->f2;if((int)_tmp64C != 4)goto _LL48E;
_LL48D: return Cyc_Absyn_slonglong_typ;_LL48E: _tmp64D=_tmp63A.f1;if(_tmp64D <= (
void*)4)goto _LL490;if(*((int*)_tmp64D)!= 5)goto _LL490;_tmp64E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64D)->f1;if((int)_tmp64E != 1)goto _LL490;_tmp64F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64D)->f2;if((int)_tmp64F != 3)goto _LL490;_LL48F: goto _LL491;_LL490: _tmp650=
_tmp63A.f2;if(_tmp650 <= (void*)4)goto _LL492;if(*((int*)_tmp650)!= 5)goto _LL492;
_tmp651=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp650)->f1;if((int)_tmp651 != 
1)goto _LL492;_tmp652=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp650)->f2;if((
int)_tmp652 != 3)goto _LL492;_LL491: return Cyc_Absyn_ulong_typ;_LL492: _tmp653=
_tmp63A.f1;if(_tmp653 <= (void*)4)goto _LL494;if(*((int*)_tmp653)!= 5)goto _LL494;
_tmp654=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp653)->f1;if((int)_tmp654 != 
1)goto _LL494;_tmp655=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp653)->f2;if((
int)_tmp655 != 2)goto _LL494;_LL493: goto _LL495;_LL494: _tmp656=_tmp63A.f2;if(
_tmp656 <= (void*)4)goto _LL496;if(*((int*)_tmp656)!= 5)goto _LL496;_tmp657=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp656)->f1;if((int)_tmp657 != 1)goto _LL496;
_tmp658=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp656)->f2;if((int)_tmp658 != 
2)goto _LL496;_LL495: return Cyc_Absyn_uint_typ;_LL496: _tmp659=_tmp63A.f1;if(
_tmp659 <= (void*)4)goto _LL498;if(*((int*)_tmp659)!= 5)goto _LL498;_tmp65A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp659)->f2;if((int)_tmp65A != 3)goto _LL498;
_LL497: goto _LL499;_LL498: _tmp65B=_tmp63A.f2;if(_tmp65B <= (void*)4)goto _LL49A;if(*((
int*)_tmp65B)!= 5)goto _LL49A;_tmp65C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp65B)->f2;if((int)_tmp65C != 3)goto _LL49A;_LL499: return Cyc_Absyn_slong_typ;
_LL49A:;_LL49B: return Cyc_Absyn_sint_typ;_LL47B:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp65D=(void*)e->r;struct Cyc_Core_Opt*_tmp65E;
_LL49D: if(*((int*)_tmp65D)!= 4)goto _LL49F;_tmp65E=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp65D)->f2;if(_tmp65E != 0)goto _LL49F;_LL49E:({void*_tmp65F[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp660="assignment in test";_tag_dynforward(_tmp660,sizeof(
char),_get_zero_arr_size(_tmp660,19));}),_tag_dynforward(_tmp65F,sizeof(void*),0));});
goto _LL49C;_LL49F:;_LL4A0: goto _LL49C;_LL49C:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp662=({struct _tuple0 _tmp661;_tmp661.f1=c1;_tmp661.f2=c2;_tmp661;});
void*_tmp663;void*_tmp664;void*_tmp665;void*_tmp666;void*_tmp667;struct Cyc_Core_Opt*
_tmp668;struct Cyc_Core_Opt**_tmp669;void*_tmp66A;struct Cyc_Core_Opt*_tmp66B;
struct Cyc_Core_Opt**_tmp66C;void*_tmp66D;struct Cyc_Core_Opt*_tmp66E;struct Cyc_Core_Opt**
_tmp66F;void*_tmp670;void*_tmp671;void*_tmp672;void*_tmp673;void*_tmp674;void*
_tmp675;struct Cyc_Core_Opt*_tmp676;struct Cyc_Core_Opt**_tmp677;void*_tmp678;void*
_tmp679;struct Cyc_Core_Opt*_tmp67A;struct Cyc_Core_Opt**_tmp67B;void*_tmp67C;void*
_tmp67D;struct Cyc_Core_Opt*_tmp67E;struct Cyc_Core_Opt**_tmp67F;void*_tmp680;
_LL4A2: _tmp663=_tmp662.f1;if(*((int*)_tmp663)!= 0)goto _LL4A4;_tmp664=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp663)->f1;_tmp665=_tmp662.f2;if(*((int*)_tmp665)
!= 0)goto _LL4A4;_tmp666=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp665)->f1;
_LL4A3: return _tmp664 == _tmp666;_LL4A4: _tmp667=_tmp662.f2;if(*((int*)_tmp667)!= 1)
goto _LL4A6;_tmp668=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp667)->f1;_tmp669=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp667)->f1;_LL4A5:*
_tmp669=({struct Cyc_Core_Opt*_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681->v=(
void*)c1;_tmp681;});return 1;_LL4A6: _tmp66A=_tmp662.f1;if(*((int*)_tmp66A)!= 1)
goto _LL4A8;_tmp66B=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp66A)->f1;_tmp66C=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp66A)->f1;_LL4A7:*
_tmp66C=({struct Cyc_Core_Opt*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->v=(
void*)c2;_tmp682;});return 1;_LL4A8: _tmp66D=_tmp662.f1;if(*((int*)_tmp66D)!= 2)
goto _LL4AA;_tmp66E=((struct Cyc_Absyn_Less_kb_struct*)_tmp66D)->f1;_tmp66F=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp66D)->f1;_tmp670=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp66D)->f2;_tmp671=_tmp662.f2;if(*((
int*)_tmp671)!= 0)goto _LL4AA;_tmp672=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp671)->f1;_LL4A9: if(Cyc_Tcutil_kind_leq(_tmp672,_tmp670)){*_tmp66F=({struct
Cyc_Core_Opt*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->v=(void*)c2;_tmp683;});
return 1;}else{return 0;}_LL4AA: _tmp673=_tmp662.f1;if(*((int*)_tmp673)!= 0)goto
_LL4AC;_tmp674=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp673)->f1;_tmp675=
_tmp662.f2;if(*((int*)_tmp675)!= 2)goto _LL4AC;_tmp676=((struct Cyc_Absyn_Less_kb_struct*)
_tmp675)->f1;_tmp677=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp675)->f1;_tmp678=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp675)->f2;
_LL4AB: if(Cyc_Tcutil_kind_leq(_tmp674,_tmp678)){*_tmp677=({struct Cyc_Core_Opt*
_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->v=(void*)c1;_tmp684;});return 1;}
else{return 0;}_LL4AC: _tmp679=_tmp662.f1;if(*((int*)_tmp679)!= 2)goto _LL4A1;
_tmp67A=((struct Cyc_Absyn_Less_kb_struct*)_tmp679)->f1;_tmp67B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp679)->f1;_tmp67C=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp679)->f2;_tmp67D=_tmp662.f2;if(*((int*)_tmp67D)!= 2)goto _LL4A1;_tmp67E=((
struct Cyc_Absyn_Less_kb_struct*)_tmp67D)->f1;_tmp67F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp67D)->f1;_tmp680=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp67D)->f2;_LL4AD: if(Cyc_Tcutil_kind_leq(_tmp67C,_tmp680)){*_tmp67F=({struct
Cyc_Core_Opt*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->v=(void*)c1;_tmp685;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp680,_tmp67C)){*_tmp67B=({struct Cyc_Core_Opt*
_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->v=(void*)c2;_tmp686;});return 1;}
else{return 0;}}_LL4A1:;}}static int Cyc_Tcutil_tvar_id_counter=0;int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dynforward_ptr
s=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp68B;_tmp68B.tag=1;_tmp68B.f1=(
unsigned long)i;{void*_tmp689[1]={& _tmp68B};Cyc_aprintf(({const char*_tmp68A="#%d";
_tag_dynforward(_tmp68A,sizeof(char),_get_zero_arr_size(_tmp68A,4));}),
_tag_dynforward(_tmp689,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687->name=({struct _dynforward_ptr*
_tmp688=_cycalloc(sizeof(struct _dynforward_ptr)* 1);_tmp688[0]=s;_tmp688;});
_tmp687->identity=- 1;_tmp687->kind=(void*)k;_tmp687;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dynforward_ptr _tmp68C=*t->name;return*((const char*)
_check_dynforward_subscript(_tmp68C,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp68F;_tmp68F.tag=0;
_tmp68F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*t->name);{void*
_tmp68D[1]={& _tmp68F};Cyc_printf(({const char*_tmp68E="%s";_tag_dynforward(
_tmp68E,sizeof(char),_get_zero_arr_size(_tmp68E,3));}),_tag_dynforward(_tmp68D,
sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(t))return;{struct
_dynforward_ptr _tmp690=Cyc_strconcat(({const char*_tmp695="`";_tag_dynforward(
_tmp695,sizeof(char),_get_zero_arr_size(_tmp695,2));}),(struct _dynforward_ptr)*t->name);({
struct _dynforward_ptr _tmp691=_dynforward_ptr_plus(_tmp690,sizeof(char),1);char
_tmp692=*((char*)_check_dynforward_subscript(_tmp691,sizeof(char),0));char
_tmp693='t';if(_get_dynforward_size(_tmp691,sizeof(char))== 1  && (_tmp692 == '\000'
 && _tmp693 != '\000'))_throw_arraybounds();*((char*)_tmp691.curr)=_tmp693;});t->name=({
struct _dynforward_ptr*_tmp694=_cycalloc(sizeof(struct _dynforward_ptr)* 1);
_tmp694[0]=(struct _dynforward_ptr)_tmp690;_tmp694;});}}struct _tuple14{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(
struct _tuple14*x){return({struct _tuple3*_tmp696=_cycalloc(sizeof(*_tmp696));
_tmp696->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp697=_cycalloc(sizeof(*
_tmp697));_tmp697->v=(*x).f1;_tmp697;});_tmp696->f2=(*x).f2;_tmp696->f3=(*x).f3;
_tmp696;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp698=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp698=({
struct Cyc_List_List*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->hd=(void*)((
void*)atts->hd);_tmp699->tl=_tmp698;_tmp699;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_FnType_struct
_tmp69B;_tmp69B.tag=8;_tmp69B.f1=({struct Cyc_Absyn_FnInfo _tmp69C;_tmp69C.tvars=
fd->tvs;_tmp69C.effect=fd->effect;_tmp69C.ret_typ=(void*)((void*)fd->ret_type);
_tmp69C.args=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp69C.c_varargs=
fd->c_varargs;_tmp69C.cyc_varargs=fd->cyc_varargs;_tmp69C.rgn_po=fd->rgn_po;
_tmp69C.attributes=_tmp698;_tmp69C;});_tmp69B;});_tmp69A;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple15{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple5*t){return(*t).f2;}static
struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t){return({struct _tuple5*
_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->f1=(*pr).f1;_tmp69D->f2=t;_tmp69D;});}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple17{
struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple3*y){return({struct _tuple17*_tmp69E=_region_malloc(
rgn,sizeof(*_tmp69E));_tmp69E->f1=({struct _tuple16*_tmp69F=_region_malloc(rgn,
sizeof(*_tmp69F));_tmp69F->f1=(*y).f1;_tmp69F->f2=(*y).f2;_tmp69F;});_tmp69E->f2=(*
y).f3;_tmp69E;});}static struct _tuple3*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp6A1;void*_tmp6A2;struct _tuple17 _tmp6A0=*w;_tmp6A1=_tmp6A0.f1;
_tmp6A2=_tmp6A0.f2;{struct Cyc_Core_Opt*_tmp6A4;struct Cyc_Absyn_Tqual _tmp6A5;
struct _tuple16 _tmp6A3=*_tmp6A1;_tmp6A4=_tmp6A3.f1;_tmp6A5=_tmp6A3.f2;return({
struct _tuple3*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->f1=_tmp6A4;_tmp6A6->f2=
_tmp6A5;_tmp6A6->f3=_tmp6A2;_tmp6A6;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp6A7=
_cycalloc(sizeof(*_tmp6A7));_tmp6A7->name=f->name;_tmp6A7->tq=f->tq;_tmp6A7->type=(
void*)t;_tmp6A7->width=f->width;_tmp6A7->attributes=f->attributes;_tmp6A7;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp6A8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp6A9;struct Cyc_Absyn_AggrInfo _tmp6AA;union Cyc_Absyn_AggrInfoU_union _tmp6AB;
struct Cyc_List_List*_tmp6AC;struct Cyc_Absyn_TunionInfo _tmp6AD;union Cyc_Absyn_TunionInfoU_union
_tmp6AE;struct Cyc_List_List*_tmp6AF;struct Cyc_Core_Opt*_tmp6B0;struct Cyc_Absyn_TunionFieldInfo
_tmp6B1;union Cyc_Absyn_TunionFieldInfoU_union _tmp6B2;struct Cyc_List_List*_tmp6B3;
struct _tuple2*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct Cyc_Absyn_Typedefdecl*
_tmp6B6;void**_tmp6B7;struct Cyc_Absyn_ArrayInfo _tmp6B8;void*_tmp6B9;struct Cyc_Absyn_Tqual
_tmp6BA;struct Cyc_Absyn_Exp*_tmp6BB;struct Cyc_Absyn_Conref*_tmp6BC;struct Cyc_Position_Segment*
_tmp6BD;struct Cyc_Absyn_PtrInfo _tmp6BE;void*_tmp6BF;struct Cyc_Absyn_Tqual _tmp6C0;
struct Cyc_Absyn_PtrAtts _tmp6C1;void*_tmp6C2;struct Cyc_Absyn_Conref*_tmp6C3;
struct Cyc_Absyn_Conref*_tmp6C4;struct Cyc_Absyn_Conref*_tmp6C5;struct Cyc_Absyn_FnInfo
_tmp6C6;struct Cyc_List_List*_tmp6C7;struct Cyc_Core_Opt*_tmp6C8;void*_tmp6C9;
struct Cyc_List_List*_tmp6CA;int _tmp6CB;struct Cyc_Absyn_VarargInfo*_tmp6CC;struct
Cyc_List_List*_tmp6CD;struct Cyc_List_List*_tmp6CE;struct Cyc_List_List*_tmp6CF;
void*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_Core_Opt*_tmp6D2;void*_tmp6D3;
void*_tmp6D4;void*_tmp6D5;void*_tmp6D6;void*_tmp6D7;void*_tmp6D8;void*_tmp6D9;
struct Cyc_List_List*_tmp6DA;_LL4AF: if(_tmp6A8 <= (void*)4)goto _LL4D3;if(*((int*)
_tmp6A8)!= 1)goto _LL4B1;_tmp6A9=((struct Cyc_Absyn_VarType_struct*)_tmp6A8)->f1;
_LL4B0: {struct _handler_cons _tmp6DB;_push_handler(& _tmp6DB);{int _tmp6DD=0;if(
setjmp(_tmp6DB.handler))_tmp6DD=1;if(!_tmp6DD){{void*_tmp6DE=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6A9);_npop_handler(0);return
_tmp6DE;};_pop_handler();}else{void*_tmp6DC=(void*)_exn_thrown;void*_tmp6E0=
_tmp6DC;_LL4E6: if(_tmp6E0 != Cyc_Core_Not_found)goto _LL4E8;_LL4E7: return t;_LL4E8:;
_LL4E9:(void)_throw(_tmp6E0);_LL4E5:;}}}_LL4B1: if(*((int*)_tmp6A8)!= 10)goto
_LL4B3;_tmp6AA=((struct Cyc_Absyn_AggrType_struct*)_tmp6A8)->f1;_tmp6AB=_tmp6AA.aggr_info;
_tmp6AC=_tmp6AA.targs;_LL4B2: {struct Cyc_List_List*_tmp6E1=Cyc_Tcutil_substs(rgn,
inst,_tmp6AC);return _tmp6E1 == _tmp6AC?t:(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_Absyn_AggrType_struct
_tmp6E3;_tmp6E3.tag=10;_tmp6E3.f1=({struct Cyc_Absyn_AggrInfo _tmp6E4;_tmp6E4.aggr_info=
_tmp6AB;_tmp6E4.targs=_tmp6E1;_tmp6E4;});_tmp6E3;});_tmp6E2;});}_LL4B3: if(*((int*)
_tmp6A8)!= 2)goto _LL4B5;_tmp6AD=((struct Cyc_Absyn_TunionType_struct*)_tmp6A8)->f1;
_tmp6AE=_tmp6AD.tunion_info;_tmp6AF=_tmp6AD.targs;_tmp6B0=_tmp6AD.rgn;_LL4B4: {
struct Cyc_List_List*_tmp6E5=Cyc_Tcutil_substs(rgn,inst,_tmp6AF);struct Cyc_Core_Opt*
new_r;if((unsigned int)_tmp6B0){void*_tmp6E6=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6B0->v);if(_tmp6E6 == (void*)_tmp6B0->v)new_r=_tmp6B0;else{new_r=({
struct Cyc_Core_Opt*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->v=(void*)_tmp6E6;
_tmp6E7;});}}else{new_r=_tmp6B0;}return _tmp6E5 == _tmp6AF  && new_r == _tmp6B0?t:(
void*)({struct Cyc_Absyn_TunionType_struct*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));
_tmp6E8[0]=({struct Cyc_Absyn_TunionType_struct _tmp6E9;_tmp6E9.tag=2;_tmp6E9.f1=({
struct Cyc_Absyn_TunionInfo _tmp6EA;_tmp6EA.tunion_info=_tmp6AE;_tmp6EA.targs=
_tmp6E5;_tmp6EA.rgn=new_r;_tmp6EA;});_tmp6E9;});_tmp6E8;});}_LL4B5: if(*((int*)
_tmp6A8)!= 3)goto _LL4B7;_tmp6B1=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6A8)->f1;_tmp6B2=_tmp6B1.field_info;_tmp6B3=_tmp6B1.targs;_LL4B6: {struct Cyc_List_List*
_tmp6EB=Cyc_Tcutil_substs(rgn,inst,_tmp6B3);return _tmp6EB == _tmp6B3?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp6EC=_cycalloc(sizeof(*_tmp6EC));
_tmp6EC[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp6ED;_tmp6ED.tag=3;_tmp6ED.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp6EE;_tmp6EE.field_info=_tmp6B2;_tmp6EE.targs=
_tmp6EB;_tmp6EE;});_tmp6ED;});_tmp6EC;});}_LL4B7: if(*((int*)_tmp6A8)!= 17)goto
_LL4B9;_tmp6B4=((struct Cyc_Absyn_TypedefType_struct*)_tmp6A8)->f1;_tmp6B5=((
struct Cyc_Absyn_TypedefType_struct*)_tmp6A8)->f2;_tmp6B6=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6A8)->f3;_tmp6B7=((struct Cyc_Absyn_TypedefType_struct*)_tmp6A8)->f4;_LL4B8: {
struct Cyc_List_List*_tmp6EF=Cyc_Tcutil_substs(rgn,inst,_tmp6B5);return _tmp6EF == 
_tmp6B5?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp6F0=_cycalloc(sizeof(*
_tmp6F0));_tmp6F0[0]=({struct Cyc_Absyn_TypedefType_struct _tmp6F1;_tmp6F1.tag=17;
_tmp6F1.f1=_tmp6B4;_tmp6F1.f2=_tmp6EF;_tmp6F1.f3=_tmp6B6;_tmp6F1.f4=_tmp6B7;
_tmp6F1;});_tmp6F0;});}_LL4B9: if(*((int*)_tmp6A8)!= 7)goto _LL4BB;_tmp6B8=((
struct Cyc_Absyn_ArrayType_struct*)_tmp6A8)->f1;_tmp6B9=(void*)_tmp6B8.elt_type;
_tmp6BA=_tmp6B8.tq;_tmp6BB=_tmp6B8.num_elts;_tmp6BC=_tmp6B8.zero_term;_tmp6BD=
_tmp6B8.zt_loc;_LL4BA: {void*_tmp6F2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B9);
return _tmp6F2 == _tmp6B9?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp6F3=
_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({struct Cyc_Absyn_ArrayType_struct _tmp6F4;
_tmp6F4.tag=7;_tmp6F4.f1=({struct Cyc_Absyn_ArrayInfo _tmp6F5;_tmp6F5.elt_type=(
void*)_tmp6F2;_tmp6F5.tq=_tmp6BA;_tmp6F5.num_elts=_tmp6BB;_tmp6F5.zero_term=
_tmp6BC;_tmp6F5.zt_loc=_tmp6BD;_tmp6F5;});_tmp6F4;});_tmp6F3;});}_LL4BB: if(*((
int*)_tmp6A8)!= 4)goto _LL4BD;_tmp6BE=((struct Cyc_Absyn_PointerType_struct*)
_tmp6A8)->f1;_tmp6BF=(void*)_tmp6BE.elt_typ;_tmp6C0=_tmp6BE.elt_tq;_tmp6C1=
_tmp6BE.ptr_atts;_tmp6C2=(void*)_tmp6C1.rgn;_tmp6C3=_tmp6C1.nullable;_tmp6C4=
_tmp6C1.bounds;_tmp6C5=_tmp6C1.zero_term;_LL4BC: {void*_tmp6F6=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6BF);void*_tmp6F7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C2);struct
Cyc_Absyn_Conref*_tmp6F8=_tmp6C4;{union Cyc_Absyn_Constraint_union _tmp6F9=(Cyc_Absyn_compress_conref(
_tmp6C4))->v;void*_tmp6FA;void*_tmp6FB;_LL4EB: if((_tmp6F9.Eq_constr).tag != 0)
goto _LL4ED;_tmp6FA=(_tmp6F9.Eq_constr).f1;if(_tmp6FA <= (void*)2)goto _LL4ED;if(*((
int*)_tmp6FA)!= 1)goto _LL4ED;_tmp6FB=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp6FA)->f1;_LL4EC: {void*_tmp6FC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FB);if(
_tmp6FB != _tmp6FC)_tmp6F8=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*
_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp6FE;_tmp6FE.tag=1;_tmp6FE.f1=(void*)_tmp6FC;_tmp6FE;});_tmp6FD;}));goto
_LL4EA;}_LL4ED:;_LL4EE: goto _LL4EA;_LL4EA:;}if((_tmp6F6 == _tmp6BF  && _tmp6F7 == 
_tmp6C2) && _tmp6F8 == _tmp6C4)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_PointerType_struct
_tmp700;_tmp700.tag=4;_tmp700.f1=({struct Cyc_Absyn_PtrInfo _tmp701;_tmp701.elt_typ=(
void*)_tmp6F6;_tmp701.elt_tq=_tmp6C0;_tmp701.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp702;_tmp702.rgn=(void*)_tmp6F7;_tmp702.nullable=_tmp6C3;_tmp702.bounds=
_tmp6F8;_tmp702.zero_term=_tmp6C5;_tmp702.ptrloc=0;_tmp702;});_tmp701;});_tmp700;});
_tmp6FF;});}_LL4BD: if(*((int*)_tmp6A8)!= 8)goto _LL4BF;_tmp6C6=((struct Cyc_Absyn_FnType_struct*)
_tmp6A8)->f1;_tmp6C7=_tmp6C6.tvars;_tmp6C8=_tmp6C6.effect;_tmp6C9=(void*)_tmp6C6.ret_typ;
_tmp6CA=_tmp6C6.args;_tmp6CB=_tmp6C6.c_varargs;_tmp6CC=_tmp6C6.cyc_varargs;
_tmp6CD=_tmp6C6.rgn_po;_tmp6CE=_tmp6C6.attributes;_LL4BE:{struct Cyc_List_List*
_tmp703=_tmp6C7;for(0;_tmp703 != 0;_tmp703=_tmp703->tl){inst=({struct Cyc_List_List*
_tmp704=_region_malloc(rgn,sizeof(*_tmp704));_tmp704->hd=({struct _tuple8*_tmp705=
_region_malloc(rgn,sizeof(*_tmp705));_tmp705->f1=(struct Cyc_Absyn_Tvar*)_tmp703->hd;
_tmp705->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp706=_cycalloc(sizeof(*
_tmp706));_tmp706[0]=({struct Cyc_Absyn_VarType_struct _tmp707;_tmp707.tag=1;
_tmp707.f1=(struct Cyc_Absyn_Tvar*)_tmp703->hd;_tmp707;});_tmp706;});_tmp705;});
_tmp704->tl=inst;_tmp704;});}}{struct Cyc_List_List*_tmp709;struct Cyc_List_List*
_tmp70A;struct _tuple1 _tmp708=((struct _tuple1(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple3*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp6CA));_tmp709=_tmp708.f1;_tmp70A=_tmp708.f2;{struct Cyc_List_List*_tmp70B=
Cyc_Tcutil_substs(rgn,inst,_tmp70A);struct Cyc_List_List*_tmp70C=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp709,_tmp70B));struct Cyc_Core_Opt*
eff2;if(_tmp6C8 == 0)eff2=0;else{void*_tmp70D=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6C8->v);if(_tmp70D == (void*)_tmp6C8->v)eff2=_tmp6C8;else{eff2=({struct
Cyc_Core_Opt*_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->v=(void*)_tmp70D;
_tmp70E;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6CC == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp710;struct Cyc_Absyn_Tqual _tmp711;void*
_tmp712;int _tmp713;struct Cyc_Absyn_VarargInfo _tmp70F=*_tmp6CC;_tmp710=_tmp70F.name;
_tmp711=_tmp70F.tq;_tmp712=(void*)_tmp70F.type;_tmp713=_tmp70F.inject;{void*
_tmp714=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp712);if(_tmp714 == _tmp712)
cyc_varargs2=_tmp6CC;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp715=
_cycalloc(sizeof(*_tmp715));_tmp715->name=_tmp710;_tmp715->tq=_tmp711;_tmp715->type=(
void*)_tmp714;_tmp715->inject=_tmp713;_tmp715;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp717;struct Cyc_List_List*_tmp718;struct _tuple1 _tmp716=Cyc_List_rsplit(
rgn,rgn,_tmp6CD);_tmp717=_tmp716.f1;_tmp718=_tmp716.f2;{struct Cyc_List_List*
_tmp719=Cyc_Tcutil_substs(rgn,inst,_tmp717);struct Cyc_List_List*_tmp71A=Cyc_Tcutil_substs(
rgn,inst,_tmp718);if(_tmp719 == _tmp717  && _tmp71A == _tmp718)rgn_po2=_tmp6CD;
else{rgn_po2=Cyc_List_zip(_tmp719,_tmp71A);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B[0]=({struct Cyc_Absyn_FnType_struct
_tmp71C;_tmp71C.tag=8;_tmp71C.f1=({struct Cyc_Absyn_FnInfo _tmp71D;_tmp71D.tvars=
_tmp6C7;_tmp71D.effect=eff2;_tmp71D.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp6C9);_tmp71D.args=_tmp70C;_tmp71D.c_varargs=_tmp6CB;_tmp71D.cyc_varargs=
cyc_varargs2;_tmp71D.rgn_po=rgn_po2;_tmp71D.attributes=_tmp6CE;_tmp71D;});
_tmp71C;});_tmp71B;});}}}}}_LL4BF: if(*((int*)_tmp6A8)!= 9)goto _LL4C1;_tmp6CF=((
struct Cyc_Absyn_TupleType_struct*)_tmp6A8)->f1;_LL4C0: {struct Cyc_List_List*
_tmp71E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6CF);struct Cyc_List_List*
_tmp71F=Cyc_Tcutil_substs(rgn,inst,_tmp71E);if(_tmp71F == _tmp71E)return t;{struct
Cyc_List_List*_tmp720=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6CF,_tmp71F);return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp721=
_cycalloc(sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Absyn_TupleType_struct _tmp722;
_tmp722.tag=9;_tmp722.f1=_tmp720;_tmp722;});_tmp721;});}}_LL4C1: if(*((int*)
_tmp6A8)!= 11)goto _LL4C3;_tmp6D0=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6A8)->f1;_tmp6D1=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6A8)->f2;_LL4C2: {
struct Cyc_List_List*_tmp723=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6D1);struct Cyc_List_List*_tmp724=Cyc_Tcutil_substs(rgn,inst,_tmp723);if(
_tmp724 == _tmp723)return t;{struct Cyc_List_List*_tmp725=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6D1,
_tmp724);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp726=_cycalloc(
sizeof(*_tmp726));_tmp726[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp727;
_tmp727.tag=11;_tmp727.f1=(void*)_tmp6D0;_tmp727.f2=_tmp725;_tmp727;});_tmp726;});}}
_LL4C3: if(*((int*)_tmp6A8)!= 0)goto _LL4C5;_tmp6D2=((struct Cyc_Absyn_Evar_struct*)
_tmp6A8)->f2;_LL4C4: if(_tmp6D2 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6D2->v);else{return t;}_LL4C5: if(*((int*)_tmp6A8)!= 15)goto _LL4C7;_tmp6D3=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp6A8)->f1;_LL4C6: {void*_tmp728=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D3);return _tmp728 == _tmp6D3?t:(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp72A;_tmp72A.tag=15;_tmp72A.f1=(void*)
_tmp728;_tmp72A;});_tmp729;});}_LL4C7: if(*((int*)_tmp6A8)!= 16)goto _LL4C9;
_tmp6D4=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp6A8)->f1;_tmp6D5=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp6A8)->f2;_LL4C8: {void*_tmp72B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6D4);void*_tmp72C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D5);return
_tmp72B == _tmp6D4  && _tmp72C == _tmp6D5?t:(void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp72E;_tmp72E.tag=16;_tmp72E.f1=(void*)_tmp72B;_tmp72E.f2=(void*)_tmp72C;
_tmp72E;});_tmp72D;});}_LL4C9: if(*((int*)_tmp6A8)!= 14)goto _LL4CB;_tmp6D6=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp6A8)->f1;_LL4CA: {void*_tmp72F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6D6);return _tmp72F == _tmp6D6?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp731;_tmp731.tag=14;_tmp731.f1=(void*)_tmp72F;_tmp731;});_tmp730;});}_LL4CB:
if(*((int*)_tmp6A8)!= 18)goto _LL4CD;_tmp6D7=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6A8)->f1;_LL4CC: {void*_tmp732=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D7);
return _tmp732 == _tmp6D7?t:(void*)({struct Cyc_Absyn_TagType_struct*_tmp733=
_cycalloc(sizeof(*_tmp733));_tmp733[0]=({struct Cyc_Absyn_TagType_struct _tmp734;
_tmp734.tag=18;_tmp734.f1=(void*)_tmp732;_tmp734;});_tmp733;});}_LL4CD: if(*((int*)
_tmp6A8)!= 19)goto _LL4CF;_LL4CE: goto _LL4D0;_LL4CF: if(*((int*)_tmp6A8)!= 12)goto
_LL4D1;_LL4D0: goto _LL4D2;_LL4D1: if(*((int*)_tmp6A8)!= 13)goto _LL4D3;_LL4D2: goto
_LL4D4;_LL4D3: if((int)_tmp6A8 != 0)goto _LL4D5;_LL4D4: goto _LL4D6;_LL4D5: if(_tmp6A8
<= (void*)4)goto _LL4D7;if(*((int*)_tmp6A8)!= 5)goto _LL4D7;_LL4D6: goto _LL4D8;
_LL4D7: if((int)_tmp6A8 != 1)goto _LL4D9;_LL4D8: goto _LL4DA;_LL4D9: if(_tmp6A8 <= (
void*)4)goto _LL4DB;if(*((int*)_tmp6A8)!= 6)goto _LL4DB;_LL4DA: goto _LL4DC;_LL4DB:
if((int)_tmp6A8 != 3)goto _LL4DD;_LL4DC: goto _LL4DE;_LL4DD: if((int)_tmp6A8 != 2)goto
_LL4DF;_LL4DE: return t;_LL4DF: if(_tmp6A8 <= (void*)4)goto _LL4E1;if(*((int*)_tmp6A8)
!= 22)goto _LL4E1;_tmp6D8=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6A8)->f1;
_LL4E0: {void*_tmp735=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D8);return _tmp735 == 
_tmp6D8?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp736=_cycalloc(sizeof(*
_tmp736));_tmp736[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp737;_tmp737.tag=22;
_tmp737.f1=(void*)_tmp735;_tmp737;});_tmp736;});}_LL4E1: if(_tmp6A8 <= (void*)4)
goto _LL4E3;if(*((int*)_tmp6A8)!= 20)goto _LL4E3;_tmp6D9=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6A8)->f1;_LL4E2: {void*_tmp738=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D9);
return _tmp738 == _tmp6D9?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp739=
_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Absyn_AccessEff_struct _tmp73A;
_tmp73A.tag=20;_tmp73A.f1=(void*)_tmp738;_tmp73A;});_tmp739;});}_LL4E3: if(
_tmp6A8 <= (void*)4)goto _LL4AE;if(*((int*)_tmp6A8)!= 21)goto _LL4AE;_tmp6DA=((
struct Cyc_Absyn_JoinEff_struct*)_tmp6A8)->f1;_LL4E4: {struct Cyc_List_List*
_tmp73B=Cyc_Tcutil_substs(rgn,inst,_tmp6DA);return _tmp73B == _tmp6DA?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp73C=_cycalloc(sizeof(*_tmp73C));_tmp73C[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp73D;_tmp73D.tag=21;_tmp73D.f1=_tmp73B;_tmp73D;});
_tmp73C;});}_LL4AE:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp73E=(void*)ts->hd;struct Cyc_List_List*_tmp73F=ts->tl;void*
_tmp740=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp73E);struct Cyc_List_List*_tmp741=Cyc_Tcutil_substs(
rgn,inst,_tmp73F);if(_tmp73E == _tmp740  && _tmp73F == _tmp741)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp742=
_cycalloc(sizeof(*_tmp742));_tmp742->hd=(void*)_tmp740;_tmp742->tl=_tmp741;
_tmp742;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmp743=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv));return({struct _tuple8*_tmp744=_cycalloc(sizeof(*
_tmp744));_tmp744->f1=tv;_tmp744->f2=Cyc_Absyn_new_evar(_tmp743,({struct Cyc_Core_Opt*
_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->v=s;_tmp745;}));_tmp744;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp747;struct Cyc_List_List*_tmp748;struct _RegionHandle*_tmp749;
struct _tuple9*_tmp746=env;_tmp747=*_tmp746;_tmp748=_tmp747.f1;_tmp749=_tmp747.f2;{
struct Cyc_Core_Opt*_tmp74A=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
return({struct _tuple8*_tmp74B=_region_malloc(_tmp749,sizeof(*_tmp74B));_tmp74B->f1=
tv;_tmp74B->f2=Cyc_Absyn_new_evar(_tmp74A,({struct Cyc_Core_Opt*_tmp74C=_cycalloc(
sizeof(*_tmp74C));_tmp74C->v=_tmp748;_tmp74C;}));_tmp74B;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp751;_tmp751.tag=0;_tmp751.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k2));{
struct Cyc_String_pa_struct _tmp750;_tmp750.tag=0;_tmp750.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k1));{
struct Cyc_String_pa_struct _tmp74F;_tmp74F.tag=0;_tmp74F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*tv->name);{void*_tmp74D[3]={& _tmp74F,&
_tmp750,& _tmp751};Cyc_Tcutil_terr(loc,({const char*_tmp74E="type variable %s is used with inconsistent kinds %s and %s";
_tag_dynforward(_tmp74E,sizeof(char),_get_zero_arr_size(_tmp74E,59));}),
_tag_dynforward(_tmp74D,sizeof(void*),3));}}}});if(tv->identity == - 1)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity)({void*_tmp752[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp753="same type variable has different identity!";
_tag_dynforward(_tmp753,sizeof(char),_get_zero_arr_size(_tmp753,43));}),
_tag_dynforward(_tmp752,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754->hd=tv;
_tmp754->tl=tvs;_tmp754;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)({void*
_tmp755[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp756="fast_add_free_tvar: bad identity in tv";_tag_dynforward(
_tmp756,sizeof(char),_get_zero_arr_size(_tmp756,39));}),_tag_dynforward(_tmp755,
sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp757=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp757->identity
== - 1)({void*_tmp758[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp759="fast_add_free_tvar: bad identity in tvs2";
_tag_dynforward(_tmp759,sizeof(char),_get_zero_arr_size(_tmp759,41));}),
_tag_dynforward(_tmp758,sizeof(void*),0));});if(_tmp757->identity == tv->identity)
return tvs;}}return({struct Cyc_List_List*_tmp75A=_cycalloc(sizeof(*_tmp75A));
_tmp75A->hd=tv;_tmp75A->tl=tvs;_tmp75A;});}struct _tuple18{struct Cyc_Absyn_Tvar*
f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity
== - 1)({void*_tmp75B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp75C="fast_add_free_tvar_bool: bad identity in tv";
_tag_dynforward(_tmp75C,sizeof(char),_get_zero_arr_size(_tmp75C,44));}),
_tag_dynforward(_tmp75B,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;
tvs2 != 0;tvs2=tvs2->tl){struct _tuple18 _tmp75E;struct Cyc_Absyn_Tvar*_tmp75F;int
_tmp760;int*_tmp761;struct _tuple18*_tmp75D=(struct _tuple18*)tvs2->hd;_tmp75E=*
_tmp75D;_tmp75F=_tmp75E.f1;_tmp760=_tmp75E.f2;_tmp761=(int*)&(*_tmp75D).f2;if(
_tmp75F->identity == - 1)({void*_tmp762[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp763="fast_add_free_tvar_bool: bad identity in tvs2";
_tag_dynforward(_tmp763,sizeof(char),_get_zero_arr_size(_tmp763,46));}),
_tag_dynforward(_tmp762,sizeof(void*),0));});if(_tmp75F->identity == tv->identity){*
_tmp761=*_tmp761  || b;return tvs;}}}return({struct Cyc_List_List*_tmp764=
_region_malloc(r,sizeof(*_tmp764));_tmp764->hd=({struct _tuple18*_tmp765=
_region_malloc(r,sizeof(*_tmp765));_tmp765->f1=tv;_tmp765->f2=b;_tmp765;});
_tmp764->tl=tvs;_tmp764;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)({struct
Cyc_String_pa_struct _tmp768;_tmp768.tag=0;_tmp768.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Tcutil_tvar2string(tv));{void*_tmp766[1]={& _tmp768};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp767="bound tvar id for %s is NULL";_tag_dynforward(_tmp767,sizeof(
char),_get_zero_arr_size(_tmp767,29));}),_tag_dynforward(_tmp766,sizeof(void*),1));}});
return({struct Cyc_List_List*_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769->hd=tv;
_tmp769->tl=tvs;_tmp769;});}struct _tuple19{void*f1;int f2;};static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp76A=Cyc_Tcutil_compress(e);int _tmp76B;_LL4F0: if(_tmp76A <= (void*)4)
goto _LL4F2;if(*((int*)_tmp76A)!= 0)goto _LL4F2;_tmp76B=((struct Cyc_Absyn_Evar_struct*)
_tmp76A)->f3;_LL4F1:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple19 _tmp76D;void*_tmp76E;int _tmp76F;int*_tmp770;struct _tuple19*_tmp76C=(
struct _tuple19*)es2->hd;_tmp76D=*_tmp76C;_tmp76E=_tmp76D.f1;_tmp76F=_tmp76D.f2;
_tmp770=(int*)&(*_tmp76C).f2;{void*_tmp771=Cyc_Tcutil_compress(_tmp76E);int
_tmp772;_LL4F5: if(_tmp771 <= (void*)4)goto _LL4F7;if(*((int*)_tmp771)!= 0)goto
_LL4F7;_tmp772=((struct Cyc_Absyn_Evar_struct*)_tmp771)->f3;_LL4F6: if(_tmp76B == 
_tmp772){if(b != *_tmp770)*_tmp770=1;return es;}goto _LL4F4;_LL4F7:;_LL4F8: goto
_LL4F4;_LL4F4:;}}}return({struct Cyc_List_List*_tmp773=_region_malloc(r,sizeof(*
_tmp773));_tmp773->hd=({struct _tuple19*_tmp774=_region_malloc(r,sizeof(*_tmp774));
_tmp774->f1=e;_tmp774->f2=b;_tmp774;});_tmp773->tl=es;_tmp773;});_LL4F2:;_LL4F3:
return es;_LL4EF:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct
_RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*
r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b
!= 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)
b->hd)->identity){present=1;break;}}}if(!present)r=({struct Cyc_List_List*_tmp775=
_region_malloc(rgn,sizeof(*_tmp775));_tmp775->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;
_tmp775->tl=r;_tmp775;});}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
r);return r;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*
res=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp777;int _tmp778;struct
_tuple18 _tmp776=*((struct _tuple18*)tvs->hd);_tmp777=_tmp776.f1;_tmp778=_tmp776.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmp777->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present)res=({
struct Cyc_List_List*_tmp779=_region_malloc(r,sizeof(*_tmp779));_tmp779->hd=(
struct _tuple18*)tvs->hd;_tmp779->tl=res;_tmp779;});}}res=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _dynforward_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp77C;_tmp77C.tag=
0;_tmp77C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp77A[1]={&
_tmp77C};Cyc_Tcutil_terr(loc,({const char*_tmp77B="bitfield %s does not have constant width";
_tag_dynforward(_tmp77B,sizeof(char),_get_zero_arr_size(_tmp77B,41));}),
_tag_dynforward(_tmp77A,sizeof(void*),1));}});else{unsigned int _tmp77E;int
_tmp77F;struct _tuple7 _tmp77D=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
width);_tmp77E=_tmp77D.f1;_tmp77F=_tmp77D.f2;if(!_tmp77F)({void*_tmp780[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp781="bitfield width cannot use sizeof or offsetof";
_tag_dynforward(_tmp781,sizeof(char),_get_zero_arr_size(_tmp781,45));}),
_tag_dynforward(_tmp780,sizeof(void*),0));});w=_tmp77E;}{void*_tmp782=Cyc_Tcutil_compress(
field_typ);void*_tmp783;_LL4FA: if(_tmp782 <= (void*)4)goto _LL4FC;if(*((int*)
_tmp782)!= 5)goto _LL4FC;_tmp783=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp782)->f2;
_LL4FB:{void*_tmp784=_tmp783;_LL4FF: if((int)_tmp784 != 0)goto _LL501;_LL500: if(w > 
8)({void*_tmp785[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp786="bitfield larger than type";
_tag_dynforward(_tmp786,sizeof(char),_get_zero_arr_size(_tmp786,26));}),
_tag_dynforward(_tmp785,sizeof(void*),0));});goto _LL4FE;_LL501: if((int)_tmp784 != 
1)goto _LL503;_LL502: if(w > 16)({void*_tmp787[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp788="bitfield larger than type";_tag_dynforward(_tmp788,sizeof(char),
_get_zero_arr_size(_tmp788,26));}),_tag_dynforward(_tmp787,sizeof(void*),0));});
goto _LL4FE;_LL503: if((int)_tmp784 != 3)goto _LL505;_LL504: goto _LL506;_LL505: if((
int)_tmp784 != 2)goto _LL507;_LL506: if(w > 32)({void*_tmp789[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp78A="bitfield larger than type";_tag_dynforward(_tmp78A,
sizeof(char),_get_zero_arr_size(_tmp78A,26));}),_tag_dynforward(_tmp789,sizeof(
void*),0));});goto _LL4FE;_LL507: if((int)_tmp784 != 4)goto _LL4FE;_LL508: if(w > 64)({
void*_tmp78B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp78C="bitfield larger than type";
_tag_dynforward(_tmp78C,sizeof(char),_get_zero_arr_size(_tmp78C,26));}),
_tag_dynforward(_tmp78B,sizeof(void*),0));});goto _LL4FE;_LL4FE:;}goto _LL4F9;
_LL4FC:;_LL4FD:({struct Cyc_String_pa_struct _tmp790;_tmp790.tag=0;_tmp790.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(field_typ));{
struct Cyc_String_pa_struct _tmp78F;_tmp78F.tag=0;_tmp78F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp78D[2]={& _tmp78F,& _tmp790};
Cyc_Tcutil_terr(loc,({const char*_tmp78E="bitfield %s must have integral type but has type %s";
_tag_dynforward(_tmp78E,sizeof(char),_get_zero_arr_size(_tmp78E,52));}),
_tag_dynforward(_tmp78D,sizeof(void*),2));}}});goto _LL4F9;_LL4F9:;}}}static void
Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp791=(void*)
atts->hd;_LL50A: if((int)_tmp791 != 5)goto _LL50C;_LL50B: continue;_LL50C: if(_tmp791
<= (void*)17)goto _LL50E;if(*((int*)_tmp791)!= 1)goto _LL50E;_LL50D: continue;
_LL50E:;_LL50F:({struct Cyc_String_pa_struct _tmp795;_tmp795.tag=0;_tmp795.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_String_pa_struct
_tmp794;_tmp794.tag=0;_tmp794.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp792[2]={& _tmp794,& _tmp795};
Cyc_Tcutil_terr(loc,({const char*_tmp793="bad attribute %s on member %s";
_tag_dynforward(_tmp793,sizeof(char),_get_zero_arr_size(_tmp793,30));}),
_tag_dynforward(_tmp792,sizeof(void*),2));}}});_LL509:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp796=t;struct
Cyc_Absyn_Typedefdecl*_tmp797;void**_tmp798;_LL511: if(_tmp796 <= (void*)4)goto
_LL513;if(*((int*)_tmp796)!= 17)goto _LL513;_tmp797=((struct Cyc_Absyn_TypedefType_struct*)
_tmp796)->f3;_tmp798=((struct Cyc_Absyn_TypedefType_struct*)_tmp796)->f4;_LL512:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp797))->tq).real_const  || (
_tmp797->tq).print_const){if(declared_const)({void*_tmp799[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp79A="extra const";_tag_dynforward(_tmp79A,sizeof(char),
_get_zero_arr_size(_tmp79A,12));}),_tag_dynforward(_tmp799,sizeof(void*),0));});
return 1;}if((unsigned int)_tmp798)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp798);else{return declared_const;}_LL513:;_LL514: return
declared_const;_LL510:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt urgn={(
void*)((void*)3)};static struct Cyc_Core_Opt hrgn={(void*)((void*)2)};{void*_tmp79B=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp79C;struct Cyc_Core_Opt**_tmp79D;
struct Cyc_Core_Opt*_tmp79E;struct Cyc_Core_Opt**_tmp79F;struct Cyc_Absyn_Tvar*
_tmp7A0;struct Cyc_List_List*_tmp7A1;struct _tuple2*_tmp7A2;struct Cyc_Absyn_Enumdecl*
_tmp7A3;struct Cyc_Absyn_Enumdecl**_tmp7A4;struct Cyc_Absyn_TunionInfo _tmp7A5;
union Cyc_Absyn_TunionInfoU_union _tmp7A6;union Cyc_Absyn_TunionInfoU_union*_tmp7A7;
struct Cyc_List_List*_tmp7A8;struct Cyc_List_List**_tmp7A9;struct Cyc_Core_Opt*
_tmp7AA;struct Cyc_Core_Opt**_tmp7AB;struct Cyc_Absyn_TunionFieldInfo _tmp7AC;union
Cyc_Absyn_TunionFieldInfoU_union _tmp7AD;union Cyc_Absyn_TunionFieldInfoU_union*
_tmp7AE;struct Cyc_List_List*_tmp7AF;struct Cyc_Absyn_PtrInfo _tmp7B0;void*_tmp7B1;
struct Cyc_Absyn_Tqual _tmp7B2;struct Cyc_Absyn_Tqual*_tmp7B3;struct Cyc_Absyn_PtrAtts
_tmp7B4;void*_tmp7B5;struct Cyc_Absyn_Conref*_tmp7B6;struct Cyc_Absyn_Conref*
_tmp7B7;struct Cyc_Absyn_Conref*_tmp7B8;void*_tmp7B9;void*_tmp7BA;struct Cyc_Absyn_ArrayInfo
_tmp7BB;void*_tmp7BC;struct Cyc_Absyn_Tqual _tmp7BD;struct Cyc_Absyn_Tqual*_tmp7BE;
struct Cyc_Absyn_Exp*_tmp7BF;struct Cyc_Absyn_Exp**_tmp7C0;struct Cyc_Absyn_Conref*
_tmp7C1;struct Cyc_Position_Segment*_tmp7C2;struct Cyc_Absyn_FnInfo _tmp7C3;struct
Cyc_List_List*_tmp7C4;struct Cyc_List_List**_tmp7C5;struct Cyc_Core_Opt*_tmp7C6;
struct Cyc_Core_Opt**_tmp7C7;void*_tmp7C8;struct Cyc_List_List*_tmp7C9;int _tmp7CA;
struct Cyc_Absyn_VarargInfo*_tmp7CB;struct Cyc_List_List*_tmp7CC;struct Cyc_List_List*
_tmp7CD;struct Cyc_List_List*_tmp7CE;void*_tmp7CF;struct Cyc_List_List*_tmp7D0;
struct Cyc_Absyn_AggrInfo _tmp7D1;union Cyc_Absyn_AggrInfoU_union _tmp7D2;union Cyc_Absyn_AggrInfoU_union*
_tmp7D3;struct Cyc_List_List*_tmp7D4;struct Cyc_List_List**_tmp7D5;struct _tuple2*
_tmp7D6;struct Cyc_List_List*_tmp7D7;struct Cyc_List_List**_tmp7D8;struct Cyc_Absyn_Typedefdecl*
_tmp7D9;struct Cyc_Absyn_Typedefdecl**_tmp7DA;void**_tmp7DB;void***_tmp7DC;void*
_tmp7DD;void*_tmp7DE;void*_tmp7DF;void*_tmp7E0;void*_tmp7E1;struct Cyc_List_List*
_tmp7E2;_LL516: if((int)_tmp79B != 0)goto _LL518;_LL517: goto _LL515;_LL518: if(
_tmp79B <= (void*)4)goto _LL52C;if(*((int*)_tmp79B)!= 0)goto _LL51A;_tmp79C=((
struct Cyc_Absyn_Evar_struct*)_tmp79B)->f1;_tmp79D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp79B)->f1;_tmp79E=((struct Cyc_Absyn_Evar_struct*)
_tmp79B)->f2;_tmp79F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp79B)->f2;_LL519: if(*_tmp79D == 0)*_tmp79D=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && Cyc_Tcutil_is_region_kind(
expected_kind)){if(expected_kind == (void*)4)*_tmp79F=(struct Cyc_Core_Opt*)& urgn;
else{*_tmp79F=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp7E3=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7E8;_tmp7E8.tag=2;_tmp7E8.f1=0;_tmp7E8.f2=(void*)expected_kind;_tmp7E8;});
_tmp7E7;}));*_tmp79F=({struct Cyc_Core_Opt*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));
_tmp7E4->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7E5=_cycalloc(
sizeof(*_tmp7E5));_tmp7E5[0]=({struct Cyc_Absyn_VarType_struct _tmp7E6;_tmp7E6.tag=
1;_tmp7E6.f1=_tmp7E3;_tmp7E6;});_tmp7E5;}));_tmp7E4;});_tmp7A0=_tmp7E3;goto
_LL51B;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,
t,put_in_effect);}}goto _LL515;_LL51A: if(*((int*)_tmp79B)!= 1)goto _LL51C;_tmp7A0=((
struct Cyc_Absyn_VarType_struct*)_tmp79B)->f1;_LL51B:{void*_tmp7E9=Cyc_Absyn_compress_kb((
void*)_tmp7A0->kind);struct Cyc_Core_Opt*_tmp7EA;struct Cyc_Core_Opt**_tmp7EB;
_LL54D: if(*((int*)_tmp7E9)!= 1)goto _LL54F;_tmp7EA=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7E9)->f1;_tmp7EB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7E9)->f1;_LL54E:*_tmp7EB=({struct Cyc_Core_Opt*_tmp7EC=_cycalloc(sizeof(*
_tmp7EC));_tmp7EC->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7ED=
_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7EE;
_tmp7EE.tag=2;_tmp7EE.f1=0;_tmp7EE.f2=(void*)expected_kind;_tmp7EE;});_tmp7ED;}));
_tmp7EC;});goto _LL54C;_LL54F:;_LL550: goto _LL54C;_LL54C:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp7A0);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp7A0,put_in_effect);{void*_tmp7EF=Cyc_Absyn_compress_kb((
void*)_tmp7A0->kind);struct Cyc_Core_Opt*_tmp7F0;struct Cyc_Core_Opt**_tmp7F1;void*
_tmp7F2;_LL552: if(*((int*)_tmp7EF)!= 2)goto _LL554;_tmp7F0=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7EF)->f1;_tmp7F1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7EF)->f1;_tmp7F2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7EF)->f2;
_LL553: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7F2))*_tmp7F1=({struct Cyc_Core_Opt*
_tmp7F3=_cycalloc(sizeof(*_tmp7F3));_tmp7F3->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7F5;_tmp7F5.tag=2;_tmp7F5.f1=0;_tmp7F5.f2=(void*)expected_kind;_tmp7F5;});
_tmp7F4;}));_tmp7F3;});goto _LL551;_LL554:;_LL555: goto _LL551;_LL551:;}goto _LL515;
_LL51C: if(*((int*)_tmp79B)!= 13)goto _LL51E;_tmp7A1=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp79B)->f1;_LL51D: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct
_RegionHandle _tmp7F6=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmp7F6;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;for(0;_tmp7A1 != 0;_tmp7A1=_tmp7A1->tl){struct Cyc_Absyn_Enumfield*
_tmp7F7=(struct Cyc_Absyn_Enumfield*)_tmp7A1->hd;if(((int(*)(int(*compare)(struct
_dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l,struct
_dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7F7->name).f2))({
struct Cyc_String_pa_struct _tmp7FA;_tmp7FA.tag=0;_tmp7FA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*(*_tmp7F7->name).f2);{void*_tmp7F8[1]={&
_tmp7FA};Cyc_Tcutil_terr(_tmp7F7->loc,({const char*_tmp7F9="duplicate enum field name %s";
_tag_dynforward(_tmp7F9,sizeof(char),_get_zero_arr_size(_tmp7F9,29));}),
_tag_dynforward(_tmp7F8,sizeof(void*),1));}});else{prev_fields=({struct Cyc_List_List*
_tmp7FB=_region_malloc(uprev_rgn,sizeof(*_tmp7FB));_tmp7FB->hd=(*_tmp7F7->name).f2;
_tmp7FB->tl=prev_fields;_tmp7FB;});}if(_tmp7F7->tag == 0)_tmp7F7->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp7F7->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp7F7->tag)))({struct Cyc_String_pa_struct
_tmp7FE;_tmp7FE.tag=0;_tmp7FE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
_tmp7F7->name).f2);{void*_tmp7FC[1]={& _tmp7FE};Cyc_Tcutil_terr(loc,({const char*
_tmp7FD="enum field %s: expression is not constant";_tag_dynforward(_tmp7FD,
sizeof(char),_get_zero_arr_size(_tmp7FD,42));}),_tag_dynforward(_tmp7FC,sizeof(
void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp7F7->tag))).f1;tag_count=t1 + 1;(*_tmp7F7->name).f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7FF;(_tmp7FF.Abs_n).tag=2;(_tmp7FF.Abs_n).f1=te->ns;
_tmp7FF;});ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k,struct _tuple19*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7F7->name).f2,(
struct _tuple19*)({struct _tuple19*_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800->f1=(
void*)({struct Cyc_Tcenv_AnonEnumRes_struct*_tmp801=_cycalloc(sizeof(*_tmp801));
_tmp801[0]=({struct Cyc_Tcenv_AnonEnumRes_struct _tmp802;_tmp802.tag=4;_tmp802.f1=(
void*)t;_tmp802.f2=_tmp7F7;_tmp802;});_tmp801;});_tmp800->f2=1;_tmp800;}));}}};
_pop_region(uprev_rgn);}goto _LL515;}_LL51E: if(*((int*)_tmp79B)!= 12)goto _LL520;
_tmp7A2=((struct Cyc_Absyn_EnumType_struct*)_tmp79B)->f1;_tmp7A3=((struct Cyc_Absyn_EnumType_struct*)
_tmp79B)->f2;_tmp7A4=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp79B)->f2;_LL51F: if(*_tmp7A4 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp7A4))->fields == 0){struct _handler_cons _tmp803;_push_handler(& _tmp803);{int
_tmp805=0;if(setjmp(_tmp803.handler))_tmp805=1;if(!_tmp805){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7A2);*_tmp7A4=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp804=(void*)_exn_thrown;void*_tmp807=_tmp804;
_LL557: if(_tmp807 != Cyc_Dict_Absent)goto _LL559;_LL558: {struct Cyc_Tcenv_Genv*
_tmp808=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp809=({struct Cyc_Absyn_Enumdecl*
_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A->sc=(void*)((void*)3);_tmp80A->name=
_tmp7A2;_tmp80A->fields=0;_tmp80A;});Cyc_Tc_tcEnumdecl(te,_tmp808,loc,_tmp809);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7A2);*_tmp7A4=(
struct Cyc_Absyn_Enumdecl*)*ed;goto _LL556;}}_LL559:;_LL55A:(void)_throw(_tmp807);
_LL556:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp7A4);*_tmp7A2=(ed->name)[_check_known_subscript_notnull(1,0)];goto _LL515;}
_LL520: if(*((int*)_tmp79B)!= 2)goto _LL522;_tmp7A5=((struct Cyc_Absyn_TunionType_struct*)
_tmp79B)->f1;_tmp7A6=_tmp7A5.tunion_info;_tmp7A7=(union Cyc_Absyn_TunionInfoU_union*)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp79B)->f1).tunion_info;_tmp7A8=_tmp7A5.targs;
_tmp7A9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp79B)->f1).targs;
_tmp7AA=_tmp7A5.rgn;_tmp7AB=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp79B)->f1).rgn;_LL521: {struct Cyc_List_List*_tmp80B=*_tmp7A9;{union Cyc_Absyn_TunionInfoU_union
_tmp80C=*_tmp7A7;struct Cyc_Absyn_UnknownTunionInfo _tmp80D;struct _tuple2*_tmp80E;
int _tmp80F;int _tmp810;struct Cyc_Absyn_Tuniondecl**_tmp811;struct Cyc_Absyn_Tuniondecl*
_tmp812;_LL55C: if((_tmp80C.UnknownTunion).tag != 0)goto _LL55E;_tmp80D=(_tmp80C.UnknownTunion).f1;
_tmp80E=_tmp80D.name;_tmp80F=_tmp80D.is_xtunion;_tmp810=_tmp80D.is_flat;_LL55D: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp813;_push_handler(&
_tmp813);{int _tmp815=0;if(setjmp(_tmp813.handler))_tmp815=1;if(!_tmp815){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp80E);;_pop_handler();}else{void*_tmp814=(
void*)_exn_thrown;void*_tmp817=_tmp814;_LL561: if(_tmp817 != Cyc_Dict_Absent)goto
_LL563;_LL562: {struct Cyc_Tcenv_Genv*_tmp818=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmp819=({struct Cyc_Absyn_Tuniondecl*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820->sc=(
void*)((void*)3);_tmp820->name=_tmp80E;_tmp820->tvs=0;_tmp820->fields=0;_tmp820->is_xtunion=
_tmp80F;_tmp820->is_flat=_tmp810;_tmp820;});Cyc_Tc_tcTuniondecl(te,_tmp818,loc,
_tmp819);tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp80E);if(_tmp80B != 0){({
struct Cyc_String_pa_struct _tmp81D;_tmp81D.tag=0;_tmp81D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp80E));{
struct Cyc_String_pa_struct _tmp81C;_tmp81C.tag=0;_tmp81C.f1=(struct
_dynforward_ptr)(_tmp80F?(struct _dynforward_ptr)((struct _dynforward_ptr)({const
char*_tmp81E="xtunion";_tag_dynforward(_tmp81E,sizeof(char),_get_zero_arr_size(
_tmp81E,8));})):(struct _dynforward_ptr)({const char*_tmp81F="tunion";
_tag_dynforward(_tmp81F,sizeof(char),_get_zero_arr_size(_tmp81F,7));}));{void*
_tmp81A[2]={& _tmp81C,& _tmp81D};Cyc_Tcutil_terr(loc,({const char*_tmp81B="declare parameterized %s %s before using";
_tag_dynforward(_tmp81B,sizeof(char),_get_zero_arr_size(_tmp81B,41));}),
_tag_dynforward(_tmp81A,sizeof(void*),2));}}});return cvtenv;}goto _LL560;}_LL563:;
_LL564:(void)_throw(_tmp817);_LL560:;}}}if((*tudp)->is_xtunion != _tmp80F)({
struct Cyc_String_pa_struct _tmp823;_tmp823.tag=0;_tmp823.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp80E));{void*
_tmp821[1]={& _tmp823};Cyc_Tcutil_terr(loc,({const char*_tmp822="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp822,sizeof(char),_get_zero_arr_size(_tmp822,48));}),
_tag_dynforward(_tmp821,sizeof(void*),1));}});*_tmp7A7=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp824;(_tmp824.KnownTunion).tag=1;(_tmp824.KnownTunion).f1=
tudp;_tmp824;});_tmp812=*tudp;goto _LL55F;}_LL55E: if((_tmp80C.KnownTunion).tag != 
1)goto _LL55B;_tmp811=(_tmp80C.KnownTunion).f1;_tmp812=*_tmp811;_LL55F: if(_tmp812->is_flat
 && (unsigned int)*_tmp7AB)({struct Cyc_String_pa_struct _tmp827;_tmp827.tag=0;
_tmp827.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7AB))->v));{void*_tmp825[1]={&
_tmp827};Cyc_Tcutil_terr(loc,({const char*_tmp826="flat tunion has region %s";
_tag_dynforward(_tmp826,sizeof(char),_get_zero_arr_size(_tmp826,26));}),
_tag_dynforward(_tmp825,sizeof(void*),1));}});if(!_tmp812->is_flat  && !((
unsigned int)*_tmp7AB))*_tmp7AB=({struct Cyc_Core_Opt*_tmp828=_cycalloc(sizeof(*
_tmp828));_tmp828->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
0);_tmp828;});if((unsigned int)*_tmp7AB){void*_tmp829=(void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp7AB))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)3,_tmp829,1);}{struct Cyc_List_List*tvs=_tmp812->tvs;for(0;_tmp80B != 0  && 
tvs != 0;(_tmp80B=_tmp80B->tl,tvs=tvs->tl)){void*t1=(void*)_tmp80B->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1,1);}if(_tmp80B != 0)({struct Cyc_String_pa_struct _tmp82C;
_tmp82C.tag=0;_tmp82C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp812->name));{void*_tmp82A[1]={& _tmp82C};Cyc_Tcutil_terr(loc,({const char*
_tmp82B="too many type arguments for tunion %s";_tag_dynforward(_tmp82B,sizeof(
char),_get_zero_arr_size(_tmp82B,38));}),_tag_dynforward(_tmp82A,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D->hd=(
void*)e;_tmp82D->tl=hidden_ts;_tmp82D;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,1);}*_tmp7A9=Cyc_List_imp_append(*_tmp7A9,Cyc_List_imp_rev(
hidden_ts));}goto _LL55B;}_LL55B:;}goto _LL515;}_LL522: if(*((int*)_tmp79B)!= 3)
goto _LL524;_tmp7AC=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp79B)->f1;
_tmp7AD=_tmp7AC.field_info;_tmp7AE=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp79B)->f1).field_info;_tmp7AF=_tmp7AC.targs;
_LL523:{union Cyc_Absyn_TunionFieldInfoU_union _tmp82E=*_tmp7AE;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp82F;struct _tuple2*_tmp830;struct _tuple2*_tmp831;int _tmp832;struct Cyc_Absyn_Tuniondecl*
_tmp833;struct Cyc_Absyn_Tunionfield*_tmp834;_LL566: if((_tmp82E.UnknownTunionfield).tag
!= 0)goto _LL568;_tmp82F=(_tmp82E.UnknownTunionfield).f1;_tmp830=_tmp82F.tunion_name;
_tmp831=_tmp82F.field_name;_tmp832=_tmp82F.is_xtunion;_LL567: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp835;_push_handler(&
_tmp835);{int _tmp837=0;if(setjmp(_tmp835.handler))_tmp837=1;if(!_tmp837){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp830);;_pop_handler();}else{void*_tmp836=(void*)_exn_thrown;void*
_tmp839=_tmp836;_LL56B: if(_tmp839 != Cyc_Dict_Absent)goto _LL56D;_LL56C:({struct
Cyc_String_pa_struct _tmp83C;_tmp83C.tag=0;_tmp83C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp830));{void*_tmp83A[1]={&
_tmp83C};Cyc_Tcutil_terr(loc,({const char*_tmp83B="unbound type tunion %s";
_tag_dynforward(_tmp83B,sizeof(char),_get_zero_arr_size(_tmp83B,23));}),
_tag_dynforward(_tmp83A,sizeof(void*),1));}});return cvtenv;_LL56D:;_LL56E:(void)
_throw(_tmp839);_LL56A:;}}}{struct _handler_cons _tmp83D;_push_handler(& _tmp83D);{
int _tmp83F=0;if(setjmp(_tmp83D.handler))_tmp83F=1;if(!_tmp83F){{struct
_RegionHandle _tmp840=_new_region("r");struct _RegionHandle*r=& _tmp840;
_push_region(r);{void*_tmp841=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp831);struct
Cyc_Absyn_Tuniondecl*_tmp842;struct Cyc_Absyn_Tunionfield*_tmp843;_LL570: if(*((
int*)_tmp841)!= 2)goto _LL572;_tmp842=((struct Cyc_Tcenv_TunionRes_struct*)_tmp841)->f1;
_tmp843=((struct Cyc_Tcenv_TunionRes_struct*)_tmp841)->f2;_LL571: tuf=_tmp843;tud=
_tmp842;if(tud->is_xtunion != _tmp832)({struct Cyc_String_pa_struct _tmp846;_tmp846.tag=
0;_tmp846.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp830));{void*_tmp844[1]={& _tmp846};Cyc_Tcutil_terr(loc,({const char*_tmp845="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp845,sizeof(char),_get_zero_arr_size(_tmp845,48));}),
_tag_dynforward(_tmp844,sizeof(void*),1));}});goto _LL56F;_LL572:;_LL573:({struct
Cyc_String_pa_struct _tmp84A;_tmp84A.tag=0;_tmp84A.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp830));{struct Cyc_String_pa_struct
_tmp849;_tmp849.tag=0;_tmp849.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp831));{void*_tmp847[2]={& _tmp849,& _tmp84A};Cyc_Tcutil_terr(
loc,({const char*_tmp848="unbound field %s in type tunion %s";_tag_dynforward(
_tmp848,sizeof(char),_get_zero_arr_size(_tmp848,35));}),_tag_dynforward(_tmp847,
sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp84B=cvtenv;_npop_handler(1);
return _tmp84B;}_LL56F:;};_pop_region(r);};_pop_handler();}else{void*_tmp83E=(
void*)_exn_thrown;void*_tmp84D=_tmp83E;_LL575: if(_tmp84D != Cyc_Dict_Absent)goto
_LL577;_LL576:({struct Cyc_String_pa_struct _tmp851;_tmp851.tag=0;_tmp851.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp830));{
struct Cyc_String_pa_struct _tmp850;_tmp850.tag=0;_tmp850.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp831));{void*
_tmp84E[2]={& _tmp850,& _tmp851};Cyc_Tcutil_terr(loc,({const char*_tmp84F="unbound field %s in type tunion %s";
_tag_dynforward(_tmp84F,sizeof(char),_get_zero_arr_size(_tmp84F,35));}),
_tag_dynforward(_tmp84E,sizeof(void*),2));}}});return cvtenv;_LL577:;_LL578:(void)
_throw(_tmp84D);_LL574:;}}}*_tmp7AE=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp852;(_tmp852.KnownTunionfield).tag=1;(
_tmp852.KnownTunionfield).f1=tud;(_tmp852.KnownTunionfield).f2=tuf;_tmp852;});
_tmp833=tud;_tmp834=tuf;goto _LL569;}_LL568: if((_tmp82E.KnownTunionfield).tag != 1)
goto _LL565;_tmp833=(_tmp82E.KnownTunionfield).f1;_tmp834=(_tmp82E.KnownTunionfield).f2;
_LL569: {struct Cyc_List_List*tvs=_tmp833->tvs;for(0;_tmp7AF != 0  && tvs != 0;(
_tmp7AF=_tmp7AF->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7AF->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp7AF != 0)({struct Cyc_String_pa_struct _tmp856;_tmp856.tag=
0;_tmp856.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp834->name));{struct Cyc_String_pa_struct _tmp855;_tmp855.tag=0;_tmp855.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp833->name));{
void*_tmp853[2]={& _tmp855,& _tmp856};Cyc_Tcutil_terr(loc,({const char*_tmp854="too many type arguments for tunion %s.%s";
_tag_dynforward(_tmp854,sizeof(char),_get_zero_arr_size(_tmp854,41));}),
_tag_dynforward(_tmp853,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct
_tmp85A;_tmp85A.tag=0;_tmp85A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp834->name));{struct Cyc_String_pa_struct _tmp859;
_tmp859.tag=0;_tmp859.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp833->name));{void*_tmp857[2]={& _tmp859,& _tmp85A};Cyc_Tcutil_terr(loc,({const
char*_tmp858="too few type arguments for tunion %s.%s";_tag_dynforward(_tmp858,
sizeof(char),_get_zero_arr_size(_tmp858,40));}),_tag_dynforward(_tmp857,sizeof(
void*),2));}}});goto _LL565;}_LL565:;}goto _LL515;_LL524: if(*((int*)_tmp79B)!= 4)
goto _LL526;_tmp7B0=((struct Cyc_Absyn_PointerType_struct*)_tmp79B)->f1;_tmp7B1=(
void*)_tmp7B0.elt_typ;_tmp7B2=_tmp7B0.elt_tq;_tmp7B3=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp79B)->f1).elt_tq;_tmp7B4=_tmp7B0.ptr_atts;
_tmp7B5=(void*)_tmp7B4.rgn;_tmp7B6=_tmp7B4.nullable;_tmp7B7=_tmp7B4.bounds;
_tmp7B8=_tmp7B4.zero_term;_LL525: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp7B1,1);_tmp7B3->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp7B3->print_const,_tmp7B1);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)5,_tmp7B5,1);{union Cyc_Absyn_Constraint_union _tmp85B=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp7B8))->v;int _tmp85C;
_LL57A: if((_tmp85B.No_constr).tag != 2)goto _LL57C;_LL57B:{void*_tmp85D=Cyc_Tcutil_compress(
_tmp7B1);void*_tmp85E;_LL581: if(_tmp85D <= (void*)4)goto _LL583;if(*((int*)_tmp85D)
!= 5)goto _LL583;_tmp85E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp85D)->f2;
if((int)_tmp85E != 0)goto _LL583;_LL582:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7B8,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL580;_LL583:;_LL584:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7B8,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL580;
_LL580:;}goto _LL579;_LL57C: if((_tmp85B.Eq_constr).tag != 0)goto _LL57E;_tmp85C=(
_tmp85B.Eq_constr).f1;if(_tmp85C != 1)goto _LL57E;_LL57D: if(!Cyc_Tcutil_admits_zero(
_tmp7B1))({struct Cyc_String_pa_struct _tmp861;_tmp861.tag=0;_tmp861.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp7B1));{void*
_tmp85F[1]={& _tmp861};Cyc_Tcutil_terr(loc,({const char*_tmp860="cannot have a pointer to zero-terminated %s elements";
_tag_dynforward(_tmp860,sizeof(char),_get_zero_arr_size(_tmp860,53));}),
_tag_dynforward(_tmp85F,sizeof(void*),1));}});is_zero_terminated=1;goto _LL579;
_LL57E:;_LL57F: is_zero_terminated=0;goto _LL579;_LL579:;}{union Cyc_Absyn_Constraint_union
_tmp862=(Cyc_Absyn_compress_conref(_tmp7B7))->v;void*_tmp863;void*_tmp864;void*
_tmp865;struct Cyc_Absyn_Exp*_tmp866;void*_tmp867;void*_tmp868;_LL586: if((_tmp862.No_constr).tag
!= 2)goto _LL588;_LL587: goto _LL589;_LL588: if((_tmp862.Eq_constr).tag != 0)goto
_LL58A;_tmp863=(_tmp862.Eq_constr).f1;if((int)_tmp863 != 0)goto _LL58A;_LL589: goto
_LL585;_LL58A: if((_tmp862.Eq_constr).tag != 0)goto _LL58C;_tmp864=(_tmp862.Eq_constr).f1;
if((int)_tmp864 != 1)goto _LL58C;_LL58B: goto _LL585;_LL58C: if((_tmp862.Eq_constr).tag
!= 0)goto _LL58E;_tmp865=(_tmp862.Eq_constr).f1;if(_tmp865 <= (void*)2)goto _LL58E;
if(*((int*)_tmp865)!= 0)goto _LL58E;_tmp866=((struct Cyc_Absyn_Upper_b_struct*)
_tmp865)->f1;_LL58D: Cyc_Tcexp_tcExp(te,0,_tmp866);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp866))({void*_tmp869[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp86A="pointer bounds expression is not an unsigned int";
_tag_dynforward(_tmp86A,sizeof(char),_get_zero_arr_size(_tmp86A,49));}),
_tag_dynforward(_tmp869,sizeof(void*),0));});{unsigned int _tmp86C;int _tmp86D;
struct _tuple7 _tmp86B=Cyc_Evexp_eval_const_uint_exp(_tmp866);_tmp86C=_tmp86B.f1;
_tmp86D=_tmp86B.f2;if(is_zero_terminated  && (!_tmp86D  || _tmp86C < 1))({void*
_tmp86E[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp86F="zero-terminated pointer cannot point to empty sequence";
_tag_dynforward(_tmp86F,sizeof(char),_get_zero_arr_size(_tmp86F,55));}),
_tag_dynforward(_tmp86E,sizeof(void*),0));});goto _LL585;}_LL58E: if((_tmp862.Eq_constr).tag
!= 0)goto _LL590;_tmp867=(_tmp862.Eq_constr).f1;if(_tmp867 <= (void*)2)goto _LL590;
if(*((int*)_tmp867)!= 1)goto _LL590;_tmp868=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp867)->f1;_LL58F: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)7,
_tmp868,1);goto _LL585;_LL590: if((_tmp862.Forward_constr).tag != 1)goto _LL585;
_LL591:({void*_tmp870[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp871="forward constraint";
_tag_dynforward(_tmp871,sizeof(char),_get_zero_arr_size(_tmp871,19));}),
_tag_dynforward(_tmp870,sizeof(void*),0));});_LL585:;}goto _LL515;}_LL526: if(*((
int*)_tmp79B)!= 18)goto _LL528;_tmp7B9=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp79B)->f1;_LL527: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)7,
_tmp7B9,1);goto _LL515;_LL528: if(*((int*)_tmp79B)!= 14)goto _LL52A;_tmp7BA=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp79B)->f1;_LL529: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp7BA,1);goto _LL515;_LL52A: if(*((int*)_tmp79B)!= 5)goto
_LL52C;_LL52B: goto _LL52D;_LL52C: if((int)_tmp79B != 1)goto _LL52E;_LL52D: goto _LL52F;
_LL52E: if(_tmp79B <= (void*)4)goto _LL53E;if(*((int*)_tmp79B)!= 6)goto _LL530;
_LL52F: goto _LL515;_LL530: if(*((int*)_tmp79B)!= 7)goto _LL532;_tmp7BB=((struct Cyc_Absyn_ArrayType_struct*)
_tmp79B)->f1;_tmp7BC=(void*)_tmp7BB.elt_type;_tmp7BD=_tmp7BB.tq;_tmp7BE=(struct
Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp79B)->f1).tq;_tmp7BF=
_tmp7BB.num_elts;_tmp7C0=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp79B)->f1).num_elts;_tmp7C1=_tmp7BB.zero_term;_tmp7C2=_tmp7BB.zt_loc;_LL531: {
struct Cyc_Absyn_Exp*_tmp872=*_tmp7C0;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)1,_tmp7BC,1);_tmp7BE->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp7BE->print_const,_tmp7BC);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union
_tmp873=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7C1))->v;int _tmp874;_LL593: if((_tmp873.No_constr).tag != 2)goto _LL595;_LL594:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7C1,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL592;
_LL595: if((_tmp873.Eq_constr).tag != 0)goto _LL597;_tmp874=(_tmp873.Eq_constr).f1;
if(_tmp874 != 1)goto _LL597;_LL596: if(!Cyc_Tcutil_admits_zero(_tmp7BC))({struct Cyc_String_pa_struct
_tmp877;_tmp877.tag=0;_tmp877.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp7BC));{void*_tmp875[1]={& _tmp877};Cyc_Tcutil_terr(loc,({
const char*_tmp876="cannot have a zero-terminated array of %s elements";
_tag_dynforward(_tmp876,sizeof(char),_get_zero_arr_size(_tmp876,51));}),
_tag_dynforward(_tmp875,sizeof(void*),1));}});is_zero_terminated=1;goto _LL592;
_LL597:;_LL598: is_zero_terminated=0;goto _LL592;_LL592:;}if(_tmp872 == 0){if(
is_zero_terminated)*_tmp7C0=(_tmp872=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,
0));else{({void*_tmp878[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp879="array bound defaults to 0 here";
_tag_dynforward(_tmp879,sizeof(char),_get_zero_arr_size(_tmp879,31));}),
_tag_dynforward(_tmp878,sizeof(void*),0));});*_tmp7C0=(_tmp872=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp872);
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp872))({void*_tmp87A[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp87B="array bounds expression is not constant";
_tag_dynforward(_tmp87B,sizeof(char),_get_zero_arr_size(_tmp87B,40));}),
_tag_dynforward(_tmp87A,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_tmp872))({void*_tmp87C[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp87D="array bounds expression is not an unsigned int";_tag_dynforward(
_tmp87D,sizeof(char),_get_zero_arr_size(_tmp87D,47));}),_tag_dynforward(_tmp87C,
sizeof(void*),0));});{unsigned int _tmp87F;int _tmp880;struct _tuple7 _tmp87E=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp872);_tmp87F=_tmp87E.f1;_tmp880=_tmp87E.f2;if((
is_zero_terminated  && _tmp880) && _tmp87F < 1)({void*_tmp881[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp882="zero terminated array cannot have zero elements";
_tag_dynforward(_tmp882,sizeof(char),_get_zero_arr_size(_tmp882,48));}),
_tag_dynforward(_tmp881,sizeof(void*),0));});goto _LL515;}}}_LL532: if(*((int*)
_tmp79B)!= 8)goto _LL534;_tmp7C3=((struct Cyc_Absyn_FnType_struct*)_tmp79B)->f1;
_tmp7C4=_tmp7C3.tvars;_tmp7C5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp79B)->f1).tvars;_tmp7C6=_tmp7C3.effect;_tmp7C7=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp79B)->f1).effect;_tmp7C8=(void*)_tmp7C3.ret_typ;
_tmp7C9=_tmp7C3.args;_tmp7CA=_tmp7C3.c_varargs;_tmp7CB=_tmp7C3.cyc_varargs;
_tmp7CC=_tmp7C3.rgn_po;_tmp7CD=_tmp7C3.attributes;_LL533: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7CD != 0;_tmp7CD=_tmp7CD->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7CD->hd))({struct Cyc_String_pa_struct _tmp885;
_tmp885.tag=0;_tmp885.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)_tmp7CD->hd));{void*_tmp883[1]={& _tmp885};Cyc_Tcutil_terr(loc,({const char*
_tmp884="bad function type attribute %s";_tag_dynforward(_tmp884,sizeof(char),
_get_zero_arr_size(_tmp884,31));}),_tag_dynforward(_tmp883,sizeof(void*),1));}});{
void*_tmp886=(void*)_tmp7CD->hd;void*_tmp887;int _tmp888;int _tmp889;_LL59A: if((
int)_tmp886 != 0)goto _LL59C;_LL59B: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL599;_LL59C: if((int)_tmp886 != 1)goto _LL59E;_LL59D: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL599;_LL59E: if((int)_tmp886 != 2)goto _LL5A0;
_LL59F: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL599;_LL5A0: if(
_tmp886 <= (void*)17)goto _LL5A2;if(*((int*)_tmp886)!= 3)goto _LL5A2;_tmp887=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp886)->f1;_tmp888=((struct Cyc_Absyn_Format_att_struct*)
_tmp886)->f2;_tmp889=((struct Cyc_Absyn_Format_att_struct*)_tmp886)->f3;_LL5A1:
if(!seen_format){seen_format=1;ft=_tmp887;fmt_desc_arg=_tmp888;fmt_arg_start=
_tmp889;}else{({void*_tmp88A[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp88B="function can't have multiple format attributes";
_tag_dynforward(_tmp88B,sizeof(char),_get_zero_arr_size(_tmp88B,47));}),
_tag_dynforward(_tmp88A,sizeof(void*),0));});}goto _LL599;_LL5A2:;_LL5A3: goto
_LL599;_LL599:;}}if(num_convs > 1)({void*_tmp88C[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp88D="function can't have multiple calling conventions";
_tag_dynforward(_tmp88D,sizeof(char),_get_zero_arr_size(_tmp88D,49));}),
_tag_dynforward(_tmp88C,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*
_tmp7C5);{struct Cyc_List_List*b=*_tmp7C5;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp88E=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp88F;void*_tmp890;_LL5A5: if(*((
int*)_tmp88E)!= 0)goto _LL5A7;_tmp88F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp88E)->f1;if((int)_tmp88F != 1)goto _LL5A7;_LL5A6:({struct Cyc_String_pa_struct
_tmp893;_tmp893.tag=0;_tmp893.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp891[1]={& _tmp893};Cyc_Tcutil_terr(
loc,({const char*_tmp892="function attempts to abstract Mem type variable %s";
_tag_dynforward(_tmp892,sizeof(char),_get_zero_arr_size(_tmp892,51));}),
_tag_dynforward(_tmp891,sizeof(void*),1));}});goto _LL5A4;_LL5A7: if(*((int*)
_tmp88E)!= 0)goto _LL5A9;_tmp890=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp88E)->f1;
if((int)_tmp890 != 5)goto _LL5A9;_LL5A8:({struct Cyc_String_pa_struct _tmp896;
_tmp896.tag=0;_tmp896.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp894[1]={& _tmp896};Cyc_Tcutil_terr(
loc,({const char*_tmp895="function attempts to abstract TopRegion type variable %s";
_tag_dynforward(_tmp895,sizeof(char),_get_zero_arr_size(_tmp895,57));}),
_tag_dynforward(_tmp894,sizeof(void*),1));}});goto _LL5A4;_LL5A9:;_LL5AA: goto
_LL5A4;_LL5A4:;}}}{struct _RegionHandle _tmp897=_new_region("newr");struct
_RegionHandle*newr=& _tmp897;_push_region(newr);{struct Cyc_Tcutil_CVTEnv _tmp898=({
struct Cyc_Tcutil_CVTEnv _tmp923;_tmp923.r=newr;_tmp923.kind_env=cvtenv.kind_env;
_tmp923.free_vars=0;_tmp923.free_evars=0;_tmp923.generalize_evars=cvtenv.generalize_evars;
_tmp923.fn_result=1;_tmp923;});_tmp898=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp898,(void*)1,_tmp7C8,1);_tmp898.fn_result=0;{struct Cyc_List_List*a=_tmp7C9;
for(0;a != 0;a=a->tl){struct _tuple3*_tmp899=(struct _tuple3*)a->hd;void*_tmp89A=(*
_tmp899).f3;_tmp898=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp898,(void*)1,
_tmp89A,1);((*_tmp899).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmp899).f2).print_const,_tmp89A);}}if(_tmp7CB != 0){if(_tmp7CA)({void*_tmp89B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp89C="both c_vararg and cyc_vararg";_tag_dynforward(_tmp89C,sizeof(
char),_get_zero_arr_size(_tmp89C,29));}),_tag_dynforward(_tmp89B,sizeof(void*),0));});{
void*_tmp89E;int _tmp89F;struct Cyc_Absyn_VarargInfo _tmp89D=*_tmp7CB;_tmp89E=(void*)
_tmp89D.type;_tmp89F=_tmp89D.inject;_tmp898=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp898,(void*)1,_tmp89E,1);(_tmp7CB->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7CB->tq).print_const,_tmp89E);if(_tmp89F){void*_tmp8A0=Cyc_Tcutil_compress(
_tmp89E);struct Cyc_Absyn_TunionInfo _tmp8A1;union Cyc_Absyn_TunionInfoU_union
_tmp8A2;struct Cyc_Absyn_Tuniondecl**_tmp8A3;_LL5AC: if(_tmp8A0 <= (void*)4)goto
_LL5AE;if(*((int*)_tmp8A0)!= 2)goto _LL5AE;_tmp8A1=((struct Cyc_Absyn_TunionType_struct*)
_tmp8A0)->f1;_tmp8A2=_tmp8A1.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp8A0)->f1).tunion_info).KnownTunion).tag != 1)goto _LL5AE;_tmp8A3=(_tmp8A2.KnownTunion).f1;
_LL5AD: if((*_tmp8A3)->is_flat)({void*_tmp8A4[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp8A5="cant inject into a flat tunion";_tag_dynforward(_tmp8A5,sizeof(
char),_get_zero_arr_size(_tmp8A5,31));}),_tag_dynforward(_tmp8A4,sizeof(void*),0));});
goto _LL5AB;_LL5AE:;_LL5AF:({void*_tmp8A6[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp8A7="can't inject a non-[x]tunion type";_tag_dynforward(_tmp8A7,sizeof(char),
_get_zero_arr_size(_tmp8A7,34));}),_tag_dynforward(_tmp8A6,sizeof(void*),0));});
goto _LL5AB;_LL5AB:;}}}if(seen_format){int _tmp8A8=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp7C9);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp8A8) || 
fmt_arg_start < 0) || _tmp7CB == 0  && fmt_arg_start != 0) || _tmp7CB != 0  && 
fmt_arg_start != _tmp8A8 + 1)({void*_tmp8A9[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp8AA="bad format descriptor";_tag_dynforward(_tmp8AA,sizeof(char),
_get_zero_arr_size(_tmp8AA,22));}),_tag_dynforward(_tmp8A9,sizeof(void*),0));});
else{void*_tmp8AC;struct _tuple3 _tmp8AB=*((struct _tuple3*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp7C9,fmt_desc_arg - 1);_tmp8AC=_tmp8AB.f3;{void*_tmp8AD=
Cyc_Tcutil_compress(_tmp8AC);struct Cyc_Absyn_PtrInfo _tmp8AE;void*_tmp8AF;struct
Cyc_Absyn_PtrAtts _tmp8B0;struct Cyc_Absyn_Conref*_tmp8B1;struct Cyc_Absyn_Conref*
_tmp8B2;_LL5B1: if(_tmp8AD <= (void*)4)goto _LL5B3;if(*((int*)_tmp8AD)!= 4)goto
_LL5B3;_tmp8AE=((struct Cyc_Absyn_PointerType_struct*)_tmp8AD)->f1;_tmp8AF=(void*)
_tmp8AE.elt_typ;_tmp8B0=_tmp8AE.ptr_atts;_tmp8B1=_tmp8B0.bounds;_tmp8B2=_tmp8B0.zero_term;
_LL5B2:{struct _tuple0 _tmp8B4=({struct _tuple0 _tmp8B3;_tmp8B3.f1=Cyc_Tcutil_compress(
_tmp8AF);_tmp8B3.f2=Cyc_Absyn_conref_def((void*)((void*)0),_tmp8B1);_tmp8B3;});
void*_tmp8B5;void*_tmp8B6;void*_tmp8B7;void*_tmp8B8;_LL5B6: _tmp8B5=_tmp8B4.f1;
if(_tmp8B5 <= (void*)4)goto _LL5B8;if(*((int*)_tmp8B5)!= 5)goto _LL5B8;_tmp8B6=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp8B5)->f1;if((int)_tmp8B6 != 2)goto
_LL5B8;_tmp8B7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp8B5)->f2;if((int)
_tmp8B7 != 0)goto _LL5B8;_tmp8B8=_tmp8B4.f2;if((int)_tmp8B8 != 0)goto _LL5B8;_LL5B7:
goto _LL5B5;_LL5B8:;_LL5B9:({void*_tmp8B9[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp8BA="format descriptor is not a char ? type";_tag_dynforward(_tmp8BA,sizeof(
char),_get_zero_arr_size(_tmp8BA,39));}),_tag_dynforward(_tmp8B9,sizeof(void*),0));});
goto _LL5B5;_LL5B5:;}goto _LL5B0;_LL5B3:;_LL5B4:({void*_tmp8BB[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8BC="format descriptor is not a char ? type";_tag_dynforward(
_tmp8BC,sizeof(char),_get_zero_arr_size(_tmp8BC,39));}),_tag_dynforward(_tmp8BB,
sizeof(void*),0));});goto _LL5B0;_LL5B0:;}if(fmt_arg_start != 0){int problem;{
struct _tuple0 _tmp8BE=({struct _tuple0 _tmp8BD;_tmp8BD.f1=ft;_tmp8BD.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7CB))->type);_tmp8BD;});void*
_tmp8BF;void*_tmp8C0;struct Cyc_Absyn_TunionInfo _tmp8C1;union Cyc_Absyn_TunionInfoU_union
_tmp8C2;struct Cyc_Absyn_Tuniondecl**_tmp8C3;struct Cyc_Absyn_Tuniondecl*_tmp8C4;
void*_tmp8C5;void*_tmp8C6;struct Cyc_Absyn_TunionInfo _tmp8C7;union Cyc_Absyn_TunionInfoU_union
_tmp8C8;struct Cyc_Absyn_Tuniondecl**_tmp8C9;struct Cyc_Absyn_Tuniondecl*_tmp8CA;
_LL5BB: _tmp8BF=_tmp8BE.f1;if((int)_tmp8BF != 0)goto _LL5BD;_tmp8C0=_tmp8BE.f2;if(
_tmp8C0 <= (void*)4)goto _LL5BD;if(*((int*)_tmp8C0)!= 2)goto _LL5BD;_tmp8C1=((
struct Cyc_Absyn_TunionType_struct*)_tmp8C0)->f1;_tmp8C2=_tmp8C1.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8C0)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL5BD;_tmp8C3=(_tmp8C2.KnownTunion).f1;_tmp8C4=*_tmp8C3;_LL5BC: problem=
Cyc_Absyn_qvar_cmp(_tmp8C4->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL5BA;
_LL5BD: _tmp8C5=_tmp8BE.f1;if((int)_tmp8C5 != 1)goto _LL5BF;_tmp8C6=_tmp8BE.f2;if(
_tmp8C6 <= (void*)4)goto _LL5BF;if(*((int*)_tmp8C6)!= 2)goto _LL5BF;_tmp8C7=((
struct Cyc_Absyn_TunionType_struct*)_tmp8C6)->f1;_tmp8C8=_tmp8C7.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8C6)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL5BF;_tmp8C9=(_tmp8C8.KnownTunion).f1;_tmp8CA=*_tmp8C9;_LL5BE: problem=
Cyc_Absyn_qvar_cmp(_tmp8CA->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL5BA;
_LL5BF:;_LL5C0: problem=1;goto _LL5BA;_LL5BA:;}if(problem)({void*_tmp8CB[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8CC="format attribute and vararg types don't match";
_tag_dynforward(_tmp8CC,sizeof(char),_get_zero_arr_size(_tmp8CC,46));}),
_tag_dynforward(_tmp8CB,sizeof(void*),0));});}}}{struct Cyc_List_List*rpo=_tmp7CC;
for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8CE;void*_tmp8CF;void*_tmp8D0;struct
_tuple0*_tmp8CD=(struct _tuple0*)rpo->hd;_tmp8CE=*_tmp8CD;_tmp8CF=_tmp8CE.f1;
_tmp8D0=_tmp8CE.f2;_tmp898=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp898,(void*)6,
_tmp8CF,1);_tmp898=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp898,(void*)3,_tmp8D0,
1);}}if(*_tmp7C7 != 0)_tmp898=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp898,(void*)
6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7C7))->v,1);else{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp898.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8D2;int _tmp8D3;struct _tuple18 _tmp8D1=*((struct _tuple18*)
tvs->hd);_tmp8D2=_tmp8D1.f1;_tmp8D3=_tmp8D1.f2;if(!_tmp8D3)continue;{void*
_tmp8D4=Cyc_Absyn_compress_kb((void*)_tmp8D2->kind);void*_tmp8D5;struct Cyc_Core_Opt*
_tmp8D6;struct Cyc_Core_Opt**_tmp8D7;void*_tmp8D8;struct Cyc_Core_Opt*_tmp8D9;
struct Cyc_Core_Opt**_tmp8DA;void*_tmp8DB;void*_tmp8DC;void*_tmp8DD;void*_tmp8DE;
struct Cyc_Core_Opt*_tmp8DF;struct Cyc_Core_Opt**_tmp8E0;void*_tmp8E1;void*_tmp8E2;
struct Cyc_Core_Opt*_tmp8E3;struct Cyc_Core_Opt**_tmp8E4;_LL5C2: if(*((int*)_tmp8D4)
!= 0)goto _LL5C4;_tmp8D5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8D4)->f1;if((
int)_tmp8D5 != 5)goto _LL5C4;_LL5C3:({void*_tmp8E5[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp8E6="TopRgnKind in toplevel effect!\n";_tag_dynforward(_tmp8E6,sizeof(char),
_get_zero_arr_size(_tmp8E6,32));}),_tag_dynforward(_tmp8E5,sizeof(void*),0));});
_LL5C4: if(*((int*)_tmp8D4)!= 2)goto _LL5C6;_tmp8D6=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D4)->f1;_tmp8D7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D4)->f1;_tmp8D8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8D4)->f2;if((
int)_tmp8D8 != 5)goto _LL5C6;_LL5C5: _tmp8DA=_tmp8D7;_tmp8DB=(void*)3;goto _LL5C7;
_LL5C6: if(*((int*)_tmp8D4)!= 2)goto _LL5C8;_tmp8D9=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D4)->f1;_tmp8DA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D4)->f1;_tmp8DB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8D4)->f2;if(!(
_tmp8DB == (void*)3  || _tmp8DB == (void*)4))goto _LL5C8;_LL5C7:*_tmp8DA=Cyc_Tcutil_kind_to_bound_opt(
_tmp8DB);_tmp8DC=_tmp8DB;goto _LL5C9;_LL5C8: if(*((int*)_tmp8D4)!= 0)goto _LL5CA;
_tmp8DC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8D4)->f1;if(!(_tmp8DC == (void*)
3  || _tmp8DC == (void*)4))goto _LL5CA;_LL5C9: effect=({struct Cyc_List_List*_tmp8E7=
_cycalloc(sizeof(*_tmp8E7));_tmp8E7->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp8E8=_cycalloc(sizeof(*_tmp8E8));_tmp8E8[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp8E9;_tmp8E9.tag=20;_tmp8E9.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8EA=_cycalloc(sizeof(*_tmp8EA));_tmp8EA[0]=({struct Cyc_Absyn_VarType_struct
_tmp8EB;_tmp8EB.tag=1;_tmp8EB.f1=_tmp8D2;_tmp8EB;});_tmp8EA;}));_tmp8E9;});
_tmp8E8;}));_tmp8E7->tl=effect;_tmp8E7;});goto _LL5C1;_LL5CA: if(*((int*)_tmp8D4)
!= 2)goto _LL5CC;_tmp8DD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8D4)->f2;
if((int)_tmp8DD != 7)goto _LL5CC;_LL5CB: goto _LL5CD;_LL5CC: if(*((int*)_tmp8D4)!= 0)
goto _LL5CE;_tmp8DE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8D4)->f1;if((int)
_tmp8DE != 7)goto _LL5CE;_LL5CD: goto _LL5C1;_LL5CE: if(*((int*)_tmp8D4)!= 2)goto
_LL5D0;_tmp8DF=((struct Cyc_Absyn_Less_kb_struct*)_tmp8D4)->f1;_tmp8E0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8D4)->f1;_tmp8E1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D4)->f2;if((int)_tmp8E1 != 6)goto _LL5D0;_LL5CF:*_tmp8E0=Cyc_Tcutil_kind_to_bound_opt((
void*)6);goto _LL5D1;_LL5D0: if(*((int*)_tmp8D4)!= 0)goto _LL5D2;_tmp8E2=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8D4)->f1;if((int)_tmp8E2 != 6)goto _LL5D2;_LL5D1:
effect=({struct Cyc_List_List*_tmp8EC=_cycalloc(sizeof(*_tmp8EC));_tmp8EC->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8ED=_cycalloc(sizeof(*_tmp8ED));
_tmp8ED[0]=({struct Cyc_Absyn_VarType_struct _tmp8EE;_tmp8EE.tag=1;_tmp8EE.f1=
_tmp8D2;_tmp8EE;});_tmp8ED;}));_tmp8EC->tl=effect;_tmp8EC;});goto _LL5C1;_LL5D2:
if(*((int*)_tmp8D4)!= 1)goto _LL5D4;_tmp8E3=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8D4)->f1;_tmp8E4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8D4)->f1;_LL5D3:*_tmp8E4=({struct Cyc_Core_Opt*_tmp8EF=_cycalloc(sizeof(*
_tmp8EF));_tmp8EF->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp8F0=
_cycalloc(sizeof(*_tmp8F0));_tmp8F0[0]=({struct Cyc_Absyn_Less_kb_struct _tmp8F1;
_tmp8F1.tag=2;_tmp8F1.f1=0;_tmp8F1.f2=(void*)((void*)0);_tmp8F1;});_tmp8F0;}));
_tmp8EF;});goto _LL5D5;_LL5D4:;_LL5D5: effect=({struct Cyc_List_List*_tmp8F2=
_cycalloc(sizeof(*_tmp8F2));_tmp8F2->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8F3=_cycalloc(sizeof(*_tmp8F3));_tmp8F3[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8F4;_tmp8F4.tag=22;_tmp8F4.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5[0]=({struct Cyc_Absyn_VarType_struct
_tmp8F6;_tmp8F6.tag=1;_tmp8F6.f1=_tmp8D2;_tmp8F6;});_tmp8F5;}));_tmp8F4;});
_tmp8F3;}));_tmp8F2->tl=effect;_tmp8F2;});goto _LL5C1;_LL5C1:;}}}{struct Cyc_List_List*
ts=_tmp898.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8F8;int _tmp8F9;struct
_tuple19 _tmp8F7=*((struct _tuple19*)ts->hd);_tmp8F8=_tmp8F7.f1;_tmp8F9=_tmp8F7.f2;
if(!_tmp8F9)continue;{void*_tmp8FA=Cyc_Tcutil_typ_kind(_tmp8F8);_LL5D7: if((int)
_tmp8FA != 5)goto _LL5D9;_LL5D8: goto _LL5DA;_LL5D9: if((int)_tmp8FA != 4)goto _LL5DB;
_LL5DA: goto _LL5DC;_LL5DB: if((int)_tmp8FA != 3)goto _LL5DD;_LL5DC: effect=({struct
Cyc_List_List*_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp8FC=_cycalloc(sizeof(*_tmp8FC));_tmp8FC[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp8FD;_tmp8FD.tag=20;_tmp8FD.f1=(void*)_tmp8F8;
_tmp8FD;});_tmp8FC;}));_tmp8FB->tl=effect;_tmp8FB;});goto _LL5D6;_LL5DD: if((int)
_tmp8FA != 6)goto _LL5DF;_LL5DE: effect=({struct Cyc_List_List*_tmp8FE=_cycalloc(
sizeof(*_tmp8FE));_tmp8FE->hd=(void*)_tmp8F8;_tmp8FE->tl=effect;_tmp8FE;});goto
_LL5D6;_LL5DF: if((int)_tmp8FA != 7)goto _LL5E1;_LL5E0: goto _LL5D6;_LL5E1:;_LL5E2:
effect=({struct Cyc_List_List*_tmp8FF=_cycalloc(sizeof(*_tmp8FF));_tmp8FF->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp900=_cycalloc(sizeof(*_tmp900));
_tmp900[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp901;_tmp901.tag=22;_tmp901.f1=(
void*)_tmp8F8;_tmp901;});_tmp900;}));_tmp8FF->tl=effect;_tmp8FF;});goto _LL5D6;
_LL5D6:;}}}*_tmp7C7=({struct Cyc_Core_Opt*_tmp902=_cycalloc(sizeof(*_tmp902));
_tmp902->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp903=_cycalloc(
sizeof(*_tmp903));_tmp903[0]=({struct Cyc_Absyn_JoinEff_struct _tmp904;_tmp904.tag=
21;_tmp904.f1=effect;_tmp904;});_tmp903;}));_tmp902;});}if(*_tmp7C5 != 0){struct
Cyc_List_List*bs=*_tmp7C5;for(0;bs != 0;bs=bs->tl){void*_tmp905=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp906;struct
Cyc_Core_Opt**_tmp907;struct Cyc_Core_Opt*_tmp908;struct Cyc_Core_Opt**_tmp909;
void*_tmp90A;struct Cyc_Core_Opt*_tmp90B;struct Cyc_Core_Opt**_tmp90C;void*_tmp90D;
struct Cyc_Core_Opt*_tmp90E;struct Cyc_Core_Opt**_tmp90F;void*_tmp910;struct Cyc_Core_Opt*
_tmp911;struct Cyc_Core_Opt**_tmp912;void*_tmp913;void*_tmp914;void*_tmp915;
_LL5E4: if(*((int*)_tmp905)!= 1)goto _LL5E6;_tmp906=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp905)->f1;_tmp907=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp905)->f1;_LL5E5:({struct Cyc_String_pa_struct _tmp918;_tmp918.tag=0;_tmp918.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);{
void*_tmp916[1]={& _tmp918};Cyc_Tcutil_warn(loc,({const char*_tmp917="Type variable %s unconstrained, assuming boxed";
_tag_dynforward(_tmp917,sizeof(char),_get_zero_arr_size(_tmp917,47));}),
_tag_dynforward(_tmp916,sizeof(void*),1));}});_tmp909=_tmp907;goto _LL5E7;_LL5E6:
if(*((int*)_tmp905)!= 2)goto _LL5E8;_tmp908=((struct Cyc_Absyn_Less_kb_struct*)
_tmp905)->f1;_tmp909=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp905)->f1;_tmp90A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp905)->f2;if((
int)_tmp90A != 1)goto _LL5E8;_LL5E7: _tmp90C=_tmp909;goto _LL5E9;_LL5E8: if(*((int*)
_tmp905)!= 2)goto _LL5EA;_tmp90B=((struct Cyc_Absyn_Less_kb_struct*)_tmp905)->f1;
_tmp90C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp905)->f1;
_tmp90D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp905)->f2;if((int)_tmp90D != 
0)goto _LL5EA;_LL5E9:*_tmp90C=Cyc_Tcutil_kind_to_bound_opt((void*)2);goto _LL5E3;
_LL5EA: if(*((int*)_tmp905)!= 2)goto _LL5EC;_tmp90E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp905)->f1;_tmp90F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp905)->f1;_tmp910=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp905)->f2;if((
int)_tmp910 != 5)goto _LL5EC;_LL5EB:*_tmp90F=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL5E3;_LL5EC: if(*((int*)_tmp905)!= 2)goto _LL5EE;_tmp911=((struct Cyc_Absyn_Less_kb_struct*)
_tmp905)->f1;_tmp912=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp905)->f1;_tmp913=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp905)->f2;
_LL5ED:*_tmp912=Cyc_Tcutil_kind_to_bound_opt(_tmp913);goto _LL5E3;_LL5EE: if(*((
int*)_tmp905)!= 0)goto _LL5F0;_tmp914=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp905)->f1;if((int)_tmp914 != 1)goto _LL5F0;_LL5EF:({void*_tmp919[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp91A="functions can't abstract M types";_tag_dynforward(
_tmp91A,sizeof(char),_get_zero_arr_size(_tmp91A,33));}),_tag_dynforward(_tmp919,
sizeof(void*),0));});goto _LL5E3;_LL5F0: if(*((int*)_tmp905)!= 0)goto _LL5F2;
_tmp915=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp905)->f1;if((int)_tmp915 != 5)
goto _LL5F2;_LL5F1:({void*_tmp91B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp91C="functions can't abstract TR types";
_tag_dynforward(_tmp91C,sizeof(char),_get_zero_arr_size(_tmp91C,34));}),
_tag_dynforward(_tmp91B,sizeof(void*),0));});goto _LL5E3;_LL5F2:;_LL5F3: goto
_LL5E3;_LL5E3:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,
_tmp898.kind_env,*_tmp7C5);_tmp898.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp898.r,_tmp898.free_vars,*_tmp7C5);{struct Cyc_List_List*tvs=_tmp898.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp91E;int _tmp91F;struct
_tuple18 _tmp91D=*((struct _tuple18*)tvs->hd);_tmp91E=_tmp91D.f1;_tmp91F=_tmp91D.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp91E,_tmp91F);}}{struct Cyc_List_List*evs=_tmp898.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp921;int _tmp922;struct _tuple19 _tmp920=*((struct _tuple19*)evs->hd);
_tmp921=_tmp920.f1;_tmp922=_tmp920.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp921,_tmp922);}}};_pop_region(newr);}goto _LL515;}
_LL534: if(*((int*)_tmp79B)!= 9)goto _LL536;_tmp7CE=((struct Cyc_Absyn_TupleType_struct*)
_tmp79B)->f1;_LL535: for(0;_tmp7CE != 0;_tmp7CE=_tmp7CE->tl){struct _tuple5*_tmp924=(
struct _tuple5*)_tmp7CE->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)1,(*_tmp924).f2,1);((*_tmp924).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp924).f1).print_const,(*_tmp924).f2);}goto _LL515;_LL536: if(*((int*)
_tmp79B)!= 11)goto _LL538;_tmp7CF=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp79B)->f1;_tmp7D0=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp79B)->f2;_LL537:{
struct _RegionHandle _tmp925=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp925;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp7D0 != 0;_tmp7D0=_tmp7D0->tl){struct Cyc_Absyn_Aggrfield _tmp927;struct
_dynforward_ptr*_tmp928;struct Cyc_Absyn_Tqual _tmp929;struct Cyc_Absyn_Tqual*
_tmp92A;void*_tmp92B;struct Cyc_Absyn_Exp*_tmp92C;struct Cyc_List_List*_tmp92D;
struct Cyc_Absyn_Aggrfield*_tmp926=(struct Cyc_Absyn_Aggrfield*)_tmp7D0->hd;
_tmp927=*_tmp926;_tmp928=_tmp927.name;_tmp929=_tmp927.tq;_tmp92A=(struct Cyc_Absyn_Tqual*)&(*
_tmp926).tq;_tmp92B=(void*)_tmp927.type;_tmp92C=_tmp927.width;_tmp92D=_tmp927.attributes;
if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct
Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,
_tmp928))({struct Cyc_String_pa_struct _tmp930;_tmp930.tag=0;_tmp930.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp928);{void*_tmp92E[1]={& _tmp930};
Cyc_Tcutil_terr(loc,({const char*_tmp92F="duplicate field %s";_tag_dynforward(
_tmp92F,sizeof(char),_get_zero_arr_size(_tmp92F,19));}),_tag_dynforward(_tmp92E,
sizeof(void*),1));}});if(Cyc_strcmp((struct _dynforward_ptr)*_tmp928,({const char*
_tmp931="";_tag_dynforward(_tmp931,sizeof(char),_get_zero_arr_size(_tmp931,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp932=_region_malloc(aprev_rgn,sizeof(*
_tmp932));_tmp932->hd=_tmp928;_tmp932->tl=prev_fields;_tmp932;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp92B,1);_tmp92A->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp92A->print_const,_tmp92B);if(_tmp7CF == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp92B))({struct Cyc_String_pa_struct _tmp935;_tmp935.tag=0;_tmp935.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp928);{void*_tmp933[1]={& _tmp935};
Cyc_Tcutil_warn(loc,({const char*_tmp934="union member %s is not `bits-only' so it can only be written and not read";
_tag_dynforward(_tmp934,sizeof(char),_get_zero_arr_size(_tmp934,74));}),
_tag_dynforward(_tmp933,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,
_tmp92B,_tmp92C,_tmp928);Cyc_Tcutil_check_field_atts(loc,_tmp928,_tmp92D);}};
_pop_region(aprev_rgn);}goto _LL515;_LL538: if(*((int*)_tmp79B)!= 10)goto _LL53A;
_tmp7D1=((struct Cyc_Absyn_AggrType_struct*)_tmp79B)->f1;_tmp7D2=_tmp7D1.aggr_info;
_tmp7D3=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp79B)->f1).aggr_info;_tmp7D4=_tmp7D1.targs;_tmp7D5=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp79B)->f1).targs;_LL539:{union Cyc_Absyn_AggrInfoU_union
_tmp936=*_tmp7D3;void*_tmp937;struct _tuple2*_tmp938;struct Cyc_Absyn_Aggrdecl**
_tmp939;struct Cyc_Absyn_Aggrdecl*_tmp93A;_LL5F5: if((_tmp936.UnknownAggr).tag != 0)
goto _LL5F7;_tmp937=(_tmp936.UnknownAggr).f1;_tmp938=(_tmp936.UnknownAggr).f2;
_LL5F6: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp93B;_push_handler(&
_tmp93B);{int _tmp93D=0;if(setjmp(_tmp93B.handler))_tmp93D=1;if(!_tmp93D){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp938);*_tmp7D3=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp93E;(_tmp93E.KnownAggr).tag=1;(_tmp93E.KnownAggr).f1=adp;_tmp93E;});;
_pop_handler();}else{void*_tmp93C=(void*)_exn_thrown;void*_tmp940=_tmp93C;_LL5FA:
if(_tmp940 != Cyc_Dict_Absent)goto _LL5FC;_LL5FB: {struct Cyc_Tcenv_Genv*_tmp941=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*_tmp942=({struct Cyc_Absyn_Aggrdecl*
_tmp947=_cycalloc(sizeof(*_tmp947));_tmp947->kind=(void*)_tmp937;_tmp947->sc=(
void*)((void*)3);_tmp947->name=_tmp938;_tmp947->tvs=0;_tmp947->impl=0;_tmp947->attributes=
0;_tmp947;});Cyc_Tc_tcAggrdecl(te,_tmp941,loc,_tmp942);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp938);*_tmp7D3=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp943;(_tmp943.KnownAggr).tag=1;(_tmp943.KnownAggr).f1=adp;_tmp943;});if(*
_tmp7D5 != 0){({struct Cyc_String_pa_struct _tmp946;_tmp946.tag=0;_tmp946.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp938));{
void*_tmp944[1]={& _tmp946};Cyc_Tcutil_terr(loc,({const char*_tmp945="declare parameterized type %s before using";
_tag_dynforward(_tmp945,sizeof(char),_get_zero_arr_size(_tmp945,43));}),
_tag_dynforward(_tmp944,sizeof(void*),1));}});return cvtenv;}goto _LL5F9;}_LL5FC:;
_LL5FD:(void)_throw(_tmp940);_LL5F9:;}}}_tmp93A=*adp;goto _LL5F8;}_LL5F7: if((
_tmp936.KnownAggr).tag != 1)goto _LL5F4;_tmp939=(_tmp936.KnownAggr).f1;_tmp93A=*
_tmp939;_LL5F8: {struct Cyc_List_List*tvs=_tmp93A->tvs;struct Cyc_List_List*ts=*
_tmp7D5;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);}if(ts != 0)({struct Cyc_String_pa_struct _tmp94A;_tmp94A.tag=
0;_tmp94A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp93A->name));{void*_tmp948[1]={& _tmp94A};Cyc_Tcutil_terr(loc,({const char*
_tmp949="too many parameters for type %s";_tag_dynforward(_tmp949,sizeof(char),
_get_zero_arr_size(_tmp949,32));}),_tag_dynforward(_tmp948,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp94B=_cycalloc(sizeof(*_tmp94B));_tmp94B->hd=(
void*)e;_tmp94B->tl=hidden_ts;_tmp94B;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmp7D5=Cyc_List_imp_append(*_tmp7D5,Cyc_List_imp_rev(
hidden_ts));}}_LL5F4:;}goto _LL515;_LL53A: if(*((int*)_tmp79B)!= 17)goto _LL53C;
_tmp7D6=((struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f1;_tmp7D7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp79B)->f2;_tmp7D8=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp79B)->f2;_tmp7D9=((struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f3;_tmp7DA=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f3;
_tmp7DB=((struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f4;_tmp7DC=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f4;_LL53B: {struct Cyc_List_List*
targs=*_tmp7D8;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp94C;
_push_handler(& _tmp94C);{int _tmp94E=0;if(setjmp(_tmp94C.handler))_tmp94E=1;if(!
_tmp94E){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7D6);;_pop_handler();}else{
void*_tmp94D=(void*)_exn_thrown;void*_tmp950=_tmp94D;_LL5FF: if(_tmp950 != Cyc_Dict_Absent)
goto _LL601;_LL600:({struct Cyc_String_pa_struct _tmp953;_tmp953.tag=0;_tmp953.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D6));{
void*_tmp951[1]={& _tmp953};Cyc_Tcutil_terr(loc,({const char*_tmp952="unbound typedef name %s";
_tag_dynforward(_tmp952,sizeof(char),_get_zero_arr_size(_tmp952,24));}),
_tag_dynforward(_tmp951,sizeof(void*),1));}});return cvtenv;_LL601:;_LL602:(void)
_throw(_tmp950);_LL5FE:;}}}*_tmp7DA=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7D6[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;{struct _RegionHandle _tmp954=_new_region("temp");
struct _RegionHandle*temp=& _tmp954;_push_region(temp);{struct Cyc_List_List*inst=0;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);inst=({struct Cyc_List_List*_tmp955=_region_malloc(temp,
sizeof(*_tmp955));_tmp955->hd=({struct _tuple8*_tmp956=_region_malloc(temp,
sizeof(*_tmp956));_tmp956->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp956->f2=(void*)
ts->hd;_tmp956;});_tmp955->tl=inst;_tmp955;});}if(ts != 0)({struct Cyc_String_pa_struct
_tmp959;_tmp959.tag=0;_tmp959.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7D6));{void*_tmp957[1]={& _tmp959};Cyc_Tcutil_terr(
loc,({const char*_tmp958="too many parameters for typedef %s";_tag_dynforward(
_tmp958,sizeof(char),_get_zero_arr_size(_tmp958,35));}),_tag_dynforward(_tmp957,
sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;
tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=
Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp95A=_cycalloc(
sizeof(*_tmp95A));_tmp95A->hd=(void*)e;_tmp95A->tl=hidden_ts;_tmp95A;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);inst=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp95B=_cycalloc(sizeof(*_tmp95B));_tmp95B->hd=(struct
_tuple8*)({struct _tuple8*_tmp95C=_cycalloc(sizeof(*_tmp95C));_tmp95C->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd;_tmp95C->f2=e;_tmp95C;});_tmp95B->tl=inst;_tmp95B;});}*
_tmp7D8=Cyc_List_imp_append(targs,Cyc_List_imp_rev(hidden_ts));}if(td->defn != 0){
void*new_typ=Cyc_Tcutil_rsubstitute(temp,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);*_tmp7DC=({void**_tmp95D=_cycalloc(sizeof(*_tmp95D));
_tmp95D[0]=new_typ;_tmp95D;});}};_pop_region(temp);}goto _LL515;}}_LL53C: if(*((
int*)_tmp79B)!= 19)goto _LL53E;_LL53D: goto _LL53F;_LL53E: if((int)_tmp79B != 3)goto
_LL540;_LL53F: goto _LL541;_LL540: if((int)_tmp79B != 2)goto _LL542;_LL541: goto _LL515;
_LL542: if(_tmp79B <= (void*)4)goto _LL544;if(*((int*)_tmp79B)!= 15)goto _LL544;
_tmp7DD=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp79B)->f1;_LL543:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp7DD,1);goto
_LL515;_LL544: if(_tmp79B <= (void*)4)goto _LL546;if(*((int*)_tmp79B)!= 16)goto
_LL546;_tmp7DE=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp79B)->f1;_tmp7DF=(
void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp79B)->f2;_LL545: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7DE,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp7DF,1);goto _LL515;_LL546: if(_tmp79B <= (void*)4)goto _LL548;
if(*((int*)_tmp79B)!= 20)goto _LL548;_tmp7E0=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp79B)->f1;_LL547: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp7E0,1);goto _LL515;_LL548: if(_tmp79B <= (void*)4)goto _LL54A;if(*((int*)_tmp79B)
!= 22)goto _LL54A;_tmp7E1=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp79B)->f1;
_LL549: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7E1,1);
goto _LL515;_LL54A: if(_tmp79B <= (void*)4)goto _LL515;if(*((int*)_tmp79B)!= 21)goto
_LL515;_tmp7E2=((struct Cyc_Absyn_JoinEff_struct*)_tmp79B)->f1;_LL54B: for(0;
_tmp7E2 != 0;_tmp7E2=_tmp7E2->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7E2->hd,1);}goto _LL515;_LL515:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp95E=t;struct Cyc_Core_Opt*
_tmp95F;struct Cyc_Core_Opt*_tmp960;_LL604: if(_tmp95E <= (void*)4)goto _LL606;if(*((
int*)_tmp95E)!= 0)goto _LL606;_tmp95F=((struct Cyc_Absyn_Evar_struct*)_tmp95E)->f1;
_tmp960=((struct Cyc_Absyn_Evar_struct*)_tmp95E)->f2;_LL605: {struct
_dynforward_ptr s;{struct Cyc_Core_Opt*_tmp961=_tmp95F;struct Cyc_Core_Opt _tmp962;
void*_tmp963;_LL609: if(_tmp961 != 0)goto _LL60B;_LL60A: s=({const char*_tmp964="kind=NULL ";
_tag_dynforward(_tmp964,sizeof(char),_get_zero_arr_size(_tmp964,11));});goto
_LL608;_LL60B: if(_tmp961 == 0)goto _LL608;_tmp962=*_tmp961;_tmp963=(void*)_tmp962.v;
_LL60C: s=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp967;_tmp967.tag=
0;_tmp967.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp963));{void*_tmp965[1]={& _tmp967};Cyc_aprintf(({const char*_tmp966="kind=%s ";
_tag_dynforward(_tmp966,sizeof(char),_get_zero_arr_size(_tmp966,9));}),
_tag_dynforward(_tmp965,sizeof(void*),1));}});goto _LL608;_LL608:;}{struct Cyc_Core_Opt*
_tmp968=_tmp960;struct Cyc_Core_Opt _tmp969;void*_tmp96A;_LL60E: if(_tmp968 != 0)
goto _LL610;_LL60F: s=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp96D;
_tmp96D.tag=0;_tmp96D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*
_tmp96B[1]={& _tmp96D};Cyc_aprintf(({const char*_tmp96C="%s ref=NULL";
_tag_dynforward(_tmp96C,sizeof(char),_get_zero_arr_size(_tmp96C,12));}),
_tag_dynforward(_tmp96B,sizeof(void*),1));}});goto _LL60D;_LL610: if(_tmp968 == 0)
goto _LL60D;_tmp969=*_tmp968;_tmp96A=(void*)_tmp969.v;_LL611: s=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp971;_tmp971.tag=0;_tmp971.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp96A));{
struct Cyc_String_pa_struct _tmp970;_tmp970.tag=0;_tmp970.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp96E[2]={& _tmp970,& _tmp971};
Cyc_aprintf(({const char*_tmp96F="%s ref=%s";_tag_dynforward(_tmp96F,sizeof(char),
_get_zero_arr_size(_tmp96F,10));}),_tag_dynforward(_tmp96E,sizeof(void*),2));}}});
goto _LL60D;_LL60D:;}({struct Cyc_String_pa_struct _tmp974;_tmp974.tag=0;_tmp974.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp972[1]={& _tmp974};Cyc_fprintf(
Cyc_stderr,({const char*_tmp973="evar info: %s\n";_tag_dynforward(_tmp973,sizeof(
char),_get_zero_arr_size(_tmp973,15));}),_tag_dynforward(_tmp972,sizeof(void*),1));}});
goto _LL603;}_LL606:;_LL607: goto _LL603;_LL603:;}({struct Cyc_String_pa_struct
_tmp979;_tmp979.tag=0;_tmp979.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(expected_kind));{struct Cyc_String_pa_struct _tmp978;
_tmp978.tag=0;_tmp978.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct _tmp977;_tmp977.tag=0;
_tmp977.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp975[3]={& _tmp977,& _tmp978,& _tmp979};Cyc_Tcutil_terr(loc,({const
char*_tmp976="type %s has kind %s but as used here needs kind %s";
_tag_dynforward(_tmp976,sizeof(char),_get_zero_arr_size(_tmp976,51));}),
_tag_dynforward(_tmp975,sizeof(void*),3));}}}});}return cvtenv;}static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvt,void*expected_kind,void*t){struct Cyc_List_List*
_tmp97A=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,
t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*
_tmp97C;struct _tuple18 _tmp97B=*((struct _tuple18*)vs->hd);_tmp97C=_tmp97B.f1;cvt.kind_env=
Cyc_Tcutil_fast_add_free_tvar(_tmp97A,_tmp97C);}}{struct Cyc_List_List*evs=cvt.free_evars;
for(0;evs != 0;evs=evs->tl){void*_tmp97E;struct _tuple19 _tmp97D=*((struct _tuple19*)
evs->hd);_tmp97E=_tmp97D.f1;{void*_tmp97F=Cyc_Tcutil_compress(_tmp97E);struct Cyc_Core_Opt*
_tmp980;struct Cyc_Core_Opt**_tmp981;_LL613: if(_tmp97F <= (void*)4)goto _LL615;if(*((
int*)_tmp97F)!= 0)goto _LL615;_tmp980=((struct Cyc_Absyn_Evar_struct*)_tmp97F)->f4;
_tmp981=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp97F)->f4;
_LL614: if(*_tmp981 == 0)*_tmp981=({struct Cyc_Core_Opt*_tmp982=_cycalloc(sizeof(*
_tmp982));_tmp982->v=_tmp97A;_tmp982;});else{struct Cyc_List_List*_tmp983=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp981))->v;struct Cyc_List_List*
_tmp984=0;for(0;_tmp983 != 0;_tmp983=_tmp983->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp97A,(struct Cyc_Absyn_Tvar*)_tmp983->hd))
_tmp984=({struct Cyc_List_List*_tmp985=_cycalloc(sizeof(*_tmp985));_tmp985->hd=(
struct Cyc_Absyn_Tvar*)_tmp983->hd;_tmp985->tl=_tmp984;_tmp985;});}*_tmp981=({
struct Cyc_Core_Opt*_tmp986=_cycalloc(sizeof(*_tmp986));_tmp986->v=_tmp984;
_tmp986;});}goto _LL612;_LL615:;_LL616: goto _LL612;_LL612:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmp987=Cyc_Tcenv_lookup_type_vars(
te);{struct _RegionHandle _tmp988=_new_region("temp");struct _RegionHandle*temp=&
_tmp988;_push_region(temp);{struct Cyc_Tcutil_CVTEnv _tmp989=Cyc_Tcutil_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9BD;_tmp9BD.r=temp;_tmp9BD.kind_env=_tmp987;
_tmp9BD.free_vars=0;_tmp9BD.free_evars=0;_tmp9BD.generalize_evars=
generalize_evars;_tmp9BD.fn_result=0;_tmp9BD;}),(void*)1,t);struct Cyc_List_List*
_tmp98A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp989.free_vars);struct Cyc_List_List*_tmp98B=((
struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),struct
Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp989.free_evars);if(_tmp987 != 0){struct Cyc_List_List*_tmp98C=0;{struct Cyc_List_List*
_tmp98D=_tmp98A;for(0;(unsigned int)_tmp98D;_tmp98D=_tmp98D->tl){struct Cyc_Absyn_Tvar*
_tmp98E=(struct Cyc_Absyn_Tvar*)_tmp98D->hd;int found=0;{struct Cyc_List_List*
_tmp98F=_tmp987;for(0;(unsigned int)_tmp98F;_tmp98F=_tmp98F->tl){if(Cyc_Absyn_tvar_cmp(
_tmp98E,(struct Cyc_Absyn_Tvar*)_tmp98F->hd)== 0){found=1;break;}}}if(!found)
_tmp98C=({struct Cyc_List_List*_tmp990=_region_malloc(temp,sizeof(*_tmp990));
_tmp990->hd=(struct Cyc_Absyn_Tvar*)_tmp98D->hd;_tmp990->tl=_tmp98C;_tmp990;});}}
_tmp98A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp98C);}{struct Cyc_List_List*x=_tmp98A;for(0;x != 0;x=x->tl){void*_tmp991=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp992;struct Cyc_Core_Opt**
_tmp993;struct Cyc_Core_Opt*_tmp994;struct Cyc_Core_Opt**_tmp995;void*_tmp996;
struct Cyc_Core_Opt*_tmp997;struct Cyc_Core_Opt**_tmp998;void*_tmp999;struct Cyc_Core_Opt*
_tmp99A;struct Cyc_Core_Opt**_tmp99B;void*_tmp99C;struct Cyc_Core_Opt*_tmp99D;
struct Cyc_Core_Opt**_tmp99E;void*_tmp99F;void*_tmp9A0;_LL618: if(*((int*)_tmp991)
!= 1)goto _LL61A;_tmp992=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp991)->f1;
_tmp993=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp991)->f1;
_LL619: _tmp995=_tmp993;goto _LL61B;_LL61A: if(*((int*)_tmp991)!= 2)goto _LL61C;
_tmp994=((struct Cyc_Absyn_Less_kb_struct*)_tmp991)->f1;_tmp995=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp991)->f1;_tmp996=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp991)->f2;if((int)_tmp996 != 1)goto _LL61C;_LL61B: _tmp998=_tmp995;goto _LL61D;
_LL61C: if(*((int*)_tmp991)!= 2)goto _LL61E;_tmp997=((struct Cyc_Absyn_Less_kb_struct*)
_tmp991)->f1;_tmp998=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp991)->f1;_tmp999=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp991)->f2;if((
int)_tmp999 != 0)goto _LL61E;_LL61D:*_tmp998=Cyc_Tcutil_kind_to_bound_opt((void*)2);
goto _LL617;_LL61E: if(*((int*)_tmp991)!= 2)goto _LL620;_tmp99A=((struct Cyc_Absyn_Less_kb_struct*)
_tmp991)->f1;_tmp99B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp991)->f1;_tmp99C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp991)->f2;if((
int)_tmp99C != 5)goto _LL620;_LL61F:*_tmp99B=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL617;_LL620: if(*((int*)_tmp991)!= 2)goto _LL622;_tmp99D=((struct Cyc_Absyn_Less_kb_struct*)
_tmp991)->f1;_tmp99E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp991)->f1;_tmp99F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp991)->f2;
_LL621:*_tmp99E=Cyc_Tcutil_kind_to_bound_opt(_tmp99F);goto _LL617;_LL622: if(*((
int*)_tmp991)!= 0)goto _LL624;_tmp9A0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp991)->f1;if((int)_tmp9A0 != 1)goto _LL624;_LL623:({struct Cyc_String_pa_struct
_tmp9A3;_tmp9A3.tag=0;_tmp9A3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp9A1[1]={& _tmp9A3};
Cyc_Tcutil_terr(loc,({const char*_tmp9A2="type variable %s cannot have kind M";
_tag_dynforward(_tmp9A2,sizeof(char),_get_zero_arr_size(_tmp9A2,36));}),
_tag_dynforward(_tmp9A1,sizeof(void*),1));}});goto _LL617;_LL624:;_LL625: goto
_LL617;_LL617:;}}if(_tmp98A != 0  || _tmp98B != 0){{void*_tmp9A4=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp9A5;struct Cyc_List_List*_tmp9A6;struct Cyc_List_List**
_tmp9A7;struct Cyc_Core_Opt*_tmp9A8;void*_tmp9A9;struct Cyc_List_List*_tmp9AA;int
_tmp9AB;struct Cyc_Absyn_VarargInfo*_tmp9AC;struct Cyc_List_List*_tmp9AD;struct Cyc_List_List*
_tmp9AE;_LL627: if(_tmp9A4 <= (void*)4)goto _LL629;if(*((int*)_tmp9A4)!= 8)goto
_LL629;_tmp9A5=((struct Cyc_Absyn_FnType_struct*)_tmp9A4)->f1;_tmp9A6=_tmp9A5.tvars;
_tmp9A7=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp9A4)->f1).tvars;
_tmp9A8=_tmp9A5.effect;_tmp9A9=(void*)_tmp9A5.ret_typ;_tmp9AA=_tmp9A5.args;
_tmp9AB=_tmp9A5.c_varargs;_tmp9AC=_tmp9A5.cyc_varargs;_tmp9AD=_tmp9A5.rgn_po;
_tmp9AE=_tmp9A5.attributes;_LL628: if(*_tmp9A7 == 0){*_tmp9A7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmp98A);_tmp98A=0;}goto _LL626;_LL629:;
_LL62A: goto _LL626;_LL626:;}if(_tmp98A != 0)({struct Cyc_String_pa_struct _tmp9B1;
_tmp9B1.tag=0;_tmp9B1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)_tmp98A->hd)->name);{void*_tmp9AF[1]={& _tmp9B1};Cyc_Tcutil_terr(
loc,({const char*_tmp9B0="unbound type variable %s";_tag_dynforward(_tmp9B0,
sizeof(char),_get_zero_arr_size(_tmp9B0,25));}),_tag_dynforward(_tmp9AF,sizeof(
void*),1));}});if(_tmp98B != 0)for(0;_tmp98B != 0;_tmp98B=_tmp98B->tl){void*e=(
void*)_tmp98B->hd;void*_tmp9B2=Cyc_Tcutil_typ_kind(e);_LL62C: if((int)_tmp9B2 != 4)
goto _LL62E;_LL62D: if(!Cyc_Tcutil_unify(e,(void*)3))({void*_tmp9B3[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9B4="can't unify evar with unique region!";_tag_dynforward(_tmp9B4,sizeof(
char),_get_zero_arr_size(_tmp9B4,37));}),_tag_dynforward(_tmp9B3,sizeof(void*),0));});
goto _LL62B;_LL62E: if((int)_tmp9B2 != 5)goto _LL630;_LL62F: goto _LL631;_LL630: if((
int)_tmp9B2 != 3)goto _LL632;_LL631: if(!Cyc_Tcutil_unify(e,(void*)2))({void*
_tmp9B5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9B6="can't unify evar with heap!";_tag_dynforward(_tmp9B6,sizeof(
char),_get_zero_arr_size(_tmp9B6,28));}),_tag_dynforward(_tmp9B5,sizeof(void*),0));});
goto _LL62B;_LL632: if((int)_tmp9B2 != 6)goto _LL634;_LL633: if(!Cyc_Tcutil_unify(e,
Cyc_Absyn_empty_effect))({void*_tmp9B7[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9B8="can't unify evar with {}!";
_tag_dynforward(_tmp9B8,sizeof(char),_get_zero_arr_size(_tmp9B8,26));}),
_tag_dynforward(_tmp9B7,sizeof(void*),0));});goto _LL62B;_LL634:;_LL635:({struct
Cyc_String_pa_struct _tmp9BC;_tmp9BC.tag=0;_tmp9BC.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp9BB;_tmp9BB.tag=0;_tmp9BB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp9B9[2]={& _tmp9BB,& _tmp9BC};Cyc_Tcutil_terr(
loc,({const char*_tmp9BA="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp9BA,sizeof(char),_get_zero_arr_size(_tmp9BA,52));}),
_tag_dynforward(_tmp9B9,sizeof(void*),2));}}});goto _LL62B;_LL62B:;}}};
_pop_region(temp);}if(Cyc_Tcutil_is_unique_pointer(t))({void*_tmp9BE[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp9BF="unique pointers not permitted at the toplevel";
_tag_dynforward(_tmp9BF,sizeof(char),_get_zero_arr_size(_tmp9BF,46));}),
_tag_dynforward(_tmp9BE,sizeof(void*),0));});}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp9C0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp9C1;struct Cyc_List_List*
_tmp9C2;struct Cyc_Core_Opt*_tmp9C3;void*_tmp9C4;_LL637: if(_tmp9C0 <= (void*)4)
goto _LL639;if(*((int*)_tmp9C0)!= 8)goto _LL639;_tmp9C1=((struct Cyc_Absyn_FnType_struct*)
_tmp9C0)->f1;_tmp9C2=_tmp9C1.tvars;_tmp9C3=_tmp9C1.effect;_tmp9C4=(void*)_tmp9C1.ret_typ;
_LL638: fd->tvs=_tmp9C2;fd->effect=_tmp9C3;goto _LL636;_LL639:;_LL63A:({void*
_tmp9C5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9C6="check_fndecl_valid_type: not a FnType";_tag_dynforward(
_tmp9C6,sizeof(char),_get_zero_arr_size(_tmp9C6,38));}),_tag_dynforward(_tmp9C5,
sizeof(void*),0));});_LL636:;}{struct _RegionHandle _tmp9C7=_new_region("r");
struct _RegionHandle*r=& _tmp9C7;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr*(*f)(struct
_tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dynforward_ptr*(*)(
struct _tuple14*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9C8="function declaration has repeated parameter";
_tag_dynforward(_tmp9C8,sizeof(char),_get_zero_arr_size(_tmp9C8,44));}));;
_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp9C9=_cycalloc(sizeof(*
_tmp9C9));_tmp9C9->v=(void*)t;_tmp9C9;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,
int allow_evars,void*t){struct _RegionHandle _tmp9CA=_new_region("r");struct
_RegionHandle*r=& _tmp9CA;_push_region(r);{struct Cyc_Tcutil_CVTEnv _tmp9CB=Cyc_Tcutil_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9DE;_tmp9DE.r=r;_tmp9DE.kind_env=bound_tvars;
_tmp9DE.free_vars=0;_tmp9DE.free_evars=0;_tmp9DE.generalize_evars=0;_tmp9DE.fn_result=
0;_tmp9DE;}),expected_kind,t);struct Cyc_List_List*_tmp9CC=Cyc_Tcutil_remove_bound_tvars(
r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp9CB.free_vars),bound_tvars);struct Cyc_List_List*
_tmp9CD=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp9CB.free_evars);{struct Cyc_List_List*fs=_tmp9CC;for(0;fs != 0;fs=fs->tl){
struct _dynforward_ptr*_tmp9CE=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct
_tmp9D2;_tmp9D2.tag=0;_tmp9D2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp9D1;_tmp9D1.tag=0;
_tmp9D1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp9CE);{void*
_tmp9CF[2]={& _tmp9D1,& _tmp9D2};Cyc_Tcutil_terr(loc,({const char*_tmp9D0="unbound type variable %s in type %s";
_tag_dynforward(_tmp9D0,sizeof(char),_get_zero_arr_size(_tmp9D0,36));}),
_tag_dynforward(_tmp9CF,sizeof(void*),2));}}});}}if(!allow_evars  && _tmp9CD != 0)
for(0;_tmp9CD != 0;_tmp9CD=_tmp9CD->tl){void*e=(void*)_tmp9CD->hd;void*_tmp9D3=
Cyc_Tcutil_typ_kind(e);_LL63C: if((int)_tmp9D3 != 4)goto _LL63E;_LL63D: if(!Cyc_Tcutil_unify(
e,(void*)3))({void*_tmp9D4[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9D5="can't unify evar with unique region!";
_tag_dynforward(_tmp9D5,sizeof(char),_get_zero_arr_size(_tmp9D5,37));}),
_tag_dynforward(_tmp9D4,sizeof(void*),0));});goto _LL63B;_LL63E: if((int)_tmp9D3 != 
5)goto _LL640;_LL63F: goto _LL641;_LL640: if((int)_tmp9D3 != 3)goto _LL642;_LL641: if(!
Cyc_Tcutil_unify(e,(void*)2))({void*_tmp9D6[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9D7="can't unify evar with heap!";
_tag_dynforward(_tmp9D7,sizeof(char),_get_zero_arr_size(_tmp9D7,28));}),
_tag_dynforward(_tmp9D6,sizeof(void*),0));});goto _LL63B;_LL642: if((int)_tmp9D3 != 
6)goto _LL644;_LL643: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*
_tmp9D8[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9D9="can't unify evar with {}!";_tag_dynforward(_tmp9D9,sizeof(
char),_get_zero_arr_size(_tmp9D9,26));}),_tag_dynforward(_tmp9D8,sizeof(void*),0));});
goto _LL63B;_LL644:;_LL645:({struct Cyc_String_pa_struct _tmp9DD;_tmp9DD.tag=0;
_tmp9DD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp9DC;_tmp9DC.tag=0;_tmp9DC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(e));{void*_tmp9DA[
2]={& _tmp9DC,& _tmp9DD};Cyc_Tcutil_terr(loc,({const char*_tmp9DB="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp9DB,sizeof(char),_get_zero_arr_size(_tmp9DB,52));}),
_tag_dynforward(_tmp9DA,sizeof(void*),2));}}});goto _LL63B;_LL63B:;}};_pop_region(
r);}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 
- 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dynforward_ptr(*a2string)(void*),struct _dynforward_ptr msg){for(0;vs != 0;
vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((
void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct _tmp9E2;_tmp9E2.tag=
0;_tmp9E2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)a2string((void*)vs->hd));{
struct Cyc_String_pa_struct _tmp9E1;_tmp9E1.tag=0;_tmp9E1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)msg);{void*_tmp9DF[2]={& _tmp9E1,& _tmp9E2};
Cyc_Tcutil_terr(loc,({const char*_tmp9E0="%s: %s";_tag_dynforward(_tmp9E0,sizeof(
char),_get_zero_arr_size(_tmp9E0,7));}),_tag_dynforward(_tmp9DF,sizeof(void*),2));}}});}}}
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
Cyc_Tcutil_tvar2string,({const char*_tmp9E3="duplicate type variable";
_tag_dynforward(_tmp9E3,sizeof(char),_get_zero_arr_size(_tmp9E3,24));}));}struct
_tuple20{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple21{struct Cyc_List_List*
f1;void*f2;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,({const char*_tmp9E4="";_tag_dynforward(_tmp9E4,sizeof(char),
_get_zero_arr_size(_tmp9E4,1));}))!= 0)fields=({struct Cyc_List_List*_tmp9E5=
_cycalloc(sizeof(*_tmp9E5));_tmp9E5->hd=({struct _tuple20*_tmp9E6=_cycalloc(
sizeof(*_tmp9E6));_tmp9E6->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp9E6->f2=
0;_tmp9E6;});_tmp9E5->tl=fields;_tmp9E5;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple21 _tmp9E8;struct Cyc_List_List*_tmp9E9;void*
_tmp9EA;struct _tuple21*_tmp9E7=(struct _tuple21*)des->hd;_tmp9E8=*_tmp9E7;_tmp9E9=
_tmp9E8.f1;_tmp9EA=_tmp9E8.f2;if(_tmp9E9 == 0){struct Cyc_List_List*_tmp9EB=fields;
for(0;_tmp9EB != 0;_tmp9EB=_tmp9EB->tl){if(!(*((struct _tuple20*)_tmp9EB->hd)).f2){(*((
struct _tuple20*)_tmp9EB->hd)).f2=1;(*((struct _tuple21*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp9EC=_cycalloc(sizeof(*_tmp9EC));_tmp9EC->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp9ED=_cycalloc(sizeof(*_tmp9ED));
_tmp9ED[0]=({struct Cyc_Absyn_FieldName_struct _tmp9EE;_tmp9EE.tag=1;_tmp9EE.f1=((*((
struct _tuple20*)_tmp9EB->hd)).f1)->name;_tmp9EE;});_tmp9ED;}));_tmp9EC->tl=0;
_tmp9EC;});ans=({struct Cyc_List_List*_tmp9EF=_region_malloc(rgn,sizeof(*_tmp9EF));
_tmp9EF->hd=({struct _tuple22*_tmp9F0=_region_malloc(rgn,sizeof(*_tmp9F0));
_tmp9F0->f1=(*((struct _tuple20*)_tmp9EB->hd)).f1;_tmp9F0->f2=_tmp9EA;_tmp9F0;});
_tmp9EF->tl=ans;_tmp9EF;});break;}}if(_tmp9EB == 0)({void*_tmp9F1[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp9F2="too many arguments to struct";_tag_dynforward(_tmp9F2,
sizeof(char),_get_zero_arr_size(_tmp9F2,29));}),_tag_dynforward(_tmp9F1,sizeof(
void*),0));});}else{if(_tmp9E9->tl != 0)({void*_tmp9F3[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp9F4="multiple designators are not supported";_tag_dynforward(
_tmp9F4,sizeof(char),_get_zero_arr_size(_tmp9F4,39));}),_tag_dynforward(_tmp9F3,
sizeof(void*),0));});else{void*_tmp9F5=(void*)_tmp9E9->hd;struct _dynforward_ptr*
_tmp9F6;_LL647: if(*((int*)_tmp9F5)!= 0)goto _LL649;_LL648:({void*_tmp9F7[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp9F8="array designator used in argument to struct";
_tag_dynforward(_tmp9F8,sizeof(char),_get_zero_arr_size(_tmp9F8,44));}),
_tag_dynforward(_tmp9F7,sizeof(void*),0));});goto _LL646;_LL649: if(*((int*)
_tmp9F5)!= 1)goto _LL646;_tmp9F6=((struct Cyc_Absyn_FieldName_struct*)_tmp9F5)->f1;
_LL64A: {struct Cyc_List_List*_tmp9F9=fields;for(0;_tmp9F9 != 0;_tmp9F9=_tmp9F9->tl){
if(Cyc_strptrcmp(_tmp9F6,((*((struct _tuple20*)_tmp9F9->hd)).f1)->name)== 0){if((*((
struct _tuple20*)_tmp9F9->hd)).f2)({struct Cyc_String_pa_struct _tmp9FC;_tmp9FC.tag=
0;_tmp9FC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp9F6);{void*
_tmp9FA[1]={& _tmp9FC};Cyc_Tcutil_terr(loc,({const char*_tmp9FB="field %s has already been used as an argument";
_tag_dynforward(_tmp9FB,sizeof(char),_get_zero_arr_size(_tmp9FB,46));}),
_tag_dynforward(_tmp9FA,sizeof(void*),1));}});(*((struct _tuple20*)_tmp9F9->hd)).f2=
1;ans=({struct Cyc_List_List*_tmp9FD=_region_malloc(rgn,sizeof(*_tmp9FD));_tmp9FD->hd=({
struct _tuple22*_tmp9FE=_region_malloc(rgn,sizeof(*_tmp9FE));_tmp9FE->f1=(*((
struct _tuple20*)_tmp9F9->hd)).f1;_tmp9FE->f2=_tmp9EA;_tmp9FE;});_tmp9FD->tl=ans;
_tmp9FD;});break;}}if(_tmp9F9 == 0)({struct Cyc_String_pa_struct _tmpA01;_tmpA01.tag=
0;_tmpA01.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp9F6);{void*
_tmp9FF[1]={& _tmpA01};Cyc_Tcutil_terr(loc,({const char*_tmpA00="bad field designator %s";
_tag_dynforward(_tmpA00,sizeof(char),_get_zero_arr_size(_tmpA00,24));}),
_tag_dynforward(_tmp9FF,sizeof(void*),1));}});goto _LL646;}_LL646:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){({void*
_tmpA02[0]={};Cyc_Tcutil_terr(loc,({const char*_tmpA03="too few arguments to struct";
_tag_dynforward(_tmpA03,sizeof(char),_get_zero_arr_size(_tmpA03,28));}),
_tag_dynforward(_tmpA02,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only){*forward_only=0;{void*_tmpA04=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA05;void*_tmpA06;struct Cyc_Absyn_PtrAtts _tmpA07;
struct Cyc_Absyn_Conref*_tmpA08;_LL64C: if(_tmpA04 <= (void*)4)goto _LL64E;if(*((int*)
_tmpA04)!= 4)goto _LL64E;_tmpA05=((struct Cyc_Absyn_PointerType_struct*)_tmpA04)->f1;
_tmpA06=(void*)_tmpA05.elt_typ;_tmpA07=_tmpA05.ptr_atts;_tmpA08=_tmpA07.bounds;
_LL64D: {struct Cyc_Absyn_Conref*_tmpA09=Cyc_Absyn_compress_conref(_tmpA08);union
Cyc_Absyn_Constraint_union _tmpA0A=(Cyc_Absyn_compress_conref(_tmpA09))->v;void*
_tmpA0B;void*_tmpA0C;_LL651: if((_tmpA0A.Eq_constr).tag != 0)goto _LL653;_tmpA0B=(
_tmpA0A.Eq_constr).f1;if((int)_tmpA0B != 0)goto _LL653;_LL652:*forward_only=1;*
elt_typ_dest=_tmpA06;return 1;_LL653: if((_tmpA0A.Eq_constr).tag != 0)goto _LL655;
_tmpA0C=(_tmpA0A.Eq_constr).f1;if((int)_tmpA0C != 1)goto _LL655;_LL654:*
elt_typ_dest=_tmpA06;return 1;_LL655: if((_tmpA0A.No_constr).tag != 2)goto _LL657;
_LL656: _tmpA09->v=(union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union
_tmpA0D;(_tmpA0D.Eq_constr).tag=0;(_tmpA0D.Eq_constr).f1=(void*)((void*)0);
_tmpA0D;});*forward_only=1;*elt_typ_dest=_tmpA06;return 1;_LL657:;_LL658: return 0;
_LL650:;}_LL64E:;_LL64F: return 0;_LL64B:;}}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpA0E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpA0F;void*_tmpA10;struct Cyc_Absyn_PtrAtts _tmpA11;struct Cyc_Absyn_Conref*
_tmpA12;_LL65A: if(_tmpA0E <= (void*)4)goto _LL65C;if(*((int*)_tmpA0E)!= 4)goto
_LL65C;_tmpA0F=((struct Cyc_Absyn_PointerType_struct*)_tmpA0E)->f1;_tmpA10=(void*)
_tmpA0F.elt_typ;_tmpA11=_tmpA0F.ptr_atts;_tmpA12=_tmpA11.zero_term;_LL65B:*
elt_typ_dest=_tmpA10;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA12);_LL65C:;_LL65D: return 0;_LL659:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmpA13=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA14;void*_tmpA15;struct Cyc_Absyn_PtrAtts
_tmpA16;struct Cyc_Absyn_Conref*_tmpA17;struct Cyc_Absyn_Conref*_tmpA18;struct Cyc_Absyn_ArrayInfo
_tmpA19;void*_tmpA1A;struct Cyc_Absyn_Tqual _tmpA1B;struct Cyc_Absyn_Exp*_tmpA1C;
struct Cyc_Absyn_Conref*_tmpA1D;_LL65F: if(_tmpA13 <= (void*)4)goto _LL663;if(*((int*)
_tmpA13)!= 4)goto _LL661;_tmpA14=((struct Cyc_Absyn_PointerType_struct*)_tmpA13)->f1;
_tmpA15=(void*)_tmpA14.elt_typ;_tmpA16=_tmpA14.ptr_atts;_tmpA17=_tmpA16.bounds;
_tmpA18=_tmpA16.zero_term;_LL660: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA18)){*ptr_type=t;*elt_type=_tmpA15;{void*_tmpA1E=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpA17);_LL666: if((int)_tmpA1E != 0)goto _LL668;_LL667:*is_dyneither=0;*
is_dynforward=1;goto _LL665;_LL668: if((int)_tmpA1E != 1)goto _LL66A;_LL669:*
is_dynforward=0;*is_dyneither=1;goto _LL665;_LL66A:;_LL66B:*is_dynforward=(*
is_dyneither=0);goto _LL665;_LL665:;}return 1;}else{return 0;}_LL661: if(*((int*)
_tmpA13)!= 7)goto _LL663;_tmpA19=((struct Cyc_Absyn_ArrayType_struct*)_tmpA13)->f1;
_tmpA1A=(void*)_tmpA19.elt_type;_tmpA1B=_tmpA19.tq;_tmpA1C=_tmpA19.num_elts;
_tmpA1D=_tmpA19.zero_term;_LL662: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA1D)){*elt_type=_tmpA1A;*is_dynforward=(*is_dyneither=0);*ptr_type=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmpA1F=_cycalloc(sizeof(*_tmpA1F));_tmpA1F[0]=({
struct Cyc_Absyn_PointerType_struct _tmpA20;_tmpA20.tag=4;_tmpA20.f1=({struct Cyc_Absyn_PtrInfo
_tmpA21;_tmpA21.elt_typ=(void*)_tmpA1A;_tmpA21.elt_tq=_tmpA1B;_tmpA21.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpA22;_tmpA22.rgn=(void*)((void*)2);_tmpA22.nullable=
Cyc_Absyn_false_conref;_tmpA22.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmpA23=_cycalloc(sizeof(*_tmpA23));_tmpA23[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpA24;_tmpA24.tag=0;_tmpA24.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA1C);
_tmpA24;});_tmpA23;}));_tmpA22.zero_term=_tmpA1D;_tmpA22.ptrloc=0;_tmpA22;});
_tmpA21;});_tmpA20;});_tmpA1F;});return 1;}else{return 0;}_LL663:;_LL664: return 0;
_LL65E:;}int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,
int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmpA25=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmpA26;struct Cyc_Absyn_Exp*_tmpA27;struct Cyc_Absyn_Exp*
_tmpA28;struct Cyc_Absyn_Exp*_tmpA29;struct Cyc_Absyn_Exp*_tmpA2A;struct Cyc_Absyn_Exp*
_tmpA2B;_LL66D: if(*((int*)_tmpA25)!= 15)goto _LL66F;_LL66E:({struct Cyc_String_pa_struct
_tmpA2E;_tmpA2E.tag=0;_tmpA2E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmpA2C[1]={& _tmpA2E};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA2D="we have a cast in a lhs:  %s";_tag_dynforward(_tmpA2D,sizeof(char),
_get_zero_arr_size(_tmpA2D,29));}),_tag_dynforward(_tmpA2C,sizeof(void*),1));}});
_LL66F: if(*((int*)_tmpA25)!= 22)goto _LL671;_tmpA26=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA25)->f1;_LL670: _tmpA27=_tmpA26;goto _LL672;_LL671: if(*((int*)_tmpA25)!= 25)
goto _LL673;_tmpA27=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA25)->f1;_LL672:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA27->topt))->v,ptr_type,is_dynforward,is_dyneither,elt_type);_LL673: if(*((
int*)_tmpA25)!= 24)goto _LL675;_tmpA28=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpA25)->f1;_LL674: _tmpA29=_tmpA28;goto _LL676;_LL675: if(*((int*)_tmpA25)!= 23)
goto _LL677;_tmpA29=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA25)->f1;_LL676:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA29->topt))->v,
ptr_type,is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct
_tmpA31;_tmpA31.tag=0;_tmpA31.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmpA2F[1]={& _tmpA31};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA30="found zero pointer aggregate member assignment: %s";_tag_dynforward(
_tmpA30,sizeof(char),_get_zero_arr_size(_tmpA30,51));}),_tag_dynforward(_tmpA2F,
sizeof(void*),1));}});return 0;_LL677: if(*((int*)_tmpA25)!= 14)goto _LL679;_tmpA2A=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmpA25)->f1;_LL678: _tmpA2B=_tmpA2A;goto
_LL67A;_LL679: if(*((int*)_tmpA25)!= 13)goto _LL67B;_tmpA2B=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpA25)->f1;_LL67A: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA2B->topt))->v,ptr_type,is_dynforward,is_dyneither,elt_type))({
struct Cyc_String_pa_struct _tmpA34;_tmpA34.tag=0;_tmpA34.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e1));{void*
_tmpA32[1]={& _tmpA34};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA33="found zero pointer instantiate/noinstantiate: %s";
_tag_dynforward(_tmpA33,sizeof(char),_get_zero_arr_size(_tmpA33,49));}),
_tag_dynforward(_tmpA32,sizeof(void*),1));}});return 0;_LL67B: if(*((int*)_tmpA25)
!= 1)goto _LL67D;_LL67C: return 0;_LL67D:;_LL67E:({struct Cyc_String_pa_struct
_tmpA37;_tmpA37.tag=0;_tmpA37.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmpA35[1]={& _tmpA37};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA36="found bad lhs in is_zero_ptr_deref: %s";_tag_dynforward(_tmpA36,sizeof(
char),_get_zero_arr_size(_tmpA36,39));}),_tag_dynforward(_tmpA35,sizeof(void*),1));}});
_LL66C:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t,int*is_forward_only){void*
ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore,
is_forward_only);}static int Cyc_Tcutil_is_unique_region(void*r){void*_tmpA38=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpA39;_LL680: if((int)_tmpA38 != 3)goto _LL682;_LL681:
return 1;_LL682: if(_tmpA38 <= (void*)4)goto _LL684;if(*((int*)_tmpA38)!= 1)goto
_LL684;_tmpA39=((struct Cyc_Absyn_VarType_struct*)_tmpA38)->f1;_LL683: return Cyc_Tcutil_tvar_kind(
_tmpA39)== (void*)4;_LL684:;_LL685: return 0;_LL67F:;}int Cyc_Tcutil_is_unique_pointer(
void*t){void*_tmpA3A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA3B;
struct Cyc_Absyn_PtrAtts _tmpA3C;void*_tmpA3D;_LL687: if(_tmpA3A <= (void*)4)goto
_LL689;if(*((int*)_tmpA3A)!= 4)goto _LL689;_tmpA3B=((struct Cyc_Absyn_PointerType_struct*)
_tmpA3A)->f1;_tmpA3C=_tmpA3B.ptr_atts;_tmpA3D=(void*)_tmpA3C.rgn;_LL688: return
Cyc_Tcutil_is_unique_region(_tmpA3D);_LL689:;_LL68A: return 0;_LL686:;}static int
Cyc_Tcutil_is_unique_field(void*t,struct _dynforward_ptr*f){void*_tmpA3E=t;struct
Cyc_Absyn_AggrInfo _tmpA3F;union Cyc_Absyn_AggrInfoU_union _tmpA40;struct Cyc_Absyn_Aggrdecl**
_tmpA41;struct Cyc_Absyn_Aggrdecl*_tmpA42;struct Cyc_List_List*_tmpA43;_LL68C: if(
_tmpA3E <= (void*)4)goto _LL68E;if(*((int*)_tmpA3E)!= 10)goto _LL68E;_tmpA3F=((
struct Cyc_Absyn_AggrType_struct*)_tmpA3E)->f1;_tmpA40=_tmpA3F.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpA3E)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL68E;_tmpA41=(_tmpA40.KnownAggr).f1;_tmpA42=*_tmpA41;_tmpA43=_tmpA3F.targs;
_LL68D: {struct Cyc_Absyn_Aggrfield*_tmpA44=Cyc_Absyn_lookup_decl_field(_tmpA42,f);
struct Cyc_Absyn_Aggrfield*_tmpA45=_tmpA44;struct Cyc_Absyn_Aggrfield _tmpA46;void*
_tmpA47;_LL691: if(_tmpA45 != 0)goto _LL693;_LL692:({void*_tmpA48[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA49="is_unique_field: missing field";_tag_dynforward(_tmpA49,sizeof(char),
_get_zero_arr_size(_tmpA49,31));}),_tag_dynforward(_tmpA48,sizeof(void*),0));});
_LL693: if(_tmpA45 == 0)goto _LL690;_tmpA46=*_tmpA45;_tmpA47=(void*)_tmpA46.type;
_LL694: return Cyc_Tcutil_is_unique_pointer(_tmpA47);_LL690:;}_LL68E:;_LL68F:({
void*_tmpA4A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA4B="is_unique_field: invalid type";
_tag_dynforward(_tmpA4B,sizeof(char),_get_zero_arr_size(_tmpA4B,30));}),
_tag_dynforward(_tmpA4A,sizeof(void*),0));});_LL68B:;}static int Cyc_Tcutil_is_unique_path_aux(
struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpA4C=(void*)e->r;struct Cyc_Absyn_Exp*
_tmpA4D;struct Cyc_Absyn_Exp*_tmpA4E;struct Cyc_Absyn_Exp*_tmpA4F;struct Cyc_Absyn_Exp*
_tmpA50;void*_tmpA51;struct Cyc_Absyn_Exp*_tmpA52;struct Cyc_Absyn_Stmt*_tmpA53;
_LL696: if(*((int*)_tmpA4C)!= 5)goto _LL698;_LL697: goto _LL699;_LL698: if(*((int*)
_tmpA4C)!= 7)goto _LL69A;_LL699: goto _LL69B;_LL69A: if(*((int*)_tmpA4C)!= 8)goto
_LL69C;_LL69B: goto _LL69D;_LL69C: if(*((int*)_tmpA4C)!= 12)goto _LL69E;_LL69D: goto
_LL69F;_LL69E: if(*((int*)_tmpA4C)!= 16)goto _LL6A0;_LL69F: goto _LL6A1;_LL6A0: if(*((
int*)_tmpA4C)!= 18)goto _LL6A2;_LL6A1: goto _LL6A3;_LL6A2: if(*((int*)_tmpA4C)!= 19)
goto _LL6A4;_LL6A3: goto _LL6A5;_LL6A4: if(*((int*)_tmpA4C)!= 20)goto _LL6A6;_LL6A5:
goto _LL6A7;_LL6A6: if(*((int*)_tmpA4C)!= 21)goto _LL6A8;_LL6A7: goto _LL6A9;_LL6A8:
if(*((int*)_tmpA4C)!= 26)goto _LL6AA;_LL6A9: goto _LL6AB;_LL6AA: if(*((int*)_tmpA4C)
!= 27)goto _LL6AC;_LL6AB: goto _LL6AD;_LL6AC: if(*((int*)_tmpA4C)!= 29)goto _LL6AE;
_LL6AD: goto _LL6AF;_LL6AE: if(*((int*)_tmpA4C)!= 30)goto _LL6B0;_LL6AF: goto _LL6B1;
_LL6B0: if(*((int*)_tmpA4C)!= 31)goto _LL6B2;_LL6B1: goto _LL6B3;_LL6B2: if(*((int*)
_tmpA4C)!= 28)goto _LL6B4;_LL6B3: goto _LL6B5;_LL6B4: if(*((int*)_tmpA4C)!= 32)goto
_LL6B6;_LL6B5: goto _LL6B7;_LL6B6: if(*((int*)_tmpA4C)!= 33)goto _LL6B8;_LL6B7: goto
_LL6B9;_LL6B8: if(*((int*)_tmpA4C)!= 34)goto _LL6BA;_LL6B9: goto _LL6BB;_LL6BA: if(*((
int*)_tmpA4C)!= 36)goto _LL6BC;_LL6BB: goto _LL6BD;_LL6BC: if(*((int*)_tmpA4C)!= 24)
goto _LL6BE;_LL6BD: goto _LL6BF;_LL6BE: if(*((int*)_tmpA4C)!= 23)goto _LL6C0;_LL6BF:
goto _LL6C1;_LL6C0: if(*((int*)_tmpA4C)!= 25)goto _LL6C2;_LL6C1: goto _LL6C3;_LL6C2:
if(*((int*)_tmpA4C)!= 22)goto _LL6C4;_LL6C3: goto _LL6C5;_LL6C4: if(*((int*)_tmpA4C)
!= 3)goto _LL6C6;_LL6C5: return 0;_LL6C6: if(*((int*)_tmpA4C)!= 0)goto _LL6C8;_LL6C7:
goto _LL6C9;_LL6C8: if(*((int*)_tmpA4C)!= 35)goto _LL6CA;_LL6C9: goto _LL6CB;_LL6CA:
if(*((int*)_tmpA4C)!= 17)goto _LL6CC;_LL6CB: goto _LL6CD;_LL6CC: if(*((int*)_tmpA4C)
!= 1)goto _LL6CE;_LL6CD: return ignore_leaf  || Cyc_Tcutil_is_unique_pointer((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6CE: if(*((int*)_tmpA4C)!= 6)
goto _LL6D0;_tmpA4D=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA4C)->f2;_LL6CF:
_tmpA4E=_tmpA4D;goto _LL6D1;_LL6D0: if(*((int*)_tmpA4C)!= 9)goto _LL6D2;_tmpA4E=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmpA4C)->f2;_LL6D1: _tmpA4F=_tmpA4E;goto _LL6D3;
_LL6D2: if(*((int*)_tmpA4C)!= 4)goto _LL6D4;_tmpA4F=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpA4C)->f1;_LL6D3: return Cyc_Tcutil_is_unique_path_aux(_tmpA4F,ignore_leaf);
_LL6D4: if(*((int*)_tmpA4C)!= 11)goto _LL6D6;_tmpA50=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpA4C)->f1;_LL6D5: {void*_tmpA54=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA50->topt))->v);struct Cyc_Absyn_FnInfo _tmpA55;void*_tmpA56;
struct Cyc_Absyn_PtrInfo _tmpA57;void*_tmpA58;_LL6DD: if(_tmpA54 <= (void*)4)goto
_LL6E1;if(*((int*)_tmpA54)!= 8)goto _LL6DF;_tmpA55=((struct Cyc_Absyn_FnType_struct*)
_tmpA54)->f1;_tmpA56=(void*)_tmpA55.ret_typ;_LL6DE: return ignore_leaf  || Cyc_Tcutil_is_unique_pointer(
_tmpA56);_LL6DF: if(*((int*)_tmpA54)!= 4)goto _LL6E1;_tmpA57=((struct Cyc_Absyn_PointerType_struct*)
_tmpA54)->f1;_tmpA58=(void*)_tmpA57.elt_typ;_LL6E0:{void*_tmpA59=Cyc_Tcutil_compress(
_tmpA58);struct Cyc_Absyn_FnInfo _tmpA5A;void*_tmpA5B;_LL6E4: if(_tmpA59 <= (void*)4)
goto _LL6E6;if(*((int*)_tmpA59)!= 8)goto _LL6E6;_tmpA5A=((struct Cyc_Absyn_FnType_struct*)
_tmpA59)->f1;_tmpA5B=(void*)_tmpA5A.ret_typ;_LL6E5: return ignore_leaf  || Cyc_Tcutil_is_unique_pointer(
_tmpA5B);_LL6E6:;_LL6E7: goto _LL6E3;_LL6E3:;}goto _LL6E2;_LL6E1:;_LL6E2:({struct
Cyc_String_pa_struct _tmpA5E;_tmpA5E.tag=0;_tmpA5E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA50->topt))->v));{void*_tmpA5C[1]={& _tmpA5E};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA5D="Fncall has non function type %s";_tag_dynforward(_tmpA5D,sizeof(char),
_get_zero_arr_size(_tmpA5D,32));}),_tag_dynforward(_tmpA5C,sizeof(void*),1));}});
_LL6DC:;}_LL6D6: if(*((int*)_tmpA4C)!= 15)goto _LL6D8;_tmpA51=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpA4C)->f1;_tmpA52=((struct Cyc_Absyn_Cast_e_struct*)_tmpA4C)->f2;_LL6D7: return
Cyc_Tcutil_is_unique_pointer(_tmpA51) && Cyc_Tcutil_is_unique_path_aux(_tmpA52,1);
_LL6D8: if(*((int*)_tmpA4C)!= 38)goto _LL6DA;_tmpA53=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpA4C)->f1;_LL6D9: while(1){void*_tmpA5F=(void*)_tmpA53->r;struct Cyc_Absyn_Stmt*
_tmpA60;struct Cyc_Absyn_Stmt*_tmpA61;struct Cyc_Absyn_Decl*_tmpA62;struct Cyc_Absyn_Stmt*
_tmpA63;struct Cyc_Absyn_Exp*_tmpA64;_LL6E9: if(_tmpA5F <= (void*)1)goto _LL6EF;if(*((
int*)_tmpA5F)!= 1)goto _LL6EB;_tmpA60=((struct Cyc_Absyn_Seq_s_struct*)_tmpA5F)->f1;
_tmpA61=((struct Cyc_Absyn_Seq_s_struct*)_tmpA5F)->f2;_LL6EA: _tmpA53=_tmpA61;goto
_LL6E8;_LL6EB: if(*((int*)_tmpA5F)!= 11)goto _LL6ED;_tmpA62=((struct Cyc_Absyn_Decl_s_struct*)
_tmpA5F)->f1;_tmpA63=((struct Cyc_Absyn_Decl_s_struct*)_tmpA5F)->f2;_LL6EC:
_tmpA53=_tmpA63;goto _LL6E8;_LL6ED: if(*((int*)_tmpA5F)!= 0)goto _LL6EF;_tmpA64=((
struct Cyc_Absyn_Exp_s_struct*)_tmpA5F)->f1;_LL6EE: return Cyc_Tcutil_is_unique_path_aux(
_tmpA64,ignore_leaf);_LL6EF:;_LL6F0:({void*_tmpA65[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA66="is_unique_stmt_exp: ill-formed StmtExp";_tag_dynforward(_tmpA66,sizeof(
char),_get_zero_arr_size(_tmpA66,39));}),_tag_dynforward(_tmpA65,sizeof(void*),0));});
_LL6E8:;}_LL6DA:;_LL6DB:({struct Cyc_String_pa_struct _tmpA69;_tmpA69.tag=0;
_tmpA69.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmpA67[1]={& _tmpA69};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA68="Called is_unique_path_aux with bogus exp |%s|\n";
_tag_dynforward(_tmpA68,sizeof(char),_get_zero_arr_size(_tmpA68,47));}),
_tag_dynforward(_tmpA67,sizeof(void*),1));}});_LL695:;}int Cyc_Tcutil_is_unique_path(
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_unique_path_aux(e,0);}struct _tuple10
Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){struct
_tuple10 bogus_ans=({struct _tuple10 _tmpAAD;_tmpAAD.f1=0;_tmpAAD.f2=(void*)2;
_tmpAAD;});void*_tmpA6A=(void*)e->r;struct _tuple2*_tmpA6B;void*_tmpA6C;struct Cyc_Absyn_Exp*
_tmpA6D;struct _dynforward_ptr*_tmpA6E;struct Cyc_Absyn_Exp*_tmpA6F;struct
_dynforward_ptr*_tmpA70;struct Cyc_Absyn_Exp*_tmpA71;struct Cyc_Absyn_Exp*_tmpA72;
struct Cyc_Absyn_Exp*_tmpA73;_LL6F2: if(*((int*)_tmpA6A)!= 1)goto _LL6F4;_tmpA6B=((
struct Cyc_Absyn_Var_e_struct*)_tmpA6A)->f1;_tmpA6C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA6A)->f2;_LL6F3: {void*_tmpA74=_tmpA6C;struct Cyc_Absyn_Vardecl*_tmpA75;
struct Cyc_Absyn_Vardecl*_tmpA76;struct Cyc_Absyn_Vardecl*_tmpA77;struct Cyc_Absyn_Vardecl*
_tmpA78;_LL6FF: if((int)_tmpA74 != 0)goto _LL701;_LL700: goto _LL702;_LL701: if(
_tmpA74 <= (void*)1)goto _LL703;if(*((int*)_tmpA74)!= 1)goto _LL703;_LL702: return
bogus_ans;_LL703: if(_tmpA74 <= (void*)1)goto _LL705;if(*((int*)_tmpA74)!= 0)goto
_LL705;_tmpA75=((struct Cyc_Absyn_Global_b_struct*)_tmpA74)->f1;_LL704: {void*
_tmpA79=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL70C: if(_tmpA79 <= (void*)4)goto _LL70E;if(*((int*)_tmpA79)!= 7)goto _LL70E;
_LL70D: return({struct _tuple10 _tmpA7A;_tmpA7A.f1=1;_tmpA7A.f2=(void*)2;_tmpA7A;});
_LL70E:;_LL70F: return({struct _tuple10 _tmpA7B;_tmpA7B.f1=(_tmpA75->tq).real_const;
_tmpA7B.f2=(void*)2;_tmpA7B;});_LL70B:;}_LL705: if(_tmpA74 <= (void*)1)goto _LL707;
if(*((int*)_tmpA74)!= 3)goto _LL707;_tmpA76=((struct Cyc_Absyn_Local_b_struct*)
_tmpA74)->f1;_LL706: {void*_tmpA7C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL711: if(_tmpA7C <= (void*)4)goto _LL713;if(*((int*)
_tmpA7C)!= 7)goto _LL713;_LL712: return({struct _tuple10 _tmpA7D;_tmpA7D.f1=1;
_tmpA7D.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpA76->rgn))->v;_tmpA7D;});
_LL713:;_LL714: _tmpA76->escapes=1;return({struct _tuple10 _tmpA7E;_tmpA7E.f1=(
_tmpA76->tq).real_const;_tmpA7E.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA76->rgn))->v;_tmpA7E;});_LL710:;}_LL707: if(_tmpA74 <= (void*)1)goto _LL709;
if(*((int*)_tmpA74)!= 4)goto _LL709;_tmpA77=((struct Cyc_Absyn_Pat_b_struct*)
_tmpA74)->f1;_LL708: _tmpA78=_tmpA77;goto _LL70A;_LL709: if(_tmpA74 <= (void*)1)goto
_LL6FE;if(*((int*)_tmpA74)!= 2)goto _LL6FE;_tmpA78=((struct Cyc_Absyn_Param_b_struct*)
_tmpA74)->f1;_LL70A: _tmpA78->escapes=1;return({struct _tuple10 _tmpA7F;_tmpA7F.f1=(
_tmpA78->tq).real_const;_tmpA7F.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA78->rgn))->v;_tmpA7F;});_LL6FE:;}_LL6F4: if(*((int*)_tmpA6A)!= 23)goto _LL6F6;
_tmpA6D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA6A)->f1;_tmpA6E=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpA6A)->f2;_LL6F5: {void*_tmpA80=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA6D->topt))->v);struct Cyc_List_List*_tmpA81;struct Cyc_Absyn_AggrInfo
_tmpA82;union Cyc_Absyn_AggrInfoU_union _tmpA83;struct Cyc_Absyn_Aggrdecl**_tmpA84;
struct Cyc_Absyn_Aggrdecl*_tmpA85;_LL716: if(_tmpA80 <= (void*)4)goto _LL71A;if(*((
int*)_tmpA80)!= 11)goto _LL718;_tmpA81=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA80)->f2;_LL717: {struct Cyc_Absyn_Aggrfield*_tmpA86=Cyc_Absyn_lookup_field(
_tmpA81,_tmpA6E);if(_tmpA86 != 0  && _tmpA86->width != 0)return({struct _tuple10
_tmpA87;_tmpA87.f1=(_tmpA86->tq).real_const;_tmpA87.f2=(Cyc_Tcutil_addressof_props(
te,_tmpA6D)).f2;_tmpA87;});return bogus_ans;}_LL718: if(*((int*)_tmpA80)!= 10)goto
_LL71A;_tmpA82=((struct Cyc_Absyn_AggrType_struct*)_tmpA80)->f1;_tmpA83=_tmpA82.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA80)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL71A;_tmpA84=(_tmpA83.KnownAggr).f1;_tmpA85=*_tmpA84;_LL719: {struct
Cyc_Absyn_Aggrfield*_tmpA88=Cyc_Absyn_lookup_decl_field(_tmpA85,_tmpA6E);if(
_tmpA88 != 0  && _tmpA88->width != 0)return({struct _tuple10 _tmpA89;_tmpA89.f1=(
_tmpA88->tq).real_const;_tmpA89.f2=(Cyc_Tcutil_addressof_props(te,_tmpA6D)).f2;
_tmpA89;});return bogus_ans;}_LL71A:;_LL71B: return bogus_ans;_LL715:;}_LL6F6: if(*((
int*)_tmpA6A)!= 24)goto _LL6F8;_tmpA6F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpA6A)->f1;_tmpA70=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA6A)->f2;_LL6F7: {
void*_tmpA8A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA6F->topt))->v);struct Cyc_Absyn_PtrInfo _tmpA8B;void*_tmpA8C;struct Cyc_Absyn_PtrAtts
_tmpA8D;void*_tmpA8E;_LL71D: if(_tmpA8A <= (void*)4)goto _LL71F;if(*((int*)_tmpA8A)
!= 4)goto _LL71F;_tmpA8B=((struct Cyc_Absyn_PointerType_struct*)_tmpA8A)->f1;
_tmpA8C=(void*)_tmpA8B.elt_typ;_tmpA8D=_tmpA8B.ptr_atts;_tmpA8E=(void*)_tmpA8D.rgn;
_LL71E: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpA8F=Cyc_Tcutil_compress(
_tmpA8C);struct Cyc_List_List*_tmpA90;struct Cyc_Absyn_AggrInfo _tmpA91;union Cyc_Absyn_AggrInfoU_union
_tmpA92;struct Cyc_Absyn_Aggrdecl**_tmpA93;struct Cyc_Absyn_Aggrdecl*_tmpA94;
_LL722: if(_tmpA8F <= (void*)4)goto _LL726;if(*((int*)_tmpA8F)!= 11)goto _LL724;
_tmpA90=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA8F)->f2;_LL723: finfo=Cyc_Absyn_lookup_field(
_tmpA90,_tmpA70);goto _LL721;_LL724: if(*((int*)_tmpA8F)!= 10)goto _LL726;_tmpA91=((
struct Cyc_Absyn_AggrType_struct*)_tmpA8F)->f1;_tmpA92=_tmpA91.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpA8F)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL726;_tmpA93=(_tmpA92.KnownAggr).f1;_tmpA94=*_tmpA93;_LL725: finfo=Cyc_Absyn_lookup_decl_field(
_tmpA94,_tmpA70);goto _LL721;_LL726:;_LL727: return bogus_ans;_LL721:;}if(finfo != 0
 && finfo->width != 0)return({struct _tuple10 _tmpA95;_tmpA95.f1=(finfo->tq).real_const;
_tmpA95.f2=_tmpA8E;_tmpA95;});return bogus_ans;}_LL71F:;_LL720: return bogus_ans;
_LL71C:;}_LL6F8: if(*((int*)_tmpA6A)!= 22)goto _LL6FA;_tmpA71=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA6A)->f1;_LL6F9: {void*_tmpA96=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA71->topt))->v);struct Cyc_Absyn_PtrInfo _tmpA97;struct Cyc_Absyn_Tqual
_tmpA98;struct Cyc_Absyn_PtrAtts _tmpA99;void*_tmpA9A;_LL729: if(_tmpA96 <= (void*)4)
goto _LL72B;if(*((int*)_tmpA96)!= 4)goto _LL72B;_tmpA97=((struct Cyc_Absyn_PointerType_struct*)
_tmpA96)->f1;_tmpA98=_tmpA97.elt_tq;_tmpA99=_tmpA97.ptr_atts;_tmpA9A=(void*)
_tmpA99.rgn;_LL72A: return({struct _tuple10 _tmpA9B;_tmpA9B.f1=_tmpA98.real_const;
_tmpA9B.f2=_tmpA9A;_tmpA9B;});_LL72B:;_LL72C: return bogus_ans;_LL728:;}_LL6FA: if(*((
int*)_tmpA6A)!= 25)goto _LL6FC;_tmpA72=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA6A)->f1;_tmpA73=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA6A)->f2;_LL6FB: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA72->topt))->v);
void*_tmpA9C=t;struct Cyc_List_List*_tmpA9D;struct Cyc_Absyn_PtrInfo _tmpA9E;struct
Cyc_Absyn_Tqual _tmpA9F;struct Cyc_Absyn_PtrAtts _tmpAA0;void*_tmpAA1;struct Cyc_Absyn_ArrayInfo
_tmpAA2;struct Cyc_Absyn_Tqual _tmpAA3;_LL72E: if(_tmpA9C <= (void*)4)goto _LL734;if(*((
int*)_tmpA9C)!= 9)goto _LL730;_tmpA9D=((struct Cyc_Absyn_TupleType_struct*)_tmpA9C)->f1;
_LL72F: {unsigned int _tmpAA5;int _tmpAA6;struct _tuple7 _tmpAA4=Cyc_Evexp_eval_const_uint_exp(
_tmpA73);_tmpAA5=_tmpAA4.f1;_tmpAA6=_tmpAA4.f2;if(!_tmpAA6)return bogus_ans;{
struct _tuple5*_tmpAA7=Cyc_Absyn_lookup_tuple_field(_tmpA9D,(int)_tmpAA5);if(
_tmpAA7 != 0)return({struct _tuple10 _tmpAA8;_tmpAA8.f1=((*_tmpAA7).f1).real_const;
_tmpAA8.f2=(Cyc_Tcutil_addressof_props(te,_tmpA72)).f2;_tmpAA8;});return
bogus_ans;}}_LL730: if(*((int*)_tmpA9C)!= 4)goto _LL732;_tmpA9E=((struct Cyc_Absyn_PointerType_struct*)
_tmpA9C)->f1;_tmpA9F=_tmpA9E.elt_tq;_tmpAA0=_tmpA9E.ptr_atts;_tmpAA1=(void*)
_tmpAA0.rgn;_LL731: return({struct _tuple10 _tmpAA9;_tmpAA9.f1=_tmpA9F.real_const;
_tmpAA9.f2=_tmpAA1;_tmpAA9;});_LL732: if(*((int*)_tmpA9C)!= 7)goto _LL734;_tmpAA2=((
struct Cyc_Absyn_ArrayType_struct*)_tmpA9C)->f1;_tmpAA3=_tmpAA2.tq;_LL733: return({
struct _tuple10 _tmpAAA;_tmpAAA.f1=_tmpAA3.real_const;_tmpAAA.f2=(Cyc_Tcutil_addressof_props(
te,_tmpA72)).f2;_tmpAAA;});_LL734:;_LL735: return bogus_ans;_LL72D:;}_LL6FC:;
_LL6FD:({void*_tmpAAB[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmpAAC="unary & applied to non-lvalue";
_tag_dynforward(_tmpAAC,sizeof(char),_get_zero_arr_size(_tmpAAC,30));}),
_tag_dynforward(_tmpAAB,sizeof(void*),0));});return bogus_ans;_LL6F1:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpAAE=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpAAF;void*_tmpAB0;struct Cyc_Absyn_Tqual
_tmpAB1;struct Cyc_Absyn_Conref*_tmpAB2;_LL737: if(_tmpAAE <= (void*)4)goto _LL739;
if(*((int*)_tmpAAE)!= 7)goto _LL739;_tmpAAF=((struct Cyc_Absyn_ArrayType_struct*)
_tmpAAE)->f1;_tmpAB0=(void*)_tmpAAF.elt_type;_tmpAB1=_tmpAAF.tq;_tmpAB2=_tmpAAF.zero_term;
_LL738: {void*_tmpAB4;struct _tuple10 _tmpAB3=Cyc_Tcutil_addressof_props(te,e);
_tmpAB4=_tmpAB3.f2;return Cyc_Absyn_atb_typ(_tmpAB0,_tmpAB4,_tmpAB1,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpAB5=_cycalloc(sizeof(*_tmpAB5));_tmpAB5[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpAB6;_tmpAB6.tag=0;_tmpAB6.f1=e;_tmpAB6;});
_tmpAB5;}),_tmpAB2);}_LL739:;_LL73A: return e_typ;_LL736:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpAB7=b->v;void*_tmpAB8;void*_tmpAB9;void*
_tmpABA;struct Cyc_Absyn_Exp*_tmpABB;void*_tmpABC;_LL73C: if((_tmpAB7.Eq_constr).tag
!= 0)goto _LL73E;_tmpAB8=(_tmpAB7.Eq_constr).f1;if((int)_tmpAB8 != 1)goto _LL73E;
_LL73D: return;_LL73E: if((_tmpAB7.Eq_constr).tag != 0)goto _LL740;_tmpAB9=(_tmpAB7.Eq_constr).f1;
if((int)_tmpAB9 != 0)goto _LL740;_LL73F: if(i < 0)({void*_tmpABD[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmpABE="dereference is out of bounds";_tag_dynforward(_tmpABE,
sizeof(char),_get_zero_arr_size(_tmpABE,29));}),_tag_dynforward(_tmpABD,sizeof(
void*),0));});return;_LL740: if((_tmpAB7.Eq_constr).tag != 0)goto _LL742;_tmpABA=(
_tmpAB7.Eq_constr).f1;if(_tmpABA <= (void*)2)goto _LL742;if(*((int*)_tmpABA)!= 0)
goto _LL742;_tmpABB=((struct Cyc_Absyn_Upper_b_struct*)_tmpABA)->f1;_LL741: {
unsigned int _tmpAC0;int _tmpAC1;struct _tuple7 _tmpABF=Cyc_Evexp_eval_const_uint_exp(
_tmpABB);_tmpAC0=_tmpABF.f1;_tmpAC1=_tmpABF.f2;if(_tmpAC1  && _tmpAC0 <= i)({
struct Cyc_Int_pa_struct _tmpAC5;_tmpAC5.tag=1;_tmpAC5.f1=(unsigned long)((int)i);{
struct Cyc_Int_pa_struct _tmpAC4;_tmpAC4.tag=1;_tmpAC4.f1=(unsigned long)((int)
_tmpAC0);{void*_tmpAC2[2]={& _tmpAC4,& _tmpAC5};Cyc_Tcutil_terr(loc,({const char*
_tmpAC3="dereference is out of bounds: %d <= %d";_tag_dynforward(_tmpAC3,sizeof(
char),_get_zero_arr_size(_tmpAC3,39));}),_tag_dynforward(_tmpAC2,sizeof(void*),2));}}});
return;}_LL742: if((_tmpAB7.Eq_constr).tag != 0)goto _LL744;_tmpABC=(_tmpAB7.Eq_constr).f1;
if(_tmpABC <= (void*)2)goto _LL744;if(*((int*)_tmpABC)!= 1)goto _LL744;_LL743:
return;_LL744: if((_tmpAB7.No_constr).tag != 2)goto _LL746;_LL745: b->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmpAC6;(_tmpAC6.Eq_constr).tag=0;(_tmpAC6.Eq_constr).f1=(
void*)((void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmpAC7=_cycalloc(sizeof(*
_tmpAC7));_tmpAC7[0]=({struct Cyc_Absyn_Upper_b_struct _tmpAC8;_tmpAC8.tag=0;
_tmpAC8.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmpAC8;});_tmpAC7;})));_tmpAC6;});return;
_LL746:;_LL747:({void*_tmpAC9[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpACA="check_bound -- bad compressed conref";
_tag_dynforward(_tmpACA,sizeof(char),_get_zero_arr_size(_tmpACA,37));}),
_tag_dynforward(_tmpAC9,sizeof(void*),0));});_LL73B:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){union Cyc_Absyn_Constraint_union
_tmpACB=(Cyc_Absyn_compress_conref(b))->v;void*_tmpACC;struct Cyc_Absyn_Exp*
_tmpACD;_LL749: if((_tmpACB.Eq_constr).tag != 0)goto _LL74B;_tmpACC=(_tmpACB.Eq_constr).f1;
if(_tmpACC <= (void*)2)goto _LL74B;if(*((int*)_tmpACC)!= 0)goto _LL74B;_tmpACD=((
struct Cyc_Absyn_Upper_b_struct*)_tmpACC)->f1;_LL74A: {unsigned int _tmpACF;int
_tmpAD0;struct _tuple7 _tmpACE=Cyc_Evexp_eval_const_uint_exp(_tmpACD);_tmpACF=
_tmpACE.f1;_tmpAD0=_tmpACE.f2;return _tmpAD0  && _tmpACF == 1;}_LL74B:;_LL74C:
return 0;_LL748:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpAD1=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpAD2;void*_tmpAD3;struct Cyc_Absyn_Conref*_tmpAD4;
struct Cyc_List_List*_tmpAD5;struct Cyc_Absyn_AggrInfo _tmpAD6;union Cyc_Absyn_AggrInfoU_union
_tmpAD7;struct Cyc_Absyn_AggrInfo _tmpAD8;union Cyc_Absyn_AggrInfoU_union _tmpAD9;
struct Cyc_Absyn_Aggrdecl**_tmpADA;struct Cyc_Absyn_Aggrdecl*_tmpADB;struct Cyc_List_List*
_tmpADC;struct Cyc_List_List*_tmpADD;_LL74E: if((int)_tmpAD1 != 0)goto _LL750;_LL74F:
goto _LL751;_LL750: if(_tmpAD1 <= (void*)4)goto _LL752;if(*((int*)_tmpAD1)!= 5)goto
_LL752;_LL751: goto _LL753;_LL752: if((int)_tmpAD1 != 1)goto _LL754;_LL753: goto _LL755;
_LL754: if(_tmpAD1 <= (void*)4)goto _LL764;if(*((int*)_tmpAD1)!= 6)goto _LL756;
_LL755: return 1;_LL756: if(*((int*)_tmpAD1)!= 12)goto _LL758;_LL757: goto _LL759;
_LL758: if(*((int*)_tmpAD1)!= 13)goto _LL75A;_LL759: return 0;_LL75A: if(*((int*)
_tmpAD1)!= 7)goto _LL75C;_tmpAD2=((struct Cyc_Absyn_ArrayType_struct*)_tmpAD1)->f1;
_tmpAD3=(void*)_tmpAD2.elt_type;_tmpAD4=_tmpAD2.zero_term;_LL75B: return !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpAD4) && Cyc_Tcutil_bits_only(
_tmpAD3);_LL75C: if(*((int*)_tmpAD1)!= 9)goto _LL75E;_tmpAD5=((struct Cyc_Absyn_TupleType_struct*)
_tmpAD1)->f1;_LL75D: for(0;_tmpAD5 != 0;_tmpAD5=_tmpAD5->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpAD5->hd)).f2))return 0;}return 1;_LL75E: if(*((int*)_tmpAD1)!= 
10)goto _LL760;_tmpAD6=((struct Cyc_Absyn_AggrType_struct*)_tmpAD1)->f1;_tmpAD7=
_tmpAD6.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAD1)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL760;_LL75F: return 0;_LL760: if(*((int*)_tmpAD1)!= 10)goto _LL762;
_tmpAD8=((struct Cyc_Absyn_AggrType_struct*)_tmpAD1)->f1;_tmpAD9=_tmpAD8.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAD1)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL762;_tmpADA=(_tmpAD9.KnownAggr).f1;_tmpADB=*_tmpADA;_tmpADC=_tmpAD8.targs;
_LL761: if(_tmpADB->impl == 0)return 0;{struct _RegionHandle _tmpADE=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpADE;_push_region(rgn);{struct Cyc_List_List*_tmpADF=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpADB->tvs,_tmpADC);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpADB->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpADF,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAE0=0;_npop_handler(0);
return _tmpAE0;}}}{int _tmpAE1=1;_npop_handler(0);return _tmpAE1;}};_pop_region(rgn);}
_LL762: if(*((int*)_tmpAD1)!= 11)goto _LL764;_tmpADD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpAD1)->f2;_LL763: for(0;_tmpADD != 0;_tmpADD=_tmpADD->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpADD->hd)->type))return 0;}return 1;_LL764:;
_LL765: return 0;_LL74D:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpAE2=(void*)e->r;struct _tuple2*_tmpAE3;void*_tmpAE4;struct Cyc_Absyn_Exp*
_tmpAE5;struct Cyc_Absyn_Exp*_tmpAE6;struct Cyc_Absyn_Exp*_tmpAE7;struct Cyc_Absyn_Exp*
_tmpAE8;struct Cyc_Absyn_Exp*_tmpAE9;struct Cyc_Absyn_Exp*_tmpAEA;struct Cyc_Absyn_Exp*
_tmpAEB;void*_tmpAEC;struct Cyc_Absyn_Exp*_tmpAED;void*_tmpAEE;void*_tmpAEF;
struct Cyc_Absyn_Exp*_tmpAF0;struct Cyc_Absyn_Exp*_tmpAF1;struct Cyc_Absyn_Exp*
_tmpAF2;struct Cyc_Absyn_Exp*_tmpAF3;struct Cyc_List_List*_tmpAF4;struct Cyc_List_List*
_tmpAF5;struct Cyc_List_List*_tmpAF6;void*_tmpAF7;struct Cyc_List_List*_tmpAF8;
struct Cyc_List_List*_tmpAF9;struct Cyc_List_List*_tmpAFA;_LL767: if(*((int*)
_tmpAE2)!= 0)goto _LL769;_LL768: goto _LL76A;_LL769: if(*((int*)_tmpAE2)!= 18)goto
_LL76B;_LL76A: goto _LL76C;_LL76B: if(*((int*)_tmpAE2)!= 19)goto _LL76D;_LL76C: goto
_LL76E;_LL76D: if(*((int*)_tmpAE2)!= 20)goto _LL76F;_LL76E: goto _LL770;_LL76F: if(*((
int*)_tmpAE2)!= 21)goto _LL771;_LL770: goto _LL772;_LL771: if(*((int*)_tmpAE2)!= 33)
goto _LL773;_LL772: goto _LL774;_LL773: if(*((int*)_tmpAE2)!= 34)goto _LL775;_LL774:
return 1;_LL775: if(*((int*)_tmpAE2)!= 1)goto _LL777;_tmpAE3=((struct Cyc_Absyn_Var_e_struct*)
_tmpAE2)->f1;_tmpAE4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpAE2)->f2;_LL776: {
void*_tmpAFB=_tmpAE4;struct Cyc_Absyn_Vardecl*_tmpAFC;_LL796: if(_tmpAFB <= (void*)
1)goto _LL79A;if(*((int*)_tmpAFB)!= 1)goto _LL798;_LL797: return 1;_LL798: if(*((int*)
_tmpAFB)!= 0)goto _LL79A;_tmpAFC=((struct Cyc_Absyn_Global_b_struct*)_tmpAFB)->f1;
_LL799: {void*_tmpAFD=Cyc_Tcutil_compress((void*)_tmpAFC->type);_LL79F: if(
_tmpAFD <= (void*)4)goto _LL7A3;if(*((int*)_tmpAFD)!= 7)goto _LL7A1;_LL7A0: goto
_LL7A2;_LL7A1: if(*((int*)_tmpAFD)!= 8)goto _LL7A3;_LL7A2: return 1;_LL7A3:;_LL7A4:
return var_okay;_LL79E:;}_LL79A: if((int)_tmpAFB != 0)goto _LL79C;_LL79B: return 0;
_LL79C:;_LL79D: return var_okay;_LL795:;}_LL777: if(*((int*)_tmpAE2)!= 6)goto _LL779;
_tmpAE5=((struct Cyc_Absyn_Conditional_e_struct*)_tmpAE2)->f1;_tmpAE6=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpAE2)->f2;_tmpAE7=((struct Cyc_Absyn_Conditional_e_struct*)_tmpAE2)->f3;_LL778:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpAE5) && Cyc_Tcutil_cnst_exp(te,0,_tmpAE6))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpAE7);_LL779: if(*((int*)_tmpAE2)!= 9)goto _LL77B;
_tmpAE8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpAE2)->f1;_tmpAE9=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpAE2)->f2;_LL77A: return Cyc_Tcutil_cnst_exp(te,0,_tmpAE8) && Cyc_Tcutil_cnst_exp(
te,0,_tmpAE9);_LL77B: if(*((int*)_tmpAE2)!= 13)goto _LL77D;_tmpAEA=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpAE2)->f1;_LL77C: _tmpAEB=_tmpAEA;goto _LL77E;_LL77D: if(*((int*)_tmpAE2)!= 14)
goto _LL77F;_tmpAEB=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpAE2)->f1;_LL77E:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpAEB);_LL77F: if(*((int*)_tmpAE2)!= 15)
goto _LL781;_tmpAEC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpAE2)->f1;_tmpAED=((
struct Cyc_Absyn_Cast_e_struct*)_tmpAE2)->f2;_tmpAEE=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpAE2)->f4;if((int)_tmpAEE != 1)goto _LL781;_LL780: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpAED);_LL781: if(*((int*)_tmpAE2)!= 15)goto _LL783;_tmpAEF=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpAE2)->f1;_tmpAF0=((struct Cyc_Absyn_Cast_e_struct*)
_tmpAE2)->f2;_LL782: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpAF0);_LL783: if(*((
int*)_tmpAE2)!= 16)goto _LL785;_tmpAF1=((struct Cyc_Absyn_Address_e_struct*)
_tmpAE2)->f1;_LL784: return Cyc_Tcutil_cnst_exp(te,1,_tmpAF1);_LL785: if(*((int*)
_tmpAE2)!= 29)goto _LL787;_tmpAF2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpAE2)->f2;_tmpAF3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpAE2)->f3;
_LL786: return Cyc_Tcutil_cnst_exp(te,0,_tmpAF2) && Cyc_Tcutil_cnst_exp(te,0,
_tmpAF3);_LL787: if(*((int*)_tmpAE2)!= 28)goto _LL789;_tmpAF4=((struct Cyc_Absyn_Array_e_struct*)
_tmpAE2)->f1;_LL788: _tmpAF5=_tmpAF4;goto _LL78A;_LL789: if(*((int*)_tmpAE2)!= 31)
goto _LL78B;_tmpAF5=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpAE2)->f2;_LL78A:
_tmpAF6=_tmpAF5;goto _LL78C;_LL78B: if(*((int*)_tmpAE2)!= 30)goto _LL78D;_tmpAF6=((
struct Cyc_Absyn_Struct_e_struct*)_tmpAE2)->f3;_LL78C: for(0;_tmpAF6 != 0;_tmpAF6=
_tmpAF6->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpAF6->hd)).f2))
return 0;}return 1;_LL78D: if(*((int*)_tmpAE2)!= 3)goto _LL78F;_tmpAF7=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpAE2)->f1;_tmpAF8=((struct Cyc_Absyn_Primop_e_struct*)
_tmpAE2)->f2;_LL78E: _tmpAF9=_tmpAF8;goto _LL790;_LL78F: if(*((int*)_tmpAE2)!= 26)
goto _LL791;_tmpAF9=((struct Cyc_Absyn_Tuple_e_struct*)_tmpAE2)->f1;_LL790: _tmpAFA=
_tmpAF9;goto _LL792;_LL791: if(*((int*)_tmpAE2)!= 32)goto _LL793;_tmpAFA=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpAE2)->f1;_LL792: for(0;_tmpAFA != 0;_tmpAFA=_tmpAFA->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpAFA->hd))return 0;}return 1;
_LL793:;_LL794: return 0;_LL766:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpAFE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpAFF;
struct Cyc_Absyn_PtrAtts _tmpB00;struct Cyc_Absyn_Conref*_tmpB01;struct Cyc_Absyn_Conref*
_tmpB02;struct Cyc_Absyn_ArrayInfo _tmpB03;void*_tmpB04;struct Cyc_List_List*
_tmpB05;struct Cyc_Absyn_AggrInfo _tmpB06;union Cyc_Absyn_AggrInfoU_union _tmpB07;
struct Cyc_List_List*_tmpB08;struct Cyc_List_List*_tmpB09;_LL7A6: if((int)_tmpAFE != 
0)goto _LL7A8;_LL7A7: goto _LL7A9;_LL7A8: if(_tmpAFE <= (void*)4)goto _LL7AA;if(*((int*)
_tmpAFE)!= 5)goto _LL7AA;_LL7A9: goto _LL7AB;_LL7AA: if((int)_tmpAFE != 1)goto _LL7AC;
_LL7AB: goto _LL7AD;_LL7AC: if(_tmpAFE <= (void*)4)goto _LL7BC;if(*((int*)_tmpAFE)!= 
6)goto _LL7AE;_LL7AD: return 1;_LL7AE: if(*((int*)_tmpAFE)!= 4)goto _LL7B0;_tmpAFF=((
struct Cyc_Absyn_PointerType_struct*)_tmpAFE)->f1;_tmpB00=_tmpAFF.ptr_atts;
_tmpB01=_tmpB00.nullable;_tmpB02=_tmpB00.bounds;_LL7AF: {void*_tmpB0A=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB02);_LL7BF: if((int)_tmpB0A != 0)goto _LL7C1;_LL7C0: return 1;
_LL7C1: if((int)_tmpB0A != 1)goto _LL7C3;_LL7C2: return 1;_LL7C3:;_LL7C4: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpB01);_LL7BE:;}_LL7B0:
if(*((int*)_tmpAFE)!= 7)goto _LL7B2;_tmpB03=((struct Cyc_Absyn_ArrayType_struct*)
_tmpAFE)->f1;_tmpB04=(void*)_tmpB03.elt_type;_LL7B1: return Cyc_Tcutil_supports_default(
_tmpB04);_LL7B2: if(*((int*)_tmpAFE)!= 9)goto _LL7B4;_tmpB05=((struct Cyc_Absyn_TupleType_struct*)
_tmpAFE)->f1;_LL7B3: for(0;_tmpB05 != 0;_tmpB05=_tmpB05->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple5*)_tmpB05->hd)).f2))return 0;}return 1;_LL7B4: if(*((int*)_tmpAFE)!= 
10)goto _LL7B6;_tmpB06=((struct Cyc_Absyn_AggrType_struct*)_tmpAFE)->f1;_tmpB07=
_tmpB06.aggr_info;_tmpB08=_tmpB06.targs;_LL7B5: {struct Cyc_Absyn_Aggrdecl*
_tmpB0B=Cyc_Absyn_get_known_aggrdecl(_tmpB07);if(_tmpB0B->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB0B->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpB0B->tvs,_tmpB08,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB0B->impl))->fields);}_LL7B6: if(*((int*)_tmpAFE)!= 11)goto _LL7B8;
_tmpB09=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpAFE)->f2;_LL7B7: return Cyc_Tcutil_fields_support_default(
0,0,_tmpB09);_LL7B8: if(*((int*)_tmpAFE)!= 13)goto _LL7BA;_LL7B9: goto _LL7BB;_LL7BA:
if(*((int*)_tmpAFE)!= 12)goto _LL7BC;_LL7BB: return 1;_LL7BC:;_LL7BD: return 0;_LL7A5:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmpB0C=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpB0C;_push_region(rgn);{struct Cyc_List_List*_tmpB0D=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB0D,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB0E=0;_npop_handler(0);return _tmpB0E;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){void*_tmpB0F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpB10;struct Cyc_Absyn_PtrAtts _tmpB11;struct Cyc_Absyn_Conref*
_tmpB12;struct Cyc_Absyn_Conref*_tmpB13;_LL7C6: if(_tmpB0F <= (void*)4)goto _LL7C8;
if(*((int*)_tmpB0F)!= 5)goto _LL7C8;_LL7C7: goto _LL7C9;_LL7C8: if((int)_tmpB0F != 1)
goto _LL7CA;_LL7C9: goto _LL7CB;_LL7CA: if(_tmpB0F <= (void*)4)goto _LL7CE;if(*((int*)
_tmpB0F)!= 6)goto _LL7CC;_LL7CB: return 1;_LL7CC: if(*((int*)_tmpB0F)!= 4)goto _LL7CE;
_tmpB10=((struct Cyc_Absyn_PointerType_struct*)_tmpB0F)->f1;_tmpB11=_tmpB10.ptr_atts;
_tmpB12=_tmpB11.nullable;_tmpB13=_tmpB11.bounds;_LL7CD: {union Cyc_Absyn_Constraint_union
_tmpB14=(Cyc_Absyn_compress_conref(_tmpB13))->v;void*_tmpB15;void*_tmpB16;_LL7D1:
if((_tmpB14.Eq_constr).tag != 0)goto _LL7D3;_tmpB15=(_tmpB14.Eq_constr).f1;if((int)
_tmpB15 != 0)goto _LL7D3;_LL7D2: return 0;_LL7D3: if((_tmpB14.Eq_constr).tag != 0)goto
_LL7D5;_tmpB16=(_tmpB14.Eq_constr).f1;if((int)_tmpB16 != 1)goto _LL7D5;_LL7D4:
return 0;_LL7D5: if((_tmpB14.Eq_constr).tag != 0)goto _LL7D7;_LL7D6: {union Cyc_Absyn_Constraint_union
_tmpB17=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpB12))->v;int _tmpB18;_LL7DA: if((_tmpB17.Eq_constr).tag != 0)goto _LL7DC;_tmpB18=(
_tmpB17.Eq_constr).f1;_LL7DB: return _tmpB18;_LL7DC:;_LL7DD: return 0;_LL7D9:;}
_LL7D7:;_LL7D8: return 0;_LL7D0:;}_LL7CE:;_LL7CF: return 0;_LL7C5:;}int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpB19=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB1A;void*
_tmpB1B;struct Cyc_Absyn_FnInfo _tmpB1C;struct Cyc_List_List*_tmpB1D;_LL7DF: if(
_tmpB19 <= (void*)4)goto _LL7E3;if(*((int*)_tmpB19)!= 4)goto _LL7E1;_tmpB1A=((
struct Cyc_Absyn_PointerType_struct*)_tmpB19)->f1;_tmpB1B=(void*)_tmpB1A.elt_typ;
_LL7E0: return Cyc_Tcutil_is_noreturn(_tmpB1B);_LL7E1: if(*((int*)_tmpB19)!= 8)goto
_LL7E3;_tmpB1C=((struct Cyc_Absyn_FnType_struct*)_tmpB19)->f1;_tmpB1D=_tmpB1C.attributes;
_LL7E2: for(0;_tmpB1D != 0;_tmpB1D=_tmpB1D->tl){void*_tmpB1E=(void*)_tmpB1D->hd;
_LL7E6: if((int)_tmpB1E != 3)goto _LL7E8;_LL7E7: return 1;_LL7E8:;_LL7E9: continue;
_LL7E5:;}goto _LL7DE;_LL7E3:;_LL7E4: goto _LL7DE;_LL7DE:;}return 0;}struct Cyc_List_List*
Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){void*_tmpB1F=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpB20;struct Cyc_List_List*_tmpB21;
struct Cyc_List_List**_tmpB22;_LL7EB: if(_tmpB1F <= (void*)4)goto _LL7ED;if(*((int*)
_tmpB1F)!= 8)goto _LL7ED;_tmpB20=((struct Cyc_Absyn_FnType_struct*)_tmpB1F)->f1;
_tmpB21=_tmpB20.attributes;_tmpB22=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmpB1F)->f1).attributes;_LL7EC: {struct Cyc_List_List*_tmpB23=0;for(0;atts != 0;
atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmpB22=({struct Cyc_List_List*
_tmpB24=_cycalloc(sizeof(*_tmpB24));_tmpB24->hd=(void*)((void*)atts->hd);_tmpB24->tl=*
_tmpB22;_tmpB24;});else{_tmpB23=({struct Cyc_List_List*_tmpB25=_cycalloc(sizeof(*
_tmpB25));_tmpB25->hd=(void*)((void*)atts->hd);_tmpB25->tl=_tmpB23;_tmpB25;});}}
return _tmpB23;}_LL7ED:;_LL7EE:({void*_tmpB26[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpB27="transfer_fn_type_atts";_tag_dynforward(_tmpB27,sizeof(char),
_get_zero_arr_size(_tmpB27,22));}),_tag_dynforward(_tmpB26,sizeof(void*),0));});
_LL7EA:;}
