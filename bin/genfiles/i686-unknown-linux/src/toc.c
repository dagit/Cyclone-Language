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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
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
Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int
Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rempty(
struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(
struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
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
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*
x);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
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
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct
Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;
struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*
Cyc_Tcutil_snd_tqt(struct _tuple4*);struct _tuple5{unsigned int f1;int f2;};struct
_tuple5 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
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
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
int Cyc_Toc_warn_bounds_checks=0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=
0;unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;static struct Cyc_List_List*Cyc_Toc_tuple_types=0;static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\n";_tag_dynforward(
_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,2));}),_tag_dynforward(_tmp0,sizeof(
void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)
Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*_tmp2[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp3="\n";_tag_dynforward(_tmp3,sizeof(char),
_get_zero_arr_size(_tmp3,2));}),_tag_dynforward(_tmp2,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";static char _tmp4[5]="curr";
static struct _dynforward_ptr Cyc_Toc_curr_string={_tmp4,_tmp4 + 5};static struct
_dynforward_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp5[4]="tag";
static struct _dynforward_ptr Cyc_Toc_tag_string={_tmp5,_tmp5 + 4};static struct
_dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp6[4]="val";
static struct _dynforward_ptr Cyc_Toc_val_string={_tmp6,_tmp6 + 4};static struct
_dynforward_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp7[14]="_handler_cons";
static struct _dynforward_ptr Cyc_Toc__handler_cons_string={_tmp7,_tmp7 + 14};static
struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp8[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp8,_tmp8 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp9[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp9,_tmp9 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmpA[17]="_DynRegionHandle";static struct _dynforward_ptr Cyc_Toc__DynRegionHandle_string={
_tmpA,_tmpA + 17};static struct _dynforward_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmpB[16]="_DynRegionFrame";static struct _dynforward_ptr Cyc_Toc__DynRegionFrame_string={
_tmpB,_tmpB + 16};static struct _dynforward_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
static char _tmpC[7]="_throw";static struct _dynforward_ptr Cyc_Toc__throw_str={_tmpC,
_tmpC + 7};static struct _tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__throw_ev={0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
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
Cyc_Toc__dyneither_ptr_decrease_size_ev;static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={
10,{(union Cyc_Absyn_AggrInfoU_union)((struct Cyc_Absyn_UnknownAggr_struct){0,(
void*)((void*)0),& Cyc_Toc__dynforward_ptr_pr}),0}};static void*Cyc_Toc_dynforward_ptr_typ=(
void*)& Cyc_Toc_dynforward_ptr_typ_v;static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={
10,{(union Cyc_Absyn_AggrInfoU_union)((struct Cyc_Absyn_UnknownAggr_struct){0,(
void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),0}};static void*Cyc_Toc_dyneither_ptr_typ=(
void*)& Cyc_Toc_dyneither_ptr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**
skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=Cyc_Absyn_skip_stmt(0);_tmpA6;});
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*
t,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmpA7=
_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_Cast_e_struct _tmpA8;
_tmpA8.tag=15;_tmpA8.f1=(void*)t;_tmpA8.f2=e;_tmpA8.f3=0;_tmpA8.f4=(void*)((void*)
1);_tmpA8;});_tmpA7;});}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));
_tmpA9[0]=({struct Cyc_Absyn_Deref_e_struct _tmpAA;_tmpAA.tag=22;_tmpAA.f1=e;
_tmpAA;});_tmpA9;});}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpAC;
_tmpAC.tag=25;_tmpAC.f1=e1;_tmpAC.f2=e2;_tmpAC;});_tmpAB;});}static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpAD=
_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpAE;
_tmpAE.tag=37;_tmpAE.f1=s;_tmpAE;});_tmpAD;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpAF=_cycalloc(
sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpB0;_tmpB0.tag=
18;_tmpB0.f1=(void*)t;_tmpB0;});_tmpAF;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpB2;_tmpB2.tag=11;_tmpB2.f1=e;_tmpB2.f2=es;_tmpB2.f3=0;_tmpB2;});_tmpB1;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_Exp_s_struct _tmpB4;
_tmpB4.tag=0;_tmpB4.f1=e;_tmpB4;});_tmpB3;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_Seq_s_struct _tmpB6;
_tmpB6.tag=1;_tmpB6.f1=s1;_tmpB6.f2=s2;_tmpB6;});_tmpB5;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));
_tmpB7[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpB8;_tmpB8.tag=6;_tmpB8.f1=e1;
_tmpB8.f2=e2;_tmpB8.f3=e3;_tmpB8;});_tmpB7;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpBA;_tmpBA.tag=23;_tmpBA.f1=e;_tmpBA.f2=n;_tmpBA;});_tmpB9;});}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*
_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_AggrArrow_e_struct
_tmpBC;_tmpBC.tag=24;_tmpBC.f1=e;_tmpBC.f2=n;_tmpBC;});_tmpBB;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpBE;_tmpBE.tag=36;_tmpBE.f1=tdopt;_tmpBE.f2=ds;_tmpBE;});_tmpBD;});}static
void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){return(
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpC0;_tmpC0.tag=7;_tmpC0.f1=v;_tmpC0.f2=s;_tmpC0;});
_tmpBF;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),0})};struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpC3=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpC4;char _tmpC5;union Cyc_Absyn_Cnst_union
_tmpC6;short _tmpC7;union Cyc_Absyn_Cnst_union _tmpC8;int _tmpC9;union Cyc_Absyn_Cnst_union
_tmpCA;long long _tmpCB;union Cyc_Absyn_Cnst_union _tmpCC;struct Cyc_Absyn_Exp*
_tmpCD;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*
_tmpD0;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*_tmpD2;_LL1: if(*((int*)
_tmpC3)!= 0)goto _LL3;_tmpC4=((struct Cyc_Absyn_Const_e_struct*)_tmpC3)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpC3)->f1).Char_c).tag != 0)goto _LL3;_tmpC5=(
_tmpC4.Char_c).f2;_LL2: return _tmpC5 == '\000';_LL3: if(*((int*)_tmpC3)!= 0)goto
_LL5;_tmpC6=((struct Cyc_Absyn_Const_e_struct*)_tmpC3)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpC3)->f1).Short_c).tag != 1)goto _LL5;_tmpC7=(_tmpC6.Short_c).f2;_LL4: return
_tmpC7 == 0;_LL5: if(*((int*)_tmpC3)!= 0)goto _LL7;_tmpC8=((struct Cyc_Absyn_Const_e_struct*)
_tmpC3)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpC3)->f1).Int_c).tag != 2)
goto _LL7;_tmpC9=(_tmpC8.Int_c).f2;_LL6: return _tmpC9 == 0;_LL7: if(*((int*)_tmpC3)
!= 0)goto _LL9;_tmpCA=((struct Cyc_Absyn_Const_e_struct*)_tmpC3)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmpC3)->f1).LongLong_c).tag != 3)goto _LL9;_tmpCB=(
_tmpCA.LongLong_c).f2;_LL8: return _tmpCB == 0;_LL9: if(*((int*)_tmpC3)!= 0)goto _LLB;
_tmpCC=((struct Cyc_Absyn_Const_e_struct*)_tmpC3)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpC3)->f1).Null_c).tag != 6)goto _LLB;_LLA: return 1;_LLB: if(*((int*)_tmpC3)!= 15)
goto _LLD;_tmpCD=((struct Cyc_Absyn_Cast_e_struct*)_tmpC3)->f2;_LLC: return Cyc_Toc_is_zero(
_tmpCD);_LLD: if(*((int*)_tmpC3)!= 26)goto _LLF;_tmpCE=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpC3)->f1;_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpCE);_LLF: if(*((int*)_tmpC3)!= 28)goto
_LL11;_tmpCF=((struct Cyc_Absyn_Array_e_struct*)_tmpC3)->f1;_LL10: _tmpD0=_tmpCF;
goto _LL12;_LL11: if(*((int*)_tmpC3)!= 30)goto _LL13;_tmpD0=((struct Cyc_Absyn_Struct_e_struct*)
_tmpC3)->f3;_LL12: _tmpD1=_tmpD0;goto _LL14;_LL13: if(*((int*)_tmpC3)!= 27)goto
_LL15;_tmpD1=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpC3)->f2;_LL14: _tmpD2=
_tmpD1;goto _LL16;_LL15: if(*((int*)_tmpC3)!= 36)goto _LL17;_tmpD2=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpC3)->f2;_LL16: for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple6*)_tmpD2->hd)).f2))return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpD3=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpD4;struct Cyc_Absyn_PtrAtts _tmpD5;struct Cyc_Absyn_Conref*
_tmpD6;_LL1A: if(_tmpD3 <= (void*)4)goto _LL1C;if(*((int*)_tmpD3)!= 4)goto _LL1C;
_tmpD4=((struct Cyc_Absyn_PointerType_struct*)_tmpD3)->f1;_tmpD5=_tmpD4.ptr_atts;
_tmpD6=_tmpD5.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpD6);_LL1C:;_LL1D:({void*_tmpD7[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpD8="is_nullable";
_tag_dynforward(_tmpD8,sizeof(char),_get_zero_arr_size(_tmpD8,12));}),
_tag_dynforward(_tmpD7,sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dynforward_ptr tag){return({struct _tuple1*_tmpD9=_cycalloc(
sizeof(*_tmpD9));_tmpD9->f1=(*x).f1;_tmpD9->f2=({struct _dynforward_ptr*_tmpDA=
_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)*(*x).f2,(struct _dynforward_ptr)tag);_tmpDA;});_tmpD9;});}
struct _tuple7{struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};static struct
_dynforward_ptr*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{struct Cyc_List_List*
_tmpDB=Cyc_Toc_tuple_types;for(0;_tmpDB != 0;_tmpDB=_tmpDB->tl){struct _tuple7
_tmpDD;struct _dynforward_ptr*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple7*
_tmpDC=(struct _tuple7*)_tmpDB->hd;_tmpDD=*_tmpDC;_tmpDE=_tmpDD.f1;_tmpDF=_tmpDD.f2;{
struct Cyc_List_List*_tmpE0=tqs0;for(0;_tmpE0 != 0  && _tmpDF != 0;(_tmpE0=_tmpE0->tl,
_tmpDF=_tmpDF->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmpE0->hd)).f2,(
void*)_tmpDF->hd))break;}if(_tmpE0 == 0  && _tmpDF == 0)return _tmpDE;}}}{struct
_dynforward_ptr*x=({struct _dynforward_ptr*_tmpEE=_cycalloc(sizeof(*_tmpEE));
_tmpEE[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpF1;_tmpF1.tag=1;
_tmpF1.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmpEF[1]={& _tmpF1};
Cyc_aprintf(({const char*_tmpF0="_tuple%d";_tag_dynforward(_tmpF0,sizeof(char),
_get_zero_arr_size(_tmpF0,9));}),_tag_dynforward(_tmpEF,sizeof(void*),1));}});
_tmpEE;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmpE1=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmpE1=({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->hd=({
struct Cyc_Absyn_Aggrfield*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->name=Cyc_Absyn_fieldname(
i);_tmpE3->tq=Cyc_Toc_mt_tq;_tmpE3->type=(void*)((void*)ts2->hd);_tmpE3->width=0;
_tmpE3->attributes=0;_tmpE3;});_tmpE2->tl=_tmpE1;_tmpE2;});}}_tmpE1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE1);{struct Cyc_Absyn_Aggrdecl*
_tmpE4=({struct Cyc_Absyn_Aggrdecl*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->kind=(
void*)((void*)0);_tmpEA->sc=(void*)((void*)2);_tmpEA->name=({struct _tuple1*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmpED;(_tmpED.Rel_n).tag=1;(_tmpED.Rel_n).f1=0;
_tmpED;});_tmpEC->f2=x;_tmpEC;});_tmpEA->tvs=0;_tmpEA->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->exist_vars=0;_tmpEB->rgn_po=0;_tmpEB->fields=
_tmpE1;_tmpEB;});_tmpEA->attributes=0;_tmpEA;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmpE7;_tmpE7.tag=4;_tmpE7.f1=_tmpE4;_tmpE7;});_tmpE6;}),0);_tmpE5->tl=Cyc_Toc_result_decls;
_tmpE5;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*
_tmpE8));_tmpE8->hd=({struct _tuple7*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->f1=
x;_tmpE9->f2=ts;_tmpE9;});_tmpE8->tl=Cyc_Toc_tuple_types;_tmpE8;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmpF2=_cycalloc(sizeof(*
_tmpF2));_tmpF2->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmpF7;(_tmpF7.Loc_n).tag=0;_tmpF7;});_tmpF2->f2=({struct _dynforward_ptr*_tmpF3=
_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmpF6;_tmpF6.tag=1;_tmpF6.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmpF4[1]={& _tmpF6};Cyc_aprintf(({const char*_tmpF5="_tmp%X";_tag_dynforward(
_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,7));}),_tag_dynforward(_tmpF4,
sizeof(void*),1));}});_tmpF3;});_tmpF2;});}static struct _dynforward_ptr*Cyc_Toc_fresh_label(){
return({struct _dynforward_ptr*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpFB;_tmpFB.tag=1;_tmpFB.f1=(
unsigned int)Cyc_Toc_fresh_label_counter ++;{void*_tmpF9[1]={& _tmpFB};Cyc_aprintf(({
const char*_tmpFA="_LL%X";_tag_dynforward(_tmpFA,sizeof(char),_get_zero_arr_size(
_tmpFA,6));}),_tag_dynforward(_tmpF9,sizeof(void*),1));}});_tmpF8;});}static
struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,struct
_tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmpFC=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmpFC))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpFC->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpFC->hd)->typs == 0)
++ ans;_tmpFC=_tmpFC->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmpFD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmpFD->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmpFF;struct Cyc_Absyn_Tqual
_tmp100;void*_tmp101;struct _tuple2 _tmpFE=*a;_tmpFF=_tmpFE.f1;_tmp100=_tmpFE.f2;
_tmp101=_tmpFE.f3;return({struct _tuple2*_tmp102=_cycalloc(sizeof(*_tmp102));
_tmp102->f1=_tmpFF;_tmp102->f2=_tmp100;_tmp102->f3=Cyc_Toc_typ_to_c(_tmp101);
_tmp102;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual
_tmp104;void*_tmp105;struct _tuple4 _tmp103=*x;_tmp104=_tmp103.f1;_tmp105=_tmp103.f2;
return({struct _tuple4*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->f1=_tmp104;
_tmp106->f2=Cyc_Toc_typ_to_c(_tmp105);_tmp106;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp107=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp108;
void*_tmp109;struct Cyc_Absyn_Tqual _tmp10A;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Conref*
_tmp10C;struct Cyc_Position_Segment*_tmp10D;struct Cyc_Core_Opt*_tmp10E;struct Cyc_Core_Opt
_tmp10F;void*_tmp110;_LL1F: if(_tmp107 <= (void*)4)goto _LL23;if(*((int*)_tmp107)!= 
7)goto _LL21;_tmp108=((struct Cyc_Absyn_ArrayType_struct*)_tmp107)->f1;_tmp109=(
void*)_tmp108.elt_type;_tmp10A=_tmp108.tq;_tmp10B=_tmp108.num_elts;_tmp10C=
_tmp108.zero_term;_tmp10D=_tmp108.zt_loc;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp109),_tmp10A,_tmp10B,Cyc_Absyn_false_conref,_tmp10D);_LL21: if(*((int*)
_tmp107)!= 0)goto _LL23;_tmp10E=((struct Cyc_Absyn_Evar_struct*)_tmp107)->f2;if(
_tmp10E == 0)goto _LL23;_tmp10F=*_tmp10E;_tmp110=(void*)_tmp10F.v;_LL22: return Cyc_Toc_typ_to_c_array(
_tmp110);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->name=f->name;_tmp111->tq=Cyc_Toc_mt_tq;
_tmp111->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp111->width=f->width;
_tmp111->attributes=f->attributes;_tmp111;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp112=
t;struct Cyc_Core_Opt*_tmp113;struct Cyc_Core_Opt*_tmp114;struct Cyc_Core_Opt
_tmp115;void*_tmp116;struct Cyc_Absyn_Tvar*_tmp117;struct Cyc_Absyn_TunionInfo
_tmp118;union Cyc_Absyn_TunionInfoU_union _tmp119;struct Cyc_Absyn_Tuniondecl**
_tmp11A;struct Cyc_Absyn_Tuniondecl*_tmp11B;struct Cyc_Absyn_TunionFieldInfo
_tmp11C;union Cyc_Absyn_TunionFieldInfoU_union _tmp11D;struct Cyc_Absyn_Tuniondecl*
_tmp11E;struct Cyc_Absyn_Tunionfield*_tmp11F;struct Cyc_Absyn_PtrInfo _tmp120;void*
_tmp121;struct Cyc_Absyn_Tqual _tmp122;struct Cyc_Absyn_PtrAtts _tmp123;struct Cyc_Absyn_Conref*
_tmp124;struct Cyc_Absyn_ArrayInfo _tmp125;void*_tmp126;struct Cyc_Absyn_Tqual
_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Position_Segment*_tmp129;struct Cyc_Absyn_FnInfo
_tmp12A;void*_tmp12B;struct Cyc_List_List*_tmp12C;int _tmp12D;struct Cyc_Absyn_VarargInfo*
_tmp12E;struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp130;void*_tmp131;
struct Cyc_List_List*_tmp132;struct Cyc_Absyn_AggrInfo _tmp133;union Cyc_Absyn_AggrInfoU_union
_tmp134;struct Cyc_List_List*_tmp135;struct _tuple1*_tmp136;struct Cyc_List_List*
_tmp137;struct _tuple1*_tmp138;struct Cyc_List_List*_tmp139;struct Cyc_Absyn_Typedefdecl*
_tmp13A;void**_tmp13B;void*_tmp13C;_LL26: if((int)_tmp112 != 0)goto _LL28;_LL27:
return t;_LL28: if(_tmp112 <= (void*)4)goto _LL3A;if(*((int*)_tmp112)!= 0)goto _LL2A;
_tmp113=((struct Cyc_Absyn_Evar_struct*)_tmp112)->f2;if(_tmp113 != 0)goto _LL2A;
_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(*((int*)_tmp112)!= 0)goto _LL2C;_tmp114=((
struct Cyc_Absyn_Evar_struct*)_tmp112)->f2;if(_tmp114 == 0)goto _LL2C;_tmp115=*
_tmp114;_tmp116=(void*)_tmp115.v;_LL2B: return Cyc_Toc_typ_to_c(_tmp116);_LL2C: if(*((
int*)_tmp112)!= 1)goto _LL2E;_tmp117=((struct Cyc_Absyn_VarType_struct*)_tmp112)->f1;
_LL2D: if(Cyc_Tcutil_tvar_kind(_tmp117)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL2E: if(*((int*)_tmp112)!= 2)goto _LL30;_tmp118=((struct Cyc_Absyn_TunionType_struct*)
_tmp112)->f1;_tmp119=_tmp118.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp112)->f1).tunion_info).KnownTunion).tag != 1)goto _LL30;_tmp11A=(_tmp119.KnownTunion).f1;
_tmp11B=*_tmp11A;_LL2F: if(_tmp11B->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp11B->name,({const char*_tmp13D="_union";_tag_dynforward(_tmp13D,sizeof(char),
_get_zero_arr_size(_tmp13D,7));})));else{return Cyc_Absyn_void_star_typ();}_LL30:
if(*((int*)_tmp112)!= 2)goto _LL32;_LL31:({void*_tmp13E[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp13F="unresolved TunionType";_tag_dynforward(_tmp13F,sizeof(char),
_get_zero_arr_size(_tmp13F,22));}),_tag_dynforward(_tmp13E,sizeof(void*),0));});
_LL32: if(*((int*)_tmp112)!= 3)goto _LL34;_tmp11C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp112)->f1;_tmp11D=_tmp11C.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp112)->f1).field_info).KnownTunionfield).tag != 1)goto _LL34;_tmp11E=(_tmp11D.KnownTunionfield).f1;
_tmp11F=(_tmp11D.KnownTunionfield).f2;_LL33: if(_tmp11E->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp11E->name,({const char*_tmp140="_union";
_tag_dynforward(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,7));})));if(
_tmp11F->typs == 0){if(_tmp11E->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp11F->name,({const char*
_tmp141="_struct";_tag_dynforward(_tmp141,sizeof(char),_get_zero_arr_size(
_tmp141,8));})));}_LL34: if(*((int*)_tmp112)!= 3)goto _LL36;_LL35:({void*_tmp142[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp143="unresolved TunionFieldType";_tag_dynforward(_tmp143,sizeof(
char),_get_zero_arr_size(_tmp143,27));}),_tag_dynforward(_tmp142,sizeof(void*),0));});
_LL36: if(*((int*)_tmp112)!= 4)goto _LL38;_tmp120=((struct Cyc_Absyn_PointerType_struct*)
_tmp112)->f1;_tmp121=(void*)_tmp120.elt_typ;_tmp122=_tmp120.elt_tq;_tmp123=
_tmp120.ptr_atts;_tmp124=_tmp123.bounds;_LL37: _tmp121=Cyc_Toc_typ_to_c_array(
_tmp121);{union Cyc_Absyn_Constraint_union _tmp144=(Cyc_Absyn_compress_conref(
_tmp124))->v;void*_tmp145;void*_tmp146;_LL63: if((_tmp144.No_constr).tag != 2)goto
_LL65;_LL64: goto _LL66;_LL65: if((_tmp144.Eq_constr).tag != 0)goto _LL67;_tmp145=(
_tmp144.Eq_constr).f1;if((int)_tmp145 != 0)goto _LL67;_LL66: return Cyc_Toc_dynforward_ptr_typ;
_LL67: if((_tmp144.Eq_constr).tag != 0)goto _LL69;_tmp146=(_tmp144.Eq_constr).f1;
if((int)_tmp146 != 1)goto _LL69;_LL68: return Cyc_Toc_dyneither_ptr_typ;_LL69:;_LL6A:
return Cyc_Absyn_star_typ(_tmp121,(void*)2,_tmp122,Cyc_Absyn_false_conref);_LL62:;}
_LL38: if(*((int*)_tmp112)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp112 != 
1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp112 <= (void*)4)goto _LL58;if(*((int*)
_tmp112)!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(*((int*)_tmp112)!= 7)goto _LL40;
_tmp125=((struct Cyc_Absyn_ArrayType_struct*)_tmp112)->f1;_tmp126=(void*)_tmp125.elt_type;
_tmp127=_tmp125.tq;_tmp128=_tmp125.num_elts;_tmp129=_tmp125.zt_loc;_LL3F: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp126),_tmp127,_tmp128,Cyc_Absyn_false_conref,
_tmp129);_LL40: if(*((int*)_tmp112)!= 8)goto _LL42;_tmp12A=((struct Cyc_Absyn_FnType_struct*)
_tmp112)->f1;_tmp12B=(void*)_tmp12A.ret_typ;_tmp12C=_tmp12A.args;_tmp12D=_tmp12A.c_varargs;
_tmp12E=_tmp12A.cyc_varargs;_tmp12F=_tmp12A.attributes;_LL41: {struct Cyc_List_List*
_tmp147=0;for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){void*_tmp148=(void*)_tmp12F->hd;
_LL6C: if((int)_tmp148 != 3)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)_tmp148 != 4)
goto _LL70;_LL6F: goto _LL71;_LL70: if(_tmp148 <= (void*)17)goto _LL74;if(*((int*)
_tmp148)!= 3)goto _LL72;_LL71: continue;_LL72: if(*((int*)_tmp148)!= 4)goto _LL74;
_LL73: continue;_LL74:;_LL75: _tmp147=({struct Cyc_List_List*_tmp149=_cycalloc(
sizeof(*_tmp149));_tmp149->hd=(void*)((void*)_tmp12F->hd);_tmp149->tl=_tmp147;
_tmp149;});goto _LL6B;_LL6B:;}{struct Cyc_List_List*_tmp14A=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp12C);if(_tmp12E != 0){void*_tmp14B=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((
void*)_tmp12E->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp14C=({struct _tuple2*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->f1=
_tmp12E->name;_tmp14E->f2=_tmp12E->tq;_tmp14E->f3=_tmp14B;_tmp14E;});_tmp14A=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp14A,({struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->hd=
_tmp14C;_tmp14D->tl=0;_tmp14D;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F[0]=({struct Cyc_Absyn_FnType_struct
_tmp150;_tmp150.tag=8;_tmp150.f1=({struct Cyc_Absyn_FnInfo _tmp151;_tmp151.tvars=0;
_tmp151.effect=0;_tmp151.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp12B);_tmp151.args=
_tmp14A;_tmp151.c_varargs=_tmp12D;_tmp151.cyc_varargs=0;_tmp151.rgn_po=0;_tmp151.attributes=
_tmp147;_tmp151;});_tmp150;});_tmp14F;});}}_LL42: if(*((int*)_tmp112)!= 9)goto
_LL44;_tmp130=((struct Cyc_Absyn_TupleType_struct*)_tmp112)->f1;_LL43: _tmp130=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp130);{struct _dynforward_ptr*_tmp152=Cyc_Toc_add_tuple_type(
_tmp130);return Cyc_Absyn_strct(_tmp152);}_LL44: if(*((int*)_tmp112)!= 11)goto
_LL46;_tmp131=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp112)->f1;_tmp132=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp112)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp154;_tmp154.tag=11;_tmp154.f1=(void*)_tmp131;_tmp154.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp132);_tmp154;});_tmp153;});_LL46: if(*((
int*)_tmp112)!= 10)goto _LL48;_tmp133=((struct Cyc_Absyn_AggrType_struct*)_tmp112)->f1;
_tmp134=_tmp133.aggr_info;_tmp135=_tmp133.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmp155=Cyc_Absyn_get_known_aggrdecl(_tmp134);if((void*)_tmp155->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmp155->name);return Cyc_Absyn_strctq(_tmp155->name);}
_LL48: if(*((int*)_tmp112)!= 12)goto _LL4A;_tmp136=((struct Cyc_Absyn_EnumType_struct*)
_tmp112)->f1;_LL49: return t;_LL4A: if(*((int*)_tmp112)!= 13)goto _LL4C;_tmp137=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp112)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmp137);return t;_LL4C: if(*((int*)_tmp112)!= 17)goto _LL4E;_tmp138=((struct Cyc_Absyn_TypedefType_struct*)
_tmp112)->f1;_tmp139=((struct Cyc_Absyn_TypedefType_struct*)_tmp112)->f2;_tmp13A=((
struct Cyc_Absyn_TypedefType_struct*)_tmp112)->f3;_tmp13B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp112)->f4;_LL4D: if(_tmp13B == 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp157;_tmp157.tag=17;_tmp157.f1=_tmp138;_tmp157.f2=0;_tmp157.f3=_tmp13A;
_tmp157.f4=0;_tmp157;});_tmp156;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp159;_tmp159.tag=17;_tmp159.f1=_tmp138;_tmp159.f2=0;_tmp159.f3=_tmp13A;
_tmp159.f4=({void**_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=Cyc_Toc_typ_to_c_array(*
_tmp13B);_tmp15A;});_tmp159;});_tmp158;});}_LL4E: if(*((int*)_tmp112)!= 14)goto
_LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp112)!= 18)goto _LL52;_LL51: return Cyc_Absyn_uint_typ;
_LL52: if(*((int*)_tmp112)!= 15)goto _LL54;_tmp13C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp112)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(*((int*)_tmp112)!= 16)goto _LL56;_LL55: return Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_LL56: if(*((int*)
_tmp112)!= 19)goto _LL58;_LL57:({void*_tmp15B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp15C="Toc::typ_to_c: type translation passed a type integer";_tag_dynforward(
_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,54));}),_tag_dynforward(_tmp15B,
sizeof(void*),0));});_LL58: if((int)_tmp112 != 2)goto _LL5A;_LL59:({void*_tmp15D[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp15E="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,55));}),
_tag_dynforward(_tmp15D,sizeof(void*),0));});_LL5A: if((int)_tmp112 != 3)goto _LL5C;
_LL5B:({void*_tmp15F[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp160="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,57));}),
_tag_dynforward(_tmp15F,sizeof(void*),0));});_LL5C: if(_tmp112 <= (void*)4)goto
_LL5E;if(*((int*)_tmp112)!= 20)goto _LL5E;_LL5D: goto _LL5F;_LL5E: if(_tmp112 <= (
void*)4)goto _LL60;if(*((int*)_tmp112)!= 21)goto _LL60;_LL5F: goto _LL61;_LL60: if(
_tmp112 <= (void*)4)goto _LL25;if(*((int*)_tmp112)!= 22)goto _LL25;_LL61:({void*
_tmp161[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp162="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,49));}),
_tag_dynforward(_tmp161,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp163=t;struct Cyc_Absyn_ArrayInfo _tmp164;void*_tmp165;struct Cyc_Absyn_Tqual
_tmp166;_LL77: if(_tmp163 <= (void*)4)goto _LL79;if(*((int*)_tmp163)!= 7)goto _LL79;
_tmp164=((struct Cyc_Absyn_ArrayType_struct*)_tmp163)->f1;_tmp165=(void*)_tmp164.elt_type;
_tmp166=_tmp164.tq;_LL78: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp165,(void*)
2,_tmp166,Cyc_Absyn_false_conref),e);_LL79:;_LL7A: return Cyc_Toc_cast_it(t,e);
_LL76:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp167=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp168;void*_tmp169;struct Cyc_Absyn_AggrInfo _tmp16A;
union Cyc_Absyn_AggrInfoU_union _tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_Absyn_TunionFieldInfo
_tmp16D;union Cyc_Absyn_TunionFieldInfoU_union _tmp16E;struct Cyc_Absyn_Tuniondecl*
_tmp16F;struct Cyc_Absyn_Tunionfield*_tmp170;struct Cyc_List_List*_tmp171;_LL7C:
if((int)_tmp167 != 0)goto _LL7E;_LL7D: return 1;_LL7E: if(_tmp167 <= (void*)4)goto
_LL86;if(*((int*)_tmp167)!= 1)goto _LL80;_LL7F: return 0;_LL80: if(*((int*)_tmp167)
!= 5)goto _LL82;_LL81: goto _LL83;_LL82: if(*((int*)_tmp167)!= 12)goto _LL84;_LL83:
goto _LL85;_LL84: if(*((int*)_tmp167)!= 13)goto _LL86;_LL85: goto _LL87;_LL86: if((int)
_tmp167 != 1)goto _LL88;_LL87: goto _LL89;_LL88: if(_tmp167 <= (void*)4)goto _LLA0;if(*((
int*)_tmp167)!= 6)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)_tmp167)!= 8)goto
_LL8C;_LL8B: goto _LL8D;_LL8C: if(*((int*)_tmp167)!= 18)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: if(*((int*)_tmp167)!= 14)goto _LL90;_LL8F: return 1;_LL90: if(*((int*)_tmp167)
!= 7)goto _LL92;_tmp168=((struct Cyc_Absyn_ArrayType_struct*)_tmp167)->f1;_tmp169=(
void*)_tmp168.elt_type;_LL91: return Cyc_Toc_atomic_typ(_tmp169);_LL92: if(*((int*)
_tmp167)!= 10)goto _LL94;_tmp16A=((struct Cyc_Absyn_AggrType_struct*)_tmp167)->f1;
_tmp16B=_tmp16A.aggr_info;_LL93:{union Cyc_Absyn_AggrInfoU_union _tmp172=_tmp16B;
_LLA3: if((_tmp172.UnknownAggr).tag != 0)goto _LLA5;_LLA4: return 0;_LLA5:;_LLA6: goto
_LLA2;_LLA2:;}{struct Cyc_Absyn_Aggrdecl*_tmp173=Cyc_Absyn_get_known_aggrdecl(
_tmp16B);if(_tmp173->impl == 0)return 0;{struct Cyc_List_List*_tmp174=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp173->impl))->fields;for(0;_tmp174 != 0;_tmp174=_tmp174->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp174->hd)->type))
return 0;}}return 1;}_LL94: if(*((int*)_tmp167)!= 11)goto _LL96;_tmp16C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp167)->f2;_LL95: for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp16C->hd)->type))return 0;}return 1;_LL96:
if(*((int*)_tmp167)!= 3)goto _LL98;_tmp16D=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp167)->f1;_tmp16E=_tmp16D.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp167)->f1).field_info).KnownTunionfield).tag != 1)goto _LL98;_tmp16F=(_tmp16E.KnownTunionfield).f1;
_tmp170=(_tmp16E.KnownTunionfield).f2;_LL97: _tmp171=_tmp170->typs;goto _LL99;
_LL98: if(*((int*)_tmp167)!= 9)goto _LL9A;_tmp171=((struct Cyc_Absyn_TupleType_struct*)
_tmp167)->f1;_LL99: for(0;_tmp171 != 0;_tmp171=_tmp171->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp171->hd)).f2))return 0;}return 1;_LL9A: if(*((int*)_tmp167)!= 2)
goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp167)!= 4)goto _LL9E;_LL9D: goto
_LL9F;_LL9E: if(*((int*)_tmp167)!= 15)goto _LLA0;_LL9F: return 0;_LLA0:;_LLA1:({
struct Cyc_String_pa_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp175[
1]={& _tmp177};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp176="atomic_typ:  bad type %s";_tag_dynforward(_tmp176,sizeof(char),
_get_zero_arr_size(_tmp176,25));}),_tag_dynforward(_tmp175,sizeof(void*),1));}});
_LL7B:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp178=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp179;void*_tmp17A;_LLA8: if(_tmp178 <= (void*)4)goto
_LLAA;if(*((int*)_tmp178)!= 4)goto _LLAA;_tmp179=((struct Cyc_Absyn_PointerType_struct*)
_tmp178)->f1;_tmp17A=(void*)_tmp179.elt_typ;_LLA9: {void*_tmp17B=Cyc_Tcutil_compress(
_tmp17A);_LLAD: if((int)_tmp17B != 0)goto _LLAF;_LLAE: return 1;_LLAF:;_LLB0: return 0;
_LLAC:;}_LLAA:;_LLAB: return 0;_LLA7:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp17C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp17D;union Cyc_Absyn_AggrInfoU_union _tmp17E;struct Cyc_List_List*_tmp17F;_LLB2:
if(_tmp17C <= (void*)4)goto _LLB6;if(*((int*)_tmp17C)!= 10)goto _LLB4;_tmp17D=((
struct Cyc_Absyn_AggrType_struct*)_tmp17C)->f1;_tmp17E=_tmp17D.aggr_info;_LLB3: {
struct Cyc_Absyn_Aggrdecl*_tmp180=Cyc_Absyn_get_known_aggrdecl(_tmp17E);if(
_tmp180->impl == 0)({void*_tmp181[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp182="is_poly_field: type missing fields";
_tag_dynforward(_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,35));}),
_tag_dynforward(_tmp181,sizeof(void*),0));});_tmp17F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp180->impl))->fields;goto _LLB5;}_LLB4: if(*((int*)_tmp17C)!= 11)
goto _LLB6;_tmp17F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp17C)->f2;_LLB5: {
struct Cyc_Absyn_Aggrfield*_tmp183=Cyc_Absyn_lookup_field(_tmp17F,f);if(_tmp183 == 
0)({struct Cyc_String_pa_struct _tmp186;_tmp186.tag=0;_tmp186.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp184[1]={& _tmp186};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp185="is_poly_field: bad field %s";_tag_dynforward(_tmp185,sizeof(char),
_get_zero_arr_size(_tmp185,28));}),_tag_dynforward(_tmp184,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp183->type);}_LLB6:;_LLB7:({struct Cyc_String_pa_struct
_tmp189;_tmp189.tag=0;_tmp189.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp187[1]={& _tmp189};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp188="is_poly_field: bad type %s";_tag_dynforward(_tmp188,sizeof(char),
_get_zero_arr_size(_tmp188,27));}),_tag_dynforward(_tmp187,sizeof(void*),1));}});
_LLB1:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp18A=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp18B;struct _dynforward_ptr*_tmp18C;struct Cyc_Absyn_Exp*
_tmp18D;struct _dynforward_ptr*_tmp18E;_LLB9: if(*((int*)_tmp18A)!= 23)goto _LLBB;
_tmp18B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp18A)->f1;_tmp18C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp18A)->f2;_LLBA: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp18B->topt))->v,_tmp18C);_LLBB: if(*((int*)_tmp18A)!= 24)goto _LLBD;
_tmp18D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp18A)->f1;_tmp18E=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp18A)->f2;_LLBC: {void*_tmp18F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp18D->topt))->v);struct Cyc_Absyn_PtrInfo _tmp190;void*_tmp191;
_LLC0: if(_tmp18F <= (void*)4)goto _LLC2;if(*((int*)_tmp18F)!= 4)goto _LLC2;_tmp190=((
struct Cyc_Absyn_PointerType_struct*)_tmp18F)->f1;_tmp191=(void*)_tmp190.elt_typ;
_LLC1: return Cyc_Toc_is_poly_field(_tmp191,_tmp18E);_LLC2:;_LLC3:({struct Cyc_String_pa_struct
_tmp194;_tmp194.tag=0;_tmp194.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp18D->topt))->v));{
void*_tmp192[1]={& _tmp194};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp193="is_poly_project: bad type %s";
_tag_dynforward(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,29));}),
_tag_dynforward(_tmp192,sizeof(void*),1));}});_LLBF:;}_LLBD:;_LLBE: return 0;_LLB8:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp195=_cycalloc(sizeof(*_tmp195));
_tmp195->hd=s;_tmp195->tl=0;_tmp195;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->hd=s;_tmp196->tl=
0;_tmp196;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp197[2];_tmp197[1]=s;_tmp197[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp197,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp198[2];_tmp198[1]=n;
_tmp198[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp198,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp199[2];_tmp199[1]=n;_tmp199[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp199,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp19A[3];_tmp19A[2]=n;_tmp19A[1]=
s;_tmp19A[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp19A,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->hd=e;_tmp19B->tl=
0;_tmp19B;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp19C=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp19D;_LLC5: if(*((int*)
_tmp19C)!= 0)goto _LLC7;_tmp19D=((struct Cyc_Absyn_Const_e_struct*)_tmp19C)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp19C)->f1).String_c).tag != 5)goto _LLC7;
_LLC6: is_string=1;goto _LLC4;_LLC7:;_LLC8: goto _LLC4;_LLC4:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F[0]=({
struct Cyc_Absyn_Var_d_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=vd;_tmp1A0;});
_tmp19F;}),0);_tmp19E->tl=Cyc_Toc_result_decls;_tmp19E;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple6*_tmp1A1[3];_tmp1A1[2]=({struct _tuple6*_tmp1A4=_cycalloc(
sizeof(*_tmp1A4));_tmp1A4->f1=0;_tmp1A4->f2=xplussz;_tmp1A4;});_tmp1A1[1]=({
struct _tuple6*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->f1=0;_tmp1A3->f2=xexp;
_tmp1A3;});_tmp1A1[0]=({struct _tuple6*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));
_tmp1A2->f1=0;_tmp1A2->f2=xexp;_tmp1A2;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1A1,sizeof(struct _tuple6*),3));}),
0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*_tmp1A5[2];_tmp1A5[
1]=({struct _tuple6*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->f1=0;_tmp1A7->f2=
xplussz;_tmp1A7;});_tmp1A5[0]=({struct _tuple6*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));
_tmp1A6->f1=0;_tmp1A6->f2=xexp;_tmp1A6;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1A5,sizeof(struct _tuple6*),2));}),
0);}return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dynforward_ptr*label;
struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dynforward_ptr**break_lab;struct _dynforward_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp1A9;int _tmp1AA;struct Cyc_Toc_Env*
_tmp1A8=nv;_tmp1A9=*_tmp1A8;_tmp1AA=_tmp1A9.toplevel;return _tmp1AA;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){struct
Cyc_Toc_Env _tmp1AC;struct Cyc_Dict_Dict _tmp1AD;struct Cyc_Toc_Env*_tmp1AB=nv;
_tmp1AC=*_tmp1AB;_tmp1AD=_tmp1AC.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp1AD,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*_tmp1AE=
_region_malloc(r,sizeof(*_tmp1AE));_tmp1AE->break_lab=(struct _dynforward_ptr**)0;
_tmp1AE->continue_lab=(struct _dynforward_ptr**)0;_tmp1AE->fallthru_info=(struct
Cyc_Toc_FallthruInfo*)0;_tmp1AE->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp1AE->toplevel=(int)1;_tmp1AE;});}static struct Cyc_Toc_Env*
Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1B0;struct _dynforward_ptr**_tmp1B1;struct _dynforward_ptr**_tmp1B2;struct Cyc_Toc_FallthruInfo*
_tmp1B3;struct Cyc_Dict_Dict _tmp1B4;int _tmp1B5;struct Cyc_Toc_Env*_tmp1AF=e;
_tmp1B0=*_tmp1AF;_tmp1B1=_tmp1B0.break_lab;_tmp1B2=_tmp1B0.continue_lab;_tmp1B3=
_tmp1B0.fallthru_info;_tmp1B4=_tmp1B0.varmap;_tmp1B5=_tmp1B0.toplevel;return({
struct Cyc_Toc_Env*_tmp1B6=_region_malloc(r,sizeof(*_tmp1B6));_tmp1B6->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1B1);_tmp1B6->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1B2);_tmp1B6->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1B3;_tmp1B6->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1B4);_tmp1B6->toplevel=(int)_tmp1B5;_tmp1B6;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1B8;struct _dynforward_ptr**_tmp1B9;struct _dynforward_ptr**_tmp1BA;struct Cyc_Toc_FallthruInfo*
_tmp1BB;struct Cyc_Dict_Dict _tmp1BC;int _tmp1BD;struct Cyc_Toc_Env*_tmp1B7=e;
_tmp1B8=*_tmp1B7;_tmp1B9=_tmp1B8.break_lab;_tmp1BA=_tmp1B8.continue_lab;_tmp1BB=
_tmp1B8.fallthru_info;_tmp1BC=_tmp1B8.varmap;_tmp1BD=_tmp1B8.toplevel;return({
struct Cyc_Toc_Env*_tmp1BE=_region_malloc(r,sizeof(*_tmp1BE));_tmp1BE->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1B9);_tmp1BE->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1BA);_tmp1BE->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1BB;_tmp1BE->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1BC);_tmp1BE->toplevel=(int)0;_tmp1BE;});}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp1C0;struct
_dynforward_ptr**_tmp1C1;struct _dynforward_ptr**_tmp1C2;struct Cyc_Toc_FallthruInfo*
_tmp1C3;struct Cyc_Dict_Dict _tmp1C4;int _tmp1C5;struct Cyc_Toc_Env*_tmp1BF=e;
_tmp1C0=*_tmp1BF;_tmp1C1=_tmp1C0.break_lab;_tmp1C2=_tmp1C0.continue_lab;_tmp1C3=
_tmp1C0.fallthru_info;_tmp1C4=_tmp1C0.varmap;_tmp1C5=_tmp1C0.toplevel;return({
struct Cyc_Toc_Env*_tmp1C6=_region_malloc(r,sizeof(*_tmp1C6));_tmp1C6->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1C1);_tmp1C6->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1C2);_tmp1C6->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1C3;_tmp1C6->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1C4);_tmp1C6->toplevel=(int)1;_tmp1C6;});}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp1C7=(*x).f1;_LLCA: if((_tmp1C7.Rel_n).tag != 1)
goto _LLCC;_LLCB:({struct Cyc_String_pa_struct _tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp1C8[1]={& _tmp1CA};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1C9="Toc::add_varmap on Rel_n: %s\n";
_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,30));}),
_tag_dynforward(_tmp1C8,sizeof(void*),1));}});_LLCC:;_LLCD: goto _LLC9;_LLC9:;}{
struct Cyc_Toc_Env _tmp1CC;struct _dynforward_ptr**_tmp1CD;struct _dynforward_ptr**
_tmp1CE;struct Cyc_Toc_FallthruInfo*_tmp1CF;struct Cyc_Dict_Dict _tmp1D0;int _tmp1D1;
struct Cyc_Toc_Env*_tmp1CB=e;_tmp1CC=*_tmp1CB;_tmp1CD=_tmp1CC.break_lab;_tmp1CE=
_tmp1CC.continue_lab;_tmp1CF=_tmp1CC.fallthru_info;_tmp1D0=_tmp1CC.varmap;
_tmp1D1=_tmp1CC.toplevel;{struct Cyc_Dict_Dict _tmp1D2=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1D0),x,y);return({struct Cyc_Toc_Env*_tmp1D3=_region_malloc(r,sizeof(*
_tmp1D3));_tmp1D3->break_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp1CD);_tmp1D3->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp1CE);_tmp1D3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp1CF;_tmp1D3->varmap=(
struct Cyc_Dict_Dict)_tmp1D2;_tmp1D3->toplevel=(int)_tmp1D1;_tmp1D3;});}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp1D5;struct _dynforward_ptr**_tmp1D6;struct _dynforward_ptr**
_tmp1D7;struct Cyc_Toc_FallthruInfo*_tmp1D8;struct Cyc_Dict_Dict _tmp1D9;int _tmp1DA;
struct Cyc_Toc_Env*_tmp1D4=e;_tmp1D5=*_tmp1D4;_tmp1D6=_tmp1D5.break_lab;_tmp1D7=
_tmp1D5.continue_lab;_tmp1D8=_tmp1D5.fallthru_info;_tmp1D9=_tmp1D5.varmap;
_tmp1DA=_tmp1D5.toplevel;return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,
sizeof(*_tmp1DB));_tmp1DB->break_lab=(struct _dynforward_ptr**)0;_tmp1DB->continue_lab=(
struct _dynforward_ptr**)0;_tmp1DB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1D8;_tmp1DB->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1D9);_tmp1DB->toplevel=(
int)_tmp1DA;_tmp1DB;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct
_dynforward_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp1DC=_region_malloc(r,sizeof(*_tmp1DC));_tmp1DC->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp1DC->tl=fallthru_vars;_tmp1DC;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp1DE;struct _dynforward_ptr**_tmp1DF;struct _dynforward_ptr**
_tmp1E0;struct Cyc_Toc_FallthruInfo*_tmp1E1;struct Cyc_Dict_Dict _tmp1E2;int _tmp1E3;
struct Cyc_Toc_Env*_tmp1DD=e;_tmp1DE=*_tmp1DD;_tmp1DF=_tmp1DE.break_lab;_tmp1E0=
_tmp1DE.continue_lab;_tmp1E1=_tmp1DE.fallthru_info;_tmp1E2=_tmp1DE.varmap;
_tmp1E3=_tmp1DE.toplevel;{struct Cyc_Toc_Env _tmp1E5;struct Cyc_Dict_Dict _tmp1E6;
struct Cyc_Toc_Env*_tmp1E4=next_case_env;_tmp1E5=*_tmp1E4;_tmp1E6=_tmp1E5.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1E9=
_region_malloc(r,sizeof(*_tmp1E9));_tmp1E9->label=fallthru_l;_tmp1E9->binders=
fallthru_vars;_tmp1E9->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1E6);_tmp1E9;});
return({struct Cyc_Toc_Env*_tmp1E7=_region_malloc(r,sizeof(*_tmp1E7));_tmp1E7->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp1E8=_region_malloc(r,
sizeof(*_tmp1E8));_tmp1E8[0]=break_l;_tmp1E8;});_tmp1E7->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp1E0);_tmp1E7->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp1E7->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1E2);_tmp1E7->toplevel=(
int)_tmp1E3;_tmp1E7;});}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct
Cyc_Toc_Env _tmp1EB;struct _dynforward_ptr**_tmp1EC;struct _dynforward_ptr**_tmp1ED;
struct Cyc_Toc_FallthruInfo*_tmp1EE;struct Cyc_Dict_Dict _tmp1EF;int _tmp1F0;struct
Cyc_Toc_Env*_tmp1EA=e;_tmp1EB=*_tmp1EA;_tmp1EC=_tmp1EB.break_lab;_tmp1ED=_tmp1EB.continue_lab;
_tmp1EE=_tmp1EB.fallthru_info;_tmp1EF=_tmp1EB.varmap;_tmp1F0=_tmp1EB.toplevel;
return({struct Cyc_Toc_Env*_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));_tmp1F1->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp1F2=_region_malloc(r,
sizeof(*_tmp1F2));_tmp1F2[0]=break_l;_tmp1F2;});_tmp1F1->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp1ED);_tmp1F1->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp1F1->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1EF);_tmp1F1->toplevel=(
int)_tmp1F0;_tmp1F1;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct
Cyc_Toc_Env _tmp1F4;struct _dynforward_ptr**_tmp1F5;struct _dynforward_ptr**_tmp1F6;
struct Cyc_Toc_FallthruInfo*_tmp1F7;struct Cyc_Dict_Dict _tmp1F8;int _tmp1F9;struct
Cyc_Toc_Env*_tmp1F3=e;_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.break_lab;_tmp1F6=_tmp1F4.continue_lab;
_tmp1F7=_tmp1F4.fallthru_info;_tmp1F8=_tmp1F4.varmap;_tmp1F9=_tmp1F4.toplevel;
return({struct Cyc_Toc_Env*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));_tmp1FA->break_lab=(
struct _dynforward_ptr**)0;_tmp1FA->continue_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp1F6);_tmp1FA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));_tmp1FB->label=
next_l;_tmp1FB->binders=0;_tmp1FB->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp1FB;});_tmp1FA->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1F8);_tmp1FA->toplevel=(
int)_tmp1F9;_tmp1FA;});}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp1FC=(void*)e->annot;
_LLCF: if(*((void**)_tmp1FC)!= Cyc_CfFlowInfo_UnknownZ)goto _LLD1;_LLD0: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD1: if(*((void**)_tmp1FC)
!= Cyc_CfFlowInfo_NotZero)goto _LLD3;_LLD2: return 0;_LLD3: if(_tmp1FC != Cyc_CfFlowInfo_IsZero)
goto _LLD5;_LLD4:({void*_tmp1FD[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp1FE="dereference of NULL pointer";
_tag_dynforward(_tmp1FE,sizeof(char),_get_zero_arr_size(_tmp1FE,28));}),
_tag_dynforward(_tmp1FD,sizeof(void*),0));});return 0;_LLD5: if(_tmp1FC != Cyc_Absyn_EmptyAnnot)
goto _LLD7;_LLD6: return 0;_LLD7: if(*((void**)_tmp1FC)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD9;_LLD8:({void*_tmp1FF[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp200="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp200,sizeof(char),_get_zero_arr_size(_tmp200,46));}),
_tag_dynforward(_tmp1FF,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD9:;_LLDA:({void*_tmp201[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp202="need_null_check";_tag_dynforward(_tmp202,sizeof(char),
_get_zero_arr_size(_tmp202,16));}),_tag_dynforward(_tmp201,sizeof(void*),0));});
_LLCE:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp203=(void*)e->annot;struct Cyc_List_List*_tmp204;struct Cyc_List_List*_tmp205;
_LLDC: if(*((void**)_tmp203)!= Cyc_CfFlowInfo_UnknownZ)goto _LLDE;_tmp204=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp203)->f1;_LLDD: return _tmp204;_LLDE: if(*((
void**)_tmp203)!= Cyc_CfFlowInfo_NotZero)goto _LLE0;_tmp205=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp203)->f1;_LLDF: return _tmp205;_LLE0: if(_tmp203 != Cyc_CfFlowInfo_IsZero)goto
_LLE2;_LLE1:({void*_tmp206[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp207="dereference of NULL pointer";
_tag_dynforward(_tmp207,sizeof(char),_get_zero_arr_size(_tmp207,28));}),
_tag_dynforward(_tmp206,sizeof(void*),0));});return 0;_LLE2: if(*((void**)_tmp203)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(_tmp203 != Cyc_Absyn_EmptyAnnot)
goto _LLE6;_LLE5: return 0;_LLE6:;_LLE7:({void*_tmp208[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp209="get_relns";_tag_dynforward(_tmp209,sizeof(char),_get_zero_arr_size(
_tmp209,10));}),_tag_dynforward(_tmp208,sizeof(void*),0));});_LLDB:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp20A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp20B;struct Cyc_Absyn_PtrAtts _tmp20C;struct Cyc_Absyn_Conref*
_tmp20D;struct Cyc_Absyn_Conref*_tmp20E;struct Cyc_Absyn_ArrayInfo _tmp20F;struct
Cyc_Absyn_Exp*_tmp210;_LLE9: if(_tmp20A <= (void*)4)goto _LLED;if(*((int*)_tmp20A)
!= 4)goto _LLEB;_tmp20B=((struct Cyc_Absyn_PointerType_struct*)_tmp20A)->f1;
_tmp20C=_tmp20B.ptr_atts;_tmp20D=_tmp20C.bounds;_tmp20E=_tmp20C.zero_term;_LLEA: {
void*_tmp211=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp20D);struct Cyc_Absyn_Exp*
_tmp212;_LLF0: if((int)_tmp211 != 0)goto _LLF2;_LLF1: return 0;_LLF2: if((int)_tmp211
!= 1)goto _LLF4;_LLF3: return 0;_LLF4: if(_tmp211 <= (void*)2)goto _LLF6;if(*((int*)
_tmp211)!= 0)goto _LLF6;_tmp212=((struct Cyc_Absyn_Upper_b_struct*)_tmp211)->f1;
_LLF5: {unsigned int _tmp214;int _tmp215;struct _tuple5 _tmp213=Cyc_Evexp_eval_const_uint_exp(
_tmp212);_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;return _tmp215  && i <= _tmp214;}
_LLF6: if(_tmp211 <= (void*)2)goto _LLEF;if(*((int*)_tmp211)!= 1)goto _LLEF;_LLF7:({
void*_tmp216[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp217="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp217,sizeof(char),_get_zero_arr_size(_tmp217,30));}),
_tag_dynforward(_tmp216,sizeof(void*),0));});_LLEF:;}_LLEB: if(*((int*)_tmp20A)!= 
7)goto _LLED;_tmp20F=((struct Cyc_Absyn_ArrayType_struct*)_tmp20A)->f1;_tmp210=
_tmp20F.num_elts;_LLEC: if(_tmp210 == 0)return 0;{unsigned int _tmp219;int _tmp21A;
struct _tuple5 _tmp218=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp210);
_tmp219=_tmp218.f1;_tmp21A=_tmp218.f2;return _tmp21A  && i <= _tmp219;}_LLED:;_LLEE:
return 0;_LLE8:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp21B=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp21B->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp21C=_tmp21B->rop;
struct Cyc_Absyn_Vardecl*_tmp21D;struct Cyc_Absyn_Vardecl*_tmp21E;_LLF9: if((
_tmp21C.LessSize).tag != 2)goto _LLFB;_tmp21D=(_tmp21C.LessSize).f1;_LLFA: _tmp21E=
_tmp21D;goto _LLFC;_LLFB: if((_tmp21C.LessEqSize).tag != 4)goto _LLFD;_tmp21E=(
_tmp21C.LessEqSize).f1;_LLFC: if(_tmp21E == v)return 1;else{goto _LLF8;}_LLFD:;_LLFE:
continue;_LLF8:;}}return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp21F=(void*)e->r;
void*_tmp220;struct Cyc_Absyn_Vardecl*_tmp221;void*_tmp222;struct Cyc_Absyn_Vardecl*
_tmp223;void*_tmp224;struct Cyc_Absyn_Vardecl*_tmp225;void*_tmp226;struct Cyc_Absyn_Vardecl*
_tmp227;void*_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_List_List _tmp22A;
struct Cyc_Absyn_Exp*_tmp22B;_LL100: if(*((int*)_tmp21F)!= 1)goto _LL102;_tmp220=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp21F)->f2;if(_tmp220 <= (void*)1)goto
_LL102;if(*((int*)_tmp220)!= 4)goto _LL102;_tmp221=((struct Cyc_Absyn_Pat_b_struct*)
_tmp220)->f1;_LL101: _tmp223=_tmp221;goto _LL103;_LL102: if(*((int*)_tmp21F)!= 1)
goto _LL104;_tmp222=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp21F)->f2;if(
_tmp222 <= (void*)1)goto _LL104;if(*((int*)_tmp222)!= 3)goto _LL104;_tmp223=((
struct Cyc_Absyn_Local_b_struct*)_tmp222)->f1;_LL103: _tmp225=_tmp223;goto _LL105;
_LL104: if(*((int*)_tmp21F)!= 1)goto _LL106;_tmp224=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp21F)->f2;if(_tmp224 <= (void*)1)goto _LL106;if(*((int*)_tmp224)!= 0)goto _LL106;
_tmp225=((struct Cyc_Absyn_Global_b_struct*)_tmp224)->f1;_LL105: _tmp227=_tmp225;
goto _LL107;_LL106: if(*((int*)_tmp21F)!= 1)goto _LL108;_tmp226=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp21F)->f2;if(_tmp226 <= (void*)1)goto _LL108;if(*((int*)_tmp226)!= 2)goto _LL108;
_tmp227=((struct Cyc_Absyn_Param_b_struct*)_tmp226)->f1;_LL107: if(_tmp227->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp227))return 1;goto _LLFF;_LL108:
if(*((int*)_tmp21F)!= 3)goto _LL10A;_tmp228=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp21F)->f1;if((int)_tmp228 != 19)goto _LL10A;_tmp229=((struct Cyc_Absyn_Primop_e_struct*)
_tmp21F)->f2;if(_tmp229 == 0)goto _LL10A;_tmp22A=*_tmp229;_tmp22B=(struct Cyc_Absyn_Exp*)
_tmp22A.hd;_LL109:{void*_tmp22C=(void*)_tmp22B->r;void*_tmp22D;struct Cyc_Absyn_Vardecl*
_tmp22E;void*_tmp22F;struct Cyc_Absyn_Vardecl*_tmp230;void*_tmp231;struct Cyc_Absyn_Vardecl*
_tmp232;void*_tmp233;struct Cyc_Absyn_Vardecl*_tmp234;_LL10D: if(*((int*)_tmp22C)
!= 1)goto _LL10F;_tmp22D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp22C)->f2;if(
_tmp22D <= (void*)1)goto _LL10F;if(*((int*)_tmp22D)!= 4)goto _LL10F;_tmp22E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp22D)->f1;_LL10E: _tmp230=_tmp22E;goto _LL110;
_LL10F: if(*((int*)_tmp22C)!= 1)goto _LL111;_tmp22F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp22C)->f2;if(_tmp22F <= (void*)1)goto _LL111;if(*((int*)_tmp22F)!= 3)goto _LL111;
_tmp230=((struct Cyc_Absyn_Local_b_struct*)_tmp22F)->f1;_LL110: _tmp232=_tmp230;
goto _LL112;_LL111: if(*((int*)_tmp22C)!= 1)goto _LL113;_tmp231=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp22C)->f2;if(_tmp231 <= (void*)1)goto _LL113;if(*((int*)_tmp231)!= 0)goto _LL113;
_tmp232=((struct Cyc_Absyn_Global_b_struct*)_tmp231)->f1;_LL112: _tmp234=_tmp232;
goto _LL114;_LL113: if(*((int*)_tmp22C)!= 1)goto _LL115;_tmp233=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp22C)->f2;if(_tmp233 <= (void*)1)goto _LL115;if(*((int*)_tmp233)!= 2)goto _LL115;
_tmp234=((struct Cyc_Absyn_Param_b_struct*)_tmp233)->f1;_LL114: return _tmp234 == v;
_LL115:;_LL116: goto _LL10C;_LL10C:;}goto _LLFF;_LL10A:;_LL10B: goto _LLFF;_LLFF:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp235=(void*)a->r;void*_tmp236;struct Cyc_Absyn_Vardecl*
_tmp237;void*_tmp238;struct Cyc_Absyn_Vardecl*_tmp239;void*_tmp23A;struct Cyc_Absyn_Vardecl*
_tmp23B;void*_tmp23C;struct Cyc_Absyn_Vardecl*_tmp23D;_LL118: if(*((int*)_tmp235)
!= 1)goto _LL11A;_tmp236=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp235)->f2;if(
_tmp236 <= (void*)1)goto _LL11A;if(*((int*)_tmp236)!= 4)goto _LL11A;_tmp237=((
struct Cyc_Absyn_Pat_b_struct*)_tmp236)->f1;_LL119: _tmp239=_tmp237;goto _LL11B;
_LL11A: if(*((int*)_tmp235)!= 1)goto _LL11C;_tmp238=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp235)->f2;if(_tmp238 <= (void*)1)goto _LL11C;if(*((int*)_tmp238)!= 3)goto _LL11C;
_tmp239=((struct Cyc_Absyn_Local_b_struct*)_tmp238)->f1;_LL11B: _tmp23B=_tmp239;
goto _LL11D;_LL11C: if(*((int*)_tmp235)!= 1)goto _LL11E;_tmp23A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp235)->f2;if(_tmp23A <= (void*)1)goto _LL11E;if(*((int*)_tmp23A)!= 0)goto _LL11E;
_tmp23B=((struct Cyc_Absyn_Global_b_struct*)_tmp23A)->f1;_LL11D: _tmp23D=_tmp23B;
goto _LL11F;_LL11E: if(*((int*)_tmp235)!= 1)goto _LL120;_tmp23C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp235)->f2;if(_tmp23C <= (void*)1)goto _LL120;if(*((int*)_tmp23C)!= 2)goto _LL120;
_tmp23D=((struct Cyc_Absyn_Param_b_struct*)_tmp23C)->f1;_LL11F: if(_tmp23D->escapes)
return 0;inner_loop: {void*_tmp23E=(void*)i->r;void*_tmp23F;struct Cyc_Absyn_Exp*
_tmp240;union Cyc_Absyn_Cnst_union _tmp241;void*_tmp242;int _tmp243;union Cyc_Absyn_Cnst_union
_tmp244;void*_tmp245;int _tmp246;union Cyc_Absyn_Cnst_union _tmp247;void*_tmp248;
int _tmp249;void*_tmp24A;struct Cyc_List_List*_tmp24B;struct Cyc_List_List _tmp24C;
struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_List_List
_tmp24F;struct Cyc_Absyn_Exp*_tmp250;void*_tmp251;struct Cyc_Absyn_Vardecl*_tmp252;
void*_tmp253;struct Cyc_Absyn_Vardecl*_tmp254;void*_tmp255;struct Cyc_Absyn_Vardecl*
_tmp256;void*_tmp257;struct Cyc_Absyn_Vardecl*_tmp258;_LL123: if(*((int*)_tmp23E)
!= 15)goto _LL125;_tmp23F=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp23E)->f1;
_tmp240=((struct Cyc_Absyn_Cast_e_struct*)_tmp23E)->f2;_LL124: i=_tmp240;goto
inner_loop;_LL125: if(*((int*)_tmp23E)!= 0)goto _LL127;_tmp241=((struct Cyc_Absyn_Const_e_struct*)
_tmp23E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp23E)->f1).Int_c).tag != 2)
goto _LL127;_tmp242=(_tmp241.Int_c).f1;if((int)_tmp242 != 2)goto _LL127;_tmp243=(
_tmp241.Int_c).f2;_LL126: _tmp246=_tmp243;goto _LL128;_LL127: if(*((int*)_tmp23E)!= 
0)goto _LL129;_tmp244=((struct Cyc_Absyn_Const_e_struct*)_tmp23E)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp23E)->f1).Int_c).tag != 2)goto _LL129;_tmp245=(
_tmp244.Int_c).f1;if((int)_tmp245 != 0)goto _LL129;_tmp246=(_tmp244.Int_c).f2;
_LL128: return _tmp246 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp246 + 1),(
void*)_tmp23D->type);_LL129: if(*((int*)_tmp23E)!= 0)goto _LL12B;_tmp247=((struct
Cyc_Absyn_Const_e_struct*)_tmp23E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp23E)->f1).Int_c).tag != 2)goto _LL12B;_tmp248=(_tmp247.Int_c).f1;if((int)
_tmp248 != 1)goto _LL12B;_tmp249=(_tmp247.Int_c).f2;_LL12A: return Cyc_Toc_check_const_array((
unsigned int)(_tmp249 + 1),(void*)_tmp23D->type);_LL12B: if(*((int*)_tmp23E)!= 3)
goto _LL12D;_tmp24A=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp23E)->f1;if((
int)_tmp24A != 4)goto _LL12D;_tmp24B=((struct Cyc_Absyn_Primop_e_struct*)_tmp23E)->f2;
if(_tmp24B == 0)goto _LL12D;_tmp24C=*_tmp24B;_tmp24D=(struct Cyc_Absyn_Exp*)_tmp24C.hd;
_tmp24E=_tmp24C.tl;if(_tmp24E == 0)goto _LL12D;_tmp24F=*_tmp24E;_tmp250=(struct Cyc_Absyn_Exp*)
_tmp24F.hd;_LL12C: return Cyc_Toc_check_leq_size(relns,_tmp23D,_tmp250);_LL12D: if(*((
int*)_tmp23E)!= 1)goto _LL12F;_tmp251=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp251 <= (void*)1)goto _LL12F;if(*((int*)_tmp251)!= 4)goto _LL12F;
_tmp252=((struct Cyc_Absyn_Pat_b_struct*)_tmp251)->f1;_LL12E: _tmp254=_tmp252;goto
_LL130;_LL12F: if(*((int*)_tmp23E)!= 1)goto _LL131;_tmp253=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp253 <= (void*)1)goto _LL131;if(*((int*)_tmp253)!= 3)goto _LL131;
_tmp254=((struct Cyc_Absyn_Local_b_struct*)_tmp253)->f1;_LL130: _tmp256=_tmp254;
goto _LL132;_LL131: if(*((int*)_tmp23E)!= 1)goto _LL133;_tmp255=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp255 <= (void*)1)goto _LL133;if(*((int*)_tmp255)!= 0)goto _LL133;
_tmp256=((struct Cyc_Absyn_Global_b_struct*)_tmp255)->f1;_LL132: _tmp258=_tmp256;
goto _LL134;_LL133: if(*((int*)_tmp23E)!= 1)goto _LL135;_tmp257=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp257 <= (void*)1)goto _LL135;if(*((int*)_tmp257)!= 2)goto _LL135;
_tmp258=((struct Cyc_Absyn_Param_b_struct*)_tmp257)->f1;_LL134: if(_tmp258->escapes)
return 0;{struct Cyc_List_List*_tmp259=relns;for(0;_tmp259 != 0;_tmp259=_tmp259->tl){
struct Cyc_CfFlowInfo_Reln*_tmp25A=(struct Cyc_CfFlowInfo_Reln*)_tmp259->hd;if(
_tmp25A->vd == _tmp258){union Cyc_CfFlowInfo_RelnOp_union _tmp25B=_tmp25A->rop;
struct Cyc_Absyn_Vardecl*_tmp25C;struct Cyc_Absyn_Vardecl*_tmp25D;unsigned int
_tmp25E;_LL138: if((_tmp25B.LessSize).tag != 2)goto _LL13A;_tmp25C=(_tmp25B.LessSize).f1;
_LL139: if(_tmp23D == _tmp25C)return 1;else{goto _LL137;}_LL13A: if((_tmp25B.LessVar).tag
!= 1)goto _LL13C;_tmp25D=(_tmp25B.LessVar).f1;_LL13B:{struct Cyc_List_List*_tmp25F=
relns;for(0;_tmp25F != 0;_tmp25F=_tmp25F->tl){struct Cyc_CfFlowInfo_Reln*_tmp260=(
struct Cyc_CfFlowInfo_Reln*)_tmp25F->hd;if(_tmp260->vd == _tmp25D){union Cyc_CfFlowInfo_RelnOp_union
_tmp261=_tmp260->rop;struct Cyc_Absyn_Vardecl*_tmp262;struct Cyc_Absyn_Vardecl*
_tmp263;unsigned int _tmp264;struct Cyc_Absyn_Vardecl*_tmp265;_LL141: if((_tmp261.LessEqSize).tag
!= 4)goto _LL143;_tmp262=(_tmp261.LessEqSize).f1;_LL142: _tmp263=_tmp262;goto
_LL144;_LL143: if((_tmp261.LessSize).tag != 2)goto _LL145;_tmp263=(_tmp261.LessSize).f1;
_LL144: if(_tmp23D == _tmp263)return 1;goto _LL140;_LL145: if((_tmp261.EqualConst).tag
!= 0)goto _LL147;_tmp264=(_tmp261.EqualConst).f1;_LL146: return Cyc_Toc_check_const_array(
_tmp264,(void*)_tmp23D->type);_LL147: if((_tmp261.LessVar).tag != 1)goto _LL149;
_tmp265=(_tmp261.LessVar).f1;_LL148: if(Cyc_Toc_check_leq_size_var(relns,_tmp23D,
_tmp265))return 1;goto _LL140;_LL149:;_LL14A: goto _LL140;_LL140:;}}}goto _LL137;
_LL13C: if((_tmp25B.LessConst).tag != 3)goto _LL13E;_tmp25E=(_tmp25B.LessConst).f1;
_LL13D: return Cyc_Toc_check_const_array(_tmp25E,(void*)_tmp23D->type);_LL13E:;
_LL13F: goto _LL137;_LL137:;}}}goto _LL122;_LL135:;_LL136: goto _LL122;_LL122:;}goto
_LL117;_LL120:;_LL121: goto _LL117;_LL117:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp266[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp267="Missing type in primop ";_tag_dynforward(_tmp267,sizeof(char),
_get_zero_arr_size(_tmp267,24));}),_tag_dynforward(_tmp266,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp268[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp269="Missing type in primop ";_tag_dynforward(_tmp269,sizeof(char),
_get_zero_arr_size(_tmp269,24));}),_tag_dynforward(_tmp268,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp26A=_cycalloc(
sizeof(*_tmp26A));_tmp26A->f1=Cyc_Toc_mt_tq;_tmp26A->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp26A;});}static struct _tuple6*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple6*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->f1=0;
_tmp26B->f2=e;_tmp26B;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp26C=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp26C):
Cyc_Toc_malloc_ptr(_tmp26C));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp26C);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp26D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp26D != 0;_tmp26D=_tmp26D->tl){struct _tuple6 _tmp26F;struct Cyc_List_List*
_tmp270;struct Cyc_Absyn_Exp*_tmp271;struct _tuple6*_tmp26E=(struct _tuple6*)
_tmp26D->hd;_tmp26F=*_tmp26E;_tmp270=_tmp26F.f1;_tmp271=_tmp26F.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp270 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp270->tl != 0)({void*_tmp272[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp273="multiple designators in array";
_tag_dynforward(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,30));}),
_tag_dynforward(_tmp272,sizeof(void*),0));});{void*_tmp274=(void*)_tmp270->hd;
void*_tmp275=_tmp274;struct Cyc_Absyn_Exp*_tmp276;_LL14C: if(*((int*)_tmp275)!= 0)
goto _LL14E;_tmp276=((struct Cyc_Absyn_ArrayElement_struct*)_tmp275)->f1;_LL14D:
Cyc_Toc_exp_to_c(nv,_tmp276);e_index=_tmp276;goto _LL14B;_LL14E: if(*((int*)
_tmp275)!= 1)goto _LL14B;_LL14F:({void*_tmp277[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp278="field name designators in array";
_tag_dynforward(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,32));}),
_tag_dynforward(_tmp277,sizeof(void*),0));});_LL14B:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp279=(void*)_tmp271->r;struct Cyc_List_List*
_tmp27A;struct Cyc_Absyn_Vardecl*_tmp27B;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*
_tmp27D;int _tmp27E;void*_tmp27F;struct Cyc_List_List*_tmp280;_LL151: if(*((int*)
_tmp279)!= 28)goto _LL153;_tmp27A=((struct Cyc_Absyn_Array_e_struct*)_tmp279)->f1;
_LL152: s=Cyc_Toc_init_array(nv,lval,_tmp27A,s);goto _LL150;_LL153: if(*((int*)
_tmp279)!= 29)goto _LL155;_tmp27B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp279)->f1;_tmp27C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp279)->f2;
_tmp27D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp279)->f3;_tmp27E=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp279)->f4;_LL154: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp27B,_tmp27C,_tmp27D,_tmp27E,s,0);goto _LL150;_LL155: if(*((int*)
_tmp279)!= 31)goto _LL157;_tmp27F=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp279)->f1;_tmp280=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp279)->f2;_LL156:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp27F,_tmp280,s);goto _LL150;_LL157:;_LL158:
Cyc_Toc_exp_to_c(nv,_tmp271);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp271,0),s,0);goto _LL150;_LL150:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp281=vd->name;
void*_tmp282=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp283=
_new_region("r2");struct _RegionHandle*r2=& _tmp283;_push_region(r2);{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(r2,nv,_tmp281,Cyc_Absyn_varb_exp(_tmp281,(void*)({struct
Cyc_Absyn_Local_b_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=({struct
Cyc_Absyn_Local_b_struct _tmp28E;_tmp28E.tag=3;_tmp28E.f1=vd;_tmp28E;});_tmp28D;}),
0));struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp281,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp281,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp281,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp281,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp284=(void*)e2->r;struct Cyc_List_List*
_tmp285;struct Cyc_Absyn_Vardecl*_tmp286;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*
_tmp288;int _tmp289;void*_tmp28A;struct Cyc_List_List*_tmp28B;_LL15A: if(*((int*)
_tmp284)!= 28)goto _LL15C;_tmp285=((struct Cyc_Absyn_Array_e_struct*)_tmp284)->f1;
_LL15B: body=Cyc_Toc_init_array(nv2,lval,_tmp285,Cyc_Toc_skip_stmt_dl());goto
_LL159;_LL15C: if(*((int*)_tmp284)!= 29)goto _LL15E;_tmp286=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp284)->f1;_tmp287=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp284)->f2;
_tmp288=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp284)->f3;_tmp289=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp284)->f4;_LL15D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp286,_tmp287,_tmp288,_tmp289,Cyc_Toc_skip_stmt_dl(),0);goto _LL159;
_LL15E: if(*((int*)_tmp284)!= 31)goto _LL160;_tmp28A=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp284)->f1;_tmp28B=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp284)->f2;_LL15F:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp28A,_tmp28B,Cyc_Toc_skip_stmt_dl());
goto _LL159;_LL160:;_LL161: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL159;_LL159:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp282,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}{struct Cyc_Absyn_Stmt*_tmp28C=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp281,
Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp28C;}}};
_pop_region(r2);}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp28F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp28F != 0;_tmp28F=_tmp28F->tl){struct _tuple6 _tmp291;
struct Cyc_List_List*_tmp292;struct Cyc_Absyn_Exp*_tmp293;struct _tuple6*_tmp290=(
struct _tuple6*)_tmp28F->hd;_tmp291=*_tmp290;_tmp292=_tmp291.f1;_tmp293=_tmp291.f2;
if(_tmp292 == 0)({void*_tmp294[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp295="empty designator list";
_tag_dynforward(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,22));}),
_tag_dynforward(_tmp294,sizeof(void*),0));});if(_tmp292->tl != 0)({void*_tmp296[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp297="too many designators in anonymous struct";_tag_dynforward(
_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,41));}),_tag_dynforward(_tmp296,
sizeof(void*),0));});{void*_tmp298=(void*)_tmp292->hd;struct _dynforward_ptr*
_tmp299;_LL163: if(*((int*)_tmp298)!= 1)goto _LL165;_tmp299=((struct Cyc_Absyn_FieldName_struct*)
_tmp298)->f1;_LL164: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp299,0);{void*_tmp29A=(void*)_tmp293->r;struct Cyc_List_List*_tmp29B;struct Cyc_Absyn_Vardecl*
_tmp29C;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29E;int _tmp29F;void*
_tmp2A0;struct Cyc_List_List*_tmp2A1;_LL168: if(*((int*)_tmp29A)!= 28)goto _LL16A;
_tmp29B=((struct Cyc_Absyn_Array_e_struct*)_tmp29A)->f1;_LL169: s=Cyc_Toc_init_array(
nv,lval,_tmp29B,s);goto _LL167;_LL16A: if(*((int*)_tmp29A)!= 29)goto _LL16C;_tmp29C=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp29A)->f1;_tmp29D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp29A)->f2;_tmp29E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp29A)->f3;
_tmp29F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp29A)->f4;_LL16B: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp29C,_tmp29D,_tmp29E,_tmp29F,s,0);goto _LL167;_LL16C: if(*((int*)
_tmp29A)!= 31)goto _LL16E;_tmp2A0=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp29A)->f1;_tmp2A1=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp29A)->f2;_LL16D:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2A0,_tmp2A1,s);goto _LL167;_LL16E:;_LL16F:
Cyc_Toc_exp_to_c(nv,_tmp293);if(Cyc_Toc_is_poly_field(struct_type,_tmp299))
_tmp293=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp293);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp293,0),0),s,0);goto _LL167;
_LL167:;}goto _LL162;}_LL165:;_LL166:({void*_tmp2A2[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2A3="array designator in struct";_tag_dynforward(_tmp2A3,sizeof(char),
_get_zero_arr_size(_tmp2A3,27));}),_tag_dynforward(_tmp2A2,sizeof(void*),0));});
_LL162:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp2A4=_new_region("r");struct _RegionHandle*r=& _tmp2A4;
_push_region(r);{struct Cyc_List_List*_tmp2A5=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _dynforward_ptr*_tmp2A6=Cyc_Toc_add_tuple_type(
_tmp2A5);struct _tuple1*_tmp2A7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2A8=
Cyc_Absyn_var_exp(_tmp2A7,0);struct Cyc_Absyn_Stmt*_tmp2A9=Cyc_Absyn_exp_stmt(
_tmp2A8,0);struct Cyc_Absyn_Exp*(*_tmp2AA)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp2AB=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2AB);for(0;_tmp2AB != 0;(_tmp2AB=
_tmp2AB->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp2AB->hd;struct
Cyc_Absyn_Exp*lval=_tmp2AA(_tmp2A8,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp2AC=(void*)e->r;struct Cyc_List_List*_tmp2AD;struct Cyc_Absyn_Vardecl*
_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B0;int _tmp2B1;
_LL171: if(*((int*)_tmp2AC)!= 28)goto _LL173;_tmp2AD=((struct Cyc_Absyn_Array_e_struct*)
_tmp2AC)->f1;_LL172: _tmp2A9=Cyc_Toc_init_array(nv,lval,_tmp2AD,_tmp2A9);goto
_LL170;_LL173: if(*((int*)_tmp2AC)!= 29)goto _LL175;_tmp2AE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2AC)->f1;_tmp2AF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AC)->f2;
_tmp2B0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2AC)->f3;_tmp2B1=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2AC)->f4;_LL174: _tmp2A9=Cyc_Toc_init_comprehension(
nv,lval,_tmp2AE,_tmp2AF,_tmp2B0,_tmp2B1,_tmp2A9,0);goto _LL170;_LL175:;_LL176: Cyc_Toc_exp_to_c(
nv,e);_tmp2A9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp2AA(
_tmp2A8,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp2A9,0);goto _LL170;_LL170:;}}}{
struct Cyc_Absyn_Exp*_tmp2B2=Cyc_Toc_make_struct(nv,_tmp2A7,Cyc_Absyn_strct(
_tmp2A6),_tmp2A9,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp2B2;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp2B3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_var_exp(_tmp2B3,0);struct Cyc_Absyn_Stmt*
_tmp2B5=Cyc_Absyn_exp_stmt(_tmp2B4,0);struct Cyc_Absyn_Exp*(*_tmp2B6)(struct Cyc_Absyn_Exp*,
struct _dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp2B7=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp2B8;union
Cyc_Absyn_AggrInfoU_union _tmp2B9;_LL178: if(_tmp2B7 <= (void*)4)goto _LL17A;if(*((
int*)_tmp2B7)!= 10)goto _LL17A;_tmp2B8=((struct Cyc_Absyn_AggrType_struct*)_tmp2B7)->f1;
_tmp2B9=_tmp2B8.aggr_info;_LL179: ad=Cyc_Absyn_get_known_aggrdecl(_tmp2B9);goto
_LL177;_LL17A:;_LL17B:({void*_tmp2BA[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2BB="init_struct: bad struct type";
_tag_dynforward(_tmp2BB,sizeof(char),_get_zero_arr_size(_tmp2BB,29));}),
_tag_dynforward(_tmp2BA,sizeof(void*),0));});_LL177:;}{struct _RegionHandle
_tmp2BC=_new_region("r");struct _RegionHandle*r=& _tmp2BC;_push_region(r);{struct
Cyc_List_List*_tmp2BD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,dles);for(0;_tmp2BD != 0;_tmp2BD=_tmp2BD->tl){struct _tuple6
_tmp2BF;struct Cyc_List_List*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2C1;struct _tuple6*
_tmp2BE=(struct _tuple6*)_tmp2BD->hd;_tmp2BF=*_tmp2BE;_tmp2C0=_tmp2BF.f1;_tmp2C1=
_tmp2BF.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2C1->topt))->v);if(_tmp2C0 == 0)({void*_tmp2C2[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2C3="empty designator list";_tag_dynforward(_tmp2C3,sizeof(char),
_get_zero_arr_size(_tmp2C3,22));}),_tag_dynforward(_tmp2C2,sizeof(void*),0));});
if(_tmp2C0->tl != 0){struct _tuple1*_tmp2C4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp2C5=Cyc_Absyn_var_exp(_tmp2C4,0);for(0;_tmp2C0 != 0;_tmp2C0=_tmp2C0->tl){void*
_tmp2C6=(void*)_tmp2C0->hd;struct _dynforward_ptr*_tmp2C7;_LL17D: if(*((int*)
_tmp2C6)!= 1)goto _LL17F;_tmp2C7=((struct Cyc_Absyn_FieldName_struct*)_tmp2C6)->f1;
_LL17E: if(Cyc_Toc_is_poly_field(struct_type,_tmp2C7))_tmp2C5=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2C5);_tmp2B5=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2B6(_tmp2B4,_tmp2C7,0),_tmp2C5,0),0),_tmp2B5,0);goto _LL17C;_LL17F:;_LL180:({
void*_tmp2C8[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp2C9="array designator in struct";
_tag_dynforward(_tmp2C9,sizeof(char),_get_zero_arr_size(_tmp2C9,27));}),
_tag_dynforward(_tmp2C8,sizeof(void*),0));});_LL17C:;}Cyc_Toc_exp_to_c(nv,
_tmp2C1);_tmp2B5=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2C5,_tmp2C1,0),0),_tmp2B5,0);}else{void*_tmp2CA=(void*)_tmp2C0->hd;struct
_dynforward_ptr*_tmp2CB;_LL182: if(*((int*)_tmp2CA)!= 1)goto _LL184;_tmp2CB=((
struct Cyc_Absyn_FieldName_struct*)_tmp2CA)->f1;_LL183: {struct Cyc_Absyn_Exp*lval=
_tmp2B6(_tmp2B4,_tmp2CB,0);{void*_tmp2CC=(void*)_tmp2C1->r;struct Cyc_List_List*
_tmp2CD;struct Cyc_Absyn_Vardecl*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Exp*
_tmp2D0;int _tmp2D1;void*_tmp2D2;struct Cyc_List_List*_tmp2D3;_LL187: if(*((int*)
_tmp2CC)!= 28)goto _LL189;_tmp2CD=((struct Cyc_Absyn_Array_e_struct*)_tmp2CC)->f1;
_LL188: _tmp2B5=Cyc_Toc_init_array(nv,lval,_tmp2CD,_tmp2B5);goto _LL186;_LL189: if(*((
int*)_tmp2CC)!= 29)goto _LL18B;_tmp2CE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2CC)->f1;_tmp2CF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2CC)->f2;
_tmp2D0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2CC)->f3;_tmp2D1=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2CC)->f4;_LL18A: _tmp2B5=Cyc_Toc_init_comprehension(
nv,lval,_tmp2CE,_tmp2CF,_tmp2D0,_tmp2D1,_tmp2B5,0);goto _LL186;_LL18B: if(*((int*)
_tmp2CC)!= 31)goto _LL18D;_tmp2D2=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2CC)->f1;_tmp2D3=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2CC)->f2;_LL18C:
_tmp2B5=Cyc_Toc_init_anon_struct(nv,lval,_tmp2D2,_tmp2D3,_tmp2B5);goto _LL186;
_LL18D:;_LL18E: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2C1->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp2C1);{struct Cyc_Absyn_Aggrfield*
_tmp2D4=Cyc_Absyn_lookup_decl_field(ad,_tmp2CB);if(Cyc_Toc_is_poly_field(
struct_type,_tmp2CB) && !was_ptr_type)_tmp2C1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2C1);if(has_exists)_tmp2C1=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp2D4))->type,_tmp2C1);_tmp2B5=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp2C1,0),0),_tmp2B5,0);goto _LL186;}}_LL186:;}goto
_LL181;}_LL184:;_LL185:({void*_tmp2D5[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D6="array designator in struct";
_tag_dynforward(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,27));}),
_tag_dynforward(_tmp2D5,sizeof(void*),0));});_LL181:;}}}{struct Cyc_Absyn_Exp*
_tmp2D7=Cyc_Toc_make_struct(nv,_tmp2B3,Cyc_Absyn_strctq(tdn),_tmp2B5,pointer,
rgnopt,is_atomic);_npop_handler(0);return _tmp2D7;};_pop_region(r);}}struct
_tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple8*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));
_tmp2D8[0]=({struct Cyc_Absyn_Increment_e_struct _tmp2D9;_tmp2D9.tag=5;_tmp2D9.f1=
e1;_tmp2D9.f2=(void*)incr;_tmp2D9;});_tmp2D8;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp2DA=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp2DB;void*_tmp2DC;struct Cyc_Absyn_Exp*
_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;struct _dynforward_ptr*_tmp2DF;_LL190: if(*((
int*)_tmp2DA)!= 37)goto _LL192;_tmp2DB=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp2DA)->f1;_LL191: Cyc_Toc_lvalue_assign_stmt(_tmp2DB,fs,f,f_env);goto _LL18F;
_LL192: if(*((int*)_tmp2DA)!= 15)goto _LL194;_tmp2DC=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2DA)->f1;_tmp2DD=((struct Cyc_Absyn_Cast_e_struct*)_tmp2DA)->f2;_LL193: Cyc_Toc_lvalue_assign(
_tmp2DD,fs,f,f_env);goto _LL18F;_LL194: if(*((int*)_tmp2DA)!= 23)goto _LL196;
_tmp2DE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2DA)->f1;_tmp2DF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2DA)->f2;_LL195:(void*)(e1->r=(void*)((void*)_tmp2DE->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp2E0=_cycalloc(sizeof(*_tmp2E0));
_tmp2E0->hd=_tmp2DF;_tmp2E0->tl=fs;_tmp2E0;}),f,f_env);goto _LL18F;_LL196:;_LL197: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL18F;}_LL18F:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp2E1=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Decl*_tmp2E3;struct Cyc_Absyn_Stmt*
_tmp2E4;struct Cyc_Absyn_Stmt*_tmp2E5;_LL199: if(_tmp2E1 <= (void*)1)goto _LL19F;if(*((
int*)_tmp2E1)!= 0)goto _LL19B;_tmp2E2=((struct Cyc_Absyn_Exp_s_struct*)_tmp2E1)->f1;
_LL19A: Cyc_Toc_lvalue_assign(_tmp2E2,fs,f,f_env);goto _LL198;_LL19B: if(*((int*)
_tmp2E1)!= 11)goto _LL19D;_tmp2E3=((struct Cyc_Absyn_Decl_s_struct*)_tmp2E1)->f1;
_tmp2E4=((struct Cyc_Absyn_Decl_s_struct*)_tmp2E1)->f2;_LL19C: Cyc_Toc_lvalue_assign_stmt(
_tmp2E4,fs,f,f_env);goto _LL198;_LL19D: if(*((int*)_tmp2E1)!= 1)goto _LL19F;_tmp2E5=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2E1)->f2;_LL19E: Cyc_Toc_lvalue_assign_stmt(
_tmp2E5,fs,f,f_env);goto _LL198;_LL19F:;_LL1A0:({struct Cyc_String_pa_struct
_tmp2E8;_tmp2E8.tag=0;_tmp2E8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_stmt2string(s));{void*_tmp2E6[1]={& _tmp2E8};Cyc_Toc_toc_impos(({
const char*_tmp2E7="lvalue_assign_stmt: %s";_tag_dynforward(_tmp2E7,sizeof(char),
_get_zero_arr_size(_tmp2E7,23));}),_tag_dynforward(_tmp2E6,sizeof(void*),1));}});
_LL198:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmp2E9=
_region_malloc(r2,sizeof(*_tmp2E9));_tmp2E9->hd=(void*)f((void*)x->hd,env);
_tmp2E9->tl=0;_tmp2E9;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp2EA=_region_malloc(r2,sizeof(*
_tmp2EA));_tmp2EA->hd=(void*)f((void*)x->hd,env);_tmp2EA->tl=0;_tmp2EA;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple6*_tmp2EB=_cycalloc(sizeof(*_tmp2EB));_tmp2EB->f1=0;
_tmp2EB->f2=e;_tmp2EB;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp2EC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2ED;_LL1A2: if(
_tmp2EC <= (void*)4)goto _LL1A4;if(*((int*)_tmp2EC)!= 4)goto _LL1A4;_tmp2ED=((
struct Cyc_Absyn_PointerType_struct*)_tmp2EC)->f1;_LL1A3: return _tmp2ED;_LL1A4:;
_LL1A5:({void*_tmp2EE[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2EF="get_ptr_typ: not a pointer!";
_tag_dynforward(_tmp2EF,sizeof(char),_get_zero_arr_size(_tmp2EF,28));}),
_tag_dynforward(_tmp2EE,sizeof(void*),0));});_LL1A1:;}static int Cyc_Toc_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*
_tmp2F0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2F1;void*_tmp2F2;
struct Cyc_Absyn_PtrAtts _tmp2F3;struct Cyc_Absyn_Conref*_tmp2F4;struct Cyc_Absyn_Conref*
_tmp2F5;struct Cyc_Absyn_ArrayInfo _tmp2F6;void*_tmp2F7;struct Cyc_Absyn_Tqual
_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;struct Cyc_Absyn_Conref*_tmp2FA;_LL1A7: if(
_tmp2F0 <= (void*)4)goto _LL1AB;if(*((int*)_tmp2F0)!= 4)goto _LL1A9;_tmp2F1=((
struct Cyc_Absyn_PointerType_struct*)_tmp2F0)->f1;_tmp2F2=(void*)_tmp2F1.elt_typ;
_tmp2F3=_tmp2F1.ptr_atts;_tmp2F4=_tmp2F3.bounds;_tmp2F5=_tmp2F3.zero_term;_LL1A8:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2F5)){*
ptr_type=t;*elt_type=_tmp2F2;{void*_tmp2FB=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2F4);_LL1AE: if((int)_tmp2FB != 0)goto _LL1B0;_LL1AF:*is_dyneither=0;*
is_dynforward=1;goto _LL1AD;_LL1B0: if((int)_tmp2FB != 1)goto _LL1B2;_LL1B1:*
is_dynforward=0;*is_dyneither=1;goto _LL1AD;_LL1B2:;_LL1B3:*is_dynforward=(*
is_dyneither=0);goto _LL1AD;_LL1AD:;}return 1;}else{return 0;}_LL1A9: if(*((int*)
_tmp2F0)!= 7)goto _LL1AB;_tmp2F6=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F0)->f1;
_tmp2F7=(void*)_tmp2F6.elt_type;_tmp2F8=_tmp2F6.tq;_tmp2F9=_tmp2F6.num_elts;
_tmp2FA=_tmp2F6.zero_term;_LL1AA: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp2FA)){*elt_type=_tmp2F7;*is_dynforward=(*is_dyneither=0);*ptr_type=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC[0]=({
struct Cyc_Absyn_PointerType_struct _tmp2FD;_tmp2FD.tag=4;_tmp2FD.f1=({struct Cyc_Absyn_PtrInfo
_tmp2FE;_tmp2FE.elt_typ=(void*)_tmp2F7;_tmp2FE.elt_tq=_tmp2F8;_tmp2FE.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp2FF;_tmp2FF.rgn=(void*)((void*)2);_tmp2FF.nullable=
Cyc_Absyn_false_conref;_tmp2FF.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp301;_tmp301.tag=0;_tmp301.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp2F9);
_tmp301;});_tmp300;}));_tmp2FF.zero_term=_tmp2FA;_tmp2FF.ptrloc=0;_tmp2FF;});
_tmp2FE;});_tmp2FD;});_tmp2FC;});return 1;}else{return 0;}_LL1AB:;_LL1AC: return 0;
_LL1A6:;}static int Cyc_Toc_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmp302=(void*)
e1->r;struct Cyc_Absyn_Exp*_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*
_tmp305;struct Cyc_Absyn_Exp*_tmp306;struct Cyc_Absyn_Exp*_tmp307;struct Cyc_Absyn_Exp*
_tmp308;_LL1B5: if(*((int*)_tmp302)!= 15)goto _LL1B7;_LL1B6:({struct Cyc_String_pa_struct
_tmp30B;_tmp30B.tag=0;_tmp30B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp309[1]={& _tmp30B};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp30A="we have a cast in a lhs:  %s";_tag_dynforward(_tmp30A,sizeof(char),
_get_zero_arr_size(_tmp30A,29));}),_tag_dynforward(_tmp309,sizeof(void*),1));}});
_LL1B7: if(*((int*)_tmp302)!= 22)goto _LL1B9;_tmp303=((struct Cyc_Absyn_Deref_e_struct*)
_tmp302)->f1;_LL1B8: _tmp304=_tmp303;goto _LL1BA;_LL1B9: if(*((int*)_tmp302)!= 25)
goto _LL1BB;_tmp304=((struct Cyc_Absyn_Subscript_e_struct*)_tmp302)->f1;_LL1BA:
return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp304->topt))->v,
ptr_type,is_dynforward,is_dyneither,elt_type);_LL1BB: if(*((int*)_tmp302)!= 24)
goto _LL1BD;_tmp305=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp302)->f1;_LL1BC:
_tmp306=_tmp305;goto _LL1BE;_LL1BD: if(*((int*)_tmp302)!= 23)goto _LL1BF;_tmp306=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp302)->f1;_LL1BE: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp306->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp30E;_tmp30E.tag=
0;_tmp30E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp30C[1]={& _tmp30E};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp30D="found zero pointer aggregate member assignment: %s";
_tag_dynforward(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,51));}),
_tag_dynforward(_tmp30C,sizeof(void*),1));}});return 0;_LL1BF: if(*((int*)_tmp302)
!= 14)goto _LL1C1;_tmp307=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp302)->f1;
_LL1C0: _tmp308=_tmp307;goto _LL1C2;_LL1C1: if(*((int*)_tmp302)!= 13)goto _LL1C3;
_tmp308=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp302)->f1;_LL1C2: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp308->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp311;_tmp311.tag=
0;_tmp311.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp30F[1]={& _tmp311};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp310="found zero pointer instantiate/noinstantiate: %s";
_tag_dynforward(_tmp310,sizeof(char),_get_zero_arr_size(_tmp310,49));}),
_tag_dynforward(_tmp30F,sizeof(void*),1));}});return 0;_LL1C3: if(*((int*)_tmp302)
!= 1)goto _LL1C5;_LL1C4: return 0;_LL1C5:;_LL1C6:({struct Cyc_String_pa_struct
_tmp314;_tmp314.tag=0;_tmp314.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp312[1]={& _tmp314};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp313="found bad lhs in is_zero_ptr_deref: %s";_tag_dynforward(_tmp313,sizeof(
char),_get_zero_arr_size(_tmp313,39));}),_tag_dynforward(_tmp312,sizeof(void*),1));}});
_LL1B4:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp315=Cyc_Tcutil_compress(t);void*_tmp316;void*_tmp317;void*_tmp318;
void*_tmp319;void*_tmp31A;void*_tmp31B;void*_tmp31C;void*_tmp31D;void*_tmp31E;
void*_tmp31F;_LL1C8: if(_tmp315 <= (void*)4)goto _LL1D8;if(*((int*)_tmp315)!= 4)
goto _LL1CA;_LL1C9: res=Cyc_Absyn_null_exp(0);goto _LL1C7;_LL1CA: if(*((int*)_tmp315)
!= 5)goto _LL1CC;_tmp316=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f1;
_tmp317=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f2;if((int)_tmp317 != 
0)goto _LL1CC;_LL1CB: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp320;(_tmp320.Char_c).tag=0;(_tmp320.Char_c).f1=(void*)
_tmp316;(_tmp320.Char_c).f2='\000';_tmp320;}),0);goto _LL1C7;_LL1CC: if(*((int*)
_tmp315)!= 5)goto _LL1CE;_tmp318=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f1;
_tmp319=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f2;if((int)_tmp319 != 
1)goto _LL1CE;_LL1CD: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp321;(_tmp321.Short_c).tag=1;(_tmp321.Short_c).f1=(void*)
_tmp318;(_tmp321.Short_c).f2=0;_tmp321;}),0);goto _LL1C7;_LL1CE: if(*((int*)
_tmp315)!= 12)goto _LL1D0;_LL1CF: goto _LL1D1;_LL1D0: if(*((int*)_tmp315)!= 13)goto
_LL1D2;_LL1D1: _tmp31A=(void*)1;goto _LL1D3;_LL1D2: if(*((int*)_tmp315)!= 5)goto
_LL1D4;_tmp31A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f1;_tmp31B=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f2;if((int)_tmp31B != 2)goto
_LL1D4;_LL1D3: _tmp31C=_tmp31A;goto _LL1D5;_LL1D4: if(*((int*)_tmp315)!= 5)goto
_LL1D6;_tmp31C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f1;_tmp31D=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f2;if((int)_tmp31D != 3)goto
_LL1D6;_LL1D5: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp322;(_tmp322.Int_c).tag=2;(_tmp322.Int_c).f1=(void*)_tmp31C;(_tmp322.Int_c).f2=
0;_tmp322;}),0);goto _LL1C7;_LL1D6: if(*((int*)_tmp315)!= 5)goto _LL1D8;_tmp31E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp315)->f1;_tmp31F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp315)->f2;if((int)_tmp31F != 4)goto _LL1D8;_LL1D7: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp323;(_tmp323.LongLong_c).tag=
3;(_tmp323.LongLong_c).f1=(void*)_tmp31E;(_tmp323.LongLong_c).f2=(long long)0;
_tmp323;}),0);goto _LL1C7;_LL1D8: if((int)_tmp315 != 1)goto _LL1DA;_LL1D9: goto _LL1DB;
_LL1DA: if(_tmp315 <= (void*)4)goto _LL1DC;if(*((int*)_tmp315)!= 6)goto _LL1DC;
_LL1DB: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp324;(_tmp324.Float_c).tag=4;(_tmp324.Float_c).f1=({const char*_tmp325="0.0";
_tag_dynforward(_tmp325,sizeof(char),_get_zero_arr_size(_tmp325,4));});_tmp324;}),
0);goto _LL1C7;_LL1DC:;_LL1DD:({struct Cyc_String_pa_struct _tmp328;_tmp328.tag=0;
_tmp328.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp326[1]={& _tmp328};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp327="found non-zero type %s in generate_zero";
_tag_dynforward(_tmp327,sizeof(char),_get_zero_arr_size(_tmp327,40));}),
_tag_dynforward(_tmp326,sizeof(void*),1));}});_LL1C7:;}res->topt=({struct Cyc_Core_Opt*
_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->v=(void*)t;_tmp329;});return res;}
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dynforward,int is_dyneither,void*elt_type){int is_fat=is_dynforward
 || is_dyneither;void*fat_ptr_type;if(is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp32A=Cyc_Toc_typ_to_c(
elt_type);void*_tmp32B=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp32C=Cyc_Absyn_cstar_typ(
_tmp32A,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp32D=({struct Cyc_Core_Opt*_tmp36B=
_cycalloc(sizeof(*_tmp36B));_tmp36B->v=(void*)_tmp32C;_tmp36B;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp32E=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Exp*
_tmp330;struct Cyc_Absyn_Exp*_tmp331;_LL1DF: if(*((int*)_tmp32E)!= 22)goto _LL1E1;
_tmp32F=((struct Cyc_Absyn_Deref_e_struct*)_tmp32E)->f1;_LL1E0: if(!is_fat){
_tmp32F=Cyc_Toc_cast_it(fat_ptr_type,_tmp32F);_tmp32F->topt=({struct Cyc_Core_Opt*
_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332->v=(void*)fat_ptr_type;_tmp332;});}
Cyc_Toc_exp_to_c(nv,_tmp32F);xinit=_tmp32F;goto _LL1DE;_LL1E1: if(*((int*)_tmp32E)
!= 25)goto _LL1E3;_tmp330=((struct Cyc_Absyn_Subscript_e_struct*)_tmp32E)->f1;
_tmp331=((struct Cyc_Absyn_Subscript_e_struct*)_tmp32E)->f2;_LL1E2: if(!is_fat){
_tmp330=Cyc_Toc_cast_it(fat_ptr_type,_tmp330);_tmp330->topt=({struct Cyc_Core_Opt*
_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333->v=(void*)fat_ptr_type;_tmp333;});}
Cyc_Toc_exp_to_c(nv,_tmp330);Cyc_Toc_exp_to_c(nv,_tmp331);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp334[3];_tmp334[2]=_tmp331;_tmp334[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp334[0]=_tmp330;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp334,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp335[3];_tmp335[2]=_tmp331;_tmp335[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp335[0]=_tmp330;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp335,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1DE;_LL1E3:;_LL1E4:({void*_tmp336[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp337="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp337,sizeof(char),_get_zero_arr_size(_tmp337,53));}),
_tag_dynforward(_tmp336,sizeof(void*),0));});_LL1DE:;}{struct _tuple1*_tmp338=Cyc_Toc_temp_var();
struct _RegionHandle _tmp339=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp339;
_push_region(rgn2);{struct Cyc_Toc_Env*_tmp33A=Cyc_Toc_add_varmap(rgn2,nv,_tmp338,
Cyc_Absyn_var_exp(_tmp338,0));struct Cyc_Absyn_Vardecl*_tmp33B=({struct Cyc_Absyn_Vardecl*
_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->sc=(void*)((void*)2);_tmp36A->name=
_tmp338;_tmp36A->tq=Cyc_Toc_mt_tq;_tmp36A->type=(void*)_tmp32B;_tmp36A->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp36A->rgn=0;_tmp36A->attributes=0;_tmp36A->escapes=
0;_tmp36A;});struct Cyc_Absyn_Local_b_struct*_tmp33C=({struct Cyc_Absyn_Local_b_struct*
_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368[0]=({struct Cyc_Absyn_Local_b_struct
_tmp369;_tmp369.tag=3;_tmp369.f1=_tmp33B;_tmp369;});_tmp368;});struct Cyc_Absyn_Exp*
_tmp33D=Cyc_Absyn_varb_exp(_tmp338,(void*)_tmp33C,0);_tmp33D->topt=({struct Cyc_Core_Opt*
_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->v=(void*)fat_ptr_type;_tmp33E;});{
struct Cyc_Absyn_Exp*_tmp33F=Cyc_Absyn_deref_exp(_tmp33D,0);_tmp33F->topt=({
struct Cyc_Core_Opt*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340->v=(void*)
elt_type;_tmp340;});Cyc_Toc_exp_to_c(_tmp33A,_tmp33F);{struct _tuple1*_tmp341=Cyc_Toc_temp_var();
_tmp33A=Cyc_Toc_add_varmap(rgn2,_tmp33A,_tmp341,Cyc_Absyn_var_exp(_tmp341,0));{
struct Cyc_Absyn_Vardecl*_tmp342=({struct Cyc_Absyn_Vardecl*_tmp367=_cycalloc(
sizeof(*_tmp367));_tmp367->sc=(void*)((void*)2);_tmp367->name=_tmp341;_tmp367->tq=
Cyc_Toc_mt_tq;_tmp367->type=(void*)_tmp32A;_tmp367->initializer=(struct Cyc_Absyn_Exp*)
_tmp33F;_tmp367->rgn=0;_tmp367->attributes=0;_tmp367->escapes=0;_tmp367;});
struct Cyc_Absyn_Local_b_struct*_tmp343=({struct Cyc_Absyn_Local_b_struct*_tmp365=
_cycalloc(sizeof(*_tmp365));_tmp365[0]=({struct Cyc_Absyn_Local_b_struct _tmp366;
_tmp366.tag=3;_tmp366.f1=_tmp342;_tmp366;});_tmp365;});struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp344=Cyc_Absyn_varb_exp(_tmp341,(
void*)_tmp343,0);_tmp344->topt=_tmp33F->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp344,e2,0);z_init->topt=_tmp344->topt;}Cyc_Toc_exp_to_c(_tmp33A,
z_init);{struct _tuple1*_tmp345=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp346=({struct Cyc_Absyn_Vardecl*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364->sc=(
void*)((void*)2);_tmp364->name=_tmp345;_tmp364->tq=Cyc_Toc_mt_tq;_tmp364->type=(
void*)_tmp32A;_tmp364->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp364->rgn=0;
_tmp364->attributes=0;_tmp364->escapes=0;_tmp364;});struct Cyc_Absyn_Local_b_struct*
_tmp347=({struct Cyc_Absyn_Local_b_struct*_tmp362=_cycalloc(sizeof(*_tmp362));
_tmp362[0]=({struct Cyc_Absyn_Local_b_struct _tmp363;_tmp363.tag=3;_tmp363.f1=
_tmp346;_tmp363;});_tmp362;});_tmp33A=Cyc_Toc_add_varmap(rgn2,_tmp33A,_tmp345,
Cyc_Absyn_var_exp(_tmp345,0));{struct Cyc_Absyn_Exp*_tmp348=Cyc_Absyn_varb_exp(
_tmp341,(void*)_tmp343,0);_tmp348->topt=_tmp33F->topt;{struct Cyc_Absyn_Exp*
_tmp349=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp34A=Cyc_Absyn_prim2_exp((
void*)5,_tmp348,_tmp349,0);_tmp34A->topt=({struct Cyc_Core_Opt*_tmp34B=_cycalloc(
sizeof(*_tmp34B));_tmp34B->v=(void*)Cyc_Absyn_sint_typ;_tmp34B;});Cyc_Toc_exp_to_c(
_tmp33A,_tmp34A);{struct Cyc_Absyn_Exp*_tmp34C=Cyc_Absyn_varb_exp(_tmp345,(void*)
_tmp347,0);_tmp34C->topt=_tmp33F->topt;{struct Cyc_Absyn_Exp*_tmp34D=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp34E=Cyc_Absyn_prim2_exp((void*)6,_tmp34C,
_tmp34D,0);_tmp34E->topt=({struct Cyc_Core_Opt*_tmp34F=_cycalloc(sizeof(*_tmp34F));
_tmp34F->v=(void*)Cyc_Absyn_sint_typ;_tmp34F;});Cyc_Toc_exp_to_c(_tmp33A,_tmp34E);{
struct Cyc_List_List*_tmp350=({struct Cyc_Absyn_Exp*_tmp361[2];_tmp361[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp361[0]=Cyc_Absyn_varb_exp(_tmp338,(void*)
_tmp33C,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp361,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp351=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;if(is_dynforward)xsize=
Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,
_tmp350,0),_tmp351,0);else{xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dyneither_size_e,_tmp350,0),_tmp351,0);}{struct Cyc_Absyn_Exp*
_tmp352=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp34A,_tmp34E,0),0);struct
Cyc_Absyn_Stmt*_tmp353=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,
0,0),0);struct Cyc_Absyn_Exp*_tmp354=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(
_tmp338,(void*)_tmp33C,0),Cyc_Toc_curr_sp,0);_tmp354=Cyc_Toc_cast_it(_tmp32C,
_tmp354);{struct Cyc_Absyn_Exp*_tmp355=Cyc_Absyn_deref_exp(_tmp354,0);struct Cyc_Absyn_Exp*
_tmp356=Cyc_Absyn_assign_exp(_tmp355,Cyc_Absyn_var_exp(_tmp345,0),0);struct Cyc_Absyn_Stmt*
_tmp357=Cyc_Absyn_exp_stmt(_tmp356,0);_tmp357=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp352,_tmp353,Cyc_Absyn_skip_stmt(0),0),_tmp357,0);_tmp357=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359[
0]=({struct Cyc_Absyn_Var_d_struct _tmp35A;_tmp35A.tag=0;_tmp35A.f1=_tmp346;
_tmp35A;});_tmp359;}));_tmp358->loc=0;_tmp358;}),_tmp357,0);_tmp357=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C[
0]=({struct Cyc_Absyn_Var_d_struct _tmp35D;_tmp35D.tag=0;_tmp35D.f1=_tmp342;
_tmp35D;});_tmp35C;}));_tmp35B->loc=0;_tmp35B;}),_tmp357,0);_tmp357=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp35E=_cycalloc(sizeof(*_tmp35E));_tmp35E->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F[
0]=({struct Cyc_Absyn_Var_d_struct _tmp360;_tmp360.tag=0;_tmp360.f1=_tmp33B;
_tmp360;});_tmp35F;}));_tmp35E->loc=0;_tmp35E;}),_tmp357,0);(void*)(e->r=(void*)
Cyc_Toc_stmt_exp_r(_tmp357));}}}}}}}}}}}};_pop_region(rgn2);}}}struct _tuple9{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp36C=(void*)e->r;if(e->topt == 0)({struct Cyc_String_pa_struct
_tmp36F;_tmp36F.tag=0;_tmp36F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp36D[1]={& _tmp36F};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp36E="exp_to_c: no type for %s";_tag_dynforward(_tmp36E,sizeof(char),
_get_zero_arr_size(_tmp36E,25));}),_tag_dynforward(_tmp36D,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp370=
_tmp36C;union Cyc_Absyn_Cnst_union _tmp371;struct _tuple1*_tmp372;void*_tmp373;
struct _tuple1*_tmp374;void*_tmp375;struct Cyc_List_List*_tmp376;struct Cyc_Absyn_Exp*
_tmp377;void*_tmp378;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Core_Opt*_tmp37A;
struct Cyc_Absyn_Exp*_tmp37B;struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_Absyn_Exp*
_tmp37D;struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Exp*_tmp37F;struct Cyc_Absyn_Exp*
_tmp380;struct Cyc_Absyn_Exp*_tmp381;struct Cyc_Absyn_Exp*_tmp382;struct Cyc_Absyn_Exp*
_tmp383;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_Absyn_Exp*_tmp385;struct Cyc_List_List*
_tmp386;struct Cyc_Absyn_Exp*_tmp387;struct Cyc_List_List*_tmp388;struct Cyc_Absyn_VarargCallInfo*
_tmp389;struct Cyc_Absyn_Exp*_tmp38A;struct Cyc_List_List*_tmp38B;struct Cyc_Absyn_VarargCallInfo*
_tmp38C;struct Cyc_Absyn_VarargCallInfo _tmp38D;int _tmp38E;struct Cyc_List_List*
_tmp38F;struct Cyc_Absyn_VarargInfo*_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Exp*
_tmp392;struct Cyc_Absyn_Exp*_tmp393;struct Cyc_List_List*_tmp394;void*_tmp395;
void**_tmp396;struct Cyc_Absyn_Exp*_tmp397;int _tmp398;void*_tmp399;struct Cyc_Absyn_Exp*
_tmp39A;struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;struct Cyc_Absyn_Exp*
_tmp39D;void*_tmp39E;void*_tmp39F;void*_tmp3A0;struct _dynforward_ptr*_tmp3A1;
void*_tmp3A2;void*_tmp3A3;unsigned int _tmp3A4;struct Cyc_Absyn_Exp*_tmp3A5;struct
Cyc_Absyn_Exp*_tmp3A6;struct _dynforward_ptr*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A8;
struct _dynforward_ptr*_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Exp*
_tmp3AB;struct Cyc_List_List*_tmp3AC;struct Cyc_List_List*_tmp3AD;struct Cyc_Absyn_Vardecl*
_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;int _tmp3B1;
struct _tuple1*_tmp3B2;struct Cyc_List_List*_tmp3B3;struct Cyc_List_List*_tmp3B4;
struct Cyc_Absyn_Aggrdecl*_tmp3B5;void*_tmp3B6;struct Cyc_List_List*_tmp3B7;struct
Cyc_List_List*_tmp3B8;struct Cyc_Absyn_Tuniondecl*_tmp3B9;struct Cyc_Absyn_Tunionfield*
_tmp3BA;struct Cyc_List_List*_tmp3BB;struct Cyc_Absyn_Tuniondecl*_tmp3BC;struct Cyc_Absyn_Tunionfield*
_tmp3BD;struct Cyc_Absyn_MallocInfo _tmp3BE;int _tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;
void**_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;int _tmp3C3;struct Cyc_Absyn_Stmt*
_tmp3C4;_LL1E6: if(*((int*)_tmp370)!= 0)goto _LL1E8;_tmp371=((struct Cyc_Absyn_Const_e_struct*)
_tmp370)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp370)->f1).Null_c).tag != 
6)goto _LL1E8;_LL1E7: {struct Cyc_Absyn_Exp*_tmp3C5=Cyc_Absyn_uint_exp(0,0);int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ,& forward_only)){if(
Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(
old_typ,_tmp3C5,_tmp3C5,forward_only))->r));else{if(forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp3C6[3];_tmp3C6[2]=_tmp3C5;_tmp3C6[1]=_tmp3C5;_tmp3C6[0]=_tmp3C5;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3C6,sizeof(struct Cyc_Absyn_Exp*),
3));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp3C7[3];_tmp3C7[2]=_tmp3C5;_tmp3C7[1]=_tmp3C5;_tmp3C7[0]=
_tmp3C5;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3C7,sizeof(struct Cyc_Absyn_Exp*),3));})));}}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1E5;}_LL1E8: if(*((int*)_tmp370)!= 0)
goto _LL1EA;_LL1E9: goto _LL1E5;_LL1EA: if(*((int*)_tmp370)!= 1)goto _LL1EC;_tmp372=((
struct Cyc_Absyn_Var_e_struct*)_tmp370)->f1;_tmp373=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp370)->f2;_LL1EB:{struct _handler_cons _tmp3C8;_push_handler(& _tmp3C8);{int
_tmp3CA=0;if(setjmp(_tmp3C8.handler))_tmp3CA=1;if(!_tmp3CA){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp372))->r));;_pop_handler();}else{void*
_tmp3C9=(void*)_exn_thrown;void*_tmp3CC=_tmp3C9;_LL23B: if(_tmp3CC != Cyc_Dict_Absent)
goto _LL23D;_LL23C:({struct Cyc_String_pa_struct _tmp3CF;_tmp3CF.tag=0;_tmp3CF.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp372));{
void*_tmp3CD[1]={& _tmp3CF};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3CE="Can't find %s in exp_to_c, Var\n";
_tag_dynforward(_tmp3CE,sizeof(char),_get_zero_arr_size(_tmp3CE,32));}),
_tag_dynforward(_tmp3CD,sizeof(void*),1));}});_LL23D:;_LL23E:(void)_throw(
_tmp3CC);_LL23A:;}}}goto _LL1E5;_LL1EC: if(*((int*)_tmp370)!= 2)goto _LL1EE;_tmp374=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp370)->f1;_LL1ED:({void*_tmp3D0[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3D1="unknownid";_tag_dynforward(_tmp3D1,sizeof(char),
_get_zero_arr_size(_tmp3D1,10));}),_tag_dynforward(_tmp3D0,sizeof(void*),0));});
_LL1EE: if(*((int*)_tmp370)!= 3)goto _LL1F0;_tmp375=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp370)->f1;_tmp376=((struct Cyc_Absyn_Primop_e_struct*)_tmp370)->f2;_LL1EF: {
struct Cyc_List_List*_tmp3D2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp376);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp376);{void*_tmp3D3=_tmp375;_LL240:
if((int)_tmp3D3 != 19)goto _LL242;_LL241: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp376))->hd;{void*_tmp3D4=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_PtrInfo _tmp3D7;void*_tmp3D8;
struct Cyc_Absyn_PtrAtts _tmp3D9;struct Cyc_Absyn_Conref*_tmp3DA;struct Cyc_Absyn_Conref*
_tmp3DB;struct Cyc_Absyn_Conref*_tmp3DC;_LL255: if(_tmp3D4 <= (void*)4)goto _LL259;
if(*((int*)_tmp3D4)!= 7)goto _LL257;_tmp3D5=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3D4)->f1;_tmp3D6=_tmp3D5.num_elts;_LL256:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp3D6))->r));goto _LL254;_LL257: if(*((int*)
_tmp3D4)!= 4)goto _LL259;_tmp3D7=((struct Cyc_Absyn_PointerType_struct*)_tmp3D4)->f1;
_tmp3D8=(void*)_tmp3D7.elt_typ;_tmp3D9=_tmp3D7.ptr_atts;_tmp3DA=_tmp3D9.nullable;
_tmp3DB=_tmp3D9.bounds;_tmp3DC=_tmp3D9.zero_term;_LL258:{void*_tmp3DD=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3DB);struct Cyc_Absyn_Exp*_tmp3DE;_LL25C: if((int)_tmp3DD
!= 0)goto _LL25E;_LL25D:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dynforward_size_e,({
struct Cyc_Absyn_Exp*_tmp3DF[2];_tmp3DF[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3D8),0);_tmp3DF[0]=(struct Cyc_Absyn_Exp*)_tmp376->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3DF,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL25B;_LL25E: if((int)_tmp3DD != 1)goto _LL260;_LL25F:(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*
_tmp3E0[2];_tmp3E0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3D8),0);
_tmp3E0[0]=(struct Cyc_Absyn_Exp*)_tmp376->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3E0,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL25B;_LL260: if(_tmp3DD <= (void*)2)goto _LL262;if(*((int*)_tmp3DD)
!= 0)goto _LL262;_tmp3DE=((struct Cyc_Absyn_Upper_b_struct*)_tmp3DD)->f1;_LL261:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3DC))(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp3E1[2];_tmp3E1[1]=_tmp3DE;_tmp3E1[0]=(struct Cyc_Absyn_Exp*)_tmp376->hd;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3E1,sizeof(struct Cyc_Absyn_Exp*),2));})));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp3DA))(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp3DE,Cyc_Absyn_uint_exp(0,0)));else{(void*)(e->r=(void*)((void*)_tmp3DE->r));
goto _LL25B;}}goto _LL25B;_LL262: if(_tmp3DD <= (void*)2)goto _LL25B;if(*((int*)
_tmp3DD)!= 1)goto _LL25B;_LL263:({void*_tmp3E2[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3E3="toc: size of AbsUpper_b";
_tag_dynforward(_tmp3E3,sizeof(char),_get_zero_arr_size(_tmp3E3,24));}),
_tag_dynforward(_tmp3E2,sizeof(void*),0));});_LL25B:;}goto _LL254;_LL259:;_LL25A:({
struct Cyc_String_pa_struct _tmp3E7;_tmp3E7.tag=0;_tmp3E7.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct _tmp3E6;
_tmp3E6.tag=0;_tmp3E6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp3E4[2]={&
_tmp3E6,& _tmp3E7};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3E5="size primop applied to non-array %s (%s)";_tag_dynforward(
_tmp3E5,sizeof(char),_get_zero_arr_size(_tmp3E5,41));}),_tag_dynforward(_tmp3E4,
sizeof(void*),2));}}});_LL254:;}goto _LL23F;}_LL242: if((int)_tmp3D3 != 0)goto
_LL244;_LL243:{void*_tmp3E8=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)
_check_null(_tmp3D2))->hd);struct Cyc_Absyn_PtrInfo _tmp3E9;void*_tmp3EA;struct Cyc_Absyn_PtrAtts
_tmp3EB;struct Cyc_Absyn_Conref*_tmp3EC;struct Cyc_Absyn_Conref*_tmp3ED;_LL265: if(
_tmp3E8 <= (void*)4)goto _LL267;if(*((int*)_tmp3E8)!= 4)goto _LL267;_tmp3E9=((
struct Cyc_Absyn_PointerType_struct*)_tmp3E8)->f1;_tmp3EA=(void*)_tmp3E9.elt_typ;
_tmp3EB=_tmp3E9.ptr_atts;_tmp3EC=_tmp3EB.bounds;_tmp3ED=_tmp3EB.zero_term;_LL266:{
void*_tmp3EE=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3EC);struct Cyc_Absyn_Exp*
_tmp3EF;_LL26A: if((int)_tmp3EE != 0)goto _LL26C;_LL26B: {struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp376))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp376->tl))->hd;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_ptr_plus_e,({struct
Cyc_Absyn_Exp*_tmp3F0[3];_tmp3F0[2]=e2;_tmp3F0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3EA),0);_tmp3F0[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3F0,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL269;}_LL26C:
if((int)_tmp3EE != 1)goto _LL26E;_LL26D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp376))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp376->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3F1[3];_tmp3F1[2]=e2;
_tmp3F1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3EA),0);_tmp3F1[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3F1,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL269;}_LL26E: if(_tmp3EE <= (
void*)2)goto _LL270;if(*((int*)_tmp3EE)!= 0)goto _LL270;_tmp3EF=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3EE)->f1;_LL26F: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3ED)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp376))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp376->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp3F2[3];_tmp3F2[2]=e2;_tmp3F2[
1]=_tmp3EF;_tmp3F2[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3F2,sizeof(struct Cyc_Absyn_Exp*),3));})));}goto _LL269;_LL270:
if(_tmp3EE <= (void*)2)goto _LL269;if(*((int*)_tmp3EE)!= 1)goto _LL269;_LL271:({
void*_tmp3F3[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp3F4="toc: plus on AbsUpper_b";_tag_dynforward(
_tmp3F4,sizeof(char),_get_zero_arr_size(_tmp3F4,24));}),_tag_dynforward(_tmp3F3,
sizeof(void*),0));});_LL269:;}goto _LL264;_LL267:;_LL268: goto _LL264;_LL264:;}goto
_LL23F;_LL244: if((int)_tmp3D3 != 2)goto _LL246;_LL245: {void*elt_typ=(void*)0;int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp3D2))->hd,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp376))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp376->tl))->hd;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp3D2->tl))->hd,& forward_only)){(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(
Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{if(forward_only)({void*_tmp3F5[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3F6="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp3F6,sizeof(char),_get_zero_arr_size(_tmp3F6,50));}),
_tag_dynforward(_tmp3F5,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3F7[3];_tmp3F7[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp3F7[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp3F7[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3F7,sizeof(struct Cyc_Absyn_Exp*),3));})));}}goto _LL23F;}
_LL246: if((int)_tmp3D3 != 5)goto _LL248;_LL247: goto _LL249;_LL248: if((int)_tmp3D3 != 
6)goto _LL24A;_LL249: goto _LL24B;_LL24A: if((int)_tmp3D3 != 7)goto _LL24C;_LL24B: goto
_LL24D;_LL24C: if((int)_tmp3D3 != 9)goto _LL24E;_LL24D: goto _LL24F;_LL24E: if((int)
_tmp3D3 != 8)goto _LL250;_LL24F: goto _LL251;_LL250: if((int)_tmp3D3 != 10)goto _LL252;
_LL251: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp376))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp376->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3D2))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp3D2->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL23F;}_LL252:;_LL253: goto _LL23F;_LL23F:;}
goto _LL1E5;}_LL1F0: if(*((int*)_tmp370)!= 5)goto _LL1F2;_tmp377=((struct Cyc_Absyn_Increment_e_struct*)
_tmp370)->f1;_tmp378=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp370)->f2;
_LL1F1: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp377->topt))->v;
void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=
0;struct _dynforward_ptr incr_str=({const char*_tmp403="increment";_tag_dynforward(
_tmp403,sizeof(char),_get_zero_arr_size(_tmp403,10));});if(_tmp378 == (void*)2
 || _tmp378 == (void*)3)incr_str=({const char*_tmp3F8="decrement";_tag_dynforward(
_tmp3F8,sizeof(char),_get_zero_arr_size(_tmp3F8,10));});if(Cyc_Toc_is_zero_ptr_deref(
_tmp377,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct
_tmp3FB;_tmp3FB.tag=0;_tmp3FB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
incr_str);{void*_tmp3F9[1]={& _tmp3FB};Cyc_Tcutil_terr(e->loc,({const char*_tmp3FA="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp3FA,sizeof(char),_get_zero_arr_size(_tmp3FA,74));}),
_tag_dynforward(_tmp3F9,sizeof(void*),1));}});({void*_tmp3FC[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3FD="in-place inc on zero-term";_tag_dynforward(_tmp3FD,sizeof(char),
_get_zero_arr_size(_tmp3FD,26));}),_tag_dynforward(_tmp3FC,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp377);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp378 == (void*)3  || _tmp378 == (void*)2)({void*_tmp3FE[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp3FF="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp3FF,sizeof(char),_get_zero_arr_size(_tmp3FF,43));}),
_tag_dynforward(_tmp3FE,sizeof(void*),0));});fn_e=_tmp378 == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp378 == (void*)1  || _tmp378
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp378 == (void*)2  || _tmp378 == (void*)3)change=- 1;}(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp400[3];_tmp400[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp400[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp400[0]=
Cyc_Absyn_address_exp(_tmp377,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp400,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp401=_tmp378 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e: Cyc_Toc__zero_arr_inplace_plus_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp401,({struct Cyc_Absyn_Exp*_tmp402[2];
_tmp402[1]=Cyc_Absyn_signed_int_exp(1,0);_tmp402[0]=_tmp377;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp402,sizeof(struct Cyc_Absyn_Exp*),
2));})));}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp377)){Cyc_Toc_lvalue_assign(
_tmp377,0,Cyc_Toc_incr_lvalue,_tmp378);(void*)(e->r=(void*)((void*)_tmp377->r));}}}
goto _LL1E5;}}_LL1F2: if(*((int*)_tmp370)!= 4)goto _LL1F4;_tmp379=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp370)->f1;_tmp37A=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp370)->f2;_tmp37B=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp370)->f3;_LL1F3: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Toc_is_zero_ptr_deref(
_tmp379,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp379,_tmp37A,_tmp37B,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp379);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp379->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp37B->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp379);
Cyc_Toc_exp_to_c(nv,_tmp37B);{int done=0;if(_tmp37A != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp404=(void*)_tmp37A->v;_LL273:
if((int)_tmp404 != 0)goto _LL275;_LL274: change=_tmp37B;goto _LL272;_LL275: if((int)
_tmp404 != 2)goto _LL277;_LL276: if(forward_only)({void*_tmp405[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp406="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp406,sizeof(char),_get_zero_arr_size(_tmp406,46));}),
_tag_dynforward(_tmp405,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp37B,0);goto _LL272;_LL277:;_LL278:({void*_tmp407[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp408="bad t ? pointer arithmetic";_tag_dynforward(_tmp408,sizeof(char),
_get_zero_arr_size(_tmp408,27));}),_tag_dynforward(_tmp407,sizeof(void*),0));});
_LL272:;}done=1;{struct Cyc_Absyn_Exp*_tmp409=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
_tmp409,({struct Cyc_Absyn_Exp*_tmp40A[3];_tmp40A[2]=change;_tmp40A[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp40A[0]=Cyc_Absyn_address_exp(_tmp379,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp40A,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp40B=(void*)_tmp37A->v;_LL27A: if((int)_tmp40B != 0)
goto _LL27C;_LL27B: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp40C[2];_tmp40C[1]=_tmp37B;_tmp40C[0]=_tmp379;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp40C,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL279;_LL27C:;_LL27D:({void*_tmp40D[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp40E="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp40E,sizeof(
char),_get_zero_arr_size(_tmp40E,39));}),_tag_dynforward(_tmp40D,sizeof(void*),0));});
_LL279:;}}}if(!done){if(e1_poly)(void*)(_tmp37B->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp37B->r,0)));if(!Cyc_Absyn_is_lvalue(_tmp379)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple8*),struct _tuple8*f_env))Cyc_Toc_lvalue_assign)(
_tmp379,0,Cyc_Toc_assignop_lvalue,({struct _tuple8*_tmp40F=_cycalloc(sizeof(
struct _tuple8)* 1);_tmp40F[0]=({struct _tuple8 _tmp410;_tmp410.f1=_tmp37A;_tmp410.f2=
_tmp37B;_tmp410;});_tmp40F;}));(void*)(e->r=(void*)((void*)_tmp379->r));}}goto
_LL1E5;}}}_LL1F4: if(*((int*)_tmp370)!= 6)goto _LL1F6;_tmp37C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp370)->f1;_tmp37D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp370)->f2;
_tmp37E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp370)->f3;_LL1F5: Cyc_Toc_exp_to_c(
nv,_tmp37C);Cyc_Toc_exp_to_c(nv,_tmp37D);Cyc_Toc_exp_to_c(nv,_tmp37E);goto _LL1E5;
_LL1F6: if(*((int*)_tmp370)!= 7)goto _LL1F8;_tmp37F=((struct Cyc_Absyn_And_e_struct*)
_tmp370)->f1;_tmp380=((struct Cyc_Absyn_And_e_struct*)_tmp370)->f2;_LL1F7: Cyc_Toc_exp_to_c(
nv,_tmp37F);Cyc_Toc_exp_to_c(nv,_tmp380);goto _LL1E5;_LL1F8: if(*((int*)_tmp370)!= 
8)goto _LL1FA;_tmp381=((struct Cyc_Absyn_Or_e_struct*)_tmp370)->f1;_tmp382=((
struct Cyc_Absyn_Or_e_struct*)_tmp370)->f2;_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp381);
Cyc_Toc_exp_to_c(nv,_tmp382);goto _LL1E5;_LL1FA: if(*((int*)_tmp370)!= 9)goto
_LL1FC;_tmp383=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp370)->f1;_tmp384=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp370)->f2;_LL1FB: Cyc_Toc_exp_to_c(nv,_tmp383);Cyc_Toc_exp_to_c(
nv,_tmp384);goto _LL1E5;_LL1FC: if(*((int*)_tmp370)!= 10)goto _LL1FE;_tmp385=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp370)->f1;_tmp386=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp370)->f2;_LL1FD: _tmp387=_tmp385;_tmp388=_tmp386;goto _LL1FF;_LL1FE: if(*((int*)
_tmp370)!= 11)goto _LL200;_tmp387=((struct Cyc_Absyn_FnCall_e_struct*)_tmp370)->f1;
_tmp388=((struct Cyc_Absyn_FnCall_e_struct*)_tmp370)->f2;_tmp389=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp370)->f3;if(_tmp389 != 0)goto _LL200;_LL1FF: Cyc_Toc_exp_to_c(nv,_tmp387);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp388);goto _LL1E5;
_LL200: if(*((int*)_tmp370)!= 11)goto _LL202;_tmp38A=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp370)->f1;_tmp38B=((struct Cyc_Absyn_FnCall_e_struct*)_tmp370)->f2;_tmp38C=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp370)->f3;if(_tmp38C == 0)goto _LL202;_tmp38D=*
_tmp38C;_tmp38E=_tmp38D.num_varargs;_tmp38F=_tmp38D.injectors;_tmp390=_tmp38D.vai;
_LL201:{struct _RegionHandle _tmp411=_new_region("r");struct _RegionHandle*r=&
_tmp411;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp38E,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp390->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp38B);int num_normargs=num_args - _tmp38E;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp38B=_tmp38B->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp38B))->hd);
new_args=({struct Cyc_List_List*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->hd=(
struct Cyc_Absyn_Exp*)_tmp38B->hd;_tmp412->tl=new_args;_tmp412;});}}new_args=({
struct Cyc_List_List*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp414[3];_tmp414[2]=
num_varargs_exp;_tmp414[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp414[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp414,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp413->tl=
new_args;_tmp413;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp38A);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp38A,new_args,0),0);if(_tmp390->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp415=Cyc_Tcutil_compress((void*)_tmp390->type);
struct Cyc_Absyn_TunionInfo _tmp416;union Cyc_Absyn_TunionInfoU_union _tmp417;struct
Cyc_Absyn_Tuniondecl**_tmp418;struct Cyc_Absyn_Tuniondecl*_tmp419;_LL27F: if(
_tmp415 <= (void*)4)goto _LL281;if(*((int*)_tmp415)!= 2)goto _LL281;_tmp416=((
struct Cyc_Absyn_TunionType_struct*)_tmp415)->f1;_tmp417=_tmp416.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp415)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL281;_tmp418=(_tmp417.KnownTunion).f1;_tmp419=*_tmp418;_LL280: tud=
_tmp419;goto _LL27E;_LL281:;_LL282:({void*_tmp41A[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp41B="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp41B,
sizeof(char),_get_zero_arr_size(_tmp41B,42));}),_tag_dynforward(_tmp41A,sizeof(
void*),0));});_LL27E:;}{struct _dynforward_ptr vs=({unsigned int _tmp423=(
unsigned int)_tmp38E;struct _tuple1**_tmp424=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp423));struct _dynforward_ptr _tmp426=
_tag_dynforward(_tmp424,sizeof(struct _tuple1*),_tmp423);{unsigned int _tmp425=
_tmp423;unsigned int i;for(i=0;i < _tmp425;i ++){_tmp424[i]=Cyc_Toc_temp_var();}}
_tmp426;});struct Cyc_List_List*_tmp41C=0;{int i=_tmp38E - 1;for(0;i >= 0;-- i){
_tmp41C=({struct Cyc_List_List*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp41D->tl=
_tmp41C;_tmp41D;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp41C,0),s,0);{int i=0;for(0;_tmp38B != 0;(((
_tmp38B=_tmp38B->tl,_tmp38F=_tmp38F->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp38B->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp41F;struct _tuple1*
_tmp420;struct Cyc_List_List*_tmp421;struct Cyc_Absyn_Tunionfield*_tmp41E=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp38F))->hd;_tmp41F=*
_tmp41E;_tmp420=_tmp41F.name;_tmp421=_tmp41F.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp421))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp420,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp420,({const char*_tmp422="_struct";
_tag_dynforward(_tmp422,sizeof(char),_get_zero_arr_size(_tmp422,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp38B != 0;(_tmp38B=_tmp38B->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp38B->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp38B->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}};_pop_region(r);}goto _LL1E5;_LL202: if(*((int*)_tmp370)!= 12)goto _LL204;
_tmp391=((struct Cyc_Absyn_Throw_e_struct*)_tmp370)->f1;_LL203: Cyc_Toc_exp_to_c(
nv,_tmp391);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp391),0))->r));goto _LL1E5;_LL204: if(*((int*)
_tmp370)!= 13)goto _LL206;_tmp392=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp370)->f1;_LL205: Cyc_Toc_exp_to_c(nv,_tmp392);goto _LL1E5;_LL206: if(*((int*)
_tmp370)!= 14)goto _LL208;_tmp393=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp370)->f1;
_tmp394=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp370)->f2;_LL207: Cyc_Toc_exp_to_c(
nv,_tmp393);for(0;_tmp394 != 0;_tmp394=_tmp394->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp394->hd);if(k != (void*)4  && k != (void*)3){{void*_tmp427=Cyc_Tcutil_compress((
void*)_tmp394->hd);_LL284: if(_tmp427 <= (void*)4)goto _LL288;if(*((int*)_tmp427)!= 
1)goto _LL286;_LL285: goto _LL287;_LL286: if(*((int*)_tmp427)!= 2)goto _LL288;_LL287:
continue;_LL288:;_LL289:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp393,
0))->r));goto _LL283;_LL283:;}break;}}goto _LL1E5;_LL208: if(*((int*)_tmp370)!= 15)
goto _LL20A;_tmp395=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp370)->f1;_tmp396=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp370)->f1);_tmp397=((struct
Cyc_Absyn_Cast_e_struct*)_tmp370)->f2;_tmp398=((struct Cyc_Absyn_Cast_e_struct*)
_tmp370)->f3;_tmp399=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp370)->f4;_LL209: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp397->topt))->v;void*
new_typ=*_tmp396;*_tmp396=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp397);{
struct _tuple0 _tmp429=({struct _tuple0 _tmp428;_tmp428.f1=Cyc_Tcutil_compress(
old_t2);_tmp428.f2=Cyc_Tcutil_compress(new_typ);_tmp428;});void*_tmp42A;struct
Cyc_Absyn_PtrInfo _tmp42B;void*_tmp42C;struct Cyc_Absyn_PtrInfo _tmp42D;void*
_tmp42E;struct Cyc_Absyn_PtrInfo _tmp42F;void*_tmp430;_LL28B: _tmp42A=_tmp429.f1;
if(_tmp42A <= (void*)4)goto _LL28D;if(*((int*)_tmp42A)!= 4)goto _LL28D;_tmp42B=((
struct Cyc_Absyn_PointerType_struct*)_tmp42A)->f1;_tmp42C=_tmp429.f2;if(_tmp42C <= (
void*)4)goto _LL28D;if(*((int*)_tmp42C)!= 4)goto _LL28D;_tmp42D=((struct Cyc_Absyn_PointerType_struct*)
_tmp42C)->f1;_LL28C: {int _tmp431=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp42B.ptr_atts).nullable);int _tmp432=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp42D.ptr_atts).nullable);void*_tmp433=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp42B.ptr_atts).bounds);void*_tmp434=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp42D.ptr_atts).bounds);int _tmp435=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp42B.ptr_atts).zero_term);int
_tmp436=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp42D.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp438=({struct _tuple0
_tmp437;_tmp437.f1=_tmp433;_tmp437.f2=_tmp434;_tmp437;});void*_tmp439;struct Cyc_Absyn_Exp*
_tmp43A;void*_tmp43B;struct Cyc_Absyn_Exp*_tmp43C;void*_tmp43D;struct Cyc_Absyn_Exp*
_tmp43E;void*_tmp43F;void*_tmp440;struct Cyc_Absyn_Exp*_tmp441;void*_tmp442;void*
_tmp443;void*_tmp444;struct Cyc_Absyn_Exp*_tmp445;void*_tmp446;void*_tmp447;
struct Cyc_Absyn_Exp*_tmp448;void*_tmp449;void*_tmp44A;void*_tmp44B;void*_tmp44C;
void*_tmp44D;void*_tmp44E;void*_tmp44F;void*_tmp450;void*_tmp451;void*_tmp452;
void*_tmp453;void*_tmp454;void*_tmp455;void*_tmp456;_LL292: _tmp439=_tmp438.f1;
if(_tmp439 <= (void*)2)goto _LL294;if(*((int*)_tmp439)!= 0)goto _LL294;_tmp43A=((
struct Cyc_Absyn_Upper_b_struct*)_tmp439)->f1;_tmp43B=_tmp438.f2;if(_tmp43B <= (
void*)2)goto _LL294;if(*((int*)_tmp43B)!= 0)goto _LL294;_tmp43C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp43B)->f1;_LL293: if(_tmp431  && !_tmp432){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp457[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp458="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp458,sizeof(char),_get_zero_arr_size(_tmp458,44));}),_tag_dynforward(_tmp457,
sizeof(void*),0));});if(_tmp399 != (void*)2)({struct Cyc_String_pa_struct _tmp45B;
_tmp45B.tag=0;_tmp45B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp459[1]={& _tmp45B};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp45A="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp45A,sizeof(char),_get_zero_arr_size(_tmp45A,41));}),
_tag_dynforward(_tmp459,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp397);if(do_null_check){if(!_tmp398)({void*_tmp45C[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp45D="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp45D,sizeof(char),_get_zero_arr_size(_tmp45D,58));}),
_tag_dynforward(_tmp45C,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp396,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp45E=
_cycalloc(sizeof(*_tmp45E));_tmp45E->hd=_tmp397;_tmp45E->tl=0;_tmp45E;}),0)));}}}
goto _LL291;_LL294: _tmp43D=_tmp438.f1;if(_tmp43D <= (void*)2)goto _LL296;if(*((int*)
_tmp43D)!= 0)goto _LL296;_tmp43E=((struct Cyc_Absyn_Upper_b_struct*)_tmp43D)->f1;
_tmp43F=_tmp438.f2;if((int)_tmp43F != 0)goto _LL296;_LL295: p2_forward_only=1;
_tmp441=_tmp43E;goto _LL297;_LL296: _tmp440=_tmp438.f1;if(_tmp440 <= (void*)2)goto
_LL298;if(*((int*)_tmp440)!= 0)goto _LL298;_tmp441=((struct Cyc_Absyn_Upper_b_struct*)
_tmp440)->f1;_tmp442=_tmp438.f2;if((int)_tmp442 != 1)goto _LL298;_LL297: if(_tmp399
== (void*)2)({struct Cyc_String_pa_struct _tmp461;_tmp461.tag=0;_tmp461.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp45F[
1]={& _tmp461};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp460="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp460,sizeof(char),_get_zero_arr_size(_tmp460,44));}),_tag_dynforward(_tmp45F,
sizeof(void*),1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp435  && !(_tmp42D.elt_tq).real_const)
 && !_tmp436)_tmp441=Cyc_Absyn_prim2_exp((void*)2,_tmp441,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp441,
_tmp397,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp462=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp435){struct _tuple1*
_tmp463=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp464=Cyc_Absyn_var_exp(_tmp463,
0);struct Cyc_Absyn_Exp*_tmp465=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp46A[2];_tmp46A[1]=_tmp441;_tmp46A[0]=_tmp464;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp46A,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp436  && !(_tmp42D.elt_tq).real_const)_tmp465=Cyc_Absyn_prim2_exp((
void*)2,_tmp465,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp42D.elt_typ),0);struct Cyc_Absyn_Exp*_tmp467=Cyc_Absyn_fncall_exp(
_tmp462,({struct Cyc_Absyn_Exp*_tmp469[3];_tmp469[2]=_tmp465;_tmp469[1]=_tmp466;
_tmp469[0]=_tmp464;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp469,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp468=Cyc_Absyn_exp_stmt(_tmp467,0);_tmp468=Cyc_Absyn_declare_stmt(_tmp463,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp397,_tmp468,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp468));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp462,({struct Cyc_Absyn_Exp*
_tmp46B[3];_tmp46B[2]=_tmp441;_tmp46B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp42D.elt_typ),0);_tmp46B[0]=_tmp397;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp46B,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL291;_LL298: _tmp443=_tmp438.f1;if((int)_tmp443 != 0)goto _LL29A;
_tmp444=_tmp438.f2;if(_tmp444 <= (void*)2)goto _LL29A;if(*((int*)_tmp444)!= 0)goto
_LL29A;_tmp445=((struct Cyc_Absyn_Upper_b_struct*)_tmp444)->f1;_LL299:
p1_forward_only=1;_tmp448=_tmp445;goto _LL29B;_LL29A: _tmp446=_tmp438.f1;if((int)
_tmp446 != 1)goto _LL29C;_tmp447=_tmp438.f2;if(_tmp447 <= (void*)2)goto _LL29C;if(*((
int*)_tmp447)!= 0)goto _LL29C;_tmp448=((struct Cyc_Absyn_Upper_b_struct*)_tmp447)->f1;
_LL29B: if(Cyc_Toc_is_toplevel(nv))({void*_tmp46C[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp46D="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp46D,sizeof(char),_get_zero_arr_size(_tmp46D,45));}),
_tag_dynforward(_tmp46C,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp46E=
_tmp448;if(_tmp435  && !_tmp436)_tmp46E=Cyc_Absyn_add_exp(_tmp448,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp46F=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_fncall_exp(
_tmp46F,({struct Cyc_Absyn_Exp*_tmp472[3];_tmp472[2]=_tmp46E;_tmp472[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp42B.elt_typ),0);_tmp472[0]=_tmp397;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp472,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp432)(void*)(_tmp470->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->hd=Cyc_Absyn_copy_exp(
_tmp470);_tmp471->tl=0;_tmp471;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp396,_tmp470));goto _LL291;}}_LL29C: _tmp449=_tmp438.f1;if((int)_tmp449 != 0)
goto _LL29E;_tmp44A=_tmp438.f2;if((int)_tmp44A != 0)goto _LL29E;_LL29D:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL29E: _tmp44B=_tmp438.f1;if((
int)_tmp44B != 0)goto _LL2A0;_tmp44C=_tmp438.f2;if((int)_tmp44C != 1)goto _LL2A0;
_LL29F: p1_forward_only=1;goto DynCast;_LL2A0: _tmp44D=_tmp438.f1;if((int)_tmp44D != 
1)goto _LL2A2;_tmp44E=_tmp438.f2;if((int)_tmp44E != 0)goto _LL2A2;_LL2A1:
p2_forward_only=1;goto DynCast;_LL2A2: _tmp44F=_tmp438.f1;if((int)_tmp44F != 1)goto
_LL2A4;_tmp450=_tmp438.f2;if((int)_tmp450 != 1)goto _LL2A4;_LL2A3: DynCast: if((
_tmp435  && !_tmp436) && !(_tmp42D.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({
void*_tmp473[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp474="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp474,sizeof(char),_get_zero_arr_size(_tmp474,70));}),
_tag_dynforward(_tmp473,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp475=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp475,({struct Cyc_Absyn_Exp*_tmp476[3];
_tmp476[2]=Cyc_Absyn_uint_exp(1,0);_tmp476[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp42B.elt_typ),0);_tmp476[0]=_tmp397;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp476,sizeof(struct Cyc_Absyn_Exp*),
3));})));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp477[1];_tmp477[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp477,sizeof(struct Cyc_Absyn_Exp*),
1));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp478[1];_tmp478[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp478,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}}else{if(p1_forward_only != p2_forward_only){if(Cyc_Toc_is_toplevel(
nv))({void*_tmp479[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp47A="can't coerce between ? and ?+- at toplevel";
_tag_dynforward(_tmp47A,sizeof(char),_get_zero_arr_size(_tmp47A,43));}),
_tag_dynforward(_tmp479,sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp47B[1];_tmp47B[0]=_tmp397;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp47B,sizeof(struct Cyc_Absyn_Exp*),1));})));
else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp47C[1];_tmp47C[0]=_tmp397;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp47C,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}goto _LL291;_LL2A4: _tmp451=_tmp438.f1;if(_tmp451 <= (void*)2)goto
_LL2A6;if(*((int*)_tmp451)!= 1)goto _LL2A6;_tmp452=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp451)->f1;_tmp453=_tmp438.f2;if(_tmp453 <= (void*)2)goto _LL2A6;if(*((int*)
_tmp453)!= 1)goto _LL2A6;_tmp454=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp453)->f1;_LL2A5: if(_tmp431  && !_tmp432){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp47D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp47E="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp47E,sizeof(char),_get_zero_arr_size(_tmp47E,44));}),_tag_dynforward(_tmp47D,
sizeof(void*),0));});if(_tmp399 != (void*)2)({struct Cyc_String_pa_struct _tmp481;
_tmp481.tag=0;_tmp481.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp47F[1]={& _tmp481};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp480="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp480,sizeof(char),_get_zero_arr_size(_tmp480,41));}),
_tag_dynforward(_tmp47F,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp397);if(do_null_check){if(!_tmp398)({void*_tmp482[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp483="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp483,sizeof(char),_get_zero_arr_size(_tmp483,58));}),
_tag_dynforward(_tmp482,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp396,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp484=
_cycalloc(sizeof(*_tmp484));_tmp484->hd=_tmp397;_tmp484->tl=0;_tmp484;}),0)));}}}
goto _LL291;_LL2A6: _tmp455=_tmp438.f1;if(_tmp455 <= (void*)2)goto _LL2A8;if(*((int*)
_tmp455)!= 1)goto _LL2A8;_LL2A7:({struct Cyc_String_pa_struct _tmp487;_tmp487.tag=0;
_tmp487.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp397->loc));{void*_tmp485[1]={& _tmp487};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp486="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp486,sizeof(char),_get_zero_arr_size(_tmp486,30));}),
_tag_dynforward(_tmp485,sizeof(void*),1));}});_LL2A8: _tmp456=_tmp438.f2;if(
_tmp456 <= (void*)2)goto _LL291;if(*((int*)_tmp456)!= 1)goto _LL291;_LL2A9: goto
_LL291;_LL291:;}goto _LL28A;}_LL28D: _tmp42E=_tmp429.f1;if(_tmp42E <= (void*)4)goto
_LL28F;if(*((int*)_tmp42E)!= 4)goto _LL28F;_tmp42F=((struct Cyc_Absyn_PointerType_struct*)
_tmp42E)->f1;_tmp430=_tmp429.f2;if(_tmp430 <= (void*)4)goto _LL28F;if(*((int*)
_tmp430)!= 5)goto _LL28F;_LL28E:{void*_tmp488=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp42F.ptr_atts).bounds);_LL2AB: if((int)_tmp488 != 0)goto _LL2AD;_LL2AC: goto
_LL2AE;_LL2AD: if((int)_tmp488 != 1)goto _LL2AF;_LL2AE:(void*)(_tmp397->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp397->r,_tmp397->loc),Cyc_Toc_curr_sp));
goto _LL2AA;_LL2AF:;_LL2B0: goto _LL2AA;_LL2AA:;}goto _LL28A;_LL28F:;_LL290: goto
_LL28A;_LL28A:;}goto _LL1E5;}_LL20A: if(*((int*)_tmp370)!= 16)goto _LL20C;_tmp39A=((
struct Cyc_Absyn_Address_e_struct*)_tmp370)->f1;_LL20B:{void*_tmp489=(void*)
_tmp39A->r;struct _tuple1*_tmp48A;struct Cyc_List_List*_tmp48B;struct Cyc_List_List*
_tmp48C;struct Cyc_List_List*_tmp48D;_LL2B2: if(*((int*)_tmp489)!= 30)goto _LL2B4;
_tmp48A=((struct Cyc_Absyn_Struct_e_struct*)_tmp489)->f1;_tmp48B=((struct Cyc_Absyn_Struct_e_struct*)
_tmp489)->f2;_tmp48C=((struct Cyc_Absyn_Struct_e_struct*)_tmp489)->f3;_LL2B3: if(
Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp490;_tmp490.tag=0;
_tmp490.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp39A->loc));{void*_tmp48E[1]={& _tmp490};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp48F="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp48F,sizeof(char),_get_zero_arr_size(_tmp48F,42));}),
_tag_dynforward(_tmp48E,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp39A->topt))->v,_tmp48B != 0,1,0,
_tmp48C,_tmp48A))->r));goto _LL2B1;_LL2B4: if(*((int*)_tmp489)!= 26)goto _LL2B6;
_tmp48D=((struct Cyc_Absyn_Tuple_e_struct*)_tmp489)->f1;_LL2B5: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp493;_tmp493.tag=0;_tmp493.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp39A->loc));{
void*_tmp491[1]={& _tmp493};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp492="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp492,sizeof(char),_get_zero_arr_size(_tmp492,42));}),
_tag_dynforward(_tmp491,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp48D))->r));goto _LL2B1;_LL2B6:;_LL2B7: Cyc_Toc_exp_to_c(nv,_tmp39A);if(
!Cyc_Absyn_is_lvalue(_tmp39A)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp39A,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp39A));}goto _LL2B1;
_LL2B1:;}goto _LL1E5;_LL20C: if(*((int*)_tmp370)!= 17)goto _LL20E;_tmp39B=((struct
Cyc_Absyn_New_e_struct*)_tmp370)->f1;_tmp39C=((struct Cyc_Absyn_New_e_struct*)
_tmp370)->f2;_LL20D: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp496;_tmp496.tag=0;_tmp496.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Position_string_of_segment(_tmp39C->loc));{void*_tmp494[1]={& _tmp496};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp495="%s: new at top-level";_tag_dynforward(_tmp495,sizeof(char),
_get_zero_arr_size(_tmp495,21));}),_tag_dynforward(_tmp494,sizeof(void*),1));}});{
void*_tmp497=(void*)_tmp39C->r;struct Cyc_List_List*_tmp498;struct Cyc_Absyn_Vardecl*
_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;int _tmp49C;
struct _tuple1*_tmp49D;struct Cyc_List_List*_tmp49E;struct Cyc_List_List*_tmp49F;
struct Cyc_List_List*_tmp4A0;_LL2B9: if(*((int*)_tmp497)!= 28)goto _LL2BB;_tmp498=((
struct Cyc_Absyn_Array_e_struct*)_tmp497)->f1;_LL2BA: {struct _tuple1*_tmp4A1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Absyn_var_exp(_tmp4A1,0);struct Cyc_Absyn_Stmt*
_tmp4A3=Cyc_Toc_init_array(nv,_tmp4A2,_tmp498,Cyc_Absyn_exp_stmt(_tmp4A2,0));
void*old_elt_typ;{void*_tmp4A4=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_Tqual _tmp4A7;struct Cyc_Absyn_PtrAtts _tmp4A8;
struct Cyc_Absyn_Conref*_tmp4A9;_LL2C4: if(_tmp4A4 <= (void*)4)goto _LL2C6;if(*((int*)
_tmp4A4)!= 4)goto _LL2C6;_tmp4A5=((struct Cyc_Absyn_PointerType_struct*)_tmp4A4)->f1;
_tmp4A6=(void*)_tmp4A5.elt_typ;_tmp4A7=_tmp4A5.elt_tq;_tmp4A8=_tmp4A5.ptr_atts;
_tmp4A9=_tmp4A8.zero_term;_LL2C5: old_elt_typ=_tmp4A6;goto _LL2C3;_LL2C6:;_LL2C7:
old_elt_typ=({void*_tmp4AA[0]={};Cyc_Toc_toc_impos(({const char*_tmp4AB="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp4AB,sizeof(char),_get_zero_arr_size(_tmp4AB,52));}),
_tag_dynforward(_tmp4AA,sizeof(void*),0));});_LL2C3:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp4AC=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4AD=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp498),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp39B == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp4AD);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp39B;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp4AD);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4A1,
_tmp4AC,(struct Cyc_Absyn_Exp*)e1,_tmp4A3,0)));goto _LL2B8;}}_LL2BB: if(*((int*)
_tmp497)!= 29)goto _LL2BD;_tmp499=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp497)->f1;_tmp49A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp497)->f2;
_tmp49B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp497)->f3;_tmp49C=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp497)->f4;_LL2BC: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp4AE=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4AF;void*_tmp4B0;struct Cyc_Absyn_Tqual _tmp4B1;struct Cyc_Absyn_PtrAtts _tmp4B2;
struct Cyc_Absyn_Conref*_tmp4B3;struct Cyc_Absyn_Conref*_tmp4B4;_LL2C9: if(_tmp4AE
<= (void*)4)goto _LL2CB;if(*((int*)_tmp4AE)!= 4)goto _LL2CB;_tmp4AF=((struct Cyc_Absyn_PointerType_struct*)
_tmp4AE)->f1;_tmp4B0=(void*)_tmp4AF.elt_typ;_tmp4B1=_tmp4AF.elt_tq;_tmp4B2=
_tmp4AF.ptr_atts;_tmp4B3=_tmp4B2.bounds;_tmp4B4=_tmp4B2.zero_term;_LL2CA:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4B3)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4B3)== (void*)1;
goto _LL2C8;_LL2CB:;_LL2CC:({void*_tmp4B5[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4B6="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp4B6,sizeof(char),_get_zero_arr_size(_tmp4B6,42));}),
_tag_dynforward(_tmp4B5,sizeof(void*),0));});_LL2C8:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp49B->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp49A);{
struct Cyc_Absyn_Exp*_tmp4B7=Cyc_Absyn_var_exp(max,0);if(_tmp49C)_tmp4B7=Cyc_Absyn_add_exp(
_tmp4B7,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp499,Cyc_Absyn_var_exp(max,0),_tmp49B,_tmp49C,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp4B8=_new_region("r");struct _RegionHandle*r=& _tmp4B8;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp4C9=
_region_malloc(r,sizeof(*_tmp4C9));_tmp4C9->hd=({struct _tuple9*_tmp4CA=
_region_malloc(r,sizeof(*_tmp4CA));_tmp4CA->f1=max;_tmp4CA->f2=Cyc_Absyn_uint_typ;
_tmp4CA->f3=(struct Cyc_Absyn_Exp*)_tmp49A;_tmp4CA;});_tmp4C9->tl=0;_tmp4C9;});
struct Cyc_Absyn_Exp*ai;if(_tmp39B == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp4B9[2];_tmp4B9[1]=_tmp4B7;
_tmp4B9[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4B9,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp39B;Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp4BA[2];_tmp4BA[1]=_tmp4B7;_tmp4BA[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4BA,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({struct Cyc_List_List*_tmp4BB=
_region_malloc(r,sizeof(*_tmp4BB));_tmp4BB->hd=({struct _tuple9*_tmp4BC=
_region_malloc(r,sizeof(*_tmp4BC));_tmp4BC->f1=a;_tmp4BC->f2=ptr_typ;_tmp4BC->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp4BC;});_tmp4BB->tl=decls;_tmp4BB;});if(
is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp4BD=Cyc_Toc_temp_var();
void*_tmp4BE=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp4BF=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp4C0=Cyc_Absyn_fncall_exp(_tmp4BF,({struct Cyc_Absyn_Exp*_tmp4C3[3];_tmp4C3[2]=
_tmp4B7;_tmp4C3[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp4C3[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4C3,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp4C1=_region_malloc(r,sizeof(*_tmp4C1));_tmp4C1->hd=({struct _tuple9*_tmp4C2=
_region_malloc(r,sizeof(*_tmp4C2));_tmp4C2->f1=_tmp4BD;_tmp4C2->f2=_tmp4BE;
_tmp4C2->f3=(struct Cyc_Absyn_Exp*)_tmp4C0;_tmp4C2;});_tmp4C1->tl=decls;_tmp4C1;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4BD,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp4C4=decls;for(0;_tmp4C4 != 0;_tmp4C4=_tmp4C4->tl){struct
_tuple1*_tmp4C6;void*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;struct _tuple9 _tmp4C5=*((
struct _tuple9*)_tmp4C4->hd);_tmp4C6=_tmp4C5.f1;_tmp4C7=_tmp4C5.f2;_tmp4C8=
_tmp4C5.f3;s=Cyc_Absyn_declare_stmt(_tmp4C6,_tmp4C7,_tmp4C8,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL2B8;}}}}_LL2BD: if(*((int*)
_tmp497)!= 30)goto _LL2BF;_tmp49D=((struct Cyc_Absyn_Struct_e_struct*)_tmp497)->f1;
_tmp49E=((struct Cyc_Absyn_Struct_e_struct*)_tmp497)->f2;_tmp49F=((struct Cyc_Absyn_Struct_e_struct*)
_tmp497)->f3;_LL2BE:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp39C->topt))->v,_tmp49E != 0,1,_tmp39B,_tmp49F,
_tmp49D))->r));goto _LL2B8;_LL2BF: if(*((int*)_tmp497)!= 26)goto _LL2C1;_tmp4A0=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp497)->f1;_LL2C0:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp39B,_tmp4A0))->r));goto _LL2B8;_LL2C1:;_LL2C2: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp39C->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp4CB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4CC=Cyc_Absyn_var_exp(_tmp4CB,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp4CC,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp39B == 0)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp39B;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*_tmp4CD=(void*)
_tmp39C->r;void*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CF;_LL2CE: if(*((int*)_tmp4CD)!= 
15)goto _LL2D0;_tmp4CE=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4CD)->f1;
_tmp4CF=((struct Cyc_Absyn_Cast_e_struct*)_tmp4CD)->f2;_LL2CF:{struct _tuple0
_tmp4D1=({struct _tuple0 _tmp4D0;_tmp4D0.f1=Cyc_Tcutil_compress(_tmp4CE);_tmp4D0.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4CF->topt))->v);
_tmp4D0;});void*_tmp4D2;struct Cyc_Absyn_PtrInfo _tmp4D3;void*_tmp4D4;struct Cyc_Absyn_PtrAtts
_tmp4D5;struct Cyc_Absyn_Conref*_tmp4D6;void*_tmp4D7;struct Cyc_Absyn_PtrInfo
_tmp4D8;struct Cyc_Absyn_PtrAtts _tmp4D9;struct Cyc_Absyn_Conref*_tmp4DA;_LL2D3:
_tmp4D2=_tmp4D1.f1;if(_tmp4D2 <= (void*)4)goto _LL2D5;if(*((int*)_tmp4D2)!= 4)goto
_LL2D5;_tmp4D3=((struct Cyc_Absyn_PointerType_struct*)_tmp4D2)->f1;_tmp4D4=(void*)
_tmp4D3.elt_typ;_tmp4D5=_tmp4D3.ptr_atts;_tmp4D6=_tmp4D5.bounds;_tmp4D7=_tmp4D1.f2;
if(_tmp4D7 <= (void*)4)goto _LL2D5;if(*((int*)_tmp4D7)!= 4)goto _LL2D5;_tmp4D8=((
struct Cyc_Absyn_PointerType_struct*)_tmp4D7)->f1;_tmp4D9=_tmp4D8.ptr_atts;
_tmp4DA=_tmp4D9.bounds;_LL2D4:{struct _tuple0 _tmp4DC=({struct _tuple0 _tmp4DB;
_tmp4DB.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4D6);_tmp4DB.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp4DA);_tmp4DB;});void*_tmp4DD;void*_tmp4DE;struct Cyc_Absyn_Exp*
_tmp4DF;void*_tmp4E0;void*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;_LL2D8: _tmp4DD=
_tmp4DC.f1;if((int)_tmp4DD != 0)goto _LL2DA;_tmp4DE=_tmp4DC.f2;if(_tmp4DE <= (void*)
2)goto _LL2DA;if(*((int*)_tmp4DE)!= 0)goto _LL2DA;_tmp4DF=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4DE)->f1;_LL2D9: forward_only=1;_tmp4E2=_tmp4DF;goto _LL2DB;_LL2DA: _tmp4E0=
_tmp4DC.f1;if((int)_tmp4E0 != 1)goto _LL2DC;_tmp4E1=_tmp4DC.f2;if(_tmp4E1 <= (void*)
2)goto _LL2DC;if(*((int*)_tmp4E1)!= 0)goto _LL2DC;_tmp4E2=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4E1)->f1;_LL2DB: Cyc_Toc_exp_to_c(nv,_tmp4CF);(void*)(inner_mexp->r=(void*)
Cyc_Toc_sizeoftyp_exp_r(elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp4E3=
forward_only?Cyc_Toc__init_dynforward_ptr_e: Cyc_Toc__init_dyneither_ptr_e;(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4E3,({struct Cyc_Absyn_Exp*_tmp4E4[4];
_tmp4E4[3]=_tmp4E2;_tmp4E4[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4D4),
0);_tmp4E4[1]=_tmp4CF;_tmp4E4[0]=mexp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4E4,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2D7;}_LL2DC:;_LL2DD: goto _LL2D7;_LL2D7:;}goto _LL2D2;_LL2D5:;
_LL2D6: goto _LL2D2;_LL2D2:;}goto _LL2CD;_LL2D0:;_LL2D1: goto _LL2CD;_LL2CD:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp4E5=Cyc_Absyn_exp_stmt(_tmp4CC,0);struct Cyc_Absyn_Exp*
_tmp4E6=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp39C);_tmp4E5=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp4CC,_tmp4E6,0),_tmp39C,0),
_tmp4E5,0);{void*_tmp4E7=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4CB,_tmp4E7,(struct Cyc_Absyn_Exp*)
mexp,_tmp4E5,0)));}}goto _LL2B8;}}_LL2B8:;}goto _LL1E5;_LL20E: if(*((int*)_tmp370)
!= 19)goto _LL210;_tmp39D=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp370)->f1;
_LL20F: Cyc_Toc_exp_to_c(nv,_tmp39D);goto _LL1E5;_LL210: if(*((int*)_tmp370)!= 18)
goto _LL212;_tmp39E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp370)->f1;
_LL211:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4E8=
_cycalloc(sizeof(*_tmp4E8));_tmp4E8[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp4E9;_tmp4E9.tag=18;_tmp4E9.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp39E);_tmp4E9;});
_tmp4E8;})));goto _LL1E5;_LL212: if(*((int*)_tmp370)!= 21)goto _LL214;_LL213:({void*
_tmp4EA[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4EB="__gen() in code generator";_tag_dynforward(_tmp4EB,sizeof(
char),_get_zero_arr_size(_tmp4EB,26));}),_tag_dynforward(_tmp4EA,sizeof(void*),0));});
_LL214: if(*((int*)_tmp370)!= 20)goto _LL216;_tmp39F=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp370)->f1;_tmp3A0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp370)->f2;
if(*((int*)_tmp3A0)!= 0)goto _LL216;_tmp3A1=((struct Cyc_Absyn_StructField_struct*)
_tmp3A0)->f1;_LL215:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp4ED;_tmp4ED.tag=20;_tmp4ED.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp39F);_tmp4ED.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4EE=_cycalloc(sizeof(*
_tmp4EE));_tmp4EE[0]=({struct Cyc_Absyn_StructField_struct _tmp4EF;_tmp4EF.tag=0;
_tmp4EF.f1=_tmp3A1;_tmp4EF;});_tmp4EE;}));_tmp4ED;});_tmp4EC;})));goto _LL1E5;
_LL216: if(*((int*)_tmp370)!= 20)goto _LL218;_tmp3A2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp370)->f1;_tmp3A3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp370)->f2;
if(*((int*)_tmp3A3)!= 1)goto _LL218;_tmp3A4=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp3A3)->f1;_LL217:{void*_tmp4F0=Cyc_Tcutil_compress(_tmp3A2);struct Cyc_Absyn_AggrInfo
_tmp4F1;union Cyc_Absyn_AggrInfoU_union _tmp4F2;struct Cyc_List_List*_tmp4F3;_LL2DF:
if(_tmp4F0 <= (void*)4)goto _LL2E7;if(*((int*)_tmp4F0)!= 10)goto _LL2E1;_tmp4F1=((
struct Cyc_Absyn_AggrType_struct*)_tmp4F0)->f1;_tmp4F2=_tmp4F1.aggr_info;_LL2E0: {
struct Cyc_Absyn_Aggrdecl*_tmp4F4=Cyc_Absyn_get_known_aggrdecl(_tmp4F2);if(
_tmp4F4->impl == 0)({void*_tmp4F5[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4F6="struct fields must be known";
_tag_dynforward(_tmp4F6,sizeof(char),_get_zero_arr_size(_tmp4F6,28));}),
_tag_dynforward(_tmp4F5,sizeof(void*),0));});_tmp4F3=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4F4->impl))->fields;goto _LL2E2;}_LL2E1: if(*((int*)_tmp4F0)!= 11)
goto _LL2E3;_tmp4F3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4F0)->f2;_LL2E2: {
struct Cyc_Absyn_Aggrfield*_tmp4F7=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp4F3,(int)_tmp3A4);(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Offsetof_e_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp4F9;_tmp4F9.tag=20;_tmp4F9.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp3A2);_tmp4F9.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4FA=
_cycalloc(sizeof(*_tmp4FA));_tmp4FA[0]=({struct Cyc_Absyn_StructField_struct
_tmp4FB;_tmp4FB.tag=0;_tmp4FB.f1=_tmp4F7->name;_tmp4FB;});_tmp4FA;}));_tmp4F9;});
_tmp4F8;})));goto _LL2DE;}_LL2E3: if(*((int*)_tmp4F0)!= 9)goto _LL2E5;_LL2E4:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4FC=_cycalloc(
sizeof(*_tmp4FC));_tmp4FC[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4FD;_tmp4FD.tag=
20;_tmp4FD.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3A2);_tmp4FD.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE[0]=({
struct Cyc_Absyn_StructField_struct _tmp4FF;_tmp4FF.tag=0;_tmp4FF.f1=Cyc_Absyn_fieldname((
int)(_tmp3A4 + 1));_tmp4FF;});_tmp4FE;}));_tmp4FD;});_tmp4FC;})));goto _LL2DE;
_LL2E5: if(*((int*)_tmp4F0)!= 3)goto _LL2E7;_LL2E6: if(_tmp3A4 == 0)(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp500=_cycalloc(sizeof(*
_tmp500));_tmp500[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp501;_tmp501.tag=20;
_tmp501.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3A2);_tmp501.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502[0]=({
struct Cyc_Absyn_StructField_struct _tmp503;_tmp503.tag=0;_tmp503.f1=Cyc_Toc_tag_sp;
_tmp503;});_tmp502;}));_tmp501;});_tmp500;})));else{(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp505;_tmp505.tag=20;_tmp505.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp3A2);_tmp505.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp506=
_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_Absyn_StructField_struct
_tmp507;_tmp507.tag=0;_tmp507.f1=Cyc_Absyn_fieldname((int)_tmp3A4);_tmp507;});
_tmp506;}));_tmp505;});_tmp504;})));}goto _LL2DE;_LL2E7:;_LL2E8:({void*_tmp508[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp509="impossible type for offsetof tuple index";_tag_dynforward(
_tmp509,sizeof(char),_get_zero_arr_size(_tmp509,41));}),_tag_dynforward(_tmp508,
sizeof(void*),0));});_LL2DE:;}goto _LL1E5;_LL218: if(*((int*)_tmp370)!= 22)goto
_LL21A;_tmp3A5=((struct Cyc_Absyn_Deref_e_struct*)_tmp370)->f1;_LL219: {void*
_tmp50A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A5->topt))->v);{
void*_tmp50B=_tmp50A;struct Cyc_Absyn_PtrInfo _tmp50C;void*_tmp50D;struct Cyc_Absyn_Tqual
_tmp50E;struct Cyc_Absyn_PtrAtts _tmp50F;void*_tmp510;struct Cyc_Absyn_Conref*
_tmp511;struct Cyc_Absyn_Conref*_tmp512;struct Cyc_Absyn_Conref*_tmp513;_LL2EA: if(
_tmp50B <= (void*)4)goto _LL2EC;if(*((int*)_tmp50B)!= 4)goto _LL2EC;_tmp50C=((
struct Cyc_Absyn_PointerType_struct*)_tmp50B)->f1;_tmp50D=(void*)_tmp50C.elt_typ;
_tmp50E=_tmp50C.elt_tq;_tmp50F=_tmp50C.ptr_atts;_tmp510=(void*)_tmp50F.rgn;
_tmp511=_tmp50F.nullable;_tmp512=_tmp50F.bounds;_tmp513=_tmp50F.zero_term;_LL2EB:{
void*_tmp514=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp512);_LL2EF: if(
_tmp514 <= (void*)2)goto _LL2F1;if(*((int*)_tmp514)!= 0)goto _LL2F1;_LL2F0: {int
do_null_check=Cyc_Toc_need_null_check(_tmp3A5);Cyc_Toc_exp_to_c(nv,_tmp3A5);if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp515[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp516="inserted null check due to dereference";
_tag_dynforward(_tmp516,sizeof(char),_get_zero_arr_size(_tmp516,39));}),
_tag_dynforward(_tmp515,sizeof(void*),0));});(void*)(_tmp3A5->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Toc_typ_to_c(_tmp50A),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517->hd=Cyc_Absyn_copy_exp(_tmp3A5);
_tmp517->tl=0;_tmp517;}),0)));}goto _LL2EE;}_LL2F1: if((int)_tmp514 != 0)goto _LL2F3;
_LL2F2: goto _LL2F4;_LL2F3: if((int)_tmp514 != 1)goto _LL2F5;_LL2F4: {struct Cyc_Absyn_Exp*
_tmp518=Cyc_Absyn_uint_exp(0,0);_tmp518->topt=({struct Cyc_Core_Opt*_tmp519=
_cycalloc(sizeof(*_tmp519));_tmp519->v=(void*)Cyc_Absyn_uint_typ;_tmp519;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp3A5,_tmp518));Cyc_Toc_exp_to_c(nv,
e);goto _LL2EE;}_LL2F5: if(_tmp514 <= (void*)2)goto _LL2EE;if(*((int*)_tmp514)!= 1)
goto _LL2EE;_LL2F6:({void*_tmp51A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp51B="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp51B,sizeof(char),_get_zero_arr_size(_tmp51B,30));}),
_tag_dynforward(_tmp51A,sizeof(void*),0));});_LL2EE:;}goto _LL2E9;_LL2EC:;_LL2ED:({
void*_tmp51C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp51D="exp_to_c: Deref: non-pointer";
_tag_dynforward(_tmp51D,sizeof(char),_get_zero_arr_size(_tmp51D,29));}),
_tag_dynforward(_tmp51C,sizeof(void*),0));});_LL2E9:;}goto _LL1E5;}_LL21A: if(*((
int*)_tmp370)!= 23)goto _LL21C;_tmp3A6=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp370)->f1;_tmp3A7=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp370)->f2;_LL21B:
Cyc_Toc_exp_to_c(nv,_tmp3A6);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E5;
_LL21C: if(*((int*)_tmp370)!= 24)goto _LL21E;_tmp3A8=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp370)->f1;_tmp3A9=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp370)->f2;_LL21D: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A8->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp3A8);Cyc_Toc_exp_to_c(nv,_tmp3A8);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp51F;struct Cyc_Absyn_Tqual
_tmp520;struct Cyc_Absyn_PtrAtts _tmp521;void*_tmp522;struct Cyc_Absyn_Conref*
_tmp523;struct Cyc_Absyn_Conref*_tmp524;struct Cyc_Absyn_Conref*_tmp525;struct Cyc_Absyn_PtrInfo
_tmp51E=Cyc_Toc_get_ptr_type(e1typ);_tmp51F=(void*)_tmp51E.elt_typ;_tmp520=
_tmp51E.elt_tq;_tmp521=_tmp51E.ptr_atts;_tmp522=(void*)_tmp521.rgn;_tmp523=
_tmp521.nullable;_tmp524=_tmp521.bounds;_tmp525=_tmp521.zero_term;{void*_tmp526=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp524);struct Cyc_Absyn_Exp*_tmp527;
_LL2F8: if(_tmp526 <= (void*)2)goto _LL2FA;if(*((int*)_tmp526)!= 0)goto _LL2FA;
_tmp527=((struct Cyc_Absyn_Upper_b_struct*)_tmp526)->f1;_LL2F9: {unsigned int
_tmp529;int _tmp52A;struct _tuple5 _tmp528=Cyc_Evexp_eval_const_uint_exp(_tmp527);
_tmp529=_tmp528.f1;_tmp52A=_tmp528.f2;if(_tmp52A){if(_tmp529 < 1)({void*_tmp52B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp52C="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp52C,sizeof(char),_get_zero_arr_size(_tmp52C,44));}),_tag_dynforward(_tmp52B,
sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*
_tmp52D[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp52E="inserted null check due to dereference";
_tag_dynforward(_tmp52E,sizeof(char),_get_zero_arr_size(_tmp52E,39));}),
_tag_dynforward(_tmp52D,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3A8->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->hd=_tmp3A8;_tmp52F->tl=0;_tmp52F;}),
0)),_tmp3A9));}}else{(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A8->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp530[4];_tmp530[3]=Cyc_Absyn_uint_exp(0,0);_tmp530[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp51F,0);_tmp530[1]=_tmp527;_tmp530[0]=_tmp3A8;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp530,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp3A9));}goto _LL2F7;}_LL2FA: if((int)_tmp526 != 0)goto _LL2FC;_LL2FB:
forward_only=1;goto _LL2FD;_LL2FC: if((int)_tmp526 != 1)goto _LL2FE;_LL2FD: {struct
Cyc_Absyn_Exp*_tmp531=forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;
void*ta1=Cyc_Toc_typ_to_c_array(_tmp51F);(void*)(_tmp3A8->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp520),Cyc_Absyn_fncall_exp(_tmp531,({struct Cyc_Absyn_Exp*
_tmp532[3];_tmp532[2]=Cyc_Absyn_uint_exp(0,0);_tmp532[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp532[0]=Cyc_Absyn_copy_exp(_tmp3A8);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp532,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)));goto _LL2F7;}_LL2FE: if(_tmp526 <= (void*)2)goto _LL2F7;if(*((int*)
_tmp526)!= 1)goto _LL2F7;_LL2FF:({void*_tmp533[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp534="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp534,sizeof(char),_get_zero_arr_size(_tmp534,34));}),
_tag_dynforward(_tmp533,sizeof(void*),0));});_LL2F7:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E5;}}
_LL21E: if(*((int*)_tmp370)!= 25)goto _LL220;_tmp3AA=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp370)->f1;_tmp3AB=((struct Cyc_Absyn_Subscript_e_struct*)_tmp370)->f2;_LL21F: {
void*_tmp535=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3AA->topt))->v);{void*_tmp536=_tmp535;struct Cyc_List_List*_tmp537;struct Cyc_Absyn_PtrInfo
_tmp538;void*_tmp539;struct Cyc_Absyn_Tqual _tmp53A;struct Cyc_Absyn_PtrAtts _tmp53B;
void*_tmp53C;struct Cyc_Absyn_Conref*_tmp53D;struct Cyc_Absyn_Conref*_tmp53E;
struct Cyc_Absyn_Conref*_tmp53F;_LL301: if(_tmp536 <= (void*)4)goto _LL305;if(*((int*)
_tmp536)!= 9)goto _LL303;_tmp537=((struct Cyc_Absyn_TupleType_struct*)_tmp536)->f1;
_LL302: Cyc_Toc_exp_to_c(nv,_tmp3AA);Cyc_Toc_exp_to_c(nv,_tmp3AB);{unsigned int
_tmp541;int _tmp542;struct _tuple5 _tmp540=Cyc_Evexp_eval_const_uint_exp(_tmp3AB);
_tmp541=_tmp540.f1;_tmp542=_tmp540.f2;if(!_tmp542)({void*_tmp543[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp544="unknown tuple subscript in translation to C";_tag_dynforward(_tmp544,
sizeof(char),_get_zero_arr_size(_tmp544,44));}),_tag_dynforward(_tmp543,sizeof(
void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp3AA,Cyc_Absyn_fieldname((
int)(_tmp541 + 1))));goto _LL300;}_LL303: if(*((int*)_tmp536)!= 4)goto _LL305;
_tmp538=((struct Cyc_Absyn_PointerType_struct*)_tmp536)->f1;_tmp539=(void*)
_tmp538.elt_typ;_tmp53A=_tmp538.elt_tq;_tmp53B=_tmp538.ptr_atts;_tmp53C=(void*)
_tmp53B.rgn;_tmp53D=_tmp53B.nullable;_tmp53E=_tmp53B.bounds;_tmp53F=_tmp53B.zero_term;
_LL304: {struct Cyc_List_List*_tmp545=Cyc_Toc_get_relns(_tmp3AA);int in_bnds=0;{
void*_tmp546=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp53E);_LL308: if(
_tmp546 <= (void*)2)goto _LL30A;if(*((int*)_tmp546)!= 1)goto _LL30A;_LL309: goto
_LL307;_LL30A:;_LL30B: in_bnds=Cyc_Toc_check_bounds(_tmp545,_tmp3AA,_tmp3AB);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp549;
_tmp549.tag=0;_tmp549.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp547[1]={& _tmp549};Cyc_Tcutil_warn(e->loc,({const char*_tmp548="bounds check necessary for %s";
_tag_dynforward(_tmp548,sizeof(char),_get_zero_arr_size(_tmp548,30));}),
_tag_dynforward(_tmp547,sizeof(void*),1));}});_LL307:;}Cyc_Toc_exp_to_c(nv,
_tmp3AA);Cyc_Toc_exp_to_c(nv,_tmp3AB);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp54A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp53E);
struct Cyc_Absyn_Exp*_tmp54B;void*_tmp54C;_LL30D: if(_tmp54A <= (void*)2)goto _LL30F;
if(*((int*)_tmp54A)!= 0)goto _LL30F;_tmp54B=((struct Cyc_Absyn_Upper_b_struct*)
_tmp54A)->f1;_LL30E: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp53D);void*ta1=Cyc_Toc_typ_to_c(_tmp539);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp53A);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp53F))(void*)(e->r=(void*)
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp54D[3];_tmp54D[2]=_tmp3AB;_tmp54D[1]=_tmp54B;_tmp54D[0]=
_tmp3AA;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp54D,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));else{if(
possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp54E[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp54F="inserted null check due to dereference";
_tag_dynforward(_tmp54F,sizeof(char),_get_zero_arr_size(_tmp54F,39));}),
_tag_dynforward(_tmp54E,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp550[4];_tmp550[3]=_tmp3AB;_tmp550[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp550[1]=_tmp54B;_tmp550[0]=_tmp3AA;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp550,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))));}else{(void*)(_tmp3AB->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp551[2];_tmp551[1]=Cyc_Absyn_copy_exp(_tmp3AB);_tmp551[0]=
_tmp54B;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp551,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL30C;}
_LL30F: if((int)_tmp54A != 0)goto _LL311;_LL310: forward_only=1;goto _LL312;_LL311:
if((int)_tmp54A != 1)goto _LL313;_LL312: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp539);
if(in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp53A),Cyc_Absyn_aggrmember_exp(
_tmp3AA,Cyc_Toc_curr_sp,0)),_tmp3AB));}else{struct Cyc_Absyn_Exp*_tmp552=
forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp53A),Cyc_Absyn_fncall_exp(_tmp552,({struct Cyc_Absyn_Exp*_tmp553[3];_tmp553[2]=
_tmp3AB;_tmp553[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp553[0]=_tmp3AA;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp553,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL30C;}_LL313: if(_tmp54A <= (void*)2)goto _LL30C;if(*((int*)
_tmp54A)!= 1)goto _LL30C;_tmp54C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp54A)->f1;_LL314:{void*_tmp554=(void*)_tmp3AB->annot;struct Cyc_List_List*
_tmp555;_LL316: if(*((void**)_tmp554)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL318;
_tmp555=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp554)->f1;_LL317: for(0;
_tmp555 != 0;_tmp555=_tmp555->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp555->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp555->hd)->bd,_tmp54C)== 0){in_bnds=1;break;}}goto _LL315;_LL318:;_LL319: goto
_LL315;_LL315:;}if(!in_bnds)({void*_tmp556[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp557="cannot determine that subscript is in bounds";_tag_dynforward(
_tmp557,sizeof(char),_get_zero_arr_size(_tmp557,45));}),_tag_dynforward(_tmp556,
sizeof(void*),0));});goto _LL30C;_LL30C:;}goto _LL300;}}_LL305:;_LL306:({void*
_tmp558[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp559="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp559,sizeof(char),_get_zero_arr_size(_tmp559,49));}),
_tag_dynforward(_tmp558,sizeof(void*),0));});_LL300:;}goto _LL1E5;}_LL220: if(*((
int*)_tmp370)!= 26)goto _LL222;_tmp3AC=((struct Cyc_Absyn_Tuple_e_struct*)_tmp370)->f1;
_LL221: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp3AC))->r));else{struct Cyc_List_List*_tmp55A=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp3AC);struct _dynforward_ptr*_tmp55B=Cyc_Toc_add_tuple_type(
_tmp55A);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp3AC != 0;(_tmp3AC=_tmp3AC->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp3AC->hd);dles=({struct Cyc_List_List*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->hd=({struct _tuple6*_tmp55D=
_cycalloc(sizeof(*_tmp55D));_tmp55D->f1=0;_tmp55D->f2=(struct Cyc_Absyn_Exp*)
_tmp3AC->hd;_tmp55D;});_tmp55C->tl=dles;_tmp55C;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,dles));}goto _LL1E5;_LL222: if(*((int*)_tmp370)!= 28)goto _LL224;_tmp3AD=((struct
Cyc_Absyn_Array_e_struct*)_tmp370)->f1;_LL223:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp3AD));{struct Cyc_List_List*_tmp55E=_tmp3AD;for(0;_tmp55E != 0;_tmp55E=
_tmp55E->tl){struct _tuple6 _tmp560;struct Cyc_Absyn_Exp*_tmp561;struct _tuple6*
_tmp55F=(struct _tuple6*)_tmp55E->hd;_tmp560=*_tmp55F;_tmp561=_tmp560.f2;Cyc_Toc_exp_to_c(
nv,_tmp561);}}goto _LL1E5;_LL224: if(*((int*)_tmp370)!= 29)goto _LL226;_tmp3AE=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp370)->f1;_tmp3AF=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp370)->f2;_tmp3B0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp370)->f3;
_tmp3B1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp370)->f4;_LL225: {
unsigned int _tmp563;int _tmp564;struct _tuple5 _tmp562=Cyc_Evexp_eval_const_uint_exp(
_tmp3AF);_tmp563=_tmp562.f1;_tmp564=_tmp562.f2;{void*_tmp565=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B0->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3B0);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp3B0)){if(!_tmp564)({
void*_tmp566[0]={};Cyc_Tcutil_terr(_tmp3AF->loc,({const char*_tmp567="cannot determine value of constant";
_tag_dynforward(_tmp567,sizeof(char),_get_zero_arr_size(_tmp567,35));}),
_tag_dynforward(_tmp566,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp563;i
++){es=({struct Cyc_List_List*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->hd=({
struct _tuple6*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->f1=0;_tmp569->f2=
_tmp3B0;_tmp569;});_tmp568->tl=es;_tmp568;});}}if(_tmp3B1){struct Cyc_Absyn_Exp*
_tmp56A=Cyc_Toc_cast_it(_tmp565,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->hd=({struct _tuple6*_tmp56C=
_cycalloc(sizeof(*_tmp56C));_tmp56C->f1=0;_tmp56C->f2=_tmp56A;_tmp56C;});_tmp56B->tl=
0;_tmp56B;}));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,es));goto
_LL1E5;}}}_LL226: if(*((int*)_tmp370)!= 30)goto _LL228;_tmp3B2=((struct Cyc_Absyn_Struct_e_struct*)
_tmp370)->f1;_tmp3B3=((struct Cyc_Absyn_Struct_e_struct*)_tmp370)->f2;_tmp3B4=((
struct Cyc_Absyn_Struct_e_struct*)_tmp370)->f3;_tmp3B5=((struct Cyc_Absyn_Struct_e_struct*)
_tmp370)->f4;_LL227: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp3B3 != 0,0,0,_tmp3B4,_tmp3B2))->r));else{if(_tmp3B5 == 0)({void*
_tmp56D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp56E="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp56E,
sizeof(char),_get_zero_arr_size(_tmp56E,35));}),_tag_dynforward(_tmp56D,sizeof(
void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp3B5;
struct _RegionHandle _tmp56F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp56F;
_push_region(rgn);{struct Cyc_List_List*_tmp570=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp3B4,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp571=0;struct Cyc_List_List*_tmp572=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp572 != 0;_tmp572=_tmp572->tl){struct Cyc_List_List*
_tmp573=_tmp570;for(0;_tmp573 != 0;_tmp573=_tmp573->tl){if((*((struct _tuple10*)
_tmp573->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp572->hd){struct _tuple10
_tmp575;struct Cyc_Absyn_Aggrfield*_tmp576;struct Cyc_Absyn_Exp*_tmp577;struct
_tuple10*_tmp574=(struct _tuple10*)_tmp573->hd;_tmp575=*_tmp574;_tmp576=_tmp575.f1;
_tmp577=_tmp575.f2;{void*_tmp578=(void*)_tmp576->type;Cyc_Toc_exp_to_c(nv,
_tmp577);if(Cyc_Toc_is_void_star(_tmp578))(void*)(_tmp577->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp577->r,0)));_tmp571=({
struct Cyc_List_List*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->hd=({struct
_tuple6*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->f1=0;_tmp57A->f2=_tmp577;
_tmp57A;});_tmp579->tl=_tmp571;_tmp579;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp571)));};
_pop_region(rgn);}}goto _LL1E5;_LL228: if(*((int*)_tmp370)!= 31)goto _LL22A;_tmp3B6=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp370)->f1;_tmp3B7=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp370)->f2;_LL229: {struct Cyc_List_List*fs;{void*_tmp57B=Cyc_Tcutil_compress(
_tmp3B6);struct Cyc_List_List*_tmp57C;_LL31B: if(_tmp57B <= (void*)4)goto _LL31D;if(*((
int*)_tmp57B)!= 11)goto _LL31D;_tmp57C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp57B)->f2;_LL31C: fs=_tmp57C;goto _LL31A;_LL31D:;_LL31E:({struct Cyc_String_pa_struct
_tmp57F;_tmp57F.tag=0;_tmp57F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp3B6));{void*_tmp57D[1]={& _tmp57F};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp57E="anon struct has type %s";_tag_dynforward(_tmp57E,sizeof(char),
_get_zero_arr_size(_tmp57E,24));}),_tag_dynforward(_tmp57D,sizeof(void*),1));}});
_LL31A:;}{struct _RegionHandle _tmp580=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp580;_push_region(rgn);{struct Cyc_List_List*_tmp581=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,
_tmp3B7,fs);for(0;_tmp581 != 0;_tmp581=_tmp581->tl){struct _tuple10 _tmp583;struct
Cyc_Absyn_Aggrfield*_tmp584;struct Cyc_Absyn_Exp*_tmp585;struct _tuple10*_tmp582=(
struct _tuple10*)_tmp581->hd;_tmp583=*_tmp582;_tmp584=_tmp583.f1;_tmp585=_tmp583.f2;{
void*_tmp586=(void*)_tmp584->type;Cyc_Toc_exp_to_c(nv,_tmp585);if(Cyc_Toc_is_void_star(
_tmp586))(void*)(_tmp585->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp585->r,0)));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp3B7));};_pop_region(rgn);}goto _LL1E5;}_LL22A: if(*((int*)_tmp370)!= 32)goto
_LL22C;_tmp3B8=((struct Cyc_Absyn_Tunion_e_struct*)_tmp370)->f1;if(_tmp3B8 != 0)
goto _LL22C;_tmp3B9=((struct Cyc_Absyn_Tunion_e_struct*)_tmp370)->f2;_tmp3BA=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp370)->f3;if(!(!_tmp3B9->is_flat))goto _LL22C;
_LL22B: {struct _tuple1*qv=_tmp3BA->name;struct Cyc_Absyn_Exp*tag_exp=_tmp3B9->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp3B9,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1E5;}_LL22C: if(*((int*)_tmp370)!= 32)goto _LL22E;
_tmp3BB=((struct Cyc_Absyn_Tunion_e_struct*)_tmp370)->f1;_tmp3BC=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp370)->f2;_tmp3BD=((struct Cyc_Absyn_Tunion_e_struct*)_tmp370)->f3;_LL22D: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp587=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp588=Cyc_Absyn_var_exp(_tmp587,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp3BC->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp3BC->name,({const char*_tmp589="_union";_tag_dynforward(_tmp589,sizeof(char),
_get_zero_arr_size(_tmp589,7));})));tag_exp=Cyc_Toc_tunion_tag(_tmp3BC,_tmp3BD->name,
1);{struct _tuple1 _tmp58B;struct _dynforward_ptr*_tmp58C;struct _tuple1*_tmp58A=
_tmp3BD->name;_tmp58B=*_tmp58A;_tmp58C=_tmp58B.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp588,_tmp58C,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3BD->name,({const char*_tmp58D="_struct";_tag_dynforward(_tmp58D,sizeof(char),
_get_zero_arr_size(_tmp58D,8));})));tag_exp=_tmp3BC->is_xtunion?Cyc_Absyn_var_exp(
_tmp3BD->name,0): Cyc_Toc_tunion_tag(_tmp3BC,_tmp3BD->name,1);member_exp=_tmp588;}{
struct Cyc_List_List*_tmp58E=_tmp3BD->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*
dles=0;for(0;_tmp3BB != 0;(_tmp3BB=_tmp3BB->tl,_tmp58E=_tmp58E->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp3BB->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp58E))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->hd=({
struct _tuple6*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->f1=0;_tmp590->f2=
cur_e;_tmp590;});_tmp58F->tl=dles;_tmp58F;});}dles=({struct Cyc_List_List*_tmp591=
_cycalloc(sizeof(*_tmp591));_tmp591->hd=({struct _tuple6*_tmp592=_cycalloc(
sizeof(*_tmp592));_tmp592->f1=0;_tmp592->f2=tag_exp;_tmp592;});_tmp591->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp591;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp3BC->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3BD->name,({const char*_tmp593="_struct";_tag_dynforward(_tmp593,sizeof(char),
_get_zero_arr_size(_tmp593,8));}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*
_tmp594=({struct Cyc_List_List*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),
tag_exp,0);_tmp59A->tl=0;_tmp59A;});{int i=1;for(0;_tmp3BB != 0;(((_tmp3BB=_tmp3BB->tl,
i ++)),_tmp58E=_tmp58E->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp3BB->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp58E))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp595=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);_tmp594=({struct Cyc_List_List*_tmp596=_cycalloc(sizeof(*_tmp596));
_tmp596->hd=_tmp595;_tmp596->tl=_tmp594;_tmp596;});}}}{struct Cyc_Absyn_Stmt*
_tmp597=Cyc_Absyn_exp_stmt(_tmp588,0);struct Cyc_Absyn_Stmt*_tmp598=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->hd=_tmp597;_tmp599->tl=_tmp594;
_tmp599;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(
_tmp587,tunion_ctype,0,_tmp598,0)));}}goto _LL1E5;}}_LL22E: if(*((int*)_tmp370)!= 
33)goto _LL230;_LL22F: goto _LL231;_LL230: if(*((int*)_tmp370)!= 34)goto _LL232;
_LL231: goto _LL1E5;_LL232: if(*((int*)_tmp370)!= 35)goto _LL234;_tmp3BE=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp370)->f1;_tmp3BF=_tmp3BE.is_calloc;_tmp3C0=_tmp3BE.rgn;_tmp3C1=_tmp3BE.elt_type;
_tmp3C2=_tmp3BE.num_elts;_tmp3C3=_tmp3BE.fat_result;_LL233: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp3C1)));Cyc_Toc_exp_to_c(nv,_tmp3C2);if(_tmp3C3){struct
_tuple1*_tmp59B=Cyc_Toc_temp_var();struct _tuple1*_tmp59C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp3BF){xexp=_tmp3C2;if(_tmp3C0 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp3C0;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp59B,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp3C1,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp59B,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp59D[3];_tmp59D[2]=Cyc_Absyn_var_exp(_tmp59B,0);_tmp59D[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp59D[0]=Cyc_Absyn_var_exp(_tmp59C,0);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp59D,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp3C2,0);if(_tmp3C0 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp3C0;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp59B,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp3C1,Cyc_Absyn_var_exp(_tmp59B,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp59E[3];_tmp59E[2]=Cyc_Absyn_var_exp(_tmp59B,0);_tmp59E[1]=Cyc_Absyn_uint_exp(
1,0);_tmp59E[0]=Cyc_Absyn_var_exp(_tmp59C,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp59E,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp59F=Cyc_Absyn_declare_stmt(_tmp59B,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp59C,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp59F));}}else{if(_tmp3C0 != 0){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp3C0;Cyc_Toc_exp_to_c(nv,rgn);(void*)(e->r=(void*)((
void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}else{(void*)(
e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp3C1,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}}
goto _LL1E5;}_LL234: if(*((int*)_tmp370)!= 37)goto _LL236;_tmp3C4=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp370)->f1;_LL235: Cyc_Toc_stmt_to_c(nv,_tmp3C4);goto _LL1E5;_LL236: if(*((int*)
_tmp370)!= 36)goto _LL238;_LL237:({void*_tmp5A0[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp5A1="UnresolvedMem";_tag_dynforward(_tmp5A1,sizeof(char),_get_zero_arr_size(
_tmp5A1,14));}),_tag_dynforward(_tmp5A0,sizeof(void*),0));});_LL238: if(*((int*)
_tmp370)!= 27)goto _LL1E5;_LL239:({void*_tmp5A2[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5A3="compoundlit";
_tag_dynforward(_tmp5A3,sizeof(char),_get_zero_arr_size(_tmp5A3,12));}),
_tag_dynforward(_tmp5A2,sizeof(void*),0));});_LL1E5:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple11{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple12{struct _tuple1*f1;void*f2;};struct
_tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp5A4=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp5A5;struct Cyc_Absyn_Vardecl
_tmp5A6;struct _tuple1*_tmp5A7;struct Cyc_Absyn_Pat*_tmp5A8;struct Cyc_Absyn_Vardecl*
_tmp5A9;struct Cyc_Absyn_Vardecl _tmp5AA;struct _tuple1*_tmp5AB;struct Cyc_Absyn_Vardecl*
_tmp5AC;struct Cyc_Absyn_Pat*_tmp5AD;void*_tmp5AE;int _tmp5AF;char _tmp5B0;struct
_dynforward_ptr _tmp5B1;struct Cyc_Absyn_Enumdecl*_tmp5B2;struct Cyc_Absyn_Enumfield*
_tmp5B3;void*_tmp5B4;struct Cyc_Absyn_Enumfield*_tmp5B5;struct Cyc_Absyn_Tuniondecl*
_tmp5B6;struct Cyc_Absyn_Tunionfield*_tmp5B7;struct Cyc_List_List*_tmp5B8;struct
Cyc_Absyn_Pat*_tmp5B9;struct Cyc_Absyn_Pat _tmp5BA;void*_tmp5BB;struct Cyc_Absyn_Tuniondecl*
_tmp5BC;struct Cyc_Absyn_Tunionfield*_tmp5BD;struct Cyc_List_List*_tmp5BE;struct
Cyc_Absyn_Tuniondecl*_tmp5BF;struct Cyc_Absyn_Tunionfield*_tmp5C0;struct Cyc_List_List*
_tmp5C1;struct Cyc_List_List*_tmp5C2;struct Cyc_List_List*_tmp5C3;struct Cyc_Absyn_AggrInfo
_tmp5C4;union Cyc_Absyn_AggrInfoU_union _tmp5C5;struct Cyc_List_List*_tmp5C6;struct
Cyc_Absyn_Pat*_tmp5C7;_LL320: if(_tmp5A4 <= (void*)2)goto _LL324;if(*((int*)_tmp5A4)
!= 0)goto _LL322;_tmp5A5=((struct Cyc_Absyn_Var_p_struct*)_tmp5A4)->f1;_tmp5A6=*
_tmp5A5;_tmp5A7=_tmp5A6.name;_tmp5A8=((struct Cyc_Absyn_Var_p_struct*)_tmp5A4)->f2;
_LL321: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp5A7,r),rgn,t,r,path,
_tmp5A8,tag_fail_stmt,fail_stmt,decls);_LL322: if(*((int*)_tmp5A4)!= 2)goto _LL324;
_tmp5A9=((struct Cyc_Absyn_TagInt_p_struct*)_tmp5A4)->f2;_tmp5AA=*_tmp5A9;_tmp5AB=
_tmp5AA.name;_LL323: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp5AB,r);goto _LL325;_LL324:
if((int)_tmp5A4 != 0)goto _LL326;_LL325: s=Cyc_Toc_skip_stmt_dl();goto _LL31F;_LL326:
if(_tmp5A4 <= (void*)2)goto _LL328;if(*((int*)_tmp5A4)!= 1)goto _LL328;_tmp5AC=((
struct Cyc_Absyn_Reference_p_struct*)_tmp5A4)->f1;_tmp5AD=((struct Cyc_Absyn_Reference_p_struct*)
_tmp5A4)->f2;_LL327: {struct _tuple1*_tmp5C8=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp5C9=_region_malloc(rgn,sizeof(*_tmp5C9));_tmp5C9->hd=({struct _tuple12*
_tmp5CA=_region_malloc(rgn,sizeof(*_tmp5CA));_tmp5CA->f1=_tmp5C8;_tmp5CA->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp5CA;});_tmp5C9->tl=decls;_tmp5C9;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp5AC->name,Cyc_Absyn_var_exp(_tmp5C8,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp5C8,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple11 _tmp5CB=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp5AD,tag_fail_stmt,fail_stmt,decls);_tmp5CB.f3=Cyc_Absyn_seq_stmt(
s,_tmp5CB.f3,0);return _tmp5CB;}}_LL328: if((int)_tmp5A4 != 1)goto _LL32A;_LL329: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL31F;_LL32A:
if(_tmp5A4 <= (void*)2)goto _LL32C;if(*((int*)_tmp5A4)!= 7)goto _LL32C;_tmp5AE=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp5A4)->f1;_tmp5AF=((struct Cyc_Absyn_Int_p_struct*)
_tmp5A4)->f2;_LL32B: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp5AE,_tmp5AF,0),
fail_stmt);goto _LL31F;_LL32C: if(_tmp5A4 <= (void*)2)goto _LL32E;if(*((int*)_tmp5A4)
!= 8)goto _LL32E;_tmp5B0=((struct Cyc_Absyn_Char_p_struct*)_tmp5A4)->f1;_LL32D: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp5B0,0),fail_stmt);goto _LL31F;_LL32E:
if(_tmp5A4 <= (void*)2)goto _LL330;if(*((int*)_tmp5A4)!= 9)goto _LL330;_tmp5B1=((
struct Cyc_Absyn_Float_p_struct*)_tmp5A4)->f1;_LL32F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp5B1,0),fail_stmt);goto _LL31F;_LL330: if(_tmp5A4 <= (void*)2)goto _LL332;if(*((
int*)_tmp5A4)!= 10)goto _LL332;_tmp5B2=((struct Cyc_Absyn_Enum_p_struct*)_tmp5A4)->f1;
_tmp5B3=((struct Cyc_Absyn_Enum_p_struct*)_tmp5A4)->f2;_LL331: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp5CC=_cycalloc(
sizeof(*_tmp5CC));_tmp5CC[0]=({struct Cyc_Absyn_Enum_e_struct _tmp5CD;_tmp5CD.tag=
33;_tmp5CD.f1=_tmp5B3->name;_tmp5CD.f2=(struct Cyc_Absyn_Enumdecl*)_tmp5B2;
_tmp5CD.f3=(struct Cyc_Absyn_Enumfield*)_tmp5B3;_tmp5CD;});_tmp5CC;}),0),
fail_stmt);goto _LL31F;_LL332: if(_tmp5A4 <= (void*)2)goto _LL334;if(*((int*)_tmp5A4)
!= 11)goto _LL334;_tmp5B4=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp5A4)->f1;
_tmp5B5=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp5A4)->f2;_LL333: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp5CE=_cycalloc(
sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp5CF;_tmp5CF.tag=
34;_tmp5CF.f1=_tmp5B5->name;_tmp5CF.f2=(void*)_tmp5B4;_tmp5CF.f3=(struct Cyc_Absyn_Enumfield*)
_tmp5B5;_tmp5CF;});_tmp5CE;}),0),fail_stmt);goto _LL31F;_LL334: if(_tmp5A4 <= (void*)
2)goto _LL336;if(*((int*)_tmp5A4)!= 6)goto _LL336;_tmp5B6=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5A4)->f1;_tmp5B7=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5A4)->f2;_tmp5B8=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp5A4)->f3;if(_tmp5B8 != 0)goto _LL336;if(!(!
_tmp5B6->is_flat))goto _LL336;_LL335: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp5B6->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp5B7->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp5B6,_tmp5B7->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL31F;}_LL336: if(_tmp5A4 <= (void*)2)goto _LL338;if(*((
int*)_tmp5A4)!= 4)goto _LL338;_tmp5B9=((struct Cyc_Absyn_Pointer_p_struct*)_tmp5A4)->f1;
_tmp5BA=*_tmp5B9;_tmp5BB=(void*)_tmp5BA.r;if(_tmp5BB <= (void*)2)goto _LL338;if(*((
int*)_tmp5BB)!= 6)goto _LL338;_tmp5BC=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5BB)->f1;
_tmp5BD=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5BB)->f2;_tmp5BE=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5BB)->f3;if(!(_tmp5BE != 0  && !_tmp5BC->is_flat))goto _LL338;_LL337: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp5BD->name,({const
char*_tmp5DC="_struct";_tag_dynforward(_tmp5DC,sizeof(char),_get_zero_arr_size(
_tmp5DC,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp5D0=_tmp5BD->typs;
for(0;_tmp5BE != 0;(((_tmp5BE=_tmp5BE->tl,_tmp5D0=((struct Cyc_List_List*)
_check_null(_tmp5D0))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp5D1=(struct Cyc_Absyn_Pat*)
_tmp5BE->hd;if((void*)_tmp5D1->r == (void*)0)continue;{void*_tmp5D2=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp5D0))->hd)).f2;struct _tuple1*
_tmp5D3=Cyc_Toc_temp_var();void*_tmp5D4=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5D1->topt))->v;void*_tmp5D5=Cyc_Toc_typ_to_c(_tmp5D4);struct Cyc_Absyn_Exp*
_tmp5D6=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp5D2)))_tmp5D6=Cyc_Toc_cast_it(_tmp5D5,_tmp5D6);decls=({
struct Cyc_List_List*_tmp5D7=_region_malloc(rgn,sizeof(*_tmp5D7));_tmp5D7->hd=({
struct _tuple12*_tmp5D8=_region_malloc(rgn,sizeof(*_tmp5D8));_tmp5D8->f1=_tmp5D3;
_tmp5D8->f2=_tmp5D5;_tmp5D8;});_tmp5D7->tl=decls;_tmp5D7;});{struct _tuple11
_tmp5D9=Cyc_Toc_xlate_pat(nv,rgn,_tmp5D4,Cyc_Absyn_var_exp(_tmp5D3,0),_tmp5D6,
_tmp5D1,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5D9.f1;decls=
_tmp5D9.f2;{struct Cyc_Absyn_Stmt*_tmp5DA=_tmp5D9.f3;struct Cyc_Absyn_Stmt*_tmp5DB=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5D3,0),_tmp5D6,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5DB,_tmp5DA,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp5BC->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp5BD->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp5BC,_tmp5BD->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp5BC);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL31F;}}_LL338: if(_tmp5A4 <= (void*)2)goto
_LL33A;if(*((int*)_tmp5A4)!= 6)goto _LL33A;_tmp5BF=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5A4)->f1;_tmp5C0=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5A4)->f2;_tmp5C1=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp5A4)->f3;if(!_tmp5BF->is_flat)goto _LL33A;
_LL339: {struct _tuple1 _tmp5DE;struct _dynforward_ptr*_tmp5DF;struct _tuple1*
_tmp5DD=_tmp5C0->name;_tmp5DE=*_tmp5DD;_tmp5DF=_tmp5DE.f2;r=Cyc_Absyn_aggrmember_exp(
r,_tmp5DF,0);path=Cyc_Absyn_aggrmember_exp(path,_tmp5DF,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
_tmp5BF,_tmp5C0->name,1),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;
_tmp5C1 != 0;(_tmp5C1=_tmp5C1->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5E0=(struct Cyc_Absyn_Pat*)
_tmp5C1->hd;if((void*)_tmp5E0->r == (void*)0)continue;{struct _tuple1*_tmp5E1=Cyc_Toc_temp_var();
void*_tmp5E2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5E0->topt))->v;decls=({
struct Cyc_List_List*_tmp5E3=_region_malloc(rgn,sizeof(*_tmp5E3));_tmp5E3->hd=({
struct _tuple12*_tmp5E4=_region_malloc(rgn,sizeof(*_tmp5E4));_tmp5E4->f1=_tmp5E1;
_tmp5E4->f2=Cyc_Toc_typ_to_c(_tmp5E2);_tmp5E4;});_tmp5E3->tl=decls;_tmp5E3;});{
struct _tuple11 _tmp5E5=Cyc_Toc_xlate_pat(nv,rgn,_tmp5E2,Cyc_Absyn_var_exp(_tmp5E1,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5E0,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp5E5.f1;decls=_tmp5E5.f2;{struct Cyc_Absyn_Stmt*
_tmp5E6=_tmp5E5.f3;struct Cyc_Absyn_Stmt*_tmp5E7=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5E1,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5E7,_tmp5E6,0),0);}}}}goto _LL31F;}}_LL33A: if(_tmp5A4 <= (
void*)2)goto _LL33C;if(*((int*)_tmp5A4)!= 6)goto _LL33C;_tmp5C2=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5A4)->f3;_LL33B: _tmp5C3=_tmp5C2;goto _LL33D;_LL33C: if(_tmp5A4 <= (void*)2)goto
_LL33E;if(*((int*)_tmp5A4)!= 3)goto _LL33E;_tmp5C3=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp5A4)->f1;_LL33D: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp5C3 != 0;(
_tmp5C3=_tmp5C3->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5E8=(struct Cyc_Absyn_Pat*)
_tmp5C3->hd;if((void*)_tmp5E8->r == (void*)0)continue;{struct _tuple1*_tmp5E9=Cyc_Toc_temp_var();
void*_tmp5EA=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5E8->topt))->v;decls=({
struct Cyc_List_List*_tmp5EB=_region_malloc(rgn,sizeof(*_tmp5EB));_tmp5EB->hd=({
struct _tuple12*_tmp5EC=_region_malloc(rgn,sizeof(*_tmp5EC));_tmp5EC->f1=_tmp5E9;
_tmp5EC->f2=Cyc_Toc_typ_to_c(_tmp5EA);_tmp5EC;});_tmp5EB->tl=decls;_tmp5EB;});{
struct _tuple11 _tmp5ED=Cyc_Toc_xlate_pat(nv,rgn,_tmp5EA,Cyc_Absyn_var_exp(_tmp5E9,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5E8,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp5ED.f1;decls=_tmp5ED.f2;{struct Cyc_Absyn_Stmt*
_tmp5EE=_tmp5ED.f3;struct Cyc_Absyn_Stmt*_tmp5EF=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5E9,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5EF,_tmp5EE,0),0);}}}}goto _LL31F;}_LL33E: if(_tmp5A4 <= (
void*)2)goto _LL340;if(*((int*)_tmp5A4)!= 5)goto _LL340;_tmp5C4=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp5A4)->f1;_tmp5C5=_tmp5C4.aggr_info;_tmp5C6=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp5A4)->f3;_LL33F: {struct Cyc_Absyn_Aggrdecl*_tmp5F0=Cyc_Absyn_get_known_aggrdecl(
_tmp5C5);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp5C6 != 0;_tmp5C6=_tmp5C6->tl){struct
_tuple13*_tmp5F1=(struct _tuple13*)_tmp5C6->hd;struct Cyc_Absyn_Pat*_tmp5F2=(*
_tmp5F1).f2;if((void*)_tmp5F2->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp5F3=(void*)((struct Cyc_List_List*)_check_null((*_tmp5F1).f1))->hd;
struct _dynforward_ptr*_tmp5F4;_LL349: if(*((int*)_tmp5F3)!= 1)goto _LL34B;_tmp5F4=((
struct Cyc_Absyn_FieldName_struct*)_tmp5F3)->f1;_LL34A: f=_tmp5F4;goto _LL348;
_LL34B:;_LL34C:(int)_throw((void*)Cyc_Toc_Match_error);_LL348:;}{struct _tuple1*
_tmp5F5=Cyc_Toc_temp_var();void*_tmp5F6=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5F2->topt))->v;void*_tmp5F7=Cyc_Toc_typ_to_c(_tmp5F6);decls=({struct Cyc_List_List*
_tmp5F8=_region_malloc(rgn,sizeof(*_tmp5F8));_tmp5F8->hd=({struct _tuple12*
_tmp5F9=_region_malloc(rgn,sizeof(*_tmp5F9));_tmp5F9->f1=_tmp5F5;_tmp5F9->f2=
_tmp5F7;_tmp5F9;});_tmp5F8->tl=decls;_tmp5F8;});{struct _tuple11 _tmp5FA=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5F6,Cyc_Absyn_var_exp(_tmp5F5,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp5F2,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5FA.f1;decls=
_tmp5FA.f2;{struct Cyc_Absyn_Exp*_tmp5FB=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5F0->impl))->fields,f)))->type))_tmp5FB=
Cyc_Toc_cast_it(_tmp5F7,_tmp5FB);{struct Cyc_Absyn_Stmt*_tmp5FC=_tmp5FA.f3;struct
Cyc_Absyn_Stmt*_tmp5FD=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5F5,0),
_tmp5FB,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp5FD,_tmp5FC,0),0);}}}}}}
goto _LL31F;}_LL340: if(_tmp5A4 <= (void*)2)goto _LL342;if(*((int*)_tmp5A4)!= 4)goto
_LL342;_tmp5C7=((struct Cyc_Absyn_Pointer_p_struct*)_tmp5A4)->f1;_LL341: {struct
_tuple1*_tmp5FE=Cyc_Toc_temp_var();void*_tmp5FF=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5C7->topt))->v;decls=({struct Cyc_List_List*_tmp600=
_region_malloc(rgn,sizeof(*_tmp600));_tmp600->hd=({struct _tuple12*_tmp601=
_region_malloc(rgn,sizeof(*_tmp601));_tmp601->f1=_tmp5FE;_tmp601->f2=Cyc_Toc_typ_to_c(
_tmp5FF);_tmp601;});_tmp600->tl=decls;_tmp600;});{struct _tuple11 _tmp602=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5FF,Cyc_Absyn_var_exp(_tmp5FE,0),Cyc_Absyn_deref_exp(path,0),_tmp5C7,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp602.f1;decls=_tmp602.f2;{
struct Cyc_Absyn_Stmt*_tmp603=_tmp602.f3;struct Cyc_Absyn_Stmt*_tmp604=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5FE,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp603,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp604,0);else{s=_tmp604;}goto _LL31F;}}}_LL342: if(_tmp5A4 <= (void*)2)goto
_LL344;if(*((int*)_tmp5A4)!= 12)goto _LL344;_LL343:({void*_tmp605[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp606="unknownid pat";_tag_dynforward(_tmp606,sizeof(char),_get_zero_arr_size(
_tmp606,14));}),_tag_dynforward(_tmp605,sizeof(void*),0));});_LL344: if(_tmp5A4 <= (
void*)2)goto _LL346;if(*((int*)_tmp5A4)!= 13)goto _LL346;_LL345:({void*_tmp607[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp608="unknowncall pat";_tag_dynforward(_tmp608,sizeof(char),
_get_zero_arr_size(_tmp608,16));}),_tag_dynforward(_tmp607,sizeof(void*),0));});
_LL346: if(_tmp5A4 <= (void*)2)goto _LL31F;if(*((int*)_tmp5A4)!= 14)goto _LL31F;
_LL347:({void*_tmp609[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp60A="exp pat";
_tag_dynforward(_tmp60A,sizeof(char),_get_zero_arr_size(_tmp60A,8));}),
_tag_dynforward(_tmp609,sizeof(void*),0));});_LL31F:;}return({struct _tuple11
_tmp60B;_tmp60B.f1=nv;_tmp60B.f2=decls;_tmp60B.f3=s;_tmp60B;});}struct _tuple14{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple14*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple14*_tmp60C=_region_malloc(r,sizeof(*_tmp60C));_tmp60C->f1=
Cyc_Toc_fresh_label();_tmp60C->f2=Cyc_Toc_fresh_label();_tmp60C->f3=sc;_tmp60C;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp60D=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp60E;union Cyc_Absyn_TunionInfoU_union _tmp60F;struct
Cyc_Absyn_Tuniondecl**_tmp610;struct Cyc_Absyn_Tuniondecl*_tmp611;_LL34E: if(
_tmp60D <= (void*)4)goto _LL350;if(*((int*)_tmp60D)!= 2)goto _LL350;_tmp60E=((
struct Cyc_Absyn_TunionType_struct*)_tmp60D)->f1;_tmp60F=_tmp60E.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp60D)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL350;_tmp610=(_tmp60F.KnownTunion).f1;_tmp611=*_tmp610;if(!(!_tmp611->is_flat))
goto _LL350;_LL34F: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*
_tmp612=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp611->fields))->v;
for(0;(unsigned int)_tmp612;_tmp612=_tmp612->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp612->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL34D;}_LL350:;_LL351: goto _LL34D;_LL34D:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp613=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp614;struct Cyc_Absyn_Pat*_tmp615;struct Cyc_Absyn_Pat
_tmp616;void*_tmp617;struct Cyc_Absyn_Tuniondecl*_tmp618;struct Cyc_Absyn_Tunionfield*
_tmp619;struct Cyc_List_List*_tmp61A;_LL353: if(_tmp613 <= (void*)2)goto _LL357;if(*((
int*)_tmp613)!= 0)goto _LL355;_tmp614=((struct Cyc_Absyn_Var_p_struct*)_tmp613)->f2;
_LL354: return Cyc_Toc_no_tag_test(_tmp614);_LL355: if(*((int*)_tmp613)!= 4)goto
_LL357;_tmp615=((struct Cyc_Absyn_Pointer_p_struct*)_tmp613)->f1;_tmp616=*_tmp615;
_tmp617=(void*)_tmp616.r;if(_tmp617 <= (void*)2)goto _LL357;if(*((int*)_tmp617)!= 
6)goto _LL357;_tmp618=((struct Cyc_Absyn_Tunion_p_struct*)_tmp617)->f1;_tmp619=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp617)->f2;_tmp61A=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp617)->f3;if(!(_tmp61A != 0  && !_tmp618->is_flat))goto _LL357;_LL356: return 0;
_LL357:;_LL358: return 1;_LL352:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
Cyc_Toc_exp_to_c(nv,e);{void*_tmp61B=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
int leave_as_switch;{void*_tmp61C=Cyc_Tcutil_compress(_tmp61B);_LL35A: if(_tmp61C
<= (void*)4)goto _LL35E;if(*((int*)_tmp61C)!= 5)goto _LL35C;_LL35B: goto _LL35D;
_LL35C: if(*((int*)_tmp61C)!= 12)goto _LL35E;_LL35D: leave_as_switch=1;goto _LL359;
_LL35E:;_LL35F: leave_as_switch=0;goto _LL359;_LL359:;}{struct Cyc_List_List*
_tmp61D=scs;for(0;_tmp61D != 0;_tmp61D=_tmp61D->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp61D->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp61D->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dynforward_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp61E=scs;for(0;_tmp61E != 0;_tmp61E=_tmp61E->tl){struct Cyc_Absyn_Stmt*
_tmp61F=((struct Cyc_Absyn_Switch_clause*)_tmp61E->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp61E->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp61F,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp620=_new_region("rgn");struct _RegionHandle*rgn=& _tmp620;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp61F);;_pop_region(rgn);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle
_tmp621=_new_region("rgn");struct _RegionHandle*rgn=& _tmp621;_push_region(rgn);{
struct Cyc_Toc_Env*_tmp622=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp61B);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp623=lscs;for(0;_tmp623 != 0;_tmp623=_tmp623->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple14*)_tmp623->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp623->tl
== 0?end_l:(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp623->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp624=_tmp623->tl;for(0;(unsigned int)
_tmp624;_tmp624=_tmp624->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple14*)_tmp624->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp625=_region_malloc(rgn,sizeof(*_tmp625));
_tmp625[0]=Cyc_Absyn_goto_stmt((*((struct _tuple14*)_tmp624->hd)).f1,0);_tmp625;});
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp626=_region_malloc(rgn,sizeof(*_tmp626));_tmp626[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp626;});}{struct Cyc_Toc_Env*_tmp628;struct Cyc_List_List*_tmp629;
struct Cyc_Absyn_Stmt*_tmp62A;struct _tuple11 _tmp627=Cyc_Toc_xlate_pat(_tmp622,rgn,
_tmp61B,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp628=_tmp627.f1;_tmp629=_tmp627.f2;_tmp62A=_tmp627.f3;if(is_tunion  && Cyc_Toc_no_tag_test(
sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*
_tmp62B=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp628,_tmp62B);_tmp62A=Cyc_Absyn_seq_stmt(_tmp62A,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp62B,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp629;nvs=({struct Cyc_List_List*_tmp62C=_region_malloc(rgn,
sizeof(*_tmp62C));_tmp62C->hd=_tmp628;_tmp62C->tl=nvs;_tmp62C;});test_stmts=({
struct Cyc_List_List*_tmp62D=_region_malloc(rgn,sizeof(*_tmp62D));_tmp62D->hd=
_tmp62A;_tmp62D->tl=test_stmts;_tmp62D;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple14 _tmp62F;struct _dynforward_ptr*_tmp630;struct _dynforward_ptr*
_tmp631;struct Cyc_Absyn_Switch_clause*_tmp632;struct _tuple14*_tmp62E=(struct
_tuple14*)lscs->hd;_tmp62F=*_tmp62E;_tmp630=_tmp62F.f1;_tmp631=_tmp62F.f2;
_tmp632=_tmp62F.f3;{struct Cyc_Toc_Env*_tmp633=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp632->body;{struct _RegionHandle
_tmp634=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp634;_push_region(rgn2);
if(lscs->tl != 0){struct _tuple14 _tmp636;struct _dynforward_ptr*_tmp637;struct Cyc_Absyn_Switch_clause*
_tmp638;struct _tuple14*_tmp635=(struct _tuple14*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp636=*_tmp635;_tmp637=_tmp636.f2;_tmp638=_tmp636.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp633,end_l,_tmp637,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp638->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp633,end_l),s);};
_pop_region(rgn2);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp630,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp631,s,0),0);stmts=({struct Cyc_List_List*_tmp639=_region_malloc(rgn,sizeof(*
_tmp639));_tmp639->hd=s;_tmp639->tl=stmts;_tmp639;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple12 _tmp63B;struct _tuple1*
_tmp63C;void*_tmp63D;struct _tuple12*_tmp63A=(struct _tuple12*)decls->hd;_tmp63B=*
_tmp63A;_tmp63C=_tmp63B.f1;_tmp63D=_tmp63B.f2;res=Cyc_Absyn_declare_stmt(_tmp63C,
_tmp63D,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp63E=
_cycalloc(sizeof(*_tmp63E));_tmp63E->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp63E->tl=0;_tmp63E;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp63F=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp640;struct Cyc_Absyn_Stmt*_tmp641;struct Cyc_Absyn_Stmt*_tmp642;struct Cyc_Absyn_Exp*
_tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Stmt*_tmp645;struct Cyc_Absyn_Stmt*
_tmp646;struct _tuple3 _tmp647;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_Absyn_Stmt*
_tmp649;struct Cyc_Absyn_Stmt*_tmp64A;struct Cyc_Absyn_Stmt*_tmp64B;struct Cyc_Absyn_Stmt*
_tmp64C;struct Cyc_Absyn_Exp*_tmp64D;struct _tuple3 _tmp64E;struct Cyc_Absyn_Exp*
_tmp64F;struct _tuple3 _tmp650;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Stmt*
_tmp652;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_List_List*_tmp654;struct Cyc_List_List*
_tmp655;struct Cyc_Absyn_Switch_clause**_tmp656;struct Cyc_Absyn_Decl*_tmp657;
struct Cyc_Absyn_Stmt*_tmp658;struct _dynforward_ptr*_tmp659;struct Cyc_Absyn_Stmt*
_tmp65A;struct Cyc_Absyn_Stmt*_tmp65B;struct _tuple3 _tmp65C;struct Cyc_Absyn_Exp*
_tmp65D;struct Cyc_Absyn_Stmt*_tmp65E;struct Cyc_List_List*_tmp65F;struct Cyc_Absyn_Tvar*
_tmp660;struct Cyc_Absyn_Vardecl*_tmp661;int _tmp662;struct Cyc_Absyn_Exp*_tmp663;
struct Cyc_Absyn_Stmt*_tmp664;struct Cyc_Absyn_Exp*_tmp665;_LL361: if((int)_tmp63F
!= 0)goto _LL363;_LL362: return;_LL363: if(_tmp63F <= (void*)1)goto _LL365;if(*((int*)
_tmp63F)!= 0)goto _LL365;_tmp640=((struct Cyc_Absyn_Exp_s_struct*)_tmp63F)->f1;
_LL364: Cyc_Toc_exp_to_c(nv,_tmp640);return;_LL365: if(_tmp63F <= (void*)1)goto
_LL367;if(*((int*)_tmp63F)!= 1)goto _LL367;_tmp641=((struct Cyc_Absyn_Seq_s_struct*)
_tmp63F)->f1;_tmp642=((struct Cyc_Absyn_Seq_s_struct*)_tmp63F)->f2;_LL366: Cyc_Toc_stmt_to_c(
nv,_tmp641);s=_tmp642;continue;_LL367: if(_tmp63F <= (void*)1)goto _LL369;if(*((int*)
_tmp63F)!= 2)goto _LL369;_tmp643=((struct Cyc_Absyn_Return_s_struct*)_tmp63F)->f1;
_LL368: {struct Cyc_Core_Opt*topt=0;if(_tmp643 != 0){topt=({struct Cyc_Core_Opt*
_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp643->topt))->v);_tmp666;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp643);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp667=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp668=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp667,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp667,(void*)topt->v,_tmp643,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp668,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL369: if(_tmp63F <= (void*)1)goto _LL36B;if(*((int*)_tmp63F)!= 3)goto
_LL36B;_tmp644=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp63F)->f1;_tmp645=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp63F)->f2;_tmp646=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp63F)->f3;_LL36A: Cyc_Toc_exp_to_c(nv,_tmp644);Cyc_Toc_stmt_to_c(nv,_tmp645);s=
_tmp646;continue;_LL36B: if(_tmp63F <= (void*)1)goto _LL36D;if(*((int*)_tmp63F)!= 4)
goto _LL36D;_tmp647=((struct Cyc_Absyn_While_s_struct*)_tmp63F)->f1;_tmp648=
_tmp647.f1;_tmp649=((struct Cyc_Absyn_While_s_struct*)_tmp63F)->f2;_LL36C: Cyc_Toc_exp_to_c(
nv,_tmp648);{struct _RegionHandle _tmp669=_new_region("temp");struct _RegionHandle*
temp=& _tmp669;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp649);;_pop_region(temp);}return;_LL36D: if(_tmp63F <= (void*)1)goto _LL36F;if(*((
int*)_tmp63F)!= 5)goto _LL36F;_tmp64A=((struct Cyc_Absyn_Break_s_struct*)_tmp63F)->f1;
_LL36E: {struct Cyc_Toc_Env _tmp66B;struct _dynforward_ptr**_tmp66C;struct Cyc_Toc_Env*
_tmp66A=nv;_tmp66B=*_tmp66A;_tmp66C=_tmp66B.break_lab;if(_tmp66C != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp66C,0));{int dest_depth=_tmp64A == 0?0: _tmp64A->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL36F: if(_tmp63F <= (
void*)1)goto _LL371;if(*((int*)_tmp63F)!= 6)goto _LL371;_tmp64B=((struct Cyc_Absyn_Continue_s_struct*)
_tmp63F)->f1;_LL370: {struct Cyc_Toc_Env _tmp66E;struct _dynforward_ptr**_tmp66F;
struct Cyc_Toc_Env*_tmp66D=nv;_tmp66E=*_tmp66D;_tmp66F=_tmp66E.continue_lab;if(
_tmp66F != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp66F,0));_tmp64C=_tmp64B;
goto _LL372;}_LL371: if(_tmp63F <= (void*)1)goto _LL373;if(*((int*)_tmp63F)!= 7)goto
_LL373;_tmp64C=((struct Cyc_Absyn_Goto_s_struct*)_tmp63F)->f2;_LL372: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp64C))->try_depth,s);
return;_LL373: if(_tmp63F <= (void*)1)goto _LL375;if(*((int*)_tmp63F)!= 8)goto
_LL375;_tmp64D=((struct Cyc_Absyn_For_s_struct*)_tmp63F)->f1;_tmp64E=((struct Cyc_Absyn_For_s_struct*)
_tmp63F)->f2;_tmp64F=_tmp64E.f1;_tmp650=((struct Cyc_Absyn_For_s_struct*)_tmp63F)->f3;
_tmp651=_tmp650.f1;_tmp652=((struct Cyc_Absyn_For_s_struct*)_tmp63F)->f4;_LL374:
Cyc_Toc_exp_to_c(nv,_tmp64D);Cyc_Toc_exp_to_c(nv,_tmp64F);Cyc_Toc_exp_to_c(nv,
_tmp651);{struct _RegionHandle _tmp670=_new_region("temp");struct _RegionHandle*
temp=& _tmp670;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp652);;_pop_region(temp);}return;_LL375: if(_tmp63F <= (void*)1)goto _LL377;if(*((
int*)_tmp63F)!= 9)goto _LL377;_tmp653=((struct Cyc_Absyn_Switch_s_struct*)_tmp63F)->f1;
_tmp654=((struct Cyc_Absyn_Switch_s_struct*)_tmp63F)->f2;_LL376: Cyc_Toc_xlate_switch(
nv,s,_tmp653,_tmp654);return;_LL377: if(_tmp63F <= (void*)1)goto _LL379;if(*((int*)
_tmp63F)!= 10)goto _LL379;_tmp655=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp63F)->f1;
_tmp656=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp63F)->f2;_LL378: {struct Cyc_Toc_Env
_tmp672;struct Cyc_Toc_FallthruInfo*_tmp673;struct Cyc_Toc_Env*_tmp671=nv;_tmp672=*
_tmp671;_tmp673=_tmp672.fallthru_info;if(_tmp673 == 0)({void*_tmp674[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp675="fallthru in unexpected place";_tag_dynforward(_tmp675,sizeof(char),
_get_zero_arr_size(_tmp675,29));}),_tag_dynforward(_tmp674,sizeof(void*),0));});{
struct _dynforward_ptr*_tmp677;struct Cyc_List_List*_tmp678;struct Cyc_Dict_Dict
_tmp679;struct Cyc_Toc_FallthruInfo _tmp676=*_tmp673;_tmp677=_tmp676.label;_tmp678=
_tmp676.binders;_tmp679=_tmp676.next_case_env;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp677,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp656)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp67A=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp678);struct Cyc_List_List*
_tmp67B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp655);
for(0;_tmp67A != 0;(_tmp67A=_tmp67A->tl,_tmp67B=_tmp67B->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp67B))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k))Cyc_Dict_lookup)(_tmp679,(struct _tuple1*)_tmp67A->hd),(struct Cyc_Absyn_Exp*)
_tmp67B->hd,0),s2,0);}(void*)(s->r=(void*)((void*)s2->r));return;}}}}_LL379: if(
_tmp63F <= (void*)1)goto _LL37B;if(*((int*)_tmp63F)!= 11)goto _LL37B;_tmp657=((
struct Cyc_Absyn_Decl_s_struct*)_tmp63F)->f1;_tmp658=((struct Cyc_Absyn_Decl_s_struct*)
_tmp63F)->f2;_LL37A:{void*_tmp67C=(void*)_tmp657->r;struct Cyc_Absyn_Vardecl*
_tmp67D;struct Cyc_Absyn_Pat*_tmp67E;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_List_List*
_tmp680;struct Cyc_Absyn_Fndecl*_tmp681;_LL386: if(_tmp67C <= (void*)2)goto _LL38E;
if(*((int*)_tmp67C)!= 0)goto _LL388;_tmp67D=((struct Cyc_Absyn_Var_d_struct*)
_tmp67C)->f1;_LL387:{struct _RegionHandle _tmp682=_new_region("temp");struct
_RegionHandle*temp=& _tmp682;_push_region(temp);{struct Cyc_Toc_Env*_tmp683=Cyc_Toc_add_varmap(
temp,nv,_tmp67D->name,Cyc_Absyn_varb_exp(_tmp67D->name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684[0]=({struct Cyc_Absyn_Local_b_struct
_tmp685;_tmp685.tag=3;_tmp685.f1=_tmp67D;_tmp685;});_tmp684;}),0));Cyc_Toc_local_decl_to_c(
_tmp683,_tmp683,_tmp67D,_tmp658);};_pop_region(temp);}goto _LL385;_LL388: if(*((
int*)_tmp67C)!= 2)goto _LL38A;_tmp67E=((struct Cyc_Absyn_Let_d_struct*)_tmp67C)->f1;
_tmp67F=((struct Cyc_Absyn_Let_d_struct*)_tmp67C)->f3;_LL389:{void*_tmp686=(void*)
_tmp67E->r;struct Cyc_Absyn_Vardecl*_tmp687;struct Cyc_Absyn_Pat*_tmp688;struct Cyc_Absyn_Pat
_tmp689;void*_tmp68A;_LL391: if(_tmp686 <= (void*)2)goto _LL393;if(*((int*)_tmp686)
!= 0)goto _LL393;_tmp687=((struct Cyc_Absyn_Var_p_struct*)_tmp686)->f1;_tmp688=((
struct Cyc_Absyn_Var_p_struct*)_tmp686)->f2;_tmp689=*_tmp688;_tmp68A=(void*)
_tmp689.r;if((int)_tmp68A != 0)goto _LL393;_LL392: {struct _tuple1*old_name=_tmp687->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp687->name=new_name;_tmp687->initializer=(
struct Cyc_Absyn_Exp*)_tmp67F;(void*)(_tmp657->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_Var_d_struct
_tmp68C;_tmp68C.tag=0;_tmp68C.f1=_tmp687;_tmp68C;});_tmp68B;})));{struct
_RegionHandle _tmp68D=_new_region("temp");struct _RegionHandle*temp=& _tmp68D;
_push_region(temp);{struct Cyc_Toc_Env*_tmp68E=Cyc_Toc_add_varmap(temp,nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_Absyn_Local_b_struct
_tmp690;_tmp690.tag=3;_tmp690.f1=_tmp687;_tmp690;});_tmp68F;}),0));Cyc_Toc_local_decl_to_c(
_tmp68E,nv,_tmp687,_tmp658);};_pop_region(temp);}goto _LL390;}_LL393:;_LL394:(
void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(nv,_tmp67E,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp67F->topt))->v,_tmp67F,_tmp658))->r));goto _LL390;_LL390:;}goto
_LL385;_LL38A: if(*((int*)_tmp67C)!= 3)goto _LL38C;_tmp680=((struct Cyc_Absyn_Letv_d_struct*)
_tmp67C)->f1;_LL38B: {struct Cyc_List_List*_tmp691=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp680);if(_tmp691 == 0)({void*_tmp692[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp693="empty Letv_d";_tag_dynforward(_tmp693,sizeof(char),
_get_zero_arr_size(_tmp693,13));}),_tag_dynforward(_tmp692,sizeof(void*),0));});(
void*)(_tmp657->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp694=
_cycalloc(sizeof(*_tmp694));_tmp694[0]=({struct Cyc_Absyn_Var_d_struct _tmp695;
_tmp695.tag=0;_tmp695.f1=(struct Cyc_Absyn_Vardecl*)_tmp691->hd;_tmp695;});
_tmp694;})));_tmp691=_tmp691->tl;for(0;_tmp691 != 0;_tmp691=_tmp691->tl){struct
Cyc_Absyn_Decl*_tmp696=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697[0]=({struct Cyc_Absyn_Var_d_struct
_tmp698;_tmp698.tag=0;_tmp698.f1=(struct Cyc_Absyn_Vardecl*)_tmp691->hd;_tmp698;});
_tmp697;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp696,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL385;}_LL38C: if(*((int*)
_tmp67C)!= 1)goto _LL38E;_tmp681=((struct Cyc_Absyn_Fn_d_struct*)_tmp67C)->f1;
_LL38D: {struct _tuple1*_tmp699=_tmp681->name;{struct _RegionHandle _tmp69A=
_new_region("temp");struct _RegionHandle*temp=& _tmp69A;_push_region(temp);{struct
Cyc_Toc_Env*_tmp69B=Cyc_Toc_add_varmap(temp,nv,_tmp681->name,Cyc_Absyn_var_exp(
_tmp699,0));Cyc_Toc_fndecl_to_c(_tmp69B,_tmp681,0);Cyc_Toc_stmt_to_c(_tmp69B,
_tmp658);};_pop_region(temp);}goto _LL385;}_LL38E:;_LL38F:({void*_tmp69C[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp69D="bad nested declaration within function";_tag_dynforward(
_tmp69D,sizeof(char),_get_zero_arr_size(_tmp69D,39));}),_tag_dynforward(_tmp69C,
sizeof(void*),0));});_LL385:;}return;_LL37B: if(_tmp63F <= (void*)1)goto _LL37D;if(*((
int*)_tmp63F)!= 12)goto _LL37D;_tmp659=((struct Cyc_Absyn_Label_s_struct*)_tmp63F)->f1;
_tmp65A=((struct Cyc_Absyn_Label_s_struct*)_tmp63F)->f2;_LL37C: s=_tmp65A;
continue;_LL37D: if(_tmp63F <= (void*)1)goto _LL37F;if(*((int*)_tmp63F)!= 13)goto
_LL37F;_tmp65B=((struct Cyc_Absyn_Do_s_struct*)_tmp63F)->f1;_tmp65C=((struct Cyc_Absyn_Do_s_struct*)
_tmp63F)->f2;_tmp65D=_tmp65C.f1;_LL37E:{struct _RegionHandle _tmp69E=_new_region("temp");
struct _RegionHandle*temp=& _tmp69E;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
temp,nv),_tmp65B);Cyc_Toc_exp_to_c(nv,_tmp65D);;_pop_region(temp);}return;_LL37F:
if(_tmp63F <= (void*)1)goto _LL381;if(*((int*)_tmp63F)!= 14)goto _LL381;_tmp65E=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp63F)->f1;_tmp65F=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp63F)->f2;_LL380: {struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=
Cyc_Toc_temp_var();struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
e_exp->topt=({struct Cyc_Core_Opt*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F->v=(
void*)e_typ;_tmp69F;});{struct _RegionHandle _tmp6A0=_new_region("temp");struct
_RegionHandle*temp=& _tmp6A0;_push_region(temp);{struct Cyc_Toc_Env*_tmp6A1=Cyc_Toc_add_varmap(
temp,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp6A1,_tmp65E);{struct Cyc_Absyn_Stmt*
_tmp6A2=Cyc_Absyn_seq_stmt(_tmp65E,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp6A3=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp6A4=
Cyc_Absyn_var_exp(_tmp6A3,0);struct Cyc_Absyn_Vardecl*_tmp6A5=Cyc_Absyn_new_vardecl(
_tmp6A3,Cyc_Absyn_exn_typ,0);_tmp6A4->topt=({struct Cyc_Core_Opt*_tmp6A6=
_cycalloc(sizeof(*_tmp6A6));_tmp6A6->v=(void*)Cyc_Absyn_exn_typ;_tmp6A6;});{
struct Cyc_Absyn_Pat*_tmp6A7=({struct Cyc_Absyn_Pat*_tmp6B7=_cycalloc(sizeof(*
_tmp6B7));_tmp6B7->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp6B9=
_cycalloc(sizeof(*_tmp6B9));_tmp6B9[0]=({struct Cyc_Absyn_Var_p_struct _tmp6BA;
_tmp6BA.tag=0;_tmp6BA.f1=_tmp6A5;_tmp6BA.f2=({struct Cyc_Absyn_Pat*_tmp6BB=
_cycalloc(sizeof(*_tmp6BB));_tmp6BB->r=(void*)((void*)0);_tmp6BB->topt=({struct
Cyc_Core_Opt*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->v=(void*)Cyc_Absyn_exn_typ;
_tmp6BC;});_tmp6BB->loc=0;_tmp6BB;});_tmp6BA;});_tmp6B9;}));_tmp6B7->topt=({
struct Cyc_Core_Opt*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->v=(void*)Cyc_Absyn_exn_typ;
_tmp6B8;});_tmp6B7->loc=0;_tmp6B7;});struct Cyc_Absyn_Exp*_tmp6A8=Cyc_Absyn_throw_exp(
_tmp6A4,0);_tmp6A8->topt=({struct Cyc_Core_Opt*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));
_tmp6A9->v=(void*)((void*)0);_tmp6A9;});{struct Cyc_Absyn_Stmt*_tmp6AA=Cyc_Absyn_exp_stmt(
_tmp6A8,0);struct Cyc_Absyn_Switch_clause*_tmp6AB=({struct Cyc_Absyn_Switch_clause*
_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->pattern=_tmp6A7;_tmp6B4->pat_vars=({
struct Cyc_Core_Opt*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->v=({struct Cyc_List_List*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->hd=_tmp6A5;_tmp6B6->tl=0;_tmp6B6;});
_tmp6B5;});_tmp6B4->where_clause=0;_tmp6B4->body=_tmp6AA;_tmp6B4->loc=0;_tmp6B4;});
struct Cyc_Absyn_Stmt*_tmp6AC=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp65F,({struct
Cyc_List_List*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3->hd=_tmp6AB;_tmp6B3->tl=
0;_tmp6B3;})),0);Cyc_Toc_stmt_to_c(_tmp6A1,_tmp6AC);{struct Cyc_Absyn_Exp*_tmp6AD=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp6B2=_cycalloc(
sizeof(*_tmp6B2));_tmp6B2->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp6B2->tl=0;_tmp6B2;}),0);struct Cyc_Absyn_Stmt*_tmp6AE=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp6B1=
_cycalloc(sizeof(*_tmp6B1));_tmp6B1->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp6B1->tl=
0;_tmp6B1;}),0),0);struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp6B0=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp6AE,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp6AF,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp6AD,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp6B0,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp6A2,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp6AC,0),0),0),0),0),0))->r));}}}}};
_pop_region(temp);}return;}_LL381: if(_tmp63F <= (void*)1)goto _LL383;if(*((int*)
_tmp63F)!= 15)goto _LL383;_tmp660=((struct Cyc_Absyn_Region_s_struct*)_tmp63F)->f1;
_tmp661=((struct Cyc_Absyn_Region_s_struct*)_tmp63F)->f2;_tmp662=((struct Cyc_Absyn_Region_s_struct*)
_tmp63F)->f3;_tmp663=((struct Cyc_Absyn_Region_s_struct*)_tmp63F)->f4;_tmp664=((
struct Cyc_Absyn_Region_s_struct*)_tmp63F)->f5;_LL382: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp661->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);{struct _RegionHandle
_tmp6BD=_new_region("temp");struct _RegionHandle*temp=& _tmp6BD;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp664);;_pop_region(
temp);}if(_tmp663 == 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,
rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp6BE->tl=0;_tmp6BE;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp6BF[1];_tmp6BF[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6BF,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp664,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp6C0[1];_tmp6C0[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6C0,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));else{Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp663);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6C1[2];_tmp6C1[1]=(struct Cyc_Absyn_Exp*)_tmp663;_tmp6C1[
0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6C1,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp664,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6C2[1];_tmp6C2[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6C2,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r));}return;}_LL383: if(_tmp63F <= (void*)1)goto _LL360;if(*((
int*)_tmp63F)!= 16)goto _LL360;_tmp665=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp63F)->f1;_LL384: Cyc_Toc_exp_to_c(nv,_tmp665);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp6C3=
_cycalloc(sizeof(*_tmp6C3));_tmp6C3->hd=_tmp665;_tmp6C3->tl=0;_tmp6C3;}),0)));
return;_LL360:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);struct
_tuple15{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void
Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){f->tvs=
0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{
struct _RegionHandle _tmp6C4=_new_region("frgn");struct _RegionHandle*frgn=& _tmp6C4;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp6C5=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp6C6=f->args;for(0;_tmp6C6 != 0;_tmp6C6=_tmp6C6->tl){struct
_tuple1*_tmp6C7=({struct _tuple1*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->f1=(
union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp6C9;(_tmp6C9.Loc_n).tag=0;_tmp6C9;}));_tmp6C8->f2=(*((struct _tuple15*)
_tmp6C6->hd)).f1;_tmp6C8;});(*((struct _tuple15*)_tmp6C6->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple15*)_tmp6C6->hd)).f3);_tmp6C5=Cyc_Toc_add_varmap(frgn,_tmp6C5,
_tmp6C7,Cyc_Absyn_var_exp(_tmp6C7,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp6CB;struct Cyc_Absyn_Tqual
_tmp6CC;void*_tmp6CD;int _tmp6CE;struct Cyc_Absyn_VarargInfo _tmp6CA=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp6CB=_tmp6CA.name;_tmp6CC=_tmp6CA.tq;_tmp6CD=(
void*)_tmp6CA.type;_tmp6CE=_tmp6CA.inject;{void*_tmp6CF=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp6CD,(void*)2,_tmp6CC,Cyc_Absyn_false_conref));struct _tuple1*_tmp6D0=({struct
_tuple1*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp6D4;(_tmp6D4.Loc_n).tag=
0;_tmp6D4;}));_tmp6D3->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp6CB))->v;_tmp6D3;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp6D1=
_cycalloc(sizeof(*_tmp6D1));_tmp6D1->hd=({struct _tuple15*_tmp6D2=_cycalloc(
sizeof(*_tmp6D2));_tmp6D2->f1=(struct _dynforward_ptr*)_tmp6CB->v;_tmp6D2->f2=
_tmp6CC;_tmp6D2->f3=_tmp6CF;_tmp6D2;});_tmp6D1->tl=0;_tmp6D1;}));_tmp6C5=Cyc_Toc_add_varmap(
frgn,_tmp6C5,_tmp6D0,Cyc_Absyn_var_exp(_tmp6D0,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp6D5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp6D5 != 0;_tmp6D5=_tmp6D5->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp6D5->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp6D5->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp6C5),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp6D6=s;_LL396:
if((int)_tmp6D6 != 1)goto _LL398;_LL397: return(void*)2;_LL398: if((int)_tmp6D6 != 4)
goto _LL39A;_LL399: return(void*)3;_LL39A:;_LL39B: return s;_LL395:;}struct Cyc_Dict_Dict*
Cyc_Toc_aggrs_so_far=0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp6D7=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({
struct Cyc_Dict_Dict*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp6D8;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp6D9=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far)),_tmp6D7);if(_tmp6D9 == 0){seen_defn_before=0;*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_aggrs_so_far))=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_aggrs_so_far)),_tmp6D7,ad);}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp6D9->v)->impl == 0){*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far)),_tmp6D7,ad);seen_defn_before=0;}else{
seen_defn_before=1;}}(void*)(ad->sc=(void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp6DA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp6DA != 0;_tmp6DA=_tmp6DA->tl){((struct Cyc_Absyn_Aggrfield*)_tmp6DA->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp6DA->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp6DA->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp6DB;});{
struct _tuple1*_tmp6DC=tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp6DC))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp6DC));{struct Cyc_List_List*
flat_structs=0;{struct Cyc_List_List*_tmp6DD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp6DD != 0;_tmp6DD=_tmp6DD->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp6DD->hd;if(f->typs != 0  || tud->is_flat){
struct Cyc_List_List*_tmp6DE=0;int i=1;{struct Cyc_List_List*_tmp6DF=f->typs;for(0;
_tmp6DF != 0;(_tmp6DF=_tmp6DF->tl,i ++)){struct _dynforward_ptr*_tmp6E0=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp6E1=({struct Cyc_Absyn_Aggrfield*_tmp6E3=
_cycalloc(sizeof(*_tmp6E3));_tmp6E3->name=_tmp6E0;_tmp6E3->tq=(*((struct _tuple4*)
_tmp6DF->hd)).f1;_tmp6E3->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp6DF->hd)).f2);_tmp6E3->width=0;_tmp6E3->attributes=0;_tmp6E3;});_tmp6DE=({
struct Cyc_List_List*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->hd=_tmp6E1;
_tmp6E2->tl=_tmp6DE;_tmp6E2;});}}_tmp6DE=({struct Cyc_List_List*_tmp6E4=_cycalloc(
sizeof(*_tmp6E4));_tmp6E4->hd=({struct Cyc_Absyn_Aggrfield*_tmp6E5=_cycalloc(
sizeof(*_tmp6E5));_tmp6E5->name=Cyc_Toc_tag_sp;_tmp6E5->tq=Cyc_Toc_mt_tq;_tmp6E5->type=(
void*)Cyc_Absyn_sint_typ;_tmp6E5->width=0;_tmp6E5->attributes=0;_tmp6E5;});
_tmp6E4->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp6DE);_tmp6E4;});{struct Cyc_Absyn_Aggrdecl*_tmp6E6=({struct Cyc_Absyn_Aggrdecl*
_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->kind=(void*)((void*)0);_tmp6F2->sc=(
void*)((void*)2);_tmp6F2->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp6F4="_struct";_tag_dynforward(_tmp6F4,sizeof(char),_get_zero_arr_size(
_tmp6F4,8));}));_tmp6F2->tvs=0;_tmp6F2->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->exist_vars=0;_tmp6F3->rgn_po=0;
_tmp6F3->fields=_tmp6DE;_tmp6F3;});_tmp6F2->attributes=0;_tmp6F2;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));
_tmp6E8[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp6E9;_tmp6E9.tag=4;_tmp6E9.f1=
_tmp6E6;_tmp6E9;});_tmp6E8;}),0);_tmp6E7->tl=Cyc_Toc_result_decls;_tmp6E7;});if(
tud->is_flat){struct Cyc_Absyn_Aggrfield*_tmp6EA=({struct Cyc_Absyn_Aggrfield*
_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC->name=(*f->name).f2;_tmp6EC->tq=Cyc_Toc_mt_tq;
_tmp6EC->type=(void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp6ED=_cycalloc(
sizeof(*_tmp6ED));_tmp6ED[0]=({struct Cyc_Absyn_AggrType_struct _tmp6EE;_tmp6EE.tag=
10;_tmp6EE.f1=({struct Cyc_Absyn_AggrInfo _tmp6EF;_tmp6EF.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp6F0;(_tmp6F0.KnownAggr).tag=1;(_tmp6F0.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1[0]=_tmp6E6;
_tmp6F1;});_tmp6F0;});_tmp6EF.targs=0;_tmp6EF;});_tmp6EE;});_tmp6ED;}));_tmp6EC->width=
0;_tmp6EC->attributes=0;_tmp6EC;});flat_structs=({struct Cyc_List_List*_tmp6EB=
_cycalloc(sizeof(*_tmp6EB));_tmp6EB->hd=_tmp6EA;_tmp6EB->tl=flat_structs;_tmp6EB;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp6F5=({struct Cyc_Absyn_Aggrdecl*
_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->kind=(void*)((void*)1);_tmp6F9->sc=(
void*)((void*)2);_tmp6F9->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp6FB="_union";_tag_dynforward(_tmp6FB,sizeof(char),_get_zero_arr_size(_tmp6FB,
7));}));_tmp6F9->tvs=0;_tmp6F9->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp6FA=
_cycalloc(sizeof(*_tmp6FA));_tmp6FA->exist_vars=0;_tmp6FA->rgn_po=0;_tmp6FA->fields=
flat_structs;_tmp6FA;});_tmp6F9->attributes=0;_tmp6F9;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));
_tmp6F7[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp6F8;_tmp6F8.tag=4;_tmp6F8.f1=
_tmp6F5;_tmp6F8;});_tmp6F7;}),0);_tmp6F6->tl=Cyc_Toc_result_decls;_tmp6F6;});}}}}}
static struct Cyc_Dict_Dict*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Dict_Dict*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp6FC;});if(xd->fields == 0)return;{struct _tuple1*_tmp6FD=xd->name;struct Cyc_List_List*
_tmp6FE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp6FE != 0;_tmp6FE=_tmp6FE->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp6FE->hd;struct _dynforward_ptr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp6FF=
Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(char))+ 4,0);void*_tmp700=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp6FF,Cyc_Absyn_false_conref,
0);struct Cyc_Core_Opt*_tmp701=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict d,
struct _tuple1*k))Cyc_Dict_lookup_opt)(*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),
f->name);struct Cyc_Core_Opt _tmp702;int _tmp703;_LL39D: if(_tmp701 != 0)goto _LL39F;
_LL39E: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){char zero='\000';
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp70A;_tmp70A.tag=0;_tmp70A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp709;
_tmp709.tag=1;_tmp709.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp708;_tmp708.tag=1;_tmp708.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp707;_tmp707.tag=1;_tmp707.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp706;_tmp706.tag=1;_tmp706.f1=(unsigned long)((int)zero);{void*_tmp704[5]={&
_tmp706,& _tmp707,& _tmp708,& _tmp709,& _tmp70A};Cyc_aprintf(({const char*_tmp705="%c%c%c%c%s";
_tag_dynforward(_tmp705,sizeof(char),_get_zero_arr_size(_tmp705,11));}),
_tag_dynforward(_tmp704,sizeof(void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*
_tmp70B=Cyc_Absyn_new_vardecl(f->name,_tmp700,initopt);(void*)(_tmp70B->sc=(void*)((
void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp70C=_cycalloc(
sizeof(*_tmp70C));_tmp70C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({struct Cyc_Absyn_Var_d_struct
_tmp70E;_tmp70E.tag=0;_tmp70E.f1=_tmp70B;_tmp70E;});_tmp70D;}),0);_tmp70C->tl=
Cyc_Toc_result_decls;_tmp70C;});*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far))=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),f->name,(void*)
f->sc != (void*)3);if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*
_tmp70F=f->typs;for(0;_tmp70F != 0;(_tmp70F=_tmp70F->tl,i ++)){struct
_dynforward_ptr*_tmp710=({struct _dynforward_ptr*_tmp714=_cycalloc(sizeof(*
_tmp714));_tmp714[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp717;
_tmp717.tag=1;_tmp717.f1=(unsigned long)i;{void*_tmp715[1]={& _tmp717};Cyc_aprintf(({
const char*_tmp716="f%d";_tag_dynforward(_tmp716,sizeof(char),_get_zero_arr_size(
_tmp716,4));}),_tag_dynforward(_tmp715,sizeof(void*),1));}});_tmp714;});struct
Cyc_Absyn_Aggrfield*_tmp711=({struct Cyc_Absyn_Aggrfield*_tmp713=_cycalloc(
sizeof(*_tmp713));_tmp713->name=_tmp710;_tmp713->tq=(*((struct _tuple4*)_tmp70F->hd)).f1;
_tmp713->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp70F->hd)).f2);
_tmp713->width=0;_tmp713->attributes=0;_tmp713;});fields=({struct Cyc_List_List*
_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712->hd=_tmp711;_tmp712->tl=fields;
_tmp712;});}}fields=({struct Cyc_List_List*_tmp718=_cycalloc(sizeof(*_tmp718));
_tmp718->hd=({struct Cyc_Absyn_Aggrfield*_tmp719=_cycalloc(sizeof(*_tmp719));
_tmp719->name=Cyc_Toc_tag_sp;_tmp719->tq=Cyc_Toc_mt_tq;_tmp719->type=(void*)Cyc_Absyn_cstar_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp719->width=0;_tmp719->attributes=0;_tmp719;});
_tmp718->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp718;});{struct Cyc_Absyn_Aggrdecl*_tmp71A=({struct Cyc_Absyn_Aggrdecl*
_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->kind=(void*)((void*)0);_tmp71E->sc=(
void*)((void*)2);_tmp71E->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp720="_struct";_tag_dynforward(_tmp720,sizeof(char),_get_zero_arr_size(
_tmp720,8));}));_tmp71E->tvs=0;_tmp71E->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->exist_vars=0;_tmp71F->rgn_po=0;
_tmp71F->fields=fields;_tmp71F;});_tmp71E->attributes=0;_tmp71E;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp71C=_cycalloc(sizeof(*_tmp71C));
_tmp71C[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp71D;_tmp71D.tag=4;_tmp71D.f1=
_tmp71A;_tmp71D;});_tmp71C;}),0);_tmp71B->tl=Cyc_Toc_result_decls;_tmp71B;});}}
goto _LL39C;}}_LL39F: if(_tmp701 == 0)goto _LL3A1;_tmp702=*_tmp701;_tmp703=(int)
_tmp702.v;if(_tmp703 != 0)goto _LL3A1;_LL3A0: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp721=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp72C;_tmp72C.tag=0;_tmp72C.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp72B;_tmp72B.tag=1;_tmp72B.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp72A;_tmp72A.tag=1;_tmp72A.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp729;_tmp729.tag=1;_tmp729.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp728;_tmp728.tag=1;_tmp728.f1=(
unsigned long)((int)zero);{void*_tmp726[5]={& _tmp728,& _tmp729,& _tmp72A,& _tmp72B,&
_tmp72C};Cyc_aprintf(({const char*_tmp727="%c%c%c%c%s";_tag_dynforward(_tmp727,
sizeof(char),_get_zero_arr_size(_tmp727,11));}),_tag_dynforward(_tmp726,sizeof(
void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp722=Cyc_Absyn_new_vardecl(f->name,
_tmp700,(struct Cyc_Absyn_Exp*)_tmp721);(void*)(_tmp722->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp723=_cycalloc(sizeof(*_tmp723));
_tmp723->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp724=
_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Absyn_Var_d_struct _tmp725;
_tmp725.tag=0;_tmp725.f1=_tmp722;_tmp725;});_tmp724;}),0);_tmp723->tl=Cyc_Toc_result_decls;
_tmp723;});*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far))=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),f->name,1);}
goto _LL39C;_LL3A1:;_LL3A2: goto _LL39C;_LL39C:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp72D=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp72E;struct Cyc_Absyn_Exp*_tmp72F;struct Cyc_Absyn_Exp*_tmp730;int _tmp731;
_LL3A4: if(*((int*)_tmp72D)!= 29)goto _LL3A6;_tmp72E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp72D)->f1;_tmp72F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp72D)->f2;
_tmp730=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp72D)->f3;_tmp731=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp72D)->f4;_LL3A5: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp72E,_tmp72F,_tmp730,_tmp731,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL3A3;_LL3A6:;_LL3A7: if((void*)vd->sc == (void*)0){struct _RegionHandle
_tmp732=_new_region("temp");struct _RegionHandle*temp=& _tmp732;_push_region(temp);{
struct Cyc_Toc_Env*_tmp733=Cyc_Toc_set_toplevel(temp,init_nv);Cyc_Toc_exp_to_c(
_tmp733,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(init_nv,init);}goto
_LL3A3;_LL3A3:;}else{void*_tmp734=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo
_tmp735;void*_tmp736;struct Cyc_Absyn_Exp*_tmp737;struct Cyc_Absyn_Conref*_tmp738;
_LL3A9: if(_tmp734 <= (void*)4)goto _LL3AB;if(*((int*)_tmp734)!= 7)goto _LL3AB;
_tmp735=((struct Cyc_Absyn_ArrayType_struct*)_tmp734)->f1;_tmp736=(void*)_tmp735.elt_type;
_tmp737=_tmp735.num_elts;_tmp738=_tmp735.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp738))goto _LL3AB;_LL3AA: if(_tmp737 == 0)({void*
_tmp739[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp73A="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp73A,sizeof(char),_get_zero_arr_size(_tmp73A,55));}),
_tag_dynforward(_tmp739,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp737;struct Cyc_Absyn_Exp*_tmp73B=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp73C=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp73B,_tmp73C,
0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL3A8;}_LL3AB:;_LL3AC: goto _LL3A8;
_LL3A8:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static
struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp73D=_cycalloc(
sizeof(*_tmp73D));_tmp73D[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp73D;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp73E=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp73E;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp73F=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp740=Cyc_Toc_share_env(
prgn,nv);struct Cyc_Toc_Env*_tmp742;struct Cyc_List_List*_tmp743;struct Cyc_Absyn_Stmt*
_tmp744;struct _tuple11 _tmp741=Cyc_Toc_xlate_pat(_tmp740,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)& _tmp73F,Cyc_Toc_throw_match_stmt(),
0);_tmp742=_tmp741.f1;_tmp743=_tmp741.f2;_tmp744=_tmp741.f3;Cyc_Toc_stmt_to_c(
_tmp742,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp744,s,0),0);for(0;_tmp743 != 0;_tmp743=_tmp743->tl){
struct _tuple12 _tmp746;struct _tuple1*_tmp747;void*_tmp748;struct _tuple12*_tmp745=(
struct _tuple12*)_tmp743->hd;_tmp746=*_tmp745;_tmp747=_tmp746.f1;_tmp748=_tmp746.f2;
s=Cyc_Absyn_declare_stmt(_tmp747,_tmp748,0,s,0);}};_pop_region(prgn);}return s;}}
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp749=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp74A;struct Cyc_Absyn_Exp*_tmp74B;struct Cyc_Absyn_Exp*
_tmp74C;struct Cyc_Absyn_Exp*_tmp74D;struct Cyc_Absyn_Exp*_tmp74E;struct Cyc_Absyn_Exp*
_tmp74F;struct Cyc_Absyn_Exp*_tmp750;struct Cyc_Absyn_Exp*_tmp751;struct Cyc_List_List*
_tmp752;struct Cyc_Absyn_Exp*_tmp753;struct Cyc_Absyn_Exp*_tmp754;struct Cyc_Absyn_Exp*
_tmp755;struct Cyc_Absyn_Exp*_tmp756;struct Cyc_Absyn_Exp*_tmp757;struct Cyc_Absyn_Exp*
_tmp758;struct Cyc_Absyn_Exp*_tmp759;struct Cyc_Absyn_Exp*_tmp75A;struct Cyc_Absyn_Exp*
_tmp75B;struct Cyc_Absyn_Exp*_tmp75C;struct Cyc_Absyn_Exp*_tmp75D;struct Cyc_Absyn_Exp*
_tmp75E;struct Cyc_Absyn_Exp*_tmp75F;struct Cyc_Absyn_Exp*_tmp760;struct Cyc_List_List*
_tmp761;struct Cyc_Absyn_Exp*_tmp762;struct Cyc_List_List*_tmp763;void*_tmp764;
void**_tmp765;struct Cyc_Absyn_Exp*_tmp766;struct _tuple2*_tmp767;struct _tuple2
_tmp768;void*_tmp769;void**_tmp76A;struct Cyc_List_List*_tmp76B;struct Cyc_List_List*
_tmp76C;struct Cyc_List_List*_tmp76D;void*_tmp76E;void**_tmp76F;void*_tmp770;void**
_tmp771;struct Cyc_Absyn_Stmt*_tmp772;struct Cyc_Absyn_MallocInfo _tmp773;struct Cyc_Absyn_MallocInfo*
_tmp774;_LL3AE: if(*((int*)_tmp749)!= 22)goto _LL3B0;_tmp74A=((struct Cyc_Absyn_Deref_e_struct*)
_tmp749)->f1;_LL3AF: _tmp74B=_tmp74A;goto _LL3B1;_LL3B0: if(*((int*)_tmp749)!= 23)
goto _LL3B2;_tmp74B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp749)->f1;_LL3B1:
_tmp74C=_tmp74B;goto _LL3B3;_LL3B2: if(*((int*)_tmp749)!= 24)goto _LL3B4;_tmp74C=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp749)->f1;_LL3B3: _tmp74D=_tmp74C;goto
_LL3B5;_LL3B4: if(*((int*)_tmp749)!= 16)goto _LL3B6;_tmp74D=((struct Cyc_Absyn_Address_e_struct*)
_tmp749)->f1;_LL3B5: _tmp74E=_tmp74D;goto _LL3B7;_LL3B6: if(*((int*)_tmp749)!= 12)
goto _LL3B8;_tmp74E=((struct Cyc_Absyn_Throw_e_struct*)_tmp749)->f1;_LL3B7: _tmp74F=
_tmp74E;goto _LL3B9;_LL3B8: if(*((int*)_tmp749)!= 13)goto _LL3BA;_tmp74F=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp749)->f1;_LL3B9: _tmp750=_tmp74F;goto _LL3BB;
_LL3BA: if(*((int*)_tmp749)!= 19)goto _LL3BC;_tmp750=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp749)->f1;_LL3BB: _tmp751=_tmp750;goto _LL3BD;_LL3BC: if(*((int*)_tmp749)!= 5)
goto _LL3BE;_tmp751=((struct Cyc_Absyn_Increment_e_struct*)_tmp749)->f1;_LL3BD: Cyc_Toc_exptypes_to_c(
_tmp751);goto _LL3AD;_LL3BE: if(*((int*)_tmp749)!= 3)goto _LL3C0;_tmp752=((struct
Cyc_Absyn_Primop_e_struct*)_tmp749)->f2;_LL3BF:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp752);goto _LL3AD;
_LL3C0: if(*((int*)_tmp749)!= 7)goto _LL3C2;_tmp753=((struct Cyc_Absyn_And_e_struct*)
_tmp749)->f1;_tmp754=((struct Cyc_Absyn_And_e_struct*)_tmp749)->f2;_LL3C1: _tmp755=
_tmp753;_tmp756=_tmp754;goto _LL3C3;_LL3C2: if(*((int*)_tmp749)!= 8)goto _LL3C4;
_tmp755=((struct Cyc_Absyn_Or_e_struct*)_tmp749)->f1;_tmp756=((struct Cyc_Absyn_Or_e_struct*)
_tmp749)->f2;_LL3C3: _tmp757=_tmp755;_tmp758=_tmp756;goto _LL3C5;_LL3C4: if(*((int*)
_tmp749)!= 9)goto _LL3C6;_tmp757=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp749)->f1;
_tmp758=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp749)->f2;_LL3C5: _tmp759=_tmp757;
_tmp75A=_tmp758;goto _LL3C7;_LL3C6: if(*((int*)_tmp749)!= 25)goto _LL3C8;_tmp759=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp749)->f1;_tmp75A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp749)->f2;_LL3C7: _tmp75B=_tmp759;_tmp75C=_tmp75A;goto _LL3C9;_LL3C8: if(*((int*)
_tmp749)!= 4)goto _LL3CA;_tmp75B=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp749)->f1;
_tmp75C=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp749)->f3;_LL3C9: Cyc_Toc_exptypes_to_c(
_tmp75B);Cyc_Toc_exptypes_to_c(_tmp75C);goto _LL3AD;_LL3CA: if(*((int*)_tmp749)!= 
6)goto _LL3CC;_tmp75D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp749)->f1;
_tmp75E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp749)->f2;_tmp75F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp749)->f3;_LL3CB: Cyc_Toc_exptypes_to_c(_tmp75D);Cyc_Toc_exptypes_to_c(_tmp75E);
Cyc_Toc_exptypes_to_c(_tmp75F);goto _LL3AD;_LL3CC: if(*((int*)_tmp749)!= 11)goto
_LL3CE;_tmp760=((struct Cyc_Absyn_FnCall_e_struct*)_tmp749)->f1;_tmp761=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp749)->f2;_LL3CD: _tmp762=_tmp760;_tmp763=_tmp761;
goto _LL3CF;_LL3CE: if(*((int*)_tmp749)!= 10)goto _LL3D0;_tmp762=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp749)->f1;_tmp763=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp749)->f2;_LL3CF:
Cyc_Toc_exptypes_to_c(_tmp762);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp763);goto _LL3AD;_LL3D0:
if(*((int*)_tmp749)!= 15)goto _LL3D2;_tmp764=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp749)->f1;_tmp765=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp749)->f1);
_tmp766=((struct Cyc_Absyn_Cast_e_struct*)_tmp749)->f2;_LL3D1:*_tmp765=Cyc_Toc_typ_to_c(*
_tmp765);Cyc_Toc_exptypes_to_c(_tmp766);goto _LL3AD;_LL3D2: if(*((int*)_tmp749)!= 
27)goto _LL3D4;_tmp767=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp749)->f1;
_tmp768=*_tmp767;_tmp769=_tmp768.f3;_tmp76A=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp749)->f1).f3;_tmp76B=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp749)->f2;
_LL3D3:*_tmp76A=Cyc_Toc_typ_to_c(*_tmp76A);_tmp76C=_tmp76B;goto _LL3D5;_LL3D4: if(*((
int*)_tmp749)!= 36)goto _LL3D6;_tmp76C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp749)->f2;_LL3D5: _tmp76D=_tmp76C;goto _LL3D7;_LL3D6: if(*((int*)_tmp749)!= 28)
goto _LL3D8;_tmp76D=((struct Cyc_Absyn_Array_e_struct*)_tmp749)->f1;_LL3D7: for(0;
_tmp76D != 0;_tmp76D=_tmp76D->tl){struct Cyc_Absyn_Exp*_tmp776;struct _tuple6
_tmp775=*((struct _tuple6*)_tmp76D->hd);_tmp776=_tmp775.f2;Cyc_Toc_exptypes_to_c(
_tmp776);}goto _LL3AD;_LL3D8: if(*((int*)_tmp749)!= 20)goto _LL3DA;_tmp76E=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp749)->f1;_tmp76F=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp749)->f1);_LL3D9: _tmp771=_tmp76F;goto
_LL3DB;_LL3DA: if(*((int*)_tmp749)!= 18)goto _LL3DC;_tmp770=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp749)->f1;_tmp771=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp749)->f1);_LL3DB:*_tmp771=Cyc_Toc_typ_to_c(*_tmp771);goto _LL3AD;_LL3DC: if(*((
int*)_tmp749)!= 37)goto _LL3DE;_tmp772=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp749)->f1;_LL3DD: Cyc_Toc_stmttypes_to_c(_tmp772);goto _LL3AD;_LL3DE: if(*((int*)
_tmp749)!= 35)goto _LL3E0;_tmp773=((struct Cyc_Absyn_Malloc_e_struct*)_tmp749)->f1;
_tmp774=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp749)->f1;
_LL3DF: if(_tmp774->elt_type != 0)_tmp774->elt_type=({void**_tmp777=_cycalloc(
sizeof(*_tmp777));_tmp777[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp774->elt_type)));
_tmp777;});Cyc_Toc_exptypes_to_c(_tmp774->num_elts);goto _LL3AD;_LL3E0: if(*((int*)
_tmp749)!= 0)goto _LL3E2;_LL3E1: goto _LL3E3;_LL3E2: if(*((int*)_tmp749)!= 1)goto
_LL3E4;_LL3E3: goto _LL3E5;_LL3E4: if(*((int*)_tmp749)!= 2)goto _LL3E6;_LL3E5: goto
_LL3E7;_LL3E6: if(*((int*)_tmp749)!= 33)goto _LL3E8;_LL3E7: goto _LL3E9;_LL3E8: if(*((
int*)_tmp749)!= 34)goto _LL3EA;_LL3E9: goto _LL3AD;_LL3EA: if(*((int*)_tmp749)!= 31)
goto _LL3EC;_LL3EB: goto _LL3ED;_LL3EC: if(*((int*)_tmp749)!= 32)goto _LL3EE;_LL3ED:
goto _LL3EF;_LL3EE: if(*((int*)_tmp749)!= 30)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0:
if(*((int*)_tmp749)!= 29)goto _LL3F2;_LL3F1: goto _LL3F3;_LL3F2: if(*((int*)_tmp749)
!= 26)goto _LL3F4;_LL3F3: goto _LL3F5;_LL3F4: if(*((int*)_tmp749)!= 14)goto _LL3F6;
_LL3F5: goto _LL3F7;_LL3F6: if(*((int*)_tmp749)!= 17)goto _LL3F8;_LL3F7: goto _LL3F9;
_LL3F8: if(*((int*)_tmp749)!= 21)goto _LL3AD;_LL3F9:({void*_tmp778[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp779="Cyclone expression within C code";_tag_dynforward(
_tmp779,sizeof(char),_get_zero_arr_size(_tmp779,33));}),_tag_dynforward(_tmp778,
sizeof(void*),0));});goto _LL3AD;_LL3AD:;}static void Cyc_Toc_decltypes_to_c(struct
Cyc_Absyn_Decl*d){void*_tmp77A=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp77B;
struct Cyc_Absyn_Fndecl*_tmp77C;struct Cyc_Absyn_Aggrdecl*_tmp77D;struct Cyc_Absyn_Enumdecl*
_tmp77E;struct Cyc_Absyn_Typedefdecl*_tmp77F;_LL3FB: if(_tmp77A <= (void*)2)goto
_LL413;if(*((int*)_tmp77A)!= 0)goto _LL3FD;_tmp77B=((struct Cyc_Absyn_Var_d_struct*)
_tmp77A)->f1;_LL3FC:(void*)(_tmp77B->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp77B->type));
if(_tmp77B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp77B->initializer));goto _LL3FA;_LL3FD: if(*((int*)_tmp77A)!= 1)
goto _LL3FF;_tmp77C=((struct Cyc_Absyn_Fn_d_struct*)_tmp77A)->f1;_LL3FE:(void*)(
_tmp77C->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp77C->ret_type));{struct Cyc_List_List*
_tmp780=_tmp77C->args;for(0;_tmp780 != 0;_tmp780=_tmp780->tl){(*((struct _tuple15*)
_tmp780->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp780->hd)).f3);}}goto
_LL3FA;_LL3FF: if(*((int*)_tmp77A)!= 4)goto _LL401;_tmp77D=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp77A)->f1;_LL400: Cyc_Toc_aggrdecl_to_c(_tmp77D);goto _LL3FA;_LL401: if(*((int*)
_tmp77A)!= 6)goto _LL403;_tmp77E=((struct Cyc_Absyn_Enum_d_struct*)_tmp77A)->f1;
_LL402: if(_tmp77E->fields != 0){struct Cyc_List_List*_tmp781=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp77E->fields))->v;for(0;_tmp781 != 0;_tmp781=
_tmp781->tl){struct Cyc_Absyn_Enumfield*_tmp782=(struct Cyc_Absyn_Enumfield*)
_tmp781->hd;if(_tmp782->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp782->tag));}}goto _LL3FA;_LL403: if(*((int*)_tmp77A)!= 7)goto
_LL405;_tmp77F=((struct Cyc_Absyn_Typedef_d_struct*)_tmp77A)->f1;_LL404: _tmp77F->defn=({
struct Cyc_Core_Opt*_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp77F->defn))->v);_tmp783;});goto
_LL3FA;_LL405: if(*((int*)_tmp77A)!= 2)goto _LL407;_LL406: goto _LL408;_LL407: if(*((
int*)_tmp77A)!= 3)goto _LL409;_LL408: goto _LL40A;_LL409: if(*((int*)_tmp77A)!= 5)
goto _LL40B;_LL40A: goto _LL40C;_LL40B: if(*((int*)_tmp77A)!= 8)goto _LL40D;_LL40C:
goto _LL40E;_LL40D: if(*((int*)_tmp77A)!= 9)goto _LL40F;_LL40E: goto _LL410;_LL40F:
if(*((int*)_tmp77A)!= 10)goto _LL411;_LL410: goto _LL412;_LL411: if(*((int*)_tmp77A)
!= 11)goto _LL413;_LL412:({void*_tmp784[0]={};Cyc_Tcutil_terr(d->loc,({const char*
_tmp785="Cyclone declaration within C code";_tag_dynforward(_tmp785,sizeof(char),
_get_zero_arr_size(_tmp785,34));}),_tag_dynforward(_tmp784,sizeof(void*),0));});
goto _LL3FA;_LL413: if((int)_tmp77A != 0)goto _LL415;_LL414: goto _LL416;_LL415: if((
int)_tmp77A != 1)goto _LL3FA;_LL416: goto _LL3FA;_LL3FA:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp786=(void*)s->r;struct Cyc_Absyn_Exp*_tmp787;
struct Cyc_Absyn_Stmt*_tmp788;struct Cyc_Absyn_Stmt*_tmp789;struct Cyc_Absyn_Exp*
_tmp78A;struct Cyc_Absyn_Exp*_tmp78B;struct Cyc_Absyn_Stmt*_tmp78C;struct Cyc_Absyn_Stmt*
_tmp78D;struct _tuple3 _tmp78E;struct Cyc_Absyn_Exp*_tmp78F;struct Cyc_Absyn_Stmt*
_tmp790;struct Cyc_Absyn_Exp*_tmp791;struct _tuple3 _tmp792;struct Cyc_Absyn_Exp*
_tmp793;struct _tuple3 _tmp794;struct Cyc_Absyn_Exp*_tmp795;struct Cyc_Absyn_Stmt*
_tmp796;struct Cyc_Absyn_Exp*_tmp797;struct Cyc_List_List*_tmp798;struct Cyc_Absyn_Decl*
_tmp799;struct Cyc_Absyn_Stmt*_tmp79A;struct Cyc_Absyn_Stmt*_tmp79B;struct _tuple3
_tmp79C;struct Cyc_Absyn_Exp*_tmp79D;_LL418: if(_tmp786 <= (void*)1)goto _LL42A;if(*((
int*)_tmp786)!= 0)goto _LL41A;_tmp787=((struct Cyc_Absyn_Exp_s_struct*)_tmp786)->f1;
_LL419: Cyc_Toc_exptypes_to_c(_tmp787);goto _LL417;_LL41A: if(*((int*)_tmp786)!= 1)
goto _LL41C;_tmp788=((struct Cyc_Absyn_Seq_s_struct*)_tmp786)->f1;_tmp789=((struct
Cyc_Absyn_Seq_s_struct*)_tmp786)->f2;_LL41B: Cyc_Toc_stmttypes_to_c(_tmp788);Cyc_Toc_stmttypes_to_c(
_tmp789);goto _LL417;_LL41C: if(*((int*)_tmp786)!= 2)goto _LL41E;_tmp78A=((struct
Cyc_Absyn_Return_s_struct*)_tmp786)->f1;_LL41D: if(_tmp78A != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp78A);goto _LL417;_LL41E: if(*((int*)_tmp786)!= 3)goto
_LL420;_tmp78B=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp786)->f1;_tmp78C=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp786)->f2;_tmp78D=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp786)->f3;_LL41F: Cyc_Toc_exptypes_to_c(_tmp78B);Cyc_Toc_stmttypes_to_c(
_tmp78C);Cyc_Toc_stmttypes_to_c(_tmp78D);goto _LL417;_LL420: if(*((int*)_tmp786)!= 
4)goto _LL422;_tmp78E=((struct Cyc_Absyn_While_s_struct*)_tmp786)->f1;_tmp78F=
_tmp78E.f1;_tmp790=((struct Cyc_Absyn_While_s_struct*)_tmp786)->f2;_LL421: Cyc_Toc_exptypes_to_c(
_tmp78F);Cyc_Toc_stmttypes_to_c(_tmp790);goto _LL417;_LL422: if(*((int*)_tmp786)!= 
8)goto _LL424;_tmp791=((struct Cyc_Absyn_For_s_struct*)_tmp786)->f1;_tmp792=((
struct Cyc_Absyn_For_s_struct*)_tmp786)->f2;_tmp793=_tmp792.f1;_tmp794=((struct
Cyc_Absyn_For_s_struct*)_tmp786)->f3;_tmp795=_tmp794.f1;_tmp796=((struct Cyc_Absyn_For_s_struct*)
_tmp786)->f4;_LL423: Cyc_Toc_exptypes_to_c(_tmp791);Cyc_Toc_exptypes_to_c(_tmp793);
Cyc_Toc_exptypes_to_c(_tmp795);Cyc_Toc_stmttypes_to_c(_tmp796);goto _LL417;_LL424:
if(*((int*)_tmp786)!= 9)goto _LL426;_tmp797=((struct Cyc_Absyn_Switch_s_struct*)
_tmp786)->f1;_tmp798=((struct Cyc_Absyn_Switch_s_struct*)_tmp786)->f2;_LL425: Cyc_Toc_exptypes_to_c(
_tmp797);for(0;_tmp798 != 0;_tmp798=_tmp798->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp798->hd)->body);}goto _LL417;_LL426: if(*((int*)
_tmp786)!= 11)goto _LL428;_tmp799=((struct Cyc_Absyn_Decl_s_struct*)_tmp786)->f1;
_tmp79A=((struct Cyc_Absyn_Decl_s_struct*)_tmp786)->f2;_LL427: Cyc_Toc_decltypes_to_c(
_tmp799);Cyc_Toc_stmttypes_to_c(_tmp79A);goto _LL417;_LL428: if(*((int*)_tmp786)!= 
13)goto _LL42A;_tmp79B=((struct Cyc_Absyn_Do_s_struct*)_tmp786)->f1;_tmp79C=((
struct Cyc_Absyn_Do_s_struct*)_tmp786)->f2;_tmp79D=_tmp79C.f1;_LL429: Cyc_Toc_stmttypes_to_c(
_tmp79B);Cyc_Toc_exptypes_to_c(_tmp79D);goto _LL417;_LL42A: if((int)_tmp786 != 0)
goto _LL42C;_LL42B: goto _LL42D;_LL42C: if(_tmp786 <= (void*)1)goto _LL42E;if(*((int*)
_tmp786)!= 5)goto _LL42E;_LL42D: goto _LL42F;_LL42E: if(_tmp786 <= (void*)1)goto
_LL430;if(*((int*)_tmp786)!= 6)goto _LL430;_LL42F: goto _LL431;_LL430: if(_tmp786 <= (
void*)1)goto _LL432;if(*((int*)_tmp786)!= 7)goto _LL432;_LL431: goto _LL417;_LL432:
if(_tmp786 <= (void*)1)goto _LL434;if(*((int*)_tmp786)!= 10)goto _LL434;_LL433: goto
_LL435;_LL434: if(_tmp786 <= (void*)1)goto _LL436;if(*((int*)_tmp786)!= 12)goto
_LL436;_LL435: goto _LL437;_LL436: if(_tmp786 <= (void*)1)goto _LL438;if(*((int*)
_tmp786)!= 14)goto _LL438;_LL437: goto _LL439;_LL438: if(_tmp786 <= (void*)1)goto
_LL43A;if(*((int*)_tmp786)!= 15)goto _LL43A;_LL439: goto _LL43B;_LL43A: if(_tmp786 <= (
void*)1)goto _LL417;if(*((int*)_tmp786)!= 16)goto _LL417;_LL43B:({void*_tmp79E[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp79F="Cyclone statement in C code";
_tag_dynforward(_tmp79F,sizeof(char),_get_zero_arr_size(_tmp79F,28));}),
_tag_dynforward(_tmp79E,sizeof(void*),0));});goto _LL417;_LL417:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({
void*_tmp7A0[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp7A1="decls_to_c: not at toplevel!";
_tag_dynforward(_tmp7A1,sizeof(char),_get_zero_arr_size(_tmp7A1,29));}),
_tag_dynforward(_tmp7A0,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp7A2=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp7A3;struct Cyc_Absyn_Fndecl*
_tmp7A4;struct Cyc_Absyn_Aggrdecl*_tmp7A5;struct Cyc_Absyn_Tuniondecl*_tmp7A6;
struct Cyc_Absyn_Enumdecl*_tmp7A7;struct Cyc_Absyn_Typedefdecl*_tmp7A8;struct Cyc_List_List*
_tmp7A9;struct Cyc_List_List*_tmp7AA;struct Cyc_List_List*_tmp7AB;struct Cyc_List_List*
_tmp7AC;_LL43D: if(_tmp7A2 <= (void*)2)goto _LL44D;if(*((int*)_tmp7A2)!= 0)goto
_LL43F;_tmp7A3=((struct Cyc_Absyn_Var_d_struct*)_tmp7A2)->f1;_LL43E: {struct
_tuple1*_tmp7AD=_tmp7A3->name;if((void*)_tmp7A3->sc == (void*)4)_tmp7AD=({struct
_tuple1*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));_tmp7AE->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7AF;(_tmp7AF.Rel_n).tag=1;(_tmp7AF.Rel_n).f1=0;
_tmp7AF;});_tmp7AE->f2=(*_tmp7AD).f2;_tmp7AE;});if(_tmp7A3->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp7A3->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp7A3->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp7A3->name,Cyc_Absyn_varb_exp(_tmp7AD,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0[0]=({
struct Cyc_Absyn_Global_b_struct _tmp7B1;_tmp7B1.tag=0;_tmp7B1.f1=_tmp7A3;_tmp7B1;});
_tmp7B0;}),0));_tmp7A3->name=_tmp7AD;(void*)(_tmp7A3->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp7A3->sc));(void*)(_tmp7A3->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp7A3->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7B2=_cycalloc(
sizeof(*_tmp7B2));_tmp7B2->hd=d;_tmp7B2->tl=Cyc_Toc_result_decls;_tmp7B2;});goto
_LL43C;}_LL43F: if(*((int*)_tmp7A2)!= 1)goto _LL441;_tmp7A4=((struct Cyc_Absyn_Fn_d_struct*)
_tmp7A2)->f1;_LL440: {struct _tuple1*_tmp7B3=_tmp7A4->name;if((void*)_tmp7A4->sc
== (void*)4){_tmp7B3=({struct _tuple1*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4->f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7B5;(_tmp7B5.Rel_n).tag=
1;(_tmp7B5.Rel_n).f1=0;_tmp7B5;});_tmp7B4->f2=(*_tmp7B3).f2;_tmp7B4;});(void*)(
_tmp7A4->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,nv,_tmp7A4->name,Cyc_Absyn_var_exp(
_tmp7B3,0));_tmp7A4->name=_tmp7B3;Cyc_Toc_fndecl_to_c(nv,_tmp7A4,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6->hd=d;_tmp7B6->tl=
Cyc_Toc_result_decls;_tmp7B6;});goto _LL43C;}_LL441: if(*((int*)_tmp7A2)!= 2)goto
_LL443;_LL442: goto _LL444;_LL443: if(*((int*)_tmp7A2)!= 3)goto _LL445;_LL444:({void*
_tmp7B7[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp7B8="letdecl at toplevel";_tag_dynforward(_tmp7B8,sizeof(char),
_get_zero_arr_size(_tmp7B8,20));}),_tag_dynforward(_tmp7B7,sizeof(void*),0));});
_LL445: if(*((int*)_tmp7A2)!= 4)goto _LL447;_tmp7A5=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp7A2)->f1;_LL446: Cyc_Toc_aggrdecl_to_c(_tmp7A5);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9->hd=d;_tmp7B9->tl=Cyc_Toc_result_decls;
_tmp7B9;});goto _LL43C;_LL447: if(*((int*)_tmp7A2)!= 5)goto _LL449;_tmp7A6=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp7A2)->f1;_LL448: if(_tmp7A6->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp7A6);else{Cyc_Toc_tuniondecl_to_c(_tmp7A6);}goto _LL43C;_LL449: if(*((int*)
_tmp7A2)!= 6)goto _LL44B;_tmp7A7=((struct Cyc_Absyn_Enum_d_struct*)_tmp7A2)->f1;
_LL44A: Cyc_Toc_enumdecl_to_c(nv,_tmp7A7);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp7BA=_cycalloc(sizeof(*_tmp7BA));_tmp7BA->hd=d;_tmp7BA->tl=Cyc_Toc_result_decls;
_tmp7BA;});goto _LL43C;_LL44B: if(*((int*)_tmp7A2)!= 7)goto _LL44D;_tmp7A8=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp7A2)->f1;_LL44C: _tmp7A8->name=_tmp7A8->name;
_tmp7A8->tvs=0;if(_tmp7A8->defn != 0)_tmp7A8->defn=({struct Cyc_Core_Opt*_tmp7BB=
_cycalloc(sizeof(*_tmp7BB));_tmp7BB->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp7A8->defn))->v);_tmp7BB;});else{void*_tmp7BC=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7A8->kind))->v;_LL45A: if((int)
_tmp7BC != 2)goto _LL45C;_LL45B: _tmp7A8->defn=({struct Cyc_Core_Opt*_tmp7BD=
_cycalloc(sizeof(*_tmp7BD));_tmp7BD->v=(void*)Cyc_Absyn_void_star_typ();_tmp7BD;});
goto _LL459;_LL45C:;_LL45D: _tmp7A8->defn=({struct Cyc_Core_Opt*_tmp7BE=_cycalloc(
sizeof(*_tmp7BE));_tmp7BE->v=(void*)((void*)0);_tmp7BE;});goto _LL459;_LL459:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));
_tmp7BF->hd=d;_tmp7BF->tl=Cyc_Toc_result_decls;_tmp7BF;});goto _LL43C;_LL44D: if((
int)_tmp7A2 != 0)goto _LL44F;_LL44E: goto _LL450;_LL44F: if((int)_tmp7A2 != 1)goto
_LL451;_LL450: goto _LL43C;_LL451: if(_tmp7A2 <= (void*)2)goto _LL453;if(*((int*)
_tmp7A2)!= 8)goto _LL453;_tmp7A9=((struct Cyc_Absyn_Namespace_d_struct*)_tmp7A2)->f2;
_LL452: _tmp7AA=_tmp7A9;goto _LL454;_LL453: if(_tmp7A2 <= (void*)2)goto _LL455;if(*((
int*)_tmp7A2)!= 9)goto _LL455;_tmp7AA=((struct Cyc_Absyn_Using_d_struct*)_tmp7A2)->f2;
_LL454: _tmp7AB=_tmp7AA;goto _LL456;_LL455: if(_tmp7A2 <= (void*)2)goto _LL457;if(*((
int*)_tmp7A2)!= 10)goto _LL457;_tmp7AB=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp7A2)->f1;_LL456: nv=Cyc_Toc_decls_to_c(r,nv,_tmp7AB,top,cinclude);goto _LL43C;
_LL457: if(_tmp7A2 <= (void*)2)goto _LL43C;if(*((int*)_tmp7A2)!= 11)goto _LL43C;
_tmp7AC=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp7A2)->f1;_LL458: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp7AC,top,1);goto _LL43C;_LL43C:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{
struct _RegionHandle _tmp7C0=_new_region("start");struct _RegionHandle*start=&
_tmp7C0;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,
1,0);;_pop_region(start);}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
