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
struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_List_List*_tmpF5;
struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_Absyn_Vardecl*
_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;int _tmpFB;struct Cyc_Absyn_Stmt*
_tmpFC;void*_tmpFD;_LL8F: if(*((int*)_tmpBA)!= 15)goto _LL91;_tmpBB=((struct Cyc_Absyn_Cast_e_struct*)
_tmpBA)->f2;_tmpBC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f4;if((int)
_tmpBC != 2)goto _LL91;_LL90: {void*_tmpFF;void*_tmp100;struct _tuple0 _tmpFE=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBB);_tmpFF=_tmpFE.f1;_tmp100=_tmpFE.f2;{void*_tmp102;void*_tmp103;
struct _tuple0 _tmp101=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmpFF,_tmpBB,
_tmp100);_tmp102=_tmp101.f1;_tmp103=_tmp101.f2;return({struct _tuple0 _tmp104;
_tmp104.f1=_tmp102;_tmp104.f2=_tmp100;_tmp104;});}}_LL91: if(*((int*)_tmpBA)!= 15)
goto _LL93;_tmpBD=((struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f2;_LL92: _tmpBE=
_tmpBD;goto _LL94;_LL93: if(*((int*)_tmpBA)!= 13)goto _LL95;_tmpBE=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpBA)->f1;_LL94: _tmpBF=_tmpBE;goto _LL96;_LL95: if(*((int*)_tmpBA)!= 14)goto
_LL97;_tmpBF=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpBA)->f1;_LL96: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpBF);_LL97: if(*((int*)_tmpBA)!= 0)goto _LL99;_tmpC0=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmpBA)->f1;if((int)_tmpC0 != 0)goto _LL99;_LL98: goto
_LL9A;_LL99: if(*((int*)_tmpBA)!= 0)goto _LL9B;_tmpC1=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpBA)->f1;if(_tmpC1 <= (void*)1)goto _LL9B;if(*((int*)_tmpC1)!= 2)goto _LL9B;
_tmpC2=((struct Cyc_Absyn_Int_c_struct*)_tmpC1)->f2;if(_tmpC2 != 0)goto _LL9B;_LL9A:
return({struct _tuple0 _tmp105;_tmp105.f1=inflow;_tmp105.f2=(void*)0;_tmp105;});
_LL9B: if(*((int*)_tmpBA)!= 0)goto _LL9D;_tmpC3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpBA)->f1;if(_tmpC3 <= (void*)1)goto _LL9D;if(*((int*)_tmpC3)!= 2)goto _LL9D;
_LL9C: goto _LL9E;_LL9D: if(*((int*)_tmpBA)!= 1)goto _LL9F;_tmpC4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f2;if(_tmpC4 <= (void*)1)goto _LL9F;if(*((int*)_tmpC4)!= 1)goto _LL9F;
_LL9E: return({struct _tuple0 _tmp106;_tmp106.f1=inflow;_tmp106.f2=(void*)1;_tmp106;});
_LL9F: if(*((int*)_tmpBA)!= 32)goto _LLA1;_tmpC5=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpBA)->f1;if(_tmpC5 != 0)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if(*((int*)_tmpBA)!= 0)
goto _LLA3;_LLA2: goto _LLA4;_LLA3: if(*((int*)_tmpBA)!= 19)goto _LLA5;_LLA4: goto
_LLA6;_LLA5: if(*((int*)_tmpBA)!= 18)goto _LLA7;_LLA6: goto _LLA8;_LLA7: if(*((int*)
_tmpBA)!= 20)goto _LLA9;_LLA8: goto _LLAA;_LLA9: if(*((int*)_tmpBA)!= 21)goto _LLAB;
_LLAA: goto _LLAC;_LLAB: if(*((int*)_tmpBA)!= 34)goto _LLAD;_LLAC: goto _LLAE;_LLAD:
if(*((int*)_tmpBA)!= 33)goto _LLAF;_LLAE: return({struct _tuple0 _tmp107;_tmp107.f1=
inflow;_tmp107.f2=Cyc_CfFlowInfo_unknown_all;_tmp107;});_LLAF: if(*((int*)_tmpBA)
!= 1)goto _LLB1;_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(
_tmpC6 <= (void*)1)goto _LLB1;if(*((int*)_tmpC6)!= 0)goto _LLB1;_LLB0: return({
struct _tuple0 _tmp108;_tmp108.f1=inflow;_tmp108.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp108;});_LLB1: if(*((int*)_tmpBA)!= 1)goto _LLB3;_tmpC7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f2;if(_tmpC7 <= (void*)1)goto _LLB3;if(*((int*)_tmpC7)!= 2)goto _LLB3;
_tmpC8=((struct Cyc_Absyn_Param_b_struct*)_tmpC7)->f1;_LLB2: _tmpCA=_tmpC8;goto
_LLB4;_LLB3: if(*((int*)_tmpBA)!= 1)goto _LLB5;_tmpC9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f2;if(_tmpC9 <= (void*)1)goto _LLB5;if(*((int*)_tmpC9)!= 3)goto _LLB5;
_tmpCA=((struct Cyc_Absyn_Local_b_struct*)_tmpC9)->f1;_LLB4: _tmpCC=_tmpCA;goto
_LLB6;_LLB5: if(*((int*)_tmpBA)!= 1)goto _LLB7;_tmpCB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f2;if(_tmpCB <= (void*)1)goto _LLB7;if(*((int*)_tmpCB)!= 4)goto _LLB7;
_tmpCC=((struct Cyc_Absyn_Pat_b_struct*)_tmpCB)->f1;_LLB6: if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)_tmpCC->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp109=({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[
0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp110;_tmp110.tag=0;_tmp110.f1=_tmpCC;
_tmp110;});_tmp10F;});struct Cyc_CfFlowInfo_Place*_tmp10A=({struct Cyc_CfFlowInfo_Place*
_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->root=(void*)((void*)_tmp109);
_tmp10E->fields=0;_tmp10E;});cinfo.may_consume=((struct Cyc_List_List*(*)(int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=_tmp10A;_tmp10B->tl=0;_tmp10B;}),
cinfo.may_consume);inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp10D;_tmp10D.tag=0;_tmp10D.f1=d;_tmp10D.f2=relns;_tmp10D.f3=cinfo;_tmp10D;});
_tmp10C;});}return({struct _tuple0 _tmp111;_tmp111.f1=inflow;_tmp111.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp112=_cycalloc(sizeof(*_tmp112));
_tmp112[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp113;_tmp113.tag=0;_tmp113.f1=
_tmpCC;_tmp113;});_tmp112;}));_tmp111;});_LLB7: if(*((int*)_tmpBA)!= 3)goto _LLB9;
_tmpCD=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmpBA)->f1;_tmpCE=((struct Cyc_Absyn_Primop_e_struct*)
_tmpBA)->f2;_LLB8: {void*_tmp115;void*_tmp116;struct _tuple0 _tmp114=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpCE,0);_tmp115=_tmp114.f1;_tmp116=_tmp114.f2;{void*_tmp117=_tmpCD;
_LLF2: if((int)_tmp117 != 0)goto _LLF4;_LLF3: goto _LLF5;_LLF4: if((int)_tmp117 != 2)
goto _LLF6;_LLF5: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpCE))->hd)->loc,_tmp115);goto _LLF1;_LLF6:;
_LLF7: _tmp115=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpCE))->hd)->loc,_tmp115);goto _LLF1;_LLF1:;}
return({struct _tuple0 _tmp118;_tmp118.f1=_tmp115;_tmp118.f2=_tmp116;_tmp118;});}
_LLB9: if(*((int*)_tmpBA)!= 5)goto _LLBB;_tmpCF=((struct Cyc_Absyn_Increment_e_struct*)
_tmpBA)->f1;_LLBA: {void*_tmp11B;struct _tuple0 _tmp11A=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmp119[1];_tmp119[0]=_tmpCF;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp119,sizeof(struct Cyc_Absyn_Exp*),
1));}),0);_tmp11B=_tmp11A.f1;Cyc_CfFlowInfo_check_unique_rvals(_tmpCF->loc,
_tmp11B);{void*_tmp11D;struct _tuple0 _tmp11C=Cyc_NewControlFlow_anal_Lexp(env,
_tmp11B,_tmpCF);_tmp11D=_tmp11C.f2;{struct _tuple0 _tmp11F=({struct _tuple0 _tmp11E;
_tmp11E.f1=_tmp11D;_tmp11E.f2=_tmp11B;_tmp11E;});void*_tmp120;struct Cyc_CfFlowInfo_Place*
_tmp121;void*_tmp122;struct Cyc_Dict_Dict*_tmp123;struct Cyc_List_List*_tmp124;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp125;_LLF9: _tmp120=_tmp11F.f1;if(_tmp120 <= (
void*)1)goto _LLFB;if(*((int*)_tmp120)!= 0)goto _LLFB;_tmp121=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp120)->f1;_tmp122=_tmp11F.f2;if(_tmp122 <= (void*)1)goto _LLFB;if(*((int*)
_tmp122)!= 0)goto _LLFB;_tmp123=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp122)->f1;_tmp124=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp122)->f2;
_tmp125=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp122)->f3;_LLFA: _tmp124=
Cyc_CfFlowInfo_reln_kill_exp(_tmp124,_tmpCF);_tmp11B=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp127;_tmp127.tag=0;_tmp127.f1=Cyc_CfFlowInfo_assign_place(_tmpCF->loc,_tmp123,
env->all_changed,_tmp121,Cyc_CfFlowInfo_unknown_all);_tmp127.f2=_tmp124;_tmp127.f3=
_tmp125;_tmp127;});_tmp126;});goto _LLF8;_LLFB:;_LLFC: goto _LLF8;_LLF8:;}return({
struct _tuple0 _tmp128;_tmp128.f1=_tmp11B;_tmp128.f2=Cyc_CfFlowInfo_unknown_all;
_tmp128;});}}_LLBB: if(*((int*)_tmpBA)!= 4)goto _LLBD;_tmpD0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBA)->f1;_tmpD1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f2;if(_tmpD1 == 
0)goto _LLBD;_tmpD2=*_tmpD1;_tmpD3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f3;
_LLBC: {void*_tmp12B;struct _tuple0 _tmp12A=Cyc_NewControlFlow_anal_use_ints(env,
inflow,({struct Cyc_Absyn_Exp*_tmp129[2];_tmp129[1]=_tmpD3;_tmp129[0]=_tmpD0;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp129,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp12B=_tmp12A.f1;{void*_tmp12D;
struct _tuple0 _tmp12C=Cyc_NewControlFlow_anal_Lexp(env,_tmp12B,e);_tmp12D=_tmp12C.f2;
_tmp12B=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp12B);{struct _tuple0
_tmp12F=({struct _tuple0 _tmp12E;_tmp12E.f1=_tmp12D;_tmp12E.f2=_tmp12B;_tmp12E;});
void*_tmp130;struct Cyc_CfFlowInfo_Place*_tmp131;void*_tmp132;struct Cyc_Dict_Dict*
_tmp133;struct Cyc_List_List*_tmp134;struct Cyc_CfFlowInfo_ConsumeInfo _tmp135;
_LLFE: _tmp130=_tmp12F.f1;if(_tmp130 <= (void*)1)goto _LL100;if(*((int*)_tmp130)!= 
0)goto _LL100;_tmp131=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp130)->f1;_tmp132=
_tmp12F.f2;if(_tmp132 <= (void*)1)goto _LL100;if(*((int*)_tmp132)!= 0)goto _LL100;
_tmp133=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp132)->f1;_tmp134=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp132)->f2;_tmp135=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp132)->f3;_LLFF: _tmp134=Cyc_CfFlowInfo_reln_kill_exp(_tmp134,_tmpD0);_tmp133=
Cyc_CfFlowInfo_assign_place(_tmpD0->loc,_tmp133,env->all_changed,_tmp131,Cyc_CfFlowInfo_unknown_all);
if(Cyc_CfFlowInfo_contains_region((void*)0,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD0->topt))->v)){_tmp135.may_consume=((struct Cyc_List_List*(*)(
int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=_tmp131;_tmp136->tl=0;_tmp136;}),
_tmp135.may_consume);if(!((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
k))Cyc_Dict_member)(_tmp135.consumed,_tmp131)){struct Cyc_CfFlowInfo_Place _tmp138;
void*_tmp139;struct Cyc_CfFlowInfo_Place*_tmp137=_tmp131;_tmp138=*_tmp137;_tmp139=(
void*)_tmp138.root;{void*_tmp13A=_tmp139;struct Cyc_Absyn_Vardecl*_tmp13B;_LL103:
if(*((int*)_tmp13A)!= 0)goto _LL105;_tmp13B=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp13A)->f1;_LL104: {struct _dynforward_ptr _tmp13C=Cyc_Absynpp_qvar2string(
_tmp13B->name);({struct Cyc_String_pa_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp13C);{void*_tmp13D[1]={&
_tmp13F};Cyc_Tcutil_warn(_tmpD0->loc,({const char*_tmp13E="may clobber unique pointer %s";
_tag_dynforward(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,30));}),
_tag_dynforward(_tmp13D,sizeof(void*),1));}});goto _LL102;}_LL105:;_LL106:({void*
_tmp140[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp141="warning locations not for VarRoots";_tag_dynforward(_tmp141,
sizeof(char),_get_zero_arr_size(_tmp141,35));}),_tag_dynforward(_tmp140,sizeof(
void*),0));});_LL102:;}}if(env->all_changed == 0){struct Cyc_Dict_Dict*_tmp142=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(
_tmp135.consumed,_tmp131);if(_tmp135.consumed != _tmp142);_tmp135.consumed=
_tmp142;}}_tmp12B=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp143=
_cycalloc(sizeof(*_tmp143));_tmp143[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp144;_tmp144.tag=0;_tmp144.f1=_tmp133;_tmp144.f2=_tmp134;_tmp144.f3=_tmp135;
_tmp144;});_tmp143;});goto _LLFD;_LL100:;_LL101: goto _LLFD;_LLFD:;}return({struct
_tuple0 _tmp145;_tmp145.f1=_tmp12B;_tmp145.f2=Cyc_CfFlowInfo_unknown_all;_tmp145;});}}
_LLBD: if(*((int*)_tmpBA)!= 4)goto _LLBF;_tmpD4=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBA)->f1;_tmpD5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f2;if(_tmpD5 != 
0)goto _LLBF;_tmpD6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f3;_LLBE: {
struct Cyc_Dict_Dict**_tmp146=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp148;void*_tmp149;struct _tuple5 _tmp147=Cyc_CfFlowInfo_save_consume_info(
inflow,1);_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp172;_tmp172.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp172.may_consume=0;_tmp172;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict**
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp14A;});{void*_tmp14C;void*_tmp14D;struct _tuple0 _tmp14B=Cyc_NewControlFlow_anal_Lexp(
env,_tmp149,_tmpD4);_tmp14C=_tmp14B.f1;_tmp14D=_tmp14B.f2;{struct Cyc_Dict_Dict*
_tmp14E=*((struct Cyc_Dict_Dict**)_check_null(env->all_changed));env->all_changed=({
struct Cyc_Dict_Dict**_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp14F;});{void*_tmp151;void*_tmp152;struct _tuple0 _tmp150=Cyc_NewControlFlow_anal_Rexp(
env,_tmp149,_tmpD6);_tmp151=_tmp150.f1;_tmp152=_tmp150.f2;{struct Cyc_Dict_Dict*
_tmp153=*((struct Cyc_Dict_Dict**)_check_null(env->all_changed));void*_tmp154=Cyc_CfFlowInfo_after_flow((
struct Cyc_Dict_Dict**)& _tmp14E,_tmp14C,_tmp151,_tmp14E,_tmp153);void*_tmp155=Cyc_CfFlowInfo_join_flow(
_tmp146,_tmp149,_tmp154,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp157;struct _tuple5
_tmp156=Cyc_CfFlowInfo_save_consume_info(_tmp155,0);_tmp157=_tmp156.f1;if(
init_consume)_tmp149=Cyc_CfFlowInfo_restore_consume_info(_tmp149,outflow_consume);
outflow_consume=_tmp157;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp155,_tmp149)){if(_tmp146 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict**)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp146,Cyc_CfFlowInfo_union_place_set(
_tmp14E,_tmp153,0),0);}_tmp154=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp154);{void*_tmp158=_tmp154;struct Cyc_Dict_Dict*_tmp159;struct Cyc_List_List*
_tmp15A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp15B;_LL108: if((int)_tmp158 != 0)goto
_LL10A;_LL109: return({struct _tuple0 _tmp15C;_tmp15C.f1=(void*)0;_tmp15C.f2=
_tmp152;_tmp15C;});_LL10A: if(_tmp158 <= (void*)1)goto _LL107;if(*((int*)_tmp158)!= 
0)goto _LL107;_tmp159=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp158)->f1;
_tmp15A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp158)->f2;_tmp15B=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp158)->f3;_LL10B: _tmp15B=Cyc_CfFlowInfo_and_consume(
_tmp148,_tmp15B);{void*_tmp15D=_tmp14D;struct Cyc_CfFlowInfo_Place*_tmp15E;_LL10D:
if(_tmp15D <= (void*)1)goto _LL10F;if(*((int*)_tmp15D)!= 0)goto _LL10F;_tmp15E=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp15D)->f1;_LL10E: _tmp159=Cyc_CfFlowInfo_assign_place(
e->loc,_tmp159,env->all_changed,_tmp15E,_tmp152);if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD4->topt))->v)){_tmp15B.may_consume=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->hd=_tmp15E;
_tmp15F->tl=0;_tmp15F;}),_tmp15B.may_consume);if(!((int(*)(struct Cyc_Dict_Dict*d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp15B.consumed,_tmp15E)){struct
Cyc_CfFlowInfo_Place _tmp161;void*_tmp162;struct Cyc_CfFlowInfo_Place*_tmp160=
_tmp15E;_tmp161=*_tmp160;_tmp162=(void*)_tmp161.root;{void*_tmp163=_tmp162;
struct Cyc_Absyn_Vardecl*_tmp164;_LL112: if(*((int*)_tmp163)!= 0)goto _LL114;
_tmp164=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp163)->f1;_LL113: {struct
_dynforward_ptr _tmp165=Cyc_Absynpp_qvar2string(_tmp164->name);({struct Cyc_String_pa_struct
_tmp168;_tmp168.tag=0;_tmp168.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp165);{void*_tmp166[1]={& _tmp168};Cyc_Tcutil_warn(_tmpD4->loc,({const char*
_tmp167="may clobber unique pointer %s";_tag_dynforward(_tmp167,sizeof(char),
_get_zero_arr_size(_tmp167,30));}),_tag_dynforward(_tmp166,sizeof(void*),1));}});
goto _LL111;}_LL114:;_LL115:({void*_tmp169[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp16A="warning locations not for VarRoots";
_tag_dynforward(_tmp16A,sizeof(char),_get_zero_arr_size(_tmp16A,35));}),
_tag_dynforward(_tmp169,sizeof(void*),0));});_LL111:;}}if(env->all_changed == 0){
struct Cyc_Dict_Dict*_tmp16B=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,
struct Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(_tmp15B.consumed,_tmp15E);if(
_tmp15B.consumed != _tmp16B);_tmp15B.consumed=_tmp16B;}}_tmp15A=Cyc_CfFlowInfo_reln_assign_exp(
_tmp15A,_tmpD4,_tmpD6);_tmp154=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp16D;_tmp16D.tag=0;_tmp16D.f1=_tmp159;_tmp16D.f2=_tmp15A;_tmp16D.f3=_tmp15B;
_tmp16D;});_tmp16C;});Cyc_NewControlFlow_update_tryflow(env,_tmp154);return({
struct _tuple0 _tmp16E;_tmp16E.f1=_tmp154;_tmp16E.f2=_tmp152;_tmp16E;});_LL10F: if((
int)_tmp15D != 0)goto _LL10C;_LL110: if(Cyc_CfFlowInfo_contains_region((void*)0,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD4->topt))->v))({void*_tmp16F[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp170="dest of unique assignment is unknown!";_tag_dynforward(
_tmp170,sizeof(char),_get_zero_arr_size(_tmp170,38));}),_tag_dynforward(_tmp16F,
sizeof(void*),0));});return({struct _tuple0 _tmp171;_tmp171.f1=Cyc_NewControlFlow_use_Rval(
env,_tmpD6->loc,_tmp154,_tmp152);_tmp171.f2=_tmp152;_tmp171;});_LL10C:;}_LL107:;}}
_tmp149=Cyc_CfFlowInfo_restore_consume_info(_tmp155,empty_consume);}}}}}}}_LLBF:
if(*((int*)_tmpBA)!= 9)goto _LLC1;_tmpD7=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBA)->f1;_tmpD8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpBA)->f2;_LLC0: {void*
_tmp174;void*_tmp175;struct _tuple0 _tmp173=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpD7);_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;_tmp174=Cyc_CfFlowInfo_drop_unique_rvals(
_tmpD7->loc,_tmp174);return Cyc_NewControlFlow_anal_Rexp(env,_tmp174,_tmpD8);}
_LLC1: if(*((int*)_tmpBA)!= 12)goto _LLC3;_tmpD9=((struct Cyc_Absyn_Throw_e_struct*)
_tmpBA)->f1;_LLC2: {void*_tmp177;void*_tmp178;struct _tuple0 _tmp176=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpD9);_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;_tmp177=Cyc_CfFlowInfo_consume_unique_rvals(
_tmpD9->loc,_tmp177);Cyc_NewControlFlow_use_Rval(env,_tmpD9->loc,_tmp177,_tmp178);
return({struct _tuple0 _tmp179;_tmp179.f1=(void*)0;_tmp179.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp179;});}_LLC3: if(*((int*)_tmpBA)!= 11)goto _LLC5;_tmpDA=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpBA)->f1;_tmpDB=((struct Cyc_Absyn_FnCall_e_struct*)_tmpBA)->f2;_LLC4: {void*
_tmp17C;struct Cyc_List_List*_tmp17D;struct _tuple7 _tmp17B=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->hd=
_tmpDA;_tmp17A->tl=_tmpDB;_tmp17A;}),1);_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;
_tmp17C=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp17C);{void*_tmp17E=Cyc_NewControlFlow_use_Rval(
env,_tmpDA->loc,_tmp17C,(void*)((struct Cyc_List_List*)_check_null(_tmp17D))->hd);
_tmp17D=_tmp17D->tl;{struct Cyc_List_List*init_params=0;{void*_tmp17F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDA->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp180;void*_tmp181;_LL117: if(_tmp17F <= (void*)4)goto _LL119;if(*((int*)_tmp17F)
!= 4)goto _LL119;_tmp180=((struct Cyc_Absyn_PointerType_struct*)_tmp17F)->f1;
_tmp181=(void*)_tmp180.elt_typ;_LL118:{void*_tmp182=Cyc_Tcutil_compress(_tmp181);
struct Cyc_Absyn_FnInfo _tmp183;struct Cyc_List_List*_tmp184;_LL11C: if(_tmp182 <= (
void*)4)goto _LL11E;if(*((int*)_tmp182)!= 8)goto _LL11E;_tmp183=((struct Cyc_Absyn_FnType_struct*)
_tmp182)->f1;_tmp184=_tmp183.attributes;_LL11D: for(0;_tmp184 != 0;_tmp184=_tmp184->tl){
void*_tmp185=(void*)_tmp184->hd;int _tmp186;_LL121: if(_tmp185 <= (void*)17)goto
_LL123;if(*((int*)_tmp185)!= 4)goto _LL123;_tmp186=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp185)->f1;_LL122: init_params=({struct Cyc_List_List*_tmp187=_cycalloc(sizeof(*
_tmp187));_tmp187->hd=(void*)_tmp186;_tmp187->tl=init_params;_tmp187;});goto
_LL120;_LL123:;_LL124: goto _LL120;_LL120:;}goto _LL11B;_LL11E:;_LL11F:({void*
_tmp188[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp189="anal_Rexp: bad function type";_tag_dynforward(_tmp189,sizeof(
char),_get_zero_arr_size(_tmp189,29));}),_tag_dynforward(_tmp188,sizeof(void*),0));});
_LL11B:;}goto _LL116;_LL119:;_LL11A:({void*_tmp18A[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp18B="anal_Rexp: bad function type";_tag_dynforward(_tmp18B,sizeof(char),
_get_zero_arr_size(_tmp18B,29));}),_tag_dynforward(_tmp18A,sizeof(void*),0));});
_LL116:;}{int i=1;for(0;_tmp17D != 0;(((_tmp17D=_tmp17D->tl,_tmpDB=((struct Cyc_List_List*)
_check_null(_tmpDB))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp17E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpDB))->hd)->loc,_tmp17E,(void*)_tmp17D->hd);
continue;}{void*_tmp18C=_tmp17C;struct Cyc_Dict_Dict*_tmp18D;_LL126: if((int)
_tmp18C != 0)goto _LL128;_LL127: goto _LL125;_LL128: if(_tmp18C <= (void*)1)goto _LL125;
if(*((int*)_tmp18C)!= 0)goto _LL125;_tmp18D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp18C)->f1;_LL129: if(Cyc_CfFlowInfo_initlevel(_tmp18D,(void*)_tmp17D->hd)== (
void*)0)({void*_tmp18E[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpDB))->hd)->loc,({const char*_tmp18F="expression may not be initialized";
_tag_dynforward(_tmp18F,sizeof(char),_get_zero_arr_size(_tmp18F,34));}),
_tag_dynforward(_tmp18E,sizeof(void*),0));});{void*_tmp190=_tmp17E;struct Cyc_Dict_Dict*
_tmp191;struct Cyc_List_List*_tmp192;struct Cyc_CfFlowInfo_ConsumeInfo _tmp193;
_LL12B: if((int)_tmp190 != 0)goto _LL12D;_LL12C: goto _LL12A;_LL12D: if(_tmp190 <= (
void*)1)goto _LL12A;if(*((int*)_tmp190)!= 0)goto _LL12A;_tmp191=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp190)->f1;_tmp192=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp190)->f2;
_tmp193=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp190)->f3;_LL12E: {struct
Cyc_Dict_Dict*_tmp194=Cyc_CfFlowInfo_escape_deref(_tmp191,env->all_changed,(void*)
_tmp17D->hd);{void*_tmp195=(void*)_tmp17D->hd;struct Cyc_CfFlowInfo_Place*_tmp196;
_LL130: if(_tmp195 <= (void*)3)goto _LL132;if(*((int*)_tmp195)!= 2)goto _LL132;
_tmp196=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp195)->f1;_LL131:{void*
_tmp197=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpDB))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp198;void*_tmp199;_LL135: if(_tmp197 <= (void*)4)goto _LL137;if(*((int*)_tmp197)
!= 4)goto _LL137;_tmp198=((struct Cyc_Absyn_PointerType_struct*)_tmp197)->f1;
_tmp199=(void*)_tmp198.elt_typ;_LL136: _tmp194=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpDB->hd)->loc,_tmp194,env->all_changed,_tmp196,Cyc_CfFlowInfo_typ_to_absrval(
_tmp199,Cyc_CfFlowInfo_esc_all));goto _LL134;_LL137:;_LL138:({void*_tmp19A[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp19B="anal_Rexp: bad type for initialized arg";_tag_dynforward(
_tmp19B,sizeof(char),_get_zero_arr_size(_tmp19B,40));}),_tag_dynforward(_tmp19A,
sizeof(void*),0));});_LL134:;}goto _LL12F;_LL132:;_LL133: goto _LL12F;_LL12F:;}
_tmp17E=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp19C=_cycalloc(
sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp19D;
_tmp19D.tag=0;_tmp19D.f1=_tmp194;_tmp19D.f2=_tmp192;_tmp19D.f3=_tmp193;_tmp19D;});
_tmp19C;});goto _LL12A;}_LL12A:;}goto _LL125;_LL125:;}}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDA->topt))->v))return({struct _tuple0
_tmp19E;_tmp19E.f1=(void*)0;_tmp19E.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp19E;});
else{return({struct _tuple0 _tmp19F;_tmp19F.f1=_tmp17E;_tmp19F.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp19F;});}}}}_LLC5: if(*((int*)_tmpBA)!= 35)goto _LLC7;_tmpDC=((struct Cyc_Absyn_Malloc_e_struct*)
_tmpBA)->f1;_tmpDD=_tmpDC.is_calloc;_tmpDE=_tmpDC.rgn;_tmpDF=_tmpDC.elt_type;
_tmpE0=_tmpDC.num_elts;_tmpE1=_tmpDC.fat_result;_LLC6: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp1B2;_tmp1B2.tag=1;_tmp1B2.f1=_tmpE0;
_tmp1B2.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp1B2;});_tmp1B1;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->root=(void*)root;_tmp1B0->fields=0;
_tmp1B0;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1AE=
_cycalloc(sizeof(*_tmp1AE));_tmp1AE[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1AF;_tmp1AF.tag=2;_tmp1AF.f1=place;_tmp1AF;});_tmp1AE;});void*place_val=
_tmpE1?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpDF)),Cyc_CfFlowInfo_unknown_none);
void*outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmpDE
!= 0){void*_tmp1A2;struct Cyc_List_List*_tmp1A3;struct _tuple7 _tmp1A1=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp1A0[2];_tmp1A0[1]=_tmpE0;_tmp1A0[0]=(struct
Cyc_Absyn_Exp*)_tmpDE;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1A0,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1A2=_tmp1A1.f1;
_tmp1A3=_tmp1A1.f2;outflow=_tmp1A2;}else{void*_tmp1A5;struct _tuple0 _tmp1A4=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpE0);_tmp1A5=_tmp1A4.f1;outflow=_tmp1A5;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpE0->loc,outflow);{void*_tmp1A6=outflow;struct Cyc_Dict_Dict*_tmp1A7;struct Cyc_List_List*
_tmp1A8;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A9;_LL13A: if((int)_tmp1A6 != 0)goto
_LL13C;_LL13B: return({struct _tuple0 _tmp1AA;_tmp1AA.f1=outflow;_tmp1AA.f2=rval;
_tmp1AA;});_LL13C: if(_tmp1A6 <= (void*)1)goto _LL139;if(*((int*)_tmp1A6)!= 0)goto
_LL139;_tmp1A7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A6)->f1;_tmp1A8=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A6)->f2;_tmp1A9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1A6)->f3;_LL13D: return({struct _tuple0 _tmp1AB;_tmp1AB.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1AD;_tmp1AD.tag=0;_tmp1AD.f1=Cyc_Dict_insert(_tmp1A7,root,place_val);_tmp1AD.f2=
_tmp1A8;_tmp1AD.f3=_tmp1A9;_tmp1AD;});_tmp1AC;});_tmp1AB.f2=rval;_tmp1AB;});
_LL139:;}}_LLC7: if(*((int*)_tmpBA)!= 17)goto _LLC9;_tmpE2=((struct Cyc_Absyn_New_e_struct*)
_tmpBA)->f1;_tmpE3=((struct Cyc_Absyn_New_e_struct*)_tmpBA)->f2;_LLC8: {void*root=(
void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));
_tmp1C5[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp1C6;_tmp1C6.tag=1;_tmp1C6.f1=
_tmpE3;_tmp1C6.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp1C6;});_tmp1C5;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->root=(void*)root;_tmp1C4->fields=0;
_tmp1C4;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1C2=
_cycalloc(sizeof(*_tmp1C2));_tmp1C2[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1C3;_tmp1C3.tag=2;_tmp1C3.f1=place;_tmp1C3;});_tmp1C2;});Cyc_CfFlowInfo_update_place_set(
env->all_changed,place,0);{void*outflow;void*place_val;if(_tmpE2 != 0){void*
_tmp1B5;struct Cyc_List_List*_tmp1B6;struct _tuple7 _tmp1B4=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp1B3[2];_tmp1B3[1]=_tmpE3;_tmp1B3[0]=(struct
Cyc_Absyn_Exp*)_tmpE2;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1B3,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp1B5=_tmp1B4.f1;
_tmp1B6=_tmp1B4.f2;outflow=_tmp1B5;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp1B6))->tl))->hd;}else{void*
_tmp1B8;void*_tmp1B9;struct _tuple0 _tmp1B7=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE3);_tmp1B8=_tmp1B7.f1;_tmp1B9=_tmp1B7.f2;outflow=_tmp1B8;place_val=
_tmp1B9;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpE3->loc,outflow);{
void*_tmp1BA=outflow;struct Cyc_Dict_Dict*_tmp1BB;struct Cyc_List_List*_tmp1BC;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1BD;_LL13F: if((int)_tmp1BA != 0)goto _LL141;
_LL140: return({struct _tuple0 _tmp1BE;_tmp1BE.f1=outflow;_tmp1BE.f2=rval;_tmp1BE;});
_LL141: if(_tmp1BA <= (void*)1)goto _LL13E;if(*((int*)_tmp1BA)!= 0)goto _LL13E;
_tmp1BB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1BA)->f1;_tmp1BC=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1BA)->f2;_tmp1BD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1BA)->f3;_LL142: return({struct _tuple0 _tmp1BF;_tmp1BF.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1C1;_tmp1C1.tag=0;_tmp1C1.f1=Cyc_Dict_insert(_tmp1BB,root,place_val);_tmp1C1.f2=
_tmp1BC;_tmp1C1.f3=_tmp1BD;_tmp1C1;});_tmp1C0;});_tmp1BF.f2=rval;_tmp1BF;});
_LL13E:;}}}_LLC9: if(*((int*)_tmpBA)!= 16)goto _LLCB;_tmpE4=((struct Cyc_Absyn_Address_e_struct*)
_tmpBA)->f1;_LLCA: {void*_tmp1C8;void*_tmp1C9;struct _tuple0 _tmp1C7=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpE4);_tmp1C8=_tmp1C7.f1;_tmp1C9=_tmp1C7.f2;{void*_tmp1CA=_tmp1C9;
struct Cyc_CfFlowInfo_Place*_tmp1CB;_LL144: if((int)_tmp1CA != 0)goto _LL146;_LL145:
return({struct _tuple0 _tmp1CC;_tmp1CC.f1=_tmp1C8;_tmp1CC.f2=(void*)1;_tmp1CC;});
_LL146: if(_tmp1CA <= (void*)1)goto _LL143;if(*((int*)_tmp1CA)!= 0)goto _LL143;
_tmp1CB=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp1CA)->f1;_LL147: return({struct
_tuple0 _tmp1CD;_tmp1CD.f1=_tmp1C8;_tmp1CD.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1CF;_tmp1CF.tag=2;_tmp1CF.f1=_tmp1CB;_tmp1CF;});_tmp1CE;});_tmp1CD;});_LL143:;}}
_LLCB: if(*((int*)_tmpBA)!= 22)goto _LLCD;_tmpE5=((struct Cyc_Absyn_Deref_e_struct*)
_tmpBA)->f1;_LLCC: {void*_tmp1D1;void*_tmp1D2;struct _tuple0 _tmp1D0=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpE5);_tmp1D1=_tmp1D0.f1;_tmp1D2=_tmp1D0.f2;_tmp1D1=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1D1);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1D1,_tmpE5,
_tmp1D2);}_LLCD: if(*((int*)_tmpBA)!= 23)goto _LLCF;_tmpE6=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpBA)->f1;_tmpE7=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpBA)->f2;_LLCE: {
void*_tmp1D4;void*_tmp1D5;struct _tuple0 _tmp1D3=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE6);_tmp1D4=_tmp1D3.f1;_tmp1D5=_tmp1D3.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE6->topt))->v))return({struct _tuple0
_tmp1D6;_tmp1D6.f1=_tmp1D4;_tmp1D6.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp1D6;});{
void*_tmp1D7=_tmp1D5;struct Cyc_Dict_Dict*_tmp1D8;_LL149: if(_tmp1D7 <= (void*)3)
goto _LL14B;if(*((int*)_tmp1D7)!= 4)goto _LL14B;_tmp1D8=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1D7)->f1;_LL14A: return({struct _tuple0 _tmp1D9;_tmp1D9.f1=_tmp1D4;_tmp1D9.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp1D8,
_tmpE7);_tmp1D9;});_LL14B:;_LL14C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct Cyc_Core_Impossible_struct
_tmp1DB;_tmp1DB.tag=Cyc_Core_Impossible;_tmp1DB.f1=({const char*_tmp1DC="anal_Rexp: AggrMember";
_tag_dynforward(_tmp1DC,sizeof(char),_get_zero_arr_size(_tmp1DC,22));});_tmp1DB;});
_tmp1DA;}));_LL148:;}}_LLCF: if(*((int*)_tmpBA)!= 24)goto _LLD1;_tmpE8=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpBA)->f1;_tmpE9=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpBA)->f2;_LLD0: {
void*_tmp1DE;void*_tmp1DF;struct _tuple0 _tmp1DD=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE8);_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;_tmp1DE=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1DE);{void*_tmp1E1;void*_tmp1E2;struct _tuple0 _tmp1E0=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp1DE,_tmpE8,_tmp1DF);_tmp1E1=_tmp1E0.f1;_tmp1E2=_tmp1E0.f2;{void*
_tmp1E3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpE8->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1E4;void*_tmp1E5;_LL14E: if(_tmp1E3 <= (void*)4)goto
_LL150;if(*((int*)_tmp1E3)!= 4)goto _LL150;_tmp1E4=((struct Cyc_Absyn_PointerType_struct*)
_tmp1E3)->f1;_tmp1E5=(void*)_tmp1E4.elt_typ;_LL14F: if(Cyc_Absyn_is_union_type(
_tmp1E5))return({struct _tuple0 _tmp1E6;_tmp1E6.f1=_tmp1E1;_tmp1E6.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1E6;});goto _LL14D;_LL150:;_LL151:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7[0]=({struct Cyc_Core_Impossible_struct
_tmp1E8;_tmp1E8.tag=Cyc_Core_Impossible;_tmp1E8.f1=({const char*_tmp1E9="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp1E9,sizeof(char),_get_zero_arr_size(_tmp1E9,25));});_tmp1E8;});
_tmp1E7;}));_LL14D:;}{void*_tmp1EA=_tmp1E2;struct Cyc_Dict_Dict*_tmp1EB;_LL153:
if(_tmp1EA <= (void*)3)goto _LL155;if(*((int*)_tmp1EA)!= 4)goto _LL155;_tmp1EB=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1EA)->f1;_LL154: return({struct _tuple0
_tmp1EC;_tmp1EC.f1=_tmp1E1;_tmp1EC.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp1EB,_tmpE9);_tmp1EC;});_LL155:;_LL156:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1ED=_cycalloc(sizeof(*
_tmp1ED));_tmp1ED[0]=({struct Cyc_Core_Impossible_struct _tmp1EE;_tmp1EE.tag=Cyc_Core_Impossible;
_tmp1EE.f1=({const char*_tmp1EF="anal_Rexp: AggrArrow";_tag_dynforward(_tmp1EF,
sizeof(char),_get_zero_arr_size(_tmp1EF,21));});_tmp1EE;});_tmp1ED;}));_LL152:;}}}
_LLD1: if(*((int*)_tmpBA)!= 6)goto _LLD3;_tmpEA=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpBA)->f1;_tmpEB=((struct Cyc_Absyn_Conditional_e_struct*)_tmpBA)->f2;_tmpEC=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpBA)->f3;_LLD2: {void*_tmp1F1;void*
_tmp1F2;struct _tuple0 _tmp1F0=Cyc_NewControlFlow_anal_test(env,inflow,_tmpEA);
_tmp1F1=_tmp1F0.f1;_tmp1F2=_tmp1F0.f2;_tmp1F1=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEA->loc,_tmp1F1);_tmp1F2=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpEA->loc,
_tmp1F2);{struct _tuple0 _tmp1F3=Cyc_NewControlFlow_anal_Rexp(env,_tmp1F1,_tmpEB);
struct _tuple0 _tmp1F4=Cyc_NewControlFlow_anal_Rexp(env,_tmp1F2,_tmpEC);return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1F3,_tmp1F4,1);}}_LLD3: if(*((int*)_tmpBA)!= 7)goto _LLD5;
_tmpED=((struct Cyc_Absyn_And_e_struct*)_tmpBA)->f1;_tmpEE=((struct Cyc_Absyn_And_e_struct*)
_tmpBA)->f2;_LLD4: {void*_tmp1F6;void*_tmp1F7;struct _tuple0 _tmp1F5=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpED);_tmp1F6=_tmp1F5.f1;_tmp1F7=_tmp1F5.f2;_tmp1F6=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpED->loc,_tmp1F6);_tmp1F7=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpED->loc,
_tmp1F7);{void*_tmp1F9;void*_tmp1FA;struct _tuple0 _tmp1F8=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1F6,_tmpEE);_tmp1F9=_tmp1F8.f1;_tmp1FA=_tmp1F8.f2;_tmp1F9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEE->loc,_tmp1F9);{struct _tuple0 _tmp1FB=({struct _tuple0 _tmp1FE;_tmp1FE.f1=
_tmp1F9;_tmp1FE.f2=_tmp1FA;_tmp1FE;});struct _tuple0 _tmp1FC=({struct _tuple0
_tmp1FD;_tmp1FD.f1=_tmp1F7;_tmp1FD.f2=(void*)((void*)0);_tmp1FD;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1FB,_tmp1FC,0);}}}_LLD5: if(*((int*)_tmpBA)!= 8)goto _LLD7;
_tmpEF=((struct Cyc_Absyn_Or_e_struct*)_tmpBA)->f1;_tmpF0=((struct Cyc_Absyn_Or_e_struct*)
_tmpBA)->f2;_LLD6: {void*_tmp200;void*_tmp201;struct _tuple0 _tmp1FF=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpEF);_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;_tmp200=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEF->loc,_tmp200);_tmp201=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpEF->loc,
_tmp201);{void*_tmp203;void*_tmp204;struct _tuple0 _tmp202=Cyc_NewControlFlow_anal_Rexp(
env,_tmp201,_tmpF0);_tmp203=_tmp202.f1;_tmp204=_tmp202.f2;_tmp203=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF0->loc,_tmp203);{struct _tuple0 _tmp205=({struct _tuple0 _tmp208;_tmp208.f1=
_tmp203;_tmp208.f2=_tmp204;_tmp208;});struct _tuple0 _tmp206=({struct _tuple0
_tmp207;_tmp207.f1=_tmp200;_tmp207.f2=(void*)((void*)1);_tmp207;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp205,_tmp206,0);}}}_LLD7: if(*((int*)_tmpBA)!= 25)goto _LLD9;
_tmpF1=((struct Cyc_Absyn_Subscript_e_struct*)_tmpBA)->f1;_tmpF2=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpBA)->f2;_LLD8: {void*_tmp20B;struct Cyc_List_List*_tmp20C;struct _tuple7
_tmp20A=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct Cyc_Absyn_Exp*
_tmp209[2];_tmp209[1]=_tmpF2;_tmp209[0]=_tmpF1;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp209,sizeof(struct Cyc_Absyn_Exp*),
2));}),1);_tmp20B=_tmp20A.f1;_tmp20C=_tmp20A.f2;_tmp20B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF2->loc,_tmp20B);{void*_tmp20D=_tmp20B;{void*_tmp20E=_tmp20B;struct Cyc_Dict_Dict*
_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_CfFlowInfo_ConsumeInfo _tmp211;
_LL158: if(_tmp20E <= (void*)1)goto _LL15A;if(*((int*)_tmp20E)!= 0)goto _LL15A;
_tmp20F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20E)->f1;_tmp210=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20E)->f2;_tmp211=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp20E)->f3;_LL159: if(Cyc_CfFlowInfo_initlevel(_tmp20F,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp20C))->tl))->hd)== (void*)0)({
void*_tmp212[0]={};Cyc_Tcutil_terr(_tmpF2->loc,({const char*_tmp213="expression may not be initialized";
_tag_dynforward(_tmp213,sizeof(char),_get_zero_arr_size(_tmp213,34));}),
_tag_dynforward(_tmp212,sizeof(void*),0));});{struct Cyc_List_List*_tmp214=Cyc_NewControlFlow_add_subscript_reln(
_tmp210,_tmpF1,_tmpF2);if(_tmp210 != _tmp214)_tmp20D=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp216;_tmp216.tag=0;_tmp216.f1=_tmp20F;_tmp216.f2=_tmp214;_tmp216.f3=_tmp211;
_tmp216;});_tmp215;});goto _LL157;}_LL15A:;_LL15B: goto _LL157;_LL157:;}{void*
_tmp217=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF1->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp218;struct Cyc_Absyn_PtrAtts _tmp219;struct Cyc_Absyn_Conref*
_tmp21A;_LL15D: if(_tmp217 <= (void*)4)goto _LL161;if(*((int*)_tmp217)!= 9)goto
_LL15F;_LL15E: {void*_tmp21B=(void*)((struct Cyc_List_List*)_check_null(_tmp20C))->hd;
struct Cyc_Dict_Dict*_tmp21C;_LL164: if(_tmp21B <= (void*)3)goto _LL166;if(*((int*)
_tmp21B)!= 4)goto _LL166;_tmp21C=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp21B)->f1;
_LL165: return({struct _tuple0 _tmp21D;_tmp21D.f1=_tmp20D;_tmp21D.f2=((void*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp21C,Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmpF2)).f1));_tmp21D;});_LL166:;_LL167:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp21E=_cycalloc(sizeof(*
_tmp21E));_tmp21E[0]=({struct Cyc_Core_Impossible_struct _tmp21F;_tmp21F.tag=Cyc_Core_Impossible;
_tmp21F.f1=({const char*_tmp220="anal_Rexp: Subscript";_tag_dynforward(_tmp220,
sizeof(char),_get_zero_arr_size(_tmp220,21));});_tmp21F;});_tmp21E;}));_LL163:;}
_LL15F: if(*((int*)_tmp217)!= 4)goto _LL161;_tmp218=((struct Cyc_Absyn_PointerType_struct*)
_tmp217)->f1;_tmp219=_tmp218.ptr_atts;_tmp21A=_tmp219.bounds;_LL160:{void*
_tmp221=(void*)(Cyc_Absyn_compress_conref(_tmp21A))->v;void*_tmp222;_LL169: if(
_tmp221 <= (void*)1)goto _LL16B;if(*((int*)_tmp221)!= 0)goto _LL16B;_tmp222=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp221)->f1;if(_tmp222 <= (void*)2)goto _LL16B;
if(*((int*)_tmp222)!= 1)goto _LL16B;_LL16A:{void*_tmp223=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp20C))->tl))->hd;struct Cyc_List_List*
_tmp224;_LL16E: if(_tmp223 <= (void*)3)goto _LL170;if(*((int*)_tmp223)!= 3)goto
_LL170;_tmp224=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp223)->f1;_LL16F:(void*)(
_tmpF2->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp225=
_cycalloc(sizeof(*_tmp225));_tmp225[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp226;_tmp226.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp226.f1=_tmp224;_tmp226;});
_tmp225;})));goto _LL16D;_LL170:;_LL171: goto _LL16D;_LL16D:;}goto _LL168;_LL16B:;
_LL16C: goto _LL168;_LL168:;}{void*_tmp228;void*_tmp229;struct _tuple0 _tmp227=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp20B,_tmpF1,(void*)((struct Cyc_List_List*)_check_null(_tmp20C))->hd);
_tmp228=_tmp227.f1;_tmp229=_tmp227.f2;return _tmp228 == (void*)0?({struct _tuple0
_tmp22A;_tmp22A.f1=_tmp228;_tmp22A.f2=_tmp229;_tmp22A;}):({struct _tuple0 _tmp22B;
_tmp22B.f1=_tmp20D;_tmp22B.f2=_tmp229;_tmp22B;});}_LL161:;_LL162:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp22C=_cycalloc(sizeof(*_tmp22C));
_tmp22C[0]=({struct Cyc_Core_Impossible_struct _tmp22D;_tmp22D.tag=Cyc_Core_Impossible;
_tmp22D.f1=({const char*_tmp22E="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,33));});_tmp22D;});
_tmp22C;}));_LL15C:;}}}_LLD9: if(*((int*)_tmpBA)!= 32)goto _LLDB;_tmpF3=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpBA)->f1;_LLDA: _tmpF4=_tmpF3;goto _LLDC;_LLDB: if(*((
int*)_tmpBA)!= 26)goto _LLDD;_tmpF4=((struct Cyc_Absyn_Tuple_e_struct*)_tmpBA)->f1;
_LLDC: {void*_tmp230;struct Cyc_List_List*_tmp231;struct _tuple7 _tmp22F=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpF4,0);_tmp230=_tmp22F.f1;_tmp231=_tmp22F.f2;_tmp230=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp230);{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{
int i=0;for(0;_tmp231 != 0;(_tmp231=_tmp231->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(aggrdict,
Cyc_Absyn_fieldname(i),(void*)_tmp231->hd);}}return({struct _tuple0 _tmp232;
_tmp232.f1=_tmp230;_tmp232.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp234;_tmp234.tag=4;_tmp234.f1=aggrdict;_tmp234;});_tmp233;});_tmp232;});}}
_LLDD: if(*((int*)_tmpBA)!= 31)goto _LLDF;_tmpF5=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBA)->f2;_LLDE: _tmpF6=_tmpF5;goto _LLE0;_LLDF: if(*((int*)_tmpBA)!= 30)goto
_LLE1;_tmpF6=((struct Cyc_Absyn_Struct_e_struct*)_tmpBA)->f3;_LLE0: {void*_tmp236;
struct Cyc_List_List*_tmp237;struct _tuple7 _tmp235=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))
Cyc_Core_snd,_tmpF6),0);_tmp236=_tmp235.f1;_tmp237=_tmp235.f2;_tmp236=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp236);{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{
int i=0;for(0;_tmp237 != 0;(((_tmp237=_tmp237->tl,_tmpF6=_tmpF6->tl)),++ i)){struct
Cyc_List_List*ds=(*((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmpF6))->hd)).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp238=(void*)ds->hd;struct _dynforward_ptr*_tmp239;
_LL173: if(*((int*)_tmp238)!= 0)goto _LL175;_LL174:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A[0]=({struct Cyc_Core_Impossible_struct
_tmp23B;_tmp23B.tag=Cyc_Core_Impossible;_tmp23B.f1=({const char*_tmp23C="anal_Rexp:Struct_e";
_tag_dynforward(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,19));});_tmp23B;});
_tmp23A;}));_LL175: if(*((int*)_tmp238)!= 1)goto _LL172;_tmp239=((struct Cyc_Absyn_FieldName_struct*)
_tmp238)->f1;_LL176: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp239,(void*)_tmp237->hd);
_LL172:;}}}return({struct _tuple0 _tmp23D;_tmp23D.f1=_tmp236;_tmp23D.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));
_tmp23E[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp23F;_tmp23F.tag=4;_tmp23F.f1=
aggrdict;_tmp23F;});_tmp23E;});_tmp23D;});}}_LLE1: if(*((int*)_tmpBA)!= 28)goto
_LLE3;_tmpF7=((struct Cyc_Absyn_Array_e_struct*)_tmpBA)->f1;_LLE2: {struct Cyc_List_List*
_tmp240=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple8*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple8*))
Cyc_Core_snd,_tmpF7);void*_tmp242;struct Cyc_List_List*_tmp243;struct _tuple7
_tmp241=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp240,0);_tmp242=
_tmp241.f1;_tmp243=_tmp241.f2;_tmp242=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp242);for(0;_tmp243 != 0;(_tmp243=_tmp243->tl,_tmp240=_tmp240->tl)){_tmp242=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp240))->hd)->loc,_tmp242,(void*)_tmp243->hd);}return({struct
_tuple0 _tmp244;_tmp244.f1=_tmp242;_tmp244.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp244;});}
_LLE3: if(*((int*)_tmpBA)!= 29)goto _LLE5;_tmpF8=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBA)->f1;_tmpF9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpBA)->f2;_tmpFA=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpBA)->f3;_tmpFB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBA)->f4;_LLE4: {void*_tmp246;void*_tmp247;struct _tuple0 _tmp245=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpF9);_tmp246=_tmp245.f1;_tmp247=_tmp245.f2;_tmp246=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF9->loc,_tmp246);{void*_tmp248=_tmp246;struct Cyc_Dict_Dict*_tmp249;struct Cyc_List_List*
_tmp24A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp24B;_LL178: if((int)_tmp248 != 0)goto
_LL17A;_LL179: return({struct _tuple0 _tmp24C;_tmp24C.f1=_tmp246;_tmp24C.f2=Cyc_CfFlowInfo_unknown_all;
_tmp24C;});_LL17A: if(_tmp248 <= (void*)1)goto _LL177;if(*((int*)_tmp248)!= 0)goto
_LL177;_tmp249=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp248)->f1;_tmp24A=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp248)->f2;_tmp24B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp248)->f3;_LL17B: if(Cyc_CfFlowInfo_initlevel(_tmp249,_tmp247)== (void*)0)({
void*_tmp24D[0]={};Cyc_Tcutil_terr(_tmpF9->loc,({const char*_tmp24E="expression may not be initialized";
_tag_dynforward(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,34));}),
_tag_dynforward(_tmp24D,sizeof(void*),0));});{struct Cyc_List_List*new_relns=
_tmp24A;comp_loop: {void*_tmp24F=(void*)_tmpF9->r;struct Cyc_Absyn_Exp*_tmp250;
void*_tmp251;struct Cyc_Absyn_Vardecl*_tmp252;void*_tmp253;struct Cyc_Absyn_Vardecl*
_tmp254;void*_tmp255;struct Cyc_Absyn_Vardecl*_tmp256;void*_tmp257;struct Cyc_Absyn_Vardecl*
_tmp258;void*_tmp259;int _tmp25A;void*_tmp25B;struct Cyc_List_List*_tmp25C;struct
Cyc_List_List _tmp25D;struct Cyc_Absyn_Exp*_tmp25E;_LL17D: if(*((int*)_tmp24F)!= 15)
goto _LL17F;_tmp250=((struct Cyc_Absyn_Cast_e_struct*)_tmp24F)->f2;_LL17E: _tmpF9=
_tmp250;goto comp_loop;_LL17F: if(*((int*)_tmp24F)!= 1)goto _LL181;_tmp251=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp24F)->f2;if(_tmp251 <= (void*)1)goto _LL181;if(*((
int*)_tmp251)!= 0)goto _LL181;_tmp252=((struct Cyc_Absyn_Global_b_struct*)_tmp251)->f1;
if(!(!_tmp252->escapes))goto _LL181;_LL180: _tmp254=_tmp252;goto _LL182;_LL181: if(*((
int*)_tmp24F)!= 1)goto _LL183;_tmp253=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24F)->f2;if(_tmp253 <= (void*)1)goto _LL183;if(*((int*)_tmp253)!= 3)goto _LL183;
_tmp254=((struct Cyc_Absyn_Local_b_struct*)_tmp253)->f1;if(!(!_tmp254->escapes))
goto _LL183;_LL182: _tmp256=_tmp254;goto _LL184;_LL183: if(*((int*)_tmp24F)!= 1)goto
_LL185;_tmp255=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24F)->f2;if(_tmp255 <= (
void*)1)goto _LL185;if(*((int*)_tmp255)!= 4)goto _LL185;_tmp256=((struct Cyc_Absyn_Pat_b_struct*)
_tmp255)->f1;if(!(!_tmp256->escapes))goto _LL185;_LL184: _tmp258=_tmp256;goto
_LL186;_LL185: if(*((int*)_tmp24F)!= 1)goto _LL187;_tmp257=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24F)->f2;if(_tmp257 <= (void*)1)goto _LL187;if(*((int*)_tmp257)!= 2)goto _LL187;
_tmp258=((struct Cyc_Absyn_Param_b_struct*)_tmp257)->f1;if(!(!_tmp258->escapes))
goto _LL187;_LL186: new_relns=({struct Cyc_List_List*_tmp25F=_cycalloc(sizeof(*
_tmp25F));_tmp25F->hd=({struct Cyc_CfFlowInfo_Reln*_tmp260=_cycalloc(sizeof(*
_tmp260));_tmp260->vd=_tmpF8;_tmp260->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp262;_tmp262.tag=1;_tmp262.f1=_tmp258;_tmp262;});_tmp261;}));_tmp260;});
_tmp25F->tl=_tmp24A;_tmp25F;});goto _LL17C;_LL187: if(*((int*)_tmp24F)!= 0)goto
_LL189;_tmp259=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp24F)->f1;if(_tmp259
<= (void*)1)goto _LL189;if(*((int*)_tmp259)!= 2)goto _LL189;_tmp25A=((struct Cyc_Absyn_Int_c_struct*)
_tmp259)->f2;_LL188: new_relns=({struct Cyc_List_List*_tmp263=_cycalloc(sizeof(*
_tmp263));_tmp263->hd=({struct Cyc_CfFlowInfo_Reln*_tmp264=_cycalloc(sizeof(*
_tmp264));_tmp264->vd=_tmpF8;_tmp264->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp265=_cycalloc_atomic(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp266;_tmp266.tag=3;_tmp266.f1=(unsigned int)_tmp25A;_tmp266;});_tmp265;}));
_tmp264;});_tmp263->tl=_tmp24A;_tmp263;});goto _LL17C;_LL189: if(*((int*)_tmp24F)
!= 3)goto _LL18B;_tmp25B=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp24F)->f1;
if((int)_tmp25B != 19)goto _LL18B;_tmp25C=((struct Cyc_Absyn_Primop_e_struct*)
_tmp24F)->f2;if(_tmp25C == 0)goto _LL18B;_tmp25D=*_tmp25C;_tmp25E=(struct Cyc_Absyn_Exp*)
_tmp25D.hd;_LL18A:{void*_tmp267=(void*)_tmp25E->r;void*_tmp268;struct Cyc_Absyn_Vardecl*
_tmp269;void*_tmp26A;struct Cyc_Absyn_Vardecl*_tmp26B;void*_tmp26C;struct Cyc_Absyn_Vardecl*
_tmp26D;void*_tmp26E;struct Cyc_Absyn_Vardecl*_tmp26F;_LL18E: if(*((int*)_tmp267)
!= 1)goto _LL190;_tmp268=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp267)->f2;if(
_tmp268 <= (void*)1)goto _LL190;if(*((int*)_tmp268)!= 0)goto _LL190;_tmp269=((
struct Cyc_Absyn_Global_b_struct*)_tmp268)->f1;if(!(!_tmp269->escapes))goto _LL190;
_LL18F: _tmp26B=_tmp269;goto _LL191;_LL190: if(*((int*)_tmp267)!= 1)goto _LL192;
_tmp26A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp267)->f2;if(_tmp26A <= (void*)
1)goto _LL192;if(*((int*)_tmp26A)!= 3)goto _LL192;_tmp26B=((struct Cyc_Absyn_Local_b_struct*)
_tmp26A)->f1;if(!(!_tmp26B->escapes))goto _LL192;_LL191: _tmp26D=_tmp26B;goto
_LL193;_LL192: if(*((int*)_tmp267)!= 1)goto _LL194;_tmp26C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp267)->f2;if(_tmp26C <= (void*)1)goto _LL194;if(*((int*)_tmp26C)!= 4)goto _LL194;
_tmp26D=((struct Cyc_Absyn_Pat_b_struct*)_tmp26C)->f1;if(!(!_tmp26D->escapes))
goto _LL194;_LL193: _tmp26F=_tmp26D;goto _LL195;_LL194: if(*((int*)_tmp267)!= 1)goto
_LL196;_tmp26E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp267)->f2;if(_tmp26E <= (
void*)1)goto _LL196;if(*((int*)_tmp26E)!= 2)goto _LL196;_tmp26F=((struct Cyc_Absyn_Param_b_struct*)
_tmp26E)->f1;if(!(!_tmp26F->escapes))goto _LL196;_LL195: new_relns=({struct Cyc_List_List*
_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271->vd=_tmpF8;_tmp271->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp272=_cycalloc(sizeof(*_tmp272));
_tmp272[0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp273;_tmp273.tag=2;_tmp273.f1=
_tmp26F;_tmp273;});_tmp272;}));_tmp271;});_tmp270->tl=_tmp24A;_tmp270;});goto
_LL18D;_LL196:;_LL197: goto _LL18D;_LL18D:;}goto _LL17C;_LL18B:;_LL18C: goto _LL17C;
_LL17C:;}if(_tmp24A != new_relns)_tmp246=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp275;_tmp275.tag=0;_tmp275.f1=_tmp249;_tmp275.f2=new_relns;_tmp275.f3=_tmp24B;
_tmp275;});_tmp274;});{void*_tmp276=_tmp247;_LL199: if((int)_tmp276 != 0)goto
_LL19B;_LL19A: return({struct _tuple0 _tmp277;_tmp277.f1=_tmp246;_tmp277.f2=Cyc_CfFlowInfo_unknown_all;
_tmp277;});_LL19B: if((int)_tmp276 != 2)goto _LL19D;_LL19C: goto _LL19E;_LL19D: if((
int)_tmp276 != 1)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(_tmp276 <= (void*)3)goto
_LL1A1;if(*((int*)_tmp276)!= 2)goto _LL1A1;_LL1A0: {struct Cyc_List_List _tmp278=({
struct Cyc_List_List _tmp283;_tmp283.hd=_tmpF8;_tmp283.tl=0;_tmp283;});_tmp246=Cyc_NewControlFlow_add_vars(
_tmp246,(struct Cyc_List_List*)& _tmp278,Cyc_CfFlowInfo_unknown_all,e->loc);{void*
_tmp27A;void*_tmp27B;struct _tuple0 _tmp279=Cyc_NewControlFlow_anal_Rexp(env,
_tmp246,_tmpFA);_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;_tmp27A=Cyc_CfFlowInfo_consume_unique_rvals(
_tmpFA->loc,_tmp27A);{void*_tmp27C=_tmp27A;struct Cyc_Dict_Dict*_tmp27D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp27E;_LL1A4: if((int)_tmp27C != 0)goto _LL1A6;_LL1A5: return({struct _tuple0
_tmp27F;_tmp27F.f1=_tmp27A;_tmp27F.f2=Cyc_CfFlowInfo_unknown_all;_tmp27F;});
_LL1A6: if(_tmp27C <= (void*)1)goto _LL1A3;if(*((int*)_tmp27C)!= 0)goto _LL1A3;
_tmp27D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp27C)->f1;_tmp27E=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp27C)->f3;_LL1A7: if(Cyc_CfFlowInfo_initlevel(
_tmp27D,_tmp27B)!= (void*)2){({void*_tmp280[0]={};Cyc_Tcutil_terr(_tmpF9->loc,({
const char*_tmp281="expression may not be initialized";_tag_dynforward(_tmp281,
sizeof(char),_get_zero_arr_size(_tmp281,34));}),_tag_dynforward(_tmp280,sizeof(
void*),0));});return({struct _tuple0 _tmp282;_tmp282.f1=(void*)0;_tmp282.f2=Cyc_CfFlowInfo_unknown_all;
_tmp282;});}_LL1A3:;}_tmp246=_tmp27A;goto _LL1A2;}}_LL1A1:;_LL1A2: while(1){struct
Cyc_List_List _tmp284=({struct Cyc_List_List _tmp28F;_tmp28F.hd=_tmpF8;_tmp28F.tl=0;
_tmp28F;});_tmp246=Cyc_NewControlFlow_add_vars(_tmp246,(struct Cyc_List_List*)&
_tmp284,Cyc_CfFlowInfo_unknown_all,e->loc);{void*_tmp286;void*_tmp287;struct
_tuple0 _tmp285=Cyc_NewControlFlow_anal_Rexp(env,_tmp246,_tmpFA);_tmp286=_tmp285.f1;
_tmp287=_tmp285.f2;_tmp286=Cyc_CfFlowInfo_consume_unique_rvals(_tmpFA->loc,
_tmp286);{void*_tmp288=_tmp286;struct Cyc_Dict_Dict*_tmp289;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp28A;_LL1A9: if((int)_tmp288 != 0)goto _LL1AB;_LL1AA: goto _LL1A8;_LL1AB: if(
_tmp288 <= (void*)1)goto _LL1A8;if(*((int*)_tmp288)!= 0)goto _LL1A8;_tmp289=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp288)->f1;_tmp28A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp288)->f3;_LL1AC: if(Cyc_CfFlowInfo_initlevel(_tmp289,_tmp287)!= (void*)2){({
void*_tmp28B[0]={};Cyc_Tcutil_terr(_tmpF9->loc,({const char*_tmp28C="expression may not be initialized";
_tag_dynforward(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,34));}),
_tag_dynforward(_tmp28B,sizeof(void*),0));});return({struct _tuple0 _tmp28D;
_tmp28D.f1=(void*)0;_tmp28D.f2=Cyc_CfFlowInfo_unknown_all;_tmp28D;});}_LL1A8:;}{
void*_tmp28E=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp246,_tmp286,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp28E,_tmp246))break;_tmp246=_tmp28E;}}}return({struct _tuple0 _tmp290;_tmp290.f1=
_tmp246;_tmp290.f2=Cyc_CfFlowInfo_unknown_all;_tmp290;});_LL198:;}}_LL177:;}}
_LLE5: if(*((int*)_tmpBA)!= 37)goto _LLE7;_tmpFC=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpBA)->f1;_LLE6: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmpFC))->flow;{void*_tmp291=(void*)_tmpFC->r;struct Cyc_Absyn_Stmt*
_tmp292;struct Cyc_Absyn_Stmt*_tmp293;struct Cyc_Absyn_Decl*_tmp294;struct Cyc_Absyn_Stmt*
_tmp295;struct Cyc_Absyn_Exp*_tmp296;_LL1AE: if(_tmp291 <= (void*)1)goto _LL1B4;if(*((
int*)_tmp291)!= 1)goto _LL1B0;_tmp292=((struct Cyc_Absyn_Seq_s_struct*)_tmp291)->f1;
_tmp293=((struct Cyc_Absyn_Seq_s_struct*)_tmp291)->f2;_LL1AF: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp292);_tmpFC=_tmp293;goto _LL1AD;_LL1B0: if(*((int*)_tmp291)!= 11)
goto _LL1B2;_tmp294=((struct Cyc_Absyn_Decl_s_struct*)_tmp291)->f1;_tmp295=((
struct Cyc_Absyn_Decl_s_struct*)_tmp291)->f2;_LL1B1: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp294);_tmpFC=_tmp295;goto _LL1AD;_LL1B2: if(*((int*)_tmp291)!= 0)
goto _LL1B4;_tmp296=((struct Cyc_Absyn_Exp_s_struct*)_tmp291)->f1;_LL1B3: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp296);_LL1B4:;_LL1B5:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297[0]=({struct Cyc_Core_Impossible_struct
_tmp298;_tmp298.tag=Cyc_Core_Impossible;_tmp298.f1=({const char*_tmp299="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size(_tmp299,33));});_tmp298;});
_tmp297;}));_LL1AD:;}}_LLE7: if(*((int*)_tmpBA)!= 1)goto _LLE9;_tmpFD=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if((int)_tmpFD != 0)goto _LLE9;_LLE8:
goto _LLEA;_LLE9: if(*((int*)_tmpBA)!= 2)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((int*)
_tmpBA)!= 10)goto _LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)_tmpBA)!= 36)goto _LLEF;
_LLEE: goto _LLF0;_LLEF: if(*((int*)_tmpBA)!= 27)goto _LL8E;_LLF0:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A[0]=({
struct Cyc_Core_Impossible_struct _tmp29B;_tmp29B.tag=Cyc_Core_Impossible;_tmp29B.f1=({
const char*_tmp29C="anal_Rexp, unexpected exp form";_tag_dynforward(_tmp29C,
sizeof(char),_get_zero_arr_size(_tmp29C,31));});_tmp29B;});_tmp29A;}));_LL8E:;}}
static struct _tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){
void*_tmp29D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp29E;void*_tmp29F;struct Cyc_Absyn_PtrAtts _tmp2A0;
struct Cyc_Absyn_Conref*_tmp2A1;struct Cyc_Absyn_Conref*_tmp2A2;_LL1B7: if(_tmp29D
<= (void*)4)goto _LL1B9;if(*((int*)_tmp29D)!= 4)goto _LL1B9;_tmp29E=((struct Cyc_Absyn_PointerType_struct*)
_tmp29D)->f1;_tmp29F=(void*)_tmp29E.elt_typ;_tmp2A0=_tmp29E.ptr_atts;_tmp2A1=
_tmp2A0.bounds;_tmp2A2=_tmp2A0.zero_term;_LL1B8: {void*_tmp2A3=f;struct Cyc_Dict_Dict*
_tmp2A4;struct Cyc_List_List*_tmp2A5;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2A6;
_LL1BC: if((int)_tmp2A3 != 0)goto _LL1BE;_LL1BD: return({struct _tuple0 _tmp2A7;
_tmp2A7.f1=f;_tmp2A7.f2=(void*)0;_tmp2A7;});_LL1BE: if(_tmp2A3 <= (void*)1)goto
_LL1BB;if(*((int*)_tmp2A3)!= 0)goto _LL1BB;_tmp2A4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2A3)->f1;_tmp2A5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A3)->f2;
_tmp2A6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A3)->f3;_LL1BF: if(Cyc_Tcutil_is_bound_one(
_tmp2A1)){void*_tmp2A8=r;struct Cyc_CfFlowInfo_Place*_tmp2A9;struct Cyc_CfFlowInfo_Place
_tmp2AA;void*_tmp2AB;struct Cyc_List_List*_tmp2AC;void*_tmp2AD;_LL1C1: if((int)
_tmp2A8 != 1)goto _LL1C3;_LL1C2: goto _LL1C4;_LL1C3: if((int)_tmp2A8 != 2)goto _LL1C5;
_LL1C4:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp2AF;_tmp2AF.tag=Cyc_CfFlowInfo_NotZero;_tmp2AF.f1=_tmp2A5;_tmp2AF;});_tmp2AE;})));
goto _LL1C0;_LL1C5: if(_tmp2A8 <= (void*)3)goto _LL1C7;if(*((int*)_tmp2A8)!= 2)goto
_LL1C7;_tmp2A9=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2A8)->f1;_tmp2AA=*
_tmp2A9;_tmp2AB=(void*)_tmp2AA.root;_tmp2AC=_tmp2AA.fields;_LL1C6:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp2B0=_cycalloc(sizeof(*
_tmp2B0));_tmp2B0[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp2B1;_tmp2B1.tag=
Cyc_CfFlowInfo_NotZero;_tmp2B1.f1=_tmp2A5;_tmp2B1;});_tmp2B0;})));return({struct
_tuple0 _tmp2B2;_tmp2B2.f1=f;_tmp2B2.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp2B4;_tmp2B4.tag=0;_tmp2B4.f1=({struct Cyc_CfFlowInfo_Place*_tmp2B5=_cycalloc(
sizeof(*_tmp2B5));_tmp2B5->root=(void*)_tmp2AB;_tmp2B5->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2AC,flds);
_tmp2B5;});_tmp2B4;});_tmp2B3;});_tmp2B2;});_LL1C7: if((int)_tmp2A8 != 0)goto
_LL1C9;_LL1C8:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple0 _tmp2B6;_tmp2B6.f1=(void*)0;_tmp2B6.f2=(void*)0;_tmp2B6;});_LL1C9:
if(_tmp2A8 <= (void*)3)goto _LL1CB;if(*((int*)_tmp2A8)!= 0)goto _LL1CB;_tmp2AD=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2A8)->f1;_LL1CA: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp2AD);goto _LL1CC;_LL1CB:;_LL1CC:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2B8;_tmp2B8.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2B8.f1=_tmp2A5;_tmp2B8;});_tmp2B7;})));_LL1C0:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));
_tmp2B9[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2BA;_tmp2BA.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2BA.f1=_tmp2A5;_tmp2BA;});_tmp2B9;})));}if(Cyc_CfFlowInfo_initlevel(_tmp2A4,
r)== (void*)0)({void*_tmp2BB[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp2BC="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp2BC,sizeof(char),_get_zero_arr_size(_tmp2BC,46));}),
_tag_dynforward(_tmp2BB,sizeof(void*),0));});return({struct _tuple0 _tmp2BD;
_tmp2BD.f1=f;_tmp2BD.f2=(void*)0;_tmp2BD;});_LL1BB:;}_LL1B9:;_LL1BA:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));
_tmp2BE[0]=({struct Cyc_Core_Impossible_struct _tmp2BF;_tmp2BF.tag=Cyc_Core_Impossible;
_tmp2BF.f1=({const char*_tmp2C0="left deref of non-pointer-type";_tag_dynforward(
_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,31));});_tmp2BF;});_tmp2BE;}));
_LL1B6:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict*
d;{void*_tmp2C1=inflow;struct Cyc_Dict_Dict*_tmp2C2;struct Cyc_List_List*_tmp2C3;
_LL1CE: if((int)_tmp2C1 != 0)goto _LL1D0;_LL1CF: return({struct _tuple0 _tmp2C4;
_tmp2C4.f1=(void*)0;_tmp2C4.f2=(void*)0;_tmp2C4;});_LL1D0: if(_tmp2C1 <= (void*)1)
goto _LL1CD;if(*((int*)_tmp2C1)!= 0)goto _LL1CD;_tmp2C2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2C1)->f1;_tmp2C3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2C1)->f2;
_LL1D1: d=_tmp2C2;_LL1CD:;}{void*_tmp2C5=(void*)e->r;struct Cyc_Absyn_Exp*_tmp2C6;
struct Cyc_Absyn_Exp*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C8;void*_tmp2C9;struct Cyc_Absyn_Vardecl*
_tmp2CA;void*_tmp2CB;struct Cyc_Absyn_Vardecl*_tmp2CC;void*_tmp2CD;struct Cyc_Absyn_Vardecl*
_tmp2CE;void*_tmp2CF;struct Cyc_Absyn_Vardecl*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;
struct _dynforward_ptr*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;struct Cyc_Absyn_Exp*
_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D6;struct
_dynforward_ptr*_tmp2D7;_LL1D3: if(*((int*)_tmp2C5)!= 15)goto _LL1D5;_tmp2C6=((
struct Cyc_Absyn_Cast_e_struct*)_tmp2C5)->f2;_LL1D4: _tmp2C7=_tmp2C6;goto _LL1D6;
_LL1D5: if(*((int*)_tmp2C5)!= 13)goto _LL1D7;_tmp2C7=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp2C5)->f1;_LL1D6: _tmp2C8=_tmp2C7;goto _LL1D8;_LL1D7: if(*((int*)_tmp2C5)!= 14)
goto _LL1D9;_tmp2C8=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2C5)->f1;_LL1D8:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp2C8,flds);_LL1D9: if(*((int*)
_tmp2C5)!= 1)goto _LL1DB;_tmp2C9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C5)->f2;
if(_tmp2C9 <= (void*)1)goto _LL1DB;if(*((int*)_tmp2C9)!= 2)goto _LL1DB;_tmp2CA=((
struct Cyc_Absyn_Param_b_struct*)_tmp2C9)->f1;_LL1DA: _tmp2CC=_tmp2CA;goto _LL1DC;
_LL1DB: if(*((int*)_tmp2C5)!= 1)goto _LL1DD;_tmp2CB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C5)->f2;if(_tmp2CB <= (void*)1)goto _LL1DD;if(*((int*)_tmp2CB)!= 3)goto _LL1DD;
_tmp2CC=((struct Cyc_Absyn_Local_b_struct*)_tmp2CB)->f1;_LL1DC: _tmp2CE=_tmp2CC;
goto _LL1DE;_LL1DD: if(*((int*)_tmp2C5)!= 1)goto _LL1DF;_tmp2CD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C5)->f2;if(_tmp2CD <= (void*)1)goto _LL1DF;if(*((int*)_tmp2CD)!= 4)goto _LL1DF;
_tmp2CE=((struct Cyc_Absyn_Pat_b_struct*)_tmp2CD)->f1;_LL1DE: return({struct
_tuple0 _tmp2D8;_tmp2D8.f1=inflow;_tmp2D8.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp2D9=_cycalloc(sizeof(*_tmp2D9));_tmp2D9[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp2DA;_tmp2DA.tag=0;_tmp2DA.f1=({struct Cyc_CfFlowInfo_Place*_tmp2DB=_cycalloc(
sizeof(*_tmp2DB));_tmp2DB->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp2DC=_cycalloc(sizeof(*_tmp2DC));_tmp2DC[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp2DD;_tmp2DD.tag=0;_tmp2DD.f1=_tmp2CE;_tmp2DD;});_tmp2DC;}));_tmp2DB->fields=
flds;_tmp2DB;});_tmp2DA;});_tmp2D9;});_tmp2D8;});_LL1DF: if(*((int*)_tmp2C5)!= 1)
goto _LL1E1;_tmp2CF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C5)->f2;if(
_tmp2CF <= (void*)1)goto _LL1E1;if(*((int*)_tmp2CF)!= 0)goto _LL1E1;_tmp2D0=((
struct Cyc_Absyn_Global_b_struct*)_tmp2CF)->f1;_LL1E0: return({struct _tuple0
_tmp2DE;_tmp2DE.f1=inflow;_tmp2DE.f2=(void*)0;_tmp2DE;});_LL1E1: if(*((int*)
_tmp2C5)!= 24)goto _LL1E3;_tmp2D1=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2C5)->f1;
_tmp2D2=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2C5)->f2;_LL1E2:{void*_tmp2DF=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D1->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp2E0;void*_tmp2E1;_LL1EC: if(_tmp2DF <= (void*)4)goto
_LL1EE;if(*((int*)_tmp2DF)!= 4)goto _LL1EE;_tmp2E0=((struct Cyc_Absyn_PointerType_struct*)
_tmp2DF)->f1;_tmp2E1=(void*)_tmp2E0.elt_typ;_LL1ED: if(!Cyc_Absyn_is_union_type(
_tmp2E1))flds=({struct Cyc_List_List*_tmp2E2=_cycalloc(sizeof(*_tmp2E2));_tmp2E2->hd=
_tmp2D2;_tmp2E2->tl=flds;_tmp2E2;});goto _LL1EB;_LL1EE:;_LL1EF:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3[0]=({
struct Cyc_Core_Impossible_struct _tmp2E4;_tmp2E4.tag=Cyc_Core_Impossible;_tmp2E4.f1=({
const char*_tmp2E5="anal_Rexp: AggrArrow ptr";_tag_dynforward(_tmp2E5,sizeof(char),
_get_zero_arr_size(_tmp2E5,25));});_tmp2E4;});_tmp2E3;}));_LL1EB:;}_tmp2D3=
_tmp2D1;goto _LL1E4;_LL1E3: if(*((int*)_tmp2C5)!= 22)goto _LL1E5;_tmp2D3=((struct
Cyc_Absyn_Deref_e_struct*)_tmp2C5)->f1;_LL1E4: {void*_tmp2E7;void*_tmp2E8;struct
_tuple0 _tmp2E6=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp2D3);_tmp2E7=_tmp2E6.f1;
_tmp2E8=_tmp2E6.f2;_tmp2E7=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp2E7);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp2E7,_tmp2D3,_tmp2E8,
flds);}_LL1E5: if(*((int*)_tmp2C5)!= 25)goto _LL1E7;_tmp2D4=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2C5)->f1;_tmp2D5=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2C5)->f2;_LL1E6: {
void*_tmp2E9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2D4->topt))->v);struct Cyc_Absyn_PtrInfo _tmp2EA;struct Cyc_Absyn_PtrAtts
_tmp2EB;struct Cyc_Absyn_Conref*_tmp2EC;_LL1F1: if(_tmp2E9 <= (void*)4)goto _LL1F5;
if(*((int*)_tmp2E9)!= 9)goto _LL1F3;_LL1F2: {struct _dynforward_ptr*_tmp2ED=Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmp2D5)).f1);return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp2D4,({struct Cyc_List_List*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));
_tmp2EE->hd=_tmp2ED;_tmp2EE->tl=flds;_tmp2EE;}));}_LL1F3: if(*((int*)_tmp2E9)!= 4)
goto _LL1F5;_tmp2EA=((struct Cyc_Absyn_PointerType_struct*)_tmp2E9)->f1;_tmp2EB=
_tmp2EA.ptr_atts;_tmp2EC=_tmp2EB.bounds;_LL1F4: {void*_tmp2F1;struct Cyc_List_List*
_tmp2F2;struct _tuple7 _tmp2F0=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({
struct Cyc_Absyn_Exp*_tmp2EF[2];_tmp2EF[1]=_tmp2D5;_tmp2EF[0]=_tmp2D4;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp2EF,sizeof(struct Cyc_Absyn_Exp*),
2));}),1);_tmp2F1=_tmp2F0.f1;_tmp2F2=_tmp2F0.f2;_tmp2F1=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp2D5->loc,_tmp2F1);{void*_tmp2F3=_tmp2F1;{void*_tmp2F4=_tmp2F1;struct Cyc_Dict_Dict*
_tmp2F5;struct Cyc_List_List*_tmp2F6;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2F7;
_LL1F8: if(_tmp2F4 <= (void*)1)goto _LL1FA;if(*((int*)_tmp2F4)!= 0)goto _LL1FA;
_tmp2F5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2F4)->f1;_tmp2F6=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2F4)->f2;_tmp2F7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2F4)->f3;_LL1F9: if(Cyc_CfFlowInfo_initlevel(_tmp2F5,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2F2))->tl))->hd)== (void*)0)({
void*_tmp2F8[0]={};Cyc_Tcutil_terr(_tmp2D5->loc,({const char*_tmp2F9="expression may not be initialized";
_tag_dynforward(_tmp2F9,sizeof(char),_get_zero_arr_size(_tmp2F9,34));}),
_tag_dynforward(_tmp2F8,sizeof(void*),0));});{struct Cyc_List_List*_tmp2FA=Cyc_NewControlFlow_add_subscript_reln(
_tmp2F6,_tmp2D4,_tmp2D5);if(_tmp2F6 != _tmp2FA)_tmp2F3=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp2FC;_tmp2FC.tag=0;_tmp2FC.f1=_tmp2F5;_tmp2FC.f2=_tmp2FA;_tmp2FC.f3=_tmp2F7;
_tmp2FC;});_tmp2FB;});goto _LL1F7;}_LL1FA:;_LL1FB: goto _LL1F7;_LL1F7:;}{void*
_tmp2FD=(void*)(Cyc_Absyn_compress_conref(_tmp2EC))->v;void*_tmp2FE;_LL1FD: if(
_tmp2FD <= (void*)1)goto _LL1FF;if(*((int*)_tmp2FD)!= 0)goto _LL1FF;_tmp2FE=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp2FD)->f1;if(_tmp2FE <= (void*)2)goto _LL1FF;
if(*((int*)_tmp2FE)!= 1)goto _LL1FF;_LL1FE:{void*_tmp2FF=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2F2))->tl))->hd;struct Cyc_List_List*
_tmp300;_LL202: if(_tmp2FF <= (void*)3)goto _LL204;if(*((int*)_tmp2FF)!= 3)goto
_LL204;_tmp300=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp2FF)->f1;_LL203:(void*)(
_tmp2D5->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp301=
_cycalloc(sizeof(*_tmp301));_tmp301[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp302;_tmp302.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp302.f1=_tmp300;_tmp302;});
_tmp301;})));goto _LL201;_LL204:;_LL205: goto _LL201;_LL201:;}goto _LL1FC;_LL1FF:;
_LL200: goto _LL1FC;_LL1FC:;}{void*_tmp304;void*_tmp305;struct _tuple0 _tmp303=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp2F1,_tmp2D4,(void*)((struct Cyc_List_List*)_check_null(_tmp2F2))->hd,
flds);_tmp304=_tmp303.f1;_tmp305=_tmp303.f2;return _tmp304 == (void*)0?({struct
_tuple0 _tmp306;_tmp306.f1=_tmp304;_tmp306.f2=_tmp305;_tmp306;}):({struct _tuple0
_tmp307;_tmp307.f1=_tmp2F3;_tmp307.f2=_tmp305;_tmp307;});}}}_LL1F5:;_LL1F6:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp308=_cycalloc(sizeof(*
_tmp308));_tmp308[0]=({struct Cyc_Core_Impossible_struct _tmp309;_tmp309.tag=Cyc_Core_Impossible;
_tmp309.f1=({const char*_tmp30A="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp30A,sizeof(char),_get_zero_arr_size(_tmp30A,33));});_tmp309;});
_tmp308;}));_LL1F0:;}_LL1E7: if(*((int*)_tmp2C5)!= 23)goto _LL1E9;_tmp2D6=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp2C5)->f1;_tmp2D7=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2C5)->f2;_LL1E8: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D6->topt))->v))return({struct _tuple0 _tmp30B;_tmp30B.f1=inflow;
_tmp30B.f2=(void*)0;_tmp30B;});return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,
_tmp2D6,({struct Cyc_List_List*_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C->hd=
_tmp2D7;_tmp30C->tl=flds;_tmp30C;}));_LL1E9:;_LL1EA: return({struct _tuple0 _tmp30D;
_tmp30D.f1=(void*)0;_tmp30D.f2=(void*)0;_tmp30D;});_LL1D2:;}}static struct _tuple0
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,
struct Cyc_Absyn_Exp*e){void*_tmp30F;void*_tmp310;struct _tuple0 _tmp30E=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp30F=_tmp30E.f1;_tmp310=_tmp30E.f2;return({struct _tuple0
_tmp311;_tmp311.f1=_tmp30F;_tmp311.f2=_tmp310;_tmp311;});}static struct _tuple0 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){{void*
_tmp312=(void*)e->r;struct Cyc_Absyn_Exp*_tmp313;struct Cyc_Absyn_Exp*_tmp314;
struct Cyc_Absyn_Exp*_tmp315;struct Cyc_Absyn_Exp*_tmp316;struct Cyc_Absyn_Exp*
_tmp317;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Exp*
_tmp31A;struct Cyc_Absyn_Exp*_tmp31B;void*_tmp31C;struct Cyc_List_List*_tmp31D;
struct Cyc_List_List _tmp31E;struct Cyc_Absyn_Exp*_tmp31F;struct Cyc_List_List*
_tmp320;void*_tmp321;struct Cyc_List_List*_tmp322;_LL207: if(*((int*)_tmp312)!= 6)
goto _LL209;_tmp313=((struct Cyc_Absyn_Conditional_e_struct*)_tmp312)->f1;_tmp314=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp312)->f2;_tmp315=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp312)->f3;_LL208: {void*_tmp324;void*_tmp325;struct _tuple0 _tmp323=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp313);_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;_tmp324=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp313->loc,_tmp324);_tmp325=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp313->loc,
_tmp325);{void*_tmp327;void*_tmp328;struct _tuple0 _tmp326=Cyc_NewControlFlow_anal_test(
env,_tmp324,_tmp314);_tmp327=_tmp326.f1;_tmp328=_tmp326.f2;{void*_tmp32A;void*
_tmp32B;struct _tuple0 _tmp329=Cyc_NewControlFlow_anal_test(env,_tmp325,_tmp315);
_tmp32A=_tmp329.f1;_tmp32B=_tmp329.f2;return({struct _tuple0 _tmp32C;_tmp32C.f1=
Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp327,_tmp32A,1);_tmp32C.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp328,_tmp32B,1);_tmp32C;});}}}_LL209: if(*((int*)_tmp312)!= 7)
goto _LL20B;_tmp316=((struct Cyc_Absyn_And_e_struct*)_tmp312)->f1;_tmp317=((struct
Cyc_Absyn_And_e_struct*)_tmp312)->f2;_LL20A: {void*_tmp32E;void*_tmp32F;struct
_tuple0 _tmp32D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp316);_tmp32E=_tmp32D.f1;
_tmp32F=_tmp32D.f2;_tmp32E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp316->loc,
_tmp32E);_tmp32F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp316->loc,_tmp32F);{
void*_tmp331;void*_tmp332;struct _tuple0 _tmp330=Cyc_NewControlFlow_anal_test(env,
_tmp32E,_tmp317);_tmp331=_tmp330.f1;_tmp332=_tmp330.f2;_tmp331=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp317->loc,_tmp331);_tmp332=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp317->loc,
_tmp332);return({struct _tuple0 _tmp333;_tmp333.f1=_tmp331;_tmp333.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp32F,_tmp332,0);_tmp333;});}}_LL20B: if(*((int*)_tmp312)!= 8)
goto _LL20D;_tmp318=((struct Cyc_Absyn_Or_e_struct*)_tmp312)->f1;_tmp319=((struct
Cyc_Absyn_Or_e_struct*)_tmp312)->f2;_LL20C: {void*_tmp335;void*_tmp336;struct
_tuple0 _tmp334=Cyc_NewControlFlow_anal_test(env,inflow,_tmp318);_tmp335=_tmp334.f1;
_tmp336=_tmp334.f2;_tmp335=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp318->loc,
_tmp335);_tmp336=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp318->loc,_tmp336);{
void*_tmp338;void*_tmp339;struct _tuple0 _tmp337=Cyc_NewControlFlow_anal_test(env,
_tmp336,_tmp319);_tmp338=_tmp337.f1;_tmp339=_tmp337.f2;_tmp338=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp319->loc,_tmp338);_tmp339=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp319->loc,
_tmp339);return({struct _tuple0 _tmp33A;_tmp33A.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp335,_tmp338,0);_tmp33A.f2=_tmp339;_tmp33A;});}}_LL20D: if(*((int*)_tmp312)!= 
9)goto _LL20F;_tmp31A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp312)->f1;_tmp31B=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp312)->f2;_LL20E: {void*_tmp33C;void*_tmp33D;
struct _tuple0 _tmp33B=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp31A);_tmp33C=
_tmp33B.f1;_tmp33D=_tmp33B.f2;_tmp33C=Cyc_CfFlowInfo_drop_unique_rvals(_tmp31A->loc,
_tmp33C);return Cyc_NewControlFlow_anal_test(env,_tmp33C,_tmp31B);}_LL20F: if(*((
int*)_tmp312)!= 3)goto _LL211;_tmp31C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp312)->f1;if((int)_tmp31C != 11)goto _LL211;_tmp31D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp312)->f2;if(_tmp31D == 0)goto _LL211;_tmp31E=*_tmp31D;_tmp31F=(struct Cyc_Absyn_Exp*)
_tmp31E.hd;_tmp320=_tmp31E.tl;if(_tmp320 != 0)goto _LL211;_LL210: {void*_tmp33F;
void*_tmp340;struct _tuple0 _tmp33E=Cyc_NewControlFlow_anal_test(env,inflow,
_tmp31F);_tmp33F=_tmp33E.f1;_tmp340=_tmp33E.f2;return({struct _tuple0 _tmp341;
_tmp341.f1=_tmp340;_tmp341.f2=_tmp33F;_tmp341;});}_LL211: if(*((int*)_tmp312)!= 3)
goto _LL213;_tmp321=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp312)->f1;
_tmp322=((struct Cyc_Absyn_Primop_e_struct*)_tmp312)->f2;_LL212: {void*_tmp343;
struct Cyc_List_List*_tmp344;struct _tuple7 _tmp342=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp322,0);_tmp343=_tmp342.f1;_tmp344=_tmp342.f2;{void*_tmp345=
_tmp321;_LL216: if((int)_tmp345 != 0)goto _LL218;_LL217: goto _LL219;_LL218: if((int)
_tmp345 != 2)goto _LL21A;_LL219: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp322))->hd)->loc,_tmp343);goto _LL215;_LL21A:;
_LL21B: _tmp343=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp322))->hd)->loc,_tmp343);goto _LL215;_LL215:;}{
void*_tmp346=_tmp343;struct Cyc_Dict_Dict*_tmp347;struct Cyc_List_List*_tmp348;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp349;_LL21D: if((int)_tmp346 != 0)goto _LL21F;
_LL21E: return({struct _tuple0 _tmp34A;_tmp34A.f1=_tmp343;_tmp34A.f2=_tmp343;
_tmp34A;});_LL21F: if(_tmp346 <= (void*)1)goto _LL21C;if(*((int*)_tmp346)!= 0)goto
_LL21C;_tmp347=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp346)->f1;_tmp348=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp346)->f2;_tmp349=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp346)->f3;_LL220: {struct Cyc_Absyn_Exp*_tmp34B=(struct Cyc_Absyn_Exp*)_tmp322->hd;
struct Cyc_Absyn_Exp*_tmp34C=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp322->tl))->hd;void*_tmp34D=(void*)((struct Cyc_List_List*)
_check_null(_tmp344))->hd;void*_tmp34E=(void*)((struct Cyc_List_List*)_check_null(
_tmp344->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp347,_tmp34D)== (void*)0)({void*
_tmp34F[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp322->hd)->loc,({const
char*_tmp350="expression may not be initialized";_tag_dynforward(_tmp350,sizeof(
char),_get_zero_arr_size(_tmp350,34));}),_tag_dynforward(_tmp34F,sizeof(void*),0));});
if(Cyc_CfFlowInfo_initlevel(_tmp347,_tmp34E)== (void*)0)({void*_tmp351[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp322->tl))->hd)->loc,({
const char*_tmp352="expression may not be initialized";_tag_dynforward(_tmp352,
sizeof(char),_get_zero_arr_size(_tmp352,34));}),_tag_dynforward(_tmp351,sizeof(
void*),0));});if(_tmp321 == (void*)5  || _tmp321 == (void*)6){struct _tuple0 _tmp354=({
struct _tuple0 _tmp353;_tmp353.f1=_tmp34D;_tmp353.f2=_tmp34E;_tmp353;});void*
_tmp355;void*_tmp356;void*_tmp357;void*_tmp358;void*_tmp359;void*_tmp35A;void*
_tmp35B;void*_tmp35C;void*_tmp35D;void*_tmp35E;void*_tmp35F;void*_tmp360;void*
_tmp361;void*_tmp362;void*_tmp363;void*_tmp364;void*_tmp365;void*_tmp366;void*
_tmp367;void*_tmp368;_LL222: _tmp355=_tmp354.f1;if(_tmp355 <= (void*)3)goto _LL224;
if(*((int*)_tmp355)!= 0)goto _LL224;_tmp356=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp355)->f1;_tmp357=_tmp354.f2;if((int)_tmp357 != 0)goto _LL224;_LL223: {void*
_tmp36A;void*_tmp36B;struct _tuple0 _tmp369=Cyc_NewControlFlow_splitzero(env,
_tmp343,_tmp343,_tmp34B,_tmp356);_tmp36A=_tmp369.f1;_tmp36B=_tmp369.f2;{void*
_tmp36C=_tmp321;_LL237: if((int)_tmp36C != 5)goto _LL239;_LL238: return({struct
_tuple0 _tmp36D;_tmp36D.f1=_tmp36B;_tmp36D.f2=_tmp36A;_tmp36D;});_LL239: if((int)
_tmp36C != 6)goto _LL23B;_LL23A: return({struct _tuple0 _tmp36E;_tmp36E.f1=_tmp36A;
_tmp36E.f2=_tmp36B;_tmp36E;});_LL23B:;_LL23C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F[0]=({struct Cyc_Core_Impossible_struct
_tmp370;_tmp370.tag=Cyc_Core_Impossible;_tmp370.f1=({const char*_tmp371="anal_test, zero-split";
_tag_dynforward(_tmp371,sizeof(char),_get_zero_arr_size(_tmp371,22));});_tmp370;});
_tmp36F;}));_LL236:;}}_LL224: _tmp358=_tmp354.f1;if((int)_tmp358 != 0)goto _LL226;
_tmp359=_tmp354.f2;if(_tmp359 <= (void*)3)goto _LL226;if(*((int*)_tmp359)!= 0)goto
_LL226;_tmp35A=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp359)->f1;
_LL225: {void*_tmp373;void*_tmp374;struct _tuple0 _tmp372=Cyc_NewControlFlow_splitzero(
env,_tmp343,_tmp343,_tmp34C,_tmp35A);_tmp373=_tmp372.f1;_tmp374=_tmp372.f2;{void*
_tmp375=_tmp321;_LL23E: if((int)_tmp375 != 5)goto _LL240;_LL23F: return({struct
_tuple0 _tmp376;_tmp376.f1=_tmp374;_tmp376.f2=_tmp373;_tmp376;});_LL240: if((int)
_tmp375 != 6)goto _LL242;_LL241: return({struct _tuple0 _tmp377;_tmp377.f1=_tmp373;
_tmp377.f2=_tmp374;_tmp377;});_LL242:;_LL243:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_Core_Impossible_struct
_tmp379;_tmp379.tag=Cyc_Core_Impossible;_tmp379.f1=({const char*_tmp37A="anal_test, zero-split";
_tag_dynforward(_tmp37A,sizeof(char),_get_zero_arr_size(_tmp37A,22));});_tmp379;});
_tmp378;}));_LL23D:;}}_LL226: _tmp35B=_tmp354.f1;if((int)_tmp35B != 0)goto _LL228;
_tmp35C=_tmp354.f2;if((int)_tmp35C != 0)goto _LL228;_LL227: if(_tmp321 == (void*)5)
return({struct _tuple0 _tmp37B;_tmp37B.f1=_tmp343;_tmp37B.f2=(void*)0;_tmp37B;});
else{return({struct _tuple0 _tmp37C;_tmp37C.f1=(void*)0;_tmp37C.f2=_tmp343;_tmp37C;});}
_LL228: _tmp35D=_tmp354.f1;if((int)_tmp35D != 0)goto _LL22A;_tmp35E=_tmp354.f2;if((
int)_tmp35E != 1)goto _LL22A;_LL229: goto _LL22B;_LL22A: _tmp35F=_tmp354.f1;if((int)
_tmp35F != 0)goto _LL22C;_tmp360=_tmp354.f2;if((int)_tmp360 != 2)goto _LL22C;_LL22B:
goto _LL22D;_LL22C: _tmp361=_tmp354.f1;if((int)_tmp361 != 0)goto _LL22E;_tmp362=
_tmp354.f2;if(_tmp362 <= (void*)3)goto _LL22E;if(*((int*)_tmp362)!= 2)goto _LL22E;
_LL22D: goto _LL22F;_LL22E: _tmp363=_tmp354.f1;if((int)_tmp363 != 1)goto _LL230;
_tmp364=_tmp354.f2;if((int)_tmp364 != 0)goto _LL230;_LL22F: goto _LL231;_LL230:
_tmp365=_tmp354.f1;if((int)_tmp365 != 2)goto _LL232;_tmp366=_tmp354.f2;if((int)
_tmp366 != 0)goto _LL232;_LL231: goto _LL233;_LL232: _tmp367=_tmp354.f1;if(_tmp367 <= (
void*)3)goto _LL234;if(*((int*)_tmp367)!= 2)goto _LL234;_tmp368=_tmp354.f2;if((int)
_tmp368 != 0)goto _LL234;_LL233: if(_tmp321 == (void*)6)return({struct _tuple0 _tmp37D;
_tmp37D.f1=_tmp343;_tmp37D.f2=(void*)0;_tmp37D;});else{return({struct _tuple0
_tmp37E;_tmp37E.f1=(void*)0;_tmp37E.f2=_tmp343;_tmp37E;});}_LL234:;_LL235: goto
_LL221;_LL221:;}{struct _tuple0 _tmp380=({struct _tuple0 _tmp37F;_tmp37F.f1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp34B->topt))->v);_tmp37F.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp34C->topt))->v);_tmp37F;});void*
_tmp381;void*_tmp382;void*_tmp383;void*_tmp384;void*_tmp385;void*_tmp386;_LL245:
_tmp381=_tmp380.f1;if(_tmp381 <= (void*)4)goto _LL247;if(*((int*)_tmp381)!= 5)goto
_LL247;_tmp382=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp381)->f1;if((int)
_tmp382 != 1)goto _LL247;_LL246: goto _LL248;_LL247: _tmp383=_tmp380.f2;if(_tmp383 <= (
void*)4)goto _LL249;if(*((int*)_tmp383)!= 5)goto _LL249;_tmp384=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp383)->f1;if((int)_tmp384 != 1)goto _LL249;_LL248: goto _LL24A;_LL249: _tmp385=
_tmp380.f1;if(_tmp385 <= (void*)4)goto _LL24B;if(*((int*)_tmp385)!= 17)goto _LL24B;
_LL24A: goto _LL24C;_LL24B: _tmp386=_tmp380.f2;if(_tmp386 <= (void*)4)goto _LL24D;if(*((
int*)_tmp386)!= 17)goto _LL24D;_LL24C: goto _LL244;_LL24D:;_LL24E: return({struct
_tuple0 _tmp387;_tmp387.f1=_tmp343;_tmp387.f2=_tmp343;_tmp387;});_LL244:;}{void*
_tmp388=_tmp321;_LL250: if((int)_tmp388 != 5)goto _LL252;_LL251: {void*_tmp389=Cyc_NewControlFlow_if_tagcmp(
env,_tmp343,_tmp34B,_tmp34D,(void*)5,_tmp34E);_tmp389=Cyc_NewControlFlow_if_tagcmp(
env,_tmp389,_tmp34C,_tmp34E,(void*)5,_tmp34D);return({struct _tuple0 _tmp38A;
_tmp38A.f1=_tmp389;_tmp38A.f2=_tmp343;_tmp38A;});}_LL252: if((int)_tmp388 != 6)
goto _LL254;_LL253: {void*_tmp38B=Cyc_NewControlFlow_if_tagcmp(env,_tmp343,
_tmp34B,_tmp34D,(void*)5,_tmp34E);_tmp38B=Cyc_NewControlFlow_if_tagcmp(env,
_tmp38B,_tmp34C,_tmp34E,(void*)5,_tmp34D);return({struct _tuple0 _tmp38C;_tmp38C.f1=
_tmp343;_tmp38C.f2=_tmp38B;_tmp38C;});}_LL254: if((int)_tmp388 != 7)goto _LL256;
_LL255: {void*_tmp38D=Cyc_NewControlFlow_if_tagcmp(env,_tmp343,_tmp34C,_tmp34E,(
void*)8,_tmp34D);void*_tmp38E=Cyc_NewControlFlow_if_tagcmp(env,_tmp343,_tmp34B,
_tmp34D,(void*)10,_tmp34E);return({struct _tuple0 _tmp38F;_tmp38F.f1=_tmp38D;
_tmp38F.f2=_tmp38E;_tmp38F;});}_LL256: if((int)_tmp388 != 9)goto _LL258;_LL257: {
void*_tmp390=Cyc_NewControlFlow_if_tagcmp(env,_tmp343,_tmp34C,_tmp34E,(void*)10,
_tmp34D);void*_tmp391=Cyc_NewControlFlow_if_tagcmp(env,_tmp343,_tmp34B,_tmp34D,(
void*)8,_tmp34E);return({struct _tuple0 _tmp392;_tmp392.f1=_tmp390;_tmp392.f2=
_tmp391;_tmp392;});}_LL258: if((int)_tmp388 != 8)goto _LL25A;_LL259: {void*_tmp393=
Cyc_NewControlFlow_if_tagcmp(env,_tmp343,_tmp34B,_tmp34D,(void*)8,_tmp34E);void*
_tmp394=Cyc_NewControlFlow_if_tagcmp(env,_tmp343,_tmp34C,_tmp34E,(void*)10,
_tmp34D);{void*_tmp395=_tmp393;struct Cyc_Dict_Dict*_tmp396;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp397;_LL25F: if((int)_tmp395 != 0)goto _LL261;_LL260:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398[0]=({
struct Cyc_Core_Impossible_struct _tmp399;_tmp399.tag=Cyc_Core_Impossible;_tmp399.f1=({
const char*_tmp39A="anal_test, Lt";_tag_dynforward(_tmp39A,sizeof(char),
_get_zero_arr_size(_tmp39A,14));});_tmp399;});_tmp398;}));_LL261: if(_tmp395 <= (
void*)1)goto _LL25E;if(*((int*)_tmp395)!= 0)goto _LL25E;_tmp396=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp395)->f1;_tmp397=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp395)->f3;
_LL262: _tmp347=_tmp396;_tmp349=_tmp397;_LL25E:;}{void*_tmp39B=(void*)_tmp34B->r;
void*_tmp39C;struct Cyc_Absyn_Vardecl*_tmp39D;void*_tmp39E;struct Cyc_Absyn_Vardecl*
_tmp39F;void*_tmp3A0;struct Cyc_Absyn_Vardecl*_tmp3A1;void*_tmp3A2;struct Cyc_Absyn_Vardecl*
_tmp3A3;_LL264: if(*((int*)_tmp39B)!= 1)goto _LL266;_tmp39C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp39B)->f2;if(_tmp39C <= (void*)1)goto _LL266;if(*((int*)_tmp39C)!= 0)goto _LL266;
_tmp39D=((struct Cyc_Absyn_Global_b_struct*)_tmp39C)->f1;if(!(!_tmp39D->escapes))
goto _LL266;_LL265: _tmp39F=_tmp39D;goto _LL267;_LL266: if(*((int*)_tmp39B)!= 1)goto
_LL268;_tmp39E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp39B)->f2;if(_tmp39E <= (
void*)1)goto _LL268;if(*((int*)_tmp39E)!= 3)goto _LL268;_tmp39F=((struct Cyc_Absyn_Local_b_struct*)
_tmp39E)->f1;if(!(!_tmp39F->escapes))goto _LL268;_LL267: _tmp3A1=_tmp39F;goto
_LL269;_LL268: if(*((int*)_tmp39B)!= 1)goto _LL26A;_tmp3A0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp39B)->f2;if(_tmp3A0 <= (void*)1)goto _LL26A;if(*((int*)_tmp3A0)!= 4)goto _LL26A;
_tmp3A1=((struct Cyc_Absyn_Pat_b_struct*)_tmp3A0)->f1;if(!(!_tmp3A1->escapes))
goto _LL26A;_LL269: _tmp3A3=_tmp3A1;goto _LL26B;_LL26A: if(*((int*)_tmp39B)!= 1)goto
_LL26C;_tmp3A2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp39B)->f2;if(_tmp3A2 <= (
void*)1)goto _LL26C;if(*((int*)_tmp3A2)!= 2)goto _LL26C;_tmp3A3=((struct Cyc_Absyn_Param_b_struct*)
_tmp3A2)->f1;if(!(!_tmp3A3->escapes))goto _LL26C;_LL26B: {void*_tmp3A4=(void*)
_tmp34C->r;void*_tmp3A5;struct Cyc_Absyn_Vardecl*_tmp3A6;void*_tmp3A7;struct Cyc_Absyn_Vardecl*
_tmp3A8;void*_tmp3A9;struct Cyc_Absyn_Vardecl*_tmp3AA;void*_tmp3AB;struct Cyc_Absyn_Vardecl*
_tmp3AC;void*_tmp3AD;int _tmp3AE;void*_tmp3AF;struct Cyc_List_List*_tmp3B0;struct
Cyc_List_List _tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;_LL26F: if(*((int*)_tmp3A4)!= 1)
goto _LL271;_tmp3A5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3A4)->f2;if(
_tmp3A5 <= (void*)1)goto _LL271;if(*((int*)_tmp3A5)!= 0)goto _LL271;_tmp3A6=((
struct Cyc_Absyn_Global_b_struct*)_tmp3A5)->f1;if(!(!_tmp3A6->escapes))goto _LL271;
_LL270: _tmp3A8=_tmp3A6;goto _LL272;_LL271: if(*((int*)_tmp3A4)!= 1)goto _LL273;
_tmp3A7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3A4)->f2;if(_tmp3A7 <= (void*)
1)goto _LL273;if(*((int*)_tmp3A7)!= 3)goto _LL273;_tmp3A8=((struct Cyc_Absyn_Local_b_struct*)
_tmp3A7)->f1;if(!(!_tmp3A8->escapes))goto _LL273;_LL272: _tmp3AA=_tmp3A8;goto
_LL274;_LL273: if(*((int*)_tmp3A4)!= 1)goto _LL275;_tmp3A9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3A4)->f2;if(_tmp3A9 <= (void*)1)goto _LL275;if(*((int*)_tmp3A9)!= 4)goto _LL275;
_tmp3AA=((struct Cyc_Absyn_Pat_b_struct*)_tmp3A9)->f1;if(!(!_tmp3AA->escapes))
goto _LL275;_LL274: _tmp3AC=_tmp3AA;goto _LL276;_LL275: if(*((int*)_tmp3A4)!= 1)goto
_LL277;_tmp3AB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3A4)->f2;if(_tmp3AB <= (
void*)1)goto _LL277;if(*((int*)_tmp3AB)!= 2)goto _LL277;_tmp3AC=((struct Cyc_Absyn_Param_b_struct*)
_tmp3AB)->f1;if(!(!_tmp3AC->escapes))goto _LL277;_LL276: {struct Cyc_List_List*
_tmp3B3=({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->vd=
_tmp3A3;_tmp3B8->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp3BA;_tmp3BA.tag=1;_tmp3BA.f1=_tmp3AC;_tmp3BA;});_tmp3B9;}));_tmp3B8;});
_tmp3B7->tl=_tmp348;_tmp3B7;});return({struct _tuple0 _tmp3B4;_tmp3B4.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));
_tmp3B5[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3B6;_tmp3B6.tag=0;
_tmp3B6.f1=_tmp347;_tmp3B6.f2=_tmp3B3;_tmp3B6.f3=_tmp349;_tmp3B6;});_tmp3B5;});
_tmp3B4.f2=_tmp394;_tmp3B4;});}_LL277: if(*((int*)_tmp3A4)!= 0)goto _LL279;_tmp3AD=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp3A4)->f1;if(_tmp3AD <= (void*)1)goto
_LL279;if(*((int*)_tmp3AD)!= 2)goto _LL279;_tmp3AE=((struct Cyc_Absyn_Int_c_struct*)
_tmp3AD)->f2;_LL278: {struct Cyc_List_List*_tmp3BB=({struct Cyc_List_List*_tmp3BF=
_cycalloc(sizeof(*_tmp3BF));_tmp3BF->hd=({struct Cyc_CfFlowInfo_Reln*_tmp3C0=
_cycalloc(sizeof(*_tmp3C0));_tmp3C0->vd=_tmp3A3;_tmp3C0->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessConst_struct*_tmp3C1=_cycalloc_atomic(sizeof(*_tmp3C1));
_tmp3C1[0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp3C2;_tmp3C2.tag=3;_tmp3C2.f1=(
unsigned int)_tmp3AE;_tmp3C2;});_tmp3C1;}));_tmp3C0;});_tmp3BF->tl=_tmp348;
_tmp3BF;});return({struct _tuple0 _tmp3BC;_tmp3BC.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3BE;_tmp3BE.tag=0;_tmp3BE.f1=_tmp347;_tmp3BE.f2=_tmp3BB;_tmp3BE.f3=_tmp349;
_tmp3BE;});_tmp3BD;});_tmp3BC.f2=_tmp394;_tmp3BC;});}_LL279: if(*((int*)_tmp3A4)
!= 3)goto _LL27B;_tmp3AF=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3A4)->f1;
if((int)_tmp3AF != 19)goto _LL27B;_tmp3B0=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3A4)->f2;if(_tmp3B0 == 0)goto _LL27B;_tmp3B1=*_tmp3B0;_tmp3B2=(struct Cyc_Absyn_Exp*)
_tmp3B1.hd;_LL27A: {void*_tmp3C3=(void*)_tmp3B2->r;void*_tmp3C4;struct Cyc_Absyn_Vardecl*
_tmp3C5;void*_tmp3C6;struct Cyc_Absyn_Vardecl*_tmp3C7;void*_tmp3C8;struct Cyc_Absyn_Vardecl*
_tmp3C9;void*_tmp3CA;struct Cyc_Absyn_Vardecl*_tmp3CB;_LL27E: if(*((int*)_tmp3C3)
!= 1)goto _LL280;_tmp3C4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3C3)->f2;if(
_tmp3C4 <= (void*)1)goto _LL280;if(*((int*)_tmp3C4)!= 0)goto _LL280;_tmp3C5=((
struct Cyc_Absyn_Global_b_struct*)_tmp3C4)->f1;if(!(!_tmp3C5->escapes))goto _LL280;
_LL27F: _tmp3C7=_tmp3C5;goto _LL281;_LL280: if(*((int*)_tmp3C3)!= 1)goto _LL282;
_tmp3C6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3C3)->f2;if(_tmp3C6 <= (void*)
1)goto _LL282;if(*((int*)_tmp3C6)!= 3)goto _LL282;_tmp3C7=((struct Cyc_Absyn_Local_b_struct*)
_tmp3C6)->f1;if(!(!_tmp3C7->escapes))goto _LL282;_LL281: _tmp3C9=_tmp3C7;goto
_LL283;_LL282: if(*((int*)_tmp3C3)!= 1)goto _LL284;_tmp3C8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3C3)->f2;if(_tmp3C8 <= (void*)1)goto _LL284;if(*((int*)_tmp3C8)!= 4)goto _LL284;
_tmp3C9=((struct Cyc_Absyn_Pat_b_struct*)_tmp3C8)->f1;if(!(!_tmp3C9->escapes))
goto _LL284;_LL283: _tmp3CB=_tmp3C9;goto _LL285;_LL284: if(*((int*)_tmp3C3)!= 1)goto
_LL286;_tmp3CA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3C3)->f2;if(_tmp3CA <= (
void*)1)goto _LL286;if(*((int*)_tmp3CA)!= 2)goto _LL286;_tmp3CB=((struct Cyc_Absyn_Param_b_struct*)
_tmp3CA)->f1;if(!(!_tmp3CB->escapes))goto _LL286;_LL285: {struct Cyc_List_List*
_tmp3CC=({struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->vd=
_tmp3A3;_tmp3D1->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp3D3;_tmp3D3.tag=2;_tmp3D3.f1=_tmp3CB;_tmp3D3;});_tmp3D2;}));_tmp3D1;});
_tmp3D0->tl=_tmp348;_tmp3D0;});return({struct _tuple0 _tmp3CD;_tmp3CD.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));
_tmp3CE[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3CF;_tmp3CF.tag=0;
_tmp3CF.f1=_tmp347;_tmp3CF.f2=_tmp3CC;_tmp3CF.f3=_tmp349;_tmp3CF;});_tmp3CE;});
_tmp3CD.f2=_tmp394;_tmp3CD;});}_LL286:;_LL287: return({struct _tuple0 _tmp3D4;
_tmp3D4.f1=_tmp393;_tmp3D4.f2=_tmp394;_tmp3D4;});_LL27D:;}_LL27B:;_LL27C: return({
struct _tuple0 _tmp3D5;_tmp3D5.f1=_tmp393;_tmp3D5.f2=_tmp394;_tmp3D5;});_LL26E:;}
_LL26C:;_LL26D: return({struct _tuple0 _tmp3D6;_tmp3D6.f1=_tmp393;_tmp3D6.f2=
_tmp394;_tmp3D6;});_LL263:;}}_LL25A: if((int)_tmp388 != 10)goto _LL25C;_LL25B: {
void*_tmp3D7=Cyc_NewControlFlow_if_tagcmp(env,_tmp343,_tmp34B,_tmp34D,(void*)10,
_tmp34E);void*_tmp3D8=Cyc_NewControlFlow_if_tagcmp(env,_tmp343,_tmp34C,_tmp34D,(
void*)8,_tmp34D);{void*_tmp3D9=_tmp3D7;struct Cyc_Dict_Dict*_tmp3DA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3DB;_LL289: if((int)_tmp3D9 != 0)goto _LL28B;_LL28A:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC[0]=({
struct Cyc_Core_Impossible_struct _tmp3DD;_tmp3DD.tag=Cyc_Core_Impossible;_tmp3DD.f1=({
const char*_tmp3DE="anal_test, Lte";_tag_dynforward(_tmp3DE,sizeof(char),
_get_zero_arr_size(_tmp3DE,15));});_tmp3DD;});_tmp3DC;}));_LL28B: if(_tmp3D9 <= (
void*)1)goto _LL288;if(*((int*)_tmp3D9)!= 0)goto _LL288;_tmp3DA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3D9)->f1;_tmp3DB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3D9)->f3;
_LL28C: _tmp347=_tmp3DA;_tmp349=_tmp3DB;_LL288:;}{void*_tmp3DF=(void*)_tmp34B->r;
void*_tmp3E0;struct Cyc_Absyn_Vardecl*_tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Vardecl*
_tmp3E3;void*_tmp3E4;struct Cyc_Absyn_Vardecl*_tmp3E5;void*_tmp3E6;struct Cyc_Absyn_Vardecl*
_tmp3E7;_LL28E: if(*((int*)_tmp3DF)!= 1)goto _LL290;_tmp3E0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3DF)->f2;if(_tmp3E0 <= (void*)1)goto _LL290;if(*((int*)_tmp3E0)!= 0)goto _LL290;
_tmp3E1=((struct Cyc_Absyn_Global_b_struct*)_tmp3E0)->f1;if(!(!_tmp3E1->escapes))
goto _LL290;_LL28F: _tmp3E3=_tmp3E1;goto _LL291;_LL290: if(*((int*)_tmp3DF)!= 1)goto
_LL292;_tmp3E2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3DF)->f2;if(_tmp3E2 <= (
void*)1)goto _LL292;if(*((int*)_tmp3E2)!= 3)goto _LL292;_tmp3E3=((struct Cyc_Absyn_Local_b_struct*)
_tmp3E2)->f1;if(!(!_tmp3E3->escapes))goto _LL292;_LL291: _tmp3E5=_tmp3E3;goto
_LL293;_LL292: if(*((int*)_tmp3DF)!= 1)goto _LL294;_tmp3E4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3DF)->f2;if(_tmp3E4 <= (void*)1)goto _LL294;if(*((int*)_tmp3E4)!= 4)goto _LL294;
_tmp3E5=((struct Cyc_Absyn_Pat_b_struct*)_tmp3E4)->f1;if(!(!_tmp3E5->escapes))
goto _LL294;_LL293: _tmp3E7=_tmp3E5;goto _LL295;_LL294: if(*((int*)_tmp3DF)!= 1)goto
_LL296;_tmp3E6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3DF)->f2;if(_tmp3E6 <= (
void*)1)goto _LL296;if(*((int*)_tmp3E6)!= 2)goto _LL296;_tmp3E7=((struct Cyc_Absyn_Param_b_struct*)
_tmp3E6)->f1;if(!(!_tmp3E7->escapes))goto _LL296;_LL295: {void*_tmp3E8=(void*)
_tmp34C->r;void*_tmp3E9;struct Cyc_List_List*_tmp3EA;struct Cyc_List_List _tmp3EB;
struct Cyc_Absyn_Exp*_tmp3EC;_LL299: if(*((int*)_tmp3E8)!= 3)goto _LL29B;_tmp3E9=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3E8)->f1;if((int)_tmp3E9 != 19)goto
_LL29B;_tmp3EA=((struct Cyc_Absyn_Primop_e_struct*)_tmp3E8)->f2;if(_tmp3EA == 0)
goto _LL29B;_tmp3EB=*_tmp3EA;_tmp3EC=(struct Cyc_Absyn_Exp*)_tmp3EB.hd;_LL29A: {
void*_tmp3ED=(void*)_tmp3EC->r;void*_tmp3EE;struct Cyc_Absyn_Vardecl*_tmp3EF;void*
_tmp3F0;struct Cyc_Absyn_Vardecl*_tmp3F1;void*_tmp3F2;struct Cyc_Absyn_Vardecl*
_tmp3F3;void*_tmp3F4;struct Cyc_Absyn_Vardecl*_tmp3F5;_LL29E: if(*((int*)_tmp3ED)
!= 1)goto _LL2A0;_tmp3EE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3ED)->f2;if(
_tmp3EE <= (void*)1)goto _LL2A0;if(*((int*)_tmp3EE)!= 0)goto _LL2A0;_tmp3EF=((
struct Cyc_Absyn_Global_b_struct*)_tmp3EE)->f1;if(!(!_tmp3EF->escapes))goto _LL2A0;
_LL29F: _tmp3F1=_tmp3EF;goto _LL2A1;_LL2A0: if(*((int*)_tmp3ED)!= 1)goto _LL2A2;
_tmp3F0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3ED)->f2;if(_tmp3F0 <= (void*)
1)goto _LL2A2;if(*((int*)_tmp3F0)!= 3)goto _LL2A2;_tmp3F1=((struct Cyc_Absyn_Local_b_struct*)
_tmp3F0)->f1;if(!(!_tmp3F1->escapes))goto _LL2A2;_LL2A1: _tmp3F3=_tmp3F1;goto
_LL2A3;_LL2A2: if(*((int*)_tmp3ED)!= 1)goto _LL2A4;_tmp3F2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3ED)->f2;if(_tmp3F2 <= (void*)1)goto _LL2A4;if(*((int*)_tmp3F2)!= 4)goto _LL2A4;
_tmp3F3=((struct Cyc_Absyn_Pat_b_struct*)_tmp3F2)->f1;if(!(!_tmp3F3->escapes))
goto _LL2A4;_LL2A3: _tmp3F5=_tmp3F3;goto _LL2A5;_LL2A4: if(*((int*)_tmp3ED)!= 1)goto
_LL2A6;_tmp3F4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3ED)->f2;if(_tmp3F4 <= (
void*)1)goto _LL2A6;if(*((int*)_tmp3F4)!= 2)goto _LL2A6;_tmp3F5=((struct Cyc_Absyn_Param_b_struct*)
_tmp3F4)->f1;if(!(!_tmp3F5->escapes))goto _LL2A6;_LL2A5: {struct Cyc_List_List*
_tmp3F6=({struct Cyc_List_List*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->vd=
_tmp3E7;_tmp3FB->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp3FD;_tmp3FD.tag=4;_tmp3FD.f1=_tmp3F5;_tmp3FD;});_tmp3FC;}));_tmp3FB;});
_tmp3FA->tl=_tmp348;_tmp3FA;});return({struct _tuple0 _tmp3F7;_tmp3F7.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));
_tmp3F8[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3F9;_tmp3F9.tag=0;
_tmp3F9.f1=_tmp347;_tmp3F9.f2=_tmp3F6;_tmp3F9.f3=_tmp349;_tmp3F9;});_tmp3F8;});
_tmp3F7.f2=_tmp3D8;_tmp3F7;});}_LL2A6:;_LL2A7: return({struct _tuple0 _tmp3FE;
_tmp3FE.f1=_tmp3D7;_tmp3FE.f2=_tmp3D8;_tmp3FE;});_LL29D:;}_LL29B:;_LL29C: return({
struct _tuple0 _tmp3FF;_tmp3FF.f1=_tmp3D7;_tmp3FF.f2=_tmp3D8;_tmp3FF;});_LL298:;}
_LL296:;_LL297: return({struct _tuple0 _tmp400;_tmp400.f1=_tmp3D7;_tmp400.f2=
_tmp3D8;_tmp400;});_LL28D:;}}_LL25C:;_LL25D: return({struct _tuple0 _tmp401;_tmp401.f1=
_tmp343;_tmp401.f2=_tmp343;_tmp401;});_LL24F:;}}_LL21C:;}}_LL213:;_LL214: goto
_LL206;_LL206:;}{void*_tmp403;void*_tmp404;struct _tuple0 _tmp402=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp403=_tmp402.f1;_tmp404=_tmp402.f2;_tmp403=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp403);{void*_tmp405=_tmp403;struct Cyc_Dict_Dict*_tmp406;_LL2A9: if((int)
_tmp405 != 0)goto _LL2AB;_LL2AA: return({struct _tuple0 _tmp407;_tmp407.f1=_tmp403;
_tmp407.f2=_tmp403;_tmp407;});_LL2AB: if(_tmp405 <= (void*)1)goto _LL2A8;if(*((int*)
_tmp405)!= 0)goto _LL2A8;_tmp406=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp405)->f1;_LL2AC: {void*_tmp408=_tmp404;void*_tmp409;void*_tmp40A;void*
_tmp40B;_LL2AE: if((int)_tmp408 != 0)goto _LL2B0;_LL2AF: return({struct _tuple0
_tmp40C;_tmp40C.f1=(void*)0;_tmp40C.f2=_tmp403;_tmp40C;});_LL2B0: if((int)_tmp408
!= 2)goto _LL2B2;_LL2B1: goto _LL2B3;_LL2B2: if((int)_tmp408 != 1)goto _LL2B4;_LL2B3:
goto _LL2B5;_LL2B4: if(_tmp408 <= (void*)3)goto _LL2B6;if(*((int*)_tmp408)!= 2)goto
_LL2B6;_LL2B5: return({struct _tuple0 _tmp40D;_tmp40D.f1=_tmp403;_tmp40D.f2=(void*)
0;_tmp40D;});_LL2B6: if(_tmp408 <= (void*)3)goto _LL2B8;if(*((int*)_tmp408)!= 0)
goto _LL2B8;_tmp409=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp408)->f1;
if((int)_tmp409 != 0)goto _LL2B8;_LL2B7: goto _LL2B9;_LL2B8: if(_tmp408 <= (void*)3)
goto _LL2BA;if(*((int*)_tmp408)!= 1)goto _LL2BA;_tmp40A=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp408)->f1;if((int)_tmp40A != 0)goto _LL2BA;_LL2B9:({void*_tmp40E[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp40F="expression may not be initialized";_tag_dynforward(
_tmp40F,sizeof(char),_get_zero_arr_size(_tmp40F,34));}),_tag_dynforward(_tmp40E,
sizeof(void*),0));});return({struct _tuple0 _tmp410;_tmp410.f1=(void*)0;_tmp410.f2=(
void*)0;_tmp410;});_LL2BA: if(_tmp408 <= (void*)3)goto _LL2BC;if(*((int*)_tmp408)!= 
0)goto _LL2BC;_tmp40B=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp408)->f1;
_LL2BB: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp403,e,_tmp40B);_LL2BC:
if(_tmp408 <= (void*)3)goto _LL2BE;if(*((int*)_tmp408)!= 1)goto _LL2BE;_LL2BD:
return({struct _tuple0 _tmp411;_tmp411.f1=_tmp403;_tmp411.f2=_tmp403;_tmp411;});
_LL2BE: if(_tmp408 <= (void*)3)goto _LL2C0;if(*((int*)_tmp408)!= 3)goto _LL2C0;
_LL2BF: return({struct _tuple0 _tmp412;_tmp412.f1=_tmp403;_tmp412.f2=_tmp403;
_tmp412;});_LL2C0: if(_tmp408 <= (void*)3)goto _LL2AD;if(*((int*)_tmp408)!= 4)goto
_LL2AD;_LL2C1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp413=
_cycalloc(sizeof(*_tmp413));_tmp413[0]=({struct Cyc_Core_Impossible_struct _tmp414;
_tmp414.tag=Cyc_Core_Impossible;_tmp414.f1=({const char*_tmp415="anal_test";
_tag_dynforward(_tmp415,sizeof(char),_get_zero_arr_size(_tmp415,10));});_tmp414;});
_tmp413;}));_LL2AD:;}_LL2A8:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,void*flow){
void*_tmp416=flow;struct Cyc_Dict_Dict*_tmp417;_LL2C3: if((int)_tmp416 != 0)goto
_LL2C5;_LL2C4: return;_LL2C5: if(_tmp416 <= (void*)1)goto _LL2C2;if(*((int*)_tmp416)
!= 0)goto _LL2C2;_tmp417=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp416)->f1;
_LL2C6:{struct Cyc_List_List*_tmp418=env->param_roots;for(0;_tmp418 != 0;_tmp418=
_tmp418->tl){if(Cyc_CfFlowInfo_initlevel(_tmp417,Cyc_CfFlowInfo_lookup_place(
_tmp417,(struct Cyc_CfFlowInfo_Place*)_tmp418->hd))!= (void*)2)({void*_tmp419[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp41A="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp41A,sizeof(char),_get_zero_arr_size(_tmp41A,76));}),
_tag_dynforward(_tmp419,sizeof(void*),0));});}}return;_LL2C2:;}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp41C;struct Cyc_Core_Opt*
_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_Stmt*_tmp41F;struct Cyc_Position_Segment*
_tmp420;struct Cyc_Absyn_Switch_clause*_tmp41B=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp41C=*_tmp41B;_tmp41D=_tmp41C.pat_vars;_tmp41E=_tmp41C.where_clause;
_tmp41F=_tmp41C.body;_tmp420=_tmp41C.loc;{void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp41D))->v,Cyc_CfFlowInfo_unknown_all,
_tmp420);void*clause_outflow;if(_tmp41E != 0){void*_tmp422;void*_tmp423;struct
_tuple0 _tmp421=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_tmp41E);_tmp422=_tmp421.f1;_tmp423=_tmp421.f2;inflow=_tmp423;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp422,_tmp41F);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp41F);}{void*_tmp424=clause_outflow;_LL2C8: if((int)_tmp424 != 0)
goto _LL2CA;_LL2C9: goto _LL2C7;_LL2CA:;_LL2CB: if(scs->tl == 0)return clause_outflow;
else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*
_tmp425[0]={};Cyc_Tcutil_terr(_tmp41F->loc,({const char*_tmp426="switch clause may implicitly fallthru";
_tag_dynforward(_tmp426,sizeof(char),_get_zero_arr_size(_tmp426,38));}),
_tag_dynforward(_tmp425,sizeof(void*),0));});else{({void*_tmp427[0]={};Cyc_Tcutil_warn(
_tmp41F->loc,({const char*_tmp428="switch clause may implicitly fallthru";
_tag_dynforward(_tmp428,sizeof(char),_get_zero_arr_size(_tmp428,38));}),
_tag_dynforward(_tmp427,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL2C7;_LL2C7:;}}}return(void*)0;}static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){void*
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp429=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);inflow=(void*)_tmp429->flow;{void*_tmp42A=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp42B;struct Cyc_Absyn_Exp*_tmp42C;struct Cyc_Absyn_Exp*_tmp42D;struct Cyc_Absyn_Stmt*
_tmp42E;struct Cyc_Absyn_Stmt*_tmp42F;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Stmt*
_tmp431;struct Cyc_Absyn_Stmt*_tmp432;struct _tuple3 _tmp433;struct Cyc_Absyn_Exp*
_tmp434;struct Cyc_Absyn_Stmt*_tmp435;struct Cyc_Absyn_Stmt*_tmp436;struct Cyc_Absyn_Stmt*
_tmp437;struct _tuple3 _tmp438;struct Cyc_Absyn_Exp*_tmp439;struct Cyc_Absyn_Stmt*
_tmp43A;struct Cyc_Absyn_Exp*_tmp43B;struct _tuple3 _tmp43C;struct Cyc_Absyn_Exp*
_tmp43D;struct Cyc_Absyn_Stmt*_tmp43E;struct _tuple3 _tmp43F;struct Cyc_Absyn_Exp*
_tmp440;struct Cyc_Absyn_Stmt*_tmp441;struct Cyc_Absyn_Stmt*_tmp442;struct Cyc_Absyn_Stmt*
_tmp443;struct Cyc_List_List*_tmp444;struct Cyc_Absyn_Switch_clause**_tmp445;
struct Cyc_Absyn_Switch_clause*_tmp446;struct Cyc_Absyn_Stmt*_tmp447;struct Cyc_Absyn_Stmt*
_tmp448;struct Cyc_Absyn_Stmt*_tmp449;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_List_List*
_tmp44B;struct Cyc_Absyn_Stmt*_tmp44C;struct Cyc_List_List*_tmp44D;struct Cyc_Absyn_Decl*
_tmp44E;struct Cyc_Absyn_Stmt*_tmp44F;struct Cyc_Absyn_Stmt*_tmp450;struct Cyc_Absyn_Tvar*
_tmp451;struct Cyc_Absyn_Vardecl*_tmp452;int _tmp453;struct Cyc_Absyn_Stmt*_tmp454;
struct Cyc_Absyn_Exp*_tmp455;_LL2CD: if((int)_tmp42A != 0)goto _LL2CF;_LL2CE: return
inflow;_LL2CF: if(_tmp42A <= (void*)1)goto _LL2F5;if(*((int*)_tmp42A)!= 2)goto
_LL2D1;_tmp42B=((struct Cyc_Absyn_Return_s_struct*)_tmp42A)->f1;if(_tmp42B != 0)
goto _LL2D1;_LL2D0: if(env->noreturn)({void*_tmp456[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp457="`noreturn' function might return";_tag_dynforward(_tmp457,
sizeof(char),_get_zero_arr_size(_tmp457,33));}),_tag_dynforward(_tmp456,sizeof(
void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return(
void*)0;_LL2D1: if(*((int*)_tmp42A)!= 2)goto _LL2D3;_tmp42C=((struct Cyc_Absyn_Return_s_struct*)
_tmp42A)->f1;_LL2D2: if(env->noreturn)({void*_tmp458[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp459="`noreturn' function might return";_tag_dynforward(_tmp459,
sizeof(char),_get_zero_arr_size(_tmp459,33));}),_tag_dynforward(_tmp458,sizeof(
void*),0));});{void*_tmp45B;void*_tmp45C;struct _tuple0 _tmp45A=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp42C));_tmp45B=_tmp45A.f1;
_tmp45C=_tmp45A.f2;_tmp45B=Cyc_CfFlowInfo_consume_unique_rvals(_tmp42C->loc,
_tmp45B);_tmp45B=Cyc_NewControlFlow_use_Rval(env,_tmp42C->loc,_tmp45B,_tmp45C);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp45B);return(void*)0;}_LL2D3:
if(*((int*)_tmp42A)!= 0)goto _LL2D5;_tmp42D=((struct Cyc_Absyn_Exp_s_struct*)
_tmp42A)->f1;_LL2D4: outflow=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp42D)).f1;
goto _LL2CC;_LL2D5: if(*((int*)_tmp42A)!= 1)goto _LL2D7;_tmp42E=((struct Cyc_Absyn_Seq_s_struct*)
_tmp42A)->f1;_tmp42F=((struct Cyc_Absyn_Seq_s_struct*)_tmp42A)->f2;_LL2D6: outflow=
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp42E),
_tmp42F);goto _LL2CC;_LL2D7: if(*((int*)_tmp42A)!= 3)goto _LL2D9;_tmp430=((struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp42A)->f1;_tmp431=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp42A)->f2;_tmp432=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp42A)->f3;_LL2D8: {
void*_tmp45E;void*_tmp45F;struct _tuple0 _tmp45D=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp430);_tmp45E=_tmp45D.f1;_tmp45F=_tmp45D.f2;_tmp45E=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp430->loc,_tmp45E);_tmp45F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp430->loc,
_tmp45F);outflow=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp45E,_tmp431),Cyc_NewControlFlow_anal_stmt(env,_tmp45F,_tmp432),1);goto
_LL2CC;}_LL2D9: if(*((int*)_tmp42A)!= 4)goto _LL2DB;_tmp433=((struct Cyc_Absyn_While_s_struct*)
_tmp42A)->f1;_tmp434=_tmp433.f1;_tmp435=_tmp433.f2;_tmp436=((struct Cyc_Absyn_While_s_struct*)
_tmp42A)->f2;_LL2DA: {void*_tmp460=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmp435))->flow;void*_tmp462;void*_tmp463;struct _tuple0 _tmp461=Cyc_NewControlFlow_anal_test(
env,_tmp460,_tmp434);_tmp462=_tmp461.f1;_tmp463=_tmp461.f2;_tmp462=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp434->loc,_tmp462);_tmp463=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp434->loc,
_tmp463);{void*_tmp464=Cyc_NewControlFlow_anal_stmt(env,_tmp462,_tmp436);Cyc_NewControlFlow_update_flow(
env,_tmp435,_tmp464);outflow=_tmp463;goto _LL2CC;}}_LL2DB: if(*((int*)_tmp42A)!= 
13)goto _LL2DD;_tmp437=((struct Cyc_Absyn_Do_s_struct*)_tmp42A)->f1;_tmp438=((
struct Cyc_Absyn_Do_s_struct*)_tmp42A)->f2;_tmp439=_tmp438.f1;_tmp43A=_tmp438.f2;
_LL2DC: {void*_tmp465=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp437);void*
_tmp466=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp465,_tmp43A))->flow;
void*_tmp468;void*_tmp469;struct _tuple0 _tmp467=Cyc_NewControlFlow_anal_test(env,
_tmp466,_tmp439);_tmp468=_tmp467.f1;_tmp469=_tmp467.f2;Cyc_NewControlFlow_update_flow(
env,_tmp437,_tmp468);outflow=_tmp469;goto _LL2CC;}_LL2DD: if(*((int*)_tmp42A)!= 8)
goto _LL2DF;_tmp43B=((struct Cyc_Absyn_For_s_struct*)_tmp42A)->f1;_tmp43C=((struct
Cyc_Absyn_For_s_struct*)_tmp42A)->f2;_tmp43D=_tmp43C.f1;_tmp43E=_tmp43C.f2;
_tmp43F=((struct Cyc_Absyn_For_s_struct*)_tmp42A)->f3;_tmp440=_tmp43F.f1;_tmp441=
_tmp43F.f2;_tmp442=((struct Cyc_Absyn_For_s_struct*)_tmp42A)->f4;_LL2DE: {void*
_tmp46A=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp43B)).f1;_tmp46A=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp43B->loc,_tmp46A);{void*_tmp46B=(void*)(Cyc_NewControlFlow_pre_stmt_check(
env,_tmp46A,_tmp43E))->flow;void*_tmp46D;void*_tmp46E;struct _tuple0 _tmp46C=Cyc_NewControlFlow_anal_test(
env,_tmp46B,_tmp43D);_tmp46D=_tmp46C.f1;_tmp46E=_tmp46C.f2;{void*_tmp46F=Cyc_NewControlFlow_anal_stmt(
env,_tmp46D,_tmp442);void*_tmp470=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp46F,_tmp441))->flow;void*_tmp471=(Cyc_NewControlFlow_anal_Rexp(env,_tmp470,
_tmp440)).f1;_tmp471=Cyc_CfFlowInfo_drop_unique_rvals(_tmp440->loc,_tmp471);Cyc_NewControlFlow_update_flow(
env,_tmp43E,_tmp471);outflow=_tmp46E;goto _LL2CC;}}}_LL2DF: if(*((int*)_tmp42A)!= 
5)goto _LL2E1;_tmp443=((struct Cyc_Absyn_Break_s_struct*)_tmp42A)->f1;if(_tmp443 != 
0)goto _LL2E1;_LL2E0: return(void*)0;_LL2E1: if(*((int*)_tmp42A)!= 10)goto _LL2E3;
_tmp444=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp42A)->f1;_tmp445=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp42A)->f2;if(_tmp445 == 0)goto _LL2E3;_tmp446=*_tmp445;_LL2E2: {void*_tmp473;
struct Cyc_List_List*_tmp474;struct _tuple7 _tmp472=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp444,0);_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;for(0;_tmp474 != 0;(
_tmp474=_tmp474->tl,_tmp444=_tmp444->tl)){_tmp473=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp444))->hd)->loc,
_tmp473,(void*)_tmp474->hd);}_tmp473=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,
_tmp473);_tmp473=Cyc_NewControlFlow_add_vars(_tmp473,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp446->pat_vars))->v,Cyc_CfFlowInfo_unknown_all,
s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp446->body,
_tmp473);return(void*)0;}_LL2E3: if(*((int*)_tmp42A)!= 5)goto _LL2E5;_tmp447=((
struct Cyc_Absyn_Break_s_struct*)_tmp42A)->f1;_LL2E4: _tmp448=_tmp447;goto _LL2E6;
_LL2E5: if(*((int*)_tmp42A)!= 6)goto _LL2E7;_tmp448=((struct Cyc_Absyn_Continue_s_struct*)
_tmp42A)->f1;_LL2E6: _tmp449=_tmp448;goto _LL2E8;_LL2E7: if(*((int*)_tmp42A)!= 7)
goto _LL2E9;_tmp449=((struct Cyc_Absyn_Goto_s_struct*)_tmp42A)->f2;_LL2E8: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp475=_tmp429->encloser;struct Cyc_Absyn_Stmt*
_tmp476=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp449)))->encloser;while(_tmp476 != _tmp475){struct Cyc_Absyn_Stmt*_tmp477=(Cyc_NewControlFlow_get_stmt_annot(
_tmp475))->encloser;if(_tmp477 == _tmp475){({void*_tmp478[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp479="goto enters local scope or exception handler";_tag_dynforward(
_tmp479,sizeof(char),_get_zero_arr_size(_tmp479,45));}),_tag_dynforward(_tmp478,
sizeof(void*),0));});break;}_tmp475=_tmp477;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp449),inflow);return(void*)0;_LL2E9: if(*((
int*)_tmp42A)!= 9)goto _LL2EB;_tmp44A=((struct Cyc_Absyn_Switch_s_struct*)_tmp42A)->f1;
_tmp44B=((struct Cyc_Absyn_Switch_s_struct*)_tmp42A)->f2;_LL2EA: {void*_tmp47B;
void*_tmp47C;struct _tuple0 _tmp47A=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp44A);_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;_tmp47B=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp44A->loc,_tmp47B);_tmp47B=Cyc_NewControlFlow_use_Rval(env,_tmp44A->loc,
_tmp47B,_tmp47C);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp47B,_tmp44B);goto
_LL2CC;}_LL2EB: if(*((int*)_tmp42A)!= 14)goto _LL2ED;_tmp44C=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp42A)->f1;_tmp44D=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp42A)->f2;_LL2EC: {
int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;env->in_try=1;(
void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp44C);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp44D);{
void*_tmp47D=scs_outflow;_LL2F8: if((int)_tmp47D != 0)goto _LL2FA;_LL2F9: goto _LL2F7;
_LL2FA:;_LL2FB:({void*_tmp47E[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp47F="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp47F,sizeof(char),_get_zero_arr_size(_tmp47F,42));}),
_tag_dynforward(_tmp47E,sizeof(void*),0));});_LL2F7:;}outflow=s1_outflow;goto
_LL2CC;}}}_LL2ED: if(*((int*)_tmp42A)!= 11)goto _LL2EF;_tmp44E=((struct Cyc_Absyn_Decl_s_struct*)
_tmp42A)->f1;_tmp44F=((struct Cyc_Absyn_Decl_s_struct*)_tmp42A)->f2;_LL2EE:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp44E),_tmp44F);goto _LL2CC;_LL2EF: if(*((int*)_tmp42A)!= 12)goto _LL2F1;_tmp450=((
struct Cyc_Absyn_Label_s_struct*)_tmp42A)->f2;_LL2F0: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp450);goto _LL2CC;_LL2F1: if(*((int*)_tmp42A)!= 15)goto _LL2F3;
_tmp451=((struct Cyc_Absyn_Region_s_struct*)_tmp42A)->f1;_tmp452=((struct Cyc_Absyn_Region_s_struct*)
_tmp42A)->f2;_tmp453=((struct Cyc_Absyn_Region_s_struct*)_tmp42A)->f3;_tmp454=((
struct Cyc_Absyn_Region_s_struct*)_tmp42A)->f4;_LL2F2: {struct Cyc_List_List
_tmp480=({struct Cyc_List_List _tmp481;_tmp481.hd=_tmp452;_tmp481.tl=0;_tmp481;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp480,Cyc_CfFlowInfo_unknown_all,
_tmp454->loc);outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp454);goto
_LL2CC;}_LL2F3: if(*((int*)_tmp42A)!= 16)goto _LL2F5;_tmp455=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp42A)->f1;_LL2F4: {void*_tmp483;void*_tmp484;struct _tuple0 _tmp482=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp455);_tmp483=_tmp482.f1;_tmp484=_tmp482.f2;_tmp483=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp455->loc,_tmp483);{void*_tmp485=Cyc_NewControlFlow_use_Rval(env,_tmp455->loc,
_tmp483,_tmp484);{void*_tmp486=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp455->topt))->v);void*_tmp487;_LL2FD: if(_tmp486 <= (void*)4)goto
_LL2FF;if(*((int*)_tmp486)!= 15)goto _LL2FF;_tmp487=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp486)->f1;_LL2FE: outflow=Cyc_CfFlowInfo_kill_flow_region(_tmp483,_tmp487);
goto _LL2FC;_LL2FF:;_LL300:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488[0]=({struct Cyc_Core_Impossible_struct
_tmp489;_tmp489.tag=Cyc_Core_Impossible;_tmp489.f1=({const char*_tmp48A="anal_stmt -- reset_region";
_tag_dynforward(_tmp48A,sizeof(char),_get_zero_arr_size(_tmp48A,26));});_tmp489;});
_tmp488;}));_LL2FC:;}goto _LL2CC;}}_LL2F5:;_LL2F6:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B[0]=({struct Cyc_Core_Impossible_struct
_tmp48C;_tmp48C.tag=Cyc_Core_Impossible;_tmp48C.f1=({const char*_tmp48D="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp48D,sizeof(char),_get_zero_arr_size(_tmp48D,56));});_tmp48C;});
_tmp48B;}));_LL2CC:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
void*_tmp48E=outflow;struct Cyc_CfFlowInfo_ConsumeInfo _tmp48F;_LL302: if(_tmp48E <= (
void*)1)goto _LL304;if(*((int*)_tmp48E)!= 0)goto _LL304;_tmp48F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp48E)->f3;_LL303: goto _LL301;_LL304:;_LL305: goto _LL301;_LL301:;}return outflow;}
static void Cyc_NewControlFlow_check_nested_fun(void*inflow,struct Cyc_Absyn_Fndecl*
fd);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,
void*inflow,struct Cyc_Absyn_Decl*decl){void*_tmp490=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp491;struct Cyc_Core_Opt*_tmp492;struct Cyc_Core_Opt _tmp493;struct Cyc_List_List*
_tmp494;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_List_List*_tmp496;struct Cyc_Absyn_Fndecl*
_tmp497;_LL307: if(_tmp490 <= (void*)2)goto _LL30F;if(*((int*)_tmp490)!= 0)goto
_LL309;_tmp491=((struct Cyc_Absyn_Var_d_struct*)_tmp490)->f1;_LL308: {struct Cyc_List_List
_tmp498=({struct Cyc_List_List _tmp4A7;_tmp4A7.hd=_tmp491;_tmp4A7.tl=0;_tmp4A7;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp498,Cyc_CfFlowInfo_unknown_none,
decl->loc);{struct Cyc_Absyn_Exp*_tmp499=_tmp491->initializer;if(_tmp499 == 0)
return inflow;{void*_tmp49B;void*_tmp49C;struct _tuple0 _tmp49A=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_tmp499);_tmp49B=_tmp49A.f1;_tmp49C=_tmp49A.f2;
_tmp49B=Cyc_CfFlowInfo_consume_unique_rvals(_tmp499->loc,_tmp49B);{void*_tmp49D=
_tmp49B;struct Cyc_Dict_Dict*_tmp49E;struct Cyc_List_List*_tmp49F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4A0;_LL312: if((int)_tmp49D != 0)goto _LL314;_LL313: return(void*)0;_LL314: if(
_tmp49D <= (void*)1)goto _LL311;if(*((int*)_tmp49D)!= 0)goto _LL311;_tmp49E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp49D)->f1;_tmp49F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp49D)->f2;_tmp4A0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp49D)->f3;
_LL315: _tmp49E=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp49E,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp4A2=_cycalloc(sizeof(*
_tmp4A2));_tmp4A2[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp4A3;_tmp4A3.tag=0;
_tmp4A3.f1=_tmp491;_tmp4A3;});_tmp4A2;}));_tmp4A1->fields=0;_tmp4A1;}),_tmp49C);
_tmp49F=Cyc_CfFlowInfo_reln_assign_var(_tmp49F,_tmp491,(struct Cyc_Absyn_Exp*)
_check_null(_tmp491->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp4A4=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp4A5=_cycalloc(sizeof(*
_tmp4A5));_tmp4A5[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp4A6;_tmp4A6.tag=
0;_tmp4A6.f1=_tmp49E;_tmp4A6.f2=_tmp49F;_tmp4A6.f3=_tmp4A0;_tmp4A6;});_tmp4A5;});
Cyc_NewControlFlow_update_tryflow(env,(void*)_tmp4A4);return(void*)_tmp4A4;}
_LL311:;}}}}_LL309: if(*((int*)_tmp490)!= 2)goto _LL30B;_tmp492=((struct Cyc_Absyn_Let_d_struct*)
_tmp490)->f2;if(_tmp492 == 0)goto _LL30B;_tmp493=*_tmp492;_tmp494=(struct Cyc_List_List*)
_tmp493.v;_tmp495=((struct Cyc_Absyn_Let_d_struct*)_tmp490)->f3;_LL30A: {void*
_tmp4A9;void*_tmp4AA;struct _tuple0 _tmp4A8=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp495);_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;_tmp4A9=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp495->loc,_tmp4A9);_tmp4A9=Cyc_NewControlFlow_use_Rval(env,_tmp495->loc,
_tmp4A9,_tmp4AA);return Cyc_NewControlFlow_add_vars(_tmp4A9,_tmp494,Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL30B: if(*((int*)_tmp490)!= 3)goto _LL30D;_tmp496=((struct Cyc_Absyn_Letv_d_struct*)
_tmp490)->f1;_LL30C: return Cyc_NewControlFlow_add_vars(inflow,_tmp496,Cyc_CfFlowInfo_unknown_none,
decl->loc);_LL30D: if(*((int*)_tmp490)!= 1)goto _LL30F;_tmp497=((struct Cyc_Absyn_Fn_d_struct*)
_tmp490)->f1;_LL30E: Cyc_NewControlFlow_check_nested_fun(inflow,_tmp497);{void*
_tmp4AB=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp497->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp4AC=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp497->fn_vardecl);
return Cyc_NewControlFlow_add_vars(inflow,({struct Cyc_List_List*_tmp4AD=_cycalloc(
sizeof(*_tmp4AD));_tmp4AD->hd=_tmp4AC;_tmp4AD->tl=0;_tmp4AD;}),Cyc_CfFlowInfo_unknown_all,
decl->loc);}_LL30F:;_LL310:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE[0]=({struct Cyc_Core_Impossible_struct
_tmp4AF;_tmp4AF.tag=Cyc_Core_Impossible;_tmp4AF.f1=({const char*_tmp4B0="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp4B0,sizeof(char),_get_zero_arr_size(_tmp4B0,35));});_tmp4AF;});
_tmp4AE;}));_LL306:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp4B1=(void*)s->r;
struct _tuple3 _tmp4B2;struct Cyc_Absyn_Stmt*_tmp4B3;struct _tuple3 _tmp4B4;struct Cyc_Absyn_Stmt*
_tmp4B5;struct Cyc_Absyn_Stmt*_tmp4B6;struct Cyc_Absyn_Stmt*_tmp4B7;struct Cyc_Absyn_Stmt*
_tmp4B8;struct _tuple3 _tmp4B9;struct Cyc_Absyn_Stmt*_tmp4BA;struct Cyc_Absyn_Stmt*
_tmp4BB;struct Cyc_Absyn_Stmt*_tmp4BC;struct _tuple3 _tmp4BD;struct Cyc_Absyn_Stmt*
_tmp4BE;struct Cyc_Absyn_Stmt*_tmp4BF;struct Cyc_Absyn_Stmt*_tmp4C0;struct Cyc_Absyn_Stmt*
_tmp4C1;struct Cyc_Absyn_Stmt*_tmp4C2;struct Cyc_Absyn_Stmt*_tmp4C3;struct Cyc_Absyn_Stmt*
_tmp4C4;struct Cyc_List_List*_tmp4C5;struct Cyc_List_List*_tmp4C6;_LL317: if(
_tmp4B1 <= (void*)1)goto _LL32B;if(*((int*)_tmp4B1)!= 8)goto _LL319;_tmp4B2=((
struct Cyc_Absyn_For_s_struct*)_tmp4B1)->f2;_tmp4B3=_tmp4B2.f2;_tmp4B4=((struct
Cyc_Absyn_For_s_struct*)_tmp4B1)->f3;_tmp4B5=_tmp4B4.f2;_tmp4B6=((struct Cyc_Absyn_For_s_struct*)
_tmp4B1)->f4;_LL318: Cyc_NewControlFlow_postpass(_tmp4B5);_tmp4B7=_tmp4B6;_tmp4B8=
_tmp4B3;goto _LL31A;_LL319: if(*((int*)_tmp4B1)!= 1)goto _LL31B;_tmp4B7=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4B1)->f1;_tmp4B8=((struct Cyc_Absyn_Seq_s_struct*)_tmp4B1)->f2;_LL31A: _tmp4BA=
_tmp4B7;_tmp4BB=_tmp4B8;goto _LL31C;_LL31B: if(*((int*)_tmp4B1)!= 4)goto _LL31D;
_tmp4B9=((struct Cyc_Absyn_While_s_struct*)_tmp4B1)->f1;_tmp4BA=_tmp4B9.f2;
_tmp4BB=((struct Cyc_Absyn_While_s_struct*)_tmp4B1)->f2;_LL31C: _tmp4BC=_tmp4BB;
_tmp4BE=_tmp4BA;goto _LL31E;_LL31D: if(*((int*)_tmp4B1)!= 13)goto _LL31F;_tmp4BC=((
struct Cyc_Absyn_Do_s_struct*)_tmp4B1)->f1;_tmp4BD=((struct Cyc_Absyn_Do_s_struct*)
_tmp4B1)->f2;_tmp4BE=_tmp4BD.f2;_LL31E: _tmp4BF=_tmp4BC;_tmp4C0=_tmp4BE;goto
_LL320;_LL31F: if(*((int*)_tmp4B1)!= 3)goto _LL321;_tmp4BF=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4B1)->f2;_tmp4C0=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4B1)->f3;_LL320:
Cyc_NewControlFlow_postpass(_tmp4BF);_tmp4C1=_tmp4C0;goto _LL322;_LL321: if(*((int*)
_tmp4B1)!= 15)goto _LL323;_tmp4C1=((struct Cyc_Absyn_Region_s_struct*)_tmp4B1)->f4;
_LL322: _tmp4C2=_tmp4C1;goto _LL324;_LL323: if(*((int*)_tmp4B1)!= 12)goto _LL325;
_tmp4C2=((struct Cyc_Absyn_Label_s_struct*)_tmp4B1)->f2;_LL324: _tmp4C3=_tmp4C2;
goto _LL326;_LL325: if(*((int*)_tmp4B1)!= 11)goto _LL327;_tmp4C3=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4B1)->f2;_LL326: Cyc_NewControlFlow_postpass(_tmp4C3);return;_LL327: if(*((int*)
_tmp4B1)!= 14)goto _LL329;_tmp4C4=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4B1)->f1;
_tmp4C5=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4B1)->f2;_LL328: Cyc_NewControlFlow_postpass(
_tmp4C4);_tmp4C6=_tmp4C5;goto _LL32A;_LL329: if(*((int*)_tmp4B1)!= 9)goto _LL32B;
_tmp4C6=((struct Cyc_Absyn_Switch_s_struct*)_tmp4B1)->f2;_LL32A: for(0;_tmp4C6 != 0;
_tmp4C6=_tmp4C6->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp4C6->hd)->body);}return;_LL32B:;_LL32C: return;_LL316:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){Cyc_NewControlFlow_check_nested_fun((void*)({struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));_tmp4C7[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp4C8;_tmp4C8.tag=0;_tmp4C8.f1=Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp);_tmp4C8.f2=0;_tmp4C8.f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4C9;_tmp4C9.consumed=Cyc_CfFlowInfo_mt_place_set();_tmp4C9.may_consume=0;
_tmp4C9;});_tmp4C8;});_tmp4C7;}),fd);}static void Cyc_NewControlFlow_check_nested_fun(
void*inflow,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Position_Segment*_tmp4CA=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,Cyc_CfFlowInfo_unknown_all,_tmp4CA);{struct
Cyc_List_List*_tmp4CB=0;{void*_tmp4CC=inflow;struct Cyc_Dict_Dict*_tmp4CD;struct
Cyc_List_List*_tmp4CE;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4CF;_LL32E: if((int)
_tmp4CC != 0)goto _LL330;_LL32F:({void*_tmp4D0[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4D1="check_fun";_tag_dynforward(_tmp4D1,sizeof(char),_get_zero_arr_size(
_tmp4D1,10));}),_tag_dynforward(_tmp4D0,sizeof(void*),0));});_LL330: if(_tmp4CC <= (
void*)1)goto _LL32D;if(*((int*)_tmp4CC)!= 0)goto _LL32D;_tmp4CD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp4CC)->f1;_tmp4CE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4CC)->f2;
_tmp4CF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4CC)->f3;_LL331: {struct
Cyc_List_List*atts;{void*_tmp4D2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp4D3;struct Cyc_List_List*
_tmp4D4;_LL333: if(_tmp4D2 <= (void*)4)goto _LL335;if(*((int*)_tmp4D2)!= 8)goto
_LL335;_tmp4D3=((struct Cyc_Absyn_FnType_struct*)_tmp4D2)->f1;_tmp4D4=_tmp4D3.attributes;
_LL334: atts=_tmp4D4;goto _LL332;_LL335:;_LL336:({void*_tmp4D5[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4D6="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp4D6,sizeof(char),_get_zero_arr_size(_tmp4D6,50));}),_tag_dynforward(_tmp4D5,
sizeof(void*),0));});_LL332:;}for(0;atts != 0;atts=atts->tl){void*_tmp4D7=(void*)
atts->hd;int _tmp4D8;_LL338: if(_tmp4D7 <= (void*)17)goto _LL33A;if(*((int*)_tmp4D7)
!= 4)goto _LL33A;_tmp4D8=((struct Cyc_Absyn_Initializes_att_struct*)_tmp4D7)->f1;
_LL339: {unsigned int j=(unsigned int)_tmp4D8;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp4D9[0]={};Cyc_Tcutil_terr(_tmp4CA,({const char*_tmp4DA="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp4DA,sizeof(char),_get_zero_arr_size(_tmp4DA,51));}),
_tag_dynforward(_tmp4D9,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp4DB=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp4DC=Cyc_Tcutil_compress((void*)_tmp4DB->type);struct Cyc_Absyn_PtrInfo
_tmp4DD;void*_tmp4DE;struct Cyc_Absyn_PtrAtts _tmp4DF;struct Cyc_Absyn_Conref*
_tmp4E0;struct Cyc_Absyn_Conref*_tmp4E1;struct Cyc_Absyn_Conref*_tmp4E2;_LL33D: if(
_tmp4DC <= (void*)4)goto _LL33F;if(*((int*)_tmp4DC)!= 4)goto _LL33F;_tmp4DD=((
struct Cyc_Absyn_PointerType_struct*)_tmp4DC)->f1;_tmp4DE=(void*)_tmp4DD.elt_typ;
_tmp4DF=_tmp4DD.ptr_atts;_tmp4E0=_tmp4DF.nullable;_tmp4E1=_tmp4DF.bounds;_tmp4E2=
_tmp4DF.zero_term;_LL33E: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp4E0))({void*_tmp4E3[0]={};Cyc_Tcutil_terr(_tmp4CA,({const char*_tmp4E4="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp4E4,sizeof(char),_get_zero_arr_size(_tmp4E4,55));}),
_tag_dynforward(_tmp4E3,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp4E1))({
void*_tmp4E5[0]={};Cyc_Tcutil_terr(_tmp4CA,({const char*_tmp4E6="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp4E6,sizeof(char),_get_zero_arr_size(_tmp4E6,57));}),
_tag_dynforward(_tmp4E5,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp4E2))({void*_tmp4E7[0]={};Cyc_Tcutil_terr(_tmp4CA,({
const char*_tmp4E8="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp4E8,sizeof(char),_get_zero_arr_size(_tmp4E8,77));}),
_tag_dynforward(_tmp4E7,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp4E9=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp4F1=_cycalloc(sizeof(*
_tmp4F1));_tmp4F1[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp4F2;_tmp4F2.tag=
2;_tmp4F2.f1=(int)j;_tmp4F2.f2=(void*)_tmp4DE;_tmp4F2;});_tmp4F1;});struct Cyc_CfFlowInfo_Place*
_tmp4EA=({struct Cyc_CfFlowInfo_Place*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->root=(
void*)((void*)_tmp4E9);_tmp4F0->fields=0;_tmp4F0;});_tmp4CD=Cyc_Dict_insert(
_tmp4CD,(void*)_tmp4E9,Cyc_CfFlowInfo_typ_to_absrval(_tmp4DE,Cyc_CfFlowInfo_esc_none));
_tmp4CD=Cyc_Dict_insert(_tmp4CD,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp4EC;_tmp4EC.tag=0;_tmp4EC.f1=_tmp4DB;_tmp4EC;});_tmp4EB;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp4EE;_tmp4EE.tag=2;_tmp4EE.f1=_tmp4EA;
_tmp4EE;});_tmp4ED;}));_tmp4CB=({struct Cyc_List_List*_tmp4EF=_cycalloc(sizeof(*
_tmp4EF));_tmp4EF->hd=_tmp4EA;_tmp4EF->tl=_tmp4CB;_tmp4EF;});goto _LL33C;}_LL33F:;
_LL340:({void*_tmp4F3[0]={};Cyc_Tcutil_terr(_tmp4CA,({const char*_tmp4F4="initializes attribute on non-pointer";
_tag_dynforward(_tmp4F4,sizeof(char),_get_zero_arr_size(_tmp4F4,37));}),
_tag_dynforward(_tmp4F3,sizeof(void*),0));});_LL33C:;}goto _LL337;}}_LL33A:;
_LL33B: goto _LL337;_LL337:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp4F6;_tmp4F6.tag=0;_tmp4F6.f1=_tmp4CD;_tmp4F6.f2=_tmp4CE;_tmp4F6.f3=_tmp4CF;
_tmp4F6;});_tmp4F5;});}_LL32D:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->iterate_again=1;_tmp4FF->iteration_num=
0;_tmp4FF->in_try=0;_tmp4FF->tryflow=(void*)inflow;_tmp4FF->all_changed=0;
_tmp4FF->noreturn=noreturn;_tmp4FF->param_roots=_tmp4CB;_tmp4FF;});void*outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{void*_tmp4F7=
outflow;_LL342: if((int)_tmp4F7 != 0)goto _LL344;_LL343: goto _LL341;_LL344:;_LL345:
Cyc_NewControlFlow_check_init_params(_tmp4CA,env,outflow);if(noreturn)({void*
_tmp4F8[0]={};Cyc_Tcutil_terr(_tmp4CA,({const char*_tmp4F9="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp4F9,sizeof(char),_get_zero_arr_size(_tmp4F9,46));}),
_tag_dynforward(_tmp4F8,sizeof(void*),0));});else{void*_tmp4FA=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL347: if((int)_tmp4FA != 0)goto _LL349;_LL348: goto _LL346;
_LL349: if(_tmp4FA <= (void*)4)goto _LL34B;if(*((int*)_tmp4FA)!= 6)goto _LL34B;
_LL34A: goto _LL34C;_LL34B: if((int)_tmp4FA != 1)goto _LL34D;_LL34C: goto _LL34E;_LL34D:
if(_tmp4FA <= (void*)4)goto _LL34F;if(*((int*)_tmp4FA)!= 5)goto _LL34F;_LL34E:({
void*_tmp4FB[0]={};Cyc_Tcutil_warn(_tmp4CA,({const char*_tmp4FC="function may not return a value";
_tag_dynforward(_tmp4FC,sizeof(char),_get_zero_arr_size(_tmp4FC,32));}),
_tag_dynforward(_tmp4FB,sizeof(void*),0));});goto _LL346;_LL34F:;_LL350:({void*
_tmp4FD[0]={};Cyc_Tcutil_terr(_tmp4CA,({const char*_tmp4FE="function may not return a value";
_tag_dynforward(_tmp4FE,sizeof(char),_get_zero_arr_size(_tmp4FE,32));}),
_tag_dynforward(_tmp4FD,sizeof(void*),0));});goto _LL346;_LL346:;}goto _LL341;
_LL341:;}Cyc_NewControlFlow_postpass(fd->body);}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp500=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp501;struct Cyc_List_List*_tmp502;struct Cyc_List_List*
_tmp503;struct Cyc_List_List*_tmp504;_LL352: if(_tmp500 <= (void*)2)goto _LL35C;if(*((
int*)_tmp500)!= 1)goto _LL354;_tmp501=((struct Cyc_Absyn_Fn_d_struct*)_tmp500)->f1;
_LL353: Cyc_NewControlFlow_check_fun(_tmp501);goto _LL351;_LL354: if(*((int*)
_tmp500)!= 10)goto _LL356;_tmp502=((struct Cyc_Absyn_ExternC_d_struct*)_tmp500)->f1;
_LL355: _tmp503=_tmp502;goto _LL357;_LL356: if(*((int*)_tmp500)!= 9)goto _LL358;
_tmp503=((struct Cyc_Absyn_Using_d_struct*)_tmp500)->f2;_LL357: _tmp504=_tmp503;
goto _LL359;_LL358: if(*((int*)_tmp500)!= 8)goto _LL35A;_tmp504=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp500)->f2;_LL359: Cyc_NewControlFlow_cf_check(_tmp504);goto _LL351;_LL35A: if(*((
int*)_tmp500)!= 11)goto _LL35C;_LL35B: goto _LL351;_LL35C:;_LL35D: goto _LL351;_LL351:;}}
