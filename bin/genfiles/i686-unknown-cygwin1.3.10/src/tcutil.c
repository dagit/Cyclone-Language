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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
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
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(
void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*
f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(
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
struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
void*f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{
int print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;
void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;
};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
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
extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _tuple4{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple5{void*f1;struct _tuple1*f2;};struct _tuple5 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);
struct _dynforward_ptr Cyc_Absynpp_kindbound2string(void*);struct _dynforward_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct
_tuple6{void*f1;void*f2;};struct _tuple6*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict*d);struct _tuple6*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
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
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple4*);int Cyc_Tcutil_supports_default(
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
_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(*((int*)_tmp3C)!= 16)
goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;if(_tmp3E != 0)
goto _LL5;_LL4: return t;_LL5: if(*((int*)_tmp3C)!= 16)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)
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
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp45=(void*)c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((int)_tmp45
!= 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if(_tmp45 <= (void*)1)goto
_LL10;if(*((int*)_tmp45)!= 0)goto _LL10;_tmp46=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp45)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp46);_LL10: if(_tmp45 <= (void*)1)
goto _LLB;if(*((int*)_tmp45)!= 1)goto _LLB;_tmp47=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp45)->f1;_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
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
_tmp51;});}static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct
_tuple2 _tmp53;struct Cyc_Core_Opt*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;
struct _tuple2*_tmp52=arg;_tmp53=*_tmp52;_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;_tmp56=
_tmp53.f3;return({struct _tuple2*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=
_tmp54;_tmp57->f2=_tmp55;_tmp57->f3=Cyc_Tcutil_copy_type(_tmp56);_tmp57;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){struct _tuple4 _tmp59;
struct Cyc_Absyn_Tqual _tmp5A;void*_tmp5B;struct _tuple4*_tmp58=arg;_tmp59=*_tmp58;
_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;return({struct _tuple4*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C->f1=_tmp5A;_tmp5C->f2=Cyc_Tcutil_copy_type(_tmp5B);_tmp5C;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->name=
f->name;_tmp5D->tq=f->tq;_tmp5D->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp5D->width=f->width;_tmp5D->attributes=f->attributes;_tmp5D;});}static struct
_tuple6*Cyc_Tcutil_copy_rgncmp(struct _tuple6*x){struct _tuple6 _tmp5F;void*_tmp60;
void*_tmp61;struct _tuple6*_tmp5E=x;_tmp5F=*_tmp5E;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;
return({struct _tuple6*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=Cyc_Tcutil_copy_type(
_tmp60);_tmp62->f2=Cyc_Tcutil_copy_type(_tmp61);_tmp62;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->name=f->name;_tmp63->tag=f->tag;_tmp63->loc=
f->loc;_tmp63;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp64=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp65;struct Cyc_Absyn_TunionInfo _tmp66;void*_tmp67;
struct Cyc_List_List*_tmp68;void*_tmp69;struct Cyc_Absyn_TunionFieldInfo _tmp6A;
void*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_PtrInfo _tmp6D;void*_tmp6E;
struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_PtrAtts _tmp70;void*_tmp71;struct Cyc_Absyn_Conref*
_tmp72;struct Cyc_Absyn_Conref*_tmp73;struct Cyc_Absyn_Conref*_tmp74;struct Cyc_Absyn_PtrLoc*
_tmp75;void*_tmp76;void*_tmp77;int _tmp78;struct Cyc_Absyn_ArrayInfo _tmp79;void*
_tmp7A;struct Cyc_Absyn_Tqual _tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Conref*
_tmp7D;struct Cyc_Position_Segment*_tmp7E;struct Cyc_Absyn_FnInfo _tmp7F;struct Cyc_List_List*
_tmp80;struct Cyc_Core_Opt*_tmp81;void*_tmp82;struct Cyc_List_List*_tmp83;int
_tmp84;struct Cyc_Absyn_VarargInfo*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*
_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Absyn_AggrInfo _tmp89;void*_tmp8A;
void*_tmp8B;struct _tuple1*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_AggrInfo
_tmp8E;void*_tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;struct Cyc_List_List*_tmp91;
void*_tmp92;struct Cyc_List_List*_tmp93;struct _tuple1*_tmp94;struct Cyc_Absyn_Enumdecl*
_tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;int _tmp98;void*_tmp99;void*_tmp9A;
struct _tuple1*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Typedefdecl*
_tmp9D;void*_tmp9E;struct Cyc_List_List*_tmp9F;void*_tmpA0;_LL1A: if((int)_tmp64 != 
0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp64 <= (void*)4)goto _LL28;if(*((int*)
_tmp64)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(*((int*)_tmp64)!= 1)goto _LL20;
_tmp65=((struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;_LL1F: return(void*)({struct
Cyc_Absyn_VarType_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_VarType_struct
_tmpA2;_tmpA2.tag=1;_tmpA2.f1=Cyc_Tcutil_copy_tvar(_tmp65);_tmpA2;});_tmpA1;});
_LL20: if(*((int*)_tmp64)!= 2)goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)
_tmp64)->f1;_tmp67=(void*)_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=(void*)
_tmp66.rgn;_LL21: return(void*)({struct Cyc_Absyn_TunionType_struct*_tmpA3=
_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_TunionType_struct _tmpA4;
_tmpA4.tag=2;_tmpA4.f1=({struct Cyc_Absyn_TunionInfo _tmpA5;_tmpA5.tunion_info=(
void*)_tmp67;_tmpA5.targs=Cyc_Tcutil_copy_types(_tmp68);_tmpA5.rgn=(void*)Cyc_Tcutil_copy_type(
_tmp69);_tmpA5;});_tmpA4;});_tmpA3;});_LL22: if(*((int*)_tmp64)!= 3)goto _LL24;
_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;_tmp6B=(void*)
_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpA7;_tmpA7.tag=3;_tmpA7.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpA8;_tmpA8.field_info=(
void*)_tmp6B;_tmpA8.targs=Cyc_Tcutil_copy_types(_tmp6C);_tmpA8;});_tmpA7;});
_tmpA6;});_LL24: if(*((int*)_tmp64)!= 4)goto _LL26;_tmp6D=((struct Cyc_Absyn_PointerType_struct*)
_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;
_tmp71=(void*)_tmp70.rgn;_tmp72=_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=
_tmp70.zero_term;_tmp75=_tmp70.ptrloc;_LL25: {void*_tmpA9=Cyc_Tcutil_copy_type(
_tmp6E);void*_tmpAA=Cyc_Tcutil_copy_type(_tmp71);struct Cyc_Absyn_Conref*_tmpAB=((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp72);struct Cyc_Absyn_Tqual _tmpAC=_tmp6F;struct Cyc_Absyn_Conref*_tmpAD=Cyc_Tcutil_copy_conref(
_tmp73);struct Cyc_Absyn_Conref*_tmpAE=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB0;_tmpB0.tag=4;_tmpB0.f1=({struct Cyc_Absyn_PtrInfo _tmpB1;_tmpB1.elt_typ=(
void*)_tmpA9;_tmpB1.elt_tq=_tmpAC;_tmpB1.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpB2;_tmpB2.rgn=(void*)_tmpAA;_tmpB2.nullable=_tmpAB;_tmpB2.bounds=_tmpAD;
_tmpB2.zero_term=_tmpAE;_tmpB2.ptrloc=_tmp75;_tmpB2;});_tmpB1;});_tmpB0;});
_tmpAF;});}_LL26: if(*((int*)_tmp64)!= 5)goto _LL28;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64)->f1;_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f2;_LL27:
return(void*)({struct Cyc_Absyn_IntType_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));
_tmpB3[0]=({struct Cyc_Absyn_IntType_struct _tmpB4;_tmpB4.tag=5;_tmpB4.f1=(void*)
_tmp76;_tmpB4.f2=(void*)_tmp77;_tmpB4;});_tmpB3;});_LL28: if((int)_tmp64 != 1)goto
_LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (void*)4)goto _LL46;if(*((int*)_tmp64)!= 6)
goto _LL2C;_tmp78=((struct Cyc_Absyn_DoubleType_struct*)_tmp64)->f1;_LL2B: return(
void*)({struct Cyc_Absyn_DoubleType_struct*_tmpB5=_cycalloc_atomic(sizeof(*_tmpB5));
_tmpB5[0]=({struct Cyc_Absyn_DoubleType_struct _tmpB6;_tmpB6.tag=6;_tmpB6.f1=
_tmp78;_tmpB6;});_tmpB5;});_LL2C: if(*((int*)_tmp64)!= 7)goto _LL2E;_tmp79=((
struct Cyc_Absyn_ArrayType_struct*)_tmp64)->f1;_tmp7A=(void*)_tmp79.elt_type;
_tmp7B=_tmp79.tq;_tmp7C=_tmp79.num_elts;_tmp7D=_tmp79.zero_term;_tmp7E=_tmp79.zt_loc;
_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB7=_cycalloc(sizeof(*
_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB8;_tmpB8.tag=7;_tmpB8.f1=({
struct Cyc_Absyn_ArrayInfo _tmpB9;_tmpB9.elt_type=(void*)Cyc_Tcutil_copy_type(
_tmp7A);_tmpB9.tq=_tmp7B;_tmpB9.num_elts=_tmp7C;_tmpB9.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp7D);_tmpB9.zt_loc=_tmp7E;
_tmpB9;});_tmpB8;});_tmpB7;});_LL2E: if(*((int*)_tmp64)!= 8)goto _LL30;_tmp7F=((
struct Cyc_Absyn_FnType_struct*)_tmp64)->f1;_tmp80=_tmp7F.tvars;_tmp81=_tmp7F.effect;
_tmp82=(void*)_tmp7F.ret_typ;_tmp83=_tmp7F.args;_tmp84=_tmp7F.c_varargs;_tmp85=
_tmp7F.cyc_varargs;_tmp86=_tmp7F.rgn_po;_tmp87=_tmp7F.attributes;_LL2F: {struct
Cyc_List_List*_tmpBA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp80);struct Cyc_Core_Opt*_tmpBB=_tmp81 == 0?0:({struct Cyc_Core_Opt*_tmpC5=
_cycalloc(sizeof(*_tmpC5));_tmpC5->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp81->v);
_tmpC5;});void*_tmpBC=Cyc_Tcutil_copy_type(_tmp82);struct Cyc_List_List*_tmpBD=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp83);int _tmpBE=_tmp84;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp85 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp85;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpBF=_cycalloc(sizeof(*_tmpBF));
_tmpBF->name=cv->name;_tmpBF->tq=cv->tq;_tmpBF->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpBF->inject=cv->inject;_tmpBF;});}{struct Cyc_List_List*_tmpC0=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp86);struct Cyc_List_List*_tmpC1=_tmp87;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));
_tmpC2[0]=({struct Cyc_Absyn_FnType_struct _tmpC3;_tmpC3.tag=8;_tmpC3.f1=({struct
Cyc_Absyn_FnInfo _tmpC4;_tmpC4.tvars=_tmpBA;_tmpC4.effect=_tmpBB;_tmpC4.ret_typ=(
void*)_tmpBC;_tmpC4.args=_tmpBD;_tmpC4.c_varargs=_tmpBE;_tmpC4.cyc_varargs=
cyc_varargs2;_tmpC4.rgn_po=_tmpC0;_tmpC4.attributes=_tmpC1;_tmpC4;});_tmpC3;});
_tmpC2;});}}_LL30: if(*((int*)_tmp64)!= 9)goto _LL32;_tmp88=((struct Cyc_Absyn_TupleType_struct*)
_tmp64)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_TupleType_struct _tmpC7;
_tmpC7.tag=9;_tmpC7.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp88);
_tmpC7;});_tmpC6;});_LL32: if(*((int*)_tmp64)!= 10)goto _LL34;_tmp89=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8A=(void*)_tmp89.aggr_info;if(*((int*)_tmp8A)!= 0)goto _LL34;
_tmp8B=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f1;_tmp8C=((struct
Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f2;_tmp8D=_tmp89.targs;_LL33: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({
struct Cyc_Absyn_AggrType_struct _tmpC9;_tmpC9.tag=10;_tmpC9.f1=({struct Cyc_Absyn_AggrInfo
_tmpCA;_tmpCA.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmpCC;_tmpCC.tag=0;_tmpCC.f1=(void*)_tmp8B;_tmpCC.f2=_tmp8C;_tmpCC;});_tmpCB;}));
_tmpCA.targs=Cyc_Tcutil_copy_types(_tmp8D);_tmpCA;});_tmpC9;});_tmpC8;});_LL34:
if(*((int*)_tmp64)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8F=(void*)_tmp8E.aggr_info;if(*((int*)_tmp8F)!= 1)goto _LL36;
_tmp90=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8F)->f1;_tmp91=_tmp8E.targs;_LL35:
return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));
_tmpCD[0]=({struct Cyc_Absyn_AggrType_struct _tmpCE;_tmpCE.tag=10;_tmpCE.f1=({
struct Cyc_Absyn_AggrInfo _tmpCF;_tmpCF.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmpD1;_tmpD1.tag=1;_tmpD1.f1=_tmp90;_tmpD1;});_tmpD0;}));_tmpCF.targs=Cyc_Tcutil_copy_types(
_tmp91);_tmpCF;});_tmpCE;});_tmpCD;});_LL36: if(*((int*)_tmp64)!= 11)goto _LL38;
_tmp92=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f1;_tmp93=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmpD3;_tmpD3.tag=11;_tmpD3.f1=(void*)_tmp92;_tmpD3.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp93);_tmpD3;});_tmpD2;});_LL38: if(*((int*)
_tmp64)!= 12)goto _LL3A;_tmp94=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;
_tmp95=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({
struct Cyc_Absyn_EnumType_struct _tmpD5;_tmpD5.tag=12;_tmpD5.f1=_tmp94;_tmpD5.f2=
_tmp95;_tmpD5;});_tmpD4;});_LL3A: if(*((int*)_tmp64)!= 13)goto _LL3C;_tmp96=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmpD7;_tmpD7.tag=13;_tmpD7.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*
f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp96);_tmpD7;});_tmpD6;});_LL3C: if(*((int*)_tmp64)!= 14)goto _LL3E;_tmp97=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D: return(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_SizeofType_struct
_tmpD9;_tmpD9.tag=14;_tmpD9.f1=(void*)Cyc_Tcutil_copy_type(_tmp97);_tmpD9;});
_tmpD8;});_LL3E: if(*((int*)_tmp64)!= 18)goto _LL40;_tmp98=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpDA=
_cycalloc_atomic(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpDB;_tmpDB.tag=18;_tmpDB.f1=_tmp98;_tmpDB;});_tmpDA;});_LL40: if(*((int*)
_tmp64)!= 17)goto _LL42;_tmp99=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp64)->f1;
_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDC=_cycalloc(sizeof(*
_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_TagType_struct _tmpDD;_tmpDD.tag=17;_tmpDD.f1=(
void*)Cyc_Tcutil_copy_type(_tmp99);_tmpDD;});_tmpDC;});_LL42: if(*((int*)_tmp64)
!= 15)goto _LL44;_tmp9A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp64)->f1;
_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDE=_cycalloc(
sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpDF;_tmpDF.tag=
15;_tmpDF.f1=(void*)Cyc_Tcutil_copy_type(_tmp9A);_tmpDF;});_tmpDE;});_LL44: if(*((
int*)_tmp64)!= 16)goto _LL46;_tmp9B=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;
_tmp9C=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f2;_tmp9D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f3;_LL45: return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpE0=
_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_TypedefType_struct _tmpE1;
_tmpE1.tag=16;_tmpE1.f1=_tmp9B;_tmpE1.f2=Cyc_Tcutil_copy_types(_tmp9C);_tmpE1.f3=
_tmp9D;_tmpE1.f4=0;_tmpE1;});_tmpE0;});_LL46: if((int)_tmp64 != 3)goto _LL48;_LL47:
goto _LL49;_LL48: if((int)_tmp64 != 2)goto _LL4A;_LL49: return t;_LL4A: if(_tmp64 <= (
void*)4)goto _LL4C;if(*((int*)_tmp64)!= 19)goto _LL4C;_tmp9E=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp64)->f1;_LL4B: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE2=
_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE3;
_tmpE3.tag=19;_tmpE3.f1=(void*)Cyc_Tcutil_copy_type(_tmp9E);_tmpE3;});_tmpE2;});
_LL4C: if(_tmp64 <= (void*)4)goto _LL4E;if(*((int*)_tmp64)!= 20)goto _LL4E;_tmp9F=((
struct Cyc_Absyn_JoinEff_struct*)_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpE5;_tmpE5.tag=20;_tmpE5.f1=Cyc_Tcutil_copy_types(_tmp9F);_tmpE5;});_tmpE4;});
_LL4E: if(_tmp64 <= (void*)4)goto _LL19;if(*((int*)_tmp64)!= 21)goto _LL19;_tmpA0=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp64)->f1;_LL4F: return(void*)({struct
Cyc_Absyn_RgnsEff_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmpE7;_tmpE7.tag=21;_tmpE7.f1=(void*)Cyc_Tcutil_copy_type(_tmpA0);_tmpE7;});
_tmpE6;});_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{
struct _tuple6 _tmpE9=({struct _tuple6 _tmpE8;_tmpE8.f1=k1;_tmpE8.f2=k2;_tmpE8;});
void*_tmpEA;void*_tmpEB;void*_tmpEC;void*_tmpED;void*_tmpEE;void*_tmpEF;_LL51:
_tmpEA=_tmpE9.f1;if((int)_tmpEA != 2)goto _LL53;_tmpEB=_tmpE9.f2;if((int)_tmpEB != 
1)goto _LL53;_LL52: goto _LL54;_LL53: _tmpEC=_tmpE9.f1;if((int)_tmpEC != 2)goto _LL55;
_tmpED=_tmpE9.f2;if((int)_tmpED != 0)goto _LL55;_LL54: goto _LL56;_LL55: _tmpEE=
_tmpE9.f1;if((int)_tmpEE != 1)goto _LL57;_tmpEF=_tmpE9.f2;if((int)_tmpEF != 0)goto
_LL57;_LL56: return 1;_LL57:;_LL58: return 0;_LL50:;}}void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv){void*_tmpF0=Cyc_Absyn_compress_kb((void*)tv->kind);void*
_tmpF1;void*_tmpF2;_LL5A: if(*((int*)_tmpF0)!= 0)goto _LL5C;_tmpF1=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmpF0)->f1;_LL5B: return _tmpF1;_LL5C: if(*((int*)_tmpF0)
!= 2)goto _LL5E;_tmpF2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpF0)->f2;_LL5D:
return _tmpF2;_LL5E:;_LL5F:({void*_tmpF3[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF4="kind not suitably constrained!";
_tag_dynforward(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,31));}),
_tag_dynforward(_tmpF3,sizeof(void*),0));});_LL59:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpF5=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpF6;struct Cyc_Core_Opt*
_tmpF7;struct Cyc_Absyn_Tvar*_tmpF8;void*_tmpF9;struct Cyc_Absyn_TunionFieldInfo
_tmpFA;void*_tmpFB;struct Cyc_Absyn_Tunionfield*_tmpFC;struct Cyc_Absyn_TunionFieldInfo
_tmpFD;void*_tmpFE;struct Cyc_Absyn_Enumdecl*_tmpFF;struct Cyc_Absyn_AggrInfo
_tmp100;void*_tmp101;struct Cyc_Absyn_AggrInfo _tmp102;void*_tmp103;struct Cyc_Absyn_Aggrdecl**
_tmp104;struct Cyc_Absyn_Aggrdecl*_tmp105;struct Cyc_Absyn_Aggrdecl _tmp106;struct
Cyc_Absyn_AggrdeclImpl*_tmp107;struct Cyc_Absyn_Enumdecl*_tmp108;struct Cyc_Absyn_PtrInfo
_tmp109;struct Cyc_Absyn_Typedefdecl*_tmp10A;_LL61: if(_tmpF5 <= (void*)4)goto _LL65;
if(*((int*)_tmpF5)!= 0)goto _LL63;_tmpF6=((struct Cyc_Absyn_Evar_struct*)_tmpF5)->f1;
_tmpF7=((struct Cyc_Absyn_Evar_struct*)_tmpF5)->f2;_LL62: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF6))->v;_LL63: if(*((int*)_tmpF5)!= 1)goto _LL65;_tmpF8=((struct
Cyc_Absyn_VarType_struct*)_tmpF5)->f1;_LL64: return Cyc_Tcutil_tvar_kind(_tmpF8);
_LL65: if((int)_tmpF5 != 0)goto _LL67;_LL66: return(void*)1;_LL67: if(_tmpF5 <= (void*)
4)goto _LL69;if(*((int*)_tmpF5)!= 5)goto _LL69;_tmpF9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpF5)->f2;_LL68: return(_tmpF9 == (void*)((void*)2) || _tmpF9 == (void*)((void*)3))?(
void*)2:(void*)1;_LL69: if((int)_tmpF5 != 1)goto _LL6B;_LL6A: goto _LL6C;_LL6B: if(
_tmpF5 <= (void*)4)goto _LL71;if(*((int*)_tmpF5)!= 6)goto _LL6D;_LL6C: goto _LL6E;
_LL6D: if(*((int*)_tmpF5)!= 8)goto _LL6F;_LL6E: return(void*)1;_LL6F: if(*((int*)
_tmpF5)!= 15)goto _LL71;_LL70: return(void*)2;_LL71: if((int)_tmpF5 != 3)goto _LL73;
_LL72: goto _LL74;_LL73: if((int)_tmpF5 != 2)goto _LL75;_LL74: return(void*)3;_LL75:
if(_tmpF5 <= (void*)4)goto _LL77;if(*((int*)_tmpF5)!= 2)goto _LL77;_LL76: return(
void*)2;_LL77: if(_tmpF5 <= (void*)4)goto _LL79;if(*((int*)_tmpF5)!= 3)goto _LL79;
_tmpFA=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF5)->f1;_tmpFB=(void*)
_tmpFA.field_info;if(*((int*)_tmpFB)!= 1)goto _LL79;_tmpFC=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpFB)->f2;_LL78: if(_tmpFC->typs == 0)return(void*)2;else{return(void*)1;}_LL79:
if(_tmpF5 <= (void*)4)goto _LL7B;if(*((int*)_tmpF5)!= 3)goto _LL7B;_tmpFD=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmpF5)->f1;_tmpFE=(void*)_tmpFD.field_info;
if(*((int*)_tmpFE)!= 0)goto _LL7B;_LL7A:({void*_tmp10B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp10C="typ_kind: Unresolved TunionFieldType";_tag_dynforward(_tmp10C,sizeof(
char),_get_zero_arr_size(_tmp10C,37));}),_tag_dynforward(_tmp10B,sizeof(void*),0));});
_LL7B: if(_tmpF5 <= (void*)4)goto _LL7D;if(*((int*)_tmpF5)!= 12)goto _LL7D;_tmpFF=((
struct Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;if(_tmpFF != 0)goto _LL7D;_LL7C: goto
_LL7E;_LL7D: if(_tmpF5 <= (void*)4)goto _LL7F;if(*((int*)_tmpF5)!= 10)goto _LL7F;
_tmp100=((struct Cyc_Absyn_AggrType_struct*)_tmpF5)->f1;_tmp101=(void*)_tmp100.aggr_info;
if(*((int*)_tmp101)!= 0)goto _LL7F;_LL7E: return(void*)0;_LL7F: if(_tmpF5 <= (void*)
4)goto _LL81;if(*((int*)_tmpF5)!= 10)goto _LL81;_tmp102=((struct Cyc_Absyn_AggrType_struct*)
_tmpF5)->f1;_tmp103=(void*)_tmp102.aggr_info;if(*((int*)_tmp103)!= 1)goto _LL81;
_tmp104=((struct Cyc_Absyn_KnownAggr_struct*)_tmp103)->f1;_tmp105=*_tmp104;
_tmp106=*_tmp105;_tmp107=_tmp106.impl;_LL80: return _tmp107 == 0?(void*)0:(void*)1;
_LL81: if(_tmpF5 <= (void*)4)goto _LL83;if(*((int*)_tmpF5)!= 11)goto _LL83;_LL82:
goto _LL84;_LL83: if(_tmpF5 <= (void*)4)goto _LL85;if(*((int*)_tmpF5)!= 13)goto _LL85;
_LL84: return(void*)1;_LL85: if(_tmpF5 <= (void*)4)goto _LL87;if(*((int*)_tmpF5)!= 
12)goto _LL87;_tmp108=((struct Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;_LL86: if(
_tmp108->fields == 0)return(void*)0;else{return(void*)1;}_LL87: if(_tmpF5 <= (void*)
4)goto _LL89;if(*((int*)_tmpF5)!= 4)goto _LL89;_tmp109=((struct Cyc_Absyn_PointerType_struct*)
_tmpF5)->f1;_LL88: {void*_tmp10D=(void*)(Cyc_Absyn_compress_conref((_tmp109.ptr_atts).bounds))->v;
void*_tmp10E;void*_tmp10F;void*_tmp110;void*_tmp111;_LL9C: if((int)_tmp10D != 0)
goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(_tmp10D <= (void*)1)goto _LLA0;if(*((int*)
_tmp10D)!= 0)goto _LLA0;_tmp10E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10D)->f1;if((int)_tmp10E != 0)goto _LLA0;_LL9F: return(void*)1;_LLA0: if(_tmp10D
<= (void*)1)goto _LLA2;if(*((int*)_tmp10D)!= 0)goto _LLA2;_tmp10F=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if((int)_tmp10F != 1)goto _LLA2;_LLA1:
return(void*)1;_LLA2: if(_tmp10D <= (void*)1)goto _LLA4;if(*((int*)_tmp10D)!= 0)
goto _LLA4;_tmp110=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if(
_tmp110 <= (void*)2)goto _LLA4;if(*((int*)_tmp110)!= 0)goto _LLA4;_LLA3: goto _LLA5;
_LLA4: if(_tmp10D <= (void*)1)goto _LLA6;if(*((int*)_tmp10D)!= 0)goto _LLA6;_tmp111=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if(_tmp111 <= (void*)2)
goto _LLA6;if(*((int*)_tmp111)!= 1)goto _LLA6;_LLA5: return(void*)2;_LLA6: if(
_tmp10D <= (void*)1)goto _LL9B;if(*((int*)_tmp10D)!= 1)goto _LL9B;_LLA7:({void*
_tmp112[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp113="typ_kind: forward constr in ptr bounds";_tag_dynforward(
_tmp113,sizeof(char),_get_zero_arr_size(_tmp113,39));}),_tag_dynforward(_tmp112,
sizeof(void*),0));});_LL9B:;}_LL89: if(_tmpF5 <= (void*)4)goto _LL8B;if(*((int*)
_tmpF5)!= 14)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpF5 <= (void*)4)goto _LL8D;
if(*((int*)_tmpF5)!= 18)goto _LL8D;_LL8C: return(void*)5;_LL8D: if(_tmpF5 <= (void*)
4)goto _LL8F;if(*((int*)_tmpF5)!= 17)goto _LL8F;_LL8E: return(void*)2;_LL8F: if(
_tmpF5 <= (void*)4)goto _LL91;if(*((int*)_tmpF5)!= 7)goto _LL91;_LL90: goto _LL92;
_LL91: if(_tmpF5 <= (void*)4)goto _LL93;if(*((int*)_tmpF5)!= 9)goto _LL93;_LL92:
return(void*)1;_LL93: if(_tmpF5 <= (void*)4)goto _LL95;if(*((int*)_tmpF5)!= 16)goto
_LL95;_tmp10A=((struct Cyc_Absyn_TypedefType_struct*)_tmpF5)->f3;_LL94: if(_tmp10A
== 0  || _tmp10A->kind == 0)({struct Cyc_String_pa_struct _tmp116;_tmp116.tag=0;
_tmp116.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp114[1]={& _tmp116};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp115="typ_kind: typedef found: %s";
_tag_dynforward(_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,28));}),
_tag_dynforward(_tmp114,sizeof(void*),1));}});return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp10A->kind))->v;_LL95: if(_tmpF5 <= (void*)4)goto _LL97;if(*((int*)
_tmpF5)!= 19)goto _LL97;_LL96: goto _LL98;_LL97: if(_tmpF5 <= (void*)4)goto _LL99;if(*((
int*)_tmpF5)!= 20)goto _LL99;_LL98: goto _LL9A;_LL99: if(_tmpF5 <= (void*)4)goto _LL60;
if(*((int*)_tmpF5)!= 21)goto _LL60;_LL9A: return(void*)4;_LL60:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp117;_push_handler(& _tmp117);{int _tmp119=
0;if(setjmp(_tmp117.handler))_tmp119=1;if(!_tmp119){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp11A=1;_npop_handler(0);return _tmp11A;};_pop_handler();}else{void*_tmp118=(
void*)_exn_thrown;void*_tmp11C=_tmp118;_LLA9: if(_tmp11C != Cyc_Tcutil_Unify)goto
_LLAB;_LLAA: return 0;_LLAB:;_LLAC:(void)_throw(_tmp11C);_LLA8:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp11D=t;struct Cyc_Absyn_Tvar*
_tmp11E;struct Cyc_Core_Opt*_tmp11F;struct Cyc_Core_Opt*_tmp120;struct Cyc_Core_Opt**
_tmp121;struct Cyc_Absyn_PtrInfo _tmp122;struct Cyc_Absyn_ArrayInfo _tmp123;void*
_tmp124;struct Cyc_Absyn_FnInfo _tmp125;struct Cyc_List_List*_tmp126;struct Cyc_Core_Opt*
_tmp127;void*_tmp128;struct Cyc_List_List*_tmp129;int _tmp12A;struct Cyc_Absyn_VarargInfo*
_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*
_tmp12E;struct Cyc_Absyn_TunionInfo _tmp12F;struct Cyc_List_List*_tmp130;void*
_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_TunionFieldInfo _tmp133;
struct Cyc_List_List*_tmp134;struct Cyc_Absyn_AggrInfo _tmp135;struct Cyc_List_List*
_tmp136;struct Cyc_List_List*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;void*
_tmp13B;void*_tmp13C;struct Cyc_List_List*_tmp13D;_LLAE: if(_tmp11D <= (void*)4)
goto _LLD0;if(*((int*)_tmp11D)!= 1)goto _LLB0;_tmp11E=((struct Cyc_Absyn_VarType_struct*)
_tmp11D)->f1;_LLAF: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp11E)){Cyc_Tcutil_failure_reason=({const char*_tmp13E="(type variable would escape scope)";
_tag_dynforward(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,35));});(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LLAD;_LLB0: if(*((int*)_tmp11D)!= 0)goto
_LLB2;_tmp11F=((struct Cyc_Absyn_Evar_struct*)_tmp11D)->f2;_tmp120=((struct Cyc_Absyn_Evar_struct*)
_tmp11D)->f4;_tmp121=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp11D)->f4;_LLB1: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp13F="(occurs check)";
_tag_dynforward(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,15));});(int)
_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmp11F != 0)Cyc_Tcutil_occurs(evar,r,
env,(void*)_tmp11F->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp121))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp140=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp121))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp140=({struct Cyc_List_List*_tmp141=
_cycalloc(sizeof(*_tmp141));_tmp141->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp141->tl=
_tmp140;_tmp141;});}}*_tmp121=({struct Cyc_Core_Opt*_tmp142=_cycalloc(sizeof(*
_tmp142));_tmp142->v=_tmp140;_tmp142;});}}}goto _LLAD;_LLB2: if(*((int*)_tmp11D)!= 
4)goto _LLB4;_tmp122=((struct Cyc_Absyn_PointerType_struct*)_tmp11D)->f1;_LLB3: Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp122.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)(_tmp122.ptr_atts).rgn);{
void*_tmp143=(void*)(Cyc_Absyn_compress_conref((_tmp122.ptr_atts).bounds))->v;
void*_tmp144;void*_tmp145;_LLD3: if(_tmp143 <= (void*)1)goto _LLD5;if(*((int*)
_tmp143)!= 0)goto _LLD5;_tmp144=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp143)->f1;if(_tmp144 <= (void*)2)goto _LLD5;if(*((int*)_tmp144)!= 1)goto _LLD5;
_tmp145=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp144)->f1;_LLD4: Cyc_Tcutil_occurs(
evar,r,env,_tmp145);goto _LLD2;_LLD5:;_LLD6: goto _LLD2;_LLD2:;}goto _LLAD;_LLB4: if(*((
int*)_tmp11D)!= 7)goto _LLB6;_tmp123=((struct Cyc_Absyn_ArrayType_struct*)_tmp11D)->f1;
_tmp124=(void*)_tmp123.elt_type;_LLB5: Cyc_Tcutil_occurs(evar,r,env,_tmp124);goto
_LLAD;_LLB6: if(*((int*)_tmp11D)!= 8)goto _LLB8;_tmp125=((struct Cyc_Absyn_FnType_struct*)
_tmp11D)->f1;_tmp126=_tmp125.tvars;_tmp127=_tmp125.effect;_tmp128=(void*)_tmp125.ret_typ;
_tmp129=_tmp125.args;_tmp12A=_tmp125.c_varargs;_tmp12B=_tmp125.cyc_varargs;
_tmp12C=_tmp125.rgn_po;_tmp12D=_tmp125.attributes;_LLB7: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp126,env);if(_tmp127 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp127->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp128);for(0;_tmp129 != 0;_tmp129=_tmp129->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp129->hd)).f3);}if(_tmp12B != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12B->type);for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){struct
_tuple6 _tmp147;void*_tmp148;void*_tmp149;struct _tuple6*_tmp146=(struct _tuple6*)
_tmp12C->hd;_tmp147=*_tmp146;_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp148);Cyc_Tcutil_occurs(evar,r,env,_tmp149);}goto _LLAD;_LLB8: if(*((
int*)_tmp11D)!= 9)goto _LLBA;_tmp12E=((struct Cyc_Absyn_TupleType_struct*)_tmp11D)->f1;
_LLB9: for(0;_tmp12E != 0;_tmp12E=_tmp12E->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple4*)_tmp12E->hd)).f2);}goto _LLAD;_LLBA: if(*((int*)_tmp11D)!= 2)goto
_LLBC;_tmp12F=((struct Cyc_Absyn_TunionType_struct*)_tmp11D)->f1;_tmp130=_tmp12F.targs;
_tmp131=(void*)_tmp12F.rgn;_LLBB: Cyc_Tcutil_occurs(evar,r,env,_tmp131);Cyc_Tcutil_occurslist(
evar,r,env,_tmp130);goto _LLAD;_LLBC: if(*((int*)_tmp11D)!= 16)goto _LLBE;_tmp132=((
struct Cyc_Absyn_TypedefType_struct*)_tmp11D)->f2;_LLBD: _tmp134=_tmp132;goto _LLBF;
_LLBE: if(*((int*)_tmp11D)!= 3)goto _LLC0;_tmp133=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11D)->f1;_tmp134=_tmp133.targs;_LLBF: _tmp136=_tmp134;goto _LLC1;_LLC0: if(*((
int*)_tmp11D)!= 10)goto _LLC2;_tmp135=((struct Cyc_Absyn_AggrType_struct*)_tmp11D)->f1;
_tmp136=_tmp135.targs;_LLC1: Cyc_Tcutil_occurslist(evar,r,env,_tmp136);goto _LLAD;
_LLC2: if(*((int*)_tmp11D)!= 11)goto _LLC4;_tmp137=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11D)->f2;_LLC3: for(0;_tmp137 != 0;_tmp137=_tmp137->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp137->hd)->type);}goto _LLAD;_LLC4:
if(*((int*)_tmp11D)!= 17)goto _LLC6;_tmp138=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp11D)->f1;_LLC5: _tmp139=_tmp138;goto _LLC7;_LLC6: if(*((int*)_tmp11D)!= 14)goto
_LLC8;_tmp139=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11D)->f1;_LLC7:
_tmp13A=_tmp139;goto _LLC9;_LLC8: if(*((int*)_tmp11D)!= 19)goto _LLCA;_tmp13A=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp11D)->f1;_LLC9: _tmp13B=_tmp13A;goto _LLCB;
_LLCA: if(*((int*)_tmp11D)!= 15)goto _LLCC;_tmp13B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp11D)->f1;_LLCB: _tmp13C=_tmp13B;goto _LLCD;_LLCC: if(*((int*)_tmp11D)!= 21)goto
_LLCE;_tmp13C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11D)->f1;_LLCD: Cyc_Tcutil_occurs(
evar,r,env,_tmp13C);goto _LLAD;_LLCE: if(*((int*)_tmp11D)!= 20)goto _LLD0;_tmp13D=((
struct Cyc_Absyn_JoinEff_struct*)_tmp11D)->f1;_LLCF: Cyc_Tcutil_occurslist(evar,r,
env,_tmp13D);goto _LLAD;_LLD0:;_LLD1: goto _LLAD;_LLAD:;}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp14A[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp14B="tq1 real_const not set.";
_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,24));}),
_tag_dynforward(_tmp14A,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp14C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp14D="tq2 real_const not set.";_tag_dynforward(
_tmp14D,sizeof(char),_get_zero_arr_size(_tmp14D,24));}),_tag_dynforward(_tmp14C,
sizeof(void*),0));});if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({
const char*_tmp14E="(qualifiers don't match)";_tag_dynforward(_tmp14E,sizeof(char),
_get_zero_arr_size(_tmp14E,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dynforward_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{void*_tmp14F=(void*)x->v;void*_tmp150;_LLD8: if((int)_tmp14F
!= 0)goto _LLDA;_LLD9:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp152;_tmp152.tag=1;_tmp152.f1=y;_tmp152;});_tmp151;})));return;_LLDA: if(
_tmp14F <= (void*)1)goto _LLDC;if(*((int*)_tmp14F)!= 0)goto _LLDC;_tmp150=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp14F)->f1;_LLDB: {void*_tmp153=(void*)y->v;
void*_tmp154;_LLDF: if((int)_tmp153 != 0)goto _LLE1;_LLE0:(void*)(y->v=(void*)((
void*)x->v));return;_LLE1: if(_tmp153 <= (void*)1)goto _LLE3;if(*((int*)_tmp153)!= 
0)goto _LLE3;_tmp154=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp153)->f1;
_LLE2: if(cmp(_tmp150,_tmp154)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLE3: if(_tmp153 <= (void*)1)goto _LLDE;if(*((int*)
_tmp153)!= 1)goto _LLDE;_LLE4:({void*_tmp155[0]={};Cyc_Tcutil_impos(({const char*
_tmp156="unify_conref: forward after compress(2)";_tag_dynforward(_tmp156,
sizeof(char),_get_zero_arr_size(_tmp156,40));}),_tag_dynforward(_tmp155,sizeof(
void*),0));});_LLDE:;}_LLDC: if(_tmp14F <= (void*)1)goto _LLD7;if(*((int*)_tmp14F)
!= 1)goto _LLD7;_LLDD:({void*_tmp157[0]={};Cyc_Tcutil_impos(({const char*_tmp158="unify_conref: forward after compress";
_tag_dynforward(_tmp158,sizeof(char),_get_zero_arr_size(_tmp158,37));}),
_tag_dynforward(_tmp157,sizeof(void*),0));});_LLD7:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp159;_push_handler(& _tmp159);{int _tmp15B=0;if(setjmp(_tmp159.handler))
_tmp15B=1;if(!_tmp15B){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct
_dynforward_ptr)_tag_dynforward(0,0,0));{int _tmp15C=1;_npop_handler(0);return
_tmp15C;};_pop_handler();}else{void*_tmp15A=(void*)_exn_thrown;void*_tmp15E=
_tmp15A;_LLE6: if(_tmp15E != Cyc_Tcutil_Unify)goto _LLE8;_LLE7: return 0;_LLE8:;_LLE9:(
void)_throw(_tmp15E);_LLE5:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple6 _tmp160=({struct _tuple6 _tmp15F;_tmp15F.f1=b1;_tmp15F.f2=b2;_tmp15F;});
void*_tmp161;void*_tmp162;void*_tmp163;void*_tmp164;void*_tmp165;void*_tmp166;
void*_tmp167;void*_tmp168;void*_tmp169;struct Cyc_Absyn_Exp*_tmp16A;void*_tmp16B;
struct Cyc_Absyn_Exp*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;void*_tmp170;
void*_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;_LLEB: _tmp161=_tmp160.f1;if((
int)_tmp161 != 0)goto _LLED;_tmp162=_tmp160.f2;if((int)_tmp162 != 0)goto _LLED;_LLEC:
return 0;_LLED: _tmp163=_tmp160.f1;if((int)_tmp163 != 0)goto _LLEF;_LLEE: return - 1;
_LLEF: _tmp164=_tmp160.f2;if((int)_tmp164 != 0)goto _LLF1;_LLF0: return 1;_LLF1:
_tmp165=_tmp160.f1;if((int)_tmp165 != 1)goto _LLF3;_tmp166=_tmp160.f2;if((int)
_tmp166 != 1)goto _LLF3;_LLF2: return 0;_LLF3: _tmp167=_tmp160.f1;if((int)_tmp167 != 1)
goto _LLF5;_LLF4: return - 1;_LLF5: _tmp168=_tmp160.f2;if((int)_tmp168 != 1)goto _LLF7;
_LLF6: return 1;_LLF7: _tmp169=_tmp160.f1;if(_tmp169 <= (void*)2)goto _LLF9;if(*((int*)
_tmp169)!= 0)goto _LLF9;_tmp16A=((struct Cyc_Absyn_Upper_b_struct*)_tmp169)->f1;
_tmp16B=_tmp160.f2;if(_tmp16B <= (void*)2)goto _LLF9;if(*((int*)_tmp16B)!= 0)goto
_LLF9;_tmp16C=((struct Cyc_Absyn_Upper_b_struct*)_tmp16B)->f1;_LLF8: return Cyc_Evexp_const_exp_cmp(
_tmp16A,_tmp16C);_LLF9: _tmp16D=_tmp160.f1;if(_tmp16D <= (void*)2)goto _LLFB;if(*((
int*)_tmp16D)!= 0)goto _LLFB;_tmp16E=_tmp160.f2;if(_tmp16E <= (void*)2)goto _LLFB;
if(*((int*)_tmp16E)!= 1)goto _LLFB;_LLFA: return - 1;_LLFB: _tmp16F=_tmp160.f1;if(
_tmp16F <= (void*)2)goto _LLFD;if(*((int*)_tmp16F)!= 1)goto _LLFD;_tmp170=_tmp160.f2;
if(_tmp170 <= (void*)2)goto _LLFD;if(*((int*)_tmp170)!= 0)goto _LLFD;_LLFC: return 1;
_LLFD: _tmp171=_tmp160.f1;if(_tmp171 <= (void*)2)goto _LLEA;if(*((int*)_tmp171)!= 1)
goto _LLEA;_tmp172=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp171)->f1;
_tmp173=_tmp160.f2;if(_tmp173 <= (void*)2)goto _LLEA;if(*((int*)_tmp173)!= 1)goto
_LLEA;_tmp174=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp173)->f1;_LLFE:
return Cyc_Tcutil_typecmp(_tmp172,_tmp174);_LLEA:;}static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple6 _tmp176=({struct _tuple6 _tmp175;_tmp175.f1=b1;
_tmp175.f2=b2;_tmp175;});void*_tmp177;void*_tmp178;void*_tmp179;void*_tmp17A;
void*_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;struct Cyc_Absyn_Exp*
_tmp180;void*_tmp181;struct Cyc_Absyn_Exp*_tmp182;void*_tmp183;struct Cyc_Absyn_Exp*
_tmp184;void*_tmp185;void*_tmp186;void*_tmp187;void*_tmp188;void*_tmp189;struct
Cyc_Absyn_Exp*_tmp18A;void*_tmp18B;void*_tmp18C;void*_tmp18D;void*_tmp18E;_LL100:
_tmp177=_tmp176.f1;if((int)_tmp177 != 0)goto _LL102;_tmp178=_tmp176.f2;if((int)
_tmp178 != 0)goto _LL102;_LL101: return 0;_LL102: _tmp179=_tmp176.f1;if((int)_tmp179
!= 0)goto _LL104;_LL103: return - 1;_LL104: _tmp17A=_tmp176.f2;if((int)_tmp17A != 0)
goto _LL106;_LL105: return 1;_LL106: _tmp17B=_tmp176.f1;if((int)_tmp17B != 1)goto
_LL108;_tmp17C=_tmp176.f2;if((int)_tmp17C != 1)goto _LL108;_LL107: return 0;_LL108:
_tmp17D=_tmp176.f1;if((int)_tmp17D != 1)goto _LL10A;_LL109: return - 1;_LL10A: _tmp17E=
_tmp176.f2;if((int)_tmp17E != 1)goto _LL10C;_LL10B: return 1;_LL10C: _tmp17F=_tmp176.f1;
if(_tmp17F <= (void*)2)goto _LL10E;if(*((int*)_tmp17F)!= 0)goto _LL10E;_tmp180=((
struct Cyc_Absyn_Upper_b_struct*)_tmp17F)->f1;_tmp181=_tmp176.f2;if(_tmp181 <= (
void*)2)goto _LL10E;if(*((int*)_tmp181)!= 0)goto _LL10E;_tmp182=((struct Cyc_Absyn_Upper_b_struct*)
_tmp181)->f1;_LL10D: return Cyc_Evexp_const_exp_cmp(_tmp180,_tmp182);_LL10E:
_tmp183=_tmp176.f1;if(_tmp183 <= (void*)2)goto _LL110;if(*((int*)_tmp183)!= 0)goto
_LL110;_tmp184=((struct Cyc_Absyn_Upper_b_struct*)_tmp183)->f1;_tmp185=_tmp176.f2;
if(_tmp185 <= (void*)2)goto _LL110;if(*((int*)_tmp185)!= 1)goto _LL110;_tmp186=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp185)->f1;_LL10F: _tmp188=_tmp186;
_tmp18A=_tmp184;goto _LL111;_LL110: _tmp187=_tmp176.f1;if(_tmp187 <= (void*)2)goto
_LL112;if(*((int*)_tmp187)!= 1)goto _LL112;_tmp188=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp187)->f1;_tmp189=_tmp176.f2;if(_tmp189 <= (void*)2)goto _LL112;if(*((int*)
_tmp189)!= 0)goto _LL112;_tmp18A=((struct Cyc_Absyn_Upper_b_struct*)_tmp189)->f1;
_LL111: {unsigned int _tmp190;int _tmp191;struct _tuple7 _tmp18F=Cyc_Evexp_eval_const_uint_exp(
_tmp18A);_tmp190=_tmp18F.f1;_tmp191=_tmp18F.f2;if(!_tmp191)return 1;_tmp18C=
_tmp188;_tmp18E=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp192=_cycalloc_atomic(
sizeof(*_tmp192));_tmp192[0]=({struct Cyc_Absyn_TypeInt_struct _tmp193;_tmp193.tag=
18;_tmp193.f1=(int)_tmp190;_tmp193;});_tmp192;});goto _LL113;}_LL112: _tmp18B=
_tmp176.f1;if(_tmp18B <= (void*)2)goto _LLFF;if(*((int*)_tmp18B)!= 1)goto _LLFF;
_tmp18C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18B)->f1;_tmp18D=_tmp176.f2;
if(_tmp18D <= (void*)2)goto _LLFF;if(*((int*)_tmp18D)!= 1)goto _LLFF;_tmp18E=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18D)->f1;_LL113: Cyc_Tcutil_unify_it(
_tmp18C,_tmp18E);return 0;_LLFF:;}static int Cyc_Tcutil_attribute_case_number(void*
att){void*_tmp194=att;_LL115: if(_tmp194 <= (void*)17)goto _LL117;if(*((int*)
_tmp194)!= 0)goto _LL117;_LL116: return 0;_LL117: if((int)_tmp194 != 0)goto _LL119;
_LL118: return 1;_LL119: if((int)_tmp194 != 1)goto _LL11B;_LL11A: return 2;_LL11B: if((
int)_tmp194 != 2)goto _LL11D;_LL11C: return 3;_LL11D: if((int)_tmp194 != 3)goto _LL11F;
_LL11E: return 4;_LL11F: if((int)_tmp194 != 4)goto _LL121;_LL120: return 5;_LL121: if(
_tmp194 <= (void*)17)goto _LL123;if(*((int*)_tmp194)!= 1)goto _LL123;_LL122: return 6;
_LL123: if((int)_tmp194 != 5)goto _LL125;_LL124: return 7;_LL125: if(_tmp194 <= (void*)
17)goto _LL127;if(*((int*)_tmp194)!= 2)goto _LL127;_LL126: return 8;_LL127: if((int)
_tmp194 != 6)goto _LL129;_LL128: return 9;_LL129: if((int)_tmp194 != 7)goto _LL12B;
_LL12A: return 10;_LL12B: if((int)_tmp194 != 8)goto _LL12D;_LL12C: return 11;_LL12D: if((
int)_tmp194 != 9)goto _LL12F;_LL12E: return 12;_LL12F: if((int)_tmp194 != 10)goto
_LL131;_LL130: return 13;_LL131: if((int)_tmp194 != 11)goto _LL133;_LL132: return 14;
_LL133: if((int)_tmp194 != 12)goto _LL135;_LL134: return 15;_LL135: if((int)_tmp194 != 
13)goto _LL137;_LL136: return 16;_LL137: if((int)_tmp194 != 14)goto _LL139;_LL138:
return 17;_LL139: if((int)_tmp194 != 15)goto _LL13B;_LL13A: return 18;_LL13B: if(
_tmp194 <= (void*)17)goto _LL13F;if(*((int*)_tmp194)!= 3)goto _LL13D;_LL13C: return
19;_LL13D: if(*((int*)_tmp194)!= 4)goto _LL13F;_LL13E: return 20;_LL13F:;_LL140:
return 21;_LL114:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple6 _tmp196=({struct _tuple6 _tmp195;_tmp195.f1=att1;_tmp195.f2=att2;_tmp195;});
void*_tmp197;int _tmp198;void*_tmp199;int _tmp19A;void*_tmp19B;int _tmp19C;void*
_tmp19D;int _tmp19E;void*_tmp19F;int _tmp1A0;void*_tmp1A1;int _tmp1A2;void*_tmp1A3;
struct _dynforward_ptr _tmp1A4;void*_tmp1A5;struct _dynforward_ptr _tmp1A6;void*
_tmp1A7;void*_tmp1A8;int _tmp1A9;int _tmp1AA;void*_tmp1AB;void*_tmp1AC;int _tmp1AD;
int _tmp1AE;_LL142: _tmp197=_tmp196.f1;if(_tmp197 <= (void*)17)goto _LL144;if(*((int*)
_tmp197)!= 0)goto _LL144;_tmp198=((struct Cyc_Absyn_Regparm_att_struct*)_tmp197)->f1;
_tmp199=_tmp196.f2;if(_tmp199 <= (void*)17)goto _LL144;if(*((int*)_tmp199)!= 0)
goto _LL144;_tmp19A=((struct Cyc_Absyn_Regparm_att_struct*)_tmp199)->f1;_LL143:
_tmp19C=_tmp198;_tmp19E=_tmp19A;goto _LL145;_LL144: _tmp19B=_tmp196.f1;if(_tmp19B
<= (void*)17)goto _LL146;if(*((int*)_tmp19B)!= 4)goto _LL146;_tmp19C=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp19B)->f1;_tmp19D=_tmp196.f2;if(_tmp19D <= (void*)17)goto _LL146;if(*((int*)
_tmp19D)!= 4)goto _LL146;_tmp19E=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp19D)->f1;_LL145: _tmp1A0=_tmp19C;_tmp1A2=_tmp19E;goto _LL147;_LL146: _tmp19F=
_tmp196.f1;if(_tmp19F <= (void*)17)goto _LL148;if(*((int*)_tmp19F)!= 1)goto _LL148;
_tmp1A0=((struct Cyc_Absyn_Aligned_att_struct*)_tmp19F)->f1;_tmp1A1=_tmp196.f2;
if(_tmp1A1 <= (void*)17)goto _LL148;if(*((int*)_tmp1A1)!= 1)goto _LL148;_tmp1A2=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp1A1)->f1;_LL147: return Cyc_Core_intcmp(
_tmp1A0,_tmp1A2);_LL148: _tmp1A3=_tmp196.f1;if(_tmp1A3 <= (void*)17)goto _LL14A;if(*((
int*)_tmp1A3)!= 2)goto _LL14A;_tmp1A4=((struct Cyc_Absyn_Section_att_struct*)
_tmp1A3)->f1;_tmp1A5=_tmp196.f2;if(_tmp1A5 <= (void*)17)goto _LL14A;if(*((int*)
_tmp1A5)!= 2)goto _LL14A;_tmp1A6=((struct Cyc_Absyn_Section_att_struct*)_tmp1A5)->f1;
_LL149: return Cyc_strcmp((struct _dynforward_ptr)_tmp1A4,(struct _dynforward_ptr)
_tmp1A6);_LL14A: _tmp1A7=_tmp196.f1;if(_tmp1A7 <= (void*)17)goto _LL14C;if(*((int*)
_tmp1A7)!= 3)goto _LL14C;_tmp1A8=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1A7)->f1;_tmp1A9=((struct Cyc_Absyn_Format_att_struct*)_tmp1A7)->f2;_tmp1AA=((
struct Cyc_Absyn_Format_att_struct*)_tmp1A7)->f3;_tmp1AB=_tmp196.f2;if(_tmp1AB <= (
void*)17)goto _LL14C;if(*((int*)_tmp1AB)!= 3)goto _LL14C;_tmp1AC=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1AB)->f1;_tmp1AD=((struct Cyc_Absyn_Format_att_struct*)
_tmp1AB)->f2;_tmp1AE=((struct Cyc_Absyn_Format_att_struct*)_tmp1AB)->f3;_LL14B: {
int _tmp1AF=Cyc_Core_intcmp((int)((unsigned int)_tmp1A8),(int)((unsigned int)
_tmp1AC));if(_tmp1AF != 0)return _tmp1AF;{int _tmp1B0=Cyc_Core_intcmp(_tmp1A9,
_tmp1AD);if(_tmp1B0 != 0)return _tmp1B0;return Cyc_Core_intcmp(_tmp1AA,_tmp1AE);}}
_LL14C:;_LL14D: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL141:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp1B1=Cyc_Tcutil_tvar_kind(tv);_LL14F:
if((int)_tmp1B1 != 3)goto _LL151;_LL150: t=(void*)2;goto _LL14E;_LL151: if((int)
_tmp1B1 != 4)goto _LL153;_LL152: t=Cyc_Absyn_empty_effect;goto _LL14E;_LL153: if((int)
_tmp1B1 != 5)goto _LL155;_LL154: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1B2=
_cycalloc_atomic(sizeof(*_tmp1B2));_tmp1B2[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp1B3;_tmp1B3.tag=18;_tmp1B3.f1=0;_tmp1B3;});_tmp1B2;});goto _LL14E;_LL155:;
_LL156: t=Cyc_Absyn_sint_typ;goto _LL14E;_LL14E:;}return({struct _tuple8*_tmp1B4=
_cycalloc(sizeof(*_tmp1B4));_tmp1B4->f1=tv;_tmp1B4->f2=t;_tmp1B4;});}static void*
Cyc_Tcutil_rgns_of(void*t){void*_tmp1B5=Cyc_Tcutil_compress(t);void*_tmp1B6;
struct Cyc_Absyn_TunionInfo _tmp1B7;struct Cyc_List_List*_tmp1B8;void*_tmp1B9;
struct Cyc_Absyn_PtrInfo _tmp1BA;void*_tmp1BB;struct Cyc_Absyn_PtrAtts _tmp1BC;void*
_tmp1BD;struct Cyc_Absyn_ArrayInfo _tmp1BE;void*_tmp1BF;struct Cyc_List_List*
_tmp1C0;struct Cyc_Absyn_TunionFieldInfo _tmp1C1;struct Cyc_List_List*_tmp1C2;
struct Cyc_Absyn_AggrInfo _tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*
_tmp1C5;void*_tmp1C6;struct Cyc_Absyn_FnInfo _tmp1C7;struct Cyc_List_List*_tmp1C8;
struct Cyc_Core_Opt*_tmp1C9;void*_tmp1CA;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_VarargInfo*
_tmp1CC;struct Cyc_List_List*_tmp1CD;void*_tmp1CE;struct Cyc_List_List*_tmp1CF;
_LL158: if((int)_tmp1B5 != 0)goto _LL15A;_LL159: goto _LL15B;_LL15A: if((int)_tmp1B5 != 
1)goto _LL15C;_LL15B: goto _LL15D;_LL15C: if(_tmp1B5 <= (void*)4)goto _LL180;if(*((int*)
_tmp1B5)!= 6)goto _LL15E;_LL15D: goto _LL15F;_LL15E: if(*((int*)_tmp1B5)!= 12)goto
_LL160;_LL15F: goto _LL161;_LL160: if(*((int*)_tmp1B5)!= 13)goto _LL162;_LL161: goto
_LL163;_LL162: if(*((int*)_tmp1B5)!= 18)goto _LL164;_LL163: goto _LL165;_LL164: if(*((
int*)_tmp1B5)!= 5)goto _LL166;_LL165: return Cyc_Absyn_empty_effect;_LL166: if(*((
int*)_tmp1B5)!= 0)goto _LL168;_LL167: goto _LL169;_LL168: if(*((int*)_tmp1B5)!= 1)
goto _LL16A;_LL169: {void*_tmp1D0=Cyc_Tcutil_typ_kind(t);_LL18D: if((int)_tmp1D0 != 
3)goto _LL18F;_LL18E: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D1=
_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D2;
_tmp1D2.tag=19;_tmp1D2.f1=(void*)t;_tmp1D2;});_tmp1D1;});_LL18F: if((int)_tmp1D0
!= 4)goto _LL191;_LL190: return t;_LL191: if((int)_tmp1D0 != 5)goto _LL193;_LL192:
return Cyc_Absyn_empty_effect;_LL193:;_LL194: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1D4;_tmp1D4.tag=21;_tmp1D4.f1=(void*)t;_tmp1D4;});_tmp1D3;});_LL18C:;}_LL16A:
if(*((int*)_tmp1B5)!= 15)goto _LL16C;_tmp1B6=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1B5)->f1;_LL16B: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D5=
_cycalloc(sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D6;
_tmp1D6.tag=19;_tmp1D6.f1=(void*)_tmp1B6;_tmp1D6;});_tmp1D5;});_LL16C: if(*((int*)
_tmp1B5)!= 2)goto _LL16E;_tmp1B7=((struct Cyc_Absyn_TunionType_struct*)_tmp1B5)->f1;
_tmp1B8=_tmp1B7.targs;_tmp1B9=(void*)_tmp1B7.rgn;_LL16D: {struct Cyc_List_List*ts=({
struct Cyc_List_List*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));
_tmp1DA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1DB;_tmp1DB.tag=19;_tmp1DB.f1=(
void*)_tmp1B9;_tmp1DB;});_tmp1DA;}));_tmp1D9->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1B8);_tmp1D9;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D8;_tmp1D8.tag=20;_tmp1D8.f1=ts;_tmp1D8;});_tmp1D7;}));}_LL16E: if(*((int*)
_tmp1B5)!= 4)goto _LL170;_tmp1BA=((struct Cyc_Absyn_PointerType_struct*)_tmp1B5)->f1;
_tmp1BB=(void*)_tmp1BA.elt_typ;_tmp1BC=_tmp1BA.ptr_atts;_tmp1BD=(void*)_tmp1BC.rgn;
_LL16F: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1DD;_tmp1DD.tag=20;_tmp1DD.f1=({void*_tmp1DE[2];_tmp1DE[1]=Cyc_Tcutil_rgns_of(
_tmp1BB);_tmp1DE[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1DF=_cycalloc(
sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1E0;_tmp1E0.tag=
19;_tmp1E0.f1=(void*)_tmp1BD;_tmp1E0;});_tmp1DF;});Cyc_List_list(_tag_dynforward(
_tmp1DE,sizeof(void*),2));});_tmp1DD;});_tmp1DC;}));_LL170: if(*((int*)_tmp1B5)!= 
7)goto _LL172;_tmp1BE=((struct Cyc_Absyn_ArrayType_struct*)_tmp1B5)->f1;_tmp1BF=(
void*)_tmp1BE.elt_type;_LL171: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp1BF));_LL172: if(*((int*)_tmp1B5)!= 9)goto _LL174;_tmp1C0=((struct Cyc_Absyn_TupleType_struct*)
_tmp1B5)->f1;_LL173: {struct Cyc_List_List*_tmp1E1=0;for(0;_tmp1C0 != 0;_tmp1C0=
_tmp1C0->tl){_tmp1E1=({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));
_tmp1E2->hd=(void*)(*((struct _tuple4*)_tmp1C0->hd)).f2;_tmp1E2->tl=_tmp1E1;
_tmp1E2;});}_tmp1C2=_tmp1E1;goto _LL175;}_LL174: if(*((int*)_tmp1B5)!= 3)goto
_LL176;_tmp1C1=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1B5)->f1;_tmp1C2=
_tmp1C1.targs;_LL175: _tmp1C4=_tmp1C2;goto _LL177;_LL176: if(*((int*)_tmp1B5)!= 10)
goto _LL178;_tmp1C3=((struct Cyc_Absyn_AggrType_struct*)_tmp1B5)->f1;_tmp1C4=
_tmp1C3.targs;_LL177: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E4;_tmp1E4.tag=20;_tmp1E4.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1C4);
_tmp1E4;});_tmp1E3;}));_LL178: if(*((int*)_tmp1B5)!= 11)goto _LL17A;_tmp1C5=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1B5)->f2;_LL179: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));
_tmp1E5[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1E6;_tmp1E6.tag=20;_tmp1E6.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1C5);_tmp1E6;});_tmp1E5;}));_LL17A:
if(*((int*)_tmp1B5)!= 14)goto _LL17C;_tmp1C6=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1B5)->f1;_LL17B: return Cyc_Tcutil_rgns_of(_tmp1C6);_LL17C: if(*((int*)_tmp1B5)
!= 17)goto _LL17E;_LL17D: return Cyc_Tcutil_rgns_of(t);_LL17E: if(*((int*)_tmp1B5)!= 
8)goto _LL180;_tmp1C7=((struct Cyc_Absyn_FnType_struct*)_tmp1B5)->f1;_tmp1C8=
_tmp1C7.tvars;_tmp1C9=_tmp1C7.effect;_tmp1CA=(void*)_tmp1C7.ret_typ;_tmp1CB=
_tmp1C7.args;_tmp1CC=_tmp1C7.cyc_varargs;_tmp1CD=_tmp1C7.rgn_po;_LL17F: {void*
_tmp1E7=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1C8),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C9))->v);return Cyc_Tcutil_normalize_effect(
_tmp1E7);}_LL180: if((int)_tmp1B5 != 3)goto _LL182;_LL181: goto _LL183;_LL182: if((int)
_tmp1B5 != 2)goto _LL184;_LL183: return Cyc_Absyn_empty_effect;_LL184: if(_tmp1B5 <= (
void*)4)goto _LL186;if(*((int*)_tmp1B5)!= 19)goto _LL186;_LL185: goto _LL187;_LL186:
if(_tmp1B5 <= (void*)4)goto _LL188;if(*((int*)_tmp1B5)!= 20)goto _LL188;_LL187:
return t;_LL188: if(_tmp1B5 <= (void*)4)goto _LL18A;if(*((int*)_tmp1B5)!= 21)goto
_LL18A;_tmp1CE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1B5)->f1;_LL189:
return Cyc_Tcutil_rgns_of(_tmp1CE);_LL18A: if(_tmp1B5 <= (void*)4)goto _LL157;if(*((
int*)_tmp1B5)!= 16)goto _LL157;_tmp1CF=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1B5)->f2;_LL18B: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E9;_tmp1E9.tag=20;_tmp1E9.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1CF);
_tmp1E9;});_tmp1E8;}));_LL157:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1EA=e;struct Cyc_List_List*_tmp1EB;struct Cyc_List_List**_tmp1EC;void*
_tmp1ED;_LL196: if(_tmp1EA <= (void*)4)goto _LL19A;if(*((int*)_tmp1EA)!= 20)goto
_LL198;_tmp1EB=((struct Cyc_Absyn_JoinEff_struct*)_tmp1EA)->f1;_tmp1EC=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1EA)->f1;_LL197: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1EC;for(0;effs != 0;effs=effs->tl){void*_tmp1EE=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1EE)));{void*
_tmp1EF=(void*)effs->hd;void*_tmp1F0;_LL19D: if(_tmp1EF <= (void*)4)goto _LL1A1;if(*((
int*)_tmp1EF)!= 20)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(*((int*)_tmp1EF)!= 19)
goto _LL1A1;_tmp1F0=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1EF)->f1;if((
int)_tmp1F0 != 2)goto _LL1A1;_LL1A0: redo_join=1;goto _LL19C;_LL1A1:;_LL1A2: goto
_LL19C;_LL19C:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1EC;for(0;effs != 0;effs=effs->tl){void*_tmp1F1=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1F2;void*_tmp1F3;_LL1A4: if(_tmp1F1 <= (
void*)4)goto _LL1A8;if(*((int*)_tmp1F1)!= 20)goto _LL1A6;_tmp1F2=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F1)->f1;_LL1A5: effects=Cyc_List_revappend(_tmp1F2,effects);goto _LL1A3;
_LL1A6: if(*((int*)_tmp1F1)!= 19)goto _LL1A8;_tmp1F3=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F1)->f1;if((int)_tmp1F3 != 2)goto _LL1A8;_LL1A7: goto _LL1A3;_LL1A8:;_LL1A9:
effects=({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->hd=(
void*)_tmp1F1;_tmp1F4->tl=effects;_tmp1F4;});goto _LL1A3;_LL1A3:;}}*_tmp1EC=Cyc_List_imp_rev(
effects);return e;}}_LL198: if(*((int*)_tmp1EA)!= 21)goto _LL19A;_tmp1ED=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1EA)->f1;_LL199: {void*_tmp1F5=Cyc_Tcutil_compress(
_tmp1ED);_LL1AB: if(_tmp1F5 <= (void*)4)goto _LL1AF;if(*((int*)_tmp1F5)!= 0)goto
_LL1AD;_LL1AC: goto _LL1AE;_LL1AD: if(*((int*)_tmp1F5)!= 1)goto _LL1AF;_LL1AE: return
e;_LL1AF:;_LL1B0: return Cyc_Tcutil_rgns_of(_tmp1ED);_LL1AA:;}_LL19A:;_LL19B:
return e;_LL195:;}}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};
static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp1F6=({struct Cyc_Absyn_FnType_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));
_tmp1F7[0]=({struct Cyc_Absyn_FnType_struct _tmp1F8;_tmp1F8.tag=8;_tmp1F8.f1=({
struct Cyc_Absyn_FnInfo _tmp1F9;_tmp1F9.tvars=0;_tmp1F9.effect=({struct Cyc_Core_Opt*
_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->v=(void*)eff;_tmp1FA;});_tmp1F9.ret_typ=(
void*)((void*)0);_tmp1F9.args=0;_tmp1F9.c_varargs=0;_tmp1F9.cyc_varargs=0;
_tmp1F9.rgn_po=0;_tmp1F9.attributes=0;_tmp1F9;});_tmp1F8;});_tmp1F7;});return Cyc_Absyn_atb_typ((
void*)_tmp1F6,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2)return 1;{void*_tmp1FB=Cyc_Tcutil_compress(e);void*_tmp1FC;
struct Cyc_List_List*_tmp1FD;void*_tmp1FE;struct Cyc_Core_Opt*_tmp1FF;struct Cyc_Core_Opt*
_tmp200;struct Cyc_Core_Opt**_tmp201;struct Cyc_Core_Opt*_tmp202;_LL1B2: if(_tmp1FB
<= (void*)4)goto _LL1BA;if(*((int*)_tmp1FB)!= 19)goto _LL1B4;_tmp1FC=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1FB)->f1;_LL1B3: if(constrain)return Cyc_Tcutil_unify(
r,_tmp1FC);_tmp1FC=Cyc_Tcutil_compress(_tmp1FC);if(r == _tmp1FC)return 1;{struct
_tuple6 _tmp204=({struct _tuple6 _tmp203;_tmp203.f1=r;_tmp203.f2=_tmp1FC;_tmp203;});
void*_tmp205;struct Cyc_Absyn_Tvar*_tmp206;void*_tmp207;struct Cyc_Absyn_Tvar*
_tmp208;_LL1BD: _tmp205=_tmp204.f1;if(_tmp205 <= (void*)4)goto _LL1BF;if(*((int*)
_tmp205)!= 1)goto _LL1BF;_tmp206=((struct Cyc_Absyn_VarType_struct*)_tmp205)->f1;
_tmp207=_tmp204.f2;if(_tmp207 <= (void*)4)goto _LL1BF;if(*((int*)_tmp207)!= 1)goto
_LL1BF;_tmp208=((struct Cyc_Absyn_VarType_struct*)_tmp207)->f1;_LL1BE: return Cyc_Absyn_tvar_cmp(
_tmp206,_tmp208)== 0;_LL1BF:;_LL1C0: return 0;_LL1BC:;}_LL1B4: if(*((int*)_tmp1FB)
!= 20)goto _LL1B6;_tmp1FD=((struct Cyc_Absyn_JoinEff_struct*)_tmp1FB)->f1;_LL1B5:
for(0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){if(Cyc_Tcutil_region_in_effect(constrain,
r,(void*)_tmp1FD->hd))return 1;}return 0;_LL1B6: if(*((int*)_tmp1FB)!= 21)goto
_LL1B8;_tmp1FE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1FB)->f1;_LL1B7: {
void*_tmp209=Cyc_Tcutil_rgns_of(_tmp1FE);void*_tmp20A;_LL1C2: if(_tmp209 <= (void*)
4)goto _LL1C4;if(*((int*)_tmp209)!= 21)goto _LL1C4;_tmp20A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp209)->f1;_LL1C3: if(!constrain)return 0;{void*_tmp20B=Cyc_Tcutil_compress(
_tmp20A);struct Cyc_Core_Opt*_tmp20C;struct Cyc_Core_Opt*_tmp20D;struct Cyc_Core_Opt**
_tmp20E;struct Cyc_Core_Opt*_tmp20F;_LL1C7: if(_tmp20B <= (void*)4)goto _LL1C9;if(*((
int*)_tmp20B)!= 0)goto _LL1C9;_tmp20C=((struct Cyc_Absyn_Evar_struct*)_tmp20B)->f1;
_tmp20D=((struct Cyc_Absyn_Evar_struct*)_tmp20B)->f2;_tmp20E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp20B)->f2;_tmp20F=((struct Cyc_Absyn_Evar_struct*)
_tmp20B)->f4;_LL1C8: {void*_tmp210=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp20F);Cyc_Tcutil_occurs(_tmp210,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20F))->v,r);{void*_tmp211=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp213=_cycalloc(sizeof(*_tmp213));
_tmp213[0]=({struct Cyc_Absyn_JoinEff_struct _tmp214;_tmp214.tag=20;_tmp214.f1=({
void*_tmp215[2];_tmp215[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp216=
_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_AccessEff_struct _tmp217;
_tmp217.tag=19;_tmp217.f1=(void*)r;_tmp217;});_tmp216;});_tmp215[0]=_tmp210;Cyc_List_list(
_tag_dynforward(_tmp215,sizeof(void*),2));});_tmp214;});_tmp213;}));*_tmp20E=({
struct Cyc_Core_Opt*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->v=(void*)_tmp211;
_tmp212;});return 1;}}_LL1C9:;_LL1CA: return 0;_LL1C6:;}_LL1C4:;_LL1C5: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp209);_LL1C1:;}_LL1B8: if(*((int*)_tmp1FB)!= 0)goto _LL1BA;_tmp1FF=((
struct Cyc_Absyn_Evar_struct*)_tmp1FB)->f1;_tmp200=((struct Cyc_Absyn_Evar_struct*)
_tmp1FB)->f2;_tmp201=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1FB)->f2;_tmp202=((struct Cyc_Absyn_Evar_struct*)_tmp1FB)->f4;_LL1B9: if(
_tmp1FF == 0  || (void*)_tmp1FF->v != (void*)4)({void*_tmp218[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp219="effect evar has wrong kind";_tag_dynforward(_tmp219,sizeof(char),
_get_zero_arr_size(_tmp219,27));}),_tag_dynforward(_tmp218,sizeof(void*),0));});
if(!constrain)return 0;{void*_tmp21A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp202);Cyc_Tcutil_occurs(_tmp21A,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp202))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp21B=({struct Cyc_Absyn_JoinEff_struct*_tmp21D=_cycalloc(sizeof(*_tmp21D));
_tmp21D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp21E;_tmp21E.tag=20;_tmp21E.f1=({
struct Cyc_List_List*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->hd=(void*)
_tmp21A;_tmp21F->tl=({struct Cyc_List_List*_tmp220=_cycalloc(sizeof(*_tmp220));
_tmp220->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp221=_cycalloc(
sizeof(*_tmp221));_tmp221[0]=({struct Cyc_Absyn_AccessEff_struct _tmp222;_tmp222.tag=
19;_tmp222.f1=(void*)r;_tmp222;});_tmp221;}));_tmp220->tl=0;_tmp220;});_tmp21F;});
_tmp21E;});_tmp21D;});*_tmp201=({struct Cyc_Core_Opt*_tmp21C=_cycalloc(sizeof(*
_tmp21C));_tmp21C->v=(void*)((void*)_tmp21B);_tmp21C;});return 1;}}_LL1BA:;_LL1BB:
return 0;_LL1B1:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp223=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp224;void*_tmp225;struct Cyc_Core_Opt*_tmp226;struct
Cyc_Core_Opt*_tmp227;struct Cyc_Core_Opt**_tmp228;struct Cyc_Core_Opt*_tmp229;
_LL1CC: if(_tmp223 <= (void*)4)goto _LL1D4;if(*((int*)_tmp223)!= 19)goto _LL1CE;
_LL1CD: return 0;_LL1CE: if(*((int*)_tmp223)!= 20)goto _LL1D0;_tmp224=((struct Cyc_Absyn_JoinEff_struct*)
_tmp223)->f1;_LL1CF: for(0;_tmp224 != 0;_tmp224=_tmp224->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp224->hd))return 1;}return 0;_LL1D0: if(*((int*)
_tmp223)!= 21)goto _LL1D2;_tmp225=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp223)->f1;_LL1D1: _tmp225=Cyc_Tcutil_compress(_tmp225);if(t == _tmp225)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp225);{void*_tmp22A=Cyc_Tcutil_rgns_of(
t);void*_tmp22B;_LL1D7: if(_tmp22A <= (void*)4)goto _LL1D9;if(*((int*)_tmp22A)!= 21)
goto _LL1D9;_tmp22B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp22A)->f1;_LL1D8: {
struct _tuple6 _tmp22D=({struct _tuple6 _tmp22C;_tmp22C.f1=Cyc_Tcutil_compress(
_tmp22B);_tmp22C.f2=_tmp225;_tmp22C;});void*_tmp22E;struct Cyc_Absyn_Tvar*_tmp22F;
void*_tmp230;struct Cyc_Absyn_Tvar*_tmp231;_LL1DC: _tmp22E=_tmp22D.f1;if(_tmp22E <= (
void*)4)goto _LL1DE;if(*((int*)_tmp22E)!= 1)goto _LL1DE;_tmp22F=((struct Cyc_Absyn_VarType_struct*)
_tmp22E)->f1;_tmp230=_tmp22D.f2;if(_tmp230 <= (void*)4)goto _LL1DE;if(*((int*)
_tmp230)!= 1)goto _LL1DE;_tmp231=((struct Cyc_Absyn_VarType_struct*)_tmp230)->f1;
_LL1DD: return Cyc_Tcutil_unify(t,_tmp225);_LL1DE:;_LL1DF: return _tmp22B == _tmp225;
_LL1DB:;}_LL1D9:;_LL1DA: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp22A);_LL1D6:;}_LL1D2: if(*((int*)_tmp223)!= 0)goto _LL1D4;_tmp226=((struct Cyc_Absyn_Evar_struct*)
_tmp223)->f1;_tmp227=((struct Cyc_Absyn_Evar_struct*)_tmp223)->f2;_tmp228=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp223)->f2;_tmp229=((struct Cyc_Absyn_Evar_struct*)
_tmp223)->f4;_LL1D3: if(_tmp226 == 0  || (void*)_tmp226->v != (void*)4)({void*
_tmp232[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp233="effect evar has wrong kind";_tag_dynforward(_tmp233,sizeof(
char),_get_zero_arr_size(_tmp233,27));}),_tag_dynforward(_tmp232,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{void*_tmp234=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp229);Cyc_Tcutil_occurs(_tmp234,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp229))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp235=({struct Cyc_Absyn_JoinEff_struct*_tmp237=_cycalloc(sizeof(*_tmp237));
_tmp237[0]=({struct Cyc_Absyn_JoinEff_struct _tmp238;_tmp238.tag=20;_tmp238.f1=({
struct Cyc_List_List*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->hd=(void*)
_tmp234;_tmp239->tl=({struct Cyc_List_List*_tmp23A=_cycalloc(sizeof(*_tmp23A));
_tmp23A->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp23B=_cycalloc(
sizeof(*_tmp23B));_tmp23B[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp23C;_tmp23C.tag=
21;_tmp23C.f1=(void*)t;_tmp23C;});_tmp23B;}));_tmp23A->tl=0;_tmp23A;});_tmp239;});
_tmp238;});_tmp237;});*_tmp228=({struct Cyc_Core_Opt*_tmp236=_cycalloc(sizeof(*
_tmp236));_tmp236->v=(void*)((void*)_tmp235);_tmp236;});return 1;}}_LL1D4:;_LL1D5:
return 0;_LL1CB:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp23D=e;struct
Cyc_Absyn_Tvar*_tmp23E;struct Cyc_List_List*_tmp23F;void*_tmp240;struct Cyc_Core_Opt*
_tmp241;struct Cyc_Core_Opt*_tmp242;struct Cyc_Core_Opt**_tmp243;struct Cyc_Core_Opt*
_tmp244;_LL1E1: if(_tmp23D <= (void*)4)goto _LL1E9;if(*((int*)_tmp23D)!= 1)goto
_LL1E3;_tmp23E=((struct Cyc_Absyn_VarType_struct*)_tmp23D)->f1;_LL1E2: return Cyc_Absyn_tvar_cmp(
v,_tmp23E)== 0;_LL1E3: if(*((int*)_tmp23D)!= 20)goto _LL1E5;_tmp23F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp23D)->f1;_LL1E4: for(0;_tmp23F != 0;_tmp23F=_tmp23F->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp23F->hd))return 1;}return 0;_LL1E5: if(*((int*)
_tmp23D)!= 21)goto _LL1E7;_tmp240=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp23D)->f1;_LL1E6: {void*_tmp245=Cyc_Tcutil_rgns_of(_tmp240);void*_tmp246;
_LL1EC: if(_tmp245 <= (void*)4)goto _LL1EE;if(*((int*)_tmp245)!= 21)goto _LL1EE;
_tmp246=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp245)->f1;_LL1ED: if(!
may_constrain_evars)return 0;{void*_tmp247=Cyc_Tcutil_compress(_tmp246);struct Cyc_Core_Opt*
_tmp248;struct Cyc_Core_Opt*_tmp249;struct Cyc_Core_Opt**_tmp24A;struct Cyc_Core_Opt*
_tmp24B;_LL1F1: if(_tmp247 <= (void*)4)goto _LL1F3;if(*((int*)_tmp247)!= 0)goto
_LL1F3;_tmp248=((struct Cyc_Absyn_Evar_struct*)_tmp247)->f1;_tmp249=((struct Cyc_Absyn_Evar_struct*)
_tmp247)->f2;_tmp24A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp247)->f2;_tmp24B=((struct Cyc_Absyn_Evar_struct*)_tmp247)->f4;_LL1F2: {void*
_tmp24C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp24B);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp24B))->v,v))return 0;{void*_tmp24D=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp24F=_cycalloc(sizeof(*_tmp24F));
_tmp24F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp250;_tmp250.tag=20;_tmp250.f1=({
void*_tmp251[2];_tmp251[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp252=
_cycalloc(sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Absyn_VarType_struct _tmp253;
_tmp253.tag=1;_tmp253.f1=v;_tmp253;});_tmp252;});_tmp251[0]=_tmp24C;Cyc_List_list(
_tag_dynforward(_tmp251,sizeof(void*),2));});_tmp250;});_tmp24F;}));*_tmp24A=({
struct Cyc_Core_Opt*_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E->v=(void*)_tmp24D;
_tmp24E;});return 1;}}_LL1F3:;_LL1F4: return 0;_LL1F0:;}_LL1EE:;_LL1EF: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp245);_LL1EB:;}_LL1E7: if(*((int*)_tmp23D)!= 0)goto
_LL1E9;_tmp241=((struct Cyc_Absyn_Evar_struct*)_tmp23D)->f1;_tmp242=((struct Cyc_Absyn_Evar_struct*)
_tmp23D)->f2;_tmp243=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp23D)->f2;_tmp244=((struct Cyc_Absyn_Evar_struct*)_tmp23D)->f4;_LL1E8: if(
_tmp241 == 0  || (void*)_tmp241->v != (void*)4)({void*_tmp254[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp255="effect evar has wrong kind";_tag_dynforward(_tmp255,sizeof(char),
_get_zero_arr_size(_tmp255,27));}),_tag_dynforward(_tmp254,sizeof(void*),0));});{
void*_tmp256=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp244);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp244))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp257=({struct Cyc_Absyn_JoinEff_struct*_tmp259=_cycalloc(sizeof(*_tmp259));
_tmp259[0]=({struct Cyc_Absyn_JoinEff_struct _tmp25A;_tmp25A.tag=20;_tmp25A.f1=({
struct Cyc_List_List*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B->hd=(void*)
_tmp256;_tmp25B->tl=({struct Cyc_List_List*_tmp25C=_cycalloc(sizeof(*_tmp25C));
_tmp25C->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp25D=_cycalloc(
sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_Absyn_VarType_struct _tmp25E;_tmp25E.tag=
1;_tmp25E.f1=v;_tmp25E;});_tmp25D;}));_tmp25C->tl=0;_tmp25C;});_tmp25B;});
_tmp25A;});_tmp259;});*_tmp243=({struct Cyc_Core_Opt*_tmp258=_cycalloc(sizeof(*
_tmp258));_tmp258->v=(void*)((void*)_tmp257);_tmp258;});return 1;}}_LL1E9:;_LL1EA:
return 0;_LL1E0:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp25F=e;struct Cyc_List_List*_tmp260;void*_tmp261;_LL1F6: if(_tmp25F <= (
void*)4)goto _LL1FC;if(*((int*)_tmp25F)!= 20)goto _LL1F8;_tmp260=((struct Cyc_Absyn_JoinEff_struct*)
_tmp25F)->f1;_LL1F7: for(0;_tmp260 != 0;_tmp260=_tmp260->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp260->hd))return 1;}return 0;_LL1F8: if(*((int*)_tmp25F)!= 21)goto
_LL1FA;_tmp261=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp25F)->f1;_LL1F9: {
void*_tmp262=Cyc_Tcutil_rgns_of(_tmp261);void*_tmp263;_LL1FF: if(_tmp262 <= (void*)
4)goto _LL201;if(*((int*)_tmp262)!= 21)goto _LL201;_tmp263=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp262)->f1;_LL200: return 0;_LL201:;_LL202: return Cyc_Tcutil_evar_in_effect(evar,
_tmp262);_LL1FE:;}_LL1FA: if(*((int*)_tmp25F)!= 0)goto _LL1FC;_LL1FB: return evar == 
e;_LL1FC:;_LL1FD: return 0;_LL1F5:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp264=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp265;void*_tmp266;struct Cyc_Absyn_Tvar*_tmp267;void*_tmp268;
struct Cyc_Core_Opt*_tmp269;struct Cyc_Core_Opt**_tmp26A;struct Cyc_Core_Opt*
_tmp26B;_LL204: if(_tmp264 <= (void*)4)goto _LL20E;if(*((int*)_tmp264)!= 20)goto
_LL206;_tmp265=((struct Cyc_Absyn_JoinEff_struct*)_tmp264)->f1;_LL205: for(0;
_tmp265 != 0;_tmp265=_tmp265->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp265->hd,e2))return 0;}return 1;_LL206: if(*((int*)
_tmp264)!= 19)goto _LL208;_tmp266=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp264)->f1;_LL207: return Cyc_Tcutil_region_in_effect(0,_tmp266,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp266,(void*)2);_LL208: if(*((int*)
_tmp264)!= 1)goto _LL20A;_tmp267=((struct Cyc_Absyn_VarType_struct*)_tmp264)->f1;
_LL209: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp267,e2);
_LL20A: if(*((int*)_tmp264)!= 21)goto _LL20C;_tmp268=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp264)->f1;_LL20B: {void*_tmp26C=Cyc_Tcutil_rgns_of(_tmp268);void*_tmp26D;
_LL211: if(_tmp26C <= (void*)4)goto _LL213;if(*((int*)_tmp26C)!= 21)goto _LL213;
_tmp26D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp26C)->f1;_LL212: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp26D,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp26D,Cyc_Absyn_sint_typ);_LL213:;_LL214: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp26C,e2);_LL210:;}_LL20C: if(*((int*)_tmp264)!= 0)goto
_LL20E;_tmp269=((struct Cyc_Absyn_Evar_struct*)_tmp264)->f2;_tmp26A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp264)->f2;_tmp26B=((struct Cyc_Absyn_Evar_struct*)
_tmp264)->f4;_LL20D: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp26A=({struct Cyc_Core_Opt*
_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->v=(void*)Cyc_Absyn_empty_effect;
_tmp26E;});return 1;_LL20E:;_LL20F:({struct Cyc_String_pa_struct _tmp271;_tmp271.tag=
0;_tmp271.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp26F[1]={& _tmp271};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp270="subset_effect: bad effect: %s";
_tag_dynforward(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,30));}),
_tag_dynforward(_tmp26F,sizeof(void*),1));}});_LL203:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){struct
_tuple6 _tmp273;void*_tmp274;void*_tmp275;struct _tuple6*_tmp272=(struct _tuple6*)
r1->hd;_tmp273=*_tmp272;_tmp274=_tmp273.f1;_tmp275=_tmp273.f2;{int found=_tmp274
== (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple6 _tmp277;void*_tmp278;void*_tmp279;struct _tuple6*_tmp276=(struct
_tuple6*)r2->hd;_tmp277=*_tmp276;_tmp278=_tmp277.f1;_tmp279=_tmp277.f2;if(Cyc_Tcutil_unify(
_tmp274,_tmp278) && Cyc_Tcutil_unify(_tmp275,_tmp279)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp27A=t1;struct Cyc_Core_Opt*_tmp27B;struct Cyc_Core_Opt*
_tmp27C;struct Cyc_Core_Opt**_tmp27D;struct Cyc_Core_Opt*_tmp27E;_LL216: if(_tmp27A
<= (void*)4)goto _LL218;if(*((int*)_tmp27A)!= 0)goto _LL218;_tmp27B=((struct Cyc_Absyn_Evar_struct*)
_tmp27A)->f1;_tmp27C=((struct Cyc_Absyn_Evar_struct*)_tmp27A)->f2;_tmp27D=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp27A)->f2;_tmp27E=((struct Cyc_Absyn_Evar_struct*)
_tmp27A)->f4;_LL217: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp27E))->v,t2);{void*_tmp27F=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp27F,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp27B))->v)){*_tmp27D=({struct Cyc_Core_Opt*_tmp280=_cycalloc(sizeof(*_tmp280));
_tmp280->v=(void*)t2;_tmp280;});return;}else{{void*_tmp281=t2;struct Cyc_Core_Opt*
_tmp282;struct Cyc_Core_Opt**_tmp283;struct Cyc_Core_Opt*_tmp284;struct Cyc_Absyn_PtrInfo
_tmp285;_LL21B: if(_tmp281 <= (void*)4)goto _LL21F;if(*((int*)_tmp281)!= 0)goto
_LL21D;_tmp282=((struct Cyc_Absyn_Evar_struct*)_tmp281)->f2;_tmp283=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp281)->f2;_tmp284=((struct Cyc_Absyn_Evar_struct*)
_tmp281)->f4;_LL21C: {struct Cyc_List_List*_tmp286=(struct Cyc_List_List*)_tmp27E->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp284))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp286,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp287="(type variable would escape scope)";_tag_dynforward(_tmp287,
sizeof(char),_get_zero_arr_size(_tmp287,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp27B->v,_tmp27F)){*_tmp283=({struct Cyc_Core_Opt*
_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->v=(void*)t1;_tmp288;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp289="(kinds are incompatible)";_tag_dynforward(_tmp289,sizeof(char),
_get_zero_arr_size(_tmp289,25));});goto _LL21A;}_LL21D: if(*((int*)_tmp281)!= 4)
goto _LL21F;_tmp285=((struct Cyc_Absyn_PointerType_struct*)_tmp281)->f1;if(!((void*)
_tmp27B->v == (void*)2))goto _LL21F;_LL21E: {struct Cyc_Absyn_Conref*_tmp28A=Cyc_Absyn_compress_conref((
_tmp285.ptr_atts).bounds);{void*_tmp28B=(void*)_tmp28A->v;_LL222: if((int)_tmp28B
!= 0)goto _LL224;_LL223:(void*)(_tmp28A->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp28D;_tmp28D.tag=0;_tmp28D.f1=(void*)Cyc_Absyn_bounds_one;_tmp28D;});_tmp28C;})));*
_tmp27D=({struct Cyc_Core_Opt*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E->v=(
void*)t2;_tmp28E;});return;_LL224:;_LL225: goto _LL221;_LL221:;}goto _LL21A;}_LL21F:;
_LL220: goto _LL21A;_LL21A:;}Cyc_Tcutil_failure_reason=({const char*_tmp28F="(kinds are incompatible)";
_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}_LL218:;_LL219: goto _LL215;_LL215:;}{struct
_tuple6 _tmp291=({struct _tuple6 _tmp290;_tmp290.f1=t2;_tmp290.f2=t1;_tmp290;});
void*_tmp292;void*_tmp293;void*_tmp294;void*_tmp295;struct Cyc_Absyn_Tvar*_tmp296;
void*_tmp297;struct Cyc_Absyn_Tvar*_tmp298;void*_tmp299;struct Cyc_Absyn_AggrInfo
_tmp29A;void*_tmp29B;struct Cyc_List_List*_tmp29C;void*_tmp29D;struct Cyc_Absyn_AggrInfo
_tmp29E;void*_tmp29F;struct Cyc_List_List*_tmp2A0;void*_tmp2A1;struct _tuple1*
_tmp2A2;void*_tmp2A3;struct _tuple1*_tmp2A4;void*_tmp2A5;struct Cyc_List_List*
_tmp2A6;void*_tmp2A7;struct Cyc_List_List*_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_TunionInfo
_tmp2AA;void*_tmp2AB;struct Cyc_Absyn_Tuniondecl**_tmp2AC;struct Cyc_Absyn_Tuniondecl*
_tmp2AD;struct Cyc_List_List*_tmp2AE;void*_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_TunionInfo
_tmp2B1;void*_tmp2B2;struct Cyc_Absyn_Tuniondecl**_tmp2B3;struct Cyc_Absyn_Tuniondecl*
_tmp2B4;struct Cyc_List_List*_tmp2B5;void*_tmp2B6;void*_tmp2B7;struct Cyc_Absyn_TunionFieldInfo
_tmp2B8;void*_tmp2B9;struct Cyc_Absyn_Tuniondecl*_tmp2BA;struct Cyc_Absyn_Tunionfield*
_tmp2BB;struct Cyc_List_List*_tmp2BC;void*_tmp2BD;struct Cyc_Absyn_TunionFieldInfo
_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Tuniondecl*_tmp2C0;struct Cyc_Absyn_Tunionfield*
_tmp2C1;struct Cyc_List_List*_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_PtrInfo _tmp2C4;
void*_tmp2C5;struct Cyc_Absyn_Tqual _tmp2C6;struct Cyc_Absyn_PtrAtts _tmp2C7;void*
_tmp2C8;struct Cyc_Absyn_Conref*_tmp2C9;struct Cyc_Absyn_Conref*_tmp2CA;struct Cyc_Absyn_Conref*
_tmp2CB;void*_tmp2CC;struct Cyc_Absyn_PtrInfo _tmp2CD;void*_tmp2CE;struct Cyc_Absyn_Tqual
_tmp2CF;struct Cyc_Absyn_PtrAtts _tmp2D0;void*_tmp2D1;struct Cyc_Absyn_Conref*
_tmp2D2;struct Cyc_Absyn_Conref*_tmp2D3;struct Cyc_Absyn_Conref*_tmp2D4;void*
_tmp2D5;void*_tmp2D6;void*_tmp2D7;void*_tmp2D8;void*_tmp2D9;void*_tmp2DA;void*
_tmp2DB;void*_tmp2DC;void*_tmp2DD;int _tmp2DE;void*_tmp2DF;int _tmp2E0;void*
_tmp2E1;void*_tmp2E2;void*_tmp2E3;void*_tmp2E4;void*_tmp2E5;int _tmp2E6;void*
_tmp2E7;int _tmp2E8;void*_tmp2E9;void*_tmp2EA;void*_tmp2EB;void*_tmp2EC;void*
_tmp2ED;struct Cyc_Absyn_ArrayInfo _tmp2EE;void*_tmp2EF;struct Cyc_Absyn_Tqual
_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Conref*_tmp2F2;void*_tmp2F3;
struct Cyc_Absyn_ArrayInfo _tmp2F4;void*_tmp2F5;struct Cyc_Absyn_Tqual _tmp2F6;
struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_Absyn_Conref*_tmp2F8;void*_tmp2F9;struct
Cyc_Absyn_FnInfo _tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_Core_Opt*_tmp2FC;
void*_tmp2FD;struct Cyc_List_List*_tmp2FE;int _tmp2FF;struct Cyc_Absyn_VarargInfo*
_tmp300;struct Cyc_List_List*_tmp301;struct Cyc_List_List*_tmp302;void*_tmp303;
struct Cyc_Absyn_FnInfo _tmp304;struct Cyc_List_List*_tmp305;struct Cyc_Core_Opt*
_tmp306;void*_tmp307;struct Cyc_List_List*_tmp308;int _tmp309;struct Cyc_Absyn_VarargInfo*
_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_List_List*_tmp30C;void*_tmp30D;
struct Cyc_List_List*_tmp30E;void*_tmp30F;struct Cyc_List_List*_tmp310;void*
_tmp311;void*_tmp312;struct Cyc_List_List*_tmp313;void*_tmp314;void*_tmp315;
struct Cyc_List_List*_tmp316;void*_tmp317;void*_tmp318;void*_tmp319;void*_tmp31A;
void*_tmp31B;void*_tmp31C;void*_tmp31D;void*_tmp31E;void*_tmp31F;void*_tmp320;
void*_tmp321;void*_tmp322;void*_tmp323;void*_tmp324;_LL227: _tmp292=_tmp291.f1;
if(_tmp292 <= (void*)4)goto _LL229;if(*((int*)_tmp292)!= 0)goto _LL229;_LL228: Cyc_Tcutil_unify_it(
t2,t1);return;_LL229: _tmp293=_tmp291.f1;if((int)_tmp293 != 0)goto _LL22B;_tmp294=
_tmp291.f2;if((int)_tmp294 != 0)goto _LL22B;_LL22A: return;_LL22B: _tmp295=_tmp291.f1;
if(_tmp295 <= (void*)4)goto _LL22D;if(*((int*)_tmp295)!= 1)goto _LL22D;_tmp296=((
struct Cyc_Absyn_VarType_struct*)_tmp295)->f1;_tmp297=_tmp291.f2;if(_tmp297 <= (
void*)4)goto _LL22D;if(*((int*)_tmp297)!= 1)goto _LL22D;_tmp298=((struct Cyc_Absyn_VarType_struct*)
_tmp297)->f1;_LL22C: {struct _dynforward_ptr*_tmp325=_tmp296->name;struct
_dynforward_ptr*_tmp326=_tmp298->name;int _tmp327=*((int*)_check_null(_tmp296->identity));
int _tmp328=*((int*)_check_null(_tmp298->identity));void*_tmp329=Cyc_Tcutil_tvar_kind(
_tmp296);void*_tmp32A=Cyc_Tcutil_tvar_kind(_tmp298);if(_tmp328 == _tmp327  && Cyc_strptrcmp(
_tmp325,_tmp326)== 0){if(_tmp329 != _tmp32A)({struct Cyc_String_pa_struct _tmp32F;
_tmp32F.tag=0;_tmp32F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp32A));{struct Cyc_String_pa_struct _tmp32E;_tmp32E.tag=0;_tmp32E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp329));{
struct Cyc_String_pa_struct _tmp32D;_tmp32D.tag=0;_tmp32D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp325);{void*_tmp32B[3]={& _tmp32D,&
_tmp32E,& _tmp32F};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp32C="same type variable %s has kinds %s and %s";_tag_dynforward(
_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,42));}),_tag_dynforward(_tmp32B,
sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*_tmp330="(variable types are not the same)";
_tag_dynforward(_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,34));});goto
_LL226;}_LL22D: _tmp299=_tmp291.f1;if(_tmp299 <= (void*)4)goto _LL22F;if(*((int*)
_tmp299)!= 10)goto _LL22F;_tmp29A=((struct Cyc_Absyn_AggrType_struct*)_tmp299)->f1;
_tmp29B=(void*)_tmp29A.aggr_info;_tmp29C=_tmp29A.targs;_tmp29D=_tmp291.f2;if(
_tmp29D <= (void*)4)goto _LL22F;if(*((int*)_tmp29D)!= 10)goto _LL22F;_tmp29E=((
struct Cyc_Absyn_AggrType_struct*)_tmp29D)->f1;_tmp29F=(void*)_tmp29E.aggr_info;
_tmp2A0=_tmp29E.targs;_LL22E: {void*_tmp332;struct _tuple1*_tmp333;struct _tuple5
_tmp331=Cyc_Absyn_aggr_kinded_name(_tmp29F);_tmp332=_tmp331.f1;_tmp333=_tmp331.f2;{
void*_tmp335;struct _tuple1*_tmp336;struct _tuple5 _tmp334=Cyc_Absyn_aggr_kinded_name(
_tmp29B);_tmp335=_tmp334.f1;_tmp336=_tmp334.f2;if(_tmp332 != _tmp335){Cyc_Tcutil_failure_reason=({
const char*_tmp337="(struct and union type)";_tag_dynforward(_tmp337,sizeof(char),
_get_zero_arr_size(_tmp337,24));});goto _LL226;}if(Cyc_Absyn_qvar_cmp(_tmp333,
_tmp336)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp338="(different type name)";
_tag_dynforward(_tmp338,sizeof(char),_get_zero_arr_size(_tmp338,22));});goto
_LL226;}Cyc_Tcutil_unify_list(_tmp2A0,_tmp29C);return;}}_LL22F: _tmp2A1=_tmp291.f1;
if(_tmp2A1 <= (void*)4)goto _LL231;if(*((int*)_tmp2A1)!= 12)goto _LL231;_tmp2A2=((
struct Cyc_Absyn_EnumType_struct*)_tmp2A1)->f1;_tmp2A3=_tmp291.f2;if(_tmp2A3 <= (
void*)4)goto _LL231;if(*((int*)_tmp2A3)!= 12)goto _LL231;_tmp2A4=((struct Cyc_Absyn_EnumType_struct*)
_tmp2A3)->f1;_LL230: if(Cyc_Absyn_qvar_cmp(_tmp2A2,_tmp2A4)== 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp339="(different enum types)";_tag_dynforward(_tmp339,sizeof(char),
_get_zero_arr_size(_tmp339,23));});goto _LL226;_LL231: _tmp2A5=_tmp291.f1;if(
_tmp2A5 <= (void*)4)goto _LL233;if(*((int*)_tmp2A5)!= 13)goto _LL233;_tmp2A6=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp2A5)->f1;_tmp2A7=_tmp291.f2;if(_tmp2A7
<= (void*)4)goto _LL233;if(*((int*)_tmp2A7)!= 13)goto _LL233;_tmp2A8=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2A7)->f1;_LL232: {int bad=0;for(0;_tmp2A6 != 0  && _tmp2A8 != 0;(_tmp2A6=_tmp2A6->tl,
_tmp2A8=_tmp2A8->tl)){struct Cyc_Absyn_Enumfield*_tmp33A=(struct Cyc_Absyn_Enumfield*)
_tmp2A6->hd;struct Cyc_Absyn_Enumfield*_tmp33B=(struct Cyc_Absyn_Enumfield*)
_tmp2A8->hd;if(Cyc_Absyn_qvar_cmp(_tmp33A->name,_tmp33B->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp33C="(different names for enum fields)";_tag_dynforward(_tmp33C,
sizeof(char),_get_zero_arr_size(_tmp33C,34));});bad=1;break;}if(_tmp33A->tag == 
_tmp33B->tag)continue;if(_tmp33A->tag == 0  || _tmp33B->tag == 0){Cyc_Tcutil_failure_reason=({
const char*_tmp33D="(different tag values for enum fields)";_tag_dynforward(
_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,39));});bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp33A->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp33B->tag))){Cyc_Tcutil_failure_reason=({const char*_tmp33E="(different tag values for enum fields)";
_tag_dynforward(_tmp33E,sizeof(char),_get_zero_arr_size(_tmp33E,39));});bad=1;
break;}}if(bad)goto _LL226;if(_tmp2A6 == 0  && _tmp2A8 == 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp33F="(different number of fields for enums)";_tag_dynforward(
_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,39));});goto _LL226;}_LL233:
_tmp2A9=_tmp291.f1;if(_tmp2A9 <= (void*)4)goto _LL235;if(*((int*)_tmp2A9)!= 2)goto
_LL235;_tmp2AA=((struct Cyc_Absyn_TunionType_struct*)_tmp2A9)->f1;_tmp2AB=(void*)
_tmp2AA.tunion_info;if(*((int*)_tmp2AB)!= 1)goto _LL235;_tmp2AC=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp2AB)->f1;_tmp2AD=*_tmp2AC;_tmp2AE=_tmp2AA.targs;_tmp2AF=(void*)_tmp2AA.rgn;
_tmp2B0=_tmp291.f2;if(_tmp2B0 <= (void*)4)goto _LL235;if(*((int*)_tmp2B0)!= 2)goto
_LL235;_tmp2B1=((struct Cyc_Absyn_TunionType_struct*)_tmp2B0)->f1;_tmp2B2=(void*)
_tmp2B1.tunion_info;if(*((int*)_tmp2B2)!= 1)goto _LL235;_tmp2B3=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp2B2)->f1;_tmp2B4=*_tmp2B3;_tmp2B5=_tmp2B1.targs;_tmp2B6=(void*)_tmp2B1.rgn;
_LL234: if(_tmp2AD == _tmp2B4  || Cyc_Absyn_qvar_cmp(_tmp2AD->name,_tmp2B4->name)== 
0){Cyc_Tcutil_unify_it(_tmp2B6,_tmp2AF);Cyc_Tcutil_unify_list(_tmp2B5,_tmp2AE);
return;}Cyc_Tcutil_failure_reason=({const char*_tmp340="(different tunion types)";
_tag_dynforward(_tmp340,sizeof(char),_get_zero_arr_size(_tmp340,25));});goto
_LL226;_LL235: _tmp2B7=_tmp291.f1;if(_tmp2B7 <= (void*)4)goto _LL237;if(*((int*)
_tmp2B7)!= 3)goto _LL237;_tmp2B8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2B7)->f1;_tmp2B9=(void*)_tmp2B8.field_info;if(*((int*)_tmp2B9)!= 1)goto
_LL237;_tmp2BA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B9)->f1;_tmp2BB=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B9)->f2;_tmp2BC=_tmp2B8.targs;
_tmp2BD=_tmp291.f2;if(_tmp2BD <= (void*)4)goto _LL237;if(*((int*)_tmp2BD)!= 3)goto
_LL237;_tmp2BE=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2BD)->f1;_tmp2BF=(
void*)_tmp2BE.field_info;if(*((int*)_tmp2BF)!= 1)goto _LL237;_tmp2C0=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2BF)->f1;_tmp2C1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2BF)->f2;
_tmp2C2=_tmp2BE.targs;_LL236: if((_tmp2BA == _tmp2C0  || Cyc_Absyn_qvar_cmp(_tmp2BA->name,
_tmp2C0->name)== 0) && (_tmp2BB == _tmp2C1  || Cyc_Absyn_qvar_cmp(_tmp2BB->name,
_tmp2C1->name)== 0)){Cyc_Tcutil_unify_list(_tmp2C2,_tmp2BC);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp341="(different tunion field types)";_tag_dynforward(_tmp341,
sizeof(char),_get_zero_arr_size(_tmp341,31));});goto _LL226;_LL237: _tmp2C3=
_tmp291.f1;if(_tmp2C3 <= (void*)4)goto _LL239;if(*((int*)_tmp2C3)!= 4)goto _LL239;
_tmp2C4=((struct Cyc_Absyn_PointerType_struct*)_tmp2C3)->f1;_tmp2C5=(void*)
_tmp2C4.elt_typ;_tmp2C6=_tmp2C4.elt_tq;_tmp2C7=_tmp2C4.ptr_atts;_tmp2C8=(void*)
_tmp2C7.rgn;_tmp2C9=_tmp2C7.nullable;_tmp2CA=_tmp2C7.bounds;_tmp2CB=_tmp2C7.zero_term;
_tmp2CC=_tmp291.f2;if(_tmp2CC <= (void*)4)goto _LL239;if(*((int*)_tmp2CC)!= 4)goto
_LL239;_tmp2CD=((struct Cyc_Absyn_PointerType_struct*)_tmp2CC)->f1;_tmp2CE=(void*)
_tmp2CD.elt_typ;_tmp2CF=_tmp2CD.elt_tq;_tmp2D0=_tmp2CD.ptr_atts;_tmp2D1=(void*)
_tmp2D0.rgn;_tmp2D2=_tmp2D0.nullable;_tmp2D3=_tmp2D0.bounds;_tmp2D4=_tmp2D0.zero_term;
_LL238: Cyc_Tcutil_unify_it(_tmp2CE,_tmp2C5);Cyc_Tcutil_unify_it(_tmp2C8,_tmp2D1);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2D4,_tmp2CB,({const char*_tmp342="(not both zero terminated)";
_tag_dynforward(_tmp342,sizeof(char),_get_zero_arr_size(_tmp342,27));}));Cyc_Tcutil_unify_tqual(
_tmp2CF,_tmp2CE,_tmp2C6,_tmp2C5);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2D3,_tmp2CA,({const char*_tmp343="(different pointer bounds)";_tag_dynforward(
_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,27));}));{void*_tmp344=(void*)(
Cyc_Absyn_compress_conref(_tmp2D3))->v;void*_tmp345;void*_tmp346;_LL262: if(
_tmp344 <= (void*)1)goto _LL266;if(*((int*)_tmp344)!= 0)goto _LL264;_tmp345=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp344)->f1;if((int)_tmp345 != 0)goto _LL264;
_LL263: return;_LL264: if(*((int*)_tmp344)!= 0)goto _LL266;_tmp346=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp344)->f1;if((int)_tmp346 != 1)goto _LL266;_LL265:
return;_LL266:;_LL267: goto _LL261;_LL261:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2D2,_tmp2C9,({const char*_tmp347="(incompatible pointer types)";
_tag_dynforward(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,29));}));return;
_LL239: _tmp2D5=_tmp291.f1;if(_tmp2D5 <= (void*)4)goto _LL23B;if(*((int*)_tmp2D5)!= 
5)goto _LL23B;_tmp2D6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D5)->f1;
_tmp2D7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D5)->f2;_tmp2D8=_tmp291.f2;
if(_tmp2D8 <= (void*)4)goto _LL23B;if(*((int*)_tmp2D8)!= 5)goto _LL23B;_tmp2D9=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D8)->f1;_tmp2DA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2D8)->f2;_LL23A: if(_tmp2D9 == _tmp2D6  && ((_tmp2DA == _tmp2D7  || _tmp2DA == (
void*)2  && _tmp2D7 == (void*)3) || _tmp2DA == (void*)3  && _tmp2D7 == (void*)2))
return;Cyc_Tcutil_failure_reason=({const char*_tmp348="(different integral types)";
_tag_dynforward(_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,27));});goto
_LL226;_LL23B: _tmp2DB=_tmp291.f1;if((int)_tmp2DB != 1)goto _LL23D;_tmp2DC=_tmp291.f2;
if((int)_tmp2DC != 1)goto _LL23D;_LL23C: return;_LL23D: _tmp2DD=_tmp291.f1;if(
_tmp2DD <= (void*)4)goto _LL23F;if(*((int*)_tmp2DD)!= 6)goto _LL23F;_tmp2DE=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2DD)->f1;_tmp2DF=_tmp291.f2;if(_tmp2DF <= (
void*)4)goto _LL23F;if(*((int*)_tmp2DF)!= 6)goto _LL23F;_tmp2E0=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2DF)->f1;_LL23E: if(_tmp2DE == _tmp2E0)return;goto _LL226;_LL23F: _tmp2E1=
_tmp291.f1;if(_tmp2E1 <= (void*)4)goto _LL241;if(*((int*)_tmp2E1)!= 14)goto _LL241;
_tmp2E2=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2E1)->f1;_tmp2E3=_tmp291.f2;
if(_tmp2E3 <= (void*)4)goto _LL241;if(*((int*)_tmp2E3)!= 14)goto _LL241;_tmp2E4=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2E3)->f1;_LL240: Cyc_Tcutil_unify_it(
_tmp2E2,_tmp2E4);return;_LL241: _tmp2E5=_tmp291.f1;if(_tmp2E5 <= (void*)4)goto
_LL243;if(*((int*)_tmp2E5)!= 18)goto _LL243;_tmp2E6=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2E5)->f1;_tmp2E7=_tmp291.f2;if(_tmp2E7 <= (void*)4)goto _LL243;if(*((int*)
_tmp2E7)!= 18)goto _LL243;_tmp2E8=((struct Cyc_Absyn_TypeInt_struct*)_tmp2E7)->f1;
_LL242: if(_tmp2E6 == _tmp2E8)return;Cyc_Tcutil_failure_reason=({const char*_tmp349="(different type integers)";
_tag_dynforward(_tmp349,sizeof(char),_get_zero_arr_size(_tmp349,26));});goto
_LL226;_LL243: _tmp2E9=_tmp291.f1;if(_tmp2E9 <= (void*)4)goto _LL245;if(*((int*)
_tmp2E9)!= 17)goto _LL245;_tmp2EA=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2E9)->f1;_tmp2EB=_tmp291.f2;if(_tmp2EB <= (void*)4)goto _LL245;if(*((int*)
_tmp2EB)!= 17)goto _LL245;_tmp2EC=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2EB)->f1;_LL244: Cyc_Tcutil_unify_it(_tmp2EA,_tmp2EC);return;_LL245: _tmp2ED=
_tmp291.f1;if(_tmp2ED <= (void*)4)goto _LL247;if(*((int*)_tmp2ED)!= 7)goto _LL247;
_tmp2EE=((struct Cyc_Absyn_ArrayType_struct*)_tmp2ED)->f1;_tmp2EF=(void*)_tmp2EE.elt_type;
_tmp2F0=_tmp2EE.tq;_tmp2F1=_tmp2EE.num_elts;_tmp2F2=_tmp2EE.zero_term;_tmp2F3=
_tmp291.f2;if(_tmp2F3 <= (void*)4)goto _LL247;if(*((int*)_tmp2F3)!= 7)goto _LL247;
_tmp2F4=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F3)->f1;_tmp2F5=(void*)_tmp2F4.elt_type;
_tmp2F6=_tmp2F4.tq;_tmp2F7=_tmp2F4.num_elts;_tmp2F8=_tmp2F4.zero_term;_LL246: Cyc_Tcutil_unify_it(
_tmp2F5,_tmp2EF);Cyc_Tcutil_unify_tqual(_tmp2F6,_tmp2F5,_tmp2F0,_tmp2EF);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2F2,_tmp2F8,({const char*_tmp34A="(not both zero terminated)";
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,27));}));if(
_tmp2F1 == _tmp2F7)return;if(_tmp2F1 == 0  || _tmp2F7 == 0)goto _LL226;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp2F1,(struct Cyc_Absyn_Exp*)_tmp2F7))return;Cyc_Tcutil_failure_reason=({
const char*_tmp34B="(different array sizes)";_tag_dynforward(_tmp34B,sizeof(char),
_get_zero_arr_size(_tmp34B,24));});goto _LL226;_LL247: _tmp2F9=_tmp291.f1;if(
_tmp2F9 <= (void*)4)goto _LL249;if(*((int*)_tmp2F9)!= 8)goto _LL249;_tmp2FA=((
struct Cyc_Absyn_FnType_struct*)_tmp2F9)->f1;_tmp2FB=_tmp2FA.tvars;_tmp2FC=
_tmp2FA.effect;_tmp2FD=(void*)_tmp2FA.ret_typ;_tmp2FE=_tmp2FA.args;_tmp2FF=
_tmp2FA.c_varargs;_tmp300=_tmp2FA.cyc_varargs;_tmp301=_tmp2FA.rgn_po;_tmp302=
_tmp2FA.attributes;_tmp303=_tmp291.f2;if(_tmp303 <= (void*)4)goto _LL249;if(*((int*)
_tmp303)!= 8)goto _LL249;_tmp304=((struct Cyc_Absyn_FnType_struct*)_tmp303)->f1;
_tmp305=_tmp304.tvars;_tmp306=_tmp304.effect;_tmp307=(void*)_tmp304.ret_typ;
_tmp308=_tmp304.args;_tmp309=_tmp304.c_varargs;_tmp30A=_tmp304.cyc_varargs;
_tmp30B=_tmp304.rgn_po;_tmp30C=_tmp304.attributes;_LL248: {int done=0;{struct
_RegionHandle _tmp34C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp34C;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp305 != 0){if(_tmp2FB == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp34D="(second function type has too few type variables)";
_tag_dynforward(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,50));});(int)
_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp34E=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp305->hd);void*_tmp34F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2FB->hd);
if(_tmp34E != _tmp34F){Cyc_Tcutil_failure_reason=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp354;_tmp354.tag=0;_tmp354.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp34F));{struct Cyc_String_pa_struct
_tmp353;_tmp353.tag=0;_tmp353.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kind2string(_tmp34E));{struct Cyc_String_pa_struct _tmp352;_tmp352.tag=
0;_tmp352.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp305->hd));{void*_tmp350[3]={& _tmp352,& _tmp353,& _tmp354};
Cyc_aprintf(({const char*_tmp351="(type var %s has different kinds %s and %s)";
_tag_dynforward(_tmp351,sizeof(char),_get_zero_arr_size(_tmp351,44));}),
_tag_dynforward(_tmp350,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp355=_region_malloc(rgn,sizeof(*_tmp355));_tmp355->hd=({
struct _tuple8*_tmp356=_region_malloc(rgn,sizeof(*_tmp356));_tmp356->f1=(struct
Cyc_Absyn_Tvar*)_tmp2FB->hd;_tmp356->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357[0]=({struct Cyc_Absyn_VarType_struct
_tmp358;_tmp358.tag=1;_tmp358.f1=(struct Cyc_Absyn_Tvar*)_tmp305->hd;_tmp358;});
_tmp357;});_tmp356;});_tmp355->tl=inst;_tmp355;});_tmp305=_tmp305->tl;_tmp2FB=
_tmp2FB->tl;}}if(_tmp2FB != 0){Cyc_Tcutil_failure_reason=({const char*_tmp359="(second function type has too many type variables)";
_tag_dynforward(_tmp359,sizeof(char),_get_zero_arr_size(_tmp359,51));});
_npop_handler(0);goto _LL226;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A[0]=({struct Cyc_Absyn_FnType_struct
_tmp35B;_tmp35B.tag=8;_tmp35B.f1=({struct Cyc_Absyn_FnInfo _tmp35C;_tmp35C.tvars=0;
_tmp35C.effect=_tmp306;_tmp35C.ret_typ=(void*)_tmp307;_tmp35C.args=_tmp308;
_tmp35C.c_varargs=_tmp309;_tmp35C.cyc_varargs=_tmp30A;_tmp35C.rgn_po=_tmp30B;
_tmp35C.attributes=_tmp30C;_tmp35C;});_tmp35B;});_tmp35A;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp35D=_cycalloc(sizeof(*
_tmp35D));_tmp35D[0]=({struct Cyc_Absyn_FnType_struct _tmp35E;_tmp35E.tag=8;
_tmp35E.f1=({struct Cyc_Absyn_FnInfo _tmp35F;_tmp35F.tvars=0;_tmp35F.effect=
_tmp2FC;_tmp35F.ret_typ=(void*)_tmp2FD;_tmp35F.args=_tmp2FE;_tmp35F.c_varargs=
_tmp2FF;_tmp35F.cyc_varargs=_tmp300;_tmp35F.rgn_po=_tmp301;_tmp35F.attributes=
_tmp302;_tmp35F;});_tmp35E;});_tmp35D;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp307,_tmp2FD);for(0;_tmp308 != 0  && _tmp2FE != 0;(
_tmp308=_tmp308->tl,_tmp2FE=_tmp2FE->tl)){struct Cyc_Absyn_Tqual _tmp361;void*
_tmp362;struct _tuple2 _tmp360=*((struct _tuple2*)_tmp308->hd);_tmp361=_tmp360.f2;
_tmp362=_tmp360.f3;{struct Cyc_Absyn_Tqual _tmp364;void*_tmp365;struct _tuple2
_tmp363=*((struct _tuple2*)_tmp2FE->hd);_tmp364=_tmp363.f2;_tmp365=_tmp363.f3;Cyc_Tcutil_unify_it(
_tmp362,_tmp365);Cyc_Tcutil_unify_tqual(_tmp361,_tmp362,_tmp364,_tmp365);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp308 != 0  || _tmp2FE != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp366="(function types have different number of arguments)";
_tag_dynforward(_tmp366,sizeof(char),_get_zero_arr_size(_tmp366,52));});goto
_LL226;}if(_tmp309 != _tmp2FF){Cyc_Tcutil_failure_reason=({const char*_tmp367="(only one function type takes C varargs)";
_tag_dynforward(_tmp367,sizeof(char),_get_zero_arr_size(_tmp367,41));});goto
_LL226;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp369=({struct _tuple11 _tmp368;
_tmp368.f1=_tmp30A;_tmp368.f2=_tmp300;_tmp368;});struct Cyc_Absyn_VarargInfo*
_tmp36A;struct Cyc_Absyn_VarargInfo*_tmp36B;struct Cyc_Absyn_VarargInfo*_tmp36C;
struct Cyc_Absyn_VarargInfo*_tmp36D;struct Cyc_Absyn_VarargInfo*_tmp36E;struct Cyc_Absyn_VarargInfo
_tmp36F;struct Cyc_Core_Opt*_tmp370;struct Cyc_Absyn_Tqual _tmp371;void*_tmp372;int
_tmp373;struct Cyc_Absyn_VarargInfo*_tmp374;struct Cyc_Absyn_VarargInfo _tmp375;
struct Cyc_Core_Opt*_tmp376;struct Cyc_Absyn_Tqual _tmp377;void*_tmp378;int _tmp379;
_LL269: _tmp36A=_tmp369.f1;if(_tmp36A != 0)goto _LL26B;_tmp36B=_tmp369.f2;if(
_tmp36B != 0)goto _LL26B;_LL26A: goto _LL268;_LL26B: _tmp36C=_tmp369.f1;if(_tmp36C != 
0)goto _LL26D;_LL26C: goto _LL26E;_LL26D: _tmp36D=_tmp369.f2;if(_tmp36D != 0)goto
_LL26F;_LL26E: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp37A="(only one function type takes varargs)";
_tag_dynforward(_tmp37A,sizeof(char),_get_zero_arr_size(_tmp37A,39));});goto
_LL268;_LL26F: _tmp36E=_tmp369.f1;if(_tmp36E == 0)goto _LL268;_tmp36F=*_tmp36E;
_tmp370=_tmp36F.name;_tmp371=_tmp36F.tq;_tmp372=(void*)_tmp36F.type;_tmp373=
_tmp36F.inject;_tmp374=_tmp369.f2;if(_tmp374 == 0)goto _LL268;_tmp375=*_tmp374;
_tmp376=_tmp375.name;_tmp377=_tmp375.tq;_tmp378=(void*)_tmp375.type;_tmp379=
_tmp375.inject;_LL270: Cyc_Tcutil_unify_it(_tmp372,_tmp378);Cyc_Tcutil_unify_tqual(
_tmp371,_tmp372,_tmp377,_tmp378);if(_tmp373 != _tmp379){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({
const char*_tmp37B="(only one function type injects varargs)";_tag_dynforward(
_tmp37B,sizeof(char),_get_zero_arr_size(_tmp37B,41));});}goto _LL268;_LL268:;}if(
bad_cyc_vararg)goto _LL226;{int bad_effect=0;{struct _tuple12 _tmp37D=({struct
_tuple12 _tmp37C;_tmp37C.f1=_tmp306;_tmp37C.f2=_tmp2FC;_tmp37C;});struct Cyc_Core_Opt*
_tmp37E;struct Cyc_Core_Opt*_tmp37F;struct Cyc_Core_Opt*_tmp380;struct Cyc_Core_Opt*
_tmp381;_LL272: _tmp37E=_tmp37D.f1;if(_tmp37E != 0)goto _LL274;_tmp37F=_tmp37D.f2;
if(_tmp37F != 0)goto _LL274;_LL273: goto _LL271;_LL274: _tmp380=_tmp37D.f1;if(_tmp380
!= 0)goto _LL276;_LL275: goto _LL277;_LL276: _tmp381=_tmp37D.f2;if(_tmp381 != 0)goto
_LL278;_LL277: bad_effect=1;goto _LL271;_LL278:;_LL279: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp306))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2FC))->v);goto _LL271;_LL271:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp382="(function type effects do not unify)";
_tag_dynforward(_tmp382,sizeof(char),_get_zero_arr_size(_tmp382,37));});goto
_LL226;}if(!Cyc_Tcutil_same_atts(_tmp302,_tmp30C)){Cyc_Tcutil_failure_reason=({
const char*_tmp383="(function types have different attributes)";_tag_dynforward(
_tmp383,sizeof(char),_get_zero_arr_size(_tmp383,43));});goto _LL226;}if(!Cyc_Tcutil_same_rgn_po(
_tmp301,_tmp30B)){Cyc_Tcutil_failure_reason=({const char*_tmp384="(function types have different region lifetime orderings)";
_tag_dynforward(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,58));});goto
_LL226;}return;}}}_LL249: _tmp30D=_tmp291.f1;if(_tmp30D <= (void*)4)goto _LL24B;if(*((
int*)_tmp30D)!= 9)goto _LL24B;_tmp30E=((struct Cyc_Absyn_TupleType_struct*)_tmp30D)->f1;
_tmp30F=_tmp291.f2;if(_tmp30F <= (void*)4)goto _LL24B;if(*((int*)_tmp30F)!= 9)goto
_LL24B;_tmp310=((struct Cyc_Absyn_TupleType_struct*)_tmp30F)->f1;_LL24A: for(0;
_tmp310 != 0  && _tmp30E != 0;(_tmp310=_tmp310->tl,_tmp30E=_tmp30E->tl)){struct Cyc_Absyn_Tqual
_tmp386;void*_tmp387;struct _tuple4 _tmp385=*((struct _tuple4*)_tmp310->hd);_tmp386=
_tmp385.f1;_tmp387=_tmp385.f2;{struct Cyc_Absyn_Tqual _tmp389;void*_tmp38A;struct
_tuple4 _tmp388=*((struct _tuple4*)_tmp30E->hd);_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;
Cyc_Tcutil_unify_it(_tmp387,_tmp38A);Cyc_Tcutil_unify_tqual(_tmp386,_tmp387,
_tmp389,_tmp38A);}}if(_tmp310 == 0  && _tmp30E == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp38B="(tuple types have different numbers of components)";
_tag_dynforward(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,51));});goto
_LL226;_LL24B: _tmp311=_tmp291.f1;if(_tmp311 <= (void*)4)goto _LL24D;if(*((int*)
_tmp311)!= 11)goto _LL24D;_tmp312=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp311)->f1;_tmp313=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp311)->f2;_tmp314=
_tmp291.f2;if(_tmp314 <= (void*)4)goto _LL24D;if(*((int*)_tmp314)!= 11)goto _LL24D;
_tmp315=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp314)->f1;_tmp316=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp314)->f2;_LL24C: if(_tmp315 != _tmp312){
Cyc_Tcutil_failure_reason=({const char*_tmp38C="(struct and union type)";
_tag_dynforward(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,24));});goto
_LL226;}for(0;_tmp316 != 0  && _tmp313 != 0;(_tmp316=_tmp316->tl,_tmp313=_tmp313->tl)){
struct Cyc_Absyn_Aggrfield*_tmp38D=(struct Cyc_Absyn_Aggrfield*)_tmp316->hd;struct
Cyc_Absyn_Aggrfield*_tmp38E=(struct Cyc_Absyn_Aggrfield*)_tmp313->hd;if(Cyc_strptrcmp(
_tmp38D->name,_tmp38E->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp38F="(different member names)";
_tag_dynforward(_tmp38F,sizeof(char),_get_zero_arr_size(_tmp38F,25));});(int)
_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp38D->type,(void*)
_tmp38E->type);Cyc_Tcutil_unify_tqual(_tmp38D->tq,(void*)_tmp38D->type,_tmp38E->tq,(
void*)_tmp38E->type);if(!Cyc_Tcutil_same_atts(_tmp38D->attributes,_tmp38E->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp390="(different attributes on member)";_tag_dynforward(_tmp390,
sizeof(char),_get_zero_arr_size(_tmp390,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp38D->width != 0  && _tmp38E->width == 0  || _tmp38E->width != 0  && _tmp38D->width
== 0) || (_tmp38D->width != 0  && _tmp38E->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp38D->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp38E->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp391="(different bitfield widths on member)";
_tag_dynforward(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,38));});(int)
_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp316 == 0  && _tmp313 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp392="(different number of members)";
_tag_dynforward(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,30));});goto
_LL226;_LL24D: _tmp317=_tmp291.f1;if((int)_tmp317 != 2)goto _LL24F;_tmp318=_tmp291.f2;
if((int)_tmp318 != 2)goto _LL24F;_LL24E: return;_LL24F: _tmp319=_tmp291.f1;if((int)
_tmp319 != 3)goto _LL251;_tmp31A=_tmp291.f2;if((int)_tmp31A != 3)goto _LL251;_LL250:
return;_LL251: _tmp31B=_tmp291.f1;if(_tmp31B <= (void*)4)goto _LL253;if(*((int*)
_tmp31B)!= 15)goto _LL253;_tmp31C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp31B)->f1;_tmp31D=_tmp291.f2;if(_tmp31D <= (void*)4)goto _LL253;if(*((int*)
_tmp31D)!= 15)goto _LL253;_tmp31E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp31D)->f1;_LL252: Cyc_Tcutil_unify_it(_tmp31C,_tmp31E);return;_LL253: _tmp31F=
_tmp291.f1;if(_tmp31F <= (void*)4)goto _LL255;if(*((int*)_tmp31F)!= 20)goto _LL255;
_LL254: goto _LL256;_LL255: _tmp320=_tmp291.f2;if(_tmp320 <= (void*)4)goto _LL257;if(*((
int*)_tmp320)!= 20)goto _LL257;_LL256: goto _LL258;_LL257: _tmp321=_tmp291.f1;if(
_tmp321 <= (void*)4)goto _LL259;if(*((int*)_tmp321)!= 19)goto _LL259;_LL258: goto
_LL25A;_LL259: _tmp322=_tmp291.f1;if(_tmp322 <= (void*)4)goto _LL25B;if(*((int*)
_tmp322)!= 21)goto _LL25B;_LL25A: goto _LL25C;_LL25B: _tmp323=_tmp291.f2;if(_tmp323
<= (void*)4)goto _LL25D;if(*((int*)_tmp323)!= 21)goto _LL25D;_LL25C: goto _LL25E;
_LL25D: _tmp324=_tmp291.f2;if(_tmp324 <= (void*)4)goto _LL25F;if(*((int*)_tmp324)!= 
19)goto _LL25F;_LL25E: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp393="(effects don't unify)";_tag_dynforward(_tmp393,sizeof(char),
_get_zero_arr_size(_tmp393,22));});goto _LL226;_LL25F:;_LL260: goto _LL226;_LL226:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp394=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp395=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp394,_tmp395);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp396=(void*)x->v;void*
_tmp397;_LL27B: if((int)_tmp396 != 0)goto _LL27D;_LL27C: return - 1;_LL27D: if(_tmp396
<= (void*)1)goto _LL27F;if(*((int*)_tmp396)!= 0)goto _LL27F;_tmp397=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp396)->f1;_LL27E: {void*_tmp398=(void*)y->v;
void*_tmp399;_LL282: if((int)_tmp398 != 0)goto _LL284;_LL283: return 1;_LL284: if(
_tmp398 <= (void*)1)goto _LL286;if(*((int*)_tmp398)!= 0)goto _LL286;_tmp399=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp398)->f1;_LL285: return cmp(_tmp397,_tmp399);
_LL286: if(_tmp398 <= (void*)1)goto _LL281;if(*((int*)_tmp398)!= 1)goto _LL281;
_LL287:({void*_tmp39A[0]={};Cyc_Tcutil_impos(({const char*_tmp39B="unify_conref: forward after compress(2)";
_tag_dynforward(_tmp39B,sizeof(char),_get_zero_arr_size(_tmp39B,40));}),
_tag_dynforward(_tmp39A,sizeof(void*),0));});_LL281:;}_LL27F: if(_tmp396 <= (void*)
1)goto _LL27A;if(*((int*)_tmp396)!= 1)goto _LL27A;_LL280:({void*_tmp39C[0]={};Cyc_Tcutil_impos(({
const char*_tmp39D="unify_conref: forward after compress";_tag_dynforward(_tmp39D,
sizeof(char),_get_zero_arr_size(_tmp39D,37));}),_tag_dynforward(_tmp39C,sizeof(
void*),0));});_LL27A:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple4*tqt1,
struct _tuple4*tqt2){struct _tuple4 _tmp39F;struct Cyc_Absyn_Tqual _tmp3A0;void*
_tmp3A1;struct _tuple4*_tmp39E=tqt1;_tmp39F=*_tmp39E;_tmp3A0=_tmp39F.f1;_tmp3A1=
_tmp39F.f2;{struct _tuple4 _tmp3A3;struct Cyc_Absyn_Tqual _tmp3A4;void*_tmp3A5;
struct _tuple4*_tmp3A2=tqt2;_tmp3A3=*_tmp3A2;_tmp3A4=_tmp3A3.f1;_tmp3A5=_tmp3A3.f2;{
int _tmp3A6=Cyc_Tcutil_tqual_cmp(_tmp3A0,_tmp3A4);if(_tmp3A6 != 0)return _tmp3A6;
return Cyc_Tcutil_typecmp(_tmp3A1,_tmp3A5);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp3A7=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp3A7 != 0)return _tmp3A7;{int _tmp3A8=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp3A8 != 0)return _tmp3A8;{int _tmp3A9=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp3A9 != 0)return _tmp3A9;{int _tmp3AA=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp3AA != 0)return
_tmp3AA;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3AB=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3AB != 0)return _tmp3AB;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3AC=t;_LL289: if((int)_tmp3AC != 0)goto _LL28B;_LL28A: return 0;
_LL28B: if(_tmp3AC <= (void*)4)goto _LL297;if(*((int*)_tmp3AC)!= 0)goto _LL28D;
_LL28C: return 1;_LL28D: if(*((int*)_tmp3AC)!= 1)goto _LL28F;_LL28E: return 2;_LL28F:
if(*((int*)_tmp3AC)!= 2)goto _LL291;_LL290: return 3;_LL291: if(*((int*)_tmp3AC)!= 3)
goto _LL293;_LL292: return 4;_LL293: if(*((int*)_tmp3AC)!= 4)goto _LL295;_LL294:
return 5;_LL295: if(*((int*)_tmp3AC)!= 5)goto _LL297;_LL296: return 6;_LL297: if((int)
_tmp3AC != 1)goto _LL299;_LL298: return 7;_LL299: if(_tmp3AC <= (void*)4)goto _LL2AD;
if(*((int*)_tmp3AC)!= 6)goto _LL29B;_LL29A: return 8;_LL29B: if(*((int*)_tmp3AC)!= 7)
goto _LL29D;_LL29C: return 9;_LL29D: if(*((int*)_tmp3AC)!= 8)goto _LL29F;_LL29E:
return 10;_LL29F: if(*((int*)_tmp3AC)!= 9)goto _LL2A1;_LL2A0: return 11;_LL2A1: if(*((
int*)_tmp3AC)!= 10)goto _LL2A3;_LL2A2: return 12;_LL2A3: if(*((int*)_tmp3AC)!= 11)
goto _LL2A5;_LL2A4: return 14;_LL2A5: if(*((int*)_tmp3AC)!= 12)goto _LL2A7;_LL2A6:
return 16;_LL2A7: if(*((int*)_tmp3AC)!= 13)goto _LL2A9;_LL2A8: return 17;_LL2A9: if(*((
int*)_tmp3AC)!= 15)goto _LL2AB;_LL2AA: return 18;_LL2AB: if(*((int*)_tmp3AC)!= 16)
goto _LL2AD;_LL2AC: return 19;_LL2AD: if((int)_tmp3AC != 3)goto _LL2AF;_LL2AE: return 20;
_LL2AF: if((int)_tmp3AC != 2)goto _LL2B1;_LL2B0: return 21;_LL2B1: if(_tmp3AC <= (void*)
4)goto _LL2B3;if(*((int*)_tmp3AC)!= 19)goto _LL2B3;_LL2B2: return 22;_LL2B3: if(
_tmp3AC <= (void*)4)goto _LL2B5;if(*((int*)_tmp3AC)!= 20)goto _LL2B5;_LL2B4: return
23;_LL2B5: if(_tmp3AC <= (void*)4)goto _LL2B7;if(*((int*)_tmp3AC)!= 21)goto _LL2B7;
_LL2B6: return 24;_LL2B7: if(_tmp3AC <= (void*)4)goto _LL2B9;if(*((int*)_tmp3AC)!= 14)
goto _LL2B9;_LL2B8: return 25;_LL2B9: if(_tmp3AC <= (void*)4)goto _LL2BB;if(*((int*)
_tmp3AC)!= 18)goto _LL2BB;_LL2BA: return 26;_LL2BB: if(_tmp3AC <= (void*)4)goto _LL288;
if(*((int*)_tmp3AC)!= 17)goto _LL288;_LL2BC: return 27;_LL288:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3AD=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3AD != 0)return _tmp3AD;{struct _tuple6 _tmp3AF=({struct _tuple6 _tmp3AE;
_tmp3AE.f1=t2;_tmp3AE.f2=t1;_tmp3AE;});void*_tmp3B0;void*_tmp3B1;void*_tmp3B2;
struct Cyc_Absyn_Tvar*_tmp3B3;void*_tmp3B4;struct Cyc_Absyn_Tvar*_tmp3B5;void*
_tmp3B6;struct Cyc_Absyn_AggrInfo _tmp3B7;void*_tmp3B8;struct Cyc_List_List*_tmp3B9;
void*_tmp3BA;struct Cyc_Absyn_AggrInfo _tmp3BB;void*_tmp3BC;struct Cyc_List_List*
_tmp3BD;void*_tmp3BE;struct _tuple1*_tmp3BF;void*_tmp3C0;struct _tuple1*_tmp3C1;
void*_tmp3C2;struct Cyc_List_List*_tmp3C3;void*_tmp3C4;struct Cyc_List_List*
_tmp3C5;void*_tmp3C6;struct Cyc_Absyn_TunionInfo _tmp3C7;void*_tmp3C8;struct Cyc_Absyn_Tuniondecl**
_tmp3C9;struct Cyc_Absyn_Tuniondecl*_tmp3CA;struct Cyc_List_List*_tmp3CB;void*
_tmp3CC;void*_tmp3CD;struct Cyc_Absyn_TunionInfo _tmp3CE;void*_tmp3CF;struct Cyc_Absyn_Tuniondecl**
_tmp3D0;struct Cyc_Absyn_Tuniondecl*_tmp3D1;struct Cyc_List_List*_tmp3D2;void*
_tmp3D3;void*_tmp3D4;struct Cyc_Absyn_TunionFieldInfo _tmp3D5;void*_tmp3D6;struct
Cyc_Absyn_Tuniondecl*_tmp3D7;struct Cyc_Absyn_Tunionfield*_tmp3D8;struct Cyc_List_List*
_tmp3D9;void*_tmp3DA;struct Cyc_Absyn_TunionFieldInfo _tmp3DB;void*_tmp3DC;struct
Cyc_Absyn_Tuniondecl*_tmp3DD;struct Cyc_Absyn_Tunionfield*_tmp3DE;struct Cyc_List_List*
_tmp3DF;void*_tmp3E0;struct Cyc_Absyn_PtrInfo _tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Tqual
_tmp3E3;struct Cyc_Absyn_PtrAtts _tmp3E4;void*_tmp3E5;struct Cyc_Absyn_Conref*
_tmp3E6;struct Cyc_Absyn_Conref*_tmp3E7;struct Cyc_Absyn_Conref*_tmp3E8;void*
_tmp3E9;struct Cyc_Absyn_PtrInfo _tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Tqual _tmp3EC;
struct Cyc_Absyn_PtrAtts _tmp3ED;void*_tmp3EE;struct Cyc_Absyn_Conref*_tmp3EF;
struct Cyc_Absyn_Conref*_tmp3F0;struct Cyc_Absyn_Conref*_tmp3F1;void*_tmp3F2;void*
_tmp3F3;void*_tmp3F4;void*_tmp3F5;void*_tmp3F6;void*_tmp3F7;void*_tmp3F8;void*
_tmp3F9;void*_tmp3FA;int _tmp3FB;void*_tmp3FC;int _tmp3FD;void*_tmp3FE;struct Cyc_Absyn_ArrayInfo
_tmp3FF;void*_tmp400;struct Cyc_Absyn_Tqual _tmp401;struct Cyc_Absyn_Exp*_tmp402;
struct Cyc_Absyn_Conref*_tmp403;void*_tmp404;struct Cyc_Absyn_ArrayInfo _tmp405;
void*_tmp406;struct Cyc_Absyn_Tqual _tmp407;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Conref*
_tmp409;void*_tmp40A;struct Cyc_Absyn_FnInfo _tmp40B;struct Cyc_List_List*_tmp40C;
struct Cyc_Core_Opt*_tmp40D;void*_tmp40E;struct Cyc_List_List*_tmp40F;int _tmp410;
struct Cyc_Absyn_VarargInfo*_tmp411;struct Cyc_List_List*_tmp412;struct Cyc_List_List*
_tmp413;void*_tmp414;struct Cyc_Absyn_FnInfo _tmp415;struct Cyc_List_List*_tmp416;
struct Cyc_Core_Opt*_tmp417;void*_tmp418;struct Cyc_List_List*_tmp419;int _tmp41A;
struct Cyc_Absyn_VarargInfo*_tmp41B;struct Cyc_List_List*_tmp41C;struct Cyc_List_List*
_tmp41D;void*_tmp41E;struct Cyc_List_List*_tmp41F;void*_tmp420;struct Cyc_List_List*
_tmp421;void*_tmp422;void*_tmp423;struct Cyc_List_List*_tmp424;void*_tmp425;void*
_tmp426;struct Cyc_List_List*_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;void*
_tmp42B;void*_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;void*
_tmp431;void*_tmp432;void*_tmp433;void*_tmp434;int _tmp435;void*_tmp436;int
_tmp437;void*_tmp438;void*_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;void*
_tmp43D;_LL2BE: _tmp3B0=_tmp3AF.f1;if(_tmp3B0 <= (void*)4)goto _LL2C0;if(*((int*)
_tmp3B0)!= 0)goto _LL2C0;_tmp3B1=_tmp3AF.f2;if(_tmp3B1 <= (void*)4)goto _LL2C0;if(*((
int*)_tmp3B1)!= 0)goto _LL2C0;_LL2BF:({void*_tmp43E[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp43F="typecmp: can only compare closed types";_tag_dynforward(_tmp43F,sizeof(
char),_get_zero_arr_size(_tmp43F,39));}),_tag_dynforward(_tmp43E,sizeof(void*),0));});
_LL2C0: _tmp3B2=_tmp3AF.f1;if(_tmp3B2 <= (void*)4)goto _LL2C2;if(*((int*)_tmp3B2)!= 
1)goto _LL2C2;_tmp3B3=((struct Cyc_Absyn_VarType_struct*)_tmp3B2)->f1;_tmp3B4=
_tmp3AF.f2;if(_tmp3B4 <= (void*)4)goto _LL2C2;if(*((int*)_tmp3B4)!= 1)goto _LL2C2;
_tmp3B5=((struct Cyc_Absyn_VarType_struct*)_tmp3B4)->f1;_LL2C1: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp3B5->identity)),*((int*)_check_null(_tmp3B3->identity)));
_LL2C2: _tmp3B6=_tmp3AF.f1;if(_tmp3B6 <= (void*)4)goto _LL2C4;if(*((int*)_tmp3B6)!= 
10)goto _LL2C4;_tmp3B7=((struct Cyc_Absyn_AggrType_struct*)_tmp3B6)->f1;_tmp3B8=(
void*)_tmp3B7.aggr_info;_tmp3B9=_tmp3B7.targs;_tmp3BA=_tmp3AF.f2;if(_tmp3BA <= (
void*)4)goto _LL2C4;if(*((int*)_tmp3BA)!= 10)goto _LL2C4;_tmp3BB=((struct Cyc_Absyn_AggrType_struct*)
_tmp3BA)->f1;_tmp3BC=(void*)_tmp3BB.aggr_info;_tmp3BD=_tmp3BB.targs;_LL2C3: {
struct _tuple1*_tmp441;struct _tuple5 _tmp440=Cyc_Absyn_aggr_kinded_name(_tmp3B8);
_tmp441=_tmp440.f2;{struct _tuple1*_tmp443;struct _tuple5 _tmp442=Cyc_Absyn_aggr_kinded_name(
_tmp3BC);_tmp443=_tmp442.f2;{int _tmp444=Cyc_Absyn_qvar_cmp(_tmp441,_tmp443);if(
_tmp444 != 0)return _tmp444;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3B9,_tmp3BD);}}}}_LL2C4: _tmp3BE=_tmp3AF.f1;if(_tmp3BE <= (void*)4)goto _LL2C6;
if(*((int*)_tmp3BE)!= 12)goto _LL2C6;_tmp3BF=((struct Cyc_Absyn_EnumType_struct*)
_tmp3BE)->f1;_tmp3C0=_tmp3AF.f2;if(_tmp3C0 <= (void*)4)goto _LL2C6;if(*((int*)
_tmp3C0)!= 12)goto _LL2C6;_tmp3C1=((struct Cyc_Absyn_EnumType_struct*)_tmp3C0)->f1;
_LL2C5: return Cyc_Absyn_qvar_cmp(_tmp3BF,_tmp3C1);_LL2C6: _tmp3C2=_tmp3AF.f1;if(
_tmp3C2 <= (void*)4)goto _LL2C8;if(*((int*)_tmp3C2)!= 13)goto _LL2C8;_tmp3C3=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3C2)->f1;_tmp3C4=_tmp3AF.f2;if(_tmp3C4
<= (void*)4)goto _LL2C8;if(*((int*)_tmp3C4)!= 13)goto _LL2C8;_tmp3C5=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3C4)->f1;_LL2C7: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3C3,_tmp3C5);_LL2C8: _tmp3C6=_tmp3AF.f1;if(_tmp3C6 <= (
void*)4)goto _LL2CA;if(*((int*)_tmp3C6)!= 2)goto _LL2CA;_tmp3C7=((struct Cyc_Absyn_TunionType_struct*)
_tmp3C6)->f1;_tmp3C8=(void*)_tmp3C7.tunion_info;if(*((int*)_tmp3C8)!= 1)goto
_LL2CA;_tmp3C9=((struct Cyc_Absyn_KnownTunion_struct*)_tmp3C8)->f1;_tmp3CA=*
_tmp3C9;_tmp3CB=_tmp3C7.targs;_tmp3CC=(void*)_tmp3C7.rgn;_tmp3CD=_tmp3AF.f2;if(
_tmp3CD <= (void*)4)goto _LL2CA;if(*((int*)_tmp3CD)!= 2)goto _LL2CA;_tmp3CE=((
struct Cyc_Absyn_TunionType_struct*)_tmp3CD)->f1;_tmp3CF=(void*)_tmp3CE.tunion_info;
if(*((int*)_tmp3CF)!= 1)goto _LL2CA;_tmp3D0=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3CF)->f1;_tmp3D1=*_tmp3D0;_tmp3D2=_tmp3CE.targs;_tmp3D3=(void*)_tmp3CE.rgn;
_LL2C9: if(_tmp3D1 == _tmp3CA)return 0;{int _tmp445=Cyc_Absyn_qvar_cmp(_tmp3D1->name,
_tmp3CA->name);if(_tmp445 != 0)return _tmp445;{int _tmp446=Cyc_Tcutil_typecmp(
_tmp3D3,_tmp3CC);if(_tmp446 != 0)return _tmp446;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3D2,_tmp3CB);}}_LL2CA: _tmp3D4=_tmp3AF.f1;if(_tmp3D4 <= (void*)4)goto _LL2CC;
if(*((int*)_tmp3D4)!= 3)goto _LL2CC;_tmp3D5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3D4)->f1;_tmp3D6=(void*)_tmp3D5.field_info;if(*((int*)_tmp3D6)!= 1)goto
_LL2CC;_tmp3D7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D6)->f1;_tmp3D8=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D6)->f2;_tmp3D9=_tmp3D5.targs;
_tmp3DA=_tmp3AF.f2;if(_tmp3DA <= (void*)4)goto _LL2CC;if(*((int*)_tmp3DA)!= 3)goto
_LL2CC;_tmp3DB=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3DA)->f1;_tmp3DC=(
void*)_tmp3DB.field_info;if(*((int*)_tmp3DC)!= 1)goto _LL2CC;_tmp3DD=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3DC)->f1;_tmp3DE=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3DC)->f2;
_tmp3DF=_tmp3DB.targs;_LL2CB: if(_tmp3DD == _tmp3D7)return 0;{int _tmp447=Cyc_Absyn_qvar_cmp(
_tmp3D7->name,_tmp3DD->name);if(_tmp447 != 0)return _tmp447;{int _tmp448=Cyc_Absyn_qvar_cmp(
_tmp3D8->name,_tmp3DE->name);if(_tmp448 != 0)return _tmp448;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3DF,_tmp3D9);}}_LL2CC: _tmp3E0=_tmp3AF.f1;if(_tmp3E0 <= (
void*)4)goto _LL2CE;if(*((int*)_tmp3E0)!= 4)goto _LL2CE;_tmp3E1=((struct Cyc_Absyn_PointerType_struct*)
_tmp3E0)->f1;_tmp3E2=(void*)_tmp3E1.elt_typ;_tmp3E3=_tmp3E1.elt_tq;_tmp3E4=
_tmp3E1.ptr_atts;_tmp3E5=(void*)_tmp3E4.rgn;_tmp3E6=_tmp3E4.nullable;_tmp3E7=
_tmp3E4.bounds;_tmp3E8=_tmp3E4.zero_term;_tmp3E9=_tmp3AF.f2;if(_tmp3E9 <= (void*)
4)goto _LL2CE;if(*((int*)_tmp3E9)!= 4)goto _LL2CE;_tmp3EA=((struct Cyc_Absyn_PointerType_struct*)
_tmp3E9)->f1;_tmp3EB=(void*)_tmp3EA.elt_typ;_tmp3EC=_tmp3EA.elt_tq;_tmp3ED=
_tmp3EA.ptr_atts;_tmp3EE=(void*)_tmp3ED.rgn;_tmp3EF=_tmp3ED.nullable;_tmp3F0=
_tmp3ED.bounds;_tmp3F1=_tmp3ED.zero_term;_LL2CD: {int _tmp449=Cyc_Tcutil_typecmp(
_tmp3EB,_tmp3E2);if(_tmp449 != 0)return _tmp449;{int _tmp44A=Cyc_Tcutil_typecmp(
_tmp3EE,_tmp3E5);if(_tmp44A != 0)return _tmp44A;{int _tmp44B=Cyc_Tcutil_tqual_cmp(
_tmp3EC,_tmp3E3);if(_tmp44B != 0)return _tmp44B;{int _tmp44C=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3F0,_tmp3E7);if(_tmp44C != 0)return _tmp44C;{int _tmp44D=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3F1,_tmp3E8);if(_tmp44D != 0)return _tmp44D;{void*_tmp44E=(
void*)(Cyc_Absyn_compress_conref(_tmp3F0))->v;void*_tmp44F;void*_tmp450;_LL2F3:
if(_tmp44E <= (void*)1)goto _LL2F7;if(*((int*)_tmp44E)!= 0)goto _LL2F5;_tmp44F=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp44E)->f1;if((int)_tmp44F != 0)goto
_LL2F5;_LL2F4: return 0;_LL2F5: if(*((int*)_tmp44E)!= 0)goto _LL2F7;_tmp450=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp44E)->f1;if((int)_tmp450 != 1)goto _LL2F7;
_LL2F6: return 0;_LL2F7:;_LL2F8: goto _LL2F2;_LL2F2:;}return((int(*)(int(*cmp)(int,
int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3EF,_tmp3E6);}}}}}_LL2CE: _tmp3F2=_tmp3AF.f1;if(_tmp3F2 <= (
void*)4)goto _LL2D0;if(*((int*)_tmp3F2)!= 5)goto _LL2D0;_tmp3F3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3F2)->f1;_tmp3F4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F2)->f2;
_tmp3F5=_tmp3AF.f2;if(_tmp3F5 <= (void*)4)goto _LL2D0;if(*((int*)_tmp3F5)!= 5)goto
_LL2D0;_tmp3F6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F5)->f1;_tmp3F7=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F5)->f2;_LL2CF: if(_tmp3F6 != _tmp3F3)
return Cyc_Core_intcmp((int)((unsigned int)_tmp3F6),(int)((unsigned int)_tmp3F3));
if(_tmp3F7 != _tmp3F4)return Cyc_Core_intcmp((int)((unsigned int)_tmp3F7),(int)((
unsigned int)_tmp3F4));return 0;_LL2D0: _tmp3F8=_tmp3AF.f1;if((int)_tmp3F8 != 1)
goto _LL2D2;_tmp3F9=_tmp3AF.f2;if((int)_tmp3F9 != 1)goto _LL2D2;_LL2D1: return 0;
_LL2D2: _tmp3FA=_tmp3AF.f1;if(_tmp3FA <= (void*)4)goto _LL2D4;if(*((int*)_tmp3FA)!= 
6)goto _LL2D4;_tmp3FB=((struct Cyc_Absyn_DoubleType_struct*)_tmp3FA)->f1;_tmp3FC=
_tmp3AF.f2;if(_tmp3FC <= (void*)4)goto _LL2D4;if(*((int*)_tmp3FC)!= 6)goto _LL2D4;
_tmp3FD=((struct Cyc_Absyn_DoubleType_struct*)_tmp3FC)->f1;_LL2D3: if(_tmp3FB == 
_tmp3FD)return 0;else{if(_tmp3FB)return - 1;else{return 1;}}_LL2D4: _tmp3FE=_tmp3AF.f1;
if(_tmp3FE <= (void*)4)goto _LL2D6;if(*((int*)_tmp3FE)!= 7)goto _LL2D6;_tmp3FF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FE)->f1;_tmp400=(void*)_tmp3FF.elt_type;
_tmp401=_tmp3FF.tq;_tmp402=_tmp3FF.num_elts;_tmp403=_tmp3FF.zero_term;_tmp404=
_tmp3AF.f2;if(_tmp404 <= (void*)4)goto _LL2D6;if(*((int*)_tmp404)!= 7)goto _LL2D6;
_tmp405=((struct Cyc_Absyn_ArrayType_struct*)_tmp404)->f1;_tmp406=(void*)_tmp405.elt_type;
_tmp407=_tmp405.tq;_tmp408=_tmp405.num_elts;_tmp409=_tmp405.zero_term;_LL2D5: {
int _tmp451=Cyc_Tcutil_tqual_cmp(_tmp407,_tmp401);if(_tmp451 != 0)return _tmp451;{
int _tmp452=Cyc_Tcutil_typecmp(_tmp406,_tmp400);if(_tmp452 != 0)return _tmp452;{int
_tmp453=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp403,_tmp409);if(_tmp453 != 0)
return _tmp453;if(_tmp402 == _tmp408)return 0;if(_tmp402 == 0  || _tmp408 == 0)({void*
_tmp454[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp455="missing expression in array index";_tag_dynforward(_tmp455,
sizeof(char),_get_zero_arr_size(_tmp455,34));}),_tag_dynforward(_tmp454,sizeof(
void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp402,_tmp408);}}}_LL2D6: _tmp40A=_tmp3AF.f1;if(_tmp40A <= (void*)4)goto _LL2D8;
if(*((int*)_tmp40A)!= 8)goto _LL2D8;_tmp40B=((struct Cyc_Absyn_FnType_struct*)
_tmp40A)->f1;_tmp40C=_tmp40B.tvars;_tmp40D=_tmp40B.effect;_tmp40E=(void*)_tmp40B.ret_typ;
_tmp40F=_tmp40B.args;_tmp410=_tmp40B.c_varargs;_tmp411=_tmp40B.cyc_varargs;
_tmp412=_tmp40B.rgn_po;_tmp413=_tmp40B.attributes;_tmp414=_tmp3AF.f2;if(_tmp414
<= (void*)4)goto _LL2D8;if(*((int*)_tmp414)!= 8)goto _LL2D8;_tmp415=((struct Cyc_Absyn_FnType_struct*)
_tmp414)->f1;_tmp416=_tmp415.tvars;_tmp417=_tmp415.effect;_tmp418=(void*)_tmp415.ret_typ;
_tmp419=_tmp415.args;_tmp41A=_tmp415.c_varargs;_tmp41B=_tmp415.cyc_varargs;
_tmp41C=_tmp415.rgn_po;_tmp41D=_tmp415.attributes;_LL2D7:({void*_tmp456[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp457="typecmp: function types not handled";_tag_dynforward(_tmp457,
sizeof(char),_get_zero_arr_size(_tmp457,36));}),_tag_dynforward(_tmp456,sizeof(
void*),0));});_LL2D8: _tmp41E=_tmp3AF.f1;if(_tmp41E <= (void*)4)goto _LL2DA;if(*((
int*)_tmp41E)!= 9)goto _LL2DA;_tmp41F=((struct Cyc_Absyn_TupleType_struct*)_tmp41E)->f1;
_tmp420=_tmp3AF.f2;if(_tmp420 <= (void*)4)goto _LL2DA;if(*((int*)_tmp420)!= 9)goto
_LL2DA;_tmp421=((struct Cyc_Absyn_TupleType_struct*)_tmp420)->f1;_LL2D9: return((
int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp421,_tmp41F);_LL2DA: _tmp422=
_tmp3AF.f1;if(_tmp422 <= (void*)4)goto _LL2DC;if(*((int*)_tmp422)!= 11)goto _LL2DC;
_tmp423=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp422)->f1;_tmp424=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp422)->f2;_tmp425=_tmp3AF.f2;if(_tmp425
<= (void*)4)goto _LL2DC;if(*((int*)_tmp425)!= 11)goto _LL2DC;_tmp426=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp425)->f1;_tmp427=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp425)->f2;_LL2DB: if(_tmp426 != _tmp423){if(_tmp426 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp427,_tmp424);_LL2DC: _tmp428=_tmp3AF.f1;if(_tmp428 <= (void*)4)goto _LL2DE;if(*((
int*)_tmp428)!= 15)goto _LL2DE;_tmp429=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp428)->f1;_tmp42A=_tmp3AF.f2;if(_tmp42A <= (void*)4)goto _LL2DE;if(*((int*)
_tmp42A)!= 15)goto _LL2DE;_tmp42B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp42A)->f1;_LL2DD: return Cyc_Tcutil_typecmp(_tmp429,_tmp42B);_LL2DE: _tmp42C=
_tmp3AF.f1;if(_tmp42C <= (void*)4)goto _LL2E0;if(*((int*)_tmp42C)!= 14)goto _LL2E0;
_tmp42D=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp42C)->f1;_tmp42E=_tmp3AF.f2;
if(_tmp42E <= (void*)4)goto _LL2E0;if(*((int*)_tmp42E)!= 14)goto _LL2E0;_tmp42F=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp42E)->f1;_LL2DF: return Cyc_Tcutil_typecmp(
_tmp42D,_tmp42F);_LL2E0: _tmp430=_tmp3AF.f1;if(_tmp430 <= (void*)4)goto _LL2E2;if(*((
int*)_tmp430)!= 17)goto _LL2E2;_tmp431=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp430)->f1;_tmp432=_tmp3AF.f2;if(_tmp432 <= (void*)4)goto _LL2E2;if(*((int*)
_tmp432)!= 17)goto _LL2E2;_tmp433=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp432)->f1;_LL2E1: return Cyc_Tcutil_typecmp(_tmp431,_tmp433);_LL2E2: _tmp434=
_tmp3AF.f1;if(_tmp434 <= (void*)4)goto _LL2E4;if(*((int*)_tmp434)!= 18)goto _LL2E4;
_tmp435=((struct Cyc_Absyn_TypeInt_struct*)_tmp434)->f1;_tmp436=_tmp3AF.f2;if(
_tmp436 <= (void*)4)goto _LL2E4;if(*((int*)_tmp436)!= 18)goto _LL2E4;_tmp437=((
struct Cyc_Absyn_TypeInt_struct*)_tmp436)->f1;_LL2E3: return Cyc_Core_intcmp(
_tmp435,_tmp437);_LL2E4: _tmp438=_tmp3AF.f1;if(_tmp438 <= (void*)4)goto _LL2E6;if(*((
int*)_tmp438)!= 20)goto _LL2E6;_LL2E5: goto _LL2E7;_LL2E6: _tmp439=_tmp3AF.f2;if(
_tmp439 <= (void*)4)goto _LL2E8;if(*((int*)_tmp439)!= 20)goto _LL2E8;_LL2E7: goto
_LL2E9;_LL2E8: _tmp43A=_tmp3AF.f1;if(_tmp43A <= (void*)4)goto _LL2EA;if(*((int*)
_tmp43A)!= 19)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp43B=_tmp3AF.f1;if(_tmp43B
<= (void*)4)goto _LL2EC;if(*((int*)_tmp43B)!= 21)goto _LL2EC;_LL2EB: goto _LL2ED;
_LL2EC: _tmp43C=_tmp3AF.f2;if(_tmp43C <= (void*)4)goto _LL2EE;if(*((int*)_tmp43C)!= 
21)goto _LL2EE;_LL2ED: goto _LL2EF;_LL2EE: _tmp43D=_tmp3AF.f2;if(_tmp43D <= (void*)4)
goto _LL2F0;if(*((int*)_tmp43D)!= 19)goto _LL2F0;_LL2EF:({void*_tmp458[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp459="typecmp: effects not handled";_tag_dynforward(_tmp459,sizeof(char),
_get_zero_arr_size(_tmp459,29));}),_tag_dynforward(_tmp458,sizeof(void*),0));});
_LL2F0:;_LL2F1:({void*_tmp45A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp45B="Unmatched case in typecmp";
_tag_dynforward(_tmp45B,sizeof(char),_get_zero_arr_size(_tmp45B,26));}),
_tag_dynforward(_tmp45A,sizeof(void*),0));});_LL2BD:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp45C=Cyc_Tcutil_compress(t);_LL2FA: if(_tmp45C <= (void*)4)goto
_LL2FC;if(*((int*)_tmp45C)!= 5)goto _LL2FC;_LL2FB: goto _LL2FD;_LL2FC: if((int)
_tmp45C != 1)goto _LL2FE;_LL2FD: goto _LL2FF;_LL2FE: if(_tmp45C <= (void*)4)goto _LL304;
if(*((int*)_tmp45C)!= 6)goto _LL300;_LL2FF: goto _LL301;_LL300: if(*((int*)_tmp45C)
!= 12)goto _LL302;_LL301: goto _LL303;_LL302: if(*((int*)_tmp45C)!= 13)goto _LL304;
_LL303: return 1;_LL304:;_LL305: return 0;_LL2F9:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple6 _tmp45E=({struct _tuple6 _tmp45D;_tmp45D.f1=t1;_tmp45D.f2=t2;_tmp45D;});
void*_tmp45F;int _tmp460;void*_tmp461;int _tmp462;void*_tmp463;void*_tmp464;void*
_tmp465;void*_tmp466;void*_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;void*
_tmp46B;void*_tmp46C;void*_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*
_tmp471;void*_tmp472;void*_tmp473;void*_tmp474;void*_tmp475;void*_tmp476;void*
_tmp477;void*_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;void*_tmp47C;void*
_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;void*_tmp481;void*_tmp482;void*
_tmp483;void*_tmp484;void*_tmp485;void*_tmp486;void*_tmp487;void*_tmp488;void*
_tmp489;void*_tmp48A;void*_tmp48B;void*_tmp48C;void*_tmp48D;void*_tmp48E;void*
_tmp48F;void*_tmp490;void*_tmp491;void*_tmp492;void*_tmp493;void*_tmp494;void*
_tmp495;void*_tmp496;void*_tmp497;void*_tmp498;void*_tmp499;void*_tmp49A;void*
_tmp49B;void*_tmp49C;void*_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;void*
_tmp4A1;void*_tmp4A2;void*_tmp4A3;void*_tmp4A4;_LL307: _tmp45F=_tmp45E.f1;if(
_tmp45F <= (void*)4)goto _LL309;if(*((int*)_tmp45F)!= 6)goto _LL309;_tmp460=((
struct Cyc_Absyn_DoubleType_struct*)_tmp45F)->f1;_tmp461=_tmp45E.f2;if(_tmp461 <= (
void*)4)goto _LL309;if(*((int*)_tmp461)!= 6)goto _LL309;_tmp462=((struct Cyc_Absyn_DoubleType_struct*)
_tmp461)->f1;_LL308: return !_tmp462  && _tmp460;_LL309: _tmp463=_tmp45E.f1;if(
_tmp463 <= (void*)4)goto _LL30B;if(*((int*)_tmp463)!= 6)goto _LL30B;_tmp464=_tmp45E.f2;
if((int)_tmp464 != 1)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp465=_tmp45E.f1;if(
_tmp465 <= (void*)4)goto _LL30D;if(*((int*)_tmp465)!= 6)goto _LL30D;_tmp466=_tmp45E.f2;
if(_tmp466 <= (void*)4)goto _LL30D;if(*((int*)_tmp466)!= 5)goto _LL30D;_LL30C: goto
_LL30E;_LL30D: _tmp467=_tmp45E.f1;if(_tmp467 <= (void*)4)goto _LL30F;if(*((int*)
_tmp467)!= 6)goto _LL30F;_tmp468=_tmp45E.f2;if(_tmp468 <= (void*)4)goto _LL30F;if(*((
int*)_tmp468)!= 14)goto _LL30F;_LL30E: goto _LL310;_LL30F: _tmp469=_tmp45E.f1;if((
int)_tmp469 != 1)goto _LL311;_tmp46A=_tmp45E.f2;if(_tmp46A <= (void*)4)goto _LL311;
if(*((int*)_tmp46A)!= 14)goto _LL311;_LL310: goto _LL312;_LL311: _tmp46B=_tmp45E.f1;
if(_tmp46B <= (void*)4)goto _LL313;if(*((int*)_tmp46B)!= 6)goto _LL313;_tmp46C=
_tmp45E.f2;if(_tmp46C <= (void*)4)goto _LL313;if(*((int*)_tmp46C)!= 17)goto _LL313;
_LL312: goto _LL314;_LL313: _tmp46D=_tmp45E.f1;if((int)_tmp46D != 1)goto _LL315;
_tmp46E=_tmp45E.f2;if(_tmp46E <= (void*)4)goto _LL315;if(*((int*)_tmp46E)!= 17)
goto _LL315;_LL314: goto _LL316;_LL315: _tmp46F=_tmp45E.f1;if((int)_tmp46F != 1)goto
_LL317;_tmp470=_tmp45E.f2;if(_tmp470 <= (void*)4)goto _LL317;if(*((int*)_tmp470)!= 
5)goto _LL317;_LL316: return 1;_LL317: _tmp471=_tmp45E.f1;if(_tmp471 <= (void*)4)goto
_LL319;if(*((int*)_tmp471)!= 5)goto _LL319;_tmp472=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp471)->f2;if((int)_tmp472 != 4)goto _LL319;_tmp473=_tmp45E.f2;if(_tmp473 <= (
void*)4)goto _LL319;if(*((int*)_tmp473)!= 5)goto _LL319;_tmp474=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp473)->f2;if((int)_tmp474 != 4)goto _LL319;_LL318: return 0;_LL319: _tmp475=
_tmp45E.f1;if(_tmp475 <= (void*)4)goto _LL31B;if(*((int*)_tmp475)!= 5)goto _LL31B;
_tmp476=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp475)->f2;if((int)_tmp476 != 
4)goto _LL31B;_LL31A: return 1;_LL31B: _tmp477=_tmp45E.f1;if(_tmp477 <= (void*)4)goto
_LL31D;if(*((int*)_tmp477)!= 5)goto _LL31D;_tmp478=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp477)->f2;if((int)_tmp478 != 3)goto _LL31D;_tmp479=_tmp45E.f2;if(_tmp479 <= (
void*)4)goto _LL31D;if(*((int*)_tmp479)!= 5)goto _LL31D;_tmp47A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp479)->f2;if((int)_tmp47A != 2)goto _LL31D;_LL31C: goto _LL31E;_LL31D: _tmp47B=
_tmp45E.f1;if(_tmp47B <= (void*)4)goto _LL31F;if(*((int*)_tmp47B)!= 5)goto _LL31F;
_tmp47C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47B)->f2;if((int)_tmp47C != 
2)goto _LL31F;_tmp47D=_tmp45E.f2;if(_tmp47D <= (void*)4)goto _LL31F;if(*((int*)
_tmp47D)!= 5)goto _LL31F;_tmp47E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47D)->f2;
if((int)_tmp47E != 3)goto _LL31F;_LL31E: return 0;_LL31F: _tmp47F=_tmp45E.f1;if(
_tmp47F <= (void*)4)goto _LL321;if(*((int*)_tmp47F)!= 5)goto _LL321;_tmp480=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp47F)->f2;if((int)_tmp480 != 3)goto _LL321;
_tmp481=_tmp45E.f2;if((int)_tmp481 != 1)goto _LL321;_LL320: goto _LL322;_LL321:
_tmp482=_tmp45E.f1;if(_tmp482 <= (void*)4)goto _LL323;if(*((int*)_tmp482)!= 5)goto
_LL323;_tmp483=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp482)->f2;if((int)
_tmp483 != 2)goto _LL323;_tmp484=_tmp45E.f2;if((int)_tmp484 != 1)goto _LL323;_LL322:
goto _LL324;_LL323: _tmp485=_tmp45E.f1;if(_tmp485 <= (void*)4)goto _LL325;if(*((int*)
_tmp485)!= 5)goto _LL325;_tmp486=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp485)->f2;
if((int)_tmp486 != 3)goto _LL325;_tmp487=_tmp45E.f2;if(_tmp487 <= (void*)4)goto
_LL325;if(*((int*)_tmp487)!= 5)goto _LL325;_tmp488=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp487)->f2;if((int)_tmp488 != 1)goto _LL325;_LL324: goto _LL326;_LL325: _tmp489=
_tmp45E.f1;if(_tmp489 <= (void*)4)goto _LL327;if(*((int*)_tmp489)!= 5)goto _LL327;
_tmp48A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp489)->f2;if((int)_tmp48A != 
2)goto _LL327;_tmp48B=_tmp45E.f2;if(_tmp48B <= (void*)4)goto _LL327;if(*((int*)
_tmp48B)!= 5)goto _LL327;_tmp48C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48B)->f2;
if((int)_tmp48C != 1)goto _LL327;_LL326: goto _LL328;_LL327: _tmp48D=_tmp45E.f1;if(
_tmp48D <= (void*)4)goto _LL329;if(*((int*)_tmp48D)!= 17)goto _LL329;_tmp48E=
_tmp45E.f2;if(_tmp48E <= (void*)4)goto _LL329;if(*((int*)_tmp48E)!= 5)goto _LL329;
_tmp48F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48E)->f2;if((int)_tmp48F != 
1)goto _LL329;_LL328: goto _LL32A;_LL329: _tmp490=_tmp45E.f1;if(_tmp490 <= (void*)4)
goto _LL32B;if(*((int*)_tmp490)!= 14)goto _LL32B;_tmp491=_tmp45E.f2;if(_tmp491 <= (
void*)4)goto _LL32B;if(*((int*)_tmp491)!= 5)goto _LL32B;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp491)->f2;if((int)_tmp492 != 1)goto _LL32B;_LL32A: goto _LL32C;_LL32B: _tmp493=
_tmp45E.f1;if(_tmp493 <= (void*)4)goto _LL32D;if(*((int*)_tmp493)!= 5)goto _LL32D;
_tmp494=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp493)->f2;if((int)_tmp494 != 
3)goto _LL32D;_tmp495=_tmp45E.f2;if(_tmp495 <= (void*)4)goto _LL32D;if(*((int*)
_tmp495)!= 5)goto _LL32D;_tmp496=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp495)->f2;
if((int)_tmp496 != 0)goto _LL32D;_LL32C: goto _LL32E;_LL32D: _tmp497=_tmp45E.f1;if(
_tmp497 <= (void*)4)goto _LL32F;if(*((int*)_tmp497)!= 5)goto _LL32F;_tmp498=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp497)->f2;if((int)_tmp498 != 2)goto _LL32F;
_tmp499=_tmp45E.f2;if(_tmp499 <= (void*)4)goto _LL32F;if(*((int*)_tmp499)!= 5)goto
_LL32F;_tmp49A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp499)->f2;if((int)
_tmp49A != 0)goto _LL32F;_LL32E: goto _LL330;_LL32F: _tmp49B=_tmp45E.f1;if(_tmp49B <= (
void*)4)goto _LL331;if(*((int*)_tmp49B)!= 5)goto _LL331;_tmp49C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49B)->f2;if((int)_tmp49C != 1)goto _LL331;_tmp49D=_tmp45E.f2;if(_tmp49D <= (
void*)4)goto _LL331;if(*((int*)_tmp49D)!= 5)goto _LL331;_tmp49E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49D)->f2;if((int)_tmp49E != 0)goto _LL331;_LL330: goto _LL332;_LL331: _tmp49F=
_tmp45E.f1;if(_tmp49F <= (void*)4)goto _LL333;if(*((int*)_tmp49F)!= 17)goto _LL333;
_tmp4A0=_tmp45E.f2;if(_tmp4A0 <= (void*)4)goto _LL333;if(*((int*)_tmp4A0)!= 5)goto
_LL333;_tmp4A1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A0)->f2;if((int)
_tmp4A1 != 0)goto _LL333;_LL332: goto _LL334;_LL333: _tmp4A2=_tmp45E.f1;if(_tmp4A2 <= (
void*)4)goto _LL335;if(*((int*)_tmp4A2)!= 14)goto _LL335;_tmp4A3=_tmp45E.f2;if(
_tmp4A3 <= (void*)4)goto _LL335;if(*((int*)_tmp4A3)!= 5)goto _LL335;_tmp4A4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4A3)->f2;if((int)_tmp4A4 != 0)goto _LL335;
_LL334: return 1;_LL335:;_LL336: return 0;_LL306:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp4A5=
_cycalloc(sizeof(*_tmp4A5));_tmp4A5->v=(void*)t1;_tmp4A5;});}}}if(max_arith_type
!= 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp4A9;_tmp4A9.tag=0;_tmp4A9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp4A8;_tmp4A8.tag=0;_tmp4A8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp4A6[2]={& _tmp4A8,& _tmp4A9};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp4A7="type mismatch: expecting %s but found %s";
_tag_dynforward(_tmp4A7,sizeof(char),_get_zero_arr_size(_tmp4A7,41));}),
_tag_dynforward(_tmp4A6,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp4AA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL338: if(_tmp4AA <= (void*)4)goto _LL33A;if(*((int*)_tmp4AA)!= 4)goto _LL33A;
_LL339: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL337;
_LL33A:;_LL33B: return 0;_LL337:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp4AB=Cyc_Tcutil_compress(t);_LL33D: if(_tmp4AB <= (void*)4)goto _LL347;if(*((
int*)_tmp4AB)!= 5)goto _LL33F;_LL33E: goto _LL340;_LL33F: if(*((int*)_tmp4AB)!= 14)
goto _LL341;_LL340: goto _LL342;_LL341: if(*((int*)_tmp4AB)!= 17)goto _LL343;_LL342:
goto _LL344;_LL343: if(*((int*)_tmp4AB)!= 12)goto _LL345;_LL344: goto _LL346;_LL345:
if(*((int*)_tmp4AB)!= 13)goto _LL347;_LL346: return 1;_LL347:;_LL348: return 0;_LL33C:;}
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*_tmp4AC[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4AD="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4AD,sizeof(char),_get_zero_arr_size(_tmp4AD,44));}),
_tag_dynforward(_tmp4AC,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp4AE[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp4AF="integral size mismatch; conversion supplied";
_tag_dynforward(_tmp4AF,sizeof(char),_get_zero_arr_size(_tmp4AF,44));}),
_tag_dynforward(_tmp4AE,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp4B1=({struct _tuple6 _tmp4B0;_tmp4B0.f1=
t1;_tmp4B0.f2=t2;_tmp4B0;});void*_tmp4B2;struct Cyc_Absyn_PtrInfo _tmp4B3;void*
_tmp4B4;struct Cyc_Absyn_PtrInfo _tmp4B5;void*_tmp4B6;struct Cyc_Absyn_ArrayInfo
_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_Tqual _tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;
struct Cyc_Absyn_Conref*_tmp4BB;void*_tmp4BC;struct Cyc_Absyn_ArrayInfo _tmp4BD;
void*_tmp4BE;struct Cyc_Absyn_Tqual _tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Conref*
_tmp4C1;void*_tmp4C2;struct Cyc_Absyn_TunionFieldInfo _tmp4C3;void*_tmp4C4;struct
Cyc_Absyn_Tuniondecl*_tmp4C5;struct Cyc_Absyn_Tunionfield*_tmp4C6;struct Cyc_List_List*
_tmp4C7;void*_tmp4C8;struct Cyc_Absyn_TunionInfo _tmp4C9;void*_tmp4CA;struct Cyc_Absyn_Tuniondecl**
_tmp4CB;struct Cyc_Absyn_Tuniondecl*_tmp4CC;struct Cyc_List_List*_tmp4CD;void*
_tmp4CE;struct Cyc_Absyn_PtrInfo _tmp4CF;void*_tmp4D0;struct Cyc_Absyn_Tqual _tmp4D1;
struct Cyc_Absyn_PtrAtts _tmp4D2;void*_tmp4D3;struct Cyc_Absyn_Conref*_tmp4D4;
struct Cyc_Absyn_Conref*_tmp4D5;struct Cyc_Absyn_Conref*_tmp4D6;void*_tmp4D7;
struct Cyc_Absyn_TunionInfo _tmp4D8;void*_tmp4D9;struct Cyc_Absyn_Tuniondecl**
_tmp4DA;struct Cyc_Absyn_Tuniondecl*_tmp4DB;struct Cyc_List_List*_tmp4DC;void*
_tmp4DD;void*_tmp4DE;void*_tmp4DF;void*_tmp4E0;void*_tmp4E1;void*_tmp4E2;void*
_tmp4E3;void*_tmp4E4;void*_tmp4E5;_LL34A: _tmp4B2=_tmp4B1.f1;if(_tmp4B2 <= (void*)
4)goto _LL34C;if(*((int*)_tmp4B2)!= 4)goto _LL34C;_tmp4B3=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B2)->f1;_tmp4B4=_tmp4B1.f2;if(_tmp4B4 <= (void*)4)goto _LL34C;if(*((int*)
_tmp4B4)!= 4)goto _LL34C;_tmp4B5=((struct Cyc_Absyn_PointerType_struct*)_tmp4B4)->f1;
_LL34B: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4B3.ptr_atts).nullable,(
_tmp4B5.ptr_atts).nullable)){void*_tmp4E6=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp4B3.ptr_atts).nullable))->v;
int _tmp4E7;_LL35B: if(_tmp4E6 <= (void*)1)goto _LL35D;if(*((int*)_tmp4E6)!= 0)goto
_LL35D;_tmp4E7=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4E6)->f1;_LL35C: okay=
!_tmp4E7;goto _LL35A;_LL35D:;_LL35E:({void*_tmp4E8[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4E9="silent_castable conref not eq";_tag_dynforward(_tmp4E9,sizeof(char),
_get_zero_arr_size(_tmp4E9,30));}),_tag_dynforward(_tmp4E8,sizeof(void*),0));});
_LL35A:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp4B3.ptr_atts).bounds,(
_tmp4B5.ptr_atts).bounds)){struct _tuple6 _tmp4EB=({struct _tuple6 _tmp4EA;_tmp4EA.f1=(
void*)(Cyc_Absyn_compress_conref((_tmp4B3.ptr_atts).bounds))->v;_tmp4EA.f2=(void*)(
Cyc_Absyn_compress_conref((_tmp4B5.ptr_atts).bounds))->v;_tmp4EA;});void*_tmp4EC;
void*_tmp4ED;void*_tmp4EE;void*_tmp4EF;void*_tmp4F0;_LL360: _tmp4EC=_tmp4EB.f1;
if(_tmp4EC <= (void*)1)goto _LL362;if(*((int*)_tmp4EC)!= 0)goto _LL362;_tmp4ED=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4EC)->f1;_tmp4EE=_tmp4EB.f2;if(
_tmp4EE <= (void*)1)goto _LL362;if(*((int*)_tmp4EE)!= 0)goto _LL362;_tmp4EF=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp4EE)->f1;_LL361:{struct _tuple6 _tmp4F2=({
struct _tuple6 _tmp4F1;_tmp4F1.f1=_tmp4ED;_tmp4F1.f2=_tmp4EF;_tmp4F1;});void*
_tmp4F3;void*_tmp4F4;void*_tmp4F5;void*_tmp4F6;void*_tmp4F7;void*_tmp4F8;void*
_tmp4F9;void*_tmp4FA;void*_tmp4FB;void*_tmp4FC;void*_tmp4FD;void*_tmp4FE;void*
_tmp4FF;struct Cyc_Absyn_Exp*_tmp500;void*_tmp501;struct Cyc_Absyn_Exp*_tmp502;
void*_tmp503;void*_tmp504;struct Cyc_Absyn_Exp*_tmp505;void*_tmp506;void*_tmp507;
struct Cyc_Absyn_Exp*_tmp508;void*_tmp509;void*_tmp50A;void*_tmp50B;void*_tmp50C;
void*_tmp50D;struct Cyc_Absyn_Exp*_tmp50E;void*_tmp50F;void*_tmp510;void*_tmp511;
void*_tmp512;_LL367: _tmp4F3=_tmp4F2.f1;if(_tmp4F3 <= (void*)2)goto _LL369;if(*((
int*)_tmp4F3)!= 0)goto _LL369;_tmp4F4=_tmp4F2.f2;if((int)_tmp4F4 != 0)goto _LL369;
_LL368: goto _LL36A;_LL369: _tmp4F5=_tmp4F2.f1;if(_tmp4F5 <= (void*)2)goto _LL36B;if(*((
int*)_tmp4F5)!= 0)goto _LL36B;_tmp4F6=_tmp4F2.f2;if((int)_tmp4F6 != 1)goto _LL36B;
_LL36A: goto _LL36C;_LL36B: _tmp4F7=_tmp4F2.f1;if((int)_tmp4F7 != 1)goto _LL36D;
_tmp4F8=_tmp4F2.f2;if((int)_tmp4F8 != 1)goto _LL36D;_LL36C: goto _LL36E;_LL36D:
_tmp4F9=_tmp4F2.f1;if((int)_tmp4F9 != 0)goto _LL36F;_tmp4FA=_tmp4F2.f2;if((int)
_tmp4FA != 1)goto _LL36F;_LL36E: goto _LL370;_LL36F: _tmp4FB=_tmp4F2.f1;if((int)
_tmp4FB != 0)goto _LL371;_tmp4FC=_tmp4F2.f2;if((int)_tmp4FC != 0)goto _LL371;_LL370:
okay=1;goto _LL366;_LL371: _tmp4FD=_tmp4F2.f1;if((int)_tmp4FD != 1)goto _LL373;
_tmp4FE=_tmp4F2.f2;if((int)_tmp4FE != 0)goto _LL373;_LL372: okay=0;goto _LL366;
_LL373: _tmp4FF=_tmp4F2.f1;if(_tmp4FF <= (void*)2)goto _LL375;if(*((int*)_tmp4FF)!= 
0)goto _LL375;_tmp500=((struct Cyc_Absyn_Upper_b_struct*)_tmp4FF)->f1;_tmp501=
_tmp4F2.f2;if(_tmp501 <= (void*)2)goto _LL375;if(*((int*)_tmp501)!= 0)goto _LL375;
_tmp502=((struct Cyc_Absyn_Upper_b_struct*)_tmp501)->f1;_LL374: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp502,_tmp500);if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4B5.ptr_atts).zero_term))({void*_tmp513[0]={};Cyc_Tcutil_warn(loc,({const
char*_tmp514="implicit cast to shorter array";_tag_dynforward(_tmp514,sizeof(
char),_get_zero_arr_size(_tmp514,31));}),_tag_dynforward(_tmp513,sizeof(void*),0));});
goto _LL366;_LL375: _tmp503=_tmp4F2.f1;if((int)_tmp503 != 0)goto _LL377;_tmp504=
_tmp4F2.f2;if(_tmp504 <= (void*)2)goto _LL377;if(*((int*)_tmp504)!= 0)goto _LL377;
_tmp505=((struct Cyc_Absyn_Upper_b_struct*)_tmp504)->f1;_LL376: _tmp508=_tmp505;
goto _LL378;_LL377: _tmp506=_tmp4F2.f1;if((int)_tmp506 != 1)goto _LL379;_tmp507=
_tmp4F2.f2;if(_tmp507 <= (void*)2)goto _LL379;if(*((int*)_tmp507)!= 0)goto _LL379;
_tmp508=((struct Cyc_Absyn_Upper_b_struct*)_tmp507)->f1;_LL378: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4B3.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4B5.ptr_atts).bounds))goto _LL366;okay=0;goto
_LL366;_LL379: _tmp509=_tmp4F2.f1;if(_tmp509 <= (void*)2)goto _LL37B;if(*((int*)
_tmp509)!= 1)goto _LL37B;_tmp50A=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp509)->f1;_tmp50B=_tmp4F2.f2;if(_tmp50B <= (void*)2)goto _LL37B;if(*((int*)
_tmp50B)!= 1)goto _LL37B;_tmp50C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp50B)->f1;_LL37A: if(!Cyc_Tcutil_unify(_tmp50A,_tmp50C)){struct _tuple6 _tmp516=({
struct _tuple6 _tmp515;_tmp515.f1=Cyc_Tcutil_compress(_tmp50A);_tmp515.f2=Cyc_Tcutil_compress(
_tmp50C);_tmp515;});void*_tmp517;int _tmp518;void*_tmp519;int _tmp51A;_LL382:
_tmp517=_tmp516.f1;if(_tmp517 <= (void*)4)goto _LL384;if(*((int*)_tmp517)!= 18)
goto _LL384;_tmp518=((struct Cyc_Absyn_TypeInt_struct*)_tmp517)->f1;_tmp519=
_tmp516.f2;if(_tmp519 <= (void*)4)goto _LL384;if(*((int*)_tmp519)!= 18)goto _LL384;
_tmp51A=((struct Cyc_Absyn_TypeInt_struct*)_tmp519)->f1;_LL383: if(_tmp518 < 
_tmp51A)okay=0;goto _LL381;_LL384:;_LL385: okay=0;goto _LL381;_LL381:;}goto _LL366;
_LL37B: _tmp50D=_tmp4F2.f1;if(_tmp50D <= (void*)2)goto _LL37D;if(*((int*)_tmp50D)!= 
0)goto _LL37D;_tmp50E=((struct Cyc_Absyn_Upper_b_struct*)_tmp50D)->f1;_tmp50F=
_tmp4F2.f2;if(_tmp50F <= (void*)2)goto _LL37D;if(*((int*)_tmp50F)!= 1)goto _LL37D;
_tmp510=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp50F)->f1;_LL37C: {
unsigned int _tmp51C;int _tmp51D;struct _tuple7 _tmp51B=Cyc_Evexp_eval_const_uint_exp(
_tmp50E);_tmp51C=_tmp51B.f1;_tmp51D=_tmp51B.f2;if(!_tmp51D){okay=0;goto _LL366;}{
void*_tmp51E=Cyc_Tcutil_compress(_tmp510);int _tmp51F;_LL387: if(_tmp51E <= (void*)
4)goto _LL389;if(*((int*)_tmp51E)!= 18)goto _LL389;_tmp51F=((struct Cyc_Absyn_TypeInt_struct*)
_tmp51E)->f1;_LL388: if(_tmp51C < _tmp51F)okay=0;goto _LL386;_LL389:;_LL38A: okay=0;
goto _LL386;_LL386:;}goto _LL366;}_LL37D: _tmp511=_tmp4F2.f1;if(_tmp511 <= (void*)2)
goto _LL37F;if(*((int*)_tmp511)!= 1)goto _LL37F;_LL37E: goto _LL380;_LL37F: _tmp512=
_tmp4F2.f2;if(_tmp512 <= (void*)2)goto _LL366;if(*((int*)_tmp512)!= 1)goto _LL366;
_LL380: okay=0;goto _LL366;_LL366:;}goto _LL35F;_LL362: _tmp4F0=_tmp4EB.f1;if((int)
_tmp4F0 != 0)goto _LL364;_LL363: okay=0;goto _LL35F;_LL364:;_LL365:({void*_tmp520[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp521="silent_castable conrefs didn't unify";_tag_dynforward(_tmp521,
sizeof(char),_get_zero_arr_size(_tmp521,37));}),_tag_dynforward(_tmp520,sizeof(
void*),0));});_LL35F:;}okay=okay  && Cyc_Tcutil_unify((void*)_tmp4B3.elt_typ,(
void*)_tmp4B5.elt_typ);okay=okay  && (Cyc_Tcutil_unify((void*)(_tmp4B3.ptr_atts).rgn,(
void*)(_tmp4B5.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(te,(void*)(_tmp4B3.ptr_atts).rgn,(
void*)(_tmp4B5.ptr_atts).rgn));okay=okay  && (!(_tmp4B3.elt_tq).real_const  || (
_tmp4B5.elt_tq).real_const);okay=okay  && (((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4B3.ptr_atts).zero_term,(
_tmp4B5.ptr_atts).zero_term) || ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,(_tmp4B3.ptr_atts).zero_term) && (_tmp4B5.elt_tq).real_const);return okay;}
_LL34C: _tmp4B6=_tmp4B1.f1;if(_tmp4B6 <= (void*)4)goto _LL34E;if(*((int*)_tmp4B6)!= 
7)goto _LL34E;_tmp4B7=((struct Cyc_Absyn_ArrayType_struct*)_tmp4B6)->f1;_tmp4B8=(
void*)_tmp4B7.elt_type;_tmp4B9=_tmp4B7.tq;_tmp4BA=_tmp4B7.num_elts;_tmp4BB=
_tmp4B7.zero_term;_tmp4BC=_tmp4B1.f2;if(_tmp4BC <= (void*)4)goto _LL34E;if(*((int*)
_tmp4BC)!= 7)goto _LL34E;_tmp4BD=((struct Cyc_Absyn_ArrayType_struct*)_tmp4BC)->f1;
_tmp4BE=(void*)_tmp4BD.elt_type;_tmp4BF=_tmp4BD.tq;_tmp4C0=_tmp4BD.num_elts;
_tmp4C1=_tmp4BD.zero_term;_LL34D: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4BB,_tmp4C1) && ((_tmp4BA != 0  && _tmp4C0 != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp4BA,(struct Cyc_Absyn_Exp*)_tmp4C0));return(okay  && Cyc_Tcutil_unify(
_tmp4B8,_tmp4BE)) && (!_tmp4B9.real_const  || _tmp4BF.real_const);}_LL34E: _tmp4C2=
_tmp4B1.f1;if(_tmp4C2 <= (void*)4)goto _LL350;if(*((int*)_tmp4C2)!= 3)goto _LL350;
_tmp4C3=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4C2)->f1;_tmp4C4=(void*)
_tmp4C3.field_info;if(*((int*)_tmp4C4)!= 1)goto _LL350;_tmp4C5=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp4C4)->f1;_tmp4C6=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4C4)->f2;
_tmp4C7=_tmp4C3.targs;_tmp4C8=_tmp4B1.f2;if(_tmp4C8 <= (void*)4)goto _LL350;if(*((
int*)_tmp4C8)!= 2)goto _LL350;_tmp4C9=((struct Cyc_Absyn_TunionType_struct*)
_tmp4C8)->f1;_tmp4CA=(void*)_tmp4C9.tunion_info;if(*((int*)_tmp4CA)!= 1)goto
_LL350;_tmp4CB=((struct Cyc_Absyn_KnownTunion_struct*)_tmp4CA)->f1;_tmp4CC=*
_tmp4CB;_tmp4CD=_tmp4C9.targs;_LL34F: if((_tmp4C5 == _tmp4CC  || Cyc_Absyn_qvar_cmp(
_tmp4C5->name,_tmp4CC->name)== 0) && _tmp4C6->typs == 0){for(0;_tmp4C7 != 0  && 
_tmp4CD != 0;(_tmp4C7=_tmp4C7->tl,_tmp4CD=_tmp4CD->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4C7->hd,(void*)_tmp4CD->hd))break;}if(_tmp4C7 == 0  && _tmp4CD == 0)
return 1;}return 0;_LL350: _tmp4CE=_tmp4B1.f1;if(_tmp4CE <= (void*)4)goto _LL352;if(*((
int*)_tmp4CE)!= 4)goto _LL352;_tmp4CF=((struct Cyc_Absyn_PointerType_struct*)
_tmp4CE)->f1;_tmp4D0=(void*)_tmp4CF.elt_typ;_tmp4D1=_tmp4CF.elt_tq;_tmp4D2=
_tmp4CF.ptr_atts;_tmp4D3=(void*)_tmp4D2.rgn;_tmp4D4=_tmp4D2.nullable;_tmp4D5=
_tmp4D2.bounds;_tmp4D6=_tmp4D2.zero_term;_tmp4D7=_tmp4B1.f2;if(_tmp4D7 <= (void*)
4)goto _LL352;if(*((int*)_tmp4D7)!= 2)goto _LL352;_tmp4D8=((struct Cyc_Absyn_TunionType_struct*)
_tmp4D7)->f1;_tmp4D9=(void*)_tmp4D8.tunion_info;if(*((int*)_tmp4D9)!= 1)goto
_LL352;_tmp4DA=((struct Cyc_Absyn_KnownTunion_struct*)_tmp4D9)->f1;_tmp4DB=*
_tmp4DA;_tmp4DC=_tmp4D8.targs;_tmp4DD=(void*)_tmp4D8.rgn;_LL351:{void*_tmp522=
Cyc_Tcutil_compress(_tmp4D0);struct Cyc_Absyn_TunionFieldInfo _tmp523;void*_tmp524;
struct Cyc_Absyn_Tuniondecl*_tmp525;struct Cyc_Absyn_Tunionfield*_tmp526;struct Cyc_List_List*
_tmp527;_LL38C: if(_tmp522 <= (void*)4)goto _LL38E;if(*((int*)_tmp522)!= 3)goto
_LL38E;_tmp523=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp522)->f1;_tmp524=(
void*)_tmp523.field_info;if(*((int*)_tmp524)!= 1)goto _LL38E;_tmp525=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp524)->f1;_tmp526=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp524)->f2;
_tmp527=_tmp523.targs;_LL38D: if(!Cyc_Tcutil_unify(_tmp4D3,_tmp4DD) && !Cyc_Tcenv_region_outlives(
te,_tmp4D3,_tmp4DD))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4D4,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4D5,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4D6,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp4DB->name,
_tmp525->name)== 0  && _tmp526->typs != 0){int okay=1;for(0;_tmp527 != 0  && _tmp4DC
!= 0;(_tmp527=_tmp527->tl,_tmp4DC=_tmp4DC->tl)){if(!Cyc_Tcutil_unify((void*)
_tmp527->hd,(void*)_tmp4DC->hd)){okay=0;break;}}if((!okay  || _tmp527 != 0) || 
_tmp4DC != 0)return 0;return 1;}goto _LL38B;_LL38E:;_LL38F: goto _LL38B;_LL38B:;}
return 0;_LL352: _tmp4DE=_tmp4B1.f1;if(_tmp4DE <= (void*)4)goto _LL354;if(*((int*)
_tmp4DE)!= 14)goto _LL354;_tmp4DF=_tmp4B1.f2;if(_tmp4DF <= (void*)4)goto _LL354;if(*((
int*)_tmp4DF)!= 5)goto _LL354;_tmp4E0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4DF)->f2;if((int)_tmp4E0 != 2)goto _LL354;_LL353: goto _LL355;_LL354: _tmp4E1=
_tmp4B1.f1;if(_tmp4E1 <= (void*)4)goto _LL356;if(*((int*)_tmp4E1)!= 14)goto _LL356;
_tmp4E2=_tmp4B1.f2;if(_tmp4E2 <= (void*)4)goto _LL356;if(*((int*)_tmp4E2)!= 5)goto
_LL356;_tmp4E3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4E2)->f2;if((int)
_tmp4E3 != 3)goto _LL356;_LL355: return 1;_LL356: _tmp4E4=_tmp4B1.f1;if(_tmp4E4 <= (
void*)4)goto _LL358;if(*((int*)_tmp4E4)!= 17)goto _LL358;_tmp4E5=_tmp4B1.f2;if(
_tmp4E5 <= (void*)4)goto _LL358;if(*((int*)_tmp4E5)!= 5)goto _LL358;_LL357: return 0;
_LL358:;_LL359: return Cyc_Tcutil_unify(t1,t2);_LL349:;}}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp528=Cyc_Tcutil_compress(t);_LL391: if(_tmp528 <= (void*)4)goto
_LL393;if(*((int*)_tmp528)!= 4)goto _LL393;_LL392: return 1;_LL393:;_LL394: return 0;
_LL390:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp529=(void*)e->r;
void*_tmp52A;int _tmp52B;void*_tmp52C;char _tmp52D;void*_tmp52E;short _tmp52F;void*
_tmp530;long long _tmp531;void*_tmp532;struct Cyc_Absyn_Exp*_tmp533;_LL396: if(*((
int*)_tmp529)!= 0)goto _LL398;_tmp52A=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp529)->f1;if(_tmp52A <= (void*)1)goto _LL398;if(*((int*)_tmp52A)!= 2)goto _LL398;
_tmp52B=((struct Cyc_Absyn_Int_c_struct*)_tmp52A)->f2;if(_tmp52B != 0)goto _LL398;
_LL397: goto _LL399;_LL398: if(*((int*)_tmp529)!= 0)goto _LL39A;_tmp52C=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp529)->f1;if(_tmp52C <= (void*)1)goto _LL39A;
if(*((int*)_tmp52C)!= 0)goto _LL39A;_tmp52D=((struct Cyc_Absyn_Char_c_struct*)
_tmp52C)->f2;if(_tmp52D != 0)goto _LL39A;_LL399: goto _LL39B;_LL39A: if(*((int*)
_tmp529)!= 0)goto _LL39C;_tmp52E=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp529)->f1;
if(_tmp52E <= (void*)1)goto _LL39C;if(*((int*)_tmp52E)!= 1)goto _LL39C;_tmp52F=((
struct Cyc_Absyn_Short_c_struct*)_tmp52E)->f2;if(_tmp52F != 0)goto _LL39C;_LL39B:
goto _LL39D;_LL39C: if(*((int*)_tmp529)!= 0)goto _LL39E;_tmp530=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp529)->f1;if(_tmp530 <= (void*)1)goto _LL39E;if(*((int*)_tmp530)!= 3)goto _LL39E;
_tmp531=((struct Cyc_Absyn_LongLong_c_struct*)_tmp530)->f2;if(_tmp531 != 0)goto
_LL39E;_LL39D: return 1;_LL39E: if(*((int*)_tmp529)!= 15)goto _LL3A0;_tmp532=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp529)->f1;_tmp533=((struct Cyc_Absyn_Cast_e_struct*)
_tmp529)->f2;_LL39F: return Cyc_Tcutil_is_zero(_tmp533) && Cyc_Tcutil_admits_zero(
_tmp532);_LL3A0:;_LL3A1: return 0;_LL395:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp534=_cycalloc(sizeof(*_tmp534));
_tmp534[0]=({struct Cyc_Absyn_Const_e_struct _tmp535;_tmp535.tag=0;_tmp535.f1=(
void*)((void*)0);_tmp535;});_tmp534;})));{struct Cyc_List_List*_tmp536=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp537=({struct Cyc_Absyn_PointerType_struct*
_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538[0]=({struct Cyc_Absyn_PointerType_struct
_tmp539;_tmp539.tag=4;_tmp539.f1=({struct Cyc_Absyn_PtrInfo _tmp53A;_tmp53A.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->v=_tmp536;_tmp53D;}));_tmp53A.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp53A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp53B;
_tmp53B.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C->v=_tmp536;
_tmp53C;}));_tmp53B.nullable=Cyc_Absyn_true_conref;_tmp53B.bounds=Cyc_Absyn_empty_conref();
_tmp53B.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp53B.ptrloc=0;_tmp53B;});_tmp53A;});_tmp539;});_tmp538;});(void*)(((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp537));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dynforward_ptr Cyc_Tcutil_coercion2string(void*c){
void*_tmp53E=c;_LL3A3: if((int)_tmp53E != 0)goto _LL3A5;_LL3A4: return({const char*
_tmp53F="unknown";_tag_dynforward(_tmp53F,sizeof(char),_get_zero_arr_size(
_tmp53F,8));});_LL3A5: if((int)_tmp53E != 1)goto _LL3A7;_LL3A6: return({const char*
_tmp540="no coercion";_tag_dynforward(_tmp540,sizeof(char),_get_zero_arr_size(
_tmp540,12));});_LL3A7: if((int)_tmp53E != 2)goto _LL3A9;_LL3A8: return({const char*
_tmp541="null check";_tag_dynforward(_tmp541,sizeof(char),_get_zero_arr_size(
_tmp541,11));});_LL3A9: if((int)_tmp53E != 3)goto _LL3A2;_LL3AA: return({const char*
_tmp542="other coercion";_tag_dynforward(_tmp542,sizeof(char),_get_zero_arr_size(
_tmp542,15));});_LL3A2:;}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(
t2) && Cyc_Tcutil_is_arithmetic_type(t1)){if(Cyc_Tcutil_will_lose_precision(t1,
t2))({struct Cyc_String_pa_struct _tmp546;_tmp546.tag=0;_tmp546.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp545;_tmp545.tag=0;_tmp545.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp543[2]={& _tmp545,& _tmp546};Cyc_Tcutil_warn(
e->loc,({const char*_tmp544="integral size mismatch; %s -> %s conversion supplied";
_tag_dynforward(_tmp544,sizeof(char),_get_zero_arr_size(_tmp544,53));}),
_tag_dynforward(_tmp543,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(
void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp54A;_tmp54A.tag=0;
_tmp54A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp549;_tmp549.tag=0;_tmp549.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp547[2]={& _tmp549,& _tmp54A};Cyc_Tcutil_warn(e->loc,({const char*_tmp548="implicit cast from %s to %s";
_tag_dynforward(_tmp548,sizeof(char),_get_zero_arr_size(_tmp548,28));}),
_tag_dynforward(_tmp547,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp54B=Cyc_Tcutil_compress(t);
_LL3AC: if(_tmp54B <= (void*)4)goto _LL3AE;if(*((int*)_tmp54B)!= 5)goto _LL3AE;
_LL3AD: goto _LL3AF;_LL3AE: if((int)_tmp54B != 1)goto _LL3B0;_LL3AF: goto _LL3B1;_LL3B0:
if(_tmp54B <= (void*)4)goto _LL3B2;if(*((int*)_tmp54B)!= 6)goto _LL3B2;_LL3B1:
return 1;_LL3B2:;_LL3B3: return 0;_LL3AB:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->f1=x->tq;_tmp54C->f2=Cyc_Tcutil_substitute(
inst,(void*)x->type);_tmp54C;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp54D=t1;
struct Cyc_List_List*_tmp54E;struct Cyc_Absyn_AggrInfo _tmp54F;void*_tmp550;struct
Cyc_Absyn_Aggrdecl**_tmp551;struct Cyc_Absyn_Aggrdecl*_tmp552;struct Cyc_List_List*
_tmp553;void*_tmp554;struct Cyc_List_List*_tmp555;struct Cyc_Absyn_FnInfo _tmp556;
_LL3B5: if((int)_tmp54D != 0)goto _LL3B7;_LL3B6: return 0;_LL3B7: if(_tmp54D <= (void*)
4)goto _LL3BF;if(*((int*)_tmp54D)!= 9)goto _LL3B9;_tmp54E=((struct Cyc_Absyn_TupleType_struct*)
_tmp54D)->f1;_LL3B8: return _tmp54E;_LL3B9: if(*((int*)_tmp54D)!= 10)goto _LL3BB;
_tmp54F=((struct Cyc_Absyn_AggrType_struct*)_tmp54D)->f1;_tmp550=(void*)_tmp54F.aggr_info;
if(*((int*)_tmp550)!= 1)goto _LL3BB;_tmp551=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp550)->f1;_tmp552=*_tmp551;_tmp553=_tmp54F.targs;_LL3BA: if((((void*)_tmp552->kind
== (void*)1  || _tmp552->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp552->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp552->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp557=_cycalloc(
sizeof(*_tmp557));_tmp557->hd=({struct _tuple4*_tmp558=_cycalloc(sizeof(*_tmp558));
_tmp558->f1=Cyc_Absyn_empty_tqual(0);_tmp558->f2=t1;_tmp558;});_tmp557->tl=0;
_tmp557;});{struct Cyc_List_List*_tmp559=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp552->tvs,_tmp553);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp559,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp552->impl))->fields);}_LL3BB: if(*((
int*)_tmp54D)!= 11)goto _LL3BD;_tmp554=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp54D)->f1;if((int)_tmp554 != 0)goto _LL3BD;_tmp555=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp54D)->f2;_LL3BC: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp555);_LL3BD: if(*((int*)_tmp54D)!= 8)goto _LL3BF;
_tmp556=((struct Cyc_Absyn_FnType_struct*)_tmp54D)->f1;_LL3BE: return({struct Cyc_List_List*
_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->hd=({struct _tuple4*_tmp55B=
_cycalloc(sizeof(*_tmp55B));_tmp55B->f1=Cyc_Absyn_const_tqual(0);_tmp55B->f2=t1;
_tmp55B;});_tmp55A->tl=0;_tmp55A;});_LL3BF:;_LL3C0: return({struct Cyc_List_List*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->hd=({struct _tuple4*_tmp55D=
_cycalloc(sizeof(*_tmp55D));_tmp55D->f1=Cyc_Absyn_empty_tqual(0);_tmp55D->f2=t1;
_tmp55D;});_tmp55C->tl=0;_tmp55C;});_LL3B4:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp55E=(void*)t->hd;_LL3C2: if((int)_tmp55E != 16)goto _LL3C4;
_LL3C3: goto _LL3C5;_LL3C4: if((int)_tmp55E != 3)goto _LL3C6;_LL3C5: goto _LL3C7;_LL3C6:
if(_tmp55E <= (void*)17)goto _LL3C8;if(*((int*)_tmp55E)!= 4)goto _LL3C8;_LL3C7:
continue;_LL3C8:;_LL3C9: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL3C1:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp560=({struct _tuple6 _tmp55F;_tmp55F.f1=t1;_tmp55F.f2=t2;_tmp55F;});void*
_tmp561;struct Cyc_Absyn_PtrInfo _tmp562;void*_tmp563;struct Cyc_Absyn_Tqual _tmp564;
struct Cyc_Absyn_PtrAtts _tmp565;void*_tmp566;struct Cyc_Absyn_Conref*_tmp567;
struct Cyc_Absyn_Conref*_tmp568;struct Cyc_Absyn_Conref*_tmp569;void*_tmp56A;
struct Cyc_Absyn_PtrInfo _tmp56B;void*_tmp56C;struct Cyc_Absyn_Tqual _tmp56D;struct
Cyc_Absyn_PtrAtts _tmp56E;void*_tmp56F;struct Cyc_Absyn_Conref*_tmp570;struct Cyc_Absyn_Conref*
_tmp571;struct Cyc_Absyn_Conref*_tmp572;void*_tmp573;struct Cyc_Absyn_FnInfo
_tmp574;void*_tmp575;struct Cyc_Absyn_FnInfo _tmp576;_LL3CB: _tmp561=_tmp560.f1;if(
_tmp561 <= (void*)4)goto _LL3CD;if(*((int*)_tmp561)!= 4)goto _LL3CD;_tmp562=((
struct Cyc_Absyn_PointerType_struct*)_tmp561)->f1;_tmp563=(void*)_tmp562.elt_typ;
_tmp564=_tmp562.elt_tq;_tmp565=_tmp562.ptr_atts;_tmp566=(void*)_tmp565.rgn;
_tmp567=_tmp565.nullable;_tmp568=_tmp565.bounds;_tmp569=_tmp565.zero_term;
_tmp56A=_tmp560.f2;if(_tmp56A <= (void*)4)goto _LL3CD;if(*((int*)_tmp56A)!= 4)goto
_LL3CD;_tmp56B=((struct Cyc_Absyn_PointerType_struct*)_tmp56A)->f1;_tmp56C=(void*)
_tmp56B.elt_typ;_tmp56D=_tmp56B.elt_tq;_tmp56E=_tmp56B.ptr_atts;_tmp56F=(void*)
_tmp56E.rgn;_tmp570=_tmp56E.nullable;_tmp571=_tmp56E.bounds;_tmp572=_tmp56E.zero_term;
_LL3CC: if(_tmp564.real_const  && !_tmp56D.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp567,_tmp570) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp567)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp570))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp569,
_tmp572) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp569)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp572))return 0;if(!Cyc_Tcutil_unify(_tmp566,_tmp56F) && !Cyc_Tcenv_region_outlives(
te,_tmp566,_tmp56F))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp568,_tmp571)){struct _tuple6 _tmp578=({struct _tuple6 _tmp577;_tmp577.f1=Cyc_Absyn_conref_val(
_tmp568);_tmp577.f2=Cyc_Absyn_conref_val(_tmp571);_tmp577;});void*_tmp579;void*
_tmp57A;void*_tmp57B;void*_tmp57C;void*_tmp57D;struct Cyc_Absyn_Exp*_tmp57E;void*
_tmp57F;struct Cyc_Absyn_Exp*_tmp580;_LL3D2: _tmp579=_tmp578.f1;if(_tmp579 <= (void*)
2)goto _LL3D4;if(*((int*)_tmp579)!= 0)goto _LL3D4;_tmp57A=_tmp578.f2;if((int)
_tmp57A != 0)goto _LL3D4;_LL3D3: goto _LL3D1;_LL3D4: _tmp57B=_tmp578.f1;if(_tmp57B <= (
void*)2)goto _LL3D6;if(*((int*)_tmp57B)!= 0)goto _LL3D6;_tmp57C=_tmp578.f2;if((int)
_tmp57C != 1)goto _LL3D6;_LL3D5: goto _LL3D1;_LL3D6: _tmp57D=_tmp578.f1;if(_tmp57D <= (
void*)2)goto _LL3D8;if(*((int*)_tmp57D)!= 0)goto _LL3D8;_tmp57E=((struct Cyc_Absyn_Upper_b_struct*)
_tmp57D)->f1;_tmp57F=_tmp578.f2;if(_tmp57F <= (void*)2)goto _LL3D8;if(*((int*)
_tmp57F)!= 0)goto _LL3D8;_tmp580=((struct Cyc_Absyn_Upper_b_struct*)_tmp57F)->f1;
_LL3D7: if(!Cyc_Evexp_lte_const_exp(_tmp580,_tmp57E))return 0;goto _LL3D1;_LL3D8:;
_LL3D9: return 0;_LL3D1:;}return Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*
_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->hd=({struct _tuple6*_tmp582=
_cycalloc(sizeof(*_tmp582));_tmp582->f1=t1;_tmp582->f2=t2;_tmp582;});_tmp581->tl=
assume;_tmp581;}),_tmp563,_tmp56C);_LL3CD: _tmp573=_tmp560.f1;if(_tmp573 <= (void*)
4)goto _LL3CF;if(*((int*)_tmp573)!= 8)goto _LL3CF;_tmp574=((struct Cyc_Absyn_FnType_struct*)
_tmp573)->f1;_tmp575=_tmp560.f2;if(_tmp575 <= (void*)4)goto _LL3CF;if(*((int*)
_tmp575)!= 8)goto _LL3CF;_tmp576=((struct Cyc_Absyn_FnType_struct*)_tmp575)->f1;
_LL3CE: if(_tmp574.tvars != 0  || _tmp576.tvars != 0){struct Cyc_List_List*_tmp583=
_tmp574.tvars;struct Cyc_List_List*_tmp584=_tmp576.tvars;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp583)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp584))return 0;{struct _RegionHandle _tmp585=_new_region("r");struct
_RegionHandle*r=& _tmp585;_push_region(r);{struct Cyc_List_List*inst=0;while(
_tmp583 != 0){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp583->hd)!= Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp584))->hd)){int
_tmp586=0;_npop_handler(0);return _tmp586;}inst=({struct Cyc_List_List*_tmp587=
_region_malloc(r,sizeof(*_tmp587));_tmp587->hd=({struct _tuple8*_tmp588=
_region_malloc(r,sizeof(*_tmp588));_tmp588->f1=(struct Cyc_Absyn_Tvar*)_tmp584->hd;
_tmp588->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp589=_cycalloc(sizeof(*
_tmp589));_tmp589[0]=({struct Cyc_Absyn_VarType_struct _tmp58A;_tmp58A.tag=1;
_tmp58A.f1=(struct Cyc_Absyn_Tvar*)_tmp583->hd;_tmp58A;});_tmp589;});_tmp588;});
_tmp587->tl=inst;_tmp587;});_tmp583=_tmp583->tl;_tmp584=_tmp584->tl;}if(inst != 0){
_tmp574.tvars=0;_tmp576.tvars=0;{int _tmp58F=Cyc_Tcutil_subtype(te,assume,(void*)({
struct Cyc_Absyn_FnType_struct*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B[0]=({
struct Cyc_Absyn_FnType_struct _tmp58C;_tmp58C.tag=8;_tmp58C.f1=_tmp574;_tmp58C;});
_tmp58B;}),(void*)({struct Cyc_Absyn_FnType_struct*_tmp58D=_cycalloc(sizeof(*
_tmp58D));_tmp58D[0]=({struct Cyc_Absyn_FnType_struct _tmp58E;_tmp58E.tag=8;
_tmp58E.f1=_tmp576;_tmp58E;});_tmp58D;}));_npop_handler(0);return _tmp58F;}}};
_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)_tmp574.ret_typ,(void*)
_tmp576.ret_typ))return 0;{struct Cyc_List_List*_tmp590=_tmp574.args;struct Cyc_List_List*
_tmp591=_tmp576.args;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp590)
!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp591))return 0;for(0;
_tmp590 != 0;(_tmp590=_tmp590->tl,_tmp591=_tmp591->tl)){struct Cyc_Absyn_Tqual
_tmp593;void*_tmp594;struct _tuple2 _tmp592=*((struct _tuple2*)_tmp590->hd);_tmp593=
_tmp592.f2;_tmp594=_tmp592.f3;{struct Cyc_Absyn_Tqual _tmp596;void*_tmp597;struct
_tuple2 _tmp595=*((struct _tuple2*)((struct Cyc_List_List*)_check_null(_tmp591))->hd);
_tmp596=_tmp595.f2;_tmp597=_tmp595.f3;if(_tmp596.real_const  && !_tmp593.real_const
 || !Cyc_Tcutil_subtype(te,assume,_tmp597,_tmp594))return 0;}}if(_tmp574.c_varargs
!= _tmp576.c_varargs)return 0;if(_tmp574.cyc_varargs != 0  && _tmp576.cyc_varargs != 
0){struct Cyc_Absyn_VarargInfo _tmp598=*_tmp574.cyc_varargs;struct Cyc_Absyn_VarargInfo
_tmp599=*_tmp576.cyc_varargs;if((_tmp599.tq).real_const  && !(_tmp598.tq).real_const
 || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp599.type,(void*)_tmp598.type))
return 0;}else{if(_tmp574.cyc_varargs != 0  || _tmp576.cyc_varargs != 0)return 0;}if(
!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp574.effect))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp576.effect))->v))return 0;if(!Cyc_Tcutil_sub_rgnpo(
_tmp574.rgn_po,_tmp576.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp574.attributes,
_tmp576.attributes))return 0;return 1;}_LL3CF:;_LL3D0: return 0;_LL3CA:;}}static int
Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp59B=({struct _tuple6
_tmp59A;_tmp59A.f1=Cyc_Tcutil_compress(t1);_tmp59A.f2=Cyc_Tcutil_compress(t2);
_tmp59A;});void*_tmp59C;void*_tmp59D;void*_tmp59E;void*_tmp59F;_LL3DB: _tmp59C=
_tmp59B.f1;if(_tmp59C <= (void*)4)goto _LL3DD;if(*((int*)_tmp59C)!= 5)goto _LL3DD;
_tmp59D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp59C)->f2;_tmp59E=_tmp59B.f2;
if(_tmp59E <= (void*)4)goto _LL3DD;if(*((int*)_tmp59E)!= 5)goto _LL3DD;_tmp59F=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp59E)->f2;_LL3DC: return(_tmp59D == 
_tmp59F  || _tmp59D == (void*)2  && _tmp59F == (void*)3) || _tmp59D == (void*)3  && 
_tmp59F == (void*)2;_LL3DD:;_LL3DE: return 0;_LL3DA:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*
tqs1=Cyc_Tcutil_flatten_typ(te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(
te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct
_tuple4 _tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;struct _tuple4*_tmp5A0=(
struct _tuple4*)tqs1->hd;_tmp5A1=*_tmp5A0;_tmp5A2=_tmp5A1.f1;_tmp5A3=_tmp5A1.f2;{
struct _tuple4 _tmp5A5;struct Cyc_Absyn_Tqual _tmp5A6;void*_tmp5A7;struct _tuple4*
_tmp5A4=(struct _tuple4*)tqs2->hd;_tmp5A5=*_tmp5A4;_tmp5A6=_tmp5A5.f1;_tmp5A7=
_tmp5A5.f2;if(_tmp5A6.real_const  && Cyc_Tcutil_subtype(te,assume,_tmp5A3,_tmp5A7))
continue;else{if(Cyc_Tcutil_unify(_tmp5A3,_tmp5A7))continue;else{if(Cyc_Tcutil_isomorphic(
_tmp5A3,_tmp5A7))continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp5A8=Cyc_Tcutil_compress(t);void*_tmp5A9;_LL3E0: if(_tmp5A8 <= (
void*)4)goto _LL3E2;if(*((int*)_tmp5A8)!= 5)goto _LL3E2;_tmp5A9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5A8)->f2;if((int)_tmp5A9 != 0)goto _LL3E2;_LL3E1: return 1;_LL3E2:;_LL3E3: return
0;_LL3DF:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp5AA=t2;
void*_tmp5AB;void*_tmp5AC;_LL3E5: if(_tmp5AA <= (void*)4)goto _LL3E9;if(*((int*)
_tmp5AA)!= 5)goto _LL3E7;_tmp5AB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5AA)->f2;
if((int)_tmp5AB != 2)goto _LL3E7;_LL3E6: goto _LL3E8;_LL3E7: if(*((int*)_tmp5AA)!= 5)
goto _LL3E9;_tmp5AC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5AA)->f2;if((int)
_tmp5AC != 3)goto _LL3E9;_LL3E8: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3E4;_LL3E9:;_LL3EA: goto _LL3E4;_LL3E4:;}{void*_tmp5AD=t1;struct Cyc_Absyn_PtrInfo
_tmp5AE;void*_tmp5AF;struct Cyc_Absyn_Tqual _tmp5B0;struct Cyc_Absyn_PtrAtts _tmp5B1;
void*_tmp5B2;struct Cyc_Absyn_Conref*_tmp5B3;struct Cyc_Absyn_Conref*_tmp5B4;
struct Cyc_Absyn_Conref*_tmp5B5;struct Cyc_Absyn_ArrayInfo _tmp5B6;void*_tmp5B7;
struct Cyc_Absyn_Tqual _tmp5B8;struct Cyc_Absyn_Exp*_tmp5B9;struct Cyc_Absyn_Conref*
_tmp5BA;struct Cyc_Absyn_Enumdecl*_tmp5BB;_LL3EC: if(_tmp5AD <= (void*)4)goto _LL3F4;
if(*((int*)_tmp5AD)!= 4)goto _LL3EE;_tmp5AE=((struct Cyc_Absyn_PointerType_struct*)
_tmp5AD)->f1;_tmp5AF=(void*)_tmp5AE.elt_typ;_tmp5B0=_tmp5AE.elt_tq;_tmp5B1=
_tmp5AE.ptr_atts;_tmp5B2=(void*)_tmp5B1.rgn;_tmp5B3=_tmp5B1.nullable;_tmp5B4=
_tmp5B1.bounds;_tmp5B5=_tmp5B1.zero_term;_LL3ED:{void*_tmp5BC=t2;struct Cyc_Absyn_PtrInfo
_tmp5BD;void*_tmp5BE;struct Cyc_Absyn_Tqual _tmp5BF;struct Cyc_Absyn_PtrAtts _tmp5C0;
void*_tmp5C1;struct Cyc_Absyn_Conref*_tmp5C2;struct Cyc_Absyn_Conref*_tmp5C3;
struct Cyc_Absyn_Conref*_tmp5C4;_LL3FB: if(_tmp5BC <= (void*)4)goto _LL3FD;if(*((int*)
_tmp5BC)!= 4)goto _LL3FD;_tmp5BD=((struct Cyc_Absyn_PointerType_struct*)_tmp5BC)->f1;
_tmp5BE=(void*)_tmp5BD.elt_typ;_tmp5BF=_tmp5BD.elt_tq;_tmp5C0=_tmp5BD.ptr_atts;
_tmp5C1=(void*)_tmp5C0.rgn;_tmp5C2=_tmp5C0.nullable;_tmp5C3=_tmp5C0.bounds;
_tmp5C4=_tmp5C0.zero_term;_LL3FC: {void*coercion=(void*)3;struct Cyc_List_List*
_tmp5C5=({struct Cyc_List_List*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->hd=({
struct _tuple6*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->f1=t1;_tmp5D5->f2=t2;
_tmp5D5;});_tmp5D4->tl=0;_tmp5D4;});int _tmp5C6=Cyc_Tcutil_ptrsubtype(te,_tmp5C5,
_tmp5AF,_tmp5BE) && (!_tmp5B0.real_const  || _tmp5BF.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5B5,_tmp5C4) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp5C4);int _tmp5C7=_tmp5C6?0:((Cyc_Tcutil_bits_only(_tmp5AF) && Cyc_Tcutil_is_char_type(
_tmp5BE)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5C4)) && (_tmp5BF.real_const  || !_tmp5B0.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5B4,_tmp5C3);if(!bounds_ok  && !_tmp5C7){struct
_tuple6 _tmp5C9=({struct _tuple6 _tmp5C8;_tmp5C8.f1=Cyc_Absyn_conref_val(_tmp5B4);
_tmp5C8.f2=Cyc_Absyn_conref_val(_tmp5C3);_tmp5C8;});void*_tmp5CA;struct Cyc_Absyn_Exp*
_tmp5CB;void*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CD;void*_tmp5CE;void*_tmp5CF;void*
_tmp5D0;void*_tmp5D1;void*_tmp5D2;void*_tmp5D3;_LL400: _tmp5CA=_tmp5C9.f1;if(
_tmp5CA <= (void*)2)goto _LL402;if(*((int*)_tmp5CA)!= 0)goto _LL402;_tmp5CB=((
struct Cyc_Absyn_Upper_b_struct*)_tmp5CA)->f1;_tmp5CC=_tmp5C9.f2;if(_tmp5CC <= (
void*)2)goto _LL402;if(*((int*)_tmp5CC)!= 0)goto _LL402;_tmp5CD=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5CC)->f1;_LL401: if(Cyc_Evexp_lte_const_exp(_tmp5CD,_tmp5CB))bounds_ok=1;goto
_LL3FF;_LL402: _tmp5CE=_tmp5C9.f1;if(_tmp5CE <= (void*)2)goto _LL404;if(*((int*)
_tmp5CE)!= 1)goto _LL404;_tmp5CF=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp5CE)->f1;_tmp5D0=_tmp5C9.f2;if(_tmp5D0 <= (void*)2)goto _LL404;if(*((int*)
_tmp5D0)!= 1)goto _LL404;_tmp5D1=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp5D0)->f1;_LL403: bounds_ok=Cyc_Tcutil_unify(_tmp5CF,_tmp5D1);goto _LL3FF;
_LL404: _tmp5D2=_tmp5C9.f1;if(_tmp5D2 <= (void*)2)goto _LL406;if(*((int*)_tmp5D2)!= 
1)goto _LL406;_LL405: goto _LL407;_LL406: _tmp5D3=_tmp5C9.f2;if(_tmp5D3 <= (void*)2)
goto _LL408;if(*((int*)_tmp5D3)!= 1)goto _LL408;_LL407: bounds_ok=0;goto _LL3FF;
_LL408:;_LL409: bounds_ok=1;goto _LL3FF;_LL3FF:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp5B3) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5C2))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp5C6  || _tmp5C7)) && (Cyc_Tcutil_unify(_tmp5B2,_tmp5C1) || Cyc_Tcenv_region_outlives(
te,_tmp5B2,_tmp5C1)))return coercion;else{return(void*)0;}}}_LL3FD:;_LL3FE: goto
_LL3FA;_LL3FA:;}return(void*)0;_LL3EE: if(*((int*)_tmp5AD)!= 7)goto _LL3F0;_tmp5B6=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5AD)->f1;_tmp5B7=(void*)_tmp5B6.elt_type;
_tmp5B8=_tmp5B6.tq;_tmp5B9=_tmp5B6.num_elts;_tmp5BA=_tmp5B6.zero_term;_LL3EF:{
void*_tmp5D6=t2;struct Cyc_Absyn_ArrayInfo _tmp5D7;void*_tmp5D8;struct Cyc_Absyn_Tqual
_tmp5D9;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Conref*_tmp5DB;_LL40B: if(
_tmp5D6 <= (void*)4)goto _LL40D;if(*((int*)_tmp5D6)!= 7)goto _LL40D;_tmp5D7=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5D6)->f1;_tmp5D8=(void*)_tmp5D7.elt_type;
_tmp5D9=_tmp5D7.tq;_tmp5DA=_tmp5D7.num_elts;_tmp5DB=_tmp5D7.zero_term;_LL40C: {
int okay;okay=((_tmp5B9 != 0  && _tmp5DA != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5BA,_tmp5DB)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp5DA,(
struct Cyc_Absyn_Exp*)_tmp5B9);return(okay  && Cyc_Tcutil_unify(_tmp5B7,_tmp5D8))
 && (!_tmp5B8.real_const  || _tmp5D9.real_const)?(void*)3:(void*)0;}_LL40D:;
_LL40E: return(void*)0;_LL40A:;}return(void*)0;_LL3F0: if(*((int*)_tmp5AD)!= 12)
goto _LL3F2;_tmp5BB=((struct Cyc_Absyn_EnumType_struct*)_tmp5AD)->f2;_LL3F1:{void*
_tmp5DC=t2;struct Cyc_Absyn_Enumdecl*_tmp5DD;_LL410: if(_tmp5DC <= (void*)4)goto
_LL412;if(*((int*)_tmp5DC)!= 12)goto _LL412;_tmp5DD=((struct Cyc_Absyn_EnumType_struct*)
_tmp5DC)->f2;_LL411: if((_tmp5BB->fields != 0  && _tmp5DD->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5BB->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5DD->fields))->v))
return(void*)1;goto _LL40F;_LL412:;_LL413: goto _LL40F;_LL40F:;}goto _LL3F3;_LL3F2:
if(*((int*)_tmp5AD)!= 5)goto _LL3F4;_LL3F3: goto _LL3F5;_LL3F4: if((int)_tmp5AD != 1)
goto _LL3F6;_LL3F5: goto _LL3F7;_LL3F6: if(_tmp5AD <= (void*)4)goto _LL3F8;if(*((int*)
_tmp5AD)!= 6)goto _LL3F8;_LL3F7: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL3F8:;_LL3F9: return(void*)0;_LL3EB:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp5DE=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp5DF=_cycalloc(
sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_Absyn_Cast_e_struct _tmp5E0;_tmp5E0.tag=
15;_tmp5E0.f1=(void*)t;_tmp5E0.f2=_tmp5DE;_tmp5E0.f3=0;_tmp5E0.f4=(void*)c;
_tmp5E0;});_tmp5DF;})));e->topt=({struct Cyc_Core_Opt*_tmp5E1=_cycalloc(sizeof(*
_tmp5E1));_tmp5E1->v=(void*)t;_tmp5E1;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp5E2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL415: if(_tmp5E2 <= (void*)4)goto _LL421;if(*((int*)_tmp5E2)!= 5)goto _LL417;
_LL416: goto _LL418;_LL417: if(*((int*)_tmp5E2)!= 12)goto _LL419;_LL418: goto _LL41A;
_LL419: if(*((int*)_tmp5E2)!= 13)goto _LL41B;_LL41A: goto _LL41C;_LL41B: if(*((int*)
_tmp5E2)!= 17)goto _LL41D;_LL41C: goto _LL41E;_LL41D: if(*((int*)_tmp5E2)!= 14)goto
_LL41F;_LL41E: return 1;_LL41F: if(*((int*)_tmp5E2)!= 0)goto _LL421;_LL420: return Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL421:;
_LL422: return 0;_LL414:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp5E3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL424: if((int)_tmp5E3 != 1)goto _LL426;_LL425: goto
_LL427;_LL426: if(_tmp5E3 <= (void*)4)goto _LL428;if(*((int*)_tmp5E3)!= 6)goto
_LL428;_LL427: return 1;_LL428:;_LL429: return 0;_LL423:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp5E4=Cyc_Tcutil_compress(t);_LL42B: if(_tmp5E4 <= (void*)4)goto
_LL42D;if(*((int*)_tmp5E4)!= 8)goto _LL42D;_LL42C: return 1;_LL42D:;_LL42E: return 0;
_LL42A:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple6
_tmp5E6=({struct _tuple6 _tmp5E5;_tmp5E5.f1=t1;_tmp5E5.f2=t2;_tmp5E5;});void*
_tmp5E7;int _tmp5E8;void*_tmp5E9;int _tmp5EA;void*_tmp5EB;void*_tmp5EC;void*
_tmp5ED;void*_tmp5EE;void*_tmp5EF;void*_tmp5F0;void*_tmp5F1;void*_tmp5F2;void*
_tmp5F3;void*_tmp5F4;void*_tmp5F5;void*_tmp5F6;void*_tmp5F7;void*_tmp5F8;void*
_tmp5F9;void*_tmp5FA;void*_tmp5FB;void*_tmp5FC;void*_tmp5FD;void*_tmp5FE;void*
_tmp5FF;void*_tmp600;void*_tmp601;void*_tmp602;void*_tmp603;void*_tmp604;void*
_tmp605;void*_tmp606;void*_tmp607;void*_tmp608;_LL430: _tmp5E7=_tmp5E6.f1;if(
_tmp5E7 <= (void*)4)goto _LL432;if(*((int*)_tmp5E7)!= 6)goto _LL432;_tmp5E8=((
struct Cyc_Absyn_DoubleType_struct*)_tmp5E7)->f1;_tmp5E9=_tmp5E6.f2;if(_tmp5E9 <= (
void*)4)goto _LL432;if(*((int*)_tmp5E9)!= 6)goto _LL432;_tmp5EA=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5E9)->f1;_LL431: if(_tmp5E8)return t1;else{return t2;}_LL432: _tmp5EB=_tmp5E6.f1;
if(_tmp5EB <= (void*)4)goto _LL434;if(*((int*)_tmp5EB)!= 6)goto _LL434;_LL433:
return t1;_LL434: _tmp5EC=_tmp5E6.f2;if(_tmp5EC <= (void*)4)goto _LL436;if(*((int*)
_tmp5EC)!= 6)goto _LL436;_LL435: return t2;_LL436: _tmp5ED=_tmp5E6.f1;if((int)
_tmp5ED != 1)goto _LL438;_LL437: goto _LL439;_LL438: _tmp5EE=_tmp5E6.f2;if((int)
_tmp5EE != 1)goto _LL43A;_LL439: return(void*)1;_LL43A: _tmp5EF=_tmp5E6.f1;if(
_tmp5EF <= (void*)4)goto _LL43C;if(*((int*)_tmp5EF)!= 5)goto _LL43C;_tmp5F0=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5EF)->f1;if((int)_tmp5F0 != 1)goto _LL43C;
_tmp5F1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EF)->f2;if((int)_tmp5F1 != 
4)goto _LL43C;_LL43B: goto _LL43D;_LL43C: _tmp5F2=_tmp5E6.f2;if(_tmp5F2 <= (void*)4)
goto _LL43E;if(*((int*)_tmp5F2)!= 5)goto _LL43E;_tmp5F3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F2)->f1;if((int)_tmp5F3 != 1)goto _LL43E;_tmp5F4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F2)->f2;if((int)_tmp5F4 != 4)goto _LL43E;_LL43D: return Cyc_Absyn_ulonglong_typ;
_LL43E: _tmp5F5=_tmp5E6.f1;if(_tmp5F5 <= (void*)4)goto _LL440;if(*((int*)_tmp5F5)!= 
5)goto _LL440;_tmp5F6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F5)->f2;if((
int)_tmp5F6 != 4)goto _LL440;_LL43F: goto _LL441;_LL440: _tmp5F7=_tmp5E6.f2;if(
_tmp5F7 <= (void*)4)goto _LL442;if(*((int*)_tmp5F7)!= 5)goto _LL442;_tmp5F8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F7)->f2;if((int)_tmp5F8 != 4)goto _LL442;
_LL441: return Cyc_Absyn_slonglong_typ;_LL442: _tmp5F9=_tmp5E6.f1;if(_tmp5F9 <= (
void*)4)goto _LL444;if(*((int*)_tmp5F9)!= 5)goto _LL444;_tmp5FA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F9)->f1;if((int)_tmp5FA != 1)goto _LL444;_tmp5FB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F9)->f2;if((int)_tmp5FB != 3)goto _LL444;_LL443: goto _LL445;_LL444: _tmp5FC=
_tmp5E6.f2;if(_tmp5FC <= (void*)4)goto _LL446;if(*((int*)_tmp5FC)!= 5)goto _LL446;
_tmp5FD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FC)->f1;if((int)_tmp5FD != 
1)goto _LL446;_tmp5FE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FC)->f2;if((
int)_tmp5FE != 3)goto _LL446;_LL445: return Cyc_Absyn_ulong_typ;_LL446: _tmp5FF=
_tmp5E6.f1;if(_tmp5FF <= (void*)4)goto _LL448;if(*((int*)_tmp5FF)!= 5)goto _LL448;
_tmp600=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FF)->f1;if((int)_tmp600 != 
1)goto _LL448;_tmp601=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FF)->f2;if((
int)_tmp601 != 2)goto _LL448;_LL447: goto _LL449;_LL448: _tmp602=_tmp5E6.f2;if(
_tmp602 <= (void*)4)goto _LL44A;if(*((int*)_tmp602)!= 5)goto _LL44A;_tmp603=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp602)->f1;if((int)_tmp603 != 1)goto _LL44A;
_tmp604=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp602)->f2;if((int)_tmp604 != 
2)goto _LL44A;_LL449: return Cyc_Absyn_uint_typ;_LL44A: _tmp605=_tmp5E6.f1;if(
_tmp605 <= (void*)4)goto _LL44C;if(*((int*)_tmp605)!= 5)goto _LL44C;_tmp606=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp605)->f2;if((int)_tmp606 != 3)goto _LL44C;
_LL44B: goto _LL44D;_LL44C: _tmp607=_tmp5E6.f2;if(_tmp607 <= (void*)4)goto _LL44E;if(*((
int*)_tmp607)!= 5)goto _LL44E;_tmp608=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp607)->f2;if((int)_tmp608 != 3)goto _LL44E;_LL44D: return Cyc_Absyn_slong_typ;
_LL44E:;_LL44F: return Cyc_Absyn_sint_typ;_LL42F:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp609=(void*)e->r;struct Cyc_Core_Opt*_tmp60A;
_LL451: if(*((int*)_tmp609)!= 4)goto _LL453;_tmp60A=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp609)->f2;if(_tmp60A != 0)goto _LL453;_LL452:({void*_tmp60B[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp60C="assignment in test";_tag_dynforward(_tmp60C,sizeof(
char),_get_zero_arr_size(_tmp60C,19));}),_tag_dynforward(_tmp60B,sizeof(void*),0));});
goto _LL450;_LL453:;_LL454: goto _LL450;_LL450:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp60E=({struct _tuple6 _tmp60D;_tmp60D.f1=c1;_tmp60D.f2=c2;_tmp60D;});
void*_tmp60F;void*_tmp610;void*_tmp611;void*_tmp612;void*_tmp613;struct Cyc_Core_Opt*
_tmp614;struct Cyc_Core_Opt**_tmp615;void*_tmp616;struct Cyc_Core_Opt*_tmp617;
struct Cyc_Core_Opt**_tmp618;void*_tmp619;struct Cyc_Core_Opt*_tmp61A;struct Cyc_Core_Opt**
_tmp61B;void*_tmp61C;void*_tmp61D;void*_tmp61E;void*_tmp61F;void*_tmp620;void*
_tmp621;struct Cyc_Core_Opt*_tmp622;struct Cyc_Core_Opt**_tmp623;void*_tmp624;void*
_tmp625;struct Cyc_Core_Opt*_tmp626;struct Cyc_Core_Opt**_tmp627;void*_tmp628;void*
_tmp629;struct Cyc_Core_Opt*_tmp62A;struct Cyc_Core_Opt**_tmp62B;void*_tmp62C;
_LL456: _tmp60F=_tmp60E.f1;if(*((int*)_tmp60F)!= 0)goto _LL458;_tmp610=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp60F)->f1;_tmp611=_tmp60E.f2;if(*((int*)_tmp611)
!= 0)goto _LL458;_tmp612=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp611)->f1;
_LL457: return _tmp610 == _tmp612;_LL458: _tmp613=_tmp60E.f2;if(*((int*)_tmp613)!= 1)
goto _LL45A;_tmp614=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp613)->f1;_tmp615=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp613)->f1;_LL459:*
_tmp615=({struct Cyc_Core_Opt*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->v=(
void*)c1;_tmp62D;});return 1;_LL45A: _tmp616=_tmp60E.f1;if(*((int*)_tmp616)!= 1)
goto _LL45C;_tmp617=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp616)->f1;_tmp618=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp616)->f1;_LL45B:*
_tmp618=({struct Cyc_Core_Opt*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->v=(
void*)c2;_tmp62E;});return 1;_LL45C: _tmp619=_tmp60E.f1;if(*((int*)_tmp619)!= 2)
goto _LL45E;_tmp61A=((struct Cyc_Absyn_Less_kb_struct*)_tmp619)->f1;_tmp61B=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp619)->f1;_tmp61C=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp619)->f2;_tmp61D=_tmp60E.f2;if(*((
int*)_tmp61D)!= 0)goto _LL45E;_tmp61E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp61D)->f1;_LL45D: if(Cyc_Tcutil_kind_leq(_tmp61E,_tmp61C)){*_tmp61B=({struct
Cyc_Core_Opt*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->v=(void*)c2;_tmp62F;});
return 1;}else{return 0;}_LL45E: _tmp61F=_tmp60E.f1;if(*((int*)_tmp61F)!= 0)goto
_LL460;_tmp620=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp61F)->f1;_tmp621=
_tmp60E.f2;if(*((int*)_tmp621)!= 2)goto _LL460;_tmp622=((struct Cyc_Absyn_Less_kb_struct*)
_tmp621)->f1;_tmp623=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp621)->f1;_tmp624=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp621)->f2;
_LL45F: if(Cyc_Tcutil_kind_leq(_tmp620,_tmp624)){*_tmp623=({struct Cyc_Core_Opt*
_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->v=(void*)c1;_tmp630;});return 1;}
else{return 0;}_LL460: _tmp625=_tmp60E.f1;if(*((int*)_tmp625)!= 2)goto _LL455;
_tmp626=((struct Cyc_Absyn_Less_kb_struct*)_tmp625)->f1;_tmp627=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp625)->f1;_tmp628=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp625)->f2;_tmp629=_tmp60E.f2;if(*((int*)_tmp629)!= 2)goto _LL455;_tmp62A=((
struct Cyc_Absyn_Less_kb_struct*)_tmp629)->f1;_tmp62B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp629)->f1;_tmp62C=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp629)->f2;_LL461: if(Cyc_Tcutil_kind_leq(_tmp628,_tmp62C)){*_tmp62B=({struct
Cyc_Core_Opt*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->v=(void*)c1;_tmp631;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp62C,_tmp628)){*_tmp627=({struct Cyc_Core_Opt*
_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->v=(void*)c2;_tmp632;});return 1;}
else{return 0;}}_LL455:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp633=_cycalloc_atomic(sizeof(*_tmp633));_tmp633[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp633;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dynforward_ptr s=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp638;_tmp638.tag=1;_tmp638.f1=(
unsigned long)i;{void*_tmp636[1]={& _tmp638};Cyc_aprintf(({const char*_tmp637="#%d";
_tag_dynforward(_tmp637,sizeof(char),_get_zero_arr_size(_tmp637,4));}),
_tag_dynforward(_tmp636,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->name=({struct _dynforward_ptr*
_tmp635=_cycalloc(sizeof(struct _dynforward_ptr)* 1);_tmp635[0]=s;_tmp635;});
_tmp634->identity=0;_tmp634->kind=(void*)k;_tmp634;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dynforward_ptr _tmp639=*t->name;return*((const char*)
_check_dynforward_subscript(_tmp639,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp63C;_tmp63C.tag=0;
_tmp63C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*t->name);{void*
_tmp63A[1]={& _tmp63C};Cyc_printf(({const char*_tmp63B="%s";_tag_dynforward(
_tmp63B,sizeof(char),_get_zero_arr_size(_tmp63B,3));}),_tag_dynforward(_tmp63A,
sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(t))return;{struct
_dynforward_ptr _tmp63D=Cyc_strconcat(({const char*_tmp642="`";_tag_dynforward(
_tmp642,sizeof(char),_get_zero_arr_size(_tmp642,2));}),(struct _dynforward_ptr)*t->name);({
struct _dynforward_ptr _tmp63E=_dynforward_ptr_plus(_tmp63D,sizeof(char),1);char
_tmp63F=*((char*)_check_dynforward_subscript(_tmp63E,sizeof(char),0));char
_tmp640='t';if(_get_dynforward_size(_tmp63E,sizeof(char))== 1  && (_tmp63F == '\000'
 && _tmp640 != '\000'))_throw_arraybounds();*((char*)_tmp63E.curr)=_tmp640;});t->name=({
struct _dynforward_ptr*_tmp641=_cycalloc(sizeof(struct _dynforward_ptr)* 1);
_tmp641[0]=(struct _dynforward_ptr)_tmp63D;_tmp641;});}}struct _tuple13{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp643=_cycalloc(sizeof(*_tmp643));
_tmp643->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp644=_cycalloc(sizeof(*
_tmp644));_tmp644->v=(*x).f1;_tmp644;});_tmp643->f2=(*x).f2;_tmp643->f3=(*x).f3;
_tmp643;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp645=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp645=({
struct Cyc_List_List*_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646->hd=(void*)((
void*)atts->hd);_tmp646->tl=_tmp645;_tmp646;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_FnType_struct
_tmp648;_tmp648.tag=8;_tmp648.f1=({struct Cyc_Absyn_FnInfo _tmp649;_tmp649.tvars=
fd->tvs;_tmp649.effect=fd->effect;_tmp649.ret_typ=(void*)((void*)fd->ret_type);
_tmp649.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp649.c_varargs=
fd->c_varargs;_tmp649.cyc_varargs=fd->cyc_varargs;_tmp649.rgn_po=fd->rgn_po;
_tmp649.attributes=_tmp645;_tmp649;});_tmp648;});_tmp647;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->f1=(*pr).f1;_tmp64A->f2=t;_tmp64A;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp64B=_region_malloc(
rgn,sizeof(*_tmp64B));_tmp64B->f1=({struct _tuple15*_tmp64C=_region_malloc(rgn,
sizeof(*_tmp64C));_tmp64C->f1=(*y).f1;_tmp64C->f2=(*y).f2;_tmp64C;});_tmp64B->f2=(*
y).f3;_tmp64B;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp64E;void*_tmp64F;struct _tuple16 _tmp64D=*w;_tmp64E=_tmp64D.f1;
_tmp64F=_tmp64D.f2;{struct Cyc_Core_Opt*_tmp651;struct Cyc_Absyn_Tqual _tmp652;
struct _tuple15 _tmp650=*_tmp64E;_tmp651=_tmp650.f1;_tmp652=_tmp650.f2;return({
struct _tuple2*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->f1=_tmp651;_tmp653->f2=
_tmp652;_tmp653->f3=_tmp64F;_tmp653;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp654=
_cycalloc(sizeof(*_tmp654));_tmp654->name=f->name;_tmp654->tq=f->tq;_tmp654->type=(
void*)t;_tmp654->width=f->width;_tmp654->attributes=f->attributes;_tmp654;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp655=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp656;struct Cyc_Absyn_AggrInfo _tmp657;void*_tmp658;struct Cyc_List_List*_tmp659;
struct Cyc_Absyn_TunionInfo _tmp65A;void*_tmp65B;struct Cyc_List_List*_tmp65C;void*
_tmp65D;struct Cyc_Absyn_TunionFieldInfo _tmp65E;void*_tmp65F;struct Cyc_List_List*
_tmp660;struct _tuple1*_tmp661;struct Cyc_List_List*_tmp662;struct Cyc_Absyn_Typedefdecl*
_tmp663;void**_tmp664;struct Cyc_Absyn_ArrayInfo _tmp665;void*_tmp666;struct Cyc_Absyn_Tqual
_tmp667;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Conref*_tmp669;struct Cyc_Position_Segment*
_tmp66A;struct Cyc_Absyn_PtrInfo _tmp66B;void*_tmp66C;struct Cyc_Absyn_Tqual _tmp66D;
struct Cyc_Absyn_PtrAtts _tmp66E;void*_tmp66F;struct Cyc_Absyn_Conref*_tmp670;
struct Cyc_Absyn_Conref*_tmp671;struct Cyc_Absyn_Conref*_tmp672;struct Cyc_Absyn_FnInfo
_tmp673;struct Cyc_List_List*_tmp674;struct Cyc_Core_Opt*_tmp675;void*_tmp676;
struct Cyc_List_List*_tmp677;int _tmp678;struct Cyc_Absyn_VarargInfo*_tmp679;struct
Cyc_List_List*_tmp67A;struct Cyc_List_List*_tmp67B;struct Cyc_List_List*_tmp67C;
void*_tmp67D;struct Cyc_List_List*_tmp67E;struct Cyc_Core_Opt*_tmp67F;void*_tmp680;
void*_tmp681;void*_tmp682;void*_tmp683;void*_tmp684;struct Cyc_List_List*_tmp685;
_LL463: if(_tmp655 <= (void*)4)goto _LL485;if(*((int*)_tmp655)!= 1)goto _LL465;
_tmp656=((struct Cyc_Absyn_VarType_struct*)_tmp655)->f1;_LL464: {struct
_handler_cons _tmp686;_push_handler(& _tmp686);{int _tmp688=0;if(setjmp(_tmp686.handler))
_tmp688=1;if(!_tmp688){{void*_tmp689=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(
Cyc_Absyn_tvar_cmp,inst,_tmp656);_npop_handler(0);return _tmp689;};_pop_handler();}
else{void*_tmp687=(void*)_exn_thrown;void*_tmp68B=_tmp687;_LL498: if(_tmp68B != 
Cyc_Core_Not_found)goto _LL49A;_LL499: return t;_LL49A:;_LL49B:(void)_throw(_tmp68B);
_LL497:;}}}_LL465: if(*((int*)_tmp655)!= 10)goto _LL467;_tmp657=((struct Cyc_Absyn_AggrType_struct*)
_tmp655)->f1;_tmp658=(void*)_tmp657.aggr_info;_tmp659=_tmp657.targs;_LL466: {
struct Cyc_List_List*_tmp68C=Cyc_Tcutil_substs(rgn,inst,_tmp659);return _tmp68C == 
_tmp659?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp68D=_cycalloc(sizeof(*
_tmp68D));_tmp68D[0]=({struct Cyc_Absyn_AggrType_struct _tmp68E;_tmp68E.tag=10;
_tmp68E.f1=({struct Cyc_Absyn_AggrInfo _tmp68F;_tmp68F.aggr_info=(void*)_tmp658;
_tmp68F.targs=_tmp68C;_tmp68F;});_tmp68E;});_tmp68D;});}_LL467: if(*((int*)
_tmp655)!= 2)goto _LL469;_tmp65A=((struct Cyc_Absyn_TunionType_struct*)_tmp655)->f1;
_tmp65B=(void*)_tmp65A.tunion_info;_tmp65C=_tmp65A.targs;_tmp65D=(void*)_tmp65A.rgn;
_LL468: {struct Cyc_List_List*_tmp690=Cyc_Tcutil_substs(rgn,inst,_tmp65C);void*
_tmp691=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp65D);return _tmp690 == _tmp65C  && 
_tmp691 == _tmp65D?t:(void*)({struct Cyc_Absyn_TunionType_struct*_tmp692=_cycalloc(
sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_TunionType_struct _tmp693;_tmp693.tag=
2;_tmp693.f1=({struct Cyc_Absyn_TunionInfo _tmp694;_tmp694.tunion_info=(void*)
_tmp65B;_tmp694.targs=_tmp690;_tmp694.rgn=(void*)_tmp691;_tmp694;});_tmp693;});
_tmp692;});}_LL469: if(*((int*)_tmp655)!= 3)goto _LL46B;_tmp65E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp655)->f1;_tmp65F=(void*)_tmp65E.field_info;_tmp660=_tmp65E.targs;_LL46A: {
struct Cyc_List_List*_tmp695=Cyc_Tcutil_substs(rgn,inst,_tmp660);return _tmp695 == 
_tmp660?t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp696=_cycalloc(
sizeof(*_tmp696));_tmp696[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp697;
_tmp697.tag=3;_tmp697.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp698;_tmp698.field_info=(
void*)_tmp65F;_tmp698.targs=_tmp695;_tmp698;});_tmp697;});_tmp696;});}_LL46B: if(*((
int*)_tmp655)!= 16)goto _LL46D;_tmp661=((struct Cyc_Absyn_TypedefType_struct*)
_tmp655)->f1;_tmp662=((struct Cyc_Absyn_TypedefType_struct*)_tmp655)->f2;_tmp663=((
struct Cyc_Absyn_TypedefType_struct*)_tmp655)->f3;_tmp664=((struct Cyc_Absyn_TypedefType_struct*)
_tmp655)->f4;_LL46C: {struct Cyc_List_List*_tmp699=Cyc_Tcutil_substs(rgn,inst,
_tmp662);return _tmp699 == _tmp662?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp69B;_tmp69B.tag=16;_tmp69B.f1=_tmp661;_tmp69B.f2=_tmp699;_tmp69B.f3=_tmp663;
_tmp69B.f4=_tmp664;_tmp69B;});_tmp69A;});}_LL46D: if(*((int*)_tmp655)!= 7)goto
_LL46F;_tmp665=((struct Cyc_Absyn_ArrayType_struct*)_tmp655)->f1;_tmp666=(void*)
_tmp665.elt_type;_tmp667=_tmp665.tq;_tmp668=_tmp665.num_elts;_tmp669=_tmp665.zero_term;
_tmp66A=_tmp665.zt_loc;_LL46E: {void*_tmp69C=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp666);return _tmp69C == _tmp666?t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp69E;_tmp69E.tag=7;_tmp69E.f1=({struct Cyc_Absyn_ArrayInfo _tmp69F;_tmp69F.elt_type=(
void*)_tmp69C;_tmp69F.tq=_tmp667;_tmp69F.num_elts=_tmp668;_tmp69F.zero_term=
_tmp669;_tmp69F.zt_loc=_tmp66A;_tmp69F;});_tmp69E;});_tmp69D;});}_LL46F: if(*((
int*)_tmp655)!= 4)goto _LL471;_tmp66B=((struct Cyc_Absyn_PointerType_struct*)
_tmp655)->f1;_tmp66C=(void*)_tmp66B.elt_typ;_tmp66D=_tmp66B.elt_tq;_tmp66E=
_tmp66B.ptr_atts;_tmp66F=(void*)_tmp66E.rgn;_tmp670=_tmp66E.nullable;_tmp671=
_tmp66E.bounds;_tmp672=_tmp66E.zero_term;_LL470: {void*_tmp6A0=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp66C);void*_tmp6A1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp66F);struct
Cyc_Absyn_Conref*_tmp6A2=_tmp671;{void*_tmp6A3=(void*)(Cyc_Absyn_compress_conref(
_tmp671))->v;void*_tmp6A4;void*_tmp6A5;_LL49D: if(_tmp6A3 <= (void*)1)goto _LL49F;
if(*((int*)_tmp6A3)!= 0)goto _LL49F;_tmp6A4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp6A3)->f1;if(_tmp6A4 <= (void*)2)goto _LL49F;if(*((int*)_tmp6A4)!= 1)goto _LL49F;
_tmp6A5=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp6A4)->f1;_LL49E: {void*
_tmp6A6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A5);if(_tmp6A5 != _tmp6A6)_tmp6A2=
Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp6A7=
_cycalloc(sizeof(*_tmp6A7));_tmp6A7[0]=({struct Cyc_Absyn_AbsUpper_b_struct
_tmp6A8;_tmp6A8.tag=1;_tmp6A8.f1=(void*)_tmp6A6;_tmp6A8;});_tmp6A7;}));goto
_LL49C;}_LL49F:;_LL4A0: goto _LL49C;_LL49C:;}if((_tmp6A0 == _tmp66C  && _tmp6A1 == 
_tmp66F) && _tmp6A2 == _tmp671)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6AA;_tmp6AA.tag=4;_tmp6AA.f1=({struct Cyc_Absyn_PtrInfo _tmp6AB;_tmp6AB.elt_typ=(
void*)_tmp6A0;_tmp6AB.elt_tq=_tmp66D;_tmp6AB.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6AC;_tmp6AC.rgn=(void*)_tmp6A1;_tmp6AC.nullable=_tmp670;_tmp6AC.bounds=
_tmp6A2;_tmp6AC.zero_term=_tmp672;_tmp6AC.ptrloc=0;_tmp6AC;});_tmp6AB;});_tmp6AA;});
_tmp6A9;});}_LL471: if(*((int*)_tmp655)!= 8)goto _LL473;_tmp673=((struct Cyc_Absyn_FnType_struct*)
_tmp655)->f1;_tmp674=_tmp673.tvars;_tmp675=_tmp673.effect;_tmp676=(void*)_tmp673.ret_typ;
_tmp677=_tmp673.args;_tmp678=_tmp673.c_varargs;_tmp679=_tmp673.cyc_varargs;
_tmp67A=_tmp673.rgn_po;_tmp67B=_tmp673.attributes;_LL472:{struct Cyc_List_List*
_tmp6AD=_tmp674;for(0;_tmp6AD != 0;_tmp6AD=_tmp6AD->tl){inst=({struct Cyc_List_List*
_tmp6AE=_region_malloc(rgn,sizeof(*_tmp6AE));_tmp6AE->hd=({struct _tuple8*_tmp6AF=
_region_malloc(rgn,sizeof(*_tmp6AF));_tmp6AF->f1=(struct Cyc_Absyn_Tvar*)_tmp6AD->hd;
_tmp6AF->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp6B0=_cycalloc(sizeof(*
_tmp6B0));_tmp6B0[0]=({struct Cyc_Absyn_VarType_struct _tmp6B1;_tmp6B1.tag=1;
_tmp6B1.f1=(struct Cyc_Absyn_Tvar*)_tmp6AD->hd;_tmp6B1;});_tmp6B0;});_tmp6AF;});
_tmp6AE->tl=inst;_tmp6AE;});}}{struct Cyc_List_List*_tmp6B3;struct Cyc_List_List*
_tmp6B4;struct _tuple0 _tmp6B2=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp677));_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;{struct Cyc_List_List*_tmp6B5=
Cyc_Tcutil_substs(rgn,inst,_tmp6B4);struct Cyc_List_List*_tmp6B6=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6B3,_tmp6B5));struct Cyc_Core_Opt*
eff2;if(_tmp675 == 0)eff2=0;else{void*_tmp6B7=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp675->v);if(_tmp6B7 == (void*)_tmp675->v)eff2=_tmp675;else{eff2=({struct
Cyc_Core_Opt*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->v=(void*)_tmp6B7;
_tmp6B8;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp679 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6BA;struct Cyc_Absyn_Tqual _tmp6BB;void*
_tmp6BC;int _tmp6BD;struct Cyc_Absyn_VarargInfo _tmp6B9=*_tmp679;_tmp6BA=_tmp6B9.name;
_tmp6BB=_tmp6B9.tq;_tmp6BC=(void*)_tmp6B9.type;_tmp6BD=_tmp6B9.inject;{void*
_tmp6BE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BC);if(_tmp6BE == _tmp6BC)
cyc_varargs2=_tmp679;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6BF=
_cycalloc(sizeof(*_tmp6BF));_tmp6BF->name=_tmp6BA;_tmp6BF->tq=_tmp6BB;_tmp6BF->type=(
void*)_tmp6BE;_tmp6BF->inject=_tmp6BD;_tmp6BF;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp6C1;struct Cyc_List_List*_tmp6C2;struct _tuple0 _tmp6C0=Cyc_List_rsplit(
rgn,rgn,_tmp67A);_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;{struct Cyc_List_List*
_tmp6C3=Cyc_Tcutil_substs(rgn,inst,_tmp6C1);struct Cyc_List_List*_tmp6C4=Cyc_Tcutil_substs(
rgn,inst,_tmp6C2);if(_tmp6C3 == _tmp6C1  && _tmp6C4 == _tmp6C2)rgn_po2=_tmp67A;
else{rgn_po2=Cyc_List_zip(_tmp6C3,_tmp6C4);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_FnType_struct
_tmp6C6;_tmp6C6.tag=8;_tmp6C6.f1=({struct Cyc_Absyn_FnInfo _tmp6C7;_tmp6C7.tvars=
_tmp674;_tmp6C7.effect=eff2;_tmp6C7.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp676);_tmp6C7.args=_tmp6B6;_tmp6C7.c_varargs=_tmp678;_tmp6C7.cyc_varargs=
cyc_varargs2;_tmp6C7.rgn_po=rgn_po2;_tmp6C7.attributes=_tmp67B;_tmp6C7;});
_tmp6C6;});_tmp6C5;});}}}}}_LL473: if(*((int*)_tmp655)!= 9)goto _LL475;_tmp67C=((
struct Cyc_Absyn_TupleType_struct*)_tmp655)->f1;_LL474: {struct Cyc_List_List*
_tmp6C8=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple4*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp67C);struct Cyc_List_List*
_tmp6C9=Cyc_Tcutil_substs(rgn,inst,_tmp6C8);if(_tmp6C9 == _tmp6C8)return t;{struct
Cyc_List_List*_tmp6CA=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp67C,_tmp6C9);return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp6CB=
_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({struct Cyc_Absyn_TupleType_struct _tmp6CC;
_tmp6CC.tag=9;_tmp6CC.f1=_tmp6CA;_tmp6CC;});_tmp6CB;});}}_LL475: if(*((int*)
_tmp655)!= 11)goto _LL477;_tmp67D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp655)->f1;_tmp67E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp655)->f2;_LL476: {
struct Cyc_List_List*_tmp6CD=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp67E);struct Cyc_List_List*_tmp6CE=Cyc_Tcutil_substs(rgn,inst,_tmp6CD);if(
_tmp6CE == _tmp6CD)return t;{struct Cyc_List_List*_tmp6CF=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp67E,
_tmp6CE);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp6D0=_cycalloc(
sizeof(*_tmp6D0));_tmp6D0[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp6D1;
_tmp6D1.tag=11;_tmp6D1.f1=(void*)_tmp67D;_tmp6D1.f2=_tmp6CF;_tmp6D1;});_tmp6D0;});}}
_LL477: if(*((int*)_tmp655)!= 0)goto _LL479;_tmp67F=((struct Cyc_Absyn_Evar_struct*)
_tmp655)->f2;_LL478: if(_tmp67F != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp67F->v);else{return t;}_LL479: if(*((int*)_tmp655)!= 15)goto _LL47B;_tmp680=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp655)->f1;_LL47A: {void*_tmp6D2=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp680);return _tmp6D2 == _tmp680?t:(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6D4;_tmp6D4.tag=15;_tmp6D4.f1=(void*)
_tmp6D2;_tmp6D4;});_tmp6D3;});}_LL47B: if(*((int*)_tmp655)!= 14)goto _LL47D;
_tmp681=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp655)->f1;_LL47C: {void*
_tmp6D5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp681);return _tmp6D5 == _tmp681?t:(void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp6D7;_tmp6D7.tag=14;_tmp6D7.f1=(void*)
_tmp6D5;_tmp6D7;});_tmp6D6;});}_LL47D: if(*((int*)_tmp655)!= 17)goto _LL47F;
_tmp682=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp655)->f1;_LL47E: {void*
_tmp6D8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp682);return _tmp6D8 == _tmp682?t:(void*)({
struct Cyc_Absyn_TagType_struct*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=({
struct Cyc_Absyn_TagType_struct _tmp6DA;_tmp6DA.tag=17;_tmp6DA.f1=(void*)_tmp6D8;
_tmp6DA;});_tmp6D9;});}_LL47F: if(*((int*)_tmp655)!= 18)goto _LL481;_LL480: goto
_LL482;_LL481: if(*((int*)_tmp655)!= 12)goto _LL483;_LL482: goto _LL484;_LL483: if(*((
int*)_tmp655)!= 13)goto _LL485;_LL484: goto _LL486;_LL485: if((int)_tmp655 != 0)goto
_LL487;_LL486: goto _LL488;_LL487: if(_tmp655 <= (void*)4)goto _LL489;if(*((int*)
_tmp655)!= 5)goto _LL489;_LL488: goto _LL48A;_LL489: if((int)_tmp655 != 1)goto _LL48B;
_LL48A: goto _LL48C;_LL48B: if(_tmp655 <= (void*)4)goto _LL48D;if(*((int*)_tmp655)!= 
6)goto _LL48D;_LL48C: goto _LL48E;_LL48D: if((int)_tmp655 != 3)goto _LL48F;_LL48E: goto
_LL490;_LL48F: if((int)_tmp655 != 2)goto _LL491;_LL490: return t;_LL491: if(_tmp655 <= (
void*)4)goto _LL493;if(*((int*)_tmp655)!= 21)goto _LL493;_tmp683=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp655)->f1;_LL492: {void*_tmp6DB=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp683);return _tmp6DB == _tmp683?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6DD;_tmp6DD.tag=21;_tmp6DD.f1=(void*)_tmp6DB;_tmp6DD;});_tmp6DC;});}_LL493:
if(_tmp655 <= (void*)4)goto _LL495;if(*((int*)_tmp655)!= 19)goto _LL495;_tmp684=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp655)->f1;_LL494: {void*_tmp6DE=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp684);return _tmp6DE == _tmp684?t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6E0;_tmp6E0.tag=19;_tmp6E0.f1=(void*)_tmp6DE;_tmp6E0;});_tmp6DF;});}_LL495:
if(_tmp655 <= (void*)4)goto _LL462;if(*((int*)_tmp655)!= 20)goto _LL462;_tmp685=((
struct Cyc_Absyn_JoinEff_struct*)_tmp655)->f1;_LL496: {struct Cyc_List_List*
_tmp6E1=Cyc_Tcutil_substs(rgn,inst,_tmp685);return _tmp6E1 == _tmp685?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp6E3;_tmp6E3.tag=20;_tmp6E3.f1=_tmp6E1;_tmp6E3;});
_tmp6E2;});}_LL462:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp6E4=(void*)ts->hd;struct Cyc_List_List*_tmp6E5=ts->tl;void*
_tmp6E6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E4);struct Cyc_List_List*_tmp6E7=Cyc_Tcutil_substs(
rgn,inst,_tmp6E5);if(_tmp6E4 == _tmp6E6  && _tmp6E5 == _tmp6E7)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp6E8=
_cycalloc(sizeof(*_tmp6E8));_tmp6E8->hd=(void*)_tmp6E6;_tmp6E8->tl=_tmp6E7;
_tmp6E8;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple8*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->f1=tv;_tmp6E9->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));
_tmp6EA->v=(void*)k;_tmp6EA;}),({struct Cyc_Core_Opt*_tmp6EB=_cycalloc(sizeof(*
_tmp6EB));_tmp6EB->v=s;_tmp6EB;}));_tmp6E9;});}struct _tuple8*Cyc_Tcutil_r_make_inst_var(
struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){struct _tuple9 _tmp6ED;struct Cyc_List_List*
_tmp6EE;struct _RegionHandle*_tmp6EF;struct _tuple9*_tmp6EC=env;_tmp6ED=*_tmp6EC;
_tmp6EE=_tmp6ED.f1;_tmp6EF=_tmp6ED.f2;{void*k=Cyc_Tcutil_tvar_kind(tv);return({
struct _tuple8*_tmp6F0=_region_malloc(_tmp6EF,sizeof(*_tmp6F0));_tmp6F0->f1=tv;
_tmp6F0->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp6F1=_cycalloc(sizeof(*
_tmp6F1));_tmp6F1->v=(void*)k;_tmp6F1;}),({struct Cyc_Core_Opt*_tmp6F2=_cycalloc(
sizeof(*_tmp6F2));_tmp6F2->v=_tmp6EE;_tmp6F2;}));_tmp6F0;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp6F7;_tmp6F7.tag=0;_tmp6F7.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k2));{
struct Cyc_String_pa_struct _tmp6F6;_tmp6F6.tag=0;_tmp6F6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(k1));{
struct Cyc_String_pa_struct _tmp6F5;_tmp6F5.tag=0;_tmp6F5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*tv->name);{void*_tmp6F3[3]={& _tmp6F5,&
_tmp6F6,& _tmp6F7};Cyc_Tcutil_terr(loc,({const char*_tmp6F4="type variable %s is used with inconsistent kinds %s and %s";
_tag_dynforward(_tmp6F4,sizeof(char),_get_zero_arr_size(_tmp6F4,59));}),
_tag_dynforward(_tmp6F3,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp6F8[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp6F9="same type variable has different identity!";_tag_dynforward(
_tmp6F9,sizeof(char),_get_zero_arr_size(_tmp6F9,43));}),_tag_dynforward(_tmp6F8,
sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({
struct Cyc_List_List*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA->hd=tv;_tmp6FA->tl=
tvs;_tmp6FA;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp6FB[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp6FC="fast_add_free_tvar: bad identity in tv";_tag_dynforward(
_tmp6FC,sizeof(char),_get_zero_arr_size(_tmp6FC,39));}),_tag_dynforward(_tmp6FB,
sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp6FD[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp6FE="fast_add_free_tvar: bad identity in tvs2";_tag_dynforward(_tmp6FE,
sizeof(char),_get_zero_arr_size(_tmp6FE,41));}),_tag_dynforward(_tmp6FD,sizeof(
void*),0));});if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))
== *((int*)_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF->hd=tv;_tmp6FF->tl=tvs;_tmp6FF;});}
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp702;
_tmp702.tag=0;_tmp702.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string(
tv));{void*_tmp700[1]={& _tmp702};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp701="bound tvar id for %s is NULL";
_tag_dynforward(_tmp701,sizeof(char),_get_zero_arr_size(_tmp701,29));}),
_tag_dynforward(_tmp700,sizeof(void*),1));}});return({struct Cyc_List_List*
_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703->hd=tv;_tmp703->tl=tvs;_tmp703;});}
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){
void*_tmp704=Cyc_Tcutil_compress(e);int _tmp705;_LL4A2: if(_tmp704 <= (void*)4)goto
_LL4A4;if(*((int*)_tmp704)!= 0)goto _LL4A4;_tmp705=((struct Cyc_Absyn_Evar_struct*)
_tmp704)->f3;_LL4A3:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*
_tmp706=Cyc_Tcutil_compress((void*)es2->hd);int _tmp707;_LL4A7: if(_tmp706 <= (void*)
4)goto _LL4A9;if(*((int*)_tmp706)!= 0)goto _LL4A9;_tmp707=((struct Cyc_Absyn_Evar_struct*)
_tmp706)->f3;_LL4A8: if(_tmp705 == _tmp707)return es;goto _LL4A6;_LL4A9:;_LL4AA: goto
_LL4A6;_LL4A6:;}}return({struct Cyc_List_List*_tmp708=_cycalloc(sizeof(*_tmp708));
_tmp708->hd=(void*)e;_tmp708->tl=es;_tmp708;});_LL4A4:;_LL4A5: return es;_LL4A1:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!present)r=({
struct Cyc_List_List*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709->hd=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp709->tl=r;_tmp709;});}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dynforward_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp70C;_tmp70C.tag=0;
_tmp70C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp70A[1]={&
_tmp70C};Cyc_Tcutil_terr(loc,({const char*_tmp70B="bitfield %s does not have constant width";
_tag_dynforward(_tmp70B,sizeof(char),_get_zero_arr_size(_tmp70B,41));}),
_tag_dynforward(_tmp70A,sizeof(void*),1));}});else{unsigned int _tmp70E;int
_tmp70F;struct _tuple7 _tmp70D=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
width);_tmp70E=_tmp70D.f1;_tmp70F=_tmp70D.f2;if(!_tmp70F)({void*_tmp710[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp711="bitfield width cannot use sizeof or offsetof";
_tag_dynforward(_tmp711,sizeof(char),_get_zero_arr_size(_tmp711,45));}),
_tag_dynforward(_tmp710,sizeof(void*),0));});w=_tmp70E;}{void*_tmp712=Cyc_Tcutil_compress(
field_typ);void*_tmp713;_LL4AC: if(_tmp712 <= (void*)4)goto _LL4AE;if(*((int*)
_tmp712)!= 5)goto _LL4AE;_tmp713=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp712)->f2;
_LL4AD:{void*_tmp714=_tmp713;_LL4B1: if((int)_tmp714 != 0)goto _LL4B3;_LL4B2: if(w > 
8)({void*_tmp715[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp716="bitfield larger than type";
_tag_dynforward(_tmp716,sizeof(char),_get_zero_arr_size(_tmp716,26));}),
_tag_dynforward(_tmp715,sizeof(void*),0));});goto _LL4B0;_LL4B3: if((int)_tmp714 != 
1)goto _LL4B5;_LL4B4: if(w > 16)({void*_tmp717[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp718="bitfield larger than type";_tag_dynforward(_tmp718,sizeof(char),
_get_zero_arr_size(_tmp718,26));}),_tag_dynforward(_tmp717,sizeof(void*),0));});
goto _LL4B0;_LL4B5: if((int)_tmp714 != 3)goto _LL4B7;_LL4B6: goto _LL4B8;_LL4B7: if((
int)_tmp714 != 2)goto _LL4B9;_LL4B8: if(w > 32)({void*_tmp719[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp71A="bitfield larger than type";_tag_dynforward(_tmp71A,
sizeof(char),_get_zero_arr_size(_tmp71A,26));}),_tag_dynforward(_tmp719,sizeof(
void*),0));});goto _LL4B0;_LL4B9: if((int)_tmp714 != 4)goto _LL4B0;_LL4BA: if(w > 64)({
void*_tmp71B[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp71C="bitfield larger than type";
_tag_dynforward(_tmp71C,sizeof(char),_get_zero_arr_size(_tmp71C,26));}),
_tag_dynforward(_tmp71B,sizeof(void*),0));});goto _LL4B0;_LL4B0:;}goto _LL4AB;
_LL4AE:;_LL4AF:({struct Cyc_String_pa_struct _tmp720;_tmp720.tag=0;_tmp720.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(field_typ));{
struct Cyc_String_pa_struct _tmp71F;_tmp71F.tag=0;_tmp71F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{void*_tmp71D[2]={& _tmp71F,& _tmp720};
Cyc_Tcutil_terr(loc,({const char*_tmp71E="bitfield %s must have integral type but has type %s";
_tag_dynforward(_tmp71E,sizeof(char),_get_zero_arr_size(_tmp71E,52));}),
_tag_dynforward(_tmp71D,sizeof(void*),2));}}});goto _LL4AB;_LL4AB:;}}}static void
Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp721=(void*)
atts->hd;_LL4BC: if((int)_tmp721 != 5)goto _LL4BE;_LL4BD: continue;_LL4BE: if(_tmp721
<= (void*)17)goto _LL4C0;if(*((int*)_tmp721)!= 1)goto _LL4C0;_LL4BF: continue;
_LL4C0:;_LL4C1:({struct Cyc_String_pa_struct _tmp725;_tmp725.tag=0;_tmp725.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_String_pa_struct
_tmp724;_tmp724.tag=0;_tmp724.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp722[2]={& _tmp724,& _tmp725};
Cyc_Tcutil_terr(loc,({const char*_tmp723="bad attribute %s on member %s";
_tag_dynforward(_tmp723,sizeof(char),_get_zero_arr_size(_tmp723,30));}),
_tag_dynforward(_tmp722,sizeof(void*),2));}}});_LL4BB:;}}struct Cyc_Tcutil_CVTEnv{
struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp726=t;struct
Cyc_Absyn_Typedefdecl*_tmp727;void**_tmp728;_LL4C3: if(_tmp726 <= (void*)4)goto
_LL4C5;if(*((int*)_tmp726)!= 16)goto _LL4C5;_tmp727=((struct Cyc_Absyn_TypedefType_struct*)
_tmp726)->f3;_tmp728=((struct Cyc_Absyn_TypedefType_struct*)_tmp726)->f4;_LL4C4:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp727))->tq).real_const  || (
_tmp727->tq).print_const){if(declared_const)({void*_tmp729[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp72A="extra const";_tag_dynforward(_tmp72A,sizeof(char),
_get_zero_arr_size(_tmp72A,12));}),_tag_dynforward(_tmp729,sizeof(void*),0));});
return 1;}if((unsigned int)_tmp728)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp728);else{return declared_const;}_LL4C5:;_LL4C6: return
declared_const;_LL4C2:;}struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t,int allow_unique){{
void*_tmp72B=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp72C;struct Cyc_Core_Opt**
_tmp72D;struct Cyc_Core_Opt*_tmp72E;struct Cyc_Core_Opt**_tmp72F;struct Cyc_Absyn_Tvar*
_tmp730;struct Cyc_List_List*_tmp731;struct _tuple1*_tmp732;struct Cyc_Absyn_Enumdecl*
_tmp733;struct Cyc_Absyn_Enumdecl**_tmp734;struct Cyc_Absyn_TunionInfo _tmp735;void*
_tmp736;void**_tmp737;struct Cyc_List_List*_tmp738;struct Cyc_List_List**_tmp739;
void*_tmp73A;struct Cyc_Absyn_TunionFieldInfo _tmp73B;void*_tmp73C;void**_tmp73D;
struct Cyc_List_List*_tmp73E;struct Cyc_Absyn_PtrInfo _tmp73F;void*_tmp740;struct
Cyc_Absyn_Tqual _tmp741;struct Cyc_Absyn_Tqual*_tmp742;struct Cyc_Absyn_PtrAtts
_tmp743;void*_tmp744;struct Cyc_Absyn_Conref*_tmp745;struct Cyc_Absyn_Conref*
_tmp746;struct Cyc_Absyn_Conref*_tmp747;void*_tmp748;void*_tmp749;struct Cyc_Absyn_ArrayInfo
_tmp74A;void*_tmp74B;struct Cyc_Absyn_Tqual _tmp74C;struct Cyc_Absyn_Tqual*_tmp74D;
struct Cyc_Absyn_Exp*_tmp74E;struct Cyc_Absyn_Exp**_tmp74F;struct Cyc_Absyn_Conref*
_tmp750;struct Cyc_Position_Segment*_tmp751;struct Cyc_Absyn_FnInfo _tmp752;struct
Cyc_List_List*_tmp753;struct Cyc_List_List**_tmp754;struct Cyc_Core_Opt*_tmp755;
struct Cyc_Core_Opt**_tmp756;void*_tmp757;struct Cyc_List_List*_tmp758;int _tmp759;
struct Cyc_Absyn_VarargInfo*_tmp75A;struct Cyc_List_List*_tmp75B;struct Cyc_List_List*
_tmp75C;struct Cyc_List_List*_tmp75D;void*_tmp75E;struct Cyc_List_List*_tmp75F;
struct Cyc_Absyn_AggrInfo _tmp760;void*_tmp761;void**_tmp762;struct Cyc_List_List*
_tmp763;struct Cyc_List_List**_tmp764;struct _tuple1*_tmp765;struct Cyc_List_List*
_tmp766;struct Cyc_List_List**_tmp767;struct Cyc_Absyn_Typedefdecl*_tmp768;struct
Cyc_Absyn_Typedefdecl**_tmp769;void**_tmp76A;void***_tmp76B;void*_tmp76C;void*
_tmp76D;void*_tmp76E;struct Cyc_List_List*_tmp76F;_LL4C8: if((int)_tmp72B != 0)goto
_LL4CA;_LL4C9: goto _LL4C7;_LL4CA: if(_tmp72B <= (void*)4)goto _LL4DE;if(*((int*)
_tmp72B)!= 0)goto _LL4CC;_tmp72C=((struct Cyc_Absyn_Evar_struct*)_tmp72B)->f1;
_tmp72D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp72B)->f1;
_tmp72E=((struct Cyc_Absyn_Evar_struct*)_tmp72B)->f2;_tmp72F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp72B)->f2;_LL4CB: if(*_tmp72D == 0)*_tmp72D=({
struct Cyc_Core_Opt*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->v=(void*)
expected_kind;_tmp770;});if((cvtenv.fn_result  && cvtenv.generalize_evars) && 
expected_kind == (void*)3)*_tmp72F=({struct Cyc_Core_Opt*_tmp771=_cycalloc(sizeof(*
_tmp771));_tmp771->v=(void*)((void*)2);_tmp771;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp772=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp777;_tmp777.tag=2;_tmp777.f1=0;_tmp777.f2=(void*)expected_kind;_tmp777;});
_tmp776;}));*_tmp72F=({struct Cyc_Core_Opt*_tmp773=_cycalloc(sizeof(*_tmp773));
_tmp773->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp774=_cycalloc(
sizeof(*_tmp774));_tmp774[0]=({struct Cyc_Absyn_VarType_struct _tmp775;_tmp775.tag=
1;_tmp775.f1=_tmp772;_tmp775;});_tmp774;}));_tmp773;});_tmp730=_tmp772;goto
_LL4CD;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL4C7;_LL4CC: if(*((int*)_tmp72B)!= 1)goto _LL4CE;_tmp730=((struct Cyc_Absyn_VarType_struct*)
_tmp72B)->f1;_LL4CD:{void*_tmp778=Cyc_Absyn_compress_kb((void*)_tmp730->kind);
struct Cyc_Core_Opt*_tmp779;struct Cyc_Core_Opt**_tmp77A;_LL4FD: if(*((int*)_tmp778)
!= 1)goto _LL4FF;_tmp779=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp778)->f1;
_tmp77A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp778)->f1;
_LL4FE:*_tmp77A=({struct Cyc_Core_Opt*_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp77C=_cycalloc(sizeof(*_tmp77C));
_tmp77C[0]=({struct Cyc_Absyn_Less_kb_struct _tmp77D;_tmp77D.tag=2;_tmp77D.f1=0;
_tmp77D.f2=(void*)expected_kind;_tmp77D;});_tmp77C;}));_tmp77B;});goto _LL4FC;
_LL4FF:;_LL500: goto _LL4FC;_LL4FC:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmp730);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,
_tmp730);goto _LL4C7;_LL4CE: if(*((int*)_tmp72B)!= 13)goto _LL4D0;_tmp731=((struct
Cyc_Absyn_AnonEnumType_struct*)_tmp72B)->f1;_LL4CF: {struct Cyc_Tcenv_Genv*ge=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);{struct _RegionHandle _tmp77E=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp77E;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp731 != 0;_tmp731=_tmp731->tl){
struct Cyc_Absyn_Enumfield*_tmp77F=(struct Cyc_Absyn_Enumfield*)_tmp731->hd;if(((
int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*
l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp77F->name).f2))({
struct Cyc_String_pa_struct _tmp782;_tmp782.tag=0;_tmp782.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*(*_tmp77F->name).f2);{void*_tmp780[1]={&
_tmp782};Cyc_Tcutil_terr(_tmp77F->loc,({const char*_tmp781="duplicate enum field name %s";
_tag_dynforward(_tmp781,sizeof(char),_get_zero_arr_size(_tmp781,29));}),
_tag_dynforward(_tmp780,sizeof(void*),1));}});else{prev_fields=({struct Cyc_List_List*
_tmp783=_region_malloc(uprev_rgn,sizeof(*_tmp783));_tmp783->hd=(*_tmp77F->name).f2;
_tmp783->tl=prev_fields;_tmp783;});}if(_tmp77F->tag == 0)_tmp77F->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp77F->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp77F->tag)))({struct Cyc_String_pa_struct
_tmp786;_tmp786.tag=0;_tmp786.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
_tmp77F->name).f2);{void*_tmp784[1]={& _tmp786};Cyc_Tcutil_terr(loc,({const char*
_tmp785="enum field %s: expression is not constant";_tag_dynforward(_tmp785,
sizeof(char),_get_zero_arr_size(_tmp785,42));}),_tag_dynforward(_tmp784,sizeof(
void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp77F->tag))).f1;tag_count=t1 + 1;(*_tmp77F->name).f1=(void*)({
struct Cyc_Absyn_Abs_n_struct*_tmp787=_cycalloc(sizeof(*_tmp787));_tmp787[0]=({
struct Cyc_Absyn_Abs_n_struct _tmp788;_tmp788.tag=1;_tmp788.f1=te->ns;_tmp788;});
_tmp787;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp77F->name).f2,({
struct _tuple17*_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp78A=_cycalloc(sizeof(*_tmp78A));_tmp78A[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp78B;_tmp78B.tag=4;_tmp78B.f1=(void*)t;
_tmp78B.f2=_tmp77F;_tmp78B;});_tmp78A;});_tmp789->f2=1;_tmp789;}));}}};
_pop_region(uprev_rgn);}goto _LL4C7;}_LL4D0: if(*((int*)_tmp72B)!= 12)goto _LL4D2;
_tmp732=((struct Cyc_Absyn_EnumType_struct*)_tmp72B)->f1;_tmp733=((struct Cyc_Absyn_EnumType_struct*)
_tmp72B)->f2;_tmp734=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp72B)->f2;_LL4D1: if(*_tmp734 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp734))->fields == 0){struct _handler_cons _tmp78C;_push_handler(& _tmp78C);{int
_tmp78E=0;if(setjmp(_tmp78C.handler))_tmp78E=1;if(!_tmp78E){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp732);*_tmp734=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp78D=(void*)_exn_thrown;void*_tmp790=_tmp78D;
_LL502: if(_tmp790 != Cyc_Dict_Absent)goto _LL504;_LL503: {struct Cyc_Tcenv_Genv*
_tmp791=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp792=({struct Cyc_Absyn_Enumdecl*
_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793->sc=(void*)((void*)3);_tmp793->name=
_tmp732;_tmp793->fields=0;_tmp793;});Cyc_Tc_tcEnumdecl(te,_tmp791,loc,_tmp792);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp732);*_tmp734=(
struct Cyc_Absyn_Enumdecl*)*ed;goto _LL501;}}_LL504:;_LL505:(void)_throw(_tmp790);
_LL501:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp734);*_tmp732=(ed->name)[_check_known_subscript_notnull(1,0)];goto _LL4C7;}
_LL4D2: if(*((int*)_tmp72B)!= 2)goto _LL4D4;_tmp735=((struct Cyc_Absyn_TunionType_struct*)
_tmp72B)->f1;_tmp736=(void*)_tmp735.tunion_info;_tmp737=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp72B)->f1).tunion_info;_tmp738=_tmp735.targs;_tmp739=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp72B)->f1).targs;_tmp73A=(void*)_tmp735.rgn;
_LL4D3: {struct Cyc_List_List*_tmp794=*_tmp739;{void*_tmp795=*_tmp737;struct Cyc_Absyn_UnknownTunionInfo
_tmp796;struct _tuple1*_tmp797;int _tmp798;struct Cyc_Absyn_Tuniondecl**_tmp799;
struct Cyc_Absyn_Tuniondecl*_tmp79A;_LL507: if(*((int*)_tmp795)!= 0)goto _LL509;
_tmp796=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp795)->f1;_tmp797=_tmp796.name;
_tmp798=_tmp796.is_xtunion;_LL508: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp79B;_push_handler(& _tmp79B);{int _tmp79D=0;if(setjmp(_tmp79B.handler))
_tmp79D=1;if(!_tmp79D){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp797);;
_pop_handler();}else{void*_tmp79C=(void*)_exn_thrown;void*_tmp79F=_tmp79C;_LL50C:
if(_tmp79F != Cyc_Dict_Absent)goto _LL50E;_LL50D: {struct Cyc_Tcenv_Genv*_tmp7A0=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp7A1=({struct Cyc_Absyn_Tuniondecl*
_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8->sc=(void*)((void*)3);_tmp7A8->name=
_tmp797;_tmp7A8->tvs=0;_tmp7A8->fields=0;_tmp7A8->is_xtunion=_tmp798;_tmp7A8;});
Cyc_Tc_tcTuniondecl(te,_tmp7A0,loc,_tmp7A1);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp797);if(_tmp794 != 0){({struct Cyc_String_pa_struct _tmp7A5;_tmp7A5.tag=0;
_tmp7A5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp797));{struct Cyc_String_pa_struct _tmp7A4;_tmp7A4.tag=0;_tmp7A4.f1=(struct
_dynforward_ptr)(_tmp798?(struct _dynforward_ptr)((struct _dynforward_ptr)({const
char*_tmp7A6="xtunion";_tag_dynforward(_tmp7A6,sizeof(char),_get_zero_arr_size(
_tmp7A6,8));})):(struct _dynforward_ptr)({const char*_tmp7A7="tunion";
_tag_dynforward(_tmp7A7,sizeof(char),_get_zero_arr_size(_tmp7A7,7));}));{void*
_tmp7A2[2]={& _tmp7A4,& _tmp7A5};Cyc_Tcutil_terr(loc,({const char*_tmp7A3="declare parameterized %s %s before using";
_tag_dynforward(_tmp7A3,sizeof(char),_get_zero_arr_size(_tmp7A3,41));}),
_tag_dynforward(_tmp7A2,sizeof(void*),2));}}});return cvtenv;}goto _LL50B;}_LL50E:;
_LL50F:(void)_throw(_tmp79F);_LL50B:;}}}if((*tudp)->is_xtunion != _tmp798)({
struct Cyc_String_pa_struct _tmp7AB;_tmp7AB.tag=0;_tmp7AB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp797));{void*
_tmp7A9[1]={& _tmp7AB};Cyc_Tcutil_terr(loc,({const char*_tmp7AA="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp7AA,sizeof(char),_get_zero_arr_size(_tmp7AA,48));}),
_tag_dynforward(_tmp7A9,sizeof(void*),1));}});*_tmp737=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp7AD;_tmp7AD.tag=1;_tmp7AD.f1=tudp;_tmp7AD;});_tmp7AC;});_tmp79A=*tudp;goto
_LL50A;}_LL509: if(*((int*)_tmp795)!= 1)goto _LL506;_tmp799=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp795)->f1;_tmp79A=*_tmp799;_LL50A: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp73A,allow_unique);{struct Cyc_List_List*tvs=_tmp79A->tvs;for(
0;_tmp794 != 0  && tvs != 0;(_tmp794=_tmp794->tl,tvs=tvs->tl)){void*t1=(void*)
_tmp794->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,0);}if(_tmp794 != 0)({struct Cyc_String_pa_struct
_tmp7B0;_tmp7B0.tag=0;_tmp7B0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp79A->name));{void*_tmp7AE[1]={& _tmp7B0};Cyc_Tcutil_terr(
loc,({const char*_tmp7AF="too many type arguments for tunion %s";_tag_dynforward(
_tmp7AF,sizeof(char),_get_zero_arr_size(_tmp7AF,38));}),_tag_dynforward(_tmp7AE,
sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;
tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=
Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp7B1=_cycalloc(
sizeof(*_tmp7B1));_tmp7B1->hd=(void*)e;_tmp7B1->tl=hidden_ts;_tmp7B1;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,0);}*_tmp739=Cyc_List_imp_append(*
_tmp739,Cyc_List_imp_rev(hidden_ts));}goto _LL506;}_LL506:;}goto _LL4C7;}_LL4D4:
if(*((int*)_tmp72B)!= 3)goto _LL4D6;_tmp73B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp72B)->f1;_tmp73C=(void*)_tmp73B.field_info;_tmp73D=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp72B)->f1).field_info;_tmp73E=_tmp73B.targs;_LL4D5:{void*_tmp7B2=*_tmp73D;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp7B3;struct _tuple1*_tmp7B4;struct
_tuple1*_tmp7B5;int _tmp7B6;struct Cyc_Absyn_Tuniondecl*_tmp7B7;struct Cyc_Absyn_Tunionfield*
_tmp7B8;_LL511: if(*((int*)_tmp7B2)!= 0)goto _LL513;_tmp7B3=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp7B2)->f1;_tmp7B4=_tmp7B3.tunion_name;_tmp7B5=_tmp7B3.field_name;_tmp7B6=
_tmp7B3.is_xtunion;_LL512: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp7B9;_push_handler(& _tmp7B9);{int _tmp7BB=0;if(setjmp(
_tmp7B9.handler))_tmp7BB=1;if(!_tmp7BB){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp7B4);;_pop_handler();}else{void*_tmp7BA=(void*)_exn_thrown;void*_tmp7BD=
_tmp7BA;_LL516: if(_tmp7BD != Cyc_Dict_Absent)goto _LL518;_LL517:({struct Cyc_String_pa_struct
_tmp7C0;_tmp7C0.tag=0;_tmp7C0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7B4));{void*_tmp7BE[1]={& _tmp7C0};Cyc_Tcutil_terr(
loc,({const char*_tmp7BF="unbound type tunion %s";_tag_dynforward(_tmp7BF,sizeof(
char),_get_zero_arr_size(_tmp7BF,23));}),_tag_dynforward(_tmp7BE,sizeof(void*),1));}});
return cvtenv;_LL518:;_LL519:(void)_throw(_tmp7BD);_LL515:;}}}{struct
_handler_cons _tmp7C1;_push_handler(& _tmp7C1);{int _tmp7C3=0;if(setjmp(_tmp7C1.handler))
_tmp7C3=1;if(!_tmp7C3){{void*_tmp7C4=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp7B5);
struct Cyc_Absyn_Tuniondecl*_tmp7C5;struct Cyc_Absyn_Tunionfield*_tmp7C6;_LL51B:
if(*((int*)_tmp7C4)!= 2)goto _LL51D;_tmp7C5=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp7C4)->f1;_tmp7C6=((struct Cyc_Tcenv_TunionRes_struct*)_tmp7C4)->f2;_LL51C: tuf=
_tmp7C6;tud=_tmp7C5;if(tud->is_xtunion != _tmp7B6)({struct Cyc_String_pa_struct
_tmp7C9;_tmp7C9.tag=0;_tmp7C9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7B4));{void*_tmp7C7[1]={& _tmp7C9};Cyc_Tcutil_terr(
loc,({const char*_tmp7C8="[x]tunion is different from type declaration %s";
_tag_dynforward(_tmp7C8,sizeof(char),_get_zero_arr_size(_tmp7C8,48));}),
_tag_dynforward(_tmp7C7,sizeof(void*),1));}});goto _LL51A;_LL51D:;_LL51E:({struct
Cyc_String_pa_struct _tmp7CD;_tmp7CD.tag=0;_tmp7CD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7B4));{struct Cyc_String_pa_struct
_tmp7CC;_tmp7CC.tag=0;_tmp7CC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7B5));{void*_tmp7CA[2]={& _tmp7CC,& _tmp7CD};Cyc_Tcutil_terr(
loc,({const char*_tmp7CB="unbound field %s in type tunion %s";_tag_dynforward(
_tmp7CB,sizeof(char),_get_zero_arr_size(_tmp7CB,35));}),_tag_dynforward(_tmp7CA,
sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp7CE=cvtenv;_npop_handler(0);
return _tmp7CE;}_LL51A:;};_pop_handler();}else{void*_tmp7C2=(void*)_exn_thrown;
void*_tmp7D0=_tmp7C2;_LL520: if(_tmp7D0 != Cyc_Dict_Absent)goto _LL522;_LL521:({
struct Cyc_String_pa_struct _tmp7D4;_tmp7D4.tag=0;_tmp7D4.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7B4));{
struct Cyc_String_pa_struct _tmp7D3;_tmp7D3.tag=0;_tmp7D3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7B5));{void*
_tmp7D1[2]={& _tmp7D3,& _tmp7D4};Cyc_Tcutil_terr(loc,({const char*_tmp7D2="unbound field %s in type tunion %s";
_tag_dynforward(_tmp7D2,sizeof(char),_get_zero_arr_size(_tmp7D2,35));}),
_tag_dynforward(_tmp7D1,sizeof(void*),2));}}});return cvtenv;_LL522:;_LL523:(void)
_throw(_tmp7D0);_LL51F:;}}}*_tmp73D=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp7D6;_tmp7D6.tag=1;_tmp7D6.f1=tud;_tmp7D6.f2=tuf;_tmp7D6;});_tmp7D5;});
_tmp7B7=tud;_tmp7B8=tuf;goto _LL514;}_LL513: if(*((int*)_tmp7B2)!= 1)goto _LL510;
_tmp7B7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7B2)->f1;_tmp7B8=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp7B2)->f2;_LL514: {struct Cyc_List_List*tvs=
_tmp7B7->tvs;for(0;_tmp73E != 0  && tvs != 0;(_tmp73E=_tmp73E->tl,tvs=tvs->tl)){
void*t1=(void*)_tmp73E->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,0);}if(_tmp73E
!= 0)({struct Cyc_String_pa_struct _tmp7DA;_tmp7DA.tag=0;_tmp7DA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7B8->name));{
struct Cyc_String_pa_struct _tmp7D9;_tmp7D9.tag=0;_tmp7D9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7B7->name));{
void*_tmp7D7[2]={& _tmp7D9,& _tmp7DA};Cyc_Tcutil_terr(loc,({const char*_tmp7D8="too many type arguments for tunion %s.%s";
_tag_dynforward(_tmp7D8,sizeof(char),_get_zero_arr_size(_tmp7D8,41));}),
_tag_dynforward(_tmp7D7,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct
_tmp7DE;_tmp7DE.tag=0;_tmp7DE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp7B8->name));{struct Cyc_String_pa_struct _tmp7DD;
_tmp7DD.tag=0;_tmp7DD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7B7->name));{void*_tmp7DB[2]={& _tmp7DD,& _tmp7DE};Cyc_Tcutil_terr(loc,({const
char*_tmp7DC="too few type arguments for tunion %s.%s";_tag_dynforward(_tmp7DC,
sizeof(char),_get_zero_arr_size(_tmp7DC,40));}),_tag_dynforward(_tmp7DB,sizeof(
void*),2));}}});goto _LL510;}_LL510:;}goto _LL4C7;_LL4D6: if(*((int*)_tmp72B)!= 4)
goto _LL4D8;_tmp73F=((struct Cyc_Absyn_PointerType_struct*)_tmp72B)->f1;_tmp740=(
void*)_tmp73F.elt_typ;_tmp741=_tmp73F.elt_tq;_tmp742=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp72B)->f1).elt_tq;_tmp743=_tmp73F.ptr_atts;
_tmp744=(void*)_tmp743.rgn;_tmp745=_tmp743.nullable;_tmp746=_tmp743.bounds;
_tmp747=_tmp743.zero_term;_LL4D7: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp740,0);_tmp742->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp742->print_const,_tmp740);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp744,allow_unique);{void*_tmp7DF=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp747))->v;int _tmp7E0;
_LL525: if((int)_tmp7DF != 0)goto _LL527;_LL526:{void*_tmp7E1=Cyc_Tcutil_compress(
_tmp740);void*_tmp7E2;_LL52C: if(_tmp7E1 <= (void*)4)goto _LL52E;if(*((int*)_tmp7E1)
!= 5)goto _LL52E;_tmp7E2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7E1)->f2;
if((int)_tmp7E2 != 0)goto _LL52E;_LL52D:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp747,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL52B;_LL52E:;_LL52F:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp747,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL52B;
_LL52B:;}goto _LL524;_LL527: if(_tmp7DF <= (void*)1)goto _LL529;if(*((int*)_tmp7DF)
!= 0)goto _LL529;_tmp7E0=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7DF)->f1;
if(_tmp7E0 != 1)goto _LL529;_LL528: if(!Cyc_Tcutil_admits_zero(_tmp740))({struct Cyc_String_pa_struct
_tmp7E5;_tmp7E5.tag=0;_tmp7E5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp740));{void*_tmp7E3[1]={& _tmp7E5};Cyc_Tcutil_terr(loc,({
const char*_tmp7E4="cannot have a pointer to zero-terminated %s elements";
_tag_dynforward(_tmp7E4,sizeof(char),_get_zero_arr_size(_tmp7E4,53));}),
_tag_dynforward(_tmp7E3,sizeof(void*),1));}});is_zero_terminated=1;goto _LL524;
_LL529:;_LL52A: is_zero_terminated=0;goto _LL524;_LL524:;}{void*_tmp7E6=(void*)(
Cyc_Absyn_compress_conref(_tmp746))->v;void*_tmp7E7;void*_tmp7E8;void*_tmp7E9;
struct Cyc_Absyn_Exp*_tmp7EA;void*_tmp7EB;void*_tmp7EC;_LL531: if((int)_tmp7E6 != 0)
goto _LL533;_LL532: goto _LL534;_LL533: if(_tmp7E6 <= (void*)1)goto _LL535;if(*((int*)
_tmp7E6)!= 0)goto _LL535;_tmp7E7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7E6)->f1;if((int)_tmp7E7 != 0)goto _LL535;_LL534: goto _LL530;_LL535: if(_tmp7E6
<= (void*)1)goto _LL537;if(*((int*)_tmp7E6)!= 0)goto _LL537;_tmp7E8=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp7E6)->f1;if((int)_tmp7E8 != 1)goto _LL537;
_LL536: goto _LL530;_LL537: if(_tmp7E6 <= (void*)1)goto _LL539;if(*((int*)_tmp7E6)!= 
0)goto _LL539;_tmp7E9=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7E6)->f1;if(
_tmp7E9 <= (void*)2)goto _LL539;if(*((int*)_tmp7E9)!= 0)goto _LL539;_tmp7EA=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7E9)->f1;_LL538: Cyc_Tcexp_tcExp(te,0,_tmp7EA);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7EA))({void*_tmp7ED[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7EE="pointer bounds expression is not an unsigned int";
_tag_dynforward(_tmp7EE,sizeof(char),_get_zero_arr_size(_tmp7EE,49));}),
_tag_dynforward(_tmp7ED,sizeof(void*),0));});{unsigned int _tmp7F0;int _tmp7F1;
struct _tuple7 _tmp7EF=Cyc_Evexp_eval_const_uint_exp(_tmp7EA);_tmp7F0=_tmp7EF.f1;
_tmp7F1=_tmp7EF.f2;if(is_zero_terminated  && (!_tmp7F1  || _tmp7F0 < 1))({void*
_tmp7F2[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7F3="zero-terminated pointer cannot point to empty sequence";
_tag_dynforward(_tmp7F3,sizeof(char),_get_zero_arr_size(_tmp7F3,55));}),
_tag_dynforward(_tmp7F2,sizeof(void*),0));});goto _LL530;}_LL539: if(_tmp7E6 <= (
void*)1)goto _LL53B;if(*((int*)_tmp7E6)!= 0)goto _LL53B;_tmp7EB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7E6)->f1;if(_tmp7EB <= (void*)2)goto _LL53B;if(*((int*)_tmp7EB)!= 1)goto _LL53B;
_tmp7EC=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp7EB)->f1;_LL53A: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp7EC,0);goto _LL530;
_LL53B: if(_tmp7E6 <= (void*)1)goto _LL530;if(*((int*)_tmp7E6)!= 1)goto _LL530;
_LL53C:({void*_tmp7F4[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7F5="forward constraint";
_tag_dynforward(_tmp7F5,sizeof(char),_get_zero_arr_size(_tmp7F5,19));}),
_tag_dynforward(_tmp7F4,sizeof(void*),0));});_LL530:;}goto _LL4C7;}_LL4D8: if(*((
int*)_tmp72B)!= 17)goto _LL4DA;_tmp748=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp72B)->f1;_LL4D9: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp748,allow_unique);goto _LL4C7;_LL4DA: if(*((int*)_tmp72B)!= 14)goto _LL4DC;
_tmp749=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp72B)->f1;_LL4DB: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp749,allow_unique);goto
_LL4C7;_LL4DC: if(*((int*)_tmp72B)!= 5)goto _LL4DE;_LL4DD: goto _LL4DF;_LL4DE: if((
int)_tmp72B != 1)goto _LL4E0;_LL4DF: goto _LL4E1;_LL4E0: if(_tmp72B <= (void*)4)goto
_LL4EE;if(*((int*)_tmp72B)!= 6)goto _LL4E2;_LL4E1: goto _LL4C7;_LL4E2: if(*((int*)
_tmp72B)!= 7)goto _LL4E4;_tmp74A=((struct Cyc_Absyn_ArrayType_struct*)_tmp72B)->f1;
_tmp74B=(void*)_tmp74A.elt_type;_tmp74C=_tmp74A.tq;_tmp74D=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp72B)->f1).tq;_tmp74E=_tmp74A.num_elts;
_tmp74F=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp72B)->f1).num_elts;
_tmp750=_tmp74A.zero_term;_tmp751=_tmp74A.zt_loc;_LL4E3: {struct Cyc_Absyn_Exp*
_tmp7F6=*_tmp74F;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp74B,0);_tmp74D->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp74D->print_const,
_tmp74B);{int is_zero_terminated;{void*_tmp7F7=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp750))->v;int _tmp7F8;
_LL53E: if((int)_tmp7F7 != 0)goto _LL540;_LL53F:((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp750,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL53D;_LL540: if(
_tmp7F7 <= (void*)1)goto _LL542;if(*((int*)_tmp7F7)!= 0)goto _LL542;_tmp7F8=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp7F7)->f1;if(_tmp7F8 != 1)goto _LL542;_LL541:
if(!Cyc_Tcutil_admits_zero(_tmp74B))({struct Cyc_String_pa_struct _tmp7FB;_tmp7FB.tag=
0;_tmp7FB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp74B));{void*_tmp7F9[1]={& _tmp7FB};Cyc_Tcutil_terr(loc,({const char*_tmp7FA="cannot have a zero-terminated array of %s elements";
_tag_dynforward(_tmp7FA,sizeof(char),_get_zero_arr_size(_tmp7FA,51));}),
_tag_dynforward(_tmp7F9,sizeof(void*),1));}});is_zero_terminated=1;goto _LL53D;
_LL542:;_LL543: is_zero_terminated=0;goto _LL53D;_LL53D:;}if(_tmp7F6 == 0){if(
is_zero_terminated)*_tmp74F=(_tmp7F6=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,
0));else{({void*_tmp7FC[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7FD="array bound defaults to 0 here";
_tag_dynforward(_tmp7FD,sizeof(char),_get_zero_arr_size(_tmp7FD,31));}),
_tag_dynforward(_tmp7FC,sizeof(void*),0));});*_tmp74F=(_tmp7F6=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp7F6);
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp7F6))({void*_tmp7FE[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp7FF="array bounds expression is not constant";
_tag_dynforward(_tmp7FF,sizeof(char),_get_zero_arr_size(_tmp7FF,40));}),
_tag_dynforward(_tmp7FE,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_tmp7F6))({void*_tmp800[0]={};Cyc_Tcutil_terr(loc,({const
char*_tmp801="array bounds expression is not an unsigned int";_tag_dynforward(
_tmp801,sizeof(char),_get_zero_arr_size(_tmp801,47));}),_tag_dynforward(_tmp800,
sizeof(void*),0));});{unsigned int _tmp803;int _tmp804;struct _tuple7 _tmp802=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp7F6);_tmp803=_tmp802.f1;_tmp804=_tmp802.f2;if((
is_zero_terminated  && _tmp804) && _tmp803 < 1)({void*_tmp805[0]={};Cyc_Tcutil_warn(
loc,({const char*_tmp806="zero terminated array cannot have zero elements";
_tag_dynforward(_tmp806,sizeof(char),_get_zero_arr_size(_tmp806,48));}),
_tag_dynforward(_tmp805,sizeof(void*),0));});goto _LL4C7;}}}_LL4E4: if(*((int*)
_tmp72B)!= 8)goto _LL4E6;_tmp752=((struct Cyc_Absyn_FnType_struct*)_tmp72B)->f1;
_tmp753=_tmp752.tvars;_tmp754=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp72B)->f1).tvars;_tmp755=_tmp752.effect;_tmp756=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp72B)->f1).effect;_tmp757=(void*)_tmp752.ret_typ;
_tmp758=_tmp752.args;_tmp759=_tmp752.c_varargs;_tmp75A=_tmp752.cyc_varargs;
_tmp75B=_tmp752.rgn_po;_tmp75C=_tmp752.attributes;_LL4E5: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp75C != 0;_tmp75C=_tmp75C->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp75C->hd))({struct Cyc_String_pa_struct _tmp809;
_tmp809.tag=0;_tmp809.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)_tmp75C->hd));{void*_tmp807[1]={& _tmp809};Cyc_Tcutil_terr(loc,({const char*
_tmp808="bad function type attribute %s";_tag_dynforward(_tmp808,sizeof(char),
_get_zero_arr_size(_tmp808,31));}),_tag_dynforward(_tmp807,sizeof(void*),1));}});{
void*_tmp80A=(void*)_tmp75C->hd;void*_tmp80B;int _tmp80C;int _tmp80D;_LL545: if((
int)_tmp80A != 0)goto _LL547;_LL546: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL544;_LL547: if((int)_tmp80A != 1)goto _LL549;_LL548: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL544;_LL549: if((int)_tmp80A != 2)goto _LL54B;
_LL54A: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL544;_LL54B: if(
_tmp80A <= (void*)17)goto _LL54D;if(*((int*)_tmp80A)!= 3)goto _LL54D;_tmp80B=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp80A)->f1;_tmp80C=((struct Cyc_Absyn_Format_att_struct*)
_tmp80A)->f2;_tmp80D=((struct Cyc_Absyn_Format_att_struct*)_tmp80A)->f3;_LL54C:
if(!seen_format){seen_format=1;ft=_tmp80B;fmt_desc_arg=_tmp80C;fmt_arg_start=
_tmp80D;}else{({void*_tmp80E[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp80F="function can't have multiple format attributes";
_tag_dynforward(_tmp80F,sizeof(char),_get_zero_arr_size(_tmp80F,47));}),
_tag_dynforward(_tmp80E,sizeof(void*),0));});}goto _LL544;_LL54D:;_LL54E: goto
_LL544;_LL544:;}}if(num_convs > 1)({void*_tmp810[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp811="function can't have multiple calling conventions";
_tag_dynforward(_tmp811,sizeof(char),_get_zero_arr_size(_tmp811,49));}),
_tag_dynforward(_tmp810,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*
_tmp754);{struct Cyc_List_List*b=*_tmp754;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp812=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp813;_LL550: if(*((int*)
_tmp812)!= 0)goto _LL552;_tmp813=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp812)->f1;
if((int)_tmp813 != 1)goto _LL552;_LL551:({struct Cyc_String_pa_struct _tmp816;
_tmp816.tag=0;_tmp816.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp814[1]={& _tmp816};Cyc_Tcutil_terr(
loc,({const char*_tmp815="function attempts to abstract Mem type variable %s";
_tag_dynforward(_tmp815,sizeof(char),_get_zero_arr_size(_tmp815,51));}),
_tag_dynforward(_tmp814,sizeof(void*),1));}});goto _LL54F;_LL552:;_LL553: goto
_LL54F;_LL54F:;}}}{struct Cyc_Tcutil_CVTEnv _tmp817=({struct Cyc_Tcutil_CVTEnv
_tmp893;_tmp893.kind_env=cvtenv.kind_env;_tmp893.free_vars=0;_tmp893.free_evars=
0;_tmp893.generalize_evars=cvtenv.generalize_evars;_tmp893.fn_result=1;_tmp893;});
_tmp817=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp817,(void*)1,_tmp757,1);_tmp817.fn_result=
0;{struct Cyc_List_List*a=_tmp758;for(0;a != 0;a=a->tl){struct _tuple2*_tmp818=(
struct _tuple2*)a->hd;void*_tmp819=(*_tmp818).f3;_tmp817=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp817,(void*)1,_tmp819,1);((*_tmp818).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp818).f2).print_const,_tmp819);}}if(_tmp75A != 0){if(_tmp759)({void*
_tmp81A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp81B="both c_vararg and cyc_vararg";_tag_dynforward(_tmp81B,sizeof(
char),_get_zero_arr_size(_tmp81B,29));}),_tag_dynforward(_tmp81A,sizeof(void*),0));});{
void*_tmp81D;int _tmp81E;struct Cyc_Absyn_VarargInfo _tmp81C=*_tmp75A;_tmp81D=(void*)
_tmp81C.type;_tmp81E=_tmp81C.inject;_tmp817=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp817,(void*)1,_tmp81D,1);(_tmp75A->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp75A->tq).print_const,_tmp81D);if(_tmp81E){void*_tmp81F=Cyc_Tcutil_compress(
_tmp81D);struct Cyc_Absyn_TunionInfo _tmp820;void*_tmp821;_LL555: if(_tmp81F <= (
void*)4)goto _LL557;if(*((int*)_tmp81F)!= 2)goto _LL557;_tmp820=((struct Cyc_Absyn_TunionType_struct*)
_tmp81F)->f1;_tmp821=(void*)_tmp820.tunion_info;if(*((int*)_tmp821)!= 1)goto
_LL557;_LL556: goto _LL554;_LL557:;_LL558:({void*_tmp822[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp823="can't inject a non-[x]tunion type";_tag_dynforward(_tmp823,
sizeof(char),_get_zero_arr_size(_tmp823,34));}),_tag_dynforward(_tmp822,sizeof(
void*),0));});goto _LL554;_LL554:;}}}if(seen_format){int _tmp824=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp758);if((((fmt_desc_arg < 0  || fmt_desc_arg
> _tmp824) || fmt_arg_start < 0) || _tmp75A == 0  && fmt_arg_start != 0) || _tmp75A
!= 0  && fmt_arg_start != _tmp824 + 1)({void*_tmp825[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp826="bad format descriptor";_tag_dynforward(_tmp826,sizeof(char),
_get_zero_arr_size(_tmp826,22));}),_tag_dynforward(_tmp825,sizeof(void*),0));});
else{void*_tmp828;struct _tuple2 _tmp827=*((struct _tuple2*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp758,fmt_desc_arg - 1);_tmp828=_tmp827.f3;{void*_tmp829=
Cyc_Tcutil_compress(_tmp828);struct Cyc_Absyn_PtrInfo _tmp82A;void*_tmp82B;struct
Cyc_Absyn_PtrAtts _tmp82C;struct Cyc_Absyn_Conref*_tmp82D;struct Cyc_Absyn_Conref*
_tmp82E;_LL55A: if(_tmp829 <= (void*)4)goto _LL55C;if(*((int*)_tmp829)!= 4)goto
_LL55C;_tmp82A=((struct Cyc_Absyn_PointerType_struct*)_tmp829)->f1;_tmp82B=(void*)
_tmp82A.elt_typ;_tmp82C=_tmp82A.ptr_atts;_tmp82D=_tmp82C.bounds;_tmp82E=_tmp82C.zero_term;
_LL55B:{struct _tuple6 _tmp830=({struct _tuple6 _tmp82F;_tmp82F.f1=Cyc_Tcutil_compress(
_tmp82B);_tmp82F.f2=Cyc_Absyn_conref_def((void*)((void*)0),_tmp82D);_tmp82F;});
void*_tmp831;void*_tmp832;void*_tmp833;void*_tmp834;_LL55F: _tmp831=_tmp830.f1;
if(_tmp831 <= (void*)4)goto _LL561;if(*((int*)_tmp831)!= 5)goto _LL561;_tmp832=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp831)->f1;if((int)_tmp832 != 2)goto
_LL561;_tmp833=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp831)->f2;if((int)
_tmp833 != 0)goto _LL561;_tmp834=_tmp830.f2;if((int)_tmp834 != 0)goto _LL561;_LL560:
goto _LL55E;_LL561:;_LL562:({void*_tmp835[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp836="format descriptor is not a char ? type";_tag_dynforward(_tmp836,sizeof(
char),_get_zero_arr_size(_tmp836,39));}),_tag_dynforward(_tmp835,sizeof(void*),0));});
goto _LL55E;_LL55E:;}goto _LL559;_LL55C:;_LL55D:({void*_tmp837[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp838="format descriptor is not a char ? type";_tag_dynforward(
_tmp838,sizeof(char),_get_zero_arr_size(_tmp838,39));}),_tag_dynforward(_tmp837,
sizeof(void*),0));});goto _LL559;_LL559:;}if(fmt_arg_start != 0){int problem;{
struct _tuple6 _tmp83A=({struct _tuple6 _tmp839;_tmp839.f1=ft;_tmp839.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp75A))->type);_tmp839;});void*
_tmp83B;void*_tmp83C;struct Cyc_Absyn_TunionInfo _tmp83D;void*_tmp83E;struct Cyc_Absyn_Tuniondecl**
_tmp83F;struct Cyc_Absyn_Tuniondecl*_tmp840;void*_tmp841;void*_tmp842;struct Cyc_Absyn_TunionInfo
_tmp843;void*_tmp844;struct Cyc_Absyn_Tuniondecl**_tmp845;struct Cyc_Absyn_Tuniondecl*
_tmp846;_LL564: _tmp83B=_tmp83A.f1;if((int)_tmp83B != 0)goto _LL566;_tmp83C=_tmp83A.f2;
if(_tmp83C <= (void*)4)goto _LL566;if(*((int*)_tmp83C)!= 2)goto _LL566;_tmp83D=((
struct Cyc_Absyn_TunionType_struct*)_tmp83C)->f1;_tmp83E=(void*)_tmp83D.tunion_info;
if(*((int*)_tmp83E)!= 1)goto _LL566;_tmp83F=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp83E)->f1;_tmp840=*_tmp83F;_LL565: problem=Cyc_Absyn_qvar_cmp(_tmp840->name,
Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL563;_LL566: _tmp841=_tmp83A.f1;if((
int)_tmp841 != 1)goto _LL568;_tmp842=_tmp83A.f2;if(_tmp842 <= (void*)4)goto _LL568;
if(*((int*)_tmp842)!= 2)goto _LL568;_tmp843=((struct Cyc_Absyn_TunionType_struct*)
_tmp842)->f1;_tmp844=(void*)_tmp843.tunion_info;if(*((int*)_tmp844)!= 1)goto
_LL568;_tmp845=((struct Cyc_Absyn_KnownTunion_struct*)_tmp844)->f1;_tmp846=*
_tmp845;_LL567: problem=Cyc_Absyn_qvar_cmp(_tmp846->name,Cyc_Absyn_tunion_scanf_arg_qvar)
!= 0;goto _LL563;_LL568:;_LL569: problem=1;goto _LL563;_LL563:;}if(problem)({void*
_tmp847[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp848="format attribute and vararg types don't match";
_tag_dynforward(_tmp848,sizeof(char),_get_zero_arr_size(_tmp848,46));}),
_tag_dynforward(_tmp847,sizeof(void*),0));});}}}{struct Cyc_List_List*rpo=_tmp75B;
for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp84A;void*_tmp84B;void*_tmp84C;struct
_tuple6*_tmp849=(struct _tuple6*)rpo->hd;_tmp84A=*_tmp849;_tmp84B=_tmp84A.f1;
_tmp84C=_tmp84A.f2;_tmp817=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp817,(void*)4,
_tmp84B,0);_tmp817=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp817,(void*)3,_tmp84C,
0);}}if(*_tmp756 != 0)_tmp817=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp817,(void*)
4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp756))->v,0);else{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp817.free_vars;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp84D=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);
struct Cyc_Core_Opt*_tmp84E;struct Cyc_Core_Opt**_tmp84F;void*_tmp850;void*_tmp851;
void*_tmp852;void*_tmp853;struct Cyc_Core_Opt*_tmp854;struct Cyc_Core_Opt**_tmp855;
void*_tmp856;void*_tmp857;struct Cyc_Core_Opt*_tmp858;struct Cyc_Core_Opt**_tmp859;
_LL56B: if(*((int*)_tmp84D)!= 2)goto _LL56D;_tmp84E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp84D)->f1;_tmp84F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp84D)->f1;_tmp850=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp84D)->f2;if((
int)_tmp850 != 3)goto _LL56D;_LL56C:*_tmp84F=({struct Cyc_Core_Opt*_tmp85A=
_cycalloc(sizeof(*_tmp85A));_tmp85A->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp85B=_cycalloc(sizeof(*_tmp85B));_tmp85B[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp85C;_tmp85C.tag=0;_tmp85C.f1=(void*)((void*)3);_tmp85C;});_tmp85B;}));
_tmp85A;});goto _LL56E;_LL56D: if(*((int*)_tmp84D)!= 0)goto _LL56F;_tmp851=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp84D)->f1;if((int)_tmp851 != 3)goto _LL56F;_LL56E:
effect=({struct Cyc_List_List*_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp85E=_cycalloc(sizeof(*
_tmp85E));_tmp85E[0]=({struct Cyc_Absyn_AccessEff_struct _tmp85F;_tmp85F.tag=19;
_tmp85F.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp860=_cycalloc(
sizeof(*_tmp860));_tmp860[0]=({struct Cyc_Absyn_VarType_struct _tmp861;_tmp861.tag=
1;_tmp861.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp861;});_tmp860;}));_tmp85F;});
_tmp85E;}));_tmp85D->tl=effect;_tmp85D;});goto _LL56A;_LL56F: if(*((int*)_tmp84D)
!= 2)goto _LL571;_tmp852=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp84D)->f2;
if((int)_tmp852 != 5)goto _LL571;_LL570: goto _LL572;_LL571: if(*((int*)_tmp84D)!= 0)
goto _LL573;_tmp853=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp84D)->f1;if((int)
_tmp853 != 5)goto _LL573;_LL572: goto _LL56A;_LL573: if(*((int*)_tmp84D)!= 2)goto
_LL575;_tmp854=((struct Cyc_Absyn_Less_kb_struct*)_tmp84D)->f1;_tmp855=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp84D)->f1;_tmp856=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp84D)->f2;if((int)_tmp856 != 4)goto _LL575;_LL574:*_tmp855=({struct Cyc_Core_Opt*
_tmp862=_cycalloc(sizeof(*_tmp862));_tmp862->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp864;_tmp864.tag=0;_tmp864.f1=(void*)((void*)4);_tmp864;});_tmp863;}));
_tmp862;});goto _LL576;_LL575: if(*((int*)_tmp84D)!= 0)goto _LL577;_tmp857=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp84D)->f1;if((int)_tmp857 != 4)goto _LL577;_LL576:
effect=({struct Cyc_List_List*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp866=_cycalloc(sizeof(*_tmp866));
_tmp866[0]=({struct Cyc_Absyn_VarType_struct _tmp867;_tmp867.tag=1;_tmp867.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp867;});_tmp866;}));_tmp865->tl=effect;_tmp865;});
goto _LL56A;_LL577: if(*((int*)_tmp84D)!= 1)goto _LL579;_tmp858=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp84D)->f1;_tmp859=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp84D)->f1;_LL578:*_tmp859=({struct Cyc_Core_Opt*_tmp868=_cycalloc(sizeof(*
_tmp868));_tmp868->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp869=
_cycalloc(sizeof(*_tmp869));_tmp869[0]=({struct Cyc_Absyn_Less_kb_struct _tmp86A;
_tmp86A.tag=2;_tmp86A.f1=0;_tmp86A.f2=(void*)((void*)0);_tmp86A;});_tmp869;}));
_tmp868;});goto _LL57A;_LL579:;_LL57A: effect=({struct Cyc_List_List*_tmp86B=
_cycalloc(sizeof(*_tmp86B));_tmp86B->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp86D;_tmp86D.tag=21;_tmp86D.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp86E=_cycalloc(sizeof(*_tmp86E));_tmp86E[0]=({struct Cyc_Absyn_VarType_struct
_tmp86F;_tmp86F.tag=1;_tmp86F.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp86F;});
_tmp86E;}));_tmp86D;});_tmp86C;}));_tmp86B->tl=effect;_tmp86B;});goto _LL56A;
_LL56A:;}}{struct Cyc_List_List*ts=_tmp817.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp870=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL57C: if((int)_tmp870 != 3)goto
_LL57E;_LL57D: effect=({struct Cyc_List_List*_tmp871=_cycalloc(sizeof(*_tmp871));
_tmp871->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp872=_cycalloc(
sizeof(*_tmp872));_tmp872[0]=({struct Cyc_Absyn_AccessEff_struct _tmp873;_tmp873.tag=
19;_tmp873.f1=(void*)((void*)ts->hd);_tmp873;});_tmp872;}));_tmp871->tl=effect;
_tmp871;});goto _LL57B;_LL57E: if((int)_tmp870 != 4)goto _LL580;_LL57F: effect=({
struct Cyc_List_List*_tmp874=_cycalloc(sizeof(*_tmp874));_tmp874->hd=(void*)((
void*)ts->hd);_tmp874->tl=effect;_tmp874;});goto _LL57B;_LL580: if((int)_tmp870 != 
5)goto _LL582;_LL581: goto _LL57B;_LL582:;_LL583: effect=({struct Cyc_List_List*
_tmp875=_cycalloc(sizeof(*_tmp875));_tmp875->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp876=_cycalloc(sizeof(*_tmp876));_tmp876[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp877;_tmp877.tag=21;_tmp877.f1=(void*)((void*)ts->hd);_tmp877;});_tmp876;}));
_tmp875->tl=effect;_tmp875;});goto _LL57B;_LL57B:;}}*_tmp756=({struct Cyc_Core_Opt*
_tmp878=_cycalloc(sizeof(*_tmp878));_tmp878->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp87A;_tmp87A.tag=20;_tmp87A.f1=effect;_tmp87A;});_tmp879;}));_tmp878;});}if(*
_tmp754 != 0){struct Cyc_List_List*bs=*_tmp754;for(0;bs != 0;bs=bs->tl){void*
_tmp87B=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp87C;struct Cyc_Core_Opt**_tmp87D;struct Cyc_Core_Opt*
_tmp87E;struct Cyc_Core_Opt**_tmp87F;void*_tmp880;struct Cyc_Core_Opt*_tmp881;
struct Cyc_Core_Opt**_tmp882;void*_tmp883;struct Cyc_Core_Opt*_tmp884;struct Cyc_Core_Opt**
_tmp885;void*_tmp886;void*_tmp887;_LL585: if(*((int*)_tmp87B)!= 1)goto _LL587;
_tmp87C=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp87B)->f1;_tmp87D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp87B)->f1;_LL586:({struct Cyc_String_pa_struct
_tmp88A;_tmp88A.tag=0;_tmp88A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp888[1]={& _tmp88A};Cyc_Tcutil_warn(
loc,({const char*_tmp889="Type variable %s unconstrained, assuming boxed";
_tag_dynforward(_tmp889,sizeof(char),_get_zero_arr_size(_tmp889,47));}),
_tag_dynforward(_tmp888,sizeof(void*),1));}});_tmp87F=_tmp87D;goto _LL588;_LL587:
if(*((int*)_tmp87B)!= 2)goto _LL589;_tmp87E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp87B)->f1;_tmp87F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp87B)->f1;_tmp880=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp87B)->f2;if((
int)_tmp880 != 1)goto _LL589;_LL588: _tmp882=_tmp87F;goto _LL58A;_LL589: if(*((int*)
_tmp87B)!= 2)goto _LL58B;_tmp881=((struct Cyc_Absyn_Less_kb_struct*)_tmp87B)->f1;
_tmp882=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp87B)->f1;
_tmp883=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp87B)->f2;if((int)_tmp883 != 
0)goto _LL58B;_LL58A:*_tmp882=({struct Cyc_Core_Opt*_tmp88B=_cycalloc(sizeof(*
_tmp88B));_tmp88B->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp88C=
_cycalloc(sizeof(*_tmp88C));_tmp88C[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp88D;
_tmp88D.tag=0;_tmp88D.f1=(void*)((void*)2);_tmp88D;});_tmp88C;}));_tmp88B;});
goto _LL584;_LL58B: if(*((int*)_tmp87B)!= 2)goto _LL58D;_tmp884=((struct Cyc_Absyn_Less_kb_struct*)
_tmp87B)->f1;_tmp885=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp87B)->f1;_tmp886=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp87B)->f2;
_LL58C:*_tmp885=({struct Cyc_Core_Opt*_tmp88E=_cycalloc(sizeof(*_tmp88E));_tmp88E->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp88F=_cycalloc(sizeof(*_tmp88F));
_tmp88F[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp890;_tmp890.tag=0;_tmp890.f1=(void*)
_tmp886;_tmp890;});_tmp88F;}));_tmp88E;});goto _LL584;_LL58D: if(*((int*)_tmp87B)
!= 0)goto _LL58F;_tmp887=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp87B)->f1;if((
int)_tmp887 != 1)goto _LL58F;_LL58E:({void*_tmp891[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp892="functions can't abstract M types";_tag_dynforward(_tmp892,
sizeof(char),_get_zero_arr_size(_tmp892,33));}),_tag_dynforward(_tmp891,sizeof(
void*),0));});goto _LL584;_LL58F:;_LL590: goto _LL584;_LL584:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp817.kind_env,*_tmp754);_tmp817.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp817.free_vars,*_tmp754);{struct Cyc_List_List*tvs=_tmp817.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp817.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL4C7;}}_LL4E6: if(*((int*)_tmp72B)!= 9)goto _LL4E8;_tmp75D=((
struct Cyc_Absyn_TupleType_struct*)_tmp72B)->f1;_LL4E7: for(0;_tmp75D != 0;_tmp75D=
_tmp75D->tl){struct _tuple4*_tmp894=(struct _tuple4*)_tmp75D->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,(*_tmp894).f2,0);((*_tmp894).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp894).f1).print_const,(*_tmp894).f2);}goto _LL4C7;_LL4E8: if(*((int*)
_tmp72B)!= 11)goto _LL4EA;_tmp75E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp72B)->f1;_tmp75F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp72B)->f2;_LL4E9:{
struct _RegionHandle _tmp895=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp895;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp75F != 0;_tmp75F=_tmp75F->tl){struct Cyc_Absyn_Aggrfield _tmp897;struct
_dynforward_ptr*_tmp898;struct Cyc_Absyn_Tqual _tmp899;struct Cyc_Absyn_Tqual*
_tmp89A;void*_tmp89B;struct Cyc_Absyn_Exp*_tmp89C;struct Cyc_List_List*_tmp89D;
struct Cyc_Absyn_Aggrfield*_tmp896=(struct Cyc_Absyn_Aggrfield*)_tmp75F->hd;
_tmp897=*_tmp896;_tmp898=_tmp897.name;_tmp899=_tmp897.tq;_tmp89A=(struct Cyc_Absyn_Tqual*)&(*
_tmp896).tq;_tmp89B=(void*)_tmp897.type;_tmp89C=_tmp897.width;_tmp89D=_tmp897.attributes;
if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct
Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,
_tmp898))({struct Cyc_String_pa_struct _tmp8A0;_tmp8A0.tag=0;_tmp8A0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp898);{void*_tmp89E[1]={& _tmp8A0};
Cyc_Tcutil_terr(loc,({const char*_tmp89F="duplicate field %s";_tag_dynforward(
_tmp89F,sizeof(char),_get_zero_arr_size(_tmp89F,19));}),_tag_dynforward(_tmp89E,
sizeof(void*),1));}});if(Cyc_strcmp((struct _dynforward_ptr)*_tmp898,({const char*
_tmp8A1="";_tag_dynforward(_tmp8A1,sizeof(char),_get_zero_arr_size(_tmp8A1,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp8A2=_region_malloc(aprev_rgn,sizeof(*
_tmp8A2));_tmp8A2->hd=_tmp898;_tmp8A2->tl=prev_fields;_tmp8A2;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp89B,0);_tmp89A->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp89A->print_const,_tmp89B);if(_tmp75E == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp89B))({struct Cyc_String_pa_struct _tmp8A5;_tmp8A5.tag=0;_tmp8A5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp898);{void*_tmp8A3[1]={& _tmp8A5};
Cyc_Tcutil_warn(loc,({const char*_tmp8A4="union member %s is not `bits-only' so it can only be written and not read";
_tag_dynforward(_tmp8A4,sizeof(char),_get_zero_arr_size(_tmp8A4,74));}),
_tag_dynforward(_tmp8A3,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,
_tmp89B,_tmp89C,_tmp898);Cyc_Tcutil_check_field_atts(loc,_tmp898,_tmp89D);}};
_pop_region(aprev_rgn);}goto _LL4C7;_LL4EA: if(*((int*)_tmp72B)!= 10)goto _LL4EC;
_tmp760=((struct Cyc_Absyn_AggrType_struct*)_tmp72B)->f1;_tmp761=(void*)_tmp760.aggr_info;
_tmp762=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp72B)->f1).aggr_info;
_tmp763=_tmp760.targs;_tmp764=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp72B)->f1).targs;_LL4EB:{void*_tmp8A6=*_tmp762;void*_tmp8A7;struct _tuple1*
_tmp8A8;struct Cyc_Absyn_Aggrdecl**_tmp8A9;struct Cyc_Absyn_Aggrdecl*_tmp8AA;
_LL592: if(*((int*)_tmp8A6)!= 0)goto _LL594;_tmp8A7=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp8A6)->f1;_tmp8A8=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8A6)->f2;_LL593: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp8AB;_push_handler(& _tmp8AB);{
int _tmp8AD=0;if(setjmp(_tmp8AB.handler))_tmp8AD=1;if(!_tmp8AD){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp8A8);*_tmp762=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp8AE=
_cycalloc(sizeof(*_tmp8AE));_tmp8AE[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp8AF;
_tmp8AF.tag=1;_tmp8AF.f1=adp;_tmp8AF;});_tmp8AE;});;_pop_handler();}else{void*
_tmp8AC=(void*)_exn_thrown;void*_tmp8B1=_tmp8AC;_LL597: if(_tmp8B1 != Cyc_Dict_Absent)
goto _LL599;_LL598: {struct Cyc_Tcenv_Genv*_tmp8B2=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp8B3=({struct Cyc_Absyn_Aggrdecl*_tmp8B9=_cycalloc(
sizeof(*_tmp8B9));_tmp8B9->kind=(void*)_tmp8A7;_tmp8B9->sc=(void*)((void*)3);
_tmp8B9->name=_tmp8A8;_tmp8B9->tvs=0;_tmp8B9->impl=0;_tmp8B9->attributes=0;
_tmp8B9;});Cyc_Tc_tcAggrdecl(te,_tmp8B2,loc,_tmp8B3);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp8A8);*_tmp762=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp8B4=
_cycalloc(sizeof(*_tmp8B4));_tmp8B4[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp8B5;
_tmp8B5.tag=1;_tmp8B5.f1=adp;_tmp8B5;});_tmp8B4;});if(*_tmp764 != 0){({struct Cyc_String_pa_struct
_tmp8B8;_tmp8B8.tag=0;_tmp8B8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp8A8));{void*_tmp8B6[1]={& _tmp8B8};Cyc_Tcutil_terr(
loc,({const char*_tmp8B7="declare parameterized type %s before using";
_tag_dynforward(_tmp8B7,sizeof(char),_get_zero_arr_size(_tmp8B7,43));}),
_tag_dynforward(_tmp8B6,sizeof(void*),1));}});return cvtenv;}goto _LL596;}_LL599:;
_LL59A:(void)_throw(_tmp8B1);_LL596:;}}}_tmp8AA=*adp;goto _LL595;}_LL594: if(*((
int*)_tmp8A6)!= 1)goto _LL591;_tmp8A9=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8A6)->f1;
_tmp8AA=*_tmp8A9;_LL595: {struct Cyc_List_List*tvs=_tmp8AA->tvs;struct Cyc_List_List*
ts=*_tmp764;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,0);}if(ts != 0)({struct Cyc_String_pa_struct _tmp8BC;_tmp8BC.tag=
0;_tmp8BC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp8AA->name));{void*_tmp8BA[1]={& _tmp8BC};Cyc_Tcutil_terr(loc,({const char*
_tmp8BB="too many parameters for type %s";_tag_dynforward(_tmp8BB,sizeof(char),
_get_zero_arr_size(_tmp8BB,32));}),_tag_dynforward(_tmp8BA,sizeof(void*),1));}});
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);hidden_ts=({struct Cyc_List_List*_tmp8BD=_cycalloc(sizeof(*_tmp8BD));_tmp8BD->hd=(
void*)e;_tmp8BD->tl=hidden_ts;_tmp8BD;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,0);}*_tmp764=Cyc_List_imp_append(*_tmp764,Cyc_List_imp_rev(
hidden_ts));}}_LL591:;}goto _LL4C7;_LL4EC: if(*((int*)_tmp72B)!= 16)goto _LL4EE;
_tmp765=((struct Cyc_Absyn_TypedefType_struct*)_tmp72B)->f1;_tmp766=((struct Cyc_Absyn_TypedefType_struct*)
_tmp72B)->f2;_tmp767=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp72B)->f2;_tmp768=((struct Cyc_Absyn_TypedefType_struct*)_tmp72B)->f3;_tmp769=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp72B)->f3;
_tmp76A=((struct Cyc_Absyn_TypedefType_struct*)_tmp72B)->f4;_tmp76B=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp72B)->f4;_LL4ED: {struct Cyc_List_List*
targs=*_tmp767;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp8BE;
_push_handler(& _tmp8BE);{int _tmp8C0=0;if(setjmp(_tmp8BE.handler))_tmp8C0=1;if(!
_tmp8C0){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp765);;_pop_handler();}else{
void*_tmp8BF=(void*)_exn_thrown;void*_tmp8C2=_tmp8BF;_LL59C: if(_tmp8C2 != Cyc_Dict_Absent)
goto _LL59E;_LL59D:({struct Cyc_String_pa_struct _tmp8C5;_tmp8C5.tag=0;_tmp8C5.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp765));{
void*_tmp8C3[1]={& _tmp8C5};Cyc_Tcutil_terr(loc,({const char*_tmp8C4="unbound typedef name %s";
_tag_dynforward(_tmp8C4,sizeof(char),_get_zero_arr_size(_tmp8C4,24));}),
_tag_dynforward(_tmp8C3,sizeof(void*),1));}});return cvtenv;_LL59E:;_LL59F:(void)
_throw(_tmp8C2);_LL59B:;}}}*_tmp769=(struct Cyc_Absyn_Typedefdecl*)td;_tmp765[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(
ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,0);inst=({
struct Cyc_List_List*_tmp8C6=_cycalloc(sizeof(*_tmp8C6));_tmp8C6->hd=({struct
_tuple8*_tmp8C7=_cycalloc(sizeof(*_tmp8C7));_tmp8C7->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp8C7->f2=(void*)ts->hd;_tmp8C7;});_tmp8C6->tl=inst;_tmp8C6;});}if(ts
!= 0)({struct Cyc_String_pa_struct _tmp8CA;_tmp8CA.tag=0;_tmp8CA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp765));{void*
_tmp8C8[1]={& _tmp8CA};Cyc_Tcutil_terr(loc,({const char*_tmp8C9="too many parameters for typedef %s";
_tag_dynforward(_tmp8C9,sizeof(char),_get_zero_arr_size(_tmp8C9,35));}),
_tag_dynforward(_tmp8C8,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp8CB=
_cycalloc(sizeof(*_tmp8CB));_tmp8CB->hd=(void*)e;_tmp8CB->tl=hidden_ts;_tmp8CB;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,0);inst=({struct Cyc_List_List*
_tmp8CC=_cycalloc(sizeof(*_tmp8CC));_tmp8CC->hd=({struct _tuple8*_tmp8CD=
_cycalloc(sizeof(*_tmp8CD));_tmp8CD->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8CD->f2=
e;_tmp8CD;});_tmp8CC->tl=inst;_tmp8CC;});}*_tmp767=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp76B=({void**_tmp8CE=
_cycalloc(sizeof(*_tmp8CE));_tmp8CE[0]=new_typ;_tmp8CE;});}goto _LL4C7;}}_LL4EE:
if((int)_tmp72B != 3)goto _LL4F0;_LL4EF: if(!allow_unique)({void*_tmp8CF[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp8D0="`U only allowed in outermost pointers";_tag_dynforward(
_tmp8D0,sizeof(char),_get_zero_arr_size(_tmp8D0,38));}),_tag_dynforward(_tmp8CF,
sizeof(void*),0));});goto _LL4C7;_LL4F0: if(_tmp72B <= (void*)4)goto _LL4F2;if(*((
int*)_tmp72B)!= 18)goto _LL4F2;_LL4F1: goto _LL4F3;_LL4F2: if((int)_tmp72B != 2)goto
_LL4F4;_LL4F3: goto _LL4C7;_LL4F4: if(_tmp72B <= (void*)4)goto _LL4F6;if(*((int*)
_tmp72B)!= 15)goto _LL4F6;_tmp76C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp72B)->f1;_LL4F5: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp76C,1);goto _LL4C7;_LL4F6: if(_tmp72B <= (void*)4)goto _LL4F8;if(*((int*)_tmp72B)
!= 19)goto _LL4F8;_tmp76D=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp72B)->f1;
_LL4F7: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp76D,0);
goto _LL4C7;_LL4F8: if(_tmp72B <= (void*)4)goto _LL4FA;if(*((int*)_tmp72B)!= 21)goto
_LL4FA;_tmp76E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp72B)->f1;_LL4F9:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp76E,0);goto
_LL4C7;_LL4FA: if(_tmp72B <= (void*)4)goto _LL4C7;if(*((int*)_tmp72B)!= 20)goto
_LL4C7;_tmp76F=((struct Cyc_Absyn_JoinEff_struct*)_tmp72B)->f1;_LL4FB: for(0;
_tmp76F != 0;_tmp76F=_tmp76F->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)4,(void*)_tmp76F->hd,0);}goto _LL4C7;_LL4C7:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind))({struct Cyc_String_pa_struct _tmp8D5;
_tmp8D5.tag=0;_tmp8D5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
expected_kind));{struct Cyc_String_pa_struct _tmp8D4;_tmp8D4.tag=0;_tmp8D4.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t)));{struct Cyc_String_pa_struct _tmp8D3;_tmp8D3.tag=0;_tmp8D3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp8D1[
3]={& _tmp8D3,& _tmp8D4,& _tmp8D5};Cyc_Tcutil_terr(loc,({const char*_tmp8D2="type %s has kind %s but as used here needs kind %s";
_tag_dynforward(_tmp8D2,sizeof(char),_get_zero_arr_size(_tmp8D2,51));}),
_tag_dynforward(_tmp8D1,sizeof(void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*kind_env,void*expected_kind,int generalize_evars,void*t,
int allow_unique){struct Cyc_Tcutil_CVTEnv _tmp8D6=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp8DF;_tmp8DF.kind_env=kind_env;_tmp8DF.free_vars=
0;_tmp8DF.free_evars=0;_tmp8DF.generalize_evars=generalize_evars;_tmp8DF.fn_result=
0;_tmp8DF;}),expected_kind,t,allow_unique);{struct Cyc_List_List*vs=_tmp8D6.free_vars;
for(0;vs != 0;vs=vs->tl){_tmp8D6.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(
struct Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp8D6.free_evars;for(
0;evs != 0;evs=evs->tl){void*_tmp8D7=Cyc_Tcutil_compress((void*)evs->hd);struct
Cyc_Core_Opt*_tmp8D8;struct Cyc_Core_Opt**_tmp8D9;_LL5A1: if(_tmp8D7 <= (void*)4)
goto _LL5A3;if(*((int*)_tmp8D7)!= 0)goto _LL5A3;_tmp8D8=((struct Cyc_Absyn_Evar_struct*)
_tmp8D7)->f4;_tmp8D9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp8D7)->f4;_LL5A2: if(*_tmp8D9 == 0)*_tmp8D9=({struct Cyc_Core_Opt*_tmp8DA=
_cycalloc(sizeof(*_tmp8DA));_tmp8DA->v=kind_env;_tmp8DA;});else{struct Cyc_List_List*
_tmp8DB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp8D9))->v;
struct Cyc_List_List*_tmp8DC=0;for(0;_tmp8DB != 0;_tmp8DB=_tmp8DB->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct
Cyc_Absyn_Tvar*)_tmp8DB->hd))_tmp8DC=({struct Cyc_List_List*_tmp8DD=_cycalloc(
sizeof(*_tmp8DD));_tmp8DD->hd=(struct Cyc_Absyn_Tvar*)_tmp8DB->hd;_tmp8DD->tl=
_tmp8DC;_tmp8DD;});}*_tmp8D9=({struct Cyc_Core_Opt*_tmp8DE=_cycalloc(sizeof(*
_tmp8DE));_tmp8DE->v=_tmp8DC;_tmp8DE;});}goto _LL5A0;_LL5A3:;_LL5A4: goto _LL5A0;
_LL5A0:;}}return _tmp8D6;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t,int allow_unique){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmp8E0=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Tcutil_CVTEnv
_tmp8E1=Cyc_Tcutil_check_valid_type(loc,te,_tmp8E0,(void*)1,generalize_evars,t,
allow_unique);struct Cyc_List_List*_tmp8E2=_tmp8E1.free_vars;struct Cyc_List_List*
_tmp8E3=_tmp8E1.free_evars;if(_tmp8E0 != 0){struct Cyc_List_List*_tmp8E4=0;{struct
Cyc_List_List*_tmp8E5=_tmp8E2;for(0;(unsigned int)_tmp8E5;_tmp8E5=_tmp8E5->tl){
struct Cyc_Absyn_Tvar*_tmp8E6=(struct Cyc_Absyn_Tvar*)_tmp8E5->hd;int found=0;{
struct Cyc_List_List*_tmp8E7=_tmp8E0;for(0;(unsigned int)_tmp8E7;_tmp8E7=_tmp8E7->tl){
if(Cyc_Absyn_tvar_cmp(_tmp8E6,(struct Cyc_Absyn_Tvar*)_tmp8E7->hd)== 0){found=1;
break;}}}if(!found)_tmp8E4=({struct Cyc_List_List*_tmp8E8=_cycalloc(sizeof(*
_tmp8E8));_tmp8E8->hd=(struct Cyc_Absyn_Tvar*)_tmp8E5->hd;_tmp8E8->tl=_tmp8E4;
_tmp8E8;});}}_tmp8E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp8E4);}{struct Cyc_List_List*x=_tmp8E2;for(0;x != 0;x=x->tl){void*_tmp8E9=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp8EA;struct Cyc_Core_Opt**
_tmp8EB;struct Cyc_Core_Opt*_tmp8EC;struct Cyc_Core_Opt**_tmp8ED;void*_tmp8EE;
struct Cyc_Core_Opt*_tmp8EF;struct Cyc_Core_Opt**_tmp8F0;void*_tmp8F1;struct Cyc_Core_Opt*
_tmp8F2;struct Cyc_Core_Opt**_tmp8F3;void*_tmp8F4;void*_tmp8F5;_LL5A6: if(*((int*)
_tmp8E9)!= 1)goto _LL5A8;_tmp8EA=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8E9)->f1;
_tmp8EB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8E9)->f1;
_LL5A7: _tmp8ED=_tmp8EB;goto _LL5A9;_LL5A8: if(*((int*)_tmp8E9)!= 2)goto _LL5AA;
_tmp8EC=((struct Cyc_Absyn_Less_kb_struct*)_tmp8E9)->f1;_tmp8ED=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8E9)->f1;_tmp8EE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E9)->f2;if((int)_tmp8EE != 1)goto _LL5AA;_LL5A9: _tmp8F0=_tmp8ED;goto _LL5AB;
_LL5AA: if(*((int*)_tmp8E9)!= 2)goto _LL5AC;_tmp8EF=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E9)->f1;_tmp8F0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E9)->f1;_tmp8F1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E9)->f2;if((
int)_tmp8F1 != 0)goto _LL5AC;_LL5AB:*_tmp8F0=({struct Cyc_Core_Opt*_tmp8F6=
_cycalloc(sizeof(*_tmp8F6));_tmp8F6->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp8F7=_cycalloc(sizeof(*_tmp8F7));_tmp8F7[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp8F8;_tmp8F8.tag=0;_tmp8F8.f1=(void*)((void*)2);_tmp8F8;});_tmp8F7;}));
_tmp8F6;});goto _LL5A5;_LL5AC: if(*((int*)_tmp8E9)!= 2)goto _LL5AE;_tmp8F2=((struct
Cyc_Absyn_Less_kb_struct*)_tmp8E9)->f1;_tmp8F3=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp8E9)->f1;_tmp8F4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E9)->f2;_LL5AD:*_tmp8F3=({struct Cyc_Core_Opt*_tmp8F9=_cycalloc(sizeof(*
_tmp8F9));_tmp8F9->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8FA=
_cycalloc(sizeof(*_tmp8FA));_tmp8FA[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8FB;
_tmp8FB.tag=0;_tmp8FB.f1=(void*)_tmp8F4;_tmp8FB;});_tmp8FA;}));_tmp8F9;});goto
_LL5A5;_LL5AE: if(*((int*)_tmp8E9)!= 0)goto _LL5B0;_tmp8F5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8E9)->f1;if((int)_tmp8F5 != 1)goto _LL5B0;_LL5AF:({struct Cyc_String_pa_struct
_tmp8FE;_tmp8FE.tag=0;_tmp8FE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp8FC[1]={& _tmp8FE};
Cyc_Tcutil_terr(loc,({const char*_tmp8FD="type variable %s cannot have kind M";
_tag_dynforward(_tmp8FD,sizeof(char),_get_zero_arr_size(_tmp8FD,36));}),
_tag_dynforward(_tmp8FC,sizeof(void*),1));}});goto _LL5A5;_LL5B0:;_LL5B1: goto
_LL5A5;_LL5A5:;}}if(_tmp8E2 != 0  || _tmp8E3 != 0){{void*_tmp8FF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp900;struct Cyc_List_List*_tmp901;struct Cyc_List_List**
_tmp902;struct Cyc_Core_Opt*_tmp903;void*_tmp904;struct Cyc_List_List*_tmp905;int
_tmp906;struct Cyc_Absyn_VarargInfo*_tmp907;struct Cyc_List_List*_tmp908;struct Cyc_List_List*
_tmp909;_LL5B3: if(_tmp8FF <= (void*)4)goto _LL5B5;if(*((int*)_tmp8FF)!= 8)goto
_LL5B5;_tmp900=((struct Cyc_Absyn_FnType_struct*)_tmp8FF)->f1;_tmp901=_tmp900.tvars;
_tmp902=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp8FF)->f1).tvars;
_tmp903=_tmp900.effect;_tmp904=(void*)_tmp900.ret_typ;_tmp905=_tmp900.args;
_tmp906=_tmp900.c_varargs;_tmp907=_tmp900.cyc_varargs;_tmp908=_tmp900.rgn_po;
_tmp909=_tmp900.attributes;_LL5B4: if(*_tmp902 == 0){*_tmp902=_tmp8E2;_tmp8E2=0;}
goto _LL5B2;_LL5B5:;_LL5B6: goto _LL5B2;_LL5B2:;}if(_tmp8E2 != 0)({struct Cyc_String_pa_struct
_tmp90C;_tmp90C.tag=0;_tmp90C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)_tmp8E2->hd)->name);{void*_tmp90A[1]={& _tmp90C};Cyc_Tcutil_terr(
loc,({const char*_tmp90B="unbound type variable %s";_tag_dynforward(_tmp90B,
sizeof(char),_get_zero_arr_size(_tmp90B,25));}),_tag_dynforward(_tmp90A,sizeof(
void*),1));}});if(_tmp8E3 != 0)for(0;_tmp8E3 != 0;_tmp8E3=_tmp8E3->tl){void*e=(
void*)_tmp8E3->hd;void*_tmp90D=Cyc_Tcutil_typ_kind(e);_LL5B8: if((int)_tmp90D != 3)
goto _LL5BA;_LL5B9: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp90E[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp90F="can't unify evar with heap!";_tag_dynforward(_tmp90F,sizeof(char),
_get_zero_arr_size(_tmp90F,28));}),_tag_dynforward(_tmp90E,sizeof(void*),0));});
goto _LL5B7;_LL5BA: if((int)_tmp90D != 4)goto _LL5BC;_LL5BB: if(!Cyc_Tcutil_unify(e,
Cyc_Absyn_empty_effect))({void*_tmp910[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp911="can't unify evar with {}!";
_tag_dynforward(_tmp911,sizeof(char),_get_zero_arr_size(_tmp911,26));}),
_tag_dynforward(_tmp910,sizeof(void*),0));});goto _LL5B7;_LL5BC:;_LL5BD:({struct
Cyc_String_pa_struct _tmp915;_tmp915.tag=0;_tmp915.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp914;_tmp914.tag=0;_tmp914.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp912[2]={& _tmp914,& _tmp915};Cyc_Tcutil_terr(
loc,({const char*_tmp913="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp913,sizeof(char),_get_zero_arr_size(_tmp913,52));}),
_tag_dynforward(_tmp912,sizeof(void*),2));}}});goto _LL5B7;_LL5B7:;}}}void Cyc_Tcutil_check_no_unique_region(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp916;_tmp916.kind_env=0;_tmp916.free_vars=0;
_tmp916.free_evars=0;_tmp916.generalize_evars=0;_tmp916.fn_result=0;_tmp916;}),(
void*)1,t,0);}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t,1);{void*_tmp917=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp918;struct Cyc_List_List*_tmp919;struct Cyc_Core_Opt*
_tmp91A;void*_tmp91B;_LL5BF: if(_tmp917 <= (void*)4)goto _LL5C1;if(*((int*)_tmp917)
!= 8)goto _LL5C1;_tmp918=((struct Cyc_Absyn_FnType_struct*)_tmp917)->f1;_tmp919=
_tmp918.tvars;_tmp91A=_tmp918.effect;_tmp91B=(void*)_tmp918.ret_typ;_LL5C0: fd->tvs=
_tmp919;fd->effect=_tmp91A;goto _LL5BE;_LL5C1:;_LL5C2:({void*_tmp91C[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp91D="check_fndecl_valid_type: not a FnType";_tag_dynforward(_tmp91D,sizeof(
char),_get_zero_arr_size(_tmp91D,38));}),_tag_dynforward(_tmp91C,sizeof(void*),0));});
_LL5BE:;}{struct _RegionHandle _tmp91E=_new_region("r");struct _RegionHandle*r=&
_tmp91E;_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr*(*f)(struct _tuple13*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,(struct _dynforward_ptr*(*)(struct _tuple13*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,({const char*_tmp91F="function declaration has repeated parameter";
_tag_dynforward(_tmp91F,sizeof(char),_get_zero_arr_size(_tmp91F,44));}));;
_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp920=_cycalloc(sizeof(*
_tmp920));_tmp920->v=(void*)t;_tmp920;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,
int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp921=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t,1);struct Cyc_List_List*_tmp922=Cyc_Tcutil_remove_bound_tvars(
_tmp921.free_vars,bound_tvars);struct Cyc_List_List*_tmp923=_tmp921.free_evars;{
struct Cyc_List_List*fs=_tmp922;for(0;fs != 0;fs=fs->tl){struct _dynforward_ptr*
_tmp924=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct
_tmp928;_tmp928.tag=0;_tmp928.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp927;_tmp927.tag=0;
_tmp927.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp924);{void*
_tmp925[2]={& _tmp927,& _tmp928};Cyc_Tcutil_terr(loc,({const char*_tmp926="unbound type variable %s in type %s";
_tag_dynforward(_tmp926,sizeof(char),_get_zero_arr_size(_tmp926,36));}),
_tag_dynforward(_tmp925,sizeof(void*),2));}}});}}if(!allow_evars  && _tmp923 != 0)
for(0;_tmp923 != 0;_tmp923=_tmp923->tl){void*e=(void*)_tmp923->hd;void*_tmp929=
Cyc_Tcutil_typ_kind(e);_LL5C4: if((int)_tmp929 != 3)goto _LL5C6;_LL5C5: if(!Cyc_Tcutil_unify(
e,(void*)2))({void*_tmp92A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp92B="can't unify evar with heap!";
_tag_dynforward(_tmp92B,sizeof(char),_get_zero_arr_size(_tmp92B,28));}),
_tag_dynforward(_tmp92A,sizeof(void*),0));});goto _LL5C3;_LL5C6: if((int)_tmp929 != 
4)goto _LL5C8;_LL5C7: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*
_tmp92C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp92D="can't unify evar with {}!";_tag_dynforward(_tmp92D,sizeof(
char),_get_zero_arr_size(_tmp92D,26));}),_tag_dynforward(_tmp92C,sizeof(void*),0));});
goto _LL5C3;_LL5C8:;_LL5C9:({struct Cyc_String_pa_struct _tmp931;_tmp931.tag=0;
_tmp931.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp930;_tmp930.tag=0;_tmp930.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(e));{void*_tmp92E[
2]={& _tmp930,& _tmp931};Cyc_Tcutil_terr(loc,({const char*_tmp92F="hidden type variable %s isn't abstracted in type %s";
_tag_dynforward(_tmp92F,sizeof(char),_get_zero_arr_size(_tmp92F,52));}),
_tag_dynforward(_tmp92E,sizeof(void*),2));}}});goto _LL5C3;_LL5C3:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr(*a2string)(
void*),struct _dynforward_ptr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=
vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({
struct Cyc_String_pa_struct _tmp935;_tmp935.tag=0;_tmp935.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)a2string((void*)vs->hd));{struct Cyc_String_pa_struct
_tmp934;_tmp934.tag=0;_tmp934.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
msg);{void*_tmp932[2]={& _tmp934,& _tmp935};Cyc_Tcutil_terr(loc,({const char*
_tmp933="%s: %s";_tag_dynforward(_tmp933,sizeof(char),_get_zero_arr_size(_tmp933,
7));}),_tag_dynforward(_tmp932,sizeof(void*),2));}}});}}}static struct
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
Cyc_Tcutil_tvar2string,({const char*_tmp936="duplicate type variable";
_tag_dynforward(_tmp936,sizeof(char),_get_zero_arr_size(_tmp936,24));}));}struct
_tuple18{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple19{struct Cyc_List_List*
f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_strcmp((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,({const char*_tmp937="";_tag_dynforward(_tmp937,sizeof(char),
_get_zero_arr_size(_tmp937,1));}))!= 0)fields=({struct Cyc_List_List*_tmp938=
_cycalloc(sizeof(*_tmp938));_tmp938->hd=({struct _tuple18*_tmp939=_cycalloc(
sizeof(*_tmp939));_tmp939->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp939->f2=
0;_tmp939;});_tmp938->tl=fields;_tmp938;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple19 _tmp93B;struct Cyc_List_List*_tmp93C;void*
_tmp93D;struct _tuple19*_tmp93A=(struct _tuple19*)des->hd;_tmp93B=*_tmp93A;_tmp93C=
_tmp93B.f1;_tmp93D=_tmp93B.f2;if(_tmp93C == 0){struct Cyc_List_List*_tmp93E=fields;
for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){if(!(*((struct _tuple18*)_tmp93E->hd)).f2){(*((
struct _tuple18*)_tmp93E->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp93F=_cycalloc(sizeof(*_tmp93F));_tmp93F->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp940=_cycalloc(sizeof(*_tmp940));
_tmp940[0]=({struct Cyc_Absyn_FieldName_struct _tmp941;_tmp941.tag=1;_tmp941.f1=((*((
struct _tuple18*)_tmp93E->hd)).f1)->name;_tmp941;});_tmp940;}));_tmp93F->tl=0;
_tmp93F;});ans=({struct Cyc_List_List*_tmp942=_region_malloc(rgn,sizeof(*_tmp942));
_tmp942->hd=({struct _tuple20*_tmp943=_region_malloc(rgn,sizeof(*_tmp943));
_tmp943->f1=(*((struct _tuple18*)_tmp93E->hd)).f1;_tmp943->f2=_tmp93D;_tmp943;});
_tmp942->tl=ans;_tmp942;});break;}}if(_tmp93E == 0)({void*_tmp944[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp945="too many arguments to struct";_tag_dynforward(_tmp945,
sizeof(char),_get_zero_arr_size(_tmp945,29));}),_tag_dynforward(_tmp944,sizeof(
void*),0));});}else{if(_tmp93C->tl != 0)({void*_tmp946[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp947="multiple designators are not supported";_tag_dynforward(
_tmp947,sizeof(char),_get_zero_arr_size(_tmp947,39));}),_tag_dynforward(_tmp946,
sizeof(void*),0));});else{void*_tmp948=(void*)_tmp93C->hd;struct _dynforward_ptr*
_tmp949;_LL5CB: if(*((int*)_tmp948)!= 0)goto _LL5CD;_LL5CC:({void*_tmp94A[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp94B="array designator used in argument to struct";
_tag_dynforward(_tmp94B,sizeof(char),_get_zero_arr_size(_tmp94B,44));}),
_tag_dynforward(_tmp94A,sizeof(void*),0));});goto _LL5CA;_LL5CD: if(*((int*)
_tmp948)!= 1)goto _LL5CA;_tmp949=((struct Cyc_Absyn_FieldName_struct*)_tmp948)->f1;
_LL5CE: {struct Cyc_List_List*_tmp94C=fields;for(0;_tmp94C != 0;_tmp94C=_tmp94C->tl){
if(Cyc_strptrcmp(_tmp949,((*((struct _tuple18*)_tmp94C->hd)).f1)->name)== 0){if((*((
struct _tuple18*)_tmp94C->hd)).f2)({struct Cyc_String_pa_struct _tmp94F;_tmp94F.tag=
0;_tmp94F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp949);{void*
_tmp94D[1]={& _tmp94F};Cyc_Tcutil_terr(loc,({const char*_tmp94E="field %s has already been used as an argument";
_tag_dynforward(_tmp94E,sizeof(char),_get_zero_arr_size(_tmp94E,46));}),
_tag_dynforward(_tmp94D,sizeof(void*),1));}});(*((struct _tuple18*)_tmp94C->hd)).f2=
1;ans=({struct Cyc_List_List*_tmp950=_region_malloc(rgn,sizeof(*_tmp950));_tmp950->hd=({
struct _tuple20*_tmp951=_region_malloc(rgn,sizeof(*_tmp951));_tmp951->f1=(*((
struct _tuple18*)_tmp94C->hd)).f1;_tmp951->f2=_tmp93D;_tmp951;});_tmp950->tl=ans;
_tmp950;});break;}}if(_tmp94C == 0)({struct Cyc_String_pa_struct _tmp954;_tmp954.tag=
0;_tmp954.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp949);{void*
_tmp952[1]={& _tmp954};Cyc_Tcutil_terr(loc,({const char*_tmp953="bad field designator %s";
_tag_dynforward(_tmp953,sizeof(char),_get_zero_arr_size(_tmp953,24));}),
_tag_dynforward(_tmp952,sizeof(void*),1));}});goto _LL5CA;}_LL5CA:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)fields->hd)).f2){({void*
_tmp955[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp956="too few arguments to struct";
_tag_dynforward(_tmp956,sizeof(char),_get_zero_arr_size(_tmp956,28));}),
_tag_dynforward(_tmp955,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only){*forward_only=0;{void*_tmp957=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp958;void*_tmp959;struct Cyc_Absyn_PtrAtts _tmp95A;
struct Cyc_Absyn_Conref*_tmp95B;_LL5D0: if(_tmp957 <= (void*)4)goto _LL5D2;if(*((int*)
_tmp957)!= 4)goto _LL5D2;_tmp958=((struct Cyc_Absyn_PointerType_struct*)_tmp957)->f1;
_tmp959=(void*)_tmp958.elt_typ;_tmp95A=_tmp958.ptr_atts;_tmp95B=_tmp95A.bounds;
_LL5D1: {struct Cyc_Absyn_Conref*_tmp95C=Cyc_Absyn_compress_conref(_tmp95B);void*
_tmp95D=(void*)(Cyc_Absyn_compress_conref(_tmp95C))->v;void*_tmp95E;void*_tmp95F;
_LL5D5: if(_tmp95D <= (void*)1)goto _LL5D9;if(*((int*)_tmp95D)!= 0)goto _LL5D7;
_tmp95E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp95D)->f1;if((int)_tmp95E
!= 0)goto _LL5D7;_LL5D6:*forward_only=1;*elt_typ_dest=_tmp959;return 1;_LL5D7: if(*((
int*)_tmp95D)!= 0)goto _LL5D9;_tmp95F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp95D)->f1;if((int)_tmp95F != 1)goto _LL5D9;_LL5D8:*elt_typ_dest=_tmp959;return 1;
_LL5D9: if((int)_tmp95D != 0)goto _LL5DB;_LL5DA:(void*)(_tmp95C->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp960=_cycalloc(sizeof(*_tmp960));_tmp960[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp961;_tmp961.tag=0;_tmp961.f1=(void*)((void*)
0);_tmp961;});_tmp960;})));*forward_only=1;*elt_typ_dest=_tmp959;return 1;_LL5DB:;
_LL5DC: return 0;_LL5D4:;}_LL5D2:;_LL5D3: return 0;_LL5CF:;}}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp962=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp963;void*_tmp964;struct Cyc_Absyn_PtrAtts _tmp965;struct Cyc_Absyn_Conref*
_tmp966;_LL5DE: if(_tmp962 <= (void*)4)goto _LL5E0;if(*((int*)_tmp962)!= 4)goto
_LL5E0;_tmp963=((struct Cyc_Absyn_PointerType_struct*)_tmp962)->f1;_tmp964=(void*)
_tmp963.elt_typ;_tmp965=_tmp963.ptr_atts;_tmp966=_tmp965.zero_term;_LL5DF:*
elt_typ_dest=_tmp964;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp966);_LL5E0:;_LL5E1: return 0;_LL5DD:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t,int*is_forward_only){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore,is_forward_only);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp9AA;
_tmp9AA.f1=0;_tmp9AA.f2=(void*)2;_tmp9AA;});void*_tmp967=(void*)e->r;struct
_tuple1*_tmp968;void*_tmp969;struct Cyc_Absyn_Exp*_tmp96A;struct _dynforward_ptr*
_tmp96B;struct Cyc_Absyn_Exp*_tmp96C;struct _dynforward_ptr*_tmp96D;struct Cyc_Absyn_Exp*
_tmp96E;struct Cyc_Absyn_Exp*_tmp96F;struct Cyc_Absyn_Exp*_tmp970;_LL5E3: if(*((int*)
_tmp967)!= 1)goto _LL5E5;_tmp968=((struct Cyc_Absyn_Var_e_struct*)_tmp967)->f1;
_tmp969=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp967)->f2;_LL5E4: {void*
_tmp971=_tmp969;struct Cyc_Absyn_Vardecl*_tmp972;struct Cyc_Absyn_Vardecl*_tmp973;
struct Cyc_Absyn_Vardecl*_tmp974;struct Cyc_Absyn_Vardecl*_tmp975;_LL5F0: if((int)
_tmp971 != 0)goto _LL5F2;_LL5F1: goto _LL5F3;_LL5F2: if(_tmp971 <= (void*)1)goto _LL5F4;
if(*((int*)_tmp971)!= 1)goto _LL5F4;_LL5F3: return bogus_ans;_LL5F4: if(_tmp971 <= (
void*)1)goto _LL5F6;if(*((int*)_tmp971)!= 0)goto _LL5F6;_tmp972=((struct Cyc_Absyn_Global_b_struct*)
_tmp971)->f1;_LL5F5: {void*_tmp976=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL5FD: if(_tmp976 <= (void*)4)goto _LL5FF;if(*((int*)
_tmp976)!= 7)goto _LL5FF;_LL5FE: return({struct _tuple10 _tmp977;_tmp977.f1=1;
_tmp977.f2=(void*)2;_tmp977;});_LL5FF:;_LL600: return({struct _tuple10 _tmp978;
_tmp978.f1=(_tmp972->tq).real_const;_tmp978.f2=(void*)2;_tmp978;});_LL5FC:;}
_LL5F6: if(_tmp971 <= (void*)1)goto _LL5F8;if(*((int*)_tmp971)!= 3)goto _LL5F8;
_tmp973=((struct Cyc_Absyn_Local_b_struct*)_tmp971)->f1;_LL5F7: {void*_tmp979=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL602: if(_tmp979 <= (void*)
4)goto _LL604;if(*((int*)_tmp979)!= 7)goto _LL604;_LL603: return({struct _tuple10
_tmp97A;_tmp97A.f1=1;_tmp97A.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp973->rgn))->v;
_tmp97A;});_LL604:;_LL605: _tmp973->escapes=1;return({struct _tuple10 _tmp97B;
_tmp97B.f1=(_tmp973->tq).real_const;_tmp97B.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp973->rgn))->v;_tmp97B;});_LL601:;}_LL5F8: if(_tmp971 <= (void*)1)
goto _LL5FA;if(*((int*)_tmp971)!= 4)goto _LL5FA;_tmp974=((struct Cyc_Absyn_Pat_b_struct*)
_tmp971)->f1;_LL5F9: _tmp975=_tmp974;goto _LL5FB;_LL5FA: if(_tmp971 <= (void*)1)goto
_LL5EF;if(*((int*)_tmp971)!= 2)goto _LL5EF;_tmp975=((struct Cyc_Absyn_Param_b_struct*)
_tmp971)->f1;_LL5FB: _tmp975->escapes=1;return({struct _tuple10 _tmp97C;_tmp97C.f1=(
_tmp975->tq).real_const;_tmp97C.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp975->rgn))->v;_tmp97C;});_LL5EF:;}_LL5E5: if(*((int*)_tmp967)!= 23)goto _LL5E7;
_tmp96A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp967)->f1;_tmp96B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp967)->f2;_LL5E6: {void*_tmp97D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp96A->topt))->v);struct Cyc_List_List*_tmp97E;struct Cyc_Absyn_AggrInfo
_tmp97F;void*_tmp980;struct Cyc_Absyn_Aggrdecl**_tmp981;struct Cyc_Absyn_Aggrdecl*
_tmp982;_LL607: if(_tmp97D <= (void*)4)goto _LL60B;if(*((int*)_tmp97D)!= 11)goto
_LL609;_tmp97E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp97D)->f2;_LL608: {
struct Cyc_Absyn_Aggrfield*_tmp983=Cyc_Absyn_lookup_field(_tmp97E,_tmp96B);if(
_tmp983 != 0  && _tmp983->width != 0)return({struct _tuple10 _tmp984;_tmp984.f1=(
_tmp983->tq).real_const;_tmp984.f2=(Cyc_Tcutil_addressof_props(te,_tmp96A)).f2;
_tmp984;});return bogus_ans;}_LL609: if(*((int*)_tmp97D)!= 10)goto _LL60B;_tmp97F=((
struct Cyc_Absyn_AggrType_struct*)_tmp97D)->f1;_tmp980=(void*)_tmp97F.aggr_info;
if(*((int*)_tmp980)!= 1)goto _LL60B;_tmp981=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp980)->f1;_tmp982=*_tmp981;_LL60A: {struct Cyc_Absyn_Aggrfield*_tmp985=Cyc_Absyn_lookup_decl_field(
_tmp982,_tmp96B);if(_tmp985 != 0  && _tmp985->width != 0)return({struct _tuple10
_tmp986;_tmp986.f1=(_tmp985->tq).real_const;_tmp986.f2=(Cyc_Tcutil_addressof_props(
te,_tmp96A)).f2;_tmp986;});return bogus_ans;}_LL60B:;_LL60C: return bogus_ans;
_LL606:;}_LL5E7: if(*((int*)_tmp967)!= 24)goto _LL5E9;_tmp96C=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp967)->f1;_tmp96D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp967)->f2;_LL5E8: {
void*_tmp987=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp96C->topt))->v);struct Cyc_Absyn_PtrInfo _tmp988;void*_tmp989;struct Cyc_Absyn_PtrAtts
_tmp98A;void*_tmp98B;_LL60E: if(_tmp987 <= (void*)4)goto _LL610;if(*((int*)_tmp987)
!= 4)goto _LL610;_tmp988=((struct Cyc_Absyn_PointerType_struct*)_tmp987)->f1;
_tmp989=(void*)_tmp988.elt_typ;_tmp98A=_tmp988.ptr_atts;_tmp98B=(void*)_tmp98A.rgn;
_LL60F: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp98C=Cyc_Tcutil_compress(
_tmp989);struct Cyc_List_List*_tmp98D;struct Cyc_Absyn_AggrInfo _tmp98E;void*
_tmp98F;struct Cyc_Absyn_Aggrdecl**_tmp990;struct Cyc_Absyn_Aggrdecl*_tmp991;
_LL613: if(_tmp98C <= (void*)4)goto _LL617;if(*((int*)_tmp98C)!= 11)goto _LL615;
_tmp98D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp98C)->f2;_LL614: finfo=Cyc_Absyn_lookup_field(
_tmp98D,_tmp96D);goto _LL612;_LL615: if(*((int*)_tmp98C)!= 10)goto _LL617;_tmp98E=((
struct Cyc_Absyn_AggrType_struct*)_tmp98C)->f1;_tmp98F=(void*)_tmp98E.aggr_info;
if(*((int*)_tmp98F)!= 1)goto _LL617;_tmp990=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp98F)->f1;_tmp991=*_tmp990;_LL616: finfo=Cyc_Absyn_lookup_decl_field(_tmp991,
_tmp96D);goto _LL612;_LL617:;_LL618: return bogus_ans;_LL612:;}if(finfo != 0  && 
finfo->width != 0)return({struct _tuple10 _tmp992;_tmp992.f1=(finfo->tq).real_const;
_tmp992.f2=_tmp98B;_tmp992;});return bogus_ans;}_LL610:;_LL611: return bogus_ans;
_LL60D:;}_LL5E9: if(*((int*)_tmp967)!= 22)goto _LL5EB;_tmp96E=((struct Cyc_Absyn_Deref_e_struct*)
_tmp967)->f1;_LL5EA: {void*_tmp993=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp96E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp994;struct Cyc_Absyn_Tqual
_tmp995;struct Cyc_Absyn_PtrAtts _tmp996;void*_tmp997;_LL61A: if(_tmp993 <= (void*)4)
goto _LL61C;if(*((int*)_tmp993)!= 4)goto _LL61C;_tmp994=((struct Cyc_Absyn_PointerType_struct*)
_tmp993)->f1;_tmp995=_tmp994.elt_tq;_tmp996=_tmp994.ptr_atts;_tmp997=(void*)
_tmp996.rgn;_LL61B: return({struct _tuple10 _tmp998;_tmp998.f1=_tmp995.real_const;
_tmp998.f2=_tmp997;_tmp998;});_LL61C:;_LL61D: return bogus_ans;_LL619:;}_LL5EB: if(*((
int*)_tmp967)!= 25)goto _LL5ED;_tmp96F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp967)->f1;_tmp970=((struct Cyc_Absyn_Subscript_e_struct*)_tmp967)->f2;_LL5EC: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp96F->topt))->v);
void*_tmp999=t;struct Cyc_List_List*_tmp99A;struct Cyc_Absyn_PtrInfo _tmp99B;struct
Cyc_Absyn_Tqual _tmp99C;struct Cyc_Absyn_PtrAtts _tmp99D;void*_tmp99E;struct Cyc_Absyn_ArrayInfo
_tmp99F;struct Cyc_Absyn_Tqual _tmp9A0;_LL61F: if(_tmp999 <= (void*)4)goto _LL625;if(*((
int*)_tmp999)!= 9)goto _LL621;_tmp99A=((struct Cyc_Absyn_TupleType_struct*)_tmp999)->f1;
_LL620: {unsigned int _tmp9A2;int _tmp9A3;struct _tuple7 _tmp9A1=Cyc_Evexp_eval_const_uint_exp(
_tmp970);_tmp9A2=_tmp9A1.f1;_tmp9A3=_tmp9A1.f2;if(!_tmp9A3)return bogus_ans;{
struct _tuple4*_tmp9A4=Cyc_Absyn_lookup_tuple_field(_tmp99A,(int)_tmp9A2);if(
_tmp9A4 != 0)return({struct _tuple10 _tmp9A5;_tmp9A5.f1=((*_tmp9A4).f1).real_const;
_tmp9A5.f2=(Cyc_Tcutil_addressof_props(te,_tmp96F)).f2;_tmp9A5;});return
bogus_ans;}}_LL621: if(*((int*)_tmp999)!= 4)goto _LL623;_tmp99B=((struct Cyc_Absyn_PointerType_struct*)
_tmp999)->f1;_tmp99C=_tmp99B.elt_tq;_tmp99D=_tmp99B.ptr_atts;_tmp99E=(void*)
_tmp99D.rgn;_LL622: return({struct _tuple10 _tmp9A6;_tmp9A6.f1=_tmp99C.real_const;
_tmp9A6.f2=_tmp99E;_tmp9A6;});_LL623: if(*((int*)_tmp999)!= 7)goto _LL625;_tmp99F=((
struct Cyc_Absyn_ArrayType_struct*)_tmp999)->f1;_tmp9A0=_tmp99F.tq;_LL624: return({
struct _tuple10 _tmp9A7;_tmp9A7.f1=_tmp9A0.real_const;_tmp9A7.f2=(Cyc_Tcutil_addressof_props(
te,_tmp96F)).f2;_tmp9A7;});_LL625:;_LL626: return bogus_ans;_LL61E:;}_LL5ED:;
_LL5EE:({void*_tmp9A8[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp9A9="unary & applied to non-lvalue";
_tag_dynforward(_tmp9A9,sizeof(char),_get_zero_arr_size(_tmp9A9,30));}),
_tag_dynforward(_tmp9A8,sizeof(void*),0));});return bogus_ans;_LL5E2:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp9AB=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmp9AC;void*_tmp9AD;struct Cyc_Absyn_Tqual
_tmp9AE;struct Cyc_Absyn_Conref*_tmp9AF;_LL628: if(_tmp9AB <= (void*)4)goto _LL62A;
if(*((int*)_tmp9AB)!= 7)goto _LL62A;_tmp9AC=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9AB)->f1;_tmp9AD=(void*)_tmp9AC.elt_type;_tmp9AE=_tmp9AC.tq;_tmp9AF=_tmp9AC.zero_term;
_LL629: {void*_tmp9B1;struct _tuple10 _tmp9B0=Cyc_Tcutil_addressof_props(te,e);
_tmp9B1=_tmp9B0.f2;return Cyc_Absyn_atb_typ(_tmp9AD,_tmp9B1,_tmp9AE,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp9B2=_cycalloc(sizeof(*_tmp9B2));_tmp9B2[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp9B3;_tmp9B3.tag=0;_tmp9B3.f1=e;_tmp9B3;});
_tmp9B2;}),_tmp9AF);}_LL62A:;_LL62B: return e_typ;_LL627:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp9B4=(void*)b->v;void*_tmp9B5;void*_tmp9B6;void*_tmp9B7;struct Cyc_Absyn_Exp*
_tmp9B8;void*_tmp9B9;_LL62D: if(_tmp9B4 <= (void*)1)goto _LL635;if(*((int*)_tmp9B4)
!= 0)goto _LL62F;_tmp9B5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9B4)->f1;
if((int)_tmp9B5 != 1)goto _LL62F;_LL62E: return;_LL62F: if(*((int*)_tmp9B4)!= 0)goto
_LL631;_tmp9B6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9B4)->f1;if((int)
_tmp9B6 != 0)goto _LL631;_LL630: if(i < 0)({void*_tmp9BA[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp9BB="dereference is out of bounds";_tag_dynforward(_tmp9BB,sizeof(
char),_get_zero_arr_size(_tmp9BB,29));}),_tag_dynforward(_tmp9BA,sizeof(void*),0));});
return;_LL631: if(*((int*)_tmp9B4)!= 0)goto _LL633;_tmp9B7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9B4)->f1;if(_tmp9B7 <= (void*)2)goto _LL633;if(*((int*)_tmp9B7)!= 0)goto _LL633;
_tmp9B8=((struct Cyc_Absyn_Upper_b_struct*)_tmp9B7)->f1;_LL632: {unsigned int
_tmp9BD;int _tmp9BE;struct _tuple7 _tmp9BC=Cyc_Evexp_eval_const_uint_exp(_tmp9B8);
_tmp9BD=_tmp9BC.f1;_tmp9BE=_tmp9BC.f2;if(_tmp9BE  && _tmp9BD <= i)({struct Cyc_Int_pa_struct
_tmp9C2;_tmp9C2.tag=1;_tmp9C2.f1=(unsigned long)((int)i);{struct Cyc_Int_pa_struct
_tmp9C1;_tmp9C1.tag=1;_tmp9C1.f1=(unsigned long)((int)_tmp9BD);{void*_tmp9BF[2]={&
_tmp9C1,& _tmp9C2};Cyc_Tcutil_terr(loc,({const char*_tmp9C0="dereference is out of bounds: %d <= %d";
_tag_dynforward(_tmp9C0,sizeof(char),_get_zero_arr_size(_tmp9C0,39));}),
_tag_dynforward(_tmp9BF,sizeof(void*),2));}}});return;}_LL633: if(*((int*)_tmp9B4)
!= 0)goto _LL635;_tmp9B9=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9B4)->f1;
if(_tmp9B9 <= (void*)2)goto _LL635;if(*((int*)_tmp9B9)!= 1)goto _LL635;_LL634:
return;_LL635: if((int)_tmp9B4 != 0)goto _LL637;_LL636:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp9C3=_cycalloc(sizeof(*_tmp9C3));_tmp9C3[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp9C4;_tmp9C4.tag=0;_tmp9C4.f1=(void*)((void*)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp9C5=_cycalloc(sizeof(*_tmp9C5));
_tmp9C5[0]=({struct Cyc_Absyn_Upper_b_struct _tmp9C6;_tmp9C6.tag=0;_tmp9C6.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp9C6;});_tmp9C5;})));_tmp9C4;});_tmp9C3;})));return;_LL637:;_LL638:({
void*_tmp9C7[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp9C8="check_bound -- bad compressed conref";
_tag_dynforward(_tmp9C8,sizeof(char),_get_zero_arr_size(_tmp9C8,37));}),
_tag_dynforward(_tmp9C7,sizeof(void*),0));});_LL62C:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){void*_tmp9C9=(
void*)(Cyc_Absyn_compress_conref(b))->v;void*_tmp9CA;struct Cyc_Absyn_Exp*_tmp9CB;
_LL63A: if(_tmp9C9 <= (void*)1)goto _LL63C;if(*((int*)_tmp9C9)!= 0)goto _LL63C;
_tmp9CA=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9C9)->f1;if(_tmp9CA <= (
void*)2)goto _LL63C;if(*((int*)_tmp9CA)!= 0)goto _LL63C;_tmp9CB=((struct Cyc_Absyn_Upper_b_struct*)
_tmp9CA)->f1;_LL63B: {unsigned int _tmp9CD;int _tmp9CE;struct _tuple7 _tmp9CC=Cyc_Evexp_eval_const_uint_exp(
_tmp9CB);_tmp9CD=_tmp9CC.f1;_tmp9CE=_tmp9CC.f2;return _tmp9CE  && _tmp9CD == 1;}
_LL63C:;_LL63D: return 0;_LL639:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp9CF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp9D0;void*_tmp9D1;struct Cyc_Absyn_Conref*_tmp9D2;
struct Cyc_List_List*_tmp9D3;struct Cyc_Absyn_AggrInfo _tmp9D4;void*_tmp9D5;struct
Cyc_Absyn_AggrInfo _tmp9D6;void*_tmp9D7;struct Cyc_Absyn_Aggrdecl**_tmp9D8;struct
Cyc_Absyn_Aggrdecl*_tmp9D9;struct Cyc_List_List*_tmp9DA;struct Cyc_List_List*
_tmp9DB;_LL63F: if((int)_tmp9CF != 0)goto _LL641;_LL640: goto _LL642;_LL641: if(
_tmp9CF <= (void*)4)goto _LL643;if(*((int*)_tmp9CF)!= 5)goto _LL643;_LL642: goto
_LL644;_LL643: if((int)_tmp9CF != 1)goto _LL645;_LL644: goto _LL646;_LL645: if(_tmp9CF
<= (void*)4)goto _LL655;if(*((int*)_tmp9CF)!= 6)goto _LL647;_LL646: return 1;_LL647:
if(*((int*)_tmp9CF)!= 12)goto _LL649;_LL648: goto _LL64A;_LL649: if(*((int*)_tmp9CF)
!= 13)goto _LL64B;_LL64A: return 0;_LL64B: if(*((int*)_tmp9CF)!= 7)goto _LL64D;
_tmp9D0=((struct Cyc_Absyn_ArrayType_struct*)_tmp9CF)->f1;_tmp9D1=(void*)_tmp9D0.elt_type;
_tmp9D2=_tmp9D0.zero_term;_LL64C: return !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp9D2) && Cyc_Tcutil_bits_only(_tmp9D1);_LL64D: if(*((
int*)_tmp9CF)!= 9)goto _LL64F;_tmp9D3=((struct Cyc_Absyn_TupleType_struct*)_tmp9CF)->f1;
_LL64E: for(0;_tmp9D3 != 0;_tmp9D3=_tmp9D3->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp9D3->hd)).f2))return 0;}return 1;_LL64F: if(*((int*)_tmp9CF)!= 
10)goto _LL651;_tmp9D4=((struct Cyc_Absyn_AggrType_struct*)_tmp9CF)->f1;_tmp9D5=(
void*)_tmp9D4.aggr_info;if(*((int*)_tmp9D5)!= 0)goto _LL651;_LL650: return 0;_LL651:
if(*((int*)_tmp9CF)!= 10)goto _LL653;_tmp9D6=((struct Cyc_Absyn_AggrType_struct*)
_tmp9CF)->f1;_tmp9D7=(void*)_tmp9D6.aggr_info;if(*((int*)_tmp9D7)!= 1)goto _LL653;
_tmp9D8=((struct Cyc_Absyn_KnownAggr_struct*)_tmp9D7)->f1;_tmp9D9=*_tmp9D8;
_tmp9DA=_tmp9D6.targs;_LL652: if(_tmp9D9->impl == 0)return 0;{struct _RegionHandle
_tmp9DC=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9DC;_push_region(rgn);{
struct Cyc_List_List*_tmp9DD=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp9D9->tvs,_tmp9DA);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp9D9->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute(rgn,_tmp9DD,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp9DE=0;_npop_handler(0);return _tmp9DE;}}}{int _tmp9DF=1;_npop_handler(0);
return _tmp9DF;}};_pop_region(rgn);}_LL653: if(*((int*)_tmp9CF)!= 11)goto _LL655;
_tmp9DB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9CF)->f2;_LL654: for(0;_tmp9DB
!= 0;_tmp9DB=_tmp9DB->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp9DB->hd)->type))return 0;}return 1;_LL655:;_LL656: return 0;_LL63E:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp9E0=(void*)e->r;
struct _tuple1*_tmp9E1;void*_tmp9E2;struct Cyc_Absyn_Exp*_tmp9E3;struct Cyc_Absyn_Exp*
_tmp9E4;struct Cyc_Absyn_Exp*_tmp9E5;struct Cyc_Absyn_Exp*_tmp9E6;struct Cyc_Absyn_Exp*
_tmp9E7;struct Cyc_Absyn_Exp*_tmp9E8;struct Cyc_Absyn_Exp*_tmp9E9;void*_tmp9EA;
struct Cyc_Absyn_Exp*_tmp9EB;void*_tmp9EC;void*_tmp9ED;struct Cyc_Absyn_Exp*
_tmp9EE;struct Cyc_Absyn_Exp*_tmp9EF;struct Cyc_Absyn_Exp*_tmp9F0;struct Cyc_Absyn_Exp*
_tmp9F1;struct Cyc_List_List*_tmp9F2;struct Cyc_List_List*_tmp9F3;struct Cyc_List_List*
_tmp9F4;void*_tmp9F5;struct Cyc_List_List*_tmp9F6;struct Cyc_List_List*_tmp9F7;
struct Cyc_List_List*_tmp9F8;_LL658: if(*((int*)_tmp9E0)!= 0)goto _LL65A;_LL659:
goto _LL65B;_LL65A: if(*((int*)_tmp9E0)!= 18)goto _LL65C;_LL65B: goto _LL65D;_LL65C:
if(*((int*)_tmp9E0)!= 19)goto _LL65E;_LL65D: goto _LL65F;_LL65E: if(*((int*)_tmp9E0)
!= 20)goto _LL660;_LL65F: goto _LL661;_LL660: if(*((int*)_tmp9E0)!= 21)goto _LL662;
_LL661: goto _LL663;_LL662: if(*((int*)_tmp9E0)!= 33)goto _LL664;_LL663: goto _LL665;
_LL664: if(*((int*)_tmp9E0)!= 34)goto _LL666;_LL665: return 1;_LL666: if(*((int*)
_tmp9E0)!= 1)goto _LL668;_tmp9E1=((struct Cyc_Absyn_Var_e_struct*)_tmp9E0)->f1;
_tmp9E2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9E0)->f2;_LL667: {void*
_tmp9F9=_tmp9E2;struct Cyc_Absyn_Vardecl*_tmp9FA;_LL687: if(_tmp9F9 <= (void*)1)
goto _LL68B;if(*((int*)_tmp9F9)!= 1)goto _LL689;_LL688: return 1;_LL689: if(*((int*)
_tmp9F9)!= 0)goto _LL68B;_tmp9FA=((struct Cyc_Absyn_Global_b_struct*)_tmp9F9)->f1;
_LL68A: {void*_tmp9FB=Cyc_Tcutil_compress((void*)_tmp9FA->type);_LL690: if(
_tmp9FB <= (void*)4)goto _LL694;if(*((int*)_tmp9FB)!= 7)goto _LL692;_LL691: goto
_LL693;_LL692: if(*((int*)_tmp9FB)!= 8)goto _LL694;_LL693: return 1;_LL694:;_LL695:
return var_okay;_LL68F:;}_LL68B: if((int)_tmp9F9 != 0)goto _LL68D;_LL68C: return 0;
_LL68D:;_LL68E: return var_okay;_LL686:;}_LL668: if(*((int*)_tmp9E0)!= 6)goto _LL66A;
_tmp9E3=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9E0)->f1;_tmp9E4=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp9E0)->f2;_tmp9E5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp9E0)->f3;_LL669:
return(Cyc_Tcutil_cnst_exp(te,0,_tmp9E3) && Cyc_Tcutil_cnst_exp(te,0,_tmp9E4))
 && Cyc_Tcutil_cnst_exp(te,0,_tmp9E5);_LL66A: if(*((int*)_tmp9E0)!= 9)goto _LL66C;
_tmp9E6=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp9E0)->f1;_tmp9E7=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp9E0)->f2;_LL66B: return Cyc_Tcutil_cnst_exp(te,0,_tmp9E6) && Cyc_Tcutil_cnst_exp(
te,0,_tmp9E7);_LL66C: if(*((int*)_tmp9E0)!= 13)goto _LL66E;_tmp9E8=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp9E0)->f1;_LL66D: _tmp9E9=_tmp9E8;goto _LL66F;_LL66E: if(*((int*)_tmp9E0)!= 14)
goto _LL670;_tmp9E9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp9E0)->f1;_LL66F:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9E9);_LL670: if(*((int*)_tmp9E0)!= 15)
goto _LL672;_tmp9EA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp9E0)->f1;_tmp9EB=((
struct Cyc_Absyn_Cast_e_struct*)_tmp9E0)->f2;_tmp9EC=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp9E0)->f4;if((int)_tmp9EC != 1)goto _LL672;_LL671: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmp9EB);_LL672: if(*((int*)_tmp9E0)!= 15)goto _LL674;_tmp9ED=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp9E0)->f1;_tmp9EE=((struct Cyc_Absyn_Cast_e_struct*)
_tmp9E0)->f2;_LL673: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9EE);_LL674: if(*((
int*)_tmp9E0)!= 16)goto _LL676;_tmp9EF=((struct Cyc_Absyn_Address_e_struct*)
_tmp9E0)->f1;_LL675: return Cyc_Tcutil_cnst_exp(te,1,_tmp9EF);_LL676: if(*((int*)
_tmp9E0)!= 29)goto _LL678;_tmp9F0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp9E0)->f2;_tmp9F1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp9E0)->f3;
_LL677: return Cyc_Tcutil_cnst_exp(te,0,_tmp9F0) && Cyc_Tcutil_cnst_exp(te,0,
_tmp9F1);_LL678: if(*((int*)_tmp9E0)!= 28)goto _LL67A;_tmp9F2=((struct Cyc_Absyn_Array_e_struct*)
_tmp9E0)->f1;_LL679: _tmp9F3=_tmp9F2;goto _LL67B;_LL67A: if(*((int*)_tmp9E0)!= 31)
goto _LL67C;_tmp9F3=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp9E0)->f2;_LL67B:
_tmp9F4=_tmp9F3;goto _LL67D;_LL67C: if(*((int*)_tmp9E0)!= 30)goto _LL67E;_tmp9F4=((
struct Cyc_Absyn_Struct_e_struct*)_tmp9E0)->f3;_LL67D: for(0;_tmp9F4 != 0;_tmp9F4=
_tmp9F4->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp9F4->hd)).f2))
return 0;}return 1;_LL67E: if(*((int*)_tmp9E0)!= 3)goto _LL680;_tmp9F5=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp9E0)->f1;_tmp9F6=((struct Cyc_Absyn_Primop_e_struct*)
_tmp9E0)->f2;_LL67F: _tmp9F7=_tmp9F6;goto _LL681;_LL680: if(*((int*)_tmp9E0)!= 26)
goto _LL682;_tmp9F7=((struct Cyc_Absyn_Tuple_e_struct*)_tmp9E0)->f1;_LL681: _tmp9F8=
_tmp9F7;goto _LL683;_LL682: if(*((int*)_tmp9E0)!= 32)goto _LL684;_tmp9F8=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp9E0)->f1;_LL683: for(0;_tmp9F8 != 0;_tmp9F8=_tmp9F8->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp9F8->hd))return 0;}return 1;
_LL684:;_LL685: return 0;_LL657:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp9FC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9FD;
struct Cyc_Absyn_PtrAtts _tmp9FE;struct Cyc_Absyn_Conref*_tmp9FF;struct Cyc_Absyn_Conref*
_tmpA00;struct Cyc_Absyn_ArrayInfo _tmpA01;void*_tmpA02;struct Cyc_List_List*
_tmpA03;struct Cyc_Absyn_AggrInfo _tmpA04;void*_tmpA05;struct Cyc_List_List*_tmpA06;
struct Cyc_List_List*_tmpA07;_LL697: if((int)_tmp9FC != 0)goto _LL699;_LL698: goto
_LL69A;_LL699: if(_tmp9FC <= (void*)4)goto _LL69B;if(*((int*)_tmp9FC)!= 5)goto
_LL69B;_LL69A: goto _LL69C;_LL69B: if((int)_tmp9FC != 1)goto _LL69D;_LL69C: goto _LL69E;
_LL69D: if(_tmp9FC <= (void*)4)goto _LL6AD;if(*((int*)_tmp9FC)!= 6)goto _LL69F;
_LL69E: return 1;_LL69F: if(*((int*)_tmp9FC)!= 4)goto _LL6A1;_tmp9FD=((struct Cyc_Absyn_PointerType_struct*)
_tmp9FC)->f1;_tmp9FE=_tmp9FD.ptr_atts;_tmp9FF=_tmp9FE.nullable;_tmpA00=_tmp9FE.bounds;
_LL6A0: {void*_tmpA08=Cyc_Absyn_conref_def((void*)((void*)0),_tmpA00);_LL6B0: if((
int)_tmpA08 != 0)goto _LL6B2;_LL6B1: return 1;_LL6B2: if((int)_tmpA08 != 1)goto _LL6B4;
_LL6B3: return 1;_LL6B4:;_LL6B5: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp9FF);_LL6AF:;}_LL6A1: if(*((int*)_tmp9FC)!= 7)goto _LL6A3;_tmpA01=((struct
Cyc_Absyn_ArrayType_struct*)_tmp9FC)->f1;_tmpA02=(void*)_tmpA01.elt_type;_LL6A2:
return Cyc_Tcutil_supports_default(_tmpA02);_LL6A3: if(*((int*)_tmp9FC)!= 9)goto
_LL6A5;_tmpA03=((struct Cyc_Absyn_TupleType_struct*)_tmp9FC)->f1;_LL6A4: for(0;
_tmpA03 != 0;_tmpA03=_tmpA03->tl){if(!Cyc_Tcutil_supports_default((*((struct
_tuple4*)_tmpA03->hd)).f2))return 0;}return 1;_LL6A5: if(*((int*)_tmp9FC)!= 10)goto
_LL6A7;_tmpA04=((struct Cyc_Absyn_AggrType_struct*)_tmp9FC)->f1;_tmpA05=(void*)
_tmpA04.aggr_info;_tmpA06=_tmpA04.targs;_LL6A6: {struct Cyc_Absyn_Aggrdecl*
_tmpA09=Cyc_Absyn_get_known_aggrdecl(_tmpA05);if(_tmpA09->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA09->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpA09->tvs,_tmpA06,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpA09->impl))->fields);}_LL6A7: if(*((int*)_tmp9FC)!= 11)goto _LL6A9;
_tmpA07=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9FC)->f2;_LL6A8: return Cyc_Tcutil_fields_support_default(
0,0,_tmpA07);_LL6A9: if(*((int*)_tmp9FC)!= 13)goto _LL6AB;_LL6AA: goto _LL6AC;_LL6AB:
if(*((int*)_tmp9FC)!= 12)goto _LL6AD;_LL6AC: return 1;_LL6AD:;_LL6AE: return 0;_LL696:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmpA0A=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpA0A;_push_region(rgn);{struct Cyc_List_List*_tmpA0B=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpA0B,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpA0C=0;_npop_handler(0);return _tmpA0C;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){void*_tmpA0D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA0E;struct Cyc_Absyn_PtrAtts _tmpA0F;struct Cyc_Absyn_Conref*
_tmpA10;struct Cyc_Absyn_Conref*_tmpA11;_LL6B7: if(_tmpA0D <= (void*)4)goto _LL6B9;
if(*((int*)_tmpA0D)!= 5)goto _LL6B9;_LL6B8: goto _LL6BA;_LL6B9: if((int)_tmpA0D != 1)
goto _LL6BB;_LL6BA: goto _LL6BC;_LL6BB: if(_tmpA0D <= (void*)4)goto _LL6BF;if(*((int*)
_tmpA0D)!= 6)goto _LL6BD;_LL6BC: return 1;_LL6BD: if(*((int*)_tmpA0D)!= 4)goto _LL6BF;
_tmpA0E=((struct Cyc_Absyn_PointerType_struct*)_tmpA0D)->f1;_tmpA0F=_tmpA0E.ptr_atts;
_tmpA10=_tmpA0F.nullable;_tmpA11=_tmpA0F.bounds;_LL6BE: {void*_tmpA12=(void*)(
Cyc_Absyn_compress_conref(_tmpA11))->v;void*_tmpA13;void*_tmpA14;_LL6C2: if(
_tmpA12 <= (void*)1)goto _LL6C8;if(*((int*)_tmpA12)!= 0)goto _LL6C4;_tmpA13=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpA12)->f1;if((int)_tmpA13 != 0)goto _LL6C4;
_LL6C3: return 0;_LL6C4: if(*((int*)_tmpA12)!= 0)goto _LL6C6;_tmpA14=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmpA12)->f1;if((int)_tmpA14 != 1)goto _LL6C6;_LL6C5:
return 0;_LL6C6: if(*((int*)_tmpA12)!= 0)goto _LL6C8;_LL6C7: {void*_tmpA15=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpA10))->v;int _tmpA16;_LL6CB: if(_tmpA15 <= (void*)1)goto _LL6CD;if(*((int*)
_tmpA15)!= 0)goto _LL6CD;_tmpA16=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmpA15)->f1;
_LL6CC: return _tmpA16;_LL6CD:;_LL6CE: return 0;_LL6CA:;}_LL6C8:;_LL6C9: return 0;
_LL6C1:;}_LL6BF:;_LL6C0: return 0;_LL6B6:;}int Cyc_Tcutil_is_noreturn(void*t){{void*
_tmpA17=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA18;void*_tmpA19;
struct Cyc_Absyn_FnInfo _tmpA1A;struct Cyc_List_List*_tmpA1B;_LL6D0: if(_tmpA17 <= (
void*)4)goto _LL6D4;if(*((int*)_tmpA17)!= 4)goto _LL6D2;_tmpA18=((struct Cyc_Absyn_PointerType_struct*)
_tmpA17)->f1;_tmpA19=(void*)_tmpA18.elt_typ;_LL6D1: return Cyc_Tcutil_is_noreturn(
_tmpA19);_LL6D2: if(*((int*)_tmpA17)!= 8)goto _LL6D4;_tmpA1A=((struct Cyc_Absyn_FnType_struct*)
_tmpA17)->f1;_tmpA1B=_tmpA1A.attributes;_LL6D3: for(0;_tmpA1B != 0;_tmpA1B=_tmpA1B->tl){
void*_tmpA1C=(void*)_tmpA1B->hd;_LL6D7: if((int)_tmpA1C != 3)goto _LL6D9;_LL6D8:
return 1;_LL6D9:;_LL6DA: continue;_LL6D6:;}goto _LL6CF;_LL6D4:;_LL6D5: goto _LL6CF;
_LL6CF:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,
struct Cyc_List_List*atts){void*_tmpA1D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpA1E;struct Cyc_List_List*_tmpA1F;struct Cyc_List_List**_tmpA20;_LL6DC: if(
_tmpA1D <= (void*)4)goto _LL6DE;if(*((int*)_tmpA1D)!= 8)goto _LL6DE;_tmpA1E=((
struct Cyc_Absyn_FnType_struct*)_tmpA1D)->f1;_tmpA1F=_tmpA1E.attributes;_tmpA20=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmpA1D)->f1).attributes;
_LL6DD: {struct Cyc_List_List*_tmpA21=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd))*_tmpA20=({struct Cyc_List_List*_tmpA22=_cycalloc(sizeof(*_tmpA22));
_tmpA22->hd=(void*)((void*)atts->hd);_tmpA22->tl=*_tmpA20;_tmpA22;});else{
_tmpA21=({struct Cyc_List_List*_tmpA23=_cycalloc(sizeof(*_tmpA23));_tmpA23->hd=(
void*)((void*)atts->hd);_tmpA23->tl=_tmpA21;_tmpA23;});}}return _tmpA21;}_LL6DE:;
_LL6DF:({void*_tmpA24[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA25="transfer_fn_type_atts";
_tag_dynforward(_tmpA25,sizeof(char),_get_zero_arr_size(_tmpA25,22));}),
_tag_dynforward(_tmpA24,sizeof(void*),0));});_LL6DB:;}
