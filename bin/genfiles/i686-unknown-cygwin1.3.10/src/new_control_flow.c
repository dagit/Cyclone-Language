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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(
struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int
Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_delete(
struct Cyc_Dict_Dict*,void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict*d);int Cyc_strptrcmp(struct _dynforward_ptr*
s1,struct _dynforward_ptr*s2);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();struct Cyc_Absyn_Rel_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct _dynforward_ptr*Cyc_Absyn_fieldname(int);int Cyc_Absyn_is_union_type(
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
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noreturn(void*);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_mt_place_set();int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict**
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict*s1,struct Cyc_Dict_Dict*s2,int
disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict*consumed;struct
Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(
void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*
f1,void*f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);struct
Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict**
all_changed,void*r);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict**all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);void*Cyc_CfFlowInfo_join_flow(struct Cyc_Dict_Dict**,void*,void*,
int);struct _tuple0 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_Dict_Dict**
all_changed,struct _tuple0 pr1,struct _tuple0 pr2,int);void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Dict_Dict**,void*,void*,struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict*);
void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};int Cyc_CfFlowInfo_contains_region(void*rgn,void*
t);void*Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,void*
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,void*f);
void*Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,void*
f);void*Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,void*f);
struct _tuple5{struct Cyc_CfFlowInfo_ConsumeInfo f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_save_consume_info(
void*f,int clear);void*Cyc_CfFlowInfo_restore_consume_info(void*f,struct Cyc_CfFlowInfo_ConsumeInfo
c);struct _dynforward_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct
Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;void*flow;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2.flow=(void*)((void*)0);_tmp2;});_tmp1;});_tmp0;})));}
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;
_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";
_tag_dynforward(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,37));});_tmp7;});
_tmp6;}));_LL0:;}struct Cyc_NewControlFlow_AnalEnv{int iterate_again;int
iteration_num;int in_try;void*tryflow;struct Cyc_Dict_Dict**all_changed;int
noreturn;struct Cyc_List_List*param_roots;};static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Stmt*);static void*Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Decl*);static struct
_tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,void*,
struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp9=Cyc_NewControlFlow_get_stmt_annot(
s);(void*)(_tmp9->flow=(void*)Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,(
void*)_tmp9->flow,1));_tmp9->visited=env->iteration_num;return _tmp9;}static void
Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,void*
new_flow){if(env->in_try)(void*)(env->tryflow=(void*)Cyc_CfFlowInfo_join_flow(
env->all_changed,new_flow,(void*)env->tryflow,1));}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,void*flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);void*_tmpB=Cyc_CfFlowInfo_join_flow(
env->all_changed,flow,(void*)_tmpA->flow,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmpB,(void*)_tmpA->flow)){(void*)(_tmpA->flow=(void*)_tmpB);if(_tmpA->visited == 
env->iteration_num)env->iterate_again=1;}}static void*Cyc_NewControlFlow_add_vars(
void*inflow,struct Cyc_List_List*vds,void*leafval,struct Cyc_Position_Segment*loc){
void*_tmpC=inflow;struct Cyc_Dict_Dict*_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_CfFlowInfo_ConsumeInfo
_tmpF;_LL6: if((int)_tmpC != 0)goto _LL8;_LL7: return(void*)0;_LL8: if(_tmpC <= (void*)
1)goto _LL5;if(*((int*)_tmpC)!= 0)goto _LL5;_tmpD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpC)->f1;_tmpE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpC)->f2;_tmpF=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpC)->f3;_LL9: for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp10=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp12;_tmp12.tag=0;_tmp12.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp12;});_tmp11;});
_tmpD=Cyc_Dict_insert(_tmpD,(void*)_tmp10,Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp14;_tmp14.tag=0;_tmp14.f1=_tmpD;_tmp14.f2=_tmpE;_tmp14.f3=_tmpF;_tmp14;});
_tmp13;});_LL5:;}struct _tuple6{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static void Cyc_NewControlFlow_remove_vars(void*outflow,struct Cyc_Dict_Dict**
out_unique_places,struct Cyc_Dict_Dict*old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp16;struct Cyc_Dict_Dict*_tmp17;struct _tuple5 _tmp15=Cyc_CfFlowInfo_save_consume_info(
outflow,0);_tmp16=_tmp15.f1;_tmp17=_tmp16.consumed;{struct _RegionHandle _tmp18=
_new_region("r");struct _RegionHandle*r=& _tmp18;_push_region(r);{struct Cyc_Iter_Iter
_tmp19=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))
Cyc_Dict_make_iter)(r,*((struct Cyc_Dict_Dict**)_check_null(out_unique_places)));
struct _tuple6 _tmp1A=*((struct _tuple6*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d))Cyc_Dict_rchoose)(r,*out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple6*))Cyc_Iter_next)(_tmp19,& _tmp1A)){struct Cyc_CfFlowInfo_Place*
_tmp1B=_tmp1A.f1;if(!((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_member)(_tmp17,_tmp1B))({struct Cyc_String_pa_struct _tmp1E;_tmp1E.tag=0;
_tmp1E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(
_tmp1B));{void*_tmp1C[1]={& _tmp1E};Cyc_Tcutil_terr(_tmp1A.f2,({const char*_tmp1D="Failed to consume unique variable %s";
_tag_dynforward(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,37));}),
_tag_dynforward(_tmp1C,sizeof(void*),1));}});}};_pop_region(r);}*
out_unique_places=old_unique_places;}static void*Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*inflow,
void*r){void*_tmp1F=inflow;struct Cyc_Dict_Dict*_tmp20;struct Cyc_List_List*_tmp21;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp22;_LLB: if((int)_tmp1F != 0)goto _LLD;_LLC:
return(void*)0;_LLD: if(_tmp1F <= (void*)1)goto _LLA;if(*((int*)_tmp1F)!= 0)goto
_LLA;_tmp20=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F)->f1;_tmp21=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F)->f2;_tmp22=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1F)->f3;_LLE: if(Cyc_CfFlowInfo_initlevel(_tmp20,r)!= (void*)2)({void*_tmp23[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp24="expression may not be fully initialized";
_tag_dynforward(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,40));}),
_tag_dynforward(_tmp23,sizeof(void*),0));});{struct Cyc_Dict_Dict*_tmp25=Cyc_CfFlowInfo_escape_deref(
_tmp20,env->all_changed,r);if(_tmp20 == _tmp25)return inflow;{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp26=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp27=_cycalloc(sizeof(*_tmp27));
_tmp27[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp28;_tmp28.tag=0;_tmp28.f1=
_tmp25;_tmp28.f2=_tmp21;_tmp28.f3=_tmp22;_tmp28;});_tmp27;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp26);return(void*)_tmp26;}}_LLA:;}struct _tuple7{void*f1;struct Cyc_List_List*
f2;};static struct _tuple7 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({
struct _tuple7 _tmp29;_tmp29.f1=inflow;_tmp29.f2=0;_tmp29;});if(es->tl == 0){void*
_tmp2B;void*_tmp2C;struct _tuple0 _tmp2A=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp2B=_tmp2A.f1;_tmp2C=_tmp2A.f2;return({struct
_tuple7 _tmp2D;_tmp2D.f1=_tmp2B;_tmp2D.f2=({struct Cyc_List_List*_tmp2E=_cycalloc(
sizeof(*_tmp2E));_tmp2E->hd=(void*)_tmp2C;_tmp2E->tl=0;_tmp2E;});_tmp2D;});}{
struct Cyc_Dict_Dict**outer_all_changed=env->all_changed;struct Cyc_Dict_Dict*
this_all_changed;void*old_inflow;void*outflow;struct Cyc_List_List*rvals;struct
Cyc_CfFlowInfo_ConsumeInfo _tmp30;void*_tmp31;struct _tuple5 _tmp2F=Cyc_CfFlowInfo_save_consume_info(
inflow,1);_tmp30=_tmp2F.f1;_tmp31=_tmp2F.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp47;_tmp47.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp47.may_consume=0;_tmp47;});int init_consume=0;do{this_all_changed=Cyc_CfFlowInfo_mt_place_set();
_tmp31=Cyc_CfFlowInfo_restore_consume_info(_tmp31,_tmp30);env->all_changed=({
struct Cyc_Dict_Dict**_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp32;});{void*_tmp34;void*_tmp35;struct _tuple0 _tmp33=Cyc_NewControlFlow_anal_Rexp(
env,_tmp31,(struct Cyc_Absyn_Exp*)es->hd);_tmp34=_tmp33.f1;_tmp35=_tmp33.f2;
outflow=_tmp34;rvals=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));
_tmp36->hd=(void*)_tmp35;_tmp36->tl=0;_tmp36;});this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict**)_check_null(env->all_changed)),0);if(
arg1_unconsumed){void*_tmp37=outflow;struct Cyc_Dict_Dict*_tmp38;struct Cyc_List_List*
_tmp39;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A;_LL10: if((int)_tmp37 != 0)goto _LL12;
_LL11: goto _LLF;_LL12: if(_tmp37 <= (void*)1)goto _LLF;if(*((int*)_tmp37)!= 0)goto
_LLF;_tmp38=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp37)->f1;_tmp39=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp37)->f2;_tmp3A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp37)->f3;_LL13: outflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3C;_tmp3C.tag=0;_tmp3C.f1=_tmp38;_tmp3C.f2=_tmp39;_tmp3C.f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3D;_tmp3D.consumed=_tmp3A.consumed;_tmp3D.may_consume=_tmp30.may_consume;
_tmp3D;});_tmp3C;});_tmp3B;});_LLF:;}{struct Cyc_List_List*es2=es->tl;for(0;es2 != 
0;es2=es2->tl){env->all_changed=({struct Cyc_Dict_Dict**_tmp3E=_cycalloc(sizeof(*
_tmp3E));_tmp3E[0]=Cyc_CfFlowInfo_mt_place_set();_tmp3E;});{void*_tmp40;void*
_tmp41;struct _tuple0 _tmp3F=Cyc_NewControlFlow_anal_Rexp(env,_tmp31,(struct Cyc_Absyn_Exp*)
es2->hd);_tmp40=_tmp3F.f1;_tmp41=_tmp3F.f2;rvals=({struct Cyc_List_List*_tmp42=
_cycalloc(sizeof(*_tmp42));_tmp42->hd=(void*)_tmp41;_tmp42->tl=rvals;_tmp42;});
outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Dict_Dict**)& this_all_changed,
outflow,_tmp40,this_all_changed,*((struct Cyc_Dict_Dict**)_check_null(env->all_changed)));
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict**)
_check_null(env->all_changed)),0);}}}{struct Cyc_CfFlowInfo_ConsumeInfo _tmp44;
struct _tuple5 _tmp43=Cyc_CfFlowInfo_save_consume_info(outflow,0);_tmp44=_tmp43.f1;
if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(_tmp31,
outflow_consume);else{old_inflow=_tmp31;}init_consume=1;outflow_consume=_tmp44;
_tmp31=Cyc_CfFlowInfo_join_flow(outer_all_changed,_tmp31,outflow,1);}}}while(!
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp31,old_inflow));if(outer_all_changed == 0)
env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict**_tmp45=_cycalloc(
sizeof(*_tmp45));_tmp45[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,
this_all_changed,0);_tmp45;});}Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple7 _tmp46;_tmp46.f1=outflow;_tmp46.f2=Cyc_List_imp_rev(rvals);
_tmp46;});}}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es,int arg1_unconsumed){void*_tmp49;struct Cyc_List_List*
_tmp4A;struct _tuple7 _tmp48=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,es,
arg1_unconsumed);_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;{void*_tmp4B=_tmp49;struct Cyc_Dict_Dict*
_tmp4C;_LL15: if(_tmp4B <= (void*)1)goto _LL17;if(*((int*)_tmp4B)!= 0)goto _LL17;
_tmp4C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B)->f1;_LL16: for(0;_tmp4A
!= 0;(_tmp4A=_tmp4A->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp4C,(void*)_tmp4A->hd)== (void*)0)({void*_tmp4D[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp4E="expression may not be initialized";_tag_dynforward(_tmp4E,sizeof(
char),_get_zero_arr_size(_tmp4E,34));}),_tag_dynforward(_tmp4D,sizeof(void*),0));});}
goto _LL14;_LL17: if((int)_tmp4B != 0)goto _LL14;_LL18: goto _LL14;_LL14:;}return({
struct _tuple0 _tmp4F;_tmp4F.f1=_tmp49;_tmp4F.f2=Cyc_CfFlowInfo_unknown_all;_tmp4F;});}
static void*Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp50=outflow;struct Cyc_Dict_Dict*
_tmp51;struct Cyc_List_List*_tmp52;struct Cyc_CfFlowInfo_ConsumeInfo _tmp53;_LL1A:
if((int)_tmp50 != 0)goto _LL1C;_LL1B: return outflow;_LL1C: if(_tmp50 <= (void*)1)goto
_LL19;if(*((int*)_tmp50)!= 0)goto _LL19;_tmp51=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp50)->f1;_tmp52=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp50)->f2;_tmp53=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp50)->f3;_LL1D: {void*_tmp54=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp55;_LL1F: if((int)_tmp54 != 0)
goto _LL21;_LL20: return outflow;_LL21: if(_tmp54 <= (void*)1)goto _LL1E;if(*((int*)
_tmp54)!= 0)goto _LL1E;_tmp55=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp54)->f1;
_LL22: {void*nzval=il == (void*)2?(void*)1:(void*)2;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp57;_tmp57.tag=0;_tmp57.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp51,env->all_changed,
_tmp55,nzval);_tmp57.f2=_tmp52;_tmp57.f3=_tmp53;_tmp57;});_tmp56;});}_LL1E:;}
_LL19:;}static struct _tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp58=outflow;
struct Cyc_Dict_Dict*_tmp59;struct Cyc_List_List*_tmp5A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp5B;_LL24: if((int)_tmp58 != 0)goto _LL26;_LL25: return({struct _tuple0 _tmp5C;
_tmp5C.f1=outflow;_tmp5C.f2=outflow;_tmp5C;});_LL26: if(_tmp58 <= (void*)1)goto
_LL23;if(*((int*)_tmp58)!= 0)goto _LL23;_tmp59=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp58)->f1;_tmp5A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp58)->f2;_tmp5B=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp58)->f3;_LL27: {void*_tmp5D=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp5E;_LL29: if((int)_tmp5D != 0)
goto _LL2B;_LL2A: return({struct _tuple0 _tmp5F;_tmp5F.f1=outflow;_tmp5F.f2=outflow;
_tmp5F;});_LL2B: if(_tmp5D <= (void*)1)goto _LL28;if(*((int*)_tmp5D)!= 0)goto _LL28;
_tmp5E=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp5D)->f1;_LL2C: {void*nzval=il == (
void*)2?(void*)1:(void*)2;return({struct _tuple0 _tmp60;_tmp60.f1=(void*)({struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp64;_tmp64.tag=0;_tmp64.f1=Cyc_CfFlowInfo_assign_place(
e->loc,_tmp59,env->all_changed,_tmp5E,nzval);_tmp64.f2=_tmp5A;_tmp64.f3=_tmp5B;
_tmp64;});_tmp63;});_tmp60.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp62;_tmp62.tag=0;_tmp62.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp59,env->all_changed,
_tmp5E,(void*)0);_tmp62.f2=_tmp5A;_tmp62.f3=_tmp5B;_tmp62;});_tmp61;});_tmp60;});}
_LL28:;}_LL23:;}static void*Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,void*flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){void*_tmp65=flow;
struct Cyc_Dict_Dict*_tmp66;struct Cyc_List_List*_tmp67;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp68;_LL2E: if((int)_tmp65 != 0)goto _LL30;_LL2F: return flow;_LL30: if(_tmp65 <= (
void*)1)goto _LL2D;if(*((int*)_tmp65)!= 0)goto _LL2D;_tmp66=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp65)->f1;_tmp67=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp65)->f2;_tmp68=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp65)->f3;_LL31: {void*_tmp69=r2;
struct Cyc_List_List*_tmp6A;_LL33: if(_tmp69 <= (void*)3)goto _LL35;if(*((int*)
_tmp69)!= 3)goto _LL35;_tmp6A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp69)->f1;
_LL34: {void*_tmp6B=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp6C;_LL38: if((int)_tmp6B != 0)goto _LL3A;_LL39: return flow;_LL3A: if(_tmp6B <= (
void*)1)goto _LL37;if(*((int*)_tmp6B)!= 0)goto _LL37;_tmp6C=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp6B)->f1;_LL3B: {struct Cyc_List_List*new_cl;{void*_tmp6D=r1;struct Cyc_List_List*
_tmp6E;void*_tmp6F;_LL3D: if(_tmp6D <= (void*)3)goto _LL41;if(*((int*)_tmp6D)!= 3)
goto _LL3F;_tmp6E=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp6D)->f1;_LL3E: new_cl=
_tmp6E;goto _LL3C;_LL3F: if(*((int*)_tmp6D)!= 0)goto _LL41;_tmp6F=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp6D)->f1;if((int)_tmp6F != 2)goto _LL41;_LL40:
goto _LL42;_LL41: if((int)_tmp6D != 0)goto _LL43;_LL42: goto _LL44;_LL43: if((int)
_tmp6D != 1)goto _LL45;_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46: return flow;_LL3C:;}
for(0;_tmp6A != 0;_tmp6A=_tmp6A->tl){void*new_cmp;{struct _tuple0 _tmp71=({struct
_tuple0 _tmp70;_tmp70.f1=p;_tmp70.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp6A->hd)->cmp;
_tmp70;});void*_tmp72;void*_tmp73;void*_tmp74;void*_tmp75;_LL48: _tmp72=_tmp71.f1;
if((int)_tmp72 != 8)goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp73=_tmp71.f2;if((int)
_tmp73 != 8)goto _LL4C;_LL4B: new_cmp=(void*)8;goto _LL47;_LL4C: _tmp74=_tmp71.f1;if((
int)_tmp74 != 5)goto _LL4E;_tmp75=_tmp71.f2;if((int)_tmp75 != 5)goto _LL4E;_LL4D:
new_cmp=(void*)5;goto _LL47;_LL4E:;_LL4F: new_cmp=(void*)10;goto _LL47;_LL47:;}
new_cl=({struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->hd=({
struct Cyc_CfFlowInfo_TagCmp*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->cmp=(void*)
new_cmp;_tmp77->bd=(void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp6A->hd)->bd);
_tmp77;});_tmp76->tl=new_cl;_tmp76;});}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp79;_tmp79.tag=0;_tmp79.f1=Cyc_CfFlowInfo_assign_place(e1->loc,_tmp66,env->all_changed,
_tmp6C,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmp7A=_cycalloc(sizeof(*
_tmp7A));_tmp7A[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmp7B;_tmp7B.tag=3;
_tmp7B.f1=new_cl;_tmp7B;});_tmp7A;}));_tmp79.f2=_tmp67;_tmp79.f3=_tmp68;_tmp79;});
_tmp78;});}_LL37:;}_LL35:;_LL36: return flow;_LL32:;}_LL2D:;}static struct _tuple0
Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,
void*f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp7C=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7D;void*
_tmp7E;struct Cyc_Absyn_PtrAtts _tmp7F;struct Cyc_Absyn_Conref*_tmp80;struct Cyc_Absyn_Conref*
_tmp81;_LL51: if(_tmp7C <= (void*)4)goto _LL53;if(*((int*)_tmp7C)!= 4)goto _LL53;
_tmp7D=((struct Cyc_Absyn_PointerType_struct*)_tmp7C)->f1;_tmp7E=(void*)_tmp7D.elt_typ;
_tmp7F=_tmp7D.ptr_atts;_tmp80=_tmp7F.bounds;_tmp81=_tmp7F.zero_term;_LL52: {void*
_tmp82=f;struct Cyc_Dict_Dict*_tmp83;struct Cyc_List_List*_tmp84;_LL56: if((int)
_tmp82 != 0)goto _LL58;_LL57: return({struct _tuple0 _tmp85;_tmp85.f1=f;_tmp85.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp7E,Cyc_CfFlowInfo_unknown_all);_tmp85;});_LL58: if(_tmp82 <= (void*)1)goto
_LL55;if(*((int*)_tmp82)!= 0)goto _LL55;_tmp83=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp82)->f1;_tmp84=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp82)->f2;_LL59:
if(Cyc_Tcutil_is_bound_one(_tmp80)){void*_tmp86=r;struct Cyc_CfFlowInfo_Place*
_tmp87;void*_tmp88;_LL5B: if((int)_tmp86 != 1)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((
int)_tmp86 != 2)goto _LL5F;_LL5E:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp8A;_tmp8A.tag=Cyc_CfFlowInfo_NotZero;_tmp8A.f1=_tmp84;_tmp8A;});_tmp89;})));
goto _LL5A;_LL5F: if(_tmp86 <= (void*)3)goto _LL61;if(*((int*)_tmp86)!= 2)goto _LL61;
_tmp87=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp86)->f1;_LL60:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp8B=_cycalloc(sizeof(*
_tmp8B));_tmp8B[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp8C;_tmp8C.tag=Cyc_CfFlowInfo_NotZero;
_tmp8C.f1=_tmp84;_tmp8C;});_tmp8B;})));return({struct _tuple0 _tmp8D;_tmp8D.f1=f;
_tmp8D.f2=Cyc_CfFlowInfo_lookup_place(_tmp83,_tmp87);_tmp8D;});_LL61: if((int)
_tmp86 != 0)goto _LL63;_LL62:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple0 _tmp8E;_tmp8E.f1=(void*)0;_tmp8E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp7E,Cyc_CfFlowInfo_unknown_all);_tmp8E;});_LL63: if(_tmp86 <= (void*)3)goto
_LL65;if(*((int*)_tmp86)!= 0)goto _LL65;_tmp88=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp86)->f1;_LL64: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp88);goto _LL66;
_LL65:;_LL66:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp90;_tmp90.tag=Cyc_CfFlowInfo_UnknownZ;_tmp90.f1=_tmp84;_tmp90;});_tmp8F;})));
_LL5A:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp92;_tmp92.tag=Cyc_CfFlowInfo_UnknownZ;_tmp92.f1=_tmp84;_tmp92;});_tmp91;})));}{
void*_tmp93=Cyc_CfFlowInfo_initlevel(_tmp83,r);_LL68: if((int)_tmp93 != 0)goto
_LL6A;_LL69:({void*_tmp94[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp95="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp95,sizeof(char),_get_zero_arr_size(_tmp95,46));}),
_tag_dynforward(_tmp94,sizeof(void*),0));});goto _LL6B;_LL6A: if((int)_tmp93 != 2)
goto _LL6C;_LL6B: return({struct _tuple0 _tmp96;_tmp96.f1=f;_tmp96.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp7E,Cyc_CfFlowInfo_unknown_all);_tmp96;});_LL6C: if((int)_tmp93 != 1)goto _LL67;
_LL6D: return({struct _tuple0 _tmp97;_tmp97.f1=f;_tmp97.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp7E,Cyc_CfFlowInfo_unknown_none);_tmp97;});_LL67:;}_LL55:;}_LL53:;_LL54:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp98=_cycalloc(sizeof(*_tmp98));
_tmp98[0]=({struct Cyc_Core_Impossible_struct _tmp99;_tmp99.tag=Cyc_Core_Impossible;
_tmp99.f1=({const char*_tmp9A="right deref of non-pointer-type";_tag_dynforward(
_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,32));});_tmp99;});_tmp98;}));_LL50:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp9B=(void*)e1->r;
void*_tmp9C;struct Cyc_Absyn_Vardecl*_tmp9D;void*_tmp9E;struct Cyc_Absyn_Vardecl*
_tmp9F;void*_tmpA0;struct Cyc_Absyn_Vardecl*_tmpA1;void*_tmpA2;struct Cyc_Absyn_Vardecl*
_tmpA3;_LL6F: if(*((int*)_tmp9B)!= 1)goto _LL71;_tmp9C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9B)->f2;if(_tmp9C <= (void*)1)goto _LL71;if(*((int*)_tmp9C)!= 4)goto _LL71;
_tmp9D=((struct Cyc_Absyn_Pat_b_struct*)_tmp9C)->f1;_LL70: _tmp9F=_tmp9D;goto _LL72;
_LL71: if(*((int*)_tmp9B)!= 1)goto _LL73;_tmp9E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9B)->f2;if(_tmp9E <= (void*)1)goto _LL73;if(*((int*)_tmp9E)!= 3)goto _LL73;
_tmp9F=((struct Cyc_Absyn_Local_b_struct*)_tmp9E)->f1;_LL72: _tmpA1=_tmp9F;goto
_LL74;_LL73: if(*((int*)_tmp9B)!= 1)goto _LL75;_tmpA0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9B)->f2;if(_tmpA0 <= (void*)1)goto _LL75;if(*((int*)_tmpA0)!= 2)goto _LL75;
_tmpA1=((struct Cyc_Absyn_Param_b_struct*)_tmpA0)->f1;_LL74: _tmpA3=_tmpA1;goto
_LL76;_LL75: if(*((int*)_tmp9B)!= 1)goto _LL77;_tmpA2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp9B)->f2;if(_tmpA2 <= (void*)1)goto _LL77;if(*((int*)_tmpA2)!= 0)goto _LL77;
_tmpA3=((struct Cyc_Absyn_Global_b_struct*)_tmpA2)->f1;_LL76: if(!_tmpA3->escapes){
void*_tmpA4=(void*)e2->r;void*_tmpA5;struct Cyc_Absyn_Vardecl*_tmpA6;void*_tmpA7;
struct Cyc_Absyn_Vardecl*_tmpA8;void*_tmpA9;struct Cyc_Absyn_Vardecl*_tmpAA;void*
_tmpAB;struct Cyc_Absyn_Vardecl*_tmpAC;_LL7A: if(*((int*)_tmpA4)!= 1)goto _LL7C;
_tmpA5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA4)->f2;if(_tmpA5 <= (void*)1)
goto _LL7C;if(*((int*)_tmpA5)!= 4)goto _LL7C;_tmpA6=((struct Cyc_Absyn_Pat_b_struct*)
_tmpA5)->f1;_LL7B: _tmpA8=_tmpA6;goto _LL7D;_LL7C: if(*((int*)_tmpA4)!= 1)goto _LL7E;
_tmpA7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA4)->f2;if(_tmpA7 <= (void*)1)
goto _LL7E;if(*((int*)_tmpA7)!= 3)goto _LL7E;_tmpA8=((struct Cyc_Absyn_Local_b_struct*)
_tmpA7)->f1;_LL7D: _tmpAA=_tmpA8;goto _LL7F;_LL7E: if(*((int*)_tmpA4)!= 1)goto _LL80;
_tmpA9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA4)->f2;if(_tmpA9 <= (void*)1)
goto _LL80;if(*((int*)_tmpA9)!= 2)goto _LL80;_tmpAA=((struct Cyc_Absyn_Param_b_struct*)
_tmpA9)->f1;_LL7F: _tmpAC=_tmpAA;goto _LL81;_LL80: if(*((int*)_tmpA4)!= 1)goto _LL82;
_tmpAB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA4)->f2;if(_tmpAB <= (void*)1)
goto _LL82;if(*((int*)_tmpAB)!= 0)goto _LL82;_tmpAC=((struct Cyc_Absyn_Global_b_struct*)
_tmpAB)->f1;_LL81: if(!_tmpAC->escapes){int found=0;{struct Cyc_List_List*_tmpAD=
relns;for(0;_tmpAD != 0;_tmpAD=_tmpAD->tl){struct Cyc_CfFlowInfo_Reln*_tmpAE=(
struct Cyc_CfFlowInfo_Reln*)_tmpAD->hd;if(_tmpAE->vd == _tmpAC){void*_tmpAF=(void*)
_tmpAE->rop;struct Cyc_Absyn_Vardecl*_tmpB0;_LL85: if(*((int*)_tmpAF)!= 2)goto
_LL87;_tmpB0=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmpAF)->f1;if(!(_tmpB0 == 
_tmpA3))goto _LL87;_LL86: return relns;_LL87:;_LL88: continue;_LL84:;}}}if(!found)
return({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->hd=({
struct Cyc_CfFlowInfo_Reln*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->vd=_tmpAC;
_tmpB2->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmpB3=
_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmpB4;_tmpB4.tag=2;_tmpB4.f1=_tmpA3;_tmpB4;});_tmpB3;}));_tmpB2;});_tmpB1->tl=
relns;_tmpB1;});}return relns;_LL82:;_LL83: return relns;_LL79:;}else{return relns;}
_LL77:;_LL78: return relns;_LL6E:;}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*
relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{void*_tmpB5=inflow;struct Cyc_Dict_Dict*
_tmpB6;struct Cyc_List_List*_tmpB7;struct Cyc_CfFlowInfo_ConsumeInfo _tmpB8;_LL8A:
if((int)_tmpB5 != 0)goto _LL8C;_LL8B: return({struct _tuple0 _tmpB9;_tmpB9.f1=(void*)
0;_tmpB9.f2=Cyc_CfFlowInfo_unknown_all;_tmpB9;});_LL8C: if(_tmpB5 <= (void*)1)goto
_LL89;if(*((int*)_tmpB5)!= 0)goto _LL89;_tmpB6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpB5)->f1;_tmpB7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpB5)->f2;_tmpB8=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpB5)->f3;_LL8D: d=_tmpB6;relns=_tmpB7;
cinfo=_tmpB8;_LL89:;}{void*_tmpBA=(void*)e->r;struct Cyc_Absyn_Exp*_tmpBB;void*
_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Exp*
_tmpBF;void*_tmpC0;void*_tmpC1;int _tmpC2;void*_tmpC3;void*_tmpC4;struct Cyc_List_List*
_tmpC5;void*_tmpC6;void*_tmpC7;struct Cyc_Absyn_Vardecl*_tmpC8;void*_tmpC9;struct
Cyc_Absyn_Vardecl*_tmpCA;void*_tmpCB;struct Cyc_Absyn_Vardecl*_tmpCC;void*_tmpCD;
struct Cyc_List_List*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;
struct Cyc_Core_Opt*_tmpD1;struct Cyc_Core_Opt _tmpD2;struct Cyc_Absyn_Exp*_tmpD3;
struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Core_Opt*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;
struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;
struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_List_List*_tmpDB;struct Cyc_Absyn_MallocInfo
_tmpDC;int _tmpDD;struct Cyc_Absyn_Exp*_tmpDE;void**_tmpDF;struct Cyc_Absyn_Exp*
_tmpE0;int _tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*
_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct
_dynforward_ptr*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct _dynforward_ptr*_tmpE9;
struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;
struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;
struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;
struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_Tuniondecl*_tmpF4;struct Cyc_List_List*
_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*
_tmpF8;struct Cyc_Absyn_Vardecl*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*
_tmpFB;int _tmpFC;struct Cyc_Absyn_Stmt*_tmpFD;void*_tmpFE;_LL8F: if(*((int*)_tmpBA)
!= 15)goto _LL91;_tmpBB=((struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f2;_tmpBC=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f4;if((int)_tmpBC != 2)goto _LL91;_LL90: {
void*_tmp100;void*_tmp101;struct _tuple0 _tmpFF=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpBB);_tmp100=_tmpFF.f1;_tmp101=_tmpFF.f2;{void*_tmp103;void*_tmp104;
struct _tuple0 _tmp102=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp100,_tmpBB,
_tmp101);_tmp103=_tmp102.f1;_tmp104=_tmp102.f2;return({struct _tuple0 _tmp105;
_tmp105.f1=_tmp103;_tmp105.f2=_tmp101;_tmp105;});}}_LL91: if(*((int*)_tmpBA)!= 15)
goto _LL93;_tmpBD=((struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f2;_LL92: _tmpBE=
_tmpBD;goto _LL94;_LL93: if(*((int*)_tmpBA)!= 13)goto _LL95;_tmpBE=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpBA)->f1;_LL94: _tmpBF=_tmpBE;goto _LL96;_LL95: if(*((int*)_tmpBA)!= 14)goto
_LL97;_tmpBF=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpBA)->f1;_LL96: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBF);_LL97: if(*((int*)_tmpBA)!= 0)goto _LL99;_tmpC0=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmpBA)->f1;if((int)_tmpC0 != 0)goto _LL99;_LL98: goto
_LL9A;_LL99: if(*((int*)_tmpBA)!= 0)goto _LL9B;_tmpC1=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpBA)->f1;if(_tmpC1 <= (void*)1)goto _LL9B;if(*((int*)_tmpC1)!= 2)goto _LL9B;
_tmpC2=((struct Cyc_Absyn_Int_c_struct*)_tmpC1)->f2;if(_tmpC2 != 0)goto _LL9B;_LL9A:
return({struct _tuple0 _tmp106;_tmp106.f1=inflow;_tmp106.f2=(void*)0;_tmp106;});
_LL9B: if(*((int*)_tmpBA)!= 0)goto _LL9D;_tmpC3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpBA)->f1;if(_tmpC3 <= (void*)1)goto _LL9D;if(*((int*)_tmpC3)!= 2)goto _LL9D;
_LL9C: goto _LL9E;_LL9D: if(*((int*)_tmpBA)!= 1)goto _LL9F;_tmpC4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f2;if(_tmpC4 <= (void*)1)goto _LL9F;if(*((int*)_tmpC4)!= 1)goto _LL9F;
_LL9E: return({struct _tuple0 _tmp107;_tmp107.f1=inflow;_tmp107.f2=(void*)1;_tmp107;});
_LL9F: if(*((int*)_tmpBA)!= 32)goto _LLA1;_tmpC5=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpBA)->f1;if(_tmpC5 != 0)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if(*((int*)_tmpBA)!= 0)
goto _LLA3;_LLA2: goto _LLA4;_LLA3: if(*((int*)_tmpBA)!= 19)goto _LLA5;_LLA4: goto
_LLA6;_LLA5: if(*((int*)_tmpBA)!= 18)goto _LLA7;_LLA6: goto _LLA8;_LLA7: if(*((int*)
_tmpBA)!= 20)goto _LLA9;_LLA8: goto _LLAA;_LLA9: if(*((int*)_tmpBA)!= 21)goto _LLAB;
_LLAA: goto _LLAC;_LLAB: if(*((int*)_tmpBA)!= 34)goto _LLAD;_LLAC: goto _LLAE;_LLAD:
if(*((int*)_tmpBA)!= 33)goto _LLAF;_LLAE: return({struct _tuple0 _tmp108;_tmp108.f1=
inflow;_tmp108.f2=Cyc_CfFlowInfo_unknown_all;_tmp108;});_LLAF: if(*((int*)_tmpBA)
!= 1)goto _LLB1;_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(
_tmpC6 <= (void*)1)goto _LLB1;if(*((int*)_tmpC6)!= 0)goto _LLB1;_LLB0: return({
struct _tuple0 _tmp109;_tmp109.f1=inflow;_tmp109.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp109;});_LLB1: if(*((int*)_tmpBA)!= 1)goto _LLB3;_tmpC7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f2;if(_tmpC7 <= (void*)1)goto _LLB3;if(*((int*)_tmpC7)!= 2)goto _LLB3;
_tmpC8=((struct Cyc_Absyn_Param_b_struct*)_tmpC7)->f1;_LLB2: _tmpCA=_tmpC8;goto
_LLB4;_LLB3: if(*((int*)_tmpBA)!= 1)goto _LLB5;_tmpC9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f2;if(_tmpC9 <= (void*)1)goto _LLB5;if(*((int*)_tmpC9)!= 3)goto _LLB5;
_tmpCA=((struct Cyc_Absyn_Local_b_struct*)_tmpC9)->f1;_LLB4: _tmpCC=_tmpCA;goto
_LLB6;_LLB5: if(*((int*)_tmpBA)!= 1)goto _LLB7;_tmpCB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f2;if(_tmpCB <= (void*)1)goto _LLB7;if(*((int*)_tmpCB)!= 4)goto _LLB7;
_tmpCC=((struct Cyc_Absyn_Pat_b_struct*)_tmpCB)->f1;_LLB6: if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)_tmpCC->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp10A=({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[
0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp111;_tmp111.tag=0;_tmp111.f1=_tmpCC;
_tmp111;});_tmp110;});struct Cyc_CfFlowInfo_Place*_tmp10B=({struct Cyc_CfFlowInfo_Place*
_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->root=(void*)((void*)_tmp10A);
_tmp10F->fields=0;_tmp10F;});cinfo.may_consume=((struct Cyc_List_List*(*)(int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=_tmp10B;_tmp10C->tl=0;_tmp10C;}),
cinfo.may_consume);inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp10E;_tmp10E.tag=0;_tmp10E.f1=d;_tmp10E.f2=relns;_tmp10E.f3=cinfo;_tmp10E;});
_tmp10D;});}return({struct _tuple0 _tmp112;_tmp112.f1=inflow;_tmp112.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp113=_cycalloc(sizeof(*_tmp113));
_tmp113[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp114;_tmp114.tag=0;_tmp114.f1=
_tmpCC;_tmp114;});_tmp113;}));_tmp112;});_LLB7: if(*((int*)_tmpBA)!= 3)goto _LLB9;
_tmpCD=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmpBA)->f1;_tmpCE=((struct Cyc_Absyn_Primop_e_struct*)
_tmpBA)->f2;_LLB8: {void*_tmp116;void*_tmp117;struct _tuple0 _tmp115=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpCE,0);_tmp116=_tmp115.f1;_tmp117=_tmp115.f2;{void*_tmp118=_tmpCD;
_LLF2: if((int)_tmp118 != 0)goto _LLF4;_LLF3: goto _LLF5;_LLF4: if((int)_tmp118 != 2)
goto _LLF6;_LLF5: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpCE))->hd)->loc,_tmp116);goto _LLF1;_LLF6:;
_LLF7: _tmp116=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpCE))->hd)->loc,_tmp116);goto _LLF1;_LLF1:;}
return({struct _tuple0 _tmp119;_tmp119.f1=_tmp116;_tmp119.f2=_tmp117;_tmp119;});}
_LLB9: if(*((int*)_tmpBA)!= 5)goto _LLBB;_tmpCF=((struct Cyc_Absyn_Increment_e_struct*)
_tmpBA)->f1;_LLBA: {void*_tmp11C;struct _tuple0 _tmp11B=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmp11A[1];_tmp11A[0]=_tmpCF;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp11A,sizeof(struct Cyc_Absyn_Exp*),
1));}),0);_tmp11C=_tmp11B.f1;Cyc_CfFlowInfo_check_unique_rvals(_tmpCF->loc,
_tmp11C);{void*_tmp11E;struct _tuple0 _tmp11D=Cyc_NewControlFlow_anal_Lexp(env,
_tmp11C,_tmpCF);_tmp11E=_tmp11D.f2;{struct _tuple0 _tmp120=({struct _tuple0 _tmp11F;
_tmp11F.f1=_tmp11E;_tmp11F.f2=_tmp11C;_tmp11F;});void*_tmp121;struct Cyc_CfFlowInfo_Place*
_tmp122;void*_tmp123;struct Cyc_Dict_Dict*_tmp124;struct Cyc_List_List*_tmp125;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp126;_LLF9: _tmp121=_tmp120.f1;if(_tmp121 <= (
void*)1)goto _LLFB;if(*((int*)_tmp121)!= 0)goto _LLFB;_tmp122=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp121)->f1;_tmp123=_tmp120.f2;if(_tmp123 <= (void*)1)goto _LLFB;if(*((int*)
_tmp123)!= 0)goto _LLFB;_tmp124=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp123)->f1;_tmp125=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp123)->f2;
_tmp126=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp123)->f3;_LLFA: _tmp125=
Cyc_CfFlowInfo_reln_kill_exp(_tmp125,_tmpCF);_tmp11C=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp128;_tmp128.tag=0;_tmp128.f1=Cyc_CfFlowInfo_assign_place(_tmpCF->loc,_tmp124,
env->all_changed,_tmp122,Cyc_CfFlowInfo_unknown_all);_tmp128.f2=_tmp125;_tmp128.f3=
_tmp126;_tmp128;});_tmp127;});goto _LLF8;_LLFB:;_LLFC: goto _LLF8;_LLF8:;}return({
struct _tuple0 _tmp129;_tmp129.f1=_tmp11C;_tmp129.f2=Cyc_CfFlowInfo_unknown_all;
_tmp129;});}}_LLBB: if(*((int*)_tmpBA)!= 4)goto _LLBD;_tmpD0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBA)->f1;_tmpD1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f2;if(_tmpD1 == 
0)goto _LLBD;_tmpD2=*_tmpD1;_tmpD3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f3;
_LLBC: {void*_tmp12C;struct _tuple0 _tmp12B=Cyc_NewControlFlow_anal_use_ints(env,
inflow,({struct Cyc_Absyn_Exp*_tmp12A[2];_tmp12A[1]=_tmpD3;_tmp12A[0]=_tmpD0;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp12A,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp12C=_tmp12B.f1;{void*_tmp12E;
struct _tuple0 _tmp12D=Cyc_NewControlFlow_anal_Lexp(env,_tmp12C,e);_tmp12E=_tmp12D.f2;
_tmp12C=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp12C);{struct _tuple0
_tmp130=({struct _tuple0 _tmp12F;_tmp12F.f1=_tmp12E;_tmp12F.f2=_tmp12C;_tmp12F;});
void*_tmp131;struct Cyc_CfFlowInfo_Place*_tmp132;void*_tmp133;struct Cyc_Dict_Dict*
_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_CfFlowInfo_ConsumeInfo _tmp136;
_LLFE: _tmp131=_tmp130.f1;if(_tmp131 <= (void*)1)goto _LL100;if(*((int*)_tmp131)!= 
0)goto _LL100;_tmp132=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp131)->f1;_tmp133=
_tmp130.f2;if(_tmp133 <= (void*)1)goto _LL100;if(*((int*)_tmp133)!= 0)goto _LL100;
_tmp134=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp133)->f1;_tmp135=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp133)->f2;_tmp136=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp133)->f3;_LLFF: _tmp135=Cyc_CfFlowInfo_reln_kill_exp(_tmp135,_tmpD0);_tmp134=
Cyc_CfFlowInfo_assign_place(_tmpD0->loc,_tmp134,env->all_changed,_tmp132,Cyc_CfFlowInfo_unknown_all);
if(Cyc_CfFlowInfo_contains_region((void*)0,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD0->topt))->v)){_tmp136.may_consume=((struct Cyc_List_List*(*)(
int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->hd=_tmp132;_tmp137->tl=0;_tmp137;}),
_tmp136.may_consume);if(!((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
k))Cyc_Dict_member)(_tmp136.consumed,_tmp132)){struct Cyc_CfFlowInfo_Place _tmp139;
void*_tmp13A;struct Cyc_CfFlowInfo_Place*_tmp138=_tmp132;_tmp139=*_tmp138;_tmp13A=(
void*)_tmp139.root;{void*_tmp13B=_tmp13A;struct Cyc_Absyn_Vardecl*_tmp13C;_LL103:
if(*((int*)_tmp13B)!= 0)goto _LL105;_tmp13C=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp13B)->f1;_LL104: {struct _dynforward_ptr _tmp13D=Cyc_Absynpp_qvar2string(
_tmp13C->name);({struct Cyc_String_pa_struct _tmp140;_tmp140.tag=0;_tmp140.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp13D);{void*_tmp13E[1]={&
_tmp140};Cyc_Tcutil_warn(_tmpD0->loc,({const char*_tmp13F="may clobber unique pointer %s";
_tag_dynforward(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,30));}),
_tag_dynforward(_tmp13E,sizeof(void*),1));}});goto _LL102;}_LL105:;_LL106:({void*
_tmp141[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp142="warning locations not for VarRoots";_tag_dynforward(_tmp142,
sizeof(char),_get_zero_arr_size(_tmp142,35));}),_tag_dynforward(_tmp141,sizeof(
void*),0));});_LL102:;}}if(env->all_changed == 0){struct Cyc_Dict_Dict*_tmp143=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(
_tmp136.consumed,_tmp132);if(_tmp136.consumed != _tmp143);_tmp136.consumed=
_tmp143;}}_tmp12C=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp144=
_cycalloc(sizeof(*_tmp144));_tmp144[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=_tmp134;_tmp145.f2=_tmp135;_tmp145.f3=_tmp136;
_tmp145;});_tmp144;});goto _LLFD;_LL100:;_LL101: goto _LLFD;_LLFD:;}return({struct
_tuple0 _tmp146;_tmp146.f1=_tmp12C;_tmp146.f2=Cyc_CfFlowInfo_unknown_all;_tmp146;});}}
_LLBD: if(*((int*)_tmpBA)!= 4)goto _LLBF;_tmpD4=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBA)->f1;_tmpD5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f2;if(_tmpD5 != 
0)goto _LLBF;_tmpD6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f3;_LLBE: {
struct Cyc_Dict_Dict**_tmp147=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp149;void*_tmp14A;struct _tuple5 _tmp148=Cyc_CfFlowInfo_save_consume_info(
inflow,1);_tmp149=_tmp148.f1;_tmp14A=_tmp148.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp173;_tmp173.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp173.may_consume=0;_tmp173;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict**
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp14B;});{void*_tmp14D;void*_tmp14E;struct _tuple0 _tmp14C=Cyc_NewControlFlow_anal_Lexp(
env,_tmp14A,_tmpD4);_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;{struct Cyc_Dict_Dict*
_tmp14F=*((struct Cyc_Dict_Dict**)_check_null(env->all_changed));env->all_changed=({
struct Cyc_Dict_Dict**_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp150;});{void*_tmp152;void*_tmp153;struct _tuple0 _tmp151=Cyc_NewControlFlow_anal_Rexp(
env,_tmp14A,_tmpD6);_tmp152=_tmp151.f1;_tmp153=_tmp151.f2;{struct Cyc_Dict_Dict*
_tmp154=*((struct Cyc_Dict_Dict**)_check_null(env->all_changed));void*_tmp155=Cyc_CfFlowInfo_after_flow((
struct Cyc_Dict_Dict**)& _tmp14F,_tmp14D,_tmp152,_tmp14F,_tmp154);void*_tmp156=Cyc_CfFlowInfo_join_flow(
_tmp147,_tmp14A,_tmp155,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp158;struct _tuple5
_tmp157=Cyc_CfFlowInfo_save_consume_info(_tmp156,0);_tmp158=_tmp157.f1;if(
init_consume)_tmp14A=Cyc_CfFlowInfo_restore_consume_info(_tmp14A,outflow_consume);
outflow_consume=_tmp158;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp156,_tmp14A)){if(_tmp147 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict**)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp147,Cyc_CfFlowInfo_union_place_set(
_tmp14F,_tmp154,0),0);}_tmp155=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp155);{void*_tmp159=_tmp155;struct Cyc_Dict_Dict*_tmp15A;struct Cyc_List_List*
_tmp15B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp15C;_LL108: if((int)_tmp159 != 0)goto
_LL10A;_LL109: return({struct _tuple0 _tmp15D;_tmp15D.f1=(void*)0;_tmp15D.f2=
_tmp153;_tmp15D;});_LL10A: if(_tmp159 <= (void*)1)goto _LL107;if(*((int*)_tmp159)!= 
0)goto _LL107;_tmp15A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp159)->f1;
_tmp15B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp159)->f2;_tmp15C=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp159)->f3;_LL10B: _tmp15C=Cyc_CfFlowInfo_and_consume(
_tmp149,_tmp15C);{void*_tmp15E=_tmp14E;struct Cyc_CfFlowInfo_Place*_tmp15F;_LL10D:
if(_tmp15E <= (void*)1)goto _LL10F;if(*((int*)_tmp15E)!= 0)goto _LL10F;_tmp15F=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp15E)->f1;_LL10E: _tmp15A=Cyc_CfFlowInfo_assign_place(
e->loc,_tmp15A,env->all_changed,_tmp15F,_tmp153);if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD4->topt))->v)){_tmp15C.may_consume=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->hd=_tmp15F;
_tmp160->tl=0;_tmp160;}),_tmp15C.may_consume);if(!((int(*)(struct Cyc_Dict_Dict*d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp15C.consumed,_tmp15F)){struct
Cyc_CfFlowInfo_Place _tmp162;void*_tmp163;struct Cyc_CfFlowInfo_Place*_tmp161=
_tmp15F;_tmp162=*_tmp161;_tmp163=(void*)_tmp162.root;{void*_tmp164=_tmp163;
struct Cyc_Absyn_Vardecl*_tmp165;_LL112: if(*((int*)_tmp164)!= 0)goto _LL114;
_tmp165=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp164)->f1;_LL113: {struct
_dynforward_ptr _tmp166=Cyc_Absynpp_qvar2string(_tmp165->name);({struct Cyc_String_pa_struct
_tmp169;_tmp169.tag=0;_tmp169.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp166);{void*_tmp167[1]={& _tmp169};Cyc_Tcutil_warn(_tmpD4->loc,({const char*
_tmp168="may clobber unique pointer %s";_tag_dynforward(_tmp168,sizeof(char),
_get_zero_arr_size(_tmp168,30));}),_tag_dynforward(_tmp167,sizeof(void*),1));}});
goto _LL111;}_LL114:;_LL115:({void*_tmp16A[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp16B="warning locations not for VarRoots";
_tag_dynforward(_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,35));}),
_tag_dynforward(_tmp16A,sizeof(void*),0));});_LL111:;}}if(env->all_changed == 0){
struct Cyc_Dict_Dict*_tmp16C=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,
struct Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(_tmp15C.consumed,_tmp15F);if(
_tmp15C.consumed != _tmp16C);_tmp15C.consumed=_tmp16C;}}_tmp15B=Cyc_CfFlowInfo_reln_assign_exp(
_tmp15B,_tmpD4,_tmpD6);_tmp155=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp16E;_tmp16E.tag=0;_tmp16E.f1=_tmp15A;_tmp16E.f2=_tmp15B;_tmp16E.f3=_tmp15C;
_tmp16E;});_tmp16D;});Cyc_NewControlFlow_update_tryflow(env,_tmp155);return({
struct _tuple0 _tmp16F;_tmp16F.f1=_tmp155;_tmp16F.f2=_tmp153;_tmp16F;});_LL10F: if((
int)_tmp15E != 0)goto _LL10C;_LL110: if(Cyc_CfFlowInfo_contains_region((void*)0,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD4->topt))->v))({void*_tmp170[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp171="dest of unique assignment is unknown!";_tag_dynforward(
_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,38));}),_tag_dynforward(_tmp170,
sizeof(void*),0));});return({struct _tuple0 _tmp172;_tmp172.f1=Cyc_NewControlFlow_use_Rval(
env,_tmpD6->loc,_tmp155,_tmp153);_tmp172.f2=_tmp153;_tmp172;});_LL10C:;}_LL107:;}}
_tmp14A=Cyc_CfFlowInfo_restore_consume_info(_tmp156,empty_consume);}}}}}}}_LLBF:
if(*((int*)_tmpBA)!= 9)goto _LLC1;_tmpD7=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBA)->f1;_tmpD8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpBA)->f2;_LLC0: {void*
_tmp175;void*_tmp176;struct _tuple0 _tmp174=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpD7);_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;_tmp175=Cyc_CfFlowInfo_drop_unique_rvals(
_tmpD7->loc,_tmp175);return Cyc_NewControlFlow_anal_Rexp(env,_tmp175,_tmpD8);}
_LLC1: if(*((int*)_tmpBA)!= 12)goto _LLC3;_tmpD9=((struct Cyc_Absyn_Throw_e_struct*)
_tmpBA)->f1;_LLC2: {void*_tmp178;void*_tmp179;struct _tuple0 _tmp177=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpD9);_tmp178=_tmp177.f1;_tmp179=_tmp177.f2;_tmp178=Cyc_CfFlowInfo_consume_unique_rvals(
_tmpD9->loc,_tmp178);Cyc_NewControlFlow_use_Rval(env,_tmpD9->loc,_tmp178,_tmp179);
return({struct _tuple0 _tmp17A;_tmp17A.f1=(void*)0;_tmp17A.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp17A;});}_LLC3: if(*((int*)_tmpBA)!= 11)goto _LLC5;_tmpDA=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpBA)->f1;_tmpDB=((struct Cyc_Absyn_FnCall_e_struct*)_tmpBA)->f2;_LLC4: {void*
_tmp17D;struct Cyc_List_List*_tmp17E;struct _tuple7 _tmp17C=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->hd=
_tmpDA;_tmp17B->tl=_tmpDB;_tmp17B;}),1);_tmp17D=_tmp17C.f1;_tmp17E=_tmp17C.f2;
_tmp17D=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp17D);{void*_tmp17F=Cyc_NewControlFlow_use_Rval(
env,_tmpDA->loc,_tmp17D,(void*)((struct Cyc_List_List*)_check_null(_tmp17E))->hd);
_tmp17E=_tmp17E->tl;{struct Cyc_List_List*init_params=0;{void*_tmp180=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDA->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp181;void*_tmp182;_LL117: if(_tmp180 <= (void*)4)goto _LL119;if(*((int*)_tmp180)
!= 4)goto _LL119;_tmp181=((struct Cyc_Absyn_PointerType_struct*)_tmp180)->f1;
_tmp182=(void*)_tmp181.elt_typ;_LL118:{void*_tmp183=Cyc_Tcutil_compress(_tmp182);
struct Cyc_Absyn_FnInfo _tmp184;struct Cyc_List_List*_tmp185;_LL11C: if(_tmp183 <= (
void*)4)goto _LL11E;if(*((int*)_tmp183)!= 8)goto _LL11E;_tmp184=((struct Cyc_Absyn_FnType_struct*)
_tmp183)->f1;_tmp185=_tmp184.attributes;_LL11D: for(0;_tmp185 != 0;_tmp185=_tmp185->tl){
void*_tmp186=(void*)_tmp185->hd;int _tmp187;_LL121: if(_tmp186 <= (void*)17)goto
_LL123;if(*((int*)_tmp186)!= 4)goto _LL123;_tmp187=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp186)->f1;_LL122: init_params=({struct Cyc_List_List*_tmp188=_cycalloc(sizeof(*
_tmp188));_tmp188->hd=(void*)_tmp187;_tmp188->tl=init_params;_tmp188;});goto
_LL120;_LL123:;_LL124: goto _LL120;_LL120:;}goto _LL11B;_LL11E:;_LL11F:({void*
_tmp189[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp18A="anal_Rexp: bad function type";_tag_dynforward(_tmp18A,sizeof(
char),_get_zero_arr_size(_tmp18A,29));}),_tag_dynforward(_tmp189,sizeof(void*),0));});
_LL11B:;}goto _LL116;_LL119:;_LL11A:({void*_tmp18B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp18C="anal_Rexp: bad function type";_tag_dynforward(_tmp18C,sizeof(char),
_get_zero_arr_size(_tmp18C,29));}),_tag_dynforward(_tmp18B,sizeof(void*),0));});
_LL116:;}{int i=1;for(0;_tmp17E != 0;(((_tmp17E=_tmp17E->tl,_tmpDB=((struct Cyc_List_List*)
_check_null(_tmpDB))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp17F=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpDB))->hd)->loc,_tmp17F,(void*)_tmp17E->hd);
continue;}{void*_tmp18D=_tmp17D;struct Cyc_Dict_Dict*_tmp18E;_LL126: if((int)
_tmp18D != 0)goto _LL128;_LL127: goto _LL125;_LL128: if(_tmp18D <= (void*)1)goto _LL125;
if(*((int*)_tmp18D)!= 0)goto _LL125;_tmp18E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp18D)->f1;_LL129: if(Cyc_CfFlowInfo_initlevel(_tmp18E,(void*)_tmp17E->hd)== (
void*)0)({void*_tmp18F[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpDB))->hd)->loc,({const char*_tmp190="expression may not be initialized";
_tag_dynforward(_tmp190,sizeof(char),_get_zero_arr_size(_tmp190,34));}),
_tag_dynforward(_tmp18F,sizeof(void*),0));});{void*_tmp191=_tmp17F;struct Cyc_Dict_Dict*
_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_CfFlowInfo_ConsumeInfo _tmp194;
_LL12B: if((int)_tmp191 != 0)goto _LL12D;_LL12C: goto _LL12A;_LL12D: if(_tmp191 <= (
void*)1)goto _LL12A;if(*((int*)_tmp191)!= 0)goto _LL12A;_tmp192=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp191)->f1;_tmp193=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp191)->f2;
_tmp194=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp191)->f3;_LL12E: {struct
Cyc_Dict_Dict*_tmp195=Cyc_CfFlowInfo_escape_deref(_tmp192,env->all_changed,(void*)
_tmp17E->hd);{void*_tmp196=(void*)_tmp17E->hd;struct Cyc_CfFlowInfo_Place*_tmp197;
_LL130: if(_tmp196 <= (void*)3)goto _LL132;if(*((int*)_tmp196)!= 2)goto _LL132;
_tmp197=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp196)->f1;_LL131:{void*
_tmp198=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpDB))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp199;void*_tmp19A;_LL135: if(_tmp198 <= (void*)4)goto _LL137;if(*((int*)_tmp198)
!= 4)goto _LL137;_tmp199=((struct Cyc_Absyn_PointerType_struct*)_tmp198)->f1;
_tmp19A=(void*)_tmp199.elt_typ;_LL136: _tmp195=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpDB->hd)->loc,_tmp195,env->all_changed,_tmp197,Cyc_CfFlowInfo_typ_to_absrval(
_tmp19A,Cyc_CfFlowInfo_esc_all));goto _LL134;_LL137:;_LL138:({void*_tmp19B[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp19C="anal_Rexp: bad type for initialized arg";_tag_dynforward(
_tmp19C,sizeof(char),_get_zero_arr_size(_tmp19C,40));}),_tag_dynforward(_tmp19B,
sizeof(void*),0));});_LL134:;}goto _LL12F;_LL132:;_LL133: goto _LL12F;_LL12F:;}
_tmp17F=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp19D=_cycalloc(
sizeof(*_tmp19D));_tmp19D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp19E;
_tmp19E.tag=0;_tmp19E.f1=_tmp195;_tmp19E.f2=_tmp193;_tmp19E.f3=_tmp194;_tmp19E;});
_tmp19D;});goto _LL12A;}_LL12A:;}goto _LL125;_LL125:;}}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDA->topt))->v))return({struct _tuple0
_tmp19F;_tmp19F.f1=(void*)0;_tmp19F.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp19F;});
else{return({struct _tuple0 _tmp1A0;_tmp1A0.f1=_tmp17F;_tmp1A0.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1A0;});}}}}_LLC5: if(*((int*)_tmpBA)!= 35)goto _LLC7;_tmpDC=((struct Cyc_Absyn_Malloc_e_struct*)
_tmpBA)->f1;_tmpDD=_tmpDC.is_calloc;_tmpDE=_tmpDC.rgn;_tmpDF=_tmpDC.elt_type;
_tmpE0=_tmpDC.num_elts;_tmpE1=_tmpDC.fat_result;_LLC6: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp1B3;_tmp1B3.tag=1;_tmp1B3.f1=_tmpE0;
_tmp1B3.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp1B3;});_tmp1B2;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->root=(void*)root;_tmp1B1->fields=0;
_tmp1B1;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1AF=
_cycalloc(sizeof(*_tmp1AF));_tmp1AF[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1B0;_tmp1B0.tag=2;_tmp1B0.f1=place;_tmp1B0;});_tmp1AF;});void*place_val=
_tmpE1?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpDF)),Cyc_CfFlowInfo_unknown_none);
void*outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmpDE
!= 0){void*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct _tuple7 _tmp1A2=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp1A1[2];_tmp1A1[1]=_tmpE0;_tmp1A1[0]=(struct
Cyc_Absyn_Exp*)_tmpDE;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1A1,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1A3=_tmp1A2.f1;
_tmp1A4=_tmp1A2.f2;outflow=_tmp1A3;}else{void*_tmp1A6;struct _tuple0 _tmp1A5=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpE0);_tmp1A6=_tmp1A5.f1;outflow=_tmp1A6;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpE0->loc,outflow);{void*_tmp1A7=outflow;struct Cyc_Dict_Dict*_tmp1A8;struct Cyc_List_List*
_tmp1A9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1AA;_LL13A: if((int)_tmp1A7 != 0)goto
_LL13C;_LL13B: return({struct _tuple0 _tmp1AB;_tmp1AB.f1=outflow;_tmp1AB.f2=rval;
_tmp1AB;});_LL13C: if(_tmp1A7 <= (void*)1)goto _LL139;if(*((int*)_tmp1A7)!= 0)goto
_LL139;_tmp1A8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A7)->f1;_tmp1A9=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A7)->f2;_tmp1AA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1A7)->f3;_LL13D: return({struct _tuple0 _tmp1AC;_tmp1AC.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1AE;_tmp1AE.tag=0;_tmp1AE.f1=Cyc_Dict_insert(_tmp1A8,root,place_val);_tmp1AE.f2=
_tmp1A9;_tmp1AE.f3=_tmp1AA;_tmp1AE;});_tmp1AD;});_tmp1AC.f2=rval;_tmp1AC;});
_LL139:;}}_LLC7: if(*((int*)_tmpBA)!= 17)goto _LLC9;_tmpE2=((struct Cyc_Absyn_New_e_struct*)
_tmpBA)->f1;_tmpE3=((struct Cyc_Absyn_New_e_struct*)_tmpBA)->f2;_LLC8: {void*root=(
void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));
_tmp1C6[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp1C7;_tmp1C7.tag=1;_tmp1C7.f1=
_tmpE3;_tmp1C7.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp1C7;});_tmp1C6;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->root=(void*)root;_tmp1C5->fields=0;
_tmp1C5;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1C3=
_cycalloc(sizeof(*_tmp1C3));_tmp1C3[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1C4;_tmp1C4.tag=2;_tmp1C4.f1=place;_tmp1C4;});_tmp1C3;});Cyc_CfFlowInfo_update_place_set(
env->all_changed,place,0);{void*outflow;void*place_val;if(_tmpE2 != 0){void*
_tmp1B6;struct Cyc_List_List*_tmp1B7;struct _tuple7 _tmp1B5=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp1B4[2];_tmp1B4[1]=_tmpE3;_tmp1B4[0]=(struct
Cyc_Absyn_Exp*)_tmpE2;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1B4,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1B6=_tmp1B5.f1;
_tmp1B7=_tmp1B5.f2;outflow=_tmp1B6;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp1B7))->tl))->hd;}else{void*
_tmp1B9;void*_tmp1BA;struct _tuple0 _tmp1B8=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE3);_tmp1B9=_tmp1B8.f1;_tmp1BA=_tmp1B8.f2;outflow=_tmp1B9;place_val=
_tmp1BA;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpE3->loc,outflow);{
void*_tmp1BB=outflow;struct Cyc_Dict_Dict*_tmp1BC;struct Cyc_List_List*_tmp1BD;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1BE;_LL13F: if((int)_tmp1BB != 0)goto _LL141;
_LL140: return({struct _tuple0 _tmp1BF;_tmp1BF.f1=outflow;_tmp1BF.f2=rval;_tmp1BF;});
_LL141: if(_tmp1BB <= (void*)1)goto _LL13E;if(*((int*)_tmp1BB)!= 0)goto _LL13E;
_tmp1BC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1BB)->f1;_tmp1BD=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1BB)->f2;_tmp1BE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1BB)->f3;_LL142: return({struct _tuple0 _tmp1C0;_tmp1C0.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1C2;_tmp1C2.tag=0;_tmp1C2.f1=Cyc_Dict_insert(_tmp1BC,root,place_val);_tmp1C2.f2=
_tmp1BD;_tmp1C2.f3=_tmp1BE;_tmp1C2;});_tmp1C1;});_tmp1C0.f2=rval;_tmp1C0;});
_LL13E:;}}}_LLC9: if(*((int*)_tmpBA)!= 16)goto _LLCB;_tmpE4=((struct Cyc_Absyn_Address_e_struct*)
_tmpBA)->f1;_LLCA: {void*_tmp1C9;void*_tmp1CA;struct _tuple0 _tmp1C8=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpE4);_tmp1C9=_tmp1C8.f1;_tmp1CA=_tmp1C8.f2;{void*_tmp1CB=_tmp1CA;
struct Cyc_CfFlowInfo_Place*_tmp1CC;_LL144: if((int)_tmp1CB != 0)goto _LL146;_LL145:
return({struct _tuple0 _tmp1CD;_tmp1CD.f1=_tmp1C9;_tmp1CD.f2=(void*)1;_tmp1CD;});
_LL146: if(_tmp1CB <= (void*)1)goto _LL143;if(*((int*)_tmp1CB)!= 0)goto _LL143;
_tmp1CC=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp1CB)->f1;_LL147: return({struct
_tuple0 _tmp1CE;_tmp1CE.f1=_tmp1C9;_tmp1CE.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1D0;_tmp1D0.tag=2;_tmp1D0.f1=_tmp1CC;_tmp1D0;});_tmp1CF;});_tmp1CE;});_LL143:;}}
_LLCB: if(*((int*)_tmpBA)!= 22)goto _LLCD;_tmpE5=((struct Cyc_Absyn_Deref_e_struct*)
_tmpBA)->f1;_LLCC: {void*_tmp1D2;void*_tmp1D3;struct _tuple0 _tmp1D1=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpE5);_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;_tmp1D2=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1D2);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D2,_tmpE5,
_tmp1D3);}_LLCD: if(*((int*)_tmpBA)!= 23)goto _LLCF;_tmpE6=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpBA)->f1;_tmpE7=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpBA)->f2;_LLCE: {
void*_tmp1D5;void*_tmp1D6;struct _tuple0 _tmp1D4=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE6);_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE6->topt))->v))return({struct _tuple0
_tmp1D7;_tmp1D7.f1=_tmp1D5;_tmp1D7.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp1D7;});{
void*_tmp1D8=_tmp1D6;struct Cyc_Dict_Dict*_tmp1D9;_LL149: if(_tmp1D8 <= (void*)3)
goto _LL14B;if(*((int*)_tmp1D8)!= 4)goto _LL14B;_tmp1D9=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1D8)->f1;_LL14A: return({struct _tuple0 _tmp1DA;_tmp1DA.f1=_tmp1D5;_tmp1DA.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp1D9,
_tmpE7);_tmp1DA;});_LL14B:;_LL14C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Core_Impossible_struct
_tmp1DC;_tmp1DC.tag=Cyc_Core_Impossible;_tmp1DC.f1=({const char*_tmp1DD="anal_Rexp: AggrMember";
_tag_dynforward(_tmp1DD,sizeof(char),_get_zero_arr_size(_tmp1DD,22));});_tmp1DC;});
_tmp1DB;}));_LL148:;}}_LLCF: if(*((int*)_tmpBA)!= 24)goto _LLD1;_tmpE8=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpBA)->f1;_tmpE9=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpBA)->f2;_LLD0: {
void*_tmp1DF;void*_tmp1E0;struct _tuple0 _tmp1DE=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE8);_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;_tmp1DF=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1DF);{void*_tmp1E2;void*_tmp1E3;struct _tuple0 _tmp1E1=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp1DF,_tmpE8,_tmp1E0);_tmp1E2=_tmp1E1.f1;_tmp1E3=_tmp1E1.f2;{void*
_tmp1E4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpE8->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1E5;void*_tmp1E6;_LL14E: if(_tmp1E4 <= (void*)4)goto
_LL150;if(*((int*)_tmp1E4)!= 4)goto _LL150;_tmp1E5=((struct Cyc_Absyn_PointerType_struct*)
_tmp1E4)->f1;_tmp1E6=(void*)_tmp1E5.elt_typ;_LL14F: if(Cyc_Absyn_is_union_type(
_tmp1E6))return({struct _tuple0 _tmp1E7;_tmp1E7.f1=_tmp1E2;_tmp1E7.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1E7;});goto _LL14D;_LL150:;_LL151:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Core_Impossible_struct
_tmp1E9;_tmp1E9.tag=Cyc_Core_Impossible;_tmp1E9.f1=({const char*_tmp1EA="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp1EA,sizeof(char),_get_zero_arr_size(_tmp1EA,25));});_tmp1E9;});
_tmp1E8;}));_LL14D:;}{void*_tmp1EB=_tmp1E3;struct Cyc_Dict_Dict*_tmp1EC;_LL153:
if(_tmp1EB <= (void*)3)goto _LL155;if(*((int*)_tmp1EB)!= 4)goto _LL155;_tmp1EC=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1EB)->f1;_LL154: return({struct _tuple0
_tmp1ED;_tmp1ED.f1=_tmp1E2;_tmp1ED.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp1EC,_tmpE9);_tmp1ED;});_LL155:;_LL156:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1EE=_cycalloc(sizeof(*
_tmp1EE));_tmp1EE[0]=({struct Cyc_Core_Impossible_struct _tmp1EF;_tmp1EF.tag=Cyc_Core_Impossible;
_tmp1EF.f1=({const char*_tmp1F0="anal_Rexp: AggrArrow";_tag_dynforward(_tmp1F0,
sizeof(char),_get_zero_arr_size(_tmp1F0,21));});_tmp1EF;});_tmp1EE;}));_LL152:;}}}
_LLD1: if(*((int*)_tmpBA)!= 6)goto _LLD3;_tmpEA=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpBA)->f1;_tmpEB=((struct Cyc_Absyn_Conditional_e_struct*)_tmpBA)->f2;_tmpEC=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpBA)->f3;_LLD2: {void*_tmp1F2;void*
_tmp1F3;struct _tuple0 _tmp1F1=Cyc_NewControlFlow_anal_test(env,inflow,_tmpEA);
_tmp1F2=_tmp1F1.f1;_tmp1F3=_tmp1F1.f2;_tmp1F2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEA->loc,_tmp1F2);_tmp1F3=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpEA->loc,
_tmp1F3);{struct _tuple0 _tmp1F4=Cyc_NewControlFlow_anal_Rexp(env,_tmp1F2,_tmpEB);
struct _tuple0 _tmp1F5=Cyc_NewControlFlow_anal_Rexp(env,_tmp1F3,_tmpEC);return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1F4,_tmp1F5,1);}}_LLD3: if(*((int*)_tmpBA)!= 7)goto _LLD5;
_tmpED=((struct Cyc_Absyn_And_e_struct*)_tmpBA)->f1;_tmpEE=((struct Cyc_Absyn_And_e_struct*)
_tmpBA)->f2;_LLD4: {void*_tmp1F7;void*_tmp1F8;struct _tuple0 _tmp1F6=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpED);_tmp1F7=_tmp1F6.f1;_tmp1F8=_tmp1F6.f2;_tmp1F7=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpED->loc,_tmp1F7);_tmp1F8=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpED->loc,
_tmp1F8);{void*_tmp1FA;void*_tmp1FB;struct _tuple0 _tmp1F9=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1F7,_tmpEE);_tmp1FA=_tmp1F9.f1;_tmp1FB=_tmp1F9.f2;_tmp1FA=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEE->loc,_tmp1FA);{struct _tuple0 _tmp1FC=({struct _tuple0 _tmp1FF;_tmp1FF.f1=
_tmp1FA;_tmp1FF.f2=_tmp1FB;_tmp1FF;});struct _tuple0 _tmp1FD=({struct _tuple0
_tmp1FE;_tmp1FE.f1=_tmp1F8;_tmp1FE.f2=(void*)((void*)0);_tmp1FE;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1FC,_tmp1FD,0);}}}_LLD5: if(*((int*)_tmpBA)!= 8)goto _LLD7;
_tmpEF=((struct Cyc_Absyn_Or_e_struct*)_tmpBA)->f1;_tmpF0=((struct Cyc_Absyn_Or_e_struct*)
_tmpBA)->f2;_LLD6: {void*_tmp201;void*_tmp202;struct _tuple0 _tmp200=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpEF);_tmp201=_tmp200.f1;_tmp202=_tmp200.f2;_tmp201=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEF->loc,_tmp201);_tmp202=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpEF->loc,
_tmp202);{void*_tmp204;void*_tmp205;struct _tuple0 _tmp203=Cyc_NewControlFlow_anal_Rexp(
env,_tmp202,_tmpF0);_tmp204=_tmp203.f1;_tmp205=_tmp203.f2;_tmp204=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF0->loc,_tmp204);{struct _tuple0 _tmp206=({struct _tuple0 _tmp209;_tmp209.f1=
_tmp204;_tmp209.f2=_tmp205;_tmp209;});struct _tuple0 _tmp207=({struct _tuple0
_tmp208;_tmp208.f1=_tmp201;_tmp208.f2=(void*)((void*)1);_tmp208;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp206,_tmp207,0);}}}_LLD7: if(*((int*)_tmpBA)!= 25)goto _LLD9;
_tmpF1=((struct Cyc_Absyn_Subscript_e_struct*)_tmpBA)->f1;_tmpF2=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpBA)->f2;_LLD8: {void*_tmp20C;struct Cyc_List_List*_tmp20D;struct _tuple7
_tmp20B=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct Cyc_Absyn_Exp*
_tmp20A[2];_tmp20A[1]=_tmpF2;_tmp20A[0]=_tmpF1;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp20A,sizeof(struct Cyc_Absyn_Exp*),
2));}),1);_tmp20C=_tmp20B.f1;_tmp20D=_tmp20B.f2;_tmp20C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF2->loc,_tmp20C);{void*_tmp20E=_tmp20C;{void*_tmp20F=_tmp20C;struct Cyc_Dict_Dict*
_tmp210;struct Cyc_List_List*_tmp211;struct Cyc_CfFlowInfo_ConsumeInfo _tmp212;
_LL158: if(_tmp20F <= (void*)1)goto _LL15A;if(*((int*)_tmp20F)!= 0)goto _LL15A;
_tmp210=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20F)->f1;_tmp211=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20F)->f2;_tmp212=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp20F)->f3;_LL159: if(Cyc_CfFlowInfo_initlevel(_tmp210,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp20D))->tl))->hd)== (void*)0)({
void*_tmp213[0]={};Cyc_Tcutil_terr(_tmpF2->loc,({const char*_tmp214="expression may not be initialized";
_tag_dynforward(_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,34));}),
_tag_dynforward(_tmp213,sizeof(void*),0));});{struct Cyc_List_List*_tmp215=Cyc_NewControlFlow_add_subscript_reln(
_tmp211,_tmpF1,_tmpF2);if(_tmp211 != _tmp215)_tmp20E=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp217;_tmp217.tag=0;_tmp217.f1=_tmp210;_tmp217.f2=_tmp215;_tmp217.f3=_tmp212;
_tmp217;});_tmp216;});goto _LL157;}_LL15A:;_LL15B: goto _LL157;_LL157:;}{void*
_tmp218=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF1->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp219;struct Cyc_Absyn_PtrAtts _tmp21A;struct Cyc_Absyn_Conref*
_tmp21B;_LL15D: if(_tmp218 <= (void*)4)goto _LL161;if(*((int*)_tmp218)!= 9)goto
_LL15F;_LL15E: {void*_tmp21C=(void*)((struct Cyc_List_List*)_check_null(_tmp20D))->hd;
struct Cyc_Dict_Dict*_tmp21D;_LL164: if(_tmp21C <= (void*)3)goto _LL166;if(*((int*)
_tmp21C)!= 4)goto _LL166;_tmp21D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp21C)->f1;
_LL165: return({struct _tuple0 _tmp21E;_tmp21E.f1=_tmp20E;_tmp21E.f2=((void*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp21D,Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmpF2)).f1));_tmp21E;});_LL166:;_LL167:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp21F=_cycalloc(sizeof(*
_tmp21F));_tmp21F[0]=({struct Cyc_Core_Impossible_struct _tmp220;_tmp220.tag=Cyc_Core_Impossible;
_tmp220.f1=({const char*_tmp221="anal_Rexp: Subscript";_tag_dynforward(_tmp221,
sizeof(char),_get_zero_arr_size(_tmp221,21));});_tmp220;});_tmp21F;}));_LL163:;}
_LL15F: if(*((int*)_tmp218)!= 4)goto _LL161;_tmp219=((struct Cyc_Absyn_PointerType_struct*)
_tmp218)->f1;_tmp21A=_tmp219.ptr_atts;_tmp21B=_tmp21A.bounds;_LL160:{void*
_tmp222=(void*)(Cyc_Absyn_compress_conref(_tmp21B))->v;void*_tmp223;_LL169: if(
_tmp222 <= (void*)1)goto _LL16B;if(*((int*)_tmp222)!= 0)goto _LL16B;_tmp223=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp222)->f1;if(_tmp223 <= (void*)2)goto _LL16B;
if(*((int*)_tmp223)!= 1)goto _LL16B;_LL16A:{void*_tmp224=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp20D))->tl))->hd;struct Cyc_List_List*
_tmp225;_LL16E: if(_tmp224 <= (void*)3)goto _LL170;if(*((int*)_tmp224)!= 3)goto
_LL170;_tmp225=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp224)->f1;_LL16F:(void*)(
_tmpF2->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp226=
_cycalloc(sizeof(*_tmp226));_tmp226[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp227;_tmp227.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp227.f1=_tmp225;_tmp227;});
_tmp226;})));goto _LL16D;_LL170:;_LL171: goto _LL16D;_LL16D:;}goto _LL168;_LL16B:;
_LL16C: goto _LL168;_LL168:;}{void*_tmp229;void*_tmp22A;struct _tuple0 _tmp228=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp20C,_tmpF1,(void*)((struct Cyc_List_List*)_check_null(_tmp20D))->hd);
_tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;return _tmp229 == (void*)0?({struct _tuple0
_tmp22B;_tmp22B.f1=_tmp229;_tmp22B.f2=_tmp22A;_tmp22B;}):({struct _tuple0 _tmp22C;
_tmp22C.f1=_tmp20E;_tmp22C.f2=_tmp22A;_tmp22C;});}_LL161:;_LL162:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp22D=_cycalloc(sizeof(*_tmp22D));
_tmp22D[0]=({struct Cyc_Core_Impossible_struct _tmp22E;_tmp22E.tag=Cyc_Core_Impossible;
_tmp22E.f1=({const char*_tmp22F="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp22F,sizeof(char),_get_zero_arr_size(_tmp22F,33));});_tmp22E;});
_tmp22D;}));_LL15C:;}}}_LLD9: if(*((int*)_tmpBA)!= 32)goto _LLDB;_tmpF3=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpBA)->f1;_tmpF4=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpBA)->f2;_LLDA: if(_tmpF4->is_flat){void*_tmp231;struct Cyc_List_List*_tmp232;
struct _tuple7 _tmp230=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmpF3,0);
_tmp231=_tmp230.f1;_tmp232=_tmp230.f2;_tmp231=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp231);for(0;(unsigned int)_tmpF3;(_tmpF3=_tmpF3->tl,_tmp232=_tmp232->tl)){
_tmp231=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmpF3->hd)->loc,
_tmp231,(void*)((struct Cyc_List_List*)_check_null(_tmp232))->hd);}return({struct
_tuple0 _tmp233;_tmp233.f1=_tmp231;_tmp233.f2=Cyc_CfFlowInfo_unknown_all;_tmp233;});}
_tmpF5=_tmpF3;goto _LLDC;_LLDB: if(*((int*)_tmpBA)!= 26)goto _LLDD;_tmpF5=((struct
Cyc_Absyn_Tuple_e_struct*)_tmpBA)->f1;_LLDC: {void*_tmp235;struct Cyc_List_List*
_tmp236;struct _tuple7 _tmp234=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,
_tmpF5,0);_tmp235=_tmp234.f1;_tmp236=_tmp234.f2;_tmp235=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp235);{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{
int i=0;for(0;_tmp236 != 0;(_tmp236=_tmp236->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(aggrdict,
Cyc_Absyn_fieldname(i),(void*)_tmp236->hd);}}return({struct _tuple0 _tmp237;
_tmp237.f1=_tmp235;_tmp237.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp239;_tmp239.tag=4;_tmp239.f1=aggrdict;_tmp239;});_tmp238;});_tmp237;});}}
_LLDD: if(*((int*)_tmpBA)!= 31)goto _LLDF;_tmpF6=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBA)->f2;_LLDE: _tmpF7=_tmpF6;goto _LLE0;_LLDF: if(*((int*)_tmpBA)!= 30)goto
_LLE1;_tmpF7=((struct Cyc_Absyn_Struct_e_struct*)_tmpBA)->f3;_LLE0: {void*_tmp23B;
struct Cyc_List_List*_tmp23C;struct _tuple7 _tmp23A=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))
Cyc_Core_snd,_tmpF7),0);_tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;_tmp23B=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp23B);{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{
int i=0;for(0;_tmp23C != 0;(((_tmp23C=_tmp23C->tl,_tmpF7=_tmpF7->tl)),++ i)){struct
Cyc_List_List*ds=(*((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmpF7))->hd)).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp23D=(void*)ds->hd;struct _dynforward_ptr*_tmp23E;
_LL173: if(*((int*)_tmp23D)!= 0)goto _LL175;_LL174:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Core_Impossible_struct
_tmp240;_tmp240.tag=Cyc_Core_Impossible;_tmp240.f1=({const char*_tmp241="anal_Rexp:Struct_e";
_tag_dynforward(_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,19));});_tmp240;});
_tmp23F;}));_LL175: if(*((int*)_tmp23D)!= 1)goto _LL172;_tmp23E=((struct Cyc_Absyn_FieldName_struct*)
_tmp23D)->f1;_LL176: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp23E,(void*)_tmp23C->hd);
_LL172:;}}}return({struct _tuple0 _tmp242;_tmp242.f1=_tmp23B;_tmp242.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp243=_cycalloc(sizeof(*_tmp243));
_tmp243[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp244;_tmp244.tag=4;_tmp244.f1=
aggrdict;_tmp244;});_tmp243;});_tmp242;});}}_LLE1: if(*((int*)_tmpBA)!= 28)goto
_LLE3;_tmpF8=((struct Cyc_Absyn_Array_e_struct*)_tmpBA)->f1;_LLE2: {struct Cyc_List_List*
_tmp245=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))
Cyc_Core_snd,_tmpF8);void*_tmp247;struct Cyc_List_List*_tmp248;struct _tuple7
_tmp246=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp245,0);_tmp247=
_tmp246.f1;_tmp248=_tmp246.f2;_tmp247=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp247);for(0;_tmp248 != 0;(_tmp248=_tmp248->tl,_tmp245=_tmp245->tl)){_tmp247=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp245))->hd)->loc,_tmp247,(void*)_tmp248->hd);}return({struct
_tuple0 _tmp249;_tmp249.f1=_tmp247;_tmp249.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp249;});}
_LLE3: if(*((int*)_tmpBA)!= 29)goto _LLE5;_tmpF9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBA)->f1;_tmpFA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpBA)->f2;_tmpFB=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpBA)->f3;_tmpFC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBA)->f4;_LLE4: {void*_tmp24B;void*_tmp24C;struct _tuple0 _tmp24A=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpFA);_tmp24B=_tmp24A.f1;_tmp24C=_tmp24A.f2;_tmp24B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpFA->loc,_tmp24B);{void*_tmp24D=_tmp24B;struct Cyc_Dict_Dict*_tmp24E;struct Cyc_List_List*
_tmp24F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp250;_LL178: if((int)_tmp24D != 0)goto
_LL17A;_LL179: return({struct _tuple0 _tmp251;_tmp251.f1=_tmp24B;_tmp251.f2=Cyc_CfFlowInfo_unknown_all;
_tmp251;});_LL17A: if(_tmp24D <= (void*)1)goto _LL177;if(*((int*)_tmp24D)!= 0)goto
_LL177;_tmp24E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24D)->f1;_tmp24F=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24D)->f2;_tmp250=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp24D)->f3;_LL17B: if(Cyc_CfFlowInfo_initlevel(_tmp24E,_tmp24C)== (void*)0)({
void*_tmp252[0]={};Cyc_Tcutil_terr(_tmpFA->loc,({const char*_tmp253="expression may not be initialized";
_tag_dynforward(_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,34));}),
_tag_dynforward(_tmp252,sizeof(void*),0));});{struct Cyc_List_List*new_relns=
_tmp24F;comp_loop: {void*_tmp254=(void*)_tmpFA->r;struct Cyc_Absyn_Exp*_tmp255;
void*_tmp256;struct Cyc_Absyn_Vardecl*_tmp257;void*_tmp258;struct Cyc_Absyn_Vardecl*
_tmp259;void*_tmp25A;struct Cyc_Absyn_Vardecl*_tmp25B;void*_tmp25C;struct Cyc_Absyn_Vardecl*
_tmp25D;void*_tmp25E;int _tmp25F;void*_tmp260;struct Cyc_List_List*_tmp261;struct
Cyc_List_List _tmp262;struct Cyc_Absyn_Exp*_tmp263;_LL17D: if(*((int*)_tmp254)!= 15)
goto _LL17F;_tmp255=((struct Cyc_Absyn_Cast_e_struct*)_tmp254)->f2;_LL17E: _tmpFA=
_tmp255;goto comp_loop;_LL17F: if(*((int*)_tmp254)!= 1)goto _LL181;_tmp256=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp254)->f2;if(_tmp256 <= (void*)1)goto _LL181;if(*((
int*)_tmp256)!= 0)goto _LL181;_tmp257=((struct Cyc_Absyn_Global_b_struct*)_tmp256)->f1;
if(!(!_tmp257->escapes))goto _LL181;_LL180: _tmp259=_tmp257;goto _LL182;_LL181: if(*((
int*)_tmp254)!= 1)goto _LL183;_tmp258=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp254)->f2;if(_tmp258 <= (void*)1)goto _LL183;if(*((int*)_tmp258)!= 3)goto _LL183;
_tmp259=((struct Cyc_Absyn_Local_b_struct*)_tmp258)->f1;if(!(!_tmp259->escapes))
goto _LL183;_LL182: _tmp25B=_tmp259;goto _LL184;_LL183: if(*((int*)_tmp254)!= 1)goto
_LL185;_tmp25A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp254)->f2;if(_tmp25A <= (
void*)1)goto _LL185;if(*((int*)_tmp25A)!= 4)goto _LL185;_tmp25B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp25A)->f1;if(!(!_tmp25B->escapes))goto _LL185;_LL184: _tmp25D=_tmp25B;goto
_LL186;_LL185: if(*((int*)_tmp254)!= 1)goto _LL187;_tmp25C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp254)->f2;if(_tmp25C <= (void*)1)goto _LL187;if(*((int*)_tmp25C)!= 2)goto _LL187;
_tmp25D=((struct Cyc_Absyn_Param_b_struct*)_tmp25C)->f1;if(!(!_tmp25D->escapes))
goto _LL187;_LL186: new_relns=({struct Cyc_List_List*_tmp264=_cycalloc(sizeof(*
_tmp264));_tmp264->hd=({struct Cyc_CfFlowInfo_Reln*_tmp265=_cycalloc(sizeof(*
_tmp265));_tmp265->vd=_tmpF9;_tmp265->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp267;_tmp267.tag=1;_tmp267.f1=_tmp25D;_tmp267;});_tmp266;}));_tmp265;});
_tmp264->tl=_tmp24F;_tmp264;});goto _LL17C;_LL187: if(*((int*)_tmp254)!= 0)goto
_LL189;_tmp25E=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp254)->f1;if(_tmp25E
<= (void*)1)goto _LL189;if(*((int*)_tmp25E)!= 2)goto _LL189;_tmp25F=((struct Cyc_Absyn_Int_c_struct*)
_tmp25E)->f2;_LL188: new_relns=({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*
_tmp268));_tmp268->hd=({struct Cyc_CfFlowInfo_Reln*_tmp269=_cycalloc(sizeof(*
_tmp269));_tmp269->vd=_tmpF9;_tmp269->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp26A=_cycalloc_atomic(sizeof(*_tmp26A));_tmp26A[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp26B;_tmp26B.tag=3;_tmp26B.f1=(unsigned int)_tmp25F;_tmp26B;});_tmp26A;}));
_tmp269;});_tmp268->tl=_tmp24F;_tmp268;});goto _LL17C;_LL189: if(*((int*)_tmp254)
!= 3)goto _LL18B;_tmp260=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp254)->f1;
if((int)_tmp260 != 19)goto _LL18B;_tmp261=((struct Cyc_Absyn_Primop_e_struct*)
_tmp254)->f2;if(_tmp261 == 0)goto _LL18B;_tmp262=*_tmp261;_tmp263=(struct Cyc_Absyn_Exp*)
_tmp262.hd;_LL18A:{void*_tmp26C=(void*)_tmp263->r;void*_tmp26D;struct Cyc_Absyn_Vardecl*
_tmp26E;void*_tmp26F;struct Cyc_Absyn_Vardecl*_tmp270;void*_tmp271;struct Cyc_Absyn_Vardecl*
_tmp272;void*_tmp273;struct Cyc_Absyn_Vardecl*_tmp274;_LL18E: if(*((int*)_tmp26C)
!= 1)goto _LL190;_tmp26D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp26C)->f2;if(
_tmp26D <= (void*)1)goto _LL190;if(*((int*)_tmp26D)!= 0)goto _LL190;_tmp26E=((
struct Cyc_Absyn_Global_b_struct*)_tmp26D)->f1;if(!(!_tmp26E->escapes))goto _LL190;
_LL18F: _tmp270=_tmp26E;goto _LL191;_LL190: if(*((int*)_tmp26C)!= 1)goto _LL192;
_tmp26F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp26C)->f2;if(_tmp26F <= (void*)
1)goto _LL192;if(*((int*)_tmp26F)!= 3)goto _LL192;_tmp270=((struct Cyc_Absyn_Local_b_struct*)
_tmp26F)->f1;if(!(!_tmp270->escapes))goto _LL192;_LL191: _tmp272=_tmp270;goto
_LL193;_LL192: if(*((int*)_tmp26C)!= 1)goto _LL194;_tmp271=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp271 <= (void*)1)goto _LL194;if(*((int*)_tmp271)!= 4)goto _LL194;
_tmp272=((struct Cyc_Absyn_Pat_b_struct*)_tmp271)->f1;if(!(!_tmp272->escapes))
goto _LL194;_LL193: _tmp274=_tmp272;goto _LL195;_LL194: if(*((int*)_tmp26C)!= 1)goto
_LL196;_tmp273=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp26C)->f2;if(_tmp273 <= (
void*)1)goto _LL196;if(*((int*)_tmp273)!= 2)goto _LL196;_tmp274=((struct Cyc_Absyn_Param_b_struct*)
_tmp273)->f1;if(!(!_tmp274->escapes))goto _LL196;_LL195: new_relns=({struct Cyc_List_List*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276->vd=_tmpF9;_tmp276->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp277=_cycalloc(sizeof(*_tmp277));
_tmp277[0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp278;_tmp278.tag=2;_tmp278.f1=
_tmp274;_tmp278;});_tmp277;}));_tmp276;});_tmp275->tl=_tmp24F;_tmp275;});goto
_LL18D;_LL196:;_LL197: goto _LL18D;_LL18D:;}goto _LL17C;_LL18B:;_LL18C: goto _LL17C;
_LL17C:;}if(_tmp24F != new_relns)_tmp24B=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp27A;_tmp27A.tag=0;_tmp27A.f1=_tmp24E;_tmp27A.f2=new_relns;_tmp27A.f3=_tmp250;
_tmp27A;});_tmp279;});{void*_tmp27B=_tmp24C;_LL199: if((int)_tmp27B != 0)goto
_LL19B;_LL19A: return({struct _tuple0 _tmp27C;_tmp27C.f1=_tmp24B;_tmp27C.f2=Cyc_CfFlowInfo_unknown_all;
_tmp27C;});_LL19B: if((int)_tmp27B != 2)goto _LL19D;_LL19C: goto _LL19E;_LL19D: if((
int)_tmp27B != 1)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(_tmp27B <= (void*)3)goto
_LL1A1;if(*((int*)_tmp27B)!= 2)goto _LL1A1;_LL1A0: {struct Cyc_List_List _tmp27D=({
struct Cyc_List_List _tmp288;_tmp288.hd=_tmpF9;_tmp288.tl=0;_tmp288;});_tmp24B=Cyc_NewControlFlow_add_vars(
_tmp24B,(struct Cyc_List_List*)& _tmp27D,Cyc_CfFlowInfo_unknown_all,e->loc);{void*
_tmp27F;void*_tmp280;struct _tuple0 _tmp27E=Cyc_NewControlFlow_anal_Rexp(env,
_tmp24B,_tmpFB);_tmp27F=_tmp27E.f1;_tmp280=_tmp27E.f2;_tmp27F=Cyc_CfFlowInfo_consume_unique_rvals(
_tmpFB->loc,_tmp27F);{void*_tmp281=_tmp27F;struct Cyc_Dict_Dict*_tmp282;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp283;_LL1A4: if((int)_tmp281 != 0)goto _LL1A6;_LL1A5: return({struct _tuple0
_tmp284;_tmp284.f1=_tmp27F;_tmp284.f2=Cyc_CfFlowInfo_unknown_all;_tmp284;});
_LL1A6: if(_tmp281 <= (void*)1)goto _LL1A3;if(*((int*)_tmp281)!= 0)goto _LL1A3;
_tmp282=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp281)->f1;_tmp283=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp281)->f3;_LL1A7: if(Cyc_CfFlowInfo_initlevel(
_tmp282,_tmp280)!= (void*)2){({void*_tmp285[0]={};Cyc_Tcutil_terr(_tmpFA->loc,({
const char*_tmp286="expression may not be initialized";_tag_dynforward(_tmp286,
sizeof(char),_get_zero_arr_size(_tmp286,34));}),_tag_dynforward(_tmp285,sizeof(
void*),0));});return({struct _tuple0 _tmp287;_tmp287.f1=(void*)0;_tmp287.f2=Cyc_CfFlowInfo_unknown_all;
_tmp287;});}_LL1A3:;}_tmp24B=_tmp27F;goto _LL1A2;}}_LL1A1:;_LL1A2: while(1){struct
Cyc_List_List _tmp289=({struct Cyc_List_List _tmp294;_tmp294.hd=_tmpF9;_tmp294.tl=0;
_tmp294;});_tmp24B=Cyc_NewControlFlow_add_vars(_tmp24B,(struct Cyc_List_List*)&
_tmp289,Cyc_CfFlowInfo_unknown_all,e->loc);{void*_tmp28B;void*_tmp28C;struct
_tuple0 _tmp28A=Cyc_NewControlFlow_anal_Rexp(env,_tmp24B,_tmpFB);_tmp28B=_tmp28A.f1;
_tmp28C=_tmp28A.f2;_tmp28B=Cyc_CfFlowInfo_consume_unique_rvals(_tmpFB->loc,
_tmp28B);{void*_tmp28D=_tmp28B;struct Cyc_Dict_Dict*_tmp28E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp28F;_LL1A9: if((int)_tmp28D != 0)goto _LL1AB;_LL1AA: goto _LL1A8;_LL1AB: if(
_tmp28D <= (void*)1)goto _LL1A8;if(*((int*)_tmp28D)!= 0)goto _LL1A8;_tmp28E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp28D)->f1;_tmp28F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp28D)->f3;_LL1AC: if(Cyc_CfFlowInfo_initlevel(_tmp28E,_tmp28C)!= (void*)2){({
void*_tmp290[0]={};Cyc_Tcutil_terr(_tmpFA->loc,({const char*_tmp291="expression may not be initialized";
_tag_dynforward(_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,34));}),
_tag_dynforward(_tmp290,sizeof(void*),0));});return({struct _tuple0 _tmp292;
_tmp292.f1=(void*)0;_tmp292.f2=Cyc_CfFlowInfo_unknown_all;_tmp292;});}_LL1A8:;}{
void*_tmp293=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp24B,_tmp28B,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp293,_tmp24B))break;_tmp24B=_tmp293;}}}return({struct _tuple0 _tmp295;_tmp295.f1=
_tmp24B;_tmp295.f2=Cyc_CfFlowInfo_unknown_all;_tmp295;});_LL198:;}}_LL177:;}}
_LLE5: if(*((int*)_tmpBA)!= 37)goto _LLE7;_tmpFD=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpBA)->f1;_LLE6: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmpFD))->flow;{void*_tmp296=(void*)_tmpFD->r;struct Cyc_Absyn_Stmt*
_tmp297;struct Cyc_Absyn_Stmt*_tmp298;struct Cyc_Absyn_Decl*_tmp299;struct Cyc_Absyn_Stmt*
_tmp29A;struct Cyc_Absyn_Exp*_tmp29B;_LL1AE: if(_tmp296 <= (void*)1)goto _LL1B4;if(*((
int*)_tmp296)!= 1)goto _LL1B0;_tmp297=((struct Cyc_Absyn_Seq_s_struct*)_tmp296)->f1;
_tmp298=((struct Cyc_Absyn_Seq_s_struct*)_tmp296)->f2;_LL1AF: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp297);_tmpFD=_tmp298;goto _LL1AD;_LL1B0: if(*((int*)_tmp296)!= 11)
goto _LL1B2;_tmp299=((struct Cyc_Absyn_Decl_s_struct*)_tmp296)->f1;_tmp29A=((
struct Cyc_Absyn_Decl_s_struct*)_tmp296)->f2;_LL1B1: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp299);_tmpFD=_tmp29A;goto _LL1AD;_LL1B2: if(*((int*)_tmp296)!= 0)
goto _LL1B4;_tmp29B=((struct Cyc_Absyn_Exp_s_struct*)_tmp296)->f1;_LL1B3: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp29B);_LL1B4:;_LL1B5:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C[0]=({struct Cyc_Core_Impossible_struct
_tmp29D;_tmp29D.tag=Cyc_Core_Impossible;_tmp29D.f1=({const char*_tmp29E="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,33));});_tmp29D;});
_tmp29C;}));_LL1AD:;}}_LLE7: if(*((int*)_tmpBA)!= 1)goto _LLE9;_tmpFE=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if((int)_tmpFE != 0)goto _LLE9;_LLE8:
goto _LLEA;_LLE9: if(*((int*)_tmpBA)!= 2)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((int*)
_tmpBA)!= 10)goto _LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)_tmpBA)!= 36)goto _LLEF;
_LLEE: goto _LLF0;_LLEF: if(*((int*)_tmpBA)!= 27)goto _LL8E;_LLF0:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F[0]=({
struct Cyc_Core_Impossible_struct _tmp2A0;_tmp2A0.tag=Cyc_Core_Impossible;_tmp2A0.f1=({
const char*_tmp2A1="anal_Rexp, unexpected exp form";_tag_dynforward(_tmp2A1,
sizeof(char),_get_zero_arr_size(_tmp2A1,31));});_tmp2A0;});_tmp29F;}));_LL8E:;}}
static struct _tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){
void*_tmp2A2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp2A3;void*_tmp2A4;struct Cyc_Absyn_PtrAtts _tmp2A5;
struct Cyc_Absyn_Conref*_tmp2A6;struct Cyc_Absyn_Conref*_tmp2A7;_LL1B7: if(_tmp2A2
<= (void*)4)goto _LL1B9;if(*((int*)_tmp2A2)!= 4)goto _LL1B9;_tmp2A3=((struct Cyc_Absyn_PointerType_struct*)
_tmp2A2)->f1;_tmp2A4=(void*)_tmp2A3.elt_typ;_tmp2A5=_tmp2A3.ptr_atts;_tmp2A6=
_tmp2A5.bounds;_tmp2A7=_tmp2A5.zero_term;_LL1B8: {void*_tmp2A8=f;struct Cyc_Dict_Dict*
_tmp2A9;struct Cyc_List_List*_tmp2AA;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2AB;
_LL1BC: if((int)_tmp2A8 != 0)goto _LL1BE;_LL1BD: return({struct _tuple0 _tmp2AC;
_tmp2AC.f1=f;_tmp2AC.f2=(void*)0;_tmp2AC;});_LL1BE: if(_tmp2A8 <= (void*)1)goto
_LL1BB;if(*((int*)_tmp2A8)!= 0)goto _LL1BB;_tmp2A9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2A8)->f1;_tmp2AA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A8)->f2;
_tmp2AB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A8)->f3;_LL1BF: if(Cyc_Tcutil_is_bound_one(
_tmp2A6)){void*_tmp2AD=r;struct Cyc_CfFlowInfo_Place*_tmp2AE;struct Cyc_CfFlowInfo_Place
_tmp2AF;void*_tmp2B0;struct Cyc_List_List*_tmp2B1;void*_tmp2B2;_LL1C1: if((int)
_tmp2AD != 1)goto _LL1C3;_LL1C2: goto _LL1C4;_LL1C3: if((int)_tmp2AD != 2)goto _LL1C5;
_LL1C4:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp2B4;_tmp2B4.tag=Cyc_CfFlowInfo_NotZero;_tmp2B4.f1=_tmp2AA;_tmp2B4;});_tmp2B3;})));
goto _LL1C0;_LL1C5: if(_tmp2AD <= (void*)3)goto _LL1C7;if(*((int*)_tmp2AD)!= 2)goto
_LL1C7;_tmp2AE=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2AD)->f1;_tmp2AF=*
_tmp2AE;_tmp2B0=(void*)_tmp2AF.root;_tmp2B1=_tmp2AF.fields;_LL1C6:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp2B5=_cycalloc(sizeof(*
_tmp2B5));_tmp2B5[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp2B6;_tmp2B6.tag=
Cyc_CfFlowInfo_NotZero;_tmp2B6.f1=_tmp2AA;_tmp2B6;});_tmp2B5;})));return({struct
_tuple0 _tmp2B7;_tmp2B7.f1=f;_tmp2B7.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=({struct Cyc_CfFlowInfo_Place*_tmp2BA=_cycalloc(
sizeof(*_tmp2BA));_tmp2BA->root=(void*)_tmp2B0;_tmp2BA->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2B1,flds);
_tmp2BA;});_tmp2B9;});_tmp2B8;});_tmp2B7;});_LL1C7: if((int)_tmp2AD != 0)goto
_LL1C9;_LL1C8:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple0 _tmp2BB;_tmp2BB.f1=(void*)0;_tmp2BB.f2=(void*)0;_tmp2BB;});_LL1C9:
if(_tmp2AD <= (void*)3)goto _LL1CB;if(*((int*)_tmp2AD)!= 0)goto _LL1CB;_tmp2B2=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2AD)->f1;_LL1CA: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp2B2);goto _LL1CC;_LL1CB:;_LL1CC:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2BD;_tmp2BD.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2BD.f1=_tmp2AA;_tmp2BD;});_tmp2BC;})));_LL1C0:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));
_tmp2BE[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2BF;_tmp2BF.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2BF.f1=_tmp2AA;_tmp2BF;});_tmp2BE;})));}if(Cyc_CfFlowInfo_initlevel(_tmp2A9,
r)== (void*)0)({void*_tmp2C0[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp2C1="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp2C1,sizeof(char),_get_zero_arr_size(_tmp2C1,46));}),
_tag_dynforward(_tmp2C0,sizeof(void*),0));});return({struct _tuple0 _tmp2C2;
_tmp2C2.f1=f;_tmp2C2.f2=(void*)0;_tmp2C2;});_LL1BB:;}_LL1B9:;_LL1BA:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));
_tmp2C3[0]=({struct Cyc_Core_Impossible_struct _tmp2C4;_tmp2C4.tag=Cyc_Core_Impossible;
_tmp2C4.f1=({const char*_tmp2C5="left deref of non-pointer-type";_tag_dynforward(
_tmp2C5,sizeof(char),_get_zero_arr_size(_tmp2C5,31));});_tmp2C4;});_tmp2C3;}));
_LL1B6:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict*
d;{void*_tmp2C6=inflow;struct Cyc_Dict_Dict*_tmp2C7;struct Cyc_List_List*_tmp2C8;
_LL1CE: if((int)_tmp2C6 != 0)goto _LL1D0;_LL1CF: return({struct _tuple0 _tmp2C9;
_tmp2C9.f1=(void*)0;_tmp2C9.f2=(void*)0;_tmp2C9;});_LL1D0: if(_tmp2C6 <= (void*)1)
goto _LL1CD;if(*((int*)_tmp2C6)!= 0)goto _LL1CD;_tmp2C7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2C6)->f1;_tmp2C8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2C6)->f2;
_LL1D1: d=_tmp2C7;_LL1CD:;}{void*_tmp2CA=(void*)e->r;struct Cyc_Absyn_Exp*_tmp2CB;
struct Cyc_Absyn_Exp*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CD;void*_tmp2CE;struct Cyc_Absyn_Vardecl*
_tmp2CF;void*_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D1;void*_tmp2D2;struct Cyc_Absyn_Vardecl*
_tmp2D3;void*_tmp2D4;struct Cyc_Absyn_Vardecl*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D6;
struct _dynforward_ptr*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Exp*
_tmp2D9;struct Cyc_Absyn_Exp*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct
_dynforward_ptr*_tmp2DC;_LL1D3: if(*((int*)_tmp2CA)!= 15)goto _LL1D5;_tmp2CB=((
struct Cyc_Absyn_Cast_e_struct*)_tmp2CA)->f2;_LL1D4: _tmp2CC=_tmp2CB;goto _LL1D6;
_LL1D5: if(*((int*)_tmp2CA)!= 13)goto _LL1D7;_tmp2CC=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp2CA)->f1;_LL1D6: _tmp2CD=_tmp2CC;goto _LL1D8;_LL1D7: if(*((int*)_tmp2CA)!= 14)
goto _LL1D9;_tmp2CD=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2CA)->f1;_LL1D8:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp2CD,flds);_LL1D9: if(*((int*)
_tmp2CA)!= 1)goto _LL1DB;_tmp2CE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2CA)->f2;
if(_tmp2CE <= (void*)1)goto _LL1DB;if(*((int*)_tmp2CE)!= 2)goto _LL1DB;_tmp2CF=((
struct Cyc_Absyn_Param_b_struct*)_tmp2CE)->f1;_LL1DA: _tmp2D1=_tmp2CF;goto _LL1DC;
_LL1DB: if(*((int*)_tmp2CA)!= 1)goto _LL1DD;_tmp2D0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2CA)->f2;if(_tmp2D0 <= (void*)1)goto _LL1DD;if(*((int*)_tmp2D0)!= 3)goto _LL1DD;
_tmp2D1=((struct Cyc_Absyn_Local_b_struct*)_tmp2D0)->f1;_LL1DC: _tmp2D3=_tmp2D1;
goto _LL1DE;_LL1DD: if(*((int*)_tmp2CA)!= 1)goto _LL1DF;_tmp2D2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2CA)->f2;if(_tmp2D2 <= (void*)1)goto _LL1DF;if(*((int*)_tmp2D2)!= 4)goto _LL1DF;
_tmp2D3=((struct Cyc_Absyn_Pat_b_struct*)_tmp2D2)->f1;_LL1DE: return({struct
_tuple0 _tmp2DD;_tmp2DD.f1=inflow;_tmp2DD.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp2DE=_cycalloc(sizeof(*_tmp2DE));_tmp2DE[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp2DF;_tmp2DF.tag=0;_tmp2DF.f1=({struct Cyc_CfFlowInfo_Place*_tmp2E0=_cycalloc(
sizeof(*_tmp2E0));_tmp2E0->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp2E1=_cycalloc(sizeof(*_tmp2E1));_tmp2E1[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp2E2;_tmp2E2.tag=0;_tmp2E2.f1=_tmp2D3;_tmp2E2;});_tmp2E1;}));_tmp2E0->fields=
flds;_tmp2E0;});_tmp2DF;});_tmp2DE;});_tmp2DD;});_LL1DF: if(*((int*)_tmp2CA)!= 1)
goto _LL1E1;_tmp2D4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2CA)->f2;if(
_tmp2D4 <= (void*)1)goto _LL1E1;if(*((int*)_tmp2D4)!= 0)goto _LL1E1;_tmp2D5=((
struct Cyc_Absyn_Global_b_struct*)_tmp2D4)->f1;_LL1E0: return({struct _tuple0
_tmp2E3;_tmp2E3.f1=inflow;_tmp2E3.f2=(void*)0;_tmp2E3;});_LL1E1: if(*((int*)
_tmp2CA)!= 24)goto _LL1E3;_tmp2D6=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2CA)->f1;
_tmp2D7=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2CA)->f2;_LL1E2:{void*_tmp2E4=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D6->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp2E5;void*_tmp2E6;_LL1EC: if(_tmp2E4 <= (void*)4)goto
_LL1EE;if(*((int*)_tmp2E4)!= 4)goto _LL1EE;_tmp2E5=((struct Cyc_Absyn_PointerType_struct*)
_tmp2E4)->f1;_tmp2E6=(void*)_tmp2E5.elt_typ;_LL1ED: if(!Cyc_Absyn_is_union_type(
_tmp2E6))flds=({struct Cyc_List_List*_tmp2E7=_cycalloc(sizeof(*_tmp2E7));_tmp2E7->hd=
_tmp2D7;_tmp2E7->tl=flds;_tmp2E7;});goto _LL1EB;_LL1EE:;_LL1EF:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2E8=_cycalloc(sizeof(*_tmp2E8));_tmp2E8[0]=({
struct Cyc_Core_Impossible_struct _tmp2E9;_tmp2E9.tag=Cyc_Core_Impossible;_tmp2E9.f1=({
const char*_tmp2EA="anal_Rexp: AggrArrow ptr";_tag_dynforward(_tmp2EA,sizeof(char),
_get_zero_arr_size(_tmp2EA,25));});_tmp2E9;});_tmp2E8;}));_LL1EB:;}_tmp2D8=
_tmp2D6;goto _LL1E4;_LL1E3: if(*((int*)_tmp2CA)!= 22)goto _LL1E5;_tmp2D8=((struct
Cyc_Absyn_Deref_e_struct*)_tmp2CA)->f1;_LL1E4: {void*_tmp2EC;void*_tmp2ED;struct
_tuple0 _tmp2EB=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp2D8);_tmp2EC=_tmp2EB.f1;
_tmp2ED=_tmp2EB.f2;_tmp2EC=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp2EC);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp2EC,_tmp2D8,_tmp2ED,
flds);}_LL1E5: if(*((int*)_tmp2CA)!= 25)goto _LL1E7;_tmp2D9=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2CA)->f1;_tmp2DA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2CA)->f2;_LL1E6: {
void*_tmp2EE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2D9->topt))->v);struct Cyc_Absyn_PtrInfo _tmp2EF;struct Cyc_Absyn_PtrAtts
_tmp2F0;struct Cyc_Absyn_Conref*_tmp2F1;_LL1F1: if(_tmp2EE <= (void*)4)goto _LL1F5;
if(*((int*)_tmp2EE)!= 9)goto _LL1F3;_LL1F2: {struct _dynforward_ptr*_tmp2F2=Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmp2DA)).f1);return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp2D9,({struct Cyc_List_List*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));
_tmp2F3->hd=_tmp2F2;_tmp2F3->tl=flds;_tmp2F3;}));}_LL1F3: if(*((int*)_tmp2EE)!= 4)
goto _LL1F5;_tmp2EF=((struct Cyc_Absyn_PointerType_struct*)_tmp2EE)->f1;_tmp2F0=
_tmp2EF.ptr_atts;_tmp2F1=_tmp2F0.bounds;_LL1F4: {void*_tmp2F6;struct Cyc_List_List*
_tmp2F7;struct _tuple7 _tmp2F5=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({
struct Cyc_Absyn_Exp*_tmp2F4[2];_tmp2F4[1]=_tmp2DA;_tmp2F4[0]=_tmp2D9;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp2F4,sizeof(struct Cyc_Absyn_Exp*),
2));}),1);_tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;_tmp2F6=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp2DA->loc,_tmp2F6);{void*_tmp2F8=_tmp2F6;{void*_tmp2F9=_tmp2F6;struct Cyc_Dict_Dict*
_tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2FC;
_LL1F8: if(_tmp2F9 <= (void*)1)goto _LL1FA;if(*((int*)_tmp2F9)!= 0)goto _LL1FA;
_tmp2FA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2F9)->f1;_tmp2FB=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2F9)->f2;_tmp2FC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2F9)->f3;_LL1F9: if(Cyc_CfFlowInfo_initlevel(_tmp2FA,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2F7))->tl))->hd)== (void*)0)({
void*_tmp2FD[0]={};Cyc_Tcutil_terr(_tmp2DA->loc,({const char*_tmp2FE="expression may not be initialized";
_tag_dynforward(_tmp2FE,sizeof(char),_get_zero_arr_size(_tmp2FE,34));}),
_tag_dynforward(_tmp2FD,sizeof(void*),0));});{struct Cyc_List_List*_tmp2FF=Cyc_NewControlFlow_add_subscript_reln(
_tmp2FB,_tmp2D9,_tmp2DA);if(_tmp2FB != _tmp2FF)_tmp2F8=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp301;_tmp301.tag=0;_tmp301.f1=_tmp2FA;_tmp301.f2=_tmp2FF;_tmp301.f3=_tmp2FC;
_tmp301;});_tmp300;});goto _LL1F7;}_LL1FA:;_LL1FB: goto _LL1F7;_LL1F7:;}{void*
_tmp302=(void*)(Cyc_Absyn_compress_conref(_tmp2F1))->v;void*_tmp303;_LL1FD: if(
_tmp302 <= (void*)1)goto _LL1FF;if(*((int*)_tmp302)!= 0)goto _LL1FF;_tmp303=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp302)->f1;if(_tmp303 <= (void*)2)goto _LL1FF;
if(*((int*)_tmp303)!= 1)goto _LL1FF;_LL1FE:{void*_tmp304=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2F7))->tl))->hd;struct Cyc_List_List*
_tmp305;_LL202: if(_tmp304 <= (void*)3)goto _LL204;if(*((int*)_tmp304)!= 3)goto
_LL204;_tmp305=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp304)->f1;_LL203:(void*)(
_tmp2DA->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp306=
_cycalloc(sizeof(*_tmp306));_tmp306[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp307;_tmp307.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp307.f1=_tmp305;_tmp307;});
_tmp306;})));goto _LL201;_LL204:;_LL205: goto _LL201;_LL201:;}goto _LL1FC;_LL1FF:;
_LL200: goto _LL1FC;_LL1FC:;}{void*_tmp309;void*_tmp30A;struct _tuple0 _tmp308=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp2F6,_tmp2D9,(void*)((struct Cyc_List_List*)_check_null(_tmp2F7))->hd,
flds);_tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;return _tmp309 == (void*)0?({struct
_tuple0 _tmp30B;_tmp30B.f1=_tmp309;_tmp30B.f2=_tmp30A;_tmp30B;}):({struct _tuple0
_tmp30C;_tmp30C.f1=_tmp2F8;_tmp30C.f2=_tmp30A;_tmp30C;});}}}_LL1F5:;_LL1F6:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp30D=_cycalloc(sizeof(*
_tmp30D));_tmp30D[0]=({struct Cyc_Core_Impossible_struct _tmp30E;_tmp30E.tag=Cyc_Core_Impossible;
_tmp30E.f1=({const char*_tmp30F="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp30F,sizeof(char),_get_zero_arr_size(_tmp30F,33));});_tmp30E;});
_tmp30D;}));_LL1F0:;}_LL1E7: if(*((int*)_tmp2CA)!= 23)goto _LL1E9;_tmp2DB=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp2CA)->f1;_tmp2DC=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2CA)->f2;_LL1E8: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2DB->topt))->v))return({struct _tuple0 _tmp310;_tmp310.f1=inflow;
_tmp310.f2=(void*)0;_tmp310;});return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,
_tmp2DB,({struct Cyc_List_List*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311->hd=
_tmp2DC;_tmp311->tl=flds;_tmp311;}));_LL1E9:;_LL1EA: return({struct _tuple0 _tmp312;
_tmp312.f1=(void*)0;_tmp312.f2=(void*)0;_tmp312;});_LL1D2:;}}static struct _tuple0
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,
struct Cyc_Absyn_Exp*e){void*_tmp314;void*_tmp315;struct _tuple0 _tmp313=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp314=_tmp313.f1;_tmp315=_tmp313.f2;return({struct _tuple0
_tmp316;_tmp316.f1=_tmp314;_tmp316.f2=_tmp315;_tmp316;});}static struct _tuple0 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){{void*
_tmp317=(void*)e->r;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Exp*_tmp319;
struct Cyc_Absyn_Exp*_tmp31A;struct Cyc_Absyn_Exp*_tmp31B;struct Cyc_Absyn_Exp*
_tmp31C;struct Cyc_Absyn_Exp*_tmp31D;struct Cyc_Absyn_Exp*_tmp31E;struct Cyc_Absyn_Exp*
_tmp31F;struct Cyc_Absyn_Exp*_tmp320;void*_tmp321;struct Cyc_List_List*_tmp322;
struct Cyc_List_List _tmp323;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_List_List*
_tmp325;void*_tmp326;struct Cyc_List_List*_tmp327;_LL207: if(*((int*)_tmp317)!= 6)
goto _LL209;_tmp318=((struct Cyc_Absyn_Conditional_e_struct*)_tmp317)->f1;_tmp319=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp317)->f2;_tmp31A=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp317)->f3;_LL208: {void*_tmp329;void*_tmp32A;struct _tuple0 _tmp328=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp318);_tmp329=_tmp328.f1;_tmp32A=_tmp328.f2;_tmp329=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp318->loc,_tmp329);_tmp32A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp318->loc,
_tmp32A);{void*_tmp32C;void*_tmp32D;struct _tuple0 _tmp32B=Cyc_NewControlFlow_anal_test(
env,_tmp329,_tmp319);_tmp32C=_tmp32B.f1;_tmp32D=_tmp32B.f2;{void*_tmp32F;void*
_tmp330;struct _tuple0 _tmp32E=Cyc_NewControlFlow_anal_test(env,_tmp32A,_tmp31A);
_tmp32F=_tmp32E.f1;_tmp330=_tmp32E.f2;return({struct _tuple0 _tmp331;_tmp331.f1=
Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp32C,_tmp32F,1);_tmp331.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp32D,_tmp330,1);_tmp331;});}}}_LL209: if(*((int*)_tmp317)!= 7)
goto _LL20B;_tmp31B=((struct Cyc_Absyn_And_e_struct*)_tmp317)->f1;_tmp31C=((struct
Cyc_Absyn_And_e_struct*)_tmp317)->f2;_LL20A: {void*_tmp333;void*_tmp334;struct
_tuple0 _tmp332=Cyc_NewControlFlow_anal_test(env,inflow,_tmp31B);_tmp333=_tmp332.f1;
_tmp334=_tmp332.f2;_tmp333=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp31B->loc,
_tmp333);_tmp334=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp31B->loc,_tmp334);{
void*_tmp336;void*_tmp337;struct _tuple0 _tmp335=Cyc_NewControlFlow_anal_test(env,
_tmp333,_tmp31C);_tmp336=_tmp335.f1;_tmp337=_tmp335.f2;_tmp336=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp31C->loc,_tmp336);_tmp337=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp31C->loc,
_tmp337);return({struct _tuple0 _tmp338;_tmp338.f1=_tmp336;_tmp338.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp334,_tmp337,0);_tmp338;});}}_LL20B: if(*((int*)_tmp317)!= 8)
goto _LL20D;_tmp31D=((struct Cyc_Absyn_Or_e_struct*)_tmp317)->f1;_tmp31E=((struct
Cyc_Absyn_Or_e_struct*)_tmp317)->f2;_LL20C: {void*_tmp33A;void*_tmp33B;struct
_tuple0 _tmp339=Cyc_NewControlFlow_anal_test(env,inflow,_tmp31D);_tmp33A=_tmp339.f1;
_tmp33B=_tmp339.f2;_tmp33A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp31D->loc,
_tmp33A);_tmp33B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp31D->loc,_tmp33B);{
void*_tmp33D;void*_tmp33E;struct _tuple0 _tmp33C=Cyc_NewControlFlow_anal_test(env,
_tmp33B,_tmp31E);_tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;_tmp33D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp31E->loc,_tmp33D);_tmp33E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp31E->loc,
_tmp33E);return({struct _tuple0 _tmp33F;_tmp33F.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp33A,_tmp33D,0);_tmp33F.f2=_tmp33E;_tmp33F;});}}_LL20D: if(*((int*)_tmp317)!= 
9)goto _LL20F;_tmp31F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp317)->f1;_tmp320=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp317)->f2;_LL20E: {void*_tmp341;void*_tmp342;
struct _tuple0 _tmp340=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp31F);_tmp341=
_tmp340.f1;_tmp342=_tmp340.f2;_tmp341=Cyc_CfFlowInfo_drop_unique_rvals(_tmp31F->loc,
_tmp341);return Cyc_NewControlFlow_anal_test(env,_tmp341,_tmp320);}_LL20F: if(*((
int*)_tmp317)!= 3)goto _LL211;_tmp321=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp317)->f1;if((int)_tmp321 != 11)goto _LL211;_tmp322=((struct Cyc_Absyn_Primop_e_struct*)
_tmp317)->f2;if(_tmp322 == 0)goto _LL211;_tmp323=*_tmp322;_tmp324=(struct Cyc_Absyn_Exp*)
_tmp323.hd;_tmp325=_tmp323.tl;if(_tmp325 != 0)goto _LL211;_LL210: {void*_tmp344;
void*_tmp345;struct _tuple0 _tmp343=Cyc_NewControlFlow_anal_test(env,inflow,
_tmp324);_tmp344=_tmp343.f1;_tmp345=_tmp343.f2;return({struct _tuple0 _tmp346;
_tmp346.f1=_tmp345;_tmp346.f2=_tmp344;_tmp346;});}_LL211: if(*((int*)_tmp317)!= 3)
goto _LL213;_tmp326=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp317)->f1;
_tmp327=((struct Cyc_Absyn_Primop_e_struct*)_tmp317)->f2;_LL212: {void*_tmp348;
struct Cyc_List_List*_tmp349;struct _tuple7 _tmp347=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp327,0);_tmp348=_tmp347.f1;_tmp349=_tmp347.f2;{void*_tmp34A=
_tmp326;_LL216: if((int)_tmp34A != 0)goto _LL218;_LL217: goto _LL219;_LL218: if((int)
_tmp34A != 2)goto _LL21A;_LL219: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp327))->hd)->loc,_tmp348);goto _LL215;_LL21A:;
_LL21B: _tmp348=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp327))->hd)->loc,_tmp348);goto _LL215;_LL215:;}{
void*_tmp34B=_tmp348;struct Cyc_Dict_Dict*_tmp34C;struct Cyc_List_List*_tmp34D;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp34E;_LL21D: if((int)_tmp34B != 0)goto _LL21F;
_LL21E: return({struct _tuple0 _tmp34F;_tmp34F.f1=_tmp348;_tmp34F.f2=_tmp348;
_tmp34F;});_LL21F: if(_tmp34B <= (void*)1)goto _LL21C;if(*((int*)_tmp34B)!= 0)goto
_LL21C;_tmp34C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp34B)->f1;_tmp34D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp34B)->f2;_tmp34E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp34B)->f3;_LL220: {struct Cyc_Absyn_Exp*_tmp350=(struct Cyc_Absyn_Exp*)_tmp327->hd;
struct Cyc_Absyn_Exp*_tmp351=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp327->tl))->hd;void*_tmp352=(void*)((struct Cyc_List_List*)
_check_null(_tmp349))->hd;void*_tmp353=(void*)((struct Cyc_List_List*)_check_null(
_tmp349->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp34C,_tmp352)== (void*)0)({void*
_tmp354[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp327->hd)->loc,({const
char*_tmp355="expression may not be initialized";_tag_dynforward(_tmp355,sizeof(
char),_get_zero_arr_size(_tmp355,34));}),_tag_dynforward(_tmp354,sizeof(void*),0));});
if(Cyc_CfFlowInfo_initlevel(_tmp34C,_tmp353)== (void*)0)({void*_tmp356[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp327->tl))->hd)->loc,({
const char*_tmp357="expression may not be initialized";_tag_dynforward(_tmp357,
sizeof(char),_get_zero_arr_size(_tmp357,34));}),_tag_dynforward(_tmp356,sizeof(
void*),0));});if(_tmp326 == (void*)5  || _tmp326 == (void*)6){struct _tuple0 _tmp359=({
struct _tuple0 _tmp358;_tmp358.f1=_tmp352;_tmp358.f2=_tmp353;_tmp358;});void*
_tmp35A;void*_tmp35B;void*_tmp35C;void*_tmp35D;void*_tmp35E;void*_tmp35F;void*
_tmp360;void*_tmp361;void*_tmp362;void*_tmp363;void*_tmp364;void*_tmp365;void*
_tmp366;void*_tmp367;void*_tmp368;void*_tmp369;void*_tmp36A;void*_tmp36B;void*
_tmp36C;void*_tmp36D;_LL222: _tmp35A=_tmp359.f1;if(_tmp35A <= (void*)3)goto _LL224;
if(*((int*)_tmp35A)!= 0)goto _LL224;_tmp35B=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp35A)->f1;_tmp35C=_tmp359.f2;if((int)_tmp35C != 0)goto _LL224;_LL223: {void*
_tmp36F;void*_tmp370;struct _tuple0 _tmp36E=Cyc_NewControlFlow_splitzero(env,
_tmp348,_tmp348,_tmp350,_tmp35B);_tmp36F=_tmp36E.f1;_tmp370=_tmp36E.f2;{void*
_tmp371=_tmp326;_LL237: if((int)_tmp371 != 5)goto _LL239;_LL238: return({struct
_tuple0 _tmp372;_tmp372.f1=_tmp370;_tmp372.f2=_tmp36F;_tmp372;});_LL239: if((int)
_tmp371 != 6)goto _LL23B;_LL23A: return({struct _tuple0 _tmp373;_tmp373.f1=_tmp36F;
_tmp373.f2=_tmp370;_tmp373;});_LL23B:;_LL23C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374[0]=({struct Cyc_Core_Impossible_struct
_tmp375;_tmp375.tag=Cyc_Core_Impossible;_tmp375.f1=({const char*_tmp376="anal_test, zero-split";
_tag_dynforward(_tmp376,sizeof(char),_get_zero_arr_size(_tmp376,22));});_tmp375;});
_tmp374;}));_LL236:;}}_LL224: _tmp35D=_tmp359.f1;if((int)_tmp35D != 0)goto _LL226;
_tmp35E=_tmp359.f2;if(_tmp35E <= (void*)3)goto _LL226;if(*((int*)_tmp35E)!= 0)goto
_LL226;_tmp35F=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp35E)->f1;
_LL225: {void*_tmp378;void*_tmp379;struct _tuple0 _tmp377=Cyc_NewControlFlow_splitzero(
env,_tmp348,_tmp348,_tmp351,_tmp35F);_tmp378=_tmp377.f1;_tmp379=_tmp377.f2;{void*
_tmp37A=_tmp326;_LL23E: if((int)_tmp37A != 5)goto _LL240;_LL23F: return({struct
_tuple0 _tmp37B;_tmp37B.f1=_tmp379;_tmp37B.f2=_tmp378;_tmp37B;});_LL240: if((int)
_tmp37A != 6)goto _LL242;_LL241: return({struct _tuple0 _tmp37C;_tmp37C.f1=_tmp378;
_tmp37C.f2=_tmp379;_tmp37C;});_LL242:;_LL243:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D[0]=({struct Cyc_Core_Impossible_struct
_tmp37E;_tmp37E.tag=Cyc_Core_Impossible;_tmp37E.f1=({const char*_tmp37F="anal_test, zero-split";
_tag_dynforward(_tmp37F,sizeof(char),_get_zero_arr_size(_tmp37F,22));});_tmp37E;});
_tmp37D;}));_LL23D:;}}_LL226: _tmp360=_tmp359.f1;if((int)_tmp360 != 0)goto _LL228;
_tmp361=_tmp359.f2;if((int)_tmp361 != 0)goto _LL228;_LL227: if(_tmp326 == (void*)5)
return({struct _tuple0 _tmp380;_tmp380.f1=_tmp348;_tmp380.f2=(void*)0;_tmp380;});
else{return({struct _tuple0 _tmp381;_tmp381.f1=(void*)0;_tmp381.f2=_tmp348;_tmp381;});}
_LL228: _tmp362=_tmp359.f1;if((int)_tmp362 != 0)goto _LL22A;_tmp363=_tmp359.f2;if((
int)_tmp363 != 1)goto _LL22A;_LL229: goto _LL22B;_LL22A: _tmp364=_tmp359.f1;if((int)
_tmp364 != 0)goto _LL22C;_tmp365=_tmp359.f2;if((int)_tmp365 != 2)goto _LL22C;_LL22B:
goto _LL22D;_LL22C: _tmp366=_tmp359.f1;if((int)_tmp366 != 0)goto _LL22E;_tmp367=
_tmp359.f2;if(_tmp367 <= (void*)3)goto _LL22E;if(*((int*)_tmp367)!= 2)goto _LL22E;
_LL22D: goto _LL22F;_LL22E: _tmp368=_tmp359.f1;if((int)_tmp368 != 1)goto _LL230;
_tmp369=_tmp359.f2;if((int)_tmp369 != 0)goto _LL230;_LL22F: goto _LL231;_LL230:
_tmp36A=_tmp359.f1;if((int)_tmp36A != 2)goto _LL232;_tmp36B=_tmp359.f2;if((int)
_tmp36B != 0)goto _LL232;_LL231: goto _LL233;_LL232: _tmp36C=_tmp359.f1;if(_tmp36C <= (
void*)3)goto _LL234;if(*((int*)_tmp36C)!= 2)goto _LL234;_tmp36D=_tmp359.f2;if((int)
_tmp36D != 0)goto _LL234;_LL233: if(_tmp326 == (void*)6)return({struct _tuple0 _tmp382;
_tmp382.f1=_tmp348;_tmp382.f2=(void*)0;_tmp382;});else{return({struct _tuple0
_tmp383;_tmp383.f1=(void*)0;_tmp383.f2=_tmp348;_tmp383;});}_LL234:;_LL235: goto
_LL221;_LL221:;}{struct _tuple0 _tmp385=({struct _tuple0 _tmp384;_tmp384.f1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp350->topt))->v);_tmp384.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp351->topt))->v);_tmp384;});void*
_tmp386;void*_tmp387;void*_tmp388;void*_tmp389;void*_tmp38A;void*_tmp38B;_LL245:
_tmp386=_tmp385.f1;if(_tmp386 <= (void*)4)goto _LL247;if(*((int*)_tmp386)!= 5)goto
_LL247;_tmp387=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp386)->f1;if((int)
_tmp387 != 1)goto _LL247;_LL246: goto _LL248;_LL247: _tmp388=_tmp385.f2;if(_tmp388 <= (
void*)4)goto _LL249;if(*((int*)_tmp388)!= 5)goto _LL249;_tmp389=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp388)->f1;if((int)_tmp389 != 1)goto _LL249;_LL248: goto _LL24A;_LL249: _tmp38A=
_tmp385.f1;if(_tmp38A <= (void*)4)goto _LL24B;if(*((int*)_tmp38A)!= 17)goto _LL24B;
_LL24A: goto _LL24C;_LL24B: _tmp38B=_tmp385.f2;if(_tmp38B <= (void*)4)goto _LL24D;if(*((
int*)_tmp38B)!= 17)goto _LL24D;_LL24C: goto _LL244;_LL24D:;_LL24E: return({struct
_tuple0 _tmp38C;_tmp38C.f1=_tmp348;_tmp38C.f2=_tmp348;_tmp38C;});_LL244:;}{void*
_tmp38D=_tmp326;_LL250: if((int)_tmp38D != 5)goto _LL252;_LL251: {void*_tmp38E=Cyc_NewControlFlow_if_tagcmp(
env,_tmp348,_tmp350,_tmp352,(void*)5,_tmp353);_tmp38E=Cyc_NewControlFlow_if_tagcmp(
env,_tmp38E,_tmp351,_tmp353,(void*)5,_tmp352);return({struct _tuple0 _tmp38F;
_tmp38F.f1=_tmp38E;_tmp38F.f2=_tmp348;_tmp38F;});}_LL252: if((int)_tmp38D != 6)
goto _LL254;_LL253: {void*_tmp390=Cyc_NewControlFlow_if_tagcmp(env,_tmp348,
_tmp350,_tmp352,(void*)5,_tmp353);_tmp390=Cyc_NewControlFlow_if_tagcmp(env,
_tmp390,_tmp351,_tmp353,(void*)5,_tmp352);return({struct _tuple0 _tmp391;_tmp391.f1=
_tmp348;_tmp391.f2=_tmp390;_tmp391;});}_LL254: if((int)_tmp38D != 7)goto _LL256;
_LL255: {void*_tmp392=Cyc_NewControlFlow_if_tagcmp(env,_tmp348,_tmp351,_tmp353,(
void*)8,_tmp352);void*_tmp393=Cyc_NewControlFlow_if_tagcmp(env,_tmp348,_tmp350,
_tmp352,(void*)10,_tmp353);return({struct _tuple0 _tmp394;_tmp394.f1=_tmp392;
_tmp394.f2=_tmp393;_tmp394;});}_LL256: if((int)_tmp38D != 9)goto _LL258;_LL257: {
void*_tmp395=Cyc_NewControlFlow_if_tagcmp(env,_tmp348,_tmp351,_tmp353,(void*)10,
_tmp352);void*_tmp396=Cyc_NewControlFlow_if_tagcmp(env,_tmp348,_tmp350,_tmp352,(
void*)8,_tmp353);return({struct _tuple0 _tmp397;_tmp397.f1=_tmp395;_tmp397.f2=
_tmp396;_tmp397;});}_LL258: if((int)_tmp38D != 8)goto _LL25A;_LL259: {void*_tmp398=
Cyc_NewControlFlow_if_tagcmp(env,_tmp348,_tmp350,_tmp352,(void*)8,_tmp353);void*
_tmp399=Cyc_NewControlFlow_if_tagcmp(env,_tmp348,_tmp351,_tmp353,(void*)10,
_tmp352);{void*_tmp39A=_tmp398;struct Cyc_Dict_Dict*_tmp39B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp39C;_LL25F: if((int)_tmp39A != 0)goto _LL261;_LL260:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({
struct Cyc_Core_Impossible_struct _tmp39E;_tmp39E.tag=Cyc_Core_Impossible;_tmp39E.f1=({
const char*_tmp39F="anal_test, Lt";_tag_dynforward(_tmp39F,sizeof(char),
_get_zero_arr_size(_tmp39F,14));});_tmp39E;});_tmp39D;}));_LL261: if(_tmp39A <= (
void*)1)goto _LL25E;if(*((int*)_tmp39A)!= 0)goto _LL25E;_tmp39B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp39A)->f1;_tmp39C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp39A)->f3;
_LL262: _tmp34C=_tmp39B;_tmp34E=_tmp39C;_LL25E:;}{void*_tmp3A0=(void*)_tmp350->r;
void*_tmp3A1;struct Cyc_Absyn_Vardecl*_tmp3A2;void*_tmp3A3;struct Cyc_Absyn_Vardecl*
_tmp3A4;void*_tmp3A5;struct Cyc_Absyn_Vardecl*_tmp3A6;void*_tmp3A7;struct Cyc_Absyn_Vardecl*
_tmp3A8;_LL264: if(*((int*)_tmp3A0)!= 1)goto _LL266;_tmp3A1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3A0)->f2;if(_tmp3A1 <= (void*)1)goto _LL266;if(*((int*)_tmp3A1)!= 0)goto _LL266;
_tmp3A2=((struct Cyc_Absyn_Global_b_struct*)_tmp3A1)->f1;if(!(!_tmp3A2->escapes))
goto _LL266;_LL265: _tmp3A4=_tmp3A2;goto _LL267;_LL266: if(*((int*)_tmp3A0)!= 1)goto
_LL268;_tmp3A3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3A0)->f2;if(_tmp3A3 <= (
void*)1)goto _LL268;if(*((int*)_tmp3A3)!= 3)goto _LL268;_tmp3A4=((struct Cyc_Absyn_Local_b_struct*)
_tmp3A3)->f1;if(!(!_tmp3A4->escapes))goto _LL268;_LL267: _tmp3A6=_tmp3A4;goto
_LL269;_LL268: if(*((int*)_tmp3A0)!= 1)goto _LL26A;_tmp3A5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3A0)->f2;if(_tmp3A5 <= (void*)1)goto _LL26A;if(*((int*)_tmp3A5)!= 4)goto _LL26A;
_tmp3A6=((struct Cyc_Absyn_Pat_b_struct*)_tmp3A5)->f1;if(!(!_tmp3A6->escapes))
goto _LL26A;_LL269: _tmp3A8=_tmp3A6;goto _LL26B;_LL26A: if(*((int*)_tmp3A0)!= 1)goto
_LL26C;_tmp3A7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3A0)->f2;if(_tmp3A7 <= (
void*)1)goto _LL26C;if(*((int*)_tmp3A7)!= 2)goto _LL26C;_tmp3A8=((struct Cyc_Absyn_Param_b_struct*)
_tmp3A7)->f1;if(!(!_tmp3A8->escapes))goto _LL26C;_LL26B: {void*_tmp3A9=(void*)
_tmp351->r;void*_tmp3AA;struct Cyc_Absyn_Vardecl*_tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Vardecl*
_tmp3AD;void*_tmp3AE;struct Cyc_Absyn_Vardecl*_tmp3AF;void*_tmp3B0;struct Cyc_Absyn_Vardecl*
_tmp3B1;void*_tmp3B2;int _tmp3B3;void*_tmp3B4;struct Cyc_List_List*_tmp3B5;struct
Cyc_List_List _tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;_LL26F: if(*((int*)_tmp3A9)!= 1)
goto _LL271;_tmp3AA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3A9)->f2;if(
_tmp3AA <= (void*)1)goto _LL271;if(*((int*)_tmp3AA)!= 0)goto _LL271;_tmp3AB=((
struct Cyc_Absyn_Global_b_struct*)_tmp3AA)->f1;if(!(!_tmp3AB->escapes))goto _LL271;
_LL270: _tmp3AD=_tmp3AB;goto _LL272;_LL271: if(*((int*)_tmp3A9)!= 1)goto _LL273;
_tmp3AC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3A9)->f2;if(_tmp3AC <= (void*)
1)goto _LL273;if(*((int*)_tmp3AC)!= 3)goto _LL273;_tmp3AD=((struct Cyc_Absyn_Local_b_struct*)
_tmp3AC)->f1;if(!(!_tmp3AD->escapes))goto _LL273;_LL272: _tmp3AF=_tmp3AD;goto
_LL274;_LL273: if(*((int*)_tmp3A9)!= 1)goto _LL275;_tmp3AE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3A9)->f2;if(_tmp3AE <= (void*)1)goto _LL275;if(*((int*)_tmp3AE)!= 4)goto _LL275;
_tmp3AF=((struct Cyc_Absyn_Pat_b_struct*)_tmp3AE)->f1;if(!(!_tmp3AF->escapes))
goto _LL275;_LL274: _tmp3B1=_tmp3AF;goto _LL276;_LL275: if(*((int*)_tmp3A9)!= 1)goto
_LL277;_tmp3B0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3A9)->f2;if(_tmp3B0 <= (
void*)1)goto _LL277;if(*((int*)_tmp3B0)!= 2)goto _LL277;_tmp3B1=((struct Cyc_Absyn_Param_b_struct*)
_tmp3B0)->f1;if(!(!_tmp3B1->escapes))goto _LL277;_LL276: {struct Cyc_List_List*
_tmp3B8=({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->vd=
_tmp3A8;_tmp3BD->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp3BF;_tmp3BF.tag=1;_tmp3BF.f1=_tmp3B1;_tmp3BF;});_tmp3BE;}));_tmp3BD;});
_tmp3BC->tl=_tmp34D;_tmp3BC;});return({struct _tuple0 _tmp3B9;_tmp3B9.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));
_tmp3BA[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3BB;_tmp3BB.tag=0;
_tmp3BB.f1=_tmp34C;_tmp3BB.f2=_tmp3B8;_tmp3BB.f3=_tmp34E;_tmp3BB;});_tmp3BA;});
_tmp3B9.f2=_tmp399;_tmp3B9;});}_LL277: if(*((int*)_tmp3A9)!= 0)goto _LL279;_tmp3B2=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp3A9)->f1;if(_tmp3B2 <= (void*)1)goto
_LL279;if(*((int*)_tmp3B2)!= 2)goto _LL279;_tmp3B3=((struct Cyc_Absyn_Int_c_struct*)
_tmp3B2)->f2;_LL278: {struct Cyc_List_List*_tmp3C0=({struct Cyc_List_List*_tmp3C4=
_cycalloc(sizeof(*_tmp3C4));_tmp3C4->hd=({struct Cyc_CfFlowInfo_Reln*_tmp3C5=
_cycalloc(sizeof(*_tmp3C5));_tmp3C5->vd=_tmp3A8;_tmp3C5->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessConst_struct*_tmp3C6=_cycalloc_atomic(sizeof(*_tmp3C6));
_tmp3C6[0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp3C7;_tmp3C7.tag=3;_tmp3C7.f1=(
unsigned int)_tmp3B3;_tmp3C7;});_tmp3C6;}));_tmp3C5;});_tmp3C4->tl=_tmp34D;
_tmp3C4;});return({struct _tuple0 _tmp3C1;_tmp3C1.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3C3;_tmp3C3.tag=0;_tmp3C3.f1=_tmp34C;_tmp3C3.f2=_tmp3C0;_tmp3C3.f3=_tmp34E;
_tmp3C3;});_tmp3C2;});_tmp3C1.f2=_tmp399;_tmp3C1;});}_LL279: if(*((int*)_tmp3A9)
!= 3)goto _LL27B;_tmp3B4=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3A9)->f1;
if((int)_tmp3B4 != 19)goto _LL27B;_tmp3B5=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3A9)->f2;if(_tmp3B5 == 0)goto _LL27B;_tmp3B6=*_tmp3B5;_tmp3B7=(struct Cyc_Absyn_Exp*)
_tmp3B6.hd;_LL27A: {void*_tmp3C8=(void*)_tmp3B7->r;void*_tmp3C9;struct Cyc_Absyn_Vardecl*
_tmp3CA;void*_tmp3CB;struct Cyc_Absyn_Vardecl*_tmp3CC;void*_tmp3CD;struct Cyc_Absyn_Vardecl*
_tmp3CE;void*_tmp3CF;struct Cyc_Absyn_Vardecl*_tmp3D0;_LL27E: if(*((int*)_tmp3C8)
!= 1)goto _LL280;_tmp3C9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3C8)->f2;if(
_tmp3C9 <= (void*)1)goto _LL280;if(*((int*)_tmp3C9)!= 0)goto _LL280;_tmp3CA=((
struct Cyc_Absyn_Global_b_struct*)_tmp3C9)->f1;if(!(!_tmp3CA->escapes))goto _LL280;
_LL27F: _tmp3CC=_tmp3CA;goto _LL281;_LL280: if(*((int*)_tmp3C8)!= 1)goto _LL282;
_tmp3CB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3C8)->f2;if(_tmp3CB <= (void*)
1)goto _LL282;if(*((int*)_tmp3CB)!= 3)goto _LL282;_tmp3CC=((struct Cyc_Absyn_Local_b_struct*)
_tmp3CB)->f1;if(!(!_tmp3CC->escapes))goto _LL282;_LL281: _tmp3CE=_tmp3CC;goto
_LL283;_LL282: if(*((int*)_tmp3C8)!= 1)goto _LL284;_tmp3CD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3C8)->f2;if(_tmp3CD <= (void*)1)goto _LL284;if(*((int*)_tmp3CD)!= 4)goto _LL284;
_tmp3CE=((struct Cyc_Absyn_Pat_b_struct*)_tmp3CD)->f1;if(!(!_tmp3CE->escapes))
goto _LL284;_LL283: _tmp3D0=_tmp3CE;goto _LL285;_LL284: if(*((int*)_tmp3C8)!= 1)goto
_LL286;_tmp3CF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3C8)->f2;if(_tmp3CF <= (
void*)1)goto _LL286;if(*((int*)_tmp3CF)!= 2)goto _LL286;_tmp3D0=((struct Cyc_Absyn_Param_b_struct*)
_tmp3CF)->f1;if(!(!_tmp3D0->escapes))goto _LL286;_LL285: {struct Cyc_List_List*
_tmp3D1=({struct Cyc_List_List*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->vd=
_tmp3A8;_tmp3D6->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp3D8;_tmp3D8.tag=2;_tmp3D8.f1=_tmp3D0;_tmp3D8;});_tmp3D7;}));_tmp3D6;});
_tmp3D5->tl=_tmp34D;_tmp3D5;});return({struct _tuple0 _tmp3D2;_tmp3D2.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));
_tmp3D3[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3D4;_tmp3D4.tag=0;
_tmp3D4.f1=_tmp34C;_tmp3D4.f2=_tmp3D1;_tmp3D4.f3=_tmp34E;_tmp3D4;});_tmp3D3;});
_tmp3D2.f2=_tmp399;_tmp3D2;});}_LL286:;_LL287: return({struct _tuple0 _tmp3D9;
_tmp3D9.f1=_tmp398;_tmp3D9.f2=_tmp399;_tmp3D9;});_LL27D:;}_LL27B:;_LL27C: return({
struct _tuple0 _tmp3DA;_tmp3DA.f1=_tmp398;_tmp3DA.f2=_tmp399;_tmp3DA;});_LL26E:;}
_LL26C:;_LL26D: return({struct _tuple0 _tmp3DB;_tmp3DB.f1=_tmp398;_tmp3DB.f2=
_tmp399;_tmp3DB;});_LL263:;}}_LL25A: if((int)_tmp38D != 10)goto _LL25C;_LL25B: {
void*_tmp3DC=Cyc_NewControlFlow_if_tagcmp(env,_tmp348,_tmp350,_tmp352,(void*)10,
_tmp353);void*_tmp3DD=Cyc_NewControlFlow_if_tagcmp(env,_tmp348,_tmp351,_tmp352,(
void*)8,_tmp352);{void*_tmp3DE=_tmp3DC;struct Cyc_Dict_Dict*_tmp3DF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3E0;_LL289: if((int)_tmp3DE != 0)goto _LL28B;_LL28A:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1[0]=({
struct Cyc_Core_Impossible_struct _tmp3E2;_tmp3E2.tag=Cyc_Core_Impossible;_tmp3E2.f1=({
const char*_tmp3E3="anal_test, Lte";_tag_dynforward(_tmp3E3,sizeof(char),
_get_zero_arr_size(_tmp3E3,15));});_tmp3E2;});_tmp3E1;}));_LL28B: if(_tmp3DE <= (
void*)1)goto _LL288;if(*((int*)_tmp3DE)!= 0)goto _LL288;_tmp3DF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3DE)->f1;_tmp3E0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3DE)->f3;
_LL28C: _tmp34C=_tmp3DF;_tmp34E=_tmp3E0;_LL288:;}{void*_tmp3E4=(void*)_tmp350->r;
void*_tmp3E5;struct Cyc_Absyn_Vardecl*_tmp3E6;void*_tmp3E7;struct Cyc_Absyn_Vardecl*
_tmp3E8;void*_tmp3E9;struct Cyc_Absyn_Vardecl*_tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Vardecl*
_tmp3EC;_LL28E: if(*((int*)_tmp3E4)!= 1)goto _LL290;_tmp3E5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3E4)->f2;if(_tmp3E5 <= (void*)1)goto _LL290;if(*((int*)_tmp3E5)!= 0)goto _LL290;
_tmp3E6=((struct Cyc_Absyn_Global_b_struct*)_tmp3E5)->f1;if(!(!_tmp3E6->escapes))
goto _LL290;_LL28F: _tmp3E8=_tmp3E6;goto _LL291;_LL290: if(*((int*)_tmp3E4)!= 1)goto
_LL292;_tmp3E7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3E4)->f2;if(_tmp3E7 <= (
void*)1)goto _LL292;if(*((int*)_tmp3E7)!= 3)goto _LL292;_tmp3E8=((struct Cyc_Absyn_Local_b_struct*)
_tmp3E7)->f1;if(!(!_tmp3E8->escapes))goto _LL292;_LL291: _tmp3EA=_tmp3E8;goto
_LL293;_LL292: if(*((int*)_tmp3E4)!= 1)goto _LL294;_tmp3E9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3E4)->f2;if(_tmp3E9 <= (void*)1)goto _LL294;if(*((int*)_tmp3E9)!= 4)goto _LL294;
_tmp3EA=((struct Cyc_Absyn_Pat_b_struct*)_tmp3E9)->f1;if(!(!_tmp3EA->escapes))
goto _LL294;_LL293: _tmp3EC=_tmp3EA;goto _LL295;_LL294: if(*((int*)_tmp3E4)!= 1)goto
_LL296;_tmp3EB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3E4)->f2;if(_tmp3EB <= (
void*)1)goto _LL296;if(*((int*)_tmp3EB)!= 2)goto _LL296;_tmp3EC=((struct Cyc_Absyn_Param_b_struct*)
_tmp3EB)->f1;if(!(!_tmp3EC->escapes))goto _LL296;_LL295: {void*_tmp3ED=(void*)
_tmp351->r;void*_tmp3EE;struct Cyc_List_List*_tmp3EF;struct Cyc_List_List _tmp3F0;
struct Cyc_Absyn_Exp*_tmp3F1;_LL299: if(*((int*)_tmp3ED)!= 3)goto _LL29B;_tmp3EE=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3ED)->f1;if((int)_tmp3EE != 19)goto
_LL29B;_tmp3EF=((struct Cyc_Absyn_Primop_e_struct*)_tmp3ED)->f2;if(_tmp3EF == 0)
goto _LL29B;_tmp3F0=*_tmp3EF;_tmp3F1=(struct Cyc_Absyn_Exp*)_tmp3F0.hd;_LL29A: {
void*_tmp3F2=(void*)_tmp3F1->r;void*_tmp3F3;struct Cyc_Absyn_Vardecl*_tmp3F4;void*
_tmp3F5;struct Cyc_Absyn_Vardecl*_tmp3F6;void*_tmp3F7;struct Cyc_Absyn_Vardecl*
_tmp3F8;void*_tmp3F9;struct Cyc_Absyn_Vardecl*_tmp3FA;_LL29E: if(*((int*)_tmp3F2)
!= 1)goto _LL2A0;_tmp3F3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3F2)->f2;if(
_tmp3F3 <= (void*)1)goto _LL2A0;if(*((int*)_tmp3F3)!= 0)goto _LL2A0;_tmp3F4=((
struct Cyc_Absyn_Global_b_struct*)_tmp3F3)->f1;if(!(!_tmp3F4->escapes))goto _LL2A0;
_LL29F: _tmp3F6=_tmp3F4;goto _LL2A1;_LL2A0: if(*((int*)_tmp3F2)!= 1)goto _LL2A2;
_tmp3F5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3F2)->f2;if(_tmp3F5 <= (void*)
1)goto _LL2A2;if(*((int*)_tmp3F5)!= 3)goto _LL2A2;_tmp3F6=((struct Cyc_Absyn_Local_b_struct*)
_tmp3F5)->f1;if(!(!_tmp3F6->escapes))goto _LL2A2;_LL2A1: _tmp3F8=_tmp3F6;goto
_LL2A3;_LL2A2: if(*((int*)_tmp3F2)!= 1)goto _LL2A4;_tmp3F7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3F2)->f2;if(_tmp3F7 <= (void*)1)goto _LL2A4;if(*((int*)_tmp3F7)!= 4)goto _LL2A4;
_tmp3F8=((struct Cyc_Absyn_Pat_b_struct*)_tmp3F7)->f1;if(!(!_tmp3F8->escapes))
goto _LL2A4;_LL2A3: _tmp3FA=_tmp3F8;goto _LL2A5;_LL2A4: if(*((int*)_tmp3F2)!= 1)goto
_LL2A6;_tmp3F9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3F2)->f2;if(_tmp3F9 <= (
void*)1)goto _LL2A6;if(*((int*)_tmp3F9)!= 2)goto _LL2A6;_tmp3FA=((struct Cyc_Absyn_Param_b_struct*)
_tmp3F9)->f1;if(!(!_tmp3FA->escapes))goto _LL2A6;_LL2A5: {struct Cyc_List_List*
_tmp3FB=({struct Cyc_List_List*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->vd=
_tmp3EC;_tmp400->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp402;_tmp402.tag=4;_tmp402.f1=_tmp3FA;_tmp402;});_tmp401;}));_tmp400;});
_tmp3FF->tl=_tmp34D;_tmp3FF;});return({struct _tuple0 _tmp3FC;_tmp3FC.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));
_tmp3FD[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3FE;_tmp3FE.tag=0;
_tmp3FE.f1=_tmp34C;_tmp3FE.f2=_tmp3FB;_tmp3FE.f3=_tmp34E;_tmp3FE;});_tmp3FD;});
_tmp3FC.f2=_tmp3DD;_tmp3FC;});}_LL2A6:;_LL2A7: return({struct _tuple0 _tmp403;
_tmp403.f1=_tmp3DC;_tmp403.f2=_tmp3DD;_tmp403;});_LL29D:;}_LL29B:;_LL29C: return({
struct _tuple0 _tmp404;_tmp404.f1=_tmp3DC;_tmp404.f2=_tmp3DD;_tmp404;});_LL298:;}
_LL296:;_LL297: return({struct _tuple0 _tmp405;_tmp405.f1=_tmp3DC;_tmp405.f2=
_tmp3DD;_tmp405;});_LL28D:;}}_LL25C:;_LL25D: return({struct _tuple0 _tmp406;_tmp406.f1=
_tmp348;_tmp406.f2=_tmp348;_tmp406;});_LL24F:;}}_LL21C:;}}_LL213:;_LL214: goto
_LL206;_LL206:;}{void*_tmp408;void*_tmp409;struct _tuple0 _tmp407=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp408=_tmp407.f1;_tmp409=_tmp407.f2;_tmp408=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp408);{void*_tmp40A=_tmp408;struct Cyc_Dict_Dict*_tmp40B;_LL2A9: if((int)
_tmp40A != 0)goto _LL2AB;_LL2AA: return({struct _tuple0 _tmp40C;_tmp40C.f1=_tmp408;
_tmp40C.f2=_tmp408;_tmp40C;});_LL2AB: if(_tmp40A <= (void*)1)goto _LL2A8;if(*((int*)
_tmp40A)!= 0)goto _LL2A8;_tmp40B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp40A)->f1;_LL2AC: {void*_tmp40D=_tmp409;void*_tmp40E;void*_tmp40F;void*
_tmp410;_LL2AE: if((int)_tmp40D != 0)goto _LL2B0;_LL2AF: return({struct _tuple0
_tmp411;_tmp411.f1=(void*)0;_tmp411.f2=_tmp408;_tmp411;});_LL2B0: if((int)_tmp40D
!= 2)goto _LL2B2;_LL2B1: goto _LL2B3;_LL2B2: if((int)_tmp40D != 1)goto _LL2B4;_LL2B3:
goto _LL2B5;_LL2B4: if(_tmp40D <= (void*)3)goto _LL2B6;if(*((int*)_tmp40D)!= 2)goto
_LL2B6;_LL2B5: return({struct _tuple0 _tmp412;_tmp412.f1=_tmp408;_tmp412.f2=(void*)
0;_tmp412;});_LL2B6: if(_tmp40D <= (void*)3)goto _LL2B8;if(*((int*)_tmp40D)!= 0)
goto _LL2B8;_tmp40E=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp40D)->f1;
if((int)_tmp40E != 0)goto _LL2B8;_LL2B7: goto _LL2B9;_LL2B8: if(_tmp40D <= (void*)3)
goto _LL2BA;if(*((int*)_tmp40D)!= 1)goto _LL2BA;_tmp40F=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp40D)->f1;if((int)_tmp40F != 0)goto _LL2BA;_LL2B9:({void*_tmp413[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp414="expression may not be initialized";_tag_dynforward(
_tmp414,sizeof(char),_get_zero_arr_size(_tmp414,34));}),_tag_dynforward(_tmp413,
sizeof(void*),0));});return({struct _tuple0 _tmp415;_tmp415.f1=(void*)0;_tmp415.f2=(
void*)0;_tmp415;});_LL2BA: if(_tmp40D <= (void*)3)goto _LL2BC;if(*((int*)_tmp40D)!= 
0)goto _LL2BC;_tmp410=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp40D)->f1;
_LL2BB: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp408,e,_tmp410);_LL2BC:
if(_tmp40D <= (void*)3)goto _LL2BE;if(*((int*)_tmp40D)!= 1)goto _LL2BE;_LL2BD:
return({struct _tuple0 _tmp416;_tmp416.f1=_tmp408;_tmp416.f2=_tmp408;_tmp416;});
_LL2BE: if(_tmp40D <= (void*)3)goto _LL2C0;if(*((int*)_tmp40D)!= 3)goto _LL2C0;
_LL2BF: return({struct _tuple0 _tmp417;_tmp417.f1=_tmp408;_tmp417.f2=_tmp408;
_tmp417;});_LL2C0: if(_tmp40D <= (void*)3)goto _LL2AD;if(*((int*)_tmp40D)!= 4)goto
_LL2AD;_LL2C1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp418=
_cycalloc(sizeof(*_tmp418));_tmp418[0]=({struct Cyc_Core_Impossible_struct _tmp419;
_tmp419.tag=Cyc_Core_Impossible;_tmp419.f1=({const char*_tmp41A="anal_test";
_tag_dynforward(_tmp41A,sizeof(char),_get_zero_arr_size(_tmp41A,10));});_tmp419;});
_tmp418;}));_LL2AD:;}_LL2A8:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,void*flow){
void*_tmp41B=flow;struct Cyc_Dict_Dict*_tmp41C;_LL2C3: if((int)_tmp41B != 0)goto
_LL2C5;_LL2C4: return;_LL2C5: if(_tmp41B <= (void*)1)goto _LL2C2;if(*((int*)_tmp41B)
!= 0)goto _LL2C2;_tmp41C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp41B)->f1;
_LL2C6:{struct Cyc_List_List*_tmp41D=env->param_roots;for(0;_tmp41D != 0;_tmp41D=
_tmp41D->tl){if(Cyc_CfFlowInfo_initlevel(_tmp41C,Cyc_CfFlowInfo_lookup_place(
_tmp41C,(struct Cyc_CfFlowInfo_Place*)_tmp41D->hd))!= (void*)2)({void*_tmp41E[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp41F="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp41F,sizeof(char),_get_zero_arr_size(_tmp41F,76));}),
_tag_dynforward(_tmp41E,sizeof(void*),0));});}}return;_LL2C2:;}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp421;struct Cyc_Core_Opt*
_tmp422;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Stmt*_tmp424;struct Cyc_Position_Segment*
_tmp425;struct Cyc_Absyn_Switch_clause*_tmp420=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp421=*_tmp420;_tmp422=_tmp421.pat_vars;_tmp423=_tmp421.where_clause;
_tmp424=_tmp421.body;_tmp425=_tmp421.loc;{void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp422))->v,Cyc_CfFlowInfo_unknown_all,
_tmp425);void*clause_outflow;if(_tmp423 != 0){void*_tmp427;void*_tmp428;struct
_tuple0 _tmp426=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_tmp423);_tmp427=_tmp426.f1;_tmp428=_tmp426.f2;inflow=_tmp428;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp427,_tmp424);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp424);}{void*_tmp429=clause_outflow;_LL2C8: if((int)_tmp429 != 0)
goto _LL2CA;_LL2C9: goto _LL2C7;_LL2CA:;_LL2CB: if(scs->tl == 0)return clause_outflow;
else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*
_tmp42A[0]={};Cyc_Tcutil_terr(_tmp424->loc,({const char*_tmp42B="switch clause may implicitly fallthru";
_tag_dynforward(_tmp42B,sizeof(char),_get_zero_arr_size(_tmp42B,38));}),
_tag_dynforward(_tmp42A,sizeof(void*),0));});else{({void*_tmp42C[0]={};Cyc_Tcutil_warn(
_tmp424->loc,({const char*_tmp42D="switch clause may implicitly fallthru";
_tag_dynforward(_tmp42D,sizeof(char),_get_zero_arr_size(_tmp42D,38));}),
_tag_dynforward(_tmp42C,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL2C7;_LL2C7:;}}}return(void*)0;}static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){void*
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp42E=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);inflow=(void*)_tmp42E->flow;{void*_tmp42F=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp430;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_Absyn_Stmt*
_tmp433;struct Cyc_Absyn_Stmt*_tmp434;struct Cyc_Absyn_Exp*_tmp435;struct Cyc_Absyn_Stmt*
_tmp436;struct Cyc_Absyn_Stmt*_tmp437;struct _tuple3 _tmp438;struct Cyc_Absyn_Exp*
_tmp439;struct Cyc_Absyn_Stmt*_tmp43A;struct Cyc_Absyn_Stmt*_tmp43B;struct Cyc_Absyn_Stmt*
_tmp43C;struct _tuple3 _tmp43D;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Stmt*
_tmp43F;struct Cyc_Absyn_Exp*_tmp440;struct _tuple3 _tmp441;struct Cyc_Absyn_Exp*
_tmp442;struct Cyc_Absyn_Stmt*_tmp443;struct _tuple3 _tmp444;struct Cyc_Absyn_Exp*
_tmp445;struct Cyc_Absyn_Stmt*_tmp446;struct Cyc_Absyn_Stmt*_tmp447;struct Cyc_Absyn_Stmt*
_tmp448;struct Cyc_List_List*_tmp449;struct Cyc_Absyn_Switch_clause**_tmp44A;
struct Cyc_Absyn_Switch_clause*_tmp44B;struct Cyc_Absyn_Stmt*_tmp44C;struct Cyc_Absyn_Stmt*
_tmp44D;struct Cyc_Absyn_Stmt*_tmp44E;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_List_List*
_tmp450;struct Cyc_Absyn_Stmt*_tmp451;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Decl*
_tmp453;struct Cyc_Absyn_Stmt*_tmp454;struct Cyc_Absyn_Stmt*_tmp455;struct Cyc_Absyn_Tvar*
_tmp456;struct Cyc_Absyn_Vardecl*_tmp457;int _tmp458;struct Cyc_Absyn_Stmt*_tmp459;
struct Cyc_Absyn_Exp*_tmp45A;_LL2CD: if((int)_tmp42F != 0)goto _LL2CF;_LL2CE: return
inflow;_LL2CF: if(_tmp42F <= (void*)1)goto _LL2F5;if(*((int*)_tmp42F)!= 2)goto
_LL2D1;_tmp430=((struct Cyc_Absyn_Return_s_struct*)_tmp42F)->f1;if(_tmp430 != 0)
goto _LL2D1;_LL2D0: if(env->noreturn)({void*_tmp45B[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp45C="`noreturn' function might return";_tag_dynforward(_tmp45C,
sizeof(char),_get_zero_arr_size(_tmp45C,33));}),_tag_dynforward(_tmp45B,sizeof(
void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return(
void*)0;_LL2D1: if(*((int*)_tmp42F)!= 2)goto _LL2D3;_tmp431=((struct Cyc_Absyn_Return_s_struct*)
_tmp42F)->f1;_LL2D2: if(env->noreturn)({void*_tmp45D[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp45E="`noreturn' function might return";_tag_dynforward(_tmp45E,
sizeof(char),_get_zero_arr_size(_tmp45E,33));}),_tag_dynforward(_tmp45D,sizeof(
void*),0));});{void*_tmp460;void*_tmp461;struct _tuple0 _tmp45F=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp431));_tmp460=_tmp45F.f1;
_tmp461=_tmp45F.f2;_tmp460=Cyc_CfFlowInfo_consume_unique_rvals(_tmp431->loc,
_tmp460);_tmp460=Cyc_NewControlFlow_use_Rval(env,_tmp431->loc,_tmp460,_tmp461);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp460);return(void*)0;}_LL2D3:
if(*((int*)_tmp42F)!= 0)goto _LL2D5;_tmp432=((struct Cyc_Absyn_Exp_s_struct*)
_tmp42F)->f1;_LL2D4: outflow=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp432)).f1;
goto _LL2CC;_LL2D5: if(*((int*)_tmp42F)!= 1)goto _LL2D7;_tmp433=((struct Cyc_Absyn_Seq_s_struct*)
_tmp42F)->f1;_tmp434=((struct Cyc_Absyn_Seq_s_struct*)_tmp42F)->f2;_LL2D6: outflow=
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp433),
_tmp434);goto _LL2CC;_LL2D7: if(*((int*)_tmp42F)!= 3)goto _LL2D9;_tmp435=((struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp42F)->f1;_tmp436=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp42F)->f2;_tmp437=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp42F)->f3;_LL2D8: {
void*_tmp463;void*_tmp464;struct _tuple0 _tmp462=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp435);_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;_tmp463=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp435->loc,_tmp463);_tmp464=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp435->loc,
_tmp464);outflow=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp463,_tmp436),Cyc_NewControlFlow_anal_stmt(env,_tmp464,_tmp437),1);goto
_LL2CC;}_LL2D9: if(*((int*)_tmp42F)!= 4)goto _LL2DB;_tmp438=((struct Cyc_Absyn_While_s_struct*)
_tmp42F)->f1;_tmp439=_tmp438.f1;_tmp43A=_tmp438.f2;_tmp43B=((struct Cyc_Absyn_While_s_struct*)
_tmp42F)->f2;_LL2DA: {void*_tmp465=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmp43A))->flow;void*_tmp467;void*_tmp468;struct _tuple0 _tmp466=Cyc_NewControlFlow_anal_test(
env,_tmp465,_tmp439);_tmp467=_tmp466.f1;_tmp468=_tmp466.f2;_tmp467=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp439->loc,_tmp467);_tmp468=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp439->loc,
_tmp468);{void*_tmp469=Cyc_NewControlFlow_anal_stmt(env,_tmp467,_tmp43B);Cyc_NewControlFlow_update_flow(
env,_tmp43A,_tmp469);outflow=_tmp468;goto _LL2CC;}}_LL2DB: if(*((int*)_tmp42F)!= 
13)goto _LL2DD;_tmp43C=((struct Cyc_Absyn_Do_s_struct*)_tmp42F)->f1;_tmp43D=((
struct Cyc_Absyn_Do_s_struct*)_tmp42F)->f2;_tmp43E=_tmp43D.f1;_tmp43F=_tmp43D.f2;
_LL2DC: {void*_tmp46A=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp43C);void*
_tmp46B=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp46A,_tmp43F))->flow;
void*_tmp46D;void*_tmp46E;struct _tuple0 _tmp46C=Cyc_NewControlFlow_anal_test(env,
_tmp46B,_tmp43E);_tmp46D=_tmp46C.f1;_tmp46E=_tmp46C.f2;Cyc_NewControlFlow_update_flow(
env,_tmp43C,_tmp46D);outflow=_tmp46E;goto _LL2CC;}_LL2DD: if(*((int*)_tmp42F)!= 8)
goto _LL2DF;_tmp440=((struct Cyc_Absyn_For_s_struct*)_tmp42F)->f1;_tmp441=((struct
Cyc_Absyn_For_s_struct*)_tmp42F)->f2;_tmp442=_tmp441.f1;_tmp443=_tmp441.f2;
_tmp444=((struct Cyc_Absyn_For_s_struct*)_tmp42F)->f3;_tmp445=_tmp444.f1;_tmp446=
_tmp444.f2;_tmp447=((struct Cyc_Absyn_For_s_struct*)_tmp42F)->f4;_LL2DE: {void*
_tmp46F=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp440)).f1;_tmp46F=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp440->loc,_tmp46F);{void*_tmp470=(void*)(Cyc_NewControlFlow_pre_stmt_check(
env,_tmp46F,_tmp443))->flow;void*_tmp472;void*_tmp473;struct _tuple0 _tmp471=Cyc_NewControlFlow_anal_test(
env,_tmp470,_tmp442);_tmp472=_tmp471.f1;_tmp473=_tmp471.f2;{void*_tmp474=Cyc_NewControlFlow_anal_stmt(
env,_tmp472,_tmp447);void*_tmp475=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp474,_tmp446))->flow;void*_tmp476=(Cyc_NewControlFlow_anal_Rexp(env,_tmp475,
_tmp445)).f1;_tmp476=Cyc_CfFlowInfo_drop_unique_rvals(_tmp445->loc,_tmp476);Cyc_NewControlFlow_update_flow(
env,_tmp443,_tmp476);outflow=_tmp473;goto _LL2CC;}}}_LL2DF: if(*((int*)_tmp42F)!= 
5)goto _LL2E1;_tmp448=((struct Cyc_Absyn_Break_s_struct*)_tmp42F)->f1;if(_tmp448 != 
0)goto _LL2E1;_LL2E0: return(void*)0;_LL2E1: if(*((int*)_tmp42F)!= 10)goto _LL2E3;
_tmp449=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp42F)->f1;_tmp44A=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp42F)->f2;if(_tmp44A == 0)goto _LL2E3;_tmp44B=*_tmp44A;_LL2E2: {void*_tmp478;
struct Cyc_List_List*_tmp479;struct _tuple7 _tmp477=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp449,0);_tmp478=_tmp477.f1;_tmp479=_tmp477.f2;for(0;_tmp479 != 0;(
_tmp479=_tmp479->tl,_tmp449=_tmp449->tl)){_tmp478=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp449))->hd)->loc,
_tmp478,(void*)_tmp479->hd);}_tmp478=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,
_tmp478);_tmp478=Cyc_NewControlFlow_add_vars(_tmp478,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp44B->pat_vars))->v,Cyc_CfFlowInfo_unknown_all,
s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp44B->body,
_tmp478);return(void*)0;}_LL2E3: if(*((int*)_tmp42F)!= 5)goto _LL2E5;_tmp44C=((
struct Cyc_Absyn_Break_s_struct*)_tmp42F)->f1;_LL2E4: _tmp44D=_tmp44C;goto _LL2E6;
_LL2E5: if(*((int*)_tmp42F)!= 6)goto _LL2E7;_tmp44D=((struct Cyc_Absyn_Continue_s_struct*)
_tmp42F)->f1;_LL2E6: _tmp44E=_tmp44D;goto _LL2E8;_LL2E7: if(*((int*)_tmp42F)!= 7)
goto _LL2E9;_tmp44E=((struct Cyc_Absyn_Goto_s_struct*)_tmp42F)->f2;_LL2E8: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp47A=_tmp42E->encloser;struct Cyc_Absyn_Stmt*
_tmp47B=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp44E)))->encloser;while(_tmp47B != _tmp47A){struct Cyc_Absyn_Stmt*_tmp47C=(Cyc_NewControlFlow_get_stmt_annot(
_tmp47A))->encloser;if(_tmp47C == _tmp47A){({void*_tmp47D[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp47E="goto enters local scope or exception handler";_tag_dynforward(
_tmp47E,sizeof(char),_get_zero_arr_size(_tmp47E,45));}),_tag_dynforward(_tmp47D,
sizeof(void*),0));});break;}_tmp47A=_tmp47C;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp44E),inflow);return(void*)0;_LL2E9: if(*((
int*)_tmp42F)!= 9)goto _LL2EB;_tmp44F=((struct Cyc_Absyn_Switch_s_struct*)_tmp42F)->f1;
_tmp450=((struct Cyc_Absyn_Switch_s_struct*)_tmp42F)->f2;_LL2EA: {void*_tmp480;
void*_tmp481;struct _tuple0 _tmp47F=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp44F);_tmp480=_tmp47F.f1;_tmp481=_tmp47F.f2;_tmp480=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp44F->loc,_tmp480);_tmp480=Cyc_NewControlFlow_use_Rval(env,_tmp44F->loc,
_tmp480,_tmp481);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp480,_tmp450);goto
_LL2CC;}_LL2EB: if(*((int*)_tmp42F)!= 14)goto _LL2ED;_tmp451=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp42F)->f1;_tmp452=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp42F)->f2;_LL2EC: {
int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;env->in_try=1;(
void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp451);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp452);{
void*_tmp482=scs_outflow;_LL2F8: if((int)_tmp482 != 0)goto _LL2FA;_LL2F9: goto _LL2F7;
_LL2FA:;_LL2FB:({void*_tmp483[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp484="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp484,sizeof(char),_get_zero_arr_size(_tmp484,42));}),
_tag_dynforward(_tmp483,sizeof(void*),0));});_LL2F7:;}outflow=s1_outflow;goto
_LL2CC;}}}_LL2ED: if(*((int*)_tmp42F)!= 11)goto _LL2EF;_tmp453=((struct Cyc_Absyn_Decl_s_struct*)
_tmp42F)->f1;_tmp454=((struct Cyc_Absyn_Decl_s_struct*)_tmp42F)->f2;_LL2EE:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp453),_tmp454);goto _LL2CC;_LL2EF: if(*((int*)_tmp42F)!= 12)goto _LL2F1;_tmp455=((
struct Cyc_Absyn_Label_s_struct*)_tmp42F)->f2;_LL2F0: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp455);goto _LL2CC;_LL2F1: if(*((int*)_tmp42F)!= 15)goto _LL2F3;
_tmp456=((struct Cyc_Absyn_Region_s_struct*)_tmp42F)->f1;_tmp457=((struct Cyc_Absyn_Region_s_struct*)
_tmp42F)->f2;_tmp458=((struct Cyc_Absyn_Region_s_struct*)_tmp42F)->f3;_tmp459=((
struct Cyc_Absyn_Region_s_struct*)_tmp42F)->f4;_LL2F2: {struct Cyc_List_List
_tmp485=({struct Cyc_List_List _tmp486;_tmp486.hd=_tmp457;_tmp486.tl=0;_tmp486;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp485,Cyc_CfFlowInfo_unknown_all,
_tmp459->loc);outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp459);goto
_LL2CC;}_LL2F3: if(*((int*)_tmp42F)!= 16)goto _LL2F5;_tmp45A=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp42F)->f1;_LL2F4: {void*_tmp488;void*_tmp489;struct _tuple0 _tmp487=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp45A);_tmp488=_tmp487.f1;_tmp489=_tmp487.f2;_tmp488=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp45A->loc,_tmp488);{void*_tmp48A=Cyc_NewControlFlow_use_Rval(env,_tmp45A->loc,
_tmp488,_tmp489);{void*_tmp48B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp45A->topt))->v);void*_tmp48C;_LL2FD: if(_tmp48B <= (void*)4)goto
_LL2FF;if(*((int*)_tmp48B)!= 15)goto _LL2FF;_tmp48C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp48B)->f1;_LL2FE: outflow=Cyc_CfFlowInfo_kill_flow_region(_tmp488,_tmp48C);
goto _LL2FC;_LL2FF:;_LL300:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({struct Cyc_Core_Impossible_struct
_tmp48E;_tmp48E.tag=Cyc_Core_Impossible;_tmp48E.f1=({const char*_tmp48F="anal_stmt -- reset_region";
_tag_dynforward(_tmp48F,sizeof(char),_get_zero_arr_size(_tmp48F,26));});_tmp48E;});
_tmp48D;}));_LL2FC:;}goto _LL2CC;}}_LL2F5:;_LL2F6:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490[0]=({struct Cyc_Core_Impossible_struct
_tmp491;_tmp491.tag=Cyc_Core_Impossible;_tmp491.f1=({const char*_tmp492="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp492,sizeof(char),_get_zero_arr_size(_tmp492,56));});_tmp491;});
_tmp490;}));_LL2CC:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
void*_tmp493=outflow;struct Cyc_CfFlowInfo_ConsumeInfo _tmp494;_LL302: if(_tmp493 <= (
void*)1)goto _LL304;if(*((int*)_tmp493)!= 0)goto _LL304;_tmp494=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp493)->f3;_LL303: goto _LL301;_LL304:;_LL305: goto _LL301;_LL301:;}return outflow;}
static void Cyc_NewControlFlow_check_nested_fun(void*inflow,struct Cyc_Absyn_Fndecl*
fd);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,
void*inflow,struct Cyc_Absyn_Decl*decl){void*_tmp495=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp496;struct Cyc_Core_Opt*_tmp497;struct Cyc_Core_Opt _tmp498;struct Cyc_List_List*
_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_List_List*_tmp49B;struct Cyc_Absyn_Fndecl*
_tmp49C;_LL307: if(_tmp495 <= (void*)2)goto _LL30F;if(*((int*)_tmp495)!= 0)goto
_LL309;_tmp496=((struct Cyc_Absyn_Var_d_struct*)_tmp495)->f1;_LL308: {struct Cyc_List_List
_tmp49D=({struct Cyc_List_List _tmp4AC;_tmp4AC.hd=_tmp496;_tmp4AC.tl=0;_tmp4AC;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp49D,Cyc_CfFlowInfo_unknown_none,
decl->loc);{struct Cyc_Absyn_Exp*_tmp49E=_tmp496->initializer;if(_tmp49E == 0)
return inflow;{void*_tmp4A0;void*_tmp4A1;struct _tuple0 _tmp49F=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_tmp49E);_tmp4A0=_tmp49F.f1;_tmp4A1=_tmp49F.f2;
_tmp4A0=Cyc_CfFlowInfo_consume_unique_rvals(_tmp49E->loc,_tmp4A0);{void*_tmp4A2=
_tmp4A0;struct Cyc_Dict_Dict*_tmp4A3;struct Cyc_List_List*_tmp4A4;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4A5;_LL312: if((int)_tmp4A2 != 0)goto _LL314;_LL313: return(void*)0;_LL314: if(
_tmp4A2 <= (void*)1)goto _LL311;if(*((int*)_tmp4A2)!= 0)goto _LL311;_tmp4A3=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4A2)->f1;_tmp4A4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp4A2)->f2;_tmp4A5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4A2)->f3;
_LL315: _tmp4A3=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp4A3,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp4A7=_cycalloc(sizeof(*
_tmp4A7));_tmp4A7[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp4A8;_tmp4A8.tag=0;
_tmp4A8.f1=_tmp496;_tmp4A8;});_tmp4A7;}));_tmp4A6->fields=0;_tmp4A6;}),_tmp4A1);
_tmp4A4=Cyc_CfFlowInfo_reln_assign_var(_tmp4A4,_tmp496,(struct Cyc_Absyn_Exp*)
_check_null(_tmp496->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp4A9=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp4AA=_cycalloc(sizeof(*
_tmp4AA));_tmp4AA[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp4AB;_tmp4AB.tag=
0;_tmp4AB.f1=_tmp4A3;_tmp4AB.f2=_tmp4A4;_tmp4AB.f3=_tmp4A5;_tmp4AB;});_tmp4AA;});
Cyc_NewControlFlow_update_tryflow(env,(void*)_tmp4A9);return(void*)_tmp4A9;}
_LL311:;}}}}_LL309: if(*((int*)_tmp495)!= 2)goto _LL30B;_tmp497=((struct Cyc_Absyn_Let_d_struct*)
_tmp495)->f2;if(_tmp497 == 0)goto _LL30B;_tmp498=*_tmp497;_tmp499=(struct Cyc_List_List*)
_tmp498.v;_tmp49A=((struct Cyc_Absyn_Let_d_struct*)_tmp495)->f3;_LL30A: {void*
_tmp4AE;void*_tmp4AF;struct _tuple0 _tmp4AD=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp49A);_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;_tmp4AE=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp49A->loc,_tmp4AE);_tmp4AE=Cyc_NewControlFlow_use_Rval(env,_tmp49A->loc,
_tmp4AE,_tmp4AF);return Cyc_NewControlFlow_add_vars(_tmp4AE,_tmp499,Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL30B: if(*((int*)_tmp495)!= 3)goto _LL30D;_tmp49B=((struct Cyc_Absyn_Letv_d_struct*)
_tmp495)->f1;_LL30C: return Cyc_NewControlFlow_add_vars(inflow,_tmp49B,Cyc_CfFlowInfo_unknown_none,
decl->loc);_LL30D: if(*((int*)_tmp495)!= 1)goto _LL30F;_tmp49C=((struct Cyc_Absyn_Fn_d_struct*)
_tmp495)->f1;_LL30E: Cyc_NewControlFlow_check_nested_fun(inflow,_tmp49C);{void*
_tmp4B0=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp49C->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp4B1=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp49C->fn_vardecl);
return Cyc_NewControlFlow_add_vars(inflow,({struct Cyc_List_List*_tmp4B2=_cycalloc(
sizeof(*_tmp4B2));_tmp4B2->hd=_tmp4B1;_tmp4B2->tl=0;_tmp4B2;}),Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL30F:;_LL310:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3[0]=({struct Cyc_Core_Impossible_struct
_tmp4B4;_tmp4B4.tag=Cyc_Core_Impossible;_tmp4B4.f1=({const char*_tmp4B5="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp4B5,sizeof(char),_get_zero_arr_size(_tmp4B5,35));});_tmp4B4;});
_tmp4B3;}));_LL306:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp4B6=(void*)s->r;
struct _tuple3 _tmp4B7;struct Cyc_Absyn_Stmt*_tmp4B8;struct _tuple3 _tmp4B9;struct Cyc_Absyn_Stmt*
_tmp4BA;struct Cyc_Absyn_Stmt*_tmp4BB;struct Cyc_Absyn_Stmt*_tmp4BC;struct Cyc_Absyn_Stmt*
_tmp4BD;struct _tuple3 _tmp4BE;struct Cyc_Absyn_Stmt*_tmp4BF;struct Cyc_Absyn_Stmt*
_tmp4C0;struct Cyc_Absyn_Stmt*_tmp4C1;struct _tuple3 _tmp4C2;struct Cyc_Absyn_Stmt*
_tmp4C3;struct Cyc_Absyn_Stmt*_tmp4C4;struct Cyc_Absyn_Stmt*_tmp4C5;struct Cyc_Absyn_Stmt*
_tmp4C6;struct Cyc_Absyn_Stmt*_tmp4C7;struct Cyc_Absyn_Stmt*_tmp4C8;struct Cyc_Absyn_Stmt*
_tmp4C9;struct Cyc_List_List*_tmp4CA;struct Cyc_List_List*_tmp4CB;_LL317: if(
_tmp4B6 <= (void*)1)goto _LL32B;if(*((int*)_tmp4B6)!= 8)goto _LL319;_tmp4B7=((
struct Cyc_Absyn_For_s_struct*)_tmp4B6)->f2;_tmp4B8=_tmp4B7.f2;_tmp4B9=((struct
Cyc_Absyn_For_s_struct*)_tmp4B6)->f3;_tmp4BA=_tmp4B9.f2;_tmp4BB=((struct Cyc_Absyn_For_s_struct*)
_tmp4B6)->f4;_LL318: Cyc_NewControlFlow_postpass(_tmp4BA);_tmp4BC=_tmp4BB;_tmp4BD=
_tmp4B8;goto _LL31A;_LL319: if(*((int*)_tmp4B6)!= 1)goto _LL31B;_tmp4BC=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4B6)->f1;_tmp4BD=((struct Cyc_Absyn_Seq_s_struct*)_tmp4B6)->f2;_LL31A: _tmp4BF=
_tmp4BC;_tmp4C0=_tmp4BD;goto _LL31C;_LL31B: if(*((int*)_tmp4B6)!= 4)goto _LL31D;
_tmp4BE=((struct Cyc_Absyn_While_s_struct*)_tmp4B6)->f1;_tmp4BF=_tmp4BE.f2;
_tmp4C0=((struct Cyc_Absyn_While_s_struct*)_tmp4B6)->f2;_LL31C: _tmp4C1=_tmp4C0;
_tmp4C3=_tmp4BF;goto _LL31E;_LL31D: if(*((int*)_tmp4B6)!= 13)goto _LL31F;_tmp4C1=((
struct Cyc_Absyn_Do_s_struct*)_tmp4B6)->f1;_tmp4C2=((struct Cyc_Absyn_Do_s_struct*)
_tmp4B6)->f2;_tmp4C3=_tmp4C2.f2;_LL31E: _tmp4C4=_tmp4C1;_tmp4C5=_tmp4C3;goto
_LL320;_LL31F: if(*((int*)_tmp4B6)!= 3)goto _LL321;_tmp4C4=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4B6)->f2;_tmp4C5=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4B6)->f3;_LL320:
Cyc_NewControlFlow_postpass(_tmp4C4);_tmp4C6=_tmp4C5;goto _LL322;_LL321: if(*((int*)
_tmp4B6)!= 15)goto _LL323;_tmp4C6=((struct Cyc_Absyn_Region_s_struct*)_tmp4B6)->f4;
_LL322: _tmp4C7=_tmp4C6;goto _LL324;_LL323: if(*((int*)_tmp4B6)!= 12)goto _LL325;
_tmp4C7=((struct Cyc_Absyn_Label_s_struct*)_tmp4B6)->f2;_LL324: _tmp4C8=_tmp4C7;
goto _LL326;_LL325: if(*((int*)_tmp4B6)!= 11)goto _LL327;_tmp4C8=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4B6)->f2;_LL326: Cyc_NewControlFlow_postpass(_tmp4C8);return;_LL327: if(*((int*)
_tmp4B6)!= 14)goto _LL329;_tmp4C9=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4B6)->f1;
_tmp4CA=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4B6)->f2;_LL328: Cyc_NewControlFlow_postpass(
_tmp4C9);_tmp4CB=_tmp4CA;goto _LL32A;_LL329: if(*((int*)_tmp4B6)!= 9)goto _LL32B;
_tmp4CB=((struct Cyc_Absyn_Switch_s_struct*)_tmp4B6)->f2;_LL32A: for(0;_tmp4CB != 0;
_tmp4CB=_tmp4CB->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp4CB->hd)->body);}return;_LL32B:;_LL32C: return;_LL316:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){Cyc_NewControlFlow_check_nested_fun((void*)({struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp4CD;_tmp4CD.tag=0;_tmp4CD.f1=Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp);_tmp4CD.f2=0;_tmp4CD.f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4CE;_tmp4CE.consumed=Cyc_CfFlowInfo_mt_place_set();_tmp4CE.may_consume=0;
_tmp4CE;});_tmp4CD;});_tmp4CC;}),fd);}static void Cyc_NewControlFlow_check_nested_fun(
void*inflow,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Position_Segment*_tmp4CF=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,Cyc_CfFlowInfo_unknown_all,_tmp4CF);{struct
Cyc_List_List*_tmp4D0=0;{void*_tmp4D1=inflow;struct Cyc_Dict_Dict*_tmp4D2;struct
Cyc_List_List*_tmp4D3;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4D4;_LL32E: if((int)
_tmp4D1 != 0)goto _LL330;_LL32F:({void*_tmp4D5[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4D6="check_fun";_tag_dynforward(_tmp4D6,sizeof(char),_get_zero_arr_size(
_tmp4D6,10));}),_tag_dynforward(_tmp4D5,sizeof(void*),0));});_LL330: if(_tmp4D1 <= (
void*)1)goto _LL32D;if(*((int*)_tmp4D1)!= 0)goto _LL32D;_tmp4D2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp4D1)->f1;_tmp4D3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4D1)->f2;
_tmp4D4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4D1)->f3;_LL331: {struct
Cyc_List_List*atts;{void*_tmp4D7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp4D8;struct Cyc_List_List*
_tmp4D9;_LL333: if(_tmp4D7 <= (void*)4)goto _LL335;if(*((int*)_tmp4D7)!= 8)goto
_LL335;_tmp4D8=((struct Cyc_Absyn_FnType_struct*)_tmp4D7)->f1;_tmp4D9=_tmp4D8.attributes;
_LL334: atts=_tmp4D9;goto _LL332;_LL335:;_LL336:({void*_tmp4DA[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4DB="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp4DB,sizeof(char),_get_zero_arr_size(_tmp4DB,50));}),_tag_dynforward(_tmp4DA,
sizeof(void*),0));});_LL332:;}for(0;atts != 0;atts=atts->tl){void*_tmp4DC=(void*)
atts->hd;int _tmp4DD;_LL338: if(_tmp4DC <= (void*)17)goto _LL33A;if(*((int*)_tmp4DC)
!= 4)goto _LL33A;_tmp4DD=((struct Cyc_Absyn_Initializes_att_struct*)_tmp4DC)->f1;
_LL339: {unsigned int j=(unsigned int)_tmp4DD;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp4DE[0]={};Cyc_Tcutil_terr(_tmp4CF,({const char*_tmp4DF="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp4DF,sizeof(char),_get_zero_arr_size(_tmp4DF,51));}),
_tag_dynforward(_tmp4DE,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp4E0=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp4E1=Cyc_Tcutil_compress((void*)_tmp4E0->type);struct Cyc_Absyn_PtrInfo
_tmp4E2;void*_tmp4E3;struct Cyc_Absyn_PtrAtts _tmp4E4;struct Cyc_Absyn_Conref*
_tmp4E5;struct Cyc_Absyn_Conref*_tmp4E6;struct Cyc_Absyn_Conref*_tmp4E7;_LL33D: if(
_tmp4E1 <= (void*)4)goto _LL33F;if(*((int*)_tmp4E1)!= 4)goto _LL33F;_tmp4E2=((
struct Cyc_Absyn_PointerType_struct*)_tmp4E1)->f1;_tmp4E3=(void*)_tmp4E2.elt_typ;
_tmp4E4=_tmp4E2.ptr_atts;_tmp4E5=_tmp4E4.nullable;_tmp4E6=_tmp4E4.bounds;_tmp4E7=
_tmp4E4.zero_term;_LL33E: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp4E5))({void*_tmp4E8[0]={};Cyc_Tcutil_terr(_tmp4CF,({const char*_tmp4E9="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp4E9,sizeof(char),_get_zero_arr_size(_tmp4E9,55));}),
_tag_dynforward(_tmp4E8,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp4E6))({
void*_tmp4EA[0]={};Cyc_Tcutil_terr(_tmp4CF,({const char*_tmp4EB="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp4EB,sizeof(char),_get_zero_arr_size(_tmp4EB,57));}),
_tag_dynforward(_tmp4EA,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp4E7))({void*_tmp4EC[0]={};Cyc_Tcutil_terr(_tmp4CF,({
const char*_tmp4ED="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp4ED,sizeof(char),_get_zero_arr_size(_tmp4ED,77));}),
_tag_dynforward(_tmp4EC,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp4EE=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp4F6=_cycalloc(sizeof(*
_tmp4F6));_tmp4F6[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp4F7;_tmp4F7.tag=
2;_tmp4F7.f1=(int)j;_tmp4F7.f2=(void*)_tmp4E3;_tmp4F7;});_tmp4F6;});struct Cyc_CfFlowInfo_Place*
_tmp4EF=({struct Cyc_CfFlowInfo_Place*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->root=(
void*)((void*)_tmp4EE);_tmp4F5->fields=0;_tmp4F5;});_tmp4D2=Cyc_Dict_insert(
_tmp4D2,(void*)_tmp4EE,Cyc_CfFlowInfo_typ_to_absrval(_tmp4E3,Cyc_CfFlowInfo_esc_none));
_tmp4D2=Cyc_Dict_insert(_tmp4D2,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp4F1;_tmp4F1.tag=0;_tmp4F1.f1=_tmp4E0;_tmp4F1;});_tmp4F0;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp4F3;_tmp4F3.tag=2;_tmp4F3.f1=_tmp4EF;
_tmp4F3;});_tmp4F2;}));_tmp4D0=({struct Cyc_List_List*_tmp4F4=_cycalloc(sizeof(*
_tmp4F4));_tmp4F4->hd=_tmp4EF;_tmp4F4->tl=_tmp4D0;_tmp4F4;});goto _LL33C;}_LL33F:;
_LL340:({void*_tmp4F8[0]={};Cyc_Tcutil_terr(_tmp4CF,({const char*_tmp4F9="initializes attribute on non-pointer";
_tag_dynforward(_tmp4F9,sizeof(char),_get_zero_arr_size(_tmp4F9,37));}),
_tag_dynforward(_tmp4F8,sizeof(void*),0));});_LL33C:;}goto _LL337;}}_LL33A:;
_LL33B: goto _LL337;_LL337:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp4FB;_tmp4FB.tag=0;_tmp4FB.f1=_tmp4D2;_tmp4FB.f2=_tmp4D3;_tmp4FB.f3=_tmp4D4;
_tmp4FB;});_tmp4FA;});}_LL32D:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->iterate_again=1;_tmp504->iteration_num=
0;_tmp504->in_try=0;_tmp504->tryflow=(void*)inflow;_tmp504->all_changed=0;
_tmp504->noreturn=noreturn;_tmp504->param_roots=_tmp4D0;_tmp504;});void*outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{void*_tmp4FC=
outflow;_LL342: if((int)_tmp4FC != 0)goto _LL344;_LL343: goto _LL341;_LL344:;_LL345:
Cyc_NewControlFlow_check_init_params(_tmp4CF,env,outflow);if(noreturn)({void*
_tmp4FD[0]={};Cyc_Tcutil_terr(_tmp4CF,({const char*_tmp4FE="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp4FE,sizeof(char),_get_zero_arr_size(_tmp4FE,46));}),
_tag_dynforward(_tmp4FD,sizeof(void*),0));});else{void*_tmp4FF=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL347: if((int)_tmp4FF != 0)goto _LL349;_LL348: goto _LL346;
_LL349: if(_tmp4FF <= (void*)4)goto _LL34B;if(*((int*)_tmp4FF)!= 6)goto _LL34B;
_LL34A: goto _LL34C;_LL34B: if((int)_tmp4FF != 1)goto _LL34D;_LL34C: goto _LL34E;_LL34D:
if(_tmp4FF <= (void*)4)goto _LL34F;if(*((int*)_tmp4FF)!= 5)goto _LL34F;_LL34E:({
void*_tmp500[0]={};Cyc_Tcutil_warn(_tmp4CF,({const char*_tmp501="function may not return a value";
_tag_dynforward(_tmp501,sizeof(char),_get_zero_arr_size(_tmp501,32));}),
_tag_dynforward(_tmp500,sizeof(void*),0));});goto _LL346;_LL34F:;_LL350:({void*
_tmp502[0]={};Cyc_Tcutil_terr(_tmp4CF,({const char*_tmp503="function may not return a value";
_tag_dynforward(_tmp503,sizeof(char),_get_zero_arr_size(_tmp503,32));}),
_tag_dynforward(_tmp502,sizeof(void*),0));});goto _LL346;_LL346:;}goto _LL341;
_LL341:;}Cyc_NewControlFlow_postpass(fd->body);}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp505=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp506;struct Cyc_List_List*_tmp507;struct Cyc_List_List*
_tmp508;struct Cyc_List_List*_tmp509;_LL352: if(_tmp505 <= (void*)2)goto _LL35C;if(*((
int*)_tmp505)!= 1)goto _LL354;_tmp506=((struct Cyc_Absyn_Fn_d_struct*)_tmp505)->f1;
_LL353: Cyc_NewControlFlow_check_fun(_tmp506);goto _LL351;_LL354: if(*((int*)
_tmp505)!= 10)goto _LL356;_tmp507=((struct Cyc_Absyn_ExternC_d_struct*)_tmp505)->f1;
_LL355: _tmp508=_tmp507;goto _LL357;_LL356: if(*((int*)_tmp505)!= 9)goto _LL358;
_tmp508=((struct Cyc_Absyn_Using_d_struct*)_tmp505)->f2;_LL357: _tmp509=_tmp508;
goto _LL359;_LL358: if(*((int*)_tmp505)!= 8)goto _LL35A;_tmp509=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp505)->f2;_LL359: Cyc_NewControlFlow_cf_check(_tmp509);goto _LL351;_LL35A: if(*((
int*)_tmp505)!= 11)goto _LL35C;_LL35B: goto _LL351;_LL35C:;_LL35D: goto _LL351;_LL351:;}}
