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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct
_dynforward_ptr Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int
Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rempty(
struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(
struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*
x);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(
struct _tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _dynforward_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*
Cyc_Tcutil_snd_tqt(struct _tuple4*);struct _tuple5{unsigned int f1;int f2;};struct
_tuple5 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
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
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
int Cyc_Toc_warn_bounds_checks=0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=
0;unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;static struct Cyc_List_List*Cyc_Toc_tuple_types=0;static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp0[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="\n";_tag_dynforward(
_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,2));}),_tag_dynforward(_tmp0,sizeof(
void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)
Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*_tmp2[0]={};
Cyc_fprintf(Cyc_stderr,({const char*_tmp3="\n";_tag_dynforward(_tmp3,sizeof(char),
_get_zero_arr_size(_tmp3,2));}),_tag_dynforward(_tmp2,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";static char _tmp4[5]="curr";
static struct _dynforward_ptr Cyc_Toc_curr_string={_tmp4,_tmp4 + 5};static struct
_dynforward_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp5[4]="tag";
static struct _dynforward_ptr Cyc_Toc_tag_string={_tmp5,_tmp5 + 4};static struct
_dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp6[4]="val";
static struct _dynforward_ptr Cyc_Toc_val_string={_tmp6,_tmp6 + 4};static struct
_dynforward_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp7[14]="_handler_cons";
static struct _dynforward_ptr Cyc_Toc__handler_cons_string={_tmp7,_tmp7 + 14};static
struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp8[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp8,_tmp8 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp9[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp9,_tmp9 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmpA[7]="_throw";static struct _dynforward_ptr Cyc_Toc__throw_str={_tmpA,
_tmpA + 7};static struct _tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__throw_ev={0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpD[7]="setjmp";
static struct _dynforward_ptr Cyc_Toc_setjmp_str={_tmpD,_tmpD + 7};static struct
_tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,&
Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp10[
14]="_push_handler";static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmp10,
_tmp10 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp13[13]="_pop_handler";static struct
_dynforward_ptr Cyc_Toc__pop_handler_str={_tmp13,_tmp13 + 13};static struct _tuple1
Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp16[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp16,_tmp16 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp19[14]="_npop_handler";static struct
_dynforward_ptr Cyc_Toc__npop_handler_str={_tmp19,_tmp19 + 14};static struct _tuple1
Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp1C[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp1C,_tmp1C + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp1F[28]="_check_known_subscript_null";static
struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={_tmp1F,_tmp1F + 28};
static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp22[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp22,_tmp22 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp25[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp25,
_tmp25 + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dynforward_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp28[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp28,_tmp28 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp2B[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp2B,_tmp2B + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_re={1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=&
Cyc_Toc__dynforward_ptr_ev;static char _tmp2E[15]="_dyneither_ptr";static struct
_dynforward_ptr Cyc_Toc__dyneither_ptr_str={_tmp2E,_tmp2E + 15};static struct
_tuple1 Cyc_Toc__dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp31[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp31,_tmp31 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_dynforward_re={1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=&
Cyc_Toc__tag_dynforward_ev;static char _tmp34[15]="_tag_dyneither";static struct
_dynforward_ptr Cyc_Toc__tag_dyneither_str={_tmp34,_tmp34 + 15};static struct
_tuple1 Cyc_Toc__tag_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp37[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp37,_tmp37 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dynforward_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp3A[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp3A,_tmp3A + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp3D[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp3D,_tmp3D + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dynforward_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp40[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp40,_tmp40 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp43[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp43,_tmp43 + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dynforward_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp46[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp46,_tmp46 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp49[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp49,_tmp49 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp4C[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp4C,_tmp4C + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp4F[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp4F,_tmp4F + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp52[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp52,_tmp52 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=&
Cyc_Toc__zero_arr_plus_ev;static char _tmp55[29]="_dynforward_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp55,
_tmp55 + 29};static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={1,&
Cyc_Toc__dynforward_ptr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_ev;static char _tmp58[28]="_dyneither_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp58,
_tmp58 + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp5B[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp5B,_tmp5B + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp5E[34]="_dynforward_ptr_inplace_plus_post";static struct
_dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmp5E,_tmp5E + 34};
static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_post_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmp61[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp61,
_tmp61 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp64[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmp64,_tmp64 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_to_dyneither_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={1,& Cyc_Toc__dynforward_to_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_to_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=& Cyc_Toc__dynforward_to_dyneither_ev;
static char _tmp67[25]="_dyneither_to_dynforward";static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={
_tmp67,_tmp67 + 25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_to_dynforward_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={1,& Cyc_Toc__dyneither_to_dynforward_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_to_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=& Cyc_Toc__dyneither_to_dynforward_ev;
static char _tmp6A[28]="_zero_arr_inplace_plus_post";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp6A,_tmp6A + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp6D[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmp6D,_tmp6D + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmp70[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmp70,_tmp70 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmp73[17]="_cycalloc_atomic";static struct
_dynforward_ptr Cyc_Toc__cycalloc_atomic_str={_tmp73,_tmp73 + 17};static struct
_tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp76[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp76,_tmp76 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp79[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmp79,_tmp79 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp7C[15]="_region_calloc";static struct
_dynforward_ptr Cyc_Toc__region_calloc_str={_tmp7C,_tmp7C + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmp7F[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmp7F,_tmp7F + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp82[12]="_new_region";static struct
_dynforward_ptr Cyc_Toc__new_region_str={_tmp82,_tmp82 + 12};static struct _tuple1
Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmp85[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmp85,_tmp85 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp88[12]="_pop_region";static struct
_dynforward_ptr Cyc_Toc__pop_region_str={_tmp88,_tmp88 + 12};static struct _tuple1
Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmp8B[14]="_reset_region";static struct _dynforward_ptr Cyc_Toc__reset_region_str={
_tmp8B,_tmp8B + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp8E[19]="_throw_arraybounds";static struct
_dynforward_ptr Cyc_Toc__throw_arraybounds_str={_tmp8E,_tmp8E + 19};static struct
_tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={
1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp91[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmp91,_tmp91 + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={1,&
Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmp94[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp94,
_tmp94 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={
10,{(union Cyc_Absyn_AggrInfoU_union)((struct Cyc_Absyn_UnknownAggr_struct){0,(
void*)((void*)0),& Cyc_Toc__dynforward_ptr_pr}),0}};static void*Cyc_Toc_dynforward_ptr_typ=(
void*)& Cyc_Toc_dynforward_ptr_typ_v;static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={
10,{(union Cyc_Absyn_AggrInfoU_union)((struct Cyc_Absyn_UnknownAggr_struct){0,(
void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),0}};static void*Cyc_Toc_dyneither_ptr_typ=(
void*)& Cyc_Toc_dyneither_ptr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**
skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**
_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=Cyc_Absyn_skip_stmt(0);_tmp9B;});
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*
t,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmp9C=
_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_Cast_e_struct _tmp9D;
_tmp9D.tag=15;_tmp9D.f1=(void*)t;_tmp9D.f2=e;_tmp9D.f3=0;_tmp9D.f4=(void*)((void*)
1);_tmp9D;});_tmp9C;});}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));
_tmp9E[0]=({struct Cyc_Absyn_Deref_e_struct _tmp9F;_tmp9F.tag=22;_tmp9F.f1=e;
_tmp9F;});_tmp9E;});}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpA1;
_tmpA1.tag=25;_tmpA1.f1=e1;_tmpA1.f2=e2;_tmpA1;});_tmpA0;});}static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpA3;
_tmpA3.tag=37;_tmpA3.f1=s;_tmpA3;});_tmpA2;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA4=_cycalloc(
sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpA5;_tmpA5.tag=
18;_tmpA5.f1=(void*)t;_tmpA5;});_tmpA4;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpA7;_tmpA7.tag=11;_tmpA7.f1=e;_tmpA7.f2=es;_tmpA7.f3=0;_tmpA7;});_tmpA6;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_Exp_s_struct _tmpA9;
_tmpA9.tag=0;_tmpA9.f1=e;_tmpA9;});_tmpA8;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_Seq_s_struct _tmpAB;
_tmpAB.tag=1;_tmpAB.f1=s1;_tmpAB.f2=s2;_tmpAB;});_tmpAA;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));
_tmpAC[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpAD;_tmpAD.tag=6;_tmpAD.f1=e1;
_tmpAD.f2=e2;_tmpAD.f3=e3;_tmpAD;});_tmpAC;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpAF;_tmpAF.tag=23;_tmpAF.f1=e;_tmpAF.f2=n;_tmpAF;});_tmpAE;});}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_AggrArrow_e_struct
_tmpB1;_tmpB1.tag=24;_tmpB1.f1=e;_tmpB1.f2=n;_tmpB1;});_tmpB0;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpB3;_tmpB3.tag=36;_tmpB3.f1=tdopt;_tmpB3.f2=ds;_tmpB3;});_tmpB2;});}static
void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){return(
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpB5;_tmpB5.tag=7;_tmpB5.f1=v;_tmpB5.f2=s;_tmpB5;});
_tmpB4;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),0})};struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmpB8=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpB9;char _tmpBA;union Cyc_Absyn_Cnst_union
_tmpBB;short _tmpBC;union Cyc_Absyn_Cnst_union _tmpBD;int _tmpBE;union Cyc_Absyn_Cnst_union
_tmpBF;long long _tmpC0;union Cyc_Absyn_Cnst_union _tmpC1;struct Cyc_Absyn_Exp*
_tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_List_List*
_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_List_List*_tmpC7;_LL1: if(*((int*)
_tmpB8)!= 0)goto _LL3;_tmpB9=((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1).Char_c).tag != 0)goto _LL3;_tmpBA=(
_tmpB9.Char_c).f2;_LL2: return _tmpBA == '\000';_LL3: if(*((int*)_tmpB8)!= 0)goto
_LL5;_tmpBB=((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpB8)->f1).Short_c).tag != 1)goto _LL5;_tmpBC=(_tmpBB.Short_c).f2;_LL4: return
_tmpBC == 0;_LL5: if(*((int*)_tmpB8)!= 0)goto _LL7;_tmpBD=((struct Cyc_Absyn_Const_e_struct*)
_tmpB8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1).Int_c).tag != 2)
goto _LL7;_tmpBE=(_tmpBD.Int_c).f2;_LL6: return _tmpBE == 0;_LL7: if(*((int*)_tmpB8)
!= 0)goto _LL9;_tmpBF=((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmpB8)->f1).LongLong_c).tag != 3)goto _LL9;_tmpC0=(
_tmpBF.LongLong_c).f2;_LL8: return _tmpC0 == 0;_LL9: if(*((int*)_tmpB8)!= 0)goto _LLB;
_tmpC1=((struct Cyc_Absyn_Const_e_struct*)_tmpB8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpB8)->f1).Null_c).tag != 6)goto _LLB;_LLA: return 1;_LLB: if(*((int*)_tmpB8)!= 15)
goto _LLD;_tmpC2=((struct Cyc_Absyn_Cast_e_struct*)_tmpB8)->f2;_LLC: return Cyc_Toc_is_zero(
_tmpC2);_LLD: if(*((int*)_tmpB8)!= 26)goto _LLF;_tmpC3=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpB8)->f1;_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpC3);_LLF: if(*((int*)_tmpB8)!= 28)goto
_LL11;_tmpC4=((struct Cyc_Absyn_Array_e_struct*)_tmpB8)->f1;_LL10: _tmpC5=_tmpC4;
goto _LL12;_LL11: if(*((int*)_tmpB8)!= 30)goto _LL13;_tmpC5=((struct Cyc_Absyn_Struct_e_struct*)
_tmpB8)->f3;_LL12: _tmpC6=_tmpC5;goto _LL14;_LL13: if(*((int*)_tmpB8)!= 27)goto
_LL15;_tmpC6=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpB8)->f2;_LL14: _tmpC7=
_tmpC6;goto _LL16;_LL15: if(*((int*)_tmpB8)!= 36)goto _LL17;_tmpC7=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpB8)->f2;_LL16: for(0;_tmpC7 != 0;_tmpC7=_tmpC7->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple6*)_tmpC7->hd)).f2))return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpC8=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpC9;struct Cyc_Absyn_PtrAtts _tmpCA;struct Cyc_Absyn_Conref*
_tmpCB;_LL1A: if(_tmpC8 <= (void*)4)goto _LL1C;if(*((int*)_tmpC8)!= 4)goto _LL1C;
_tmpC9=((struct Cyc_Absyn_PointerType_struct*)_tmpC8)->f1;_tmpCA=_tmpC9.ptr_atts;
_tmpCB=_tmpCA.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCB);_LL1C:;_LL1D:({void*_tmpCC[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpCD="is_nullable";
_tag_dynforward(_tmpCD,sizeof(char),_get_zero_arr_size(_tmpCD,12));}),
_tag_dynforward(_tmpCC,sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dynforward_ptr tag){return({struct _tuple1*_tmpCE=_cycalloc(
sizeof(*_tmpCE));_tmpCE->f1=(*x).f1;_tmpCE->f2=({struct _dynforward_ptr*_tmpCF=
_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)*(*x).f2,(struct _dynforward_ptr)tag);_tmpCF;});_tmpCE;});}
struct _tuple7{struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};static struct
_dynforward_ptr*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{struct Cyc_List_List*
_tmpD0=Cyc_Toc_tuple_types;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct _tuple7
_tmpD2;struct _dynforward_ptr*_tmpD3;struct Cyc_List_List*_tmpD4;struct _tuple7*
_tmpD1=(struct _tuple7*)_tmpD0->hd;_tmpD2=*_tmpD1;_tmpD3=_tmpD2.f1;_tmpD4=_tmpD2.f2;{
struct Cyc_List_List*_tmpD5=tqs0;for(0;_tmpD5 != 0  && _tmpD4 != 0;(_tmpD5=_tmpD5->tl,
_tmpD4=_tmpD4->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmpD5->hd)).f2,(
void*)_tmpD4->hd))break;}if(_tmpD5 == 0  && _tmpD4 == 0)return _tmpD3;}}}{struct
_dynforward_ptr*x=({struct _dynforward_ptr*_tmpE3=_cycalloc(sizeof(*_tmpE3));
_tmpE3[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpE6;_tmpE6.tag=1;
_tmpE6.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmpE4[1]={& _tmpE6};
Cyc_aprintf(({const char*_tmpE5="_tuple%d";_tag_dynforward(_tmpE5,sizeof(char),
_get_zero_arr_size(_tmpE5,9));}),_tag_dynforward(_tmpE4,sizeof(void*),1));}});
_tmpE3;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmpD6=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmpD6=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=({
struct Cyc_Absyn_Aggrfield*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->name=Cyc_Absyn_fieldname(
i);_tmpD8->tq=Cyc_Toc_mt_tq;_tmpD8->type=(void*)((void*)ts2->hd);_tmpD8->width=0;
_tmpD8->attributes=0;_tmpD8;});_tmpD7->tl=_tmpD6;_tmpD7;});}}_tmpD6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD6);{struct Cyc_Absyn_Aggrdecl*
_tmpD9=({struct Cyc_Absyn_Aggrdecl*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->kind=(
void*)((void*)0);_tmpDF->sc=(void*)((void*)2);_tmpDF->name=({struct _tuple1*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmpE2;(_tmpE2.Rel_n).tag=1;(_tmpE2.Rel_n).f1=0;
_tmpE2;});_tmpE1->f2=x;_tmpE1;});_tmpDF->tvs=0;_tmpDF->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->exist_vars=0;_tmpE0->rgn_po=0;_tmpE0->fields=
_tmpD6;_tmpE0;});_tmpDF->attributes=0;_tmpDF;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmpDC;_tmpDC.tag=4;_tmpDC.f1=_tmpD9;_tmpDC;});_tmpDB;}),0);_tmpDA->tl=Cyc_Toc_result_decls;
_tmpDA;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*
_tmpDD));_tmpDD->hd=({struct _tuple7*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->f1=
x;_tmpDE->f2=ts;_tmpDE;});_tmpDD->tl=Cyc_Toc_tuple_types;_tmpDD;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmpE7=_cycalloc(sizeof(*
_tmpE7));_tmpE7->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmpEC;(_tmpEC.Loc_n).tag=0;_tmpEC;});_tmpE7->f2=({struct _dynforward_ptr*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmpEB;_tmpEB.tag=1;_tmpEB.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmpE9[1]={& _tmpEB};Cyc_aprintf(({const char*_tmpEA="_tmp%X";_tag_dynforward(
_tmpEA,sizeof(char),_get_zero_arr_size(_tmpEA,7));}),_tag_dynforward(_tmpE9,
sizeof(void*),1));}});_tmpE8;});_tmpE7;});}static struct _dynforward_ptr*Cyc_Toc_fresh_label(){
return({struct _dynforward_ptr*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpF0;_tmpF0.tag=1;_tmpF0.f1=(
unsigned int)Cyc_Toc_fresh_label_counter ++;{void*_tmpEE[1]={& _tmpF0};Cyc_aprintf(({
const char*_tmpEF="_LL%X";_tag_dynforward(_tmpEF,sizeof(char),_get_zero_arr_size(
_tmpEF,6));}),_tag_dynforward(_tmpEE,sizeof(void*),1));}});_tmpED;});}static
struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,struct
_tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmpF1=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmpF1))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpF1->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpF1->hd)->typs == 0)
++ ans;_tmpF1=_tmpF1->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmpF2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmpF2->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmpF4;struct Cyc_Absyn_Tqual
_tmpF5;void*_tmpF6;struct _tuple2 _tmpF3=*a;_tmpF4=_tmpF3.f1;_tmpF5=_tmpF3.f2;
_tmpF6=_tmpF3.f3;return({struct _tuple2*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->f1=
_tmpF4;_tmpF7->f2=_tmpF5;_tmpF7->f3=Cyc_Toc_typ_to_c(_tmpF6);_tmpF7;});}static
struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual _tmpF9;
void*_tmpFA;struct _tuple4 _tmpF8=*x;_tmpF9=_tmpF8.f1;_tmpFA=_tmpF8.f2;return({
struct _tuple4*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=_tmpF9;_tmpFB->f2=Cyc_Toc_typ_to_c(
_tmpFA);_tmpFB;});}static void*Cyc_Toc_typ_to_c_array(void*t){void*_tmpFC=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFD;void*_tmpFE;struct Cyc_Absyn_Tqual _tmpFF;
struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Conref*_tmp101;struct Cyc_Position_Segment*
_tmp102;struct Cyc_Core_Opt*_tmp103;struct Cyc_Core_Opt _tmp104;void*_tmp105;_LL1F:
if(_tmpFC <= (void*)4)goto _LL23;if(*((int*)_tmpFC)!= 7)goto _LL21;_tmpFD=((struct
Cyc_Absyn_ArrayType_struct*)_tmpFC)->f1;_tmpFE=(void*)_tmpFD.elt_type;_tmpFF=
_tmpFD.tq;_tmp100=_tmpFD.num_elts;_tmp101=_tmpFD.zero_term;_tmp102=_tmpFD.zt_loc;
_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmpFE),_tmpFF,_tmp100,
Cyc_Absyn_false_conref,_tmp102);_LL21: if(*((int*)_tmpFC)!= 0)goto _LL23;_tmp103=((
struct Cyc_Absyn_Evar_struct*)_tmpFC)->f2;if(_tmp103 == 0)goto _LL23;_tmp104=*
_tmp103;_tmp105=(void*)_tmp104.v;_LL22: return Cyc_Toc_typ_to_c_array(_tmp105);
_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->name=f->name;_tmp106->tq=Cyc_Toc_mt_tq;
_tmp106->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp106->width=f->width;
_tmp106->attributes=f->attributes;_tmp106;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp107=
t;struct Cyc_Core_Opt*_tmp108;struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt
_tmp10A;void*_tmp10B;struct Cyc_Absyn_Tvar*_tmp10C;struct Cyc_Absyn_TunionInfo
_tmp10D;union Cyc_Absyn_TunionInfoU_union _tmp10E;struct Cyc_Absyn_Tuniondecl**
_tmp10F;struct Cyc_Absyn_Tuniondecl*_tmp110;struct Cyc_Absyn_TunionFieldInfo
_tmp111;union Cyc_Absyn_TunionFieldInfoU_union _tmp112;struct Cyc_Absyn_Tuniondecl*
_tmp113;struct Cyc_Absyn_Tunionfield*_tmp114;struct Cyc_Absyn_PtrInfo _tmp115;void*
_tmp116;struct Cyc_Absyn_Tqual _tmp117;struct Cyc_Absyn_PtrAtts _tmp118;struct Cyc_Absyn_Conref*
_tmp119;struct Cyc_Absyn_ArrayInfo _tmp11A;void*_tmp11B;struct Cyc_Absyn_Tqual
_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Position_Segment*_tmp11E;struct Cyc_Absyn_FnInfo
_tmp11F;void*_tmp120;struct Cyc_List_List*_tmp121;int _tmp122;struct Cyc_Absyn_VarargInfo*
_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_List_List*_tmp125;void*_tmp126;
struct Cyc_List_List*_tmp127;struct Cyc_Absyn_AggrInfo _tmp128;union Cyc_Absyn_AggrInfoU_union
_tmp129;struct Cyc_List_List*_tmp12A;struct _tuple1*_tmp12B;struct Cyc_List_List*
_tmp12C;struct _tuple1*_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_Absyn_Typedefdecl*
_tmp12F;void**_tmp130;void*_tmp131;_LL26: if((int)_tmp107 != 0)goto _LL28;_LL27:
return t;_LL28: if(_tmp107 <= (void*)4)goto _LL3A;if(*((int*)_tmp107)!= 0)goto _LL2A;
_tmp108=((struct Cyc_Absyn_Evar_struct*)_tmp107)->f2;if(_tmp108 != 0)goto _LL2A;
_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(*((int*)_tmp107)!= 0)goto _LL2C;_tmp109=((
struct Cyc_Absyn_Evar_struct*)_tmp107)->f2;if(_tmp109 == 0)goto _LL2C;_tmp10A=*
_tmp109;_tmp10B=(void*)_tmp10A.v;_LL2B: return Cyc_Toc_typ_to_c(_tmp10B);_LL2C: if(*((
int*)_tmp107)!= 1)goto _LL2E;_tmp10C=((struct Cyc_Absyn_VarType_struct*)_tmp107)->f1;
_LL2D: if(Cyc_Tcutil_tvar_kind(_tmp10C)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL2E: if(*((int*)_tmp107)!= 2)goto _LL30;_tmp10D=((struct Cyc_Absyn_TunionType_struct*)
_tmp107)->f1;_tmp10E=_tmp10D.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp107)->f1).tunion_info).KnownTunion).tag != 1)goto _LL30;_tmp10F=(_tmp10E.KnownTunion).f1;
_tmp110=*_tmp10F;_LL2F: if(_tmp110->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp110->name,({const char*_tmp132="_union";_tag_dynforward(_tmp132,sizeof(char),
_get_zero_arr_size(_tmp132,7));})));else{return Cyc_Absyn_void_star_typ();}_LL30:
if(*((int*)_tmp107)!= 2)goto _LL32;_LL31:({void*_tmp133[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp134="unresolved TunionType";_tag_dynforward(_tmp134,sizeof(char),
_get_zero_arr_size(_tmp134,22));}),_tag_dynforward(_tmp133,sizeof(void*),0));});
_LL32: if(*((int*)_tmp107)!= 3)goto _LL34;_tmp111=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp107)->f1;_tmp112=_tmp111.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp107)->f1).field_info).KnownTunionfield).tag != 1)goto _LL34;_tmp113=(_tmp112.KnownTunionfield).f1;
_tmp114=(_tmp112.KnownTunionfield).f2;_LL33: if(_tmp113->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp113->name,({const char*_tmp135="_union";
_tag_dynforward(_tmp135,sizeof(char),_get_zero_arr_size(_tmp135,7));})));if(
_tmp114->typs == 0){if(_tmp113->is_xtunion)return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);else{return Cyc_Absyn_uint_typ;}}
else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp114->name,({const char*
_tmp136="_struct";_tag_dynforward(_tmp136,sizeof(char),_get_zero_arr_size(
_tmp136,8));})));}_LL34: if(*((int*)_tmp107)!= 3)goto _LL36;_LL35:({void*_tmp137[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp138="unresolved TunionFieldType";_tag_dynforward(_tmp138,sizeof(
char),_get_zero_arr_size(_tmp138,27));}),_tag_dynforward(_tmp137,sizeof(void*),0));});
_LL36: if(*((int*)_tmp107)!= 4)goto _LL38;_tmp115=((struct Cyc_Absyn_PointerType_struct*)
_tmp107)->f1;_tmp116=(void*)_tmp115.elt_typ;_tmp117=_tmp115.elt_tq;_tmp118=
_tmp115.ptr_atts;_tmp119=_tmp118.bounds;_LL37: _tmp116=Cyc_Toc_typ_to_c_array(
_tmp116);{union Cyc_Absyn_Constraint_union _tmp139=(Cyc_Absyn_compress_conref(
_tmp119))->v;void*_tmp13A;void*_tmp13B;_LL61: if((_tmp139.No_constr).tag != 2)goto
_LL63;_LL62: goto _LL64;_LL63: if((_tmp139.Eq_constr).tag != 0)goto _LL65;_tmp13A=(
_tmp139.Eq_constr).f1;if((int)_tmp13A != 0)goto _LL65;_LL64: return Cyc_Toc_dynforward_ptr_typ;
_LL65: if((_tmp139.Eq_constr).tag != 0)goto _LL67;_tmp13B=(_tmp139.Eq_constr).f1;
if((int)_tmp13B != 1)goto _LL67;_LL66: return Cyc_Toc_dyneither_ptr_typ;_LL67:;_LL68:
return Cyc_Absyn_star_typ(_tmp116,(void*)2,_tmp117,Cyc_Absyn_false_conref);_LL60:;}
_LL38: if(*((int*)_tmp107)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp107 != 
1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp107 <= (void*)4)goto _LL56;if(*((int*)
_tmp107)!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(*((int*)_tmp107)!= 7)goto _LL40;
_tmp11A=((struct Cyc_Absyn_ArrayType_struct*)_tmp107)->f1;_tmp11B=(void*)_tmp11A.elt_type;
_tmp11C=_tmp11A.tq;_tmp11D=_tmp11A.num_elts;_tmp11E=_tmp11A.zt_loc;_LL3F: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp11B),_tmp11C,_tmp11D,Cyc_Absyn_false_conref,
_tmp11E);_LL40: if(*((int*)_tmp107)!= 8)goto _LL42;_tmp11F=((struct Cyc_Absyn_FnType_struct*)
_tmp107)->f1;_tmp120=(void*)_tmp11F.ret_typ;_tmp121=_tmp11F.args;_tmp122=_tmp11F.c_varargs;
_tmp123=_tmp11F.cyc_varargs;_tmp124=_tmp11F.attributes;_LL41: {struct Cyc_List_List*
_tmp13C=0;for(0;_tmp124 != 0;_tmp124=_tmp124->tl){void*_tmp13D=(void*)_tmp124->hd;
_LL6A: if((int)_tmp13D != 3)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)_tmp13D != 4)
goto _LL6E;_LL6D: goto _LL6F;_LL6E: if(_tmp13D <= (void*)17)goto _LL72;if(*((int*)
_tmp13D)!= 3)goto _LL70;_LL6F: continue;_LL70: if(*((int*)_tmp13D)!= 4)goto _LL72;
_LL71: continue;_LL72:;_LL73: _tmp13C=({struct Cyc_List_List*_tmp13E=_cycalloc(
sizeof(*_tmp13E));_tmp13E->hd=(void*)((void*)_tmp124->hd);_tmp13E->tl=_tmp13C;
_tmp13E;});goto _LL69;_LL69:;}{struct Cyc_List_List*_tmp13F=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp121);if(_tmp123 != 0){void*_tmp140=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((
void*)_tmp123->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp141=({struct _tuple2*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->f1=
_tmp123->name;_tmp143->f2=_tmp123->tq;_tmp143->f3=_tmp140;_tmp143;});_tmp13F=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp13F,({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->hd=
_tmp141;_tmp142->tl=0;_tmp142;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144[0]=({struct Cyc_Absyn_FnType_struct
_tmp145;_tmp145.tag=8;_tmp145.f1=({struct Cyc_Absyn_FnInfo _tmp146;_tmp146.tvars=0;
_tmp146.effect=0;_tmp146.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp120);_tmp146.args=
_tmp13F;_tmp146.c_varargs=_tmp122;_tmp146.cyc_varargs=0;_tmp146.rgn_po=0;_tmp146.attributes=
_tmp13C;_tmp146;});_tmp145;});_tmp144;});}}_LL42: if(*((int*)_tmp107)!= 9)goto
_LL44;_tmp125=((struct Cyc_Absyn_TupleType_struct*)_tmp107)->f1;_LL43: _tmp125=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp125);{struct _dynforward_ptr*_tmp147=Cyc_Toc_add_tuple_type(
_tmp125);return Cyc_Absyn_strct(_tmp147);}_LL44: if(*((int*)_tmp107)!= 11)goto
_LL46;_tmp126=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp107)->f1;_tmp127=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp107)->f2;_LL45: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp149;_tmp149.tag=11;_tmp149.f1=(void*)_tmp126;_tmp149.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp127);_tmp149;});_tmp148;});_LL46: if(*((
int*)_tmp107)!= 10)goto _LL48;_tmp128=((struct Cyc_Absyn_AggrType_struct*)_tmp107)->f1;
_tmp129=_tmp128.aggr_info;_tmp12A=_tmp128.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*
_tmp14A=Cyc_Absyn_get_known_aggrdecl(_tmp129);if((void*)_tmp14A->kind == (void*)1)
return Cyc_Absyn_unionq_typ(_tmp14A->name);return Cyc_Absyn_strctq(_tmp14A->name);}
_LL48: if(*((int*)_tmp107)!= 12)goto _LL4A;_tmp12B=((struct Cyc_Absyn_EnumType_struct*)
_tmp107)->f1;_LL49: return t;_LL4A: if(*((int*)_tmp107)!= 13)goto _LL4C;_tmp12C=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp107)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmp12C);return t;_LL4C: if(*((int*)_tmp107)!= 16)goto _LL4E;_tmp12D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp107)->f1;_tmp12E=((struct Cyc_Absyn_TypedefType_struct*)_tmp107)->f2;_tmp12F=((
struct Cyc_Absyn_TypedefType_struct*)_tmp107)->f3;_tmp130=((struct Cyc_Absyn_TypedefType_struct*)
_tmp107)->f4;_LL4D: if(_tmp130 == 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp14C;_tmp14C.tag=16;_tmp14C.f1=_tmp12D;_tmp14C.f2=0;_tmp14C.f3=_tmp12F;
_tmp14C.f4=0;_tmp14C;});_tmp14B;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp14E;_tmp14E.tag=16;_tmp14E.f1=_tmp12D;_tmp14E.f2=0;_tmp14E.f3=_tmp12F;
_tmp14E.f4=({void**_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F[0]=Cyc_Toc_typ_to_c_array(*
_tmp130);_tmp14F;});_tmp14E;});_tmp14D;});}_LL4E: if(*((int*)_tmp107)!= 14)goto
_LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp107)!= 17)goto _LL52;_LL51: return Cyc_Absyn_uint_typ;
_LL52: if(*((int*)_tmp107)!= 15)goto _LL54;_tmp131=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp107)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(*((int*)_tmp107)!= 18)goto _LL56;_LL55:({void*_tmp150[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp151="Toc::typ_to_c: type translation passed a type integer";
_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size(_tmp151,54));}),
_tag_dynforward(_tmp150,sizeof(void*),0));});_LL56: if((int)_tmp107 != 2)goto _LL58;
_LL57:({void*_tmp152[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp153="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,55));}),
_tag_dynforward(_tmp152,sizeof(void*),0));});_LL58: if((int)_tmp107 != 3)goto _LL5A;
_LL59:({void*_tmp154[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp155="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,57));}),
_tag_dynforward(_tmp154,sizeof(void*),0));});_LL5A: if(_tmp107 <= (void*)4)goto
_LL5C;if(*((int*)_tmp107)!= 19)goto _LL5C;_LL5B: goto _LL5D;_LL5C: if(_tmp107 <= (
void*)4)goto _LL5E;if(*((int*)_tmp107)!= 20)goto _LL5E;_LL5D: goto _LL5F;_LL5E: if(
_tmp107 <= (void*)4)goto _LL25;if(*((int*)_tmp107)!= 21)goto _LL25;_LL5F:({void*
_tmp156[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp157="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,49));}),
_tag_dynforward(_tmp156,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp158=t;struct Cyc_Absyn_ArrayInfo _tmp159;void*_tmp15A;struct Cyc_Absyn_Tqual
_tmp15B;_LL75: if(_tmp158 <= (void*)4)goto _LL77;if(*((int*)_tmp158)!= 7)goto _LL77;
_tmp159=((struct Cyc_Absyn_ArrayType_struct*)_tmp158)->f1;_tmp15A=(void*)_tmp159.elt_type;
_tmp15B=_tmp159.tq;_LL76: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp15A,(void*)
2,_tmp15B,Cyc_Absyn_false_conref),e);_LL77:;_LL78: return Cyc_Toc_cast_it(t,e);
_LL74:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp15C=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp15D;void*_tmp15E;struct Cyc_Absyn_AggrInfo _tmp15F;
union Cyc_Absyn_AggrInfoU_union _tmp160;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_TunionFieldInfo
_tmp162;union Cyc_Absyn_TunionFieldInfoU_union _tmp163;struct Cyc_Absyn_Tuniondecl*
_tmp164;struct Cyc_Absyn_Tunionfield*_tmp165;struct Cyc_List_List*_tmp166;_LL7A:
if((int)_tmp15C != 0)goto _LL7C;_LL7B: return 1;_LL7C: if(_tmp15C <= (void*)4)goto
_LL84;if(*((int*)_tmp15C)!= 1)goto _LL7E;_LL7D: return 0;_LL7E: if(*((int*)_tmp15C)
!= 5)goto _LL80;_LL7F: goto _LL81;_LL80: if(*((int*)_tmp15C)!= 12)goto _LL82;_LL81:
goto _LL83;_LL82: if(*((int*)_tmp15C)!= 13)goto _LL84;_LL83: goto _LL85;_LL84: if((int)
_tmp15C != 1)goto _LL86;_LL85: goto _LL87;_LL86: if(_tmp15C <= (void*)4)goto _LL9E;if(*((
int*)_tmp15C)!= 6)goto _LL88;_LL87: goto _LL89;_LL88: if(*((int*)_tmp15C)!= 8)goto
_LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)_tmp15C)!= 17)goto _LL8C;_LL8B: goto _LL8D;
_LL8C: if(*((int*)_tmp15C)!= 14)goto _LL8E;_LL8D: return 1;_LL8E: if(*((int*)_tmp15C)
!= 7)goto _LL90;_tmp15D=((struct Cyc_Absyn_ArrayType_struct*)_tmp15C)->f1;_tmp15E=(
void*)_tmp15D.elt_type;_LL8F: return Cyc_Toc_atomic_typ(_tmp15E);_LL90: if(*((int*)
_tmp15C)!= 10)goto _LL92;_tmp15F=((struct Cyc_Absyn_AggrType_struct*)_tmp15C)->f1;
_tmp160=_tmp15F.aggr_info;_LL91:{union Cyc_Absyn_AggrInfoU_union _tmp167=_tmp160;
_LLA1: if((_tmp167.UnknownAggr).tag != 0)goto _LLA3;_LLA2: return 0;_LLA3:;_LLA4: goto
_LLA0;_LLA0:;}{struct Cyc_Absyn_Aggrdecl*_tmp168=Cyc_Absyn_get_known_aggrdecl(
_tmp160);if(_tmp168->impl == 0)return 0;{struct Cyc_List_List*_tmp169=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp168->impl))->fields;for(0;_tmp169 != 0;_tmp169=_tmp169->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp169->hd)->type))
return 0;}}return 1;}_LL92: if(*((int*)_tmp15C)!= 11)goto _LL94;_tmp161=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp15C)->f2;_LL93: for(0;_tmp161 != 0;_tmp161=_tmp161->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp161->hd)->type))return 0;}return 1;_LL94:
if(*((int*)_tmp15C)!= 3)goto _LL96;_tmp162=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp15C)->f1;_tmp163=_tmp162.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp15C)->f1).field_info).KnownTunionfield).tag != 1)goto _LL96;_tmp164=(_tmp163.KnownTunionfield).f1;
_tmp165=(_tmp163.KnownTunionfield).f2;_LL95: _tmp166=_tmp165->typs;goto _LL97;
_LL96: if(*((int*)_tmp15C)!= 9)goto _LL98;_tmp166=((struct Cyc_Absyn_TupleType_struct*)
_tmp15C)->f1;_LL97: for(0;_tmp166 != 0;_tmp166=_tmp166->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp166->hd)).f2))return 0;}return 1;_LL98: if(*((int*)_tmp15C)!= 2)
goto _LL9A;_LL99: goto _LL9B;_LL9A: if(*((int*)_tmp15C)!= 4)goto _LL9C;_LL9B: goto
_LL9D;_LL9C: if(*((int*)_tmp15C)!= 15)goto _LL9E;_LL9D: return 0;_LL9E:;_LL9F:({
struct Cyc_String_pa_struct _tmp16C;_tmp16C.tag=0;_tmp16C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp16A[
1]={& _tmp16C};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp16B="atomic_typ:  bad type %s";_tag_dynforward(_tmp16B,sizeof(char),
_get_zero_arr_size(_tmp16B,25));}),_tag_dynforward(_tmp16A,sizeof(void*),1));}});
_LL79:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp16D=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp16E;void*_tmp16F;_LLA6: if(_tmp16D <= (void*)4)goto
_LLA8;if(*((int*)_tmp16D)!= 4)goto _LLA8;_tmp16E=((struct Cyc_Absyn_PointerType_struct*)
_tmp16D)->f1;_tmp16F=(void*)_tmp16E.elt_typ;_LLA7: {void*_tmp170=Cyc_Tcutil_compress(
_tmp16F);_LLAB: if((int)_tmp170 != 0)goto _LLAD;_LLAC: return 1;_LLAD:;_LLAE: return 0;
_LLAA:;}_LLA8:;_LLA9: return 0;_LLA5:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp171=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp172;union Cyc_Absyn_AggrInfoU_union _tmp173;struct Cyc_List_List*_tmp174;_LLB0:
if(_tmp171 <= (void*)4)goto _LLB4;if(*((int*)_tmp171)!= 10)goto _LLB2;_tmp172=((
struct Cyc_Absyn_AggrType_struct*)_tmp171)->f1;_tmp173=_tmp172.aggr_info;_LLB1: {
struct Cyc_Absyn_Aggrdecl*_tmp175=Cyc_Absyn_get_known_aggrdecl(_tmp173);if(
_tmp175->impl == 0)({void*_tmp176[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp177="is_poly_field: type missing fields";
_tag_dynforward(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,35));}),
_tag_dynforward(_tmp176,sizeof(void*),0));});_tmp174=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp175->impl))->fields;goto _LLB3;}_LLB2: if(*((int*)_tmp171)!= 11)
goto _LLB4;_tmp174=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp171)->f2;_LLB3: {
struct Cyc_Absyn_Aggrfield*_tmp178=Cyc_Absyn_lookup_field(_tmp174,f);if(_tmp178 == 
0)({struct Cyc_String_pa_struct _tmp17B;_tmp17B.tag=0;_tmp17B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp179[1]={& _tmp17B};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp17A="is_poly_field: bad field %s";_tag_dynforward(_tmp17A,sizeof(char),
_get_zero_arr_size(_tmp17A,28));}),_tag_dynforward(_tmp179,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp178->type);}_LLB4:;_LLB5:({struct Cyc_String_pa_struct
_tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp17C[1]={& _tmp17E};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp17D="is_poly_field: bad type %s";_tag_dynforward(_tmp17D,sizeof(char),
_get_zero_arr_size(_tmp17D,27));}),_tag_dynforward(_tmp17C,sizeof(void*),1));}});
_LLAF:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp17F=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp180;struct _dynforward_ptr*_tmp181;struct Cyc_Absyn_Exp*
_tmp182;struct _dynforward_ptr*_tmp183;_LLB7: if(*((int*)_tmp17F)!= 23)goto _LLB9;
_tmp180=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp17F)->f1;_tmp181=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp17F)->f2;_LLB8: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp180->topt))->v,_tmp181);_LLB9: if(*((int*)_tmp17F)!= 24)goto _LLBB;
_tmp182=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp17F)->f1;_tmp183=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp17F)->f2;_LLBA: {void*_tmp184=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp182->topt))->v);struct Cyc_Absyn_PtrInfo _tmp185;void*_tmp186;
_LLBE: if(_tmp184 <= (void*)4)goto _LLC0;if(*((int*)_tmp184)!= 4)goto _LLC0;_tmp185=((
struct Cyc_Absyn_PointerType_struct*)_tmp184)->f1;_tmp186=(void*)_tmp185.elt_typ;
_LLBF: return Cyc_Toc_is_poly_field(_tmp186,_tmp183);_LLC0:;_LLC1:({struct Cyc_String_pa_struct
_tmp189;_tmp189.tag=0;_tmp189.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp182->topt))->v));{
void*_tmp187[1]={& _tmp189};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp188="is_poly_project: bad type %s";
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size(_tmp188,29));}),
_tag_dynforward(_tmp187,sizeof(void*),1));}});_LLBD:;}_LLBB:;_LLBC: return 0;_LLB6:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp18A=_cycalloc(sizeof(*_tmp18A));
_tmp18A->hd=s;_tmp18A->tl=0;_tmp18A;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->hd=s;_tmp18B->tl=
0;_tmp18B;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp18C[2];_tmp18C[1]=s;_tmp18C[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp18C,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp18D[2];_tmp18D[1]=n;
_tmp18D[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp18D,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp18E[2];_tmp18E[1]=n;_tmp18E[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp18E,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp18F[3];_tmp18F[2]=n;_tmp18F[1]=
s;_tmp18F[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp18F,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=e;_tmp190->tl=
0;_tmp190;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp191=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp192;_LLC3: if(*((int*)
_tmp191)!= 0)goto _LLC5;_tmp192=((struct Cyc_Absyn_Const_e_struct*)_tmp191)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp191)->f1).String_c).tag != 5)goto _LLC5;
_LLC4: is_string=1;goto _LLC2;_LLC5:;_LLC6: goto _LLC2;_LLC2:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[0]=({
struct Cyc_Absyn_Var_d_struct _tmp195;_tmp195.tag=0;_tmp195.f1=vd;_tmp195;});
_tmp194;}),0);_tmp193->tl=Cyc_Toc_result_decls;_tmp193;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple6*_tmp196[3];_tmp196[2]=({struct _tuple6*_tmp199=_cycalloc(
sizeof(*_tmp199));_tmp199->f1=0;_tmp199->f2=xplussz;_tmp199;});_tmp196[1]=({
struct _tuple6*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->f1=0;_tmp198->f2=xexp;
_tmp198;});_tmp196[0]=({struct _tuple6*_tmp197=_cycalloc(sizeof(*_tmp197));
_tmp197->f1=0;_tmp197->f2=xexp;_tmp197;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp196,sizeof(struct _tuple6*),3));}),
0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*_tmp19A[2];_tmp19A[
1]=({struct _tuple6*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->f1=0;_tmp19C->f2=
xplussz;_tmp19C;});_tmp19A[0]=({struct _tuple6*_tmp19B=_cycalloc(sizeof(*_tmp19B));
_tmp19B->f1=0;_tmp19B->f2=xexp;_tmp19B;});((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp19A,sizeof(struct _tuple6*),2));}),
0);}return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dynforward_ptr*label;
struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dynforward_ptr**break_lab;struct _dynforward_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp19E;int _tmp19F;struct Cyc_Toc_Env*
_tmp19D=nv;_tmp19E=*_tmp19D;_tmp19F=_tmp19E.toplevel;return _tmp19F;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){struct
Cyc_Toc_Env _tmp1A1;struct Cyc_Dict_Dict _tmp1A2;struct Cyc_Toc_Env*_tmp1A0=nv;
_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp1A2,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*_tmp1A3=
_region_malloc(r,sizeof(*_tmp1A3));_tmp1A3->break_lab=(struct _dynforward_ptr**)0;
_tmp1A3->continue_lab=(struct _dynforward_ptr**)0;_tmp1A3->fallthru_info=(struct
Cyc_Toc_FallthruInfo*)0;_tmp1A3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp1A3->toplevel=(int)1;_tmp1A3;});}static struct Cyc_Toc_Env*
Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1A5;struct _dynforward_ptr**_tmp1A6;struct _dynforward_ptr**_tmp1A7;struct Cyc_Toc_FallthruInfo*
_tmp1A8;struct Cyc_Dict_Dict _tmp1A9;int _tmp1AA;struct Cyc_Toc_Env*_tmp1A4=e;
_tmp1A5=*_tmp1A4;_tmp1A6=_tmp1A5.break_lab;_tmp1A7=_tmp1A5.continue_lab;_tmp1A8=
_tmp1A5.fallthru_info;_tmp1A9=_tmp1A5.varmap;_tmp1AA=_tmp1A5.toplevel;return({
struct Cyc_Toc_Env*_tmp1AB=_region_malloc(r,sizeof(*_tmp1AB));_tmp1AB->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1A6);_tmp1AB->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1A7);_tmp1AB->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1A8;_tmp1AB->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1A9);_tmp1AB->toplevel=(int)_tmp1AA;_tmp1AB;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1AD;struct _dynforward_ptr**_tmp1AE;struct _dynforward_ptr**_tmp1AF;struct Cyc_Toc_FallthruInfo*
_tmp1B0;struct Cyc_Dict_Dict _tmp1B1;int _tmp1B2;struct Cyc_Toc_Env*_tmp1AC=e;
_tmp1AD=*_tmp1AC;_tmp1AE=_tmp1AD.break_lab;_tmp1AF=_tmp1AD.continue_lab;_tmp1B0=
_tmp1AD.fallthru_info;_tmp1B1=_tmp1AD.varmap;_tmp1B2=_tmp1AD.toplevel;return({
struct Cyc_Toc_Env*_tmp1B3=_region_malloc(r,sizeof(*_tmp1B3));_tmp1B3->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1AE);_tmp1B3->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1AF);_tmp1B3->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1B0;_tmp1B3->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1B1);_tmp1B3->toplevel=(int)0;_tmp1B3;});}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp1B5;struct
_dynforward_ptr**_tmp1B6;struct _dynforward_ptr**_tmp1B7;struct Cyc_Toc_FallthruInfo*
_tmp1B8;struct Cyc_Dict_Dict _tmp1B9;int _tmp1BA;struct Cyc_Toc_Env*_tmp1B4=e;
_tmp1B5=*_tmp1B4;_tmp1B6=_tmp1B5.break_lab;_tmp1B7=_tmp1B5.continue_lab;_tmp1B8=
_tmp1B5.fallthru_info;_tmp1B9=_tmp1B5.varmap;_tmp1BA=_tmp1B5.toplevel;return({
struct Cyc_Toc_Env*_tmp1BB=_region_malloc(r,sizeof(*_tmp1BB));_tmp1BB->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1B6);_tmp1BB->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp1B7);_tmp1BB->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1B8;_tmp1BB->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1B9);_tmp1BB->toplevel=(int)1;_tmp1BB;});}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp1BC=(*x).f1;_LLC8: if((_tmp1BC.Rel_n).tag != 1)
goto _LLCA;_LLC9:({struct Cyc_String_pa_struct _tmp1BF;_tmp1BF.tag=0;_tmp1BF.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp1BD[1]={& _tmp1BF};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1BE="Toc::add_varmap on Rel_n: %s\n";
_tag_dynforward(_tmp1BE,sizeof(char),_get_zero_arr_size(_tmp1BE,30));}),
_tag_dynforward(_tmp1BD,sizeof(void*),1));}});_LLCA:;_LLCB: goto _LLC7;_LLC7:;}{
struct Cyc_Toc_Env _tmp1C1;struct _dynforward_ptr**_tmp1C2;struct _dynforward_ptr**
_tmp1C3;struct Cyc_Toc_FallthruInfo*_tmp1C4;struct Cyc_Dict_Dict _tmp1C5;int _tmp1C6;
struct Cyc_Toc_Env*_tmp1C0=e;_tmp1C1=*_tmp1C0;_tmp1C2=_tmp1C1.break_lab;_tmp1C3=
_tmp1C1.continue_lab;_tmp1C4=_tmp1C1.fallthru_info;_tmp1C5=_tmp1C1.varmap;
_tmp1C6=_tmp1C1.toplevel;{struct Cyc_Dict_Dict _tmp1C7=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1C5),x,y);return({struct Cyc_Toc_Env*_tmp1C8=_region_malloc(r,sizeof(*
_tmp1C8));_tmp1C8->break_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp1C2);_tmp1C8->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp1C3);_tmp1C8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp1C4;_tmp1C8->varmap=(
struct Cyc_Dict_Dict)_tmp1C7;_tmp1C8->toplevel=(int)_tmp1C6;_tmp1C8;});}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp1CA;struct _dynforward_ptr**_tmp1CB;struct _dynforward_ptr**
_tmp1CC;struct Cyc_Toc_FallthruInfo*_tmp1CD;struct Cyc_Dict_Dict _tmp1CE;int _tmp1CF;
struct Cyc_Toc_Env*_tmp1C9=e;_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1CA.break_lab;_tmp1CC=
_tmp1CA.continue_lab;_tmp1CD=_tmp1CA.fallthru_info;_tmp1CE=_tmp1CA.varmap;
_tmp1CF=_tmp1CA.toplevel;return({struct Cyc_Toc_Env*_tmp1D0=_region_malloc(r,
sizeof(*_tmp1D0));_tmp1D0->break_lab=(struct _dynforward_ptr**)0;_tmp1D0->continue_lab=(
struct _dynforward_ptr**)0;_tmp1D0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1CD;_tmp1D0->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1CE);_tmp1D0->toplevel=(
int)_tmp1CF;_tmp1D0;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct
_dynforward_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp1D1=_region_malloc(r,sizeof(*_tmp1D1));_tmp1D1->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp1D1->tl=fallthru_vars;_tmp1D1;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp1D3;struct _dynforward_ptr**_tmp1D4;struct _dynforward_ptr**
_tmp1D5;struct Cyc_Toc_FallthruInfo*_tmp1D6;struct Cyc_Dict_Dict _tmp1D7;int _tmp1D8;
struct Cyc_Toc_Env*_tmp1D2=e;_tmp1D3=*_tmp1D2;_tmp1D4=_tmp1D3.break_lab;_tmp1D5=
_tmp1D3.continue_lab;_tmp1D6=_tmp1D3.fallthru_info;_tmp1D7=_tmp1D3.varmap;
_tmp1D8=_tmp1D3.toplevel;{struct Cyc_Toc_Env _tmp1DA;struct Cyc_Dict_Dict _tmp1DB;
struct Cyc_Toc_Env*_tmp1D9=next_case_env;_tmp1DA=*_tmp1D9;_tmp1DB=_tmp1DA.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1DE=
_region_malloc(r,sizeof(*_tmp1DE));_tmp1DE->label=fallthru_l;_tmp1DE->binders=
fallthru_vars;_tmp1DE->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1DB);_tmp1DE;});
return({struct Cyc_Toc_Env*_tmp1DC=_region_malloc(r,sizeof(*_tmp1DC));_tmp1DC->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp1DD=_region_malloc(r,
sizeof(*_tmp1DD));_tmp1DD[0]=break_l;_tmp1DD;});_tmp1DC->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp1D5);_tmp1DC->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp1DC->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1D7);_tmp1DC->toplevel=(
int)_tmp1D8;_tmp1DC;});}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct
Cyc_Toc_Env _tmp1E0;struct _dynforward_ptr**_tmp1E1;struct _dynforward_ptr**_tmp1E2;
struct Cyc_Toc_FallthruInfo*_tmp1E3;struct Cyc_Dict_Dict _tmp1E4;int _tmp1E5;struct
Cyc_Toc_Env*_tmp1DF=e;_tmp1E0=*_tmp1DF;_tmp1E1=_tmp1E0.break_lab;_tmp1E2=_tmp1E0.continue_lab;
_tmp1E3=_tmp1E0.fallthru_info;_tmp1E4=_tmp1E0.varmap;_tmp1E5=_tmp1E0.toplevel;
return({struct Cyc_Toc_Env*_tmp1E6=_region_malloc(r,sizeof(*_tmp1E6));_tmp1E6->break_lab=(
struct _dynforward_ptr**)({struct _dynforward_ptr**_tmp1E7=_region_malloc(r,
sizeof(*_tmp1E7));_tmp1E7[0]=break_l;_tmp1E7;});_tmp1E6->continue_lab=(struct
_dynforward_ptr**)((struct _dynforward_ptr**)_tmp1E2);_tmp1E6->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp1E6->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1E4);_tmp1E6->toplevel=(
int)_tmp1E5;_tmp1E6;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct
Cyc_Toc_Env _tmp1E9;struct _dynforward_ptr**_tmp1EA;struct _dynforward_ptr**_tmp1EB;
struct Cyc_Toc_FallthruInfo*_tmp1EC;struct Cyc_Dict_Dict _tmp1ED;int _tmp1EE;struct
Cyc_Toc_Env*_tmp1E8=e;_tmp1E9=*_tmp1E8;_tmp1EA=_tmp1E9.break_lab;_tmp1EB=_tmp1E9.continue_lab;
_tmp1EC=_tmp1E9.fallthru_info;_tmp1ED=_tmp1E9.varmap;_tmp1EE=_tmp1E9.toplevel;
return({struct Cyc_Toc_Env*_tmp1EF=_region_malloc(r,sizeof(*_tmp1EF));_tmp1EF->break_lab=(
struct _dynforward_ptr**)0;_tmp1EF->continue_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp1EB);_tmp1EF->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));_tmp1F0->label=
next_l;_tmp1F0->binders=0;_tmp1F0->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp1F0;});_tmp1EF->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1ED);_tmp1EF->toplevel=(
int)_tmp1EE;_tmp1EF;});}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp1F1=(void*)e->annot;
_LLCD: if(*((void**)_tmp1F1)!= Cyc_CfFlowInfo_UnknownZ)goto _LLCF;_LLCE: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLCF: if(*((void**)_tmp1F1)
!= Cyc_CfFlowInfo_NotZero)goto _LLD1;_LLD0: return 0;_LLD1: if(_tmp1F1 != Cyc_CfFlowInfo_IsZero)
goto _LLD3;_LLD2:({void*_tmp1F2[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp1F3="dereference of NULL pointer";
_tag_dynforward(_tmp1F3,sizeof(char),_get_zero_arr_size(_tmp1F3,28));}),
_tag_dynforward(_tmp1F2,sizeof(void*),0));});return 0;_LLD3: if(_tmp1F1 != Cyc_Absyn_EmptyAnnot)
goto _LLD5;_LLD4: return 0;_LLD5: if(*((void**)_tmp1F1)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD7;_LLD6:({void*_tmp1F4[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp1F5="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp1F5,sizeof(char),_get_zero_arr_size(_tmp1F5,46));}),
_tag_dynforward(_tmp1F4,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD7:;_LLD8:({void*_tmp1F6[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1F7="need_null_check";_tag_dynforward(_tmp1F7,sizeof(char),
_get_zero_arr_size(_tmp1F7,16));}),_tag_dynforward(_tmp1F6,sizeof(void*),0));});
_LLCC:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp1F8=(void*)e->annot;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List*_tmp1FA;
_LLDA: if(*((void**)_tmp1F8)!= Cyc_CfFlowInfo_UnknownZ)goto _LLDC;_tmp1F9=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp1F8)->f1;_LLDB: return _tmp1F9;_LLDC: if(*((
void**)_tmp1F8)!= Cyc_CfFlowInfo_NotZero)goto _LLDE;_tmp1FA=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp1F8)->f1;_LLDD: return _tmp1FA;_LLDE: if(_tmp1F8 != Cyc_CfFlowInfo_IsZero)goto
_LLE0;_LLDF:({void*_tmp1FB[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp1FC="dereference of NULL pointer";
_tag_dynforward(_tmp1FC,sizeof(char),_get_zero_arr_size(_tmp1FC,28));}),
_tag_dynforward(_tmp1FB,sizeof(void*),0));});return 0;_LLE0: if(*((void**)_tmp1F8)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(_tmp1F8 != Cyc_Absyn_EmptyAnnot)
goto _LLE4;_LLE3: return 0;_LLE4:;_LLE5:({void*_tmp1FD[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1FE="get_relns";_tag_dynforward(_tmp1FE,sizeof(char),_get_zero_arr_size(
_tmp1FE,10));}),_tag_dynforward(_tmp1FD,sizeof(void*),0));});_LLD9:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp1FF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp200;struct Cyc_Absyn_PtrAtts _tmp201;struct Cyc_Absyn_Conref*
_tmp202;struct Cyc_Absyn_Conref*_tmp203;struct Cyc_Absyn_ArrayInfo _tmp204;struct
Cyc_Absyn_Exp*_tmp205;_LLE7: if(_tmp1FF <= (void*)4)goto _LLEB;if(*((int*)_tmp1FF)
!= 4)goto _LLE9;_tmp200=((struct Cyc_Absyn_PointerType_struct*)_tmp1FF)->f1;
_tmp201=_tmp200.ptr_atts;_tmp202=_tmp201.bounds;_tmp203=_tmp201.zero_term;_LLE8: {
void*_tmp206=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp202);struct Cyc_Absyn_Exp*
_tmp207;_LLEE: if((int)_tmp206 != 0)goto _LLF0;_LLEF: return 0;_LLF0: if((int)_tmp206
!= 1)goto _LLF2;_LLF1: return 0;_LLF2: if(_tmp206 <= (void*)2)goto _LLF4;if(*((int*)
_tmp206)!= 0)goto _LLF4;_tmp207=((struct Cyc_Absyn_Upper_b_struct*)_tmp206)->f1;
_LLF3: {unsigned int _tmp209;int _tmp20A;struct _tuple5 _tmp208=Cyc_Evexp_eval_const_uint_exp(
_tmp207);_tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;return _tmp20A  && i <= _tmp209;}
_LLF4: if(_tmp206 <= (void*)2)goto _LLED;if(*((int*)_tmp206)!= 1)goto _LLED;_LLF5:({
void*_tmp20B[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp20C="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp20C,sizeof(char),_get_zero_arr_size(_tmp20C,30));}),
_tag_dynforward(_tmp20B,sizeof(void*),0));});_LLED:;}_LLE9: if(*((int*)_tmp1FF)!= 
7)goto _LLEB;_tmp204=((struct Cyc_Absyn_ArrayType_struct*)_tmp1FF)->f1;_tmp205=
_tmp204.num_elts;_LLEA: if(_tmp205 == 0)return 0;{unsigned int _tmp20E;int _tmp20F;
struct _tuple5 _tmp20D=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp205);
_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;return _tmp20F  && i <= _tmp20E;}_LLEB:;_LLEC:
return 0;_LLE6:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp210=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp210->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp211=_tmp210->rop;
struct Cyc_Absyn_Vardecl*_tmp212;struct Cyc_Absyn_Vardecl*_tmp213;_LLF7: if((
_tmp211.LessSize).tag != 2)goto _LLF9;_tmp212=(_tmp211.LessSize).f1;_LLF8: _tmp213=
_tmp212;goto _LLFA;_LLF9: if((_tmp211.LessEqSize).tag != 4)goto _LLFB;_tmp213=(
_tmp211.LessEqSize).f1;_LLFA: if(_tmp213 == v)return 1;else{goto _LLF6;}_LLFB:;_LLFC:
continue;_LLF6:;}}return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp214=(void*)e->r;
void*_tmp215;struct Cyc_Absyn_Vardecl*_tmp216;void*_tmp217;struct Cyc_Absyn_Vardecl*
_tmp218;void*_tmp219;struct Cyc_Absyn_Vardecl*_tmp21A;void*_tmp21B;struct Cyc_Absyn_Vardecl*
_tmp21C;void*_tmp21D;struct Cyc_List_List*_tmp21E;struct Cyc_List_List _tmp21F;
struct Cyc_Absyn_Exp*_tmp220;_LLFE: if(*((int*)_tmp214)!= 1)goto _LL100;_tmp215=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp214)->f2;if(_tmp215 <= (void*)1)goto
_LL100;if(*((int*)_tmp215)!= 4)goto _LL100;_tmp216=((struct Cyc_Absyn_Pat_b_struct*)
_tmp215)->f1;_LLFF: _tmp218=_tmp216;goto _LL101;_LL100: if(*((int*)_tmp214)!= 1)
goto _LL102;_tmp217=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp214)->f2;if(
_tmp217 <= (void*)1)goto _LL102;if(*((int*)_tmp217)!= 3)goto _LL102;_tmp218=((
struct Cyc_Absyn_Local_b_struct*)_tmp217)->f1;_LL101: _tmp21A=_tmp218;goto _LL103;
_LL102: if(*((int*)_tmp214)!= 1)goto _LL104;_tmp219=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp214)->f2;if(_tmp219 <= (void*)1)goto _LL104;if(*((int*)_tmp219)!= 0)goto _LL104;
_tmp21A=((struct Cyc_Absyn_Global_b_struct*)_tmp219)->f1;_LL103: _tmp21C=_tmp21A;
goto _LL105;_LL104: if(*((int*)_tmp214)!= 1)goto _LL106;_tmp21B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp214)->f2;if(_tmp21B <= (void*)1)goto _LL106;if(*((int*)_tmp21B)!= 2)goto _LL106;
_tmp21C=((struct Cyc_Absyn_Param_b_struct*)_tmp21B)->f1;_LL105: if(_tmp21C->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp21C))return 1;goto _LLFD;_LL106:
if(*((int*)_tmp214)!= 3)goto _LL108;_tmp21D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp214)->f1;if((int)_tmp21D != 19)goto _LL108;_tmp21E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp214)->f2;if(_tmp21E == 0)goto _LL108;_tmp21F=*_tmp21E;_tmp220=(struct Cyc_Absyn_Exp*)
_tmp21F.hd;_LL107:{void*_tmp221=(void*)_tmp220->r;void*_tmp222;struct Cyc_Absyn_Vardecl*
_tmp223;void*_tmp224;struct Cyc_Absyn_Vardecl*_tmp225;void*_tmp226;struct Cyc_Absyn_Vardecl*
_tmp227;void*_tmp228;struct Cyc_Absyn_Vardecl*_tmp229;_LL10B: if(*((int*)_tmp221)
!= 1)goto _LL10D;_tmp222=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp221)->f2;if(
_tmp222 <= (void*)1)goto _LL10D;if(*((int*)_tmp222)!= 4)goto _LL10D;_tmp223=((
struct Cyc_Absyn_Pat_b_struct*)_tmp222)->f1;_LL10C: _tmp225=_tmp223;goto _LL10E;
_LL10D: if(*((int*)_tmp221)!= 1)goto _LL10F;_tmp224=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp221)->f2;if(_tmp224 <= (void*)1)goto _LL10F;if(*((int*)_tmp224)!= 3)goto _LL10F;
_tmp225=((struct Cyc_Absyn_Local_b_struct*)_tmp224)->f1;_LL10E: _tmp227=_tmp225;
goto _LL110;_LL10F: if(*((int*)_tmp221)!= 1)goto _LL111;_tmp226=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp221)->f2;if(_tmp226 <= (void*)1)goto _LL111;if(*((int*)_tmp226)!= 0)goto _LL111;
_tmp227=((struct Cyc_Absyn_Global_b_struct*)_tmp226)->f1;_LL110: _tmp229=_tmp227;
goto _LL112;_LL111: if(*((int*)_tmp221)!= 1)goto _LL113;_tmp228=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp221)->f2;if(_tmp228 <= (void*)1)goto _LL113;if(*((int*)_tmp228)!= 2)goto _LL113;
_tmp229=((struct Cyc_Absyn_Param_b_struct*)_tmp228)->f1;_LL112: return _tmp229 == v;
_LL113:;_LL114: goto _LL10A;_LL10A:;}goto _LLFD;_LL108:;_LL109: goto _LLFD;_LLFD:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp22A=(void*)a->r;void*_tmp22B;struct Cyc_Absyn_Vardecl*
_tmp22C;void*_tmp22D;struct Cyc_Absyn_Vardecl*_tmp22E;void*_tmp22F;struct Cyc_Absyn_Vardecl*
_tmp230;void*_tmp231;struct Cyc_Absyn_Vardecl*_tmp232;_LL116: if(*((int*)_tmp22A)
!= 1)goto _LL118;_tmp22B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp22A)->f2;if(
_tmp22B <= (void*)1)goto _LL118;if(*((int*)_tmp22B)!= 4)goto _LL118;_tmp22C=((
struct Cyc_Absyn_Pat_b_struct*)_tmp22B)->f1;_LL117: _tmp22E=_tmp22C;goto _LL119;
_LL118: if(*((int*)_tmp22A)!= 1)goto _LL11A;_tmp22D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp22A)->f2;if(_tmp22D <= (void*)1)goto _LL11A;if(*((int*)_tmp22D)!= 3)goto _LL11A;
_tmp22E=((struct Cyc_Absyn_Local_b_struct*)_tmp22D)->f1;_LL119: _tmp230=_tmp22E;
goto _LL11B;_LL11A: if(*((int*)_tmp22A)!= 1)goto _LL11C;_tmp22F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp22A)->f2;if(_tmp22F <= (void*)1)goto _LL11C;if(*((int*)_tmp22F)!= 0)goto _LL11C;
_tmp230=((struct Cyc_Absyn_Global_b_struct*)_tmp22F)->f1;_LL11B: _tmp232=_tmp230;
goto _LL11D;_LL11C: if(*((int*)_tmp22A)!= 1)goto _LL11E;_tmp231=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp22A)->f2;if(_tmp231 <= (void*)1)goto _LL11E;if(*((int*)_tmp231)!= 2)goto _LL11E;
_tmp232=((struct Cyc_Absyn_Param_b_struct*)_tmp231)->f1;_LL11D: if(_tmp232->escapes)
return 0;inner_loop: {void*_tmp233=(void*)i->r;void*_tmp234;struct Cyc_Absyn_Exp*
_tmp235;union Cyc_Absyn_Cnst_union _tmp236;void*_tmp237;int _tmp238;union Cyc_Absyn_Cnst_union
_tmp239;void*_tmp23A;int _tmp23B;union Cyc_Absyn_Cnst_union _tmp23C;void*_tmp23D;
int _tmp23E;void*_tmp23F;struct Cyc_List_List*_tmp240;struct Cyc_List_List _tmp241;
struct Cyc_Absyn_Exp*_tmp242;struct Cyc_List_List*_tmp243;struct Cyc_List_List
_tmp244;struct Cyc_Absyn_Exp*_tmp245;void*_tmp246;struct Cyc_Absyn_Vardecl*_tmp247;
void*_tmp248;struct Cyc_Absyn_Vardecl*_tmp249;void*_tmp24A;struct Cyc_Absyn_Vardecl*
_tmp24B;void*_tmp24C;struct Cyc_Absyn_Vardecl*_tmp24D;_LL121: if(*((int*)_tmp233)
!= 15)goto _LL123;_tmp234=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp233)->f1;
_tmp235=((struct Cyc_Absyn_Cast_e_struct*)_tmp233)->f2;_LL122: i=_tmp235;goto
inner_loop;_LL123: if(*((int*)_tmp233)!= 0)goto _LL125;_tmp236=((struct Cyc_Absyn_Const_e_struct*)
_tmp233)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp233)->f1).Int_c).tag != 2)
goto _LL125;_tmp237=(_tmp236.Int_c).f1;if((int)_tmp237 != 2)goto _LL125;_tmp238=(
_tmp236.Int_c).f2;_LL124: _tmp23B=_tmp238;goto _LL126;_LL125: if(*((int*)_tmp233)!= 
0)goto _LL127;_tmp239=((struct Cyc_Absyn_Const_e_struct*)_tmp233)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp233)->f1).Int_c).tag != 2)goto _LL127;_tmp23A=(
_tmp239.Int_c).f1;if((int)_tmp23A != 0)goto _LL127;_tmp23B=(_tmp239.Int_c).f2;
_LL126: return _tmp23B >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp23B + 1),(
void*)_tmp232->type);_LL127: if(*((int*)_tmp233)!= 0)goto _LL129;_tmp23C=((struct
Cyc_Absyn_Const_e_struct*)_tmp233)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp233)->f1).Int_c).tag != 2)goto _LL129;_tmp23D=(_tmp23C.Int_c).f1;if((int)
_tmp23D != 1)goto _LL129;_tmp23E=(_tmp23C.Int_c).f2;_LL128: return Cyc_Toc_check_const_array((
unsigned int)(_tmp23E + 1),(void*)_tmp232->type);_LL129: if(*((int*)_tmp233)!= 3)
goto _LL12B;_tmp23F=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp233)->f1;if((
int)_tmp23F != 4)goto _LL12B;_tmp240=((struct Cyc_Absyn_Primop_e_struct*)_tmp233)->f2;
if(_tmp240 == 0)goto _LL12B;_tmp241=*_tmp240;_tmp242=(struct Cyc_Absyn_Exp*)_tmp241.hd;
_tmp243=_tmp241.tl;if(_tmp243 == 0)goto _LL12B;_tmp244=*_tmp243;_tmp245=(struct Cyc_Absyn_Exp*)
_tmp244.hd;_LL12A: return Cyc_Toc_check_leq_size(relns,_tmp232,_tmp245);_LL12B: if(*((
int*)_tmp233)!= 1)goto _LL12D;_tmp246=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp233)->f2;if(_tmp246 <= (void*)1)goto _LL12D;if(*((int*)_tmp246)!= 4)goto _LL12D;
_tmp247=((struct Cyc_Absyn_Pat_b_struct*)_tmp246)->f1;_LL12C: _tmp249=_tmp247;goto
_LL12E;_LL12D: if(*((int*)_tmp233)!= 1)goto _LL12F;_tmp248=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp233)->f2;if(_tmp248 <= (void*)1)goto _LL12F;if(*((int*)_tmp248)!= 3)goto _LL12F;
_tmp249=((struct Cyc_Absyn_Local_b_struct*)_tmp248)->f1;_LL12E: _tmp24B=_tmp249;
goto _LL130;_LL12F: if(*((int*)_tmp233)!= 1)goto _LL131;_tmp24A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp233)->f2;if(_tmp24A <= (void*)1)goto _LL131;if(*((int*)_tmp24A)!= 0)goto _LL131;
_tmp24B=((struct Cyc_Absyn_Global_b_struct*)_tmp24A)->f1;_LL130: _tmp24D=_tmp24B;
goto _LL132;_LL131: if(*((int*)_tmp233)!= 1)goto _LL133;_tmp24C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp233)->f2;if(_tmp24C <= (void*)1)goto _LL133;if(*((int*)_tmp24C)!= 2)goto _LL133;
_tmp24D=((struct Cyc_Absyn_Param_b_struct*)_tmp24C)->f1;_LL132: if(_tmp24D->escapes)
return 0;{struct Cyc_List_List*_tmp24E=relns;for(0;_tmp24E != 0;_tmp24E=_tmp24E->tl){
struct Cyc_CfFlowInfo_Reln*_tmp24F=(struct Cyc_CfFlowInfo_Reln*)_tmp24E->hd;if(
_tmp24F->vd == _tmp24D){union Cyc_CfFlowInfo_RelnOp_union _tmp250=_tmp24F->rop;
struct Cyc_Absyn_Vardecl*_tmp251;struct Cyc_Absyn_Vardecl*_tmp252;unsigned int
_tmp253;_LL136: if((_tmp250.LessSize).tag != 2)goto _LL138;_tmp251=(_tmp250.LessSize).f1;
_LL137: if(_tmp232 == _tmp251)return 1;else{goto _LL135;}_LL138: if((_tmp250.LessVar).tag
!= 1)goto _LL13A;_tmp252=(_tmp250.LessVar).f1;_LL139:{struct Cyc_List_List*_tmp254=
relns;for(0;_tmp254 != 0;_tmp254=_tmp254->tl){struct Cyc_CfFlowInfo_Reln*_tmp255=(
struct Cyc_CfFlowInfo_Reln*)_tmp254->hd;if(_tmp255->vd == _tmp252){union Cyc_CfFlowInfo_RelnOp_union
_tmp256=_tmp255->rop;struct Cyc_Absyn_Vardecl*_tmp257;struct Cyc_Absyn_Vardecl*
_tmp258;unsigned int _tmp259;struct Cyc_Absyn_Vardecl*_tmp25A;_LL13F: if((_tmp256.LessEqSize).tag
!= 4)goto _LL141;_tmp257=(_tmp256.LessEqSize).f1;_LL140: _tmp258=_tmp257;goto
_LL142;_LL141: if((_tmp256.LessSize).tag != 2)goto _LL143;_tmp258=(_tmp256.LessSize).f1;
_LL142: if(_tmp232 == _tmp258)return 1;goto _LL13E;_LL143: if((_tmp256.EqualConst).tag
!= 0)goto _LL145;_tmp259=(_tmp256.EqualConst).f1;_LL144: return Cyc_Toc_check_const_array(
_tmp259,(void*)_tmp232->type);_LL145: if((_tmp256.LessVar).tag != 1)goto _LL147;
_tmp25A=(_tmp256.LessVar).f1;_LL146: if(Cyc_Toc_check_leq_size_var(relns,_tmp232,
_tmp25A))return 1;goto _LL13E;_LL147:;_LL148: goto _LL13E;_LL13E:;}}}goto _LL135;
_LL13A: if((_tmp250.LessConst).tag != 3)goto _LL13C;_tmp253=(_tmp250.LessConst).f1;
_LL13B: return Cyc_Toc_check_const_array(_tmp253,(void*)_tmp232->type);_LL13C:;
_LL13D: goto _LL135;_LL135:;}}}goto _LL120;_LL133:;_LL134: goto _LL120;_LL120:;}goto
_LL115;_LL11E:;_LL11F: goto _LL115;_LL115:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp25B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp25C="Missing type in primop ";_tag_dynforward(_tmp25C,sizeof(char),
_get_zero_arr_size(_tmp25C,24));}),_tag_dynforward(_tmp25B,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp25D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp25E="Missing type in primop ";_tag_dynforward(_tmp25E,sizeof(char),
_get_zero_arr_size(_tmp25E,24));}),_tag_dynforward(_tmp25D,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp25F=_cycalloc(
sizeof(*_tmp25F));_tmp25F->f1=Cyc_Toc_mt_tq;_tmp25F->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp25F;});}static struct _tuple6*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple6*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->f1=0;
_tmp260->f2=e;_tmp260;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp261=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp261):
Cyc_Toc_malloc_ptr(_tmp261));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp261);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp262=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp262 != 0;_tmp262=_tmp262->tl){struct _tuple6 _tmp264;struct Cyc_List_List*
_tmp265;struct Cyc_Absyn_Exp*_tmp266;struct _tuple6*_tmp263=(struct _tuple6*)
_tmp262->hd;_tmp264=*_tmp263;_tmp265=_tmp264.f1;_tmp266=_tmp264.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp265 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp265->tl != 0)({void*_tmp267[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp268="multiple designators in array";
_tag_dynforward(_tmp268,sizeof(char),_get_zero_arr_size(_tmp268,30));}),
_tag_dynforward(_tmp267,sizeof(void*),0));});{void*_tmp269=(void*)_tmp265->hd;
void*_tmp26A=_tmp269;struct Cyc_Absyn_Exp*_tmp26B;_LL14A: if(*((int*)_tmp26A)!= 0)
goto _LL14C;_tmp26B=((struct Cyc_Absyn_ArrayElement_struct*)_tmp26A)->f1;_LL14B:
Cyc_Toc_exp_to_c(nv,_tmp26B);e_index=_tmp26B;goto _LL149;_LL14C: if(*((int*)
_tmp26A)!= 1)goto _LL149;_LL14D:({void*_tmp26C[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp26D="field name designators in array";
_tag_dynforward(_tmp26D,sizeof(char),_get_zero_arr_size(_tmp26D,32));}),
_tag_dynforward(_tmp26C,sizeof(void*),0));});_LL149:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp26E=(void*)_tmp266->r;struct Cyc_List_List*
_tmp26F;struct Cyc_Absyn_Vardecl*_tmp270;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*
_tmp272;int _tmp273;void*_tmp274;struct Cyc_List_List*_tmp275;_LL14F: if(*((int*)
_tmp26E)!= 28)goto _LL151;_tmp26F=((struct Cyc_Absyn_Array_e_struct*)_tmp26E)->f1;
_LL150: s=Cyc_Toc_init_array(nv,lval,_tmp26F,s);goto _LL14E;_LL151: if(*((int*)
_tmp26E)!= 29)goto _LL153;_tmp270=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp26E)->f1;_tmp271=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp26E)->f2;
_tmp272=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp26E)->f3;_tmp273=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp26E)->f4;_LL152: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp270,_tmp271,_tmp272,_tmp273,s,0);goto _LL14E;_LL153: if(*((int*)
_tmp26E)!= 31)goto _LL155;_tmp274=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp26E)->f1;_tmp275=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp26E)->f2;_LL154:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp274,_tmp275,s);goto _LL14E;_LL155:;_LL156:
Cyc_Toc_exp_to_c(nv,_tmp266);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp266,0),s,0);goto _LL14E;_LL14E:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp276=vd->name;
void*_tmp277=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp278=
_new_region("r2");struct _RegionHandle*r2=& _tmp278;_push_region(r2);{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(r2,nv,_tmp276,Cyc_Absyn_varb_exp(_tmp276,(void*)({struct
Cyc_Absyn_Local_b_struct*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282[0]=({struct
Cyc_Absyn_Local_b_struct _tmp283;_tmp283.tag=3;_tmp283.f1=vd;_tmp283;});_tmp282;}),
0));struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp276,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp276,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp276,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp276,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp279=(void*)e2->r;struct Cyc_List_List*
_tmp27A;struct Cyc_Absyn_Vardecl*_tmp27B;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*
_tmp27D;int _tmp27E;void*_tmp27F;struct Cyc_List_List*_tmp280;_LL158: if(*((int*)
_tmp279)!= 28)goto _LL15A;_tmp27A=((struct Cyc_Absyn_Array_e_struct*)_tmp279)->f1;
_LL159: body=Cyc_Toc_init_array(nv2,lval,_tmp27A,Cyc_Toc_skip_stmt_dl());goto
_LL157;_LL15A: if(*((int*)_tmp279)!= 29)goto _LL15C;_tmp27B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp279)->f1;_tmp27C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp279)->f2;
_tmp27D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp279)->f3;_tmp27E=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp279)->f4;_LL15B: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp27B,_tmp27C,_tmp27D,_tmp27E,Cyc_Toc_skip_stmt_dl(),0);goto _LL157;
_LL15C: if(*((int*)_tmp279)!= 31)goto _LL15E;_tmp27F=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp279)->f1;_tmp280=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp279)->f2;_LL15D:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp27F,_tmp280,Cyc_Toc_skip_stmt_dl());
goto _LL157;_LL15E:;_LL15F: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL157;_LL157:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp277,Cyc_Absyn_uint_exp(0,0)),
0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{struct Cyc_Absyn_Stmt*
_tmp281=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp276,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);_npop_handler(0);return _tmp281;}}};_pop_region(r2);}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*
struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){{struct Cyc_List_List*
_tmp284=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);
for(0;_tmp284 != 0;_tmp284=_tmp284->tl){struct _tuple6 _tmp286;struct Cyc_List_List*
_tmp287;struct Cyc_Absyn_Exp*_tmp288;struct _tuple6*_tmp285=(struct _tuple6*)
_tmp284->hd;_tmp286=*_tmp285;_tmp287=_tmp286.f1;_tmp288=_tmp286.f2;if(_tmp287 == 
0)({void*_tmp289[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp28A="empty designator list";_tag_dynforward(
_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,22));}),_tag_dynforward(_tmp289,
sizeof(void*),0));});if(_tmp287->tl != 0)({void*_tmp28B[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp28C="too many designators in anonymous struct";_tag_dynforward(_tmp28C,
sizeof(char),_get_zero_arr_size(_tmp28C,41));}),_tag_dynforward(_tmp28B,sizeof(
void*),0));});{void*_tmp28D=(void*)_tmp287->hd;struct _dynforward_ptr*_tmp28E;
_LL161: if(*((int*)_tmp28D)!= 1)goto _LL163;_tmp28E=((struct Cyc_Absyn_FieldName_struct*)
_tmp28D)->f1;_LL162: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp28E,0);{void*_tmp28F=(void*)_tmp288->r;struct Cyc_List_List*_tmp290;struct Cyc_Absyn_Vardecl*
_tmp291;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp293;int _tmp294;void*
_tmp295;struct Cyc_List_List*_tmp296;_LL166: if(*((int*)_tmp28F)!= 28)goto _LL168;
_tmp290=((struct Cyc_Absyn_Array_e_struct*)_tmp28F)->f1;_LL167: s=Cyc_Toc_init_array(
nv,lval,_tmp290,s);goto _LL165;_LL168: if(*((int*)_tmp28F)!= 29)goto _LL16A;_tmp291=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp28F)->f1;_tmp292=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp28F)->f2;_tmp293=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp28F)->f3;
_tmp294=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp28F)->f4;_LL169: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp291,_tmp292,_tmp293,_tmp294,s,0);goto _LL165;_LL16A: if(*((int*)
_tmp28F)!= 31)goto _LL16C;_tmp295=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp28F)->f1;_tmp296=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp28F)->f2;_LL16B:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp295,_tmp296,s);goto _LL165;_LL16C:;_LL16D:
Cyc_Toc_exp_to_c(nv,_tmp288);if(Cyc_Toc_is_poly_field(struct_type,_tmp28E))
_tmp288=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp288);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp288,0),0),s,0);goto _LL165;
_LL165:;}goto _LL160;}_LL163:;_LL164:({void*_tmp297[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp298="array designator in struct";_tag_dynforward(_tmp298,sizeof(char),
_get_zero_arr_size(_tmp298,27));}),_tag_dynforward(_tmp297,sizeof(void*),0));});
_LL160:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp299=_new_region("r");struct _RegionHandle*r=& _tmp299;
_push_region(r);{struct Cyc_List_List*_tmp29A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _dynforward_ptr*_tmp29B=Cyc_Toc_add_tuple_type(
_tmp29A);struct _tuple1*_tmp29C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp29D=
Cyc_Absyn_var_exp(_tmp29C,0);struct Cyc_Absyn_Stmt*_tmp29E=Cyc_Absyn_exp_stmt(
_tmp29D,0);struct Cyc_Absyn_Exp*(*_tmp29F)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp2A0=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2A0);for(0;_tmp2A0 != 0;(_tmp2A0=
_tmp2A0->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp2A0->hd;struct
Cyc_Absyn_Exp*lval=_tmp29F(_tmp29D,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp2A1=(void*)e->r;struct Cyc_List_List*_tmp2A2;struct Cyc_Absyn_Vardecl*
_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A5;int _tmp2A6;
_LL16F: if(*((int*)_tmp2A1)!= 28)goto _LL171;_tmp2A2=((struct Cyc_Absyn_Array_e_struct*)
_tmp2A1)->f1;_LL170: _tmp29E=Cyc_Toc_init_array(nv,lval,_tmp2A2,_tmp29E);goto
_LL16E;_LL171: if(*((int*)_tmp2A1)!= 29)goto _LL173;_tmp2A3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2A1)->f1;_tmp2A4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A1)->f2;
_tmp2A5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A1)->f3;_tmp2A6=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2A1)->f4;_LL172: _tmp29E=Cyc_Toc_init_comprehension(
nv,lval,_tmp2A3,_tmp2A4,_tmp2A5,_tmp2A6,_tmp29E,0);goto _LL16E;_LL173:;_LL174: Cyc_Toc_exp_to_c(
nv,e);_tmp29E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp29F(
_tmp29D,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp29E,0);goto _LL16E;_LL16E:;}}}{
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Toc_make_struct(nv,_tmp29C,Cyc_Absyn_strct(
_tmp29B),_tmp29E,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp2A7;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp2A8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2A9=Cyc_Absyn_var_exp(_tmp2A8,0);struct Cyc_Absyn_Stmt*
_tmp2AA=Cyc_Absyn_exp_stmt(_tmp2A9,0);struct Cyc_Absyn_Exp*(*_tmp2AB)(struct Cyc_Absyn_Exp*,
struct _dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp2AC=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp2AD;union
Cyc_Absyn_AggrInfoU_union _tmp2AE;_LL176: if(_tmp2AC <= (void*)4)goto _LL178;if(*((
int*)_tmp2AC)!= 10)goto _LL178;_tmp2AD=((struct Cyc_Absyn_AggrType_struct*)_tmp2AC)->f1;
_tmp2AE=_tmp2AD.aggr_info;_LL177: ad=Cyc_Absyn_get_known_aggrdecl(_tmp2AE);goto
_LL175;_LL178:;_LL179:({void*_tmp2AF[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2B0="init_struct: bad struct type";
_tag_dynforward(_tmp2B0,sizeof(char),_get_zero_arr_size(_tmp2B0,29));}),
_tag_dynforward(_tmp2AF,sizeof(void*),0));});_LL175:;}{struct _RegionHandle
_tmp2B1=_new_region("r");struct _RegionHandle*r=& _tmp2B1;_push_region(r);{struct
Cyc_List_List*_tmp2B2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,dles);for(0;_tmp2B2 != 0;_tmp2B2=_tmp2B2->tl){struct _tuple6
_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;struct _tuple6*
_tmp2B3=(struct _tuple6*)_tmp2B2->hd;_tmp2B4=*_tmp2B3;_tmp2B5=_tmp2B4.f1;_tmp2B6=
_tmp2B4.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B6->topt))->v);if(_tmp2B5 == 0)({void*_tmp2B7[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2B8="empty designator list";_tag_dynforward(_tmp2B8,sizeof(char),
_get_zero_arr_size(_tmp2B8,22));}),_tag_dynforward(_tmp2B7,sizeof(void*),0));});
if(_tmp2B5->tl != 0){struct _tuple1*_tmp2B9=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp2BA=Cyc_Absyn_var_exp(_tmp2B9,0);for(0;_tmp2B5 != 0;_tmp2B5=_tmp2B5->tl){void*
_tmp2BB=(void*)_tmp2B5->hd;struct _dynforward_ptr*_tmp2BC;_LL17B: if(*((int*)
_tmp2BB)!= 1)goto _LL17D;_tmp2BC=((struct Cyc_Absyn_FieldName_struct*)_tmp2BB)->f1;
_LL17C: if(Cyc_Toc_is_poly_field(struct_type,_tmp2BC))_tmp2BA=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2BA);_tmp2AA=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2AB(_tmp2A9,_tmp2BC,0),_tmp2BA,0),0),_tmp2AA,0);goto _LL17A;_LL17D:;_LL17E:({
void*_tmp2BD[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp2BE="array designator in struct";
_tag_dynforward(_tmp2BE,sizeof(char),_get_zero_arr_size(_tmp2BE,27));}),
_tag_dynforward(_tmp2BD,sizeof(void*),0));});_LL17A:;}Cyc_Toc_exp_to_c(nv,
_tmp2B6);_tmp2AA=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2BA,_tmp2B6,0),0),_tmp2AA,0);}else{void*_tmp2BF=(void*)_tmp2B5->hd;struct
_dynforward_ptr*_tmp2C0;_LL180: if(*((int*)_tmp2BF)!= 1)goto _LL182;_tmp2C0=((
struct Cyc_Absyn_FieldName_struct*)_tmp2BF)->f1;_LL181: {struct Cyc_Absyn_Exp*lval=
_tmp2AB(_tmp2A9,_tmp2C0,0);{void*_tmp2C1=(void*)_tmp2B6->r;struct Cyc_List_List*
_tmp2C2;struct Cyc_Absyn_Vardecl*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*
_tmp2C5;int _tmp2C6;void*_tmp2C7;struct Cyc_List_List*_tmp2C8;_LL185: if(*((int*)
_tmp2C1)!= 28)goto _LL187;_tmp2C2=((struct Cyc_Absyn_Array_e_struct*)_tmp2C1)->f1;
_LL186: _tmp2AA=Cyc_Toc_init_array(nv,lval,_tmp2C2,_tmp2AA);goto _LL184;_LL187: if(*((
int*)_tmp2C1)!= 29)goto _LL189;_tmp2C3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2C1)->f1;_tmp2C4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2C1)->f2;
_tmp2C5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2C1)->f3;_tmp2C6=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2C1)->f4;_LL188: _tmp2AA=Cyc_Toc_init_comprehension(
nv,lval,_tmp2C3,_tmp2C4,_tmp2C5,_tmp2C6,_tmp2AA,0);goto _LL184;_LL189: if(*((int*)
_tmp2C1)!= 31)goto _LL18B;_tmp2C7=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2C1)->f1;_tmp2C8=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2C1)->f2;_LL18A:
_tmp2AA=Cyc_Toc_init_anon_struct(nv,lval,_tmp2C7,_tmp2C8,_tmp2AA);goto _LL184;
_LL18B:;_LL18C: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2B6->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp2B6);{struct Cyc_Absyn_Aggrfield*
_tmp2C9=Cyc_Absyn_lookup_decl_field(ad,_tmp2C0);if(Cyc_Toc_is_poly_field(
struct_type,_tmp2C0) && !was_ptr_type)_tmp2B6=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp2B6);if(has_exists)_tmp2B6=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp2C9))->type,_tmp2B6);_tmp2AA=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp2B6,0),0),_tmp2AA,0);goto _LL184;}}_LL184:;}goto
_LL17F;}_LL182:;_LL183:({void*_tmp2CA[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2CB="array designator in struct";
_tag_dynforward(_tmp2CB,sizeof(char),_get_zero_arr_size(_tmp2CB,27));}),
_tag_dynforward(_tmp2CA,sizeof(void*),0));});_LL17F:;}}}{struct Cyc_Absyn_Exp*
_tmp2CC=Cyc_Toc_make_struct(nv,_tmp2A8,Cyc_Absyn_strctq(tdn),_tmp2AA,pointer,
rgnopt,is_atomic);_npop_handler(0);return _tmp2CC;};_pop_region(r);}}struct
_tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple8*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));
_tmp2CD[0]=({struct Cyc_Absyn_Increment_e_struct _tmp2CE;_tmp2CE.tag=5;_tmp2CE.f1=
e1;_tmp2CE.f2=(void*)incr;_tmp2CE;});_tmp2CD;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp2CF=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp2D0;void*_tmp2D1;struct Cyc_Absyn_Exp*
_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;struct _dynforward_ptr*_tmp2D4;_LL18E: if(*((
int*)_tmp2CF)!= 37)goto _LL190;_tmp2D0=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp2CF)->f1;_LL18F: Cyc_Toc_lvalue_assign_stmt(_tmp2D0,fs,f,f_env);goto _LL18D;
_LL190: if(*((int*)_tmp2CF)!= 15)goto _LL192;_tmp2D1=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2CF)->f1;_tmp2D2=((struct Cyc_Absyn_Cast_e_struct*)_tmp2CF)->f2;_LL191: Cyc_Toc_lvalue_assign(
_tmp2D2,fs,f,f_env);goto _LL18D;_LL192: if(*((int*)_tmp2CF)!= 23)goto _LL194;
_tmp2D3=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2CF)->f1;_tmp2D4=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2CF)->f2;_LL193:(void*)(e1->r=(void*)((void*)_tmp2D3->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));
_tmp2D5->hd=_tmp2D4;_tmp2D5->tl=fs;_tmp2D5;}),f,f_env);goto _LL18D;_LL194:;_LL195: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL18D;}_LL18D:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp2D6=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Absyn_Decl*_tmp2D8;struct Cyc_Absyn_Stmt*
_tmp2D9;struct Cyc_Absyn_Stmt*_tmp2DA;_LL197: if(_tmp2D6 <= (void*)1)goto _LL19D;if(*((
int*)_tmp2D6)!= 0)goto _LL199;_tmp2D7=((struct Cyc_Absyn_Exp_s_struct*)_tmp2D6)->f1;
_LL198: Cyc_Toc_lvalue_assign(_tmp2D7,fs,f,f_env);goto _LL196;_LL199: if(*((int*)
_tmp2D6)!= 11)goto _LL19B;_tmp2D8=((struct Cyc_Absyn_Decl_s_struct*)_tmp2D6)->f1;
_tmp2D9=((struct Cyc_Absyn_Decl_s_struct*)_tmp2D6)->f2;_LL19A: Cyc_Toc_lvalue_assign_stmt(
_tmp2D9,fs,f,f_env);goto _LL196;_LL19B: if(*((int*)_tmp2D6)!= 1)goto _LL19D;_tmp2DA=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2D6)->f2;_LL19C: Cyc_Toc_lvalue_assign_stmt(
_tmp2DA,fs,f,f_env);goto _LL196;_LL19D:;_LL19E:({struct Cyc_String_pa_struct
_tmp2DD;_tmp2DD.tag=0;_tmp2DD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_stmt2string(s));{void*_tmp2DB[1]={& _tmp2DD};Cyc_Toc_toc_impos(({
const char*_tmp2DC="lvalue_assign_stmt: %s";_tag_dynforward(_tmp2DC,sizeof(char),
_get_zero_arr_size(_tmp2DC,23));}),_tag_dynforward(_tmp2DB,sizeof(void*),1));}});
_LL196:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmp2DE=
_region_malloc(r2,sizeof(*_tmp2DE));_tmp2DE->hd=(void*)f((void*)x->hd,env);
_tmp2DE->tl=0;_tmp2DE;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp2DF=_region_malloc(r2,sizeof(*
_tmp2DF));_tmp2DF->hd=(void*)f((void*)x->hd,env);_tmp2DF->tl=0;_tmp2DF;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple6*_tmp2E0=_cycalloc(sizeof(*_tmp2E0));_tmp2E0->f1=0;
_tmp2E0->f2=e;_tmp2E0;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp2E1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2E2;_LL1A0: if(
_tmp2E1 <= (void*)4)goto _LL1A2;if(*((int*)_tmp2E1)!= 4)goto _LL1A2;_tmp2E2=((
struct Cyc_Absyn_PointerType_struct*)_tmp2E1)->f1;_LL1A1: return _tmp2E2;_LL1A2:;
_LL1A3:({void*_tmp2E3[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E4="get_ptr_typ: not a pointer!";
_tag_dynforward(_tmp2E4,sizeof(char),_get_zero_arr_size(_tmp2E4,28));}),
_tag_dynforward(_tmp2E3,sizeof(void*),0));});_LL19F:;}static int Cyc_Toc_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*
_tmp2E5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2E6;void*_tmp2E7;
struct Cyc_Absyn_PtrAtts _tmp2E8;struct Cyc_Absyn_Conref*_tmp2E9;struct Cyc_Absyn_Conref*
_tmp2EA;struct Cyc_Absyn_ArrayInfo _tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Tqual
_tmp2ED;struct Cyc_Absyn_Exp*_tmp2EE;struct Cyc_Absyn_Conref*_tmp2EF;_LL1A5: if(
_tmp2E5 <= (void*)4)goto _LL1A9;if(*((int*)_tmp2E5)!= 4)goto _LL1A7;_tmp2E6=((
struct Cyc_Absyn_PointerType_struct*)_tmp2E5)->f1;_tmp2E7=(void*)_tmp2E6.elt_typ;
_tmp2E8=_tmp2E6.ptr_atts;_tmp2E9=_tmp2E8.bounds;_tmp2EA=_tmp2E8.zero_term;_LL1A6:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2EA)){*
ptr_type=t;*elt_type=_tmp2E7;{void*_tmp2F0=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2E9);_LL1AC: if((int)_tmp2F0 != 0)goto _LL1AE;_LL1AD:*is_dyneither=0;*
is_dynforward=1;goto _LL1AB;_LL1AE: if((int)_tmp2F0 != 1)goto _LL1B0;_LL1AF:*
is_dynforward=0;*is_dyneither=1;goto _LL1AB;_LL1B0:;_LL1B1:*is_dynforward=(*
is_dyneither=0);goto _LL1AB;_LL1AB:;}return 1;}else{return 0;}_LL1A7: if(*((int*)
_tmp2E5)!= 7)goto _LL1A9;_tmp2EB=((struct Cyc_Absyn_ArrayType_struct*)_tmp2E5)->f1;
_tmp2EC=(void*)_tmp2EB.elt_type;_tmp2ED=_tmp2EB.tq;_tmp2EE=_tmp2EB.num_elts;
_tmp2EF=_tmp2EB.zero_term;_LL1A8: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp2EF)){*elt_type=_tmp2EC;*is_dynforward=(*is_dyneither=0);*ptr_type=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1[0]=({
struct Cyc_Absyn_PointerType_struct _tmp2F2;_tmp2F2.tag=4;_tmp2F2.f1=({struct Cyc_Absyn_PtrInfo
_tmp2F3;_tmp2F3.elt_typ=(void*)_tmp2EC;_tmp2F3.elt_tq=_tmp2ED;_tmp2F3.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp2F4;_tmp2F4.rgn=(void*)((void*)2);_tmp2F4.nullable=
Cyc_Absyn_false_conref;_tmp2F4.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp2F6;_tmp2F6.tag=0;_tmp2F6.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp2EE);
_tmp2F6;});_tmp2F5;}));_tmp2F4.zero_term=_tmp2EF;_tmp2F4.ptrloc=0;_tmp2F4;});
_tmp2F3;});_tmp2F2;});_tmp2F1;});return 1;}else{return 0;}_LL1A9:;_LL1AA: return 0;
_LL1A4:;}static int Cyc_Toc_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmp2F7=(void*)
e1->r;struct Cyc_Absyn_Exp*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;struct Cyc_Absyn_Exp*
_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FB;struct Cyc_Absyn_Exp*_tmp2FC;struct Cyc_Absyn_Exp*
_tmp2FD;_LL1B3: if(*((int*)_tmp2F7)!= 15)goto _LL1B5;_LL1B4:({struct Cyc_String_pa_struct
_tmp300;_tmp300.tag=0;_tmp300.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp2FE[1]={& _tmp300};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp2FF="we have a cast in a lhs:  %s";_tag_dynforward(_tmp2FF,sizeof(char),
_get_zero_arr_size(_tmp2FF,29));}),_tag_dynforward(_tmp2FE,sizeof(void*),1));}});
_LL1B5: if(*((int*)_tmp2F7)!= 22)goto _LL1B7;_tmp2F8=((struct Cyc_Absyn_Deref_e_struct*)
_tmp2F7)->f1;_LL1B6: _tmp2F9=_tmp2F8;goto _LL1B8;_LL1B7: if(*((int*)_tmp2F7)!= 25)
goto _LL1B9;_tmp2F9=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2F7)->f1;_LL1B8:
return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2F9->topt))->v,
ptr_type,is_dynforward,is_dyneither,elt_type);_LL1B9: if(*((int*)_tmp2F7)!= 24)
goto _LL1BB;_tmp2FA=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2F7)->f1;_LL1BA:
_tmp2FB=_tmp2FA;goto _LL1BC;_LL1BB: if(*((int*)_tmp2F7)!= 23)goto _LL1BD;_tmp2FB=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp2F7)->f1;_LL1BC: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2FB->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp303;_tmp303.tag=
0;_tmp303.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp301[1]={& _tmp303};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp302="found zero pointer aggregate member assignment: %s";
_tag_dynforward(_tmp302,sizeof(char),_get_zero_arr_size(_tmp302,51));}),
_tag_dynforward(_tmp301,sizeof(void*),1));}});return 0;_LL1BD: if(*((int*)_tmp2F7)
!= 14)goto _LL1BF;_tmp2FC=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2F7)->f1;
_LL1BE: _tmp2FD=_tmp2FC;goto _LL1C0;_LL1BF: if(*((int*)_tmp2F7)!= 13)goto _LL1C1;
_tmp2FD=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp2F7)->f1;_LL1C0: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2FD->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp306;_tmp306.tag=
0;_tmp306.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp304[1]={& _tmp306};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp305="found zero pointer instantiate/noinstantiate: %s";
_tag_dynforward(_tmp305,sizeof(char),_get_zero_arr_size(_tmp305,49));}),
_tag_dynforward(_tmp304,sizeof(void*),1));}});return 0;_LL1C1: if(*((int*)_tmp2F7)
!= 1)goto _LL1C3;_LL1C2: return 0;_LL1C3:;_LL1C4:({struct Cyc_String_pa_struct
_tmp309;_tmp309.tag=0;_tmp309.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp307[1]={& _tmp309};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp308="found bad lhs in is_zero_ptr_deref: %s";_tag_dynforward(_tmp308,sizeof(
char),_get_zero_arr_size(_tmp308,39));}),_tag_dynforward(_tmp307,sizeof(void*),1));}});
_LL1B2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp30A=Cyc_Tcutil_compress(t);void*_tmp30B;void*_tmp30C;void*_tmp30D;
void*_tmp30E;void*_tmp30F;void*_tmp310;void*_tmp311;void*_tmp312;void*_tmp313;
void*_tmp314;_LL1C6: if(_tmp30A <= (void*)4)goto _LL1D6;if(*((int*)_tmp30A)!= 4)
goto _LL1C8;_LL1C7: res=Cyc_Absyn_null_exp(0);goto _LL1C5;_LL1C8: if(*((int*)_tmp30A)
!= 5)goto _LL1CA;_tmp30B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f1;
_tmp30C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f2;if((int)_tmp30C != 
0)goto _LL1CA;_LL1C9: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp315;(_tmp315.Char_c).tag=0;(_tmp315.Char_c).f1=(void*)
_tmp30B;(_tmp315.Char_c).f2='\000';_tmp315;}),0);goto _LL1C5;_LL1CA: if(*((int*)
_tmp30A)!= 5)goto _LL1CC;_tmp30D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f1;
_tmp30E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f2;if((int)_tmp30E != 
1)goto _LL1CC;_LL1CB: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmp316;(_tmp316.Short_c).tag=1;(_tmp316.Short_c).f1=(void*)
_tmp30D;(_tmp316.Short_c).f2=0;_tmp316;}),0);goto _LL1C5;_LL1CC: if(*((int*)
_tmp30A)!= 12)goto _LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(*((int*)_tmp30A)!= 13)goto
_LL1D0;_LL1CF: _tmp30F=(void*)1;goto _LL1D1;_LL1D0: if(*((int*)_tmp30A)!= 5)goto
_LL1D2;_tmp30F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f1;_tmp310=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f2;if((int)_tmp310 != 2)goto
_LL1D2;_LL1D1: _tmp311=_tmp30F;goto _LL1D3;_LL1D2: if(*((int*)_tmp30A)!= 5)goto
_LL1D4;_tmp311=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f1;_tmp312=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f2;if((int)_tmp312 != 3)goto
_LL1D4;_LL1D3: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp317;(_tmp317.Int_c).tag=2;(_tmp317.Int_c).f1=(void*)_tmp311;(_tmp317.Int_c).f2=
0;_tmp317;}),0);goto _LL1C5;_LL1D4: if(*((int*)_tmp30A)!= 5)goto _LL1D6;_tmp313=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp30A)->f1;_tmp314=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp30A)->f2;if((int)_tmp314 != 4)goto _LL1D6;_LL1D5: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp318;(_tmp318.LongLong_c).tag=
3;(_tmp318.LongLong_c).f1=(void*)_tmp313;(_tmp318.LongLong_c).f2=(long long)0;
_tmp318;}),0);goto _LL1C5;_LL1D6: if((int)_tmp30A != 1)goto _LL1D8;_LL1D7: goto _LL1D9;
_LL1D8: if(_tmp30A <= (void*)4)goto _LL1DA;if(*((int*)_tmp30A)!= 6)goto _LL1DA;
_LL1D9: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp319;(_tmp319.Float_c).tag=4;(_tmp319.Float_c).f1=({const char*_tmp31A="0.0";
_tag_dynforward(_tmp31A,sizeof(char),_get_zero_arr_size(_tmp31A,4));});_tmp319;}),
0);goto _LL1C5;_LL1DA:;_LL1DB:({struct Cyc_String_pa_struct _tmp31D;_tmp31D.tag=0;
_tmp31D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp31B[1]={& _tmp31D};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp31C="found non-zero type %s in generate_zero";
_tag_dynforward(_tmp31C,sizeof(char),_get_zero_arr_size(_tmp31C,40));}),
_tag_dynforward(_tmp31B,sizeof(void*),1));}});_LL1C5:;}res->topt=({struct Cyc_Core_Opt*
_tmp31E=_cycalloc(sizeof(*_tmp31E));_tmp31E->v=(void*)t;_tmp31E;});return res;}
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dynforward,int is_dyneither,void*elt_type){int is_fat=is_dynforward
 || is_dyneither;void*fat_ptr_type;if(is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp31F=Cyc_Toc_typ_to_c(
elt_type);void*_tmp320=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp321=Cyc_Absyn_cstar_typ(
_tmp31F,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp322=({struct Cyc_Core_Opt*_tmp360=
_cycalloc(sizeof(*_tmp360));_tmp360->v=(void*)_tmp321;_tmp360;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp323=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_Absyn_Exp*
_tmp325;struct Cyc_Absyn_Exp*_tmp326;_LL1DD: if(*((int*)_tmp323)!= 22)goto _LL1DF;
_tmp324=((struct Cyc_Absyn_Deref_e_struct*)_tmp323)->f1;_LL1DE: if(!is_fat){
_tmp324=Cyc_Toc_cast_it(fat_ptr_type,_tmp324);_tmp324->topt=({struct Cyc_Core_Opt*
_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->v=(void*)fat_ptr_type;_tmp327;});}
Cyc_Toc_exp_to_c(nv,_tmp324);xinit=_tmp324;goto _LL1DC;_LL1DF: if(*((int*)_tmp323)
!= 25)goto _LL1E1;_tmp325=((struct Cyc_Absyn_Subscript_e_struct*)_tmp323)->f1;
_tmp326=((struct Cyc_Absyn_Subscript_e_struct*)_tmp323)->f2;_LL1E0: if(!is_fat){
_tmp325=Cyc_Toc_cast_it(fat_ptr_type,_tmp325);_tmp325->topt=({struct Cyc_Core_Opt*
_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328->v=(void*)fat_ptr_type;_tmp328;});}
Cyc_Toc_exp_to_c(nv,_tmp325);Cyc_Toc_exp_to_c(nv,_tmp326);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp329[3];_tmp329[2]=_tmp326;_tmp329[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp329[0]=_tmp325;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp329,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp32A[3];_tmp32A[2]=_tmp326;_tmp32A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp32A[0]=_tmp325;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp32A,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1DC;_LL1E1:;_LL1E2:({void*_tmp32B[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp32C="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,53));}),
_tag_dynforward(_tmp32B,sizeof(void*),0));});_LL1DC:;}{struct _tuple1*_tmp32D=Cyc_Toc_temp_var();
struct _RegionHandle _tmp32E=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp32E;
_push_region(rgn2);{struct Cyc_Toc_Env*_tmp32F=Cyc_Toc_add_varmap(rgn2,nv,_tmp32D,
Cyc_Absyn_var_exp(_tmp32D,0));struct Cyc_Absyn_Vardecl*_tmp330=({struct Cyc_Absyn_Vardecl*
_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F->sc=(void*)((void*)2);_tmp35F->name=
_tmp32D;_tmp35F->tq=Cyc_Toc_mt_tq;_tmp35F->type=(void*)_tmp320;_tmp35F->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp35F->rgn=0;_tmp35F->attributes=0;_tmp35F->escapes=
0;_tmp35F;});struct Cyc_Absyn_Local_b_struct*_tmp331=({struct Cyc_Absyn_Local_b_struct*
_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D[0]=({struct Cyc_Absyn_Local_b_struct
_tmp35E;_tmp35E.tag=3;_tmp35E.f1=_tmp330;_tmp35E;});_tmp35D;});struct Cyc_Absyn_Exp*
_tmp332=Cyc_Absyn_varb_exp(_tmp32D,(void*)_tmp331,0);_tmp332->topt=({struct Cyc_Core_Opt*
_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333->v=(void*)fat_ptr_type;_tmp333;});{
struct Cyc_Absyn_Exp*_tmp334=Cyc_Absyn_deref_exp(_tmp332,0);_tmp334->topt=({
struct Cyc_Core_Opt*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335->v=(void*)
elt_type;_tmp335;});Cyc_Toc_exp_to_c(_tmp32F,_tmp334);{struct _tuple1*_tmp336=Cyc_Toc_temp_var();
_tmp32F=Cyc_Toc_add_varmap(rgn2,_tmp32F,_tmp336,Cyc_Absyn_var_exp(_tmp336,0));{
struct Cyc_Absyn_Vardecl*_tmp337=({struct Cyc_Absyn_Vardecl*_tmp35C=_cycalloc(
sizeof(*_tmp35C));_tmp35C->sc=(void*)((void*)2);_tmp35C->name=_tmp336;_tmp35C->tq=
Cyc_Toc_mt_tq;_tmp35C->type=(void*)_tmp31F;_tmp35C->initializer=(struct Cyc_Absyn_Exp*)
_tmp334;_tmp35C->rgn=0;_tmp35C->attributes=0;_tmp35C->escapes=0;_tmp35C;});
struct Cyc_Absyn_Local_b_struct*_tmp338=({struct Cyc_Absyn_Local_b_struct*_tmp35A=
_cycalloc(sizeof(*_tmp35A));_tmp35A[0]=({struct Cyc_Absyn_Local_b_struct _tmp35B;
_tmp35B.tag=3;_tmp35B.f1=_tmp337;_tmp35B;});_tmp35A;});struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp339=Cyc_Absyn_varb_exp(_tmp336,(
void*)_tmp338,0);_tmp339->topt=_tmp334->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp339,e2,0);z_init->topt=_tmp339->topt;}Cyc_Toc_exp_to_c(_tmp32F,
z_init);{struct _tuple1*_tmp33A=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp33B=({struct Cyc_Absyn_Vardecl*_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359->sc=(
void*)((void*)2);_tmp359->name=_tmp33A;_tmp359->tq=Cyc_Toc_mt_tq;_tmp359->type=(
void*)_tmp31F;_tmp359->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp359->rgn=0;
_tmp359->attributes=0;_tmp359->escapes=0;_tmp359;});struct Cyc_Absyn_Local_b_struct*
_tmp33C=({struct Cyc_Absyn_Local_b_struct*_tmp357=_cycalloc(sizeof(*_tmp357));
_tmp357[0]=({struct Cyc_Absyn_Local_b_struct _tmp358;_tmp358.tag=3;_tmp358.f1=
_tmp33B;_tmp358;});_tmp357;});_tmp32F=Cyc_Toc_add_varmap(rgn2,_tmp32F,_tmp33A,
Cyc_Absyn_var_exp(_tmp33A,0));{struct Cyc_Absyn_Exp*_tmp33D=Cyc_Absyn_varb_exp(
_tmp336,(void*)_tmp338,0);_tmp33D->topt=_tmp334->topt;{struct Cyc_Absyn_Exp*
_tmp33E=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp33F=Cyc_Absyn_prim2_exp((
void*)5,_tmp33D,_tmp33E,0);_tmp33F->topt=({struct Cyc_Core_Opt*_tmp340=_cycalloc(
sizeof(*_tmp340));_tmp340->v=(void*)Cyc_Absyn_sint_typ;_tmp340;});Cyc_Toc_exp_to_c(
_tmp32F,_tmp33F);{struct Cyc_Absyn_Exp*_tmp341=Cyc_Absyn_varb_exp(_tmp33A,(void*)
_tmp33C,0);_tmp341->topt=_tmp334->topt;{struct Cyc_Absyn_Exp*_tmp342=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp343=Cyc_Absyn_prim2_exp((void*)6,_tmp341,
_tmp342,0);_tmp343->topt=({struct Cyc_Core_Opt*_tmp344=_cycalloc(sizeof(*_tmp344));
_tmp344->v=(void*)Cyc_Absyn_sint_typ;_tmp344;});Cyc_Toc_exp_to_c(_tmp32F,_tmp343);{
struct Cyc_List_List*_tmp345=({struct Cyc_Absyn_Exp*_tmp356[2];_tmp356[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp356[0]=Cyc_Absyn_varb_exp(_tmp32D,(void*)
_tmp331,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp356,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp346=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;if(is_dynforward)xsize=
Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,
_tmp345,0),_tmp346,0);else{xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dyneither_size_e,_tmp345,0),_tmp346,0);}{struct Cyc_Absyn_Exp*
_tmp347=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp33F,_tmp343,0),0);struct
Cyc_Absyn_Stmt*_tmp348=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,
0,0),0);struct Cyc_Absyn_Exp*_tmp349=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(
_tmp32D,(void*)_tmp331,0),Cyc_Toc_curr_sp,0);_tmp349=Cyc_Toc_cast_it(_tmp321,
_tmp349);{struct Cyc_Absyn_Exp*_tmp34A=Cyc_Absyn_deref_exp(_tmp349,0);struct Cyc_Absyn_Exp*
_tmp34B=Cyc_Absyn_assign_exp(_tmp34A,Cyc_Absyn_var_exp(_tmp33A,0),0);struct Cyc_Absyn_Stmt*
_tmp34C=Cyc_Absyn_exp_stmt(_tmp34B,0);_tmp34C=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp347,_tmp348,Cyc_Absyn_skip_stmt(0),0),_tmp34C,0);_tmp34C=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[
0]=({struct Cyc_Absyn_Var_d_struct _tmp34F;_tmp34F.tag=0;_tmp34F.f1=_tmp33B;
_tmp34F;});_tmp34E;}));_tmp34D->loc=0;_tmp34D;}),_tmp34C,0);_tmp34C=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351[
0]=({struct Cyc_Absyn_Var_d_struct _tmp352;_tmp352.tag=0;_tmp352.f1=_tmp337;
_tmp352;});_tmp351;}));_tmp350->loc=0;_tmp350;}),_tmp34C,0);_tmp34C=Cyc_Absyn_decl_stmt(({
struct Cyc_Absyn_Decl*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[
0]=({struct Cyc_Absyn_Var_d_struct _tmp355;_tmp355.tag=0;_tmp355.f1=_tmp330;
_tmp355;});_tmp354;}));_tmp353->loc=0;_tmp353;}),_tmp34C,0);(void*)(e->r=(void*)
Cyc_Toc_stmt_exp_r(_tmp34C));}}}}}}}}}}}};_pop_region(rgn2);}}}struct _tuple9{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp361=(void*)e->r;if(e->topt == 0)({struct Cyc_String_pa_struct
_tmp364;_tmp364.tag=0;_tmp364.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp362[1]={& _tmp364};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp363="exp_to_c: no type for %s";_tag_dynforward(_tmp363,sizeof(char),
_get_zero_arr_size(_tmp363,25));}),_tag_dynforward(_tmp362,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp365=
_tmp361;union Cyc_Absyn_Cnst_union _tmp366;struct _tuple1*_tmp367;void*_tmp368;
struct _tuple1*_tmp369;void*_tmp36A;struct Cyc_List_List*_tmp36B;struct Cyc_Absyn_Exp*
_tmp36C;void*_tmp36D;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_Core_Opt*_tmp36F;
struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*_tmp371;struct Cyc_Absyn_Exp*
_tmp372;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*
_tmp375;struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*
_tmp378;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Exp*_tmp37A;struct Cyc_List_List*
_tmp37B;struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_List_List*_tmp37D;struct Cyc_Absyn_VarargCallInfo*
_tmp37E;struct Cyc_Absyn_Exp*_tmp37F;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_VarargCallInfo*
_tmp381;struct Cyc_Absyn_VarargCallInfo _tmp382;int _tmp383;struct Cyc_List_List*
_tmp384;struct Cyc_Absyn_VarargInfo*_tmp385;struct Cyc_Absyn_Exp*_tmp386;struct Cyc_Absyn_Exp*
_tmp387;struct Cyc_Absyn_Exp*_tmp388;struct Cyc_List_List*_tmp389;void*_tmp38A;
void**_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;int _tmp38D;void*_tmp38E;struct Cyc_Absyn_Exp*
_tmp38F;struct Cyc_Absyn_Exp*_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Exp*
_tmp392;void*_tmp393;void*_tmp394;void*_tmp395;struct _dynforward_ptr*_tmp396;
void*_tmp397;void*_tmp398;unsigned int _tmp399;struct Cyc_Absyn_Exp*_tmp39A;struct
Cyc_Absyn_Exp*_tmp39B;struct _dynforward_ptr*_tmp39C;struct Cyc_Absyn_Exp*_tmp39D;
struct _dynforward_ptr*_tmp39E;struct Cyc_Absyn_Exp*_tmp39F;struct Cyc_Absyn_Exp*
_tmp3A0;struct Cyc_List_List*_tmp3A1;struct Cyc_List_List*_tmp3A2;struct Cyc_Absyn_Vardecl*
_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A5;int _tmp3A6;
struct _tuple1*_tmp3A7;struct Cyc_List_List*_tmp3A8;struct Cyc_List_List*_tmp3A9;
struct Cyc_Absyn_Aggrdecl*_tmp3AA;void*_tmp3AB;struct Cyc_List_List*_tmp3AC;struct
Cyc_List_List*_tmp3AD;struct Cyc_Absyn_Tuniondecl*_tmp3AE;struct Cyc_Absyn_Tunionfield*
_tmp3AF;struct Cyc_List_List*_tmp3B0;struct Cyc_Absyn_Tuniondecl*_tmp3B1;struct Cyc_Absyn_Tunionfield*
_tmp3B2;struct Cyc_Absyn_MallocInfo _tmp3B3;int _tmp3B4;struct Cyc_Absyn_Exp*_tmp3B5;
void**_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;int _tmp3B8;struct Cyc_Absyn_Stmt*
_tmp3B9;_LL1E4: if(*((int*)_tmp365)!= 0)goto _LL1E6;_tmp366=((struct Cyc_Absyn_Const_e_struct*)
_tmp365)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp365)->f1).Null_c).tag != 
6)goto _LL1E6;_LL1E5: {struct Cyc_Absyn_Exp*_tmp3BA=Cyc_Absyn_uint_exp(0,0);int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ,& forward_only)){if(
Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(
old_typ,_tmp3BA,_tmp3BA,forward_only))->r));else{if(forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp3BB[3];_tmp3BB[2]=_tmp3BA;_tmp3BB[1]=_tmp3BA;_tmp3BB[0]=_tmp3BA;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3BB,sizeof(struct Cyc_Absyn_Exp*),
3));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp3BC[3];_tmp3BC[2]=_tmp3BA;_tmp3BC[1]=_tmp3BA;_tmp3BC[0]=
_tmp3BA;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3BC,sizeof(struct Cyc_Absyn_Exp*),3));})));}}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1E3;}_LL1E6: if(*((int*)_tmp365)!= 0)
goto _LL1E8;_LL1E7: goto _LL1E3;_LL1E8: if(*((int*)_tmp365)!= 1)goto _LL1EA;_tmp367=((
struct Cyc_Absyn_Var_e_struct*)_tmp365)->f1;_tmp368=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp365)->f2;_LL1E9:{struct _handler_cons _tmp3BD;_push_handler(& _tmp3BD);{int
_tmp3BF=0;if(setjmp(_tmp3BD.handler))_tmp3BF=1;if(!_tmp3BF){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp367))->r));;_pop_handler();}else{void*
_tmp3BE=(void*)_exn_thrown;void*_tmp3C1=_tmp3BE;_LL239: if(_tmp3C1 != Cyc_Dict_Absent)
goto _LL23B;_LL23A:({struct Cyc_String_pa_struct _tmp3C4;_tmp3C4.tag=0;_tmp3C4.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp367));{
void*_tmp3C2[1]={& _tmp3C4};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3C3="Can't find %s in exp_to_c, Var\n";
_tag_dynforward(_tmp3C3,sizeof(char),_get_zero_arr_size(_tmp3C3,32));}),
_tag_dynforward(_tmp3C2,sizeof(void*),1));}});_LL23B:;_LL23C:(void)_throw(
_tmp3C1);_LL238:;}}}goto _LL1E3;_LL1EA: if(*((int*)_tmp365)!= 2)goto _LL1EC;_tmp369=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp365)->f1;_LL1EB:({void*_tmp3C5[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3C6="unknownid";_tag_dynforward(_tmp3C6,sizeof(char),
_get_zero_arr_size(_tmp3C6,10));}),_tag_dynforward(_tmp3C5,sizeof(void*),0));});
_LL1EC: if(*((int*)_tmp365)!= 3)goto _LL1EE;_tmp36A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp365)->f1;_tmp36B=((struct Cyc_Absyn_Primop_e_struct*)_tmp365)->f2;_LL1ED: {
struct Cyc_List_List*_tmp3C7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp36B);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp36B);{void*_tmp3C8=_tmp36A;_LL23E:
if((int)_tmp3C8 != 19)goto _LL240;_LL23F: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp36B))->hd;{void*_tmp3C9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_PtrInfo _tmp3CC;void*_tmp3CD;
struct Cyc_Absyn_PtrAtts _tmp3CE;struct Cyc_Absyn_Conref*_tmp3CF;struct Cyc_Absyn_Conref*
_tmp3D0;struct Cyc_Absyn_Conref*_tmp3D1;_LL253: if(_tmp3C9 <= (void*)4)goto _LL257;
if(*((int*)_tmp3C9)!= 7)goto _LL255;_tmp3CA=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3C9)->f1;_tmp3CB=_tmp3CA.num_elts;_LL254:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp3CB))->r));goto _LL252;_LL255: if(*((int*)
_tmp3C9)!= 4)goto _LL257;_tmp3CC=((struct Cyc_Absyn_PointerType_struct*)_tmp3C9)->f1;
_tmp3CD=(void*)_tmp3CC.elt_typ;_tmp3CE=_tmp3CC.ptr_atts;_tmp3CF=_tmp3CE.nullable;
_tmp3D0=_tmp3CE.bounds;_tmp3D1=_tmp3CE.zero_term;_LL256:{void*_tmp3D2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp3D0);struct Cyc_Absyn_Exp*_tmp3D3;_LL25A: if((int)_tmp3D2
!= 0)goto _LL25C;_LL25B:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dynforward_size_e,({
struct Cyc_Absyn_Exp*_tmp3D4[2];_tmp3D4[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3CD),0);_tmp3D4[0]=(struct Cyc_Absyn_Exp*)_tmp36B->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3D4,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL259;_LL25C: if((int)_tmp3D2 != 1)goto _LL25E;_LL25D:(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*
_tmp3D5[2];_tmp3D5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3CD),0);
_tmp3D5[0]=(struct Cyc_Absyn_Exp*)_tmp36B->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3D5,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL259;_LL25E: if(_tmp3D2 <= (void*)2)goto _LL260;if(*((int*)_tmp3D2)
!= 0)goto _LL260;_tmp3D3=((struct Cyc_Absyn_Upper_b_struct*)_tmp3D2)->f1;_LL25F:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3D1))(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_zero_arr_size_e,({struct Cyc_Absyn_Exp*
_tmp3D6[2];_tmp3D6[1]=_tmp3D3;_tmp3D6[0]=(struct Cyc_Absyn_Exp*)_tmp36B->hd;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3D6,sizeof(struct Cyc_Absyn_Exp*),2));})));else{if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp3CF))(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp3D3,Cyc_Absyn_uint_exp(0,0)));else{(void*)(e->r=(void*)((void*)_tmp3D3->r));
goto _LL259;}}goto _LL259;_LL260: if(_tmp3D2 <= (void*)2)goto _LL259;if(*((int*)
_tmp3D2)!= 1)goto _LL259;_LL261:({void*_tmp3D7[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3D8="toc: size of AbsUpper_b";
_tag_dynforward(_tmp3D8,sizeof(char),_get_zero_arr_size(_tmp3D8,24));}),
_tag_dynforward(_tmp3D7,sizeof(void*),0));});_LL259:;}goto _LL252;_LL257:;_LL258:({
struct Cyc_String_pa_struct _tmp3DC;_tmp3DC.tag=0;_tmp3DC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct _tmp3DB;
_tmp3DB.tag=0;_tmp3DB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp3D9[2]={&
_tmp3DB,& _tmp3DC};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3DA="size primop applied to non-array %s (%s)";_tag_dynforward(
_tmp3DA,sizeof(char),_get_zero_arr_size(_tmp3DA,41));}),_tag_dynforward(_tmp3D9,
sizeof(void*),2));}}});_LL252:;}goto _LL23D;}_LL240: if((int)_tmp3C8 != 0)goto
_LL242;_LL241:{void*_tmp3DD=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)
_check_null(_tmp3C7))->hd);struct Cyc_Absyn_PtrInfo _tmp3DE;void*_tmp3DF;struct Cyc_Absyn_PtrAtts
_tmp3E0;struct Cyc_Absyn_Conref*_tmp3E1;struct Cyc_Absyn_Conref*_tmp3E2;_LL263: if(
_tmp3DD <= (void*)4)goto _LL265;if(*((int*)_tmp3DD)!= 4)goto _LL265;_tmp3DE=((
struct Cyc_Absyn_PointerType_struct*)_tmp3DD)->f1;_tmp3DF=(void*)_tmp3DE.elt_typ;
_tmp3E0=_tmp3DE.ptr_atts;_tmp3E1=_tmp3E0.bounds;_tmp3E2=_tmp3E0.zero_term;_LL264:{
void*_tmp3E3=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3E1);struct Cyc_Absyn_Exp*
_tmp3E4;_LL268: if((int)_tmp3E3 != 0)goto _LL26A;_LL269: {struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp36B))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp36B->tl))->hd;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_ptr_plus_e,({struct
Cyc_Absyn_Exp*_tmp3E5[3];_tmp3E5[2]=e2;_tmp3E5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp3DF),0);_tmp3E5[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3E5,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL267;}_LL26A:
if((int)_tmp3E3 != 1)goto _LL26C;_LL26B: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp36B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp36B->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3E6[3];_tmp3E6[2]=e2;
_tmp3E6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3DF),0);_tmp3E6[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3E6,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL267;}_LL26C: if(_tmp3E3 <= (
void*)2)goto _LL26E;if(*((int*)_tmp3E3)!= 0)goto _LL26E;_tmp3E4=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3E3)->f1;_LL26D: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3E2)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp36B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp36B->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__zero_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp3E7[3];_tmp3E7[2]=e2;_tmp3E7[
1]=_tmp3E4;_tmp3E7[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3E7,sizeof(struct Cyc_Absyn_Exp*),3));})));}goto _LL267;_LL26E:
if(_tmp3E3 <= (void*)2)goto _LL267;if(*((int*)_tmp3E3)!= 1)goto _LL267;_LL26F:({
void*_tmp3E8[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp3E9="toc: plus on AbsUpper_b";_tag_dynforward(
_tmp3E9,sizeof(char),_get_zero_arr_size(_tmp3E9,24));}),_tag_dynforward(_tmp3E8,
sizeof(void*),0));});_LL267:;}goto _LL262;_LL265:;_LL266: goto _LL262;_LL262:;}goto
_LL23D;_LL242: if((int)_tmp3C8 != 2)goto _LL244;_LL243: {void*elt_typ=(void*)0;int
forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp3C7))->hd,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp36B))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp36B->tl))->hd;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp3C7->tl))->hd,& forward_only)){(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(
Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{if(forward_only)({void*_tmp3EA[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3EB="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp3EB,sizeof(char),_get_zero_arr_size(_tmp3EB,50));}),
_tag_dynforward(_tmp3EA,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3EC[3];_tmp3EC[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp3EC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp3EC[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3EC,sizeof(struct Cyc_Absyn_Exp*),3));})));}}goto _LL23D;}
_LL244: if((int)_tmp3C8 != 5)goto _LL246;_LL245: goto _LL247;_LL246: if((int)_tmp3C8 != 
6)goto _LL248;_LL247: goto _LL249;_LL248: if((int)_tmp3C8 != 7)goto _LL24A;_LL249: goto
_LL24B;_LL24A: if((int)_tmp3C8 != 9)goto _LL24C;_LL24B: goto _LL24D;_LL24C: if((int)
_tmp3C8 != 8)goto _LL24E;_LL24D: goto _LL24F;_LL24E: if((int)_tmp3C8 != 10)goto _LL250;
_LL24F: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp36B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp36B->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3C7))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp3C7->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL23D;}_LL250:;_LL251: goto _LL23D;_LL23D:;}
goto _LL1E3;}_LL1EE: if(*((int*)_tmp365)!= 5)goto _LL1F0;_tmp36C=((struct Cyc_Absyn_Increment_e_struct*)
_tmp365)->f1;_tmp36D=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp365)->f2;
_LL1EF: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp36C->topt))->v;
void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=
0;struct _dynforward_ptr incr_str=({const char*_tmp3F8="increment";_tag_dynforward(
_tmp3F8,sizeof(char),_get_zero_arr_size(_tmp3F8,10));});if(_tmp36D == (void*)2
 || _tmp36D == (void*)3)incr_str=({const char*_tmp3ED="decrement";_tag_dynforward(
_tmp3ED,sizeof(char),_get_zero_arr_size(_tmp3ED,10));});if(Cyc_Toc_is_zero_ptr_deref(
_tmp36C,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct
_tmp3F0;_tmp3F0.tag=0;_tmp3F0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
incr_str);{void*_tmp3EE[1]={& _tmp3F0};Cyc_Tcutil_terr(e->loc,({const char*_tmp3EF="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp3EF,sizeof(char),_get_zero_arr_size(_tmp3EF,74));}),
_tag_dynforward(_tmp3EE,sizeof(void*),1));}});({void*_tmp3F1[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3F2="in-place inc on zero-term";_tag_dynforward(_tmp3F2,sizeof(char),
_get_zero_arr_size(_tmp3F2,26));}),_tag_dynforward(_tmp3F1,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp36C);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp36D == (void*)3  || _tmp36D == (void*)2)({void*_tmp3F3[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp3F4="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp3F4,sizeof(char),_get_zero_arr_size(_tmp3F4,43));}),
_tag_dynforward(_tmp3F3,sizeof(void*),0));});fn_e=_tmp36D == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp36D == (void*)1  || _tmp36D
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp36D == (void*)2  || _tmp36D == (void*)3)change=- 1;}(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp3F5[3];_tmp3F5[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp3F5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp3F5[0]=
Cyc_Absyn_address_exp(_tmp36C,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp3F5,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp3F6=_tmp36D == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e: Cyc_Toc__zero_arr_inplace_plus_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp3F6,({struct Cyc_Absyn_Exp*_tmp3F7[2];
_tmp3F7[1]=Cyc_Absyn_signed_int_exp(1,0);_tmp3F7[0]=_tmp36C;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3F7,sizeof(struct Cyc_Absyn_Exp*),
2));})));}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp36C)){Cyc_Toc_lvalue_assign(
_tmp36C,0,Cyc_Toc_incr_lvalue,_tmp36D);(void*)(e->r=(void*)((void*)_tmp36C->r));}}}
goto _LL1E3;}}_LL1F0: if(*((int*)_tmp365)!= 4)goto _LL1F2;_tmp36E=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp365)->f1;_tmp36F=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp365)->f2;_tmp370=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp365)->f3;_LL1F1: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Toc_is_zero_ptr_deref(
_tmp36E,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp36E,_tmp36F,_tmp370,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp36E);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp36E->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp370->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp36E);
Cyc_Toc_exp_to_c(nv,_tmp370);{int done=0;if(_tmp36F != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp3F9=(void*)_tmp36F->v;_LL271:
if((int)_tmp3F9 != 0)goto _LL273;_LL272: change=_tmp370;goto _LL270;_LL273: if((int)
_tmp3F9 != 2)goto _LL275;_LL274: if(forward_only)({void*_tmp3FA[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp3FB="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp3FB,sizeof(char),_get_zero_arr_size(_tmp3FB,46));}),
_tag_dynforward(_tmp3FA,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp370,0);goto _LL270;_LL275:;_LL276:({void*_tmp3FC[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp3FD="bad t ? pointer arithmetic";_tag_dynforward(_tmp3FD,sizeof(char),
_get_zero_arr_size(_tmp3FD,27));}),_tag_dynforward(_tmp3FC,sizeof(void*),0));});
_LL270:;}done=1;{struct Cyc_Absyn_Exp*_tmp3FE=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
_tmp3FE,({struct Cyc_Absyn_Exp*_tmp3FF[3];_tmp3FF[2]=change;_tmp3FF[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp3FF[0]=Cyc_Absyn_address_exp(_tmp36E,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3FF,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp400=(void*)_tmp36F->v;_LL278: if((int)_tmp400 != 0)
goto _LL27A;_LL279: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp401[2];_tmp401[1]=_tmp370;_tmp401[0]=_tmp36E;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp401,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL277;_LL27A:;_LL27B:({void*_tmp402[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp403="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp403,sizeof(
char),_get_zero_arr_size(_tmp403,39));}),_tag_dynforward(_tmp402,sizeof(void*),0));});
_LL277:;}}}if(!done){if(e1_poly)(void*)(_tmp370->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp370->r,0)));if(!Cyc_Absyn_is_lvalue(_tmp36E)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple8*),struct _tuple8*f_env))Cyc_Toc_lvalue_assign)(
_tmp36E,0,Cyc_Toc_assignop_lvalue,({struct _tuple8*_tmp404=_cycalloc(sizeof(
struct _tuple8)* 1);_tmp404[0]=({struct _tuple8 _tmp405;_tmp405.f1=_tmp36F;_tmp405.f2=
_tmp370;_tmp405;});_tmp404;}));(void*)(e->r=(void*)((void*)_tmp36E->r));}}goto
_LL1E3;}}}_LL1F2: if(*((int*)_tmp365)!= 6)goto _LL1F4;_tmp371=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp365)->f1;_tmp372=((struct Cyc_Absyn_Conditional_e_struct*)_tmp365)->f2;
_tmp373=((struct Cyc_Absyn_Conditional_e_struct*)_tmp365)->f3;_LL1F3: Cyc_Toc_exp_to_c(
nv,_tmp371);Cyc_Toc_exp_to_c(nv,_tmp372);Cyc_Toc_exp_to_c(nv,_tmp373);goto _LL1E3;
_LL1F4: if(*((int*)_tmp365)!= 7)goto _LL1F6;_tmp374=((struct Cyc_Absyn_And_e_struct*)
_tmp365)->f1;_tmp375=((struct Cyc_Absyn_And_e_struct*)_tmp365)->f2;_LL1F5: Cyc_Toc_exp_to_c(
nv,_tmp374);Cyc_Toc_exp_to_c(nv,_tmp375);goto _LL1E3;_LL1F6: if(*((int*)_tmp365)!= 
8)goto _LL1F8;_tmp376=((struct Cyc_Absyn_Or_e_struct*)_tmp365)->f1;_tmp377=((
struct Cyc_Absyn_Or_e_struct*)_tmp365)->f2;_LL1F7: Cyc_Toc_exp_to_c(nv,_tmp376);
Cyc_Toc_exp_to_c(nv,_tmp377);goto _LL1E3;_LL1F8: if(*((int*)_tmp365)!= 9)goto
_LL1FA;_tmp378=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp365)->f1;_tmp379=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp365)->f2;_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp378);Cyc_Toc_exp_to_c(
nv,_tmp379);goto _LL1E3;_LL1FA: if(*((int*)_tmp365)!= 10)goto _LL1FC;_tmp37A=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp365)->f1;_tmp37B=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp365)->f2;_LL1FB: _tmp37C=_tmp37A;_tmp37D=_tmp37B;goto _LL1FD;_LL1FC: if(*((int*)
_tmp365)!= 11)goto _LL1FE;_tmp37C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp365)->f1;
_tmp37D=((struct Cyc_Absyn_FnCall_e_struct*)_tmp365)->f2;_tmp37E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp365)->f3;if(_tmp37E != 0)goto _LL1FE;_LL1FD: Cyc_Toc_exp_to_c(nv,_tmp37C);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp37D);goto _LL1E3;
_LL1FE: if(*((int*)_tmp365)!= 11)goto _LL200;_tmp37F=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp365)->f1;_tmp380=((struct Cyc_Absyn_FnCall_e_struct*)_tmp365)->f2;_tmp381=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp365)->f3;if(_tmp381 == 0)goto _LL200;_tmp382=*
_tmp381;_tmp383=_tmp382.num_varargs;_tmp384=_tmp382.injectors;_tmp385=_tmp382.vai;
_LL1FF:{struct _RegionHandle _tmp406=_new_region("r");struct _RegionHandle*r=&
_tmp406;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp383,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp385->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp380);int num_normargs=num_args - _tmp383;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp380=_tmp380->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp380))->hd);
new_args=({struct Cyc_List_List*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->hd=(
struct Cyc_Absyn_Exp*)_tmp380->hd;_tmp407->tl=new_args;_tmp407;});}}new_args=({
struct Cyc_List_List*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp409[3];_tmp409[2]=
num_varargs_exp;_tmp409[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp409[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp409,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp408->tl=
new_args;_tmp408;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp37F);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp37F,new_args,0),0);if(_tmp385->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp40A=Cyc_Tcutil_compress((void*)_tmp385->type);
struct Cyc_Absyn_TunionInfo _tmp40B;union Cyc_Absyn_TunionInfoU_union _tmp40C;struct
Cyc_Absyn_Tuniondecl**_tmp40D;struct Cyc_Absyn_Tuniondecl*_tmp40E;_LL27D: if(
_tmp40A <= (void*)4)goto _LL27F;if(*((int*)_tmp40A)!= 2)goto _LL27F;_tmp40B=((
struct Cyc_Absyn_TunionType_struct*)_tmp40A)->f1;_tmp40C=_tmp40B.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp40A)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL27F;_tmp40D=(_tmp40C.KnownTunion).f1;_tmp40E=*_tmp40D;_LL27E: tud=
_tmp40E;goto _LL27C;_LL27F:;_LL280:({void*_tmp40F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp410="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp410,
sizeof(char),_get_zero_arr_size(_tmp410,42));}),_tag_dynforward(_tmp40F,sizeof(
void*),0));});_LL27C:;}{struct _dynforward_ptr vs=({unsigned int _tmp418=(
unsigned int)_tmp383;struct _tuple1**_tmp419=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp418));struct _dynforward_ptr _tmp41B=
_tag_dynforward(_tmp419,sizeof(struct _tuple1*),_tmp418);{unsigned int _tmp41A=
_tmp418;unsigned int i;for(i=0;i < _tmp41A;i ++){_tmp419[i]=Cyc_Toc_temp_var();}}
_tmp41B;});struct Cyc_List_List*_tmp411=0;{int i=_tmp383 - 1;for(0;i >= 0;-- i){
_tmp411=({struct Cyc_List_List*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp412->tl=
_tmp411;_tmp412;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp411,0),s,0);{int i=0;for(0;_tmp380 != 0;(((
_tmp380=_tmp380->tl,_tmp384=_tmp384->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp380->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp414;struct _tuple1*
_tmp415;struct Cyc_List_List*_tmp416;struct Cyc_Absyn_Tunionfield*_tmp413=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp384))->hd;_tmp414=*
_tmp413;_tmp415=_tmp414.name;_tmp416=_tmp414.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp416))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp415,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp415,({const char*_tmp417="_struct";
_tag_dynforward(_tmp417,sizeof(char),_get_zero_arr_size(_tmp417,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp380 != 0;(_tmp380=_tmp380->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp380->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp380->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}};_pop_region(r);}goto _LL1E3;_LL200: if(*((int*)_tmp365)!= 12)goto _LL202;
_tmp386=((struct Cyc_Absyn_Throw_e_struct*)_tmp365)->f1;_LL201: Cyc_Toc_exp_to_c(
nv,_tmp386);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp386),0))->r));goto _LL1E3;_LL202: if(*((int*)
_tmp365)!= 13)goto _LL204;_tmp387=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp365)->f1;_LL203: Cyc_Toc_exp_to_c(nv,_tmp387);goto _LL1E3;_LL204: if(*((int*)
_tmp365)!= 14)goto _LL206;_tmp388=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp365)->f1;
_tmp389=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp365)->f2;_LL205: Cyc_Toc_exp_to_c(
nv,_tmp388);for(0;_tmp389 != 0;_tmp389=_tmp389->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp389->hd);if(k != (void*)4  && k != (void*)3){{void*_tmp41C=Cyc_Tcutil_compress((
void*)_tmp389->hd);_LL282: if(_tmp41C <= (void*)4)goto _LL286;if(*((int*)_tmp41C)!= 
1)goto _LL284;_LL283: goto _LL285;_LL284: if(*((int*)_tmp41C)!= 2)goto _LL286;_LL285:
continue;_LL286:;_LL287:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp388,
0))->r));goto _LL281;_LL281:;}break;}}goto _LL1E3;_LL206: if(*((int*)_tmp365)!= 15)
goto _LL208;_tmp38A=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp365)->f1;_tmp38B=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp365)->f1);_tmp38C=((struct
Cyc_Absyn_Cast_e_struct*)_tmp365)->f2;_tmp38D=((struct Cyc_Absyn_Cast_e_struct*)
_tmp365)->f3;_tmp38E=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp365)->f4;_LL207: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp38C->topt))->v;void*
new_typ=*_tmp38B;*_tmp38B=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp38C);{
struct _tuple0 _tmp41E=({struct _tuple0 _tmp41D;_tmp41D.f1=Cyc_Tcutil_compress(
old_t2);_tmp41D.f2=Cyc_Tcutil_compress(new_typ);_tmp41D;});void*_tmp41F;struct
Cyc_Absyn_PtrInfo _tmp420;void*_tmp421;struct Cyc_Absyn_PtrInfo _tmp422;void*
_tmp423;struct Cyc_Absyn_PtrInfo _tmp424;void*_tmp425;_LL289: _tmp41F=_tmp41E.f1;
if(_tmp41F <= (void*)4)goto _LL28B;if(*((int*)_tmp41F)!= 4)goto _LL28B;_tmp420=((
struct Cyc_Absyn_PointerType_struct*)_tmp41F)->f1;_tmp421=_tmp41E.f2;if(_tmp421 <= (
void*)4)goto _LL28B;if(*((int*)_tmp421)!= 4)goto _LL28B;_tmp422=((struct Cyc_Absyn_PointerType_struct*)
_tmp421)->f1;_LL28A: {int _tmp426=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp420.ptr_atts).nullable);int _tmp427=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp422.ptr_atts).nullable);void*_tmp428=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp420.ptr_atts).bounds);void*_tmp429=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp422.ptr_atts).bounds);int _tmp42A=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp420.ptr_atts).zero_term);int
_tmp42B=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp422.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp42D=({struct _tuple0
_tmp42C;_tmp42C.f1=_tmp428;_tmp42C.f2=_tmp429;_tmp42C;});void*_tmp42E;struct Cyc_Absyn_Exp*
_tmp42F;void*_tmp430;struct Cyc_Absyn_Exp*_tmp431;void*_tmp432;struct Cyc_Absyn_Exp*
_tmp433;void*_tmp434;void*_tmp435;struct Cyc_Absyn_Exp*_tmp436;void*_tmp437;void*
_tmp438;void*_tmp439;struct Cyc_Absyn_Exp*_tmp43A;void*_tmp43B;void*_tmp43C;
struct Cyc_Absyn_Exp*_tmp43D;void*_tmp43E;void*_tmp43F;void*_tmp440;void*_tmp441;
void*_tmp442;void*_tmp443;void*_tmp444;void*_tmp445;void*_tmp446;void*_tmp447;
void*_tmp448;void*_tmp449;void*_tmp44A;void*_tmp44B;_LL290: _tmp42E=_tmp42D.f1;
if(_tmp42E <= (void*)2)goto _LL292;if(*((int*)_tmp42E)!= 0)goto _LL292;_tmp42F=((
struct Cyc_Absyn_Upper_b_struct*)_tmp42E)->f1;_tmp430=_tmp42D.f2;if(_tmp430 <= (
void*)2)goto _LL292;if(*((int*)_tmp430)!= 0)goto _LL292;_tmp431=((struct Cyc_Absyn_Upper_b_struct*)
_tmp430)->f1;_LL291: if(_tmp426  && !_tmp427){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp44C[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp44D="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp44D,sizeof(char),_get_zero_arr_size(_tmp44D,44));}),_tag_dynforward(_tmp44C,
sizeof(void*),0));});if(_tmp38E != (void*)2)({struct Cyc_String_pa_struct _tmp450;
_tmp450.tag=0;_tmp450.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp44E[1]={& _tmp450};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp44F="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp44F,sizeof(char),_get_zero_arr_size(_tmp44F,41));}),
_tag_dynforward(_tmp44E,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp38C);if(do_null_check){if(!_tmp38D)({void*_tmp451[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp452="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp452,sizeof(char),_get_zero_arr_size(_tmp452,58));}),
_tag_dynforward(_tmp451,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp38B,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp453=
_cycalloc(sizeof(*_tmp453));_tmp453->hd=_tmp38C;_tmp453->tl=0;_tmp453;}),0)));}}}
goto _LL28F;_LL292: _tmp432=_tmp42D.f1;if(_tmp432 <= (void*)2)goto _LL294;if(*((int*)
_tmp432)!= 0)goto _LL294;_tmp433=((struct Cyc_Absyn_Upper_b_struct*)_tmp432)->f1;
_tmp434=_tmp42D.f2;if((int)_tmp434 != 0)goto _LL294;_LL293: p2_forward_only=1;
_tmp436=_tmp433;goto _LL295;_LL294: _tmp435=_tmp42D.f1;if(_tmp435 <= (void*)2)goto
_LL296;if(*((int*)_tmp435)!= 0)goto _LL296;_tmp436=((struct Cyc_Absyn_Upper_b_struct*)
_tmp435)->f1;_tmp437=_tmp42D.f2;if((int)_tmp437 != 1)goto _LL296;_LL295: if(_tmp38E
== (void*)2)({struct Cyc_String_pa_struct _tmp456;_tmp456.tag=0;_tmp456.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp454[
1]={& _tmp456};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp455="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp455,sizeof(char),_get_zero_arr_size(_tmp455,44));}),_tag_dynforward(_tmp454,
sizeof(void*),1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp42A  && !(_tmp422.elt_tq).real_const)
 && !_tmp42B)_tmp436=Cyc_Absyn_prim2_exp((void*)2,_tmp436,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp436,
_tmp38C,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp457=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp42A){struct _tuple1*
_tmp458=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp459=Cyc_Absyn_var_exp(_tmp458,
0);struct Cyc_Absyn_Exp*_tmp45A=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp45F[2];_tmp45F[1]=_tmp436;_tmp45F[0]=_tmp459;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp45F,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp42B  && !(_tmp422.elt_tq).real_const)_tmp45A=Cyc_Absyn_prim2_exp((
void*)2,_tmp45A,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp45B=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp422.elt_typ),0);struct Cyc_Absyn_Exp*_tmp45C=Cyc_Absyn_fncall_exp(
_tmp457,({struct Cyc_Absyn_Exp*_tmp45E[3];_tmp45E[2]=_tmp45A;_tmp45E[1]=_tmp45B;
_tmp45E[0]=_tmp459;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp45E,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp45D=Cyc_Absyn_exp_stmt(_tmp45C,0);_tmp45D=Cyc_Absyn_declare_stmt(_tmp458,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp38C,_tmp45D,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp45D));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp457,({struct Cyc_Absyn_Exp*
_tmp460[3];_tmp460[2]=_tmp436;_tmp460[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp422.elt_typ),0);_tmp460[0]=_tmp38C;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp460,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL28F;_LL296: _tmp438=_tmp42D.f1;if((int)_tmp438 != 0)goto _LL298;
_tmp439=_tmp42D.f2;if(_tmp439 <= (void*)2)goto _LL298;if(*((int*)_tmp439)!= 0)goto
_LL298;_tmp43A=((struct Cyc_Absyn_Upper_b_struct*)_tmp439)->f1;_LL297:
p1_forward_only=1;_tmp43D=_tmp43A;goto _LL299;_LL298: _tmp43B=_tmp42D.f1;if((int)
_tmp43B != 1)goto _LL29A;_tmp43C=_tmp42D.f2;if(_tmp43C <= (void*)2)goto _LL29A;if(*((
int*)_tmp43C)!= 0)goto _LL29A;_tmp43D=((struct Cyc_Absyn_Upper_b_struct*)_tmp43C)->f1;
_LL299: if(Cyc_Toc_is_toplevel(nv))({void*_tmp461[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp462="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp462,sizeof(char),_get_zero_arr_size(_tmp462,45));}),
_tag_dynforward(_tmp461,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp463=
_tmp43D;if(_tmp42A  && !_tmp42B)_tmp463=Cyc_Absyn_add_exp(_tmp43D,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp464=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp465=Cyc_Absyn_fncall_exp(
_tmp464,({struct Cyc_Absyn_Exp*_tmp467[3];_tmp467[2]=_tmp463;_tmp467[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp420.elt_typ),0);_tmp467[0]=_tmp38C;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp467,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp427)(void*)(_tmp465->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->hd=Cyc_Absyn_copy_exp(
_tmp465);_tmp466->tl=0;_tmp466;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp38B,_tmp465));goto _LL28F;}}_LL29A: _tmp43E=_tmp42D.f1;if((int)_tmp43E != 0)
goto _LL29C;_tmp43F=_tmp42D.f2;if((int)_tmp43F != 0)goto _LL29C;_LL29B:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL29C: _tmp440=_tmp42D.f1;if((
int)_tmp440 != 0)goto _LL29E;_tmp441=_tmp42D.f2;if((int)_tmp441 != 1)goto _LL29E;
_LL29D: p1_forward_only=1;goto DynCast;_LL29E: _tmp442=_tmp42D.f1;if((int)_tmp442 != 
1)goto _LL2A0;_tmp443=_tmp42D.f2;if((int)_tmp443 != 0)goto _LL2A0;_LL29F:
p2_forward_only=1;goto DynCast;_LL2A0: _tmp444=_tmp42D.f1;if((int)_tmp444 != 1)goto
_LL2A2;_tmp445=_tmp42D.f2;if((int)_tmp445 != 1)goto _LL2A2;_LL2A1: DynCast: if((
_tmp42A  && !_tmp42B) && !(_tmp422.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({
void*_tmp468[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp469="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp469,sizeof(char),_get_zero_arr_size(_tmp469,70));}),
_tag_dynforward(_tmp468,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp46A=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp46A,({struct Cyc_Absyn_Exp*_tmp46B[3];
_tmp46B[2]=Cyc_Absyn_uint_exp(1,0);_tmp46B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp420.elt_typ),0);_tmp46B[0]=_tmp38C;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp46B,sizeof(struct Cyc_Absyn_Exp*),
3));})));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp46C[1];_tmp46C[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp46C,sizeof(struct Cyc_Absyn_Exp*),
1));})));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp46D[1];_tmp46D[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp46D,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}}else{if(p1_forward_only != p2_forward_only){if(Cyc_Toc_is_toplevel(
nv))({void*_tmp46E[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp46F="can't coerce between ? and ?+- at toplevel";
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size(_tmp46F,43));}),
_tag_dynforward(_tmp46E,sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp470[1];_tmp470[0]=_tmp38C;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp470,sizeof(struct Cyc_Absyn_Exp*),1));})));
else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp471[1];_tmp471[0]=_tmp38C;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp471,sizeof(struct Cyc_Absyn_Exp*),
1));})));}}}goto _LL28F;_LL2A2: _tmp446=_tmp42D.f1;if(_tmp446 <= (void*)2)goto
_LL2A4;if(*((int*)_tmp446)!= 1)goto _LL2A4;_tmp447=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp446)->f1;_tmp448=_tmp42D.f2;if(_tmp448 <= (void*)2)goto _LL2A4;if(*((int*)
_tmp448)!= 1)goto _LL2A4;_tmp449=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp448)->f1;_LL2A3: if(_tmp426  && !_tmp427){if(Cyc_Toc_is_toplevel(nv))({void*
_tmp472[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp473="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp473,sizeof(char),_get_zero_arr_size(_tmp473,44));}),_tag_dynforward(_tmp472,
sizeof(void*),0));});if(_tmp38E != (void*)2)({struct Cyc_String_pa_struct _tmp476;
_tmp476.tag=0;_tmp476.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp474[1]={& _tmp476};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp475="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp475,sizeof(char),_get_zero_arr_size(_tmp475,41));}),
_tag_dynforward(_tmp474,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp38C);if(do_null_check){if(!_tmp38D)({void*_tmp477[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp478="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp478,sizeof(char),_get_zero_arr_size(_tmp478,58));}),
_tag_dynforward(_tmp477,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp38B,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp479=
_cycalloc(sizeof(*_tmp479));_tmp479->hd=_tmp38C;_tmp479->tl=0;_tmp479;}),0)));}}}
goto _LL28F;_LL2A4: _tmp44A=_tmp42D.f1;if(_tmp44A <= (void*)2)goto _LL2A6;if(*((int*)
_tmp44A)!= 1)goto _LL2A6;_LL2A5:({struct Cyc_String_pa_struct _tmp47C;_tmp47C.tag=0;
_tmp47C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp38C->loc));{void*_tmp47A[1]={& _tmp47C};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp47B="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp47B,sizeof(char),_get_zero_arr_size(_tmp47B,30));}),
_tag_dynforward(_tmp47A,sizeof(void*),1));}});_LL2A6: _tmp44B=_tmp42D.f2;if(
_tmp44B <= (void*)2)goto _LL28F;if(*((int*)_tmp44B)!= 1)goto _LL28F;_LL2A7: goto
_LL28F;_LL28F:;}goto _LL288;}_LL28B: _tmp423=_tmp41E.f1;if(_tmp423 <= (void*)4)goto
_LL28D;if(*((int*)_tmp423)!= 4)goto _LL28D;_tmp424=((struct Cyc_Absyn_PointerType_struct*)
_tmp423)->f1;_tmp425=_tmp41E.f2;if(_tmp425 <= (void*)4)goto _LL28D;if(*((int*)
_tmp425)!= 5)goto _LL28D;_LL28C:{void*_tmp47D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp424.ptr_atts).bounds);_LL2A9: if((int)_tmp47D != 0)goto _LL2AB;_LL2AA: goto
_LL2AC;_LL2AB: if((int)_tmp47D != 1)goto _LL2AD;_LL2AC:(void*)(_tmp38C->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp38C->r,_tmp38C->loc),Cyc_Toc_curr_sp));
goto _LL2A8;_LL2AD:;_LL2AE: goto _LL2A8;_LL2A8:;}goto _LL288;_LL28D:;_LL28E: goto
_LL288;_LL288:;}goto _LL1E3;}_LL208: if(*((int*)_tmp365)!= 16)goto _LL20A;_tmp38F=((
struct Cyc_Absyn_Address_e_struct*)_tmp365)->f1;_LL209:{void*_tmp47E=(void*)
_tmp38F->r;struct _tuple1*_tmp47F;struct Cyc_List_List*_tmp480;struct Cyc_List_List*
_tmp481;struct Cyc_List_List*_tmp482;_LL2B0: if(*((int*)_tmp47E)!= 30)goto _LL2B2;
_tmp47F=((struct Cyc_Absyn_Struct_e_struct*)_tmp47E)->f1;_tmp480=((struct Cyc_Absyn_Struct_e_struct*)
_tmp47E)->f2;_tmp481=((struct Cyc_Absyn_Struct_e_struct*)_tmp47E)->f3;_LL2B1: if(
Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp485;_tmp485.tag=0;
_tmp485.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp38F->loc));{void*_tmp483[1]={& _tmp485};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp484="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp484,sizeof(char),_get_zero_arr_size(_tmp484,42));}),
_tag_dynforward(_tmp483,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp38F->topt))->v,_tmp480 != 0,1,0,
_tmp481,_tmp47F))->r));goto _LL2AF;_LL2B2: if(*((int*)_tmp47E)!= 26)goto _LL2B4;
_tmp482=((struct Cyc_Absyn_Tuple_e_struct*)_tmp47E)->f1;_LL2B3: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp488;_tmp488.tag=0;_tmp488.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp38F->loc));{
void*_tmp486[1]={& _tmp488};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp487="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp487,sizeof(char),_get_zero_arr_size(_tmp487,42));}),
_tag_dynforward(_tmp486,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp482))->r));goto _LL2AF;_LL2B4:;_LL2B5: Cyc_Toc_exp_to_c(nv,_tmp38F);if(
!Cyc_Absyn_is_lvalue(_tmp38F)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp38F,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp38F));}goto _LL2AF;
_LL2AF:;}goto _LL1E3;_LL20A: if(*((int*)_tmp365)!= 17)goto _LL20C;_tmp390=((struct
Cyc_Absyn_New_e_struct*)_tmp365)->f1;_tmp391=((struct Cyc_Absyn_New_e_struct*)
_tmp365)->f2;_LL20B: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp48B;_tmp48B.tag=0;_tmp48B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Position_string_of_segment(_tmp391->loc));{void*_tmp489[1]={& _tmp48B};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp48A="%s: new at top-level";_tag_dynforward(_tmp48A,sizeof(char),
_get_zero_arr_size(_tmp48A,21));}),_tag_dynforward(_tmp489,sizeof(void*),1));}});{
void*_tmp48C=(void*)_tmp391->r;struct Cyc_List_List*_tmp48D;struct Cyc_Absyn_Vardecl*
_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;int _tmp491;
struct _tuple1*_tmp492;struct Cyc_List_List*_tmp493;struct Cyc_List_List*_tmp494;
struct Cyc_List_List*_tmp495;_LL2B7: if(*((int*)_tmp48C)!= 28)goto _LL2B9;_tmp48D=((
struct Cyc_Absyn_Array_e_struct*)_tmp48C)->f1;_LL2B8: {struct _tuple1*_tmp496=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp497=Cyc_Absyn_var_exp(_tmp496,0);struct Cyc_Absyn_Stmt*
_tmp498=Cyc_Toc_init_array(nv,_tmp497,_tmp48D,Cyc_Absyn_exp_stmt(_tmp497,0));
void*old_elt_typ;{void*_tmp499=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp49A;void*_tmp49B;struct Cyc_Absyn_Tqual _tmp49C;struct Cyc_Absyn_PtrAtts _tmp49D;
struct Cyc_Absyn_Conref*_tmp49E;_LL2C2: if(_tmp499 <= (void*)4)goto _LL2C4;if(*((int*)
_tmp499)!= 4)goto _LL2C4;_tmp49A=((struct Cyc_Absyn_PointerType_struct*)_tmp499)->f1;
_tmp49B=(void*)_tmp49A.elt_typ;_tmp49C=_tmp49A.elt_tq;_tmp49D=_tmp49A.ptr_atts;
_tmp49E=_tmp49D.zero_term;_LL2C3: old_elt_typ=_tmp49B;goto _LL2C1;_LL2C4:;_LL2C5:
old_elt_typ=({void*_tmp49F[0]={};Cyc_Toc_toc_impos(({const char*_tmp4A0="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp4A0,sizeof(char),_get_zero_arr_size(_tmp4A0,52));}),
_tag_dynforward(_tmp49F,sizeof(void*),0));});_LL2C1:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp4A1=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp4A2=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp48D),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp390 == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp4A2);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp390;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp4A2);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp496,
_tmp4A1,(struct Cyc_Absyn_Exp*)e1,_tmp498,0)));goto _LL2B6;}}_LL2B9: if(*((int*)
_tmp48C)!= 29)goto _LL2BB;_tmp48E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp48C)->f1;_tmp48F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f2;
_tmp490=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f3;_tmp491=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f4;_LL2BA: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp4A3=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4A4;void*_tmp4A5;struct Cyc_Absyn_Tqual _tmp4A6;struct Cyc_Absyn_PtrAtts _tmp4A7;
struct Cyc_Absyn_Conref*_tmp4A8;struct Cyc_Absyn_Conref*_tmp4A9;_LL2C7: if(_tmp4A3
<= (void*)4)goto _LL2C9;if(*((int*)_tmp4A3)!= 4)goto _LL2C9;_tmp4A4=((struct Cyc_Absyn_PointerType_struct*)
_tmp4A3)->f1;_tmp4A5=(void*)_tmp4A4.elt_typ;_tmp4A6=_tmp4A4.elt_tq;_tmp4A7=
_tmp4A4.ptr_atts;_tmp4A8=_tmp4A7.bounds;_tmp4A9=_tmp4A7.zero_term;_LL2C8:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4A8)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4A8)== (void*)1;
goto _LL2C6;_LL2C9:;_LL2CA:({void*_tmp4AA[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4AB="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp4AB,sizeof(char),_get_zero_arr_size(_tmp4AB,42));}),
_tag_dynforward(_tmp4AA,sizeof(void*),0));});_LL2C6:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp490->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp48F);{
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_Absyn_var_exp(max,0);if(_tmp491)_tmp4AC=Cyc_Absyn_add_exp(
_tmp4AC,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp48E,Cyc_Absyn_var_exp(max,0),_tmp490,_tmp491,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp4AD=_new_region("r");struct _RegionHandle*r=& _tmp4AD;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp4BE=
_region_malloc(r,sizeof(*_tmp4BE));_tmp4BE->hd=({struct _tuple9*_tmp4BF=
_region_malloc(r,sizeof(*_tmp4BF));_tmp4BF->f1=max;_tmp4BF->f2=Cyc_Absyn_uint_typ;
_tmp4BF->f3=(struct Cyc_Absyn_Exp*)_tmp48F;_tmp4BF;});_tmp4BE->tl=0;_tmp4BE;});
struct Cyc_Absyn_Exp*ai;if(_tmp390 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp4AE[2];_tmp4AE[1]=_tmp4AC;
_tmp4AE[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4AE,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp390;Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp4AF[2];_tmp4AF[1]=_tmp4AC;_tmp4AF[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4AF,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({struct Cyc_List_List*_tmp4B0=
_region_malloc(r,sizeof(*_tmp4B0));_tmp4B0->hd=({struct _tuple9*_tmp4B1=
_region_malloc(r,sizeof(*_tmp4B1));_tmp4B1->f1=a;_tmp4B1->f2=ptr_typ;_tmp4B1->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp4B1;});_tmp4B0->tl=decls;_tmp4B0;});if(
is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp4B2=Cyc_Toc_temp_var();
void*_tmp4B3=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp4B4=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp4B5=Cyc_Absyn_fncall_exp(_tmp4B4,({struct Cyc_Absyn_Exp*_tmp4B8[3];_tmp4B8[2]=
_tmp4AC;_tmp4B8[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp4B8[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4B8,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp4B6=_region_malloc(r,sizeof(*_tmp4B6));_tmp4B6->hd=({struct _tuple9*_tmp4B7=
_region_malloc(r,sizeof(*_tmp4B7));_tmp4B7->f1=_tmp4B2;_tmp4B7->f2=_tmp4B3;
_tmp4B7->f3=(struct Cyc_Absyn_Exp*)_tmp4B5;_tmp4B7;});_tmp4B6->tl=decls;_tmp4B6;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4B2,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp4B9=decls;for(0;_tmp4B9 != 0;_tmp4B9=_tmp4B9->tl){struct
_tuple1*_tmp4BB;void*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BD;struct _tuple9 _tmp4BA=*((
struct _tuple9*)_tmp4B9->hd);_tmp4BB=_tmp4BA.f1;_tmp4BC=_tmp4BA.f2;_tmp4BD=
_tmp4BA.f3;s=Cyc_Absyn_declare_stmt(_tmp4BB,_tmp4BC,_tmp4BD,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL2B6;}}}}_LL2BB: if(*((int*)
_tmp48C)!= 30)goto _LL2BD;_tmp492=((struct Cyc_Absyn_Struct_e_struct*)_tmp48C)->f1;
_tmp493=((struct Cyc_Absyn_Struct_e_struct*)_tmp48C)->f2;_tmp494=((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f3;_LL2BC:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp391->topt))->v,_tmp493 != 0,1,_tmp390,_tmp494,
_tmp492))->r));goto _LL2B6;_LL2BD: if(*((int*)_tmp48C)!= 26)goto _LL2BF;_tmp495=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp48C)->f1;_LL2BE:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp390,_tmp495))->r));goto _LL2B6;_LL2BF:;_LL2C0: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp391->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp4C0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C1=Cyc_Absyn_var_exp(_tmp4C0,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp4C1,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp390 == 0)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp390;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*_tmp4C2=(void*)
_tmp391->r;void*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C4;_LL2CC: if(*((int*)_tmp4C2)!= 
15)goto _LL2CE;_tmp4C3=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4C2)->f1;
_tmp4C4=((struct Cyc_Absyn_Cast_e_struct*)_tmp4C2)->f2;_LL2CD:{struct _tuple0
_tmp4C6=({struct _tuple0 _tmp4C5;_tmp4C5.f1=Cyc_Tcutil_compress(_tmp4C3);_tmp4C5.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4C4->topt))->v);
_tmp4C5;});void*_tmp4C7;struct Cyc_Absyn_PtrInfo _tmp4C8;void*_tmp4C9;struct Cyc_Absyn_PtrAtts
_tmp4CA;struct Cyc_Absyn_Conref*_tmp4CB;void*_tmp4CC;struct Cyc_Absyn_PtrInfo
_tmp4CD;struct Cyc_Absyn_PtrAtts _tmp4CE;struct Cyc_Absyn_Conref*_tmp4CF;_LL2D1:
_tmp4C7=_tmp4C6.f1;if(_tmp4C7 <= (void*)4)goto _LL2D3;if(*((int*)_tmp4C7)!= 4)goto
_LL2D3;_tmp4C8=((struct Cyc_Absyn_PointerType_struct*)_tmp4C7)->f1;_tmp4C9=(void*)
_tmp4C8.elt_typ;_tmp4CA=_tmp4C8.ptr_atts;_tmp4CB=_tmp4CA.bounds;_tmp4CC=_tmp4C6.f2;
if(_tmp4CC <= (void*)4)goto _LL2D3;if(*((int*)_tmp4CC)!= 4)goto _LL2D3;_tmp4CD=((
struct Cyc_Absyn_PointerType_struct*)_tmp4CC)->f1;_tmp4CE=_tmp4CD.ptr_atts;
_tmp4CF=_tmp4CE.bounds;_LL2D2:{struct _tuple0 _tmp4D1=({struct _tuple0 _tmp4D0;
_tmp4D0.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4CB);_tmp4D0.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp4CF);_tmp4D0;});void*_tmp4D2;void*_tmp4D3;struct Cyc_Absyn_Exp*
_tmp4D4;void*_tmp4D5;void*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D7;_LL2D6: _tmp4D2=
_tmp4D1.f1;if((int)_tmp4D2 != 0)goto _LL2D8;_tmp4D3=_tmp4D1.f2;if(_tmp4D3 <= (void*)
2)goto _LL2D8;if(*((int*)_tmp4D3)!= 0)goto _LL2D8;_tmp4D4=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4D3)->f1;_LL2D7: forward_only=1;_tmp4D7=_tmp4D4;goto _LL2D9;_LL2D8: _tmp4D5=
_tmp4D1.f1;if((int)_tmp4D5 != 1)goto _LL2DA;_tmp4D6=_tmp4D1.f2;if(_tmp4D6 <= (void*)
2)goto _LL2DA;if(*((int*)_tmp4D6)!= 0)goto _LL2DA;_tmp4D7=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4D6)->f1;_LL2D9: Cyc_Toc_exp_to_c(nv,_tmp4C4);(void*)(inner_mexp->r=(void*)
Cyc_Toc_sizeoftyp_exp_r(elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp4D8=
forward_only?Cyc_Toc__init_dynforward_ptr_e: Cyc_Toc__init_dyneither_ptr_e;(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4D8,({struct Cyc_Absyn_Exp*_tmp4D9[4];
_tmp4D9[3]=_tmp4D7;_tmp4D9[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4C9),
0);_tmp4D9[1]=_tmp4C4;_tmp4D9[0]=mexp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4D9,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2D5;}_LL2DA:;_LL2DB: goto _LL2D5;_LL2D5:;}goto _LL2D0;_LL2D3:;
_LL2D4: goto _LL2D0;_LL2D0:;}goto _LL2CB;_LL2CE:;_LL2CF: goto _LL2CB;_LL2CB:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp4DA=Cyc_Absyn_exp_stmt(_tmp4C1,0);struct Cyc_Absyn_Exp*
_tmp4DB=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp391);_tmp4DA=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp4C1,_tmp4DB,0),_tmp391,0),
_tmp4DA,0);{void*_tmp4DC=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4C0,_tmp4DC,(struct Cyc_Absyn_Exp*)
mexp,_tmp4DA,0)));}}goto _LL2B6;}}_LL2B6:;}goto _LL1E3;_LL20C: if(*((int*)_tmp365)
!= 19)goto _LL20E;_tmp392=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp365)->f1;
_LL20D: Cyc_Toc_exp_to_c(nv,_tmp392);goto _LL1E3;_LL20E: if(*((int*)_tmp365)!= 18)
goto _LL210;_tmp393=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp365)->f1;
_LL20F:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4DD=
_cycalloc(sizeof(*_tmp4DD));_tmp4DD[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp4DE;_tmp4DE.tag=18;_tmp4DE.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp393);_tmp4DE;});
_tmp4DD;})));goto _LL1E3;_LL210: if(*((int*)_tmp365)!= 21)goto _LL212;_LL211:({void*
_tmp4DF[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4E0="__gen() in code generator";_tag_dynforward(_tmp4E0,sizeof(
char),_get_zero_arr_size(_tmp4E0,26));}),_tag_dynforward(_tmp4DF,sizeof(void*),0));});
_LL212: if(*((int*)_tmp365)!= 20)goto _LL214;_tmp394=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp365)->f1;_tmp395=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp365)->f2;
if(*((int*)_tmp395)!= 0)goto _LL214;_tmp396=((struct Cyc_Absyn_StructField_struct*)
_tmp395)->f1;_LL213:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp4E2;_tmp4E2.tag=20;_tmp4E2.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp394);_tmp4E2.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4E3=_cycalloc(sizeof(*
_tmp4E3));_tmp4E3[0]=({struct Cyc_Absyn_StructField_struct _tmp4E4;_tmp4E4.tag=0;
_tmp4E4.f1=_tmp396;_tmp4E4;});_tmp4E3;}));_tmp4E2;});_tmp4E1;})));goto _LL1E3;
_LL214: if(*((int*)_tmp365)!= 20)goto _LL216;_tmp397=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp365)->f1;_tmp398=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp365)->f2;
if(*((int*)_tmp398)!= 1)goto _LL216;_tmp399=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp398)->f1;_LL215:{void*_tmp4E5=Cyc_Tcutil_compress(_tmp397);struct Cyc_Absyn_AggrInfo
_tmp4E6;union Cyc_Absyn_AggrInfoU_union _tmp4E7;struct Cyc_List_List*_tmp4E8;_LL2DD:
if(_tmp4E5 <= (void*)4)goto _LL2E5;if(*((int*)_tmp4E5)!= 10)goto _LL2DF;_tmp4E6=((
struct Cyc_Absyn_AggrType_struct*)_tmp4E5)->f1;_tmp4E7=_tmp4E6.aggr_info;_LL2DE: {
struct Cyc_Absyn_Aggrdecl*_tmp4E9=Cyc_Absyn_get_known_aggrdecl(_tmp4E7);if(
_tmp4E9->impl == 0)({void*_tmp4EA[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4EB="struct fields must be known";
_tag_dynforward(_tmp4EB,sizeof(char),_get_zero_arr_size(_tmp4EB,28));}),
_tag_dynforward(_tmp4EA,sizeof(void*),0));});_tmp4E8=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4E9->impl))->fields;goto _LL2E0;}_LL2DF: if(*((int*)_tmp4E5)!= 11)
goto _LL2E1;_tmp4E8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4E5)->f2;_LL2E0: {
struct Cyc_Absyn_Aggrfield*_tmp4EC=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp4E8,(int)_tmp399);(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Offsetof_e_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp4EE;_tmp4EE.tag=20;_tmp4EE.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp397);_tmp4EE.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4EF=
_cycalloc(sizeof(*_tmp4EF));_tmp4EF[0]=({struct Cyc_Absyn_StructField_struct
_tmp4F0;_tmp4F0.tag=0;_tmp4F0.f1=_tmp4EC->name;_tmp4F0;});_tmp4EF;}));_tmp4EE;});
_tmp4ED;})));goto _LL2DC;}_LL2E1: if(*((int*)_tmp4E5)!= 9)goto _LL2E3;_LL2E2:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4F1=_cycalloc(
sizeof(*_tmp4F1));_tmp4F1[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4F2;_tmp4F2.tag=
20;_tmp4F2.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp397);_tmp4F2.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=({
struct Cyc_Absyn_StructField_struct _tmp4F4;_tmp4F4.tag=0;_tmp4F4.f1=Cyc_Absyn_fieldname((
int)(_tmp399 + 1));_tmp4F4;});_tmp4F3;}));_tmp4F2;});_tmp4F1;})));goto _LL2DC;
_LL2E3: if(*((int*)_tmp4E5)!= 3)goto _LL2E5;_LL2E4: if(_tmp399 == 0)(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp4F5=_cycalloc(sizeof(*
_tmp4F5));_tmp4F5[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp4F6;_tmp4F6.tag=20;
_tmp4F6.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp397);_tmp4F6.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7[0]=({
struct Cyc_Absyn_StructField_struct _tmp4F8;_tmp4F8.tag=0;_tmp4F8.f1=Cyc_Toc_tag_sp;
_tmp4F8;});_tmp4F7;}));_tmp4F6;});_tmp4F5;})));else{(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp4FA;_tmp4FA.tag=20;_tmp4FA.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp397);_tmp4FA.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp4FB=
_cycalloc(sizeof(*_tmp4FB));_tmp4FB[0]=({struct Cyc_Absyn_StructField_struct
_tmp4FC;_tmp4FC.tag=0;_tmp4FC.f1=Cyc_Absyn_fieldname((int)_tmp399);_tmp4FC;});
_tmp4FB;}));_tmp4FA;});_tmp4F9;})));}goto _LL2DC;_LL2E5:;_LL2E6:({void*_tmp4FD[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4FE="impossible type for offsetof tuple index";_tag_dynforward(
_tmp4FE,sizeof(char),_get_zero_arr_size(_tmp4FE,41));}),_tag_dynforward(_tmp4FD,
sizeof(void*),0));});_LL2DC:;}goto _LL1E3;_LL216: if(*((int*)_tmp365)!= 22)goto
_LL218;_tmp39A=((struct Cyc_Absyn_Deref_e_struct*)_tmp365)->f1;_LL217: {void*
_tmp4FF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39A->topt))->v);{
void*_tmp500=_tmp4FF;struct Cyc_Absyn_PtrInfo _tmp501;void*_tmp502;struct Cyc_Absyn_Tqual
_tmp503;struct Cyc_Absyn_PtrAtts _tmp504;void*_tmp505;struct Cyc_Absyn_Conref*
_tmp506;struct Cyc_Absyn_Conref*_tmp507;struct Cyc_Absyn_Conref*_tmp508;_LL2E8: if(
_tmp500 <= (void*)4)goto _LL2EA;if(*((int*)_tmp500)!= 4)goto _LL2EA;_tmp501=((
struct Cyc_Absyn_PointerType_struct*)_tmp500)->f1;_tmp502=(void*)_tmp501.elt_typ;
_tmp503=_tmp501.elt_tq;_tmp504=_tmp501.ptr_atts;_tmp505=(void*)_tmp504.rgn;
_tmp506=_tmp504.nullable;_tmp507=_tmp504.bounds;_tmp508=_tmp504.zero_term;_LL2E9:{
void*_tmp509=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp507);_LL2ED: if(
_tmp509 <= (void*)2)goto _LL2EF;if(*((int*)_tmp509)!= 0)goto _LL2EF;_LL2EE: {int
do_null_check=Cyc_Toc_need_null_check(_tmp39A);Cyc_Toc_exp_to_c(nv,_tmp39A);if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp50A[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp50B="inserted null check due to dereference";
_tag_dynforward(_tmp50B,sizeof(char),_get_zero_arr_size(_tmp50B,39));}),
_tag_dynforward(_tmp50A,sizeof(void*),0));});(void*)(_tmp39A->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Toc_typ_to_c(_tmp4FF),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->hd=Cyc_Absyn_copy_exp(_tmp39A);
_tmp50C->tl=0;_tmp50C;}),0)));}goto _LL2EC;}_LL2EF: if((int)_tmp509 != 0)goto _LL2F1;
_LL2F0: goto _LL2F2;_LL2F1: if((int)_tmp509 != 1)goto _LL2F3;_LL2F2: {struct Cyc_Absyn_Exp*
_tmp50D=Cyc_Absyn_uint_exp(0,0);_tmp50D->topt=({struct Cyc_Core_Opt*_tmp50E=
_cycalloc(sizeof(*_tmp50E));_tmp50E->v=(void*)Cyc_Absyn_uint_typ;_tmp50E;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp39A,_tmp50D));Cyc_Toc_exp_to_c(nv,
e);goto _LL2EC;}_LL2F3: if(_tmp509 <= (void*)2)goto _LL2EC;if(*((int*)_tmp509)!= 1)
goto _LL2EC;_LL2F4:({void*_tmp50F[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp510="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp510,sizeof(char),_get_zero_arr_size(_tmp510,30));}),
_tag_dynforward(_tmp50F,sizeof(void*),0));});_LL2EC:;}goto _LL2E7;_LL2EA:;_LL2EB:({
void*_tmp511[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp512="exp_to_c: Deref: non-pointer";
_tag_dynforward(_tmp512,sizeof(char),_get_zero_arr_size(_tmp512,29));}),
_tag_dynforward(_tmp511,sizeof(void*),0));});_LL2E7:;}goto _LL1E3;}_LL218: if(*((
int*)_tmp365)!= 23)goto _LL21A;_tmp39B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp365)->f1;_tmp39C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp365)->f2;_LL219:
Cyc_Toc_exp_to_c(nv,_tmp39B);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E3;
_LL21A: if(*((int*)_tmp365)!= 24)goto _LL21C;_tmp39D=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp365)->f1;_tmp39E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp365)->f2;_LL21B: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39D->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp39D);Cyc_Toc_exp_to_c(nv,_tmp39D);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp514;struct Cyc_Absyn_Tqual
_tmp515;struct Cyc_Absyn_PtrAtts _tmp516;void*_tmp517;struct Cyc_Absyn_Conref*
_tmp518;struct Cyc_Absyn_Conref*_tmp519;struct Cyc_Absyn_Conref*_tmp51A;struct Cyc_Absyn_PtrInfo
_tmp513=Cyc_Toc_get_ptr_type(e1typ);_tmp514=(void*)_tmp513.elt_typ;_tmp515=
_tmp513.elt_tq;_tmp516=_tmp513.ptr_atts;_tmp517=(void*)_tmp516.rgn;_tmp518=
_tmp516.nullable;_tmp519=_tmp516.bounds;_tmp51A=_tmp516.zero_term;{void*_tmp51B=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp519);struct Cyc_Absyn_Exp*_tmp51C;
_LL2F6: if(_tmp51B <= (void*)2)goto _LL2F8;if(*((int*)_tmp51B)!= 0)goto _LL2F8;
_tmp51C=((struct Cyc_Absyn_Upper_b_struct*)_tmp51B)->f1;_LL2F7: {unsigned int
_tmp51E;int _tmp51F;struct _tuple5 _tmp51D=Cyc_Evexp_eval_const_uint_exp(_tmp51C);
_tmp51E=_tmp51D.f1;_tmp51F=_tmp51D.f2;if(_tmp51F){if(_tmp51E < 1)({void*_tmp520[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp521="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp521,sizeof(char),_get_zero_arr_size(_tmp521,44));}),_tag_dynforward(_tmp520,
sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*
_tmp522[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp523="inserted null check due to dereference";
_tag_dynforward(_tmp523,sizeof(char),_get_zero_arr_size(_tmp523,39));}),
_tag_dynforward(_tmp522,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp39D->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->hd=_tmp39D;_tmp524->tl=0;_tmp524;}),
0)),_tmp39E));}}else{(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39D->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*
_tmp525[4];_tmp525[3]=Cyc_Absyn_uint_exp(0,0);_tmp525[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp514,0);_tmp525[1]=_tmp51C;_tmp525[0]=_tmp39D;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp525,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp39E));}goto _LL2F5;}_LL2F8: if((int)_tmp51B != 0)goto _LL2FA;_LL2F9:
forward_only=1;goto _LL2FB;_LL2FA: if((int)_tmp51B != 1)goto _LL2FC;_LL2FB: {struct
Cyc_Absyn_Exp*_tmp526=forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;
void*ta1=Cyc_Toc_typ_to_c_array(_tmp514);(void*)(_tmp39D->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp515),Cyc_Absyn_fncall_exp(_tmp526,({struct Cyc_Absyn_Exp*
_tmp527[3];_tmp527[2]=Cyc_Absyn_uint_exp(0,0);_tmp527[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp527[0]=Cyc_Absyn_copy_exp(_tmp39D);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp527,sizeof(struct Cyc_Absyn_Exp*),
3));}),0)));goto _LL2F5;}_LL2FC: if(_tmp51B <= (void*)2)goto _LL2F5;if(*((int*)
_tmp51B)!= 1)goto _LL2F5;_LL2FD:({void*_tmp528[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp529="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp529,sizeof(char),_get_zero_arr_size(_tmp529,34));}),
_tag_dynforward(_tmp528,sizeof(void*),0));});_LL2F5:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E3;}}
_LL21C: if(*((int*)_tmp365)!= 25)goto _LL21E;_tmp39F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp365)->f1;_tmp3A0=((struct Cyc_Absyn_Subscript_e_struct*)_tmp365)->f2;_LL21D: {
void*_tmp52A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp39F->topt))->v);{void*_tmp52B=_tmp52A;struct Cyc_List_List*_tmp52C;struct Cyc_Absyn_PtrInfo
_tmp52D;void*_tmp52E;struct Cyc_Absyn_Tqual _tmp52F;struct Cyc_Absyn_PtrAtts _tmp530;
void*_tmp531;struct Cyc_Absyn_Conref*_tmp532;struct Cyc_Absyn_Conref*_tmp533;
struct Cyc_Absyn_Conref*_tmp534;_LL2FF: if(_tmp52B <= (void*)4)goto _LL303;if(*((int*)
_tmp52B)!= 9)goto _LL301;_tmp52C=((struct Cyc_Absyn_TupleType_struct*)_tmp52B)->f1;
_LL300: Cyc_Toc_exp_to_c(nv,_tmp39F);Cyc_Toc_exp_to_c(nv,_tmp3A0);{unsigned int
_tmp536;int _tmp537;struct _tuple5 _tmp535=Cyc_Evexp_eval_const_uint_exp(_tmp3A0);
_tmp536=_tmp535.f1;_tmp537=_tmp535.f2;if(!_tmp537)({void*_tmp538[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp539="unknown tuple subscript in translation to C";_tag_dynforward(_tmp539,
sizeof(char),_get_zero_arr_size(_tmp539,44));}),_tag_dynforward(_tmp538,sizeof(
void*),0));});(void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp39F,Cyc_Absyn_fieldname((
int)(_tmp536 + 1))));goto _LL2FE;}_LL301: if(*((int*)_tmp52B)!= 4)goto _LL303;
_tmp52D=((struct Cyc_Absyn_PointerType_struct*)_tmp52B)->f1;_tmp52E=(void*)
_tmp52D.elt_typ;_tmp52F=_tmp52D.elt_tq;_tmp530=_tmp52D.ptr_atts;_tmp531=(void*)
_tmp530.rgn;_tmp532=_tmp530.nullable;_tmp533=_tmp530.bounds;_tmp534=_tmp530.zero_term;
_LL302: {struct Cyc_List_List*_tmp53A=Cyc_Toc_get_relns(_tmp39F);int in_bnds=0;{
void*_tmp53B=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp533);_LL306: if(
_tmp53B <= (void*)2)goto _LL308;if(*((int*)_tmp53B)!= 1)goto _LL308;_LL307: goto
_LL305;_LL308:;_LL309: in_bnds=Cyc_Toc_check_bounds(_tmp53A,_tmp39F,_tmp3A0);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp53E;
_tmp53E.tag=0;_tmp53E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp53C[1]={& _tmp53E};Cyc_Tcutil_warn(e->loc,({const char*_tmp53D="bounds check necessary for %s";
_tag_dynforward(_tmp53D,sizeof(char),_get_zero_arr_size(_tmp53D,30));}),
_tag_dynforward(_tmp53C,sizeof(void*),1));}});_LL305:;}Cyc_Toc_exp_to_c(nv,
_tmp39F);Cyc_Toc_exp_to_c(nv,_tmp3A0);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp53F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp533);
struct Cyc_Absyn_Exp*_tmp540;void*_tmp541;_LL30B: if(_tmp53F <= (void*)2)goto _LL30D;
if(*((int*)_tmp53F)!= 0)goto _LL30D;_tmp540=((struct Cyc_Absyn_Upper_b_struct*)
_tmp53F)->f1;_LL30C: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp532);void*ta1=Cyc_Toc_typ_to_c(_tmp52E);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp52F);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp534))(void*)(e->r=(void*)
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp542[3];_tmp542[2]=_tmp3A0;_tmp542[1]=_tmp540;_tmp542[0]=
_tmp39F;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp542,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));else{if(
possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp543[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp544="inserted null check due to dereference";
_tag_dynforward(_tmp544,sizeof(char),_get_zero_arr_size(_tmp544,39));}),
_tag_dynforward(_tmp543,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp545[4];_tmp545[3]=_tmp3A0;_tmp545[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp545[1]=_tmp540;_tmp545[0]=_tmp39F;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp545,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))));}else{(void*)(_tmp3A0->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp546[2];_tmp546[1]=Cyc_Absyn_copy_exp(_tmp3A0);_tmp546[0]=
_tmp540;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp546,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL30A;}
_LL30D: if((int)_tmp53F != 0)goto _LL30F;_LL30E: forward_only=1;goto _LL310;_LL30F:
if((int)_tmp53F != 1)goto _LL311;_LL310: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp52E);
if(in_bnds){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp52F),Cyc_Absyn_aggrmember_exp(
_tmp39F,Cyc_Toc_curr_sp,0)),_tmp3A0));}else{struct Cyc_Absyn_Exp*_tmp547=
forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp52F),Cyc_Absyn_fncall_exp(_tmp547,({struct Cyc_Absyn_Exp*_tmp548[3];_tmp548[2]=
_tmp3A0;_tmp548[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp548[0]=_tmp39F;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp548,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL30A;}_LL311: if(_tmp53F <= (void*)2)goto _LL30A;if(*((int*)
_tmp53F)!= 1)goto _LL30A;_tmp541=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp53F)->f1;_LL312:{void*_tmp549=(void*)_tmp3A0->annot;struct Cyc_List_List*
_tmp54A;_LL314: if(*((void**)_tmp549)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL316;
_tmp54A=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp549)->f1;_LL315: for(0;
_tmp54A != 0;_tmp54A=_tmp54A->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp54A->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp54A->hd)->bd,_tmp541)== 0){in_bnds=1;break;}}goto _LL313;_LL316:;_LL317: goto
_LL313;_LL313:;}if(!in_bnds)({void*_tmp54B[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp54C="cannot determine that subscript is in bounds";_tag_dynforward(
_tmp54C,sizeof(char),_get_zero_arr_size(_tmp54C,45));}),_tag_dynforward(_tmp54B,
sizeof(void*),0));});goto _LL30A;_LL30A:;}goto _LL2FE;}}_LL303:;_LL304:({void*
_tmp54D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp54E="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp54E,sizeof(char),_get_zero_arr_size(_tmp54E,49));}),
_tag_dynforward(_tmp54D,sizeof(void*),0));});_LL2FE:;}goto _LL1E3;}_LL21E: if(*((
int*)_tmp365)!= 26)goto _LL220;_tmp3A1=((struct Cyc_Absyn_Tuple_e_struct*)_tmp365)->f1;
_LL21F: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp3A1))->r));else{struct Cyc_List_List*_tmp54F=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp3A1);struct _dynforward_ptr*_tmp550=Cyc_Toc_add_tuple_type(
_tmp54F);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp3A1 != 0;(_tmp3A1=_tmp3A1->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp3A1->hd);dles=({struct Cyc_List_List*
_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->hd=({struct _tuple6*_tmp552=
_cycalloc(sizeof(*_tmp552));_tmp552->f1=0;_tmp552->f2=(struct Cyc_Absyn_Exp*)
_tmp3A1->hd;_tmp552;});_tmp551->tl=dles;_tmp551;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,dles));}goto _LL1E3;_LL220: if(*((int*)_tmp365)!= 28)goto _LL222;_tmp3A2=((struct
Cyc_Absyn_Array_e_struct*)_tmp365)->f1;_LL221:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp3A2));{struct Cyc_List_List*_tmp553=_tmp3A2;for(0;_tmp553 != 0;_tmp553=
_tmp553->tl){struct _tuple6 _tmp555;struct Cyc_Absyn_Exp*_tmp556;struct _tuple6*
_tmp554=(struct _tuple6*)_tmp553->hd;_tmp555=*_tmp554;_tmp556=_tmp555.f2;Cyc_Toc_exp_to_c(
nv,_tmp556);}}goto _LL1E3;_LL222: if(*((int*)_tmp365)!= 29)goto _LL224;_tmp3A3=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp365)->f1;_tmp3A4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp365)->f2;_tmp3A5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp365)->f3;
_tmp3A6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp365)->f4;_LL223: {
unsigned int _tmp558;int _tmp559;struct _tuple5 _tmp557=Cyc_Evexp_eval_const_uint_exp(
_tmp3A4);_tmp558=_tmp557.f1;_tmp559=_tmp557.f2;{void*_tmp55A=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A5->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3A5);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp3A5)){if(!_tmp559)({
void*_tmp55B[0]={};Cyc_Tcutil_terr(_tmp3A4->loc,({const char*_tmp55C="cannot determine value of constant";
_tag_dynforward(_tmp55C,sizeof(char),_get_zero_arr_size(_tmp55C,35));}),
_tag_dynforward(_tmp55B,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp558;i
++){es=({struct Cyc_List_List*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->hd=({
struct _tuple6*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->f1=0;_tmp55E->f2=
_tmp3A5;_tmp55E;});_tmp55D->tl=es;_tmp55D;});}}if(_tmp3A6){struct Cyc_Absyn_Exp*
_tmp55F=Cyc_Toc_cast_it(_tmp55A,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->hd=({struct _tuple6*_tmp561=
_cycalloc(sizeof(*_tmp561));_tmp561->f1=0;_tmp561->f2=_tmp55F;_tmp561;});_tmp560->tl=
0;_tmp560;}));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,es));goto
_LL1E3;}}}_LL224: if(*((int*)_tmp365)!= 30)goto _LL226;_tmp3A7=((struct Cyc_Absyn_Struct_e_struct*)
_tmp365)->f1;_tmp3A8=((struct Cyc_Absyn_Struct_e_struct*)_tmp365)->f2;_tmp3A9=((
struct Cyc_Absyn_Struct_e_struct*)_tmp365)->f3;_tmp3AA=((struct Cyc_Absyn_Struct_e_struct*)
_tmp365)->f4;_LL225: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp3A8 != 0,0,0,_tmp3A9,_tmp3A7))->r));else{if(_tmp3AA == 0)({void*
_tmp562[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp563="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp563,
sizeof(char),_get_zero_arr_size(_tmp563,35));}),_tag_dynforward(_tmp562,sizeof(
void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp3AA;
struct _RegionHandle _tmp564=_new_region("rgn");struct _RegionHandle*rgn=& _tmp564;
_push_region(rgn);{struct Cyc_List_List*_tmp565=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp3A9,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp566=0;struct Cyc_List_List*_tmp567=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp567 != 0;_tmp567=_tmp567->tl){struct Cyc_List_List*
_tmp568=_tmp565;for(0;_tmp568 != 0;_tmp568=_tmp568->tl){if((*((struct _tuple10*)
_tmp568->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp567->hd){struct _tuple10
_tmp56A;struct Cyc_Absyn_Aggrfield*_tmp56B;struct Cyc_Absyn_Exp*_tmp56C;struct
_tuple10*_tmp569=(struct _tuple10*)_tmp568->hd;_tmp56A=*_tmp569;_tmp56B=_tmp56A.f1;
_tmp56C=_tmp56A.f2;{void*_tmp56D=(void*)_tmp56B->type;Cyc_Toc_exp_to_c(nv,
_tmp56C);if(Cyc_Toc_is_void_star(_tmp56D))(void*)(_tmp56C->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp56C->r,0)));_tmp566=({
struct Cyc_List_List*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->hd=({struct
_tuple6*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->f1=0;_tmp56F->f2=_tmp56C;
_tmp56F;});_tmp56E->tl=_tmp566;_tmp56E;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp566)));};
_pop_region(rgn);}}goto _LL1E3;_LL226: if(*((int*)_tmp365)!= 31)goto _LL228;_tmp3AB=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp365)->f1;_tmp3AC=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp365)->f2;_LL227: {struct Cyc_List_List*fs;{void*_tmp570=Cyc_Tcutil_compress(
_tmp3AB);struct Cyc_List_List*_tmp571;_LL319: if(_tmp570 <= (void*)4)goto _LL31B;if(*((
int*)_tmp570)!= 11)goto _LL31B;_tmp571=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp570)->f2;_LL31A: fs=_tmp571;goto _LL318;_LL31B:;_LL31C:({struct Cyc_String_pa_struct
_tmp574;_tmp574.tag=0;_tmp574.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(_tmp3AB));{void*_tmp572[1]={& _tmp574};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp573="anon struct has type %s";_tag_dynforward(_tmp573,sizeof(char),
_get_zero_arr_size(_tmp573,24));}),_tag_dynforward(_tmp572,sizeof(void*),1));}});
_LL318:;}{struct _RegionHandle _tmp575=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp575;_push_region(rgn);{struct Cyc_List_List*_tmp576=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,
_tmp3AC,fs);for(0;_tmp576 != 0;_tmp576=_tmp576->tl){struct _tuple10 _tmp578;struct
Cyc_Absyn_Aggrfield*_tmp579;struct Cyc_Absyn_Exp*_tmp57A;struct _tuple10*_tmp577=(
struct _tuple10*)_tmp576->hd;_tmp578=*_tmp577;_tmp579=_tmp578.f1;_tmp57A=_tmp578.f2;{
void*_tmp57B=(void*)_tmp579->type;Cyc_Toc_exp_to_c(nv,_tmp57A);if(Cyc_Toc_is_void_star(
_tmp57B))(void*)(_tmp57A->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp57A->r,0)));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp3AC));};_pop_region(rgn);}goto _LL1E3;}_LL228: if(*((int*)_tmp365)!= 32)goto
_LL22A;_tmp3AD=((struct Cyc_Absyn_Tunion_e_struct*)_tmp365)->f1;if(_tmp3AD != 0)
goto _LL22A;_tmp3AE=((struct Cyc_Absyn_Tunion_e_struct*)_tmp365)->f2;_tmp3AF=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp365)->f3;if(!(!_tmp3AE->is_flat))goto _LL22A;
_LL229: {struct _tuple1*qv=_tmp3AF->name;struct Cyc_Absyn_Exp*tag_exp=_tmp3AE->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp3AE,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1E3;}_LL22A: if(*((int*)_tmp365)!= 32)goto _LL22C;
_tmp3B0=((struct Cyc_Absyn_Tunion_e_struct*)_tmp365)->f1;_tmp3B1=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp365)->f2;_tmp3B2=((struct Cyc_Absyn_Tunion_e_struct*)_tmp365)->f3;_LL22B: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp57C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp57D=Cyc_Absyn_var_exp(_tmp57C,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp3B1->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp3B1->name,({const char*_tmp57E="_union";_tag_dynforward(_tmp57E,sizeof(char),
_get_zero_arr_size(_tmp57E,7));})));tag_exp=Cyc_Toc_tunion_tag(_tmp3B1,_tmp3B2->name,
1);{struct _tuple1 _tmp580;struct _dynforward_ptr*_tmp581;struct _tuple1*_tmp57F=
_tmp3B2->name;_tmp580=*_tmp57F;_tmp581=_tmp580.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp57D,_tmp581,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3B2->name,({const char*_tmp582="_struct";_tag_dynforward(_tmp582,sizeof(char),
_get_zero_arr_size(_tmp582,8));})));tag_exp=_tmp3B1->is_xtunion?Cyc_Absyn_var_exp(
_tmp3B2->name,0): Cyc_Toc_tunion_tag(_tmp3B1,_tmp3B2->name,1);member_exp=_tmp57D;}{
struct Cyc_List_List*_tmp583=_tmp3B2->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*
dles=0;for(0;_tmp3B0 != 0;(_tmp3B0=_tmp3B0->tl,_tmp583=_tmp583->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp3B0->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp583))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->hd=({
struct _tuple6*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->f1=0;_tmp585->f2=
cur_e;_tmp585;});_tmp584->tl=dles;_tmp584;});}dles=({struct Cyc_List_List*_tmp586=
_cycalloc(sizeof(*_tmp586));_tmp586->hd=({struct _tuple6*_tmp587=_cycalloc(
sizeof(*_tmp587));_tmp587->f1=0;_tmp587->f2=tag_exp;_tmp587;});_tmp586->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp586;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp3B1->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp3B2->name,({const char*_tmp588="_struct";_tag_dynforward(_tmp588,sizeof(char),
_get_zero_arr_size(_tmp588,8));}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*
_tmp589=({struct Cyc_List_List*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->hd=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),
tag_exp,0);_tmp58F->tl=0;_tmp58F;});{int i=1;for(0;_tmp3B0 != 0;(((_tmp3B0=_tmp3B0->tl,
i ++)),_tmp583=_tmp583->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp3B0->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp583))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp58A=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);_tmp589=({struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*_tmp58B));
_tmp58B->hd=_tmp58A;_tmp58B->tl=_tmp589;_tmp58B;});}}}{struct Cyc_Absyn_Stmt*
_tmp58C=Cyc_Absyn_exp_stmt(_tmp57D,0);struct Cyc_Absyn_Stmt*_tmp58D=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E->hd=_tmp58C;_tmp58E->tl=_tmp589;
_tmp58E;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(
_tmp57C,tunion_ctype,0,_tmp58D,0)));}}goto _LL1E3;}}_LL22C: if(*((int*)_tmp365)!= 
33)goto _LL22E;_LL22D: goto _LL22F;_LL22E: if(*((int*)_tmp365)!= 34)goto _LL230;
_LL22F: goto _LL1E3;_LL230: if(*((int*)_tmp365)!= 35)goto _LL232;_tmp3B3=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp365)->f1;_tmp3B4=_tmp3B3.is_calloc;_tmp3B5=_tmp3B3.rgn;_tmp3B6=_tmp3B3.elt_type;
_tmp3B7=_tmp3B3.num_elts;_tmp3B8=_tmp3B3.fat_result;_LL231: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp3B6)));Cyc_Toc_exp_to_c(nv,_tmp3B7);if(_tmp3B8){struct
_tuple1*_tmp590=Cyc_Toc_temp_var();struct _tuple1*_tmp591=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp3B4){xexp=_tmp3B7;if(_tmp3B5 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp3B5;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp590,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp3B6,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp590,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp592[3];_tmp592[2]=Cyc_Absyn_var_exp(_tmp590,0);_tmp592[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp592[0]=Cyc_Absyn_var_exp(_tmp591,0);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp592,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp3B7,0);if(_tmp3B5 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp3B5;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp590,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp3B6,Cyc_Absyn_var_exp(_tmp590,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp593[3];_tmp593[2]=Cyc_Absyn_var_exp(_tmp590,0);_tmp593[1]=Cyc_Absyn_uint_exp(
1,0);_tmp593[0]=Cyc_Absyn_var_exp(_tmp591,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp593,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp594=Cyc_Absyn_declare_stmt(_tmp590,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp591,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp594));}}else{if(_tmp3B5 != 0){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp3B5;Cyc_Toc_exp_to_c(nv,rgn);(void*)(e->r=(void*)((
void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}else{(void*)(
e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp3B6,Cyc_Absyn_sizeoftyp_exp(t_c,0)))->r));}}
goto _LL1E3;}_LL232: if(*((int*)_tmp365)!= 37)goto _LL234;_tmp3B9=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp365)->f1;_LL233: Cyc_Toc_stmt_to_c(nv,_tmp3B9);goto _LL1E3;_LL234: if(*((int*)
_tmp365)!= 36)goto _LL236;_LL235:({void*_tmp595[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp596="UnresolvedMem";_tag_dynforward(_tmp596,sizeof(char),_get_zero_arr_size(
_tmp596,14));}),_tag_dynforward(_tmp595,sizeof(void*),0));});_LL236: if(*((int*)
_tmp365)!= 27)goto _LL1E3;_LL237:({void*_tmp597[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp598="compoundlit";
_tag_dynforward(_tmp598,sizeof(char),_get_zero_arr_size(_tmp598,12));}),
_tag_dynforward(_tmp597,sizeof(void*),0));});_LL1E3:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple11{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple12{struct _tuple1*f1;void*f2;};struct
_tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp599=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp59A;struct Cyc_Absyn_Vardecl
_tmp59B;struct _tuple1*_tmp59C;struct Cyc_Absyn_Pat*_tmp59D;struct Cyc_Absyn_Vardecl*
_tmp59E;struct Cyc_Absyn_Vardecl _tmp59F;struct _tuple1*_tmp5A0;struct Cyc_Absyn_Vardecl*
_tmp5A1;struct Cyc_Absyn_Pat*_tmp5A2;void*_tmp5A3;int _tmp5A4;char _tmp5A5;struct
_dynforward_ptr _tmp5A6;struct Cyc_Absyn_Enumdecl*_tmp5A7;struct Cyc_Absyn_Enumfield*
_tmp5A8;void*_tmp5A9;struct Cyc_Absyn_Enumfield*_tmp5AA;struct Cyc_Absyn_Tuniondecl*
_tmp5AB;struct Cyc_Absyn_Tunionfield*_tmp5AC;struct Cyc_List_List*_tmp5AD;struct
Cyc_Absyn_Pat*_tmp5AE;struct Cyc_Absyn_Pat _tmp5AF;void*_tmp5B0;struct Cyc_Absyn_Tuniondecl*
_tmp5B1;struct Cyc_Absyn_Tunionfield*_tmp5B2;struct Cyc_List_List*_tmp5B3;struct
Cyc_Absyn_Tuniondecl*_tmp5B4;struct Cyc_Absyn_Tunionfield*_tmp5B5;struct Cyc_List_List*
_tmp5B6;struct Cyc_List_List*_tmp5B7;struct Cyc_List_List*_tmp5B8;struct Cyc_Absyn_AggrInfo
_tmp5B9;union Cyc_Absyn_AggrInfoU_union _tmp5BA;struct Cyc_List_List*_tmp5BB;struct
Cyc_Absyn_Pat*_tmp5BC;_LL31E: if(_tmp599 <= (void*)2)goto _LL322;if(*((int*)_tmp599)
!= 0)goto _LL320;_tmp59A=((struct Cyc_Absyn_Var_p_struct*)_tmp599)->f1;_tmp59B=*
_tmp59A;_tmp59C=_tmp59B.name;_tmp59D=((struct Cyc_Absyn_Var_p_struct*)_tmp599)->f2;
_LL31F: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp59C,r),rgn,t,r,path,
_tmp59D,tag_fail_stmt,fail_stmt,decls);_LL320: if(*((int*)_tmp599)!= 2)goto _LL322;
_tmp59E=((struct Cyc_Absyn_TagInt_p_struct*)_tmp599)->f2;_tmp59F=*_tmp59E;_tmp5A0=
_tmp59F.name;_LL321: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp5A0,r);goto _LL323;_LL322:
if((int)_tmp599 != 0)goto _LL324;_LL323: s=Cyc_Toc_skip_stmt_dl();goto _LL31D;_LL324:
if(_tmp599 <= (void*)2)goto _LL326;if(*((int*)_tmp599)!= 1)goto _LL326;_tmp5A1=((
struct Cyc_Absyn_Reference_p_struct*)_tmp599)->f1;_tmp5A2=((struct Cyc_Absyn_Reference_p_struct*)
_tmp599)->f2;_LL325: {struct _tuple1*_tmp5BD=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp5BE=_region_malloc(rgn,sizeof(*_tmp5BE));_tmp5BE->hd=({struct _tuple12*
_tmp5BF=_region_malloc(rgn,sizeof(*_tmp5BF));_tmp5BF->f1=_tmp5BD;_tmp5BF->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp5BF;});_tmp5BE->tl=decls;_tmp5BE;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp5A1->name,Cyc_Absyn_var_exp(_tmp5BD,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp5BD,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple11 _tmp5C0=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp5A2,tag_fail_stmt,fail_stmt,decls);_tmp5C0.f3=Cyc_Absyn_seq_stmt(
s,_tmp5C0.f3,0);return _tmp5C0;}}_LL326: if((int)_tmp599 != 1)goto _LL328;_LL327: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL31D;_LL328:
if(_tmp599 <= (void*)2)goto _LL32A;if(*((int*)_tmp599)!= 7)goto _LL32A;_tmp5A3=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp599)->f1;_tmp5A4=((struct Cyc_Absyn_Int_p_struct*)
_tmp599)->f2;_LL329: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp5A3,_tmp5A4,0),
fail_stmt);goto _LL31D;_LL32A: if(_tmp599 <= (void*)2)goto _LL32C;if(*((int*)_tmp599)
!= 8)goto _LL32C;_tmp5A5=((struct Cyc_Absyn_Char_p_struct*)_tmp599)->f1;_LL32B: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp5A5,0),fail_stmt);goto _LL31D;_LL32C:
if(_tmp599 <= (void*)2)goto _LL32E;if(*((int*)_tmp599)!= 9)goto _LL32E;_tmp5A6=((
struct Cyc_Absyn_Float_p_struct*)_tmp599)->f1;_LL32D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp5A6,0),fail_stmt);goto _LL31D;_LL32E: if(_tmp599 <= (void*)2)goto _LL330;if(*((
int*)_tmp599)!= 10)goto _LL330;_tmp5A7=((struct Cyc_Absyn_Enum_p_struct*)_tmp599)->f1;
_tmp5A8=((struct Cyc_Absyn_Enum_p_struct*)_tmp599)->f2;_LL32F: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp5C1=_cycalloc(
sizeof(*_tmp5C1));_tmp5C1[0]=({struct Cyc_Absyn_Enum_e_struct _tmp5C2;_tmp5C2.tag=
33;_tmp5C2.f1=_tmp5A8->name;_tmp5C2.f2=(struct Cyc_Absyn_Enumdecl*)_tmp5A7;
_tmp5C2.f3=(struct Cyc_Absyn_Enumfield*)_tmp5A8;_tmp5C2;});_tmp5C1;}),0),
fail_stmt);goto _LL31D;_LL330: if(_tmp599 <= (void*)2)goto _LL332;if(*((int*)_tmp599)
!= 11)goto _LL332;_tmp5A9=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp599)->f1;
_tmp5AA=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp599)->f2;_LL331: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp5C3=_cycalloc(
sizeof(*_tmp5C3));_tmp5C3[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp5C4;_tmp5C4.tag=
34;_tmp5C4.f1=_tmp5AA->name;_tmp5C4.f2=(void*)_tmp5A9;_tmp5C4.f3=(struct Cyc_Absyn_Enumfield*)
_tmp5AA;_tmp5C4;});_tmp5C3;}),0),fail_stmt);goto _LL31D;_LL332: if(_tmp599 <= (void*)
2)goto _LL334;if(*((int*)_tmp599)!= 6)goto _LL334;_tmp5AB=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp599)->f1;_tmp5AC=((struct Cyc_Absyn_Tunion_p_struct*)_tmp599)->f2;_tmp5AD=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp599)->f3;if(_tmp5AD != 0)goto _LL334;if(!(!
_tmp5AB->is_flat))goto _LL334;_LL333: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp5AB->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp5AC->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp5AB,_tmp5AC->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL31D;}_LL334: if(_tmp599 <= (void*)2)goto _LL336;if(*((
int*)_tmp599)!= 4)goto _LL336;_tmp5AE=((struct Cyc_Absyn_Pointer_p_struct*)_tmp599)->f1;
_tmp5AF=*_tmp5AE;_tmp5B0=(void*)_tmp5AF.r;if(_tmp5B0 <= (void*)2)goto _LL336;if(*((
int*)_tmp5B0)!= 6)goto _LL336;_tmp5B1=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5B0)->f1;
_tmp5B2=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5B0)->f2;_tmp5B3=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5B0)->f3;if(!(_tmp5B3 != 0  && !_tmp5B1->is_flat))goto _LL336;_LL335: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp5B2->name,({const
char*_tmp5D1="_struct";_tag_dynforward(_tmp5D1,sizeof(char),_get_zero_arr_size(
_tmp5D1,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp5C5=_tmp5B2->typs;
for(0;_tmp5B3 != 0;(((_tmp5B3=_tmp5B3->tl,_tmp5C5=((struct Cyc_List_List*)
_check_null(_tmp5C5))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp5C6=(struct Cyc_Absyn_Pat*)
_tmp5B3->hd;if((void*)_tmp5C6->r == (void*)0)continue;{void*_tmp5C7=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp5C5))->hd)).f2;struct _tuple1*
_tmp5C8=Cyc_Toc_temp_var();void*_tmp5C9=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5C6->topt))->v;void*_tmp5CA=Cyc_Toc_typ_to_c(_tmp5C9);struct Cyc_Absyn_Exp*
_tmp5CB=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp5C7)))_tmp5CB=Cyc_Toc_cast_it(_tmp5CA,_tmp5CB);decls=({
struct Cyc_List_List*_tmp5CC=_region_malloc(rgn,sizeof(*_tmp5CC));_tmp5CC->hd=({
struct _tuple12*_tmp5CD=_region_malloc(rgn,sizeof(*_tmp5CD));_tmp5CD->f1=_tmp5C8;
_tmp5CD->f2=_tmp5CA;_tmp5CD;});_tmp5CC->tl=decls;_tmp5CC;});{struct _tuple11
_tmp5CE=Cyc_Toc_xlate_pat(nv,rgn,_tmp5C9,Cyc_Absyn_var_exp(_tmp5C8,0),_tmp5CB,
_tmp5C6,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5CE.f1;decls=
_tmp5CE.f2;{struct Cyc_Absyn_Stmt*_tmp5CF=_tmp5CE.f3;struct Cyc_Absyn_Stmt*_tmp5D0=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5C8,0),_tmp5CB,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5D0,_tmp5CF,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp5B1->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp5B2->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp5B1,_tmp5B2->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp5B1);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL31D;}}_LL336: if(_tmp599 <= (void*)2)goto
_LL338;if(*((int*)_tmp599)!= 6)goto _LL338;_tmp5B4=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp599)->f1;_tmp5B5=((struct Cyc_Absyn_Tunion_p_struct*)_tmp599)->f2;_tmp5B6=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp599)->f3;if(!_tmp5B4->is_flat)goto _LL338;
_LL337: {struct _tuple1 _tmp5D3;struct _dynforward_ptr*_tmp5D4;struct _tuple1*
_tmp5D2=_tmp5B5->name;_tmp5D3=*_tmp5D2;_tmp5D4=_tmp5D3.f2;r=Cyc_Absyn_aggrmember_exp(
r,_tmp5D4,0);path=Cyc_Absyn_aggrmember_exp(path,_tmp5D4,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
_tmp5B4,_tmp5B5->name,1),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;
_tmp5B6 != 0;(_tmp5B6=_tmp5B6->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5D5=(struct Cyc_Absyn_Pat*)
_tmp5B6->hd;if((void*)_tmp5D5->r == (void*)0)continue;{struct _tuple1*_tmp5D6=Cyc_Toc_temp_var();
void*_tmp5D7=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5D5->topt))->v;decls=({
struct Cyc_List_List*_tmp5D8=_region_malloc(rgn,sizeof(*_tmp5D8));_tmp5D8->hd=({
struct _tuple12*_tmp5D9=_region_malloc(rgn,sizeof(*_tmp5D9));_tmp5D9->f1=_tmp5D6;
_tmp5D9->f2=Cyc_Toc_typ_to_c(_tmp5D7);_tmp5D9;});_tmp5D8->tl=decls;_tmp5D8;});{
struct _tuple11 _tmp5DA=Cyc_Toc_xlate_pat(nv,rgn,_tmp5D7,Cyc_Absyn_var_exp(_tmp5D6,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5D5,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp5DA.f1;decls=_tmp5DA.f2;{struct Cyc_Absyn_Stmt*
_tmp5DB=_tmp5DA.f3;struct Cyc_Absyn_Stmt*_tmp5DC=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5D6,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5DC,_tmp5DB,0),0);}}}}goto _LL31D;}}_LL338: if(_tmp599 <= (
void*)2)goto _LL33A;if(*((int*)_tmp599)!= 6)goto _LL33A;_tmp5B7=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp599)->f3;_LL339: _tmp5B8=_tmp5B7;goto _LL33B;_LL33A: if(_tmp599 <= (void*)2)goto
_LL33C;if(*((int*)_tmp599)!= 3)goto _LL33C;_tmp5B8=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp599)->f1;_LL33B: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp5B8 != 0;(
_tmp5B8=_tmp5B8->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp5DD=(struct Cyc_Absyn_Pat*)
_tmp5B8->hd;if((void*)_tmp5DD->r == (void*)0)continue;{struct _tuple1*_tmp5DE=Cyc_Toc_temp_var();
void*_tmp5DF=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5DD->topt))->v;decls=({
struct Cyc_List_List*_tmp5E0=_region_malloc(rgn,sizeof(*_tmp5E0));_tmp5E0->hd=({
struct _tuple12*_tmp5E1=_region_malloc(rgn,sizeof(*_tmp5E1));_tmp5E1->f1=_tmp5DE;
_tmp5E1->f2=Cyc_Toc_typ_to_c(_tmp5DF);_tmp5E1;});_tmp5E0->tl=decls;_tmp5E0;});{
struct _tuple11 _tmp5E2=Cyc_Toc_xlate_pat(nv,rgn,_tmp5DF,Cyc_Absyn_var_exp(_tmp5DE,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp5DD,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp5E2.f1;decls=_tmp5E2.f2;{struct Cyc_Absyn_Stmt*
_tmp5E3=_tmp5E2.f3;struct Cyc_Absyn_Stmt*_tmp5E4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp5DE,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp5E4,_tmp5E3,0),0);}}}}goto _LL31D;}_LL33C: if(_tmp599 <= (
void*)2)goto _LL33E;if(*((int*)_tmp599)!= 5)goto _LL33E;_tmp5B9=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp599)->f1;_tmp5BA=_tmp5B9.aggr_info;_tmp5BB=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp599)->f3;_LL33D: {struct Cyc_Absyn_Aggrdecl*_tmp5E5=Cyc_Absyn_get_known_aggrdecl(
_tmp5BA);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp5BB != 0;_tmp5BB=_tmp5BB->tl){struct
_tuple13*_tmp5E6=(struct _tuple13*)_tmp5BB->hd;struct Cyc_Absyn_Pat*_tmp5E7=(*
_tmp5E6).f2;if((void*)_tmp5E7->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp5E8=(void*)((struct Cyc_List_List*)_check_null((*_tmp5E6).f1))->hd;
struct _dynforward_ptr*_tmp5E9;_LL347: if(*((int*)_tmp5E8)!= 1)goto _LL349;_tmp5E9=((
struct Cyc_Absyn_FieldName_struct*)_tmp5E8)->f1;_LL348: f=_tmp5E9;goto _LL346;
_LL349:;_LL34A:(int)_throw((void*)Cyc_Toc_Match_error);_LL346:;}{struct _tuple1*
_tmp5EA=Cyc_Toc_temp_var();void*_tmp5EB=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5E7->topt))->v;void*_tmp5EC=Cyc_Toc_typ_to_c(_tmp5EB);decls=({struct Cyc_List_List*
_tmp5ED=_region_malloc(rgn,sizeof(*_tmp5ED));_tmp5ED->hd=({struct _tuple12*
_tmp5EE=_region_malloc(rgn,sizeof(*_tmp5EE));_tmp5EE->f1=_tmp5EA;_tmp5EE->f2=
_tmp5EC;_tmp5EE;});_tmp5ED->tl=decls;_tmp5ED;});{struct _tuple11 _tmp5EF=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5EB,Cyc_Absyn_var_exp(_tmp5EA,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp5E7,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5EF.f1;decls=
_tmp5EF.f2;{struct Cyc_Absyn_Exp*_tmp5F0=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5E5->impl))->fields,f)))->type))_tmp5F0=
Cyc_Toc_cast_it(_tmp5EC,_tmp5F0);{struct Cyc_Absyn_Stmt*_tmp5F1=_tmp5EF.f3;struct
Cyc_Absyn_Stmt*_tmp5F2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5EA,0),
_tmp5F0,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp5F2,_tmp5F1,0),0);}}}}}}
goto _LL31D;}_LL33E: if(_tmp599 <= (void*)2)goto _LL340;if(*((int*)_tmp599)!= 4)goto
_LL340;_tmp5BC=((struct Cyc_Absyn_Pointer_p_struct*)_tmp599)->f1;_LL33F: {struct
_tuple1*_tmp5F3=Cyc_Toc_temp_var();void*_tmp5F4=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5BC->topt))->v;decls=({struct Cyc_List_List*_tmp5F5=
_region_malloc(rgn,sizeof(*_tmp5F5));_tmp5F5->hd=({struct _tuple12*_tmp5F6=
_region_malloc(rgn,sizeof(*_tmp5F6));_tmp5F6->f1=_tmp5F3;_tmp5F6->f2=Cyc_Toc_typ_to_c(
_tmp5F4);_tmp5F6;});_tmp5F5->tl=decls;_tmp5F5;});{struct _tuple11 _tmp5F7=Cyc_Toc_xlate_pat(
nv,rgn,_tmp5F4,Cyc_Absyn_var_exp(_tmp5F3,0),Cyc_Absyn_deref_exp(path,0),_tmp5BC,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp5F7.f1;decls=_tmp5F7.f2;{
struct Cyc_Absyn_Stmt*_tmp5F8=_tmp5F7.f3;struct Cyc_Absyn_Stmt*_tmp5F9=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp5F3,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp5F8,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp5F9,0);else{s=_tmp5F9;}goto _LL31D;}}}_LL340: if(_tmp599 <= (void*)2)goto
_LL342;if(*((int*)_tmp599)!= 12)goto _LL342;_LL341:({void*_tmp5FA[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp5FB="unknownid pat";_tag_dynforward(_tmp5FB,sizeof(char),_get_zero_arr_size(
_tmp5FB,14));}),_tag_dynforward(_tmp5FA,sizeof(void*),0));});_LL342: if(_tmp599 <= (
void*)2)goto _LL344;if(*((int*)_tmp599)!= 13)goto _LL344;_LL343:({void*_tmp5FC[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5FD="unknowncall pat";_tag_dynforward(_tmp5FD,sizeof(char),
_get_zero_arr_size(_tmp5FD,16));}),_tag_dynforward(_tmp5FC,sizeof(void*),0));});
_LL344: if(_tmp599 <= (void*)2)goto _LL31D;if(*((int*)_tmp599)!= 14)goto _LL31D;
_LL345:({void*_tmp5FE[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5FF="exp pat";
_tag_dynforward(_tmp5FF,sizeof(char),_get_zero_arr_size(_tmp5FF,8));}),
_tag_dynforward(_tmp5FE,sizeof(void*),0));});_LL31D:;}return({struct _tuple11
_tmp600;_tmp600.f1=nv;_tmp600.f2=decls;_tmp600.f3=s;_tmp600;});}struct _tuple14{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple14*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple14*_tmp601=_region_malloc(r,sizeof(*_tmp601));_tmp601->f1=
Cyc_Toc_fresh_label();_tmp601->f2=Cyc_Toc_fresh_label();_tmp601->f3=sc;_tmp601;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp602=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp603;union Cyc_Absyn_TunionInfoU_union _tmp604;struct
Cyc_Absyn_Tuniondecl**_tmp605;struct Cyc_Absyn_Tuniondecl*_tmp606;_LL34C: if(
_tmp602 <= (void*)4)goto _LL34E;if(*((int*)_tmp602)!= 2)goto _LL34E;_tmp603=((
struct Cyc_Absyn_TunionType_struct*)_tmp602)->f1;_tmp604=_tmp603.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp602)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL34E;_tmp605=(_tmp604.KnownTunion).f1;_tmp606=*_tmp605;if(!(!_tmp606->is_flat))
goto _LL34E;_LL34D: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*
_tmp607=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp606->fields))->v;
for(0;(unsigned int)_tmp607;_tmp607=_tmp607->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp607->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL34B;}_LL34E:;_LL34F: goto _LL34B;_LL34B:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp608=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp609;struct Cyc_Absyn_Pat*_tmp60A;struct Cyc_Absyn_Pat
_tmp60B;void*_tmp60C;struct Cyc_Absyn_Tuniondecl*_tmp60D;struct Cyc_Absyn_Tunionfield*
_tmp60E;struct Cyc_List_List*_tmp60F;_LL351: if(_tmp608 <= (void*)2)goto _LL355;if(*((
int*)_tmp608)!= 0)goto _LL353;_tmp609=((struct Cyc_Absyn_Var_p_struct*)_tmp608)->f2;
_LL352: return Cyc_Toc_no_tag_test(_tmp609);_LL353: if(*((int*)_tmp608)!= 4)goto
_LL355;_tmp60A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp608)->f1;_tmp60B=*_tmp60A;
_tmp60C=(void*)_tmp60B.r;if(_tmp60C <= (void*)2)goto _LL355;if(*((int*)_tmp60C)!= 
6)goto _LL355;_tmp60D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp60C)->f1;_tmp60E=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp60C)->f2;_tmp60F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp60C)->f3;if(!(_tmp60F != 0  && !_tmp60D->is_flat))goto _LL355;_LL354: return 0;
_LL355:;_LL356: return 1;_LL350:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
Cyc_Toc_exp_to_c(nv,e);{void*_tmp610=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
int leave_as_switch;{void*_tmp611=Cyc_Tcutil_compress(_tmp610);_LL358: if(_tmp611
<= (void*)4)goto _LL35C;if(*((int*)_tmp611)!= 5)goto _LL35A;_LL359: goto _LL35B;
_LL35A: if(*((int*)_tmp611)!= 12)goto _LL35C;_LL35B: leave_as_switch=1;goto _LL357;
_LL35C:;_LL35D: leave_as_switch=0;goto _LL357;_LL357:;}{struct Cyc_List_List*
_tmp612=scs;for(0;_tmp612 != 0;_tmp612=_tmp612->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp612->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp612->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dynforward_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp613=scs;for(0;_tmp613 != 0;_tmp613=_tmp613->tl){struct Cyc_Absyn_Stmt*
_tmp614=((struct Cyc_Absyn_Switch_clause*)_tmp613->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp613->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp614,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp615=_new_region("rgn");struct _RegionHandle*rgn=& _tmp615;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp614);;_pop_region(rgn);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle
_tmp616=_new_region("rgn");struct _RegionHandle*rgn=& _tmp616;_push_region(rgn);{
struct Cyc_Toc_Env*_tmp617=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp610);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp618=lscs;for(0;_tmp618 != 0;_tmp618=_tmp618->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple14*)_tmp618->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp618->tl
== 0?end_l:(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp618->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp619=_tmp618->tl;for(0;(unsigned int)
_tmp619;_tmp619=_tmp619->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple14*)_tmp619->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp61A=_region_malloc(rgn,sizeof(*_tmp61A));
_tmp61A[0]=Cyc_Absyn_goto_stmt((*((struct _tuple14*)_tmp619->hd)).f1,0);_tmp61A;});
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp61B=_region_malloc(rgn,sizeof(*_tmp61B));_tmp61B[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp61B;});}{struct Cyc_Toc_Env*_tmp61D;struct Cyc_List_List*_tmp61E;
struct Cyc_Absyn_Stmt*_tmp61F;struct _tuple11 _tmp61C=Cyc_Toc_xlate_pat(_tmp617,rgn,
_tmp610,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp61D=_tmp61C.f1;_tmp61E=_tmp61C.f2;_tmp61F=_tmp61C.f3;if(is_tunion  && Cyc_Toc_no_tag_test(
sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*
_tmp620=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp61D,_tmp620);_tmp61F=Cyc_Absyn_seq_stmt(_tmp61F,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp620,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp61E;nvs=({struct Cyc_List_List*_tmp621=_region_malloc(rgn,
sizeof(*_tmp621));_tmp621->hd=_tmp61D;_tmp621->tl=nvs;_tmp621;});test_stmts=({
struct Cyc_List_List*_tmp622=_region_malloc(rgn,sizeof(*_tmp622));_tmp622->hd=
_tmp61F;_tmp622->tl=test_stmts;_tmp622;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple14 _tmp624;struct _dynforward_ptr*_tmp625;struct _dynforward_ptr*
_tmp626;struct Cyc_Absyn_Switch_clause*_tmp627;struct _tuple14*_tmp623=(struct
_tuple14*)lscs->hd;_tmp624=*_tmp623;_tmp625=_tmp624.f1;_tmp626=_tmp624.f2;
_tmp627=_tmp624.f3;{struct Cyc_Toc_Env*_tmp628=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp627->body;{struct _RegionHandle
_tmp629=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp629;_push_region(rgn2);
if(lscs->tl != 0){struct _tuple14 _tmp62B;struct _dynforward_ptr*_tmp62C;struct Cyc_Absyn_Switch_clause*
_tmp62D;struct _tuple14*_tmp62A=(struct _tuple14*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp62B=*_tmp62A;_tmp62C=_tmp62B.f2;_tmp62D=_tmp62B.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp628,end_l,_tmp62C,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp62D->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp628,end_l),s);};
_pop_region(rgn2);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp625,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp626,s,0),0);stmts=({struct Cyc_List_List*_tmp62E=_region_malloc(rgn,sizeof(*
_tmp62E));_tmp62E->hd=s;_tmp62E->tl=stmts;_tmp62E;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple12 _tmp630;struct _tuple1*
_tmp631;void*_tmp632;struct _tuple12*_tmp62F=(struct _tuple12*)decls->hd;_tmp630=*
_tmp62F;_tmp631=_tmp630.f1;_tmp632=_tmp630.f2;res=Cyc_Absyn_declare_stmt(_tmp631,
_tmp632,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,
int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp633=
_cycalloc(sizeof(*_tmp633));_tmp633->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp633->tl=0;_tmp633;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp634=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp635;struct Cyc_Absyn_Stmt*_tmp636;struct Cyc_Absyn_Stmt*_tmp637;struct Cyc_Absyn_Exp*
_tmp638;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Stmt*_tmp63A;struct Cyc_Absyn_Stmt*
_tmp63B;struct _tuple3 _tmp63C;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Stmt*
_tmp63E;struct Cyc_Absyn_Stmt*_tmp63F;struct Cyc_Absyn_Stmt*_tmp640;struct Cyc_Absyn_Stmt*
_tmp641;struct Cyc_Absyn_Exp*_tmp642;struct _tuple3 _tmp643;struct Cyc_Absyn_Exp*
_tmp644;struct _tuple3 _tmp645;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Stmt*
_tmp647;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_List_List*_tmp649;struct Cyc_List_List*
_tmp64A;struct Cyc_Absyn_Switch_clause**_tmp64B;struct Cyc_Absyn_Decl*_tmp64C;
struct Cyc_Absyn_Stmt*_tmp64D;struct _dynforward_ptr*_tmp64E;struct Cyc_Absyn_Stmt*
_tmp64F;struct Cyc_Absyn_Stmt*_tmp650;struct _tuple3 _tmp651;struct Cyc_Absyn_Exp*
_tmp652;struct Cyc_Absyn_Stmt*_tmp653;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_Tvar*
_tmp655;struct Cyc_Absyn_Vardecl*_tmp656;int _tmp657;struct Cyc_Absyn_Stmt*_tmp658;
struct Cyc_Absyn_Exp*_tmp659;_LL35F: if((int)_tmp634 != 0)goto _LL361;_LL360: return;
_LL361: if(_tmp634 <= (void*)1)goto _LL363;if(*((int*)_tmp634)!= 0)goto _LL363;
_tmp635=((struct Cyc_Absyn_Exp_s_struct*)_tmp634)->f1;_LL362: Cyc_Toc_exp_to_c(nv,
_tmp635);return;_LL363: if(_tmp634 <= (void*)1)goto _LL365;if(*((int*)_tmp634)!= 1)
goto _LL365;_tmp636=((struct Cyc_Absyn_Seq_s_struct*)_tmp634)->f1;_tmp637=((struct
Cyc_Absyn_Seq_s_struct*)_tmp634)->f2;_LL364: Cyc_Toc_stmt_to_c(nv,_tmp636);s=
_tmp637;continue;_LL365: if(_tmp634 <= (void*)1)goto _LL367;if(*((int*)_tmp634)!= 2)
goto _LL367;_tmp638=((struct Cyc_Absyn_Return_s_struct*)_tmp634)->f1;_LL366: {
struct Cyc_Core_Opt*topt=0;if(_tmp638 != 0){topt=({struct Cyc_Core_Opt*_tmp65A=
_cycalloc(sizeof(*_tmp65A));_tmp65A->v=(void*)Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp638->topt))->v);_tmp65A;});Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp638);}if(s->try_depth > 0){if(topt != 0){struct _tuple1*
_tmp65B=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp65C=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp65B,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp65B,(void*)topt->v,_tmp638,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp65C,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL367: if(_tmp634 <= (void*)1)goto _LL369;if(*((int*)_tmp634)!= 3)goto
_LL369;_tmp639=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp634)->f1;_tmp63A=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp634)->f2;_tmp63B=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp634)->f3;_LL368: Cyc_Toc_exp_to_c(nv,_tmp639);Cyc_Toc_stmt_to_c(nv,_tmp63A);s=
_tmp63B;continue;_LL369: if(_tmp634 <= (void*)1)goto _LL36B;if(*((int*)_tmp634)!= 4)
goto _LL36B;_tmp63C=((struct Cyc_Absyn_While_s_struct*)_tmp634)->f1;_tmp63D=
_tmp63C.f1;_tmp63E=((struct Cyc_Absyn_While_s_struct*)_tmp634)->f2;_LL36A: Cyc_Toc_exp_to_c(
nv,_tmp63D);{struct _RegionHandle _tmp65D=_new_region("temp");struct _RegionHandle*
temp=& _tmp65D;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp63E);;_pop_region(temp);}return;_LL36B: if(_tmp634 <= (void*)1)goto _LL36D;if(*((
int*)_tmp634)!= 5)goto _LL36D;_tmp63F=((struct Cyc_Absyn_Break_s_struct*)_tmp634)->f1;
_LL36C: {struct Cyc_Toc_Env _tmp65F;struct _dynforward_ptr**_tmp660;struct Cyc_Toc_Env*
_tmp65E=nv;_tmp65F=*_tmp65E;_tmp660=_tmp65F.break_lab;if(_tmp660 != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp660,0));{int dest_depth=_tmp63F == 0?0: _tmp63F->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL36D: if(_tmp634 <= (
void*)1)goto _LL36F;if(*((int*)_tmp634)!= 6)goto _LL36F;_tmp640=((struct Cyc_Absyn_Continue_s_struct*)
_tmp634)->f1;_LL36E: {struct Cyc_Toc_Env _tmp662;struct _dynforward_ptr**_tmp663;
struct Cyc_Toc_Env*_tmp661=nv;_tmp662=*_tmp661;_tmp663=_tmp662.continue_lab;if(
_tmp663 != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp663,0));_tmp641=_tmp640;
goto _LL370;}_LL36F: if(_tmp634 <= (void*)1)goto _LL371;if(*((int*)_tmp634)!= 7)goto
_LL371;_tmp641=((struct Cyc_Absyn_Goto_s_struct*)_tmp634)->f2;_LL370: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp641))->try_depth,s);
return;_LL371: if(_tmp634 <= (void*)1)goto _LL373;if(*((int*)_tmp634)!= 8)goto
_LL373;_tmp642=((struct Cyc_Absyn_For_s_struct*)_tmp634)->f1;_tmp643=((struct Cyc_Absyn_For_s_struct*)
_tmp634)->f2;_tmp644=_tmp643.f1;_tmp645=((struct Cyc_Absyn_For_s_struct*)_tmp634)->f3;
_tmp646=_tmp645.f1;_tmp647=((struct Cyc_Absyn_For_s_struct*)_tmp634)->f4;_LL372:
Cyc_Toc_exp_to_c(nv,_tmp642);Cyc_Toc_exp_to_c(nv,_tmp644);Cyc_Toc_exp_to_c(nv,
_tmp646);{struct _RegionHandle _tmp664=_new_region("temp");struct _RegionHandle*
temp=& _tmp664;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp647);;_pop_region(temp);}return;_LL373: if(_tmp634 <= (void*)1)goto _LL375;if(*((
int*)_tmp634)!= 9)goto _LL375;_tmp648=((struct Cyc_Absyn_Switch_s_struct*)_tmp634)->f1;
_tmp649=((struct Cyc_Absyn_Switch_s_struct*)_tmp634)->f2;_LL374: Cyc_Toc_xlate_switch(
nv,s,_tmp648,_tmp649);return;_LL375: if(_tmp634 <= (void*)1)goto _LL377;if(*((int*)
_tmp634)!= 10)goto _LL377;_tmp64A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp634)->f1;
_tmp64B=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp634)->f2;_LL376: {struct Cyc_Toc_Env
_tmp666;struct Cyc_Toc_FallthruInfo*_tmp667;struct Cyc_Toc_Env*_tmp665=nv;_tmp666=*
_tmp665;_tmp667=_tmp666.fallthru_info;if(_tmp667 == 0)({void*_tmp668[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp669="fallthru in unexpected place";_tag_dynforward(_tmp669,sizeof(char),
_get_zero_arr_size(_tmp669,29));}),_tag_dynforward(_tmp668,sizeof(void*),0));});{
struct _dynforward_ptr*_tmp66B;struct Cyc_List_List*_tmp66C;struct Cyc_Dict_Dict
_tmp66D;struct Cyc_Toc_FallthruInfo _tmp66A=*_tmp667;_tmp66B=_tmp66A.label;_tmp66C=
_tmp66A.binders;_tmp66D=_tmp66A.next_case_env;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp66B,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp64B)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp66E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp66C);struct Cyc_List_List*
_tmp66F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp64A);
for(0;_tmp66E != 0;(_tmp66E=_tmp66E->tl,_tmp66F=_tmp66F->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp66F))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k))Cyc_Dict_lookup)(_tmp66D,(struct _tuple1*)_tmp66E->hd),(struct Cyc_Absyn_Exp*)
_tmp66F->hd,0),s2,0);}(void*)(s->r=(void*)((void*)s2->r));return;}}}}_LL377: if(
_tmp634 <= (void*)1)goto _LL379;if(*((int*)_tmp634)!= 11)goto _LL379;_tmp64C=((
struct Cyc_Absyn_Decl_s_struct*)_tmp634)->f1;_tmp64D=((struct Cyc_Absyn_Decl_s_struct*)
_tmp634)->f2;_LL378:{void*_tmp670=(void*)_tmp64C->r;struct Cyc_Absyn_Vardecl*
_tmp671;struct Cyc_Absyn_Pat*_tmp672;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_List_List*
_tmp674;struct Cyc_Absyn_Fndecl*_tmp675;_LL384: if(_tmp670 <= (void*)2)goto _LL38C;
if(*((int*)_tmp670)!= 0)goto _LL386;_tmp671=((struct Cyc_Absyn_Var_d_struct*)
_tmp670)->f1;_LL385:{struct _RegionHandle _tmp676=_new_region("temp");struct
_RegionHandle*temp=& _tmp676;_push_region(temp);{struct Cyc_Toc_Env*_tmp677=Cyc_Toc_add_varmap(
temp,nv,_tmp671->name,Cyc_Absyn_varb_exp(_tmp671->name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678[0]=({struct Cyc_Absyn_Local_b_struct
_tmp679;_tmp679.tag=3;_tmp679.f1=_tmp671;_tmp679;});_tmp678;}),0));Cyc_Toc_local_decl_to_c(
_tmp677,_tmp677,_tmp671,_tmp64D);};_pop_region(temp);}goto _LL383;_LL386: if(*((
int*)_tmp670)!= 2)goto _LL388;_tmp672=((struct Cyc_Absyn_Let_d_struct*)_tmp670)->f1;
_tmp673=((struct Cyc_Absyn_Let_d_struct*)_tmp670)->f3;_LL387:{void*_tmp67A=(void*)
_tmp672->r;struct Cyc_Absyn_Vardecl*_tmp67B;struct Cyc_Absyn_Pat*_tmp67C;struct Cyc_Absyn_Pat
_tmp67D;void*_tmp67E;_LL38F: if(_tmp67A <= (void*)2)goto _LL391;if(*((int*)_tmp67A)
!= 0)goto _LL391;_tmp67B=((struct Cyc_Absyn_Var_p_struct*)_tmp67A)->f1;_tmp67C=((
struct Cyc_Absyn_Var_p_struct*)_tmp67A)->f2;_tmp67D=*_tmp67C;_tmp67E=(void*)
_tmp67D.r;if((int)_tmp67E != 0)goto _LL391;_LL390: {struct _tuple1*old_name=_tmp67B->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp67B->name=new_name;_tmp67B->initializer=(
struct Cyc_Absyn_Exp*)_tmp673;(void*)(_tmp64C->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=({struct Cyc_Absyn_Var_d_struct
_tmp680;_tmp680.tag=0;_tmp680.f1=_tmp67B;_tmp680;});_tmp67F;})));{struct
_RegionHandle _tmp681=_new_region("temp");struct _RegionHandle*temp=& _tmp681;
_push_region(temp);{struct Cyc_Toc_Env*_tmp682=Cyc_Toc_add_varmap(temp,nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Absyn_Local_b_struct
_tmp684;_tmp684.tag=3;_tmp684.f1=_tmp67B;_tmp684;});_tmp683;}),0));Cyc_Toc_local_decl_to_c(
_tmp682,nv,_tmp67B,_tmp64D);};_pop_region(temp);}goto _LL38E;}_LL391:;_LL392:(
void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(nv,_tmp672,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp673->topt))->v,_tmp673,_tmp64D))->r));goto _LL38E;_LL38E:;}goto
_LL383;_LL388: if(*((int*)_tmp670)!= 3)goto _LL38A;_tmp674=((struct Cyc_Absyn_Letv_d_struct*)
_tmp670)->f1;_LL389: {struct Cyc_List_List*_tmp685=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp674);if(_tmp685 == 0)({void*_tmp686[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp687="empty Letv_d";_tag_dynforward(_tmp687,sizeof(char),
_get_zero_arr_size(_tmp687,13));}),_tag_dynforward(_tmp686,sizeof(void*),0));});(
void*)(_tmp64C->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp688=
_cycalloc(sizeof(*_tmp688));_tmp688[0]=({struct Cyc_Absyn_Var_d_struct _tmp689;
_tmp689.tag=0;_tmp689.f1=(struct Cyc_Absyn_Vardecl*)_tmp685->hd;_tmp689;});
_tmp688;})));_tmp685=_tmp685->tl;for(0;_tmp685 != 0;_tmp685=_tmp685->tl){struct
Cyc_Absyn_Decl*_tmp68A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_Var_d_struct
_tmp68C;_tmp68C.tag=0;_tmp68C.f1=(struct Cyc_Absyn_Vardecl*)_tmp685->hd;_tmp68C;});
_tmp68B;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp68A,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL383;}_LL38A: if(*((int*)
_tmp670)!= 1)goto _LL38C;_tmp675=((struct Cyc_Absyn_Fn_d_struct*)_tmp670)->f1;
_LL38B: {struct _tuple1*_tmp68D=_tmp675->name;{struct _RegionHandle _tmp68E=
_new_region("temp");struct _RegionHandle*temp=& _tmp68E;_push_region(temp);{struct
Cyc_Toc_Env*_tmp68F=Cyc_Toc_add_varmap(temp,nv,_tmp675->name,Cyc_Absyn_var_exp(
_tmp68D,0));Cyc_Toc_fndecl_to_c(_tmp68F,_tmp675,0);Cyc_Toc_stmt_to_c(_tmp68F,
_tmp64D);};_pop_region(temp);}goto _LL383;}_LL38C:;_LL38D:({void*_tmp690[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp691="bad nested declaration within function";_tag_dynforward(
_tmp691,sizeof(char),_get_zero_arr_size(_tmp691,39));}),_tag_dynforward(_tmp690,
sizeof(void*),0));});_LL383:;}return;_LL379: if(_tmp634 <= (void*)1)goto _LL37B;if(*((
int*)_tmp634)!= 12)goto _LL37B;_tmp64E=((struct Cyc_Absyn_Label_s_struct*)_tmp634)->f1;
_tmp64F=((struct Cyc_Absyn_Label_s_struct*)_tmp634)->f2;_LL37A: s=_tmp64F;
continue;_LL37B: if(_tmp634 <= (void*)1)goto _LL37D;if(*((int*)_tmp634)!= 13)goto
_LL37D;_tmp650=((struct Cyc_Absyn_Do_s_struct*)_tmp634)->f1;_tmp651=((struct Cyc_Absyn_Do_s_struct*)
_tmp634)->f2;_tmp652=_tmp651.f1;_LL37C:{struct _RegionHandle _tmp692=_new_region("temp");
struct _RegionHandle*temp=& _tmp692;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
temp,nv),_tmp650);Cyc_Toc_exp_to_c(nv,_tmp652);;_pop_region(temp);}return;_LL37D:
if(_tmp634 <= (void*)1)goto _LL37F;if(*((int*)_tmp634)!= 14)goto _LL37F;_tmp653=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp634)->f1;_tmp654=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp634)->f2;_LL37E: {struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=
Cyc_Toc_temp_var();struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
e_exp->topt=({struct Cyc_Core_Opt*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->v=(
void*)e_typ;_tmp693;});{struct _RegionHandle _tmp694=_new_region("temp");struct
_RegionHandle*temp=& _tmp694;_push_region(temp);{struct Cyc_Toc_Env*_tmp695=Cyc_Toc_add_varmap(
temp,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp695,_tmp653);{struct Cyc_Absyn_Stmt*
_tmp696=Cyc_Absyn_seq_stmt(_tmp653,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp697=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp698=
Cyc_Absyn_var_exp(_tmp697,0);struct Cyc_Absyn_Vardecl*_tmp699=Cyc_Absyn_new_vardecl(
_tmp697,Cyc_Absyn_exn_typ,0);_tmp698->topt=({struct Cyc_Core_Opt*_tmp69A=
_cycalloc(sizeof(*_tmp69A));_tmp69A->v=(void*)Cyc_Absyn_exn_typ;_tmp69A;});{
struct Cyc_Absyn_Pat*_tmp69B=({struct Cyc_Absyn_Pat*_tmp6AB=_cycalloc(sizeof(*
_tmp6AB));_tmp6AB->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp6AD=
_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_Var_p_struct _tmp6AE;
_tmp6AE.tag=0;_tmp6AE.f1=_tmp699;_tmp6AE.f2=({struct Cyc_Absyn_Pat*_tmp6AF=
_cycalloc(sizeof(*_tmp6AF));_tmp6AF->r=(void*)((void*)0);_tmp6AF->topt=({struct
Cyc_Core_Opt*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->v=(void*)Cyc_Absyn_exn_typ;
_tmp6B0;});_tmp6AF->loc=0;_tmp6AF;});_tmp6AE;});_tmp6AD;}));_tmp6AB->topt=({
struct Cyc_Core_Opt*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC->v=(void*)Cyc_Absyn_exn_typ;
_tmp6AC;});_tmp6AB->loc=0;_tmp6AB;});struct Cyc_Absyn_Exp*_tmp69C=Cyc_Absyn_throw_exp(
_tmp698,0);_tmp69C->topt=({struct Cyc_Core_Opt*_tmp69D=_cycalloc(sizeof(*_tmp69D));
_tmp69D->v=(void*)((void*)0);_tmp69D;});{struct Cyc_Absyn_Stmt*_tmp69E=Cyc_Absyn_exp_stmt(
_tmp69C,0);struct Cyc_Absyn_Switch_clause*_tmp69F=({struct Cyc_Absyn_Switch_clause*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8->pattern=_tmp69B;_tmp6A8->pat_vars=({
struct Cyc_Core_Opt*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->v=({struct Cyc_List_List*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->hd=_tmp699;_tmp6AA->tl=0;_tmp6AA;});
_tmp6A9;});_tmp6A8->where_clause=0;_tmp6A8->body=_tmp69E;_tmp6A8->loc=0;_tmp6A8;});
struct Cyc_Absyn_Stmt*_tmp6A0=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp654,({struct
Cyc_List_List*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->hd=_tmp69F;_tmp6A7->tl=
0;_tmp6A7;})),0);Cyc_Toc_stmt_to_c(_tmp695,_tmp6A0);{struct Cyc_Absyn_Exp*_tmp6A1=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp6A6=_cycalloc(
sizeof(*_tmp6A6));_tmp6A6->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp6A6->tl=0;_tmp6A6;}),0);struct Cyc_Absyn_Stmt*_tmp6A2=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp6A5=
_cycalloc(sizeof(*_tmp6A5));_tmp6A5->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp6A5->tl=
0;_tmp6A5;}),0),0);struct Cyc_Absyn_Exp*_tmp6A3=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp6A4=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp6A2,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp6A3,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp6A1,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp6A4,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp696,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp6A0,0),0),0),0),0),0))->r));}}}}};
_pop_region(temp);}return;}_LL37F: if(_tmp634 <= (void*)1)goto _LL381;if(*((int*)
_tmp634)!= 15)goto _LL381;_tmp655=((struct Cyc_Absyn_Region_s_struct*)_tmp634)->f1;
_tmp656=((struct Cyc_Absyn_Region_s_struct*)_tmp634)->f2;_tmp657=((struct Cyc_Absyn_Region_s_struct*)
_tmp634)->f3;_tmp658=((struct Cyc_Absyn_Region_s_struct*)_tmp634)->f4;_LL380: {
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*
rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp656->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);{struct _RegionHandle _tmp6B1=_new_region("temp");struct _RegionHandle*
temp=& _tmp6B1;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,
x_var,x_exp),_tmp658);;_pop_region(temp);}(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp6B2->tl=0;_tmp6B2;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp6B3[1];_tmp6B3[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6B3,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp658,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp6B4[1];_tmp6B4[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp6B4,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));return;}_LL381: if(
_tmp634 <= (void*)1)goto _LL35E;if(*((int*)_tmp634)!= 16)goto _LL35E;_tmp659=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmp634)->f1;_LL382: Cyc_Toc_exp_to_c(nv,
_tmp659);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({
struct Cyc_List_List*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->hd=_tmp659;
_tmp6B5->tl=0;_tmp6B5;}),0)));return;_LL35E:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple15{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct _RegionHandle _tmp6B6=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp6B6;_push_region(frgn);{struct Cyc_Toc_Env*_tmp6B7=Cyc_Toc_share_env(
frgn,nv);{struct Cyc_List_List*_tmp6B8=f->args;for(0;_tmp6B8 != 0;_tmp6B8=_tmp6B8->tl){
struct _tuple1*_tmp6B9=({struct _tuple1*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));
_tmp6BA->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union
Cyc_Absyn_Nmspace_union _tmp6BB;(_tmp6BB.Loc_n).tag=0;_tmp6BB;}));_tmp6BA->f2=(*((
struct _tuple15*)_tmp6B8->hd)).f1;_tmp6BA;});(*((struct _tuple15*)_tmp6B8->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp6B8->hd)).f3);_tmp6B7=Cyc_Toc_add_varmap(
frgn,_tmp6B7,_tmp6B9,Cyc_Absyn_var_exp(_tmp6B9,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(
f->body);_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp6BD;struct Cyc_Absyn_Tqual
_tmp6BE;void*_tmp6BF;int _tmp6C0;struct Cyc_Absyn_VarargInfo _tmp6BC=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp6BD=_tmp6BC.name;_tmp6BE=_tmp6BC.tq;_tmp6BF=(
void*)_tmp6BC.type;_tmp6C0=_tmp6BC.inject;{void*_tmp6C1=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp6BF,(void*)2,_tmp6BE,Cyc_Absyn_false_conref));struct _tuple1*_tmp6C2=({struct
_tuple1*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp6C6;(_tmp6C6.Loc_n).tag=
0;_tmp6C6;}));_tmp6C5->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp6BD))->v;_tmp6C5;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp6C3=
_cycalloc(sizeof(*_tmp6C3));_tmp6C3->hd=({struct _tuple15*_tmp6C4=_cycalloc(
sizeof(*_tmp6C4));_tmp6C4->f1=(struct _dynforward_ptr*)_tmp6BD->v;_tmp6C4->f2=
_tmp6BE;_tmp6C4->f3=_tmp6C1;_tmp6C4;});_tmp6C3->tl=0;_tmp6C3;}));_tmp6B7=Cyc_Toc_add_varmap(
frgn,_tmp6B7,_tmp6C2,Cyc_Absyn_var_exp(_tmp6C2,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp6C7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp6C7 != 0;_tmp6C7=_tmp6C7->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp6C7->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp6C7->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp6B7),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp6C8=s;_LL394:
if((int)_tmp6C8 != 1)goto _LL396;_LL395: return(void*)2;_LL396: if((int)_tmp6C8 != 4)
goto _LL398;_LL397: return(void*)3;_LL398:;_LL399: return s;_LL393:;}struct Cyc_Dict_Dict*
Cyc_Toc_aggrs_so_far=0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp6C9=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({
struct Cyc_Dict_Dict*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp6CA;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp6CB=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far)),_tmp6C9);if(_tmp6CB == 0){seen_defn_before=0;*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_aggrs_so_far))=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_aggrs_so_far)),_tmp6C9,ad);}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp6CB->v)->impl == 0){*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far))=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(*((struct Cyc_Dict_Dict*)
_check_null(Cyc_Toc_aggrs_so_far)),_tmp6C9,ad);seen_defn_before=0;}else{
seen_defn_before=1;}}(void*)(ad->sc=(void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp6CC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp6CC != 0;_tmp6CC=_tmp6CC->tl){((struct Cyc_Absyn_Aggrfield*)_tmp6CC->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp6CC->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp6CC->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp6CD;});{
struct _tuple1*_tmp6CE=tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp6CE))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp6CE));{struct Cyc_List_List*
flat_structs=0;{struct Cyc_List_List*_tmp6CF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp6CF != 0;_tmp6CF=_tmp6CF->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp6CF->hd;if(f->typs != 0  || tud->is_flat){
struct Cyc_List_List*_tmp6D0=0;int i=1;{struct Cyc_List_List*_tmp6D1=f->typs;for(0;
_tmp6D1 != 0;(_tmp6D1=_tmp6D1->tl,i ++)){struct _dynforward_ptr*_tmp6D2=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp6D3=({struct Cyc_Absyn_Aggrfield*_tmp6D5=
_cycalloc(sizeof(*_tmp6D5));_tmp6D5->name=_tmp6D2;_tmp6D5->tq=(*((struct _tuple4*)
_tmp6D1->hd)).f1;_tmp6D5->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp6D1->hd)).f2);_tmp6D5->width=0;_tmp6D5->attributes=0;_tmp6D5;});_tmp6D0=({
struct Cyc_List_List*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->hd=_tmp6D3;
_tmp6D4->tl=_tmp6D0;_tmp6D4;});}}_tmp6D0=({struct Cyc_List_List*_tmp6D6=_cycalloc(
sizeof(*_tmp6D6));_tmp6D6->hd=({struct Cyc_Absyn_Aggrfield*_tmp6D7=_cycalloc(
sizeof(*_tmp6D7));_tmp6D7->name=Cyc_Toc_tag_sp;_tmp6D7->tq=Cyc_Toc_mt_tq;_tmp6D7->type=(
void*)Cyc_Absyn_sint_typ;_tmp6D7->width=0;_tmp6D7->attributes=0;_tmp6D7;});
_tmp6D6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp6D0);_tmp6D6;});{struct Cyc_Absyn_Aggrdecl*_tmp6D8=({struct Cyc_Absyn_Aggrdecl*
_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4->kind=(void*)((void*)0);_tmp6E4->sc=(
void*)((void*)2);_tmp6E4->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp6E6="_struct";_tag_dynforward(_tmp6E6,sizeof(char),_get_zero_arr_size(
_tmp6E6,8));}));_tmp6E4->tvs=0;_tmp6E4->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5->exist_vars=0;_tmp6E5->rgn_po=0;
_tmp6E5->fields=_tmp6D0;_tmp6E5;});_tmp6E4->attributes=0;_tmp6E4;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));
_tmp6DA[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp6DB;_tmp6DB.tag=4;_tmp6DB.f1=
_tmp6D8;_tmp6DB;});_tmp6DA;}),0);_tmp6D9->tl=Cyc_Toc_result_decls;_tmp6D9;});if(
tud->is_flat){struct Cyc_Absyn_Aggrfield*_tmp6DC=({struct Cyc_Absyn_Aggrfield*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->name=(*f->name).f2;_tmp6DE->tq=Cyc_Toc_mt_tq;
_tmp6DE->type=(void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp6DF=_cycalloc(
sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_AggrType_struct _tmp6E0;_tmp6E0.tag=
10;_tmp6E0.f1=({struct Cyc_Absyn_AggrInfo _tmp6E1;_tmp6E1.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp6E2;(_tmp6E2.KnownAggr).tag=1;(_tmp6E2.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=_tmp6D8;
_tmp6E3;});_tmp6E2;});_tmp6E1.targs=0;_tmp6E1;});_tmp6E0;});_tmp6DF;}));_tmp6DE->width=
0;_tmp6DE->attributes=0;_tmp6DE;});flat_structs=({struct Cyc_List_List*_tmp6DD=
_cycalloc(sizeof(*_tmp6DD));_tmp6DD->hd=_tmp6DC;_tmp6DD->tl=flat_structs;_tmp6DD;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp6E7=({struct Cyc_Absyn_Aggrdecl*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB->kind=(void*)((void*)1);_tmp6EB->sc=(
void*)((void*)2);_tmp6EB->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp6ED="_union";_tag_dynforward(_tmp6ED,sizeof(char),_get_zero_arr_size(_tmp6ED,
7));}));_tmp6EB->tvs=0;_tmp6EB->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp6EC=
_cycalloc(sizeof(*_tmp6EC));_tmp6EC->exist_vars=0;_tmp6EC->rgn_po=0;_tmp6EC->fields=
flat_structs;_tmp6EC;});_tmp6EB->attributes=0;_tmp6EB;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));
_tmp6E9[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp6EA;_tmp6EA.tag=4;_tmp6EA.f1=
_tmp6E7;_tmp6EA;});_tmp6E9;}),0);_tmp6E8->tl=Cyc_Toc_result_decls;_tmp6E8;});}}}}}
static struct Cyc_Dict_Dict*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Dict_Dict*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));_tmp6EE[0]=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp6EE;});if(xd->fields == 0)return;{struct _tuple1*_tmp6EF=xd->name;struct Cyc_List_List*
_tmp6F0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp6F0 != 0;_tmp6F0=_tmp6F0->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp6F0->hd;struct _dynforward_ptr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp6F1=
Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(char))+ 4,0);void*_tmp6F2=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp6F1,Cyc_Absyn_false_conref,
0);struct Cyc_Core_Opt*_tmp6F3=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict d,
struct _tuple1*k))Cyc_Dict_lookup_opt)(*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),
f->name);struct Cyc_Core_Opt _tmp6F4;int _tmp6F5;_LL39B: if(_tmp6F3 != 0)goto _LL39D;
_LL39C: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){char zero='\000';
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp6FC;_tmp6FC.tag=0;_tmp6FC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp6FB;
_tmp6FB.tag=1;_tmp6FB.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp6FA;_tmp6FA.tag=1;_tmp6FA.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp6F9;_tmp6F9.tag=1;_tmp6F9.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp6F8;_tmp6F8.tag=1;_tmp6F8.f1=(unsigned long)((int)zero);{void*_tmp6F6[5]={&
_tmp6F8,& _tmp6F9,& _tmp6FA,& _tmp6FB,& _tmp6FC};Cyc_aprintf(({const char*_tmp6F7="%c%c%c%c%s";
_tag_dynforward(_tmp6F7,sizeof(char),_get_zero_arr_size(_tmp6F7,11));}),
_tag_dynforward(_tmp6F6,sizeof(void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*
_tmp6FD=Cyc_Absyn_new_vardecl(f->name,_tmp6F2,initopt);(void*)(_tmp6FD->sc=(void*)((
void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6FE=_cycalloc(
sizeof(*_tmp6FE));_tmp6FE->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_Var_d_struct
_tmp700;_tmp700.tag=0;_tmp700.f1=_tmp6FD;_tmp700;});_tmp6FF;}),0);_tmp6FE->tl=
Cyc_Toc_result_decls;_tmp6FE;});*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far))=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),f->name,(void*)
f->sc != (void*)3);if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*
_tmp701=f->typs;for(0;_tmp701 != 0;(_tmp701=_tmp701->tl,i ++)){struct
_dynforward_ptr*_tmp702=({struct _dynforward_ptr*_tmp706=_cycalloc(sizeof(*
_tmp706));_tmp706[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp709;
_tmp709.tag=1;_tmp709.f1=(unsigned long)i;{void*_tmp707[1]={& _tmp709};Cyc_aprintf(({
const char*_tmp708="f%d";_tag_dynforward(_tmp708,sizeof(char),_get_zero_arr_size(
_tmp708,4));}),_tag_dynforward(_tmp707,sizeof(void*),1));}});_tmp706;});struct
Cyc_Absyn_Aggrfield*_tmp703=({struct Cyc_Absyn_Aggrfield*_tmp705=_cycalloc(
sizeof(*_tmp705));_tmp705->name=_tmp702;_tmp705->tq=(*((struct _tuple4*)_tmp701->hd)).f1;
_tmp705->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp701->hd)).f2);
_tmp705->width=0;_tmp705->attributes=0;_tmp705;});fields=({struct Cyc_List_List*
_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704->hd=_tmp703;_tmp704->tl=fields;
_tmp704;});}}fields=({struct Cyc_List_List*_tmp70A=_cycalloc(sizeof(*_tmp70A));
_tmp70A->hd=({struct Cyc_Absyn_Aggrfield*_tmp70B=_cycalloc(sizeof(*_tmp70B));
_tmp70B->name=Cyc_Toc_tag_sp;_tmp70B->tq=Cyc_Toc_mt_tq;_tmp70B->type=(void*)Cyc_Absyn_cstar_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp70B->width=0;_tmp70B->attributes=0;_tmp70B;});
_tmp70A->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp70A;});{struct Cyc_Absyn_Aggrdecl*_tmp70C=({struct Cyc_Absyn_Aggrdecl*
_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->kind=(void*)((void*)0);_tmp710->sc=(
void*)((void*)2);_tmp710->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp712="_struct";_tag_dynforward(_tmp712,sizeof(char),_get_zero_arr_size(
_tmp712,8));}));_tmp710->tvs=0;_tmp710->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->exist_vars=0;_tmp711->rgn_po=0;
_tmp711->fields=fields;_tmp711;});_tmp710->attributes=0;_tmp710;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp70E=_cycalloc(sizeof(*_tmp70E));
_tmp70E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp70F;_tmp70F.tag=4;_tmp70F.f1=
_tmp70C;_tmp70F;});_tmp70E;}),0);_tmp70D->tl=Cyc_Toc_result_decls;_tmp70D;});}}
goto _LL39A;}}_LL39D: if(_tmp6F3 == 0)goto _LL39F;_tmp6F4=*_tmp6F3;_tmp6F5=(int)
_tmp6F4.v;if(_tmp6F5 != 0)goto _LL39F;_LL39E: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp713=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp71E;_tmp71E.tag=0;_tmp71E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp71D;_tmp71D.tag=1;_tmp71D.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp71C;_tmp71C.tag=1;_tmp71C.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp71B;_tmp71B.tag=1;_tmp71B.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp71A;_tmp71A.tag=1;_tmp71A.f1=(
unsigned long)((int)zero);{void*_tmp718[5]={& _tmp71A,& _tmp71B,& _tmp71C,& _tmp71D,&
_tmp71E};Cyc_aprintf(({const char*_tmp719="%c%c%c%c%s";_tag_dynforward(_tmp719,
sizeof(char),_get_zero_arr_size(_tmp719,11));}),_tag_dynforward(_tmp718,sizeof(
void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp714=Cyc_Absyn_new_vardecl(f->name,
_tmp6F2,(struct Cyc_Absyn_Exp*)_tmp713);(void*)(_tmp714->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp715=_cycalloc(sizeof(*_tmp715));
_tmp715->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp716=
_cycalloc(sizeof(*_tmp716));_tmp716[0]=({struct Cyc_Absyn_Var_d_struct _tmp717;
_tmp717.tag=0;_tmp717.f1=_tmp714;_tmp717;});_tmp716;}),0);_tmp715->tl=Cyc_Toc_result_decls;
_tmp715;});*((struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far))=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict*)_check_null(Cyc_Toc_xtunion_fields_so_far)),f->name,1);}
goto _LL39A;_LL39F:;_LL3A0: goto _LL39A;_LL39A:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp71F=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp720;struct Cyc_Absyn_Exp*_tmp721;struct Cyc_Absyn_Exp*_tmp722;int _tmp723;
_LL3A2: if(*((int*)_tmp71F)!= 29)goto _LL3A4;_tmp720=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp71F)->f1;_tmp721=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp71F)->f2;
_tmp722=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp71F)->f3;_tmp723=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp71F)->f4;_LL3A3: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp720,_tmp721,_tmp722,_tmp723,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL3A1;_LL3A4:;_LL3A5: if((void*)vd->sc == (void*)0){struct _RegionHandle
_tmp724=_new_region("temp");struct _RegionHandle*temp=& _tmp724;_push_region(temp);{
struct Cyc_Toc_Env*_tmp725=Cyc_Toc_set_toplevel(temp,init_nv);Cyc_Toc_exp_to_c(
_tmp725,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(init_nv,init);}goto
_LL3A1;_LL3A1:;}else{void*_tmp726=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo
_tmp727;void*_tmp728;struct Cyc_Absyn_Exp*_tmp729;struct Cyc_Absyn_Conref*_tmp72A;
_LL3A7: if(_tmp726 <= (void*)4)goto _LL3A9;if(*((int*)_tmp726)!= 7)goto _LL3A9;
_tmp727=((struct Cyc_Absyn_ArrayType_struct*)_tmp726)->f1;_tmp728=(void*)_tmp727.elt_type;
_tmp729=_tmp727.num_elts;_tmp72A=_tmp727.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp72A))goto _LL3A9;_LL3A8: if(_tmp729 == 0)({void*
_tmp72B[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp72C="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp72C,sizeof(char),_get_zero_arr_size(_tmp72C,55));}),
_tag_dynforward(_tmp72B,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp729;struct Cyc_Absyn_Exp*_tmp72D=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp72E=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp72D,_tmp72E,
0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL3A6;}_LL3A9:;_LL3AA: goto _LL3A6;
_LL3A6:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static
struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp72F=_cycalloc(
sizeof(*_tmp72F));_tmp72F[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp72F;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp730=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp730;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp731=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp732=Cyc_Toc_share_env(
prgn,nv);struct Cyc_Toc_Env*_tmp734;struct Cyc_List_List*_tmp735;struct Cyc_Absyn_Stmt*
_tmp736;struct _tuple11 _tmp733=Cyc_Toc_xlate_pat(_tmp732,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)& _tmp731,Cyc_Toc_throw_match_stmt(),
0);_tmp734=_tmp733.f1;_tmp735=_tmp733.f2;_tmp736=_tmp733.f3;Cyc_Toc_stmt_to_c(
_tmp734,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp736,s,0),0);for(0;_tmp735 != 0;_tmp735=_tmp735->tl){
struct _tuple12 _tmp738;struct _tuple1*_tmp739;void*_tmp73A;struct _tuple12*_tmp737=(
struct _tuple12*)_tmp735->hd;_tmp738=*_tmp737;_tmp739=_tmp738.f1;_tmp73A=_tmp738.f2;
s=Cyc_Absyn_declare_stmt(_tmp739,_tmp73A,0,s,0);}};_pop_region(prgn);}return s;}}
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp73B=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Exp*_tmp73D;struct Cyc_Absyn_Exp*
_tmp73E;struct Cyc_Absyn_Exp*_tmp73F;struct Cyc_Absyn_Exp*_tmp740;struct Cyc_Absyn_Exp*
_tmp741;struct Cyc_Absyn_Exp*_tmp742;struct Cyc_Absyn_Exp*_tmp743;struct Cyc_List_List*
_tmp744;struct Cyc_Absyn_Exp*_tmp745;struct Cyc_Absyn_Exp*_tmp746;struct Cyc_Absyn_Exp*
_tmp747;struct Cyc_Absyn_Exp*_tmp748;struct Cyc_Absyn_Exp*_tmp749;struct Cyc_Absyn_Exp*
_tmp74A;struct Cyc_Absyn_Exp*_tmp74B;struct Cyc_Absyn_Exp*_tmp74C;struct Cyc_Absyn_Exp*
_tmp74D;struct Cyc_Absyn_Exp*_tmp74E;struct Cyc_Absyn_Exp*_tmp74F;struct Cyc_Absyn_Exp*
_tmp750;struct Cyc_Absyn_Exp*_tmp751;struct Cyc_Absyn_Exp*_tmp752;struct Cyc_List_List*
_tmp753;struct Cyc_Absyn_Exp*_tmp754;struct Cyc_List_List*_tmp755;void*_tmp756;
void**_tmp757;struct Cyc_Absyn_Exp*_tmp758;struct _tuple2*_tmp759;struct _tuple2
_tmp75A;void*_tmp75B;void**_tmp75C;struct Cyc_List_List*_tmp75D;struct Cyc_List_List*
_tmp75E;struct Cyc_List_List*_tmp75F;void*_tmp760;void**_tmp761;void*_tmp762;void**
_tmp763;struct Cyc_Absyn_Stmt*_tmp764;struct Cyc_Absyn_MallocInfo _tmp765;struct Cyc_Absyn_MallocInfo*
_tmp766;_LL3AC: if(*((int*)_tmp73B)!= 22)goto _LL3AE;_tmp73C=((struct Cyc_Absyn_Deref_e_struct*)
_tmp73B)->f1;_LL3AD: _tmp73D=_tmp73C;goto _LL3AF;_LL3AE: if(*((int*)_tmp73B)!= 23)
goto _LL3B0;_tmp73D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp73B)->f1;_LL3AF:
_tmp73E=_tmp73D;goto _LL3B1;_LL3B0: if(*((int*)_tmp73B)!= 24)goto _LL3B2;_tmp73E=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp73B)->f1;_LL3B1: _tmp73F=_tmp73E;goto
_LL3B3;_LL3B2: if(*((int*)_tmp73B)!= 16)goto _LL3B4;_tmp73F=((struct Cyc_Absyn_Address_e_struct*)
_tmp73B)->f1;_LL3B3: _tmp740=_tmp73F;goto _LL3B5;_LL3B4: if(*((int*)_tmp73B)!= 12)
goto _LL3B6;_tmp740=((struct Cyc_Absyn_Throw_e_struct*)_tmp73B)->f1;_LL3B5: _tmp741=
_tmp740;goto _LL3B7;_LL3B6: if(*((int*)_tmp73B)!= 13)goto _LL3B8;_tmp741=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp73B)->f1;_LL3B7: _tmp742=_tmp741;goto _LL3B9;
_LL3B8: if(*((int*)_tmp73B)!= 19)goto _LL3BA;_tmp742=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp73B)->f1;_LL3B9: _tmp743=_tmp742;goto _LL3BB;_LL3BA: if(*((int*)_tmp73B)!= 5)
goto _LL3BC;_tmp743=((struct Cyc_Absyn_Increment_e_struct*)_tmp73B)->f1;_LL3BB: Cyc_Toc_exptypes_to_c(
_tmp743);goto _LL3AB;_LL3BC: if(*((int*)_tmp73B)!= 3)goto _LL3BE;_tmp744=((struct
Cyc_Absyn_Primop_e_struct*)_tmp73B)->f2;_LL3BD:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp744);goto _LL3AB;
_LL3BE: if(*((int*)_tmp73B)!= 7)goto _LL3C0;_tmp745=((struct Cyc_Absyn_And_e_struct*)
_tmp73B)->f1;_tmp746=((struct Cyc_Absyn_And_e_struct*)_tmp73B)->f2;_LL3BF: _tmp747=
_tmp745;_tmp748=_tmp746;goto _LL3C1;_LL3C0: if(*((int*)_tmp73B)!= 8)goto _LL3C2;
_tmp747=((struct Cyc_Absyn_Or_e_struct*)_tmp73B)->f1;_tmp748=((struct Cyc_Absyn_Or_e_struct*)
_tmp73B)->f2;_LL3C1: _tmp749=_tmp747;_tmp74A=_tmp748;goto _LL3C3;_LL3C2: if(*((int*)
_tmp73B)!= 9)goto _LL3C4;_tmp749=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp73B)->f1;
_tmp74A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp73B)->f2;_LL3C3: _tmp74B=_tmp749;
_tmp74C=_tmp74A;goto _LL3C5;_LL3C4: if(*((int*)_tmp73B)!= 25)goto _LL3C6;_tmp74B=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp73B)->f1;_tmp74C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp73B)->f2;_LL3C5: _tmp74D=_tmp74B;_tmp74E=_tmp74C;goto _LL3C7;_LL3C6: if(*((int*)
_tmp73B)!= 4)goto _LL3C8;_tmp74D=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp73B)->f1;
_tmp74E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp73B)->f3;_LL3C7: Cyc_Toc_exptypes_to_c(
_tmp74D);Cyc_Toc_exptypes_to_c(_tmp74E);goto _LL3AB;_LL3C8: if(*((int*)_tmp73B)!= 
6)goto _LL3CA;_tmp74F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp73B)->f1;
_tmp750=((struct Cyc_Absyn_Conditional_e_struct*)_tmp73B)->f2;_tmp751=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp73B)->f3;_LL3C9: Cyc_Toc_exptypes_to_c(_tmp74F);Cyc_Toc_exptypes_to_c(_tmp750);
Cyc_Toc_exptypes_to_c(_tmp751);goto _LL3AB;_LL3CA: if(*((int*)_tmp73B)!= 11)goto
_LL3CC;_tmp752=((struct Cyc_Absyn_FnCall_e_struct*)_tmp73B)->f1;_tmp753=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp73B)->f2;_LL3CB: _tmp754=_tmp752;_tmp755=_tmp753;
goto _LL3CD;_LL3CC: if(*((int*)_tmp73B)!= 10)goto _LL3CE;_tmp754=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp73B)->f1;_tmp755=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp73B)->f2;_LL3CD:
Cyc_Toc_exptypes_to_c(_tmp754);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp755);goto _LL3AB;_LL3CE:
if(*((int*)_tmp73B)!= 15)goto _LL3D0;_tmp756=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp73B)->f1;_tmp757=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp73B)->f1);
_tmp758=((struct Cyc_Absyn_Cast_e_struct*)_tmp73B)->f2;_LL3CF:*_tmp757=Cyc_Toc_typ_to_c(*
_tmp757);Cyc_Toc_exptypes_to_c(_tmp758);goto _LL3AB;_LL3D0: if(*((int*)_tmp73B)!= 
27)goto _LL3D2;_tmp759=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp73B)->f1;
_tmp75A=*_tmp759;_tmp75B=_tmp75A.f3;_tmp75C=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp73B)->f1).f3;_tmp75D=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp73B)->f2;
_LL3D1:*_tmp75C=Cyc_Toc_typ_to_c(*_tmp75C);_tmp75E=_tmp75D;goto _LL3D3;_LL3D2: if(*((
int*)_tmp73B)!= 36)goto _LL3D4;_tmp75E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp73B)->f2;_LL3D3: _tmp75F=_tmp75E;goto _LL3D5;_LL3D4: if(*((int*)_tmp73B)!= 28)
goto _LL3D6;_tmp75F=((struct Cyc_Absyn_Array_e_struct*)_tmp73B)->f1;_LL3D5: for(0;
_tmp75F != 0;_tmp75F=_tmp75F->tl){struct Cyc_Absyn_Exp*_tmp768;struct _tuple6
_tmp767=*((struct _tuple6*)_tmp75F->hd);_tmp768=_tmp767.f2;Cyc_Toc_exptypes_to_c(
_tmp768);}goto _LL3AB;_LL3D6: if(*((int*)_tmp73B)!= 20)goto _LL3D8;_tmp760=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp73B)->f1;_tmp761=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp73B)->f1);_LL3D7: _tmp763=_tmp761;goto
_LL3D9;_LL3D8: if(*((int*)_tmp73B)!= 18)goto _LL3DA;_tmp762=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp73B)->f1;_tmp763=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp73B)->f1);_LL3D9:*_tmp763=Cyc_Toc_typ_to_c(*_tmp763);goto _LL3AB;_LL3DA: if(*((
int*)_tmp73B)!= 37)goto _LL3DC;_tmp764=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp73B)->f1;_LL3DB: Cyc_Toc_stmttypes_to_c(_tmp764);goto _LL3AB;_LL3DC: if(*((int*)
_tmp73B)!= 35)goto _LL3DE;_tmp765=((struct Cyc_Absyn_Malloc_e_struct*)_tmp73B)->f1;
_tmp766=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp73B)->f1;
_LL3DD: if(_tmp766->elt_type != 0)_tmp766->elt_type=({void**_tmp769=_cycalloc(
sizeof(*_tmp769));_tmp769[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp766->elt_type)));
_tmp769;});Cyc_Toc_exptypes_to_c(_tmp766->num_elts);goto _LL3AB;_LL3DE: if(*((int*)
_tmp73B)!= 0)goto _LL3E0;_LL3DF: goto _LL3E1;_LL3E0: if(*((int*)_tmp73B)!= 1)goto
_LL3E2;_LL3E1: goto _LL3E3;_LL3E2: if(*((int*)_tmp73B)!= 2)goto _LL3E4;_LL3E3: goto
_LL3E5;_LL3E4: if(*((int*)_tmp73B)!= 33)goto _LL3E6;_LL3E5: goto _LL3E7;_LL3E6: if(*((
int*)_tmp73B)!= 34)goto _LL3E8;_LL3E7: goto _LL3AB;_LL3E8: if(*((int*)_tmp73B)!= 31)
goto _LL3EA;_LL3E9: goto _LL3EB;_LL3EA: if(*((int*)_tmp73B)!= 32)goto _LL3EC;_LL3EB:
goto _LL3ED;_LL3EC: if(*((int*)_tmp73B)!= 30)goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE:
if(*((int*)_tmp73B)!= 29)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(*((int*)_tmp73B)
!= 26)goto _LL3F2;_LL3F1: goto _LL3F3;_LL3F2: if(*((int*)_tmp73B)!= 14)goto _LL3F4;
_LL3F3: goto _LL3F5;_LL3F4: if(*((int*)_tmp73B)!= 17)goto _LL3F6;_LL3F5: goto _LL3F7;
_LL3F6: if(*((int*)_tmp73B)!= 21)goto _LL3AB;_LL3F7:({void*_tmp76A[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp76B="Cyclone expression within C code";_tag_dynforward(
_tmp76B,sizeof(char),_get_zero_arr_size(_tmp76B,33));}),_tag_dynforward(_tmp76A,
sizeof(void*),0));});goto _LL3AB;_LL3AB:;}static void Cyc_Toc_decltypes_to_c(struct
Cyc_Absyn_Decl*d){void*_tmp76C=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp76D;
struct Cyc_Absyn_Fndecl*_tmp76E;struct Cyc_Absyn_Aggrdecl*_tmp76F;struct Cyc_Absyn_Enumdecl*
_tmp770;struct Cyc_Absyn_Typedefdecl*_tmp771;_LL3F9: if(_tmp76C <= (void*)2)goto
_LL411;if(*((int*)_tmp76C)!= 0)goto _LL3FB;_tmp76D=((struct Cyc_Absyn_Var_d_struct*)
_tmp76C)->f1;_LL3FA:(void*)(_tmp76D->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp76D->type));
if(_tmp76D->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp76D->initializer));goto _LL3F8;_LL3FB: if(*((int*)_tmp76C)!= 1)
goto _LL3FD;_tmp76E=((struct Cyc_Absyn_Fn_d_struct*)_tmp76C)->f1;_LL3FC:(void*)(
_tmp76E->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp76E->ret_type));{struct Cyc_List_List*
_tmp772=_tmp76E->args;for(0;_tmp772 != 0;_tmp772=_tmp772->tl){(*((struct _tuple15*)
_tmp772->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp772->hd)).f3);}}goto
_LL3F8;_LL3FD: if(*((int*)_tmp76C)!= 4)goto _LL3FF;_tmp76F=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp76C)->f1;_LL3FE: Cyc_Toc_aggrdecl_to_c(_tmp76F);goto _LL3F8;_LL3FF: if(*((int*)
_tmp76C)!= 6)goto _LL401;_tmp770=((struct Cyc_Absyn_Enum_d_struct*)_tmp76C)->f1;
_LL400: if(_tmp770->fields != 0){struct Cyc_List_List*_tmp773=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp770->fields))->v;for(0;_tmp773 != 0;_tmp773=
_tmp773->tl){struct Cyc_Absyn_Enumfield*_tmp774=(struct Cyc_Absyn_Enumfield*)
_tmp773->hd;if(_tmp774->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp774->tag));}}goto _LL3F8;_LL401: if(*((int*)_tmp76C)!= 7)goto
_LL403;_tmp771=((struct Cyc_Absyn_Typedef_d_struct*)_tmp76C)->f1;_LL402: _tmp771->defn=({
struct Cyc_Core_Opt*_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp771->defn))->v);_tmp775;});goto
_LL3F8;_LL403: if(*((int*)_tmp76C)!= 2)goto _LL405;_LL404: goto _LL406;_LL405: if(*((
int*)_tmp76C)!= 3)goto _LL407;_LL406: goto _LL408;_LL407: if(*((int*)_tmp76C)!= 5)
goto _LL409;_LL408: goto _LL40A;_LL409: if(*((int*)_tmp76C)!= 8)goto _LL40B;_LL40A:
goto _LL40C;_LL40B: if(*((int*)_tmp76C)!= 9)goto _LL40D;_LL40C: goto _LL40E;_LL40D:
if(*((int*)_tmp76C)!= 10)goto _LL40F;_LL40E: goto _LL410;_LL40F: if(*((int*)_tmp76C)
!= 11)goto _LL411;_LL410:({void*_tmp776[0]={};Cyc_Tcutil_terr(d->loc,({const char*
_tmp777="Cyclone declaration within C code";_tag_dynforward(_tmp777,sizeof(char),
_get_zero_arr_size(_tmp777,34));}),_tag_dynforward(_tmp776,sizeof(void*),0));});
goto _LL3F8;_LL411: if((int)_tmp76C != 0)goto _LL413;_LL412: goto _LL414;_LL413: if((
int)_tmp76C != 1)goto _LL3F8;_LL414: goto _LL3F8;_LL3F8:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp778=(void*)s->r;struct Cyc_Absyn_Exp*_tmp779;
struct Cyc_Absyn_Stmt*_tmp77A;struct Cyc_Absyn_Stmt*_tmp77B;struct Cyc_Absyn_Exp*
_tmp77C;struct Cyc_Absyn_Exp*_tmp77D;struct Cyc_Absyn_Stmt*_tmp77E;struct Cyc_Absyn_Stmt*
_tmp77F;struct _tuple3 _tmp780;struct Cyc_Absyn_Exp*_tmp781;struct Cyc_Absyn_Stmt*
_tmp782;struct Cyc_Absyn_Exp*_tmp783;struct _tuple3 _tmp784;struct Cyc_Absyn_Exp*
_tmp785;struct _tuple3 _tmp786;struct Cyc_Absyn_Exp*_tmp787;struct Cyc_Absyn_Stmt*
_tmp788;struct Cyc_Absyn_Exp*_tmp789;struct Cyc_List_List*_tmp78A;struct Cyc_Absyn_Decl*
_tmp78B;struct Cyc_Absyn_Stmt*_tmp78C;struct Cyc_Absyn_Stmt*_tmp78D;struct _tuple3
_tmp78E;struct Cyc_Absyn_Exp*_tmp78F;_LL416: if(_tmp778 <= (void*)1)goto _LL428;if(*((
int*)_tmp778)!= 0)goto _LL418;_tmp779=((struct Cyc_Absyn_Exp_s_struct*)_tmp778)->f1;
_LL417: Cyc_Toc_exptypes_to_c(_tmp779);goto _LL415;_LL418: if(*((int*)_tmp778)!= 1)
goto _LL41A;_tmp77A=((struct Cyc_Absyn_Seq_s_struct*)_tmp778)->f1;_tmp77B=((struct
Cyc_Absyn_Seq_s_struct*)_tmp778)->f2;_LL419: Cyc_Toc_stmttypes_to_c(_tmp77A);Cyc_Toc_stmttypes_to_c(
_tmp77B);goto _LL415;_LL41A: if(*((int*)_tmp778)!= 2)goto _LL41C;_tmp77C=((struct
Cyc_Absyn_Return_s_struct*)_tmp778)->f1;_LL41B: if(_tmp77C != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp77C);goto _LL415;_LL41C: if(*((int*)_tmp778)!= 3)goto
_LL41E;_tmp77D=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp778)->f1;_tmp77E=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp778)->f2;_tmp77F=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp778)->f3;_LL41D: Cyc_Toc_exptypes_to_c(_tmp77D);Cyc_Toc_stmttypes_to_c(
_tmp77E);Cyc_Toc_stmttypes_to_c(_tmp77F);goto _LL415;_LL41E: if(*((int*)_tmp778)!= 
4)goto _LL420;_tmp780=((struct Cyc_Absyn_While_s_struct*)_tmp778)->f1;_tmp781=
_tmp780.f1;_tmp782=((struct Cyc_Absyn_While_s_struct*)_tmp778)->f2;_LL41F: Cyc_Toc_exptypes_to_c(
_tmp781);Cyc_Toc_stmttypes_to_c(_tmp782);goto _LL415;_LL420: if(*((int*)_tmp778)!= 
8)goto _LL422;_tmp783=((struct Cyc_Absyn_For_s_struct*)_tmp778)->f1;_tmp784=((
struct Cyc_Absyn_For_s_struct*)_tmp778)->f2;_tmp785=_tmp784.f1;_tmp786=((struct
Cyc_Absyn_For_s_struct*)_tmp778)->f3;_tmp787=_tmp786.f1;_tmp788=((struct Cyc_Absyn_For_s_struct*)
_tmp778)->f4;_LL421: Cyc_Toc_exptypes_to_c(_tmp783);Cyc_Toc_exptypes_to_c(_tmp785);
Cyc_Toc_exptypes_to_c(_tmp787);Cyc_Toc_stmttypes_to_c(_tmp788);goto _LL415;_LL422:
if(*((int*)_tmp778)!= 9)goto _LL424;_tmp789=((struct Cyc_Absyn_Switch_s_struct*)
_tmp778)->f1;_tmp78A=((struct Cyc_Absyn_Switch_s_struct*)_tmp778)->f2;_LL423: Cyc_Toc_exptypes_to_c(
_tmp789);for(0;_tmp78A != 0;_tmp78A=_tmp78A->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp78A->hd)->body);}goto _LL415;_LL424: if(*((int*)
_tmp778)!= 11)goto _LL426;_tmp78B=((struct Cyc_Absyn_Decl_s_struct*)_tmp778)->f1;
_tmp78C=((struct Cyc_Absyn_Decl_s_struct*)_tmp778)->f2;_LL425: Cyc_Toc_decltypes_to_c(
_tmp78B);Cyc_Toc_stmttypes_to_c(_tmp78C);goto _LL415;_LL426: if(*((int*)_tmp778)!= 
13)goto _LL428;_tmp78D=((struct Cyc_Absyn_Do_s_struct*)_tmp778)->f1;_tmp78E=((
struct Cyc_Absyn_Do_s_struct*)_tmp778)->f2;_tmp78F=_tmp78E.f1;_LL427: Cyc_Toc_stmttypes_to_c(
_tmp78D);Cyc_Toc_exptypes_to_c(_tmp78F);goto _LL415;_LL428: if((int)_tmp778 != 0)
goto _LL42A;_LL429: goto _LL42B;_LL42A: if(_tmp778 <= (void*)1)goto _LL42C;if(*((int*)
_tmp778)!= 5)goto _LL42C;_LL42B: goto _LL42D;_LL42C: if(_tmp778 <= (void*)1)goto
_LL42E;if(*((int*)_tmp778)!= 6)goto _LL42E;_LL42D: goto _LL42F;_LL42E: if(_tmp778 <= (
void*)1)goto _LL430;if(*((int*)_tmp778)!= 7)goto _LL430;_LL42F: goto _LL415;_LL430:
if(_tmp778 <= (void*)1)goto _LL432;if(*((int*)_tmp778)!= 10)goto _LL432;_LL431: goto
_LL433;_LL432: if(_tmp778 <= (void*)1)goto _LL434;if(*((int*)_tmp778)!= 12)goto
_LL434;_LL433: goto _LL435;_LL434: if(_tmp778 <= (void*)1)goto _LL436;if(*((int*)
_tmp778)!= 14)goto _LL436;_LL435: goto _LL437;_LL436: if(_tmp778 <= (void*)1)goto
_LL438;if(*((int*)_tmp778)!= 15)goto _LL438;_LL437: goto _LL439;_LL438: if(_tmp778 <= (
void*)1)goto _LL415;if(*((int*)_tmp778)!= 16)goto _LL415;_LL439:({void*_tmp790[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp791="Cyclone statement in C code";
_tag_dynforward(_tmp791,sizeof(char),_get_zero_arr_size(_tmp791,28));}),
_tag_dynforward(_tmp790,sizeof(void*),0));});goto _LL415;_LL415:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({
void*_tmp792[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp793="decls_to_c: not at toplevel!";
_tag_dynforward(_tmp793,sizeof(char),_get_zero_arr_size(_tmp793,29));}),
_tag_dynforward(_tmp792,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp794=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp795;struct Cyc_Absyn_Fndecl*
_tmp796;struct Cyc_Absyn_Aggrdecl*_tmp797;struct Cyc_Absyn_Tuniondecl*_tmp798;
struct Cyc_Absyn_Enumdecl*_tmp799;struct Cyc_Absyn_Typedefdecl*_tmp79A;struct Cyc_List_List*
_tmp79B;struct Cyc_List_List*_tmp79C;struct Cyc_List_List*_tmp79D;struct Cyc_List_List*
_tmp79E;_LL43B: if(_tmp794 <= (void*)2)goto _LL44B;if(*((int*)_tmp794)!= 0)goto
_LL43D;_tmp795=((struct Cyc_Absyn_Var_d_struct*)_tmp794)->f1;_LL43C: {struct
_tuple1*_tmp79F=_tmp795->name;if((void*)_tmp795->sc == (void*)4)_tmp79F=({struct
_tuple1*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7A1;(_tmp7A1.Rel_n).tag=1;(_tmp7A1.Rel_n).f1=0;
_tmp7A1;});_tmp7A0->f2=(*_tmp79F).f2;_tmp7A0;});if(_tmp795->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp795->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp795->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp795->name,Cyc_Absyn_varb_exp(_tmp79F,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({
struct Cyc_Absyn_Global_b_struct _tmp7A3;_tmp7A3.tag=0;_tmp7A3.f1=_tmp795;_tmp7A3;});
_tmp7A2;}),0));_tmp795->name=_tmp79F;(void*)(_tmp795->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp795->sc));(void*)(_tmp795->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp795->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7A4=_cycalloc(
sizeof(*_tmp7A4));_tmp7A4->hd=d;_tmp7A4->tl=Cyc_Toc_result_decls;_tmp7A4;});goto
_LL43A;}_LL43D: if(*((int*)_tmp794)!= 1)goto _LL43F;_tmp796=((struct Cyc_Absyn_Fn_d_struct*)
_tmp794)->f1;_LL43E: {struct _tuple1*_tmp7A5=_tmp796->name;if((void*)_tmp796->sc
== (void*)4){_tmp7A5=({struct _tuple1*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6->f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7A7;(_tmp7A7.Rel_n).tag=
1;(_tmp7A7.Rel_n).f1=0;_tmp7A7;});_tmp7A6->f2=(*_tmp7A5).f2;_tmp7A6;});(void*)(
_tmp796->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,nv,_tmp796->name,Cyc_Absyn_var_exp(
_tmp7A5,0));_tmp796->name=_tmp7A5;Cyc_Toc_fndecl_to_c(nv,_tmp796,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8->hd=d;_tmp7A8->tl=
Cyc_Toc_result_decls;_tmp7A8;});goto _LL43A;}_LL43F: if(*((int*)_tmp794)!= 2)goto
_LL441;_LL440: goto _LL442;_LL441: if(*((int*)_tmp794)!= 3)goto _LL443;_LL442:({void*
_tmp7A9[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp7AA="letdecl at toplevel";_tag_dynforward(_tmp7AA,sizeof(char),
_get_zero_arr_size(_tmp7AA,20));}),_tag_dynforward(_tmp7A9,sizeof(void*),0));});
_LL443: if(*((int*)_tmp794)!= 4)goto _LL445;_tmp797=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp794)->f1;_LL444: Cyc_Toc_aggrdecl_to_c(_tmp797);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB->hd=d;_tmp7AB->tl=Cyc_Toc_result_decls;
_tmp7AB;});goto _LL43A;_LL445: if(*((int*)_tmp794)!= 5)goto _LL447;_tmp798=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp794)->f1;_LL446: if(_tmp798->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp798);else{Cyc_Toc_tuniondecl_to_c(_tmp798);}goto _LL43A;_LL447: if(*((int*)
_tmp794)!= 6)goto _LL449;_tmp799=((struct Cyc_Absyn_Enum_d_struct*)_tmp794)->f1;
_LL448: Cyc_Toc_enumdecl_to_c(nv,_tmp799);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC->hd=d;_tmp7AC->tl=Cyc_Toc_result_decls;
_tmp7AC;});goto _LL43A;_LL449: if(*((int*)_tmp794)!= 7)goto _LL44B;_tmp79A=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp794)->f1;_LL44A: _tmp79A->name=_tmp79A->name;
_tmp79A->tvs=0;if(_tmp79A->defn != 0)_tmp79A->defn=({struct Cyc_Core_Opt*_tmp7AD=
_cycalloc(sizeof(*_tmp7AD));_tmp7AD->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp79A->defn))->v);_tmp7AD;});else{void*_tmp7AE=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp79A->kind))->v;_LL458: if((int)
_tmp7AE != 2)goto _LL45A;_LL459: _tmp79A->defn=({struct Cyc_Core_Opt*_tmp7AF=
_cycalloc(sizeof(*_tmp7AF));_tmp7AF->v=(void*)Cyc_Absyn_void_star_typ();_tmp7AF;});
goto _LL457;_LL45A:;_LL45B: _tmp79A->defn=({struct Cyc_Core_Opt*_tmp7B0=_cycalloc(
sizeof(*_tmp7B0));_tmp7B0->v=(void*)((void*)0);_tmp7B0;});goto _LL457;_LL457:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));
_tmp7B1->hd=d;_tmp7B1->tl=Cyc_Toc_result_decls;_tmp7B1;});goto _LL43A;_LL44B: if((
int)_tmp794 != 0)goto _LL44D;_LL44C: goto _LL44E;_LL44D: if((int)_tmp794 != 1)goto
_LL44F;_LL44E: goto _LL43A;_LL44F: if(_tmp794 <= (void*)2)goto _LL451;if(*((int*)
_tmp794)!= 8)goto _LL451;_tmp79B=((struct Cyc_Absyn_Namespace_d_struct*)_tmp794)->f2;
_LL450: _tmp79C=_tmp79B;goto _LL452;_LL451: if(_tmp794 <= (void*)2)goto _LL453;if(*((
int*)_tmp794)!= 9)goto _LL453;_tmp79C=((struct Cyc_Absyn_Using_d_struct*)_tmp794)->f2;
_LL452: _tmp79D=_tmp79C;goto _LL454;_LL453: if(_tmp794 <= (void*)2)goto _LL455;if(*((
int*)_tmp794)!= 10)goto _LL455;_tmp79D=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp794)->f1;_LL454: nv=Cyc_Toc_decls_to_c(r,nv,_tmp79D,top,cinclude);goto _LL43A;
_LL455: if(_tmp794 <= (void*)2)goto _LL43A;if(*((int*)_tmp794)!= 11)goto _LL43A;
_tmp79E=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp794)->f1;_LL456: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp79E,top,1);goto _LL43A;_LL43A:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{
struct _RegionHandle _tmp7B2=_new_region("start");struct _RegionHandle*start=&
_tmp7B2;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,
1,0);;_pop_region(start);}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
