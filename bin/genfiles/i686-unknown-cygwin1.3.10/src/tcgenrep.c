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
 struct Cyc_Core_Opt{void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};int isalnum(int);struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*
env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];void*Cyc_List_fold_left(
void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);void*Cyc_List_fold_right_c(
void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_forall(
int(*pred)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_zip3(struct
Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct
Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict*
d);struct _tuple1{void*f1;void*f2;};struct _tuple1*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple1*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_delete(struct
Cyc_Dict_Dict*,void*);unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strdup(
struct _dynforward_ptr src);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple2{unsigned int f1;struct _dynforward_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dynforward_ptr*f1;unsigned int f2;struct _dynforward_ptr f3;};struct
_tuple3{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dynforward_ptr f2;};struct _tuple4{unsigned int f1;struct
_dynforward_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dynforward_ptr
f1;struct _dynforward_ptr f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;unsigned int f3;struct
_dynforward_ptr f4;};struct _tuple5{struct _dynforward_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc_timespec{long tv_sec;long tv_nsec;};struct Cyc_itimerspec{
struct Cyc_timespec it_interval;struct Cyc_timespec it_value;};struct Cyc_tm{int
tm_sec;int tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int
tm_yday;int tm_isdst;};struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple6{union Cyc_Absyn_Nmspace_union
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
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
_tuple6*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;
struct Cyc_List_List*targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple6*tunion_name;struct _tuple6*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple6*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple6*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple6*f1;
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
_tuple6*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple6*f1;
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
struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple6*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple6*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple6*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple6*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple6*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple6*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple6*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple6*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple6*,struct _tuple6*);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dynforward_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple6*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple6*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_typ2cstring(void*);
struct _tuple9{unsigned int f1;int f2;};struct _tuple9 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
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
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*aggrdecls;
struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_typecmp(void*,
void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*,int allow_unique);struct
Cyc_Tcgenrep_RepInfo{struct Cyc_List_List*decls;struct Cyc_Absyn_Exp*exp;struct Cyc_List_List*
dependencies;struct Cyc_Core_Opt*fwd_decl;int emitted;int is_extern;};struct Cyc_Dict_Dict*
Cyc_Tcgenrep_empty_typerep_dict(){return((struct Cyc_Dict_Dict*(*)(int(*cmp)(void*,
void*)))Cyc_Dict_empty)(Cyc_Tcutil_typecmp);}void Cyc_Tcgenrep_print_dict_entry(
void*type,struct Cyc_Tcgenrep_RepInfo*info){({struct Cyc_Int_pa_struct _tmp4;_tmp4.tag=
1;_tmp4.f1=(unsigned long)info->emitted;{struct Cyc_Int_pa_struct _tmp3;_tmp3.tag=
1;_tmp3.f1=(unsigned int)info;{struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(type));{
void*_tmp0[3]={& _tmp2,& _tmp3,& _tmp4};Cyc_printf(({const char*_tmp1="(%s,%x:%d,";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,11));}),
_tag_dynforward(_tmp0,sizeof(void*),3));}}}});{struct Cyc_List_List*p=info->dependencies;
for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct _tmp7;_tmp7.tag=1;_tmp7.f1=(
unsigned int)((struct Cyc_Tcgenrep_RepInfo*)p->hd);{void*_tmp5[1]={& _tmp7};Cyc_printf(({
const char*_tmp6="%x";_tag_dynforward(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,
3));}),_tag_dynforward(_tmp5,sizeof(void*),1));}});if(p->tl != 0)({void*_tmp8[0]={};
Cyc_printf(({const char*_tmp9=",";_tag_dynforward(_tmp9,sizeof(char),
_get_zero_arr_size(_tmp9,2));}),_tag_dynforward(_tmp8,sizeof(void*),0));});}}({
void*_tmpA[0]={};Cyc_printf(({const char*_tmpB=")\n";_tag_dynforward(_tmpB,
sizeof(char),_get_zero_arr_size(_tmpB,3));}),_tag_dynforward(_tmpA,sizeof(void*),
0));});}void Cyc_Tcgenrep_print_typerep_dict(struct Cyc_Dict_Dict*dict){((void(*)(
void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Dict_Dict*d))Cyc_Dict_iter)(
Cyc_Tcgenrep_print_dict_entry,dict);}static int Cyc_Tcgenrep_rephash(struct Cyc_Tcgenrep_RepInfo*
ri){return(int)ri;}static int Cyc_Tcgenrep_repcmp(struct Cyc_Tcgenrep_RepInfo*r1,
struct Cyc_Tcgenrep_RepInfo*r2){unsigned int r1p;unsigned int r2p;r1p=(unsigned int)
r1;r2p=(unsigned int)r2;if(r1 < r2)return - 1;else{if(r1 == r2)return 0;else{return 1;}}}
static struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_getField(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple6*fieldname){if(td->fields == 0)({void*_tmpC[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpD="Could not find field in tuniondecl";
_tag_dynforward(_tmpD,sizeof(char),_get_zero_arr_size(_tmpD,35));}),
_tag_dynforward(_tmpC,sizeof(void*),0));});else{struct Cyc_List_List*l=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;l != 0;l=l->tl){if(!Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)l->hd)->name,fieldname))return(struct Cyc_Absyn_Tunionfield*)
l->hd;}}({void*_tmpE[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF="Could not find field in tuniondecl";
_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,35));}),
_tag_dynforward(_tmpE,sizeof(void*),0));});}static char _tmp10[8]="Typerep";static
struct _dynforward_ptr Cyc_Tcgenrep_typerep_nm={_tmp10,_tmp10 + 8};static struct Cyc_List_List
Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,0};static struct Cyc_List_List*
Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*ri,struct Cyc_Hashtable_Table*
visited){struct _handler_cons _tmp11;_push_handler(& _tmp11);{int _tmp13=0;if(setjmp(
_tmp11.handler))_tmp13=1;if(!_tmp13){((int(*)(struct Cyc_Hashtable_Table*t,struct
Cyc_Tcgenrep_RepInfo*key))Cyc_Hashtable_lookup)(visited,ri);{struct Cyc_List_List*
_tmp14=0;_npop_handler(0);return _tmp14;};_pop_handler();}else{void*_tmp12=(void*)
_exn_thrown;void*_tmp16=_tmp12;_LL1: if(_tmp16 != Cyc_Core_Not_found)goto _LL3;_LL2:((
void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(
visited,ri,1);{struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;
for(0;l != 0;l=l->tl){ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_imp_append)(ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)
l->hd,visited));}}if(ri->emitted == 0){if(ri->fwd_decl != 0)ds=({struct Cyc_List_List*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=({struct Cyc_Absyn_Decl*_tmp18[1];
_tmp18[0]=(struct Cyc_Absyn_Decl*)((struct Cyc_Core_Opt*)_check_null(ri->fwd_decl))->v;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp18,sizeof(struct Cyc_Absyn_Decl*),1));});_tmp17->tl=ds;_tmp17;});{struct Cyc_List_List*
_tmp19=({struct Cyc_List_List*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->hd=ri->decls;
_tmp1A->tl=0;_tmp1A;});ri->emitted=1;return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(ds,_tmp19);}}else{return ds;}}_LL3:;
_LL4:(void)_throw(_tmp16);_LL0:;}}}static struct Cyc_List_List*Cyc_Tcgenrep_dfs(
struct Cyc_Tcgenrep_RepInfo*ri){struct Cyc_Hashtable_Table*tab=((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct Cyc_Tcgenrep_RepInfo*,struct Cyc_Tcgenrep_RepInfo*),int(*
hash)(struct Cyc_Tcgenrep_RepInfo*)))Cyc_Hashtable_create)(53,Cyc_Tcgenrep_repcmp,
Cyc_Tcgenrep_rephash);struct Cyc_List_List*ds=Cyc_Tcgenrep_dfsvisit(ri,tab);
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(ds);}
static char _tmp1B[11]="Typestruct";static struct _dynforward_ptr Cyc_Tcgenrep_typestruct_str={
_tmp1B,_tmp1B + 11};static char _tmp1C[4]="Var";static struct _dynforward_ptr Cyc_Tcgenrep_var_str={
_tmp1C,_tmp1C + 4};static char _tmp1D[4]="Int";static struct _dynforward_ptr Cyc_Tcgenrep_int_str={
_tmp1D,_tmp1D + 4};static char _tmp1E[6]="Float";static struct _dynforward_ptr Cyc_Tcgenrep_float_str={
_tmp1E,_tmp1E + 6};static char _tmp1F[7]="Double";static struct _dynforward_ptr Cyc_Tcgenrep_double_str={
_tmp1F,_tmp1F + 7};static char _tmp20[8]="ThinPtr";static struct _dynforward_ptr Cyc_Tcgenrep_thinptr_str={
_tmp20,_tmp20 + 8};static char _tmp21[7]="FatPtr";static struct _dynforward_ptr Cyc_Tcgenrep_fatptr_str={
_tmp21,_tmp21 + 7};static char _tmp22[6]="Tuple";static struct _dynforward_ptr Cyc_Tcgenrep_tuple_str={
_tmp22,_tmp22 + 6};static char _tmp23[12]="TUnionField";static struct _dynforward_ptr
Cyc_Tcgenrep_tunionfield_str={_tmp23,_tmp23 + 12};static char _tmp24[7]="Struct";
static struct _dynforward_ptr Cyc_Tcgenrep_struct_str={_tmp24,_tmp24 + 7};static char
_tmp25[7]="TUnion";static struct _dynforward_ptr Cyc_Tcgenrep_tunion_str={_tmp25,
_tmp25 + 7};static char _tmp26[8]="XTUnion";static struct _dynforward_ptr Cyc_Tcgenrep_xtunion_str={
_tmp26,_tmp26 + 8};static char _tmp27[6]="Union";static struct _dynforward_ptr Cyc_Tcgenrep_union_str={
_tmp27,_tmp27 + 6};static char _tmp28[5]="Enum";static struct _dynforward_ptr Cyc_Tcgenrep_enum_str={
_tmp28,_tmp28 + 5};static char _tmp29[7]="name_t";static struct _dynforward_ptr Cyc_Tcgenrep_name_t_str={
_tmp29,_tmp29 + 7};static struct _tuple6*Cyc_Tcgenrep_typerep_name(struct
_dynforward_ptr*name){return({struct _tuple6*_tmp2A=_cycalloc(sizeof(*_tmp2A));
_tmp2A->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp2B;(
_tmp2B.Abs_n).tag=2;(_tmp2B.Abs_n).f1=(struct Cyc_List_List*)& Cyc_Tcgenrep_l2;
_tmp2B;});_tmp2A->f2=name;_tmp2A;});}static int Cyc_Tcgenrep_gen_id_counter=0;
static struct _dynforward_ptr*Cyc_Tcgenrep_new_gen_id(struct _dynforward_ptr name){
return({struct _dynforward_ptr*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp30;_tmp30.tag=1;_tmp30.f1=(
unsigned long)Cyc_Tcgenrep_gen_id_counter ++;{struct Cyc_String_pa_struct _tmp2F;
_tmp2F.tag=0;_tmp2F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)name);{
void*_tmp2D[2]={& _tmp2F,& _tmp30};Cyc_aprintf(({const char*_tmp2E="_gen%s_%d";
_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,10));}),
_tag_dynforward(_tmp2D,sizeof(void*),2));}}});_tmp2C;});}static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List*l){({void*_tmp31[0]={};Cyc_printf(({const char*_tmp32="<";
_tag_dynforward(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,2));}),
_tag_dynforward(_tmp31,sizeof(void*),0));});{struct Cyc_List_List*p=l;for(0;p != 0;
p=p->tl){({struct Cyc_Int_pa_struct _tmp36;_tmp36.tag=1;_tmp36.f1=(unsigned long)((
int)(p->tl != 0?',':' '));{struct Cyc_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)p->hd));{
void*_tmp33[2]={& _tmp35,& _tmp36};Cyc_printf(({const char*_tmp34="%s%c";
_tag_dynforward(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,5));}),
_tag_dynforward(_tmp33,sizeof(void*),2));}}});}}({void*_tmp37[0]={};Cyc_printf(({
const char*_tmp38=">\n";_tag_dynforward(_tmp38,sizeof(char),_get_zero_arr_size(
_tmp38,3));}),_tag_dynforward(_tmp37,sizeof(void*),0));});}static void Cyc_Tcgenrep_print_tvars(
struct Cyc_List_List*l){({void*_tmp39[0]={};Cyc_printf(({const char*_tmp3A="<";
_tag_dynforward(_tmp3A,sizeof(char),_get_zero_arr_size(_tmp3A,2));}),
_tag_dynforward(_tmp39,sizeof(void*),0));});{struct Cyc_List_List*p=l;for(0;p != 0;
p=p->tl){({struct Cyc_Int_pa_struct _tmp3E;_tmp3E.tag=1;_tmp3E.f1=(unsigned long)((
int)(p->tl != 0?',':' '));{struct Cyc_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Tvar*)p->hd)->name);{
void*_tmp3B[2]={& _tmp3D,& _tmp3E};Cyc_printf(({const char*_tmp3C="%s%c";
_tag_dynforward(_tmp3C,sizeof(char),_get_zero_arr_size(_tmp3C,5));}),
_tag_dynforward(_tmp3B,sizeof(void*),2));}}});}}({void*_tmp3F[0]={};Cyc_printf(({
const char*_tmp40=">\n";_tag_dynforward(_tmp40,sizeof(char),_get_zero_arr_size(
_tmp40,3));}),_tag_dynforward(_tmp3F,sizeof(void*),0));});}static struct _tuple6*
Cyc_Tcgenrep_toplevel_name(struct _dynforward_ptr*name){return({struct _tuple6*
_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp42;(_tmp42.Rel_n).tag=1;(_tmp42.Rel_n).f1=0;
_tmp42;});_tmp41->f2=(struct _dynforward_ptr*)name;_tmp41;});}static struct _tuple6*
Cyc_Tcgenrep_relative_name(struct Cyc_List_List*pathl,struct _dynforward_ptr*name){
return({struct _tuple6*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp44;(_tmp44.Rel_n).tag=1;(_tmp44.Rel_n).f1=pathl;
_tmp44;});_tmp43->f2=(struct _dynforward_ptr*)name;_tmp43;});}static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_decl(struct _dynforward_ptr*name,void*type,struct Cyc_Absyn_Exp*
init,struct Cyc_Position_Segment*seg){struct _tuple6*qvar=Cyc_Tcgenrep_toplevel_name(
name);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(qvar,type,init);(void*)(
vd->sc=(void*)((void*)0));{struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*
_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Absyn_Var_d_struct _tmp46;
_tmp46.tag=0;_tmp46.f1=vd;_tmp46;});_tmp45;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(
r2,seg);}}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(struct _tuple6*name,
void*type,struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*seg){
struct _tuple6*topname=Cyc_Tcgenrep_toplevel_name((*name).f2);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(topname,type,init);(void*)(vd->sc=(void*)sc);{struct Cyc_Absyn_Var_d_struct*
r1=({struct Cyc_Absyn_Var_d_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({
struct Cyc_Absyn_Var_d_struct _tmp48;_tmp48.tag=0;_tmp48.f1=vd;_tmp48;});_tmp47;});
void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_string(struct _dynforward_ptr s,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp49;(_tmp49.String_c).tag=5;(_tmp49.String_c).f1=s;
_tmp49;})),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string_cls(struct
Cyc_Position_Segment*seg,struct _dynforward_ptr*s){return Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp4A;(_tmp4A.String_c).tag=5;(_tmp4A.String_c).f1=*s;_tmp4A;})),seg);}static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_qvar_string_cls(struct Cyc_Position_Segment*
seg,struct _tuple6*s){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp4B;(_tmp4B.String_c).tag=
5;(_tmp4B.String_c).f1=*(*s).f2;_tmp4B;})),seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(
int i,struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp4C;(_tmp4C.Int_c).tag=2;(
_tmp4C.Int_c).f1=(void*)((void*)0);(_tmp4C.Int_c).f2=i;_tmp4C;})),seg);}static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int_cls(struct Cyc_Position_Segment*seg,int
i){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp4D;(_tmp4D.Int_c).tag=2;(_tmp4D.Int_c).f1=(void*)((
void*)0);(_tmp4D.Int_c).f2=i;_tmp4D;})),seg);}static int Cyc_Tcgenrep_size_of2int(
void*sz){void*_tmp4E=sz;_LL6: if((int)_tmp4E != 0)goto _LL8;_LL7: return 8;_LL8: if((
int)_tmp4E != 1)goto _LLA;_LL9: return 16;_LLA: if((int)_tmp4E != 2)goto _LLC;_LLB:
return 32;_LLC: if((int)_tmp4E != 3)goto _LLE;_LLD: return 32;_LLE: if((int)_tmp4E != 4)
goto _LL5;_LLF: return 64;_LL5:;}static void*Cyc_Tcgenrep_tunion_typ(struct _tuple6*
name){return(void*)({struct Cyc_Absyn_TunionType_struct*_tmp4F=_cycalloc(sizeof(*
_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_TunionType_struct _tmp50;_tmp50.tag=2;_tmp50.f1=({
struct Cyc_Absyn_TunionInfo _tmp51;_tmp51.tunion_info=(void*)((void*)({struct Cyc_Absyn_UnknownTunion_struct*
_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_Absyn_UnknownTunion_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=({struct Cyc_Absyn_UnknownTunionInfo _tmp55;_tmp55.name=
name;_tmp55.is_xtunion=0;_tmp55.is_flat=0;_tmp55;});_tmp54;});_tmp53;}));_tmp51.targs=
0;_tmp51.rgn=({struct Cyc_Core_Opt*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->v=(
void*)((void*)2);_tmp52;});_tmp51;});_tmp50;});_tmp4F;});}static void*Cyc_Tcgenrep_tunionfield_typ(
struct _tuple6*name,struct _tuple6*fieldname){return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp57;_tmp57.tag=3;_tmp57.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp58;_tmp58.field_info=(
void*)((void*)({struct Cyc_Absyn_UnknownTunionfield_struct*_tmp59=_cycalloc(
sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Absyn_UnknownTunionfield_struct _tmp5A;
_tmp5A.tag=0;_tmp5A.f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp5B;_tmp5B.tunion_name=
name;_tmp5B.field_name=fieldname;_tmp5B.is_xtunion=0;_tmp5B;});_tmp5A;});_tmp59;}));
_tmp58.targs=0;_tmp58;});_tmp57;});_tmp56;});}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_call_exp(
struct _tuple6*name,struct Cyc_List_List*args,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(name,loc),args,loc);}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(struct _tuple6*
tunionname,struct _tuple6*fieldname,struct _tuple6*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp5C=Cyc_Tcgenrep_tunionfield_typ(
tunionname,fieldname);struct Cyc_Absyn_Exp*_tmp5D=Cyc_Tcgenrep_call_exp(fieldname,
args,loc);struct Cyc_Absyn_Decl*_tmp5E=Cyc_Tcgenrep_gen_vardecl(varname,_tmp5C,(
struct Cyc_Absyn_Exp*)_tmp5D,sc,loc);return _tmp5E;}struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple10*pair){return(*
pair).f2;}struct _tuple11{struct _dynforward_ptr*f1;struct Cyc_Absyn_Exp*f2;};
static struct _dynforward_ptr*Cyc_Tcgenrep_get_first(struct _tuple11*pair){return(*
pair).f1;}static char _tmp5F[5]="list";static struct _dynforward_ptr Cyc_Tcgenrep_list_str={
_tmp5F,_tmp5F + 5};static char _tmp60[5]="List";static struct _dynforward_ptr Cyc_Tcgenrep_List_str={
_tmp60,_tmp60 + 5};struct _tuple12{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;}
;static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(struct Cyc_Position_Segment*
loc,struct _tuple12*es){struct _tuple12 _tmp62;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Exp*
_tmp64;struct _tuple12*_tmp61=es;_tmp62=*_tmp61;_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;
return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*_tmp65[2];_tmp65[1]=_tmp64;
_tmp65[0]=_tmp63;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp65,sizeof(struct Cyc_Absyn_Exp*),2));}),loc);}struct _tuple13{
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple3_exp_cls(struct Cyc_Position_Segment*loc,
struct _tuple13*es){struct _tuple13 _tmp67;struct Cyc_Absyn_Exp*_tmp68;struct Cyc_Absyn_Exp*
_tmp69;struct Cyc_Absyn_Exp*_tmp6A;struct _tuple13*_tmp66=es;_tmp67=*_tmp66;_tmp68=
_tmp67.f1;_tmp69=_tmp67.f2;_tmp6A=_tmp67.f3;return Cyc_Absyn_tuple_exp(({struct
Cyc_Absyn_Exp*_tmp6B[3];_tmp6B[2]=_tmp6A;_tmp6B[1]=_tmp69;_tmp6B[0]=_tmp68;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp6B,sizeof(struct Cyc_Absyn_Exp*),3));}),loc);}static char _tmp6C[5]="NULL";
static struct _dynforward_ptr Cyc_Tcgenrep_null_str={_tmp6C,_tmp6C + 5};static struct
Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct Cyc_List_List*l,struct Cyc_Position_Segment*
loc){if(l == 0)return Cyc_Absyn_null_exp(loc);return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(&
Cyc_Tcgenrep_list_str),l,loc);}struct _tuple14{void*f1;struct Cyc_Position_Segment*
f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_make_offsetof_exp(struct _tuple14*
typeloc,int index){struct _tuple14 _tmp6E;void*_tmp6F;struct Cyc_Position_Segment*
_tmp70;struct _tuple14*_tmp6D=typeloc;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.f1;_tmp70=
_tmp6E.f2;return Cyc_Absyn_offsetof_exp(_tmp6F,(void*)({struct Cyc_Absyn_TupleIndex_struct*
_tmp71=_cycalloc_atomic(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp72;_tmp72.tag=1;_tmp72.f1=(unsigned int)index;_tmp72;});_tmp71;}),_tmp70);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_get_and_cast_ri_exp(struct Cyc_Position_Segment*
loc,struct Cyc_Tcgenrep_RepInfo*info){return Cyc_Absyn_cast_exp(Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),(struct Cyc_Absyn_Exp*)
_check_null(info->exp),1,(void*)0,loc);}static struct _tuple11*Cyc_Tcgenrep_gen_id_for_exp(
struct Cyc_Absyn_Exp*e){return({struct _tuple11*_tmp73=_cycalloc(sizeof(*_tmp73));
_tmp73->f1=Cyc_Tcgenrep_new_gen_id(({const char*_tmp74="tuple";_tag_dynforward(
_tmp74,sizeof(char),_get_zero_arr_size(_tmp74,6));}));_tmp73->f2=e;_tmp73;});}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(struct _tuple14*env,struct
_tuple11*name_exp){struct _tuple14 _tmp76;void*_tmp77;struct Cyc_Position_Segment*
_tmp78;struct _tuple14*_tmp75=env;_tmp76=*_tmp75;_tmp77=_tmp76.f1;_tmp78=_tmp76.f2;{
struct _tuple11 _tmp7A;struct _dynforward_ptr*_tmp7B;struct Cyc_Absyn_Exp*_tmp7C;
struct _tuple11*_tmp79=name_exp;_tmp7A=*_tmp79;_tmp7B=_tmp7A.f1;_tmp7C=_tmp7A.f2;
return Cyc_Tcgenrep_gen_decl(_tmp7B,_tmp77,(struct Cyc_Absyn_Exp*)_tmp7C,_tmp78);}}
struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple15*Cyc_Tcgenrep_null_designator_exp(struct Cyc_Absyn_Exp*e){return({struct
_tuple15*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=0;_tmp7D->f2=e;_tmp7D;});}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*l,struct
Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp7E=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(0,_tmp7E,
loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple6*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Aggrfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Aggrfield*sf){unsigned int _tmp80;int _tmp81;struct
_tuple9 _tmp7F=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(sf->width));
_tmp80=_tmp7F.f1;_tmp81=_tmp7F.f2;if(!_tmp81)({void*_tmp82[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp83="add_bitfield_sizes: sizeof or offsetof in bitfield size";
_tag_dynforward(_tmp83,sizeof(char),_get_zero_arr_size(_tmp83,56));}),
_tag_dynforward(_tmp82,sizeof(void*),0));});return(int)(_tmp80 + total);}static
void*Cyc_Tcgenrep_select_structfield_type(struct Cyc_Absyn_Aggrfield*sf){if(Cyc_strcmp((
struct _dynforward_ptr)*sf->name,({const char*_tmp84="";_tag_dynforward(_tmp84,
sizeof(char),_get_zero_arr_size(_tmp84,1));}))== 0)({void*_tmp85[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp86="gen(): anonymous (padding) structfield not handled yet";_tag_dynforward(
_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,55));}),_tag_dynforward(_tmp85,
sizeof(void*),0));});if(Cyc_Tcgenrep_has_bitfield(sf))({void*_tmp87[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp88="gen(): mixed struct bitfields and union bitfields not handled";
_tag_dynforward(_tmp88,sizeof(char),_get_zero_arr_size(_tmp88,62));}),
_tag_dynforward(_tmp87,sizeof(void*),0));});return(void*)sf->type;}struct
_tuple16{struct _dynforward_ptr*f1;void*f2;};static struct _tuple16*Cyc_Tcgenrep_select_structfield_nmtype(
struct Cyc_Absyn_Aggrfield*sf){if(Cyc_strcmp((struct _dynforward_ptr)*sf->name,({
const char*_tmp89="";_tag_dynforward(_tmp89,sizeof(char),_get_zero_arr_size(
_tmp89,1));}))== 0)({void*_tmp8A[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8B="gen(): anonymous (padding) structfield not handled yet";
_tag_dynforward(_tmp8B,sizeof(char),_get_zero_arr_size(_tmp8B,55));}),
_tag_dynforward(_tmp8A,sizeof(void*),0));});if(Cyc_Tcgenrep_has_bitfield(sf))({
void*_tmp8C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp8D="gen(): mixed struct bitfields and union bitfields not handled";
_tag_dynforward(_tmp8D,sizeof(char),_get_zero_arr_size(_tmp8D,62));}),
_tag_dynforward(_tmp8C,sizeof(void*),0));});return({struct _tuple16*_tmp8E=
_cycalloc(sizeof(*_tmp8E));_tmp8E->f1=sf->name;_tmp8E->f2=(void*)sf->type;_tmp8E;});}
struct _tuple17{int f1;struct _tuple6*f2;};struct _tuple17*Cyc_Tcgenrep_select_enumfield_tagnm(
struct Cyc_Absyn_Enumfield*ef){if(ef->tag == 0)({void*_tmp8F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp90="Enum tag exp should be filled in by now";
_tag_dynforward(_tmp90,sizeof(char),_get_zero_arr_size(_tmp90,40));}),
_tag_dynforward(_tmp8F,sizeof(void*),0));});return({struct _tuple17*_tmp91=
_cycalloc(sizeof(*_tmp91));_tmp91->f1=(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag))).f1;_tmp91->f2=ef->name;_tmp91;});}
static struct Cyc_Dict_Dict*Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict*dict,void*
type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Core_Opt*_tmp92=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp92 != 0){if(((
struct Cyc_Tcgenrep_RepInfo*)_tmp92->v)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);({void*_tmp93[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp94="Updating non-forward declaration";
_tag_dynforward(_tmp94,sizeof(char),_get_zero_arr_size(_tmp94,33));}),
_tag_dynforward(_tmp93,sizeof(void*),0));});}((struct Cyc_Tcgenrep_RepInfo*)
_tmp92->v)->decls=decls;((struct Cyc_Tcgenrep_RepInfo*)_tmp92->v)->exp=exp;((
struct Cyc_Tcgenrep_RepInfo*)_tmp92->v)->dependencies=dependencies;return dict;}
else{return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*_tmp95=_cycalloc(
sizeof(*_tmp95));_tmp95->decls=decls;_tmp95->exp=exp;_tmp95->dependencies=
dependencies;_tmp95->fwd_decl=fwd_decl;_tmp95->emitted=0;_tmp95->is_extern=0;
_tmp95;}));}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict*
dict,void*type,struct Cyc_Absyn_Exp*exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){
struct Cyc_Core_Opt*_tmp96=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))
Cyc_Dict_lookup_opt)(dict,type);if(_tmp96 != 0){({struct Cyc_String_pa_struct
_tmp99;_tmp99.tag=0;_tmp99.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
type));{void*_tmp97[1]={& _tmp99};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp98="Repinfo for fwd declaration of %s already exists";
_tag_dynforward(_tmp98,sizeof(char),_get_zero_arr_size(_tmp98,49));}),
_tag_dynforward(_tmp97,sizeof(void*),1));}});return dict;}else{return((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,
type,({struct Cyc_Tcgenrep_RepInfo*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->decls=
0;_tmp9A->exp=exp;_tmp9A->dependencies=0;_tmp9A->fwd_decl=fwd_decl;_tmp9A->emitted=
0;_tmp9A->is_extern=is_extern;_tmp9A;}));}}static struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={
0,0,0,0,0};static struct _tuple10*Cyc_Tcgenrep_tqual_type(struct Cyc_Absyn_Tqual*tq,
void*type){return({struct _tuple10*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->f1=*((
struct Cyc_Absyn_Tqual*)_check_null(tq));_tmp9B->f2=type;_tmp9B;});}static void*
Cyc_Tcgenrep_tuple_typ(struct Cyc_List_List*types){struct Cyc_List_List*_tmp9C=((
struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Tqual*,void*),struct
Cyc_Absyn_Tqual*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tqual_type,(
struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,types);struct Cyc_Absyn_TupleType_struct*
tuple_type_base=({struct Cyc_Absyn_TupleType_struct*_tmp9D=_cycalloc(sizeof(*
_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_TupleType_struct _tmp9E;_tmp9E.tag=9;_tmp9E.f1=
_tmp9C;_tmp9E;});_tmp9D;});void*tuple_type=(void*)tuple_type_base;return
tuple_type;}static struct _tuple15*Cyc_Tcgenrep_array_decls(void*type,struct Cyc_List_List*
exps,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_gen_id_for_exp,exps);struct Cyc_List_List*_tmpA0=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple14*,struct _tuple11*),struct _tuple14*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({struct
_tuple14*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->f1=type;_tmpAE->f2=loc;_tmpAE;}),
_tmp9F);struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(struct
_dynforward_ptr*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,
_tmp9F);struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,
_tmpA1);struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmpA2);
struct Cyc_List_List*_tmpA4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_address_exp_cls,loc,_tmpA3);
struct Cyc_Absyn_Exp*_tmpA5=Cyc_Tcgenrep_arr_init_exp(_tmpA4,loc);struct
_dynforward_ptr*_tmpA6=Cyc_Tcgenrep_new_gen_id(({const char*_tmpAD="arr";
_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size(_tmpAD,4));}));void*
_tmpA7=Cyc_Absyn_at_typ(type,(void*)2,Cyc_Tcgenrep_tq_none,Cyc_Absyn_false_conref);
void*_tmpA8=Cyc_Absyn_array_typ(_tmpA7,Cyc_Tcgenrep_tq_none,0,Cyc_Absyn_false_conref,
0);struct Cyc_Absyn_Decl*_tmpA9=Cyc_Tcgenrep_gen_decl(_tmpA6,_tmpA8,(struct Cyc_Absyn_Exp*)
_tmpA5,loc);struct Cyc_Absyn_Exp*_tmpAA=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmpA6),loc);return({struct _tuple15*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmpA0,({struct Cyc_Absyn_Decl*_tmpAC[1];_tmpAC[0]=_tmpA9;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpAC,sizeof(struct Cyc_Absyn_Decl*),
1));}));_tmpAB->f2=_tmpAA;_tmpAB;});}static void*Cyc_Tcgenrep_check_tunionfield_and_get_type(
struct Cyc_Absyn_Tunionfield*tuf){return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_TupleType_struct
_tmpB0;_tmpB0.tag=9;_tmpB0.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple10*_tmpB1[1];_tmpB1[0]=({
struct _tuple10*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->f1=Cyc_Tcgenrep_tq_none;
_tmpB2->f2=Cyc_Absyn_uint_typ;_tmpB2;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpB1,sizeof(struct _tuple10*),1));}),
tuf->typs);_tmpB0;});_tmpAF;});}static struct _tuple6*Cyc_Tcgenrep_check_tunionfield_and_get_name(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->name;}struct _tuple18{struct _tuple6*
f1;void*f2;};static struct _tuple18*Cyc_Tcgenrep_check_tunionfield_and_get_nmtype(
struct Cyc_Absyn_Tunionfield*tuf){return({struct _tuple18*_tmpB3=_cycalloc(sizeof(*
_tmpB3));_tmpB3->f1=tuf->name;_tmpB3->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_TupleType_struct
_tmpB5;_tmpB5.tag=9;_tmpB5.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple10*_tmpB6[1];_tmpB6[0]=({
struct _tuple10*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->f1=Cyc_Tcgenrep_tq_none;
_tmpB7->f2=Cyc_Absyn_uint_typ;_tmpB7;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpB6,sizeof(struct _tuple10*),1));}),
tuf->typs);_tmpB5;});_tmpB4;});_tmpB3;});}static struct _tuple16*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(
struct Cyc_Absyn_Tunionfield*tuf){return({struct _tuple16*_tmpB8=_cycalloc(sizeof(*
_tmpB8));_tmpB8->f1=(*tuf->name).f2;_tmpB8->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_TupleType_struct
_tmpBA;_tmpBA.tag=9;_tmpBA.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple10*_tmpBB[1];_tmpBB[0]=({
struct _tuple10*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->f1=Cyc_Tcgenrep_tq_none;
_tmpBC->f2=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Tcgenrep_tq_none,
Cyc_Absyn_true_conref);_tmpBC;});((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmpBB,sizeof(struct _tuple10*),1));}),tuf->typs);
_tmpBA;});_tmpB9;});_tmpB8;});}static int Cyc_Tcgenrep_filter_empty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs != 0;}static int Cyc_Tcgenrep_filter_nonempty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs == 0;}static struct
_dynforward_ptr*Cyc_Tcgenrep_get_tunionfield_name(struct Cyc_Absyn_Tunionfield*
tuf){return(*tuf->name).f2;}struct Cyc_Absyn_Aggrfield*Cyc_Tcgenrep_substitute_structfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Aggrfield*sf){return({struct Cyc_Absyn_Aggrfield*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->name=sf->name;_tmpBD->tq=sf->tq;_tmpBD->type=(
void*)Cyc_Tcutil_substitute(subst,(void*)sf->type);_tmpBD->width=sf->width;
_tmpBD->attributes=sf->attributes;_tmpBD;});}struct _tuple10*Cyc_Tcgenrep_substitute_tqual_type(
struct Cyc_List_List*subst,struct _tuple10*pair){struct _tuple10 _tmpBF;struct Cyc_Absyn_Tqual
_tmpC0;void*_tmpC1;struct _tuple10*_tmpBE=pair;_tmpBF=*_tmpBE;_tmpC0=_tmpBF.f1;
_tmpC1=_tmpBF.f2;return({struct _tuple10*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->f1=
_tmpC0;_tmpC2->f2=Cyc_Tcutil_substitute(subst,_tmpC1);_tmpC2;});}struct Cyc_Absyn_Tunionfield*
Cyc_Tcgenrep_substitute_tunionfield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*
tf){return({struct Cyc_Absyn_Tunionfield*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->name=
tf->name;_tmpC3->typs=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_List_List*,
struct _tuple10*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmpC3->loc=tf->loc;_tmpC3->sc=(
void*)((void*)tf->sc);_tmpC3;});}void*Cyc_Tcgenrep_monomorphize_type(void*type){
void*_tmpC4=Cyc_Tcutil_compress(type);struct Cyc_Absyn_AggrInfo _tmpC5;void*_tmpC6;
struct Cyc_List_List*_tmpC7;struct Cyc_Absyn_TunionInfo _tmpC8;void*_tmpC9;struct
Cyc_Absyn_Tuniondecl**_tmpCA;struct Cyc_Absyn_Tuniondecl*_tmpCB;struct Cyc_List_List*
_tmpCC;struct Cyc_Core_Opt*_tmpCD;_LL11: if(_tmpC4 <= (void*)4)goto _LL15;if(*((int*)
_tmpC4)!= 10)goto _LL13;_tmpC5=((struct Cyc_Absyn_AggrType_struct*)_tmpC4)->f1;
_tmpC6=(void*)_tmpC5.aggr_info;_tmpC7=_tmpC5.targs;_LL12: {struct Cyc_Absyn_Aggrdecl*
_tmpCE=Cyc_Absyn_get_known_aggrdecl(_tmpC6);struct Cyc_List_List*_tmpCF=_tmpCE->tvs;
if(Cyc_List_length(_tmpC7)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpCF))({struct Cyc_Int_pa_struct _tmpD3;_tmpD3.tag=1;_tmpD3.f1=(unsigned long)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCF);{struct Cyc_Int_pa_struct
_tmpD2;_tmpD2.tag=1;_tmpD2.f1=(unsigned long)Cyc_List_length(_tmpC7);{void*
_tmpD0[2]={& _tmpD2,& _tmpD3};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpD1="gen(): number of params %d differs from number of tyvars %d";
_tag_dynforward(_tmpD1,sizeof(char),_get_zero_arr_size(_tmpD1,60));}),
_tag_dynforward(_tmpD0,sizeof(void*),2));}}});{struct Cyc_List_List*_tmpD4=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpCF,_tmpC7);struct Cyc_List_List*fields=0;if(_tmpCE->impl != 0)fields=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_structfield_type,
_tmpD4,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE->impl))->fields);{
struct Cyc_Absyn_AggrdeclImpl*_tmpD5=_tmpCE->impl == 0?0:({struct Cyc_Absyn_AggrdeclImpl*
_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->exist_vars=0;_tmpDD->rgn_po=0;_tmpDD->fields=
fields;_tmpDD;});struct Cyc_Absyn_Aggrdecl*ad2=({struct Cyc_Absyn_Aggrdecl*_tmpDC=
_cycalloc(sizeof(*_tmpDC));_tmpDC->kind=(void*)((void*)_tmpCE->kind);_tmpDC->sc=(
void*)((void*)_tmpCE->sc);_tmpDC->name=_tmpCE->name;_tmpDC->tvs=0;_tmpDC->impl=
_tmpD5;_tmpDC->attributes=_tmpCE->attributes;_tmpDC;});return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_AggrType_struct
_tmpD7;_tmpD7.tag=10;_tmpD7.f1=({struct Cyc_Absyn_AggrInfo _tmpD8;_tmpD8.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));
_tmpD9[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpDA;_tmpDA.tag=1;_tmpDA.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=ad2;_tmpDB;});
_tmpDA;});_tmpD9;}));_tmpD8.targs=0;_tmpD8;});_tmpD7;});_tmpD6;});}}}_LL13: if(*((
int*)_tmpC4)!= 2)goto _LL15;_tmpC8=((struct Cyc_Absyn_TunionType_struct*)_tmpC4)->f1;
_tmpC9=(void*)_tmpC8.tunion_info;if(*((int*)_tmpC9)!= 1)goto _LL15;_tmpCA=((
struct Cyc_Absyn_KnownTunion_struct*)_tmpC9)->f1;_tmpCB=*_tmpCA;_tmpCC=_tmpC8.targs;
_tmpCD=_tmpC8.rgn;_LL14: {struct Cyc_List_List*_tmpDE=_tmpCB->tvs;if(Cyc_List_length(
_tmpCC)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpDE))({struct Cyc_Int_pa_struct
_tmpE2;_tmpE2.tag=1;_tmpE2.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpDE);{struct Cyc_Int_pa_struct _tmpE1;_tmpE1.tag=1;_tmpE1.f1=(unsigned long)Cyc_List_length(
_tmpCC);{void*_tmpDF[2]={& _tmpE1,& _tmpE2};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpE0="gen(): number of params %d differs from number of tyvars %d";
_tag_dynforward(_tmpE0,sizeof(char),_get_zero_arr_size(_tmpE0,60));}),
_tag_dynforward(_tmpDF,sizeof(void*),2));}}});{struct Cyc_List_List*_tmpE3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpDE,_tmpCC);struct Cyc_Core_Opt*fields=0;if(_tmpCB->fields != 0)fields=({struct
Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->v=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tunionfield_type,
_tmpE3,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpCB->fields))->v);
_tmpE4;});{struct Cyc_Absyn_Tuniondecl*_tmpE5=({struct Cyc_Absyn_Tuniondecl*_tmpEE=
_cycalloc(sizeof(*_tmpEE));_tmpEE->sc=(void*)((void*)_tmpCB->sc);_tmpEE->name=
_tmpCB->name;_tmpEE->tvs=0;_tmpEE->fields=fields;_tmpEE->is_xtunion=_tmpCB->is_xtunion;
_tmpEE->is_flat=_tmpCB->is_flat;_tmpEE;});struct Cyc_Absyn_TunionType_struct*
_tmpE6=({struct Cyc_Absyn_TunionType_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));
_tmpE8[0]=({struct Cyc_Absyn_TunionType_struct _tmpE9;_tmpE9.tag=2;_tmpE9.f1=({
struct Cyc_Absyn_TunionInfo _tmpEA;_tmpEA.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpEC;_tmpEC.tag=1;_tmpEC.f1=({struct Cyc_Absyn_Tuniondecl**_tmpED=_cycalloc(
sizeof(*_tmpED));_tmpED[0]=_tmpE5;_tmpED;});_tmpEC;});_tmpEB;}));_tmpEA.targs=
_tmpCC;_tmpEA.rgn=_tmpCD;_tmpEA;});_tmpE9;});_tmpE8;});struct Cyc_Position_Segment*
_tmpE7=Cyc_Position_segment_of_abs(0,0);return(void*)_tmpE6;}}}_LL15:;_LL16:
return type;_LL10:;}struct _dynforward_ptr Cyc_Tcgenrep_make_type_cstring(void*t){
struct _dynforward_ptr s=Cyc_strdup((struct _dynforward_ptr)Cyc_Absynpp_typ2cstring(
t));{int i=0;for(0;i < Cyc_strlen((struct _dynforward_ptr)s);i ++){if(*((char*)
_check_dynforward_subscript(s,sizeof(char),i))== ' ')({struct _dynforward_ptr
_tmpEF=_dynforward_ptr_plus(s,sizeof(char),i);char _tmpF0=*((char*)
_check_dynforward_subscript(_tmpEF,sizeof(char),0));char _tmpF1='_';if(
_get_dynforward_size(_tmpEF,sizeof(char))== 1  && (_tmpF0 == '\000'  && _tmpF1 != '\000'))
_throw_arraybounds();*((char*)_tmpEF.curr)=_tmpF1;});else{if(!isalnum((int)*((
char*)_check_dynforward_subscript(s,sizeof(char),i))) && *((char*)
_check_dynforward_subscript(s,sizeof(char),i))!= '_')({struct _dynforward_ptr
_tmpF2=_dynforward_ptr_plus(s,sizeof(char),i);char _tmpF3=*((char*)
_check_dynforward_subscript(_tmpF2,sizeof(char),0));char _tmpF4=(char)('0' + *((
char*)_check_dynforward_subscript(s,sizeof(char),i))% 10);if(
_get_dynforward_size(_tmpF2,sizeof(char))== 1  && (_tmpF3 == '\000'  && _tmpF4 != '\000'))
_throw_arraybounds();*((char*)_tmpF2.curr)=_tmpF4;});}}}return(struct
_dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)s,({const char*_tmpF5="_rep";
_tag_dynforward(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,5));}));}struct
_tuple19{struct Cyc_Dict_Dict*f1;struct Cyc_Tcgenrep_RepInfo*f2;};static struct
_tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,
struct Cyc_Position_Segment*loc,void*type);struct _tuple20{struct Cyc_Dict_Dict*f1;
struct Cyc_List_List*f2;};struct _tuple21{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Position_Segment*
f2;};static struct _tuple20*Cyc_Tcgenrep_lookupRepsCls(struct _tuple21*env,void*
type,struct _tuple20*carry){struct _tuple19 _tmpF7;struct Cyc_Dict_Dict*_tmpF8;
struct Cyc_Tcgenrep_RepInfo*_tmpF9;struct _tuple19*_tmpF6=Cyc_Tcgenrep_lookupRep((*
env).f1,(*carry).f1,(*env).f2,type);_tmpF7=*_tmpF6;_tmpF8=_tmpF7.f1;_tmpF9=
_tmpF7.f2;return({struct _tuple20*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->f1=
_tmpF8;_tmpFA->f2=({struct Cyc_List_List*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->hd=
_tmpF9;_tmpFB->tl=(*carry).f2;_tmpFB;});_tmpFA;});}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTuple(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,
struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*types){struct Cyc_Absyn_Exp*
_tmpFC=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple21*_tmpFD=({struct _tuple21*
_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->f1=te;_tmp112->f2=loc;_tmp112;});
struct _tuple20*_tmpFE=({struct _tuple20*_tmp111=_cycalloc(sizeof(*_tmp111));
_tmp111->f1=dict;_tmp111->f2=0;_tmp111;});struct _tuple20 _tmp100;struct Cyc_Dict_Dict*
_tmp101;struct Cyc_List_List*_tmp102;struct _tuple20*_tmpFF=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmpFD,types,_tmpFE);_tmp100=*_tmpFF;_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;dict=
_tmp101;{struct Cyc_List_List*_tmp103=((struct Cyc_List_List*(*)(int n,int(*f)(int)))
Cyc_List_tabulate)(Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);struct
Cyc_List_List*_tmp104=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct
_tuple14*,int),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple14*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->f1=type;_tmp110->f2=
loc;_tmp110;}),_tmp103);struct Cyc_List_List*_tmp105=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp102);struct Cyc_List_List*_tmp106=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp104,_tmp105));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp10F[2];
_tmp10F[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp10F[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dynforward(_tmp10F,sizeof(void*),
2));}));struct _tuple15 _tmp108;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_Exp*
_tmp10A;struct _tuple15*_tmp107=Cyc_Tcgenrep_array_decls(tuple_type,_tmp106,loc);
_tmp108=*_tmp107;_tmp109=_tmp108.f1;_tmp10A=_tmp108.f2;{struct Cyc_Absyn_Decl*
_tmp10B=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str),varname,({struct Cyc_Absyn_Exp*
_tmp10E[2];_tmp10E[1]=_tmp10A;_tmp10E[0]=_tmpFC;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp10E,sizeof(struct Cyc_Absyn_Exp*),
2));}),sc,loc);struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp109,({struct Cyc_Absyn_Decl*
_tmp10D[1];_tmp10D[0]=_tmp10B;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp10D,sizeof(struct Cyc_Absyn_Decl*),1));}));
return Cyc_Tcgenrep_update_info(dict,type,_tmp10C,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp102,0);}}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_buildRepTunionfield(struct _tuple6*tname,struct _tuple6*fname,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct
_tuple6*varname,void*sc,void*type,struct Cyc_List_List*types){struct Cyc_Absyn_Exp*
_tmp113=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple21*_tmp114=({struct
_tuple21*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->f1=te;_tmp12B->f2=loc;
_tmp12B;});struct _tuple20*_tmp115=({struct _tuple20*_tmp12A=_cycalloc(sizeof(*
_tmp12A));_tmp12A->f1=dict;_tmp12A->f2=0;_tmp12A;});struct _tuple20 _tmp117;struct
Cyc_Dict_Dict*_tmp118;struct Cyc_List_List*_tmp119;struct _tuple20*_tmp116=((
struct _tuple20*(*)(struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),
struct _tuple21*,struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls,_tmp114,types,_tmp115);_tmp117=*_tmp116;_tmp118=
_tmp117.f1;_tmp119=_tmp117.f2;dict=_tmp118;{struct Cyc_List_List*_tmp11A=((struct
Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(Cyc_List_length(types),(
int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple14*,int),struct _tuple14*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple14*_tmp129=
_cycalloc(sizeof(*_tmp129));_tmp129->f1=type;_tmp129->f2=loc;_tmp129;}),_tmp11A);
struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp119);struct Cyc_List_List*_tmp11D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp11B,_tmp11C));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp128[2];
_tmp128[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp128[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dynforward(_tmp128,sizeof(void*),
2));}));struct Cyc_Absyn_Exp*_tmp11E=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);
struct Cyc_Absyn_Exp*_tmp11F=Cyc_Tcgenrep_cnst_string(*(*fname).f2,loc);struct
_tuple15 _tmp121;struct Cyc_List_List*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct
_tuple15*_tmp120=Cyc_Tcgenrep_array_decls(tuple_type,_tmp11D,loc);_tmp121=*
_tmp120;_tmp122=_tmp121.f1;_tmp123=_tmp121.f2;{struct Cyc_Absyn_Decl*_tmp124=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunionfield_str),varname,({struct Cyc_Absyn_Exp*_tmp127[4];_tmp127[3]=
_tmp123;_tmp127[2]=_tmp113;_tmp127[1]=_tmp11F;_tmp127[0]=_tmp11E;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp127,sizeof(struct Cyc_Absyn_Exp*),
4));}),sc,loc);struct Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp122,({struct Cyc_Absyn_Decl*
_tmp126[1];_tmp126[0]=_tmp124;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp126,sizeof(struct Cyc_Absyn_Decl*),1));}));
return Cyc_Tcgenrep_update_info(dict,type,_tmp125,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp119,0);}}}static struct Cyc_Dict_Dict*
Cyc_Tcgenrep_buildRepStruct(struct Cyc_Core_Opt*sname,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,
void*sc,void*type,struct Cyc_List_List*nmtypes){struct Cyc_Absyn_Exp*_tmp12C=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple21*_tmp12D=({struct _tuple21*_tmp14A=_cycalloc(sizeof(*
_tmp14A));_tmp14A->f1=te;_tmp14A->f2=loc;_tmp14A;});struct _tuple20*_tmp12E=({
struct _tuple20*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->f1=dict;_tmp149->f2=
0;_tmp149;});struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp131;struct
_tuple0 _tmp12F=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nmtypes);
_tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;{struct _tuple20 _tmp133;struct Cyc_Dict_Dict*
_tmp134;struct Cyc_List_List*_tmp135;struct _tuple20*_tmp132=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp12D,_tmp131,_tmp12E);_tmp133=*_tmp132;_tmp134=_tmp133.f1;_tmp135=_tmp133.f2;
dict=_tmp134;{struct Cyc_List_List*_tmp136=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(Cyc_List_length(_tmp131),(int(*)(int))Cyc_Core_identity);
struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct _tuple14*,int),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple14*_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148->f1=type;_tmp148->f2=loc;_tmp148;}),_tmp136);struct Cyc_List_List*
_tmp138=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _dynforward_ptr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,_tmp130);struct Cyc_List_List*
_tmp139=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,_tmp135);struct Cyc_List_List*
_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple13*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple3_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp137,_tmp138,
_tmp139));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp147[3];_tmp147[2]=
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp147[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp147[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_dynforward(_tmp147,sizeof(void*),3));}));struct _tuple15
_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;struct _tuple15*
_tmp13B=Cyc_Tcgenrep_array_decls(tuple_type,_tmp13A,loc);_tmp13C=*_tmp13B;
_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;{struct Cyc_Absyn_Exp*name;if(sname == 0)
name=Cyc_Absyn_null_exp(loc);else{struct _dynforward_ptr*_tmp13F=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp142="name";_tag_dynforward(_tmp142,sizeof(char),_get_zero_arr_size(
_tmp142,5));}));struct Cyc_Absyn_Decl*_tmp140=Cyc_Tcgenrep_gen_decl(_tmp13F,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple6*)
sname->v)).f2,loc),loc);_tmp13D=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*
_tmp141));_tmp141->hd=_tmp140;_tmp141->tl=_tmp13D;_tmp141;});name=Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(_tmp13F),loc),loc);}{struct
Cyc_Absyn_Decl*_tmp143=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_struct_str),
varname,({struct Cyc_Absyn_Exp*_tmp146[3];_tmp146[2]=_tmp13E;_tmp146[1]=_tmp12C;
_tmp146[0]=name;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp146,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);struct Cyc_List_List*
_tmp144=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp13D,({struct Cyc_Absyn_Decl*_tmp145[1];_tmp145[0]=
_tmp143;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp145,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp144,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp135,0);}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct Cyc_Absyn_Exp*_tmp14B=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple21*_tmp14C=({struct _tuple21*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->f1=
te;_tmp166->f2=loc;_tmp166;});struct _tuple20*_tmp14D=({struct _tuple20*_tmp165=
_cycalloc(sizeof(*_tmp165));_tmp165->f1=dict;_tmp165->f2=0;_tmp165;});struct Cyc_List_List*
_tmp14F;struct Cyc_List_List*_tmp150;struct _tuple0 _tmp14E=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_tmp14F=_tmp14E.f1;_tmp150=
_tmp14E.f2;{struct _tuple20 _tmp152;struct Cyc_Dict_Dict*_tmp153;struct Cyc_List_List*
_tmp154;struct _tuple20*_tmp151=((struct _tuple20*(*)(struct _tuple20*(*f)(struct
_tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct
_tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp14C,
_tmp150,_tmp14D);_tmp152=*_tmp151;_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;dict=
_tmp153;{struct Cyc_List_List*_tmp155=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dynforward_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp14F);struct Cyc_List_List*_tmp156=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp154);struct Cyc_List_List*_tmp157=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp155,_tmp156));void*_tmp158=Cyc_Tcgenrep_tuple_typ(({void*_tmp164[2];_tmp164[
1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp164[0]=Cyc_Absyn_const_string_typ((void*)2);Cyc_List_list(_tag_dynforward(
_tmp164,sizeof(void*),2));}));struct _tuple15 _tmp15A;struct Cyc_List_List*_tmp15B;
struct Cyc_Absyn_Exp*_tmp15C;struct _tuple15*_tmp159=Cyc_Tcgenrep_array_decls(
_tmp158,_tmp157,loc);_tmp15A=*_tmp159;_tmp15B=_tmp15A.f1;_tmp15C=_tmp15A.f2;{
struct Cyc_Absyn_Exp*name;if(uname == 0)name=Cyc_Absyn_null_exp(loc);else{struct
_dynforward_ptr*_tmp15D=Cyc_Tcgenrep_new_gen_id(({const char*_tmp160="name";
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size(_tmp160,5));}));struct
Cyc_Absyn_Decl*_tmp15E=Cyc_Tcgenrep_gen_decl(_tmp15D,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple6*)
uname->v)).f2,loc),loc);_tmp15B=({struct Cyc_List_List*_tmp15F=_cycalloc(sizeof(*
_tmp15F));_tmp15F->hd=_tmp15E;_tmp15F->tl=_tmp15B;_tmp15F;});name=Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(_tmp15D),loc),loc);}{struct
Cyc_Absyn_Decl*_tmp161=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str),
varname,({struct Cyc_Absyn_Exp*_tmp163[3];_tmp163[2]=_tmp15C;_tmp163[1]=_tmp14B;
_tmp163[0]=name;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp163,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp15B,({struct Cyc_Absyn_Decl*_tmp162[1];_tmp162[0]=
_tmp161;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp162,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp154,0);}}}}}
static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepEnum(struct _tuple6**ename,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,struct
_tuple6*varname,void*sc,void*type,struct Cyc_List_List*tagnms){struct Cyc_Absyn_Exp*
_tmp167=Cyc_Absyn_sizeoftyp_exp(type,loc);struct Cyc_List_List*_tmp169;struct Cyc_List_List*
_tmp16A;struct _tuple0 _tmp168=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
tagnms);_tmp169=_tmp168.f1;_tmp16A=_tmp168.f2;{struct Cyc_List_List*_tmp16B=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple6*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_qvar_string_cls,loc,_tmp16A);struct Cyc_List_List*_tmp16C=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,
loc,_tmp169);struct Cyc_List_List*_tmp16D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp16C,_tmp16B));void*_tmp16E=Cyc_Tcgenrep_tuple_typ(({void*_tmp17A[2];_tmp17A[
1]=Cyc_Absyn_const_string_typ((void*)2);_tmp17A[0]=Cyc_Absyn_uint_typ;Cyc_List_list(
_tag_dynforward(_tmp17A,sizeof(void*),2));}));struct _tuple15 _tmp170;struct Cyc_List_List*
_tmp171;struct Cyc_Absyn_Exp*_tmp172;struct _tuple15*_tmp16F=Cyc_Tcgenrep_array_decls(
_tmp16E,_tmp16D,loc);_tmp170=*_tmp16F;_tmp171=_tmp170.f1;_tmp172=_tmp170.f2;{
struct Cyc_Absyn_Exp*name;if(ename == 0)name=Cyc_Absyn_null_exp(loc);else{struct
_dynforward_ptr*_tmp173=Cyc_Tcgenrep_new_gen_id(({const char*_tmp176="name";
_tag_dynforward(_tmp176,sizeof(char),_get_zero_arr_size(_tmp176,5));}));struct
Cyc_Absyn_Decl*_tmp174=Cyc_Tcgenrep_gen_decl(_tmp173,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*(*ename)).f2,loc),loc);
_tmp171=({struct Cyc_List_List*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->hd=
_tmp174;_tmp175->tl=_tmp171;_tmp175;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name(_tmp173),loc),loc);}{struct Cyc_Absyn_Decl*_tmp177=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str),varname,({struct Cyc_Absyn_Exp*_tmp179[3];_tmp179[2]=
_tmp172;_tmp179[1]=_tmp167;_tmp179[0]=name;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp179,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp171,({
struct Cyc_Absyn_Decl*_tmp178[1];_tmp178[0]=_tmp177;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp178,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),0,0);}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepTunion(
struct _tuple6*tname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tonames,struct
Cyc_List_List*nmtypes){struct Cyc_List_List*_tmp17B=((struct Cyc_List_List*(*)(int
n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tonames),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp17C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp17B);
struct Cyc_List_List*_tmp17D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
tonames);struct Cyc_List_List*_tmp17E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp17C,_tmp17D);struct Cyc_List_List*
_tmp17F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp17E);void*_tmp180=Cyc_Tcgenrep_tuple_typ(({
void*_tmp19F[2];_tmp19F[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp19F[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_dynforward(_tmp19F,sizeof(void*),2));}));struct _tuple15
_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct _tuple15*
_tmp181=Cyc_Tcgenrep_array_decls(_tmp180,_tmp17F,loc);_tmp182=*_tmp181;_tmp183=
_tmp182.f1;_tmp184=_tmp182.f2;{struct _tuple21*_tmp185=({struct _tuple21*_tmp19E=
_cycalloc(sizeof(*_tmp19E));_tmp19E->f1=te;_tmp19E->f2=loc;_tmp19E;});struct
_tuple20*_tmp186=({struct _tuple20*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->f1=
dict;_tmp19D->f2=0;_tmp19D;});struct Cyc_List_List*_tmp188;struct Cyc_List_List*
_tmp189;struct _tuple0 _tmp187=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_tmp188=_tmp187.f1;_tmp189=_tmp187.f2;{struct _tuple20 _tmp18B;struct Cyc_Dict_Dict*
_tmp18C;struct Cyc_List_List*_tmp18D;struct _tuple20*_tmp18A=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp185,_tmp189,_tmp186);_tmp18B=*_tmp18A;_tmp18C=_tmp18B.f1;_tmp18D=_tmp18B.f2;
dict=_tmp18C;{struct Cyc_List_List*_tmp18E=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp18D),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp18F=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp18E);
struct Cyc_List_List*_tmp190=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp188);struct Cyc_List_List*_tmp191=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp18D);struct Cyc_List_List*_tmp192=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp18F,_tmp190,
_tmp191);struct Cyc_List_List*_tmp193=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,_tmp192);
void*tuple_type2=Cyc_Tcgenrep_tuple_typ(({void*_tmp19C[3];_tmp19C[2]=Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));_tmp19C[1]=Cyc_Absyn_const_string_typ((
void*)2);_tmp19C[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dynforward(_tmp19C,
sizeof(void*),3));}));struct _tuple15 _tmp195;struct Cyc_List_List*_tmp196;struct
Cyc_Absyn_Exp*_tmp197;struct _tuple15*_tmp194=Cyc_Tcgenrep_array_decls(
tuple_type2,_tmp193,loc);_tmp195=*_tmp194;_tmp196=_tmp195.f1;_tmp197=_tmp195.f2;{
struct Cyc_Absyn_Exp*_tmp198=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);struct Cyc_Absyn_Decl*
_tmp199=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str),varname,({struct Cyc_Absyn_Exp*
_tmp19B[3];_tmp19B[2]=_tmp197;_tmp19B[1]=_tmp184;_tmp19B[0]=_tmp198;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp19B,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp183,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp196,({struct Cyc_Absyn_Decl*_tmp19A[1];_tmp19A[0]=_tmp199;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp19A,sizeof(struct Cyc_Absyn_Decl*),
1));}))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp18D,0);return dict;}}}}}static struct Cyc_Dict_Dict*Cyc_Tcgenrep_buildRepXTunion(
struct _tuple6*xname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*nametypes){
struct Cyc_List_List*_tmp1A1;struct Cyc_List_List*_tmp1A2;struct _tuple0 _tmp1A0=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nametypes);_tmp1A1=
_tmp1A0.f1;_tmp1A2=_tmp1A0.f2;{struct _tuple21*_tmp1A3=({struct _tuple21*_tmp1B8=
_cycalloc(sizeof(*_tmp1B8));_tmp1B8->f1=te;_tmp1B8->f2=loc;_tmp1B8;});struct
_tuple20*_tmp1A4=({struct _tuple20*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->f1=
dict;_tmp1B7->f2=0;_tmp1B7;});struct _tuple20 _tmp1A6;struct Cyc_Dict_Dict*_tmp1A7;
struct Cyc_List_List*_tmp1A8;struct _tuple20*_tmp1A5=((struct _tuple20*(*)(struct
_tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*
x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp1A3,_tmp1A2,_tmp1A4);_tmp1A6=*_tmp1A5;_tmp1A7=_tmp1A6.f1;_tmp1A8=_tmp1A6.f2;
dict=_tmp1A7;{struct Cyc_List_List*_tmp1A9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dynforward_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp1A1);struct Cyc_List_List*_tmp1AA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp1A8);struct Cyc_List_List*_tmp1AB=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1A9,_tmp1AA);struct Cyc_List_List*
_tmp1AC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp1AB);void*name_type=Cyc_Absyn_dynforward_typ(
Cyc_Absyn_char_typ,(void*)2,({struct Cyc_Absyn_Tqual _tmp1B6;_tmp1B6.print_const=1;
_tmp1B6.q_volatile=0;_tmp1B6.q_restrict=0;_tmp1B6.real_const=1;_tmp1B6.loc=0;
_tmp1B6;}),Cyc_Absyn_true_conref);void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*
_tmp1B5[2];_tmp1B5[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp1B5[0]=name_type;Cyc_List_list(_tag_dynforward(_tmp1B5,sizeof(void*),2));}));
struct _tuple15 _tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B0;
struct _tuple15*_tmp1AD=Cyc_Tcgenrep_array_decls(tuple_type,_tmp1AC,loc);_tmp1AE=*
_tmp1AD;_tmp1AF=_tmp1AE.f1;_tmp1B0=_tmp1AE.f2;{struct Cyc_Absyn_Exp*_tmp1B1=Cyc_Tcgenrep_cnst_string(*(*
xname).f2,loc);struct Cyc_Absyn_Decl*_tmp1B2=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str),varname,({struct Cyc_Absyn_Exp*_tmp1B4[2];_tmp1B4[1]=
_tmp1B0;_tmp1B4[0]=_tmp1B1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1B4,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp1AF,({struct Cyc_Absyn_Decl*_tmp1B3[1];_tmp1B3[0]=
_tmp1B2;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1B3,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp1A8,0);
return dict;}}}}static struct _tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Dict_Dict*dict,struct Cyc_Position_Segment*loc,void*type){struct Cyc_Core_Opt*
info=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(info != 0){if(((struct Cyc_Tcgenrep_RepInfo*)info->v)->is_extern)
dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,void*))Cyc_Dict_delete)(dict,
type);else{struct _tuple19*_tmp1B9=({struct _tuple19*_tmp1BA=_cycalloc(sizeof(*
_tmp1BA));_tmp1BA->f1=dict;_tmp1BA->f2=(struct Cyc_Tcgenrep_RepInfo*)info->v;
_tmp1BA;});return _tmp1B9;}}{void*_tmp1BB=Cyc_Tcutil_compress(Cyc_Tcgenrep_monomorphize_type(
type));void*_tmp1BC;void*_tmp1BD;int _tmp1BE;struct Cyc_Absyn_PtrInfo _tmp1BF;
struct Cyc_Absyn_ArrayInfo _tmp1C0;void*_tmp1C1;struct Cyc_Absyn_Tqual _tmp1C2;
struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Conref*_tmp1C4;struct Cyc_List_List*
_tmp1C5;struct _tuple6*_tmp1C6;struct Cyc_List_List*_tmp1C7;struct Cyc_Absyn_Typedefdecl*
_tmp1C8;void**_tmp1C9;struct Cyc_Absyn_Tvar*_tmp1CA;struct Cyc_Absyn_FnInfo _tmp1CB;
struct Cyc_Absyn_TunionInfo _tmp1CC;void*_tmp1CD;struct Cyc_Absyn_Tuniondecl**
_tmp1CE;struct Cyc_Absyn_Tuniondecl*_tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_Core_Opt*
_tmp1D1;struct Cyc_Absyn_TunionFieldInfo _tmp1D2;struct Cyc_Absyn_AggrInfo _tmp1D3;
void*_tmp1D4;struct Cyc_List_List*_tmp1D5;void*_tmp1D6;struct Cyc_List_List*
_tmp1D7;void*_tmp1D8;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Enumdecl*
_tmp1DA;struct Cyc_List_List*_tmp1DB;_LL18: if(_tmp1BB <= (void*)4)goto _LL1A;if(*((
int*)_tmp1BB)!= 5)goto _LL1A;_tmp1BC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1BB)->f1;_tmp1BD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1BB)->f2;_LL19: {
struct _tuple6*_tmp1DC=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp1E0="rep";_tag_dynforward(_tmp1E0,sizeof(char),_get_zero_arr_size(
_tmp1E0,4));})));struct Cyc_Absyn_Decl*_tmp1DD=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp1DC,({struct Cyc_Absyn_Exp*_tmp1DF[2];_tmp1DF[1]=Cyc_Tcgenrep_cnst_int(
Cyc_Tcgenrep_size_of2int(_tmp1BD),loc);_tmp1DF[0]=Cyc_Tcgenrep_cnst_int(_tmp1BC
== (void*)0?1: 0,loc);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1DF,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp1DE[1];_tmp1DE[0]=
_tmp1DD;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp1DE,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1DC,loc),loc),0,0);goto _LL17;}
_LL1A: if((int)_tmp1BB != 1)goto _LL1C;_LL1B: dict=Cyc_Tcgenrep_update_info(dict,
type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_float_str),loc),0,0);goto _LL17;_LL1C: if(_tmp1BB <= (void*)4)goto
_LL26;if(*((int*)_tmp1BB)!= 6)goto _LL1E;_tmp1BE=((struct Cyc_Absyn_DoubleType_struct*)
_tmp1BB)->f1;_LL1D: dict=Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_double_str),loc),
0,0);goto _LL17;_LL1E: if(*((int*)_tmp1BB)!= 4)goto _LL20;_tmp1BF=((struct Cyc_Absyn_PointerType_struct*)
_tmp1BB)->f1;_LL1F:{void*_tmp1E1=Cyc_Absyn_conref_val((_tmp1BF.ptr_atts).bounds);
struct Cyc_Absyn_Exp*_tmp1E2;_LL51: if(_tmp1E1 <= (void*)2)goto _LL53;if(*((int*)
_tmp1E1)!= 0)goto _LL53;_tmp1E2=((struct Cyc_Absyn_Upper_b_struct*)_tmp1E1)->f1;
_LL52: {struct _tuple6*_tmp1E3=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp1EE="rep";_tag_dynforward(_tmp1EE,sizeof(char),_get_zero_arr_size(
_tmp1EE,4));})));struct Cyc_Core_Opt*_tmp1E4=({struct Cyc_Core_Opt*_tmp1ED=
_cycalloc(sizeof(*_tmp1ED));_tmp1ED->v=Cyc_Tcgenrep_gen_vardecl(_tmp1E3,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str)),0,(void*)3,loc);_tmp1ED;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1E3,loc),loc),_tmp1E4,0);{struct _tuple19 _tmp1E6;struct Cyc_Dict_Dict*_tmp1E7;
struct Cyc_Tcgenrep_RepInfo*_tmp1E8;struct _tuple19*_tmp1E5=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)_tmp1BF.elt_typ);_tmp1E6=*_tmp1E5;_tmp1E7=_tmp1E6.f1;_tmp1E8=
_tmp1E6.f2;dict=_tmp1E7;{struct Cyc_Absyn_Decl*_tmp1E9=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp1E3,({struct Cyc_Absyn_Exp*_tmp1EC[2];_tmp1EC[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1E8->exp);_tmp1EC[0]=_tmp1E2;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1EC,sizeof(struct Cyc_Absyn_Exp*),
2));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp1EA[1];_tmp1EA[0]=_tmp1E9;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp1EA,sizeof(struct Cyc_Absyn_Decl*),1));}),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1E3,loc),
loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1EB[1];_tmp1EB[0]=_tmp1E8;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1EB,sizeof(struct Cyc_Tcgenrep_RepInfo*),
1));}),0);goto _LL50;}}}_LL53: if((int)_tmp1E1 != 0)goto _LL55;_LL54: goto _LL56;_LL55:
if((int)_tmp1E1 != 1)goto _LL57;_LL56: {struct _tuple6*_tmp1EF=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp1FA="rep";_tag_dynforward(_tmp1FA,
sizeof(char),_get_zero_arr_size(_tmp1FA,4));})));struct Cyc_Core_Opt*_tmp1F0=({
struct Cyc_Core_Opt*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1EF,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,(void*)3,loc);_tmp1F9;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1EF,loc),loc),_tmp1F0,0);{struct _tuple19 _tmp1F2;
struct Cyc_Dict_Dict*_tmp1F3;struct Cyc_Tcgenrep_RepInfo*_tmp1F4;struct _tuple19*
_tmp1F1=Cyc_Tcgenrep_lookupRep(te,dict,loc,(void*)_tmp1BF.elt_typ);_tmp1F2=*
_tmp1F1;_tmp1F3=_tmp1F2.f1;_tmp1F4=_tmp1F2.f2;dict=_tmp1F3;{struct Cyc_Absyn_Decl*
_tmp1F5=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str),_tmp1EF,({struct Cyc_Absyn_Exp*
_tmp1F8[1];_tmp1F8[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1F4->exp);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1F8,
sizeof(struct Cyc_Absyn_Exp*),1));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1F6[1];_tmp1F6[0]=_tmp1F5;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp1F6,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1EF,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1F7[1];_tmp1F7[0]=_tmp1F4;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp1F7,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL50;}}}_LL57: if(
_tmp1E1 <= (void*)2)goto _LL50;if(*((int*)_tmp1E1)!= 1)goto _LL50;_LL58:({void*
_tmp1FB[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1FC="tcgenregp: no support for abstract array bounds";
_tag_dynforward(_tmp1FC,sizeof(char),_get_zero_arr_size(_tmp1FC,48));}),
_tag_dynforward(_tmp1FB,sizeof(void*),0));});_LL50:;}goto _LL17;_LL20: if(*((int*)
_tmp1BB)!= 7)goto _LL22;_tmp1C0=((struct Cyc_Absyn_ArrayType_struct*)_tmp1BB)->f1;
_tmp1C1=(void*)_tmp1C0.elt_type;_tmp1C2=_tmp1C0.tq;_tmp1C3=_tmp1C0.num_elts;
_tmp1C4=_tmp1C0.zero_term;_LL21: if(_tmp1C3 == 0)({void*_tmp1FD[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp1FE="tcgenrep:At this point, array bounds must be constants";_tag_dynforward(
_tmp1FE,sizeof(char),_get_zero_arr_size(_tmp1FE,55));}),_tag_dynforward(_tmp1FD,
sizeof(void*),0));});{struct _tuple19 _tmp200;struct Cyc_Dict_Dict*_tmp201;struct
Cyc_Tcgenrep_RepInfo*_tmp202;struct _tuple19*_tmp1FF=Cyc_Tcgenrep_lookupRep(te,
dict,loc,_tmp1C1);_tmp200=*_tmp1FF;_tmp201=_tmp200.f1;_tmp202=_tmp200.f2;dict=
_tmp201;{struct _tuple6*_tmp203=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp208="rep";_tag_dynforward(_tmp208,sizeof(char),_get_zero_arr_size(
_tmp208,4));})));struct Cyc_Absyn_Decl*_tmp204=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp203,({struct Cyc_Absyn_Exp*_tmp207[2];_tmp207[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp202->exp);_tmp207[0]=(struct Cyc_Absyn_Exp*)
_tmp1C3;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp207,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp205[1];_tmp205[0]=
_tmp204;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp205,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp203,loc),loc),({struct Cyc_Tcgenrep_RepInfo*
_tmp206[1];_tmp206[0]=_tmp202;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp206,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),
0);goto _LL17;}}_LL22: if(*((int*)_tmp1BB)!= 9)goto _LL24;_tmp1C5=((struct Cyc_Absyn_TupleType_struct*)
_tmp1BB)->f1;_LL23: {struct _tuple6*_tmp209=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp20D="rep";_tag_dynforward(_tmp20D,sizeof(char),_get_zero_arr_size(
_tmp20D,4));})));struct Cyc_Core_Opt*_tmp20A=({struct Cyc_Core_Opt*_tmp20C=
_cycalloc(sizeof(*_tmp20C));_tmp20C->v=Cyc_Tcgenrep_gen_vardecl(_tmp209,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp20C;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp209,loc),loc),_tmp20A,0);{struct Cyc_List_List*_tmp20B=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,
_tmp1C5);dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp209,(void*)0,type,
_tmp20B);goto _LL17;}}_LL24: if(*((int*)_tmp1BB)!= 16)goto _LL26;_tmp1C6=((struct
Cyc_Absyn_TypedefType_struct*)_tmp1BB)->f1;_tmp1C7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1BB)->f2;_tmp1C8=((struct Cyc_Absyn_TypedefType_struct*)_tmp1BB)->f3;_tmp1C9=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1BB)->f4;_LL25: if(_tmp1C9 == 0)({void*
_tmp20E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp20F="gen(): can't gen for abstract typedef";_tag_dynforward(
_tmp20F,sizeof(char),_get_zero_arr_size(_tmp20F,38));}),_tag_dynforward(_tmp20E,
sizeof(void*),0));});{struct _tuple19 _tmp211;struct Cyc_Dict_Dict*_tmp212;struct
Cyc_Tcgenrep_RepInfo*_tmp213;struct _tuple19*_tmp210=Cyc_Tcgenrep_lookupRep(te,
dict,loc,*_tmp1C9);_tmp211=*_tmp210;_tmp212=_tmp211.f1;_tmp213=_tmp211.f2;dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp212,type,_tmp213);goto _LL17;}_LL26: if((int)_tmp1BB != 0)
goto _LL28;_LL27:({void*_tmp214[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp215="found void in gen() expression";
_tag_dynforward(_tmp215,sizeof(char),_get_zero_arr_size(_tmp215,31));}),
_tag_dynforward(_tmp214,sizeof(void*),0));});goto _LL17;_LL28: if(_tmp1BB <= (void*)
4)goto _LL30;if(*((int*)_tmp1BB)!= 0)goto _LL2A;_LL29:({void*_tmp216[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp217="found evar in gen() expression";_tag_dynforward(_tmp217,
sizeof(char),_get_zero_arr_size(_tmp217,31));}),_tag_dynforward(_tmp216,sizeof(
void*),0));});goto _LL17;_LL2A: if(*((int*)_tmp1BB)!= 1)goto _LL2C;_tmp1CA=((struct
Cyc_Absyn_VarType_struct*)_tmp1BB)->f1;_LL2B:({void*_tmp218[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp219="found tyvar in gen() expression";_tag_dynforward(
_tmp219,sizeof(char),_get_zero_arr_size(_tmp219,32));}),_tag_dynforward(_tmp218,
sizeof(void*),0));});goto _LL17;_LL2C: if(*((int*)_tmp1BB)!= 8)goto _LL2E;_tmp1CB=((
struct Cyc_Absyn_FnType_struct*)_tmp1BB)->f1;_LL2D:({void*_tmp21A[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp21B="found function type in gen() expression";
_tag_dynforward(_tmp21B,sizeof(char),_get_zero_arr_size(_tmp21B,40));}),
_tag_dynforward(_tmp21A,sizeof(void*),0));});goto _LL17;_LL2E: if(*((int*)_tmp1BB)
!= 15)goto _LL30;_LL2F: goto _LL31;_LL30: if((int)_tmp1BB != 3)goto _LL32;_LL31: goto
_LL33;_LL32: if((int)_tmp1BB != 2)goto _LL34;_LL33: goto _LL35;_LL34: if(_tmp1BB <= (
void*)4)goto _LL36;if(*((int*)_tmp1BB)!= 19)goto _LL36;_LL35: goto _LL37;_LL36: if(
_tmp1BB <= (void*)4)goto _LL38;if(*((int*)_tmp1BB)!= 20)goto _LL38;_LL37: goto _LL39;
_LL38: if(_tmp1BB <= (void*)4)goto _LL3A;if(*((int*)_tmp1BB)!= 21)goto _LL3A;_LL39:({
void*_tmp21C[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp21D="gen(): unhandled region, handle or effect type";
_tag_dynforward(_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,47));}),
_tag_dynforward(_tmp21C,sizeof(void*),0));});goto _LL17;_LL3A: if(_tmp1BB <= (void*)
4)goto _LL3C;if(*((int*)_tmp1BB)!= 2)goto _LL3C;_tmp1CC=((struct Cyc_Absyn_TunionType_struct*)
_tmp1BB)->f1;_tmp1CD=(void*)_tmp1CC.tunion_info;if(*((int*)_tmp1CD)!= 1)goto
_LL3C;_tmp1CE=((struct Cyc_Absyn_KnownTunion_struct*)_tmp1CD)->f1;_tmp1CF=*
_tmp1CE;_tmp1D0=_tmp1CC.targs;_tmp1D1=_tmp1CC.rgn;_LL3B: if(_tmp1CF->tvs != 0)({
void*_tmp21E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp21F="gen(): tunion type variables not handled yet";
_tag_dynforward(_tmp21F,sizeof(char),_get_zero_arr_size(_tmp21F,45));}),
_tag_dynforward(_tmp21E,sizeof(void*),0));});if(_tmp1CF->fields == 0){struct
_tuple6*_tmp220=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp223=
_cycalloc(sizeof(*_tmp223));_tmp223[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp223;}));struct Cyc_Core_Opt*_tmp221=({struct Cyc_Core_Opt*_tmp222=_cycalloc(
sizeof(*_tmp222));_tmp222->v=Cyc_Tcgenrep_gen_vardecl(_tmp220,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp222;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp220,loc),loc),_tmp221,1);goto _LL17;}if(!_tmp1CF->is_xtunion){struct _tuple6*
_tmp224=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp22C=_cycalloc(
sizeof(*_tmp22C));_tmp22C[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp22C;}));
struct Cyc_Core_Opt*_tmp225=({struct Cyc_Core_Opt*_tmp22B=_cycalloc(sizeof(*
_tmp22B));_tmp22B->v=Cyc_Tcgenrep_gen_vardecl(_tmp224,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc);_tmp22B;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp224,loc),loc),_tmp225,0);{struct Cyc_List_List*_tmp226=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CF->fields))->v;struct Cyc_List_List*_tmp227=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp226);struct Cyc_List_List*
_tmp228=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_nonempty_tunionfield,
_tmp226);struct Cyc_List_List*_tmp229=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_nmtype,
_tmp227);struct Cyc_List_List*_tmp22A=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_name,
_tmp228);dict=Cyc_Tcgenrep_buildRepTunion(_tmp1CF->name,te,dict,loc,_tmp224,(
void*)2,type,_tmp22A,_tmp229);}}else{struct _tuple6*_tmp22D=Cyc_Tcgenrep_toplevel_name(({
struct _dynforward_ptr*_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp233;}));struct Cyc_Core_Opt*_tmp22E=({struct Cyc_Core_Opt*_tmp232=
_cycalloc(sizeof(*_tmp232));_tmp232->v=Cyc_Tcgenrep_gen_vardecl(_tmp22D,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str)),0,(void*)3,loc);_tmp232;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp22D,loc),loc),_tmp22E,0);{struct Cyc_List_List*_tmp22F=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CF->fields))->v;struct Cyc_List_List*_tmp230=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp22F);struct Cyc_List_List*
_tmp231=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xtunionfield_and_get_name_type,
_tmp230);dict=Cyc_Tcgenrep_buildRepXTunion(_tmp1CF->name,te,dict,loc,_tmp22D,(
void*)2,type,_tmp231);}}goto _LL17;_LL3C: if(_tmp1BB <= (void*)4)goto _LL3E;if(*((
int*)_tmp1BB)!= 2)goto _LL3E;_LL3D:({void*_tmp234[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp235="gen(): tunion must be resolved by now";_tag_dynforward(_tmp235,sizeof(
char),_get_zero_arr_size(_tmp235,38));}),_tag_dynforward(_tmp234,sizeof(void*),0));});
_LL3E: if(_tmp1BB <= (void*)4)goto _LL40;if(*((int*)_tmp1BB)!= 3)goto _LL40;_tmp1D2=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp1BB)->f1;_LL3F: if(_tmp1D2.targs != 0)({
void*_tmp236[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp237="gen(): tunionfield type parameters not handled yet";
_tag_dynforward(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,51));}),
_tag_dynforward(_tmp236,sizeof(void*),0));});{void*_tmp238=(void*)_tmp1D2.field_info;
struct Cyc_Absyn_Tuniondecl*_tmp239;struct Cyc_Absyn_Tunionfield*_tmp23A;_LL5A: if(*((
int*)_tmp238)!= 1)goto _LL5C;_tmp239=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp238)->f1;_tmp23A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp238)->f2;
_LL5B: {struct Cyc_List_List*_tmp23B=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp23A->typs);
struct Cyc_List_List*_tmp23C=({struct Cyc_List_List*_tmp23F=_cycalloc(sizeof(*
_tmp23F));_tmp23F->hd=(void*)Cyc_Absyn_uint_typ;_tmp23F->tl=_tmp23B;_tmp23F;});
struct _tuple6*_tmp23D=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp23E="rep";_tag_dynforward(_tmp23E,sizeof(char),_get_zero_arr_size(
_tmp23E,4));})));dict=Cyc_Tcgenrep_buildRepTunionfield(_tmp239->name,_tmp23A->name,
te,dict,loc,_tmp23D,(void*)0,type,_tmp23C);goto _LL59;}_LL5C:;_LL5D:({void*
_tmp240[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp241="gen(): tunionfield must be known at this point";
_tag_dynforward(_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,47));}),
_tag_dynforward(_tmp240,sizeof(void*),0));});_LL59:;}goto _LL17;_LL40: if(_tmp1BB
<= (void*)4)goto _LL42;if(*((int*)_tmp1BB)!= 10)goto _LL42;_tmp1D3=((struct Cyc_Absyn_AggrType_struct*)
_tmp1BB)->f1;_tmp1D4=(void*)_tmp1D3.aggr_info;_tmp1D5=_tmp1D3.targs;_LL41: {
struct Cyc_Absyn_Aggrdecl*_tmp242=Cyc_Absyn_get_known_aggrdecl(_tmp1D4);if(
_tmp242->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp242->impl))->exist_vars
!= 0)({void*_tmp243[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp244="gen(): existential type variables not yet supported";
_tag_dynforward(_tmp244,sizeof(char),_get_zero_arr_size(_tmp244,52));}),
_tag_dynforward(_tmp243,sizeof(void*),0));});if((void*)_tmp242->kind == (void*)0){
struct Cyc_Absyn_Aggrdecl*_tmp245=_tmp242;if(_tmp245->impl == 0){struct _tuple6*
_tmp246=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp249=_cycalloc(
sizeof(*_tmp249));_tmp249[0]=Cyc_Tcgenrep_make_type_cstring(type);_tmp249;}));
struct Cyc_Core_Opt*_tmp247=({struct Cyc_Core_Opt*_tmp248=_cycalloc(sizeof(*
_tmp248));_tmp248->v=Cyc_Tcgenrep_gen_vardecl(_tmp246,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp248;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp246,loc),loc),_tmp247,1);goto _LL17;}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp245->impl))->fields != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp245->impl))->fields)){int _tmp24A=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),
int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp245->impl))->fields);int
_tmp24B=_tmp24A / 8 + (_tmp24A % 8 == 0?0: 1);struct Cyc_List_List*chars=0;{int i=0;
for(0;i < _tmp24B;i ++){chars=({struct Cyc_List_List*_tmp24C=_cycalloc(sizeof(*
_tmp24C));_tmp24C->hd=({struct _tuple10*_tmp24D=_cycalloc(sizeof(*_tmp24D));
_tmp24D->f1=Cyc_Tcgenrep_tq_none;_tmp24D->f2=Cyc_Absyn_char_typ;_tmp24D;});
_tmp24C->tl=chars;_tmp24C;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Absyn_TupleType_struct
_tmp253;_tmp253.tag=9;_tmp253.f1=chars;_tmp253;});_tmp252;});struct _tuple19
_tmp24F;struct Cyc_Dict_Dict*_tmp250;struct Cyc_Tcgenrep_RepInfo*_tmp251;struct
_tuple19*_tmp24E=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp24F=*_tmp24E;
_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp250,type,_tmp251);}}
else{struct _tuple6*_tmp254=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*
_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp259;}));struct Cyc_Core_Opt*_tmp255=({struct Cyc_Core_Opt*_tmp258=
_cycalloc(sizeof(*_tmp258));_tmp258->v=Cyc_Tcgenrep_gen_vardecl(_tmp254,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp258;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp254,loc),loc),_tmp255,0);{struct Cyc_List_List*_tmp256=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp245->impl))->fields);dict=Cyc_Tcgenrep_buildRepStruct(({struct
Cyc_Core_Opt*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->v=_tmp245->name;
_tmp257;}),te,dict,loc,_tmp254,(void*)2,type,_tmp256);}}}else{struct Cyc_Absyn_Aggrdecl*
_tmp25A=_tmp242;if(_tmp25A->tvs != 0)({void*_tmp25B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp25C="gen: unions with parameters not yet supported";_tag_dynforward(_tmp25C,
sizeof(char),_get_zero_arr_size(_tmp25C,46));}),_tag_dynforward(_tmp25B,sizeof(
void*),0));});if(_tmp25A->impl == 0){struct _tuple6*_tmp25D=Cyc_Tcgenrep_toplevel_name(({
struct _dynforward_ptr*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp260;}));struct Cyc_Core_Opt*_tmp25E=({struct Cyc_Core_Opt*_tmp25F=
_cycalloc(sizeof(*_tmp25F));_tmp25F->v=Cyc_Tcgenrep_gen_vardecl(_tmp25D,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,(void*)3,loc);_tmp25F;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp25D,loc),loc),_tmp25E,1);goto _LL17;}{struct Cyc_List_List*_tmp261=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp25A->impl))->fields);struct _tuple6*_tmp262=Cyc_Tcgenrep_toplevel_name(({
struct _dynforward_ptr*_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp264;}));dict=Cyc_Tcgenrep_buildRepUnion(({struct Cyc_Core_Opt*_tmp263=
_cycalloc(sizeof(*_tmp263));_tmp263->v=_tmp25A->name;_tmp263;}),te,dict,loc,
_tmp262,(void*)2,type,_tmp261);}}goto _LL17;}_LL42: if(_tmp1BB <= (void*)4)goto
_LL44;if(*((int*)_tmp1BB)!= 11)goto _LL44;_tmp1D6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BB)->f1;if((int)_tmp1D6 != 0)goto _LL44;_tmp1D7=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BB)->f2;_LL43: if(_tmp1D7 != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp1D7)){int
_tmp265=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp1D7);int _tmp266=
_tmp265 / 8 + (_tmp265 % 8 == 0?0: 1);void*base_type=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Tcgenrep_tq_none,(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_int(_tmp266,loc),
Cyc_Absyn_true_conref,0);struct _tuple19 _tmp268;struct Cyc_Dict_Dict*_tmp269;
struct Cyc_Tcgenrep_RepInfo*_tmp26A;struct _tuple19*_tmp267=Cyc_Tcgenrep_lookupRep(
te,dict,loc,base_type);_tmp268=*_tmp267;_tmp269=_tmp268.f1;_tmp26A=_tmp268.f2;
dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp269,type,_tmp26A);}else{struct Cyc_List_List*_tmp26B=((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,_tmp1D7);
struct _tuple6*_tmp26C=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp26D="rep";_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size(
_tmp26D,4));})));dict=Cyc_Tcgenrep_buildRepStruct(0,te,dict,loc,_tmp26C,(void*)0,
type,_tmp26B);}goto _LL17;_LL44: if(_tmp1BB <= (void*)4)goto _LL46;if(*((int*)
_tmp1BB)!= 11)goto _LL46;_tmp1D8=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BB)->f1;if((int)_tmp1D8 != 1)goto _LL46;_tmp1D9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BB)->f2;_LL45: {struct Cyc_List_List*_tmp26E=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,_tmp1D9);struct _tuple6*_tmp26F=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp270="rep";_tag_dynforward(_tmp270,
sizeof(char),_get_zero_arr_size(_tmp270,4));})));dict=Cyc_Tcgenrep_buildRepUnion(
0,te,dict,loc,_tmp26F,(void*)0,type,_tmp26E);goto _LL17;}_LL46: if(_tmp1BB <= (void*)
4)goto _LL48;if(*((int*)_tmp1BB)!= 12)goto _LL48;_tmp1DA=((struct Cyc_Absyn_EnumType_struct*)
_tmp1BB)->f2;_LL47: if(_tmp1DA == 0)({void*_tmp271[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp272="gen(): enum declaration must be present by now";_tag_dynforward(_tmp272,
sizeof(char),_get_zero_arr_size(_tmp272,47));}),_tag_dynforward(_tmp271,sizeof(
void*),0));});{struct Cyc_Absyn_Enumdecl _tmp273=*_tmp1DA;if(_tmp273.fields == 0){
struct _tuple6*_tmp274=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp277=
_cycalloc(sizeof(*_tmp277));_tmp277[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp277;}));struct Cyc_Core_Opt*_tmp275=({struct Cyc_Core_Opt*_tmp276=_cycalloc(
sizeof(*_tmp276));_tmp276->v=Cyc_Tcgenrep_gen_vardecl(_tmp274,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str)),0,(void*)3,loc);_tmp276;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp274,loc),loc),_tmp275,1);goto _LL17;}{struct Cyc_List_List*_tmp278=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,(struct Cyc_List_List*)(_tmp273.fields)->v);
struct _tuple6*_tmp279=Cyc_Tcgenrep_toplevel_name(({struct _dynforward_ptr*_tmp27A=
_cycalloc(sizeof(*_tmp27A));_tmp27A[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp27A;}));dict=Cyc_Tcgenrep_buildRepEnum((struct _tuple6**)& _tmp273.name,te,
dict,loc,_tmp279,(void*)2,type,_tmp278);goto _LL17;}}_LL48: if(_tmp1BB <= (void*)4)
goto _LL4A;if(*((int*)_tmp1BB)!= 13)goto _LL4A;_tmp1DB=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1BB)->f1;_LL49: {struct Cyc_List_List*_tmp27B=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,_tmp1DB);struct _tuple6*_tmp27C=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp27D="rep";_tag_dynforward(_tmp27D,
sizeof(char),_get_zero_arr_size(_tmp27D,4));})));dict=Cyc_Tcgenrep_buildRepEnum(
0,te,dict,loc,_tmp27C,(void*)0,type,_tmp27B);goto _LL17;}_LL4A: if(_tmp1BB <= (void*)
4)goto _LL4C;if(*((int*)_tmp1BB)!= 14)goto _LL4C;_LL4B:({void*_tmp27E[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp27F="gen() for sizeof_t<-> not yet supported";_tag_dynforward(_tmp27F,
sizeof(char),_get_zero_arr_size(_tmp27F,40));}),_tag_dynforward(_tmp27E,sizeof(
void*),0));});_LL4C: if(_tmp1BB <= (void*)4)goto _LL4E;if(*((int*)_tmp1BB)!= 18)
goto _LL4E;_LL4D:({void*_tmp280[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp281="gen() for `i not yet supported";
_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size(_tmp281,31));}),
_tag_dynforward(_tmp280,sizeof(void*),0));});_LL4E: if(_tmp1BB <= (void*)4)goto
_LL17;if(*((int*)_tmp1BB)!= 17)goto _LL17;_LL4F:({void*_tmp282[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp283="gen() for tag_t<-> not yet supported";_tag_dynforward(_tmp283,sizeof(
char),_get_zero_arr_size(_tmp283,37));}),_tag_dynforward(_tmp282,sizeof(void*),0));});
_LL17:;}return({struct _tuple19*_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->f1=
dict;_tmp284->f2=((struct Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict*d,void*k))
Cyc_Dict_lookup)(dict,type);_tmp284;});}static int Cyc_Tcgenrep_not_emitted_filter(
struct Cyc_Tcgenrep_RepInfo*ri){return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(
struct Cyc_Tcgenrep_RepInfo*ri){ri->emitted=1;}struct _tuple22{struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict*dict){Cyc_Tcutil_check_valid_toplevel_type(loc,te,
type,0);Cyc_Tcutil_check_valid_toplevel_type(loc,te,type,0);{struct _tuple19
_tmp286;struct Cyc_Dict_Dict*_tmp287;struct Cyc_Tcgenrep_RepInfo*_tmp288;struct
_tuple19*_tmp285=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp286=*_tmp285;
_tmp287=_tmp286.f1;_tmp288=_tmp286.f2;{struct Cyc_List_List*_tmp289=Cyc_Tcgenrep_dfs(
_tmp288);return({struct _tuple22 _tmp28A;_tmp28A.f1=_tmp287;_tmp28A.f2=_tmp289;
_tmp28A.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp288->exp);_tmp28A;});}}}
