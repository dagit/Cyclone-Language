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
_dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);void*Cyc_List_hd(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_List_mem(int(*
compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_delete_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;extern
struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d,void*accum);void Cyc_Dict_iter(void(*f)(void*,void*),
struct Cyc_Dict_Dict*d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_map_c(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(void*(*
f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);int Cyc_Dict_forall_c(int(*
f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict*d);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct
_dynforward_ptr*s2);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;
struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;int Cyc_Position_segment_equals(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _dynforward_ptr Cyc_Position_string_of_segment(
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
_tuple1*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;
struct Cyc_List_List*targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1*tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);struct _dynforward_ptr*Cyc_Absyn_fieldname(int);struct
Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Absyn_is_union_type(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*aggrdecls;
struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_typecmp(void*,
void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[15]="\000\000\000\000HasTagCmps\000";
struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[
11]="\000\000\000\000IsZero\000";char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
13]="\000\000\000\000UnknownZ\000";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_mt_place_set();int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict**
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict*s1,struct Cyc_Dict_Dict*s2,int
disjoint);int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict*s1,struct Cyc_Dict_Dict*
s2);int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict*s1,struct Cyc_Dict_Dict*
s2);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict*consumed;struct Cyc_List_List*
may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_this;
extern void*Cyc_CfFlowInfo_unknown_all;extern void*Cyc_CfFlowInfo_esc_none;extern
void*Cyc_CfFlowInfo_esc_this;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*
f1,void*f2);void Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_initlevel(
void*il);void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_place_set(
struct Cyc_Dict_Dict*p);void Cyc_CfFlowInfo_print_place_list(struct Cyc_List_List*p);
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict*d);void Cyc_CfFlowInfo_print_flow(
void*f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict**all_changed,void*r);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Dict_Dict**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);void*
Cyc_CfFlowInfo_join_flow(struct Cyc_Dict_Dict**,void*,void*,int);struct _tuple0 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_Dict_Dict**all_changed,struct _tuple0 pr1,struct _tuple0 pr2,int);void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Dict_Dict**,void*,void*,struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict*);
void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};int Cyc_CfFlowInfo_contains_region(void*rgn,void*
t);void*Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,void*
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,void*f);
void*Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,void*
f);void*Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,void*f);
struct _tuple4{struct Cyc_CfFlowInfo_ConsumeInfo f1;void*f2;};struct _tuple4 Cyc_CfFlowInfo_save_consume_info(
void*f,int clear);void*Cyc_CfFlowInfo_restore_consume_info(void*f,struct Cyc_CfFlowInfo_ConsumeInfo
c);struct _dynforward_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={
0,(void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_all_v={
0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_none_v={
1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_this_v={
1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_all_v={
1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(void*)& Cyc_CfFlowInfo_unknown_none_v;
void*Cyc_CfFlowInfo_unknown_this=(void*)& Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(
void*)& Cyc_CfFlowInfo_unknown_all_v;void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;
void*Cyc_CfFlowInfo_esc_this=(void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(
void*)& Cyc_CfFlowInfo_esc_all_v;struct Cyc_Dict_Dict*Cyc_CfFlowInfo_mt_place_set(){
static struct Cyc_Dict_Dict**mt_place_set_opt=0;if(mt_place_set_opt == 0)
mt_place_set_opt=({struct Cyc_Dict_Dict**_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_empty)(Cyc_CfFlowInfo_place_cmp);_tmp6;});return*mt_place_set_opt;}int
Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict**set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(*set,place))return 1;else{*set=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k,
struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct
_dynforward_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp8;void*_tmp9;struct Cyc_CfFlowInfo_Place*_tmp7=
place;_tmp8=*_tmp7;_tmp9=(void*)_tmp8.root;{void*_tmpA=_tmp9;struct Cyc_Absyn_Vardecl*
_tmpB;_LL1: if(*((int*)_tmpA)!= 0)goto _LL3;_tmpB=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpA)->f1;_LL2: return Cyc_Absynpp_qvar2string(_tmpB->name);_LL3:;_LL4:({void*
_tmpC[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpD="error locations not for VarRoots";_tag_dynforward(_tmpD,sizeof(
char),_get_zero_arr_size(_tmpD,33));}),_tag_dynforward(_tmpC,sizeof(void*),0));});
_LL0:;}}static void Cyc_CfFlowInfo_unique_err(struct Cyc_CfFlowInfo_Place*place,
struct _dynforward_ptr err_msg1,struct _dynforward_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc){struct _dynforward_ptr _tmpE=Cyc_CfFlowInfo_place_err_string(
place);if(!Cyc_Position_segment_equals(consumed_loc,loc)){struct _dynforward_ptr
_tmpF=Cyc_Position_string_of_segment(consumed_loc);({struct Cyc_String_pa_struct
_tmp12;_tmp12.tag=0;_tmp12.f1=(struct _dynforward_ptr)_tmpF;{struct Cyc_String_pa_struct
_tmp11;_tmp11.tag=0;_tmp11.f1=(struct _dynforward_ptr)_tmpE;{void*_tmp10[2]={&
_tmp11,& _tmp12};Cyc_Tcutil_terr(loc,err_msg1,_tag_dynforward(_tmp10,sizeof(void*),
2));}}});}else{({struct Cyc_String_pa_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct
_dynforward_ptr)_tmpE;{void*_tmp13[1]={& _tmp14};Cyc_Tcutil_terr(loc,err_msg2,
_tag_dynforward(_tmp13,sizeof(void*),1));}});}}static struct Cyc_Position_Segment*
Cyc_CfFlowInfo_combine_consume_f(int isErr,struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2){if(isErr)Cyc_CfFlowInfo_unique_err(
place,({const char*_tmp15="May consume unique pointer %s more than once (cf. %s)";
_tag_dynforward(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,54));}),({const
char*_tmp16="May consume unique pointer %s more than once";_tag_dynforward(
_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,45));}),loc1,loc2);return loc1;}
struct Cyc_Dict_Dict*Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict*s1,struct
Cyc_Dict_Dict*s2,int disjoint){struct Cyc_Dict_Dict*_tmp17=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Position_Segment*(*f)(int,struct Cyc_CfFlowInfo_Place*,struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*),int env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_combine_consume_f,disjoint,s1,s2);
return _tmp17;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
struct _tuple5{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*f2;};int
Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict*s1,struct Cyc_Dict_Dict*s2){
if(s1 == s2)return 1;{struct _handler_cons _tmp18;_push_handler(& _tmp18);{int _tmp1A=
0;if(setjmp(_tmp18.handler))_tmp1A=1;if(!_tmp1A){{struct _RegionHandle _tmp1B=
_new_region("r");struct _RegionHandle*r=& _tmp1B;_push_region(r);{struct Cyc_Iter_Iter
_tmp1C=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))
Cyc_Dict_make_iter)(r,s1);struct _tuple5 _tmp1D=*((struct _tuple5*(*)(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(r,s1);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple5*))Cyc_Iter_next)(_tmp1C,& _tmp1D)){struct Cyc_CfFlowInfo_Place*
_tmp1E=_tmp1D.f1;if(!((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_member)(s2,_tmp1E)){int _tmp1F=0;_npop_handler(1);return _tmp1F;}}{int
_tmp20=1;_npop_handler(1);return _tmp20;}};_pop_region(r);};_pop_handler();}else{
void*_tmp19=(void*)_exn_thrown;void*_tmp22=_tmp19;_LL6:;_LL7: return 1;_LL8:;_LL9:(
void)_throw(_tmp22);_LL5:;}}}}int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict*
s1,struct Cyc_Dict_Dict*s2){return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{
struct _tuple0 _tmp24=({struct _tuple0 _tmp23;_tmp23.f1=r1;_tmp23.f2=r2;_tmp23;});
void*_tmp25;struct Cyc_Absyn_Vardecl*_tmp26;void*_tmp27;struct Cyc_Absyn_Vardecl*
_tmp28;void*_tmp29;void*_tmp2A;void*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;void*
_tmp2D;struct Cyc_Absyn_Exp*_tmp2E;void*_tmp2F;void*_tmp30;void*_tmp31;int _tmp32;
void*_tmp33;int _tmp34;_LLB: _tmp25=_tmp24.f1;if(*((int*)_tmp25)!= 0)goto _LLD;
_tmp26=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp25)->f1;_tmp27=_tmp24.f2;if(*((
int*)_tmp27)!= 0)goto _LLD;_tmp28=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp27)->f1;
_LLC: return(int)_tmp26 - (int)_tmp28;_LLD: _tmp29=_tmp24.f1;if(*((int*)_tmp29)!= 0)
goto _LLF;_LLE: return - 1;_LLF: _tmp2A=_tmp24.f2;if(*((int*)_tmp2A)!= 0)goto _LL11;
_LL10: return 1;_LL11: _tmp2B=_tmp24.f1;if(*((int*)_tmp2B)!= 1)goto _LL13;_tmp2C=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2B)->f1;_tmp2D=_tmp24.f2;if(*((int*)
_tmp2D)!= 1)goto _LL13;_tmp2E=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2D)->f1;
_LL12: return(int)_tmp2C - (int)_tmp2E;_LL13: _tmp2F=_tmp24.f1;if(*((int*)_tmp2F)!= 
1)goto _LL15;_LL14: return - 1;_LL15: _tmp30=_tmp24.f2;if(*((int*)_tmp30)!= 1)goto
_LL17;_LL16: return 1;_LL17: _tmp31=_tmp24.f1;if(*((int*)_tmp31)!= 2)goto _LLA;
_tmp32=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp31)->f1;_tmp33=_tmp24.f2;if(*((
int*)_tmp33)!= 2)goto _LLA;_tmp34=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp33)->f1;
_LL18: return _tmp32 - _tmp34;_LLA:;}}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{int i=Cyc_CfFlowInfo_root_cmp((
void*)p1->root,(void*)p2->root);if(i != 0)return i;return((int(*)(int(*cmp)(struct
_dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,p1->fields,p2->fields);}}static struct
_dynforward_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){struct
Cyc_List_List*sl=0;{void*_tmp35=(void*)p->root;struct Cyc_Absyn_Vardecl*_tmp36;
struct Cyc_Absyn_Exp*_tmp37;int _tmp38;_LL1A: if(*((int*)_tmp35)!= 0)goto _LL1C;
_tmp36=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp35)->f1;_LL1B: sl=({struct Cyc_List_List*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->hd=({struct _dynforward_ptr*_tmp3A=
_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_String_pa_struct _tmp3D;_tmp3D.tag=
0;_tmp3D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*_tmp36->name).f2);{
void*_tmp3B[1]={& _tmp3D};Cyc_aprintf(({const char*_tmp3C="%s";_tag_dynforward(
_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,3));}),_tag_dynforward(_tmp3B,
sizeof(void*),1));}});_tmp3A;});_tmp39->tl=sl;_tmp39;});goto _LL19;_LL1C: if(*((
int*)_tmp35)!= 1)goto _LL1E;_tmp37=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp35)->f1;
_LL1D: sl=({struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->hd=({
struct _dynforward_ptr*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Int_pa_struct
_tmp42;_tmp42.tag=1;_tmp42.f1=(unsigned long)((int)_tmp37);{void*_tmp40[1]={&
_tmp42};Cyc_aprintf(({const char*_tmp41="mpt%d";_tag_dynforward(_tmp41,sizeof(
char),_get_zero_arr_size(_tmp41,6));}),_tag_dynforward(_tmp40,sizeof(void*),1));}});
_tmp3F;});_tmp3E->tl=sl;_tmp3E;});goto _LL19;_LL1E: if(*((int*)_tmp35)!= 2)goto
_LL19;_tmp38=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp35)->f1;_LL1F: sl=({
struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->hd=({struct
_dynforward_ptr*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_Int_pa_struct
_tmp47;_tmp47.tag=1;_tmp47.f1=(unsigned long)_tmp38;{void*_tmp45[1]={& _tmp47};
Cyc_aprintf(({const char*_tmp46="param%d";_tag_dynforward(_tmp46,sizeof(char),
_get_zero_arr_size(_tmp46,8));}),_tag_dynforward(_tmp45,sizeof(void*),1));}});
_tmp44;});_tmp43->tl=sl;_tmp43;});goto _LL19;_LL19:;}{struct Cyc_List_List*fields=
p->fields;for(0;fields != 0;fields=fields->tl){sl=({struct Cyc_List_List*_tmp48=
_cycalloc(sizeof(*_tmp48));_tmp48->hd=({struct _dynforward_ptr*_tmp49=_cycalloc(
sizeof(*_tmp49));_tmp49[0]=({struct Cyc_String_pa_struct _tmp4C;_tmp4C.tag=0;
_tmp4C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)fields->hd));{void*_tmp4A[1]={& _tmp4C};Cyc_aprintf(({const char*
_tmp4B="%s";_tag_dynforward(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,3));}),
_tag_dynforward(_tmp4A,sizeof(void*),1));}});_tmp49;});_tmp48->tl=sl;_tmp48;});}}{
struct _dynforward_ptr*_tmp4D=({struct _dynforward_ptr*_tmp52=_cycalloc(sizeof(*
_tmp52));_tmp52[0]=({struct Cyc_String_pa_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(
struct _dynforward_ptr)({const char*_tmp56="";_tag_dynforward(_tmp56,sizeof(char),
_get_zero_arr_size(_tmp56,1));});{void*_tmp53[1]={& _tmp55};Cyc_aprintf(({const
char*_tmp54="%s";_tag_dynforward(_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,3));}),
_tag_dynforward(_tmp53,sizeof(void*),1));}});_tmp52;});for(0;sl != 0;sl=sl->tl){*
_tmp4D=({struct Cyc_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp4D);{struct Cyc_String_pa_struct
_tmp50;_tmp50.tag=0;_tmp50.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)sl->hd));{void*_tmp4E[2]={& _tmp50,& _tmp51};Cyc_aprintf(({
const char*_tmp4F="%s.%s";_tag_dynforward(_tmp4F,sizeof(char),_get_zero_arr_size(
_tmp4F,6));}),_tag_dynforward(_tmp4E,sizeof(void*),2));}}});}return _tmp4D;}}
struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
int allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_union_type(t))return Cyc_CfFlowInfo_unknown_all;{
void*_tmp57=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo _tmp58;void*
_tmp59;struct Cyc_Absyn_Tunionfield*_tmp5A;struct Cyc_List_List*_tmp5B;struct Cyc_Absyn_AggrInfo
_tmp5C;void*_tmp5D;void*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_Absyn_ArrayInfo
_tmp60;void*_tmp61;struct Cyc_Absyn_Conref*_tmp62;void*_tmp63;struct Cyc_Absyn_PtrInfo
_tmp64;struct Cyc_Absyn_PtrAtts _tmp65;struct Cyc_Absyn_Conref*_tmp66;_LL21: if(
_tmp57 <= (void*)4)goto _LL2F;if(*((int*)_tmp57)!= 3)goto _LL23;_tmp58=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp57)->f1;_tmp59=(void*)_tmp58.field_info;if(*((int*)_tmp59)!= 1)goto _LL23;
_tmp5A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp59)->f2;_LL22: if(_tmp5A->typs
== 0)return leafval;_tmp5B=_tmp5A->typs;goto _LL24;_LL23: if(*((int*)_tmp57)!= 9)
goto _LL25;_tmp5B=((struct Cyc_Absyn_TupleType_struct*)_tmp57)->f1;_LL24: {struct
Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;_tmp5B != 0;(
_tmp5B=_tmp5B->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_i_typ_to_absrval(
0,(*((struct _tuple6*)_tmp5B->hd)).f2,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp68;_tmp68.tag=4;_tmp68.f1=d;_tmp68;});_tmp67;});}_LL25: if(*((int*)_tmp57)!= 
10)goto _LL27;_tmp5C=((struct Cyc_Absyn_AggrType_struct*)_tmp57)->f1;_tmp5D=(void*)
_tmp5C.aggr_info;_LL26: {struct Cyc_Absyn_Aggrdecl*_tmp69=Cyc_Absyn_get_known_aggrdecl(
_tmp5D);if(_tmp69->impl == 0)goto _LL20;_tmp5F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp69->impl))->fields;goto _LL28;}_LL27: if(*((int*)_tmp57)!= 11)goto
_LL29;_tmp5E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp57)->f1;if((int)
_tmp5E != 0)goto _LL29;_tmp5F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp57)->f2;
_LL28: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);for(0;
_tmp5F != 0;_tmp5F=_tmp5F->tl){struct Cyc_Absyn_Aggrfield _tmp6B;struct
_dynforward_ptr*_tmp6C;void*_tmp6D;struct Cyc_Absyn_Aggrfield*_tmp6A=(struct Cyc_Absyn_Aggrfield*)
_tmp5F->hd;_tmp6B=*_tmp6A;_tmp6C=_tmp6B.name;_tmp6D=(void*)_tmp6B.type;if(
_get_dynforward_size(*_tmp6C,sizeof(char))!= 1)d=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(d,_tmp6C,
Cyc_CfFlowInfo_i_typ_to_absrval(0,_tmp6D,leafval));}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp6F;_tmp6F.tag=4;_tmp6F.f1=d;_tmp6F;});_tmp6E;});}_LL29: if(*((int*)_tmp57)!= 
7)goto _LL2B;_tmp60=((struct Cyc_Absyn_ArrayType_struct*)_tmp57)->f1;_tmp61=(void*)
_tmp60.elt_type;_tmp62=_tmp60.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp62))goto _LL2B;_LL2A: return allow_zeroterm  && Cyc_Tcutil_bits_only(
_tmp61)?Cyc_CfFlowInfo_unknown_all: leafval;_LL2B: if(*((int*)_tmp57)!= 17)goto
_LL2D;_tmp63=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp57)->f1;_LL2C: {void*
_tmp70=leafval;void*_tmp71;void*_tmp72;_LL32: if(_tmp70 <= (void*)3)goto _LL36;if(*((
int*)_tmp70)!= 0)goto _LL34;_tmp71=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp70)->f1;if((int)_tmp71 != 2)goto _LL34;_LL33: goto _LL35;_LL34: if(*((int*)_tmp70)
!= 1)goto _LL36;_tmp72=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp70)->f1;if((
int)_tmp72 != 2)goto _LL36;_LL35: goto _LL37;_LL36: if((int)_tmp70 != 0)goto _LL38;
_LL37: goto _LL39;_LL38: if((int)_tmp70 != 1)goto _LL3A;_LL39: return(void*)({struct
Cyc_CfFlowInfo_TagCmps_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73[0]=({
struct Cyc_CfFlowInfo_TagCmps_struct _tmp74;_tmp74.tag=3;_tmp74.f1=({struct Cyc_CfFlowInfo_TagCmp*
_tmp75[1];_tmp75[0]=({struct Cyc_CfFlowInfo_TagCmp*_tmp76=_cycalloc(sizeof(*
_tmp76));_tmp76->cmp=(void*)((void*)5);_tmp76->bd=(void*)_tmp63;_tmp76;});((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp75,sizeof(struct Cyc_CfFlowInfo_TagCmp*),1));});_tmp74;});_tmp73;});_LL3A:;
_LL3B: return leafval;_LL31:;}_LL2D: if(*((int*)_tmp57)!= 4)goto _LL2F;_tmp64=((
struct Cyc_Absyn_PointerType_struct*)_tmp57)->f1;_tmp65=_tmp64.ptr_atts;_tmp66=
_tmp65.nullable;if(!(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp66)))goto _LL2F;_LL2E:{void*_tmp77=leafval;void*_tmp78;void*_tmp79;_LL3D:
if(_tmp77 <= (void*)3)goto _LL41;if(*((int*)_tmp77)!= 0)goto _LL3F;_tmp78=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp77)->f1;if((int)_tmp78 != 1)goto _LL3F;
_LL3E: return(void*)2;_LL3F: if(*((int*)_tmp77)!= 0)goto _LL41;_tmp79=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp77)->f1;if((int)_tmp79 != 2)goto _LL41;
_LL40: return(void*)1;_LL41:;_LL42: goto _LL3C;_LL3C:;}goto _LL20;_LL2F:;_LL30: goto
_LL20;_LL20:;}return Cyc_Tcutil_bits_only(t)?Cyc_CfFlowInfo_unknown_all: leafval;}
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
1,t,leafval);}static int Cyc_CfFlowInfo_prefix_of_member(struct _RegionHandle*r,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict*set){struct _tuple5 _tmp7A=({
struct _tuple5 _tmp7F;_tmp7F.f1=place;_tmp7F.f2=0;_tmp7F;});struct Cyc_Iter_Iter
_tmp7B=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))
Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple5*))
Cyc_Iter_next)(_tmp7B,& _tmp7A)){struct Cyc_CfFlowInfo_Place*_tmp7C=_tmp7A.f1;if(
Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp7C->root)!= 0)continue;{
struct Cyc_List_List*_tmp7D=place->fields;struct Cyc_List_List*_tmp7E=_tmp7C->fields;
for(0;_tmp7D != 0  && _tmp7E != 0;(_tmp7D=_tmp7D->tl,_tmp7E=_tmp7E->tl)){if(Cyc_strptrcmp((
struct _dynforward_ptr*)_tmp7D->hd,(struct _dynforward_ptr*)_tmp7E->hd)!= 0)break;}
if(_tmp7D == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{struct
_RegionHandle*rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,
place))pile->places=({struct Cyc_List_List*_tmp80=_region_malloc(pile->rgn,
sizeof(*_tmp80));_tmp80->hd=place;_tmp80->tl=pile->places;_tmp80;});}static void
Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*a,void*r){void*
_tmp81=r;struct Cyc_CfFlowInfo_Place*_tmp82;struct Cyc_Dict_Dict*_tmp83;_LL44: if(
_tmp81 <= (void*)3)goto _LL48;if(*((int*)_tmp81)!= 2)goto _LL46;_tmp82=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp81)->f1;_LL45: Cyc_CfFlowInfo_add_place(pile,_tmp82);return;_LL46: if(*((int*)
_tmp81)!= 4)goto _LL48;_tmp83=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp81)->f1;
_LL47:((void(*)(void(*f)(struct Cyc_CfFlowInfo_EscPile*,struct _dynforward_ptr*,
void*),struct Cyc_CfFlowInfo_EscPile*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,struct _dynforward_ptr*a,void*r))Cyc_CfFlowInfo_add_places,
pile,_tmp83);return;_LL48:;_LL49: return;_LL43:;}static void*Cyc_CfFlowInfo_insert_place_inner(
void*new_val,void*old_val){void*_tmp84=old_val;struct Cyc_Dict_Dict*_tmp85;_LL4B:
if(_tmp84 <= (void*)3)goto _LL4D;if(*((int*)_tmp84)!= 4)goto _LL4D;_tmp85=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmp84)->f1;_LL4C: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp87;_tmp87.tag=4;_tmp87.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp85);_tmp87;});_tmp86;});_LL4D:;_LL4E: return new_val;_LL4A:;}struct
_tuple7{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple7 _tmp89=({struct _tuple7 _tmp88;_tmp88.f1=fs;_tmp88.f2=
old_val;_tmp88;});struct Cyc_List_List*_tmp8A;struct Cyc_List_List _tmp8B;struct
_dynforward_ptr*_tmp8C;struct Cyc_List_List*_tmp8D;void*_tmp8E;struct Cyc_Dict_Dict*
_tmp8F;_LL50: _tmp8A=_tmp89.f1;if(_tmp8A == 0)goto _LL52;_tmp8B=*_tmp8A;_tmp8C=(
struct _dynforward_ptr*)_tmp8B.hd;_tmp8D=_tmp8B.tl;_tmp8E=_tmp89.f2;if(_tmp8E <= (
void*)3)goto _LL52;if(*((int*)_tmp8E)!= 4)goto _LL52;_tmp8F=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp8E)->f1;_LL51: {void*_tmp90=Cyc_CfFlowInfo_insert_place_outer(_tmp8D,((void*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp8F,_tmp8C),
new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp91=_cycalloc(
sizeof(*_tmp91));_tmp91[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp92;_tmp92.tag=
4;_tmp92.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k,void*v))Cyc_Dict_insert)(_tmp8F,_tmp8C,_tmp90);_tmp92;});
_tmp91;});}_LL52:;_LL53:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Core_Impossible_struct
_tmp94;_tmp94.tag=Cyc_Core_Impossible;_tmp94.f1=({const char*_tmp95="bad insert place";
_tag_dynforward(_tmp95,sizeof(char),_get_zero_arr_size(_tmp95,17));});_tmp94;});
_tmp93;}));_LL4F:;}}static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_these(struct
Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict**all_changed,struct Cyc_Dict_Dict*
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp96=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;Cyc_CfFlowInfo_update_place_set(all_changed,
_tmp96,0);{void*oldval;void*newval;{struct _handler_cons _tmp97;_push_handler(&
_tmp97);{int _tmp99=0;if(setjmp(_tmp97.handler))_tmp99=1;if(!_tmp99){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp96);;_pop_handler();}else{void*_tmp98=(void*)_exn_thrown;void*_tmp9B=
_tmp98;_LL55: if(_tmp9B != Cyc_Dict_Absent)goto _LL57;_LL56: continue;_LL57:;_LL58:(
void)_throw(_tmp9B);_LL54:;}}}{void*_tmp9C=Cyc_CfFlowInfo_initlevel(d,oldval);
_LL5A: if((int)_tmp9C != 2)goto _LL5C;_LL5B: newval=Cyc_CfFlowInfo_esc_all;goto _LL59;
_LL5C: if((int)_tmp9C != 1)goto _LL5E;_LL5D: newval=Cyc_CfFlowInfo_esc_this;goto
_LL59;_LL5E: if((int)_tmp9C != 0)goto _LL59;_LL5F: newval=Cyc_CfFlowInfo_esc_none;
goto _LL59;_LL59:;}((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,oldval);d=Cyc_Dict_insert(d,(void*)_tmp96->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp96->fields,Cyc_Dict_lookup(d,(void*)_tmp96->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp9D=r;void*_tmp9E;
void*_tmp9F;_LL61: if(_tmp9D <= (void*)3)goto _LL65;if(*((int*)_tmp9D)!= 0)goto
_LL63;_tmp9E=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp9D)->f1;_LL62:
return _tmp9E;_LL63: if(*((int*)_tmp9D)!= 1)goto _LL65;_tmp9F=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp9D)->f1;_LL64: return _tmp9F;_LL65: if((int)_tmp9D != 0)goto _LL67;_LL66: goto
_LL68;_LL67: if((int)_tmp9D != 1)goto _LL69;_LL68: return(void*)2;_LL69: if((int)
_tmp9D != 2)goto _LL6B;_LL6A: return(void*)1;_LL6B: if(_tmp9D <= (void*)3)goto _LL6D;
if(*((int*)_tmp9D)!= 3)goto _LL6D;_LL6C: return(void*)2;_LL6D:;_LL6E:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));
_tmpA0[0]=({struct Cyc_Core_Impossible_struct _tmpA1;_tmpA1.tag=Cyc_Core_Impossible;
_tmpA1.f1=({const char*_tmpA2="initlevel_approx";_tag_dynforward(_tmpA2,sizeof(
char),_get_zero_arr_size(_tmpA2,17));});_tmpA1;});_tmpA0;}));_LL60:;}static void*
Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*a,void*r,
void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*_tmpA3=r;struct
Cyc_Dict_Dict*_tmpA4;struct Cyc_CfFlowInfo_Place*_tmpA5;_LL70: if(_tmpA3 <= (void*)
3)goto _LL74;if(*((int*)_tmpA3)!= 4)goto _LL72;_tmpA4=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpA3)->f1;_LL71: this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,
struct _dynforward_ptr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct
Cyc_Dict_Dict*d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,struct _dynforward_ptr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,
_tmpA4,(void*)2);goto _LL6F;_LL72: if(*((int*)_tmpA3)!= 2)goto _LL74;_tmpA5=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpA3)->f1;_LL73: if(((int(*)(int(*
compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmpA5))this_ans=(void*)2;else{env->seen=({struct Cyc_List_List*_tmpA6=_cycalloc(
sizeof(*_tmpA6));_tmpA6->hd=_tmpA5;_tmpA6->tl=env->seen;_tmpA6;});this_ans=((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(
env,0,Cyc_CfFlowInfo_lookup_place(env->d,_tmpA5),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL6F;
_LL74:;_LL75: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL6F:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1  || acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmpA7=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmpA8;_tmpA8.d=d;_tmpA8.seen=0;_tmpA8;});return((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(& _tmpA7,0,r,(void*)2);}
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){struct Cyc_CfFlowInfo_Place _tmpAA;void*_tmpAB;struct Cyc_List_List*_tmpAC;
struct Cyc_CfFlowInfo_Place*_tmpA9=place;_tmpAA=*_tmpA9;_tmpAB=(void*)_tmpAA.root;
_tmpAC=_tmpAA.fields;{void*_tmpAD=Cyc_Dict_lookup(d,_tmpAB);for(0;_tmpAC != 0;
_tmpAC=_tmpAC->tl){struct _tuple1 _tmpAF=({struct _tuple1 _tmpAE;_tmpAE.f1=_tmpAD;
_tmpAE.f2=(struct _dynforward_ptr*)_tmpAC->hd;_tmpAE;});void*_tmpB0;struct Cyc_Dict_Dict*
_tmpB1;struct _dynforward_ptr*_tmpB2;_LL77: _tmpB0=_tmpAF.f1;if(_tmpB0 <= (void*)3)
goto _LL79;if(*((int*)_tmpB0)!= 4)goto _LL79;_tmpB1=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpB0)->f1;_tmpB2=_tmpAF.f2;_LL78: _tmpAD=((void*(*)(struct Cyc_Dict_Dict*d,
struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmpB1,_tmpB2);goto _LL76;_LL79:;_LL7A:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpB3=_cycalloc(sizeof(*
_tmpB3));_tmpB3[0]=({struct Cyc_Core_Impossible_struct _tmpB4;_tmpB4.tag=Cyc_Core_Impossible;
_tmpB4.f1=({const char*_tmpB5="bad lookup_place";_tag_dynforward(_tmpB5,sizeof(
char),_get_zero_arr_size(_tmpB5,17));});_tmpB4;});_tmpB3;}));_LL76:;}return
_tmpAD;}}static int Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,void*rval){void*
_tmpB6=rval;struct Cyc_Dict_Dict*_tmpB7;_LL7C: if(_tmpB6 <= (void*)3)goto _LL80;if(*((
int*)_tmpB6)!= 1)goto _LL7E;_LL7D: return 0;_LL7E: if(*((int*)_tmpB6)!= 4)goto _LL80;
_tmpB7=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpB6)->f1;_LL7F: return((int(*)(
int(*f)(int,struct _dynforward_ptr*,void*),int env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((
int(*)(int a,struct _dynforward_ptr*b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped,
0,_tmpB7);_LL80:;_LL81: return 1;_LL7B:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(
0,0,Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict**all_changed,void*r){struct
_RegionHandle _tmpB8=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB8;
_push_region(rgn);{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*
_tmpBA=_region_malloc(rgn,sizeof(*_tmpBA));_tmpBA->rgn=rgn;_tmpBA->places=0;
_tmpBA;});((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmpB9=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmpB9;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*a,void*oldval,void*newval){struct _tuple0 _tmpBC=({struct _tuple0 _tmpBB;
_tmpBB.f1=oldval;_tmpBB.f2=newval;_tmpBB;});void*_tmpBD;void*_tmpBE;struct Cyc_CfFlowInfo_Place*
_tmpBF;void*_tmpC0;void*_tmpC1;struct Cyc_Dict_Dict*_tmpC2;void*_tmpC3;struct Cyc_Dict_Dict*
_tmpC4;void*_tmpC5;void*_tmpC6;_LL83: _tmpBD=_tmpBC.f1;if(_tmpBD <= (void*)3)goto
_LL85;if(*((int*)_tmpBD)!= 1)goto _LL85;_tmpBE=_tmpBC.f2;if(_tmpBE <= (void*)3)
goto _LL85;if(*((int*)_tmpBE)!= 2)goto _LL85;_tmpBF=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpBE)->f1;_LL84: Cyc_CfFlowInfo_add_place(env->pile,_tmpBF);goto _LL86;_LL85:
_tmpC0=_tmpBC.f1;if(_tmpC0 <= (void*)3)goto _LL87;if(*((int*)_tmpC0)!= 1)goto _LL87;
_LL86: if(Cyc_CfFlowInfo_initlevel(env->d,newval)!= (void*)2)({void*_tmpC7[0]={};
Cyc_Tcutil_terr(env->loc,({const char*_tmpC8="assignment puts possibly-uninitialized data in an escaped location";
_tag_dynforward(_tmpC8,sizeof(char),_get_zero_arr_size(_tmpC8,67));}),
_tag_dynforward(_tmpC7,sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;_LL87:
_tmpC1=_tmpBC.f1;if(_tmpC1 <= (void*)3)goto _LL89;if(*((int*)_tmpC1)!= 4)goto _LL89;
_tmpC2=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpC1)->f1;_tmpC3=_tmpBC.f2;if(
_tmpC3 <= (void*)3)goto _LL89;if(*((int*)_tmpC3)!= 4)goto _LL89;_tmpC4=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC3)->f1;_LL88: {struct Cyc_Dict_Dict*_tmpC9=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_AssignEnv*,struct _dynforward_ptr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,struct _dynforward_ptr*a,void*oldval,void*
newval))Cyc_CfFlowInfo_assign_place_inner,env,_tmpC2,_tmpC4);if(_tmpC9 == _tmpC2)
return oldval;if(_tmpC9 == _tmpC4)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpCB;_tmpCB.tag=4;_tmpCB.f1=_tmpC9;_tmpCB;});_tmpCA;});}_LL89: _tmpC5=_tmpBC.f2;
if(_tmpC5 <= (void*)3)goto _LL8B;if(*((int*)_tmpC5)!= 1)goto _LL8B;_tmpC6=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmpC5)->f1;_LL8A: {void*_tmpCC=_tmpC6;_LL8E:
if((int)_tmpCC != 0)goto _LL90;_LL8F: return Cyc_CfFlowInfo_unknown_none;_LL90: if((
int)_tmpCC != 1)goto _LL92;_LL91: return Cyc_CfFlowInfo_unknown_this;_LL92: if((int)
_tmpCC != 2)goto _LL8D;_LL93: return Cyc_CfFlowInfo_unknown_all;_LL8D:;}_LL8B:;_LL8C:
return newval;_LL82:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);{struct _tuple7 _tmpCE=({struct _tuple7 _tmpCD;_tmpCD.f1=fs;
_tmpCD.f2=oldval;_tmpCD;});struct Cyc_List_List*_tmpCF;struct Cyc_List_List _tmpD0;
struct _dynforward_ptr*_tmpD1;struct Cyc_List_List*_tmpD2;void*_tmpD3;struct Cyc_Dict_Dict*
_tmpD4;_LL95: _tmpCF=_tmpCE.f1;if(_tmpCF == 0)goto _LL97;_tmpD0=*_tmpCF;_tmpD1=(
struct _dynforward_ptr*)_tmpD0.hd;_tmpD2=_tmpD0.tl;_tmpD3=_tmpCE.f2;if(_tmpD3 <= (
void*)3)goto _LL97;if(*((int*)_tmpD3)!= 4)goto _LL97;_tmpD4=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD3)->f1;_LL96: {void*_tmpD5=Cyc_CfFlowInfo_assign_place_outer(env,_tmpD2,((
void*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmpD4,
_tmpD1),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpD6=
_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpD7;_tmpD7.tag=4;_tmpD7.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(_tmpD4,_tmpD1,_tmpD5);_tmpD7;});
_tmpD6;});}_LL97:;_LL98:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Core_Impossible_struct
_tmpD9;_tmpD9.tag=Cyc_Core_Impossible;_tmpD9.f1=({const char*_tmpDA="bad assign place";
_tag_dynforward(_tmpDA,sizeof(char),_get_zero_arr_size(_tmpDA,17));});_tmpD9;});
_tmpD8;}));_LL94:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r){Cyc_CfFlowInfo_update_place_set(all_changed,place,0);{struct
_RegionHandle _tmpDB=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDB;
_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmpDD;void*_tmpDE;struct Cyc_List_List*
_tmpDF;struct Cyc_CfFlowInfo_Place*_tmpDC=place;_tmpDD=*_tmpDC;_tmpDE=(void*)
_tmpDD.root;_tmpDF=_tmpDD.fields;{struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv
_tmpE1;_tmpE1.pile=({struct Cyc_CfFlowInfo_EscPile*_tmpE2=_region_malloc(rgn,
sizeof(*_tmpE2));_tmpE2->rgn=rgn;_tmpE2->places=0;_tmpE2;});_tmpE1.d=d;_tmpE1.loc=
loc;_tmpE1;});void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmpDF,Cyc_Dict_lookup(
d,_tmpDE),r);struct Cyc_Dict_Dict*_tmpE0=Cyc_CfFlowInfo_escape_these(env.pile,
all_changed,Cyc_Dict_insert(d,_tmpDE,newval));_npop_handler(0);return _tmpE0;}};
_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict*
chg1;struct Cyc_Dict_Dict*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*
_tmpE3=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmpE5;void*_tmpE6;
void*_tmpE7;struct Cyc_CfFlowInfo_TagCmp*_tmpE4=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;
_tmpE5=*_tmpE4;_tmpE6=(void*)_tmpE5.cmp;_tmpE7=(void*)_tmpE5.bd;{int found=0;void*
joined_cmp=(void*)10;{struct Cyc_List_List*_tmpE8=l1;for(0;_tmpE8 != 0;_tmpE8=
_tmpE8->tl){struct Cyc_CfFlowInfo_TagCmp _tmpEA;void*_tmpEB;void*_tmpEC;struct Cyc_CfFlowInfo_TagCmp*
_tmpE9=(struct Cyc_CfFlowInfo_TagCmp*)_tmpE8->hd;_tmpEA=*_tmpE9;_tmpEB=(void*)
_tmpEA.cmp;_tmpEC=(void*)_tmpEA.bd;if(Cyc_Tcutil_typecmp(_tmpE7,_tmpEC)== 0){
found=1;if(_tmpEB == _tmpE6){joined_cmp=_tmpEB;break;}}}}if(found)_tmpE3=({struct
Cyc_List_List*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->hd=({struct Cyc_CfFlowInfo_TagCmp*
_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->cmp=(void*)joined_cmp;_tmpEE->bd=(void*)
_tmpE7;_tmpEE;});_tmpED->tl=_tmpE3;_tmpED;});}}return _tmpE3;}}static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{
struct _tuple0 _tmpF0=({struct _tuple0 _tmpEF;_tmpEF.f1=r1;_tmpEF.f2=r2;_tmpEF;});
void*_tmpF1;struct Cyc_CfFlowInfo_Place*_tmpF2;void*_tmpF3;struct Cyc_CfFlowInfo_Place*
_tmpF4;void*_tmpF5;struct Cyc_CfFlowInfo_Place*_tmpF6;void*_tmpF7;struct Cyc_CfFlowInfo_Place*
_tmpF8;void*_tmpF9;void*_tmpFA;void*_tmpFB;void*_tmpFC;void*_tmpFD;struct Cyc_Dict_Dict*
_tmpFE;void*_tmpFF;struct Cyc_Dict_Dict*_tmp100;void*_tmp101;struct Cyc_List_List*
_tmp102;void*_tmp103;struct Cyc_List_List*_tmp104;void*_tmp105;void*_tmp106;_LL9A:
_tmpF1=_tmpF0.f1;if(_tmpF1 <= (void*)3)goto _LL9C;if(*((int*)_tmpF1)!= 2)goto _LL9C;
_tmpF2=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpF1)->f1;_tmpF3=_tmpF0.f2;if(
_tmpF3 <= (void*)3)goto _LL9C;if(*((int*)_tmpF3)!= 2)goto _LL9C;_tmpF4=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpF3)->f1;_LL9B: if(Cyc_CfFlowInfo_place_cmp(_tmpF2,_tmpF4)== 0)return r1;Cyc_CfFlowInfo_add_place(
env->pile,_tmpF2);Cyc_CfFlowInfo_add_place(env->pile,_tmpF4);goto _LL99;_LL9C:
_tmpF5=_tmpF0.f1;if(_tmpF5 <= (void*)3)goto _LL9E;if(*((int*)_tmpF5)!= 2)goto _LL9E;
_tmpF6=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpF5)->f1;_LL9D: Cyc_CfFlowInfo_add_place(
env->pile,_tmpF6);goto _LL99;_LL9E: _tmpF7=_tmpF0.f2;if(_tmpF7 <= (void*)3)goto
_LLA0;if(*((int*)_tmpF7)!= 2)goto _LLA0;_tmpF8=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpF7)->f1;_LL9F: Cyc_CfFlowInfo_add_place(env->pile,_tmpF8);goto _LL99;_LLA0:
_tmpF9=_tmpF0.f1;if((int)_tmpF9 != 1)goto _LLA2;_tmpFA=_tmpF0.f2;if((int)_tmpFA != 
2)goto _LLA2;_LLA1: goto _LLA3;_LLA2: _tmpFB=_tmpF0.f1;if((int)_tmpFB != 2)goto _LLA4;
_tmpFC=_tmpF0.f2;if((int)_tmpFC != 1)goto _LLA4;_LLA3: return(void*)2;_LLA4: _tmpFD=
_tmpF0.f1;if(_tmpFD <= (void*)3)goto _LLA6;if(*((int*)_tmpFD)!= 4)goto _LLA6;_tmpFE=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpFD)->f1;_tmpFF=_tmpF0.f2;if(_tmpFF <= (
void*)3)goto _LLA6;if(*((int*)_tmpFF)!= 4)goto _LLA6;_tmp100=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpFF)->f1;_LLA5: {struct Cyc_Dict_Dict*_tmp107=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_JoinEnv*,struct _dynforward_ptr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,struct _dynforward_ptr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,
env,_tmpFE,_tmp100);if(_tmp107 == _tmpFE)return r1;if(_tmp107 == _tmp100)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp108=_cycalloc(sizeof(*
_tmp108));_tmp108[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp109;_tmp109.tag=
4;_tmp109.f1=_tmp107;_tmp109;});_tmp108;});}_LLA6: _tmp101=_tmpF0.f1;if(_tmp101 <= (
void*)3)goto _LLA8;if(*((int*)_tmp101)!= 3)goto _LLA8;_tmp102=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp101)->f1;_tmp103=_tmpF0.f2;if(_tmp103 <= (void*)3)goto _LLA8;if(*((int*)
_tmp103)!= 3)goto _LLA8;_tmp104=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp103)->f1;
_LLA7: {struct Cyc_List_List*_tmp10A=Cyc_CfFlowInfo_join_tag_cmps(_tmp102,_tmp104);
if(_tmp10A == _tmp102)return r1;return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp10C;_tmp10C.tag=3;_tmp10C.f1=_tmp10A;_tmp10C;});_tmp10B;});}_LLA8: _tmp105=
_tmpF0.f1;if(_tmp105 <= (void*)3)goto _LLAA;if(*((int*)_tmp105)!= 3)goto _LLAA;
_LLA9: return r2;_LLAA: _tmp106=_tmpF0.f2;if(_tmp106 <= (void*)3)goto _LLAC;if(*((int*)
_tmp106)!= 3)goto _LLAC;_LLAB: return r1;_LLAC:;_LLAD: goto _LL99;_LL99:;}{void*il1=
Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,
r2);struct _tuple0 _tmp10E=({struct _tuple0 _tmp10D;_tmp10D.f1=r1;_tmp10D.f2=r2;
_tmp10D;});void*_tmp10F;void*_tmp110;_LLAF: _tmp10F=_tmp10E.f1;if(_tmp10F <= (void*)
3)goto _LLB1;if(*((int*)_tmp10F)!= 1)goto _LLB1;_LLB0: goto _LLB2;_LLB1: _tmp110=
_tmp10E.f2;if(_tmp110 <= (void*)3)goto _LLB3;if(*((int*)_tmp110)!= 1)goto _LLB3;
_LLB2: {struct _tuple0 _tmp112=({struct _tuple0 _tmp111;_tmp111.f1=il1;_tmp111.f2=
il2;_tmp111;});void*_tmp113;void*_tmp114;void*_tmp115;void*_tmp116;_LLB6: _tmp113=
_tmp112.f2;if((int)_tmp113 != 0)goto _LLB8;_LLB7: goto _LLB9;_LLB8: _tmp114=_tmp112.f1;
if((int)_tmp114 != 0)goto _LLBA;_LLB9: return Cyc_CfFlowInfo_esc_none;_LLBA: _tmp115=
_tmp112.f2;if((int)_tmp115 != 1)goto _LLBC;_LLBB: goto _LLBD;_LLBC: _tmp116=_tmp112.f1;
if((int)_tmp116 != 1)goto _LLBE;_LLBD: return Cyc_CfFlowInfo_esc_this;_LLBE:;_LLBF:
return Cyc_CfFlowInfo_esc_all;_LLB5:;}_LLB3:;_LLB4: {struct _tuple0 _tmp118=({
struct _tuple0 _tmp117;_tmp117.f1=il1;_tmp117.f2=il2;_tmp117;});void*_tmp119;void*
_tmp11A;void*_tmp11B;void*_tmp11C;_LLC1: _tmp119=_tmp118.f2;if((int)_tmp119 != 0)
goto _LLC3;_LLC2: goto _LLC4;_LLC3: _tmp11A=_tmp118.f1;if((int)_tmp11A != 0)goto _LLC5;
_LLC4: return Cyc_CfFlowInfo_unknown_none;_LLC5: _tmp11B=_tmp118.f2;if((int)_tmp11B
!= 1)goto _LLC7;_LLC6: goto _LLC8;_LLC7: _tmp11C=_tmp118.f1;if((int)_tmp11C != 1)goto
_LLC9;_LLC8: return Cyc_CfFlowInfo_unknown_this;_LLC9:;_LLCA: return Cyc_CfFlowInfo_unknown_all;
_LLC0:;}_LLAE:;}}static int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2)
return 1;{struct _tuple0 _tmp11E=({struct _tuple0 _tmp11D;_tmp11D.f1=r1;_tmp11D.f2=r2;
_tmp11D;});void*_tmp11F;unsigned int _tmp120;void*_tmp121;unsigned int _tmp122;
void*_tmp123;struct Cyc_Absyn_Vardecl*_tmp124;void*_tmp125;struct Cyc_Absyn_Vardecl*
_tmp126;void*_tmp127;struct Cyc_Absyn_Vardecl*_tmp128;void*_tmp129;struct Cyc_Absyn_Vardecl*
_tmp12A;void*_tmp12B;unsigned int _tmp12C;void*_tmp12D;unsigned int _tmp12E;void*
_tmp12F;struct Cyc_Absyn_Vardecl*_tmp130;void*_tmp131;struct Cyc_Absyn_Vardecl*
_tmp132;_LLCC: _tmp11F=_tmp11E.f1;if(*((int*)_tmp11F)!= 0)goto _LLCE;_tmp120=((
struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp11F)->f1;_tmp121=_tmp11E.f2;if(*((
int*)_tmp121)!= 0)goto _LLCE;_tmp122=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp121)->f1;_LLCD: return _tmp120 == _tmp122;_LLCE: _tmp123=_tmp11E.f1;if(*((int*)
_tmp123)!= 1)goto _LLD0;_tmp124=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp123)->f1;
_tmp125=_tmp11E.f2;if(*((int*)_tmp125)!= 1)goto _LLD0;_tmp126=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp125)->f1;_LLCF: return _tmp124 == _tmp126;_LLD0: _tmp127=_tmp11E.f1;if(*((int*)
_tmp127)!= 2)goto _LLD2;_tmp128=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp127)->f1;
_tmp129=_tmp11E.f2;if(*((int*)_tmp129)!= 2)goto _LLD2;_tmp12A=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp129)->f1;_LLD1: return _tmp128 == _tmp12A;_LLD2: _tmp12B=_tmp11E.f1;if(*((int*)
_tmp12B)!= 3)goto _LLD4;_tmp12C=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp12B)->f1;
_tmp12D=_tmp11E.f2;if(*((int*)_tmp12D)!= 3)goto _LLD4;_tmp12E=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmp12D)->f1;_LLD3: return _tmp12C == _tmp12E;_LLD4: _tmp12F=_tmp11E.f1;if(*((int*)
_tmp12F)!= 4)goto _LLD6;_tmp130=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp12F)->f1;
_tmp131=_tmp11E.f2;if(*((int*)_tmp131)!= 4)goto _LLD6;_tmp132=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp131)->f1;_LLD5: return _tmp130 == _tmp132;_LLD6:;_LLD7: return 0;_LLCB:;}}static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct Cyc_List_List*r1s,struct Cyc_List_List*
r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*
_tmp133=r1s;for(0;_tmp133 != 0;_tmp133=_tmp133->tl){struct Cyc_CfFlowInfo_Reln*
_tmp134=(struct Cyc_CfFlowInfo_Reln*)_tmp133->hd;int found=0;{struct Cyc_List_List*
_tmp135=r2s;for(0;_tmp135 != 0;_tmp135=_tmp135->tl){struct Cyc_CfFlowInfo_Reln*
_tmp136=(struct Cyc_CfFlowInfo_Reln*)_tmp135->hd;if(_tmp134 == _tmp136  || _tmp134->vd
== _tmp136->vd  && Cyc_CfFlowInfo_same_relop((void*)_tmp134->rop,(void*)_tmp136->rop)){
res=({struct Cyc_List_List*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->hd=
_tmp134;_tmp137->tl=res;_tmp137;});found=1;break;}}}if(!found)diff=1;}}if(!diff)
res=r1s;return res;}}static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,
struct Cyc_CfFlowInfo_Place*place){struct _handler_cons _tmp138;_push_handler(&
_tmp138);{int _tmp13A=0;if(setjmp(_tmp138.handler))_tmp13A=1;if(!_tmp13A){*l=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*
l,place);;_pop_handler();}else{void*_tmp139=(void*)_exn_thrown;void*_tmp13C=
_tmp139;_LLD9: if(_tmp13C != Cyc_Core_Not_found)goto _LLDB;_LLDA: goto _LLD8;_LLDB:;
_LLDC:(void)_throw(_tmp13C);_LLD8:;}}}void Cyc_CfFlowInfo_print_place_list(struct
Cyc_List_List*x){int _tmp13D=1;while(x != 0){if(!_tmp13D){({void*_tmp13E[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp13F=", ";_tag_dynforward(_tmp13F,sizeof(char),
_get_zero_arr_size(_tmp13F,3));}),_tag_dynforward(_tmp13E,sizeof(void*),0));});
_tmp13D=0;}Cyc_CfFlowInfo_print_place((struct Cyc_CfFlowInfo_Place*)x->hd);x=x->tl;}({
void*_tmp140[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp141="\n";
_tag_dynforward(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,2));}),
_tag_dynforward(_tmp140,sizeof(void*),0));});}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_or_consume(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;{struct _RegionHandle _tmp142=
_new_region("r");struct _RegionHandle*r=& _tmp142;_push_region(r);{struct Cyc_List_List*
_tmp143=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))
Cyc_List_rcopy)(r,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp143,c2.may_consume);while(_tmp143 != 0){mc=({struct
Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp143))->hd;_tmp144->tl=mc;_tmp144;});_tmp143=((
struct Cyc_List_List*)_check_null(_tmp143))->tl;}};_pop_region(r);}return({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp145;_tmp145.consumed=Cyc_CfFlowInfo_union_place_set(
c1.consumed,c2.consumed,0);_tmp145.may_consume=mc;_tmp145;});}struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp146=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(c1.may_consume,c2.may_consume);return({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp147;_tmp147.consumed=Cyc_CfFlowInfo_union_place_set(
c1.consumed,c2.consumed,1);_tmp147.may_consume=_tmp146;_tmp147;});}void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Dict_Dict**all_changed,void*f1,void*f2,int or_consumed){if(f1 == f2)
return f1;{struct _tuple0 _tmp149=({struct _tuple0 _tmp148;_tmp148.f1=f1;_tmp148.f2=
f2;_tmp148;});void*_tmp14A;void*_tmp14B;void*_tmp14C;struct Cyc_Dict_Dict*_tmp14D;
struct Cyc_List_List*_tmp14E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp14F;void*_tmp150;
struct Cyc_Dict_Dict*_tmp151;struct Cyc_List_List*_tmp152;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp153;_LLDE: _tmp14A=_tmp149.f1;if((int)_tmp14A != 0)goto _LLE0;_LLDF: return f2;
_LLE0: _tmp14B=_tmp149.f2;if((int)_tmp14B != 0)goto _LLE2;_LLE1: return f1;_LLE2:
_tmp14C=_tmp149.f1;if(_tmp14C <= (void*)1)goto _LLDD;if(*((int*)_tmp14C)!= 0)goto
_LLDD;_tmp14D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14C)->f1;_tmp14E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14C)->f2;_tmp14F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp14C)->f3;_tmp150=_tmp149.f2;if(_tmp150 <= (void*)1)goto _LLDD;if(*((int*)
_tmp150)!= 0)goto _LLDD;_tmp151=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp150)->f1;_tmp152=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp150)->f2;
_tmp153=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp150)->f3;_LLE3: if(((
_tmp14D == _tmp151  && _tmp14E == _tmp152) && _tmp14F.may_consume == _tmp153.may_consume)
 && _tmp14F.consumed == _tmp153.consumed)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmp154=_new_region("rgn");struct _RegionHandle*rgn=& _tmp154;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp155=({struct Cyc_CfFlowInfo_JoinEnv
_tmp15C;_tmp15C.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp15D=_region_malloc(rgn,
sizeof(*_tmp15D));_tmp15D->rgn=rgn;_tmp15D->places=0;_tmp15D;});_tmp15C.d1=
_tmp14D;_tmp15C.d2=_tmp151;_tmp15C;});struct Cyc_Dict_Dict*_tmp156=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmp155,_tmp14D,_tmp151);struct Cyc_List_List*_tmp157=Cyc_CfFlowInfo_join_relns(
_tmp14E,_tmp152);struct Cyc_CfFlowInfo_ConsumeInfo _tmp158=or_consumed?Cyc_CfFlowInfo_or_consume(
_tmp14F,_tmp153): Cyc_CfFlowInfo_and_consume(_tmp14F,_tmp153);void*_tmp15B=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp159=_cycalloc(sizeof(*_tmp159));
_tmp159[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp15A;_tmp15A.tag=0;
_tmp15A.f1=Cyc_CfFlowInfo_escape_these(_tmp155.pile,all_changed,_tmp156);_tmp15A.f2=
_tmp157;_tmp15A.f3=_tmp158;_tmp15A;});_tmp159;});_npop_handler(0);return _tmp15B;};
_pop_region(rgn);}_LLDD:;}}struct _tuple8{void*f1;void*f2;void*f3;};struct _tuple0
Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Dict_Dict**all_changed,struct _tuple0
pr1,struct _tuple0 pr2,int or_consumed){void*_tmp15F;void*_tmp160;struct _tuple0
_tmp15E=pr1;_tmp15F=_tmp15E.f1;_tmp160=_tmp15E.f2;{void*_tmp162;void*_tmp163;
struct _tuple0 _tmp161=pr2;_tmp162=_tmp161.f1;_tmp163=_tmp161.f2;{void*outflow=Cyc_CfFlowInfo_join_flow(
all_changed,_tmp15F,_tmp162,or_consumed);struct _tuple8 _tmp165=({struct _tuple8
_tmp164;_tmp164.f1=_tmp15F;_tmp164.f2=_tmp162;_tmp164.f3=outflow;_tmp164;});void*
_tmp166;void*_tmp167;void*_tmp168;struct Cyc_Dict_Dict*_tmp169;void*_tmp16A;
struct Cyc_Dict_Dict*_tmp16B;void*_tmp16C;struct Cyc_Dict_Dict*_tmp16D;struct Cyc_List_List*
_tmp16E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp16F;_LLE5: _tmp166=_tmp165.f1;if((int)
_tmp166 != 0)goto _LLE7;_LLE6: return({struct _tuple0 _tmp170;_tmp170.f1=outflow;
_tmp170.f2=_tmp163;_tmp170;});_LLE7: _tmp167=_tmp165.f2;if((int)_tmp167 != 0)goto
_LLE9;_LLE8: return({struct _tuple0 _tmp171;_tmp171.f1=outflow;_tmp171.f2=_tmp160;
_tmp171;});_LLE9: _tmp168=_tmp165.f1;if(_tmp168 <= (void*)1)goto _LLEB;if(*((int*)
_tmp168)!= 0)goto _LLEB;_tmp169=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp168)->f1;_tmp16A=_tmp165.f2;if(_tmp16A <= (void*)1)goto _LLEB;if(*((int*)
_tmp16A)!= 0)goto _LLEB;_tmp16B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp16A)->f1;_tmp16C=_tmp165.f3;if(_tmp16C <= (void*)1)goto _LLEB;if(*((int*)
_tmp16C)!= 0)goto _LLEB;_tmp16D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp16C)->f1;_tmp16E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp16C)->f2;
_tmp16F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp16C)->f3;_LLEA: if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp160,
_tmp163))return({struct _tuple0 _tmp172;_tmp172.f1=outflow;_tmp172.f2=_tmp163;
_tmp172;});if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp163,_tmp160))return({struct _tuple0 _tmp173;_tmp173.f1=outflow;_tmp173.f2=
_tmp160;_tmp173;});{struct _RegionHandle _tmp174=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp174;_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv
_tmp175=({struct Cyc_CfFlowInfo_JoinEnv _tmp17B;_tmp17B.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmp17C=_region_malloc(rgn,sizeof(*_tmp17C));_tmp17C->rgn=rgn;_tmp17C->places=0;
_tmp17C;});_tmp17B.d1=_tmp169;_tmp17B.d2=_tmp16B;_tmp17B;});void*_tmp176=((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(&
_tmp175,0,_tmp160,_tmp163);struct _tuple0 _tmp17A=({struct _tuple0 _tmp177;_tmp177.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp178=_cycalloc(sizeof(*
_tmp178));_tmp178[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp179;_tmp179.tag=
0;_tmp179.f1=Cyc_CfFlowInfo_escape_these(_tmp175.pile,all_changed,_tmp16D);
_tmp179.f2=_tmp16E;_tmp179.f3=_tmp16F;_tmp179;});_tmp178;});_tmp177.f2=_tmp176;
_tmp177;});_npop_handler(0);return _tmp17A;};_pop_region(rgn);}_LLEB:;_LLEC:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp17D=_cycalloc(sizeof(*
_tmp17D));_tmp17D[0]=({struct Cyc_Core_Impossible_struct _tmp17E;_tmp17E.tag=Cyc_Core_Impossible;
_tmp17E.f1=({const char*_tmp17F="join_flow_and_rval: BottomFL outflow";
_tag_dynforward(_tmp17F,sizeof(char),_get_zero_arr_size(_tmp17F,37));});_tmp17E;});
_tmp17D;}));_LLE4:;}}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,
struct _dynforward_ptr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){int changed1=env->changed == 
Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
k))Cyc_Dict_member)(env->chg1,env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two
 || ((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
env->chg2,env->curr_place);if(changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(((env->joinenv).pile)->rgn,env->curr_place,
env->chg2))return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(((
env->joinenv).pile)->rgn,env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{
struct _tuple0 _tmp181=({struct _tuple0 _tmp180;_tmp180.f1=r1;_tmp180.f2=r2;_tmp180;});
void*_tmp182;struct Cyc_Dict_Dict*_tmp183;void*_tmp184;struct Cyc_Dict_Dict*
_tmp185;_LLEE: _tmp182=_tmp181.f1;if(_tmp182 <= (void*)3)goto _LLF0;if(*((int*)
_tmp182)!= 4)goto _LLF0;_tmp183=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp182)->f1;
_tmp184=_tmp181.f2;if(_tmp184 <= (void*)3)goto _LLF0;if(*((int*)_tmp184)!= 4)goto
_LLF0;_tmp185=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp184)->f1;_LLEF: {
struct Cyc_Dict_Dict*_tmp186=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,
struct _dynforward_ptr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmp183,_tmp185);if(_tmp186 == _tmp183)return r1;if(_tmp186 == _tmp185)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp187=_cycalloc(sizeof(*
_tmp187));_tmp187[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp188;_tmp188.tag=
4;_tmp188.f1=_tmp186;_tmp188;});_tmp187;});}_LLF0:;_LLF1:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189[0]=({
struct Cyc_Core_Impossible_struct _tmp18A;_tmp18A.tag=Cyc_Core_Impossible;_tmp18A.f1=({
const char*_tmp18B="after_absRval -- non-aggregates!";_tag_dynforward(_tmp18B,
sizeof(char),_get_zero_arr_size(_tmp18B,33));});_tmp18A;});_tmp189;}));_LLED:;}}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,
struct _dynforward_ptr*key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**
_tmp18C=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp18D=env->changed;*
env->last_field_cell=({struct Cyc_List_List*_tmp18E=_cycalloc(sizeof(*_tmp18E));
_tmp18E->hd=key;_tmp18E->tl=0;_tmp18E;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmp18F=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp18C;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp18D;return _tmp18F;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp190;_tmp190.root=(void*)root;
_tmp190.fields=0;_tmp190;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Dict_Dict**all_changed,void*f1,void*f2,struct Cyc_Dict_Dict*chg1,struct
Cyc_Dict_Dict*chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(void*)((
void*)0)};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((void*)& dummy_rawexp),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};if(f1 == f2)return f1;{struct _tuple0
_tmp192=({struct _tuple0 _tmp191;_tmp191.f1=f1;_tmp191.f2=f2;_tmp191;});void*
_tmp193;void*_tmp194;void*_tmp195;struct Cyc_Dict_Dict*_tmp196;struct Cyc_List_List*
_tmp197;struct Cyc_CfFlowInfo_ConsumeInfo _tmp198;void*_tmp199;struct Cyc_Dict_Dict*
_tmp19A;struct Cyc_List_List*_tmp19B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp19C;
_LLF3: _tmp193=_tmp192.f1;if((int)_tmp193 != 0)goto _LLF5;_LLF4: goto _LLF6;_LLF5:
_tmp194=_tmp192.f2;if((int)_tmp194 != 0)goto _LLF7;_LLF6: return(void*)0;_LLF7:
_tmp195=_tmp192.f1;if(_tmp195 <= (void*)1)goto _LLF2;if(*((int*)_tmp195)!= 0)goto
_LLF2;_tmp196=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp195)->f1;_tmp197=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp195)->f2;_tmp198=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp195)->f3;_tmp199=_tmp192.f2;if(_tmp199 <= (void*)1)goto _LLF2;if(*((int*)
_tmp199)!= 0)goto _LLF2;_tmp19A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp199)->f1;_tmp19B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp199)->f2;
_tmp19C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp199)->f3;_LLF8: if(((
_tmp196 == _tmp19A  && _tmp197 == _tmp19B) && _tmp198.may_consume == _tmp19C.may_consume)
 && _tmp198.consumed == _tmp19C.consumed)return f1;{struct _RegionHandle _tmp19D=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp19D;_push_region(rgn);{struct Cyc_CfFlowInfo_Place*
_tmp19E=({struct Cyc_CfFlowInfo_Place*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->root=(
void*)((void*)& dummy_root);_tmp1A9->fields=0;_tmp1A9;});struct Cyc_CfFlowInfo_AfterEnv
_tmp19F=({struct Cyc_CfFlowInfo_AfterEnv _tmp1A6;_tmp1A6.joinenv=({struct Cyc_CfFlowInfo_JoinEnv
_tmp1A7;_tmp1A7.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1A8=_region_malloc(rgn,
sizeof(*_tmp1A8));_tmp1A8->rgn=rgn;_tmp1A8->places=0;_tmp1A8;});_tmp1A7.d1=
_tmp196;_tmp1A7.d2=_tmp19A;_tmp1A7;});_tmp1A6.chg1=chg1;_tmp1A6.chg2=chg2;
_tmp1A6.curr_place=_tmp19E;_tmp1A6.last_field_cell=& _tmp19E->fields;_tmp1A6.changed=
Cyc_CfFlowInfo_Neither;_tmp1A6;});struct Cyc_Dict_Dict*_tmp1A0=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp19F,_tmp196,_tmp19A);struct Cyc_List_List*_tmp1A1=Cyc_CfFlowInfo_join_relns(
_tmp197,_tmp19B);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A2=Cyc_CfFlowInfo_and_consume(
_tmp198,_tmp19C);void*_tmp1A5=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=Cyc_CfFlowInfo_escape_these((_tmp19F.joinenv).pile,
all_changed,_tmp1A0);_tmp1A4.f2=_tmp1A1;_tmp1A4.f3=_tmp1A2;_tmp1A4;});_tmp1A3;});
_npop_handler(0);return _tmp1A5;};_pop_region(rgn);}_LLF2:;}}static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1AD;void*_tmp1AE;void*_tmp1AF;struct Cyc_CfFlowInfo_TagCmp*_tmp1AC=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp1AD=*_tmp1AC;_tmp1AE=(void*)_tmp1AD.cmp;_tmp1AF=(void*)_tmp1AD.bd;{
struct Cyc_List_List*_tmp1B0=l1;for(0;_tmp1B0 != 0;_tmp1B0=_tmp1B0->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1B2;void*_tmp1B3;void*_tmp1B4;struct Cyc_CfFlowInfo_TagCmp*_tmp1B1=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp1B0->hd;_tmp1B2=*_tmp1B1;_tmp1B3=(void*)_tmp1B2.cmp;_tmp1B4=(void*)_tmp1B2.bd;
if(_tmp1B3 == _tmp1AE  && Cyc_Tcutil_typecmp(_tmp1B4,_tmp1AF)== 0)break;}if(
_tmp1B0 == 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp1B6=({struct
_tuple0 _tmp1B5;_tmp1B5.f1=r1;_tmp1B5.f2=r2;_tmp1B5;});void*_tmp1B7;struct Cyc_CfFlowInfo_Place*
_tmp1B8;void*_tmp1B9;struct Cyc_CfFlowInfo_Place*_tmp1BA;void*_tmp1BB;void*
_tmp1BC;void*_tmp1BD;struct Cyc_Dict_Dict*_tmp1BE;void*_tmp1BF;struct Cyc_Dict_Dict*
_tmp1C0;void*_tmp1C1;void*_tmp1C2;void*_tmp1C3;void*_tmp1C4;void*_tmp1C5;void*
_tmp1C6;void*_tmp1C7;struct Cyc_List_List*_tmp1C8;void*_tmp1C9;struct Cyc_List_List*
_tmp1CA;void*_tmp1CB;_LLFA: _tmp1B7=_tmp1B6.f1;if(_tmp1B7 <= (void*)3)goto _LLFC;
if(*((int*)_tmp1B7)!= 2)goto _LLFC;_tmp1B8=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp1B7)->f1;_tmp1B9=_tmp1B6.f2;if(_tmp1B9 <= (void*)3)goto _LLFC;if(*((int*)
_tmp1B9)!= 2)goto _LLFC;_tmp1BA=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1B9)->f1;
_LLFB: return Cyc_CfFlowInfo_place_cmp(_tmp1B8,_tmp1BA)== 0;_LLFC: _tmp1BB=_tmp1B6.f1;
if(_tmp1BB <= (void*)3)goto _LLFE;if(*((int*)_tmp1BB)!= 2)goto _LLFE;_LLFD: goto
_LLFF;_LLFE: _tmp1BC=_tmp1B6.f2;if(_tmp1BC <= (void*)3)goto _LL100;if(*((int*)
_tmp1BC)!= 2)goto _LL100;_LLFF: return 0;_LL100: _tmp1BD=_tmp1B6.f1;if(_tmp1BD <= (
void*)3)goto _LL102;if(*((int*)_tmp1BD)!= 4)goto _LL102;_tmp1BE=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1BD)->f1;_tmp1BF=_tmp1B6.f2;if(_tmp1BF <= (void*)3)goto _LL102;if(*((int*)
_tmp1BF)!= 4)goto _LL102;_tmp1C0=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1BF)->f1;
_LL101: return _tmp1BE == _tmp1C0  || ((int(*)(int(*f)(struct _dynforward_ptr*,void*,
void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((
int(*)(struct _dynforward_ptr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp1BE,_tmp1C0);_LL102: _tmp1C1=_tmp1B6.f2;if((int)_tmp1C1 != 2)goto _LL104;_LL103:
return r1 == (void*)1;_LL104: _tmp1C2=_tmp1B6.f2;if((int)_tmp1C2 != 0)goto _LL106;
_LL105: goto _LL107;_LL106: _tmp1C3=_tmp1B6.f2;if((int)_tmp1C3 != 1)goto _LL108;
_LL107: return 0;_LL108: _tmp1C4=_tmp1B6.f1;if(_tmp1C4 <= (void*)3)goto _LL10A;if(*((
int*)_tmp1C4)!= 1)goto _LL10A;_tmp1C5=_tmp1B6.f2;if(_tmp1C5 <= (void*)3)goto _LL10A;
if(*((int*)_tmp1C5)!= 1)goto _LL10A;_LL109: goto _LLF9;_LL10A: _tmp1C6=_tmp1B6.f1;
if(_tmp1C6 <= (void*)3)goto _LL10C;if(*((int*)_tmp1C6)!= 1)goto _LL10C;_LL10B:
return 0;_LL10C: _tmp1C7=_tmp1B6.f1;if(_tmp1C7 <= (void*)3)goto _LL10E;if(*((int*)
_tmp1C7)!= 3)goto _LL10E;_tmp1C8=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1C7)->f1;
_tmp1C9=_tmp1B6.f2;if(_tmp1C9 <= (void*)3)goto _LL10E;if(*((int*)_tmp1C9)!= 3)goto
_LL10E;_tmp1CA=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1C9)->f1;_LL10D: return
Cyc_CfFlowInfo_tag_cmps_lessthan_approx(_tmp1C8,_tmp1CA);_LL10E: _tmp1CB=_tmp1B6.f2;
if(_tmp1CB <= (void*)3)goto _LL110;if(*((int*)_tmp1CB)!= 3)goto _LL110;_LL10F:
return 0;_LL110:;_LL111: goto _LLF9;_LLF9:;}{struct _tuple0 _tmp1CD=({struct _tuple0
_tmp1CC;_tmp1CC.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp1CC.f2=Cyc_CfFlowInfo_initlevel_approx(
r2);_tmp1CC;});void*_tmp1CE;void*_tmp1CF;void*_tmp1D0;void*_tmp1D1;void*_tmp1D2;
void*_tmp1D3;_LL113: _tmp1CE=_tmp1CD.f1;if((int)_tmp1CE != 2)goto _LL115;_tmp1CF=
_tmp1CD.f2;if((int)_tmp1CF != 2)goto _LL115;_LL114: return 1;_LL115: _tmp1D0=_tmp1CD.f2;
if((int)_tmp1D0 != 0)goto _LL117;_LL116: return 1;_LL117: _tmp1D1=_tmp1CD.f1;if((int)
_tmp1D1 != 0)goto _LL119;_LL118: return 0;_LL119: _tmp1D2=_tmp1CD.f2;if((int)_tmp1D2
!= 1)goto _LL11B;_LL11A: return 1;_LL11B: _tmp1D3=_tmp1CD.f1;if((int)_tmp1D3 != 1)
goto _LL112;_LL11C: return 0;_LL112:;}}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp1D4=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp1D5=r2s;for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1D6=(struct Cyc_CfFlowInfo_Reln*)_tmp1D5->hd;if(
_tmp1D4 == _tmp1D6  || _tmp1D4->vd == _tmp1D6->vd  && Cyc_CfFlowInfo_same_relop((
void*)_tmp1D4->rop,(void*)_tmp1D6->rop)){found=1;break;}}}if(!found)return 0;}
return 1;}int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo c1,
struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume != c2.may_consume){struct
Cyc_List_List*_tmp1D7=c1.may_consume;struct Cyc_List_List*_tmp1D8=c2.may_consume;
for(0;_tmp1D8 != 0  && _tmp1D7 != 0;_tmp1D8=_tmp1D8->tl){if((unsigned int)_tmp1D7 == (
unsigned int)_tmp1D8)goto compare_consumed_set;{int _tmp1D9=Cyc_CfFlowInfo_place_cmp((
struct Cyc_CfFlowInfo_Place*)_tmp1D7->hd,(struct Cyc_CfFlowInfo_Place*)_tmp1D8->hd);
if(_tmp1D9 == 0)_tmp1D7=_tmp1D7->tl;}}if(_tmp1D7 != 0)return 0;}
compare_consumed_set: return Cyc_CfFlowInfo_place_set_subset(c1.consumed,c2.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2){if(f1 == f2)return 1;{
struct _tuple0 _tmp1DB=({struct _tuple0 _tmp1DA;_tmp1DA.f1=f1;_tmp1DA.f2=f2;_tmp1DA;});
void*_tmp1DC;void*_tmp1DD;void*_tmp1DE;struct Cyc_Dict_Dict*_tmp1DF;struct Cyc_List_List*
_tmp1E0;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1E1;void*_tmp1E2;struct Cyc_Dict_Dict*
_tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1E5;
_LL11E: _tmp1DC=_tmp1DB.f1;if((int)_tmp1DC != 0)goto _LL120;_LL11F: return 1;_LL120:
_tmp1DD=_tmp1DB.f2;if((int)_tmp1DD != 0)goto _LL122;_LL121: return 0;_LL122: _tmp1DE=
_tmp1DB.f1;if(_tmp1DE <= (void*)1)goto _LL11D;if(*((int*)_tmp1DE)!= 0)goto _LL11D;
_tmp1DF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1DE)->f1;_tmp1E0=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1DE)->f2;_tmp1E1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1DE)->f3;_tmp1E2=_tmp1DB.f2;if(_tmp1E2 <= (void*)1)goto _LL11D;if(*((int*)
_tmp1E2)!= 0)goto _LL11D;_tmp1E3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1E2)->f1;_tmp1E4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1E2)->f2;
_tmp1E5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1E2)->f3;_LL123: if(((
_tmp1DF == _tmp1E3  && _tmp1E0 == _tmp1E4) && _tmp1E1.may_consume == _tmp1E5.may_consume)
 && _tmp1E1.consumed == _tmp1E5.consumed)return 1;return(Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp1DF,_tmp1E3) && Cyc_CfFlowInfo_relns_approx(
_tmp1E0,_tmp1E4)) && Cyc_CfFlowInfo_consume_approx(_tmp1E1,_tmp1E5);_LL11D:;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*rs,struct
Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=
p->tl){struct Cyc_CfFlowInfo_Reln*_tmp1E6=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(
_tmp1E6->vd == v){found=1;break;}{void*_tmp1E7=(void*)_tmp1E6->rop;struct Cyc_Absyn_Vardecl*
_tmp1E8;struct Cyc_Absyn_Vardecl*_tmp1E9;struct Cyc_Absyn_Vardecl*_tmp1EA;_LL125:
if(*((int*)_tmp1E7)!= 1)goto _LL127;_tmp1E8=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp1E7)->f1;_LL126: _tmp1E9=_tmp1E8;goto _LL128;_LL127: if(*((int*)_tmp1E7)!= 2)
goto _LL129;_tmp1E9=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1E7)->f1;_LL128:
_tmp1EA=_tmp1E9;goto _LL12A;_LL129: if(*((int*)_tmp1E7)!= 4)goto _LL12B;_tmp1EA=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp1E7)->f1;_LL12A: if(v == _tmp1EA)found=
1;goto _LL124;_LL12B:;_LL12C: goto _LL124;_LL124:;}}if(!found)return rs;{struct Cyc_List_List*
_tmp1EB=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp1EC=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp1EC->vd != v){{void*_tmp1ED=(void*)_tmp1EC->rop;struct Cyc_Absyn_Vardecl*
_tmp1EE;struct Cyc_Absyn_Vardecl*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;_LL12E:
if(*((int*)_tmp1ED)!= 1)goto _LL130;_tmp1EE=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp1ED)->f1;_LL12F: _tmp1EF=_tmp1EE;goto _LL131;_LL130: if(*((int*)_tmp1ED)!= 2)
goto _LL132;_tmp1EF=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1ED)->f1;_LL131:
_tmp1F0=_tmp1EF;goto _LL133;_LL132: if(*((int*)_tmp1ED)!= 4)goto _LL134;_tmp1F0=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp1ED)->f1;_LL133: if(v == _tmp1F0)
continue;goto _LL12D;_LL134:;_LL135: goto _LL12D;_LL12D:;}_tmp1EB=({struct Cyc_List_List*
_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->hd=_tmp1EC;_tmp1F1->tl=_tmp1EB;
_tmp1F1;});}}return _tmp1EB;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp1F2=(void*)e->r;void*
_tmp1F3;struct Cyc_Absyn_Vardecl*_tmp1F4;void*_tmp1F5;struct Cyc_Absyn_Vardecl*
_tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Vardecl*_tmp1F8;void*_tmp1F9;struct Cyc_Absyn_Vardecl*
_tmp1FA;_LL137: if(*((int*)_tmp1F2)!= 1)goto _LL139;_tmp1F3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F2)->f2;if(_tmp1F3 <= (void*)1)goto _LL139;if(*((int*)_tmp1F3)!= 0)goto _LL139;
_tmp1F4=((struct Cyc_Absyn_Global_b_struct*)_tmp1F3)->f1;_LL138: _tmp1F6=_tmp1F4;
goto _LL13A;_LL139: if(*((int*)_tmp1F2)!= 1)goto _LL13B;_tmp1F5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F2)->f2;if(_tmp1F5 <= (void*)1)goto _LL13B;if(*((int*)_tmp1F5)!= 2)goto _LL13B;
_tmp1F6=((struct Cyc_Absyn_Param_b_struct*)_tmp1F5)->f1;_LL13A: _tmp1F8=_tmp1F6;
goto _LL13C;_LL13B: if(*((int*)_tmp1F2)!= 1)goto _LL13D;_tmp1F7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F2)->f2;if(_tmp1F7 <= (void*)1)goto _LL13D;if(*((int*)_tmp1F7)!= 3)goto _LL13D;
_tmp1F8=((struct Cyc_Absyn_Local_b_struct*)_tmp1F7)->f1;_LL13C: _tmp1FA=_tmp1F8;
goto _LL13E;_LL13D: if(*((int*)_tmp1F2)!= 1)goto _LL13F;_tmp1F9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F2)->f2;if(_tmp1F9 <= (void*)1)goto _LL13F;if(*((int*)_tmp1F9)!= 4)goto _LL13F;
_tmp1FA=((struct Cyc_Absyn_Pat_b_struct*)_tmp1F9)->f1;_LL13E: if(!_tmp1FA->escapes)
return Cyc_CfFlowInfo_reln_kill_var(r,_tmp1FA);goto _LL136;_LL13F:;_LL140: goto
_LL136;_LL136:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)
return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{void*_tmp1FB=(void*)e->r;struct Cyc_Absyn_MallocInfo
_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;int _tmp1FE;_LL142: if(*((int*)_tmp1FB)!= 35)
goto _LL144;_tmp1FC=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1FB)->f1;_tmp1FD=
_tmp1FC.num_elts;_tmp1FE=_tmp1FC.fat_result;if(_tmp1FE != 1)goto _LL144;_LL143:
malloc_loop: {void*_tmp1FF=(void*)_tmp1FD->r;struct Cyc_Absyn_Exp*_tmp200;void*
_tmp201;struct Cyc_Absyn_Vardecl*_tmp202;void*_tmp203;struct Cyc_Absyn_Vardecl*
_tmp204;void*_tmp205;struct Cyc_Absyn_Vardecl*_tmp206;void*_tmp207;struct Cyc_Absyn_Vardecl*
_tmp208;_LL147: if(*((int*)_tmp1FF)!= 15)goto _LL149;_tmp200=((struct Cyc_Absyn_Cast_e_struct*)
_tmp1FF)->f2;_LL148: _tmp1FD=_tmp200;goto malloc_loop;_LL149: if(*((int*)_tmp1FF)!= 
1)goto _LL14B;_tmp201=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1FF)->f2;if(
_tmp201 <= (void*)1)goto _LL14B;if(*((int*)_tmp201)!= 4)goto _LL14B;_tmp202=((
struct Cyc_Absyn_Pat_b_struct*)_tmp201)->f1;_LL14A: _tmp204=_tmp202;goto _LL14C;
_LL14B: if(*((int*)_tmp1FF)!= 1)goto _LL14D;_tmp203=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FF)->f2;if(_tmp203 <= (void*)1)goto _LL14D;if(*((int*)_tmp203)!= 3)goto _LL14D;
_tmp204=((struct Cyc_Absyn_Local_b_struct*)_tmp203)->f1;_LL14C: _tmp206=_tmp204;
goto _LL14E;_LL14D: if(*((int*)_tmp1FF)!= 1)goto _LL14F;_tmp205=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FF)->f2;if(_tmp205 <= (void*)1)goto _LL14F;if(*((int*)_tmp205)!= 2)goto _LL14F;
_tmp206=((struct Cyc_Absyn_Param_b_struct*)_tmp205)->f1;_LL14E: _tmp208=_tmp206;
goto _LL150;_LL14F: if(*((int*)_tmp1FF)!= 1)goto _LL151;_tmp207=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FF)->f2;if(_tmp207 <= (void*)1)goto _LL151;if(*((int*)_tmp207)!= 0)goto _LL151;
_tmp208=((struct Cyc_Absyn_Global_b_struct*)_tmp207)->f1;_LL150: if(_tmp208->escapes)
return r;return({struct Cyc_List_List*_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->vd=
_tmp208;_tmp20A->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp20C;_tmp20C.tag=4;_tmp20C.f1=v;_tmp20C;});_tmp20B;}));_tmp20A;});_tmp209->tl=
r;_tmp209;});_LL151:;_LL152: return r;_LL146:;}_LL144:;_LL145: goto _LL141;_LL141:;}{
void*_tmp20D=Cyc_Tcutil_compress((void*)v->type);_LL154: if(_tmp20D <= (void*)4)
goto _LL156;if(*((int*)_tmp20D)!= 5)goto _LL156;_LL155: goto _LL153;_LL156:;_LL157:
return r;_LL153:;}loop: {void*_tmp20E=(void*)e->r;struct Cyc_Absyn_Exp*_tmp20F;
void*_tmp210;int _tmp211;void*_tmp212;struct Cyc_List_List*_tmp213;struct Cyc_List_List
_tmp214;struct Cyc_List_List*_tmp215;struct Cyc_List_List _tmp216;struct Cyc_Absyn_Exp*
_tmp217;void*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_List_List _tmp21A;
struct Cyc_Absyn_Exp*_tmp21B;_LL159: if(*((int*)_tmp20E)!= 15)goto _LL15B;_tmp20F=((
struct Cyc_Absyn_Cast_e_struct*)_tmp20E)->f2;_LL15A: e=_tmp20F;goto loop;_LL15B: if(*((
int*)_tmp20E)!= 0)goto _LL15D;_tmp210=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp20E)->f1;if(_tmp210 <= (void*)1)goto _LL15D;if(*((int*)_tmp210)!= 2)goto _LL15D;
_tmp211=((struct Cyc_Absyn_Int_c_struct*)_tmp210)->f2;_LL15C: return({struct Cyc_List_List*
_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D->vd=v;_tmp21D->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_EqualConst_struct*_tmp21E=_cycalloc_atomic(sizeof(*_tmp21E));
_tmp21E[0]=({struct Cyc_CfFlowInfo_EqualConst_struct _tmp21F;_tmp21F.tag=0;_tmp21F.f1=(
unsigned int)_tmp211;_tmp21F;});_tmp21E;}));_tmp21D;});_tmp21C->tl=r;_tmp21C;});
_LL15D: if(*((int*)_tmp20E)!= 3)goto _LL15F;_tmp212=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp20E)->f1;if((int)_tmp212 != 4)goto _LL15F;_tmp213=((struct Cyc_Absyn_Primop_e_struct*)
_tmp20E)->f2;if(_tmp213 == 0)goto _LL15F;_tmp214=*_tmp213;_tmp215=_tmp214.tl;if(
_tmp215 == 0)goto _LL15F;_tmp216=*_tmp215;_tmp217=(struct Cyc_Absyn_Exp*)_tmp216.hd;
_LL15E:{void*_tmp220=(void*)_tmp217->r;void*_tmp221;struct Cyc_List_List*_tmp222;
struct Cyc_List_List _tmp223;struct Cyc_Absyn_Exp*_tmp224;_LL164: if(*((int*)_tmp220)
!= 3)goto _LL166;_tmp221=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp220)->f1;
if((int)_tmp221 != 19)goto _LL166;_tmp222=((struct Cyc_Absyn_Primop_e_struct*)
_tmp220)->f2;if(_tmp222 == 0)goto _LL166;_tmp223=*_tmp222;_tmp224=(struct Cyc_Absyn_Exp*)
_tmp223.hd;_LL165:{void*_tmp225=(void*)_tmp224->r;void*_tmp226;struct Cyc_Absyn_Vardecl*
_tmp227;void*_tmp228;struct Cyc_Absyn_Vardecl*_tmp229;void*_tmp22A;struct Cyc_Absyn_Vardecl*
_tmp22B;void*_tmp22C;struct Cyc_Absyn_Vardecl*_tmp22D;_LL169: if(*((int*)_tmp225)
!= 1)goto _LL16B;_tmp226=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp225)->f2;if(
_tmp226 <= (void*)1)goto _LL16B;if(*((int*)_tmp226)!= 0)goto _LL16B;_tmp227=((
struct Cyc_Absyn_Global_b_struct*)_tmp226)->f1;_LL16A: _tmp229=_tmp227;goto _LL16C;
_LL16B: if(*((int*)_tmp225)!= 1)goto _LL16D;_tmp228=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp225)->f2;if(_tmp228 <= (void*)1)goto _LL16D;if(*((int*)_tmp228)!= 3)goto _LL16D;
_tmp229=((struct Cyc_Absyn_Local_b_struct*)_tmp228)->f1;_LL16C: _tmp22B=_tmp229;
goto _LL16E;_LL16D: if(*((int*)_tmp225)!= 1)goto _LL16F;_tmp22A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp225)->f2;if(_tmp22A <= (void*)1)goto _LL16F;if(*((int*)_tmp22A)!= 2)goto _LL16F;
_tmp22B=((struct Cyc_Absyn_Param_b_struct*)_tmp22A)->f1;_LL16E: _tmp22D=_tmp22B;
goto _LL170;_LL16F: if(*((int*)_tmp225)!= 1)goto _LL171;_tmp22C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp225)->f2;if(_tmp22C <= (void*)1)goto _LL171;if(*((int*)_tmp22C)!= 4)goto _LL171;
_tmp22D=((struct Cyc_Absyn_Pat_b_struct*)_tmp22C)->f1;_LL170: if(_tmp22D->escapes)
return r;return({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F->vd=v;
_tmp22F->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp230=
_cycalloc(sizeof(*_tmp230));_tmp230[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp231;_tmp231.tag=2;_tmp231.f1=_tmp22D;_tmp231;});_tmp230;}));_tmp22F;});
_tmp22E->tl=r;_tmp22E;});_LL171:;_LL172: goto _LL168;_LL168:;}goto _LL163;_LL166:;
_LL167: goto _LL163;_LL163:;}goto _LL158;_LL15F: if(*((int*)_tmp20E)!= 3)goto _LL161;
_tmp218=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp20E)->f1;if((int)_tmp218 != 
19)goto _LL161;_tmp219=((struct Cyc_Absyn_Primop_e_struct*)_tmp20E)->f2;if(_tmp219
== 0)goto _LL161;_tmp21A=*_tmp219;_tmp21B=(struct Cyc_Absyn_Exp*)_tmp21A.hd;_LL160:{
void*_tmp232=(void*)_tmp21B->r;void*_tmp233;struct Cyc_Absyn_Vardecl*_tmp234;void*
_tmp235;struct Cyc_Absyn_Vardecl*_tmp236;void*_tmp237;struct Cyc_Absyn_Vardecl*
_tmp238;void*_tmp239;struct Cyc_Absyn_Vardecl*_tmp23A;_LL174: if(*((int*)_tmp232)
!= 1)goto _LL176;_tmp233=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp232)->f2;if(
_tmp233 <= (void*)1)goto _LL176;if(*((int*)_tmp233)!= 0)goto _LL176;_tmp234=((
struct Cyc_Absyn_Global_b_struct*)_tmp233)->f1;_LL175: _tmp236=_tmp234;goto _LL177;
_LL176: if(*((int*)_tmp232)!= 1)goto _LL178;_tmp235=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp232)->f2;if(_tmp235 <= (void*)1)goto _LL178;if(*((int*)_tmp235)!= 3)goto _LL178;
_tmp236=((struct Cyc_Absyn_Local_b_struct*)_tmp235)->f1;_LL177: _tmp238=_tmp236;
goto _LL179;_LL178: if(*((int*)_tmp232)!= 1)goto _LL17A;_tmp237=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp232)->f2;if(_tmp237 <= (void*)1)goto _LL17A;if(*((int*)_tmp237)!= 2)goto _LL17A;
_tmp238=((struct Cyc_Absyn_Param_b_struct*)_tmp237)->f1;_LL179: _tmp23A=_tmp238;
goto _LL17B;_LL17A: if(*((int*)_tmp232)!= 1)goto _LL17C;_tmp239=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp232)->f2;if(_tmp239 <= (void*)1)goto _LL17C;if(*((int*)_tmp239)!= 4)goto _LL17C;
_tmp23A=((struct Cyc_Absyn_Pat_b_struct*)_tmp239)->f1;_LL17B: if(_tmp23A->escapes)
return r;return({struct Cyc_List_List*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C->vd=v;
_tmp23C->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp23D=
_cycalloc(sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp23E;_tmp23E.tag=4;_tmp23E.f1=_tmp23A;_tmp23E;});_tmp23D;}));_tmp23C;});
_tmp23B->tl=r;_tmp23B;});_LL17C:;_LL17D: goto _LL173;_LL173:;}goto _LL158;_LL161:;
_LL162: goto _LL158;_LL158:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*
_tmp23F=(void*)e1->r;void*_tmp240;struct Cyc_Absyn_Vardecl*_tmp241;void*_tmp242;
struct Cyc_Absyn_Vardecl*_tmp243;void*_tmp244;struct Cyc_Absyn_Vardecl*_tmp245;
void*_tmp246;struct Cyc_Absyn_Vardecl*_tmp247;_LL17F: if(*((int*)_tmp23F)!= 1)goto
_LL181;_tmp240=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp23F)->f2;if(_tmp240 <= (
void*)1)goto _LL181;if(*((int*)_tmp240)!= 0)goto _LL181;_tmp241=((struct Cyc_Absyn_Global_b_struct*)
_tmp240)->f1;_LL180: _tmp243=_tmp241;goto _LL182;_LL181: if(*((int*)_tmp23F)!= 1)
goto _LL183;_tmp242=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp23F)->f2;if(
_tmp242 <= (void*)1)goto _LL183;if(*((int*)_tmp242)!= 2)goto _LL183;_tmp243=((
struct Cyc_Absyn_Param_b_struct*)_tmp242)->f1;_LL182: _tmp245=_tmp243;goto _LL184;
_LL183: if(*((int*)_tmp23F)!= 1)goto _LL185;_tmp244=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23F)->f2;if(_tmp244 <= (void*)1)goto _LL185;if(*((int*)_tmp244)!= 3)goto _LL185;
_tmp245=((struct Cyc_Absyn_Local_b_struct*)_tmp244)->f1;_LL184: _tmp247=_tmp245;
goto _LL186;_LL185: if(*((int*)_tmp23F)!= 1)goto _LL187;_tmp246=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23F)->f2;if(_tmp246 <= (void*)1)goto _LL187;if(*((int*)_tmp246)!= 4)goto _LL187;
_tmp247=((struct Cyc_Absyn_Pat_b_struct*)_tmp246)->f1;_LL186: if(!_tmp247->escapes)
return Cyc_CfFlowInfo_reln_assign_var(r,_tmp247,e2);goto _LL17E;_LL187:;_LL188:
goto _LL17E;_LL17E:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){({struct Cyc_String_pa_struct _tmp24A;_tmp24A.tag=0;_tmp24A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string((r->vd)->name));{
void*_tmp248[1]={& _tmp24A};Cyc_fprintf(Cyc_stderr,({const char*_tmp249="%s";
_tag_dynforward(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,3));}),
_tag_dynforward(_tmp248,sizeof(void*),1));}});{void*_tmp24B=(void*)r->rop;
unsigned int _tmp24C;struct Cyc_Absyn_Vardecl*_tmp24D;struct Cyc_Absyn_Vardecl*
_tmp24E;unsigned int _tmp24F;struct Cyc_Absyn_Vardecl*_tmp250;_LL18A: if(*((int*)
_tmp24B)!= 0)goto _LL18C;_tmp24C=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp24B)->f1;_LL18B:({struct Cyc_Int_pa_struct _tmp253;_tmp253.tag=1;_tmp253.f1=(
unsigned long)((int)_tmp24C);{void*_tmp251[1]={& _tmp253};Cyc_fprintf(Cyc_stderr,({
const char*_tmp252="==%d";_tag_dynforward(_tmp252,sizeof(char),_get_zero_arr_size(
_tmp252,5));}),_tag_dynforward(_tmp251,sizeof(void*),1));}});goto _LL189;_LL18C:
if(*((int*)_tmp24B)!= 1)goto _LL18E;_tmp24D=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp24B)->f1;_LL18D:({struct Cyc_String_pa_struct _tmp256;_tmp256.tag=0;_tmp256.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp24D->name));{
void*_tmp254[1]={& _tmp256};Cyc_fprintf(Cyc_stderr,({const char*_tmp255="<%s";
_tag_dynforward(_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,4));}),
_tag_dynforward(_tmp254,sizeof(void*),1));}});goto _LL189;_LL18E: if(*((int*)
_tmp24B)!= 2)goto _LL190;_tmp24E=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp24B)->f1;
_LL18F:({struct Cyc_String_pa_struct _tmp259;_tmp259.tag=0;_tmp259.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp24E->name));{
void*_tmp257[1]={& _tmp259};Cyc_fprintf(Cyc_stderr,({const char*_tmp258="<%s.size";
_tag_dynforward(_tmp258,sizeof(char),_get_zero_arr_size(_tmp258,9));}),
_tag_dynforward(_tmp257,sizeof(void*),1));}});goto _LL189;_LL190: if(*((int*)
_tmp24B)!= 3)goto _LL192;_tmp24F=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp24B)->f1;
_LL191:({struct Cyc_Int_pa_struct _tmp25C;_tmp25C.tag=1;_tmp25C.f1=(unsigned long)((
int)_tmp24F);{void*_tmp25A[1]={& _tmp25C};Cyc_fprintf(Cyc_stderr,({const char*
_tmp25B="<%d";_tag_dynforward(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,4));}),
_tag_dynforward(_tmp25A,sizeof(void*),1));}});goto _LL189;_LL192: if(*((int*)
_tmp24B)!= 4)goto _LL189;_tmp250=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp24B)->f1;_LL193:({struct Cyc_String_pa_struct _tmp25F;_tmp25F.tag=0;_tmp25F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp250->name));{
void*_tmp25D[1]={& _tmp25F};Cyc_fprintf(Cyc_stderr,({const char*_tmp25E="<=%s.size";
_tag_dynforward(_tmp25E,sizeof(char),_get_zero_arr_size(_tmp25E,10));}),
_tag_dynforward(_tmp25D,sizeof(void*),1));}});goto _LL189;_LL189:;}}void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)
r->hd);if(r->tl != 0)({void*_tmp260[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp261=",";_tag_dynforward(_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,2));}),
_tag_dynforward(_tmp260,sizeof(void*),0));});}}void Cyc_CfFlowInfo_print_initlevel(
void*il){void*_tmp262=il;_LL195: if((int)_tmp262 != 0)goto _LL197;_LL196:({void*
_tmp263[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp264="uninitialized";
_tag_dynforward(_tmp264,sizeof(char),_get_zero_arr_size(_tmp264,14));}),
_tag_dynforward(_tmp263,sizeof(void*),0));});goto _LL194;_LL197: if((int)_tmp262 != 
1)goto _LL199;_LL198:({void*_tmp265[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp266="this-initialized";_tag_dynforward(_tmp266,sizeof(char),
_get_zero_arr_size(_tmp266,17));}),_tag_dynforward(_tmp265,sizeof(void*),0));});
goto _LL194;_LL199: if((int)_tmp262 != 2)goto _LL194;_LL19A:({void*_tmp267[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp268="all-initialized";_tag_dynforward(_tmp268,sizeof(
char),_get_zero_arr_size(_tmp268,16));}),_tag_dynforward(_tmp267,sizeof(void*),0));});
goto _LL194;_LL194:;}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp269=root;
struct Cyc_Absyn_Vardecl*_tmp26A;_LL19C: if(*((int*)_tmp269)!= 0)goto _LL19E;
_tmp26A=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp269)->f1;_LL19D:({struct Cyc_String_pa_struct
_tmp26D;_tmp26D.tag=0;_tmp26D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp26A->name));{void*_tmp26B[1]={& _tmp26D};Cyc_fprintf(
Cyc_stderr,({const char*_tmp26C="Root(%s): ";_tag_dynforward(_tmp26C,sizeof(char),
_get_zero_arr_size(_tmp26C,11));}),_tag_dynforward(_tmp26B,sizeof(void*),1));}});
goto _LL19B;_LL19E: if(*((int*)_tmp269)!= 1)goto _LL1A0;_LL19F:({void*_tmp26E[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp26F="MallocPt(_,_)";_tag_dynforward(
_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,14));}),_tag_dynforward(_tmp26E,
sizeof(void*),0));});goto _LL19B;_LL1A0: if(*((int*)_tmp269)!= 2)goto _LL19B;_LL1A1:({
void*_tmp270[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp271="InitParam(_,_)";
_tag_dynforward(_tmp271,sizeof(char),_get_zero_arr_size(_tmp271,15));}),
_tag_dynforward(_tmp270,sizeof(void*),0));});goto _LL19B;_LL19B:;}void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root((void*)p->root);if(p->fields
!= 0)({void*_tmp272[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp273="+(...)";
_tag_dynforward(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,7));}),
_tag_dynforward(_tmp272,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_place_set(
struct Cyc_Dict_Dict*p){struct _RegionHandle _tmp274=_new_region("r");struct
_RegionHandle*r=& _tmp274;_push_region(r);{struct _handler_cons _tmp275;
_push_handler(& _tmp275);{int _tmp277=0;if(setjmp(_tmp275.handler))_tmp277=1;if(!
_tmp277){{struct Cyc_Iter_Iter _tmp278=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict*d))Cyc_Dict_make_iter)(r,p);struct _tuple5
_tmp279=*((struct _tuple5*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(
r,p);int first=1;({void*_tmp27A[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp27B="{ ";
_tag_dynforward(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,3));}),
_tag_dynforward(_tmp27A,sizeof(void*),0));});while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple5*))Cyc_Iter_next)(_tmp278,& _tmp279)){struct Cyc_CfFlowInfo_Place*
_tmp27C=_tmp279.f1;if(!first){({void*_tmp27D[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp27E=", ";_tag_dynforward(_tmp27E,sizeof(char),_get_zero_arr_size(
_tmp27E,3));}),_tag_dynforward(_tmp27D,sizeof(void*),0));});first=0;}Cyc_CfFlowInfo_print_place(
_tmp27C);}({void*_tmp27F[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp280="}\n";
_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,3));}),
_tag_dynforward(_tmp27F,sizeof(void*),0));});};_pop_handler();}else{void*_tmp276=(
void*)_exn_thrown;void*_tmp282=_tmp276;_LL1A3:;_LL1A4:({void*_tmp283[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp284="{ }\n";_tag_dynforward(_tmp284,sizeof(char),
_get_zero_arr_size(_tmp284,5));}),_tag_dynforward(_tmp283,sizeof(void*),0));});
_npop_handler(0);return;_LL1A5:;_LL1A6:(void)_throw(_tmp282);_LL1A2:;}}};
_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*_tmp285=rval;
void*_tmp286;void*_tmp287;struct Cyc_CfFlowInfo_Place*_tmp288;struct Cyc_Dict_Dict*
_tmp289;_LL1A8: if((int)_tmp285 != 0)goto _LL1AA;_LL1A9:({void*_tmp28A[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp28B="Zero";_tag_dynforward(_tmp28B,sizeof(char),
_get_zero_arr_size(_tmp28B,5));}),_tag_dynforward(_tmp28A,sizeof(void*),0));});
goto _LL1A7;_LL1AA: if((int)_tmp285 != 1)goto _LL1AC;_LL1AB:({void*_tmp28C[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp28D="NotZeroAll";_tag_dynforward(_tmp28D,sizeof(char),
_get_zero_arr_size(_tmp28D,11));}),_tag_dynforward(_tmp28C,sizeof(void*),0));});
goto _LL1A7;_LL1AC: if((int)_tmp285 != 2)goto _LL1AE;_LL1AD:({void*_tmp28E[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp28F="NotZeroThis";_tag_dynforward(_tmp28F,sizeof(char),
_get_zero_arr_size(_tmp28F,12));}),_tag_dynforward(_tmp28E,sizeof(void*),0));});
goto _LL1A7;_LL1AE: if(_tmp285 <= (void*)3)goto _LL1B0;if(*((int*)_tmp285)!= 0)goto
_LL1B0;_tmp286=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp285)->f1;
_LL1AF:({void*_tmp290[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp291="Unknown(";
_tag_dynforward(_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,9));}),
_tag_dynforward(_tmp290,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(
_tmp286);({void*_tmp292[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp293=")";
_tag_dynforward(_tmp293,sizeof(char),_get_zero_arr_size(_tmp293,2));}),
_tag_dynforward(_tmp292,sizeof(void*),0));});goto _LL1A7;_LL1B0: if(_tmp285 <= (
void*)3)goto _LL1B2;if(*((int*)_tmp285)!= 1)goto _LL1B2;_tmp287=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp285)->f1;_LL1B1:({void*_tmp294[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp295="Esc(";_tag_dynforward(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,5));}),
_tag_dynforward(_tmp294,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(
_tmp287);({void*_tmp296[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp297=")";
_tag_dynforward(_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,2));}),
_tag_dynforward(_tmp296,sizeof(void*),0));});goto _LL1A7;_LL1B2: if(_tmp285 <= (
void*)3)goto _LL1B4;if(*((int*)_tmp285)!= 2)goto _LL1B4;_tmp288=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp285)->f1;_LL1B3:({void*_tmp298[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp299="AddrOf(";_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size(
_tmp299,8));}),_tag_dynforward(_tmp298,sizeof(void*),0));});Cyc_CfFlowInfo_print_place(
_tmp288);({void*_tmp29A[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp29B=")";
_tag_dynforward(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,2));}),
_tag_dynforward(_tmp29A,sizeof(void*),0));});goto _LL1A7;_LL1B4: if(_tmp285 <= (
void*)3)goto _LL1B6;if(*((int*)_tmp285)!= 3)goto _LL1B6;_LL1B5:({void*_tmp29C[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp29D="TagCmps(?)";_tag_dynforward(_tmp29D,
sizeof(char),_get_zero_arr_size(_tmp29D,11));}),_tag_dynforward(_tmp29C,sizeof(
void*),0));});goto _LL1A7;_LL1B6: if(_tmp285 <= (void*)3)goto _LL1A7;if(*((int*)
_tmp285)!= 4)goto _LL1A7;_tmp289=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp285)->f1;
_LL1B7:({void*_tmp29E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp29F="Aggregate(?)";
_tag_dynforward(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,13));}),
_tag_dynforward(_tmp29E,sizeof(void*),0));});goto _LL1A7;_LL1A7:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){({void*_tmp2A0[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2A1="    ";_tag_dynforward(_tmp2A1,sizeof(char),_get_zero_arr_size(_tmp2A1,5));}),
_tag_dynforward(_tmp2A0,sizeof(void*),0));});Cyc_CfFlowInfo_print_root(root);({
void*_tmp2A2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A3=" --> ";
_tag_dynforward(_tmp2A3,sizeof(char),_get_zero_arr_size(_tmp2A3,6));}),
_tag_dynforward(_tmp2A2,sizeof(void*),0));});Cyc_CfFlowInfo_print_absrval(rval);({
void*_tmp2A4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A5="\n";
_tag_dynforward(_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,2));}),
_tag_dynforward(_tmp2A4,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_flowdict(
struct Cyc_Dict_Dict*d){Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}void
Cyc_CfFlowInfo_print_flow(void*f){void*_tmp2A6=f;struct Cyc_Dict_Dict*_tmp2A7;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp2A8;_LL1B9: if((int)_tmp2A6 != 0)goto _LL1BB;
_LL1BA:({void*_tmp2A9[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AA="  BottomFL\n";
_tag_dynforward(_tmp2AA,sizeof(char),_get_zero_arr_size(_tmp2AA,12));}),
_tag_dynforward(_tmp2A9,sizeof(void*),0));});goto _LL1B8;_LL1BB: if(_tmp2A6 <= (
void*)1)goto _LL1B8;if(*((int*)_tmp2A6)!= 0)goto _LL1B8;_tmp2A7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2A6)->f1;_tmp2A8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A6)->f3;
_LL1BC:({void*_tmp2AB[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AC="  ReachableFL:\n";
_tag_dynforward(_tmp2AC,sizeof(char),_get_zero_arr_size(_tmp2AC,16));}),
_tag_dynforward(_tmp2AB,sizeof(void*),0));});Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,
_tmp2A7);({void*_tmp2AD[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AE="\n    consumed: ";
_tag_dynforward(_tmp2AE,sizeof(char),_get_zero_arr_size(_tmp2AE,16));}),
_tag_dynforward(_tmp2AD,sizeof(void*),0));});Cyc_CfFlowInfo_print_place_set(
_tmp2A8.consumed);({void*_tmp2AF[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2B0="\n    may_consume: ";_tag_dynforward(_tmp2B0,sizeof(char),
_get_zero_arr_size(_tmp2B0,19));}),_tag_dynforward(_tmp2AF,sizeof(void*),0));});
Cyc_CfFlowInfo_print_place_list(_tmp2A8.may_consume);({void*_tmp2B1[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2B2="\n";_tag_dynforward(_tmp2B2,sizeof(char),
_get_zero_arr_size(_tmp2B2,2));}),_tag_dynforward(_tmp2B1,sizeof(void*),0));});
goto _LL1B8;_LL1B8:;}int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*
_tmp2B3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp2B4;struct Cyc_Absyn_TunionInfo
_tmp2B5;struct Cyc_List_List*_tmp2B6;struct Cyc_Core_Opt*_tmp2B7;struct Cyc_List_List*
_tmp2B8;struct Cyc_Absyn_AggrInfo _tmp2B9;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_TunionFieldInfo
_tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_List_List*_tmp2BD;struct Cyc_Absyn_PtrInfo
_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_PtrAtts _tmp2C0;void*_tmp2C1;struct Cyc_List_List*
_tmp2C2;struct Cyc_List_List*_tmp2C3;struct Cyc_Absyn_ArrayInfo _tmp2C4;void*
_tmp2C5;void*_tmp2C6;void*_tmp2C7;void*_tmp2C8;_LL1BE: if((int)_tmp2B3 != 0)goto
_LL1C0;_LL1BF: goto _LL1C1;_LL1C0: if(_tmp2B3 <= (void*)4)goto _LL1C2;if(*((int*)
_tmp2B3)!= 5)goto _LL1C2;_LL1C1: goto _LL1C3;_LL1C2: if((int)_tmp2B3 != 1)goto _LL1C4;
_LL1C3: goto _LL1C5;_LL1C4: if(_tmp2B3 <= (void*)4)goto _LL1D0;if(*((int*)_tmp2B3)!= 
6)goto _LL1C6;_LL1C5: goto _LL1C7;_LL1C6: if(*((int*)_tmp2B3)!= 12)goto _LL1C8;_LL1C7:
goto _LL1C9;_LL1C8: if(*((int*)_tmp2B3)!= 13)goto _LL1CA;_LL1C9: goto _LL1CB;_LL1CA:
if(*((int*)_tmp2B3)!= 14)goto _LL1CC;_LL1CB: goto _LL1CD;_LL1CC: if(*((int*)_tmp2B3)
!= 17)goto _LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(*((int*)_tmp2B3)!= 18)goto _LL1D0;
_LL1CF: goto _LL1D1;_LL1D0: if((int)_tmp2B3 != 2)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2:
if(_tmp2B3 <= (void*)4)goto _LL1D4;if(*((int*)_tmp2B3)!= 0)goto _LL1D4;_LL1D3:
return 0;_LL1D4: if((int)_tmp2B3 != 3)goto _LL1D6;_LL1D5: {void*_tmp2C9=rgn;_LL1F3:
if((int)_tmp2C9 != 0)goto _LL1F5;_LL1F4: return 1;_LL1F5:;_LL1F6: return 0;_LL1F2:;}
_LL1D6: if(_tmp2B3 <= (void*)4)goto _LL1D8;if(*((int*)_tmp2B3)!= 1)goto _LL1D8;
_tmp2B4=((struct Cyc_Absyn_VarType_struct*)_tmp2B3)->f1;_LL1D7: {void*_tmp2CA=rgn;
struct Cyc_Absyn_Tvar*_tmp2CB;_LL1F8: if((int)_tmp2CA != 0)goto _LL1FA;_LL1F9: return
0;_LL1FA: if(_tmp2CA <= (void*)1)goto _LL1F7;if(*((int*)_tmp2CA)!= 0)goto _LL1F7;
_tmp2CB=((struct Cyc_CfFlowInfo_Region_k_struct*)_tmp2CA)->f1;_LL1FB: return Cyc_Absyn_tvar_cmp(
_tmp2B4,_tmp2CB)== 0;_LL1F7:;}_LL1D8: if(_tmp2B3 <= (void*)4)goto _LL1DA;if(*((int*)
_tmp2B3)!= 2)goto _LL1DA;_tmp2B5=((struct Cyc_Absyn_TunionType_struct*)_tmp2B3)->f1;
_tmp2B6=_tmp2B5.targs;_tmp2B7=_tmp2B5.rgn;_LL1D9: if((unsigned int)_tmp2B7  && Cyc_CfFlowInfo_contains_region(
rgn,(void*)_tmp2B7->v))return 1;_tmp2B8=_tmp2B6;goto _LL1DB;_LL1DA: if(_tmp2B3 <= (
void*)4)goto _LL1DC;if(*((int*)_tmp2B3)!= 16)goto _LL1DC;_tmp2B8=((struct Cyc_Absyn_TypedefType_struct*)
_tmp2B3)->f2;_LL1DB: _tmp2BA=_tmp2B8;goto _LL1DD;_LL1DC: if(_tmp2B3 <= (void*)4)goto
_LL1DE;if(*((int*)_tmp2B3)!= 10)goto _LL1DE;_tmp2B9=((struct Cyc_Absyn_AggrType_struct*)
_tmp2B3)->f1;_tmp2BA=_tmp2B9.targs;_LL1DD: _tmp2BC=_tmp2BA;goto _LL1DF;_LL1DE: if(
_tmp2B3 <= (void*)4)goto _LL1E0;if(*((int*)_tmp2B3)!= 3)goto _LL1E0;_tmp2BB=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2B3)->f1;_tmp2BC=_tmp2BB.targs;
_LL1DF: _tmp2BD=_tmp2BC;goto _LL1E1;_LL1E0: if(_tmp2B3 <= (void*)4)goto _LL1E2;if(*((
int*)_tmp2B3)!= 20)goto _LL1E2;_tmp2BD=((struct Cyc_Absyn_JoinEff_struct*)_tmp2B3)->f1;
_LL1E1: return Cyc_List_exists_c(Cyc_CfFlowInfo_contains_region,rgn,_tmp2BD);
_LL1E2: if(_tmp2B3 <= (void*)4)goto _LL1E4;if(*((int*)_tmp2B3)!= 4)goto _LL1E4;
_tmp2BE=((struct Cyc_Absyn_PointerType_struct*)_tmp2B3)->f1;_tmp2BF=(void*)
_tmp2BE.elt_typ;_tmp2C0=_tmp2BE.ptr_atts;_tmp2C1=(void*)_tmp2C0.rgn;_LL1E3:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp2C1) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp2BF);_LL1E4: if(_tmp2B3 <= (void*)4)goto _LL1E6;if(*((int*)_tmp2B3)!= 8)
goto _LL1E6;_LL1E5: return 0;_LL1E6: if(_tmp2B3 <= (void*)4)goto _LL1E8;if(*((int*)
_tmp2B3)!= 9)goto _LL1E8;_tmp2C2=((struct Cyc_Absyn_TupleType_struct*)_tmp2B3)->f1;
_LL1E7: for(0;_tmp2C2 != 0;_tmp2C2=_tmp2C2->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple6*)_tmp2C2->hd)).f2))return 1;}return 0;_LL1E8: if(_tmp2B3 <= (
void*)4)goto _LL1EA;if(*((int*)_tmp2B3)!= 11)goto _LL1EA;_tmp2C3=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2B3)->f2;_LL1E9: for(0;_tmp2C3 != 0;_tmp2C3=_tmp2C3->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp2C3->hd)->type))return 1;}return 0;
_LL1EA: if(_tmp2B3 <= (void*)4)goto _LL1EC;if(*((int*)_tmp2B3)!= 7)goto _LL1EC;
_tmp2C4=((struct Cyc_Absyn_ArrayType_struct*)_tmp2B3)->f1;_tmp2C5=(void*)_tmp2C4.elt_type;
_LL1EB: _tmp2C6=_tmp2C5;goto _LL1ED;_LL1EC: if(_tmp2B3 <= (void*)4)goto _LL1EE;if(*((
int*)_tmp2B3)!= 19)goto _LL1EE;_tmp2C6=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp2B3)->f1;_LL1ED: _tmp2C7=_tmp2C6;goto _LL1EF;_LL1EE: if(_tmp2B3 <= (void*)4)goto
_LL1F0;if(*((int*)_tmp2B3)!= 21)goto _LL1F0;_tmp2C7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp2B3)->f1;_LL1EF: return Cyc_CfFlowInfo_contains_region(rgn,_tmp2C7);_LL1F0: if(
_tmp2B3 <= (void*)4)goto _LL1BD;if(*((int*)_tmp2B3)!= 15)goto _LL1BD;_tmp2C8=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2B3)->f1;_LL1F1: return 0;_LL1BD:;}
struct _tuple9{struct Cyc_Dict_Dict*f1;void*f2;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple9*env,void*root,void*rval){struct _tuple9 _tmp2CD;struct Cyc_Dict_Dict*
_tmp2CE;struct Cyc_Dict_Dict**_tmp2CF;void*_tmp2D0;struct _tuple9*_tmp2CC=env;
_tmp2CD=*_tmp2CC;_tmp2CE=_tmp2CD.f1;_tmp2CF=(struct Cyc_Dict_Dict**)&(*_tmp2CC).f1;
_tmp2D0=_tmp2CD.f2;{void*_tmp2D1=root;struct Cyc_Absyn_Vardecl*_tmp2D2;void*
_tmp2D3;_LL1FD: if(*((int*)_tmp2D1)!= 0)goto _LL1FF;_tmp2D2=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp2D1)->f1;_LL1FE: if(Cyc_CfFlowInfo_contains_region(_tmp2D0,(void*)_tmp2D2->type))
rval=Cyc_CfFlowInfo_typ_to_absrval((void*)_tmp2D2->type,Cyc_CfFlowInfo_unknown_none);*
_tmp2CF=Cyc_Dict_insert(*_tmp2CF,root,rval);goto _LL1FC;_LL1FF: if(*((int*)_tmp2D1)
!= 1)goto _LL201;_tmp2D3=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2D1)->f2;
_LL200: if(!Cyc_CfFlowInfo_contains_region(_tmp2D0,_tmp2D3))*_tmp2CF=Cyc_Dict_insert(*
_tmp2CF,root,rval);goto _LL1FC;_LL201: if(*((int*)_tmp2D1)!= 2)goto _LL1FC;_LL202:
goto _LL1FC;_LL1FC:;}}static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_Dict_Dict*fd,void*rgn){void*_tmp2D4=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*
_tmp2D5;_LL204: if(_tmp2D4 <= (void*)4)goto _LL206;if(*((int*)_tmp2D4)!= 1)goto
_LL206;_tmp2D5=((struct Cyc_Absyn_VarType_struct*)_tmp2D4)->f1;_LL205: {struct Cyc_CfFlowInfo_Region_k_struct
v=({struct Cyc_CfFlowInfo_Region_k_struct _tmp2D7;_tmp2D7.tag=0;_tmp2D7.f1=_tmp2D5;
_tmp2D7;});void*v2=(void*)& v;struct _tuple9 env=({struct _tuple9 _tmp2D6;_tmp2D6.f1=
Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp2D6.f2=v2;_tmp2D6;});((void(*)(void(*
f)(struct _tuple9*,void*,void*),struct _tuple9*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}_LL206: if((int)_tmp2D4 != 3)goto
_LL208;_LL207: {struct _tuple9 env=({struct _tuple9 _tmp2D8;_tmp2D8.f1=Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp);_tmp2D8.f2=(void*)0;_tmp2D8;});((void(*)(void(*f)(
struct _tuple9*,void*,void*),struct _tuple9*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}_LL208:;_LL209:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));_tmp2D9[0]=({
struct Cyc_Core_Impossible_struct _tmp2DA;_tmp2DA.tag=Cyc_Core_Impossible;_tmp2DA.f1=({
const char*_tmp2DB="kill_flowdict_region";_tag_dynforward(_tmp2DB,sizeof(char),
_get_zero_arr_size(_tmp2DB,21));});_tmp2DA;});_tmp2D9;}));_LL203:;}static struct
Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*
rgn){return 0;}void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn){void*_tmp2DC=
f;struct Cyc_Dict_Dict*_tmp2DD;struct Cyc_List_List*_tmp2DE;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2DF;_LL20B: if((int)_tmp2DC != 0)goto _LL20D;_LL20C: return f;_LL20D: if(_tmp2DC <= (
void*)1)goto _LL20A;if(*((int*)_tmp2DC)!= 0)goto _LL20A;_tmp2DD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2DC)->f1;_tmp2DE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2DC)->f2;
_tmp2DF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2DC)->f3;_LL20E: {struct
Cyc_Dict_Dict*_tmp2E0=Cyc_CfFlowInfo_kill_flowdict_region(_tmp2DD,rgn);struct Cyc_List_List*
_tmp2E1=Cyc_CfFlowInfo_kill_relns_region(_tmp2DE,rgn);return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2E2=_cycalloc(sizeof(*_tmp2E2));_tmp2E2[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp2E3;_tmp2E3.tag=0;_tmp2E3.f1=_tmp2E0;_tmp2E3.f2=_tmp2E1;_tmp2E3.f3=_tmp2DF;
_tmp2E3;});_tmp2E2;});}_LL20A:;}struct _tuple10{struct Cyc_Dict_Dict**f1;struct Cyc_Position_Segment*
f2;};static void Cyc_CfFlowInfo_consume_f(struct _tuple10*env,struct Cyc_CfFlowInfo_Place*
place){struct _tuple10 _tmp2E5;struct Cyc_Dict_Dict**_tmp2E6;struct Cyc_Position_Segment*
_tmp2E7;struct _tuple10*_tmp2E4=env;_tmp2E5=*_tmp2E4;_tmp2E6=_tmp2E5.f1;_tmp2E7=
_tmp2E5.f2;if(Cyc_CfFlowInfo_update_place_set((struct Cyc_Dict_Dict**)_tmp2E6,
place,_tmp2E7)){struct Cyc_Position_Segment*_tmp2E8=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(*_tmp2E6,
place);Cyc_CfFlowInfo_unique_err(place,({const char*_tmp2E9="May consume unique pointer %s more than once (cf. %s)";
_tag_dynforward(_tmp2E9,sizeof(char),_get_zero_arr_size(_tmp2E9,54));}),({const
char*_tmp2EA="May consume unique pointer %s more than once";_tag_dynforward(
_tmp2EA,sizeof(char),_get_zero_arr_size(_tmp2EA,45));}),_tmp2E8,_tmp2E7);}}void*
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,void*f){void*
_tmp2EB=f;struct Cyc_Dict_Dict*_tmp2EC;struct Cyc_List_List*_tmp2ED;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2EE;_LL210: if((int)_tmp2EB != 0)goto _LL212;_LL211: return f;_LL212: if(_tmp2EB <= (
void*)1)goto _LL20F;if(*((int*)_tmp2EB)!= 0)goto _LL20F;_tmp2EC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2EB)->f1;_tmp2ED=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2EB)->f2;
_tmp2EE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2EB)->f3;_LL213: if(
_tmp2EE.may_consume == 0)return f;{struct Cyc_Dict_Dict*_tmp2EF=_tmp2EE.consumed;((
void(*)(void(*f)(struct _tuple10*,struct Cyc_CfFlowInfo_Place*),struct _tuple10*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,({struct
_tuple10*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0->f1=& _tmp2EF;_tmp2F0->f2=
loc;_tmp2F0;}),_tmp2EE.may_consume);return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp2F2;_tmp2F2.tag=0;_tmp2F2.f1=_tmp2EC;_tmp2F2.f2=_tmp2ED;_tmp2F2.f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp2F3;_tmp2F3.consumed=_tmp2EF;_tmp2F3.may_consume=0;
_tmp2F3;});_tmp2F2;});_tmp2F1;});}_LL20F:;}void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,void*f){void*_tmp2F4=f;struct Cyc_Dict_Dict*
_tmp2F5;struct Cyc_List_List*_tmp2F6;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2F7;
_LL215: if((int)_tmp2F4 != 0)goto _LL217;_LL216: return;_LL217: if(_tmp2F4 <= (void*)1)
goto _LL214;if(*((int*)_tmp2F4)!= 0)goto _LL214;_tmp2F5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2F4)->f1;_tmp2F6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2F4)->f2;
_tmp2F7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2F4)->f3;_LL218: {struct
Cyc_List_List*_tmp2F8=_tmp2F7.may_consume;while(_tmp2F8 != 0){if(((int(*)(struct
Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp2F7.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp2F8->hd)){struct Cyc_Position_Segment*_tmp2F9=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
k))Cyc_Dict_lookup)(_tmp2F7.consumed,(struct Cyc_CfFlowInfo_Place*)_tmp2F8->hd);
Cyc_CfFlowInfo_unique_err((struct Cyc_CfFlowInfo_Place*)_tmp2F8->hd,({const char*
_tmp2FA="Read through possibly consumed unique pointer %s (cf. %s)";
_tag_dynforward(_tmp2FA,sizeof(char),_get_zero_arr_size(_tmp2FA,58));}),({const
char*_tmp2FB="Read through possibly consumed unique pointer %s";_tag_dynforward(
_tmp2FB,sizeof(char),_get_zero_arr_size(_tmp2FB,49));}),_tmp2F9,loc);break;}
_tmp2F8=_tmp2F8->tl;}return;}_LL214:;}void*Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,void*f){void*_tmp2FC=f;struct Cyc_Dict_Dict*
_tmp2FD;struct Cyc_List_List*_tmp2FE;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2FF;
_LL21A: if((int)_tmp2FC != 0)goto _LL21C;_LL21B: return f;_LL21C: if(_tmp2FC <= (void*)
1)goto _LL219;if(*((int*)_tmp2FC)!= 0)goto _LL219;_tmp2FD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2FC)->f1;_tmp2FE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2FC)->f2;
_tmp2FF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2FC)->f3;_LL21D: _tmp2FF.may_consume=
0;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp300=_cycalloc(
sizeof(*_tmp300));_tmp300[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp301;
_tmp301.tag=0;_tmp301.f1=_tmp2FD;_tmp301.f2=_tmp2FE;_tmp301.f3=_tmp2FF;_tmp301;});
_tmp300;});_LL219:;}void*Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*
loc,void*f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{void*_tmp302=f;struct Cyc_Dict_Dict*
_tmp303;struct Cyc_List_List*_tmp304;struct Cyc_CfFlowInfo_ConsumeInfo _tmp305;
_LL21F: if((int)_tmp302 != 0)goto _LL221;_LL220: return f;_LL221: if(_tmp302 <= (void*)
1)goto _LL21E;if(*((int*)_tmp302)!= 0)goto _LL21E;_tmp303=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp302)->f1;_tmp304=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp302)->f2;
_tmp305=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp302)->f3;_LL222: _tmp305.may_consume=
0;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp306=_cycalloc(
sizeof(*_tmp306));_tmp306[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp307;
_tmp307.tag=0;_tmp307.f1=_tmp303;_tmp307.f2=_tmp304;_tmp307.f3=_tmp305;_tmp307;});
_tmp306;});_LL21E:;}}struct _tuple4 Cyc_CfFlowInfo_save_consume_info(void*f,int
clear){static struct Cyc_Dict_Dict**empty_info=0;void*_tmp308=f;struct Cyc_Dict_Dict*
_tmp309;struct Cyc_List_List*_tmp30A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30B;
_LL224: if((int)_tmp308 != 0)goto _LL226;_LL225: if(empty_info == 0)empty_info=({
struct Cyc_Dict_Dict**_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp30C;});return({struct _tuple4 _tmp30D;_tmp30D.f1=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp30E;_tmp30E.consumed=*empty_info;_tmp30E.may_consume=0;_tmp30E;});_tmp30D.f2=
f;_tmp30D;});_LL226: if(_tmp308 <= (void*)1)goto _LL223;if(*((int*)_tmp308)!= 0)
goto _LL223;_tmp309=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp308)->f1;
_tmp30A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp308)->f2;_tmp30B=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp308)->f3;_LL227: if(clear)return({struct
_tuple4 _tmp30F;_tmp30F.f1=_tmp30B;_tmp30F.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp311;_tmp311.tag=0;_tmp311.f1=_tmp309;_tmp311.f2=_tmp30A;_tmp311.f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp312;_tmp312.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp312.may_consume=0;_tmp312;});_tmp311;});_tmp310;});_tmp30F;});else{return({
struct _tuple4 _tmp313;_tmp313.f1=_tmp30B;_tmp313.f2=f;_tmp313;});}_LL223:;}void*
Cyc_CfFlowInfo_restore_consume_info(void*f,struct Cyc_CfFlowInfo_ConsumeInfo c){
void*_tmp314=f;struct Cyc_Dict_Dict*_tmp315;struct Cyc_List_List*_tmp316;_LL229:
if((int)_tmp314 != 0)goto _LL22B;_LL22A: return f;_LL22B: if(_tmp314 <= (void*)1)goto
_LL228;if(*((int*)_tmp314)!= 0)goto _LL228;_tmp315=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp314)->f1;_tmp316=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp314)->f2;
_LL22C: return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp317=_cycalloc(
sizeof(*_tmp317));_tmp317[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp318;
_tmp318.tag=0;_tmp318.f1=_tmp315;_tmp318.f2=_tmp316;_tmp318.f3=c;_tmp318;});
_tmp317;});_LL228:;}
