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
 void exit(int);void*abort();struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct _dynforward_ptr Cstring_to_string(char*);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dynforward_ptr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),
struct Cyc_List_List*x);extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{char*
tag;struct _dynforward_ptr f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr,struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;
int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(
struct _dynforward_ptr),struct _dynforward_ptr errmsg,struct _dynforward_ptr args);
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,
const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct
_dynforward_ptr);unsigned int Cyc_fread(struct _dynforward_ptr,unsigned int,
unsigned int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;
};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};unsigned int Cyc_fwrite(struct _dynforward_ptr,
unsigned int,unsigned int,struct Cyc___cycFILE*);int Cyc_printf(struct
_dynforward_ptr,struct _dynforward_ptr);int remove(const char*);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dynforward_ptr,struct _dynforward_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;
void*abort();void exit(int);char*getenv(const char*);int system(const char*);
unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strconcat_l(
struct Cyc_List_List*);struct _dynforward_ptr Cyc_str_sepstr(struct Cyc_List_List*,
struct _dynforward_ptr);struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr,
int ofs,unsigned int n);struct _dynforward_ptr Cyc_strchr(struct _dynforward_ptr s,
char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
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
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
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
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{
int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern int Cyc_Absyn_porting_c_code;
extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dynforward_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple4*f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple5{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;void*f1;};struct Cyc_YY7_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};
struct Cyc_YY11_struct{int tag;struct _tuple6*f1;};struct Cyc_YY12_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_YY13_struct{int tag;struct _tuple7*f1;};struct Cyc_YY14_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY15_struct{int tag;struct _tuple6*f1;};
struct Cyc_YY16_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY17_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_Declaration_spec*
f1;};struct _tuple8{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY19_struct{
int tag;struct _tuple8*f1;};struct Cyc_YY20_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY21_struct{int tag;void*f1;};struct Cyc_YY22_struct{int tag;void*f1;};
struct Cyc_YY23_struct{int tag;void*f1;};struct Cyc_YY24_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY25_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY26_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY28_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY29_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY30_struct{int tag;int f1;};
struct Cyc_YY31_struct{int tag;void*f1;};struct Cyc_YY32_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY33_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY34_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY35_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY36_struct{int tag;struct _tuple2*f1;};struct Cyc_YY37_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY38_struct{int tag;
struct _tuple10*f1;};struct Cyc_YY39_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;
void*f1;};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;
void*f1;};struct Cyc_YY44_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY45_struct{
int tag;void*f1;};struct Cyc_YY46_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY47_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY48_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY49_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY50_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY51_struct{
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
Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*
tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{
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
int Cyc_Toc_warn_all_null_deref;static int Cyc_pp_r=0;static int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;static int Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=
0;static int Cyc_tc_r=0;static int Cyc_cf_r=0;static int Cyc_noprint_r=0;static int Cyc_ic_r=
0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=0;static int Cyc_stop_after_asmfile_r=
0;static int Cyc_tovc_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=0;static int
Cyc_save_c_r=0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=
0;static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int
Cyc_print_all_kinds_r=0;static int Cyc_print_all_effects_r=0;void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=
1;}static struct _dynforward_ptr*Cyc_output_file=0;static void Cyc_set_output_file(
struct _dynforward_ptr s){Cyc_output_file=({struct _dynforward_ptr*_tmp0=_cycalloc(
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
static void Cyc_set_inline_functions(){Cyc_add_ccarg(({const char*_tmp22="-D_INLINE_FUNCTIONS";
_tag_dynforward(_tmp22,sizeof(char),_get_zero_arr_size(_tmp22,20));}));}static
void Cyc_set_noboundschecks(){Cyc_add_ccarg(({const char*_tmp23="-DNO_CYC_BOUNDS_CHECKS";
_tag_dynforward(_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,23));}));}static
void Cyc_set_nonullchecks(){Cyc_add_ccarg(({const char*_tmp24="-DNO_CYC_NULL_CHECKS";
_tag_dynforward(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,21));}));}static
void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=
1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;Cyc_add_ccarg(({const char*
_tmp25="-DNO_CYC_PREFIX";_tag_dynforward(_tmp25,sizeof(char),_get_zero_arr_size(
_tmp25,16));}));}static void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(({const char*
_tmp26="-DCYC_REGION_PROFILE";_tag_dynforward(_tmp26,sizeof(char),
_get_zero_arr_size(_tmp26,21));}));}static void Cyc_set_pg(){Cyc_pg_r=1;Cyc_add_ccarg(({
const char*_tmp27="-pg";_tag_dynforward(_tmp27,sizeof(char),_get_zero_arr_size(
_tmp27,4));}));}static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=
1;Cyc_add_ccarg(({const char*_tmp28="-S";_tag_dynforward(_tmp28,sizeof(char),
_get_zero_arr_size(_tmp28,3));}));}static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}static void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(
struct _dynforward_ptr s){if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp29="cyc";_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,4));}))
== 0)Cyc_intype=(void*)1;else{if(Cyc_strcmp((struct _dynforward_ptr)s,({const char*
_tmp2A="none";_tag_dynforward(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,5));}))
== 0)Cyc_intype=(void*)0;else{({struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;
_tmp2D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp2B[1]={&
_tmp2D};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C="Input type '%s' not supported\n";
_tag_dynforward(_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,31));}),
_tag_dynforward(_tmp2B,sizeof(void*),1));}});}}}struct _dynforward_ptr Cyc_make_base_filename(
struct _dynforward_ptr s,struct _dynforward_ptr*output_file){struct _dynforward_ptr
_tmp2E=Cyc_Filename_dirname(output_file == 0?s:*output_file);struct
_dynforward_ptr _tmp2F=Cyc_Filename_chop_extension((struct _dynforward_ptr)Cyc_Filename_basename(
s));struct _dynforward_ptr _tmp30=Cyc_strlen((struct _dynforward_ptr)_tmp2E)> 0?Cyc_Filename_concat((
struct _dynforward_ptr)_tmp2E,(struct _dynforward_ptr)_tmp2F): _tmp2F;return(struct
_dynforward_ptr)_tmp30;}static struct Cyc_List_List*Cyc_cyclone_files=0;static void
Cyc_add_other(struct _dynforward_ptr s){if(Cyc_intype == (void*)1  || Cyc_is_cyclone_sourcefile(
s)){Cyc_cyclone_files=({struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));
_tmp31->hd=({struct _dynforward_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=s;
_tmp32;});_tmp31->tl=Cyc_cyclone_files;_tmp31;});{struct _dynforward_ptr _tmp33=
Cyc_make_base_filename(s,Cyc_output_file);struct _dynforward_ptr _tmp34=Cyc_strconcat((
struct _dynforward_ptr)_tmp33,({const char*_tmp35=".c";_tag_dynforward(_tmp35,
sizeof(char),_get_zero_arr_size(_tmp35,3));}));Cyc_add_ccarg((struct
_dynforward_ptr)_tmp34);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _dynforward_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dynforward_ptr(s,sizeof(char),1));}}int Cyc_compile_failure=0;struct Cyc___cycFILE*
Cyc_try_file_open(struct _dynforward_ptr filename,struct _dynforward_ptr mode,struct
_dynforward_ptr msg_part){struct _handler_cons _tmp36;_push_handler(& _tmp36);{int
_tmp38=0;if(setjmp(_tmp36.handler))_tmp38=1;if(!_tmp38){{struct Cyc___cycFILE*
_tmp39=(struct Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);
return _tmp39;};_pop_handler();}else{void*_tmp37=(void*)_exn_thrown;void*_tmp3B=
_tmp37;_LL1:;_LL2: Cyc_compile_failure=1;({struct Cyc_String_pa_struct _tmp3F;
_tmp3F.tag=0;_tmp3F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)filename);{
struct Cyc_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)msg_part);{void*_tmp3C[2]={& _tmp3E,& _tmp3F};Cyc_fprintf(
Cyc_stderr,({const char*_tmp3D="\nError: couldn't open %s %s\n";_tag_dynforward(
_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,29));}),_tag_dynforward(_tmp3C,
sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;
_LL4:(void)_throw(_tmp3B);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),
void*env,struct Cyc_List_List*tds){struct _dynforward_ptr exn_string=({const char*
_tmp5D="";_tag_dynforward(_tmp5D,sizeof(char),_get_zero_arr_size(_tmp5D,1));});
struct _dynforward_ptr explain_string=({const char*_tmp5C="";_tag_dynforward(_tmp5C,
sizeof(char),_get_zero_arr_size(_tmp5C,1));});int other_exn=0;void*ex=(void*)({
struct Cyc_Core_Impossible_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({
struct Cyc_Core_Impossible_struct _tmp5A;_tmp5A.tag=Cyc_Core_Impossible;_tmp5A.f1=({
const char*_tmp5B="";_tag_dynforward(_tmp5B,sizeof(char),_get_zero_arr_size(
_tmp5B,1));});_tmp5A;});_tmp59;});struct Cyc_List_List*_tmp40=0;{struct
_handler_cons _tmp41;_push_handler(& _tmp41);{int _tmp43=0;if(setjmp(_tmp41.handler))
_tmp43=1;if(!_tmp43){_tmp40=f(env,tds);;_pop_handler();}else{void*_tmp42=(void*)
_exn_thrown;void*_tmp45=_tmp42;struct _dynforward_ptr _tmp46;struct _dynforward_ptr
_tmp47;struct _dynforward_ptr _tmp48;_LL6: if(*((void**)_tmp45)!= Cyc_Core_Impossible)
goto _LL8;_tmp46=((struct Cyc_Core_Impossible_struct*)_tmp45)->f1;_LL7: exn_string=({
const char*_tmp49="Exception Core::Impossible";_tag_dynforward(_tmp49,sizeof(char),
_get_zero_arr_size(_tmp49,27));});explain_string=_tmp46;goto _LL5;_LL8: if(_tmp45
!= Cyc_Dict_Absent)goto _LLA;_LL9: exn_string=({const char*_tmp4A="Exception Dict::Absent";
_tag_dynforward(_tmp4A,sizeof(char),_get_zero_arr_size(_tmp4A,23));});goto _LL5;
_LLA: if(*((void**)_tmp45)!= Cyc_Core_Invalid_argument)goto _LLC;_tmp47=((struct
Cyc_Core_Invalid_argument_struct*)_tmp45)->f1;_LLB: exn_string=({const char*_tmp4B="Exception Core::Invalid_argument";
_tag_dynforward(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,33));});
explain_string=_tmp47;goto _LL5;_LLC: if(*((void**)_tmp45)!= Cyc_Core_Failure)goto
_LLE;_tmp48=((struct Cyc_Core_Failure_struct*)_tmp45)->f1;_LLD: exn_string=({const
char*_tmp4C="Exception Core::Failure";_tag_dynforward(_tmp4C,sizeof(char),
_get_zero_arr_size(_tmp4C,24));});explain_string=_tmp48;goto _LL5;_LLE:;_LLF: ex=
_tmp45;other_exn=1;exn_string=({const char*_tmp4D="Uncaught exception";
_tag_dynforward(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,19));});goto _LL5;
_LL10:;_LL11:(void)_throw(_tmp45);_LL5:;}}}if(Cyc_Position_error_p())Cyc_compile_failure=
1;if(Cyc_strcmp((struct _dynforward_ptr)exn_string,({const char*_tmp4E="";
_tag_dynforward(_tmp4E,sizeof(char),_get_zero_arr_size(_tmp4E,1));}))!= 0){Cyc_compile_failure=
1;({struct Cyc_String_pa_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)explain_string);{struct Cyc_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
stage_name);{struct Cyc_String_pa_struct _tmp51;_tmp51.tag=0;_tmp51.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)exn_string);{void*_tmp4F[3]={& _tmp51,&
_tmp52,& _tmp53};Cyc_fprintf(Cyc_stderr,({const char*_tmp50="\n%s thrown during %s: %s";
_tag_dynforward(_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,25));}),
_tag_dynforward(_tmp4F,sizeof(void*),3));}}}});}if(Cyc_compile_failure){({void*
_tmp54[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp55="\nCOMPILATION FAILED!\n";
_tag_dynforward(_tmp55,sizeof(char),_get_zero_arr_size(_tmp55,22));}),
_tag_dynforward(_tmp54,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
if(other_exn)(int)_throw(ex);return _tmp40;}else{if(Cyc_v_r){({struct Cyc_String_pa_struct
_tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
stage_name);{void*_tmp56[1]={& _tmp58};Cyc_fprintf(Cyc_stderr,({const char*_tmp57="%s completed.\n";
_tag_dynforward(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,15));}),
_tag_dynforward(_tmp56,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return _tmp40;}}return _tmp40;}struct Cyc_List_List*Cyc_do_parse(struct
Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*
_tmp5E=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);return _tmp5E;}}struct _tuple12{
struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(
struct _tuple12*tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp60;struct
Cyc_Tcenv_Tenv*_tmp61;struct _tuple12 _tmp5F=*tcenv;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;
Cyc_Tc_tc(_tmp60,_tmp61,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp61,tds);
return tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);return tds;}struct _tuple13{struct Cyc_Tcenv_Tenv*
f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(
struct _tuple13*params,struct Cyc_List_List*tds){struct _tuple13 _tmp63;struct Cyc_Tcenv_Tenv*
_tmp64;struct Cyc___cycFILE*_tmp65;struct Cyc___cycFILE*_tmp66;struct _tuple13*
_tmp62=params;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;_tmp66=_tmp63.f3;{
struct Cyc_Interface_I*_tmp67=Cyc_Interface_extract(_tmp64->ae);if(_tmp65 == 0)Cyc_Interface_save(
_tmp67,_tmp66);else{struct Cyc_Interface_I*_tmp68=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmp65);if(!Cyc_Interface_is_subinterface(_tmp68,_tmp67,({struct _tuple11*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->f1=({const char*_tmp6B="written interface";
_tag_dynforward(_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,18));});_tmp69->f2=({
const char*_tmp6A="maximal interface";_tag_dynforward(_tmp6A,sizeof(char),
_get_zero_arr_size(_tmp6A,18));});_tmp69;})))Cyc_compile_failure=1;else{Cyc_Interface_save(
_tmp68,_tmp66);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,
struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds){return Cyc_Tovc_tovc(tds);}static char _tmp6C[1]="";
static struct _dynforward_ptr Cyc_cyc_include={_tmp6C,_tmp6C + 1};static void Cyc_set_cyc_include(
struct _dynforward_ptr f){Cyc_cyc_include=f;}struct Cyc_List_List*Cyc_do_print(
struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params
_tmp6D=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;_tmp6D.expand_typedefs=
!Cyc_noexpand_r;_tmp6D.to_VC=Cyc_tovc_r;_tmp6D.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmp6D.generate_line_directives=Cyc_generate_line_directives_r;_tmp6D.print_full_evars=
Cyc_print_full_evars_r;_tmp6D.print_all_tvars=Cyc_print_all_tvars_r;_tmp6D.print_all_kinds=
Cyc_print_all_kinds_r;_tmp6D.print_all_effects=Cyc_print_all_effects_r;{struct
Cyc___cycFILE*_tmp6E=Cyc_try_file_open(Cyc_cyc_include,({const char*_tmp74="r";
_tag_dynforward(_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,2));}),({const char*
_tmp75="internal compiler file";_tag_dynforward(_tmp75,sizeof(char),
_get_zero_arr_size(_tmp75,23));}));if(_tmp6E == 0)return tds;{unsigned int n_read=
1024;unsigned int n_written;char buf[1024];{unsigned int _tmp73=1024;unsigned int i;
for(i=0;i < _tmp73;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=Cyc_fread(
_tag_dynforward(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)_tmp6E);if(
n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp6E)){Cyc_compile_failure=1;({
void*_tmp6F[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp70="\nError: problem copying cyc_include.h\n";
_tag_dynforward(_tmp70,sizeof(char),_get_zero_arr_size(_tmp70,39));}),
_tag_dynforward(_tmp6F,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
return tds;}n_written=Cyc_fwrite(_tag_dynforward(buf,sizeof(char),1024),1,n_read,
out_file);if(n_read != n_written){Cyc_compile_failure=1;({void*_tmp71[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp72="\nError: problem copying cyc_include.h\n";
_tag_dynforward(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,39));}),
_tag_dynforward(_tmp71,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
return tds;}}Cyc_fclose((struct Cyc___cycFILE*)_tmp6E);if(Cyc_pp_r){Cyc_Absynpp_set_params(&
_tmp6D);Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(&
_tmp6D);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}}}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dynforward_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dynforward_ptr s,char c){if(s.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmp76=0;unsigned int
_tmp77=Cyc_strlen((struct _dynforward_ptr)s);while(_tmp77 > 0){struct
_dynforward_ptr _tmp78=Cyc_strchr(s,c);if(_tmp78.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr){_tmp76=({struct Cyc_List_List*_tmp79=_cycalloc(
sizeof(*_tmp79));_tmp79->hd=({struct _dynforward_ptr*_tmp7A=_cycalloc(sizeof(*
_tmp7A));_tmp7A[0]=s;_tmp7A;});_tmp79->tl=_tmp76;_tmp79;});break;}else{_tmp76=({
struct Cyc_List_List*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->hd=({struct
_dynforward_ptr*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=(struct
_dynforward_ptr)Cyc_substring((struct _dynforward_ptr)s,0,(unsigned int)((((
struct _dynforward_ptr)_tmp78).curr - s.curr)/ sizeof(char)));_tmp7C;});_tmp7B->tl=
_tmp76;_tmp7B;});_tmp77 -=(((struct _dynforward_ptr)_tmp78).curr - s.curr)/ sizeof(
char);s=_dynforward_ptr_plus(_tmp78,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp76);}}static int Cyc_file_exists(
struct _dynforward_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmp7D;
_push_handler(& _tmp7D);{int _tmp7F=0;if(setjmp(_tmp7D.handler))_tmp7F=1;if(!
_tmp7F){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({const char*_tmp80="r";
_tag_dynforward(_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,2));}));;
_pop_handler();}else{void*_tmp7E=(void*)_exn_thrown;void*_tmp82=_tmp7E;_LL13:;
_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmp82);_LL12:;}}}if(f == 0)return 0;
else{Cyc_fclose((struct Cyc___cycFILE*)f);return 1;}}static struct _dynforward_ptr
Cyc_sprint_list(struct Cyc_List_List*dirs){struct _dynforward_ptr tmp=({const char*
_tmp85="";_tag_dynforward(_tmp85,sizeof(char),_get_zero_arr_size(_tmp85,1));});
for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmp83=*((struct
_dynforward_ptr*)dirs->hd);if(_tmp83.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmp83)== 0)
continue;_tmp83=(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)
_tmp83,({const char*_tmp84=":";_tag_dynforward(_tmp84,sizeof(char),
_get_zero_arr_size(_tmp84,2));}));tmp=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)_tmp83,(struct _dynforward_ptr)tmp);}return tmp;}static
struct _dynforward_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dynforward_ptr
file){if(file.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)return
0;for(0;dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmp86=*((struct
_dynforward_ptr*)dirs->hd);if(_tmp86.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || Cyc_strlen((struct _dynforward_ptr)_tmp86)== 0)
continue;{struct _dynforward_ptr s=(struct _dynforward_ptr)Cyc_Filename_concat(
_tmp86,file);if(Cyc_file_exists(s))return({struct _dynforward_ptr*_tmp87=
_cycalloc(sizeof(*_tmp87));_tmp87[0]=s;_tmp87;});}}return 0;}static struct
_dynforward_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dynforward_ptr file){
struct _dynforward_ptr*_tmp88=Cyc_find(dirs,file);if(_tmp88 == 0){Cyc_compile_failure=
1;Cyc_remove_cfiles();({struct Cyc_String_pa_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{struct Cyc_String_pa_struct
_tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
file);{void*_tmp89[2]={& _tmp8B,& _tmp8C};Cyc_fprintf(Cyc_stderr,({const char*
_tmp8A="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,56));}),
_tag_dynforward(_tmp89,sizeof(void*),2));}}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp8D=_cycalloc(
sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Core_Failure_struct _tmp8E;_tmp8E.tag=Cyc_Core_Failure;
_tmp8E.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp92;_tmp92.tag=0;
_tmp92.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs));{
struct Cyc_String_pa_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file);{void*_tmp8F[2]={& _tmp91,& _tmp92};Cyc_aprintf(({
const char*_tmp90="Error: can't find internal compiler file %s in path %s\n";
_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,56));}),
_tag_dynforward(_tmp8F,sizeof(void*),2));}}});_tmp8E;});_tmp8D;}));}return*
_tmp88;}static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct
_dynforward_ptr subdir){struct Cyc_List_List*_tmp93=0;for(0;dirs != 0;dirs=dirs->tl){
_tmp93=({struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->hd=({
struct _dynforward_ptr*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=(struct
_dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),subdir);
_tmp95;});_tmp94->tl=_tmp93;_tmp94;});}_tmp93=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp93);return _tmp93;}static struct Cyc_List_List*
Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dynforward_ptr subdir){struct Cyc_List_List*
_tmp96=0;for(0;dirs != 0;dirs=dirs->tl){_tmp96=({struct Cyc_List_List*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97->hd=(struct _dynforward_ptr*)dirs->hd;_tmp97->tl=
_tmp96;_tmp97;});_tmp96=({struct Cyc_List_List*_tmp98=_cycalloc(sizeof(*_tmp98));
_tmp98->hd=({struct _dynforward_ptr*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=(
struct _dynforward_ptr)Cyc_Filename_concat(*((struct _dynforward_ptr*)dirs->hd),
subdir);_tmp99;});_tmp98->tl=_tmp96;_tmp98;});}_tmp96=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp96);return _tmp96;}static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dynforward_ptr Cyc_sh_escape_string(
struct _dynforward_ptr s){unsigned int _tmp9A=Cyc_strlen((struct _dynforward_ptr)s);
int _tmp9B=0;int _tmp9C=0;{int i=0;for(0;i < _tmp9A;i ++){char _tmp9D=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(_tmp9D == '\'')_tmp9B ++;else{
if(Cyc_is_other_special(_tmp9D))_tmp9C ++;}}}if(_tmp9B == 0  && _tmp9C == 0)return s;
if(_tmp9B == 0)return(struct _dynforward_ptr)Cyc_strconcat_l(({struct
_dynforward_ptr*_tmp9E[3];_tmp9E[2]=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"'",sizeof(char),2);_tmp9E[1]=({struct _dynforward_ptr*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=(struct _dynforward_ptr)s;_tmpA0;});_tmp9E[0]=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"'",sizeof(char),2);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp9E,sizeof(struct _dynforward_ptr*),3));}));{unsigned int _tmpA2=(_tmp9A + 
_tmp9B)+ _tmp9C;struct _dynforward_ptr s2=({unsigned int _tmpAC=_tmpA2 + 1;char*
_tmpAD=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpAC + 1));struct
_dynforward_ptr _tmpAF=_tag_dynforward(_tmpAD,sizeof(char),_tmpAC + 1);{
unsigned int _tmpAE=_tmpAC;unsigned int i;for(i=0;i < _tmpAE;i ++){_tmpAD[i]='\000';}
_tmpAD[_tmpAE]=(char)0;}_tmpAF;});int _tmpA3=0;int _tmpA4=0;for(0;_tmpA3 < _tmp9A;
_tmpA3 ++){char _tmpA5=*((const char*)_check_dynforward_subscript(s,sizeof(char),
_tmpA3));if(_tmpA5 == '\''  || Cyc_is_other_special(_tmpA5))({struct
_dynforward_ptr _tmpA6=_dynforward_ptr_plus(s2,sizeof(char),_tmpA4 ++);char _tmpA7=*((
char*)_check_dynforward_subscript(_tmpA6,sizeof(char),0));char _tmpA8='\\';if(
_get_dynforward_size(_tmpA6,sizeof(char))== 1  && (_tmpA7 == '\000'  && _tmpA8 != '\000'))
_throw_arraybounds();*((char*)_tmpA6.curr)=_tmpA8;});({struct _dynforward_ptr
_tmpA9=_dynforward_ptr_plus(s2,sizeof(char),_tmpA4 ++);char _tmpAA=*((char*)
_check_dynforward_subscript(_tmpA9,sizeof(char),0));char _tmpAB=_tmpA5;if(
_get_dynforward_size(_tmpA9,sizeof(char))== 1  && (_tmpAA == '\000'  && _tmpAB != '\000'))
_throw_arraybounds();*((char*)_tmpA9.curr)=_tmpAB;});}return(struct
_dynforward_ptr)s2;}}static struct _dynforward_ptr*Cyc_sh_escape_stringptr(struct
_dynforward_ptr*sp){return({struct _dynforward_ptr*_tmpB0=_cycalloc(sizeof(*
_tmpB0));_tmpB0[0]=Cyc_sh_escape_string(*sp);_tmpB0;});}static void Cyc_process_file(
struct _dynforward_ptr filename){struct _dynforward_ptr _tmpB1=Cyc_make_base_filename(
filename,Cyc_output_file);struct _dynforward_ptr _tmpB2=Cyc_strconcat((struct
_dynforward_ptr)_tmpB1,({const char*_tmp10C=".cyp";_tag_dynforward(_tmp10C,
sizeof(char),_get_zero_arr_size(_tmp10C,5));}));struct _dynforward_ptr _tmpB3=Cyc_strconcat((
struct _dynforward_ptr)_tmpB1,({const char*_tmp10B=".cyci";_tag_dynforward(_tmp10B,
sizeof(char),_get_zero_arr_size(_tmp10B,6));}));struct _dynforward_ptr _tmpB4=Cyc_strconcat((
struct _dynforward_ptr)_tmpB1,({const char*_tmp10A=".cycio";_tag_dynforward(
_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,7));}));struct _dynforward_ptr
_tmpB5=Cyc_strconcat((struct _dynforward_ptr)_tmpB1,({const char*_tmp109=".c";
_tag_dynforward(_tmp109,sizeof(char),_get_zero_arr_size(_tmp109,3));}));if(Cyc_v_r)({
struct Cyc_String_pa_struct _tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)filename);{void*_tmpB6[1]={& _tmpB8};Cyc_fprintf(Cyc_stderr,({
const char*_tmpB7="Compiling %s\n";_tag_dynforward(_tmpB7,sizeof(char),
_get_zero_arr_size(_tmpB7,14));}),_tag_dynforward(_tmpB6,sizeof(void*),1));}});{
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmp107="r";
_tag_dynforward(_tmp107,sizeof(char),_get_zero_arr_size(_tmp107,2));}),({const
char*_tmp108="input file";_tag_dynforward(_tmp108,sizeof(char),
_get_zero_arr_size(_tmp108,11));}));if(Cyc_compile_failure  || !((unsigned int)f0))
return;Cyc_fclose((struct Cyc___cycFILE*)f0);{struct _dynforward_ptr _tmpB9=Cyc_str_sepstr(({
struct Cyc_List_List*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->hd=({struct
_dynforward_ptr*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104[0]=(struct
_dynforward_ptr)({const char*_tmp105="";_tag_dynforward(_tmp105,sizeof(char),
_get_zero_arr_size(_tmp105,1));});_tmp104;});_tmp103->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmp103;}),({const char*_tmp106=" ";_tag_dynforward(_tmp106,sizeof(
char),_get_zero_arr_size(_tmp106,2));}));struct _dynforward_ptr def_inc_path=(
struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*
_tmpBA=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);_tmpBA=Cyc_add_subdir(
_tmpBA,({const char*_tmpBB="include";_tag_dynforward(_tmpBB,sizeof(char),
_get_zero_arr_size(_tmpBB,8));}));if(Cyc_strlen((struct _dynforward_ptr)
def_inc_path)> 0)_tmpBA=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));
_tmpBC->hd=({struct _dynforward_ptr*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=
def_inc_path;_tmpBD;});_tmpBC->tl=_tmpBA;_tmpBC;});{char*_tmpBE=getenv((const
char*)"CYCLONE_INCLUDE_PATH");if((char*)_tmpBE != 0)_tmpBA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({
char*_tmpBF=_tmpBE;_tag_dynforward(_tmpBF,sizeof(char),_get_zero_arr_size(_tmpBF,
1));}),':'),_tmpBA);{struct _dynforward_ptr stdinc_string=(struct _dynforward_ptr)
Cyc_str_sepstr(({struct Cyc_List_List*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->hd=({
struct _dynforward_ptr*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100[0]=(struct
_dynforward_ptr)({const char*_tmp101="";_tag_dynforward(_tmp101,sizeof(char),
_get_zero_arr_size(_tmp101,1));});_tmp100;});_tmpFF->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpBA);_tmpFF;}),({const char*_tmp102=" -I";
_tag_dynforward(_tmp102,sizeof(char),_get_zero_arr_size(_tmp102,4));}));struct
_dynforward_ptr ofile_string;if(Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)
ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=
0;_tmpC2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)));{void*_tmpC0[1]={& _tmpC2};Cyc_aprintf(({
const char*_tmpC1=" > %s";_tag_dynforward(_tmpC1,sizeof(char),_get_zero_arr_size(
_tmpC1,6));}),_tag_dynforward(_tmpC0,sizeof(void*),1));}});else{ofile_string=({
const char*_tmpC3="";_tag_dynforward(_tmpC3,sizeof(char),_get_zero_arr_size(
_tmpC3,1));});}}else{ofile_string=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmpC6;_tmpC6.tag=0;_tmpC6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string((
struct _dynforward_ptr)_tmpB2));{void*_tmpC4[1]={& _tmpC6};Cyc_aprintf(({const char*
_tmpC5=" > %s";_tag_dynforward(_tmpC5,sizeof(char),_get_zero_arr_size(_tmpC5,6));}),
_tag_dynforward(_tmpC4,sizeof(void*),1));}});}{struct _dynforward_ptr fixup_string;
if(Cyc_produce_dependencies){if(Cyc_dependencies_target == 0)fixup_string=({const
char*_tmpC7=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_dynforward(_tmpC7,
sizeof(char),_get_zero_arr_size(_tmpC7,35));});else{fixup_string=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_null(Cyc_dependencies_target)));{void*_tmpC8[1]={& _tmpCA};Cyc_aprintf(({
const char*_tmpC9=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dynforward(_tmpC9,sizeof(
char),_get_zero_arr_size(_tmpC9,29));}),_tag_dynforward(_tmpC8,sizeof(void*),1));}});}}
else{fixup_string=({const char*_tmpCB="";_tag_dynforward(_tmpCB,sizeof(char),
_get_zero_arr_size(_tmpCB,1));});}{struct _dynforward_ptr _tmpCC=({struct Cyc_String_pa_struct
_tmpFE;_tmpFE.tag=0;_tmpFE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
ofile_string);{struct Cyc_String_pa_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)fixup_string);{struct Cyc_String_pa_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(
filename));{struct Cyc_String_pa_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdinc_string);{struct Cyc_String_pa_struct
_tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpB9);{struct Cyc_String_pa_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_cpp);{void*_tmpF7[6]={& _tmpF9,&
_tmpFA,& _tmpFB,& _tmpFC,& _tmpFD,& _tmpFE};Cyc_aprintf(({const char*_tmpF8="%s %s%s %s%s%s";
_tag_dynforward(_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,15));}),
_tag_dynforward(_tmpF7,sizeof(void*),6));}}}}}}});if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmpCF;_tmpCF.tag=0;_tmpCF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpCC);{void*_tmpCD[1]={& _tmpCF};Cyc_fprintf(Cyc_stderr,({const char*_tmpCE="%s\n";
_tag_dynforward(_tmpCE,sizeof(char),_get_zero_arr_size(_tmpCE,4));}),
_tag_dynforward(_tmpCD,sizeof(void*),1));}});if(system((const char*)
_untag_dynforward_ptr(_tmpCC,sizeof(char),1))!= 0){Cyc_compile_failure=1;({void*
_tmpD0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpD1="\nError: preprocessing\n";
_tag_dynforward(_tmpD1,sizeof(char),_get_zero_arr_size(_tmpD1,23));}),
_tag_dynforward(_tmpD0,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)
return;Cyc_Position_reset_position((struct _dynforward_ptr)_tmpB2);{struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dynforward_ptr)_tmpB2,({const char*_tmpF5="r";
_tag_dynforward(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,2));}),({const char*
_tmpF6="file";_tag_dynforward(_tmpF6,sizeof(char),_get_zero_arr_size(_tmpF6,5));}));
if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{struct _handler_cons
_tmpD2;_push_handler(& _tmpD2);{int _tmpD4=0;if(setjmp(_tmpD2.handler))_tmpD4=1;
if(!_tmpD4){tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct
Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpD5="parsing";
_tag_dynforward(_tmpD5,sizeof(char),_get_zero_arr_size(_tmpD5,8));}),Cyc_do_parse,(
struct Cyc___cycFILE*)_check_null(in_file),tds);;_pop_handler();}else{void*_tmpD3=(
void*)_exn_thrown;void*_tmpD7=_tmpD3;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dynforward_ptr)_tmpB2);(int)_throw(
_tmpD7);_LL29:;_LL2A:(void)_throw(_tmpD7);_LL26:;}}}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpB2);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmpD8=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmpD8;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmpD9=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r){_npop_handler(0);goto PRINTC;}{struct _tuple12 _tmpDA=({struct
_tuple12 _tmpE2;_tmpE2.f1=tc_rgn;_tmpE2.f2=_tmpD9;_tmpE2;});tds=((struct Cyc_List_List*(*)(
struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple12*,struct
Cyc_List_List*),struct _tuple12*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpDB="type checking";_tag_dynforward(_tmpDB,sizeof(char),
_get_zero_arr_size(_tmpDB,14));}),Cyc_do_typecheck,& _tmpDA,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpB2);_npop_handler(0);return;}if(Cyc_tc_r){
_npop_handler(0);goto PRINTC;}tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpDC="control-flow checking";_tag_dynforward(
_tmpDC,sizeof(char),_get_zero_arr_size(_tmpDC,22));}),Cyc_do_cfcheck,1,tds);if(
Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpB2);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dynforward_ptr(_tmpB3,sizeof(char),1)),(const char*)"r");
struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dynforward_ptr)
_tmpB4,({const char*_tmpE0="w";_tag_dynforward(_tmpE0,sizeof(char),
_get_zero_arr_size(_tmpE0,2));}),({const char*_tmpE1="interface object file";
_tag_dynforward(_tmpE1,sizeof(char),_get_zero_arr_size(_tmpE1,22));}));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dynforward_ptr)_tmpB3);{struct _tuple13 _tmpDD=({struct _tuple13 _tmpDF;
_tmpDF.f1=_tmpD9;_tmpDF.f2=inter_file;_tmpDF.f3=(struct Cyc___cycFILE*)
inter_objfile;_tmpDF;});tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(struct _tuple13*,struct Cyc_List_List*),struct
_tuple13*env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpDE="interface checking";
_tag_dynforward(_tmpDE,sizeof(char),_get_zero_arr_size(_tmpDE,19));}),Cyc_do_interface,&
_tmpDD,tds);if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);}}}};_pop_region(tc_rgn);}
if(Cyc_cf_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmpE3="translation to C";_tag_dynforward(_tmpE3,
sizeof(char),_get_zero_arr_size(_tmpE3,17));}),Cyc_do_translate,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpB2);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dynforward_ptr)_tmpB2);if(
Cyc_compile_failure)return;if(Cyc_tovc_r)tds=((struct Cyc_List_List*(*)(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmpE4="post-pass to VC";
_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,16));}),Cyc_do_tovc,
1,tds);if(Cyc_compile_failure)return;PRINTC: {struct Cyc___cycFILE*out_file;if((
Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmpE5="w";
_tag_dynforward(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,2));}),({const char*
_tmpE6="output file";_tag_dynforward(_tmpE6,sizeof(char),_get_zero_arr_size(
_tmpE6,12));}));else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0)out_file=Cyc_try_file_open(*((struct _dynforward_ptr*)
_check_null(Cyc_output_file)),({const char*_tmpE7="w";_tag_dynforward(_tmpE7,
sizeof(char),_get_zero_arr_size(_tmpE7,2));}),({const char*_tmpE8="output file";
_tag_dynforward(_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,12));}));else{
out_file=Cyc_try_file_open((struct _dynforward_ptr)_tmpB5,({const char*_tmpE9="w";
_tag_dynforward(_tmpE9,sizeof(char),_get_zero_arr_size(_tmpE9,2));}),({const char*
_tmpEA="output file";_tag_dynforward(_tmpEA,sizeof(char),_get_zero_arr_size(
_tmpEA,12));}));}}if(Cyc_compile_failure  || !((unsigned int)out_file))return;{
struct _handler_cons _tmpEB;_push_handler(& _tmpEB);{int _tmpED=0;if(setjmp(_tmpEB.handler))
_tmpED=1;if(!_tmpED){if(!Cyc_noprint_r)tds=((struct Cyc_List_List*(*)(struct
_dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct
Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmpEE="printing";_tag_dynforward(_tmpEE,sizeof(char),
_get_zero_arr_size(_tmpEE,9));}),Cyc_do_print,(struct Cyc___cycFILE*)out_file,tds);;
_pop_handler();}else{void*_tmpEC=(void*)_exn_thrown;void*_tmpF0=_tmpEC;_LL2C:;
_LL2D: Cyc_compile_failure=1;Cyc_file_close((struct Cyc___cycFILE*)out_file);Cyc_cfiles=({
struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->hd=({struct
_dynforward_ptr*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=(struct
_dynforward_ptr)_tmpB5;_tmpF2;});_tmpF1->tl=Cyc_cfiles;_tmpF1;});(int)_throw(
_tmpF0);_LL2E:;_LL2F:(void)_throw(_tmpF0);_LL2B:;}}}Cyc_file_close((struct Cyc___cycFILE*)
out_file);Cyc_cfiles=({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(*_tmpF3));
_tmpF3->hd=({struct _dynforward_ptr*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=(
struct _dynforward_ptr)_tmpB5;_tmpF4;});_tmpF3->tl=Cyc_cfiles;_tmpF3;});}}}}}}}}}}
static char _tmp10D[8]="<final>";static struct _dynforward_ptr Cyc_final_str={_tmp10D,
_tmp10D + 8};static struct _dynforward_ptr*Cyc_final_strptr=& Cyc_final_str;static
struct Cyc_Interface_I*Cyc_read_cycio(struct _dynforward_ptr*n){if(n == (struct
_dynforward_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{struct
_dynforward_ptr basename;{struct _handler_cons _tmp10E;_push_handler(& _tmp10E);{int
_tmp110=0;if(setjmp(_tmp10E.handler))_tmp110=1;if(!_tmp110){basename=(struct
_dynforward_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp10F=(void*)_exn_thrown;void*_tmp112=_tmp10F;_LL31: if(*((void**)_tmp112)!= 
Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(
void)_throw(_tmp112);_LL30:;}}}{struct _dynforward_ptr _tmp113=Cyc_strconcat((
struct _dynforward_ptr)basename,({const char*_tmp118=".cycio";_tag_dynforward(
_tmp118,sizeof(char),_get_zero_arr_size(_tmp118,7));}));struct Cyc___cycFILE*
_tmp114=Cyc_try_file_open((struct _dynforward_ptr)_tmp113,({const char*_tmp116="rb";
_tag_dynforward(_tmp116,sizeof(char),_get_zero_arr_size(_tmp116,3));}),({const
char*_tmp117="interface object file";_tag_dynforward(_tmp117,sizeof(char),
_get_zero_arr_size(_tmp117,22));}));if(_tmp114 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();
exit(1);}Cyc_Position_reset_position((struct _dynforward_ptr)_tmp113);{struct Cyc_Interface_I*
_tmp115=Cyc_Interface_load((struct Cyc___cycFILE*)_tmp114);Cyc_file_close((struct
Cyc___cycFILE*)_tmp114);return _tmp115;}}}}static int Cyc_is_cfile(struct
_dynforward_ptr*n){return*((const char*)_check_dynforward_subscript(*n,sizeof(
char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple14{struct
_dynforward_ptr f1;int f2;struct _dynforward_ptr f3;void*f4;struct _dynforward_ptr f5;
};int Cyc_main(int argc,struct _dynforward_ptr argv){GC_blacklist_warn_clear();{
struct _dynforward_ptr comp=(struct _dynforward_ptr)Cstring_to_string(Ccomp);Cyc_set_cpp((
struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)comp,({const char*
_tmp119=" -x c -E -U__GNUC__ -nostdinc";_tag_dynforward(_tmp119,sizeof(char),
_get_zero_arr_size(_tmp119,30));})));{struct Cyc_List_List*options=({struct
_tuple14*_tmp180[60];_tmp180[59]=({struct _tuple14*_tmp2E3=_cycalloc(sizeof(*
_tmp2E3));_tmp2E3->f1=({const char*_tmp2E8="-port";_tag_dynforward(_tmp2E8,
sizeof(char),_get_zero_arr_size(_tmp2E8,6));});_tmp2E3->f2=0;_tmp2E3->f3=({const
char*_tmp2E7="";_tag_dynforward(_tmp2E7,sizeof(char),_get_zero_arr_size(_tmp2E7,
1));});_tmp2E3->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp2E5=_cycalloc(
sizeof(*_tmp2E5));_tmp2E5[0]=({struct Cyc_Arg_Unit_spec_struct _tmp2E6;_tmp2E6.tag=
0;_tmp2E6.f1=Cyc_set_port_c_code;_tmp2E6;});_tmp2E5;});_tmp2E3->f5=({const char*
_tmp2E4="Suggest how to port C code to Cyclone";_tag_dynforward(_tmp2E4,sizeof(
char),_get_zero_arr_size(_tmp2E4,38));});_tmp2E3;});_tmp180[58]=({struct _tuple14*
_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->f1=({const char*_tmp2E2="-detailedlocation";
_tag_dynforward(_tmp2E2,sizeof(char),_get_zero_arr_size(_tmp2E2,18));});_tmp2DD->f2=
0;_tmp2DD->f3=({const char*_tmp2E1="";_tag_dynforward(_tmp2E1,sizeof(char),
_get_zero_arr_size(_tmp2E1,1));});_tmp2DD->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*
_tmp2DF=_cycalloc(sizeof(*_tmp2DF));_tmp2DF[0]=({struct Cyc_Arg_Clear_spec_struct
_tmp2E0;_tmp2E0.tag=4;_tmp2E0.f1=& Cyc_Position_use_gcc_style_location;_tmp2E0;});
_tmp2DF;});_tmp2DD->f5=({const char*_tmp2DE="Try to give more detailed location information for errors";
_tag_dynforward(_tmp2DE,sizeof(char),_get_zero_arr_size(_tmp2DE,58));});_tmp2DD;});
_tmp180[57]=({struct _tuple14*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->f1=({
const char*_tmp2DC="-noregister";_tag_dynforward(_tmp2DC,sizeof(char),
_get_zero_arr_size(_tmp2DC,12));});_tmp2D7->f2=0;_tmp2D7->f3=({const char*_tmp2DB="";
_tag_dynforward(_tmp2DB,sizeof(char),_get_zero_arr_size(_tmp2DB,1));});_tmp2D7->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));
_tmp2D9[0]=({struct Cyc_Arg_Set_spec_struct _tmp2DA;_tmp2DA.tag=3;_tmp2DA.f1=& Cyc_Parse_no_register;
_tmp2DA;});_tmp2D9;});_tmp2D7->f5=({const char*_tmp2D8="Treat register storage class as public";
_tag_dynforward(_tmp2D8,sizeof(char),_get_zero_arr_size(_tmp2D8,39));});_tmp2D7;});
_tmp180[56]=({struct _tuple14*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->f1=({
const char*_tmp2D6="-warnnullchecks";_tag_dynforward(_tmp2D6,sizeof(char),
_get_zero_arr_size(_tmp2D6,16));});_tmp2D1->f2=0;_tmp2D1->f3=({const char*_tmp2D5="";
_tag_dynforward(_tmp2D5,sizeof(char),_get_zero_arr_size(_tmp2D5,1));});_tmp2D1->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));
_tmp2D3[0]=({struct Cyc_Arg_Set_spec_struct _tmp2D4;_tmp2D4.tag=3;_tmp2D4.f1=& Cyc_Toc_warn_all_null_deref;
_tmp2D4;});_tmp2D3;});_tmp2D1->f5=({const char*_tmp2D2="Warn when any null check can't be eliminated";
_tag_dynforward(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,45));});_tmp2D1;});
_tmp180[55]=({struct _tuple14*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->f1=({
const char*_tmp2D0="-warnboundschecks";_tag_dynforward(_tmp2D0,sizeof(char),
_get_zero_arr_size(_tmp2D0,18));});_tmp2CB->f2=0;_tmp2CB->f3=({const char*_tmp2CF="";
_tag_dynforward(_tmp2CF,sizeof(char),_get_zero_arr_size(_tmp2CF,1));});_tmp2CB->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));
_tmp2CD[0]=({struct Cyc_Arg_Set_spec_struct _tmp2CE;_tmp2CE.tag=3;_tmp2CE.f1=& Cyc_Toc_warn_bounds_checks;
_tmp2CE;});_tmp2CD;});_tmp2CB->f5=({const char*_tmp2CC="Warn when bounds checks can't be eliminated";
_tag_dynforward(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,44));});_tmp2CB;});
_tmp180[54]=({struct _tuple14*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5->f1=({
const char*_tmp2CA="-CI";_tag_dynforward(_tmp2CA,sizeof(char),_get_zero_arr_size(
_tmp2CA,4));});_tmp2C5->f2=0;_tmp2C5->f3=({const char*_tmp2C9=" <file>";
_tag_dynforward(_tmp2C9,sizeof(char),_get_zero_arr_size(_tmp2C9,8));});_tmp2C5->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));
_tmp2C7[0]=({struct Cyc_Arg_String_spec_struct _tmp2C8;_tmp2C8.tag=5;_tmp2C8.f1=
Cyc_set_cyc_include;_tmp2C8;});_tmp2C7;});_tmp2C5->f5=({const char*_tmp2C6="Set cyc_include.h to be <file>";
_tag_dynforward(_tmp2C6,sizeof(char),_get_zero_arr_size(_tmp2C6,31));});_tmp2C5;});
_tmp180[53]=({struct _tuple14*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->f1=({
const char*_tmp2C4="-printalleffects";_tag_dynforward(_tmp2C4,sizeof(char),
_get_zero_arr_size(_tmp2C4,17));});_tmp2BF->f2=0;_tmp2BF->f3=({const char*_tmp2C3="";
_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,1));});_tmp2BF->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));
_tmp2C1[0]=({struct Cyc_Arg_Set_spec_struct _tmp2C2;_tmp2C2.tag=3;_tmp2C2.f1=& Cyc_print_all_effects_r;
_tmp2C2;});_tmp2C1;});_tmp2BF->f5=({const char*_tmp2C0="Print effects for functions (type debugging)";
_tag_dynforward(_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,45));});_tmp2BF;});
_tmp180[52]=({struct _tuple14*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->f1=({
const char*_tmp2BE="-printfullevars";_tag_dynforward(_tmp2BE,sizeof(char),
_get_zero_arr_size(_tmp2BE,16));});_tmp2B9->f2=0;_tmp2B9->f3=({const char*_tmp2BD="";
_tag_dynforward(_tmp2BD,sizeof(char),_get_zero_arr_size(_tmp2BD,1));});_tmp2B9->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));
_tmp2BB[0]=({struct Cyc_Arg_Set_spec_struct _tmp2BC;_tmp2BC.tag=3;_tmp2BC.f1=& Cyc_print_full_evars_r;
_tmp2BC;});_tmp2BB;});_tmp2B9->f5=({const char*_tmp2BA="Print full information for evars (type debugging)";
_tag_dynforward(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,50));});_tmp2B9;});
_tmp180[51]=({struct _tuple14*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->f1=({
const char*_tmp2B8="-printallkinds";_tag_dynforward(_tmp2B8,sizeof(char),
_get_zero_arr_size(_tmp2B8,15));});_tmp2B3->f2=0;_tmp2B3->f3=({const char*_tmp2B7="";
_tag_dynforward(_tmp2B7,sizeof(char),_get_zero_arr_size(_tmp2B7,1));});_tmp2B3->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));
_tmp2B5[0]=({struct Cyc_Arg_Set_spec_struct _tmp2B6;_tmp2B6.tag=3;_tmp2B6.f1=& Cyc_print_all_kinds_r;
_tmp2B6;});_tmp2B5;});_tmp2B3->f5=({const char*_tmp2B4="Always print kinds of type variables";
_tag_dynforward(_tmp2B4,sizeof(char),_get_zero_arr_size(_tmp2B4,37));});_tmp2B3;});
_tmp180[50]=({struct _tuple14*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->f1=({
const char*_tmp2B2="-printalltvars";_tag_dynforward(_tmp2B2,sizeof(char),
_get_zero_arr_size(_tmp2B2,15));});_tmp2AD->f2=0;_tmp2AD->f3=({const char*_tmp2B1="";
_tag_dynforward(_tmp2B1,sizeof(char),_get_zero_arr_size(_tmp2B1,1));});_tmp2AD->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));
_tmp2AF[0]=({struct Cyc_Arg_Set_spec_struct _tmp2B0;_tmp2B0.tag=3;_tmp2B0.f1=& Cyc_print_all_tvars_r;
_tmp2B0;});_tmp2AF;});_tmp2AD->f5=({const char*_tmp2AE="Print all type variables (even implicit default effects)";
_tag_dynforward(_tmp2AE,sizeof(char),_get_zero_arr_size(_tmp2AE,57));});_tmp2AD;});
_tmp180[49]=({struct _tuple14*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->f1=({
const char*_tmp2AC="-noexpandtypedefs";_tag_dynforward(_tmp2AC,sizeof(char),
_get_zero_arr_size(_tmp2AC,18));});_tmp2A7->f2=0;_tmp2A7->f3=({const char*_tmp2AB="";
_tag_dynforward(_tmp2AB,sizeof(char),_get_zero_arr_size(_tmp2AB,1));});_tmp2A7->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));
_tmp2A9[0]=({struct Cyc_Arg_Set_spec_struct _tmp2AA;_tmp2AA.tag=3;_tmp2AA.f1=& Cyc_noexpand_r;
_tmp2AA;});_tmp2A9;});_tmp2A7->f5=({const char*_tmp2A8="Don't expand typedefs in pretty printing";
_tag_dynforward(_tmp2A8,sizeof(char),_get_zero_arr_size(_tmp2A8,41));});_tmp2A7;});
_tmp180[48]=({struct _tuple14*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->f1=({
const char*_tmp2A6="-noremoveunused";_tag_dynforward(_tmp2A6,sizeof(char),
_get_zero_arr_size(_tmp2A6,16));});_tmp2A1->f2=0;_tmp2A1->f3=({const char*_tmp2A5="";
_tag_dynforward(_tmp2A5,sizeof(char),_get_zero_arr_size(_tmp2A5,1));});_tmp2A1->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));
_tmp2A3[0]=({struct Cyc_Arg_Set_spec_struct _tmp2A4;_tmp2A4.tag=3;_tmp2A4.f1=& Cyc_noshake_r;
_tmp2A4;});_tmp2A3;});_tmp2A1->f5=({const char*_tmp2A2="Don't remove externed variables that aren't used";
_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,49));});_tmp2A1;});
_tmp180[47]=({struct _tuple14*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->f1=({
const char*_tmp2A0="-nogc";_tag_dynforward(_tmp2A0,sizeof(char),
_get_zero_arr_size(_tmp2A0,6));});_tmp29B->f2=0;_tmp29B->f3=({const char*_tmp29F="";
_tag_dynforward(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,1));});_tmp29B->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));
_tmp29D[0]=({struct Cyc_Arg_Set_spec_struct _tmp29E;_tmp29E.tag=3;_tmp29E.f1=& Cyc_nogc_r;
_tmp29E;});_tmp29D;});_tmp29B->f5=({const char*_tmp29C="Don't link in the garbage collector";
_tag_dynforward(_tmp29C,sizeof(char),_get_zero_arr_size(_tmp29C,36));});_tmp29B;});
_tmp180[46]=({struct _tuple14*_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295->f1=({
const char*_tmp29A="-nocyc";_tag_dynforward(_tmp29A,sizeof(char),
_get_zero_arr_size(_tmp29A,7));});_tmp295->f2=0;_tmp295->f3=({const char*_tmp299="";
_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size(_tmp299,1));});_tmp295->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp297=_cycalloc(sizeof(*_tmp297));
_tmp297[0]=({struct Cyc_Arg_Unit_spec_struct _tmp298;_tmp298.tag=0;_tmp298.f1=Cyc_set_nocyc;
_tmp298;});_tmp297;});_tmp295->f5=({const char*_tmp296="Don't add implicit namespace Cyc";
_tag_dynforward(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,33));});_tmp295;});
_tmp180[45]=({struct _tuple14*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->f1=({
const char*_tmp294="-no-cpp-precomp";_tag_dynforward(_tmp294,sizeof(char),
_get_zero_arr_size(_tmp294,16));});_tmp28F->f2=0;_tmp28F->f3=({const char*_tmp293="";
_tag_dynforward(_tmp293,sizeof(char),_get_zero_arr_size(_tmp293,1));});_tmp28F->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp291=_cycalloc(sizeof(*_tmp291));
_tmp291[0]=({struct Cyc_Arg_Unit_spec_struct _tmp292;_tmp292.tag=0;_tmp292.f1=Cyc_set_nocppprecomp;
_tmp292;});_tmp291;});_tmp28F->f5=({const char*_tmp290="Don't do smart preprocessing (mac only)";
_tag_dynforward(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,40));});_tmp28F;});
_tmp180[44]=({struct _tuple14*_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289->f1=({
const char*_tmp28E="-use-cpp";_tag_dynforward(_tmp28E,sizeof(char),
_get_zero_arr_size(_tmp28E,9));});_tmp289->f2=0;_tmp289->f3=({const char*_tmp28D="<path>";
_tag_dynforward(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,7));});_tmp289->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));
_tmp28B[0]=({struct Cyc_Arg_String_spec_struct _tmp28C;_tmp28C.tag=5;_tmp28C.f1=
Cyc_set_cpp;_tmp28C;});_tmp28B;});_tmp289->f5=({const char*_tmp28A="Indicate which preprocessor to use";
_tag_dynforward(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,35));});_tmp289;});
_tmp180[43]=({struct _tuple14*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283->f1=({
const char*_tmp288="--inline-checks";_tag_dynforward(_tmp288,sizeof(char),
_get_zero_arr_size(_tmp288,16));});_tmp283->f2=0;_tmp283->f3=({const char*_tmp287="";
_tag_dynforward(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,1));});_tmp283->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp285=_cycalloc(sizeof(*_tmp285));
_tmp285[0]=({struct Cyc_Arg_Unit_spec_struct _tmp286;_tmp286.tag=0;_tmp286.f1=Cyc_set_inline_functions;
_tmp286;});_tmp285;});_tmp283->f5=({const char*_tmp284="Inline bounds checks instead of #define";
_tag_dynforward(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,40));});_tmp283;});
_tmp180[42]=({struct _tuple14*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->f1=({
const char*_tmp282="--noboundschecks";_tag_dynforward(_tmp282,sizeof(char),
_get_zero_arr_size(_tmp282,17));});_tmp27D->f2=0;_tmp27D->f3=({const char*_tmp281="";
_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(_tmp281,1));});_tmp27D->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp27F=_cycalloc(sizeof(*_tmp27F));
_tmp27F[0]=({struct Cyc_Arg_Unit_spec_struct _tmp280;_tmp280.tag=0;_tmp280.f1=Cyc_set_noboundschecks;
_tmp280;});_tmp27F;});_tmp27D->f5=({const char*_tmp27E="Disable bounds checks";
_tag_dynforward(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,22));});_tmp27D;});
_tmp180[41]=({struct _tuple14*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->f1=({
const char*_tmp27C="--nonullchecks";_tag_dynforward(_tmp27C,sizeof(char),
_get_zero_arr_size(_tmp27C,15));});_tmp277->f2=0;_tmp277->f3=({const char*_tmp27B="";
_tag_dynforward(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,1));});_tmp277->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp279=_cycalloc(sizeof(*_tmp279));
_tmp279[0]=({struct Cyc_Arg_Unit_spec_struct _tmp27A;_tmp27A.tag=0;_tmp27A.f1=Cyc_set_nonullchecks;
_tmp27A;});_tmp279;});_tmp277->f5=({const char*_tmp278="Disable null checks";
_tag_dynforward(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,20));});_tmp277;});
_tmp180[40]=({struct _tuple14*_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271->f1=({
const char*_tmp276="--nochecks";_tag_dynforward(_tmp276,sizeof(char),
_get_zero_arr_size(_tmp276,11));});_tmp271->f2=0;_tmp271->f3=({const char*_tmp275="";
_tag_dynforward(_tmp275,sizeof(char),_get_zero_arr_size(_tmp275,1));});_tmp271->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp273=_cycalloc(sizeof(*_tmp273));
_tmp273[0]=({struct Cyc_Arg_Unit_spec_struct _tmp274;_tmp274.tag=0;_tmp274.f1=Cyc_set_nochecks;
_tmp274;});_tmp273;});_tmp271->f5=({const char*_tmp272="Disable bounds/null checks";
_tag_dynforward(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,27));});_tmp271;});
_tmp180[39]=({struct _tuple14*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->f1=({
const char*_tmp270="--lineno";_tag_dynforward(_tmp270,sizeof(char),
_get_zero_arr_size(_tmp270,9));});_tmp26B->f2=0;_tmp26B->f3=({const char*_tmp26F="";
_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,1));});_tmp26B->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp26D=_cycalloc(sizeof(*_tmp26D));
_tmp26D[0]=({struct Cyc_Arg_Unit_spec_struct _tmp26E;_tmp26E.tag=0;_tmp26E.f1=Cyc_set_lineno;
_tmp26E;});_tmp26D;});_tmp26B->f5=({const char*_tmp26C="Generate line numbers for debugging";
_tag_dynforward(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,36));});_tmp26B;});
_tmp180[38]=({struct _tuple14*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->f1=({
const char*_tmp26A="-save-c";_tag_dynforward(_tmp26A,sizeof(char),
_get_zero_arr_size(_tmp26A,8));});_tmp265->f2=0;_tmp265->f3=({const char*_tmp269="";
_tag_dynforward(_tmp269,sizeof(char),_get_zero_arr_size(_tmp269,1));});_tmp265->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp267=_cycalloc(sizeof(*_tmp267));
_tmp267[0]=({struct Cyc_Arg_Set_spec_struct _tmp268;_tmp268.tag=3;_tmp268.f1=& Cyc_save_c_r;
_tmp268;});_tmp267;});_tmp265->f5=({const char*_tmp266="Don't delete temporary C files";
_tag_dynforward(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,31));});_tmp265;});
_tmp180[37]=({struct _tuple14*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->f1=({
const char*_tmp264="-save-temps";_tag_dynforward(_tmp264,sizeof(char),
_get_zero_arr_size(_tmp264,12));});_tmp25F->f2=0;_tmp25F->f3=({const char*_tmp263="";
_tag_dynforward(_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,1));});_tmp25F->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp261=_cycalloc(sizeof(*_tmp261));
_tmp261[0]=({struct Cyc_Arg_Unit_spec_struct _tmp262;_tmp262.tag=0;_tmp262.f1=Cyc_set_save_temps;
_tmp262;});_tmp261;});_tmp25F->f5=({const char*_tmp260="Don't delete temporary files";
_tag_dynforward(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,29));});_tmp25F;});
_tmp180[36]=({struct _tuple14*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->f1=({
const char*_tmp25E="-tovc";_tag_dynforward(_tmp25E,sizeof(char),
_get_zero_arr_size(_tmp25E,6));});_tmp259->f2=0;_tmp259->f3=({const char*_tmp25D="";
_tag_dynforward(_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,1));});_tmp259->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp25B=_cycalloc(sizeof(*_tmp25B));
_tmp25B[0]=({struct Cyc_Arg_Set_spec_struct _tmp25C;_tmp25C.tag=3;_tmp25C.f1=& Cyc_tovc_r;
_tmp25C;});_tmp25B;});_tmp259->f5=({const char*_tmp25A="Avoid gcc extensions in C output";
_tag_dynforward(_tmp25A,sizeof(char),_get_zero_arr_size(_tmp25A,33));});_tmp259;});
_tmp180[35]=({struct _tuple14*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->f1=({
const char*_tmp258="-up";_tag_dynforward(_tmp258,sizeof(char),_get_zero_arr_size(
_tmp258,4));});_tmp253->f2=0;_tmp253->f3=({const char*_tmp257="";_tag_dynforward(
_tmp257,sizeof(char),_get_zero_arr_size(_tmp257,1));});_tmp253->f4=(void*)({
struct Cyc_Arg_Clear_spec_struct*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[0]=({
struct Cyc_Arg_Clear_spec_struct _tmp256;_tmp256.tag=4;_tmp256.f1=& Cyc_pp_r;
_tmp256;});_tmp255;});_tmp253->f5=({const char*_tmp254="Ugly print";
_tag_dynforward(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,11));});_tmp253;});
_tmp180[34]=({struct _tuple14*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->f1=({
const char*_tmp252="-pp";_tag_dynforward(_tmp252,sizeof(char),_get_zero_arr_size(
_tmp252,4));});_tmp24D->f2=0;_tmp24D->f3=({const char*_tmp251="";_tag_dynforward(
_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,1));});_tmp24D->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F[0]=({
struct Cyc_Arg_Set_spec_struct _tmp250;_tmp250.tag=3;_tmp250.f1=& Cyc_pp_r;_tmp250;});
_tmp24F;});_tmp24D->f5=({const char*_tmp24E="Pretty print";_tag_dynforward(
_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,13));});_tmp24D;});_tmp180[33]=({
struct _tuple14*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247->f1=({const char*
_tmp24C="-ic";_tag_dynforward(_tmp24C,sizeof(char),_get_zero_arr_size(_tmp24C,4));});
_tmp247->f2=0;_tmp247->f3=({const char*_tmp24B="";_tag_dynforward(_tmp24B,sizeof(
char),_get_zero_arr_size(_tmp24B,1));});_tmp247->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Arg_Set_spec_struct
_tmp24A;_tmp24A.tag=3;_tmp24A.f1=& Cyc_ic_r;_tmp24A;});_tmp249;});_tmp247->f5=({
const char*_tmp248="Activate the link-checker";_tag_dynforward(_tmp248,sizeof(
char),_get_zero_arr_size(_tmp248,26));});_tmp247;});_tmp180[32]=({struct _tuple14*
_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241->f1=({const char*_tmp246="-stopafter-toc";
_tag_dynforward(_tmp246,sizeof(char),_get_zero_arr_size(_tmp246,15));});_tmp241->f2=
0;_tmp241->f3=({const char*_tmp245="";_tag_dynforward(_tmp245,sizeof(char),
_get_zero_arr_size(_tmp245,1));});_tmp241->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Arg_Set_spec_struct
_tmp244;_tmp244.tag=3;_tmp244.f1=& Cyc_toc_r;_tmp244;});_tmp243;});_tmp241->f5=({
const char*_tmp242="Stop after translation to C";_tag_dynforward(_tmp242,sizeof(
char),_get_zero_arr_size(_tmp242,28));});_tmp241;});_tmp180[31]=({struct _tuple14*
_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->f1=({const char*_tmp240="-stopafter-cf";
_tag_dynforward(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,14));});_tmp23B->f2=
0;_tmp23B->f3=({const char*_tmp23F="";_tag_dynforward(_tmp23F,sizeof(char),
_get_zero_arr_size(_tmp23F,1));});_tmp23B->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_Arg_Set_spec_struct
_tmp23E;_tmp23E.tag=3;_tmp23E.f1=& Cyc_cf_r;_tmp23E;});_tmp23D;});_tmp23B->f5=({
const char*_tmp23C="Stop after control-flow checking";_tag_dynforward(_tmp23C,
sizeof(char),_get_zero_arr_size(_tmp23C,33));});_tmp23B;});_tmp180[30]=({struct
_tuple14*_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235->f1=({const char*_tmp23A="-noprint";
_tag_dynforward(_tmp23A,sizeof(char),_get_zero_arr_size(_tmp23A,9));});_tmp235->f2=
0;_tmp235->f3=({const char*_tmp239="";_tag_dynforward(_tmp239,sizeof(char),
_get_zero_arr_size(_tmp239,1));});_tmp235->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237[0]=({struct Cyc_Arg_Set_spec_struct
_tmp238;_tmp238.tag=3;_tmp238.f1=& Cyc_noprint_r;_tmp238;});_tmp237;});_tmp235->f5=({
const char*_tmp236="Do not print output (when stopping early)";_tag_dynforward(
_tmp236,sizeof(char),_get_zero_arr_size(_tmp236,42));});_tmp235;});_tmp180[29]=({
struct _tuple14*_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F->f1=({const char*
_tmp234="-stopafter-tc";_tag_dynforward(_tmp234,sizeof(char),_get_zero_arr_size(
_tmp234,14));});_tmp22F->f2=0;_tmp22F->f3=({const char*_tmp233="";_tag_dynforward(
_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,1));});_tmp22F->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231[0]=({
struct Cyc_Arg_Set_spec_struct _tmp232;_tmp232.tag=3;_tmp232.f1=& Cyc_tc_r;_tmp232;});
_tmp231;});_tmp22F->f5=({const char*_tmp230="Stop after type checking";
_tag_dynforward(_tmp230,sizeof(char),_get_zero_arr_size(_tmp230,25));});_tmp22F;});
_tmp180[28]=({struct _tuple14*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->f1=({
const char*_tmp22E="-stopafter-parse";_tag_dynforward(_tmp22E,sizeof(char),
_get_zero_arr_size(_tmp22E,17));});_tmp229->f2=0;_tmp229->f3=({const char*_tmp22D="";
_tag_dynforward(_tmp22D,sizeof(char),_get_zero_arr_size(_tmp22D,1));});_tmp229->f4=(
void*)({struct Cyc_Arg_Set_spec_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));
_tmp22B[0]=({struct Cyc_Arg_Set_spec_struct _tmp22C;_tmp22C.tag=3;_tmp22C.f1=& Cyc_parseonly_r;
_tmp22C;});_tmp22B;});_tmp229->f5=({const char*_tmp22A="Stop after parsing";
_tag_dynforward(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,19));});_tmp229;});
_tmp180[27]=({struct _tuple14*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->f1=({
const char*_tmp228="-E";_tag_dynforward(_tmp228,sizeof(char),_get_zero_arr_size(
_tmp228,3));});_tmp223->f2=0;_tmp223->f3=({const char*_tmp227="";_tag_dynforward(
_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,1));});_tmp223->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225[0]=({
struct Cyc_Arg_Set_spec_struct _tmp226;_tmp226.tag=3;_tmp226.f1=& Cyc_stop_after_cpp_r;
_tmp226;});_tmp225;});_tmp223->f5=({const char*_tmp224="Stop after preprocessing";
_tag_dynforward(_tmp224,sizeof(char),_get_zero_arr_size(_tmp224,25));});_tmp223;});
_tmp180[26]=({struct _tuple14*_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D->f1=({
const char*_tmp222="-Wall";_tag_dynforward(_tmp222,sizeof(char),
_get_zero_arr_size(_tmp222,6));});_tmp21D->f2=0;_tmp21D->f3=({const char*_tmp221="";
_tag_dynforward(_tmp221,sizeof(char),_get_zero_arr_size(_tmp221,1));});_tmp21D->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp21F=_cycalloc(sizeof(*_tmp21F));
_tmp21F[0]=({struct Cyc_Arg_Unit_spec_struct _tmp220;_tmp220.tag=0;_tmp220.f1=Cyc_set_all_warnings;
_tmp220;});_tmp21F;});_tmp21D->f5=({const char*_tmp21E="Turn on all warnings";
_tag_dynforward(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,21));});_tmp21D;});
_tmp180[25]=({struct _tuple14*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->f1=({
const char*_tmp21C="-b";_tag_dynforward(_tmp21C,sizeof(char),_get_zero_arr_size(
_tmp21C,3));});_tmp217->f2=0;_tmp217->f3=({const char*_tmp21B="<arch>";
_tag_dynforward(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,7));});_tmp217->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp219=_cycalloc(sizeof(*_tmp219));
_tmp219[0]=({struct Cyc_Arg_String_spec_struct _tmp21A;_tmp21A.tag=5;_tmp21A.f1=
Cyc_set_target_arch;_tmp21A;});_tmp219;});_tmp217->f5=({const char*_tmp218="Set target architecture; implies -stopafter-toc";
_tag_dynforward(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,48));});_tmp217;});
_tmp180[24]=({struct _tuple14*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->f1=({
const char*_tmp216="-MT";_tag_dynforward(_tmp216,sizeof(char),_get_zero_arr_size(
_tmp216,4));});_tmp211->f2=0;_tmp211->f3=({const char*_tmp215=" <target>";
_tag_dynforward(_tmp215,sizeof(char),_get_zero_arr_size(_tmp215,10));});_tmp211->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp213=_cycalloc(sizeof(*_tmp213));
_tmp213[0]=({struct Cyc_Arg_String_spec_struct _tmp214;_tmp214.tag=5;_tmp214.f1=
Cyc_set_dependencies_target;_tmp214;});_tmp213;});_tmp211->f5=({const char*
_tmp212="Give target for dependencies";_tag_dynforward(_tmp212,sizeof(char),
_get_zero_arr_size(_tmp212,29));});_tmp211;});_tmp180[23]=({struct _tuple14*
_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B->f1=({const char*_tmp210="-MG";
_tag_dynforward(_tmp210,sizeof(char),_get_zero_arr_size(_tmp210,4));});_tmp20B->f2=
0;_tmp20B->f3=({const char*_tmp20F="";_tag_dynforward(_tmp20F,sizeof(char),
_get_zero_arr_size(_tmp20F,1));});_tmp20B->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp20E;_tmp20E.tag=1;_tmp20E.f1=Cyc_add_cpparg;_tmp20E;});_tmp20D;});_tmp20B->f5=({
const char*_tmp20C="When producing dependencies assume that missing files are generated";
_tag_dynforward(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,68));});_tmp20B;});
_tmp180[22]=({struct _tuple14*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->f1=({
const char*_tmp20A="-M";_tag_dynforward(_tmp20A,sizeof(char),_get_zero_arr_size(
_tmp20A,3));});_tmp205->f2=0;_tmp205->f3=({const char*_tmp209="";_tag_dynforward(
_tmp209,sizeof(char),_get_zero_arr_size(_tmp209,1));});_tmp205->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp208;_tmp208.tag=0;_tmp208.f1=Cyc_set_produce_dependencies;
_tmp208;});_tmp207;});_tmp205->f5=({const char*_tmp206="Produce dependencies";
_tag_dynforward(_tmp206,sizeof(char),_get_zero_arr_size(_tmp206,21));});_tmp205;});
_tmp180[21]=({struct _tuple14*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->f1=({
const char*_tmp204="-S";_tag_dynforward(_tmp204,sizeof(char),_get_zero_arr_size(
_tmp204,3));});_tmp1FF->f2=0;_tmp1FF->f3=({const char*_tmp203="";_tag_dynforward(
_tmp203,sizeof(char),_get_zero_arr_size(_tmp203,1));});_tmp1FF->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp202;_tmp202.tag=0;_tmp202.f1=Cyc_set_stop_after_asmfile;
_tmp202;});_tmp201;});_tmp1FF->f5=({const char*_tmp200="Stop after producing assembly code";
_tag_dynforward(_tmp200,sizeof(char),_get_zero_arr_size(_tmp200,35));});_tmp1FF;});
_tmp180[20]=({struct _tuple14*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9->f1=({
const char*_tmp1FE="-pa";_tag_dynforward(_tmp1FE,sizeof(char),_get_zero_arr_size(
_tmp1FE,4));});_tmp1F9->f2=0;_tmp1F9->f3=({const char*_tmp1FD="";_tag_dynforward(
_tmp1FD,sizeof(char),_get_zero_arr_size(_tmp1FD,1));});_tmp1F9->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=Cyc_set_pa;
_tmp1FC;});_tmp1FB;});_tmp1F9->f5=({const char*_tmp1FA="Compile for profiling with aprof";
_tag_dynforward(_tmp1FA,sizeof(char),_get_zero_arr_size(_tmp1FA,33));});_tmp1F9;});
_tmp180[19]=({struct _tuple14*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->f1=({
const char*_tmp1F8="-pg";_tag_dynforward(_tmp1F8,sizeof(char),_get_zero_arr_size(
_tmp1F8,4));});_tmp1F3->f2=0;_tmp1F3->f3=({const char*_tmp1F7="";_tag_dynforward(
_tmp1F7,sizeof(char),_get_zero_arr_size(_tmp1F7,1));});_tmp1F3->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=Cyc_set_pg;
_tmp1F6;});_tmp1F5;});_tmp1F3->f5=({const char*_tmp1F4="Compile for profiling with gprof";
_tag_dynforward(_tmp1F4,sizeof(char),_get_zero_arr_size(_tmp1F4,33));});_tmp1F3;});
_tmp180[18]=({struct _tuple14*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->f1=({
const char*_tmp1F2="-p";_tag_dynforward(_tmp1F2,sizeof(char),_get_zero_arr_size(
_tmp1F2,3));});_tmp1ED->f2=0;_tmp1ED->f3=({const char*_tmp1F1="";_tag_dynforward(
_tmp1F1,sizeof(char),_get_zero_arr_size(_tmp1F1,1));});_tmp1ED->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1F0;_tmp1F0.tag=1;_tmp1F0.f1=Cyc_add_ccarg;
_tmp1F0;});_tmp1EF;});_tmp1ED->f5=({const char*_tmp1EE="Compile for profiling with prof";
_tag_dynforward(_tmp1EE,sizeof(char),_get_zero_arr_size(_tmp1EE,32));});_tmp1ED;});
_tmp180[17]=({struct _tuple14*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->f1=({
const char*_tmp1EC="-g";_tag_dynforward(_tmp1EC,sizeof(char),_get_zero_arr_size(
_tmp1EC,3));});_tmp1E7->f2=0;_tmp1E7->f3=({const char*_tmp1EB="";_tag_dynforward(
_tmp1EB,sizeof(char),_get_zero_arr_size(_tmp1EB,1));});_tmp1E7->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1EA;_tmp1EA.tag=1;_tmp1EA.f1=Cyc_add_ccarg;
_tmp1EA;});_tmp1E9;});_tmp1E7->f5=({const char*_tmp1E8="Compile for debugging";
_tag_dynforward(_tmp1E8,sizeof(char),_get_zero_arr_size(_tmp1E8,22));});_tmp1E7;});
_tmp180[16]=({struct _tuple14*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->f1=({
const char*_tmp1E6="-fomit-frame-pointer";_tag_dynforward(_tmp1E6,sizeof(char),
_get_zero_arr_size(_tmp1E6,21));});_tmp1E1->f2=0;_tmp1E1->f3=({const char*_tmp1E5="";
_tag_dynforward(_tmp1E5,sizeof(char),_get_zero_arr_size(_tmp1E5,1));});_tmp1E1->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));
_tmp1E3[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1E4;_tmp1E4.tag=1;_tmp1E4.f1=Cyc_add_ccarg;
_tmp1E4;});_tmp1E3;});_tmp1E1->f5=({const char*_tmp1E2="Omit frame pointer";
_tag_dynforward(_tmp1E2,sizeof(char),_get_zero_arr_size(_tmp1E2,19));});_tmp1E1;});
_tmp180[15]=({struct _tuple14*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->f1=({
const char*_tmp1E0="-O3";_tag_dynforward(_tmp1E0,sizeof(char),_get_zero_arr_size(
_tmp1E0,4));});_tmp1DB->f2=0;_tmp1DB->f3=({const char*_tmp1DF="";_tag_dynforward(
_tmp1DF,sizeof(char),_get_zero_arr_size(_tmp1DF,1));});_tmp1DB->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1DE;_tmp1DE.tag=1;_tmp1DE.f1=Cyc_add_ccarg;
_tmp1DE;});_tmp1DD;});_tmp1DB->f5=({const char*_tmp1DC="Even more optimization";
_tag_dynforward(_tmp1DC,sizeof(char),_get_zero_arr_size(_tmp1DC,23));});_tmp1DB;});
_tmp180[14]=({struct _tuple14*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->f1=({
const char*_tmp1DA="-O2";_tag_dynforward(_tmp1DA,sizeof(char),_get_zero_arr_size(
_tmp1DA,4));});_tmp1D5->f2=0;_tmp1D5->f3=({const char*_tmp1D9="";_tag_dynforward(
_tmp1D9,sizeof(char),_get_zero_arr_size(_tmp1D9,1));});_tmp1D5->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1D8;_tmp1D8.tag=1;_tmp1D8.f1=Cyc_add_ccarg;
_tmp1D8;});_tmp1D7;});_tmp1D5->f5=({const char*_tmp1D6="A higher level of optimization";
_tag_dynforward(_tmp1D6,sizeof(char),_get_zero_arr_size(_tmp1D6,31));});_tmp1D5;});
_tmp180[13]=({struct _tuple14*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->f1=({
const char*_tmp1D4="-O";_tag_dynforward(_tmp1D4,sizeof(char),_get_zero_arr_size(
_tmp1D4,3));});_tmp1CF->f2=0;_tmp1CF->f3=({const char*_tmp1D3="";_tag_dynforward(
_tmp1D3,sizeof(char),_get_zero_arr_size(_tmp1D3,1));});_tmp1CF->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1D2;_tmp1D2.tag=1;_tmp1D2.f1=Cyc_add_ccarg;
_tmp1D2;});_tmp1D1;});_tmp1CF->f5=({const char*_tmp1D0="Optimize";_tag_dynforward(
_tmp1D0,sizeof(char),_get_zero_arr_size(_tmp1D0,9));});_tmp1CF;});_tmp180[12]=({
struct _tuple14*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->f1=({const char*
_tmp1CE="-O0";_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,4));});
_tmp1C9->f2=0;_tmp1C9->f3=({const char*_tmp1CD="";_tag_dynforward(_tmp1CD,sizeof(
char),_get_zero_arr_size(_tmp1CD,1));});_tmp1C9->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1CC;_tmp1CC.tag=1;_tmp1CC.f1=Cyc_add_ccarg;_tmp1CC;});_tmp1CB;});_tmp1C9->f5=({
const char*_tmp1CA="Don't optimize";_tag_dynforward(_tmp1CA,sizeof(char),
_get_zero_arr_size(_tmp1CA,15));});_tmp1C9;});_tmp180[11]=({struct _tuple14*
_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->f1=({const char*_tmp1C8="-s";
_tag_dynforward(_tmp1C8,sizeof(char),_get_zero_arr_size(_tmp1C8,3));});_tmp1C3->f2=
0;_tmp1C3->f3=({const char*_tmp1C7="";_tag_dynforward(_tmp1C7,sizeof(char),
_get_zero_arr_size(_tmp1C7,1));});_tmp1C3->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C6;_tmp1C6.tag=1;_tmp1C6.f1=Cyc_add_ccarg;_tmp1C6;});_tmp1C5;});_tmp1C3->f5=({
const char*_tmp1C4="Remove all symbol table and relocation info from executable";
_tag_dynforward(_tmp1C4,sizeof(char),_get_zero_arr_size(_tmp1C4,60));});_tmp1C3;});
_tmp180[10]=({struct _tuple14*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->f1=({
const char*_tmp1C2="-x";_tag_dynforward(_tmp1C2,sizeof(char),_get_zero_arr_size(
_tmp1C2,3));});_tmp1BD->f2=0;_tmp1BD->f3=({const char*_tmp1C1=" <language>";
_tag_dynforward(_tmp1C1,sizeof(char),_get_zero_arr_size(_tmp1C1,12));});_tmp1BD->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));
_tmp1BF[0]=({struct Cyc_Arg_String_spec_struct _tmp1C0;_tmp1C0.tag=5;_tmp1C0.f1=
Cyc_set_inputtype;_tmp1C0;});_tmp1BF;});_tmp1BD->f5=({const char*_tmp1BE="Specify <language> for the following input files";
_tag_dynforward(_tmp1BE,sizeof(char),_get_zero_arr_size(_tmp1BE,49));});_tmp1BD;});
_tmp180[9]=({struct _tuple14*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->f1=({
const char*_tmp1BC="-c";_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size(
_tmp1BC,3));});_tmp1B7->f2=0;_tmp1B7->f3=({const char*_tmp1BB="";_tag_dynforward(
_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,1));});_tmp1B7->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=Cyc_set_stop_after_objectfile;
_tmp1BA;});_tmp1B9;});_tmp1B7->f5=({const char*_tmp1B8="Produce an object file instead of an executable; do not link";
_tag_dynforward(_tmp1B8,sizeof(char),_get_zero_arr_size(_tmp1B8,61));});_tmp1B7;});
_tmp180[8]=({struct _tuple14*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->f1=({
const char*_tmp1B6="-m";_tag_dynforward(_tmp1B6,sizeof(char),_get_zero_arr_size(
_tmp1B6,3));});_tmp1B1->f2=1;_tmp1B1->f3=({const char*_tmp1B5="<option>";
_tag_dynforward(_tmp1B5,sizeof(char),_get_zero_arr_size(_tmp1B5,9));});_tmp1B1->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));
_tmp1B3[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1B4;_tmp1B4.tag=1;_tmp1B4.f1=Cyc_add_marg;
_tmp1B4;});_tmp1B3;});_tmp1B1->f5=({const char*_tmp1B2="GCC specific: pass machine-dependent flag on to gcc";
_tag_dynforward(_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,52));});_tmp1B1;});
_tmp180[7]=({struct _tuple14*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->f1=({
const char*_tmp1B0="-l";_tag_dynforward(_tmp1B0,sizeof(char),_get_zero_arr_size(
_tmp1B0,3));});_tmp1AB->f2=1;_tmp1AB->f3=({const char*_tmp1AF="<libname>";
_tag_dynforward(_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,10));});_tmp1AB->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));
_tmp1AD[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1AE;_tmp1AE.tag=1;_tmp1AE.f1=Cyc_add_libarg;
_tmp1AE;});_tmp1AD;});_tmp1AB->f5=({const char*_tmp1AC="Library file";
_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,13));});_tmp1AB;});
_tmp180[6]=({struct _tuple14*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->f1=({
const char*_tmp1AA="-L";_tag_dynforward(_tmp1AA,sizeof(char),_get_zero_arr_size(
_tmp1AA,3));});_tmp1A5->f2=1;_tmp1A5->f3=({const char*_tmp1A9="<dir>";
_tag_dynforward(_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,6));});_tmp1A5->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));
_tmp1A7[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1A8;_tmp1A8.tag=1;_tmp1A8.f1=Cyc_add_ccarg;
_tmp1A8;});_tmp1A7;});_tmp1A5->f5=({const char*_tmp1A6="Add to the list of directories for -l";
_tag_dynforward(_tmp1A6,sizeof(char),_get_zero_arr_size(_tmp1A6,38));});_tmp1A5;});
_tmp180[5]=({struct _tuple14*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->f1=({
const char*_tmp1A4="-I";_tag_dynforward(_tmp1A4,sizeof(char),_get_zero_arr_size(
_tmp1A4,3));});_tmp19F->f2=1;_tmp19F->f3=({const char*_tmp1A3="<dir>";
_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,6));});_tmp19F->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));
_tmp1A1[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1A2;_tmp1A2.tag=1;_tmp1A2.f1=Cyc_add_cpparg;
_tmp1A2;});_tmp1A1;});_tmp19F->f5=({const char*_tmp1A0="Add to the list of directories to search for include files";
_tag_dynforward(_tmp1A0,sizeof(char),_get_zero_arr_size(_tmp1A0,59));});_tmp19F;});
_tmp180[4]=({struct _tuple14*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->f1=({
const char*_tmp19E="-B";_tag_dynforward(_tmp19E,sizeof(char),_get_zero_arr_size(
_tmp19E,3));});_tmp199->f2=1;_tmp199->f3=({const char*_tmp19D="<file>";
_tag_dynforward(_tmp19D,sizeof(char),_get_zero_arr_size(_tmp19D,7));});_tmp199->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));
_tmp19B[0]=({struct Cyc_Arg_Flag_spec_struct _tmp19C;_tmp19C.tag=1;_tmp19C.f1=Cyc_add_cyclone_exec_path;
_tmp19C;});_tmp19B;});_tmp199->f5=({const char*_tmp19A="Add to the list of directories to search for compiler files";
_tag_dynforward(_tmp19A,sizeof(char),_get_zero_arr_size(_tmp19A,60));});_tmp199;});
_tmp180[3]=({struct _tuple14*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->f1=({
const char*_tmp198="-D";_tag_dynforward(_tmp198,sizeof(char),_get_zero_arr_size(
_tmp198,3));});_tmp193->f2=1;_tmp193->f3=({const char*_tmp197="<name>[=<value>]";
_tag_dynforward(_tmp197,sizeof(char),_get_zero_arr_size(_tmp197,17));});_tmp193->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp195=_cycalloc(sizeof(*_tmp195));
_tmp195[0]=({struct Cyc_Arg_Flag_spec_struct _tmp196;_tmp196.tag=1;_tmp196.f1=Cyc_add_cpparg;
_tmp196;});_tmp195;});_tmp193->f5=({const char*_tmp194="Pass definition to preprocessor";
_tag_dynforward(_tmp194,sizeof(char),_get_zero_arr_size(_tmp194,32));});_tmp193;});
_tmp180[2]=({struct _tuple14*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->f1=({
const char*_tmp192="-o";_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size(
_tmp192,3));});_tmp18D->f2=0;_tmp18D->f3=({const char*_tmp191=" <file>";
_tag_dynforward(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,8));});_tmp18D->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp18F=_cycalloc(sizeof(*_tmp18F));
_tmp18F[0]=({struct Cyc_Arg_String_spec_struct _tmp190;_tmp190.tag=5;_tmp190.f1=
Cyc_set_output_file;_tmp190;});_tmp18F;});_tmp18D->f5=({const char*_tmp18E="Set the output file name to <file>";
_tag_dynforward(_tmp18E,sizeof(char),_get_zero_arr_size(_tmp18E,35));});_tmp18D;});
_tmp180[1]=({struct _tuple14*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->f1=({
const char*_tmp18C="--version";_tag_dynforward(_tmp18C,sizeof(char),
_get_zero_arr_size(_tmp18C,10));});_tmp187->f2=0;_tmp187->f3=({const char*_tmp18B="";
_tag_dynforward(_tmp18B,sizeof(char),_get_zero_arr_size(_tmp18B,1));});_tmp187->f4=(
void*)({struct Cyc_Arg_Unit_spec_struct*_tmp189=_cycalloc(sizeof(*_tmp189));
_tmp189[0]=({struct Cyc_Arg_Unit_spec_struct _tmp18A;_tmp18A.tag=0;_tmp18A.f1=Cyc_print_version;
_tmp18A;});_tmp189;});_tmp187->f5=({const char*_tmp188="Print version information and exit";
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,35));});_tmp187;});
_tmp180[0]=({struct _tuple14*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->f1=({
const char*_tmp186="-v";_tag_dynforward(_tmp186,sizeof(char),_get_zero_arr_size(
_tmp186,3));});_tmp181->f2=0;_tmp181->f3=({const char*_tmp185="";_tag_dynforward(
_tmp185,sizeof(char),_get_zero_arr_size(_tmp185,1));});_tmp181->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=({
struct Cyc_Arg_Set_spec_struct _tmp184;_tmp184.tag=3;_tmp184.f1=& Cyc_v_r;_tmp184;});
_tmp183;});_tmp181->f5=({const char*_tmp182="Print compilation stages verbosely";
_tag_dynforward(_tmp182,sizeof(char),_get_zero_arr_size(_tmp182,35));});_tmp181;});((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp180,sizeof(struct _tuple14*),60));});struct _dynforward_ptr optstring=({const
char*_tmp17F="Options:";_tag_dynforward(_tmp17F,sizeof(char),_get_zero_arr_size(
_tmp17F,9));});Cyc_Arg_parse(options,Cyc_add_other,optstring,argv);if(Cyc_cyclone_files
== 0  && Cyc_ccargs == 0){({void*_tmp11A[0]={};Cyc_fprintf(Cyc_stderr,({const char*
_tmp11B="missing file\n";_tag_dynforward(_tmp11B,sizeof(char),_get_zero_arr_size(
_tmp11B,14));}),_tag_dynforward(_tmp11A,sizeof(void*),0));});exit(1);}if(Cyc_target_arch.curr
== ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)Cyc_target_arch=(struct
_dynforward_ptr)Cstring_to_string(Carch);{struct _dynforward_ptr
cyclone_exec_prefix=({char*_tmp17E=getenv((const char*)"CYCLONE_EXEC_PREFIX");
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,1));});if(
cyclone_exec_prefix.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));
_tmp11C->hd=({struct _dynforward_ptr*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[
0]=cyclone_exec_prefix;_tmp11D;});_tmp11C->tl=Cyc_cyclone_exec_path;_tmp11C;});{
struct _dynforward_ptr def_lib_path=(struct _dynforward_ptr)Cstring_to_string(
Cdef_lib_path);if(Cyc_strlen((struct _dynforward_ptr)def_lib_path)> 0)Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->hd=({struct
_dynforward_ptr*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=(struct
_dynforward_ptr)Cyc_Filename_concat(def_lib_path,({const char*_tmp120="cyc-lib";
_tag_dynforward(_tmp120,sizeof(char),_get_zero_arr_size(_tmp120,8));}));_tmp11F;});
_tmp11E->tl=Cyc_cyclone_exec_path;_tmp11E;});Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);{struct Cyc_List_List*
_tmp121=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);if(!Cyc_stop_after_cpp_r
 && Cyc_strlen((struct _dynforward_ptr)Cyc_cyc_include)== 0)Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,({const char*_tmp122="cyc_include.h";_tag_dynforward(
_tmp122,sizeof(char),_get_zero_arr_size(_tmp122,14));}));{struct Cyc_List_List*
_tmp123=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);
for(0;_tmp123 != 0;_tmp123=_tmp123->tl){Cyc_process_file(*((struct _dynforward_ptr*)
_tmp123->hd));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;Cyc_add_ccarg((
struct _dynforward_ptr)Cyc_strconcat(({const char*_tmp124="-L";_tag_dynforward(
_tmp124,sizeof(char),_get_zero_arr_size(_tmp124,3));}),(struct _dynforward_ptr)
def_lib_path));Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_ccargs);{struct _dynforward_ptr _tmp125=Cyc_str_sepstr(((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),({const char*_tmp17D=" ";_tag_dynforward(
_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,2));}));Cyc_libargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{struct _dynforward_ptr _tmp126=
Cyc_str_sepstr(({struct Cyc_List_List*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->hd=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp17A->tl=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct
_dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs);_tmp17A;}),({const char*_tmp17C=" ";_tag_dynforward(_tmp17C,sizeof(
char),_get_zero_arr_size(_tmp17C,2));}));struct Cyc_List_List*stdlib;struct
_dynforward_ptr stdlib_string;int _tmp127=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dynforward_ptr*)
_check_null(Cyc_output_file)),({const char*_tmp178=".a";_tag_dynforward(_tmp178,
sizeof(char),_get_zero_arr_size(_tmp178,3));}))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file)),({const char*_tmp179=".lib";
_tag_dynforward(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,5));}));if(
_tmp127){stdlib=0;stdlib_string=(struct _dynforward_ptr)({const char*_tmp128="";
_tag_dynforward(_tmp128,sizeof(char),_get_zero_arr_size(_tmp128,1));});}else{
struct _dynforward_ptr libcyc_flag;struct _dynforward_ptr nogc_filename;struct
_dynforward_ptr runtime_filename;struct _dynforward_ptr gc_filename;if(Cyc_pa_r){
libcyc_flag=({const char*_tmp129="-lcyc_a";_tag_dynforward(_tmp129,sizeof(char),
_get_zero_arr_size(_tmp129,8));});nogc_filename=({const char*_tmp12A="nogc_a.a";
_tag_dynforward(_tmp12A,sizeof(char),_get_zero_arr_size(_tmp12A,9));});
runtime_filename=({const char*_tmp12B="runtime_cyc_a.o";_tag_dynforward(_tmp12B,
sizeof(char),_get_zero_arr_size(_tmp12B,16));});}else{if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp12C="-lcyc_nocheck";_tag_dynforward(_tmp12C,sizeof(
char),_get_zero_arr_size(_tmp12C,14));});nogc_filename=({const char*_tmp12D="nogc_nocheck.a";
_tag_dynforward(_tmp12D,sizeof(char),_get_zero_arr_size(_tmp12D,15));});
runtime_filename=({const char*_tmp12E="runtime_cyc.o";_tag_dynforward(_tmp12E,
sizeof(char),_get_zero_arr_size(_tmp12E,14));});}else{if(Cyc_pg_r){libcyc_flag=({
const char*_tmp12F="-lcyc_pg";_tag_dynforward(_tmp12F,sizeof(char),
_get_zero_arr_size(_tmp12F,9));});runtime_filename=({const char*_tmp130="runtime_cyc_pg.o";
_tag_dynforward(_tmp130,sizeof(char),_get_zero_arr_size(_tmp130,17));});
nogc_filename=({const char*_tmp131="nogc_pg.a";_tag_dynforward(_tmp131,sizeof(
char),_get_zero_arr_size(_tmp131,10));});}else{libcyc_flag=({const char*_tmp132="-lcyc";
_tag_dynforward(_tmp132,sizeof(char),_get_zero_arr_size(_tmp132,6));});
nogc_filename=({const char*_tmp133="nogc.a";_tag_dynforward(_tmp133,sizeof(char),
_get_zero_arr_size(_tmp133,7));});runtime_filename=({const char*_tmp134="runtime_cyc.o";
_tag_dynforward(_tmp134,sizeof(char),_get_zero_arr_size(_tmp134,14));});}}}
gc_filename=({const char*_tmp135="gc.a";_tag_dynforward(_tmp135,sizeof(char),
_get_zero_arr_size(_tmp135,5));});{struct _dynforward_ptr _tmp136=Cyc_nogc_r?Cyc_do_find(
_tmp121,nogc_filename): Cyc_do_find(_tmp121,gc_filename);struct _dynforward_ptr
_tmp137=Cyc_do_find(_tmp121,runtime_filename);stdlib=0;stdlib_string=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp13C;_tmp13C.tag=0;_tmp13C.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp136);{struct Cyc_String_pa_struct
_tmp13B;_tmp13B.tag=0;_tmp13B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
libcyc_flag);{struct Cyc_String_pa_struct _tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp137);{void*_tmp138[3]={& _tmp13A,&
_tmp13B,& _tmp13C};Cyc_aprintf(({const char*_tmp139=" %s %s %s";_tag_dynforward(
_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,10));}),_tag_dynforward(_tmp138,
sizeof(void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmp13D;_push_handler(&
_tmp13D);{int _tmp13F=0;if(setjmp(_tmp13D.handler))_tmp13F=1;if(!_tmp13F){Cyc_ccargs=((
struct Cyc_List_List*(*)(int(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))
Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_libargs);{struct Cyc_List_List*_tmp140=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp127)_tmp140=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));
_tmp141->hd=Cyc_final_strptr;_tmp141->tl=_tmp140;_tmp141;});{struct Cyc_Interface_I*
_tmp142=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dynforward_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp140,_tmp140);if(_tmp142 == 0){({void*_tmp143[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp144="Error: interfaces incompatible\n";
_tag_dynforward(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,32));}),
_tag_dynforward(_tmp143,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp145=1;_npop_handler(0);return _tmp145;}}if(_tmp127){if(Cyc_output_file != 0){
struct _dynforward_ptr _tmp146=({struct Cyc_String_pa_struct _tmp14D;_tmp14D.tag=0;
_tmp14D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Filename_chop_extension(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp14B[1]={&
_tmp14D};Cyc_aprintf(({const char*_tmp14C="%s.cycio";_tag_dynforward(_tmp14C,
sizeof(char),_get_zero_arr_size(_tmp14C,9));}),_tag_dynforward(_tmp14B,sizeof(
void*),1));}});struct Cyc___cycFILE*_tmp147=Cyc_try_file_open((struct
_dynforward_ptr)_tmp146,({const char*_tmp149="wb";_tag_dynforward(_tmp149,sizeof(
char),_get_zero_arr_size(_tmp149,3));}),({const char*_tmp14A="interface object file";
_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,22));}));if(
_tmp147 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp148=1;
_npop_handler(0);return _tmp148;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_tmp142,(struct Cyc___cycFILE*)_tmp147);Cyc_file_close((struct Cyc___cycFILE*)
_tmp147);}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp142,({struct _tuple11*_tmp14E=_cycalloc(sizeof(*_tmp14E));
_tmp14E->f1=({const char*_tmp150="empty interface";_tag_dynforward(_tmp150,
sizeof(char),_get_zero_arr_size(_tmp150,16));});_tmp14E->f2=({const char*_tmp14F="global interface";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,17));});_tmp14E;}))){({
void*_tmp151[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp152="Error: some objects are still undefined\n";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,41));}),
_tag_dynforward(_tmp151,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp153=1;_npop_handler(0);return _tmp153;}}}}};_pop_handler();}else{void*
_tmp13E=(void*)_exn_thrown;void*_tmp155=_tmp13E;_LL36:;_LL37:{void*_tmp156=
_tmp155;struct _dynforward_ptr _tmp157;struct _dynforward_ptr _tmp158;struct
_dynforward_ptr _tmp159;_LL3B: if(*((void**)_tmp156)!= Cyc_Core_Failure)goto _LL3D;
_tmp157=((struct Cyc_Core_Failure_struct*)_tmp156)->f1;_LL3C:({struct Cyc_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp157);{void*_tmp15A[1]={& _tmp15C};Cyc_fprintf(Cyc_stderr,({const char*_tmp15B="Exception Core::Failure %s\n";
_tag_dynforward(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,28));}),
_tag_dynforward(_tmp15A,sizeof(void*),1));}});goto _LL3A;_LL3D: if(*((void**)
_tmp156)!= Cyc_Core_Impossible)goto _LL3F;_tmp158=((struct Cyc_Core_Impossible_struct*)
_tmp156)->f1;_LL3E:({struct Cyc_String_pa_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp158);{void*_tmp15D[1]={&
_tmp15F};Cyc_fprintf(Cyc_stderr,({const char*_tmp15E="Exception Core::Impossible %s\n";
_tag_dynforward(_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,31));}),
_tag_dynforward(_tmp15D,sizeof(void*),1));}});goto _LL3A;_LL3F: if(_tmp156 != Cyc_Dict_Absent)
goto _LL41;_LL40:({void*_tmp160[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp161="Exception Dict::Absent\n";
_tag_dynforward(_tmp161,sizeof(char),_get_zero_arr_size(_tmp161,24));}),
_tag_dynforward(_tmp160,sizeof(void*),0));});goto _LL3A;_LL41: if(*((void**)
_tmp156)!= Cyc_Core_Invalid_argument)goto _LL43;_tmp159=((struct Cyc_Core_Invalid_argument_struct*)
_tmp156)->f1;_LL42:({struct Cyc_String_pa_struct _tmp164;_tmp164.tag=0;_tmp164.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp159);{void*_tmp162[1]={&
_tmp164};Cyc_fprintf(Cyc_stderr,({const char*_tmp163="Exception Core::Invalid_argument %s\n";
_tag_dynforward(_tmp163,sizeof(char),_get_zero_arr_size(_tmp163,37));}),
_tag_dynforward(_tmp162,sizeof(void*),1));}});goto _LL3A;_LL43:;_LL44:({void*
_tmp165[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp166="Uncaught exception\n";
_tag_dynforward(_tmp166,sizeof(char),_get_zero_arr_size(_tmp166,20));}),
_tag_dynforward(_tmp165,sizeof(void*),0));});goto _LL3A;_LL3A:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL38:;_LL39:(void)_throw(_tmp155);_LL35:;}}}{
struct _dynforward_ptr outfile_str=({const char*_tmp177="";_tag_dynforward(_tmp177,
sizeof(char),_get_zero_arr_size(_tmp177,1));});if(Cyc_output_file != 0)
outfile_str=(struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp169;_tmp169.tag=
0;_tmp169.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string(*((
struct _dynforward_ptr*)_check_null(Cyc_output_file))));{void*_tmp167[1]={&
_tmp169};Cyc_aprintf(({const char*_tmp168=" -o %s";_tag_dynforward(_tmp168,
sizeof(char),_get_zero_arr_size(_tmp168,7));}),_tag_dynforward(_tmp167,sizeof(
void*),1));}});{struct _dynforward_ptr _tmp16A=({struct Cyc_String_pa_struct _tmp176;
_tmp176.tag=0;_tmp176.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp126);{
struct Cyc_String_pa_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdlib_string);{struct Cyc_String_pa_struct
_tmp174;_tmp174.tag=0;_tmp174.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp125);{struct Cyc_String_pa_struct _tmp173;_tmp173.tag=0;_tmp173.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)outfile_str);{struct Cyc_String_pa_struct
_tmp172;_tmp172.tag=0;_tmp172.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
comp);{void*_tmp170[5]={& _tmp172,& _tmp173,& _tmp174,& _tmp175,& _tmp176};Cyc_aprintf(({
const char*_tmp171="%s %s %s%s%s";_tag_dynforward(_tmp171,sizeof(char),
_get_zero_arr_size(_tmp171,13));}),_tag_dynforward(_tmp170,sizeof(void*),5));}}}}}});
if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp16D;_tmp16D.tag=0;_tmp16D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp16A);{void*_tmp16B[1]={& _tmp16D};Cyc_fprintf(
Cyc_stderr,({const char*_tmp16C="%s\n";_tag_dynforward(_tmp16C,sizeof(char),
_get_zero_arr_size(_tmp16C,4));}),_tag_dynforward(_tmp16B,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)
_untag_dynforward_ptr(_tmp16A,sizeof(char),1))!= 0){({void*_tmp16E[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp16F="Error: C compiler failed\n";_tag_dynforward(
_tmp16F,sizeof(char),_get_zero_arr_size(_tmp16F,26));}),_tag_dynforward(_tmp16E,
sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();return 1;}Cyc_remove_cfiles();
return Cyc_compile_failure?1: 0;}}}}}}}}}}
