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
 typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};int Cyc_sscanf(struct _dynforward_ptr,struct
_dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[19];extern
char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[8];unsigned int Cyc_strlen(
struct _dynforward_ptr s);int Cyc_strcmp(struct _dynforward_ptr s1,struct
_dynforward_ptr s2);int Cyc_strncmp(struct _dynforward_ptr s1,struct _dynforward_ptr
s2,unsigned int len);struct _dynforward_ptr Cyc_strconcat(struct _dynforward_ptr,
struct _dynforward_ptr);extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{char*
tag;struct _dynforward_ptr f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr,struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;
int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};void Cyc_Arg_usage(struct Cyc_List_List*,struct _dynforward_ptr);
extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*
anonfun)(struct _dynforward_ptr),struct _dynforward_ptr errmsg,struct
_dynforward_ptr args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);struct _dynforward_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _dynforward_ptr,int offset,
int len);void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dynforward_ptr);int
isspace(int);void*Cyc___assert_fail(struct _dynforward_ptr assertion,struct
_dynforward_ptr file,unsigned int line);char Cyc_Arg_Bad[8]="\000\000\000\000Bad\000";
char Cyc_Arg_Error[10]="\000\000\000\000Error\000";struct Cyc_Arg_Prefix_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Arg_Exact_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Arg_Unknown_struct{int tag;struct _dynforward_ptr f1;
};struct Cyc_Arg_Missing_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Arg_Message_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Arg_Wrong_struct{int tag;struct
_dynforward_ptr f1;struct _dynforward_ptr f2;struct _dynforward_ptr f3;};struct
_tuple0{struct _dynforward_ptr f1;int f2;struct _dynforward_ptr f3;void*f4;struct
_dynforward_ptr f5;};static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct
_dynforward_ptr x){while(l != 0){struct _dynforward_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;
unsigned int _tmp1=Cyc_strlen((struct _dynforward_ptr)_tmp0);if(_tmp1 > 0  && (*((
struct _tuple0*)l->hd)).f2){if(Cyc_strncmp((struct _dynforward_ptr)x,(struct
_dynforward_ptr)(*((struct _tuple0*)l->hd)).f1,_tmp1)== 0)return(*((struct _tuple0*)
l->hd)).f4;}else{if(Cyc_strcmp((struct _dynforward_ptr)x,(struct _dynforward_ptr)(*((
struct _tuple0*)l->hd)).f1)== 0)return(*((struct _tuple0*)l->hd)).f4;}l=l->tl;}(
int)_throw((void*)Cyc_Core_Not_found);}static struct _dynforward_ptr Cyc_Arg_Justify_break_line(
struct Cyc_Buffer_t*b,int howmuch,struct _dynforward_ptr s){if(s.curr == ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr)return(struct _dynforward_ptr)
_tag_dynforward(0,0,0);if(howmuch < 0)howmuch=0;{unsigned int _tmp2=Cyc_strlen((
struct _dynforward_ptr)s);_tmp2 <= _get_dynforward_size(s,sizeof(char))?0:((int(*)(
struct _dynforward_ptr assertion,struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(({
const char*_tmp3="len <= s.size";_tag_dynforward(_tmp3,sizeof(char),
_get_zero_arr_size(_tmp3,14));}),({const char*_tmp4="arg.cyc";_tag_dynforward(
_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,8));}),94);if(howmuch > _tmp2){Cyc_Buffer_add_string(
b,s);return(struct _dynforward_ptr)_tag_dynforward(0,0,0);}{int i;for(i=howmuch - 1;
i >= 0  && !isspace((int)*((const char*)_check_dynforward_subscript(s,sizeof(char),
i)));i --){;}if(i < 0)for(i=howmuch?howmuch - 1: 0;(i < _tmp2  && (int)((const char*)s.curr)[
i]) && !isspace((int)((const char*)s.curr)[i]);i ++){;}Cyc_Buffer_add_substring(b,
s,0,i);{struct _dynforward_ptr whatsleft=(struct _dynforward_ptr)_tag_dynforward(0,
0,0);for(0;(i < _tmp2  && (int)((const char*)s.curr)[i]) && isspace((int)((const
char*)s.curr)[i]);i ++){;}if(i < _tmp2  && (int)((const char*)s.curr)[i])whatsleft=
_dynforward_ptr_plus(s,sizeof(char),i);return whatsleft;}}}}void Cyc_Arg_Justify_justify_b(
struct Cyc_Buffer_t*b,int indent,int margin,struct _dynforward_ptr item,struct
_dynforward_ptr desc){if(item.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,
0)).curr)Cyc_Buffer_add_string(b,item);if(desc.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return;if(indent < 0)indent=0;if(margin < 0)margin=0;{
struct _dynforward_ptr indentstr=({unsigned int _tmp11=(unsigned int)(indent + 2);
char*_tmp12=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp11 + 1));struct
_dynforward_ptr _tmp14=_tag_dynforward(_tmp12,sizeof(char),_tmp11 + 1);{
unsigned int _tmp13=_tmp11;unsigned int i;for(i=0;i < _tmp13;i ++){_tmp12[i]='\000';}
_tmp12[_tmp13]=(char)0;}_tmp14;});{unsigned int i=0;for(0;i < indent + 1;i ++){({
struct _dynforward_ptr _tmp5=_dynforward_ptr_plus(indentstr,sizeof(char),(int)i);
char _tmp6=*((char*)_check_dynforward_subscript(_tmp5,sizeof(char),0));char _tmp7=
i == 0?'\n':' ';if(_get_dynforward_size(_tmp5,sizeof(char))== 1  && (_tmp6 == '\000'
 && _tmp7 != '\000'))_throw_arraybounds();*((char*)_tmp5.curr)=_tmp7;});}}{
unsigned int _tmp8=Cyc_strlen((struct _dynforward_ptr)item);struct _dynforward_ptr
itemsep;if(Cyc_strlen((struct _dynforward_ptr)desc)> 0){if(_tmp8 + 1 > indent)
itemsep=indentstr;else{struct _dynforward_ptr temp=({unsigned int _tmpC=(indent - 
_tmp8)+ 1;char*_tmpD=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpC + 1));
struct _dynforward_ptr _tmpF=_tag_dynforward(_tmpD,sizeof(char),_tmpC + 1);{
unsigned int _tmpE=_tmpC;unsigned int i;for(i=0;i < _tmpE;i ++){_tmpD[i]='\000';}
_tmpD[_tmpE]=(char)0;}_tmpF;});{unsigned int i=0;for(0;i < indent - _tmp8;i ++){({
struct _dynforward_ptr _tmp9=_dynforward_ptr_plus(temp,sizeof(char),(int)i);char
_tmpA=*((char*)_check_dynforward_subscript(_tmp9,sizeof(char),0));char _tmpB=' ';
if(_get_dynforward_size(_tmp9,sizeof(char))== 1  && (_tmpA == '\000'  && _tmpB != '\000'))
_throw_arraybounds();*((char*)_tmp9.curr)=_tmpB;});}}itemsep=temp;}}else{return;}
Cyc_Buffer_add_string(b,(struct _dynforward_ptr)itemsep);while(desc.curr != ((
struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr){desc=Cyc_Arg_Justify_break_line(
b,margin - indent,desc);if(desc.curr != ((struct _dynforward_ptr)_tag_dynforward(0,
0,0)).curr)Cyc_Buffer_add_string(b,(struct _dynforward_ptr)indentstr);else{Cyc_Buffer_add_string(
b,({const char*_tmp10="\n";_tag_dynforward(_tmp10,sizeof(char),_get_zero_arr_size(
_tmp10,2));}));}}return;}}}void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct
_dynforward_ptr errmsg){({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)errmsg);{void*_tmp15[1]={& _tmp17};
Cyc_fprintf(Cyc_stderr,({const char*_tmp16="%s\n";_tag_dynforward(_tmp16,sizeof(
char),_get_zero_arr_size(_tmp16,4));}),_tag_dynforward(_tmp15,sizeof(void*),1));}});{
struct Cyc_Buffer_t*_tmp18=Cyc_Buffer_create(1024);while(speclist != 0){Cyc_Arg_Justify_justify_b(
_tmp18,12,72,(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)(*((
struct _tuple0*)speclist->hd)).f1,(struct _dynforward_ptr)(*((struct _tuple0*)
speclist->hd)).f3),(*((struct _tuple0*)speclist->hd)).f5);speclist=speclist->tl;}({
struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Buffer_contents(_tmp18));{void*_tmp19[1]={& _tmp1B};Cyc_fprintf(
Cyc_stderr,({const char*_tmp1A="%s";_tag_dynforward(_tmp1A,sizeof(char),
_get_zero_arr_size(_tmp1A,3));}),_tag_dynforward(_tmp19,sizeof(void*),1));}});}}
int Cyc_Arg_current=0;static struct _dynforward_ptr Cyc_Arg_args={(void*)0,(void*)(0
+ 0)};static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,
struct _dynforward_ptr errmsg){struct _dynforward_ptr progname=prog_pos < 
_get_dynforward_size(Cyc_Arg_args,sizeof(struct _dynforward_ptr))?*((struct
_dynforward_ptr*)_check_dynforward_subscript(Cyc_Arg_args,sizeof(struct
_dynforward_ptr),prog_pos)):({const char*_tmp36="(?)";_tag_dynforward(_tmp36,
sizeof(char),_get_zero_arr_size(_tmp36,4));});{void*_tmp1C=e;struct
_dynforward_ptr _tmp1D;struct _dynforward_ptr _tmp1E;struct _dynforward_ptr _tmp1F;
struct _dynforward_ptr _tmp20;struct _dynforward_ptr _tmp21;struct _dynforward_ptr
_tmp22;_LL1: if(*((int*)_tmp1C)!= 0)goto _LL3;_tmp1D=((struct Cyc_Arg_Unknown_struct*)
_tmp1C)->f1;_LL2: if(Cyc_strcmp((struct _dynforward_ptr)_tmp1D,({const char*_tmp23="-help";
_tag_dynforward(_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,6));}))!= 0)({
struct Cyc_String_pa_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp1D);{struct Cyc_String_pa_struct _tmp26;_tmp26.tag=0;
_tmp26.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)progname);{void*_tmp24[
2]={& _tmp26,& _tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp25="%s: unknown option `%s'.\n";
_tag_dynforward(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,26));}),
_tag_dynforward(_tmp24,sizeof(void*),2));}}});goto _LL0;_LL3: if(*((int*)_tmp1C)!= 
1)goto _LL5;_tmp1E=((struct Cyc_Arg_Missing_struct*)_tmp1C)->f1;_LL4:({struct Cyc_String_pa_struct
_tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp1E);{struct Cyc_String_pa_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)progname);{void*_tmp28[2]={& _tmp2A,&
_tmp2B};Cyc_fprintf(Cyc_stderr,({const char*_tmp29="%s: option `%s' needs an argument.\n";
_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,36));}),
_tag_dynforward(_tmp28,sizeof(void*),2));}}});goto _LL0;_LL5: if(*((int*)_tmp1C)!= 
3)goto _LL7;_tmp1F=((struct Cyc_Arg_Wrong_struct*)_tmp1C)->f1;_tmp20=((struct Cyc_Arg_Wrong_struct*)
_tmp1C)->f2;_tmp21=((struct Cyc_Arg_Wrong_struct*)_tmp1C)->f3;_LL6:({struct Cyc_String_pa_struct
_tmp31;_tmp31.tag=0;_tmp31.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp21);{struct Cyc_String_pa_struct _tmp30;_tmp30.tag=0;_tmp30.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp1F);{struct Cyc_String_pa_struct
_tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp20);{struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)progname);{void*_tmp2C[4]={& _tmp2E,&
_tmp2F,& _tmp30,& _tmp31};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D="%s: wrong argument `%s'; option `%s' expects %s.\n";
_tag_dynforward(_tmp2D,sizeof(char),_get_zero_arr_size(_tmp2D,50));}),
_tag_dynforward(_tmp2C,sizeof(void*),4));}}}}});goto _LL0;_LL7: if(*((int*)_tmp1C)
!= 2)goto _LL0;_tmp22=((struct Cyc_Arg_Message_struct*)_tmp1C)->f1;_LL8:({struct
Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp22);{struct Cyc_String_pa_struct _tmp34;_tmp34.tag=0;
_tmp34.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)progname);{void*_tmp32[
2]={& _tmp34,& _tmp35};Cyc_fprintf(Cyc_stderr,({const char*_tmp33="%s: %s.\n";
_tag_dynforward(_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,9));}),
_tag_dynforward(_tmp32,sizeof(void*),2));}}});goto _LL0;_LL0:;}Cyc_Arg_usage(
speclist,errmsg);Cyc_Arg_current=(int)_get_dynforward_size(Cyc_Arg_args,sizeof(
struct _dynforward_ptr));}void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*
anonfun)(struct _dynforward_ptr),struct _dynforward_ptr errmsg,struct
_dynforward_ptr orig_args){Cyc_Arg_args=orig_args;{int initpos=Cyc_Arg_current;
unsigned int l=_get_dynforward_size(Cyc_Arg_args,sizeof(struct _dynforward_ptr));
if((*((struct _dynforward_ptr*)_check_dynforward_subscript(Cyc_Arg_args,sizeof(
struct _dynforward_ptr),(int)(l - 1)))).curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)l=l - 1;++ Cyc_Arg_current;while(Cyc_Arg_current < l){
struct _dynforward_ptr s=*((struct _dynforward_ptr*)_check_dynforward_subscript(Cyc_Arg_args,
sizeof(struct _dynforward_ptr),Cyc_Arg_current));if((s.curr != ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr  && _get_dynforward_size(s,sizeof(
char))>= 1) && *((const char*)_check_dynforward_subscript(s,sizeof(char),0))== '-'){
void*action;{struct _handler_cons _tmp37;_push_handler(& _tmp37);{int _tmp39=0;if(
setjmp(_tmp37.handler))_tmp39=1;if(!_tmp39){action=Cyc_Arg_lookup(speclist,s);;
_pop_handler();}else{void*_tmp38=(void*)_exn_thrown;void*_tmp3B=_tmp38;_LLA: if(
_tmp3B != Cyc_Core_Not_found)goto _LLC;_LLB: Cyc_Arg_stop(initpos,(void*)({struct
Cyc_Arg_Unknown_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Arg_Unknown_struct
_tmp3D;_tmp3D.tag=0;_tmp3D.f1=s;_tmp3D;});_tmp3C;}),speclist,errmsg);return;_LLC:;
_LLD:(void)_throw(_tmp3B);_LL9:;}}}{struct _handler_cons _tmp3E;_push_handler(&
_tmp3E);{int _tmp40=0;if(setjmp(_tmp3E.handler))_tmp40=1;if(!_tmp40){{void*_tmp41=
action;void(*_tmp42)();void(*_tmp43)(struct _dynforward_ptr);int*_tmp44;int*
_tmp45;void(*_tmp46)(struct _dynforward_ptr,struct _dynforward_ptr);void(*_tmp47)(
struct _dynforward_ptr);void(*_tmp48)(int);void(*_tmp49)(struct _dynforward_ptr);
_LLF: if(*((int*)_tmp41)!= 0)goto _LL11;_tmp42=((struct Cyc_Arg_Unit_spec_struct*)
_tmp41)->f1;_LL10: _tmp42();goto _LLE;_LL11: if(*((int*)_tmp41)!= 1)goto _LL13;
_tmp43=((struct Cyc_Arg_Flag_spec_struct*)_tmp41)->f1;_LL12: _tmp43(s);goto _LLE;
_LL13: if(*((int*)_tmp41)!= 3)goto _LL15;_tmp44=((struct Cyc_Arg_Set_spec_struct*)
_tmp41)->f1;_LL14:*_tmp44=1;goto _LLE;_LL15: if(*((int*)_tmp41)!= 4)goto _LL17;
_tmp45=((struct Cyc_Arg_Clear_spec_struct*)_tmp41)->f1;_LL16:*_tmp45=0;goto _LLE;
_LL17: if(*((int*)_tmp41)!= 2)goto _LL19;_tmp46=((struct Cyc_Arg_FlagString_spec_struct*)
_tmp41)->f1;_LL18: if(Cyc_Arg_current + 1 < l){_tmp46(s,*((struct _dynforward_ptr*)
_check_dynforward_subscript(Cyc_Arg_args,sizeof(struct _dynforward_ptr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}else{Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Missing_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Arg_Missing_struct _tmp4B;
_tmp4B.tag=1;_tmp4B.f1=s;_tmp4B;});_tmp4A;}),speclist,errmsg);}goto _LLE;_LL19:
if(*((int*)_tmp41)!= 5)goto _LL1B;_tmp47=((struct Cyc_Arg_String_spec_struct*)
_tmp41)->f1;_LL1A: if(Cyc_Arg_current + 1 < l){_tmp47(*((struct _dynforward_ptr*)
_check_dynforward_subscript(Cyc_Arg_args,sizeof(struct _dynforward_ptr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}else{Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Missing_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Arg_Missing_struct _tmp4D;
_tmp4D.tag=1;_tmp4D.f1=s;_tmp4D;});_tmp4C;}),speclist,errmsg);}goto _LLE;_LL1B:
if(*((int*)_tmp41)!= 6)goto _LL1D;_tmp48=((struct Cyc_Arg_Int_spec_struct*)_tmp41)->f1;
_LL1C: {struct _dynforward_ptr arg=*((struct _dynforward_ptr*)
_check_dynforward_subscript(Cyc_Arg_args,sizeof(struct _dynforward_ptr),Cyc_Arg_current
+ 1));int n=0;if(({struct Cyc_IntPtr_sa_struct _tmp50;_tmp50.tag=2;_tmp50.f1=& n;{
void*_tmp4E[1]={& _tmp50};Cyc_sscanf(arg,({const char*_tmp4F="%d";_tag_dynforward(
_tmp4F,sizeof(char),_get_zero_arr_size(_tmp4F,3));}),_tag_dynforward(_tmp4E,
sizeof(void*),1));}})!= 1){Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Wrong_struct*
_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Arg_Wrong_struct _tmp52;
_tmp52.tag=3;_tmp52.f1=s;_tmp52.f2=arg;_tmp52.f3=({const char*_tmp53="an integer";
_tag_dynforward(_tmp53,sizeof(char),_get_zero_arr_size(_tmp53,11));});_tmp52;});
_tmp51;}),speclist,errmsg);_npop_handler(0);return;}_tmp48(n);++ Cyc_Arg_current;
goto _LLE;}_LL1D: if(*((int*)_tmp41)!= 7)goto _LLE;_tmp49=((struct Cyc_Arg_Rest_spec_struct*)
_tmp41)->f1;_LL1E: while(Cyc_Arg_current < l - 1){_tmp49(*((struct _dynforward_ptr*)
_check_dynforward_subscript(Cyc_Arg_args,sizeof(struct _dynforward_ptr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}goto _LLE;_LLE:;};_pop_handler();}else{void*_tmp3F=(
void*)_exn_thrown;void*_tmp55=_tmp3F;struct _dynforward_ptr _tmp56;_LL20: if(*((
void**)_tmp55)!= Cyc_Arg_Bad)goto _LL22;_tmp56=((struct Cyc_Arg_Bad_struct*)_tmp55)->f1;
_LL21: Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Message_struct*_tmp57=
_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_Arg_Message_struct _tmp58;_tmp58.tag=
2;_tmp58.f1=_tmp56;_tmp58;});_tmp57;}),speclist,errmsg);goto _LL1F;_LL22:;_LL23:(
void)_throw(_tmp55);_LL1F:;}}}++ Cyc_Arg_current;}else{{struct _handler_cons _tmp59;
_push_handler(& _tmp59);{int _tmp5B=0;if(setjmp(_tmp59.handler))_tmp5B=1;if(!
_tmp5B){anonfun(s);;_pop_handler();}else{void*_tmp5A=(void*)_exn_thrown;void*
_tmp5D=_tmp5A;struct _dynforward_ptr _tmp5E;_LL25: if(*((void**)_tmp5D)!= Cyc_Arg_Bad)
goto _LL27;_tmp5E=((struct Cyc_Arg_Bad_struct*)_tmp5D)->f1;_LL26: Cyc_Arg_stop(
initpos,(void*)({struct Cyc_Arg_Message_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));
_tmp5F[0]=({struct Cyc_Arg_Message_struct _tmp60;_tmp60.tag=2;_tmp60.f1=_tmp5E;
_tmp60;});_tmp5F;}),speclist,errmsg);goto _LL24;_LL27:;_LL28:(void)_throw(_tmp5D);
_LL24:;}}}++ Cyc_Arg_current;}}}}
