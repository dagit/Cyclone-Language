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
_dynforward_ptr f1;};struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,struct
_dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*d);int Cyc_Dict_member(struct Cyc_Dict_Dict*
d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*
v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);void**Cyc_Dict_rlookup_opt(
struct _RegionHandle*,struct Cyc_Dict_Dict*d,void*k);struct _tuple0{void*f1;void*f2;
};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_Dict_Dict*Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict*,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];
struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[12];extern char Cyc_SlowDict_Absent[
11];struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_dynforward_typ(void*
t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
le;};void*Cyc_Tcenv_env_err(struct _dynforward_ptr msg);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(
struct _RegionHandle*);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct
_RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*old_fenv,
struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dynforward_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dynforward_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple4{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_compress(void*
t);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error\000";
void*Cyc_Tcenv_env_err(struct _dynforward_ptr msg){({struct Cyc_String_pa_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)msg);{
void*_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="Internal error in tcenv: %s\n";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,29));}),
_tag_dynforward(_tmp0,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(
int)_throw((void*)Cyc_Tcenv_Env_error);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
struct Cyc_Tcenv_CtrlEnv{void*continue_stmt;void*break_stmt;struct _tuple4*
fallthru_clause;void*next_stmt;int try_depth;};struct Cyc_Tcenv_SharedFenv{struct
_RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict*seen_labels;struct Cyc_Dict_Dict*
needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dynforward_ptr*v;
void*b;struct Cyc_Tcenv_Bindings*const tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;};char Cyc_Tcenv_NoBinding[
14]="\000\000\000\000NoBinding\000";void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*
bs,struct _dynforward_ptr*v){for(0;(unsigned int)bs;bs=bs->tl){if(Cyc_strptrcmp(v,
bs->v)== 0)return(void*)bs->b;}(int)_throw((void*)Cyc_Tcenv_NoBinding);}struct
Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){return({struct Cyc_Tcenv_Genv*
_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->grgn=r;_tmp3->namespaces=((struct
Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp3->aggrdecls=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->tuniondecls=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->enumdecls=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->typedefs=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))
Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->availables=0;_tmp3;});}struct _tuple5{
void*f1;int f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp4=_cycalloc(sizeof(*_tmp4));
_tmp4->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp4;});{struct Cyc_Tcenv_Genv*
_tmp5=Cyc_Tcenv_empty_genv(r);_tmp5->tuniondecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(
_tmp5->tuniondecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Tuniondecl**_tmp6=
_cycalloc(sizeof(*_tmp6));_tmp6[0]=Cyc_Absyn_exn_tud;_tmp6;}));{struct Cyc_List_List*
_tmp7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp7 != 0;_tmp7=_tmp7->tl){_tmp5->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(
_tmp5->ordinaries,(*((struct Cyc_Absyn_Tunionfield*)_tmp7->hd)->name).f2,({struct
_tuple5*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Tcenv_TunionRes_struct _tmpA;
_tmpA.tag=2;_tmpA.f1=Cyc_Absyn_exn_tud;_tmpA.f2=(struct Cyc_Absyn_Tunionfield*)
_tmp7->hd;_tmpA;});_tmp9;});_tmp8->f2=1;_tmp8;}));}}{struct Cyc_Dict_Dict*ae=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),
0,_tmp5);return({struct Cyc_Tcenv_Tenv*_tmpB=_region_malloc(r,sizeof(*_tmpB));
_tmpB->ns=0;_tmpB->ae=ae;_tmpB->le=0;_tmpB;});}}}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*n,
struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,
loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns){if(ns == 0)return((struct Cyc_List_List*(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(({
const char*_tmpC="outer_namespace";_tag_dynforward(_tmpC,sizeof(char),
_get_zero_arr_size(_tmpC,16));}));return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dynforward_ptr*)n1->hd,(
struct _dynforward_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct
_dynforward_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){for(0;nss != 0;
nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);struct
_handler_cons _tmpD;_push_handler(& _tmpD);{int _tmpF=0;if(setjmp(_tmpD.handler))
_tmpF=1;if(!_tmpF){lookup(ge2,v);({struct Cyc_String_pa_struct _tmp12;_tmp12.tag=0;
_tmp12.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*v);{void*_tmp10[1]={&
_tmp12};Cyc_Tcutil_terr(loc,({const char*_tmp11="%s is ambiguous";_tag_dynforward(
_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,16));}),_tag_dynforward(_tmp10,
sizeof(void*),1));}});;_pop_handler();}else{void*_tmpE=(void*)_exn_thrown;void*
_tmp14=_tmpE;_LL1: if(_tmp14 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(
void)_throw(_tmp14);_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dynforward_ptr*),struct _dynforward_ptr*v){struct Cyc_List_List*cns=te->ns;
while(1){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp15;
_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!
_tmp17){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp18=result;_npop_handler(0);return _tmp18;}};_pop_handler();}else{void*
_tmp16=(void*)_exn_thrown;void*_tmp1A=_tmp16;_LL6: if(_tmp1A != Cyc_Dict_Absent)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp1A);_LL5:;}}}{struct Cyc_List_List*
_tmp1B=ge->availables;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp1B->hd);struct _handler_cons _tmp1C;_push_handler(&
_tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp1B->hd,_tmp1B->tl);{void*_tmp1F=result;_npop_handler(0);return _tmp1F;}};
_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;_LLB: if(
_tmp21 != Cyc_Dict_Absent)goto _LLD;_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(_tmp21);
_LLA:;}}}}if(cns == 0)(int)_throw((void*)Cyc_Dict_Absent);cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dynforward_ptr*v){struct Cyc_Dict_Dict*_tmp22=ge->ordinaries;struct _tuple5*
ans=((struct _tuple5*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(
_tmp22,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp24;void*_tmp25;struct _dynforward_ptr*_tmp26;struct _tuple1*_tmp23=q;
_tmp24=*_tmp23;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;{void*_tmp27=_tmp25;struct Cyc_List_List*
_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List _tmp2A;struct
_dynforward_ptr*_tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2D;
_LL10: if((int)_tmp27 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if(_tmp27 <= (void*)1)
goto _LL14;if(*((int*)_tmp27)!= 0)goto _LL14;_tmp28=((struct Cyc_Absyn_Rel_n_struct*)
_tmp27)->f1;if(_tmp28 != 0)goto _LL14;_LL13: return Cyc_Tcenv_scoped_lookup(te,loc,
Cyc_Tcenv_lookup_ordinary_global_f,_tmp26);_LL14: if(_tmp27 <= (void*)1)goto _LL16;
if(*((int*)_tmp27)!= 0)goto _LL16;_tmp29=((struct Cyc_Absyn_Rel_n_struct*)_tmp27)->f1;
if(_tmp29 == 0)goto _LL16;_tmp2A=*_tmp29;_tmp2B=(struct _dynforward_ptr*)_tmp2A.hd;
_tmp2C=_tmp2A.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp2E=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp2B,_tmp2C);return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2E,_tmp26);}
_LL16: if(_tmp27 <= (void*)1)goto _LLF;if(*((int*)_tmp27)!= 1)goto _LLF;_tmp2D=((
struct Cyc_Absyn_Abs_n_struct*)_tmp27)->f1;_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp2D),_tmp26);_LLF:;}}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*n,
struct Cyc_List_List*ns){struct Cyc_List_List*_tmp2F=te->ns;struct _RegionHandle
_tmp30=_new_region("temp");struct _RegionHandle*temp=& _tmp30;_push_region(temp);{
struct Cyc_List_List*_tmp31=0;while(1){struct Cyc_Tcenv_Genv*_tmp32=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2F);
struct Cyc_List_List*_tmp33=_tmp32->availables;struct Cyc_Set_Set*_tmp34=_tmp32->namespaces;{
struct Cyc_List_List*_tmp35=_tmp33;for(0;_tmp35 != 0;_tmp35=_tmp35->tl){struct Cyc_Set_Set*
_tmp36=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp35->hd))->namespaces;if(((int(*)(
struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_member)(_tmp36,n))_tmp31=({
struct Cyc_List_List*_tmp37=_region_malloc(temp,sizeof(*_tmp37));_tmp37->hd=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp2F,({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->hd=n;
_tmp38->tl=ns;_tmp38;}));_tmp37->tl=_tmp31;_tmp37;});}}if(((int(*)(struct Cyc_Set_Set*
s,struct _dynforward_ptr*elt))Cyc_Set_member)(_tmp34,n))_tmp31=({struct Cyc_List_List*
_tmp39=_region_malloc(temp,sizeof(*_tmp39));_tmp39->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2F,({struct Cyc_List_List*
_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->hd=n;_tmp3A->tl=ns;_tmp3A;}));_tmp39->tl=
_tmp31;_tmp39;});if(_tmp31 != 0){if(_tmp31->tl != 0)({struct Cyc_String_pa_struct
_tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*n);{
void*_tmp3B[1]={& _tmp3D};Cyc_Tcutil_terr(loc,({const char*_tmp3C="%s is ambiguous";
_tag_dynforward(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,16));}),
_tag_dynforward(_tmp3B,sizeof(void*),1));}});{struct Cyc_List_List*_tmp3E=(struct
Cyc_List_List*)_tmp31->hd;_npop_handler(0);return _tmp3E;}}if(_tmp2F == 0)(int)
_throw((void*)Cyc_Dict_Absent);_tmp2F=Cyc_Tcenv_outer_namespace(_tmp2F);}};
_pop_region(temp);}static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict*_tmp3F=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp3F,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp41;void*_tmp42;struct _dynforward_ptr*_tmp43;struct _tuple1*_tmp40=q;
_tmp41=*_tmp40;_tmp42=_tmp41.f1;_tmp43=_tmp41.f2;{void*_tmp44=_tmp42;struct Cyc_List_List*
_tmp45;struct Cyc_List_List*_tmp46;struct Cyc_List_List*_tmp47;struct Cyc_List_List
_tmp48;struct _dynforward_ptr*_tmp49;struct Cyc_List_List*_tmp4A;_LL19: if((int)
_tmp44 != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp44 <= (void*)1)goto _LL1D;if(*((
int*)_tmp44)!= 0)goto _LL1D;_tmp45=((struct Cyc_Absyn_Rel_n_struct*)_tmp44)->f1;
if(_tmp45 != 0)goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dynforward_ptr*),struct _dynforward_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,
Cyc_Tcenv_lookup_aggrdecl_f,_tmp43);_LL1D: if(_tmp44 <= (void*)1)goto _LL1F;if(*((
int*)_tmp44)!= 1)goto _LL1F;_tmp46=((struct Cyc_Absyn_Abs_n_struct*)_tmp44)->f1;
_LL1E: {struct Cyc_Dict_Dict*_tmp4B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp46))->aggrdecls;return((
struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(
_tmp4B,_tmp43);}_LL1F: if(_tmp44 <= (void*)1)goto _LL18;if(*((int*)_tmp44)!= 0)goto
_LL18;_tmp47=((struct Cyc_Absyn_Rel_n_struct*)_tmp44)->f1;if(_tmp47 == 0)goto _LL18;
_tmp48=*_tmp47;_tmp49=(struct _dynforward_ptr*)_tmp48.hd;_tmp4A=_tmp48.tl;_LL20: {
struct Cyc_Dict_Dict*_tmp4C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp49,_tmp4A))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp4C,_tmp43);}_LL18:;}}static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl_f(struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){
struct Cyc_Dict_Dict*_tmp4D=ge->tuniondecls;return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp4D,v);}
struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp4F;void*_tmp50;
struct _dynforward_ptr*_tmp51;struct _tuple1*_tmp4E=q;_tmp4F=*_tmp4E;_tmp50=_tmp4F.f1;
_tmp51=_tmp4F.f2;{void*_tmp52=_tmp50;struct Cyc_List_List*_tmp53;struct Cyc_List_List*
_tmp54;struct Cyc_List_List _tmp55;struct _dynforward_ptr*_tmp56;struct Cyc_List_List*
_tmp57;struct Cyc_List_List*_tmp58;_LL22: if((int)_tmp52 != 0)goto _LL24;_LL23: goto
_LL25;_LL24: if(_tmp52 <= (void*)1)goto _LL26;if(*((int*)_tmp52)!= 0)goto _LL26;
_tmp53=((struct Cyc_Absyn_Rel_n_struct*)_tmp52)->f1;if(_tmp53 != 0)goto _LL26;_LL25:
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Tuniondecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dynforward_ptr*),struct _dynforward_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_tuniondecl_f,
_tmp51);_LL26: if(_tmp52 <= (void*)1)goto _LL28;if(*((int*)_tmp52)!= 0)goto _LL28;
_tmp54=((struct Cyc_Absyn_Rel_n_struct*)_tmp52)->f1;if(_tmp54 == 0)goto _LL28;
_tmp55=*_tmp54;_tmp56=(struct _dynforward_ptr*)_tmp55.hd;_tmp57=_tmp55.tl;_LL27: {
struct Cyc_Dict_Dict*_tmp59=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp56,_tmp57))->tuniondecls;
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp59,_tmp51);}_LL28: if(_tmp52 <= (void*)1)
goto _LL21;if(*((int*)_tmp52)!= 1)goto _LL21;_tmp58=((struct Cyc_Absyn_Abs_n_struct*)
_tmp52)->f1;_LL29: {struct Cyc_Dict_Dict*_tmp5A=(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp58))->tuniondecls;
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp5A,_tmp51);}_LL21:;}}static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_xtuniondecl_f(struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q){struct _tuple1 _tmp5C;void*_tmp5D;struct _dynforward_ptr*_tmp5E;
struct _tuple1*_tmp5B=q;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{void*
_tmp5F=_tmp5D;struct Cyc_List_List*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_List_List
_tmp62;struct _dynforward_ptr*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*
_tmp65;_LL2B: if(_tmp5F <= (void*)1)goto _LL2D;if(*((int*)_tmp5F)!= 0)goto _LL2D;
_tmp60=((struct Cyc_Absyn_Rel_n_struct*)_tmp5F)->f1;if(_tmp60 != 0)goto _LL2D;_LL2C: {
struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))
_tmp68=1;if(!_tmp68){{struct Cyc_Core_Opt*_tmp6A=({struct Cyc_Core_Opt*_tmp69=
_region_malloc(r,sizeof(*_tmp69));_tmp69->v=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct _dynforward_ptr*v))
Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xtuniondecl_f,_tmp5E);_tmp69;});
_npop_handler(0);return _tmp6A;};_pop_handler();}else{void*_tmp67=(void*)
_exn_thrown;void*_tmp6C=_tmp67;_LL34: if(_tmp6C != Cyc_Dict_Absent)goto _LL36;_LL35:
return 0;_LL36:;_LL37:(void)_throw(_tmp6C);_LL33:;}}}_LL2D: if((int)_tmp5F != 0)
goto _LL2F;_LL2E:({void*_tmp6D[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E="lookup_xtuniondecl: impossible";
_tag_dynforward(_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,31));}),
_tag_dynforward(_tmp6D,sizeof(void*),0));});return 0;_LL2F: if(_tmp5F <= (void*)1)
goto _LL31;if(*((int*)_tmp5F)!= 0)goto _LL31;_tmp61=((struct Cyc_Absyn_Rel_n_struct*)
_tmp5F)->f1;if(_tmp61 == 0)goto _LL31;_tmp62=*_tmp61;_tmp63=(struct _dynforward_ptr*)
_tmp62.hd;_tmp64=_tmp62.tl;_LL30: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons
_tmp6F;_push_handler(& _tmp6F);{int _tmp71=0;if(setjmp(_tmp6F.handler))_tmp71=1;
if(!_tmp71){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp63,_tmp64);;_pop_handler();}
else{void*_tmp70=(void*)_exn_thrown;void*_tmp73=_tmp70;_LL39: if(_tmp73 != Cyc_Dict_Absent)
goto _LL3B;_LL3A:({void*_tmp74[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp75="bad qualified name for xtunion";
_tag_dynforward(_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,31));}),
_tag_dynforward(_tmp74,sizeof(void*),0));});(int)_throw((void*)Cyc_Dict_Absent);
_LL3B:;_LL3C:(void)_throw(_tmp73);_LL38:;}}}{struct Cyc_Dict_Dict*_tmp76=ge->tuniondecls;
return({struct Cyc_Core_Opt*_tmp77=_region_malloc(r,sizeof(*_tmp77));_tmp77->v=((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))
Cyc_Dict_lookup)(_tmp76,_tmp5E);_tmp77;});}}_LL31: if(_tmp5F <= (void*)1)goto _LL2A;
if(*((int*)_tmp5F)!= 1)goto _LL2A;_tmp65=((struct Cyc_Absyn_Abs_n_struct*)_tmp5F)->f1;
_LL32: {struct Cyc_Dict_Dict*_tmp78=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->tuniondecls;return({
struct Cyc_Core_Opt*_tmp79=_region_malloc(r,sizeof(*_tmp79));_tmp79->v=((struct
Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(
_tmp78,_tmp5E);_tmp79;});}_LL2A:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict*_tmp7A=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp7A,v);}struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp7C;void*_tmp7D;struct _dynforward_ptr*_tmp7E;struct _tuple1*_tmp7B=q;
_tmp7C=*_tmp7B;_tmp7D=_tmp7C.f1;_tmp7E=_tmp7C.f2;{void*_tmp7F=_tmp7D;struct Cyc_List_List*
_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List _tmp82;struct
_dynforward_ptr*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp85;
_LL3E: if((int)_tmp7F != 0)goto _LL40;_LL3F: goto _LL41;_LL40: if(_tmp7F <= (void*)1)
goto _LL42;if(*((int*)_tmp7F)!= 0)goto _LL42;_tmp80=((struct Cyc_Absyn_Rel_n_struct*)
_tmp7F)->f1;if(_tmp80 != 0)goto _LL42;_LL41: return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct _dynforward_ptr*v))
Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp7E);_LL42: if(
_tmp7F <= (void*)1)goto _LL44;if(*((int*)_tmp7F)!= 0)goto _LL44;_tmp81=((struct Cyc_Absyn_Rel_n_struct*)
_tmp7F)->f1;if(_tmp81 == 0)goto _LL44;_tmp82=*_tmp81;_tmp83=(struct _dynforward_ptr*)
_tmp82.hd;_tmp84=_tmp82.tl;_LL43: {struct Cyc_Dict_Dict*_tmp86=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp83,_tmp84))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(struct
Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp86,_tmp7E);}_LL44:
if(_tmp7F <= (void*)1)goto _LL3D;if(*((int*)_tmp7F)!= 1)goto _LL3D;_tmp85=((struct
Cyc_Absyn_Abs_n_struct*)_tmp7F)->f1;_LL45: {struct Cyc_Dict_Dict*_tmp87=(((struct
Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp85))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*
d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp87,_tmp7E);}_LL3D:;}}static
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict*_tmp88=ge->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))
Cyc_Dict_lookup)(_tmp88,v);}struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp8A;void*_tmp8B;struct _dynforward_ptr*_tmp8C;struct _tuple1*_tmp89=q;
_tmp8A=*_tmp89;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;{void*_tmp8D=_tmp8B;struct Cyc_List_List*
_tmp8E;struct Cyc_List_List*_tmp8F;struct Cyc_List_List _tmp90;struct
_dynforward_ptr*_tmp91;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp93;
_LL47: if((int)_tmp8D != 0)goto _LL49;_LL48: goto _LL4A;_LL49: if(_tmp8D <= (void*)1)
goto _LL4B;if(*((int*)_tmp8D)!= 0)goto _LL4B;_tmp8E=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8D)->f1;if(_tmp8E != 0)goto _LL4B;_LL4A: return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct _dynforward_ptr*v))
Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp8C);_LL4B: if(
_tmp8D <= (void*)1)goto _LL4D;if(*((int*)_tmp8D)!= 0)goto _LL4D;_tmp8F=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8D)->f1;if(_tmp8F == 0)goto _LL4D;_tmp90=*_tmp8F;_tmp91=(struct _dynforward_ptr*)
_tmp90.hd;_tmp92=_tmp90.tl;_LL4C: {struct Cyc_Dict_Dict*_tmp94=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp91,_tmp92))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(struct
Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp94,_tmp8C);}_LL4D:
if(_tmp8D <= (void*)1)goto _LL46;if(*((int*)_tmp8D)!= 1)goto _LL46;_tmp93=((struct
Cyc_Absyn_Abs_n_struct*)_tmp8D)->f1;_LL4E: {struct Cyc_Dict_Dict*_tmp95=(((struct
Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp93))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*
d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp95,_tmp8C);}_LL46:;}}static
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct
_dynforward_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmp96=te->le;if((struct Cyc_Tcenv_Tenv*)
te == 0)((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(err_msg);return(
struct Cyc_Tcenv_Fenv*)_check_null(_tmp96);}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le
== 0)((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp97="put_fenv";
_tag_dynforward(_tmp97,sizeof(char),_get_zero_arr_size(_tmp97,9));}));return({
struct Cyc_Tcenv_Tenv*_tmp98=_region_malloc(l,sizeof(*_tmp98));_tmp98->ns=te->ns;
_tmp98->ae=te->ae;_tmp98->le=(struct Cyc_Tcenv_Fenv*)fe;_tmp98;});}static struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*
te){return({struct Cyc_Tcenv_Tenv*_tmp99=_region_malloc(l,sizeof(*_tmp99));_tmp99->ns=
te->ns;_tmp99->ae=te->ae;_tmp99->le=0;_tmp99;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmp9B;
struct Cyc_List_List*_tmp9C;struct Cyc_RgnOrder_RgnPO*_tmp9D;struct Cyc_Tcenv_Bindings*
_tmp9E;struct Cyc_Absyn_Stmt*_tmp9F;struct Cyc_Tcenv_CtrlEnv*_tmpA0;void*_tmpA1;
void*_tmpA2;int _tmpA3;struct Cyc_Tcenv_Fenv _tmp9A=*f;_tmp9B=_tmp9A.shared;_tmp9C=
_tmp9A.type_vars;_tmp9D=_tmp9A.region_order;_tmp9E=_tmp9A.locals;_tmp9F=_tmp9A.encloser;
_tmpA0=_tmp9A.ctrl_env;_tmpA1=(void*)_tmp9A.capability;_tmpA2=(void*)_tmp9A.curr_rgn;
_tmpA3=_tmp9A.in_notreadctxt;return({struct Cyc_Tcenv_Fenv*_tmpA4=_region_malloc(
l,sizeof(*_tmpA4));_tmpA4->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9B;_tmpA4->type_vars=(
struct Cyc_List_List*)_tmp9C;_tmpA4->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp9D;_tmpA4->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp9E);_tmpA4->encloser=(struct Cyc_Absyn_Stmt*)_tmp9F;_tmpA4->ctrl_env=(struct
Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpA0);_tmpA4->capability=(void*)((
void*)_tmpA1);_tmpA4->curr_rgn=(void*)((void*)_tmpA2);_tmpA4->in_notreadctxt=(
int)_tmpA3;_tmpA4;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpA6;
struct Cyc_List_List*_tmpA7;struct Cyc_RgnOrder_RgnPO*_tmpA8;struct Cyc_Tcenv_Bindings*
_tmpA9;struct Cyc_Absyn_Stmt*_tmpAA;struct Cyc_Tcenv_CtrlEnv*_tmpAB;void*_tmpAC;
void*_tmpAD;int _tmpAE;struct Cyc_Tcenv_Fenv _tmpA5=*f;_tmpA6=_tmpA5.shared;_tmpA7=
_tmpA5.type_vars;_tmpA8=_tmpA5.region_order;_tmpA9=_tmpA5.locals;_tmpAA=_tmpA5.encloser;
_tmpAB=_tmpA5.ctrl_env;_tmpAC=(void*)_tmpA5.capability;_tmpAD=(void*)_tmpA5.curr_rgn;
_tmpAE=_tmpA5.in_notreadctxt;{void*_tmpB0;void*_tmpB1;struct _tuple4*_tmpB2;void*
_tmpB3;int _tmpB4;struct Cyc_Tcenv_CtrlEnv _tmpAF=*_tmpAB;_tmpB0=(void*)_tmpAF.continue_stmt;
_tmpB1=(void*)_tmpAF.break_stmt;_tmpB2=_tmpAF.fallthru_clause;_tmpB3=(void*)
_tmpAF.next_stmt;_tmpB4=_tmpAF.try_depth;{struct Cyc_Tcenv_CtrlEnv*_tmpB5=({
struct Cyc_Tcenv_CtrlEnv*_tmpB7=_region_malloc(l,sizeof(*_tmpB7));_tmpB7->continue_stmt=(
void*)_tmpB0;_tmpB7->break_stmt=(void*)_tmpB1;_tmpB7->fallthru_clause=_tmpB2;
_tmpB7->next_stmt=(void*)_tmpB3;_tmpB7->try_depth=_tmpB4;_tmpB7;});return({
struct Cyc_Tcenv_Fenv*_tmpB6=_region_malloc(l,sizeof(*_tmpB6));_tmpB6->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpA6;_tmpB6->type_vars=(struct Cyc_List_List*)
_tmpA7;_tmpB6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA8;_tmpB6->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpA9);_tmpB6->encloser=(
struct Cyc_Absyn_Stmt*)_tmpAA;_tmpB6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB5;
_tmpB6->capability=(void*)((void*)_tmpAC);_tmpB6->curr_rgn=(void*)((void*)_tmpAD);
_tmpB6->in_notreadctxt=(int)_tmpAE;_tmpB6;});}}}void*Cyc_Tcenv_return_typ(struct
Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmpBA;struct Cyc_Tcenv_SharedFenv*_tmpBB;
struct Cyc_Tcenv_Fenv*_tmpB9=Cyc_Tcenv_get_fenv(te,({const char*_tmpB8="return_typ";
_tag_dynforward(_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,11));}));_tmpBA=*
_tmpB9;_tmpBB=_tmpBA.shared;{void*_tmpBD;struct Cyc_Tcenv_SharedFenv _tmpBC=*
_tmpBB;_tmpBD=(void*)_tmpBC.return_typ;return _tmpBD;}}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpBE=te->le;if(te->le == 0)return
0;{struct Cyc_List_List*_tmpC0;struct Cyc_Tcenv_Fenv _tmpBF=*((struct Cyc_Tcenv_Fenv*)
_check_null(_tmpBE));_tmpC0=_tmpBF.type_vars;return _tmpC0;}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
r,Cyc_Tcenv_get_fenv(te,({const char*_tmpC6="add_type_vars";_tag_dynforward(
_tmpC6,sizeof(char),_get_zero_arr_size(_tmpC6,14));})));Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_Tcenv_Fenv _tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_List_List**
_tmpC4;struct Cyc_Tcenv_Fenv*_tmpC1=fe;_tmpC2=*_tmpC1;_tmpC3=_tmpC2.type_vars;
_tmpC4=(struct Cyc_List_List**)&(*_tmpC1).type_vars;{struct Cyc_List_List*_tmpC5=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
tvs,*_tmpC4);Cyc_Tcutil_check_unique_tvars(loc,_tmpC5);*_tmpC4=_tmpC5;return Cyc_Tcenv_put_fenv(
r,te,fe);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpC7=te->le;if(_tmpC7 == 0)return
Cyc_Tcenv_put_emptyfenv(r,te);else{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
r,(struct Cyc_Tcenv_Fenv*)_tmpC7);return Cyc_Tcenv_put_fenv(r,te,fe);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct Cyc_Tcenv_Fenv*_tmpC8=
Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmpD5="add_local_var";
_tag_dynforward(_tmpD5,sizeof(char),_get_zero_arr_size(_tmpD5,14));})));struct
_dynforward_ptr*_tmpC9=(*vd->name).f2;{void*_tmpCA=(*vd->name).f1;_LL50: if((int)
_tmpCA != 0)goto _LL52;_LL51: goto _LL4F;_LL52:;_LL53:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Core_Impossible_struct
_tmpCC;_tmpCC.tag=Cyc_Core_Impossible;_tmpCC.f1=({const char*_tmpCD="add_local_var: called with Rel_n";
_tag_dynforward(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,33));});_tmpCC;});
_tmpCB;}));_LL4F:;}{struct Cyc_Tcenv_Fenv _tmpCF;struct Cyc_Tcenv_Bindings*_tmpD0;
struct Cyc_Tcenv_Bindings**_tmpD1;struct Cyc_Tcenv_Fenv*_tmpCE=_tmpC8;_tmpCF=*
_tmpCE;_tmpD0=_tmpCF.locals;_tmpD1=(struct Cyc_Tcenv_Bindings**)&(*_tmpCE).locals;*
_tmpD1=({struct Cyc_Tcenv_Bindings*_tmpD2=_region_malloc(r,sizeof(*_tmpD2));
_tmpD2->v=_tmpC9;_tmpD2->b=(void*)((void*)({struct Cyc_Absyn_Local_b_struct*
_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_Local_b_struct
_tmpD4;_tmpD4.tag=3;_tmpD4.f1=vd;_tmpD4;});_tmpD3;}));_tmpD2->tl=*_tmpD1;_tmpD2;});
return Cyc_Tcenv_put_fenv(r,te,_tmpC8);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpD6=te->le;
if(_tmpD6 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_Fenv*_tmpD7=
Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)_tmpD6);struct Cyc_Tcenv_Fenv
_tmpD9;int _tmpDA;int*_tmpDB;struct Cyc_Tcenv_Fenv*_tmpD8=_tmpD7;_tmpD9=*_tmpD8;
_tmpDA=_tmpD9.in_notreadctxt;_tmpDB=(int*)&(*_tmpD8).in_notreadctxt;*_tmpDB=1;
return Cyc_Tcenv_put_fenv(r,te,_tmpD7);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpDC=te->le;
if(_tmpDC == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_Fenv*_tmpDD=
Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)_tmpDC);struct Cyc_Tcenv_Fenv
_tmpDF;int _tmpE0;int*_tmpE1;struct Cyc_Tcenv_Fenv*_tmpDE=_tmpDD;_tmpDF=*_tmpDE;
_tmpE0=_tmpDF.in_notreadctxt;_tmpE1=(int*)&(*_tmpDE).in_notreadctxt;*_tmpE1=0;
return Cyc_Tcenv_put_fenv(r,te,_tmpDD);}}int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmpE2=te->le;if(_tmpE2 == 0)return 0;{struct Cyc_Tcenv_Fenv
_tmpE4;int _tmpE5;struct Cyc_Tcenv_Fenv*_tmpE3=(struct Cyc_Tcenv_Fenv*)_tmpE2;
_tmpE4=*_tmpE3;_tmpE5=_tmpE4.in_notreadctxt;return _tmpE5;}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct Cyc_Tcenv_Fenv*_tmpE6=
Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmpEF="add_pat_var";
_tag_dynforward(_tmpEF,sizeof(char),_get_zero_arr_size(_tmpEF,12));})));struct
_dynforward_ptr*_tmpE7=(*vd->name).f2;struct Cyc_Tcenv_Fenv _tmpE9;struct Cyc_Tcenv_Bindings*
_tmpEA;struct Cyc_Tcenv_Bindings**_tmpEB;struct Cyc_Tcenv_Fenv*_tmpE8=_tmpE6;
_tmpE9=*_tmpE8;_tmpEA=_tmpE9.locals;_tmpEB=(struct Cyc_Tcenv_Bindings**)&(*_tmpE8).locals;*
_tmpEB=({struct Cyc_Tcenv_Bindings*_tmpEC=_region_malloc(r,sizeof(*_tmpEC));
_tmpEC->v=_tmpE7;_tmpEC->b=(void*)((void*)({struct Cyc_Absyn_Pat_b_struct*_tmpED=
_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_Pat_b_struct _tmpEE;_tmpEE.tag=
4;_tmpEE.f1=vd;_tmpEE;});_tmpED;}));_tmpEC->tl=*_tmpEB;_tmpEC;});return Cyc_Tcenv_put_fenv(
r,te,_tmpE6);}void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct Cyc_Tcenv_Fenv*_tmpF0=
te->le;struct _tuple1 _tmpF2;void*_tmpF3;struct _dynforward_ptr*_tmpF4;struct
_tuple1*_tmpF1=q;_tmpF2=*_tmpF1;_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;{void*_tmpF5=
_tmpF3;struct Cyc_List_List*_tmpF6;_LL55: if((int)_tmpF5 != 0)goto _LL57;_LL56: if(
_tmpF0 == 0)(int)_throw((void*)Cyc_Dict_Absent);goto _LL58;_LL57: if(_tmpF5 <= (void*)
1)goto _LL59;if(*((int*)_tmpF5)!= 0)goto _LL59;_tmpF6=((struct Cyc_Absyn_Rel_n_struct*)
_tmpF5)->f1;if(_tmpF6 != 0)goto _LL59;if(!(_tmpF0 != 0))goto _LL59;_LL58: {struct Cyc_Tcenv_Fenv
_tmpF8;struct Cyc_Tcenv_Bindings*_tmpF9;struct Cyc_Tcenv_Fenv*_tmpF7=(struct Cyc_Tcenv_Fenv*)
_tmpF0;_tmpF8=*_tmpF7;_tmpF9=_tmpF8.locals;{struct _handler_cons _tmpFA;
_push_handler(& _tmpFA);{int _tmpFC=0;if(setjmp(_tmpFA.handler))_tmpFC=1;if(!
_tmpFC){{void*_tmpFF=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmpFD=
_region_malloc(r,sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Tcenv_VarRes_struct
_tmpFE;_tmpFE.tag=0;_tmpFE.f1=(void*)Cyc_Tcenv_lookup_binding(_tmpF9,_tmpF4);
_tmpFE;});_tmpFD;});_npop_handler(0);return _tmpFF;};_pop_handler();}else{void*
_tmpFB=(void*)_exn_thrown;void*_tmp101=_tmpFB;_LL5C:;_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(
te,loc,q);_LL5E:;_LL5F:(void)_throw(_tmp101);_LL5B:;}}}}_LL59:;_LL5A: {struct
_handler_cons _tmp102;_push_handler(& _tmp102);{int _tmp104=0;if(setjmp(_tmp102.handler))
_tmp104=1;if(!_tmp104){{void*_tmp105=(void*)Cyc_Tcenv_lookup_ordinary_global(te,
loc,q);_npop_handler(0);return _tmp105;};_pop_handler();}else{void*_tmp103=(void*)
_exn_thrown;void*_tmp107=_tmp103;_LL61: if(_tmp107 != Cyc_Dict_Absent)goto _LL63;
_LL62: return(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp108=_region_malloc(r,
sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Tcenv_VarRes_struct _tmp109;_tmp109.tag=
0;_tmp109.f1=(void*)((void*)0);_tmp109;});_tmp108;});_LL63:;_LL64:(void)_throw(
_tmp107);_LL60:;}}}_LL54:;}}void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp10C;struct Cyc_Tcenv_CtrlEnv*_tmp10D;struct Cyc_Tcenv_Fenv*_tmp10B=Cyc_Tcenv_get_fenv(
te,({const char*_tmp10A="process_continue";_tag_dynforward(_tmp10A,sizeof(char),
_get_zero_arr_size(_tmp10A,17));}));_tmp10C=*_tmp10B;_tmp10D=_tmp10C.ctrl_env;{
void*_tmp10E=(void*)_tmp10D->continue_stmt;struct Cyc_Absyn_Stmt*_tmp10F;_LL66:
if(_tmp10E <= (void*)3)goto _LL68;if(*((int*)_tmp10E)!= 0)goto _LL68;_tmp10F=((
struct Cyc_Tcenv_Stmt_j_struct*)_tmp10E)->f1;_LL67: _tmp10F->non_local_preds=({
struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->hd=s;_tmp110->tl=
_tmp10F->non_local_preds;_tmp110;});*sopt=(struct Cyc_Absyn_Stmt*)_tmp10F;return;
_LL68: if((int)_tmp10E != 0)goto _LL6A;_LL69:({void*_tmp111[0]={};Cyc_Tcutil_terr(s->loc,({
const char*_tmp112="continue not in a loop";_tag_dynforward(_tmp112,sizeof(char),
_get_zero_arr_size(_tmp112,23));}),_tag_dynforward(_tmp111,sizeof(void*),0));});
return;_LL6A: if((int)_tmp10E != 1)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)_tmp10E
!= 2)goto _LL65;_LL6D:((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(({
const char*_tmp113="bad continue destination";_tag_dynforward(_tmp113,sizeof(char),
_get_zero_arr_size(_tmp113,25));}));_LL65:;}}void Cyc_Tcenv_process_break(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp116;struct Cyc_Tcenv_CtrlEnv*_tmp117;struct Cyc_Tcenv_SharedFenv*_tmp118;
struct Cyc_Tcenv_Fenv*_tmp115=Cyc_Tcenv_get_fenv(te,({const char*_tmp114="process_break";
_tag_dynforward(_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,14));}));_tmp116=*
_tmp115;_tmp117=_tmp116.ctrl_env;_tmp118=_tmp116.shared;{void*_tmp119=(void*)
_tmp117->break_stmt;struct Cyc_Absyn_Stmt*_tmp11A;_LL6F: if(_tmp119 <= (void*)3)
goto _LL71;if(*((int*)_tmp119)!= 0)goto _LL71;_tmp11A=((struct Cyc_Tcenv_Stmt_j_struct*)
_tmp119)->f1;_LL70: _tmp11A->non_local_preds=({struct Cyc_List_List*_tmp11B=
_cycalloc(sizeof(*_tmp11B));_tmp11B->hd=s;_tmp11B->tl=_tmp11A->non_local_preds;
_tmp11B;});*sopt=(struct Cyc_Absyn_Stmt*)_tmp11A;return;_LL71: if((int)_tmp119 != 0)
goto _LL73;_LL72:({void*_tmp11C[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp11D="break not in a loop or switch";
_tag_dynforward(_tmp11D,sizeof(char),_get_zero_arr_size(_tmp11D,30));}),
_tag_dynforward(_tmp11C,sizeof(void*),0));});return;_LL73: if((int)_tmp119 != 2)
goto _LL75;_LL74: if((void*)_tmp118->return_typ != (void*)((void*)0))({void*_tmp11E[
0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp11F="break causes function not to return a value";
_tag_dynforward(_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,44));}),
_tag_dynforward(_tmp11E,sizeof(void*),0));});return;_LL75: if((int)_tmp119 != 1)
goto _LL6E;_LL76:({void*_tmp120[0]={};Cyc_Tcutil_terr(s->loc,({const char*_tmp121="break causes outer switch clause to implicitly fallthru";
_tag_dynforward(_tmp121,sizeof(char),_get_zero_arr_size(_tmp121,56));}),
_tag_dynforward(_tmp120,sizeof(void*),0));});return;_LL6E:;}}void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dynforward_ptr*l,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp124;struct Cyc_Tcenv_SharedFenv*_tmp125;struct Cyc_Tcenv_Fenv*
_tmp123=Cyc_Tcenv_get_fenv(te,({const char*_tmp122="process_goto";_tag_dynforward(
_tmp122,sizeof(char),_get_zero_arr_size(_tmp122,13));}));_tmp124=*_tmp123;
_tmp125=_tmp124.shared;{struct _RegionHandle _tmp126=_new_region("temp");struct
_RegionHandle*temp=& _tmp126;_push_region(temp);{struct Cyc_Absyn_Stmt**sopt2=((
struct Cyc_Absyn_Stmt**(*)(struct _RegionHandle*,struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_rlookup_opt)(temp,_tmp125->seen_labels,l);if(sopt2 == 
0){struct Cyc_Dict_Dict*_tmp127=_tmp125->needed_labels;struct Cyc_List_List**slopt=((
struct Cyc_List_List**(*)(struct _RegionHandle*,struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_rlookup_opt)(temp,_tmp127,l);if(slopt == 0)slopt=({
struct Cyc_List_List**_tmp128=_region_malloc(temp,sizeof(*_tmp128));_tmp128[0]=0;
_tmp128;});{struct _RegionHandle*frgn=_tmp125->frgn;struct Cyc_List_List*
new_needed=({struct Cyc_List_List*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->hd=
s;_tmp129->tl=*((struct Cyc_List_List**)_check_null(slopt));_tmp129;});_tmp125->needed_labels=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,struct Cyc_List_List*
v))Cyc_Dict_insert)(_tmp127,l,new_needed);}}else{struct Cyc_Absyn_Stmt*s=*sopt2;s->non_local_preds=({
struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->hd=s;_tmp12A->tl=
s->non_local_preds;_tmp12A;});*sopt=(struct Cyc_Absyn_Stmt*)s;}};_pop_region(temp);}}
struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Switch_clause***clauseopt){struct Cyc_Tcenv_Fenv _tmp12D;struct
Cyc_Tcenv_CtrlEnv*_tmp12E;struct Cyc_Tcenv_Fenv*_tmp12C=Cyc_Tcenv_get_fenv(te,({
const char*_tmp12B="process_fallthru";_tag_dynforward(_tmp12B,sizeof(char),
_get_zero_arr_size(_tmp12B,17));}));_tmp12D=*_tmp12C;_tmp12E=_tmp12D.ctrl_env;{
struct _tuple4*_tmp12F=_tmp12E->fallthru_clause;if(_tmp12F != 0){(((*_tmp12F).f1)->body)->non_local_preds=({
struct Cyc_List_List*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->hd=s;_tmp130->tl=(((*
_tmp12F).f1)->body)->non_local_preds;_tmp130;});*clauseopt=(struct Cyc_Absyn_Switch_clause**)({
struct Cyc_Absyn_Switch_clause**_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131[0]=(*
_tmp12F).f1;_tmp131;});}return _tmp12F;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){struct Cyc_List_List*
_tmp132=0;for(0;vds != 0;vds=vds->tl){_tmp132=({struct Cyc_List_List*_tmp133=
_cycalloc(sizeof(*_tmp133));_tmp133->hd=(void*)((void*)((struct Cyc_Absyn_Vardecl*)
vds->hd)->type);_tmp133->tl=_tmp132;_tmp133;});}_tmp132=Cyc_List_imp_rev(_tmp132);{
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({
const char*_tmp138="set_fallthru";_tag_dynforward(_tmp138,sizeof(char),
_get_zero_arr_size(_tmp138,13));})));struct Cyc_Tcenv_Fenv _tmp135;struct Cyc_Tcenv_CtrlEnv*
_tmp136;struct Cyc_Tcenv_Fenv*_tmp134=fe;_tmp135=*_tmp134;_tmp136=_tmp135.ctrl_env;
_tmp136->fallthru_clause=({struct _tuple4*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137->f1=clause;_tmp137->f2=new_tvs;_tmp137->f3=_tmp132;_tmp137;});return Cyc_Tcenv_put_fenv(
r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,
Cyc_Tcenv_get_fenv(te,({const char*_tmp13C="clear_fallthru";_tag_dynforward(
_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,15));})));struct Cyc_Tcenv_Fenv
_tmp13A;struct Cyc_Tcenv_CtrlEnv*_tmp13B;struct Cyc_Tcenv_Fenv*_tmp139=fe;_tmp13A=*
_tmp139;_tmp13B=_tmp13A.ctrl_env;_tmp13B->fallthru_clause=0;return Cyc_Tcenv_put_fenv(
r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){struct Cyc_Tcenv_Fenv
_tmp13F;struct Cyc_Tcenv_SharedFenv*_tmp140;struct Cyc_List_List*_tmp141;struct Cyc_RgnOrder_RgnPO*
_tmp142;struct Cyc_Tcenv_Bindings*_tmp143;struct Cyc_Absyn_Stmt*_tmp144;struct Cyc_Tcenv_CtrlEnv*
_tmp145;void*_tmp146;void*_tmp147;int _tmp148;struct Cyc_Tcenv_Fenv*_tmp13E=Cyc_Tcenv_get_fenv(
te,({const char*_tmp13D="set_in_loop";_tag_dynforward(_tmp13D,sizeof(char),
_get_zero_arr_size(_tmp13D,12));}));_tmp13F=*_tmp13E;_tmp140=_tmp13F.shared;
_tmp141=_tmp13F.type_vars;_tmp142=_tmp13F.region_order;_tmp143=_tmp13F.locals;
_tmp144=_tmp13F.encloser;_tmp145=_tmp13F.ctrl_env;_tmp146=(void*)_tmp13F.capability;
_tmp147=(void*)_tmp13F.curr_rgn;_tmp148=_tmp13F.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*
new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp14A=_region_malloc(r,sizeof(*_tmp14A));
_tmp14A->continue_stmt=(void*)((void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp14D=
_region_malloc(r,sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmp14E;_tmp14E.tag=0;_tmp14E.f1=continue_dest;_tmp14E;});_tmp14D;}));_tmp14A->break_stmt=(
void*)((void*)((void*)_tmp145->next_stmt));_tmp14A->next_stmt=(void*)((void*)({
struct Cyc_Tcenv_Stmt_j_struct*_tmp14B=_region_malloc(r,sizeof(*_tmp14B));_tmp14B[
0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=continue_dest;
_tmp14C;});_tmp14B;}));_tmp14A->fallthru_clause=_tmp145->fallthru_clause;_tmp14A->try_depth=
_tmp145->try_depth;_tmp14A;});struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*
_tmp149=_region_malloc(r,sizeof(*_tmp149));_tmp149->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp140;_tmp149->type_vars=(struct Cyc_List_List*)_tmp141;_tmp149->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp142;_tmp149->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp143);_tmp149->encloser=(struct Cyc_Absyn_Stmt*)
_tmp144;_tmp149->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp149->capability=(
void*)((void*)_tmp146);_tmp149->curr_rgn=(void*)((void*)_tmp147);_tmp149->in_notreadctxt=(
int)_tmp148;_tmp149;});return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp152="enter_try";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size(_tmp152,10));})));struct
Cyc_Tcenv_Fenv _tmp150;struct Cyc_Tcenv_CtrlEnv*_tmp151;struct Cyc_Tcenv_Fenv*
_tmp14F=fe;_tmp150=*_tmp14F;_tmp151=_tmp150.ctrl_env;++ _tmp151->try_depth;return
Cyc_Tcenv_put_fenv(r,te,fe);}int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmp155;struct Cyc_Tcenv_CtrlEnv*_tmp156;struct Cyc_Tcenv_Fenv*
_tmp154=Cyc_Tcenv_get_fenv(te,({const char*_tmp153="get_try_depth";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,14));}));_tmp155=*
_tmp154;_tmp156=_tmp155.ctrl_env;return _tmp156->try_depth;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp15A="set_in_switch";
_tag_dynforward(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,14));})));struct
Cyc_Tcenv_Fenv _tmp158;struct Cyc_Tcenv_CtrlEnv*_tmp159;struct Cyc_Tcenv_Fenv*
_tmp157=fe;_tmp158=*_tmp157;_tmp159=_tmp158.ctrl_env;(void*)(_tmp159->break_stmt=(
void*)((void*)_tmp159->next_stmt));(void*)(_tmp159->next_stmt=(void*)((void*)1));
return Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){struct Cyc_Tcenv_SharedFenv*
_tmp15D;struct Cyc_List_List*_tmp15E;struct Cyc_RgnOrder_RgnPO*_tmp15F;struct Cyc_Tcenv_Bindings*
_tmp160;struct Cyc_Absyn_Stmt*_tmp161;struct Cyc_Tcenv_CtrlEnv*_tmp162;void*
_tmp163;void*_tmp164;int _tmp165;struct Cyc_Tcenv_Fenv _tmp15C=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp15B="set_next";_tag_dynforward(_tmp15B,sizeof(char),
_get_zero_arr_size(_tmp15B,9));}));_tmp15D=_tmp15C.shared;_tmp15E=_tmp15C.type_vars;
_tmp15F=_tmp15C.region_order;_tmp160=_tmp15C.locals;_tmp161=_tmp15C.encloser;
_tmp162=_tmp15C.ctrl_env;_tmp163=(void*)_tmp15C.capability;_tmp164=(void*)
_tmp15C.curr_rgn;_tmp165=_tmp15C.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*
new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp167=_region_malloc(r,sizeof(*_tmp167));
_tmp167->continue_stmt=(void*)((void*)((void*)_tmp162->continue_stmt));_tmp167->break_stmt=(
void*)((void*)((void*)_tmp162->break_stmt));_tmp167->next_stmt=(void*)j;_tmp167->fallthru_clause=
_tmp162->fallthru_clause;_tmp167->try_depth=_tmp162->try_depth;_tmp167;});struct
Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp166=_region_malloc(r,sizeof(*
_tmp166));_tmp166->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15D;_tmp166->type_vars=(
struct Cyc_List_List*)_tmp15E;_tmp166->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp15F;_tmp166->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp160);_tmp166->encloser=(struct Cyc_Absyn_Stmt*)_tmp161;_tmp166->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp166->capability=(void*)((void*)_tmp163);
_tmp166->curr_rgn=(void*)((void*)_tmp164);_tmp166->in_notreadctxt=(int)_tmp165;
_tmp166;});return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*te,struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){struct
Cyc_Tcenv_Fenv _tmp16A;struct Cyc_Tcenv_SharedFenv*_tmp16B;struct Cyc_Tcenv_Fenv*
_tmp169=Cyc_Tcenv_get_fenv(te,({const char*_tmp168="add_label";_tag_dynforward(
_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,10));}));_tmp16A=*_tmp169;
_tmp16B=_tmp16A.shared;{struct Cyc_Dict_Dict*needed=_tmp16B->needed_labels;{
struct _RegionHandle _tmp16C=_new_region("temp");struct _RegionHandle*temp=& _tmp16C;
_push_region(temp);{struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k))Cyc_Dict_rlookup_opt)(
temp,needed,v);struct _RegionHandle*frgn=_tmp16B->frgn;if(sl_opt != 0){_tmp16B->needed_labels=((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict*,struct
_dynforward_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp16D=*
sl_opt;s->non_local_preds=_tmp16D;for(0;_tmp16D != 0;_tmp16D=_tmp16D->tl){void*
_tmp16E=(void*)((struct Cyc_Absyn_Stmt*)_tmp16D->hd)->r;struct Cyc_Absyn_Stmt*
_tmp16F;struct Cyc_Absyn_Stmt**_tmp170;_LL78: if(_tmp16E <= (void*)1)goto _LL7A;if(*((
int*)_tmp16E)!= 7)goto _LL7A;_tmp16F=((struct Cyc_Absyn_Goto_s_struct*)_tmp16E)->f2;
_tmp170=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)_tmp16E)->f2;
_LL79:*_tmp170=(struct Cyc_Absyn_Stmt*)s;goto _LL77;_LL7A:;_LL7B:((int(*)(struct
_dynforward_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp171="Tcenv: add_label backpatching of non-goto";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size(_tmp171,42));}));goto
_LL77;_LL77:;}}}};_pop_region(temp);}if(((int(*)(struct Cyc_Dict_Dict*d,struct
_dynforward_ptr*k))Cyc_Dict_member)(_tmp16B->seen_labels,v))({struct Cyc_String_pa_struct
_tmp174;_tmp174.tag=0;_tmp174.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
v);{void*_tmp172[1]={& _tmp174};Cyc_Tcutil_terr(s->loc,({const char*_tmp173="Repeated label: %s";
_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size(_tmp173,19));}),
_tag_dynforward(_tmp172,sizeof(void*),1));}});_tmp16B->seen_labels=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _dynforward_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(
_tmp16B->seen_labels,v,s);return te;}}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp177;struct Cyc_Tcenv_SharedFenv*_tmp178;struct Cyc_Tcenv_Fenv*
_tmp176=Cyc_Tcenv_get_fenv(te,({const char*_tmp175="all_labels_resolved";
_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size(_tmp175,20));}));_tmp177=*
_tmp176;_tmp178=_tmp177.shared;return((int(*)(struct Cyc_Dict_Dict*d))Cyc_Dict_is_empty)(
_tmp178->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp17B;struct Cyc_Absyn_Stmt*_tmp17C;struct Cyc_Tcenv_Fenv*
_tmp17A=Cyc_Tcenv_get_fenv(te,({const char*_tmp179="get_encloser";_tag_dynforward(
_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,13));}));_tmp17B=*_tmp17A;
_tmp17C=_tmp17B.encloser;return _tmp17C;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp181="set_encloser";
_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,13));})));struct
Cyc_Tcenv_Fenv _tmp17E;struct Cyc_Absyn_Stmt*_tmp17F;struct Cyc_Absyn_Stmt**_tmp180;
struct Cyc_Tcenv_Fenv*_tmp17D=fe;_tmp17E=*_tmp17D;_tmp17F=_tmp17E.encloser;
_tmp180=(struct Cyc_Absyn_Stmt**)&(*_tmp17D).encloser;*_tmp180=s;return Cyc_Tcenv_put_fenv(
r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,void*rgn,int resetable){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
r,Cyc_Tcenv_get_fenv(te,({const char*_tmp191="add_region";_tag_dynforward(_tmp191,
sizeof(char),_get_zero_arr_size(_tmp191,11));})));struct Cyc_Absyn_Tvar*tv;{void*
_tmp182=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp183;_LL7D: if(_tmp182 <= (
void*)4)goto _LL7F;if(*((int*)_tmp182)!= 1)goto _LL7F;_tmp183=((struct Cyc_Absyn_VarType_struct*)
_tmp182)->f1;_LL7E: tv=_tmp183;goto _LL7C;_LL7F:;_LL80: tv=((struct Cyc_Absyn_Tvar*(*)(
struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp184="bad add region";
_tag_dynforward(_tmp184,sizeof(char),_get_zero_arr_size(_tmp184,15));}));goto
_LL7C;_LL7C:;}{struct Cyc_Tcenv_Fenv _tmp186;struct Cyc_RgnOrder_RgnPO*_tmp187;
struct Cyc_RgnOrder_RgnPO**_tmp188;void*_tmp189;void**_tmp18A;struct Cyc_Tcenv_Fenv*
_tmp185=fe;_tmp186=*_tmp185;_tmp187=_tmp186.region_order;_tmp188=(struct Cyc_RgnOrder_RgnPO**)&(*
_tmp185).region_order;_tmp189=(void*)_tmp186.capability;_tmp18A=(void**)&(*
_tmp185).capability;*_tmp188=Cyc_RgnOrder_add_youngest(*_tmp188,tv,resetable);*
_tmp18A=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp18B=_cycalloc(sizeof(*
_tmp18B));_tmp18B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp18C;_tmp18C.tag=20;
_tmp18C.f1=({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp18F=_cycalloc(sizeof(*
_tmp18F));_tmp18F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp190;_tmp190.tag=19;
_tmp190.f1=(void*)rgn;_tmp190;});_tmp18F;}));_tmp18D->tl=({struct Cyc_List_List*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->hd=(void*)*_tmp18A;_tmp18E->tl=0;
_tmp18E;});_tmp18D;});_tmp18C;});_tmp18B;});return Cyc_Tcenv_put_fenv(r,te,fe);}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1A5="new_block";
_tag_dynforward(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,10));})));struct
Cyc_Tcenv_Fenv _tmp193;struct Cyc_List_List*_tmp194;struct Cyc_List_List**_tmp195;
struct Cyc_RgnOrder_RgnPO*_tmp196;struct Cyc_RgnOrder_RgnPO**_tmp197;void*_tmp198;
void**_tmp199;void*_tmp19A;void**_tmp19B;struct Cyc_Tcenv_Fenv*_tmp192=fe;_tmp193=*
_tmp192;_tmp194=_tmp193.type_vars;_tmp195=(struct Cyc_List_List**)&(*_tmp192).type_vars;
_tmp196=_tmp193.region_order;_tmp197=(struct Cyc_RgnOrder_RgnPO**)&(*_tmp192).region_order;
_tmp198=(void*)_tmp193.capability;_tmp199=(void**)&(*_tmp192).capability;_tmp19A=(
void*)_tmp193.curr_rgn;_tmp19B=(void**)&(*_tmp192).curr_rgn;{void*block_typ=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));
_tmp1A3[0]=({struct Cyc_Absyn_VarType_struct _tmp1A4;_tmp1A4.tag=1;_tmp1A4.f1=
block_rgn;_tmp1A4;});_tmp1A3;});*_tmp195=({struct Cyc_List_List*_tmp19C=_cycalloc(
sizeof(*_tmp19C));_tmp19C->hd=block_rgn;_tmp19C->tl=*_tmp195;_tmp19C;});Cyc_Tcutil_check_unique_tvars(
loc,*_tmp195);*_tmp197=Cyc_RgnOrder_add_youngest(*_tmp197,block_rgn,0);*_tmp199=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));
_tmp19D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp19E;_tmp19E.tag=20;_tmp19E.f1=({
struct Cyc_List_List*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));
_tmp1A1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1A2;_tmp1A2.tag=19;_tmp1A2.f1=(
void*)block_typ;_tmp1A2;});_tmp1A1;}));_tmp19F->tl=({struct Cyc_List_List*_tmp1A0=
_cycalloc(sizeof(*_tmp1A0));_tmp1A0->hd=(void*)*_tmp199;_tmp1A0->tl=0;_tmp1A0;});
_tmp19F;});_tmp19E;});_tmp19D;});*_tmp19B=block_typ;return Cyc_Tcenv_put_fenv(r,
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6[
0]=({struct Cyc_Absyn_Eq_kb_struct _tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=(void*)((void*)
3);_tmp1A7;});_tmp1A6;}));Cyc_Tcutil_add_tvar_identity(t);return Cyc_Tcenv_new_named_block(
r,loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1AC="new_outlives_constraints";
_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size(_tmp1AC,25));})));struct
Cyc_Tcenv_Fenv _tmp1A9;struct Cyc_RgnOrder_RgnPO*_tmp1AA;struct Cyc_RgnOrder_RgnPO**
_tmp1AB;struct Cyc_Tcenv_Fenv*_tmp1A8=fe;_tmp1A9=*_tmp1A8;_tmp1AA=_tmp1A9.region_order;
_tmp1AB=(struct Cyc_RgnOrder_RgnPO**)&(*_tmp1A8).region_order;for(0;cs != 0;cs=cs->tl){*
_tmp1AB=Cyc_RgnOrder_add_outlives_constraint(*_tmp1AB,(*((struct _tuple0*)cs->hd)).f1,(*((
struct _tuple0*)cs->hd)).f2);}return Cyc_Tcenv_put_fenv(r,te,fe);}void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp1AD=te->le;if(_tmp1AD == 0)
return(void*)2;{struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp1AD;struct Cyc_Tcenv_Fenv
_tmp1AF;void*_tmp1B0;struct Cyc_Tcenv_Fenv*_tmp1AE=fe;_tmp1AF=*_tmp1AE;_tmp1B0=(
void*)_tmp1AF.curr_rgn;return _tmp1B0;}}void Cyc_Tcenv_check_rgn_accessible(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp1BA="check_rgn_accessible";
_tag_dynforward(_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,21));}));struct
Cyc_Tcenv_Fenv _tmp1B2;void*_tmp1B3;struct Cyc_RgnOrder_RgnPO*_tmp1B4;struct Cyc_Tcenv_Fenv*
_tmp1B1=fe;_tmp1B2=*_tmp1B1;_tmp1B3=(void*)_tmp1B2.capability;_tmp1B4=_tmp1B2.region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp1B3) || Cyc_Tcutil_region_in_effect(1,
rgn,_tmp1B3))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp1B4,(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1B6;_tmp1B6.tag=19;_tmp1B6.f1=(void*)rgn;_tmp1B6;});_tmp1B5;}),_tmp1B3))
return;({struct Cyc_String_pa_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(rgn));{void*
_tmp1B7[1]={& _tmp1B9};Cyc_Tcutil_terr(loc,({const char*_tmp1B8="Expression accesses unavailable region %s";
_tag_dynforward(_tmp1B8,sizeof(char),_get_zero_arr_size(_tmp1B8,42));}),
_tag_dynforward(_tmp1B7,sizeof(void*),1));}});}void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);{struct Cyc_Tcenv_Fenv _tmp1BD;struct Cyc_RgnOrder_RgnPO*_tmp1BE;struct
Cyc_Tcenv_Fenv*_tmp1BC=Cyc_Tcenv_get_fenv(te,({const char*_tmp1BB="check_rgn_resetable";
_tag_dynforward(_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,20));}));_tmp1BD=*
_tmp1BC;_tmp1BE=_tmp1BD.region_order;{void*_tmp1BF=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp1C0;_LL82: if(_tmp1BF <= (void*)4)goto _LL84;if(*((int*)
_tmp1BF)!= 1)goto _LL84;_tmp1C0=((struct Cyc_Absyn_VarType_struct*)_tmp1BF)->f1;
_LL83: if(!Cyc_RgnOrder_is_region_resetable(_tmp1BE,_tmp1C0))({struct Cyc_String_pa_struct
_tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(rgn));{void*_tmp1C1[1]={& _tmp1C3};Cyc_Tcutil_terr(loc,({
const char*_tmp1C2="Region %s is not resetable";_tag_dynforward(_tmp1C2,sizeof(
char),_get_zero_arr_size(_tmp1C2,27));}),_tag_dynforward(_tmp1C1,sizeof(void*),1));}});
return;_LL84:;_LL85:({void*_tmp1C4[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C5="check_rgn_resetable";
_tag_dynforward(_tmp1C5,sizeof(char),_get_zero_arr_size(_tmp1C5,20));}),
_tag_dynforward(_tmp1C4,sizeof(void*),0));});_LL81:;}}}int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){struct Cyc_Tcenv_Fenv*_tmp1C6=te->le;
if(_tmp1C6 == 0)return rt_a == (void*)2;{struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)
_tmp1C6;struct Cyc_Tcenv_Fenv _tmp1C8;struct Cyc_RgnOrder_RgnPO*_tmp1C9;struct Cyc_Tcenv_Fenv*
_tmp1C7=fe;_tmp1C8=*_tmp1C7;_tmp1C9=_tmp1C8.region_order;return Cyc_RgnOrder_effect_outlives(
_tmp1C9,(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1CA=_cycalloc(sizeof(*
_tmp1CA));_tmp1CA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1CB;_tmp1CB.tag=19;
_tmp1CB.f1=(void*)rt_a;_tmp1CB;});_tmp1CA;}),rt_b);}}struct _tuple6{void*f1;void*
f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*f4;};void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff){struct Cyc_Tcenv_Fenv
_tmp1CE;void*_tmp1CF;struct Cyc_RgnOrder_RgnPO*_tmp1D0;struct Cyc_Tcenv_SharedFenv*
_tmp1D1;struct Cyc_Tcenv_Fenv*_tmp1CD=Cyc_Tcenv_get_fenv(te,({const char*_tmp1CC="check_effect_accessible";
_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size(_tmp1CC,24));}));_tmp1CE=*
_tmp1CD;_tmp1CF=(void*)_tmp1CE.capability;_tmp1D0=_tmp1CE.region_order;_tmp1D1=
_tmp1CE.shared;if(Cyc_Tcutil_subset_effect(0,eff,_tmp1CF))return;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp1D0,eff,_tmp1CF))return;{struct _RegionHandle*frgn=_tmp1D1->frgn;_tmp1D1->delayed_effect_checks=({
struct Cyc_List_List*_tmp1D2=_region_malloc(frgn,sizeof(*_tmp1D2));_tmp1D2->hd=({
struct _tuple6*_tmp1D3=_region_malloc(frgn,sizeof(*_tmp1D3));_tmp1D3->f1=_tmp1CF;
_tmp1D3->f2=eff;_tmp1D3->f3=_tmp1D0;_tmp1D3->f4=loc;_tmp1D3;});_tmp1D2->tl=
_tmp1D1->delayed_effect_checks;_tmp1D2;});}}void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1D6;struct Cyc_Tcenv_SharedFenv*
_tmp1D7;struct Cyc_Tcenv_Fenv*_tmp1D5=Cyc_Tcenv_get_fenv(te,({const char*_tmp1D4="check_delayed_effects";
_tag_dynforward(_tmp1D4,sizeof(char),_get_zero_arr_size(_tmp1D4,22));}));_tmp1D6=*
_tmp1D5;_tmp1D7=_tmp1D6.shared;{struct Cyc_List_List*_tmp1D8=_tmp1D7->delayed_effect_checks;
for(0;_tmp1D8 != 0;_tmp1D8=_tmp1D8->tl){struct _tuple6 _tmp1DA;void*_tmp1DB;void*
_tmp1DC;struct Cyc_RgnOrder_RgnPO*_tmp1DD;struct Cyc_Position_Segment*_tmp1DE;
struct _tuple6*_tmp1D9=(struct _tuple6*)_tmp1D8->hd;_tmp1DA=*_tmp1D9;_tmp1DB=
_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;_tmp1DD=_tmp1DA.f3;_tmp1DE=_tmp1DA.f4;if(Cyc_Tcutil_subset_effect(
1,_tmp1DC,_tmp1DB))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp1DD,_tmp1DC,
_tmp1DB))continue;({struct Cyc_String_pa_struct _tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp1DC));{
struct Cyc_String_pa_struct _tmp1E1;_tmp1E1.tag=0;_tmp1E1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp1DB));{void*
_tmp1DF[2]={& _tmp1E1,& _tmp1E2};Cyc_Tcutil_terr(_tmp1DE,({const char*_tmp1E0="Capability \n%s\ndoes not cover function's effect\n%s";
_tag_dynforward(_tmp1E0,sizeof(char),_get_zero_arr_size(_tmp1E0,51));}),
_tag_dynforward(_tmp1DF,sizeof(void*),2));}}});}}}struct _tuple7{struct Cyc_RgnOrder_RgnPO*
f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*f3;};void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){
struct Cyc_Tcenv_Fenv*_tmp1E3=te->le;if(_tmp1E3 == 0){for(0;po != 0;po=po->tl){if(!
Cyc_Tcutil_subset_effect(1,(*((struct _tuple0*)po->hd)).f1,Cyc_Absyn_empty_effect)
 || !Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1E5;_tmp1E5.tag=19;_tmp1E5.f1=(void*)(*((struct _tuple0*)po->hd)).f2;_tmp1E5;});
_tmp1E4;}),Cyc_Absyn_empty_effect))({void*_tmp1E6[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp1E7="the required region ordering is not satisfied here";
_tag_dynforward(_tmp1E7,sizeof(char),_get_zero_arr_size(_tmp1E7,51));}),
_tag_dynforward(_tmp1E6,sizeof(void*),0));});}return;}{struct Cyc_Tcenv_Fenv
_tmp1E9;struct Cyc_RgnOrder_RgnPO*_tmp1EA;struct Cyc_Tcenv_SharedFenv*_tmp1EB;
struct Cyc_Tcenv_Fenv*_tmp1E8=(struct Cyc_Tcenv_Fenv*)_tmp1E3;_tmp1E9=*_tmp1E8;
_tmp1EA=_tmp1E9.region_order;_tmp1EB=_tmp1E9.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp1EA,po,(void*)2,0))_tmp1EB->delayed_constraint_checks=({struct Cyc_List_List*
_tmp1EC=_region_malloc(_tmp1EB->frgn,sizeof(*_tmp1EC));_tmp1EC->hd=({struct
_tuple7*_tmp1ED=_region_malloc(_tmp1EB->frgn,sizeof(*_tmp1ED));_tmp1ED->f1=
_tmp1EA;_tmp1ED->f2=po;_tmp1ED->f3=loc;_tmp1ED;});_tmp1EC->tl=_tmp1EB->delayed_constraint_checks;
_tmp1EC;});}}void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmp1F0;struct Cyc_Tcenv_SharedFenv*_tmp1F1;struct Cyc_Tcenv_Fenv*
_tmp1EF=Cyc_Tcenv_get_fenv(te,({const char*_tmp1EE="check_delayed_constraints";
_tag_dynforward(_tmp1EE,sizeof(char),_get_zero_arr_size(_tmp1EE,26));}));_tmp1F0=*
_tmp1EF;_tmp1F1=_tmp1F0.shared;{struct Cyc_List_List*_tmp1F2=_tmp1F1->delayed_constraint_checks;
for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){struct _tuple7 _tmp1F4;struct Cyc_RgnOrder_RgnPO*
_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_Position_Segment*_tmp1F7;struct
_tuple7*_tmp1F3=(struct _tuple7*)_tmp1F2->hd;_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.f1;
_tmp1F6=_tmp1F4.f2;_tmp1F7=_tmp1F4.f3;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp1F5,_tmp1F6,(void*)2,1))({void*_tmp1F8[0]={};Cyc_Tcutil_terr(_tmp1F7,({const
char*_tmp1F9="the required region ordering is not satisfied here";
_tag_dynforward(_tmp1F9,sizeof(char),_get_zero_arr_size(_tmp1F9,51));}),
_tag_dynforward(_tmp1F8,sizeof(void*),0));});}}}struct _tuple8{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Bindings*locals=0;struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*
_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->name=({struct _dynforward_ptr*
_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A[0]=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp22D;_tmp22D.tag=0;_tmp22D.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*(*fd->name).f2);{void*_tmp22B[1]={& _tmp22D};Cyc_aprintf(({
const char*_tmp22C="`%s";_tag_dynforward(_tmp22C,sizeof(char),_get_zero_arr_size(
_tmp22C,4));}),_tag_dynforward(_tmp22B,sizeof(void*),1));}});_tmp22A;});_tmp227->identity=
Cyc_Tcutil_new_tvar_id();_tmp227->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp229;_tmp229.tag=0;_tmp229.f1=(void*)((void*)3);_tmp229;});_tmp228;}));
_tmp227;});struct Cyc_List_List*_tmp1FA=({struct Cyc_List_List*_tmp226=_cycalloc(
sizeof(*_tmp226));_tmp226->hd=rgn0;_tmp226->tl=fd->tvs;_tmp226;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp1FA);{struct Cyc_RgnOrder_RgnPO*_tmp1FB=Cyc_RgnOrder_initial_fn_po(fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0);void*
param_rgn=(void*)({struct Cyc_Absyn_VarType_struct*_tmp224=_cycalloc(sizeof(*
_tmp224));_tmp224[0]=({struct Cyc_Absyn_VarType_struct _tmp225;_tmp225.tag=1;
_tmp225.f1=rgn0;_tmp225;});_tmp224;});struct Cyc_List_List*_tmp1FC=0;{struct Cyc_List_List*
_tmp1FD=fd->args;for(0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){struct Cyc_Absyn_Vardecl*
_tmp1FE=({struct Cyc_Absyn_Vardecl*_tmp204=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp204[0]=({struct Cyc_Absyn_Vardecl _tmp205;_tmp205.sc=(void*)((void*)2);
_tmp205.name=({struct _tuple1*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->f1=(
void*)((void*)0);_tmp207->f2=(*((struct _tuple8*)_tmp1FD->hd)).f1;_tmp207;});
_tmp205.tq=(*((struct _tuple8*)_tmp1FD->hd)).f2;_tmp205.type=(void*)(*((struct
_tuple8*)_tmp1FD->hd)).f3;_tmp205.initializer=0;_tmp205.rgn=({struct Cyc_Core_Opt*
_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->v=(void*)param_rgn;_tmp206;});
_tmp205.attributes=0;_tmp205.escapes=0;_tmp205;});_tmp204;});_tmp1FC=({struct Cyc_List_List*
_tmp1FF=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp1FF[0]=({struct Cyc_List_List
_tmp200;_tmp200.hd=_tmp1FE;_tmp200.tl=_tmp1FC;_tmp200;});_tmp1FF;});locals=({
struct Cyc_Tcenv_Bindings*_tmp201=_region_malloc(r,sizeof(*_tmp201));_tmp201->v=(*((
struct _tuple8*)_tmp1FD->hd)).f1;_tmp201->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*
_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202[0]=({struct Cyc_Absyn_Param_b_struct
_tmp203;_tmp203.tag=2;_tmp203.f1=_tmp1FE;_tmp203;});_tmp202;}));_tmp201->tl=
locals;_tmp201;});}}if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp209;struct Cyc_Absyn_Tqual
_tmp20A;void*_tmp20B;int _tmp20C;struct Cyc_Absyn_VarargInfo _tmp208=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp209=_tmp208.name;_tmp20A=_tmp208.tq;_tmp20B=(
void*)_tmp208.type;_tmp20C=_tmp208.inject;if(_tmp209 != 0){void*_tmp20D=Cyc_Absyn_dynforward_typ(
_tmp20B,param_rgn,_tmp20A,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*
_tmp20E=({struct Cyc_Absyn_Vardecl*_tmp213=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp213[0]=({struct Cyc_Absyn_Vardecl _tmp214;_tmp214.sc=(void*)((void*)2);
_tmp214.name=({struct _tuple1*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->f1=(
void*)((void*)0);_tmp216->f2=(struct _dynforward_ptr*)_tmp209->v;_tmp216;});
_tmp214.tq=Cyc_Absyn_empty_tqual(0);_tmp214.type=(void*)_tmp20D;_tmp214.initializer=
0;_tmp214.rgn=({struct Cyc_Core_Opt*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->v=(
void*)param_rgn;_tmp215;});_tmp214.attributes=0;_tmp214.escapes=0;_tmp214;});
_tmp213;});_tmp1FC=({struct Cyc_List_List*_tmp20F=_cycalloc(sizeof(*_tmp20F));
_tmp20F->hd=_tmp20E;_tmp20F->tl=_tmp1FC;_tmp20F;});locals=({struct Cyc_Tcenv_Bindings*
_tmp210=_region_malloc(r,sizeof(*_tmp210));_tmp210->v=(struct _dynforward_ptr*)
_tmp209->v;_tmp210->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp211=
_cycalloc(sizeof(*_tmp211));_tmp211[0]=({struct Cyc_Absyn_Param_b_struct _tmp212;
_tmp212.tag=2;_tmp212.f1=_tmp20E;_tmp212;});_tmp211;}));_tmp210->tl=locals;
_tmp210;});}else{({void*_tmp217[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp218="missing name for varargs";
_tag_dynforward(_tmp218,sizeof(char),_get_zero_arr_size(_tmp218,25));}),
_tag_dynforward(_tmp217,sizeof(void*),0));});}}fd->param_vardecls=({struct Cyc_Core_Opt*
_tmp219=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp219[0]=({struct Cyc_Core_Opt
_tmp21A;_tmp21A.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp1FC);_tmp21A;});_tmp219;});return({struct Cyc_Tcenv_Fenv*_tmp21B=
_region_malloc(r,sizeof(*_tmp21B));_tmp21B->shared=(struct Cyc_Tcenv_SharedFenv*)({
struct Cyc_Tcenv_SharedFenv*_tmp223=_region_malloc(r,sizeof(*_tmp223));_tmp223->frgn=
r;_tmp223->return_typ=(void*)((void*)fd->ret_type);_tmp223->seen_labels=((struct
Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp223->needed_labels=((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp223->delayed_effect_checks=
0;_tmp223->delayed_constraint_checks=0;_tmp223;});_tmp21B->type_vars=(struct Cyc_List_List*)
_tmp1FA;_tmp21B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1FB;_tmp21B->locals=(
struct Cyc_Tcenv_Bindings*)locals;_tmp21B->encloser=(struct Cyc_Absyn_Stmt*)fd->body;
_tmp21B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp222=
_region_malloc(r,sizeof(*_tmp222));_tmp222->continue_stmt=(void*)((void*)0);
_tmp222->break_stmt=(void*)((void*)0);_tmp222->fallthru_clause=0;_tmp222->next_stmt=(
void*)((void*)2);_tmp222->try_depth=0;_tmp222;});_tmp21B->capability=(void*)((
void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp21C=_cycalloc(sizeof(*_tmp21C));
_tmp21C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp21D;_tmp21D.tag=20;_tmp21D.f1=({
struct Cyc_List_List*_tmp21E=_cycalloc(sizeof(*_tmp21E));_tmp21E->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp220=_cycalloc(sizeof(*_tmp220));
_tmp220[0]=({struct Cyc_Absyn_AccessEff_struct _tmp221;_tmp221.tag=19;_tmp221.f1=(
void*)param_rgn;_tmp221;});_tmp220;}));_tmp21E->tl=({struct Cyc_List_List*_tmp21F=
_cycalloc(sizeof(*_tmp21F));_tmp21F->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v);_tmp21F->tl=0;_tmp21F;});_tmp21E;});_tmp21D;});
_tmp21C;})));_tmp21B->curr_rgn=(void*)((void*)param_rgn);_tmp21B->in_notreadctxt=(
int)0;_tmp21B;});}}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Fenv*old_fenv,
struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv _tmp22F;struct Cyc_Tcenv_Bindings*
_tmp230;struct Cyc_RgnOrder_RgnPO*_tmp231;struct Cyc_List_List*_tmp232;struct Cyc_Tcenv_Fenv*
_tmp22E=old_fenv;_tmp22F=*_tmp22E;_tmp230=_tmp22F.locals;_tmp231=_tmp22F.region_order;
_tmp232=_tmp22F.type_vars;{struct Cyc_Tcenv_Bindings*_tmp233=(struct Cyc_Tcenv_Bindings*)
_tmp230;struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp261=_cycalloc(
sizeof(*_tmp261));_tmp261->name=({struct _dynforward_ptr*_tmp264=_cycalloc(
sizeof(*_tmp264));_tmp264[0]=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp267;_tmp267.tag=0;_tmp267.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
fd->name).f2);{void*_tmp265[1]={& _tmp267};Cyc_aprintf(({const char*_tmp266="`%s";
_tag_dynforward(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,4));}),
_tag_dynforward(_tmp265,sizeof(void*),1));}});_tmp264;});_tmp261->identity=Cyc_Tcutil_new_tvar_id();
_tmp261->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp262=_cycalloc(
sizeof(*_tmp262));_tmp262[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp263;_tmp263.tag=0;
_tmp263.f1=(void*)((void*)3);_tmp263;});_tmp262;}));_tmp261;});{struct Cyc_List_List*
_tmp234=fd->tvs;for(0;_tmp234 != 0;_tmp234=_tmp234->tl){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp234->hd)== (void*)3)_tmp231=Cyc_RgnOrder_add_youngest(
_tmp231,(struct Cyc_Absyn_Tvar*)_tmp234->hd,0);}}_tmp231=Cyc_RgnOrder_add_youngest(
_tmp231,rgn0,0);{struct Cyc_List_List*_tmp235=({struct Cyc_List_List*_tmp260=
_cycalloc(sizeof(*_tmp260));_tmp260->hd=rgn0;_tmp260->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp232);
_tmp260;});Cyc_Tcutil_check_unique_tvars(loc,_tmp235);{void*param_rgn=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E[0]=({
struct Cyc_Absyn_VarType_struct _tmp25F;_tmp25F.tag=1;_tmp25F.f1=rgn0;_tmp25F;});
_tmp25E;});struct Cyc_List_List*_tmp236=0;{struct Cyc_List_List*_tmp237=fd->args;
for(0;_tmp237 != 0;_tmp237=_tmp237->tl){struct Cyc_Absyn_Vardecl*_tmp238=({struct
Cyc_Absyn_Vardecl*_tmp23E=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp23E[
0]=({struct Cyc_Absyn_Vardecl _tmp23F;_tmp23F.sc=(void*)((void*)2);_tmp23F.name=({
struct _tuple1*_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241->f1=(void*)((void*)0);
_tmp241->f2=(*((struct _tuple8*)_tmp237->hd)).f1;_tmp241;});_tmp23F.tq=(*((struct
_tuple8*)_tmp237->hd)).f2;_tmp23F.type=(void*)(*((struct _tuple8*)_tmp237->hd)).f3;
_tmp23F.initializer=0;_tmp23F.rgn=({struct Cyc_Core_Opt*_tmp240=_cycalloc(sizeof(*
_tmp240));_tmp240->v=(void*)param_rgn;_tmp240;});_tmp23F.attributes=0;_tmp23F.escapes=
0;_tmp23F;});_tmp23E;});_tmp236=({struct Cyc_List_List*_tmp239=_cycalloc(sizeof(
struct Cyc_List_List)* 1);_tmp239[0]=({struct Cyc_List_List _tmp23A;_tmp23A.hd=
_tmp238;_tmp23A.tl=_tmp236;_tmp23A;});_tmp239;});_tmp233=({struct Cyc_Tcenv_Bindings*
_tmp23B=_region_malloc(r,sizeof(*_tmp23B));_tmp23B->v=(*((struct _tuple8*)_tmp237->hd)).f1;
_tmp23B->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp23C=_cycalloc(
sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Absyn_Param_b_struct _tmp23D;_tmp23D.tag=
2;_tmp23D.f1=_tmp238;_tmp23D;});_tmp23C;}));_tmp23B->tl=_tmp233;_tmp23B;});}}if(
fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp243;struct Cyc_Absyn_Tqual _tmp244;
void*_tmp245;int _tmp246;struct Cyc_Absyn_VarargInfo _tmp242=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp243=_tmp242.name;_tmp244=_tmp242.tq;_tmp245=(
void*)_tmp242.type;_tmp246=_tmp242.inject;if(_tmp243 != 0){void*_tmp247=Cyc_Absyn_dynforward_typ(
_tmp245,param_rgn,_tmp244,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*
_tmp248=({struct Cyc_Absyn_Vardecl*_tmp24D=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp24D[0]=({struct Cyc_Absyn_Vardecl _tmp24E;_tmp24E.sc=(void*)((void*)2);
_tmp24E.name=({struct _tuple1*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->f1=(
void*)((void*)0);_tmp250->f2=(struct _dynforward_ptr*)_tmp243->v;_tmp250;});
_tmp24E.tq=Cyc_Absyn_empty_tqual(0);_tmp24E.type=(void*)_tmp247;_tmp24E.initializer=
0;_tmp24E.rgn=({struct Cyc_Core_Opt*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F->v=(
void*)param_rgn;_tmp24F;});_tmp24E.attributes=0;_tmp24E.escapes=0;_tmp24E;});
_tmp24D;});_tmp236=({struct Cyc_List_List*_tmp249=_cycalloc(sizeof(*_tmp249));
_tmp249->hd=_tmp248;_tmp249->tl=_tmp236;_tmp249;});_tmp233=({struct Cyc_Tcenv_Bindings*
_tmp24A=_region_malloc(r,sizeof(*_tmp24A));_tmp24A->v=(struct _dynforward_ptr*)
_tmp243->v;_tmp24A->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp24B=
_cycalloc(sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Absyn_Param_b_struct _tmp24C;
_tmp24C.tag=2;_tmp24C.f1=_tmp248;_tmp24C;});_tmp24B;}));_tmp24A->tl=_tmp233;
_tmp24A;});}else{({void*_tmp251[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp252="missing name for varargs";
_tag_dynforward(_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,25));}),
_tag_dynforward(_tmp251,sizeof(void*),0));});}}fd->param_vardecls=({struct Cyc_Core_Opt*
_tmp253=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp253[0]=({struct Cyc_Core_Opt
_tmp254;_tmp254.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp236);_tmp254;});_tmp253;});return({struct Cyc_Tcenv_Fenv*_tmp255=
_region_malloc(r,sizeof(*_tmp255));_tmp255->shared=(struct Cyc_Tcenv_SharedFenv*)({
struct Cyc_Tcenv_SharedFenv*_tmp25D=_region_malloc(r,sizeof(*_tmp25D));_tmp25D->frgn=
r;_tmp25D->return_typ=(void*)((void*)fd->ret_type);_tmp25D->seen_labels=((struct
Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp25D->needed_labels=((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp25D->delayed_effect_checks=
0;_tmp25D->delayed_constraint_checks=0;_tmp25D;});_tmp255->type_vars=(struct Cyc_List_List*)
_tmp235;_tmp255->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp231;_tmp255->locals=(
struct Cyc_Tcenv_Bindings*)_tmp233;_tmp255->encloser=(struct Cyc_Absyn_Stmt*)fd->body;
_tmp255->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp25C=
_region_malloc(r,sizeof(*_tmp25C));_tmp25C->continue_stmt=(void*)((void*)0);
_tmp25C->break_stmt=(void*)((void*)0);_tmp25C->fallthru_clause=0;_tmp25C->next_stmt=(
void*)((void*)2);_tmp25C->try_depth=0;_tmp25C;});_tmp255->capability=(void*)((
void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp256=_cycalloc(sizeof(*_tmp256));
_tmp256[0]=({struct Cyc_Absyn_JoinEff_struct _tmp257;_tmp257.tag=20;_tmp257.f1=({
struct Cyc_List_List*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));
_tmp25A[0]=({struct Cyc_Absyn_AccessEff_struct _tmp25B;_tmp25B.tag=19;_tmp25B.f1=(
void*)param_rgn;_tmp25B;});_tmp25A;}));_tmp258->tl=({struct Cyc_List_List*_tmp259=
_cycalloc(sizeof(*_tmp259));_tmp259->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v);_tmp259->tl=0;_tmp259;});_tmp258;});_tmp257;});
_tmp256;})));_tmp255->curr_rgn=(void*)((void*)param_rgn);_tmp255->in_notreadctxt=(
int)0;_tmp255;});}}}}
