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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion
Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];void Cyc_Core_free_dynregion(struct _DynRegionHandle*);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
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
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct
_dynforward_ptr Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct
_RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct
_RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_rlookup_opt(
struct _RegionHandle*,struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;
};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct _dynforward_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(
struct _tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _dynforward_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void*Cyc_Tcutil_impos(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dynforward_ptr,int*is_dyneither_ptr);int Cyc_Tcutil_unify(void*,
void*);int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type);struct _tuple4{struct
Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple4*);struct
_tuple5{unsigned int f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
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
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dynforward_ptr Cyc_Toc_globals;int Cyc_Toc_warn_bounds_checks=0;int
Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**tunions_so_far;
struct Cyc_Dict_Dict*xtunions_so_far;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=
0;static int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";
char Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*
Cyc_Toc_unimp(struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,
fmt,ap);({void*_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\n";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,2));}),
_tag_dynforward(_tmp0,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(
int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="\n";_tag_dynforward(
_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,2));}),_tag_dynforward(_tmp2,sizeof(
void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)
Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";
static char _tmp4[5]="curr";static struct _dynforward_ptr Cyc_Toc_curr_string={_tmp4,
_tmp4 + 5};static struct _dynforward_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp5[4]="tag";static struct _dynforward_ptr Cyc_Toc_tag_string={_tmp5,
_tmp5 + 4};static struct _dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static
char _tmp6[4]="val";static struct _dynforward_ptr Cyc_Toc_val_string={_tmp6,_tmp6 + 4};
static struct _dynforward_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp7[
14]="_handler_cons";static struct _dynforward_ptr Cyc_Toc__handler_cons_string={
_tmp7,_tmp7 + 14};static struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp8[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp8,_tmp8 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp9[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp9,_tmp9 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmpA[17]="_DynRegionHandle";static struct _dynforward_ptr Cyc_Toc__DynRegionHandle_string={
_tmpA,_tmpA + 17};static struct _dynforward_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmpB[16]="_DynRegionFrame";static struct _dynforward_ptr Cyc_Toc__DynRegionFrame_string={
_tmpB,_tmpB + 16};static struct _dynforward_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dynforward_ptr Cyc_Toc_globals={(void*)0,(void*)(0 + 0)};static char _tmpC[7]="_throw";
static struct _dynforward_ptr Cyc_Toc__throw_str={_tmpC,_tmpC + 7};static struct
_tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,&
Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpF[7]="setjmp";
static struct _dynforward_ptr Cyc_Toc_setjmp_str={_tmpF,_tmpF + 7};static struct
_tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,&
Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp12[
14]="_push_handler";static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmp12,
_tmp12 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp15[13]="_pop_handler";static struct
_dynforward_ptr Cyc_Toc__pop_handler_str={_tmp15,_tmp15 + 13};static struct _tuple1
Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp18[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp18,_tmp18 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp1B[14]="_npop_handler";static struct
_dynforward_ptr Cyc_Toc__npop_handler_str={_tmp1B,_tmp1B + 14};static struct _tuple1
Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp1E[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp1E,_tmp1E + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp21[28]="_check_known_subscript_null";static
struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={_tmp21,_tmp21 + 28};
static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp24[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp24,_tmp24 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp27[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp27,
_tmp27 + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dynforward_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp2A[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp2A,_tmp2A + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp2D[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp2D,_tmp2D + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_re={1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=&
Cyc_Toc__dynforward_ptr_ev;static char _tmp30[15]="_dyneither_ptr";static struct
_dynforward_ptr Cyc_Toc__dyneither_ptr_str={_tmp30,_tmp30 + 15};static struct
_tuple1 Cyc_Toc__dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp33[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp33,_tmp33 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_dynforward_re={1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=&
Cyc_Toc__tag_dynforward_ev;static char _tmp36[15]="_tag_dyneither";static struct
_dynforward_ptr Cyc_Toc__tag_dyneither_str={_tmp36,_tmp36 + 15};static struct
_tuple1 Cyc_Toc__tag_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp39[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp39,_tmp39 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dynforward_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp3C[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp3C,_tmp3C + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp3F[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp3F,_tmp3F + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dynforward_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp42[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp42,_tmp42 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp45[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp45,_tmp45 + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dynforward_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp48[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp48,_tmp48 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp4B[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp4B,_tmp4B + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp4E[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp4E,_tmp4E + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp51[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp51,_tmp51 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp54[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp54,_tmp54 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=&
Cyc_Toc__zero_arr_plus_ev;static char _tmp57[29]="_dynforward_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp57,
_tmp57 + 29};static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={1,&
Cyc_Toc__dynforward_ptr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_ev;static char _tmp5A[28]="_dyneither_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp5A,
_tmp5A + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp5D[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp5D,_tmp5D + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp60[34]="_dynforward_ptr_inplace_plus_post";static struct
_dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmp60,_tmp60 + 34};
static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_post_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmp63[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp63,
_tmp63 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp66[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmp66,_tmp66 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_to_dyneither_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={1,& Cyc_Toc__dynforward_to_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_to_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=& Cyc_Toc__dynforward_to_dyneither_ev;
static char _tmp69[25]="_dyneither_to_dynforward";static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={
_tmp69,_tmp69 + 25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_to_dynforward_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={1,& Cyc_Toc__dyneither_to_dynforward_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_to_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=& Cyc_Toc__dyneither_to_dynforward_ev;
static char _tmp6C[28]="_zero_arr_inplace_plus_post";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp6C,_tmp6C + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp6F[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmp6F,_tmp6F + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmp72[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmp72,_tmp72 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmp75[17]="_cycalloc_atomic";static struct
_dynforward_ptr Cyc_Toc__cycalloc_atomic_str={_tmp75,_tmp75 + 17};static struct
_tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp78[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp78,_tmp78 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp7B[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmp7B,_tmp7B + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp7E[15]="_region_calloc";static struct
_dynforward_ptr Cyc_Toc__region_calloc_str={_tmp7E,_tmp7E + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmp81[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmp81,_tmp81 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp84[12]="_new_region";static struct
_dynforward_ptr Cyc_Toc__new_region_str={_tmp84,_tmp84 + 12};static struct _tuple1
Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmp87[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmp87,_tmp87 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp8A[12]="_pop_region";static struct
_dynforward_ptr Cyc_Toc__pop_region_str={_tmp8A,_tmp8A + 12};static struct _tuple1
Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmp8D[16]="_open_dynregion";static struct _dynforward_ptr Cyc_Toc__open_dynregion_str={
_tmp8D,_tmp8D + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)((void*)& Cyc_Toc__open_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=&
Cyc_Toc__open_dynregion_ev;static char _tmp90[16]="_push_dynregion";static struct
_dynforward_ptr Cyc_Toc__push_dynregion_str={_tmp90,_tmp90 + 16};static struct
_tuple1 Cyc_Toc__push_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={
0,(void*)((void*)& Cyc_Toc__push_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;
static char _tmp93[15]="_pop_dynregion";static struct _dynforward_ptr Cyc_Toc__pop_dynregion_str={
_tmp93,_tmp93 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)((void*)& Cyc_Toc__pop_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=&
Cyc_Toc__pop_dynregion_ev;static char _tmp96[14]="_reset_region";static struct
_dynforward_ptr Cyc_Toc__reset_region_str={_tmp96,_tmp96 + 14};static struct _tuple1
Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={
0,(void*)((void*)& Cyc_Toc__reset_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;
static char _tmp99[19]="_throw_arraybounds";static struct _dynforward_ptr Cyc_Toc__throw_arraybounds_str={
_tmp99,_tmp99 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_arraybounds_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp9C[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmp9C,_tmp9C + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={1,&
Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmp9F[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp9F,
_tmp9F + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA2[11]="_swap_word";static
struct _dynforward_ptr Cyc_Toc__swap_word_str={_tmpA2,_tmpA2 + 11};static struct
_tuple1 Cyc_Toc__swap_word_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)((void*)& Cyc_Toc__swap_word_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char
_tmpA5[17]="_swap_dynforward";static struct _dynforward_ptr Cyc_Toc__swap_dynforward_str={
_tmpA5,_tmpA5 + 17};static struct _tuple1 Cyc_Toc__swap_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__swap_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__swap_dynforward_re={1,& Cyc_Toc__swap_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__swap_dynforward_ev={0,(void*)((void*)& Cyc_Toc__swap_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dynforward_e=&
Cyc_Toc__swap_dynforward_ev;static char _tmpA8[16]="_swap_dyneither";static struct
_dynforward_ptr Cyc_Toc__swap_dyneither_str={_tmpA8,_tmpA8 + 16};static struct
_tuple1 Cyc_Toc__swap_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={
1,& Cyc_Toc__swap_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__swap_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dynforward_ptr_pr}),
0}};static void*Cyc_Toc_dynforward_ptr_typ=(void*)& Cyc_Toc_dynforward_ptr_typ_v;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),
0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**_tmpAF=_cycalloc(sizeof(*
_tmpAF));_tmpAF[0]=Cyc_Absyn_skip_stmt(0);_tmpAF;});return*skip_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(
t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));
_tmpB0[0]=({struct Cyc_Absyn_Cast_e_struct _tmpB1;_tmpB1.tag=15;_tmpB1.f1=(void*)t;
_tmpB1.f2=e;_tmpB1.f3=0;_tmpB1.f4=(void*)((void*)1);_tmpB1;});_tmpB0;});}static
void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Deref_e_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_Deref_e_struct
_tmpB3;_tmpB3.tag=22;_tmpB3.f1=e;_tmpB3;});_tmpB2;});}static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*
_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_Subscript_e_struct
_tmpB5;_tmpB5.tag=25;_tmpB5.f1=e1;_tmpB5.f2=e2;_tmpB5;});_tmpB4;});}static void*
Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_StmtExp_e_struct
_tmpB7;_tmpB7.tag=38;_tmpB7.f1=s;_tmpB7;});_tmpB6;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpB8=_cycalloc(
sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpB9;_tmpB9.tag=
18;_tmpB9.f1=(void*)t;_tmpB9;});_tmpB8;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpBB;_tmpBB.tag=11;_tmpBB.f1=e;_tmpBB.f2=es;_tmpBB.f3=0;_tmpBB;});_tmpBA;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_Exp_s_struct _tmpBD;
_tmpBD.tag=0;_tmpBD.f1=e;_tmpBD;});_tmpBC;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_Seq_s_struct _tmpBF;
_tmpBF.tag=1;_tmpBF.f1=s1;_tmpBF.f2=s2;_tmpBF;});_tmpBE;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpC1;_tmpC1.tag=6;_tmpC1.f1=e1;
_tmpC1.f2=e2;_tmpC1.f3=e3;_tmpC1;});_tmpC0;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpC3;_tmpC3.tag=23;_tmpC3.f1=e;_tmpC3.f2=n;_tmpC3;});_tmpC2;});}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_AggrArrow_e_struct
_tmpC5;_tmpC5.tag=24;_tmpC5.f1=e;_tmpC5.f2=n;_tmpC5;});_tmpC4;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpC7;_tmpC7.tag=37;_tmpC7.f1=tdopt;_tmpC7.f2=ds;_tmpC7;});_tmpC6;});}static
void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){return(
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpC9;_tmpC9.tag=7;_tmpC9.f1=v;_tmpC9.f2=s;_tmpC9;});
_tmpC8;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),0})};struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpCC=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpCD;char _tmpCE;union Cyc_Absyn_Cnst_union
_tmpCF;short _tmpD0;union Cyc_Absyn_Cnst_union _tmpD1;int _tmpD2;union Cyc_Absyn_Cnst_union
_tmpD3;long long _tmpD4;union Cyc_Absyn_Cnst_union _tmpD5;struct Cyc_Absyn_Exp*
_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*
_tmpD9;struct Cyc_List_List*_tmpDA;struct Cyc_List_List*_tmpDB;_LL1: if(*((int*)
_tmpCC)!= 0)goto _LL3;_tmpCD=((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1).Char_c).tag != 0)goto _LL3;_tmpCE=(
_tmpCD.Char_c).f2;_LL2: return _tmpCE == '\000';_LL3: if(*((int*)_tmpCC)!= 0)goto
_LL5;_tmpCF=((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpCC)->f1).Short_c).tag != 1)goto _LL5;_tmpD0=(_tmpCF.Short_c).f2;_LL4: return
_tmpD0 == 0;_LL5: if(*((int*)_tmpCC)!= 0)goto _LL7;_tmpD1=((struct Cyc_Absyn_Const_e_struct*)
_tmpCC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1).Int_c).tag != 2)
goto _LL7;_tmpD2=(_tmpD1.Int_c).f2;_LL6: return _tmpD2 == 0;_LL7: if(*((int*)_tmpCC)
!= 0)goto _LL9;_tmpD3=((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmpCC)->f1).LongLong_c).tag != 3)goto _LL9;_tmpD4=(
_tmpD3.LongLong_c).f2;_LL8: return _tmpD4 == 0;_LL9: if(*((int*)_tmpCC)!= 0)goto _LLB;
_tmpD5=((struct Cyc_Absyn_Const_e_struct*)_tmpCC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpCC)->f1).Null_c).tag != 6)goto _LLB;_LLA: return 1;_LLB: if(*((int*)_tmpCC)!= 15)
goto _LLD;_tmpD6=((struct Cyc_Absyn_Cast_e_struct*)_tmpCC)->f2;_LLC: return Cyc_Toc_is_zero(
_tmpD6);_LLD: if(*((int*)_tmpCC)!= 26)goto _LLF;_tmpD7=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpCC)->f1;_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpD7);_LLF: if(*((int*)_tmpCC)!= 28)goto
_LL11;_tmpD8=((struct Cyc_Absyn_Array_e_struct*)_tmpCC)->f1;_LL10: _tmpD9=_tmpD8;
goto _LL12;_LL11: if(*((int*)_tmpCC)!= 30)goto _LL13;_tmpD9=((struct Cyc_Absyn_Struct_e_struct*)
_tmpCC)->f3;_LL12: _tmpDA=_tmpD9;goto _LL14;_LL13: if(*((int*)_tmpCC)!= 27)goto
_LL15;_tmpDA=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpCC)->f2;_LL14: _tmpDB=
_tmpDA;goto _LL16;_LL15: if(*((int*)_tmpCC)!= 37)goto _LL17;_tmpDB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpCC)->f2;_LL16: for(0;_tmpDB != 0;_tmpDB=_tmpDB->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple6*)_tmpDB->hd)).f2))return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpDC=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpDD;struct Cyc_Absyn_PtrAtts _tmpDE;struct Cyc_Absyn_Conref*
_tmpDF;_LL1A: if(_tmpDC <= (void*)4)goto _LL1C;if(*((int*)_tmpDC)!= 4)goto _LL1C;
_tmpDD=((struct Cyc_Absyn_PointerType_struct*)_tmpDC)->f1;_tmpDE=_tmpDD.ptr_atts;
_tmpDF=_tmpDE.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpDF);_LL1C:;_LL1D:({void*_tmpE0[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpE1="is_nullable";
_tag_dynforward(_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,12));}),
_tag_dynforward(_tmpE0,sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dynforward_ptr tag){return({struct _tuple1*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2->f1=(*x).f1;_tmpE2->f2=({struct _dynforward_ptr*_tmpE3=
_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)*(*x).f2,(struct _dynforward_ptr)tag);_tmpE3;});_tmpE2;});}
struct _tuple7{void*f1;struct Cyc_List_List*f2;};static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmpE5;struct Cyc_List_List**
_tmpE6;struct Cyc_Toc_TocState _tmpE4=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmpE5=_tmpE4.dyn;_tmpE6=_tmpE4.tuple_types;{struct _DynRegionFrame _tmpE7;struct
_RegionHandle*d=_open_dynregion(& _tmpE7,_tmpE5);{struct Cyc_List_List*_tmpE8=*
_tmpE6;for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){struct _tuple7 _tmpEA;void*_tmpEB;
struct Cyc_List_List*_tmpEC;struct _tuple7*_tmpE9=(struct _tuple7*)_tmpE8->hd;
_tmpEA=*_tmpE9;_tmpEB=_tmpEA.f1;_tmpEC=_tmpEA.f2;{struct Cyc_List_List*_tmpED=
tqs0;for(0;_tmpED != 0  && _tmpEC != 0;(_tmpED=_tmpED->tl,_tmpEC=_tmpEC->tl)){if(!
Cyc_Tcutil_unify((*((struct _tuple4*)_tmpED->hd)).f2,(void*)_tmpEC->hd))break;}
if(_tmpED == 0  && _tmpEC == 0){void*_tmpEE=_tmpEB;_npop_handler(0);return _tmpEE;}}}}{
struct _dynforward_ptr*xname=({struct _dynforward_ptr*_tmpFD=_cycalloc(sizeof(*
_tmpFD));_tmpFD[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp100;
_tmp100.tag=1;_tmp100.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;{void*
_tmpFE[1]={& _tmp100};Cyc_aprintf(({const char*_tmpFF="_tuple%d";_tag_dynforward(
_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,9));}),_tag_dynforward(_tmpFE,
sizeof(void*),1));}});_tmpFD;});void*x=Cyc_Absyn_strct(xname);struct Cyc_List_List*
ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple4*),
struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*
_tmpEF=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmpEF=({struct Cyc_List_List*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->hd=({
struct Cyc_Absyn_Aggrfield*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->name=Cyc_Absyn_fieldname(
i);_tmpF1->tq=Cyc_Toc_mt_tq;_tmpF1->type=(void*)((void*)ts2->hd);_tmpF1->width=0;
_tmpF1->attributes=0;_tmpF1;});_tmpF0->tl=_tmpEF;_tmpF0;});}}_tmpEF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpEF);{struct Cyc_Absyn_Aggrdecl*
_tmpF2=({struct Cyc_Absyn_Aggrdecl*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->kind=(
void*)((void*)0);_tmpF9->sc=(void*)((void*)2);_tmpF9->name=({struct _tuple1*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmpFC;(_tmpFC.Rel_n).tag=1;(_tmpFC.Rel_n).f1=0;
_tmpFC;});_tmpFB->f2=xname;_tmpFB;});_tmpF9->tvs=0;_tmpF9->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->exist_vars=0;_tmpFA->rgn_po=0;_tmpFA->fields=
_tmpEF;_tmpFA;});_tmpF9->attributes=0;_tmpF9;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmpF5;_tmpF5.tag=4;_tmpF5.f1=_tmpF2;_tmpF5;});_tmpF4;}),0);_tmpF3->tl=Cyc_Toc_result_decls;
_tmpF3;});*_tmpE6=({struct Cyc_List_List*_tmpF6=_region_malloc(d,sizeof(*_tmpF6));
_tmpF6->hd=({struct _tuple7*_tmpF7=_region_malloc(d,sizeof(*_tmpF7));_tmpF7->f1=x;
_tmpF7->f2=ts;_tmpF7;});_tmpF6->tl=*_tmpE6;_tmpF6;});{void*_tmpF8=x;
_npop_handler(0);return _tmpF8;}}};_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp106;(_tmp106.Loc_n).tag=0;_tmp106;});_tmp101->f2=({
struct _dynforward_ptr*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp105;_tmp105.tag=1;_tmp105.f1=(
unsigned int)Cyc_Toc_temp_var_counter ++;{void*_tmp103[1]={& _tmp105};Cyc_aprintf(({
const char*_tmp104="_tmp%X";_tag_dynforward(_tmp104,sizeof(char),
_get_zero_arr_size(_tmp104,7));}),_tag_dynforward(_tmp103,sizeof(void*),1));}});
_tmp102;});_tmp101;});}static struct _dynforward_ptr*Cyc_Toc_fresh_label(){return({
struct _dynforward_ptr*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp10A;_tmp10A.tag=1;_tmp10A.f1=(
unsigned int)Cyc_Toc_fresh_label_counter ++;{void*_tmp108[1]={& _tmp10A};Cyc_aprintf(({
const char*_tmp109="_LL%X";_tag_dynforward(_tmp109,sizeof(char),
_get_zero_arr_size(_tmp109,6));}),_tag_dynforward(_tmp108,sizeof(void*),1));}});
_tmp107;});}static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp10B=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp10B))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp10B->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp10B->hd)->typs == 0)
++ ans;_tmp10B=_tmp10B->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmp10C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmp10C != 0;_tmp10C=_tmp10C->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmp10C->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmp10E;struct Cyc_Absyn_Tqual
_tmp10F;void*_tmp110;struct _tuple2 _tmp10D=*a;_tmp10E=_tmp10D.f1;_tmp10F=_tmp10D.f2;
_tmp110=_tmp10D.f3;return({struct _tuple2*_tmp111=_cycalloc(sizeof(*_tmp111));
_tmp111->f1=_tmp10E;_tmp111->f2=_tmp10F;_tmp111->f3=Cyc_Toc_typ_to_c(_tmp110);
_tmp111;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual
_tmp113;void*_tmp114;struct _tuple4 _tmp112=*x;_tmp113=_tmp112.f1;_tmp114=_tmp112.f2;
return({struct _tuple4*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->f1=_tmp113;
_tmp115->f2=Cyc_Toc_typ_to_c(_tmp114);_tmp115;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp116=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp117;
void*_tmp118;struct Cyc_Absyn_Tqual _tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Conref*
_tmp11B;struct Cyc_Position_Segment*_tmp11C;struct Cyc_Core_Opt*_tmp11D;struct Cyc_Core_Opt
_tmp11E;void*_tmp11F;_LL1F: if(_tmp116 <= (void*)4)goto _LL23;if(*((int*)_tmp116)!= 
7)goto _LL21;_tmp117=((struct Cyc_Absyn_ArrayType_struct*)_tmp116)->f1;_tmp118=(
void*)_tmp117.elt_type;_tmp119=_tmp117.tq;_tmp11A=_tmp117.num_elts;_tmp11B=
_tmp117.zero_term;_tmp11C=_tmp117.zt_loc;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp118),_tmp119,_tmp11A,Cyc_Absyn_false_conref,_tmp11C);_LL21: if(*((int*)
_tmp116)!= 0)goto _LL23;_tmp11D=((struct Cyc_Absyn_Evar_struct*)_tmp116)->f2;if(
_tmp11D == 0)goto _LL23;_tmp11E=*_tmp11D;_tmp11F=(void*)_tmp11E.v;_LL22: return Cyc_Toc_typ_to_c_array(
_tmp11F);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->name=f->name;_tmp120->tq=Cyc_Toc_mt_tq;
_tmp120->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp120->width=f->width;
_tmp120->attributes=f->attributes;_tmp120;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp121=
t;struct Cyc_Core_Opt*_tmp122;struct Cyc_Core_Opt*_tmp123;struct Cyc_Core_Opt
_tmp124;void*_tmp125;struct Cyc_Absyn_Tvar*_tmp126;struct Cyc_Absyn_TunionInfo
_tmp127;union Cyc_Absyn_TunionInfoU_union _tmp128;struct Cyc_Absyn_Tuniondecl**
_tmp129;struct Cyc_Absyn_Tuniondecl*_tmp12A;struct Cyc_Absyn_TunionFieldInfo
_tmp12B;union Cyc_Absyn_TunionFieldInfoU_union _tmp12C;struct Cyc_Absyn_Tuniondecl*
_tmp12D;struct Cyc_Absyn_Tunionfield*_tmp12E;struct Cyc_Absyn_PtrInfo _tmp12F;void*
_tmp130;struct Cyc_Absyn_Tqual _tmp131;struct Cyc_Absyn_PtrAtts _tmp132;struct Cyc_Absyn_Conref*
_tmp133;struct Cyc_Absyn_ArrayInfo _tmp134;void*_tmp135;struct Cyc_Absyn_Tqual
_tmp136;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Position_Segment*_tmp138;struct Cyc_Absyn_FnInfo
_tmp139;void*_tmp13A;struct Cyc_List_List*_tmp13B;int _tmp13C;struct Cyc_Absyn_VarargInfo*
_tmp13D;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*_tmp13F;void*_tmp140;
struct Cyc_List_List*_tmp141;struct Cyc_Absyn_AggrInfo _tmp142;union Cyc_Absyn_AggrInfoU_union
_tmp143;struct Cyc_List_List*_tmp144;struct _tuple1*_tmp145;struct Cyc_List_List*
_tmp146;struct _tuple1*_tmp147;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_Typedefdecl*
_tmp149;void**_tmp14A;void*_tmp14B;_LL26: if((int)_tmp121 != 0)goto _LL28;_LL27:
return t;_LL28: if(_tmp121 <= (void*)4)goto _LL3A;if(*((int*)_tmp121)!= 0)goto _LL2A;
_tmp122=((struct Cyc_Absyn_Evar_struct*)_tmp121)->f2;if(_tmp122 != 0)goto _LL2A;
_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(*((int*)_tmp121)!= 0)goto _LL2C;_tmp123=((
struct Cyc_Absyn_Evar_struct*)_tmp121)->f2;if(_tmp123 == 0)goto _LL2C;_tmp124=*
_tmp123;_tmp125=(void*)_tmp124.v;_LL2B: return Cyc_Toc_typ_to_c(_tmp125);_LL2C: if(*((
int*)_tmp121)!= 1)goto _LL2E;_tmp126=((struct Cyc_Absyn_VarType_struct*)_tmp121)->f1;
_LL2D: if(Cyc_Tcutil_tvar_kind(_tmp126)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL2E: if(*((int*)_tmp121)!= 2)goto _LL30;_tmp127=((struct Cyc_Absyn_TunionType_struct*)
_tmp121)->f1;_tmp128=_tmp127.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp121)->f1).tunion_info).KnownTunion).tag != 1)goto _LL30;_tmp129=(_tmp128.KnownTunion).f1;
_tmp12A=*_tmp129;_LL2F: if(_tmp12A->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp12A->name,({const char*_tmp14C="_union";_tag_dynforward(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,7));})));else{return Cyc_Absyn_void_star_typ();}_LL30:
if(*((int*)_tmp121)!= 2)goto _LL32;_LL31:({void*_tmp14D[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp14E="unresolved TunionType";_tag_dynforward(_tmp14E,sizeof(char),
_get_zero_arr_size(_tmp14E,22));}),_tag_dynforward(_tmp14D,sizeof(void*),0));});
_LL32: if(*((int*)_tmp121)!= 3)goto _LL34;_tmp12B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp121)->f1;_tmp12C=_tmp12B.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp121)->f1).field_info).KnownTunionfield).tag != 1)goto _LL34;_tmp12D=(_tmp12C.KnownTunionfield).f1;
_tmp12E=(_tmp12C.KnownTunionfield).f2;_LL33: if(_tmp12D->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp12D->name,({const char*_tmp14F="_union";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,7));})));if(
_tmp12E->typs == 0){if(_tmp12D->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp12E->name,({const char*
_tmp150="_struct";_tag_dynforward(_tmp150,sizeof(char),_get_zero_arr_size(
_tmp150,8));})));}_LL34: if(*((int*)_tmp121)!= 3)goto _LL36;_LL35:({void*_tmp151[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp152="unresolved TunionFieldType";_tag_dynforward(_tmp152,sizeof(
char),_get_zero_arr_size(_tmp152,27));}),_tag_dynforward(_tmp151,sizeof(void*),0));});
_LL36: if(*((int*)_tmp121)!= 4)goto _LL38;_tmp12F=((struct Cyc_Absyn_PointerType_struct*)
_tmp121)->f1;_tmp130=(void*)_tmp12F.elt_typ;_tmp131=_tmp12F.elt_tq;_tmp132=
_tmp12F.ptr_atts;_tmp133=_tmp132.bounds;_LL37: _tmp130=Cyc_Toc_typ_to_c_array(
_tmp130);{union Cyc_Absyn_Constraint_union _tmp153=(Cyc_Absyn_compress_conref(
_tmp133))->v;void*_tmp154;void*_tmp155;_LL63: if((_tmp153.No_constr).tag != 2)goto
_LL65;_LL64: goto _LL66;_LL65: if((_tmp153.Eq_constr).tag != 0)goto _LL67;_tmp154=(
_tmp153.Eq_constr).f1;if((int)_tmp154 != 0)goto _LL67;_LL66: return Cyc_Toc_dynforward_ptr_typ;
_LL67: if((_tmp153.Eq_constr).tag != 0)goto _LL69;_tmp155=(_tmp153.Eq_constr).f1;
if((int)_tmp155 != 1)goto _LL69;_LL68: return Cyc_Toc_dyneither_ptr_typ;_LL69:;_LL6A:
return Cyc_Absyn_star_typ(_tmp130,(void*)2,_tmp131,Cyc_Absyn_false_conref);_LL62:;}
_LL38: if(*((int*)_tmp121)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp121 != 
1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp121 <= (void*)4)goto _LL58;if(*((int*)
_tmp121)!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(*((int*)_tmp121)!= 7)goto _LL40;
_tmp134=((struct Cyc_Absyn_ArrayType_struct*)_tmp121)->f1;_tmp135=(void*)_tmp134.elt_type;
_tmp136=_tmp134.tq;_tmp137=_tmp134.num_elts;_tmp138=_tmp134.zt_loc;_LL3F: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp135),_tmp136,_tmp137,Cyc_Absyn_false_conref,
_tmp138);_LL40: if(*((int*)_tmp121)!= 8)goto _LL42;_tmp139=((struct Cyc_Absyn_FnType_struct*)
_tmp121)->f1;_tmp13A=(void*)_tmp139.ret_typ;_tmp13B=_tmp139.args;_tmp13C=_tmp139.c_varargs;
_tmp13D=_tmp139.cyc_varargs;_tmp13E=_tmp139.attributes;_LL41: {struct Cyc_List_List*
_tmp156=0;for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){void*_tmp157=(void*)_tmp13E->hd;
_LL6C: if((int)_tmp157 != 3)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)_tmp157 != 4)
goto _LL70;_LL6F: goto _LL71;_LL70: if(_tmp157 <= (void*)17)goto _LL74;if(*((int*)
_tmp157)!= 3)goto _LL72;_LL71: continue;_LL72: if(*((int*)_tmp157)!= 4)goto _LL74;
_LL73: continue;_LL74:;_LL75: _tmp156=({struct Cyc_List_List*_tmp158=_cycalloc(
sizeof(*_tmp158));_tmp158->hd=(void*)((void*)_tmp13E->hd);_tmp158->tl=_tmp156;
_tmp158;});goto _LL6B;_LL6B:;}{struct Cyc_List_List*_tmp159=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp13B);if(_tmp13D != 0){void*_tmp15A=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((
void*)_tmp13D->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp15B=({struct _tuple2*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->f1=
_tmp13D->name;_tmp15D->f2=_tmp13D->tq;_tmp15D->f3=_tmp15A;_tmp15D;});_tmp159=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp159,({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->hd=
_tmp15B;_tmp15C->tl=0;_tmp15C;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[0]=({struct Cyc_Absyn_FnType_struct
_tmp15F;_tmp15F.tag=8;_tmp15F.f1=({struct Cyc_Absyn_FnInfo _tmp160;_tmp160.tvars=0;
_tmp160.effect=0;_tmp160.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp13A);_tmp160.args=
_tmp159;_tmp160.c_varargs=_tmp13C;_tmp160.cyc_varargs=0;_tmp160.rgn_po=0;_tmp160.attributes=
_tmp156;_tmp160;});_tmp15F;});_tmp15E;});}}_LL42: if(*((int*)_tmp121)!= 9)goto
_LL44;_tmp13F=((struct Cyc_Absyn_TupleType_struct*)_tmp121)->f1;_LL43: _tmp13F=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp13F);return Cyc_Toc_add_tuple_type(_tmp13F);
_LL44: if(*((int*)_tmp121)!= 11)goto _LL46;_tmp140=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp121)->f1;_tmp141=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp121)->f2;_LL45:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp161=_cycalloc(sizeof(*
_tmp161));_tmp161[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp162;_tmp162.tag=11;
_tmp162.f1=(void*)_tmp140;_tmp162.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp141);_tmp162;});_tmp161;});_LL46: if(*((int*)_tmp121)!= 10)goto _LL48;_tmp142=((
struct Cyc_Absyn_AggrType_struct*)_tmp121)->f1;_tmp143=_tmp142.aggr_info;_tmp144=
_tmp142.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*_tmp163=Cyc_Absyn_get_known_aggrdecl(
_tmp143);if((void*)_tmp163->kind == (void*)1)return Cyc_Absyn_unionq_typ(_tmp163->name);
return Cyc_Absyn_strctq(_tmp163->name);}_LL48: if(*((int*)_tmp121)!= 12)goto _LL4A;
_tmp145=((struct Cyc_Absyn_EnumType_struct*)_tmp121)->f1;_LL49: return t;_LL4A: if(*((
int*)_tmp121)!= 13)goto _LL4C;_tmp146=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp121)->f1;_LL4B: Cyc_Toc_enumfields_to_c(_tmp146);return t;_LL4C: if(*((int*)
_tmp121)!= 17)goto _LL4E;_tmp147=((struct Cyc_Absyn_TypedefType_struct*)_tmp121)->f1;
_tmp148=((struct Cyc_Absyn_TypedefType_struct*)_tmp121)->f2;_tmp149=((struct Cyc_Absyn_TypedefType_struct*)
_tmp121)->f3;_tmp14A=((struct Cyc_Absyn_TypedefType_struct*)_tmp121)->f4;_LL4D:
if(_tmp14A == 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp164=
_cycalloc(sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp165;_tmp165.tag=17;_tmp165.f1=_tmp147;_tmp165.f2=0;_tmp165.f3=_tmp149;
_tmp165.f4=0;_tmp165;});_tmp164;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp167;_tmp167.tag=17;_tmp167.f1=_tmp147;_tmp167.f2=0;_tmp167.f3=_tmp149;
_tmp167.f4=({void**_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168[0]=Cyc_Toc_typ_to_c_array(*
_tmp14A);_tmp168;});_tmp167;});_tmp166;});}_LL4E: if(*((int*)_tmp121)!= 14)goto
_LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp121)!= 18)goto _LL52;_LL51: return Cyc_Absyn_uint_typ;
_LL52: if(*((int*)_tmp121)!= 15)goto _LL54;_tmp14B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp121)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(*((int*)_tmp121)!= 16)goto _LL56;_LL55: return Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_LL56: if(*((int*)
_tmp121)!= 19)goto _LL58;_LL57:({void*_tmp169[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp16A="Toc::typ_to_c: type translation passed a type integer";_tag_dynforward(
_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,54));}),_tag_dynforward(_tmp169,
sizeof(void*),0));});_LL58: if((int)_tmp121 != 2)goto _LL5A;_LL59:({void*_tmp16B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp16C="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,55));}),
_tag_dynforward(_tmp16B,sizeof(void*),0));});_LL5A: if((int)_tmp121 != 3)goto _LL5C;
_LL5B:({void*_tmp16D[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp16E="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp16E,sizeof(char),_get_zero_arr_size(_tmp16E,57));}),
_tag_dynforward(_tmp16D,sizeof(void*),0));});_LL5C: if(_tmp121 <= (void*)4)goto
_LL5E;if(*((int*)_tmp121)!= 20)goto _LL5E;_LL5D: goto _LL5F;_LL5E: if(_tmp121 <= (
void*)4)goto _LL60;if(*((int*)_tmp121)!= 21)goto _LL60;_LL5F: goto _LL61;_LL60: if(
_tmp121 <= (void*)4)goto _LL25;if(*((int*)_tmp121)!= 22)goto _LL25;_LL61:({void*
_tmp16F[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp170="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp170,sizeof(char),_get_zero_arr_size(_tmp170,49));}),
_tag_dynforward(_tmp16F,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp171=t;struct Cyc_Absyn_ArrayInfo _tmp172;void*_tmp173;struct Cyc_Absyn_Tqual
_tmp174;_LL77: if(_tmp171 <= (void*)4)goto _LL79;if(*((int*)_tmp171)!= 7)goto _LL79;
_tmp172=((struct Cyc_Absyn_ArrayType_struct*)_tmp171)->f1;_tmp173=(void*)_tmp172.elt_type;
_tmp174=_tmp172.tq;_LL78: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp173,(void*)
2,_tmp174,Cyc_Absyn_false_conref),e);_LL79:;_LL7A: return Cyc_Toc_cast_it(t,e);
_LL76:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp175=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp176;void*_tmp177;struct Cyc_Absyn_AggrInfo _tmp178;
union Cyc_Absyn_AggrInfoU_union _tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_TunionFieldInfo
_tmp17B;union Cyc_Absyn_TunionFieldInfoU_union _tmp17C;struct Cyc_Absyn_Tuniondecl*
_tmp17D;struct Cyc_Absyn_Tunionfield*_tmp17E;struct Cyc_List_List*_tmp17F;_LL7C:
if((int)_tmp175 != 0)goto _LL7E;_LL7D: return 1;_LL7E: if(_tmp175 <= (void*)4)goto
_LL86;if(*((int*)_tmp175)!= 1)goto _LL80;_LL7F: return 0;_LL80: if(*((int*)_tmp175)
!= 5)goto _LL82;_LL81: goto _LL83;_LL82: if(*((int*)_tmp175)!= 12)goto _LL84;_LL83:
goto _LL85;_LL84: if(*((int*)_tmp175)!= 13)goto _LL86;_LL85: goto _LL87;_LL86: if((int)
_tmp175 != 1)goto _LL88;_LL87: goto _LL89;_LL88: if(_tmp175 <= (void*)4)goto _LLA0;if(*((
int*)_tmp175)!= 6)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)_tmp175)!= 8)goto
_LL8C;_LL8B: goto _LL8D;_LL8C: if(*((int*)_tmp175)!= 18)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: if(*((int*)_tmp175)!= 14)goto _LL90;_LL8F: return 1;_LL90: if(*((int*)_tmp175)
!= 7)goto _LL92;_tmp176=((struct Cyc_Absyn_ArrayType_struct*)_tmp175)->f1;_tmp177=(
void*)_tmp176.elt_type;_LL91: return Cyc_Toc_atomic_typ(_tmp177);_LL92: if(*((int*)
_tmp175)!= 10)goto _LL94;_tmp178=((struct Cyc_Absyn_AggrType_struct*)_tmp175)->f1;
_tmp179=_tmp178.aggr_info;_LL93:{union Cyc_Absyn_AggrInfoU_union _tmp180=_tmp179;
_LLA3: if((_tmp180.UnknownAggr).tag != 0)goto _LLA5;_LLA4: return 0;_LLA5:;_LLA6: goto
_LLA2;_LLA2:;}{struct Cyc_Absyn_Aggrdecl*_tmp181=Cyc_Absyn_get_known_aggrdecl(
_tmp179);if(_tmp181->impl == 0)return 0;{struct Cyc_List_List*_tmp182=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp181->impl))->fields;for(0;_tmp182 != 0;_tmp182=_tmp182->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp182->hd)->type))
return 0;}}return 1;}_LL94: if(*((int*)_tmp175)!= 11)goto _LL96;_tmp17A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp175)->f2;_LL95: for(0;_tmp17A != 0;_tmp17A=_tmp17A->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp17A->hd)->type))return 0;}return 1;_LL96:
if(*((int*)_tmp175)!= 3)goto _LL98;_tmp17B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp175)->f1;_tmp17C=_tmp17B.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp175)->f1).field_info).KnownTunionfield).tag != 1)goto _LL98;_tmp17D=(_tmp17C.KnownTunionfield).f1;
_tmp17E=(_tmp17C.KnownTunionfield).f2;_LL97: _tmp17F=_tmp17E->typs;goto _LL99;
_LL98: if(*((int*)_tmp175)!= 9)goto _LL9A;_tmp17F=((struct Cyc_Absyn_TupleType_struct*)
_tmp175)->f1;_LL99: for(0;_tmp17F != 0;_tmp17F=_tmp17F->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp17F->hd)).f2))return 0;}return 1;_LL9A: if(*((int*)_tmp175)!= 2)
goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp175)!= 4)goto _LL9E;_LL9D: goto
_LL9F;_LL9E: if(*((int*)_tmp175)!= 15)goto _LLA0;_LL9F: return 0;_LLA0:;_LLA1:({
struct Cyc_String_pa_struct _tmp185;_tmp185.tag=0;_tmp185.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp183[
1]={& _tmp185};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp184="atomic_typ:  bad type %s";_tag_dynforward(_tmp184,sizeof(char),
_get_zero_arr_size(_tmp184,25));}),_tag_dynforward(_tmp183,sizeof(void*),1));}});
_LL7B:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp186=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp187;void*_tmp188;_LLA8: if(_tmp186 <= (void*)4)goto
_LLAA;if(*((int*)_tmp186)!= 4)goto _LLAA;_tmp187=((struct Cyc_Absyn_PointerType_struct*)
_tmp186)->f1;_tmp188=(void*)_tmp187.elt_typ;_LLA9: {void*_tmp189=Cyc_Tcutil_compress(
_tmp188);_LLAD: if((int)_tmp189 != 0)goto _LLAF;_LLAE: return 1;_LLAF:;_LLB0: return 0;
_LLAC:;}_LLAA:;_LLAB: return 0;_LLA7:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp18A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp18B;union Cyc_Absyn_AggrInfoU_union _tmp18C;struct Cyc_List_List*_tmp18D;_LLB2:
if(_tmp18A <= (void*)4)goto _LLB6;if(*((int*)_tmp18A)!= 10)goto _LLB4;_tmp18B=((
struct Cyc_Absyn_AggrType_struct*)_tmp18A)->f1;_tmp18C=_tmp18B.aggr_info;_LLB3: {
struct Cyc_Absyn_Aggrdecl*_tmp18E=Cyc_Absyn_get_known_aggrdecl(_tmp18C);if(
_tmp18E->impl == 0)({void*_tmp18F[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp190="is_poly_field: type missing fields";
_tag_dynforward(_tmp190,sizeof(char),_get_zero_arr_size(_tmp190,35));}),
_tag_dynforward(_tmp18F,sizeof(void*),0));});_tmp18D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp18E->impl))->fields;goto _LLB5;}_LLB4: if(*((int*)_tmp18A)!= 11)
goto _LLB6;_tmp18D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp18A)->f2;_LLB5: {
struct Cyc_Absyn_Aggrfield*_tmp191=Cyc_Absyn_lookup_field(_tmp18D,f);if(_tmp191 == 
0)({struct Cyc_String_pa_struct _tmp194;_tmp194.tag=0;_tmp194.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp192[1]={& _tmp194};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp193="is_poly_field: bad field %s";_tag_dynforward(_tmp193,sizeof(char),
_get_zero_arr_size(_tmp193,28));}),_tag_dynforward(_tmp192,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp191->type);}_LLB6:;_LLB7:({struct Cyc_String_pa_struct
_tmp197;_tmp197.tag=0;_tmp197.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp195[1]={& _tmp197};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp196="is_poly_field: bad type %s";_tag_dynforward(_tmp196,sizeof(char),
_get_zero_arr_size(_tmp196,27));}),_tag_dynforward(_tmp195,sizeof(void*),1));}});
_LLB1:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp198=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp199;struct _dynforward_ptr*_tmp19A;struct Cyc_Absyn_Exp*
_tmp19B;struct _dynforward_ptr*_tmp19C;_LLB9: if(*((int*)_tmp198)!= 23)goto _LLBB;
_tmp199=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp198)->f1;_tmp19A=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp198)->f2;_LLBA: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp199->topt))->v,_tmp19A);_LLBB: if(*((int*)_tmp198)!= 24)goto _LLBD;
_tmp19B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp198)->f1;_tmp19C=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp198)->f2;_LLBC: {void*_tmp19D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp19B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp19E;void*_tmp19F;
_LLC0: if(_tmp19D <= (void*)4)goto _LLC2;if(*((int*)_tmp19D)!= 4)goto _LLC2;_tmp19E=((
struct Cyc_Absyn_PointerType_struct*)_tmp19D)->f1;_tmp19F=(void*)_tmp19E.elt_typ;
_LLC1: return Cyc_Toc_is_poly_field(_tmp19F,_tmp19C);_LLC2:;_LLC3:({struct Cyc_String_pa_struct
_tmp1A2;_tmp1A2.tag=0;_tmp1A2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp19B->topt))->v));{
void*_tmp1A0[1]={& _tmp1A2};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1A1="is_poly_project: bad type %s";
_tag_dynforward(_tmp1A1,sizeof(char),_get_zero_arr_size(_tmp1A1,29));}),
_tag_dynforward(_tmp1A0,sizeof(void*),1));}});_LLBF:;}_LLBD:;_LLBE: return 0;_LLB8:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));
_tmp1A3->hd=s;_tmp1A3->tl=0;_tmp1A3;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->hd=s;_tmp1A4->tl=
0;_tmp1A4;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp1A5[2];_tmp1A5[1]=s;_tmp1A5[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1A5,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp1A6[2];_tmp1A6[1]=n;
_tmp1A6[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1A6,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp1A7[2];_tmp1A7[1]=n;_tmp1A7[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp1A7,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp1A8[3];_tmp1A8[2]=n;_tmp1A8[1]=
s;_tmp1A8[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1A8,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->hd=e;_tmp1A9->tl=
0;_tmp1A9;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp1AA=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp1AB;_LLC5: if(*((int*)
_tmp1AA)!= 0)goto _LLC7;_tmp1AB=((struct Cyc_Absyn_Const_e_struct*)_tmp1AA)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp1AA)->f1).String_c).tag != 5)goto _LLC7;
_LLC6: is_string=1;goto _LLC4;_LLC7:;_LLC8: goto _LLC4;_LLC4:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=({
struct Cyc_Absyn_Var_d_struct _tmp1AE;_tmp1AE.tag=0;_tmp1AE.f1=vd;_tmp1AE;});
_tmp1AD;}),0);_tmp1AC->tl=Cyc_Toc_result_decls;_tmp1AC;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple6*_tmp1AF[3];_tmp1AF[2]=({struct _tuple6*_tmp1B2=_cycalloc(
sizeof(*_tmp1B2));_tmp1B2->f1=0;_tmp1B2->f2=xplussz;_tmp1B2;});_tmp1AF[1]=({
struct _tuple6*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->f1=0;_tmp1B1->f2=xexp;
_tmp1B1;});_tmp1AF[0]=({struct _tuple6*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));
_tmp1B0->f1=0;_tmp1B0->f2=xexp;_tmp1B0;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1AF,sizeof(struct _tuple6*),3));}),
0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*_tmp1B3[2];_tmp1B3[
1]=({struct _tuple6*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->f1=0;_tmp1B5->f2=
xplussz;_tmp1B5;});_tmp1B3[0]=({struct _tuple6*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));
_tmp1B4->f1=0;_tmp1B4->f2=xexp;_tmp1B4;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1B3,sizeof(struct _tuple6*),2));}),
0);}return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dynforward_ptr*label;
struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dynforward_ptr**break_lab;struct _dynforward_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp1B7;int _tmp1B8;struct Cyc_Toc_Env*
_tmp1B6=nv;_tmp1B7=*_tmp1B6;_tmp1B8=_tmp1B7.toplevel;return _tmp1B8;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){struct
Cyc_Toc_Env _tmp1BA;struct Cyc_Dict_Dict _tmp1BB;struct Cyc_Toc_Env*_tmp1B9=nv;
_tmp1BA=*_tmp1B9;_tmp1BB=_tmp1BA.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp1BB,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*_tmp1BC=
_region_malloc(r,sizeof(*_tmp1BC));_tmp1BC->break_lab=(struct _dynforward_ptr**)0;
_tmp1BC->continue_lab=(struct _dynforward_ptr**)0;_tmp1BC->fallthru_info=(struct
Cyc_Toc_FallthruInfo*)0;_tmp1BC->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp1BC->toplevel=(int)1;_tmp1BC;});}static struct Cyc_Toc_Env*
Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1BE;struct _dynforward_ptr**_tmp1BF;struct _dynforward_ptr**_tmp1C0;struct Cyc_Toc_FallthruInfo*
_tmp1C1;struct Cyc_Dict_Dict _tmp1C2;int _tmp1C3;struct Cyc_Toc_Env*_tmp1BD=e;
_tmp1BE=*_tmp1BD;_tmp1BF=_tmp1BE.break_lab;_tmp1C0=_tmp1BE.continue_lab;_tmp1C1=
_tmp1BE.fallthru_info;_tmp1C2=_tmp1BE.varmap;_tmp1C3=_tmp1BE.toplevel;return({
struct Cyc_Toc_Env*_tmp1C4=_region_malloc(r,sizeof(*_tmp1C4));_tmp1C4->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1BF);_tmp1C4->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1C0);_tmp1C4->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1C1;_tmp1C4->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1C2);_tmp1C4->toplevel=(int)_tmp1C3;_tmp1C4;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1C6;struct _dynforward_ptr**_tmp1C7;struct _dynforward_ptr**_tmp1C8;struct Cyc_Toc_FallthruInfo*
_tmp1C9;struct Cyc_Dict_Dict _tmp1CA;int _tmp1CB;struct Cyc_Toc_Env*_tmp1C5=e;
_tmp1C6=*_tmp1C5;_tmp1C7=_tmp1C6.break_lab;_tmp1C8=_tmp1C6.continue_lab;_tmp1C9=
_tmp1C6.fallthru_info;_tmp1CA=_tmp1C6.varmap;_tmp1CB=_tmp1C6.toplevel;return({
struct Cyc_Toc_Env*_tmp1CC=_region_malloc(r,sizeof(*_tmp1CC));_tmp1CC->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1C7);_tmp1CC->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1C8);_tmp1CC->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1C9;_tmp1CC->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1CA);_tmp1CC->toplevel=(int)0;_tmp1CC;});}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp1CE;struct
_dynforward_ptr**_tmp1CF;struct _dynforward_ptr**_tmp1D0;struct Cyc_Toc_FallthruInfo*
_tmp1D1;struct Cyc_Dict_Dict _tmp1D2;int _tmp1D3;struct Cyc_Toc_Env*_tmp1CD=e;
_tmp1CE=*_tmp1CD;_tmp1CF=_tmp1CE.break_lab;_tmp1D0=_tmp1CE.continue_lab;_tmp1D1=
_tmp1CE.fallthru_info;_tmp1D2=_tmp1CE.varmap;_tmp1D3=_tmp1CE.toplevel;return({
struct Cyc_Toc_Env*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));_tmp1D4->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1CF);_tmp1D4->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1D0);_tmp1D4->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1D1;_tmp1D4->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1D2);_tmp1D4->toplevel=(int)1;_tmp1D4;});}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp1D5=(*x).f1;_LLCA: if((_tmp1D5.Rel_n).tag != 1)
goto _LLCC;_LLCB:({struct Cyc_String_pa_struct _tmp1D8;_tmp1D8.tag=0;_tmp1D8.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp1D6[1]={& _tmp1D8};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1D7="Toc::add_varmap on Rel_n: %s\n";
_tag_dynforward(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,30));}),
_tag_dynforward(_tmp1D6,sizeof(void*),1));}});_LLCC:;_LLCD: goto _LLC9;_LLC9:;}{
struct Cyc_Toc_Env _tmp1DA;struct _dynforward_ptr**_tmp1DB;struct _dynforward_ptr**
_tmp1DC;struct Cyc_Toc_FallthruInfo*_tmp1DD;struct Cyc_Dict_Dict _tmp1DE;int _tmp1DF;
struct Cyc_Toc_Env*_tmp1D9=e;_tmp1DA=*_tmp1D9;_tmp1DB=_tmp1DA.break_lab;_tmp1DC=
_tmp1DA.continue_lab;_tmp1DD=_tmp1DA.fallthru_info;_tmp1DE=_tmp1DA.varmap;
_tmp1DF=_tmp1DA.toplevel;{struct Cyc_Dict_Dict _tmp1E0=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1DE),x,y);return({struct Cyc_Toc_Env*_tmp1E1=_region_malloc(r,sizeof(*
_tmp1E1));_tmp1E1->break_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp1DB);_tmp1E1->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp1DC);_tmp1E1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp1DD;_tmp1E1->varmap=(
struct Cyc_Dict_Dict)_tmp1E0;_tmp1E1->toplevel=(int)_tmp1DF;_tmp1E1;});}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp1E3;struct _dynforward_ptr**_tmp1E4;struct _dynforward_ptr**
_tmp1E5;struct Cyc_Toc_FallthruInfo*_tmp1E6;struct Cyc_Dict_Dict _tmp1E7;int _tmp1E8;
struct Cyc_Toc_Env*_tmp1E2=e;_tmp1E3=*_tmp1E2;_tmp1E4=_tmp1E3.break_lab;_tmp1E5=
_tmp1E3.continue_lab;_tmp1E6=_tmp1E3.fallthru_info;_tmp1E7=_tmp1E3.varmap;
_tmp1E8=_tmp1E3.toplevel;return({struct Cyc_Toc_Env*_tmp1E9=_region_malloc(r,
sizeof(*_tmp1E9));_tmp1E9->break_lab=(struct _dynforward_ptr**)0;_tmp1E9->continue_lab=(
struct _dynforward_ptr**)0;_tmp1E9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1E6;_tmp1E9->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1E7);_tmp1E9->toplevel=(
int)_tmp1E8;_tmp1E9;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct
_dynforward_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp1EA=_region_malloc(r,sizeof(*_tmp1EA));_tmp1EA->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp1EA->tl=fallthru_vars;_tmp1EA;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp1EC;struct _dynforward_ptr**_tmp1ED;struct _dynforward_ptr**
_tmp1EE;struct Cyc_Toc_FallthruInfo*_tmp1EF;struct Cyc_Dict_Dict _tmp1F0;int _tmp1F1;
struct Cyc_Toc_Env*_tmp1EB=e;_tmp1EC=*_tmp1EB;_tmp1ED=_tmp1EC.break_lab;_tmp1EE=
_tmp1EC.continue_lab;_tmp1EF=_tmp1EC.fallthru_info;_tmp1F0=_tmp1EC.varmap;
_tmp1F1=_tmp1EC.toplevel;{struct Cyc_Toc_Env _tmp1F3;struct Cyc_Dict_Dict _tmp1F4;
struct Cyc_Toc_Env*_tmp1F2=next_case_env;_tmp1F3=*_tmp1F2;_tmp1F4=_tmp1F3.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1F7=
_region_malloc(r,sizeof(*_tmp1F7));_tmp1F7->label=fallthru_l;_tmp1F7->binders=
fallthru_vars;_tmp1F7->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1F4);_tmp1F7;});
return({struct Cyc_Toc_Env*_tmp1F5=_region_malloc(r,sizeof(*_tmp1F5));_tmp1F5->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp1F6=_region_malloc(r,
sizeof(*_tmp1F6));_tmp1F6[0]=break_l;_tmp1F6;});_tmp1F5->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp1EE);_tmp1F5->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp1F5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1F0);_tmp1F5->toplevel=(
int)_tmp1F1;_tmp1F5;});}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct
Cyc_Toc_Env _tmp1F9;struct _dynforward_ptr**_tmp1FA;struct _dynforward_ptr**_tmp1FB;
struct Cyc_Toc_FallthruInfo*_tmp1FC;struct Cyc_Dict_Dict _tmp1FD;int _tmp1FE;struct
Cyc_Toc_Env*_tmp1F8=e;_tmp1F9=*_tmp1F8;_tmp1FA=_tmp1F9.break_lab;_tmp1FB=_tmp1F9.continue_lab;
_tmp1FC=_tmp1F9.fallthru_info;_tmp1FD=_tmp1F9.varmap;_tmp1FE=_tmp1F9.toplevel;
return({struct Cyc_Toc_Env*_tmp1FF=_region_malloc(r,sizeof(*_tmp1FF));_tmp1FF->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp200=_region_malloc(r,
sizeof(*_tmp200));_tmp200[0]=break_l;_tmp200;});_tmp1FF->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp1FB);_tmp1FF->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp1FF->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1FD);_tmp1FF->toplevel=(
int)_tmp1FE;_tmp1FF;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct
Cyc_Toc_Env _tmp202;struct _dynforward_ptr**_tmp203;struct _dynforward_ptr**_tmp204;
struct Cyc_Toc_FallthruInfo*_tmp205;struct Cyc_Dict_Dict _tmp206;int _tmp207;struct
Cyc_Toc_Env*_tmp201=e;_tmp202=*_tmp201;_tmp203=_tmp202.break_lab;_tmp204=_tmp202.continue_lab;
_tmp205=_tmp202.fallthru_info;_tmp206=_tmp202.varmap;_tmp207=_tmp202.toplevel;
return({struct Cyc_Toc_Env*_tmp208=_region_malloc(r,sizeof(*_tmp208));_tmp208->break_lab=(
struct _dynforward_ptr**)0;_tmp208->continue_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp204);_tmp208->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp209=_region_malloc(r,sizeof(*_tmp209));_tmp209->label=
next_l;_tmp209->binders=0;_tmp209->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp209;});_tmp208->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp206);_tmp208->toplevel=(
int)_tmp207;_tmp208;});}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp20A=(void*)e->annot;
_LLCF: if(*((void**)_tmp20A)!= Cyc_CfFlowInfo_UnknownZ)goto _LLD1;_LLD0: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD1: if(*((void**)_tmp20A)
!= Cyc_CfFlowInfo_NotZero)goto _LLD3;_LLD2: return 0;_LLD3: if(_tmp20A != Cyc_CfFlowInfo_IsZero)
goto _LLD5;_LLD4:({void*_tmp20B[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp20C="dereference of NULL pointer";
_tag_dynforward(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,28));}),
_tag_dynforward(_tmp20B,sizeof(void*),0));});return 0;_LLD5: if(_tmp20A != Cyc_Absyn_EmptyAnnot)
goto _LLD7;_LLD6: return 0;_LLD7: if(*((void**)_tmp20A)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD9;_LLD8:({void*_tmp20D[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp20E="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp20E,sizeof(char),_get_zero_arr_size(_tmp20E,46));}),
_tag_dynforward(_tmp20D,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD9:;_LLDA:({void*_tmp20F[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp210="need_null_check";_tag_dynforward(_tmp210,sizeof(char),
_get_zero_arr_size(_tmp210,16));}),_tag_dynforward(_tmp20F,sizeof(void*),0));});
_LLCE:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp211=(void*)e->annot;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp213;
_LLDC: if(*((void**)_tmp211)!= Cyc_CfFlowInfo_UnknownZ)goto _LLDE;_tmp212=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp211)->f1;_LLDD: return _tmp212;_LLDE: if(*((
void**)_tmp211)!= Cyc_CfFlowInfo_NotZero)goto _LLE0;_tmp213=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp211)->f1;_LLDF: return _tmp213;_LLE0: if(_tmp211 != Cyc_CfFlowInfo_IsZero)goto
_LLE2;_LLE1:({void*_tmp214[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp215="dereference of NULL pointer";
_tag_dynforward(_tmp215,sizeof(char),_get_zero_arr_size(_tmp215,28));}),
_tag_dynforward(_tmp214,sizeof(void*),0));});return 0;_LLE2: if(*((void**)_tmp211)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(_tmp211 != Cyc_Absyn_EmptyAnnot)
goto _LLE6;_LLE5: return 0;_LLE6:;_LLE7:({void*_tmp216[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp217="get_relns";_tag_dynforward(_tmp217,sizeof(char),_get_zero_arr_size(
_tmp217,10));}),_tag_dynforward(_tmp216,sizeof(void*),0));});_LLDB:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp218=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp219;struct Cyc_Absyn_PtrAtts _tmp21A;struct Cyc_Absyn_Conref*
_tmp21B;struct Cyc_Absyn_Conref*_tmp21C;struct Cyc_Absyn_ArrayInfo _tmp21D;struct
Cyc_Absyn_Exp*_tmp21E;_LLE9: if(_tmp218 <= (void*)4)goto _LLED;if(*((int*)_tmp218)
!= 4)goto _LLEB;_tmp219=((struct Cyc_Absyn_PointerType_struct*)_tmp218)->f1;
_tmp21A=_tmp219.ptr_atts;_tmp21B=_tmp21A.bounds;_tmp21C=_tmp21A.zero_term;_LLEA: {
void*_tmp21F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp21B);struct Cyc_Absyn_Exp*
_tmp220;_LLF0: if((int)_tmp21F != 0)goto _LLF2;_LLF1: return 0;_LLF2: if((int)_tmp21F
!= 1)goto _LLF4;_LLF3: return 0;_LLF4: if(_tmp21F <= (void*)2)goto _LLF6;if(*((int*)
_tmp21F)!= 0)goto _LLF6;_tmp220=((struct Cyc_Absyn_Upper_b_struct*)_tmp21F)->f1;
_LLF5: {unsigned int _tmp222;int _tmp223;struct _tuple5 _tmp221=Cyc_Evexp_eval_const_uint_exp(
_tmp220);_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;return _tmp223  && i <= _tmp222;}
_LLF6: if(_tmp21F <= (void*)2)goto _LLEF;if(*((int*)_tmp21F)!= 1)goto _LLEF;_LLF7:({
void*_tmp224[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp225="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp225,sizeof(char),_get_zero_arr_size(_tmp225,30));}),
_tag_dynforward(_tmp224,sizeof(void*),0));});_LLEF:;}_LLEB: if(*((int*)_tmp218)!= 
7)goto _LLED;_tmp21D=((struct Cyc_Absyn_ArrayType_struct*)_tmp218)->f1;_tmp21E=
_tmp21D.num_elts;_LLEC: if(_tmp21E == 0)return 0;{unsigned int _tmp227;int _tmp228;
struct _tuple5 _tmp226=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp21E);
_tmp227=_tmp226.f1;_tmp228=_tmp226.f2;return _tmp228  && i <= _tmp227;}_LLED:;_LLEE:
return 0;_LLE8:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp229=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp229->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp22A=_tmp229->rop;
struct Cyc_Absyn_Vardecl*_tmp22B;struct Cyc_Absyn_Vardecl*_tmp22C;_LLF9: if((
_tmp22A.LessSize).tag != 2)goto _LLFB;_tmp22B=(_tmp22A.LessSize).f1;_LLFA: _tmp22C=
_tmp22B;goto _LLFC;_LLFB: if((_tmp22A.LessEqSize).tag != 4)goto _LLFD;_tmp22C=(
_tmp22A.LessEqSize).f1;_LLFC: if(_tmp22C == v)return 1;else{goto _LLF8;}_LLFD:;_LLFE:
continue;_LLF8:;}}return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp22D=(void*)e->r;
void*_tmp22E;struct Cyc_Absyn_Vardecl*_tmp22F;void*_tmp230;struct Cyc_Absyn_Vardecl*
_tmp231;void*_tmp232;struct Cyc_Absyn_Vardecl*_tmp233;void*_tmp234;struct Cyc_Absyn_Vardecl*
_tmp235;void*_tmp236;struct Cyc_List_List*_tmp237;struct Cyc_List_List _tmp238;
struct Cyc_Absyn_Exp*_tmp239;_LL100: if(*((int*)_tmp22D)!= 1)goto _LL102;_tmp22E=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp22D)->f2;if(_tmp22E <= (void*)1)goto
_LL102;if(*((int*)_tmp22E)!= 4)goto _LL102;_tmp22F=((struct Cyc_Absyn_Pat_b_struct*)
_tmp22E)->f1;_LL101: _tmp231=_tmp22F;goto _LL103;_LL102: if(*((int*)_tmp22D)!= 1)
goto _LL104;_tmp230=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp22D)->f2;if(
_tmp230 <= (void*)1)goto _LL104;if(*((int*)_tmp230)!= 3)goto _LL104;_tmp231=((
struct Cyc_Absyn_Local_b_struct*)_tmp230)->f1;_LL103: _tmp233=_tmp231;goto _LL105;
_LL104: if(*((int*)_tmp22D)!= 1)goto _LL106;_tmp232=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp22D)->f2;if(_tmp232 <= (void*)1)goto _LL106;if(*((int*)_tmp232)!= 0)goto _LL106;
_tmp233=((struct Cyc_Absyn_Global_b_struct*)_tmp232)->f1;_LL105: _tmp235=_tmp233;
goto _LL107;_LL106: if(*((int*)_tmp22D)!= 1)goto _LL108;_tmp234=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp22D)->f2;if(_tmp234 <= (void*)1)goto _LL108;if(*((int*)_tmp234)!= 2)goto _LL108;
_tmp235=((struct Cyc_Absyn_Param_b_struct*)_tmp234)->f1;_LL107: if(_tmp235->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp235))return 1;goto _LLFF;_LL108:
if(*((int*)_tmp22D)!= 3)goto _LL10A;_tmp236=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp22D)->f1;if((int)_tmp236 != 19)goto _LL10A;_tmp237=((struct Cyc_Absyn_Primop_e_struct*)
_tmp22D)->f2;if(_tmp237 == 0)goto _LL10A;_tmp238=*_tmp237;_tmp239=(struct Cyc_Absyn_Exp*)
_tmp238.hd;_LL109:{void*_tmp23A=(void*)_tmp239->r;void*_tmp23B;struct Cyc_Absyn_Vardecl*
_tmp23C;void*_tmp23D;struct Cyc_Absyn_Vardecl*_tmp23E;void*_tmp23F;struct Cyc_Absyn_Vardecl*
_tmp240;void*_tmp241;struct Cyc_Absyn_Vardecl*_tmp242;_LL10D: if(*((int*)_tmp23A)
!= 1)goto _LL10F;_tmp23B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp23A)->f2;if(
_tmp23B <= (void*)1)goto _LL10F;if(*((int*)_tmp23B)!= 4)goto _LL10F;_tmp23C=((
struct Cyc_Absyn_Pat_b_struct*)_tmp23B)->f1;_LL10E: _tmp23E=_tmp23C;goto _LL110;
_LL10F: if(*((int*)_tmp23A)!= 1)goto _LL111;_tmp23D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23A)->f2;if(_tmp23D <= (void*)1)goto _LL111;if(*((int*)_tmp23D)!= 3)goto _LL111;
_tmp23E=((struct Cyc_Absyn_Local_b_struct*)_tmp23D)->f1;_LL110: _tmp240=_tmp23E;
goto _LL112;_LL111: if(*((int*)_tmp23A)!= 1)goto _LL113;_tmp23F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23A)->f2;if(_tmp23F <= (void*)1)goto _LL113;if(*((int*)_tmp23F)!= 0)goto _LL113;
_tmp240=((struct Cyc_Absyn_Global_b_struct*)_tmp23F)->f1;_LL112: _tmp242=_tmp240;
goto _LL114;_LL113: if(*((int*)_tmp23A)!= 1)goto _LL115;_tmp241=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23A)->f2;if(_tmp241 <= (void*)1)goto _LL115;if(*((int*)_tmp241)!= 2)goto _LL115;
_tmp242=((struct Cyc_Absyn_Param_b_struct*)_tmp241)->f1;_LL114: return _tmp242 == v;
_LL115:;_LL116: goto _LL10C;_LL10C:;}goto _LLFF;_LL10A:;_LL10B: goto _LLFF;_LLFF:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp243=(void*)a->r;void*_tmp244;struct Cyc_Absyn_Vardecl*
_tmp245;void*_tmp246;struct Cyc_Absyn_Vardecl*_tmp247;void*_tmp248;struct Cyc_Absyn_Vardecl*
_tmp249;void*_tmp24A;struct Cyc_Absyn_Vardecl*_tmp24B;_LL118: if(*((int*)_tmp243)
!= 1)goto _LL11A;_tmp244=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp243)->f2;if(
_tmp244 <= (void*)1)goto _LL11A;if(*((int*)_tmp244)!= 4)goto _LL11A;_tmp245=((
struct Cyc_Absyn_Pat_b_struct*)_tmp244)->f1;_LL119: _tmp247=_tmp245;goto _LL11B;
_LL11A: if(*((int*)_tmp243)!= 1)goto _LL11C;_tmp246=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp243)->f2;if(_tmp246 <= (void*)1)goto _LL11C;if(*((int*)_tmp246)!= 3)goto _LL11C;
_tmp247=((struct Cyc_Absyn_Local_b_struct*)_tmp246)->f1;_LL11B: _tmp249=_tmp247;
goto _LL11D;_LL11C: if(*((int*)_tmp243)!= 1)goto _LL11E;_tmp248=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp243)->f2;if(_tmp248 <= (void*)1)goto _LL11E;if(*((int*)_tmp248)!= 0)goto _LL11E;
_tmp249=((struct Cyc_Absyn_Global_b_struct*)_tmp248)->f1;_LL11D: _tmp24B=_tmp249;
goto _LL11F;_LL11E: if(*((int*)_tmp243)!= 1)goto _LL120;_tmp24A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp243)->f2;if(_tmp24A <= (void*)1)goto _LL120;if(*((int*)_tmp24A)!= 2)goto _LL120;
_tmp24B=((struct Cyc_Absyn_Param_b_struct*)_tmp24A)->f1;_LL11F: if(_tmp24B->escapes)
return 0;inner_loop: {void*_tmp24C=(void*)i->r;void*_tmp24D;struct Cyc_Absyn_Exp*
_tmp24E;union Cyc_Absyn_Cnst_union _tmp24F;void*_tmp250;int _tmp251;union Cyc_Absyn_Cnst_union
_tmp252;void*_tmp253;int _tmp254;union Cyc_Absyn_Cnst_union _tmp255;void*_tmp256;
int _tmp257;void*_tmp258;struct Cyc_List_List*_tmp259;struct Cyc_List_List _tmp25A;
struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List
_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;void*_tmp25F;struct Cyc_Absyn_Vardecl*_tmp260;
void*_tmp261;struct Cyc_Absyn_Vardecl*_tmp262;void*_tmp263;struct Cyc_Absyn_Vardecl*
_tmp264;void*_tmp265;struct Cyc_Absyn_Vardecl*_tmp266;_LL123: if(*((int*)_tmp24C)
!= 15)goto _LL125;_tmp24D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp24C)->f1;
_tmp24E=((struct Cyc_Absyn_Cast_e_struct*)_tmp24C)->f2;_LL124: i=_tmp24E;goto
inner_loop;_LL125: if(*((int*)_tmp24C)!= 0)goto _LL127;_tmp24F=((struct Cyc_Absyn_Const_e_struct*)
_tmp24C)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp24C)->f1).Int_c).tag != 2)
goto _LL127;_tmp250=(_tmp24F.Int_c).f1;if((int)_tmp250 != 2)goto _LL127;_tmp251=(
_tmp24F.Int_c).f2;_LL126: _tmp254=_tmp251;goto _LL128;_LL127: if(*((int*)_tmp24C)!= 
0)goto _LL129;_tmp252=((struct Cyc_Absyn_Const_e_struct*)_tmp24C)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp24C)->f1).Int_c).tag != 2)goto _LL129;_tmp253=(
_tmp252.Int_c).f1;if((int)_tmp253 != 0)goto _LL129;_tmp254=(_tmp252.Int_c).f2;
_LL128: return _tmp254 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp254 + 1),(
void*)_tmp24B->type);_LL129: if(*((int*)_tmp24C)!= 0)goto _LL12B;_tmp255=((struct
Cyc_Absyn_Const_e_struct*)_tmp24C)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp24C)->f1).Int_c).tag != 2)goto _LL12B;_tmp256=(_tmp255.Int_c).f1;if((int)
_tmp256 != 1)goto _LL12B;_tmp257=(_tmp255.Int_c).f2;_LL12A: return Cyc_Toc_check_const_array((
unsigned int)(_tmp257 + 1),(void*)_tmp24B->type);_LL12B: if(*((int*)_tmp24C)!= 3)
goto _LL12D;_tmp258=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp24C)->f1;if((
int)_tmp258 != 4)goto _LL12D;_tmp259=((struct Cyc_Absyn_Primop_e_struct*)_tmp24C)->f2;
if(_tmp259 == 0)goto _LL12D;_tmp25A=*_tmp259;_tmp25B=(struct Cyc_Absyn_Exp*)_tmp25A.hd;
_tmp25C=_tmp25A.tl;if(_tmp25C == 0)goto _LL12D;_tmp25D=*_tmp25C;_tmp25E=(struct Cyc_Absyn_Exp*)
_tmp25D.hd;_LL12C: return Cyc_Toc_check_leq_size(relns,_tmp24B,_tmp25E);_LL12D: if(*((
int*)_tmp24C)!= 1)goto _LL12F;_tmp25F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24C)->f2;if(_tmp25F <= (void*)1)goto _LL12F;if(*((int*)_tmp25F)!= 4)goto _LL12F;
_tmp260=((struct Cyc_Absyn_Pat_b_struct*)_tmp25F)->f1;_LL12E: _tmp262=_tmp260;goto
_LL130;_LL12F: if(*((int*)_tmp24C)!= 1)goto _LL131;_tmp261=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24C)->f2;if(_tmp261 <= (void*)1)goto _LL131;if(*((int*)_tmp261)!= 3)goto _LL131;
_tmp262=((struct Cyc_Absyn_Local_b_struct*)_tmp261)->f1;_LL130: _tmp264=_tmp262;
goto _LL132;_LL131: if(*((int*)_tmp24C)!= 1)goto _LL133;_tmp263=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24C)->f2;if(_tmp263 <= (void*)1)goto _LL133;if(*((int*)_tmp263)!= 0)goto _LL133;
_tmp264=((struct Cyc_Absyn_Global_b_struct*)_tmp263)->f1;_LL132: _tmp266=_tmp264;
goto _LL134;_LL133: if(*((int*)_tmp24C)!= 1)goto _LL135;_tmp265=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24C)->f2;if(_tmp265 <= (void*)1)goto _LL135;if(*((int*)_tmp265)!= 2)goto _LL135;
_tmp266=((struct Cyc_Absyn_Param_b_struct*)_tmp265)->f1;_LL134: if(_tmp266->escapes)
return 0;{struct Cyc_List_List*_tmp267=relns;for(0;_tmp267 != 0;_tmp267=_tmp267->tl){
struct Cyc_CfFlowInfo_Reln*_tmp268=(struct Cyc_CfFlowInfo_Reln*)_tmp267->hd;if(
_tmp268->vd == _tmp266){union Cyc_CfFlowInfo_RelnOp_union _tmp269=_tmp268->rop;
struct Cyc_Absyn_Vardecl*_tmp26A;struct Cyc_Absyn_Vardecl*_tmp26B;unsigned int
_tmp26C;_LL138: if((_tmp269.LessSize).tag != 2)goto _LL13A;_tmp26A=(_tmp269.LessSize).f1;
_LL139: if(_tmp24B == _tmp26A)return 1;else{goto _LL137;}_LL13A: if((_tmp269.LessVar).tag
!= 1)goto _LL13C;_tmp26B=(_tmp269.LessVar).f1;_LL13B:{struct Cyc_List_List*_tmp26D=
relns;for(0;_tmp26D != 0;_tmp26D=_tmp26D->tl){struct Cyc_CfFlowInfo_Reln*_tmp26E=(
struct Cyc_CfFlowInfo_Reln*)_tmp26D->hd;if(_tmp26E->vd == _tmp26B){union Cyc_CfFlowInfo_RelnOp_union
_tmp26F=_tmp26E->rop;struct Cyc_Absyn_Vardecl*_tmp270;struct Cyc_Absyn_Vardecl*
_tmp271;unsigned int _tmp272;struct Cyc_Absyn_Vardecl*_tmp273;_LL141: if((_tmp26F.LessEqSize).tag
!= 4)goto _LL143;_tmp270=(_tmp26F.LessEqSize).f1;_LL142: _tmp271=_tmp270;goto
_LL144;_LL143: if((_tmp26F.LessSize).tag != 2)goto _LL145;_tmp271=(_tmp26F.LessSize).f1;
_LL144: if(_tmp24B == _tmp271)return 1;goto _LL140;_LL145: if((_tmp26F.EqualConst).tag
!= 0)goto _LL147;_tmp272=(_tmp26F.EqualConst).f1;_LL146: return Cyc_Toc_check_const_array(
_tmp272,(void*)_tmp24B->type);_LL147: if((_tmp26F.LessVar).tag != 1)goto _LL149;
_tmp273=(_tmp26F.LessVar).f1;_LL148: if(Cyc_Toc_check_leq_size_var(relns,_tmp24B,
_tmp273))return 1;goto _LL140;_LL149:;_LL14A: goto _LL140;_LL140:;}}}goto _LL137;
_LL13C: if((_tmp269.LessConst).tag != 3)goto _LL13E;_tmp26C=(_tmp269.LessConst).f1;
_LL13D: return Cyc_Toc_check_const_array(_tmp26C,(void*)_tmp24B->type);_LL13E:;
_LL13F: goto _LL137;_LL137:;}}}goto _LL122;_LL135:;_LL136: goto _LL122;_LL122:;}goto
_LL117;_LL120:;_LL121: goto _LL117;_LL117:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp274[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp275="Missing type in primop ";_tag_dynforward(_tmp275,sizeof(char),
_get_zero_arr_size(_tmp275,24));}),_tag_dynforward(_tmp274,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp276[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp277="Missing type in primop ";_tag_dynforward(_tmp277,sizeof(char),
_get_zero_arr_size(_tmp277,24));}),_tag_dynforward(_tmp276,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp278=_cycalloc(
sizeof(*_tmp278));_tmp278->f1=Cyc_Toc_mt_tq;_tmp278->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp278;});}static struct _tuple6*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple6*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->f1=0;
_tmp279->f2=e;_tmp279;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp27A=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp27A):
Cyc_Toc_malloc_ptr(_tmp27A));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp27A);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp27B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp27B != 0;_tmp27B=_tmp27B->tl){struct _tuple6 _tmp27D;struct Cyc_List_List*
_tmp27E;struct Cyc_Absyn_Exp*_tmp27F;struct _tuple6*_tmp27C=(struct _tuple6*)
_tmp27B->hd;_tmp27D=*_tmp27C;_tmp27E=_tmp27D.f1;_tmp27F=_tmp27D.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp27E == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp27E->tl != 0)({void*_tmp280[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp281="multiple designators in array";
_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(_tmp281,30));}),
_tag_dynforward(_tmp280,sizeof(void*),0));});{void*_tmp282=(void*)_tmp27E->hd;
void*_tmp283=_tmp282;struct Cyc_Absyn_Exp*_tmp284;_LL14C: if(*((int*)_tmp283)!= 0)
goto _LL14E;_tmp284=((struct Cyc_Absyn_ArrayElement_struct*)_tmp283)->f1;_LL14D:
Cyc_Toc_exp_to_c(nv,_tmp284);e_index=_tmp284;goto _LL14B;_LL14E: if(*((int*)
_tmp283)!= 1)goto _LL14B;_LL14F:({void*_tmp285[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp286="field name designators in array";
_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,32));}),
_tag_dynforward(_tmp285,sizeof(void*),0));});_LL14B:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp287=(void*)_tmp27F->r;struct Cyc_List_List*
_tmp288;struct Cyc_Absyn_Vardecl*_tmp289;struct Cyc_Absyn_Exp*_tmp28A;struct Cyc_Absyn_Exp*
_tmp28B;int _tmp28C;void*_tmp28D;struct Cyc_List_List*_tmp28E;_LL151: if(*((int*)
_tmp287)!= 28)goto _LL153;_tmp288=((struct Cyc_Absyn_Array_e_struct*)_tmp287)->f1;
_LL152: s=Cyc_Toc_init_array(nv,lval,_tmp288,s);goto _LL150;_LL153: if(*((int*)
_tmp287)!= 29)goto _LL155;_tmp289=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp287)->f1;_tmp28A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp287)->f2;
_tmp28B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp287)->f3;_tmp28C=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp287)->f4;_LL154: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp289,_tmp28A,_tmp28B,_tmp28C,s,0);goto _LL150;_LL155: if(*((int*)
_tmp287)!= 31)goto _LL157;_tmp28D=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp287)->f1;_tmp28E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp287)->f2;_LL156:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp28D,_tmp28E,s);goto _LL150;_LL157:;_LL158:
Cyc_Toc_exp_to_c(nv,_tmp27F);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp27F,0),s,0);goto _LL150;_LL150:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp28F=vd->name;
void*_tmp290=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp291=
_new_region("r2");struct _RegionHandle*r2=& _tmp291;_push_region(r2);{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(r2,nv,_tmp28F,Cyc_Absyn_varb_exp(_tmp28F,(void*)({struct
Cyc_Absyn_Local_b_struct*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B[0]=({struct
Cyc_Absyn_Local_b_struct _tmp29C;_tmp29C.tag=3;_tmp29C.f1=vd;_tmp29C;});_tmp29B;}),
0));struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp28F,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp28F,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp28F,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp28F,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp292=(void*)e2->r;struct Cyc_List_List*
_tmp293;struct Cyc_Absyn_Vardecl*_tmp294;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*
_tmp296;int _tmp297;void*_tmp298;struct Cyc_List_List*_tmp299;_LL15A: if(*((int*)
_tmp292)!= 28)goto _LL15C;_tmp293=((struct Cyc_Absyn_Array_e_struct*)_tmp292)->f1;
_LL15B: body=Cyc_Toc_init_array(nv2,lval,_tmp293,Cyc_Toc_skip_stmt_dl());goto
_LL159;_LL15C: if(*((int*)_tmp292)!= 29)goto _LL15E;_tmp294=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp292)->f1;_tmp295=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp292)->f2;
_tmp296=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp292)->f3;_tmp297=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp292)->f4;_LL15D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp294,_tmp295,_tmp296,_tmp297,Cyc_Toc_skip_stmt_dl(),0);goto _LL159;
_LL15E: if(*((int*)_tmp292)!= 31)goto _LL160;_tmp298=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp292)->f1;_tmp299=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp292)->f2;_LL15F:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp298,_tmp299,Cyc_Toc_skip_stmt_dl());
goto _LL159;_LL160:;_LL161: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL159;_LL159:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp290,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}{struct Cyc_Absyn_Stmt*_tmp29A=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp28F,
Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp29A;}}};
_pop_region(r2);}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp29D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp29D != 0;_tmp29D=_tmp29D->tl){struct _tuple6 _tmp29F;
struct Cyc_List_List*_tmp2A0;struct Cyc_Absyn_Exp*_tmp2A1;struct _tuple6*_tmp29E=(
struct _tuple6*)_tmp29D->hd;_tmp29F=*_tmp29E;_tmp2A0=_tmp29F.f1;_tmp2A1=_tmp29F.f2;
if(_tmp2A0 == 0)({void*_tmp2A2[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2A3="empty designator list";
_tag_dynforward(_tmp2A3,sizeof(char),_get_zero_arr_size(_tmp2A3,22));}),
_tag_dynforward(_tmp2A2,sizeof(void*),0));});if(_tmp2A0->tl != 0)({void*_tmp2A4[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2A5="too many designators in anonymous struct";_tag_dynforward(
_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,41));}),_tag_dynforward(_tmp2A4,
sizeof(void*),0));});{void*_tmp2A6=(void*)_tmp2A0->hd;struct _dynforward_ptr*
_tmp2A7;_LL163: if(*((int*)_tmp2A6)!= 1)goto _LL165;_tmp2A7=((struct Cyc_Absyn_FieldName_struct*)
_tmp2A6)->f1;_LL164: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp2A7,0);{void*_tmp2A8=(void*)_tmp2A1->r;struct Cyc_List_List*_tmp2A9;struct Cyc_Absyn_Vardecl*
_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AC;int _tmp2AD;void*
_tmp2AE;struct Cyc_List_List*_tmp2AF;_LL168: if(*((int*)_tmp2A8)!= 28)goto _LL16A;
_tmp2A9=((struct Cyc_Absyn_Array_e_struct*)_tmp2A8)->f1;_LL169: s=Cyc_Toc_init_array(
nv,lval,_tmp2A9,s);goto _LL167;_LL16A: if(*((int*)_tmp2A8)!= 29)goto _LL16C;_tmp2AA=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A8)->f1;_tmp2AB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2A8)->f2;_tmp2AC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A8)->f3;
_tmp2AD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A8)->f4;_LL16B: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2AA,_tmp2AB,_tmp2AC,_tmp2AD,s,0);goto _LL167;_LL16C: if(*((int*)
_tmp2A8)!= 31)goto _LL16E;_tmp2AE=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2A8)->f1;_tmp2AF=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2A8)->f2;_LL16D:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2AE,_tmp2AF,s);goto _LL167;_LL16E:;_LL16F:
Cyc_Toc_exp_to_c(nv,_tmp2A1);if(Cyc_Toc_is_poly_field(struct_type,_tmp2A7))
_tmp2A1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp2A1);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp2A1,0),0),s,0);goto _LL167;
_LL167:;}goto _LL162;}_LL165:;_LL166:({void*_tmp2B0[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2B1="array designator in struct";_tag_dynforward(_tmp2B1,sizeof(char),
_get_zero_arr_size(_tmp2B1,27));}),_tag_dynforward(_tmp2B0,sizeof(void*),0));});
_LL162:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp2B2=_new_region("r");struct _RegionHandle*r=& _tmp2B2;
_push_region(r);{struct Cyc_List_List*_tmp2B3=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);void*_tmp2B4=Cyc_Toc_add_tuple_type(
_tmp2B3);struct _tuple1*_tmp2B5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2B6=
Cyc_Absyn_var_exp(_tmp2B5,0);struct Cyc_Absyn_Stmt*_tmp2B7=Cyc_Absyn_exp_stmt(
_tmp2B6,0);struct Cyc_Absyn_Exp*(*_tmp2B8)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp2B9=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2B9);for(0;_tmp2B9 != 0;(_tmp2B9=
_tmp2B9->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp2B9->hd;struct
Cyc_Absyn_Exp*lval=_tmp2B8(_tmp2B6,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp2BA=(void*)e->r;struct Cyc_List_List*_tmp2BB;struct Cyc_Absyn_Vardecl*
_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BE;int _tmp2BF;
_LL171: if(*((int*)_tmp2BA)!= 28)goto _LL173;_tmp2BB=((struct Cyc_Absyn_Array_e_struct*)
_tmp2BA)->f1;_LL172: _tmp2B7=Cyc_Toc_init_array(nv,lval,_tmp2BB,_tmp2B7);goto
_LL170;_LL173: if(*((int*)_tmp2BA)!= 29)goto _LL175;_tmp2BC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2BA)->f1;_tmp2BD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2BA)->f2;
_tmp2BE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2BA)->f3;_tmp2BF=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2BA)->f4;_LL174: _tmp2B7=Cyc_Toc_init_comprehension(
nv,lval,_tmp2BC,_tmp2BD,_tmp2BE,_tmp2BF,_tmp2B7,0);goto _LL170;_LL175:;_LL176: Cyc_Toc_exp_to_c(
nv,e);_tmp2B7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp2B8(
_tmp2B6,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp2B7,0);goto _LL170;_LL170:;}}}{
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Toc_make_struct(nv,_tmp2B5,_tmp2B4,_tmp2B7,
pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp2C0;}};_pop_region(r);}
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*
struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
dles,struct _tuple1*tdn){struct _tuple1*_tmp2C1=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp2C2=Cyc_Absyn_var_exp(_tmp2C1,0);struct Cyc_Absyn_Stmt*_tmp2C3=Cyc_Absyn_exp_stmt(
_tmp2C2,0);struct Cyc_Absyn_Exp*(*_tmp2C4)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp2C5=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp2C6;union
Cyc_Absyn_AggrInfoU_union _tmp2C7;_LL178: if(_tmp2C5 <= (void*)4)goto _LL17A;if(*((
int*)_tmp2C5)!= 10)goto _LL17A;_tmp2C6=((struct Cyc_Absyn_AggrType_struct*)_tmp2C5)->f1;
_tmp2C7=_tmp2C6.aggr_info;_LL179: ad=Cyc_Absyn_get_known_aggrdecl(_tmp2C7);goto
_LL177;_LL17A:;_LL17B:({void*_tmp2C8[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2C9="init_struct: bad struct type";
_tag_dynforward(_tmp2C9,sizeof(char),_get_zero_arr_size(_tmp2C9,29));}),
_tag_dynforward(_tmp2C8,sizeof(void*),0));});_LL177:;}{struct _RegionHandle
_tmp2CA=_new_region("r");struct _RegionHandle*r=& _tmp2CA;_push_region(r);{struct
Cyc_List_List*_tmp2CB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,dles);for(0;_tmp2CB != 0;_tmp2CB=_tmp2CB->tl){struct _tuple6
_tmp2CD;struct Cyc_List_List*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CF;struct _tuple6*
_tmp2CC=(struct _tuple6*)_tmp2CB->hd;_tmp2CD=*_tmp2CC;_tmp2CE=_tmp2CD.f1;_tmp2CF=
_tmp2CD.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CF->topt))->v);if(_tmp2CE == 0)({void*_tmp2D0[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2D1="empty designator list";_tag_dynforward(_tmp2D1,sizeof(char),
_get_zero_arr_size(_tmp2D1,22));}),_tag_dynforward(_tmp2D0,sizeof(void*),0));});
if(_tmp2CE->tl != 0){struct _tuple1*_tmp2D2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp2D3=Cyc_Absyn_var_exp(_tmp2D2,0);for(0;_tmp2CE != 0;_tmp2CE=_tmp2CE->tl){void*
_tmp2D4=(void*)_tmp2CE->hd;struct _dynforward_ptr*_tmp2D5;_LL17D: if(*((int*)
_tmp2D4)!= 1)goto _LL17F;_tmp2D5=((struct Cyc_Absyn_FieldName_struct*)_tmp2D4)->f1;
_LL17E: if(Cyc_Toc_is_poly_field(struct_type,_tmp2D5))_tmp2D3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2D3);_tmp2C3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2C4(_tmp2C2,_tmp2D5,0),_tmp2D3,0),0),_tmp2C3,0);goto _LL17C;_LL17F:;_LL180:({
void*_tmp2D6[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp2D7="array designator in struct";
_tag_dynforward(_tmp2D7,sizeof(char),_get_zero_arr_size(_tmp2D7,27));}),
_tag_dynforward(_tmp2D6,sizeof(void*),0));});_LL17C:;}Cyc_Toc_exp_to_c(nv,
_tmp2CF);_tmp2C3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2D3,_tmp2CF,0),0),_tmp2C3,0);}else{void*_tmp2D8=(void*)_tmp2CE->hd;struct
_dynforward_ptr*_tmp2D9;_LL182: if(*((int*)_tmp2D8)!= 1)goto _LL184;_tmp2D9=((
struct Cyc_Absyn_FieldName_struct*)_tmp2D8)->f1;_LL183: {struct Cyc_Absyn_Exp*lval=
_tmp2C4(_tmp2C2,_tmp2D9,0);{void*_tmp2DA=(void*)_tmp2CF->r;struct Cyc_List_List*
_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*
_tmp2DE;int _tmp2DF;void*_tmp2E0;struct Cyc_List_List*_tmp2E1;_LL187: if(*((int*)
_tmp2DA)!= 28)goto _LL189;_tmp2DB=((struct Cyc_Absyn_Array_e_struct*)_tmp2DA)->f1;
_LL188: _tmp2C3=Cyc_Toc_init_array(nv,lval,_tmp2DB,_tmp2C3);goto _LL186;_LL189: if(*((
int*)_tmp2DA)!= 29)goto _LL18B;_tmp2DC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2DA)->f1;_tmp2DD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f2;
_tmp2DE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f3;_tmp2DF=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f4;_LL18A: _tmp2C3=Cyc_Toc_init_comprehension(
nv,lval,_tmp2DC,_tmp2DD,_tmp2DE,_tmp2DF,_tmp2C3,0);goto _LL186;_LL18B: if(*((int*)
_tmp2DA)!= 31)goto _LL18D;_tmp2E0=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2DA)->f1;_tmp2E1=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2DA)->f2;_LL18C:
_tmp2C3=Cyc_Toc_init_anon_struct(nv,lval,_tmp2E0,_tmp2E1,_tmp2C3);goto _LL186;
_LL18D:;_LL18E: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CF->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp2CF);{struct Cyc_Absyn_Aggrfield*
_tmp2E2=Cyc_Absyn_lookup_decl_field(ad,_tmp2D9);if(Cyc_Toc_is_poly_field(
struct_type,_tmp2D9) && !was_ptr_type)_tmp2CF=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2CF);if(has_exists)_tmp2CF=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp2E2))->type,_tmp2CF);_tmp2C3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp2CF,0),0),_tmp2C3,0);goto _LL186;}}_LL186:;}goto
_LL181;}_LL184:;_LL185:({void*_tmp2E3[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E4="array designator in struct";
_tag_dynforward(_tmp2E4,sizeof(char),_get_zero_arr_size(_tmp2E4,27));}),
_tag_dynforward(_tmp2E3,sizeof(void*),0));});_LL181:;}}}{struct Cyc_Absyn_Exp*
_tmp2E5=Cyc_Toc_make_struct(nv,_tmp2C1,Cyc_Absyn_strctq(tdn),_tmp2C3,pointer,
rgnopt,is_atomic);_npop_handler(0);return _tmp2E5;};_pop_region(r);}}struct
_tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple8*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp2E6=_cycalloc(sizeof(*_tmp2E6));
_tmp2E6[0]=({struct Cyc_Absyn_Increment_e_struct _tmp2E7;_tmp2E7.tag=5;_tmp2E7.f1=
e1;_tmp2E7.f2=(void*)incr;_tmp2E7;});_tmp2E6;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp2E8=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Exp*
_tmp2EB;struct Cyc_Absyn_Exp*_tmp2EC;struct _dynforward_ptr*_tmp2ED;_LL190: if(*((
int*)_tmp2E8)!= 38)goto _LL192;_tmp2E9=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp2E8)->f1;_LL191: Cyc_Toc_lvalue_assign_stmt(_tmp2E9,fs,f,f_env);goto _LL18F;
_LL192: if(*((int*)_tmp2E8)!= 15)goto _LL194;_tmp2EA=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2E8)->f1;_tmp2EB=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E8)->f2;_LL193: Cyc_Toc_lvalue_assign(
_tmp2EB,fs,f,f_env);goto _LL18F;_LL194: if(*((int*)_tmp2E8)!= 23)goto _LL196;
_tmp2EC=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2E8)->f1;_tmp2ED=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2E8)->f2;_LL195:(void*)(e1->r=(void*)((void*)_tmp2EC->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));
_tmp2EE->hd=_tmp2ED;_tmp2EE->tl=fs;_tmp2EE;}),f,f_env);goto _LL18F;_LL196:;_LL197: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL18F;}_LL18F:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp2EF=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp2F0;struct Cyc_Absyn_Decl*_tmp2F1;struct Cyc_Absyn_Stmt*
_tmp2F2;struct Cyc_Absyn_Stmt*_tmp2F3;_LL199: if(_tmp2EF <= (void*)1)goto _LL19F;if(*((
int*)_tmp2EF)!= 0)goto _LL19B;_tmp2F0=((struct Cyc_Absyn_Exp_s_struct*)_tmp2EF)->f1;
_LL19A: Cyc_Toc_lvalue_assign(_tmp2F0,fs,f,f_env);goto _LL198;_LL19B: if(*((int*)
_tmp2EF)!= 11)goto _LL19D;_tmp2F1=((struct Cyc_Absyn_Decl_s_struct*)_tmp2EF)->f1;
_tmp2F2=((struct Cyc_Absyn_Decl_s_struct*)_tmp2EF)->f2;_LL19C: Cyc_Toc_lvalue_assign_stmt(
_tmp2F2,fs,f,f_env);goto _LL198;_LL19D: if(*((int*)_tmp2EF)!= 1)goto _LL19F;_tmp2F3=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2EF)->f2;_LL19E: Cyc_Toc_lvalue_assign_stmt(
_tmp2F3,fs,f,f_env);goto _LL198;_LL19F:;_LL1A0:({struct Cyc_String_pa_struct
_tmp2F6;_tmp2F6.tag=0;_tmp2F6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_stmt2string(s));{void*_tmp2F4[1]={& _tmp2F6};Cyc_Toc_toc_impos(({
const char*_tmp2F5="lvalue_assign_stmt: %s";_tag_dynforward(_tmp2F5,sizeof(char),
_get_zero_arr_size(_tmp2F5,23));}),_tag_dynforward(_tmp2F4,sizeof(void*),1));}});
_LL198:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmp2F7=
_region_malloc(r2,sizeof(*_tmp2F7));_tmp2F7->hd=(void*)f((void*)x->hd,env);
_tmp2F7->tl=0;_tmp2F7;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp2F8=_region_malloc(r2,sizeof(*
_tmp2F8));_tmp2F8->hd=(void*)f((void*)x->hd,env);_tmp2F8->tl=0;_tmp2F8;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple6*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->f1=0;
_tmp2F9->f2=e;_tmp2F9;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp2FA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2FB;_LL1A2: if(
_tmp2FA <= (void*)4)goto _LL1A4;if(*((int*)_tmp2FA)!= 4)goto _LL1A4;_tmp2FB=((
struct Cyc_Absyn_PointerType_struct*)_tmp2FA)->f1;_LL1A3: return _tmp2FB;_LL1A4:;
_LL1A5:({void*_tmp2FC[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2FD="get_ptr_typ: not a pointer!";
_tag_dynforward(_tmp2FD,sizeof(char),_get_zero_arr_size(_tmp2FD,28));}),
_tag_dynforward(_tmp2FC,sizeof(void*),0));});_LL1A1:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp2FE=Cyc_Tcutil_compress(
t);void*_tmp2FF;void*_tmp300;void*_tmp301;void*_tmp302;void*_tmp303;void*_tmp304;
void*_tmp305;void*_tmp306;void*_tmp307;void*_tmp308;_LL1A7: if(_tmp2FE <= (void*)4)
goto _LL1B7;if(*((int*)_tmp2FE)!= 4)goto _LL1A9;_LL1A8: res=Cyc_Absyn_null_exp(0);
goto _LL1A6;_LL1A9: if(*((int*)_tmp2FE)!= 5)goto _LL1AB;_tmp2FF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2FE)->f1;_tmp300=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2FE)->f2;if((
int)_tmp300 != 0)goto _LL1AB;_LL1AA: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp309;(_tmp309.Char_c).tag=0;(_tmp309.Char_c).f1=(
void*)_tmp2FF;(_tmp309.Char_c).f2='\000';_tmp309;}),0);goto _LL1A6;_LL1AB: if(*((
int*)_tmp2FE)!= 5)goto _LL1AD;_tmp301=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2FE)->f1;_tmp302=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2FE)->f2;if((
int)_tmp302 != 1)goto _LL1AD;_LL1AC: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp30A;(_tmp30A.Short_c).tag=1;(_tmp30A.Short_c).f1=(
void*)_tmp301;(_tmp30A.Short_c).f2=0;_tmp30A;}),0);goto _LL1A6;_LL1AD: if(*((int*)
_tmp2FE)!= 12)goto _LL1AF;_LL1AE: goto _LL1B0;_LL1AF: if(*((int*)_tmp2FE)!= 13)goto
_LL1B1;_LL1B0: _tmp303=(void*)1;goto _LL1B2;_LL1B1: if(*((int*)_tmp2FE)!= 5)goto
_LL1B3;_tmp303=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2FE)->f1;_tmp304=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2FE)->f2;if((int)_tmp304 != 2)goto
_LL1B3;_LL1B2: _tmp305=_tmp303;goto _LL1B4;_LL1B3: if(*((int*)_tmp2FE)!= 5)goto
_LL1B5;_tmp305=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2FE)->f1;_tmp306=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2FE)->f2;if((int)_tmp306 != 3)goto
_LL1B5;_LL1B4: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp30B;(_tmp30B.Int_c).tag=2;(_tmp30B.Int_c).f1=(void*)_tmp305;(_tmp30B.Int_c).f2=
0;_tmp30B;}),0);goto _LL1A6;_LL1B5: if(*((int*)_tmp2FE)!= 5)goto _LL1B7;_tmp307=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2FE)->f1;_tmp308=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2FE)->f2;if((int)_tmp308 != 4)goto _LL1B7;_LL1B6: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp30C;(_tmp30C.LongLong_c).tag=
3;(_tmp30C.LongLong_c).f1=(void*)_tmp307;(_tmp30C.LongLong_c).f2=(long long)0;
_tmp30C;}),0);goto _LL1A6;_LL1B7: if((int)_tmp2FE != 1)goto _LL1B9;_LL1B8: goto _LL1BA;
_LL1B9: if(_tmp2FE <= (void*)4)goto _LL1BB;if(*((int*)_tmp2FE)!= 6)goto _LL1BB;
_LL1BA: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp30D;(_tmp30D.Float_c).tag=4;(_tmp30D.Float_c).f1=({const char*_tmp30E="0.0";
_tag_dynforward(_tmp30E,sizeof(char),_get_zero_arr_size(_tmp30E,4));});_tmp30D;}),
0);goto _LL1A6;_LL1BB:;_LL1BC:({struct Cyc_String_pa_struct _tmp311;_tmp311.tag=0;
_tmp311.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp30F[1]={& _tmp311};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp310="found non-zero type %s in generate_zero";
_tag_dynforward(_tmp310,sizeof(char),_get_zero_arr_size(_tmp310,40));}),
_tag_dynforward(_tmp30F,sizeof(void*),1));}});_LL1A6:;}res->topt=({struct Cyc_Core_Opt*
_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->v=(void*)t;_tmp312;});return res;}
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dynforward,int is_dyneither,void*elt_type){int is_fat=is_dynforward
 || is_dyneither;void*fat_ptr_type;if(is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp313=Cyc_Toc_typ_to_c(
elt_type);void*_tmp314=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp315=Cyc_Absyn_cstar_typ(
_tmp313,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp316=({struct Cyc_Core_Opt*_tmp354=
_cycalloc(sizeof(*_tmp354));_tmp354->v=(void*)_tmp315;_tmp354;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp317=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Exp*
_tmp319;struct Cyc_Absyn_Exp*_tmp31A;_LL1BE: if(*((int*)_tmp317)!= 22)goto _LL1C0;
_tmp318=((struct Cyc_Absyn_Deref_e_struct*)_tmp317)->f1;_LL1BF: if(!is_fat){
_tmp318=Cyc_Toc_cast_it(fat_ptr_type,_tmp318);_tmp318->topt=({struct Cyc_Core_Opt*
_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B->v=(void*)fat_ptr_type;_tmp31B;});}
Cyc_Toc_exp_to_c(nv,_tmp318);xinit=_tmp318;goto _LL1BD;_LL1C0: if(*((int*)_tmp317)
!= 25)goto _LL1C2;_tmp319=((struct Cyc_Absyn_Subscript_e_struct*)_tmp317)->f1;
_tmp31A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp317)->f2;_LL1C1: if(!is_fat){
_tmp319=Cyc_Toc_cast_it(fat_ptr_type,_tmp319);_tmp319->topt=({struct Cyc_Core_Opt*
_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->v=(void*)fat_ptr_type;_tmp31C;});}
Cyc_Toc_exp_to_c(nv,_tmp319);Cyc_Toc_exp_to_c(nv,_tmp31A);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp31D[3];_tmp31D[2]=_tmp31A;_tmp31D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp31D[0]=_tmp319;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp31D,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp31E[3];_tmp31E[2]=_tmp31A;_tmp31E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp31E[0]=_tmp319;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp31E,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1BD;_LL1C2:;_LL1C3:({void*_tmp31F[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp320="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,53));}),
_tag_dynforward(_tmp31F,sizeof(void*),0));});_LL1BD:;}{struct _tuple1*_tmp321=Cyc_Toc_temp_var();
struct _RegionHandle _tmp322=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp322;
_push_region(rgn2);{struct Cyc_Toc_Env*_tmp323=Cyc_Toc_add_varmap(rgn2,nv,_tmp321,
Cyc_Absyn_var_exp(_tmp321,0));struct Cyc_Absyn_Vardecl*_tmp324=({struct Cyc_Absyn_Vardecl*
_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->sc=(void*)((void*)2);_tmp353->name=
_tmp321;_tmp353->tq=Cyc_Toc_mt_tq;_tmp353->type=(void*)_tmp314;_tmp353->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp353->rgn=0;_tmp353->attributes=0;_tmp353->escapes=
0;_tmp353;});struct Cyc_Absyn_Local_b_struct*_tmp325=({struct Cyc_Absyn_Local_b_struct*
_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351[0]=({struct Cyc_Absyn_Local_b_struct
_tmp352;_tmp352.tag=3;_tmp352.f1=_tmp324;_tmp352;});_tmp351;});struct Cyc_Absyn_Exp*
_tmp326=Cyc_Absyn_varb_exp(_tmp321,(void*)_tmp325,0);_tmp326->topt=({struct Cyc_Core_Opt*
_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->v=(void*)fat_ptr_type;_tmp327;});{
struct Cyc_Absyn_Exp*_tmp328=Cyc_Absyn_deref_exp(_tmp326,0);_tmp328->topt=({
struct Cyc_Core_Opt*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->v=(void*)
elt_type;_tmp329;});Cyc_Toc_exp_to_c(_tmp323,_tmp328);{struct _tuple1*_tmp32A=Cyc_Toc_temp_var();
_tmp323=Cyc_Toc_add_varmap(rgn2,_tmp323,_tmp32A,Cyc_Absyn_var_exp(_tmp32A,0));{
struct Cyc_Absyn_Vardecl*_tmp32B=({struct Cyc_Absyn_Vardecl*_tmp350=_cycalloc(
sizeof(*_tmp350));_tmp350->sc=(void*)((void*)2);_tmp350->name=_tmp32A;_tmp350->tq=
Cyc_Toc_mt_tq;_tmp350->type=(void*)_tmp313;_tmp350->initializer=(struct Cyc_Absyn_Exp*)
_tmp328;_tmp350->rgn=0;_tmp350->attributes=0;_tmp350->escapes=0;_tmp350;});
struct Cyc_Absyn_Local_b_struct*_tmp32C=({struct Cyc_Absyn_Local_b_struct*_tmp34E=
_cycalloc(sizeof(*_tmp34E));_tmp34E[0]=({struct Cyc_Absyn_Local_b_struct _tmp34F;
_tmp34F.tag=3;_tmp34F.f1=_tmp32B;_tmp34F;});_tmp34E;});struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp32D=Cyc_Absyn_varb_exp(_tmp32A,(
void*)_tmp32C,0);_tmp32D->topt=_tmp328->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp32D,e2,0);z_init->topt=_tmp32D->topt;}Cyc_Toc_exp_to_c(_tmp323,
z_init);{struct _tuple1*_tmp32E=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp32F=({struct Cyc_Absyn_Vardecl*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->sc=(
void*)((void*)2);_tmp34D->name=_tmp32E;_tmp34D->tq=Cyc_Toc_mt_tq;_tmp34D->type=(
void*)_tmp313;_tmp34D->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp34D->rgn=0;
_tmp34D->attributes=0;_tmp34D->escapes=0;_tmp34D;});struct Cyc_Absyn_Local_b_struct*
_tmp330=({struct Cyc_Absyn_Local_b_struct*_tmp34B=_cycalloc(sizeof(*_tmp34B));
_tmp34B[0]=({struct Cyc_Absyn_Local_b_struct _tmp34C;_tmp34C.tag=3;_tmp34C.f1=
_tmp32F;_tmp34C;});_tmp34B;});_tmp323=Cyc_Toc_add_varmap(rgn2,_tmp323,_tmp32E,
Cyc_Absyn_var_exp(_tmp32E,0));{struct Cyc_Absyn_Exp*_tmp331=Cyc_Absyn_varb_exp(
_tmp32A,(void*)_tmp32C,0);_tmp331->topt=_tmp328->topt;{struct Cyc_Absyn_Exp*
_tmp332=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp333=Cyc_Absyn_prim2_exp((
void*)5,_tmp331,_tmp332,0);_tmp333->topt=({struct Cyc_Core_Opt*_tmp334=_cycalloc(
sizeof(*_tmp334));_tmp334->v=(void*)Cyc_Absyn_sint_typ;_tmp334;});Cyc_Toc_exp_to_c(
_tmp323,_tmp333);{struct Cyc_Absyn_Exp*_tmp335=Cyc_Absyn_varb_exp(_tmp32E,(void*)
_tmp330,0);_tmp335->topt=_tmp328->topt;{struct Cyc_Absyn_Exp*_tmp336=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp337=Cyc_Absyn_prim2_exp((void*)6,_tmp335,
_tmp336,0);_tmp337->topt=({struct Cyc_Core_Opt*_tmp338=_cycalloc(sizeof(*_tmp338));
_tmp338->v=(void*)Cyc_Absyn_sint_typ;_tmp338;});Cyc_Toc_exp_to_c(_tmp323,_tmp337);{
struct Cyc_List_List*_tmp339=({struct Cyc_Absyn_Exp*_tmp34A[2];_tmp34A[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp34A[0]=Cyc_Absyn_varb_exp(_tmp321,(void*)
_tmp325,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp34A,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp33A=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;if(is_dynforward)xsize=
Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,
_tmp339,0),_tmp33A,0);else{xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dyneither_size_e,_tmp339,0),_tmp33A,0);}{struct Cyc_Absyn_Exp*
_tmp33B=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp333,_tmp337,0),0);struct
Cyc_Absyn_Stmt*_tmp33C=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,
0,0),0);struct Cyc_Absyn_Exp*_tmp33D=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(
_tmp321,(void*)_tmp325,0),Cyc_Toc_curr_sp,0);_tmp33D=Cyc_Toc_cast_it(_tmp315,
_tmp33D);{struct Cyc_Absyn_Exp*_tmp33E=Cyc_Absyn_deref_exp(_tmp33D,0);struct Cyc_Absyn_Exp*
_tmp33F=Cyc_Absyn_assign_exp(_tmp33E,Cyc_Absyn_var_exp(_tmp32E,0),0);struct Cyc_Absyn_Stmt*
_tmp340=Cyc_Absyn_exp_stmt(_tmp33F,0);_tmp340=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp33B,_tmp33C,Cyc_Absyn_skip_stmt(0),0),_tmp340,0);_tmp340=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342[
0]=({struct Cyc_Absyn_Var_d_struct _tmp343;_tmp343.tag=0;_tmp343.f1=_tmp32F;
_tmp343;});_tmp342;}));_tmp341->loc=0;_tmp341;}),_tmp340,0);_tmp340=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[
0]=({struct Cyc_Absyn_Var_d_struct _tmp346;_tmp346.tag=0;_tmp346.f1=_tmp32B;
_tmp346;});_tmp345;}));_tmp344->loc=0;_tmp344;}),_tmp340,0);_tmp340=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348[
0]=({struct Cyc_Absyn_Var_d_struct _tmp349;_tmp349.tag=0;_tmp349.f1=_tmp324;
_tmp349;});_tmp348;}));_tmp347->loc=0;_tmp347;}),_tmp340,0);(void*)(e->r=(void*)
Cyc_Toc_stmt_exp_r(_tmp340));}}}}}}}}}}}};_pop_region(rgn2);}}}struct _tuple9{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp355=(void*)e->r;if(e->topt == 0)({struct Cyc_String_pa_struct
_tmp358;_tmp358.tag=0;_tmp358.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp356[1]={& _tmp358};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp357="exp_to_c: no type for %s";_tag_dynforward(_tmp357,sizeof(char),
_get_zero_arr_size(_tmp357,25));}),_tag_dynforward(_tmp356,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp359=
_tmp355;union Cyc_Absyn_Cnst_union _tmp35A;struct _tuple1*_tmp35B;void*_tmp35C;
struct _tuple1*_tmp35D;void*_tmp35E;struct Cyc_List_List*_tmp35F;struct Cyc_Absyn_Exp*
_tmp360;void*_tmp361;struct Cyc_Absyn_Exp*_tmp362;struct Cyc_Core_Opt*_tmp363;
struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Exp*_tmp365;struct Cyc_Absyn_Exp*
_tmp366;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Exp*
_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;struct Cyc_Absyn_Exp*
_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_List_List*
_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_List_List*_tmp371;struct Cyc_Absyn_VarargCallInfo*
_tmp372;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_List_List*_tmp374;struct Cyc_Absyn_VarargCallInfo*
_tmp375;struct Cyc_Absyn_VarargCallInfo _tmp376;int _tmp377;struct Cyc_List_List*
_tmp378;struct Cyc_Absyn_VarargInfo*_tmp379;struct Cyc_Absyn_Exp*_tmp37A;struct Cyc_Absyn_Exp*
_tmp37B;struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_List_List*_tmp37D;void*_tmp37E;
void**_tmp37F;struct Cyc_Absyn_Exp*_tmp380;int _tmp381;void*_tmp382;struct Cyc_Absyn_Exp*
_tmp383;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_Absyn_Exp*_tmp385;struct Cyc_Absyn_Exp*
_tmp386;void*_tmp387;void*_tmp388;void*_tmp389;struct _dynforward_ptr*_tmp38A;
void*_tmp38B;void*_tmp38C;unsigned int _tmp38D;struct Cyc_Absyn_Exp*_tmp38E;struct
Cyc_Absyn_Exp*_tmp38F;struct _dynforward_ptr*_tmp390;struct Cyc_Absyn_Exp*_tmp391;
struct _dynforward_ptr*_tmp392;struct Cyc_Absyn_Exp*_tmp393;struct Cyc_Absyn_Exp*
_tmp394;struct Cyc_List_List*_tmp395;struct Cyc_List_List*_tmp396;struct Cyc_Absyn_Vardecl*
_tmp397;struct Cyc_Absyn_Exp*_tmp398;struct Cyc_Absyn_Exp*_tmp399;int _tmp39A;
struct _tuple1*_tmp39B;struct Cyc_List_List*_tmp39C;struct Cyc_List_List*_tmp39D;
struct Cyc_Absyn_Aggrdecl*_tmp39E;void*_tmp39F;struct Cyc_List_List*_tmp3A0;struct
Cyc_List_List*_tmp3A1;struct Cyc_Absyn_Tuniondecl*_tmp3A2;struct Cyc_Absyn_Tunionfield*
_tmp3A3;struct Cyc_List_List*_tmp3A4;struct Cyc_Absyn_Tuniondecl*_tmp3A5;struct Cyc_Absyn_Tunionfield*
_tmp3A6;struct Cyc_Absyn_MallocInfo _tmp3A7;int _tmp3A8;struct Cyc_Absyn_Exp*_tmp3A9;
void**_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AB;int _tmp3AC;struct Cyc_Absyn_Exp*_tmp3AD;
struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_Absyn_Stmt*_tmp3AF;_LL1C5: if(*((int*)
_tmp359)!= 0)goto _LL1C7;_tmp35A=((struct Cyc_Absyn_Const_e_struct*)_tmp359)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp359)->f1).Null_c).tag != 6)goto _LL1C7;
_LL1C6: {struct Cyc_Absyn_Exp*_tmp3B0=Cyc_Absyn_uint_exp(0,0);int forward_only=0;
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ,& forward_only)){if(Cyc_Toc_is_toplevel(
nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp3B0,
_tmp3B0,forward_only))->r));else{if(forward_only)(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp3B1[3];_tmp3B1[2]=_tmp3B0;
_tmp3B1[1]=_tmp3B0;_tmp3B1[0]=_tmp3B0;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3B1,sizeof(struct Cyc_Absyn_Exp*),
3));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp3B2[3];_tmp3B2[2]=_tmp3B0;_tmp3B2[1]=_tmp3B0;_tmp3B2[0]=
_tmp3B0;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3B2,sizeof(struct Cyc_Absyn_Exp*),3));})));}}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1C4;}_LL1C7: if(*((int*)_tmp359)!= 0)
goto _LL1C9;_LL1C8: goto _LL1C4;_LL1C9: if(*((int*)_tmp359)!= 1)goto _LL1CB;_tmp35B=((
struct Cyc_Absyn_Var_e_struct*)_tmp359)->f1;_tmp35C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp359)->f2;_LL1CA:{struct _handler_cons _tmp3B3;_push_handler(& _tmp3B3);{int
_tmp3B5=0;if(setjmp(_tmp3B3.handler))_tmp3B5=1;if(!_tmp3B5){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp35B))->r));;_pop_handler();}else{void*
_tmp3B4=(void*)_exn_thrown;void*_tmp3B7=_tmp3B4;_LL21C: if(_tmp3B7 != Cyc_Dict_Absent)
goto _LL21E;_LL21D:({struct Cyc_String_pa_struct _tmp3BA;_tmp3BA.tag=0;_tmp3BA.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp35B));{
void*_tmp3B8[1]={& _tmp3BA};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3B9="Can't find %s in exp_to_c, Var\n";
_tag_dynforward(_tmp3B9,sizeof(char),_get_zero_arr_size(_tmp3B9,32));}),
_tag_dynforward(_tmp3B8,sizeof(void*),1));}});_LL21E:;_LL21F:(void)_throw(
_tmp3B7);_LL21B:;}}}goto _LL1C4;_LL1CB: if(*((int*)_tmp359)!= 2)goto _LL1CD;_tmp35D=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp359)->f1;_LL1CC:({void*_tmp3BB[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3BC="unknownid";_tag_dynforward(_tmp3BC,sizeof(char),
_get_zero_arr_size(_tmp3BC,10));}),_tag_dynforward(_tmp3BB,sizeof(void*),0));});
_LL1CD: if(*((int*)_tmp359)!= 3)goto _LL1CF;_tmp35E=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp359)->f1;_tmp35F=((struct Cyc_Absyn_Primop_e_struct*)_tmp359)->f2;_LL1CE: {
struct Cyc_List_List*_tmp3BD=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp35F);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp35F);{void*_tmp3BE=_tmp35E;_LL221:
if((int)_tmp3BE != 19)goto _LL223;_LL222: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp35F))->hd;{void*_tmp3BF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_PtrInfo _tmp3C2;void*_tmp3C3;
struct Cyc_Absyn_PtrAtts _tmp3C4;struct Cyc_Absyn_Conref*_tmp3C5;struct Cyc_Absyn_Conref*
_tmp3C6;struct Cyc_Absyn_Conref*_tmp3C7;_LL236: if(_tmp3BF <= (void*)4)goto _LL23A;
if(*((int*)_tmp3BF)!= 7)goto _LL238;_tmp3C0=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3BF)->f1;_tmp3C1=_tmp3C0.num_elts;_LL237:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp3C1))->r));goto _LL235;_LL238: if(*((int*)
_tmp3BF)!= 4)goto _LL23A;_tmp3C2=((struct Cyc_Absyn_PointerType_struct*)_tmp3BF)->f1;
_tmp3C3=(void*)_tmp3C2.elt_typ;_tmp3C4=_tmp3C2.ptr_atts;_tmp3C5=_tmp3C4.nullable;
_tmp3C6=_tmp3C4.bounds;_tmp3C7=_tmp3C4.zero_term;_LL239:{void*_tmp3C8=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3C6);struct Cyc_Absyn_Exp*_tmp3C9;_LL23D: if((int)_tmp3C8
!= 0)goto _LL23F;_LL23E:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dynforward_size_e,({
struct Cyc_Absyn_Exp*_tmp3CA[2];_tmp3CA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3C3),0);_tmp3CA[0]=(struct Cyc_Absyn_Exp*)_tmp35F->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3CA,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL23C;_LL23F: if((int)_tmp3C8 != 1)goto _LL241;_LL240:(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*
_tmp3CB[2];_tmp3CB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3C3),0);
_tmp3CB[0]=(struct Cyc_Absyn_Exp*)_tmp35F->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3CB,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL23C;_LL241: if(_tmp3C8 <= (void*)2)goto _LL243;if(*((int*)_tmp3C8)
!= 0)goto _LL243;_tmp3C9=((struct Cyc_Absyn_Upper_b_struct*)_tmp3C8)->f1;_LL242:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3C7))(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp3CC[2];_tmp3CC[1]=_tmp3C9;_tmp3CC[0]=(struct Cyc_Absyn_Exp*)_tmp35F->hd;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3CC,sizeof(struct Cyc_Absyn_Exp*),2));})));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp3C5))(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp3C9,Cyc_Absyn_uint_exp(0,0)));else{(void*)(e->r=(void*)((void*)_tmp3C9->r));
goto _LL23C;}}goto _LL23C;_LL243: if(_tmp3C8 <= (void*)2)goto _LL23C;if(*((int*)
_tmp3C8)!= 1)goto _LL23C;_LL244:({void*_tmp3CD[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3CE="toc: size of AbsUpper_b";
_tag_dynforward(_tmp3CE,sizeof(char),_get_zero_arr_size(_tmp3CE,24));}),
_tag_dynforward(_tmp3CD,sizeof(void*),0));});_LL23C:;}goto _LL235;_LL23A:;_LL23B:({
struct Cyc_String_pa_struct _tmp3D2;_tmp3D2.tag=0;_tmp3D2.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct _tmp3D1;
_tmp3D1.tag=0;_tmp3D1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp3CF[2]={&
_tmp3D1,& _tmp3D2};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3D0="size primop applied to non-array %s (%s)";_tag_dynforward(
_tmp3D0,sizeof(char),_get_zero_arr_size(_tmp3D0,41));}),_tag_dynforward(_tmp3CF,
sizeof(void*),2));}}});_LL235:;}goto _LL220;}_LL223: if((int)_tmp3BE != 0)goto
_LL225;_LL224:{void*_tmp3D3=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)
_check_null(_tmp3BD))->hd);struct Cyc_Absyn_PtrInfo _tmp3D4;void*_tmp3D5;struct Cyc_Absyn_PtrAtts
_tmp3D6;struct Cyc_Absyn_Conref*_tmp3D7;struct Cyc_Absyn_Conref*_tmp3D8;_LL246: if(
_tmp3D3 <= (void*)4)goto _LL248;if(*((int*)_tmp3D3)!= 4)goto _LL248;_tmp3D4=((
struct Cyc_Absyn_PointerType_struct*)_tmp3D3)->f1;_tmp3D5=(void*)_tmp3D4.elt_typ;
_tmp3D6=_tmp3D4.ptr_atts;_tmp3D7=_tmp3D6.bounds;_tmp3D8=_tmp3D6.zero_term;_LL247:{
void*_tmp3D9=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3D7);struct Cyc_Absyn_Exp*
_tmp3DA;_LL24B: if((int)_tmp3D9 != 0)goto _LL24D;_LL24C: {struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35F))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35F->tl))->hd;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_ptr_plus_e,({struct
Cyc_Absyn_Exp*_tmp3DB[3];_tmp3DB[2]=e2;_tmp3DB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3D5),0);_tmp3DB[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3DB,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL24A;}_LL24D:
if((int)_tmp3D9 != 1)goto _LL24F;_LL24E: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp35F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp35F->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3DC[3];_tmp3DC[2]=e2;
_tmp3DC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3D5),0);_tmp3DC[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3DC,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL24A;}_LL24F: if(_tmp3D9 <= (
void*)2)goto _LL251;if(*((int*)_tmp3D9)!= 0)goto _LL251;_tmp3DA=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3D9)->f1;_LL250: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3D8)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp35F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp35F->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp3DD[3];_tmp3DD[2]=e2;_tmp3DD[
1]=_tmp3DA;_tmp3DD[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3DD,sizeof(struct Cyc_Absyn_Exp*),3));})));}goto _LL24A;_LL251:
if(_tmp3D9 <= (void*)2)goto _LL24A;if(*((int*)_tmp3D9)!= 1)goto _LL24A;_LL252:({
void*_tmp3DE[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp3DF="toc: plus on AbsUpper_b";_tag_dynforward(
_tmp3DF,sizeof(char),_get_zero_arr_size(_tmp3DF,24));}),_tag_dynforward(_tmp3DE,
sizeof(void*),0));});_LL24A:;}goto _LL245;_LL248:;_LL249: goto _LL245;_LL245:;}goto
_LL220;_LL225: if((int)_tmp3BE != 2)goto _LL227;_LL226: {void*elt_typ=(void*)0;int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp3BD))->hd,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35F))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35F->tl))->hd;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp3BD->tl))->hd,& forward_only)){(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(
Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{if(forward_only)({void*_tmp3E0[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3E1="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp3E1,sizeof(char),_get_zero_arr_size(_tmp3E1,50));}),
_tag_dynforward(_tmp3E0,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3E2[3];_tmp3E2[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp3E2[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp3E2[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3E2,sizeof(struct Cyc_Absyn_Exp*),3));})));}}goto _LL220;}
_LL227: if((int)_tmp3BE != 5)goto _LL229;_LL228: goto _LL22A;_LL229: if((int)_tmp3BE != 
6)goto _LL22B;_LL22A: goto _LL22C;_LL22B: if((int)_tmp3BE != 7)goto _LL22D;_LL22C: goto
_LL22E;_LL22D: if((int)_tmp3BE != 9)goto _LL22F;_LL22E: goto _LL230;_LL22F: if((int)
_tmp3BE != 8)goto _LL231;_LL230: goto _LL232;_LL231: if((int)_tmp3BE != 10)goto _LL233;
_LL232: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp35F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp35F->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3BD))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp3BD->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL220;}_LL233:;_LL234: goto _LL220;_LL220:;}
goto _LL1C4;}_LL1CF: if(*((int*)_tmp359)!= 5)goto _LL1D1;_tmp360=((struct Cyc_Absyn_Increment_e_struct*)
_tmp359)->f1;_tmp361=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp359)->f2;
_LL1D0: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp360->topt))->v;
void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=
0;struct _dynforward_ptr incr_str=({const char*_tmp3EE="increment";_tag_dynforward(
_tmp3EE,sizeof(char),_get_zero_arr_size(_tmp3EE,10));});if(_tmp361 == (void*)2
 || _tmp361 == (void*)3)incr_str=({const char*_tmp3E3="decrement";_tag_dynforward(
_tmp3E3,sizeof(char),_get_zero_arr_size(_tmp3E3,10));});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp360,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct
_tmp3E6;_tmp3E6.tag=0;_tmp3E6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
incr_str);{void*_tmp3E4[1]={& _tmp3E6};Cyc_Tcutil_terr(e->loc,({const char*_tmp3E5="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp3E5,sizeof(char),_get_zero_arr_size(_tmp3E5,74));}),
_tag_dynforward(_tmp3E4,sizeof(void*),1));}});({void*_tmp3E7[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3E8="in-place inc on zero-term";_tag_dynforward(_tmp3E8,sizeof(char),
_get_zero_arr_size(_tmp3E8,26));}),_tag_dynforward(_tmp3E7,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp360);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp361 == (void*)3  || _tmp361 == (void*)2)({void*_tmp3E9[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp3EA="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp3EA,sizeof(char),_get_zero_arr_size(_tmp3EA,43));}),
_tag_dynforward(_tmp3E9,sizeof(void*),0));});fn_e=_tmp361 == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp361 == (void*)1  || _tmp361
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp361 == (void*)2  || _tmp361 == (void*)3)change=- 1;}(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp3EB[3];_tmp3EB[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp3EB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp3EB[0]=
Cyc_Absyn_address_exp(_tmp360,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp3EB,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp3EC=_tmp361 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e: Cyc_Toc__zero_arr_inplace_plus_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp3EC,({struct Cyc_Absyn_Exp*_tmp3ED[2];
_tmp3ED[1]=Cyc_Absyn_signed_int_exp(1,0);_tmp3ED[0]=_tmp360;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3ED,sizeof(struct Cyc_Absyn_Exp*),
2));})));}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp360)){Cyc_Toc_lvalue_assign(
_tmp360,0,Cyc_Toc_incr_lvalue,_tmp361);(void*)(e->r=(void*)((void*)_tmp360->r));}}}
goto _LL1C4;}}_LL1D1: if(*((int*)_tmp359)!= 4)goto _LL1D3;_tmp362=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp359)->f1;_tmp363=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp359)->f2;_tmp364=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp359)->f3;_LL1D2: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp362,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp362,_tmp363,_tmp364,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp362);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp362->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp364->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp362);
Cyc_Toc_exp_to_c(nv,_tmp364);{int done=0;if(_tmp363 != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp3EF=(void*)_tmp363->v;_LL254:
if((int)_tmp3EF != 0)goto _LL256;_LL255: change=_tmp364;goto _LL253;_LL256: if((int)
_tmp3EF != 2)goto _LL258;_LL257: if(forward_only)({void*_tmp3F0[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3F1="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp3F1,sizeof(char),_get_zero_arr_size(_tmp3F1,46));}),
_tag_dynforward(_tmp3F0,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp364,0);goto _LL253;_LL258:;_LL259:({void*_tmp3F2[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3F3="bad t ? pointer arithmetic";_tag_dynforward(_tmp3F3,sizeof(char),
_get_zero_arr_size(_tmp3F3,27));}),_tag_dynforward(_tmp3F2,sizeof(void*),0));});
_LL253:;}done=1;{struct Cyc_Absyn_Exp*_tmp3F4=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
_tmp3F4,({struct Cyc_Absyn_Exp*_tmp3F5[3];_tmp3F5[2]=change;_tmp3F5[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp3F5[0]=Cyc_Absyn_address_exp(_tmp362,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3F5,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp3F6=(void*)_tmp363->v;_LL25B: if((int)_tmp3F6 != 0)
goto _LL25D;_LL25C: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp3F7[2];_tmp3F7[1]=_tmp364;_tmp3F7[0]=_tmp362;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3F7,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL25A;_LL25D:;_LL25E:({void*_tmp3F8[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3F9="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp3F9,sizeof(
char),_get_zero_arr_size(_tmp3F9,39));}),_tag_dynforward(_tmp3F8,sizeof(void*),0));});
_LL25A:;}}}if(!done){if(e1_poly)(void*)(_tmp364->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp364->r,0)));if(!Cyc_Absyn_is_lvalue(_tmp362)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple8*),struct _tuple8*f_env))Cyc_Toc_lvalue_assign)(
_tmp362,0,Cyc_Toc_assignop_lvalue,({struct _tuple8*_tmp3FA=_cycalloc(sizeof(
struct _tuple8)* 1);_tmp3FA[0]=({struct _tuple8 _tmp3FB;_tmp3FB.f1=_tmp363;_tmp3FB.f2=
_tmp364;_tmp3FB;});_tmp3FA;}));(void*)(e->r=(void*)((void*)_tmp362->r));}}goto
_LL1C4;}}}_LL1D3: if(*((int*)_tmp359)!= 6)goto _LL1D5;_tmp365=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp359)->f1;_tmp366=((struct Cyc_Absyn_Conditional_e_struct*)_tmp359)->f2;
_tmp367=((struct Cyc_Absyn_Conditional_e_struct*)_tmp359)->f3;_LL1D4: Cyc_Toc_exp_to_c(
nv,_tmp365);Cyc_Toc_exp_to_c(nv,_tmp366);Cyc_Toc_exp_to_c(nv,_tmp367);goto _LL1C4;
_LL1D5: if(*((int*)_tmp359)!= 7)goto _LL1D7;_tmp368=((struct Cyc_Absyn_And_e_struct*)
_tmp359)->f1;_tmp369=((struct Cyc_Absyn_And_e_struct*)_tmp359)->f2;_LL1D6: Cyc_Toc_exp_to_c(
nv,_tmp368);Cyc_Toc_exp_to_c(nv,_tmp369);goto _LL1C4;_LL1D7: if(*((int*)_tmp359)!= 
8)goto _LL1D9;_tmp36A=((struct Cyc_Absyn_Or_e_struct*)_tmp359)->f1;_tmp36B=((
struct Cyc_Absyn_Or_e_struct*)_tmp359)->f2;_LL1D8: Cyc_Toc_exp_to_c(nv,_tmp36A);
Cyc_Toc_exp_to_c(nv,_tmp36B);goto _LL1C4;_LL1D9: if(*((int*)_tmp359)!= 9)goto
_LL1DB;_tmp36C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp359)->f1;_tmp36D=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp359)->f2;_LL1DA: Cyc_Toc_exp_to_c(nv,_tmp36C);Cyc_Toc_exp_to_c(
nv,_tmp36D);goto _LL1C4;_LL1DB: if(*((int*)_tmp359)!= 10)goto _LL1DD;_tmp36E=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp359)->f1;_tmp36F=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp359)->f2;_LL1DC: _tmp370=_tmp36E;_tmp371=_tmp36F;goto _LL1DE;_LL1DD: if(*((int*)
_tmp359)!= 11)goto _LL1DF;_tmp370=((struct Cyc_Absyn_FnCall_e_struct*)_tmp359)->f1;
_tmp371=((struct Cyc_Absyn_FnCall_e_struct*)_tmp359)->f2;_tmp372=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp359)->f3;if(_tmp372 != 0)goto _LL1DF;_LL1DE: Cyc_Toc_exp_to_c(nv,_tmp370);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp371);goto _LL1C4;
_LL1DF: if(*((int*)_tmp359)!= 11)goto _LL1E1;_tmp373=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp359)->f1;_tmp374=((struct Cyc_Absyn_FnCall_e_struct*)_tmp359)->f2;_tmp375=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp359)->f3;if(_tmp375 == 0)goto _LL1E1;_tmp376=*
_tmp375;_tmp377=_tmp376.num_varargs;_tmp378=_tmp376.injectors;_tmp379=_tmp376.vai;
_LL1E0:{struct _RegionHandle _tmp3FC=_new_region("r");struct _RegionHandle*r=&
_tmp3FC;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp377,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp379->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp374);int num_normargs=num_args - _tmp377;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp374=_tmp374->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp374))->hd);
new_args=({struct Cyc_List_List*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->hd=(
struct Cyc_Absyn_Exp*)_tmp374->hd;_tmp3FD->tl=new_args;_tmp3FD;});}}new_args=({
struct Cyc_List_List*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp3FF[3];_tmp3FF[2]=
num_varargs_exp;_tmp3FF[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp3FF[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3FF,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp3FE->tl=
new_args;_tmp3FE;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp373);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp373,new_args,0),0);if(_tmp379->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp400=Cyc_Tcutil_compress((void*)_tmp379->type);
struct Cyc_Absyn_TunionInfo _tmp401;union Cyc_Absyn_TunionInfoU_union _tmp402;struct
Cyc_Absyn_Tuniondecl**_tmp403;struct Cyc_Absyn_Tuniondecl*_tmp404;_LL260: if(
_tmp400 <= (void*)4)goto _LL262;if(*((int*)_tmp400)!= 2)goto _LL262;_tmp401=((
struct Cyc_Absyn_TunionType_struct*)_tmp400)->f1;_tmp402=_tmp401.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp400)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL262;_tmp403=(_tmp402.KnownTunion).f1;_tmp404=*_tmp403;_LL261: tud=
_tmp404;goto _LL25F;_LL262:;_LL263:({void*_tmp405[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp406="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp406,
sizeof(char),_get_zero_arr_size(_tmp406,42));}),_tag_dynforward(_tmp405,sizeof(
void*),0));});_LL25F:;}{struct _dynforward_ptr vs=({unsigned int _tmp40E=(
unsigned int)_tmp377;struct _tuple1**_tmp40F=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp40E));struct _dynforward_ptr _tmp411=
_tag_dynforward(_tmp40F,sizeof(struct _tuple1*),_tmp40E);{unsigned int _tmp410=
_tmp40E;unsigned int i;for(i=0;i < _tmp410;i ++){_tmp40F[i]=Cyc_Toc_temp_var();}}
_tmp411;});struct Cyc_List_List*_tmp407=0;{int i=_tmp377 - 1;for(0;i >= 0;-- i){
_tmp407=({struct Cyc_List_List*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp408->tl=
_tmp407;_tmp408;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp407,0),s,0);{int i=0;for(0;_tmp374 != 0;(((
_tmp374=_tmp374->tl,_tmp378=_tmp378->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp374->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp40A;struct _tuple1*
_tmp40B;struct Cyc_List_List*_tmp40C;struct Cyc_Absyn_Tunionfield*_tmp409=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp378))->hd;_tmp40A=*
_tmp409;_tmp40B=_tmp40A.name;_tmp40C=_tmp40A.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp40C))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp40B,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp40B,({const char*_tmp40D="_struct";
_tag_dynforward(_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp374 != 0;(_tmp374=_tmp374->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp374->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp374->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}};_pop_region(r);}goto _LL1C4;_LL1E1: if(*((int*)_tmp359)!= 12)goto _LL1E3;
_tmp37A=((struct Cyc_Absyn_Throw_e_struct*)_tmp359)->f1;_LL1E2: Cyc_Toc_exp_to_c(
nv,_tmp37A);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp37A),0))->r));goto _LL1C4;_LL1E3: if(*((int*)
_tmp359)!= 13)goto _LL1E5;_tmp37B=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp359)->f1;_LL1E4: Cyc_Toc_exp_to_c(nv,_tmp37B);goto _LL1C4;_LL1E5: if(*((int*)
_tmp359)!= 14)goto _LL1E7;_tmp37C=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp359)->f1;
_tmp37D=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp359)->f2;_LL1E6: Cyc_Toc_exp_to_c(
nv,_tmp37C);for(0;_tmp37D != 0;_tmp37D=_tmp37D->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp37D->hd);if(((k != (void*)6  && k != (void*)3) && k != (void*)4) && k != (
void*)5){{void*_tmp412=Cyc_Tcutil_compress((void*)_tmp37D->hd);_LL265: if(_tmp412
<= (void*)4)goto _LL269;if(*((int*)_tmp412)!= 1)goto _LL267;_LL266: goto _LL268;
_LL267: if(*((int*)_tmp412)!= 2)goto _LL269;_LL268: continue;_LL269:;_LL26A:(void*)(
e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp37C,0))->r));goto _LL264;_LL264:;}
break;}}goto _LL1C4;_LL1E7: if(*((int*)_tmp359)!= 15)goto _LL1E9;_tmp37E=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp359)->f1;_tmp37F=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp359)->f1);_tmp380=((struct Cyc_Absyn_Cast_e_struct*)_tmp359)->f2;_tmp381=((
struct Cyc_Absyn_Cast_e_struct*)_tmp359)->f3;_tmp382=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp359)->f4;_LL1E8: {void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp380->topt))->v;void*new_typ=*_tmp37F;*_tmp37F=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(
nv,_tmp380);{struct _tuple0 _tmp414=({struct _tuple0 _tmp413;_tmp413.f1=Cyc_Tcutil_compress(
old_t2);_tmp413.f2=Cyc_Tcutil_compress(new_typ);_tmp413;});void*_tmp415;struct
Cyc_Absyn_PtrInfo _tmp416;void*_tmp417;struct Cyc_Absyn_PtrInfo _tmp418;void*
_tmp419;struct Cyc_Absyn_PtrInfo _tmp41A;void*_tmp41B;_LL26C: _tmp415=_tmp414.f1;
if(_tmp415 <= (void*)4)goto _LL26E;if(*((int*)_tmp415)!= 4)goto _LL26E;_tmp416=((
struct Cyc_Absyn_PointerType_struct*)_tmp415)->f1;_tmp417=_tmp414.f2;if(_tmp417 <= (
void*)4)goto _LL26E;if(*((int*)_tmp417)!= 4)goto _LL26E;_tmp418=((struct Cyc_Absyn_PointerType_struct*)
_tmp417)->f1;_LL26D: {int _tmp41C=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp416.ptr_atts).nullable);int _tmp41D=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp418.ptr_atts).nullable);void*_tmp41E=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp416.ptr_atts).bounds);void*_tmp41F=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp418.ptr_atts).bounds);int _tmp420=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp416.ptr_atts).zero_term);int
_tmp421=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp418.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp423=({struct _tuple0
_tmp422;_tmp422.f1=_tmp41E;_tmp422.f2=_tmp41F;_tmp422;});void*_tmp424;struct Cyc_Absyn_Exp*
_tmp425;void*_tmp426;struct Cyc_Absyn_Exp*_tmp427;void*_tmp428;struct Cyc_Absyn_Exp*
_tmp429;void*_tmp42A;void*_tmp42B;struct Cyc_Absyn_Exp*_tmp42C;void*_tmp42D;void*
_tmp42E;void*_tmp42F;struct Cyc_Absyn_Exp*_tmp430;void*_tmp431;void*_tmp432;
struct Cyc_Absyn_Exp*_tmp433;void*_tmp434;void*_tmp435;void*_tmp436;void*_tmp437;
void*_tmp438;void*_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;void*_tmp43D;
void*_tmp43E;void*_tmp43F;void*_tmp440;void*_tmp441;_LL273: _tmp424=_tmp423.f1;
if(_tmp424 <= (void*)2)goto _LL275;if(*((int*)_tmp424)!= 0)goto _LL275;_tmp425=((
struct Cyc_Absyn_Upper_b_struct*)_tmp424)->f1;_tmp426=_tmp423.f2;if(_tmp426 <= (
void*)2)goto _LL275;if(*((int*)_tmp426)!= 0)goto _LL275;_tmp427=((struct Cyc_Absyn_Upper_b_struct*)
_tmp426)->f1;_LL274: if(_tmp41C  && !_tmp41D){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp442[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp443="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp443,sizeof(char),_get_zero_arr_size(_tmp443,44));}),_tag_dynforward(_tmp442,
sizeof(void*),0));});if(_tmp382 != (void*)2)({struct Cyc_String_pa_struct _tmp446;
_tmp446.tag=0;_tmp446.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp444[1]={& _tmp446};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp445="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp445,sizeof(char),_get_zero_arr_size(_tmp445,41));}),
_tag_dynforward(_tmp444,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp380);if(do_null_check){if(!_tmp381)({void*_tmp447[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp448="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp448,sizeof(char),_get_zero_arr_size(_tmp448,58));}),
_tag_dynforward(_tmp447,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp37F,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp449=
_cycalloc(sizeof(*_tmp449));_tmp449->hd=_tmp380;_tmp449->tl=0;_tmp449;}),0)));}}}
goto _LL272;_LL275: _tmp428=_tmp423.f1;if(_tmp428 <= (void*)2)goto _LL277;if(*((int*)
_tmp428)!= 0)goto _LL277;_tmp429=((struct Cyc_Absyn_Upper_b_struct*)_tmp428)->f1;
_tmp42A=_tmp423.f2;if((int)_tmp42A != 0)goto _LL277;_LL276: p2_forward_only=1;
_tmp42C=_tmp429;goto _LL278;_LL277: _tmp42B=_tmp423.f1;if(_tmp42B <= (void*)2)goto
_LL279;if(*((int*)_tmp42B)!= 0)goto _LL279;_tmp42C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp42B)->f1;_tmp42D=_tmp423.f2;if((int)_tmp42D != 1)goto _LL279;_LL278: if(_tmp382
== (void*)2)({struct Cyc_String_pa_struct _tmp44C;_tmp44C.tag=0;_tmp44C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp44A[
1]={& _tmp44C};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp44B="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp44B,sizeof(char),_get_zero_arr_size(_tmp44B,44));}),_tag_dynforward(_tmp44A,
sizeof(void*),1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp420  && !(_tmp418.elt_tq).real_const)
 && !_tmp421)_tmp42C=Cyc_Absyn_prim2_exp((void*)2,_tmp42C,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp42C,
_tmp380,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp44D=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp420){struct _tuple1*
_tmp44E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_var_exp(_tmp44E,
0);struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp455[2];_tmp455[1]=_tmp42C;_tmp455[0]=_tmp44F;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp455,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp421  && !(_tmp418.elt_tq).real_const)_tmp450=Cyc_Absyn_prim2_exp((
void*)2,_tmp450,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp418.elt_typ),0);struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_fncall_exp(
_tmp44D,({struct Cyc_Absyn_Exp*_tmp454[3];_tmp454[2]=_tmp450;_tmp454[1]=_tmp451;
_tmp454[0]=_tmp44F;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp454,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp453=Cyc_Absyn_exp_stmt(_tmp452,0);_tmp453=Cyc_Absyn_declare_stmt(_tmp44E,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp380,_tmp453,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp453));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp44D,({struct Cyc_Absyn_Exp*
_tmp456[3];_tmp456[2]=_tmp42C;_tmp456[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp418.elt_typ),0);_tmp456[0]=_tmp380;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp456,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL272;_LL279: _tmp42E=_tmp423.f1;if((int)_tmp42E != 0)goto _LL27B;
_tmp42F=_tmp423.f2;if(_tmp42F <= (void*)2)goto _LL27B;if(*((int*)_tmp42F)!= 0)goto
_LL27B;_tmp430=((struct Cyc_Absyn_Upper_b_struct*)_tmp42F)->f1;_LL27A:
p1_forward_only=1;_tmp433=_tmp430;goto _LL27C;_LL27B: _tmp431=_tmp423.f1;if((int)
_tmp431 != 1)goto _LL27D;_tmp432=_tmp423.f2;if(_tmp432 <= (void*)2)goto _LL27D;if(*((
int*)_tmp432)!= 0)goto _LL27D;_tmp433=((struct Cyc_Absyn_Upper_b_struct*)_tmp432)->f1;
_LL27C: if(Cyc_Toc_is_toplevel(nv))({void*_tmp457[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp458="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp458,sizeof(char),_get_zero_arr_size(_tmp458,45));}),
_tag_dynforward(_tmp457,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp459=
_tmp433;if(_tmp420  && !_tmp421)_tmp459=Cyc_Absyn_add_exp(_tmp433,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp45A=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp45B=Cyc_Absyn_fncall_exp(
_tmp45A,({struct Cyc_Absyn_Exp*_tmp45D[3];_tmp45D[2]=_tmp459;_tmp45D[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp416.elt_typ),0);_tmp45D[0]=_tmp380;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp45D,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp41D)(void*)(_tmp45B->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->hd=Cyc_Absyn_copy_exp(
_tmp45B);_tmp45C->tl=0;_tmp45C;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp37F,_tmp45B));goto _LL272;}}_LL27D: _tmp434=_tmp423.f1;if((int)_tmp434 != 0)
goto _LL27F;_tmp435=_tmp423.f2;if((int)_tmp435 != 0)goto _LL27F;_LL27E:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL27F: _tmp436=_tmp423.f1;if((
int)_tmp436 != 0)goto _LL281;_tmp437=_tmp423.f2;if((int)_tmp437 != 1)goto _LL281;
_LL280: p1_forward_only=1;goto DynCast;_LL281: _tmp438=_tmp423.f1;if((int)_tmp438 != 
1)goto _LL283;_tmp439=_tmp423.f2;if((int)_tmp439 != 0)goto _LL283;_LL282:
p2_forward_only=1;goto DynCast;_LL283: _tmp43A=_tmp423.f1;if((int)_tmp43A != 1)goto
_LL285;_tmp43B=_tmp423.f2;if((int)_tmp43B != 1)goto _LL285;_LL284: DynCast: if((
_tmp420  && !_tmp421) && !(_tmp418.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({
void*_tmp45E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp45F="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp45F,sizeof(char),_get_zero_arr_size(_tmp45F,70));}),
_tag_dynforward(_tmp45E,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp460=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp460,({struct Cyc_Absyn_Exp*_tmp461[3];
_tmp461[2]=Cyc_Absyn_uint_exp(1,0);_tmp461[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp416.elt_typ),0);_tmp461[0]=_tmp380;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp461,sizeof(struct Cyc_Absyn_Exp*),
3));})));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp462[1];_tmp462[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp462,sizeof(struct Cyc_Absyn_Exp*),
1));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp463[1];_tmp463[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp463,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}}else{if(p1_forward_only != p2_forward_only){if(Cyc_Toc_is_toplevel(
nv))({void*_tmp464[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp465="can't coerce between ? and ?+- at toplevel";
_tag_dynforward(_tmp465,sizeof(char),_get_zero_arr_size(_tmp465,43));}),
_tag_dynforward(_tmp464,sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp466[1];_tmp466[0]=_tmp380;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp466,sizeof(struct Cyc_Absyn_Exp*),1));})));
else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp467[1];_tmp467[0]=_tmp380;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp467,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}goto _LL272;_LL285: _tmp43C=_tmp423.f1;if(_tmp43C <= (void*)2)goto
_LL287;if(*((int*)_tmp43C)!= 1)goto _LL287;_tmp43D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp43C)->f1;_tmp43E=_tmp423.f2;if(_tmp43E <= (void*)2)goto _LL287;if(*((int*)
_tmp43E)!= 1)goto _LL287;_tmp43F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp43E)->f1;_LL286: if(_tmp41C  && !_tmp41D){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp468[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp469="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp469,sizeof(char),_get_zero_arr_size(_tmp469,44));}),_tag_dynforward(_tmp468,
sizeof(void*),0));});if(_tmp382 != (void*)2)({struct Cyc_String_pa_struct _tmp46C;
_tmp46C.tag=0;_tmp46C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp46A[1]={& _tmp46C};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp46B="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp46B,sizeof(char),_get_zero_arr_size(_tmp46B,41));}),
_tag_dynforward(_tmp46A,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp380);if(do_null_check){if(!_tmp381)({void*_tmp46D[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp46E="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp46E,sizeof(char),_get_zero_arr_size(_tmp46E,58));}),
_tag_dynforward(_tmp46D,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp37F,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp46F=
_cycalloc(sizeof(*_tmp46F));_tmp46F->hd=_tmp380;_tmp46F->tl=0;_tmp46F;}),0)));}}}
goto _LL272;_LL287: _tmp440=_tmp423.f1;if(_tmp440 <= (void*)2)goto _LL289;if(*((int*)
_tmp440)!= 1)goto _LL289;_LL288:({struct Cyc_String_pa_struct _tmp472;_tmp472.tag=0;
_tmp472.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp380->loc));{void*_tmp470[1]={& _tmp472};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp471="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp471,sizeof(char),_get_zero_arr_size(_tmp471,30));}),
_tag_dynforward(_tmp470,sizeof(void*),1));}});_LL289: _tmp441=_tmp423.f2;if(
_tmp441 <= (void*)2)goto _LL272;if(*((int*)_tmp441)!= 1)goto _LL272;_LL28A: goto
_LL272;_LL272:;}goto _LL26B;}_LL26E: _tmp419=_tmp414.f1;if(_tmp419 <= (void*)4)goto
_LL270;if(*((int*)_tmp419)!= 4)goto _LL270;_tmp41A=((struct Cyc_Absyn_PointerType_struct*)
_tmp419)->f1;_tmp41B=_tmp414.f2;if(_tmp41B <= (void*)4)goto _LL270;if(*((int*)
_tmp41B)!= 5)goto _LL270;_LL26F:{void*_tmp473=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp41A.ptr_atts).bounds);_LL28C: if((int)_tmp473 != 0)goto _LL28E;_LL28D: goto
_LL28F;_LL28E: if((int)_tmp473 != 1)goto _LL290;_LL28F:(void*)(_tmp380->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp380->r,_tmp380->loc),Cyc_Toc_curr_sp));
goto _LL28B;_LL290:;_LL291: goto _LL28B;_LL28B:;}goto _LL26B;_LL270:;_LL271: goto
_LL26B;_LL26B:;}goto _LL1C4;}_LL1E9: if(*((int*)_tmp359)!= 16)goto _LL1EB;_tmp383=((
struct Cyc_Absyn_Address_e_struct*)_tmp359)->f1;_LL1EA:{void*_tmp474=(void*)
_tmp383->r;struct _tuple1*_tmp475;struct Cyc_List_List*_tmp476;struct Cyc_List_List*
_tmp477;struct Cyc_List_List*_tmp478;_LL293: if(*((int*)_tmp474)!= 30)goto _LL295;
_tmp475=((struct Cyc_Absyn_Struct_e_struct*)_tmp474)->f1;_tmp476=((struct Cyc_Absyn_Struct_e_struct*)
_tmp474)->f2;_tmp477=((struct Cyc_Absyn_Struct_e_struct*)_tmp474)->f3;_LL294: if(
Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp47B;_tmp47B.tag=0;
_tmp47B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp383->loc));{void*_tmp479[1]={& _tmp47B};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp47A="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp47A,sizeof(char),_get_zero_arr_size(_tmp47A,42));}),
_tag_dynforward(_tmp479,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp383->topt))->v,_tmp476 != 0,1,0,
_tmp477,_tmp475))->r));goto _LL292;_LL295: if(*((int*)_tmp474)!= 26)goto _LL297;
_tmp478=((struct Cyc_Absyn_Tuple_e_struct*)_tmp474)->f1;_LL296: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp47E;_tmp47E.tag=0;_tmp47E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp383->loc));{
void*_tmp47C[1]={& _tmp47E};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp47D="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp47D,sizeof(char),_get_zero_arr_size(_tmp47D,42));}),
_tag_dynforward(_tmp47C,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp478))->r));goto _LL292;_LL297:;_LL298: Cyc_Toc_exp_to_c(nv,_tmp383);if(
!Cyc_Absyn_is_lvalue(_tmp383)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp383,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp383));}goto _LL292;
_LL292:;}goto _LL1C4;_LL1EB: if(*((int*)_tmp359)!= 17)goto _LL1ED;_tmp384=((struct
Cyc_Absyn_New_e_struct*)_tmp359)->f1;_tmp385=((struct Cyc_Absyn_New_e_struct*)
_tmp359)->f2;_LL1EC: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp481;_tmp481.tag=0;_tmp481.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Position_string_of_segment(_tmp385->loc));{void*_tmp47F[1]={& _tmp481};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp480="%s: new at top-level";_tag_dynforward(_tmp480,sizeof(char),
_get_zero_arr_size(_tmp480,21));}),_tag_dynforward(_tmp47F,sizeof(void*),1));}});{
void*_tmp482=(void*)_tmp385->r;struct Cyc_List_List*_tmp483;struct Cyc_Absyn_Vardecl*
_tmp484;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Exp*_tmp486;int _tmp487;
struct _tuple1*_tmp488;struct Cyc_List_List*_tmp489;struct Cyc_List_List*_tmp48A;
struct Cyc_List_List*_tmp48B;_LL29A: if(*((int*)_tmp482)!= 28)goto _LL29C;_tmp483=((
struct Cyc_Absyn_Array_e_struct*)_tmp482)->f1;_LL29B: {struct _tuple1*_tmp48C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp48D=Cyc_Absyn_var_exp(_tmp48C,0);struct Cyc_Absyn_Stmt*
_tmp48E=Cyc_Toc_init_array(nv,_tmp48D,_tmp483,Cyc_Absyn_exp_stmt(_tmp48D,0));
void*old_elt_typ;{void*_tmp48F=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp490;void*_tmp491;struct Cyc_Absyn_Tqual _tmp492;struct Cyc_Absyn_PtrAtts _tmp493;
struct Cyc_Absyn_Conref*_tmp494;_LL2A5: if(_tmp48F <= (void*)4)goto _LL2A7;if(*((int*)
_tmp48F)!= 4)goto _LL2A7;_tmp490=((struct Cyc_Absyn_PointerType_struct*)_tmp48F)->f1;
_tmp491=(void*)_tmp490.elt_typ;_tmp492=_tmp490.elt_tq;_tmp493=_tmp490.ptr_atts;
_tmp494=_tmp493.zero_term;_LL2A6: old_elt_typ=_tmp491;goto _LL2A4;_LL2A7:;_LL2A8:
old_elt_typ=({void*_tmp495[0]={};Cyc_Toc_toc_impos(({const char*_tmp496="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp496,sizeof(char),_get_zero_arr_size(_tmp496,52));}),
_tag_dynforward(_tmp495,sizeof(void*),0));});_LL2A4:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp497=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp498=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp483),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp384 == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp498);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp384;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp498);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp48C,
_tmp497,(struct Cyc_Absyn_Exp*)e1,_tmp48E,0)));goto _LL299;}}_LL29C: if(*((int*)
_tmp482)!= 29)goto _LL29E;_tmp484=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp482)->f1;_tmp485=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp482)->f2;
_tmp486=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp482)->f3;_tmp487=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp482)->f4;_LL29D: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp499=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp49A;void*_tmp49B;struct Cyc_Absyn_Tqual _tmp49C;struct Cyc_Absyn_PtrAtts _tmp49D;
struct Cyc_Absyn_Conref*_tmp49E;struct Cyc_Absyn_Conref*_tmp49F;_LL2AA: if(_tmp499
<= (void*)4)goto _LL2AC;if(*((int*)_tmp499)!= 4)goto _LL2AC;_tmp49A=((struct Cyc_Absyn_PointerType_struct*)
_tmp499)->f1;_tmp49B=(void*)_tmp49A.elt_typ;_tmp49C=_tmp49A.elt_tq;_tmp49D=
_tmp49A.ptr_atts;_tmp49E=_tmp49D.bounds;_tmp49F=_tmp49D.zero_term;_LL2AB:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp49E)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp49E)== (void*)1;
goto _LL2A9;_LL2AC:;_LL2AD:({void*_tmp4A0[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4A1="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp4A1,sizeof(char),_get_zero_arr_size(_tmp4A1,42));}),
_tag_dynforward(_tmp4A0,sizeof(void*),0));});_LL2A9:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp486->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp485);{
struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Absyn_var_exp(max,0);if(_tmp487)_tmp4A2=Cyc_Absyn_add_exp(
_tmp4A2,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp484,Cyc_Absyn_var_exp(max,0),_tmp486,_tmp487,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp4A3=_new_region("r");struct _RegionHandle*r=& _tmp4A3;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp4B4=
_region_malloc(r,sizeof(*_tmp4B4));_tmp4B4->hd=({struct _tuple9*_tmp4B5=
_region_malloc(r,sizeof(*_tmp4B5));_tmp4B5->f1=max;_tmp4B5->f2=Cyc_Absyn_uint_typ;
_tmp4B5->f3=(struct Cyc_Absyn_Exp*)_tmp485;_tmp4B5;});_tmp4B4->tl=0;_tmp4B4;});
struct Cyc_Absyn_Exp*ai;if(_tmp384 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp4A4[2];_tmp4A4[1]=_tmp4A2;
_tmp4A4[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4A4,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp384;Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp4A5[2];_tmp4A5[1]=_tmp4A2;_tmp4A5[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4A5,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({struct Cyc_List_List*_tmp4A6=
_region_malloc(r,sizeof(*_tmp4A6));_tmp4A6->hd=({struct _tuple9*_tmp4A7=
_region_malloc(r,sizeof(*_tmp4A7));_tmp4A7->f1=a;_tmp4A7->f2=ptr_typ;_tmp4A7->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp4A7;});_tmp4A6->tl=decls;_tmp4A6;});if(
is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp4A8=Cyc_Toc_temp_var();
void*_tmp4A9=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp4AA=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp4AB=Cyc_Absyn_fncall_exp(_tmp4AA,({struct Cyc_Absyn_Exp*_tmp4AE[3];_tmp4AE[2]=
_tmp4A2;_tmp4AE[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp4AE[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4AE,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp4AC=_region_malloc(r,sizeof(*_tmp4AC));_tmp4AC->hd=({struct _tuple9*_tmp4AD=
_region_malloc(r,sizeof(*_tmp4AD));_tmp4AD->f1=_tmp4A8;_tmp4AD->f2=_tmp4A9;
_tmp4AD->f3=(struct Cyc_Absyn_Exp*)_tmp4AB;_tmp4AD;});_tmp4AC->tl=decls;_tmp4AC;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4A8,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp4AF=decls;for(0;_tmp4AF != 0;_tmp4AF=_tmp4AF->tl){struct
_tuple1*_tmp4B1;void*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct _tuple9 _tmp4B0=*((
struct _tuple9*)_tmp4AF->hd);_tmp4B1=_tmp4B0.f1;_tmp4B2=_tmp4B0.f2;_tmp4B3=
_tmp4B0.f3;s=Cyc_Absyn_declare_stmt(_tmp4B1,_tmp4B2,_tmp4B3,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL299;}}}}_LL29E: if(*((int*)
_tmp482)!= 30)goto _LL2A0;_tmp488=((struct Cyc_Absyn_Struct_e_struct*)_tmp482)->f1;
_tmp489=((struct Cyc_Absyn_Struct_e_struct*)_tmp482)->f2;_tmp48A=((struct Cyc_Absyn_Struct_e_struct*)
_tmp482)->f3;_LL29F:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp385->topt))->v,_tmp489 != 0,1,_tmp384,_tmp48A,
_tmp488))->r));goto _LL299;_LL2A0: if(*((int*)_tmp482)!= 26)goto _LL2A2;_tmp48B=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp482)->f1;_LL2A1:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp384,_tmp48B))->r));goto _LL299;_LL2A2:;_LL2A3: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp385->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp4B6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4B7=Cyc_Absyn_var_exp(_tmp4B6,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp4B7,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp384 == 0)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp384;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*_tmp4B8=(void*)
_tmp385->r;void*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;_LL2AF: if(*((int*)_tmp4B8)!= 
15)goto _LL2B1;_tmp4B9=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4B8)->f1;
_tmp4BA=((struct Cyc_Absyn_Cast_e_struct*)_tmp4B8)->f2;_LL2B0:{struct _tuple0
_tmp4BC=({struct _tuple0 _tmp4BB;_tmp4BB.f1=Cyc_Tcutil_compress(_tmp4B9);_tmp4BB.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4BA->topt))->v);
_tmp4BB;});void*_tmp4BD;struct Cyc_Absyn_PtrInfo _tmp4BE;void*_tmp4BF;struct Cyc_Absyn_PtrAtts
_tmp4C0;struct Cyc_Absyn_Conref*_tmp4C1;void*_tmp4C2;struct Cyc_Absyn_PtrInfo
_tmp4C3;struct Cyc_Absyn_PtrAtts _tmp4C4;struct Cyc_Absyn_Conref*_tmp4C5;_LL2B4:
_tmp4BD=_tmp4BC.f1;if(_tmp4BD <= (void*)4)goto _LL2B6;if(*((int*)_tmp4BD)!= 4)goto
_LL2B6;_tmp4BE=((struct Cyc_Absyn_PointerType_struct*)_tmp4BD)->f1;_tmp4BF=(void*)
_tmp4BE.elt_typ;_tmp4C0=_tmp4BE.ptr_atts;_tmp4C1=_tmp4C0.bounds;_tmp4C2=_tmp4BC.f2;
if(_tmp4C2 <= (void*)4)goto _LL2B6;if(*((int*)_tmp4C2)!= 4)goto _LL2B6;_tmp4C3=((
struct Cyc_Absyn_PointerType_struct*)_tmp4C2)->f1;_tmp4C4=_tmp4C3.ptr_atts;
_tmp4C5=_tmp4C4.bounds;_LL2B5:{struct _tuple0 _tmp4C7=({struct _tuple0 _tmp4C6;
_tmp4C6.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4C1);_tmp4C6.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp4C5);_tmp4C6;});void*_tmp4C8;void*_tmp4C9;struct Cyc_Absyn_Exp*
_tmp4CA;void*_tmp4CB;void*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CD;_LL2B9: _tmp4C8=
_tmp4C7.f1;if((int)_tmp4C8 != 0)goto _LL2BB;_tmp4C9=_tmp4C7.f2;if(_tmp4C9 <= (void*)
2)goto _LL2BB;if(*((int*)_tmp4C9)!= 0)goto _LL2BB;_tmp4CA=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4C9)->f1;_LL2BA: forward_only=1;_tmp4CD=_tmp4CA;goto _LL2BC;_LL2BB: _tmp4CB=
_tmp4C7.f1;if((int)_tmp4CB != 1)goto _LL2BD;_tmp4CC=_tmp4C7.f2;if(_tmp4CC <= (void*)
2)goto _LL2BD;if(*((int*)_tmp4CC)!= 0)goto _LL2BD;_tmp4CD=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4CC)->f1;_LL2BC: Cyc_Toc_exp_to_c(nv,_tmp4BA);(void*)(inner_mexp->r=(void*)
Cyc_Toc_sizeoftyp_exp_r(elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp4CE=
forward_only?Cyc_Toc__init_dynforward_ptr_e: Cyc_Toc__init_dyneither_ptr_e;(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4CE,({struct Cyc_Absyn_Exp*_tmp4CF[4];
_tmp4CF[3]=_tmp4CD;_tmp4CF[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4BF),
0);_tmp4CF[1]=_tmp4BA;_tmp4CF[0]=mexp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4CF,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2B8;}_LL2BD:;_LL2BE: goto _LL2B8;_LL2B8:;}goto _LL2B3;_LL2B6:;
_LL2B7: goto _LL2B3;_LL2B3:;}goto _LL2AE;_LL2B1:;_LL2B2: goto _LL2AE;_LL2AE:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp4D0=Cyc_Absyn_exp_stmt(_tmp4B7,0);struct Cyc_Absyn_Exp*
_tmp4D1=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp385);_tmp4D0=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp4B7,_tmp4D1,0),_tmp385,0),
_tmp4D0,0);{void*_tmp4D2=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4B6,_tmp4D2,(struct Cyc_Absyn_Exp*)
mexp,_tmp4D0,0)));}}goto _LL299;}}_LL299:;}goto _LL1C4;_LL1ED: if(*((int*)_tmp359)
!= 19)goto _LL1EF;_tmp386=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp359)->f1;
_LL1EE: Cyc_Toc_exp_to_c(nv,_tmp386);goto _LL1C4;_LL1EF: if(*((int*)_tmp359)!= 18)
goto _LL1F1;_tmp387=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp359)->f1;
_LL1F0:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4D3=
_cycalloc(sizeof(*_tmp4D3));_tmp4D3[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp4D4;_tmp4D4.tag=18;_tmp4D4.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp387);_tmp4D4;});
_tmp4D3;})));goto _LL1C4;_LL1F1: if(*((int*)_tmp359)!= 21)goto _LL1F3;_LL1F2:({void*
_tmp4D5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4D6="__gen() in code generator";_tag_dynforward(_tmp4D6,sizeof(
char),_get_zero_arr_size(_tmp4D6,26));}),_tag_dynforward(_tmp4D5,sizeof(void*),0));});
_LL1F3: if(*((int*)_tmp359)!= 20)goto _LL1F5;_tmp388=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp359)->f1;_tmp389=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp359)->f2;
if(*((int*)_tmp389)!= 0)goto _LL1F5;_tmp38A=((struct Cyc_Absyn_StructField_struct*)
_tmp389)->f1;_LL1F4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp4D8;_tmp4D8.tag=20;_tmp4D8.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp388);_tmp4D8.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4D9=_cycalloc(sizeof(*
_tmp4D9));_tmp4D9[0]=({struct Cyc_Absyn_StructField_struct _tmp4DA;_tmp4DA.tag=0;
_tmp4DA.f1=_tmp38A;_tmp4DA;});_tmp4D9;}));_tmp4D8;});_tmp4D7;})));goto _LL1C4;
_LL1F5: if(*((int*)_tmp359)!= 20)goto _LL1F7;_tmp38B=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp359)->f1;_tmp38C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp359)->f2;
if(*((int*)_tmp38C)!= 1)goto _LL1F7;_tmp38D=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp38C)->f1;_LL1F6:{void*_tmp4DB=Cyc_Tcutil_compress(_tmp38B);struct Cyc_Absyn_AggrInfo
_tmp4DC;union Cyc_Absyn_AggrInfoU_union _tmp4DD;struct Cyc_List_List*_tmp4DE;_LL2C0:
if(_tmp4DB <= (void*)4)goto _LL2C8;if(*((int*)_tmp4DB)!= 10)goto _LL2C2;_tmp4DC=((
struct Cyc_Absyn_AggrType_struct*)_tmp4DB)->f1;_tmp4DD=_tmp4DC.aggr_info;_LL2C1: {
struct Cyc_Absyn_Aggrdecl*_tmp4DF=Cyc_Absyn_get_known_aggrdecl(_tmp4DD);if(
_tmp4DF->impl == 0)({void*_tmp4E0[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4E1="struct fields must be known";
_tag_dynforward(_tmp4E1,sizeof(char),_get_zero_arr_size(_tmp4E1,28));}),
_tag_dynforward(_tmp4E0,sizeof(void*),0));});_tmp4DE=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4DF->impl))->fields;goto _LL2C3;}_LL2C2: if(*((int*)_tmp4DB)!= 11)
goto _LL2C4;_tmp4DE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4DB)->f2;_LL2C3: {
struct Cyc_Absyn_Aggrfield*_tmp4E2=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp4DE,(int)_tmp38D);(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Offsetof_e_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp4E4;_tmp4E4.tag=20;_tmp4E4.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp38B);_tmp4E4.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4E5=
_cycalloc(sizeof(*_tmp4E5));_tmp4E5[0]=({struct Cyc_Absyn_StructField_struct
_tmp4E6;_tmp4E6.tag=0;_tmp4E6.f1=_tmp4E2->name;_tmp4E6;});_tmp4E5;}));_tmp4E4;});
_tmp4E3;})));goto _LL2BF;}_LL2C4: if(*((int*)_tmp4DB)!= 9)goto _LL2C6;_LL2C5:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4E7=_cycalloc(
sizeof(*_tmp4E7));_tmp4E7[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4E8;_tmp4E8.tag=
20;_tmp4E8.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp38B);_tmp4E8.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9[0]=({
struct Cyc_Absyn_StructField_struct _tmp4EA;_tmp4EA.tag=0;_tmp4EA.f1=Cyc_Absyn_fieldname((
int)(_tmp38D + 1));_tmp4EA;});_tmp4E9;}));_tmp4E8;});_tmp4E7;})));goto _LL2BF;
_LL2C6: if(*((int*)_tmp4DB)!= 3)goto _LL2C8;_LL2C7: if(_tmp38D == 0)(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4EB=_cycalloc(sizeof(*
_tmp4EB));_tmp4EB[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4EC;_tmp4EC.tag=20;
_tmp4EC.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp38B);_tmp4EC.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[0]=({
struct Cyc_Absyn_StructField_struct _tmp4EE;_tmp4EE.tag=0;_tmp4EE.f1=Cyc_Toc_tag_sp;
_tmp4EE;});_tmp4ED;}));_tmp4EC;});_tmp4EB;})));else{(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp4F0;_tmp4F0.tag=20;_tmp4F0.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp38B);_tmp4F0.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4F1=
_cycalloc(sizeof(*_tmp4F1));_tmp4F1[0]=({struct Cyc_Absyn_StructField_struct
_tmp4F2;_tmp4F2.tag=0;_tmp4F2.f1=Cyc_Absyn_fieldname((int)_tmp38D);_tmp4F2;});
_tmp4F1;}));_tmp4F0;});_tmp4EF;})));}goto _LL2BF;_LL2C8:;_LL2C9:({void*_tmp4F3[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4F4="impossible type for offsetof tuple index";_tag_dynforward(
_tmp4F4,sizeof(char),_get_zero_arr_size(_tmp4F4,41));}),_tag_dynforward(_tmp4F3,
sizeof(void*),0));});_LL2BF:;}goto _LL1C4;_LL1F7: if(*((int*)_tmp359)!= 22)goto
_LL1F9;_tmp38E=((struct Cyc_Absyn_Deref_e_struct*)_tmp359)->f1;_LL1F8: {void*
_tmp4F5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp38E->topt))->v);{
void*_tmp4F6=_tmp4F5;struct Cyc_Absyn_PtrInfo _tmp4F7;void*_tmp4F8;struct Cyc_Absyn_Tqual
_tmp4F9;struct Cyc_Absyn_PtrAtts _tmp4FA;void*_tmp4FB;struct Cyc_Absyn_Conref*
_tmp4FC;struct Cyc_Absyn_Conref*_tmp4FD;struct Cyc_Absyn_Conref*_tmp4FE;_LL2CB: if(
_tmp4F6 <= (void*)4)goto _LL2CD;if(*((int*)_tmp4F6)!= 4)goto _LL2CD;_tmp4F7=((
struct Cyc_Absyn_PointerType_struct*)_tmp4F6)->f1;_tmp4F8=(void*)_tmp4F7.elt_typ;
_tmp4F9=_tmp4F7.elt_tq;_tmp4FA=_tmp4F7.ptr_atts;_tmp4FB=(void*)_tmp4FA.rgn;
_tmp4FC=_tmp4FA.nullable;_tmp4FD=_tmp4FA.bounds;_tmp4FE=_tmp4FA.zero_term;_LL2CC:{
void*_tmp4FF=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4FD);_LL2D0: if(
_tmp4FF <= (void*)2)goto _LL2D2;if(*((int*)_tmp4FF)!= 0)goto _LL2D2;_LL2D1: {int
do_null_check=Cyc_Toc_need_null_check(_tmp38E);Cyc_Toc_exp_to_c(nv,_tmp38E);if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp500[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp501="inserted null check due to dereference";
_tag_dynforward(_tmp501,sizeof(char),_get_zero_arr_size(_tmp501,39));}),
_tag_dynforward(_tmp500,sizeof(void*),0));});(void*)(_tmp38E->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Toc_typ_to_c(_tmp4F5),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->hd=Cyc_Absyn_copy_exp(_tmp38E);
_tmp502->tl=0;_tmp502;}),0)));}goto _LL2CF;}_LL2D2: if((int)_tmp4FF != 0)goto _LL2D4;
_LL2D3: goto _LL2D5;_LL2D4: if((int)_tmp4FF != 1)goto _LL2D6;_LL2D5: {struct Cyc_Absyn_Exp*
_tmp503=Cyc_Absyn_uint_exp(0,0);_tmp503->topt=({struct Cyc_Core_Opt*_tmp504=
_cycalloc(sizeof(*_tmp504));_tmp504->v=(void*)Cyc_Absyn_uint_typ;_tmp504;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp38E,_tmp503));Cyc_Toc_exp_to_c(nv,
e);goto _LL2CF;}_LL2D6: if(_tmp4FF <= (void*)2)goto _LL2CF;if(*((int*)_tmp4FF)!= 1)
goto _LL2CF;_LL2D7:({void*_tmp505[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp506="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp506,sizeof(char),_get_zero_arr_size(_tmp506,30));}),
_tag_dynforward(_tmp505,sizeof(void*),0));});_LL2CF:;}goto _LL2CA;_LL2CD:;_LL2CE:({
void*_tmp507[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp508="exp_to_c: Deref: non-pointer";
_tag_dynforward(_tmp508,sizeof(char),_get_zero_arr_size(_tmp508,29));}),
_tag_dynforward(_tmp507,sizeof(void*),0));});_LL2CA:;}goto _LL1C4;}_LL1F9: if(*((
int*)_tmp359)!= 23)goto _LL1FB;_tmp38F=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp359)->f1;_tmp390=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp359)->f2;_LL1FA:
Cyc_Toc_exp_to_c(nv,_tmp38F);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1C4;
_LL1FB: if(*((int*)_tmp359)!= 24)goto _LL1FD;_tmp391=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp359)->f1;_tmp392=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp359)->f2;_LL1FC: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp391->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp391);Cyc_Toc_exp_to_c(nv,_tmp391);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp50A;struct Cyc_Absyn_Tqual
_tmp50B;struct Cyc_Absyn_PtrAtts _tmp50C;void*_tmp50D;struct Cyc_Absyn_Conref*
_tmp50E;struct Cyc_Absyn_Conref*_tmp50F;struct Cyc_Absyn_Conref*_tmp510;struct Cyc_Absyn_PtrInfo
_tmp509=Cyc_Toc_get_ptr_type(e1typ);_tmp50A=(void*)_tmp509.elt_typ;_tmp50B=
_tmp509.elt_tq;_tmp50C=_tmp509.ptr_atts;_tmp50D=(void*)_tmp50C.rgn;_tmp50E=
_tmp50C.nullable;_tmp50F=_tmp50C.bounds;_tmp510=_tmp50C.zero_term;{void*_tmp511=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp50F);struct Cyc_Absyn_Exp*_tmp512;
_LL2D9: if(_tmp511 <= (void*)2)goto _LL2DB;if(*((int*)_tmp511)!= 0)goto _LL2DB;
_tmp512=((struct Cyc_Absyn_Upper_b_struct*)_tmp511)->f1;_LL2DA: {unsigned int
_tmp514;int _tmp515;struct _tuple5 _tmp513=Cyc_Evexp_eval_const_uint_exp(_tmp512);
_tmp514=_tmp513.f1;_tmp515=_tmp513.f2;if(_tmp515){if(_tmp514 < 1)({void*_tmp516[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp517="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp517,sizeof(char),_get_zero_arr_size(_tmp517,44));}),_tag_dynforward(_tmp516,
sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*
_tmp518[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp519="inserted null check due to dereference";
_tag_dynforward(_tmp519,sizeof(char),_get_zero_arr_size(_tmp519,39));}),
_tag_dynforward(_tmp518,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp391->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->hd=_tmp391;_tmp51A->tl=0;_tmp51A;}),
0)),_tmp392));}}else{(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp391->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp51B[4];_tmp51B[3]=Cyc_Absyn_uint_exp(0,0);_tmp51B[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp50A,0);_tmp51B[1]=_tmp512;_tmp51B[0]=_tmp391;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp51B,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp392));}goto _LL2D8;}_LL2DB: if((int)_tmp511 != 0)goto _LL2DD;_LL2DC:
forward_only=1;goto _LL2DE;_LL2DD: if((int)_tmp511 != 1)goto _LL2DF;_LL2DE: {struct
Cyc_Absyn_Exp*_tmp51C=forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;
void*ta1=Cyc_Toc_typ_to_c_array(_tmp50A);(void*)(_tmp391->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp50B),Cyc_Absyn_fncall_exp(_tmp51C,({struct Cyc_Absyn_Exp*
_tmp51D[3];_tmp51D[2]=Cyc_Absyn_uint_exp(0,0);_tmp51D[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp51D[0]=Cyc_Absyn_copy_exp(_tmp391);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp51D,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)));goto _LL2D8;}_LL2DF: if(_tmp511 <= (void*)2)goto _LL2D8;if(*((int*)
_tmp511)!= 1)goto _LL2D8;_LL2E0:({void*_tmp51E[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp51F="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp51F,sizeof(char),_get_zero_arr_size(_tmp51F,34));}),
_tag_dynforward(_tmp51E,sizeof(void*),0));});_LL2D8:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1C4;}}
_LL1FD: if(*((int*)_tmp359)!= 25)goto _LL1FF;_tmp393=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp359)->f1;_tmp394=((struct Cyc_Absyn_Subscript_e_struct*)_tmp359)->f2;_LL1FE: {
void*_tmp520=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp393->topt))->v);{void*_tmp521=_tmp520;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_PtrInfo
_tmp523;void*_tmp524;struct Cyc_Absyn_Tqual _tmp525;struct Cyc_Absyn_PtrAtts _tmp526;
void*_tmp527;struct Cyc_Absyn_Conref*_tmp528;struct Cyc_Absyn_Conref*_tmp529;
struct Cyc_Absyn_Conref*_tmp52A;_LL2E2: if(_tmp521 <= (void*)4)goto _LL2E6;if(*((int*)
_tmp521)!= 9)goto _LL2E4;_tmp522=((struct Cyc_Absyn_TupleType_struct*)_tmp521)->f1;
_LL2E3: Cyc_Toc_exp_to_c(nv,_tmp393);Cyc_Toc_exp_to_c(nv,_tmp394);{unsigned int
_tmp52C;int _tmp52D;struct _tuple5 _tmp52B=Cyc_Evexp_eval_const_uint_exp(_tmp394);
_tmp52C=_tmp52B.f1;_tmp52D=_tmp52B.f2;if(!_tmp52D)({void*_tmp52E[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp52F="unknown tuple subscript in translation to C";_tag_dynforward(_tmp52F,
sizeof(char),_get_zero_arr_size(_tmp52F,44));}),_tag_dynforward(_tmp52E,sizeof(
void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp393,Cyc_Absyn_fieldname((
int)(_tmp52C + 1))));goto _LL2E1;}_LL2E4: if(*((int*)_tmp521)!= 4)goto _LL2E6;
_tmp523=((struct Cyc_Absyn_PointerType_struct*)_tmp521)->f1;_tmp524=(void*)
_tmp523.elt_typ;_tmp525=_tmp523.elt_tq;_tmp526=_tmp523.ptr_atts;_tmp527=(void*)
_tmp526.rgn;_tmp528=_tmp526.nullable;_tmp529=_tmp526.bounds;_tmp52A=_tmp526.zero_term;
_LL2E5: {struct Cyc_List_List*_tmp530=Cyc_Toc_get_relns(_tmp393);int in_bnds=0;{
void*_tmp531=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp529);_LL2E9: if(
_tmp531 <= (void*)2)goto _LL2EB;if(*((int*)_tmp531)!= 1)goto _LL2EB;_LL2EA: goto
_LL2E8;_LL2EB:;_LL2EC: in_bnds=Cyc_Toc_check_bounds(_tmp530,_tmp393,_tmp394);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp534;
_tmp534.tag=0;_tmp534.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp532[1]={& _tmp534};Cyc_Tcutil_warn(e->loc,({const char*_tmp533="bounds check necessary for %s";
_tag_dynforward(_tmp533,sizeof(char),_get_zero_arr_size(_tmp533,30));}),
_tag_dynforward(_tmp532,sizeof(void*),1));}});_LL2E8:;}Cyc_Toc_exp_to_c(nv,
_tmp393);Cyc_Toc_exp_to_c(nv,_tmp394);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp535=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp529);
struct Cyc_Absyn_Exp*_tmp536;void*_tmp537;_LL2EE: if(_tmp535 <= (void*)2)goto _LL2F0;
if(*((int*)_tmp535)!= 0)goto _LL2F0;_tmp536=((struct Cyc_Absyn_Upper_b_struct*)
_tmp535)->f1;_LL2EF: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp528);void*ta1=Cyc_Toc_typ_to_c(_tmp524);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp525);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp52A))(void*)(e->r=(void*)
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp538[3];_tmp538[2]=_tmp394;_tmp538[1]=_tmp536;_tmp538[0]=
_tmp393;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp538,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));else{if(
possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp539[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp53A="inserted null check due to dereference";
_tag_dynforward(_tmp53A,sizeof(char),_get_zero_arr_size(_tmp53A,39));}),
_tag_dynforward(_tmp539,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp53B[4];_tmp53B[3]=_tmp394;_tmp53B[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp53B[1]=_tmp536;_tmp53B[0]=_tmp393;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp53B,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))));}else{(void*)(_tmp394->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp53C[2];_tmp53C[1]=Cyc_Absyn_copy_exp(_tmp394);_tmp53C[0]=
_tmp536;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp53C,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL2ED;}
_LL2F0: if((int)_tmp535 != 0)goto _LL2F2;_LL2F1: forward_only=1;goto _LL2F3;_LL2F2:
if((int)_tmp535 != 1)goto _LL2F4;_LL2F3: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp524);
if(in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp525),Cyc_Absyn_aggrmember_exp(
_tmp393,Cyc_Toc_curr_sp,0)),_tmp394));}else{struct Cyc_Absyn_Exp*_tmp53D=
forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp525),Cyc_Absyn_fncall_exp(_tmp53D,({struct Cyc_Absyn_Exp*_tmp53E[3];_tmp53E[2]=
_tmp394;_tmp53E[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp53E[0]=_tmp393;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp53E,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL2ED;}_LL2F4: if(_tmp535 <= (void*)2)goto _LL2ED;if(*((int*)
_tmp535)!= 1)goto _LL2ED;_tmp537=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp535)->f1;_LL2F5:{void*_tmp53F=(void*)_tmp394->annot;struct Cyc_List_List*
_tmp540;_LL2F7: if(*((void**)_tmp53F)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL2F9;
_tmp540=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp53F)->f1;_LL2F8: for(0;
_tmp540 != 0;_tmp540=_tmp540->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp540->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp540->hd)->bd,_tmp537)== 0){in_bnds=1;break;}}goto _LL2F6;_LL2F9:;_LL2FA: goto
_LL2F6;_LL2F6:;}if(!in_bnds)({void*_tmp541[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp542="cannot determine that subscript is in bounds";_tag_dynforward(
_tmp542,sizeof(char),_get_zero_arr_size(_tmp542,45));}),_tag_dynforward(_tmp541,
sizeof(void*),0));});goto _LL2ED;_LL2ED:;}goto _LL2E1;}}_LL2E6:;_LL2E7:({void*
_tmp543[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp544="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp544,sizeof(char),_get_zero_arr_size(_tmp544,49));}),
_tag_dynforward(_tmp543,sizeof(void*),0));});_LL2E1:;}goto _LL1C4;}_LL1FF: if(*((
int*)_tmp359)!= 26)goto _LL201;_tmp395=((struct Cyc_Absyn_Tuple_e_struct*)_tmp359)->f1;
_LL200: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp395))->r));else{struct Cyc_List_List*_tmp545=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp395);void*_tmp546=Cyc_Toc_add_tuple_type(_tmp545);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp395 != 0;(_tmp395=_tmp395->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp395->hd);dles=({struct Cyc_List_List*_tmp547=_cycalloc(
sizeof(*_tmp547));_tmp547->hd=({struct _tuple6*_tmp548=_cycalloc(sizeof(*_tmp548));
_tmp548->f1=0;_tmp548->f2=(struct Cyc_Absyn_Exp*)_tmp395->hd;_tmp548;});_tmp547->tl=
dles;_tmp547;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));}goto _LL1C4;
_LL201: if(*((int*)_tmp359)!= 28)goto _LL203;_tmp396=((struct Cyc_Absyn_Array_e_struct*)
_tmp359)->f1;_LL202:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,_tmp396));{
struct Cyc_List_List*_tmp549=_tmp396;for(0;_tmp549 != 0;_tmp549=_tmp549->tl){
struct _tuple6 _tmp54B;struct Cyc_Absyn_Exp*_tmp54C;struct _tuple6*_tmp54A=(struct
_tuple6*)_tmp549->hd;_tmp54B=*_tmp54A;_tmp54C=_tmp54B.f2;Cyc_Toc_exp_to_c(nv,
_tmp54C);}}goto _LL1C4;_LL203: if(*((int*)_tmp359)!= 29)goto _LL205;_tmp397=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp359)->f1;_tmp398=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp359)->f2;_tmp399=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp359)->f3;
_tmp39A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp359)->f4;_LL204: {
unsigned int _tmp54E;int _tmp54F;struct _tuple5 _tmp54D=Cyc_Evexp_eval_const_uint_exp(
_tmp398);_tmp54E=_tmp54D.f1;_tmp54F=_tmp54D.f2;{void*_tmp550=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp399->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp399);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp399)){if(!_tmp54F)({
void*_tmp551[0]={};Cyc_Tcutil_terr(_tmp398->loc,({const char*_tmp552="cannot determine value of constant";
_tag_dynforward(_tmp552,sizeof(char),_get_zero_arr_size(_tmp552,35));}),
_tag_dynforward(_tmp551,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp54E;i
++){es=({struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->hd=({
struct _tuple6*_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->f1=0;_tmp554->f2=
_tmp399;_tmp554;});_tmp553->tl=es;_tmp553;});}}if(_tmp39A){struct Cyc_Absyn_Exp*
_tmp555=Cyc_Toc_cast_it(_tmp550,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->hd=({struct _tuple6*_tmp557=
_cycalloc(sizeof(*_tmp557));_tmp557->f1=0;_tmp557->f2=_tmp555;_tmp557;});_tmp556->tl=
0;_tmp556;}));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,es));goto
_LL1C4;}}}_LL205: if(*((int*)_tmp359)!= 30)goto _LL207;_tmp39B=((struct Cyc_Absyn_Struct_e_struct*)
_tmp359)->f1;_tmp39C=((struct Cyc_Absyn_Struct_e_struct*)_tmp359)->f2;_tmp39D=((
struct Cyc_Absyn_Struct_e_struct*)_tmp359)->f3;_tmp39E=((struct Cyc_Absyn_Struct_e_struct*)
_tmp359)->f4;_LL206: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp39C != 0,0,0,_tmp39D,_tmp39B))->r));else{if(_tmp39E == 0)({void*
_tmp558[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp559="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp559,
sizeof(char),_get_zero_arr_size(_tmp559,35));}),_tag_dynforward(_tmp558,sizeof(
void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp39E;
struct _RegionHandle _tmp55A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp55A;
_push_region(rgn);{struct Cyc_List_List*_tmp55B=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp39D,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp55C=0;struct Cyc_List_List*_tmp55D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp55D != 0;_tmp55D=_tmp55D->tl){struct Cyc_List_List*
_tmp55E=_tmp55B;for(0;_tmp55E != 0;_tmp55E=_tmp55E->tl){if((*((struct _tuple10*)
_tmp55E->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp55D->hd){struct _tuple10
_tmp560;struct Cyc_Absyn_Aggrfield*_tmp561;struct Cyc_Absyn_Exp*_tmp562;struct
_tuple10*_tmp55F=(struct _tuple10*)_tmp55E->hd;_tmp560=*_tmp55F;_tmp561=_tmp560.f1;
_tmp562=_tmp560.f2;{void*_tmp563=(void*)_tmp561->type;Cyc_Toc_exp_to_c(nv,
_tmp562);if(Cyc_Toc_is_void_star(_tmp563))(void*)(_tmp562->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp562->r,0)));_tmp55C=({
struct Cyc_List_List*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->hd=({struct
_tuple6*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->f1=0;_tmp565->f2=_tmp562;
_tmp565;});_tmp564->tl=_tmp55C;_tmp564;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp55C)));};
_pop_region(rgn);}}goto _LL1C4;_LL207: if(*((int*)_tmp359)!= 31)goto _LL209;_tmp39F=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp359)->f1;_tmp3A0=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp359)->f2;_LL208: {struct Cyc_List_List*fs;{void*_tmp566=Cyc_Tcutil_compress(
_tmp39F);struct Cyc_List_List*_tmp567;_LL2FC: if(_tmp566 <= (void*)4)goto _LL2FE;if(*((
int*)_tmp566)!= 11)goto _LL2FE;_tmp567=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp566)->f2;_LL2FD: fs=_tmp567;goto _LL2FB;_LL2FE:;_LL2FF:({struct Cyc_String_pa_struct
_tmp56A;_tmp56A.tag=0;_tmp56A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp39F));{void*_tmp568[1]={& _tmp56A};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp569="anon struct has type %s";_tag_dynforward(_tmp569,sizeof(char),
_get_zero_arr_size(_tmp569,24));}),_tag_dynforward(_tmp568,sizeof(void*),1));}});
_LL2FB:;}{struct _RegionHandle _tmp56B=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp56B;_push_region(rgn);{struct Cyc_List_List*_tmp56C=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,
_tmp3A0,fs);for(0;_tmp56C != 0;_tmp56C=_tmp56C->tl){struct _tuple10 _tmp56E;struct
Cyc_Absyn_Aggrfield*_tmp56F;struct Cyc_Absyn_Exp*_tmp570;struct _tuple10*_tmp56D=(
struct _tuple10*)_tmp56C->hd;_tmp56E=*_tmp56D;_tmp56F=_tmp56E.f1;_tmp570=_tmp56E.f2;{
void*_tmp571=(void*)_tmp56F->type;Cyc_Toc_exp_to_c(nv,_tmp570);if(Cyc_Toc_is_void_star(
_tmp571))(void*)(_tmp570->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp570->r,0)));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp3A0));};_pop_region(rgn);}goto _LL1C4;}_LL209: if(*((int*)_tmp359)!= 32)goto
_LL20B;_tmp3A1=((struct Cyc_Absyn_Tunion_e_struct*)_tmp359)->f1;if(_tmp3A1 != 0)
goto _LL20B;_tmp3A2=((struct Cyc_Absyn_Tunion_e_struct*)_tmp359)->f2;_tmp3A3=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp359)->f3;if(!(!_tmp3A2->is_flat))goto _LL20B;
_LL20A: {struct _tuple1*qv=_tmp3A3->name;struct Cyc_Absyn_Exp*tag_exp=_tmp3A2->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp3A2,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1C4;}_LL20B: if(*((int*)_tmp359)!= 32)goto _LL20D;
_tmp3A4=((struct Cyc_Absyn_Tunion_e_struct*)_tmp359)->f1;_tmp3A5=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp359)->f2;_tmp3A6=((struct Cyc_Absyn_Tunion_e_struct*)_tmp359)->f3;_LL20C: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp572=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp573=Cyc_Absyn_var_exp(_tmp572,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp3A5->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp3A5->name,({const char*_tmp574="_union";_tag_dynforward(_tmp574,sizeof(char),
_get_zero_arr_size(_tmp574,7));})));tag_exp=Cyc_Toc_tunion_tag(_tmp3A5,_tmp3A6->name,
1);{struct _tuple1 _tmp576;struct _dynforward_ptr*_tmp577;struct _tuple1*_tmp575=
_tmp3A6->name;_tmp576=*_tmp575;_tmp577=_tmp576.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp573,_tmp577,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3A6->name,({const char*_tmp578="_struct";_tag_dynforward(_tmp578,sizeof(char),
_get_zero_arr_size(_tmp578,8));})));tag_exp=_tmp3A5->is_xtunion?Cyc_Absyn_var_exp(
_tmp3A6->name,0): Cyc_Toc_tunion_tag(_tmp3A5,_tmp3A6->name,1);member_exp=_tmp573;}{
struct Cyc_List_List*_tmp579=_tmp3A6->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*
dles=0;for(0;_tmp3A4 != 0;(_tmp3A4=_tmp3A4->tl,_tmp579=_tmp579->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp3A4->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp579))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->hd=({
struct _tuple6*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->f1=0;_tmp57B->f2=
cur_e;_tmp57B;});_tmp57A->tl=dles;_tmp57A;});}dles=({struct Cyc_List_List*_tmp57C=
_cycalloc(sizeof(*_tmp57C));_tmp57C->hd=({struct _tuple6*_tmp57D=_cycalloc(
sizeof(*_tmp57D));_tmp57D->f1=0;_tmp57D->f2=tag_exp;_tmp57D;});_tmp57C->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp57C;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp3A5->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3A6->name,({const char*_tmp57E="_struct";_tag_dynforward(_tmp57E,sizeof(char),
_get_zero_arr_size(_tmp57E,8));}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*
_tmp57F=({struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),
tag_exp,0);_tmp585->tl=0;_tmp585;});{int i=1;for(0;_tmp3A4 != 0;(((_tmp3A4=_tmp3A4->tl,
i ++)),_tmp579=_tmp579->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp3A4->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp579))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp580=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);_tmp57F=({struct Cyc_List_List*_tmp581=_cycalloc(sizeof(*_tmp581));
_tmp581->hd=_tmp580;_tmp581->tl=_tmp57F;_tmp581;});}}}{struct Cyc_Absyn_Stmt*
_tmp582=Cyc_Absyn_exp_stmt(_tmp573,0);struct Cyc_Absyn_Stmt*_tmp583=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->hd=_tmp582;_tmp584->tl=_tmp57F;
_tmp584;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(
_tmp572,tunion_ctype,0,_tmp583,0)));}}goto _LL1C4;}}_LL20D: if(*((int*)_tmp359)!= 
33)goto _LL20F;_LL20E: goto _LL210;_LL20F: if(*((int*)_tmp359)!= 34)goto _LL211;
_LL210: goto _LL1C4;_LL211: if(*((int*)_tmp359)!= 35)goto _LL213;_tmp3A7=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp359)->f1;_tmp3A8=_tmp3A7.is_calloc;_tmp3A9=_tmp3A7.rgn;_tmp3AA=_tmp3A7.elt_type;
_tmp3AB=_tmp3A7.num_elts;_tmp3AC=_tmp3A7.fat_result;_LL212: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp3AA)));Cyc_Toc_exp_to_c(nv,_tmp3AB);if(_tmp3AC){struct
_tuple1*_tmp586=Cyc_Toc_temp_var();struct _tuple1*_tmp587=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp3A8){xexp=_tmp3AB;if(_tmp3A9 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp3A9;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp586,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp3AA,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp586,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp588[3];_tmp588[2]=Cyc_Absyn_var_exp(_tmp586,0);_tmp588[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp588[0]=Cyc_Absyn_var_exp(_tmp587,0);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp588,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp3AB,0);if(_tmp3A9 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp3A9;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp586,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp3AA,Cyc_Absyn_var_exp(_tmp586,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp589[3];_tmp589[2]=Cyc_Absyn_var_exp(_tmp586,0);_tmp589[1]=Cyc_Absyn_uint_exp(
1,0);_tmp589[0]=Cyc_Absyn_var_exp(_tmp587,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp589,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp58A=Cyc_Absyn_declare_stmt(_tmp586,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp587,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp58A));}}else{if(_tmp3A9 != 0){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp3A9;Cyc_Toc_exp_to_c(nv,rgn);(void*)(e->r=(void*)((
void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}else{(void*)(
e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp3AA,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}}
goto _LL1C4;}_LL213: if(*((int*)_tmp359)!= 36)goto _LL215;_tmp3AD=((struct Cyc_Absyn_Swap_e_struct*)
_tmp359)->f1;_tmp3AE=((struct Cyc_Absyn_Swap_e_struct*)_tmp359)->f2;_LL214: {int
is_dynforward_ptr=0;int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3AD->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3AE->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dynforward_ptr,& is_dyneither_ptr))({void*_tmp58B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp58C="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dynforward(_tmp58C,sizeof(char),_get_zero_arr_size(_tmp58C,57));}),
_tag_dynforward(_tmp58B,sizeof(void*),0));});{struct Cyc_Absyn_Exp*swap_fn;if(
is_dynforward_ptr)swap_fn=Cyc_Toc__swap_dynforward_e;else{if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{swap_fn=Cyc_Toc__swap_word_e;}}if(!Cyc_Absyn_is_lvalue(
_tmp3AD))({struct Cyc_String_pa_struct _tmp58F;_tmp58F.tag=0;_tmp58F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp3AD));{void*
_tmp58D[1]={& _tmp58F};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp58E="Swap_e: %s is not an l-value\n";
_tag_dynforward(_tmp58E,sizeof(char),_get_zero_arr_size(_tmp58E,30));}),
_tag_dynforward(_tmp58D,sizeof(void*),1));}});if(!Cyc_Absyn_is_lvalue(_tmp3AE))({
struct Cyc_String_pa_struct _tmp592;_tmp592.tag=0;_tmp592.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(_tmp3AE));{void*
_tmp590[1]={& _tmp592};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp591="Swap_e: %s is not an l-value\n";
_tag_dynforward(_tmp591,sizeof(char),_get_zero_arr_size(_tmp591,30));}),
_tag_dynforward(_tmp590,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp3AD);Cyc_Toc_exp_to_c(
nv,_tmp3AE);(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*
_tmp593[2];_tmp593[1]=Cyc_Absyn_address_exp(_tmp3AE,0);_tmp593[0]=Cyc_Absyn_address_exp(
_tmp3AD,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp593,sizeof(struct Cyc_Absyn_Exp*),2));})));goto _LL1C4;}}
_LL215: if(*((int*)_tmp359)!= 38)goto _LL217;_tmp3AF=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp359)->f1;_LL216: Cyc_Toc_stmt_to_c(nv,_tmp3AF);goto _LL1C4;_LL217: if(*((int*)
_tmp359)!= 37)goto _LL219;_LL218:({void*_tmp594[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp595="UnresolvedMem";_tag_dynforward(_tmp595,sizeof(char),_get_zero_arr_size(
_tmp595,14));}),_tag_dynforward(_tmp594,sizeof(void*),0));});_LL219: if(*((int*)
_tmp359)!= 27)goto _LL1C4;_LL21A:({void*_tmp596[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp597="compoundlit";
_tag_dynforward(_tmp597,sizeof(char),_get_zero_arr_size(_tmp597,12));}),
_tag_dynforward(_tmp596,sizeof(void*),0));});_LL1C4:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple11{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple12{struct _tuple1*f1;void*f2;};struct
_tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp598=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp599;struct Cyc_Absyn_Vardecl
_tmp59A;struct _tuple1*_tmp59B;struct Cyc_Absyn_Pat*_tmp59C;struct Cyc_Absyn_Vardecl*
_tmp59D;struct Cyc_Absyn_Vardecl _tmp59E;struct _tuple1*_tmp59F;struct Cyc_Absyn_Vardecl*
_tmp5A0;struct Cyc_Absyn_Pat*_tmp5A1;void*_tmp5A2;int _tmp5A3;char _tmp5A4;struct
_dynforward_ptr _tmp5A5;struct Cyc_Absyn_Enumdecl*_tmp5A6;struct Cyc_Absyn_Enumfield*
_tmp5A7;void*_tmp5A8;struct Cyc_Absyn_Enumfield*_tmp5A9;struct Cyc_Absyn_Tuniondecl*
_tmp5AA;struct Cyc_Absyn_Tunionfield*_tmp5AB;struct Cyc_List_List*_tmp5AC;struct
Cyc_Absyn_Pat*_tmp5AD;struct Cyc_Absyn_Pat _tmp5AE;void*_tmp5AF;struct Cyc_Absyn_Tuniondecl*
_tmp5B0;struct Cyc_Absyn_Tunionfield*_tmp5B1;struct Cyc_List_List*_tmp5B2;struct
Cyc_Absyn_Tuniondecl*_tmp5B3;struct Cyc_Absyn_Tunionfield*_tmp5B4;struct Cyc_List_List*
_tmp5B5;struct Cyc_List_List*_tmp5B6;struct Cyc_List_List*_tmp5B7;struct Cyc_Absyn_AggrInfo
_tmp5B8;union Cyc_Absyn_AggrInfoU_union _tmp5B9;struct Cyc_List_List*_tmp5BA;struct
Cyc_Absyn_Pat*_tmp5BB;_LL301: if(_tmp598 <= (void*)2)goto _LL305;if(*((int*)_tmp598)
!= 0)goto _LL303;_tmp599=((struct Cyc_Absyn_Var_p_struct*)_tmp598)->f1;_tmp59A=*
_tmp599;_tmp59B=_tmp59A.name;_tmp59C=((struct Cyc_Absyn_Var_p_struct*)_tmp598)->f2;
_LL302: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp59B,r),rgn,t,r,path,
_tmp59C,tag_fail_stmt,fail_stmt,decls);_LL303: if(*((int*)_tmp598)!= 2)goto _LL305;
_tmp59D=((struct Cyc_Absyn_TagInt_p_struct*)_tmp598)->f2;_tmp59E=*_tmp59D;_tmp59F=
_tmp59E.name;_LL304: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp59F,r);goto _LL306;_LL305:
if((int)_tmp598 != 0)goto _LL307;_LL306: s=Cyc_Toc_skip_stmt_dl();goto _LL300;_LL307:
if(_tmp598 <= (void*)2)goto _LL309;if(*((int*)_tmp598)!= 1)goto _LL309;_tmp5A0=((
struct Cyc_Absyn_Reference_p_struct*)_tmp598)->f1;_tmp5A1=((struct Cyc_Absyn_Reference_p_struct*)
_tmp598)->f2;_LL308: {struct _tuple1*_tmp5BC=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp5BD=_region_malloc(rgn,sizeof(*_tmp5BD));_tmp5BD->hd=({struct _tuple12*
_tmp5BE=_region_malloc(rgn,sizeof(*_tmp5BE));_tmp5BE->f1=_tmp5BC;_tmp5BE->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp5BE;});_tmp5BD->tl=decls;_tmp5BD;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp5A0->name,Cyc_Absyn_var_exp(_tmp5BC,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp5BC,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple11 _tmp5BF=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp5A1,tag_fail_stmt,fail_stmt,decls);_tmp5BF.f3=Cyc_Absyn_seq_stmt(
s,_tmp5BF.f3,0);return _tmp5BF;}}_LL309: if((int)_tmp598 != 1)goto _LL30B;_LL30A: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL300;_LL30B:
if(_tmp598 <= (void*)2)goto _LL30D;if(*((int*)_tmp598)!= 7)goto _LL30D;_tmp5A2=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp598)->f1;_tmp5A3=((struct Cyc_Absyn_Int_p_struct*)
_tmp598)->f2;_LL30C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp5A2,_tmp5A3,0),
fail_stmt);goto _LL300;_LL30D: if(_tmp598 <= (void*)2)goto _LL30F;if(*((int*)_tmp598)
!= 8)goto _LL30F;_tmp5A4=((struct Cyc_Absyn_Char_p_struct*)_tmp598)->f1;_LL30E: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp5A4,0),fail_stmt);goto _LL300;_LL30F:
if(_tmp598 <= (void*)2)goto _LL311;if(*((int*)_tmp598)!= 9)goto _LL311;_tmp5A5=((
struct Cyc_Absyn_Float_p_struct*)_tmp598)->f1;_LL310: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp5A5,0),fail_stmt);goto _LL300;_LL311: if(_tmp598 <= (void*)2)goto _LL313;if(*((
int*)_tmp598)!= 10)goto _LL313;_tmp5A6=((struct Cyc_Absyn_Enum_p_struct*)_tmp598)->f1;
_tmp5A7=((struct Cyc_Absyn_Enum_p_struct*)_tmp598)->f2;_LL312: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp5C0=_cycalloc(
sizeof(*_tmp5C0));_tmp5C0[0]=({struct Cyc_Absyn_Enum_e_struct _tmp5C1;_tmp5C1.tag=
33;_tmp5C1.f1=_tmp5A7->name;_tmp5C1.f2=(struct Cyc_Absyn_Enumdecl*)_tmp5A6;
_tmp5C1.f3=(struct Cyc_Absyn_Enumfield*)_tmp5A7;_tmp5C1;});_tmp5C0;}),0),
fail_stmt);goto _LL300;_LL313: if(_tmp598 <= (void*)2)goto _LL315;if(*((int*)_tmp598)
!= 11)goto _LL315;_tmp5A8=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp598)->f1;
_tmp5A9=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp598)->f2;_LL314: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp5C2=_cycalloc(
sizeof(*_tmp5C2));_tmp5C2[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp5C3;_tmp5C3.tag=
34;_tmp5C3.f1=_tmp5A9->name;_tmp5C3.f2=(void*)_tmp5A8;_tmp5C3.f3=(struct Cyc_Absyn_Enumfield*)
_tmp5A9;_tmp5C3;});_tmp5C2;}),0),fail_stmt);goto _LL300;_LL315: if(_tmp598 <= (void*)
2)goto _LL317;if(*((int*)_tmp598)!= 6)goto _LL317;_tmp5AA=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp598)->f1;_tmp5AB=((struct Cyc_Absyn_Tunion_p_struct*)_tmp598)->f2;_tmp5AC=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp598)->f3;if(_tmp5AC != 0)goto _LL317;if(!(!
_tmp5AA->is_flat))goto _LL317;_LL316: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp5AA->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp5AB->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp5AA,_tmp5AB->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL300;}_LL317: if(_tmp598 <= (void*)2)goto _LL319;if(*((
int*)_tmp598)!= 4)goto _LL319;_tmp5AD=((struct Cyc_Absyn_Pointer_p_struct*)_tmp598)->f1;
_tmp5AE=*_tmp5AD;_tmp5AF=(void*)_tmp5AE.r;if(_tmp5AF <= (void*)2)goto _LL319;if(*((
int*)_tmp5AF)!= 6)goto _LL319;_tmp5B0=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5AF)->f1;
_tmp5B1=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5AF)->f2;_tmp5B2=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5AF)->f3;if(!(_tmp5B2 != 0  && !_tmp5B0->is_flat))goto _LL319;_LL318: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp5B1->name,({const
char*_tmp5D0="_struct";_tag_dynforward(_tmp5D0,sizeof(char),_get_zero_arr_size(
_tmp5D0,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp5C4=_tmp5B1->typs;
for(0;_tmp5B2 != 0;(((_tmp5B2=_tmp5B2->tl,_tmp5C4=((struct Cyc_List_List*)
_check_null(_tmp5C4))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp5C5=(struct Cyc_Absyn_Pat*)
_tmp5B2->hd;if((void*)_tmp5C5->r == (void*)0)continue;{void*_tmp5C6=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp5C4))->hd)).f2;struct _tuple1*
_tmp5C7=Cyc_Toc_temp_var();void*_tmp5C8=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5C5->topt))->v;void*_tmp5C9=Cyc_Toc_typ_to_c(_tmp5C8);struct Cyc_Absyn_Exp*
_tmp5CA=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp5C6)))_tmp5CA=Cyc_Toc_cast_it(_tmp5C9,_tmp5CA);decls=({
struct Cyc_List_List*_tmp5CB=_region_malloc(rgn,sizeof(*_tmp5CB));_tmp5CB->hd=({
struct _tuple12*_tmp5CC=_region_malloc(rgn,sizeof(*_tmp5CC));_tmp5CC->f1=_tmp5C7;
_tmp5CC->f2=_tmp5C9;_tmp5CC;});_tmp5CB->tl=decls;_tmp5CB;});{struct _tuple11
_tmp5CD=Cyc_Toc_xlate_pat(nv,rgn,_tmp5C8,Cyc_Absyn_var_exp(_tmp5C7,0),_tmp5CA,
_tmp5C5,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5CD.f1;decls=
_tmp5CD.f2;{struct Cyc_Absyn_Stmt*_tmp5CE=_tmp5CD.f3;struct Cyc_Absyn_Stmt*_tmp5CF=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5C7,0),_tmp5CA,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5CF,_tmp5CE,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp5B0->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp5B1->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp5B0,_tmp5B1->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp5B0);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL300;}}_LL319: if(_tmp598 <= (void*)2)goto
_LL31B;if(*((int*)_tmp598)!= 6)goto _LL31B;_tmp5B3=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp598)->f1;_tmp5B4=((struct Cyc_Absyn_Tunion_p_struct*)_tmp598)->f2;_tmp5B5=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp598)->f3;if(!_tmp5B3->is_flat)goto _LL31B;
_LL31A: {struct _tuple1 _tmp5D2;struct _dynforward_ptr*_tmp5D3;struct _tuple1*
_tmp5D1=_tmp5B4->name;_tmp5D2=*_tmp5D1;_tmp5D3=_tmp5D2.f2;r=Cyc_Absyn_aggrmember_exp(
r,_tmp5D3,0);path=Cyc_Absyn_aggrmember_exp(path,_tmp5D3,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
_tmp5B3,_tmp5B4->name,1),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;
_tmp5B5 != 0;(_tmp5B5=_tmp5B5->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5D4=(struct Cyc_Absyn_Pat*)
_tmp5B5->hd;if((void*)_tmp5D4->r == (void*)0)continue;{struct _tuple1*_tmp5D5=Cyc_Toc_temp_var();
void*_tmp5D6=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5D4->topt))->v;decls=({
struct Cyc_List_List*_tmp5D7=_region_malloc(rgn,sizeof(*_tmp5D7));_tmp5D7->hd=({
struct _tuple12*_tmp5D8=_region_malloc(rgn,sizeof(*_tmp5D8));_tmp5D8->f1=_tmp5D5;
_tmp5D8->f2=Cyc_Toc_typ_to_c(_tmp5D6);_tmp5D8;});_tmp5D7->tl=decls;_tmp5D7;});{
struct _tuple11 _tmp5D9=Cyc_Toc_xlate_pat(nv,rgn,_tmp5D6,Cyc_Absyn_var_exp(_tmp5D5,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5D4,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp5D9.f1;decls=_tmp5D9.f2;{struct Cyc_Absyn_Stmt*
_tmp5DA=_tmp5D9.f3;struct Cyc_Absyn_Stmt*_tmp5DB=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5D5,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5DB,_tmp5DA,0),0);}}}}goto _LL300;}}_LL31B: if(_tmp598 <= (
void*)2)goto _LL31D;if(*((int*)_tmp598)!= 6)goto _LL31D;_tmp5B6=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp598)->f3;_LL31C: _tmp5B7=_tmp5B6;goto _LL31E;_LL31D: if(_tmp598 <= (void*)2)goto
_LL31F;if(*((int*)_tmp598)!= 3)goto _LL31F;_tmp5B7=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp598)->f1;_LL31E: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp5B7 != 0;(
_tmp5B7=_tmp5B7->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5DC=(struct Cyc_Absyn_Pat*)
_tmp5B7->hd;if((void*)_tmp5DC->r == (void*)0)continue;{struct _tuple1*_tmp5DD=Cyc_Toc_temp_var();
void*_tmp5DE=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5DC->topt))->v;decls=({
struct Cyc_List_List*_tmp5DF=_region_malloc(rgn,sizeof(*_tmp5DF));_tmp5DF->hd=({
struct _tuple12*_tmp5E0=_region_malloc(rgn,sizeof(*_tmp5E0));_tmp5E0->f1=_tmp5DD;
_tmp5E0->f2=Cyc_Toc_typ_to_c(_tmp5DE);_tmp5E0;});_tmp5DF->tl=decls;_tmp5DF;});{
struct _tuple11 _tmp5E1=Cyc_Toc_xlate_pat(nv,rgn,_tmp5DE,Cyc_Absyn_var_exp(_tmp5DD,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5DC,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp5E1.f1;decls=_tmp5E1.f2;{struct Cyc_Absyn_Stmt*
_tmp5E2=_tmp5E1.f3;struct Cyc_Absyn_Stmt*_tmp5E3=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5DD,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5E3,_tmp5E2,0),0);}}}}goto _LL300;}_LL31F: if(_tmp598 <= (
void*)2)goto _LL321;if(*((int*)_tmp598)!= 5)goto _LL321;_tmp5B8=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp598)->f1;_tmp5B9=_tmp5B8.aggr_info;_tmp5BA=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp598)->f3;_LL320: {struct Cyc_Absyn_Aggrdecl*_tmp5E4=Cyc_Absyn_get_known_aggrdecl(
_tmp5B9);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp5BA != 0;_tmp5BA=_tmp5BA->tl){struct
_tuple13*_tmp5E5=(struct _tuple13*)_tmp5BA->hd;struct Cyc_Absyn_Pat*_tmp5E6=(*
_tmp5E5).f2;if((void*)_tmp5E6->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp5E7=(void*)((struct Cyc_List_List*)_check_null((*_tmp5E5).f1))->hd;
struct _dynforward_ptr*_tmp5E8;_LL32A: if(*((int*)_tmp5E7)!= 1)goto _LL32C;_tmp5E8=((
struct Cyc_Absyn_FieldName_struct*)_tmp5E7)->f1;_LL32B: f=_tmp5E8;goto _LL329;
_LL32C:;_LL32D:(int)_throw((void*)Cyc_Toc_Match_error);_LL329:;}{struct _tuple1*
_tmp5E9=Cyc_Toc_temp_var();void*_tmp5EA=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5E6->topt))->v;void*_tmp5EB=Cyc_Toc_typ_to_c(_tmp5EA);decls=({struct Cyc_List_List*
_tmp5EC=_region_malloc(rgn,sizeof(*_tmp5EC));_tmp5EC->hd=({struct _tuple12*
_tmp5ED=_region_malloc(rgn,sizeof(*_tmp5ED));_tmp5ED->f1=_tmp5E9;_tmp5ED->f2=
_tmp5EB;_tmp5ED;});_tmp5EC->tl=decls;_tmp5EC;});{struct _tuple11 _tmp5EE=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5EA,Cyc_Absyn_var_exp(_tmp5E9,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp5E6,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5EE.f1;decls=
_tmp5EE.f2;{struct Cyc_Absyn_Exp*_tmp5EF=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5E4->impl))->fields,f)))->type))_tmp5EF=
Cyc_Toc_cast_it(_tmp5EB,_tmp5EF);{struct Cyc_Absyn_Stmt*_tmp5F0=_tmp5EE.f3;struct
Cyc_Absyn_Stmt*_tmp5F1=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5E9,0),
_tmp5EF,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp5F1,_tmp5F0,0),0);}}}}}}
goto _LL300;}_LL321: if(_tmp598 <= (void*)2)goto _LL323;if(*((int*)_tmp598)!= 4)goto
_LL323;_tmp5BB=((struct Cyc_Absyn_Pointer_p_struct*)_tmp598)->f1;_LL322: {struct
_tuple1*_tmp5F2=Cyc_Toc_temp_var();void*_tmp5F3=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5BB->topt))->v;decls=({struct Cyc_List_List*_tmp5F4=
_region_malloc(rgn,sizeof(*_tmp5F4));_tmp5F4->hd=({struct _tuple12*_tmp5F5=
_region_malloc(rgn,sizeof(*_tmp5F5));_tmp5F5->f1=_tmp5F2;_tmp5F5->f2=Cyc_Toc_typ_to_c(
_tmp5F3);_tmp5F5;});_tmp5F4->tl=decls;_tmp5F4;});{struct _tuple11 _tmp5F6=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5F3,Cyc_Absyn_var_exp(_tmp5F2,0),Cyc_Absyn_deref_exp(path,0),_tmp5BB,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5F6.f1;decls=_tmp5F6.f2;{
struct Cyc_Absyn_Stmt*_tmp5F7=_tmp5F6.f3;struct Cyc_Absyn_Stmt*_tmp5F8=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5F2,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp5F7,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp5F8,0);else{s=_tmp5F8;}goto _LL300;}}}_LL323: if(_tmp598 <= (void*)2)goto
_LL325;if(*((int*)_tmp598)!= 12)goto _LL325;_LL324:({void*_tmp5F9[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp5FA="unknownid pat";_tag_dynforward(_tmp5FA,sizeof(char),_get_zero_arr_size(
_tmp5FA,14));}),_tag_dynforward(_tmp5F9,sizeof(void*),0));});_LL325: if(_tmp598 <= (
void*)2)goto _LL327;if(*((int*)_tmp598)!= 13)goto _LL327;_LL326:({void*_tmp5FB[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5FC="unknowncall pat";_tag_dynforward(_tmp5FC,sizeof(char),
_get_zero_arr_size(_tmp5FC,16));}),_tag_dynforward(_tmp5FB,sizeof(void*),0));});
_LL327: if(_tmp598 <= (void*)2)goto _LL300;if(*((int*)_tmp598)!= 14)goto _LL300;
_LL328:({void*_tmp5FD[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5FE="exp pat";
_tag_dynforward(_tmp5FE,sizeof(char),_get_zero_arr_size(_tmp5FE,8));}),
_tag_dynforward(_tmp5FD,sizeof(void*),0));});_LL300:;}return({struct _tuple11
_tmp5FF;_tmp5FF.f1=nv;_tmp5FF.f2=decls;_tmp5FF.f3=s;_tmp5FF;});}struct _tuple14{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple14*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple14*_tmp600=_region_malloc(r,sizeof(*_tmp600));_tmp600->f1=
Cyc_Toc_fresh_label();_tmp600->f2=Cyc_Toc_fresh_label();_tmp600->f3=sc;_tmp600;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp601=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp602;union Cyc_Absyn_TunionInfoU_union _tmp603;struct
Cyc_Absyn_Tuniondecl**_tmp604;struct Cyc_Absyn_Tuniondecl*_tmp605;_LL32F: if(
_tmp601 <= (void*)4)goto _LL331;if(*((int*)_tmp601)!= 2)goto _LL331;_tmp602=((
struct Cyc_Absyn_TunionType_struct*)_tmp601)->f1;_tmp603=_tmp602.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp601)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL331;_tmp604=(_tmp603.KnownTunion).f1;_tmp605=*_tmp604;if(!(!_tmp605->is_flat))
goto _LL331;_LL330: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*
_tmp606=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp605->fields))->v;
for(0;(unsigned int)_tmp606;_tmp606=_tmp606->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp606->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL32E;}_LL331:;_LL332: goto _LL32E;_LL32E:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp607=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp608;struct Cyc_Absyn_Pat*_tmp609;struct Cyc_Absyn_Pat
_tmp60A;void*_tmp60B;struct Cyc_Absyn_Tuniondecl*_tmp60C;struct Cyc_Absyn_Tunionfield*
_tmp60D;struct Cyc_List_List*_tmp60E;_LL334: if(_tmp607 <= (void*)2)goto _LL338;if(*((
int*)_tmp607)!= 0)goto _LL336;_tmp608=((struct Cyc_Absyn_Var_p_struct*)_tmp607)->f2;
_LL335: return Cyc_Toc_no_tag_test(_tmp608);_LL336: if(*((int*)_tmp607)!= 4)goto
_LL338;_tmp609=((struct Cyc_Absyn_Pointer_p_struct*)_tmp607)->f1;_tmp60A=*_tmp609;
_tmp60B=(void*)_tmp60A.r;if(_tmp60B <= (void*)2)goto _LL338;if(*((int*)_tmp60B)!= 
6)goto _LL338;_tmp60C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp60B)->f1;_tmp60D=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp60B)->f2;_tmp60E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp60B)->f3;if(!(_tmp60E != 0  && !_tmp60C->is_flat))goto _LL338;_LL337: return 0;
_LL338:;_LL339: return 1;_LL333:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
Cyc_Toc_exp_to_c(nv,e);{void*_tmp60F=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
int leave_as_switch;{void*_tmp610=Cyc_Tcutil_compress(_tmp60F);_LL33B: if(_tmp610
<= (void*)4)goto _LL33F;if(*((int*)_tmp610)!= 5)goto _LL33D;_LL33C: goto _LL33E;
_LL33D: if(*((int*)_tmp610)!= 12)goto _LL33F;_LL33E: leave_as_switch=1;goto _LL33A;
_LL33F:;_LL340: leave_as_switch=0;goto _LL33A;_LL33A:;}{struct Cyc_List_List*
_tmp611=scs;for(0;_tmp611 != 0;_tmp611=_tmp611->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp611->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp611->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dynforward_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp612=scs;for(0;_tmp612 != 0;_tmp612=_tmp612->tl){struct Cyc_Absyn_Stmt*
_tmp613=((struct Cyc_Absyn_Switch_clause*)_tmp612->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp612->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp613,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp614=_new_region("rgn");struct _RegionHandle*rgn=& _tmp614;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp613);;_pop_region(rgn);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle
_tmp615=_new_region("rgn");struct _RegionHandle*rgn=& _tmp615;_push_region(rgn);{
struct Cyc_Toc_Env*_tmp616=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp60F);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp617=lscs;for(0;_tmp617 != 0;_tmp617=_tmp617->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple14*)_tmp617->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp617->tl
== 0?end_l:(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp617->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp618=_tmp617->tl;for(0;(unsigned int)
_tmp618;_tmp618=_tmp618->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple14*)_tmp618->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp619=_region_malloc(rgn,sizeof(*_tmp619));
_tmp619[0]=Cyc_Absyn_goto_stmt((*((struct _tuple14*)_tmp618->hd)).f1,0);_tmp619;});
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp61A=_region_malloc(rgn,sizeof(*_tmp61A));_tmp61A[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp61A;});}{struct Cyc_Toc_Env*_tmp61C;struct Cyc_List_List*_tmp61D;
struct Cyc_Absyn_Stmt*_tmp61E;struct _tuple11 _tmp61B=Cyc_Toc_xlate_pat(_tmp616,rgn,
_tmp60F,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp61C=_tmp61B.f1;_tmp61D=_tmp61B.f2;_tmp61E=_tmp61B.f3;if(is_tunion  && Cyc_Toc_no_tag_test(
sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*
_tmp61F=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp61C,_tmp61F);_tmp61E=Cyc_Absyn_seq_stmt(_tmp61E,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp61F,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp61D;nvs=({struct Cyc_List_List*_tmp620=_region_malloc(rgn,
sizeof(*_tmp620));_tmp620->hd=_tmp61C;_tmp620->tl=nvs;_tmp620;});test_stmts=({
struct Cyc_List_List*_tmp621=_region_malloc(rgn,sizeof(*_tmp621));_tmp621->hd=
_tmp61E;_tmp621->tl=test_stmts;_tmp621;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple14 _tmp623;struct _dynforward_ptr*_tmp624;struct _dynforward_ptr*
_tmp625;struct Cyc_Absyn_Switch_clause*_tmp626;struct _tuple14*_tmp622=(struct
_tuple14*)lscs->hd;_tmp623=*_tmp622;_tmp624=_tmp623.f1;_tmp625=_tmp623.f2;
_tmp626=_tmp623.f3;{struct Cyc_Toc_Env*_tmp627=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp626->body;{struct _RegionHandle
_tmp628=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp628;_push_region(rgn2);
if(lscs->tl != 0){struct _tuple14 _tmp62A;struct _dynforward_ptr*_tmp62B;struct Cyc_Absyn_Switch_clause*
_tmp62C;struct _tuple14*_tmp629=(struct _tuple14*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp62A=*_tmp629;_tmp62B=_tmp62A.f2;_tmp62C=_tmp62A.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp627,end_l,_tmp62B,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp62C->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp627,end_l),s);};
_pop_region(rgn2);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp624,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp625,s,0),0);stmts=({struct Cyc_List_List*_tmp62D=_region_malloc(rgn,sizeof(*
_tmp62D));_tmp62D->hd=s;_tmp62D->tl=stmts;_tmp62D;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple12 _tmp62F;struct _tuple1*
_tmp630;void*_tmp631;struct _tuple12*_tmp62E=(struct _tuple12*)decls->hd;_tmp62F=*
_tmp62E;_tmp630=_tmp62F.f1;_tmp631=_tmp62F.f2;res=Cyc_Absyn_declare_stmt(_tmp630,
_tmp631,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp632=
_cycalloc(sizeof(*_tmp632));_tmp632->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp632->tl=0;_tmp632;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp633=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp634;struct Cyc_Absyn_Stmt*_tmp635;struct Cyc_Absyn_Stmt*_tmp636;struct Cyc_Absyn_Exp*
_tmp637;struct Cyc_Absyn_Exp*_tmp638;struct Cyc_Absyn_Stmt*_tmp639;struct Cyc_Absyn_Stmt*
_tmp63A;struct _tuple3 _tmp63B;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Stmt*
_tmp63D;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_Absyn_Stmt*_tmp63F;struct Cyc_Absyn_Stmt*
_tmp640;struct Cyc_Absyn_Exp*_tmp641;struct _tuple3 _tmp642;struct Cyc_Absyn_Exp*
_tmp643;struct _tuple3 _tmp644;struct Cyc_Absyn_Exp*_tmp645;struct Cyc_Absyn_Stmt*
_tmp646;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_List_List*_tmp648;struct Cyc_List_List*
_tmp649;struct Cyc_Absyn_Switch_clause**_tmp64A;struct Cyc_Absyn_Decl*_tmp64B;
struct Cyc_Absyn_Stmt*_tmp64C;struct _dynforward_ptr*_tmp64D;struct Cyc_Absyn_Stmt*
_tmp64E;struct Cyc_Absyn_Stmt*_tmp64F;struct _tuple3 _tmp650;struct Cyc_Absyn_Exp*
_tmp651;struct Cyc_Absyn_Stmt*_tmp652;struct Cyc_List_List*_tmp653;struct Cyc_Absyn_Tvar*
_tmp654;struct Cyc_Absyn_Vardecl*_tmp655;int _tmp656;struct Cyc_Absyn_Exp*_tmp657;
struct Cyc_Absyn_Stmt*_tmp658;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Absyn_Exp*
_tmp65A;struct Cyc_Absyn_Tvar*_tmp65B;struct Cyc_Absyn_Vardecl*_tmp65C;struct Cyc_Absyn_Stmt*
_tmp65D;_LL342: if((int)_tmp633 != 0)goto _LL344;_LL343: return;_LL344: if(_tmp633 <= (
void*)1)goto _LL346;if(*((int*)_tmp633)!= 0)goto _LL346;_tmp634=((struct Cyc_Absyn_Exp_s_struct*)
_tmp633)->f1;_LL345: Cyc_Toc_exp_to_c(nv,_tmp634);return;_LL346: if(_tmp633 <= (
void*)1)goto _LL348;if(*((int*)_tmp633)!= 1)goto _LL348;_tmp635=((struct Cyc_Absyn_Seq_s_struct*)
_tmp633)->f1;_tmp636=((struct Cyc_Absyn_Seq_s_struct*)_tmp633)->f2;_LL347: Cyc_Toc_stmt_to_c(
nv,_tmp635);s=_tmp636;continue;_LL348: if(_tmp633 <= (void*)1)goto _LL34A;if(*((int*)
_tmp633)!= 2)goto _LL34A;_tmp637=((struct Cyc_Absyn_Return_s_struct*)_tmp633)->f1;
_LL349: {struct Cyc_Core_Opt*topt=0;if(_tmp637 != 0){topt=({struct Cyc_Core_Opt*
_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp637->topt))->v);_tmp65E;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp637);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp65F=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp660=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp65F,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp65F,(void*)topt->v,_tmp637,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp660,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL34A: if(_tmp633 <= (void*)1)goto _LL34C;if(*((int*)_tmp633)!= 3)goto
_LL34C;_tmp638=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp633)->f1;_tmp639=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp633)->f2;_tmp63A=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp633)->f3;_LL34B: Cyc_Toc_exp_to_c(nv,_tmp638);Cyc_Toc_stmt_to_c(nv,_tmp639);s=
_tmp63A;continue;_LL34C: if(_tmp633 <= (void*)1)goto _LL34E;if(*((int*)_tmp633)!= 4)
goto _LL34E;_tmp63B=((struct Cyc_Absyn_While_s_struct*)_tmp633)->f1;_tmp63C=
_tmp63B.f1;_tmp63D=((struct Cyc_Absyn_While_s_struct*)_tmp633)->f2;_LL34D: Cyc_Toc_exp_to_c(
nv,_tmp63C);{struct _RegionHandle _tmp661=_new_region("temp");struct _RegionHandle*
temp=& _tmp661;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp63D);;_pop_region(temp);}return;_LL34E: if(_tmp633 <= (void*)1)goto _LL350;if(*((
int*)_tmp633)!= 5)goto _LL350;_tmp63E=((struct Cyc_Absyn_Break_s_struct*)_tmp633)->f1;
_LL34F: {struct Cyc_Toc_Env _tmp663;struct _dynforward_ptr**_tmp664;struct Cyc_Toc_Env*
_tmp662=nv;_tmp663=*_tmp662;_tmp664=_tmp663.break_lab;if(_tmp664 != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp664,0));{int dest_depth=_tmp63E == 0?0: _tmp63E->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL350: if(_tmp633 <= (
void*)1)goto _LL352;if(*((int*)_tmp633)!= 6)goto _LL352;_tmp63F=((struct Cyc_Absyn_Continue_s_struct*)
_tmp633)->f1;_LL351: {struct Cyc_Toc_Env _tmp666;struct _dynforward_ptr**_tmp667;
struct Cyc_Toc_Env*_tmp665=nv;_tmp666=*_tmp665;_tmp667=_tmp666.continue_lab;if(
_tmp667 != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp667,0));_tmp640=_tmp63F;
goto _LL353;}_LL352: if(_tmp633 <= (void*)1)goto _LL354;if(*((int*)_tmp633)!= 7)goto
_LL354;_tmp640=((struct Cyc_Absyn_Goto_s_struct*)_tmp633)->f2;_LL353: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp640))->try_depth,s);
return;_LL354: if(_tmp633 <= (void*)1)goto _LL356;if(*((int*)_tmp633)!= 8)goto
_LL356;_tmp641=((struct Cyc_Absyn_For_s_struct*)_tmp633)->f1;_tmp642=((struct Cyc_Absyn_For_s_struct*)
_tmp633)->f2;_tmp643=_tmp642.f1;_tmp644=((struct Cyc_Absyn_For_s_struct*)_tmp633)->f3;
_tmp645=_tmp644.f1;_tmp646=((struct Cyc_Absyn_For_s_struct*)_tmp633)->f4;_LL355:
Cyc_Toc_exp_to_c(nv,_tmp641);Cyc_Toc_exp_to_c(nv,_tmp643);Cyc_Toc_exp_to_c(nv,
_tmp645);{struct _RegionHandle _tmp668=_new_region("temp");struct _RegionHandle*
temp=& _tmp668;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp646);;_pop_region(temp);}return;_LL356: if(_tmp633 <= (void*)1)goto _LL358;if(*((
int*)_tmp633)!= 9)goto _LL358;_tmp647=((struct Cyc_Absyn_Switch_s_struct*)_tmp633)->f1;
_tmp648=((struct Cyc_Absyn_Switch_s_struct*)_tmp633)->f2;_LL357: Cyc_Toc_xlate_switch(
nv,s,_tmp647,_tmp648);return;_LL358: if(_tmp633 <= (void*)1)goto _LL35A;if(*((int*)
_tmp633)!= 10)goto _LL35A;_tmp649=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp633)->f1;
_tmp64A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp633)->f2;_LL359: {struct Cyc_Toc_Env
_tmp66A;struct Cyc_Toc_FallthruInfo*_tmp66B;struct Cyc_Toc_Env*_tmp669=nv;_tmp66A=*
_tmp669;_tmp66B=_tmp66A.fallthru_info;if(_tmp66B == 0)({void*_tmp66C[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp66D="fallthru in unexpected place";_tag_dynforward(_tmp66D,sizeof(char),
_get_zero_arr_size(_tmp66D,29));}),_tag_dynforward(_tmp66C,sizeof(void*),0));});{
struct _dynforward_ptr*_tmp66F;struct Cyc_List_List*_tmp670;struct Cyc_Dict_Dict
_tmp671;struct Cyc_Toc_FallthruInfo _tmp66E=*_tmp66B;_tmp66F=_tmp66E.label;_tmp670=
_tmp66E.binders;_tmp671=_tmp66E.next_case_env;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp66F,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp64A)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp672=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp670);struct Cyc_List_List*
_tmp673=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp649);
for(0;_tmp672 != 0;(_tmp672=_tmp672->tl,_tmp673=_tmp673->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp673))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k))Cyc_Dict_lookup)(_tmp671,(struct _tuple1*)_tmp672->hd),(struct Cyc_Absyn_Exp*)
_tmp673->hd,0),s2,0);}(void*)(s->r=(void*)((void*)s2->r));return;}}}}_LL35A: if(
_tmp633 <= (void*)1)goto _LL35C;if(*((int*)_tmp633)!= 11)goto _LL35C;_tmp64B=((
struct Cyc_Absyn_Decl_s_struct*)_tmp633)->f1;_tmp64C=((struct Cyc_Absyn_Decl_s_struct*)
_tmp633)->f2;_LL35B:{void*_tmp674=(void*)_tmp64B->r;struct Cyc_Absyn_Vardecl*
_tmp675;struct Cyc_Absyn_Pat*_tmp676;struct Cyc_Absyn_Exp*_tmp677;struct Cyc_List_List*
_tmp678;struct Cyc_Absyn_Fndecl*_tmp679;_LL369: if(_tmp674 <= (void*)2)goto _LL371;
if(*((int*)_tmp674)!= 0)goto _LL36B;_tmp675=((struct Cyc_Absyn_Var_d_struct*)
_tmp674)->f1;_LL36A:{struct _RegionHandle _tmp67A=_new_region("temp");struct
_RegionHandle*temp=& _tmp67A;_push_region(temp);{struct Cyc_Toc_Env*_tmp67B=Cyc_Toc_add_varmap(
temp,nv,_tmp675->name,Cyc_Absyn_varb_exp(_tmp675->name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({struct Cyc_Absyn_Local_b_struct
_tmp67D;_tmp67D.tag=3;_tmp67D.f1=_tmp675;_tmp67D;});_tmp67C;}),0));Cyc_Toc_local_decl_to_c(
_tmp67B,_tmp67B,_tmp675,_tmp64C);};_pop_region(temp);}goto _LL368;_LL36B: if(*((
int*)_tmp674)!= 2)goto _LL36D;_tmp676=((struct Cyc_Absyn_Let_d_struct*)_tmp674)->f1;
_tmp677=((struct Cyc_Absyn_Let_d_struct*)_tmp674)->f3;_LL36C:{void*_tmp67E=(void*)
_tmp676->r;struct Cyc_Absyn_Vardecl*_tmp67F;struct Cyc_Absyn_Pat*_tmp680;struct Cyc_Absyn_Pat
_tmp681;void*_tmp682;_LL374: if(_tmp67E <= (void*)2)goto _LL376;if(*((int*)_tmp67E)
!= 0)goto _LL376;_tmp67F=((struct Cyc_Absyn_Var_p_struct*)_tmp67E)->f1;_tmp680=((
struct Cyc_Absyn_Var_p_struct*)_tmp67E)->f2;_tmp681=*_tmp680;_tmp682=(void*)
_tmp681.r;if((int)_tmp682 != 0)goto _LL376;_LL375: {struct _tuple1*old_name=_tmp67F->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp67F->name=new_name;_tmp67F->initializer=(
struct Cyc_Absyn_Exp*)_tmp677;(void*)(_tmp64B->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Absyn_Var_d_struct
_tmp684;_tmp684.tag=0;_tmp684.f1=_tmp67F;_tmp684;});_tmp683;})));{struct
_RegionHandle _tmp685=_new_region("temp");struct _RegionHandle*temp=& _tmp685;
_push_region(temp);{struct Cyc_Toc_Env*_tmp686=Cyc_Toc_add_varmap(temp,nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687[0]=({struct Cyc_Absyn_Local_b_struct
_tmp688;_tmp688.tag=3;_tmp688.f1=_tmp67F;_tmp688;});_tmp687;}),0));Cyc_Toc_local_decl_to_c(
_tmp686,nv,_tmp67F,_tmp64C);};_pop_region(temp);}goto _LL373;}_LL376:;_LL377:(
void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(nv,_tmp676,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp677->topt))->v,_tmp677,_tmp64C))->r));goto _LL373;_LL373:;}goto
_LL368;_LL36D: if(*((int*)_tmp674)!= 3)goto _LL36F;_tmp678=((struct Cyc_Absyn_Letv_d_struct*)
_tmp674)->f1;_LL36E: {struct Cyc_List_List*_tmp689=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp678);if(_tmp689 == 0)({void*_tmp68A[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp68B="empty Letv_d";_tag_dynforward(_tmp68B,sizeof(char),
_get_zero_arr_size(_tmp68B,13));}),_tag_dynforward(_tmp68A,sizeof(void*),0));});(
void*)(_tmp64B->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp68C=
_cycalloc(sizeof(*_tmp68C));_tmp68C[0]=({struct Cyc_Absyn_Var_d_struct _tmp68D;
_tmp68D.tag=0;_tmp68D.f1=(struct Cyc_Absyn_Vardecl*)_tmp689->hd;_tmp68D;});
_tmp68C;})));_tmp689=_tmp689->tl;for(0;_tmp689 != 0;_tmp689=_tmp689->tl){struct
Cyc_Absyn_Decl*_tmp68E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_Absyn_Var_d_struct
_tmp690;_tmp690.tag=0;_tmp690.f1=(struct Cyc_Absyn_Vardecl*)_tmp689->hd;_tmp690;});
_tmp68F;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp68E,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL368;}_LL36F: if(*((int*)
_tmp674)!= 1)goto _LL371;_tmp679=((struct Cyc_Absyn_Fn_d_struct*)_tmp674)->f1;
_LL370: {struct _tuple1*_tmp691=_tmp679->name;{struct _RegionHandle _tmp692=
_new_region("temp");struct _RegionHandle*temp=& _tmp692;_push_region(temp);{struct
Cyc_Toc_Env*_tmp693=Cyc_Toc_add_varmap(temp,nv,_tmp679->name,Cyc_Absyn_var_exp(
_tmp691,0));Cyc_Toc_fndecl_to_c(_tmp693,_tmp679,0);Cyc_Toc_stmt_to_c(_tmp693,
_tmp64C);};_pop_region(temp);}goto _LL368;}_LL371:;_LL372:({void*_tmp694[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp695="bad nested declaration within function";_tag_dynforward(
_tmp695,sizeof(char),_get_zero_arr_size(_tmp695,39));}),_tag_dynforward(_tmp694,
sizeof(void*),0));});_LL368:;}return;_LL35C: if(_tmp633 <= (void*)1)goto _LL35E;if(*((
int*)_tmp633)!= 12)goto _LL35E;_tmp64D=((struct Cyc_Absyn_Label_s_struct*)_tmp633)->f1;
_tmp64E=((struct Cyc_Absyn_Label_s_struct*)_tmp633)->f2;_LL35D: s=_tmp64E;
continue;_LL35E: if(_tmp633 <= (void*)1)goto _LL360;if(*((int*)_tmp633)!= 13)goto
_LL360;_tmp64F=((struct Cyc_Absyn_Do_s_struct*)_tmp633)->f1;_tmp650=((struct Cyc_Absyn_Do_s_struct*)
_tmp633)->f2;_tmp651=_tmp650.f1;_LL35F:{struct _RegionHandle _tmp696=_new_region("temp");
struct _RegionHandle*temp=& _tmp696;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
temp,nv),_tmp64F);Cyc_Toc_exp_to_c(nv,_tmp651);;_pop_region(temp);}return;_LL360:
if(_tmp633 <= (void*)1)goto _LL362;if(*((int*)_tmp633)!= 14)goto _LL362;_tmp652=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp633)->f1;_tmp653=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp633)->f2;_LL361: {struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=
Cyc_Toc_temp_var();struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
e_exp->topt=({struct Cyc_Core_Opt*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->v=(
void*)e_typ;_tmp697;});{struct _RegionHandle _tmp698=_new_region("temp");struct
_RegionHandle*temp=& _tmp698;_push_region(temp);{struct Cyc_Toc_Env*_tmp699=Cyc_Toc_add_varmap(
temp,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp699,_tmp652);{struct Cyc_Absyn_Stmt*
_tmp69A=Cyc_Absyn_seq_stmt(_tmp652,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp69B=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp69C=
Cyc_Absyn_var_exp(_tmp69B,0);struct Cyc_Absyn_Vardecl*_tmp69D=Cyc_Absyn_new_vardecl(
_tmp69B,Cyc_Absyn_exn_typ,0);_tmp69C->topt=({struct Cyc_Core_Opt*_tmp69E=
_cycalloc(sizeof(*_tmp69E));_tmp69E->v=(void*)Cyc_Absyn_exn_typ;_tmp69E;});{
struct Cyc_Absyn_Pat*_tmp69F=({struct Cyc_Absyn_Pat*_tmp6AF=_cycalloc(sizeof(*
_tmp6AF));_tmp6AF->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp6B1=
_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_Absyn_Var_p_struct _tmp6B2;
_tmp6B2.tag=0;_tmp6B2.f1=_tmp69D;_tmp6B2.f2=({struct Cyc_Absyn_Pat*_tmp6B3=
_cycalloc(sizeof(*_tmp6B3));_tmp6B3->r=(void*)((void*)0);_tmp6B3->topt=({struct
Cyc_Core_Opt*_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->v=(void*)Cyc_Absyn_exn_typ;
_tmp6B4;});_tmp6B3->loc=0;_tmp6B3;});_tmp6B2;});_tmp6B1;}));_tmp6AF->topt=({
struct Cyc_Core_Opt*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->v=(void*)Cyc_Absyn_exn_typ;
_tmp6B0;});_tmp6AF->loc=0;_tmp6AF;});struct Cyc_Absyn_Exp*_tmp6A0=Cyc_Absyn_throw_exp(
_tmp69C,0);_tmp6A0->topt=({struct Cyc_Core_Opt*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));
_tmp6A1->v=(void*)((void*)0);_tmp6A1;});{struct Cyc_Absyn_Stmt*_tmp6A2=Cyc_Absyn_exp_stmt(
_tmp6A0,0);struct Cyc_Absyn_Switch_clause*_tmp6A3=({struct Cyc_Absyn_Switch_clause*
_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC->pattern=_tmp69F;_tmp6AC->pat_vars=({
struct Cyc_Core_Opt*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->v=({struct Cyc_List_List*
_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE->hd=_tmp69D;_tmp6AE->tl=0;_tmp6AE;});
_tmp6AD;});_tmp6AC->where_clause=0;_tmp6AC->body=_tmp6A2;_tmp6AC->loc=0;_tmp6AC;});
struct Cyc_Absyn_Stmt*_tmp6A4=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp653,({struct
Cyc_List_List*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB->hd=_tmp6A3;_tmp6AB->tl=
0;_tmp6AB;})),0);Cyc_Toc_stmt_to_c(_tmp699,_tmp6A4);{struct Cyc_Absyn_Exp*_tmp6A5=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp6AA=_cycalloc(
sizeof(*_tmp6AA));_tmp6AA->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp6AA->tl=0;_tmp6AA;}),0);struct Cyc_Absyn_Stmt*_tmp6A6=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp6A9=
_cycalloc(sizeof(*_tmp6A9));_tmp6A9->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp6A9->tl=
0;_tmp6A9;}),0),0);struct Cyc_Absyn_Exp*_tmp6A7=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp6A8=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp6A6,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp6A7,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp6A5,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp6A8,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp69A,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp6A4,0),0),0),0),0),0))->r));}}}}};
_pop_region(temp);}return;}_LL362: if(_tmp633 <= (void*)1)goto _LL364;if(*((int*)
_tmp633)!= 15)goto _LL364;_tmp654=((struct Cyc_Absyn_Region_s_struct*)_tmp633)->f1;
_tmp655=((struct Cyc_Absyn_Region_s_struct*)_tmp633)->f2;_tmp656=((struct Cyc_Absyn_Region_s_struct*)
_tmp633)->f3;_tmp657=((struct Cyc_Absyn_Region_s_struct*)_tmp633)->f4;_tmp658=((
struct Cyc_Absyn_Region_s_struct*)_tmp633)->f5;_LL363: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp655->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);{struct _RegionHandle
_tmp6B5=_new_region("temp");struct _RegionHandle*temp=& _tmp6B5;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp658);;_pop_region(
temp);}if(_tmp657 == 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,
rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp6B6->tl=0;_tmp6B6;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp6B7[1];_tmp6B7[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6B7,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp658,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp6B8[1];_tmp6B8[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6B8,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));else{Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp657);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6B9[2];_tmp6B9[1]=(struct Cyc_Absyn_Exp*)_tmp657;_tmp6B9[
0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6B9,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp658,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6BA[1];_tmp6BA[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6BA,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r));}return;}_LL364: if(_tmp633 <= (void*)1)goto _LL366;if(*((
int*)_tmp633)!= 16)goto _LL366;_tmp659=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp633)->f1;_LL365: Cyc_Toc_exp_to_c(nv,_tmp659);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp6BB=
_cycalloc(sizeof(*_tmp6BB));_tmp6BB->hd=_tmp659;_tmp6BB->tl=0;_tmp6BB;}),0)));
return;_LL366: if(_tmp633 <= (void*)1)goto _LL341;if(*((int*)_tmp633)!= 17)goto
_LL341;_tmp65A=((struct Cyc_Absyn_Alias_s_struct*)_tmp633)->f1;_tmp65B=((struct
Cyc_Absyn_Alias_s_struct*)_tmp633)->f2;_tmp65C=((struct Cyc_Absyn_Alias_s_struct*)
_tmp633)->f3;_tmp65D=((struct Cyc_Absyn_Alias_s_struct*)_tmp633)->f4;_LL367: {
struct _tuple1*old_name=_tmp65C->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp65C->name=new_name;_tmp65C->initializer=(struct Cyc_Absyn_Exp*)_tmp65A;(void*)(
s->r=(void*)((void*)({struct Cyc_Absyn_Decl_s_struct*_tmp6BC=_cycalloc(sizeof(*
_tmp6BC));_tmp6BC[0]=({struct Cyc_Absyn_Decl_s_struct _tmp6BD;_tmp6BD.tag=11;
_tmp6BD.f1=({struct Cyc_Absyn_Decl*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->r=(
void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));
_tmp6BF[0]=({struct Cyc_Absyn_Var_d_struct _tmp6C0;_tmp6C0.tag=0;_tmp6C0.f1=
_tmp65C;_tmp6C0;});_tmp6BF;}));_tmp6BE->loc=0;_tmp6BE;});_tmp6BD.f2=_tmp65D;
_tmp6BD;});_tmp6BC;})));{struct _RegionHandle _tmp6C1=_new_region("temp");struct
_RegionHandle*temp=& _tmp6C1;_push_region(temp);{struct Cyc_Toc_Env*_tmp6C2=Cyc_Toc_add_varmap(
temp,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_Local_b_struct
_tmp6C4;_tmp6C4.tag=3;_tmp6C4.f1=_tmp65C;_tmp6C4;});_tmp6C3;}),0));Cyc_Toc_local_decl_to_c(
_tmp6C2,nv,_tmp65C,_tmp65D);};_pop_region(temp);}return;}_LL341:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple15{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct _RegionHandle _tmp6C5=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp6C5;_push_region(frgn);{struct Cyc_Toc_Env*_tmp6C6=Cyc_Toc_share_env(
frgn,nv);{struct Cyc_List_List*_tmp6C7=f->args;for(0;_tmp6C7 != 0;_tmp6C7=_tmp6C7->tl){
struct _tuple1*_tmp6C8=({struct _tuple1*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));
_tmp6C9->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union
Cyc_Absyn_Nmspace_union _tmp6CA;(_tmp6CA.Loc_n).tag=0;_tmp6CA;}));_tmp6C9->f2=(*((
struct _tuple15*)_tmp6C7->hd)).f1;_tmp6C9;});(*((struct _tuple15*)_tmp6C7->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp6C7->hd)).f3);_tmp6C6=Cyc_Toc_add_varmap(
frgn,_tmp6C6,_tmp6C8,Cyc_Absyn_var_exp(_tmp6C8,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(
f->body);_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp6CC;struct Cyc_Absyn_Tqual
_tmp6CD;void*_tmp6CE;int _tmp6CF;struct Cyc_Absyn_VarargInfo _tmp6CB=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp6CC=_tmp6CB.name;_tmp6CD=_tmp6CB.tq;_tmp6CE=(
void*)_tmp6CB.type;_tmp6CF=_tmp6CB.inject;{void*_tmp6D0=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp6CE,(void*)2,_tmp6CD,Cyc_Absyn_false_conref));struct _tuple1*_tmp6D1=({struct
_tuple1*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp6D5;(_tmp6D5.Loc_n).tag=
0;_tmp6D5;}));_tmp6D4->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp6CC))->v;_tmp6D4;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp6D2=
_cycalloc(sizeof(*_tmp6D2));_tmp6D2->hd=({struct _tuple15*_tmp6D3=_cycalloc(
sizeof(*_tmp6D3));_tmp6D3->f1=(struct _dynforward_ptr*)_tmp6CC->v;_tmp6D3->f2=
_tmp6CD;_tmp6D3->f3=_tmp6D0;_tmp6D3;});_tmp6D2->tl=0;_tmp6D2;}));_tmp6C6=Cyc_Toc_add_varmap(
frgn,_tmp6C6,_tmp6D1,Cyc_Absyn_var_exp(_tmp6D1,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp6D6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp6D6 != 0;_tmp6D6=_tmp6D6->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp6D6->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp6D6->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp6C6),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp6D7=s;_LL379:
if((int)_tmp6D7 != 1)goto _LL37B;_LL37A: return(void*)2;_LL37B: if((int)_tmp6D7 != 4)
goto _LL37D;_LL37C: return(void*)3;_LL37D:;_LL37E: return s;_LL378:;}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*_tmp6D8=ad->name;struct
_DynRegionHandle*_tmp6DA;struct Cyc_Dict_Dict*_tmp6DB;struct Cyc_Toc_TocState
_tmp6D9=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp6DA=
_tmp6D9.dyn;_tmp6DB=_tmp6D9.aggrs_so_far;{struct _DynRegionFrame _tmp6DC;struct
_RegionHandle*d=_open_dynregion(& _tmp6DC,_tmp6DA);{int seen_defn_before;{struct
_RegionHandle _tmp6DD=_new_region("temp");struct _RegionHandle*temp=& _tmp6DD;
_push_region(temp);{struct Cyc_Absyn_Aggrdecl**_tmp6DE=((struct Cyc_Absyn_Aggrdecl**(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_rlookup_opt)(
temp,*_tmp6DB,_tmp6D8);if(_tmp6DE == 0){seen_defn_before=0;*_tmp6DB=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(*
_tmp6DB,_tmp6D8,ad);}else{if((*_tmp6DE)->impl == 0){*_tmp6DB=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(*
_tmp6DB,_tmp6D8,ad);seen_defn_before=0;}else{seen_defn_before=1;}}};_pop_region(
temp);}(void*)(ad->sc=(void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp6DF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp6DF != 0;_tmp6DF=_tmp6DF->tl){((struct Cyc_Absyn_Aggrfield*)_tmp6DF->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp6DF->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp6DF->hd)->type));}}}};_pop_dynregion(d);}}
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud){struct
_DynRegionHandle*_tmp6E1;struct Cyc_Set_Set**_tmp6E2;struct Cyc_Toc_TocState
_tmp6E0=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp6E1=
_tmp6E0.dyn;_tmp6E2=_tmp6E0.tunions_so_far;{struct _DynRegionFrame _tmp6E3;struct
_RegionHandle*d=_open_dynregion(& _tmp6E3,_tmp6E1);{struct _tuple1*_tmp6E4=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*
_tmp6E2,_tmp6E4)){_npop_handler(0);return;}*_tmp6E2=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp6E2,_tmp6E4);};_pop_dynregion(d);}{struct Cyc_List_List*flat_structs=0;{
struct Cyc_List_List*_tmp6E5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp6E5 != 0;_tmp6E5=_tmp6E5->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp6E5->hd;if(f->typs != 0  || tud->is_flat){
struct Cyc_List_List*_tmp6E6=0;int i=1;{struct Cyc_List_List*_tmp6E7=f->typs;for(0;
_tmp6E7 != 0;(_tmp6E7=_tmp6E7->tl,i ++)){struct _dynforward_ptr*_tmp6E8=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp6E9=({struct Cyc_Absyn_Aggrfield*_tmp6EB=
_cycalloc(sizeof(*_tmp6EB));_tmp6EB->name=_tmp6E8;_tmp6EB->tq=(*((struct _tuple4*)
_tmp6E7->hd)).f1;_tmp6EB->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp6E7->hd)).f2);_tmp6EB->width=0;_tmp6EB->attributes=0;_tmp6EB;});_tmp6E6=({
struct Cyc_List_List*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA->hd=_tmp6E9;
_tmp6EA->tl=_tmp6E6;_tmp6EA;});}}_tmp6E6=({struct Cyc_List_List*_tmp6EC=_cycalloc(
sizeof(*_tmp6EC));_tmp6EC->hd=({struct Cyc_Absyn_Aggrfield*_tmp6ED=_cycalloc(
sizeof(*_tmp6ED));_tmp6ED->name=Cyc_Toc_tag_sp;_tmp6ED->tq=Cyc_Toc_mt_tq;_tmp6ED->type=(
void*)Cyc_Absyn_sint_typ;_tmp6ED->width=0;_tmp6ED->attributes=0;_tmp6ED;});
_tmp6EC->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp6E6);_tmp6EC;});{struct Cyc_Absyn_Aggrdecl*_tmp6EE=({struct Cyc_Absyn_Aggrdecl*
_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA->kind=(void*)((void*)0);_tmp6FA->sc=(
void*)((void*)2);_tmp6FA->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp6FC="_struct";_tag_dynforward(_tmp6FC,sizeof(char),_get_zero_arr_size(
_tmp6FC,8));}));_tmp6FA->tvs=0;_tmp6FA->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB->exist_vars=0;_tmp6FB->rgn_po=0;
_tmp6FB->fields=_tmp6E6;_tmp6FB;});_tmp6FA->attributes=0;_tmp6FA;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));
_tmp6F0[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp6F1;_tmp6F1.tag=4;_tmp6F1.f1=
_tmp6EE;_tmp6F1;});_tmp6F0;}),0);_tmp6EF->tl=Cyc_Toc_result_decls;_tmp6EF;});if(
tud->is_flat){struct Cyc_Absyn_Aggrfield*_tmp6F2=({struct Cyc_Absyn_Aggrfield*
_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4->name=(*f->name).f2;_tmp6F4->tq=Cyc_Toc_mt_tq;
_tmp6F4->type=(void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp6F5=_cycalloc(
sizeof(*_tmp6F5));_tmp6F5[0]=({struct Cyc_Absyn_AggrType_struct _tmp6F6;_tmp6F6.tag=
10;_tmp6F6.f1=({struct Cyc_Absyn_AggrInfo _tmp6F7;_tmp6F7.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp6F8;(_tmp6F8.KnownAggr).tag=1;(_tmp6F8.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9[0]=_tmp6EE;
_tmp6F9;});_tmp6F8;});_tmp6F7.targs=0;_tmp6F7;});_tmp6F6;});_tmp6F5;}));_tmp6F4->width=
0;_tmp6F4->attributes=0;_tmp6F4;});flat_structs=({struct Cyc_List_List*_tmp6F3=
_cycalloc(sizeof(*_tmp6F3));_tmp6F3->hd=_tmp6F2;_tmp6F3->tl=flat_structs;_tmp6F3;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp6FD=({struct Cyc_Absyn_Aggrdecl*
_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701->kind=(void*)((void*)1);_tmp701->sc=(
void*)((void*)2);_tmp701->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp703="_union";_tag_dynforward(_tmp703,sizeof(char),_get_zero_arr_size(_tmp703,
7));}));_tmp701->tvs=0;_tmp701->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp702=
_cycalloc(sizeof(*_tmp702));_tmp702->exist_vars=0;_tmp702->rgn_po=0;_tmp702->fields=
flat_structs;_tmp702;});_tmp701->attributes=0;_tmp701;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));
_tmp6FF[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp700;_tmp700.tag=4;_tmp700.f1=
_tmp6FD;_tmp700;});_tmp6FF;}),0);_tmp6FE->tl=Cyc_Toc_result_decls;_tmp6FE;});}}}}
static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp705;struct Cyc_Dict_Dict*_tmp706;struct
Cyc_Toc_TocState _tmp704=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp705=_tmp704.dyn;_tmp706=_tmp704.xtunions_so_far;{struct _DynRegionFrame
_tmp707;struct _RegionHandle*d=_open_dynregion(& _tmp707,_tmp705);{struct
_RegionHandle _tmp708=_new_region("temp");struct _RegionHandle*temp=& _tmp708;
_push_region(temp);{struct _tuple1*_tmp709=xd->name;struct Cyc_List_List*_tmp70A=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;
_tmp70A != 0;_tmp70A=_tmp70A->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp70A->hd;struct _dynforward_ptr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp70B=
Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(char))+ 4,0);void*_tmp70C=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp70B,Cyc_Absyn_false_conref,
0);int*_tmp70D=((int*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d,struct _tuple1*
k))Cyc_Dict_rlookup_opt)(temp,*_tmp706,f->name);int _tmp70E;_LL380: if(_tmp70D != 0)
goto _LL382;_LL381: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){
char zero='\000';initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp715;_tmp715.tag=0;_tmp715.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct
_tmp714;_tmp714.tag=1;_tmp714.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp713;_tmp713.tag=1;_tmp713.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp712;_tmp712.tag=1;_tmp712.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp711;_tmp711.tag=1;_tmp711.f1=(unsigned long)((int)zero);{void*_tmp70F[5]={&
_tmp711,& _tmp712,& _tmp713,& _tmp714,& _tmp715};Cyc_aprintf(({const char*_tmp710="%c%c%c%c%s";
_tag_dynforward(_tmp710,sizeof(char),_get_zero_arr_size(_tmp710,11));}),
_tag_dynforward(_tmp70F,sizeof(void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*
_tmp716=Cyc_Absyn_new_vardecl(f->name,_tmp70C,initopt);(void*)(_tmp716->sc=(void*)((
void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp717=_cycalloc(
sizeof(*_tmp717));_tmp717->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718[0]=({struct Cyc_Absyn_Var_d_struct
_tmp719;_tmp719.tag=0;_tmp719.f1=_tmp716;_tmp719;});_tmp718;}),0);_tmp717->tl=
Cyc_Toc_result_decls;_tmp717;});*_tmp706=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp706,f->name,(void*)f->sc != (void*)
3);if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*
_tmp71A=f->typs;for(0;_tmp71A != 0;(_tmp71A=_tmp71A->tl,i ++)){struct
_dynforward_ptr*_tmp71B=({struct _dynforward_ptr*_tmp71F=_cycalloc(sizeof(*
_tmp71F));_tmp71F[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp722;
_tmp722.tag=1;_tmp722.f1=(unsigned long)i;{void*_tmp720[1]={& _tmp722};Cyc_aprintf(({
const char*_tmp721="f%d";_tag_dynforward(_tmp721,sizeof(char),_get_zero_arr_size(
_tmp721,4));}),_tag_dynforward(_tmp720,sizeof(void*),1));}});_tmp71F;});struct
Cyc_Absyn_Aggrfield*_tmp71C=({struct Cyc_Absyn_Aggrfield*_tmp71E=_cycalloc(
sizeof(*_tmp71E));_tmp71E->name=_tmp71B;_tmp71E->tq=(*((struct _tuple4*)_tmp71A->hd)).f1;
_tmp71E->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp71A->hd)).f2);
_tmp71E->width=0;_tmp71E->attributes=0;_tmp71E;});fields=({struct Cyc_List_List*
_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->hd=_tmp71C;_tmp71D->tl=fields;
_tmp71D;});}}fields=({struct Cyc_List_List*_tmp723=_cycalloc(sizeof(*_tmp723));
_tmp723->hd=({struct Cyc_Absyn_Aggrfield*_tmp724=_cycalloc(sizeof(*_tmp724));
_tmp724->name=Cyc_Toc_tag_sp;_tmp724->tq=Cyc_Toc_mt_tq;_tmp724->type=(void*)Cyc_Absyn_cstar_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp724->width=0;_tmp724->attributes=0;_tmp724;});
_tmp723->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp723;});{struct Cyc_Absyn_Aggrdecl*_tmp725=({struct Cyc_Absyn_Aggrdecl*
_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729->kind=(void*)((void*)0);_tmp729->sc=(
void*)((void*)2);_tmp729->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp72B="_struct";_tag_dynforward(_tmp72B,sizeof(char),_get_zero_arr_size(
_tmp72B,8));}));_tmp729->tvs=0;_tmp729->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A->exist_vars=0;_tmp72A->rgn_po=0;
_tmp72A->fields=fields;_tmp72A;});_tmp729->attributes=0;_tmp729;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp726=_cycalloc(sizeof(*_tmp726));_tmp726->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp727=_cycalloc(sizeof(*_tmp727));
_tmp727[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp728;_tmp728.tag=4;_tmp728.f1=
_tmp725;_tmp728;});_tmp727;}),0);_tmp726->tl=Cyc_Toc_result_decls;_tmp726;});}}
goto _LL37F;}}_LL382: if(_tmp70D == 0)goto _LL384;_tmp70E=*_tmp70D;if(_tmp70E != 0)
goto _LL384;_LL383: if((void*)f->sc != (void*)3){char zero='\000';struct Cyc_Absyn_Exp*
_tmp72C=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp737;_tmp737.tag=0;_tmp737.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
fn);{struct Cyc_Int_pa_struct _tmp736;_tmp736.tag=1;_tmp736.f1=(unsigned long)((
int)zero);{struct Cyc_Int_pa_struct _tmp735;_tmp735.tag=1;_tmp735.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp734;_tmp734.tag=1;_tmp734.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp733;_tmp733.tag=1;_tmp733.f1=(
unsigned long)((int)zero);{void*_tmp731[5]={& _tmp733,& _tmp734,& _tmp735,& _tmp736,&
_tmp737};Cyc_aprintf(({const char*_tmp732="%c%c%c%c%s";_tag_dynforward(_tmp732,
sizeof(char),_get_zero_arr_size(_tmp732,11));}),_tag_dynforward(_tmp731,sizeof(
void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp72D=Cyc_Absyn_new_vardecl(f->name,
_tmp70C,(struct Cyc_Absyn_Exp*)_tmp72C);(void*)(_tmp72D->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp72E=_cycalloc(sizeof(*_tmp72E));
_tmp72E->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp72F=
_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({struct Cyc_Absyn_Var_d_struct _tmp730;
_tmp730.tag=0;_tmp730.f1=_tmp72D;_tmp730;});_tmp72F;}),0);_tmp72E->tl=Cyc_Toc_result_decls;
_tmp72E;});*_tmp706=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*
k,int v))Cyc_Dict_insert)(*_tmp706,f->name,1);}goto _LL37F;_LL384:;_LL385: goto
_LL37F;_LL37F:;}};_pop_region(temp);};_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp738=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp739;struct Cyc_Absyn_Exp*_tmp73A;struct Cyc_Absyn_Exp*_tmp73B;int _tmp73C;
_LL387: if(*((int*)_tmp738)!= 29)goto _LL389;_tmp739=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp738)->f1;_tmp73A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp738)->f2;
_tmp73B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp738)->f3;_tmp73C=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp738)->f4;_LL388: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp739,_tmp73A,_tmp73B,_tmp73C,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL386;_LL389:;_LL38A: if((void*)vd->sc == (void*)0){struct _RegionHandle
_tmp73D=_new_region("temp");struct _RegionHandle*temp=& _tmp73D;_push_region(temp);{
struct Cyc_Toc_Env*_tmp73E=Cyc_Toc_set_toplevel(temp,init_nv);Cyc_Toc_exp_to_c(
_tmp73E,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(init_nv,init);}goto
_LL386;_LL386:;}else{void*_tmp73F=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo
_tmp740;void*_tmp741;struct Cyc_Absyn_Exp*_tmp742;struct Cyc_Absyn_Conref*_tmp743;
_LL38C: if(_tmp73F <= (void*)4)goto _LL38E;if(*((int*)_tmp73F)!= 7)goto _LL38E;
_tmp740=((struct Cyc_Absyn_ArrayType_struct*)_tmp73F)->f1;_tmp741=(void*)_tmp740.elt_type;
_tmp742=_tmp740.num_elts;_tmp743=_tmp740.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp743))goto _LL38E;_LL38D: if(_tmp742 == 0)({void*
_tmp744[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp745="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp745,sizeof(char),_get_zero_arr_size(_tmp745,55));}),
_tag_dynforward(_tmp744,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp742;struct Cyc_Absyn_Exp*_tmp746=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp747=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp746,_tmp747,
0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL38B;}_LL38E:;_LL38F: goto _LL38B;
_LL38B:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static
struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp748=_cycalloc(
sizeof(*_tmp748));_tmp748[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp748;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp749=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp749;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp74A=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp74B=Cyc_Toc_share_env(
prgn,nv);struct Cyc_Toc_Env*_tmp74D;struct Cyc_List_List*_tmp74E;struct Cyc_Absyn_Stmt*
_tmp74F;struct _tuple11 _tmp74C=Cyc_Toc_xlate_pat(_tmp74B,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)& _tmp74A,Cyc_Toc_throw_match_stmt(),
0);_tmp74D=_tmp74C.f1;_tmp74E=_tmp74C.f2;_tmp74F=_tmp74C.f3;Cyc_Toc_stmt_to_c(
_tmp74D,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp74F,s,0),0);for(0;_tmp74E != 0;_tmp74E=_tmp74E->tl){
struct _tuple12 _tmp751;struct _tuple1*_tmp752;void*_tmp753;struct _tuple12*_tmp750=(
struct _tuple12*)_tmp74E->hd;_tmp751=*_tmp750;_tmp752=_tmp751.f1;_tmp753=_tmp751.f2;
s=Cyc_Absyn_declare_stmt(_tmp752,_tmp753,0,s,0);}};_pop_region(prgn);}return s;}}
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp754=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp755;struct Cyc_Absyn_Exp*_tmp756;struct Cyc_Absyn_Exp*
_tmp757;struct Cyc_Absyn_Exp*_tmp758;struct Cyc_Absyn_Exp*_tmp759;struct Cyc_Absyn_Exp*
_tmp75A;struct Cyc_Absyn_Exp*_tmp75B;struct Cyc_Absyn_Exp*_tmp75C;struct Cyc_List_List*
_tmp75D;struct Cyc_Absyn_Exp*_tmp75E;struct Cyc_Absyn_Exp*_tmp75F;struct Cyc_Absyn_Exp*
_tmp760;struct Cyc_Absyn_Exp*_tmp761;struct Cyc_Absyn_Exp*_tmp762;struct Cyc_Absyn_Exp*
_tmp763;struct Cyc_Absyn_Exp*_tmp764;struct Cyc_Absyn_Exp*_tmp765;struct Cyc_Absyn_Exp*
_tmp766;struct Cyc_Absyn_Exp*_tmp767;struct Cyc_Absyn_Exp*_tmp768;struct Cyc_Absyn_Exp*
_tmp769;struct Cyc_Absyn_Exp*_tmp76A;struct Cyc_Absyn_Exp*_tmp76B;struct Cyc_Absyn_Exp*
_tmp76C;struct Cyc_Absyn_Exp*_tmp76D;struct Cyc_List_List*_tmp76E;struct Cyc_Absyn_Exp*
_tmp76F;struct Cyc_List_List*_tmp770;void*_tmp771;void**_tmp772;struct Cyc_Absyn_Exp*
_tmp773;struct _tuple2*_tmp774;struct _tuple2 _tmp775;void*_tmp776;void**_tmp777;
struct Cyc_List_List*_tmp778;struct Cyc_List_List*_tmp779;struct Cyc_List_List*
_tmp77A;void*_tmp77B;void**_tmp77C;void*_tmp77D;void**_tmp77E;struct Cyc_Absyn_Stmt*
_tmp77F;struct Cyc_Absyn_MallocInfo _tmp780;struct Cyc_Absyn_MallocInfo*_tmp781;
_LL391: if(*((int*)_tmp754)!= 22)goto _LL393;_tmp755=((struct Cyc_Absyn_Deref_e_struct*)
_tmp754)->f1;_LL392: _tmp756=_tmp755;goto _LL394;_LL393: if(*((int*)_tmp754)!= 23)
goto _LL395;_tmp756=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp754)->f1;_LL394:
_tmp757=_tmp756;goto _LL396;_LL395: if(*((int*)_tmp754)!= 24)goto _LL397;_tmp757=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp754)->f1;_LL396: _tmp758=_tmp757;goto
_LL398;_LL397: if(*((int*)_tmp754)!= 16)goto _LL399;_tmp758=((struct Cyc_Absyn_Address_e_struct*)
_tmp754)->f1;_LL398: _tmp759=_tmp758;goto _LL39A;_LL399: if(*((int*)_tmp754)!= 12)
goto _LL39B;_tmp759=((struct Cyc_Absyn_Throw_e_struct*)_tmp754)->f1;_LL39A: _tmp75A=
_tmp759;goto _LL39C;_LL39B: if(*((int*)_tmp754)!= 13)goto _LL39D;_tmp75A=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp754)->f1;_LL39C: _tmp75B=_tmp75A;goto _LL39E;
_LL39D: if(*((int*)_tmp754)!= 19)goto _LL39F;_tmp75B=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp754)->f1;_LL39E: _tmp75C=_tmp75B;goto _LL3A0;_LL39F: if(*((int*)_tmp754)!= 5)
goto _LL3A1;_tmp75C=((struct Cyc_Absyn_Increment_e_struct*)_tmp754)->f1;_LL3A0: Cyc_Toc_exptypes_to_c(
_tmp75C);goto _LL390;_LL3A1: if(*((int*)_tmp754)!= 3)goto _LL3A3;_tmp75D=((struct
Cyc_Absyn_Primop_e_struct*)_tmp754)->f2;_LL3A2:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp75D);goto _LL390;
_LL3A3: if(*((int*)_tmp754)!= 7)goto _LL3A5;_tmp75E=((struct Cyc_Absyn_And_e_struct*)
_tmp754)->f1;_tmp75F=((struct Cyc_Absyn_And_e_struct*)_tmp754)->f2;_LL3A4: _tmp760=
_tmp75E;_tmp761=_tmp75F;goto _LL3A6;_LL3A5: if(*((int*)_tmp754)!= 8)goto _LL3A7;
_tmp760=((struct Cyc_Absyn_Or_e_struct*)_tmp754)->f1;_tmp761=((struct Cyc_Absyn_Or_e_struct*)
_tmp754)->f2;_LL3A6: _tmp762=_tmp760;_tmp763=_tmp761;goto _LL3A8;_LL3A7: if(*((int*)
_tmp754)!= 9)goto _LL3A9;_tmp762=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp754)->f1;
_tmp763=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp754)->f2;_LL3A8: _tmp764=_tmp762;
_tmp765=_tmp763;goto _LL3AA;_LL3A9: if(*((int*)_tmp754)!= 25)goto _LL3AB;_tmp764=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp754)->f1;_tmp765=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp754)->f2;_LL3AA: _tmp766=_tmp764;_tmp767=_tmp765;goto _LL3AC;_LL3AB: if(*((int*)
_tmp754)!= 36)goto _LL3AD;_tmp766=((struct Cyc_Absyn_Swap_e_struct*)_tmp754)->f1;
_tmp767=((struct Cyc_Absyn_Swap_e_struct*)_tmp754)->f2;_LL3AC: _tmp768=_tmp766;
_tmp769=_tmp767;goto _LL3AE;_LL3AD: if(*((int*)_tmp754)!= 4)goto _LL3AF;_tmp768=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp754)->f1;_tmp769=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp754)->f3;_LL3AE: Cyc_Toc_exptypes_to_c(_tmp768);Cyc_Toc_exptypes_to_c(_tmp769);
goto _LL390;_LL3AF: if(*((int*)_tmp754)!= 6)goto _LL3B1;_tmp76A=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp754)->f1;_tmp76B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp754)->f2;
_tmp76C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp754)->f3;_LL3B0: Cyc_Toc_exptypes_to_c(
_tmp76A);Cyc_Toc_exptypes_to_c(_tmp76B);Cyc_Toc_exptypes_to_c(_tmp76C);goto
_LL390;_LL3B1: if(*((int*)_tmp754)!= 11)goto _LL3B3;_tmp76D=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp754)->f1;_tmp76E=((struct Cyc_Absyn_FnCall_e_struct*)_tmp754)->f2;_LL3B2:
_tmp76F=_tmp76D;_tmp770=_tmp76E;goto _LL3B4;_LL3B3: if(*((int*)_tmp754)!= 10)goto
_LL3B5;_tmp76F=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp754)->f1;_tmp770=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp754)->f2;_LL3B4: Cyc_Toc_exptypes_to_c(
_tmp76F);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp770);goto _LL390;_LL3B5: if(*((int*)_tmp754)!= 15)goto
_LL3B7;_tmp771=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp754)->f1;_tmp772=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp754)->f1);_tmp773=((struct
Cyc_Absyn_Cast_e_struct*)_tmp754)->f2;_LL3B6:*_tmp772=Cyc_Toc_typ_to_c(*_tmp772);
Cyc_Toc_exptypes_to_c(_tmp773);goto _LL390;_LL3B7: if(*((int*)_tmp754)!= 27)goto
_LL3B9;_tmp774=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp754)->f1;_tmp775=*
_tmp774;_tmp776=_tmp775.f3;_tmp777=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp754)->f1).f3;_tmp778=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp754)->f2;
_LL3B8:*_tmp777=Cyc_Toc_typ_to_c(*_tmp777);_tmp779=_tmp778;goto _LL3BA;_LL3B9: if(*((
int*)_tmp754)!= 37)goto _LL3BB;_tmp779=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp754)->f2;_LL3BA: _tmp77A=_tmp779;goto _LL3BC;_LL3BB: if(*((int*)_tmp754)!= 28)
goto _LL3BD;_tmp77A=((struct Cyc_Absyn_Array_e_struct*)_tmp754)->f1;_LL3BC: for(0;
_tmp77A != 0;_tmp77A=_tmp77A->tl){struct Cyc_Absyn_Exp*_tmp783;struct _tuple6
_tmp782=*((struct _tuple6*)_tmp77A->hd);_tmp783=_tmp782.f2;Cyc_Toc_exptypes_to_c(
_tmp783);}goto _LL390;_LL3BD: if(*((int*)_tmp754)!= 20)goto _LL3BF;_tmp77B=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp754)->f1;_tmp77C=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp754)->f1);_LL3BE: _tmp77E=_tmp77C;goto
_LL3C0;_LL3BF: if(*((int*)_tmp754)!= 18)goto _LL3C1;_tmp77D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp754)->f1;_tmp77E=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp754)->f1);_LL3C0:*_tmp77E=Cyc_Toc_typ_to_c(*_tmp77E);goto _LL390;_LL3C1: if(*((
int*)_tmp754)!= 38)goto _LL3C3;_tmp77F=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp754)->f1;_LL3C2: Cyc_Toc_stmttypes_to_c(_tmp77F);goto _LL390;_LL3C3: if(*((int*)
_tmp754)!= 35)goto _LL3C5;_tmp780=((struct Cyc_Absyn_Malloc_e_struct*)_tmp754)->f1;
_tmp781=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp754)->f1;
_LL3C4: if(_tmp781->elt_type != 0)_tmp781->elt_type=({void**_tmp784=_cycalloc(
sizeof(*_tmp784));_tmp784[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp781->elt_type)));
_tmp784;});Cyc_Toc_exptypes_to_c(_tmp781->num_elts);goto _LL390;_LL3C5: if(*((int*)
_tmp754)!= 0)goto _LL3C7;_LL3C6: goto _LL3C8;_LL3C7: if(*((int*)_tmp754)!= 1)goto
_LL3C9;_LL3C8: goto _LL3CA;_LL3C9: if(*((int*)_tmp754)!= 2)goto _LL3CB;_LL3CA: goto
_LL3CC;_LL3CB: if(*((int*)_tmp754)!= 33)goto _LL3CD;_LL3CC: goto _LL3CE;_LL3CD: if(*((
int*)_tmp754)!= 34)goto _LL3CF;_LL3CE: goto _LL390;_LL3CF: if(*((int*)_tmp754)!= 31)
goto _LL3D1;_LL3D0: goto _LL3D2;_LL3D1: if(*((int*)_tmp754)!= 32)goto _LL3D3;_LL3D2:
goto _LL3D4;_LL3D3: if(*((int*)_tmp754)!= 30)goto _LL3D5;_LL3D4: goto _LL3D6;_LL3D5:
if(*((int*)_tmp754)!= 29)goto _LL3D7;_LL3D6: goto _LL3D8;_LL3D7: if(*((int*)_tmp754)
!= 26)goto _LL3D9;_LL3D8: goto _LL3DA;_LL3D9: if(*((int*)_tmp754)!= 14)goto _LL3DB;
_LL3DA: goto _LL3DC;_LL3DB: if(*((int*)_tmp754)!= 17)goto _LL3DD;_LL3DC: goto _LL3DE;
_LL3DD: if(*((int*)_tmp754)!= 21)goto _LL390;_LL3DE:({void*_tmp785[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp786="Cyclone expression within C code";_tag_dynforward(
_tmp786,sizeof(char),_get_zero_arr_size(_tmp786,33));}),_tag_dynforward(_tmp785,
sizeof(void*),0));});goto _LL390;_LL390:;}static void Cyc_Toc_decltypes_to_c(struct
Cyc_Absyn_Decl*d){void*_tmp787=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp788;
struct Cyc_Absyn_Fndecl*_tmp789;struct Cyc_Absyn_Aggrdecl*_tmp78A;struct Cyc_Absyn_Enumdecl*
_tmp78B;struct Cyc_Absyn_Typedefdecl*_tmp78C;_LL3E0: if(_tmp787 <= (void*)2)goto
_LL3F8;if(*((int*)_tmp787)!= 0)goto _LL3E2;_tmp788=((struct Cyc_Absyn_Var_d_struct*)
_tmp787)->f1;_LL3E1:(void*)(_tmp788->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp788->type));
if(_tmp788->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp788->initializer));goto _LL3DF;_LL3E2: if(*((int*)_tmp787)!= 1)
goto _LL3E4;_tmp789=((struct Cyc_Absyn_Fn_d_struct*)_tmp787)->f1;_LL3E3:(void*)(
_tmp789->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp789->ret_type));{struct Cyc_List_List*
_tmp78D=_tmp789->args;for(0;_tmp78D != 0;_tmp78D=_tmp78D->tl){(*((struct _tuple15*)
_tmp78D->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp78D->hd)).f3);}}goto
_LL3DF;_LL3E4: if(*((int*)_tmp787)!= 4)goto _LL3E6;_tmp78A=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp787)->f1;_LL3E5: Cyc_Toc_aggrdecl_to_c(_tmp78A);goto _LL3DF;_LL3E6: if(*((int*)
_tmp787)!= 6)goto _LL3E8;_tmp78B=((struct Cyc_Absyn_Enum_d_struct*)_tmp787)->f1;
_LL3E7: if(_tmp78B->fields != 0){struct Cyc_List_List*_tmp78E=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp78B->fields))->v;for(0;_tmp78E != 0;_tmp78E=
_tmp78E->tl){struct Cyc_Absyn_Enumfield*_tmp78F=(struct Cyc_Absyn_Enumfield*)
_tmp78E->hd;if(_tmp78F->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp78F->tag));}}goto _LL3DF;_LL3E8: if(*((int*)_tmp787)!= 7)goto
_LL3EA;_tmp78C=((struct Cyc_Absyn_Typedef_d_struct*)_tmp787)->f1;_LL3E9: _tmp78C->defn=({
struct Cyc_Core_Opt*_tmp790=_cycalloc(sizeof(*_tmp790));_tmp790->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp78C->defn))->v);_tmp790;});goto
_LL3DF;_LL3EA: if(*((int*)_tmp787)!= 2)goto _LL3EC;_LL3EB: goto _LL3ED;_LL3EC: if(*((
int*)_tmp787)!= 3)goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE: if(*((int*)_tmp787)!= 5)
goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(*((int*)_tmp787)!= 8)goto _LL3F2;_LL3F1:
goto _LL3F3;_LL3F2: if(*((int*)_tmp787)!= 9)goto _LL3F4;_LL3F3: goto _LL3F5;_LL3F4:
if(*((int*)_tmp787)!= 10)goto _LL3F6;_LL3F5: goto _LL3F7;_LL3F6: if(*((int*)_tmp787)
!= 11)goto _LL3F8;_LL3F7:({void*_tmp791[0]={};Cyc_Tcutil_terr(d->loc,({const char*
_tmp792="Cyclone declaration within C code";_tag_dynforward(_tmp792,sizeof(char),
_get_zero_arr_size(_tmp792,34));}),_tag_dynforward(_tmp791,sizeof(void*),0));});
goto _LL3DF;_LL3F8: if((int)_tmp787 != 0)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if((
int)_tmp787 != 1)goto _LL3DF;_LL3FB: goto _LL3DF;_LL3DF:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp793=(void*)s->r;struct Cyc_Absyn_Exp*_tmp794;
struct Cyc_Absyn_Stmt*_tmp795;struct Cyc_Absyn_Stmt*_tmp796;struct Cyc_Absyn_Exp*
_tmp797;struct Cyc_Absyn_Exp*_tmp798;struct Cyc_Absyn_Stmt*_tmp799;struct Cyc_Absyn_Stmt*
_tmp79A;struct _tuple3 _tmp79B;struct Cyc_Absyn_Exp*_tmp79C;struct Cyc_Absyn_Stmt*
_tmp79D;struct Cyc_Absyn_Exp*_tmp79E;struct _tuple3 _tmp79F;struct Cyc_Absyn_Exp*
_tmp7A0;struct _tuple3 _tmp7A1;struct Cyc_Absyn_Exp*_tmp7A2;struct Cyc_Absyn_Stmt*
_tmp7A3;struct Cyc_Absyn_Exp*_tmp7A4;struct Cyc_List_List*_tmp7A5;struct Cyc_Absyn_Decl*
_tmp7A6;struct Cyc_Absyn_Stmt*_tmp7A7;struct Cyc_Absyn_Stmt*_tmp7A8;struct _tuple3
_tmp7A9;struct Cyc_Absyn_Exp*_tmp7AA;_LL3FD: if(_tmp793 <= (void*)1)goto _LL40F;if(*((
int*)_tmp793)!= 0)goto _LL3FF;_tmp794=((struct Cyc_Absyn_Exp_s_struct*)_tmp793)->f1;
_LL3FE: Cyc_Toc_exptypes_to_c(_tmp794);goto _LL3FC;_LL3FF: if(*((int*)_tmp793)!= 1)
goto _LL401;_tmp795=((struct Cyc_Absyn_Seq_s_struct*)_tmp793)->f1;_tmp796=((struct
Cyc_Absyn_Seq_s_struct*)_tmp793)->f2;_LL400: Cyc_Toc_stmttypes_to_c(_tmp795);Cyc_Toc_stmttypes_to_c(
_tmp796);goto _LL3FC;_LL401: if(*((int*)_tmp793)!= 2)goto _LL403;_tmp797=((struct
Cyc_Absyn_Return_s_struct*)_tmp793)->f1;_LL402: if(_tmp797 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp797);goto _LL3FC;_LL403: if(*((int*)_tmp793)!= 3)goto
_LL405;_tmp798=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp793)->f1;_tmp799=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp793)->f2;_tmp79A=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp793)->f3;_LL404: Cyc_Toc_exptypes_to_c(_tmp798);Cyc_Toc_stmttypes_to_c(
_tmp799);Cyc_Toc_stmttypes_to_c(_tmp79A);goto _LL3FC;_LL405: if(*((int*)_tmp793)!= 
4)goto _LL407;_tmp79B=((struct Cyc_Absyn_While_s_struct*)_tmp793)->f1;_tmp79C=
_tmp79B.f1;_tmp79D=((struct Cyc_Absyn_While_s_struct*)_tmp793)->f2;_LL406: Cyc_Toc_exptypes_to_c(
_tmp79C);Cyc_Toc_stmttypes_to_c(_tmp79D);goto _LL3FC;_LL407: if(*((int*)_tmp793)!= 
8)goto _LL409;_tmp79E=((struct Cyc_Absyn_For_s_struct*)_tmp793)->f1;_tmp79F=((
struct Cyc_Absyn_For_s_struct*)_tmp793)->f2;_tmp7A0=_tmp79F.f1;_tmp7A1=((struct
Cyc_Absyn_For_s_struct*)_tmp793)->f3;_tmp7A2=_tmp7A1.f1;_tmp7A3=((struct Cyc_Absyn_For_s_struct*)
_tmp793)->f4;_LL408: Cyc_Toc_exptypes_to_c(_tmp79E);Cyc_Toc_exptypes_to_c(_tmp7A0);
Cyc_Toc_exptypes_to_c(_tmp7A2);Cyc_Toc_stmttypes_to_c(_tmp7A3);goto _LL3FC;_LL409:
if(*((int*)_tmp793)!= 9)goto _LL40B;_tmp7A4=((struct Cyc_Absyn_Switch_s_struct*)
_tmp793)->f1;_tmp7A5=((struct Cyc_Absyn_Switch_s_struct*)_tmp793)->f2;_LL40A: Cyc_Toc_exptypes_to_c(
_tmp7A4);for(0;_tmp7A5 != 0;_tmp7A5=_tmp7A5->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp7A5->hd)->body);}goto _LL3FC;_LL40B: if(*((int*)
_tmp793)!= 11)goto _LL40D;_tmp7A6=((struct Cyc_Absyn_Decl_s_struct*)_tmp793)->f1;
_tmp7A7=((struct Cyc_Absyn_Decl_s_struct*)_tmp793)->f2;_LL40C: Cyc_Toc_decltypes_to_c(
_tmp7A6);Cyc_Toc_stmttypes_to_c(_tmp7A7);goto _LL3FC;_LL40D: if(*((int*)_tmp793)!= 
13)goto _LL40F;_tmp7A8=((struct Cyc_Absyn_Do_s_struct*)_tmp793)->f1;_tmp7A9=((
struct Cyc_Absyn_Do_s_struct*)_tmp793)->f2;_tmp7AA=_tmp7A9.f1;_LL40E: Cyc_Toc_stmttypes_to_c(
_tmp7A8);Cyc_Toc_exptypes_to_c(_tmp7AA);goto _LL3FC;_LL40F: if((int)_tmp793 != 0)
goto _LL411;_LL410: goto _LL412;_LL411: if(_tmp793 <= (void*)1)goto _LL413;if(*((int*)
_tmp793)!= 5)goto _LL413;_LL412: goto _LL414;_LL413: if(_tmp793 <= (void*)1)goto
_LL415;if(*((int*)_tmp793)!= 6)goto _LL415;_LL414: goto _LL416;_LL415: if(_tmp793 <= (
void*)1)goto _LL417;if(*((int*)_tmp793)!= 7)goto _LL417;_LL416: goto _LL3FC;_LL417:
if(_tmp793 <= (void*)1)goto _LL419;if(*((int*)_tmp793)!= 10)goto _LL419;_LL418: goto
_LL41A;_LL419: if(_tmp793 <= (void*)1)goto _LL41B;if(*((int*)_tmp793)!= 12)goto
_LL41B;_LL41A: goto _LL41C;_LL41B: if(_tmp793 <= (void*)1)goto _LL41D;if(*((int*)
_tmp793)!= 14)goto _LL41D;_LL41C: goto _LL41E;_LL41D: if(_tmp793 <= (void*)1)goto
_LL41F;if(*((int*)_tmp793)!= 15)goto _LL41F;_LL41E: goto _LL420;_LL41F: if(_tmp793 <= (
void*)1)goto _LL421;if(*((int*)_tmp793)!= 17)goto _LL421;_LL420: goto _LL422;_LL421:
if(_tmp793 <= (void*)1)goto _LL3FC;if(*((int*)_tmp793)!= 16)goto _LL3FC;_LL422:({
void*_tmp7AB[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp7AC="Cyclone statement in C code";
_tag_dynforward(_tmp7AC,sizeof(char),_get_zero_arr_size(_tmp7AC,28));}),
_tag_dynforward(_tmp7AB,sizeof(void*),0));});goto _LL3FC;_LL3FC:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({
void*_tmp7AD[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp7AE="decls_to_c: not at toplevel!";
_tag_dynforward(_tmp7AE,sizeof(char),_get_zero_arr_size(_tmp7AE,29));}),
_tag_dynforward(_tmp7AD,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp7AF=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp7B0;struct Cyc_Absyn_Fndecl*
_tmp7B1;struct Cyc_Absyn_Aggrdecl*_tmp7B2;struct Cyc_Absyn_Tuniondecl*_tmp7B3;
struct Cyc_Absyn_Enumdecl*_tmp7B4;struct Cyc_Absyn_Typedefdecl*_tmp7B5;struct Cyc_List_List*
_tmp7B6;struct Cyc_List_List*_tmp7B7;struct Cyc_List_List*_tmp7B8;struct Cyc_List_List*
_tmp7B9;_LL424: if(_tmp7AF <= (void*)2)goto _LL434;if(*((int*)_tmp7AF)!= 0)goto
_LL426;_tmp7B0=((struct Cyc_Absyn_Var_d_struct*)_tmp7AF)->f1;_LL425: {struct
_tuple1*_tmp7BA=_tmp7B0->name;if((void*)_tmp7B0->sc == (void*)4)_tmp7BA=({struct
_tuple1*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7BC;(_tmp7BC.Rel_n).tag=1;(_tmp7BC.Rel_n).f1=0;
_tmp7BC;});_tmp7BB->f2=(*_tmp7BA).f2;_tmp7BB;});if(_tmp7B0->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp7B0->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp7B0->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp7B0->name,Cyc_Absyn_varb_exp(_tmp7BA,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD[0]=({
struct Cyc_Absyn_Global_b_struct _tmp7BE;_tmp7BE.tag=0;_tmp7BE.f1=_tmp7B0;_tmp7BE;});
_tmp7BD;}),0));_tmp7B0->name=_tmp7BA;(void*)(_tmp7B0->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp7B0->sc));(void*)(_tmp7B0->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp7B0->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7BF=_cycalloc(
sizeof(*_tmp7BF));_tmp7BF->hd=d;_tmp7BF->tl=Cyc_Toc_result_decls;_tmp7BF;});goto
_LL423;}_LL426: if(*((int*)_tmp7AF)!= 1)goto _LL428;_tmp7B1=((struct Cyc_Absyn_Fn_d_struct*)
_tmp7AF)->f1;_LL427: {struct _tuple1*_tmp7C0=_tmp7B1->name;if((void*)_tmp7B1->sc
== (void*)4){_tmp7C0=({struct _tuple1*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1->f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7C2;(_tmp7C2.Rel_n).tag=
1;(_tmp7C2.Rel_n).f1=0;_tmp7C2;});_tmp7C1->f2=(*_tmp7C0).f2;_tmp7C1;});(void*)(
_tmp7B1->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,nv,_tmp7B1->name,Cyc_Absyn_var_exp(
_tmp7C0,0));_tmp7B1->name=_tmp7C0;Cyc_Toc_fndecl_to_c(nv,_tmp7B1,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3->hd=d;_tmp7C3->tl=
Cyc_Toc_result_decls;_tmp7C3;});goto _LL423;}_LL428: if(*((int*)_tmp7AF)!= 2)goto
_LL42A;_LL429: goto _LL42B;_LL42A: if(*((int*)_tmp7AF)!= 3)goto _LL42C;_LL42B:({void*
_tmp7C4[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp7C5="letdecl at toplevel";_tag_dynforward(_tmp7C5,sizeof(char),
_get_zero_arr_size(_tmp7C5,20));}),_tag_dynforward(_tmp7C4,sizeof(void*),0));});
_LL42C: if(*((int*)_tmp7AF)!= 4)goto _LL42E;_tmp7B2=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp7AF)->f1;_LL42D: Cyc_Toc_aggrdecl_to_c(_tmp7B2);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6->hd=d;_tmp7C6->tl=Cyc_Toc_result_decls;
_tmp7C6;});goto _LL423;_LL42E: if(*((int*)_tmp7AF)!= 5)goto _LL430;_tmp7B3=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp7AF)->f1;_LL42F: if(_tmp7B3->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp7B3);else{Cyc_Toc_tuniondecl_to_c(_tmp7B3);}goto _LL423;_LL430: if(*((int*)
_tmp7AF)!= 6)goto _LL432;_tmp7B4=((struct Cyc_Absyn_Enum_d_struct*)_tmp7AF)->f1;
_LL431: Cyc_Toc_enumdecl_to_c(nv,_tmp7B4);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp7C7=_cycalloc(sizeof(*_tmp7C7));_tmp7C7->hd=d;_tmp7C7->tl=Cyc_Toc_result_decls;
_tmp7C7;});goto _LL423;_LL432: if(*((int*)_tmp7AF)!= 7)goto _LL434;_tmp7B5=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp7AF)->f1;_LL433: _tmp7B5->name=_tmp7B5->name;
_tmp7B5->tvs=0;if(_tmp7B5->defn != 0)_tmp7B5->defn=({struct Cyc_Core_Opt*_tmp7C8=
_cycalloc(sizeof(*_tmp7C8));_tmp7C8->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp7B5->defn))->v);_tmp7C8;});else{void*_tmp7C9=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7B5->kind))->v;_LL441: if((int)
_tmp7C9 != 2)goto _LL443;_LL442: _tmp7B5->defn=({struct Cyc_Core_Opt*_tmp7CA=
_cycalloc(sizeof(*_tmp7CA));_tmp7CA->v=(void*)Cyc_Absyn_void_star_typ();_tmp7CA;});
goto _LL440;_LL443:;_LL444: _tmp7B5->defn=({struct Cyc_Core_Opt*_tmp7CB=_cycalloc(
sizeof(*_tmp7CB));_tmp7CB->v=(void*)((void*)0);_tmp7CB;});goto _LL440;_LL440:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7CC=_cycalloc(sizeof(*_tmp7CC));
_tmp7CC->hd=d;_tmp7CC->tl=Cyc_Toc_result_decls;_tmp7CC;});goto _LL423;_LL434: if((
int)_tmp7AF != 0)goto _LL436;_LL435: goto _LL437;_LL436: if((int)_tmp7AF != 1)goto
_LL438;_LL437: goto _LL423;_LL438: if(_tmp7AF <= (void*)2)goto _LL43A;if(*((int*)
_tmp7AF)!= 8)goto _LL43A;_tmp7B6=((struct Cyc_Absyn_Namespace_d_struct*)_tmp7AF)->f2;
_LL439: _tmp7B7=_tmp7B6;goto _LL43B;_LL43A: if(_tmp7AF <= (void*)2)goto _LL43C;if(*((
int*)_tmp7AF)!= 9)goto _LL43C;_tmp7B7=((struct Cyc_Absyn_Using_d_struct*)_tmp7AF)->f2;
_LL43B: _tmp7B8=_tmp7B7;goto _LL43D;_LL43C: if(_tmp7AF <= (void*)2)goto _LL43E;if(*((
int*)_tmp7AF)!= 10)goto _LL43E;_tmp7B8=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp7AF)->f1;_LL43D: nv=Cyc_Toc_decls_to_c(r,nv,_tmp7B8,top,cinclude);goto _LL423;
_LL43E: if(_tmp7AF <= (void*)2)goto _LL423;if(*((int*)_tmp7AF)!= 11)goto _LL423;
_tmp7B9=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp7AF)->f1;_LL43F: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp7B9,top,1);goto _LL423;_LL423:;}}return nv;}static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp7CE;struct Cyc_Core_NewRegion _tmp7CD=Cyc_Core_new_dynregion();
_tmp7CE=_tmp7CD.dynregion;{struct _DynRegionFrame _tmp7CF;struct _RegionHandle*d=
_open_dynregion(& _tmp7CF,_tmp7CE);Cyc_Toc_toc_state=({struct Cyc_Toc_TocState*
_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0->dyn=(struct _DynRegionHandle*)
_tmp7CE;_tmp7D0->tuple_types=(struct Cyc_List_List**)({struct Cyc_List_List**
_tmp7D4=_region_malloc(d,sizeof(*_tmp7D4));_tmp7D4[0]=0;_tmp7D4;});_tmp7D0->aggrs_so_far=(
struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*_tmp7D3=_region_malloc(d,sizeof(*
_tmp7D3));_tmp7D3[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmp7D3;});
_tmp7D0->tunions_so_far=(struct Cyc_Set_Set**)({struct Cyc_Set_Set**_tmp7D2=
_region_malloc(d,sizeof(*_tmp7D2));_tmp7D2[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);
_tmp7D2;});_tmp7D0->xtunions_so_far=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*
_tmp7D1=_region_malloc(d,sizeof(*_tmp7D1));_tmp7D1[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp);_tmp7D1;});_tmp7D0;});;_pop_dynregion(d);}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;Cyc_Toc_globals=
_tag_dynforward(({struct _dynforward_ptr**_tmp7D5=_cycalloc(sizeof(struct
_dynforward_ptr*)* 50);_tmp7D5[0]=& Cyc_Toc__throw_str;_tmp7D5[1]=& Cyc_Toc_setjmp_str;
_tmp7D5[2]=& Cyc_Toc__push_handler_str;_tmp7D5[3]=& Cyc_Toc__pop_handler_str;
_tmp7D5[4]=& Cyc_Toc__exn_thrown_str;_tmp7D5[5]=& Cyc_Toc__npop_handler_str;
_tmp7D5[6]=& Cyc_Toc__check_null_str;_tmp7D5[7]=& Cyc_Toc__check_known_subscript_null_str;
_tmp7D5[8]=& Cyc_Toc__check_known_subscript_notnull_str;_tmp7D5[9]=& Cyc_Toc__check_dynforward_subscript_str;
_tmp7D5[10]=& Cyc_Toc__check_dyneither_subscript_str;_tmp7D5[11]=& Cyc_Toc__dynforward_ptr_str;
_tmp7D5[12]=& Cyc_Toc__dyneither_ptr_str;_tmp7D5[13]=& Cyc_Toc__tag_dynforward_str;
_tmp7D5[14]=& Cyc_Toc__tag_dyneither_str;_tmp7D5[15]=& Cyc_Toc__init_dynforward_ptr_str;
_tmp7D5[16]=& Cyc_Toc__init_dyneither_ptr_str;_tmp7D5[17]=& Cyc_Toc__untag_dynforward_ptr_str;
_tmp7D5[18]=& Cyc_Toc__untag_dyneither_ptr_str;_tmp7D5[19]=& Cyc_Toc__get_dynforward_size_str;
_tmp7D5[20]=& Cyc_Toc__get_dyneither_size_str;_tmp7D5[21]=& Cyc_Toc__get_zero_arr_size_str;
_tmp7D5[22]=& Cyc_Toc__dynforward_ptr_plus_str;_tmp7D5[23]=& Cyc_Toc__dyneither_ptr_plus_str;
_tmp7D5[24]=& Cyc_Toc__zero_arr_plus_str;_tmp7D5[25]=& Cyc_Toc__dynforward_ptr_inplace_plus_str;
_tmp7D5[26]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp7D5[27]=& Cyc_Toc__zero_arr_inplace_plus_str;
_tmp7D5[28]=& Cyc_Toc__dynforward_ptr_inplace_plus_post_str;_tmp7D5[29]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;
_tmp7D5[30]=& Cyc_Toc__dynforward_to_dyneither_str;_tmp7D5[31]=& Cyc_Toc__dyneither_to_dynforward_str;
_tmp7D5[32]=& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp7D5[33]=& Cyc_Toc__cycalloc_str;
_tmp7D5[34]=& Cyc_Toc__cyccalloc_str;_tmp7D5[35]=& Cyc_Toc__cycalloc_atomic_str;
_tmp7D5[36]=& Cyc_Toc__cyccalloc_atomic_str;_tmp7D5[37]=& Cyc_Toc__region_malloc_str;
_tmp7D5[38]=& Cyc_Toc__region_calloc_str;_tmp7D5[39]=& Cyc_Toc__check_times_str;
_tmp7D5[40]=& Cyc_Toc__new_region_str;_tmp7D5[41]=& Cyc_Toc__push_region_str;
_tmp7D5[42]=& Cyc_Toc__pop_region_str;_tmp7D5[43]=& Cyc_Toc__open_dynregion_str;
_tmp7D5[44]=& Cyc_Toc__push_dynregion_str;_tmp7D5[45]=& Cyc_Toc__pop_dynregion_str;
_tmp7D5[46]=& Cyc_Toc__reset_region_str;_tmp7D5[47]=& Cyc_Toc__throw_arraybounds_str;
_tmp7D5[48]=& Cyc_Toc__dynforward_ptr_decrease_size_str;_tmp7D5[49]=& Cyc_Toc__dyneither_ptr_decrease_size_str;
_tmp7D5;}),sizeof(struct _dynforward_ptr*),50);}struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmp7D6=_new_region("start");
struct _RegionHandle*start=& _tmp7D6;_push_region(start);Cyc_Toc_decls_to_c(start,
Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmp7D8;struct Cyc_Toc_TocState
_tmp7D7=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7D8=
_tmp7D7.dyn;Cyc_Core_free_dynregion(_tmp7D8);};_pop_region(start);}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
