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
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_memq(struct Cyc_List_List*l,void*x);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_Dict_Dict*Cyc_Dict_delete(struct Cyc_Dict_Dict*,void*);int Cyc_strptrcmp(
struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _dynforward_ptr*f2;}
;struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
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
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*
t);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
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
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(
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
c);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct Cyc_NewControlFlow_CFStmtAnnot{
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
void*inflow,struct Cyc_List_List*vds,void*leafval){void*_tmpC=inflow;struct Cyc_Dict_Dict*
_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_CfFlowInfo_ConsumeInfo _tmpF;_LL6: if((
int)_tmpC != 0)goto _LL8;_LL7: return(void*)0;_LL8: if(_tmpC <= (void*)1)goto _LL5;if(*((
int*)_tmpC)!= 0)goto _LL5;_tmpD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpC)->f1;
_tmpE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpC)->f2;_tmpF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpC)->f3;_LL9: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp10=({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp12;_tmp12.tag=0;_tmp12.f1=(
struct Cyc_Absyn_Vardecl*)vds->hd;_tmp12;});_tmp11;});_tmpD=Cyc_Dict_insert(_tmpD,(
void*)_tmp10,Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,
leafval));}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp13=
_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp14;_tmp14.tag=0;_tmp14.f1=_tmpD;_tmp14.f2=_tmpE;_tmp14.f3=_tmpF;_tmp14;});
_tmp13;});_LL5:;}static void*Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,void*inflow,void*r){void*_tmp15=inflow;struct
Cyc_Dict_Dict*_tmp16;struct Cyc_List_List*_tmp17;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp18;_LLB: if((int)_tmp15 != 0)goto _LLD;_LLC: return(void*)0;_LLD: if(_tmp15 <= (
void*)1)goto _LLA;if(*((int*)_tmp15)!= 0)goto _LLA;_tmp16=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp15)->f1;_tmp17=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp15)->f2;_tmp18=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp15)->f3;_LLE: if(Cyc_CfFlowInfo_initlevel(
_tmp16,r)!= (void*)2)({void*_tmp19[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp1A="expression may not be fully initialized";
_tag_dynforward(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,40));}),
_tag_dynforward(_tmp19,sizeof(void*),0));});{struct Cyc_Dict_Dict*_tmp1B=Cyc_CfFlowInfo_escape_deref(
_tmp16,env->all_changed,r);if(_tmp16 == _tmp1B)return inflow;{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1C=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));
_tmp1D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=
_tmp1B;_tmp1E.f2=_tmp17;_tmp1E.f3=_tmp18;_tmp1E;});_tmp1D;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp1C);return(void*)_tmp1C;}}_LLA:;}struct _tuple6{void*f1;struct Cyc_List_List*
f2;};static struct _tuple6 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({
struct _tuple6 _tmp1F;_tmp1F.f1=inflow;_tmp1F.f2=0;_tmp1F;});if(es->tl == 0){void*
_tmp21;void*_tmp22;struct _tuple0 _tmp20=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp21=_tmp20.f1;_tmp22=_tmp20.f2;return({struct
_tuple6 _tmp23;_tmp23.f1=_tmp21;_tmp23.f2=({struct Cyc_List_List*_tmp24=_cycalloc(
sizeof(*_tmp24));_tmp24->hd=(void*)_tmp22;_tmp24->tl=0;_tmp24;});_tmp23;});}{
struct Cyc_Dict_Dict**outer_all_changed=env->all_changed;struct Cyc_Dict_Dict*
this_all_changed;void*old_inflow;void*outflow;struct Cyc_List_List*rvals;struct
Cyc_CfFlowInfo_ConsumeInfo _tmp26;void*_tmp27;struct _tuple5 _tmp25=Cyc_CfFlowInfo_save_consume_info(
inflow,1);_tmp26=_tmp25.f1;_tmp27=_tmp25.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
sanity_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp3F;_tmp3F.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp3F.may_consume=0;_tmp3F;});int init_sanity=0;do{this_all_changed=Cyc_CfFlowInfo_mt_place_set();
_tmp27=Cyc_CfFlowInfo_restore_consume_info(_tmp27,_tmp26);env->all_changed=({
struct Cyc_Dict_Dict**_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp28;});{void*_tmp2A;void*_tmp2B;struct _tuple0 _tmp29=Cyc_NewControlFlow_anal_Rexp(
env,_tmp27,(struct Cyc_Absyn_Exp*)es->hd);_tmp2A=_tmp29.f1;_tmp2B=_tmp29.f2;
outflow=_tmp2A;rvals=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));
_tmp2C->hd=(void*)_tmp2B;_tmp2C->tl=0;_tmp2C;});this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict**)_check_null(env->all_changed)),0);if(
arg1_unconsumed){void*_tmp2D=outflow;struct Cyc_Dict_Dict*_tmp2E;struct Cyc_List_List*
_tmp2F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30;_LL10: if((int)_tmp2D != 0)goto _LL12;
_LL11: goto _LLF;_LL12: if(_tmp2D <= (void*)1)goto _LLF;if(*((int*)_tmp2D)!= 0)goto
_LLF;_tmp2E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2D)->f1;_tmp2F=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2D)->f2;_tmp30=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2D)->f3;_LL13: outflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp32;_tmp32.tag=0;_tmp32.f1=_tmp2E;_tmp32.f2=_tmp2F;_tmp32.f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp33;_tmp33.consumed=_tmp30.consumed;_tmp33.may_consume=_tmp26.may_consume;
_tmp33;});_tmp32;});_tmp31;});_LLF:;}{struct Cyc_List_List*es2=es->tl;for(0;es2 != 
0;es2=es2->tl){env->all_changed=({struct Cyc_Dict_Dict**_tmp34=_cycalloc(sizeof(*
_tmp34));_tmp34[0]=Cyc_CfFlowInfo_mt_place_set();_tmp34;});{void*_tmp36;void*
_tmp37;struct _tuple0 _tmp35=Cyc_NewControlFlow_anal_Rexp(env,_tmp27,(struct Cyc_Absyn_Exp*)
es2->hd);_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;rvals=({struct Cyc_List_List*_tmp38=
_cycalloc(sizeof(*_tmp38));_tmp38->hd=(void*)_tmp37;_tmp38->tl=rvals;_tmp38;});
outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Dict_Dict**)& this_all_changed,
outflow,_tmp36,this_all_changed,*((struct Cyc_Dict_Dict**)_check_null(env->all_changed)));
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict**)
_check_null(env->all_changed)),0);}}}{void*_tmp39=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3A;_LL15: if((int)_tmp39 != 0)goto _LL17;_LL16: goto _LL14;_LL17: if(_tmp39 <= (
void*)1)goto _LL14;if(*((int*)_tmp39)!= 0)goto _LL14;_tmp3A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp39)->f3;_LL18: if(init_sanity){if(!Cyc_CfFlowInfo_consume_approx(_tmp3A,
sanity_consume))({void*_tmp3B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3C="anal_unordered_exps failed to preserve consumed info";
_tag_dynforward(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,53));}),
_tag_dynforward(_tmp3B,sizeof(void*),0));});}sanity_consume=_tmp3A;init_sanity=1;
_LL14:;}old_inflow=_tmp27;_tmp27=Cyc_CfFlowInfo_join_flow(outer_all_changed,
_tmp27,outflow,1);}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp27,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict**
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp3D;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple6 _tmp3E;_tmp3E.f1=outflow;_tmp3E.f2=Cyc_List_imp_rev(
rvals);_tmp3E;});}}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*es,int
arg1_unconsumed){void*_tmp41;struct Cyc_List_List*_tmp42;struct _tuple6 _tmp40=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,es,arg1_unconsumed);_tmp41=_tmp40.f1;_tmp42=_tmp40.f2;{void*_tmp43=
_tmp41;struct Cyc_Dict_Dict*_tmp44;_LL1A: if(_tmp43 <= (void*)1)goto _LL1C;if(*((int*)
_tmp43)!= 0)goto _LL1C;_tmp44=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp43)->f1;
_LL1B: for(0;_tmp42 != 0;(_tmp42=_tmp42->tl,es=((struct Cyc_List_List*)_check_null(
es))->tl)){if(Cyc_CfFlowInfo_initlevel(_tmp44,(void*)_tmp42->hd)== (void*)0)({
void*_tmp45[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,({const char*_tmp46="expression may not be initialized";
_tag_dynforward(_tmp46,sizeof(char),_get_zero_arr_size(_tmp46,34));}),
_tag_dynforward(_tmp45,sizeof(void*),0));});}goto _LL19;_LL1C: if((int)_tmp43 != 0)
goto _LL19;_LL1D: goto _LL19;_LL19:;}return({struct _tuple0 _tmp47;_tmp47.f1=_tmp41;
_tmp47.f2=Cyc_CfFlowInfo_unknown_all;_tmp47;});}static void*Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,void*outflow,struct Cyc_Absyn_Exp*
e,void*il){void*_tmp48=outflow;struct Cyc_Dict_Dict*_tmp49;struct Cyc_List_List*
_tmp4A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4B;_LL1F: if((int)_tmp48 != 0)goto _LL21;
_LL20: return outflow;_LL21: if(_tmp48 <= (void*)1)goto _LL1E;if(*((int*)_tmp48)!= 0)
goto _LL1E;_tmp49=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp48)->f1;_tmp4A=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp48)->f2;_tmp4B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp48)->f3;_LL22: {void*_tmp4C=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;
struct Cyc_CfFlowInfo_Place*_tmp4D;_LL24: if((int)_tmp4C != 0)goto _LL26;_LL25:
return outflow;_LL26: if(_tmp4C <= (void*)1)goto _LL23;if(*((int*)_tmp4C)!= 0)goto
_LL23;_tmp4D=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp4C)->f1;_LL27: {void*
nzval=il == (void*)2?(void*)1:(void*)2;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp49,env->all_changed,
_tmp4D,nzval);_tmp4F.f2=_tmp4A;_tmp4F.f3=_tmp4B;_tmp4F;});_tmp4E;});}_LL23:;}
_LL1E:;}static struct _tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp50=outflow;
struct Cyc_Dict_Dict*_tmp51;struct Cyc_List_List*_tmp52;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp53;_LL29: if((int)_tmp50 != 0)goto _LL2B;_LL2A: return({struct _tuple0 _tmp54;
_tmp54.f1=outflow;_tmp54.f2=outflow;_tmp54;});_LL2B: if(_tmp50 <= (void*)1)goto
_LL28;if(*((int*)_tmp50)!= 0)goto _LL28;_tmp51=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp50)->f1;_tmp52=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp50)->f2;_tmp53=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp50)->f3;_LL2C: {void*_tmp55=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp56;_LL2E: if((int)_tmp55 != 0)
goto _LL30;_LL2F: return({struct _tuple0 _tmp57;_tmp57.f1=outflow;_tmp57.f2=outflow;
_tmp57;});_LL30: if(_tmp55 <= (void*)1)goto _LL2D;if(*((int*)_tmp55)!= 0)goto _LL2D;
_tmp56=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp55)->f1;_LL31: {void*nzval=il == (
void*)2?(void*)1:(void*)2;return({struct _tuple0 _tmp58;_tmp58.f1=(void*)({struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp5C;_tmp5C.tag=0;_tmp5C.f1=Cyc_CfFlowInfo_assign_place(
e->loc,_tmp51,env->all_changed,_tmp56,nzval);_tmp5C.f2=_tmp52;_tmp5C.f3=_tmp53;
_tmp5C;});_tmp5B;});_tmp58.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp5A;_tmp5A.tag=0;_tmp5A.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp51,env->all_changed,
_tmp56,(void*)0);_tmp5A.f2=_tmp52;_tmp5A.f3=_tmp53;_tmp5A;});_tmp59;});_tmp58;});}
_LL2D:;}_LL28:;}static void*Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,void*flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){void*_tmp5D=flow;
struct Cyc_Dict_Dict*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp60;_LL33: if((int)_tmp5D != 0)goto _LL35;_LL34: return flow;_LL35: if(_tmp5D <= (
void*)1)goto _LL32;if(*((int*)_tmp5D)!= 0)goto _LL32;_tmp5E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp5D)->f1;_tmp5F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp5D)->f2;_tmp60=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp5D)->f3;_LL36: {void*_tmp61=r2;
struct Cyc_List_List*_tmp62;_LL38: if(_tmp61 <= (void*)3)goto _LL3A;if(*((int*)
_tmp61)!= 3)goto _LL3A;_tmp62=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp61)->f1;
_LL39: {void*_tmp63=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp64;_LL3D: if((int)_tmp63 != 0)goto _LL3F;_LL3E: return flow;_LL3F: if(_tmp63 <= (
void*)1)goto _LL3C;if(*((int*)_tmp63)!= 0)goto _LL3C;_tmp64=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp63)->f1;_LL40: {struct Cyc_List_List*new_cl;{void*_tmp65=r1;struct Cyc_List_List*
_tmp66;void*_tmp67;_LL42: if(_tmp65 <= (void*)3)goto _LL46;if(*((int*)_tmp65)!= 3)
goto _LL44;_tmp66=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp65)->f1;_LL43: new_cl=
_tmp66;goto _LL41;_LL44: if(*((int*)_tmp65)!= 0)goto _LL46;_tmp67=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp65)->f1;if((int)_tmp67 != 2)goto _LL46;_LL45:
goto _LL47;_LL46: if((int)_tmp65 != 0)goto _LL48;_LL47: goto _LL49;_LL48: if((int)
_tmp65 != 1)goto _LL4A;_LL49: new_cl=0;goto _LL41;_LL4A:;_LL4B: return flow;_LL41:;}
for(0;_tmp62 != 0;_tmp62=_tmp62->tl){void*new_cmp;{struct _tuple0 _tmp69=({struct
_tuple0 _tmp68;_tmp68.f1=p;_tmp68.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp62->hd)->cmp;
_tmp68;});void*_tmp6A;void*_tmp6B;void*_tmp6C;void*_tmp6D;_LL4D: _tmp6A=_tmp69.f1;
if((int)_tmp6A != 8)goto _LL4F;_LL4E: goto _LL50;_LL4F: _tmp6B=_tmp69.f2;if((int)
_tmp6B != 8)goto _LL51;_LL50: new_cmp=(void*)8;goto _LL4C;_LL51: _tmp6C=_tmp69.f1;if((
int)_tmp6C != 5)goto _LL53;_tmp6D=_tmp69.f2;if((int)_tmp6D != 5)goto _LL53;_LL52:
new_cmp=(void*)5;goto _LL4C;_LL53:;_LL54: new_cmp=(void*)10;goto _LL4C;_LL4C:;}
new_cl=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=({
struct Cyc_CfFlowInfo_TagCmp*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->cmp=(void*)
new_cmp;_tmp6F->bd=(void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp62->hd)->bd);
_tmp6F;});_tmp6E->tl=new_cl;_tmp6E;});}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp71;_tmp71.tag=0;_tmp71.f1=Cyc_CfFlowInfo_assign_place(e1->loc,_tmp5E,env->all_changed,
_tmp64,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmp72=_cycalloc(sizeof(*
_tmp72));_tmp72[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmp73;_tmp73.tag=3;
_tmp73.f1=new_cl;_tmp73;});_tmp72;}));_tmp71.f2=_tmp5F;_tmp71.f3=_tmp60;_tmp71;});
_tmp70;});}_LL3C:;}_LL3A:;_LL3B: return flow;_LL37:;}_LL32:;}static struct _tuple0
Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,
void*f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp74=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp75;void*
_tmp76;struct Cyc_Absyn_PtrAtts _tmp77;struct Cyc_Absyn_Conref*_tmp78;struct Cyc_Absyn_Conref*
_tmp79;_LL56: if(_tmp74 <= (void*)4)goto _LL58;if(*((int*)_tmp74)!= 4)goto _LL58;
_tmp75=((struct Cyc_Absyn_PointerType_struct*)_tmp74)->f1;_tmp76=(void*)_tmp75.elt_typ;
_tmp77=_tmp75.ptr_atts;_tmp78=_tmp77.bounds;_tmp79=_tmp77.zero_term;_LL57: {void*
_tmp7A=f;struct Cyc_Dict_Dict*_tmp7B;struct Cyc_List_List*_tmp7C;_LL5B: if((int)
_tmp7A != 0)goto _LL5D;_LL5C: return({struct _tuple0 _tmp7D;_tmp7D.f1=f;_tmp7D.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp76,Cyc_CfFlowInfo_unknown_all);_tmp7D;});_LL5D: if(_tmp7A <= (void*)1)goto
_LL5A;if(*((int*)_tmp7A)!= 0)goto _LL5A;_tmp7B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp7A)->f1;_tmp7C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp7A)->f2;_LL5E:
if(Cyc_Tcutil_is_bound_one(_tmp78)){void*_tmp7E=r;struct Cyc_CfFlowInfo_Place*
_tmp7F;void*_tmp80;_LL60: if((int)_tmp7E != 1)goto _LL62;_LL61: goto _LL63;_LL62: if((
int)_tmp7E != 2)goto _LL64;_LL63:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp82;_tmp82.tag=Cyc_CfFlowInfo_NotZero;_tmp82.f1=_tmp7C;_tmp82;});_tmp81;})));
goto _LL5F;_LL64: if(_tmp7E <= (void*)3)goto _LL66;if(*((int*)_tmp7E)!= 2)goto _LL66;
_tmp7F=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp7E)->f1;_LL65:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp83=_cycalloc(sizeof(*
_tmp83));_tmp83[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp84;_tmp84.tag=Cyc_CfFlowInfo_NotZero;
_tmp84.f1=_tmp7C;_tmp84;});_tmp83;})));return({struct _tuple0 _tmp85;_tmp85.f1=f;
_tmp85.f2=Cyc_CfFlowInfo_lookup_place(_tmp7B,_tmp7F);_tmp85;});_LL66: if((int)
_tmp7E != 0)goto _LL68;_LL67:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple0 _tmp86;_tmp86.f1=(void*)0;_tmp86.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp76,Cyc_CfFlowInfo_unknown_all);_tmp86;});_LL68: if(_tmp7E <= (void*)3)goto
_LL6A;if(*((int*)_tmp7E)!= 0)goto _LL6A;_tmp80=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp7E)->f1;_LL69: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp80);goto _LL6B;
_LL6A:;_LL6B:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp88;_tmp88.tag=Cyc_CfFlowInfo_UnknownZ;_tmp88.f1=_tmp7C;_tmp88;});_tmp87;})));
_LL5F:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp8A;_tmp8A.tag=Cyc_CfFlowInfo_UnknownZ;_tmp8A.f1=_tmp7C;_tmp8A;});_tmp89;})));}{
void*_tmp8B=Cyc_CfFlowInfo_initlevel(_tmp7B,r);_LL6D: if((int)_tmp8B != 0)goto
_LL6F;_LL6E:({void*_tmp8C[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp8D="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp8D,sizeof(char),_get_zero_arr_size(_tmp8D,46));}),
_tag_dynforward(_tmp8C,sizeof(void*),0));});goto _LL70;_LL6F: if((int)_tmp8B != 2)
goto _LL71;_LL70: return({struct _tuple0 _tmp8E;_tmp8E.f1=f;_tmp8E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp76,Cyc_CfFlowInfo_unknown_all);_tmp8E;});_LL71: if((int)_tmp8B != 1)goto _LL6C;
_LL72: return({struct _tuple0 _tmp8F;_tmp8F.f1=f;_tmp8F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp76,Cyc_CfFlowInfo_unknown_none);_tmp8F;});_LL6C:;}_LL5A:;}_LL58:;_LL59:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp90=_cycalloc(sizeof(*_tmp90));
_tmp90[0]=({struct Cyc_Core_Impossible_struct _tmp91;_tmp91.tag=Cyc_Core_Impossible;
_tmp91.f1=({const char*_tmp92="right deref of non-pointer-type";_tag_dynforward(
_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,32));});_tmp91;});_tmp90;}));_LL55:;}
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp93=(void*)e1->r;
void*_tmp94;struct Cyc_Absyn_Vardecl*_tmp95;void*_tmp96;struct Cyc_Absyn_Vardecl*
_tmp97;void*_tmp98;struct Cyc_Absyn_Vardecl*_tmp99;void*_tmp9A;struct Cyc_Absyn_Vardecl*
_tmp9B;_LL74: if(*((int*)_tmp93)!= 1)goto _LL76;_tmp94=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp93)->f2;if(_tmp94 <= (void*)1)goto _LL76;if(*((int*)_tmp94)!= 4)goto _LL76;
_tmp95=((struct Cyc_Absyn_Pat_b_struct*)_tmp94)->f1;_LL75: _tmp97=_tmp95;goto _LL77;
_LL76: if(*((int*)_tmp93)!= 1)goto _LL78;_tmp96=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp93)->f2;if(_tmp96 <= (void*)1)goto _LL78;if(*((int*)_tmp96)!= 3)goto _LL78;
_tmp97=((struct Cyc_Absyn_Local_b_struct*)_tmp96)->f1;_LL77: _tmp99=_tmp97;goto
_LL79;_LL78: if(*((int*)_tmp93)!= 1)goto _LL7A;_tmp98=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp93)->f2;if(_tmp98 <= (void*)1)goto _LL7A;if(*((int*)_tmp98)!= 2)goto _LL7A;
_tmp99=((struct Cyc_Absyn_Param_b_struct*)_tmp98)->f1;_LL79: _tmp9B=_tmp99;goto
_LL7B;_LL7A: if(*((int*)_tmp93)!= 1)goto _LL7C;_tmp9A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp93)->f2;if(_tmp9A <= (void*)1)goto _LL7C;if(*((int*)_tmp9A)!= 0)goto _LL7C;
_tmp9B=((struct Cyc_Absyn_Global_b_struct*)_tmp9A)->f1;_LL7B: if(!_tmp9B->escapes){
void*_tmp9C=(void*)e2->r;void*_tmp9D;struct Cyc_Absyn_Vardecl*_tmp9E;void*_tmp9F;
struct Cyc_Absyn_Vardecl*_tmpA0;void*_tmpA1;struct Cyc_Absyn_Vardecl*_tmpA2;void*
_tmpA3;struct Cyc_Absyn_Vardecl*_tmpA4;_LL7F: if(*((int*)_tmp9C)!= 1)goto _LL81;
_tmp9D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9C)->f2;if(_tmp9D <= (void*)1)
goto _LL81;if(*((int*)_tmp9D)!= 4)goto _LL81;_tmp9E=((struct Cyc_Absyn_Pat_b_struct*)
_tmp9D)->f1;_LL80: _tmpA0=_tmp9E;goto _LL82;_LL81: if(*((int*)_tmp9C)!= 1)goto _LL83;
_tmp9F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9C)->f2;if(_tmp9F <= (void*)1)
goto _LL83;if(*((int*)_tmp9F)!= 3)goto _LL83;_tmpA0=((struct Cyc_Absyn_Local_b_struct*)
_tmp9F)->f1;_LL82: _tmpA2=_tmpA0;goto _LL84;_LL83: if(*((int*)_tmp9C)!= 1)goto _LL85;
_tmpA1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9C)->f2;if(_tmpA1 <= (void*)1)
goto _LL85;if(*((int*)_tmpA1)!= 2)goto _LL85;_tmpA2=((struct Cyc_Absyn_Param_b_struct*)
_tmpA1)->f1;_LL84: _tmpA4=_tmpA2;goto _LL86;_LL85: if(*((int*)_tmp9C)!= 1)goto _LL87;
_tmpA3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp9C)->f2;if(_tmpA3 <= (void*)1)
goto _LL87;if(*((int*)_tmpA3)!= 0)goto _LL87;_tmpA4=((struct Cyc_Absyn_Global_b_struct*)
_tmpA3)->f1;_LL86: if(!_tmpA4->escapes){int found=0;{struct Cyc_List_List*_tmpA5=
relns;for(0;_tmpA5 != 0;_tmpA5=_tmpA5->tl){struct Cyc_CfFlowInfo_Reln*_tmpA6=(
struct Cyc_CfFlowInfo_Reln*)_tmpA5->hd;if(_tmpA6->vd == _tmpA4){void*_tmpA7=(void*)
_tmpA6->rop;struct Cyc_Absyn_Vardecl*_tmpA8;_LL8A: if(*((int*)_tmpA7)!= 2)goto
_LL8C;_tmpA8=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmpA7)->f1;if(!(_tmpA8 == 
_tmp9B))goto _LL8C;_LL8B: return relns;_LL8C:;_LL8D: continue;_LL89:;}}}if(!found)
return({struct Cyc_List_List*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->hd=({
struct Cyc_CfFlowInfo_Reln*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->vd=_tmpA4;
_tmpAA->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmpAC;_tmpAC.tag=2;_tmpAC.f1=_tmp9B;_tmpAC;});_tmpAB;}));_tmpAA;});_tmpA9->tl=
relns;_tmpA9;});}return relns;_LL87:;_LL88: return relns;_LL7E:;}else{return relns;}
_LL7C:;_LL7D: return relns;_LL73:;}struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*
relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{void*_tmpAD=inflow;struct Cyc_Dict_Dict*
_tmpAE;struct Cyc_List_List*_tmpAF;struct Cyc_CfFlowInfo_ConsumeInfo _tmpB0;_LL8F:
if((int)_tmpAD != 0)goto _LL91;_LL90: return({struct _tuple0 _tmpB1;_tmpB1.f1=(void*)
0;_tmpB1.f2=Cyc_CfFlowInfo_unknown_all;_tmpB1;});_LL91: if(_tmpAD <= (void*)1)goto
_LL8E;if(*((int*)_tmpAD)!= 0)goto _LL8E;_tmpAE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpAD)->f1;_tmpAF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpAD)->f2;_tmpB0=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpAD)->f3;_LL92: d=_tmpAE;relns=_tmpAF;
cinfo=_tmpB0;_LL8E:;}{void*_tmpB2=(void*)e->r;struct Cyc_Absyn_Exp*_tmpB3;void*
_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*
_tmpB7;void*_tmpB8;void*_tmpB9;int _tmpBA;void*_tmpBB;void*_tmpBC;struct Cyc_List_List*
_tmpBD;void*_tmpBE;void*_tmpBF;struct Cyc_Absyn_Vardecl*_tmpC0;void*_tmpC1;struct
Cyc_Absyn_Vardecl*_tmpC2;void*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC4;void*_tmpC5;
struct Cyc_List_List*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;
struct Cyc_Core_Opt*_tmpC9;struct Cyc_Core_Opt _tmpCA;struct Cyc_Absyn_Exp*_tmpCB;
struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Core_Opt*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;
struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;
struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_List_List*_tmpD3;struct Cyc_Absyn_MallocInfo
_tmpD4;int _tmpD5;struct Cyc_Absyn_Exp*_tmpD6;void**_tmpD7;struct Cyc_Absyn_Exp*
_tmpD8;int _tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*
_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct
_dynforward_ptr*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct _dynforward_ptr*_tmpE1;
struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;
struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;
struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;
struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_List_List*_tmpED;
struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Vardecl*
_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;int _tmpF3;struct Cyc_Absyn_Stmt*
_tmpF4;void*_tmpF5;_LL94: if(*((int*)_tmpB2)!= 15)goto _LL96;_tmpB3=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB2)->f2;_tmpB4=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB2)->f4;if((int)
_tmpB4 != 2)goto _LL96;_LL95: {void*_tmpF7;void*_tmpF8;struct _tuple0 _tmpF6=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB3);_tmpF7=_tmpF6.f1;_tmpF8=_tmpF6.f2;{void*_tmpFA;void*_tmpFB;
struct _tuple0 _tmpF9=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmpF7,_tmpB3,
_tmpF8);_tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;return({struct _tuple0 _tmpFC;_tmpFC.f1=
_tmpFA;_tmpFC.f2=_tmpF8;_tmpFC;});}}_LL96: if(*((int*)_tmpB2)!= 15)goto _LL98;
_tmpB5=((struct Cyc_Absyn_Cast_e_struct*)_tmpB2)->f2;_LL97: _tmpB6=_tmpB5;goto
_LL99;_LL98: if(*((int*)_tmpB2)!= 13)goto _LL9A;_tmpB6=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB2)->f1;_LL99: _tmpB7=_tmpB6;goto _LL9B;_LL9A: if(*((int*)_tmpB2)!= 14)goto
_LL9C;_tmpB7=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB2)->f1;_LL9B: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB7);_LL9C: if(*((int*)_tmpB2)!= 0)goto _LL9E;_tmpB8=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmpB2)->f1;if((int)_tmpB8 != 0)goto _LL9E;_LL9D: goto
_LL9F;_LL9E: if(*((int*)_tmpB2)!= 0)goto _LLA0;_tmpB9=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpB2)->f1;if(_tmpB9 <= (void*)1)goto _LLA0;if(*((int*)_tmpB9)!= 2)goto _LLA0;
_tmpBA=((struct Cyc_Absyn_Int_c_struct*)_tmpB9)->f2;if(_tmpBA != 0)goto _LLA0;_LL9F:
return({struct _tuple0 _tmpFD;_tmpFD.f1=inflow;_tmpFD.f2=(void*)0;_tmpFD;});_LLA0:
if(*((int*)_tmpB2)!= 0)goto _LLA2;_tmpBB=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpB2)->f1;if(_tmpBB <= (void*)1)goto _LLA2;if(*((int*)_tmpBB)!= 2)goto _LLA2;
_LLA1: goto _LLA3;_LLA2: if(*((int*)_tmpB2)!= 1)goto _LLA4;_tmpBC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB2)->f2;if(_tmpBC <= (void*)1)goto _LLA4;if(*((int*)_tmpBC)!= 1)goto _LLA4;
_LLA3: return({struct _tuple0 _tmpFE;_tmpFE.f1=inflow;_tmpFE.f2=(void*)1;_tmpFE;});
_LLA4: if(*((int*)_tmpB2)!= 32)goto _LLA6;_tmpBD=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpB2)->f1;if(_tmpBD != 0)goto _LLA6;_LLA5: goto _LLA7;_LLA6: if(*((int*)_tmpB2)!= 0)
goto _LLA8;_LLA7: goto _LLA9;_LLA8: if(*((int*)_tmpB2)!= 19)goto _LLAA;_LLA9: goto
_LLAB;_LLAA: if(*((int*)_tmpB2)!= 18)goto _LLAC;_LLAB: goto _LLAD;_LLAC: if(*((int*)
_tmpB2)!= 20)goto _LLAE;_LLAD: goto _LLAF;_LLAE: if(*((int*)_tmpB2)!= 21)goto _LLB0;
_LLAF: goto _LLB1;_LLB0: if(*((int*)_tmpB2)!= 34)goto _LLB2;_LLB1: goto _LLB3;_LLB2:
if(*((int*)_tmpB2)!= 33)goto _LLB4;_LLB3: return({struct _tuple0 _tmpFF;_tmpFF.f1=
inflow;_tmpFF.f2=Cyc_CfFlowInfo_unknown_all;_tmpFF;});_LLB4: if(*((int*)_tmpB2)!= 
1)goto _LLB6;_tmpBE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB2)->f2;if(_tmpBE
<= (void*)1)goto _LLB6;if(*((int*)_tmpBE)!= 0)goto _LLB6;_LLB5: return({struct
_tuple0 _tmp100;_tmp100.f1=inflow;_tmp100.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp100;});
_LLB6: if(*((int*)_tmpB2)!= 1)goto _LLB8;_tmpBF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB2)->f2;if(_tmpBF <= (void*)1)goto _LLB8;if(*((int*)_tmpBF)!= 2)goto _LLB8;
_tmpC0=((struct Cyc_Absyn_Param_b_struct*)_tmpBF)->f1;_LLB7: _tmpC2=_tmpC0;goto
_LLB9;_LLB8: if(*((int*)_tmpB2)!= 1)goto _LLBA;_tmpC1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB2)->f2;if(_tmpC1 <= (void*)1)goto _LLBA;if(*((int*)_tmpC1)!= 3)goto _LLBA;
_tmpC2=((struct Cyc_Absyn_Local_b_struct*)_tmpC1)->f1;_LLB9: _tmpC4=_tmpC2;goto
_LLBB;_LLBA: if(*((int*)_tmpB2)!= 1)goto _LLBC;_tmpC3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpB2)->f2;if(_tmpC3 <= (void*)1)goto _LLBC;if(*((int*)_tmpC3)!= 4)goto _LLBC;
_tmpC4=((struct Cyc_Absyn_Pat_b_struct*)_tmpC3)->f1;_LLBB: if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)_tmpC4->type)){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp101=({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[
0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp108;_tmp108.tag=0;_tmp108.f1=_tmpC4;
_tmp108;});_tmp107;});struct Cyc_CfFlowInfo_Place*_tmp102=({struct Cyc_CfFlowInfo_Place*
_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->root=(void*)((void*)_tmp101);
_tmp106->fields=0;_tmp106;});cinfo.may_consume=((struct Cyc_List_List*(*)(int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->hd=_tmp102;_tmp103->tl=0;_tmp103;}),
cinfo.may_consume);inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp105;_tmp105.tag=0;_tmp105.f1=d;_tmp105.f2=relns;_tmp105.f3=cinfo;_tmp105;});
_tmp104;});}return({struct _tuple0 _tmp109;_tmp109.f1=inflow;_tmp109.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));
_tmp10A[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp10B;_tmp10B.tag=0;_tmp10B.f1=
_tmpC4;_tmp10B;});_tmp10A;}));_tmp109;});_LLBC: if(*((int*)_tmpB2)!= 3)goto _LLBE;
_tmpC5=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmpB2)->f1;_tmpC6=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB2)->f2;_LLBD: {void*_tmp10D;void*_tmp10E;struct _tuple0 _tmp10C=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmpC6,0);_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;{void*_tmp10F=_tmpC5;
_LLF7: if((int)_tmp10F != 0)goto _LLF9;_LLF8: goto _LLFA;_LLF9: if((int)_tmp10F != 2)
goto _LLFB;_LLFA: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpC6))->hd)->loc,_tmp10D);goto _LLF6;_LLFB:;
_LLFC: _tmp10D=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpC6))->hd)->loc,_tmp10D);goto _LLF6;_LLF6:;}
return({struct _tuple0 _tmp110;_tmp110.f1=_tmp10D;_tmp110.f2=_tmp10E;_tmp110;});}
_LLBE: if(*((int*)_tmpB2)!= 5)goto _LLC0;_tmpC7=((struct Cyc_Absyn_Increment_e_struct*)
_tmpB2)->f1;_LLBF: {void*_tmp113;struct _tuple0 _tmp112=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmp111[1];_tmp111[0]=_tmpC7;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp111,sizeof(struct Cyc_Absyn_Exp*),
1));}),0);_tmp113=_tmp112.f1;Cyc_CfFlowInfo_check_unique_rvals(_tmpC7->loc,
_tmp113);{void*_tmp115;struct _tuple0 _tmp114=Cyc_NewControlFlow_anal_Lexp(env,
_tmp113,_tmpC7);_tmp115=_tmp114.f2;{struct _tuple0 _tmp117=({struct _tuple0 _tmp116;
_tmp116.f1=_tmp115;_tmp116.f2=_tmp113;_tmp116;});void*_tmp118;struct Cyc_CfFlowInfo_Place*
_tmp119;void*_tmp11A;struct Cyc_Dict_Dict*_tmp11B;struct Cyc_List_List*_tmp11C;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp11D;_LLFE: _tmp118=_tmp117.f1;if(_tmp118 <= (
void*)1)goto _LL100;if(*((int*)_tmp118)!= 0)goto _LL100;_tmp119=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp118)->f1;_tmp11A=_tmp117.f2;if(_tmp11A <= (void*)1)goto _LL100;if(*((int*)
_tmp11A)!= 0)goto _LL100;_tmp11B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp11A)->f1;_tmp11C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11A)->f2;
_tmp11D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11A)->f3;_LLFF: _tmp11C=
Cyc_CfFlowInfo_reln_kill_exp(_tmp11C,_tmpC7);_tmp113=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp11F;_tmp11F.tag=0;_tmp11F.f1=Cyc_CfFlowInfo_assign_place(_tmpC7->loc,_tmp11B,
env->all_changed,_tmp119,Cyc_CfFlowInfo_unknown_all);_tmp11F.f2=_tmp11C;_tmp11F.f3=
_tmp11D;_tmp11F;});_tmp11E;});goto _LLFD;_LL100:;_LL101: goto _LLFD;_LLFD:;}return({
struct _tuple0 _tmp120;_tmp120.f1=_tmp113;_tmp120.f2=Cyc_CfFlowInfo_unknown_all;
_tmp120;});}}_LLC0: if(*((int*)_tmpB2)!= 4)goto _LLC2;_tmpC8=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpB2)->f1;_tmpC9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB2)->f2;if(_tmpC9 == 
0)goto _LLC2;_tmpCA=*_tmpC9;_tmpCB=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB2)->f3;
_LLC1: {void*_tmp123;struct _tuple0 _tmp122=Cyc_NewControlFlow_anal_use_ints(env,
inflow,({struct Cyc_Absyn_Exp*_tmp121[2];_tmp121[1]=_tmpCB;_tmp121[0]=_tmpC8;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp121,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp123=_tmp122.f1;{void*_tmp125;
struct _tuple0 _tmp124=Cyc_NewControlFlow_anal_Lexp(env,_tmp123,e);_tmp125=_tmp124.f2;
_tmp123=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp123);{struct _tuple0
_tmp127=({struct _tuple0 _tmp126;_tmp126.f1=_tmp125;_tmp126.f2=_tmp123;_tmp126;});
void*_tmp128;struct Cyc_CfFlowInfo_Place*_tmp129;void*_tmp12A;struct Cyc_Dict_Dict*
_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp12D;
_LL103: _tmp128=_tmp127.f1;if(_tmp128 <= (void*)1)goto _LL105;if(*((int*)_tmp128)!= 
0)goto _LL105;_tmp129=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp128)->f1;_tmp12A=
_tmp127.f2;if(_tmp12A <= (void*)1)goto _LL105;if(*((int*)_tmp12A)!= 0)goto _LL105;
_tmp12B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp12A)->f1;_tmp12C=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp12A)->f2;_tmp12D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp12A)->f3;_LL104: _tmp12C=Cyc_CfFlowInfo_reln_kill_exp(_tmp12C,_tmpC8);_tmp12B=
Cyc_CfFlowInfo_assign_place(_tmpC8->loc,_tmp12B,env->all_changed,_tmp129,Cyc_CfFlowInfo_unknown_all);
if(Cyc_CfFlowInfo_contains_region((void*)0,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpC8->topt))->v)){_tmp12D.may_consume=((struct Cyc_List_List*(*)(
int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({struct Cyc_List_List*
_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->hd=_tmp129;_tmp12E->tl=0;_tmp12E;}),
_tmp12D.may_consume);if(env->all_changed == 0){struct Cyc_Dict_Dict*_tmp12F=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_delete)(
_tmp12D.consumed,_tmp129);if(_tmp12D.consumed != _tmp12F);_tmp12D.consumed=
_tmp12F;}}_tmp123=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp130=
_cycalloc(sizeof(*_tmp130));_tmp130[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp131;_tmp131.tag=0;_tmp131.f1=_tmp12B;_tmp131.f2=_tmp12C;_tmp131.f3=_tmp12D;
_tmp131;});_tmp130;});goto _LL102;_LL105:;_LL106: goto _LL102;_LL102:;}return({
struct _tuple0 _tmp132;_tmp132.f1=_tmp123;_tmp132.f2=Cyc_CfFlowInfo_unknown_all;
_tmp132;});}}_LLC2: if(*((int*)_tmpB2)!= 4)goto _LLC4;_tmpCC=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpB2)->f1;_tmpCD=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB2)->f2;if(_tmpCD != 
0)goto _LLC4;_tmpCE=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB2)->f3;_LLC3: {
struct Cyc_Dict_Dict**_tmp133=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp135;void*_tmp136;struct _tuple5 _tmp134=Cyc_CfFlowInfo_save_consume_info(
inflow,1);_tmp135=_tmp134.f1;_tmp136=_tmp134.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
sanity_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp158;_tmp158.consumed=Cyc_CfFlowInfo_mt_place_set();
_tmp158.may_consume=0;_tmp158;});int init_sanity=0;while(1){env->all_changed=({
struct Cyc_Dict_Dict**_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp137;});{void*_tmp139;void*_tmp13A;struct _tuple0 _tmp138=Cyc_NewControlFlow_anal_Lexp(
env,_tmp136,_tmpCC);_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;{struct Cyc_Dict_Dict*
_tmp13B=*((struct Cyc_Dict_Dict**)_check_null(env->all_changed));env->all_changed=({
struct Cyc_Dict_Dict**_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp13C;});{void*_tmp13E;void*_tmp13F;struct _tuple0 _tmp13D=Cyc_NewControlFlow_anal_Rexp(
env,_tmp136,_tmpCE);_tmp13E=_tmp13D.f1;_tmp13F=_tmp13D.f2;{struct Cyc_Dict_Dict*
_tmp140=*((struct Cyc_Dict_Dict**)_check_null(env->all_changed));void*_tmp141=Cyc_CfFlowInfo_after_flow((
struct Cyc_Dict_Dict**)& _tmp13B,_tmp139,_tmp13E,_tmp13B,_tmp140);void*_tmp142=Cyc_CfFlowInfo_join_flow(
_tmp133,_tmp136,_tmp141,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp142,
_tmp136)){if(_tmp133 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict**)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp133,Cyc_CfFlowInfo_union_place_set(
_tmp13B,_tmp140,0),0);}_tmp141=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp141);{void*_tmp143=_tmp141;struct Cyc_Dict_Dict*_tmp144;struct Cyc_List_List*
_tmp145;struct Cyc_CfFlowInfo_ConsumeInfo _tmp146;_LL108: if((int)_tmp143 != 0)goto
_LL10A;_LL109: return({struct _tuple0 _tmp147;_tmp147.f1=(void*)0;_tmp147.f2=
_tmp13F;_tmp147;});_LL10A: if(_tmp143 <= (void*)1)goto _LL107;if(*((int*)_tmp143)!= 
0)goto _LL107;_tmp144=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp143)->f1;
_tmp145=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp143)->f2;_tmp146=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp143)->f3;_LL10B: _tmp146=Cyc_CfFlowInfo_and_consume(
_tmp135,_tmp146);{void*_tmp148=_tmp13A;struct Cyc_CfFlowInfo_Place*_tmp149;_LL10D:
if(_tmp148 <= (void*)1)goto _LL10F;if(*((int*)_tmp148)!= 0)goto _LL10F;_tmp149=((
struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp148)->f1;_LL10E: _tmp144=Cyc_CfFlowInfo_assign_place(
e->loc,_tmp144,env->all_changed,_tmp149,_tmp13F);if(Cyc_CfFlowInfo_contains_region((
void*)0,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpCC->topt))->v)){_tmp146.may_consume=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->hd=_tmp149;
_tmp14A->tl=0;_tmp14A;}),_tmp146.may_consume);if(env->all_changed == 0){struct Cyc_Dict_Dict*
_tmp14B=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_delete)(_tmp146.consumed,_tmp149);if(_tmp146.consumed != _tmp14B);
_tmp146.consumed=_tmp14B;}}_tmp145=Cyc_CfFlowInfo_reln_assign_exp(_tmp145,_tmpCC,
_tmpCE);_tmp141=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp14C=
_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp14D;_tmp14D.tag=0;_tmp14D.f1=_tmp144;_tmp14D.f2=_tmp145;_tmp14D.f3=_tmp146;
_tmp14D;});_tmp14C;});Cyc_NewControlFlow_update_tryflow(env,_tmp141);return({
struct _tuple0 _tmp14E;_tmp14E.f1=_tmp141;_tmp14E.f2=_tmp13F;_tmp14E;});_LL10F: if((
int)_tmp148 != 0)goto _LL10C;_LL110: if(Cyc_CfFlowInfo_contains_region((void*)0,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpCC->topt))->v))({void*_tmp14F[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp150="dest of unique assignment is unknown!";_tag_dynforward(
_tmp150,sizeof(char),_get_zero_arr_size(_tmp150,38));}),_tag_dynforward(_tmp14F,
sizeof(void*),0));});return({struct _tuple0 _tmp151;_tmp151.f1=Cyc_NewControlFlow_use_Rval(
env,_tmpCE->loc,_tmp141,_tmp13F);_tmp151.f2=_tmp13F;_tmp151;});_LL10C:;}_LL107:;}}
_tmp136=_tmp142;{void*_tmp152=_tmp136;struct Cyc_Dict_Dict*_tmp153;struct Cyc_List_List*
_tmp154;struct Cyc_CfFlowInfo_ConsumeInfo _tmp155;_LL112: if((int)_tmp152 != 0)goto
_LL114;_LL113: goto _LL111;_LL114: if(_tmp152 <= (void*)1)goto _LL111;if(*((int*)
_tmp152)!= 0)goto _LL111;_tmp153=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp152)->f1;_tmp154=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp152)->f2;
_tmp155=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp152)->f3;_LL115: if(
init_sanity){if(!Cyc_CfFlowInfo_consume_approx(_tmp155,sanity_consume))({void*
_tmp156[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp157="assignOp_e failed to preserve consume info";_tag_dynforward(
_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,43));}),_tag_dynforward(_tmp156,
sizeof(void*),0));});}sanity_consume=_tmp155;init_sanity=1;goto _LL111;_LL111:;}
_tmp136=Cyc_CfFlowInfo_restore_consume_info(_tmp136,_tmp135);}}}}}}}_LLC4: if(*((
int*)_tmpB2)!= 9)goto _LLC6;_tmpCF=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB2)->f1;
_tmpD0=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB2)->f2;_LLC5: {void*_tmp15A;void*
_tmp15B;struct _tuple0 _tmp159=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpCF);
_tmp15A=_tmp159.f1;_tmp15B=_tmp159.f2;_tmp15A=Cyc_CfFlowInfo_drop_unique_rvals(
_tmpCF->loc,_tmp15A);return Cyc_NewControlFlow_anal_Rexp(env,_tmp15A,_tmpD0);}
_LLC6: if(*((int*)_tmpB2)!= 12)goto _LLC8;_tmpD1=((struct Cyc_Absyn_Throw_e_struct*)
_tmpB2)->f1;_LLC7: {void*_tmp15D;void*_tmp15E;struct _tuple0 _tmp15C=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpD1);_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;_tmp15D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmpD1->loc,_tmp15D);Cyc_NewControlFlow_use_Rval(env,_tmpD1->loc,_tmp15D,_tmp15E);
return({struct _tuple0 _tmp15F;_tmp15F.f1=(void*)0;_tmp15F.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp15F;});}_LLC8: if(*((int*)_tmpB2)!= 11)goto _LLCA;_tmpD2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpB2)->f1;_tmpD3=((struct Cyc_Absyn_FnCall_e_struct*)_tmpB2)->f2;_LLC9: {void*
_tmp162;struct Cyc_List_List*_tmp163;struct _tuple6 _tmp161=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_List_List*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->hd=
_tmpD2;_tmp160->tl=_tmpD3;_tmp160;}),1);_tmp162=_tmp161.f1;_tmp163=_tmp161.f2;
_tmp162=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp162);{void*_tmp164=Cyc_NewControlFlow_use_Rval(
env,_tmpD2->loc,_tmp162,(void*)((struct Cyc_List_List*)_check_null(_tmp163))->hd);
_tmp163=_tmp163->tl;{struct Cyc_List_List*init_params=0;{void*_tmp165=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD2->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp166;void*_tmp167;_LL117: if(_tmp165 <= (void*)4)goto _LL119;if(*((int*)_tmp165)
!= 4)goto _LL119;_tmp166=((struct Cyc_Absyn_PointerType_struct*)_tmp165)->f1;
_tmp167=(void*)_tmp166.elt_typ;_LL118:{void*_tmp168=Cyc_Tcutil_compress(_tmp167);
struct Cyc_Absyn_FnInfo _tmp169;struct Cyc_List_List*_tmp16A;_LL11C: if(_tmp168 <= (
void*)4)goto _LL11E;if(*((int*)_tmp168)!= 8)goto _LL11E;_tmp169=((struct Cyc_Absyn_FnType_struct*)
_tmp168)->f1;_tmp16A=_tmp169.attributes;_LL11D: for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
void*_tmp16B=(void*)_tmp16A->hd;int _tmp16C;_LL121: if(_tmp16B <= (void*)17)goto
_LL123;if(*((int*)_tmp16B)!= 4)goto _LL123;_tmp16C=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp16B)->f1;_LL122: init_params=({struct Cyc_List_List*_tmp16D=_cycalloc(sizeof(*
_tmp16D));_tmp16D->hd=(void*)_tmp16C;_tmp16D->tl=init_params;_tmp16D;});goto
_LL120;_LL123:;_LL124: goto _LL120;_LL120:;}goto _LL11B;_LL11E:;_LL11F:({void*
_tmp16E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp16F="anal_Rexp: bad function type";_tag_dynforward(_tmp16F,sizeof(
char),_get_zero_arr_size(_tmp16F,29));}),_tag_dynforward(_tmp16E,sizeof(void*),0));});
_LL11B:;}goto _LL116;_LL119:;_LL11A:({void*_tmp170[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp171="anal_Rexp: bad function type";_tag_dynforward(_tmp171,sizeof(char),
_get_zero_arr_size(_tmp171,29));}),_tag_dynforward(_tmp170,sizeof(void*),0));});
_LL116:;}{int i=1;for(0;_tmp163 != 0;(((_tmp163=_tmp163->tl,_tmpD3=((struct Cyc_List_List*)
_check_null(_tmpD3))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i)){_tmp164=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpD3))->hd)->loc,_tmp164,(void*)_tmp163->hd);
continue;}{void*_tmp172=_tmp162;struct Cyc_Dict_Dict*_tmp173;_LL126: if((int)
_tmp172 != 0)goto _LL128;_LL127: goto _LL125;_LL128: if(_tmp172 <= (void*)1)goto _LL125;
if(*((int*)_tmp172)!= 0)goto _LL125;_tmp173=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp172)->f1;_LL129: if(Cyc_CfFlowInfo_initlevel(_tmp173,(void*)_tmp163->hd)== (
void*)0)({void*_tmp174[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmpD3))->hd)->loc,({const char*_tmp175="expression may not be initialized";
_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,34));}),
_tag_dynforward(_tmp174,sizeof(void*),0));});{void*_tmp176=_tmp164;struct Cyc_Dict_Dict*
_tmp177;struct Cyc_List_List*_tmp178;struct Cyc_CfFlowInfo_ConsumeInfo _tmp179;
_LL12B: if((int)_tmp176 != 0)goto _LL12D;_LL12C: goto _LL12A;_LL12D: if(_tmp176 <= (
void*)1)goto _LL12A;if(*((int*)_tmp176)!= 0)goto _LL12A;_tmp177=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp176)->f1;_tmp178=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp176)->f2;
_tmp179=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp176)->f3;_LL12E: {struct
Cyc_Dict_Dict*_tmp17A=Cyc_CfFlowInfo_escape_deref(_tmp177,env->all_changed,(void*)
_tmp163->hd);{void*_tmp17B=(void*)_tmp163->hd;struct Cyc_CfFlowInfo_Place*_tmp17C;
_LL130: if(_tmp17B <= (void*)3)goto _LL132;if(*((int*)_tmp17B)!= 2)goto _LL132;
_tmp17C=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp17B)->f1;_LL131:{void*
_tmp17D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpD3))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp17E;void*_tmp17F;_LL135: if(_tmp17D <= (void*)4)goto _LL137;if(*((int*)_tmp17D)
!= 4)goto _LL137;_tmp17E=((struct Cyc_Absyn_PointerType_struct*)_tmp17D)->f1;
_tmp17F=(void*)_tmp17E.elt_typ;_LL136: _tmp17A=Cyc_CfFlowInfo_assign_place(((
struct Cyc_Absyn_Exp*)_tmpD3->hd)->loc,_tmp17A,env->all_changed,_tmp17C,Cyc_CfFlowInfo_typ_to_absrval(
_tmp17F,Cyc_CfFlowInfo_esc_all));goto _LL134;_LL137:;_LL138:({void*_tmp180[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp181="anal_Rexp: bad type for initialized arg";_tag_dynforward(
_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,40));}),_tag_dynforward(_tmp180,
sizeof(void*),0));});_LL134:;}goto _LL12F;_LL132:;_LL133: goto _LL12F;_LL12F:;}
_tmp164=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp182=_cycalloc(
sizeof(*_tmp182));_tmp182[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp183;
_tmp183.tag=0;_tmp183.f1=_tmp17A;_tmp183.f2=_tmp178;_tmp183.f3=_tmp179;_tmp183;});
_tmp182;});goto _LL12A;}_LL12A:;}goto _LL125;_LL125:;}}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD2->topt))->v))return({struct _tuple0
_tmp184;_tmp184.f1=(void*)0;_tmp184.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp184;});
else{return({struct _tuple0 _tmp185;_tmp185.f1=_tmp164;_tmp185.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp185;});}}}}_LLCA: if(*((int*)_tmpB2)!= 35)goto _LLCC;_tmpD4=((struct Cyc_Absyn_Malloc_e_struct*)
_tmpB2)->f1;_tmpD5=_tmpD4.is_calloc;_tmpD6=_tmpD4.rgn;_tmpD7=_tmpD4.elt_type;
_tmpD8=_tmpD4.num_elts;_tmpD9=_tmpD4.fat_result;_LLCB: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197[
0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp198;_tmp198.tag=1;_tmp198.f1=_tmpD8;
_tmp198.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp198;});_tmp197;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->root=(void*)root;_tmp196->fields=0;
_tmp196;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp194=
_cycalloc(sizeof(*_tmp194));_tmp194[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp195;_tmp195.tag=2;_tmp195.f1=place;_tmp195;});_tmp194;});void*place_val=
_tmpD9?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpD7)),Cyc_CfFlowInfo_unknown_none);
void*outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmpD6
!= 0){void*_tmp188;struct Cyc_List_List*_tmp189;struct _tuple6 _tmp187=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp186[2];_tmp186[1]=_tmpD8;_tmp186[0]=(struct
Cyc_Absyn_Exp*)_tmpD6;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp186,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp188=_tmp187.f1;
_tmp189=_tmp187.f2;outflow=_tmp188;}else{void*_tmp18B;struct _tuple0 _tmp18A=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpD8);_tmp18B=_tmp18A.f1;outflow=_tmp18B;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpD8->loc,outflow);{void*_tmp18C=outflow;struct Cyc_Dict_Dict*_tmp18D;struct Cyc_List_List*
_tmp18E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp18F;_LL13A: if((int)_tmp18C != 0)goto
_LL13C;_LL13B: return({struct _tuple0 _tmp190;_tmp190.f1=outflow;_tmp190.f2=rval;
_tmp190;});_LL13C: if(_tmp18C <= (void*)1)goto _LL139;if(*((int*)_tmp18C)!= 0)goto
_LL139;_tmp18D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp18C)->f1;_tmp18E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp18C)->f2;_tmp18F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp18C)->f3;_LL13D: return({struct _tuple0 _tmp191;_tmp191.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp193;_tmp193.tag=0;_tmp193.f1=Cyc_Dict_insert(_tmp18D,root,place_val);_tmp193.f2=
_tmp18E;_tmp193.f3=_tmp18F;_tmp193;});_tmp192;});_tmp191.f2=rval;_tmp191;});
_LL139:;}}_LLCC: if(*((int*)_tmpB2)!= 17)goto _LLCE;_tmpDA=((struct Cyc_Absyn_New_e_struct*)
_tmpB2)->f1;_tmpDB=((struct Cyc_Absyn_New_e_struct*)_tmpB2)->f2;_LLCD: {void*root=(
void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));
_tmp1AB[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp1AC;_tmp1AC.tag=1;_tmp1AC.f1=
_tmpDB;_tmp1AC.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp1AC;});_tmp1AB;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->root=(void*)root;_tmp1AA->fields=0;
_tmp1AA;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1A8=
_cycalloc(sizeof(*_tmp1A8));_tmp1A8[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1A9;_tmp1A9.tag=2;_tmp1A9.f1=place;_tmp1A9;});_tmp1A8;});Cyc_CfFlowInfo_update_place_set(
env->all_changed,place,0);{void*outflow;void*place_val;if(_tmpDA != 0){void*
_tmp19B;struct Cyc_List_List*_tmp19C;struct _tuple6 _tmp19A=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp199[2];_tmp199[1]=_tmpDB;_tmp199[0]=(struct
Cyc_Absyn_Exp*)_tmpDA;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp199,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp19B=_tmp19A.f1;
_tmp19C=_tmp19A.f2;outflow=_tmp19B;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp19C))->tl))->hd;}else{void*
_tmp19E;void*_tmp19F;struct _tuple0 _tmp19D=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpDB);_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;outflow=_tmp19E;place_val=
_tmp19F;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpDB->loc,outflow);{
void*_tmp1A0=outflow;struct Cyc_Dict_Dict*_tmp1A1;struct Cyc_List_List*_tmp1A2;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A3;_LL13F: if((int)_tmp1A0 != 0)goto _LL141;
_LL140: return({struct _tuple0 _tmp1A4;_tmp1A4.f1=outflow;_tmp1A4.f2=rval;_tmp1A4;});
_LL141: if(_tmp1A0 <= (void*)1)goto _LL13E;if(*((int*)_tmp1A0)!= 0)goto _LL13E;
_tmp1A1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A0)->f1;_tmp1A2=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1A0)->f2;_tmp1A3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1A0)->f3;_LL142: return({struct _tuple0 _tmp1A5;_tmp1A5.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=Cyc_Dict_insert(_tmp1A1,root,place_val);_tmp1A7.f2=
_tmp1A2;_tmp1A7.f3=_tmp1A3;_tmp1A7;});_tmp1A6;});_tmp1A5.f2=rval;_tmp1A5;});
_LL13E:;}}}_LLCE: if(*((int*)_tmpB2)!= 16)goto _LLD0;_tmpDC=((struct Cyc_Absyn_Address_e_struct*)
_tmpB2)->f1;_LLCF: {void*_tmp1AE;void*_tmp1AF;struct _tuple0 _tmp1AD=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmpDC);_tmp1AE=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;{void*_tmp1B0=_tmp1AF;
struct Cyc_CfFlowInfo_Place*_tmp1B1;_LL144: if((int)_tmp1B0 != 0)goto _LL146;_LL145:
return({struct _tuple0 _tmp1B2;_tmp1B2.f1=_tmp1AE;_tmp1B2.f2=(void*)1;_tmp1B2;});
_LL146: if(_tmp1B0 <= (void*)1)goto _LL143;if(*((int*)_tmp1B0)!= 0)goto _LL143;
_tmp1B1=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp1B0)->f1;_LL147: return({struct
_tuple0 _tmp1B3;_tmp1B3.f1=_tmp1AE;_tmp1B3.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp1B5;_tmp1B5.tag=2;_tmp1B5.f1=_tmp1B1;_tmp1B5;});_tmp1B4;});_tmp1B3;});_LL143:;}}
_LLD0: if(*((int*)_tmpB2)!= 22)goto _LLD2;_tmpDD=((struct Cyc_Absyn_Deref_e_struct*)
_tmpB2)->f1;_LLD1: {void*_tmp1B7;void*_tmp1B8;struct _tuple0 _tmp1B6=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpDD);_tmp1B7=_tmp1B6.f1;_tmp1B8=_tmp1B6.f2;_tmp1B7=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1B7);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1B7,_tmpDD,
_tmp1B8);}_LLD2: if(*((int*)_tmpB2)!= 23)goto _LLD4;_tmpDE=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpB2)->f1;_tmpDF=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpB2)->f2;_LLD3: {
void*_tmp1BA;void*_tmp1BB;struct _tuple0 _tmp1B9=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpDE);_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDE->topt))->v))return({struct _tuple0
_tmp1BC;_tmp1BC.f1=_tmp1BA;_tmp1BC.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp1BC;});{
void*_tmp1BD=_tmp1BB;struct Cyc_Dict_Dict*_tmp1BE;_LL149: if(_tmp1BD <= (void*)3)
goto _LL14B;if(*((int*)_tmp1BD)!= 4)goto _LL14B;_tmp1BE=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1BD)->f1;_LL14A: return({struct _tuple0 _tmp1BF;_tmp1BF.f1=_tmp1BA;_tmp1BF.f2=((
void*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp1BE,
_tmpDF);_tmp1BF;});_LL14B:;_LL14C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0[0]=({struct Cyc_Core_Impossible_struct
_tmp1C1;_tmp1C1.tag=Cyc_Core_Impossible;_tmp1C1.f1=({const char*_tmp1C2="anal_Rexp: AggrMember";
_tag_dynforward(_tmp1C2,sizeof(char),_get_zero_arr_size(_tmp1C2,22));});_tmp1C1;});
_tmp1C0;}));_LL148:;}}_LLD4: if(*((int*)_tmpB2)!= 24)goto _LLD6;_tmpE0=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpB2)->f1;_tmpE1=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpB2)->f2;_LLD5: {
void*_tmp1C4;void*_tmp1C5;struct _tuple0 _tmp1C3=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpE0);_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;_tmp1C4=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp1C4);{void*_tmp1C7;void*_tmp1C8;struct _tuple0 _tmp1C6=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp1C4,_tmpE0,_tmp1C5);_tmp1C7=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;{void*
_tmp1C9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpE0->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1CA;void*_tmp1CB;_LL14E: if(_tmp1C9 <= (void*)4)goto
_LL150;if(*((int*)_tmp1C9)!= 4)goto _LL150;_tmp1CA=((struct Cyc_Absyn_PointerType_struct*)
_tmp1C9)->f1;_tmp1CB=(void*)_tmp1CA.elt_typ;_LL14F: if(Cyc_Absyn_is_union_type(
_tmp1CB))return({struct _tuple0 _tmp1CC;_tmp1CC.f1=_tmp1C7;_tmp1CC.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1CC;});goto _LL14D;_LL150:;_LL151:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD[0]=({struct Cyc_Core_Impossible_struct
_tmp1CE;_tmp1CE.tag=Cyc_Core_Impossible;_tmp1CE.f1=({const char*_tmp1CF="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp1CF,sizeof(char),_get_zero_arr_size(_tmp1CF,25));});_tmp1CE;});
_tmp1CD;}));_LL14D:;}{void*_tmp1D0=_tmp1C8;struct Cyc_Dict_Dict*_tmp1D1;_LL153:
if(_tmp1D0 <= (void*)3)goto _LL155;if(*((int*)_tmp1D0)!= 4)goto _LL155;_tmp1D1=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1D0)->f1;_LL154: return({struct _tuple0
_tmp1D2;_tmp1D2.f1=_tmp1C7;_tmp1D2.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp1D1,_tmpE1);_tmp1D2;});_LL155:;_LL156:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1D3=_cycalloc(sizeof(*
_tmp1D3));_tmp1D3[0]=({struct Cyc_Core_Impossible_struct _tmp1D4;_tmp1D4.tag=Cyc_Core_Impossible;
_tmp1D4.f1=({const char*_tmp1D5="anal_Rexp: AggrArrow";_tag_dynforward(_tmp1D5,
sizeof(char),_get_zero_arr_size(_tmp1D5,21));});_tmp1D4;});_tmp1D3;}));_LL152:;}}}
_LLD6: if(*((int*)_tmpB2)!= 6)goto _LLD8;_tmpE2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB2)->f1;_tmpE3=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB2)->f2;_tmpE4=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpB2)->f3;_LLD7: {void*_tmp1D7;void*
_tmp1D8;struct _tuple0 _tmp1D6=Cyc_NewControlFlow_anal_test(env,inflow,_tmpE2);
_tmp1D7=_tmp1D6.f1;_tmp1D8=_tmp1D6.f2;_tmp1D7=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpE2->loc,_tmp1D7);_tmp1D8=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpE2->loc,
_tmp1D8);{struct _tuple0 _tmp1D9=Cyc_NewControlFlow_anal_Rexp(env,_tmp1D7,_tmpE3);
struct _tuple0 _tmp1DA=Cyc_NewControlFlow_anal_Rexp(env,_tmp1D8,_tmpE4);return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1D9,_tmp1DA,1);}}_LLD8: if(*((int*)_tmpB2)!= 7)goto _LLDA;
_tmpE5=((struct Cyc_Absyn_And_e_struct*)_tmpB2)->f1;_tmpE6=((struct Cyc_Absyn_And_e_struct*)
_tmpB2)->f2;_LLD9: {void*_tmp1DC;void*_tmp1DD;struct _tuple0 _tmp1DB=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpE5);_tmp1DC=_tmp1DB.f1;_tmp1DD=_tmp1DB.f2;_tmp1DC=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpE5->loc,_tmp1DC);_tmp1DD=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpE5->loc,
_tmp1DD);{void*_tmp1DF;void*_tmp1E0;struct _tuple0 _tmp1DE=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1DC,_tmpE6);_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;_tmp1DF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpE6->loc,_tmp1DF);{struct _tuple0 _tmp1E1=({struct _tuple0 _tmp1E4;_tmp1E4.f1=
_tmp1DF;_tmp1E4.f2=_tmp1E0;_tmp1E4;});struct _tuple0 _tmp1E2=({struct _tuple0
_tmp1E3;_tmp1E3.f1=_tmp1DD;_tmp1E3.f2=(void*)((void*)0);_tmp1E3;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1E1,_tmp1E2,0);}}}_LLDA: if(*((int*)_tmpB2)!= 8)goto _LLDC;
_tmpE7=((struct Cyc_Absyn_Or_e_struct*)_tmpB2)->f1;_tmpE8=((struct Cyc_Absyn_Or_e_struct*)
_tmpB2)->f2;_LLDB: {void*_tmp1E6;void*_tmp1E7;struct _tuple0 _tmp1E5=Cyc_NewControlFlow_anal_test(
env,inflow,_tmpE7);_tmp1E6=_tmp1E5.f1;_tmp1E7=_tmp1E5.f2;_tmp1E6=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpE7->loc,_tmp1E6);_tmp1E7=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmpE7->loc,
_tmp1E7);{void*_tmp1E9;void*_tmp1EA;struct _tuple0 _tmp1E8=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1E7,_tmpE8);_tmp1E9=_tmp1E8.f1;_tmp1EA=_tmp1E8.f2;_tmp1E9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpE8->loc,_tmp1E9);{struct _tuple0 _tmp1EB=({struct _tuple0 _tmp1EE;_tmp1EE.f1=
_tmp1E9;_tmp1EE.f2=_tmp1EA;_tmp1EE;});struct _tuple0 _tmp1EC=({struct _tuple0
_tmp1ED;_tmp1ED.f1=_tmp1E6;_tmp1ED.f2=(void*)((void*)1);_tmp1ED;});return Cyc_CfFlowInfo_join_flow_and_rval(
env->all_changed,_tmp1EB,_tmp1EC,0);}}}_LLDC: if(*((int*)_tmpB2)!= 25)goto _LLDE;
_tmpE9=((struct Cyc_Absyn_Subscript_e_struct*)_tmpB2)->f1;_tmpEA=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpB2)->f2;_LLDD: {void*_tmp1F1;struct Cyc_List_List*_tmp1F2;struct _tuple6
_tmp1F0=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct Cyc_Absyn_Exp*
_tmp1EF[2];_tmp1EF[1]=_tmpEA;_tmp1EF[0]=_tmpE9;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1EF,sizeof(struct Cyc_Absyn_Exp*),
2));}),1);_tmp1F1=_tmp1F0.f1;_tmp1F2=_tmp1F0.f2;_tmp1F1=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpEA->loc,_tmp1F1);{void*_tmp1F3=_tmp1F1;{void*_tmp1F4=_tmp1F1;struct Cyc_Dict_Dict*
_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1F7;
_LL158: if(_tmp1F4 <= (void*)1)goto _LL15A;if(*((int*)_tmp1F4)!= 0)goto _LL15A;
_tmp1F5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F4)->f1;_tmp1F6=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F4)->f2;_tmp1F7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1F4)->f3;_LL159: if(Cyc_CfFlowInfo_initlevel(_tmp1F5,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp1F2))->tl))->hd)== (void*)0)({
void*_tmp1F8[0]={};Cyc_Tcutil_terr(_tmpEA->loc,({const char*_tmp1F9="expression may not be initialized";
_tag_dynforward(_tmp1F9,sizeof(char),_get_zero_arr_size(_tmp1F9,34));}),
_tag_dynforward(_tmp1F8,sizeof(void*),0));});{struct Cyc_List_List*_tmp1FA=Cyc_NewControlFlow_add_subscript_reln(
_tmp1F6,_tmpE9,_tmpEA);if(_tmp1F6 != _tmp1FA)_tmp1F3=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=_tmp1F5;_tmp1FC.f2=_tmp1FA;_tmp1FC.f3=_tmp1F7;
_tmp1FC;});_tmp1FB;});goto _LL157;}_LL15A:;_LL15B: goto _LL157;_LL157:;}{void*
_tmp1FD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpE9->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1FE;struct Cyc_Absyn_PtrAtts _tmp1FF;struct Cyc_Absyn_Conref*
_tmp200;_LL15D: if(_tmp1FD <= (void*)4)goto _LL161;if(*((int*)_tmp1FD)!= 9)goto
_LL15F;_LL15E: {void*_tmp201=(void*)((struct Cyc_List_List*)_check_null(_tmp1F2))->hd;
struct Cyc_Dict_Dict*_tmp202;_LL164: if(_tmp201 <= (void*)3)goto _LL166;if(*((int*)
_tmp201)!= 4)goto _LL166;_tmp202=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp201)->f1;
_LL165: return({struct _tuple0 _tmp203;_tmp203.f1=_tmp1F3;_tmp203.f2=((void*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp202,Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmpEA)).f1));_tmp203;});_LL166:;_LL167:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp204=_cycalloc(sizeof(*
_tmp204));_tmp204[0]=({struct Cyc_Core_Impossible_struct _tmp205;_tmp205.tag=Cyc_Core_Impossible;
_tmp205.f1=({const char*_tmp206="anal_Rexp: Subscript";_tag_dynforward(_tmp206,
sizeof(char),_get_zero_arr_size(_tmp206,21));});_tmp205;});_tmp204;}));_LL163:;}
_LL15F: if(*((int*)_tmp1FD)!= 4)goto _LL161;_tmp1FE=((struct Cyc_Absyn_PointerType_struct*)
_tmp1FD)->f1;_tmp1FF=_tmp1FE.ptr_atts;_tmp200=_tmp1FF.bounds;_LL160:{void*
_tmp207=(void*)(Cyc_Absyn_compress_conref(_tmp200))->v;void*_tmp208;_LL169: if(
_tmp207 <= (void*)1)goto _LL16B;if(*((int*)_tmp207)!= 0)goto _LL16B;_tmp208=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp207)->f1;if(_tmp208 <= (void*)2)goto _LL16B;
if(*((int*)_tmp208)!= 1)goto _LL16B;_LL16A:{void*_tmp209=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp1F2))->tl))->hd;struct Cyc_List_List*
_tmp20A;_LL16E: if(_tmp209 <= (void*)3)goto _LL170;if(*((int*)_tmp209)!= 3)goto
_LL170;_tmp20A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp209)->f1;_LL16F:(void*)(
_tmpEA->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp20B=
_cycalloc(sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp20C;_tmp20C.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp20C.f1=_tmp20A;_tmp20C;});
_tmp20B;})));goto _LL16D;_LL170:;_LL171: goto _LL16D;_LL16D:;}goto _LL168;_LL16B:;
_LL16C: goto _LL168;_LL168:;}{void*_tmp20E;void*_tmp20F;struct _tuple0 _tmp20D=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp1F1,_tmpE9,(void*)((struct Cyc_List_List*)_check_null(_tmp1F2))->hd);
_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;return _tmp20E == (void*)0?({struct _tuple0
_tmp210;_tmp210.f1=_tmp20E;_tmp210.f2=_tmp20F;_tmp210;}):({struct _tuple0 _tmp211;
_tmp211.f1=_tmp1F3;_tmp211.f2=_tmp20F;_tmp211;});}_LL161:;_LL162:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp212=_cycalloc(sizeof(*_tmp212));
_tmp212[0]=({struct Cyc_Core_Impossible_struct _tmp213;_tmp213.tag=Cyc_Core_Impossible;
_tmp213.f1=({const char*_tmp214="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp214,sizeof(char),_get_zero_arr_size(_tmp214,33));});_tmp213;});
_tmp212;}));_LL15C:;}}}_LLDE: if(*((int*)_tmpB2)!= 32)goto _LLE0;_tmpEB=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpB2)->f1;_LLDF: _tmpEC=_tmpEB;goto _LLE1;_LLE0: if(*((
int*)_tmpB2)!= 26)goto _LLE2;_tmpEC=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB2)->f1;
_LLE1: {void*_tmp216;struct Cyc_List_List*_tmp217;struct _tuple6 _tmp215=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpEC,0);_tmp216=_tmp215.f1;_tmp217=_tmp215.f2;_tmp216=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp216);{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{
int i=0;for(0;_tmp217 != 0;(_tmp217=_tmp217->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(aggrdict,
Cyc_Absyn_fieldname(i),(void*)_tmp217->hd);}}return({struct _tuple0 _tmp218;
_tmp218.f1=_tmp216;_tmp218.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp21A;_tmp21A.tag=4;_tmp21A.f1=aggrdict;_tmp21A;});_tmp219;});_tmp218;});}}
_LLE2: if(*((int*)_tmpB2)!= 31)goto _LLE4;_tmpED=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpB2)->f2;_LLE3: _tmpEE=_tmpED;goto _LLE5;_LLE4: if(*((int*)_tmpB2)!= 30)goto
_LLE6;_tmpEE=((struct Cyc_Absyn_Struct_e_struct*)_tmpB2)->f3;_LLE5: {void*_tmp21C;
struct Cyc_List_List*_tmp21D;struct _tuple6 _tmp21B=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple7*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple7*))
Cyc_Core_snd,_tmpEE),0);_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;_tmp21C=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp21C);{struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);{
int i=0;for(0;_tmp21D != 0;(((_tmp21D=_tmp21D->tl,_tmpEE=_tmpEE->tl)),++ i)){struct
Cyc_List_List*ds=(*((struct _tuple7*)((struct Cyc_List_List*)_check_null(_tmpEE))->hd)).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp21E=(void*)ds->hd;struct _dynforward_ptr*_tmp21F;
_LL173: if(*((int*)_tmp21E)!= 0)goto _LL175;_LL174:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Core_Impossible_struct
_tmp221;_tmp221.tag=Cyc_Core_Impossible;_tmp221.f1=({const char*_tmp222="anal_Rexp:Struct_e";
_tag_dynforward(_tmp222,sizeof(char),_get_zero_arr_size(_tmp222,19));});_tmp221;});
_tmp220;}));_LL175: if(*((int*)_tmp21E)!= 1)goto _LL172;_tmp21F=((struct Cyc_Absyn_FieldName_struct*)
_tmp21E)->f1;_LL176: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _dynforward_ptr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp21F,(void*)_tmp21D->hd);
_LL172:;}}}return({struct _tuple0 _tmp223;_tmp223.f1=_tmp21C;_tmp223.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp224=_cycalloc(sizeof(*_tmp224));
_tmp224[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp225;_tmp225.tag=4;_tmp225.f1=
aggrdict;_tmp225;});_tmp224;});_tmp223;});}}_LLE6: if(*((int*)_tmpB2)!= 28)goto
_LLE8;_tmpEF=((struct Cyc_Absyn_Array_e_struct*)_tmpB2)->f1;_LLE7: {struct Cyc_List_List*
_tmp226=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple7*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple7*))
Cyc_Core_snd,_tmpEF);void*_tmp228;struct Cyc_List_List*_tmp229;struct _tuple6
_tmp227=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp226,0);_tmp228=
_tmp227.f1;_tmp229=_tmp227.f2;_tmp228=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp228);for(0;_tmp229 != 0;(_tmp229=_tmp229->tl,_tmp226=_tmp226->tl)){_tmp228=
Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp226))->hd)->loc,_tmp228,(void*)_tmp229->hd);}return({struct
_tuple0 _tmp22A;_tmp22A.f1=_tmp228;_tmp22A.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp22A;});}
_LLE8: if(*((int*)_tmpB2)!= 29)goto _LLEA;_tmpF0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB2)->f1;_tmpF1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB2)->f2;_tmpF2=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpB2)->f3;_tmpF3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB2)->f4;_LLE9: {void*_tmp22C;void*_tmp22D;struct _tuple0 _tmp22B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpF1);_tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;_tmp22C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmpF1->loc,_tmp22C);{void*_tmp22E=_tmp22C;struct Cyc_Dict_Dict*_tmp22F;struct Cyc_List_List*
_tmp230;struct Cyc_CfFlowInfo_ConsumeInfo _tmp231;_LL178: if((int)_tmp22E != 0)goto
_LL17A;_LL179: return({struct _tuple0 _tmp232;_tmp232.f1=_tmp22C;_tmp232.f2=Cyc_CfFlowInfo_unknown_all;
_tmp232;});_LL17A: if(_tmp22E <= (void*)1)goto _LL177;if(*((int*)_tmp22E)!= 0)goto
_LL177;_tmp22F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp22E)->f1;_tmp230=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp22E)->f2;_tmp231=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp22E)->f3;_LL17B: if(Cyc_CfFlowInfo_initlevel(_tmp22F,_tmp22D)== (void*)0)({
void*_tmp233[0]={};Cyc_Tcutil_terr(_tmpF1->loc,({const char*_tmp234="expression may not be initialized";
_tag_dynforward(_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,34));}),
_tag_dynforward(_tmp233,sizeof(void*),0));});{struct Cyc_List_List*new_relns=
_tmp230;comp_loop: {void*_tmp235=(void*)_tmpF1->r;struct Cyc_Absyn_Exp*_tmp236;
void*_tmp237;struct Cyc_Absyn_Vardecl*_tmp238;void*_tmp239;struct Cyc_Absyn_Vardecl*
_tmp23A;void*_tmp23B;struct Cyc_Absyn_Vardecl*_tmp23C;void*_tmp23D;struct Cyc_Absyn_Vardecl*
_tmp23E;void*_tmp23F;int _tmp240;void*_tmp241;struct Cyc_List_List*_tmp242;struct
Cyc_List_List _tmp243;struct Cyc_Absyn_Exp*_tmp244;_LL17D: if(*((int*)_tmp235)!= 15)
goto _LL17F;_tmp236=((struct Cyc_Absyn_Cast_e_struct*)_tmp235)->f2;_LL17E: _tmpF1=
_tmp236;goto comp_loop;_LL17F: if(*((int*)_tmp235)!= 1)goto _LL181;_tmp237=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp235)->f2;if(_tmp237 <= (void*)1)goto _LL181;if(*((
int*)_tmp237)!= 0)goto _LL181;_tmp238=((struct Cyc_Absyn_Global_b_struct*)_tmp237)->f1;
if(!(!_tmp238->escapes))goto _LL181;_LL180: _tmp23A=_tmp238;goto _LL182;_LL181: if(*((
int*)_tmp235)!= 1)goto _LL183;_tmp239=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp235)->f2;if(_tmp239 <= (void*)1)goto _LL183;if(*((int*)_tmp239)!= 3)goto _LL183;
_tmp23A=((struct Cyc_Absyn_Local_b_struct*)_tmp239)->f1;if(!(!_tmp23A->escapes))
goto _LL183;_LL182: _tmp23C=_tmp23A;goto _LL184;_LL183: if(*((int*)_tmp235)!= 1)goto
_LL185;_tmp23B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp235)->f2;if(_tmp23B <= (
void*)1)goto _LL185;if(*((int*)_tmp23B)!= 4)goto _LL185;_tmp23C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp23B)->f1;if(!(!_tmp23C->escapes))goto _LL185;_LL184: _tmp23E=_tmp23C;goto
_LL186;_LL185: if(*((int*)_tmp235)!= 1)goto _LL187;_tmp23D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp235)->f2;if(_tmp23D <= (void*)1)goto _LL187;if(*((int*)_tmp23D)!= 2)goto _LL187;
_tmp23E=((struct Cyc_Absyn_Param_b_struct*)_tmp23D)->f1;if(!(!_tmp23E->escapes))
goto _LL187;_LL186: new_relns=({struct Cyc_List_List*_tmp245=_cycalloc(sizeof(*
_tmp245));_tmp245->hd=({struct Cyc_CfFlowInfo_Reln*_tmp246=_cycalloc(sizeof(*
_tmp246));_tmp246->vd=_tmpF0;_tmp246->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp248;_tmp248.tag=1;_tmp248.f1=_tmp23E;_tmp248;});_tmp247;}));_tmp246;});
_tmp245->tl=_tmp230;_tmp245;});goto _LL17C;_LL187: if(*((int*)_tmp235)!= 0)goto
_LL189;_tmp23F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp235)->f1;if(_tmp23F
<= (void*)1)goto _LL189;if(*((int*)_tmp23F)!= 2)goto _LL189;_tmp240=((struct Cyc_Absyn_Int_c_struct*)
_tmp23F)->f2;_LL188: new_relns=({struct Cyc_List_List*_tmp249=_cycalloc(sizeof(*
_tmp249));_tmp249->hd=({struct Cyc_CfFlowInfo_Reln*_tmp24A=_cycalloc(sizeof(*
_tmp24A));_tmp24A->vd=_tmpF0;_tmp24A->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp24B=_cycalloc_atomic(sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp24C;_tmp24C.tag=3;_tmp24C.f1=(unsigned int)_tmp240;_tmp24C;});_tmp24B;}));
_tmp24A;});_tmp249->tl=_tmp230;_tmp249;});goto _LL17C;_LL189: if(*((int*)_tmp235)
!= 3)goto _LL18B;_tmp241=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp235)->f1;
if((int)_tmp241 != 19)goto _LL18B;_tmp242=((struct Cyc_Absyn_Primop_e_struct*)
_tmp235)->f2;if(_tmp242 == 0)goto _LL18B;_tmp243=*_tmp242;_tmp244=(struct Cyc_Absyn_Exp*)
_tmp243.hd;_LL18A:{void*_tmp24D=(void*)_tmp244->r;void*_tmp24E;struct Cyc_Absyn_Vardecl*
_tmp24F;void*_tmp250;struct Cyc_Absyn_Vardecl*_tmp251;void*_tmp252;struct Cyc_Absyn_Vardecl*
_tmp253;void*_tmp254;struct Cyc_Absyn_Vardecl*_tmp255;_LL18E: if(*((int*)_tmp24D)
!= 1)goto _LL190;_tmp24E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24D)->f2;if(
_tmp24E <= (void*)1)goto _LL190;if(*((int*)_tmp24E)!= 0)goto _LL190;_tmp24F=((
struct Cyc_Absyn_Global_b_struct*)_tmp24E)->f1;if(!(!_tmp24F->escapes))goto _LL190;
_LL18F: _tmp251=_tmp24F;goto _LL191;_LL190: if(*((int*)_tmp24D)!= 1)goto _LL192;
_tmp250=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24D)->f2;if(_tmp250 <= (void*)
1)goto _LL192;if(*((int*)_tmp250)!= 3)goto _LL192;_tmp251=((struct Cyc_Absyn_Local_b_struct*)
_tmp250)->f1;if(!(!_tmp251->escapes))goto _LL192;_LL191: _tmp253=_tmp251;goto
_LL193;_LL192: if(*((int*)_tmp24D)!= 1)goto _LL194;_tmp252=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24D)->f2;if(_tmp252 <= (void*)1)goto _LL194;if(*((int*)_tmp252)!= 4)goto _LL194;
_tmp253=((struct Cyc_Absyn_Pat_b_struct*)_tmp252)->f1;if(!(!_tmp253->escapes))
goto _LL194;_LL193: _tmp255=_tmp253;goto _LL195;_LL194: if(*((int*)_tmp24D)!= 1)goto
_LL196;_tmp254=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24D)->f2;if(_tmp254 <= (
void*)1)goto _LL196;if(*((int*)_tmp254)!= 2)goto _LL196;_tmp255=((struct Cyc_Absyn_Param_b_struct*)
_tmp254)->f1;if(!(!_tmp255->escapes))goto _LL196;_LL195: new_relns=({struct Cyc_List_List*
_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->vd=_tmpF0;_tmp257->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp258=_cycalloc(sizeof(*_tmp258));
_tmp258[0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp259;_tmp259.tag=2;_tmp259.f1=
_tmp255;_tmp259;});_tmp258;}));_tmp257;});_tmp256->tl=_tmp230;_tmp256;});goto
_LL18D;_LL196:;_LL197: goto _LL18D;_LL18D:;}goto _LL17C;_LL18B:;_LL18C: goto _LL17C;
_LL17C:;}if(_tmp230 != new_relns)_tmp22C=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp25B;_tmp25B.tag=0;_tmp25B.f1=_tmp22F;_tmp25B.f2=new_relns;_tmp25B.f3=_tmp231;
_tmp25B;});_tmp25A;});{void*_tmp25C=_tmp22D;_LL199: if((int)_tmp25C != 0)goto
_LL19B;_LL19A: return({struct _tuple0 _tmp25D;_tmp25D.f1=_tmp22C;_tmp25D.f2=Cyc_CfFlowInfo_unknown_all;
_tmp25D;});_LL19B: if((int)_tmp25C != 2)goto _LL19D;_LL19C: goto _LL19E;_LL19D: if((
int)_tmp25C != 1)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(_tmp25C <= (void*)3)goto
_LL1A1;if(*((int*)_tmp25C)!= 2)goto _LL1A1;_LL1A0: {struct Cyc_List_List _tmp25E=({
struct Cyc_List_List _tmp269;_tmp269.hd=_tmpF0;_tmp269.tl=0;_tmp269;});_tmp22C=Cyc_NewControlFlow_add_vars(
_tmp22C,(struct Cyc_List_List*)& _tmp25E,Cyc_CfFlowInfo_unknown_all);{void*_tmp260;
void*_tmp261;struct _tuple0 _tmp25F=Cyc_NewControlFlow_anal_Rexp(env,_tmp22C,
_tmpF2);_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;_tmp260=Cyc_CfFlowInfo_consume_unique_rvals(
_tmpF2->loc,_tmp260);{void*_tmp262=_tmp260;struct Cyc_Dict_Dict*_tmp263;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp264;_LL1A4: if((int)_tmp262 != 0)goto _LL1A6;_LL1A5: return({struct _tuple0
_tmp265;_tmp265.f1=_tmp260;_tmp265.f2=Cyc_CfFlowInfo_unknown_all;_tmp265;});
_LL1A6: if(_tmp262 <= (void*)1)goto _LL1A3;if(*((int*)_tmp262)!= 0)goto _LL1A3;
_tmp263=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp262)->f1;_tmp264=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp262)->f3;_LL1A7: if(Cyc_CfFlowInfo_initlevel(
_tmp263,_tmp261)!= (void*)2){({void*_tmp266[0]={};Cyc_Tcutil_terr(_tmpF1->loc,({
const char*_tmp267="expression may not be initialized";_tag_dynforward(_tmp267,
sizeof(char),_get_zero_arr_size(_tmp267,34));}),_tag_dynforward(_tmp266,sizeof(
void*),0));});return({struct _tuple0 _tmp268;_tmp268.f1=(void*)0;_tmp268.f2=Cyc_CfFlowInfo_unknown_all;
_tmp268;});}_LL1A3:;}_tmp22C=_tmp260;goto _LL1A2;}}_LL1A1:;_LL1A2: while(1){struct
Cyc_List_List _tmp26A=({struct Cyc_List_List _tmp275;_tmp275.hd=_tmpF0;_tmp275.tl=0;
_tmp275;});_tmp22C=Cyc_NewControlFlow_add_vars(_tmp22C,(struct Cyc_List_List*)&
_tmp26A,Cyc_CfFlowInfo_unknown_all);{void*_tmp26C;void*_tmp26D;struct _tuple0
_tmp26B=Cyc_NewControlFlow_anal_Rexp(env,_tmp22C,_tmpF2);_tmp26C=_tmp26B.f1;
_tmp26D=_tmp26B.f2;_tmp26C=Cyc_CfFlowInfo_consume_unique_rvals(_tmpF2->loc,
_tmp26C);{void*_tmp26E=_tmp26C;struct Cyc_Dict_Dict*_tmp26F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp270;_LL1A9: if((int)_tmp26E != 0)goto _LL1AB;_LL1AA: goto _LL1A8;_LL1AB: if(
_tmp26E <= (void*)1)goto _LL1A8;if(*((int*)_tmp26E)!= 0)goto _LL1A8;_tmp26F=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp26E)->f1;_tmp270=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp26E)->f3;_LL1AC: if(Cyc_CfFlowInfo_initlevel(_tmp26F,_tmp26D)!= (void*)2){({
void*_tmp271[0]={};Cyc_Tcutil_terr(_tmpF1->loc,({const char*_tmp272="expression may not be initialized";
_tag_dynforward(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,34));}),
_tag_dynforward(_tmp271,sizeof(void*),0));});return({struct _tuple0 _tmp273;
_tmp273.f1=(void*)0;_tmp273.f2=Cyc_CfFlowInfo_unknown_all;_tmp273;});}_LL1A8:;}{
void*_tmp274=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp22C,_tmp26C,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp274,_tmp22C))break;_tmp22C=_tmp274;}}}return({struct _tuple0 _tmp276;_tmp276.f1=
_tmp22C;_tmp276.f2=Cyc_CfFlowInfo_unknown_all;_tmp276;});_LL198:;}}_LL177:;}}
_LLEA: if(*((int*)_tmpB2)!= 37)goto _LLEC;_tmpF4=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpB2)->f1;_LLEB: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmpF4))->flow;{void*_tmp277=(void*)_tmpF4->r;struct Cyc_Absyn_Stmt*
_tmp278;struct Cyc_Absyn_Stmt*_tmp279;struct Cyc_Absyn_Decl*_tmp27A;struct Cyc_Absyn_Stmt*
_tmp27B;struct Cyc_Absyn_Exp*_tmp27C;_LL1AE: if(_tmp277 <= (void*)1)goto _LL1B4;if(*((
int*)_tmp277)!= 1)goto _LL1B0;_tmp278=((struct Cyc_Absyn_Seq_s_struct*)_tmp277)->f1;
_tmp279=((struct Cyc_Absyn_Seq_s_struct*)_tmp277)->f2;_LL1AF: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp278);_tmpF4=_tmp279;goto _LL1AD;_LL1B0: if(*((int*)_tmp277)!= 11)
goto _LL1B2;_tmp27A=((struct Cyc_Absyn_Decl_s_struct*)_tmp277)->f1;_tmp27B=((
struct Cyc_Absyn_Decl_s_struct*)_tmp277)->f2;_LL1B1: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp27A);_tmpF4=_tmp27B;goto _LL1AD;_LL1B2: if(*((int*)_tmp277)!= 0)
goto _LL1B4;_tmp27C=((struct Cyc_Absyn_Exp_s_struct*)_tmp277)->f1;_LL1B3: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp27C);_LL1B4:;_LL1B5:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D[0]=({struct Cyc_Core_Impossible_struct
_tmp27E;_tmp27E.tag=Cyc_Core_Impossible;_tmp27E.f1=({const char*_tmp27F="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,33));});_tmp27E;});
_tmp27D;}));_LL1AD:;}}_LLEC: if(*((int*)_tmpB2)!= 1)goto _LLEE;_tmpF5=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpB2)->f2;if((int)_tmpF5 != 0)goto _LLEE;_LLED:
goto _LLEF;_LLEE: if(*((int*)_tmpB2)!= 2)goto _LLF0;_LLEF: goto _LLF1;_LLF0: if(*((int*)
_tmpB2)!= 10)goto _LLF2;_LLF1: goto _LLF3;_LLF2: if(*((int*)_tmpB2)!= 36)goto _LLF4;
_LLF3: goto _LLF5;_LLF4: if(*((int*)_tmpB2)!= 27)goto _LL93;_LLF5:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280[0]=({
struct Cyc_Core_Impossible_struct _tmp281;_tmp281.tag=Cyc_Core_Impossible;_tmp281.f1=({
const char*_tmp282="anal_Rexp, unexpected exp form";_tag_dynforward(_tmp282,
sizeof(char),_get_zero_arr_size(_tmp282,31));});_tmp281;});_tmp280;}));_LL93:;}}
static struct _tuple0 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){
void*_tmp283=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp284;void*_tmp285;struct Cyc_Absyn_PtrAtts _tmp286;
struct Cyc_Absyn_Conref*_tmp287;struct Cyc_Absyn_Conref*_tmp288;_LL1B7: if(_tmp283
<= (void*)4)goto _LL1B9;if(*((int*)_tmp283)!= 4)goto _LL1B9;_tmp284=((struct Cyc_Absyn_PointerType_struct*)
_tmp283)->f1;_tmp285=(void*)_tmp284.elt_typ;_tmp286=_tmp284.ptr_atts;_tmp287=
_tmp286.bounds;_tmp288=_tmp286.zero_term;_LL1B8: {void*_tmp289=f;struct Cyc_Dict_Dict*
_tmp28A;struct Cyc_List_List*_tmp28B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp28C;
_LL1BC: if((int)_tmp289 != 0)goto _LL1BE;_LL1BD: return({struct _tuple0 _tmp28D;
_tmp28D.f1=f;_tmp28D.f2=(void*)0;_tmp28D;});_LL1BE: if(_tmp289 <= (void*)1)goto
_LL1BB;if(*((int*)_tmp289)!= 0)goto _LL1BB;_tmp28A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp289)->f1;_tmp28B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp289)->f2;
_tmp28C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp289)->f3;_LL1BF: if(Cyc_Tcutil_is_bound_one(
_tmp287)){void*_tmp28E=r;struct Cyc_CfFlowInfo_Place*_tmp28F;struct Cyc_CfFlowInfo_Place
_tmp290;void*_tmp291;struct Cyc_List_List*_tmp292;void*_tmp293;_LL1C1: if((int)
_tmp28E != 1)goto _LL1C3;_LL1C2: goto _LL1C4;_LL1C3: if((int)_tmp28E != 2)goto _LL1C5;
_LL1C4:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp295;_tmp295.tag=Cyc_CfFlowInfo_NotZero;_tmp295.f1=_tmp28B;_tmp295;});_tmp294;})));
goto _LL1C0;_LL1C5: if(_tmp28E <= (void*)3)goto _LL1C7;if(*((int*)_tmp28E)!= 2)goto
_LL1C7;_tmp28F=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp28E)->f1;_tmp290=*
_tmp28F;_tmp291=(void*)_tmp290.root;_tmp292=_tmp290.fields;_LL1C6:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp296=_cycalloc(sizeof(*
_tmp296));_tmp296[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp297;_tmp297.tag=
Cyc_CfFlowInfo_NotZero;_tmp297.f1=_tmp28B;_tmp297;});_tmp296;})));return({struct
_tuple0 _tmp298;_tmp298.f1=f;_tmp298.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp29A;_tmp29A.tag=0;_tmp29A.f1=({struct Cyc_CfFlowInfo_Place*_tmp29B=_cycalloc(
sizeof(*_tmp29B));_tmp29B->root=(void*)_tmp291;_tmp29B->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp292,flds);
_tmp29B;});_tmp29A;});_tmp299;});_tmp298;});_LL1C7: if((int)_tmp28E != 0)goto
_LL1C9;_LL1C8:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));return({
struct _tuple0 _tmp29C;_tmp29C.f1=(void*)0;_tmp29C.f2=(void*)0;_tmp29C;});_LL1C9:
if(_tmp28E <= (void*)3)goto _LL1CB;if(*((int*)_tmp28E)!= 0)goto _LL1CB;_tmp293=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp28E)->f1;_LL1CA: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp293);goto _LL1CC;_LL1CB:;_LL1CC:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D[
0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp29E;_tmp29E.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp29E.f1=_tmp28B;_tmp29E;});_tmp29D;})));_LL1C0:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp29F=_cycalloc(sizeof(*_tmp29F));
_tmp29F[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp2A0;_tmp2A0.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp2A0.f1=_tmp28B;_tmp2A0;});_tmp29F;})));}if(Cyc_CfFlowInfo_initlevel(_tmp28A,
r)== (void*)0)({void*_tmp2A1[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp2A2="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,46));}),
_tag_dynforward(_tmp2A1,sizeof(void*),0));});return({struct _tuple0 _tmp2A3;
_tmp2A3.f1=f;_tmp2A3.f2=(void*)0;_tmp2A3;});_LL1BB:;}_LL1B9:;_LL1BA:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));
_tmp2A4[0]=({struct Cyc_Core_Impossible_struct _tmp2A5;_tmp2A5.tag=Cyc_Core_Impossible;
_tmp2A5.f1=({const char*_tmp2A6="left deref of non-pointer-type";_tag_dynforward(
_tmp2A6,sizeof(char),_get_zero_arr_size(_tmp2A6,31));});_tmp2A5;});_tmp2A4;}));
_LL1B6:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict*
d;{void*_tmp2A7=inflow;struct Cyc_Dict_Dict*_tmp2A8;struct Cyc_List_List*_tmp2A9;
_LL1CE: if((int)_tmp2A7 != 0)goto _LL1D0;_LL1CF: return({struct _tuple0 _tmp2AA;
_tmp2AA.f1=(void*)0;_tmp2AA.f2=(void*)0;_tmp2AA;});_LL1D0: if(_tmp2A7 <= (void*)1)
goto _LL1CD;if(*((int*)_tmp2A7)!= 0)goto _LL1CD;_tmp2A8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2A7)->f1;_tmp2A9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2A7)->f2;
_LL1D1: d=_tmp2A8;_LL1CD:;}{void*_tmp2AB=(void*)e->r;struct Cyc_Absyn_Exp*_tmp2AC;
struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AE;void*_tmp2AF;struct Cyc_Absyn_Vardecl*
_tmp2B0;void*_tmp2B1;struct Cyc_Absyn_Vardecl*_tmp2B2;void*_tmp2B3;struct Cyc_Absyn_Vardecl*
_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Vardecl*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B7;
struct _dynforward_ptr*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*
_tmp2BA;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct
_dynforward_ptr*_tmp2BD;_LL1D3: if(*((int*)_tmp2AB)!= 15)goto _LL1D5;_tmp2AC=((
struct Cyc_Absyn_Cast_e_struct*)_tmp2AB)->f2;_LL1D4: _tmp2AD=_tmp2AC;goto _LL1D6;
_LL1D5: if(*((int*)_tmp2AB)!= 13)goto _LL1D7;_tmp2AD=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp2AB)->f1;_LL1D6: _tmp2AE=_tmp2AD;goto _LL1D8;_LL1D7: if(*((int*)_tmp2AB)!= 14)
goto _LL1D9;_tmp2AE=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2AB)->f1;_LL1D8:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp2AE,flds);_LL1D9: if(*((int*)
_tmp2AB)!= 1)goto _LL1DB;_tmp2AF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2AB)->f2;
if(_tmp2AF <= (void*)1)goto _LL1DB;if(*((int*)_tmp2AF)!= 2)goto _LL1DB;_tmp2B0=((
struct Cyc_Absyn_Param_b_struct*)_tmp2AF)->f1;_LL1DA: _tmp2B2=_tmp2B0;goto _LL1DC;
_LL1DB: if(*((int*)_tmp2AB)!= 1)goto _LL1DD;_tmp2B1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AB)->f2;if(_tmp2B1 <= (void*)1)goto _LL1DD;if(*((int*)_tmp2B1)!= 3)goto _LL1DD;
_tmp2B2=((struct Cyc_Absyn_Local_b_struct*)_tmp2B1)->f1;_LL1DC: _tmp2B4=_tmp2B2;
goto _LL1DE;_LL1DD: if(*((int*)_tmp2AB)!= 1)goto _LL1DF;_tmp2B3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AB)->f2;if(_tmp2B3 <= (void*)1)goto _LL1DF;if(*((int*)_tmp2B3)!= 4)goto _LL1DF;
_tmp2B4=((struct Cyc_Absyn_Pat_b_struct*)_tmp2B3)->f1;_LL1DE: return({struct
_tuple0 _tmp2BE;_tmp2BE.f1=inflow;_tmp2BE.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp2C0;_tmp2C0.tag=0;_tmp2C0.f1=({struct Cyc_CfFlowInfo_Place*_tmp2C1=_cycalloc(
sizeof(*_tmp2C1));_tmp2C1->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=_tmp2B4;_tmp2C3;});_tmp2C2;}));_tmp2C1->fields=
flds;_tmp2C1;});_tmp2C0;});_tmp2BF;});_tmp2BE;});_LL1DF: if(*((int*)_tmp2AB)!= 1)
goto _LL1E1;_tmp2B5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2AB)->f2;if(
_tmp2B5 <= (void*)1)goto _LL1E1;if(*((int*)_tmp2B5)!= 0)goto _LL1E1;_tmp2B6=((
struct Cyc_Absyn_Global_b_struct*)_tmp2B5)->f1;_LL1E0: return({struct _tuple0
_tmp2C4;_tmp2C4.f1=inflow;_tmp2C4.f2=(void*)0;_tmp2C4;});_LL1E1: if(*((int*)
_tmp2AB)!= 24)goto _LL1E3;_tmp2B7=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2AB)->f1;
_tmp2B8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2AB)->f2;_LL1E2:{void*_tmp2C5=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2B7->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp2C6;void*_tmp2C7;_LL1EC: if(_tmp2C5 <= (void*)4)goto
_LL1EE;if(*((int*)_tmp2C5)!= 4)goto _LL1EE;_tmp2C6=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C5)->f1;_tmp2C7=(void*)_tmp2C6.elt_typ;_LL1ED: if(!Cyc_Absyn_is_union_type(
_tmp2C7))flds=({struct Cyc_List_List*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->hd=
_tmp2B8;_tmp2C8->tl=flds;_tmp2C8;});goto _LL1EB;_LL1EE:;_LL1EF:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9[0]=({
struct Cyc_Core_Impossible_struct _tmp2CA;_tmp2CA.tag=Cyc_Core_Impossible;_tmp2CA.f1=({
const char*_tmp2CB="anal_Rexp: AggrArrow ptr";_tag_dynforward(_tmp2CB,sizeof(char),
_get_zero_arr_size(_tmp2CB,25));});_tmp2CA;});_tmp2C9;}));_LL1EB:;}_tmp2B9=
_tmp2B7;goto _LL1E4;_LL1E3: if(*((int*)_tmp2AB)!= 22)goto _LL1E5;_tmp2B9=((struct
Cyc_Absyn_Deref_e_struct*)_tmp2AB)->f1;_LL1E4: {void*_tmp2CD;void*_tmp2CE;struct
_tuple0 _tmp2CC=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp2B9);_tmp2CD=_tmp2CC.f1;
_tmp2CE=_tmp2CC.f2;_tmp2CD=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp2CD);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp2CD,_tmp2B9,_tmp2CE,
flds);}_LL1E5: if(*((int*)_tmp2AB)!= 25)goto _LL1E7;_tmp2BA=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2AB)->f1;_tmp2BB=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2AB)->f2;_LL1E6: {
void*_tmp2CF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2BA->topt))->v);struct Cyc_Absyn_PtrInfo _tmp2D0;struct Cyc_Absyn_PtrAtts
_tmp2D1;struct Cyc_Absyn_Conref*_tmp2D2;_LL1F1: if(_tmp2CF <= (void*)4)goto _LL1F5;
if(*((int*)_tmp2CF)!= 9)goto _LL1F3;_LL1F2: {struct _dynforward_ptr*_tmp2D3=Cyc_Absyn_fieldname((
int)(Cyc_Evexp_eval_const_uint_exp(_tmp2BB)).f1);return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp2BA,({struct Cyc_List_List*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));
_tmp2D4->hd=_tmp2D3;_tmp2D4->tl=flds;_tmp2D4;}));}_LL1F3: if(*((int*)_tmp2CF)!= 4)
goto _LL1F5;_tmp2D0=((struct Cyc_Absyn_PointerType_struct*)_tmp2CF)->f1;_tmp2D1=
_tmp2D0.ptr_atts;_tmp2D2=_tmp2D1.bounds;_LL1F4: {void*_tmp2D7;struct Cyc_List_List*
_tmp2D8;struct _tuple6 _tmp2D6=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({
struct Cyc_Absyn_Exp*_tmp2D5[2];_tmp2D5[1]=_tmp2BB;_tmp2D5[0]=_tmp2BA;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp2D5,sizeof(struct Cyc_Absyn_Exp*),
2));}),1);_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;_tmp2D7=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp2BB->loc,_tmp2D7);{void*_tmp2D9=_tmp2D7;{void*_tmp2DA=_tmp2D7;struct Cyc_Dict_Dict*
_tmp2DB;struct Cyc_List_List*_tmp2DC;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2DD;
_LL1F8: if(_tmp2DA <= (void*)1)goto _LL1FA;if(*((int*)_tmp2DA)!= 0)goto _LL1FA;
_tmp2DB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2DA)->f1;_tmp2DC=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2DA)->f2;_tmp2DD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2DA)->f3;_LL1F9: if(Cyc_CfFlowInfo_initlevel(_tmp2DB,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2D8))->tl))->hd)== (void*)0)({
void*_tmp2DE[0]={};Cyc_Tcutil_terr(_tmp2BB->loc,({const char*_tmp2DF="expression may not be initialized";
_tag_dynforward(_tmp2DF,sizeof(char),_get_zero_arr_size(_tmp2DF,34));}),
_tag_dynforward(_tmp2DE,sizeof(void*),0));});{struct Cyc_List_List*_tmp2E0=Cyc_NewControlFlow_add_subscript_reln(
_tmp2DC,_tmp2BA,_tmp2BB);if(_tmp2DC != _tmp2E0)_tmp2D9=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2E1=_cycalloc(sizeof(*_tmp2E1));_tmp2E1[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp2E2;_tmp2E2.tag=0;_tmp2E2.f1=_tmp2DB;_tmp2E2.f2=_tmp2E0;_tmp2E2.f3=_tmp2DD;
_tmp2E2;});_tmp2E1;});goto _LL1F7;}_LL1FA:;_LL1FB: goto _LL1F7;_LL1F7:;}{void*
_tmp2E3=(void*)(Cyc_Absyn_compress_conref(_tmp2D2))->v;void*_tmp2E4;_LL1FD: if(
_tmp2E3 <= (void*)1)goto _LL1FF;if(*((int*)_tmp2E3)!= 0)goto _LL1FF;_tmp2E4=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp2E3)->f1;if(_tmp2E4 <= (void*)2)goto _LL1FF;
if(*((int*)_tmp2E4)!= 1)goto _LL1FF;_LL1FE:{void*_tmp2E5=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2D8))->tl))->hd;struct Cyc_List_List*
_tmp2E6;_LL202: if(_tmp2E5 <= (void*)3)goto _LL204;if(*((int*)_tmp2E5)!= 3)goto
_LL204;_tmp2E6=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp2E5)->f1;_LL203:(void*)(
_tmp2BB->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp2E7=
_cycalloc(sizeof(*_tmp2E7));_tmp2E7[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp2E8;_tmp2E8.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp2E8.f1=_tmp2E6;_tmp2E8;});
_tmp2E7;})));goto _LL201;_LL204:;_LL205: goto _LL201;_LL201:;}goto _LL1FC;_LL1FF:;
_LL200: goto _LL1FC;_LL1FC:;}{void*_tmp2EA;void*_tmp2EB;struct _tuple0 _tmp2E9=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp2D7,_tmp2BA,(void*)((struct Cyc_List_List*)_check_null(_tmp2D8))->hd,
flds);_tmp2EA=_tmp2E9.f1;_tmp2EB=_tmp2E9.f2;return _tmp2EA == (void*)0?({struct
_tuple0 _tmp2EC;_tmp2EC.f1=_tmp2EA;_tmp2EC.f2=_tmp2EB;_tmp2EC;}):({struct _tuple0
_tmp2ED;_tmp2ED.f1=_tmp2D9;_tmp2ED.f2=_tmp2EB;_tmp2ED;});}}}_LL1F5:;_LL1F6:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2EE=_cycalloc(sizeof(*
_tmp2EE));_tmp2EE[0]=({struct Cyc_Core_Impossible_struct _tmp2EF;_tmp2EF.tag=Cyc_Core_Impossible;
_tmp2EF.f1=({const char*_tmp2F0="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp2F0,sizeof(char),_get_zero_arr_size(_tmp2F0,33));});_tmp2EF;});
_tmp2EE;}));_LL1F0:;}_LL1E7: if(*((int*)_tmp2AB)!= 23)goto _LL1E9;_tmp2BC=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp2AB)->f1;_tmp2BD=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2AB)->f2;_LL1E8: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2BC->topt))->v))return({struct _tuple0 _tmp2F1;_tmp2F1.f1=inflow;
_tmp2F1.f2=(void*)0;_tmp2F1;});return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,
_tmp2BC,({struct Cyc_List_List*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2->hd=
_tmp2BD;_tmp2F2->tl=flds;_tmp2F2;}));_LL1E9:;_LL1EA: return({struct _tuple0 _tmp2F3;
_tmp2F3.f1=(void*)0;_tmp2F3.f2=(void*)0;_tmp2F3;});_LL1D2:;}}static struct _tuple0
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_ConsumeInfo _tmp2F5;struct _tuple5
_tmp2F4=Cyc_CfFlowInfo_save_consume_info(inflow,0);_tmp2F5=_tmp2F4.f1;{void*
_tmp2F7;void*_tmp2F8;struct _tuple0 _tmp2F6=Cyc_NewControlFlow_anal_Lexp_rec(env,
inflow,e,0);_tmp2F7=_tmp2F6.f1;_tmp2F8=_tmp2F6.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2FA;struct _tuple5 _tmp2F9=Cyc_CfFlowInfo_save_consume_info(inflow,0);_tmp2FA=
_tmp2F9.f1;if(_tmp2F5.may_consume != _tmp2FA.may_consume  || _tmp2F5.consumed != 
_tmp2FA.consumed)({void*_tmp2FB[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2FC="oops: anal_Lexp modified consume lists";
_tag_dynforward(_tmp2FC,sizeof(char),_get_zero_arr_size(_tmp2FC,39));}),
_tag_dynforward(_tmp2FB,sizeof(void*),0));});return({struct _tuple0 _tmp2FD;
_tmp2FD.f1=_tmp2F7;_tmp2FD.f2=_tmp2F8;_tmp2FD;});}}}static struct _tuple0 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){{void*
_tmp2FE=(void*)e->r;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Exp*_tmp300;
struct Cyc_Absyn_Exp*_tmp301;struct Cyc_Absyn_Exp*_tmp302;struct Cyc_Absyn_Exp*
_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp305;struct Cyc_Absyn_Exp*
_tmp306;struct Cyc_Absyn_Exp*_tmp307;void*_tmp308;struct Cyc_List_List*_tmp309;
struct Cyc_List_List _tmp30A;struct Cyc_Absyn_Exp*_tmp30B;struct Cyc_List_List*
_tmp30C;void*_tmp30D;struct Cyc_List_List*_tmp30E;_LL207: if(*((int*)_tmp2FE)!= 6)
goto _LL209;_tmp2FF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2FE)->f1;_tmp300=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp2FE)->f2;_tmp301=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2FE)->f3;_LL208: {void*_tmp310;void*_tmp311;struct _tuple0 _tmp30F=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp2FF);_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;_tmp310=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp2FF->loc,_tmp310);_tmp311=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp2FF->loc,
_tmp311);{void*_tmp313;void*_tmp314;struct _tuple0 _tmp312=Cyc_NewControlFlow_anal_test(
env,_tmp310,_tmp300);_tmp313=_tmp312.f1;_tmp314=_tmp312.f2;{void*_tmp316;void*
_tmp317;struct _tuple0 _tmp315=Cyc_NewControlFlow_anal_test(env,_tmp311,_tmp301);
_tmp316=_tmp315.f1;_tmp317=_tmp315.f2;return({struct _tuple0 _tmp318;_tmp318.f1=
Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp313,_tmp316,1);_tmp318.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp314,_tmp317,1);_tmp318;});}}}_LL209: if(*((int*)_tmp2FE)!= 7)
goto _LL20B;_tmp302=((struct Cyc_Absyn_And_e_struct*)_tmp2FE)->f1;_tmp303=((struct
Cyc_Absyn_And_e_struct*)_tmp2FE)->f2;_LL20A: {void*_tmp31A;void*_tmp31B;struct
_tuple0 _tmp319=Cyc_NewControlFlow_anal_test(env,inflow,_tmp302);_tmp31A=_tmp319.f1;
_tmp31B=_tmp319.f2;_tmp31A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp302->loc,
_tmp31A);_tmp31B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp302->loc,_tmp31B);{
void*_tmp31D;void*_tmp31E;struct _tuple0 _tmp31C=Cyc_NewControlFlow_anal_test(env,
_tmp31A,_tmp303);_tmp31D=_tmp31C.f1;_tmp31E=_tmp31C.f2;_tmp31D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp303->loc,_tmp31D);_tmp31E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp303->loc,
_tmp31E);return({struct _tuple0 _tmp31F;_tmp31F.f1=_tmp31D;_tmp31F.f2=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp31B,_tmp31E,0);_tmp31F;});}}_LL20B: if(*((int*)_tmp2FE)!= 8)
goto _LL20D;_tmp304=((struct Cyc_Absyn_Or_e_struct*)_tmp2FE)->f1;_tmp305=((struct
Cyc_Absyn_Or_e_struct*)_tmp2FE)->f2;_LL20C: {void*_tmp321;void*_tmp322;struct
_tuple0 _tmp320=Cyc_NewControlFlow_anal_test(env,inflow,_tmp304);_tmp321=_tmp320.f1;
_tmp322=_tmp320.f2;_tmp321=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp304->loc,
_tmp321);_tmp322=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp304->loc,_tmp322);{
void*_tmp324;void*_tmp325;struct _tuple0 _tmp323=Cyc_NewControlFlow_anal_test(env,
_tmp322,_tmp305);_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;_tmp324=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp305->loc,_tmp324);_tmp325=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp305->loc,
_tmp325);return({struct _tuple0 _tmp326;_tmp326.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp321,_tmp324,0);_tmp326.f2=_tmp325;_tmp326;});}}_LL20D: if(*((int*)_tmp2FE)!= 
9)goto _LL20F;_tmp306=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2FE)->f1;_tmp307=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp2FE)->f2;_LL20E: {void*_tmp328;void*_tmp329;
struct _tuple0 _tmp327=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp306);_tmp328=
_tmp327.f1;_tmp329=_tmp327.f2;_tmp328=Cyc_CfFlowInfo_drop_unique_rvals(_tmp306->loc,
_tmp328);return Cyc_NewControlFlow_anal_test(env,_tmp328,_tmp307);}_LL20F: if(*((
int*)_tmp2FE)!= 3)goto _LL211;_tmp308=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2FE)->f1;if((int)_tmp308 != 11)goto _LL211;_tmp309=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2FE)->f2;if(_tmp309 == 0)goto _LL211;_tmp30A=*_tmp309;_tmp30B=(struct Cyc_Absyn_Exp*)
_tmp30A.hd;_tmp30C=_tmp30A.tl;if(_tmp30C != 0)goto _LL211;_LL210: {void*_tmp32B;
void*_tmp32C;struct _tuple0 _tmp32A=Cyc_NewControlFlow_anal_test(env,inflow,
_tmp30B);_tmp32B=_tmp32A.f1;_tmp32C=_tmp32A.f2;return({struct _tuple0 _tmp32D;
_tmp32D.f1=_tmp32C;_tmp32D.f2=_tmp32B;_tmp32D;});}_LL211: if(*((int*)_tmp2FE)!= 3)
goto _LL213;_tmp30D=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2FE)->f1;
_tmp30E=((struct Cyc_Absyn_Primop_e_struct*)_tmp2FE)->f2;_LL212: {void*_tmp32F;
struct Cyc_List_List*_tmp330;struct _tuple6 _tmp32E=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp30E,0);_tmp32F=_tmp32E.f1;_tmp330=_tmp32E.f2;{void*_tmp331=
_tmp30D;_LL216: if((int)_tmp331 != 0)goto _LL218;_LL217: goto _LL219;_LL218: if((int)
_tmp331 != 2)goto _LL21A;_LL219: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp30E))->hd)->loc,_tmp32F);goto _LL215;_LL21A:;
_LL21B: _tmp32F=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp30E))->hd)->loc,_tmp32F);goto _LL215;_LL215:;}{
void*_tmp332=_tmp32F;struct Cyc_Dict_Dict*_tmp333;struct Cyc_List_List*_tmp334;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp335;_LL21D: if((int)_tmp332 != 0)goto _LL21F;
_LL21E: return({struct _tuple0 _tmp336;_tmp336.f1=_tmp32F;_tmp336.f2=_tmp32F;
_tmp336;});_LL21F: if(_tmp332 <= (void*)1)goto _LL21C;if(*((int*)_tmp332)!= 0)goto
_LL21C;_tmp333=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp332)->f1;_tmp334=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp332)->f2;_tmp335=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp332)->f3;_LL220: {struct Cyc_Absyn_Exp*_tmp337=(struct Cyc_Absyn_Exp*)_tmp30E->hd;
struct Cyc_Absyn_Exp*_tmp338=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp30E->tl))->hd;void*_tmp339=(void*)((struct Cyc_List_List*)
_check_null(_tmp330))->hd;void*_tmp33A=(void*)((struct Cyc_List_List*)_check_null(
_tmp330->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp333,_tmp339)== (void*)0)({void*
_tmp33B[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp30E->hd)->loc,({const
char*_tmp33C="expression may not be initialized";_tag_dynforward(_tmp33C,sizeof(
char),_get_zero_arr_size(_tmp33C,34));}),_tag_dynforward(_tmp33B,sizeof(void*),0));});
if(Cyc_CfFlowInfo_initlevel(_tmp333,_tmp33A)== (void*)0)({void*_tmp33D[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp30E->tl))->hd)->loc,({
const char*_tmp33E="expression may not be initialized";_tag_dynforward(_tmp33E,
sizeof(char),_get_zero_arr_size(_tmp33E,34));}),_tag_dynforward(_tmp33D,sizeof(
void*),0));});if(_tmp30D == (void*)5  || _tmp30D == (void*)6){struct _tuple0 _tmp340=({
struct _tuple0 _tmp33F;_tmp33F.f1=_tmp339;_tmp33F.f2=_tmp33A;_tmp33F;});void*
_tmp341;void*_tmp342;void*_tmp343;void*_tmp344;void*_tmp345;void*_tmp346;void*
_tmp347;void*_tmp348;void*_tmp349;void*_tmp34A;void*_tmp34B;void*_tmp34C;void*
_tmp34D;void*_tmp34E;void*_tmp34F;void*_tmp350;void*_tmp351;void*_tmp352;void*
_tmp353;void*_tmp354;_LL222: _tmp341=_tmp340.f1;if(_tmp341 <= (void*)3)goto _LL224;
if(*((int*)_tmp341)!= 0)goto _LL224;_tmp342=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp341)->f1;_tmp343=_tmp340.f2;if((int)_tmp343 != 0)goto _LL224;_LL223: {void*
_tmp356;void*_tmp357;struct _tuple0 _tmp355=Cyc_NewControlFlow_splitzero(env,
_tmp32F,_tmp32F,_tmp337,_tmp342);_tmp356=_tmp355.f1;_tmp357=_tmp355.f2;{void*
_tmp358=_tmp30D;_LL237: if((int)_tmp358 != 5)goto _LL239;_LL238: return({struct
_tuple0 _tmp359;_tmp359.f1=_tmp357;_tmp359.f2=_tmp356;_tmp359;});_LL239: if((int)
_tmp358 != 6)goto _LL23B;_LL23A: return({struct _tuple0 _tmp35A;_tmp35A.f1=_tmp356;
_tmp35A.f2=_tmp357;_tmp35A;});_LL23B:;_LL23C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B[0]=({struct Cyc_Core_Impossible_struct
_tmp35C;_tmp35C.tag=Cyc_Core_Impossible;_tmp35C.f1=({const char*_tmp35D="anal_test, zero-split";
_tag_dynforward(_tmp35D,sizeof(char),_get_zero_arr_size(_tmp35D,22));});_tmp35C;});
_tmp35B;}));_LL236:;}}_LL224: _tmp344=_tmp340.f1;if((int)_tmp344 != 0)goto _LL226;
_tmp345=_tmp340.f2;if(_tmp345 <= (void*)3)goto _LL226;if(*((int*)_tmp345)!= 0)goto
_LL226;_tmp346=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp345)->f1;
_LL225: {void*_tmp35F;void*_tmp360;struct _tuple0 _tmp35E=Cyc_NewControlFlow_splitzero(
env,_tmp32F,_tmp32F,_tmp338,_tmp346);_tmp35F=_tmp35E.f1;_tmp360=_tmp35E.f2;{void*
_tmp361=_tmp30D;_LL23E: if((int)_tmp361 != 5)goto _LL240;_LL23F: return({struct
_tuple0 _tmp362;_tmp362.f1=_tmp360;_tmp362.f2=_tmp35F;_tmp362;});_LL240: if((int)
_tmp361 != 6)goto _LL242;_LL241: return({struct _tuple0 _tmp363;_tmp363.f1=_tmp35F;
_tmp363.f2=_tmp360;_tmp363;});_LL242:;_LL243:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364[0]=({struct Cyc_Core_Impossible_struct
_tmp365;_tmp365.tag=Cyc_Core_Impossible;_tmp365.f1=({const char*_tmp366="anal_test, zero-split";
_tag_dynforward(_tmp366,sizeof(char),_get_zero_arr_size(_tmp366,22));});_tmp365;});
_tmp364;}));_LL23D:;}}_LL226: _tmp347=_tmp340.f1;if((int)_tmp347 != 0)goto _LL228;
_tmp348=_tmp340.f2;if((int)_tmp348 != 0)goto _LL228;_LL227: if(_tmp30D == (void*)5)
return({struct _tuple0 _tmp367;_tmp367.f1=_tmp32F;_tmp367.f2=(void*)0;_tmp367;});
else{return({struct _tuple0 _tmp368;_tmp368.f1=(void*)0;_tmp368.f2=_tmp32F;_tmp368;});}
_LL228: _tmp349=_tmp340.f1;if((int)_tmp349 != 0)goto _LL22A;_tmp34A=_tmp340.f2;if((
int)_tmp34A != 1)goto _LL22A;_LL229: goto _LL22B;_LL22A: _tmp34B=_tmp340.f1;if((int)
_tmp34B != 0)goto _LL22C;_tmp34C=_tmp340.f2;if((int)_tmp34C != 2)goto _LL22C;_LL22B:
goto _LL22D;_LL22C: _tmp34D=_tmp340.f1;if((int)_tmp34D != 0)goto _LL22E;_tmp34E=
_tmp340.f2;if(_tmp34E <= (void*)3)goto _LL22E;if(*((int*)_tmp34E)!= 2)goto _LL22E;
_LL22D: goto _LL22F;_LL22E: _tmp34F=_tmp340.f1;if((int)_tmp34F != 1)goto _LL230;
_tmp350=_tmp340.f2;if((int)_tmp350 != 0)goto _LL230;_LL22F: goto _LL231;_LL230:
_tmp351=_tmp340.f1;if((int)_tmp351 != 2)goto _LL232;_tmp352=_tmp340.f2;if((int)
_tmp352 != 0)goto _LL232;_LL231: goto _LL233;_LL232: _tmp353=_tmp340.f1;if(_tmp353 <= (
void*)3)goto _LL234;if(*((int*)_tmp353)!= 2)goto _LL234;_tmp354=_tmp340.f2;if((int)
_tmp354 != 0)goto _LL234;_LL233: if(_tmp30D == (void*)6)return({struct _tuple0 _tmp369;
_tmp369.f1=_tmp32F;_tmp369.f2=(void*)0;_tmp369;});else{return({struct _tuple0
_tmp36A;_tmp36A.f1=(void*)0;_tmp36A.f2=_tmp32F;_tmp36A;});}_LL234:;_LL235: goto
_LL221;_LL221:;}{struct _tuple0 _tmp36C=({struct _tuple0 _tmp36B;_tmp36B.f1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp337->topt))->v);_tmp36B.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp338->topt))->v);_tmp36B;});void*
_tmp36D;void*_tmp36E;void*_tmp36F;void*_tmp370;void*_tmp371;void*_tmp372;_LL245:
_tmp36D=_tmp36C.f1;if(_tmp36D <= (void*)4)goto _LL247;if(*((int*)_tmp36D)!= 5)goto
_LL247;_tmp36E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp36D)->f1;if((int)
_tmp36E != 1)goto _LL247;_LL246: goto _LL248;_LL247: _tmp36F=_tmp36C.f2;if(_tmp36F <= (
void*)4)goto _LL249;if(*((int*)_tmp36F)!= 5)goto _LL249;_tmp370=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp36F)->f1;if((int)_tmp370 != 1)goto _LL249;_LL248: goto _LL24A;_LL249: _tmp371=
_tmp36C.f1;if(_tmp371 <= (void*)4)goto _LL24B;if(*((int*)_tmp371)!= 17)goto _LL24B;
_LL24A: goto _LL24C;_LL24B: _tmp372=_tmp36C.f2;if(_tmp372 <= (void*)4)goto _LL24D;if(*((
int*)_tmp372)!= 17)goto _LL24D;_LL24C: goto _LL244;_LL24D:;_LL24E: return({struct
_tuple0 _tmp373;_tmp373.f1=_tmp32F;_tmp373.f2=_tmp32F;_tmp373;});_LL244:;}{void*
_tmp374=_tmp30D;_LL250: if((int)_tmp374 != 5)goto _LL252;_LL251: {void*_tmp375=Cyc_NewControlFlow_if_tagcmp(
env,_tmp32F,_tmp337,_tmp339,(void*)5,_tmp33A);_tmp375=Cyc_NewControlFlow_if_tagcmp(
env,_tmp375,_tmp338,_tmp33A,(void*)5,_tmp339);return({struct _tuple0 _tmp376;
_tmp376.f1=_tmp375;_tmp376.f2=_tmp32F;_tmp376;});}_LL252: if((int)_tmp374 != 6)
goto _LL254;_LL253: {void*_tmp377=Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,
_tmp337,_tmp339,(void*)5,_tmp33A);_tmp377=Cyc_NewControlFlow_if_tagcmp(env,
_tmp377,_tmp338,_tmp33A,(void*)5,_tmp339);return({struct _tuple0 _tmp378;_tmp378.f1=
_tmp32F;_tmp378.f2=_tmp377;_tmp378;});}_LL254: if((int)_tmp374 != 7)goto _LL256;
_LL255: {void*_tmp379=Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,_tmp338,_tmp33A,(
void*)8,_tmp339);void*_tmp37A=Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,_tmp337,
_tmp339,(void*)10,_tmp33A);return({struct _tuple0 _tmp37B;_tmp37B.f1=_tmp379;
_tmp37B.f2=_tmp37A;_tmp37B;});}_LL256: if((int)_tmp374 != 9)goto _LL258;_LL257: {
void*_tmp37C=Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,_tmp338,_tmp33A,(void*)10,
_tmp339);void*_tmp37D=Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,_tmp337,_tmp339,(
void*)8,_tmp33A);return({struct _tuple0 _tmp37E;_tmp37E.f1=_tmp37C;_tmp37E.f2=
_tmp37D;_tmp37E;});}_LL258: if((int)_tmp374 != 8)goto _LL25A;_LL259: {void*_tmp37F=
Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,_tmp337,_tmp339,(void*)8,_tmp33A);void*
_tmp380=Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,_tmp338,_tmp33A,(void*)10,
_tmp339);{void*_tmp381=_tmp37F;struct Cyc_Dict_Dict*_tmp382;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp383;_LL25F: if((int)_tmp381 != 0)goto _LL261;_LL260:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384[0]=({
struct Cyc_Core_Impossible_struct _tmp385;_tmp385.tag=Cyc_Core_Impossible;_tmp385.f1=({
const char*_tmp386="anal_test, Lt";_tag_dynforward(_tmp386,sizeof(char),
_get_zero_arr_size(_tmp386,14));});_tmp385;});_tmp384;}));_LL261: if(_tmp381 <= (
void*)1)goto _LL25E;if(*((int*)_tmp381)!= 0)goto _LL25E;_tmp382=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp381)->f1;_tmp383=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp381)->f3;
_LL262: _tmp333=_tmp382;_tmp335=_tmp383;_LL25E:;}{void*_tmp387=(void*)_tmp337->r;
void*_tmp388;struct Cyc_Absyn_Vardecl*_tmp389;void*_tmp38A;struct Cyc_Absyn_Vardecl*
_tmp38B;void*_tmp38C;struct Cyc_Absyn_Vardecl*_tmp38D;void*_tmp38E;struct Cyc_Absyn_Vardecl*
_tmp38F;_LL264: if(*((int*)_tmp387)!= 1)goto _LL266;_tmp388=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp387)->f2;if(_tmp388 <= (void*)1)goto _LL266;if(*((int*)_tmp388)!= 0)goto _LL266;
_tmp389=((struct Cyc_Absyn_Global_b_struct*)_tmp388)->f1;if(!(!_tmp389->escapes))
goto _LL266;_LL265: _tmp38B=_tmp389;goto _LL267;_LL266: if(*((int*)_tmp387)!= 1)goto
_LL268;_tmp38A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp387)->f2;if(_tmp38A <= (
void*)1)goto _LL268;if(*((int*)_tmp38A)!= 3)goto _LL268;_tmp38B=((struct Cyc_Absyn_Local_b_struct*)
_tmp38A)->f1;if(!(!_tmp38B->escapes))goto _LL268;_LL267: _tmp38D=_tmp38B;goto
_LL269;_LL268: if(*((int*)_tmp387)!= 1)goto _LL26A;_tmp38C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp387)->f2;if(_tmp38C <= (void*)1)goto _LL26A;if(*((int*)_tmp38C)!= 4)goto _LL26A;
_tmp38D=((struct Cyc_Absyn_Pat_b_struct*)_tmp38C)->f1;if(!(!_tmp38D->escapes))
goto _LL26A;_LL269: _tmp38F=_tmp38D;goto _LL26B;_LL26A: if(*((int*)_tmp387)!= 1)goto
_LL26C;_tmp38E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp387)->f2;if(_tmp38E <= (
void*)1)goto _LL26C;if(*((int*)_tmp38E)!= 2)goto _LL26C;_tmp38F=((struct Cyc_Absyn_Param_b_struct*)
_tmp38E)->f1;if(!(!_tmp38F->escapes))goto _LL26C;_LL26B: {void*_tmp390=(void*)
_tmp338->r;void*_tmp391;struct Cyc_Absyn_Vardecl*_tmp392;void*_tmp393;struct Cyc_Absyn_Vardecl*
_tmp394;void*_tmp395;struct Cyc_Absyn_Vardecl*_tmp396;void*_tmp397;struct Cyc_Absyn_Vardecl*
_tmp398;void*_tmp399;int _tmp39A;void*_tmp39B;struct Cyc_List_List*_tmp39C;struct
Cyc_List_List _tmp39D;struct Cyc_Absyn_Exp*_tmp39E;_LL26F: if(*((int*)_tmp390)!= 1)
goto _LL271;_tmp391=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp390)->f2;if(
_tmp391 <= (void*)1)goto _LL271;if(*((int*)_tmp391)!= 0)goto _LL271;_tmp392=((
struct Cyc_Absyn_Global_b_struct*)_tmp391)->f1;if(!(!_tmp392->escapes))goto _LL271;
_LL270: _tmp394=_tmp392;goto _LL272;_LL271: if(*((int*)_tmp390)!= 1)goto _LL273;
_tmp393=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp390)->f2;if(_tmp393 <= (void*)
1)goto _LL273;if(*((int*)_tmp393)!= 3)goto _LL273;_tmp394=((struct Cyc_Absyn_Local_b_struct*)
_tmp393)->f1;if(!(!_tmp394->escapes))goto _LL273;_LL272: _tmp396=_tmp394;goto
_LL274;_LL273: if(*((int*)_tmp390)!= 1)goto _LL275;_tmp395=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp390)->f2;if(_tmp395 <= (void*)1)goto _LL275;if(*((int*)_tmp395)!= 4)goto _LL275;
_tmp396=((struct Cyc_Absyn_Pat_b_struct*)_tmp395)->f1;if(!(!_tmp396->escapes))
goto _LL275;_LL274: _tmp398=_tmp396;goto _LL276;_LL275: if(*((int*)_tmp390)!= 1)goto
_LL277;_tmp397=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp390)->f2;if(_tmp397 <= (
void*)1)goto _LL277;if(*((int*)_tmp397)!= 2)goto _LL277;_tmp398=((struct Cyc_Absyn_Param_b_struct*)
_tmp397)->f1;if(!(!_tmp398->escapes))goto _LL277;_LL276: {struct Cyc_List_List*
_tmp39F=({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->vd=
_tmp38F;_tmp3A4->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp3A6;_tmp3A6.tag=1;_tmp3A6.f1=_tmp398;_tmp3A6;});_tmp3A5;}));_tmp3A4;});
_tmp3A3->tl=_tmp334;_tmp3A3;});return({struct _tuple0 _tmp3A0;_tmp3A0.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));
_tmp3A1[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3A2;_tmp3A2.tag=0;
_tmp3A2.f1=_tmp333;_tmp3A2.f2=_tmp39F;_tmp3A2.f3=_tmp335;_tmp3A2;});_tmp3A1;});
_tmp3A0.f2=_tmp380;_tmp3A0;});}_LL277: if(*((int*)_tmp390)!= 0)goto _LL279;_tmp399=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp390)->f1;if(_tmp399 <= (void*)1)goto
_LL279;if(*((int*)_tmp399)!= 2)goto _LL279;_tmp39A=((struct Cyc_Absyn_Int_c_struct*)
_tmp399)->f2;_LL278: {struct Cyc_List_List*_tmp3A7=({struct Cyc_List_List*_tmp3AB=
_cycalloc(sizeof(*_tmp3AB));_tmp3AB->hd=({struct Cyc_CfFlowInfo_Reln*_tmp3AC=
_cycalloc(sizeof(*_tmp3AC));_tmp3AC->vd=_tmp38F;_tmp3AC->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessConst_struct*_tmp3AD=_cycalloc_atomic(sizeof(*_tmp3AD));
_tmp3AD[0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp3AE;_tmp3AE.tag=3;_tmp3AE.f1=(
unsigned int)_tmp39A;_tmp3AE;});_tmp3AD;}));_tmp3AC;});_tmp3AB->tl=_tmp334;
_tmp3AB;});return({struct _tuple0 _tmp3A8;_tmp3A8.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=_tmp333;_tmp3AA.f2=_tmp3A7;_tmp3AA.f3=_tmp335;
_tmp3AA;});_tmp3A9;});_tmp3A8.f2=_tmp380;_tmp3A8;});}_LL279: if(*((int*)_tmp390)
!= 3)goto _LL27B;_tmp39B=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp390)->f1;
if((int)_tmp39B != 19)goto _LL27B;_tmp39C=((struct Cyc_Absyn_Primop_e_struct*)
_tmp390)->f2;if(_tmp39C == 0)goto _LL27B;_tmp39D=*_tmp39C;_tmp39E=(struct Cyc_Absyn_Exp*)
_tmp39D.hd;_LL27A: {void*_tmp3AF=(void*)_tmp39E->r;void*_tmp3B0;struct Cyc_Absyn_Vardecl*
_tmp3B1;void*_tmp3B2;struct Cyc_Absyn_Vardecl*_tmp3B3;void*_tmp3B4;struct Cyc_Absyn_Vardecl*
_tmp3B5;void*_tmp3B6;struct Cyc_Absyn_Vardecl*_tmp3B7;_LL27E: if(*((int*)_tmp3AF)
!= 1)goto _LL280;_tmp3B0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3AF)->f2;if(
_tmp3B0 <= (void*)1)goto _LL280;if(*((int*)_tmp3B0)!= 0)goto _LL280;_tmp3B1=((
struct Cyc_Absyn_Global_b_struct*)_tmp3B0)->f1;if(!(!_tmp3B1->escapes))goto _LL280;
_LL27F: _tmp3B3=_tmp3B1;goto _LL281;_LL280: if(*((int*)_tmp3AF)!= 1)goto _LL282;
_tmp3B2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3AF)->f2;if(_tmp3B2 <= (void*)
1)goto _LL282;if(*((int*)_tmp3B2)!= 3)goto _LL282;_tmp3B3=((struct Cyc_Absyn_Local_b_struct*)
_tmp3B2)->f1;if(!(!_tmp3B3->escapes))goto _LL282;_LL281: _tmp3B5=_tmp3B3;goto
_LL283;_LL282: if(*((int*)_tmp3AF)!= 1)goto _LL284;_tmp3B4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3AF)->f2;if(_tmp3B4 <= (void*)1)goto _LL284;if(*((int*)_tmp3B4)!= 4)goto _LL284;
_tmp3B5=((struct Cyc_Absyn_Pat_b_struct*)_tmp3B4)->f1;if(!(!_tmp3B5->escapes))
goto _LL284;_LL283: _tmp3B7=_tmp3B5;goto _LL285;_LL284: if(*((int*)_tmp3AF)!= 1)goto
_LL286;_tmp3B6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3AF)->f2;if(_tmp3B6 <= (
void*)1)goto _LL286;if(*((int*)_tmp3B6)!= 2)goto _LL286;_tmp3B7=((struct Cyc_Absyn_Param_b_struct*)
_tmp3B6)->f1;if(!(!_tmp3B7->escapes))goto _LL286;_LL285: {struct Cyc_List_List*
_tmp3B8=({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->vd=
_tmp38F;_tmp3BD->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp3BF;_tmp3BF.tag=2;_tmp3BF.f1=_tmp3B7;_tmp3BF;});_tmp3BE;}));_tmp3BD;});
_tmp3BC->tl=_tmp334;_tmp3BC;});return({struct _tuple0 _tmp3B9;_tmp3B9.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));
_tmp3BA[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3BB;_tmp3BB.tag=0;
_tmp3BB.f1=_tmp333;_tmp3BB.f2=_tmp3B8;_tmp3BB.f3=_tmp335;_tmp3BB;});_tmp3BA;});
_tmp3B9.f2=_tmp380;_tmp3B9;});}_LL286:;_LL287: return({struct _tuple0 _tmp3C0;
_tmp3C0.f1=_tmp37F;_tmp3C0.f2=_tmp380;_tmp3C0;});_LL27D:;}_LL27B:;_LL27C: return({
struct _tuple0 _tmp3C1;_tmp3C1.f1=_tmp37F;_tmp3C1.f2=_tmp380;_tmp3C1;});_LL26E:;}
_LL26C:;_LL26D: return({struct _tuple0 _tmp3C2;_tmp3C2.f1=_tmp37F;_tmp3C2.f2=
_tmp380;_tmp3C2;});_LL263:;}}_LL25A: if((int)_tmp374 != 10)goto _LL25C;_LL25B: {
void*_tmp3C3=Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,_tmp337,_tmp339,(void*)10,
_tmp33A);void*_tmp3C4=Cyc_NewControlFlow_if_tagcmp(env,_tmp32F,_tmp338,_tmp339,(
void*)8,_tmp339);{void*_tmp3C5=_tmp3C3;struct Cyc_Dict_Dict*_tmp3C6;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3C7;_LL289: if((int)_tmp3C5 != 0)goto _LL28B;_LL28A:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8[0]=({
struct Cyc_Core_Impossible_struct _tmp3C9;_tmp3C9.tag=Cyc_Core_Impossible;_tmp3C9.f1=({
const char*_tmp3CA="anal_test, Lte";_tag_dynforward(_tmp3CA,sizeof(char),
_get_zero_arr_size(_tmp3CA,15));});_tmp3C9;});_tmp3C8;}));_LL28B: if(_tmp3C5 <= (
void*)1)goto _LL288;if(*((int*)_tmp3C5)!= 0)goto _LL288;_tmp3C6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3C5)->f1;_tmp3C7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3C5)->f3;
_LL28C: _tmp333=_tmp3C6;_tmp335=_tmp3C7;_LL288:;}{void*_tmp3CB=(void*)_tmp337->r;
void*_tmp3CC;struct Cyc_Absyn_Vardecl*_tmp3CD;void*_tmp3CE;struct Cyc_Absyn_Vardecl*
_tmp3CF;void*_tmp3D0;struct Cyc_Absyn_Vardecl*_tmp3D1;void*_tmp3D2;struct Cyc_Absyn_Vardecl*
_tmp3D3;_LL28E: if(*((int*)_tmp3CB)!= 1)goto _LL290;_tmp3CC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3CB)->f2;if(_tmp3CC <= (void*)1)goto _LL290;if(*((int*)_tmp3CC)!= 0)goto _LL290;
_tmp3CD=((struct Cyc_Absyn_Global_b_struct*)_tmp3CC)->f1;if(!(!_tmp3CD->escapes))
goto _LL290;_LL28F: _tmp3CF=_tmp3CD;goto _LL291;_LL290: if(*((int*)_tmp3CB)!= 1)goto
_LL292;_tmp3CE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3CB)->f2;if(_tmp3CE <= (
void*)1)goto _LL292;if(*((int*)_tmp3CE)!= 3)goto _LL292;_tmp3CF=((struct Cyc_Absyn_Local_b_struct*)
_tmp3CE)->f1;if(!(!_tmp3CF->escapes))goto _LL292;_LL291: _tmp3D1=_tmp3CF;goto
_LL293;_LL292: if(*((int*)_tmp3CB)!= 1)goto _LL294;_tmp3D0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3CB)->f2;if(_tmp3D0 <= (void*)1)goto _LL294;if(*((int*)_tmp3D0)!= 4)goto _LL294;
_tmp3D1=((struct Cyc_Absyn_Pat_b_struct*)_tmp3D0)->f1;if(!(!_tmp3D1->escapes))
goto _LL294;_LL293: _tmp3D3=_tmp3D1;goto _LL295;_LL294: if(*((int*)_tmp3CB)!= 1)goto
_LL296;_tmp3D2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3CB)->f2;if(_tmp3D2 <= (
void*)1)goto _LL296;if(*((int*)_tmp3D2)!= 2)goto _LL296;_tmp3D3=((struct Cyc_Absyn_Param_b_struct*)
_tmp3D2)->f1;if(!(!_tmp3D3->escapes))goto _LL296;_LL295: {void*_tmp3D4=(void*)
_tmp338->r;void*_tmp3D5;struct Cyc_List_List*_tmp3D6;struct Cyc_List_List _tmp3D7;
struct Cyc_Absyn_Exp*_tmp3D8;_LL299: if(*((int*)_tmp3D4)!= 3)goto _LL29B;_tmp3D5=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3D4)->f1;if((int)_tmp3D5 != 19)goto
_LL29B;_tmp3D6=((struct Cyc_Absyn_Primop_e_struct*)_tmp3D4)->f2;if(_tmp3D6 == 0)
goto _LL29B;_tmp3D7=*_tmp3D6;_tmp3D8=(struct Cyc_Absyn_Exp*)_tmp3D7.hd;_LL29A: {
void*_tmp3D9=(void*)_tmp3D8->r;void*_tmp3DA;struct Cyc_Absyn_Vardecl*_tmp3DB;void*
_tmp3DC;struct Cyc_Absyn_Vardecl*_tmp3DD;void*_tmp3DE;struct Cyc_Absyn_Vardecl*
_tmp3DF;void*_tmp3E0;struct Cyc_Absyn_Vardecl*_tmp3E1;_LL29E: if(*((int*)_tmp3D9)
!= 1)goto _LL2A0;_tmp3DA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3D9)->f2;if(
_tmp3DA <= (void*)1)goto _LL2A0;if(*((int*)_tmp3DA)!= 0)goto _LL2A0;_tmp3DB=((
struct Cyc_Absyn_Global_b_struct*)_tmp3DA)->f1;if(!(!_tmp3DB->escapes))goto _LL2A0;
_LL29F: _tmp3DD=_tmp3DB;goto _LL2A1;_LL2A0: if(*((int*)_tmp3D9)!= 1)goto _LL2A2;
_tmp3DC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3D9)->f2;if(_tmp3DC <= (void*)
1)goto _LL2A2;if(*((int*)_tmp3DC)!= 3)goto _LL2A2;_tmp3DD=((struct Cyc_Absyn_Local_b_struct*)
_tmp3DC)->f1;if(!(!_tmp3DD->escapes))goto _LL2A2;_LL2A1: _tmp3DF=_tmp3DD;goto
_LL2A3;_LL2A2: if(*((int*)_tmp3D9)!= 1)goto _LL2A4;_tmp3DE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3D9)->f2;if(_tmp3DE <= (void*)1)goto _LL2A4;if(*((int*)_tmp3DE)!= 4)goto _LL2A4;
_tmp3DF=((struct Cyc_Absyn_Pat_b_struct*)_tmp3DE)->f1;if(!(!_tmp3DF->escapes))
goto _LL2A4;_LL2A3: _tmp3E1=_tmp3DF;goto _LL2A5;_LL2A4: if(*((int*)_tmp3D9)!= 1)goto
_LL2A6;_tmp3E0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3D9)->f2;if(_tmp3E0 <= (
void*)1)goto _LL2A6;if(*((int*)_tmp3E0)!= 2)goto _LL2A6;_tmp3E1=((struct Cyc_Absyn_Param_b_struct*)
_tmp3E0)->f1;if(!(!_tmp3E1->escapes))goto _LL2A6;_LL2A5: {struct Cyc_List_List*
_tmp3E2=({struct Cyc_List_List*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->vd=
_tmp3D3;_tmp3E7->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp3E9;_tmp3E9.tag=4;_tmp3E9.f1=_tmp3E1;_tmp3E9;});_tmp3E8;}));_tmp3E7;});
_tmp3E6->tl=_tmp334;_tmp3E6;});return({struct _tuple0 _tmp3E3;_tmp3E3.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));
_tmp3E4[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3E5;_tmp3E5.tag=0;
_tmp3E5.f1=_tmp333;_tmp3E5.f2=_tmp3E2;_tmp3E5.f3=_tmp335;_tmp3E5;});_tmp3E4;});
_tmp3E3.f2=_tmp3C4;_tmp3E3;});}_LL2A6:;_LL2A7: return({struct _tuple0 _tmp3EA;
_tmp3EA.f1=_tmp3C3;_tmp3EA.f2=_tmp3C4;_tmp3EA;});_LL29D:;}_LL29B:;_LL29C: return({
struct _tuple0 _tmp3EB;_tmp3EB.f1=_tmp3C3;_tmp3EB.f2=_tmp3C4;_tmp3EB;});_LL298:;}
_LL296:;_LL297: return({struct _tuple0 _tmp3EC;_tmp3EC.f1=_tmp3C3;_tmp3EC.f2=
_tmp3C4;_tmp3EC;});_LL28D:;}}_LL25C:;_LL25D: return({struct _tuple0 _tmp3ED;_tmp3ED.f1=
_tmp32F;_tmp3ED.f2=_tmp32F;_tmp3ED;});_LL24F:;}}_LL21C:;}}_LL213:;_LL214: goto
_LL206;_LL206:;}{void*_tmp3EF;void*_tmp3F0;struct _tuple0 _tmp3EE=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp3EF=_tmp3EE.f1;_tmp3F0=_tmp3EE.f2;_tmp3EF=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp3EF);{void*_tmp3F1=_tmp3EF;struct Cyc_Dict_Dict*_tmp3F2;_LL2A9: if((int)
_tmp3F1 != 0)goto _LL2AB;_LL2AA: return({struct _tuple0 _tmp3F3;_tmp3F3.f1=_tmp3EF;
_tmp3F3.f2=_tmp3EF;_tmp3F3;});_LL2AB: if(_tmp3F1 <= (void*)1)goto _LL2A8;if(*((int*)
_tmp3F1)!= 0)goto _LL2A8;_tmp3F2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp3F1)->f1;_LL2AC: {void*_tmp3F4=_tmp3F0;void*_tmp3F5;void*_tmp3F6;void*
_tmp3F7;_LL2AE: if((int)_tmp3F4 != 0)goto _LL2B0;_LL2AF: return({struct _tuple0
_tmp3F8;_tmp3F8.f1=(void*)0;_tmp3F8.f2=_tmp3EF;_tmp3F8;});_LL2B0: if((int)_tmp3F4
!= 2)goto _LL2B2;_LL2B1: goto _LL2B3;_LL2B2: if((int)_tmp3F4 != 1)goto _LL2B4;_LL2B3:
goto _LL2B5;_LL2B4: if(_tmp3F4 <= (void*)3)goto _LL2B6;if(*((int*)_tmp3F4)!= 2)goto
_LL2B6;_LL2B5: return({struct _tuple0 _tmp3F9;_tmp3F9.f1=_tmp3EF;_tmp3F9.f2=(void*)
0;_tmp3F9;});_LL2B6: if(_tmp3F4 <= (void*)3)goto _LL2B8;if(*((int*)_tmp3F4)!= 0)
goto _LL2B8;_tmp3F5=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3F4)->f1;
if((int)_tmp3F5 != 0)goto _LL2B8;_LL2B7: goto _LL2B9;_LL2B8: if(_tmp3F4 <= (void*)3)
goto _LL2BA;if(*((int*)_tmp3F4)!= 1)goto _LL2BA;_tmp3F6=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp3F4)->f1;if((int)_tmp3F6 != 0)goto _LL2BA;_LL2B9:({void*_tmp3FA[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3FB="expression may not be initialized";_tag_dynforward(
_tmp3FB,sizeof(char),_get_zero_arr_size(_tmp3FB,34));}),_tag_dynforward(_tmp3FA,
sizeof(void*),0));});return({struct _tuple0 _tmp3FC;_tmp3FC.f1=(void*)0;_tmp3FC.f2=(
void*)0;_tmp3FC;});_LL2BA: if(_tmp3F4 <= (void*)3)goto _LL2BC;if(*((int*)_tmp3F4)!= 
0)goto _LL2BC;_tmp3F7=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3F4)->f1;
_LL2BB: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp3EF,e,_tmp3F7);_LL2BC:
if(_tmp3F4 <= (void*)3)goto _LL2BE;if(*((int*)_tmp3F4)!= 1)goto _LL2BE;_LL2BD:
return({struct _tuple0 _tmp3FD;_tmp3FD.f1=_tmp3EF;_tmp3FD.f2=_tmp3EF;_tmp3FD;});
_LL2BE: if(_tmp3F4 <= (void*)3)goto _LL2C0;if(*((int*)_tmp3F4)!= 3)goto _LL2C0;
_LL2BF: return({struct _tuple0 _tmp3FE;_tmp3FE.f1=_tmp3EF;_tmp3FE.f2=_tmp3EF;
_tmp3FE;});_LL2C0: if(_tmp3F4 <= (void*)3)goto _LL2AD;if(*((int*)_tmp3F4)!= 4)goto
_LL2AD;_LL2C1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3FF=
_cycalloc(sizeof(*_tmp3FF));_tmp3FF[0]=({struct Cyc_Core_Impossible_struct _tmp400;
_tmp400.tag=Cyc_Core_Impossible;_tmp400.f1=({const char*_tmp401="anal_test";
_tag_dynforward(_tmp401,sizeof(char),_get_zero_arr_size(_tmp401,10));});_tmp400;});
_tmp3FF;}));_LL2AD:;}_LL2A8:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,void*flow){
void*_tmp402=flow;struct Cyc_Dict_Dict*_tmp403;_LL2C3: if((int)_tmp402 != 0)goto
_LL2C5;_LL2C4: return;_LL2C5: if(_tmp402 <= (void*)1)goto _LL2C2;if(*((int*)_tmp402)
!= 0)goto _LL2C2;_tmp403=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp402)->f1;
_LL2C6:{struct Cyc_List_List*_tmp404=env->param_roots;for(0;_tmp404 != 0;_tmp404=
_tmp404->tl){if(Cyc_CfFlowInfo_initlevel(_tmp403,Cyc_CfFlowInfo_lookup_place(
_tmp403,(struct Cyc_CfFlowInfo_Place*)_tmp404->hd))!= (void*)2)({void*_tmp405[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp406="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp406,sizeof(char),_get_zero_arr_size(_tmp406,76));}),
_tag_dynforward(_tmp405,sizeof(void*),0));});}}return;_LL2C2:;}static void*Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_List_List*scs){for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp408;struct Cyc_Core_Opt*
_tmp409;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_Absyn_Stmt*_tmp40B;struct Cyc_Absyn_Switch_clause*
_tmp407=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp408=*_tmp407;_tmp409=_tmp408.pat_vars;
_tmp40A=_tmp408.where_clause;_tmp40B=_tmp408.body;{void*clause_inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp409))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp40A != 0){void*_tmp40D;void*_tmp40E;struct _tuple0
_tmp40C=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_tmp40A);_tmp40D=_tmp40C.f1;_tmp40E=_tmp40C.f2;inflow=_tmp40E;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp40D,_tmp40B);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp40B);}{void*_tmp40F=clause_outflow;_LL2C8: if((int)_tmp40F != 0)
goto _LL2CA;_LL2C9: goto _LL2C7;_LL2CA:;_LL2CB: if(scs->tl == 0)return clause_outflow;
else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*
_tmp410[0]={};Cyc_Tcutil_terr(_tmp40B->loc,({const char*_tmp411="switch clause may implicitly fallthru";
_tag_dynforward(_tmp411,sizeof(char),_get_zero_arr_size(_tmp411,38));}),
_tag_dynforward(_tmp410,sizeof(void*),0));});else{({void*_tmp412[0]={};Cyc_Tcutil_warn(
_tmp40B->loc,({const char*_tmp413="switch clause may implicitly fallthru";
_tag_dynforward(_tmp413,sizeof(char),_get_zero_arr_size(_tmp413,38));}),
_tag_dynforward(_tmp412,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((
struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL2C7;_LL2C7:;}}}return(void*)0;}static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){void*
outflow;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp414=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);inflow=(void*)_tmp414->flow;{void*_tmp415=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp416;struct Cyc_Absyn_Exp*_tmp417;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Stmt*
_tmp419;struct Cyc_Absyn_Stmt*_tmp41A;struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_Absyn_Stmt*
_tmp41C;struct Cyc_Absyn_Stmt*_tmp41D;struct _tuple3 _tmp41E;struct Cyc_Absyn_Exp*
_tmp41F;struct Cyc_Absyn_Stmt*_tmp420;struct Cyc_Absyn_Stmt*_tmp421;struct Cyc_Absyn_Stmt*
_tmp422;struct _tuple3 _tmp423;struct Cyc_Absyn_Exp*_tmp424;struct Cyc_Absyn_Stmt*
_tmp425;struct Cyc_Absyn_Exp*_tmp426;struct _tuple3 _tmp427;struct Cyc_Absyn_Exp*
_tmp428;struct Cyc_Absyn_Stmt*_tmp429;struct _tuple3 _tmp42A;struct Cyc_Absyn_Exp*
_tmp42B;struct Cyc_Absyn_Stmt*_tmp42C;struct Cyc_Absyn_Stmt*_tmp42D;struct Cyc_Absyn_Stmt*
_tmp42E;struct Cyc_List_List*_tmp42F;struct Cyc_Absyn_Switch_clause**_tmp430;
struct Cyc_Absyn_Switch_clause*_tmp431;struct Cyc_Absyn_Stmt*_tmp432;struct Cyc_Absyn_Stmt*
_tmp433;struct Cyc_Absyn_Stmt*_tmp434;struct Cyc_Absyn_Exp*_tmp435;struct Cyc_List_List*
_tmp436;struct Cyc_Absyn_Stmt*_tmp437;struct Cyc_List_List*_tmp438;struct Cyc_Absyn_Decl*
_tmp439;struct Cyc_Absyn_Stmt*_tmp43A;struct Cyc_Absyn_Stmt*_tmp43B;struct Cyc_Absyn_Tvar*
_tmp43C;struct Cyc_Absyn_Vardecl*_tmp43D;int _tmp43E;struct Cyc_Absyn_Stmt*_tmp43F;
struct Cyc_Absyn_Exp*_tmp440;_LL2CD: if((int)_tmp415 != 0)goto _LL2CF;_LL2CE: return
inflow;_LL2CF: if(_tmp415 <= (void*)1)goto _LL2F5;if(*((int*)_tmp415)!= 2)goto
_LL2D1;_tmp416=((struct Cyc_Absyn_Return_s_struct*)_tmp415)->f1;if(_tmp416 != 0)
goto _LL2D1;_LL2D0: if(env->noreturn)({void*_tmp441[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp442="`noreturn' function might return";_tag_dynforward(_tmp442,
sizeof(char),_get_zero_arr_size(_tmp442,33));}),_tag_dynforward(_tmp441,sizeof(
void*),0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return(
void*)0;_LL2D1: if(*((int*)_tmp415)!= 2)goto _LL2D3;_tmp417=((struct Cyc_Absyn_Return_s_struct*)
_tmp415)->f1;_LL2D2: if(env->noreturn)({void*_tmp443[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp444="`noreturn' function might return";_tag_dynforward(_tmp444,
sizeof(char),_get_zero_arr_size(_tmp444,33));}),_tag_dynforward(_tmp443,sizeof(
void*),0));});{void*_tmp446;void*_tmp447;struct _tuple0 _tmp445=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp417));_tmp446=_tmp445.f1;
_tmp447=_tmp445.f2;_tmp446=Cyc_CfFlowInfo_consume_unique_rvals(_tmp417->loc,
_tmp446);_tmp446=Cyc_NewControlFlow_use_Rval(env,_tmp417->loc,_tmp446,_tmp447);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp446);return(void*)0;}_LL2D3:
if(*((int*)_tmp415)!= 0)goto _LL2D5;_tmp418=((struct Cyc_Absyn_Exp_s_struct*)
_tmp415)->f1;_LL2D4: outflow=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp418)).f1;
goto _LL2CC;_LL2D5: if(*((int*)_tmp415)!= 1)goto _LL2D7;_tmp419=((struct Cyc_Absyn_Seq_s_struct*)
_tmp415)->f1;_tmp41A=((struct Cyc_Absyn_Seq_s_struct*)_tmp415)->f2;_LL2D6: outflow=
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp419),
_tmp41A);goto _LL2CC;_LL2D7: if(*((int*)_tmp415)!= 3)goto _LL2D9;_tmp41B=((struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp415)->f1;_tmp41C=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp415)->f2;_tmp41D=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp415)->f3;_LL2D8: {
void*_tmp449;void*_tmp44A;struct _tuple0 _tmp448=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp41B);_tmp449=_tmp448.f1;_tmp44A=_tmp448.f2;_tmp449=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp41B->loc,_tmp449);_tmp44A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp41B->loc,
_tmp44A);outflow=Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp449,_tmp41C),Cyc_NewControlFlow_anal_stmt(env,_tmp44A,_tmp41D),1);goto
_LL2CC;}_LL2D9: if(*((int*)_tmp415)!= 4)goto _LL2DB;_tmp41E=((struct Cyc_Absyn_While_s_struct*)
_tmp415)->f1;_tmp41F=_tmp41E.f1;_tmp420=_tmp41E.f2;_tmp421=((struct Cyc_Absyn_While_s_struct*)
_tmp415)->f2;_LL2DA: {void*_tmp44B=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
inflow,_tmp420))->flow;void*_tmp44D;void*_tmp44E;struct _tuple0 _tmp44C=Cyc_NewControlFlow_anal_test(
env,_tmp44B,_tmp41F);_tmp44D=_tmp44C.f1;_tmp44E=_tmp44C.f2;_tmp44D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp41F->loc,_tmp44D);_tmp44E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp41F->loc,
_tmp44E);{void*_tmp44F=Cyc_NewControlFlow_anal_stmt(env,_tmp44D,_tmp421);Cyc_NewControlFlow_update_flow(
env,_tmp420,_tmp44F);outflow=_tmp44E;goto _LL2CC;}}_LL2DB: if(*((int*)_tmp415)!= 
13)goto _LL2DD;_tmp422=((struct Cyc_Absyn_Do_s_struct*)_tmp415)->f1;_tmp423=((
struct Cyc_Absyn_Do_s_struct*)_tmp415)->f2;_tmp424=_tmp423.f1;_tmp425=_tmp423.f2;
_LL2DC: {void*_tmp450=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp422);void*
_tmp451=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp450,_tmp425))->flow;
void*_tmp453;void*_tmp454;struct _tuple0 _tmp452=Cyc_NewControlFlow_anal_test(env,
_tmp451,_tmp424);_tmp453=_tmp452.f1;_tmp454=_tmp452.f2;Cyc_NewControlFlow_update_flow(
env,_tmp422,_tmp453);outflow=_tmp454;goto _LL2CC;}_LL2DD: if(*((int*)_tmp415)!= 8)
goto _LL2DF;_tmp426=((struct Cyc_Absyn_For_s_struct*)_tmp415)->f1;_tmp427=((struct
Cyc_Absyn_For_s_struct*)_tmp415)->f2;_tmp428=_tmp427.f1;_tmp429=_tmp427.f2;
_tmp42A=((struct Cyc_Absyn_For_s_struct*)_tmp415)->f3;_tmp42B=_tmp42A.f1;_tmp42C=
_tmp42A.f2;_tmp42D=((struct Cyc_Absyn_For_s_struct*)_tmp415)->f4;_LL2DE: {void*
_tmp455=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp426)).f1;_tmp455=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp426->loc,_tmp455);{void*_tmp456=(void*)(Cyc_NewControlFlow_pre_stmt_check(
env,_tmp455,_tmp429))->flow;void*_tmp458;void*_tmp459;struct _tuple0 _tmp457=Cyc_NewControlFlow_anal_test(
env,_tmp456,_tmp428);_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;{void*_tmp45A=Cyc_NewControlFlow_anal_stmt(
env,_tmp458,_tmp42D);void*_tmp45B=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,
_tmp45A,_tmp42C))->flow;void*_tmp45C=(Cyc_NewControlFlow_anal_Rexp(env,_tmp45B,
_tmp42B)).f1;_tmp45C=Cyc_CfFlowInfo_drop_unique_rvals(_tmp42B->loc,_tmp45C);Cyc_NewControlFlow_update_flow(
env,_tmp429,_tmp45C);outflow=_tmp459;goto _LL2CC;}}}_LL2DF: if(*((int*)_tmp415)!= 
5)goto _LL2E1;_tmp42E=((struct Cyc_Absyn_Break_s_struct*)_tmp415)->f1;if(_tmp42E != 
0)goto _LL2E1;_LL2E0: return(void*)0;_LL2E1: if(*((int*)_tmp415)!= 10)goto _LL2E3;
_tmp42F=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp415)->f1;_tmp430=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp415)->f2;if(_tmp430 == 0)goto _LL2E3;_tmp431=*_tmp430;_LL2E2: {void*_tmp45E;
struct Cyc_List_List*_tmp45F;struct _tuple6 _tmp45D=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp42F,0);_tmp45E=_tmp45D.f1;_tmp45F=_tmp45D.f2;for(0;_tmp45F != 0;(
_tmp45F=_tmp45F->tl,_tmp42F=_tmp42F->tl)){_tmp45E=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42F))->hd)->loc,
_tmp45E,(void*)_tmp45F->hd);}_tmp45E=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,
_tmp45E);_tmp45E=Cyc_NewControlFlow_add_vars(_tmp45E,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp431->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp431->body,_tmp45E);
return(void*)0;}_LL2E3: if(*((int*)_tmp415)!= 5)goto _LL2E5;_tmp432=((struct Cyc_Absyn_Break_s_struct*)
_tmp415)->f1;_LL2E4: _tmp433=_tmp432;goto _LL2E6;_LL2E5: if(*((int*)_tmp415)!= 6)
goto _LL2E7;_tmp433=((struct Cyc_Absyn_Continue_s_struct*)_tmp415)->f1;_LL2E6:
_tmp434=_tmp433;goto _LL2E8;_LL2E7: if(*((int*)_tmp415)!= 7)goto _LL2E9;_tmp434=((
struct Cyc_Absyn_Goto_s_struct*)_tmp415)->f2;_LL2E8: if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp460=_tmp414->encloser;struct Cyc_Absyn_Stmt*_tmp461=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp434)))->encloser;while(_tmp461 != _tmp460){
struct Cyc_Absyn_Stmt*_tmp462=(Cyc_NewControlFlow_get_stmt_annot(_tmp460))->encloser;
if(_tmp462 == _tmp460){({void*_tmp463[0]={};Cyc_Tcutil_terr(s->loc,({const char*
_tmp464="goto enters local scope or exception handler";_tag_dynforward(_tmp464,
sizeof(char),_get_zero_arr_size(_tmp464,45));}),_tag_dynforward(_tmp463,sizeof(
void*),0));});break;}_tmp460=_tmp462;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp434),inflow);return(void*)0;_LL2E9: if(*((
int*)_tmp415)!= 9)goto _LL2EB;_tmp435=((struct Cyc_Absyn_Switch_s_struct*)_tmp415)->f1;
_tmp436=((struct Cyc_Absyn_Switch_s_struct*)_tmp415)->f2;_LL2EA: {void*_tmp466;
void*_tmp467;struct _tuple0 _tmp465=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp435);_tmp466=_tmp465.f1;_tmp467=_tmp465.f2;_tmp466=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp435->loc,_tmp466);_tmp466=Cyc_NewControlFlow_use_Rval(env,_tmp435->loc,
_tmp466,_tmp467);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp466,_tmp436);goto
_LL2CC;}_LL2EB: if(*((int*)_tmp415)!= 14)goto _LL2ED;_tmp437=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp415)->f1;_tmp438=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp415)->f2;_LL2EC: {
int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;env->in_try=1;(
void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp437);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp438);{
void*_tmp468=scs_outflow;_LL2F8: if((int)_tmp468 != 0)goto _LL2FA;_LL2F9: goto _LL2F7;
_LL2FA:;_LL2FB:({void*_tmp469[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp46A="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp46A,sizeof(char),_get_zero_arr_size(_tmp46A,42));}),
_tag_dynforward(_tmp469,sizeof(void*),0));});_LL2F7:;}outflow=s1_outflow;goto
_LL2CC;}}}_LL2ED: if(*((int*)_tmp415)!= 11)goto _LL2EF;_tmp439=((struct Cyc_Absyn_Decl_s_struct*)
_tmp415)->f1;_tmp43A=((struct Cyc_Absyn_Decl_s_struct*)_tmp415)->f2;_LL2EE:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp439),_tmp43A);goto _LL2CC;_LL2EF: if(*((int*)_tmp415)!= 12)goto _LL2F1;_tmp43B=((
struct Cyc_Absyn_Label_s_struct*)_tmp415)->f2;_LL2F0: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp43B);goto _LL2CC;_LL2F1: if(*((int*)_tmp415)!= 15)goto _LL2F3;
_tmp43C=((struct Cyc_Absyn_Region_s_struct*)_tmp415)->f1;_tmp43D=((struct Cyc_Absyn_Region_s_struct*)
_tmp415)->f2;_tmp43E=((struct Cyc_Absyn_Region_s_struct*)_tmp415)->f3;_tmp43F=((
struct Cyc_Absyn_Region_s_struct*)_tmp415)->f4;_LL2F2: {struct Cyc_List_List
_tmp46B=({struct Cyc_List_List _tmp46C;_tmp46C.hd=_tmp43D;_tmp46C.tl=0;_tmp46C;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp46B,Cyc_CfFlowInfo_unknown_all);
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp43F);goto _LL2CC;}_LL2F3: if(*((
int*)_tmp415)!= 16)goto _LL2F5;_tmp440=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp415)->f1;_LL2F4: {void*_tmp46E;void*_tmp46F;struct _tuple0 _tmp46D=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp440);_tmp46E=_tmp46D.f1;_tmp46F=_tmp46D.f2;_tmp46E=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp440->loc,_tmp46E);{void*_tmp470=Cyc_NewControlFlow_use_Rval(env,_tmp440->loc,
_tmp46E,_tmp46F);{void*_tmp471=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp440->topt))->v);void*_tmp472;_LL2FD: if(_tmp471 <= (void*)4)goto
_LL2FF;if(*((int*)_tmp471)!= 15)goto _LL2FF;_tmp472=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp471)->f1;_LL2FE: outflow=Cyc_CfFlowInfo_kill_flow_region(_tmp46E,_tmp472);
goto _LL2FC;_LL2FF:;_LL300:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473[0]=({struct Cyc_Core_Impossible_struct
_tmp474;_tmp474.tag=Cyc_Core_Impossible;_tmp474.f1=({const char*_tmp475="anal_stmt -- reset_region";
_tag_dynforward(_tmp475,sizeof(char),_get_zero_arr_size(_tmp475,26));});_tmp474;});
_tmp473;}));_LL2FC:;}goto _LL2CC;}}_LL2F5:;_LL2F6:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476[0]=({struct Cyc_Core_Impossible_struct
_tmp477;_tmp477.tag=Cyc_Core_Impossible;_tmp477.f1=({const char*_tmp478="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp478,sizeof(char),_get_zero_arr_size(_tmp478,56));});_tmp477;});
_tmp476;}));_LL2CC:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{
void*_tmp479=outflow;struct Cyc_CfFlowInfo_ConsumeInfo _tmp47A;_LL302: if(_tmp479 <= (
void*)1)goto _LL304;if(*((int*)_tmp479)!= 0)goto _LL304;_tmp47A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp479)->f3;_LL303: goto _LL301;_LL304:;_LL305: goto _LL301;_LL301:;}return outflow;}
static void Cyc_NewControlFlow_check_nested_fun(void*inflow,struct Cyc_Absyn_Fndecl*
fd);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,
void*inflow,struct Cyc_Absyn_Decl*decl){void*_tmp47B=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp47C;struct Cyc_Core_Opt*_tmp47D;struct Cyc_Core_Opt _tmp47E;struct Cyc_List_List*
_tmp47F;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_Fndecl*
_tmp482;_LL307: if(_tmp47B <= (void*)2)goto _LL30F;if(*((int*)_tmp47B)!= 0)goto
_LL309;_tmp47C=((struct Cyc_Absyn_Var_d_struct*)_tmp47B)->f1;_LL308: {struct Cyc_List_List
_tmp483=({struct Cyc_List_List _tmp492;_tmp492.hd=_tmp47C;_tmp492.tl=0;_tmp492;});
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)& _tmp483,Cyc_CfFlowInfo_unknown_none);{
struct Cyc_Absyn_Exp*_tmp484=_tmp47C->initializer;if(_tmp484 == 0)return inflow;{
void*_tmp486;void*_tmp487;struct _tuple0 _tmp485=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp484);_tmp486=_tmp485.f1;_tmp487=_tmp485.f2;
_tmp486=Cyc_CfFlowInfo_consume_unique_rvals(_tmp484->loc,_tmp486);{void*_tmp488=
_tmp486;struct Cyc_Dict_Dict*_tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp48B;_LL312: if((int)_tmp488 != 0)goto _LL314;_LL313: return(void*)0;_LL314: if(
_tmp488 <= (void*)1)goto _LL311;if(*((int*)_tmp488)!= 0)goto _LL311;_tmp489=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp488)->f1;_tmp48A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp488)->f2;_tmp48B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp488)->f3;
_LL315: _tmp489=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp489,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp48D=_cycalloc(sizeof(*
_tmp48D));_tmp48D[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp48E;_tmp48E.tag=0;
_tmp48E.f1=_tmp47C;_tmp48E;});_tmp48D;}));_tmp48C->fields=0;_tmp48C;}),_tmp487);
_tmp48A=Cyc_CfFlowInfo_reln_assign_var(_tmp48A,_tmp47C,(struct Cyc_Absyn_Exp*)
_check_null(_tmp47C->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp48F=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp490=_cycalloc(sizeof(*
_tmp490));_tmp490[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp491;_tmp491.tag=
0;_tmp491.f1=_tmp489;_tmp491.f2=_tmp48A;_tmp491.f3=_tmp48B;_tmp491;});_tmp490;});
Cyc_NewControlFlow_update_tryflow(env,(void*)_tmp48F);return(void*)_tmp48F;}
_LL311:;}}}}_LL309: if(*((int*)_tmp47B)!= 2)goto _LL30B;_tmp47D=((struct Cyc_Absyn_Let_d_struct*)
_tmp47B)->f2;if(_tmp47D == 0)goto _LL30B;_tmp47E=*_tmp47D;_tmp47F=(struct Cyc_List_List*)
_tmp47E.v;_tmp480=((struct Cyc_Absyn_Let_d_struct*)_tmp47B)->f3;_LL30A: {void*
_tmp494;void*_tmp495;struct _tuple0 _tmp493=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp480);_tmp494=_tmp493.f1;_tmp495=_tmp493.f2;_tmp494=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp480->loc,_tmp494);_tmp494=Cyc_NewControlFlow_use_Rval(env,_tmp480->loc,
_tmp494,_tmp495);return Cyc_NewControlFlow_add_vars(_tmp494,_tmp47F,Cyc_CfFlowInfo_unknown_all);}
_LL30B: if(*((int*)_tmp47B)!= 3)goto _LL30D;_tmp481=((struct Cyc_Absyn_Letv_d_struct*)
_tmp47B)->f1;_LL30C: return Cyc_NewControlFlow_add_vars(inflow,_tmp481,Cyc_CfFlowInfo_unknown_none);
_LL30D: if(*((int*)_tmp47B)!= 1)goto _LL30F;_tmp482=((struct Cyc_Absyn_Fn_d_struct*)
_tmp47B)->f1;_LL30E: Cyc_NewControlFlow_check_nested_fun(inflow,_tmp482);{void*
_tmp496=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp482->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp497=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp482->fn_vardecl);
return Cyc_NewControlFlow_add_vars(inflow,({struct Cyc_List_List*_tmp498=_cycalloc(
sizeof(*_tmp498));_tmp498->hd=_tmp497;_tmp498->tl=0;_tmp498;}),Cyc_CfFlowInfo_unknown_all);}
_LL30F:;_LL310:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp499=
_cycalloc(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_Core_Impossible_struct _tmp49A;
_tmp49A.tag=Cyc_Core_Impossible;_tmp49A.f1=({const char*_tmp49B="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp49B,sizeof(char),_get_zero_arr_size(_tmp49B,35));});_tmp49A;});
_tmp499;}));_LL306:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp49C=(void*)s->r;
struct _tuple3 _tmp49D;struct Cyc_Absyn_Stmt*_tmp49E;struct _tuple3 _tmp49F;struct Cyc_Absyn_Stmt*
_tmp4A0;struct Cyc_Absyn_Stmt*_tmp4A1;struct Cyc_Absyn_Stmt*_tmp4A2;struct Cyc_Absyn_Stmt*
_tmp4A3;struct _tuple3 _tmp4A4;struct Cyc_Absyn_Stmt*_tmp4A5;struct Cyc_Absyn_Stmt*
_tmp4A6;struct Cyc_Absyn_Stmt*_tmp4A7;struct _tuple3 _tmp4A8;struct Cyc_Absyn_Stmt*
_tmp4A9;struct Cyc_Absyn_Stmt*_tmp4AA;struct Cyc_Absyn_Stmt*_tmp4AB;struct Cyc_Absyn_Stmt*
_tmp4AC;struct Cyc_Absyn_Stmt*_tmp4AD;struct Cyc_Absyn_Stmt*_tmp4AE;struct Cyc_Absyn_Stmt*
_tmp4AF;struct Cyc_List_List*_tmp4B0;struct Cyc_List_List*_tmp4B1;_LL317: if(
_tmp49C <= (void*)1)goto _LL32B;if(*((int*)_tmp49C)!= 8)goto _LL319;_tmp49D=((
struct Cyc_Absyn_For_s_struct*)_tmp49C)->f2;_tmp49E=_tmp49D.f2;_tmp49F=((struct
Cyc_Absyn_For_s_struct*)_tmp49C)->f3;_tmp4A0=_tmp49F.f2;_tmp4A1=((struct Cyc_Absyn_For_s_struct*)
_tmp49C)->f4;_LL318: Cyc_NewControlFlow_postpass(_tmp4A0);_tmp4A2=_tmp4A1;_tmp4A3=
_tmp49E;goto _LL31A;_LL319: if(*((int*)_tmp49C)!= 1)goto _LL31B;_tmp4A2=((struct Cyc_Absyn_Seq_s_struct*)
_tmp49C)->f1;_tmp4A3=((struct Cyc_Absyn_Seq_s_struct*)_tmp49C)->f2;_LL31A: _tmp4A5=
_tmp4A2;_tmp4A6=_tmp4A3;goto _LL31C;_LL31B: if(*((int*)_tmp49C)!= 4)goto _LL31D;
_tmp4A4=((struct Cyc_Absyn_While_s_struct*)_tmp49C)->f1;_tmp4A5=_tmp4A4.f2;
_tmp4A6=((struct Cyc_Absyn_While_s_struct*)_tmp49C)->f2;_LL31C: _tmp4A7=_tmp4A6;
_tmp4A9=_tmp4A5;goto _LL31E;_LL31D: if(*((int*)_tmp49C)!= 13)goto _LL31F;_tmp4A7=((
struct Cyc_Absyn_Do_s_struct*)_tmp49C)->f1;_tmp4A8=((struct Cyc_Absyn_Do_s_struct*)
_tmp49C)->f2;_tmp4A9=_tmp4A8.f2;_LL31E: _tmp4AA=_tmp4A7;_tmp4AB=_tmp4A9;goto
_LL320;_LL31F: if(*((int*)_tmp49C)!= 3)goto _LL321;_tmp4AA=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp49C)->f2;_tmp4AB=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp49C)->f3;_LL320:
Cyc_NewControlFlow_postpass(_tmp4AA);_tmp4AC=_tmp4AB;goto _LL322;_LL321: if(*((int*)
_tmp49C)!= 15)goto _LL323;_tmp4AC=((struct Cyc_Absyn_Region_s_struct*)_tmp49C)->f4;
_LL322: _tmp4AD=_tmp4AC;goto _LL324;_LL323: if(*((int*)_tmp49C)!= 12)goto _LL325;
_tmp4AD=((struct Cyc_Absyn_Label_s_struct*)_tmp49C)->f2;_LL324: _tmp4AE=_tmp4AD;
goto _LL326;_LL325: if(*((int*)_tmp49C)!= 11)goto _LL327;_tmp4AE=((struct Cyc_Absyn_Decl_s_struct*)
_tmp49C)->f2;_LL326: Cyc_NewControlFlow_postpass(_tmp4AE);return;_LL327: if(*((int*)
_tmp49C)!= 14)goto _LL329;_tmp4AF=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp49C)->f1;
_tmp4B0=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp49C)->f2;_LL328: Cyc_NewControlFlow_postpass(
_tmp4AF);_tmp4B1=_tmp4B0;goto _LL32A;_LL329: if(*((int*)_tmp49C)!= 9)goto _LL32B;
_tmp4B1=((struct Cyc_Absyn_Switch_s_struct*)_tmp49C)->f2;_LL32A: for(0;_tmp4B1 != 0;
_tmp4B1=_tmp4B1->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp4B1->hd)->body);}return;_LL32B:;_LL32C: return;_LL316:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){Cyc_NewControlFlow_check_nested_fun((void*)({struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp4B3;_tmp4B3.tag=0;_tmp4B3.f1=Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp);_tmp4B3.f2=0;_tmp4B3.f3=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4B4;_tmp4B4.consumed=Cyc_CfFlowInfo_mt_place_set();_tmp4B4.may_consume=0;
_tmp4B4;});_tmp4B3;});_tmp4B2;}),fd);}static void Cyc_NewControlFlow_check_nested_fun(
void*inflow,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Position_Segment*_tmp4B5=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,Cyc_CfFlowInfo_unknown_all);{struct Cyc_List_List*
_tmp4B6=0;{void*_tmp4B7=inflow;struct Cyc_Dict_Dict*_tmp4B8;struct Cyc_List_List*
_tmp4B9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4BA;_LL32E: if((int)_tmp4B7 != 0)goto
_LL330;_LL32F:({void*_tmp4BB[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4BC="check_fun";
_tag_dynforward(_tmp4BC,sizeof(char),_get_zero_arr_size(_tmp4BC,10));}),
_tag_dynforward(_tmp4BB,sizeof(void*),0));});_LL330: if(_tmp4B7 <= (void*)1)goto
_LL32D;if(*((int*)_tmp4B7)!= 0)goto _LL32D;_tmp4B8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp4B7)->f1;_tmp4B9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B7)->f2;
_tmp4BA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4B7)->f3;_LL331: {struct
Cyc_List_List*atts;{void*_tmp4BD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp4BE;struct Cyc_List_List*
_tmp4BF;_LL333: if(_tmp4BD <= (void*)4)goto _LL335;if(*((int*)_tmp4BD)!= 8)goto
_LL335;_tmp4BE=((struct Cyc_Absyn_FnType_struct*)_tmp4BD)->f1;_tmp4BF=_tmp4BE.attributes;
_LL334: atts=_tmp4BF;goto _LL332;_LL335:;_LL336:({void*_tmp4C0[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4C1="check_fun: non-function type cached with fndecl_t";_tag_dynforward(
_tmp4C1,sizeof(char),_get_zero_arr_size(_tmp4C1,50));}),_tag_dynforward(_tmp4C0,
sizeof(void*),0));});_LL332:;}for(0;atts != 0;atts=atts->tl){void*_tmp4C2=(void*)
atts->hd;int _tmp4C3;_LL338: if(_tmp4C2 <= (void*)17)goto _LL33A;if(*((int*)_tmp4C2)
!= 4)goto _LL33A;_tmp4C3=((struct Cyc_Absyn_Initializes_att_struct*)_tmp4C2)->f1;
_LL339: {unsigned int j=(unsigned int)_tmp4C3;if(j > ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp4C4[0]={};Cyc_Tcutil_terr(_tmp4B5,({const char*_tmp4C5="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp4C5,sizeof(char),_get_zero_arr_size(_tmp4C5,51));}),
_tag_dynforward(_tmp4C4,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp4C6=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp4C7=Cyc_Tcutil_compress((void*)_tmp4C6->type);struct Cyc_Absyn_PtrInfo
_tmp4C8;void*_tmp4C9;struct Cyc_Absyn_PtrAtts _tmp4CA;struct Cyc_Absyn_Conref*
_tmp4CB;struct Cyc_Absyn_Conref*_tmp4CC;struct Cyc_Absyn_Conref*_tmp4CD;_LL33D: if(
_tmp4C7 <= (void*)4)goto _LL33F;if(*((int*)_tmp4C7)!= 4)goto _LL33F;_tmp4C8=((
struct Cyc_Absyn_PointerType_struct*)_tmp4C7)->f1;_tmp4C9=(void*)_tmp4C8.elt_typ;
_tmp4CA=_tmp4C8.ptr_atts;_tmp4CB=_tmp4CA.nullable;_tmp4CC=_tmp4CA.bounds;_tmp4CD=
_tmp4CA.zero_term;_LL33E: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp4CB))({void*_tmp4CE[0]={};Cyc_Tcutil_terr(_tmp4B5,({const char*_tmp4CF="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp4CF,sizeof(char),_get_zero_arr_size(_tmp4CF,55));}),
_tag_dynforward(_tmp4CE,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp4CC))({
void*_tmp4D0[0]={};Cyc_Tcutil_terr(_tmp4B5,({const char*_tmp4D1="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp4D1,sizeof(char),_get_zero_arr_size(_tmp4D1,57));}),
_tag_dynforward(_tmp4D0,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp4CD))({void*_tmp4D2[0]={};Cyc_Tcutil_terr(_tmp4B5,({
const char*_tmp4D3="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp4D3,sizeof(char),_get_zero_arr_size(_tmp4D3,77));}),
_tag_dynforward(_tmp4D2,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp4D4=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp4DC=_cycalloc(sizeof(*
_tmp4DC));_tmp4DC[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp4DD;_tmp4DD.tag=
2;_tmp4DD.f1=(int)j;_tmp4DD.f2=(void*)_tmp4C9;_tmp4DD;});_tmp4DC;});struct Cyc_CfFlowInfo_Place*
_tmp4D5=({struct Cyc_CfFlowInfo_Place*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->root=(
void*)((void*)_tmp4D4);_tmp4DB->fields=0;_tmp4DB;});_tmp4B8=Cyc_Dict_insert(
_tmp4B8,(void*)_tmp4D4,Cyc_CfFlowInfo_typ_to_absrval(_tmp4C9,Cyc_CfFlowInfo_esc_none));
_tmp4B8=Cyc_Dict_insert(_tmp4B8,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp4D7;_tmp4D7.tag=0;_tmp4D7.f1=_tmp4C6;_tmp4D7;});_tmp4D6;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp4D9;_tmp4D9.tag=2;_tmp4D9.f1=_tmp4D5;
_tmp4D9;});_tmp4D8;}));_tmp4B6=({struct Cyc_List_List*_tmp4DA=_cycalloc(sizeof(*
_tmp4DA));_tmp4DA->hd=_tmp4D5;_tmp4DA->tl=_tmp4B6;_tmp4DA;});goto _LL33C;}_LL33F:;
_LL340:({void*_tmp4DE[0]={};Cyc_Tcutil_terr(_tmp4B5,({const char*_tmp4DF="initializes attribute on non-pointer";
_tag_dynforward(_tmp4DF,sizeof(char),_get_zero_arr_size(_tmp4DF,37));}),
_tag_dynforward(_tmp4DE,sizeof(void*),0));});_LL33C:;}goto _LL337;}}_LL33A:;
_LL33B: goto _LL337;_LL337:;}inflow=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp4E1;_tmp4E1.tag=0;_tmp4E1.f1=_tmp4B8;_tmp4E1.f2=_tmp4B9;_tmp4E1.f3=_tmp4BA;
_tmp4E1;});_tmp4E0;});}_LL32D:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->iterate_again=1;_tmp4EA->iteration_num=
0;_tmp4EA->in_try=0;_tmp4EA->tryflow=(void*)inflow;_tmp4EA->all_changed=0;
_tmp4EA->noreturn=noreturn;_tmp4EA->param_roots=_tmp4B6;_tmp4EA;});void*outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{void*_tmp4E2=
outflow;_LL342: if((int)_tmp4E2 != 0)goto _LL344;_LL343: goto _LL341;_LL344:;_LL345:
Cyc_NewControlFlow_check_init_params(_tmp4B5,env,outflow);if(noreturn)({void*
_tmp4E3[0]={};Cyc_Tcutil_terr(_tmp4B5,({const char*_tmp4E4="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp4E4,sizeof(char),_get_zero_arr_size(_tmp4E4,46));}),
_tag_dynforward(_tmp4E3,sizeof(void*),0));});else{void*_tmp4E5=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL347: if((int)_tmp4E5 != 0)goto _LL349;_LL348: goto _LL346;
_LL349: if(_tmp4E5 <= (void*)4)goto _LL34B;if(*((int*)_tmp4E5)!= 6)goto _LL34B;
_LL34A: goto _LL34C;_LL34B: if((int)_tmp4E5 != 1)goto _LL34D;_LL34C: goto _LL34E;_LL34D:
if(_tmp4E5 <= (void*)4)goto _LL34F;if(*((int*)_tmp4E5)!= 5)goto _LL34F;_LL34E:({
void*_tmp4E6[0]={};Cyc_Tcutil_warn(_tmp4B5,({const char*_tmp4E7="function may not return a value";
_tag_dynforward(_tmp4E7,sizeof(char),_get_zero_arr_size(_tmp4E7,32));}),
_tag_dynforward(_tmp4E6,sizeof(void*),0));});goto _LL346;_LL34F:;_LL350:({void*
_tmp4E8[0]={};Cyc_Tcutil_terr(_tmp4B5,({const char*_tmp4E9="function may not return a value";
_tag_dynforward(_tmp4E9,sizeof(char),_get_zero_arr_size(_tmp4E9,32));}),
_tag_dynforward(_tmp4E8,sizeof(void*),0));});goto _LL346;_LL346:;}goto _LL341;
_LL341:;}Cyc_NewControlFlow_postpass(fd->body);}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp4EB=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp4EC;struct Cyc_List_List*_tmp4ED;struct Cyc_List_List*
_tmp4EE;struct Cyc_List_List*_tmp4EF;_LL352: if(_tmp4EB <= (void*)2)goto _LL35C;if(*((
int*)_tmp4EB)!= 1)goto _LL354;_tmp4EC=((struct Cyc_Absyn_Fn_d_struct*)_tmp4EB)->f1;
_LL353: Cyc_NewControlFlow_check_fun(_tmp4EC);goto _LL351;_LL354: if(*((int*)
_tmp4EB)!= 10)goto _LL356;_tmp4ED=((struct Cyc_Absyn_ExternC_d_struct*)_tmp4EB)->f1;
_LL355: _tmp4EE=_tmp4ED;goto _LL357;_LL356: if(*((int*)_tmp4EB)!= 9)goto _LL358;
_tmp4EE=((struct Cyc_Absyn_Using_d_struct*)_tmp4EB)->f2;_LL357: _tmp4EF=_tmp4EE;
goto _LL359;_LL358: if(*((int*)_tmp4EB)!= 8)goto _LL35A;_tmp4EF=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp4EB)->f2;_LL359: Cyc_NewControlFlow_cf_check(_tmp4EF);goto _LL351;_LL35A: if(*((
int*)_tmp4EB)!= 11)goto _LL35C;_LL35B: goto _LL351;_LL35C:;_LL35D: goto _LL351;_LL351:;}}
