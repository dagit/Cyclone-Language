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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct _dynforward_ptr Cstring_to_string(char*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
struct _dynforward_ptr Cyc_List_to_array(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[8];
struct Cyc_Arg_Bad_struct{char*tag;struct _dynforward_ptr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr,struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dynforward_ptr),struct _dynforward_ptr errmsg,struct
_dynforward_ptr args);typedef struct{int __count;union{unsigned int __wch;char __wchb[
4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);unsigned int Cyc_fread(struct
_dynforward_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
unsigned int Cyc_fwrite(struct _dynforward_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);int remove(const char*);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dynforward_ptr,struct _dynforward_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;
void*abort();void exit(int);char*getenv(const char*);int system(const char*);
unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);int Cyc_strncmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned int len);struct _dynforward_ptr
Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr
Cyc_strconcat_l(struct Cyc_List_List*);struct _dynforward_ptr Cyc_str_sepstr(struct
Cyc_List_List*,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strdup(struct
_dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr,int
ofs,unsigned int n);struct _dynforward_ptr Cyc_strchr(struct _dynforward_ptr s,char c);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct _dynforward_ptr Cyc_Filename_concat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_chop_extension(
struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_dirname(struct
_dynforward_ptr);struct _dynforward_ptr Cyc_Filename_basename(struct
_dynforward_ptr);int Cyc_Filename_check_suffix(struct _dynforward_ptr,struct
_dynforward_ptr);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dynforward_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();int isspace(int);
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dynforward_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple4 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple5{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple6*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple7*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY16_struct{int tag;struct _tuple6*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*f1;};struct _tuple8{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{int tag;
struct _tuple8*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};struct
Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};struct Cyc_YY38_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY39_struct{int tag;
struct _tuple10*f1;};struct Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY42_struct{int tag;
void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{int tag;
void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY46_struct{
int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY49_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY52_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
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
le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
extern int Cyc_Tovc_elim_array_initializers;struct Cyc_List_List*Cyc_Tovc_tovc(
struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
f1;};void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple11{struct _dynforward_ptr f1;struct _dynforward_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);void Cyc_Port_port(
struct Cyc_List_List*);void Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;
int Cyc_Toc_warn_all_null_deref;static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int
Cyc_noshake_r=0;static int Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;static int Cyc_cf_r=0;static int Cyc_noprint_r=0;static int Cyc_ic_r=
0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=0;static int Cyc_stop_after_asmfile_r=
0;static int Cyc_inline_functions_r=0;static int Cyc_elim_se_r=0;static int Cyc_tovc_r=
0;static int Cyc_v_r=0;static int Cyc_save_temps_r=0;static int Cyc_save_c_r=0;static
int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=0;static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=0;
static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int Cyc_print_all_kinds_r=
0;static int Cyc_print_all_effects_r=0;void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=
1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct
_dynforward_ptr*Cyc_output_file=0;static void Cyc_set_output_file(struct
_dynforward_ptr s){Cyc_output_file=({struct _dynforward_ptr*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0[0]=s;_tmp0;});}extern char*Cdef_inc_path;extern char*
Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;static char
_tmp1[1]="";static struct _dynforward_ptr Cyc_cpp={_tmp1,_tmp1 + 1};static void Cyc_set_cpp(
struct _dynforward_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dynforward_ptr s){Cyc_cppargs=({struct Cyc_List_List*
_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=({struct _dynforward_ptr*_tmp3=
_cycalloc(sizeof(*_tmp3));_tmp3[0]=s;_tmp3;});_tmp2->tl=Cyc_cppargs;_tmp2;});}
static struct _dynforward_ptr Cyc_target_arch=(struct _dynforward_ptr){(void*)0,(
void*)(0 + 0)};static void Cyc_set_target_arch(struct _dynforward_ptr s){Cyc_toc_r=1;
Cyc_target_arch=s;}static void Cyc_print_version(){({struct Cyc_String_pa_struct
_tmp6;_tmp6.tag=0;_tmp6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Cversion));{void*_tmp4[1]={& _tmp6};Cyc_printf(({const char*
_tmp5="The Cyclone compiler, version %s\n";_tag_dynforward(_tmp5,sizeof(char),
_get_zero_arr_size(_tmp5,34));}),_tag_dynforward(_tmp4,sizeof(void*),1));}});({
struct Cyc_String_pa_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cstring_to_string(Ccomp));{struct Cyc_String_pa_struct _tmp9;
_tmp9.tag=0;_tmp9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cstring_to_string(Carch));{void*_tmp7[2]={& _tmp9,& _tmpA};Cyc_printf(({const char*
_tmp8="Compiled for architecture: %s CC=\"%s\"\n";_tag_dynforward(_tmp8,sizeof(
char),_get_zero_arr_size(_tmp8,39));}),_tag_dynforward(_tmp7,sizeof(void*),2));}}});({
struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cstring_to_string(Cdef_lib_path));{void*_tmpB[1]={& _tmpD};
Cyc_printf(({const char*_tmpC="Standard library directory: %s\n";_tag_dynforward(
_tmpC,sizeof(char),_get_zero_arr_size(_tmpC,32));}),_tag_dynforward(_tmpB,
sizeof(void*),1));}});({struct Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path));{
void*_tmpE[1]={& _tmp10};Cyc_printf(({const char*_tmpF="Standard include directory: %s\n";
_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,32));}),
_tag_dynforward(_tmpE,sizeof(void*),1));}});exit(0);}static int Cyc_is_cyclone_sourcefile(
struct _dynforward_ptr s){unsigned int _tmp11=Cyc_strlen((struct _dynforward_ptr)s);
if(_tmp11 <= 4)return 0;else{return Cyc_strcmp((struct _dynforward_ptr)
_dynforward_ptr_plus(s,sizeof(char),(int)(_tmp11 - 4)),({const char*_tmp12=".cyc";
_tag_dynforward(_tmp12,sizeof(char),_get_zero_arr_size(_tmp12,5));}))== 0;}}
static struct Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct
_dynforward_ptr s){Cyc_ccargs=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*
_tmp13));_tmp13->hd=({struct _dynforward_ptr*_tmp14=_cycalloc(sizeof(*_tmp14));
_tmp14[0]=s;_tmp14;});_tmp13->tl=Cyc_ccargs;_tmp13;});}static struct Cyc_List_List*
Cyc_cyclone_exec_path=0;static void Cyc_add_cyclone_exec_path(struct
_dynforward_ptr s){unsigned int _tmp15=Cyc_strlen((struct _dynforward_ptr)s);if(
_tmp15 <= 2)return;{struct _dynforward_ptr _tmp16=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)s,2,_tmp15 - 2);Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=({struct _dynforward_ptr*_tmp18=
_cycalloc(sizeof(*_tmp18));_tmp18[0]=_tmp16;_tmp18;});_tmp17->tl=Cyc_cyclone_exec_path;
_tmp17;});}}static struct Cyc_List_List*Cyc_libargs=0;static void Cyc_add_libarg(
struct _dynforward_ptr s){if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp19="-lxml";_tag_dynforward(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,6));}))
== 0)Cyc_add_ccarg(s);else{Cyc_libargs=({struct Cyc_List_List*_tmp1A=_cycalloc(
sizeof(*_tmp1A));_tmp1A->hd=({struct _dynforward_ptr*_tmp1B=_cycalloc(sizeof(*
_tmp1B));_tmp1B[0]=s;_tmp1B;});_tmp1A->tl=Cyc_libargs;_tmp1A;});}}static void Cyc_add_marg(
struct _dynforward_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps(){Cyc_save_temps_r=
1;Cyc_add_ccarg(({const char*_tmp1C="-save-temps";_tag_dynforward(_tmp1C,sizeof(
char),_get_zero_arr_size(_tmp1C,12));}));}static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=
1;Cyc_add_cpparg(({const char*_tmp1D="-M";_tag_dynforward(_tmp1D,sizeof(char),
_get_zero_arr_size(_tmp1D,3));}));}static struct _dynforward_ptr*Cyc_dependencies_target=
0;static void Cyc_set_dependencies_target(struct _dynforward_ptr s){Cyc_dependencies_target=({
struct _dynforward_ptr*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=s;_tmp1E;});}
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(({
const char*_tmp1F="-c";_tag_dynforward(_tmp1F,sizeof(char),_get_zero_arr_size(
_tmp1F,3));}));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(({const char*
_tmp20="-no-cpp-precomp";_tag_dynforward(_tmp20,sizeof(char),_get_zero_arr_size(
_tmp20,16));}));Cyc_add_ccarg(({const char*_tmp21="-no-cpp-precomp";
_tag_dynforward(_tmp21,sizeof(char),_get_zero_arr_size(_tmp21,16));}));}static
void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions(){Cyc_inline_functions_r=1;}static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;Cyc_set_inline_functions();}static void Cyc_set_tovc(){Cyc_tovc_r=
1;Cyc_set_elim_se();}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(({const
char*_tmp22="-DNO_CYC_BOUNDS_CHECKS";_tag_dynforward(_tmp22,sizeof(char),
_get_zero_arr_size(_tmp22,23));}));}static void Cyc_set_nonullchecks(){Cyc_add_ccarg(({
const char*_tmp23="-DNO_CYC_NULL_CHECKS";_tag_dynforward(_tmp23,sizeof(char),
_get_zero_arr_size(_tmp23,21));}));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=
0;Cyc_add_ccarg(({const char*_tmp24="-DNO_CYC_PREFIX";_tag_dynforward(_tmp24,
sizeof(char),_get_zero_arr_size(_tmp24,16));}));}static void Cyc_set_pa(){Cyc_pa_r=
1;Cyc_add_ccarg(({const char*_tmp25="-DCYC_REGION_PROFILE";_tag_dynforward(_tmp25,
sizeof(char),_get_zero_arr_size(_tmp25,21));}));}static void Cyc_set_pg(){Cyc_pg_r=
1;Cyc_add_ccarg(({const char*_tmp26="-pg";_tag_dynforward(_tmp26,sizeof(char),
_get_zero_arr_size(_tmp26,4));}));}static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=
1;Cyc_add_ccarg(({const char*_tmp27="-S";_tag_dynforward(_tmp27,sizeof(char),
_get_zero_arr_size(_tmp27,3));}));}static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}static void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(
struct _dynforward_ptr s){if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp28="cyc";_tag_dynforward(_tmp28,sizeof(char),_get_zero_arr_size(_tmp28,4));}))
== 0)Cyc_intype=(void*)1;else{if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp29="none";_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,5));}))
== 0)Cyc_intype=(void*)0;else{({struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;
_tmp2C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp2A[1]={&
_tmp2C};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B="Input type '%s' not supported\n";
_tag_dynforward(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,31));}),
_tag_dynforward(_tmp2A,sizeof(void*),1));}});}}}struct _dynforward_ptr Cyc_make_base_filename(
struct _dynforward_ptr s,struct _dynforward_ptr*output_file){struct _dynforward_ptr
_tmp2D=Cyc_Filename_dirname(output_file == 0?s:*output_file);struct
_dynforward_ptr _tmp2E=Cyc_Filename_chop_extension((struct _dynforward_ptr)Cyc_Filename_basename(
s));struct _dynforward_ptr _tmp2F=Cyc_strlen((struct _dynforward_ptr)_tmp2D)> 0?Cyc_Filename_concat((
struct _dynforward_ptr)_tmp2D,(struct _dynforward_ptr)_tmp2E): _tmp2E;return(struct
_dynforward_ptr)_tmp2F;}static struct Cyc_List_List*Cyc_cyclone_files=0;static void
Cyc_add_other(struct _dynforward_ptr s){if(Cyc_intype == (void*)1  || Cyc_is_cyclone_sourcefile(
s)){Cyc_cyclone_files=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));
_tmp30->hd=({struct _dynforward_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=s;
_tmp31;});_tmp30->tl=Cyc_cyclone_files;_tmp30;});{struct _dynforward_ptr _tmp32=
Cyc_make_base_filename(s,Cyc_output_file);struct _dynforward_ptr _tmp33=Cyc_strconcat((
struct _dynforward_ptr)_tmp32,({const char*_tmp34=".c";_tag_dynforward(_tmp34,
sizeof(char),_get_zero_arr_size(_tmp34,3));}));Cyc_add_ccarg((struct
_dynforward_ptr)_tmp33);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _dynforward_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dynforward_ptr(s,sizeof(char),1));}}struct _tuple12{struct _dynforward_ptr*
f1;struct _dynforward_ptr*f2;};struct Cyc_List_List*Cyc_read_specs(struct
_dynforward_ptr file){struct Cyc_List_List*_tmp35=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dynforward_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){({
struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmp36[1]={& _tmp38};Cyc_fprintf(Cyc_stderr,({
const char*_tmp37="Error opening spec file %s\n";_tag_dynforward(_tmp37,sizeof(
char),_get_zero_arr_size(_tmp37,28));}),_tag_dynforward(_tmp36,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){({struct Cyc_Int_pa_struct _tmp3C;_tmp3C.tag=1;_tmp3C.f1=(unsigned long)c;{
struct Cyc_String_pa_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmp39[2]={& _tmp3B,& _tmp3C};Cyc_fprintf(Cyc_stderr,({
const char*_tmp3A="Error reading spec file %s: unexpected character '%c'\n";
_tag_dynforward(_tmp3A,sizeof(char),_get_zero_arr_size(_tmp3A,55));}),
_tag_dynforward(_tmp39,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);}goto CLEANUP_AND_RETURN;}JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(c == - 1){({struct Cyc_String_pa_struct _tmp3F;
_tmp3F.tag=0;_tmp3F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file);{
void*_tmp3D[1]={& _tmp3F};Cyc_fprintf(Cyc_stderr,({const char*_tmp3E="Error reading spec file %s: unexpected EOF\n";
_tag_dynforward(_tmp3E,sizeof(char),_get_zero_arr_size(_tmp3E,44));}),
_tag_dynforward(_tmp3D,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){strname[
_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;i ++;if(i >= 256){({struct Cyc_String_pa_struct
_tmp42;_tmp42.tag=0;_tmp42.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
file);{void*_tmp40[1]={& _tmp42};Cyc_fprintf(Cyc_stderr,({const char*_tmp41="Error reading spec file %s: string name too long\n";
_tag_dynforward(_tmp41,sizeof(char),_get_zero_arr_size(_tmp41,50));}),
_tag_dynforward(_tmp40,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)
spec_file);if(isspace(c))continue;break;}if(c == '*'){_tmp35=({struct Cyc_List_List*
_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->hd=({struct _tuple12*_tmp44=_cycalloc(
sizeof(*_tmp44));_tmp44->f1=({struct _dynforward_ptr*_tmp47=_cycalloc(sizeof(*
_tmp47));_tmp47[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strname,
sizeof(char),256));_tmp47;});_tmp44->f2=({struct _dynforward_ptr*_tmp45=_cycalloc(
sizeof(*_tmp45));_tmp45[0]=(struct _dynforward_ptr)Cyc_strdup(({const char*_tmp46="";
_tag_dynforward(_tmp46,sizeof(char),_get_zero_arr_size(_tmp46,1));}));_tmp45;});
_tmp44;});_tmp43->tl=_tmp35;_tmp43;});goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=
1;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;i ++;if(i >= 4096){({
struct Cyc_String_pa_struct _tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct _dynforward_ptr)
_tag_dynforward(strname,sizeof(char),256);{struct Cyc_String_pa_struct _tmp4A;
_tmp4A.tag=0;_tmp4A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file);{
void*_tmp48[2]={& _tmp4A,& _tmp4B};Cyc_fprintf(Cyc_stderr,({const char*_tmp49="Error reading spec file %s: value of %s too long\n";
_tag_dynforward(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,50));}),
_tag_dynforward(_tmp48,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}}_tmp35=({struct Cyc_List_List*_tmp4C=
_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=({struct _tuple12*_tmp4D=_cycalloc(sizeof(*
_tmp4D));_tmp4D->f1=({struct _dynforward_ptr*_tmp4F=_cycalloc(sizeof(*_tmp4F));
_tmp4F[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strname,sizeof(char),
256));_tmp4F;});_tmp4D->f2=({struct _dynforward_ptr*_tmp4E=_cycalloc(sizeof(*
_tmp4E));_tmp4E[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strvalue,
sizeof(char),4096));_tmp4E;});_tmp4D;});_tmp4C->tl=_tmp35;_tmp4C;});if(c == - 1)
goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((struct Cyc___cycFILE*)
spec_file);return _tmp35;}struct _dynforward_ptr Cyc_split_specs(struct
_dynforward_ptr cmdline){if(cmdline.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return _tag_dynforward(0,0,0);{unsigned int _tmp50=
Cyc_strlen((struct _dynforward_ptr)cmdline);struct Cyc_List_List*_tmp51=0;char buf[
4096];int i=0;int j=0;if(_tmp50 > 4096)goto DONE;while(1){while(1){if(i >= _tmp50)
goto DONE;if(*((const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))== 
0)goto DONE;if(!isspace((int)*((const char*)_check_dynforward_subscript(cmdline,
sizeof(char),i))))break;i ++;}j=0;while(1){if(i >= _tmp50)break;if(*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))== '\\'){i ++;if(i
>= _tmp50)break;if(*((const char*)_check_dynforward_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i));j ++;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i));j ++;}i ++;}if(j < 4096)buf[
_check_known_subscript_notnull(4096,j)]='\000';_tmp51=({struct Cyc_List_List*
_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->hd=({struct _dynforward_ptr*_tmp53=
_cycalloc(sizeof(*_tmp53));_tmp53[0]=(struct _dynforward_ptr)Cyc_strdup(
_tag_dynforward(buf,sizeof(char),4096));_tmp53;});_tmp52->tl=_tmp51;_tmp52;});}
DONE: _tmp51=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp51);_tmp51=({struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp54->tl=_tmp51;_tmp54;});{struct _dynforward_ptr _tmp56=((struct _dynforward_ptr(*)(
struct Cyc_List_List*x))Cyc_List_to_array)(_tmp51);struct _dynforward_ptr _tmp57=({
unsigned int _tmp58=_get_dynforward_size(_tmp56,sizeof(struct _dynforward_ptr*));
struct _dynforward_ptr*_tmp59=(struct _dynforward_ptr*)_cycalloc(_check_times(
sizeof(struct _dynforward_ptr),_tmp58));struct _dynforward_ptr _tmp5B=
_tag_dynforward(_tmp59,sizeof(struct _dynforward_ptr),_tmp58);{unsigned int _tmp5A=
_tmp58;unsigned int k;for(k=0;k < _tmp5A;k ++){_tmp59[k]=*(*((struct _dynforward_ptr**)
_check_dynforward_subscript(_tmp56,sizeof(struct _dynforward_ptr*),(int)k)));}}
_tmp5B;});return _tmp57;}}}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dynforward_ptr filename,struct _dynforward_ptr mode,struct _dynforward_ptr
msg_part){struct _handler_cons _tmp5C;_push_handler(& _tmp5C);{int _tmp5E=0;if(
setjmp(_tmp5C.handler))_tmp5E=1;if(!_tmp5E){{struct Cyc___cycFILE*_tmp5F=(struct
Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);return _tmp5F;};
_pop_handler();}else{void*_tmp5D=(void*)_exn_thrown;void*_tmp61=_tmp5D;_LL1:;
_LL2: Cyc_compile_failure=1;({struct Cyc_String_pa_struct _tmp65;_tmp65.tag=0;
_tmp65.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)filename);{struct Cyc_String_pa_struct
_tmp64;_tmp64.tag=0;_tmp64.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg_part);{void*_tmp62[2]={& _tmp64,& _tmp65};Cyc_fprintf(Cyc_stderr,({const char*
_tmp63="\nError: couldn't open %s %s\n";_tag_dynforward(_tmp63,sizeof(char),
_get_zero_arr_size(_tmp63,29));}),_tag_dynforward(_tmp62,sizeof(void*),2));}}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(
_tmp61);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct
Cyc_List_List*tds){struct _dynforward_ptr exn_string=({const char*_tmp83="";
_tag_dynforward(_tmp83,sizeof(char),_get_zero_arr_size(_tmp83,1));});struct
_dynforward_ptr explain_string=({const char*_tmp82="";_tag_dynforward(_tmp82,
sizeof(char),_get_zero_arr_size(_tmp82,1));});int other_exn=0;void*ex=(void*)({
struct Cyc_Core_Impossible_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({
struct Cyc_Core_Impossible_struct _tmp80;_tmp80.tag=Cyc_Core_Impossible;_tmp80.f1=({
const char*_tmp81="";_tag_dynforward(_tmp81,sizeof(char),_get_zero_arr_size(
_tmp81,1));});_tmp80;});_tmp7F;});struct Cyc_List_List*_tmp66=0;{struct
_handler_cons _tmp67;_push_handler(& _tmp67);{int _tmp69=0;if(setjmp(_tmp67.handler))
_tmp69=1;if(!_tmp69){_tmp66=f(env,tds);;_pop_handler();}else{void*_tmp68=(void*)
_exn_thrown;void*_tmp6B=_tmp68;struct _dynforward_ptr _tmp6C;struct _dynforward_ptr
_tmp6D;struct _dynforward_ptr _tmp6E;_LL6: if(*((void**)_tmp6B)!= Cyc_Core_Impossible)
goto _LL8;_tmp6C=((struct Cyc_Core_Impossible_struct*)_tmp6B)->f1;_LL7: exn_string=({
const char*_tmp6F="Exception Core::Impossible";_tag_dynforward(_tmp6F,sizeof(char),
_get_zero_arr_size(_tmp6F,27));});explain_string=_tmp6C;goto _LL5;_LL8: if(_tmp6B
!= Cyc_Dict_Absent)goto _LLA;_LL9: exn_string=({const char*_tmp70="Exception Dict::Absent";
_tag_dynforward(_tmp70,sizeof(char),_get_zero_arr_size(_tmp70,23));});goto _LL5;
_LLA: if(*((void**)_tmp6B)!= Cyc_Core_Invalid_argument)goto _LLC;_tmp6D=((struct
Cyc_Core_Invalid_argument_struct*)_tmp6B)->f1;_LLB: exn_string=({const char*_tmp71="Exception Core::Invalid_argument";
_tag_dynforward(_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,33));});
explain_string=_tmp6D;goto _LL5;_LLC: if(*((void**)_tmp6B)!= Cyc_Core_Failure)goto
_LLE;_tmp6E=((struct Cyc_Core_Failure_struct*)_tmp6B)->f1;_LLD: exn_string=({const
char*_tmp72="Exception Core::Failure";_tag_dynforward(_tmp72,sizeof(char),
_get_zero_arr_size(_tmp72,24));});explain_string=_tmp6E;goto _LL5;_LLE:;_LLF: ex=
_tmp6B;other_exn=1;exn_string=({const char*_tmp73="Uncaught exception";
_tag_dynforward(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,19));});goto _LL5;
_LL10:;_LL11:(void)_throw(_tmp6B);_LL5:;}}}if(Cyc_Position_error_p())Cyc_compile_failure=
1;if(Cyc_strcmp((struct _dynforward_ptr)exn_string,({const char*_tmp74="";
_tag_dynforward(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,1));}))!= 0){Cyc_compile_failure=
1;({struct Cyc_String_pa_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)explain_string);{struct Cyc_String_pa_struct
_tmp78;_tmp78.tag=0;_tmp78.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
stage_name);{struct Cyc_String_pa_struct _tmp77;_tmp77.tag=0;_tmp77.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)exn_string);{void*_tmp75[3]={& _tmp77,&
_tmp78,& _tmp79};Cyc_fprintf(Cyc_stderr,({const char*_tmp76="\n%s thrown during %s: %s";
_tag_dynforward(_tmp76,sizeof(char),_get_zero_arr_size(_tmp76,25));}),
_tag_dynforward(_tmp75,sizeof(void*),3));}}}});}if(Cyc_compile_failure){({void*
_tmp7A[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp7B="\nCOMPILATION FAILED!\n";
_tag_dynforward(_tmp7B,sizeof(char),_get_zero_arr_size(_tmp7B,22));}),
_tag_dynforward(_tmp7A,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
if(other_exn)(int)_throw(ex);return _tmp66;}else{if(Cyc_v_r){({struct Cyc_String_pa_struct
_tmp7E;_tmp7E.tag=0;_tmp7E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
stage_name);{void*_tmp7C[1]={& _tmp7E};Cyc_fprintf(Cyc_stderr,({const char*_tmp7D="%s completed.\n";
_tag_dynforward(_tmp7D,sizeof(char),_get_zero_arr_size(_tmp7D,15));}),
_tag_dynforward(_tmp7C,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return _tmp66;}}return _tmp66;}struct Cyc_List_List*Cyc_do_parse(struct
Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*
_tmp84=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);return _tmp84;}}struct _tuple13{
struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(
struct _tuple13*tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp86;struct
Cyc_Tcenv_Tenv*_tmp87;struct _tuple13 _tmp85=*tcenv;_tmp86=_tmp85.f1;_tmp87=_tmp85.f2;
Cyc_Tc_tc(_tmp86,_tmp87,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp87,tds);
return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);return tds;}struct _tuple14{struct Cyc_Tcenv_Tenv*
f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(
struct _tuple14*params,struct Cyc_List_List*tds){struct _tuple14 _tmp89;struct Cyc_Tcenv_Tenv*
_tmp8A;struct Cyc___cycFILE*_tmp8B;struct Cyc___cycFILE*_tmp8C;struct _tuple14*
_tmp88=params;_tmp89=*_tmp88;_tmp8A=_tmp89.f1;_tmp8B=_tmp89.f2;_tmp8C=_tmp89.f3;{
struct Cyc_Interface_I*_tmp8D=Cyc_Interface_extract(_tmp8A->ae);if(_tmp8B == 0)Cyc_Interface_save(
_tmp8D,_tmp8C);else{struct Cyc_Interface_I*_tmp8E=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmp8B);if(!Cyc_Interface_is_subinterface(_tmp8E,_tmp8D,({struct _tuple11*_tmp8F=
_cycalloc(sizeof(*_tmp8F));_tmp8F->f1=({const char*_tmp91="written interface";
_tag_dynforward(_tmp91,sizeof(char),_get_zero_arr_size(_tmp91,18));});_tmp8F->f2=({
const char*_tmp90="maximal interface";_tag_dynforward(_tmp90,sizeof(char),
_get_zero_arr_size(_tmp90,18));});_tmp8F;})))Cyc_compile_failure=1;else{Cyc_Interface_save(
_tmp8E,_tmp8C);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,
struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=Cyc_tovc_r;
return Cyc_Tovc_tovc(tds);}static char _tmp92[1]="";static struct _dynforward_ptr Cyc_cyc_include={
_tmp92,_tmp92 + 1};static void Cyc_set_cyc_include(struct _dynforward_ptr f){Cyc_cyc_include=
f;}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*
tds){struct Cyc_Absynpp_Params _tmp93=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmp93.expand_typedefs=!Cyc_noexpand_r;_tmp93.to_VC=Cyc_tovc_r;_tmp93.add_cyc_prefix=
Cyc_add_cyc_namespace_r;_tmp93.generate_line_directives=Cyc_generate_line_directives_r;
_tmp93.print_full_evars=Cyc_print_full_evars_r;_tmp93.print_all_tvars=Cyc_print_all_tvars_r;
_tmp93.print_all_kinds=Cyc_print_all_kinds_r;_tmp93.print_all_effects=Cyc_print_all_effects_r;
if(Cyc_inline_functions_r)({void*_tmp94[0]={};Cyc_fprintf(out_file,({const char*
_tmp95="#define _INLINE_FUNCTIONS\n";_tag_dynforward(_tmp95,sizeof(char),
_get_zero_arr_size(_tmp95,27));}),_tag_dynforward(_tmp94,sizeof(void*),0));});{
struct Cyc___cycFILE*_tmp96=Cyc_try_file_open(Cyc_cyc_include,({const char*_tmp9C="r";
_tag_dynforward(_tmp9C,sizeof(char),_get_zero_arr_size(_tmp9C,2));}),({const char*
_tmp9D="internal compiler file";_tag_dynforward(_tmp9D,sizeof(char),
_get_zero_arr_size(_tmp9D,23));}));if(_tmp96 == 0)return tds;{unsigned int n_read=
1024;unsigned int n_written;char buf[1024];{unsigned int _tmp9B=1024;unsigned int i;
for(i=0;i < _tmp9B;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=Cyc_fread(
_tag_dynforward(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)_tmp96);if(
n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp96)){Cyc_compile_failure=1;({
void*_tmp97[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp98="\nError: problem copying cyc_include.h\n";
_tag_dynforward(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,39));}),
_tag_dynforward(_tmp97,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
return tds;}n_written=Cyc_fwrite(_tag_dynforward(buf,sizeof(char),1024),1,n_read,
out_file);if(n_read != n_written){Cyc_compile_failure=1;({void*_tmp99[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp9A="\nError: problem copying cyc_include.h\n";
_tag_dynforward(_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,39));}),
_tag_dynforward(_tmp99,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
return tds;}}Cyc_fclose((struct Cyc___cycFILE*)_tmp96);if(Cyc_pp_r){Cyc_Absynpp_set_params(&
_tmp93);Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(&
_tmp93);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}}}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dynforward_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dynforward_ptr s,char c){if(s.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmp9E=0;unsigned int
_tmp9F=Cyc_strlen((struct _dynforward_ptr)s);while(_tmp9F > 0){struct
_dynforward_ptr _tmpA0=Cyc_strchr(s,c);if(_tmpA0.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr){_tmp9E=({struct Cyc_List_List*_tmpA1=_cycalloc(
sizeof(*_tmpA1));_tmpA1->hd=({struct _dynforward_ptr*_tmpA2=_cycalloc(sizeof(*
_tmpA2));_tmpA2[0]=s;_tmpA2;});_tmpA1->tl=_tmp9E;_tmpA1;});break;}else{_tmp9E=({
struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=({struct
_dynforward_ptr*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=(struct
_dynforward_ptr)Cyc_substring((struct _dynforward_ptr)s,0,(unsigned int)((((
struct _dynforward_ptr)_tmpA0).curr - s.curr)/ sizeof(char)));_tmpA4;});_tmpA3->tl=
_tmp9E;_tmpA3;});_tmp9F -=(((struct _dynforward_ptr)_tmpA0).curr - s.curr)/ sizeof(
char);s=_dynforward_ptr_plus(_tmpA0,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9E);}}static int Cyc_file_exists(
struct _dynforward_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmpA5;
_push_handler(& _tmpA5);{int _tmpA7=0;if(setjmp(_tmpA5.handler))_tmpA7=1;if(!
_tmpA7){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({const char*_tmpA8="r";
_tag_dynforward(_tmpA8,sizeof(char),_get_zero_arr_size(_tmpA8,2));}));;
_pop_handler();}else{void*_tmpA6=(void*)_exn_thrown;void*_tmpAA=_tmpA6;_LL13:;
_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmpAA);_LL12:;}}}if(f == 0)return 0;
else{Cyc_fclose((struct Cyc___cycFILE*)f);return 1;}}static struct _dynforward_ptr
Cyc_sprint_list(struct Cyc_List_List*dirs){struct _dynforward_ptr tmp=({const char*
_tmpAD="";_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size(_tmpAD,1));});
for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmpAB=*((struct
_dynforward_ptr*)dirs->hd);if(_tmpAB.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmpAB)== 0)
continue;_tmpAB=(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)
_tmpAB,({const char*_tmpAC=":";_tag_dynforward(_tmpAC,sizeof(char),
_get_zero_arr_size(_tmpAC,2));}));tmp=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)_tmpAB,(struct _dynforward_ptr)tmp);}return tmp;}static
struct _dynforward_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dynforward_ptr
file){if(file.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)return
0;for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmpAE=*((struct
_dynforward_ptr*)dirs->hd);if(_tmpAE.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmpAE)== 0)
continue;{struct _dynforward_ptr s=(struct _dynforward_ptr)Cyc_Filename_concat(
_tmpAE,file);if(Cyc_file_exists(s))return({struct _dynforward_ptr*_tmpAF=
_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=s;_tmpAF;});}}return 0;}static struct
_dynforward_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dynforward_ptr file){
struct _dynforward_ptr*_tmpB0=Cyc_find(dirs,file);if(_tmpB0 == 0){Cyc_compile_failure=
1;Cyc_remove_cfiles();({struct Cyc_String_pa_struct _tmpB4;_tmpB4.tag=0;_tmpB4.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{struct Cyc_String_pa_struct
_tmpB3;_tmpB3.tag=0;_tmpB3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
file);{void*_tmpB1[2]={& _tmpB3,& _tmpB4};Cyc_fprintf(Cyc_stderr,({const char*
_tmpB2="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size(_tmpB2,56));}),
_tag_dynforward(_tmpB1,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpB5=_cycalloc(
sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Core_Failure_struct _tmpB6;_tmpB6.tag=Cyc_Core_Failure;
_tmpB6.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmpBA;_tmpBA.tag=0;
_tmpBA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{
struct Cyc_String_pa_struct _tmpB9;_tmpB9.tag=0;_tmpB9.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmpB7[2]={& _tmpB9,& _tmpBA};Cyc_aprintf(({
const char*_tmpB8="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,56));}),
_tag_dynforward(_tmpB7,sizeof(void*),2));}}});_tmpB6;});_tmpB5;}));}return*
_tmpB0;}static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct
_dynforward_ptr subdir){struct Cyc_List_List*_tmpBB=0;for(0;dirs != 0;dirs=dirs->tl){
_tmpBB=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->hd=({
struct _dynforward_ptr*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=(struct
_dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),subdir);
_tmpBD;});_tmpBC->tl=_tmpBB;_tmpBC;});}_tmpBB=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBB);return _tmpBB;}static struct Cyc_List_List*
Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dynforward_ptr subdir){struct Cyc_List_List*
_tmpBE=0;for(0;dirs != 0;dirs=dirs->tl){_tmpBE=({struct Cyc_List_List*_tmpBF=
_cycalloc(sizeof(*_tmpBF));_tmpBF->hd=(struct _dynforward_ptr*)dirs->hd;_tmpBF->tl=
_tmpBE;_tmpBF;});_tmpBE=({struct Cyc_List_List*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0->hd=({struct _dynforward_ptr*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=(
struct _dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),
subdir);_tmpC1;});_tmpC0->tl=_tmpBE;_tmpC0;});}_tmpBE=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBE);return _tmpBE;}static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dynforward_ptr Cyc_sh_escape_string(
struct _dynforward_ptr s){unsigned int _tmpC2=Cyc_strlen((struct _dynforward_ptr)s);
int _tmpC3=0;int _tmpC4=0;{int i=0;for(0;i < _tmpC2;i ++){char _tmpC5=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(_tmpC5 == '\'')_tmpC3 ++;else{
if(Cyc_is_other_special(_tmpC5))_tmpC4 ++;}}}if(_tmpC3 == 0  && _tmpC4 == 0)return s;
if(_tmpC3 == 0)return(struct _dynforward_ptr)Cyc_strconcat_l(({struct
_dynforward_ptr*_tmpC6[3];_tmpC6[2]=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"'",sizeof(char),2);_tmpC6[1]=({struct _dynforward_ptr*_tmpC8=
_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=(struct _dynforward_ptr)s;_tmpC8;});_tmpC6[0]=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"'",sizeof(char),2);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmpC6,sizeof(struct _dynforward_ptr*),3));}));{unsigned int _tmpCA=(_tmpC2 + 
_tmpC3)+ _tmpC4;struct _dynforward_ptr s2=({unsigned int _tmpD4=_tmpCA + 1;char*
_tmpD5=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpD4 + 1));struct
_dynforward_ptr _tmpD7=_tag_dynforward(_tmpD5,sizeof(char),_tmpD4 + 1);{
unsigned int _tmpD6=_tmpD4;unsigned int i;for(i=0;i < _tmpD6;i ++){_tmpD5[i]='\000';}
_tmpD5[_tmpD6]=(char)0;}_tmpD7;});int _tmpCB=0;int _tmpCC=0;for(0;_tmpCB < _tmpC2;
_tmpCB ++){char _tmpCD=*((const char*)_check_dynforward_subscript(s,sizeof(char),
_tmpCB));if(_tmpCD == '\''  || Cyc_is_other_special(_tmpCD))({struct
_dynforward_ptr _tmpCE=_dynforward_ptr_plus(s2,sizeof(char),_tmpCC ++);char _tmpCF=*((
char*)_check_dynforward_subscript(_tmpCE,sizeof(char),0));char _tmpD0='\\';if(
_get_dynforward_size(_tmpCE,sizeof(char))== 1  && (_tmpCF == '\000'  && _tmpD0 != '\000'))
_throw_arraybounds();*((char*)_tmpCE.curr)=_tmpD0;});({struct _dynforward_ptr
_tmpD1=_dynforward_ptr_plus(s2,sizeof(char),_tmpCC ++);char _tmpD2=*((char*)
_check_dynforward_subscript(_tmpD1,sizeof(char),0));char _tmpD3=_tmpCD;if(
_get_dynforward_size(_tmpD1,sizeof(char))== 1  && (_tmpD2 == '\000'  && _tmpD3 != '\000'))
_throw_arraybounds();*((char*)_tmpD1.curr)=_tmpD3;});}return(struct
_dynforward_ptr)s2;}}static struct _dynforward_ptr*Cyc_sh_escape_stringptr(struct
_dynforward_ptr*sp){return({struct _dynforward_ptr*_tmpD8=_cycalloc(sizeof(*
_tmpD8));_tmpD8[0]=Cyc_sh_escape_string(*sp);_tmpD8;});}static void Cyc_process_file(
struct _dynforward_ptr filename){struct _dynforward_ptr _tmpD9=Cyc_make_base_filename(
filename,Cyc_output_file);struct _dynforward_ptr _tmpDA=Cyc_strconcat((struct
_dynforward_ptr)_tmpD9,({const char*_tmp134=".cyp";_tag_dynforward(_tmp134,
sizeof(char),_get_zero_arr_size(_tmp134,5));}));struct _dynforward_ptr _tmpDB=Cyc_strconcat((
struct _dynforward_ptr)_tmpD9,({const char*_tmp133=".cyci";_tag_dynforward(_tmp133,
sizeof(char),_get_zero_arr_size(_tmp133,6));}));struct _dynforward_ptr _tmpDC=Cyc_strconcat((
struct _dynforward_ptr)_tmpD9,({const char*_tmp132=".cycio";_tag_dynforward(
_tmp132,sizeof(char),_get_zero_arr_size(_tmp132,7));}));struct _dynforward_ptr
_tmpDD=Cyc_strconcat((struct _dynforward_ptr)_tmpD9,({const char*_tmp131=".c";
_tag_dynforward(_tmp131,sizeof(char),_get_zero_arr_size(_tmp131,3));}));if(Cyc_v_r)({
struct Cyc_String_pa_struct _tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)filename);{void*_tmpDE[1]={& _tmpE0};Cyc_fprintf(Cyc_stderr,({
const char*_tmpDF="Compiling %s\n";_tag_dynforward(_tmpDF,sizeof(char),
_get_zero_arr_size(_tmpDF,14));}),_tag_dynforward(_tmpDE,sizeof(void*),1));}});{
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmp12F="r";
_tag_dynforward(_tmp12F,sizeof(char),_get_zero_arr_size(_tmp12F,2));}),({const
char*_tmp130="input file";_tag_dynforward(_tmp130,sizeof(char),
_get_zero_arr_size(_tmp130,11));}));if(Cyc_compile_failure  || !((unsigned int)f0))
return;Cyc_fclose((struct Cyc___cycFILE*)f0);{struct _dynforward_ptr _tmpE1=Cyc_str_sepstr(({
struct Cyc_List_List*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->hd=({struct
_dynforward_ptr*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C[0]=(struct
_dynforward_ptr)({const char*_tmp12D="";_tag_dynforward(_tmp12D,sizeof(char),
_get_zero_arr_size(_tmp12D,1));});_tmp12C;});_tmp12B->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmp12B;}),({const char*_tmp12E=" ";_tag_dynforward(_tmp12E,sizeof(
char),_get_zero_arr_size(_tmp12E,2));}));struct _dynforward_ptr def_inc_path=(
struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*
_tmpE2=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);_tmpE2=Cyc_add_subdir(
_tmpE2,({const char*_tmpE3="include";_tag_dynforward(_tmpE3,sizeof(char),
_get_zero_arr_size(_tmpE3,8));}));if(Cyc_strlen((struct _dynforward_ptr)
def_inc_path)> 0)_tmpE2=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*_tmpE4));
_tmpE4->hd=({struct _dynforward_ptr*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=
def_inc_path;_tmpE5;});_tmpE4->tl=_tmpE2;_tmpE4;});{char*_tmpE6=getenv((const
char*)"CYCLONE_INCLUDE_PATH");if((char*)_tmpE6 != 0)_tmpE2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({
char*_tmpE7=_tmpE6;_tag_dynforward(_tmpE7,sizeof(char),_get_zero_arr_size(_tmpE7,
1));}),':'),_tmpE2);{struct _dynforward_ptr stdinc_string=(struct _dynforward_ptr)
Cyc_str_sepstr(({struct Cyc_List_List*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->hd=({
struct _dynforward_ptr*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=(struct
_dynforward_ptr)({const char*_tmp129="";_tag_dynforward(_tmp129,sizeof(char),
_get_zero_arr_size(_tmp129,1));});_tmp128;});_tmp127->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpE2);_tmp127;}),({const char*_tmp12A=" -I";
_tag_dynforward(_tmp12A,sizeof(char),_get_zero_arr_size(_tmp12A,4));}));struct
_dynforward_ptr ofile_string;if(Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)
ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmpEA;_tmpEA.tag=
0;_tmpEA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)));{void*_tmpE8[1]={& _tmpEA};Cyc_aprintf(({
const char*_tmpE9=" > %s";_tag_dynforward(_tmpE9,sizeof(char),_get_zero_arr_size(
_tmpE9,6));}),_tag_dynforward(_tmpE8,sizeof(void*),1));}});else{ofile_string=({
const char*_tmpEB="";_tag_dynforward(_tmpEB,sizeof(char),_get_zero_arr_size(
_tmpEB,1));});}}else{ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string((
struct _dynforward_ptr)_tmpDA));{void*_tmpEC[1]={& _tmpEE};Cyc_aprintf(({const char*
_tmpED=" > %s";_tag_dynforward(_tmpED,sizeof(char),_get_zero_arr_size(_tmpED,6));}),
_tag_dynforward(_tmpEC,sizeof(void*),1));}});}{struct _dynforward_ptr fixup_string;
if(Cyc_produce_dependencies){if(Cyc_dependencies_target == 0)fixup_string=({const
char*_tmpEF=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_dynforward(_tmpEF,
sizeof(char),_get_zero_arr_size(_tmpEF,35));});else{fixup_string=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_null(Cyc_dependencies_target)));{void*_tmpF0[1]={& _tmpF2};Cyc_aprintf(({
const char*_tmpF1=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dynforward(_tmpF1,sizeof(
char),_get_zero_arr_size(_tmpF1,29));}),_tag_dynforward(_tmpF0,sizeof(void*),1));}});}}
else{fixup_string=({const char*_tmpF3="";_tag_dynforward(_tmpF3,sizeof(char),
_get_zero_arr_size(_tmpF3,1));});}{struct _dynforward_ptr _tmpF4=({struct Cyc_String_pa_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
ofile_string);{struct Cyc_String_pa_struct _tmp125;_tmp125.tag=0;_tmp125.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)fixup_string);{struct Cyc_String_pa_struct
_tmp124;_tmp124.tag=0;_tmp124.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_sh_escape_string(filename));{struct Cyc_String_pa_struct _tmp123;_tmp123.tag=0;
_tmp123.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)stdinc_string);{struct
Cyc_String_pa_struct _tmp122;_tmp122.tag=0;_tmp122.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmpE1);{struct Cyc_String_pa_struct _tmp121;_tmp121.tag=0;
_tmp121.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_cpp);{void*_tmp11F[
6]={& _tmp121,& _tmp122,& _tmp123,& _tmp124,& _tmp125,& _tmp126};Cyc_aprintf(({const
char*_tmp120="%s %s%s %s%s%s";_tag_dynforward(_tmp120,sizeof(char),
_get_zero_arr_size(_tmp120,15));}),_tag_dynforward(_tmp11F,sizeof(void*),6));}}}}}}});
if(Cyc_v_r)({struct Cyc_String_pa_struct _tmpF7;_tmpF7.tag=0;_tmpF7.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpF4);{void*_tmpF5[1]={& _tmpF7};Cyc_fprintf(
Cyc_stderr,({const char*_tmpF6="%s\n";_tag_dynforward(_tmpF6,sizeof(char),
_get_zero_arr_size(_tmpF6,4));}),_tag_dynforward(_tmpF5,sizeof(void*),1));}});
if(system((const char*)_untag_dynforward_ptr(_tmpF4,sizeof(char),1))!= 0){Cyc_compile_failure=
1;({void*_tmpF8[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF9="\nError: preprocessing\n";
_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(_tmpF9,23));}),
_tag_dynforward(_tmpF8,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)
return;Cyc_Position_reset_position((struct _dynforward_ptr)_tmpDA);{struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dynforward_ptr)_tmpDA,({const char*_tmp11D="r";
_tag_dynforward(_tmp11D,sizeof(char),_get_zero_arr_size(_tmp11D,2));}),({const
char*_tmp11E="file";_tag_dynforward(_tmp11E,sizeof(char),_get_zero_arr_size(
_tmp11E,5));}));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{struct
_handler_cons _tmpFA;_push_handler(& _tmpFA);{int _tmpFC=0;if(setjmp(_tmpFA.handler))
_tmpFC=1;if(!_tmpFC){tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),
struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*
_tmpFD="parsing";_tag_dynforward(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,8));}),
Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);;_pop_handler();}
else{void*_tmpFB=(void*)_exn_thrown;void*_tmpFF=_tmpFB;_LL27:;_LL28: Cyc_file_close((
struct Cyc___cycFILE*)_check_null(in_file));Cyc_remove_file((struct
_dynforward_ptr)_tmpDA);(int)_throw(_tmpFF);_LL29:;_LL2A:(void)_throw(_tmpFF);
_LL26:;}}}Cyc_file_close((struct Cyc___cycFILE*)in_file);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpDA);return;}if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);return;}{struct _RegionHandle _tmp100=_new_region("tc_rgn");
struct _RegionHandle*tc_rgn=& _tmp100;_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*
_tmp101=Cyc_Tcenv_tc_init(tc_rgn);if(Cyc_parseonly_r){_npop_handler(0);goto
PRINTC;}{struct _tuple13 _tmp102=({struct _tuple13 _tmp10A;_tmp10A.f1=tc_rgn;_tmp10A.f2=
_tmp101;_tmp10A;});tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,
struct Cyc_List_List*(*f)(struct _tuple13*,struct Cyc_List_List*),struct _tuple13*
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp103="type checking";
_tag_dynforward(_tmp103,sizeof(char),_get_zero_arr_size(_tmp103,14));}),Cyc_do_typecheck,&
_tmp102,tds);if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)
_tmpDA);_npop_handler(0);return;}if(Cyc_tc_r){_npop_handler(0);goto PRINTC;}tds=((
struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const
char*_tmp104="control-flow checking";_tag_dynforward(_tmp104,sizeof(char),
_get_zero_arr_size(_tmp104,22));}),Cyc_do_cfcheck,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpDA);_npop_handler(0);return;}if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)((char*)
_untag_dynforward_ptr(_tmpDB,sizeof(char),1)),(const char*)"r");struct Cyc___cycFILE*
inter_objfile=Cyc_try_file_open((struct _dynforward_ptr)_tmpDC,({const char*
_tmp108="w";_tag_dynforward(_tmp108,sizeof(char),_get_zero_arr_size(_tmp108,2));}),({
const char*_tmp109="interface object file";_tag_dynforward(_tmp109,sizeof(char),
_get_zero_arr_size(_tmp109,22));}));if(inter_objfile == 0){Cyc_compile_failure=1;
_npop_handler(0);return;}Cyc_Position_reset_position((struct _dynforward_ptr)
_tmpDB);{struct _tuple14 _tmp105=({struct _tuple14 _tmp107;_tmp107.f1=_tmp101;
_tmp107.f2=inter_file;_tmp107.f3=(struct Cyc___cycFILE*)inter_objfile;_tmp107;});
tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*
f)(struct _tuple14*,struct Cyc_List_List*),struct _tuple14*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp106="interface checking";_tag_dynforward(
_tmp106,sizeof(char),_get_zero_arr_size(_tmp106,19));}),Cyc_do_interface,&
_tmp105,tds);if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);}}}};_pop_region(tc_rgn);}
if(Cyc_cf_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp10B="translation to C";_tag_dynforward(
_tmp10B,sizeof(char),_get_zero_arr_size(_tmp10B,17));}),Cyc_do_translate,1,tds);
if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpDA);return;}
if(!Cyc_compile_failure)Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct
_dynforward_ptr)_tmpDA);if(Cyc_compile_failure)return;if(Cyc_tovc_r  || Cyc_elim_se_r)
tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp10C="post-pass to VC";_tag_dynforward(_tmp10C,sizeof(char),
_get_zero_arr_size(_tmp10C,16));}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure)
return;PRINTC: {struct Cyc___cycFILE*out_file;if((Cyc_parseonly_r  || Cyc_tc_r)
 || Cyc_cf_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp10D="w";
_tag_dynforward(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,2));}),({const
char*_tmp10E="output file";_tag_dynforward(_tmp10E,sizeof(char),
_get_zero_arr_size(_tmp10E,12));}));else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}
else{if(Cyc_toc_r  && Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp10F="w";
_tag_dynforward(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,2));}),({const
char*_tmp110="output file";_tag_dynforward(_tmp110,sizeof(char),
_get_zero_arr_size(_tmp110,12));}));else{out_file=Cyc_try_file_open((struct
_dynforward_ptr)_tmpDD,({const char*_tmp111="w";_tag_dynforward(_tmp111,sizeof(
char),_get_zero_arr_size(_tmp111,2));}),({const char*_tmp112="output file";
_tag_dynforward(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,12));}));}}if(
Cyc_compile_failure  || !((unsigned int)out_file))return;{struct _handler_cons
_tmp113;_push_handler(& _tmp113);{int _tmp115=0;if(setjmp(_tmp113.handler))_tmp115=
1;if(!_tmp115){if(!Cyc_noprint_r)tds=((struct Cyc_List_List*(*)(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct
Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp116="printing";_tag_dynforward(_tmp116,sizeof(char),
_get_zero_arr_size(_tmp116,9));}),Cyc_do_print,(struct Cyc___cycFILE*)out_file,
tds);;_pop_handler();}else{void*_tmp114=(void*)_exn_thrown;void*_tmp118=_tmp114;
_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((struct Cyc___cycFILE*)out_file);
Cyc_cfiles=({struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=({
struct _dynforward_ptr*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=(struct
_dynforward_ptr)_tmpDD;_tmp11A;});_tmp119->tl=Cyc_cfiles;_tmp119;});(int)_throw(
_tmp118);_LL2E:;_LL2F:(void)_throw(_tmp118);_LL2B:;}}}Cyc_file_close((struct Cyc___cycFILE*)
out_file);Cyc_cfiles=({struct Cyc_List_List*_tmp11B=_cycalloc(sizeof(*_tmp11B));
_tmp11B->hd=({struct _dynforward_ptr*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[
0]=(struct _dynforward_ptr)_tmpDD;_tmp11C;});_tmp11B->tl=Cyc_cfiles;_tmp11B;});}}}}}}}}}}
static char _tmp135[8]="<final>";static struct _dynforward_ptr Cyc_final_str={_tmp135,
_tmp135 + 8};static struct _dynforward_ptr*Cyc_final_strptr=& Cyc_final_str;static
struct Cyc_Interface_I*Cyc_read_cycio(struct _dynforward_ptr*n){if(n == (struct
_dynforward_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{struct
_dynforward_ptr basename;{struct _handler_cons _tmp136;_push_handler(& _tmp136);{int
_tmp138=0;if(setjmp(_tmp136.handler))_tmp138=1;if(!_tmp138){basename=(struct
_dynforward_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp137=(void*)_exn_thrown;void*_tmp13A=_tmp137;_LL31: if(*((void**)_tmp13A)!= 
Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(
void)_throw(_tmp13A);_LL30:;}}}{struct _dynforward_ptr _tmp13B=Cyc_strconcat((
struct _dynforward_ptr)basename,({const char*_tmp140=".cycio";_tag_dynforward(
_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,7));}));struct Cyc___cycFILE*
_tmp13C=Cyc_try_file_open((struct _dynforward_ptr)_tmp13B,({const char*_tmp13E="rb";
_tag_dynforward(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,3));}),({const
char*_tmp13F="interface object file";_tag_dynforward(_tmp13F,sizeof(char),
_get_zero_arr_size(_tmp13F,22));}));if(_tmp13C == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();
exit(1);}Cyc_Position_reset_position((struct _dynforward_ptr)_tmp13B);{struct Cyc_Interface_I*
_tmp13D=Cyc_Interface_load((struct Cyc___cycFILE*)_tmp13C);Cyc_file_close((struct
Cyc___cycFILE*)_tmp13C);return _tmp13D;}}}}static int Cyc_is_cfile(struct
_dynforward_ptr*n){return*((const char*)_check_dynforward_subscript(*n,sizeof(
char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple15{struct
_dynforward_ptr f1;int f2;struct _dynforward_ptr f3;void*f4;struct _dynforward_ptr f5;
};int Cyc_main(int argc,struct _dynforward_ptr argv){GC_blacklist_warn_clear();{
struct Cyc_List_List*cyclone_arch_path;struct _dynforward_ptr def_lib_path;struct
_dynforward_ptr comp=(struct _dynforward_ptr)Cstring_to_string(Ccomp);{struct
_RegionHandle _tmp141=_new_region("r");struct _RegionHandle*r=& _tmp141;
_push_region(r);{struct _dynforward_ptr _tmp142=({unsigned int _tmp2DF=(
unsigned int)argc;int*_tmp2E0=(int*)_cycalloc_atomic(_check_times(sizeof(int),
_tmp2DF));struct _dynforward_ptr _tmp2E2=_tag_dynforward(_tmp2E0,sizeof(int),
_tmp2DF);{unsigned int _tmp2E1=_tmp2DF;unsigned int i;for(i=0;i < _tmp2E1;i ++){
_tmp2E0[i]=0;}}_tmp2E2;});int _tmp143=0;int i;int j;int k;for(i=1;i < argc;i ++){if(
Cyc_strncmp(({const char*_tmp144="-B";_tag_dynforward(_tmp144,sizeof(char),
_get_zero_arr_size(_tmp144,3));}),(struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
i)),2)== 0){*((int*)_check_dynforward_subscript(_tmp142,sizeof(int),i))=1;
_tmp143 ++;}else{if(Cyc_strcmp(({const char*_tmp145="-b";_tag_dynforward(_tmp145,
sizeof(char),_get_zero_arr_size(_tmp145,3));}),(struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
i)))== 0){*((int*)_check_dynforward_subscript(_tmp142,sizeof(int),i))=1;_tmp143
++;i ++;if(i >= argc)break;*((int*)_check_dynforward_subscript(_tmp142,sizeof(int),
i))=1;_tmp143 ++;}}}{struct _dynforward_ptr _tmp146=({unsigned int _tmp2DB=(
unsigned int)(_tmp143 + 1);struct _dynforward_ptr*_tmp2DC=(struct _dynforward_ptr*)
_cycalloc(_check_times(sizeof(struct _dynforward_ptr),_tmp2DB));struct
_dynforward_ptr _tmp2DE=_tag_dynforward(_tmp2DC,sizeof(struct _dynforward_ptr),
_tmp2DB);{unsigned int _tmp2DD=_tmp2DB;unsigned int n;for(n=0;n < _tmp2DD;n ++){
_tmp2DC[n]=(struct _dynforward_ptr)_tag_dynforward(0,0,0);}}_tmp2DE;});struct
_dynforward_ptr _tmp147=({unsigned int _tmp2D7=(unsigned int)(argc - _tmp143);
struct _dynforward_ptr*_tmp2D8=(struct _dynforward_ptr*)_cycalloc(_check_times(
sizeof(struct _dynforward_ptr),_tmp2D7));struct _dynforward_ptr _tmp2DA=
_tag_dynforward(_tmp2D8,sizeof(struct _dynforward_ptr),_tmp2D7);{unsigned int
_tmp2D9=_tmp2D7;unsigned int n;for(n=0;n < _tmp2D9;n ++){_tmp2D8[n]=(struct
_dynforward_ptr)_tag_dynforward(0,0,0);}}_tmp2DA;});*((struct _dynforward_ptr*)
_check_dynforward_subscript(_tmp146,sizeof(struct _dynforward_ptr),0))=(*((struct
_dynforward_ptr*)_check_dynforward_subscript(_tmp147,sizeof(struct
_dynforward_ptr),0))=*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),0)));for(i=(j=(k=1));i < argc;i ++){if(*((int*)
_check_dynforward_subscript(_tmp142,sizeof(int),i)))*((struct _dynforward_ptr*)
_check_dynforward_subscript(_tmp146,sizeof(struct _dynforward_ptr),j ++))=*((
struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct
_dynforward_ptr),i));else{*((struct _dynforward_ptr*)_check_dynforward_subscript(
_tmp147,sizeof(struct _dynforward_ptr),k ++))=*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),i));}}{struct Cyc_List_List*
options=({struct _tuple15*_tmp162[62];_tmp162[61]=({struct _tuple15*_tmp2D1=
_region_malloc(r,sizeof(*_tmp2D1));_tmp2D1->f1=({const char*_tmp2D6="-noregions";
_tag_dynforward(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,11));});_tmp2D1->f2=
0;_tmp2D1->f3=({const char*_tmp2D5="";_tag_dynforward(_tmp2D5,sizeof(char),
_get_zero_arr_size(_tmp2D5,1));});_tmp2D1->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2D3=_region_malloc(r,sizeof(*_tmp2D3));_tmp2D3[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2D4;_tmp2D4.tag=3;_tmp2D4.f1=& Cyc_Absyn_no_regions;_tmp2D4;});_tmp2D3;});
_tmp2D1->f5=({const char*_tmp2D2="Generate code that doesn't use regions";
_tag_dynforward(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,39));});_tmp2D1;});
_tmp162[60]=({struct _tuple15*_tmp2CB=_region_malloc(r,sizeof(*_tmp2CB));_tmp2CB->f1=({
const char*_tmp2D0="-port";_tag_dynforward(_tmp2D0,sizeof(char),
_get_zero_arr_size(_tmp2D0,6));});_tmp2CB->f2=0;_tmp2CB->f3=({const char*_tmp2CF="";
_tag_dynforward(_tmp2CF,sizeof(char),_get_zero_arr_size(_tmp2CF,1));});_tmp2CB->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp2CD=_region_malloc(r,sizeof(*_tmp2CD));
_tmp2CD[0]=({struct Cyc_Arg_Unit_spec_struct _tmp2CE;_tmp2CE.tag=0;_tmp2CE.f1=Cyc_set_port_c_code;
_tmp2CE;});_tmp2CD;});_tmp2CB->f5=({const char*_tmp2CC="Suggest how to port C code to Cyclone";
_tag_dynforward(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,38));});_tmp2CB;});
_tmp162[59]=({struct _tuple15*_tmp2C5=_region_malloc(r,sizeof(*_tmp2C5));_tmp2C5->f1=({
const char*_tmp2CA="-detailedlocation";_tag_dynforward(_tmp2CA,sizeof(char),
_get_zero_arr_size(_tmp2CA,18));});_tmp2C5->f2=0;_tmp2C5->f3=({const char*_tmp2C9="";
_tag_dynforward(_tmp2C9,sizeof(char),_get_zero_arr_size(_tmp2C9,1));});_tmp2C5->f4=(
void*)({struct Cyc_Arg_Clear_spec_struct*_tmp2C7=_region_malloc(r,sizeof(*_tmp2C7));
_tmp2C7[0]=({struct Cyc_Arg_Clear_spec_struct _tmp2C8;_tmp2C8.tag=4;_tmp2C8.f1=&
Cyc_Position_use_gcc_style_location;_tmp2C8;});_tmp2C7;});_tmp2C5->f5=({const
char*_tmp2C6="Try to give more detailed location information for errors";
_tag_dynforward(_tmp2C6,sizeof(char),_get_zero_arr_size(_tmp2C6,58));});_tmp2C5;});
_tmp162[58]=({struct _tuple15*_tmp2BF=_region_malloc(r,sizeof(*_tmp2BF));_tmp2BF->f1=({
const char*_tmp2C4="-noregister";_tag_dynforward(_tmp2C4,sizeof(char),
_get_zero_arr_size(_tmp2C4,12));});_tmp2BF->f2=0;_tmp2BF->f3=({const char*_tmp2C3="";
_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,1));});_tmp2BF->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2C1=_region_malloc(r,sizeof(*_tmp2C1));
_tmp2C1[0]=({struct Cyc_Arg_Set_spec_struct _tmp2C2;_tmp2C2.tag=3;_tmp2C2.f1=& Cyc_Parse_no_register;
_tmp2C2;});_tmp2C1;});_tmp2BF->f5=({const char*_tmp2C0="Treat register storage class as public";
_tag_dynforward(_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,39));});_tmp2BF;});
_tmp162[57]=({struct _tuple15*_tmp2B9=_region_malloc(r,sizeof(*_tmp2B9));_tmp2B9->f1=({
const char*_tmp2BE="-warnnullchecks";_tag_dynforward(_tmp2BE,sizeof(char),
_get_zero_arr_size(_tmp2BE,16));});_tmp2B9->f2=0;_tmp2B9->f3=({const char*_tmp2BD="";
_tag_dynforward(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,1));});_tmp2B9->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2BB=_region_malloc(r,sizeof(*_tmp2BB));
_tmp2BB[0]=({struct Cyc_Arg_Set_spec_struct _tmp2BC;_tmp2BC.tag=3;_tmp2BC.f1=& Cyc_Toc_warn_all_null_deref;
_tmp2BC;});_tmp2BB;});_tmp2B9->f5=({const char*_tmp2BA="Warn when any null check can't be eliminated";
_tag_dynforward(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,45));});_tmp2B9;});
_tmp162[56]=({struct _tuple15*_tmp2B3=_region_malloc(r,sizeof(*_tmp2B3));_tmp2B3->f1=({
const char*_tmp2B8="-warnboundschecks";_tag_dynforward(_tmp2B8,sizeof(char),
_get_zero_arr_size(_tmp2B8,18));});_tmp2B3->f2=0;_tmp2B3->f3=({const char*_tmp2B7="";
_tag_dynforward(_tmp2B7,sizeof(char),_get_zero_arr_size(_tmp2B7,1));});_tmp2B3->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2B5=_region_malloc(r,sizeof(*_tmp2B5));
_tmp2B5[0]=({struct Cyc_Arg_Set_spec_struct _tmp2B6;_tmp2B6.tag=3;_tmp2B6.f1=& Cyc_Toc_warn_bounds_checks;
_tmp2B6;});_tmp2B5;});_tmp2B3->f5=({const char*_tmp2B4="Warn when bounds checks can't be eliminated";
_tag_dynforward(_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,44));});_tmp2B3;});
_tmp162[55]=({struct _tuple15*_tmp2AD=_region_malloc(r,sizeof(*_tmp2AD));_tmp2AD->f1=({
const char*_tmp2B2="-CI";_tag_dynforward(_tmp2B2,sizeof(char),_get_zero_arr_size(
_tmp2B2,4));});_tmp2AD->f2=0;_tmp2AD->f3=({const char*_tmp2B1=" <file>";
_tag_dynforward(_tmp2B1,sizeof(char),_get_zero_arr_size(_tmp2B1,8));});_tmp2AD->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp2AF=_region_malloc(r,sizeof(*
_tmp2AF));_tmp2AF[0]=({struct Cyc_Arg_String_spec_struct _tmp2B0;_tmp2B0.tag=5;
_tmp2B0.f1=Cyc_set_cyc_include;_tmp2B0;});_tmp2AF;});_tmp2AD->f5=({const char*
_tmp2AE="Set cyc_include.h to be <file>";_tag_dynforward(_tmp2AE,sizeof(char),
_get_zero_arr_size(_tmp2AE,31));});_tmp2AD;});_tmp162[54]=({struct _tuple15*
_tmp2A7=_region_malloc(r,sizeof(*_tmp2A7));_tmp2A7->f1=({const char*_tmp2AC="-printalleffects";
_tag_dynforward(_tmp2AC,sizeof(char),_get_zero_arr_size(_tmp2AC,17));});_tmp2A7->f2=
0;_tmp2A7->f3=({const char*_tmp2AB="";_tag_dynforward(_tmp2AB,sizeof(char),
_get_zero_arr_size(_tmp2AB,1));});_tmp2A7->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2A9=_region_malloc(r,sizeof(*_tmp2A9));_tmp2A9[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2AA;_tmp2AA.tag=3;_tmp2AA.f1=& Cyc_print_all_effects_r;_tmp2AA;});_tmp2A9;});
_tmp2A7->f5=({const char*_tmp2A8="Print effects for functions (type debugging)";
_tag_dynforward(_tmp2A8,sizeof(char),_get_zero_arr_size(_tmp2A8,45));});_tmp2A7;});
_tmp162[53]=({struct _tuple15*_tmp2A1=_region_malloc(r,sizeof(*_tmp2A1));_tmp2A1->f1=({
const char*_tmp2A6="-printfullevars";_tag_dynforward(_tmp2A6,sizeof(char),
_get_zero_arr_size(_tmp2A6,16));});_tmp2A1->f2=0;_tmp2A1->f3=({const char*_tmp2A5="";
_tag_dynforward(_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,1));});_tmp2A1->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2A3=_region_malloc(r,sizeof(*_tmp2A3));
_tmp2A3[0]=({struct Cyc_Arg_Set_spec_struct _tmp2A4;_tmp2A4.tag=3;_tmp2A4.f1=& Cyc_print_full_evars_r;
_tmp2A4;});_tmp2A3;});_tmp2A1->f5=({const char*_tmp2A2="Print full information for evars (type debugging)";
_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,50));});_tmp2A1;});
_tmp162[52]=({struct _tuple15*_tmp29B=_region_malloc(r,sizeof(*_tmp29B));_tmp29B->f1=({
const char*_tmp2A0="-printallkinds";_tag_dynforward(_tmp2A0,sizeof(char),
_get_zero_arr_size(_tmp2A0,15));});_tmp29B->f2=0;_tmp29B->f3=({const char*_tmp29F="";
_tag_dynforward(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,1));});_tmp29B->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp29D=_region_malloc(r,sizeof(*_tmp29D));
_tmp29D[0]=({struct Cyc_Arg_Set_spec_struct _tmp29E;_tmp29E.tag=3;_tmp29E.f1=& Cyc_print_all_kinds_r;
_tmp29E;});_tmp29D;});_tmp29B->f5=({const char*_tmp29C="Always print kinds of type variables";
_tag_dynforward(_tmp29C,sizeof(char),_get_zero_arr_size(_tmp29C,37));});_tmp29B;});
_tmp162[51]=({struct _tuple15*_tmp295=_region_malloc(r,sizeof(*_tmp295));_tmp295->f1=({
const char*_tmp29A="-printalltvars";_tag_dynforward(_tmp29A,sizeof(char),
_get_zero_arr_size(_tmp29A,15));});_tmp295->f2=0;_tmp295->f3=({const char*_tmp299="";
_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size(_tmp299,1));});_tmp295->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp297=_region_malloc(r,sizeof(*_tmp297));
_tmp297[0]=({struct Cyc_Arg_Set_spec_struct _tmp298;_tmp298.tag=3;_tmp298.f1=& Cyc_print_all_tvars_r;
_tmp298;});_tmp297;});_tmp295->f5=({const char*_tmp296="Print all type variables (even implicit default effects)";
_tag_dynforward(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,57));});_tmp295;});
_tmp162[50]=({struct _tuple15*_tmp28F=_region_malloc(r,sizeof(*_tmp28F));_tmp28F->f1=({
const char*_tmp294="-noexpandtypedefs";_tag_dynforward(_tmp294,sizeof(char),
_get_zero_arr_size(_tmp294,18));});_tmp28F->f2=0;_tmp28F->f3=({const char*_tmp293="";
_tag_dynforward(_tmp293,sizeof(char),_get_zero_arr_size(_tmp293,1));});_tmp28F->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp291=_region_malloc(r,sizeof(*_tmp291));
_tmp291[0]=({struct Cyc_Arg_Set_spec_struct _tmp292;_tmp292.tag=3;_tmp292.f1=& Cyc_noexpand_r;
_tmp292;});_tmp291;});_tmp28F->f5=({const char*_tmp290="Don't expand typedefs in pretty printing";
_tag_dynforward(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,41));});_tmp28F;});
_tmp162[49]=({struct _tuple15*_tmp289=_region_malloc(r,sizeof(*_tmp289));_tmp289->f1=({
const char*_tmp28E="-noremoveunused";_tag_dynforward(_tmp28E,sizeof(char),
_get_zero_arr_size(_tmp28E,16));});_tmp289->f2=0;_tmp289->f3=({const char*_tmp28D="";
_tag_dynforward(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,1));});_tmp289->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp28B=_region_malloc(r,sizeof(*_tmp28B));
_tmp28B[0]=({struct Cyc_Arg_Set_spec_struct _tmp28C;_tmp28C.tag=3;_tmp28C.f1=& Cyc_noshake_r;
_tmp28C;});_tmp28B;});_tmp289->f5=({const char*_tmp28A="Don't remove externed variables that aren't used";
_tag_dynforward(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,49));});_tmp289;});
_tmp162[48]=({struct _tuple15*_tmp283=_region_malloc(r,sizeof(*_tmp283));_tmp283->f1=({
const char*_tmp288="-nogc";_tag_dynforward(_tmp288,sizeof(char),
_get_zero_arr_size(_tmp288,6));});_tmp283->f2=0;_tmp283->f3=({const char*_tmp287="";
_tag_dynforward(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,1));});_tmp283->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp285=_region_malloc(r,sizeof(*_tmp285));
_tmp285[0]=({struct Cyc_Arg_Set_spec_struct _tmp286;_tmp286.tag=3;_tmp286.f1=& Cyc_nogc_r;
_tmp286;});_tmp285;});_tmp283->f5=({const char*_tmp284="Don't link in the garbage collector";
_tag_dynforward(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,36));});_tmp283;});
_tmp162[47]=({struct _tuple15*_tmp27D=_region_malloc(r,sizeof(*_tmp27D));_tmp27D->f1=({
const char*_tmp282="-nocyc";_tag_dynforward(_tmp282,sizeof(char),
_get_zero_arr_size(_tmp282,7));});_tmp27D->f2=0;_tmp27D->f3=({const char*_tmp281="";
_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(_tmp281,1));});_tmp27D->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp27F=_region_malloc(r,sizeof(*_tmp27F));
_tmp27F[0]=({struct Cyc_Arg_Unit_spec_struct _tmp280;_tmp280.tag=0;_tmp280.f1=Cyc_set_nocyc;
_tmp280;});_tmp27F;});_tmp27D->f5=({const char*_tmp27E="Don't add implicit namespace Cyc";
_tag_dynforward(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,33));});_tmp27D;});
_tmp162[46]=({struct _tuple15*_tmp277=_region_malloc(r,sizeof(*_tmp277));_tmp277->f1=({
const char*_tmp27C="-no-cpp-precomp";_tag_dynforward(_tmp27C,sizeof(char),
_get_zero_arr_size(_tmp27C,16));});_tmp277->f2=0;_tmp277->f3=({const char*_tmp27B="";
_tag_dynforward(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,1));});_tmp277->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp279=_region_malloc(r,sizeof(*_tmp279));
_tmp279[0]=({struct Cyc_Arg_Unit_spec_struct _tmp27A;_tmp27A.tag=0;_tmp27A.f1=Cyc_set_nocppprecomp;
_tmp27A;});_tmp279;});_tmp277->f5=({const char*_tmp278="Don't do smart preprocessing (mac only)";
_tag_dynforward(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,40));});_tmp277;});
_tmp162[45]=({struct _tuple15*_tmp271=_region_malloc(r,sizeof(*_tmp271));_tmp271->f1=({
const char*_tmp276="-use-cpp";_tag_dynforward(_tmp276,sizeof(char),
_get_zero_arr_size(_tmp276,9));});_tmp271->f2=0;_tmp271->f3=({const char*_tmp275="<path>";
_tag_dynforward(_tmp275,sizeof(char),_get_zero_arr_size(_tmp275,7));});_tmp271->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp273=_region_malloc(r,sizeof(*
_tmp273));_tmp273[0]=({struct Cyc_Arg_String_spec_struct _tmp274;_tmp274.tag=5;
_tmp274.f1=Cyc_set_cpp;_tmp274;});_tmp273;});_tmp271->f5=({const char*_tmp272="Indicate which preprocessor to use";
_tag_dynforward(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,35));});_tmp271;});
_tmp162[44]=({struct _tuple15*_tmp26B=_region_malloc(r,sizeof(*_tmp26B));_tmp26B->f1=({
const char*_tmp270="--inline-checks";_tag_dynforward(_tmp270,sizeof(char),
_get_zero_arr_size(_tmp270,16));});_tmp26B->f2=0;_tmp26B->f3=({const char*_tmp26F="";
_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,1));});_tmp26B->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp26D=_region_malloc(r,sizeof(*_tmp26D));
_tmp26D[0]=({struct Cyc_Arg_Unit_spec_struct _tmp26E;_tmp26E.tag=0;_tmp26E.f1=Cyc_set_inline_functions;
_tmp26E;});_tmp26D;});_tmp26B->f5=({const char*_tmp26C="Inline bounds checks instead of #define";
_tag_dynforward(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,40));});_tmp26B;});
_tmp162[43]=({struct _tuple15*_tmp265=_region_malloc(r,sizeof(*_tmp265));_tmp265->f1=({
const char*_tmp26A="--noboundschecks";_tag_dynforward(_tmp26A,sizeof(char),
_get_zero_arr_size(_tmp26A,17));});_tmp265->f2=0;_tmp265->f3=({const char*_tmp269="";
_tag_dynforward(_tmp269,sizeof(char),_get_zero_arr_size(_tmp269,1));});_tmp265->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp267=_region_malloc(r,sizeof(*_tmp267));
_tmp267[0]=({struct Cyc_Arg_Unit_spec_struct _tmp268;_tmp268.tag=0;_tmp268.f1=Cyc_set_noboundschecks;
_tmp268;});_tmp267;});_tmp265->f5=({const char*_tmp266="Disable bounds checks";
_tag_dynforward(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,22));});_tmp265;});
_tmp162[42]=({struct _tuple15*_tmp25F=_region_malloc(r,sizeof(*_tmp25F));_tmp25F->f1=({
const char*_tmp264="--nonullchecks";_tag_dynforward(_tmp264,sizeof(char),
_get_zero_arr_size(_tmp264,15));});_tmp25F->f2=0;_tmp25F->f3=({const char*_tmp263="";
_tag_dynforward(_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,1));});_tmp25F->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp261=_region_malloc(r,sizeof(*_tmp261));
_tmp261[0]=({struct Cyc_Arg_Unit_spec_struct _tmp262;_tmp262.tag=0;_tmp262.f1=Cyc_set_nonullchecks;
_tmp262;});_tmp261;});_tmp25F->f5=({const char*_tmp260="Disable null checks";
_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,20));});_tmp25F;});
_tmp162[41]=({struct _tuple15*_tmp259=_region_malloc(r,sizeof(*_tmp259));_tmp259->f1=({
const char*_tmp25E="--nochecks";_tag_dynforward(_tmp25E,sizeof(char),
_get_zero_arr_size(_tmp25E,11));});_tmp259->f2=0;_tmp259->f3=({const char*_tmp25D="";
_tag_dynforward(_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,1));});_tmp259->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp25B=_region_malloc(r,sizeof(*_tmp25B));
_tmp25B[0]=({struct Cyc_Arg_Unit_spec_struct _tmp25C;_tmp25C.tag=0;_tmp25C.f1=Cyc_set_nochecks;
_tmp25C;});_tmp25B;});_tmp259->f5=({const char*_tmp25A="Disable bounds/null checks";
_tag_dynforward(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,27));});_tmp259;});
_tmp162[40]=({struct _tuple15*_tmp253=_region_malloc(r,sizeof(*_tmp253));_tmp253->f1=({
const char*_tmp258="--lineno";_tag_dynforward(_tmp258,sizeof(char),
_get_zero_arr_size(_tmp258,9));});_tmp253->f2=0;_tmp253->f3=({const char*_tmp257="";
_tag_dynforward(_tmp257,sizeof(char),_get_zero_arr_size(_tmp257,1));});_tmp253->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp255=_region_malloc(r,sizeof(*_tmp255));
_tmp255[0]=({struct Cyc_Arg_Unit_spec_struct _tmp256;_tmp256.tag=0;_tmp256.f1=Cyc_set_lineno;
_tmp256;});_tmp255;});_tmp253->f5=({const char*_tmp254="Generate line numbers for debugging";
_tag_dynforward(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,36));});_tmp253;});
_tmp162[39]=({struct _tuple15*_tmp24D=_region_malloc(r,sizeof(*_tmp24D));_tmp24D->f1=({
const char*_tmp252="-save-c";_tag_dynforward(_tmp252,sizeof(char),
_get_zero_arr_size(_tmp252,8));});_tmp24D->f2=0;_tmp24D->f3=({const char*_tmp251="";
_tag_dynforward(_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,1));});_tmp24D->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp24F=_region_malloc(r,sizeof(*_tmp24F));
_tmp24F[0]=({struct Cyc_Arg_Set_spec_struct _tmp250;_tmp250.tag=3;_tmp250.f1=& Cyc_save_c_r;
_tmp250;});_tmp24F;});_tmp24D->f5=({const char*_tmp24E="Don't delete temporary C files";
_tag_dynforward(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,31));});_tmp24D;});
_tmp162[38]=({struct _tuple15*_tmp247=_region_malloc(r,sizeof(*_tmp247));_tmp247->f1=({
const char*_tmp24C="-save-temps";_tag_dynforward(_tmp24C,sizeof(char),
_get_zero_arr_size(_tmp24C,12));});_tmp247->f2=0;_tmp247->f3=({const char*_tmp24B="";
_tag_dynforward(_tmp24B,sizeof(char),_get_zero_arr_size(_tmp24B,1));});_tmp247->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp249=_region_malloc(r,sizeof(*_tmp249));
_tmp249[0]=({struct Cyc_Arg_Unit_spec_struct _tmp24A;_tmp24A.tag=0;_tmp24A.f1=Cyc_set_save_temps;
_tmp24A;});_tmp249;});_tmp247->f5=({const char*_tmp248="Don't delete temporary files";
_tag_dynforward(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,29));});_tmp247;});
_tmp162[37]=({struct _tuple15*_tmp241=_region_malloc(r,sizeof(*_tmp241));_tmp241->f1=({
const char*_tmp246="-tovc";_tag_dynforward(_tmp246,sizeof(char),
_get_zero_arr_size(_tmp246,6));});_tmp241->f2=0;_tmp241->f3=({const char*_tmp245="";
_tag_dynforward(_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,1));});_tmp241->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp243=_region_malloc(r,sizeof(*_tmp243));
_tmp243[0]=({struct Cyc_Arg_Unit_spec_struct _tmp244;_tmp244.tag=0;_tmp244.f1=Cyc_set_tovc;
_tmp244;});_tmp243;});_tmp241->f5=({const char*_tmp242="Avoid gcc extensions in C output";
_tag_dynforward(_tmp242,sizeof(char),_get_zero_arr_size(_tmp242,33));});_tmp241;});
_tmp162[36]=({struct _tuple15*_tmp23B=_region_malloc(r,sizeof(*_tmp23B));_tmp23B->f1=({
const char*_tmp240="-elim-statement-expressions";_tag_dynforward(_tmp240,sizeof(
char),_get_zero_arr_size(_tmp240,28));});_tmp23B->f2=0;_tmp23B->f3=({const char*
_tmp23F="";_tag_dynforward(_tmp23F,sizeof(char),_get_zero_arr_size(_tmp23F,1));});
_tmp23B->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp23D=_region_malloc(r,
sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_Arg_Unit_spec_struct _tmp23E;_tmp23E.tag=
0;_tmp23E.f1=Cyc_set_elim_se;_tmp23E;});_tmp23D;});_tmp23B->f5=({const char*
_tmp23C="Avoid statement expressions in C output (implies --inline-checks)";
_tag_dynforward(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,66));});_tmp23B;});
_tmp162[35]=({struct _tuple15*_tmp235=_region_malloc(r,sizeof(*_tmp235));_tmp235->f1=({
const char*_tmp23A="-up";_tag_dynforward(_tmp23A,sizeof(char),_get_zero_arr_size(
_tmp23A,4));});_tmp235->f2=0;_tmp235->f3=({const char*_tmp239="";_tag_dynforward(
_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,1));});_tmp235->f4=(void*)({
struct Cyc_Arg_Clear_spec_struct*_tmp237=_region_malloc(r,sizeof(*_tmp237));
_tmp237[0]=({struct Cyc_Arg_Clear_spec_struct _tmp238;_tmp238.tag=4;_tmp238.f1=&
Cyc_pp_r;_tmp238;});_tmp237;});_tmp235->f5=({const char*_tmp236="Ugly print";
_tag_dynforward(_tmp236,sizeof(char),_get_zero_arr_size(_tmp236,11));});_tmp235;});
_tmp162[34]=({struct _tuple15*_tmp22F=_region_malloc(r,sizeof(*_tmp22F));_tmp22F->f1=({
const char*_tmp234="-pp";_tag_dynforward(_tmp234,sizeof(char),_get_zero_arr_size(
_tmp234,4));});_tmp22F->f2=0;_tmp22F->f3=({const char*_tmp233="";_tag_dynforward(
_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,1));});_tmp22F->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp231=_region_malloc(r,sizeof(*_tmp231));_tmp231[
0]=({struct Cyc_Arg_Set_spec_struct _tmp232;_tmp232.tag=3;_tmp232.f1=& Cyc_pp_r;
_tmp232;});_tmp231;});_tmp22F->f5=({const char*_tmp230="Pretty print";
_tag_dynforward(_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,13));});_tmp22F;});
_tmp162[33]=({struct _tuple15*_tmp229=_region_malloc(r,sizeof(*_tmp229));_tmp229->f1=({
const char*_tmp22E="-ic";_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size(
_tmp22E,4));});_tmp229->f2=0;_tmp229->f3=({const char*_tmp22D="";_tag_dynforward(
_tmp22D,sizeof(char),_get_zero_arr_size(_tmp22D,1));});_tmp229->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp22B=_region_malloc(r,sizeof(*_tmp22B));_tmp22B[
0]=({struct Cyc_Arg_Set_spec_struct _tmp22C;_tmp22C.tag=3;_tmp22C.f1=& Cyc_ic_r;
_tmp22C;});_tmp22B;});_tmp229->f5=({const char*_tmp22A="Activate the link-checker";
_tag_dynforward(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,26));});_tmp229;});
_tmp162[32]=({struct _tuple15*_tmp223=_region_malloc(r,sizeof(*_tmp223));_tmp223->f1=({
const char*_tmp228="-stopafter-toc";_tag_dynforward(_tmp228,sizeof(char),
_get_zero_arr_size(_tmp228,15));});_tmp223->f2=0;_tmp223->f3=({const char*_tmp227="";
_tag_dynforward(_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,1));});_tmp223->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp225=_region_malloc(r,sizeof(*_tmp225));
_tmp225[0]=({struct Cyc_Arg_Set_spec_struct _tmp226;_tmp226.tag=3;_tmp226.f1=& Cyc_toc_r;
_tmp226;});_tmp225;});_tmp223->f5=({const char*_tmp224="Stop after translation to C";
_tag_dynforward(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,28));});_tmp223;});
_tmp162[31]=({struct _tuple15*_tmp21D=_region_malloc(r,sizeof(*_tmp21D));_tmp21D->f1=({
const char*_tmp222="-stopafter-cf";_tag_dynforward(_tmp222,sizeof(char),
_get_zero_arr_size(_tmp222,14));});_tmp21D->f2=0;_tmp21D->f3=({const char*_tmp221="";
_tag_dynforward(_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,1));});_tmp21D->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp21F=_region_malloc(r,sizeof(*_tmp21F));
_tmp21F[0]=({struct Cyc_Arg_Set_spec_struct _tmp220;_tmp220.tag=3;_tmp220.f1=& Cyc_cf_r;
_tmp220;});_tmp21F;});_tmp21D->f5=({const char*_tmp21E="Stop after control-flow checking";
_tag_dynforward(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,33));});_tmp21D;});
_tmp162[30]=({struct _tuple15*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->f1=({
const char*_tmp21C="-noprint";_tag_dynforward(_tmp21C,sizeof(char),
_get_zero_arr_size(_tmp21C,9));});_tmp217->f2=0;_tmp217->f3=({const char*_tmp21B="";
_tag_dynforward(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,1));});_tmp217->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp219=_region_malloc(r,sizeof(*_tmp219));
_tmp219[0]=({struct Cyc_Arg_Set_spec_struct _tmp21A;_tmp21A.tag=3;_tmp21A.f1=& Cyc_noprint_r;
_tmp21A;});_tmp219;});_tmp217->f5=({const char*_tmp218="Do not print output (when stopping early)";
_tag_dynforward(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,42));});_tmp217;});
_tmp162[29]=({struct _tuple15*_tmp211=_region_malloc(r,sizeof(*_tmp211));_tmp211->f1=({
const char*_tmp216="-stopafter-tc";_tag_dynforward(_tmp216,sizeof(char),
_get_zero_arr_size(_tmp216,14));});_tmp211->f2=0;_tmp211->f3=({const char*_tmp215="";
_tag_dynforward(_tmp215,sizeof(char),_get_zero_arr_size(_tmp215,1));});_tmp211->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp213=_region_malloc(r,sizeof(*_tmp213));
_tmp213[0]=({struct Cyc_Arg_Set_spec_struct _tmp214;_tmp214.tag=3;_tmp214.f1=& Cyc_tc_r;
_tmp214;});_tmp213;});_tmp211->f5=({const char*_tmp212="Stop after type checking";
_tag_dynforward(_tmp212,sizeof(char),_get_zero_arr_size(_tmp212,25));});_tmp211;});
_tmp162[28]=({struct _tuple15*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));_tmp20B->f1=({
const char*_tmp210="-stopafter-parse";_tag_dynforward(_tmp210,sizeof(char),
_get_zero_arr_size(_tmp210,17));});_tmp20B->f2=0;_tmp20B->f3=({const char*_tmp20F="";
_tag_dynforward(_tmp20F,sizeof(char),_get_zero_arr_size(_tmp20F,1));});_tmp20B->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp20D=_region_malloc(r,sizeof(*_tmp20D));
_tmp20D[0]=({struct Cyc_Arg_Set_spec_struct _tmp20E;_tmp20E.tag=3;_tmp20E.f1=& Cyc_parseonly_r;
_tmp20E;});_tmp20D;});_tmp20B->f5=({const char*_tmp20C="Stop after parsing";
_tag_dynforward(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,19));});_tmp20B;});
_tmp162[27]=({struct _tuple15*_tmp205=_region_malloc(r,sizeof(*_tmp205));_tmp205->f1=({
const char*_tmp20A="-E";_tag_dynforward(_tmp20A,sizeof(char),_get_zero_arr_size(
_tmp20A,3));});_tmp205->f2=0;_tmp205->f3=({const char*_tmp209="";_tag_dynforward(
_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,1));});_tmp205->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp207=_region_malloc(r,sizeof(*_tmp207));_tmp207[
0]=({struct Cyc_Arg_Set_spec_struct _tmp208;_tmp208.tag=3;_tmp208.f1=& Cyc_stop_after_cpp_r;
_tmp208;});_tmp207;});_tmp205->f5=({const char*_tmp206="Stop after preprocessing";
_tag_dynforward(_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,25));});_tmp205;});
_tmp162[26]=({struct _tuple15*_tmp1FF=_region_malloc(r,sizeof(*_tmp1FF));_tmp1FF->f1=({
const char*_tmp204="-Wall";_tag_dynforward(_tmp204,sizeof(char),
_get_zero_arr_size(_tmp204,6));});_tmp1FF->f2=0;_tmp1FF->f3=({const char*_tmp203="";
_tag_dynforward(_tmp203,sizeof(char),_get_zero_arr_size(_tmp203,1));});_tmp1FF->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp201=_region_malloc(r,sizeof(*_tmp201));
_tmp201[0]=({struct Cyc_Arg_Unit_spec_struct _tmp202;_tmp202.tag=0;_tmp202.f1=Cyc_set_all_warnings;
_tmp202;});_tmp201;});_tmp1FF->f5=({const char*_tmp200="Turn on all warnings";
_tag_dynforward(_tmp200,sizeof(char),_get_zero_arr_size(_tmp200,21));});_tmp1FF;});
_tmp162[25]=({struct _tuple15*_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));_tmp1F9->f1=({
const char*_tmp1FE="-b";_tag_dynforward(_tmp1FE,sizeof(char),_get_zero_arr_size(
_tmp1FE,3));});_tmp1F9->f2=0;_tmp1F9->f3=({const char*_tmp1FD="<arch>";
_tag_dynforward(_tmp1FD,sizeof(char),_get_zero_arr_size(_tmp1FD,7));});_tmp1F9->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1FB=_region_malloc(r,sizeof(*
_tmp1FB));_tmp1FB[0]=({struct Cyc_Arg_String_spec_struct _tmp1FC;_tmp1FC.tag=5;
_tmp1FC.f1=Cyc_set_target_arch;_tmp1FC;});_tmp1FB;});_tmp1F9->f5=({const char*
_tmp1FA="Set target architecture; implies -stopafter-toc";_tag_dynforward(
_tmp1FA,sizeof(char),_get_zero_arr_size(_tmp1FA,48));});_tmp1F9;});_tmp162[24]=({
struct _tuple15*_tmp1F3=_region_malloc(r,sizeof(*_tmp1F3));_tmp1F3->f1=({const
char*_tmp1F8="-MT";_tag_dynforward(_tmp1F8,sizeof(char),_get_zero_arr_size(
_tmp1F8,4));});_tmp1F3->f2=0;_tmp1F3->f3=({const char*_tmp1F7=" <target>";
_tag_dynforward(_tmp1F7,sizeof(char),_get_zero_arr_size(_tmp1F7,10));});_tmp1F3->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1F5=_region_malloc(r,sizeof(*
_tmp1F5));_tmp1F5[0]=({struct Cyc_Arg_String_spec_struct _tmp1F6;_tmp1F6.tag=5;
_tmp1F6.f1=Cyc_set_dependencies_target;_tmp1F6;});_tmp1F5;});_tmp1F3->f5=({const
char*_tmp1F4="Give target for dependencies";_tag_dynforward(_tmp1F4,sizeof(char),
_get_zero_arr_size(_tmp1F4,29));});_tmp1F3;});_tmp162[23]=({struct _tuple15*
_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->f1=({const char*_tmp1F2="-MG";
_tag_dynforward(_tmp1F2,sizeof(char),_get_zero_arr_size(_tmp1F2,4));});_tmp1ED->f2=
0;_tmp1ED->f3=({const char*_tmp1F1="";_tag_dynforward(_tmp1F1,sizeof(char),
_get_zero_arr_size(_tmp1F1,1));});_tmp1ED->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1EF=_region_malloc(r,sizeof(*_tmp1EF));_tmp1EF[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1F0;_tmp1F0.tag=1;_tmp1F0.f1=Cyc_add_cpparg;_tmp1F0;});_tmp1EF;});_tmp1ED->f5=({
const char*_tmp1EE="When producing dependencies assume that missing files are generated";
_tag_dynforward(_tmp1EE,sizeof(char),_get_zero_arr_size(_tmp1EE,68));});_tmp1ED;});
_tmp162[22]=({struct _tuple15*_tmp1E7=_region_malloc(r,sizeof(*_tmp1E7));_tmp1E7->f1=({
const char*_tmp1EC="-M";_tag_dynforward(_tmp1EC,sizeof(char),_get_zero_arr_size(
_tmp1EC,3));});_tmp1E7->f2=0;_tmp1E7->f3=({const char*_tmp1EB="";_tag_dynforward(
_tmp1EB,sizeof(char),_get_zero_arr_size(_tmp1EB,1));});_tmp1E7->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1E9=_region_malloc(r,sizeof(*_tmp1E9));
_tmp1E9[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=Cyc_set_produce_dependencies;
_tmp1EA;});_tmp1E9;});_tmp1E7->f5=({const char*_tmp1E8="Produce dependencies";
_tag_dynforward(_tmp1E8,sizeof(char),_get_zero_arr_size(_tmp1E8,21));});_tmp1E7;});
_tmp162[21]=({struct _tuple15*_tmp1E1=_region_malloc(r,sizeof(*_tmp1E1));_tmp1E1->f1=({
const char*_tmp1E6="-S";_tag_dynforward(_tmp1E6,sizeof(char),_get_zero_arr_size(
_tmp1E6,3));});_tmp1E1->f2=0;_tmp1E1->f3=({const char*_tmp1E5="";_tag_dynforward(
_tmp1E5,sizeof(char),_get_zero_arr_size(_tmp1E5,1));});_tmp1E1->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1E3=_region_malloc(r,sizeof(*_tmp1E3));
_tmp1E3[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1E4;_tmp1E4.tag=0;_tmp1E4.f1=Cyc_set_stop_after_asmfile;
_tmp1E4;});_tmp1E3;});_tmp1E1->f5=({const char*_tmp1E2="Stop after producing assembly code";
_tag_dynforward(_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,35));});_tmp1E1;});
_tmp162[20]=({struct _tuple15*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->f1=({
const char*_tmp1E0="-pa";_tag_dynforward(_tmp1E0,sizeof(char),_get_zero_arr_size(
_tmp1E0,4));});_tmp1DB->f2=0;_tmp1DB->f3=({const char*_tmp1DF="";_tag_dynforward(
_tmp1DF,sizeof(char),_get_zero_arr_size(_tmp1DF,1));});_tmp1DB->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));
_tmp1DD[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1DE;_tmp1DE.tag=0;_tmp1DE.f1=Cyc_set_pa;
_tmp1DE;});_tmp1DD;});_tmp1DB->f5=({const char*_tmp1DC="Compile for profiling with aprof";
_tag_dynforward(_tmp1DC,sizeof(char),_get_zero_arr_size(_tmp1DC,33));});_tmp1DB;});
_tmp162[19]=({struct _tuple15*_tmp1D5=_region_malloc(r,sizeof(*_tmp1D5));_tmp1D5->f1=({
const char*_tmp1DA="-pg";_tag_dynforward(_tmp1DA,sizeof(char),_get_zero_arr_size(
_tmp1DA,4));});_tmp1D5->f2=0;_tmp1D5->f3=({const char*_tmp1D9="";_tag_dynforward(
_tmp1D9,sizeof(char),_get_zero_arr_size(_tmp1D9,1));});_tmp1D5->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1D7=_region_malloc(r,sizeof(*_tmp1D7));
_tmp1D7[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1D8;_tmp1D8.tag=0;_tmp1D8.f1=Cyc_set_pg;
_tmp1D8;});_tmp1D7;});_tmp1D5->f5=({const char*_tmp1D6="Compile for profiling with gprof";
_tag_dynforward(_tmp1D6,sizeof(char),_get_zero_arr_size(_tmp1D6,33));});_tmp1D5;});
_tmp162[18]=({struct _tuple15*_tmp1CF=_region_malloc(r,sizeof(*_tmp1CF));_tmp1CF->f1=({
const char*_tmp1D4="-p";_tag_dynforward(_tmp1D4,sizeof(char),_get_zero_arr_size(
_tmp1D4,3));});_tmp1CF->f2=0;_tmp1CF->f3=({const char*_tmp1D3="";_tag_dynforward(
_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,1));});_tmp1CF->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1D1=_region_malloc(r,sizeof(*_tmp1D1));
_tmp1D1[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1D2;_tmp1D2.tag=1;_tmp1D2.f1=Cyc_add_ccarg;
_tmp1D2;});_tmp1D1;});_tmp1CF->f5=({const char*_tmp1D0="Compile for profiling with prof";
_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,32));});_tmp1CF;});
_tmp162[17]=({struct _tuple15*_tmp1C9=_region_malloc(r,sizeof(*_tmp1C9));_tmp1C9->f1=({
const char*_tmp1CE="-g";_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size(
_tmp1CE,3));});_tmp1C9->f2=0;_tmp1C9->f3=({const char*_tmp1CD="";_tag_dynforward(
_tmp1CD,sizeof(char),_get_zero_arr_size(_tmp1CD,1));});_tmp1C9->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1CB=_region_malloc(r,sizeof(*_tmp1CB));
_tmp1CB[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1CC;_tmp1CC.tag=1;_tmp1CC.f1=Cyc_add_ccarg;
_tmp1CC;});_tmp1CB;});_tmp1C9->f5=({const char*_tmp1CA="Compile for debugging";
_tag_dynforward(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,22));});_tmp1C9;});
_tmp162[16]=({struct _tuple15*_tmp1C3=_region_malloc(r,sizeof(*_tmp1C3));_tmp1C3->f1=({
const char*_tmp1C8="-fomit-frame-pointer";_tag_dynforward(_tmp1C8,sizeof(char),
_get_zero_arr_size(_tmp1C8,21));});_tmp1C3->f2=0;_tmp1C3->f3=({const char*_tmp1C7="";
_tag_dynforward(_tmp1C7,sizeof(char),_get_zero_arr_size(_tmp1C7,1));});_tmp1C3->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1C5=_region_malloc(r,sizeof(*_tmp1C5));
_tmp1C5[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1C6;_tmp1C6.tag=1;_tmp1C6.f1=Cyc_add_ccarg;
_tmp1C6;});_tmp1C5;});_tmp1C3->f5=({const char*_tmp1C4="Omit frame pointer";
_tag_dynforward(_tmp1C4,sizeof(char),_get_zero_arr_size(_tmp1C4,19));});_tmp1C3;});
_tmp162[15]=({struct _tuple15*_tmp1BD=_region_malloc(r,sizeof(*_tmp1BD));_tmp1BD->f1=({
const char*_tmp1C2="-O3";_tag_dynforward(_tmp1C2,sizeof(char),_get_zero_arr_size(
_tmp1C2,4));});_tmp1BD->f2=0;_tmp1BD->f3=({const char*_tmp1C1="";_tag_dynforward(
_tmp1C1,sizeof(char),_get_zero_arr_size(_tmp1C1,1));});_tmp1BD->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1BF=_region_malloc(r,sizeof(*_tmp1BF));
_tmp1BF[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1C0;_tmp1C0.tag=1;_tmp1C0.f1=Cyc_add_ccarg;
_tmp1C0;});_tmp1BF;});_tmp1BD->f5=({const char*_tmp1BE="Even more optimization";
_tag_dynforward(_tmp1BE,sizeof(char),_get_zero_arr_size(_tmp1BE,23));});_tmp1BD;});
_tmp162[14]=({struct _tuple15*_tmp1B7=_region_malloc(r,sizeof(*_tmp1B7));_tmp1B7->f1=({
const char*_tmp1BC="-O2";_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size(
_tmp1BC,4));});_tmp1B7->f2=0;_tmp1B7->f3=({const char*_tmp1BB="";_tag_dynforward(
_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,1));});_tmp1B7->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1B9=_region_malloc(r,sizeof(*_tmp1B9));
_tmp1B9[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1BA;_tmp1BA.tag=1;_tmp1BA.f1=Cyc_add_ccarg;
_tmp1BA;});_tmp1B9;});_tmp1B7->f5=({const char*_tmp1B8="A higher level of optimization";
_tag_dynforward(_tmp1B8,sizeof(char),_get_zero_arr_size(_tmp1B8,31));});_tmp1B7;});
_tmp162[13]=({struct _tuple15*_tmp1B1=_region_malloc(r,sizeof(*_tmp1B1));_tmp1B1->f1=({
const char*_tmp1B6="-O";_tag_dynforward(_tmp1B6,sizeof(char),_get_zero_arr_size(
_tmp1B6,3));});_tmp1B1->f2=0;_tmp1B1->f3=({const char*_tmp1B5="";_tag_dynforward(
_tmp1B5,sizeof(char),_get_zero_arr_size(_tmp1B5,1));});_tmp1B1->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1B3=_region_malloc(r,sizeof(*_tmp1B3));
_tmp1B3[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1B4;_tmp1B4.tag=1;_tmp1B4.f1=Cyc_add_ccarg;
_tmp1B4;});_tmp1B3;});_tmp1B1->f5=({const char*_tmp1B2="Optimize";_tag_dynforward(
_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,9));});_tmp1B1;});_tmp162[12]=({
struct _tuple15*_tmp1AB=_region_malloc(r,sizeof(*_tmp1AB));_tmp1AB->f1=({const
char*_tmp1B0="-O0";_tag_dynforward(_tmp1B0,sizeof(char),_get_zero_arr_size(
_tmp1B0,4));});_tmp1AB->f2=0;_tmp1AB->f3=({const char*_tmp1AF="";_tag_dynforward(
_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,1));});_tmp1AB->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1AD=_region_malloc(r,sizeof(*_tmp1AD));
_tmp1AD[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1AE;_tmp1AE.tag=1;_tmp1AE.f1=Cyc_add_ccarg;
_tmp1AE;});_tmp1AD;});_tmp1AB->f5=({const char*_tmp1AC="Don't optimize";
_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,15));});_tmp1AB;});
_tmp162[11]=({struct _tuple15*_tmp1A5=_region_malloc(r,sizeof(*_tmp1A5));_tmp1A5->f1=({
const char*_tmp1AA="-s";_tag_dynforward(_tmp1AA,sizeof(char),_get_zero_arr_size(
_tmp1AA,3));});_tmp1A5->f2=0;_tmp1A5->f3=({const char*_tmp1A9="";_tag_dynforward(
_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,1));});_tmp1A5->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1A7=_region_malloc(r,sizeof(*_tmp1A7));
_tmp1A7[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1A8;_tmp1A8.tag=1;_tmp1A8.f1=Cyc_add_ccarg;
_tmp1A8;});_tmp1A7;});_tmp1A5->f5=({const char*_tmp1A6="Remove all symbol table and relocation info from executable";
_tag_dynforward(_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,60));});_tmp1A5;});
_tmp162[10]=({struct _tuple15*_tmp19F=_region_malloc(r,sizeof(*_tmp19F));_tmp19F->f1=({
const char*_tmp1A4="-x";_tag_dynforward(_tmp1A4,sizeof(char),_get_zero_arr_size(
_tmp1A4,3));});_tmp19F->f2=0;_tmp19F->f3=({const char*_tmp1A3=" <language>";
_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,12));});_tmp19F->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1A1=_region_malloc(r,sizeof(*
_tmp1A1));_tmp1A1[0]=({struct Cyc_Arg_String_spec_struct _tmp1A2;_tmp1A2.tag=5;
_tmp1A2.f1=Cyc_set_inputtype;_tmp1A2;});_tmp1A1;});_tmp19F->f5=({const char*
_tmp1A0="Specify <language> for the following input files";_tag_dynforward(
_tmp1A0,sizeof(char),_get_zero_arr_size(_tmp1A0,49));});_tmp19F;});_tmp162[9]=({
struct _tuple15*_tmp199=_region_malloc(r,sizeof(*_tmp199));_tmp199->f1=({const
char*_tmp19E="-c";_tag_dynforward(_tmp19E,sizeof(char),_get_zero_arr_size(
_tmp19E,3));});_tmp199->f2=0;_tmp199->f3=({const char*_tmp19D="";_tag_dynforward(
_tmp19D,sizeof(char),_get_zero_arr_size(_tmp19D,1));});_tmp199->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp19B=_region_malloc(r,sizeof(*_tmp19B));
_tmp19B[0]=({struct Cyc_Arg_Unit_spec_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=Cyc_set_stop_after_objectfile;
_tmp19C;});_tmp19B;});_tmp199->f5=({const char*_tmp19A="Produce an object file instead of an executable; do not link";
_tag_dynforward(_tmp19A,sizeof(char),_get_zero_arr_size(_tmp19A,61));});_tmp199;});
_tmp162[8]=({struct _tuple15*_tmp193=_region_malloc(r,sizeof(*_tmp193));_tmp193->f1=({
const char*_tmp198="-m";_tag_dynforward(_tmp198,sizeof(char),_get_zero_arr_size(
_tmp198,3));});_tmp193->f2=1;_tmp193->f3=({const char*_tmp197="<option>";
_tag_dynforward(_tmp197,sizeof(char),_get_zero_arr_size(_tmp197,9));});_tmp193->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp195=_region_malloc(r,sizeof(*_tmp195));
_tmp195[0]=({struct Cyc_Arg_Flag_spec_struct _tmp196;_tmp196.tag=1;_tmp196.f1=Cyc_add_marg;
_tmp196;});_tmp195;});_tmp193->f5=({const char*_tmp194="GCC specific: pass machine-dependent flag on to gcc";
_tag_dynforward(_tmp194,sizeof(char),_get_zero_arr_size(_tmp194,52));});_tmp193;});
_tmp162[7]=({struct _tuple15*_tmp18D=_region_malloc(r,sizeof(*_tmp18D));_tmp18D->f1=({
const char*_tmp192="-l";_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size(
_tmp192,3));});_tmp18D->f2=1;_tmp18D->f3=({const char*_tmp191="<libname>";
_tag_dynforward(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,10));});_tmp18D->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp18F=_region_malloc(r,sizeof(*_tmp18F));
_tmp18F[0]=({struct Cyc_Arg_Flag_spec_struct _tmp190;_tmp190.tag=1;_tmp190.f1=Cyc_add_libarg;
_tmp190;});_tmp18F;});_tmp18D->f5=({const char*_tmp18E="Library file";
_tag_dynforward(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,13));});_tmp18D;});
_tmp162[6]=({struct _tuple15*_tmp187=_region_malloc(r,sizeof(*_tmp187));_tmp187->f1=({
const char*_tmp18C="-L";_tag_dynforward(_tmp18C,sizeof(char),_get_zero_arr_size(
_tmp18C,3));});_tmp187->f2=1;_tmp187->f3=({const char*_tmp18B="<dir>";
_tag_dynforward(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,6));});_tmp187->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp189=_region_malloc(r,sizeof(*_tmp189));
_tmp189[0]=({struct Cyc_Arg_Flag_spec_struct _tmp18A;_tmp18A.tag=1;_tmp18A.f1=Cyc_add_ccarg;
_tmp18A;});_tmp189;});_tmp187->f5=({const char*_tmp188="Add to the list of directories for -l";
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,38));});_tmp187;});
_tmp162[5]=({struct _tuple15*_tmp181=_region_malloc(r,sizeof(*_tmp181));_tmp181->f1=({
const char*_tmp186="-I";_tag_dynforward(_tmp186,sizeof(char),_get_zero_arr_size(
_tmp186,3));});_tmp181->f2=1;_tmp181->f3=({const char*_tmp185="<dir>";
_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,6));});_tmp181->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp183=_region_malloc(r,sizeof(*_tmp183));
_tmp183[0]=({struct Cyc_Arg_Flag_spec_struct _tmp184;_tmp184.tag=1;_tmp184.f1=Cyc_add_cpparg;
_tmp184;});_tmp183;});_tmp181->f5=({const char*_tmp182="Add to the list of directories to search for include files";
_tag_dynforward(_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,59));});_tmp181;});
_tmp162[4]=({struct _tuple15*_tmp17B=_region_malloc(r,sizeof(*_tmp17B));_tmp17B->f1=({
const char*_tmp180="-B";_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size(
_tmp180,3));});_tmp17B->f2=1;_tmp17B->f3=({const char*_tmp17F="<file>";
_tag_dynforward(_tmp17F,sizeof(char),_get_zero_arr_size(_tmp17F,7));});_tmp17B->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp17D=_region_malloc(r,sizeof(*_tmp17D));
_tmp17D[0]=({struct Cyc_Arg_Flag_spec_struct _tmp17E;_tmp17E.tag=1;_tmp17E.f1=Cyc_add_cyclone_exec_path;
_tmp17E;});_tmp17D;});_tmp17B->f5=({const char*_tmp17C="Add to the list of directories to search for compiler files";
_tag_dynforward(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,60));});_tmp17B;});
_tmp162[3]=({struct _tuple15*_tmp175=_region_malloc(r,sizeof(*_tmp175));_tmp175->f1=({
const char*_tmp17A="-D";_tag_dynforward(_tmp17A,sizeof(char),_get_zero_arr_size(
_tmp17A,3));});_tmp175->f2=1;_tmp175->f3=({const char*_tmp179="<name>[=<value>]";
_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,17));});_tmp175->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp177=_region_malloc(r,sizeof(*_tmp177));
_tmp177[0]=({struct Cyc_Arg_Flag_spec_struct _tmp178;_tmp178.tag=1;_tmp178.f1=Cyc_add_cpparg;
_tmp178;});_tmp177;});_tmp175->f5=({const char*_tmp176="Pass definition to preprocessor";
_tag_dynforward(_tmp176,sizeof(char),_get_zero_arr_size(_tmp176,32));});_tmp175;});
_tmp162[2]=({struct _tuple15*_tmp16F=_region_malloc(r,sizeof(*_tmp16F));_tmp16F->f1=({
const char*_tmp174="-o";_tag_dynforward(_tmp174,sizeof(char),_get_zero_arr_size(
_tmp174,3));});_tmp16F->f2=0;_tmp16F->f3=({const char*_tmp173=" <file>";
_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,8));});_tmp16F->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp171=_region_malloc(r,sizeof(*
_tmp171));_tmp171[0]=({struct Cyc_Arg_String_spec_struct _tmp172;_tmp172.tag=5;
_tmp172.f1=Cyc_set_output_file;_tmp172;});_tmp171;});_tmp16F->f5=({const char*
_tmp170="Set the output file name to <file>";_tag_dynforward(_tmp170,sizeof(char),
_get_zero_arr_size(_tmp170,35));});_tmp16F;});_tmp162[1]=({struct _tuple15*
_tmp169=_region_malloc(r,sizeof(*_tmp169));_tmp169->f1=({const char*_tmp16E="--version";
_tag_dynforward(_tmp16E,sizeof(char),_get_zero_arr_size(_tmp16E,10));});_tmp169->f2=
0;_tmp169->f3=({const char*_tmp16D="";_tag_dynforward(_tmp16D,sizeof(char),
_get_zero_arr_size(_tmp16D,1));});_tmp169->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp16B=_region_malloc(r,sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp16C;_tmp16C.tag=0;_tmp16C.f1=Cyc_print_version;_tmp16C;});_tmp16B;});_tmp169->f5=({
const char*_tmp16A="Print version information and exit";_tag_dynforward(_tmp16A,
sizeof(char),_get_zero_arr_size(_tmp16A,35));});_tmp169;});_tmp162[0]=({struct
_tuple15*_tmp163=_region_malloc(r,sizeof(*_tmp163));_tmp163->f1=({const char*
_tmp168="-v";_tag_dynforward(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,3));});
_tmp163->f2=0;_tmp163->f3=({const char*_tmp167="";_tag_dynforward(_tmp167,sizeof(
char),_get_zero_arr_size(_tmp167,1));});_tmp163->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp165=_region_malloc(r,sizeof(*_tmp165));_tmp165[0]=({struct Cyc_Arg_Set_spec_struct
_tmp166;_tmp166.tag=3;_tmp166.f1=& Cyc_v_r;_tmp166;});_tmp165;});_tmp163->f5=({
const char*_tmp164="Print compilation stages verbosely";_tag_dynforward(_tmp164,
sizeof(char),_get_zero_arr_size(_tmp164,35));});_tmp163;});((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(r,_tag_dynforward(
_tmp162,sizeof(struct _tuple15*),62));});struct _dynforward_ptr optstring=({const
char*_tmp161="Options:";_tag_dynforward(_tmp161,sizeof(char),_get_zero_arr_size(
_tmp161,9));});Cyc_Arg_current=0;Cyc_Arg_parse(options,Cyc_add_other,optstring,
_tmp146);if(Cyc_target_arch.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)
Cyc_target_arch=(struct _dynforward_ptr)Cstring_to_string(Carch);{struct
_dynforward_ptr cyclone_exec_prefix=({char*_tmp160=getenv((const char*)"CYCLONE_EXEC_PREFIX");
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,1));});if(
cyclone_exec_prefix.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*_tmp148));
_tmp148->hd=({struct _dynforward_ptr*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149[
0]=cyclone_exec_prefix;_tmp149;});_tmp148->tl=Cyc_cyclone_exec_path;_tmp148;});
def_lib_path=(struct _dynforward_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((
struct _dynforward_ptr)def_lib_path)> 0)Cyc_cyclone_exec_path=({struct Cyc_List_List*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->hd=({struct _dynforward_ptr*_tmp14B=
_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=(struct _dynforward_ptr)Cyc_Filename_concat(
def_lib_path,({const char*_tmp14C="cyc-lib";_tag_dynforward(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,8));}));_tmp14B;});_tmp14A->tl=Cyc_cyclone_exec_path;
_tmp14A;});Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);{struct _dynforward_ptr _tmp14D=Cyc_do_find(cyclone_arch_path,({
const char*_tmp15F="cycspecs";_tag_dynforward(_tmp15F,sizeof(char),
_get_zero_arr_size(_tmp15F,9));}));if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmp150;_tmp150.tag=0;_tmp150.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp14D);{void*_tmp14E[1]={& _tmp150};Cyc_fprintf(Cyc_stderr,({const char*_tmp14F="Reading from specs file %s\n";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,28));}),
_tag_dynforward(_tmp14E,sizeof(void*),1));}});{struct Cyc_List_List*_tmp151=Cyc_read_specs(
_tmp14D);struct _dynforward_ptr _tmp152=_tag_dynforward(0,0,0);{struct
_handler_cons _tmp153;_push_handler(& _tmp153);{int _tmp155=0;if(setjmp(_tmp153.handler))
_tmp155=1;if(!_tmp155){{struct _dynforward_ptr _tmp156=*((struct _dynforward_ptr*(*)(
int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l,
struct _dynforward_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp151,
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"cyclone",sizeof(
char),8));_tmp152=Cyc_split_specs(_tmp156);};_pop_handler();}else{void*_tmp154=(
void*)_exn_thrown;void*_tmp159=_tmp154;_LL36: if(_tmp159 != Cyc_Core_Not_found)
goto _LL38;_LL37: goto _LL35;_LL38:;_LL39:(void)_throw(_tmp159);_LL35:;}}}if(
_tmp152.curr != (_tag_dynforward(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp152);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp147);if(Cyc_strcmp((struct _dynforward_ptr)
Cyc_cpp,({const char*_tmp15A="";_tag_dynforward(_tmp15A,sizeof(char),
_get_zero_arr_size(_tmp15A,1));}))== 0)Cyc_set_cpp((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp15E;_tmp15E.tag=0;_tmp15E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp14D);{struct Cyc_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
comp);{void*_tmp15B[2]={& _tmp15D,& _tmp15E};Cyc_aprintf(({const char*_tmp15C="%s -x c -E -specs %s";
_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,21));}),
_tag_dynforward(_tmp15B,sizeof(void*),2));}}}));}}}}}};_pop_region(r);}if(Cyc_cyclone_files
== 0  && Cyc_ccargs == 0){({void*_tmp2E3[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2E4="missing file\n";_tag_dynforward(_tmp2E4,sizeof(char),_get_zero_arr_size(
_tmp2E4,14));}),_tag_dynforward(_tmp2E3,sizeof(void*),0));});exit(1);}if(!Cyc_stop_after_cpp_r
 && Cyc_strlen((struct _dynforward_ptr)Cyc_cyc_include)== 0)Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,({const char*_tmp2E5="cyc_include.h";_tag_dynforward(
_tmp2E5,sizeof(char),_get_zero_arr_size(_tmp2E5,14));}));{struct Cyc_List_List*
_tmp2E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);
for(0;_tmp2E6 != 0;_tmp2E6=_tmp2E6->tl){Cyc_process_file(*((struct _dynforward_ptr*)
_tmp2E6->hd));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp2E7="-L";_tag_dynforward(
_tmp2E7,sizeof(char),_get_zero_arr_size(_tmp2E7,3));}),(struct _dynforward_ptr)
def_lib_path));Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_ccargs);{struct _dynforward_ptr _tmp2E8=Cyc_str_sepstr(((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),({const char*_tmp340=" ";_tag_dynforward(
_tmp340,sizeof(char),_get_zero_arr_size(_tmp340,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _dynforward_ptr _tmp2E9=
Cyc_str_sepstr(({struct Cyc_List_List*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->hd=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp33D->tl=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct
_dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs);_tmp33D;}),({const char*_tmp33F=" ";_tag_dynforward(_tmp33F,sizeof(
char),_get_zero_arr_size(_tmp33F,2));}));struct Cyc_List_List*stdlib;struct
_dynforward_ptr stdlib_string;int _tmp2EA=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dynforward_ptr*)
_check_null(Cyc_output_file)),({const char*_tmp33B=".a";_tag_dynforward(_tmp33B,
sizeof(char),_get_zero_arr_size(_tmp33B,3));}))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp33C=".lib";
_tag_dynforward(_tmp33C,sizeof(char),_get_zero_arr_size(_tmp33C,5));}));if(
_tmp2EA){stdlib=0;stdlib_string=(struct _dynforward_ptr)({const char*_tmp2EB="";
_tag_dynforward(_tmp2EB,sizeof(char),_get_zero_arr_size(_tmp2EB,1));});}else{
struct _dynforward_ptr libcyc_flag;struct _dynforward_ptr nogc_filename;struct
_dynforward_ptr runtime_filename;struct _dynforward_ptr gc_filename;if(Cyc_pa_r){
libcyc_flag=({const char*_tmp2EC="-lcyc_a";_tag_dynforward(_tmp2EC,sizeof(char),
_get_zero_arr_size(_tmp2EC,8));});nogc_filename=({const char*_tmp2ED="nogc_a.a";
_tag_dynforward(_tmp2ED,sizeof(char),_get_zero_arr_size(_tmp2ED,9));});
runtime_filename=({const char*_tmp2EE="runtime_cyc_a.o";_tag_dynforward(_tmp2EE,
sizeof(char),_get_zero_arr_size(_tmp2EE,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp2EF="-lcyc_nocheck";_tag_dynforward(_tmp2EF,sizeof(
char),_get_zero_arr_size(_tmp2EF,14));});nogc_filename=({const char*_tmp2F0="nogc_nocheck.a";
_tag_dynforward(_tmp2F0,sizeof(char),_get_zero_arr_size(_tmp2F0,15));});
runtime_filename=({const char*_tmp2F1="runtime_cyc.o";_tag_dynforward(_tmp2F1,
sizeof(char),_get_zero_arr_size(_tmp2F1,14));});}else{if(Cyc_pg_r){libcyc_flag=({
const char*_tmp2F2="-lcyc_pg";_tag_dynforward(_tmp2F2,sizeof(char),
_get_zero_arr_size(_tmp2F2,9));});runtime_filename=({const char*_tmp2F3="runtime_cyc_pg.o";
_tag_dynforward(_tmp2F3,sizeof(char),_get_zero_arr_size(_tmp2F3,17));});
nogc_filename=({const char*_tmp2F4="nogc_pg.a";_tag_dynforward(_tmp2F4,sizeof(
char),_get_zero_arr_size(_tmp2F4,10));});}else{libcyc_flag=({const char*_tmp2F5="-lcyc";
_tag_dynforward(_tmp2F5,sizeof(char),_get_zero_arr_size(_tmp2F5,6));});
nogc_filename=({const char*_tmp2F6="nogc.a";_tag_dynforward(_tmp2F6,sizeof(char),
_get_zero_arr_size(_tmp2F6,7));});runtime_filename=({const char*_tmp2F7="runtime_cyc.o";
_tag_dynforward(_tmp2F7,sizeof(char),_get_zero_arr_size(_tmp2F7,14));});}}}
gc_filename=({const char*_tmp2F8="gc.a";_tag_dynforward(_tmp2F8,sizeof(char),
_get_zero_arr_size(_tmp2F8,5));});{struct _dynforward_ptr _tmp2F9=Cyc_nogc_r?Cyc_do_find(
cyclone_arch_path,nogc_filename): Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dynforward_ptr _tmp2FA=Cyc_do_find(cyclone_arch_path,runtime_filename);
stdlib=0;stdlib_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp2FF;_tmp2FF.tag=0;_tmp2FF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2F9);{struct Cyc_String_pa_struct _tmp2FE;_tmp2FE.tag=0;_tmp2FE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)libcyc_flag);{struct Cyc_String_pa_struct
_tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2FA);{void*_tmp2FB[3]={& _tmp2FD,& _tmp2FE,& _tmp2FF};Cyc_aprintf(({const char*
_tmp2FC=" %s %s %s";_tag_dynforward(_tmp2FC,sizeof(char),_get_zero_arr_size(
_tmp2FC,10));}),_tag_dynforward(_tmp2FB,sizeof(void*),3));}}}});}}if(Cyc_ic_r){
struct _handler_cons _tmp300;_push_handler(& _tmp300);{int _tmp302=0;if(setjmp(
_tmp300.handler))_tmp302=1;if(!_tmp302){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*
f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dynforward_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp303=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp2EA)_tmp303=({
struct Cyc_List_List*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->hd=Cyc_final_strptr;
_tmp304->tl=_tmp303;_tmp304;});{struct Cyc_Interface_I*_tmp305=((struct Cyc_Interface_I*(*)(
struct Cyc_Interface_I*(*get)(struct _dynforward_ptr*),struct Cyc_List_List*la,
struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,
_tmp303,_tmp303);if(_tmp305 == 0){({void*_tmp306[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp307="Error: interfaces incompatible\n";_tag_dynforward(_tmp307,
sizeof(char),_get_zero_arr_size(_tmp307,32));}),_tag_dynforward(_tmp306,sizeof(
void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp308=1;
_npop_handler(0);return _tmp308;}}if(_tmp2EA){if(Cyc_output_file != 0){struct
_dynforward_ptr _tmp309=({struct Cyc_String_pa_struct _tmp310;_tmp310.tag=0;_tmp310.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Filename_chop_extension(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp30E[1]={&
_tmp310};Cyc_aprintf(({const char*_tmp30F="%s.cycio";_tag_dynforward(_tmp30F,
sizeof(char),_get_zero_arr_size(_tmp30F,9));}),_tag_dynforward(_tmp30E,sizeof(
void*),1));}});struct Cyc___cycFILE*_tmp30A=Cyc_try_file_open((struct
_dynforward_ptr)_tmp309,({const char*_tmp30C="wb";_tag_dynforward(_tmp30C,sizeof(
char),_get_zero_arr_size(_tmp30C,3));}),({const char*_tmp30D="interface object file";
_tag_dynforward(_tmp30D,sizeof(char),_get_zero_arr_size(_tmp30D,22));}));if(
_tmp30A == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp30B=1;
_npop_handler(0);return _tmp30B;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_tmp305,(struct Cyc___cycFILE*)_tmp30A);Cyc_file_close((struct Cyc___cycFILE*)
_tmp30A);}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp305,({struct _tuple11*_tmp311=_cycalloc(sizeof(*_tmp311));
_tmp311->f1=({const char*_tmp313="empty interface";_tag_dynforward(_tmp313,
sizeof(char),_get_zero_arr_size(_tmp313,16));});_tmp311->f2=({const char*_tmp312="global interface";
_tag_dynforward(_tmp312,sizeof(char),_get_zero_arr_size(_tmp312,17));});_tmp311;}))){({
void*_tmp314[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp315="Error: some objects are still undefined\n";
_tag_dynforward(_tmp315,sizeof(char),_get_zero_arr_size(_tmp315,41));}),
_tag_dynforward(_tmp314,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp316=1;_npop_handler(0);return _tmp316;}}}}};_pop_handler();}else{void*
_tmp301=(void*)_exn_thrown;void*_tmp318=_tmp301;_LL3B:;_LL3C:{void*_tmp319=
_tmp318;struct _dynforward_ptr _tmp31A;struct _dynforward_ptr _tmp31B;struct
_dynforward_ptr _tmp31C;_LL40: if(*((void**)_tmp319)!= Cyc_Core_Failure)goto _LL42;
_tmp31A=((struct Cyc_Core_Failure_struct*)_tmp319)->f1;_LL41:({struct Cyc_String_pa_struct
_tmp31F;_tmp31F.tag=0;_tmp31F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp31A);{void*_tmp31D[1]={& _tmp31F};Cyc_fprintf(Cyc_stderr,({const char*_tmp31E="Exception Core::Failure %s\n";
_tag_dynforward(_tmp31E,sizeof(char),_get_zero_arr_size(_tmp31E,28));}),
_tag_dynforward(_tmp31D,sizeof(void*),1));}});goto _LL3F;_LL42: if(*((void**)
_tmp319)!= Cyc_Core_Impossible)goto _LL44;_tmp31B=((struct Cyc_Core_Impossible_struct*)
_tmp319)->f1;_LL43:({struct Cyc_String_pa_struct _tmp322;_tmp322.tag=0;_tmp322.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp31B);{void*_tmp320[1]={&
_tmp322};Cyc_fprintf(Cyc_stderr,({const char*_tmp321="Exception Core::Impossible %s\n";
_tag_dynforward(_tmp321,sizeof(char),_get_zero_arr_size(_tmp321,31));}),
_tag_dynforward(_tmp320,sizeof(void*),1));}});goto _LL3F;_LL44: if(_tmp319 != Cyc_Dict_Absent)
goto _LL46;_LL45:({void*_tmp323[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp324="Exception Dict::Absent\n";
_tag_dynforward(_tmp324,sizeof(char),_get_zero_arr_size(_tmp324,24));}),
_tag_dynforward(_tmp323,sizeof(void*),0));});goto _LL3F;_LL46: if(*((void**)
_tmp319)!= Cyc_Core_Invalid_argument)goto _LL48;_tmp31C=((struct Cyc_Core_Invalid_argument_struct*)
_tmp319)->f1;_LL47:({struct Cyc_String_pa_struct _tmp327;_tmp327.tag=0;_tmp327.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp31C);{void*_tmp325[1]={&
_tmp327};Cyc_fprintf(Cyc_stderr,({const char*_tmp326="Exception Core::Invalid_argument %s\n";
_tag_dynforward(_tmp326,sizeof(char),_get_zero_arr_size(_tmp326,37));}),
_tag_dynforward(_tmp325,sizeof(void*),1));}});goto _LL3F;_LL48:;_LL49:({void*
_tmp328[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp329="Uncaught exception\n";
_tag_dynforward(_tmp329,sizeof(char),_get_zero_arr_size(_tmp329,20));}),
_tag_dynforward(_tmp328,sizeof(void*),0));});goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL3D:;_LL3E:(void)_throw(_tmp318);_LL3A:;}}}{
struct _dynforward_ptr outfile_str=({const char*_tmp33A="";_tag_dynforward(_tmp33A,
sizeof(char),_get_zero_arr_size(_tmp33A,1));});if(Cyc_output_file != 0)
outfile_str=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp32C;_tmp32C.tag=
0;_tmp32C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp32A[1]={&
_tmp32C};Cyc_aprintf(({const char*_tmp32B=" -o %s";_tag_dynforward(_tmp32B,
sizeof(char),_get_zero_arr_size(_tmp32B,7));}),_tag_dynforward(_tmp32A,sizeof(
void*),1));}});{struct _dynforward_ptr _tmp32D=({struct Cyc_String_pa_struct _tmp339;
_tmp339.tag=0;_tmp339.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp2E9);{
struct Cyc_String_pa_struct _tmp338;_tmp338.tag=0;_tmp338.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdlib_string);{struct Cyc_String_pa_struct
_tmp337;_tmp337.tag=0;_tmp337.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2E8);{struct Cyc_String_pa_struct _tmp336;_tmp336.tag=0;_tmp336.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)outfile_str);{struct Cyc_String_pa_struct
_tmp335;_tmp335.tag=0;_tmp335.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
comp);{void*_tmp333[5]={& _tmp335,& _tmp336,& _tmp337,& _tmp338,& _tmp339};Cyc_aprintf(({
const char*_tmp334="%s %s %s%s%s";_tag_dynforward(_tmp334,sizeof(char),
_get_zero_arr_size(_tmp334,13));}),_tag_dynforward(_tmp333,sizeof(void*),5));}}}}}});
if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp330;_tmp330.tag=0;_tmp330.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp32D);{void*_tmp32E[1]={& _tmp330};Cyc_fprintf(
Cyc_stderr,({const char*_tmp32F="%s\n";_tag_dynforward(_tmp32F,sizeof(char),
_get_zero_arr_size(_tmp32F,4));}),_tag_dynforward(_tmp32E,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)
_untag_dynforward_ptr(_tmp32D,sizeof(char),1))!= 0){({void*_tmp331[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp332="Error: C compiler failed\n";_tag_dynforward(
_tmp332,sizeof(char),_get_zero_arr_size(_tmp332,26));}),_tag_dynforward(_tmp331,
sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();
return Cyc_compile_failure?1: 0;}}}}}}
