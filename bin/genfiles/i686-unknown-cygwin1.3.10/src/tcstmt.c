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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct
Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple0{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
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
_tuple0*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct
_tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
_tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;
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
struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_exn_typ;
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
_dynforward_ptr Cyc_Absyn_attribute2string(void*);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple3{void*f1;void*f2;};struct _tuple3*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
le;};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);void*Cyc_Tcenv_return_typ(
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
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dynforward_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple4{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_Tcenv_CList*
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
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable);void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple5{struct
Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);int*Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr msg_part);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
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
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,
int new_block);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
s->try_depth=Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult
pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
struct _tuple6*_tmp1;struct Cyc_List_List*_tmp2;struct Cyc_Tcpat_TcPatResult _tmp0=
pat_res;_tmp1=_tmp0.tvars_and_bounds_opt;_tmp2=_tmp0.patvars;{struct Cyc_List_List*
_tmp3=_tmp1 == 0?0:(*_tmp1).f1;struct Cyc_List_List*_tmp4=_tmp1 == 0?0:(*_tmp1).f2;{
struct _RegionHandle _tmp5=_new_region("r");struct _RegionHandle*r=& _tmp5;
_push_region(r);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmp3);
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmp4);if(new_block)te2=Cyc_Tcenv_new_block(
r,loc,te2);{struct Cyc_Core_Opt*_tmp6=({struct Cyc_Core_Opt*_tmp9=_cycalloc(
sizeof(*_tmp9));_tmp9->v=(void*)Cyc_Tcenv_curr_rgn(te2);_tmp9;});{struct Cyc_List_List*
_tmp7=_tmp2;for(0;_tmp7 != 0;_tmp7=_tmp7->tl){te2=Cyc_Tcenv_add_pat_var(r,loc,te2,(
struct Cyc_Absyn_Vardecl*)_tmp7->hd);((struct Cyc_Absyn_Vardecl*)_tmp7->hd)->rgn=
_tmp6;}}if(where_opt != 0)Cyc_Tcexp_tcTest(te2,(struct Cyc_Absyn_Exp*)where_opt,({
const char*_tmp8="switch clause guard";_tag_dynforward(_tmp8,sizeof(char),
_get_zero_arr_size(_tmp8,20));}));if(_tmp2 != 0)te2=Cyc_Tcenv_set_encloser(r,te2,
s);Cyc_Tcstmt_tcStmt(te2,s,0);}};_pop_region(r);}if(_tmp2 != 0)Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));}}static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(
struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Tcenv_CList*x){
if(x == 0)return 0;else{return({struct Cyc_List_List*_tmpA=_region_malloc(r,sizeof(*
_tmpA));_tmpA->hd=(void*)f(env,(void*)x->hd);_tmpA->tl=Cyc_Tcstmt_cmap_c(r,f,env,
x->tl);_tmpA;});}}void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s0,int new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*_tmpB=(void*)s0->r;struct
Cyc_Absyn_Exp*_tmpC;struct Cyc_Absyn_Stmt*_tmpD;struct Cyc_Absyn_Stmt*_tmpE;struct
Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Stmt*_tmp11;
struct Cyc_Absyn_Stmt*_tmp12;struct _tuple2 _tmp13;struct Cyc_Absyn_Exp*_tmp14;
struct Cyc_Absyn_Stmt*_tmp15;struct Cyc_Absyn_Stmt*_tmp16;struct Cyc_Absyn_Exp*
_tmp17;struct _tuple2 _tmp18;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Stmt*
_tmp1A;struct _tuple2 _tmp1B;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Stmt*
_tmp1D;struct Cyc_Absyn_Stmt*_tmp1E;struct Cyc_Absyn_Stmt*_tmp1F;struct _tuple2
_tmp20;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Stmt*_tmp22;struct Cyc_Absyn_Stmt*
_tmp23;struct Cyc_Absyn_Stmt**_tmp24;struct Cyc_Absyn_Stmt*_tmp25;struct Cyc_Absyn_Stmt**
_tmp26;struct _dynforward_ptr*_tmp27;struct Cyc_Absyn_Stmt*_tmp28;struct Cyc_Absyn_Stmt**
_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Switch_clause**_tmp2B;struct
Cyc_Absyn_Switch_clause***_tmp2C;struct _dynforward_ptr*_tmp2D;struct Cyc_Absyn_Stmt*
_tmp2E;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_List_List*_tmp30;struct Cyc_Absyn_Stmt*
_tmp31;struct Cyc_List_List*_tmp32;struct Cyc_Absyn_Decl*_tmp33;struct Cyc_Absyn_Stmt*
_tmp34;struct Cyc_Absyn_Tvar*_tmp35;struct Cyc_Absyn_Vardecl*_tmp36;int _tmp37;
struct Cyc_Absyn_Stmt*_tmp38;struct Cyc_Absyn_Exp*_tmp39;_LL1: if((int)_tmpB != 0)
goto _LL3;_LL2: return;_LL3: if(_tmpB <= (void*)1)goto _LL5;if(*((int*)_tmpB)!= 0)
goto _LL5;_tmpC=((struct Cyc_Absyn_Exp_s_struct*)_tmpB)->f1;_LL4: Cyc_Tcexp_tcExp(
te,0,_tmpC);return;_LL5: if(_tmpB <= (void*)1)goto _LL7;if(*((int*)_tmpB)!= 1)goto
_LL7;_tmpD=((struct Cyc_Absyn_Seq_s_struct*)_tmpB)->f1;_tmpE=((struct Cyc_Absyn_Seq_s_struct*)
_tmpB)->f2;_LL6:{struct _RegionHandle _tmp3A=_new_region("r");struct _RegionHandle*
r=& _tmp3A;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp3B=Cyc_Tcenv_set_next(r,te,(
void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({
struct Cyc_Tcenv_Stmt_j_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=_tmpE;_tmp3D;});
_tmp3C;}));Cyc_Tcstmt_tcStmt(_tmp3B,_tmpD,1);};_pop_region(r);}Cyc_Tcstmt_tcStmt(
te,_tmpE,1);return;_LL7: if(_tmpB <= (void*)1)goto _LL9;if(*((int*)_tmpB)!= 2)goto
_LL9;_tmpF=((struct Cyc_Absyn_Return_s_struct*)_tmpB)->f1;_LL8: {void*t=Cyc_Tcenv_return_typ(
te);if(_tmpF == 0){void*_tmp3E=Cyc_Tcutil_compress(t);_LL26: if((int)_tmp3E != 0)
goto _LL28;_LL27: goto _LL25;_LL28:;_LL29:({struct Cyc_String_pa_struct _tmp41;_tmp41.tag=
0;_tmp41.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp3F[1]={& _tmp41};Cyc_Tcutil_terr(s0->loc,({const char*_tmp40="must return a value of type %s";
_tag_dynforward(_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,31));}),
_tag_dynforward(_tmp3F,sizeof(void*),1));}});goto _LL25;_LL25:;}else{struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmpF;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_String_pa_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp44;_tmp44.tag=0;_tmp44.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp42[2]={& _tmp44,&
_tmp45};Cyc_Tcutil_terr(s0->loc,({const char*_tmp43="returns value of type %s but requires %s";
_tag_dynforward(_tmp43,sizeof(char),_get_zero_arr_size(_tmp43,41));}),
_tag_dynforward(_tmp42,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
return;}_LL9: if(_tmpB <= (void*)1)goto _LLB;if(*((int*)_tmpB)!= 3)goto _LLB;_tmp10=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmpB)->f1;_tmp11=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpB)->f2;_tmp12=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpB)->f3;_LLA: Cyc_Tcexp_tcTest(
te,_tmp10,({const char*_tmp46="if statement";_tag_dynforward(_tmp46,sizeof(char),
_get_zero_arr_size(_tmp46,13));}));Cyc_Tcstmt_tcStmt(te,_tmp11,1);Cyc_Tcstmt_tcStmt(
te,_tmp12,1);return;_LLB: if(_tmpB <= (void*)1)goto _LLD;if(*((int*)_tmpB)!= 4)goto
_LLD;_tmp13=((struct Cyc_Absyn_While_s_struct*)_tmpB)->f1;_tmp14=_tmp13.f1;_tmp15=
_tmp13.f2;_tmp16=((struct Cyc_Absyn_While_s_struct*)_tmpB)->f2;_LLC: Cyc_Tcstmt_decorate_stmt(
te,_tmp15);Cyc_Tcexp_tcTest(te,_tmp14,({const char*_tmp47="while loop";
_tag_dynforward(_tmp47,sizeof(char),_get_zero_arr_size(_tmp47,11));}));{struct
_RegionHandle _tmp48=_new_region("r");struct _RegionHandle*r=& _tmp48;_push_region(
r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp15),_tmp16,1);;_pop_region(r);}
return;_LLD: if(_tmpB <= (void*)1)goto _LLF;if(*((int*)_tmpB)!= 8)goto _LLF;_tmp17=((
struct Cyc_Absyn_For_s_struct*)_tmpB)->f1;_tmp18=((struct Cyc_Absyn_For_s_struct*)
_tmpB)->f2;_tmp19=_tmp18.f1;_tmp1A=_tmp18.f2;_tmp1B=((struct Cyc_Absyn_For_s_struct*)
_tmpB)->f3;_tmp1C=_tmp1B.f1;_tmp1D=_tmp1B.f2;_tmp1E=((struct Cyc_Absyn_For_s_struct*)
_tmpB)->f4;_LLE: Cyc_Tcstmt_decorate_stmt(te,_tmp1A);Cyc_Tcstmt_decorate_stmt(te,
_tmp1D);Cyc_Tcexp_tcExp(te,0,_tmp17);Cyc_Tcexp_tcTest(te,_tmp19,({const char*
_tmp49="for loop";_tag_dynforward(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,
9));}));{struct _RegionHandle _tmp4A=_new_region("r");struct _RegionHandle*r=&
_tmp4A;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp4B=Cyc_Tcenv_set_in_loop(r,te,
_tmp1D);Cyc_Tcstmt_tcStmt(_tmp4B,_tmp1E,1);Cyc_Tcexp_tcExp(_tmp4B,0,_tmp1C);};
_pop_region(r);}return;_LLF: if(_tmpB <= (void*)1)goto _LL11;if(*((int*)_tmpB)!= 13)
goto _LL11;_tmp1F=((struct Cyc_Absyn_Do_s_struct*)_tmpB)->f1;_tmp20=((struct Cyc_Absyn_Do_s_struct*)
_tmpB)->f2;_tmp21=_tmp20.f1;_tmp22=_tmp20.f2;_LL10: Cyc_Tcstmt_decorate_stmt(te,
_tmp22);{struct _RegionHandle _tmp4C=_new_region("r");struct _RegionHandle*r=&
_tmp4C;_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp22),
_tmp1F,1);;_pop_region(r);}Cyc_Tcexp_tcTest(te,_tmp21,({const char*_tmp4D="do loop";
_tag_dynforward(_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,8));}));return;
_LL11: if(_tmpB <= (void*)1)goto _LL13;if(*((int*)_tmpB)!= 5)goto _LL13;_tmp23=((
struct Cyc_Absyn_Break_s_struct*)_tmpB)->f1;_tmp24=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Break_s_struct*)_tmpB)->f1;_LL12: Cyc_Tcenv_process_break(te,s0,
_tmp24);return;_LL13: if(_tmpB <= (void*)1)goto _LL15;if(*((int*)_tmpB)!= 6)goto
_LL15;_tmp25=((struct Cyc_Absyn_Continue_s_struct*)_tmpB)->f1;_tmp26=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Continue_s_struct*)_tmpB)->f1;_LL14: Cyc_Tcenv_process_continue(
te,s0,_tmp26);return;_LL15: if(_tmpB <= (void*)1)goto _LL17;if(*((int*)_tmpB)!= 7)
goto _LL17;_tmp27=((struct Cyc_Absyn_Goto_s_struct*)_tmpB)->f1;_tmp28=((struct Cyc_Absyn_Goto_s_struct*)
_tmpB)->f2;_tmp29=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)
_tmpB)->f2;_LL16: Cyc_Tcenv_process_goto(te,s0,_tmp27,_tmp29);return;_LL17: if(
_tmpB <= (void*)1)goto _LL19;if(*((int*)_tmpB)!= 10)goto _LL19;_tmp2A=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmpB)->f1;_tmp2B=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpB)->f2;_tmp2C=(struct
Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_struct*)_tmpB)->f2;
_LL18: {struct _tuple4*_tmp4E=Cyc_Tcenv_process_fallthru(te,s0,_tmp2C);if(_tmp4E
== 0){({void*_tmp4F[0]={};Cyc_Tcutil_terr(s0->loc,({const char*_tmp50="fallthru not in a non-last case";
_tag_dynforward(_tmp50,sizeof(char),_get_zero_arr_size(_tmp50,32));}),
_tag_dynforward(_tmp4F,sizeof(void*),0));});return;}{struct Cyc_List_List*_tmp51=(*
_tmp4E).f2;struct Cyc_Tcenv_CList*_tmp52=(*_tmp4E).f3;struct Cyc_List_List*
instantiation=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp51);struct Cyc_List_List*
_tmp53=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,
void*),struct Cyc_List_List*env,struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,
Cyc_Tcutil_substitute,instantiation,_tmp52);for(0;_tmp53 != 0  && _tmp2A != 0;(
_tmp53=_tmp53->tl,_tmp2A=_tmp2A->tl)){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp2A->hd);if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp2A->hd,(void*)
_tmp53->hd)){({struct Cyc_String_pa_struct _tmp57;_tmp57.tag=0;_tmp57.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)_tmp53->hd));{
struct Cyc_String_pa_struct _tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)_tmp2A->hd)->topt))->v));{void*_tmp54[2]={&
_tmp56,& _tmp57};Cyc_Tcutil_terr(s0->loc,({const char*_tmp55="fallthru argument has type %s but pattern variable has type %s";
_tag_dynforward(_tmp55,sizeof(char),_get_zero_arr_size(_tmp55,63));}),
_tag_dynforward(_tmp54,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}if(
_tmp2A != 0)({void*_tmp58[0]={};Cyc_Tcutil_terr(s0->loc,({const char*_tmp59="too many arguments to explicit fallthru";
_tag_dynforward(_tmp59,sizeof(char),_get_zero_arr_size(_tmp59,40));}),
_tag_dynforward(_tmp58,sizeof(void*),0));});if(_tmp53 != 0)({void*_tmp5A[0]={};
Cyc_Tcutil_terr(s0->loc,({const char*_tmp5B="too few arguments to explicit fallthru";
_tag_dynforward(_tmp5B,sizeof(char),_get_zero_arr_size(_tmp5B,39));}),
_tag_dynforward(_tmp5A,sizeof(void*),0));});return;}}_LL19: if(_tmpB <= (void*)1)
goto _LL1B;if(*((int*)_tmpB)!= 12)goto _LL1B;_tmp2D=((struct Cyc_Absyn_Label_s_struct*)
_tmpB)->f1;_tmp2E=((struct Cyc_Absyn_Label_s_struct*)_tmpB)->f2;_LL1A:{struct
_RegionHandle _tmp5C=_new_region("r");struct _RegionHandle*r=& _tmp5C;_push_region(
r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(r,s0->loc,Cyc_Tcenv_add_label(te,
_tmp2D,_tmp2E),({struct Cyc_Absyn_Tvar*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->name=({
struct _dynforward_ptr*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp63;_tmp63.tag=0;_tmp63.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp2D);{void*_tmp61[1]={& _tmp63};
Cyc_aprintf(({const char*_tmp62="`%s";_tag_dynforward(_tmp62,sizeof(char),
_get_zero_arr_size(_tmp62,4));}),_tag_dynforward(_tmp61,sizeof(void*),1));}});
_tmp60;});_tmp5D->identity=Cyc_Tcutil_new_tvar_id();_tmp5D->kind=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp5F;_tmp5F.tag=0;_tmp5F.f1=(void*)((void*)3);
_tmp5F;});_tmp5E;}));_tmp5D;})),_tmp2E,0);;_pop_region(r);}return;_LL1B: if(_tmpB
<= (void*)1)goto _LL1D;if(*((int*)_tmpB)!= 9)goto _LL1D;_tmp2F=((struct Cyc_Absyn_Switch_s_struct*)
_tmpB)->f1;_tmp30=((struct Cyc_Absyn_Switch_s_struct*)_tmpB)->f2;_LL1C: Cyc_Tcexp_tcExp(
te,0,_tmp2F);{void*_tmp64=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2F->topt))->v;{
struct _RegionHandle _tmp65=_new_region("r");struct _RegionHandle*r=& _tmp65;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp66=Cyc_Tcenv_set_in_switch(r,te);
_tmp66=Cyc_Tcenv_clear_fallthru(r,_tmp66);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp30);for(0;scs != 0;scs=scs->tl){struct
Cyc_Absyn_Pat*_tmp67=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct
Cyc_Tcpat_TcPatResult _tmp68=Cyc_Tcpat_tcPat(_tmp66,_tmp67,(void**)& _tmp64);
struct Cyc_List_List*_tmp69=_tmp68.tvars_and_bounds_opt == 0?0:(*_tmp68.tvars_and_bounds_opt).f1;
if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp67->topt))->v,
_tmp64)){({struct Cyc_String_pa_struct _tmp6D;_tmp6D.tag=0;_tmp6D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp67->topt))->v));{struct Cyc_String_pa_struct _tmp6C;
_tmp6C.tag=0;_tmp6C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp64));{void*_tmp6A[2]={& _tmp6C,& _tmp6D};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,({const char*_tmp6B="switch on type %s, but case expects type %s";
_tag_dynforward(_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,44));}),
_tag_dynforward(_tmp6A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(_tmp66,_tmp67);}((struct Cyc_Absyn_Switch_clause*)
scs->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->v=
_tmp68.patvars;_tmp6E;});Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,_tmp66,_tmp68,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((
struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(_tmp68.tvars_and_bounds_opt
!= 0  && (*_tmp68.tvars_and_bounds_opt).f2 != 0)_tmp66=Cyc_Tcenv_clear_fallthru(r,
_tmp66);else{_tmp66=Cyc_Tcenv_set_fallthru(r,_tmp66,_tmp69,_tmp68.patvars,(
struct Cyc_Absyn_Switch_clause*)scs->hd);}}}};_pop_region(r);}Cyc_Tcpat_check_switch_exhaustive(
s0->loc,_tmp30);return;}_LL1D: if(_tmpB <= (void*)1)goto _LL1F;if(*((int*)_tmpB)!= 
14)goto _LL1F;_tmp31=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpB)->f1;_tmp32=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmpB)->f2;_LL1E:(void*)(_tmp31->r=(void*)((
void*)(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Seq_s_struct*_tmp6F=_cycalloc(
sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Absyn_Seq_s_struct _tmp70;_tmp70.tag=1;
_tmp70.f1=Cyc_Absyn_new_stmt((void*)_tmp31->r,_tmp31->loc);_tmp70.f2=Cyc_Absyn_skip_stmt(
_tmp31->loc);_tmp70;});_tmp6F;}),_tmp31->loc))->r));{struct _RegionHandle _tmp71=
_new_region("r");struct _RegionHandle*r=& _tmp71;_push_region(r);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_enter_try(r,te),s0),_tmp31,1);;_pop_region(r);}{
struct _RegionHandle _tmp72=_new_region("r2");struct _RegionHandle*r2=& _tmp72;
_push_region(r2);{struct Cyc_Tcenv_Tenv*_tmp73=Cyc_Tcenv_set_in_switch(r2,te);
_tmp73=Cyc_Tcenv_clear_fallthru(r2,_tmp73);{struct Cyc_List_List*_tmp74=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp32);for(0;_tmp74 != 0;
_tmp74=_tmp74->tl){struct Cyc_Absyn_Pat*_tmp75=((struct Cyc_Absyn_Switch_clause*)
_tmp74->hd)->pattern;struct Cyc_Tcpat_TcPatResult _tmp76=Cyc_Tcpat_tcPat(_tmp73,
_tmp75,(void**)& Cyc_Absyn_exn_typ);struct Cyc_List_List*_tmp77=_tmp76.tvars_and_bounds_opt
== 0?0:(*_tmp76.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp75->topt))->v,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct
_tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp75->topt))->v));{void*_tmp78[1]={&
_tmp7A};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->loc,({
const char*_tmp79="expected xtunion exn but found %s";_tag_dynforward(_tmp79,
sizeof(char),_get_zero_arr_size(_tmp79,34));}),_tag_dynforward(_tmp78,sizeof(
void*),1));}});else{Cyc_Tcpat_check_pat_regions(_tmp73,_tmp75);}((struct Cyc_Absyn_Switch_clause*)
_tmp74->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp7B=_cycalloc(sizeof(*_tmp7B));
_tmp7B->v=_tmp76.patvars;_tmp7B;});Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)
_tmp74->hd)->loc,_tmp73,_tmp76,((struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->body,((
struct Cyc_Absyn_Switch_clause*)_tmp74->hd)->where_clause,1);if(_tmp76.tvars_and_bounds_opt
!= 0  && (*_tmp76.tvars_and_bounds_opt).f2 != 0)_tmp73=Cyc_Tcenv_clear_fallthru(r2,
_tmp73);else{_tmp73=Cyc_Tcenv_set_fallthru(r2,_tmp73,_tmp77,_tmp76.patvars,(
struct Cyc_Absyn_Switch_clause*)_tmp74->hd);}}}};_pop_region(r2);}Cyc_Tcpat_check_catch_overlap(
s0->loc,_tmp32);return;_LL1F: if(_tmpB <= (void*)1)goto _LL21;if(*((int*)_tmpB)!= 
11)goto _LL21;_tmp33=((struct Cyc_Absyn_Decl_s_struct*)_tmpB)->f1;_tmp34=((struct
Cyc_Absyn_Decl_s_struct*)_tmpB)->f2;_LL20: {struct _dynforward_ptr unimp_msg_part;{
struct _RegionHandle _tmp7C=_new_region("decl_rgn");struct _RegionHandle*decl_rgn=&
_tmp7C;_push_region(decl_rgn);{struct Cyc_Tcenv_Tenv*te2;if(new_block)te2=Cyc_Tcenv_new_block(
decl_rgn,s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(decl_rgn,te);}{struct Cyc_Tcenv_Tenv*
_tmp7D=te2;void*_tmp7E=(void*)_tmp33->r;struct Cyc_Absyn_Vardecl*_tmp7F;struct Cyc_Absyn_Pat*
_tmp80;struct Cyc_Core_Opt*_tmp81;struct Cyc_Core_Opt**_tmp82;struct Cyc_Absyn_Exp*
_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_Absyn_Fndecl*_tmp85;struct
_dynforward_ptr*_tmp86;struct Cyc_List_List*_tmp87;struct _tuple0*_tmp88;struct Cyc_List_List*
_tmp89;_LL2B: if(_tmp7E <= (void*)2)goto _LL43;if(*((int*)_tmp7E)!= 0)goto _LL2D;
_tmp7F=((struct Cyc_Absyn_Var_d_struct*)_tmp7E)->f1;_LL2C: {struct Cyc_Absyn_Vardecl
_tmp8B;void*_tmp8C;struct _tuple0*_tmp8D;struct _tuple0 _tmp8E;union Cyc_Absyn_Nmspace_union
_tmp8F;struct _dynforward_ptr*_tmp90;struct Cyc_Absyn_Tqual _tmp91;void*_tmp92;
struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Core_Opt*_tmp94;struct Cyc_Core_Opt**_tmp95;
struct Cyc_List_List*_tmp96;struct Cyc_Absyn_Vardecl*_tmp8A=_tmp7F;_tmp8B=*_tmp8A;
_tmp8C=(void*)_tmp8B.sc;_tmp8D=_tmp8B.name;_tmp8E=*_tmp8D;_tmp8F=_tmp8E.f1;
_tmp90=_tmp8E.f2;_tmp91=_tmp8B.tq;_tmp92=(void*)_tmp8B.type;_tmp93=_tmp8B.initializer;
_tmp94=_tmp8B.rgn;_tmp95=(struct Cyc_Core_Opt**)&(*_tmp8A).rgn;_tmp96=_tmp8B.attributes;{
void*_tmp97=Cyc_Tcenv_curr_rgn(_tmp7D);int is_local;{void*_tmp98=_tmp8C;_LL48: if((
int)_tmp98 != 0)goto _LL4A;_LL49: goto _LL4B;_LL4A: if((int)_tmp98 != 3)goto _LL4C;
_LL4B: goto _LL4D;_LL4C: if((int)_tmp98 != 4)goto _LL4E;_LL4D: is_local=0;goto _LL47;
_LL4E: if((int)_tmp98 != 1)goto _LL50;_LL4F:({void*_tmp99[0]={};Cyc_Tcutil_terr(
_tmp33->loc,({const char*_tmp9A="bad abstract scope for local variable";
_tag_dynforward(_tmp9A,sizeof(char),_get_zero_arr_size(_tmp9A,38));}),
_tag_dynforward(_tmp99,sizeof(void*),0));});goto _LL51;_LL50: if((int)_tmp98 != 5)
goto _LL52;_LL51: goto _LL53;_LL52: if((int)_tmp98 != 2)goto _LL47;_LL53: is_local=1;
goto _LL47;_LL47:;}*_tmp95=is_local?({struct Cyc_Core_Opt*_tmp9B=_cycalloc(sizeof(*
_tmp9B));_tmp9B->v=(void*)_tmp97;_tmp9B;}):({struct Cyc_Core_Opt*_tmp9C=_cycalloc(
sizeof(*_tmp9C));_tmp9C->v=(void*)((void*)2);_tmp9C;});{union Cyc_Absyn_Nmspace_union
_tmp9D=_tmp8F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9F;_LL55: if((
_tmp9D.Loc_n).tag != 0)goto _LL57;_LL56: goto _LL54;_LL57: if((_tmp9D.Rel_n).tag != 1)
goto _LL59;_tmp9E=(_tmp9D.Rel_n).f1;if(_tmp9E != 0)goto _LL59;_LL58:(*_tmp7F->name).f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmpA0;(_tmpA0.Loc_n).tag=
0;_tmpA0;});goto _LL54;_LL59: if((_tmp9D.Abs_n).tag != 2)goto _LL5B;_tmp9F=(_tmp9D.Abs_n).f1;
_LL5A:(int)_throw(({void*_tmpA1[0]={};Cyc_Tcutil_impos(({const char*_tmpA2="tcstmt: Abs_n declaration";
_tag_dynforward(_tmpA2,sizeof(char),_get_zero_arr_size(_tmpA2,26));}),
_tag_dynforward(_tmpA1,sizeof(void*),0));}));_LL5B:;_LL5C:({void*_tmpA3[0]={};
Cyc_Tcutil_terr(_tmp33->loc,({const char*_tmpA4="cannot declare a qualified local variable";
_tag_dynforward(_tmpA4,sizeof(char),_get_zero_arr_size(_tmpA4,42));}),
_tag_dynforward(_tmpA3,sizeof(void*),0));});goto _LL54;_LL54:;}{void*_tmpA5=Cyc_Tcutil_compress(
_tmp92);struct Cyc_Absyn_ArrayInfo _tmpA6;void*_tmpA7;struct Cyc_Absyn_Tqual _tmpA8;
struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Conref*_tmpAA;struct Cyc_Position_Segment*
_tmpAB;_LL5E: if(_tmpA5 <= (void*)4)goto _LL60;if(*((int*)_tmpA5)!= 7)goto _LL60;
_tmpA6=((struct Cyc_Absyn_ArrayType_struct*)_tmpA5)->f1;_tmpA7=(void*)_tmpA6.elt_type;
_tmpA8=_tmpA6.tq;_tmpA9=_tmpA6.num_elts;if(_tmpA9 != 0)goto _LL60;_tmpAA=_tmpA6.zero_term;
_tmpAB=_tmpA6.zt_loc;if(!(_tmp7F->initializer != 0))goto _LL60;_LL5F:{void*_tmpAC=(
void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp7F->initializer))->r;union Cyc_Absyn_Cnst_union
_tmpAD;struct _dynforward_ptr _tmpAE;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_List_List*
_tmpB0;struct Cyc_List_List*_tmpB1;_LL63: if(*((int*)_tmpAC)!= 0)goto _LL65;_tmpAD=((
struct Cyc_Absyn_Const_e_struct*)_tmpAC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpAC)->f1).String_c).tag != 5)goto _LL65;_tmpAE=(_tmpAD.String_c).f1;_LL64:
_tmp92=(void*)(_tmp7F->type=(void*)Cyc_Absyn_array_typ(_tmpA7,_tmpA8,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(_get_dynforward_size(_tmpAE,sizeof(char)),0),_tmpAA,_tmpAB));
goto _LL62;_LL65: if(*((int*)_tmpAC)!= 29)goto _LL67;_tmpAF=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpAC)->f2;_LL66: _tmp92=(void*)(_tmp7F->type=(void*)Cyc_Absyn_array_typ(_tmpA7,
_tmpA8,(struct Cyc_Absyn_Exp*)_tmpAF,_tmpAA,_tmpAB));goto _LL62;_LL67: if(*((int*)
_tmpAC)!= 36)goto _LL69;_tmpB0=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpAC)->f2;
_LL68: _tmpB1=_tmpB0;goto _LL6A;_LL69: if(*((int*)_tmpAC)!= 28)goto _LL6B;_tmpB1=((
struct Cyc_Absyn_Array_e_struct*)_tmpAC)->f1;_LL6A: _tmp92=(void*)(_tmp7F->type=(
void*)Cyc_Absyn_array_typ(_tmpA7,_tmpA8,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB1),0),_tmpAA,
_tmpAB));goto _LL62;_LL6B:;_LL6C: goto _LL62;_LL62:;}goto _LL5D;_LL60:;_LL61: goto
_LL5D;_LL5D:;}{struct Cyc_List_List*_tmpB2=!is_local?0: Cyc_Tcenv_lookup_type_vars(
_tmp7D);int _tmpB3=!is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,_tmp7D,_tmpB2,(
void*)1,_tmpB3,_tmp92);{struct Cyc_Tcenv_Tenv*_tmpB4=Cyc_Tcenv_add_local_var(
decl_rgn,_tmp34->loc,_tmp7D,_tmp7F);if(_tmp8C == (void*)3  || _tmp8C == (void*)4)({
void*_tmpB5[0]={};Cyc_Tcutil_terr(_tmp33->loc,({const char*_tmpB6="local extern declarations not yet supported";
_tag_dynforward(_tmpB6,sizeof(char),_get_zero_arr_size(_tmpB6,44));}),
_tag_dynforward(_tmpB5,sizeof(void*),0));});if(_tmp93 != 0){Cyc_Tcexp_tcExpInitializer(
_tmpB4,(void**)& _tmp92,(struct Cyc_Absyn_Exp*)_tmp93);if(!is_local  && !Cyc_Tcutil_is_const_exp(
_tmp7D,(struct Cyc_Absyn_Exp*)_tmp93))({void*_tmpB7[0]={};Cyc_Tcutil_terr(_tmp33->loc,({
const char*_tmpB8="initializer needs to be a constant expression";_tag_dynforward(
_tmpB8,sizeof(char),_get_zero_arr_size(_tmpB8,46));}),_tag_dynforward(_tmpB7,
sizeof(void*),0));});if(!Cyc_Tcutil_coerce_assign(_tmpB4,(struct Cyc_Absyn_Exp*)
_tmp93,_tmp92)){struct _dynforward_ptr _tmpB9=*_tmp90;struct _dynforward_ptr _tmpBA=
Cyc_Absynpp_typ2string(_tmp92);struct _dynforward_ptr _tmpBB=Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp93->topt))->v);if(((
_get_dynforward_size(_tmpB9,sizeof(char))+ _get_dynforward_size(_tmpBA,sizeof(
char)))+ _get_dynforward_size(_tmpBB,sizeof(char)))+ 50 < 80)({struct Cyc_String_pa_struct
_tmpC0;_tmpC0.tag=0;_tmpC0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpBB);{struct Cyc_String_pa_struct _tmpBF;_tmpBF.tag=0;_tmpBF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpBA);{struct Cyc_String_pa_struct
_tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpB9);{void*_tmpBC[3]={& _tmpBE,& _tmpBF,& _tmpC0};Cyc_Tcutil_terr(_tmp33->loc,({
const char*_tmpBD="%s declared with type %s but initialized with type %s.";
_tag_dynforward(_tmpBD,sizeof(char),_get_zero_arr_size(_tmpBD,55));}),
_tag_dynforward(_tmpBC,sizeof(void*),3));}}}});else{if((_get_dynforward_size(
_tmpB9,sizeof(char))+ _get_dynforward_size(_tmpBA,sizeof(char)))+ 25 < 80  && 
_get_dynforward_size(_tmpBB,sizeof(char))+ 25 < 80)({struct Cyc_String_pa_struct
_tmpC5;_tmpC5.tag=0;_tmpC5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpBB);{struct Cyc_String_pa_struct _tmpC4;_tmpC4.tag=0;_tmpC4.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpBA);{struct Cyc_String_pa_struct
_tmpC3;_tmpC3.tag=0;_tmpC3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpB9);{void*_tmpC1[3]={& _tmpC3,& _tmpC4,& _tmpC5};Cyc_Tcutil_terr(_tmp33->loc,({
const char*_tmpC2="%s declared with type %s\n but initialized with type %s.";
_tag_dynforward(_tmpC2,sizeof(char),_get_zero_arr_size(_tmpC2,56));}),
_tag_dynforward(_tmpC1,sizeof(void*),3));}}}});else{({struct Cyc_String_pa_struct
_tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpBB);{struct Cyc_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpBA);{struct Cyc_String_pa_struct
_tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmpB9);{void*_tmpC6[3]={& _tmpC8,& _tmpC9,& _tmpCA};Cyc_Tcutil_terr(_tmp33->loc,({
const char*_tmpC7="%s declared with type \n%s\n but initialized with type \n%s.";
_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size(_tmpC7,58));}),
_tag_dynforward(_tmpC6,sizeof(void*),3));}}}});}}Cyc_Tcutil_unify(_tmp92,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp93->topt))->v);Cyc_Tcutil_explain_failure();}}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpB4,s0),_tmp34,0);
_npop_handler(0);return;}}}}_LL2D: if(*((int*)_tmp7E)!= 2)goto _LL2F;_tmp80=((
struct Cyc_Absyn_Let_d_struct*)_tmp7E)->f1;_tmp81=((struct Cyc_Absyn_Let_d_struct*)
_tmp7E)->f2;_tmp82=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_struct*)_tmp7E)->f2;
_tmp83=((struct Cyc_Absyn_Let_d_struct*)_tmp7E)->f3;_LL2E: Cyc_Tcexp_tcExpInitializer(
_tmp7D,0,_tmp83);{struct Cyc_Tcpat_TcPatResult _tmpCB=Cyc_Tcpat_tcPat(_tmp7D,
_tmp80,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(_tmp83->topt))->v)));*
_tmp82=({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->v=_tmpCB.patvars;
_tmpCC;});if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp83->topt))->v) && !Cyc_Tcutil_coerce_assign(
_tmp7D,_tmp83,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v)){({
struct Cyc_String_pa_struct _tmpD0;_tmpD0.tag=0;_tmpD0.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp83->topt))->v));{struct Cyc_String_pa_struct _tmpCF;_tmpCF.tag=0;
_tmpCF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v));{void*_tmpCD[2]={&
_tmpCF,& _tmpD0};Cyc_Tcutil_terr(_tmp33->loc,({const char*_tmpCE="pattern type %s does not match definition type %s";
_tag_dynforward(_tmpCE,sizeof(char),_get_zero_arr_size(_tmpCE,50));}),
_tag_dynforward(_tmpCD,sizeof(void*),2));}}});Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp80->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp83->topt))->v);Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(
_tmp7D,_tmp80);}Cyc_Tcpat_check_let_pat_exhaustive(_tmp80->loc,_tmp80);Cyc_Tcstmt_pattern_synth(
s0->loc,_tmp7D,_tmpCB,_tmp34,0,0);_npop_handler(0);return;}_LL2F: if(*((int*)
_tmp7E)!= 3)goto _LL31;_tmp84=((struct Cyc_Absyn_Letv_d_struct*)_tmp7E)->f1;_LL30: {
void*_tmpD1=Cyc_Tcenv_curr_rgn(_tmp7D);struct Cyc_Tcenv_Tenv*_tmpD2=_tmp7D;for(0;
_tmp84 != 0;_tmp84=_tmp84->tl){struct Cyc_Absyn_Vardecl*_tmpD3=(struct Cyc_Absyn_Vardecl*)
_tmp84->hd;struct Cyc_Absyn_Vardecl _tmpD5;struct _tuple0*_tmpD6;struct _tuple0
_tmpD7;union Cyc_Absyn_Nmspace_union _tmpD8;void*_tmpD9;struct Cyc_Core_Opt*_tmpDA;
struct Cyc_Core_Opt**_tmpDB;struct Cyc_Absyn_Vardecl*_tmpD4=_tmpD3;_tmpD5=*_tmpD4;
_tmpD6=_tmpD5.name;_tmpD7=*_tmpD6;_tmpD8=_tmpD7.f1;_tmpD9=(void*)_tmpD5.type;
_tmpDA=_tmpD5.rgn;_tmpDB=(struct Cyc_Core_Opt**)&(*_tmpD4).rgn;*_tmpDB=({struct
Cyc_Core_Opt*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->v=(void*)_tmpD1;_tmpDC;});{
union Cyc_Absyn_Nmspace_union _tmpDD=_tmpD8;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*
_tmpDF;_LL6E: if((_tmpDD.Loc_n).tag != 0)goto _LL70;_LL6F: goto _LL6D;_LL70: if((
_tmpDD.Rel_n).tag != 1)goto _LL72;_tmpDE=(_tmpDD.Rel_n).f1;if(_tmpDE != 0)goto _LL72;
_LL71:(*_tmpD3->name).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmpE0;(_tmpE0.Loc_n).tag=0;_tmpE0;});goto _LL6D;_LL72: if((_tmpDD.Abs_n).tag != 2)
goto _LL74;_tmpDF=(_tmpDD.Abs_n).f1;_LL73:(int)_throw(({void*_tmpE1[0]={};Cyc_Tcutil_impos(({
const char*_tmpE2="tcstmt: Abs_n declaration";_tag_dynforward(_tmpE2,sizeof(char),
_get_zero_arr_size(_tmpE2,26));}),_tag_dynforward(_tmpE1,sizeof(void*),0));}));
_LL74:;_LL75:({void*_tmpE3[0]={};Cyc_Tcutil_terr(_tmp33->loc,({const char*_tmpE4="cannot declare a qualified local variable";
_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size(_tmpE4,42));}),
_tag_dynforward(_tmpE3,sizeof(void*),0));});goto _LL6D;_LL6D:;}Cyc_Tcutil_check_type(
s0->loc,_tmpD2,Cyc_Tcenv_lookup_type_vars(_tmpD2),(void*)1,1,_tmpD9);_tmpD2=Cyc_Tcenv_add_local_var(
decl_rgn,_tmp34->loc,_tmpD2,_tmpD3);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
decl_rgn,_tmpD2,s0),_tmp34,0);_npop_handler(0);return;}_LL31: if(*((int*)_tmp7E)
!= 1)goto _LL33;_tmp85=((struct Cyc_Absyn_Fn_d_struct*)_tmp7E)->f1;_LL32: {void*
_tmpE5=Cyc_Tcenv_curr_rgn(_tmp7D);if((void*)_tmp85->sc != (void*)2)({void*_tmpE6[
0]={};Cyc_Tcutil_terr(_tmp33->loc,({const char*_tmpE7="bad storage class for inner function";
_tag_dynforward(_tmpE7,sizeof(char),_get_zero_arr_size(_tmpE7,37));}),
_tag_dynforward(_tmpE6,sizeof(void*),0));});{union Cyc_Absyn_Nmspace_union _tmpE8=(*
_tmp85->name).f1;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*_tmpEA;_LL77: if((
_tmpE8.Rel_n).tag != 1)goto _LL79;_tmpE9=(_tmpE8.Rel_n).f1;if(_tmpE9 != 0)goto _LL79;
_LL78: goto _LL76;_LL79: if((_tmpE8.Abs_n).tag != 2)goto _LL7B;_tmpEA=(_tmpE8.Abs_n).f1;
_LL7A:({void*_tmpEB[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpEC="tc: Abs_n in tcStmt var decl";
_tag_dynforward(_tmpEC,sizeof(char),_get_zero_arr_size(_tmpEC,29));}),
_tag_dynforward(_tmpEB,sizeof(void*),0));});_LL7B:;_LL7C:({void*_tmpED[0]={};Cyc_Tcutil_terr(
_tmp33->loc,({const char*_tmpEE="explicit namespace not allowed on inner function declaration";
_tag_dynforward(_tmpEE,sizeof(char),_get_zero_arr_size(_tmpEE,61));}),
_tag_dynforward(_tmpED,sizeof(void*),0));});_LL76:;}(*_tmp85->name).f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmpEF;(_tmpEF.Loc_n).tag=0;_tmpEF;});Cyc_Tcutil_check_fndecl_valid_type(
_tmp33->loc,_tmp7D,_tmp85);{void*t=Cyc_Tcutil_fndecl2typ(_tmp85);_tmp85->attributes=
Cyc_Tcutil_transfer_fn_type_atts(t,_tmp85->attributes);{struct Cyc_List_List*atts=
_tmp85->attributes;for(0;(unsigned int)atts;atts=atts->tl){void*_tmpF0=(void*)
atts->hd;_LL7E: if((int)_tmpF0 != 5)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmpF0 <= (
void*)17)goto _LL82;if(*((int*)_tmpF0)!= 1)goto _LL82;_LL81:({struct Cyc_String_pa_struct
_tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)atts->hd));{void*_tmpF1[1]={& _tmpF3};Cyc_Tcutil_terr(_tmp33->loc,({const
char*_tmpF2="bad attribute %s for function";_tag_dynforward(_tmpF2,sizeof(char),
_get_zero_arr_size(_tmpF2,30));}),_tag_dynforward(_tmpF1,sizeof(void*),1));}});
goto _LL7D;_LL82:;_LL83: goto _LL7D;_LL7D:;}}{struct Cyc_Absyn_Vardecl*vd=({struct
Cyc_Absyn_Vardecl*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->sc=(void*)((void*)
_tmp85->sc);_tmpF6->name=_tmp85->name;_tmpF6->tq=Cyc_Absyn_const_tqual(0);_tmpF6->type=(
void*)Cyc_Absyn_at_typ(t,_tmpE5,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmpF6->initializer=0;_tmpF6->rgn=({struct Cyc_Core_Opt*_tmpF7=_cycalloc(sizeof(*
_tmpF7));_tmpF7->v=(void*)_tmpE5;_tmpF7;});_tmpF6->attributes=0;_tmpF6->escapes=
0;_tmpF6;});_tmp85->fn_vardecl=(struct Cyc_Absyn_Vardecl*)vd;_tmp7D=Cyc_Tcenv_add_local_var(
decl_rgn,_tmp33->loc,_tmp7D,vd);{struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)
_check_null(_tmp7D->le);_tmp7D->le=(struct Cyc_Tcenv_Fenv*)Cyc_Tcenv_nested_fenv(
decl_rgn,_tmp33->loc,old_fenv,_tmp85);Cyc_Tcstmt_tcStmt(_tmp7D,_tmp85->body,0);
Cyc_Tcenv_check_delayed_effects(_tmp7D);Cyc_Tcenv_check_delayed_constraints(
_tmp7D);if(!Cyc_Tcenv_all_labels_resolved(_tmp7D))({void*_tmpF4[0]={};Cyc_Tcutil_terr(
_tmp33->loc,({const char*_tmpF5="function has goto statements to undefined labels";
_tag_dynforward(_tmpF5,sizeof(char),_get_zero_arr_size(_tmpF5,49));}),
_tag_dynforward(_tmpF4,sizeof(void*),0));});_tmp7D->le=(struct Cyc_Tcenv_Fenv*)
old_fenv;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp7D,s0),_tmp34,0);
_npop_handler(0);return;}}}}_LL33: if(*((int*)_tmp7E)!= 8)goto _LL35;_tmp86=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp7E)->f1;_tmp87=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp7E)->f2;_LL34: unimp_msg_part=({const char*_tmpF8="namespace";_tag_dynforward(
_tmpF8,sizeof(char),_get_zero_arr_size(_tmpF8,10));});goto _LL2A;_LL35: if(*((int*)
_tmp7E)!= 9)goto _LL37;_tmp88=((struct Cyc_Absyn_Using_d_struct*)_tmp7E)->f1;
_tmp89=((struct Cyc_Absyn_Using_d_struct*)_tmp7E)->f2;_LL36: unimp_msg_part=({
const char*_tmpF9="using";_tag_dynforward(_tmpF9,sizeof(char),_get_zero_arr_size(
_tmpF9,6));});goto _LL2A;_LL37: if(*((int*)_tmp7E)!= 4)goto _LL39;_LL38:
unimp_msg_part=({const char*_tmpFA="type";_tag_dynforward(_tmpFA,sizeof(char),
_get_zero_arr_size(_tmpFA,5));});goto _LL2A;_LL39: if(*((int*)_tmp7E)!= 5)goto
_LL3B;_LL3A: unimp_msg_part=({const char*_tmpFB="[x]tunion";_tag_dynforward(_tmpFB,
sizeof(char),_get_zero_arr_size(_tmpFB,10));});goto _LL2A;_LL3B: if(*((int*)_tmp7E)
!= 6)goto _LL3D;_LL3C: unimp_msg_part=({const char*_tmpFC="enum";_tag_dynforward(
_tmpFC,sizeof(char),_get_zero_arr_size(_tmpFC,5));});goto _LL2A;_LL3D: if(*((int*)
_tmp7E)!= 7)goto _LL3F;_LL3E: unimp_msg_part=({const char*_tmpFD="typedef";
_tag_dynforward(_tmpFD,sizeof(char),_get_zero_arr_size(_tmpFD,8));});goto _LL2A;
_LL3F: if(*((int*)_tmp7E)!= 10)goto _LL41;_LL40: unimp_msg_part=({const char*_tmpFE="extern \"C\"";
_tag_dynforward(_tmpFE,sizeof(char),_get_zero_arr_size(_tmpFE,11));});goto _LL2A;
_LL41: if(*((int*)_tmp7E)!= 11)goto _LL43;_LL42: unimp_msg_part=({const char*_tmpFF="extern \"C include\"";
_tag_dynforward(_tmpFF,sizeof(char),_get_zero_arr_size(_tmpFF,19));});goto _LL2A;
_LL43: if((int)_tmp7E != 0)goto _LL45;_LL44: unimp_msg_part=({const char*_tmp100="__cyclone_port_on__";
_tag_dynforward(_tmp100,sizeof(char),_get_zero_arr_size(_tmp100,20));});goto
_LL2A;_LL45: if((int)_tmp7E != 1)goto _LL2A;_LL46: unimp_msg_part=({const char*
_tmp101="__cyclone_port_off__";_tag_dynforward(_tmp101,sizeof(char),
_get_zero_arr_size(_tmp101,21));});goto _LL2A;_LL2A:;}};_pop_region(decl_rgn);}(
int)_throw(({struct Cyc_String_pa_struct _tmp104;_tmp104.tag=0;_tmp104.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)unimp_msg_part);{void*_tmp102[1]={&
_tmp104};Cyc_Tcutil_impos(({const char*_tmp103="tcStmt: nested %s declarations unimplemented";
_tag_dynforward(_tmp103,sizeof(char),_get_zero_arr_size(_tmp103,45));}),
_tag_dynforward(_tmp102,sizeof(void*),1));}}));}_LL21: if(_tmpB <= (void*)1)goto
_LL23;if(*((int*)_tmpB)!= 15)goto _LL23;_tmp35=((struct Cyc_Absyn_Region_s_struct*)
_tmpB)->f1;_tmp36=((struct Cyc_Absyn_Region_s_struct*)_tmpB)->f2;_tmp37=((struct
Cyc_Absyn_Region_s_struct*)_tmpB)->f3;_tmp38=((struct Cyc_Absyn_Region_s_struct*)
_tmpB)->f4;_LL22:(void*)(_tmp38->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Seq_s_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({
struct Cyc_Absyn_Seq_s_struct _tmp106;_tmp106.tag=1;_tmp106.f1=Cyc_Absyn_new_stmt((
void*)_tmp38->r,_tmp38->loc);_tmp106.f2=Cyc_Absyn_skip_stmt(_tmp38->loc);_tmp106;});
_tmp105;}),_tmp38->loc))->r));{struct _RegionHandle _tmp107=_new_region("r");
struct _RegionHandle*r=& _tmp107;_push_region(r);{struct Cyc_Tcenv_Tenv*te2;if(
new_block)te2=Cyc_Tcenv_new_block(r,s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(r,
te);}{struct Cyc_Absyn_Vardecl _tmp109;void*_tmp10A;struct Cyc_Core_Opt*_tmp10B;
struct Cyc_Core_Opt**_tmp10C;struct Cyc_Absyn_Vardecl*_tmp108=_tmp36;_tmp109=*
_tmp108;_tmp10A=(void*)_tmp109.type;_tmp10B=_tmp109.rgn;_tmp10C=(struct Cyc_Core_Opt**)&(*
_tmp108).rgn;{void*_tmp10D=Cyc_Tcenv_curr_rgn(te2);void*rgn_typ=(void*)({struct
Cyc_Absyn_VarType_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct
Cyc_Absyn_VarType_struct _tmp115;_tmp115.tag=1;_tmp115.f1=_tmp35;_tmp115;});
_tmp114;});*_tmp10C=({struct Cyc_Core_Opt*_tmp10E=_cycalloc(sizeof(*_tmp10E));
_tmp10E->v=(void*)_tmp10D;_tmp10E;});te2=Cyc_Tcenv_add_type_vars(r,s0->loc,te2,({
struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->hd=_tmp35;
_tmp10F->tl=0;_tmp10F;}));Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(
te2),(void*)2,1,_tmp10A);te2=Cyc_Tcenv_add_region(r,te2,rgn_typ,_tmp37);if(!Cyc_Tcutil_unify(
_tmp10A,(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp110=_cycalloc(sizeof(*
_tmp110));_tmp110[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp111;_tmp111.tag=
15;_tmp111.f1=(void*)rgn_typ;_tmp111;});_tmp110;})))({void*_tmp112[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp113="region stmt: type of region handle is wrong!";_tag_dynforward(_tmp113,
sizeof(char),_get_zero_arr_size(_tmp113,45));}),_tag_dynforward(_tmp112,sizeof(
void*),0));});Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_add_local_var(
r,_tmp38->loc,Cyc_Tcenv_enter_try(r,te2),_tmp36),s0),_tmp38,0);}}};_pop_region(r);}
return;_LL23: if(_tmpB <= (void*)1)goto _LL0;if(*((int*)_tmpB)!= 16)goto _LL0;_tmp39=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmpB)->f1;_LL24: {void*rgn_type=Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->v=(void*)((void*)
3);_tmp11B;}),({struct Cyc_Core_Opt*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp11C;}));void*etype=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp11A;_tmp11A.tag=15;_tmp11A.f1=(void*)rgn_type;_tmp11A;});_tmp119;});if(!Cyc_Tcutil_unify(
Cyc_Tcexp_tcExp(te,(void**)& etype,_tmp39),etype))({struct Cyc_String_pa_struct
_tmp118;_tmp118.tag=0;_tmp118.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39->topt))->v));{
void*_tmp116[1]={& _tmp118};Cyc_Tcutil_terr(_tmp39->loc,({const char*_tmp117="expecting region_t but found %s";
_tag_dynforward(_tmp117,sizeof(char),_get_zero_arr_size(_tmp117,32));}),
_tag_dynforward(_tmp116,sizeof(void*),1));}});Cyc_Tcenv_check_rgn_resetable(te,
s0->loc,rgn_type);return;}_LL0:;}}
