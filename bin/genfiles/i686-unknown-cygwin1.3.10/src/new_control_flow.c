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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
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
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_Hashtable_Table;
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*
cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*
t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,
void*key);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(void*t);struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noreturn(void*);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
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
struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);struct Cyc_CfFlowInfo_BottomFL_struct{int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};union Cyc_CfFlowInfo_FlowInfo_union{struct Cyc_CfFlowInfo_BottomFL_struct
BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{
struct _RegionHandle*r;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);struct _dynforward_ptr
Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,
void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,
void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r);void*
Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);struct
Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,
struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,
union Cyc_CfFlowInfo_FlowInfo_union f,int clear);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c);struct _dynforward_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;})));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_AbsLVal_union f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)goto _LL3;_tmp4=((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6=_cycalloc(sizeof(*
_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;
_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";
_tag_dynforward(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,37));});_tmp7;});
_tmp6;}));_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*Cyc_NewControlFlow_get_stmt_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union**
sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);if(sflow
== 0){union Cyc_CfFlowInfo_FlowInfo_union*res=({union Cyc_CfFlowInfo_FlowInfo_union*
_tmp9=_region_malloc(env->r,sizeof(*_tmp9));_tmp9[0]=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmpA;(_tmpA.BottomFL).tag=0;_tmpA;});_tmp9;});((
void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo_union*
val))Cyc_Hashtable_insert)(env->flow_table,s,res);return res;}return*sflow;}
struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo_union*
f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo_union*
_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*_tmpC=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,inflow,*_tmpC,1);_tmpB->visited=env->iteration_num;
return({struct _tuple9 _tmpD;_tmpD.f1=_tmpB;_tmpD.f2=_tmpC;_tmpD;});}static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dynforward_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dynforward_ptr*_tmpE=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmpE,f)== 0)return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Core_Impossible_struct
_tmp10;_tmp10.tag=Cyc_Core_Impossible;_tmp10.f1=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*f);{void*_tmp11[1]={& _tmp13};Cyc_aprintf(({const char*
_tmp12="get_field_index_fs failed: %s";_tag_dynforward(_tmp12,sizeof(char),
_get_zero_arr_size(_tmp12,30));}),_tag_dynforward(_tmp11,sizeof(void*),1));}});
_tmp10;});_tmpF;}));}static int Cyc_NewControlFlow_get_field_index(void*t,struct
_dynforward_ptr*f){void*_tmp14=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp15;union Cyc_Absyn_AggrInfoU_union _tmp16;struct Cyc_List_List*_tmp17;_LL6: if(
_tmp14 <= (void*)4)goto _LLA;if(*((int*)_tmp14)!= 10)goto _LL8;_tmp15=((struct Cyc_Absyn_AggrType_struct*)
_tmp14)->f1;_tmp16=_tmp15.aggr_info;_LL7: {struct Cyc_List_List*_tmp18=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp16))->impl))->fields;
_tmp17=_tmp18;goto _LL9;}_LL8: if(*((int*)_tmp14)!= 11)goto _LLA;_tmp17=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp17,f);_LLA:;
_LLB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp19=_cycalloc(
sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Core_Impossible_struct _tmp1A;_tmp1A.tag=
Cyc_Core_Impossible;_tmp1A.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1B[1]={& _tmp1D};Cyc_aprintf(({const char*_tmp1C="get_field_index failed: %s";
_tag_dynforward(_tmp1C,sizeof(char),_get_zero_arr_size(_tmp1C,27));}),
_tag_dynforward(_tmp1B,sizeof(void*),1));}});_tmp1A;});_tmp19;}));_LL5:;}static
void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow,1);}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(
env,s);union Cyc_CfFlowInfo_FlowInfo_union _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,flow,*_tmp1F,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*
_tmp1F)){*_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo_union
_tmp21=inflow;struct Cyc_Dict_Dict _tmp22;struct Cyc_List_List*_tmp23;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp24;_LLD: if((_tmp21.BottomFL).tag != 0)goto _LLF;_LLE: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp25;(_tmp25.BottomFL).tag=0;_tmp25;});_LLF:
if((_tmp21.ReachableFL).tag != 1)goto _LLC;_tmp22=(_tmp21.ReachableFL).f1;_tmp23=(
_tmp21.ReachableFL).f2;_tmp24=(_tmp21.ReachableFL).f3;_LL10: for(0;vds != 0;vds=
vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp26=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp27=_region_malloc(fenv->r,sizeof(*_tmp27));_tmp27[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp28;_tmp28.tag=0;_tmp28.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp28;});_tmp27;});
_tmp22=Cyc_Dict_insert(_tmp22,(void*)_tmp26,Cyc_CfFlowInfo_typ_to_absrval(fenv,(
void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp29;(_tmp29.ReachableFL).tag=1;(_tmp29.ReachableFL).f1=
_tmp22;(_tmp29.ReachableFL).f2=_tmp23;(_tmp29.ReachableFL).f3=_tmp24;_tmp29;});
_LLC:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp2B;
struct Cyc_Dict_Dict _tmp2C;struct _tuple6 _tmp2A=Cyc_CfFlowInfo_save_consume_info(
fenv,outflow,0);_tmp2B=_tmp2A.f1;_tmp2C=_tmp2B.consumed;{struct _RegionHandle
_tmp2D=_new_region("tmp");struct _RegionHandle*tmp=& _tmp2D;_push_region(tmp);{
struct Cyc_Iter_Iter _tmp2E=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(tmp,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple10 _tmp2F=*((struct _tuple10*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(tmp,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp2E,& _tmp2F)){struct Cyc_CfFlowInfo_Place*_tmp30=_tmp2F.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp2C,_tmp30))({
struct Cyc_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(_tmp30));{void*_tmp31[1]={&
_tmp33};Cyc_Tcutil_terr(_tmp2F.f2,({const char*_tmp32="Failed to consume unique variable %s";
_tag_dynforward(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,37));}),
_tag_dynforward(_tmp31,sizeof(void*),1));}});}};_pop_region(tmp);}*
out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union _tmp34=inflow;struct Cyc_Dict_Dict
_tmp35;struct Cyc_List_List*_tmp36;struct Cyc_CfFlowInfo_ConsumeInfo _tmp37;_LL12:
if((_tmp34.BottomFL).tag != 0)goto _LL14;_LL13: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp38;(_tmp38.BottomFL).tag=0;_tmp38;});_LL14:
if((_tmp34.ReachableFL).tag != 1)goto _LL11;_tmp35=(_tmp34.ReachableFL).f1;_tmp36=(
_tmp34.ReachableFL).f2;_tmp37=(_tmp34.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp35,r)!= (void*)2)({void*_tmp39[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp3A="expression may not be fully initialized";
_tag_dynforward(_tmp3A,sizeof(char),_get_zero_arr_size(_tmp3A,40));}),
_tag_dynforward(_tmp39,sizeof(void*),0));});{struct Cyc_Dict_Dict _tmp3B=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp35,env->all_changed,r);if(_tmp35.t == _tmp3B.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3C=({union Cyc_CfFlowInfo_FlowInfo_union _tmp3D;(
_tmp3D.ReachableFL).tag=1;(_tmp3D.ReachableFL).f1=_tmp3B;(_tmp3D.ReachableFL).f2=
_tmp36;(_tmp3D.ReachableFL).f3=_tmp37;_tmp3D;});Cyc_NewControlFlow_update_tryflow(
env,(union Cyc_CfFlowInfo_FlowInfo_union)_tmp3C);return(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp3C;}}_LL11:;}struct _tuple11{union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*
f2;};static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(struct
_RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct
_tuple11 _tmp3E;_tmp3E.f1=inflow;_tmp3E.f2=0;_tmp3E;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp40;void*_tmp41;struct _tuple5 _tmp3F=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp40=_tmp3F.f1;_tmp41=_tmp3F.f2;return({struct
_tuple11 _tmp42;_tmp42.f1=_tmp40;_tmp42.f2=({struct Cyc_List_List*_tmp43=
_region_malloc(rgn,sizeof(*_tmp43));_tmp43->hd=(void*)_tmp41;_tmp43->tl=0;_tmp43;});
_tmp42;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union old_inflow;union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp45;union
Cyc_CfFlowInfo_FlowInfo_union _tmp46;struct _tuple6 _tmp44=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp45=_tmp44.f1;_tmp46=_tmp44.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp5B;_tmp5B.consumed=(env->fenv)->mt_place_set;
_tmp5B.may_consume=0;_tmp5B;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp46=Cyc_CfFlowInfo_restore_consume_info(_tmp46,_tmp45);env->all_changed=({
struct Cyc_Dict_Dict*_tmp47=_region_malloc(env->r,sizeof(*_tmp47));_tmp47[0]=(env->fenv)->mt_place_set;
_tmp47;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp49;void*_tmp4A;struct _tuple5
_tmp48=Cyc_NewControlFlow_anal_Rexp(env,_tmp46,(struct Cyc_Absyn_Exp*)es->hd);
_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;outflow=_tmp49;rvals=({struct Cyc_List_List*
_tmp4B=_region_malloc(rgn,sizeof(*_tmp4B));_tmp4B->hd=(void*)_tmp4A;_tmp4B->tl=0;
_tmp4B;});this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((
struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(arg1_unconsumed){union
Cyc_CfFlowInfo_FlowInfo_union _tmp4C=outflow;struct Cyc_Dict_Dict _tmp4D;struct Cyc_List_List*
_tmp4E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4F;_LL17: if((_tmp4C.BottomFL).tag != 0)
goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp4C.ReachableFL).tag != 1)goto _LL16;_tmp4D=(
_tmp4C.ReachableFL).f1;_tmp4E=(_tmp4C.ReachableFL).f2;_tmp4F=(_tmp4C.ReachableFL).f3;
_LL1A: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp50;(_tmp50.ReachableFL).tag=1;(_tmp50.ReachableFL).f1=_tmp4D;(_tmp50.ReachableFL).f2=
_tmp4E;(_tmp50.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp51;_tmp51.consumed=
_tmp4F.consumed;_tmp51.may_consume=_tmp45.may_consume;_tmp51;});_tmp50;});_LL16:;}{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Dict_Dict*_tmp52=_region_malloc(env->r,sizeof(*_tmp52));_tmp52[0]=(env->fenv)->mt_place_set;
_tmp52;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp54;void*_tmp55;struct _tuple5
_tmp53=Cyc_NewControlFlow_anal_Rexp(env,_tmp46,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;rvals=({struct Cyc_List_List*_tmp56=
_region_malloc(rgn,sizeof(*_tmp56));_tmp56->hd=(void*)_tmp55;_tmp56->tl=rvals;
_tmp56;});outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp54,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp58;struct _tuple6 _tmp57=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp58=_tmp57.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp46,outflow_consume);else{old_inflow=_tmp46;}init_consume=1;outflow_consume=
_tmp58;_tmp46=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,_tmp46,
outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp46,old_inflow));if(
outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict*
_tmp59=_region_malloc(env->r,sizeof(*_tmp59));_tmp59[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp59;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple11 _tmp5A;_tmp5A.f1=outflow;_tmp5A.f2=Cyc_List_imp_rev(
rvals);_tmp5A;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp5C=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp5C;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple11 _tmp5D=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp60=_tmp5E;struct Cyc_Dict_Dict _tmp61;_LL1C: if((_tmp60.ReachableFL).tag != 1)
goto _LL1E;_tmp61=(_tmp60.ReachableFL).f1;_LL1D: for(0;_tmp5F != 0;(_tmp5F=_tmp5F->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp61,(void*)_tmp5F->hd)== (void*)0)({void*_tmp62[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp63="expression may not be initialized";_tag_dynforward(_tmp63,sizeof(
char),_get_zero_arr_size(_tmp63,34));}),_tag_dynforward(_tmp62,sizeof(void*),0));});}
goto _LL1B;_LL1E: if((_tmp60.BottomFL).tag != 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{
struct _tuple5 _tmp65=({struct _tuple5 _tmp64;_tmp64.f1=_tmp5E;_tmp64.f2=(void*)(env->fenv)->unknown_all;
_tmp64;});_npop_handler(0);return _tmp65;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp66=outflow;struct Cyc_Dict_Dict _tmp67;
struct Cyc_List_List*_tmp68;struct Cyc_CfFlowInfo_ConsumeInfo _tmp69;_LL21: if((
_tmp66.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23: if((_tmp66.ReachableFL).tag
!= 1)goto _LL20;_tmp67=(_tmp66.ReachableFL).f1;_tmp68=(_tmp66.ReachableFL).f2;
_tmp69=(_tmp66.ReachableFL).f3;_LL24: {union Cyc_CfFlowInfo_AbsLVal_union _tmp6A=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp6B;
_LL26: if((_tmp6A.UnknownL).tag != 1)goto _LL28;_LL27: return outflow;_LL28: if((
_tmp6A.PlaceL).tag != 0)goto _LL25;_tmp6B=(_tmp6A.PlaceL).f1;_LL29: {void*nzval=il
== (void*)2?(void*)1:(void*)2;return(union Cyc_CfFlowInfo_FlowInfo_union)({union
Cyc_CfFlowInfo_FlowInfo_union _tmp6C;(_tmp6C.ReachableFL).tag=1;(_tmp6C.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp67,env->all_changed,_tmp6B,
nzval);(_tmp6C.ReachableFL).f2=_tmp68;(_tmp6C.ReachableFL).f3=_tmp69;_tmp6C;});}
_LL25:;}_LL20:;}static struct _tuple8 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp6D=
outflow;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp70;_LL2B: if((_tmp6D.BottomFL).tag != 0)goto _LL2D;_LL2C: return({struct _tuple8
_tmp71;_tmp71.f1=outflow;_tmp71.f2=outflow;_tmp71;});_LL2D: if((_tmp6D.ReachableFL).tag
!= 1)goto _LL2A;_tmp6E=(_tmp6D.ReachableFL).f1;_tmp6F=(_tmp6D.ReachableFL).f2;
_tmp70=(_tmp6D.ReachableFL).f3;_LL2E: {union Cyc_CfFlowInfo_AbsLVal_union _tmp72=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp73;
_LL30: if((_tmp72.UnknownL).tag != 1)goto _LL32;_LL31: return({struct _tuple8 _tmp74;
_tmp74.f1=outflow;_tmp74.f2=outflow;_tmp74;});_LL32: if((_tmp72.PlaceL).tag != 0)
goto _LL2F;_tmp73=(_tmp72.PlaceL).f1;_LL33: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple8 _tmp75;_tmp75.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp77;(_tmp77.ReachableFL).tag=1;(_tmp77.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6E,env->all_changed,_tmp73,
nzval);(_tmp77.ReachableFL).f2=_tmp6F;(_tmp77.ReachableFL).f3=_tmp70;_tmp77;});
_tmp75.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp76;(_tmp76.ReachableFL).tag=1;(_tmp76.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp6E,env->all_changed,_tmp73,(void*)0);(_tmp76.ReachableFL).f2=
_tmp6F;(_tmp76.ReachableFL).f3=_tmp70;_tmp76;});_tmp75;});}_LL2F:;}_LL2A:;}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,
void*r2){union Cyc_CfFlowInfo_FlowInfo_union _tmp78=flow;struct Cyc_Dict_Dict _tmp79;
struct Cyc_List_List*_tmp7A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp7B;_LL35: if((
_tmp78.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((_tmp78.ReachableFL).tag
!= 1)goto _LL34;_tmp79=(_tmp78.ReachableFL).f1;_tmp7A=(_tmp78.ReachableFL).f2;
_tmp7B=(_tmp78.ReachableFL).f3;_LL38: {void*_tmp7C=r2;struct Cyc_List_List*_tmp7D;
_LL3A: if(_tmp7C <= (void*)3)goto _LL3C;if(*((int*)_tmp7C)!= 3)goto _LL3C;_tmp7D=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7C)->f1;_LL3B: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp7E=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp7F;_LL3F: if((_tmp7E.UnknownL).tag != 1)goto _LL41;_LL40: return flow;_LL41: if((
_tmp7E.PlaceL).tag != 0)goto _LL3E;_tmp7F=(_tmp7E.PlaceL).f1;_LL42: {struct Cyc_List_List*
new_cl;{void*_tmp80=r1;struct Cyc_List_List*_tmp81;void*_tmp82;_LL44: if(_tmp80 <= (
void*)3)goto _LL48;if(*((int*)_tmp80)!= 3)goto _LL46;_tmp81=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp80)->f1;_LL45: new_cl=_tmp81;goto _LL43;_LL46: if(*((int*)_tmp80)!= 0)goto _LL48;
_tmp82=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp80)->f1;if((int)_tmp82
!= 2)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp80 != 0)goto _LL4A;_LL49: goto
_LL4B;_LL4A: if((int)_tmp80 != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D:
return flow;_LL43:;}for(0;_tmp7D != 0;_tmp7D=_tmp7D->tl){void*new_cmp;{struct
_tuple0 _tmp84=({struct _tuple0 _tmp83;_tmp83.f1=p;_tmp83.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp7D->hd)->cmp;_tmp83;});void*_tmp85;void*_tmp86;void*_tmp87;void*_tmp88;_LL4F:
_tmp85=_tmp84.f1;if((int)_tmp85 != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp86=
_tmp84.f2;if((int)_tmp86 != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53:
_tmp87=_tmp84.f1;if((int)_tmp87 != 5)goto _LL55;_tmp88=_tmp84.f2;if((int)_tmp88 != 
5)goto _LL55;_LL54: new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto
_LL4E;_LL4E:;}new_cl=({struct Cyc_List_List*_tmp89=_region_malloc(env->r,sizeof(*
_tmp89));_tmp89->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp8A=_region_malloc(env->r,
sizeof(*_tmp8A));_tmp8A->cmp=(void*)new_cmp;_tmp8A->bd=(void*)((void*)((struct
Cyc_CfFlowInfo_TagCmp*)_tmp7D->hd)->bd);_tmp8A;});_tmp89->tl=new_cl;_tmp89;});}
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp8B;(_tmp8B.ReachableFL).tag=1;(_tmp8B.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp79,env->all_changed,_tmp7F,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp8C=_region_malloc(env->r,sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp8D;_tmp8D.tag=3;_tmp8D.f1=new_cl;_tmp8D;});_tmp8C;}));(_tmp8B.ReachableFL).f2=
_tmp7A;(_tmp8B.ReachableFL).f3=_tmp7B;_tmp8B;});}_LL3E:;}_LL3C:;_LL3D: return flow;
_LL39:;}_LL34:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp90=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp91;void*_tmp92;struct Cyc_Absyn_PtrAtts _tmp93;struct
Cyc_Absyn_Conref*_tmp94;struct Cyc_Absyn_Conref*_tmp95;_LL58: if(_tmp90 <= (void*)4)
goto _LL5A;if(*((int*)_tmp90)!= 4)goto _LL5A;_tmp91=((struct Cyc_Absyn_PointerType_struct*)
_tmp90)->f1;_tmp92=(void*)_tmp91.elt_typ;_tmp93=_tmp91.ptr_atts;_tmp94=_tmp93.bounds;
_tmp95=_tmp93.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp96=f;
struct Cyc_Dict_Dict _tmp97;struct Cyc_List_List*_tmp98;_LL5D: if((_tmp96.BottomFL).tag
!= 0)goto _LL5F;_LL5E: return({struct _tuple5 _tmp99;_tmp99.f1=f;_tmp99.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_all);_tmp99;});_LL5F: if((_tmp96.ReachableFL).tag
!= 1)goto _LL5C;_tmp97=(_tmp96.ReachableFL).f1;_tmp98=(_tmp96.ReachableFL).f2;
_LL60: if(Cyc_Tcutil_is_bound_one(_tmp94)){void*_tmp9A=r;struct Cyc_CfFlowInfo_Place*
_tmp9B;void*_tmp9C;_LL62: if((int)_tmp9A != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmp9A != 2)goto _LL66;_LL65:{void*_tmp9D=(void*)e->annot;struct Cyc_List_List*
_tmp9E;_LL6F: if(*((void**)_tmp9D)!= Cyc_CfFlowInfo_NotZero)goto _LL71;_tmp9E=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp9D)->f1;_LL70: if(!Cyc_CfFlowInfo_same_relns(
_tmp98,_tmp9E))goto _LL72;goto _LL6E;_LL71:;_LL72:{void*_tmp9F=(void*)e->r;_LL74:
if(*((int*)_tmp9F)!= 25)goto _LL76;_LL75:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_NotZero_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmpA1;_tmpA1.tag=Cyc_CfFlowInfo_NotZero;
_tmpA1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp98);_tmpA1;});
_tmpA0;})));goto _LL73;_LL76:;_LL77:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL73;_LL73:;}goto _LL6E;_LL6E:;}goto _LL61;_LL66: if(_tmp9A <= (void*)3)goto
_LL68;if(*((int*)_tmp9A)!= 2)goto _LL68;_tmp9B=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9A)->f1;_LL67:{void*_tmpA2=(void*)e->annot;struct Cyc_List_List*_tmpA3;_LL79:
if(*((void**)_tmpA2)!= Cyc_CfFlowInfo_NotZero)goto _LL7B;_tmpA3=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA2)->f1;_LL7A: if(!Cyc_CfFlowInfo_same_relns(_tmp98,_tmpA3))goto _LL7C;goto
_LL78;_LL7B:;_LL7C:{void*_tmpA4=(void*)e->r;_LL7E: if(*((int*)_tmpA4)!= 25)goto
_LL80;_LL7F:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmpA6;_tmpA6.tag=Cyc_CfFlowInfo_NotZero;_tmpA6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp98);_tmpA6;});_tmpA5;})));goto _LL7D;_LL80:;_LL81:(void*)(e->annot=(void*)((
void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL7D;_LL7D:;}goto _LL78;_LL78:;}
return({struct _tuple5 _tmpA7;_tmpA7.f1=f;_tmpA7.f2=Cyc_CfFlowInfo_lookup_place(
_tmp97,_tmp9B);_tmpA7;});_LL68: if((int)_tmp9A != 0)goto _LL6A;_LL69:(void*)(e->annot=(
void*)((void*)Cyc_CfFlowInfo_IsZero));return({struct _tuple5 _tmpA8;_tmpA8.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpA9;(
_tmpA9.BottomFL).tag=0;_tmpA9;});_tmpA8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmp92,(void*)(env->fenv)->unknown_all);_tmpA8;});_LL6A: if(_tmp9A <= (void*)3)
goto _LL6C;if(*((int*)_tmp9A)!= 0)goto _LL6C;_tmp9C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp9A)->f1;_LL6B: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp9C);goto _LL6D;
_LL6C:;_LL6D:{void*_tmpAA=(void*)e->r;_LL83: if(*((int*)_tmpAA)!= 25)goto _LL85;
_LL84:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpAC;_tmpAC.tag=Cyc_CfFlowInfo_UnknownZ;_tmpAC.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp98);_tmpAC;});_tmpAB;})));goto _LL82;_LL85:;_LL86:(void*)(
e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));goto _LL82;_LL82:;}
goto _LL61;_LL61:;}else{void*_tmpAD=(void*)e->annot;struct Cyc_List_List*_tmpAE;
_LL88: if(*((void**)_tmpAD)!= Cyc_CfFlowInfo_UnknownZ)goto _LL8A;_tmpAE=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmpAD)->f1;_LL89: if(!Cyc_CfFlowInfo_same_relns(
_tmp98,_tmpAE))goto _LL8B;goto _LL87;_LL8A:;_LL8B:{void*_tmpAF=(void*)e->r;_LL8D:
if(*((int*)_tmpAF)!= 25)goto _LL8F;_LL8E:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownZ_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmpB1;_tmpB1.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpB1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp98);_tmpB1;});
_tmpB0;})));goto _LL8C;_LL8F:;_LL90:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL8C;_LL8C:;}goto _LL87;_LL87:;}{void*_tmpB2=Cyc_CfFlowInfo_initlevel(_tmp97,
r);_LL92: if((int)_tmpB2 != 0)goto _LL94;_LL93:({void*_tmpB3[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmpB4="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmpB4,sizeof(char),_get_zero_arr_size(_tmpB4,46));}),
_tag_dynforward(_tmpB3,sizeof(void*),0));});goto _LL95;_LL94: if((int)_tmpB2 != 2)
goto _LL96;_LL95: return({struct _tuple5 _tmpB5;_tmpB5.f1=f;_tmpB5.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_all);_tmpB5;});_LL96: if((int)_tmpB2
!= 1)goto _LL91;_LL97: return({struct _tuple5 _tmpB6;_tmpB6.f1=f;_tmpB6.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp92,(void*)(env->fenv)->unknown_none);_tmpB6;});_LL91:;}_LL5C:;}
_LL5A:;_LL5B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpB7=
_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Core_Impossible_struct _tmpB8;
_tmpB8.tag=Cyc_Core_Impossible;_tmpB8.f1=({const char*_tmpB9="right deref of non-pointer-type";
_tag_dynforward(_tmpB9,sizeof(char),_get_zero_arr_size(_tmpB9,32));});_tmpB8;});
_tmpB7;}));_LL57:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpBA=(void*)e1->r;void*_tmpBB;struct Cyc_Absyn_Vardecl*
_tmpBC;void*_tmpBD;struct Cyc_Absyn_Vardecl*_tmpBE;void*_tmpBF;struct Cyc_Absyn_Vardecl*
_tmpC0;void*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC2;_LL99: if(*((int*)_tmpBA)!= 1)
goto _LL9B;_tmpBB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBB <= (
void*)1)goto _LL9B;if(*((int*)_tmpBB)!= 4)goto _LL9B;_tmpBC=((struct Cyc_Absyn_Pat_b_struct*)
_tmpBB)->f1;_LL9A: _tmpBE=_tmpBC;goto _LL9C;_LL9B: if(*((int*)_tmpBA)!= 1)goto _LL9D;
_tmpBD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBD <= (void*)1)
goto _LL9D;if(*((int*)_tmpBD)!= 3)goto _LL9D;_tmpBE=((struct Cyc_Absyn_Local_b_struct*)
_tmpBD)->f1;_LL9C: _tmpC0=_tmpBE;goto _LL9E;_LL9D: if(*((int*)_tmpBA)!= 1)goto _LL9F;
_tmpBF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBF <= (void*)1)
goto _LL9F;if(*((int*)_tmpBF)!= 2)goto _LL9F;_tmpC0=((struct Cyc_Absyn_Param_b_struct*)
_tmpBF)->f1;_LL9E: _tmpC2=_tmpC0;goto _LLA0;_LL9F: if(*((int*)_tmpBA)!= 1)goto _LLA1;
_tmpC1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpC1 <= (void*)1)
goto _LLA1;if(*((int*)_tmpC1)!= 0)goto _LLA1;_tmpC2=((struct Cyc_Absyn_Global_b_struct*)
_tmpC1)->f1;_LLA0: if(!_tmpC2->escapes){void*_tmpC3=(void*)e2->r;void*_tmpC4;
struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;void*
_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_Absyn_Vardecl*_tmpCB;
_LLA4: if(*((int*)_tmpC3)!= 1)goto _LLA6;_tmpC4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC4 <= (void*)1)goto _LLA6;if(*((int*)_tmpC4)!= 4)goto _LLA6;
_tmpC5=((struct Cyc_Absyn_Pat_b_struct*)_tmpC4)->f1;_LLA5: _tmpC7=_tmpC5;goto _LLA7;
_LLA6: if(*((int*)_tmpC3)!= 1)goto _LLA8;_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC6 <= (void*)1)goto _LLA8;if(*((int*)_tmpC6)!= 3)goto _LLA8;
_tmpC7=((struct Cyc_Absyn_Local_b_struct*)_tmpC6)->f1;_LLA7: _tmpC9=_tmpC7;goto
_LLA9;_LLA8: if(*((int*)_tmpC3)!= 1)goto _LLAA;_tmpC8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC8 <= (void*)1)goto _LLAA;if(*((int*)_tmpC8)!= 2)goto _LLAA;
_tmpC9=((struct Cyc_Absyn_Param_b_struct*)_tmpC8)->f1;_LLA9: _tmpCB=_tmpC9;goto
_LLAB;_LLAA: if(*((int*)_tmpC3)!= 1)goto _LLAC;_tmpCA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpCA <= (void*)1)goto _LLAC;if(*((int*)_tmpCA)!= 0)goto _LLAC;
_tmpCB=((struct Cyc_Absyn_Global_b_struct*)_tmpCA)->f1;_LLAB: if(!_tmpCB->escapes){
int found=0;{struct Cyc_List_List*_tmpCC=relns;for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
struct Cyc_CfFlowInfo_Reln*_tmpCD=(struct Cyc_CfFlowInfo_Reln*)_tmpCC->hd;if(
_tmpCD->vd == _tmpCB){union Cyc_CfFlowInfo_RelnOp_union _tmpCE=_tmpCD->rop;struct
Cyc_Absyn_Vardecl*_tmpCF;_LLAF: if((_tmpCE.LessSize).tag != 2)goto _LLB1;_tmpCF=(
_tmpCE.LessSize).f1;if(!(_tmpCF == _tmpC2))goto _LLB1;_LLB0: return relns;_LLB1:;
_LLB2: continue;_LLAE:;}}}if(!found)return({struct Cyc_List_List*_tmpD0=
_region_malloc(rgn,sizeof(*_tmpD0));_tmpD0->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpD1=_region_malloc(rgn,sizeof(*_tmpD1));_tmpD1->vd=_tmpCB;_tmpD1->rop=(union
Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmpD2;(_tmpD2.LessSize).tag=
2;(_tmpD2.LessSize).f1=_tmpC2;_tmpD2;});_tmpD1;});_tmpD0->tl=relns;_tmpD0;});}
return relns;_LLAC:;_LLAD: return relns;_LLA3:;}else{return relns;}_LLA1:;_LLA2:
return relns;_LL98:;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD3=env->fenv;struct Cyc_List_List*_tmpD4=Cyc_NewControlFlow_noalias_ptrs_rec(
env,place,t);if(ps != 0)*ps=_tmpD4;if(_tmpD4 == 0)return cinfo;cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD3->r,Cyc_CfFlowInfo_place_cmp,_tmpD4,cinfo.may_consume);return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD5=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union _tmpD7;struct _tuple7 _tmpD6=Cyc_NewControlFlow_anal_Lexp(
env,inflow,e);_tmpD7=_tmpD6.f2;{struct _tuple12 _tmpD9=({struct _tuple12 _tmpD8;
_tmpD8.f1=_tmpD7;_tmpD8.f2=inflow;_tmpD8;});union Cyc_CfFlowInfo_AbsLVal_union
_tmpDA;struct Cyc_CfFlowInfo_Place*_tmpDB;union Cyc_CfFlowInfo_FlowInfo_union
_tmpDC;struct Cyc_Dict_Dict _tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_CfFlowInfo_ConsumeInfo
_tmpDF;_LLB4: _tmpDA=_tmpD9.f1;if(((_tmpD9.f1).PlaceL).tag != 0)goto _LLB6;_tmpDB=(
_tmpDA.PlaceL).f1;_tmpDC=_tmpD9.f2;if(((_tmpD9.f2).ReachableFL).tag != 1)goto
_LLB6;_tmpDD=(_tmpDC.ReachableFL).f1;_tmpDE=(_tmpDC.ReachableFL).f2;_tmpDF=(
_tmpDC.ReachableFL).f3;_LLB5: {struct Cyc_CfFlowInfo_ConsumeInfo _tmpE0=Cyc_NewControlFlow_may_consume_place(
env,_tmpDF,_tmpDB,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmpE1;(_tmpE1.ReachableFL).tag=1;(_tmpE1.ReachableFL).f1=_tmpDD;(_tmpE1.ReachableFL).f2=
_tmpDE;(_tmpE1.ReachableFL).f3=_tmpE0;_tmpE1;});}_LLB6:;_LLB7: return inflow;_LLB3:;}}
return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*loc){if(!((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(cinfo.consumed,
p)){struct Cyc_CfFlowInfo_Place _tmpE3;void*_tmpE4;struct Cyc_List_List*_tmpE5;
struct Cyc_CfFlowInfo_Place*_tmpE2=p;_tmpE3=*_tmpE2;_tmpE4=(void*)_tmpE3.root;
_tmpE5=_tmpE3.fields;{void*rval=(void*)0;if((Cyc_Dict_lookup_bool(outdict,_tmpE4,&
rval) && Cyc_CfFlowInfo_initlevel(outdict,rval)!= (void*)0) && rval != (void*)0){
void*_tmpE6=_tmpE4;struct Cyc_Absyn_Vardecl*_tmpE7;_LLB9: if(*((int*)_tmpE6)!= 0)
goto _LLBB;_tmpE7=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpE6)->f1;_LLBA: {
struct _dynforward_ptr _tmpE8=Cyc_Absynpp_qvar2string(_tmpE7->name);if(_tmpE5 == 0)({
struct Cyc_String_pa_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmpE8);{void*_tmpE9[1]={& _tmpEB};Cyc_Tcutil_warn(loc,({
const char*_tmpEA="may clobber unique pointer %s";_tag_dynforward(_tmpEA,sizeof(
char),_get_zero_arr_size(_tmpEA,30));}),_tag_dynforward(_tmpE9,sizeof(void*),1));}});
else{({struct Cyc_String_pa_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpE8);{void*_tmpEC[1]={& _tmpEE};Cyc_Tcutil_warn(
loc,({const char*_tmpED="may clobber unique pointer contained in %s";
_tag_dynforward(_tmpED,sizeof(char),_get_zero_arr_size(_tmpED,43));}),
_tag_dynforward(_tmpEC,sizeof(void*),1));}});}goto _LLB8;}_LLBB:;_LLBC:({void*
_tmpEF[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpF0="warning locations not for VarRoots";_tag_dynforward(_tmpF0,
sizeof(char),_get_zero_arr_size(_tmpF0,35));}),_tag_dynforward(_tmpEF,sizeof(
void*),0));});_LLB8:;}}}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpF1=env->fenv;struct Cyc_List_List*_tmpF2=0;cinfo=Cyc_NewControlFlow_may_consume_place(
env,cinfo,p,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)&
_tmpF2);while(_tmpF2 != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmpF2))->hd,cinfo,outdict,e->loc);_tmpF2=((
struct Cyc_List_List*)_check_null(_tmpF2))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmpF4;void*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_CfFlowInfo_Place*_tmpF3=
p;_tmpF4=*_tmpF3;_tmpF5=(void*)_tmpF4.root;_tmpF6=_tmpF4.fields;{int fld=0;for(0;
ts != 0;(fld ++,ts=ts->tl)){void*_tmpF7=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmpF7)){struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpF6,({int _tmpFB[1];_tmpFB[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(env->r,_tag_dynforward(
_tmpFB,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmpF9=_region_malloc(env->r,sizeof(*_tmpF9));_tmpF9->hd=({
struct Cyc_CfFlowInfo_Place*_tmpFA=_region_malloc(env->r,sizeof(*_tmpFA));_tmpFA->root=(
void*)_tmpF5;_tmpFA->fields=_tmpF8;_tmpFA;});_tmpF9->tl=0;_tmpF9;}),l);}else{if(
Cyc_Absyn_is_aggr_type(_tmpF7)){struct Cyc_List_List*_tmpFC=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpF6,({int _tmpFF[1];_tmpFF[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(env->r,_tag_dynforward(
_tmpFF,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*_tmpFD=({struct Cyc_CfFlowInfo_Place*
_tmpFE=_region_malloc(env->r,sizeof(*_tmpFE));_tmpFE->root=(void*)_tmpF5;_tmpFE->fields=
_tmpFC;_tmpFE;});l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,
struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,l,Cyc_NewControlFlow_noalias_ptrs_rec(
env,_tmpFD,_tmpF7));}}}}return l;}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;
};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t))
return({struct Cyc_List_List*_tmp100=_region_malloc(env->r,sizeof(*_tmp100));
_tmp100->hd=p;_tmp100->tl=0;_tmp100;});{void*_tmp101=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp102;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_AggrInfo
_tmp104;union Cyc_Absyn_AggrInfoU_union _tmp105;struct Cyc_Absyn_Aggrdecl**_tmp106;
struct Cyc_List_List*_tmp107;struct Cyc_Absyn_AggrInfo _tmp108;union Cyc_Absyn_AggrInfoU_union
_tmp109;_LLBE: if(_tmp101 <= (void*)4)goto _LLC6;if(*((int*)_tmp101)!= 9)goto _LLC0;
_tmp102=((struct Cyc_Absyn_TupleType_struct*)_tmp101)->f1;_LLBF: {struct Cyc_List_List*
_tmp10A=0;while(_tmp102 != 0){_tmp10A=({struct Cyc_List_List*_tmp10B=
_region_malloc(env->r,sizeof(*_tmp10B));_tmp10B->hd=(void*)(*((struct _tuple13*)
_tmp102->hd)).f2;_tmp10B->tl=_tmp10A;_tmp10B;});_tmp102=_tmp102->tl;}_tmp10A=Cyc_List_imp_rev(
_tmp10A);return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp10A);}_LLC0: if(*((
int*)_tmp101)!= 11)goto _LLC2;_tmp103=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp101)->f2;_LLC1: {struct Cyc_List_List*_tmp10C=0;while(_tmp103 != 0){_tmp10C=({
struct Cyc_List_List*_tmp10D=_region_malloc(env->r,sizeof(*_tmp10D));_tmp10D->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp103->hd)->type);_tmp10D->tl=
_tmp10C;_tmp10D;});_tmp103=_tmp103->tl;}_tmp10C=Cyc_List_imp_rev(_tmp10C);return
Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp10C);}_LLC2: if(*((int*)_tmp101)!= 
10)goto _LLC4;_tmp104=((struct Cyc_Absyn_AggrType_struct*)_tmp101)->f1;_tmp105=
_tmp104.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp101)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLC4;_tmp106=(_tmp105.KnownAggr).f1;_tmp107=_tmp104.targs;_LLC3: if((*
_tmp106)->impl == 0)return 0;else{struct Cyc_List_List*_tmp10E=0;{struct
_RegionHandle _tmp10F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp10F;
_push_region(rgn);{struct Cyc_List_List*_tmp110=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmp106)->tvs,_tmp107);struct Cyc_List_List*_tmp111=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp106)->impl))->fields;while(
_tmp111 != 0){_tmp10E=({struct Cyc_List_List*_tmp112=_region_malloc(env->r,sizeof(*
_tmp112));_tmp112->hd=(void*)Cyc_Tcutil_rsubstitute(rgn,_tmp110,(void*)((struct
Cyc_Absyn_Aggrfield*)_tmp111->hd)->type);_tmp112->tl=_tmp10E;_tmp112;});_tmp111=
_tmp111->tl;}};_pop_region(rgn);}_tmp10E=Cyc_List_imp_rev(_tmp10E);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp10E);}_LLC4: if(*((int*)_tmp101)!= 10)goto _LLC6;_tmp108=((struct Cyc_Absyn_AggrType_struct*)
_tmp101)->f1;_tmp109=_tmp108.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp101)->f1).aggr_info).UnknownAggr).tag != 0)goto _LLC6;_LLC5:({void*_tmp113[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp114="got unknown aggr in is_noalias_aggr";_tag_dynforward(_tmp114,
sizeof(char),_get_zero_arr_size(_tmp114,36));}),_tag_dynforward(_tmp113,sizeof(
void*),0));});_LLC6:;_LLC7: return 0;_LLBD:;}}struct _tuple14{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp115=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp116=inflow;struct Cyc_Dict_Dict _tmp117;struct Cyc_List_List*_tmp118;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp119;_LLC9: if((_tmp116.BottomFL).tag != 0)goto _LLCB;_LLCA: return({struct
_tuple5 _tmp11A;_tmp11A.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp11B;(_tmp11B.BottomFL).tag=0;_tmp11B;});_tmp11A.f2=(void*)_tmp115->unknown_all;
_tmp11A;});_LLCB: if((_tmp116.ReachableFL).tag != 1)goto _LLC8;_tmp117=(_tmp116.ReachableFL).f1;
_tmp118=(_tmp116.ReachableFL).f2;_tmp119=(_tmp116.ReachableFL).f3;_LLCC: d=
_tmp117;relns=_tmp118;cinfo=_tmp119;_LLC8:;}{void*_tmp11C=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp11D;void*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;
struct Cyc_Absyn_Exp*_tmp121;union Cyc_Absyn_Cnst_union _tmp122;union Cyc_Absyn_Cnst_union
_tmp123;int _tmp124;union Cyc_Absyn_Cnst_union _tmp125;void*_tmp126;struct Cyc_List_List*
_tmp127;void*_tmp128;void*_tmp129;struct Cyc_Absyn_Vardecl*_tmp12A;void*_tmp12B;
struct Cyc_Absyn_Vardecl*_tmp12C;void*_tmp12D;struct Cyc_Absyn_Vardecl*_tmp12E;
void*_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*
_tmp132;struct Cyc_Core_Opt*_tmp133;struct Cyc_Core_Opt _tmp134;struct Cyc_Absyn_Exp*
_tmp135;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Core_Opt*_tmp137;struct Cyc_Absyn_Exp*
_tmp138;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*
_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_MallocInfo
_tmp13E;int _tmp13F;struct Cyc_Absyn_Exp*_tmp140;void**_tmp141;struct Cyc_Absyn_Exp*
_tmp142;int _tmp143;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp145;
struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*
_tmp148;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14A;struct
_dynforward_ptr*_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;struct _dynforward_ptr*
_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_Exp*
_tmp150;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_Absyn_Exp*
_tmp153;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*
_tmp156;struct Cyc_List_List*_tmp157;struct Cyc_Absyn_Tuniondecl*_tmp158;struct Cyc_List_List*
_tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_Absyn_Aggrdecl*
_tmp15C;struct Cyc_Absyn_Aggrdecl _tmp15D;struct Cyc_Absyn_AggrdeclImpl*_tmp15E;
struct Cyc_Absyn_AggrdeclImpl _tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List*
_tmp161;struct Cyc_Absyn_Vardecl*_tmp162;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*
_tmp164;int _tmp165;struct Cyc_Absyn_Stmt*_tmp166;void*_tmp167;_LLCE: if(*((int*)
_tmp11C)!= 15)goto _LLD0;_tmp11D=((struct Cyc_Absyn_Cast_e_struct*)_tmp11C)->f2;
_tmp11E=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp11C)->f4;if((int)_tmp11E != 2)
goto _LLD0;_LLCF: {union Cyc_CfFlowInfo_FlowInfo_union _tmp169;void*_tmp16A;struct
_tuple5 _tmp168=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp11D);_tmp169=_tmp168.f1;
_tmp16A=_tmp168.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp16C;void*_tmp16D;
struct _tuple5 _tmp16B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp169,_tmp11D,
_tmp16A);_tmp16C=_tmp16B.f1;_tmp16D=_tmp16B.f2;return({struct _tuple5 _tmp16E;
_tmp16E.f1=_tmp16C;_tmp16E.f2=_tmp16A;_tmp16E;});}}_LLD0: if(*((int*)_tmp11C)!= 
15)goto _LLD2;_tmp11F=((struct Cyc_Absyn_Cast_e_struct*)_tmp11C)->f2;_LLD1: _tmp120=
_tmp11F;goto _LLD3;_LLD2: if(*((int*)_tmp11C)!= 13)goto _LLD4;_tmp120=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp11C)->f1;_LLD3: _tmp121=_tmp120;goto _LLD5;_LLD4: if(*((int*)_tmp11C)!= 14)goto
_LLD6;_tmp121=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp11C)->f1;_LLD5: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp121);_LLD6: if(*((int*)_tmp11C)!= 0)
goto _LLD8;_tmp122=((struct Cyc_Absyn_Const_e_struct*)_tmp11C)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp11C)->f1).Null_c).tag != 6)goto _LLD8;_LLD7: goto
_LLD9;_LLD8: if(*((int*)_tmp11C)!= 0)goto _LLDA;_tmp123=((struct Cyc_Absyn_Const_e_struct*)
_tmp11C)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp11C)->f1).Int_c).tag != 2)
goto _LLDA;_tmp124=(_tmp123.Int_c).f2;if(_tmp124 != 0)goto _LLDA;_LLD9: return({
struct _tuple5 _tmp16F;_tmp16F.f1=inflow;_tmp16F.f2=(void*)0;_tmp16F;});_LLDA: if(*((
int*)_tmp11C)!= 0)goto _LLDC;_tmp125=((struct Cyc_Absyn_Const_e_struct*)_tmp11C)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp11C)->f1).Int_c).tag != 2)goto _LLDC;
_LLDB: goto _LLDD;_LLDC: if(*((int*)_tmp11C)!= 1)goto _LLDE;_tmp126=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp11C)->f2;if(_tmp126 <= (void*)1)goto _LLDE;if(*((int*)
_tmp126)!= 1)goto _LLDE;_LLDD: return({struct _tuple5 _tmp170;_tmp170.f1=inflow;
_tmp170.f2=(void*)1;_tmp170;});_LLDE: if(*((int*)_tmp11C)!= 32)goto _LLE0;_tmp127=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp11C)->f1;if(_tmp127 != 0)goto _LLE0;_LLDF:
goto _LLE1;_LLE0: if(*((int*)_tmp11C)!= 0)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(*((
int*)_tmp11C)!= 19)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(*((int*)_tmp11C)!= 18)goto
_LLE6;_LLE5: goto _LLE7;_LLE6: if(*((int*)_tmp11C)!= 20)goto _LLE8;_LLE7: goto _LLE9;
_LLE8: if(*((int*)_tmp11C)!= 21)goto _LLEA;_LLE9: goto _LLEB;_LLEA: if(*((int*)
_tmp11C)!= 34)goto _LLEC;_LLEB: goto _LLED;_LLEC: if(*((int*)_tmp11C)!= 33)goto _LLEE;
_LLED: return({struct _tuple5 _tmp171;_tmp171.f1=inflow;_tmp171.f2=(void*)_tmp115->unknown_all;
_tmp171;});_LLEE: if(*((int*)_tmp11C)!= 1)goto _LLF0;_tmp128=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp11C)->f2;if(_tmp128 <= (void*)1)goto _LLF0;if(*((int*)_tmp128)!= 0)goto _LLF0;
_LLEF: return({struct _tuple5 _tmp172;_tmp172.f1=inflow;_tmp172.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp115->unknown_all);
_tmp172;});_LLF0: if(*((int*)_tmp11C)!= 1)goto _LLF2;_tmp129=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp11C)->f2;if(_tmp129 <= (void*)1)goto _LLF2;if(*((int*)_tmp129)!= 2)goto _LLF2;
_tmp12A=((struct Cyc_Absyn_Param_b_struct*)_tmp129)->f1;_LLF1: _tmp12C=_tmp12A;
goto _LLF3;_LLF2: if(*((int*)_tmp11C)!= 1)goto _LLF4;_tmp12B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp11C)->f2;if(_tmp12B <= (void*)1)goto _LLF4;if(*((int*)_tmp12B)!= 3)goto _LLF4;
_tmp12C=((struct Cyc_Absyn_Local_b_struct*)_tmp12B)->f1;_LLF3: _tmp12E=_tmp12C;
goto _LLF5;_LLF4: if(*((int*)_tmp11C)!= 1)goto _LLF6;_tmp12D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp11C)->f2;if(_tmp12D <= (void*)1)goto _LLF6;if(*((int*)_tmp12D)!= 4)goto _LLF6;
_tmp12E=((struct Cyc_Absyn_Pat_b_struct*)_tmp12D)->f1;_LLF5: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);return({struct _tuple5 _tmp173;_tmp173.f1=inflow;_tmp173.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp174=_region_malloc(env->r,
sizeof(*_tmp174));_tmp174[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp175;
_tmp175.tag=0;_tmp175.f1=_tmp12E;_tmp175;});_tmp174;}));_tmp173;});_LLF6: if(*((
int*)_tmp11C)!= 3)goto _LLF8;_tmp12F=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp11C)->f1;_tmp130=((struct Cyc_Absyn_Primop_e_struct*)_tmp11C)->f2;_LLF7: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp177;void*_tmp178;struct _tuple5 _tmp176=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp130,0);_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;{void*_tmp179=
_tmp12F;_LL135: if((int)_tmp179 != 0)goto _LL137;_LL136: goto _LL138;_LL137: if((int)
_tmp179 != 2)goto _LL139;_LL138: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp130))->hd)->loc,_tmp177);goto _LL134;_LL139:;
_LL13A: _tmp177=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp130))->hd)->loc,_tmp177);goto _LL134;_LL134:;}
return({struct _tuple5 _tmp17A;_tmp17A.f1=_tmp177;_tmp17A.f2=_tmp178;_tmp17A;});}
_LLF8: if(*((int*)_tmp11C)!= 5)goto _LLFA;_tmp131=((struct Cyc_Absyn_Increment_e_struct*)
_tmp11C)->f1;_LLF9: {struct Cyc_List_List _tmp17B=({struct Cyc_List_List _tmp18A;
_tmp18A.hd=_tmp131;_tmp18A.tl=0;_tmp18A;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp17D;struct _tuple5 _tmp17C=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp17B,0);_tmp17D=_tmp17C.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp131->loc,_tmp17D);{union Cyc_CfFlowInfo_AbsLVal_union _tmp17F;struct _tuple7
_tmp17E=Cyc_NewControlFlow_anal_Lexp(env,_tmp17D,_tmp131);_tmp17F=_tmp17E.f2;{
struct _tuple12 _tmp181=({struct _tuple12 _tmp180;_tmp180.f1=_tmp17F;_tmp180.f2=
_tmp17D;_tmp180;});union Cyc_CfFlowInfo_AbsLVal_union _tmp182;struct Cyc_CfFlowInfo_Place*
_tmp183;union Cyc_CfFlowInfo_FlowInfo_union _tmp184;struct Cyc_Dict_Dict _tmp185;
struct Cyc_List_List*_tmp186;struct Cyc_CfFlowInfo_ConsumeInfo _tmp187;_LL13C:
_tmp182=_tmp181.f1;if(((_tmp181.f1).PlaceL).tag != 0)goto _LL13E;_tmp183=(_tmp182.PlaceL).f1;
_tmp184=_tmp181.f2;if(((_tmp181.f2).ReachableFL).tag != 1)goto _LL13E;_tmp185=(
_tmp184.ReachableFL).f1;_tmp186=(_tmp184.ReachableFL).f2;_tmp187=(_tmp184.ReachableFL).f3;
_LL13D: _tmp186=Cyc_CfFlowInfo_reln_kill_exp(_tmp115->r,_tmp186,_tmp131);_tmp17D=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp188;(
_tmp188.ReachableFL).tag=1;(_tmp188.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp115,_tmp131->loc,_tmp185,env->all_changed,_tmp183,(void*)_tmp115->unknown_all);(
_tmp188.ReachableFL).f2=_tmp186;(_tmp188.ReachableFL).f3=_tmp187;_tmp188;});goto
_LL13B;_LL13E:;_LL13F: goto _LL13B;_LL13B:;}return({struct _tuple5 _tmp189;_tmp189.f1=
_tmp17D;_tmp189.f2=(void*)_tmp115->unknown_all;_tmp189;});}}_LLFA: if(*((int*)
_tmp11C)!= 4)goto _LLFC;_tmp132=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11C)->f1;
_tmp133=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11C)->f2;if(_tmp133 == 0)goto
_LLFC;_tmp134=*_tmp133;_tmp135=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11C)->f3;
_LLFB: {struct Cyc_List_List _tmp18B=({struct Cyc_List_List _tmp19A;_tmp19A.hd=
_tmp135;_tmp19A.tl=0;_tmp19A;});struct Cyc_List_List _tmp18C=({struct Cyc_List_List
_tmp199;_tmp199.hd=_tmp132;_tmp199.tl=(struct Cyc_List_List*)& _tmp18B;_tmp199;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp18E;struct _tuple5 _tmp18D=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp18C,1);_tmp18E=_tmp18D.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp190;struct _tuple7 _tmp18F=Cyc_NewControlFlow_anal_Lexp(env,_tmp18E,e);_tmp190=
_tmp18F.f2;_tmp18E=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp18E);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp191=_tmp18E;struct Cyc_Dict_Dict _tmp192;struct Cyc_List_List*_tmp193;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp194;_LL141: if((_tmp191.ReachableFL).tag != 1)goto _LL143;_tmp192=(_tmp191.ReachableFL).f1;
_tmp193=(_tmp191.ReachableFL).f2;_tmp194=(_tmp191.ReachableFL).f3;_LL142:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp195=_tmp190;struct Cyc_CfFlowInfo_Place*_tmp196;
_LL146: if((_tmp195.PlaceL).tag != 0)goto _LL148;_tmp196=(_tmp195.PlaceL).f1;_LL147:
_tmp194=Cyc_NewControlFlow_consume_assignment(env,_tmp196,_tmp194,_tmp192,
_tmp132);_tmp193=Cyc_CfFlowInfo_reln_kill_exp(_tmp115->r,_tmp193,_tmp132);
_tmp192=Cyc_CfFlowInfo_assign_place(_tmp115,_tmp132->loc,_tmp192,env->all_changed,
_tmp196,(void*)_tmp115->unknown_all);_tmp18E=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp197;(_tmp197.ReachableFL).tag=1;(_tmp197.ReachableFL).f1=
_tmp192;(_tmp197.ReachableFL).f2=_tmp193;(_tmp197.ReachableFL).f3=_tmp194;
_tmp197;});goto _LL145;_LL148: if((_tmp195.UnknownL).tag != 1)goto _LL145;_LL149:
goto _LL145;_LL145:;}goto _LL140;_LL143:;_LL144: goto _LL140;_LL140:;}return({struct
_tuple5 _tmp198;_tmp198.f1=_tmp18E;_tmp198.f2=(void*)_tmp115->unknown_all;_tmp198;});}}
_LLFC: if(*((int*)_tmp11C)!= 4)goto _LLFE;_tmp136=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp11C)->f1;_tmp137=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11C)->f2;if(
_tmp137 != 0)goto _LLFE;_tmp138=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11C)->f3;
_LLFD: {struct Cyc_Dict_Dict*_tmp19B=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp19D;union Cyc_CfFlowInfo_FlowInfo_union _tmp19E;struct _tuple6 _tmp19C=Cyc_CfFlowInfo_save_consume_info(
_tmp115,inflow,1);_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B8;_tmp1B8.consumed=_tmp115->mt_place_set;
_tmp1B8.may_consume=0;_tmp1B8;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp19F=_region_malloc(env->r,sizeof(*_tmp19F));_tmp19F[0]=_tmp115->mt_place_set;
_tmp19F;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp1A1;union Cyc_CfFlowInfo_AbsLVal_union
_tmp1A2;struct _tuple7 _tmp1A0=Cyc_NewControlFlow_anal_Lexp(env,_tmp19E,_tmp136);
_tmp1A1=_tmp1A0.f1;_tmp1A2=_tmp1A0.f2;{struct Cyc_Dict_Dict _tmp1A3=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp1A4=
_region_malloc(env->r,sizeof(*_tmp1A4));_tmp1A4[0]=_tmp115->mt_place_set;_tmp1A4;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1A6;void*_tmp1A7;struct _tuple5 _tmp1A5=Cyc_NewControlFlow_anal_Rexp(
env,_tmp19E,_tmp138);_tmp1A6=_tmp1A5.f1;_tmp1A7=_tmp1A5.f2;{struct Cyc_Dict_Dict
_tmp1A8=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A9=Cyc_CfFlowInfo_after_flow(_tmp115,(struct Cyc_Dict_Dict*)& _tmp1A3,_tmp1A1,
_tmp1A6,_tmp1A3,_tmp1A8);union Cyc_CfFlowInfo_FlowInfo_union _tmp1AA=Cyc_CfFlowInfo_join_flow(
_tmp115,_tmp19B,_tmp19E,_tmp1A9,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1AC;
struct _tuple6 _tmp1AB=Cyc_CfFlowInfo_save_consume_info(_tmp115,_tmp1AA,0);_tmp1AC=
_tmp1AB.f1;if(init_consume)_tmp19E=Cyc_CfFlowInfo_restore_consume_info(_tmp19E,
outflow_consume);outflow_consume=_tmp1AC;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp1AA,_tmp19E)){if(_tmp19B == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp19B,Cyc_CfFlowInfo_union_place_set(
_tmp1A3,_tmp1A8,0),0);}_tmp1A9=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1A9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1AD=_tmp1A9;struct Cyc_Dict_Dict
_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B0;
_LL14B: if((_tmp1AD.BottomFL).tag != 0)goto _LL14D;_LL14C: return({struct _tuple5
_tmp1B1;_tmp1B1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B2;(_tmp1B2.BottomFL).tag=0;_tmp1B2;});_tmp1B1.f2=_tmp1A7;_tmp1B1;});_LL14D:
if((_tmp1AD.ReachableFL).tag != 1)goto _LL14A;_tmp1AE=(_tmp1AD.ReachableFL).f1;
_tmp1AF=(_tmp1AD.ReachableFL).f2;_tmp1B0=(_tmp1AD.ReachableFL).f3;_LL14E: _tmp1B0=
Cyc_CfFlowInfo_and_consume(_tmp115->r,_tmp19D,_tmp1B0);{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B3=_tmp1A2;struct Cyc_CfFlowInfo_Place*_tmp1B4;_LL150: if((_tmp1B3.PlaceL).tag
!= 0)goto _LL152;_tmp1B4=(_tmp1B3.PlaceL).f1;_LL151: _tmp1B0=Cyc_NewControlFlow_consume_assignment(
env,_tmp1B4,_tmp1B0,_tmp1AE,_tmp136);_tmp1AE=Cyc_CfFlowInfo_assign_place(_tmp115,
e->loc,_tmp1AE,env->all_changed,_tmp1B4,_tmp1A7);_tmp1AF=Cyc_CfFlowInfo_reln_assign_exp(
_tmp115->r,_tmp1AF,_tmp136,_tmp138);_tmp1A9=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B5;(_tmp1B5.ReachableFL).tag=1;(_tmp1B5.ReachableFL).f1=
_tmp1AE;(_tmp1B5.ReachableFL).f2=_tmp1AF;(_tmp1B5.ReachableFL).f3=_tmp1B0;
_tmp1B5;});Cyc_NewControlFlow_update_tryflow(env,_tmp1A9);return({struct _tuple5
_tmp1B6;_tmp1B6.f1=_tmp1A9;_tmp1B6.f2=_tmp1A7;_tmp1B6;});_LL152: if((_tmp1B3.UnknownL).tag
!= 1)goto _LL14F;_LL153: return({struct _tuple5 _tmp1B7;_tmp1B7.f1=Cyc_NewControlFlow_use_Rval(
env,_tmp138->loc,_tmp1A9,_tmp1A7);_tmp1B7.f2=_tmp1A7;_tmp1B7;});_LL14F:;}_LL14A:;}}
_tmp19E=Cyc_CfFlowInfo_restore_consume_info(_tmp1AA,empty_consume);}}}}}}}_LLFE:
if(*((int*)_tmp11C)!= 9)goto _LL100;_tmp139=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp11C)->f1;_tmp13A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp11C)->f2;_LLFF: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp1BA;void*_tmp1BB;struct _tuple5 _tmp1B9=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp139);_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;_tmp1BA=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp139->loc,_tmp1BA);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1BA,_tmp13A);}
_LL100: if(*((int*)_tmp11C)!= 12)goto _LL102;_tmp13B=((struct Cyc_Absyn_Throw_e_struct*)
_tmp11C)->f1;_LL101: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1BD;void*_tmp1BE;
struct _tuple5 _tmp1BC=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp13B);_tmp1BD=
_tmp1BC.f1;_tmp1BE=_tmp1BC.f2;_tmp1BD=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp13B->loc,_tmp1BD);Cyc_NewControlFlow_use_Rval(env,_tmp13B->loc,_tmp1BD,
_tmp1BE);return({struct _tuple5 _tmp1BF;_tmp1BF.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C0;(_tmp1C0.BottomFL).tag=0;_tmp1C0;});
_tmp1BF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp115,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)_tmp115->unknown_all);_tmp1BF;});}_LL102: if(*((
int*)_tmp11C)!= 11)goto _LL104;_tmp13C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp11C)->f1;
_tmp13D=((struct Cyc_Absyn_FnCall_e_struct*)_tmp11C)->f2;_LL103: {struct
_RegionHandle _tmp1C1=_new_region("temp");struct _RegionHandle*temp=& _tmp1C1;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C4;struct Cyc_List_List*
_tmp1C5;struct _tuple11 _tmp1C3=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp1C2=_region_malloc(temp,sizeof(*_tmp1C2));
_tmp1C2->hd=_tmp13C;_tmp1C2->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp13D);_tmp1C2;}),1);_tmp1C4=
_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;_tmp1C4=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1C4);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C6=Cyc_NewControlFlow_use_Rval(
env,_tmp13C->loc,_tmp1C4,(void*)((struct Cyc_List_List*)_check_null(_tmp1C5))->hd);
_tmp1C5=_tmp1C5->tl;{struct Cyc_List_List*init_params=0;{void*_tmp1C7=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp13C->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1C8;void*_tmp1C9;_LL155: if(_tmp1C7 <= (void*)4)goto _LL157;if(*((int*)_tmp1C7)
!= 4)goto _LL157;_tmp1C8=((struct Cyc_Absyn_PointerType_struct*)_tmp1C7)->f1;
_tmp1C9=(void*)_tmp1C8.elt_typ;_LL156:{void*_tmp1CA=Cyc_Tcutil_compress(_tmp1C9);
struct Cyc_Absyn_FnInfo _tmp1CB;struct Cyc_List_List*_tmp1CC;_LL15A: if(_tmp1CA <= (
void*)4)goto _LL15C;if(*((int*)_tmp1CA)!= 8)goto _LL15C;_tmp1CB=((struct Cyc_Absyn_FnType_struct*)
_tmp1CA)->f1;_tmp1CC=_tmp1CB.attributes;_LL15B: for(0;_tmp1CC != 0;_tmp1CC=_tmp1CC->tl){
void*_tmp1CD=(void*)_tmp1CC->hd;int _tmp1CE;_LL15F: if(_tmp1CD <= (void*)17)goto
_LL161;if(*((int*)_tmp1CD)!= 4)goto _LL161;_tmp1CE=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1CD)->f1;_LL160: init_params=({struct Cyc_List_List*_tmp1CF=_region_malloc(
temp,sizeof(*_tmp1CF));_tmp1CF->hd=(void*)_tmp1CE;_tmp1CF->tl=init_params;
_tmp1CF;});goto _LL15E;_LL161:;_LL162: goto _LL15E;_LL15E:;}goto _LL159;_LL15C:;
_LL15D:({void*_tmp1D0[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1D1="anal_Rexp: bad function type";
_tag_dynforward(_tmp1D1,sizeof(char),_get_zero_arr_size(_tmp1D1,29));}),
_tag_dynforward(_tmp1D0,sizeof(void*),0));});_LL159:;}goto _LL154;_LL157:;_LL158:({
void*_tmp1D2[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1D3="anal_Rexp: bad function type";
_tag_dynforward(_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,29));}),
_tag_dynforward(_tmp1D2,sizeof(void*),0));});_LL154:;}{int i=1;for(0;_tmp1C5 != 0;(((
_tmp1C5=_tmp1C5->tl,_tmp13D=((struct Cyc_List_List*)_check_null(_tmp13D))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
_tmp1C6=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp13D))->hd)->loc,_tmp1C6,(void*)_tmp1C5->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D4=_tmp1C4;struct Cyc_Dict_Dict _tmp1D5;_LL164: if((_tmp1D4.BottomFL).tag != 0)
goto _LL166;_LL165: goto _LL163;_LL166: if((_tmp1D4.ReachableFL).tag != 1)goto _LL163;
_tmp1D5=(_tmp1D4.ReachableFL).f1;_LL167: if(Cyc_CfFlowInfo_initlevel(_tmp1D5,(
void*)_tmp1C5->hd)== (void*)0)({void*_tmp1D6[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp13D))->hd)->loc,({const char*_tmp1D7="expression may not be initialized";
_tag_dynforward(_tmp1D7,sizeof(char),_get_zero_arr_size(_tmp1D7,34));}),
_tag_dynforward(_tmp1D6,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D8=_tmp1C6;struct Cyc_Dict_Dict _tmp1D9;struct Cyc_List_List*_tmp1DA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1DB;_LL169: if((_tmp1D8.BottomFL).tag != 0)goto _LL16B;_LL16A: goto _LL168;_LL16B:
if((_tmp1D8.ReachableFL).tag != 1)goto _LL168;_tmp1D9=(_tmp1D8.ReachableFL).f1;
_tmp1DA=(_tmp1D8.ReachableFL).f2;_tmp1DB=(_tmp1D8.ReachableFL).f3;_LL16C: {
struct Cyc_Dict_Dict _tmp1DC=Cyc_CfFlowInfo_escape_deref(_tmp115,_tmp1D9,env->all_changed,(
void*)_tmp1C5->hd);{void*_tmp1DD=(void*)_tmp1C5->hd;struct Cyc_CfFlowInfo_Place*
_tmp1DE;_LL16E: if(_tmp1DD <= (void*)3)goto _LL170;if(*((int*)_tmp1DD)!= 2)goto
_LL170;_tmp1DE=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DD)->f1;_LL16F:{
void*_tmp1DF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp13D))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1E0;void*_tmp1E1;_LL173: if(_tmp1DF <= (void*)4)goto
_LL175;if(*((int*)_tmp1DF)!= 4)goto _LL175;_tmp1E0=((struct Cyc_Absyn_PointerType_struct*)
_tmp1DF)->f1;_tmp1E1=(void*)_tmp1E0.elt_typ;_LL174: _tmp1DC=Cyc_CfFlowInfo_assign_place(
_tmp115,((struct Cyc_Absyn_Exp*)_tmp13D->hd)->loc,_tmp1DC,env->all_changed,
_tmp1DE,Cyc_CfFlowInfo_typ_to_absrval(_tmp115,_tmp1E1,(void*)_tmp115->esc_all));
goto _LL172;_LL175:;_LL176:({void*_tmp1E2[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1E3="anal_Rexp:bad type for initialized arg";
_tag_dynforward(_tmp1E3,sizeof(char),_get_zero_arr_size(_tmp1E3,39));}),
_tag_dynforward(_tmp1E2,sizeof(void*),0));});_LL172:;}goto _LL16D;_LL170:;_LL171:
goto _LL16D;_LL16D:;}_tmp1C6=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E4;(_tmp1E4.ReachableFL).tag=1;(_tmp1E4.ReachableFL).f1=_tmp1DC;(_tmp1E4.ReachableFL).f2=
_tmp1DA;(_tmp1E4.ReachableFL).f3=_tmp1DB;_tmp1E4;});goto _LL168;}_LL168:;}goto
_LL163;_LL163:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp13C->topt))->v)){struct _tuple5 _tmp1E7=({struct _tuple5 _tmp1E5;
_tmp1E5.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E6;(_tmp1E6.BottomFL).tag=0;_tmp1E6;});_tmp1E5.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp115->unknown_all);
_tmp1E5;});_npop_handler(0);return _tmp1E7;}else{struct _tuple5 _tmp1E9=({struct
_tuple5 _tmp1E8;_tmp1E8.f1=_tmp1C6;_tmp1E8.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp115->unknown_all);
_tmp1E8;});_npop_handler(0);return _tmp1E9;}}}};_pop_region(temp);}_LL104: if(*((
int*)_tmp11C)!= 35)goto _LL106;_tmp13E=((struct Cyc_Absyn_Malloc_e_struct*)_tmp11C)->f1;
_tmp13F=_tmp13E.is_calloc;_tmp140=_tmp13E.rgn;_tmp141=_tmp13E.elt_type;_tmp142=
_tmp13E.num_elts;_tmp143=_tmp13E.fat_result;_LL105: {void*root=(void*)({struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp1FB=_region_malloc(_tmp115->r,sizeof(*_tmp1FB));
_tmp1FB[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp1FC;_tmp1FC.tag=1;_tmp1FC.f1=
_tmp142;_tmp1FC.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp1FC;});_tmp1FB;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp1FA=_region_malloc(_tmp115->r,sizeof(*_tmp1FA));_tmp1FA->root=(void*)root;
_tmp1FA->fields=0;_tmp1FA;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F8=_region_malloc(_tmp115->r,sizeof(*_tmp1F8));_tmp1F8[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1F9;_tmp1F9.tag=2;_tmp1F9.f1=place;_tmp1F9;});_tmp1F8;});void*place_val=
_tmp143?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(_tmp115,*((void**)_check_null(
_tmp141)),(void*)_tmp115->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmp140 != 0){
struct _RegionHandle _tmp1EA=_new_region("temp");struct _RegionHandle*temp=& _tmp1EA;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1ED;struct Cyc_List_List*
_tmp1EE;struct _tuple11 _tmp1EC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1EB[2];_tmp1EB[1]=_tmp142;_tmp1EB[0]=(struct Cyc_Absyn_Exp*)
_tmp140;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp1EB,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp1ED=_tmp1EC.f1;_tmp1EE=_tmp1EC.f2;outflow=_tmp1ED;};_pop_region(temp);}
else{union Cyc_CfFlowInfo_FlowInfo_union _tmp1F0;struct _tuple5 _tmp1EF=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp142);_tmp1F0=_tmp1EF.f1;outflow=_tmp1F0;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp142->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1F1=outflow;struct
Cyc_Dict_Dict _tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1F4;_LL178: if((_tmp1F1.BottomFL).tag != 0)goto _LL17A;_LL179: return({struct
_tuple5 _tmp1F5;_tmp1F5.f1=outflow;_tmp1F5.f2=rval;_tmp1F5;});_LL17A: if((_tmp1F1.ReachableFL).tag
!= 1)goto _LL177;_tmp1F2=(_tmp1F1.ReachableFL).f1;_tmp1F3=(_tmp1F1.ReachableFL).f2;
_tmp1F4=(_tmp1F1.ReachableFL).f3;_LL17B: return({struct _tuple5 _tmp1F6;_tmp1F6.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1F7;(
_tmp1F7.ReachableFL).tag=1;(_tmp1F7.ReachableFL).f1=Cyc_Dict_insert(_tmp1F2,root,
place_val);(_tmp1F7.ReachableFL).f2=_tmp1F3;(_tmp1F7.ReachableFL).f3=_tmp1F4;
_tmp1F7;});_tmp1F6.f2=rval;_tmp1F6;});_LL177:;}}_LL106: if(*((int*)_tmp11C)!= 36)
goto _LL108;_tmp144=((struct Cyc_Absyn_Swap_e_struct*)_tmp11C)->f1;_tmp145=((
struct Cyc_Absyn_Swap_e_struct*)_tmp11C)->f2;_LL107: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle _tmp1FD=
_new_region("temp");struct _RegionHandle*temp=& _tmp1FD;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp200;struct Cyc_List_List*_tmp201;struct _tuple11
_tmp1FF=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp1FE[2];_tmp1FE[1]=_tmp145;_tmp1FE[0]=_tmp144;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp1FE,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp200=_tmp1FF.f1;_tmp201=
_tmp1FF.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp201))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp201->tl))->hd;outflow=
_tmp200;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp203;struct _tuple7 _tmp202=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp144);_tmp203=_tmp202.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp205;struct _tuple7 _tmp204=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp145);
_tmp205=_tmp204.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp206=outflow;struct Cyc_Dict_Dict
_tmp207;struct Cyc_List_List*_tmp208;struct Cyc_CfFlowInfo_ConsumeInfo _tmp209;
_LL17D: if((_tmp206.ReachableFL).tag != 1)goto _LL17F;_tmp207=(_tmp206.ReachableFL).f1;
_tmp208=(_tmp206.ReachableFL).f2;_tmp209=(_tmp206.ReachableFL).f3;_LL17E:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp20A=_tmp203;struct Cyc_CfFlowInfo_Place*_tmp20B;
_LL182: if((_tmp20A.PlaceL).tag != 0)goto _LL184;_tmp20B=(_tmp20A.PlaceL).f1;_LL183:
_tmp207=Cyc_CfFlowInfo_assign_place(_tmp115,_tmp144->loc,_tmp207,env->all_changed,
_tmp20B,right_rval);goto _LL181;_LL184: if((_tmp20A.UnknownL).tag != 1)goto _LL181;
_LL185: goto _LL181;_LL181:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp20C=_tmp205;
struct Cyc_CfFlowInfo_Place*_tmp20D;_LL187: if((_tmp20C.PlaceL).tag != 0)goto _LL189;
_tmp20D=(_tmp20C.PlaceL).f1;_LL188: _tmp207=Cyc_CfFlowInfo_assign_place(_tmp115,
_tmp145->loc,_tmp207,env->all_changed,_tmp20D,left_rval);goto _LL186;_LL189: if((
_tmp20C.UnknownL).tag != 1)goto _LL186;_LL18A: goto _LL186;_LL186:;}_tmp208=Cyc_CfFlowInfo_reln_kill_exp(
_tmp115->r,_tmp208,_tmp144);_tmp208=Cyc_CfFlowInfo_reln_kill_exp(_tmp115->r,
_tmp208,_tmp145);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp20E;(_tmp20E.ReachableFL).tag=1;(_tmp20E.ReachableFL).f1=_tmp207;(_tmp20E.ReachableFL).f2=
_tmp208;(_tmp20E.ReachableFL).f3=_tmp209;_tmp20E;});goto _LL17C;_LL17F:;_LL180:
goto _LL17C;_LL17C:;}return({struct _tuple5 _tmp20F;_tmp20F.f1=outflow;_tmp20F.f2=(
void*)_tmp115->unknown_all;_tmp20F;});}}}_LL108: if(*((int*)_tmp11C)!= 17)goto
_LL10A;_tmp146=((struct Cyc_Absyn_New_e_struct*)_tmp11C)->f1;_tmp147=((struct Cyc_Absyn_New_e_struct*)
_tmp11C)->f2;_LL109: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp222=_region_malloc(_tmp115->r,sizeof(*_tmp222));_tmp222[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp223;_tmp223.tag=1;_tmp223.f1=_tmp147;_tmp223.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp223;});_tmp222;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp221=_region_malloc(_tmp115->r,sizeof(*_tmp221));
_tmp221->root=(void*)root;_tmp221->fields=0;_tmp221;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp21F=_region_malloc(_tmp115->r,sizeof(*
_tmp21F));_tmp21F[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp220;_tmp220.tag=
2;_tmp220.f1=place;_tmp220;});_tmp21F;});Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmp146 != 
0){struct _RegionHandle _tmp210=_new_region("temp");struct _RegionHandle*temp=&
_tmp210;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp213;struct Cyc_List_List*
_tmp214;struct _tuple11 _tmp212=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp211[2];_tmp211[1]=_tmp147;_tmp211[0]=(struct Cyc_Absyn_Exp*)
_tmp146;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp211,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp213=_tmp212.f1;_tmp214=_tmp212.f2;outflow=_tmp213;place_val=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp214))->tl))->hd;};
_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp216;void*_tmp217;
struct _tuple5 _tmp215=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp147);_tmp216=
_tmp215.f1;_tmp217=_tmp215.f2;outflow=_tmp216;place_val=_tmp217;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp147->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp218=outflow;struct
Cyc_Dict_Dict _tmp219;struct Cyc_List_List*_tmp21A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp21B;_LL18C: if((_tmp218.BottomFL).tag != 0)goto _LL18E;_LL18D: return({struct
_tuple5 _tmp21C;_tmp21C.f1=outflow;_tmp21C.f2=rval;_tmp21C;});_LL18E: if((_tmp218.ReachableFL).tag
!= 1)goto _LL18B;_tmp219=(_tmp218.ReachableFL).f1;_tmp21A=(_tmp218.ReachableFL).f2;
_tmp21B=(_tmp218.ReachableFL).f3;_LL18F: return({struct _tuple5 _tmp21D;_tmp21D.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp21E;(
_tmp21E.ReachableFL).tag=1;(_tmp21E.ReachableFL).f1=Cyc_Dict_insert(_tmp219,root,
place_val);(_tmp21E.ReachableFL).f2=_tmp21A;(_tmp21E.ReachableFL).f3=_tmp21B;
_tmp21E;});_tmp21D.f2=rval;_tmp21D;});_LL18B:;}}}_LL10A: if(*((int*)_tmp11C)!= 16)
goto _LL10C;_tmp148=((struct Cyc_Absyn_Address_e_struct*)_tmp11C)->f1;_LL10B: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp225;struct _tuple5 _tmp224=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp148);_tmp225=_tmp224.f1;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp227;
struct _tuple6 _tmp226=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp225,0);
_tmp227=_tmp226.f1;{union Cyc_CfFlowInfo_FlowInfo_union _tmp229;union Cyc_CfFlowInfo_AbsLVal_union
_tmp22A;struct _tuple7 _tmp228=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp148);
_tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;_tmp229=Cyc_CfFlowInfo_restore_consume_info(
_tmp229,_tmp227);{union Cyc_CfFlowInfo_AbsLVal_union _tmp22B=_tmp22A;struct Cyc_CfFlowInfo_Place*
_tmp22C;_LL191: if((_tmp22B.UnknownL).tag != 1)goto _LL193;_LL192: return({struct
_tuple5 _tmp22D;_tmp22D.f1=_tmp229;_tmp22D.f2=(void*)1;_tmp22D;});_LL193: if((
_tmp22B.PlaceL).tag != 0)goto _LL190;_tmp22C=(_tmp22B.PlaceL).f1;_LL194: return({
struct _tuple5 _tmp22E;_tmp22E.f1=_tmp229;_tmp22E.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp22F=_region_malloc(env->r,sizeof(*_tmp22F));_tmp22F[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp230;_tmp230.tag=2;_tmp230.f1=_tmp22C;_tmp230;});_tmp22F;});_tmp22E;});_LL190:;}}}}
_LL10C: if(*((int*)_tmp11C)!= 22)goto _LL10E;_tmp149=((struct Cyc_Absyn_Deref_e_struct*)
_tmp11C)->f1;_LL10D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp232;void*_tmp233;
struct _tuple5 _tmp231=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp149);_tmp232=
_tmp231.f1;_tmp233=_tmp231.f2;_tmp232=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp232);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp232,_tmp149,_tmp233);}
_LL10E: if(*((int*)_tmp11C)!= 23)goto _LL110;_tmp14A=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp11C)->f1;_tmp14B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp11C)->f2;_LL10F: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp235;void*_tmp236;struct _tuple5 _tmp234=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp14A);_tmp235=_tmp234.f1;_tmp236=_tmp234.f2;_tmp235=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp235);_tmp235=Cyc_NewControlFlow_may_consume_exp(env,_tmp235,e);if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp14A->topt))->v))return({struct
_tuple5 _tmp237;_tmp237.f1=_tmp235;_tmp237.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp115->unknown_all);
_tmp237;});{void*_tmp238=_tmp236;struct _dynforward_ptr _tmp239;_LL196: if(_tmp238
<= (void*)3)goto _LL198;if(*((int*)_tmp238)!= 4)goto _LL198;_tmp239=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp238)->f1;_LL197: {int _tmp23A=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp14A->topt))->v,_tmp14B);return({struct
_tuple5 _tmp23B;_tmp23B.f1=_tmp235;_tmp23B.f2=*((void**)
_check_dynforward_subscript(_tmp239,sizeof(void*),_tmp23A));_tmp23B;});}_LL198:;
_LL199:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp23C=_cycalloc(
sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Core_Impossible_struct _tmp23D;_tmp23D.tag=
Cyc_Core_Impossible;_tmp23D.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp240;_tmp240.tag=0;_tmp240.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp23E[1]={& _tmp240};Cyc_aprintf(({const char*
_tmp23F="anal_Rexp: AggrMember: %s";_tag_dynforward(_tmp23F,sizeof(char),
_get_zero_arr_size(_tmp23F,26));}),_tag_dynforward(_tmp23E,sizeof(void*),1));}});
_tmp23D;});_tmp23C;}));_LL195:;}}_LL110: if(*((int*)_tmp11C)!= 24)goto _LL112;
_tmp14C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp11C)->f1;_tmp14D=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp11C)->f2;_LL111: {union Cyc_CfFlowInfo_FlowInfo_union _tmp242;void*_tmp243;
struct _tuple5 _tmp241=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp14C);_tmp242=
_tmp241.f1;_tmp243=_tmp241.f2;_tmp242=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp242);{union Cyc_CfFlowInfo_FlowInfo_union _tmp245;void*_tmp246;struct _tuple5
_tmp244=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp242,_tmp14C,_tmp243);
_tmp245=_tmp244.f1;_tmp246=_tmp244.f2;{void*_tmp247=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp14C->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp248;void*_tmp249;_LL19B: if(_tmp247 <= (void*)4)goto _LL19D;if(*((int*)_tmp247)
!= 4)goto _LL19D;_tmp248=((struct Cyc_Absyn_PointerType_struct*)_tmp247)->f1;
_tmp249=(void*)_tmp248.elt_typ;_LL19C: if(Cyc_Absyn_is_union_type(_tmp249))return({
struct _tuple5 _tmp24A;_tmp24A.f1=_tmp245;_tmp24A.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp115->unknown_all);
_tmp24A;});{void*_tmp24B=_tmp246;struct _dynforward_ptr _tmp24C;_LL1A0: if(_tmp24B
<= (void*)3)goto _LL1A2;if(*((int*)_tmp24B)!= 4)goto _LL1A2;_tmp24C=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp24B)->f1;_LL1A1: {int _tmp24D=Cyc_NewControlFlow_get_field_index(_tmp249,
_tmp14D);return({struct _tuple5 _tmp24E;_tmp24E.f1=_tmp245;_tmp24E.f2=*((void**)
_check_dynforward_subscript(_tmp24C,sizeof(void*),_tmp24D));_tmp24E;});}_LL1A2:;
_LL1A3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp24F=_cycalloc(
sizeof(*_tmp24F));_tmp24F[0]=({struct Cyc_Core_Impossible_struct _tmp250;_tmp250.tag=
Cyc_Core_Impossible;_tmp250.f1=({const char*_tmp251="anal_Rexp: AggrArrow";
_tag_dynforward(_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,21));});_tmp250;});
_tmp24F;}));_LL19F:;}_LL19D:;_LL19E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Core_Impossible_struct
_tmp253;_tmp253.tag=Cyc_Core_Impossible;_tmp253.f1=({const char*_tmp254="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,25));});_tmp253;});
_tmp252;}));_LL19A:;}}}_LL112: if(*((int*)_tmp11C)!= 6)goto _LL114;_tmp14E=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp11C)->f1;_tmp14F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp11C)->f2;_tmp150=((struct Cyc_Absyn_Conditional_e_struct*)_tmp11C)->f3;_LL113: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp256;union Cyc_CfFlowInfo_FlowInfo_union
_tmp257;struct _tuple8 _tmp255=Cyc_NewControlFlow_anal_test(env,inflow,_tmp14E);
_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;_tmp256=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp14E->loc,_tmp256);_tmp257=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp14E->loc,
_tmp257);{struct _tuple5 _tmp258=Cyc_NewControlFlow_anal_Rexp(env,_tmp256,_tmp14F);
struct _tuple5 _tmp259=Cyc_NewControlFlow_anal_Rexp(env,_tmp257,_tmp150);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp115,env->all_changed,_tmp258,_tmp259,1);}}_LL114: if(*((int*)_tmp11C)!= 7)
goto _LL116;_tmp151=((struct Cyc_Absyn_And_e_struct*)_tmp11C)->f1;_tmp152=((struct
Cyc_Absyn_And_e_struct*)_tmp11C)->f2;_LL115: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp25B;union Cyc_CfFlowInfo_FlowInfo_union _tmp25C;struct _tuple8 _tmp25A=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp151);_tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;_tmp25B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp151->loc,_tmp25B);_tmp25C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp151->loc,
_tmp25C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp25E;void*_tmp25F;struct _tuple5
_tmp25D=Cyc_NewControlFlow_anal_Rexp(env,_tmp25B,_tmp152);_tmp25E=_tmp25D.f1;
_tmp25F=_tmp25D.f2;_tmp25E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp152->loc,
_tmp25E);{struct _tuple5 _tmp260=({struct _tuple5 _tmp263;_tmp263.f1=_tmp25E;_tmp263.f2=
_tmp25F;_tmp263;});struct _tuple5 _tmp261=({struct _tuple5 _tmp262;_tmp262.f1=
_tmp25C;_tmp262.f2=(void*)((void*)0);_tmp262;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp115,env->all_changed,_tmp260,_tmp261,0);}}}_LL116: if(*((int*)_tmp11C)!= 8)
goto _LL118;_tmp153=((struct Cyc_Absyn_Or_e_struct*)_tmp11C)->f1;_tmp154=((struct
Cyc_Absyn_Or_e_struct*)_tmp11C)->f2;_LL117: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp265;union Cyc_CfFlowInfo_FlowInfo_union _tmp266;struct _tuple8 _tmp264=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp153);_tmp265=_tmp264.f1;_tmp266=_tmp264.f2;_tmp265=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp153->loc,_tmp265);_tmp266=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp153->loc,
_tmp266);{union Cyc_CfFlowInfo_FlowInfo_union _tmp268;void*_tmp269;struct _tuple5
_tmp267=Cyc_NewControlFlow_anal_Rexp(env,_tmp266,_tmp154);_tmp268=_tmp267.f1;
_tmp269=_tmp267.f2;_tmp268=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp154->loc,
_tmp268);{struct _tuple5 _tmp26A=({struct _tuple5 _tmp26D;_tmp26D.f1=_tmp268;_tmp26D.f2=
_tmp269;_tmp26D;});struct _tuple5 _tmp26B=({struct _tuple5 _tmp26C;_tmp26C.f1=
_tmp265;_tmp26C.f2=(void*)((void*)1);_tmp26C;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp115,env->all_changed,_tmp26A,_tmp26B,0);}}}_LL118: if(*((int*)_tmp11C)!= 25)
goto _LL11A;_tmp155=((struct Cyc_Absyn_Subscript_e_struct*)_tmp11C)->f1;_tmp156=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp11C)->f2;_LL119: {struct _RegionHandle
_tmp26E=_new_region("temp");struct _RegionHandle*temp=& _tmp26E;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp271;struct Cyc_List_List*_tmp272;struct
_tuple11 _tmp270=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct
Cyc_Absyn_Exp*_tmp26F[2];_tmp26F[1]=_tmp156;_tmp26F[0]=_tmp155;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp26F,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp271=_tmp270.f1;_tmp272=
_tmp270.f2;_tmp271=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp156->loc,_tmp271);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp273=_tmp271;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp274=_tmp271;struct Cyc_Dict_Dict _tmp275;struct Cyc_List_List*_tmp276;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp277;_LL1A5: if((_tmp274.ReachableFL).tag != 1)goto _LL1A7;_tmp275=(_tmp274.ReachableFL).f1;
_tmp276=(_tmp274.ReachableFL).f2;_tmp277=(_tmp274.ReachableFL).f3;_LL1A6: if(Cyc_CfFlowInfo_initlevel(
_tmp275,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp272))->tl))->hd)== (void*)0)({void*_tmp278[0]={};Cyc_Tcutil_terr(
_tmp156->loc,({const char*_tmp279="expression may not be initialized";
_tag_dynforward(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,34));}),
_tag_dynforward(_tmp278,sizeof(void*),0));});{struct Cyc_List_List*_tmp27A=Cyc_NewControlFlow_add_subscript_reln(
_tmp115->r,_tmp276,_tmp155,_tmp156);if(_tmp276 != _tmp27A)_tmp273=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp27B;(_tmp27B.ReachableFL).tag=1;(_tmp27B.ReachableFL).f1=
_tmp275;(_tmp27B.ReachableFL).f2=_tmp27A;(_tmp27B.ReachableFL).f3=_tmp277;
_tmp27B;});goto _LL1A4;}_LL1A7:;_LL1A8: goto _LL1A4;_LL1A4:;}{void*_tmp27C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp155->topt))->v);struct Cyc_List_List*
_tmp27D;struct Cyc_Absyn_PtrInfo _tmp27E;struct Cyc_Absyn_PtrAtts _tmp27F;struct Cyc_Absyn_Conref*
_tmp280;_LL1AA: if(_tmp27C <= (void*)4)goto _LL1AE;if(*((int*)_tmp27C)!= 9)goto
_LL1AC;_tmp27D=((struct Cyc_Absyn_TupleType_struct*)_tmp27C)->f1;_LL1AB: {void*
_tmp281=(void*)((struct Cyc_List_List*)_check_null(_tmp272))->hd;struct
_dynforward_ptr _tmp282;_LL1B1: if(_tmp281 <= (void*)3)goto _LL1B3;if(*((int*)
_tmp281)!= 4)goto _LL1B3;_tmp282=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp281)->f1;
_LL1B2: _tmp273=Cyc_NewControlFlow_may_consume_exp(env,_tmp273,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp156)).f1;struct _tuple5 _tmp284=({struct _tuple5
_tmp283;_tmp283.f1=_tmp273;_tmp283.f2=*((void**)_check_dynforward_subscript(
_tmp282,sizeof(void*),(int)i));_tmp283;});_npop_handler(0);return _tmp284;}_LL1B3:;
_LL1B4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp285=_cycalloc(
sizeof(*_tmp285));_tmp285[0]=({struct Cyc_Core_Impossible_struct _tmp286;_tmp286.tag=
Cyc_Core_Impossible;_tmp286.f1=({const char*_tmp287="anal_Rexp: Subscript";
_tag_dynforward(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,21));});_tmp286;});
_tmp285;}));_LL1B0:;}_LL1AC: if(*((int*)_tmp27C)!= 4)goto _LL1AE;_tmp27E=((struct
Cyc_Absyn_PointerType_struct*)_tmp27C)->f1;_tmp27F=_tmp27E.ptr_atts;_tmp280=
_tmp27F.bounds;_LL1AD:{union Cyc_Absyn_Constraint_union _tmp288=(Cyc_Absyn_compress_conref(
_tmp280))->v;void*_tmp289;_LL1B6: if((_tmp288.Eq_constr).tag != 0)goto _LL1B8;
_tmp289=(_tmp288.Eq_constr).f1;if(_tmp289 <= (void*)2)goto _LL1B8;if(*((int*)
_tmp289)!= 1)goto _LL1B8;_LL1B7:{void*_tmp28A=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp272))->tl))->hd;struct Cyc_List_List*
_tmp28B;_LL1BB: if(_tmp28A <= (void*)3)goto _LL1BD;if(*((int*)_tmp28A)!= 3)goto
_LL1BD;_tmp28B=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp28A)->f1;_LL1BC:(void*)(
_tmp156->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp28C=
_cycalloc(sizeof(*_tmp28C));_tmp28C[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp28D;_tmp28D.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp28D.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp28B);_tmp28D;});_tmp28C;})));goto _LL1BA;_LL1BD:;_LL1BE:
goto _LL1BA;_LL1BA:;}goto _LL1B5;_LL1B8:;_LL1B9: goto _LL1B5;_LL1B5:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp28F;void*_tmp290;struct _tuple5 _tmp28E=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp271,_tmp155,(void*)((struct Cyc_List_List*)_check_null(_tmp272))->hd);
_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp291=
_tmp28F;_LL1C0: if((_tmp291.BottomFL).tag != 0)goto _LL1C2;_LL1C1: {struct _tuple5
_tmp293=({struct _tuple5 _tmp292;_tmp292.f1=_tmp28F;_tmp292.f2=_tmp290;_tmp292;});
_npop_handler(0);return _tmp293;}_LL1C2:;_LL1C3: {struct _tuple5 _tmp295=({struct
_tuple5 _tmp294;_tmp294.f1=_tmp273;_tmp294.f2=_tmp290;_tmp294;});_npop_handler(0);
return _tmp295;}_LL1BF:;}}_LL1AE:;_LL1AF:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296[0]=({struct Cyc_Core_Impossible_struct
_tmp297;_tmp297.tag=Cyc_Core_Impossible;_tmp297.f1=({const char*_tmp298="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(_tmp298,33));});_tmp297;});
_tmp296;}));_LL1A9:;}}};_pop_region(temp);}_LL11A: if(*((int*)_tmp11C)!= 32)goto
_LL11C;_tmp157=((struct Cyc_Absyn_Tunion_e_struct*)_tmp11C)->f1;_tmp158=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp11C)->f2;_LL11B: if(_tmp158->is_flat){struct
_RegionHandle _tmp299=_new_region("temp");struct _RegionHandle*temp=& _tmp299;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp29B;struct Cyc_List_List*
_tmp29C;struct _tuple11 _tmp29A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,_tmp157,0);_tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;_tmp29B=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp29B);for(0;(unsigned int)_tmp157;(_tmp157=_tmp157->tl,_tmp29C=_tmp29C->tl)){
_tmp29B=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp157->hd)->loc,
_tmp29B,(void*)((struct Cyc_List_List*)_check_null(_tmp29C))->hd);}{struct _tuple5
_tmp29E=({struct _tuple5 _tmp29D;_tmp29D.f1=_tmp29B;_tmp29D.f2=(void*)_tmp115->unknown_all;
_tmp29D;});_npop_handler(0);return _tmp29E;}};_pop_region(temp);}_tmp159=_tmp157;
goto _LL11D;_LL11C: if(*((int*)_tmp11C)!= 26)goto _LL11E;_tmp159=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp11C)->f1;_LL11D: {struct _RegionHandle _tmp29F=_new_region("temp");struct
_RegionHandle*temp=& _tmp29F;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2A1;struct Cyc_List_List*_tmp2A2;struct _tuple11 _tmp2A0=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp159,0);_tmp2A1=_tmp2A0.f1;_tmp2A2=_tmp2A0.f2;_tmp2A1=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2A1);{struct _dynforward_ptr aggrdict=({unsigned int _tmp2A7=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp159);void**
_tmp2A8=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp2A7));
struct _dynforward_ptr _tmp2AB=_tag_dynforward(_tmp2A8,sizeof(void*),_tmp2A7);{
unsigned int _tmp2A9=_tmp2A7;unsigned int i;for(i=0;i < _tmp2A9;i ++){_tmp2A8[i]=({
void*_tmp2AA=(void*)((struct Cyc_List_List*)_check_null(_tmp2A2))->hd;_tmp2A2=
_tmp2A2->tl;_tmp2AA;});}}_tmp2AB;});struct _tuple5 _tmp2A6=({struct _tuple5 _tmp2A3;
_tmp2A3.f1=_tmp2A1;_tmp2A3.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2A4=_region_malloc(env->r,sizeof(*_tmp2A4));_tmp2A4[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2A5;_tmp2A5.tag=4;_tmp2A5.f1=aggrdict;_tmp2A5;});_tmp2A4;});_tmp2A3;});
_npop_handler(0);return _tmp2A6;}};_pop_region(temp);}_LL11E: if(*((int*)_tmp11C)
!= 31)goto _LL120;_tmp15A=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp11C)->f2;
_LL11F: {struct Cyc_List_List*fs;{void*_tmp2AC=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*_tmp2AD;_LL1C5:
if(_tmp2AC <= (void*)4)goto _LL1C7;if(*((int*)_tmp2AC)!= 11)goto _LL1C7;_tmp2AD=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2AC)->f2;_LL1C6: fs=_tmp2AD;goto _LL1C4;
_LL1C7:;_LL1C8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2AE=
_cycalloc(sizeof(*_tmp2AE));_tmp2AE[0]=({struct Cyc_Core_Impossible_struct _tmp2AF;
_tmp2AF.tag=Cyc_Core_Impossible;_tmp2AF.f1=({const char*_tmp2B0="anal_Rexp:anon struct has bad type";
_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,35));});_tmp2AF;});
_tmp2AE;}));_LL1C4:;}_tmp15B=_tmp15A;_tmp160=fs;goto _LL121;}_LL120: if(*((int*)
_tmp11C)!= 30)goto _LL122;_tmp15B=((struct Cyc_Absyn_Struct_e_struct*)_tmp11C)->f3;
_tmp15C=((struct Cyc_Absyn_Struct_e_struct*)_tmp11C)->f4;if(_tmp15C == 0)goto
_LL122;_tmp15D=*_tmp15C;_tmp15E=_tmp15D.impl;if(_tmp15E == 0)goto _LL122;_tmp15F=*
_tmp15E;_tmp160=_tmp15F.fields;_LL121: {struct _RegionHandle _tmp2B1=_new_region("temp");
struct _RegionHandle*temp=& _tmp2B1;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B3;struct Cyc_List_List*_tmp2B4;struct _tuple11 _tmp2B2=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp15B),0);_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;
_tmp2B3=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp2B3);{struct
_dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp115,_tmp160,(
void*)_tmp115->unknown_all);{int i=0;for(0;_tmp2B4 != 0;(((_tmp2B4=_tmp2B4->tl,
_tmp15B=_tmp15B->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple14*)((struct
Cyc_List_List*)_check_null(_tmp15B))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp2B5=(void*)ds->hd;struct _dynforward_ptr*_tmp2B6;_LL1CA: if(*((int*)_tmp2B5)!= 
0)goto _LL1CC;_LL1CB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7[0]=({struct Cyc_Core_Impossible_struct
_tmp2B8;_tmp2B8.tag=Cyc_Core_Impossible;_tmp2B8.f1=({const char*_tmp2B9="anal_Rexp:Struct_e";
_tag_dynforward(_tmp2B9,sizeof(char),_get_zero_arr_size(_tmp2B9,19));});_tmp2B8;});
_tmp2B7;}));_LL1CC: if(*((int*)_tmp2B5)!= 1)goto _LL1C9;_tmp2B6=((struct Cyc_Absyn_FieldName_struct*)
_tmp2B5)->f1;_LL1CD: {int _tmp2BA=Cyc_NewControlFlow_get_field_index_fs(_tmp160,
_tmp2B6);*((void**)_check_dynforward_subscript(aggrdict,sizeof(void*),_tmp2BA))=(
void*)_tmp2B4->hd;}_LL1C9:;}}}{struct _tuple5 _tmp2BE=({struct _tuple5 _tmp2BB;
_tmp2BB.f1=_tmp2B3;_tmp2BB.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2BC=_region_malloc(env->r,sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2BD;_tmp2BD.tag=4;_tmp2BD.f1=aggrdict;_tmp2BD;});_tmp2BC;});_tmp2BB;});
_npop_handler(0);return _tmp2BE;}}};_pop_region(temp);}_LL122: if(*((int*)_tmp11C)
!= 30)goto _LL124;_LL123:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF[0]=({struct Cyc_Core_Impossible_struct
_tmp2C0;_tmp2C0.tag=Cyc_Core_Impossible;_tmp2C0.f1=({const char*_tmp2C1="anal_Rexp:missing aggrdeclimpl";
_tag_dynforward(_tmp2C1,sizeof(char),_get_zero_arr_size(_tmp2C1,31));});_tmp2C0;});
_tmp2BF;}));_LL124: if(*((int*)_tmp11C)!= 28)goto _LL126;_tmp161=((struct Cyc_Absyn_Array_e_struct*)
_tmp11C)->f1;_LL125: {struct _RegionHandle _tmp2C2=_new_region("temp");struct
_RegionHandle*temp=& _tmp2C2;_push_region(temp);{struct Cyc_List_List*_tmp2C3=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp161);union Cyc_CfFlowInfo_FlowInfo_union _tmp2C5;
struct Cyc_List_List*_tmp2C6;struct _tuple11 _tmp2C4=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp2C3,0);_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;_tmp2C5=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2C5);for(0;_tmp2C6 != 0;(_tmp2C6=_tmp2C6->tl,_tmp2C3=_tmp2C3->tl)){
_tmp2C5=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2C3))->hd)->loc,_tmp2C5,(void*)_tmp2C6->hd);}{struct _tuple5
_tmp2C8=({struct _tuple5 _tmp2C7;_tmp2C7.f1=_tmp2C5;_tmp2C7.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp115,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp115->unknown_all);
_tmp2C7;});_npop_handler(0);return _tmp2C8;}};_pop_region(temp);}_LL126: if(*((int*)
_tmp11C)!= 29)goto _LL128;_tmp162=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp11C)->f1;_tmp163=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp11C)->f2;
_tmp164=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp11C)->f3;_tmp165=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp11C)->f4;_LL127: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2CA;void*_tmp2CB;struct _tuple5 _tmp2C9=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp163);_tmp2CA=_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;_tmp2CA=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp163->loc,_tmp2CA);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2CC=_tmp2CA;struct
Cyc_Dict_Dict _tmp2CD;struct Cyc_List_List*_tmp2CE;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2CF;_LL1CF: if((_tmp2CC.BottomFL).tag != 0)goto _LL1D1;_LL1D0: return({struct
_tuple5 _tmp2D0;_tmp2D0.f1=_tmp2CA;_tmp2D0.f2=(void*)_tmp115->unknown_all;_tmp2D0;});
_LL1D1: if((_tmp2CC.ReachableFL).tag != 1)goto _LL1CE;_tmp2CD=(_tmp2CC.ReachableFL).f1;
_tmp2CE=(_tmp2CC.ReachableFL).f2;_tmp2CF=(_tmp2CC.ReachableFL).f3;_LL1D2: if(Cyc_CfFlowInfo_initlevel(
_tmp2CD,_tmp2CB)== (void*)0)({void*_tmp2D1[0]={};Cyc_Tcutil_terr(_tmp163->loc,({
const char*_tmp2D2="expression may not be initialized";_tag_dynforward(_tmp2D2,
sizeof(char),_get_zero_arr_size(_tmp2D2,34));}),_tag_dynforward(_tmp2D1,sizeof(
void*),0));});{struct Cyc_List_List*new_relns=_tmp2CE;comp_loop: {void*_tmp2D3=(
void*)_tmp163->r;struct Cyc_Absyn_Exp*_tmp2D4;void*_tmp2D5;struct Cyc_Absyn_Vardecl*
_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_Vardecl*_tmp2D8;void*_tmp2D9;struct Cyc_Absyn_Vardecl*
_tmp2DA;void*_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DC;union Cyc_Absyn_Cnst_union
_tmp2DD;int _tmp2DE;void*_tmp2DF;struct Cyc_List_List*_tmp2E0;struct Cyc_List_List
_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;_LL1D4: if(*((int*)_tmp2D3)!= 15)goto _LL1D6;
_tmp2D4=((struct Cyc_Absyn_Cast_e_struct*)_tmp2D3)->f2;_LL1D5: _tmp163=_tmp2D4;
goto comp_loop;_LL1D6: if(*((int*)_tmp2D3)!= 1)goto _LL1D8;_tmp2D5=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp2D3)->f2;if(_tmp2D5 <= (void*)1)goto _LL1D8;if(*((int*)
_tmp2D5)!= 0)goto _LL1D8;_tmp2D6=((struct Cyc_Absyn_Global_b_struct*)_tmp2D5)->f1;
if(!(!_tmp2D6->escapes))goto _LL1D8;_LL1D7: _tmp2D8=_tmp2D6;goto _LL1D9;_LL1D8: if(*((
int*)_tmp2D3)!= 1)goto _LL1DA;_tmp2D7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D3)->f2;if(_tmp2D7 <= (void*)1)goto _LL1DA;if(*((int*)_tmp2D7)!= 3)goto _LL1DA;
_tmp2D8=((struct Cyc_Absyn_Local_b_struct*)_tmp2D7)->f1;if(!(!_tmp2D8->escapes))
goto _LL1DA;_LL1D9: _tmp2DA=_tmp2D8;goto _LL1DB;_LL1DA: if(*((int*)_tmp2D3)!= 1)goto
_LL1DC;_tmp2D9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D3)->f2;if(_tmp2D9 <= (
void*)1)goto _LL1DC;if(*((int*)_tmp2D9)!= 4)goto _LL1DC;_tmp2DA=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2D9)->f1;if(!(!_tmp2DA->escapes))goto _LL1DC;_LL1DB: _tmp2DC=_tmp2DA;goto
_LL1DD;_LL1DC: if(*((int*)_tmp2D3)!= 1)goto _LL1DE;_tmp2DB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D3)->f2;if(_tmp2DB <= (void*)1)goto _LL1DE;if(*((int*)_tmp2DB)!= 2)goto _LL1DE;
_tmp2DC=((struct Cyc_Absyn_Param_b_struct*)_tmp2DB)->f1;if(!(!_tmp2DC->escapes))
goto _LL1DE;_LL1DD: new_relns=({struct Cyc_List_List*_tmp2E3=_region_malloc(env->r,
sizeof(*_tmp2E3));_tmp2E3->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2E4=_region_malloc(
env->r,sizeof(*_tmp2E4));_tmp2E4->vd=_tmp162;_tmp2E4->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2E5;(_tmp2E5.LessVar).tag=1;(_tmp2E5.LessVar).f1=
_tmp2DC;_tmp2E5;});_tmp2E4;});_tmp2E3->tl=_tmp2CE;_tmp2E3;});goto _LL1D3;_LL1DE:
if(*((int*)_tmp2D3)!= 0)goto _LL1E0;_tmp2DD=((struct Cyc_Absyn_Const_e_struct*)
_tmp2D3)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2D3)->f1).Int_c).tag != 2)
goto _LL1E0;_tmp2DE=(_tmp2DD.Int_c).f2;_LL1DF: new_relns=({struct Cyc_List_List*
_tmp2E6=_region_malloc(env->r,sizeof(*_tmp2E6));_tmp2E6->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2E7=_region_malloc(env->r,sizeof(*_tmp2E7));_tmp2E7->vd=_tmp162;_tmp2E7->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2E8;(
_tmp2E8.LessConst).tag=3;(_tmp2E8.LessConst).f1=(unsigned int)_tmp2DE;_tmp2E8;});
_tmp2E7;});_tmp2E6->tl=_tmp2CE;_tmp2E6;});goto _LL1D3;_LL1E0: if(*((int*)_tmp2D3)
!= 3)goto _LL1E2;_tmp2DF=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2D3)->f1;
if((int)_tmp2DF != 19)goto _LL1E2;_tmp2E0=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2D3)->f2;if(_tmp2E0 == 0)goto _LL1E2;_tmp2E1=*_tmp2E0;_tmp2E2=(struct Cyc_Absyn_Exp*)
_tmp2E1.hd;_LL1E1:{void*_tmp2E9=(void*)_tmp2E2->r;void*_tmp2EA;struct Cyc_Absyn_Vardecl*
_tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;void*_tmp2EE;struct Cyc_Absyn_Vardecl*
_tmp2EF;void*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F1;_LL1E5: if(*((int*)_tmp2E9)
!= 1)goto _LL1E7;_tmp2EA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E9)->f2;if(
_tmp2EA <= (void*)1)goto _LL1E7;if(*((int*)_tmp2EA)!= 0)goto _LL1E7;_tmp2EB=((
struct Cyc_Absyn_Global_b_struct*)_tmp2EA)->f1;if(!(!_tmp2EB->escapes))goto _LL1E7;
_LL1E6: _tmp2ED=_tmp2EB;goto _LL1E8;_LL1E7: if(*((int*)_tmp2E9)!= 1)goto _LL1E9;
_tmp2EC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E9)->f2;if(_tmp2EC <= (void*)
1)goto _LL1E9;if(*((int*)_tmp2EC)!= 3)goto _LL1E9;_tmp2ED=((struct Cyc_Absyn_Local_b_struct*)
_tmp2EC)->f1;if(!(!_tmp2ED->escapes))goto _LL1E9;_LL1E8: _tmp2EF=_tmp2ED;goto
_LL1EA;_LL1E9: if(*((int*)_tmp2E9)!= 1)goto _LL1EB;_tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E9)->f2;if(_tmp2EE <= (void*)1)goto _LL1EB;if(*((int*)_tmp2EE)!= 4)goto _LL1EB;
_tmp2EF=((struct Cyc_Absyn_Pat_b_struct*)_tmp2EE)->f1;if(!(!_tmp2EF->escapes))
goto _LL1EB;_LL1EA: _tmp2F1=_tmp2EF;goto _LL1EC;_LL1EB: if(*((int*)_tmp2E9)!= 1)goto
_LL1ED;_tmp2F0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E9)->f2;if(_tmp2F0 <= (
void*)1)goto _LL1ED;if(*((int*)_tmp2F0)!= 2)goto _LL1ED;_tmp2F1=((struct Cyc_Absyn_Param_b_struct*)
_tmp2F0)->f1;if(!(!_tmp2F1->escapes))goto _LL1ED;_LL1EC: new_relns=({struct Cyc_List_List*
_tmp2F2=_region_malloc(env->r,sizeof(*_tmp2F2));_tmp2F2->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2F3=_region_malloc(env->r,sizeof(*_tmp2F3));_tmp2F3->vd=_tmp162;_tmp2F3->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2F4;(
_tmp2F4.LessSize).tag=2;(_tmp2F4.LessSize).f1=_tmp2F1;_tmp2F4;});_tmp2F3;});
_tmp2F2->tl=_tmp2CE;_tmp2F2;});goto _LL1E4;_LL1ED:;_LL1EE: goto _LL1E4;_LL1E4:;}
goto _LL1D3;_LL1E2:;_LL1E3: goto _LL1D3;_LL1D3:;}if(_tmp2CE != new_relns)_tmp2CA=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp2F5;(
_tmp2F5.ReachableFL).tag=1;(_tmp2F5.ReachableFL).f1=_tmp2CD;(_tmp2F5.ReachableFL).f2=
new_relns;(_tmp2F5.ReachableFL).f3=_tmp2CF;_tmp2F5;});{void*_tmp2F6=_tmp2CB;
_LL1F0: if((int)_tmp2F6 != 0)goto _LL1F2;_LL1F1: return({struct _tuple5 _tmp2F7;
_tmp2F7.f1=_tmp2CA;_tmp2F7.f2=(void*)_tmp115->unknown_all;_tmp2F7;});_LL1F2: if((
int)_tmp2F6 != 2)goto _LL1F4;_LL1F3: goto _LL1F5;_LL1F4: if((int)_tmp2F6 != 1)goto
_LL1F6;_LL1F5: goto _LL1F7;_LL1F6: if(_tmp2F6 <= (void*)3)goto _LL1F8;if(*((int*)
_tmp2F6)!= 2)goto _LL1F8;_LL1F7: {struct Cyc_List_List _tmp2F8=({struct Cyc_List_List
_tmp304;_tmp304.hd=_tmp162;_tmp304.tl=0;_tmp304;});_tmp2CA=Cyc_NewControlFlow_add_vars(
_tmp115,_tmp2CA,(struct Cyc_List_List*)& _tmp2F8,(void*)_tmp115->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp2FA;void*_tmp2FB;struct _tuple5 _tmp2F9=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2CA,_tmp164);_tmp2FA=_tmp2F9.f1;_tmp2FB=_tmp2F9.f2;_tmp2FA=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp164->loc,_tmp2FA);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2FC=_tmp2FA;struct
Cyc_Dict_Dict _tmp2FD;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2FE;_LL1FB: if((_tmp2FC.BottomFL).tag
!= 0)goto _LL1FD;_LL1FC: return({struct _tuple5 _tmp2FF;_tmp2FF.f1=_tmp2FA;_tmp2FF.f2=(
void*)_tmp115->unknown_all;_tmp2FF;});_LL1FD: if((_tmp2FC.ReachableFL).tag != 1)
goto _LL1FA;_tmp2FD=(_tmp2FC.ReachableFL).f1;_tmp2FE=(_tmp2FC.ReachableFL).f3;
_LL1FE: if(Cyc_CfFlowInfo_initlevel(_tmp2FD,_tmp2FB)!= (void*)2){({void*_tmp300[0]={};
Cyc_Tcutil_terr(_tmp163->loc,({const char*_tmp301="expression may not be initialized";
_tag_dynforward(_tmp301,sizeof(char),_get_zero_arr_size(_tmp301,34));}),
_tag_dynforward(_tmp300,sizeof(void*),0));});return({struct _tuple5 _tmp302;
_tmp302.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp303;(_tmp303.BottomFL).tag=0;_tmp303;});_tmp302.f2=(void*)_tmp115->unknown_all;
_tmp302;});}_LL1FA:;}_tmp2CA=_tmp2FA;goto _LL1F9;}}_LL1F8:;_LL1F9: while(1){struct
Cyc_List_List _tmp305=({struct Cyc_List_List _tmp311;_tmp311.hd=_tmp162;_tmp311.tl=
0;_tmp311;});_tmp2CA=Cyc_NewControlFlow_add_vars(_tmp115,_tmp2CA,(struct Cyc_List_List*)&
_tmp305,(void*)_tmp115->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp307;void*_tmp308;struct _tuple5 _tmp306=Cyc_NewControlFlow_anal_Rexp(env,
_tmp2CA,_tmp164);_tmp307=_tmp306.f1;_tmp308=_tmp306.f2;_tmp307=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp164->loc,_tmp307);{union Cyc_CfFlowInfo_FlowInfo_union _tmp309=_tmp307;struct
Cyc_Dict_Dict _tmp30A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30B;_LL200: if((_tmp309.BottomFL).tag
!= 0)goto _LL202;_LL201: goto _LL1FF;_LL202: if((_tmp309.ReachableFL).tag != 1)goto
_LL1FF;_tmp30A=(_tmp309.ReachableFL).f1;_tmp30B=(_tmp309.ReachableFL).f3;_LL203:
if(Cyc_CfFlowInfo_initlevel(_tmp30A,_tmp308)!= (void*)2){({void*_tmp30C[0]={};
Cyc_Tcutil_terr(_tmp163->loc,({const char*_tmp30D="expression may not be initialized";
_tag_dynforward(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,34));}),
_tag_dynforward(_tmp30C,sizeof(void*),0));});return({struct _tuple5 _tmp30E;
_tmp30E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp30F;(_tmp30F.BottomFL).tag=0;_tmp30F;});_tmp30E.f2=(void*)_tmp115->unknown_all;
_tmp30E;});}_LL1FF:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp310=Cyc_CfFlowInfo_join_flow(
_tmp115,env->all_changed,_tmp2CA,_tmp307,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp310,_tmp2CA))break;_tmp2CA=_tmp310;}}}return({struct _tuple5 _tmp312;_tmp312.f1=
_tmp2CA;_tmp312.f2=(void*)_tmp115->unknown_all;_tmp312;});_LL1EF:;}}_LL1CE:;}}
_LL128: if(*((int*)_tmp11C)!= 38)goto _LL12A;_tmp166=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp11C)->f1;_LL129: while(1){union Cyc_CfFlowInfo_FlowInfo_union*_tmp314;struct
_tuple9 _tmp313=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp166);_tmp314=
_tmp313.f2;inflow=*_tmp314;{void*_tmp315=(void*)_tmp166->r;struct Cyc_Absyn_Stmt*
_tmp316;struct Cyc_Absyn_Stmt*_tmp317;struct Cyc_Absyn_Decl*_tmp318;struct Cyc_Absyn_Stmt*
_tmp319;struct Cyc_Absyn_Exp*_tmp31A;_LL205: if(_tmp315 <= (void*)1)goto _LL20B;if(*((
int*)_tmp315)!= 1)goto _LL207;_tmp316=((struct Cyc_Absyn_Seq_s_struct*)_tmp315)->f1;
_tmp317=((struct Cyc_Absyn_Seq_s_struct*)_tmp315)->f2;_LL206: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp316);_tmp166=_tmp317;goto _LL204;_LL207: if(*((int*)_tmp315)!= 11)
goto _LL209;_tmp318=((struct Cyc_Absyn_Decl_s_struct*)_tmp315)->f1;_tmp319=((
struct Cyc_Absyn_Decl_s_struct*)_tmp315)->f2;_LL208: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp318);_tmp166=_tmp319;goto _LL204;_LL209: if(*((int*)_tmp315)!= 0)
goto _LL20B;_tmp31A=((struct Cyc_Absyn_Exp_s_struct*)_tmp315)->f1;_LL20A: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp31A);_LL20B:;_LL20C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B[0]=({struct Cyc_Core_Impossible_struct
_tmp31C;_tmp31C.tag=Cyc_Core_Impossible;_tmp31C.f1=({const char*_tmp31D="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp31D,sizeof(char),_get_zero_arr_size(_tmp31D,33));});_tmp31C;});
_tmp31B;}));_LL204:;}}_LL12A: if(*((int*)_tmp11C)!= 1)goto _LL12C;_tmp167=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp11C)->f2;if((int)_tmp167 != 0)goto _LL12C;_LL12B:
goto _LL12D;_LL12C: if(*((int*)_tmp11C)!= 2)goto _LL12E;_LL12D: goto _LL12F;_LL12E:
if(*((int*)_tmp11C)!= 10)goto _LL130;_LL12F: goto _LL131;_LL130: if(*((int*)_tmp11C)
!= 37)goto _LL132;_LL131: goto _LL133;_LL132: if(*((int*)_tmp11C)!= 27)goto _LLCD;
_LL133:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp31E=_cycalloc(
sizeof(*_tmp31E));_tmp31E[0]=({struct Cyc_Core_Impossible_struct _tmp31F;_tmp31F.tag=
Cyc_Core_Impossible;_tmp31F.f1=({const char*_tmp320="anal_Rexp, unexpected exp form";
_tag_dynforward(_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,31));});_tmp31F;});
_tmp31E;}));_LLCD:;}}static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,
struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*
_tmp321=env->fenv;void*_tmp322=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp323;void*_tmp324;struct Cyc_Absyn_PtrAtts
_tmp325;struct Cyc_Absyn_Conref*_tmp326;struct Cyc_Absyn_Conref*_tmp327;_LL20E: if(
_tmp322 <= (void*)4)goto _LL210;if(*((int*)_tmp322)!= 4)goto _LL210;_tmp323=((
struct Cyc_Absyn_PointerType_struct*)_tmp322)->f1;_tmp324=(void*)_tmp323.elt_typ;
_tmp325=_tmp323.ptr_atts;_tmp326=_tmp325.bounds;_tmp327=_tmp325.zero_term;_LL20F: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp328=f;struct Cyc_Dict_Dict _tmp329;struct Cyc_List_List*
_tmp32A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp32B;_LL213: if((_tmp328.BottomFL).tag
!= 0)goto _LL215;_LL214: return({struct _tuple7 _tmp32C;_tmp32C.f1=f;_tmp32C.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp32D;(
_tmp32D.UnknownL).tag=1;_tmp32D;});_tmp32C;});_LL215: if((_tmp328.ReachableFL).tag
!= 1)goto _LL212;_tmp329=(_tmp328.ReachableFL).f1;_tmp32A=(_tmp328.ReachableFL).f2;
_tmp32B=(_tmp328.ReachableFL).f3;_LL216: if(Cyc_Tcutil_is_bound_one(_tmp326)){
void*_tmp32E=r;struct Cyc_CfFlowInfo_Place*_tmp32F;struct Cyc_CfFlowInfo_Place
_tmp330;void*_tmp331;struct Cyc_List_List*_tmp332;void*_tmp333;_LL218: if((int)
_tmp32E != 1)goto _LL21A;_LL219: goto _LL21B;_LL21A: if((int)_tmp32E != 2)goto _LL21C;
_LL21B:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp335;_tmp335.tag=Cyc_CfFlowInfo_NotZero;_tmp335.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp32A);_tmp335;});_tmp334;})));goto _LL217;_LL21C: if(
_tmp32E <= (void*)3)goto _LL21E;if(*((int*)_tmp32E)!= 2)goto _LL21E;_tmp32F=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp32E)->f1;_tmp330=*_tmp32F;_tmp331=(
void*)_tmp330.root;_tmp332=_tmp330.fields;_LL21D:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp337;_tmp337.tag=Cyc_CfFlowInfo_NotZero;
_tmp337.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp32A);_tmp337;});
_tmp336;})));return({struct _tuple7 _tmp338;_tmp338.f1=f;_tmp338.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp339;(_tmp339.PlaceL).tag=0;(_tmp339.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp33A=_region_malloc(_tmp321->r,sizeof(*_tmp33A));
_tmp33A->root=(void*)_tmp331;_tmp33A->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp321->r,_tmp332,flds);_tmp33A;});_tmp339;});_tmp338;});_LL21E: if((int)_tmp32E
!= 0)goto _LL220;_LL21F:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp33B;_tmp33B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp33D;(_tmp33D.BottomFL).tag=0;_tmp33D;});
_tmp33B.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp33C;(_tmp33C.UnknownL).tag=1;_tmp33C;});_tmp33B;});_LL220: if(_tmp32E <= (void*)
3)goto _LL222;if(*((int*)_tmp32E)!= 0)goto _LL222;_tmp333=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp32E)->f1;_LL221: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp333);goto
_LL223;_LL222:;_LL223:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp33F;_tmp33F.tag=Cyc_CfFlowInfo_UnknownZ;_tmp33F.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp32A);_tmp33F;});_tmp33E;})));_LL217:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp340=_cycalloc(sizeof(*
_tmp340));_tmp340[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp341;_tmp341.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp341.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp32A);_tmp341;});_tmp340;})));}if(Cyc_CfFlowInfo_initlevel(_tmp329,r)== (void*)
0)({void*_tmp342[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp343="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,46));}),
_tag_dynforward(_tmp342,sizeof(void*),0));});return({struct _tuple7 _tmp344;
_tmp344.f1=f;_tmp344.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp345;(_tmp345.UnknownL).tag=1;_tmp345;});_tmp344;});_LL212:;}_LL210:;_LL211:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp346=_cycalloc(sizeof(*
_tmp346));_tmp346[0]=({struct Cyc_Core_Impossible_struct _tmp347;_tmp347.tag=Cyc_Core_Impossible;
_tmp347.f1=({const char*_tmp348="left deref of non-pointer-type";_tag_dynforward(
_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,31));});_tmp347;});_tmp346;}));
_LL20D:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp349=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp34A=inflow;struct Cyc_Dict_Dict _tmp34B;struct Cyc_List_List*
_tmp34C;_LL225: if((_tmp34A.BottomFL).tag != 0)goto _LL227;_LL226: return({struct
_tuple7 _tmp34D;_tmp34D.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp34F;(_tmp34F.BottomFL).tag=0;_tmp34F;});_tmp34D.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp34E;(_tmp34E.UnknownL).tag=1;_tmp34E;});
_tmp34D;});_LL227: if((_tmp34A.ReachableFL).tag != 1)goto _LL224;_tmp34B=(_tmp34A.ReachableFL).f1;
_tmp34C=(_tmp34A.ReachableFL).f2;_LL228: d=_tmp34B;_LL224:;}{void*_tmp350=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp351;struct Cyc_Absyn_Exp*_tmp352;struct Cyc_Absyn_Exp*
_tmp353;void*_tmp354;struct Cyc_Absyn_Vardecl*_tmp355;void*_tmp356;struct Cyc_Absyn_Vardecl*
_tmp357;void*_tmp358;struct Cyc_Absyn_Vardecl*_tmp359;void*_tmp35A;struct Cyc_Absyn_Vardecl*
_tmp35B;struct Cyc_Absyn_Exp*_tmp35C;struct _dynforward_ptr*_tmp35D;struct Cyc_Absyn_Exp*
_tmp35E;struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_Absyn_Exp*_tmp360;struct Cyc_Absyn_Exp*
_tmp361;struct _dynforward_ptr*_tmp362;_LL22A: if(*((int*)_tmp350)!= 15)goto _LL22C;
_tmp351=((struct Cyc_Absyn_Cast_e_struct*)_tmp350)->f2;_LL22B: _tmp352=_tmp351;
goto _LL22D;_LL22C: if(*((int*)_tmp350)!= 13)goto _LL22E;_tmp352=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp350)->f1;_LL22D: _tmp353=_tmp352;goto _LL22F;_LL22E: if(*((int*)_tmp350)!= 14)
goto _LL230;_tmp353=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp350)->f1;_LL22F:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp353,flds);_LL230: if(*((int*)
_tmp350)!= 1)goto _LL232;_tmp354=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp350)->f2;
if(_tmp354 <= (void*)1)goto _LL232;if(*((int*)_tmp354)!= 2)goto _LL232;_tmp355=((
struct Cyc_Absyn_Param_b_struct*)_tmp354)->f1;_LL231: _tmp357=_tmp355;goto _LL233;
_LL232: if(*((int*)_tmp350)!= 1)goto _LL234;_tmp356=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp350)->f2;if(_tmp356 <= (void*)1)goto _LL234;if(*((int*)_tmp356)!= 3)goto _LL234;
_tmp357=((struct Cyc_Absyn_Local_b_struct*)_tmp356)->f1;_LL233: _tmp359=_tmp357;
goto _LL235;_LL234: if(*((int*)_tmp350)!= 1)goto _LL236;_tmp358=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp350)->f2;if(_tmp358 <= (void*)1)goto _LL236;if(*((int*)_tmp358)!= 4)goto _LL236;
_tmp359=((struct Cyc_Absyn_Pat_b_struct*)_tmp358)->f1;_LL235: return({struct
_tuple7 _tmp363;_tmp363.f1=inflow;_tmp363.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp364;(_tmp364.PlaceL).tag=0;(_tmp364.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp365=_region_malloc(env->r,sizeof(*_tmp365));
_tmp365->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp366=
_region_malloc(env->r,sizeof(*_tmp366));_tmp366[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp367;_tmp367.tag=0;_tmp367.f1=_tmp359;_tmp367;});_tmp366;}));_tmp365->fields=
flds;_tmp365;});_tmp364;});_tmp363;});_LL236: if(*((int*)_tmp350)!= 1)goto _LL238;
_tmp35A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp350)->f2;if(_tmp35A <= (void*)
1)goto _LL238;if(*((int*)_tmp35A)!= 0)goto _LL238;_tmp35B=((struct Cyc_Absyn_Global_b_struct*)
_tmp35A)->f1;_LL237: return({struct _tuple7 _tmp368;_tmp368.f1=inflow;_tmp368.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp369;(
_tmp369.UnknownL).tag=1;_tmp369;});_tmp368;});_LL238: if(*((int*)_tmp350)!= 24)
goto _LL23A;_tmp35C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp350)->f1;_tmp35D=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp350)->f2;_LL239:{void*_tmp36A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp35C->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp36B;void*_tmp36C;_LL243: if(_tmp36A <= (void*)4)goto _LL245;if(*((int*)_tmp36A)
!= 4)goto _LL245;_tmp36B=((struct Cyc_Absyn_PointerType_struct*)_tmp36A)->f1;
_tmp36C=(void*)_tmp36B.elt_typ;_LL244: if(!Cyc_Absyn_is_union_type(_tmp36C))flds=({
struct Cyc_List_List*_tmp36D=_region_malloc(env->r,sizeof(*_tmp36D));_tmp36D->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp36C,_tmp35D);_tmp36D->tl=flds;
_tmp36D;});goto _LL242;_LL245:;_LL246:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E[0]=({struct Cyc_Core_Impossible_struct
_tmp36F;_tmp36F.tag=Cyc_Core_Impossible;_tmp36F.f1=({const char*_tmp370="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,25));});_tmp36F;});
_tmp36E;}));_LL242:;}_tmp35E=_tmp35C;goto _LL23B;_LL23A: if(*((int*)_tmp350)!= 22)
goto _LL23C;_tmp35E=((struct Cyc_Absyn_Deref_e_struct*)_tmp350)->f1;_LL23B: {union
Cyc_CfFlowInfo_FlowInfo_union _tmp372;void*_tmp373;struct _tuple5 _tmp371=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp35E);_tmp372=_tmp371.f1;_tmp373=_tmp371.f2;_tmp372=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp372);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp372,_tmp35E,
_tmp373,flds);}_LL23C: if(*((int*)_tmp350)!= 25)goto _LL23E;_tmp35F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp350)->f1;_tmp360=((struct Cyc_Absyn_Subscript_e_struct*)_tmp350)->f2;_LL23D: {
void*_tmp374=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp35F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp375;struct Cyc_Absyn_PtrAtts
_tmp376;struct Cyc_Absyn_Conref*_tmp377;_LL248: if(_tmp374 <= (void*)4)goto _LL24C;
if(*((int*)_tmp374)!= 9)goto _LL24A;_LL249: {unsigned int _tmp378=(Cyc_Evexp_eval_const_uint_exp(
_tmp360)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp35F,({struct
Cyc_List_List*_tmp379=_region_malloc(env->r,sizeof(*_tmp379));_tmp379->hd=(void*)((
int)_tmp378);_tmp379->tl=flds;_tmp379;}));}_LL24A: if(*((int*)_tmp374)!= 4)goto
_LL24C;_tmp375=((struct Cyc_Absyn_PointerType_struct*)_tmp374)->f1;_tmp376=
_tmp375.ptr_atts;_tmp377=_tmp376.bounds;_LL24B: {struct _RegionHandle _tmp37A=
_new_region("temp");struct _RegionHandle*temp=& _tmp37A;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp37D;struct Cyc_List_List*_tmp37E;struct _tuple11
_tmp37C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp37B[2];_tmp37B[1]=_tmp360;_tmp37B[0]=_tmp35F;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp37B,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp37D=_tmp37C.f1;_tmp37E=
_tmp37C.f2;_tmp37D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp360->loc,_tmp37D);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp37F=_tmp37D;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp380=_tmp37D;struct Cyc_Dict_Dict _tmp381;struct Cyc_List_List*_tmp382;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp383;_LL24F: if((_tmp380.ReachableFL).tag != 1)goto _LL251;_tmp381=(_tmp380.ReachableFL).f1;
_tmp382=(_tmp380.ReachableFL).f2;_tmp383=(_tmp380.ReachableFL).f3;_LL250: if(Cyc_CfFlowInfo_initlevel(
_tmp381,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp37E))->tl))->hd)== (void*)0)({void*_tmp384[0]={};Cyc_Tcutil_terr(
_tmp360->loc,({const char*_tmp385="expression may not be initialized";
_tag_dynforward(_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,34));}),
_tag_dynforward(_tmp384,sizeof(void*),0));});{struct Cyc_List_List*_tmp386=Cyc_NewControlFlow_add_subscript_reln(
_tmp349->r,_tmp382,_tmp35F,_tmp360);if(_tmp382 != _tmp386)_tmp37F=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp387;(_tmp387.ReachableFL).tag=1;(_tmp387.ReachableFL).f1=
_tmp381;(_tmp387.ReachableFL).f2=_tmp386;(_tmp387.ReachableFL).f3=_tmp383;
_tmp387;});goto _LL24E;}_LL251:;_LL252: goto _LL24E;_LL24E:;}{union Cyc_Absyn_Constraint_union
_tmp388=(Cyc_Absyn_compress_conref(_tmp377))->v;void*_tmp389;_LL254: if((_tmp388.Eq_constr).tag
!= 0)goto _LL256;_tmp389=(_tmp388.Eq_constr).f1;if(_tmp389 <= (void*)2)goto _LL256;
if(*((int*)_tmp389)!= 1)goto _LL256;_LL255:{void*_tmp38A=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp37E))->tl))->hd;struct Cyc_List_List*
_tmp38B;_LL259: if(_tmp38A <= (void*)3)goto _LL25B;if(*((int*)_tmp38A)!= 3)goto
_LL25B;_tmp38B=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp38A)->f1;_LL25A:(void*)(
_tmp360->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp38C=
_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp38D;_tmp38D.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp38D.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp38B);_tmp38D;});_tmp38C;})));goto _LL258;_LL25B:;_LL25C:
goto _LL258;_LL258:;}goto _LL253;_LL256:;_LL257: goto _LL253;_LL253:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp38F;union Cyc_CfFlowInfo_AbsLVal_union _tmp390;struct _tuple7 _tmp38E=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp37D,_tmp35F,(void*)((struct Cyc_List_List*)_check_null(_tmp37E))->hd,
flds);_tmp38F=_tmp38E.f1;_tmp390=_tmp38E.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp391=_tmp38F;_LL25E: if((_tmp391.BottomFL).tag != 0)goto _LL260;_LL25F: {struct
_tuple7 _tmp393=({struct _tuple7 _tmp392;_tmp392.f1=_tmp38F;_tmp392.f2=_tmp390;
_tmp392;});_npop_handler(0);return _tmp393;}_LL260:;_LL261: {struct _tuple7 _tmp395=({
struct _tuple7 _tmp394;_tmp394.f1=_tmp37F;_tmp394.f2=_tmp390;_tmp394;});
_npop_handler(0);return _tmp395;}_LL25D:;}}}};_pop_region(temp);}_LL24C:;_LL24D:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp396=_cycalloc(sizeof(*
_tmp396));_tmp396[0]=({struct Cyc_Core_Impossible_struct _tmp397;_tmp397.tag=Cyc_Core_Impossible;
_tmp397.f1=({const char*_tmp398="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp398,sizeof(char),_get_zero_arr_size(_tmp398,33));});_tmp397;});
_tmp396;}));_LL247:;}_LL23E: if(*((int*)_tmp350)!= 23)goto _LL240;_tmp361=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp350)->f1;_tmp362=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp350)->f2;_LL23F: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp361->topt))->v))return({struct _tuple7 _tmp399;_tmp399.f1=inflow;
_tmp399.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp39A;(_tmp39A.UnknownL).tag=1;_tmp39A;});_tmp399;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp361,({struct Cyc_List_List*_tmp39B=_region_malloc(env->r,sizeof(*
_tmp39B));_tmp39B->hd=(void*)Cyc_NewControlFlow_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp361->topt))->v,_tmp362);_tmp39B->tl=flds;_tmp39B;}));
_LL240:;_LL241: return({struct _tuple7 _tmp39C;_tmp39C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp39E;(_tmp39E.BottomFL).tag=0;_tmp39E;});
_tmp39C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp39D;(_tmp39D.UnknownL).tag=1;_tmp39D;});_tmp39C;});_LL229:;}}static struct
_tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A0;union Cyc_CfFlowInfo_AbsLVal_union _tmp3A1;struct _tuple7 _tmp39F=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp3A0=_tmp39F.f1;_tmp3A1=_tmp39F.f2;return({struct _tuple7
_tmp3A2;_tmp3A2.f1=_tmp3A0;_tmp3A2.f2=_tmp3A1;_tmp3A2;});}static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp3A3=env->fenv;{void*
_tmp3A4=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;
struct Cyc_Absyn_Exp*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A8;struct Cyc_Absyn_Exp*
_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AB;struct Cyc_Absyn_Exp*
_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AD;void*_tmp3AE;struct Cyc_List_List*_tmp3AF;
struct Cyc_List_List _tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_List_List*
_tmp3B2;void*_tmp3B3;struct Cyc_List_List*_tmp3B4;_LL263: if(*((int*)_tmp3A4)!= 6)
goto _LL265;_tmp3A5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3A4)->f1;_tmp3A6=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp3A4)->f2;_tmp3A7=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3A4)->f3;_LL264: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3B6;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3B7;struct _tuple8 _tmp3B5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3A5);
_tmp3B6=_tmp3B5.f1;_tmp3B7=_tmp3B5.f2;_tmp3B6=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3A5->loc,_tmp3B6);_tmp3B7=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3A5->loc,
_tmp3B7);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3B9;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3BA;struct _tuple8 _tmp3B8=Cyc_NewControlFlow_anal_test(env,_tmp3B6,_tmp3A6);
_tmp3B9=_tmp3B8.f1;_tmp3BA=_tmp3B8.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3BC;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3BD;struct _tuple8 _tmp3BB=Cyc_NewControlFlow_anal_test(
env,_tmp3B7,_tmp3A7);_tmp3BC=_tmp3BB.f1;_tmp3BD=_tmp3BB.f2;return({struct _tuple8
_tmp3BE;_tmp3BE.f1=Cyc_CfFlowInfo_join_flow(_tmp3A3,env->all_changed,_tmp3B9,
_tmp3BC,1);_tmp3BE.f2=Cyc_CfFlowInfo_join_flow(_tmp3A3,env->all_changed,_tmp3BA,
_tmp3BD,1);_tmp3BE;});}}}_LL265: if(*((int*)_tmp3A4)!= 7)goto _LL267;_tmp3A8=((
struct Cyc_Absyn_And_e_struct*)_tmp3A4)->f1;_tmp3A9=((struct Cyc_Absyn_And_e_struct*)
_tmp3A4)->f2;_LL266: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3C0;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C1;struct _tuple8 _tmp3BF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3A8);
_tmp3C0=_tmp3BF.f1;_tmp3C1=_tmp3BF.f2;_tmp3C0=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3A8->loc,_tmp3C0);_tmp3C1=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3A8->loc,
_tmp3C1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3C3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C4;struct _tuple8 _tmp3C2=Cyc_NewControlFlow_anal_test(env,_tmp3C0,_tmp3A9);
_tmp3C3=_tmp3C2.f1;_tmp3C4=_tmp3C2.f2;_tmp3C3=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3A9->loc,_tmp3C3);_tmp3C4=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3A9->loc,
_tmp3C4);return({struct _tuple8 _tmp3C5;_tmp3C5.f1=_tmp3C3;_tmp3C5.f2=Cyc_CfFlowInfo_join_flow(
_tmp3A3,env->all_changed,_tmp3C1,_tmp3C4,0);_tmp3C5;});}}_LL267: if(*((int*)
_tmp3A4)!= 8)goto _LL269;_tmp3AA=((struct Cyc_Absyn_Or_e_struct*)_tmp3A4)->f1;
_tmp3AB=((struct Cyc_Absyn_Or_e_struct*)_tmp3A4)->f2;_LL268: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C7;union Cyc_CfFlowInfo_FlowInfo_union _tmp3C8;struct _tuple8 _tmp3C6=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3AA);_tmp3C7=_tmp3C6.f1;_tmp3C8=_tmp3C6.f2;_tmp3C7=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3AA->loc,_tmp3C7);_tmp3C8=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3AA->loc,
_tmp3C8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CA;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CB;struct _tuple8 _tmp3C9=Cyc_NewControlFlow_anal_test(env,_tmp3C8,_tmp3AB);
_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;_tmp3CA=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3AB->loc,_tmp3CA);_tmp3CB=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3AB->loc,
_tmp3CB);return({struct _tuple8 _tmp3CC;_tmp3CC.f1=Cyc_CfFlowInfo_join_flow(
_tmp3A3,env->all_changed,_tmp3C7,_tmp3CA,0);_tmp3CC.f2=_tmp3CB;_tmp3CC;});}}
_LL269: if(*((int*)_tmp3A4)!= 9)goto _LL26B;_tmp3AC=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3A4)->f1;_tmp3AD=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3A4)->f2;_LL26A: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3CE;void*_tmp3CF;struct _tuple5 _tmp3CD=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3AC);_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;_tmp3CE=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3AC->loc,_tmp3CE);return Cyc_NewControlFlow_anal_test(env,_tmp3CE,_tmp3AD);}
_LL26B: if(*((int*)_tmp3A4)!= 3)goto _LL26D;_tmp3AE=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3A4)->f1;if((int)_tmp3AE != 11)goto _LL26D;_tmp3AF=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3A4)->f2;if(_tmp3AF == 0)goto _LL26D;_tmp3B0=*_tmp3AF;_tmp3B1=(struct Cyc_Absyn_Exp*)
_tmp3B0.hd;_tmp3B2=_tmp3B0.tl;if(_tmp3B2 != 0)goto _LL26D;_LL26C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D1;union Cyc_CfFlowInfo_FlowInfo_union _tmp3D2;struct _tuple8 _tmp3D0=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3B1);_tmp3D1=_tmp3D0.f1;_tmp3D2=_tmp3D0.f2;return({struct _tuple8
_tmp3D3;_tmp3D3.f1=_tmp3D2;_tmp3D3.f2=_tmp3D1;_tmp3D3;});}_LL26D: if(*((int*)
_tmp3A4)!= 3)goto _LL26F;_tmp3B3=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3A4)->f1;_tmp3B4=((struct Cyc_Absyn_Primop_e_struct*)_tmp3A4)->f2;_LL26E: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3D4=
_new_region("temp");struct _RegionHandle*temp=& _tmp3D4;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3D6;struct Cyc_List_List*_tmp3D7;struct _tuple11
_tmp3D5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3B4,0);
_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D5.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3D7))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3D7->tl))->hd;
f=_tmp3D6;};_pop_region(temp);}{void*_tmp3D8=_tmp3B3;_LL272: if((int)_tmp3D8 != 0)
goto _LL274;_LL273: goto _LL275;_LL274: if((int)_tmp3D8 != 2)goto _LL276;_LL275: Cyc_CfFlowInfo_check_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3B4))->hd)->loc,f);
goto _LL271;_LL276:;_LL277: f=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3B4))->hd)->loc,f);goto _LL271;_LL271:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D9=f;struct Cyc_Dict_Dict _tmp3DA;struct Cyc_List_List*
_tmp3DB;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3DC;_LL279: if((_tmp3D9.BottomFL).tag
!= 0)goto _LL27B;_LL27A: return({struct _tuple8 _tmp3DD;_tmp3DD.f1=f;_tmp3DD.f2=f;
_tmp3DD;});_LL27B: if((_tmp3D9.ReachableFL).tag != 1)goto _LL278;_tmp3DA=(_tmp3D9.ReachableFL).f1;
_tmp3DB=(_tmp3D9.ReachableFL).f2;_tmp3DC=(_tmp3D9.ReachableFL).f3;_LL27C: {
struct Cyc_Absyn_Exp*_tmp3DE=(struct Cyc_Absyn_Exp*)_tmp3B4->hd;struct Cyc_Absyn_Exp*
_tmp3DF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3B4->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(_tmp3DA,r1)== (void*)0)({void*_tmp3E0[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp3B4->hd)->loc,({const char*_tmp3E1="expression may not be initialized";
_tag_dynforward(_tmp3E1,sizeof(char),_get_zero_arr_size(_tmp3E1,34));}),
_tag_dynforward(_tmp3E0,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp3DA,
r2)== (void*)0)({void*_tmp3E2[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3B4->tl))->hd)->loc,({const char*_tmp3E3="expression may not be initialized";
_tag_dynforward(_tmp3E3,sizeof(char),_get_zero_arr_size(_tmp3E3,34));}),
_tag_dynforward(_tmp3E2,sizeof(void*),0));});if(_tmp3B3 == (void*)5  || _tmp3B3 == (
void*)6){struct _tuple0 _tmp3E5=({struct _tuple0 _tmp3E4;_tmp3E4.f1=r1;_tmp3E4.f2=r2;
_tmp3E4;});void*_tmp3E6;void*_tmp3E7;void*_tmp3E8;void*_tmp3E9;void*_tmp3EA;void*
_tmp3EB;void*_tmp3EC;void*_tmp3ED;void*_tmp3EE;void*_tmp3EF;void*_tmp3F0;void*
_tmp3F1;void*_tmp3F2;void*_tmp3F3;void*_tmp3F4;void*_tmp3F5;void*_tmp3F6;void*
_tmp3F7;void*_tmp3F8;void*_tmp3F9;_LL27E: _tmp3E6=_tmp3E5.f1;if(_tmp3E6 <= (void*)
3)goto _LL280;if(*((int*)_tmp3E6)!= 0)goto _LL280;_tmp3E7=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3E6)->f1;_tmp3E8=_tmp3E5.f2;if((int)_tmp3E8 != 0)goto _LL280;_LL27F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3FB;union Cyc_CfFlowInfo_FlowInfo_union _tmp3FC;struct _tuple8 _tmp3FA=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3DE,_tmp3E7);_tmp3FB=_tmp3FA.f1;_tmp3FC=_tmp3FA.f2;{void*_tmp3FD=
_tmp3B3;_LL293: if((int)_tmp3FD != 5)goto _LL295;_LL294: return({struct _tuple8
_tmp3FE;_tmp3FE.f1=_tmp3FC;_tmp3FE.f2=_tmp3FB;_tmp3FE;});_LL295: if((int)_tmp3FD
!= 6)goto _LL297;_LL296: return({struct _tuple8 _tmp3FF;_tmp3FF.f1=_tmp3FB;_tmp3FF.f2=
_tmp3FC;_tmp3FF;});_LL297:;_LL298:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400[0]=({struct Cyc_Core_Impossible_struct
_tmp401;_tmp401.tag=Cyc_Core_Impossible;_tmp401.f1=({const char*_tmp402="anal_test, zero-split";
_tag_dynforward(_tmp402,sizeof(char),_get_zero_arr_size(_tmp402,22));});_tmp401;});
_tmp400;}));_LL292:;}}_LL280: _tmp3E9=_tmp3E5.f1;if((int)_tmp3E9 != 0)goto _LL282;
_tmp3EA=_tmp3E5.f2;if(_tmp3EA <= (void*)3)goto _LL282;if(*((int*)_tmp3EA)!= 0)goto
_LL282;_tmp3EB=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3EA)->f1;
_LL281: {union Cyc_CfFlowInfo_FlowInfo_union _tmp404;union Cyc_CfFlowInfo_FlowInfo_union
_tmp405;struct _tuple8 _tmp403=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3DF,
_tmp3EB);_tmp404=_tmp403.f1;_tmp405=_tmp403.f2;{void*_tmp406=_tmp3B3;_LL29A: if((
int)_tmp406 != 5)goto _LL29C;_LL29B: return({struct _tuple8 _tmp407;_tmp407.f1=
_tmp405;_tmp407.f2=_tmp404;_tmp407;});_LL29C: if((int)_tmp406 != 6)goto _LL29E;
_LL29D: return({struct _tuple8 _tmp408;_tmp408.f1=_tmp404;_tmp408.f2=_tmp405;
_tmp408;});_LL29E:;_LL29F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409[0]=({struct Cyc_Core_Impossible_struct
_tmp40A;_tmp40A.tag=Cyc_Core_Impossible;_tmp40A.f1=({const char*_tmp40B="anal_test, zero-split";
_tag_dynforward(_tmp40B,sizeof(char),_get_zero_arr_size(_tmp40B,22));});_tmp40A;});
_tmp409;}));_LL299:;}}_LL282: _tmp3EC=_tmp3E5.f1;if((int)_tmp3EC != 0)goto _LL284;
_tmp3ED=_tmp3E5.f2;if((int)_tmp3ED != 0)goto _LL284;_LL283: if(_tmp3B3 == (void*)5)
return({struct _tuple8 _tmp40C;_tmp40C.f1=f;_tmp40C.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp40D;(_tmp40D.BottomFL).tag=0;_tmp40D;});
_tmp40C;});else{return({struct _tuple8 _tmp40E;_tmp40E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp40F;(_tmp40F.BottomFL).tag=0;_tmp40F;});
_tmp40E.f2=f;_tmp40E;});}_LL284: _tmp3EE=_tmp3E5.f1;if((int)_tmp3EE != 0)goto
_LL286;_tmp3EF=_tmp3E5.f2;if((int)_tmp3EF != 1)goto _LL286;_LL285: goto _LL287;
_LL286: _tmp3F0=_tmp3E5.f1;if((int)_tmp3F0 != 0)goto _LL288;_tmp3F1=_tmp3E5.f2;if((
int)_tmp3F1 != 2)goto _LL288;_LL287: goto _LL289;_LL288: _tmp3F2=_tmp3E5.f1;if((int)
_tmp3F2 != 0)goto _LL28A;_tmp3F3=_tmp3E5.f2;if(_tmp3F3 <= (void*)3)goto _LL28A;if(*((
int*)_tmp3F3)!= 2)goto _LL28A;_LL289: goto _LL28B;_LL28A: _tmp3F4=_tmp3E5.f1;if((int)
_tmp3F4 != 1)goto _LL28C;_tmp3F5=_tmp3E5.f2;if((int)_tmp3F5 != 0)goto _LL28C;_LL28B:
goto _LL28D;_LL28C: _tmp3F6=_tmp3E5.f1;if((int)_tmp3F6 != 2)goto _LL28E;_tmp3F7=
_tmp3E5.f2;if((int)_tmp3F7 != 0)goto _LL28E;_LL28D: goto _LL28F;_LL28E: _tmp3F8=
_tmp3E5.f1;if(_tmp3F8 <= (void*)3)goto _LL290;if(*((int*)_tmp3F8)!= 2)goto _LL290;
_tmp3F9=_tmp3E5.f2;if((int)_tmp3F9 != 0)goto _LL290;_LL28F: if(_tmp3B3 == (void*)6)
return({struct _tuple8 _tmp410;_tmp410.f1=f;_tmp410.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp411;(_tmp411.BottomFL).tag=0;_tmp411;});
_tmp410;});else{return({struct _tuple8 _tmp412;_tmp412.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp413;(_tmp413.BottomFL).tag=0;_tmp413;});
_tmp412.f2=f;_tmp412;});}_LL290:;_LL291: goto _LL27D;_LL27D:;}{struct _tuple0
_tmp415=({struct _tuple0 _tmp414;_tmp414.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3DE->topt))->v);_tmp414.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3DF->topt))->v);_tmp414;});void*_tmp416;void*
_tmp417;void*_tmp418;void*_tmp419;void*_tmp41A;void*_tmp41B;_LL2A1: _tmp416=
_tmp415.f1;if(_tmp416 <= (void*)4)goto _LL2A3;if(*((int*)_tmp416)!= 5)goto _LL2A3;
_tmp417=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp416)->f1;if((int)_tmp417 != 
1)goto _LL2A3;_LL2A2: goto _LL2A4;_LL2A3: _tmp418=_tmp415.f2;if(_tmp418 <= (void*)4)
goto _LL2A5;if(*((int*)_tmp418)!= 5)goto _LL2A5;_tmp419=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp418)->f1;if((int)_tmp419 != 1)goto _LL2A5;_LL2A4: goto _LL2A6;_LL2A5: _tmp41A=
_tmp415.f1;if(_tmp41A <= (void*)4)goto _LL2A7;if(*((int*)_tmp41A)!= 18)goto _LL2A7;
_LL2A6: goto _LL2A8;_LL2A7: _tmp41B=_tmp415.f2;if(_tmp41B <= (void*)4)goto _LL2A9;if(*((
int*)_tmp41B)!= 18)goto _LL2A9;_LL2A8: goto _LL2A0;_LL2A9:;_LL2AA: return({struct
_tuple8 _tmp41C;_tmp41C.f1=f;_tmp41C.f2=f;_tmp41C;});_LL2A0:;}{void*_tmp41D=
_tmp3B3;_LL2AC: if((int)_tmp41D != 5)goto _LL2AE;_LL2AD: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp41E=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3DE,r1,(void*)5,r2);_tmp41E=Cyc_NewControlFlow_if_tagcmp(
env,_tmp41E,_tmp3DF,r2,(void*)5,r1);return({struct _tuple8 _tmp41F;_tmp41F.f1=
_tmp41E;_tmp41F.f2=f;_tmp41F;});}_LL2AE: if((int)_tmp41D != 6)goto _LL2B0;_LL2AF: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp420=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3DE,r1,(void*)5,r2);_tmp420=Cyc_NewControlFlow_if_tagcmp(env,_tmp420,_tmp3DF,
r2,(void*)5,r1);return({struct _tuple8 _tmp421;_tmp421.f1=f;_tmp421.f2=_tmp420;
_tmp421;});}_LL2B0: if((int)_tmp41D != 7)goto _LL2B2;_LL2B1: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp422=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3DF,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp423=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3DE,r1,(void*)10,r2);return({
struct _tuple8 _tmp424;_tmp424.f1=_tmp422;_tmp424.f2=_tmp423;_tmp424;});}_LL2B2:
if((int)_tmp41D != 9)goto _LL2B4;_LL2B3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp425=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3DF,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp426=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3DE,r1,(void*)8,r2);return({
struct _tuple8 _tmp427;_tmp427.f1=_tmp425;_tmp427.f2=_tmp426;_tmp427;});}_LL2B4:
if((int)_tmp41D != 8)goto _LL2B6;_LL2B5: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp428=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3DE,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp429=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3DF,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp42A=_tmp428;struct Cyc_Dict_Dict _tmp42B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp42C;_LL2BB: if((_tmp42A.BottomFL).tag != 0)goto _LL2BD;_LL2BC:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D[0]=({
struct Cyc_Core_Impossible_struct _tmp42E;_tmp42E.tag=Cyc_Core_Impossible;_tmp42E.f1=({
const char*_tmp42F="anal_test, Lt";_tag_dynforward(_tmp42F,sizeof(char),
_get_zero_arr_size(_tmp42F,14));});_tmp42E;});_tmp42D;}));_LL2BD: if((_tmp42A.ReachableFL).tag
!= 1)goto _LL2BA;_tmp42B=(_tmp42A.ReachableFL).f1;_tmp42C=(_tmp42A.ReachableFL).f3;
_LL2BE: _tmp3DA=_tmp42B;_tmp3DC=_tmp42C;_LL2BA:;}{void*_tmp430=(void*)_tmp3DE->r;
void*_tmp431;struct Cyc_Absyn_Vardecl*_tmp432;void*_tmp433;struct Cyc_Absyn_Vardecl*
_tmp434;void*_tmp435;struct Cyc_Absyn_Vardecl*_tmp436;void*_tmp437;struct Cyc_Absyn_Vardecl*
_tmp438;_LL2C0: if(*((int*)_tmp430)!= 1)goto _LL2C2;_tmp431=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp430)->f2;if(_tmp431 <= (void*)1)goto _LL2C2;if(*((int*)_tmp431)!= 0)goto _LL2C2;
_tmp432=((struct Cyc_Absyn_Global_b_struct*)_tmp431)->f1;if(!(!_tmp432->escapes))
goto _LL2C2;_LL2C1: _tmp434=_tmp432;goto _LL2C3;_LL2C2: if(*((int*)_tmp430)!= 1)goto
_LL2C4;_tmp433=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp430)->f2;if(_tmp433 <= (
void*)1)goto _LL2C4;if(*((int*)_tmp433)!= 3)goto _LL2C4;_tmp434=((struct Cyc_Absyn_Local_b_struct*)
_tmp433)->f1;if(!(!_tmp434->escapes))goto _LL2C4;_LL2C3: _tmp436=_tmp434;goto
_LL2C5;_LL2C4: if(*((int*)_tmp430)!= 1)goto _LL2C6;_tmp435=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp430)->f2;if(_tmp435 <= (void*)1)goto _LL2C6;if(*((int*)_tmp435)!= 4)goto _LL2C6;
_tmp436=((struct Cyc_Absyn_Pat_b_struct*)_tmp435)->f1;if(!(!_tmp436->escapes))
goto _LL2C6;_LL2C5: _tmp438=_tmp436;goto _LL2C7;_LL2C6: if(*((int*)_tmp430)!= 1)goto
_LL2C8;_tmp437=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp430)->f2;if(_tmp437 <= (
void*)1)goto _LL2C8;if(*((int*)_tmp437)!= 2)goto _LL2C8;_tmp438=((struct Cyc_Absyn_Param_b_struct*)
_tmp437)->f1;if(!(!_tmp438->escapes))goto _LL2C8;_LL2C7: {void*_tmp439=(void*)
_tmp3DF->r;void*_tmp43A;struct Cyc_Absyn_Vardecl*_tmp43B;void*_tmp43C;struct Cyc_Absyn_Vardecl*
_tmp43D;void*_tmp43E;struct Cyc_Absyn_Vardecl*_tmp43F;void*_tmp440;struct Cyc_Absyn_Vardecl*
_tmp441;union Cyc_Absyn_Cnst_union _tmp442;int _tmp443;void*_tmp444;struct Cyc_List_List*
_tmp445;struct Cyc_List_List _tmp446;struct Cyc_Absyn_Exp*_tmp447;_LL2CB: if(*((int*)
_tmp439)!= 1)goto _LL2CD;_tmp43A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp439)->f2;
if(_tmp43A <= (void*)1)goto _LL2CD;if(*((int*)_tmp43A)!= 0)goto _LL2CD;_tmp43B=((
struct Cyc_Absyn_Global_b_struct*)_tmp43A)->f1;if(!(!_tmp43B->escapes))goto _LL2CD;
_LL2CC: _tmp43D=_tmp43B;goto _LL2CE;_LL2CD: if(*((int*)_tmp439)!= 1)goto _LL2CF;
_tmp43C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp439)->f2;if(_tmp43C <= (void*)
1)goto _LL2CF;if(*((int*)_tmp43C)!= 3)goto _LL2CF;_tmp43D=((struct Cyc_Absyn_Local_b_struct*)
_tmp43C)->f1;if(!(!_tmp43D->escapes))goto _LL2CF;_LL2CE: _tmp43F=_tmp43D;goto
_LL2D0;_LL2CF: if(*((int*)_tmp439)!= 1)goto _LL2D1;_tmp43E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp439)->f2;if(_tmp43E <= (void*)1)goto _LL2D1;if(*((int*)_tmp43E)!= 4)goto _LL2D1;
_tmp43F=((struct Cyc_Absyn_Pat_b_struct*)_tmp43E)->f1;if(!(!_tmp43F->escapes))
goto _LL2D1;_LL2D0: _tmp441=_tmp43F;goto _LL2D2;_LL2D1: if(*((int*)_tmp439)!= 1)goto
_LL2D3;_tmp440=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp439)->f2;if(_tmp440 <= (
void*)1)goto _LL2D3;if(*((int*)_tmp440)!= 2)goto _LL2D3;_tmp441=((struct Cyc_Absyn_Param_b_struct*)
_tmp440)->f1;if(!(!_tmp441->escapes))goto _LL2D3;_LL2D2: {struct _RegionHandle*
_tmp448=(env->fenv)->r;struct Cyc_List_List*_tmp449=({struct Cyc_List_List*_tmp44C=
_region_malloc(_tmp448,sizeof(*_tmp44C));_tmp44C->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp44D=_region_malloc(_tmp448,sizeof(*_tmp44D));_tmp44D->vd=_tmp438;_tmp44D->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp44E;(
_tmp44E.LessVar).tag=1;(_tmp44E.LessVar).f1=_tmp441;_tmp44E;});_tmp44D;});
_tmp44C->tl=_tmp3DB;_tmp44C;});return({struct _tuple8 _tmp44A;_tmp44A.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp44B;(_tmp44B.ReachableFL).tag=1;(_tmp44B.ReachableFL).f1=
_tmp3DA;(_tmp44B.ReachableFL).f2=_tmp449;(_tmp44B.ReachableFL).f3=_tmp3DC;
_tmp44B;});_tmp44A.f2=_tmp429;_tmp44A;});}_LL2D3: if(*((int*)_tmp439)!= 0)goto
_LL2D5;_tmp442=((struct Cyc_Absyn_Const_e_struct*)_tmp439)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp439)->f1).Int_c).tag != 2)goto _LL2D5;_tmp443=(_tmp442.Int_c).f2;_LL2D4: {
struct _RegionHandle*_tmp44F=(env->fenv)->r;struct Cyc_List_List*_tmp450=({struct
Cyc_List_List*_tmp453=_region_malloc(_tmp44F,sizeof(*_tmp453));_tmp453->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp454=_region_malloc(_tmp44F,sizeof(*_tmp454));
_tmp454->vd=_tmp438;_tmp454->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp455;(_tmp455.LessConst).tag=3;(_tmp455.LessConst).f1=(unsigned int)_tmp443;
_tmp455;});_tmp454;});_tmp453->tl=_tmp3DB;_tmp453;});return({struct _tuple8
_tmp451;_tmp451.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp452;(_tmp452.ReachableFL).tag=1;(_tmp452.ReachableFL).f1=_tmp3DA;(_tmp452.ReachableFL).f2=
_tmp450;(_tmp452.ReachableFL).f3=_tmp3DC;_tmp452;});_tmp451.f2=_tmp429;_tmp451;});}
_LL2D5: if(*((int*)_tmp439)!= 3)goto _LL2D7;_tmp444=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp439)->f1;if((int)_tmp444 != 19)goto _LL2D7;_tmp445=((struct Cyc_Absyn_Primop_e_struct*)
_tmp439)->f2;if(_tmp445 == 0)goto _LL2D7;_tmp446=*_tmp445;_tmp447=(struct Cyc_Absyn_Exp*)
_tmp446.hd;_LL2D6: {void*_tmp456=(void*)_tmp447->r;void*_tmp457;struct Cyc_Absyn_Vardecl*
_tmp458;void*_tmp459;struct Cyc_Absyn_Vardecl*_tmp45A;void*_tmp45B;struct Cyc_Absyn_Vardecl*
_tmp45C;void*_tmp45D;struct Cyc_Absyn_Vardecl*_tmp45E;_LL2DA: if(*((int*)_tmp456)
!= 1)goto _LL2DC;_tmp457=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp456)->f2;if(
_tmp457 <= (void*)1)goto _LL2DC;if(*((int*)_tmp457)!= 0)goto _LL2DC;_tmp458=((
struct Cyc_Absyn_Global_b_struct*)_tmp457)->f1;if(!(!_tmp458->escapes))goto _LL2DC;
_LL2DB: _tmp45A=_tmp458;goto _LL2DD;_LL2DC: if(*((int*)_tmp456)!= 1)goto _LL2DE;
_tmp459=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp456)->f2;if(_tmp459 <= (void*)
1)goto _LL2DE;if(*((int*)_tmp459)!= 3)goto _LL2DE;_tmp45A=((struct Cyc_Absyn_Local_b_struct*)
_tmp459)->f1;if(!(!_tmp45A->escapes))goto _LL2DE;_LL2DD: _tmp45C=_tmp45A;goto
_LL2DF;_LL2DE: if(*((int*)_tmp456)!= 1)goto _LL2E0;_tmp45B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp456)->f2;if(_tmp45B <= (void*)1)goto _LL2E0;if(*((int*)_tmp45B)!= 4)goto _LL2E0;
_tmp45C=((struct Cyc_Absyn_Pat_b_struct*)_tmp45B)->f1;if(!(!_tmp45C->escapes))
goto _LL2E0;_LL2DF: _tmp45E=_tmp45C;goto _LL2E1;_LL2E0: if(*((int*)_tmp456)!= 1)goto
_LL2E2;_tmp45D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp456)->f2;if(_tmp45D <= (
void*)1)goto _LL2E2;if(*((int*)_tmp45D)!= 2)goto _LL2E2;_tmp45E=((struct Cyc_Absyn_Param_b_struct*)
_tmp45D)->f1;if(!(!_tmp45E->escapes))goto _LL2E2;_LL2E1: {struct _RegionHandle*
_tmp45F=(env->fenv)->r;struct Cyc_List_List*_tmp460=({struct Cyc_List_List*_tmp463=
_region_malloc(_tmp45F,sizeof(*_tmp463));_tmp463->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp464=_region_malloc(_tmp45F,sizeof(*_tmp464));_tmp464->vd=_tmp438;_tmp464->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp465;(
_tmp465.LessSize).tag=2;(_tmp465.LessSize).f1=_tmp45E;_tmp465;});_tmp464;});
_tmp463->tl=_tmp3DB;_tmp463;});return({struct _tuple8 _tmp461;_tmp461.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp462;(_tmp462.ReachableFL).tag=1;(_tmp462.ReachableFL).f1=
_tmp3DA;(_tmp462.ReachableFL).f2=_tmp460;(_tmp462.ReachableFL).f3=_tmp3DC;
_tmp462;});_tmp461.f2=_tmp429;_tmp461;});}_LL2E2:;_LL2E3: return({struct _tuple8
_tmp466;_tmp466.f1=_tmp428;_tmp466.f2=_tmp429;_tmp466;});_LL2D9:;}_LL2D7:;_LL2D8:
return({struct _tuple8 _tmp467;_tmp467.f1=_tmp428;_tmp467.f2=_tmp429;_tmp467;});
_LL2CA:;}_LL2C8:;_LL2C9: return({struct _tuple8 _tmp468;_tmp468.f1=_tmp428;_tmp468.f2=
_tmp429;_tmp468;});_LL2BF:;}}_LL2B6: if((int)_tmp41D != 10)goto _LL2B8;_LL2B7: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp469=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3DE,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp46A=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3DF,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp46B=_tmp469;
struct Cyc_Dict_Dict _tmp46C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp46D;_LL2E5: if((
_tmp46B.BottomFL).tag != 0)goto _LL2E7;_LL2E6:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E[0]=({struct Cyc_Core_Impossible_struct
_tmp46F;_tmp46F.tag=Cyc_Core_Impossible;_tmp46F.f1=({const char*_tmp470="anal_test, Lte";
_tag_dynforward(_tmp470,sizeof(char),_get_zero_arr_size(_tmp470,15));});_tmp46F;});
_tmp46E;}));_LL2E7: if((_tmp46B.ReachableFL).tag != 1)goto _LL2E4;_tmp46C=(_tmp46B.ReachableFL).f1;
_tmp46D=(_tmp46B.ReachableFL).f3;_LL2E8: _tmp3DA=_tmp46C;_tmp3DC=_tmp46D;_LL2E4:;}{
void*_tmp471=(void*)_tmp3DE->r;void*_tmp472;struct Cyc_Absyn_Vardecl*_tmp473;void*
_tmp474;struct Cyc_Absyn_Vardecl*_tmp475;void*_tmp476;struct Cyc_Absyn_Vardecl*
_tmp477;void*_tmp478;struct Cyc_Absyn_Vardecl*_tmp479;_LL2EA: if(*((int*)_tmp471)
!= 1)goto _LL2EC;_tmp472=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp471)->f2;if(
_tmp472 <= (void*)1)goto _LL2EC;if(*((int*)_tmp472)!= 0)goto _LL2EC;_tmp473=((
struct Cyc_Absyn_Global_b_struct*)_tmp472)->f1;if(!(!_tmp473->escapes))goto _LL2EC;
_LL2EB: _tmp475=_tmp473;goto _LL2ED;_LL2EC: if(*((int*)_tmp471)!= 1)goto _LL2EE;
_tmp474=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp471)->f2;if(_tmp474 <= (void*)
1)goto _LL2EE;if(*((int*)_tmp474)!= 3)goto _LL2EE;_tmp475=((struct Cyc_Absyn_Local_b_struct*)
_tmp474)->f1;if(!(!_tmp475->escapes))goto _LL2EE;_LL2ED: _tmp477=_tmp475;goto
_LL2EF;_LL2EE: if(*((int*)_tmp471)!= 1)goto _LL2F0;_tmp476=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp471)->f2;if(_tmp476 <= (void*)1)goto _LL2F0;if(*((int*)_tmp476)!= 4)goto _LL2F0;
_tmp477=((struct Cyc_Absyn_Pat_b_struct*)_tmp476)->f1;if(!(!_tmp477->escapes))
goto _LL2F0;_LL2EF: _tmp479=_tmp477;goto _LL2F1;_LL2F0: if(*((int*)_tmp471)!= 1)goto
_LL2F2;_tmp478=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp471)->f2;if(_tmp478 <= (
void*)1)goto _LL2F2;if(*((int*)_tmp478)!= 2)goto _LL2F2;_tmp479=((struct Cyc_Absyn_Param_b_struct*)
_tmp478)->f1;if(!(!_tmp479->escapes))goto _LL2F2;_LL2F1: {void*_tmp47A=(void*)
_tmp3DF->r;void*_tmp47B;struct Cyc_List_List*_tmp47C;struct Cyc_List_List _tmp47D;
struct Cyc_Absyn_Exp*_tmp47E;_LL2F5: if(*((int*)_tmp47A)!= 3)goto _LL2F7;_tmp47B=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp47A)->f1;if((int)_tmp47B != 19)goto
_LL2F7;_tmp47C=((struct Cyc_Absyn_Primop_e_struct*)_tmp47A)->f2;if(_tmp47C == 0)
goto _LL2F7;_tmp47D=*_tmp47C;_tmp47E=(struct Cyc_Absyn_Exp*)_tmp47D.hd;_LL2F6: {
void*_tmp47F=(void*)_tmp47E->r;void*_tmp480;struct Cyc_Absyn_Vardecl*_tmp481;void*
_tmp482;struct Cyc_Absyn_Vardecl*_tmp483;void*_tmp484;struct Cyc_Absyn_Vardecl*
_tmp485;void*_tmp486;struct Cyc_Absyn_Vardecl*_tmp487;_LL2FA: if(*((int*)_tmp47F)
!= 1)goto _LL2FC;_tmp480=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp47F)->f2;if(
_tmp480 <= (void*)1)goto _LL2FC;if(*((int*)_tmp480)!= 0)goto _LL2FC;_tmp481=((
struct Cyc_Absyn_Global_b_struct*)_tmp480)->f1;if(!(!_tmp481->escapes))goto _LL2FC;
_LL2FB: _tmp483=_tmp481;goto _LL2FD;_LL2FC: if(*((int*)_tmp47F)!= 1)goto _LL2FE;
_tmp482=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp47F)->f2;if(_tmp482 <= (void*)
1)goto _LL2FE;if(*((int*)_tmp482)!= 3)goto _LL2FE;_tmp483=((struct Cyc_Absyn_Local_b_struct*)
_tmp482)->f1;if(!(!_tmp483->escapes))goto _LL2FE;_LL2FD: _tmp485=_tmp483;goto
_LL2FF;_LL2FE: if(*((int*)_tmp47F)!= 1)goto _LL300;_tmp484=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp47F)->f2;if(_tmp484 <= (void*)1)goto _LL300;if(*((int*)_tmp484)!= 4)goto _LL300;
_tmp485=((struct Cyc_Absyn_Pat_b_struct*)_tmp484)->f1;if(!(!_tmp485->escapes))
goto _LL300;_LL2FF: _tmp487=_tmp485;goto _LL301;_LL300: if(*((int*)_tmp47F)!= 1)goto
_LL302;_tmp486=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp47F)->f2;if(_tmp486 <= (
void*)1)goto _LL302;if(*((int*)_tmp486)!= 2)goto _LL302;_tmp487=((struct Cyc_Absyn_Param_b_struct*)
_tmp486)->f1;if(!(!_tmp487->escapes))goto _LL302;_LL301: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp488=env->fenv;struct Cyc_List_List*_tmp489=({struct Cyc_List_List*_tmp48C=
_region_malloc(_tmp488->r,sizeof(*_tmp48C));_tmp48C->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp48D=_region_malloc(_tmp488->r,sizeof(*_tmp48D));_tmp48D->vd=_tmp479;_tmp48D->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp48E;(
_tmp48E.LessEqSize).tag=4;(_tmp48E.LessEqSize).f1=_tmp487;_tmp48E;});_tmp48D;});
_tmp48C->tl=_tmp3DB;_tmp48C;});return({struct _tuple8 _tmp48A;_tmp48A.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp48B;(_tmp48B.ReachableFL).tag=1;(_tmp48B.ReachableFL).f1=
_tmp3DA;(_tmp48B.ReachableFL).f2=_tmp489;(_tmp48B.ReachableFL).f3=_tmp3DC;
_tmp48B;});_tmp48A.f2=_tmp46A;_tmp48A;});}_LL302:;_LL303: return({struct _tuple8
_tmp48F;_tmp48F.f1=_tmp469;_tmp48F.f2=_tmp46A;_tmp48F;});_LL2F9:;}_LL2F7:;_LL2F8:
return({struct _tuple8 _tmp490;_tmp490.f1=_tmp469;_tmp490.f2=_tmp46A;_tmp490;});
_LL2F4:;}_LL2F2:;_LL2F3: return({struct _tuple8 _tmp491;_tmp491.f1=_tmp469;_tmp491.f2=
_tmp46A;_tmp491;});_LL2E9:;}}_LL2B8:;_LL2B9: return({struct _tuple8 _tmp492;_tmp492.f1=
f;_tmp492.f2=f;_tmp492;});_LL2AB:;}}_LL278:;}}_LL26F:;_LL270: goto _LL262;_LL262:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp494;void*_tmp495;struct _tuple5 _tmp493=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp494=_tmp493.f1;_tmp495=_tmp493.f2;_tmp494=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp494);{union Cyc_CfFlowInfo_FlowInfo_union _tmp496=_tmp494;struct Cyc_Dict_Dict
_tmp497;_LL305: if((_tmp496.BottomFL).tag != 0)goto _LL307;_LL306: return({struct
_tuple8 _tmp498;_tmp498.f1=_tmp494;_tmp498.f2=_tmp494;_tmp498;});_LL307: if((
_tmp496.ReachableFL).tag != 1)goto _LL304;_tmp497=(_tmp496.ReachableFL).f1;_LL308: {
void*_tmp499=_tmp495;void*_tmp49A;void*_tmp49B;void*_tmp49C;_LL30A: if((int)
_tmp499 != 0)goto _LL30C;_LL30B: return({struct _tuple8 _tmp49D;_tmp49D.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp49E;(_tmp49E.BottomFL).tag=0;_tmp49E;});
_tmp49D.f2=_tmp494;_tmp49D;});_LL30C: if((int)_tmp499 != 2)goto _LL30E;_LL30D: goto
_LL30F;_LL30E: if((int)_tmp499 != 1)goto _LL310;_LL30F: goto _LL311;_LL310: if(_tmp499
<= (void*)3)goto _LL312;if(*((int*)_tmp499)!= 2)goto _LL312;_LL311: return({struct
_tuple8 _tmp49F;_tmp49F.f1=_tmp494;_tmp49F.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4A0;(_tmp4A0.BottomFL).tag=0;_tmp4A0;});
_tmp49F;});_LL312: if(_tmp499 <= (void*)3)goto _LL314;if(*((int*)_tmp499)!= 0)goto
_LL314;_tmp49A=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp499)->f1;if((
int)_tmp49A != 0)goto _LL314;_LL313: goto _LL315;_LL314: if(_tmp499 <= (void*)3)goto
_LL316;if(*((int*)_tmp499)!= 1)goto _LL316;_tmp49B=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp499)->f1;if((int)_tmp49B != 0)goto _LL316;_LL315:({void*_tmp4A1[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp4A2="expression may not be initialized";_tag_dynforward(
_tmp4A2,sizeof(char),_get_zero_arr_size(_tmp4A2,34));}),_tag_dynforward(_tmp4A1,
sizeof(void*),0));});return({struct _tuple8 _tmp4A3;_tmp4A3.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4A5;(_tmp4A5.BottomFL).tag=0;_tmp4A5;});
_tmp4A3.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4A4;(_tmp4A4.BottomFL).tag=0;_tmp4A4;});_tmp4A3;});_LL316: if(_tmp499 <= (void*)
3)goto _LL318;if(*((int*)_tmp499)!= 0)goto _LL318;_tmp49C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp499)->f1;_LL317: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp494,e,
_tmp49C);_LL318: if(_tmp499 <= (void*)3)goto _LL31A;if(*((int*)_tmp499)!= 1)goto
_LL31A;_LL319: return({struct _tuple8 _tmp4A6;_tmp4A6.f1=_tmp494;_tmp4A6.f2=_tmp494;
_tmp4A6;});_LL31A: if(_tmp499 <= (void*)3)goto _LL31C;if(*((int*)_tmp499)!= 3)goto
_LL31C;_LL31B: return({struct _tuple8 _tmp4A7;_tmp4A7.f1=_tmp494;_tmp4A7.f2=_tmp494;
_tmp4A7;});_LL31C: if(_tmp499 <= (void*)3)goto _LL309;if(*((int*)_tmp499)!= 4)goto
_LL309;_LL31D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4A8=
_cycalloc(sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_Core_Impossible_struct _tmp4A9;
_tmp4A9.tag=Cyc_Core_Impossible;_tmp4A9.f1=({const char*_tmp4AA="anal_test";
_tag_dynforward(_tmp4AA,sizeof(char),_get_zero_arr_size(_tmp4AA,10));});_tmp4A9;});
_tmp4A8;}));_LL309:;}_LL304:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp4AB=flow;struct Cyc_Dict_Dict _tmp4AC;
_LL31F: if((_tmp4AB.BottomFL).tag != 0)goto _LL321;_LL320: return;_LL321: if((_tmp4AB.ReachableFL).tag
!= 1)goto _LL31E;_tmp4AC=(_tmp4AB.ReachableFL).f1;_LL322:{struct Cyc_List_List*
_tmp4AD=env->param_roots;for(0;_tmp4AD != 0;_tmp4AD=_tmp4AD->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp4AC,Cyc_CfFlowInfo_lookup_place(_tmp4AC,(struct Cyc_CfFlowInfo_Place*)_tmp4AD->hd))
!= (void*)2)({void*_tmp4AE[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp4AF="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp4AF,sizeof(char),_get_zero_arr_size(_tmp4AF,76));}),
_tag_dynforward(_tmp4AE,sizeof(void*),0));});}}return;_LL31E:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4B0=env->fenv;
for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4B2;struct Cyc_Core_Opt*
_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Stmt*_tmp4B5;struct Cyc_Position_Segment*
_tmp4B6;struct Cyc_Absyn_Switch_clause*_tmp4B1=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4B2=*_tmp4B1;_tmp4B3=_tmp4B2.pat_vars;_tmp4B4=_tmp4B2.where_clause;
_tmp4B5=_tmp4B2.body;_tmp4B6=_tmp4B2.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4B0,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4B3))->v,(void*)_tmp4B0->unknown_all,_tmp4B6);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4B4 != 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B8;union Cyc_CfFlowInfo_FlowInfo_union _tmp4B9;struct _tuple8 _tmp4B7=Cyc_NewControlFlow_anal_test(
env,clause_inflow,(struct Cyc_Absyn_Exp*)_tmp4B4);_tmp4B8=_tmp4B7.f1;_tmp4B9=
_tmp4B7.f2;inflow=_tmp4B9;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
_tmp4B8,_tmp4B5);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp4B5);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4BA=
clause_outflow;_LL324: if((_tmp4BA.BottomFL).tag != 0)goto _LL326;_LL325: goto _LL323;
_LL326:;_LL327: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp4BB[0]={};Cyc_Tcutil_terr(
_tmp4B5->loc,({const char*_tmp4BC="switch clause may implicitly fallthru";
_tag_dynforward(_tmp4BC,sizeof(char),_get_zero_arr_size(_tmp4BC,38));}),
_tag_dynforward(_tmp4BB,sizeof(void*),0));});else{({void*_tmp4BD[0]={};Cyc_Tcutil_warn(
_tmp4B5->loc,({const char*_tmp4BE="switch clause may implicitly fallthru";
_tag_dynforward(_tmp4BE,sizeof(char),_get_zero_arr_size(_tmp4BE,38));}),
_tag_dynforward(_tmp4BD,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL323;_LL323:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4BF;(_tmp4BF.BottomFL).tag=0;_tmp4BF;});}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp4C1;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4C2;struct _tuple9 _tmp4C0=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp4C1=_tmp4C0.f1;_tmp4C2=_tmp4C0.f2;inflow=*_tmp4C2;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4C3=env->fenv;{void*_tmp4C4=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4C5;struct
Cyc_Absyn_Exp*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Stmt*_tmp4C8;
struct Cyc_Absyn_Stmt*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Stmt*
_tmp4CB;struct Cyc_Absyn_Stmt*_tmp4CC;struct _tuple3 _tmp4CD;struct Cyc_Absyn_Exp*
_tmp4CE;struct Cyc_Absyn_Stmt*_tmp4CF;struct Cyc_Absyn_Stmt*_tmp4D0;struct Cyc_Absyn_Stmt*
_tmp4D1;struct _tuple3 _tmp4D2;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Stmt*
_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct _tuple3 _tmp4D6;struct Cyc_Absyn_Exp*
_tmp4D7;struct Cyc_Absyn_Stmt*_tmp4D8;struct _tuple3 _tmp4D9;struct Cyc_Absyn_Exp*
_tmp4DA;struct Cyc_Absyn_Stmt*_tmp4DB;struct Cyc_Absyn_Stmt*_tmp4DC;struct Cyc_Absyn_Stmt*
_tmp4DD;struct Cyc_List_List*_tmp4DE;struct Cyc_Absyn_Switch_clause**_tmp4DF;
struct Cyc_Absyn_Switch_clause*_tmp4E0;struct Cyc_Absyn_Stmt*_tmp4E1;struct Cyc_Absyn_Stmt*
_tmp4E2;struct Cyc_Absyn_Stmt*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_List_List*
_tmp4E5;struct Cyc_Absyn_Stmt*_tmp4E6;struct Cyc_List_List*_tmp4E7;struct Cyc_Absyn_Decl*
_tmp4E8;struct Cyc_Absyn_Stmt*_tmp4E9;struct Cyc_Absyn_Stmt*_tmp4EA;struct Cyc_Absyn_Tvar*
_tmp4EB;struct Cyc_Absyn_Vardecl*_tmp4EC;int _tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;
struct Cyc_Absyn_Stmt*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*
_tmp4F1;struct Cyc_Absyn_Tvar*_tmp4F2;struct Cyc_Absyn_Vardecl*_tmp4F3;struct Cyc_Absyn_Stmt*
_tmp4F4;_LL329: if((int)_tmp4C4 != 0)goto _LL32B;_LL32A: return inflow;_LL32B: if(
_tmp4C4 <= (void*)1)goto _LL353;if(*((int*)_tmp4C4)!= 2)goto _LL32D;_tmp4C5=((
struct Cyc_Absyn_Return_s_struct*)_tmp4C4)->f1;if(_tmp4C5 != 0)goto _LL32D;_LL32C:
if(env->noreturn)({void*_tmp4F5[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4F6="`noreturn' function might return";
_tag_dynforward(_tmp4F6,sizeof(char),_get_zero_arr_size(_tmp4F6,33));}),
_tag_dynforward(_tmp4F5,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4F7;(_tmp4F7.BottomFL).tag=0;_tmp4F7;});_LL32D: if(*((int*)_tmp4C4)!= 2)goto
_LL32F;_tmp4C6=((struct Cyc_Absyn_Return_s_struct*)_tmp4C4)->f1;_LL32E: if(env->noreturn)({
void*_tmp4F8[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp4F9="`noreturn' function might return";
_tag_dynforward(_tmp4F9,sizeof(char),_get_zero_arr_size(_tmp4F9,33));}),
_tag_dynforward(_tmp4F8,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4FB;void*_tmp4FC;struct _tuple5 _tmp4FA=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4C6));_tmp4FB=_tmp4FA.f1;_tmp4FC=
_tmp4FA.f2;_tmp4FB=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4C6->loc,_tmp4FB);
_tmp4FB=Cyc_NewControlFlow_use_Rval(env,_tmp4C6->loc,_tmp4FB,_tmp4FC);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp4FB);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4FD;(_tmp4FD.BottomFL).tag=0;_tmp4FD;});}_LL32F: if(*((int*)_tmp4C4)!= 0)goto
_LL331;_tmp4C7=((struct Cyc_Absyn_Exp_s_struct*)_tmp4C4)->f1;_LL330: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4C7)).f1;goto _LL328;_LL331: if(*((int*)_tmp4C4)!= 1)goto _LL333;
_tmp4C8=((struct Cyc_Absyn_Seq_s_struct*)_tmp4C4)->f1;_tmp4C9=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4C4)->f2;_LL332: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4C8),_tmp4C9);goto _LL328;_LL333: if(*((int*)_tmp4C4)!= 3)goto
_LL335;_tmp4CA=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4C4)->f1;_tmp4CB=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4C4)->f2;_tmp4CC=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4C4)->f3;_LL334: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4FF;union Cyc_CfFlowInfo_FlowInfo_union
_tmp500;struct _tuple8 _tmp4FE=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4CA);
_tmp4FF=_tmp4FE.f1;_tmp500=_tmp4FE.f2;_tmp4FF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4CA->loc,_tmp4FF);_tmp500=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4CA->loc,
_tmp500);outflow=Cyc_CfFlowInfo_join_flow(_tmp4C3,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp4FF,_tmp4CB),Cyc_NewControlFlow_anal_stmt(env,_tmp500,_tmp4CC),1);goto
_LL328;}_LL335: if(*((int*)_tmp4C4)!= 4)goto _LL337;_tmp4CD=((struct Cyc_Absyn_While_s_struct*)
_tmp4C4)->f1;_tmp4CE=_tmp4CD.f1;_tmp4CF=_tmp4CD.f2;_tmp4D0=((struct Cyc_Absyn_While_s_struct*)
_tmp4C4)->f2;_LL336: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp502;struct _tuple9
_tmp501=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4CF);_tmp502=_tmp501.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp503=*_tmp502;union Cyc_CfFlowInfo_FlowInfo_union
_tmp505;union Cyc_CfFlowInfo_FlowInfo_union _tmp506;struct _tuple8 _tmp504=Cyc_NewControlFlow_anal_test(
env,_tmp503,_tmp4CE);_tmp505=_tmp504.f1;_tmp506=_tmp504.f2;_tmp505=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4CE->loc,_tmp505);_tmp506=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4CE->loc,
_tmp506);{union Cyc_CfFlowInfo_FlowInfo_union _tmp507=Cyc_NewControlFlow_anal_stmt(
env,_tmp505,_tmp4D0);Cyc_NewControlFlow_update_flow(env,_tmp4CF,_tmp507);outflow=
_tmp506;goto _LL328;}}}_LL337: if(*((int*)_tmp4C4)!= 13)goto _LL339;_tmp4D1=((
struct Cyc_Absyn_Do_s_struct*)_tmp4C4)->f1;_tmp4D2=((struct Cyc_Absyn_Do_s_struct*)
_tmp4C4)->f2;_tmp4D3=_tmp4D2.f1;_tmp4D4=_tmp4D2.f2;_LL338: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp508=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4D1);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp50A;struct _tuple9 _tmp509=Cyc_NewControlFlow_pre_stmt_check(env,_tmp508,
_tmp4D4);_tmp50A=_tmp509.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp50B=*_tmp50A;
union Cyc_CfFlowInfo_FlowInfo_union _tmp50D;union Cyc_CfFlowInfo_FlowInfo_union
_tmp50E;struct _tuple8 _tmp50C=Cyc_NewControlFlow_anal_test(env,_tmp50B,_tmp4D3);
_tmp50D=_tmp50C.f1;_tmp50E=_tmp50C.f2;Cyc_NewControlFlow_update_flow(env,_tmp4D1,
_tmp50D);outflow=_tmp50E;goto _LL328;}}_LL339: if(*((int*)_tmp4C4)!= 8)goto _LL33B;
_tmp4D5=((struct Cyc_Absyn_For_s_struct*)_tmp4C4)->f1;_tmp4D6=((struct Cyc_Absyn_For_s_struct*)
_tmp4C4)->f2;_tmp4D7=_tmp4D6.f1;_tmp4D8=_tmp4D6.f2;_tmp4D9=((struct Cyc_Absyn_For_s_struct*)
_tmp4C4)->f3;_tmp4DA=_tmp4D9.f1;_tmp4DB=_tmp4D9.f2;_tmp4DC=((struct Cyc_Absyn_For_s_struct*)
_tmp4C4)->f4;_LL33A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp50F=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4D5)).f1;_tmp50F=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4D5->loc,
_tmp50F);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp511;struct _tuple9 _tmp510=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp50F,_tmp4D8);_tmp511=_tmp510.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp512=*_tmp511;union Cyc_CfFlowInfo_FlowInfo_union _tmp514;union Cyc_CfFlowInfo_FlowInfo_union
_tmp515;struct _tuple8 _tmp513=Cyc_NewControlFlow_anal_test(env,_tmp512,_tmp4D7);
_tmp514=_tmp513.f1;_tmp515=_tmp513.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp516=
Cyc_NewControlFlow_anal_stmt(env,_tmp514,_tmp4DC);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp518;struct _tuple9 _tmp517=Cyc_NewControlFlow_pre_stmt_check(env,_tmp516,
_tmp4DB);_tmp518=_tmp517.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp519=*_tmp518;
union Cyc_CfFlowInfo_FlowInfo_union _tmp51A=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp519,_tmp4DA)).f1;_tmp51A=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4DA->loc,
_tmp51A);Cyc_NewControlFlow_update_flow(env,_tmp4D8,_tmp51A);outflow=_tmp515;
goto _LL328;}}}}}_LL33B: if(*((int*)_tmp4C4)!= 5)goto _LL33D;_tmp4DD=((struct Cyc_Absyn_Break_s_struct*)
_tmp4C4)->f1;if(_tmp4DD != 0)goto _LL33D;_LL33C: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp51B;(_tmp51B.BottomFL).tag=0;_tmp51B;});
_LL33D: if(*((int*)_tmp4C4)!= 10)goto _LL33F;_tmp4DE=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4C4)->f1;_tmp4DF=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4C4)->f2;if(
_tmp4DF == 0)goto _LL33F;_tmp4E0=*_tmp4DF;_LL33E: {struct _RegionHandle _tmp51C=
_new_region("temp");struct _RegionHandle*temp=& _tmp51C;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp51E;struct Cyc_List_List*_tmp51F;struct _tuple11
_tmp51D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4DE,0);
_tmp51E=_tmp51D.f1;_tmp51F=_tmp51D.f2;for(0;_tmp51F != 0;(_tmp51F=_tmp51F->tl,
_tmp4DE=_tmp4DE->tl)){_tmp51E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4DE))->hd)->loc,_tmp51E,(void*)_tmp51F->hd);}
_tmp51E=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp51E);_tmp51E=Cyc_NewControlFlow_add_vars(
_tmp4C3,_tmp51E,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4E0->pat_vars))->v,(
void*)_tmp4C3->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4E0->body,_tmp51E);{union Cyc_CfFlowInfo_FlowInfo_union _tmp521=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp520;(_tmp520.BottomFL).tag=0;_tmp520;});
_npop_handler(0);return _tmp521;}};_pop_region(temp);}_LL33F: if(*((int*)_tmp4C4)
!= 5)goto _LL341;_tmp4E1=((struct Cyc_Absyn_Break_s_struct*)_tmp4C4)->f1;_LL340:
_tmp4E2=_tmp4E1;goto _LL342;_LL341: if(*((int*)_tmp4C4)!= 6)goto _LL343;_tmp4E2=((
struct Cyc_Absyn_Continue_s_struct*)_tmp4C4)->f1;_LL342: _tmp4E3=_tmp4E2;goto
_LL344;_LL343: if(*((int*)_tmp4C4)!= 7)goto _LL345;_tmp4E3=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4C4)->f2;_LL344: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp522=
_tmp4C1->encloser;struct Cyc_Absyn_Stmt*_tmp523=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4E3)))->encloser;while(_tmp523 != _tmp522){
struct Cyc_Absyn_Stmt*_tmp524=(Cyc_NewControlFlow_get_stmt_annot(_tmp522))->encloser;
if(_tmp524 == _tmp522){({void*_tmp525[0]={};Cyc_Tcutil_terr(s->loc,({const char*
_tmp526="goto enters local scope or exception handler";_tag_dynforward(_tmp526,
sizeof(char),_get_zero_arr_size(_tmp526,45));}),_tag_dynforward(_tmp525,sizeof(
void*),0));});break;}_tmp522=_tmp524;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp4E3),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp527;(_tmp527.BottomFL).tag=0;_tmp527;});
_LL345: if(*((int*)_tmp4C4)!= 9)goto _LL347;_tmp4E4=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4C4)->f1;_tmp4E5=((struct Cyc_Absyn_Switch_s_struct*)_tmp4C4)->f2;_LL346: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp529;void*_tmp52A;struct _tuple5 _tmp528=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4E4);_tmp529=_tmp528.f1;_tmp52A=_tmp528.f2;_tmp529=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4E4->loc,_tmp529);_tmp529=Cyc_NewControlFlow_use_Rval(env,_tmp4E4->loc,
_tmp529,_tmp52A);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp529,_tmp4E5);goto
_LL328;}_LL347: if(*((int*)_tmp4C4)!= 14)goto _LL349;_tmp4E6=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4C4)->f1;_tmp4E7=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4C4)->f2;_LL348: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4E6);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4E7);{union Cyc_CfFlowInfo_FlowInfo_union _tmp52B=scs_outflow;
_LL356: if((_tmp52B.BottomFL).tag != 0)goto _LL358;_LL357: goto _LL355;_LL358:;_LL359:({
void*_tmp52C[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp52D="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp52D,sizeof(char),_get_zero_arr_size(_tmp52D,42));}),
_tag_dynforward(_tmp52C,sizeof(void*),0));});_LL355:;}outflow=s1_outflow;goto
_LL328;}}}_LL349: if(*((int*)_tmp4C4)!= 11)goto _LL34B;_tmp4E8=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4C4)->f1;_tmp4E9=((struct Cyc_Absyn_Decl_s_struct*)_tmp4C4)->f2;_LL34A:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4E8),_tmp4E9);goto _LL328;_LL34B: if(*((int*)_tmp4C4)!= 12)goto _LL34D;_tmp4EA=((
struct Cyc_Absyn_Label_s_struct*)_tmp4C4)->f2;_LL34C: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4EA);goto _LL328;_LL34D: if(*((int*)_tmp4C4)!= 15)goto _LL34F;
_tmp4EB=((struct Cyc_Absyn_Region_s_struct*)_tmp4C4)->f1;_tmp4EC=((struct Cyc_Absyn_Region_s_struct*)
_tmp4C4)->f2;_tmp4ED=((struct Cyc_Absyn_Region_s_struct*)_tmp4C4)->f3;_tmp4EE=((
struct Cyc_Absyn_Region_s_struct*)_tmp4C4)->f4;_tmp4EF=((struct Cyc_Absyn_Region_s_struct*)
_tmp4C4)->f5;_LL34E: if(_tmp4EE != 0){struct Cyc_Absyn_Exp*_tmp52E=(struct Cyc_Absyn_Exp*)
_tmp4EE;union Cyc_CfFlowInfo_FlowInfo_union _tmp530;void*_tmp531;struct _tuple5
_tmp52F=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp52E);_tmp530=_tmp52F.f1;
_tmp531=_tmp52F.f2;_tmp530=Cyc_CfFlowInfo_consume_unique_rvals(_tmp52E->loc,
_tmp530);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp52E->loc,_tmp530,_tmp531);}{
struct Cyc_List_List _tmp532=({struct Cyc_List_List _tmp533;_tmp533.hd=_tmp4EC;
_tmp533.tl=0;_tmp533;});inflow=Cyc_NewControlFlow_add_vars(_tmp4C3,inflow,(
struct Cyc_List_List*)& _tmp532,(void*)_tmp4C3->unknown_all,_tmp4EF->loc);outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4EF);goto _LL328;}_LL34F: if(*((int*)
_tmp4C4)!= 16)goto _LL351;_tmp4F0=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4C4)->f1;
_LL350: {union Cyc_CfFlowInfo_FlowInfo_union _tmp535;void*_tmp536;struct _tuple5
_tmp534=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4F0);_tmp535=_tmp534.f1;
_tmp536=_tmp534.f2;_tmp535=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4F0->loc,
_tmp535);{union Cyc_CfFlowInfo_FlowInfo_union _tmp537=Cyc_NewControlFlow_use_Rval(
env,_tmp4F0->loc,_tmp535,_tmp536);{void*_tmp538=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4F0->topt))->v);void*_tmp539;_LL35B: if(
_tmp538 <= (void*)4)goto _LL35D;if(*((int*)_tmp538)!= 15)goto _LL35D;_tmp539=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp538)->f1;_LL35C: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4C3,_tmp535,_tmp539);goto _LL35A;_LL35D:;_LL35E:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A[0]=({
struct Cyc_Core_Impossible_struct _tmp53B;_tmp53B.tag=Cyc_Core_Impossible;_tmp53B.f1=({
const char*_tmp53C="anal_stmt -- reset_region";_tag_dynforward(_tmp53C,sizeof(
char),_get_zero_arr_size(_tmp53C,26));});_tmp53B;});_tmp53A;}));_LL35A:;}goto
_LL328;}}_LL351: if(*((int*)_tmp4C4)!= 17)goto _LL353;_tmp4F1=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4C4)->f1;_tmp4F2=((struct Cyc_Absyn_Alias_s_struct*)_tmp4C4)->f2;_tmp4F3=((
struct Cyc_Absyn_Alias_s_struct*)_tmp4C4)->f3;_tmp4F4=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4C4)->f4;_LL352: {union Cyc_CfFlowInfo_FlowInfo_union _tmp53E;void*_tmp53F;
struct _tuple5 _tmp53D=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4F1);_tmp53E=
_tmp53D.f1;_tmp53F=_tmp53D.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp541;struct Cyc_List_List*
_tmp542;struct _tuple6 _tmp540=Cyc_CfFlowInfo_save_consume_info(_tmp4C3,_tmp53E,0);
_tmp541=_tmp540.f1;_tmp542=_tmp541.may_consume;_tmp53E=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F1->loc,_tmp53E);_tmp53E=Cyc_NewControlFlow_use_Rval(env,_tmp4F1->loc,
_tmp53E,_tmp53F);{struct Cyc_List_List _tmp543=({struct Cyc_List_List _tmp54A;
_tmp54A.hd=_tmp4F3;_tmp54A.tl=0;_tmp54A;});_tmp53E=Cyc_NewControlFlow_add_vars(
_tmp4C3,_tmp53E,(struct Cyc_List_List*)& _tmp543,(void*)_tmp4C3->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp53E,_tmp4F4);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp544=outflow;struct Cyc_Dict_Dict _tmp545;struct Cyc_List_List*_tmp546;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp547;_LL360: if((_tmp544.BottomFL).tag != 0)goto _LL362;_LL361: goto _LL35F;_LL362:
if((_tmp544.ReachableFL).tag != 1)goto _LL35F;_tmp545=(_tmp544.ReachableFL).f1;
_tmp546=(_tmp544.ReachableFL).f2;_tmp547=(_tmp544.ReachableFL).f3;_LL363: while(
_tmp542 != 0){struct Cyc_Dict_Dict _tmp548=_tmp547.consumed;_tmp547.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4C3->r,_tmp547.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp542->hd);if((_tmp547.consumed).t != _tmp548.t);_tmp542=_tmp542->tl;}outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp549;(
_tmp549.ReachableFL).tag=1;(_tmp549.ReachableFL).f1=_tmp545;(_tmp549.ReachableFL).f2=
_tmp546;(_tmp549.ReachableFL).f3=_tmp547;_tmp549;});goto _LL35F;_LL35F:;}goto
_LL328;}}}_LL353:;_LL354:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({struct Cyc_Core_Impossible_struct
_tmp54C;_tmp54C.tag=Cyc_Core_Impossible;_tmp54C.f1=({const char*_tmp54D="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp54D,sizeof(char),_get_zero_arr_size(_tmp54D,56));});_tmp54C;});
_tmp54B;}));_LL328:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp54E=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp54F;_LL365: if((_tmp54E.ReachableFL).tag != 1)goto _LL367;_tmp54F=(_tmp54E.ReachableFL).f3;
_LL366: goto _LL364;_LL367:;_LL368: goto _LL364;_LL364:;}return outflow;}}static void
Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp550=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp551;struct Cyc_Core_Opt*_tmp552;struct Cyc_Core_Opt _tmp553;struct Cyc_List_List*
_tmp554;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_List_List*_tmp556;struct Cyc_Absyn_Fndecl*
_tmp557;_LL36A: if(_tmp550 <= (void*)2)goto _LL372;if(*((int*)_tmp550)!= 0)goto
_LL36C;_tmp551=((struct Cyc_Absyn_Var_d_struct*)_tmp550)->f1;_LL36B: {struct Cyc_List_List
_tmp558=({struct Cyc_List_List _tmp567;_tmp567.hd=_tmp551;_tmp567.tl=0;_tmp567;});
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp558,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp559=
_tmp551->initializer;if(_tmp559 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp55B;void*_tmp55C;struct _tuple5 _tmp55A=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp559);_tmp55B=_tmp55A.f1;_tmp55C=_tmp55A.f2;
_tmp55B=Cyc_CfFlowInfo_consume_unique_rvals(_tmp559->loc,_tmp55B);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp55D=_tmp55B;struct Cyc_Dict_Dict _tmp55E;struct Cyc_List_List*_tmp55F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp560;_LL375: if((_tmp55D.BottomFL).tag != 0)goto _LL377;_LL376: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp561;(_tmp561.BottomFL).tag=0;_tmp561;});
_LL377: if((_tmp55D.ReachableFL).tag != 1)goto _LL374;_tmp55E=(_tmp55D.ReachableFL).f1;
_tmp55F=(_tmp55D.ReachableFL).f2;_tmp560=(_tmp55D.ReachableFL).f3;_LL378: _tmp55E=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp55E,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp562=_region_malloc(env->r,sizeof(*_tmp562));
_tmp562->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp563=
_region_malloc(env->r,sizeof(*_tmp563));_tmp563[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp564;_tmp564.tag=0;_tmp564.f1=_tmp551;_tmp564;});_tmp563;}));_tmp562->fields=
0;_tmp562;}),_tmp55C);_tmp55F=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp55F,
_tmp551,(struct Cyc_Absyn_Exp*)_check_null(_tmp551->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp565=({union Cyc_CfFlowInfo_FlowInfo_union _tmp566;(_tmp566.ReachableFL).tag=1;(
_tmp566.ReachableFL).f1=_tmp55E;(_tmp566.ReachableFL).f2=_tmp55F;(_tmp566.ReachableFL).f3=
_tmp560;_tmp566;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp565);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp565;}_LL374:;}}}}_LL36C:
if(*((int*)_tmp550)!= 2)goto _LL36E;_tmp552=((struct Cyc_Absyn_Let_d_struct*)
_tmp550)->f2;if(_tmp552 == 0)goto _LL36E;_tmp553=*_tmp552;_tmp554=(struct Cyc_List_List*)
_tmp553.v;_tmp555=((struct Cyc_Absyn_Let_d_struct*)_tmp550)->f3;_LL36D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp569;void*_tmp56A;struct _tuple5 _tmp568=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp555);_tmp569=_tmp568.f1;_tmp56A=_tmp568.f2;_tmp569=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp555->loc,_tmp569);_tmp569=Cyc_NewControlFlow_use_Rval(env,_tmp555->loc,
_tmp569,_tmp56A);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp569,_tmp554,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL36E: if(*((int*)_tmp550)!= 3)goto
_LL370;_tmp556=((struct Cyc_Absyn_Letv_d_struct*)_tmp550)->f1;_LL36F: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp556,(void*)(env->fenv)->unknown_none,decl->loc);_LL370: if(*((
int*)_tmp550)!= 1)goto _LL372;_tmp557=((struct Cyc_Absyn_Fn_d_struct*)_tmp550)->f1;
_LL371: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp557);{void*
_tmp56B=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp557->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp56C=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp557->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp56D=
_region_malloc(env->r,sizeof(*_tmp56D));_tmp56D->hd=_tmp56C;_tmp56D->tl=0;
_tmp56D;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL372:;_LL373:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp56E=_cycalloc(sizeof(*
_tmp56E));_tmp56E[0]=({struct Cyc_Core_Impossible_struct _tmp56F;_tmp56F.tag=Cyc_Core_Impossible;
_tmp56F.f1=({const char*_tmp570="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp570,sizeof(char),_get_zero_arr_size(_tmp570,35));});_tmp56F;});
_tmp56E;}));_LL369:;}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp571=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp571;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp572;(_tmp572.ReachableFL).tag=1;(_tmp572.ReachableFL).f1=
fenv->mt_flowdict;(_tmp572.ReachableFL).f2=0;(_tmp572.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp573;_tmp573.consumed=fenv->mt_place_set;_tmp573.may_consume=
0;_tmp573;});_tmp572;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp574=fenv->r;struct Cyc_Position_Segment*_tmp575=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp575);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp576=inflow;struct Cyc_Dict_Dict
_tmp577;struct Cyc_List_List*_tmp578;struct Cyc_CfFlowInfo_ConsumeInfo _tmp579;
_LL37A: if((_tmp576.BottomFL).tag != 0)goto _LL37C;_LL37B:({void*_tmp57A[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp57B="check_fun";_tag_dynforward(_tmp57B,sizeof(char),
_get_zero_arr_size(_tmp57B,10));}),_tag_dynforward(_tmp57A,sizeof(void*),0));});
_LL37C: if((_tmp576.ReachableFL).tag != 1)goto _LL379;_tmp577=(_tmp576.ReachableFL).f1;
_tmp578=(_tmp576.ReachableFL).f2;_tmp579=(_tmp576.ReachableFL).f3;_LL37D: {
struct Cyc_List_List*atts;{void*_tmp57C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp57D;struct Cyc_List_List*
_tmp57E;_LL37F: if(_tmp57C <= (void*)4)goto _LL381;if(*((int*)_tmp57C)!= 8)goto
_LL381;_tmp57D=((struct Cyc_Absyn_FnType_struct*)_tmp57C)->f1;_tmp57E=_tmp57D.attributes;
_LL380: atts=_tmp57E;goto _LL37E;_LL381:;_LL382:({void*_tmp57F[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp580="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp580,sizeof(char),_get_zero_arr_size(_tmp580,50));}),_tag_dynforward(_tmp57F,
sizeof(void*),0));});_LL37E:;}for(0;atts != 0;atts=atts->tl){void*_tmp581=(void*)
atts->hd;int _tmp582;_LL384: if(_tmp581 <= (void*)17)goto _LL386;if(*((int*)_tmp581)
!= 4)goto _LL386;_tmp582=((struct Cyc_Absyn_Initializes_att_struct*)_tmp581)->f1;
_LL385: {unsigned int j=(unsigned int)_tmp582;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp583[0]={};Cyc_Tcutil_terr(_tmp575,({const char*_tmp584="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp584,sizeof(char),_get_zero_arr_size(_tmp584,51));}),
_tag_dynforward(_tmp583,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp585=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp586=Cyc_Tcutil_compress((void*)_tmp585->type);struct Cyc_Absyn_PtrInfo
_tmp587;void*_tmp588;struct Cyc_Absyn_PtrAtts _tmp589;struct Cyc_Absyn_Conref*
_tmp58A;struct Cyc_Absyn_Conref*_tmp58B;struct Cyc_Absyn_Conref*_tmp58C;_LL389: if(
_tmp586 <= (void*)4)goto _LL38B;if(*((int*)_tmp586)!= 4)goto _LL38B;_tmp587=((
struct Cyc_Absyn_PointerType_struct*)_tmp586)->f1;_tmp588=(void*)_tmp587.elt_typ;
_tmp589=_tmp587.ptr_atts;_tmp58A=_tmp589.nullable;_tmp58B=_tmp589.bounds;_tmp58C=
_tmp589.zero_term;_LL38A: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp58A))({void*_tmp58D[0]={};Cyc_Tcutil_terr(_tmp575,({const char*_tmp58E="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp58E,sizeof(char),_get_zero_arr_size(_tmp58E,55));}),
_tag_dynforward(_tmp58D,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp58B))({
void*_tmp58F[0]={};Cyc_Tcutil_terr(_tmp575,({const char*_tmp590="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp590,sizeof(char),_get_zero_arr_size(_tmp590,57));}),
_tag_dynforward(_tmp58F,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp58C))({void*_tmp591[0]={};Cyc_Tcutil_terr(_tmp575,({
const char*_tmp592="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp592,sizeof(char),_get_zero_arr_size(_tmp592,77));}),
_tag_dynforward(_tmp591,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp593=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp59B=_region_malloc(_tmp574,
sizeof(*_tmp59B));_tmp59B[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp59C;
_tmp59C.tag=2;_tmp59C.f1=(int)j;_tmp59C.f2=(void*)_tmp588;_tmp59C;});_tmp59B;});
struct Cyc_CfFlowInfo_Place*_tmp594=({struct Cyc_CfFlowInfo_Place*_tmp59A=
_region_malloc(_tmp574,sizeof(*_tmp59A));_tmp59A->root=(void*)((void*)_tmp593);
_tmp59A->fields=0;_tmp59A;});_tmp577=Cyc_Dict_insert(_tmp577,(void*)_tmp593,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp588,(void*)fenv->esc_none));_tmp577=Cyc_Dict_insert(_tmp577,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp595=_region_malloc(_tmp574,sizeof(*
_tmp595));_tmp595[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp596;_tmp596.tag=0;
_tmp596.f1=_tmp585;_tmp596;});_tmp595;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp597=_region_malloc(_tmp574,sizeof(*_tmp597));_tmp597[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp598;_tmp598.tag=2;_tmp598.f1=_tmp594;_tmp598;});_tmp597;}));param_roots=({
struct Cyc_List_List*_tmp599=_region_malloc(_tmp574,sizeof(*_tmp599));_tmp599->hd=
_tmp594;_tmp599->tl=param_roots;_tmp599;});goto _LL388;}_LL38B:;_LL38C:({void*
_tmp59D[0]={};Cyc_Tcutil_terr(_tmp575,({const char*_tmp59E="initializes attribute on non-pointer";
_tag_dynforward(_tmp59E,sizeof(char),_get_zero_arr_size(_tmp59E,37));}),
_tag_dynforward(_tmp59D,sizeof(void*),0));});_LL388:;}goto _LL383;}}_LL386:;
_LL387: goto _LL383;_LL383:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp59F;(_tmp59F.ReachableFL).tag=1;(_tmp59F.ReachableFL).f1=_tmp577;(_tmp59F.ReachableFL).f2=
_tmp578;(_tmp59F.ReachableFL).f3=_tmp579;_tmp59F;});}_LL379:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(
struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),
int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp574,33,(int(*)(
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp5A8=_region_malloc(_tmp574,sizeof(*_tmp5A8));_tmp5A8->r=_tmp574;_tmp5A8->fenv=
fenv;_tmp5A8->iterate_again=1;_tmp5A8->iteration_num=0;_tmp5A8->in_try=0;_tmp5A8->tryflow=
inflow;_tmp5A8->all_changed=0;_tmp5A8->noreturn=noreturn;_tmp5A8->param_roots=
param_roots;_tmp5A8->flow_table=flow_table;_tmp5A8;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5A0=outflow;_LL38E: if((_tmp5A0.BottomFL).tag != 0)goto _LL390;_LL38F: goto
_LL38D;_LL390:;_LL391: Cyc_NewControlFlow_check_init_params(_tmp575,env,outflow);
if(noreturn)({void*_tmp5A1[0]={};Cyc_Tcutil_terr(_tmp575,({const char*_tmp5A2="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp5A2,sizeof(char),_get_zero_arr_size(_tmp5A2,46));}),
_tag_dynforward(_tmp5A1,sizeof(void*),0));});else{void*_tmp5A3=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL393: if((int)_tmp5A3 != 0)goto _LL395;_LL394: goto _LL392;
_LL395: if(_tmp5A3 <= (void*)4)goto _LL397;if(*((int*)_tmp5A3)!= 6)goto _LL397;
_LL396: goto _LL398;_LL397: if((int)_tmp5A3 != 1)goto _LL399;_LL398: goto _LL39A;_LL399:
if(_tmp5A3 <= (void*)4)goto _LL39B;if(*((int*)_tmp5A3)!= 5)goto _LL39B;_LL39A:({
void*_tmp5A4[0]={};Cyc_Tcutil_warn(_tmp575,({const char*_tmp5A5="function may not return a value";
_tag_dynforward(_tmp5A5,sizeof(char),_get_zero_arr_size(_tmp5A5,32));}),
_tag_dynforward(_tmp5A4,sizeof(void*),0));});goto _LL392;_LL39B:;_LL39C:({void*
_tmp5A6[0]={};Cyc_Tcutil_terr(_tmp575,({const char*_tmp5A7="function may not return a value";
_tag_dynforward(_tmp5A7,sizeof(char),_get_zero_arr_size(_tmp5A7,32));}),
_tag_dynforward(_tmp5A6,sizeof(void*),0));});goto _LL392;_LL392:;}goto _LL38D;
_LL38D:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;
ds=ds->tl){void*_tmp5A9=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*
_tmp5AA;struct Cyc_List_List*_tmp5AB;struct Cyc_List_List*_tmp5AC;struct Cyc_List_List*
_tmp5AD;_LL39E: if(_tmp5A9 <= (void*)2)goto _LL3A8;if(*((int*)_tmp5A9)!= 1)goto
_LL3A0;_tmp5AA=((struct Cyc_Absyn_Fn_d_struct*)_tmp5A9)->f1;_LL39F: Cyc_NewControlFlow_check_fun(
_tmp5AA);goto _LL39D;_LL3A0: if(*((int*)_tmp5A9)!= 10)goto _LL3A2;_tmp5AB=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp5A9)->f1;_LL3A1: _tmp5AC=_tmp5AB;goto _LL3A3;
_LL3A2: if(*((int*)_tmp5A9)!= 9)goto _LL3A4;_tmp5AC=((struct Cyc_Absyn_Using_d_struct*)
_tmp5A9)->f2;_LL3A3: _tmp5AD=_tmp5AC;goto _LL3A5;_LL3A4: if(*((int*)_tmp5A9)!= 8)
goto _LL3A6;_tmp5AD=((struct Cyc_Absyn_Namespace_d_struct*)_tmp5A9)->f2;_LL3A5: Cyc_NewControlFlow_cf_check(
_tmp5AD);goto _LL39D;_LL3A6: if(*((int*)_tmp5A9)!= 11)goto _LL3A8;_LL3A7: goto _LL39D;
_LL3A8:;_LL3A9: goto _LL39D;_LL39D:;}}
