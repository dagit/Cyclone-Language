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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
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
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
struct _tuple1{void*f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;struct
Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
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
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
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
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
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
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Dict_Dict*ae;struct Cyc_List_List*le;};void*Cyc_Tcutil_impos(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_unify(void*,void*);
int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_Tcutil_snd_tqt(
struct _tuple4*);struct _tuple5{unsigned int f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict*consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
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
_dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp6[14]="_handler_cons";
static struct _dynforward_ptr Cyc_Toc__handler_cons_string={_tmp6,_tmp6 + 14};static
struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp7[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp7,_tmp7 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp8[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp8,_tmp8 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp9[7]="_throw";static struct _dynforward_ptr Cyc_Toc__throw_str={_tmp9,
_tmp9 + 7};static struct _tuple1 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpB[7]="setjmp";static struct
_dynforward_ptr Cyc_Toc_setjmp_str={_tmpB,_tmpB + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmpD[14]="_push_handler";
static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmpD,_tmpD + 14};static
struct _tuple1 Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char
_tmpF[13]="_pop_handler";static struct _dynforward_ptr Cyc_Toc__pop_handler_str={
_tmpF,_tmpF + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp11[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp11,_tmp11 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
char _tmp13[14]="_npop_handler";static struct _dynforward_ptr Cyc_Toc__npop_handler_str={
_tmp13,_tmp13 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp15[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp15,_tmp15 + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={
0,(void*)((void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static
char _tmp17[28]="_check_known_subscript_null";static struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={
_tmp17,_tmp17 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp19[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp19,_tmp19 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp1B[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp1B,
_tmp1B + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_dynforward_subscript_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp1D[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp1D,_tmp1D + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp1F[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp1F,_tmp1F + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_re={
1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=& Cyc_Toc__dynforward_ptr_ev;
static char _tmp21[15]="_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_str={
_tmp21,_tmp21 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp23[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp23,_tmp23 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dynforward_re={
1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=& Cyc_Toc__tag_dynforward_ev;
static char _tmp25[15]="_tag_dyneither";static struct _dynforward_ptr Cyc_Toc__tag_dyneither_str={
_tmp25,_tmp25 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp27[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp27,_tmp27 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(void*)((
void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp29[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp29,_tmp29 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)
0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)((void*)&
Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp2B[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp2B,_tmp2B + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(void*)((
void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp2D[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp2D,_tmp2D + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)((
void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp2F[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp2F,_tmp2F + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_dynforward_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(void*)((
void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp31[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp31,_tmp31 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)
0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)((void*)&
Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp33[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp33,_tmp33 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={
1,& Cyc_Toc__get_zero_arr_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp35[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp35,_tmp35 + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dynforward_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(void*)((
void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp37[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp37,_tmp37 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)
0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)((void*)&
Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp39[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp39,_tmp39 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={
1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;
static char _tmp3B[29]="_dynforward_ptr_inplace_plus";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp3B,_tmp3B + 29};static struct _tuple1
Cyc_Toc__dynforward_ptr_inplace_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_ev;static char _tmp3D[28]="_dyneither_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp3D,
_tmp3D + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dyneither_ptr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp3F[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp3F,_tmp3F + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp41[34]="_dynforward_ptr_inplace_plus_post";static struct
_dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmp41,_tmp41 + 34};
static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_inplace_plus_post_re={1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmp43[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp43,
_tmp43 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp45[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmp45,_tmp45 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_to_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={
1,& Cyc_Toc__dynforward_to_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_to_dyneither_ev={0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=&
Cyc_Toc__dynforward_to_dyneither_ev;static char _tmp47[25]="_dyneither_to_dynforward";
static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={_tmp47,_tmp47 + 
25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dyneither_to_dynforward_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={
1,& Cyc_Toc__dyneither_to_dynforward_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_to_dynforward_ev={0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=&
Cyc_Toc__dyneither_to_dynforward_ev;static char _tmp49[28]="_zero_arr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp49,
_tmp49 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp4B[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmp4B,_tmp4B + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp4D[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmp4D,_tmp4D + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char
_tmp4F[17]="_cycalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cycalloc_atomic_str={
_tmp4F,_tmp4F + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp51[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp51,_tmp51 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp53[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmp53,_tmp53 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={
1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={
0,(void*)((void*)& Cyc_Toc__region_malloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;
static char _tmp55[15]="_region_calloc";static struct _dynforward_ptr Cyc_Toc__region_calloc_str={
_tmp55,_tmp55 + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmp57[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmp57,_tmp57 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={
1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={
0,(void*)((void*)& Cyc_Toc__check_times_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static
char _tmp59[12]="_new_region";static struct _dynforward_ptr Cyc_Toc__new_region_str={
_tmp59,_tmp59 + 12};static struct _tuple1 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmp5B[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmp5B,_tmp5B + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={
1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={
0,(void*)((void*)& Cyc_Toc__push_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static
char _tmp5D[12]="_pop_region";static struct _dynforward_ptr Cyc_Toc__pop_region_str={
_tmp5D,_tmp5D + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmp5F[14]="_reset_region";static struct _dynforward_ptr Cyc_Toc__reset_region_str={
_tmp5F,_tmp5F + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={
0,(void*)((void*)& Cyc_Toc__reset_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;
static char _tmp61[19]="_throw_arraybounds";static struct _dynforward_ptr Cyc_Toc__throw_arraybounds_str={
_tmp61,_tmp61 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={
1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp63[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmp63,_tmp63 + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={
1,& Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmp65[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp65,
_tmp65 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dyneither_ptr_decrease_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;
static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_dynforward_ptr_typ_u={0,(void*)((
void*)0),& Cyc_Toc__dynforward_ptr_pr};static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={
10,{(void*)((void*)& Cyc_Toc_dynforward_ptr_typ_u),0}};static void*Cyc_Toc_dynforward_ptr_typ=(
void*)& Cyc_Toc_dynforward_ptr_typ_v;static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_dyneither_ptr_typ_u={
0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_AggrType_struct
Cyc_Toc_dyneither_ptr_typ_v={10,{(void*)((void*)& Cyc_Toc_dyneither_ptr_typ_u),0}};
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B[0]=Cyc_Absyn_skip_stmt(
0);_tmp6B;});return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp6C=(void*)e->r;void*_tmp6D;char _tmp6E;void*
_tmp6F;short _tmp70;void*_tmp71;int _tmp72;void*_tmp73;long long _tmp74;void*_tmp75;
struct Cyc_Absyn_Exp*_tmp76;struct Cyc_List_List*_tmp77;struct Cyc_List_List*_tmp78;
struct Cyc_List_List*_tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_List_List*_tmp7B;
_LL1: if(*((int*)_tmp6C)!= 0)goto _LL3;_tmp6D=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp6C)->f1;if(_tmp6D <= (void*)1)goto _LL3;if(*((int*)_tmp6D)!= 0)goto _LL3;_tmp6E=((
struct Cyc_Absyn_Char_c_struct*)_tmp6D)->f2;_LL2: return _tmp6E == '\000';_LL3: if(*((
int*)_tmp6C)!= 0)goto _LL5;_tmp6F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp6C)->f1;
if(_tmp6F <= (void*)1)goto _LL5;if(*((int*)_tmp6F)!= 1)goto _LL5;_tmp70=((struct Cyc_Absyn_Short_c_struct*)
_tmp6F)->f2;_LL4: return _tmp70 == 0;_LL5: if(*((int*)_tmp6C)!= 0)goto _LL7;_tmp71=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp6C)->f1;if(_tmp71 <= (void*)1)goto
_LL7;if(*((int*)_tmp71)!= 2)goto _LL7;_tmp72=((struct Cyc_Absyn_Int_c_struct*)
_tmp71)->f2;_LL6: return _tmp72 == 0;_LL7: if(*((int*)_tmp6C)!= 0)goto _LL9;_tmp73=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp6C)->f1;if(_tmp73 <= (void*)1)goto
_LL9;if(*((int*)_tmp73)!= 3)goto _LL9;_tmp74=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp73)->f2;_LL8: return _tmp74 == 0;_LL9: if(*((int*)_tmp6C)!= 0)goto _LLB;_tmp75=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp6C)->f1;if((int)_tmp75 != 0)goto _LLB;
_LLA: return 1;_LLB: if(*((int*)_tmp6C)!= 15)goto _LLD;_tmp76=((struct Cyc_Absyn_Cast_e_struct*)
_tmp6C)->f2;_LLC: return Cyc_Toc_is_zero(_tmp76);_LLD: if(*((int*)_tmp6C)!= 26)goto
_LLF;_tmp77=((struct Cyc_Absyn_Tuple_e_struct*)_tmp6C)->f1;_LLE: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp77);_LLF: if(*((int*)_tmp6C)!= 28)goto _LL11;_tmp78=((struct Cyc_Absyn_Array_e_struct*)
_tmp6C)->f1;_LL10: _tmp79=_tmp78;goto _LL12;_LL11: if(*((int*)_tmp6C)!= 30)goto
_LL13;_tmp79=((struct Cyc_Absyn_Struct_e_struct*)_tmp6C)->f3;_LL12: _tmp7A=_tmp79;
goto _LL14;_LL13: if(*((int*)_tmp6C)!= 27)goto _LL15;_tmp7A=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp6C)->f2;_LL14: _tmp7B=_tmp7A;goto _LL16;_LL15: if(*((int*)_tmp6C)!= 36)goto
_LL17;_tmp7B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp6C)->f2;_LL16: for(0;
_tmp7B != 0;_tmp7B=_tmp7B->tl){if(!Cyc_Toc_is_zero((*((struct _tuple6*)_tmp7B->hd)).f2))
return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}static int Cyc_Toc_is_nullable(void*t){
void*_tmp7C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp7D;struct Cyc_Absyn_PtrAtts
_tmp7E;struct Cyc_Absyn_Conref*_tmp7F;_LL1A: if(_tmp7C <= (void*)4)goto _LL1C;if(*((
int*)_tmp7C)!= 4)goto _LL1C;_tmp7D=((struct Cyc_Absyn_PointerType_struct*)_tmp7C)->f1;
_tmp7E=_tmp7D.ptr_atts;_tmp7F=_tmp7E.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp7F);_LL1C:;_LL1D:({void*_tmp80[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp81="is_nullable";_tag_dynforward(_tmp81,sizeof(char),_get_zero_arr_size(
_tmp81,12));}),_tag_dynforward(_tmp80,sizeof(void*),0));});_LL19:;}static struct
_tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct _dynforward_ptr tag){
return({struct _tuple1*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(*x).f1;
_tmp82->f2=({struct _dynforward_ptr*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=(
struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)*(*x).f2,(struct
_dynforward_ptr)tag);_tmp83;});_tmp82;});}struct _tuple7{struct _dynforward_ptr*f1;
struct Cyc_List_List*f2;};static struct _dynforward_ptr*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){{struct Cyc_List_List*_tmp84=Cyc_Toc_tuple_types;for(0;
_tmp84 != 0;_tmp84=_tmp84->tl){struct _tuple7 _tmp86;struct _dynforward_ptr*_tmp87;
struct Cyc_List_List*_tmp88;struct _tuple7*_tmp85=(struct _tuple7*)_tmp84->hd;
_tmp86=*_tmp85;_tmp87=_tmp86.f1;_tmp88=_tmp86.f2;{struct Cyc_List_List*_tmp89=
tqs0;for(0;_tmp89 != 0  && _tmp88 != 0;(_tmp89=_tmp89->tl,_tmp88=_tmp88->tl)){if(!
Cyc_Tcutil_unify((*((struct _tuple4*)_tmp89->hd)).f2,(void*)_tmp88->hd))break;}
if(_tmp89 == 0  && _tmp88 == 0)return _tmp87;}}}{struct _dynforward_ptr*x=({struct
_dynforward_ptr*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp99;_tmp99.tag=1;_tmp99.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmp97[1]={& _tmp99};Cyc_aprintf(({
const char*_tmp98="_tuple%d";_tag_dynforward(_tmp98,sizeof(char),
_get_zero_arr_size(_tmp98,9));}),_tag_dynforward(_tmp97,sizeof(void*),1));}});
_tmp96;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmp8A=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmp8A=({struct Cyc_List_List*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->hd=({
struct Cyc_Absyn_Aggrfield*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->name=Cyc_Absyn_fieldname(
i);_tmp8C->tq=Cyc_Toc_mt_tq;_tmp8C->type=(void*)((void*)ts2->hd);_tmp8C->width=0;
_tmp8C->attributes=0;_tmp8C;});_tmp8B->tl=_tmp8A;_tmp8B;});}}_tmp8A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8A);{struct Cyc_Absyn_Aggrdecl*
_tmp8D=({struct Cyc_Absyn_Aggrdecl*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->kind=(
void*)((void*)0);_tmp93->sc=(void*)((void*)2);_tmp93->name=({struct _tuple1*
_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->f1=Cyc_Absyn_rel_ns_null;_tmp95->f2=x;
_tmp95;});_tmp93->tvs=0;_tmp93->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp94=
_cycalloc(sizeof(*_tmp94));_tmp94->exist_vars=0;_tmp94->rgn_po=0;_tmp94->fields=
_tmp8A;_tmp94;});_tmp93->attributes=0;_tmp93;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp90;_tmp90.tag=4;_tmp90.f1=_tmp8D;_tmp90;});_tmp8F;}),0);_tmp8E->tl=Cyc_Toc_result_decls;
_tmp8E;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp91=_cycalloc(sizeof(*
_tmp91));_tmp91->hd=({struct _tuple7*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->f1=
x;_tmp92->f2=ts;_tmp92;});_tmp91->tl=Cyc_Toc_tuple_types;_tmp91;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmp9A=_cycalloc(sizeof(*
_tmp9A));_tmp9A->f1=(void*)0;_tmp9A->f2=({struct _dynforward_ptr*_tmp9B=_cycalloc(
sizeof(*_tmp9B));_tmp9B[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp9E;_tmp9E.tag=1;_tmp9E.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmp9C[1]={& _tmp9E};Cyc_aprintf(({const char*_tmp9D="_tmp%X";_tag_dynforward(
_tmp9D,sizeof(char),_get_zero_arr_size(_tmp9D,7));}),_tag_dynforward(_tmp9C,
sizeof(void*),1));}});_tmp9B;});_tmp9A;});}static struct _dynforward_ptr*Cyc_Toc_fresh_label(){
return({struct _dynforward_ptr*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpA2;_tmpA2.tag=1;_tmpA2.f1=(
unsigned int)Cyc_Toc_fresh_label_counter ++;{void*_tmpA0[1]={& _tmpA2};Cyc_aprintf(({
const char*_tmpA1="_LL%X";_tag_dynforward(_tmpA1,sizeof(char),_get_zero_arr_size(
_tmpA1,6));}),_tag_dynforward(_tmpA0,sizeof(void*),1));}});_tmp9F;});}static
struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,struct
_tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmpA3=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmpA3))->hd)->name)
!= 0){if(carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpA3->hd)->typs != 0
 || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpA3->hd)->typs == 0)++ ans;
_tmpA3=_tmpA3->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmpA4=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;_tmpA4 != 0;_tmpA4=_tmpA4->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmpA4->hd)->typs == 0)++ ans;}}return ans;}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*Cyc_Toc_arg_to_c(struct
_tuple2*a){struct Cyc_Core_Opt*_tmpA6;struct Cyc_Absyn_Tqual _tmpA7;void*_tmpA8;
struct _tuple2 _tmpA5=*a;_tmpA6=_tmpA5.f1;_tmpA7=_tmpA5.f2;_tmpA8=_tmpA5.f3;return({
struct _tuple2*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->f1=_tmpA6;_tmpA9->f2=
_tmpA7;_tmpA9->f3=Cyc_Toc_typ_to_c(_tmpA8);_tmpA9;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(
struct _tuple4*x){struct Cyc_Absyn_Tqual _tmpAB;void*_tmpAC;struct _tuple4 _tmpAA=*x;
_tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;return({struct _tuple4*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD->f1=_tmpAB;_tmpAD->f2=Cyc_Toc_typ_to_c(_tmpAC);_tmpAD;});}static
void*Cyc_Toc_typ_to_c_array(void*t){void*_tmpAE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmpAF;void*_tmpB0;struct Cyc_Absyn_Tqual _tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct
Cyc_Absyn_Conref*_tmpB3;struct Cyc_Position_Segment*_tmpB4;struct Cyc_Core_Opt*
_tmpB5;struct Cyc_Core_Opt _tmpB6;void*_tmpB7;_LL1F: if(_tmpAE <= (void*)4)goto _LL23;
if(*((int*)_tmpAE)!= 7)goto _LL21;_tmpAF=((struct Cyc_Absyn_ArrayType_struct*)
_tmpAE)->f1;_tmpB0=(void*)_tmpAF.elt_type;_tmpB1=_tmpAF.tq;_tmpB2=_tmpAF.num_elts;
_tmpB3=_tmpAF.zero_term;_tmpB4=_tmpAF.zt_loc;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmpB0),_tmpB1,_tmpB2,Cyc_Absyn_false_conref,_tmpB4);_LL21: if(*((int*)_tmpAE)!= 
0)goto _LL23;_tmpB5=((struct Cyc_Absyn_Evar_struct*)_tmpAE)->f2;if(_tmpB5 == 0)goto
_LL23;_tmpB6=*_tmpB5;_tmpB7=(void*)_tmpB6.v;_LL22: return Cyc_Toc_typ_to_c_array(
_tmpB7);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->name=f->name;_tmpB8->tq=f->tq;_tmpB8->type=(
void*)Cyc_Toc_typ_to_c((void*)f->type);_tmpB8->width=f->width;_tmpB8->attributes=
f->attributes;_tmpB8;});}static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*
fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmpB9=t;struct Cyc_Core_Opt*
_tmpBA;struct Cyc_Core_Opt*_tmpBB;struct Cyc_Core_Opt _tmpBC;void*_tmpBD;struct Cyc_Absyn_Tvar*
_tmpBE;struct Cyc_Absyn_TunionInfo _tmpBF;void*_tmpC0;struct Cyc_Absyn_TunionFieldInfo
_tmpC1;void*_tmpC2;struct Cyc_Absyn_Tuniondecl*_tmpC3;struct Cyc_Absyn_Tunionfield*
_tmpC4;struct Cyc_Absyn_PtrInfo _tmpC5;void*_tmpC6;struct Cyc_Absyn_Tqual _tmpC7;
struct Cyc_Absyn_PtrAtts _tmpC8;struct Cyc_Absyn_Conref*_tmpC9;struct Cyc_Absyn_ArrayInfo
_tmpCA;void*_tmpCB;struct Cyc_Absyn_Tqual _tmpCC;struct Cyc_Absyn_Exp*_tmpCD;struct
Cyc_Position_Segment*_tmpCE;struct Cyc_Absyn_FnInfo _tmpCF;void*_tmpD0;struct Cyc_List_List*
_tmpD1;int _tmpD2;struct Cyc_Absyn_VarargInfo*_tmpD3;struct Cyc_List_List*_tmpD4;
struct Cyc_List_List*_tmpD5;void*_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_Absyn_AggrInfo
_tmpD8;void*_tmpD9;struct Cyc_List_List*_tmpDA;struct _tuple1*_tmpDB;struct Cyc_List_List*
_tmpDC;struct _tuple1*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Absyn_Typedefdecl*
_tmpDF;void**_tmpE0;void*_tmpE1;_LL26: if((int)_tmpB9 != 0)goto _LL28;_LL27: return t;
_LL28: if(_tmpB9 <= (void*)4)goto _LL3A;if(*((int*)_tmpB9)!= 0)goto _LL2A;_tmpBA=((
struct Cyc_Absyn_Evar_struct*)_tmpB9)->f2;if(_tmpBA != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;
_LL2A: if(*((int*)_tmpB9)!= 0)goto _LL2C;_tmpBB=((struct Cyc_Absyn_Evar_struct*)
_tmpB9)->f2;if(_tmpBB == 0)goto _LL2C;_tmpBC=*_tmpBB;_tmpBD=(void*)_tmpBC.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmpBD);_LL2C: if(*((int*)_tmpB9)!= 1)goto _LL2E;_tmpBE=((
struct Cyc_Absyn_VarType_struct*)_tmpB9)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(_tmpBE)
== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E: if(*((int*)
_tmpB9)!= 2)goto _LL30;_tmpBF=((struct Cyc_Absyn_TunionType_struct*)_tmpB9)->f1;
_tmpC0=(void*)_tmpBF.tunion_info;if(*((int*)_tmpC0)!= 1)goto _LL30;_LL2F: return
Cyc_Absyn_void_star_typ();_LL30: if(*((int*)_tmpB9)!= 2)goto _LL32;_LL31:({void*
_tmpE2[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE3="unresolved TunionType";_tag_dynforward(_tmpE3,sizeof(char),
_get_zero_arr_size(_tmpE3,22));}),_tag_dynforward(_tmpE2,sizeof(void*),0));});
_LL32: if(*((int*)_tmpB9)!= 3)goto _LL34;_tmpC1=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpB9)->f1;_tmpC2=(void*)_tmpC1.field_info;if(*((int*)_tmpC2)!= 1)goto _LL34;
_tmpC3=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpC2)->f1;_tmpC4=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpC2)->f2;_LL33: if(_tmpC4->typs == 0){if(_tmpC3->is_xtunion)return Cyc_Absyn_star_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmpC4->name,({const char*
_tmpE4="_struct";_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,8));})));}
_LL34: if(*((int*)_tmpB9)!= 3)goto _LL36;_LL35:({void*_tmpE5[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmpE6="unresolved TunionFieldType";_tag_dynforward(_tmpE6,sizeof(char),
_get_zero_arr_size(_tmpE6,27));}),_tag_dynforward(_tmpE5,sizeof(void*),0));});
_LL36: if(*((int*)_tmpB9)!= 4)goto _LL38;_tmpC5=((struct Cyc_Absyn_PointerType_struct*)
_tmpB9)->f1;_tmpC6=(void*)_tmpC5.elt_typ;_tmpC7=_tmpC5.elt_tq;_tmpC8=_tmpC5.ptr_atts;
_tmpC9=_tmpC8.bounds;_LL37: _tmpC6=Cyc_Toc_typ_to_c_array(_tmpC6);{void*_tmpE7=(
void*)(Cyc_Absyn_compress_conref(_tmpC9))->v;void*_tmpE8;void*_tmpE9;_LL61: if((
int)_tmpE7 != 0)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmpE7 <= (void*)1)goto _LL67;
if(*((int*)_tmpE7)!= 0)goto _LL65;_tmpE8=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpE7)->f1;if((int)_tmpE8 != 0)goto _LL65;_LL64: return Cyc_Toc_dynforward_ptr_typ;
_LL65: if(*((int*)_tmpE7)!= 0)goto _LL67;_tmpE9=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpE7)->f1;if((int)_tmpE9 != 1)goto _LL67;_LL66: return Cyc_Toc_dyneither_ptr_typ;
_LL67:;_LL68: return Cyc_Absyn_star_typ(_tmpC6,(void*)2,_tmpC7,Cyc_Absyn_false_conref);
_LL60:;}_LL38: if(*((int*)_tmpB9)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)
_tmpB9 != 1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmpB9 <= (void*)4)goto _LL56;if(*((
int*)_tmpB9)!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(*((int*)_tmpB9)!= 7)goto _LL40;
_tmpCA=((struct Cyc_Absyn_ArrayType_struct*)_tmpB9)->f1;_tmpCB=(void*)_tmpCA.elt_type;
_tmpCC=_tmpCA.tq;_tmpCD=_tmpCA.num_elts;_tmpCE=_tmpCA.zt_loc;_LL3F: return Cyc_Absyn_array_typ(
Cyc_Toc_typ_to_c_array(_tmpCB),_tmpCC,_tmpCD,Cyc_Absyn_false_conref,_tmpCE);
_LL40: if(*((int*)_tmpB9)!= 8)goto _LL42;_tmpCF=((struct Cyc_Absyn_FnType_struct*)
_tmpB9)->f1;_tmpD0=(void*)_tmpCF.ret_typ;_tmpD1=_tmpCF.args;_tmpD2=_tmpCF.c_varargs;
_tmpD3=_tmpCF.cyc_varargs;_tmpD4=_tmpCF.attributes;_LL41: {struct Cyc_List_List*
_tmpEA=0;for(0;_tmpD4 != 0;_tmpD4=_tmpD4->tl){void*_tmpEB=(void*)_tmpD4->hd;_LL6A:
if((int)_tmpEB != 3)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)_tmpEB != 4)goto _LL6E;
_LL6D: goto _LL6F;_LL6E: if(_tmpEB <= (void*)17)goto _LL72;if(*((int*)_tmpEB)!= 3)
goto _LL70;_LL6F: continue;_LL70: if(*((int*)_tmpEB)!= 4)goto _LL72;_LL71: continue;
_LL72:;_LL73: _tmpEA=({struct Cyc_List_List*_tmpEC=_cycalloc(sizeof(*_tmpEC));
_tmpEC->hd=(void*)((void*)_tmpD4->hd);_tmpEC->tl=_tmpEA;_tmpEC;});goto _LL69;
_LL69:;}{struct Cyc_List_List*_tmpED=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(
struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmpD1);
if(_tmpD3 != 0){void*_tmpEE=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((void*)
_tmpD3->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct _tuple2*
_tmpEF=({struct _tuple2*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->f1=_tmpD3->name;
_tmpF1->f2=_tmpD3->tq;_tmpF1->f3=_tmpEE;_tmpF1;});_tmpED=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpED,({
struct Cyc_List_List*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->hd=_tmpEF;_tmpF0->tl=
0;_tmpF0;}));}return(void*)({struct Cyc_Absyn_FnType_struct*_tmpF2=_cycalloc(
sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_FnType_struct _tmpF3;_tmpF3.tag=8;
_tmpF3.f1=({struct Cyc_Absyn_FnInfo _tmpF4;_tmpF4.tvars=0;_tmpF4.effect=0;_tmpF4.ret_typ=(
void*)Cyc_Toc_typ_to_c(_tmpD0);_tmpF4.args=_tmpED;_tmpF4.c_varargs=_tmpD2;_tmpF4.cyc_varargs=
0;_tmpF4.rgn_po=0;_tmpF4.attributes=_tmpEA;_tmpF4;});_tmpF3;});_tmpF2;});}}_LL42:
if(*((int*)_tmpB9)!= 9)goto _LL44;_tmpD5=((struct Cyc_Absyn_TupleType_struct*)
_tmpB9)->f1;_LL43: _tmpD5=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpD5);{
struct _dynforward_ptr*_tmpF5=Cyc_Toc_add_tuple_type(_tmpD5);return Cyc_Absyn_strct(
_tmpF5);}_LL44: if(*((int*)_tmpB9)!= 11)goto _LL46;_tmpD6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB9)->f1;_tmpD7=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpB9)->f2;_LL45:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpF6=_cycalloc(sizeof(*
_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpF7;_tmpF7.tag=11;
_tmpF7.f1=(void*)_tmpD6;_tmpF7.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmpD7);_tmpF7;});_tmpF6;});_LL46: if(*((int*)_tmpB9)!= 10)goto _LL48;_tmpD8=((
struct Cyc_Absyn_AggrType_struct*)_tmpB9)->f1;_tmpD9=(void*)_tmpD8.aggr_info;
_tmpDA=_tmpD8.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*_tmpF8=Cyc_Absyn_get_known_aggrdecl(
_tmpD9);if((void*)_tmpF8->kind == (void*)1)return Cyc_Absyn_unionq_typ(_tmpF8->name);
return Cyc_Absyn_strctq(_tmpF8->name);}_LL48: if(*((int*)_tmpB9)!= 12)goto _LL4A;
_tmpDB=((struct Cyc_Absyn_EnumType_struct*)_tmpB9)->f1;_LL49: return t;_LL4A: if(*((
int*)_tmpB9)!= 13)goto _LL4C;_tmpDC=((struct Cyc_Absyn_AnonEnumType_struct*)_tmpB9)->f1;
_LL4B: Cyc_Toc_enumfields_to_c(_tmpDC);return t;_LL4C: if(*((int*)_tmpB9)!= 16)goto
_LL4E;_tmpDD=((struct Cyc_Absyn_TypedefType_struct*)_tmpB9)->f1;_tmpDE=((struct
Cyc_Absyn_TypedefType_struct*)_tmpB9)->f2;_tmpDF=((struct Cyc_Absyn_TypedefType_struct*)
_tmpB9)->f3;_tmpE0=((struct Cyc_Absyn_TypedefType_struct*)_tmpB9)->f4;_LL4D: if(
_tmpE0 == 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpF9=_cycalloc(
sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_TypedefType_struct _tmpFA;_tmpFA.tag=
16;_tmpFA.f1=_tmpDD;_tmpFA.f2=0;_tmpFA.f3=_tmpDF;_tmpFA.f4=0;_tmpFA;});_tmpF9;});
else{return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpFB=_cycalloc(sizeof(*
_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_TypedefType_struct _tmpFC;_tmpFC.tag=16;
_tmpFC.f1=_tmpDD;_tmpFC.f2=0;_tmpFC.f3=_tmpDF;_tmpFC.f4=({void**_tmpFD=_cycalloc(
sizeof(*_tmpFD));_tmpFD[0]=Cyc_Toc_typ_to_c_array(*_tmpE0);_tmpFD;});_tmpFC;});
_tmpFB;});}_LL4E: if(*((int*)_tmpB9)!= 14)goto _LL50;_LL4F: goto _LL51;_LL50: if(*((
int*)_tmpB9)!= 17)goto _LL52;_LL51: return Cyc_Absyn_uint_typ;_LL52: if(*((int*)
_tmpB9)!= 15)goto _LL54;_tmpE1=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpB9)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(*((int*)_tmpB9)!= 18)goto _LL56;_LL55:({void*_tmpFE[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmpFF="Toc::typ_to_c: type translation passed a type integer";
_tag_dynforward(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,54));}),
_tag_dynforward(_tmpFE,sizeof(void*),0));});_LL56: if((int)_tmpB9 != 2)goto _LL58;
_LL57:({void*_tmp100[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp101="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp101,sizeof(char),_get_zero_arr_size(_tmp101,55));}),
_tag_dynforward(_tmp100,sizeof(void*),0));});_LL58: if((int)_tmpB9 != 3)goto _LL5A;
_LL59:({void*_tmp102[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp103="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp103,sizeof(char),_get_zero_arr_size(_tmp103,57));}),
_tag_dynforward(_tmp102,sizeof(void*),0));});_LL5A: if(_tmpB9 <= (void*)4)goto
_LL5C;if(*((int*)_tmpB9)!= 19)goto _LL5C;_LL5B: goto _LL5D;_LL5C: if(_tmpB9 <= (void*)
4)goto _LL5E;if(*((int*)_tmpB9)!= 20)goto _LL5E;_LL5D: goto _LL5F;_LL5E: if(_tmpB9 <= (
void*)4)goto _LL25;if(*((int*)_tmpB9)!= 21)goto _LL25;_LL5F:({void*_tmp104[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp105="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp105,sizeof(char),_get_zero_arr_size(_tmp105,49));}),
_tag_dynforward(_tmp104,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp106=t;struct Cyc_Absyn_ArrayInfo _tmp107;void*_tmp108;struct Cyc_Absyn_Tqual
_tmp109;_LL75: if(_tmp106 <= (void*)4)goto _LL77;if(*((int*)_tmp106)!= 7)goto _LL77;
_tmp107=((struct Cyc_Absyn_ArrayType_struct*)_tmp106)->f1;_tmp108=(void*)_tmp107.elt_type;
_tmp109=_tmp107.tq;_LL76: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp108,(void*)
2,_tmp109,Cyc_Absyn_false_conref),e);_LL77:;_LL78: return Cyc_Toc_cast_it(t,e);
_LL74:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp10A=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp10B;void*_tmp10C;struct Cyc_Absyn_AggrInfo _tmp10D;
void*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_Absyn_TunionFieldInfo _tmp110;
void*_tmp111;struct Cyc_Absyn_Tuniondecl*_tmp112;struct Cyc_Absyn_Tunionfield*
_tmp113;struct Cyc_List_List*_tmp114;_LL7A: if((int)_tmp10A != 0)goto _LL7C;_LL7B:
return 1;_LL7C: if(_tmp10A <= (void*)4)goto _LL84;if(*((int*)_tmp10A)!= 1)goto _LL7E;
_LL7D: return 0;_LL7E: if(*((int*)_tmp10A)!= 5)goto _LL80;_LL7F: goto _LL81;_LL80: if(*((
int*)_tmp10A)!= 12)goto _LL82;_LL81: goto _LL83;_LL82: if(*((int*)_tmp10A)!= 13)goto
_LL84;_LL83: goto _LL85;_LL84: if((int)_tmp10A != 1)goto _LL86;_LL85: goto _LL87;_LL86:
if(_tmp10A <= (void*)4)goto _LL9E;if(*((int*)_tmp10A)!= 6)goto _LL88;_LL87: goto
_LL89;_LL88: if(*((int*)_tmp10A)!= 8)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)
_tmp10A)!= 17)goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(*((int*)_tmp10A)!= 14)goto _LL8E;
_LL8D: return 1;_LL8E: if(*((int*)_tmp10A)!= 7)goto _LL90;_tmp10B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp10A)->f1;_tmp10C=(void*)_tmp10B.elt_type;_LL8F: return Cyc_Toc_atomic_typ(
_tmp10C);_LL90: if(*((int*)_tmp10A)!= 10)goto _LL92;_tmp10D=((struct Cyc_Absyn_AggrType_struct*)
_tmp10A)->f1;_tmp10E=(void*)_tmp10D.aggr_info;_LL91:{void*_tmp115=_tmp10E;_LLA1:
if(*((int*)_tmp115)!= 0)goto _LLA3;_LLA2: return 0;_LLA3:;_LLA4: goto _LLA0;_LLA0:;}{
struct Cyc_Absyn_Aggrdecl*_tmp116=Cyc_Absyn_get_known_aggrdecl(_tmp10E);if(
_tmp116->impl == 0)return 0;{struct Cyc_List_List*_tmp117=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp116->impl))->fields;for(0;_tmp117 != 0;_tmp117=_tmp117->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp117->hd)->type))
return 0;}}return 1;}_LL92: if(*((int*)_tmp10A)!= 11)goto _LL94;_tmp10F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp10A)->f2;_LL93: for(0;_tmp10F != 0;_tmp10F=_tmp10F->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp10F->hd)->type))return 0;}return 1;_LL94:
if(*((int*)_tmp10A)!= 3)goto _LL96;_tmp110=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp10A)->f1;_tmp111=(void*)_tmp110.field_info;if(*((int*)_tmp111)!= 1)goto _LL96;
_tmp112=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp111)->f1;_tmp113=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp111)->f2;_LL95: _tmp114=_tmp113->typs;goto
_LL97;_LL96: if(*((int*)_tmp10A)!= 9)goto _LL98;_tmp114=((struct Cyc_Absyn_TupleType_struct*)
_tmp10A)->f1;_LL97: for(0;_tmp114 != 0;_tmp114=_tmp114->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp114->hd)).f2))return 0;}return 1;_LL98: if(*((int*)_tmp10A)!= 2)
goto _LL9A;_LL99: goto _LL9B;_LL9A: if(*((int*)_tmp10A)!= 4)goto _LL9C;_LL9B: goto
_LL9D;_LL9C: if(*((int*)_tmp10A)!= 15)goto _LL9E;_LL9D: return 0;_LL9E:;_LL9F:({
struct Cyc_String_pa_struct _tmp11A;_tmp11A.tag=0;_tmp11A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp118[
1]={& _tmp11A};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp119="atomic_typ:  bad type %s";_tag_dynforward(_tmp119,sizeof(char),
_get_zero_arr_size(_tmp119,25));}),_tag_dynforward(_tmp118,sizeof(void*),1));}});
_LL79:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp11B=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp11C;void*_tmp11D;_LLA6: if(_tmp11B <= (void*)4)goto
_LLA8;if(*((int*)_tmp11B)!= 4)goto _LLA8;_tmp11C=((struct Cyc_Absyn_PointerType_struct*)
_tmp11B)->f1;_tmp11D=(void*)_tmp11C.elt_typ;_LLA7: {void*_tmp11E=Cyc_Tcutil_compress(
_tmp11D);_LLAB: if((int)_tmp11E != 0)goto _LLAD;_LLAC: return 1;_LLAD:;_LLAE: return 0;
_LLAA:;}_LLA8:;_LLA9: return 0;_LLA5:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp11F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp120;void*_tmp121;struct Cyc_List_List*_tmp122;_LLB0: if(_tmp11F <= (void*)4)
goto _LLB4;if(*((int*)_tmp11F)!= 10)goto _LLB2;_tmp120=((struct Cyc_Absyn_AggrType_struct*)
_tmp11F)->f1;_tmp121=(void*)_tmp120.aggr_info;_LLB1: {struct Cyc_Absyn_Aggrdecl*
_tmp123=Cyc_Absyn_get_known_aggrdecl(_tmp121);if(_tmp123->impl == 0)({void*
_tmp124[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp125="is_poly_field: type missing fields";_tag_dynforward(_tmp125,
sizeof(char),_get_zero_arr_size(_tmp125,35));}),_tag_dynforward(_tmp124,sizeof(
void*),0));});_tmp122=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp123->impl))->fields;
goto _LLB3;}_LLB2: if(*((int*)_tmp11F)!= 11)goto _LLB4;_tmp122=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11F)->f2;_LLB3: {struct Cyc_Absyn_Aggrfield*_tmp126=Cyc_Absyn_lookup_field(
_tmp122,f);if(_tmp126 == 0)({struct Cyc_String_pa_struct _tmp129;_tmp129.tag=0;
_tmp129.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp127[1]={&
_tmp129};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp128="is_poly_field: bad field %s";_tag_dynforward(_tmp128,sizeof(
char),_get_zero_arr_size(_tmp128,28));}),_tag_dynforward(_tmp127,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp126->type);}_LLB4:;_LLB5:({struct Cyc_String_pa_struct
_tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp12A[1]={& _tmp12C};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp12B="is_poly_field: bad type %s";_tag_dynforward(_tmp12B,sizeof(char),
_get_zero_arr_size(_tmp12B,27));}),_tag_dynforward(_tmp12A,sizeof(void*),1));}});
_LLAF:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp12D=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp12E;struct _dynforward_ptr*_tmp12F;struct Cyc_Absyn_Exp*
_tmp130;struct _dynforward_ptr*_tmp131;_LLB7: if(*((int*)_tmp12D)!= 23)goto _LLB9;
_tmp12E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp12D)->f1;_tmp12F=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp12D)->f2;_LLB8: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp12E->topt))->v,_tmp12F);_LLB9: if(*((int*)_tmp12D)!= 24)goto _LLBB;
_tmp130=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp12D)->f1;_tmp131=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp12D)->f2;_LLBA: {void*_tmp132=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp130->topt))->v);struct Cyc_Absyn_PtrInfo _tmp133;void*_tmp134;
_LLBE: if(_tmp132 <= (void*)4)goto _LLC0;if(*((int*)_tmp132)!= 4)goto _LLC0;_tmp133=((
struct Cyc_Absyn_PointerType_struct*)_tmp132)->f1;_tmp134=(void*)_tmp133.elt_typ;
_LLBF: return Cyc_Toc_is_poly_field(_tmp134,_tmp131);_LLC0:;_LLC1:({struct Cyc_String_pa_struct
_tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp130->topt))->v));{
void*_tmp135[1]={& _tmp137};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp136="is_poly_project: bad type %s";
_tag_dynforward(_tmp136,sizeof(char),_get_zero_arr_size(_tmp136,29));}),
_tag_dynforward(_tmp135,sizeof(void*),1));}});_LLBD:;}_LLBB:;_LLBC: return 0;_LLB6:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));
_tmp138->hd=s;_tmp138->tl=0;_tmp138;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=s;_tmp139->tl=
0;_tmp139;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp13A[2];_tmp13A[1]=s;_tmp13A[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp13A,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp13B[2];_tmp13B[1]=n;
_tmp13B[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp13B,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp13C[2];_tmp13C[1]=n;_tmp13C[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp13C,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp13D[3];_tmp13D[2]=n;_tmp13D[1]=
s;_tmp13D[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp13D,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=e;_tmp13E->tl=
0;_tmp13E;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp13F=(void*)e->r;void*_tmp140;_LLC3: if(*((int*)_tmp13F)!= 0)goto _LLC5;
_tmp140=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;if(_tmp140 <= (void*)
1)goto _LLC5;if(*((int*)_tmp140)!= 5)goto _LLC5;_LLC4: is_string=1;goto _LLC2;_LLC5:;
_LLC6: goto _LLC2;_LLC2:;}{struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;
if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();void*vd_typ=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref,
0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)
e);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));
_tmp141->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp142=
_cycalloc(sizeof(*_tmp142));_tmp142[0]=({struct Cyc_Absyn_Var_d_struct _tmp143;
_tmp143.tag=0;_tmp143.f1=vd;_tmp143;});_tmp142;}),0);_tmp141->tl=Cyc_Toc_result_decls;
_tmp141;});xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}
else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*
urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*
_tmp144[3];_tmp144[2]=({struct _tuple6*_tmp147=_cycalloc(sizeof(*_tmp147));
_tmp147->f1=0;_tmp147->f2=xplussz;_tmp147;});_tmp144[1]=({struct _tuple6*_tmp146=
_cycalloc(sizeof(*_tmp146));_tmp146->f1=0;_tmp146->f2=xexp;_tmp146;});_tmp144[0]=({
struct _tuple6*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->f1=0;_tmp145->f2=xexp;
_tmp145;});((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp144,sizeof(struct _tuple6*),3));}),0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple6*_tmp148[2];_tmp148[1]=({struct _tuple6*_tmp14A=_cycalloc(
sizeof(*_tmp14A));_tmp14A->f1=0;_tmp14A->f2=xplussz;_tmp14A;});_tmp148[0]=({
struct _tuple6*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->f1=0;_tmp149->f2=xexp;
_tmp149;});((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp148,sizeof(struct _tuple6*),2));}),0);}return urm_exp;}}}
struct Cyc_Toc_Env{struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;
struct Cyc_Core_Opt*fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static
struct Cyc_Toc_Env*Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp14B=
_cycalloc(sizeof(*_tmp14B));_tmp14B->break_lab=0;_tmp14B->continue_lab=0;_tmp14B->fallthru_info=
0;_tmp14B->varmap=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14B->toplevel=1;_tmp14B;});}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*
ans=({struct Cyc_Toc_Env*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=*e;
_tmp14C;});ans->toplevel=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{void*_tmp14D=(*x).f1;
_LLC8: if(_tmp14D <= (void*)1)goto _LLCA;if(*((int*)_tmp14D)!= 0)goto _LLCA;_LLC9:({
struct Cyc_String_pa_struct _tmp150;_tmp150.tag=0;_tmp150.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp14E[1]={& _tmp150};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp14F="Toc::add_varmap on Rel_n: %s\n";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,30));}),
_tag_dynforward(_tmp14E,sizeof(void*),1));}});_LLCA:;_LLCB: goto _LLC7;_LLC7:;}{
struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp151=_cycalloc(sizeof(*_tmp151));
_tmp151[0]=*e;_tmp151;});ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);
return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct
Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[
0]=*e;_tmp152;});ans->break_lab=0;ans->continue_lab=0;return ans;}struct _tuple8{
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*f3;};static
struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*e,struct
_dynforward_ptr*break_l,struct _dynforward_ptr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp153=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp153=({
struct Cyc_List_List*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp154->tl=_tmp153;_tmp154;});}_tmp153=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp153);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=*e;_tmp158;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->v=
break_l;_tmp155;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp156=_cycalloc(
sizeof(*_tmp156));_tmp156->v=({struct _tuple8*_tmp157=_cycalloc(sizeof(*_tmp157));
_tmp157->f1=fallthru_l;_tmp157->f2=_tmp153;_tmp157->f3=next_case_env->varmap;
_tmp157;});_tmp156;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=*e;_tmp15A;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->v=
break_l;_tmp159;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct Cyc_Toc_Env*ans=({struct
Cyc_Toc_Env*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D[0]=*e;_tmp15D;});ans->break_lab=
0;ans->fallthru_info=({struct Cyc_Core_Opt*_tmp15B=_cycalloc(sizeof(*_tmp15B));
_tmp15B->v=({struct _tuple8*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->f1=
next_l;_tmp15C->f2=0;_tmp15C->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp15C;});_tmp15B;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp15E=(void*)e->annot;
_LLCD: if(*((void**)_tmp15E)!= Cyc_CfFlowInfo_UnknownZ)goto _LLCF;_LLCE: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLCF: if(*((void**)_tmp15E)
!= Cyc_CfFlowInfo_NotZero)goto _LLD1;_LLD0: return 0;_LLD1: if(_tmp15E != Cyc_CfFlowInfo_IsZero)
goto _LLD3;_LLD2:({void*_tmp15F[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp160="dereference of NULL pointer";
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,28));}),
_tag_dynforward(_tmp15F,sizeof(void*),0));});return 0;_LLD3: if(_tmp15E != Cyc_Absyn_EmptyAnnot)
goto _LLD5;_LLD4: return 0;_LLD5: if(*((void**)_tmp15E)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD7;_LLD6:({void*_tmp161[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp162="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,46));}),
_tag_dynforward(_tmp161,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD7:;_LLD8:({void*_tmp163[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp164="need_null_check";_tag_dynforward(_tmp164,sizeof(char),
_get_zero_arr_size(_tmp164,16));}),_tag_dynforward(_tmp163,sizeof(void*),0));});
_LLCC:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp165=(void*)e->annot;struct Cyc_List_List*_tmp166;struct Cyc_List_List*_tmp167;
_LLDA: if(*((void**)_tmp165)!= Cyc_CfFlowInfo_UnknownZ)goto _LLDC;_tmp166=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp165)->f1;_LLDB: return _tmp166;_LLDC: if(*((
void**)_tmp165)!= Cyc_CfFlowInfo_NotZero)goto _LLDE;_tmp167=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp165)->f1;_LLDD: return _tmp167;_LLDE: if(_tmp165 != Cyc_CfFlowInfo_IsZero)goto
_LLE0;_LLDF:({void*_tmp168[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp169="dereference of NULL pointer";
_tag_dynforward(_tmp169,sizeof(char),_get_zero_arr_size(_tmp169,28));}),
_tag_dynforward(_tmp168,sizeof(void*),0));});return 0;_LLE0: if(*((void**)_tmp165)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(_tmp165 != Cyc_Absyn_EmptyAnnot)
goto _LLE4;_LLE3: return 0;_LLE4:;_LLE5:({void*_tmp16A[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp16B="get_relns";_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(
_tmp16B,10));}),_tag_dynforward(_tmp16A,sizeof(void*),0));});_LLD9:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp16C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp16D;struct Cyc_Absyn_PtrAtts _tmp16E;struct Cyc_Absyn_Conref*
_tmp16F;struct Cyc_Absyn_Conref*_tmp170;struct Cyc_Absyn_ArrayInfo _tmp171;struct
Cyc_Absyn_Exp*_tmp172;_LLE7: if(_tmp16C <= (void*)4)goto _LLEB;if(*((int*)_tmp16C)
!= 4)goto _LLE9;_tmp16D=((struct Cyc_Absyn_PointerType_struct*)_tmp16C)->f1;
_tmp16E=_tmp16D.ptr_atts;_tmp16F=_tmp16E.bounds;_tmp170=_tmp16E.zero_term;_LLE8: {
void*_tmp173=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp16F);struct Cyc_Absyn_Exp*
_tmp174;_LLEE: if((int)_tmp173 != 0)goto _LLF0;_LLEF: return 0;_LLF0: if((int)_tmp173
!= 1)goto _LLF2;_LLF1: return 0;_LLF2: if(_tmp173 <= (void*)2)goto _LLF4;if(*((int*)
_tmp173)!= 0)goto _LLF4;_tmp174=((struct Cyc_Absyn_Upper_b_struct*)_tmp173)->f1;
_LLF3: {unsigned int _tmp176;int _tmp177;struct _tuple5 _tmp175=Cyc_Evexp_eval_const_uint_exp(
_tmp174);_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;return _tmp177  && i <= _tmp176;}
_LLF4: if(_tmp173 <= (void*)2)goto _LLED;if(*((int*)_tmp173)!= 1)goto _LLED;_LLF5:({
void*_tmp178[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp179="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,30));}),
_tag_dynforward(_tmp178,sizeof(void*),0));});_LLED:;}_LLE9: if(*((int*)_tmp16C)!= 
7)goto _LLEB;_tmp171=((struct Cyc_Absyn_ArrayType_struct*)_tmp16C)->f1;_tmp172=
_tmp171.num_elts;_LLEA: if(_tmp172 == 0)return 0;{unsigned int _tmp17B;int _tmp17C;
struct _tuple5 _tmp17A=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp172);
_tmp17B=_tmp17A.f1;_tmp17C=_tmp17A.f2;return _tmp17C  && i <= _tmp17B;}_LLEB:;_LLEC:
return 0;_LLE6:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp17D=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp17D->vd != y)continue;{void*_tmp17E=(void*)_tmp17D->rop;struct Cyc_Absyn_Vardecl*
_tmp17F;struct Cyc_Absyn_Vardecl*_tmp180;_LLF7: if(*((int*)_tmp17E)!= 2)goto _LLF9;
_tmp17F=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp17E)->f1;_LLF8: _tmp180=
_tmp17F;goto _LLFA;_LLF9: if(*((int*)_tmp17E)!= 4)goto _LLFB;_tmp180=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp17E)->f1;_LLFA: if(_tmp180 == v)return 1;else{goto _LLF6;}_LLFB:;_LLFC: continue;
_LLF6:;}}return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp181=(void*)e->r;void*
_tmp182;struct Cyc_Absyn_Vardecl*_tmp183;void*_tmp184;struct Cyc_Absyn_Vardecl*
_tmp185;void*_tmp186;struct Cyc_Absyn_Vardecl*_tmp187;void*_tmp188;struct Cyc_Absyn_Vardecl*
_tmp189;void*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_List_List _tmp18C;
struct Cyc_Absyn_Exp*_tmp18D;_LLFE: if(*((int*)_tmp181)!= 1)goto _LL100;_tmp182=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp181)->f2;if(_tmp182 <= (void*)1)goto
_LL100;if(*((int*)_tmp182)!= 4)goto _LL100;_tmp183=((struct Cyc_Absyn_Pat_b_struct*)
_tmp182)->f1;_LLFF: _tmp185=_tmp183;goto _LL101;_LL100: if(*((int*)_tmp181)!= 1)
goto _LL102;_tmp184=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp181)->f2;if(
_tmp184 <= (void*)1)goto _LL102;if(*((int*)_tmp184)!= 3)goto _LL102;_tmp185=((
struct Cyc_Absyn_Local_b_struct*)_tmp184)->f1;_LL101: _tmp187=_tmp185;goto _LL103;
_LL102: if(*((int*)_tmp181)!= 1)goto _LL104;_tmp186=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp181)->f2;if(_tmp186 <= (void*)1)goto _LL104;if(*((int*)_tmp186)!= 0)goto _LL104;
_tmp187=((struct Cyc_Absyn_Global_b_struct*)_tmp186)->f1;_LL103: _tmp189=_tmp187;
goto _LL105;_LL104: if(*((int*)_tmp181)!= 1)goto _LL106;_tmp188=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp181)->f2;if(_tmp188 <= (void*)1)goto _LL106;if(*((int*)_tmp188)!= 2)goto _LL106;
_tmp189=((struct Cyc_Absyn_Param_b_struct*)_tmp188)->f1;_LL105: if(_tmp189->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp189))return 1;goto _LLFD;_LL106:
if(*((int*)_tmp181)!= 3)goto _LL108;_tmp18A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp181)->f1;if((int)_tmp18A != 19)goto _LL108;_tmp18B=((struct Cyc_Absyn_Primop_e_struct*)
_tmp181)->f2;if(_tmp18B == 0)goto _LL108;_tmp18C=*_tmp18B;_tmp18D=(struct Cyc_Absyn_Exp*)
_tmp18C.hd;_LL107:{void*_tmp18E=(void*)_tmp18D->r;void*_tmp18F;struct Cyc_Absyn_Vardecl*
_tmp190;void*_tmp191;struct Cyc_Absyn_Vardecl*_tmp192;void*_tmp193;struct Cyc_Absyn_Vardecl*
_tmp194;void*_tmp195;struct Cyc_Absyn_Vardecl*_tmp196;_LL10B: if(*((int*)_tmp18E)
!= 1)goto _LL10D;_tmp18F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp18E)->f2;if(
_tmp18F <= (void*)1)goto _LL10D;if(*((int*)_tmp18F)!= 4)goto _LL10D;_tmp190=((
struct Cyc_Absyn_Pat_b_struct*)_tmp18F)->f1;_LL10C: _tmp192=_tmp190;goto _LL10E;
_LL10D: if(*((int*)_tmp18E)!= 1)goto _LL10F;_tmp191=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18E)->f2;if(_tmp191 <= (void*)1)goto _LL10F;if(*((int*)_tmp191)!= 3)goto _LL10F;
_tmp192=((struct Cyc_Absyn_Local_b_struct*)_tmp191)->f1;_LL10E: _tmp194=_tmp192;
goto _LL110;_LL10F: if(*((int*)_tmp18E)!= 1)goto _LL111;_tmp193=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18E)->f2;if(_tmp193 <= (void*)1)goto _LL111;if(*((int*)_tmp193)!= 0)goto _LL111;
_tmp194=((struct Cyc_Absyn_Global_b_struct*)_tmp193)->f1;_LL110: _tmp196=_tmp194;
goto _LL112;_LL111: if(*((int*)_tmp18E)!= 1)goto _LL113;_tmp195=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18E)->f2;if(_tmp195 <= (void*)1)goto _LL113;if(*((int*)_tmp195)!= 2)goto _LL113;
_tmp196=((struct Cyc_Absyn_Param_b_struct*)_tmp195)->f1;_LL112: return _tmp196 == v;
_LL113:;_LL114: goto _LL10A;_LL10A:;}goto _LLFD;_LL108:;_LL109: goto _LLFD;_LLFD:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp197=(void*)a->r;void*_tmp198;struct Cyc_Absyn_Vardecl*
_tmp199;void*_tmp19A;struct Cyc_Absyn_Vardecl*_tmp19B;void*_tmp19C;struct Cyc_Absyn_Vardecl*
_tmp19D;void*_tmp19E;struct Cyc_Absyn_Vardecl*_tmp19F;_LL116: if(*((int*)_tmp197)
!= 1)goto _LL118;_tmp198=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp197)->f2;if(
_tmp198 <= (void*)1)goto _LL118;if(*((int*)_tmp198)!= 4)goto _LL118;_tmp199=((
struct Cyc_Absyn_Pat_b_struct*)_tmp198)->f1;_LL117: _tmp19B=_tmp199;goto _LL119;
_LL118: if(*((int*)_tmp197)!= 1)goto _LL11A;_tmp19A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp197)->f2;if(_tmp19A <= (void*)1)goto _LL11A;if(*((int*)_tmp19A)!= 3)goto _LL11A;
_tmp19B=((struct Cyc_Absyn_Local_b_struct*)_tmp19A)->f1;_LL119: _tmp19D=_tmp19B;
goto _LL11B;_LL11A: if(*((int*)_tmp197)!= 1)goto _LL11C;_tmp19C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp197)->f2;if(_tmp19C <= (void*)1)goto _LL11C;if(*((int*)_tmp19C)!= 0)goto _LL11C;
_tmp19D=((struct Cyc_Absyn_Global_b_struct*)_tmp19C)->f1;_LL11B: _tmp19F=_tmp19D;
goto _LL11D;_LL11C: if(*((int*)_tmp197)!= 1)goto _LL11E;_tmp19E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp197)->f2;if(_tmp19E <= (void*)1)goto _LL11E;if(*((int*)_tmp19E)!= 2)goto _LL11E;
_tmp19F=((struct Cyc_Absyn_Param_b_struct*)_tmp19E)->f1;_LL11D: if(_tmp19F->escapes)
return 0;inner_loop: {void*_tmp1A0=(void*)i->r;void*_tmp1A1;struct Cyc_Absyn_Exp*
_tmp1A2;void*_tmp1A3;void*_tmp1A4;int _tmp1A5;void*_tmp1A6;void*_tmp1A7;int
_tmp1A8;void*_tmp1A9;void*_tmp1AA;int _tmp1AB;void*_tmp1AC;struct Cyc_List_List*
_tmp1AD;struct Cyc_List_List _tmp1AE;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_List_List*
_tmp1B0;struct Cyc_List_List _tmp1B1;struct Cyc_Absyn_Exp*_tmp1B2;void*_tmp1B3;
struct Cyc_Absyn_Vardecl*_tmp1B4;void*_tmp1B5;struct Cyc_Absyn_Vardecl*_tmp1B6;
void*_tmp1B7;struct Cyc_Absyn_Vardecl*_tmp1B8;void*_tmp1B9;struct Cyc_Absyn_Vardecl*
_tmp1BA;_LL121: if(*((int*)_tmp1A0)!= 15)goto _LL123;_tmp1A1=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1A0)->f1;_tmp1A2=((struct Cyc_Absyn_Cast_e_struct*)_tmp1A0)->f2;_LL122: i=
_tmp1A2;goto inner_loop;_LL123: if(*((int*)_tmp1A0)!= 0)goto _LL125;_tmp1A3=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp1A0)->f1;if(_tmp1A3 <= (void*)1)goto _LL125;
if(*((int*)_tmp1A3)!= 2)goto _LL125;_tmp1A4=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp1A3)->f1;if((int)_tmp1A4 != 2)goto _LL125;_tmp1A5=((struct Cyc_Absyn_Int_c_struct*)
_tmp1A3)->f2;_LL124: _tmp1A8=_tmp1A5;goto _LL126;_LL125: if(*((int*)_tmp1A0)!= 0)
goto _LL127;_tmp1A6=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1A0)->f1;if(
_tmp1A6 <= (void*)1)goto _LL127;if(*((int*)_tmp1A6)!= 2)goto _LL127;_tmp1A7=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp1A6)->f1;if((int)_tmp1A7 != 0)goto _LL127;
_tmp1A8=((struct Cyc_Absyn_Int_c_struct*)_tmp1A6)->f2;_LL126: return _tmp1A8 >= 0
 && Cyc_Toc_check_const_array((unsigned int)(_tmp1A8 + 1),(void*)_tmp19F->type);
_LL127: if(*((int*)_tmp1A0)!= 0)goto _LL129;_tmp1A9=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1A0)->f1;if(_tmp1A9 <= (void*)1)goto _LL129;if(*((int*)_tmp1A9)!= 2)goto _LL129;
_tmp1AA=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp1A9)->f1;if((int)_tmp1AA != 1)
goto _LL129;_tmp1AB=((struct Cyc_Absyn_Int_c_struct*)_tmp1A9)->f2;_LL128: return Cyc_Toc_check_const_array((
unsigned int)(_tmp1AB + 1),(void*)_tmp19F->type);_LL129: if(*((int*)_tmp1A0)!= 3)
goto _LL12B;_tmp1AC=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1A0)->f1;if((
int)_tmp1AC != 4)goto _LL12B;_tmp1AD=((struct Cyc_Absyn_Primop_e_struct*)_tmp1A0)->f2;
if(_tmp1AD == 0)goto _LL12B;_tmp1AE=*_tmp1AD;_tmp1AF=(struct Cyc_Absyn_Exp*)_tmp1AE.hd;
_tmp1B0=_tmp1AE.tl;if(_tmp1B0 == 0)goto _LL12B;_tmp1B1=*_tmp1B0;_tmp1B2=(struct Cyc_Absyn_Exp*)
_tmp1B1.hd;_LL12A: return Cyc_Toc_check_leq_size(relns,_tmp19F,_tmp1B2);_LL12B: if(*((
int*)_tmp1A0)!= 1)goto _LL12D;_tmp1B3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A0)->f2;if(_tmp1B3 <= (void*)1)goto _LL12D;if(*((int*)_tmp1B3)!= 4)goto _LL12D;
_tmp1B4=((struct Cyc_Absyn_Pat_b_struct*)_tmp1B3)->f1;_LL12C: _tmp1B6=_tmp1B4;goto
_LL12E;_LL12D: if(*((int*)_tmp1A0)!= 1)goto _LL12F;_tmp1B5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A0)->f2;if(_tmp1B5 <= (void*)1)goto _LL12F;if(*((int*)_tmp1B5)!= 3)goto _LL12F;
_tmp1B6=((struct Cyc_Absyn_Local_b_struct*)_tmp1B5)->f1;_LL12E: _tmp1B8=_tmp1B6;
goto _LL130;_LL12F: if(*((int*)_tmp1A0)!= 1)goto _LL131;_tmp1B7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A0)->f2;if(_tmp1B7 <= (void*)1)goto _LL131;if(*((int*)_tmp1B7)!= 0)goto _LL131;
_tmp1B8=((struct Cyc_Absyn_Global_b_struct*)_tmp1B7)->f1;_LL130: _tmp1BA=_tmp1B8;
goto _LL132;_LL131: if(*((int*)_tmp1A0)!= 1)goto _LL133;_tmp1B9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A0)->f2;if(_tmp1B9 <= (void*)1)goto _LL133;if(*((int*)_tmp1B9)!= 2)goto _LL133;
_tmp1BA=((struct Cyc_Absyn_Param_b_struct*)_tmp1B9)->f1;_LL132: if(_tmp1BA->escapes)
return 0;{struct Cyc_List_List*_tmp1BB=relns;for(0;_tmp1BB != 0;_tmp1BB=_tmp1BB->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1BC=(struct Cyc_CfFlowInfo_Reln*)_tmp1BB->hd;if(
_tmp1BC->vd == _tmp1BA){void*_tmp1BD=(void*)_tmp1BC->rop;struct Cyc_Absyn_Vardecl*
_tmp1BE;struct Cyc_Absyn_Vardecl*_tmp1BF;unsigned int _tmp1C0;_LL136: if(*((int*)
_tmp1BD)!= 2)goto _LL138;_tmp1BE=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1BD)->f1;
_LL137: if(_tmp19F == _tmp1BE)return 1;else{goto _LL135;}_LL138: if(*((int*)_tmp1BD)
!= 1)goto _LL13A;_tmp1BF=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1BD)->f1;
_LL139:{struct Cyc_List_List*_tmp1C1=relns;for(0;_tmp1C1 != 0;_tmp1C1=_tmp1C1->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1C2=(struct Cyc_CfFlowInfo_Reln*)_tmp1C1->hd;if(
_tmp1C2->vd == _tmp1BF){void*_tmp1C3=(void*)_tmp1C2->rop;struct Cyc_Absyn_Vardecl*
_tmp1C4;struct Cyc_Absyn_Vardecl*_tmp1C5;unsigned int _tmp1C6;struct Cyc_Absyn_Vardecl*
_tmp1C7;_LL13F: if(*((int*)_tmp1C3)!= 4)goto _LL141;_tmp1C4=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1C3)->f1;_LL140: _tmp1C5=_tmp1C4;goto _LL142;_LL141: if(*((int*)_tmp1C3)!= 2)
goto _LL143;_tmp1C5=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1C3)->f1;_LL142:
if(_tmp19F == _tmp1C5)return 1;goto _LL13E;_LL143: if(*((int*)_tmp1C3)!= 0)goto
_LL145;_tmp1C6=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1C3)->f1;_LL144:
return Cyc_Toc_check_const_array(_tmp1C6,(void*)_tmp19F->type);_LL145: if(*((int*)
_tmp1C3)!= 1)goto _LL147;_tmp1C7=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1C3)->f1;
_LL146: if(Cyc_Toc_check_leq_size_var(relns,_tmp19F,_tmp1C7))return 1;goto _LL13E;
_LL147:;_LL148: goto _LL13E;_LL13E:;}}}goto _LL135;_LL13A: if(*((int*)_tmp1BD)!= 3)
goto _LL13C;_tmp1C0=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp1BD)->f1;_LL13B:
return Cyc_Toc_check_const_array(_tmp1C0,(void*)_tmp19F->type);_LL13C:;_LL13D:
goto _LL135;_LL135:;}}}goto _LL120;_LL133:;_LL134: goto _LL120;_LL120:;}goto _LL115;
_LL11E:;_LL11F: goto _LL115;_LL115:;}return 0;}static void*Cyc_Toc_get_c_typ(struct
Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp1C8[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1C9="Missing type in primop ";_tag_dynforward(_tmp1C9,sizeof(char),
_get_zero_arr_size(_tmp1C9,24));}),_tag_dynforward(_tmp1C8,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp1CA[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1CB="Missing type in primop ";_tag_dynforward(_tmp1CB,sizeof(char),
_get_zero_arr_size(_tmp1CB,24));}),_tag_dynforward(_tmp1CA,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp1CC=_cycalloc(
sizeof(*_tmp1CC));_tmp1CC->f1=Cyc_Toc_mt_tq;_tmp1CC->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp1CC;});}static struct _tuple6*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple6*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->f1=0;
_tmp1CD->f2=e;_tmp1CD;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp1CE=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp1CE):
Cyc_Toc_malloc_ptr(_tmp1CE));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp1CE);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp1CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp1CF != 0;_tmp1CF=_tmp1CF->tl){struct _tuple6 _tmp1D1;struct Cyc_List_List*
_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;struct _tuple6*_tmp1D0=(struct _tuple6*)
_tmp1CF->hd;_tmp1D1=*_tmp1D0;_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp1D2 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp1D2->tl != 0)({void*_tmp1D4[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp1D5="multiple designators in array";
_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size(_tmp1D5,30));}),
_tag_dynforward(_tmp1D4,sizeof(void*),0));});{void*_tmp1D6=(void*)_tmp1D2->hd;
void*_tmp1D7=_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D8;_LL14A: if(*((int*)_tmp1D7)!= 0)
goto _LL14C;_tmp1D8=((struct Cyc_Absyn_ArrayElement_struct*)_tmp1D7)->f1;_LL14B:
Cyc_Toc_exp_to_c(nv,_tmp1D8);e_index=_tmp1D8;goto _LL149;_LL14C: if(*((int*)
_tmp1D7)!= 1)goto _LL149;_LL14D:({void*_tmp1D9[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp1DA="field name designators in array";
_tag_dynforward(_tmp1DA,sizeof(char),_get_zero_arr_size(_tmp1DA,32));}),
_tag_dynforward(_tmp1D9,sizeof(void*),0));});_LL149:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp1DB=(void*)_tmp1D3->r;struct Cyc_List_List*
_tmp1DC;struct Cyc_Absyn_Vardecl*_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Exp*
_tmp1DF;int _tmp1E0;void*_tmp1E1;struct Cyc_List_List*_tmp1E2;_LL14F: if(*((int*)
_tmp1DB)!= 28)goto _LL151;_tmp1DC=((struct Cyc_Absyn_Array_e_struct*)_tmp1DB)->f1;
_LL150: s=Cyc_Toc_init_array(nv,lval,_tmp1DC,s);goto _LL14E;_LL151: if(*((int*)
_tmp1DB)!= 29)goto _LL153;_tmp1DD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1DB)->f1;_tmp1DE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DB)->f2;
_tmp1DF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DB)->f3;_tmp1E0=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1DB)->f4;_LL152: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1DD,_tmp1DE,_tmp1DF,_tmp1E0,s,0);goto _LL14E;_LL153: if(*((int*)
_tmp1DB)!= 31)goto _LL155;_tmp1E1=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1DB)->f1;_tmp1E2=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1DB)->f2;_LL154:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1E1,_tmp1E2,s);goto _LL14E;_LL155:;_LL156:
Cyc_Toc_exp_to_c(nv,_tmp1D3);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp1D3,0),s,0);goto _LL14E;_LL14E:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp1E3=vd->name;
void*_tmp1E4=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp1E3,Cyc_Absyn_varb_exp(_tmp1E3,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_Local_b_struct
_tmp1EE;_tmp1EE.tag=3;_tmp1EE.f1=vd;_tmp1EE;});_tmp1ED;}),0));struct _tuple1*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp1E3,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp1E3,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp1E3,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp1E3,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp1E5=(void*)e2->r;struct Cyc_List_List*_tmp1E6;struct Cyc_Absyn_Vardecl*
_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;int _tmp1EA;void*
_tmp1EB;struct Cyc_List_List*_tmp1EC;_LL158: if(*((int*)_tmp1E5)!= 28)goto _LL15A;
_tmp1E6=((struct Cyc_Absyn_Array_e_struct*)_tmp1E5)->f1;_LL159: body=Cyc_Toc_init_array(
nv2,lval,_tmp1E6,Cyc_Toc_skip_stmt_dl());goto _LL157;_LL15A: if(*((int*)_tmp1E5)!= 
29)goto _LL15C;_tmp1E7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1E5)->f1;
_tmp1E8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1E5)->f2;_tmp1E9=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1E5)->f3;_tmp1EA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E5)->f4;_LL15B: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp1E7,_tmp1E8,
_tmp1E9,_tmp1EA,Cyc_Toc_skip_stmt_dl(),0);goto _LL157;_LL15C: if(*((int*)_tmp1E5)
!= 31)goto _LL15E;_tmp1EB=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1E5)->f1;
_tmp1EC=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1E5)->f2;_LL15D: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1EB,_tmp1EC,Cyc_Toc_skip_stmt_dl());goto _LL157;_LL15E:;_LL15F: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL157;_LL157:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp1E4,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp1E3,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp1EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp1EF != 0;_tmp1EF=_tmp1EF->tl){struct _tuple6 _tmp1F1;
struct Cyc_List_List*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct _tuple6*_tmp1F0=(
struct _tuple6*)_tmp1EF->hd;_tmp1F1=*_tmp1F0;_tmp1F2=_tmp1F1.f1;_tmp1F3=_tmp1F1.f2;
if(_tmp1F2 == 0)({void*_tmp1F4[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1F5="empty designator list";
_tag_dynforward(_tmp1F5,sizeof(char),_get_zero_arr_size(_tmp1F5,22));}),
_tag_dynforward(_tmp1F4,sizeof(void*),0));});if(_tmp1F2->tl != 0)({void*_tmp1F6[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1F7="too many designators in anonymous struct";_tag_dynforward(
_tmp1F7,sizeof(char),_get_zero_arr_size(_tmp1F7,41));}),_tag_dynforward(_tmp1F6,
sizeof(void*),0));});{void*_tmp1F8=(void*)_tmp1F2->hd;struct _dynforward_ptr*
_tmp1F9;_LL161: if(*((int*)_tmp1F8)!= 1)goto _LL163;_tmp1F9=((struct Cyc_Absyn_FieldName_struct*)
_tmp1F8)->f1;_LL162: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp1F9,0);{void*_tmp1FA=(void*)_tmp1F3->r;struct Cyc_List_List*_tmp1FB;struct Cyc_Absyn_Vardecl*
_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_Absyn_Exp*_tmp1FE;int _tmp1FF;void*
_tmp200;struct Cyc_List_List*_tmp201;_LL166: if(*((int*)_tmp1FA)!= 28)goto _LL168;
_tmp1FB=((struct Cyc_Absyn_Array_e_struct*)_tmp1FA)->f1;_LL167: s=Cyc_Toc_init_array(
nv,lval,_tmp1FB,s);goto _LL165;_LL168: if(*((int*)_tmp1FA)!= 29)goto _LL16A;_tmp1FC=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1FA)->f1;_tmp1FD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1FA)->f2;_tmp1FE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1FA)->f3;
_tmp1FF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1FA)->f4;_LL169: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1FC,_tmp1FD,_tmp1FE,_tmp1FF,s,0);goto _LL165;_LL16A: if(*((int*)
_tmp1FA)!= 31)goto _LL16C;_tmp200=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1FA)->f1;_tmp201=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1FA)->f2;_LL16B:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp200,_tmp201,s);goto _LL165;_LL16C:;_LL16D:
Cyc_Toc_exp_to_c(nv,_tmp1F3);if(Cyc_Toc_is_poly_field(struct_type,_tmp1F9))
_tmp1F3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp1F3);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1F3,0),0),s,0);goto _LL165;
_LL165:;}goto _LL160;}_LL163:;_LL164:({void*_tmp202[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp203="array designator in struct";_tag_dynforward(_tmp203,sizeof(char),
_get_zero_arr_size(_tmp203,27));}),_tag_dynforward(_tmp202,sizeof(void*),0));});
_LL160:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp204=_new_region("r");struct _RegionHandle*r=& _tmp204;
_push_region(r);{struct Cyc_List_List*_tmp205=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _dynforward_ptr*_tmp206=Cyc_Toc_add_tuple_type(
_tmp205);struct _tuple1*_tmp207=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp208=
Cyc_Absyn_var_exp(_tmp207,0);struct Cyc_Absyn_Stmt*_tmp209=Cyc_Absyn_exp_stmt(
_tmp208,0);struct Cyc_Absyn_Exp*(*_tmp20A)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp20B=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp20B);for(0;_tmp20B != 0;(_tmp20B=
_tmp20B->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp20B->hd;struct
Cyc_Absyn_Exp*lval=_tmp20A(_tmp208,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp20C=(void*)e->r;struct Cyc_List_List*_tmp20D;struct Cyc_Absyn_Vardecl*
_tmp20E;struct Cyc_Absyn_Exp*_tmp20F;struct Cyc_Absyn_Exp*_tmp210;int _tmp211;
_LL16F: if(*((int*)_tmp20C)!= 28)goto _LL171;_tmp20D=((struct Cyc_Absyn_Array_e_struct*)
_tmp20C)->f1;_LL170: _tmp209=Cyc_Toc_init_array(nv,lval,_tmp20D,_tmp209);goto
_LL16E;_LL171: if(*((int*)_tmp20C)!= 29)goto _LL173;_tmp20E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp20C)->f1;_tmp20F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20C)->f2;
_tmp210=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp20C)->f3;_tmp211=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp20C)->f4;_LL172: _tmp209=Cyc_Toc_init_comprehension(
nv,lval,_tmp20E,_tmp20F,_tmp210,_tmp211,_tmp209,0);goto _LL16E;_LL173:;_LL174: Cyc_Toc_exp_to_c(
nv,e);_tmp209=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp20A(
_tmp208,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp209,0);goto _LL16E;_LL16E:;}}}{
struct Cyc_Absyn_Exp*_tmp212=Cyc_Toc_make_struct(nv,_tmp207,Cyc_Absyn_strct(
_tmp206),_tmp209,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp212;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp213=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp214=Cyc_Absyn_var_exp(_tmp213,0);struct Cyc_Absyn_Stmt*
_tmp215=Cyc_Absyn_exp_stmt(_tmp214,0);struct Cyc_Absyn_Exp*(*_tmp216)(struct Cyc_Absyn_Exp*,
struct _dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp217=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp218;void*
_tmp219;_LL176: if(_tmp217 <= (void*)4)goto _LL178;if(*((int*)_tmp217)!= 10)goto
_LL178;_tmp218=((struct Cyc_Absyn_AggrType_struct*)_tmp217)->f1;_tmp219=(void*)
_tmp218.aggr_info;_LL177: ad=Cyc_Absyn_get_known_aggrdecl(_tmp219);goto _LL175;
_LL178:;_LL179:({void*_tmp21A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp21B="init_struct: bad struct type";
_tag_dynforward(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,29));}),
_tag_dynforward(_tmp21A,sizeof(void*),0));});_LL175:;}{struct _RegionHandle
_tmp21C=_new_region("r");struct _RegionHandle*r=& _tmp21C;_push_region(r);{struct
Cyc_List_List*_tmp21D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,dles);for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){struct _tuple6
_tmp21F;struct Cyc_List_List*_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct _tuple6*
_tmp21E=(struct _tuple6*)_tmp21D->hd;_tmp21F=*_tmp21E;_tmp220=_tmp21F.f1;_tmp221=
_tmp21F.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp221->topt))->v);if(_tmp220 == 0)({void*_tmp222[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp223="empty designator list";_tag_dynforward(_tmp223,sizeof(char),
_get_zero_arr_size(_tmp223,22));}),_tag_dynforward(_tmp222,sizeof(void*),0));});
if(_tmp220->tl != 0){struct _tuple1*_tmp224=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp225=Cyc_Absyn_var_exp(_tmp224,0);for(0;_tmp220 != 0;_tmp220=_tmp220->tl){void*
_tmp226=(void*)_tmp220->hd;struct _dynforward_ptr*_tmp227;_LL17B: if(*((int*)
_tmp226)!= 1)goto _LL17D;_tmp227=((struct Cyc_Absyn_FieldName_struct*)_tmp226)->f1;
_LL17C: if(Cyc_Toc_is_poly_field(struct_type,_tmp227))_tmp225=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp225);_tmp215=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp216(_tmp214,_tmp227,0),_tmp225,0),0),_tmp215,0);goto _LL17A;_LL17D:;_LL17E:({
void*_tmp228[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp229="array designator in struct";
_tag_dynforward(_tmp229,sizeof(char),_get_zero_arr_size(_tmp229,27));}),
_tag_dynforward(_tmp228,sizeof(void*),0));});_LL17A:;}Cyc_Toc_exp_to_c(nv,
_tmp221);_tmp215=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp225,_tmp221,0),0),_tmp215,0);}else{void*_tmp22A=(void*)_tmp220->hd;struct
_dynforward_ptr*_tmp22B;_LL180: if(*((int*)_tmp22A)!= 1)goto _LL182;_tmp22B=((
struct Cyc_Absyn_FieldName_struct*)_tmp22A)->f1;_LL181: {struct Cyc_Absyn_Exp*lval=
_tmp216(_tmp214,_tmp22B,0);{void*_tmp22C=(void*)_tmp221->r;struct Cyc_List_List*
_tmp22D;struct Cyc_Absyn_Vardecl*_tmp22E;struct Cyc_Absyn_Exp*_tmp22F;struct Cyc_Absyn_Exp*
_tmp230;int _tmp231;void*_tmp232;struct Cyc_List_List*_tmp233;_LL185: if(*((int*)
_tmp22C)!= 28)goto _LL187;_tmp22D=((struct Cyc_Absyn_Array_e_struct*)_tmp22C)->f1;
_LL186: _tmp215=Cyc_Toc_init_array(nv,lval,_tmp22D,_tmp215);goto _LL184;_LL187: if(*((
int*)_tmp22C)!= 29)goto _LL189;_tmp22E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp22C)->f1;_tmp22F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp22C)->f2;
_tmp230=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp22C)->f3;_tmp231=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp22C)->f4;_LL188: _tmp215=Cyc_Toc_init_comprehension(
nv,lval,_tmp22E,_tmp22F,_tmp230,_tmp231,_tmp215,0);goto _LL184;_LL189: if(*((int*)
_tmp22C)!= 31)goto _LL18B;_tmp232=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp22C)->f1;_tmp233=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp22C)->f2;_LL18A:
_tmp215=Cyc_Toc_init_anon_struct(nv,lval,_tmp232,_tmp233,_tmp215);goto _LL184;
_LL18B:;_LL18C: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp221->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp221);if(Cyc_Toc_is_poly_field(
struct_type,_tmp22B) && !was_ptr_type)_tmp221=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp221);if(has_exists)_tmp221=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp22B)))->type,_tmp221);_tmp215=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp221,0),0),_tmp215,0);goto _LL184;}
_LL184:;}goto _LL17F;}_LL182:;_LL183:({void*_tmp234[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp235="array designator in struct";_tag_dynforward(_tmp235,sizeof(char),
_get_zero_arr_size(_tmp235,27));}),_tag_dynforward(_tmp234,sizeof(void*),0));});
_LL17F:;}}}{struct Cyc_Absyn_Exp*_tmp236=Cyc_Toc_make_struct(nv,_tmp213,Cyc_Absyn_strctq(
tdn),_tmp215,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp236;};
_pop_region(r);}}struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple9*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*
incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp238;_tmp238.tag=5;_tmp238.f1=e1;_tmp238.f2=(void*)incr;_tmp238;});_tmp237;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp239=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp23A;void*_tmp23B;struct Cyc_Absyn_Exp*_tmp23C;struct Cyc_Absyn_Exp*_tmp23D;
struct _dynforward_ptr*_tmp23E;_LL18E: if(*((int*)_tmp239)!= 37)goto _LL190;_tmp23A=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp239)->f1;_LL18F: Cyc_Toc_lvalue_assign_stmt(
_tmp23A,fs,f,f_env);goto _LL18D;_LL190: if(*((int*)_tmp239)!= 15)goto _LL192;
_tmp23B=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp239)->f1;_tmp23C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp239)->f2;_LL191: Cyc_Toc_lvalue_assign(_tmp23C,fs,f,f_env);goto _LL18D;_LL192:
if(*((int*)_tmp239)!= 23)goto _LL194;_tmp23D=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp239)->f1;_tmp23E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp239)->f2;_LL193:(
void*)(e1->r=(void*)((void*)_tmp23D->r));Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F->hd=_tmp23E;
_tmp23F->tl=fs;_tmp23F;}),f,f_env);goto _LL18D;_LL194:;_LL195: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((
void*)(f(e1_copy,f_env))->r));goto _LL18D;}_LL18D:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp240=(void*)s->r;struct Cyc_Absyn_Exp*_tmp241;struct
Cyc_Absyn_Decl*_tmp242;struct Cyc_Absyn_Stmt*_tmp243;struct Cyc_Absyn_Stmt*_tmp244;
_LL197: if(_tmp240 <= (void*)1)goto _LL19D;if(*((int*)_tmp240)!= 0)goto _LL199;
_tmp241=((struct Cyc_Absyn_Exp_s_struct*)_tmp240)->f1;_LL198: Cyc_Toc_lvalue_assign(
_tmp241,fs,f,f_env);goto _LL196;_LL199: if(*((int*)_tmp240)!= 11)goto _LL19B;
_tmp242=((struct Cyc_Absyn_Decl_s_struct*)_tmp240)->f1;_tmp243=((struct Cyc_Absyn_Decl_s_struct*)
_tmp240)->f2;_LL19A: Cyc_Toc_lvalue_assign_stmt(_tmp243,fs,f,f_env);goto _LL196;
_LL19B: if(*((int*)_tmp240)!= 1)goto _LL19D;_tmp244=((struct Cyc_Absyn_Seq_s_struct*)
_tmp240)->f2;_LL19C: Cyc_Toc_lvalue_assign_stmt(_tmp244,fs,f,f_env);goto _LL196;
_LL19D:;_LL19E:({struct Cyc_String_pa_struct _tmp247;_tmp247.tag=0;_tmp247.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_stmt2string(s));{void*
_tmp245[1]={& _tmp247};Cyc_Toc_toc_impos(({const char*_tmp246="lvalue_assign_stmt: %s";
_tag_dynforward(_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,23));}),
_tag_dynforward(_tmp245,sizeof(void*),1));}});_LL196:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp248=_region_malloc(r2,sizeof(*_tmp248));_tmp248->hd=(
void*)f((void*)x->hd,env);_tmp248->tl=0;_tmp248;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp249=_region_malloc(r2,sizeof(*_tmp249));_tmp249->hd=(void*)f((void*)x->hd,
env);_tmp249->tl=0;_tmp249;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple6*_tmp24A=_cycalloc(sizeof(*_tmp24A));_tmp24A->f1=0;_tmp24A->f2=e;_tmp24A;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp24B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp24C;_LL1A0: if(_tmp24B <= (void*)4)goto _LL1A2;if(*((
int*)_tmp24B)!= 4)goto _LL1A2;_tmp24C=((struct Cyc_Absyn_PointerType_struct*)
_tmp24B)->f1;_LL1A1: return _tmp24C;_LL1A2:;_LL1A3:({void*_tmp24D[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp24E="get_ptr_typ: not a pointer!";_tag_dynforward(_tmp24E,sizeof(char),
_get_zero_arr_size(_tmp24E,28));}),_tag_dynforward(_tmp24D,sizeof(void*),0));});
_LL19F:;}static int Cyc_Toc_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dynforward,int*is_dyneither,void**elt_type){void*_tmp24F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp250;void*_tmp251;struct Cyc_Absyn_PtrAtts _tmp252;
struct Cyc_Absyn_Conref*_tmp253;struct Cyc_Absyn_Conref*_tmp254;struct Cyc_Absyn_ArrayInfo
_tmp255;void*_tmp256;struct Cyc_Absyn_Tqual _tmp257;struct Cyc_Absyn_Exp*_tmp258;
struct Cyc_Absyn_Conref*_tmp259;_LL1A5: if(_tmp24F <= (void*)4)goto _LL1A9;if(*((int*)
_tmp24F)!= 4)goto _LL1A7;_tmp250=((struct Cyc_Absyn_PointerType_struct*)_tmp24F)->f1;
_tmp251=(void*)_tmp250.elt_typ;_tmp252=_tmp250.ptr_atts;_tmp253=_tmp252.bounds;
_tmp254=_tmp252.zero_term;_LL1A6: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp254)){*ptr_type=t;*elt_type=_tmp251;{void*_tmp25A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp253);_LL1AC: if((int)_tmp25A != 0)goto _LL1AE;_LL1AD:*is_dyneither=0;*
is_dynforward=1;goto _LL1AB;_LL1AE: if((int)_tmp25A != 1)goto _LL1B0;_LL1AF:*
is_dynforward=0;*is_dyneither=1;goto _LL1AB;_LL1B0:;_LL1B1:*is_dynforward=(*
is_dyneither=0);goto _LL1AB;_LL1AB:;}return 1;}else{return 0;}_LL1A7: if(*((int*)
_tmp24F)!= 7)goto _LL1A9;_tmp255=((struct Cyc_Absyn_ArrayType_struct*)_tmp24F)->f1;
_tmp256=(void*)_tmp255.elt_type;_tmp257=_tmp255.tq;_tmp258=_tmp255.num_elts;
_tmp259=_tmp255.zero_term;_LL1A8: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp259)){*elt_type=_tmp256;*is_dynforward=(*is_dyneither=0);*ptr_type=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B[0]=({
struct Cyc_Absyn_PointerType_struct _tmp25C;_tmp25C.tag=4;_tmp25C.f1=({struct Cyc_Absyn_PtrInfo
_tmp25D;_tmp25D.elt_typ=(void*)_tmp256;_tmp25D.elt_tq=_tmp257;_tmp25D.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp25E;_tmp25E.rgn=(void*)((void*)2);_tmp25E.nullable=
Cyc_Absyn_false_conref;_tmp25E.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp260;_tmp260.tag=0;_tmp260.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp258);
_tmp260;});_tmp25F;}));_tmp25E.zero_term=_tmp259;_tmp25E.ptrloc=0;_tmp25E;});
_tmp25D;});_tmp25C;});_tmp25B;});return 1;}else{return 0;}_LL1A9:;_LL1AA: return 0;
_LL1A4:;}static int Cyc_Toc_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmp261=(void*)
e1->r;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*
_tmp264;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*
_tmp267;_LL1B3: if(*((int*)_tmp261)!= 15)goto _LL1B5;_LL1B4:({struct Cyc_String_pa_struct
_tmp26A;_tmp26A.tag=0;_tmp26A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp268[1]={& _tmp26A};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp269="we have a cast in a lhs:  %s";_tag_dynforward(_tmp269,sizeof(char),
_get_zero_arr_size(_tmp269,29));}),_tag_dynforward(_tmp268,sizeof(void*),1));}});
_LL1B5: if(*((int*)_tmp261)!= 22)goto _LL1B7;_tmp262=((struct Cyc_Absyn_Deref_e_struct*)
_tmp261)->f1;_LL1B6: _tmp263=_tmp262;goto _LL1B8;_LL1B7: if(*((int*)_tmp261)!= 25)
goto _LL1B9;_tmp263=((struct Cyc_Absyn_Subscript_e_struct*)_tmp261)->f1;_LL1B8:
return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp263->topt))->v,
ptr_type,is_dynforward,is_dyneither,elt_type);_LL1B9: if(*((int*)_tmp261)!= 24)
goto _LL1BB;_tmp264=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp261)->f1;_LL1BA:
_tmp265=_tmp264;goto _LL1BC;_LL1BB: if(*((int*)_tmp261)!= 23)goto _LL1BD;_tmp265=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp261)->f1;_LL1BC: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp265->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp26D;_tmp26D.tag=
0;_tmp26D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp26B[1]={& _tmp26D};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp26C="found zero pointer aggregate member assignment: %s";
_tag_dynforward(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,51));}),
_tag_dynforward(_tmp26B,sizeof(void*),1));}});return 0;_LL1BD: if(*((int*)_tmp261)
!= 14)goto _LL1BF;_tmp266=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp261)->f1;
_LL1BE: _tmp267=_tmp266;goto _LL1C0;_LL1BF: if(*((int*)_tmp261)!= 13)goto _LL1C1;
_tmp267=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp261)->f1;_LL1C0: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp267->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp270;_tmp270.tag=
0;_tmp270.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp26E[1]={& _tmp270};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp26F="found zero pointer instantiate/noinstantiate: %s";
_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,49));}),
_tag_dynforward(_tmp26E,sizeof(void*),1));}});return 0;_LL1C1: if(*((int*)_tmp261)
!= 1)goto _LL1C3;_LL1C2: return 0;_LL1C3:;_LL1C4:({struct Cyc_String_pa_struct
_tmp273;_tmp273.tag=0;_tmp273.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp271[1]={& _tmp273};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp272="found bad lhs in is_zero_ptr_deref: %s";_tag_dynforward(_tmp272,sizeof(
char),_get_zero_arr_size(_tmp272,39));}),_tag_dynforward(_tmp271,sizeof(void*),1));}});
_LL1B2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp274=Cyc_Tcutil_compress(t);void*_tmp275;void*_tmp276;void*_tmp277;
void*_tmp278;void*_tmp279;void*_tmp27A;void*_tmp27B;void*_tmp27C;void*_tmp27D;
void*_tmp27E;_LL1C6: if(_tmp274 <= (void*)4)goto _LL1D6;if(*((int*)_tmp274)!= 4)
goto _LL1C8;_LL1C7: res=Cyc_Absyn_null_exp(0);goto _LL1C5;_LL1C8: if(*((int*)_tmp274)
!= 5)goto _LL1CA;_tmp275=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp274)->f1;
_tmp276=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp274)->f2;if((int)_tmp276 != 
0)goto _LL1CA;_LL1C9: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F[0]=({struct Cyc_Absyn_Char_c_struct
_tmp280;_tmp280.tag=0;_tmp280.f1=(void*)_tmp275;_tmp280.f2='\000';_tmp280;});
_tmp27F;}),0);goto _LL1C5;_LL1CA: if(*((int*)_tmp274)!= 5)goto _LL1CC;_tmp277=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp274)->f1;_tmp278=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp274)->f2;if((int)_tmp278 != 1)goto _LL1CC;_LL1CB: res=Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_Short_c_struct*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({
struct Cyc_Absyn_Short_c_struct _tmp282;_tmp282.tag=1;_tmp282.f1=(void*)_tmp277;
_tmp282.f2=0;_tmp282;});_tmp281;}),0);goto _LL1C5;_LL1CC: if(*((int*)_tmp274)!= 12)
goto _LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(*((int*)_tmp274)!= 13)goto _LL1D0;_LL1CF:
_tmp279=(void*)1;goto _LL1D1;_LL1D0: if(*((int*)_tmp274)!= 5)goto _LL1D2;_tmp279=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp274)->f1;_tmp27A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp274)->f2;if((int)_tmp27A != 2)goto _LL1D2;_LL1D1: _tmp27B=_tmp279;goto _LL1D3;
_LL1D2: if(*((int*)_tmp274)!= 5)goto _LL1D4;_tmp27B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp274)->f1;_tmp27C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp274)->f2;if((
int)_tmp27C != 3)goto _LL1D4;_LL1D3: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283[0]=({struct Cyc_Absyn_Int_c_struct
_tmp284;_tmp284.tag=2;_tmp284.f1=(void*)_tmp27B;_tmp284.f2=0;_tmp284;});_tmp283;}),
0);goto _LL1C5;_LL1D4: if(*((int*)_tmp274)!= 5)goto _LL1D6;_tmp27D=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp274)->f1;_tmp27E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp274)->f2;if((int)_tmp27E != 4)goto _LL1D6;_LL1D5: res=Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_LongLong_c_struct*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285[0]=({
struct Cyc_Absyn_LongLong_c_struct _tmp286;_tmp286.tag=3;_tmp286.f1=(void*)_tmp27D;
_tmp286.f2=(long long)0;_tmp286;});_tmp285;}),0);goto _LL1C5;_LL1D6: if((int)
_tmp274 != 1)goto _LL1D8;_LL1D7: goto _LL1D9;_LL1D8: if(_tmp274 <= (void*)4)goto _LL1DA;
if(*((int*)_tmp274)!= 6)goto _LL1DA;_LL1D9: res=Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_Float_c_struct*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287[0]=({
struct Cyc_Absyn_Float_c_struct _tmp288;_tmp288.tag=4;_tmp288.f1=({const char*
_tmp289="0.0";_tag_dynforward(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,4));});
_tmp288;});_tmp287;}),0);goto _LL1C5;_LL1DA:;_LL1DB:({struct Cyc_String_pa_struct
_tmp28C;_tmp28C.tag=0;_tmp28C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp28A[1]={& _tmp28C};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp28B="found non-zero type %s in generate_zero";_tag_dynforward(_tmp28B,
sizeof(char),_get_zero_arr_size(_tmp28B,40));}),_tag_dynforward(_tmp28A,sizeof(
void*),1));}});_LL1C5:;}res->topt=({struct Cyc_Core_Opt*_tmp28D=_cycalloc(sizeof(*
_tmp28D));_tmp28D->v=(void*)t;_tmp28D;});return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dynforward,int is_dyneither,void*
elt_type){int is_fat=is_dynforward  || is_dyneither;void*fat_ptr_type;if(
is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)
2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp28E=Cyc_Toc_typ_to_c(elt_type);
void*_tmp28F=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp290=Cyc_Absyn_cstar_typ(
_tmp28E,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp291=({struct Cyc_Core_Opt*_tmp2CD=
_cycalloc(sizeof(*_tmp2CD));_tmp2CD->v=(void*)_tmp290;_tmp2CD;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp292=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*
_tmp294;struct Cyc_Absyn_Exp*_tmp295;_LL1DD: if(*((int*)_tmp292)!= 22)goto _LL1DF;
_tmp293=((struct Cyc_Absyn_Deref_e_struct*)_tmp292)->f1;_LL1DE: if(!is_fat){
_tmp293=Cyc_Toc_cast_it(fat_ptr_type,_tmp293);_tmp293->topt=({struct Cyc_Core_Opt*
_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->v=(void*)fat_ptr_type;_tmp296;});}
Cyc_Toc_exp_to_c(nv,_tmp293);xinit=_tmp293;goto _LL1DC;_LL1DF: if(*((int*)_tmp292)
!= 25)goto _LL1E1;_tmp294=((struct Cyc_Absyn_Subscript_e_struct*)_tmp292)->f1;
_tmp295=((struct Cyc_Absyn_Subscript_e_struct*)_tmp292)->f2;_LL1E0: if(!is_fat){
_tmp294=Cyc_Toc_cast_it(fat_ptr_type,_tmp294);_tmp294->topt=({struct Cyc_Core_Opt*
_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->v=(void*)fat_ptr_type;_tmp297;});}
Cyc_Toc_exp_to_c(nv,_tmp294);Cyc_Toc_exp_to_c(nv,_tmp295);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp298[3];_tmp298[2]=_tmp295;_tmp298[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp298[0]=_tmp294;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp298,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp299[3];_tmp299[2]=_tmp295;_tmp299[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp299[0]=_tmp294;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp299,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1DC;_LL1E1:;_LL1E2:({void*_tmp29A[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp29B="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,53));}),
_tag_dynforward(_tmp29A,sizeof(void*),0));});_LL1DC:;}{struct _tuple1*_tmp29C=Cyc_Toc_temp_var();
nv=Cyc_Toc_add_varmap(nv,_tmp29C,Cyc_Absyn_var_exp(_tmp29C,0));{struct Cyc_Absyn_Vardecl*
_tmp29D=({struct Cyc_Absyn_Vardecl*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->sc=(
void*)((void*)2);_tmp2CC->name=_tmp29C;_tmp2CC->tq=Cyc_Toc_mt_tq;_tmp2CC->type=(
void*)_tmp28F;_tmp2CC->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp2CC->rgn=0;
_tmp2CC->attributes=0;_tmp2CC->escapes=0;_tmp2CC;});struct Cyc_Absyn_Local_b_struct*
_tmp29E=({struct Cyc_Absyn_Local_b_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));
_tmp2CA[0]=({struct Cyc_Absyn_Local_b_struct _tmp2CB;_tmp2CB.tag=3;_tmp2CB.f1=
_tmp29D;_tmp2CB;});_tmp2CA;});struct Cyc_Absyn_Exp*_tmp29F=Cyc_Absyn_varb_exp(
_tmp29C,(void*)_tmp29E,0);_tmp29F->topt=({struct Cyc_Core_Opt*_tmp2A0=_cycalloc(
sizeof(*_tmp2A0));_tmp2A0->v=(void*)fat_ptr_type;_tmp2A0;});{struct Cyc_Absyn_Exp*
_tmp2A1=Cyc_Absyn_deref_exp(_tmp29F,0);_tmp2A1->topt=({struct Cyc_Core_Opt*
_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->v=(void*)elt_type;_tmp2A2;});Cyc_Toc_exp_to_c(
nv,_tmp2A1);{struct _tuple1*_tmp2A3=Cyc_Toc_temp_var();nv=Cyc_Toc_add_varmap(nv,
_tmp2A3,Cyc_Absyn_var_exp(_tmp2A3,0));{struct Cyc_Absyn_Vardecl*_tmp2A4=({struct
Cyc_Absyn_Vardecl*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9->sc=(void*)((void*)
2);_tmp2C9->name=_tmp2A3;_tmp2C9->tq=Cyc_Toc_mt_tq;_tmp2C9->type=(void*)_tmp28E;
_tmp2C9->initializer=(struct Cyc_Absyn_Exp*)_tmp2A1;_tmp2C9->rgn=0;_tmp2C9->attributes=
0;_tmp2C9->escapes=0;_tmp2C9;});struct Cyc_Absyn_Local_b_struct*_tmp2A5=({struct
Cyc_Absyn_Local_b_struct*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2C8;_tmp2C8.tag=3;_tmp2C8.f1=_tmp2A4;_tmp2C8;});
_tmp2C7;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp2A6=Cyc_Absyn_varb_exp(_tmp2A3,(void*)_tmp2A5,0);_tmp2A6->topt=_tmp2A1->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp2A6,e2,0);z_init->topt=_tmp2A6->topt;}
Cyc_Toc_exp_to_c(nv,z_init);{struct _tuple1*_tmp2A7=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp2A8=({struct Cyc_Absyn_Vardecl*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6->sc=(
void*)((void*)2);_tmp2C6->name=_tmp2A7;_tmp2C6->tq=Cyc_Toc_mt_tq;_tmp2C6->type=(
void*)_tmp28E;_tmp2C6->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp2C6->rgn=0;
_tmp2C6->attributes=0;_tmp2C6->escapes=0;_tmp2C6;});struct Cyc_Absyn_Local_b_struct*
_tmp2A9=({struct Cyc_Absyn_Local_b_struct*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));
_tmp2C4[0]=({struct Cyc_Absyn_Local_b_struct _tmp2C5;_tmp2C5.tag=3;_tmp2C5.f1=
_tmp2A8;_tmp2C5;});_tmp2C4;});nv=Cyc_Toc_add_varmap(nv,_tmp2A7,Cyc_Absyn_var_exp(
_tmp2A7,0));{struct Cyc_Absyn_Exp*_tmp2AA=Cyc_Absyn_varb_exp(_tmp2A3,(void*)
_tmp2A5,0);_tmp2AA->topt=_tmp2A1->topt;{struct Cyc_Absyn_Exp*_tmp2AB=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp2AC=Cyc_Absyn_prim2_exp((void*)5,_tmp2AA,
_tmp2AB,0);_tmp2AC->topt=({struct Cyc_Core_Opt*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));
_tmp2AD->v=(void*)Cyc_Absyn_sint_typ;_tmp2AD;});Cyc_Toc_exp_to_c(nv,_tmp2AC);{
struct Cyc_Absyn_Exp*_tmp2AE=Cyc_Absyn_varb_exp(_tmp2A7,(void*)_tmp2A9,0);_tmp2AE->topt=
_tmp2A1->topt;{struct Cyc_Absyn_Exp*_tmp2AF=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_prim2_exp((void*)6,_tmp2AE,_tmp2AF,0);
_tmp2B0->topt=({struct Cyc_Core_Opt*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->v=(
void*)Cyc_Absyn_sint_typ;_tmp2B1;});Cyc_Toc_exp_to_c(nv,_tmp2B0);{struct Cyc_List_List*
_tmp2B2=({struct Cyc_Absyn_Exp*_tmp2C3[2];_tmp2C3[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp2C3[0]=Cyc_Absyn_varb_exp(_tmp29C,(void*)_tmp29E,0);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp2C3,sizeof(struct Cyc_Absyn_Exp*),
2));});struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*
xsize;if(is_dynforward)xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dynforward_size_e,_tmp2B2,0),_tmp2B3,0);else{xsize=Cyc_Absyn_prim2_exp((
void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2B2,0),_tmp2B3,0);}{
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp2AC,
_tmp2B0,0),0);struct Cyc_Absyn_Stmt*_tmp2B5=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp2B6=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp29C,(void*)_tmp29E,0),Cyc_Toc_curr_sp,0);_tmp2B6=Cyc_Toc_cast_it(
_tmp290,_tmp2B6);{struct Cyc_Absyn_Exp*_tmp2B7=Cyc_Absyn_deref_exp(_tmp2B6,0);
struct Cyc_Absyn_Exp*_tmp2B8=Cyc_Absyn_assign_exp(_tmp2B7,Cyc_Absyn_var_exp(
_tmp2A7,0),0);struct Cyc_Absyn_Stmt*_tmp2B9=Cyc_Absyn_exp_stmt(_tmp2B8,0);_tmp2B9=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp2B4,_tmp2B5,Cyc_Absyn_skip_stmt(
0),0),_tmp2B9,0);_tmp2B9=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp2BA=
_cycalloc(sizeof(*_tmp2BA));_tmp2BA->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=_tmp2A8;_tmp2BC;});_tmp2BB;}));_tmp2BA->loc=0;
_tmp2BA;}),_tmp2B9,0);_tmp2B9=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp2BD=
_cycalloc(sizeof(*_tmp2BD));_tmp2BD->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=_tmp2A4;_tmp2BF;});_tmp2BE;}));_tmp2BD->loc=0;
_tmp2BD;}),_tmp2B9,0);_tmp2B9=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp2C0=
_cycalloc(sizeof(*_tmp2C0));_tmp2C0->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=_tmp29D;_tmp2C2;});_tmp2C1;}));_tmp2C0->loc=0;
_tmp2C0;}),_tmp2B9,0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp2B9,0))->r));}}}}}}}}}}}}}}}
struct _tuple10{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp2CE=(void*)e->r;if(e->topt
== 0)({struct Cyc_String_pa_struct _tmp2D1;_tmp2D1.tag=0;_tmp2D1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp2CF[
1]={& _tmp2D1};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2D0="exp_to_c: no type for %s";_tag_dynforward(_tmp2D0,sizeof(char),
_get_zero_arr_size(_tmp2D0,25));}),_tag_dynforward(_tmp2CF,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp2D2=
_tmp2CE;void*_tmp2D3;struct _tuple1*_tmp2D4;void*_tmp2D5;struct _tuple1*_tmp2D6;
void*_tmp2D7;struct Cyc_List_List*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D9;void*
_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Core_Opt*_tmp2DC;struct Cyc_Absyn_Exp*
_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DF;struct Cyc_Absyn_Exp*
_tmp2E0;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Exp*
_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E5;struct Cyc_Absyn_Exp*
_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E7;struct Cyc_List_List*_tmp2E8;struct Cyc_Absyn_Exp*
_tmp2E9;struct Cyc_List_List*_tmp2EA;struct Cyc_Absyn_VarargCallInfo*_tmp2EB;
struct Cyc_Absyn_Exp*_tmp2EC;struct Cyc_List_List*_tmp2ED;struct Cyc_Absyn_VarargCallInfo*
_tmp2EE;struct Cyc_Absyn_VarargCallInfo _tmp2EF;int _tmp2F0;struct Cyc_List_List*
_tmp2F1;struct Cyc_Absyn_VarargInfo*_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F3;struct Cyc_Absyn_Exp*
_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;struct Cyc_List_List*_tmp2F6;void*_tmp2F7;
void**_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;int _tmp2FA;void*_tmp2FB;struct Cyc_Absyn_Exp*
_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Exp*
_tmp2FF;void*_tmp300;void*_tmp301;void*_tmp302;struct _dynforward_ptr*_tmp303;
void*_tmp304;void*_tmp305;unsigned int _tmp306;struct Cyc_Absyn_Exp*_tmp307;struct
Cyc_Absyn_Exp*_tmp308;struct _dynforward_ptr*_tmp309;struct Cyc_Absyn_Exp*_tmp30A;
struct _dynforward_ptr*_tmp30B;struct Cyc_Absyn_Exp*_tmp30C;struct Cyc_Absyn_Exp*
_tmp30D;struct Cyc_List_List*_tmp30E;struct Cyc_List_List*_tmp30F;struct Cyc_Absyn_Vardecl*
_tmp310;struct Cyc_Absyn_Exp*_tmp311;struct Cyc_Absyn_Exp*_tmp312;int _tmp313;
struct _tuple1*_tmp314;struct Cyc_List_List*_tmp315;struct Cyc_List_List*_tmp316;
struct Cyc_Absyn_Aggrdecl*_tmp317;void*_tmp318;struct Cyc_List_List*_tmp319;struct
Cyc_List_List*_tmp31A;struct Cyc_Absyn_Tuniondecl*_tmp31B;struct Cyc_Absyn_Tunionfield*
_tmp31C;struct Cyc_List_List*_tmp31D;struct Cyc_Absyn_Tuniondecl*_tmp31E;struct Cyc_Absyn_Tunionfield*
_tmp31F;struct Cyc_Absyn_MallocInfo _tmp320;int _tmp321;struct Cyc_Absyn_Exp*_tmp322;
void**_tmp323;struct Cyc_Absyn_Exp*_tmp324;int _tmp325;struct Cyc_Absyn_Stmt*
_tmp326;_LL1E4: if(*((int*)_tmp2D2)!= 0)goto _LL1E6;_tmp2D3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2D2)->f1;if((int)_tmp2D3 != 0)goto _LL1E6;_LL1E5: {struct Cyc_Absyn_Exp*_tmp327=
Cyc_Absyn_uint_exp(0,0);int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only)){if(nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(
old_typ,_tmp327,_tmp327,forward_only))->r));else{if(forward_only)(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp328[3];_tmp328[2]=_tmp327;_tmp328[1]=_tmp327;_tmp328[0]=_tmp327;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp328,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp329[3];_tmp329[2]=_tmp327;_tmp329[1]=_tmp327;_tmp329[0]=
_tmp327;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp329,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}}else{(
void*)(e->r=(void*)((void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL1E3;}
_LL1E6: if(*((int*)_tmp2D2)!= 0)goto _LL1E8;_LL1E7: goto _LL1E3;_LL1E8: if(*((int*)
_tmp2D2)!= 1)goto _LL1EA;_tmp2D4=((struct Cyc_Absyn_Var_e_struct*)_tmp2D2)->f1;
_tmp2D5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D2)->f2;_LL1E9:{struct
_handler_cons _tmp32A;_push_handler(& _tmp32A);{int _tmp32C=0;if(setjmp(_tmp32A.handler))
_tmp32C=1;if(!_tmp32C){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp2D4))->r));;
_pop_handler();}else{void*_tmp32B=(void*)_exn_thrown;void*_tmp32E=_tmp32B;_LL239:
if(_tmp32E != Cyc_Dict_Absent)goto _LL23B;_LL23A:({struct Cyc_String_pa_struct
_tmp331;_tmp331.tag=0;_tmp331.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp2D4));{void*_tmp32F[1]={& _tmp331};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp330="Can't find %s in exp_to_c, Var\n";_tag_dynforward(_tmp330,sizeof(char),
_get_zero_arr_size(_tmp330,32));}),_tag_dynforward(_tmp32F,sizeof(void*),1));}});
_LL23B:;_LL23C:(void)_throw(_tmp32E);_LL238:;}}}goto _LL1E3;_LL1EA: if(*((int*)
_tmp2D2)!= 2)goto _LL1EC;_tmp2D6=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp2D2)->f1;
_LL1EB:({void*_tmp332[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp333="unknownid";
_tag_dynforward(_tmp333,sizeof(char),_get_zero_arr_size(_tmp333,10));}),
_tag_dynforward(_tmp332,sizeof(void*),0));});_LL1EC: if(*((int*)_tmp2D2)!= 3)goto
_LL1EE;_tmp2D7=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2D2)->f1;_tmp2D8=((
struct Cyc_Absyn_Primop_e_struct*)_tmp2D2)->f2;_LL1ED: {struct Cyc_List_List*
_tmp334=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp2D8);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp2D8);{void*_tmp335=_tmp2D7;_LL23E: if((int)_tmp335 != 19)
goto _LL240;_LL23F: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D8))->hd;{void*_tmp336=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp337;struct Cyc_Absyn_Exp*
_tmp338;struct Cyc_Absyn_PtrInfo _tmp339;void*_tmp33A;struct Cyc_Absyn_PtrAtts
_tmp33B;struct Cyc_Absyn_Conref*_tmp33C;struct Cyc_Absyn_Conref*_tmp33D;struct Cyc_Absyn_Conref*
_tmp33E;_LL253: if(_tmp336 <= (void*)4)goto _LL257;if(*((int*)_tmp336)!= 7)goto
_LL255;_tmp337=((struct Cyc_Absyn_ArrayType_struct*)_tmp336)->f1;_tmp338=_tmp337.num_elts;
_LL254:(void*)(e->r=(void*)((void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp338))->r));
goto _LL252;_LL255: if(*((int*)_tmp336)!= 4)goto _LL257;_tmp339=((struct Cyc_Absyn_PointerType_struct*)
_tmp336)->f1;_tmp33A=(void*)_tmp339.elt_typ;_tmp33B=_tmp339.ptr_atts;_tmp33C=
_tmp33B.nullable;_tmp33D=_tmp33B.bounds;_tmp33E=_tmp33B.zero_term;_LL256:{void*
_tmp33F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp33D);struct Cyc_Absyn_Exp*
_tmp340;_LL25A: if((int)_tmp33F != 0)goto _LL25C;_LL25B:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,({struct Cyc_Absyn_Exp*
_tmp341[2];_tmp341[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp33A),0);
_tmp341[0]=(struct Cyc_Absyn_Exp*)_tmp2D8->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp341,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL259;_LL25C: if((int)_tmp33F != 1)goto _LL25E;_LL25D:(void*)(
e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,({struct
Cyc_Absyn_Exp*_tmp342[2];_tmp342[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp33A),0);_tmp342[0]=(struct Cyc_Absyn_Exp*)_tmp2D8->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp342,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL259;_LL25E: if(_tmp33F <= (void*)2)goto _LL260;if(*((int*)
_tmp33F)!= 0)goto _LL260;_tmp340=((struct Cyc_Absyn_Upper_b_struct*)_tmp33F)->f1;
_LL25F: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp33E))(
void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp343[2];_tmp343[1]=_tmp340;_tmp343[0]=(struct Cyc_Absyn_Exp*)
_tmp2D8->hd;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp343,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));else{if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp33C))(void*)(e->r=(
void*)((void*)(Cyc_Absyn_conditional_exp(arg,_tmp340,Cyc_Absyn_uint_exp(0,0),0))->r));
else{(void*)(e->r=(void*)((void*)_tmp340->r));goto _LL259;}}goto _LL259;_LL260: if(
_tmp33F <= (void*)2)goto _LL259;if(*((int*)_tmp33F)!= 1)goto _LL259;_LL261:({void*
_tmp344[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp345="toc: size of AbsUpper_b";_tag_dynforward(_tmp345,sizeof(char),
_get_zero_arr_size(_tmp345,24));}),_tag_dynforward(_tmp344,sizeof(void*),0));});
_LL259:;}goto _LL252;_LL257:;_LL258:({struct Cyc_String_pa_struct _tmp349;_tmp349.tag=
0;_tmp349.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct
_tmp348;_tmp348.tag=0;_tmp348.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{
void*_tmp346[2]={& _tmp348,& _tmp349};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp347="size primop applied to non-array %s (%s)";
_tag_dynforward(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,41));}),
_tag_dynforward(_tmp346,sizeof(void*),2));}}});_LL252:;}goto _LL23D;}_LL240: if((
int)_tmp335 != 0)goto _LL242;_LL241:{void*_tmp34A=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp334))->hd);struct Cyc_Absyn_PtrInfo _tmp34B;
void*_tmp34C;struct Cyc_Absyn_PtrAtts _tmp34D;struct Cyc_Absyn_Conref*_tmp34E;
struct Cyc_Absyn_Conref*_tmp34F;_LL263: if(_tmp34A <= (void*)4)goto _LL265;if(*((int*)
_tmp34A)!= 4)goto _LL265;_tmp34B=((struct Cyc_Absyn_PointerType_struct*)_tmp34A)->f1;
_tmp34C=(void*)_tmp34B.elt_typ;_tmp34D=_tmp34B.ptr_atts;_tmp34E=_tmp34D.bounds;
_tmp34F=_tmp34D.zero_term;_LL264:{void*_tmp350=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp34E);struct Cyc_Absyn_Exp*_tmp351;_LL268: if((int)_tmp350 != 0)goto _LL26A;
_LL269: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D8))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2D8->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp352[3];_tmp352[2]=e2;
_tmp352[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp34C),0);_tmp352[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp352,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));goto _LL267;}_LL26A: if((int)
_tmp350 != 1)goto _LL26C;_LL26B: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2D8))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2D8->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp353[
3];_tmp353[2]=e2;_tmp353[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp34C),0);
_tmp353[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp353,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));goto _LL267;}
_LL26C: if(_tmp350 <= (void*)2)goto _LL26E;if(*((int*)_tmp350)!= 0)goto _LL26E;
_tmp351=((struct Cyc_Absyn_Upper_b_struct*)_tmp350)->f1;_LL26D: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp34F)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2D8))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2D8->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp354[3];_tmp354[2]=e2;_tmp354[1]=_tmp351;_tmp354[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp354,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL267;_LL26E: if(
_tmp350 <= (void*)2)goto _LL267;if(*((int*)_tmp350)!= 1)goto _LL267;_LL26F:({void*
_tmp355[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp356="toc: plus on AbsUpper_b";_tag_dynforward(_tmp356,sizeof(char),
_get_zero_arr_size(_tmp356,24));}),_tag_dynforward(_tmp355,sizeof(void*),0));});
_LL267:;}goto _LL262;_LL265:;_LL266: goto _LL262;_LL262:;}goto _LL23D;_LL242: if((int)
_tmp335 != 2)goto _LL244;_LL243: {void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp334))->hd,& elt_typ,& forward_only)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2D8))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D8->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp334->tl))->hd,& forward_only)){(void*)(
e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),
Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e->r=(void*)((
void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{if(forward_only)({void*_tmp357[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp358="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp358,sizeof(char),_get_zero_arr_size(_tmp358,50));}),
_tag_dynforward(_tmp357,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp359[3];_tmp359[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp359[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp359[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp359,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto
_LL23D;}_LL244: if((int)_tmp335 != 5)goto _LL246;_LL245: goto _LL247;_LL246: if((int)
_tmp335 != 6)goto _LL248;_LL247: goto _LL249;_LL248: if((int)_tmp335 != 7)goto _LL24A;
_LL249: goto _LL24B;_LL24A: if((int)_tmp335 != 9)goto _LL24C;_LL24B: goto _LL24D;_LL24C:
if((int)_tmp335 != 8)goto _LL24E;_LL24D: goto _LL24F;_LL24E: if((int)_tmp335 != 10)
goto _LL250;_LL24F: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D8))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2D8->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp334))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp334->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));goto _LL23D;}_LL250:;_LL251: goto _LL23D;
_LL23D:;}goto _LL1E3;}_LL1EE: if(*((int*)_tmp2D2)!= 5)goto _LL1F0;_tmp2D9=((struct
Cyc_Absyn_Increment_e_struct*)_tmp2D2)->f1;_tmp2DA=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp2D2)->f2;_LL1EF: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2D9->topt))->v;void*ptr_type=(void*)0;void*elt_type=(void*)0;int
is_dynforward=0;int is_dyneither=0;struct _dynforward_ptr incr_str=({const char*
_tmp365="increment";_tag_dynforward(_tmp365,sizeof(char),_get_zero_arr_size(
_tmp365,10));});if(_tmp2DA == (void*)2  || _tmp2DA == (void*)3)incr_str=({const char*
_tmp35A="decrement";_tag_dynforward(_tmp35A,sizeof(char),_get_zero_arr_size(
_tmp35A,10));});if(Cyc_Toc_is_zero_ptr_deref(_tmp2D9,& ptr_type,& is_dynforward,&
is_dyneither,& elt_type)){({struct Cyc_String_pa_struct _tmp35D;_tmp35D.tag=0;
_tmp35D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)incr_str);{void*
_tmp35B[1]={& _tmp35D};Cyc_Tcutil_terr(e->loc,({const char*_tmp35C="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp35C,sizeof(char),_get_zero_arr_size(_tmp35C,74));}),
_tag_dynforward(_tmp35B,sizeof(void*),1));}});({void*_tmp35E[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp35F="in-place inc on zero-term";_tag_dynforward(_tmp35F,sizeof(char),
_get_zero_arr_size(_tmp35F,26));}),_tag_dynforward(_tmp35E,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp2D9);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp2DA == (void*)3  || _tmp2DA == (void*)2)({void*_tmp360[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp361="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp361,sizeof(char),_get_zero_arr_size(_tmp361,43));}),
_tag_dynforward(_tmp360,sizeof(void*),0));});fn_e=_tmp2DA == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp2DA == (void*)1  || _tmp2DA
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp2DA == (void*)2  || _tmp2DA == (void*)3)change=- 1;}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(fn_e,({struct Cyc_Absyn_Exp*_tmp362[3];_tmp362[2]=Cyc_Absyn_signed_int_exp(
change,0);_tmp362[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp362[0]=Cyc_Absyn_address_exp(_tmp2D9,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp362,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*_tmp363=_tmp2DA == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e:
Cyc_Toc__zero_arr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp363,({struct Cyc_Absyn_Exp*_tmp364[2];_tmp364[1]=Cyc_Absyn_signed_int_exp(1,0);
_tmp364[0]=_tmp2D9;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp364,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}else{if(
elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp2D9)){Cyc_Toc_lvalue_assign(
_tmp2D9,0,Cyc_Toc_incr_lvalue,_tmp2DA);(void*)(e->r=(void*)((void*)_tmp2D9->r));}}}
goto _LL1E3;}}_LL1F0: if(*((int*)_tmp2D2)!= 4)goto _LL1F2;_tmp2DB=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp2D2)->f1;_tmp2DC=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp2D2)->f2;_tmp2DD=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp2D2)->f3;_LL1F1: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Toc_is_zero_ptr_deref(
_tmp2DB,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp2DB,_tmp2DC,_tmp2DD,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp2DB);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2DB->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2DD->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp2DB);
Cyc_Toc_exp_to_c(nv,_tmp2DD);{int done=0;if(_tmp2DC != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp366=(void*)_tmp2DC->v;_LL271:
if((int)_tmp366 != 0)goto _LL273;_LL272: change=_tmp2DD;goto _LL270;_LL273: if((int)
_tmp366 != 2)goto _LL275;_LL274: if(forward_only)({void*_tmp367[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp368="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp368,sizeof(char),_get_zero_arr_size(_tmp368,46));}),
_tag_dynforward(_tmp367,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp2DD,0);goto _LL270;_LL275:;_LL276:({void*_tmp369[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp36A="bad t ? pointer arithmetic";_tag_dynforward(_tmp36A,sizeof(char),
_get_zero_arr_size(_tmp36A,27));}),_tag_dynforward(_tmp369,sizeof(void*),0));});
_LL270:;}done=1;{struct Cyc_Absyn_Exp*_tmp36B=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp36B,({struct Cyc_Absyn_Exp*_tmp36C[3];_tmp36C[2]=change;_tmp36C[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp36C[0]=Cyc_Absyn_address_exp(_tmp2DB,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp36C,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp36D=(void*)_tmp2DC->v;_LL278: if((int)_tmp36D != 0)
goto _LL27A;_LL279: done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp36E[2];_tmp36E[1]=_tmp2DD;_tmp36E[0]=_tmp2DB;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp36E,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL277;_LL27A:;_LL27B:({void*_tmp36F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp370="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp370,sizeof(
char),_get_zero_arr_size(_tmp370,39));}),_tag_dynforward(_tmp36F,sizeof(void*),0));});
_LL277:;}}}if(!done){if(e1_poly)(void*)(_tmp2DD->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp2DD->r,0)))->r));if(!Cyc_Absyn_is_lvalue(
_tmp2DB)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,struct _tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(
_tmp2DB,0,Cyc_Toc_assignop_lvalue,({struct _tuple9*_tmp371=_cycalloc(sizeof(
struct _tuple9)* 1);_tmp371[0]=({struct _tuple9 _tmp372;_tmp372.f1=_tmp2DC;_tmp372.f2=
_tmp2DD;_tmp372;});_tmp371;}));(void*)(e->r=(void*)((void*)_tmp2DB->r));}}goto
_LL1E3;}}}_LL1F2: if(*((int*)_tmp2D2)!= 6)goto _LL1F4;_tmp2DE=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2D2)->f1;_tmp2DF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2D2)->f2;
_tmp2E0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2D2)->f3;_LL1F3: Cyc_Toc_exp_to_c(
nv,_tmp2DE);Cyc_Toc_exp_to_c(nv,_tmp2DF);Cyc_Toc_exp_to_c(nv,_tmp2E0);goto _LL1E3;
_LL1F4: if(*((int*)_tmp2D2)!= 7)goto _LL1F6;_tmp2E1=((struct Cyc_Absyn_And_e_struct*)
_tmp2D2)->f1;_tmp2E2=((struct Cyc_Absyn_And_e_struct*)_tmp2D2)->f2;_LL1F5: Cyc_Toc_exp_to_c(
nv,_tmp2E1);Cyc_Toc_exp_to_c(nv,_tmp2E2);goto _LL1E3;_LL1F6: if(*((int*)_tmp2D2)!= 
8)goto _LL1F8;_tmp2E3=((struct Cyc_Absyn_Or_e_struct*)_tmp2D2)->f1;_tmp2E4=((
struct Cyc_Absyn_Or_e_struct*)_tmp2D2)->f2;_LL1F7: Cyc_Toc_exp_to_c(nv,_tmp2E3);
Cyc_Toc_exp_to_c(nv,_tmp2E4);goto _LL1E3;_LL1F8: if(*((int*)_tmp2D2)!= 9)goto
_LL1FA;_tmp2E5=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2D2)->f1;_tmp2E6=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp2D2)->f2;_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp2E5);Cyc_Toc_exp_to_c(
nv,_tmp2E6);goto _LL1E3;_LL1FA: if(*((int*)_tmp2D2)!= 10)goto _LL1FC;_tmp2E7=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2D2)->f1;_tmp2E8=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp2D2)->f2;_LL1FB: _tmp2E9=_tmp2E7;_tmp2EA=_tmp2E8;goto _LL1FD;_LL1FC: if(*((int*)
_tmp2D2)!= 11)goto _LL1FE;_tmp2E9=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2D2)->f1;
_tmp2EA=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2D2)->f2;_tmp2EB=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2D2)->f3;if(_tmp2EB != 0)goto _LL1FE;_LL1FD: Cyc_Toc_exp_to_c(nv,_tmp2E9);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2EA);goto _LL1E3;
_LL1FE: if(*((int*)_tmp2D2)!= 11)goto _LL200;_tmp2EC=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2D2)->f1;_tmp2ED=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2D2)->f2;_tmp2EE=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp2D2)->f3;if(_tmp2EE == 0)goto _LL200;_tmp2EF=*
_tmp2EE;_tmp2F0=_tmp2EF.num_varargs;_tmp2F1=_tmp2EF.injectors;_tmp2F2=_tmp2EF.vai;
_LL1FF:{struct _RegionHandle _tmp373=_new_region("r");struct _RegionHandle*r=&
_tmp373;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp2F0,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp2F2->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2ED);int num_normargs=num_args - _tmp2F0;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp2ED=_tmp2ED->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2ED))->hd);
new_args=({struct Cyc_List_List*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->hd=(
struct Cyc_Absyn_Exp*)_tmp2ED->hd;_tmp374->tl=new_args;_tmp374;});}}new_args=({
struct Cyc_List_List*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp376[3];_tmp376[2]=
num_varargs_exp;_tmp376[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp376[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp376,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp375->tl=
new_args;_tmp375;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp2EC);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp2EC,new_args,0),0);if(_tmp2F2->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp377=Cyc_Tcutil_compress((void*)_tmp2F2->type);
struct Cyc_Absyn_TunionInfo _tmp378;void*_tmp379;struct Cyc_Absyn_Tuniondecl**
_tmp37A;struct Cyc_Absyn_Tuniondecl*_tmp37B;_LL27D: if(_tmp377 <= (void*)4)goto
_LL27F;if(*((int*)_tmp377)!= 2)goto _LL27F;_tmp378=((struct Cyc_Absyn_TunionType_struct*)
_tmp377)->f1;_tmp379=(void*)_tmp378.tunion_info;if(*((int*)_tmp379)!= 1)goto
_LL27F;_tmp37A=((struct Cyc_Absyn_KnownTunion_struct*)_tmp379)->f1;_tmp37B=*
_tmp37A;_LL27E: tud=_tmp37B;goto _LL27C;_LL27F:;_LL280:({void*_tmp37C[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp37D="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp37D,
sizeof(char),_get_zero_arr_size(_tmp37D,42));}),_tag_dynforward(_tmp37C,sizeof(
void*),0));});_LL27C:;}{struct _dynforward_ptr vs=({unsigned int _tmp385=(
unsigned int)_tmp2F0;struct _tuple1**_tmp386=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp385));struct _dynforward_ptr _tmp388=
_tag_dynforward(_tmp386,sizeof(struct _tuple1*),_tmp385);{unsigned int _tmp387=
_tmp385;unsigned int i;for(i=0;i < _tmp387;i ++){_tmp386[i]=Cyc_Toc_temp_var();}}
_tmp388;});struct Cyc_List_List*_tmp37E=0;{int i=_tmp2F0 - 1;for(0;i >= 0;-- i){
_tmp37E=({struct Cyc_List_List*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp37F->tl=
_tmp37E;_tmp37F;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp37E,0),s,0);{int i=0;for(0;_tmp2ED != 0;(((
_tmp2ED=_tmp2ED->tl,_tmp2F1=_tmp2F1->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp2ED->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp381;struct _tuple1*
_tmp382;struct Cyc_List_List*_tmp383;struct Cyc_Absyn_Tunionfield*_tmp380=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp2F1))->hd;_tmp381=*
_tmp380;_tmp382=_tmp381.name;_tmp383=_tmp381.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp383))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp382,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp382,({const char*_tmp384="_struct";
_tag_dynforward(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp2ED != 0;(_tmp2ED=_tmp2ED->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp2ED->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp2ED->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL1E3;_LL200: if(*((
int*)_tmp2D2)!= 12)goto _LL202;_tmp2F3=((struct Cyc_Absyn_Throw_e_struct*)_tmp2D2)->f1;
_LL201: Cyc_Toc_exp_to_c(nv,_tmp2F3);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp2F3),0))->r));goto _LL1E3;
_LL202: if(*((int*)_tmp2D2)!= 13)goto _LL204;_tmp2F4=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp2D2)->f1;_LL203: Cyc_Toc_exp_to_c(nv,_tmp2F4);goto _LL1E3;_LL204: if(*((int*)
_tmp2D2)!= 14)goto _LL206;_tmp2F5=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2D2)->f1;
_tmp2F6=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2D2)->f2;_LL205: Cyc_Toc_exp_to_c(
nv,_tmp2F5);for(0;_tmp2F6 != 0;_tmp2F6=_tmp2F6->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp2F6->hd);if(k != (void*)4  && k != (void*)3){{void*_tmp389=Cyc_Tcutil_compress((
void*)_tmp2F6->hd);_LL282: if(_tmp389 <= (void*)4)goto _LL286;if(*((int*)_tmp389)!= 
1)goto _LL284;_LL283: goto _LL285;_LL284: if(*((int*)_tmp389)!= 2)goto _LL286;_LL285:
continue;_LL286:;_LL287:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2F5,
0))->r));goto _LL281;_LL281:;}break;}}goto _LL1E3;_LL206: if(*((int*)_tmp2D2)!= 15)
goto _LL208;_tmp2F7=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2D2)->f1;_tmp2F8=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2D2)->f1);_tmp2F9=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2D2)->f2;_tmp2FA=((struct Cyc_Absyn_Cast_e_struct*)
_tmp2D2)->f3;_tmp2FB=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2D2)->f4;_LL207: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2F9->topt))->v;void*
new_typ=*_tmp2F8;*_tmp2F8=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp2F9);{
struct _tuple0 _tmp38B=({struct _tuple0 _tmp38A;_tmp38A.f1=Cyc_Tcutil_compress(
old_t2);_tmp38A.f2=Cyc_Tcutil_compress(new_typ);_tmp38A;});void*_tmp38C;struct
Cyc_Absyn_PtrInfo _tmp38D;void*_tmp38E;struct Cyc_Absyn_PtrInfo _tmp38F;void*
_tmp390;struct Cyc_Absyn_PtrInfo _tmp391;void*_tmp392;_LL289: _tmp38C=_tmp38B.f1;
if(_tmp38C <= (void*)4)goto _LL28B;if(*((int*)_tmp38C)!= 4)goto _LL28B;_tmp38D=((
struct Cyc_Absyn_PointerType_struct*)_tmp38C)->f1;_tmp38E=_tmp38B.f2;if(_tmp38E <= (
void*)4)goto _LL28B;if(*((int*)_tmp38E)!= 4)goto _LL28B;_tmp38F=((struct Cyc_Absyn_PointerType_struct*)
_tmp38E)->f1;_LL28A: {int _tmp393=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp38D.ptr_atts).nullable);int _tmp394=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp38F.ptr_atts).nullable);void*_tmp395=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp38D.ptr_atts).bounds);void*_tmp396=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp38F.ptr_atts).bounds);int _tmp397=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp38D.ptr_atts).zero_term);int
_tmp398=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp38F.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp39A=({struct _tuple0
_tmp399;_tmp399.f1=_tmp395;_tmp399.f2=_tmp396;_tmp399;});void*_tmp39B;struct Cyc_Absyn_Exp*
_tmp39C;void*_tmp39D;struct Cyc_Absyn_Exp*_tmp39E;void*_tmp39F;struct Cyc_Absyn_Exp*
_tmp3A0;void*_tmp3A1;void*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A3;void*_tmp3A4;void*
_tmp3A5;void*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A7;void*_tmp3A8;void*_tmp3A9;
struct Cyc_Absyn_Exp*_tmp3AA;void*_tmp3AB;void*_tmp3AC;void*_tmp3AD;void*_tmp3AE;
void*_tmp3AF;void*_tmp3B0;void*_tmp3B1;void*_tmp3B2;void*_tmp3B3;void*_tmp3B4;
void*_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;_LL290: _tmp39B=_tmp39A.f1;
if(_tmp39B <= (void*)2)goto _LL292;if(*((int*)_tmp39B)!= 0)goto _LL292;_tmp39C=((
struct Cyc_Absyn_Upper_b_struct*)_tmp39B)->f1;_tmp39D=_tmp39A.f2;if(_tmp39D <= (
void*)2)goto _LL292;if(*((int*)_tmp39D)!= 0)goto _LL292;_tmp39E=((struct Cyc_Absyn_Upper_b_struct*)
_tmp39D)->f1;_LL291: if(_tmp393  && !_tmp394){if(nv->toplevel)({void*_tmp3B9[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp3BA="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp3BA,sizeof(char),_get_zero_arr_size(_tmp3BA,44));}),_tag_dynforward(_tmp3B9,
sizeof(void*),0));});if(_tmp2FB != (void*)2)({struct Cyc_String_pa_struct _tmp3BD;
_tmp3BD.tag=0;_tmp3BD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp3BB[1]={& _tmp3BD};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3BC="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp3BC,sizeof(char),_get_zero_arr_size(_tmp3BC,41));}),
_tag_dynforward(_tmp3BB,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp2F9);if(do_null_check){if(!_tmp2FA)({void*_tmp3BE[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp3BF="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp3BF,sizeof(char),_get_zero_arr_size(_tmp3BF,58));}),
_tag_dynforward(_tmp3BE,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2F8,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp3C0=
_cycalloc(sizeof(*_tmp3C0));_tmp3C0->hd=_tmp2F9;_tmp3C0->tl=0;_tmp3C0;}),0)))->r));}}}
goto _LL28F;_LL292: _tmp39F=_tmp39A.f1;if(_tmp39F <= (void*)2)goto _LL294;if(*((int*)
_tmp39F)!= 0)goto _LL294;_tmp3A0=((struct Cyc_Absyn_Upper_b_struct*)_tmp39F)->f1;
_tmp3A1=_tmp39A.f2;if((int)_tmp3A1 != 0)goto _LL294;_LL293: p2_forward_only=1;
_tmp3A3=_tmp3A0;goto _LL295;_LL294: _tmp3A2=_tmp39A.f1;if(_tmp3A2 <= (void*)2)goto
_LL296;if(*((int*)_tmp3A2)!= 0)goto _LL296;_tmp3A3=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3A2)->f1;_tmp3A4=_tmp39A.f2;if((int)_tmp3A4 != 1)goto _LL296;_LL295: if(_tmp2FB
== (void*)2)({struct Cyc_String_pa_struct _tmp3C3;_tmp3C3.tag=0;_tmp3C3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp3C1[
1]={& _tmp3C3};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3C2="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp3C2,sizeof(char),_get_zero_arr_size(_tmp3C2,44));}),_tag_dynforward(_tmp3C1,
sizeof(void*),1));}});if(nv->toplevel){if((_tmp397  && !(_tmp38F.elt_tq).real_const)
 && !_tmp398)_tmp3A3=Cyc_Absyn_prim2_exp((void*)2,_tmp3A3,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3A3,
_tmp2F9,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp3C4=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp397){struct _tuple1*
_tmp3C5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3C6=Cyc_Absyn_var_exp(_tmp3C5,
0);struct Cyc_Absyn_Exp*_tmp3C7=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp3CC[2];_tmp3CC[1]=_tmp3A3;_tmp3CC[0]=_tmp3C6;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3CC,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp398  && !(_tmp38F.elt_tq).real_const)_tmp3C7=Cyc_Absyn_prim2_exp((
void*)2,_tmp3C7,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp3C8=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp38F.elt_typ),0);struct Cyc_Absyn_Exp*_tmp3C9=Cyc_Absyn_fncall_exp(
_tmp3C4,({struct Cyc_Absyn_Exp*_tmp3CB[3];_tmp3CB[2]=_tmp3C7;_tmp3CB[1]=_tmp3C8;
_tmp3CB[0]=_tmp3C6;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3CB,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp3CA=Cyc_Absyn_exp_stmt(_tmp3C9,0);_tmp3CA=Cyc_Absyn_declare_stmt(_tmp3C5,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp2F9,_tmp3CA,0);(void*)(e->r=(void*)((void*)(
Cyc_Absyn_stmt_exp(_tmp3CA,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp3C4,({struct Cyc_Absyn_Exp*_tmp3CD[3];_tmp3CD[2]=_tmp3A3;_tmp3CD[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp38F.elt_typ),0);_tmp3CD[0]=_tmp2F9;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3CD,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}}goto _LL28F;_LL296: _tmp3A5=_tmp39A.f1;if((int)_tmp3A5 != 0)goto
_LL298;_tmp3A6=_tmp39A.f2;if(_tmp3A6 <= (void*)2)goto _LL298;if(*((int*)_tmp3A6)!= 
0)goto _LL298;_tmp3A7=((struct Cyc_Absyn_Upper_b_struct*)_tmp3A6)->f1;_LL297:
p1_forward_only=1;_tmp3AA=_tmp3A7;goto _LL299;_LL298: _tmp3A8=_tmp39A.f1;if((int)
_tmp3A8 != 1)goto _LL29A;_tmp3A9=_tmp39A.f2;if(_tmp3A9 <= (void*)2)goto _LL29A;if(*((
int*)_tmp3A9)!= 0)goto _LL29A;_tmp3AA=((struct Cyc_Absyn_Upper_b_struct*)_tmp3A9)->f1;
_LL299: if(nv->toplevel)({void*_tmp3CE[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3CF="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp3CF,sizeof(char),_get_zero_arr_size(_tmp3CF,45));}),
_tag_dynforward(_tmp3CE,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp3D0=
_tmp3AA;if(_tmp397  && !_tmp398)_tmp3D0=Cyc_Absyn_add_exp(_tmp3AA,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp3D1=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp3D2=Cyc_Absyn_fncall_exp(
_tmp3D1,({struct Cyc_Absyn_Exp*_tmp3D4[3];_tmp3D4[2]=_tmp3D0;_tmp3D4[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp38D.elt_typ),0);_tmp3D4[0]=_tmp2F9;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3D4,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp394)(void*)(_tmp3D2->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->hd=Cyc_Absyn_copy_exp(
_tmp3D2);_tmp3D3->tl=0;_tmp3D3;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2F8,_tmp3D2))->r));goto _LL28F;}}_LL29A: _tmp3AB=_tmp39A.f1;if((int)_tmp3AB != 
0)goto _LL29C;_tmp3AC=_tmp39A.f2;if((int)_tmp3AC != 0)goto _LL29C;_LL29B:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL29C: _tmp3AD=_tmp39A.f1;if((
int)_tmp3AD != 0)goto _LL29E;_tmp3AE=_tmp39A.f2;if((int)_tmp3AE != 1)goto _LL29E;
_LL29D: p1_forward_only=1;goto DynCast;_LL29E: _tmp3AF=_tmp39A.f1;if((int)_tmp3AF != 
1)goto _LL2A0;_tmp3B0=_tmp39A.f2;if((int)_tmp3B0 != 0)goto _LL2A0;_LL29F:
p2_forward_only=1;goto DynCast;_LL2A0: _tmp3B1=_tmp39A.f1;if((int)_tmp3B1 != 1)goto
_LL2A2;_tmp3B2=_tmp39A.f2;if((int)_tmp3B2 != 1)goto _LL2A2;_LL2A1: DynCast: if((
_tmp397  && !_tmp398) && !(_tmp38F.elt_tq).real_const){if(nv->toplevel)({void*
_tmp3D5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp3D6="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp3D6,sizeof(char),_get_zero_arr_size(_tmp3D6,70));}),
_tag_dynforward(_tmp3D5,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp3D7=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(_tmp3D7,({struct Cyc_Absyn_Exp*
_tmp3D8[3];_tmp3D8[2]=Cyc_Absyn_uint_exp(1,0);_tmp3D8[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp38D.elt_typ),0);_tmp3D8[0]=_tmp2F9;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3D8,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(
e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_to_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp3D9[1];_tmp3D9[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3D9,sizeof(struct Cyc_Absyn_Exp*),
1));}),0))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp3DA[1];_tmp3DA[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3DA,sizeof(struct Cyc_Absyn_Exp*),
1));}),0))->r));}}}}else{if(p1_forward_only != p2_forward_only){if(nv->toplevel)({
void*_tmp3DB[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp3DC="can't coerce between ? and ?+- at toplevel";
_tag_dynforward(_tmp3DC,sizeof(char),_get_zero_arr_size(_tmp3DC,43));}),
_tag_dynforward(_tmp3DB,sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_to_dyneither_e,({struct
Cyc_Absyn_Exp*_tmp3DD[1];_tmp3DD[0]=_tmp2F9;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3DD,sizeof(struct Cyc_Absyn_Exp*),
1));}),0))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp3DE[1];_tmp3DE[0]=_tmp2F9;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3DE,sizeof(struct Cyc_Absyn_Exp*),
1));}),0))->r));}}}goto _LL28F;_LL2A2: _tmp3B3=_tmp39A.f1;if(_tmp3B3 <= (void*)2)
goto _LL2A4;if(*((int*)_tmp3B3)!= 1)goto _LL2A4;_tmp3B4=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp3B3)->f1;_tmp3B5=_tmp39A.f2;if(_tmp3B5 <= (void*)2)goto _LL2A4;if(*((int*)
_tmp3B5)!= 1)goto _LL2A4;_tmp3B6=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp3B5)->f1;_LL2A3: if(_tmp393  && !_tmp394){if(nv->toplevel)({void*_tmp3DF[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp3E0="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp3E0,sizeof(char),_get_zero_arr_size(_tmp3E0,44));}),_tag_dynforward(_tmp3DF,
sizeof(void*),0));});if(_tmp2FB != (void*)2)({struct Cyc_String_pa_struct _tmp3E3;
_tmp3E3.tag=0;_tmp3E3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp3E1[1]={& _tmp3E3};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3E2="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp3E2,sizeof(char),_get_zero_arr_size(_tmp3E2,41));}),
_tag_dynforward(_tmp3E1,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp2F9);if(do_null_check){if(!_tmp2FA)({void*_tmp3E4[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp3E5="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp3E5,sizeof(char),_get_zero_arr_size(_tmp3E5,58));}),
_tag_dynforward(_tmp3E4,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2F8,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp3E6=
_cycalloc(sizeof(*_tmp3E6));_tmp3E6->hd=_tmp2F9;_tmp3E6->tl=0;_tmp3E6;}),0)))->r));}}}
goto _LL28F;_LL2A4: _tmp3B7=_tmp39A.f1;if(_tmp3B7 <= (void*)2)goto _LL2A6;if(*((int*)
_tmp3B7)!= 1)goto _LL2A6;_LL2A5:({struct Cyc_String_pa_struct _tmp3E9;_tmp3E9.tag=0;
_tmp3E9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp2F9->loc));{void*_tmp3E7[1]={& _tmp3E9};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3E8="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp3E8,sizeof(char),_get_zero_arr_size(_tmp3E8,30));}),
_tag_dynforward(_tmp3E7,sizeof(void*),1));}});_LL2A6: _tmp3B8=_tmp39A.f2;if(
_tmp3B8 <= (void*)2)goto _LL28F;if(*((int*)_tmp3B8)!= 1)goto _LL28F;_LL2A7: goto
_LL28F;_LL28F:;}goto _LL288;}_LL28B: _tmp390=_tmp38B.f1;if(_tmp390 <= (void*)4)goto
_LL28D;if(*((int*)_tmp390)!= 4)goto _LL28D;_tmp391=((struct Cyc_Absyn_PointerType_struct*)
_tmp390)->f1;_tmp392=_tmp38B.f2;if(_tmp392 <= (void*)4)goto _LL28D;if(*((int*)
_tmp392)!= 5)goto _LL28D;_LL28C:{void*_tmp3EA=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp391.ptr_atts).bounds);_LL2A9: if((int)_tmp3EA != 0)goto _LL2AB;_LL2AA: goto
_LL2AC;_LL2AB: if((int)_tmp3EA != 1)goto _LL2AD;_LL2AC:(void*)(_tmp2F9->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)_tmp2F9->r,_tmp2F9->loc),
Cyc_Toc_curr_sp,0))->r));goto _LL2A8;_LL2AD:;_LL2AE: goto _LL2A8;_LL2A8:;}goto
_LL288;_LL28D:;_LL28E: goto _LL288;_LL288:;}goto _LL1E3;}_LL208: if(*((int*)_tmp2D2)
!= 16)goto _LL20A;_tmp2FC=((struct Cyc_Absyn_Address_e_struct*)_tmp2D2)->f1;_LL209:{
void*_tmp3EB=(void*)_tmp2FC->r;struct _tuple1*_tmp3EC;struct Cyc_List_List*_tmp3ED;
struct Cyc_List_List*_tmp3EE;struct Cyc_List_List*_tmp3EF;_LL2B0: if(*((int*)
_tmp3EB)!= 30)goto _LL2B2;_tmp3EC=((struct Cyc_Absyn_Struct_e_struct*)_tmp3EB)->f1;
_tmp3ED=((struct Cyc_Absyn_Struct_e_struct*)_tmp3EB)->f2;_tmp3EE=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3EB)->f3;_LL2B1: if(nv->toplevel)({struct Cyc_String_pa_struct _tmp3F2;_tmp3F2.tag=
0;_tmp3F2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp2FC->loc));{void*_tmp3F0[1]={& _tmp3F2};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3F1="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp3F1,sizeof(char),_get_zero_arr_size(_tmp3F1,42));}),
_tag_dynforward(_tmp3F0,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2FC->topt))->v,_tmp3ED != 0,1,0,
_tmp3EE,_tmp3EC))->r));goto _LL2AF;_LL2B2: if(*((int*)_tmp3EB)!= 26)goto _LL2B4;
_tmp3EF=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3EB)->f1;_LL2B3: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp3F5;_tmp3F5.tag=0;_tmp3F5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp2FC->loc));{
void*_tmp3F3[1]={& _tmp3F5};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3F4="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp3F4,sizeof(char),_get_zero_arr_size(_tmp3F4,42));}),
_tag_dynforward(_tmp3F3,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp3EF))->r));goto _LL2AF;_LL2B4:;_LL2B5: Cyc_Toc_exp_to_c(nv,_tmp2FC);if(
!Cyc_Absyn_is_lvalue(_tmp2FC)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp2FC,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2FC))->r));}
goto _LL2AF;_LL2AF:;}goto _LL1E3;_LL20A: if(*((int*)_tmp2D2)!= 17)goto _LL20C;
_tmp2FD=((struct Cyc_Absyn_New_e_struct*)_tmp2D2)->f1;_tmp2FE=((struct Cyc_Absyn_New_e_struct*)
_tmp2D2)->f2;_LL20B: if(nv->toplevel)({struct Cyc_String_pa_struct _tmp3F8;_tmp3F8.tag=
0;_tmp3F8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp2FE->loc));{void*_tmp3F6[1]={& _tmp3F8};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3F7="%s: new at top-level";
_tag_dynforward(_tmp3F7,sizeof(char),_get_zero_arr_size(_tmp3F7,21));}),
_tag_dynforward(_tmp3F6,sizeof(void*),1));}});{void*_tmp3F9=(void*)_tmp2FE->r;
struct Cyc_List_List*_tmp3FA;struct Cyc_Absyn_Vardecl*_tmp3FB;struct Cyc_Absyn_Exp*
_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FD;int _tmp3FE;struct _tuple1*_tmp3FF;struct Cyc_List_List*
_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp402;_LL2B7: if(*((int*)
_tmp3F9)!= 28)goto _LL2B9;_tmp3FA=((struct Cyc_Absyn_Array_e_struct*)_tmp3F9)->f1;
_LL2B8: {struct _tuple1*_tmp403=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp404=
Cyc_Absyn_var_exp(_tmp403,0);struct Cyc_Absyn_Stmt*_tmp405=Cyc_Toc_init_array(nv,
_tmp404,_tmp3FA,Cyc_Absyn_exp_stmt(_tmp404,0));void*old_elt_typ;{void*_tmp406=
Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp407;void*_tmp408;struct
Cyc_Absyn_Tqual _tmp409;struct Cyc_Absyn_PtrAtts _tmp40A;struct Cyc_Absyn_Conref*
_tmp40B;_LL2C2: if(_tmp406 <= (void*)4)goto _LL2C4;if(*((int*)_tmp406)!= 4)goto
_LL2C4;_tmp407=((struct Cyc_Absyn_PointerType_struct*)_tmp406)->f1;_tmp408=(void*)
_tmp407.elt_typ;_tmp409=_tmp407.elt_tq;_tmp40A=_tmp407.ptr_atts;_tmp40B=_tmp40A.zero_term;
_LL2C3: old_elt_typ=_tmp408;goto _LL2C1;_LL2C4:;_LL2C5: old_elt_typ=({void*_tmp40C[
0]={};Cyc_Toc_toc_impos(({const char*_tmp40D="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,52));}),
_tag_dynforward(_tmp40C,sizeof(void*),0));});_LL2C1:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp40E=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp40F=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3FA),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp2FD == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp40F);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp2FD;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp40F);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp403,_tmp40E,(struct Cyc_Absyn_Exp*)e1,_tmp405,0),0))->r));goto _LL2B6;}}_LL2B9:
if(*((int*)_tmp3F9)!= 29)goto _LL2BB;_tmp3FB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3F9)->f1;_tmp3FC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3F9)->f2;
_tmp3FD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3F9)->f3;_tmp3FE=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp3F9)->f4;_LL2BA: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp410=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp411;void*_tmp412;struct Cyc_Absyn_Tqual _tmp413;struct Cyc_Absyn_PtrAtts _tmp414;
struct Cyc_Absyn_Conref*_tmp415;struct Cyc_Absyn_Conref*_tmp416;_LL2C7: if(_tmp410
<= (void*)4)goto _LL2C9;if(*((int*)_tmp410)!= 4)goto _LL2C9;_tmp411=((struct Cyc_Absyn_PointerType_struct*)
_tmp410)->f1;_tmp412=(void*)_tmp411.elt_typ;_tmp413=_tmp411.elt_tq;_tmp414=
_tmp411.ptr_atts;_tmp415=_tmp414.bounds;_tmp416=_tmp414.zero_term;_LL2C8:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp415)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp415)== (void*)1;
goto _LL2C6;_LL2C9:;_LL2CA:({void*_tmp417[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp418="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp418,sizeof(char),_get_zero_arr_size(_tmp418,42));}),
_tag_dynforward(_tmp417,sizeof(void*),0));});_LL2C6:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3FD->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp3FC);{
struct Cyc_Absyn_Exp*_tmp419=Cyc_Absyn_var_exp(max,0);if(_tmp3FE)_tmp419=Cyc_Absyn_add_exp(
_tmp419,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp3FB,Cyc_Absyn_var_exp(max,0),_tmp3FD,_tmp3FE,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp41A=_new_region("r");struct _RegionHandle*r=& _tmp41A;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp42B=
_region_malloc(r,sizeof(*_tmp42B));_tmp42B->hd=({struct _tuple10*_tmp42C=
_region_malloc(r,sizeof(*_tmp42C));_tmp42C->f1=max;_tmp42C->f2=Cyc_Absyn_uint_typ;
_tmp42C->f3=(struct Cyc_Absyn_Exp*)_tmp3FC;_tmp42C;});_tmp42B->tl=0;_tmp42B;});
struct Cyc_Absyn_Exp*ai;if(_tmp2FD == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp41B[2];_tmp41B[1]=_tmp419;
_tmp41B[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp41B,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp2FD;Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp41C[2];_tmp41C[1]=_tmp419;_tmp41C[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp41C,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({struct Cyc_List_List*_tmp41D=
_region_malloc(r,sizeof(*_tmp41D));_tmp41D->hd=({struct _tuple10*_tmp41E=
_region_malloc(r,sizeof(*_tmp41E));_tmp41E->f1=a;_tmp41E->f2=ptr_typ;_tmp41E->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp41E;});_tmp41D->tl=decls;_tmp41D;});if(
is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp41F=Cyc_Toc_temp_var();
void*_tmp420=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp421=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp422=Cyc_Absyn_fncall_exp(_tmp421,({struct Cyc_Absyn_Exp*_tmp425[3];_tmp425[2]=
_tmp419;_tmp425[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp425[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp425,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp423=_region_malloc(r,sizeof(*_tmp423));_tmp423->hd=({struct _tuple10*_tmp424=
_region_malloc(r,sizeof(*_tmp424));_tmp424->f1=_tmp41F;_tmp424->f2=_tmp420;
_tmp424->f3=(struct Cyc_Absyn_Exp*)_tmp422;_tmp424;});_tmp423->tl=decls;_tmp423;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp41F,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp426=decls;for(0;_tmp426 != 0;_tmp426=_tmp426->tl){struct
_tuple1*_tmp428;void*_tmp429;struct Cyc_Absyn_Exp*_tmp42A;struct _tuple10 _tmp427=*((
struct _tuple10*)_tmp426->hd);_tmp428=_tmp427.f1;_tmp429=_tmp427.f2;_tmp42A=
_tmp427.f3;s=Cyc_Absyn_declare_stmt(_tmp428,_tmp429,_tmp42A,s,0);}}(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL2B6;}}}}
_LL2BB: if(*((int*)_tmp3F9)!= 30)goto _LL2BD;_tmp3FF=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3F9)->f1;_tmp400=((struct Cyc_Absyn_Struct_e_struct*)_tmp3F9)->f2;_tmp401=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3F9)->f3;_LL2BC:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2FE->topt))->v,
_tmp400 != 0,1,_tmp2FD,_tmp401,_tmp3FF))->r));goto _LL2B6;_LL2BD: if(*((int*)
_tmp3F9)!= 26)goto _LL2BF;_tmp402=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3F9)->f1;
_LL2BE:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp2FD,_tmp402))->r));
goto _LL2B6;_LL2BF:;_LL2C0: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2FE->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp42D=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_var_exp(
_tmp42D,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp42E,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp2FD == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp2FD;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*
_tmp42F=(void*)_tmp2FE->r;void*_tmp430;struct Cyc_Absyn_Exp*_tmp431;_LL2CC: if(*((
int*)_tmp42F)!= 15)goto _LL2CE;_tmp430=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp42F)->f1;_tmp431=((struct Cyc_Absyn_Cast_e_struct*)_tmp42F)->f2;_LL2CD:{
struct _tuple0 _tmp433=({struct _tuple0 _tmp432;_tmp432.f1=Cyc_Tcutil_compress(
_tmp430);_tmp432.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp431->topt))->v);_tmp432;});void*_tmp434;struct Cyc_Absyn_PtrInfo _tmp435;void*
_tmp436;struct Cyc_Absyn_PtrAtts _tmp437;struct Cyc_Absyn_Conref*_tmp438;void*
_tmp439;struct Cyc_Absyn_PtrInfo _tmp43A;struct Cyc_Absyn_PtrAtts _tmp43B;struct Cyc_Absyn_Conref*
_tmp43C;_LL2D1: _tmp434=_tmp433.f1;if(_tmp434 <= (void*)4)goto _LL2D3;if(*((int*)
_tmp434)!= 4)goto _LL2D3;_tmp435=((struct Cyc_Absyn_PointerType_struct*)_tmp434)->f1;
_tmp436=(void*)_tmp435.elt_typ;_tmp437=_tmp435.ptr_atts;_tmp438=_tmp437.bounds;
_tmp439=_tmp433.f2;if(_tmp439 <= (void*)4)goto _LL2D3;if(*((int*)_tmp439)!= 4)goto
_LL2D3;_tmp43A=((struct Cyc_Absyn_PointerType_struct*)_tmp439)->f1;_tmp43B=
_tmp43A.ptr_atts;_tmp43C=_tmp43B.bounds;_LL2D2:{struct _tuple0 _tmp43E=({struct
_tuple0 _tmp43D;_tmp43D.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp438);
_tmp43D.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp43C);_tmp43D;});void*
_tmp43F;void*_tmp440;struct Cyc_Absyn_Exp*_tmp441;void*_tmp442;void*_tmp443;
struct Cyc_Absyn_Exp*_tmp444;_LL2D6: _tmp43F=_tmp43E.f1;if((int)_tmp43F != 0)goto
_LL2D8;_tmp440=_tmp43E.f2;if(_tmp440 <= (void*)2)goto _LL2D8;if(*((int*)_tmp440)!= 
0)goto _LL2D8;_tmp441=((struct Cyc_Absyn_Upper_b_struct*)_tmp440)->f1;_LL2D7:
forward_only=1;_tmp444=_tmp441;goto _LL2D9;_LL2D8: _tmp442=_tmp43E.f1;if((int)
_tmp442 != 1)goto _LL2DA;_tmp443=_tmp43E.f2;if(_tmp443 <= (void*)2)goto _LL2DA;if(*((
int*)_tmp443)!= 0)goto _LL2DA;_tmp444=((struct Cyc_Absyn_Upper_b_struct*)_tmp443)->f1;
_LL2D9: Cyc_Toc_exp_to_c(nv,_tmp431);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;{struct Cyc_Absyn_Exp*_tmp445=forward_only?Cyc_Toc__init_dynforward_ptr_e:
Cyc_Toc__init_dyneither_ptr_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp445,({struct Cyc_Absyn_Exp*_tmp446[4];_tmp446[3]=_tmp444;_tmp446[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp436),0);_tmp446[1]=_tmp431;_tmp446[0]=mexp;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp446,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL2D5;}_LL2DA:;_LL2DB: goto _LL2D5;_LL2D5:;}goto _LL2D0;_LL2D3:;
_LL2D4: goto _LL2D0;_LL2D0:;}goto _LL2CB;_LL2CE:;_LL2CF: goto _LL2CB;_LL2CB:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp447=Cyc_Absyn_exp_stmt(_tmp42E,0);struct Cyc_Absyn_Exp*
_tmp448=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp2FE);_tmp447=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp42E,_tmp448,0),_tmp2FE,0),
_tmp447,0);{void*_tmp449=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp42D,_tmp449,(struct
Cyc_Absyn_Exp*)mexp,_tmp447,0),0))->r));}}goto _LL2B6;}}_LL2B6:;}goto _LL1E3;
_LL20C: if(*((int*)_tmp2D2)!= 19)goto _LL20E;_tmp2FF=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp2D2)->f1;_LL20D: Cyc_Toc_exp_to_c(nv,_tmp2FF);goto _LL1E3;_LL20E: if(*((int*)
_tmp2D2)!= 18)goto _LL210;_tmp300=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2D2)->f1;_LL20F:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp44B;_tmp44B.tag=18;_tmp44B.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp300);_tmp44B;});
_tmp44A;})));goto _LL1E3;_LL210: if(*((int*)_tmp2D2)!= 21)goto _LL212;_LL211:({void*
_tmp44C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp44D="__gen() in code generator";_tag_dynforward(_tmp44D,sizeof(
char),_get_zero_arr_size(_tmp44D,26));}),_tag_dynforward(_tmp44C,sizeof(void*),0));});
_LL212: if(*((int*)_tmp2D2)!= 20)goto _LL214;_tmp301=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2D2)->f1;_tmp302=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2D2)->f2;
if(*((int*)_tmp302)!= 0)goto _LL214;_tmp303=((struct Cyc_Absyn_StructField_struct*)
_tmp302)->f1;_LL213:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp44F;_tmp44F.tag=20;_tmp44F.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp301);_tmp44F.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp450=_cycalloc(sizeof(*
_tmp450));_tmp450[0]=({struct Cyc_Absyn_StructField_struct _tmp451;_tmp451.tag=0;
_tmp451.f1=_tmp303;_tmp451;});_tmp450;}));_tmp44F;});_tmp44E;})));goto _LL1E3;
_LL214: if(*((int*)_tmp2D2)!= 20)goto _LL216;_tmp304=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2D2)->f1;_tmp305=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2D2)->f2;
if(*((int*)_tmp305)!= 1)goto _LL216;_tmp306=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp305)->f1;_LL215:{void*_tmp452=Cyc_Tcutil_compress(_tmp304);struct Cyc_Absyn_AggrInfo
_tmp453;void*_tmp454;struct Cyc_List_List*_tmp455;_LL2DD: if(_tmp452 <= (void*)4)
goto _LL2E5;if(*((int*)_tmp452)!= 10)goto _LL2DF;_tmp453=((struct Cyc_Absyn_AggrType_struct*)
_tmp452)->f1;_tmp454=(void*)_tmp453.aggr_info;_LL2DE: {struct Cyc_Absyn_Aggrdecl*
_tmp456=Cyc_Absyn_get_known_aggrdecl(_tmp454);if(_tmp456->impl == 0)({void*
_tmp457[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp458="struct fields must be known";_tag_dynforward(_tmp458,sizeof(
char),_get_zero_arr_size(_tmp458,28));}),_tag_dynforward(_tmp457,sizeof(void*),0));});
_tmp455=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp456->impl))->fields;goto
_LL2E0;}_LL2DF: if(*((int*)_tmp452)!= 11)goto _LL2E1;_tmp455=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp452)->f2;_LL2E0: {struct Cyc_Absyn_Aggrfield*_tmp459=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp455,(int)_tmp306);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp45A=_cycalloc(sizeof(*
_tmp45A));_tmp45A[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp45B;_tmp45B.tag=20;
_tmp45B.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp304);_tmp45B.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C[0]=({
struct Cyc_Absyn_StructField_struct _tmp45D;_tmp45D.tag=0;_tmp45D.f1=_tmp459->name;
_tmp45D;});_tmp45C;}));_tmp45B;});_tmp45A;})));goto _LL2DC;}_LL2E1: if(*((int*)
_tmp452)!= 9)goto _LL2E3;_LL2E2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp45F;_tmp45F.tag=20;_tmp45F.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp304);_tmp45F.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp460=_cycalloc(sizeof(*
_tmp460));_tmp460[0]=({struct Cyc_Absyn_StructField_struct _tmp461;_tmp461.tag=0;
_tmp461.f1=Cyc_Absyn_fieldname((int)(_tmp306 + 1));_tmp461;});_tmp460;}));_tmp45F;});
_tmp45E;})));goto _LL2DC;_LL2E3: if(*((int*)_tmp452)!= 3)goto _LL2E5;_LL2E4: if(
_tmp306 == 0)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp463;_tmp463.tag=20;_tmp463.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp304);_tmp463.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp464=_cycalloc(sizeof(*
_tmp464));_tmp464[0]=({struct Cyc_Absyn_StructField_struct _tmp465;_tmp465.tag=0;
_tmp465.f1=Cyc_Toc_tag_sp;_tmp465;});_tmp464;}));_tmp463;});_tmp462;})));else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp466=_cycalloc(
sizeof(*_tmp466));_tmp466[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp467;_tmp467.tag=
20;_tmp467.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp304);_tmp467.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468[0]=({
struct Cyc_Absyn_StructField_struct _tmp469;_tmp469.tag=0;_tmp469.f1=Cyc_Absyn_fieldname((
int)_tmp306);_tmp469;});_tmp468;}));_tmp467;});_tmp466;})));}goto _LL2DC;_LL2E5:;
_LL2E6:({void*_tmp46A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp46B="impossible type for offsetof tuple index";
_tag_dynforward(_tmp46B,sizeof(char),_get_zero_arr_size(_tmp46B,41));}),
_tag_dynforward(_tmp46A,sizeof(void*),0));});_LL2DC:;}goto _LL1E3;_LL216: if(*((
int*)_tmp2D2)!= 22)goto _LL218;_tmp307=((struct Cyc_Absyn_Deref_e_struct*)_tmp2D2)->f1;
_LL217: {void*_tmp46C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp307->topt))->v);{void*_tmp46D=_tmp46C;struct Cyc_Absyn_PtrInfo
_tmp46E;void*_tmp46F;struct Cyc_Absyn_Tqual _tmp470;struct Cyc_Absyn_PtrAtts _tmp471;
void*_tmp472;struct Cyc_Absyn_Conref*_tmp473;struct Cyc_Absyn_Conref*_tmp474;
struct Cyc_Absyn_Conref*_tmp475;_LL2E8: if(_tmp46D <= (void*)4)goto _LL2EA;if(*((int*)
_tmp46D)!= 4)goto _LL2EA;_tmp46E=((struct Cyc_Absyn_PointerType_struct*)_tmp46D)->f1;
_tmp46F=(void*)_tmp46E.elt_typ;_tmp470=_tmp46E.elt_tq;_tmp471=_tmp46E.ptr_atts;
_tmp472=(void*)_tmp471.rgn;_tmp473=_tmp471.nullable;_tmp474=_tmp471.bounds;
_tmp475=_tmp471.zero_term;_LL2E9:{void*_tmp476=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp474);_LL2ED: if(_tmp476 <= (void*)2)goto _LL2EF;if(*((int*)_tmp476)!= 0)goto
_LL2EF;_LL2EE: {int do_null_check=Cyc_Toc_need_null_check(_tmp307);Cyc_Toc_exp_to_c(
nv,_tmp307);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp477[0]={};
Cyc_Tcutil_warn(e->loc,({const char*_tmp478="inserted null check due to dereference";
_tag_dynforward(_tmp478,sizeof(char),_get_zero_arr_size(_tmp478,39));}),
_tag_dynforward(_tmp477,sizeof(void*),0));});(void*)(_tmp307->r=(void*)((void*)(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(_tmp46C),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->hd=Cyc_Absyn_copy_exp(
_tmp307);_tmp479->tl=0;_tmp479;}),0)))->r));}goto _LL2EC;}_LL2EF: if((int)_tmp476
!= 0)goto _LL2F1;_LL2F0: goto _LL2F2;_LL2F1: if((int)_tmp476 != 1)goto _LL2F3;_LL2F2: {
struct Cyc_Absyn_Exp*_tmp47A=Cyc_Absyn_uint_exp(0,0);_tmp47A->topt=({struct Cyc_Core_Opt*
_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->v=(void*)Cyc_Absyn_uint_typ;_tmp47B;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp307,_tmp47A,0))->r));Cyc_Toc_exp_to_c(
nv,e);goto _LL2EC;}_LL2F3: if(_tmp476 <= (void*)2)goto _LL2EC;if(*((int*)_tmp476)!= 
1)goto _LL2EC;_LL2F4:({void*_tmp47C[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp47D="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp47D,sizeof(char),_get_zero_arr_size(_tmp47D,30));}),
_tag_dynforward(_tmp47C,sizeof(void*),0));});_LL2EC:;}goto _LL2E7;_LL2EA:;_LL2EB:({
void*_tmp47E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp47F="exp_to_c: Deref: non-pointer";
_tag_dynforward(_tmp47F,sizeof(char),_get_zero_arr_size(_tmp47F,29));}),
_tag_dynforward(_tmp47E,sizeof(void*),0));});_LL2E7:;}goto _LL1E3;}_LL218: if(*((
int*)_tmp2D2)!= 23)goto _LL21A;_tmp308=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2D2)->f1;_tmp309=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2D2)->f2;_LL219:
Cyc_Toc_exp_to_c(nv,_tmp308);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E3;
_LL21A: if(*((int*)_tmp2D2)!= 24)goto _LL21C;_tmp30A=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp2D2)->f1;_tmp30B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2D2)->f2;_LL21B: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp30A->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp30A);Cyc_Toc_exp_to_c(nv,_tmp30A);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp481;struct Cyc_Absyn_Tqual
_tmp482;struct Cyc_Absyn_PtrAtts _tmp483;void*_tmp484;struct Cyc_Absyn_Conref*
_tmp485;struct Cyc_Absyn_Conref*_tmp486;struct Cyc_Absyn_Conref*_tmp487;struct Cyc_Absyn_PtrInfo
_tmp480=Cyc_Toc_get_ptr_type(e1typ);_tmp481=(void*)_tmp480.elt_typ;_tmp482=
_tmp480.elt_tq;_tmp483=_tmp480.ptr_atts;_tmp484=(void*)_tmp483.rgn;_tmp485=
_tmp483.nullable;_tmp486=_tmp483.bounds;_tmp487=_tmp483.zero_term;{void*_tmp488=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp486);struct Cyc_Absyn_Exp*_tmp489;
_LL2F6: if(_tmp488 <= (void*)2)goto _LL2F8;if(*((int*)_tmp488)!= 0)goto _LL2F8;
_tmp489=((struct Cyc_Absyn_Upper_b_struct*)_tmp488)->f1;_LL2F7: {unsigned int
_tmp48B;int _tmp48C;struct _tuple5 _tmp48A=Cyc_Evexp_eval_const_uint_exp(_tmp489);
_tmp48B=_tmp48A.f1;_tmp48C=_tmp48A.f2;if(_tmp48C){if(_tmp48B < 1)({void*_tmp48D[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp48E="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp48E,sizeof(char),_get_zero_arr_size(_tmp48E,44));}),_tag_dynforward(_tmp48D,
sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*
_tmp48F[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp490="inserted null check due to dereference";
_tag_dynforward(_tmp490,sizeof(char),_get_zero_arr_size(_tmp490,39));}),
_tag_dynforward(_tmp48F,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp30A->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->hd=_tmp30A;_tmp491->tl=0;_tmp491;}),
0)),_tmp30B,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp30A->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp492[4];_tmp492[3]=Cyc_Absyn_uint_exp(0,0);_tmp492[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp481,0);_tmp492[1]=_tmp489;_tmp492[0]=_tmp30A;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp492,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp30B,0))->r));}goto _LL2F5;}_LL2F8: if((int)_tmp488 != 0)goto _LL2FA;
_LL2F9: forward_only=1;goto _LL2FB;_LL2FA: if((int)_tmp488 != 1)goto _LL2FC;_LL2FB: {
struct Cyc_Absyn_Exp*_tmp493=forward_only?Cyc_Toc__check_dynforward_subscript_e:
Cyc_Toc__check_dyneither_subscript_e;void*ta1=Cyc_Toc_typ_to_c_array(_tmp481);(
void*)(_tmp30A->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp482),Cyc_Absyn_fncall_exp(_tmp493,({struct Cyc_Absyn_Exp*_tmp494[3];_tmp494[2]=
Cyc_Absyn_uint_exp(0,0);_tmp494[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp494[0]=Cyc_Absyn_copy_exp(
_tmp30A);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp494,sizeof(struct Cyc_Absyn_Exp*),3));}),0)))->r));goto _LL2F5;}
_LL2FC: if(_tmp488 <= (void*)2)goto _LL2F5;if(*((int*)_tmp488)!= 1)goto _LL2F5;
_LL2FD:({void*_tmp495[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp496="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp496,sizeof(char),_get_zero_arr_size(_tmp496,34));}),
_tag_dynforward(_tmp495,sizeof(void*),0));});_LL2F5:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E3;}}
_LL21C: if(*((int*)_tmp2D2)!= 25)goto _LL21E;_tmp30C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2D2)->f1;_tmp30D=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2D2)->f2;_LL21D: {
void*_tmp497=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp30C->topt))->v);{void*_tmp498=_tmp497;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_PtrInfo
_tmp49A;void*_tmp49B;struct Cyc_Absyn_Tqual _tmp49C;struct Cyc_Absyn_PtrAtts _tmp49D;
void*_tmp49E;struct Cyc_Absyn_Conref*_tmp49F;struct Cyc_Absyn_Conref*_tmp4A0;
struct Cyc_Absyn_Conref*_tmp4A1;_LL2FF: if(_tmp498 <= (void*)4)goto _LL303;if(*((int*)
_tmp498)!= 9)goto _LL301;_tmp499=((struct Cyc_Absyn_TupleType_struct*)_tmp498)->f1;
_LL300: Cyc_Toc_exp_to_c(nv,_tmp30C);Cyc_Toc_exp_to_c(nv,_tmp30D);{unsigned int
_tmp4A3;int _tmp4A4;struct _tuple5 _tmp4A2=Cyc_Evexp_eval_const_uint_exp(_tmp30D);
_tmp4A3=_tmp4A2.f1;_tmp4A4=_tmp4A2.f2;if(!_tmp4A4)({void*_tmp4A5[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4A6="unknown tuple subscript in translation to C";_tag_dynforward(_tmp4A6,
sizeof(char),_get_zero_arr_size(_tmp4A6,44));}),_tag_dynforward(_tmp4A5,sizeof(
void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(_tmp30C,Cyc_Absyn_fieldname((
int)(_tmp4A3 + 1)),0))->r));goto _LL2FE;}_LL301: if(*((int*)_tmp498)!= 4)goto _LL303;
_tmp49A=((struct Cyc_Absyn_PointerType_struct*)_tmp498)->f1;_tmp49B=(void*)
_tmp49A.elt_typ;_tmp49C=_tmp49A.elt_tq;_tmp49D=_tmp49A.ptr_atts;_tmp49E=(void*)
_tmp49D.rgn;_tmp49F=_tmp49D.nullable;_tmp4A0=_tmp49D.bounds;_tmp4A1=_tmp49D.zero_term;
_LL302: {struct Cyc_List_List*_tmp4A7=Cyc_Toc_get_relns(_tmp30C);int in_bnds=0;{
void*_tmp4A8=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4A0);_LL306: if(
_tmp4A8 <= (void*)2)goto _LL308;if(*((int*)_tmp4A8)!= 1)goto _LL308;_LL307: goto
_LL305;_LL308:;_LL309: in_bnds=Cyc_Toc_check_bounds(_tmp4A7,_tmp30C,_tmp30D);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp4AB;
_tmp4AB.tag=0;_tmp4AB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp4A9[1]={& _tmp4AB};Cyc_Tcutil_warn(e->loc,({const char*_tmp4AA="bounds check necessary for %s";
_tag_dynforward(_tmp4AA,sizeof(char),_get_zero_arr_size(_tmp4AA,30));}),
_tag_dynforward(_tmp4A9,sizeof(void*),1));}});_LL305:;}Cyc_Toc_exp_to_c(nv,
_tmp30C);Cyc_Toc_exp_to_c(nv,_tmp30D);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp4AC=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4A0);
struct Cyc_Absyn_Exp*_tmp4AD;void*_tmp4AE;_LL30B: if(_tmp4AC <= (void*)2)goto _LL30D;
if(*((int*)_tmp4AC)!= 0)goto _LL30D;_tmp4AD=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4AC)->f1;_LL30C: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp49F);void*ta1=Cyc_Toc_typ_to_c(_tmp49B);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp49C);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4A1))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp4AF[3];_tmp4AF[2]=_tmp30D;_tmp4AF[1]=_tmp4AD;_tmp4AF[0]=
_tmp30C;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4AF,sizeof(struct Cyc_Absyn_Exp*),3));}),0)),0))->r));else{
if(possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp4B0[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4B1="inserted null check due to dereference";
_tag_dynforward(_tmp4B1,sizeof(char),_get_zero_arr_size(_tmp4B1,39));}),
_tag_dynforward(_tmp4B0,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp4B2[4];_tmp4B2[3]=_tmp30D;_tmp4B2[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp4B2[1]=_tmp4AD;_tmp4B2[0]=_tmp30C;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4B2,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),0))->r));}else{(void*)(_tmp30D->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp4B3[2];
_tmp4B3[1]=Cyc_Absyn_copy_exp(_tmp30D);_tmp4B3[0]=_tmp4AD;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4B3,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));}}}goto _LL30A;}_LL30D: if((int)_tmp4AC != 0)goto _LL30F;_LL30E:
forward_only=1;goto _LL310;_LL30F: if((int)_tmp4AC != 1)goto _LL311;_LL310: {void*
ta1=Cyc_Toc_typ_to_c_array(_tmp49B);if(in_bnds){Cyc_Toc_bounds_checks_eliminated
++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
ta1,_tmp49C),Cyc_Absyn_aggrmember_exp(_tmp30C,Cyc_Toc_curr_sp,0)),_tmp30D,0))->r));}
else{struct Cyc_Absyn_Exp*_tmp4B4=forward_only?Cyc_Toc__check_dynforward_subscript_e:
Cyc_Toc__check_dyneither_subscript_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp49C),Cyc_Absyn_fncall_exp(_tmp4B4,({
struct Cyc_Absyn_Exp*_tmp4B5[3];_tmp4B5[2]=_tmp30D;_tmp4B5[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp4B5[0]=_tmp30C;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4B5,sizeof(struct Cyc_Absyn_Exp*),3));}),0)),0))->r));}goto
_LL30A;}_LL311: if(_tmp4AC <= (void*)2)goto _LL30A;if(*((int*)_tmp4AC)!= 1)goto
_LL30A;_tmp4AE=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4AC)->f1;_LL312:{
void*_tmp4B6=(void*)_tmp30D->annot;struct Cyc_List_List*_tmp4B7;_LL314: if(*((void**)
_tmp4B6)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL316;_tmp4B7=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)
_tmp4B6)->f1;_LL315: for(0;_tmp4B7 != 0;_tmp4B7=_tmp4B7->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp4B7->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp4B7->hd)->bd,_tmp4AE)== 0){in_bnds=1;break;}}goto _LL313;_LL316:;_LL317: goto
_LL313;_LL313:;}if(!in_bnds)({void*_tmp4B8[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp4B9="cannot determine that subscript is in bounds";_tag_dynforward(
_tmp4B9,sizeof(char),_get_zero_arr_size(_tmp4B9,45));}),_tag_dynforward(_tmp4B8,
sizeof(void*),0));});goto _LL30A;_LL30A:;}goto _LL2FE;}}_LL303:;_LL304:({void*
_tmp4BA[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4BB="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp4BB,sizeof(char),_get_zero_arr_size(_tmp4BB,49));}),
_tag_dynforward(_tmp4BA,sizeof(void*),0));});_LL2FE:;}goto _LL1E3;}_LL21E: if(*((
int*)_tmp2D2)!= 26)goto _LL220;_tmp30E=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2D2)->f1;
_LL21F: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,
_tmp30E))->r));else{struct Cyc_List_List*_tmp4BC=((struct Cyc_List_List*(*)(struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp30E);struct _dynforward_ptr*_tmp4BD=Cyc_Toc_add_tuple_type(_tmp4BC);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp30E != 0;(_tmp30E=_tmp30E->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp30E->hd);dles=({struct Cyc_List_List*_tmp4BE=_cycalloc(
sizeof(*_tmp4BE));_tmp4BE->hd=({struct _tuple6*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));
_tmp4BF->f1=0;_tmp4BF->f2=(struct Cyc_Absyn_Exp*)_tmp30E->hd;_tmp4BF;});_tmp4BE->tl=
dles;_tmp4BE;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL1E3;_LL220: if(*((int*)_tmp2D2)!= 28)goto _LL222;_tmp30F=((struct Cyc_Absyn_Array_e_struct*)
_tmp2D2)->f1;_LL221:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp30F,0))->r));{struct Cyc_List_List*_tmp4C0=_tmp30F;for(0;_tmp4C0 != 0;_tmp4C0=
_tmp4C0->tl){struct _tuple6 _tmp4C2;struct Cyc_Absyn_Exp*_tmp4C3;struct _tuple6*
_tmp4C1=(struct _tuple6*)_tmp4C0->hd;_tmp4C2=*_tmp4C1;_tmp4C3=_tmp4C2.f2;Cyc_Toc_exp_to_c(
nv,_tmp4C3);}}goto _LL1E3;_LL222: if(*((int*)_tmp2D2)!= 29)goto _LL224;_tmp310=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D2)->f1;_tmp311=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2D2)->f2;_tmp312=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D2)->f3;
_tmp313=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D2)->f4;_LL223: {
unsigned int _tmp4C5;int _tmp4C6;struct _tuple5 _tmp4C4=Cyc_Evexp_eval_const_uint_exp(
_tmp311);_tmp4C5=_tmp4C4.f1;_tmp4C6=_tmp4C4.f2;{void*_tmp4C7=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp312->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp312);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp312)){if(!_tmp4C6)({
void*_tmp4C8[0]={};Cyc_Tcutil_terr(_tmp311->loc,({const char*_tmp4C9="cannot determine value of constant";
_tag_dynforward(_tmp4C9,sizeof(char),_get_zero_arr_size(_tmp4C9,35));}),
_tag_dynforward(_tmp4C8,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp4C5;i
++){es=({struct Cyc_List_List*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->hd=({
struct _tuple6*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->f1=0;_tmp4CB->f2=
_tmp312;_tmp4CB;});_tmp4CA->tl=es;_tmp4CA;});}}if(_tmp313){struct Cyc_Absyn_Exp*
_tmp4CC=Cyc_Toc_cast_it(_tmp4C7,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD->hd=({struct _tuple6*_tmp4CE=
_cycalloc(sizeof(*_tmp4CE));_tmp4CE->f1=0;_tmp4CE->f2=_tmp4CC;_tmp4CE;});_tmp4CD->tl=
0;_tmp4CD;}));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));
goto _LL1E3;}}}_LL224: if(*((int*)_tmp2D2)!= 30)goto _LL226;_tmp314=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2D2)->f1;_tmp315=((struct Cyc_Absyn_Struct_e_struct*)_tmp2D2)->f2;_tmp316=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2D2)->f3;_tmp317=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2D2)->f4;_LL225: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp315 != 0,0,0,_tmp316,_tmp314))->r));else{if(_tmp317 == 0)({void*
_tmp4CF[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4D0="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp4D0,
sizeof(char),_get_zero_arr_size(_tmp4D0,35));}),_tag_dynforward(_tmp4CF,sizeof(
void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp317;
struct _RegionHandle _tmp4D1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4D1;
_push_region(rgn);{struct Cyc_List_List*_tmp4D2=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp316,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp4D3=0;struct Cyc_List_List*_tmp4D4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp4D4 != 0;_tmp4D4=_tmp4D4->tl){struct Cyc_List_List*
_tmp4D5=_tmp4D2;for(0;_tmp4D5 != 0;_tmp4D5=_tmp4D5->tl){if((*((struct _tuple11*)
_tmp4D5->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp4D4->hd){struct _tuple11
_tmp4D7;struct Cyc_Absyn_Aggrfield*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct
_tuple11*_tmp4D6=(struct _tuple11*)_tmp4D5->hd;_tmp4D7=*_tmp4D6;_tmp4D8=_tmp4D7.f1;
_tmp4D9=_tmp4D7.f2;{void*_tmp4DA=(void*)_tmp4D8->type;Cyc_Toc_exp_to_c(nv,
_tmp4D9);if(Cyc_Toc_is_void_star(_tmp4DA))(void*)(_tmp4D9->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp4D9->r,0)))->r));_tmp4D3=({
struct Cyc_List_List*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->hd=({struct
_tuple6*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->f1=0;_tmp4DC->f2=_tmp4D9;
_tmp4DC;});_tmp4DB->tl=_tmp4D3;_tmp4DB;});break;}}}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp4D3),0))->r));};_pop_region(rgn);}}goto _LL1E3;_LL226: if(*((
int*)_tmp2D2)!= 31)goto _LL228;_tmp318=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2D2)->f1;_tmp319=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2D2)->f2;_LL227: {
struct Cyc_List_List*fs;{void*_tmp4DD=Cyc_Tcutil_compress(_tmp318);struct Cyc_List_List*
_tmp4DE;_LL319: if(_tmp4DD <= (void*)4)goto _LL31B;if(*((int*)_tmp4DD)!= 11)goto
_LL31B;_tmp4DE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4DD)->f2;_LL31A: fs=
_tmp4DE;goto _LL318;_LL31B:;_LL31C:({struct Cyc_String_pa_struct _tmp4E1;_tmp4E1.tag=
0;_tmp4E1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp318));{void*_tmp4DF[1]={& _tmp4E1};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4E0="anon struct has type %s";
_tag_dynforward(_tmp4E0,sizeof(char),_get_zero_arr_size(_tmp4E0,24));}),
_tag_dynforward(_tmp4DF,sizeof(void*),1));}});_LL318:;}{struct _RegionHandle
_tmp4E2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4E2;_push_region(rgn);{
struct Cyc_List_List*_tmp4E3=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp319,fs);for(0;
_tmp4E3 != 0;_tmp4E3=_tmp4E3->tl){struct _tuple11 _tmp4E5;struct Cyc_Absyn_Aggrfield*
_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;struct _tuple11*_tmp4E4=(struct _tuple11*)
_tmp4E3->hd;_tmp4E5=*_tmp4E4;_tmp4E6=_tmp4E5.f1;_tmp4E7=_tmp4E5.f2;{void*_tmp4E8=(
void*)_tmp4E6->type;Cyc_Toc_exp_to_c(nv,_tmp4E7);if(Cyc_Toc_is_void_star(_tmp4E8))(
void*)(_tmp4E7->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp4E7->r,0)))->r));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,_tmp319,0))->r));};_pop_region(rgn);}goto _LL1E3;}_LL228: if(*((int*)_tmp2D2)!= 
32)goto _LL22A;_tmp31A=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2D2)->f1;if(_tmp31A
!= 0)goto _LL22A;_tmp31B=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2D2)->f2;_tmp31C=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp2D2)->f3;_LL229: {struct _tuple1*qv=_tmp31C->name;
struct Cyc_Absyn_Exp*tag_exp=_tmp31B->is_xtunion?Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(
_tmp31B,qv,0);(void*)(e->r=(void*)((void*)tag_exp->r));goto _LL1E3;}_LL22A: if(*((
int*)_tmp2D2)!= 32)goto _LL22C;_tmp31D=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2D2)->f1;
_tmp31E=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2D2)->f2;_tmp31F=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2D2)->f3;_LL22B: {struct _tuple1*_tmp4E9=_tmp31F->name;struct Cyc_List_List*
_tmp4EA=_tmp31F->typs;struct _tuple1*_tmp4EB=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp4EC=Cyc_Absyn_var_exp(_tmp4EB,0);void*_tmp4ED=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4E9,({const char*_tmp4FB="_struct";_tag_dynforward(_tmp4FB,sizeof(char),
_get_zero_arr_size(_tmp4FB,8));})));struct Cyc_Absyn_Exp*tag_exp=_tmp31E->is_xtunion?
Cyc_Absyn_var_exp(_tmp4E9,0): Cyc_Toc_tunion_tag(_tmp31E,_tmp4E9,1);if(nv->toplevel){
struct Cyc_List_List*dles=0;for(0;_tmp31D != 0;(_tmp31D=_tmp31D->tl,_tmp4EA=
_tmp4EA->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp31D->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp4EA))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);dles=({struct Cyc_List_List*_tmp4EE=
_cycalloc(sizeof(*_tmp4EE));_tmp4EE->hd=({struct _tuple6*_tmp4EF=_cycalloc(
sizeof(*_tmp4EF));_tmp4EF->f1=0;_tmp4EF->f2=cur_e;_tmp4EF;});_tmp4EE->tl=dles;
_tmp4EE;});}dles=({struct Cyc_List_List*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));
_tmp4F0->hd=({struct _tuple6*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1->f1=0;
_tmp4F1->f2=tag_exp;_tmp4F1;});_tmp4F0->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp4F0;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->v=Cyc_Toc_collapse_qvar_tag(
_tmp4E9,({const char*_tmp4F3="_struct";_tag_dynforward(_tmp4F3,sizeof(char),
_get_zero_arr_size(_tmp4F3,8));}));_tmp4F2;}),dles,0))->r));}else{struct Cyc_List_List*
_tmp4F4=({struct Cyc_List_List*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp4EC,Cyc_Toc_tag_sp,0),tag_exp,
0);_tmp4FA->tl=0;_tmp4FA;});{int i=1;for(0;_tmp31D != 0;(((_tmp31D=_tmp31D->tl,i ++)),
_tmp4EA=_tmp4EA->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp31D->hd;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp4EA))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp4F5=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp4EC,Cyc_Absyn_fieldname(i),0),
cur_e,0);_tmp4F4=({struct Cyc_List_List*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));
_tmp4F6->hd=_tmp4F5;_tmp4F6->tl=_tmp4F4;_tmp4F6;});}}}{struct Cyc_Absyn_Stmt*
_tmp4F7=Cyc_Absyn_exp_stmt(_tmp4EC,0);struct Cyc_Absyn_Stmt*_tmp4F8=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->hd=_tmp4F7;_tmp4F9->tl=_tmp4F4;
_tmp4F9;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp4EB,_tmp4ED,0,_tmp4F8,0),0))->r));}}goto _LL1E3;}_LL22C: if(*((int*)_tmp2D2)!= 
33)goto _LL22E;_LL22D: goto _LL22F;_LL22E: if(*((int*)_tmp2D2)!= 34)goto _LL230;
_LL22F: goto _LL1E3;_LL230: if(*((int*)_tmp2D2)!= 35)goto _LL232;_tmp320=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp2D2)->f1;_tmp321=_tmp320.is_calloc;_tmp322=_tmp320.rgn;_tmp323=_tmp320.elt_type;
_tmp324=_tmp320.num_elts;_tmp325=_tmp320.fat_result;_LL231: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp323)));Cyc_Toc_exp_to_c(nv,_tmp324);if(_tmp325){struct
_tuple1*_tmp4FC=Cyc_Toc_temp_var();struct _tuple1*_tmp4FD=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp321){xexp=_tmp324;if(_tmp322 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp322;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp4FC,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp323,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp4FC,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp4FE[3];_tmp4FE[2]=Cyc_Absyn_var_exp(_tmp4FC,0);_tmp4FE[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp4FE[0]=Cyc_Absyn_var_exp(_tmp4FD,0);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4FE,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp324,0);if(_tmp322 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp322;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp4FC,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp323,Cyc_Absyn_var_exp(_tmp4FC,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp4FF[3];_tmp4FF[2]=Cyc_Absyn_var_exp(_tmp4FC,0);_tmp4FF[1]=Cyc_Absyn_uint_exp(
1,0);_tmp4FF[0]=Cyc_Absyn_var_exp(_tmp4FD,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4FF,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp500=Cyc_Absyn_declare_stmt(_tmp4FC,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp4FD,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp500,0))->r));}}else{if(_tmp322 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp322;Cyc_Toc_exp_to_c(nv,rgn);(
void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,
0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp323,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1E3;}_LL232: if(*((int*)_tmp2D2)!= 37)goto _LL234;_tmp326=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp2D2)->f1;_LL233: Cyc_Toc_stmt_to_c(nv,
_tmp326);goto _LL1E3;_LL234: if(*((int*)_tmp2D2)!= 36)goto _LL236;_LL235:({void*
_tmp501[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp502="UnresolvedMem";_tag_dynforward(_tmp502,sizeof(char),
_get_zero_arr_size(_tmp502,14));}),_tag_dynforward(_tmp501,sizeof(void*),0));});
_LL236: if(*((int*)_tmp2D2)!= 27)goto _LL1E3;_LL237:({void*_tmp503[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp504="compoundlit";_tag_dynforward(_tmp504,sizeof(char),_get_zero_arr_size(
_tmp504,12));}),_tag_dynforward(_tmp503,sizeof(void*),0));});_LL1E3:;}}static
struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple12{struct Cyc_Toc_Env*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple13{struct _tuple1*
f1;void*f2;};struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
static struct _tuple12 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*
rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,
struct Cyc_Absyn_Stmt**tag_fail_stmt,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*
decls){struct Cyc_Absyn_Stmt*s;{void*_tmp505=(void*)p->r;struct Cyc_Absyn_Vardecl*
_tmp506;struct Cyc_Absyn_Vardecl _tmp507;struct _tuple1*_tmp508;struct Cyc_Absyn_Pat*
_tmp509;struct Cyc_Absyn_Vardecl*_tmp50A;struct Cyc_Absyn_Vardecl _tmp50B;struct
_tuple1*_tmp50C;struct Cyc_Absyn_Vardecl*_tmp50D;struct Cyc_Absyn_Pat*_tmp50E;void*
_tmp50F;int _tmp510;char _tmp511;struct _dynforward_ptr _tmp512;struct Cyc_Absyn_Enumdecl*
_tmp513;struct Cyc_Absyn_Enumfield*_tmp514;void*_tmp515;struct Cyc_Absyn_Enumfield*
_tmp516;struct Cyc_Absyn_Tuniondecl*_tmp517;struct Cyc_Absyn_Tunionfield*_tmp518;
struct Cyc_List_List*_tmp519;struct Cyc_Absyn_Pat*_tmp51A;struct Cyc_Absyn_Pat
_tmp51B;void*_tmp51C;struct Cyc_Absyn_Tuniondecl*_tmp51D;struct Cyc_Absyn_Tunionfield*
_tmp51E;struct Cyc_List_List*_tmp51F;struct Cyc_List_List*_tmp520;struct Cyc_List_List*
_tmp521;struct Cyc_Absyn_AggrInfo _tmp522;void*_tmp523;struct Cyc_List_List*_tmp524;
struct Cyc_Absyn_Pat*_tmp525;_LL31E: if(_tmp505 <= (void*)2)goto _LL322;if(*((int*)
_tmp505)!= 0)goto _LL320;_tmp506=((struct Cyc_Absyn_Var_p_struct*)_tmp505)->f1;
_tmp507=*_tmp506;_tmp508=_tmp507.name;_tmp509=((struct Cyc_Absyn_Var_p_struct*)
_tmp505)->f2;_LL31F: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(nv,_tmp508,r),rgn,
t,r,path,_tmp509,tag_fail_stmt,fail_stmt,decls);_LL320: if(*((int*)_tmp505)!= 2)
goto _LL322;_tmp50A=((struct Cyc_Absyn_TagInt_p_struct*)_tmp505)->f2;_tmp50B=*
_tmp50A;_tmp50C=_tmp50B.name;_LL321: nv=Cyc_Toc_add_varmap(nv,_tmp50C,r);goto
_LL323;_LL322: if((int)_tmp505 != 0)goto _LL324;_LL323: s=Cyc_Toc_skip_stmt_dl();
goto _LL31D;_LL324: if(_tmp505 <= (void*)2)goto _LL326;if(*((int*)_tmp505)!= 1)goto
_LL326;_tmp50D=((struct Cyc_Absyn_Reference_p_struct*)_tmp505)->f1;_tmp50E=((
struct Cyc_Absyn_Reference_p_struct*)_tmp505)->f2;_LL325: {struct _tuple1*_tmp526=
Cyc_Toc_temp_var();decls=({struct Cyc_List_List*_tmp527=_region_malloc(rgn,
sizeof(*_tmp527));_tmp527->hd=({struct _tuple13*_tmp528=_region_malloc(rgn,
sizeof(*_tmp528));_tmp528->f1=_tmp526;_tmp528->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq);_tmp528;});_tmp527->tl=decls;_tmp527;});nv=Cyc_Toc_add_varmap(
nv,_tmp50D->name,Cyc_Absyn_var_exp(_tmp526,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp526,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(path,0)),0);{struct _tuple12 _tmp529=Cyc_Toc_xlate_pat(nv,
rgn,t,r,path,_tmp50E,tag_fail_stmt,fail_stmt,decls);_tmp529.f3=Cyc_Absyn_seq_stmt(
s,_tmp529.f3,0);return _tmp529;}}_LL326: if((int)_tmp505 != 1)goto _LL328;_LL327: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL31D;_LL328:
if(_tmp505 <= (void*)2)goto _LL32A;if(*((int*)_tmp505)!= 7)goto _LL32A;_tmp50F=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp505)->f1;_tmp510=((struct Cyc_Absyn_Int_p_struct*)
_tmp505)->f2;_LL329: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp50F,_tmp510,0),
fail_stmt);goto _LL31D;_LL32A: if(_tmp505 <= (void*)2)goto _LL32C;if(*((int*)_tmp505)
!= 8)goto _LL32C;_tmp511=((struct Cyc_Absyn_Char_p_struct*)_tmp505)->f1;_LL32B: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp511,0),fail_stmt);goto _LL31D;_LL32C:
if(_tmp505 <= (void*)2)goto _LL32E;if(*((int*)_tmp505)!= 9)goto _LL32E;_tmp512=((
struct Cyc_Absyn_Float_p_struct*)_tmp505)->f1;_LL32D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp512,0),fail_stmt);goto _LL31D;_LL32E: if(_tmp505 <= (void*)2)goto _LL330;if(*((
int*)_tmp505)!= 10)goto _LL330;_tmp513=((struct Cyc_Absyn_Enum_p_struct*)_tmp505)->f1;
_tmp514=((struct Cyc_Absyn_Enum_p_struct*)_tmp505)->f2;_LL32F: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp52A=_cycalloc(
sizeof(*_tmp52A));_tmp52A[0]=({struct Cyc_Absyn_Enum_e_struct _tmp52B;_tmp52B.tag=
33;_tmp52B.f1=_tmp514->name;_tmp52B.f2=(struct Cyc_Absyn_Enumdecl*)_tmp513;
_tmp52B.f3=(struct Cyc_Absyn_Enumfield*)_tmp514;_tmp52B;});_tmp52A;}),0),
fail_stmt);goto _LL31D;_LL330: if(_tmp505 <= (void*)2)goto _LL332;if(*((int*)_tmp505)
!= 11)goto _LL332;_tmp515=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp505)->f1;
_tmp516=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp505)->f2;_LL331: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp52C=_cycalloc(
sizeof(*_tmp52C));_tmp52C[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp52D;_tmp52D.tag=
34;_tmp52D.f1=_tmp516->name;_tmp52D.f2=(void*)_tmp515;_tmp52D.f3=(struct Cyc_Absyn_Enumfield*)
_tmp516;_tmp52D;});_tmp52C;}),0),fail_stmt);goto _LL31D;_LL332: if(_tmp505 <= (void*)
2)goto _LL334;if(*((int*)_tmp505)!= 6)goto _LL334;_tmp517=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp505)->f1;_tmp518=((struct Cyc_Absyn_Tunion_p_struct*)_tmp505)->f2;_tmp519=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp505)->f3;if(_tmp519 != 0)goto _LL334;_LL333: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp517->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(
_tmp518->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(_tmp517,_tmp518->name,0);r=Cyc_Toc_cast_it(
Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);goto _LL31D;}
_LL334: if(_tmp505 <= (void*)2)goto _LL336;if(*((int*)_tmp505)!= 4)goto _LL336;
_tmp51A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp505)->f1;_tmp51B=*_tmp51A;
_tmp51C=(void*)_tmp51B.r;if(_tmp51C <= (void*)2)goto _LL336;if(*((int*)_tmp51C)!= 
6)goto _LL336;_tmp51D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp51C)->f1;_tmp51E=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp51C)->f2;_tmp51F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp51C)->f3;if(!(_tmp51F != 0))goto _LL336;_LL335: s=Cyc_Toc_skip_stmt_dl();{int
cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp51E->name,({const char*
_tmp53A="_struct";_tag_dynforward(_tmp53A,sizeof(char),_get_zero_arr_size(
_tmp53A,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp52E=_tmp51E->typs;
for(0;_tmp51F != 0;(((_tmp51F=_tmp51F->tl,_tmp52E=((struct Cyc_List_List*)
_check_null(_tmp52E))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp52F=(struct Cyc_Absyn_Pat*)
_tmp51F->hd;if((void*)_tmp52F->r == (void*)0)continue;{void*_tmp530=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp52E))->hd)).f2;struct _tuple1*
_tmp531=Cyc_Toc_temp_var();void*_tmp532=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp52F->topt))->v;void*_tmp533=Cyc_Toc_typ_to_c(_tmp532);struct Cyc_Absyn_Exp*
_tmp534=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp530)))_tmp534=Cyc_Toc_cast_it(_tmp533,_tmp534);decls=({
struct Cyc_List_List*_tmp535=_region_malloc(rgn,sizeof(*_tmp535));_tmp535->hd=({
struct _tuple13*_tmp536=_region_malloc(rgn,sizeof(*_tmp536));_tmp536->f1=_tmp531;
_tmp536->f2=_tmp533;_tmp536;});_tmp535->tl=decls;_tmp535;});{struct _tuple12
_tmp537=Cyc_Toc_xlate_pat(nv,rgn,_tmp532,Cyc_Absyn_var_exp(_tmp531,0),_tmp534,
_tmp52F,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp537.f1;decls=
_tmp537.f2;{struct Cyc_Absyn_Stmt*_tmp538=_tmp537.f3;struct Cyc_Absyn_Stmt*_tmp539=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp531,0),_tmp534,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp539,_tmp538,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp51D->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp51E->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp51D,_tmp51E->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp51D);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL31D;}}_LL336: if(_tmp505 <= (void*)2)goto
_LL338;if(*((int*)_tmp505)!= 6)goto _LL338;_tmp520=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp505)->f3;_LL337: _tmp521=_tmp520;goto _LL339;_LL338: if(_tmp505 <= (void*)2)goto
_LL33A;if(*((int*)_tmp505)!= 3)goto _LL33A;_tmp521=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp505)->f1;_LL339: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp521 != 0;(
_tmp521=_tmp521->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp53B=(struct Cyc_Absyn_Pat*)
_tmp521->hd;if((void*)_tmp53B->r == (void*)0)continue;{struct _tuple1*_tmp53C=Cyc_Toc_temp_var();
void*_tmp53D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp53B->topt))->v;decls=({
struct Cyc_List_List*_tmp53E=_region_malloc(rgn,sizeof(*_tmp53E));_tmp53E->hd=({
struct _tuple13*_tmp53F=_region_malloc(rgn,sizeof(*_tmp53F));_tmp53F->f1=_tmp53C;
_tmp53F->f2=Cyc_Toc_typ_to_c(_tmp53D);_tmp53F;});_tmp53E->tl=decls;_tmp53E;});{
struct _tuple12 _tmp540=Cyc_Toc_xlate_pat(nv,rgn,_tmp53D,Cyc_Absyn_var_exp(_tmp53C,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp53B,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp540.f1;decls=_tmp540.f2;{struct Cyc_Absyn_Stmt*
_tmp541=_tmp540.f3;struct Cyc_Absyn_Stmt*_tmp542=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp53C,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp542,_tmp541,0),0);}}}}goto _LL31D;}_LL33A: if(_tmp505 <= (
void*)2)goto _LL33C;if(*((int*)_tmp505)!= 5)goto _LL33C;_tmp522=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp505)->f1;_tmp523=(void*)_tmp522.aggr_info;_tmp524=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp505)->f3;_LL33B: {struct Cyc_Absyn_Aggrdecl*_tmp543=Cyc_Absyn_get_known_aggrdecl(
_tmp523);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp524 != 0;_tmp524=_tmp524->tl){struct
_tuple14*_tmp544=(struct _tuple14*)_tmp524->hd;struct Cyc_Absyn_Pat*_tmp545=(*
_tmp544).f2;if((void*)_tmp545->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp546=(void*)((struct Cyc_List_List*)_check_null((*_tmp544).f1))->hd;
struct _dynforward_ptr*_tmp547;_LL345: if(*((int*)_tmp546)!= 1)goto _LL347;_tmp547=((
struct Cyc_Absyn_FieldName_struct*)_tmp546)->f1;_LL346: f=_tmp547;goto _LL344;
_LL347:;_LL348:(int)_throw((void*)Cyc_Toc_Match_error);_LL344:;}{struct _tuple1*
_tmp548=Cyc_Toc_temp_var();void*_tmp549=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp545->topt))->v;void*_tmp54A=Cyc_Toc_typ_to_c(_tmp549);decls=({struct Cyc_List_List*
_tmp54B=_region_malloc(rgn,sizeof(*_tmp54B));_tmp54B->hd=({struct _tuple13*
_tmp54C=_region_malloc(rgn,sizeof(*_tmp54C));_tmp54C->f1=_tmp548;_tmp54C->f2=
_tmp54A;_tmp54C;});_tmp54B->tl=decls;_tmp54B;});{struct _tuple12 _tmp54D=Cyc_Toc_xlate_pat(
nv,rgn,_tmp549,Cyc_Absyn_var_exp(_tmp548,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp545,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp54D.f1;decls=
_tmp54D.f2;{struct Cyc_Absyn_Exp*_tmp54E=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp543->impl))->fields,f)))->type))_tmp54E=
Cyc_Toc_cast_it(_tmp54A,_tmp54E);{struct Cyc_Absyn_Stmt*_tmp54F=_tmp54D.f3;struct
Cyc_Absyn_Stmt*_tmp550=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp548,0),
_tmp54E,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp550,_tmp54F,0),0);}}}}}}
goto _LL31D;}_LL33C: if(_tmp505 <= (void*)2)goto _LL33E;if(*((int*)_tmp505)!= 4)goto
_LL33E;_tmp525=((struct Cyc_Absyn_Pointer_p_struct*)_tmp505)->f1;_LL33D: {struct
_tuple1*_tmp551=Cyc_Toc_temp_var();void*_tmp552=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp525->topt))->v;decls=({struct Cyc_List_List*_tmp553=
_region_malloc(rgn,sizeof(*_tmp553));_tmp553->hd=({struct _tuple13*_tmp554=
_region_malloc(rgn,sizeof(*_tmp554));_tmp554->f1=_tmp551;_tmp554->f2=Cyc_Toc_typ_to_c(
_tmp552);_tmp554;});_tmp553->tl=decls;_tmp553;});{struct _tuple12 _tmp555=Cyc_Toc_xlate_pat(
nv,rgn,_tmp552,Cyc_Absyn_var_exp(_tmp551,0),Cyc_Absyn_deref_exp(path,0),_tmp525,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp555.f1;decls=_tmp555.f2;{
struct Cyc_Absyn_Stmt*_tmp556=_tmp555.f3;struct Cyc_Absyn_Stmt*_tmp557=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp551,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp556,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp557,0);else{s=_tmp557;}goto _LL31D;}}}_LL33E: if(_tmp505 <= (void*)2)goto
_LL340;if(*((int*)_tmp505)!= 12)goto _LL340;_LL33F:({void*_tmp558[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp559="unknownid pat";_tag_dynforward(_tmp559,sizeof(char),_get_zero_arr_size(
_tmp559,14));}),_tag_dynforward(_tmp558,sizeof(void*),0));});_LL340: if(_tmp505 <= (
void*)2)goto _LL342;if(*((int*)_tmp505)!= 13)goto _LL342;_LL341:({void*_tmp55A[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp55B="unknowncall pat";_tag_dynforward(_tmp55B,sizeof(char),
_get_zero_arr_size(_tmp55B,16));}),_tag_dynforward(_tmp55A,sizeof(void*),0));});
_LL342: if(_tmp505 <= (void*)2)goto _LL31D;if(*((int*)_tmp505)!= 14)goto _LL31D;
_LL343:({void*_tmp55C[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp55D="exp pat";
_tag_dynforward(_tmp55D,sizeof(char),_get_zero_arr_size(_tmp55D,8));}),
_tag_dynforward(_tmp55C,sizeof(void*),0));});_LL31D:;}return({struct _tuple12
_tmp55E;_tmp55E.f1=nv;_tmp55E.f2=decls;_tmp55E.f3=s;_tmp55E;});}struct _tuple15{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple15*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple15*_tmp55F=_region_malloc(r,sizeof(*_tmp55F));_tmp55F->f1=
Cyc_Toc_fresh_label();_tmp55F->f2=Cyc_Toc_fresh_label();_tmp55F->f3=sc;_tmp55F;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp560=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp561;void*_tmp562;struct Cyc_Absyn_Tuniondecl**
_tmp563;struct Cyc_Absyn_Tuniondecl*_tmp564;_LL34A: if(_tmp560 <= (void*)4)goto
_LL34C;if(*((int*)_tmp560)!= 2)goto _LL34C;_tmp561=((struct Cyc_Absyn_TunionType_struct*)
_tmp560)->f1;_tmp562=(void*)_tmp561.tunion_info;if(*((int*)_tmp562)!= 1)goto
_LL34C;_tmp563=((struct Cyc_Absyn_KnownTunion_struct*)_tmp562)->f1;_tmp564=*
_tmp563;_LL34B: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*_tmp565=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp564->fields))->v;for(
0;(unsigned int)_tmp565;_tmp565=_tmp565->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp565->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL349;}_LL34C:;_LL34D: goto _LL349;_LL349:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp566=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp567;struct Cyc_Absyn_Pat*_tmp568;struct Cyc_Absyn_Pat
_tmp569;void*_tmp56A;struct Cyc_Absyn_Tuniondecl*_tmp56B;struct Cyc_Absyn_Tunionfield*
_tmp56C;struct Cyc_List_List*_tmp56D;_LL34F: if(_tmp566 <= (void*)2)goto _LL353;if(*((
int*)_tmp566)!= 0)goto _LL351;_tmp567=((struct Cyc_Absyn_Var_p_struct*)_tmp566)->f2;
_LL350: return Cyc_Toc_no_tag_test(_tmp567);_LL351: if(*((int*)_tmp566)!= 4)goto
_LL353;_tmp568=((struct Cyc_Absyn_Pointer_p_struct*)_tmp566)->f1;_tmp569=*_tmp568;
_tmp56A=(void*)_tmp569.r;if(_tmp56A <= (void*)2)goto _LL353;if(*((int*)_tmp56A)!= 
6)goto _LL353;_tmp56B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp56A)->f1;_tmp56C=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp56A)->f2;_tmp56D=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp56A)->f3;if(!(_tmp56D != 0))goto _LL353;_LL352: return 0;_LL353:;_LL354: return 1;
_LL34E:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp56E=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp56F=Cyc_Tcutil_compress(_tmp56E);_LL356: if(_tmp56F <= (
void*)4)goto _LL35A;if(*((int*)_tmp56F)!= 5)goto _LL358;_LL357: goto _LL359;_LL358:
if(*((int*)_tmp56F)!= 12)goto _LL35A;_LL359: leave_as_switch=1;goto _LL355;_LL35A:;
_LL35B: leave_as_switch=0;goto _LL355;_LL355:;}{struct Cyc_List_List*_tmp570=scs;
for(0;_tmp570 != 0;_tmp570=_tmp570->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp570->hd)->pat_vars))->v != 0  || ((
struct Cyc_Absyn_Switch_clause*)_tmp570->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _dynforward_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp571=scs;for(0;_tmp571 != 0;_tmp571=_tmp571->tl){struct Cyc_Absyn_Stmt*
_tmp572=((struct Cyc_Absyn_Switch_clause*)_tmp571->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp571->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp572,0);next_l=Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(nv,next_l),_tmp572);}}return;}{
struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct _dynforward_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp573=_new_region("rgn");struct _RegionHandle*rgn=& _tmp573;
_push_region(rgn);{struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple15*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;int is_tunion=Cyc_Toc_is_mixed_tunion(_tmp56E);int needs_tag_test=
is_tunion;{struct Cyc_List_List*_tmp574=lscs;for(0;_tmp574 != 0;_tmp574=_tmp574->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple15*)_tmp574->hd)).f3;struct
_dynforward_ptr*fail_lab=_tmp574->tl == 0?end_l:(*((struct _tuple15*)((struct Cyc_List_List*)
_check_null(_tmp574->tl))->hd)).f1;struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(
needs_tag_test  && !Cyc_Toc_no_tag_test(sc->pattern)){{struct Cyc_List_List*
_tmp575=_tmp574->tl;for(0;(unsigned int)_tmp575;_tmp575=_tmp575->tl){if(Cyc_Toc_no_tag_test(((*((
struct _tuple15*)_tmp575->hd)).f3)->pattern)){tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp576=_region_malloc(rgn,sizeof(*_tmp576));_tmp576[0]=Cyc_Absyn_goto_stmt((*((
struct _tuple15*)_tmp575->hd)).f1,0);_tmp576;});needs_tag_test=0;break;}}}if(
tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp577=_region_malloc(
rgn,sizeof(*_tmp577));_tmp577[0]=Cyc_Absyn_goto_stmt(fail_lab,0);_tmp577;});}{
struct Cyc_Toc_Env*_tmp579;struct Cyc_List_List*_tmp57A;struct Cyc_Absyn_Stmt*
_tmp57B;struct _tuple12 _tmp578=Cyc_Toc_xlate_pat(nv,rgn,_tmp56E,r,path,sc->pattern,
tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp579=_tmp578.f1;_tmp57A=
_tmp578.f2;_tmp57B=_tmp578.f3;if(is_tunion  && Cyc_Toc_no_tag_test(sc->pattern))
needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp57C=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp579,_tmp57C);_tmp57B=Cyc_Absyn_seq_stmt(
_tmp57B,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,_tmp57C,0),Cyc_Absyn_goto_stmt(
fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp57A;nvs=({struct Cyc_List_List*
_tmp57D=_region_malloc(rgn,sizeof(*_tmp57D));_tmp57D->hd=_tmp579;_tmp57D->tl=nvs;
_tmp57D;});test_stmts=({struct Cyc_List_List*_tmp57E=_region_malloc(rgn,sizeof(*
_tmp57E));_tmp57E->hd=_tmp57B;_tmp57E->tl=test_stmts;_tmp57E;});}}}nvs=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple15 _tmp580;struct _dynforward_ptr*_tmp581;
struct _dynforward_ptr*_tmp582;struct Cyc_Absyn_Switch_clause*_tmp583;struct
_tuple15*_tmp57F=(struct _tuple15*)lscs->hd;_tmp580=*_tmp57F;_tmp581=_tmp580.f1;
_tmp582=_tmp580.f2;_tmp583=_tmp580.f3;{struct Cyc_Toc_Env*_tmp584=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp583->body;
if(lscs->tl != 0){struct _tuple15 _tmp586;struct _dynforward_ptr*_tmp587;struct Cyc_Absyn_Switch_clause*
_tmp588;struct _tuple15*_tmp585=(struct _tuple15*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp586=*_tmp585;_tmp587=_tmp586.f2;_tmp588=_tmp586.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp584,end_l,_tmp587,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp588->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp584,end_l),s);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_label_stmt(_tmp581,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)
_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(_tmp582,s,0),0);stmts=({
struct Cyc_List_List*_tmp589=_region_malloc(rgn,sizeof(*_tmp589));_tmp589->hd=s;
_tmp589->tl=stmts;_tmp589;});}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple13 _tmp58B;struct _tuple1*_tmp58C;void*_tmp58D;struct _tuple13*_tmp58A=(
struct _tuple13*)decls->hd;_tmp58B=*_tmp58A;_tmp58C=_tmp58B.f1;_tmp58D=_tmp58B.f2;
res=Cyc_Absyn_declare_stmt(_tmp58C,_tmp58D,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp58E=_cycalloc(sizeof(*_tmp58E));
_tmp58E->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp58E->tl=0;_tmp58E;}),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)(void*)(s->r=(
void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp58F=(void*)s->r;struct Cyc_Absyn_Exp*_tmp590;
struct Cyc_Absyn_Stmt*_tmp591;struct Cyc_Absyn_Stmt*_tmp592;struct Cyc_Absyn_Exp*
_tmp593;struct Cyc_Absyn_Exp*_tmp594;struct Cyc_Absyn_Stmt*_tmp595;struct Cyc_Absyn_Stmt*
_tmp596;struct _tuple3 _tmp597;struct Cyc_Absyn_Exp*_tmp598;struct Cyc_Absyn_Stmt*
_tmp599;struct Cyc_Absyn_Stmt*_tmp59A;struct Cyc_Absyn_Stmt*_tmp59B;struct Cyc_Absyn_Stmt*
_tmp59C;struct Cyc_Absyn_Exp*_tmp59D;struct _tuple3 _tmp59E;struct Cyc_Absyn_Exp*
_tmp59F;struct _tuple3 _tmp5A0;struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_Absyn_Stmt*
_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A3;struct Cyc_List_List*_tmp5A4;struct Cyc_List_List*
_tmp5A5;struct Cyc_Absyn_Switch_clause**_tmp5A6;struct Cyc_Absyn_Decl*_tmp5A7;
struct Cyc_Absyn_Stmt*_tmp5A8;struct _dynforward_ptr*_tmp5A9;struct Cyc_Absyn_Stmt*
_tmp5AA;struct Cyc_Absyn_Stmt*_tmp5AB;struct _tuple3 _tmp5AC;struct Cyc_Absyn_Exp*
_tmp5AD;struct Cyc_Absyn_Stmt*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct Cyc_Absyn_Tvar*
_tmp5B0;struct Cyc_Absyn_Vardecl*_tmp5B1;int _tmp5B2;struct Cyc_Absyn_Stmt*_tmp5B3;
struct Cyc_Absyn_Exp*_tmp5B4;_LL35D: if((int)_tmp58F != 0)goto _LL35F;_LL35E: return;
_LL35F: if(_tmp58F <= (void*)1)goto _LL361;if(*((int*)_tmp58F)!= 0)goto _LL361;
_tmp590=((struct Cyc_Absyn_Exp_s_struct*)_tmp58F)->f1;_LL360: Cyc_Toc_exp_to_c(nv,
_tmp590);return;_LL361: if(_tmp58F <= (void*)1)goto _LL363;if(*((int*)_tmp58F)!= 1)
goto _LL363;_tmp591=((struct Cyc_Absyn_Seq_s_struct*)_tmp58F)->f1;_tmp592=((struct
Cyc_Absyn_Seq_s_struct*)_tmp58F)->f2;_LL362: Cyc_Toc_stmt_to_c(nv,_tmp591);s=
_tmp592;continue;_LL363: if(_tmp58F <= (void*)1)goto _LL365;if(*((int*)_tmp58F)!= 2)
goto _LL365;_tmp593=((struct Cyc_Absyn_Return_s_struct*)_tmp58F)->f1;_LL364: {
struct Cyc_Core_Opt*topt=0;if(_tmp593 != 0){topt=({struct Cyc_Core_Opt*_tmp5B5=
_cycalloc(sizeof(*_tmp5B5));_tmp5B5->v=(void*)Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp593->topt))->v);_tmp5B5;});Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp593);}if(s->try_depth > 0){if(topt != 0){struct _tuple1*
_tmp5B6=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp5B7=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp5B6,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp5B6,(void*)topt->v,_tmp593,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp5B7,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL365: if(_tmp58F <= (void*)1)goto _LL367;if(*((int*)_tmp58F)!= 3)goto
_LL367;_tmp594=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp58F)->f1;_tmp595=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp58F)->f2;_tmp596=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp58F)->f3;_LL366: Cyc_Toc_exp_to_c(nv,_tmp594);Cyc_Toc_stmt_to_c(nv,_tmp595);s=
_tmp596;continue;_LL367: if(_tmp58F <= (void*)1)goto _LL369;if(*((int*)_tmp58F)!= 4)
goto _LL369;_tmp597=((struct Cyc_Absyn_While_s_struct*)_tmp58F)->f1;_tmp598=
_tmp597.f1;_tmp599=((struct Cyc_Absyn_While_s_struct*)_tmp58F)->f2;_LL368: Cyc_Toc_exp_to_c(
nv,_tmp598);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp599);return;_LL369: if(
_tmp58F <= (void*)1)goto _LL36B;if(*((int*)_tmp58F)!= 5)goto _LL36B;_tmp59A=((
struct Cyc_Absyn_Break_s_struct*)_tmp58F)->f1;_LL36A: if(nv->break_lab != 0)(void*)(
s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(nv->break_lab))->v,0))->r));{int dest_depth=_tmp59A == 0?0: _tmp59A->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}_LL36B: if(_tmp58F <= (
void*)1)goto _LL36D;if(*((int*)_tmp58F)!= 6)goto _LL36D;_tmp59B=((struct Cyc_Absyn_Continue_s_struct*)
_tmp58F)->f1;_LL36C: if(nv->continue_lab != 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((
struct _dynforward_ptr*)((struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));
_tmp59C=_tmp59B;goto _LL36E;_LL36D: if(_tmp58F <= (void*)1)goto _LL36F;if(*((int*)
_tmp58F)!= 7)goto _LL36F;_tmp59C=((struct Cyc_Absyn_Goto_s_struct*)_tmp58F)->f2;
_LL36E: Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(
_tmp59C))->try_depth,s);return;_LL36F: if(_tmp58F <= (void*)1)goto _LL371;if(*((int*)
_tmp58F)!= 8)goto _LL371;_tmp59D=((struct Cyc_Absyn_For_s_struct*)_tmp58F)->f1;
_tmp59E=((struct Cyc_Absyn_For_s_struct*)_tmp58F)->f2;_tmp59F=_tmp59E.f1;_tmp5A0=((
struct Cyc_Absyn_For_s_struct*)_tmp58F)->f3;_tmp5A1=_tmp5A0.f1;_tmp5A2=((struct
Cyc_Absyn_For_s_struct*)_tmp58F)->f4;_LL370: Cyc_Toc_exp_to_c(nv,_tmp59D);Cyc_Toc_exp_to_c(
nv,_tmp59F);Cyc_Toc_exp_to_c(nv,_tmp5A1);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp5A2);return;_LL371: if(_tmp58F <= (void*)1)goto _LL373;if(*((int*)_tmp58F)!= 9)
goto _LL373;_tmp5A3=((struct Cyc_Absyn_Switch_s_struct*)_tmp58F)->f1;_tmp5A4=((
struct Cyc_Absyn_Switch_s_struct*)_tmp58F)->f2;_LL372: Cyc_Toc_xlate_switch(nv,s,
_tmp5A3,_tmp5A4);return;_LL373: if(_tmp58F <= (void*)1)goto _LL375;if(*((int*)
_tmp58F)!= 10)goto _LL375;_tmp5A5=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp58F)->f1;
_tmp5A6=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp58F)->f2;_LL374: if(nv->fallthru_info
== 0)({void*_tmp5B8[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5B9="fallthru in unexpected place";
_tag_dynforward(_tmp5B9,sizeof(char),_get_zero_arr_size(_tmp5B9,29));}),
_tag_dynforward(_tmp5B8,sizeof(void*),0));});{struct _tuple8 _tmp5BB;struct
_dynforward_ptr*_tmp5BC;struct Cyc_List_List*_tmp5BD;struct Cyc_Dict_Dict*_tmp5BE;
struct _tuple8*_tmp5BA=(struct _tuple8*)((struct Cyc_Core_Opt*)_check_null(nv->fallthru_info))->v;
_tmp5BB=*_tmp5BA;_tmp5BC=_tmp5BB.f1;_tmp5BD=_tmp5BB.f2;_tmp5BE=_tmp5BB.f3;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp5BC,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp5A6)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp5BF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp5BD);struct Cyc_List_List*_tmp5C0=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp5A5);for(0;_tmp5BF != 0;(_tmp5BF=_tmp5BF->tl,
_tmp5C0=_tmp5C0->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5C0))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp5BE,(struct _tuple1*)_tmp5BF->hd),(struct Cyc_Absyn_Exp*)_tmp5C0->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL375: if(_tmp58F <= (void*)1)goto
_LL377;if(*((int*)_tmp58F)!= 11)goto _LL377;_tmp5A7=((struct Cyc_Absyn_Decl_s_struct*)
_tmp58F)->f1;_tmp5A8=((struct Cyc_Absyn_Decl_s_struct*)_tmp58F)->f2;_LL376:{void*
_tmp5C1=(void*)_tmp5A7->r;struct Cyc_Absyn_Vardecl*_tmp5C2;struct Cyc_Absyn_Pat*
_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C4;struct Cyc_List_List*_tmp5C5;struct Cyc_Absyn_Fndecl*
_tmp5C6;_LL382: if(_tmp5C1 <= (void*)2)goto _LL38A;if(*((int*)_tmp5C1)!= 0)goto
_LL384;_tmp5C2=((struct Cyc_Absyn_Var_d_struct*)_tmp5C1)->f1;_LL383: {struct Cyc_Toc_Env*
_tmp5C7=Cyc_Toc_add_varmap(nv,_tmp5C2->name,Cyc_Absyn_varb_exp(_tmp5C2->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));
_tmp5C8[0]=({struct Cyc_Absyn_Local_b_struct _tmp5C9;_tmp5C9.tag=3;_tmp5C9.f1=
_tmp5C2;_tmp5C9;});_tmp5C8;}),0));Cyc_Toc_local_decl_to_c(_tmp5C7,_tmp5C7,
_tmp5C2,_tmp5A8);goto _LL381;}_LL384: if(*((int*)_tmp5C1)!= 2)goto _LL386;_tmp5C3=((
struct Cyc_Absyn_Let_d_struct*)_tmp5C1)->f1;_tmp5C4=((struct Cyc_Absyn_Let_d_struct*)
_tmp5C1)->f3;_LL385:{void*_tmp5CA=(void*)_tmp5C3->r;struct Cyc_Absyn_Vardecl*
_tmp5CB;struct Cyc_Absyn_Pat*_tmp5CC;struct Cyc_Absyn_Pat _tmp5CD;void*_tmp5CE;
_LL38D: if(_tmp5CA <= (void*)2)goto _LL38F;if(*((int*)_tmp5CA)!= 0)goto _LL38F;
_tmp5CB=((struct Cyc_Absyn_Var_p_struct*)_tmp5CA)->f1;_tmp5CC=((struct Cyc_Absyn_Var_p_struct*)
_tmp5CA)->f2;_tmp5CD=*_tmp5CC;_tmp5CE=(void*)_tmp5CD.r;if((int)_tmp5CE != 0)goto
_LL38F;_LL38E: {struct _tuple1*old_name=_tmp5CB->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp5CB->name=new_name;_tmp5CB->initializer=(struct Cyc_Absyn_Exp*)_tmp5C4;(void*)(
_tmp5A7->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp5CF=_cycalloc(
sizeof(*_tmp5CF));_tmp5CF[0]=({struct Cyc_Absyn_Var_d_struct _tmp5D0;_tmp5D0.tag=0;
_tmp5D0.f1=_tmp5CB;_tmp5D0;});_tmp5CF;})));{struct Cyc_Toc_Env*_tmp5D1=Cyc_Toc_add_varmap(
nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2[0]=({struct Cyc_Absyn_Local_b_struct
_tmp5D3;_tmp5D3.tag=3;_tmp5D3.f1=_tmp5CB;_tmp5D3;});_tmp5D2;}),0));Cyc_Toc_local_decl_to_c(
_tmp5D1,nv,_tmp5CB,_tmp5A8);goto _LL38C;}}_LL38F:;_LL390:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp5C3,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5C4->topt))->v,_tmp5C4,_tmp5A8))->r));goto _LL38C;_LL38C:;}goto _LL381;_LL386:
if(*((int*)_tmp5C1)!= 3)goto _LL388;_tmp5C5=((struct Cyc_Absyn_Letv_d_struct*)
_tmp5C1)->f1;_LL387: {struct Cyc_List_List*_tmp5D4=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp5C5);if(_tmp5D4 == 0)({void*_tmp5D5[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp5D6="empty Letv_d";_tag_dynforward(_tmp5D6,sizeof(char),
_get_zero_arr_size(_tmp5D6,13));}),_tag_dynforward(_tmp5D5,sizeof(void*),0));});(
void*)(_tmp5A7->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp5D7=
_cycalloc(sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_Absyn_Var_d_struct _tmp5D8;
_tmp5D8.tag=0;_tmp5D8.f1=(struct Cyc_Absyn_Vardecl*)_tmp5D4->hd;_tmp5D8;});
_tmp5D7;})));_tmp5D4=_tmp5D4->tl;for(0;_tmp5D4 != 0;_tmp5D4=_tmp5D4->tl){struct
Cyc_Absyn_Decl*_tmp5D9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA[0]=({struct Cyc_Absyn_Var_d_struct
_tmp5DB;_tmp5DB.tag=0;_tmp5DB.f1=(struct Cyc_Absyn_Vardecl*)_tmp5D4->hd;_tmp5DB;});
_tmp5DA;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp5D9,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL381;}_LL388: if(*((int*)
_tmp5C1)!= 1)goto _LL38A;_tmp5C6=((struct Cyc_Absyn_Fn_d_struct*)_tmp5C1)->f1;
_LL389: {struct _tuple1*_tmp5DC=_tmp5C6->name;nv=Cyc_Toc_add_varmap(nv,_tmp5C6->name,
Cyc_Absyn_var_exp(_tmp5DC,0));Cyc_Toc_fndecl_to_c(nv,_tmp5C6,0);Cyc_Toc_stmt_to_c(
nv,_tmp5A8);goto _LL381;}_LL38A:;_LL38B:({void*_tmp5DD[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp5DE="bad nested declaration within function";_tag_dynforward(_tmp5DE,sizeof(
char),_get_zero_arr_size(_tmp5DE,39));}),_tag_dynforward(_tmp5DD,sizeof(void*),0));});
_LL381:;}return;_LL377: if(_tmp58F <= (void*)1)goto _LL379;if(*((int*)_tmp58F)!= 12)
goto _LL379;_tmp5A9=((struct Cyc_Absyn_Label_s_struct*)_tmp58F)->f1;_tmp5AA=((
struct Cyc_Absyn_Label_s_struct*)_tmp58F)->f2;_LL378: s=_tmp5AA;continue;_LL379:
if(_tmp58F <= (void*)1)goto _LL37B;if(*((int*)_tmp58F)!= 13)goto _LL37B;_tmp5AB=((
struct Cyc_Absyn_Do_s_struct*)_tmp58F)->f1;_tmp5AC=((struct Cyc_Absyn_Do_s_struct*)
_tmp58F)->f2;_tmp5AD=_tmp5AC.f1;_LL37A: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp5AB);Cyc_Toc_exp_to_c(nv,_tmp5AD);return;_LL37B: if(_tmp58F <= (void*)1)goto
_LL37D;if(*((int*)_tmp58F)!= 14)goto _LL37D;_tmp5AE=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp58F)->f1;_tmp5AF=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp58F)->f2;_LL37C: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->v=(void*)e_typ;_tmp5DF;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp5AE);{struct Cyc_Absyn_Stmt*_tmp5E0=Cyc_Absyn_seq_stmt(
_tmp5AE,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp5E1=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5E2=Cyc_Absyn_var_exp(
_tmp5E1,0);struct Cyc_Absyn_Vardecl*_tmp5E3=Cyc_Absyn_new_vardecl(_tmp5E1,Cyc_Absyn_exn_typ,
0);_tmp5E2->topt=({struct Cyc_Core_Opt*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));
_tmp5E4->v=(void*)Cyc_Absyn_exn_typ;_tmp5E4;});{struct Cyc_Absyn_Pat*_tmp5E5=({
struct Cyc_Absyn_Pat*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7[0]=({
struct Cyc_Absyn_Var_p_struct _tmp5F8;_tmp5F8.tag=0;_tmp5F8.f1=_tmp5E3;_tmp5F8.f2=({
struct Cyc_Absyn_Pat*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->r=(void*)((void*)
0);_tmp5F9->topt=({struct Cyc_Core_Opt*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));
_tmp5FA->v=(void*)Cyc_Absyn_exn_typ;_tmp5FA;});_tmp5F9->loc=0;_tmp5F9;});_tmp5F8;});
_tmp5F7;}));_tmp5F5->topt=({struct Cyc_Core_Opt*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));
_tmp5F6->v=(void*)Cyc_Absyn_exn_typ;_tmp5F6;});_tmp5F5->loc=0;_tmp5F5;});struct
Cyc_Absyn_Exp*_tmp5E6=Cyc_Absyn_throw_exp(_tmp5E2,0);_tmp5E6->topt=({struct Cyc_Core_Opt*
_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->v=(void*)((void*)0);_tmp5E7;});{
struct Cyc_Absyn_Stmt*_tmp5E8=Cyc_Absyn_exp_stmt(_tmp5E6,0);struct Cyc_Absyn_Switch_clause*
_tmp5E9=({struct Cyc_Absyn_Switch_clause*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));
_tmp5F2->pattern=_tmp5E5;_tmp5F2->pat_vars=({struct Cyc_Core_Opt*_tmp5F3=
_cycalloc(sizeof(*_tmp5F3));_tmp5F3->v=({struct Cyc_List_List*_tmp5F4=_cycalloc(
sizeof(*_tmp5F4));_tmp5F4->hd=_tmp5E3;_tmp5F4->tl=0;_tmp5F4;});_tmp5F3;});
_tmp5F2->where_clause=0;_tmp5F2->body=_tmp5E8;_tmp5F2->loc=0;_tmp5F2;});struct
Cyc_Absyn_Stmt*_tmp5EA=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5AF,({struct
Cyc_List_List*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->hd=_tmp5E9;_tmp5F1->tl=
0;_tmp5F1;})),0);Cyc_Toc_stmt_to_c(nv,_tmp5EA);{struct Cyc_Absyn_Exp*_tmp5EB=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));
_tmp5F0->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp5F0->tl=0;
_tmp5F0;}),0);struct Cyc_Absyn_Stmt*_tmp5EC=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));
_tmp5EF->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp5EF->tl=0;_tmp5EF;}),0),0);struct
Cyc_Absyn_Exp*_tmp5ED=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp5EE=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp5EC,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp5ED,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp5EB,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp5EE,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp5E0,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Toc_cast_it(
e_typ,Cyc_Toc__exn_thrown_e),_tmp5EA,0),0),0),0),0),0))->r));return;}}}}}_LL37D:
if(_tmp58F <= (void*)1)goto _LL37F;if(*((int*)_tmp58F)!= 15)goto _LL37F;_tmp5B0=((
struct Cyc_Absyn_Region_s_struct*)_tmp58F)->f1;_tmp5B1=((struct Cyc_Absyn_Region_s_struct*)
_tmp58F)->f2;_tmp5B2=((struct Cyc_Absyn_Region_s_struct*)_tmp58F)->f3;_tmp5B3=((
struct Cyc_Absyn_Region_s_struct*)_tmp58F)->f4;_LL37E: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp5B1->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp5B3);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp5FB->tl=0;_tmp5FB;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp5FC[1];_tmp5FC[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp5FC,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp5B3,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp5FD[1];_tmp5FD[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp5FD,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));return;}_LL37F: if(
_tmp58F <= (void*)1)goto _LL35C;if(*((int*)_tmp58F)!= 16)goto _LL35C;_tmp5B4=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmp58F)->f1;_LL380: Cyc_Toc_exp_to_c(nv,
_tmp5B4);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));
_tmp5FE->hd=_tmp5B4;_tmp5FE->tl=0;_tmp5FE;}),0),0))->r));return;_LL35C:;}}static
void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);struct _tuple16{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){f->tvs=0;f->effect=0;
f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{
struct Cyc_List_List*_tmp5FF=f->args;for(0;_tmp5FF != 0;_tmp5FF=_tmp5FF->tl){
struct _tuple1*_tmp600=({struct _tuple1*_tmp601=_cycalloc(sizeof(*_tmp601));
_tmp601->f1=(void*)((void*)0);_tmp601->f2=(*((struct _tuple16*)_tmp5FF->hd)).f1;
_tmp601;});(*((struct _tuple16*)_tmp5FF->hd)).f3=Cyc_Toc_typ_to_c((*((struct
_tuple16*)_tmp5FF->hd)).f3);nv=Cyc_Toc_add_varmap(nv,_tmp600,Cyc_Absyn_var_exp(
_tmp600,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);return;}if((
unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name
!= 0){struct Cyc_Core_Opt*_tmp603;struct Cyc_Absyn_Tqual _tmp604;void*_tmp605;int
_tmp606;struct Cyc_Absyn_VarargInfo _tmp602=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp603=_tmp602.name;_tmp604=_tmp602.tq;_tmp605=(
void*)_tmp602.type;_tmp606=_tmp602.inject;{void*_tmp607=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp605,(void*)2,_tmp604,Cyc_Absyn_false_conref));struct _tuple1*_tmp608=({struct
_tuple1*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->f1=(void*)((void*)0);
_tmp60B->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp603))->v;
_tmp60B;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp609=_cycalloc(sizeof(*
_tmp609));_tmp609->hd=({struct _tuple16*_tmp60A=_cycalloc(sizeof(*_tmp60A));
_tmp60A->f1=(struct _dynforward_ptr*)_tmp603->v;_tmp60A->f2=_tmp604;_tmp60A->f3=
_tmp607;_tmp60A;});_tmp609->tl=0;_tmp609;}));nv=Cyc_Toc_add_varmap(nv,_tmp608,
Cyc_Absyn_var_exp(_tmp608,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp60C=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp60C != 0;_tmp60C=_tmp60C->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp60C->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp60C->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(nv),f->body);}
static void*Cyc_Toc_scope_to_c(void*s){void*_tmp60D=s;_LL392: if((int)_tmp60D != 1)
goto _LL394;_LL393: return(void*)2;_LL394: if((int)_tmp60D != 4)goto _LL396;_LL395:
return(void*)3;_LL396:;_LL397: return s;_LL391:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=
0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp60E=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*
_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp60F;});{
int seen_defn_before;struct Cyc_Core_Opt*_tmp610=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp60E);if(_tmp610 == 
0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(
Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp60E,ad));}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp610->v)->impl == 0){(struct Cyc_Dict_Dict*)(((struct
Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,
_tmp60E,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(void*)((
void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po=
0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*_tmp611=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp611 != 0;_tmp611=_tmp611->tl){(void*)(((
struct Cyc_Absyn_Aggrfield*)_tmp611->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp611->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp612;});{
struct _tuple1*_tmp613=tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp613))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp613));{struct Cyc_List_List*
_tmp614=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp614 != 0;_tmp614=_tmp614->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp614->hd;if(f->typs != 0){struct Cyc_List_List*_tmp615=0;int i=1;{struct Cyc_List_List*
_tmp616=f->typs;for(0;_tmp616 != 0;(_tmp616=_tmp616->tl,i ++)){struct
_dynforward_ptr*_tmp617=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp618=({
struct Cyc_Absyn_Aggrfield*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->name=
_tmp617;_tmp61A->tq=(*((struct _tuple4*)_tmp616->hd)).f1;_tmp61A->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp616->hd)).f2);_tmp61A->width=0;_tmp61A->attributes=0;_tmp61A;});
_tmp615=({struct Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->hd=
_tmp618;_tmp619->tl=_tmp615;_tmp619;});}}_tmp615=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp615);_tmp615=({struct Cyc_List_List*
_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->hd=({struct Cyc_Absyn_Aggrfield*
_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->name=Cyc_Toc_tag_sp;_tmp61C->tq=Cyc_Toc_mt_tq;
_tmp61C->type=(void*)Cyc_Absyn_sint_typ;_tmp61C->width=0;_tmp61C->attributes=0;
_tmp61C;});_tmp61B->tl=_tmp615;_tmp61B;});{struct Cyc_Absyn_Aggrdecl*_tmp61D=({
struct Cyc_Absyn_Aggrdecl*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->kind=(void*)((
void*)0);_tmp621->sc=(void*)((void*)2);_tmp621->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp623="_struct";_tag_dynforward(_tmp623,sizeof(char),
_get_zero_arr_size(_tmp623,8));}));_tmp621->tvs=0;_tmp621->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->exist_vars=0;_tmp622->rgn_po=0;
_tmp622->fields=_tmp615;_tmp622;});_tmp621->attributes=0;_tmp621;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp61F=_cycalloc(sizeof(*_tmp61F));
_tmp61F[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp620;_tmp620.tag=4;_tmp620.f1=
_tmp61D;_tmp620;});_tmp61F;}),0);_tmp61E->tl=Cyc_Toc_result_decls;_tmp61E;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp624;});if(xd->fields == 0)return;{struct _tuple1*_tmp625=xd->name;struct Cyc_List_List*
_tmp626=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp626 != 0;_tmp626=_tmp626->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp626->hd;struct _dynforward_ptr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp627=
Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(char))+ 4,0);void*_tmp628=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp627,Cyc_Absyn_false_conref,
0);struct Cyc_Core_Opt*_tmp629=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);struct Cyc_Core_Opt
_tmp62A;int _tmp62B;_LL399: if(_tmp629 != 0)goto _LL39B;_LL39A: {struct Cyc_Absyn_Exp*
initopt=0;if((void*)f->sc != (void*)3){char zero='\000';initopt=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp632;
_tmp632.tag=0;_tmp632.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{
struct Cyc_Int_pa_struct _tmp631;_tmp631.tag=1;_tmp631.f1=(unsigned long)((int)
zero);{struct Cyc_Int_pa_struct _tmp630;_tmp630.tag=1;_tmp630.f1=(unsigned long)((
int)zero);{struct Cyc_Int_pa_struct _tmp62F;_tmp62F.tag=1;_tmp62F.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp62E;_tmp62E.tag=1;_tmp62E.f1=(
unsigned long)((int)zero);{void*_tmp62C[5]={& _tmp62E,& _tmp62F,& _tmp630,& _tmp631,&
_tmp632};Cyc_aprintf(({const char*_tmp62D="%c%c%c%c%s";_tag_dynforward(_tmp62D,
sizeof(char),_get_zero_arr_size(_tmp62D,11));}),_tag_dynforward(_tmp62C,sizeof(
void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*_tmp633=Cyc_Absyn_new_vardecl(f->name,
_tmp628,initopt);(void*)(_tmp633->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635[
0]=({struct Cyc_Absyn_Var_d_struct _tmp636;_tmp636.tag=0;_tmp636.f1=_tmp633;
_tmp636;});_tmp635;}),0);_tmp634->tl=Cyc_Toc_result_decls;_tmp634;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp637=f->typs;for(0;_tmp637 != 0;(_tmp637=_tmp637->tl,
i ++)){struct _dynforward_ptr*_tmp638=({struct _dynforward_ptr*_tmp63C=_cycalloc(
sizeof(*_tmp63C));_tmp63C[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp63F;_tmp63F.tag=1;_tmp63F.f1=(unsigned long)i;{void*_tmp63D[1]={& _tmp63F};
Cyc_aprintf(({const char*_tmp63E="f%d";_tag_dynforward(_tmp63E,sizeof(char),
_get_zero_arr_size(_tmp63E,4));}),_tag_dynforward(_tmp63D,sizeof(void*),1));}});
_tmp63C;});struct Cyc_Absyn_Aggrfield*_tmp639=({struct Cyc_Absyn_Aggrfield*_tmp63B=
_cycalloc(sizeof(*_tmp63B));_tmp63B->name=_tmp638;_tmp63B->tq=(*((struct _tuple4*)
_tmp637->hd)).f1;_tmp63B->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp637->hd)).f2);_tmp63B->width=0;_tmp63B->attributes=0;_tmp63B;});fields=({
struct Cyc_List_List*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->hd=_tmp639;
_tmp63A->tl=fields;_tmp63A;});}}fields=({struct Cyc_List_List*_tmp640=_cycalloc(
sizeof(*_tmp640));_tmp640->hd=({struct Cyc_Absyn_Aggrfield*_tmp641=_cycalloc(
sizeof(*_tmp641));_tmp641->name=Cyc_Toc_tag_sp;_tmp641->tq=Cyc_Toc_mt_tq;_tmp641->type=(
void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp641->width=0;
_tmp641->attributes=0;_tmp641;});_tmp640->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp640;});{struct Cyc_Absyn_Aggrdecl*_tmp642=({
struct Cyc_Absyn_Aggrdecl*_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646->kind=(void*)((
void*)0);_tmp646->sc=(void*)((void*)2);_tmp646->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp648="_struct";_tag_dynforward(_tmp648,sizeof(char),
_get_zero_arr_size(_tmp648,8));}));_tmp646->tvs=0;_tmp646->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647->exist_vars=0;_tmp647->rgn_po=0;
_tmp647->fields=fields;_tmp647;});_tmp646->attributes=0;_tmp646;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp644=_cycalloc(sizeof(*_tmp644));
_tmp644[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp645;_tmp645.tag=4;_tmp645.f1=
_tmp642;_tmp645;});_tmp644;}),0);_tmp643->tl=Cyc_Toc_result_decls;_tmp643;});}}
goto _LL398;}}_LL39B: if(_tmp629 == 0)goto _LL39D;_tmp62A=*_tmp629;_tmp62B=(int)
_tmp62A.v;if(_tmp62B != 0)goto _LL39D;_LL39C: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp649=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp654;_tmp654.tag=0;_tmp654.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp653;_tmp653.tag=1;_tmp653.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp652;_tmp652.tag=1;_tmp652.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp651;_tmp651.tag=1;_tmp651.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp650;_tmp650.tag=1;_tmp650.f1=(
unsigned long)((int)zero);{void*_tmp64E[5]={& _tmp650,& _tmp651,& _tmp652,& _tmp653,&
_tmp654};Cyc_aprintf(({const char*_tmp64F="%c%c%c%c%s";_tag_dynforward(_tmp64F,
sizeof(char),_get_zero_arr_size(_tmp64F,11));}),_tag_dynforward(_tmp64E,sizeof(
void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp64A=Cyc_Absyn_new_vardecl(f->name,
_tmp628,(struct Cyc_Absyn_Exp*)_tmp649);(void*)(_tmp64A->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp64B=_cycalloc(sizeof(*_tmp64B));
_tmp64B->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp64C=
_cycalloc(sizeof(*_tmp64C));_tmp64C[0]=({struct Cyc_Absyn_Var_d_struct _tmp64D;
_tmp64D.tag=0;_tmp64D.f1=_tmp64A;_tmp64D;});_tmp64C;}),0);_tmp64B->tl=Cyc_Toc_result_decls;
_tmp64B;});(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL398;_LL39D:;_LL39E: goto _LL398;_LL398:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp655=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp656;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*_tmp658;int _tmp659;
_LL3A0: if(*((int*)_tmp655)!= 29)goto _LL3A2;_tmp656=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp655)->f1;_tmp657=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp655)->f2;
_tmp658=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp655)->f3;_tmp659=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp655)->f4;_LL3A1: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp656,_tmp657,_tmp658,_tmp659,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL39F;_LL3A2:;_LL3A3: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp65A=({
struct Cyc_Toc_Env*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B[0]=*init_nv;
_tmp65B;});_tmp65A->toplevel=1;Cyc_Toc_exp_to_c(_tmp65A,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL39F;_LL39F:;}else{void*_tmp65C=Cyc_Tcutil_compress(old_typ);
struct Cyc_Absyn_ArrayInfo _tmp65D;void*_tmp65E;struct Cyc_Absyn_Exp*_tmp65F;struct
Cyc_Absyn_Conref*_tmp660;_LL3A5: if(_tmp65C <= (void*)4)goto _LL3A7;if(*((int*)
_tmp65C)!= 7)goto _LL3A7;_tmp65D=((struct Cyc_Absyn_ArrayType_struct*)_tmp65C)->f1;
_tmp65E=(void*)_tmp65D.elt_type;_tmp65F=_tmp65D.num_elts;_tmp660=_tmp65D.zero_term;
if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp660))goto
_LL3A7;_LL3A6: if(_tmp65F == 0)({void*_tmp661[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp662="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp662,sizeof(char),_get_zero_arr_size(_tmp662,55));}),
_tag_dynforward(_tmp661,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp65F;struct Cyc_Absyn_Exp*_tmp663=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp664=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp663,_tmp664,0),0),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));goto _LL3A4;}
_LL3A7:;_LL3A8: goto _LL3A4;_LL3A4:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp665=_cycalloc(
sizeof(*_tmp665));_tmp665[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp665;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp666=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp666;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp667=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp669;struct Cyc_List_List*
_tmp66A;struct Cyc_Absyn_Stmt*_tmp66B;struct _tuple12 _tmp668=Cyc_Toc_xlate_pat(nv,
prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)&
_tmp667,Cyc_Toc_throw_match_stmt(),0);_tmp669=_tmp668.f1;_tmp66A=_tmp668.f2;
_tmp66B=_tmp668.f3;Cyc_Toc_stmt_to_c(_tmp669,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp66B,s,0),0);for(0;_tmp66A != 0;
_tmp66A=_tmp66A->tl){struct _tuple13 _tmp66D;struct _tuple1*_tmp66E;void*_tmp66F;
struct _tuple13*_tmp66C=(struct _tuple13*)_tmp66A->hd;_tmp66D=*_tmp66C;_tmp66E=
_tmp66D.f1;_tmp66F=_tmp66D.f2;s=Cyc_Absyn_declare_stmt(_tmp66E,_tmp66F,0,s,0);}};
_pop_region(prgn);}return s;}}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*
e){void*_tmp670=(void*)e->r;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_Absyn_Exp*
_tmp672;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_Absyn_Exp*_tmp674;struct Cyc_Absyn_Exp*
_tmp675;struct Cyc_Absyn_Exp*_tmp676;struct Cyc_Absyn_Exp*_tmp677;struct Cyc_Absyn_Exp*
_tmp678;struct Cyc_List_List*_tmp679;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_Absyn_Exp*
_tmp67B;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*
_tmp67E;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_Absyn_Exp*
_tmp681;struct Cyc_Absyn_Exp*_tmp682;struct Cyc_Absyn_Exp*_tmp683;struct Cyc_Absyn_Exp*
_tmp684;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_Absyn_Exp*
_tmp687;struct Cyc_List_List*_tmp688;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_List_List*
_tmp68A;void*_tmp68B;void**_tmp68C;struct Cyc_Absyn_Exp*_tmp68D;struct _tuple2*
_tmp68E;struct _tuple2 _tmp68F;void*_tmp690;void**_tmp691;struct Cyc_List_List*
_tmp692;struct Cyc_List_List*_tmp693;struct Cyc_List_List*_tmp694;void*_tmp695;
void**_tmp696;void*_tmp697;void**_tmp698;struct Cyc_Absyn_Stmt*_tmp699;struct Cyc_Absyn_MallocInfo
_tmp69A;struct Cyc_Absyn_MallocInfo*_tmp69B;_LL3AA: if(*((int*)_tmp670)!= 22)goto
_LL3AC;_tmp671=((struct Cyc_Absyn_Deref_e_struct*)_tmp670)->f1;_LL3AB: _tmp672=
_tmp671;goto _LL3AD;_LL3AC: if(*((int*)_tmp670)!= 23)goto _LL3AE;_tmp672=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp670)->f1;_LL3AD: _tmp673=_tmp672;goto _LL3AF;
_LL3AE: if(*((int*)_tmp670)!= 24)goto _LL3B0;_tmp673=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp670)->f1;_LL3AF: _tmp674=_tmp673;goto _LL3B1;_LL3B0: if(*((int*)_tmp670)!= 16)
goto _LL3B2;_tmp674=((struct Cyc_Absyn_Address_e_struct*)_tmp670)->f1;_LL3B1:
_tmp675=_tmp674;goto _LL3B3;_LL3B2: if(*((int*)_tmp670)!= 12)goto _LL3B4;_tmp675=((
struct Cyc_Absyn_Throw_e_struct*)_tmp670)->f1;_LL3B3: _tmp676=_tmp675;goto _LL3B5;
_LL3B4: if(*((int*)_tmp670)!= 13)goto _LL3B6;_tmp676=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp670)->f1;_LL3B5: _tmp677=_tmp676;goto _LL3B7;_LL3B6: if(*((int*)_tmp670)!= 19)
goto _LL3B8;_tmp677=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp670)->f1;_LL3B7:
_tmp678=_tmp677;goto _LL3B9;_LL3B8: if(*((int*)_tmp670)!= 5)goto _LL3BA;_tmp678=((
struct Cyc_Absyn_Increment_e_struct*)_tmp670)->f1;_LL3B9: Cyc_Toc_exptypes_to_c(
_tmp678);goto _LL3A9;_LL3BA: if(*((int*)_tmp670)!= 3)goto _LL3BC;_tmp679=((struct
Cyc_Absyn_Primop_e_struct*)_tmp670)->f2;_LL3BB:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp679);goto _LL3A9;
_LL3BC: if(*((int*)_tmp670)!= 7)goto _LL3BE;_tmp67A=((struct Cyc_Absyn_And_e_struct*)
_tmp670)->f1;_tmp67B=((struct Cyc_Absyn_And_e_struct*)_tmp670)->f2;_LL3BD: _tmp67C=
_tmp67A;_tmp67D=_tmp67B;goto _LL3BF;_LL3BE: if(*((int*)_tmp670)!= 8)goto _LL3C0;
_tmp67C=((struct Cyc_Absyn_Or_e_struct*)_tmp670)->f1;_tmp67D=((struct Cyc_Absyn_Or_e_struct*)
_tmp670)->f2;_LL3BF: _tmp67E=_tmp67C;_tmp67F=_tmp67D;goto _LL3C1;_LL3C0: if(*((int*)
_tmp670)!= 9)goto _LL3C2;_tmp67E=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp670)->f1;
_tmp67F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp670)->f2;_LL3C1: _tmp680=_tmp67E;
_tmp681=_tmp67F;goto _LL3C3;_LL3C2: if(*((int*)_tmp670)!= 25)goto _LL3C4;_tmp680=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp670)->f1;_tmp681=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp670)->f2;_LL3C3: _tmp682=_tmp680;_tmp683=_tmp681;goto _LL3C5;_LL3C4: if(*((int*)
_tmp670)!= 4)goto _LL3C6;_tmp682=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp670)->f1;
_tmp683=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp670)->f3;_LL3C5: Cyc_Toc_exptypes_to_c(
_tmp682);Cyc_Toc_exptypes_to_c(_tmp683);goto _LL3A9;_LL3C6: if(*((int*)_tmp670)!= 
6)goto _LL3C8;_tmp684=((struct Cyc_Absyn_Conditional_e_struct*)_tmp670)->f1;
_tmp685=((struct Cyc_Absyn_Conditional_e_struct*)_tmp670)->f2;_tmp686=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp670)->f3;_LL3C7: Cyc_Toc_exptypes_to_c(_tmp684);Cyc_Toc_exptypes_to_c(_tmp685);
Cyc_Toc_exptypes_to_c(_tmp686);goto _LL3A9;_LL3C8: if(*((int*)_tmp670)!= 11)goto
_LL3CA;_tmp687=((struct Cyc_Absyn_FnCall_e_struct*)_tmp670)->f1;_tmp688=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp670)->f2;_LL3C9: _tmp689=_tmp687;_tmp68A=_tmp688;
goto _LL3CB;_LL3CA: if(*((int*)_tmp670)!= 10)goto _LL3CC;_tmp689=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp670)->f1;_tmp68A=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp670)->f2;_LL3CB:
Cyc_Toc_exptypes_to_c(_tmp689);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp68A);goto _LL3A9;_LL3CC:
if(*((int*)_tmp670)!= 15)goto _LL3CE;_tmp68B=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp670)->f1;_tmp68C=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp670)->f1);
_tmp68D=((struct Cyc_Absyn_Cast_e_struct*)_tmp670)->f2;_LL3CD:*_tmp68C=Cyc_Toc_typ_to_c(*
_tmp68C);Cyc_Toc_exptypes_to_c(_tmp68D);goto _LL3A9;_LL3CE: if(*((int*)_tmp670)!= 
27)goto _LL3D0;_tmp68E=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp670)->f1;
_tmp68F=*_tmp68E;_tmp690=_tmp68F.f3;_tmp691=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp670)->f1).f3;_tmp692=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp670)->f2;
_LL3CF:*_tmp691=Cyc_Toc_typ_to_c(*_tmp691);_tmp693=_tmp692;goto _LL3D1;_LL3D0: if(*((
int*)_tmp670)!= 36)goto _LL3D2;_tmp693=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp670)->f2;_LL3D1: _tmp694=_tmp693;goto _LL3D3;_LL3D2: if(*((int*)_tmp670)!= 28)
goto _LL3D4;_tmp694=((struct Cyc_Absyn_Array_e_struct*)_tmp670)->f1;_LL3D3: for(0;
_tmp694 != 0;_tmp694=_tmp694->tl){struct Cyc_Absyn_Exp*_tmp69D;struct _tuple6
_tmp69C=*((struct _tuple6*)_tmp694->hd);_tmp69D=_tmp69C.f2;Cyc_Toc_exptypes_to_c(
_tmp69D);}goto _LL3A9;_LL3D4: if(*((int*)_tmp670)!= 20)goto _LL3D6;_tmp695=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp670)->f1;_tmp696=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp670)->f1);_LL3D5: _tmp698=_tmp696;goto
_LL3D7;_LL3D6: if(*((int*)_tmp670)!= 18)goto _LL3D8;_tmp697=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp670)->f1;_tmp698=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp670)->f1);_LL3D7:*_tmp698=Cyc_Toc_typ_to_c(*_tmp698);goto _LL3A9;_LL3D8: if(*((
int*)_tmp670)!= 37)goto _LL3DA;_tmp699=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp670)->f1;_LL3D9: Cyc_Toc_stmttypes_to_c(_tmp699);goto _LL3A9;_LL3DA: if(*((int*)
_tmp670)!= 35)goto _LL3DC;_tmp69A=((struct Cyc_Absyn_Malloc_e_struct*)_tmp670)->f1;
_tmp69B=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp670)->f1;
_LL3DB: if(_tmp69B->elt_type != 0)_tmp69B->elt_type=({void**_tmp69E=_cycalloc(
sizeof(*_tmp69E));_tmp69E[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp69B->elt_type)));
_tmp69E;});Cyc_Toc_exptypes_to_c(_tmp69B->num_elts);goto _LL3A9;_LL3DC: if(*((int*)
_tmp670)!= 0)goto _LL3DE;_LL3DD: goto _LL3DF;_LL3DE: if(*((int*)_tmp670)!= 1)goto
_LL3E0;_LL3DF: goto _LL3E1;_LL3E0: if(*((int*)_tmp670)!= 2)goto _LL3E2;_LL3E1: goto
_LL3E3;_LL3E2: if(*((int*)_tmp670)!= 33)goto _LL3E4;_LL3E3: goto _LL3E5;_LL3E4: if(*((
int*)_tmp670)!= 34)goto _LL3E6;_LL3E5: goto _LL3A9;_LL3E6: if(*((int*)_tmp670)!= 31)
goto _LL3E8;_LL3E7: goto _LL3E9;_LL3E8: if(*((int*)_tmp670)!= 32)goto _LL3EA;_LL3E9:
goto _LL3EB;_LL3EA: if(*((int*)_tmp670)!= 30)goto _LL3EC;_LL3EB: goto _LL3ED;_LL3EC:
if(*((int*)_tmp670)!= 29)goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE: if(*((int*)_tmp670)
!= 26)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(*((int*)_tmp670)!= 14)goto _LL3F2;
_LL3F1: goto _LL3F3;_LL3F2: if(*((int*)_tmp670)!= 17)goto _LL3F4;_LL3F3: goto _LL3F5;
_LL3F4: if(*((int*)_tmp670)!= 21)goto _LL3A9;_LL3F5:({void*_tmp69F[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp6A0="Cyclone expression within C code";_tag_dynforward(
_tmp6A0,sizeof(char),_get_zero_arr_size(_tmp6A0,33));}),_tag_dynforward(_tmp69F,
sizeof(void*),0));});goto _LL3A9;_LL3A9:;}static void Cyc_Toc_decltypes_to_c(struct
Cyc_Absyn_Decl*d){void*_tmp6A1=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp6A2;
struct Cyc_Absyn_Fndecl*_tmp6A3;struct Cyc_Absyn_Aggrdecl*_tmp6A4;struct Cyc_Absyn_Enumdecl*
_tmp6A5;struct Cyc_Absyn_Typedefdecl*_tmp6A6;_LL3F7: if(_tmp6A1 <= (void*)2)goto
_LL40F;if(*((int*)_tmp6A1)!= 0)goto _LL3F9;_tmp6A2=((struct Cyc_Absyn_Var_d_struct*)
_tmp6A1)->f1;_LL3F8:(void*)(_tmp6A2->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp6A2->type));
if(_tmp6A2->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp6A2->initializer));goto _LL3F6;_LL3F9: if(*((int*)_tmp6A1)!= 1)
goto _LL3FB;_tmp6A3=((struct Cyc_Absyn_Fn_d_struct*)_tmp6A1)->f1;_LL3FA:(void*)(
_tmp6A3->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp6A3->ret_type));{struct Cyc_List_List*
_tmp6A7=_tmp6A3->args;for(0;_tmp6A7 != 0;_tmp6A7=_tmp6A7->tl){(*((struct _tuple16*)
_tmp6A7->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp6A7->hd)).f3);}}goto
_LL3F6;_LL3FB: if(*((int*)_tmp6A1)!= 4)goto _LL3FD;_tmp6A4=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp6A1)->f1;_LL3FC: Cyc_Toc_aggrdecl_to_c(_tmp6A4);goto _LL3F6;_LL3FD: if(*((int*)
_tmp6A1)!= 6)goto _LL3FF;_tmp6A5=((struct Cyc_Absyn_Enum_d_struct*)_tmp6A1)->f1;
_LL3FE: if(_tmp6A5->fields != 0){struct Cyc_List_List*_tmp6A8=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp6A5->fields))->v;for(0;_tmp6A8 != 0;_tmp6A8=
_tmp6A8->tl){struct Cyc_Absyn_Enumfield*_tmp6A9=(struct Cyc_Absyn_Enumfield*)
_tmp6A8->hd;if(_tmp6A9->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp6A9->tag));}}goto _LL3F6;_LL3FF: if(*((int*)_tmp6A1)!= 7)goto
_LL401;_tmp6A6=((struct Cyc_Absyn_Typedef_d_struct*)_tmp6A1)->f1;_LL400: _tmp6A6->defn=({
struct Cyc_Core_Opt*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6A6->defn))->v);_tmp6AA;});goto
_LL3F6;_LL401: if(*((int*)_tmp6A1)!= 2)goto _LL403;_LL402: goto _LL404;_LL403: if(*((
int*)_tmp6A1)!= 3)goto _LL405;_LL404: goto _LL406;_LL405: if(*((int*)_tmp6A1)!= 5)
goto _LL407;_LL406: goto _LL408;_LL407: if(*((int*)_tmp6A1)!= 8)goto _LL409;_LL408:
goto _LL40A;_LL409: if(*((int*)_tmp6A1)!= 9)goto _LL40B;_LL40A: goto _LL40C;_LL40B:
if(*((int*)_tmp6A1)!= 10)goto _LL40D;_LL40C: goto _LL40E;_LL40D: if(*((int*)_tmp6A1)
!= 11)goto _LL40F;_LL40E:({void*_tmp6AB[0]={};Cyc_Tcutil_terr(d->loc,({const char*
_tmp6AC="Cyclone declaration within C code";_tag_dynforward(_tmp6AC,sizeof(char),
_get_zero_arr_size(_tmp6AC,34));}),_tag_dynforward(_tmp6AB,sizeof(void*),0));});
goto _LL3F6;_LL40F: if((int)_tmp6A1 != 0)goto _LL411;_LL410: goto _LL412;_LL411: if((
int)_tmp6A1 != 1)goto _LL3F6;_LL412: goto _LL3F6;_LL3F6:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp6AD=(void*)s->r;struct Cyc_Absyn_Exp*_tmp6AE;
struct Cyc_Absyn_Stmt*_tmp6AF;struct Cyc_Absyn_Stmt*_tmp6B0;struct Cyc_Absyn_Exp*
_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B2;struct Cyc_Absyn_Stmt*_tmp6B3;struct Cyc_Absyn_Stmt*
_tmp6B4;struct _tuple3 _tmp6B5;struct Cyc_Absyn_Exp*_tmp6B6;struct Cyc_Absyn_Stmt*
_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B8;struct _tuple3 _tmp6B9;struct Cyc_Absyn_Exp*
_tmp6BA;struct _tuple3 _tmp6BB;struct Cyc_Absyn_Exp*_tmp6BC;struct Cyc_Absyn_Stmt*
_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BE;struct Cyc_List_List*_tmp6BF;struct Cyc_Absyn_Decl*
_tmp6C0;struct Cyc_Absyn_Stmt*_tmp6C1;struct Cyc_Absyn_Stmt*_tmp6C2;struct _tuple3
_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C4;_LL414: if(_tmp6AD <= (void*)1)goto _LL426;if(*((
int*)_tmp6AD)!= 0)goto _LL416;_tmp6AE=((struct Cyc_Absyn_Exp_s_struct*)_tmp6AD)->f1;
_LL415: Cyc_Toc_exptypes_to_c(_tmp6AE);goto _LL413;_LL416: if(*((int*)_tmp6AD)!= 1)
goto _LL418;_tmp6AF=((struct Cyc_Absyn_Seq_s_struct*)_tmp6AD)->f1;_tmp6B0=((struct
Cyc_Absyn_Seq_s_struct*)_tmp6AD)->f2;_LL417: Cyc_Toc_stmttypes_to_c(_tmp6AF);Cyc_Toc_stmttypes_to_c(
_tmp6B0);goto _LL413;_LL418: if(*((int*)_tmp6AD)!= 2)goto _LL41A;_tmp6B1=((struct
Cyc_Absyn_Return_s_struct*)_tmp6AD)->f1;_LL419: if(_tmp6B1 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp6B1);goto _LL413;_LL41A: if(*((int*)_tmp6AD)!= 3)goto
_LL41C;_tmp6B2=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6AD)->f1;_tmp6B3=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6AD)->f2;_tmp6B4=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp6AD)->f3;_LL41B: Cyc_Toc_exptypes_to_c(_tmp6B2);Cyc_Toc_stmttypes_to_c(
_tmp6B3);Cyc_Toc_stmttypes_to_c(_tmp6B4);goto _LL413;_LL41C: if(*((int*)_tmp6AD)!= 
4)goto _LL41E;_tmp6B5=((struct Cyc_Absyn_While_s_struct*)_tmp6AD)->f1;_tmp6B6=
_tmp6B5.f1;_tmp6B7=((struct Cyc_Absyn_While_s_struct*)_tmp6AD)->f2;_LL41D: Cyc_Toc_exptypes_to_c(
_tmp6B6);Cyc_Toc_stmttypes_to_c(_tmp6B7);goto _LL413;_LL41E: if(*((int*)_tmp6AD)!= 
8)goto _LL420;_tmp6B8=((struct Cyc_Absyn_For_s_struct*)_tmp6AD)->f1;_tmp6B9=((
struct Cyc_Absyn_For_s_struct*)_tmp6AD)->f2;_tmp6BA=_tmp6B9.f1;_tmp6BB=((struct
Cyc_Absyn_For_s_struct*)_tmp6AD)->f3;_tmp6BC=_tmp6BB.f1;_tmp6BD=((struct Cyc_Absyn_For_s_struct*)
_tmp6AD)->f4;_LL41F: Cyc_Toc_exptypes_to_c(_tmp6B8);Cyc_Toc_exptypes_to_c(_tmp6BA);
Cyc_Toc_exptypes_to_c(_tmp6BC);Cyc_Toc_stmttypes_to_c(_tmp6BD);goto _LL413;_LL420:
if(*((int*)_tmp6AD)!= 9)goto _LL422;_tmp6BE=((struct Cyc_Absyn_Switch_s_struct*)
_tmp6AD)->f1;_tmp6BF=((struct Cyc_Absyn_Switch_s_struct*)_tmp6AD)->f2;_LL421: Cyc_Toc_exptypes_to_c(
_tmp6BE);for(0;_tmp6BF != 0;_tmp6BF=_tmp6BF->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp6BF->hd)->body);}goto _LL413;_LL422: if(*((int*)
_tmp6AD)!= 11)goto _LL424;_tmp6C0=((struct Cyc_Absyn_Decl_s_struct*)_tmp6AD)->f1;
_tmp6C1=((struct Cyc_Absyn_Decl_s_struct*)_tmp6AD)->f2;_LL423: Cyc_Toc_decltypes_to_c(
_tmp6C0);Cyc_Toc_stmttypes_to_c(_tmp6C1);goto _LL413;_LL424: if(*((int*)_tmp6AD)!= 
13)goto _LL426;_tmp6C2=((struct Cyc_Absyn_Do_s_struct*)_tmp6AD)->f1;_tmp6C3=((
struct Cyc_Absyn_Do_s_struct*)_tmp6AD)->f2;_tmp6C4=_tmp6C3.f1;_LL425: Cyc_Toc_stmttypes_to_c(
_tmp6C2);Cyc_Toc_exptypes_to_c(_tmp6C4);goto _LL413;_LL426: if((int)_tmp6AD != 0)
goto _LL428;_LL427: goto _LL429;_LL428: if(_tmp6AD <= (void*)1)goto _LL42A;if(*((int*)
_tmp6AD)!= 5)goto _LL42A;_LL429: goto _LL42B;_LL42A: if(_tmp6AD <= (void*)1)goto
_LL42C;if(*((int*)_tmp6AD)!= 6)goto _LL42C;_LL42B: goto _LL42D;_LL42C: if(_tmp6AD <= (
void*)1)goto _LL42E;if(*((int*)_tmp6AD)!= 7)goto _LL42E;_LL42D: goto _LL413;_LL42E:
if(_tmp6AD <= (void*)1)goto _LL430;if(*((int*)_tmp6AD)!= 10)goto _LL430;_LL42F: goto
_LL431;_LL430: if(_tmp6AD <= (void*)1)goto _LL432;if(*((int*)_tmp6AD)!= 12)goto
_LL432;_LL431: goto _LL433;_LL432: if(_tmp6AD <= (void*)1)goto _LL434;if(*((int*)
_tmp6AD)!= 14)goto _LL434;_LL433: goto _LL435;_LL434: if(_tmp6AD <= (void*)1)goto
_LL436;if(*((int*)_tmp6AD)!= 15)goto _LL436;_LL435: goto _LL437;_LL436: if(_tmp6AD <= (
void*)1)goto _LL413;if(*((int*)_tmp6AD)!= 16)goto _LL413;_LL437:({void*_tmp6C5[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp6C6="Cyclone statement in C code";
_tag_dynforward(_tmp6C6,sizeof(char),_get_zero_arr_size(_tmp6C6,28));}),
_tag_dynforward(_tmp6C5,sizeof(void*),0));});goto _LL413;_LL413:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int
cinclude){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp6C7[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp6C8="decls_to_c: not at toplevel!";_tag_dynforward(_tmp6C8,sizeof(char),
_get_zero_arr_size(_tmp6C8,29));}),_tag_dynforward(_tmp6C7,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp6C9=(void*)d->r;
struct Cyc_Absyn_Vardecl*_tmp6CA;struct Cyc_Absyn_Fndecl*_tmp6CB;struct Cyc_Absyn_Aggrdecl*
_tmp6CC;struct Cyc_Absyn_Tuniondecl*_tmp6CD;struct Cyc_Absyn_Enumdecl*_tmp6CE;
struct Cyc_Absyn_Typedefdecl*_tmp6CF;struct Cyc_List_List*_tmp6D0;struct Cyc_List_List*
_tmp6D1;struct Cyc_List_List*_tmp6D2;struct Cyc_List_List*_tmp6D3;_LL439: if(
_tmp6C9 <= (void*)2)goto _LL449;if(*((int*)_tmp6C9)!= 0)goto _LL43B;_tmp6CA=((
struct Cyc_Absyn_Var_d_struct*)_tmp6C9)->f1;_LL43A: {struct _tuple1*_tmp6D4=
_tmp6CA->name;if((void*)_tmp6CA->sc == (void*)4)_tmp6D4=({struct _tuple1*_tmp6D5=
_cycalloc(sizeof(*_tmp6D5));_tmp6D5->f1=Cyc_Absyn_rel_ns_null;_tmp6D5->f2=(*
_tmp6D4).f2;_tmp6D5;});if(_tmp6CA->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp6CA->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp6CA->initializer));}}nv=Cyc_Toc_add_varmap(
nv,_tmp6CA->name,Cyc_Absyn_varb_exp(_tmp6D4,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({struct Cyc_Absyn_Global_b_struct
_tmp6D7;_tmp6D7.tag=0;_tmp6D7.f1=_tmp6CA;_tmp6D7;});_tmp6D6;}),0));_tmp6CA->name=
_tmp6D4;(void*)(_tmp6CA->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp6CA->sc));(void*)(
_tmp6CA->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp6CA->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8->hd=d;_tmp6D8->tl=
Cyc_Toc_result_decls;_tmp6D8;});goto _LL438;}_LL43B: if(*((int*)_tmp6C9)!= 1)goto
_LL43D;_tmp6CB=((struct Cyc_Absyn_Fn_d_struct*)_tmp6C9)->f1;_LL43C: {struct
_tuple1*_tmp6D9=_tmp6CB->name;if((void*)_tmp6CB->sc == (void*)4){_tmp6D9=({struct
_tuple1*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA->f1=Cyc_Absyn_rel_ns_null;
_tmp6DA->f2=(*_tmp6D9).f2;_tmp6DA;});(void*)(_tmp6CB->sc=(void*)((void*)2));}nv=
Cyc_Toc_add_varmap(nv,_tmp6CB->name,Cyc_Absyn_var_exp(_tmp6D9,0));_tmp6CB->name=
_tmp6D9;Cyc_Toc_fndecl_to_c(nv,_tmp6CB,cinclude);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB->hd=d;_tmp6DB->tl=Cyc_Toc_result_decls;
_tmp6DB;});goto _LL438;}_LL43D: if(*((int*)_tmp6C9)!= 2)goto _LL43F;_LL43E: goto
_LL440;_LL43F: if(*((int*)_tmp6C9)!= 3)goto _LL441;_LL440:({void*_tmp6DC[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp6DD="letdecl at toplevel";_tag_dynforward(_tmp6DD,sizeof(char),
_get_zero_arr_size(_tmp6DD,20));}),_tag_dynforward(_tmp6DC,sizeof(void*),0));});
_LL441: if(*((int*)_tmp6C9)!= 4)goto _LL443;_tmp6CC=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp6C9)->f1;_LL442: Cyc_Toc_aggrdecl_to_c(_tmp6CC);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->hd=d;_tmp6DE->tl=Cyc_Toc_result_decls;
_tmp6DE;});goto _LL438;_LL443: if(*((int*)_tmp6C9)!= 5)goto _LL445;_tmp6CD=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp6C9)->f1;_LL444: if(_tmp6CD->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp6CD);else{Cyc_Toc_tuniondecl_to_c(_tmp6CD);}goto _LL438;_LL445: if(*((int*)
_tmp6C9)!= 6)goto _LL447;_tmp6CE=((struct Cyc_Absyn_Enum_d_struct*)_tmp6C9)->f1;
_LL446: Cyc_Toc_enumdecl_to_c(nv,_tmp6CE);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->hd=d;_tmp6DF->tl=Cyc_Toc_result_decls;
_tmp6DF;});goto _LL438;_LL447: if(*((int*)_tmp6C9)!= 7)goto _LL449;_tmp6CF=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp6C9)->f1;_LL448: _tmp6CF->name=_tmp6CF->name;
_tmp6CF->tvs=0;if(_tmp6CF->defn != 0)_tmp6CF->defn=({struct Cyc_Core_Opt*_tmp6E0=
_cycalloc(sizeof(*_tmp6E0));_tmp6E0->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp6CF->defn))->v);_tmp6E0;});else{void*_tmp6E1=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6CF->kind))->v;_LL456: if((int)
_tmp6E1 != 2)goto _LL458;_LL457: _tmp6CF->defn=({struct Cyc_Core_Opt*_tmp6E2=
_cycalloc(sizeof(*_tmp6E2));_tmp6E2->v=(void*)Cyc_Absyn_void_star_typ();_tmp6E2;});
goto _LL455;_LL458:;_LL459: _tmp6CF->defn=({struct Cyc_Core_Opt*_tmp6E3=_cycalloc(
sizeof(*_tmp6E3));_tmp6E3->v=(void*)((void*)0);_tmp6E3;});goto _LL455;_LL455:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));
_tmp6E4->hd=d;_tmp6E4->tl=Cyc_Toc_result_decls;_tmp6E4;});goto _LL438;_LL449: if((
int)_tmp6C9 != 0)goto _LL44B;_LL44A: goto _LL44C;_LL44B: if((int)_tmp6C9 != 1)goto
_LL44D;_LL44C: goto _LL438;_LL44D: if(_tmp6C9 <= (void*)2)goto _LL44F;if(*((int*)
_tmp6C9)!= 8)goto _LL44F;_tmp6D0=((struct Cyc_Absyn_Namespace_d_struct*)_tmp6C9)->f2;
_LL44E: _tmp6D1=_tmp6D0;goto _LL450;_LL44F: if(_tmp6C9 <= (void*)2)goto _LL451;if(*((
int*)_tmp6C9)!= 9)goto _LL451;_tmp6D1=((struct Cyc_Absyn_Using_d_struct*)_tmp6C9)->f2;
_LL450: _tmp6D2=_tmp6D1;goto _LL452;_LL451: if(_tmp6C9 <= (void*)2)goto _LL453;if(*((
int*)_tmp6C9)!= 10)goto _LL453;_tmp6D2=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp6C9)->f1;_LL452: nv=Cyc_Toc_decls_to_c(nv,_tmp6D2,top,cinclude);goto _LL438;
_LL453: if(_tmp6C9 <= (void*)2)goto _LL438;if(*((int*)_tmp6C9)!= 11)goto _LL438;
_tmp6D3=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp6C9)->f1;_LL454: nv=Cyc_Toc_decls_to_c(
nv,_tmp6D3,top,1);goto _LL438;_LL438:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1,0);return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
