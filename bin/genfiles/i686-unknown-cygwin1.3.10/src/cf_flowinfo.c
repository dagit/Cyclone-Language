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
eff1,void*eff2);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
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
c);static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={0,(
void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
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
struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}static
void Cyc_CfFlowInfo_unique_err(struct Cyc_CfFlowInfo_Place*place,struct
_dynforward_ptr err_msg1,struct _dynforward_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc){struct Cyc_CfFlowInfo_Place _tmp8;
void*_tmp9;struct Cyc_CfFlowInfo_Place*_tmp7=place;_tmp8=*_tmp7;_tmp9=(void*)
_tmp8.root;{void*_tmpA=_tmp9;struct Cyc_Absyn_Vardecl*_tmpB;_LL1: if(*((int*)_tmpA)
!= 0)goto _LL3;_tmpB=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpA)->f1;_LL2: if(!
Cyc_Position_segment_equals(consumed_loc,loc)){struct _dynforward_ptr _tmpC=Cyc_Position_string_of_segment(
consumed_loc);({struct Cyc_String_pa_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(struct
_dynforward_ptr)_tmpC;{struct Cyc_String_pa_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmpB->name);{void*_tmpD[2]={&
_tmpE,& _tmpF};Cyc_Tcutil_terr(loc,err_msg1,_tag_dynforward(_tmpD,sizeof(void*),2));}}});}
else{({struct Cyc_String_pa_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(_tmpB->name);{void*_tmp10[1]={& _tmp11};
Cyc_Tcutil_terr(loc,err_msg2,_tag_dynforward(_tmp10,sizeof(void*),1));}});}goto
_LL0;_LL3:;_LL4:({void*_tmp12[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp13="error locations not for VarRoots";
_tag_dynforward(_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,33));}),
_tag_dynforward(_tmp12,sizeof(void*),0));});_LL0:;}}static struct Cyc_Position_Segment*
Cyc_CfFlowInfo_combine_consume_f(int isErr,struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2){if(isErr)Cyc_CfFlowInfo_unique_err(
place,({const char*_tmp14="May consume unique pointer %s more than once (cf. %s)";
_tag_dynforward(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,54));}),({const
char*_tmp15="May consume unique pointer %s more than once";_tag_dynforward(
_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,45));}),loc1,loc2);return loc1;}
struct Cyc_Dict_Dict*Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict*s1,struct
Cyc_Dict_Dict*s2,int disjoint){struct Cyc_Dict_Dict*_tmp16=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Position_Segment*(*f)(int,struct Cyc_CfFlowInfo_Place*,struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*),int env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_combine_consume_f,disjoint,s1,s2);
return _tmp16;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
struct _tuple5{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*f2;};int
Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict*s1,struct Cyc_Dict_Dict*s2){
if(s1 == s2)return 1;{struct _handler_cons _tmp17;_push_handler(& _tmp17);{int _tmp19=
0;if(setjmp(_tmp17.handler))_tmp19=1;if(!_tmp19){{struct _RegionHandle _tmp1A=
_new_region("r");struct _RegionHandle*r=& _tmp1A;_push_region(r);{struct Cyc_Iter_Iter
_tmp1B=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))
Cyc_Dict_make_iter)(r,s1);struct _tuple5 _tmp1C=*((struct _tuple5*(*)(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(r,s1);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple5*))Cyc_Iter_next)(_tmp1B,& _tmp1C)){struct Cyc_CfFlowInfo_Place*
_tmp1D=_tmp1C.f1;if(!((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_member)(s2,_tmp1D)){int _tmp1E=0;_npop_handler(1);return _tmp1E;}}{int
_tmp1F=1;_npop_handler(1);return _tmp1F;}};_pop_region(r);};_pop_handler();}else{
void*_tmp18=(void*)_exn_thrown;void*_tmp21=_tmp18;_LL6:;_LL7: return 1;_LL8:;_LL9:(
void)_throw(_tmp21);_LL5:;}}}}int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict*
s1,struct Cyc_Dict_Dict*s2){return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{
struct _tuple0 _tmp23=({struct _tuple0 _tmp22;_tmp22.f1=r1;_tmp22.f2=r2;_tmp22;});
void*_tmp24;struct Cyc_Absyn_Vardecl*_tmp25;void*_tmp26;struct Cyc_Absyn_Vardecl*
_tmp27;void*_tmp28;void*_tmp29;void*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;void*
_tmp2C;struct Cyc_Absyn_Exp*_tmp2D;void*_tmp2E;void*_tmp2F;void*_tmp30;int _tmp31;
void*_tmp32;int _tmp33;_LLB: _tmp24=_tmp23.f1;if(*((int*)_tmp24)!= 0)goto _LLD;
_tmp25=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp24)->f1;_tmp26=_tmp23.f2;if(*((
int*)_tmp26)!= 0)goto _LLD;_tmp27=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp26)->f1;
_LLC: return(int)_tmp25 - (int)_tmp27;_LLD: _tmp28=_tmp23.f1;if(*((int*)_tmp28)!= 0)
goto _LLF;_LLE: return - 1;_LLF: _tmp29=_tmp23.f2;if(*((int*)_tmp29)!= 0)goto _LL11;
_LL10: return 1;_LL11: _tmp2A=_tmp23.f1;if(*((int*)_tmp2A)!= 1)goto _LL13;_tmp2B=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2A)->f1;_tmp2C=_tmp23.f2;if(*((int*)
_tmp2C)!= 1)goto _LL13;_tmp2D=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2C)->f1;
_LL12: return(int)_tmp2B - (int)_tmp2D;_LL13: _tmp2E=_tmp23.f1;if(*((int*)_tmp2E)!= 
1)goto _LL15;_LL14: return - 1;_LL15: _tmp2F=_tmp23.f2;if(*((int*)_tmp2F)!= 1)goto
_LL17;_LL16: return 1;_LL17: _tmp30=_tmp23.f1;if(*((int*)_tmp30)!= 2)goto _LLA;
_tmp31=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp30)->f1;_tmp32=_tmp23.f2;if(*((
int*)_tmp32)!= 2)goto _LLA;_tmp33=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp32)->f1;
_LL18: return _tmp31 - _tmp33;_LLA:;}}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{int i=Cyc_CfFlowInfo_root_cmp((
void*)p1->root,(void*)p2->root);if(i != 0)return i;return((int(*)(int(*cmp)(struct
_dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,p1->fields,p2->fields);}}static struct
_dynforward_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){struct
Cyc_List_List*sl=0;{void*_tmp34=(void*)p->root;struct Cyc_Absyn_Vardecl*_tmp35;
struct Cyc_Absyn_Exp*_tmp36;int _tmp37;_LL1A: if(*((int*)_tmp34)!= 0)goto _LL1C;
_tmp35=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp34)->f1;_LL1B: sl=({struct Cyc_List_List*
_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->hd=({struct _dynforward_ptr*_tmp39=
_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_String_pa_struct _tmp3C;_tmp3C.tag=
0;_tmp3C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*_tmp35->name).f2);{
void*_tmp3A[1]={& _tmp3C};Cyc_aprintf(({const char*_tmp3B="%s";_tag_dynforward(
_tmp3B,sizeof(char),_get_zero_arr_size(_tmp3B,3));}),_tag_dynforward(_tmp3A,
sizeof(void*),1));}});_tmp39;});_tmp38->tl=sl;_tmp38;});goto _LL19;_LL1C: if(*((
int*)_tmp34)!= 1)goto _LL1E;_tmp36=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp34)->f1;
_LL1D: sl=({struct Cyc_List_List*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->hd=({
struct _dynforward_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({struct Cyc_Int_pa_struct
_tmp41;_tmp41.tag=1;_tmp41.f1=(unsigned long)((int)_tmp36);{void*_tmp3F[1]={&
_tmp41};Cyc_aprintf(({const char*_tmp40="mpt%d";_tag_dynforward(_tmp40,sizeof(
char),_get_zero_arr_size(_tmp40,6));}),_tag_dynforward(_tmp3F,sizeof(void*),1));}});
_tmp3E;});_tmp3D->tl=sl;_tmp3D;});goto _LL19;_LL1E: if(*((int*)_tmp34)!= 2)goto
_LL19;_tmp37=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp34)->f1;_LL1F: sl=({
struct Cyc_List_List*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->hd=({struct
_dynforward_ptr*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({struct Cyc_Int_pa_struct
_tmp46;_tmp46.tag=1;_tmp46.f1=(unsigned long)_tmp37;{void*_tmp44[1]={& _tmp46};
Cyc_aprintf(({const char*_tmp45="param%d";_tag_dynforward(_tmp45,sizeof(char),
_get_zero_arr_size(_tmp45,8));}),_tag_dynforward(_tmp44,sizeof(void*),1));}});
_tmp43;});_tmp42->tl=sl;_tmp42;});goto _LL19;_LL19:;}{struct Cyc_List_List*fields=
p->fields;for(0;fields != 0;fields=fields->tl){sl=({struct Cyc_List_List*_tmp47=
_cycalloc(sizeof(*_tmp47));_tmp47->hd=({struct _dynforward_ptr*_tmp48=_cycalloc(
sizeof(*_tmp48));_tmp48[0]=({struct Cyc_String_pa_struct _tmp4B;_tmp4B.tag=0;
_tmp4B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)fields->hd));{void*_tmp49[1]={& _tmp4B};Cyc_aprintf(({const char*
_tmp4A="%s";_tag_dynforward(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,3));}),
_tag_dynforward(_tmp49,sizeof(void*),1));}});_tmp48;});_tmp47->tl=sl;_tmp47;});}}{
struct _dynforward_ptr*_tmp4C=({struct _dynforward_ptr*_tmp51=_cycalloc(sizeof(*
_tmp51));_tmp51[0]=({struct Cyc_String_pa_struct _tmp54;_tmp54.tag=0;_tmp54.f1=(
struct _dynforward_ptr)({const char*_tmp55="";_tag_dynforward(_tmp55,sizeof(char),
_get_zero_arr_size(_tmp55,1));});{void*_tmp52[1]={& _tmp54};Cyc_aprintf(({const
char*_tmp53="%s";_tag_dynforward(_tmp53,sizeof(char),_get_zero_arr_size(_tmp53,3));}),
_tag_dynforward(_tmp52,sizeof(void*),1));}});_tmp51;});for(0;sl != 0;sl=sl->tl){*
_tmp4C=({struct Cyc_String_pa_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp4C);{struct Cyc_String_pa_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)sl->hd));{void*_tmp4D[2]={& _tmp4F,& _tmp50};Cyc_aprintf(({
const char*_tmp4E="%s.%s";_tag_dynforward(_tmp4E,sizeof(char),_get_zero_arr_size(
_tmp4E,6));}),_tag_dynforward(_tmp4D,sizeof(void*),2));}}});}return _tmp4C;}}
struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
int allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_union_type(t))return Cyc_CfFlowInfo_unknown_all;{
void*_tmp56=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo _tmp57;void*
_tmp58;struct Cyc_Absyn_Tunionfield*_tmp59;struct Cyc_List_List*_tmp5A;struct Cyc_Absyn_AggrInfo
_tmp5B;void*_tmp5C;void*_tmp5D;struct Cyc_List_List*_tmp5E;struct Cyc_Absyn_ArrayInfo
_tmp5F;void*_tmp60;struct Cyc_Absyn_Conref*_tmp61;void*_tmp62;struct Cyc_Absyn_PtrInfo
_tmp63;struct Cyc_Absyn_PtrAtts _tmp64;struct Cyc_Absyn_Conref*_tmp65;_LL21: if(
_tmp56 <= (void*)4)goto _LL2F;if(*((int*)_tmp56)!= 3)goto _LL23;_tmp57=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp56)->f1;_tmp58=(void*)_tmp57.field_info;if(*((int*)_tmp58)!= 1)goto _LL23;
_tmp59=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp58)->f2;_LL22: if(_tmp59->typs
== 0)return leafval;_tmp5A=_tmp59->typs;goto _LL24;_LL23: if(*((int*)_tmp56)!= 9)
goto _LL25;_tmp5A=((struct Cyc_Absyn_TupleType_struct*)_tmp56)->f1;_LL24: {struct
Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{int i=0;for(0;_tmp5A != 0;(
_tmp5A=_tmp5A->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_i_typ_to_absrval(
0,(*((struct _tuple6*)_tmp5A->hd)).f2,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp67;_tmp67.tag=4;_tmp67.f1=d;_tmp67;});_tmp66;});}_LL25: if(*((int*)_tmp56)!= 
10)goto _LL27;_tmp5B=((struct Cyc_Absyn_AggrType_struct*)_tmp56)->f1;_tmp5C=(void*)
_tmp5B.aggr_info;_LL26: {struct Cyc_Absyn_Aggrdecl*_tmp68=Cyc_Absyn_get_known_aggrdecl(
_tmp5C);if(_tmp68->impl == 0)goto _LL20;_tmp5E=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp68->impl))->fields;goto _LL28;}_LL27: if(*((int*)_tmp56)!= 11)goto
_LL29;_tmp5D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp56)->f1;if((int)
_tmp5D != 0)goto _LL29;_tmp5E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp56)->f2;
_LL28: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);for(0;
_tmp5E != 0;_tmp5E=_tmp5E->tl){struct Cyc_Absyn_Aggrfield _tmp6A;struct
_dynforward_ptr*_tmp6B;void*_tmp6C;struct Cyc_Absyn_Aggrfield*_tmp69=(struct Cyc_Absyn_Aggrfield*)
_tmp5E->hd;_tmp6A=*_tmp69;_tmp6B=_tmp6A.name;_tmp6C=(void*)_tmp6A.type;if(
_get_dynforward_size(*_tmp6B,sizeof(char))!= 1)d=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(d,_tmp6B,
Cyc_CfFlowInfo_i_typ_to_absrval(0,_tmp6C,leafval));}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp6E;_tmp6E.tag=4;_tmp6E.f1=d;_tmp6E;});_tmp6D;});}_LL29: if(*((int*)_tmp56)!= 
7)goto _LL2B;_tmp5F=((struct Cyc_Absyn_ArrayType_struct*)_tmp56)->f1;_tmp60=(void*)
_tmp5F.elt_type;_tmp61=_tmp5F.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp61))goto _LL2B;_LL2A: return allow_zeroterm  && Cyc_Tcutil_bits_only(
_tmp60)?Cyc_CfFlowInfo_unknown_all: leafval;_LL2B: if(*((int*)_tmp56)!= 17)goto
_LL2D;_tmp62=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp56)->f1;_LL2C: {void*
_tmp6F=leafval;void*_tmp70;void*_tmp71;_LL32: if(_tmp6F <= (void*)3)goto _LL36;if(*((
int*)_tmp6F)!= 0)goto _LL34;_tmp70=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp6F)->f1;if((int)_tmp70 != 2)goto _LL34;_LL33: goto _LL35;_LL34: if(*((int*)_tmp6F)
!= 1)goto _LL36;_tmp71=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp6F)->f1;if((
int)_tmp71 != 2)goto _LL36;_LL35: goto _LL37;_LL36: if((int)_tmp6F != 0)goto _LL38;
_LL37: goto _LL39;_LL38: if((int)_tmp6F != 1)goto _LL3A;_LL39: return(void*)({struct
Cyc_CfFlowInfo_TagCmps_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({
struct Cyc_CfFlowInfo_TagCmps_struct _tmp73;_tmp73.tag=3;_tmp73.f1=({struct Cyc_CfFlowInfo_TagCmp*
_tmp74[1];_tmp74[0]=({struct Cyc_CfFlowInfo_TagCmp*_tmp75=_cycalloc(sizeof(*
_tmp75));_tmp75->cmp=(void*)((void*)5);_tmp75->bd=(void*)_tmp62;_tmp75;});((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp74,sizeof(struct Cyc_CfFlowInfo_TagCmp*),1));});_tmp73;});_tmp72;});_LL3A:;
_LL3B: return leafval;_LL31:;}_LL2D: if(*((int*)_tmp56)!= 4)goto _LL2F;_tmp63=((
struct Cyc_Absyn_PointerType_struct*)_tmp56)->f1;_tmp64=_tmp63.ptr_atts;_tmp65=
_tmp64.nullable;if(!(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp65)))goto _LL2F;_LL2E:{void*_tmp76=leafval;void*_tmp77;void*_tmp78;_LL3D:
if(_tmp76 <= (void*)3)goto _LL41;if(*((int*)_tmp76)!= 0)goto _LL3F;_tmp77=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp76)->f1;if((int)_tmp77 != 1)goto _LL3F;
_LL3E: return(void*)2;_LL3F: if(*((int*)_tmp76)!= 0)goto _LL41;_tmp78=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp76)->f1;if((int)_tmp78 != 2)goto _LL41;
_LL40: return(void*)1;_LL41:;_LL42: goto _LL3C;_LL3C:;}goto _LL20;_LL2F:;_LL30: goto
_LL20;_LL20:;}return Cyc_Tcutil_bits_only(t)?Cyc_CfFlowInfo_unknown_all: leafval;}
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
1,t,leafval);}static int Cyc_CfFlowInfo_prefix_of_member(struct _RegionHandle*r,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict*set){struct _tuple5 _tmp79=({
struct _tuple5 _tmp7E;_tmp7E.f1=place;_tmp7E.f2=0;_tmp7E;});struct Cyc_Iter_Iter
_tmp7A=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))
Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple5*))
Cyc_Iter_next)(_tmp7A,& _tmp79)){struct Cyc_CfFlowInfo_Place*_tmp7B=_tmp79.f1;if(
Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp7B->root)!= 0)continue;{
struct Cyc_List_List*_tmp7C=place->fields;struct Cyc_List_List*_tmp7D=_tmp7B->fields;
for(0;_tmp7C != 0  && _tmp7D != 0;(_tmp7C=_tmp7C->tl,_tmp7D=_tmp7D->tl)){if(Cyc_strptrcmp((
struct _dynforward_ptr*)_tmp7C->hd,(struct _dynforward_ptr*)_tmp7D->hd)!= 0)break;}
if(_tmp7C == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{struct
_RegionHandle*rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,
place))pile->places=({struct Cyc_List_List*_tmp7F=_region_malloc(pile->rgn,
sizeof(*_tmp7F));_tmp7F->hd=place;_tmp7F->tl=pile->places;_tmp7F;});}static void
Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*a,void*r){void*
_tmp80=r;struct Cyc_CfFlowInfo_Place*_tmp81;struct Cyc_Dict_Dict*_tmp82;_LL44: if(
_tmp80 <= (void*)3)goto _LL48;if(*((int*)_tmp80)!= 2)goto _LL46;_tmp81=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp80)->f1;_LL45: Cyc_CfFlowInfo_add_place(pile,_tmp81);return;_LL46: if(*((int*)
_tmp80)!= 4)goto _LL48;_tmp82=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp80)->f1;
_LL47:((void(*)(void(*f)(struct Cyc_CfFlowInfo_EscPile*,struct _dynforward_ptr*,
void*),struct Cyc_CfFlowInfo_EscPile*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,struct _dynforward_ptr*a,void*r))Cyc_CfFlowInfo_add_places,
pile,_tmp82);return;_LL48:;_LL49: return;_LL43:;}static void*Cyc_CfFlowInfo_insert_place_inner(
void*new_val,void*old_val){void*_tmp83=old_val;struct Cyc_Dict_Dict*_tmp84;_LL4B:
if(_tmp83 <= (void*)3)goto _LL4D;if(*((int*)_tmp83)!= 4)goto _LL4D;_tmp84=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmp83)->f1;_LL4C: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp86;_tmp86.tag=4;_tmp86.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp84);_tmp86;});_tmp85;});_LL4D:;_LL4E: return new_val;_LL4A:;}struct
_tuple7{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple7 _tmp88=({struct _tuple7 _tmp87;_tmp87.f1=fs;_tmp87.f2=
old_val;_tmp87;});struct Cyc_List_List*_tmp89;struct Cyc_List_List _tmp8A;struct
_dynforward_ptr*_tmp8B;struct Cyc_List_List*_tmp8C;void*_tmp8D;struct Cyc_Dict_Dict*
_tmp8E;_LL50: _tmp89=_tmp88.f1;if(_tmp89 == 0)goto _LL52;_tmp8A=*_tmp89;_tmp8B=(
struct _dynforward_ptr*)_tmp8A.hd;_tmp8C=_tmp8A.tl;_tmp8D=_tmp88.f2;if(_tmp8D <= (
void*)3)goto _LL52;if(*((int*)_tmp8D)!= 4)goto _LL52;_tmp8E=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp8D)->f1;_LL51: {void*_tmp8F=Cyc_CfFlowInfo_insert_place_outer(_tmp8C,((void*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp8E,_tmp8B),
new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp90=_cycalloc(
sizeof(*_tmp90));_tmp90[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp91;_tmp91.tag=
4;_tmp91.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k,void*v))Cyc_Dict_insert)(_tmp8E,_tmp8B,_tmp8F);_tmp91;});
_tmp90;});}_LL52:;_LL53:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Core_Impossible_struct
_tmp93;_tmp93.tag=Cyc_Core_Impossible;_tmp93.f1=({const char*_tmp94="bad insert place";
_tag_dynforward(_tmp94,sizeof(char),_get_zero_arr_size(_tmp94,17));});_tmp93;});
_tmp92;}));_LL4F:;}}static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_these(struct
Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict**all_changed,struct Cyc_Dict_Dict*
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp95=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;Cyc_CfFlowInfo_update_place_set(all_changed,
_tmp95,0);{void*oldval;void*newval;{struct _handler_cons _tmp96;_push_handler(&
_tmp96);{int _tmp98=0;if(setjmp(_tmp96.handler))_tmp98=1;if(!_tmp98){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp95);;_pop_handler();}else{void*_tmp97=(void*)_exn_thrown;void*_tmp9A=
_tmp97;_LL55: if(_tmp9A != Cyc_Dict_Absent)goto _LL57;_LL56: continue;_LL57:;_LL58:(
void)_throw(_tmp9A);_LL54:;}}}{void*_tmp9B=Cyc_CfFlowInfo_initlevel(d,oldval);
_LL5A: if((int)_tmp9B != 2)goto _LL5C;_LL5B: newval=Cyc_CfFlowInfo_esc_all;goto _LL59;
_LL5C: if((int)_tmp9B != 1)goto _LL5E;_LL5D: newval=Cyc_CfFlowInfo_esc_this;goto
_LL59;_LL5E: if((int)_tmp9B != 0)goto _LL59;_LL5F: newval=Cyc_CfFlowInfo_esc_none;
goto _LL59;_LL59:;}((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,oldval);d=Cyc_Dict_insert(d,(void*)_tmp95->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp95->fields,Cyc_Dict_lookup(d,(void*)_tmp95->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp9C=r;void*_tmp9D;
void*_tmp9E;_LL61: if(_tmp9C <= (void*)3)goto _LL65;if(*((int*)_tmp9C)!= 0)goto
_LL63;_tmp9D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp9C)->f1;_LL62:
return _tmp9D;_LL63: if(*((int*)_tmp9C)!= 1)goto _LL65;_tmp9E=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp9C)->f1;_LL64: return _tmp9E;_LL65: if((int)_tmp9C != 0)goto _LL67;_LL66: goto
_LL68;_LL67: if((int)_tmp9C != 1)goto _LL69;_LL68: return(void*)2;_LL69: if((int)
_tmp9C != 2)goto _LL6B;_LL6A: return(void*)1;_LL6B: if(_tmp9C <= (void*)3)goto _LL6D;
if(*((int*)_tmp9C)!= 3)goto _LL6D;_LL6C: return(void*)2;_LL6D:;_LL6E:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));
_tmp9F[0]=({struct Cyc_Core_Impossible_struct _tmpA0;_tmpA0.tag=Cyc_Core_Impossible;
_tmpA0.f1=({const char*_tmpA1="initlevel_approx";_tag_dynforward(_tmpA1,sizeof(
char),_get_zero_arr_size(_tmpA1,17));});_tmpA0;});_tmp9F;}));_LL60:;}static void*
Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*a,void*r,
void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*_tmpA2=r;struct
Cyc_Dict_Dict*_tmpA3;struct Cyc_CfFlowInfo_Place*_tmpA4;_LL70: if(_tmpA2 <= (void*)
3)goto _LL74;if(*((int*)_tmpA2)!= 4)goto _LL72;_tmpA3=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpA2)->f1;_LL71: this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,
struct _dynforward_ptr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct
Cyc_Dict_Dict*d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,struct _dynforward_ptr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,
_tmpA3,(void*)2);goto _LL6F;_LL72: if(*((int*)_tmpA2)!= 2)goto _LL74;_tmpA4=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpA2)->f1;_LL73: if(((int(*)(int(*
compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmpA4))this_ans=(void*)2;else{env->seen=({struct Cyc_List_List*_tmpA5=_cycalloc(
sizeof(*_tmpA5));_tmpA5->hd=_tmpA4;_tmpA5->tl=env->seen;_tmpA5;});this_ans=((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(
env,0,Cyc_CfFlowInfo_lookup_place(env->d,_tmpA4),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL6F;
_LL74:;_LL75: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL6F:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1  || acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmpA6=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmpA7;_tmpA7.d=d;_tmpA7.seen=0;_tmpA7;});return((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(& _tmpA6,0,r,(void*)2);}
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){struct Cyc_CfFlowInfo_Place _tmpA9;void*_tmpAA;struct Cyc_List_List*_tmpAB;
struct Cyc_CfFlowInfo_Place*_tmpA8=place;_tmpA9=*_tmpA8;_tmpAA=(void*)_tmpA9.root;
_tmpAB=_tmpA9.fields;{void*_tmpAC=Cyc_Dict_lookup(d,_tmpAA);for(0;_tmpAB != 0;
_tmpAB=_tmpAB->tl){struct _tuple1 _tmpAE=({struct _tuple1 _tmpAD;_tmpAD.f1=_tmpAC;
_tmpAD.f2=(struct _dynforward_ptr*)_tmpAB->hd;_tmpAD;});void*_tmpAF;struct Cyc_Dict_Dict*
_tmpB0;struct _dynforward_ptr*_tmpB1;_LL77: _tmpAF=_tmpAE.f1;if(_tmpAF <= (void*)3)
goto _LL79;if(*((int*)_tmpAF)!= 4)goto _LL79;_tmpB0=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpAF)->f1;_tmpB1=_tmpAE.f2;_LL78: _tmpAC=((void*(*)(struct Cyc_Dict_Dict*d,
struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmpB0,_tmpB1);goto _LL76;_LL79:;_LL7A:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpB2=_cycalloc(sizeof(*
_tmpB2));_tmpB2[0]=({struct Cyc_Core_Impossible_struct _tmpB3;_tmpB3.tag=Cyc_Core_Impossible;
_tmpB3.f1=({const char*_tmpB4="bad lookup_place";_tag_dynforward(_tmpB4,sizeof(
char),_get_zero_arr_size(_tmpB4,17));});_tmpB3;});_tmpB2;}));_LL76:;}return
_tmpAC;}}static int Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,void*rval){void*
_tmpB5=rval;struct Cyc_Dict_Dict*_tmpB6;_LL7C: if(_tmpB5 <= (void*)3)goto _LL80;if(*((
int*)_tmpB5)!= 1)goto _LL7E;_LL7D: return 0;_LL7E: if(*((int*)_tmpB5)!= 4)goto _LL80;
_tmpB6=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpB5)->f1;_LL7F: return((int(*)(
int(*f)(int,struct _dynforward_ptr*,void*),int env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((
int(*)(int a,struct _dynforward_ptr*b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped,
0,_tmpB6);_LL80:;_LL81: return 1;_LL7B:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(
0,0,Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict**all_changed,void*r){struct
_RegionHandle _tmpB7=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB7;
_push_region(rgn);{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*
_tmpB9=_region_malloc(rgn,sizeof(*_tmpB9));_tmpB9->rgn=rgn;_tmpB9->places=0;
_tmpB9;});((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmpB8=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmpB8;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*a,void*oldval,void*newval){struct _tuple0 _tmpBB=({struct _tuple0 _tmpBA;
_tmpBA.f1=oldval;_tmpBA.f2=newval;_tmpBA;});void*_tmpBC;void*_tmpBD;struct Cyc_CfFlowInfo_Place*
_tmpBE;void*_tmpBF;void*_tmpC0;struct Cyc_Dict_Dict*_tmpC1;void*_tmpC2;struct Cyc_Dict_Dict*
_tmpC3;void*_tmpC4;void*_tmpC5;_LL83: _tmpBC=_tmpBB.f1;if(_tmpBC <= (void*)3)goto
_LL85;if(*((int*)_tmpBC)!= 1)goto _LL85;_tmpBD=_tmpBB.f2;if(_tmpBD <= (void*)3)
goto _LL85;if(*((int*)_tmpBD)!= 2)goto _LL85;_tmpBE=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpBD)->f1;_LL84: Cyc_CfFlowInfo_add_place(env->pile,_tmpBE);goto _LL86;_LL85:
_tmpBF=_tmpBB.f1;if(_tmpBF <= (void*)3)goto _LL87;if(*((int*)_tmpBF)!= 1)goto _LL87;
_LL86: if(Cyc_CfFlowInfo_initlevel(env->d,newval)!= (void*)2)({void*_tmpC6[0]={};
Cyc_Tcutil_terr(env->loc,({const char*_tmpC7="assignment puts possibly-uninitialized data in an escaped location";
_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size(_tmpC7,67));}),
_tag_dynforward(_tmpC6,sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;_LL87:
_tmpC0=_tmpBB.f1;if(_tmpC0 <= (void*)3)goto _LL89;if(*((int*)_tmpC0)!= 4)goto _LL89;
_tmpC1=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpC0)->f1;_tmpC2=_tmpBB.f2;if(
_tmpC2 <= (void*)3)goto _LL89;if(*((int*)_tmpC2)!= 4)goto _LL89;_tmpC3=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC2)->f1;_LL88: {struct Cyc_Dict_Dict*_tmpC8=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_AssignEnv*,struct _dynforward_ptr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,struct _dynforward_ptr*a,void*oldval,void*
newval))Cyc_CfFlowInfo_assign_place_inner,env,_tmpC1,_tmpC3);if(_tmpC8 == _tmpC1)
return oldval;if(_tmpC8 == _tmpC3)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpCA;_tmpCA.tag=4;_tmpCA.f1=_tmpC8;_tmpCA;});_tmpC9;});}_LL89: _tmpC4=_tmpBB.f2;
if(_tmpC4 <= (void*)3)goto _LL8B;if(*((int*)_tmpC4)!= 1)goto _LL8B;_tmpC5=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmpC4)->f1;_LL8A: {void*_tmpCB=_tmpC5;_LL8E:
if((int)_tmpCB != 0)goto _LL90;_LL8F: return Cyc_CfFlowInfo_unknown_none;_LL90: if((
int)_tmpCB != 1)goto _LL92;_LL91: return Cyc_CfFlowInfo_unknown_this;_LL92: if((int)
_tmpCB != 2)goto _LL8D;_LL93: return Cyc_CfFlowInfo_unknown_all;_LL8D:;}_LL8B:;_LL8C:
return newval;_LL82:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);{struct _tuple7 _tmpCD=({struct _tuple7 _tmpCC;_tmpCC.f1=fs;
_tmpCC.f2=oldval;_tmpCC;});struct Cyc_List_List*_tmpCE;struct Cyc_List_List _tmpCF;
struct _dynforward_ptr*_tmpD0;struct Cyc_List_List*_tmpD1;void*_tmpD2;struct Cyc_Dict_Dict*
_tmpD3;_LL95: _tmpCE=_tmpCD.f1;if(_tmpCE == 0)goto _LL97;_tmpCF=*_tmpCE;_tmpD0=(
struct _dynforward_ptr*)_tmpCF.hd;_tmpD1=_tmpCF.tl;_tmpD2=_tmpCD.f2;if(_tmpD2 <= (
void*)3)goto _LL97;if(*((int*)_tmpD2)!= 4)goto _LL97;_tmpD3=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD2)->f1;_LL96: {void*_tmpD4=Cyc_CfFlowInfo_assign_place_outer(env,_tmpD1,((
void*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmpD3,
_tmpD0),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpD5=
_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpD6;_tmpD6.tag=4;_tmpD6.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(_tmpD3,_tmpD0,_tmpD4);_tmpD6;});
_tmpD5;});}_LL97:;_LL98:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Core_Impossible_struct
_tmpD8;_tmpD8.tag=Cyc_Core_Impossible;_tmpD8.f1=({const char*_tmpD9="bad assign place";
_tag_dynforward(_tmpD9,sizeof(char),_get_zero_arr_size(_tmpD9,17));});_tmpD8;});
_tmpD7;}));_LL94:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r){Cyc_CfFlowInfo_update_place_set(all_changed,place,0);{struct
_RegionHandle _tmpDA=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDA;
_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmpDC;void*_tmpDD;struct Cyc_List_List*
_tmpDE;struct Cyc_CfFlowInfo_Place*_tmpDB=place;_tmpDC=*_tmpDB;_tmpDD=(void*)
_tmpDC.root;_tmpDE=_tmpDC.fields;{struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv
_tmpE0;_tmpE0.pile=({struct Cyc_CfFlowInfo_EscPile*_tmpE1=_region_malloc(rgn,
sizeof(*_tmpE1));_tmpE1->rgn=rgn;_tmpE1->places=0;_tmpE1;});_tmpE0.d=d;_tmpE0.loc=
loc;_tmpE0;});void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmpDE,Cyc_Dict_lookup(
d,_tmpDD),r);struct Cyc_Dict_Dict*_tmpDF=Cyc_CfFlowInfo_escape_these(env.pile,
all_changed,Cyc_Dict_insert(d,_tmpDD,newval));_npop_handler(0);return _tmpDF;}};
_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict*
chg1;struct Cyc_Dict_Dict*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*
_tmpE2=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmpE4;void*_tmpE5;
void*_tmpE6;struct Cyc_CfFlowInfo_TagCmp*_tmpE3=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;
_tmpE4=*_tmpE3;_tmpE5=(void*)_tmpE4.cmp;_tmpE6=(void*)_tmpE4.bd;{int found=0;void*
joined_cmp=(void*)10;{struct Cyc_List_List*_tmpE7=l1;for(0;_tmpE7 != 0;_tmpE7=
_tmpE7->tl){struct Cyc_CfFlowInfo_TagCmp _tmpE9;void*_tmpEA;void*_tmpEB;struct Cyc_CfFlowInfo_TagCmp*
_tmpE8=(struct Cyc_CfFlowInfo_TagCmp*)_tmpE7->hd;_tmpE9=*_tmpE8;_tmpEA=(void*)
_tmpE9.cmp;_tmpEB=(void*)_tmpE9.bd;if(Cyc_Tcutil_typecmp(_tmpE6,_tmpEB)== 0){
found=1;if(_tmpEA == _tmpE5){joined_cmp=_tmpEA;break;}}}}if(found)_tmpE2=({struct
Cyc_List_List*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->hd=({struct Cyc_CfFlowInfo_TagCmp*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->cmp=(void*)joined_cmp;_tmpED->bd=(void*)
_tmpE6;_tmpED;});_tmpEC->tl=_tmpE2;_tmpEC;});}}return _tmpE2;}}static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{
struct _tuple0 _tmpEF=({struct _tuple0 _tmpEE;_tmpEE.f1=r1;_tmpEE.f2=r2;_tmpEE;});
void*_tmpF0;struct Cyc_CfFlowInfo_Place*_tmpF1;void*_tmpF2;struct Cyc_CfFlowInfo_Place*
_tmpF3;void*_tmpF4;struct Cyc_CfFlowInfo_Place*_tmpF5;void*_tmpF6;struct Cyc_CfFlowInfo_Place*
_tmpF7;void*_tmpF8;void*_tmpF9;void*_tmpFA;void*_tmpFB;void*_tmpFC;struct Cyc_Dict_Dict*
_tmpFD;void*_tmpFE;struct Cyc_Dict_Dict*_tmpFF;void*_tmp100;struct Cyc_List_List*
_tmp101;void*_tmp102;struct Cyc_List_List*_tmp103;void*_tmp104;void*_tmp105;_LL9A:
_tmpF0=_tmpEF.f1;if(_tmpF0 <= (void*)3)goto _LL9C;if(*((int*)_tmpF0)!= 2)goto _LL9C;
_tmpF1=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpF0)->f1;_tmpF2=_tmpEF.f2;if(
_tmpF2 <= (void*)3)goto _LL9C;if(*((int*)_tmpF2)!= 2)goto _LL9C;_tmpF3=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpF2)->f1;_LL9B: if(Cyc_CfFlowInfo_place_cmp(_tmpF1,_tmpF3)== 0)return r1;Cyc_CfFlowInfo_add_place(
env->pile,_tmpF1);Cyc_CfFlowInfo_add_place(env->pile,_tmpF3);goto _LL99;_LL9C:
_tmpF4=_tmpEF.f1;if(_tmpF4 <= (void*)3)goto _LL9E;if(*((int*)_tmpF4)!= 2)goto _LL9E;
_tmpF5=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpF4)->f1;_LL9D: Cyc_CfFlowInfo_add_place(
env->pile,_tmpF5);goto _LL99;_LL9E: _tmpF6=_tmpEF.f2;if(_tmpF6 <= (void*)3)goto
_LLA0;if(*((int*)_tmpF6)!= 2)goto _LLA0;_tmpF7=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpF6)->f1;_LL9F: Cyc_CfFlowInfo_add_place(env->pile,_tmpF7);goto _LL99;_LLA0:
_tmpF8=_tmpEF.f1;if((int)_tmpF8 != 1)goto _LLA2;_tmpF9=_tmpEF.f2;if((int)_tmpF9 != 
2)goto _LLA2;_LLA1: goto _LLA3;_LLA2: _tmpFA=_tmpEF.f1;if((int)_tmpFA != 2)goto _LLA4;
_tmpFB=_tmpEF.f2;if((int)_tmpFB != 1)goto _LLA4;_LLA3: return(void*)2;_LLA4: _tmpFC=
_tmpEF.f1;if(_tmpFC <= (void*)3)goto _LLA6;if(*((int*)_tmpFC)!= 4)goto _LLA6;_tmpFD=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpFC)->f1;_tmpFE=_tmpEF.f2;if(_tmpFE <= (
void*)3)goto _LLA6;if(*((int*)_tmpFE)!= 4)goto _LLA6;_tmpFF=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpFE)->f1;_LLA5: {struct Cyc_Dict_Dict*_tmp106=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_JoinEnv*,struct _dynforward_ptr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,struct _dynforward_ptr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,
env,_tmpFD,_tmpFF);if(_tmp106 == _tmpFD)return r1;if(_tmp106 == _tmpFF)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp107=_cycalloc(sizeof(*
_tmp107));_tmp107[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp108;_tmp108.tag=
4;_tmp108.f1=_tmp106;_tmp108;});_tmp107;});}_LLA6: _tmp100=_tmpEF.f1;if(_tmp100 <= (
void*)3)goto _LLA8;if(*((int*)_tmp100)!= 3)goto _LLA8;_tmp101=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp100)->f1;_tmp102=_tmpEF.f2;if(_tmp102 <= (void*)3)goto _LLA8;if(*((int*)
_tmp102)!= 3)goto _LLA8;_tmp103=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp102)->f1;
_LLA7: {struct Cyc_List_List*_tmp109=Cyc_CfFlowInfo_join_tag_cmps(_tmp101,_tmp103);
if(_tmp109 == _tmp101)return r1;return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp10B;_tmp10B.tag=3;_tmp10B.f1=_tmp109;_tmp10B;});_tmp10A;});}_LLA8: _tmp104=
_tmpEF.f1;if(_tmp104 <= (void*)3)goto _LLAA;if(*((int*)_tmp104)!= 3)goto _LLAA;
_LLA9: return r2;_LLAA: _tmp105=_tmpEF.f2;if(_tmp105 <= (void*)3)goto _LLAC;if(*((int*)
_tmp105)!= 3)goto _LLAC;_LLAB: return r1;_LLAC:;_LLAD: goto _LL99;_LL99:;}{void*il1=
Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,
r2);struct _tuple0 _tmp10D=({struct _tuple0 _tmp10C;_tmp10C.f1=r1;_tmp10C.f2=r2;
_tmp10C;});void*_tmp10E;void*_tmp10F;_LLAF: _tmp10E=_tmp10D.f1;if(_tmp10E <= (void*)
3)goto _LLB1;if(*((int*)_tmp10E)!= 1)goto _LLB1;_LLB0: goto _LLB2;_LLB1: _tmp10F=
_tmp10D.f2;if(_tmp10F <= (void*)3)goto _LLB3;if(*((int*)_tmp10F)!= 1)goto _LLB3;
_LLB2: {struct _tuple0 _tmp111=({struct _tuple0 _tmp110;_tmp110.f1=il1;_tmp110.f2=
il2;_tmp110;});void*_tmp112;void*_tmp113;void*_tmp114;void*_tmp115;_LLB6: _tmp112=
_tmp111.f2;if((int)_tmp112 != 0)goto _LLB8;_LLB7: goto _LLB9;_LLB8: _tmp113=_tmp111.f1;
if((int)_tmp113 != 0)goto _LLBA;_LLB9: return Cyc_CfFlowInfo_esc_none;_LLBA: _tmp114=
_tmp111.f2;if((int)_tmp114 != 1)goto _LLBC;_LLBB: goto _LLBD;_LLBC: _tmp115=_tmp111.f1;
if((int)_tmp115 != 1)goto _LLBE;_LLBD: return Cyc_CfFlowInfo_esc_this;_LLBE:;_LLBF:
return Cyc_CfFlowInfo_esc_all;_LLB5:;}_LLB3:;_LLB4: {struct _tuple0 _tmp117=({
struct _tuple0 _tmp116;_tmp116.f1=il1;_tmp116.f2=il2;_tmp116;});void*_tmp118;void*
_tmp119;void*_tmp11A;void*_tmp11B;_LLC1: _tmp118=_tmp117.f2;if((int)_tmp118 != 0)
goto _LLC3;_LLC2: goto _LLC4;_LLC3: _tmp119=_tmp117.f1;if((int)_tmp119 != 0)goto _LLC5;
_LLC4: return Cyc_CfFlowInfo_unknown_none;_LLC5: _tmp11A=_tmp117.f2;if((int)_tmp11A
!= 1)goto _LLC7;_LLC6: goto _LLC8;_LLC7: _tmp11B=_tmp117.f1;if((int)_tmp11B != 1)goto
_LLC9;_LLC8: return Cyc_CfFlowInfo_unknown_this;_LLC9:;_LLCA: return Cyc_CfFlowInfo_unknown_all;
_LLC0:;}_LLAE:;}}static int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2)
return 1;{struct _tuple0 _tmp11D=({struct _tuple0 _tmp11C;_tmp11C.f1=r1;_tmp11C.f2=r2;
_tmp11C;});void*_tmp11E;unsigned int _tmp11F;void*_tmp120;unsigned int _tmp121;
void*_tmp122;struct Cyc_Absyn_Vardecl*_tmp123;void*_tmp124;struct Cyc_Absyn_Vardecl*
_tmp125;void*_tmp126;struct Cyc_Absyn_Vardecl*_tmp127;void*_tmp128;struct Cyc_Absyn_Vardecl*
_tmp129;void*_tmp12A;unsigned int _tmp12B;void*_tmp12C;unsigned int _tmp12D;void*
_tmp12E;struct Cyc_Absyn_Vardecl*_tmp12F;void*_tmp130;struct Cyc_Absyn_Vardecl*
_tmp131;_LLCC: _tmp11E=_tmp11D.f1;if(*((int*)_tmp11E)!= 0)goto _LLCE;_tmp11F=((
struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp11E)->f1;_tmp120=_tmp11D.f2;if(*((
int*)_tmp120)!= 0)goto _LLCE;_tmp121=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp120)->f1;_LLCD: return _tmp11F == _tmp121;_LLCE: _tmp122=_tmp11D.f1;if(*((int*)
_tmp122)!= 1)goto _LLD0;_tmp123=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp122)->f1;
_tmp124=_tmp11D.f2;if(*((int*)_tmp124)!= 1)goto _LLD0;_tmp125=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp124)->f1;_LLCF: return _tmp123 == _tmp125;_LLD0: _tmp126=_tmp11D.f1;if(*((int*)
_tmp126)!= 2)goto _LLD2;_tmp127=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp126)->f1;
_tmp128=_tmp11D.f2;if(*((int*)_tmp128)!= 2)goto _LLD2;_tmp129=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp128)->f1;_LLD1: return _tmp127 == _tmp129;_LLD2: _tmp12A=_tmp11D.f1;if(*((int*)
_tmp12A)!= 3)goto _LLD4;_tmp12B=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp12A)->f1;
_tmp12C=_tmp11D.f2;if(*((int*)_tmp12C)!= 3)goto _LLD4;_tmp12D=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmp12C)->f1;_LLD3: return _tmp12B == _tmp12D;_LLD4: _tmp12E=_tmp11D.f1;if(*((int*)
_tmp12E)!= 4)goto _LLD6;_tmp12F=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp12E)->f1;
_tmp130=_tmp11D.f2;if(*((int*)_tmp130)!= 4)goto _LLD6;_tmp131=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp130)->f1;_LLD5: return _tmp12F == _tmp131;_LLD6:;_LLD7: return 0;_LLCB:;}}static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct Cyc_List_List*r1s,struct Cyc_List_List*
r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*
_tmp132=r1s;for(0;_tmp132 != 0;_tmp132=_tmp132->tl){struct Cyc_CfFlowInfo_Reln*
_tmp133=(struct Cyc_CfFlowInfo_Reln*)_tmp132->hd;int found=0;{struct Cyc_List_List*
_tmp134=r2s;for(0;_tmp134 != 0;_tmp134=_tmp134->tl){struct Cyc_CfFlowInfo_Reln*
_tmp135=(struct Cyc_CfFlowInfo_Reln*)_tmp134->hd;if(_tmp133 == _tmp135  || _tmp133->vd
== _tmp135->vd  && Cyc_CfFlowInfo_same_relop((void*)_tmp133->rop,(void*)_tmp135->rop)){
res=({struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=
_tmp133;_tmp136->tl=res;_tmp136;});found=1;break;}}}if(!found)diff=1;}}if(!diff)
res=r1s;return res;}}static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,
struct Cyc_CfFlowInfo_Place*place){struct _handler_cons _tmp137;_push_handler(&
_tmp137);{int _tmp139=0;if(setjmp(_tmp137.handler))_tmp139=1;if(!_tmp139){*l=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*
l,place);;_pop_handler();}else{void*_tmp138=(void*)_exn_thrown;void*_tmp13B=
_tmp138;_LLD9: if(_tmp13B != Cyc_Core_Not_found)goto _LLDB;_LLDA: goto _LLD8;_LLDB:;
_LLDC:(void)_throw(_tmp13B);_LLD8:;}}}void Cyc_CfFlowInfo_print_place_list(struct
Cyc_List_List*x){int _tmp13C=1;while(x != 0){if(!_tmp13C){({void*_tmp13D[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp13E=", ";_tag_dynforward(_tmp13E,sizeof(char),
_get_zero_arr_size(_tmp13E,3));}),_tag_dynforward(_tmp13D,sizeof(void*),0));});
_tmp13C=0;}Cyc_CfFlowInfo_print_place((struct Cyc_CfFlowInfo_Place*)x->hd);x=x->tl;}({
void*_tmp13F[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp140="\n";
_tag_dynforward(_tmp140,sizeof(char),_get_zero_arr_size(_tmp140,2));}),
_tag_dynforward(_tmp13F,sizeof(void*),0));});}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_or_consume(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;{struct _RegionHandle _tmp141=
_new_region("r");struct _RegionHandle*r=& _tmp141;_push_region(r);{struct Cyc_List_List*
_tmp142=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))
Cyc_List_rcopy)(r,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp142,c2.may_consume);while(_tmp142 != 0){mc=({struct
Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp142))->hd;_tmp143->tl=mc;_tmp143;});_tmp142=((
struct Cyc_List_List*)_check_null(_tmp142))->tl;}};_pop_region(r);}return({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp144;_tmp144.consumed=Cyc_CfFlowInfo_union_place_set(
c1.consumed,c2.consumed,0);_tmp144.may_consume=mc;_tmp144;});}struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp145=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(c1.may_consume,c2.may_consume);return({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp146;_tmp146.consumed=Cyc_CfFlowInfo_union_place_set(
c1.consumed,c2.consumed,1);_tmp146.may_consume=_tmp145;_tmp146;});}void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Dict_Dict**all_changed,void*f1,void*f2,int or_consumed){if(f1 == f2)
return f1;{struct _tuple0 _tmp148=({struct _tuple0 _tmp147;_tmp147.f1=f1;_tmp147.f2=
f2;_tmp147;});void*_tmp149;void*_tmp14A;void*_tmp14B;struct Cyc_Dict_Dict*_tmp14C;
struct Cyc_List_List*_tmp14D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp14E;void*_tmp14F;
struct Cyc_Dict_Dict*_tmp150;struct Cyc_List_List*_tmp151;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp152;_LLDE: _tmp149=_tmp148.f1;if((int)_tmp149 != 0)goto _LLE0;_LLDF: return f2;
_LLE0: _tmp14A=_tmp148.f2;if((int)_tmp14A != 0)goto _LLE2;_LLE1: return f1;_LLE2:
_tmp14B=_tmp148.f1;if(_tmp14B <= (void*)1)goto _LLDD;if(*((int*)_tmp14B)!= 0)goto
_LLDD;_tmp14C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14B)->f1;_tmp14D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14B)->f2;_tmp14E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp14B)->f3;_tmp14F=_tmp148.f2;if(_tmp14F <= (void*)1)goto _LLDD;if(*((int*)
_tmp14F)!= 0)goto _LLDD;_tmp150=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp14F)->f1;_tmp151=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14F)->f2;
_tmp152=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp14F)->f3;_LLE3: if(((
_tmp14C == _tmp150  && _tmp14D == _tmp151) && _tmp14E.may_consume == _tmp152.may_consume)
 && _tmp14E.consumed == _tmp152.consumed)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmp153=_new_region("rgn");struct _RegionHandle*rgn=& _tmp153;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp154=({struct Cyc_CfFlowInfo_JoinEnv
_tmp15B;_tmp15B.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp15C=_region_malloc(rgn,
sizeof(*_tmp15C));_tmp15C->rgn=rgn;_tmp15C->places=0;_tmp15C;});_tmp15B.d1=
_tmp14C;_tmp15B.d2=_tmp150;_tmp15B;});struct Cyc_Dict_Dict*_tmp155=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmp154,_tmp14C,_tmp150);struct Cyc_List_List*_tmp156=Cyc_CfFlowInfo_join_relns(
_tmp14D,_tmp151);struct Cyc_CfFlowInfo_ConsumeInfo _tmp157=or_consumed?Cyc_CfFlowInfo_or_consume(
_tmp14E,_tmp152): Cyc_CfFlowInfo_and_consume(_tmp14E,_tmp152);void*_tmp15A=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp158=_cycalloc(sizeof(*_tmp158));
_tmp158[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp159;_tmp159.tag=0;
_tmp159.f1=Cyc_CfFlowInfo_escape_these(_tmp154.pile,all_changed,_tmp155);_tmp159.f2=
_tmp156;_tmp159.f3=_tmp157;_tmp159;});_tmp158;});_npop_handler(0);return _tmp15A;};
_pop_region(rgn);}_LLDD:;}}struct _tuple8{void*f1;void*f2;void*f3;};struct _tuple0
Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Dict_Dict**all_changed,struct _tuple0
pr1,struct _tuple0 pr2,int or_consumed){void*_tmp15E;void*_tmp15F;struct _tuple0
_tmp15D=pr1;_tmp15E=_tmp15D.f1;_tmp15F=_tmp15D.f2;{void*_tmp161;void*_tmp162;
struct _tuple0 _tmp160=pr2;_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;{void*outflow=Cyc_CfFlowInfo_join_flow(
all_changed,_tmp15E,_tmp161,or_consumed);struct _tuple8 _tmp164=({struct _tuple8
_tmp163;_tmp163.f1=_tmp15E;_tmp163.f2=_tmp161;_tmp163.f3=outflow;_tmp163;});void*
_tmp165;void*_tmp166;void*_tmp167;struct Cyc_Dict_Dict*_tmp168;void*_tmp169;
struct Cyc_Dict_Dict*_tmp16A;void*_tmp16B;struct Cyc_Dict_Dict*_tmp16C;struct Cyc_List_List*
_tmp16D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp16E;_LLE5: _tmp165=_tmp164.f1;if((int)
_tmp165 != 0)goto _LLE7;_LLE6: return({struct _tuple0 _tmp16F;_tmp16F.f1=outflow;
_tmp16F.f2=_tmp162;_tmp16F;});_LLE7: _tmp166=_tmp164.f2;if((int)_tmp166 != 0)goto
_LLE9;_LLE8: return({struct _tuple0 _tmp170;_tmp170.f1=outflow;_tmp170.f2=_tmp15F;
_tmp170;});_LLE9: _tmp167=_tmp164.f1;if(_tmp167 <= (void*)1)goto _LLEB;if(*((int*)
_tmp167)!= 0)goto _LLEB;_tmp168=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp167)->f1;_tmp169=_tmp164.f2;if(_tmp169 <= (void*)1)goto _LLEB;if(*((int*)
_tmp169)!= 0)goto _LLEB;_tmp16A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp169)->f1;_tmp16B=_tmp164.f3;if(_tmp16B <= (void*)1)goto _LLEB;if(*((int*)
_tmp16B)!= 0)goto _LLEB;_tmp16C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp16B)->f1;_tmp16D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp16B)->f2;
_tmp16E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp16B)->f3;_LLEA: if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp15F,
_tmp162))return({struct _tuple0 _tmp171;_tmp171.f1=outflow;_tmp171.f2=_tmp162;
_tmp171;});if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp162,_tmp15F))return({struct _tuple0 _tmp172;_tmp172.f1=outflow;_tmp172.f2=
_tmp15F;_tmp172;});{struct _RegionHandle _tmp173=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp173;_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv
_tmp174=({struct Cyc_CfFlowInfo_JoinEnv _tmp17A;_tmp17A.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmp17B=_region_malloc(rgn,sizeof(*_tmp17B));_tmp17B->rgn=rgn;_tmp17B->places=0;
_tmp17B;});_tmp17A.d1=_tmp168;_tmp17A.d2=_tmp16A;_tmp17A;});void*_tmp175=((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(&
_tmp174,0,_tmp15F,_tmp162);struct _tuple0 _tmp179=({struct _tuple0 _tmp176;_tmp176.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp177=_cycalloc(sizeof(*
_tmp177));_tmp177[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp178;_tmp178.tag=
0;_tmp178.f1=Cyc_CfFlowInfo_escape_these(_tmp174.pile,all_changed,_tmp16C);
_tmp178.f2=_tmp16D;_tmp178.f3=_tmp16E;_tmp178;});_tmp177;});_tmp176.f2=_tmp175;
_tmp176;});_npop_handler(0);return _tmp179;};_pop_region(rgn);}_LLEB:;_LLEC:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp17C=_cycalloc(sizeof(*
_tmp17C));_tmp17C[0]=({struct Cyc_Core_Impossible_struct _tmp17D;_tmp17D.tag=Cyc_Core_Impossible;
_tmp17D.f1=({const char*_tmp17E="join_flow_and_rval: BottomFL outflow";
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,37));});_tmp17D;});
_tmp17C;}));_LLE4:;}}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,
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
struct _tuple0 _tmp180=({struct _tuple0 _tmp17F;_tmp17F.f1=r1;_tmp17F.f2=r2;_tmp17F;});
void*_tmp181;struct Cyc_Dict_Dict*_tmp182;void*_tmp183;struct Cyc_Dict_Dict*
_tmp184;_LLEE: _tmp181=_tmp180.f1;if(_tmp181 <= (void*)3)goto _LLF0;if(*((int*)
_tmp181)!= 4)goto _LLF0;_tmp182=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp181)->f1;
_tmp183=_tmp180.f2;if(_tmp183 <= (void*)3)goto _LLF0;if(*((int*)_tmp183)!= 4)goto
_LLF0;_tmp184=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp183)->f1;_LLEF: {
struct Cyc_Dict_Dict*_tmp185=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,
struct _dynforward_ptr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmp182,_tmp184);if(_tmp185 == _tmp182)return r1;if(_tmp185 == _tmp184)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp186=_cycalloc(sizeof(*
_tmp186));_tmp186[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp187;_tmp187.tag=
4;_tmp187.f1=_tmp185;_tmp187;});_tmp186;});}_LLF0:;_LLF1:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188[0]=({
struct Cyc_Core_Impossible_struct _tmp189;_tmp189.tag=Cyc_Core_Impossible;_tmp189.f1=({
const char*_tmp18A="after_absRval -- non-aggregates!";_tag_dynforward(_tmp18A,
sizeof(char),_get_zero_arr_size(_tmp18A,33));});_tmp189;});_tmp188;}));_LLED:;}}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,
struct _dynforward_ptr*key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**
_tmp18B=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp18C=env->changed;*
env->last_field_cell=({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));
_tmp18D->hd=key;_tmp18D->tl=0;_tmp18D;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmp18E=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp18B;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp18C;return _tmp18E;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp18F;_tmp18F.root=(void*)root;
_tmp18F.fields=0;_tmp18F;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Dict_Dict**all_changed,void*f1,void*f2,struct Cyc_Dict_Dict*chg1,struct
Cyc_Dict_Dict*chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(void*)((
void*)0)};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((void*)& dummy_rawexp),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};if(f1 == f2)return f1;{struct _tuple0
_tmp191=({struct _tuple0 _tmp190;_tmp190.f1=f1;_tmp190.f2=f2;_tmp190;});void*
_tmp192;void*_tmp193;void*_tmp194;struct Cyc_Dict_Dict*_tmp195;struct Cyc_List_List*
_tmp196;struct Cyc_CfFlowInfo_ConsumeInfo _tmp197;void*_tmp198;struct Cyc_Dict_Dict*
_tmp199;struct Cyc_List_List*_tmp19A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp19B;
_LLF3: _tmp192=_tmp191.f1;if((int)_tmp192 != 0)goto _LLF5;_LLF4: goto _LLF6;_LLF5:
_tmp193=_tmp191.f2;if((int)_tmp193 != 0)goto _LLF7;_LLF6: return(void*)0;_LLF7:
_tmp194=_tmp191.f1;if(_tmp194 <= (void*)1)goto _LLF2;if(*((int*)_tmp194)!= 0)goto
_LLF2;_tmp195=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp194)->f1;_tmp196=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp194)->f2;_tmp197=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp194)->f3;_tmp198=_tmp191.f2;if(_tmp198 <= (void*)1)goto _LLF2;if(*((int*)
_tmp198)!= 0)goto _LLF2;_tmp199=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp198)->f1;_tmp19A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp198)->f2;
_tmp19B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp198)->f3;_LLF8: if(((
_tmp195 == _tmp199  && _tmp196 == _tmp19A) && _tmp197.may_consume == _tmp19B.may_consume)
 && _tmp197.consumed == _tmp19B.consumed)return f1;{struct _RegionHandle _tmp19C=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp19C;_push_region(rgn);{struct Cyc_CfFlowInfo_Place*
_tmp19D=({struct Cyc_CfFlowInfo_Place*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->root=(
void*)((void*)& dummy_root);_tmp1A8->fields=0;_tmp1A8;});struct Cyc_CfFlowInfo_AfterEnv
_tmp19E=({struct Cyc_CfFlowInfo_AfterEnv _tmp1A5;_tmp1A5.joinenv=({struct Cyc_CfFlowInfo_JoinEnv
_tmp1A6;_tmp1A6.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1A7=_region_malloc(rgn,
sizeof(*_tmp1A7));_tmp1A7->rgn=rgn;_tmp1A7->places=0;_tmp1A7;});_tmp1A6.d1=
_tmp195;_tmp1A6.d2=_tmp199;_tmp1A6;});_tmp1A5.chg1=chg1;_tmp1A5.chg2=chg2;
_tmp1A5.curr_place=_tmp19D;_tmp1A5.last_field_cell=& _tmp19D->fields;_tmp1A5.changed=
Cyc_CfFlowInfo_Neither;_tmp1A5;});struct Cyc_Dict_Dict*_tmp19F=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp19E,_tmp195,_tmp199);struct Cyc_List_List*_tmp1A0=Cyc_CfFlowInfo_join_relns(
_tmp196,_tmp19A);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A1=Cyc_CfFlowInfo_and_consume(
_tmp197,_tmp19B);void*_tmp1A4=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1A3;_tmp1A3.tag=0;_tmp1A3.f1=Cyc_CfFlowInfo_escape_these((_tmp19E.joinenv).pile,
all_changed,_tmp19F);_tmp1A3.f2=_tmp1A0;_tmp1A3.f3=_tmp1A1;_tmp1A3;});_tmp1A2;});
_npop_handler(0);return _tmp1A4;};_pop_region(rgn);}_LLF2:;}}static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1AC;void*_tmp1AD;void*_tmp1AE;struct Cyc_CfFlowInfo_TagCmp*_tmp1AB=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp1AC=*_tmp1AB;_tmp1AD=(void*)_tmp1AC.cmp;_tmp1AE=(void*)_tmp1AC.bd;{
struct Cyc_List_List*_tmp1AF=l1;for(0;_tmp1AF != 0;_tmp1AF=_tmp1AF->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1B1;void*_tmp1B2;void*_tmp1B3;struct Cyc_CfFlowInfo_TagCmp*_tmp1B0=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp1AF->hd;_tmp1B1=*_tmp1B0;_tmp1B2=(void*)_tmp1B1.cmp;_tmp1B3=(void*)_tmp1B1.bd;
if(_tmp1B2 == _tmp1AD  && Cyc_Tcutil_typecmp(_tmp1B3,_tmp1AE)== 0)break;}if(
_tmp1AF == 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp1B5=({struct
_tuple0 _tmp1B4;_tmp1B4.f1=r1;_tmp1B4.f2=r2;_tmp1B4;});void*_tmp1B6;struct Cyc_CfFlowInfo_Place*
_tmp1B7;void*_tmp1B8;struct Cyc_CfFlowInfo_Place*_tmp1B9;void*_tmp1BA;void*
_tmp1BB;void*_tmp1BC;struct Cyc_Dict_Dict*_tmp1BD;void*_tmp1BE;struct Cyc_Dict_Dict*
_tmp1BF;void*_tmp1C0;void*_tmp1C1;void*_tmp1C2;void*_tmp1C3;void*_tmp1C4;void*
_tmp1C5;void*_tmp1C6;struct Cyc_List_List*_tmp1C7;void*_tmp1C8;struct Cyc_List_List*
_tmp1C9;void*_tmp1CA;_LLFA: _tmp1B6=_tmp1B5.f1;if(_tmp1B6 <= (void*)3)goto _LLFC;
if(*((int*)_tmp1B6)!= 2)goto _LLFC;_tmp1B7=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp1B6)->f1;_tmp1B8=_tmp1B5.f2;if(_tmp1B8 <= (void*)3)goto _LLFC;if(*((int*)
_tmp1B8)!= 2)goto _LLFC;_tmp1B9=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1B8)->f1;
_LLFB: return Cyc_CfFlowInfo_place_cmp(_tmp1B7,_tmp1B9)== 0;_LLFC: _tmp1BA=_tmp1B5.f1;
if(_tmp1BA <= (void*)3)goto _LLFE;if(*((int*)_tmp1BA)!= 2)goto _LLFE;_LLFD: goto
_LLFF;_LLFE: _tmp1BB=_tmp1B5.f2;if(_tmp1BB <= (void*)3)goto _LL100;if(*((int*)
_tmp1BB)!= 2)goto _LL100;_LLFF: return 0;_LL100: _tmp1BC=_tmp1B5.f1;if(_tmp1BC <= (
void*)3)goto _LL102;if(*((int*)_tmp1BC)!= 4)goto _LL102;_tmp1BD=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1BC)->f1;_tmp1BE=_tmp1B5.f2;if(_tmp1BE <= (void*)3)goto _LL102;if(*((int*)
_tmp1BE)!= 4)goto _LL102;_tmp1BF=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1BE)->f1;
_LL101: return _tmp1BD == _tmp1BF  || ((int(*)(int(*f)(struct _dynforward_ptr*,void*,
void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((
int(*)(struct _dynforward_ptr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp1BD,_tmp1BF);_LL102: _tmp1C0=_tmp1B5.f2;if((int)_tmp1C0 != 2)goto _LL104;_LL103:
return r1 == (void*)1;_LL104: _tmp1C1=_tmp1B5.f2;if((int)_tmp1C1 != 0)goto _LL106;
_LL105: goto _LL107;_LL106: _tmp1C2=_tmp1B5.f2;if((int)_tmp1C2 != 1)goto _LL108;
_LL107: return 0;_LL108: _tmp1C3=_tmp1B5.f1;if(_tmp1C3 <= (void*)3)goto _LL10A;if(*((
int*)_tmp1C3)!= 1)goto _LL10A;_tmp1C4=_tmp1B5.f2;if(_tmp1C4 <= (void*)3)goto _LL10A;
if(*((int*)_tmp1C4)!= 1)goto _LL10A;_LL109: goto _LLF9;_LL10A: _tmp1C5=_tmp1B5.f1;
if(_tmp1C5 <= (void*)3)goto _LL10C;if(*((int*)_tmp1C5)!= 1)goto _LL10C;_LL10B:
return 0;_LL10C: _tmp1C6=_tmp1B5.f1;if(_tmp1C6 <= (void*)3)goto _LL10E;if(*((int*)
_tmp1C6)!= 3)goto _LL10E;_tmp1C7=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1C6)->f1;
_tmp1C8=_tmp1B5.f2;if(_tmp1C8 <= (void*)3)goto _LL10E;if(*((int*)_tmp1C8)!= 3)goto
_LL10E;_tmp1C9=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1C8)->f1;_LL10D: return
Cyc_CfFlowInfo_tag_cmps_lessthan_approx(_tmp1C7,_tmp1C9);_LL10E: _tmp1CA=_tmp1B5.f2;
if(_tmp1CA <= (void*)3)goto _LL110;if(*((int*)_tmp1CA)!= 3)goto _LL110;_LL10F:
return 0;_LL110:;_LL111: goto _LLF9;_LLF9:;}{struct _tuple0 _tmp1CC=({struct _tuple0
_tmp1CB;_tmp1CB.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp1CB.f2=Cyc_CfFlowInfo_initlevel_approx(
r2);_tmp1CB;});void*_tmp1CD;void*_tmp1CE;void*_tmp1CF;void*_tmp1D0;void*_tmp1D1;
void*_tmp1D2;_LL113: _tmp1CD=_tmp1CC.f1;if((int)_tmp1CD != 2)goto _LL115;_tmp1CE=
_tmp1CC.f2;if((int)_tmp1CE != 2)goto _LL115;_LL114: return 1;_LL115: _tmp1CF=_tmp1CC.f2;
if((int)_tmp1CF != 0)goto _LL117;_LL116: return 1;_LL117: _tmp1D0=_tmp1CC.f1;if((int)
_tmp1D0 != 0)goto _LL119;_LL118: return 0;_LL119: _tmp1D1=_tmp1CC.f2;if((int)_tmp1D1
!= 1)goto _LL11B;_LL11A: return 1;_LL11B: _tmp1D2=_tmp1CC.f1;if((int)_tmp1D2 != 1)
goto _LL112;_LL11C: return 0;_LL112:;}}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp1D3=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp1D4=r2s;for(0;_tmp1D4 != 0;_tmp1D4=_tmp1D4->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1D5=(struct Cyc_CfFlowInfo_Reln*)_tmp1D4->hd;if(
_tmp1D3 == _tmp1D5  || _tmp1D3->vd == _tmp1D5->vd  && Cyc_CfFlowInfo_same_relop((
void*)_tmp1D3->rop,(void*)_tmp1D5->rop)){found=1;break;}}}if(!found)return 0;}
return 1;}int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo c1,
struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume != c2.may_consume){struct
Cyc_List_List*_tmp1D6=c2.may_consume;struct Cyc_List_List*_tmp1D7=c1.may_consume;
for(0;_tmp1D7 != 0  && _tmp1D6 != 0;_tmp1D7=_tmp1D7->tl){if((unsigned int)_tmp1D7 == (
unsigned int)_tmp1D6)goto compare_consumed_set;{int _tmp1D8=Cyc_CfFlowInfo_place_cmp((
struct Cyc_CfFlowInfo_Place*)_tmp1D7->hd,(struct Cyc_CfFlowInfo_Place*)_tmp1D6->hd);
if(_tmp1D8 == 0)_tmp1D6=_tmp1D6->tl;}}if(_tmp1D6 != 0)return 0;}
compare_consumed_set: return Cyc_CfFlowInfo_place_set_subset(c2.consumed,c1.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2){if(f1 == f2)return 1;{
struct _tuple0 _tmp1DA=({struct _tuple0 _tmp1D9;_tmp1D9.f1=f1;_tmp1D9.f2=f2;_tmp1D9;});
void*_tmp1DB;void*_tmp1DC;void*_tmp1DD;struct Cyc_Dict_Dict*_tmp1DE;struct Cyc_List_List*
_tmp1DF;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1E0;void*_tmp1E1;struct Cyc_Dict_Dict*
_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1E4;
_LL11E: _tmp1DB=_tmp1DA.f1;if((int)_tmp1DB != 0)goto _LL120;_LL11F: return 1;_LL120:
_tmp1DC=_tmp1DA.f2;if((int)_tmp1DC != 0)goto _LL122;_LL121: return 0;_LL122: _tmp1DD=
_tmp1DA.f1;if(_tmp1DD <= (void*)1)goto _LL11D;if(*((int*)_tmp1DD)!= 0)goto _LL11D;
_tmp1DE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1DD)->f1;_tmp1DF=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1DD)->f2;_tmp1E0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1DD)->f3;_tmp1E1=_tmp1DA.f2;if(_tmp1E1 <= (void*)1)goto _LL11D;if(*((int*)
_tmp1E1)!= 0)goto _LL11D;_tmp1E2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1E1)->f1;_tmp1E3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1E1)->f2;
_tmp1E4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1E1)->f3;_LL123: if(((
_tmp1DE == _tmp1E2  && _tmp1DF == _tmp1E3) && _tmp1E0.may_consume == _tmp1E4.may_consume)
 && _tmp1E0.consumed == _tmp1E4.consumed)return 1;return(Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp1DE,_tmp1E2) && Cyc_CfFlowInfo_relns_approx(
_tmp1DF,_tmp1E3)) && Cyc_CfFlowInfo_consume_approx(_tmp1E0,_tmp1E4);_LL11D:;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*rs,struct
Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=
p->tl){struct Cyc_CfFlowInfo_Reln*_tmp1E5=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(
_tmp1E5->vd == v){found=1;break;}{void*_tmp1E6=(void*)_tmp1E5->rop;struct Cyc_Absyn_Vardecl*
_tmp1E7;struct Cyc_Absyn_Vardecl*_tmp1E8;struct Cyc_Absyn_Vardecl*_tmp1E9;_LL125:
if(*((int*)_tmp1E6)!= 1)goto _LL127;_tmp1E7=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp1E6)->f1;_LL126: _tmp1E8=_tmp1E7;goto _LL128;_LL127: if(*((int*)_tmp1E6)!= 2)
goto _LL129;_tmp1E8=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1E6)->f1;_LL128:
_tmp1E9=_tmp1E8;goto _LL12A;_LL129: if(*((int*)_tmp1E6)!= 4)goto _LL12B;_tmp1E9=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp1E6)->f1;_LL12A: if(v == _tmp1E9)found=
1;goto _LL124;_LL12B:;_LL12C: goto _LL124;_LL124:;}}if(!found)return rs;{struct Cyc_List_List*
_tmp1EA=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp1EB=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp1EB->vd != v){{void*_tmp1EC=(void*)_tmp1EB->rop;struct Cyc_Absyn_Vardecl*
_tmp1ED;struct Cyc_Absyn_Vardecl*_tmp1EE;struct Cyc_Absyn_Vardecl*_tmp1EF;_LL12E:
if(*((int*)_tmp1EC)!= 1)goto _LL130;_tmp1ED=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp1EC)->f1;_LL12F: _tmp1EE=_tmp1ED;goto _LL131;_LL130: if(*((int*)_tmp1EC)!= 2)
goto _LL132;_tmp1EE=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1EC)->f1;_LL131:
_tmp1EF=_tmp1EE;goto _LL133;_LL132: if(*((int*)_tmp1EC)!= 4)goto _LL134;_tmp1EF=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp1EC)->f1;_LL133: if(v == _tmp1EF)
continue;goto _LL12D;_LL134:;_LL135: goto _LL12D;_LL12D:;}_tmp1EA=({struct Cyc_List_List*
_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->hd=_tmp1EB;_tmp1F0->tl=_tmp1EA;
_tmp1F0;});}}return _tmp1EA;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp1F1=(void*)e->r;void*
_tmp1F2;struct Cyc_Absyn_Vardecl*_tmp1F3;void*_tmp1F4;struct Cyc_Absyn_Vardecl*
_tmp1F5;void*_tmp1F6;struct Cyc_Absyn_Vardecl*_tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Vardecl*
_tmp1F9;_LL137: if(*((int*)_tmp1F1)!= 1)goto _LL139;_tmp1F2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F1)->f2;if(_tmp1F2 <= (void*)1)goto _LL139;if(*((int*)_tmp1F2)!= 0)goto _LL139;
_tmp1F3=((struct Cyc_Absyn_Global_b_struct*)_tmp1F2)->f1;_LL138: _tmp1F5=_tmp1F3;
goto _LL13A;_LL139: if(*((int*)_tmp1F1)!= 1)goto _LL13B;_tmp1F4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F1)->f2;if(_tmp1F4 <= (void*)1)goto _LL13B;if(*((int*)_tmp1F4)!= 2)goto _LL13B;
_tmp1F5=((struct Cyc_Absyn_Param_b_struct*)_tmp1F4)->f1;_LL13A: _tmp1F7=_tmp1F5;
goto _LL13C;_LL13B: if(*((int*)_tmp1F1)!= 1)goto _LL13D;_tmp1F6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F1)->f2;if(_tmp1F6 <= (void*)1)goto _LL13D;if(*((int*)_tmp1F6)!= 3)goto _LL13D;
_tmp1F7=((struct Cyc_Absyn_Local_b_struct*)_tmp1F6)->f1;_LL13C: _tmp1F9=_tmp1F7;
goto _LL13E;_LL13D: if(*((int*)_tmp1F1)!= 1)goto _LL13F;_tmp1F8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1F1)->f2;if(_tmp1F8 <= (void*)1)goto _LL13F;if(*((int*)_tmp1F8)!= 4)goto _LL13F;
_tmp1F9=((struct Cyc_Absyn_Pat_b_struct*)_tmp1F8)->f1;_LL13E: if(!_tmp1F9->escapes)
return Cyc_CfFlowInfo_reln_kill_var(r,_tmp1F9);goto _LL136;_LL13F:;_LL140: goto
_LL136;_LL136:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)
return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{void*_tmp1FA=(void*)e->r;struct Cyc_Absyn_MallocInfo
_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;int _tmp1FD;_LL142: if(*((int*)_tmp1FA)!= 35)
goto _LL144;_tmp1FB=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1FA)->f1;_tmp1FC=
_tmp1FB.num_elts;_tmp1FD=_tmp1FB.fat_result;if(_tmp1FD != 1)goto _LL144;_LL143:
malloc_loop: {void*_tmp1FE=(void*)_tmp1FC->r;struct Cyc_Absyn_Exp*_tmp1FF;void*
_tmp200;struct Cyc_Absyn_Vardecl*_tmp201;void*_tmp202;struct Cyc_Absyn_Vardecl*
_tmp203;void*_tmp204;struct Cyc_Absyn_Vardecl*_tmp205;void*_tmp206;struct Cyc_Absyn_Vardecl*
_tmp207;_LL147: if(*((int*)_tmp1FE)!= 15)goto _LL149;_tmp1FF=((struct Cyc_Absyn_Cast_e_struct*)
_tmp1FE)->f2;_LL148: _tmp1FC=_tmp1FF;goto malloc_loop;_LL149: if(*((int*)_tmp1FE)!= 
1)goto _LL14B;_tmp200=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1FE)->f2;if(
_tmp200 <= (void*)1)goto _LL14B;if(*((int*)_tmp200)!= 4)goto _LL14B;_tmp201=((
struct Cyc_Absyn_Pat_b_struct*)_tmp200)->f1;_LL14A: _tmp203=_tmp201;goto _LL14C;
_LL14B: if(*((int*)_tmp1FE)!= 1)goto _LL14D;_tmp202=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FE)->f2;if(_tmp202 <= (void*)1)goto _LL14D;if(*((int*)_tmp202)!= 3)goto _LL14D;
_tmp203=((struct Cyc_Absyn_Local_b_struct*)_tmp202)->f1;_LL14C: _tmp205=_tmp203;
goto _LL14E;_LL14D: if(*((int*)_tmp1FE)!= 1)goto _LL14F;_tmp204=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FE)->f2;if(_tmp204 <= (void*)1)goto _LL14F;if(*((int*)_tmp204)!= 2)goto _LL14F;
_tmp205=((struct Cyc_Absyn_Param_b_struct*)_tmp204)->f1;_LL14E: _tmp207=_tmp205;
goto _LL150;_LL14F: if(*((int*)_tmp1FE)!= 1)goto _LL151;_tmp206=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1FE)->f2;if(_tmp206 <= (void*)1)goto _LL151;if(*((int*)_tmp206)!= 0)goto _LL151;
_tmp207=((struct Cyc_Absyn_Global_b_struct*)_tmp206)->f1;_LL150: if(_tmp207->escapes)
return r;return({struct Cyc_List_List*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209->vd=
_tmp207;_tmp209->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp20B;_tmp20B.tag=4;_tmp20B.f1=v;_tmp20B;});_tmp20A;}));_tmp209;});_tmp208->tl=
r;_tmp208;});_LL151:;_LL152: return r;_LL146:;}_LL144:;_LL145: goto _LL141;_LL141:;}{
void*_tmp20C=Cyc_Tcutil_compress((void*)v->type);_LL154: if(_tmp20C <= (void*)4)
goto _LL156;if(*((int*)_tmp20C)!= 5)goto _LL156;_LL155: goto _LL153;_LL156:;_LL157:
return r;_LL153:;}loop: {void*_tmp20D=(void*)e->r;struct Cyc_Absyn_Exp*_tmp20E;
void*_tmp20F;int _tmp210;void*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_List_List
_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List _tmp215;struct Cyc_Absyn_Exp*
_tmp216;void*_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_List_List _tmp219;
struct Cyc_Absyn_Exp*_tmp21A;_LL159: if(*((int*)_tmp20D)!= 15)goto _LL15B;_tmp20E=((
struct Cyc_Absyn_Cast_e_struct*)_tmp20D)->f2;_LL15A: e=_tmp20E;goto loop;_LL15B: if(*((
int*)_tmp20D)!= 0)goto _LL15D;_tmp20F=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp20D)->f1;if(_tmp20F <= (void*)1)goto _LL15D;if(*((int*)_tmp20F)!= 2)goto _LL15D;
_tmp210=((struct Cyc_Absyn_Int_c_struct*)_tmp20F)->f2;_LL15C: return({struct Cyc_List_List*
_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->vd=v;_tmp21C->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_EqualConst_struct*_tmp21D=_cycalloc_atomic(sizeof(*_tmp21D));
_tmp21D[0]=({struct Cyc_CfFlowInfo_EqualConst_struct _tmp21E;_tmp21E.tag=0;_tmp21E.f1=(
unsigned int)_tmp210;_tmp21E;});_tmp21D;}));_tmp21C;});_tmp21B->tl=r;_tmp21B;});
_LL15D: if(*((int*)_tmp20D)!= 3)goto _LL15F;_tmp211=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp20D)->f1;if((int)_tmp211 != 4)goto _LL15F;_tmp212=((struct Cyc_Absyn_Primop_e_struct*)
_tmp20D)->f2;if(_tmp212 == 0)goto _LL15F;_tmp213=*_tmp212;_tmp214=_tmp213.tl;if(
_tmp214 == 0)goto _LL15F;_tmp215=*_tmp214;_tmp216=(struct Cyc_Absyn_Exp*)_tmp215.hd;
_LL15E:{void*_tmp21F=(void*)_tmp216->r;void*_tmp220;struct Cyc_List_List*_tmp221;
struct Cyc_List_List _tmp222;struct Cyc_Absyn_Exp*_tmp223;_LL164: if(*((int*)_tmp21F)
!= 3)goto _LL166;_tmp220=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp21F)->f1;
if((int)_tmp220 != 19)goto _LL166;_tmp221=((struct Cyc_Absyn_Primop_e_struct*)
_tmp21F)->f2;if(_tmp221 == 0)goto _LL166;_tmp222=*_tmp221;_tmp223=(struct Cyc_Absyn_Exp*)
_tmp222.hd;_LL165:{void*_tmp224=(void*)_tmp223->r;void*_tmp225;struct Cyc_Absyn_Vardecl*
_tmp226;void*_tmp227;struct Cyc_Absyn_Vardecl*_tmp228;void*_tmp229;struct Cyc_Absyn_Vardecl*
_tmp22A;void*_tmp22B;struct Cyc_Absyn_Vardecl*_tmp22C;_LL169: if(*((int*)_tmp224)
!= 1)goto _LL16B;_tmp225=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp224)->f2;if(
_tmp225 <= (void*)1)goto _LL16B;if(*((int*)_tmp225)!= 0)goto _LL16B;_tmp226=((
struct Cyc_Absyn_Global_b_struct*)_tmp225)->f1;_LL16A: _tmp228=_tmp226;goto _LL16C;
_LL16B: if(*((int*)_tmp224)!= 1)goto _LL16D;_tmp227=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp224)->f2;if(_tmp227 <= (void*)1)goto _LL16D;if(*((int*)_tmp227)!= 3)goto _LL16D;
_tmp228=((struct Cyc_Absyn_Local_b_struct*)_tmp227)->f1;_LL16C: _tmp22A=_tmp228;
goto _LL16E;_LL16D: if(*((int*)_tmp224)!= 1)goto _LL16F;_tmp229=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp224)->f2;if(_tmp229 <= (void*)1)goto _LL16F;if(*((int*)_tmp229)!= 2)goto _LL16F;
_tmp22A=((struct Cyc_Absyn_Param_b_struct*)_tmp229)->f1;_LL16E: _tmp22C=_tmp22A;
goto _LL170;_LL16F: if(*((int*)_tmp224)!= 1)goto _LL171;_tmp22B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp224)->f2;if(_tmp22B <= (void*)1)goto _LL171;if(*((int*)_tmp22B)!= 4)goto _LL171;
_tmp22C=((struct Cyc_Absyn_Pat_b_struct*)_tmp22B)->f1;_LL170: if(_tmp22C->escapes)
return r;return({struct Cyc_List_List*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->vd=v;
_tmp22E->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp22F=
_cycalloc(sizeof(*_tmp22F));_tmp22F[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp230;_tmp230.tag=2;_tmp230.f1=_tmp22C;_tmp230;});_tmp22F;}));_tmp22E;});
_tmp22D->tl=r;_tmp22D;});_LL171:;_LL172: goto _LL168;_LL168:;}goto _LL163;_LL166:;
_LL167: goto _LL163;_LL163:;}goto _LL158;_LL15F: if(*((int*)_tmp20D)!= 3)goto _LL161;
_tmp217=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp20D)->f1;if((int)_tmp217 != 
19)goto _LL161;_tmp218=((struct Cyc_Absyn_Primop_e_struct*)_tmp20D)->f2;if(_tmp218
== 0)goto _LL161;_tmp219=*_tmp218;_tmp21A=(struct Cyc_Absyn_Exp*)_tmp219.hd;_LL160:{
void*_tmp231=(void*)_tmp21A->r;void*_tmp232;struct Cyc_Absyn_Vardecl*_tmp233;void*
_tmp234;struct Cyc_Absyn_Vardecl*_tmp235;void*_tmp236;struct Cyc_Absyn_Vardecl*
_tmp237;void*_tmp238;struct Cyc_Absyn_Vardecl*_tmp239;_LL174: if(*((int*)_tmp231)
!= 1)goto _LL176;_tmp232=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp231)->f2;if(
_tmp232 <= (void*)1)goto _LL176;if(*((int*)_tmp232)!= 0)goto _LL176;_tmp233=((
struct Cyc_Absyn_Global_b_struct*)_tmp232)->f1;_LL175: _tmp235=_tmp233;goto _LL177;
_LL176: if(*((int*)_tmp231)!= 1)goto _LL178;_tmp234=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp231)->f2;if(_tmp234 <= (void*)1)goto _LL178;if(*((int*)_tmp234)!= 3)goto _LL178;
_tmp235=((struct Cyc_Absyn_Local_b_struct*)_tmp234)->f1;_LL177: _tmp237=_tmp235;
goto _LL179;_LL178: if(*((int*)_tmp231)!= 1)goto _LL17A;_tmp236=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp231)->f2;if(_tmp236 <= (void*)1)goto _LL17A;if(*((int*)_tmp236)!= 2)goto _LL17A;
_tmp237=((struct Cyc_Absyn_Param_b_struct*)_tmp236)->f1;_LL179: _tmp239=_tmp237;
goto _LL17B;_LL17A: if(*((int*)_tmp231)!= 1)goto _LL17C;_tmp238=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp231)->f2;if(_tmp238 <= (void*)1)goto _LL17C;if(*((int*)_tmp238)!= 4)goto _LL17C;
_tmp239=((struct Cyc_Absyn_Pat_b_struct*)_tmp238)->f1;_LL17B: if(_tmp239->escapes)
return r;return({struct Cyc_List_List*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->vd=v;
_tmp23B->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp23C=
_cycalloc(sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp23D;_tmp23D.tag=4;_tmp23D.f1=_tmp239;_tmp23D;});_tmp23C;}));_tmp23B;});
_tmp23A->tl=r;_tmp23A;});_LL17C:;_LL17D: goto _LL173;_LL173:;}goto _LL158;_LL161:;
_LL162: goto _LL158;_LL158:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*
_tmp23E=(void*)e1->r;void*_tmp23F;struct Cyc_Absyn_Vardecl*_tmp240;void*_tmp241;
struct Cyc_Absyn_Vardecl*_tmp242;void*_tmp243;struct Cyc_Absyn_Vardecl*_tmp244;
void*_tmp245;struct Cyc_Absyn_Vardecl*_tmp246;_LL17F: if(*((int*)_tmp23E)!= 1)goto
_LL181;_tmp23F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp23E)->f2;if(_tmp23F <= (
void*)1)goto _LL181;if(*((int*)_tmp23F)!= 0)goto _LL181;_tmp240=((struct Cyc_Absyn_Global_b_struct*)
_tmp23F)->f1;_LL180: _tmp242=_tmp240;goto _LL182;_LL181: if(*((int*)_tmp23E)!= 1)
goto _LL183;_tmp241=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp23E)->f2;if(
_tmp241 <= (void*)1)goto _LL183;if(*((int*)_tmp241)!= 2)goto _LL183;_tmp242=((
struct Cyc_Absyn_Param_b_struct*)_tmp241)->f1;_LL182: _tmp244=_tmp242;goto _LL184;
_LL183: if(*((int*)_tmp23E)!= 1)goto _LL185;_tmp243=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp243 <= (void*)1)goto _LL185;if(*((int*)_tmp243)!= 3)goto _LL185;
_tmp244=((struct Cyc_Absyn_Local_b_struct*)_tmp243)->f1;_LL184: _tmp246=_tmp244;
goto _LL186;_LL185: if(*((int*)_tmp23E)!= 1)goto _LL187;_tmp245=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23E)->f2;if(_tmp245 <= (void*)1)goto _LL187;if(*((int*)_tmp245)!= 4)goto _LL187;
_tmp246=((struct Cyc_Absyn_Pat_b_struct*)_tmp245)->f1;_LL186: if(!_tmp246->escapes)
return Cyc_CfFlowInfo_reln_assign_var(r,_tmp246,e2);goto _LL17E;_LL187:;_LL188:
goto _LL17E;_LL17E:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){({struct Cyc_String_pa_struct _tmp249;_tmp249.tag=0;_tmp249.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string((r->vd)->name));{
void*_tmp247[1]={& _tmp249};Cyc_fprintf(Cyc_stderr,({const char*_tmp248="%s";
_tag_dynforward(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,3));}),
_tag_dynforward(_tmp247,sizeof(void*),1));}});{void*_tmp24A=(void*)r->rop;
unsigned int _tmp24B;struct Cyc_Absyn_Vardecl*_tmp24C;struct Cyc_Absyn_Vardecl*
_tmp24D;unsigned int _tmp24E;struct Cyc_Absyn_Vardecl*_tmp24F;_LL18A: if(*((int*)
_tmp24A)!= 0)goto _LL18C;_tmp24B=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp24A)->f1;_LL18B:({struct Cyc_Int_pa_struct _tmp252;_tmp252.tag=1;_tmp252.f1=(
unsigned long)((int)_tmp24B);{void*_tmp250[1]={& _tmp252};Cyc_fprintf(Cyc_stderr,({
const char*_tmp251="==%d";_tag_dynforward(_tmp251,sizeof(char),_get_zero_arr_size(
_tmp251,5));}),_tag_dynforward(_tmp250,sizeof(void*),1));}});goto _LL189;_LL18C:
if(*((int*)_tmp24A)!= 1)goto _LL18E;_tmp24C=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp24A)->f1;_LL18D:({struct Cyc_String_pa_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp24C->name));{
void*_tmp253[1]={& _tmp255};Cyc_fprintf(Cyc_stderr,({const char*_tmp254="<%s";
_tag_dynforward(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,4));}),
_tag_dynforward(_tmp253,sizeof(void*),1));}});goto _LL189;_LL18E: if(*((int*)
_tmp24A)!= 2)goto _LL190;_tmp24D=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp24A)->f1;
_LL18F:({struct Cyc_String_pa_struct _tmp258;_tmp258.tag=0;_tmp258.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp24D->name));{
void*_tmp256[1]={& _tmp258};Cyc_fprintf(Cyc_stderr,({const char*_tmp257="<%s.size";
_tag_dynforward(_tmp257,sizeof(char),_get_zero_arr_size(_tmp257,9));}),
_tag_dynforward(_tmp256,sizeof(void*),1));}});goto _LL189;_LL190: if(*((int*)
_tmp24A)!= 3)goto _LL192;_tmp24E=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp24A)->f1;
_LL191:({struct Cyc_Int_pa_struct _tmp25B;_tmp25B.tag=1;_tmp25B.f1=(unsigned long)((
int)_tmp24E);{void*_tmp259[1]={& _tmp25B};Cyc_fprintf(Cyc_stderr,({const char*
_tmp25A="<%d";_tag_dynforward(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,4));}),
_tag_dynforward(_tmp259,sizeof(void*),1));}});goto _LL189;_LL192: if(*((int*)
_tmp24A)!= 4)goto _LL189;_tmp24F=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp24A)->f1;_LL193:({struct Cyc_String_pa_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp24F->name));{
void*_tmp25C[1]={& _tmp25E};Cyc_fprintf(Cyc_stderr,({const char*_tmp25D="<=%s.size";
_tag_dynforward(_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,10));}),
_tag_dynforward(_tmp25C,sizeof(void*),1));}});goto _LL189;_LL189:;}}void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)
r->hd);if(r->tl != 0)({void*_tmp25F[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp260=",";_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,2));}),
_tag_dynforward(_tmp25F,sizeof(void*),0));});}}void Cyc_CfFlowInfo_print_initlevel(
void*il){void*_tmp261=il;_LL195: if((int)_tmp261 != 0)goto _LL197;_LL196:({void*
_tmp262[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp263="uninitialized";
_tag_dynforward(_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,14));}),
_tag_dynforward(_tmp262,sizeof(void*),0));});goto _LL194;_LL197: if((int)_tmp261 != 
1)goto _LL199;_LL198:({void*_tmp264[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp265="this-initialized";_tag_dynforward(_tmp265,sizeof(char),
_get_zero_arr_size(_tmp265,17));}),_tag_dynforward(_tmp264,sizeof(void*),0));});
goto _LL194;_LL199: if((int)_tmp261 != 2)goto _LL194;_LL19A:({void*_tmp266[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp267="all-initialized";_tag_dynforward(_tmp267,sizeof(
char),_get_zero_arr_size(_tmp267,16));}),_tag_dynforward(_tmp266,sizeof(void*),0));});
goto _LL194;_LL194:;}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp268=root;
struct Cyc_Absyn_Vardecl*_tmp269;_LL19C: if(*((int*)_tmp268)!= 0)goto _LL19E;
_tmp269=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp268)->f1;_LL19D:({struct Cyc_String_pa_struct
_tmp26C;_tmp26C.tag=0;_tmp26C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp269->name));{void*_tmp26A[1]={& _tmp26C};Cyc_fprintf(
Cyc_stderr,({const char*_tmp26B="Root(%s): ";_tag_dynforward(_tmp26B,sizeof(char),
_get_zero_arr_size(_tmp26B,11));}),_tag_dynforward(_tmp26A,sizeof(void*),1));}});
goto _LL19B;_LL19E: if(*((int*)_tmp268)!= 1)goto _LL1A0;_LL19F:({void*_tmp26D[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp26E="MallocPt(_,_)";_tag_dynforward(
_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,14));}),_tag_dynforward(_tmp26D,
sizeof(void*),0));});goto _LL19B;_LL1A0: if(*((int*)_tmp268)!= 2)goto _LL19B;_LL1A1:({
void*_tmp26F[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp270="InitParam(_,_)";
_tag_dynforward(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,15));}),
_tag_dynforward(_tmp26F,sizeof(void*),0));});goto _LL19B;_LL19B:;}void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root((void*)p->root);if(p->fields
!= 0)({void*_tmp271[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp272="+(...)";
_tag_dynforward(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,7));}),
_tag_dynforward(_tmp271,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_place_set(
struct Cyc_Dict_Dict*p){struct _RegionHandle _tmp273=_new_region("r");struct
_RegionHandle*r=& _tmp273;_push_region(r);{struct _handler_cons _tmp274;
_push_handler(& _tmp274);{int _tmp276=0;if(setjmp(_tmp274.handler))_tmp276=1;if(!
_tmp276){{struct Cyc_Iter_Iter _tmp277=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict*d))Cyc_Dict_make_iter)(r,p);struct _tuple5
_tmp278=*((struct _tuple5*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(
r,p);int first=1;({void*_tmp279[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp27A="{ ";
_tag_dynforward(_tmp27A,sizeof(char),_get_zero_arr_size(_tmp27A,3));}),
_tag_dynforward(_tmp279,sizeof(void*),0));});while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple5*))Cyc_Iter_next)(_tmp277,& _tmp278)){struct Cyc_CfFlowInfo_Place*
_tmp27B=_tmp278.f1;if(!first){({void*_tmp27C[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp27D=", ";_tag_dynforward(_tmp27D,sizeof(char),_get_zero_arr_size(
_tmp27D,3));}),_tag_dynforward(_tmp27C,sizeof(void*),0));});first=0;}Cyc_CfFlowInfo_print_place(
_tmp27B);}({void*_tmp27E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp27F="}\n";
_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,3));}),
_tag_dynforward(_tmp27E,sizeof(void*),0));});};_pop_handler();}else{void*_tmp275=(
void*)_exn_thrown;void*_tmp281=_tmp275;_LL1A3:;_LL1A4:({void*_tmp282[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp283="{ }\n";_tag_dynforward(_tmp283,sizeof(char),
_get_zero_arr_size(_tmp283,5));}),_tag_dynforward(_tmp282,sizeof(void*),0));});
_npop_handler(0);return;_LL1A5:;_LL1A6:(void)_throw(_tmp281);_LL1A2:;}}};
_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*_tmp284=rval;
void*_tmp285;void*_tmp286;struct Cyc_CfFlowInfo_Place*_tmp287;struct Cyc_Dict_Dict*
_tmp288;_LL1A8: if((int)_tmp284 != 0)goto _LL1AA;_LL1A9:({void*_tmp289[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp28A="Zero";_tag_dynforward(_tmp28A,sizeof(char),
_get_zero_arr_size(_tmp28A,5));}),_tag_dynforward(_tmp289,sizeof(void*),0));});
goto _LL1A7;_LL1AA: if((int)_tmp284 != 1)goto _LL1AC;_LL1AB:({void*_tmp28B[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp28C="NotZeroAll";_tag_dynforward(_tmp28C,sizeof(char),
_get_zero_arr_size(_tmp28C,11));}),_tag_dynforward(_tmp28B,sizeof(void*),0));});
goto _LL1A7;_LL1AC: if((int)_tmp284 != 2)goto _LL1AE;_LL1AD:({void*_tmp28D[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp28E="NotZeroThis";_tag_dynforward(_tmp28E,sizeof(char),
_get_zero_arr_size(_tmp28E,12));}),_tag_dynforward(_tmp28D,sizeof(void*),0));});
goto _LL1A7;_LL1AE: if(_tmp284 <= (void*)3)goto _LL1B0;if(*((int*)_tmp284)!= 0)goto
_LL1B0;_tmp285=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp284)->f1;
_LL1AF:({void*_tmp28F[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp290="Unknown(";
_tag_dynforward(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,9));}),
_tag_dynforward(_tmp28F,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(
_tmp285);({void*_tmp291[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp292=")";
_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,2));}),
_tag_dynforward(_tmp291,sizeof(void*),0));});goto _LL1A7;_LL1B0: if(_tmp284 <= (
void*)3)goto _LL1B2;if(*((int*)_tmp284)!= 1)goto _LL1B2;_tmp286=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp284)->f1;_LL1B1:({void*_tmp293[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp294="Esc(";_tag_dynforward(_tmp294,sizeof(char),_get_zero_arr_size(_tmp294,5));}),
_tag_dynforward(_tmp293,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(
_tmp286);({void*_tmp295[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp296=")";
_tag_dynforward(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,2));}),
_tag_dynforward(_tmp295,sizeof(void*),0));});goto _LL1A7;_LL1B2: if(_tmp284 <= (
void*)3)goto _LL1B4;if(*((int*)_tmp284)!= 2)goto _LL1B4;_tmp287=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp284)->f1;_LL1B3:({void*_tmp297[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp298="AddrOf(";_tag_dynforward(_tmp298,sizeof(char),_get_zero_arr_size(
_tmp298,8));}),_tag_dynforward(_tmp297,sizeof(void*),0));});Cyc_CfFlowInfo_print_place(
_tmp287);({void*_tmp299[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp29A=")";
_tag_dynforward(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,2));}),
_tag_dynforward(_tmp299,sizeof(void*),0));});goto _LL1A7;_LL1B4: if(_tmp284 <= (
void*)3)goto _LL1B6;if(*((int*)_tmp284)!= 3)goto _LL1B6;_LL1B5:({void*_tmp29B[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp29C="TagCmps(?)";_tag_dynforward(_tmp29C,
sizeof(char),_get_zero_arr_size(_tmp29C,11));}),_tag_dynforward(_tmp29B,sizeof(
void*),0));});goto _LL1A7;_LL1B6: if(_tmp284 <= (void*)3)goto _LL1A7;if(*((int*)
_tmp284)!= 4)goto _LL1A7;_tmp288=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp284)->f1;
_LL1B7:({void*_tmp29D[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp29E="Aggregate(?)";
_tag_dynforward(_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,13));}),
_tag_dynforward(_tmp29D,sizeof(void*),0));});goto _LL1A7;_LL1A7:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){({void*_tmp29F[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2A0="    ";_tag_dynforward(_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,5));}),
_tag_dynforward(_tmp29F,sizeof(void*),0));});Cyc_CfFlowInfo_print_root(root);({
void*_tmp2A1[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A2=" --> ";
_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,6));}),
_tag_dynforward(_tmp2A1,sizeof(void*),0));});Cyc_CfFlowInfo_print_absrval(rval);({
void*_tmp2A3[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A4="\n";
_tag_dynforward(_tmp2A4,sizeof(char),_get_zero_arr_size(_tmp2A4,2));}),
_tag_dynforward(_tmp2A3,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_flowdict(
struct Cyc_Dict_Dict*d){Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}void
Cyc_CfFlowInfo_print_flow(void*f){void*_tmp2A5=f;struct Cyc_Dict_Dict*_tmp2A6;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp2A7;_LL1B9: if((int)_tmp2A5 != 0)goto _LL1BB;
_LL1BA:({void*_tmp2A8[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A9="  BottomFL\n";
_tag_dynforward(_tmp2A9,sizeof(char),_get_zero_arr_size(_tmp2A9,12));}),
_tag_dynforward(_tmp2A8,sizeof(void*),0));});goto _LL1B8;_LL1BB: if(_tmp2A5 <= (
void*)1)goto _LL1B8;if(*((int*)_tmp2A5)!= 0)goto _LL1B8;_tmp2A6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2A5)->f1;_tmp2A7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A5)->f3;
_LL1BC:({void*_tmp2AA[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AB="  ReachableFL:\n";
_tag_dynforward(_tmp2AB,sizeof(char),_get_zero_arr_size(_tmp2AB,16));}),
_tag_dynforward(_tmp2AA,sizeof(void*),0));});Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,
_tmp2A6);({void*_tmp2AC[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AD="\n    consumed: ";
_tag_dynforward(_tmp2AD,sizeof(char),_get_zero_arr_size(_tmp2AD,16));}),
_tag_dynforward(_tmp2AC,sizeof(void*),0));});Cyc_CfFlowInfo_print_place_set(
_tmp2A7.consumed);({void*_tmp2AE[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2AF="\n    may_consume: ";_tag_dynforward(_tmp2AF,sizeof(char),
_get_zero_arr_size(_tmp2AF,19));}),_tag_dynforward(_tmp2AE,sizeof(void*),0));});
Cyc_CfFlowInfo_print_place_list(_tmp2A7.may_consume);({void*_tmp2B0[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2B1="\n";_tag_dynforward(_tmp2B1,sizeof(char),
_get_zero_arr_size(_tmp2B1,2));}),_tag_dynforward(_tmp2B0,sizeof(void*),0));});
goto _LL1B8;_LL1B8:;}int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*
_tmp2B2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp2B3;struct Cyc_Absyn_TunionInfo
_tmp2B4;struct Cyc_List_List*_tmp2B5;void*_tmp2B6;struct Cyc_List_List*_tmp2B7;
struct Cyc_Absyn_AggrInfo _tmp2B8;struct Cyc_List_List*_tmp2B9;struct Cyc_Absyn_TunionFieldInfo
_tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_PtrInfo
_tmp2BD;void*_tmp2BE;struct Cyc_Absyn_PtrAtts _tmp2BF;void*_tmp2C0;struct Cyc_List_List*
_tmp2C1;struct Cyc_List_List*_tmp2C2;struct Cyc_Absyn_ArrayInfo _tmp2C3;void*
_tmp2C4;void*_tmp2C5;void*_tmp2C6;void*_tmp2C7;_LL1BE: if((int)_tmp2B2 != 0)goto
_LL1C0;_LL1BF: goto _LL1C1;_LL1C0: if(_tmp2B2 <= (void*)4)goto _LL1C2;if(*((int*)
_tmp2B2)!= 5)goto _LL1C2;_LL1C1: goto _LL1C3;_LL1C2: if((int)_tmp2B2 != 1)goto _LL1C4;
_LL1C3: goto _LL1C5;_LL1C4: if(_tmp2B2 <= (void*)4)goto _LL1D0;if(*((int*)_tmp2B2)!= 
6)goto _LL1C6;_LL1C5: goto _LL1C7;_LL1C6: if(*((int*)_tmp2B2)!= 12)goto _LL1C8;_LL1C7:
goto _LL1C9;_LL1C8: if(*((int*)_tmp2B2)!= 13)goto _LL1CA;_LL1C9: goto _LL1CB;_LL1CA:
if(*((int*)_tmp2B2)!= 14)goto _LL1CC;_LL1CB: goto _LL1CD;_LL1CC: if(*((int*)_tmp2B2)
!= 17)goto _LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(*((int*)_tmp2B2)!= 18)goto _LL1D0;
_LL1CF: goto _LL1D1;_LL1D0: if((int)_tmp2B2 != 2)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2:
if(_tmp2B2 <= (void*)4)goto _LL1D4;if(*((int*)_tmp2B2)!= 0)goto _LL1D4;_LL1D3:
return 0;_LL1D4: if((int)_tmp2B2 != 3)goto _LL1D6;_LL1D5: {void*_tmp2C8=rgn;_LL1F3:
if((int)_tmp2C8 != 0)goto _LL1F5;_LL1F4: return 1;_LL1F5:;_LL1F6: return 0;_LL1F2:;}
_LL1D6: if(_tmp2B2 <= (void*)4)goto _LL1D8;if(*((int*)_tmp2B2)!= 1)goto _LL1D8;
_tmp2B3=((struct Cyc_Absyn_VarType_struct*)_tmp2B2)->f1;_LL1D7: {void*_tmp2C9=rgn;
struct Cyc_Absyn_Tvar*_tmp2CA;_LL1F8: if((int)_tmp2C9 != 0)goto _LL1FA;_LL1F9: return
0;_LL1FA: if(_tmp2C9 <= (void*)1)goto _LL1F7;if(*((int*)_tmp2C9)!= 0)goto _LL1F7;
_tmp2CA=((struct Cyc_CfFlowInfo_Region_k_struct*)_tmp2C9)->f1;_LL1FB: return Cyc_Absyn_tvar_cmp(
_tmp2B3,_tmp2CA)== 0;_LL1F7:;}_LL1D8: if(_tmp2B2 <= (void*)4)goto _LL1DA;if(*((int*)
_tmp2B2)!= 2)goto _LL1DA;_tmp2B4=((struct Cyc_Absyn_TunionType_struct*)_tmp2B2)->f1;
_tmp2B5=_tmp2B4.targs;_tmp2B6=(void*)_tmp2B4.rgn;_LL1D9: if(Cyc_CfFlowInfo_contains_region(
rgn,_tmp2B6))return 1;_tmp2B7=_tmp2B5;goto _LL1DB;_LL1DA: if(_tmp2B2 <= (void*)4)
goto _LL1DC;if(*((int*)_tmp2B2)!= 16)goto _LL1DC;_tmp2B7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp2B2)->f2;_LL1DB: _tmp2B9=_tmp2B7;goto _LL1DD;_LL1DC: if(_tmp2B2 <= (void*)4)goto
_LL1DE;if(*((int*)_tmp2B2)!= 10)goto _LL1DE;_tmp2B8=((struct Cyc_Absyn_AggrType_struct*)
_tmp2B2)->f1;_tmp2B9=_tmp2B8.targs;_LL1DD: _tmp2BB=_tmp2B9;goto _LL1DF;_LL1DE: if(
_tmp2B2 <= (void*)4)goto _LL1E0;if(*((int*)_tmp2B2)!= 3)goto _LL1E0;_tmp2BA=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2B2)->f1;_tmp2BB=_tmp2BA.targs;
_LL1DF: _tmp2BC=_tmp2BB;goto _LL1E1;_LL1E0: if(_tmp2B2 <= (void*)4)goto _LL1E2;if(*((
int*)_tmp2B2)!= 20)goto _LL1E2;_tmp2BC=((struct Cyc_Absyn_JoinEff_struct*)_tmp2B2)->f1;
_LL1E1: return Cyc_List_exists_c(Cyc_CfFlowInfo_contains_region,rgn,_tmp2BC);
_LL1E2: if(_tmp2B2 <= (void*)4)goto _LL1E4;if(*((int*)_tmp2B2)!= 4)goto _LL1E4;
_tmp2BD=((struct Cyc_Absyn_PointerType_struct*)_tmp2B2)->f1;_tmp2BE=(void*)
_tmp2BD.elt_typ;_tmp2BF=_tmp2BD.ptr_atts;_tmp2C0=(void*)_tmp2BF.rgn;_LL1E3:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp2C0) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp2BE);_LL1E4: if(_tmp2B2 <= (void*)4)goto _LL1E6;if(*((int*)_tmp2B2)!= 8)
goto _LL1E6;_LL1E5: return 0;_LL1E6: if(_tmp2B2 <= (void*)4)goto _LL1E8;if(*((int*)
_tmp2B2)!= 9)goto _LL1E8;_tmp2C1=((struct Cyc_Absyn_TupleType_struct*)_tmp2B2)->f1;
_LL1E7: for(0;_tmp2C1 != 0;_tmp2C1=_tmp2C1->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple6*)_tmp2C1->hd)).f2))return 1;}return 0;_LL1E8: if(_tmp2B2 <= (
void*)4)goto _LL1EA;if(*((int*)_tmp2B2)!= 11)goto _LL1EA;_tmp2C2=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2B2)->f2;_LL1E9: for(0;_tmp2C2 != 0;_tmp2C2=_tmp2C2->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp2C2->hd)->type))return 1;}return 0;
_LL1EA: if(_tmp2B2 <= (void*)4)goto _LL1EC;if(*((int*)_tmp2B2)!= 7)goto _LL1EC;
_tmp2C3=((struct Cyc_Absyn_ArrayType_struct*)_tmp2B2)->f1;_tmp2C4=(void*)_tmp2C3.elt_type;
_LL1EB: _tmp2C5=_tmp2C4;goto _LL1ED;_LL1EC: if(_tmp2B2 <= (void*)4)goto _LL1EE;if(*((
int*)_tmp2B2)!= 19)goto _LL1EE;_tmp2C5=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp2B2)->f1;_LL1ED: _tmp2C6=_tmp2C5;goto _LL1EF;_LL1EE: if(_tmp2B2 <= (void*)4)goto
_LL1F0;if(*((int*)_tmp2B2)!= 21)goto _LL1F0;_tmp2C6=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp2B2)->f1;_LL1EF: return Cyc_CfFlowInfo_contains_region(rgn,_tmp2C6);_LL1F0: if(
_tmp2B2 <= (void*)4)goto _LL1BD;if(*((int*)_tmp2B2)!= 15)goto _LL1BD;_tmp2C7=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2B2)->f1;_LL1F1: return 0;_LL1BD:;}
struct _tuple9{struct Cyc_Dict_Dict*f1;void*f2;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple9*env,void*root,void*rval){struct _tuple9 _tmp2CC;struct Cyc_Dict_Dict*
_tmp2CD;struct Cyc_Dict_Dict**_tmp2CE;void*_tmp2CF;struct _tuple9*_tmp2CB=env;
_tmp2CC=*_tmp2CB;_tmp2CD=_tmp2CC.f1;_tmp2CE=(struct Cyc_Dict_Dict**)&(*_tmp2CB).f1;
_tmp2CF=_tmp2CC.f2;{void*_tmp2D0=root;struct Cyc_Absyn_Vardecl*_tmp2D1;void*
_tmp2D2;_LL1FD: if(*((int*)_tmp2D0)!= 0)goto _LL1FF;_tmp2D1=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp2D0)->f1;_LL1FE: if(Cyc_CfFlowInfo_contains_region(_tmp2CF,(void*)_tmp2D1->type))
rval=Cyc_CfFlowInfo_typ_to_absrval((void*)_tmp2D1->type,Cyc_CfFlowInfo_unknown_none);*
_tmp2CE=Cyc_Dict_insert(*_tmp2CE,root,rval);goto _LL1FC;_LL1FF: if(*((int*)_tmp2D0)
!= 1)goto _LL201;_tmp2D2=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2D0)->f2;
_LL200: if(!Cyc_CfFlowInfo_contains_region(_tmp2CF,_tmp2D2))*_tmp2CE=Cyc_Dict_insert(*
_tmp2CE,root,rval);goto _LL1FC;_LL201: if(*((int*)_tmp2D0)!= 2)goto _LL1FC;_LL202:
goto _LL1FC;_LL1FC:;}}static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_Dict_Dict*fd,void*rgn){void*_tmp2D3=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*
_tmp2D4;_LL204: if(_tmp2D3 <= (void*)4)goto _LL206;if(*((int*)_tmp2D3)!= 1)goto
_LL206;_tmp2D4=((struct Cyc_Absyn_VarType_struct*)_tmp2D3)->f1;_LL205: {struct Cyc_CfFlowInfo_Region_k_struct
v=({struct Cyc_CfFlowInfo_Region_k_struct _tmp2D6;_tmp2D6.tag=0;_tmp2D6.f1=_tmp2D4;
_tmp2D6;});void*v2=(void*)& v;struct _tuple9 env=({struct _tuple9 _tmp2D5;_tmp2D5.f1=
Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp2D5.f2=v2;_tmp2D5;});((void(*)(void(*
f)(struct _tuple9*,void*,void*),struct _tuple9*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}_LL206: if((int)_tmp2D3 != 3)goto
_LL208;_LL207: {struct _tuple9 env=({struct _tuple9 _tmp2D7;_tmp2D7.f1=Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp);_tmp2D7.f2=(void*)0;_tmp2D7;});((void(*)(void(*f)(
struct _tuple9*,void*,void*),struct _tuple9*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f1;}_LL208:;_LL209:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=({
struct Cyc_Core_Impossible_struct _tmp2D9;_tmp2D9.tag=Cyc_Core_Impossible;_tmp2D9.f1=({
const char*_tmp2DA="kill_flowdict_region";_tag_dynforward(_tmp2DA,sizeof(char),
_get_zero_arr_size(_tmp2DA,21));});_tmp2D9;});_tmp2D8;}));_LL203:;}static struct
Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*
rgn){return 0;}void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn){void*_tmp2DB=
f;struct Cyc_Dict_Dict*_tmp2DC;struct Cyc_List_List*_tmp2DD;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2DE;_LL20B: if((int)_tmp2DB != 0)goto _LL20D;_LL20C: return f;_LL20D: if(_tmp2DB <= (
void*)1)goto _LL20A;if(*((int*)_tmp2DB)!= 0)goto _LL20A;_tmp2DC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2DB)->f1;_tmp2DD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2DB)->f2;
_tmp2DE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2DB)->f3;_LL20E: {struct
Cyc_Dict_Dict*_tmp2DF=Cyc_CfFlowInfo_kill_flowdict_region(_tmp2DC,rgn);struct Cyc_List_List*
_tmp2E0=Cyc_CfFlowInfo_kill_relns_region(_tmp2DD,rgn);return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2E1=_cycalloc(sizeof(*_tmp2E1));_tmp2E1[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp2E2;_tmp2E2.tag=0;_tmp2E2.f1=_tmp2DF;_tmp2E2.f2=_tmp2E0;_tmp2E2.f3=_tmp2DE;
_tmp2E2;});_tmp2E1;});}_LL20A:;}struct _tuple10{struct Cyc_Dict_Dict**f1;struct Cyc_Position_Segment*
f2;};static void Cyc_CfFlowInfo_consume_f(struct _tuple10*env,struct Cyc_CfFlowInfo_Place*
place){struct _tuple10 _tmp2E4;struct Cyc_Dict_Dict**_tmp2E5;struct Cyc_Position_Segment*
_tmp2E6;struct _tuple10*_tmp2E3=env;_tmp2E4=*_tmp2E3;_tmp2E5=_tmp2E4.f1;_tmp2E6=
_tmp2E4.f2;if(Cyc_CfFlowInfo_update_place_set((struct Cyc_Dict_Dict**)_tmp2E5,
place,_tmp2E6)){struct Cyc_Position_Segment*_tmp2E7=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(*_tmp2E5,
place);Cyc_CfFlowInfo_unique_err(place,({const char*_tmp2E8="May consume unique pointer %s more than once (cf. %s)";
_tag_dynforward(_tmp2E8,sizeof(char),_get_zero_arr_size(_tmp2E8,54));}),({const
char*_tmp2E9="May consume unique pointer %s more than once";_tag_dynforward(
_tmp2E9,sizeof(char),_get_zero_arr_size(_tmp2E9,45));}),_tmp2E7,_tmp2E6);}}void*
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,void*f){void*
_tmp2EA=f;struct Cyc_Dict_Dict*_tmp2EB;struct Cyc_List_List*_tmp2EC;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2ED;_LL210: if((int)_tmp2EA != 0)goto _LL212;_LL211: return f;_LL212: if(_tmp2EA <= (
void*)1)goto _LL20F;if(*((int*)_tmp2EA)!= 0)goto _LL20F;_tmp2EB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2EA)->f1;_tmp2EC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2EA)->f2;
_tmp2ED=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2EA)->f3;_LL213: if(
_tmp2ED.may_consume == 0)return f;{struct Cyc_Dict_Dict*_tmp2EE=_tmp2ED.consumed;((
void(*)(void(*f)(struct _tuple10*,struct Cyc_CfFlowInfo_Place*),struct _tuple10*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,({struct
_tuple10*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF->f1=& _tmp2EE;_tmp2EF->f2=
loc;_tmp2EF;}),_tmp2ED.may_consume);return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp2F1;_tmp2F1.tag=0;_tmp2F1.f1=_tmp2EB;_tmp2F1.f2=_tmp2EC;_tmp2F1.f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp2F2;_tmp2F2.consumed=_tmp2EE;_tmp2F2.may_consume=0;
_tmp2F2;});_tmp2F1;});_tmp2F0;});}_LL20F:;}void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,void*f){void*_tmp2F3=f;struct Cyc_Dict_Dict*
_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2F6;
_LL215: if((int)_tmp2F3 != 0)goto _LL217;_LL216: return;_LL217: if(_tmp2F3 <= (void*)1)
goto _LL214;if(*((int*)_tmp2F3)!= 0)goto _LL214;_tmp2F4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2F3)->f1;_tmp2F5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2F3)->f2;
_tmp2F6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2F3)->f3;_LL218: {struct
Cyc_List_List*_tmp2F7=_tmp2F6.may_consume;while(_tmp2F7 != 0){if(((int(*)(struct
Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp2F6.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp2F7->hd)){struct Cyc_Position_Segment*_tmp2F8=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
k))Cyc_Dict_lookup)(_tmp2F6.consumed,(struct Cyc_CfFlowInfo_Place*)_tmp2F7->hd);
Cyc_CfFlowInfo_unique_err((struct Cyc_CfFlowInfo_Place*)_tmp2F7->hd,({const char*
_tmp2F9="Read through possibly consumed unique pointer %s (cf. %s)";
_tag_dynforward(_tmp2F9,sizeof(char),_get_zero_arr_size(_tmp2F9,58));}),({const
char*_tmp2FA="Read through possibly consumed unique pointer %s";_tag_dynforward(
_tmp2FA,sizeof(char),_get_zero_arr_size(_tmp2FA,49));}),_tmp2F8,loc);break;}
_tmp2F7=_tmp2F7->tl;}return;}_LL214:;}void*Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,void*f){void*_tmp2FB=f;struct Cyc_Dict_Dict*
_tmp2FC;struct Cyc_List_List*_tmp2FD;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2FE;
_LL21A: if((int)_tmp2FB != 0)goto _LL21C;_LL21B: return f;_LL21C: if(_tmp2FB <= (void*)
1)goto _LL219;if(*((int*)_tmp2FB)!= 0)goto _LL219;_tmp2FC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2FB)->f1;_tmp2FD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2FB)->f2;
_tmp2FE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2FB)->f3;_LL21D: _tmp2FE.may_consume=
0;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2FF=_cycalloc(
sizeof(*_tmp2FF));_tmp2FF[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp300;
_tmp300.tag=0;_tmp300.f1=_tmp2FC;_tmp300.f2=_tmp2FD;_tmp300.f3=_tmp2FE;_tmp300;});
_tmp2FF;});_LL219:;}void*Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*
loc,void*f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{void*_tmp301=f;struct Cyc_Dict_Dict*
_tmp302;struct Cyc_List_List*_tmp303;struct Cyc_CfFlowInfo_ConsumeInfo _tmp304;
_LL21F: if((int)_tmp301 != 0)goto _LL221;_LL220: return f;_LL221: if(_tmp301 <= (void*)
1)goto _LL21E;if(*((int*)_tmp301)!= 0)goto _LL21E;_tmp302=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp301)->f1;_tmp303=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp301)->f2;
_tmp304=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp301)->f3;_LL222: _tmp304.may_consume=
0;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp305=_cycalloc(
sizeof(*_tmp305));_tmp305[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp306;
_tmp306.tag=0;_tmp306.f1=_tmp302;_tmp306.f2=_tmp303;_tmp306.f3=_tmp304;_tmp306;});
_tmp305;});_LL21E:;}}struct _tuple4 Cyc_CfFlowInfo_save_consume_info(void*f,int
clear){static struct Cyc_Dict_Dict**empty_info=0;void*_tmp307=f;struct Cyc_Dict_Dict*
_tmp308;struct Cyc_List_List*_tmp309;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30A;
_LL224: if((int)_tmp307 != 0)goto _LL226;_LL225: if(empty_info == 0)empty_info=({
struct Cyc_Dict_Dict**_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp30B;});return({struct _tuple4 _tmp30C;_tmp30C.f1=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp30D;_tmp30D.consumed=*empty_info;_tmp30D.may_consume=0;_tmp30D;});_tmp30C.f2=
f;_tmp30C;});_LL226: if(_tmp307 <= (void*)1)goto _LL223;if(*((int*)_tmp307)!= 0)
goto _LL223;_tmp308=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp307)->f1;
_tmp309=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp307)->f2;_tmp30A=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp307)->f3;_LL227: if(clear)return({struct
_tuple4 _tmp30E;_tmp30E.f1=_tmp30A;_tmp30E.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp310;_tmp310.tag=0;_tmp310.f1=_tmp308;_tmp310.f2=_tmp309;_tmp310.f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp311;_tmp311.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp311.may_consume=0;_tmp311;});_tmp310;});_tmp30F;});_tmp30E;});else{return({
struct _tuple4 _tmp312;_tmp312.f1=_tmp30A;_tmp312.f2=f;_tmp312;});}_LL223:;}void*
Cyc_CfFlowInfo_restore_consume_info(void*f,struct Cyc_CfFlowInfo_ConsumeInfo c){
void*_tmp313=f;struct Cyc_Dict_Dict*_tmp314;struct Cyc_List_List*_tmp315;_LL229:
if((int)_tmp313 != 0)goto _LL22B;_LL22A: return f;_LL22B: if(_tmp313 <= (void*)1)goto
_LL228;if(*((int*)_tmp313)!= 0)goto _LL228;_tmp314=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp313)->f1;_tmp315=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp313)->f2;
_LL22C: return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp316=_cycalloc(
sizeof(*_tmp316));_tmp316[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp317;
_tmp317.tag=0;_tmp317.f1=_tmp314;_tmp317.f2=_tmp315;_tmp317.f3=c;_tmp317;});
_tmp316;});_LL228:;}
