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
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);

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
  unsigned int _dfpp_elts = (((unsigned)_ans->last_plus_one) - 
                             ((unsigned)_ans->curr)) / elt_sz;
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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(
struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{
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
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
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
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
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
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;
struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _dynforward_ptr*name;int*identity;void*
kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{
int tag;void*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;
struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*
tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
Cyc_Absyn_Aggrdecl**f1;};struct Cyc_Absyn_AggrInfo{void*aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_TagType_struct{
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
struct Cyc_Absyn_String_c_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dynforward_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
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
f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
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
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
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
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict*ae;struct Cyc_List_List*le;};void*Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_List_List*);void Cyc_Tcenv_check_rgn_accessible(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void*Cyc_Tcutil_impos(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,
void*k2);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern
struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern
struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;void*_tmp6;void*_tmp7;struct _tuple1*
_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;struct Cyc_List_List*
_tmpB;int _tmpC;struct Cyc_Absyn_Exp*_tmpD;_LL1: if(_tmp0 <= (void*)2)goto _LL9;if(*((
int*)_tmp0)!= 12)goto _LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;
_LL2:{struct _handler_cons _tmpE;_push_handler(& _tmpE);{int _tmp10=0;if(setjmp(
_tmpE.handler))_tmp10=1;if(!_tmp10){{void*_tmp11=Cyc_Tcenv_lookup_ordinary(te,p->loc,
_tmp1);struct Cyc_Absyn_Tuniondecl*_tmp12;struct Cyc_Absyn_Tunionfield*_tmp13;
struct Cyc_Absyn_Enumdecl*_tmp14;struct Cyc_Absyn_Enumfield*_tmp15;void*_tmp16;
struct Cyc_Absyn_Enumfield*_tmp17;_LLC: if(*((int*)_tmp11)!= 1)goto _LLE;_LLD:(void*)(
p->r=(void*)((void*)0));({void*_tmp18[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp19="struct tag used without arguments in pattern";_tag_dynforward(_tmp19,
sizeof(char),_get_zero_arr_size(_tmp19,45));}),_tag_dynforward(_tmp18,sizeof(
void*),0));});_npop_handler(0);return;_LLE: if(*((int*)_tmp11)!= 2)goto _LL10;
_tmp12=((struct Cyc_Tcenv_TunionRes_struct*)_tmp11)->f1;_tmp13=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp11)->f2;_LLF:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp1B;_tmp1B.tag=6;_tmp1B.f1=_tmp12;_tmp1B.f2=_tmp13;_tmp1B.f3=0;_tmp1B.f4=0;
_tmp1B;});_tmp1A;})));_npop_handler(0);return;_LL10: if(*((int*)_tmp11)!= 3)goto
_LL12;_tmp14=((struct Cyc_Tcenv_EnumRes_struct*)_tmp11)->f1;_tmp15=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp11)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1D;_tmp1D.tag=10;_tmp1D.f1=_tmp14;_tmp1D.f2=_tmp15;_tmp1D;});_tmp1C;})));
_npop_handler(0);return;_LL12: if(*((int*)_tmp11)!= 4)goto _LL14;_tmp16=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp11)->f1;_tmp17=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp11)->f2;_LL13:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp1F;_tmp1F.tag=11;_tmp1F.f1=(void*)_tmp16;_tmp1F.f2=_tmp17;_tmp1F;});_tmp1E;})));
_npop_handler(0);return;_LL14: if(*((int*)_tmp11)!= 0)goto _LLB;_LL15: goto _LLB;
_LLB:;};_pop_handler();}else{void*_tmpF=(void*)_exn_thrown;void*_tmp21=_tmpF;
_LL17: if(_tmp21 != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;_LL19:;_LL1A:(void)
_throw(_tmp21);_LL16:;}}}{void*_tmp22=(*_tmp1).f1;struct Cyc_List_List*_tmp23;
_LL1C: if((int)_tmp22 != 0)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(_tmp22 <= (void*)1)
goto _LL20;if(*((int*)_tmp22)!= 0)goto _LL20;_tmp23=((struct Cyc_Absyn_Rel_n_struct*)
_tmp22)->f1;if(_tmp23 != 0)goto _LL20;_LL1F:(*_tmp1).f1=(void*)0;(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp24=_cycalloc(sizeof(*_tmp24));
_tmp24[0]=({struct Cyc_Absyn_Var_p_struct _tmp25;_tmp25.tag=0;_tmp25.f1=Cyc_Absyn_new_vardecl(
_tmp1,(void*)0,0);_tmp25.f2=Cyc_Absyn_new_pat((void*)0,0);_tmp25;});_tmp24;})));
return;_LL20:;_LL21:({void*_tmp26[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmp27="qualified variable in pattern";_tag_dynforward(_tmp27,sizeof(char),
_get_zero_arr_size(_tmp27,30));}),_tag_dynforward(_tmp26,sizeof(void*),0));});
return;_LL1B:;}_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct _handler_cons _tmp28;
_push_handler(& _tmp28);{int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!
_tmp2A){{void*_tmp2B=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*
_tmp2C;struct Cyc_Absyn_Tuniondecl*_tmp2D;struct Cyc_Absyn_Tunionfield*_tmp2E;
_LL23: if(*((int*)_tmp2B)!= 1)goto _LL25;_tmp2C=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp2B)->f1;_LL24: if((void*)_tmp2C->kind == (void*)1){(void*)(p->r=(void*)((void*)
0));({void*_tmp2F[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp30="cannot pattern-match a union";
_tag_dynforward(_tmp30,sizeof(char),_get_zero_arr_size(_tmp30,29));}),
_tag_dynforward(_tmp2F,sizeof(void*),0));});_npop_handler(0);return;}{struct Cyc_List_List*
_tmp31=0;for(0;_tmp3 != 0;_tmp3=_tmp3->tl){_tmp31=({struct Cyc_List_List*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32->hd=({struct _tuple8*_tmp33=_cycalloc(sizeof(*
_tmp33));_tmp33->f1=0;_tmp33->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd;_tmp33;});
_tmp32->tl=_tmp31;_tmp32;});}(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp35;_tmp35.tag=5;_tmp35.f1=({struct Cyc_Absyn_AggrInfo _tmp36;_tmp36.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp37=_cycalloc(sizeof(*_tmp37));
_tmp37[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp38;_tmp38.tag=1;_tmp38.f1=({
struct Cyc_Absyn_Aggrdecl**_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=_tmp2C;
_tmp39;});_tmp38;});_tmp37;}));_tmp36.targs=0;_tmp36;});_tmp35.f2=0;_tmp35.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp31);_tmp35.f4=
_tmp4;_tmp35;});_tmp34;})));_npop_handler(0);return;}_LL25: if(*((int*)_tmp2B)!= 
2)goto _LL27;_tmp2D=((struct Cyc_Tcenv_TunionRes_struct*)_tmp2B)->f1;_tmp2E=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp2B)->f2;_LL26:(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_p_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({
struct Cyc_Absyn_Tunion_p_struct _tmp3B;_tmp3B.tag=6;_tmp3B.f1=_tmp2D;_tmp3B.f2=
_tmp2E;_tmp3B.f3=_tmp3;_tmp3B.f4=_tmp4;_tmp3B;});_tmp3A;})));_npop_handler(0);
return;_LL27: if(*((int*)_tmp2B)!= 3)goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)
_tmp2B)!= 4)goto _LL2B;_LL2A:({void*_tmp3C[0]={};Cyc_Tcutil_terr(p->loc,({const
char*_tmp3D="enum tag used with arguments in pattern";_tag_dynforward(_tmp3D,
sizeof(char),_get_zero_arr_size(_tmp3D,40));}),_tag_dynforward(_tmp3C,sizeof(
void*),0));});(void*)(p->r=(void*)((void*)0));_npop_handler(0);return;_LL2B: if(*((
int*)_tmp2B)!= 0)goto _LL22;_LL2C: goto _LL22;_LL22:;};_pop_handler();}else{void*
_tmp29=(void*)_exn_thrown;void*_tmp3F=_tmp29;_LL2E: if(_tmp3F != Cyc_Dict_Absent)
goto _LL30;_LL2F: goto _LL2D;_LL30:;_LL31:(void)_throw(_tmp3F);_LL2D:;}}}({struct
Cyc_String_pa_struct _tmp42;_tmp42.tag=0;_tmp42.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp2));{void*_tmp40[1]={& _tmp42};
Cyc_Tcutil_terr(p->loc,({const char*_tmp41="%s is not a constructor in pattern";
_tag_dynforward(_tmp41,sizeof(char),_get_zero_arr_size(_tmp41,35));}),
_tag_dynforward(_tmp40,sizeof(void*),1));}});(void*)(p->r=(void*)((void*)0));
return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;_tmp6=(void*)_tmp5.aggr_info;if(*((int*)_tmp6)!= 0)goto _LL7;_tmp7=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6)->f1;_tmp8=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp6)->f2;_tmp9=_tmp5.targs;_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmpB=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f3;_tmpC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f4;_LL6:{struct _handler_cons _tmp43;_push_handler(& _tmp43);{int _tmp45=0;
if(setjmp(_tmp43.handler))_tmp45=1;if(!_tmp45){{struct Cyc_Absyn_Aggrdecl**_tmp46=
Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp47=*
_tmp46;if((void*)_tmp47->kind == (void*)1){({void*_tmp48[0]={};Cyc_Tcutil_terr(p->loc,({
const char*_tmp49="cannot pattern-match a union";_tag_dynforward(_tmp49,sizeof(
char),_get_zero_arr_size(_tmp49,29));}),_tag_dynforward(_tmp48,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}if(_tmp47->impl == 0){({
void*_tmp4A[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4B="can't destructure an abstract struct";
_tag_dynforward(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,37));}),
_tag_dynforward(_tmp4A,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));
_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47->impl))->exist_vars)- ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpA);if(more_exists != 0)({void*_tmp4C[
0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp4D="wrong number of existentially bound type variables";
_tag_dynforward(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,51));}),
_tag_dynforward(_tmp4C,sizeof(void*),0));});(void*)(p->r=(void*)((void*)({struct
Cyc_Absyn_Aggr_p_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp4F;_tmp4F.tag=5;_tmp4F.f1=({struct Cyc_Absyn_AggrInfo _tmp50;_tmp50.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp51=_cycalloc(sizeof(*_tmp51));
_tmp51[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp52;_tmp52.tag=1;_tmp52.f1=_tmp46;
_tmp52;});_tmp51;}));_tmp50.targs=_tmp9;_tmp50;});_tmp4F.f2=_tmpA;_tmp4F.f3=
_tmpB;_tmp4F.f4=_tmpC;_tmp4F;});_tmp4E;})));}};_pop_handler();}else{void*_tmp44=(
void*)_exn_thrown;void*_tmp54=_tmp44;_LL33: if(_tmp54 != Cyc_Dict_Absent)goto _LL35;
_LL34:({void*_tmp55[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp56="Non-struct name has designator patterns";
_tag_dynforward(_tmp56,sizeof(char),_get_zero_arr_size(_tmp56,40));}),
_tag_dynforward(_tmp55,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));goto
_LL32;_LL35:;_LL36:(void)_throw(_tmp54);_LL32:;}}}return;_LL7: if(*((int*)_tmp0)
!= 14)goto _LL9;_tmpD=((struct Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(
te,0,_tmpD);if(!Cyc_Tcutil_is_const_exp(te,_tmpD)){({void*_tmp57[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmp58="non-constant expression in case pattern";
_tag_dynforward(_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,40));}),
_tag_dynforward(_tmp57,sizeof(void*),0));});(void*)(p->r=(void*)((void*)0));}{
unsigned int _tmp5A;int _tmp5B;struct _tuple7 _tmp59=Cyc_Evexp_eval_const_uint_exp(
_tmpD);_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Absyn_Int_p_struct _tmp5D;
_tmp5D.tag=7;_tmp5D.f1=(void*)((void*)2);_tmp5D.f2=(int)_tmp5A;_tmp5D;});_tmp5C;})));
return;}_LL9:;_LLA: return;_LL0:;}static struct _dynforward_ptr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp5E=_cycalloc(sizeof(*
_tmp5E));_tmp5E->v=s;_tmp5E;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp5F=Cyc_Tcutil_compress(
numt);_LL38: if(_tmp5F <= (void*)4)goto _LL3A;if(*((int*)_tmp5F)!= 12)goto _LL3A;
_LL39: if(topt != 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return
numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp60=_cycalloc(
sizeof(*_tmp60));_tmp60->hd=vd;_tmp60->tl=*v_result_ptr;_tmp60;});}static struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,
struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp62;struct Cyc_List_List*_tmp63;
struct Cyc_Tcpat_TcPatResult _tmp61=res1;_tmp62=_tmp61.tvars_and_bounds_opt;_tmp63=
_tmp61.patvars;{struct _tuple6*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_Tcpat_TcPatResult
_tmp64=res2;_tmp65=_tmp64.tvars_and_bounds_opt;_tmp66=_tmp64.patvars;if(_tmp62 != 
0  || _tmp65 != 0){if(_tmp62 == 0)_tmp62=({struct _tuple6*_tmp67=_cycalloc(sizeof(*
_tmp67));_tmp67->f1=0;_tmp67->f2=0;_tmp67;});if(_tmp65 == 0)_tmp65=({struct
_tuple6*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=0;_tmp68->f2=0;_tmp68;});
return({struct Cyc_Tcpat_TcPatResult _tmp69;_tmp69.tvars_and_bounds_opt=({struct
_tuple6*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp62))).f1,(*((struct _tuple6*)_check_null(_tmp65))).f1);_tmp6A->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple6*)_check_null(_tmp62))).f2,(*((struct _tuple6*)_check_null(_tmp65))).f2);
_tmp6A;});_tmp69.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp63,_tmp66);_tmp69;});}return({struct Cyc_Tcpat_TcPatResult
_tmp6B;_tmp6B.tvars_and_bounds_opt=0;_tmp6B.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp63,_tmp66);
_tmp6B;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->loc=
loc;_tmp6C->topt=0;_tmp6C->r=(void*)((void*)0);_tmp6C;});}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp13D;
_tmp13D.tvars_and_bounds_opt=0;_tmp13D.patvars=0;_tmp13D;});{void*_tmp6D=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp6E;struct Cyc_Absyn_Pat*_tmp6F;struct Cyc_Absyn_Vardecl*
_tmp70;struct Cyc_Absyn_Pat*_tmp71;struct Cyc_Absyn_Tvar*_tmp72;struct Cyc_Absyn_Vardecl*
_tmp73;void*_tmp74;void*_tmp75;void*_tmp76;struct Cyc_Absyn_Enumdecl*_tmp77;void*
_tmp78;struct Cyc_Absyn_Pat*_tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_List_List**
_tmp7B;int _tmp7C;struct Cyc_Absyn_AggrInfo _tmp7D;void*_tmp7E;struct Cyc_Absyn_Aggrdecl**
_tmp7F;struct Cyc_Absyn_Aggrdecl*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List**
_tmp82;struct Cyc_List_List*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List**
_tmp85;int _tmp86;struct Cyc_Absyn_Tuniondecl*_tmp87;struct Cyc_Absyn_Tunionfield*
_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_List_List**_tmp8A;int _tmp8B;struct
Cyc_Absyn_AggrInfo _tmp8C;void*_tmp8D;_LL3D: if((int)_tmp6D != 0)goto _LL3F;_LL3E: t=
Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL3C;_LL3F: if(
_tmp6D <= (void*)2)goto _LL53;if(*((int*)_tmp6D)!= 0)goto _LL41;_tmp6E=((struct Cyc_Absyn_Var_p_struct*)
_tmp6D)->f1;_tmp6F=((struct Cyc_Absyn_Var_p_struct*)_tmp6D)->f2;_LL40: res=Cyc_Tcpat_tcPatRec(
te,_tmp6F,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6F->topt))->v;
Cyc_Tcpat_set_vd(_tmp6E,& res.patvars,t);goto _LL3C;_LL41: if(*((int*)_tmp6D)!= 1)
goto _LL43;_tmp70=((struct Cyc_Absyn_Reference_p_struct*)_tmp6D)->f1;_tmp71=((
struct Cyc_Absyn_Reference_p_struct*)_tmp6D)->f2;_LL42: res=Cyc_Tcpat_tcPatRec(te,
_tmp71,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp71->topt))->v;
if(rgn_opt == 0){({void*_tmp8E[0]={};Cyc_Tcutil_terr(p->loc,({const char*_tmp8F="* pattern would point into an unknown/unallowed region";
_tag_dynforward(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,55));}),
_tag_dynforward(_tmp8E,sizeof(void*),0));});goto _LL3C;}Cyc_Tcpat_set_vd(_tmp70,&
res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp90=_cycalloc(sizeof(*
_tmp90));_tmp90[0]=({struct Cyc_Absyn_PointerType_struct _tmp91;_tmp91.tag=4;
_tmp91.f1=({struct Cyc_Absyn_PtrInfo _tmp92;_tmp92.elt_typ=(void*)t;_tmp92.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp92.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp93;_tmp93.rgn=(
void*)*rgn_opt;_tmp93.nullable=Cyc_Absyn_false_conref;_tmp93.bounds=Cyc_Absyn_empty_conref();
_tmp93.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp93.ptrloc=
0;_tmp93;});_tmp92;});_tmp91;});_tmp90;}));goto _LL3C;_LL43: if(*((int*)_tmp6D)!= 
2)goto _LL45;_tmp72=((struct Cyc_Absyn_TagInt_p_struct*)_tmp6D)->f1;_tmp73=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp6D)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp73,& res.patvars,(
void*)_tmp73->type);Cyc_Tcenv_add_type_vars(p->loc,te,({struct Cyc_Absyn_Tvar*
_tmp94[1];_tmp94[0]=_tmp72;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp94,sizeof(struct Cyc_Absyn_Tvar*),1));}));if(res.tvars_and_bounds_opt
== 0)res.tvars_and_bounds_opt=({struct _tuple6*_tmp95=_cycalloc(sizeof(*_tmp95));
_tmp95->f1=0;_tmp95->f2=0;_tmp95;});(*res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->hd=_tmp72;_tmp96->tl=(*res.tvars_and_bounds_opt).f1;
_tmp96;});t=Cyc_Absyn_uint_typ;goto _LL3C;_LL45: if(*((int*)_tmp6D)!= 7)goto _LL47;
_tmp74=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp6D)->f1;if((int)_tmp74 != 1)
goto _LL47;_LL46: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL3C;_LL47:
if(*((int*)_tmp6D)!= 7)goto _LL49;_tmp75=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp6D)->f1;if((int)_tmp75 != 2)goto _LL49;_LL48: goto _LL4A;_LL49: if(*((int*)_tmp6D)
!= 7)goto _LL4B;_tmp76=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp6D)->f1;if((int)
_tmp76 != 0)goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto
_LL3C;_LL4B: if(*((int*)_tmp6D)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL3C;_LL4D: if(*((int*)_tmp6D)!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL3C;_LL4F: if(*((int*)_tmp6D)!= 10)goto _LL51;_tmp77=((
struct Cyc_Absyn_Enum_p_struct*)_tmp6D)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({
struct Cyc_Absyn_EnumType_struct _tmp98;_tmp98.tag=12;_tmp98.f1=_tmp77->name;
_tmp98.f2=(struct Cyc_Absyn_Enumdecl*)_tmp77;_tmp98;});_tmp97;}));goto _LL3C;_LL51:
if(*((int*)_tmp6D)!= 11)goto _LL53;_tmp78=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp6D)->f1;_LL52: t=Cyc_Tcpat_num_type(topt,_tmp78);goto _LL3C;_LL53: if((int)
_tmp6D != 1)goto _LL55;_LL54: if(topt != 0){void*_tmp99=Cyc_Tcutil_compress(*topt);
_LL66: if(_tmp99 <= (void*)4)goto _LL68;if(*((int*)_tmp99)!= 4)goto _LL68;_LL67: t=*
topt;goto tcpat_end;_LL68:;_LL69: goto _LL65;_LL65:;}{struct Cyc_List_List*_tmp9A=
Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_PointerType_struct
_tmp9C;_tmp9C.tag=4;_tmp9C.f1=({struct Cyc_Absyn_PtrInfo _tmp9D;_tmp9D.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->v=_tmp9A;_tmpA0;}));_tmp9D.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp9D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9E;_tmp9E.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F->v=_tmp9A;_tmp9F;}));_tmp9E.nullable=Cyc_Absyn_true_conref;
_tmp9E.bounds=Cyc_Absyn_empty_conref();_tmp9E.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp9E.ptrloc=0;_tmp9E;});_tmp9D;});_tmp9C;});_tmp9B;});
goto _LL3C;}_LL55: if(_tmp6D <= (void*)2)goto _LL57;if(*((int*)_tmp6D)!= 4)goto _LL57;
_tmp79=((struct Cyc_Absyn_Pointer_p_struct*)_tmp6D)->f1;_LL56: {void*inner_typ=(
void*)0;void**_tmpA1=0;if(topt != 0){void*_tmpA2=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpA3;void*_tmpA4;_LL6B: if(_tmpA2 <= (void*)4)goto _LL6D;
if(*((int*)_tmpA2)!= 4)goto _LL6D;_tmpA3=((struct Cyc_Absyn_PointerType_struct*)
_tmpA2)->f1;_tmpA4=(void*)_tmpA3.elt_typ;_LL6C: inner_typ=_tmpA4;_tmpA1=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpB9=_cycalloc(sizeof(*
_tmpB9));_tmpB9->v=Cyc_Tcenv_lookup_type_vars(te);_tmpB9;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp79,_tmpA1,(void**)& ptr_rgn));if(_tmpA1 == 0){void*
_tmpA5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp79->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmpA6;void*_tmpA7;struct Cyc_Absyn_Tuniondecl*
_tmpA8;struct Cyc_Absyn_Tunionfield*_tmpA9;struct Cyc_List_List*_tmpAA;_LL70: if(
_tmpA5 <= (void*)4)goto _LL72;if(*((int*)_tmpA5)!= 3)goto _LL72;_tmpA6=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpA5)->f1;_tmpA7=(void*)_tmpA6.field_info;if(*((int*)_tmpA7)!= 1)goto _LL72;
_tmpA8=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpA7)->f1;_tmpA9=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpA7)->f2;_tmpAA=_tmpA6.targs;_LL71: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_TunionType_struct
_tmpAC;_tmpAC.tag=2;_tmpAC.f1=({struct Cyc_Absyn_TunionInfo _tmpAD;_tmpAD.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpAF;_tmpAF.tag=1;
_tmpAF.f1=({struct Cyc_Absyn_Tuniondecl**_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[
0]=_tmpA8;_tmpB0;});_tmpAF;});_tmpAE;}));_tmpAD.targs=_tmpAA;_tmpAD.rgn=(void*)
ptr_rgn;_tmpAD;});_tmpAC;});_tmpAB;});goto _LL6F;_LL72:;_LL73: t=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({
struct Cyc_Absyn_PointerType_struct _tmpB2;_tmpB2.tag=4;_tmpB2.f1=({struct Cyc_Absyn_PtrInfo
_tmpB3;_tmpB3.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp79->topt))->v);
_tmpB3.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpB3.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpB4;_tmpB4.rgn=(void*)ptr_rgn;_tmpB4.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpB4.bounds=Cyc_Absyn_empty_conref();_tmpB4.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpB4.ptrloc=0;_tmpB4;});
_tmpB3;});_tmpB2;});_tmpB1;});goto _LL6F;_LL6F:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB6;_tmpB6.tag=4;_tmpB6.f1=({struct Cyc_Absyn_PtrInfo _tmpB7;_tmpB7.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp79->topt))->v);_tmpB7.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmpB7.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpB8;_tmpB8.rgn=(
void*)ptr_rgn;_tmpB8.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmpB8.bounds=Cyc_Absyn_empty_conref();_tmpB8.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpB8.ptrloc=0;_tmpB8;});_tmpB7;});_tmpB6;});_tmpB5;});}
goto _LL3C;}}_LL57: if(_tmp6D <= (void*)2)goto _LL59;if(*((int*)_tmp6D)!= 3)goto
_LL59;_tmp7A=((struct Cyc_Absyn_Tuple_p_struct*)_tmp6D)->f1;_tmp7B=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Tuple_p_struct*)_tmp6D)->f1;_tmp7C=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp6D)->f2;_LL58: {struct Cyc_List_List*_tmpBA=*_tmp7B;struct Cyc_List_List*
pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*_tmpBB=Cyc_Tcutil_compress(*
topt);struct Cyc_List_List*_tmpBC;_LL75: if(_tmpBB <= (void*)4)goto _LL77;if(*((int*)
_tmpBB)!= 9)goto _LL77;_tmpBC=((struct Cyc_Absyn_TupleType_struct*)_tmpBB)->f1;
_LL76: topt_ts=_tmpBC;if(_tmp7C){int _tmpBD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpBA);int _tmpBE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBC);if(
_tmpBD < _tmpBE){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpBE - _tmpBD;i
++){wild_ps=({struct Cyc_List_List*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->hd=
Cyc_Tcpat_wild_pat(p->loc);_tmpBF->tl=wild_ps;_tmpBF;});}}*_tmp7B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpBA,wild_ps);
_tmpBA=*_tmp7B;}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)
== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBC))({void*_tmpC0[0]={};
Cyc_Tcutil_warn(p->loc,({const char*_tmpC1="unnecessary ... in tuple pattern";
_tag_dynforward(_tmpC1,sizeof(char),_get_zero_arr_size(_tmpC1,33));}),
_tag_dynforward(_tmpC0,sizeof(void*),0));});}}goto _LL74;_LL77:;_LL78: goto _LL74;
_LL74:;}else{if(_tmp7C)({void*_tmpC2[0]={};Cyc_Tcutil_terr(p->loc,({const char*
_tmpC3="cannot determine missing fields for ... in tuple pattern";
_tag_dynforward(_tmpC3,sizeof(char),_get_zero_arr_size(_tmpC3,57));}),
_tag_dynforward(_tmpC2,sizeof(void*),0));});}for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){
void**_tmpC4=0;if(topt_ts != 0){_tmpC4=(void**)&(*((struct _tuple9*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(
struct Cyc_Absyn_Pat*)_tmpBA->hd,_tmpC4,rgn_opt));pat_ts=({struct Cyc_List_List*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->hd=({struct _tuple9*_tmpC6=_cycalloc(
sizeof(*_tmpC6));_tmpC6->f1=Cyc_Absyn_empty_tqual(0);_tmpC6->f2=(void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)_tmpBA->hd)->topt))->v;_tmpC6;});
_tmpC5->tl=pat_ts;_tmpC5;});}t=(void*)({struct Cyc_Absyn_TupleType_struct*_tmpC7=
_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_TupleType_struct _tmpC8;
_tmpC8.tag=9;_tmpC8.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts);_tmpC8;});_tmpC7;});goto _LL3C;}_LL59: if(_tmp6D <= (void*)2)goto _LL5B;if(*((
int*)_tmp6D)!= 5)goto _LL5B;_tmp7D=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f1;
_tmp7E=(void*)_tmp7D.aggr_info;if(*((int*)_tmp7E)!= 1)goto _LL5B;_tmp7F=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp7E)->f1;_tmp80=*_tmp7F;_tmp81=_tmp7D.targs;
_tmp82=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f1).targs;
_tmp83=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f2;_tmp84=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp6D)->f3;_tmp85=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp6D)->f3;_tmp86=((struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f4;_LL5A: {struct
Cyc_List_List*_tmpC9=*_tmp85;if(_tmp80->impl == 0){({void*_tmpCA[0]={};Cyc_Tcutil_terr(
p->loc,({const char*_tmpCB="can't destructure an abstract struct";_tag_dynforward(
_tmpCB,sizeof(char),_get_zero_arr_size(_tmpCB,37));}),_tag_dynforward(_tmpCA,
sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpCC=_cycalloc(
sizeof(*_tmpCC));_tmpCC->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCC;}));goto _LL3C;}{
struct _RegionHandle _tmpCD=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCD;
_push_region(rgn);{struct Cyc_List_List*_tmpCE=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpCF=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp80->impl))->exist_vars;{struct Cyc_List_List*t=_tmp83;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpCF))->hd;_tmpCF=
_tmpCF->tl;{void*_tmpD0=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpD1=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpD2=_tmpD1;void*_tmpD3;void*_tmpD4;
_LL7A: if(*((int*)_tmpD2)!= 2)goto _LL7C;_tmpD3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD2)->f2;_LL7B: _tmpD4=_tmpD3;goto _LL7D;_LL7C: if(*((int*)_tmpD2)!= 0)goto _LL7E;
_tmpD4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD2)->f1;_LL7D: k2=_tmpD4;goto
_LL79;_LL7E:;_LL7F:({void*_tmpD5[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpD6="unconstrained existential type variable in struct";
_tag_dynforward(_tmpD6,sizeof(char),_get_zero_arr_size(_tmpD6,50));}),
_tag_dynforward(_tmpD5,sizeof(void*),0));});_LL79:;}{void*_tmpD7=_tmpD0;void*
_tmpD8;struct Cyc_Core_Opt*_tmpD9;struct Cyc_Core_Opt**_tmpDA;void*_tmpDB;struct
Cyc_Core_Opt*_tmpDC;struct Cyc_Core_Opt**_tmpDD;_LL81: if(*((int*)_tmpD7)!= 0)goto
_LL83;_tmpD8=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD7)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(
k2,_tmpD8))error=1;goto _LL80;_LL83: if(*((int*)_tmpD7)!= 2)goto _LL85;_tmpD9=((
struct Cyc_Absyn_Less_kb_struct*)_tmpD7)->f1;_tmpDA=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpD7)->f1;_tmpDB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD7)->f2;_LL84: _tmpDD=_tmpDA;goto _LL86;_LL85: if(*((int*)_tmpD7)!= 1)goto _LL80;
_tmpDC=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpD7)->f1;_tmpDD=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD7)->f1;_LL86:*_tmpDD=({struct Cyc_Core_Opt*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->v=(void*)_tmpD1;_tmpDE;});goto _LL80;
_LL80:;}if(error)({struct Cyc_String_pa_struct _tmpE3;_tmpE3.tag=0;_tmpE3.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(k2));{
struct Cyc_String_pa_struct _tmpE2;_tmpE2.tag=0;_tmpE2.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(_tmpD0));{struct Cyc_String_pa_struct
_tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*tv->name);{
void*_tmpDF[3]={& _tmpE1,& _tmpE2,& _tmpE3};Cyc_Tcutil_terr(p->loc,({const char*
_tmpE0="type variable %s has kind %s but must have at least kind %s";
_tag_dynforward(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,60));}),
_tag_dynforward(_tmpDF,sizeof(void*),3));}}}});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Absyn_VarType_struct
_tmpE8;_tmpE8.tag=1;_tmpE8.f1=tv;_tmpE8;});_tmpE7;});_tmpCE=({struct Cyc_List_List*
_tmpE4=_region_malloc(rgn,sizeof(*_tmpE4));_tmpE4->hd=(void*)((void*)vartype);
_tmpE4->tl=_tmpCE;_tmpE4;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->hd=({struct _tuple0*_tmpE6=_cycalloc(
sizeof(*_tmpE6));_tmpE6->f1=Cyc_Absyn_empty_effect;_tmpE6->f2=vartype;_tmpE6;});
_tmpE5->tl=outlives_constraints;_tmpE5;});}}}}_tmpCE=Cyc_List_imp_rev(_tmpCE);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmp83);struct Cyc_List_List*
_tmpE9=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4 _tmpEA=({struct _tuple4
_tmp10B;_tmp10B.f1=_tmpE9;_tmp10B.f2=rgn;_tmp10B;});struct Cyc_List_List*_tmpEB=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmpEA,_tmp80->tvs);struct Cyc_List_List*_tmpEC=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp80->impl))->exist_vars,_tmpCE);struct Cyc_List_List*_tmpED=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpEB);struct Cyc_List_List*_tmpEE=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpEC);struct Cyc_List_List*_tmpEF=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmpEB,_tmpEC);if(_tmp83 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp80->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->f1=0;_tmpF0->f2=0;_tmpF0;});(*
res.tvars_and_bounds_opt).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmp83);(*
res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpF1=0;{struct Cyc_List_List*_tmpF2=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp80->impl))->rgn_po;for(0;_tmpF2 != 
0;_tmpF2=_tmpF2->tl){_tmpF1=({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(*
_tmpF3));_tmpF3->hd=({struct _tuple0*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpEF,(*((struct _tuple0*)_tmpF2->hd)).f1);_tmpF4->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpEF,(*((struct _tuple0*)_tmpF2->hd)).f2);_tmpF4;});
_tmpF3->tl=_tmpF1;_tmpF3;});}}_tmpF1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpF1);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpF1);}}*_tmp82=_tmpED;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpF5=
_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Absyn_AggrType_struct _tmpF6;
_tmpF6.tag=10;_tmpF6.f1=({struct Cyc_Absyn_AggrInfo _tmpF7;_tmpF7.aggr_info=(void*)((
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));
_tmpF8[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpF9;_tmpF9.tag=1;_tmpF9.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA[0]=_tmp80;
_tmpFA;});_tmpF9;});_tmpF8;}));_tmpF7.targs=*_tmp82;_tmpF7;});_tmpF6;});_tmpF5;});
if(_tmp86){int _tmpFB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC9);
int _tmpFC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp80->impl))->fields);if(_tmpFB < _tmpFC){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmpFC - _tmpFB;i ++){wild_dps=({struct Cyc_List_List*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->hd=({struct _tuple8*_tmpFE=_cycalloc(
sizeof(*_tmpFE));_tmpFE->f1=0;_tmpFE->f2=Cyc_Tcpat_wild_pat(p->loc);_tmpFE;});
_tmpFD->tl=wild_dps;_tmpFD;});}}*_tmp85=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC9,wild_dps);_tmpC9=*_tmp85;}
else{if(_tmpFB == _tmpFC)({void*_tmpFF[0]={};Cyc_Tcutil_warn(p->loc,({const char*
_tmp100="unnecessary ... in struct pattern";_tag_dynforward(_tmp100,sizeof(char),
_get_zero_arr_size(_tmp100,34));}),_tag_dynforward(_tmpFF,sizeof(void*),0));});}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,_tmpC9,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp80->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple10 _tmp102;struct Cyc_Absyn_Aggrfield*_tmp103;struct Cyc_Absyn_Pat*_tmp104;
struct _tuple10*_tmp101=(struct _tuple10*)fields->hd;_tmp102=*_tmp101;_tmp103=
_tmp102.f1;_tmp104=_tmp102.f2;{void*_tmp105=Cyc_Tcutil_rsubstitute(rgn,_tmpEF,(
void*)_tmp103->type);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,
_tmp104,(void**)& _tmp105,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp104->topt))->v,_tmp105))({struct Cyc_String_pa_struct _tmp10A;
_tmp10A.tag=0;_tmp10A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp104->topt))->v));{struct Cyc_String_pa_struct
_tmp109;_tmp109.tag=0;_tmp109.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp105));{struct Cyc_String_pa_struct _tmp108;_tmp108.tag=
0;_tmp108.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp103->name);{
void*_tmp106[3]={& _tmp108,& _tmp109,& _tmp10A};Cyc_Tcutil_terr(p->loc,({const char*
_tmp107="field %s of struct pattern expects type %s != %s";_tag_dynforward(
_tmp107,sizeof(char),_get_zero_arr_size(_tmp107,49));}),_tag_dynforward(_tmp106,
sizeof(void*),3));}}}});}}}}};_pop_region(rgn);}goto _LL3C;}_LL5B: if(_tmp6D <= (
void*)2)goto _LL5D;if(*((int*)_tmp6D)!= 6)goto _LL5D;_tmp87=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6D)->f1;_tmp88=((struct Cyc_Absyn_Tunion_p_struct*)_tmp6D)->f2;_tmp89=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp6D)->f3;_tmp8A=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Tunion_p_struct*)_tmp6D)->f3;_tmp8B=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6D)->f4;_LL5C: {struct Cyc_List_List*_tmp10C=*_tmp8A;{struct _RegionHandle
_tmp10D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp10D;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp88->typs;struct Cyc_List_List*_tmp10E=Cyc_Tcenv_lookup_type_vars(
te);struct _tuple4 _tmp10F=({struct _tuple4 _tmp13A;_tmp13A.f1=_tmp10E;_tmp13A.f2=
rgn;_tmp13A;});struct Cyc_List_List*_tmp110=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp10F,_tmp87->tvs);struct Cyc_List_List*_tmp111=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp110);if(tqts == 0)t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_Absyn_TunionType_struct
_tmp113;_tmp113.tag=2;_tmp113.f1=({struct Cyc_Absyn_TunionInfo _tmp114;_tmp114.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp116=_cycalloc(sizeof(*
_tmp116));_tmp116[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp117;_tmp117.tag=1;
_tmp117.f1=({struct Cyc_Absyn_Tuniondecl**_tmp118=_cycalloc(sizeof(*_tmp118));
_tmp118[0]=_tmp87;_tmp118;});_tmp117;});_tmp116;}));_tmp114.targs=_tmp111;
_tmp114.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->v=_tmp10E;
_tmp115;}));_tmp114;});_tmp113;});_tmp112;});else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp11A;_tmp11A.tag=3;_tmp11A.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp11B;
_tmp11B.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp11D;_tmp11D.tag=1;_tmp11D.f1=_tmp87;_tmp11D.f2=_tmp88;_tmp11D;});_tmp11C;}));
_tmp11B.targs=_tmp111;_tmp11B;});_tmp11A;});_tmp119;});}if(topt != 0  && tqts == 0){
void*_tmp11E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp11F;struct
Cyc_List_List*_tmp120;_LL88: if(_tmp11E <= (void*)4)goto _LL8C;if(*((int*)_tmp11E)
!= 3)goto _LL8A;_LL89: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp121=
_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp122;_tmp122.tag=3;_tmp122.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp123;
_tmp123.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp125;_tmp125.tag=1;_tmp125.f1=_tmp87;_tmp125.f2=_tmp88;_tmp125;});_tmp124;}));
_tmp123.targs=_tmp111;_tmp123;});_tmp122;});_tmp121;});goto _LL87;_LL8A: if(*((int*)
_tmp11E)!= 2)goto _LL8C;_tmp11F=((struct Cyc_Absyn_TunionType_struct*)_tmp11E)->f1;
_tmp120=_tmp11F.targs;_LL8B: {struct Cyc_List_List*_tmp126=_tmp111;for(0;_tmp126
!= 0  && _tmp120 != 0;(_tmp126=_tmp126->tl,_tmp120=_tmp120->tl)){Cyc_Tcutil_unify((
void*)_tmp126->hd,(void*)_tmp120->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}
if(_tmp8B){int _tmp127=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10C);
int _tmp128=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp127 < 
_tmp128){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmp128 - _tmp127;i ++){
wild_ps=({struct Cyc_List_List*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->hd=
Cyc_Tcpat_wild_pat(p->loc);_tmp129->tl=wild_ps;_tmp129;});}}*_tmp8A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp10C,
wild_ps);_tmp10C=*_tmp8A;}else{if(_tmp127 == _tmp128)({struct Cyc_String_pa_struct
_tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp87->name));{void*_tmp12A[1]={& _tmp12C};Cyc_Tcutil_warn(
p->loc,({const char*_tmp12B="unnecessary ... in tunion field %s";_tag_dynforward(
_tmp12B,sizeof(char),_get_zero_arr_size(_tmp12B,35));}),_tag_dynforward(_tmp12A,
sizeof(void*),1));}});}}for(0;_tmp10C != 0  && tqts != 0;(_tmp10C=_tmp10C->tl,tqts=
tqts->tl)){struct Cyc_Absyn_Pat*_tmp12D=(struct Cyc_Absyn_Pat*)_tmp10C->hd;void*
_tmp12E=Cyc_Tcutil_rsubstitute(rgn,_tmp110,(*((struct _tuple9*)tqts->hd)).f2);res=
Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp12D,(void**)& _tmp12E,
rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp12D->topt))->v,
_tmp12E))({struct Cyc_String_pa_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp12D->topt))->v));{struct Cyc_String_pa_struct
_tmp132;_tmp132.tag=0;_tmp132.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp12E));{struct Cyc_String_pa_struct _tmp131;_tmp131.tag=
0;_tmp131.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp88->name));{void*_tmp12F[3]={& _tmp131,& _tmp132,& _tmp133};Cyc_Tcutil_terr(
_tmp12D->loc,({const char*_tmp130="%s expects argument type %s, not %s";
_tag_dynforward(_tmp130,sizeof(char),_get_zero_arr_size(_tmp130,36));}),
_tag_dynforward(_tmp12F,sizeof(void*),3));}}}});}if(_tmp10C != 0)({struct Cyc_String_pa_struct
_tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp88->name));{void*_tmp134[1]={& _tmp136};Cyc_Tcutil_terr(
p->loc,({const char*_tmp135="too many arguments for tunion constructor %s";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,45));}),
_tag_dynforward(_tmp134,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_String_pa_struct
_tmp139;_tmp139.tag=0;_tmp139.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp88->name));{void*_tmp137[1]={& _tmp139};Cyc_Tcutil_terr(
p->loc,({const char*_tmp138="too few arguments for tunion constructor %s";
_tag_dynforward(_tmp138,sizeof(char),_get_zero_arr_size(_tmp138,44));}),
_tag_dynforward(_tmp137,sizeof(void*),1));}});};_pop_region(rgn);}goto _LL3C;}
_LL5D: if(_tmp6D <= (void*)2)goto _LL5F;if(*((int*)_tmp6D)!= 5)goto _LL5F;_tmp8C=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp6D)->f1;_tmp8D=(void*)_tmp8C.aggr_info;if(*((
int*)_tmp8D)!= 0)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(_tmp6D <= (void*)2)goto _LL61;
if(*((int*)_tmp6D)!= 12)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmp6D <= (void*)2)
goto _LL63;if(*((int*)_tmp6D)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmp6D <= (
void*)2)goto _LL3C;if(*((int*)_tmp6D)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp13B;}));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp13C=
_cycalloc(sizeof(*_tmp13C));_tmp13C->v=(void*)t;_tmp13C;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp13E=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct
_RegionHandle _tmp13F=_new_region("r");struct _RegionHandle*r=& _tmp13F;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp13E.patvars),p->loc,({const char*
_tmp140="pattern contains a repeated variable";_tag_dynforward(_tmp140,sizeof(
char),_get_zero_arr_size(_tmp140,37));}));;_pop_region(r);}return _tmp13E;}void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*
_tmp141=(void*)p->r;struct Cyc_Absyn_Pat*_tmp142;struct Cyc_List_List*_tmp143;
struct Cyc_List_List*_tmp144;struct Cyc_List_List*_tmp145;_LL8F: if(_tmp141 <= (void*)
2)goto _LL97;if(*((int*)_tmp141)!= 4)goto _LL91;_tmp142=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp141)->f1;_LL90: Cyc_Tcpat_check_pat_regions(te,_tmp142);{void*_tmp146=(void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo _tmp147;
struct Cyc_Absyn_PtrAtts _tmp148;void*_tmp149;struct Cyc_Absyn_TunionInfo _tmp14A;
void*_tmp14B;_LL9A: if(_tmp146 <= (void*)4)goto _LL9E;if(*((int*)_tmp146)!= 4)goto
_LL9C;_tmp147=((struct Cyc_Absyn_PointerType_struct*)_tmp146)->f1;_tmp148=_tmp147.ptr_atts;
_tmp149=(void*)_tmp148.rgn;_LL9B: _tmp14B=_tmp149;goto _LL9D;_LL9C: if(*((int*)
_tmp146)!= 2)goto _LL9E;_tmp14A=((struct Cyc_Absyn_TunionType_struct*)_tmp146)->f1;
_tmp14B=(void*)_tmp14A.rgn;_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,
_tmp14B);return;_LL9E:;_LL9F:({void*_tmp14C[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp14D="check_pat_regions: bad pointer type";
_tag_dynforward(_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,36));}),
_tag_dynforward(_tmp14C,sizeof(void*),0));});_LL99:;}_LL91: if(*((int*)_tmp141)!= 
6)goto _LL93;_tmp143=((struct Cyc_Absyn_Tunion_p_struct*)_tmp141)->f3;_LL92: for(0;
_tmp143 != 0;_tmp143=_tmp143->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp143->hd);}{void*_tmp14E=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_TunionInfo _tmp14F;void*_tmp150;_LLA1: if(_tmp14E <= (void*)4)goto
_LLA5;if(*((int*)_tmp14E)!= 2)goto _LLA3;_tmp14F=((struct Cyc_Absyn_TunionType_struct*)
_tmp14E)->f1;_tmp150=(void*)_tmp14F.rgn;_LLA2: Cyc_Tcenv_check_rgn_accessible(te,
p->loc,_tmp150);return;_LLA3: if(*((int*)_tmp14E)!= 3)goto _LLA5;_LLA4: return;
_LLA5:;_LLA6:({void*_tmp151[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp152="check_pat_regions: bad tunion type";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,35));}),
_tag_dynforward(_tmp151,sizeof(void*),0));});_LLA0:;}_LL93: if(*((int*)_tmp141)!= 
5)goto _LL95;_tmp144=((struct Cyc_Absyn_Aggr_p_struct*)_tmp141)->f3;_LL94: for(0;
_tmp144 != 0;_tmp144=_tmp144->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple8*)_tmp144->hd)).f2);}return;_LL95: if(*((int*)_tmp141)!= 3)goto _LL97;
_tmp145=((struct Cyc_Absyn_Tuple_p_struct*)_tmp141)->f1;_LL96: for(0;_tmp145 != 0;
_tmp145=_tmp145->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp145->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};
struct Cyc_Tcpat_Con_s{void*name;int arity;struct Cyc_Core_Opt*span;struct Cyc_Absyn_Pat*
orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*
f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*
c2){void*_tmp153=(void*)c1->name;struct _dynforward_ptr _tmp154;int _tmp155;_LLA8:
if(*((int*)_tmp153)!= 0)goto _LLAA;_tmp154=((struct Cyc_Tcpat_Name_v_struct*)
_tmp153)->f1;_LLA9: {void*_tmp156=(void*)c2->name;struct _dynforward_ptr _tmp157;
_LLAD: if(*((int*)_tmp156)!= 0)goto _LLAF;_tmp157=((struct Cyc_Tcpat_Name_v_struct*)
_tmp156)->f1;_LLAE: return Cyc_strcmp((struct _dynforward_ptr)_tmp154,(struct
_dynforward_ptr)_tmp157);_LLAF: if(*((int*)_tmp156)!= 1)goto _LLAC;_LLB0: return - 1;
_LLAC:;}_LLAA: if(*((int*)_tmp153)!= 1)goto _LLA7;_tmp155=((struct Cyc_Tcpat_Int_v_struct*)
_tmp153)->f1;_LLAB: {void*_tmp158=(void*)c2->name;int _tmp159;_LLB2: if(*((int*)
_tmp158)!= 0)goto _LLB4;_LLB3: return 1;_LLB4: if(*((int*)_tmp158)!= 1)goto _LLB1;
_tmp159=((struct Cyc_Tcpat_Int_v_struct*)_tmp158)->f1;_LLB5: return _tmp155 - 
_tmp159;_LLB1:;}_LLA7:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct
_RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(
void*)2};static struct Cyc_Core_Opt*Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)&
Cyc_Tcpat_one_opt;static char _tmp15B[5]="NULL";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={0,{_tmp15B,_tmp15B + 5}};static char _tmp15D[2]="&";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={0,{_tmp15D,_tmp15D + 
2}};static char _tmp15F[2]="$";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0,{_tmp15F,_tmp15F + 2}};static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->name=(
void*)((void*)& Cyc_Tcpat_null_name_value);_tmp160->arity=0;_tmp160->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_two_opt;_tmp160->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp160;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->name=(
void*)((void*)& Cyc_Tcpat_ptr_name_value);_tmp161->arity=1;_tmp161->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_two_opt;_tmp161->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp161;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->name=(
void*)((void*)& Cyc_Tcpat_ptr_name_value);_tmp162->arity=1;_tmp162->span=(struct
Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;_tmp162->orig_pat=(struct Cyc_Absyn_Pat*)p;
_tmp162;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp164=_cycalloc_atomic(sizeof(*
_tmp164));_tmp164[0]=({struct Cyc_Tcpat_Int_v_struct _tmp165;_tmp165.tag=1;_tmp165.f1=
i;_tmp165;});_tmp164;}));_tmp163->arity=0;_tmp163->span=0;_tmp163->orig_pat=p;
_tmp163;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct
_dynforward_ptr f,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp166=
_cycalloc(sizeof(*_tmp166));_tmp166->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp168;_tmp168.tag=0;_tmp168.f1=f;_tmp168;});_tmp167;}));_tmp166->arity=0;
_tmp166->span=0;_tmp166->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp166;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->name=(void*)((
void*)({struct Cyc_Tcpat_Int_v_struct*_tmp16B=_cycalloc_atomic(sizeof(*_tmp16B));
_tmp16B[0]=({struct Cyc_Tcpat_Int_v_struct _tmp16C;_tmp16C.tag=1;_tmp16C.f1=(int)c;
_tmp16C;});_tmp16B;}));_tmp169->arity=0;_tmp169->span=({struct Cyc_Core_Opt*
_tmp16A=_cycalloc_atomic(sizeof(*_tmp16A));_tmp16A->v=(void*)256;_tmp16A;});
_tmp169->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp169;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->name=(void*)((void*)& Cyc_Tcpat_tuple_name_value);
_tmp16D->arity=i;_tmp16D->span=Cyc_Tcpat_one_opt_ptr;_tmp16D->orig_pat=p;_tmp16D;});}
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[0]=({struct Cyc_Tcpat_Con_struct
_tmp16F;_tmp16F.tag=0;_tmp16F.f1=Cyc_Tcpat_null_con(p);_tmp16F.f2=0;_tmp16F;});
_tmp16E;});}static void*Cyc_Tcpat_int_pat(int i,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[0]=({
struct Cyc_Tcpat_Con_struct _tmp171;_tmp171.tag=0;_tmp171.f1=Cyc_Tcpat_int_con(i,p);
_tmp171.f2=0;_tmp171;});_tmp170;});}static void*Cyc_Tcpat_char_pat(char c,struct
Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp172=_cycalloc(
sizeof(*_tmp172));_tmp172[0]=({struct Cyc_Tcpat_Con_struct _tmp173;_tmp173.tag=0;
_tmp173.f1=Cyc_Tcpat_char_con(c,p);_tmp173.f2=0;_tmp173;});_tmp172;});}static
void*Cyc_Tcpat_float_pat(struct _dynforward_ptr f,struct Cyc_Absyn_Pat*p){return(
void*)({struct Cyc_Tcpat_Con_struct*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174[0]=({
struct Cyc_Tcpat_Con_struct _tmp175;_tmp175.tag=0;_tmp175.f1=Cyc_Tcpat_float_con(f,
p);_tmp175.f2=0;_tmp175;});_tmp174;});}static void*Cyc_Tcpat_null_ptr_pat(void*p,
struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp176=
_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Tcpat_Con_struct _tmp177;
_tmp177.tag=0;_tmp177.f1=Cyc_Tcpat_null_ptr_con(p0);_tmp177.f2=({struct Cyc_List_List*
_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->hd=(void*)p;_tmp178->tl=0;_tmp178;});
_tmp177;});_tmp176;});}static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*
p0){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp179=_cycalloc(sizeof(*_tmp179));
_tmp179[0]=({struct Cyc_Tcpat_Con_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=Cyc_Tcpat_ptr_con(
p0);_tmp17A.f2=({struct Cyc_List_List*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->hd=(
void*)p;_tmp17B->tl=0;_tmp17B;});_tmp17A;});_tmp179;});}static void*Cyc_Tcpat_tuple_pat(
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Tcpat_Con_struct
_tmp17D;_tmp17D.tag=0;_tmp17D.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss),p);
_tmp17D.f2=ss;_tmp17D;});_tmp17C;});}static void*Cyc_Tcpat_con_pat(struct
_dynforward_ptr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp180=_cycalloc(sizeof(*
_tmp180));_tmp180->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp181=
_cycalloc(sizeof(*_tmp181));_tmp181[0]=({struct Cyc_Tcpat_Name_v_struct _tmp182;
_tmp182.tag=0;_tmp182.f1=con_name;_tmp182;});_tmp181;}));_tmp180->arity=Cyc_List_length(
ps);_tmp180->span=span;_tmp180->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp180;});
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp17E=_cycalloc(sizeof(*_tmp17E));
_tmp17E[0]=({struct Cyc_Tcpat_Con_struct _tmp17F;_tmp17F.tag=0;_tmp17F.f1=c;
_tmp17F.f2=ps;_tmp17F;});_tmp17E;});}static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*
p){void*s;{void*_tmp183=(void*)p->r;void*_tmp184;int _tmp185;char _tmp186;struct
_dynforward_ptr _tmp187;struct Cyc_Absyn_Pat*_tmp188;struct Cyc_Absyn_Pat*_tmp189;
struct Cyc_Absyn_Pat*_tmp18A;struct Cyc_Absyn_Tuniondecl*_tmp18B;struct Cyc_Absyn_Tunionfield*
_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*
_tmp18F;struct Cyc_Absyn_AggrInfo _tmp190;void*_tmp191;struct Cyc_Absyn_Aggrdecl**
_tmp192;struct Cyc_Absyn_Aggrdecl*_tmp193;struct Cyc_List_List*_tmp194;struct Cyc_Absyn_Enumdecl*
_tmp195;struct Cyc_Absyn_Enumfield*_tmp196;void*_tmp197;struct Cyc_Absyn_Enumfield*
_tmp198;_LLB7: if((int)_tmp183 != 0)goto _LLB9;_LLB8: goto _LLBA;_LLB9: if(_tmp183 <= (
void*)2)goto _LLBB;if(*((int*)_tmp183)!= 2)goto _LLBB;_LLBA: s=(void*)0;goto _LLB6;
_LLBB: if((int)_tmp183 != 1)goto _LLBD;_LLBC: s=Cyc_Tcpat_null_pat(p);goto _LLB6;
_LLBD: if(_tmp183 <= (void*)2)goto _LLBF;if(*((int*)_tmp183)!= 7)goto _LLBF;_tmp184=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp183)->f1;_tmp185=((struct Cyc_Absyn_Int_p_struct*)
_tmp183)->f2;_LLBE: s=Cyc_Tcpat_int_pat(_tmp185,(struct Cyc_Absyn_Pat*)p);goto
_LLB6;_LLBF: if(_tmp183 <= (void*)2)goto _LLC1;if(*((int*)_tmp183)!= 8)goto _LLC1;
_tmp186=((struct Cyc_Absyn_Char_p_struct*)_tmp183)->f1;_LLC0: s=Cyc_Tcpat_char_pat(
_tmp186,p);goto _LLB6;_LLC1: if(_tmp183 <= (void*)2)goto _LLC3;if(*((int*)_tmp183)!= 
9)goto _LLC3;_tmp187=((struct Cyc_Absyn_Float_p_struct*)_tmp183)->f1;_LLC2: s=Cyc_Tcpat_float_pat(
_tmp187,p);goto _LLB6;_LLC3: if(_tmp183 <= (void*)2)goto _LLC5;if(*((int*)_tmp183)!= 
0)goto _LLC5;_tmp188=((struct Cyc_Absyn_Var_p_struct*)_tmp183)->f2;_LLC4: s=Cyc_Tcpat_compile_pat(
_tmp188);goto _LLB6;_LLC5: if(_tmp183 <= (void*)2)goto _LLC7;if(*((int*)_tmp183)!= 1)
goto _LLC7;_tmp189=((struct Cyc_Absyn_Reference_p_struct*)_tmp183)->f2;_LLC6: s=Cyc_Tcpat_compile_pat(
_tmp189);goto _LLB6;_LLC7: if(_tmp183 <= (void*)2)goto _LLC9;if(*((int*)_tmp183)!= 4)
goto _LLC9;_tmp18A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp183)->f1;_LLC8:{void*
_tmp199=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp19A;struct Cyc_Absyn_PtrAtts _tmp19B;struct Cyc_Absyn_Conref*
_tmp19C;_LLDE: if(_tmp199 <= (void*)4)goto _LLE2;if(*((int*)_tmp199)!= 4)goto _LLE0;
_tmp19A=((struct Cyc_Absyn_PointerType_struct*)_tmp199)->f1;_tmp19B=_tmp19A.ptr_atts;
_tmp19C=_tmp19B.nullable;_LLDF: {int is_nullable=0;int still_working=1;while(
still_working){void*_tmp19D=(void*)_tmp19C->v;struct Cyc_Absyn_Conref*_tmp19E;int
_tmp19F;_LLE5: if(_tmp19D <= (void*)1)goto _LLE7;if(*((int*)_tmp19D)!= 1)goto _LLE7;
_tmp19E=((struct Cyc_Absyn_Forward_constr_struct*)_tmp19D)->f1;_LLE6:(void*)(
_tmp19C->v=(void*)((void*)_tmp19E->v));continue;_LLE7: if((int)_tmp19D != 0)goto
_LLE9;_LLE8:(void*)(_tmp19C->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp1A1;_tmp1A1.tag=0;_tmp1A1.f1=(void*)0;_tmp1A1;});_tmp1A0;})));is_nullable=0;
still_working=0;goto _LLE4;_LLE9: if(_tmp19D <= (void*)1)goto _LLE4;if(*((int*)
_tmp19D)!= 0)goto _LLE4;_tmp19F=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp19D)->f1;
_LLEA: is_nullable=(int)_tmp19F;still_working=0;goto _LLE4;_LLE4:;}{void*ss=Cyc_Tcpat_compile_pat(
_tmp18A);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(ss,p);else{s=Cyc_Tcpat_ptr_pat(
ss,p);}goto _LLDD;}}_LLE0: if(*((int*)_tmp199)!= 2)goto _LLE2;_LLE1:{void*_tmp1A2=(
void*)_tmp18A->r;struct Cyc_Absyn_Tuniondecl*_tmp1A3;struct Cyc_Absyn_Tunionfield*
_tmp1A4;struct Cyc_List_List*_tmp1A5;_LLEC: if(_tmp1A2 <= (void*)2)goto _LLEE;if(*((
int*)_tmp1A2)!= 6)goto _LLEE;_tmp1A3=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A2)->f1;
_tmp1A4=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A2)->f2;_tmp1A5=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1A2)->f3;_LLED: {struct Cyc_Core_Opt*span;if(_tmp1A3->is_xtunion)span=0;else{
span=({struct Cyc_Core_Opt*_tmp1A6=_cycalloc_atomic(sizeof(*_tmp1A6));_tmp1A6->v=(
void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1A3->fields))->v);_tmp1A6;});}s=Cyc_Tcpat_con_pat(*(*
_tmp1A4->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1A5),p);goto _LLEB;}
_LLEE:;_LLEF:({void*_tmp1A7[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A8="non-[x]tunion pattern has tunion type";
_tag_dynforward(_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,38));}),
_tag_dynforward(_tmp1A7,sizeof(void*),0));});_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({
void*_tmp1A9[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1AA="pointer pattern does not have pointer type";
_tag_dynforward(_tmp1AA,sizeof(char),_get_zero_arr_size(_tmp1AA,43));}),
_tag_dynforward(_tmp1A9,sizeof(void*),0));});_LLDD:;}goto _LLB6;_LLC9: if(_tmp183
<= (void*)2)goto _LLCB;if(*((int*)_tmp183)!= 6)goto _LLCB;_tmp18B=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp183)->f1;_tmp18C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp183)->f2;_tmp18D=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp183)->f3;if(_tmp18D != 0)goto _LLCB;_LLCA: {
struct Cyc_Core_Opt*span;{void*_tmp1AB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);_LLF1: if(_tmp1AB <= (void*)4)goto _LLF5;if(*((int*)
_tmp1AB)!= 2)goto _LLF3;_LLF2: if(_tmp18B->is_xtunion)span=0;else{span=({struct Cyc_Core_Opt*
_tmp1AC=_cycalloc_atomic(sizeof(*_tmp1AC));_tmp1AC->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp18B->fields))->v);_tmp1AC;});}goto _LLF0;_LLF3: if(*((int*)_tmp1AB)!= 3)goto
_LLF5;_LLF4: span=({struct Cyc_Core_Opt*_tmp1AD=_cycalloc_atomic(sizeof(*_tmp1AD));
_tmp1AD->v=(void*)1;_tmp1AD;});goto _LLF0;_LLF5:;_LLF6: span=({void*_tmp1AE[0]={};((
struct Cyc_Core_Opt*(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1AF="void tunion pattern has bad type";_tag_dynforward(_tmp1AF,
sizeof(char),_get_zero_arr_size(_tmp1AF,33));}),_tag_dynforward(_tmp1AE,sizeof(
void*),0));});goto _LLF0;_LLF0:;}s=Cyc_Tcpat_con_pat(*(*_tmp18C->name).f2,span,0,
p);goto _LLB6;}_LLCB: if(_tmp183 <= (void*)2)goto _LLCD;if(*((int*)_tmp183)!= 6)goto
_LLCD;_tmp18E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp183)->f3;_LLCC: _tmp18F=
_tmp18E;goto _LLCE;_LLCD: if(_tmp183 <= (void*)2)goto _LLCF;if(*((int*)_tmp183)!= 3)
goto _LLCF;_tmp18F=((struct Cyc_Absyn_Tuple_p_struct*)_tmp183)->f1;_LLCE: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp18F),(struct Cyc_Absyn_Pat*)p);goto _LLB6;
_LLCF: if(_tmp183 <= (void*)2)goto _LLD1;if(*((int*)_tmp183)!= 5)goto _LLD1;_tmp190=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp183)->f1;_tmp191=(void*)_tmp190.aggr_info;if(*((
int*)_tmp191)!= 1)goto _LLD1;_tmp192=((struct Cyc_Absyn_KnownAggr_struct*)_tmp191)->f1;
_tmp193=*_tmp192;_tmp194=((struct Cyc_Absyn_Aggr_p_struct*)_tmp183)->f3;_LLD0: {
struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp193->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=
Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({
const char*_tmp1BE="";_tag_dynforward(_tmp1BE,sizeof(char),_get_zero_arr_size(
_tmp1BE,1));}))== 0;{struct Cyc_List_List*dlps0=_tmp194;for(0;!found  && dlps0 != 0;
dlps0=dlps0->tl){struct _tuple8 _tmp1B1;struct Cyc_List_List*_tmp1B2;struct Cyc_Absyn_Pat*
_tmp1B3;struct _tuple8*_tmp1B0=(struct _tuple8*)dlps0->hd;_tmp1B1=*_tmp1B0;_tmp1B2=
_tmp1B1.f1;_tmp1B3=_tmp1B1.f2;{struct Cyc_List_List*_tmp1B4=_tmp1B2;struct Cyc_List_List
_tmp1B5;void*_tmp1B6;struct _dynforward_ptr*_tmp1B7;struct Cyc_List_List*_tmp1B8;
_LLF8: if(_tmp1B4 == 0)goto _LLFA;_tmp1B5=*_tmp1B4;_tmp1B6=(void*)_tmp1B5.hd;if(*((
int*)_tmp1B6)!= 1)goto _LLFA;_tmp1B7=((struct Cyc_Absyn_FieldName_struct*)_tmp1B6)->f1;
_tmp1B8=_tmp1B5.tl;if(_tmp1B8 != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1B7,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp1B3);_tmp1B9->tl=ps;_tmp1B9;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*
_tmp1BA[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1BB="bad designator(s)";_tag_dynforward(_tmp1BB,sizeof(char),
_get_zero_arr_size(_tmp1BB,18));}),_tag_dynforward(_tmp1BA,sizeof(void*),0));});
_LLF7:;}}}if(!found)({void*_tmp1BC[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1BD="bad designator";
_tag_dynforward(_tmp1BD,sizeof(char),_get_zero_arr_size(_tmp1BD,15));}),
_tag_dynforward(_tmp1BC,sizeof(void*),0));});}}s=Cyc_Tcpat_tuple_pat(ps,(struct
Cyc_Absyn_Pat*)p);goto _LLB6;}_LLD1: if(_tmp183 <= (void*)2)goto _LLD3;if(*((int*)
_tmp183)!= 10)goto _LLD3;_tmp195=((struct Cyc_Absyn_Enum_p_struct*)_tmp183)->f1;
_tmp196=((struct Cyc_Absyn_Enum_p_struct*)_tmp183)->f2;_LLD2: {int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp195->fields))->v);s=Cyc_Tcpat_con_pat(*(*_tmp196->name).f2,({
struct Cyc_Core_Opt*_tmp1BF=_cycalloc_atomic(sizeof(*_tmp1BF));_tmp1BF->v=(void*)
span;_tmp1BF;}),0,p);goto _LLB6;}_LLD3: if(_tmp183 <= (void*)2)goto _LLD5;if(*((int*)
_tmp183)!= 11)goto _LLD5;_tmp197=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp183)->f1;_tmp198=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp183)->f2;_LLD4: {
struct Cyc_List_List*fields;{void*_tmp1C0=Cyc_Tcutil_compress(_tmp197);struct Cyc_List_List*
_tmp1C1;_LLFD: if(_tmp1C0 <= (void*)4)goto _LLFF;if(*((int*)_tmp1C0)!= 13)goto _LLFF;
_tmp1C1=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1C0)->f1;_LLFE: fields=_tmp1C1;
goto _LLFC;_LLFF:;_LL100:({void*_tmp1C2[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C3="bad type in AnonEnum_p";
_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,23));}),
_tag_dynforward(_tmp1C2,sizeof(void*),0));});_LLFC:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*_tmp198->name).f2,({struct Cyc_Core_Opt*
_tmp1C4=_cycalloc_atomic(sizeof(*_tmp1C4));_tmp1C4->v=(void*)span;_tmp1C4;}),0,p);
goto _LLB6;}}_LLD5: if(_tmp183 <= (void*)2)goto _LLD7;if(*((int*)_tmp183)!= 12)goto
_LLD7;_LLD6: goto _LLD8;_LLD7: if(_tmp183 <= (void*)2)goto _LLD9;if(*((int*)_tmp183)
!= 13)goto _LLD9;_LLD8: goto _LLDA;_LLD9: if(_tmp183 <= (void*)2)goto _LLDB;if(*((int*)
_tmp183)!= 5)goto _LLDB;_LLDA: goto _LLDC;_LLDB: if(_tmp183 <= (void*)2)goto _LLB6;if(*((
int*)_tmp183)!= 14)goto _LLB6;_LLDC: s=(void*)0;_LLB6:;}return s;}struct Cyc_Tcpat_Pos_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};
struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp1C5=td;struct Cyc_Set_Set*_tmp1C6;_LL102: if(*((int*)_tmp1C5)!= 1)goto
_LL104;_tmp1C6=((struct Cyc_Tcpat_Neg_struct*)_tmp1C5)->f1;_LL103: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1C6,c))({void*
_tmp1C7[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1C8="add_neg called when constructor already in set";
_tag_dynforward(_tmp1C8,sizeof(char),_get_zero_arr_size(_tmp1C8,47));}),
_tag_dynforward(_tmp1C7,sizeof(void*),0));});if(c->span != 0  && ((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1C6)+ 1 >= (int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v)({void*_tmp1C9[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1CA="add_neg called when |cs U {c}| >= span(c)";
_tag_dynforward(_tmp1CA,sizeof(char),_get_zero_arr_size(_tmp1CA,42));}),
_tag_dynforward(_tmp1C9,sizeof(void*),0));});return(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp1CB=_region_malloc(r,sizeof(*_tmp1CB));_tmp1CB[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1CC;_tmp1CC.tag=1;_tmp1CC.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp1C6,c);
_tmp1CC;});_tmp1CB;});_LL104: if(*((int*)_tmp1C5)!= 0)goto _LL101;_LL105:({void*
_tmp1CD[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1CE="add_neg called when td is Positive";_tag_dynforward(_tmp1CE,
sizeof(char),_get_zero_arr_size(_tmp1CE,35));}),_tag_dynforward(_tmp1CD,sizeof(
void*),0));});_LL101:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,
void*td){void*_tmp1CF=td;struct Cyc_Tcpat_Con_s*_tmp1D0;struct Cyc_Set_Set*_tmp1D1;
_LL107: if(*((int*)_tmp1CF)!= 0)goto _LL109;_tmp1D0=((struct Cyc_Tcpat_Pos_struct*)
_tmp1CF)->f1;_LL108: if(Cyc_Tcpat_compare_con(c,_tmp1D0)== 0)return(void*)0;else{
return(void*)1;}_LL109: if(*((int*)_tmp1CF)!= 1)goto _LL106;_tmp1D1=((struct Cyc_Tcpat_Neg_struct*)
_tmp1CF)->f1;_LL10A: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1D1,c))return(void*)1;else{if(c->span != 0  && (int)((struct
Cyc_Core_Opt*)_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1D1)+ 1)return(void*)0;else{return(void*)2;}}_LL106:;}struct _tuple12{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp1D2=ctxt;struct Cyc_List_List _tmp1D3;struct _tuple12*_tmp1D4;struct _tuple12
_tmp1D5;struct Cyc_Tcpat_Con_s*_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*
_tmp1D8;_LL10C: if(_tmp1D2 != 0)goto _LL10E;_LL10D: return 0;_LL10E: if(_tmp1D2 == 0)
goto _LL10B;_tmp1D3=*_tmp1D2;_tmp1D4=(struct _tuple12*)_tmp1D3.hd;_tmp1D5=*_tmp1D4;
_tmp1D6=_tmp1D5.f1;_tmp1D7=_tmp1D5.f2;_tmp1D8=_tmp1D3.tl;_LL10F: return({struct
Cyc_List_List*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));_tmp1D9->hd=({struct
_tuple12*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));_tmp1DA->f1=_tmp1D6;_tmp1DA->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1DB=_region_malloc(r,sizeof(*
_tmp1DB));_tmp1DB->hd=(void*)dsc;_tmp1DB->tl=_tmp1D7;_tmp1DB;});_tmp1DA;});
_tmp1D9->tl=_tmp1D8;_tmp1D9;});_LL10B:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1DC=ctxt;
struct Cyc_List_List _tmp1DD;struct _tuple12*_tmp1DE;struct _tuple12 _tmp1DF;struct
Cyc_Tcpat_Con_s*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E2;
_LL111: if(_tmp1DC != 0)goto _LL113;_LL112:({void*_tmp1E3[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp1E4="norm_context: empty context";_tag_dynforward(_tmp1E4,sizeof(char),
_get_zero_arr_size(_tmp1E4,28));}),_tag_dynforward(_tmp1E3,sizeof(void*),0));});
_LL113: if(_tmp1DC == 0)goto _LL110;_tmp1DD=*_tmp1DC;_tmp1DE=(struct _tuple12*)
_tmp1DD.hd;_tmp1DF=*_tmp1DE;_tmp1E0=_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;_tmp1E2=
_tmp1DD.tl;_LL114: return Cyc_Tcpat_augment(r,_tmp1E2,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1E6;_tmp1E6.tag=0;_tmp1E6.f1=_tmp1E0;_tmp1E6.f2=Cyc_List_rrev(r,_tmp1E1);
_tmp1E6;});_tmp1E5;}));_LL110:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1E8=({struct _tuple6 _tmp1E7;_tmp1E7.f1=ctxt;_tmp1E7.f2=work;
_tmp1E7;});struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*
_tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_List_List
_tmp1EE;struct _tuple12*_tmp1EF;struct _tuple12 _tmp1F0;struct Cyc_Tcpat_Con_s*
_tmp1F1;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List*
_tmp1F4;struct Cyc_List_List _tmp1F5;struct _tuple11*_tmp1F6;struct _tuple11 _tmp1F7;
struct Cyc_List_List*_tmp1F8;struct Cyc_List_List*_tmp1F9;_LL116: _tmp1E9=_tmp1E8.f1;
if(_tmp1E9 != 0)goto _LL118;_tmp1EA=_tmp1E8.f2;if(_tmp1EA != 0)goto _LL118;_LL117:
return dsc;_LL118: _tmp1EB=_tmp1E8.f1;if(_tmp1EB != 0)goto _LL11A;_LL119: goto _LL11B;
_LL11A: _tmp1EC=_tmp1E8.f2;if(_tmp1EC != 0)goto _LL11C;_LL11B:({void*_tmp1FA[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1FB="build_desc: ctxt and work don't match";_tag_dynforward(
_tmp1FB,sizeof(char),_get_zero_arr_size(_tmp1FB,38));}),_tag_dynforward(_tmp1FA,
sizeof(void*),0));});_LL11C: _tmp1ED=_tmp1E8.f1;if(_tmp1ED == 0)goto _LL115;_tmp1EE=*
_tmp1ED;_tmp1EF=(struct _tuple12*)_tmp1EE.hd;_tmp1F0=*_tmp1EF;_tmp1F1=_tmp1F0.f1;
_tmp1F2=_tmp1F0.f2;_tmp1F3=_tmp1EE.tl;_tmp1F4=_tmp1E8.f2;if(_tmp1F4 == 0)goto
_LL115;_tmp1F5=*_tmp1F4;_tmp1F6=(struct _tuple11*)_tmp1F5.hd;_tmp1F7=*_tmp1F6;
_tmp1F8=_tmp1F7.f3;_tmp1F9=_tmp1F5.tl;_LL11D: {struct Cyc_Tcpat_Pos_struct*
_tmp1FC=({struct Cyc_Tcpat_Pos_struct*_tmp1FD=_region_malloc(r,sizeof(*_tmp1FD));
_tmp1FD[0]=({struct Cyc_Tcpat_Pos_struct _tmp1FE;_tmp1FE.tag=0;_tmp1FE.f1=_tmp1F1;
_tmp1FE.f2=Cyc_List_rappend(r,Cyc_List_rrev(r,_tmp1F2),({struct Cyc_List_List*
_tmp1FF=_region_malloc(r,sizeof(*_tmp1FF));_tmp1FF->hd=(void*)dsc;_tmp1FF->tl=
_tmp1F8;_tmp1FF;}));_tmp1FE;});_tmp1FD;});return Cyc_Tcpat_build_desc(r,_tmp1F3,(
void*)_tmp1FC,_tmp1F9);}_LL115:;}static void*Cyc_Tcpat_match(struct _RegionHandle*,
void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,void*,
struct Cyc_List_List*);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp200=allmrules;struct
Cyc_List_List _tmp201;struct _tuple0*_tmp202;struct _tuple0 _tmp203;void*_tmp204;
void*_tmp205;struct Cyc_List_List*_tmp206;_LL11F: if(_tmp200 != 0)goto _LL121;_LL120:
return(void*)({struct Cyc_Tcpat_Failure_struct*_tmp207=_cycalloc(sizeof(*_tmp207));
_tmp207[0]=({struct Cyc_Tcpat_Failure_struct _tmp208;_tmp208.tag=0;_tmp208.f1=(
void*)dsc;_tmp208;});_tmp207;});_LL121: if(_tmp200 == 0)goto _LL11E;_tmp201=*
_tmp200;_tmp202=(struct _tuple0*)_tmp201.hd;_tmp203=*_tmp202;_tmp204=_tmp203.f1;
_tmp205=_tmp203.f2;_tmp206=_tmp201.tl;_LL122: return Cyc_Tcpat_match(r,_tmp204,0,
dsc,0,0,_tmp205,_tmp206);_LL11E:;}static void*Cyc_Tcpat_match_compile(struct
_RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp209=_region_malloc(r,sizeof(*_tmp209));_tmp209[0]=({
struct Cyc_Tcpat_Neg_struct _tmp20A;_tmp20A.tag=1;_tmp20A.f1=Cyc_Tcpat_empty_con_set(
r);_tmp20A;});_tmp209;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp20B=work;
struct Cyc_List_List _tmp20C;struct _tuple11*_tmp20D;struct _tuple11 _tmp20E;struct
Cyc_List_List*_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_List_List*_tmp211;
struct Cyc_List_List*_tmp212;struct Cyc_List_List _tmp213;struct _tuple11*_tmp214;
struct _tuple11 _tmp215;struct Cyc_List_List*_tmp216;struct Cyc_List_List*_tmp217;
struct Cyc_List_List*_tmp218;struct Cyc_List_List*_tmp219;_LL124: if(_tmp20B != 0)
goto _LL126;_LL125: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp21A=
_region_malloc(r,sizeof(*_tmp21A));_tmp21A[0]=({struct Cyc_Tcpat_Success_struct
_tmp21B;_tmp21B.tag=1;_tmp21B.f1=(void*)right_hand_side;_tmp21B;});_tmp21A;});
_LL126: if(_tmp20B == 0)goto _LL128;_tmp20C=*_tmp20B;_tmp20D=(struct _tuple11*)
_tmp20C.hd;_tmp20E=*_tmp20D;_tmp20F=_tmp20E.f1;if(_tmp20F != 0)goto _LL128;_tmp210=
_tmp20E.f2;if(_tmp210 != 0)goto _LL128;_tmp211=_tmp20E.f3;if(_tmp211 != 0)goto
_LL128;_tmp212=_tmp20C.tl;_LL127: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp212,right_hand_side,rules);_LL128: if(_tmp20B == 0)goto _LL123;_tmp213=*
_tmp20B;_tmp214=(struct _tuple11*)_tmp213.hd;_tmp215=*_tmp214;_tmp216=_tmp215.f1;
_tmp217=_tmp215.f2;_tmp218=_tmp215.f3;_tmp219=_tmp213.tl;_LL129: if((_tmp216 == 0
 || _tmp217 == 0) || _tmp218 == 0)({void*_tmp21C[0]={};Cyc_Tcutil_impos(({const
char*_tmp21D="tcpat:and_match: malformed work frame";_tag_dynforward(_tmp21D,
sizeof(char),_get_zero_arr_size(_tmp21D,38));}),_tag_dynforward(_tmp21C,sizeof(
void*),0));});{struct Cyc_List_List _tmp21F;void*_tmp220;struct Cyc_List_List*
_tmp221;struct Cyc_List_List*_tmp21E=(struct Cyc_List_List*)_tmp216;_tmp21F=*
_tmp21E;_tmp220=(void*)_tmp21F.hd;_tmp221=_tmp21F.tl;{struct Cyc_List_List _tmp223;
struct Cyc_List_List*_tmp224;struct Cyc_List_List*_tmp225;struct Cyc_List_List*
_tmp222=(struct Cyc_List_List*)_tmp217;_tmp223=*_tmp222;_tmp224=(struct Cyc_List_List*)
_tmp223.hd;_tmp225=_tmp223.tl;{struct Cyc_List_List _tmp227;void*_tmp228;struct Cyc_List_List*
_tmp229;struct Cyc_List_List*_tmp226=(struct Cyc_List_List*)_tmp218;_tmp227=*
_tmp226;_tmp228=(void*)_tmp227.hd;_tmp229=_tmp227.tl;{struct _tuple11*_tmp22A=({
struct _tuple11*_tmp22C=_region_malloc(r,sizeof(*_tmp22C));_tmp22C->f1=_tmp221;
_tmp22C->f2=_tmp225;_tmp22C->f3=_tmp229;_tmp22C;});return Cyc_Tcpat_match(r,
_tmp220,_tmp224,_tmp228,ctx,({struct Cyc_List_List*_tmp22B=_region_malloc(r,
sizeof(*_tmp22B));_tmp22B->hd=_tmp22A;_tmp22B->tl=_tmp219;_tmp22B;}),
right_hand_side,rules);}}}}_LL123:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp22D=dsc;
struct Cyc_Set_Set*_tmp22E;struct Cyc_List_List*_tmp22F;_LL12B: if(*((int*)_tmp22D)
!= 1)goto _LL12D;_tmp22E=((struct Cyc_Tcpat_Neg_struct*)_tmp22D)->f1;_LL12C: {void*
any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp232=_region_malloc(r,sizeof(*_tmp232));
_tmp232[0]=({struct Cyc_Tcpat_Neg_struct _tmp233;_tmp233.tag=1;_tmp233.f1=Cyc_Tcpat_empty_con_set(
r);_tmp233;});_tmp232;});struct Cyc_List_List*_tmp230=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp230=({struct Cyc_List_List*_tmp231=_region_malloc(r,sizeof(*_tmp231));
_tmp231->hd=(void*)any;_tmp231->tl=_tmp230;_tmp231;});}}return _tmp230;}_LL12D:
if(*((int*)_tmp22D)!= 0)goto _LL12A;_tmp22F=((struct Cyc_Tcpat_Pos_struct*)_tmp22D)->f2;
_LL12E: return _tmp22F;_LL12A:;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct
_tuple13 _tmp235;struct _RegionHandle*_tmp236;struct Cyc_List_List*_tmp237;struct
_tuple13*_tmp234=env;_tmp235=*_tmp234;_tmp236=_tmp235.f1;_tmp237=_tmp235.f2;
return({struct Cyc_List_List*_tmp238=_region_malloc(_tmp236,sizeof(*_tmp238));
_tmp238->hd=(void*)(i + 1);_tmp238->tl=_tmp237;_tmp238;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple13 _tmp239=({struct _tuple13 _tmp23A;_tmp23A.f1=r;_tmp23A.f2=obj;
_tmp23A;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp239);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp23B=p;struct Cyc_Tcpat_Con_s*
_tmp23C;struct Cyc_List_List*_tmp23D;_LL130: if((int)_tmp23B != 0)goto _LL132;_LL131:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL132: if(_tmp23B <= (void*)1)goto _LL12F;if(*((int*)_tmp23B)!= 0)goto
_LL12F;_tmp23C=((struct Cyc_Tcpat_Con_struct*)_tmp23B)->f1;_tmp23D=((struct Cyc_Tcpat_Con_struct*)
_tmp23B)->f2;_LL133: {void*_tmp23E=Cyc_Tcpat_static_match(_tmp23C,dsc);_LL135:
if((int)_tmp23E != 0)goto _LL137;_LL136: {struct Cyc_List_List*_tmp23F=({struct Cyc_List_List*
_tmp244=_region_malloc(r,sizeof(*_tmp244));_tmp244->hd=({struct _tuple12*_tmp245=
_region_malloc(r,sizeof(*_tmp245));_tmp245->f1=_tmp23C;_tmp245->f2=0;_tmp245;});
_tmp244->tl=ctx;_tmp244;});struct _tuple11*_tmp240=({struct _tuple11*_tmp243=
_region_malloc(r,sizeof(*_tmp243));_tmp243->f1=_tmp23D;_tmp243->f2=Cyc_Tcpat_getoargs(
r,_tmp23C,obj);_tmp243->f3=Cyc_Tcpat_getdargs(r,_tmp23C,dsc);_tmp243;});struct
Cyc_List_List*_tmp241=({struct Cyc_List_List*_tmp242=_region_malloc(r,sizeof(*
_tmp242));_tmp242->hd=_tmp240;_tmp242->tl=work;_tmp242;});return Cyc_Tcpat_and_match(
r,_tmp23F,_tmp241,right_hand_side,rules);}_LL137: if((int)_tmp23E != 1)goto _LL139;
_LL138: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL139: if((int)_tmp23E != 2)goto _LL134;_LL13A: {struct Cyc_List_List*_tmp246=({
struct Cyc_List_List*_tmp24F=_region_malloc(r,sizeof(*_tmp24F));_tmp24F->hd=({
struct _tuple12*_tmp250=_region_malloc(r,sizeof(*_tmp250));_tmp250->f1=_tmp23C;
_tmp250->f2=0;_tmp250;});_tmp24F->tl=ctx;_tmp24F;});struct _tuple11*_tmp247=({
struct _tuple11*_tmp24E=_region_malloc(r,sizeof(*_tmp24E));_tmp24E->f1=_tmp23D;
_tmp24E->f2=Cyc_Tcpat_getoargs(r,_tmp23C,obj);_tmp24E->f3=Cyc_Tcpat_getdargs(r,
_tmp23C,dsc);_tmp24E;});struct Cyc_List_List*_tmp248=({struct Cyc_List_List*
_tmp24D=_region_malloc(r,sizeof(*_tmp24D));_tmp24D->hd=_tmp247;_tmp24D->tl=work;
_tmp24D;});void*_tmp249=Cyc_Tcpat_and_match(r,_tmp246,_tmp248,right_hand_side,
rules);void*_tmp24A=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp23C),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp24B=
_region_malloc(r,sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp24C;_tmp24C.tag=2;_tmp24C.f1=obj;_tmp24C.f2=_tmp23C;_tmp24C.f3=(void*)
_tmp249;_tmp24C.f4=(void*)_tmp24A;_tmp24C;});_tmp24B;});}_LL134:;}_LL12F:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp251=d;void*_tmp252;void*_tmp253;void*_tmp254;void*_tmp255;_LL13C: if(*((int*)
_tmp251)!= 0)goto _LL13E;_tmp252=(void*)((struct Cyc_Tcpat_Failure_struct*)_tmp251)->f1;
_LL13D: if(!(*exhaust_done)){not_exhaust(env1,_tmp252);*exhaust_done=1;}goto
_LL13B;_LL13E: if(*((int*)_tmp251)!= 1)goto _LL140;_tmp253=(void*)((struct Cyc_Tcpat_Success_struct*)
_tmp251)->f1;_LL13F: rhs_appears(env2,_tmp253);goto _LL13B;_LL140: if(*((int*)
_tmp251)!= 2)goto _LL13B;_tmp254=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp251)->f3;
_tmp255=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp251)->f4;_LL141: Cyc_Tcpat_check_exhaust_overlap(
_tmp254,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
_tmp255,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13B;_LL13B:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp257;struct _RegionHandle*_tmp258;int*_tmp259;struct
_tuple16*_tmp256=env;_tmp257=*_tmp256;_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;{
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple14*rhs=({struct _tuple14*
_tmp25F=_region_malloc(_tmp258,sizeof(*_tmp25F));_tmp25F->f1=0;_tmp25F->f2=(swc->pattern)->loc;
_tmp25F;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->hd=(void*)sp0;_tmp25A->tl=({struct
Cyc_List_List*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp259,0);_tmp25B->tl=0;_tmp25B;});_tmp25A;}),0);*_tmp259=*_tmp259 + 1;}else{sp=
Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*_tmp25C=_cycalloc(sizeof(*_tmp25C));
_tmp25C->hd=(void*)sp0;_tmp25C->tl=({struct Cyc_List_List*_tmp25D=_cycalloc(
sizeof(*_tmp25D));_tmp25D->hd=(void*)((void*)0);_tmp25D->tl=0;_tmp25D;});_tmp25C;}),
0);}return({struct _tuple15*_tmp25E=_region_malloc(_tmp258,sizeof(*_tmp25E));
_tmp25E->f1=sp;_tmp25E->f2=rhs;_tmp25E;});}}void Cyc_Tcpat_add_con_to_list(struct
Cyc_Tcpat_Con_s*c,struct Cyc_List_List**cs){*cs=({struct Cyc_List_List*_tmp260=
_cycalloc(sizeof(*_tmp260));_tmp260->hd=c;_tmp260->tl=*cs;_tmp260;});}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _dynforward_ptr Cyc_Tcpat_descs2string(
struct Cyc_List_List*);static struct _dynforward_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp261=d;struct Cyc_Tcpat_Con_s*_tmp262;struct Cyc_List_List*_tmp263;struct
Cyc_Set_Set*_tmp264;_LL143: if(*((int*)_tmp261)!= 0)goto _LL145;_tmp262=((struct
Cyc_Tcpat_Pos_struct*)_tmp261)->f1;_tmp263=((struct Cyc_Tcpat_Pos_struct*)_tmp261)->f2;
_LL144: {void*_tmp265=(void*)_tmp262->name;struct Cyc_Absyn_Pat*_tmp266=_tmp262->orig_pat;
if(_tmp266 == 0)return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)
_check_null(_tmp263))->hd);{void*_tmp267=(void*)_tmp266->r;struct Cyc_Absyn_Vardecl*
_tmp268;struct Cyc_Absyn_Vardecl*_tmp269;struct Cyc_Absyn_Tvar*_tmp26A;struct Cyc_Absyn_Vardecl*
_tmp26B;struct Cyc_Absyn_Pat*_tmp26C;struct Cyc_Absyn_Pat _tmp26D;void*_tmp26E;
struct Cyc_Absyn_Tunionfield*_tmp26F;struct Cyc_Absyn_AggrInfo _tmp270;void*_tmp271;
struct Cyc_Absyn_Aggrdecl**_tmp272;struct Cyc_Absyn_Aggrdecl*_tmp273;struct Cyc_Absyn_Tunionfield*
_tmp274;int _tmp275;char _tmp276;struct _dynforward_ptr _tmp277;struct Cyc_Absyn_Enumfield*
_tmp278;struct Cyc_Absyn_Enumfield*_tmp279;struct Cyc_Absyn_Exp*_tmp27A;_LL148: if((
int)_tmp267 != 0)goto _LL14A;_LL149: return({const char*_tmp27B="_";_tag_dynforward(
_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,2));});_LL14A: if(_tmp267 <= (void*)
2)goto _LL15A;if(*((int*)_tmp267)!= 0)goto _LL14C;_tmp268=((struct Cyc_Absyn_Var_p_struct*)
_tmp267)->f1;_LL14B: return Cyc_Absynpp_qvar2string(_tmp268->name);_LL14C: if(*((
int*)_tmp267)!= 1)goto _LL14E;_tmp269=((struct Cyc_Absyn_Reference_p_struct*)
_tmp267)->f1;_LL14D: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp27E;_tmp27E.tag=0;_tmp27E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp269->name));{void*_tmp27C[1]={& _tmp27E};Cyc_aprintf(({
const char*_tmp27D="*%s";_tag_dynforward(_tmp27D,sizeof(char),_get_zero_arr_size(
_tmp27D,4));}),_tag_dynforward(_tmp27C,sizeof(void*),1));}});_LL14E: if(*((int*)
_tmp267)!= 2)goto _LL150;_tmp26A=((struct Cyc_Absyn_TagInt_p_struct*)_tmp267)->f1;
_tmp26B=((struct Cyc_Absyn_TagInt_p_struct*)_tmp267)->f2;_LL14F: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp282;_tmp282.tag=0;_tmp282.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp26A->name);{struct Cyc_String_pa_struct
_tmp281;_tmp281.tag=0;_tmp281.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp26B->name));{void*_tmp27F[2]={& _tmp281,& _tmp282};Cyc_aprintf(({
const char*_tmp280="%s<`%s>";_tag_dynforward(_tmp280,sizeof(char),
_get_zero_arr_size(_tmp280,8));}),_tag_dynforward(_tmp27F,sizeof(void*),2));}}});
_LL150: if(*((int*)_tmp267)!= 3)goto _LL152;_LL151: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp285;_tmp285.tag=0;_tmp285.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp263));{void*
_tmp283[1]={& _tmp285};Cyc_aprintf(({const char*_tmp284="$(%s)";_tag_dynforward(
_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,6));}),_tag_dynforward(_tmp283,
sizeof(void*),1));}});_LL152: if(*((int*)_tmp267)!= 4)goto _LL154;_tmp26C=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp267)->f1;_tmp26D=*_tmp26C;_tmp26E=(void*)_tmp26D.r;
if(_tmp26E <= (void*)2)goto _LL154;if(*((int*)_tmp26E)!= 6)goto _LL154;_tmp26F=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp26E)->f2;_LL153: return(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp289;_tmp289.tag=0;_tmp289.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp263));{
struct Cyc_String_pa_struct _tmp288;_tmp288.tag=0;_tmp288.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp26F->name));{
void*_tmp286[2]={& _tmp288,& _tmp289};Cyc_aprintf(({const char*_tmp287="&%s(%s)";
_tag_dynforward(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,8));}),
_tag_dynforward(_tmp286,sizeof(void*),2));}}});_LL154: if(*((int*)_tmp267)!= 4)
goto _LL156;_LL155: return(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp28C;_tmp28C.tag=0;_tmp28C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcpat_descs2string(_tmp263));{void*_tmp28A[1]={& _tmp28C};Cyc_aprintf(({const
char*_tmp28B="&%s";_tag_dynforward(_tmp28B,sizeof(char),_get_zero_arr_size(
_tmp28B,4));}),_tag_dynforward(_tmp28A,sizeof(void*),1));}});_LL156: if(*((int*)
_tmp267)!= 5)goto _LL158;_tmp270=((struct Cyc_Absyn_Aggr_p_struct*)_tmp267)->f1;
_tmp271=(void*)_tmp270.aggr_info;if(*((int*)_tmp271)!= 1)goto _LL158;_tmp272=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp271)->f1;_tmp273=*_tmp272;_LL157: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp290;_tmp290.tag=0;_tmp290.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcpat_descs2string(_tmp263));{
struct Cyc_String_pa_struct _tmp28F;_tmp28F.tag=0;_tmp28F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp273->name));{
void*_tmp28D[2]={& _tmp28F,& _tmp290};Cyc_aprintf(({const char*_tmp28E="%s{%s}";
_tag_dynforward(_tmp28E,sizeof(char),_get_zero_arr_size(_tmp28E,7));}),
_tag_dynforward(_tmp28D,sizeof(void*),2));}}});_LL158: if(*((int*)_tmp267)!= 6)
goto _LL15A;_tmp274=((struct Cyc_Absyn_Tunion_p_struct*)_tmp267)->f2;_LL159: return(
struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp293;_tmp293.tag=0;_tmp293.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp274->name));{
void*_tmp291[1]={& _tmp293};Cyc_aprintf(({const char*_tmp292="%s";_tag_dynforward(
_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,3));}),_tag_dynforward(_tmp291,
sizeof(void*),1));}});_LL15A: if((int)_tmp267 != 1)goto _LL15C;_LL15B: return({const
char*_tmp294="NULL";_tag_dynforward(_tmp294,sizeof(char),_get_zero_arr_size(
_tmp294,5));});_LL15C: if(_tmp267 <= (void*)2)goto _LL168;if(*((int*)_tmp267)!= 7)
goto _LL15E;_tmp275=((struct Cyc_Absyn_Int_p_struct*)_tmp267)->f2;_LL15D: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp297;_tmp297.tag=1;_tmp297.f1=(
unsigned long)_tmp275;{void*_tmp295[1]={& _tmp297};Cyc_aprintf(({const char*
_tmp296="%d";_tag_dynforward(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,3));}),
_tag_dynforward(_tmp295,sizeof(void*),1));}});_LL15E: if(*((int*)_tmp267)!= 8)
goto _LL160;_tmp276=((struct Cyc_Absyn_Char_p_struct*)_tmp267)->f1;_LL15F: return(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp29A;_tmp29A.tag=1;_tmp29A.f1=(
unsigned long)((int)_tmp276);{void*_tmp298[1]={& _tmp29A};Cyc_aprintf(({const char*
_tmp299="%d";_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size(_tmp299,3));}),
_tag_dynforward(_tmp298,sizeof(void*),1));}});_LL160: if(*((int*)_tmp267)!= 9)
goto _LL162;_tmp277=((struct Cyc_Absyn_Float_p_struct*)_tmp267)->f1;_LL161: return
_tmp277;_LL162: if(*((int*)_tmp267)!= 10)goto _LL164;_tmp278=((struct Cyc_Absyn_Enum_p_struct*)
_tmp267)->f2;_LL163: _tmp279=_tmp278;goto _LL165;_LL164: if(*((int*)_tmp267)!= 11)
goto _LL166;_tmp279=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp267)->f2;_LL165:
return Cyc_Absynpp_qvar2string(_tmp279->name);_LL166: if(*((int*)_tmp267)!= 14)
goto _LL168;_tmp27A=((struct Cyc_Absyn_Exp_p_struct*)_tmp267)->f1;_LL167: return Cyc_Absynpp_exp2string(
_tmp27A);_LL168:;_LL169:(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL147:;}}
_LL145: if(*((int*)_tmp261)!= 1)goto _LL142;_tmp264=((struct Cyc_Tcpat_Neg_struct*)
_tmp261)->f1;_LL146: if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp264))
return({const char*_tmp29B="_";_tag_dynforward(_tmp29B,sizeof(char),
_get_zero_arr_size(_tmp29B,2));});{struct Cyc_Tcpat_Con_s*_tmp29C=((struct Cyc_Tcpat_Con_s*(*)(
struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp264);struct Cyc_Absyn_Pat*_tmp29D=
_tmp29C->orig_pat;if(_tmp29D == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{void*
_tmp29E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp29D->topt))->v);
void*_tmp29F;struct Cyc_Absyn_PtrInfo _tmp2A0;struct Cyc_Absyn_PtrAtts _tmp2A1;
struct Cyc_Absyn_TunionInfo _tmp2A2;void*_tmp2A3;struct Cyc_Absyn_Tuniondecl**
_tmp2A4;struct Cyc_Absyn_Tuniondecl*_tmp2A5;_LL16B: if(_tmp29E <= (void*)4)goto
_LL173;if(*((int*)_tmp29E)!= 5)goto _LL16D;_tmp29F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp29E)->f2;if((int)_tmp29F != 0)goto _LL16D;_LL16C:{int i=0;for(0;i < 256;i ++){
struct Cyc_Tcpat_Con_s*_tmp2A6=Cyc_Tcpat_char_con((char)i,(struct Cyc_Absyn_Pat*)
_tmp29D);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp264,_tmp2A6))return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2A9;
_tmp2A9.tag=1;_tmp2A9.f1=(unsigned long)i;{void*_tmp2A7[1]={& _tmp2A9};Cyc_aprintf(({
const char*_tmp2A8="%d";_tag_dynforward(_tmp2A8,sizeof(char),_get_zero_arr_size(
_tmp2A8,3));}),_tag_dynforward(_tmp2A7,sizeof(void*),1));}});}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL16D: if(*((int*)_tmp29E)!= 5)goto _LL16F;_LL16E:{
unsigned int i=0;for(0;i < 0 - 1;i ++){struct Cyc_Tcpat_Con_s*_tmp2AA=Cyc_Tcpat_int_con((
int)i,_tmp29D);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp264,_tmp2AA))return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp2AD;
_tmp2AD.tag=1;_tmp2AD.f1=(unsigned long)((int)i);{void*_tmp2AB[1]={& _tmp2AD};Cyc_aprintf(({
const char*_tmp2AC="%d";_tag_dynforward(_tmp2AC,sizeof(char),_get_zero_arr_size(
_tmp2AC,3));}),_tag_dynforward(_tmp2AB,sizeof(void*),1));}});}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL16F: if(*((int*)_tmp29E)!= 4)goto _LL171;_tmp2A0=((
struct Cyc_Absyn_PointerType_struct*)_tmp29E)->f1;_tmp2A1=_tmp2A0.ptr_atts;_LL170: {
struct Cyc_Absyn_Conref*_tmp2AE=_tmp2A1.nullable;int is_nullable=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2AE);if(is_nullable){if(!((
int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp264,
Cyc_Tcpat_null_con((struct Cyc_Absyn_Pat*)_tmp29D)))return({const char*_tmp2AF="NULL";
_tag_dynforward(_tmp2AF,sizeof(char),_get_zero_arr_size(_tmp2AF,5));});}return({
const char*_tmp2B0="&_";_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(
_tmp2B0,3));});}_LL171: if(*((int*)_tmp29E)!= 2)goto _LL173;_tmp2A2=((struct Cyc_Absyn_TunionType_struct*)
_tmp29E)->f1;_tmp2A3=(void*)_tmp2A2.tunion_info;if(*((int*)_tmp2A3)!= 1)goto
_LL173;_tmp2A4=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A3)->f1;_tmp2A5=*
_tmp2A4;_LL172: if(_tmp2A5->is_xtunion)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2B1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2A5->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2B1);for(0;(unsigned int)_tmp2B1;_tmp2B1=_tmp2B1->tl){struct _dynforward_ptr n=*(*((
struct Cyc_Absyn_Tunionfield*)_tmp2B1->hd)->name).f2;struct Cyc_List_List*_tmp2B2=((
struct Cyc_Absyn_Tunionfield*)_tmp2B1->hd)->typs;if(!((int(*)(struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp264,({struct Cyc_Tcpat_Con_s*
_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=n;_tmp2B5;});_tmp2B4;}));_tmp2B3->arity=0;
_tmp2B3->span=0;_tmp2B3->orig_pat=_tmp29D;_tmp2B3;}))){if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2B2)== 0)return n;else{return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp2B8;_tmp2B8.tag=0;_tmp2B8.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)n);{void*_tmp2B6[1]={& _tmp2B8};Cyc_aprintf(({
const char*_tmp2B7="&%s(...)";_tag_dynforward(_tmp2B7,sizeof(char),
_get_zero_arr_size(_tmp2B7,9));}),_tag_dynforward(_tmp2B6,sizeof(void*),1));}});}}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);}_LL173:;_LL174:(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16A:;}}_LL142:;}static struct _dynforward_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dynforward_ptr*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9[0]=Cyc_Tcpat_desc2string(
d);_tmp2B9;});}static struct _dynforward_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp2BA=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dynforward_ptr*comma=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),",",sizeof(char),2);{struct Cyc_List_List*_tmp2BB=_tmp2BA;for(0;
_tmp2BB != 0;_tmp2BB=((struct Cyc_List_List*)_check_null(_tmp2BB))->tl){if(_tmp2BB->tl
!= 0){_tmp2BB->tl=({struct Cyc_List_List*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));
_tmp2BC->hd=comma;_tmp2BC->tl=_tmp2BB->tl;_tmp2BC;});_tmp2BB=_tmp2BB->tl;}}}
return(struct _dynforward_ptr)Cyc_strconcat_l(_tmp2BA);}static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment*loc,void*desc){struct _handler_cons _tmp2BE;
_push_handler(& _tmp2BE);{int _tmp2C0=0;if(setjmp(_tmp2BE.handler))_tmp2C0=1;if(!
_tmp2C0){{struct _dynforward_ptr _tmp2C1=Cyc_Tcpat_desc2string(desc);({struct Cyc_String_pa_struct
_tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp2C1);{void*_tmp2C2[1]={& _tmp2C4};Cyc_Tcutil_terr(loc,({const char*_tmp2C3="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,53));}),
_tag_dynforward(_tmp2C2,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2BF=(void*)_exn_thrown;void*_tmp2C6=_tmp2BF;_LL176: if(_tmp2C6 != Cyc_Tcpat_Desc2string)
goto _LL178;_LL177:({void*_tmp2C7[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp2C8="patterns may not be exhaustive.";
_tag_dynforward(_tmp2C8,sizeof(char),_get_zero_arr_size(_tmp2C8,32));}),
_tag_dynforward(_tmp2C7,sizeof(void*),0));});goto _LL175;_LL178:;_LL179:(void)
_throw(_tmp2C6);_LL175:;}}}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct _RegionHandle _tmp2C9=_new_region("r");struct
_RegionHandle*r=& _tmp2C9;_push_region(r);{int _tmp2CA=0;struct _tuple16 _tmp2CB=({
struct _tuple16 _tmp2D7;_tmp2D7.f1=r;_tmp2D7.f2=& _tmp2CA;_tmp2D7;});struct Cyc_List_List*
_tmp2CC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(
struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct _tuple16*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp2CB,swcs);void*_tmp2CD=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2CC);int _tmp2CE=0;((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp2CD,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2CE);for(0;_tmp2CC != 0;_tmp2CC=_tmp2CC->tl){
struct _tuple15 _tmp2D0;struct _tuple14*_tmp2D1;struct _tuple14 _tmp2D2;int _tmp2D3;
struct Cyc_Position_Segment*_tmp2D4;struct _tuple15*_tmp2CF=(struct _tuple15*)
_tmp2CC->hd;_tmp2D0=*_tmp2CF;_tmp2D1=_tmp2D0.f2;_tmp2D2=*_tmp2D1;_tmp2D3=_tmp2D2.f1;
_tmp2D4=_tmp2D2.f2;if(!_tmp2D3)({void*_tmp2D5[0]={};Cyc_Tcutil_terr(_tmp2D4,({
const char*_tmp2D6="redundant pattern";_tag_dynforward(_tmp2D6,sizeof(char),
_get_zero_arr_size(_tmp2D6,18));}),_tag_dynforward(_tmp2D5,sizeof(void*),0));});}};
_pop_region(r);}struct _tuple17{struct Cyc_Position_Segment*f1;int f2;};static void
Cyc_Tcpat_not_exhaust_warn(struct _tuple17*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp2D8;_push_handler(& _tmp2D8);{int _tmp2DA=0;if(setjmp(_tmp2D8.handler))
_tmp2DA=1;if(!_tmp2DA){{struct _dynforward_ptr _tmp2DB=Cyc_Tcpat_desc2string(desc);({
struct Cyc_String_pa_struct _tmp2DE;_tmp2DE.tag=0;_tmp2DE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2DB);{void*_tmp2DC[1]={& _tmp2DE};Cyc_Tcutil_warn((*
pr).f1,({const char*_tmp2DD="pattern not exhaustive.\n\tmissing case for %s";
_tag_dynforward(_tmp2DD,sizeof(char),_get_zero_arr_size(_tmp2DD,45));}),
_tag_dynforward(_tmp2DC,sizeof(void*),1));}});};_pop_handler();}else{void*
_tmp2D9=(void*)_exn_thrown;void*_tmp2E0=_tmp2D9;_LL17B: if(_tmp2E0 != Cyc_Tcpat_Desc2string)
goto _LL17D;_LL17C:({void*_tmp2E1[0]={};Cyc_Tcutil_warn((*pr).f1,({const char*
_tmp2E2="pattern not exhaustive.";_tag_dynforward(_tmp2E2,sizeof(char),
_get_zero_arr_size(_tmp2E2,24));}),_tag_dynforward(_tmp2E1,sizeof(void*),0));});
goto _LL17A;_LL17D:;_LL17E:(void)_throw(_tmp2E0);_LL17A:;}}}}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple18{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp2E3=
_new_region("r");struct _RegionHandle*r=& _tmp2E3;_push_region(r);{struct Cyc_List_List*
_tmp2E4=({struct Cyc_List_List*_tmp2EA=_region_malloc(r,sizeof(*_tmp2EA));_tmp2EA->hd=({
struct _tuple18*_tmp2EB=_region_malloc(r,sizeof(*_tmp2EB));_tmp2EB->f1=Cyc_Tcpat_compile_pat(
p);_tmp2EB->f2=0;_tmp2EB;});_tmp2EA->tl=0;_tmp2EA;});void*_tmp2E5=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2E4);struct _tuple17 _tmp2E6=({struct _tuple17 _tmp2E9;_tmp2E9.f1=loc;_tmp2E9.f2=
1;_tmp2E9;});int _tmp2E7=0;((void(*)(void*d,void(*not_exhaust)(struct _tuple17*,
void*),struct _tuple17*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))
Cyc_Tcpat_check_exhaust_overlap)(_tmp2E5,Cyc_Tcpat_not_exhaust_warn,& _tmp2E6,Cyc_Tcpat_dummy_fn,
0,& _tmp2E7);{int _tmp2E8=_tmp2E6.f2;_npop_handler(0);return _tmp2E8;}};_pop_region(
r);}static struct _tuple15*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple14*rhs=({struct _tuple14*
_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->f1=0;_tmp2ED->f2=(swc->pattern)->loc;
_tmp2ED;});return({struct _tuple15*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC->f1=
sp0;_tmp2EC->f2=rhs;_tmp2EC;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*swcs){struct Cyc_List_List*_tmp2EE=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp2EF=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp2EE);
int _tmp2F0=0;((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp2EF,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2F0);for(0;_tmp2EE != 0;_tmp2EE=_tmp2EE->tl){
struct _tuple15 _tmp2F2;struct _tuple14*_tmp2F3;struct _tuple14 _tmp2F4;int _tmp2F5;
struct Cyc_Position_Segment*_tmp2F6;struct _tuple15*_tmp2F1=(struct _tuple15*)
_tmp2EE->hd;_tmp2F2=*_tmp2F1;_tmp2F3=_tmp2F2.f2;_tmp2F4=*_tmp2F3;_tmp2F5=_tmp2F4.f1;
_tmp2F6=_tmp2F4.f2;if(!_tmp2F5)({void*_tmp2F7[0]={};Cyc_Tcutil_terr(_tmp2F6,({
const char*_tmp2F8="redundant pattern";_tag_dynforward(_tmp2F8,sizeof(char),
_get_zero_arr_size(_tmp2F8,18));}),_tag_dynforward(_tmp2F7,sizeof(void*),0));});}}
