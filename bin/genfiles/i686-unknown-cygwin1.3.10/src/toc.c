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
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _dynforward_ptr Cyc_Position_string_of_segment(
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
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
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
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
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
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
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
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
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*aggrdecls;
struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Tcenv_Fenv*
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
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[15];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[11];extern char
Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*
f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict*consumed;
struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;
};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct
Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
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
_tmpA + 7};static struct _tuple1 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmpC[7]="setjmp";static struct
_dynforward_ptr Cyc_Toc_setjmp_str={_tmpC,_tmpC + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmpE[14]="_push_handler";
static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmpE,_tmpE + 14};static
struct _tuple1 Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char
_tmp10[13]="_pop_handler";static struct _dynforward_ptr Cyc_Toc__pop_handler_str={
_tmp10,_tmp10 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp12[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp12,_tmp12 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
char _tmp14[14]="_npop_handler";static struct _dynforward_ptr Cyc_Toc__npop_handler_str={
_tmp14,_tmp14 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp16[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp16,_tmp16 + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={
0,(void*)((void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static
char _tmp18[28]="_check_known_subscript_null";static struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={
_tmp18,_tmp18 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp1A[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp1A,_tmp1A + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp1C[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp1C,
_tmp1C + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_dynforward_subscript_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp1E[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp1E,_tmp1E + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp20[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp20,_tmp20 + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_re={
1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=& Cyc_Toc__dynforward_ptr_ev;
static char _tmp22[15]="_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_str={
_tmp22,_tmp22 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp24[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp24,_tmp24 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dynforward_re={
1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=& Cyc_Toc__tag_dynforward_ev;
static char _tmp26[15]="_tag_dyneither";static struct _dynforward_ptr Cyc_Toc__tag_dyneither_str={
_tmp26,_tmp26 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp28[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp28,_tmp28 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(void*)((
void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp2A[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp2A,_tmp2A + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)
0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)((void*)&
Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp2C[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp2C,_tmp2C + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(void*)((
void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp2E[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp2E,_tmp2E + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)((
void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp30[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp30,_tmp30 + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_dynforward_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(void*)((
void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp32[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp32,_tmp32 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)
0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)((void*)&
Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp34[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp34,_tmp34 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={
1,& Cyc_Toc__get_zero_arr_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp36[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp36,_tmp36 + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dynforward_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(void*)((
void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp38[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp38,_tmp38 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)
0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)((void*)&
Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp3A[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp3A,_tmp3A + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={
1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;
static char _tmp3C[29]="_dynforward_ptr_inplace_plus";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp3C,_tmp3C + 29};static struct _tuple1
Cyc_Toc__dynforward_ptr_inplace_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_ev;static char _tmp3E[28]="_dyneither_ptr_inplace_plus";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp3E,
_tmp3E + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dyneither_ptr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp40[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp40,_tmp40 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp42[34]="_dynforward_ptr_inplace_plus_post";static struct
_dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmp42,_tmp42 + 34};
static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_inplace_plus_post_re={1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmp44[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp44,
_tmp44 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp46[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmp46,_tmp46 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_to_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={
1,& Cyc_Toc__dynforward_to_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_to_dyneither_ev={0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=&
Cyc_Toc__dynforward_to_dyneither_ev;static char _tmp48[25]="_dyneither_to_dynforward";
static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={_tmp48,_tmp48 + 
25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dyneither_to_dynforward_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={
1,& Cyc_Toc__dyneither_to_dynforward_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_to_dynforward_ev={0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=&
Cyc_Toc__dyneither_to_dynforward_ev;static char _tmp4A[28]="_zero_arr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp4A,
_tmp4A + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp4C[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmp4C,_tmp4C + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp4E[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmp4E,_tmp4E + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char
_tmp50[17]="_cycalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cycalloc_atomic_str={
_tmp50,_tmp50 + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp52[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp52,_tmp52 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp54[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmp54,_tmp54 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={
1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={
0,(void*)((void*)& Cyc_Toc__region_malloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;
static char _tmp56[15]="_region_calloc";static struct _dynforward_ptr Cyc_Toc__region_calloc_str={
_tmp56,_tmp56 + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmp58[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmp58,_tmp58 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={
1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={
0,(void*)((void*)& Cyc_Toc__check_times_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static
char _tmp5A[12]="_new_region";static struct _dynforward_ptr Cyc_Toc__new_region_str={
_tmp5A,_tmp5A + 12};static struct _tuple1 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmp5C[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmp5C,_tmp5C + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={
1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={
0,(void*)((void*)& Cyc_Toc__push_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static
char _tmp5E[12]="_pop_region";static struct _dynforward_ptr Cyc_Toc__pop_region_str={
_tmp5E,_tmp5E + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmp60[14]="_reset_region";static struct _dynforward_ptr Cyc_Toc__reset_region_str={
_tmp60,_tmp60 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={
0,(void*)((void*)& Cyc_Toc__reset_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;
static char _tmp62[19]="_throw_arraybounds";static struct _dynforward_ptr Cyc_Toc__throw_arraybounds_str={
_tmp62,_tmp62 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={
1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmp64[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmp64,_tmp64 + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__dynforward_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={
1,& Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmp66[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp66,
_tmp66 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__dyneither_ptr_decrease_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;
static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_dynforward_ptr_typ_u={0,(void*)((
void*)0),& Cyc_Toc__dynforward_ptr_pr};static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={
10,{(void*)((void*)& Cyc_Toc_dynforward_ptr_typ_u),0}};static void*Cyc_Toc_dynforward_ptr_typ=(
void*)& Cyc_Toc_dynforward_ptr_typ_v;static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_dyneither_ptr_typ_u={
0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_AggrType_struct
Cyc_Toc_dyneither_ptr_typ_v={10,{(void*)((void*)& Cyc_Toc_dyneither_ptr_typ_u),0}};
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=Cyc_Absyn_skip_stmt(
0);_tmp6C;});return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp6D=(void*)e->r;void*_tmp6E;char _tmp6F;void*
_tmp70;short _tmp71;void*_tmp72;int _tmp73;void*_tmp74;long long _tmp75;void*_tmp76;
struct Cyc_Absyn_Exp*_tmp77;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp79;
struct Cyc_List_List*_tmp7A;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp7C;
_LL1: if(*((int*)_tmp6D)!= 0)goto _LL3;_tmp6E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp6D)->f1;if(_tmp6E <= (void*)1)goto _LL3;if(*((int*)_tmp6E)!= 0)goto _LL3;_tmp6F=((
struct Cyc_Absyn_Char_c_struct*)_tmp6E)->f2;_LL2: return _tmp6F == '\000';_LL3: if(*((
int*)_tmp6D)!= 0)goto _LL5;_tmp70=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp6D)->f1;
if(_tmp70 <= (void*)1)goto _LL5;if(*((int*)_tmp70)!= 1)goto _LL5;_tmp71=((struct Cyc_Absyn_Short_c_struct*)
_tmp70)->f2;_LL4: return _tmp71 == 0;_LL5: if(*((int*)_tmp6D)!= 0)goto _LL7;_tmp72=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp6D)->f1;if(_tmp72 <= (void*)1)goto
_LL7;if(*((int*)_tmp72)!= 2)goto _LL7;_tmp73=((struct Cyc_Absyn_Int_c_struct*)
_tmp72)->f2;_LL6: return _tmp73 == 0;_LL7: if(*((int*)_tmp6D)!= 0)goto _LL9;_tmp74=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp6D)->f1;if(_tmp74 <= (void*)1)goto
_LL9;if(*((int*)_tmp74)!= 3)goto _LL9;_tmp75=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp74)->f2;_LL8: return _tmp75 == 0;_LL9: if(*((int*)_tmp6D)!= 0)goto _LLB;_tmp76=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp6D)->f1;if((int)_tmp76 != 0)goto _LLB;
_LLA: return 1;_LLB: if(*((int*)_tmp6D)!= 15)goto _LLD;_tmp77=((struct Cyc_Absyn_Cast_e_struct*)
_tmp6D)->f2;_LLC: return Cyc_Toc_is_zero(_tmp77);_LLD: if(*((int*)_tmp6D)!= 26)goto
_LLF;_tmp78=((struct Cyc_Absyn_Tuple_e_struct*)_tmp6D)->f1;_LLE: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp78);_LLF: if(*((int*)_tmp6D)!= 28)goto _LL11;_tmp79=((struct Cyc_Absyn_Array_e_struct*)
_tmp6D)->f1;_LL10: _tmp7A=_tmp79;goto _LL12;_LL11: if(*((int*)_tmp6D)!= 30)goto
_LL13;_tmp7A=((struct Cyc_Absyn_Struct_e_struct*)_tmp6D)->f3;_LL12: _tmp7B=_tmp7A;
goto _LL14;_LL13: if(*((int*)_tmp6D)!= 27)goto _LL15;_tmp7B=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp6D)->f2;_LL14: _tmp7C=_tmp7B;goto _LL16;_LL15: if(*((int*)_tmp6D)!= 36)goto
_LL17;_tmp7C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp6D)->f2;_LL16: for(0;
_tmp7C != 0;_tmp7C=_tmp7C->tl){if(!Cyc_Toc_is_zero((*((struct _tuple6*)_tmp7C->hd)).f2))
return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}static int Cyc_Toc_is_nullable(void*t){
void*_tmp7D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp7E;struct Cyc_Absyn_PtrAtts
_tmp7F;struct Cyc_Absyn_Conref*_tmp80;_LL1A: if(_tmp7D <= (void*)4)goto _LL1C;if(*((
int*)_tmp7D)!= 4)goto _LL1C;_tmp7E=((struct Cyc_Absyn_PointerType_struct*)_tmp7D)->f1;
_tmp7F=_tmp7E.ptr_atts;_tmp80=_tmp7F.nullable;_LL1B: return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp80);_LL1C:;_LL1D:({void*_tmp81[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp82="is_nullable";_tag_dynforward(_tmp82,sizeof(char),_get_zero_arr_size(
_tmp82,12));}),_tag_dynforward(_tmp81,sizeof(void*),0));});_LL19:;}static struct
_tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct _dynforward_ptr tag){
return({struct _tuple1*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->f1=(*x).f1;
_tmp83->f2=({struct _dynforward_ptr*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=(
struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)*(*x).f2,(struct
_dynforward_ptr)tag);_tmp84;});_tmp83;});}struct _tuple7{struct _dynforward_ptr*f1;
struct Cyc_List_List*f2;};static struct _dynforward_ptr*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){{struct Cyc_List_List*_tmp85=Cyc_Toc_tuple_types;for(0;
_tmp85 != 0;_tmp85=_tmp85->tl){struct _tuple7 _tmp87;struct _dynforward_ptr*_tmp88;
struct Cyc_List_List*_tmp89;struct _tuple7*_tmp86=(struct _tuple7*)_tmp85->hd;
_tmp87=*_tmp86;_tmp88=_tmp87.f1;_tmp89=_tmp87.f2;{struct Cyc_List_List*_tmp8A=
tqs0;for(0;_tmp8A != 0  && _tmp89 != 0;(_tmp8A=_tmp8A->tl,_tmp89=_tmp89->tl)){if(!
Cyc_Tcutil_unify((*((struct _tuple4*)_tmp8A->hd)).f2,(void*)_tmp89->hd))break;}
if(_tmp8A == 0  && _tmp89 == 0)return _tmp88;}}}{struct _dynforward_ptr*x=({struct
_dynforward_ptr*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp9A;_tmp9A.tag=1;_tmp9A.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmp98[1]={& _tmp9A};Cyc_aprintf(({
const char*_tmp99="_tuple%d";_tag_dynforward(_tmp99,sizeof(char),
_get_zero_arr_size(_tmp99,9));}),_tag_dynforward(_tmp98,sizeof(void*),1));}});
_tmp97;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmp8B=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmp8B=({struct Cyc_List_List*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->hd=({
struct Cyc_Absyn_Aggrfield*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->name=Cyc_Absyn_fieldname(
i);_tmp8D->tq=Cyc_Toc_mt_tq;_tmp8D->type=(void*)((void*)ts2->hd);_tmp8D->width=0;
_tmp8D->attributes=0;_tmp8D;});_tmp8C->tl=_tmp8B;_tmp8C;});}}_tmp8B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8B);{struct Cyc_Absyn_Aggrdecl*
_tmp8E=({struct Cyc_Absyn_Aggrdecl*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->kind=(
void*)((void*)0);_tmp94->sc=(void*)((void*)2);_tmp94->name=({struct _tuple1*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->f1=Cyc_Absyn_rel_ns_null;_tmp96->f2=x;
_tmp96;});_tmp94->tvs=0;_tmp94->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp95=
_cycalloc(sizeof(*_tmp95));_tmp95->exist_vars=0;_tmp95->rgn_po=0;_tmp95->fields=
_tmp8B;_tmp95;});_tmp94->attributes=0;_tmp94;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp91;_tmp91.tag=4;_tmp91.f1=_tmp8E;_tmp91;});_tmp90;}),0);_tmp8F->tl=Cyc_Toc_result_decls;
_tmp8F;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp92=_cycalloc(sizeof(*
_tmp92));_tmp92->hd=({struct _tuple7*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->f1=
x;_tmp93->f2=ts;_tmp93;});_tmp92->tl=Cyc_Toc_tuple_types;_tmp92;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmp9B=_cycalloc(sizeof(*
_tmp9B));_tmp9B->f1=(void*)0;_tmp9B->f2=({struct _dynforward_ptr*_tmp9C=_cycalloc(
sizeof(*_tmp9C));_tmp9C[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp9F;_tmp9F.tag=1;_tmp9F.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmp9D[1]={& _tmp9F};Cyc_aprintf(({const char*_tmp9E="_tmp%X";_tag_dynforward(
_tmp9E,sizeof(char),_get_zero_arr_size(_tmp9E,7));}),_tag_dynforward(_tmp9D,
sizeof(void*),1));}});_tmp9C;});_tmp9B;});}static struct _dynforward_ptr*Cyc_Toc_fresh_label(){
return({struct _dynforward_ptr*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=(
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmpA3;_tmpA3.tag=1;_tmpA3.f1=(
unsigned int)Cyc_Toc_fresh_label_counter ++;{void*_tmpA1[1]={& _tmpA3};Cyc_aprintf(({
const char*_tmpA2="_LL%X";_tag_dynforward(_tmpA2,sizeof(char),_get_zero_arr_size(
_tmpA2,6));}),_tag_dynforward(_tmpA1,sizeof(void*),1));}});_tmpA0;});}static
struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,struct
_tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmpA4=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmpA4))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpA4->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmpA4->hd)->typs == 0)
++ ans;_tmpA4=_tmpA4->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmpA5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmpA5 != 0;_tmpA5=_tmpA5->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmpA5->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmpA7;struct Cyc_Absyn_Tqual
_tmpA8;void*_tmpA9;struct _tuple2 _tmpA6=*a;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;
_tmpA9=_tmpA6.f3;return({struct _tuple2*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->f1=
_tmpA7;_tmpAA->f2=_tmpA8;_tmpAA->f3=Cyc_Toc_typ_to_c(_tmpA9);_tmpAA;});}static
struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual _tmpAC;
void*_tmpAD;struct _tuple4 _tmpAB=*x;_tmpAC=_tmpAB.f1;_tmpAD=_tmpAB.f2;return({
struct _tuple4*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->f1=_tmpAC;_tmpAE->f2=Cyc_Toc_typ_to_c(
_tmpAD);_tmpAE;});}static void*Cyc_Toc_typ_to_c_array(void*t){void*_tmpAF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpB0;void*_tmpB1;struct Cyc_Absyn_Tqual _tmpB2;
struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Conref*_tmpB4;struct Cyc_Position_Segment*
_tmpB5;struct Cyc_Core_Opt*_tmpB6;struct Cyc_Core_Opt _tmpB7;void*_tmpB8;_LL1F: if(
_tmpAF <= (void*)4)goto _LL23;if(*((int*)_tmpAF)!= 7)goto _LL21;_tmpB0=((struct Cyc_Absyn_ArrayType_struct*)
_tmpAF)->f1;_tmpB1=(void*)_tmpB0.elt_type;_tmpB2=_tmpB0.tq;_tmpB3=_tmpB0.num_elts;
_tmpB4=_tmpB0.zero_term;_tmpB5=_tmpB0.zt_loc;_LL20: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmpB1),_tmpB2,_tmpB3,Cyc_Absyn_false_conref,_tmpB5);_LL21: if(*((int*)_tmpAF)!= 
0)goto _LL23;_tmpB6=((struct Cyc_Absyn_Evar_struct*)_tmpAF)->f2;if(_tmpB6 == 0)goto
_LL23;_tmpB7=*_tmpB6;_tmpB8=(void*)_tmpB7.v;_LL22: return Cyc_Toc_typ_to_c_array(
_tmpB8);_LL23:;_LL24: return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->name=f->name;_tmpB9->tq=Cyc_Toc_mt_tq;
_tmpB9->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmpB9->width=f->width;
_tmpB9->attributes=f->attributes;_tmpB9;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmpBA=
t;struct Cyc_Core_Opt*_tmpBB;struct Cyc_Core_Opt*_tmpBC;struct Cyc_Core_Opt _tmpBD;
void*_tmpBE;struct Cyc_Absyn_Tvar*_tmpBF;struct Cyc_Absyn_TunionInfo _tmpC0;void*
_tmpC1;struct Cyc_Absyn_Tuniondecl**_tmpC2;struct Cyc_Absyn_Tuniondecl*_tmpC3;
struct Cyc_Absyn_TunionFieldInfo _tmpC4;void*_tmpC5;struct Cyc_Absyn_Tuniondecl*
_tmpC6;struct Cyc_Absyn_Tunionfield*_tmpC7;struct Cyc_Absyn_PtrInfo _tmpC8;void*
_tmpC9;struct Cyc_Absyn_Tqual _tmpCA;struct Cyc_Absyn_PtrAtts _tmpCB;struct Cyc_Absyn_Conref*
_tmpCC;struct Cyc_Absyn_ArrayInfo _tmpCD;void*_tmpCE;struct Cyc_Absyn_Tqual _tmpCF;
struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Position_Segment*_tmpD1;struct Cyc_Absyn_FnInfo
_tmpD2;void*_tmpD3;struct Cyc_List_List*_tmpD4;int _tmpD5;struct Cyc_Absyn_VarargInfo*
_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD8;void*_tmpD9;struct
Cyc_List_List*_tmpDA;struct Cyc_Absyn_AggrInfo _tmpDB;void*_tmpDC;struct Cyc_List_List*
_tmpDD;struct _tuple1*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple1*_tmpE0;
struct Cyc_List_List*_tmpE1;struct Cyc_Absyn_Typedefdecl*_tmpE2;void**_tmpE3;void*
_tmpE4;_LL26: if((int)_tmpBA != 0)goto _LL28;_LL27: return t;_LL28: if(_tmpBA <= (void*)
4)goto _LL3A;if(*((int*)_tmpBA)!= 0)goto _LL2A;_tmpBB=((struct Cyc_Absyn_Evar_struct*)
_tmpBA)->f2;if(_tmpBB != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(*((
int*)_tmpBA)!= 0)goto _LL2C;_tmpBC=((struct Cyc_Absyn_Evar_struct*)_tmpBA)->f2;if(
_tmpBC == 0)goto _LL2C;_tmpBD=*_tmpBC;_tmpBE=(void*)_tmpBD.v;_LL2B: return Cyc_Toc_typ_to_c(
_tmpBE);_LL2C: if(*((int*)_tmpBA)!= 1)goto _LL2E;_tmpBF=((struct Cyc_Absyn_VarType_struct*)
_tmpBA)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(_tmpBF)== (void*)0)return(void*)0;
else{return Cyc_Absyn_void_star_typ();}_LL2E: if(*((int*)_tmpBA)!= 2)goto _LL30;
_tmpC0=((struct Cyc_Absyn_TunionType_struct*)_tmpBA)->f1;_tmpC1=(void*)_tmpC0.tunion_info;
if(*((int*)_tmpC1)!= 1)goto _LL30;_tmpC2=((struct Cyc_Absyn_KnownTunion_struct*)
_tmpC1)->f1;_tmpC3=*_tmpC2;_LL2F: if(_tmpC3->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmpC3->name,({const char*_tmpE5="_union";
_tag_dynforward(_tmpE5,sizeof(char),_get_zero_arr_size(_tmpE5,7));})));else{
return Cyc_Absyn_void_star_typ();}_LL30: if(*((int*)_tmpBA)!= 2)goto _LL32;_LL31:({
void*_tmpE6[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmpE7="unresolved TunionType";_tag_dynforward(_tmpE7,sizeof(char),
_get_zero_arr_size(_tmpE7,22));}),_tag_dynforward(_tmpE6,sizeof(void*),0));});
_LL32: if(*((int*)_tmpBA)!= 3)goto _LL34;_tmpC4=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpBA)->f1;_tmpC5=(void*)_tmpC4.field_info;if(*((int*)_tmpC5)!= 1)goto _LL34;
_tmpC6=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpC5)->f1;_tmpC7=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpC5)->f2;_LL33: if(_tmpC6->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmpC6->name,({const char*_tmpE8="_union";_tag_dynforward(_tmpE8,sizeof(char),
_get_zero_arr_size(_tmpE8,7));})));if(_tmpC7->typs == 0){if(_tmpC6->is_xtunion)
return Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);
else{return Cyc_Absyn_uint_typ;}}else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmpC7->name,({const char*_tmpE9="_struct";_tag_dynforward(_tmpE9,sizeof(char),
_get_zero_arr_size(_tmpE9,8));})));}_LL34: if(*((int*)_tmpBA)!= 3)goto _LL36;_LL35:({
void*_tmpEA[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmpEB="unresolved TunionFieldType";_tag_dynforward(_tmpEB,sizeof(char),
_get_zero_arr_size(_tmpEB,27));}),_tag_dynforward(_tmpEA,sizeof(void*),0));});
_LL36: if(*((int*)_tmpBA)!= 4)goto _LL38;_tmpC8=((struct Cyc_Absyn_PointerType_struct*)
_tmpBA)->f1;_tmpC9=(void*)_tmpC8.elt_typ;_tmpCA=_tmpC8.elt_tq;_tmpCB=_tmpC8.ptr_atts;
_tmpCC=_tmpCB.bounds;_LL37: _tmpC9=Cyc_Toc_typ_to_c_array(_tmpC9);{void*_tmpEC=(
void*)(Cyc_Absyn_compress_conref(_tmpCC))->v;void*_tmpED;void*_tmpEE;_LL61: if((
int)_tmpEC != 0)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmpEC <= (void*)1)goto _LL67;
if(*((int*)_tmpEC)!= 0)goto _LL65;_tmpED=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpEC)->f1;if((int)_tmpED != 0)goto _LL65;_LL64: return Cyc_Toc_dynforward_ptr_typ;
_LL65: if(*((int*)_tmpEC)!= 0)goto _LL67;_tmpEE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpEC)->f1;if((int)_tmpEE != 1)goto _LL67;_LL66: return Cyc_Toc_dyneither_ptr_typ;
_LL67:;_LL68: return Cyc_Absyn_star_typ(_tmpC9,(void*)2,_tmpCA,Cyc_Absyn_false_conref);
_LL60:;}_LL38: if(*((int*)_tmpBA)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)
_tmpBA != 1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmpBA <= (void*)4)goto _LL56;if(*((
int*)_tmpBA)!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(*((int*)_tmpBA)!= 7)goto _LL40;
_tmpCD=((struct Cyc_Absyn_ArrayType_struct*)_tmpBA)->f1;_tmpCE=(void*)_tmpCD.elt_type;
_tmpCF=_tmpCD.tq;_tmpD0=_tmpCD.num_elts;_tmpD1=_tmpCD.zt_loc;_LL3F: return Cyc_Absyn_array_typ(
Cyc_Toc_typ_to_c_array(_tmpCE),_tmpCF,_tmpD0,Cyc_Absyn_false_conref,_tmpD1);
_LL40: if(*((int*)_tmpBA)!= 8)goto _LL42;_tmpD2=((struct Cyc_Absyn_FnType_struct*)
_tmpBA)->f1;_tmpD3=(void*)_tmpD2.ret_typ;_tmpD4=_tmpD2.args;_tmpD5=_tmpD2.c_varargs;
_tmpD6=_tmpD2.cyc_varargs;_tmpD7=_tmpD2.attributes;_LL41: {struct Cyc_List_List*
_tmpEF=0;for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){void*_tmpF0=(void*)_tmpD7->hd;_LL6A:
if((int)_tmpF0 != 3)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)_tmpF0 != 4)goto _LL6E;
_LL6D: goto _LL6F;_LL6E: if(_tmpF0 <= (void*)17)goto _LL72;if(*((int*)_tmpF0)!= 3)
goto _LL70;_LL6F: continue;_LL70: if(*((int*)_tmpF0)!= 4)goto _LL72;_LL71: continue;
_LL72:;_LL73: _tmpEF=({struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*_tmpF1));
_tmpF1->hd=(void*)((void*)_tmpD7->hd);_tmpF1->tl=_tmpEF;_tmpF1;});goto _LL69;
_LL69:;}{struct Cyc_List_List*_tmpF2=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(
struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmpD4);
if(_tmpD6 != 0){void*_tmpF3=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((void*)
_tmpD6->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct _tuple2*
_tmpF4=({struct _tuple2*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->f1=_tmpD6->name;
_tmpF6->f2=_tmpD6->tq;_tmpF6->f3=_tmpF3;_tmpF6;});_tmpF2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpF2,({
struct Cyc_List_List*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=_tmpF4;_tmpF5->tl=
0;_tmpF5;}));}return(void*)({struct Cyc_Absyn_FnType_struct*_tmpF7=_cycalloc(
sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_Absyn_FnType_struct _tmpF8;_tmpF8.tag=8;
_tmpF8.f1=({struct Cyc_Absyn_FnInfo _tmpF9;_tmpF9.tvars=0;_tmpF9.effect=0;_tmpF9.ret_typ=(
void*)Cyc_Toc_typ_to_c(_tmpD3);_tmpF9.args=_tmpF2;_tmpF9.c_varargs=_tmpD5;_tmpF9.cyc_varargs=
0;_tmpF9.rgn_po=0;_tmpF9.attributes=_tmpEF;_tmpF9;});_tmpF8;});_tmpF7;});}}_LL42:
if(*((int*)_tmpBA)!= 9)goto _LL44;_tmpD8=((struct Cyc_Absyn_TupleType_struct*)
_tmpBA)->f1;_LL43: _tmpD8=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpD8);{
struct _dynforward_ptr*_tmpFA=Cyc_Toc_add_tuple_type(_tmpD8);return Cyc_Absyn_strct(
_tmpFA);}_LL44: if(*((int*)_tmpBA)!= 11)goto _LL46;_tmpD9=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpBA)->f1;_tmpDA=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpBA)->f2;_LL45:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpFB=_cycalloc(sizeof(*
_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpFC;_tmpFC.tag=11;
_tmpFC.f1=(void*)_tmpD9;_tmpFC.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmpDA);_tmpFC;});_tmpFB;});_LL46: if(*((int*)_tmpBA)!= 10)goto _LL48;_tmpDB=((
struct Cyc_Absyn_AggrType_struct*)_tmpBA)->f1;_tmpDC=(void*)_tmpDB.aggr_info;
_tmpDD=_tmpDB.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*_tmpFD=Cyc_Absyn_get_known_aggrdecl(
_tmpDC);if((void*)_tmpFD->kind == (void*)1)return Cyc_Absyn_unionq_typ(_tmpFD->name);
return Cyc_Absyn_strctq(_tmpFD->name);}_LL48: if(*((int*)_tmpBA)!= 12)goto _LL4A;
_tmpDE=((struct Cyc_Absyn_EnumType_struct*)_tmpBA)->f1;_LL49: return t;_LL4A: if(*((
int*)_tmpBA)!= 13)goto _LL4C;_tmpDF=((struct Cyc_Absyn_AnonEnumType_struct*)_tmpBA)->f1;
_LL4B: Cyc_Toc_enumfields_to_c(_tmpDF);return t;_LL4C: if(*((int*)_tmpBA)!= 16)goto
_LL4E;_tmpE0=((struct Cyc_Absyn_TypedefType_struct*)_tmpBA)->f1;_tmpE1=((struct
Cyc_Absyn_TypedefType_struct*)_tmpBA)->f2;_tmpE2=((struct Cyc_Absyn_TypedefType_struct*)
_tmpBA)->f3;_tmpE3=((struct Cyc_Absyn_TypedefType_struct*)_tmpBA)->f4;_LL4D: if(
_tmpE3 == 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpFE=_cycalloc(
sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_TypedefType_struct _tmpFF;_tmpFF.tag=
16;_tmpFF.f1=_tmpE0;_tmpFF.f2=0;_tmpFF.f3=_tmpE2;_tmpFF.f4=0;_tmpFF;});_tmpFE;});
else{return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp100=_cycalloc(sizeof(*
_tmp100));_tmp100[0]=({struct Cyc_Absyn_TypedefType_struct _tmp101;_tmp101.tag=16;
_tmp101.f1=_tmpE0;_tmp101.f2=0;_tmp101.f3=_tmpE2;_tmp101.f4=({void**_tmp102=
_cycalloc(sizeof(*_tmp102));_tmp102[0]=Cyc_Toc_typ_to_c_array(*_tmpE3);_tmp102;});
_tmp101;});_tmp100;});}_LL4E: if(*((int*)_tmpBA)!= 14)goto _LL50;_LL4F: goto _LL51;
_LL50: if(*((int*)_tmpBA)!= 17)goto _LL52;_LL51: return Cyc_Absyn_uint_typ;_LL52: if(*((
int*)_tmpBA)!= 15)goto _LL54;_tmpE4=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpBA)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(*((int*)_tmpBA)!= 18)goto _LL56;_LL55:({void*_tmp103[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp104="Toc::typ_to_c: type translation passed a type integer";
_tag_dynforward(_tmp104,sizeof(char),_get_zero_arr_size(_tmp104,54));}),
_tag_dynforward(_tmp103,sizeof(void*),0));});_LL56: if((int)_tmpBA != 2)goto _LL58;
_LL57:({void*_tmp105[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp106="Toc::typ_to_c: type translation passed the heap region";
_tag_dynforward(_tmp106,sizeof(char),_get_zero_arr_size(_tmp106,55));}),
_tag_dynforward(_tmp105,sizeof(void*),0));});_LL58: if((int)_tmpBA != 3)goto _LL5A;
_LL59:({void*_tmp107[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp108="Toc::typ_to_c: type translation passed the unique region";
_tag_dynforward(_tmp108,sizeof(char),_get_zero_arr_size(_tmp108,57));}),
_tag_dynforward(_tmp107,sizeof(void*),0));});_LL5A: if(_tmpBA <= (void*)4)goto
_LL5C;if(*((int*)_tmpBA)!= 19)goto _LL5C;_LL5B: goto _LL5D;_LL5C: if(_tmpBA <= (void*)
4)goto _LL5E;if(*((int*)_tmpBA)!= 20)goto _LL5E;_LL5D: goto _LL5F;_LL5E: if(_tmpBA <= (
void*)4)goto _LL25;if(*((int*)_tmpBA)!= 21)goto _LL25;_LL5F:({void*_tmp109[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp10A="Toc::typ_to_c: type translation passed an effect";
_tag_dynforward(_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,49));}),
_tag_dynforward(_tmp109,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmp10B=t;struct Cyc_Absyn_ArrayInfo _tmp10C;void*_tmp10D;struct Cyc_Absyn_Tqual
_tmp10E;_LL75: if(_tmp10B <= (void*)4)goto _LL77;if(*((int*)_tmp10B)!= 7)goto _LL77;
_tmp10C=((struct Cyc_Absyn_ArrayType_struct*)_tmp10B)->f1;_tmp10D=(void*)_tmp10C.elt_type;
_tmp10E=_tmp10C.tq;_LL76: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp10D,(void*)
2,_tmp10E,Cyc_Absyn_false_conref),e);_LL77:;_LL78: return Cyc_Toc_cast_it(t,e);
_LL74:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp10F=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_ArrayInfo _tmp110;void*_tmp111;struct Cyc_Absyn_AggrInfo _tmp112;
void*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_Absyn_TunionFieldInfo _tmp115;
void*_tmp116;struct Cyc_Absyn_Tuniondecl*_tmp117;struct Cyc_Absyn_Tunionfield*
_tmp118;struct Cyc_List_List*_tmp119;_LL7A: if((int)_tmp10F != 0)goto _LL7C;_LL7B:
return 1;_LL7C: if(_tmp10F <= (void*)4)goto _LL84;if(*((int*)_tmp10F)!= 1)goto _LL7E;
_LL7D: return 0;_LL7E: if(*((int*)_tmp10F)!= 5)goto _LL80;_LL7F: goto _LL81;_LL80: if(*((
int*)_tmp10F)!= 12)goto _LL82;_LL81: goto _LL83;_LL82: if(*((int*)_tmp10F)!= 13)goto
_LL84;_LL83: goto _LL85;_LL84: if((int)_tmp10F != 1)goto _LL86;_LL85: goto _LL87;_LL86:
if(_tmp10F <= (void*)4)goto _LL9E;if(*((int*)_tmp10F)!= 6)goto _LL88;_LL87: goto
_LL89;_LL88: if(*((int*)_tmp10F)!= 8)goto _LL8A;_LL89: goto _LL8B;_LL8A: if(*((int*)
_tmp10F)!= 17)goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(*((int*)_tmp10F)!= 14)goto _LL8E;
_LL8D: return 1;_LL8E: if(*((int*)_tmp10F)!= 7)goto _LL90;_tmp110=((struct Cyc_Absyn_ArrayType_struct*)
_tmp10F)->f1;_tmp111=(void*)_tmp110.elt_type;_LL8F: return Cyc_Toc_atomic_typ(
_tmp111);_LL90: if(*((int*)_tmp10F)!= 10)goto _LL92;_tmp112=((struct Cyc_Absyn_AggrType_struct*)
_tmp10F)->f1;_tmp113=(void*)_tmp112.aggr_info;_LL91:{void*_tmp11A=_tmp113;_LLA1:
if(*((int*)_tmp11A)!= 0)goto _LLA3;_LLA2: return 0;_LLA3:;_LLA4: goto _LLA0;_LLA0:;}{
struct Cyc_Absyn_Aggrdecl*_tmp11B=Cyc_Absyn_get_known_aggrdecl(_tmp113);if(
_tmp11B->impl == 0)return 0;{struct Cyc_List_List*_tmp11C=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp11B->impl))->fields;for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp11C->hd)->type))
return 0;}}return 1;}_LL92: if(*((int*)_tmp10F)!= 11)goto _LL94;_tmp114=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp10F)->f2;_LL93: for(0;_tmp114 != 0;_tmp114=_tmp114->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp114->hd)->type))return 0;}return 1;_LL94:
if(*((int*)_tmp10F)!= 3)goto _LL96;_tmp115=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp10F)->f1;_tmp116=(void*)_tmp115.field_info;if(*((int*)_tmp116)!= 1)goto _LL96;
_tmp117=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp116)->f1;_tmp118=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp116)->f2;_LL95: _tmp119=_tmp118->typs;goto
_LL97;_LL96: if(*((int*)_tmp10F)!= 9)goto _LL98;_tmp119=((struct Cyc_Absyn_TupleType_struct*)
_tmp10F)->f1;_LL97: for(0;_tmp119 != 0;_tmp119=_tmp119->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp119->hd)).f2))return 0;}return 1;_LL98: if(*((int*)_tmp10F)!= 2)
goto _LL9A;_LL99: goto _LL9B;_LL9A: if(*((int*)_tmp10F)!= 4)goto _LL9C;_LL9B: goto
_LL9D;_LL9C: if(*((int*)_tmp10F)!= 15)goto _LL9E;_LL9D: return 0;_LL9E:;_LL9F:({
struct Cyc_String_pa_struct _tmp11F;_tmp11F.tag=0;_tmp11F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp11D[
1]={& _tmp11F};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp11E="atomic_typ:  bad type %s";_tag_dynforward(_tmp11E,sizeof(char),
_get_zero_arr_size(_tmp11E,25));}),_tag_dynforward(_tmp11D,sizeof(void*),1));}});
_LL79:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp120=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp121;void*_tmp122;_LLA6: if(_tmp120 <= (void*)4)goto
_LLA8;if(*((int*)_tmp120)!= 4)goto _LLA8;_tmp121=((struct Cyc_Absyn_PointerType_struct*)
_tmp120)->f1;_tmp122=(void*)_tmp121.elt_typ;_LLA7: {void*_tmp123=Cyc_Tcutil_compress(
_tmp122);_LLAB: if((int)_tmp123 != 0)goto _LLAD;_LLAC: return 1;_LLAD:;_LLAE: return 0;
_LLAA:;}_LLA8:;_LLA9: return 0;_LLA5:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f){void*_tmp124=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp125;void*_tmp126;struct Cyc_List_List*_tmp127;_LLB0: if(_tmp124 <= (void*)4)
goto _LLB4;if(*((int*)_tmp124)!= 10)goto _LLB2;_tmp125=((struct Cyc_Absyn_AggrType_struct*)
_tmp124)->f1;_tmp126=(void*)_tmp125.aggr_info;_LLB1: {struct Cyc_Absyn_Aggrdecl*
_tmp128=Cyc_Absyn_get_known_aggrdecl(_tmp126);if(_tmp128->impl == 0)({void*
_tmp129[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp12A="is_poly_field: type missing fields";_tag_dynforward(_tmp12A,
sizeof(char),_get_zero_arr_size(_tmp12A,35));}),_tag_dynforward(_tmp129,sizeof(
void*),0));});_tmp127=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp128->impl))->fields;
goto _LLB3;}_LLB2: if(*((int*)_tmp124)!= 11)goto _LLB4;_tmp127=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp124)->f2;_LLB3: {struct Cyc_Absyn_Aggrfield*_tmp12B=Cyc_Absyn_lookup_field(
_tmp127,f);if(_tmp12B == 0)({struct Cyc_String_pa_struct _tmp12E;_tmp12E.tag=0;
_tmp12E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f);{void*_tmp12C[1]={&
_tmp12E};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp12D="is_poly_field: bad field %s";_tag_dynforward(_tmp12D,sizeof(
char),_get_zero_arr_size(_tmp12D,28));}),_tag_dynforward(_tmp12C,sizeof(void*),1));}});
return Cyc_Toc_is_void_star((void*)_tmp12B->type);}_LLB4:;_LLB5:({struct Cyc_String_pa_struct
_tmp131;_tmp131.tag=0;_tmp131.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp12F[1]={& _tmp131};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp130="is_poly_field: bad type %s";_tag_dynforward(_tmp130,sizeof(char),
_get_zero_arr_size(_tmp130,27));}),_tag_dynforward(_tmp12F,sizeof(void*),1));}});
_LLAF:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmp132=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp133;struct _dynforward_ptr*_tmp134;struct Cyc_Absyn_Exp*
_tmp135;struct _dynforward_ptr*_tmp136;_LLB7: if(*((int*)_tmp132)!= 23)goto _LLB9;
_tmp133=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp132)->f1;_tmp134=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp132)->f2;_LLB8: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp133->topt))->v,_tmp134);_LLB9: if(*((int*)_tmp132)!= 24)goto _LLBB;
_tmp135=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp132)->f1;_tmp136=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp132)->f2;_LLBA: {void*_tmp137=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp135->topt))->v);struct Cyc_Absyn_PtrInfo _tmp138;void*_tmp139;
_LLBE: if(_tmp137 <= (void*)4)goto _LLC0;if(*((int*)_tmp137)!= 4)goto _LLC0;_tmp138=((
struct Cyc_Absyn_PointerType_struct*)_tmp137)->f1;_tmp139=(void*)_tmp138.elt_typ;
_LLBF: return Cyc_Toc_is_poly_field(_tmp139,_tmp136);_LLC0:;_LLC1:({struct Cyc_String_pa_struct
_tmp13C;_tmp13C.tag=0;_tmp13C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp135->topt))->v));{
void*_tmp13A[1]={& _tmp13C};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp13B="is_poly_project: bad type %s";
_tag_dynforward(_tmp13B,sizeof(char),_get_zero_arr_size(_tmp13B,29));}),
_tag_dynforward(_tmp13A,sizeof(void*),1));}});_LLBD:;}_LLBB:;_LLBC: return 0;_LLB6:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*_tmp13D));
_tmp13D->hd=s;_tmp13D->tl=0;_tmp13D;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({
struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=s;_tmp13E->tl=
0;_tmp13E;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*
s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(
s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp13F[2];_tmp13F[1]=s;_tmp13F[0]=rgn;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp13F,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp140[2];_tmp140[1]=n;
_tmp140[0]=s;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp140,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp141[2];_tmp141[1]=n;_tmp141[0]=s;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp141,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp142[3];_tmp142[2]=n;_tmp142[1]=
s;_tmp142[0]=rgn;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp142,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=e;_tmp143->tl=
0;_tmp143;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,
struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{
void*_tmp144=(void*)e->r;void*_tmp145;_LLC3: if(*((int*)_tmp144)!= 0)goto _LLC5;
_tmp145=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp144)->f1;if(_tmp145 <= (void*)
1)goto _LLC5;if(*((int*)_tmp145)!= 5)goto _LLC5;_LLC4: is_string=1;goto _LLC2;_LLC5:;
_LLC6: goto _LLC2;_LLC2:;}{struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;
if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();void*vd_typ=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref,
0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)
e);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp146=_cycalloc(sizeof(*_tmp146));
_tmp146->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp147=
_cycalloc(sizeof(*_tmp147));_tmp147[0]=({struct Cyc_Absyn_Var_d_struct _tmp148;
_tmp148.tag=0;_tmp148.f1=vd;_tmp148;});_tmp147;}),0);_tmp146->tl=Cyc_Toc_result_decls;
_tmp146;});xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}
else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*
urm_exp;if(!forward_only)urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple6*
_tmp149[3];_tmp149[2]=({struct _tuple6*_tmp14C=_cycalloc(sizeof(*_tmp14C));
_tmp14C->f1=0;_tmp14C->f2=xplussz;_tmp14C;});_tmp149[1]=({struct _tuple6*_tmp14B=
_cycalloc(sizeof(*_tmp14B));_tmp14B->f1=0;_tmp14B->f2=xexp;_tmp14B;});_tmp149[0]=({
struct _tuple6*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->f1=0;_tmp14A->f2=xexp;
_tmp14A;});((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp149,sizeof(struct _tuple6*),3));}),0);else{urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple6*_tmp14D[2];_tmp14D[1]=({struct _tuple6*_tmp14F=_cycalloc(
sizeof(*_tmp14F));_tmp14F->f1=0;_tmp14F->f2=xplussz;_tmp14F;});_tmp14D[0]=({
struct _tuple6*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->f1=0;_tmp14E->f2=xexp;
_tmp14E;});((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp14D,sizeof(struct _tuple6*),2));}),0);}return urm_exp;}}}
struct Cyc_Toc_Env{struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;
struct Cyc_Core_Opt*fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static
struct Cyc_Toc_Env*Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp150=
_cycalloc(sizeof(*_tmp150));_tmp150->break_lab=0;_tmp150->continue_lab=0;_tmp150->fallthru_info=
0;_tmp150->varmap=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp150->toplevel=1;_tmp150;});}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*
ans=({struct Cyc_Toc_Env*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151[0]=*e;
_tmp151;});ans->toplevel=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{void*_tmp152=(*x).f1;
_LLC8: if(_tmp152 <= (void*)1)goto _LLCA;if(*((int*)_tmp152)!= 0)goto _LLCA;_LLC9:({
struct Cyc_String_pa_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp153[1]={& _tmp155};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp154="Toc::add_varmap on Rel_n: %s\n";
_tag_dynforward(_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,30));}),
_tag_dynforward(_tmp153,sizeof(void*),1));}});_LLCA:;_LLCB: goto _LLC7;_LLC7:;}{
struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp156=_cycalloc(sizeof(*_tmp156));
_tmp156[0]=*e;_tmp156;});ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);
return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct
Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157[
0]=*e;_tmp157;});ans->break_lab=0;ans->continue_lab=0;return ans;}struct _tuple8{
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*f3;};static
struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*e,struct
_dynforward_ptr*break_l,struct _dynforward_ptr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp158=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp158=({
struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp159->tl=_tmp158;_tmp159;});}_tmp158=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp158);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D[0]=*e;_tmp15D;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A->v=
break_l;_tmp15A;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp15B=_cycalloc(
sizeof(*_tmp15B));_tmp15B->v=({struct _tuple8*_tmp15C=_cycalloc(sizeof(*_tmp15C));
_tmp15C->f1=fallthru_l;_tmp15C->f2=_tmp158;_tmp15C->f3=next_case_env->varmap;
_tmp15C;});_tmp15B;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F[0]=*e;_tmp15F;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->v=
break_l;_tmp15E;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct Cyc_Toc_Env*ans=({struct
Cyc_Toc_Env*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162[0]=*e;_tmp162;});ans->break_lab=
0;ans->fallthru_info=({struct Cyc_Core_Opt*_tmp160=_cycalloc(sizeof(*_tmp160));
_tmp160->v=({struct _tuple8*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->f1=
next_l;_tmp161->f2=0;_tmp161->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp161;});_tmp160;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp163=(void*)e->annot;
_LLCD: if(*((void**)_tmp163)!= Cyc_CfFlowInfo_UnknownZ)goto _LLCF;_LLCE: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLCF: if(*((void**)_tmp163)
!= Cyc_CfFlowInfo_NotZero)goto _LLD1;_LLD0: return 0;_LLD1: if(_tmp163 != Cyc_CfFlowInfo_IsZero)
goto _LLD3;_LLD2:({void*_tmp164[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp165="dereference of NULL pointer";
_tag_dynforward(_tmp165,sizeof(char),_get_zero_arr_size(_tmp165,28));}),
_tag_dynforward(_tmp164,sizeof(void*),0));});return 0;_LLD3: if(_tmp163 != Cyc_Absyn_EmptyAnnot)
goto _LLD5;_LLD4: return 0;_LLD5: if(*((void**)_tmp163)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LLD7;_LLD6:({void*_tmp166[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp167="compiler oddity: pointer compared to tag type";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size(_tmp167,46));}),
_tag_dynforward(_tmp166,sizeof(void*),0));});return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLD7:;_LLD8:({void*_tmp168[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp169="need_null_check";_tag_dynforward(_tmp169,sizeof(char),
_get_zero_arr_size(_tmp169,16));}),_tag_dynforward(_tmp168,sizeof(void*),0));});
_LLCC:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp16A=(void*)e->annot;struct Cyc_List_List*_tmp16B;struct Cyc_List_List*_tmp16C;
_LLDA: if(*((void**)_tmp16A)!= Cyc_CfFlowInfo_UnknownZ)goto _LLDC;_tmp16B=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp16A)->f1;_LLDB: return _tmp16B;_LLDC: if(*((
void**)_tmp16A)!= Cyc_CfFlowInfo_NotZero)goto _LLDE;_tmp16C=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp16A)->f1;_LLDD: return _tmp16C;_LLDE: if(_tmp16A != Cyc_CfFlowInfo_IsZero)goto
_LLE0;_LLDF:({void*_tmp16D[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp16E="dereference of NULL pointer";
_tag_dynforward(_tmp16E,sizeof(char),_get_zero_arr_size(_tmp16E,28));}),
_tag_dynforward(_tmp16D,sizeof(void*),0));});return 0;_LLE0: if(*((void**)_tmp16A)
!= Cyc_CfFlowInfo_HasTagCmps)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(_tmp16A != Cyc_Absyn_EmptyAnnot)
goto _LLE4;_LLE3: return 0;_LLE4:;_LLE5:({void*_tmp16F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp170="get_relns";_tag_dynforward(_tmp170,sizeof(char),_get_zero_arr_size(
_tmp170,10));}),_tag_dynforward(_tmp16F,sizeof(void*),0));});_LLD9:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp171=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp172;struct Cyc_Absyn_PtrAtts _tmp173;struct Cyc_Absyn_Conref*
_tmp174;struct Cyc_Absyn_Conref*_tmp175;struct Cyc_Absyn_ArrayInfo _tmp176;struct
Cyc_Absyn_Exp*_tmp177;_LLE7: if(_tmp171 <= (void*)4)goto _LLEB;if(*((int*)_tmp171)
!= 4)goto _LLE9;_tmp172=((struct Cyc_Absyn_PointerType_struct*)_tmp171)->f1;
_tmp173=_tmp172.ptr_atts;_tmp174=_tmp173.bounds;_tmp175=_tmp173.zero_term;_LLE8: {
void*_tmp178=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp174);struct Cyc_Absyn_Exp*
_tmp179;_LLEE: if((int)_tmp178 != 0)goto _LLF0;_LLEF: return 0;_LLF0: if((int)_tmp178
!= 1)goto _LLF2;_LLF1: return 0;_LLF2: if(_tmp178 <= (void*)2)goto _LLF4;if(*((int*)
_tmp178)!= 0)goto _LLF4;_tmp179=((struct Cyc_Absyn_Upper_b_struct*)_tmp178)->f1;
_LLF3: {unsigned int _tmp17B;int _tmp17C;struct _tuple5 _tmp17A=Cyc_Evexp_eval_const_uint_exp(
_tmp179);_tmp17B=_tmp17A.f1;_tmp17C=_tmp17A.f2;return _tmp17C  && i <= _tmp17B;}
_LLF4: if(_tmp178 <= (void*)2)goto _LLED;if(*((int*)_tmp178)!= 1)goto _LLED;_LLF5:({
void*_tmp17D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp17E="check_const_array: AbsUpper_b";
_tag_dynforward(_tmp17E,sizeof(char),_get_zero_arr_size(_tmp17E,30));}),
_tag_dynforward(_tmp17D,sizeof(void*),0));});_LLED:;}_LLE9: if(*((int*)_tmp171)!= 
7)goto _LLEB;_tmp176=((struct Cyc_Absyn_ArrayType_struct*)_tmp171)->f1;_tmp177=
_tmp176.num_elts;_LLEA: if(_tmp177 == 0)return 0;{unsigned int _tmp180;int _tmp181;
struct _tuple5 _tmp17F=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp177);
_tmp180=_tmp17F.f1;_tmp181=_tmp17F.f2;return _tmp181  && i <= _tmp180;}_LLEB:;_LLEC:
return 0;_LLE6:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp182=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(
_tmp182->vd != y)continue;{void*_tmp183=(void*)_tmp182->rop;struct Cyc_Absyn_Vardecl*
_tmp184;struct Cyc_Absyn_Vardecl*_tmp185;_LLF7: if(*((int*)_tmp183)!= 2)goto _LLF9;
_tmp184=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp183)->f1;_LLF8: _tmp185=
_tmp184;goto _LLFA;_LLF9: if(*((int*)_tmp183)!= 4)goto _LLFB;_tmp185=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp183)->f1;_LLFA: if(_tmp185 == v)return 1;else{goto _LLF6;}_LLFB:;_LLFC: continue;
_LLF6:;}}return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp186=(void*)e->r;void*
_tmp187;struct Cyc_Absyn_Vardecl*_tmp188;void*_tmp189;struct Cyc_Absyn_Vardecl*
_tmp18A;void*_tmp18B;struct Cyc_Absyn_Vardecl*_tmp18C;void*_tmp18D;struct Cyc_Absyn_Vardecl*
_tmp18E;void*_tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_List_List _tmp191;
struct Cyc_Absyn_Exp*_tmp192;_LLFE: if(*((int*)_tmp186)!= 1)goto _LL100;_tmp187=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp186)->f2;if(_tmp187 <= (void*)1)goto
_LL100;if(*((int*)_tmp187)!= 4)goto _LL100;_tmp188=((struct Cyc_Absyn_Pat_b_struct*)
_tmp187)->f1;_LLFF: _tmp18A=_tmp188;goto _LL101;_LL100: if(*((int*)_tmp186)!= 1)
goto _LL102;_tmp189=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp186)->f2;if(
_tmp189 <= (void*)1)goto _LL102;if(*((int*)_tmp189)!= 3)goto _LL102;_tmp18A=((
struct Cyc_Absyn_Local_b_struct*)_tmp189)->f1;_LL101: _tmp18C=_tmp18A;goto _LL103;
_LL102: if(*((int*)_tmp186)!= 1)goto _LL104;_tmp18B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp186)->f2;if(_tmp18B <= (void*)1)goto _LL104;if(*((int*)_tmp18B)!= 0)goto _LL104;
_tmp18C=((struct Cyc_Absyn_Global_b_struct*)_tmp18B)->f1;_LL103: _tmp18E=_tmp18C;
goto _LL105;_LL104: if(*((int*)_tmp186)!= 1)goto _LL106;_tmp18D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp186)->f2;if(_tmp18D <= (void*)1)goto _LL106;if(*((int*)_tmp18D)!= 2)goto _LL106;
_tmp18E=((struct Cyc_Absyn_Param_b_struct*)_tmp18D)->f1;_LL105: if(_tmp18E->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp18E))return 1;goto _LLFD;_LL106:
if(*((int*)_tmp186)!= 3)goto _LL108;_tmp18F=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp186)->f1;if((int)_tmp18F != 19)goto _LL108;_tmp190=((struct Cyc_Absyn_Primop_e_struct*)
_tmp186)->f2;if(_tmp190 == 0)goto _LL108;_tmp191=*_tmp190;_tmp192=(struct Cyc_Absyn_Exp*)
_tmp191.hd;_LL107:{void*_tmp193=(void*)_tmp192->r;void*_tmp194;struct Cyc_Absyn_Vardecl*
_tmp195;void*_tmp196;struct Cyc_Absyn_Vardecl*_tmp197;void*_tmp198;struct Cyc_Absyn_Vardecl*
_tmp199;void*_tmp19A;struct Cyc_Absyn_Vardecl*_tmp19B;_LL10B: if(*((int*)_tmp193)
!= 1)goto _LL10D;_tmp194=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp193)->f2;if(
_tmp194 <= (void*)1)goto _LL10D;if(*((int*)_tmp194)!= 4)goto _LL10D;_tmp195=((
struct Cyc_Absyn_Pat_b_struct*)_tmp194)->f1;_LL10C: _tmp197=_tmp195;goto _LL10E;
_LL10D: if(*((int*)_tmp193)!= 1)goto _LL10F;_tmp196=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp193)->f2;if(_tmp196 <= (void*)1)goto _LL10F;if(*((int*)_tmp196)!= 3)goto _LL10F;
_tmp197=((struct Cyc_Absyn_Local_b_struct*)_tmp196)->f1;_LL10E: _tmp199=_tmp197;
goto _LL110;_LL10F: if(*((int*)_tmp193)!= 1)goto _LL111;_tmp198=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp193)->f2;if(_tmp198 <= (void*)1)goto _LL111;if(*((int*)_tmp198)!= 0)goto _LL111;
_tmp199=((struct Cyc_Absyn_Global_b_struct*)_tmp198)->f1;_LL110: _tmp19B=_tmp199;
goto _LL112;_LL111: if(*((int*)_tmp193)!= 1)goto _LL113;_tmp19A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp193)->f2;if(_tmp19A <= (void*)1)goto _LL113;if(*((int*)_tmp19A)!= 2)goto _LL113;
_tmp19B=((struct Cyc_Absyn_Param_b_struct*)_tmp19A)->f1;_LL112: return _tmp19B == v;
_LL113:;_LL114: goto _LL10A;_LL10A:;}goto _LLFD;_LL108:;_LL109: goto _LLFD;_LLFD:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp19C=(void*)a->r;void*_tmp19D;struct Cyc_Absyn_Vardecl*
_tmp19E;void*_tmp19F;struct Cyc_Absyn_Vardecl*_tmp1A0;void*_tmp1A1;struct Cyc_Absyn_Vardecl*
_tmp1A2;void*_tmp1A3;struct Cyc_Absyn_Vardecl*_tmp1A4;_LL116: if(*((int*)_tmp19C)
!= 1)goto _LL118;_tmp19D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp19C)->f2;if(
_tmp19D <= (void*)1)goto _LL118;if(*((int*)_tmp19D)!= 4)goto _LL118;_tmp19E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp19D)->f1;_LL117: _tmp1A0=_tmp19E;goto _LL119;
_LL118: if(*((int*)_tmp19C)!= 1)goto _LL11A;_tmp19F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp19C)->f2;if(_tmp19F <= (void*)1)goto _LL11A;if(*((int*)_tmp19F)!= 3)goto _LL11A;
_tmp1A0=((struct Cyc_Absyn_Local_b_struct*)_tmp19F)->f1;_LL119: _tmp1A2=_tmp1A0;
goto _LL11B;_LL11A: if(*((int*)_tmp19C)!= 1)goto _LL11C;_tmp1A1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp19C)->f2;if(_tmp1A1 <= (void*)1)goto _LL11C;if(*((int*)_tmp1A1)!= 0)goto _LL11C;
_tmp1A2=((struct Cyc_Absyn_Global_b_struct*)_tmp1A1)->f1;_LL11B: _tmp1A4=_tmp1A2;
goto _LL11D;_LL11C: if(*((int*)_tmp19C)!= 1)goto _LL11E;_tmp1A3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp19C)->f2;if(_tmp1A3 <= (void*)1)goto _LL11E;if(*((int*)_tmp1A3)!= 2)goto _LL11E;
_tmp1A4=((struct Cyc_Absyn_Param_b_struct*)_tmp1A3)->f1;_LL11D: if(_tmp1A4->escapes)
return 0;inner_loop: {void*_tmp1A5=(void*)i->r;void*_tmp1A6;struct Cyc_Absyn_Exp*
_tmp1A7;void*_tmp1A8;void*_tmp1A9;int _tmp1AA;void*_tmp1AB;void*_tmp1AC;int
_tmp1AD;void*_tmp1AE;void*_tmp1AF;int _tmp1B0;void*_tmp1B1;struct Cyc_List_List*
_tmp1B2;struct Cyc_List_List _tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_List_List*
_tmp1B5;struct Cyc_List_List _tmp1B6;struct Cyc_Absyn_Exp*_tmp1B7;void*_tmp1B8;
struct Cyc_Absyn_Vardecl*_tmp1B9;void*_tmp1BA;struct Cyc_Absyn_Vardecl*_tmp1BB;
void*_tmp1BC;struct Cyc_Absyn_Vardecl*_tmp1BD;void*_tmp1BE;struct Cyc_Absyn_Vardecl*
_tmp1BF;_LL121: if(*((int*)_tmp1A5)!= 15)goto _LL123;_tmp1A6=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1A5)->f1;_tmp1A7=((struct Cyc_Absyn_Cast_e_struct*)_tmp1A5)->f2;_LL122: i=
_tmp1A7;goto inner_loop;_LL123: if(*((int*)_tmp1A5)!= 0)goto _LL125;_tmp1A8=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp1A5)->f1;if(_tmp1A8 <= (void*)1)goto _LL125;
if(*((int*)_tmp1A8)!= 2)goto _LL125;_tmp1A9=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp1A8)->f1;if((int)_tmp1A9 != 2)goto _LL125;_tmp1AA=((struct Cyc_Absyn_Int_c_struct*)
_tmp1A8)->f2;_LL124: _tmp1AD=_tmp1AA;goto _LL126;_LL125: if(*((int*)_tmp1A5)!= 0)
goto _LL127;_tmp1AB=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1A5)->f1;if(
_tmp1AB <= (void*)1)goto _LL127;if(*((int*)_tmp1AB)!= 2)goto _LL127;_tmp1AC=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp1AB)->f1;if((int)_tmp1AC != 0)goto _LL127;
_tmp1AD=((struct Cyc_Absyn_Int_c_struct*)_tmp1AB)->f2;_LL126: return _tmp1AD >= 0
 && Cyc_Toc_check_const_array((unsigned int)(_tmp1AD + 1),(void*)_tmp1A4->type);
_LL127: if(*((int*)_tmp1A5)!= 0)goto _LL129;_tmp1AE=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1A5)->f1;if(_tmp1AE <= (void*)1)goto _LL129;if(*((int*)_tmp1AE)!= 2)goto _LL129;
_tmp1AF=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp1AE)->f1;if((int)_tmp1AF != 1)
goto _LL129;_tmp1B0=((struct Cyc_Absyn_Int_c_struct*)_tmp1AE)->f2;_LL128: return Cyc_Toc_check_const_array((
unsigned int)(_tmp1B0 + 1),(void*)_tmp1A4->type);_LL129: if(*((int*)_tmp1A5)!= 3)
goto _LL12B;_tmp1B1=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1A5)->f1;if((
int)_tmp1B1 != 4)goto _LL12B;_tmp1B2=((struct Cyc_Absyn_Primop_e_struct*)_tmp1A5)->f2;
if(_tmp1B2 == 0)goto _LL12B;_tmp1B3=*_tmp1B2;_tmp1B4=(struct Cyc_Absyn_Exp*)_tmp1B3.hd;
_tmp1B5=_tmp1B3.tl;if(_tmp1B5 == 0)goto _LL12B;_tmp1B6=*_tmp1B5;_tmp1B7=(struct Cyc_Absyn_Exp*)
_tmp1B6.hd;_LL12A: return Cyc_Toc_check_leq_size(relns,_tmp1A4,_tmp1B7);_LL12B: if(*((
int*)_tmp1A5)!= 1)goto _LL12D;_tmp1B8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A5)->f2;if(_tmp1B8 <= (void*)1)goto _LL12D;if(*((int*)_tmp1B8)!= 4)goto _LL12D;
_tmp1B9=((struct Cyc_Absyn_Pat_b_struct*)_tmp1B8)->f1;_LL12C: _tmp1BB=_tmp1B9;goto
_LL12E;_LL12D: if(*((int*)_tmp1A5)!= 1)goto _LL12F;_tmp1BA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A5)->f2;if(_tmp1BA <= (void*)1)goto _LL12F;if(*((int*)_tmp1BA)!= 3)goto _LL12F;
_tmp1BB=((struct Cyc_Absyn_Local_b_struct*)_tmp1BA)->f1;_LL12E: _tmp1BD=_tmp1BB;
goto _LL130;_LL12F: if(*((int*)_tmp1A5)!= 1)goto _LL131;_tmp1BC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A5)->f2;if(_tmp1BC <= (void*)1)goto _LL131;if(*((int*)_tmp1BC)!= 0)goto _LL131;
_tmp1BD=((struct Cyc_Absyn_Global_b_struct*)_tmp1BC)->f1;_LL130: _tmp1BF=_tmp1BD;
goto _LL132;_LL131: if(*((int*)_tmp1A5)!= 1)goto _LL133;_tmp1BE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A5)->f2;if(_tmp1BE <= (void*)1)goto _LL133;if(*((int*)_tmp1BE)!= 2)goto _LL133;
_tmp1BF=((struct Cyc_Absyn_Param_b_struct*)_tmp1BE)->f1;_LL132: if(_tmp1BF->escapes)
return 0;{struct Cyc_List_List*_tmp1C0=relns;for(0;_tmp1C0 != 0;_tmp1C0=_tmp1C0->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1C1=(struct Cyc_CfFlowInfo_Reln*)_tmp1C0->hd;if(
_tmp1C1->vd == _tmp1BF){void*_tmp1C2=(void*)_tmp1C1->rop;struct Cyc_Absyn_Vardecl*
_tmp1C3;struct Cyc_Absyn_Vardecl*_tmp1C4;unsigned int _tmp1C5;_LL136: if(*((int*)
_tmp1C2)!= 2)goto _LL138;_tmp1C3=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1C2)->f1;
_LL137: if(_tmp1A4 == _tmp1C3)return 1;else{goto _LL135;}_LL138: if(*((int*)_tmp1C2)
!= 1)goto _LL13A;_tmp1C4=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1C2)->f1;
_LL139:{struct Cyc_List_List*_tmp1C6=relns;for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1C7=(struct Cyc_CfFlowInfo_Reln*)_tmp1C6->hd;if(
_tmp1C7->vd == _tmp1C4){void*_tmp1C8=(void*)_tmp1C7->rop;struct Cyc_Absyn_Vardecl*
_tmp1C9;struct Cyc_Absyn_Vardecl*_tmp1CA;unsigned int _tmp1CB;struct Cyc_Absyn_Vardecl*
_tmp1CC;_LL13F: if(*((int*)_tmp1C8)!= 4)goto _LL141;_tmp1C9=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp1C8)->f1;_LL140: _tmp1CA=_tmp1C9;goto _LL142;_LL141: if(*((int*)_tmp1C8)!= 2)
goto _LL143;_tmp1CA=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1C8)->f1;_LL142:
if(_tmp1A4 == _tmp1CA)return 1;goto _LL13E;_LL143: if(*((int*)_tmp1C8)!= 0)goto
_LL145;_tmp1CB=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1C8)->f1;_LL144:
return Cyc_Toc_check_const_array(_tmp1CB,(void*)_tmp1A4->type);_LL145: if(*((int*)
_tmp1C8)!= 1)goto _LL147;_tmp1CC=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1C8)->f1;
_LL146: if(Cyc_Toc_check_leq_size_var(relns,_tmp1A4,_tmp1CC))return 1;goto _LL13E;
_LL147:;_LL148: goto _LL13E;_LL13E:;}}}goto _LL135;_LL13A: if(*((int*)_tmp1C2)!= 3)
goto _LL13C;_tmp1C5=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp1C2)->f1;_LL13B:
return Cyc_Toc_check_const_array(_tmp1C5,(void*)_tmp1A4->type);_LL13C:;_LL13D:
goto _LL135;_LL135:;}}}goto _LL120;_LL133:;_LL134: goto _LL120;_LL120:;}goto _LL115;
_LL11E:;_LL11F: goto _LL115;_LL115:;}return 0;}static void*Cyc_Toc_get_c_typ(struct
Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp1CD[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1CE="Missing type in primop ";_tag_dynforward(_tmp1CE,sizeof(char),
_get_zero_arr_size(_tmp1CE,24));}),_tag_dynforward(_tmp1CD,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp1CF[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1D0="Missing type in primop ";_tag_dynforward(_tmp1D0,sizeof(char),
_get_zero_arr_size(_tmp1D0,24));}),_tag_dynforward(_tmp1CF,sizeof(void*),0));});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*
Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp1D1=_cycalloc(
sizeof(*_tmp1D1));_tmp1D1->f1=Cyc_Toc_mt_tq;_tmp1D1->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp1D1;});}static struct _tuple6*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple6*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0;
_tmp1D2->f2=e;_tmp1D2;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp1D3=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp1D3):
Cyc_Toc_malloc_ptr(_tmp1D3));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp1D3);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp1D4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp1D4 != 0;_tmp1D4=_tmp1D4->tl){struct _tuple6 _tmp1D6;struct Cyc_List_List*
_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;struct _tuple6*_tmp1D5=(struct _tuple6*)
_tmp1D4->hd;_tmp1D6=*_tmp1D5;_tmp1D7=_tmp1D6.f1;_tmp1D8=_tmp1D6.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp1D7 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp1D7->tl != 0)({void*_tmp1D9[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp1DA="multiple designators in array";
_tag_dynforward(_tmp1DA,sizeof(char),_get_zero_arr_size(_tmp1DA,30));}),
_tag_dynforward(_tmp1D9,sizeof(void*),0));});{void*_tmp1DB=(void*)_tmp1D7->hd;
void*_tmp1DC=_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DD;_LL14A: if(*((int*)_tmp1DC)!= 0)
goto _LL14C;_tmp1DD=((struct Cyc_Absyn_ArrayElement_struct*)_tmp1DC)->f1;_LL14B:
Cyc_Toc_exp_to_c(nv,_tmp1DD);e_index=_tmp1DD;goto _LL149;_LL14C: if(*((int*)
_tmp1DC)!= 1)goto _LL149;_LL14D:({void*_tmp1DE[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp1DF="field name designators in array";
_tag_dynforward(_tmp1DF,sizeof(char),_get_zero_arr_size(_tmp1DF,32));}),
_tag_dynforward(_tmp1DE,sizeof(void*),0));});_LL149:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp1E0=(void*)_tmp1D8->r;struct Cyc_List_List*
_tmp1E1;struct Cyc_Absyn_Vardecl*_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*
_tmp1E4;int _tmp1E5;void*_tmp1E6;struct Cyc_List_List*_tmp1E7;_LL14F: if(*((int*)
_tmp1E0)!= 28)goto _LL151;_tmp1E1=((struct Cyc_Absyn_Array_e_struct*)_tmp1E0)->f1;
_LL150: s=Cyc_Toc_init_array(nv,lval,_tmp1E1,s);goto _LL14E;_LL151: if(*((int*)
_tmp1E0)!= 29)goto _LL153;_tmp1E2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E0)->f1;_tmp1E3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1E0)->f2;
_tmp1E4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1E0)->f3;_tmp1E5=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1E0)->f4;_LL152: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1E2,_tmp1E3,_tmp1E4,_tmp1E5,s,0);goto _LL14E;_LL153: if(*((int*)
_tmp1E0)!= 31)goto _LL155;_tmp1E6=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1E0)->f1;_tmp1E7=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1E0)->f2;_LL154:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1E6,_tmp1E7,s);goto _LL14E;_LL155:;_LL156:
Cyc_Toc_exp_to_c(nv,_tmp1D8);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp1D8,0),s,0);goto _LL14E;_LL14E:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp1E8=vd->name;
void*_tmp1E9=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp1E8,Cyc_Absyn_varb_exp(_tmp1E8,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_Absyn_Local_b_struct
_tmp1F3;_tmp1F3.tag=3;_tmp1F3.f1=vd;_tmp1F3;});_tmp1F2;}),0));struct _tuple1*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp1E8,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp1E8,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp1E8,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp1E8,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp1EA=(void*)e2->r;struct Cyc_List_List*_tmp1EB;struct Cyc_Absyn_Vardecl*
_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;int _tmp1EF;void*
_tmp1F0;struct Cyc_List_List*_tmp1F1;_LL158: if(*((int*)_tmp1EA)!= 28)goto _LL15A;
_tmp1EB=((struct Cyc_Absyn_Array_e_struct*)_tmp1EA)->f1;_LL159: body=Cyc_Toc_init_array(
nv2,lval,_tmp1EB,Cyc_Toc_skip_stmt_dl());goto _LL157;_LL15A: if(*((int*)_tmp1EA)!= 
29)goto _LL15C;_tmp1EC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EA)->f1;
_tmp1ED=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EA)->f2;_tmp1EE=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1EA)->f3;_tmp1EF=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1EA)->f4;_LL15B: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp1EC,_tmp1ED,
_tmp1EE,_tmp1EF,Cyc_Toc_skip_stmt_dl(),0);goto _LL157;_LL15C: if(*((int*)_tmp1EA)
!= 31)goto _LL15E;_tmp1F0=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1EA)->f1;
_tmp1F1=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1EA)->f2;_LL15D: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1F0,_tmp1F1,Cyc_Toc_skip_stmt_dl());goto _LL157;_LL15E:;_LL15F: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL157;_LL157:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp1E9,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp1E8,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp1F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp1F4 != 0;_tmp1F4=_tmp1F4->tl){struct _tuple6 _tmp1F6;
struct Cyc_List_List*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;struct _tuple6*_tmp1F5=(
struct _tuple6*)_tmp1F4->hd;_tmp1F6=*_tmp1F5;_tmp1F7=_tmp1F6.f1;_tmp1F8=_tmp1F6.f2;
if(_tmp1F7 == 0)({void*_tmp1F9[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1FA="empty designator list";
_tag_dynforward(_tmp1FA,sizeof(char),_get_zero_arr_size(_tmp1FA,22));}),
_tag_dynforward(_tmp1F9,sizeof(void*),0));});if(_tmp1F7->tl != 0)({void*_tmp1FB[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1FC="too many designators in anonymous struct";_tag_dynforward(
_tmp1FC,sizeof(char),_get_zero_arr_size(_tmp1FC,41));}),_tag_dynforward(_tmp1FB,
sizeof(void*),0));});{void*_tmp1FD=(void*)_tmp1F7->hd;struct _dynforward_ptr*
_tmp1FE;_LL161: if(*((int*)_tmp1FD)!= 1)goto _LL163;_tmp1FE=((struct Cyc_Absyn_FieldName_struct*)
_tmp1FD)->f1;_LL162: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,
_tmp1FE,0);{void*_tmp1FF=(void*)_tmp1F8->r;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_Vardecl*
_tmp201;struct Cyc_Absyn_Exp*_tmp202;struct Cyc_Absyn_Exp*_tmp203;int _tmp204;void*
_tmp205;struct Cyc_List_List*_tmp206;_LL166: if(*((int*)_tmp1FF)!= 28)goto _LL168;
_tmp200=((struct Cyc_Absyn_Array_e_struct*)_tmp1FF)->f1;_LL167: s=Cyc_Toc_init_array(
nv,lval,_tmp200,s);goto _LL165;_LL168: if(*((int*)_tmp1FF)!= 29)goto _LL16A;_tmp201=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1FF)->f1;_tmp202=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1FF)->f2;_tmp203=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1FF)->f3;
_tmp204=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1FF)->f4;_LL169: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp201,_tmp202,_tmp203,_tmp204,s,0);goto _LL165;_LL16A: if(*((int*)
_tmp1FF)!= 31)goto _LL16C;_tmp205=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1FF)->f1;_tmp206=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1FF)->f2;_LL16B:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp205,_tmp206,s);goto _LL165;_LL16C:;_LL16D:
Cyc_Toc_exp_to_c(nv,_tmp1F8);if(Cyc_Toc_is_poly_field(struct_type,_tmp1FE))
_tmp1F8=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp1F8);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1F8,0),0),s,0);goto _LL165;
_LL165:;}goto _LL160;}_LL163:;_LL164:({void*_tmp207[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp208="array designator in struct";_tag_dynforward(_tmp208,sizeof(char),
_get_zero_arr_size(_tmp208,27));}),_tag_dynforward(_tmp207,sizeof(void*),0));});
_LL160:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct
_RegionHandle _tmp209=_new_region("r");struct _RegionHandle*r=& _tmp209;
_push_region(r);{struct Cyc_List_List*_tmp20A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _dynforward_ptr*_tmp20B=Cyc_Toc_add_tuple_type(
_tmp20A);struct _tuple1*_tmp20C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp20D=
Cyc_Absyn_var_exp(_tmp20C,0);struct Cyc_Absyn_Stmt*_tmp20E=Cyc_Absyn_exp_stmt(
_tmp20D,0);struct Cyc_Absyn_Exp*(*_tmp20F)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp210=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp210);for(0;_tmp210 != 0;(_tmp210=
_tmp210->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp210->hd;struct
Cyc_Absyn_Exp*lval=_tmp20F(_tmp20D,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp211=(void*)e->r;struct Cyc_List_List*_tmp212;struct Cyc_Absyn_Vardecl*
_tmp213;struct Cyc_Absyn_Exp*_tmp214;struct Cyc_Absyn_Exp*_tmp215;int _tmp216;
_LL16F: if(*((int*)_tmp211)!= 28)goto _LL171;_tmp212=((struct Cyc_Absyn_Array_e_struct*)
_tmp211)->f1;_LL170: _tmp20E=Cyc_Toc_init_array(nv,lval,_tmp212,_tmp20E);goto
_LL16E;_LL171: if(*((int*)_tmp211)!= 29)goto _LL173;_tmp213=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp211)->f1;_tmp214=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp211)->f2;
_tmp215=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp211)->f3;_tmp216=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp211)->f4;_LL172: _tmp20E=Cyc_Toc_init_comprehension(
nv,lval,_tmp213,_tmp214,_tmp215,_tmp216,_tmp20E,0);goto _LL16E;_LL173:;_LL174: Cyc_Toc_exp_to_c(
nv,e);_tmp20E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp20F(
_tmp20D,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp20E,0);goto _LL16E;_LL16E:;}}}{
struct Cyc_Absyn_Exp*_tmp217=Cyc_Toc_make_struct(nv,_tmp20C,Cyc_Absyn_strct(
_tmp20B),_tmp20E,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp217;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp218=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp219=Cyc_Absyn_var_exp(_tmp218,0);struct Cyc_Absyn_Stmt*
_tmp21A=Cyc_Absyn_exp_stmt(_tmp219,0);struct Cyc_Absyn_Exp*(*_tmp21B)(struct Cyc_Absyn_Exp*,
struct _dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp21C=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp21D;void*
_tmp21E;_LL176: if(_tmp21C <= (void*)4)goto _LL178;if(*((int*)_tmp21C)!= 10)goto
_LL178;_tmp21D=((struct Cyc_Absyn_AggrType_struct*)_tmp21C)->f1;_tmp21E=(void*)
_tmp21D.aggr_info;_LL177: ad=Cyc_Absyn_get_known_aggrdecl(_tmp21E);goto _LL175;
_LL178:;_LL179:({void*_tmp21F[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp220="init_struct: bad struct type";
_tag_dynforward(_tmp220,sizeof(char),_get_zero_arr_size(_tmp220,29));}),
_tag_dynforward(_tmp21F,sizeof(void*),0));});_LL175:;}{struct _RegionHandle
_tmp221=_new_region("r");struct _RegionHandle*r=& _tmp221;_push_region(r);{struct
Cyc_List_List*_tmp222=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,dles);for(0;_tmp222 != 0;_tmp222=_tmp222->tl){struct _tuple6
_tmp224;struct Cyc_List_List*_tmp225;struct Cyc_Absyn_Exp*_tmp226;struct _tuple6*
_tmp223=(struct _tuple6*)_tmp222->hd;_tmp224=*_tmp223;_tmp225=_tmp224.f1;_tmp226=
_tmp224.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp226->topt))->v);if(_tmp225 == 0)({void*_tmp227[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp228="empty designator list";_tag_dynforward(_tmp228,sizeof(char),
_get_zero_arr_size(_tmp228,22));}),_tag_dynforward(_tmp227,sizeof(void*),0));});
if(_tmp225->tl != 0){struct _tuple1*_tmp229=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp22A=Cyc_Absyn_var_exp(_tmp229,0);for(0;_tmp225 != 0;_tmp225=_tmp225->tl){void*
_tmp22B=(void*)_tmp225->hd;struct _dynforward_ptr*_tmp22C;_LL17B: if(*((int*)
_tmp22B)!= 1)goto _LL17D;_tmp22C=((struct Cyc_Absyn_FieldName_struct*)_tmp22B)->f1;
_LL17C: if(Cyc_Toc_is_poly_field(struct_type,_tmp22C))_tmp22A=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp22A);_tmp21A=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp21B(_tmp219,_tmp22C,0),_tmp22A,0),0),_tmp21A,0);goto _LL17A;_LL17D:;_LL17E:({
void*_tmp22D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp22E="array designator in struct";
_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size(_tmp22E,27));}),
_tag_dynforward(_tmp22D,sizeof(void*),0));});_LL17A:;}Cyc_Toc_exp_to_c(nv,
_tmp226);_tmp21A=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp22A,_tmp226,0),0),_tmp21A,0);}else{void*_tmp22F=(void*)_tmp225->hd;struct
_dynforward_ptr*_tmp230;_LL180: if(*((int*)_tmp22F)!= 1)goto _LL182;_tmp230=((
struct Cyc_Absyn_FieldName_struct*)_tmp22F)->f1;_LL181: {struct Cyc_Absyn_Exp*lval=
_tmp21B(_tmp219,_tmp230,0);{void*_tmp231=(void*)_tmp226->r;struct Cyc_List_List*
_tmp232;struct Cyc_Absyn_Vardecl*_tmp233;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_Absyn_Exp*
_tmp235;int _tmp236;void*_tmp237;struct Cyc_List_List*_tmp238;_LL185: if(*((int*)
_tmp231)!= 28)goto _LL187;_tmp232=((struct Cyc_Absyn_Array_e_struct*)_tmp231)->f1;
_LL186: _tmp21A=Cyc_Toc_init_array(nv,lval,_tmp232,_tmp21A);goto _LL184;_LL187: if(*((
int*)_tmp231)!= 29)goto _LL189;_tmp233=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp231)->f1;_tmp234=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp231)->f2;
_tmp235=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp231)->f3;_tmp236=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp231)->f4;_LL188: _tmp21A=Cyc_Toc_init_comprehension(
nv,lval,_tmp233,_tmp234,_tmp235,_tmp236,_tmp21A,0);goto _LL184;_LL189: if(*((int*)
_tmp231)!= 31)goto _LL18B;_tmp237=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp231)->f1;_tmp238=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp231)->f2;_LL18A:
_tmp21A=Cyc_Toc_init_anon_struct(nv,lval,_tmp237,_tmp238,_tmp21A);goto _LL184;
_LL18B:;_LL18C: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp226->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp226);{struct Cyc_Absyn_Aggrfield*
_tmp239=Cyc_Absyn_lookup_decl_field(ad,_tmp230);if(Cyc_Toc_is_poly_field(
struct_type,_tmp230) && !was_ptr_type)_tmp226=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp226);if(has_exists)_tmp226=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp239))->type,_tmp226);_tmp21A=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp226,0),0),_tmp21A,0);goto _LL184;}}_LL184:;}goto
_LL17F;}_LL182:;_LL183:({void*_tmp23A[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp23B="array designator in struct";
_tag_dynforward(_tmp23B,sizeof(char),_get_zero_arr_size(_tmp23B,27));}),
_tag_dynforward(_tmp23A,sizeof(void*),0));});_LL17F:;}}}{struct Cyc_Absyn_Exp*
_tmp23C=Cyc_Toc_make_struct(nv,_tmp218,Cyc_Absyn_strctq(tdn),_tmp21A,pointer,
rgnopt,is_atomic);_npop_handler(0);return _tmp23C;};_pop_region(r);}}struct
_tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple9*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp23D=_cycalloc(sizeof(*_tmp23D));
_tmp23D[0]=({struct Cyc_Absyn_Increment_e_struct _tmp23E;_tmp23E.tag=5;_tmp23E.f1=
e1;_tmp23E.f2=(void*)incr;_tmp23E;});_tmp23D;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp23F=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp240;void*_tmp241;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_Absyn_Exp*_tmp243;struct _dynforward_ptr*_tmp244;_LL18E: if(*((
int*)_tmp23F)!= 37)goto _LL190;_tmp240=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp23F)->f1;_LL18F: Cyc_Toc_lvalue_assign_stmt(_tmp240,fs,f,f_env);goto _LL18D;
_LL190: if(*((int*)_tmp23F)!= 15)goto _LL192;_tmp241=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp23F)->f1;_tmp242=((struct Cyc_Absyn_Cast_e_struct*)_tmp23F)->f2;_LL191: Cyc_Toc_lvalue_assign(
_tmp242,fs,f,f_env);goto _LL18D;_LL192: if(*((int*)_tmp23F)!= 23)goto _LL194;
_tmp243=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp23F)->f1;_tmp244=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp23F)->f2;_LL193:(void*)(e1->r=(void*)((void*)_tmp243->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp245=_cycalloc(sizeof(*_tmp245));
_tmp245->hd=_tmp244;_tmp245->tl=fs;_tmp245;}),f,f_env);goto _LL18D;_LL194:;_LL195: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dynforward_ptr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL18D;}_LL18D:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp246=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Decl*_tmp248;struct Cyc_Absyn_Stmt*
_tmp249;struct Cyc_Absyn_Stmt*_tmp24A;_LL197: if(_tmp246 <= (void*)1)goto _LL19D;if(*((
int*)_tmp246)!= 0)goto _LL199;_tmp247=((struct Cyc_Absyn_Exp_s_struct*)_tmp246)->f1;
_LL198: Cyc_Toc_lvalue_assign(_tmp247,fs,f,f_env);goto _LL196;_LL199: if(*((int*)
_tmp246)!= 11)goto _LL19B;_tmp248=((struct Cyc_Absyn_Decl_s_struct*)_tmp246)->f1;
_tmp249=((struct Cyc_Absyn_Decl_s_struct*)_tmp246)->f2;_LL19A: Cyc_Toc_lvalue_assign_stmt(
_tmp249,fs,f,f_env);goto _LL196;_LL19B: if(*((int*)_tmp246)!= 1)goto _LL19D;_tmp24A=((
struct Cyc_Absyn_Seq_s_struct*)_tmp246)->f2;_LL19C: Cyc_Toc_lvalue_assign_stmt(
_tmp24A,fs,f,f_env);goto _LL196;_LL19D:;_LL19E:({struct Cyc_String_pa_struct
_tmp24D;_tmp24D.tag=0;_tmp24D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_stmt2string(s));{void*_tmp24B[1]={& _tmp24D};Cyc_Toc_toc_impos(({
const char*_tmp24C="lvalue_assign_stmt: %s";_tag_dynforward(_tmp24C,sizeof(char),
_get_zero_arr_size(_tmp24C,23));}),_tag_dynforward(_tmp24B,sizeof(void*),1));}});
_LL196:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;
struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmp24E=
_region_malloc(r2,sizeof(*_tmp24E));_tmp24E->hd=(void*)f((void*)x->hd,env);
_tmp24E->tl=0;_tmp24E;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp24F=_region_malloc(r2,sizeof(*
_tmp24F));_tmp24F->hd=(void*)f((void*)x->hd,env);_tmp24F->tl=0;_tmp24F;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple6*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->f1=0;
_tmp250->f2=e;_tmp250;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp251=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp252;_LL1A0: if(
_tmp251 <= (void*)4)goto _LL1A2;if(*((int*)_tmp251)!= 4)goto _LL1A2;_tmp252=((
struct Cyc_Absyn_PointerType_struct*)_tmp251)->f1;_LL1A1: return _tmp252;_LL1A2:;
_LL1A3:({void*_tmp253[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp254="get_ptr_typ: not a pointer!";
_tag_dynforward(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,28));}),
_tag_dynforward(_tmp253,sizeof(void*),0));});_LL19F:;}static int Cyc_Toc_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*
_tmp255=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp256;void*_tmp257;
struct Cyc_Absyn_PtrAtts _tmp258;struct Cyc_Absyn_Conref*_tmp259;struct Cyc_Absyn_Conref*
_tmp25A;struct Cyc_Absyn_ArrayInfo _tmp25B;void*_tmp25C;struct Cyc_Absyn_Tqual
_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Conref*_tmp25F;_LL1A5: if(
_tmp255 <= (void*)4)goto _LL1A9;if(*((int*)_tmp255)!= 4)goto _LL1A7;_tmp256=((
struct Cyc_Absyn_PointerType_struct*)_tmp255)->f1;_tmp257=(void*)_tmp256.elt_typ;
_tmp258=_tmp256.ptr_atts;_tmp259=_tmp258.bounds;_tmp25A=_tmp258.zero_term;_LL1A6:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp25A)){*
ptr_type=t;*elt_type=_tmp257;{void*_tmp260=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp259);_LL1AC: if((int)_tmp260 != 0)goto _LL1AE;_LL1AD:*is_dyneither=0;*
is_dynforward=1;goto _LL1AB;_LL1AE: if((int)_tmp260 != 1)goto _LL1B0;_LL1AF:*
is_dynforward=0;*is_dyneither=1;goto _LL1AB;_LL1B0:;_LL1B1:*is_dynforward=(*
is_dyneither=0);goto _LL1AB;_LL1AB:;}return 1;}else{return 0;}_LL1A7: if(*((int*)
_tmp255)!= 7)goto _LL1A9;_tmp25B=((struct Cyc_Absyn_ArrayType_struct*)_tmp255)->f1;
_tmp25C=(void*)_tmp25B.elt_type;_tmp25D=_tmp25B.tq;_tmp25E=_tmp25B.num_elts;
_tmp25F=_tmp25B.zero_term;_LL1A8: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp25F)){*elt_type=_tmp25C;*is_dynforward=(*is_dyneither=0);*ptr_type=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261[0]=({
struct Cyc_Absyn_PointerType_struct _tmp262;_tmp262.tag=4;_tmp262.f1=({struct Cyc_Absyn_PtrInfo
_tmp263;_tmp263.elt_typ=(void*)_tmp25C;_tmp263.elt_tq=_tmp25D;_tmp263.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp264;_tmp264.rgn=(void*)((void*)2);_tmp264.nullable=
Cyc_Absyn_false_conref;_tmp264.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp266;_tmp266.tag=0;_tmp266.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp25E);
_tmp266;});_tmp265;}));_tmp264.zero_term=_tmp25F;_tmp264.ptrloc=0;_tmp264;});
_tmp263;});_tmp262;});_tmp261;});return 1;}else{return 0;}_LL1A9:;_LL1AA: return 0;
_LL1A4:;}static int Cyc_Toc_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*_tmp267=(void*)
e1->r;struct Cyc_Absyn_Exp*_tmp268;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*
_tmp26A;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Exp*
_tmp26D;_LL1B3: if(*((int*)_tmp267)!= 15)goto _LL1B5;_LL1B4:({struct Cyc_String_pa_struct
_tmp270;_tmp270.tag=0;_tmp270.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp26E[1]={& _tmp270};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp26F="we have a cast in a lhs:  %s";_tag_dynforward(_tmp26F,sizeof(char),
_get_zero_arr_size(_tmp26F,29));}),_tag_dynforward(_tmp26E,sizeof(void*),1));}});
_LL1B5: if(*((int*)_tmp267)!= 22)goto _LL1B7;_tmp268=((struct Cyc_Absyn_Deref_e_struct*)
_tmp267)->f1;_LL1B6: _tmp269=_tmp268;goto _LL1B8;_LL1B7: if(*((int*)_tmp267)!= 25)
goto _LL1B9;_tmp269=((struct Cyc_Absyn_Subscript_e_struct*)_tmp267)->f1;_LL1B8:
return Cyc_Toc_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp269->topt))->v,
ptr_type,is_dynforward,is_dyneither,elt_type);_LL1B9: if(*((int*)_tmp267)!= 24)
goto _LL1BB;_tmp26A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp267)->f1;_LL1BA:
_tmp26B=_tmp26A;goto _LL1BC;_LL1BB: if(*((int*)_tmp267)!= 23)goto _LL1BD;_tmp26B=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp267)->f1;_LL1BC: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp26B->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp273;_tmp273.tag=
0;_tmp273.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp271[1]={& _tmp273};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp272="found zero pointer aggregate member assignment: %s";
_tag_dynforward(_tmp272,sizeof(char),_get_zero_arr_size(_tmp272,51));}),
_tag_dynforward(_tmp271,sizeof(void*),1));}});return 0;_LL1BD: if(*((int*)_tmp267)
!= 14)goto _LL1BF;_tmp26C=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp267)->f1;
_LL1BE: _tmp26D=_tmp26C;goto _LL1C0;_LL1BF: if(*((int*)_tmp267)!= 13)goto _LL1C1;
_tmp26D=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp267)->f1;_LL1C0: if(Cyc_Toc_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp26D->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmp276;_tmp276.tag=
0;_tmp276.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmp274[1]={& _tmp276};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp275="found zero pointer instantiate/noinstantiate: %s";
_tag_dynforward(_tmp275,sizeof(char),_get_zero_arr_size(_tmp275,49));}),
_tag_dynforward(_tmp274,sizeof(void*),1));}});return 0;_LL1C1: if(*((int*)_tmp267)
!= 1)goto _LL1C3;_LL1C2: return 0;_LL1C3:;_LL1C4:({struct Cyc_String_pa_struct
_tmp279;_tmp279.tag=0;_tmp279.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e1));{void*_tmp277[1]={& _tmp279};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp278="found bad lhs in is_zero_ptr_deref: %s";_tag_dynforward(_tmp278,sizeof(
char),_get_zero_arr_size(_tmp278,39));}),_tag_dynforward(_tmp277,sizeof(void*),1));}});
_LL1B2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp27A=Cyc_Tcutil_compress(t);void*_tmp27B;void*_tmp27C;void*_tmp27D;
void*_tmp27E;void*_tmp27F;void*_tmp280;void*_tmp281;void*_tmp282;void*_tmp283;
void*_tmp284;_LL1C6: if(_tmp27A <= (void*)4)goto _LL1D6;if(*((int*)_tmp27A)!= 4)
goto _LL1C8;_LL1C7: res=Cyc_Absyn_null_exp(0);goto _LL1C5;_LL1C8: if(*((int*)_tmp27A)
!= 5)goto _LL1CA;_tmp27B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp27A)->f1;
_tmp27C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp27A)->f2;if((int)_tmp27C != 
0)goto _LL1CA;_LL1C9: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285[0]=({struct Cyc_Absyn_Char_c_struct
_tmp286;_tmp286.tag=0;_tmp286.f1=(void*)_tmp27B;_tmp286.f2='\000';_tmp286;});
_tmp285;}),0);goto _LL1C5;_LL1CA: if(*((int*)_tmp27A)!= 5)goto _LL1CC;_tmp27D=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp27A)->f1;_tmp27E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp27A)->f2;if((int)_tmp27E != 1)goto _LL1CC;_LL1CB: res=Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_Short_c_struct*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287[0]=({
struct Cyc_Absyn_Short_c_struct _tmp288;_tmp288.tag=1;_tmp288.f1=(void*)_tmp27D;
_tmp288.f2=0;_tmp288;});_tmp287;}),0);goto _LL1C5;_LL1CC: if(*((int*)_tmp27A)!= 12)
goto _LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(*((int*)_tmp27A)!= 13)goto _LL1D0;_LL1CF:
_tmp27F=(void*)1;goto _LL1D1;_LL1D0: if(*((int*)_tmp27A)!= 5)goto _LL1D2;_tmp27F=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp27A)->f1;_tmp280=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp27A)->f2;if((int)_tmp280 != 2)goto _LL1D2;_LL1D1: _tmp281=_tmp27F;goto _LL1D3;
_LL1D2: if(*((int*)_tmp27A)!= 5)goto _LL1D4;_tmp281=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp27A)->f1;_tmp282=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp27A)->f2;if((
int)_tmp282 != 3)goto _LL1D4;_LL1D3: res=Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289[0]=({struct Cyc_Absyn_Int_c_struct
_tmp28A;_tmp28A.tag=2;_tmp28A.f1=(void*)_tmp281;_tmp28A.f2=0;_tmp28A;});_tmp289;}),
0);goto _LL1C5;_LL1D4: if(*((int*)_tmp27A)!= 5)goto _LL1D6;_tmp283=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp27A)->f1;_tmp284=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp27A)->f2;if((int)_tmp284 != 4)goto _LL1D6;_LL1D5: res=Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_LongLong_c_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B[0]=({
struct Cyc_Absyn_LongLong_c_struct _tmp28C;_tmp28C.tag=3;_tmp28C.f1=(void*)_tmp283;
_tmp28C.f2=(long long)0;_tmp28C;});_tmp28B;}),0);goto _LL1C5;_LL1D6: if((int)
_tmp27A != 1)goto _LL1D8;_LL1D7: goto _LL1D9;_LL1D8: if(_tmp27A <= (void*)4)goto _LL1DA;
if(*((int*)_tmp27A)!= 6)goto _LL1DA;_LL1D9: res=Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_Float_c_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=({
struct Cyc_Absyn_Float_c_struct _tmp28E;_tmp28E.tag=4;_tmp28E.f1=({const char*
_tmp28F="0.0";_tag_dynforward(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,4));});
_tmp28E;});_tmp28D;}),0);goto _LL1C5;_LL1DA:;_LL1DB:({struct Cyc_String_pa_struct
_tmp292;_tmp292.tag=0;_tmp292.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp290[1]={& _tmp292};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp291="found non-zero type %s in generate_zero";_tag_dynforward(_tmp291,
sizeof(char),_get_zero_arr_size(_tmp291,40));}),_tag_dynforward(_tmp290,sizeof(
void*),1));}});_LL1C5:;}res->topt=({struct Cyc_Core_Opt*_tmp293=_cycalloc(sizeof(*
_tmp293));_tmp293->v=(void*)t;_tmp293;});return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dynforward,int is_dyneither,void*
elt_type){int is_fat=is_dynforward  || is_dyneither;void*fat_ptr_type;if(
is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,
Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)
2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp294=Cyc_Toc_typ_to_c(elt_type);
void*_tmp295=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp296=Cyc_Absyn_cstar_typ(
_tmp294,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp297=({struct Cyc_Core_Opt*_tmp2D3=
_cycalloc(sizeof(*_tmp2D3));_tmp2D3->v=(void*)_tmp296;_tmp2D3;});struct Cyc_Absyn_Exp*
xinit;{void*_tmp298=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*
_tmp29A;struct Cyc_Absyn_Exp*_tmp29B;_LL1DD: if(*((int*)_tmp298)!= 22)goto _LL1DF;
_tmp299=((struct Cyc_Absyn_Deref_e_struct*)_tmp298)->f1;_LL1DE: if(!is_fat){
_tmp299=Cyc_Toc_cast_it(fat_ptr_type,_tmp299);_tmp299->topt=({struct Cyc_Core_Opt*
_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->v=(void*)fat_ptr_type;_tmp29C;});}
Cyc_Toc_exp_to_c(nv,_tmp299);xinit=_tmp299;goto _LL1DC;_LL1DF: if(*((int*)_tmp298)
!= 25)goto _LL1E1;_tmp29A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp298)->f1;
_tmp29B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp298)->f2;_LL1E0: if(!is_fat){
_tmp29A=Cyc_Toc_cast_it(fat_ptr_type,_tmp29A);_tmp29A->topt=({struct Cyc_Core_Opt*
_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->v=(void*)fat_ptr_type;_tmp29D;});}
Cyc_Toc_exp_to_c(nv,_tmp29A);Cyc_Toc_exp_to_c(nv,_tmp29B);if(is_dynforward)xinit=
Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp29E[3];_tmp29E[2]=_tmp29B;_tmp29E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp29E[0]=_tmp29A;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp29E,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
else{xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp29F[3];_tmp29F[2]=_tmp29B;_tmp29F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp29F[0]=_tmp29A;((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp29F,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
goto _LL1DC;_LL1E1:;_LL1E2:({void*_tmp2A0[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2A1="found bad lhs for zero-terminated pointer assignment";
_tag_dynforward(_tmp2A1,sizeof(char),_get_zero_arr_size(_tmp2A1,53));}),
_tag_dynforward(_tmp2A0,sizeof(void*),0));});_LL1DC:;}{struct _tuple1*_tmp2A2=Cyc_Toc_temp_var();
nv=Cyc_Toc_add_varmap(nv,_tmp2A2,Cyc_Absyn_var_exp(_tmp2A2,0));{struct Cyc_Absyn_Vardecl*
_tmp2A3=({struct Cyc_Absyn_Vardecl*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->sc=(
void*)((void*)2);_tmp2D2->name=_tmp2A2;_tmp2D2->tq=Cyc_Toc_mt_tq;_tmp2D2->type=(
void*)_tmp295;_tmp2D2->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp2D2->rgn=0;
_tmp2D2->attributes=0;_tmp2D2->escapes=0;_tmp2D2;});struct Cyc_Absyn_Local_b_struct*
_tmp2A4=({struct Cyc_Absyn_Local_b_struct*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));
_tmp2D0[0]=({struct Cyc_Absyn_Local_b_struct _tmp2D1;_tmp2D1.tag=3;_tmp2D1.f1=
_tmp2A3;_tmp2D1;});_tmp2D0;});struct Cyc_Absyn_Exp*_tmp2A5=Cyc_Absyn_varb_exp(
_tmp2A2,(void*)_tmp2A4,0);_tmp2A5->topt=({struct Cyc_Core_Opt*_tmp2A6=_cycalloc(
sizeof(*_tmp2A6));_tmp2A6->v=(void*)fat_ptr_type;_tmp2A6;});{struct Cyc_Absyn_Exp*
_tmp2A7=Cyc_Absyn_deref_exp(_tmp2A5,0);_tmp2A7->topt=({struct Cyc_Core_Opt*
_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->v=(void*)elt_type;_tmp2A8;});Cyc_Toc_exp_to_c(
nv,_tmp2A7);{struct _tuple1*_tmp2A9=Cyc_Toc_temp_var();nv=Cyc_Toc_add_varmap(nv,
_tmp2A9,Cyc_Absyn_var_exp(_tmp2A9,0));{struct Cyc_Absyn_Vardecl*_tmp2AA=({struct
Cyc_Absyn_Vardecl*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF->sc=(void*)((void*)
2);_tmp2CF->name=_tmp2A9;_tmp2CF->tq=Cyc_Toc_mt_tq;_tmp2CF->type=(void*)_tmp294;
_tmp2CF->initializer=(struct Cyc_Absyn_Exp*)_tmp2A7;_tmp2CF->rgn=0;_tmp2CF->attributes=
0;_tmp2CF->escapes=0;_tmp2CF;});struct Cyc_Absyn_Local_b_struct*_tmp2AB=({struct
Cyc_Absyn_Local_b_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2CE;_tmp2CE.tag=3;_tmp2CE.f1=_tmp2AA;_tmp2CE;});
_tmp2CD;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp2AC=Cyc_Absyn_varb_exp(_tmp2A9,(void*)_tmp2AB,0);_tmp2AC->topt=_tmp2A7->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp2AC,e2,0);z_init->topt=_tmp2AC->topt;}
Cyc_Toc_exp_to_c(nv,z_init);{struct _tuple1*_tmp2AD=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp2AE=({struct Cyc_Absyn_Vardecl*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->sc=(
void*)((void*)2);_tmp2CC->name=_tmp2AD;_tmp2CC->tq=Cyc_Toc_mt_tq;_tmp2CC->type=(
void*)_tmp294;_tmp2CC->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp2CC->rgn=0;
_tmp2CC->attributes=0;_tmp2CC->escapes=0;_tmp2CC;});struct Cyc_Absyn_Local_b_struct*
_tmp2AF=({struct Cyc_Absyn_Local_b_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));
_tmp2CA[0]=({struct Cyc_Absyn_Local_b_struct _tmp2CB;_tmp2CB.tag=3;_tmp2CB.f1=
_tmp2AE;_tmp2CB;});_tmp2CA;});nv=Cyc_Toc_add_varmap(nv,_tmp2AD,Cyc_Absyn_var_exp(
_tmp2AD,0));{struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_varb_exp(_tmp2A9,(void*)
_tmp2AB,0);_tmp2B0->topt=_tmp2A7->topt;{struct Cyc_Absyn_Exp*_tmp2B1=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp2B2=Cyc_Absyn_prim2_exp((void*)5,_tmp2B0,
_tmp2B1,0);_tmp2B2->topt=({struct Cyc_Core_Opt*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));
_tmp2B3->v=(void*)Cyc_Absyn_sint_typ;_tmp2B3;});Cyc_Toc_exp_to_c(nv,_tmp2B2);{
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_varb_exp(_tmp2AD,(void*)_tmp2AF,0);_tmp2B4->topt=
_tmp2A7->topt;{struct Cyc_Absyn_Exp*_tmp2B5=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2B6=Cyc_Absyn_prim2_exp((void*)6,_tmp2B4,_tmp2B5,0);
_tmp2B6->topt=({struct Cyc_Core_Opt*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->v=(
void*)Cyc_Absyn_sint_typ;_tmp2B7;});Cyc_Toc_exp_to_c(nv,_tmp2B6);{struct Cyc_List_List*
_tmp2B8=({struct Cyc_Absyn_Exp*_tmp2C9[2];_tmp2C9[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp2C9[0]=Cyc_Absyn_varb_exp(_tmp2A2,(void*)_tmp2A4,0);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp2C9,sizeof(struct Cyc_Absyn_Exp*),
2));});struct Cyc_Absyn_Exp*_tmp2B9=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*
xsize;if(is_dynforward)xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(
Cyc_Toc__get_dynforward_size_e,_tmp2B8,0),_tmp2B9,0);else{xsize=Cyc_Absyn_prim2_exp((
void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2B8,0),_tmp2B9,0);}{
struct Cyc_Absyn_Exp*_tmp2BA=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp2B2,
_tmp2B6,0),0);struct Cyc_Absyn_Stmt*_tmp2BB=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp2A2,(void*)_tmp2A4,0),Cyc_Toc_curr_sp,0);_tmp2BC=Cyc_Toc_cast_it(
_tmp296,_tmp2BC);{struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_deref_exp(_tmp2BC,0);
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_assign_exp(_tmp2BD,Cyc_Absyn_var_exp(
_tmp2AD,0),0);struct Cyc_Absyn_Stmt*_tmp2BF=Cyc_Absyn_exp_stmt(_tmp2BE,0);_tmp2BF=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp2BA,_tmp2BB,Cyc_Absyn_skip_stmt(
0),0),_tmp2BF,0);_tmp2BF=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp2C0=
_cycalloc(sizeof(*_tmp2C0));_tmp2C0->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=_tmp2AE;_tmp2C2;});_tmp2C1;}));_tmp2C0->loc=0;
_tmp2C0;}),_tmp2BF,0);_tmp2BF=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp2C3=
_cycalloc(sizeof(*_tmp2C3));_tmp2C3->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2C5;_tmp2C5.tag=0;_tmp2C5.f1=_tmp2AA;_tmp2C5;});_tmp2C4;}));_tmp2C3->loc=0;
_tmp2C3;}),_tmp2BF,0);_tmp2BF=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp2C6=
_cycalloc(sizeof(*_tmp2C6));_tmp2C6->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7[0]=({struct Cyc_Absyn_Var_d_struct
_tmp2C8;_tmp2C8.tag=0;_tmp2C8.f1=_tmp2A3;_tmp2C8;});_tmp2C7;}));_tmp2C6->loc=0;
_tmp2C6;}),_tmp2BF,0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp2BF,0))->r));}}}}}}}}}}}}}}}
struct _tuple10{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp2D4=(void*)e->r;if(e->topt
== 0)({struct Cyc_String_pa_struct _tmp2D7;_tmp2D7.tag=0;_tmp2D7.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp2D5[
1]={& _tmp2D7};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2D6="exp_to_c: no type for %s";_tag_dynforward(_tmp2D6,sizeof(char),
_get_zero_arr_size(_tmp2D6,25));}),_tag_dynforward(_tmp2D5,sizeof(void*),1));}});{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp2D8=
_tmp2D4;void*_tmp2D9;struct _tuple1*_tmp2DA;void*_tmp2DB;struct _tuple1*_tmp2DC;
void*_tmp2DD;struct Cyc_List_List*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DF;void*
_tmp2E0;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_Core_Opt*_tmp2E2;struct Cyc_Absyn_Exp*
_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E5;struct Cyc_Absyn_Exp*
_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_Absyn_Exp*
_tmp2E9;struct Cyc_Absyn_Exp*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EB;struct Cyc_Absyn_Exp*
_tmp2EC;struct Cyc_Absyn_Exp*_tmp2ED;struct Cyc_List_List*_tmp2EE;struct Cyc_Absyn_Exp*
_tmp2EF;struct Cyc_List_List*_tmp2F0;struct Cyc_Absyn_VarargCallInfo*_tmp2F1;
struct Cyc_Absyn_Exp*_tmp2F2;struct Cyc_List_List*_tmp2F3;struct Cyc_Absyn_VarargCallInfo*
_tmp2F4;struct Cyc_Absyn_VarargCallInfo _tmp2F5;int _tmp2F6;struct Cyc_List_List*
_tmp2F7;struct Cyc_Absyn_VarargInfo*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;struct Cyc_Absyn_Exp*
_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FB;struct Cyc_List_List*_tmp2FC;void*_tmp2FD;
void**_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;int _tmp300;void*_tmp301;struct Cyc_Absyn_Exp*
_tmp302;struct Cyc_Absyn_Exp*_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*
_tmp305;void*_tmp306;void*_tmp307;void*_tmp308;struct _dynforward_ptr*_tmp309;
void*_tmp30A;void*_tmp30B;unsigned int _tmp30C;struct Cyc_Absyn_Exp*_tmp30D;struct
Cyc_Absyn_Exp*_tmp30E;struct _dynforward_ptr*_tmp30F;struct Cyc_Absyn_Exp*_tmp310;
struct _dynforward_ptr*_tmp311;struct Cyc_Absyn_Exp*_tmp312;struct Cyc_Absyn_Exp*
_tmp313;struct Cyc_List_List*_tmp314;struct Cyc_List_List*_tmp315;struct Cyc_Absyn_Vardecl*
_tmp316;struct Cyc_Absyn_Exp*_tmp317;struct Cyc_Absyn_Exp*_tmp318;int _tmp319;
struct _tuple1*_tmp31A;struct Cyc_List_List*_tmp31B;struct Cyc_List_List*_tmp31C;
struct Cyc_Absyn_Aggrdecl*_tmp31D;void*_tmp31E;struct Cyc_List_List*_tmp31F;struct
Cyc_List_List*_tmp320;struct Cyc_Absyn_Tuniondecl*_tmp321;struct Cyc_Absyn_Tunionfield*
_tmp322;struct Cyc_List_List*_tmp323;struct Cyc_Absyn_Tuniondecl*_tmp324;struct Cyc_Absyn_Tunionfield*
_tmp325;struct Cyc_Absyn_MallocInfo _tmp326;int _tmp327;struct Cyc_Absyn_Exp*_tmp328;
void**_tmp329;struct Cyc_Absyn_Exp*_tmp32A;int _tmp32B;struct Cyc_Absyn_Stmt*
_tmp32C;_LL1E4: if(*((int*)_tmp2D8)!= 0)goto _LL1E6;_tmp2D9=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2D8)->f1;if((int)_tmp2D9 != 0)goto _LL1E6;_LL1E5: {struct Cyc_Absyn_Exp*_tmp32D=
Cyc_Absyn_uint_exp(0,0);int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only)){if(nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(
old_typ,_tmp32D,_tmp32D,forward_only))->r));else{if(forward_only)(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp32E[3];_tmp32E[2]=_tmp32D;_tmp32E[1]=_tmp32D;_tmp32E[0]=_tmp32D;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp32E,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp32F[3];_tmp32F[2]=_tmp32D;_tmp32F[1]=_tmp32D;_tmp32F[0]=
_tmp32D;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp32F,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}}else{(
void*)(e->r=(void*)((void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL1E3;}
_LL1E6: if(*((int*)_tmp2D8)!= 0)goto _LL1E8;_LL1E7: goto _LL1E3;_LL1E8: if(*((int*)
_tmp2D8)!= 1)goto _LL1EA;_tmp2DA=((struct Cyc_Absyn_Var_e_struct*)_tmp2D8)->f1;
_tmp2DB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D8)->f2;_LL1E9:{struct
_handler_cons _tmp330;_push_handler(& _tmp330);{int _tmp332=0;if(setjmp(_tmp330.handler))
_tmp332=1;if(!_tmp332){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp2DA))->r));;
_pop_handler();}else{void*_tmp331=(void*)_exn_thrown;void*_tmp334=_tmp331;_LL239:
if(_tmp334 != Cyc_Dict_Absent)goto _LL23B;_LL23A:({struct Cyc_String_pa_struct
_tmp337;_tmp337.tag=0;_tmp337.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_qvar2string(_tmp2DA));{void*_tmp335[1]={& _tmp337};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp336="Can't find %s in exp_to_c, Var\n";_tag_dynforward(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,32));}),_tag_dynforward(_tmp335,sizeof(void*),1));}});
_LL23B:;_LL23C:(void)_throw(_tmp334);_LL238:;}}}goto _LL1E3;_LL1EA: if(*((int*)
_tmp2D8)!= 2)goto _LL1EC;_tmp2DC=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp2D8)->f1;
_LL1EB:({void*_tmp338[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp339="unknownid";
_tag_dynforward(_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,10));}),
_tag_dynforward(_tmp338,sizeof(void*),0));});_LL1EC: if(*((int*)_tmp2D8)!= 3)goto
_LL1EE;_tmp2DD=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2D8)->f1;_tmp2DE=((
struct Cyc_Absyn_Primop_e_struct*)_tmp2D8)->f2;_LL1ED: {struct Cyc_List_List*
_tmp33A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp2DE);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp2DE);{void*_tmp33B=_tmp2DD;_LL23E: if((int)_tmp33B != 19)
goto _LL240;_LL23F: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2DE))->hd;{void*_tmp33C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp33D;struct Cyc_Absyn_Exp*
_tmp33E;struct Cyc_Absyn_PtrInfo _tmp33F;void*_tmp340;struct Cyc_Absyn_PtrAtts
_tmp341;struct Cyc_Absyn_Conref*_tmp342;struct Cyc_Absyn_Conref*_tmp343;struct Cyc_Absyn_Conref*
_tmp344;_LL253: if(_tmp33C <= (void*)4)goto _LL257;if(*((int*)_tmp33C)!= 7)goto
_LL255;_tmp33D=((struct Cyc_Absyn_ArrayType_struct*)_tmp33C)->f1;_tmp33E=_tmp33D.num_elts;
_LL254:(void*)(e->r=(void*)((void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp33E))->r));
goto _LL252;_LL255: if(*((int*)_tmp33C)!= 4)goto _LL257;_tmp33F=((struct Cyc_Absyn_PointerType_struct*)
_tmp33C)->f1;_tmp340=(void*)_tmp33F.elt_typ;_tmp341=_tmp33F.ptr_atts;_tmp342=
_tmp341.nullable;_tmp343=_tmp341.bounds;_tmp344=_tmp341.zero_term;_LL256:{void*
_tmp345=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp343);struct Cyc_Absyn_Exp*
_tmp346;_LL25A: if((int)_tmp345 != 0)goto _LL25C;_LL25B:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,({struct Cyc_Absyn_Exp*
_tmp347[2];_tmp347[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp340),0);
_tmp347[0]=(struct Cyc_Absyn_Exp*)_tmp2DE->hd;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp347,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL259;_LL25C: if((int)_tmp345 != 1)goto _LL25E;_LL25D:(void*)(
e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,({struct
Cyc_Absyn_Exp*_tmp348[2];_tmp348[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp340),0);_tmp348[0]=(struct Cyc_Absyn_Exp*)_tmp2DE->hd;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp348,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL259;_LL25E: if(_tmp345 <= (void*)2)goto _LL260;if(*((int*)
_tmp345)!= 0)goto _LL260;_tmp346=((struct Cyc_Absyn_Upper_b_struct*)_tmp345)->f1;
_LL25F: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp344))(
void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp349[2];_tmp349[1]=_tmp346;_tmp349[0]=(struct Cyc_Absyn_Exp*)
_tmp2DE->hd;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp349,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));else{if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp342))(void*)(e->r=(
void*)((void*)(Cyc_Absyn_conditional_exp(arg,_tmp346,Cyc_Absyn_uint_exp(0,0),0))->r));
else{(void*)(e->r=(void*)((void*)_tmp346->r));goto _LL259;}}goto _LL259;_LL260: if(
_tmp345 <= (void*)2)goto _LL259;if(*((int*)_tmp345)!= 1)goto _LL259;_LL261:({void*
_tmp34A[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp34B="toc: size of AbsUpper_b";_tag_dynforward(_tmp34B,sizeof(char),
_get_zero_arr_size(_tmp34B,24));}),_tag_dynforward(_tmp34A,sizeof(void*),0));});
_LL259:;}goto _LL252;_LL257:;_LL258:({struct Cyc_String_pa_struct _tmp34F;_tmp34F.tag=
0;_tmp34F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{struct Cyc_String_pa_struct
_tmp34E;_tmp34E.tag=0;_tmp34E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{
void*_tmp34C[2]={& _tmp34E,& _tmp34F};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp34D="size primop applied to non-array %s (%s)";
_tag_dynforward(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,41));}),
_tag_dynforward(_tmp34C,sizeof(void*),2));}}});_LL252:;}goto _LL23D;}_LL240: if((
int)_tmp33B != 0)goto _LL242;_LL241:{void*_tmp350=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp33A))->hd);struct Cyc_Absyn_PtrInfo _tmp351;
void*_tmp352;struct Cyc_Absyn_PtrAtts _tmp353;struct Cyc_Absyn_Conref*_tmp354;
struct Cyc_Absyn_Conref*_tmp355;_LL263: if(_tmp350 <= (void*)4)goto _LL265;if(*((int*)
_tmp350)!= 4)goto _LL265;_tmp351=((struct Cyc_Absyn_PointerType_struct*)_tmp350)->f1;
_tmp352=(void*)_tmp351.elt_typ;_tmp353=_tmp351.ptr_atts;_tmp354=_tmp353.bounds;
_tmp355=_tmp353.zero_term;_LL264:{void*_tmp356=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp354);struct Cyc_Absyn_Exp*_tmp357;_LL268: if((int)_tmp356 != 0)goto _LL26A;
_LL269: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2DE))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2DE->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__dynforward_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp358[3];_tmp358[2]=e2;
_tmp358[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp352),0);_tmp358[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp358,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));goto _LL267;}_LL26A: if((int)
_tmp356 != 1)goto _LL26C;_LL26B: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2DE))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp2DE->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp359[
3];_tmp359[2]=e2;_tmp359[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp352),0);
_tmp359[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp359,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));goto _LL267;}
_LL26C: if(_tmp356 <= (void*)2)goto _LL26E;if(*((int*)_tmp356)!= 0)goto _LL26E;
_tmp357=((struct Cyc_Absyn_Upper_b_struct*)_tmp356)->f1;_LL26D: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp355)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2DE))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2DE->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp35A[3];_tmp35A[2]=e2;_tmp35A[1]=_tmp357;_tmp35A[0]=e1;((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp35A,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL267;_LL26E: if(
_tmp356 <= (void*)2)goto _LL267;if(*((int*)_tmp356)!= 1)goto _LL267;_LL26F:({void*
_tmp35B[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp35C="toc: plus on AbsUpper_b";_tag_dynforward(_tmp35C,sizeof(char),
_get_zero_arr_size(_tmp35C,24));}),_tag_dynforward(_tmp35B,sizeof(void*),0));});
_LL267:;}goto _LL262;_LL265:;_LL266: goto _LL262;_LL262:;}goto _LL23D;_LL242: if((int)
_tmp33B != 2)goto _LL244;_LL243: {void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp33A))->hd,& elt_typ,& forward_only)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp2DE))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2DE->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp33A->tl))->hd,& forward_only)){(void*)(
e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),
Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e->r=(void*)((
void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{if(forward_only)({void*_tmp35D[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp35E="subtraction not allowed on forward-only ? pointer";
_tag_dynforward(_tmp35E,sizeof(char),_get_zero_arr_size(_tmp35E,50));}),
_tag_dynforward(_tmp35D,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp35F[3];_tmp35F[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0);_tmp35F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp35F[0]=e1;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp35F,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto
_LL23D;}_LL244: if((int)_tmp33B != 5)goto _LL246;_LL245: goto _LL247;_LL246: if((int)
_tmp33B != 6)goto _LL248;_LL247: goto _LL249;_LL248: if((int)_tmp33B != 7)goto _LL24A;
_LL249: goto _LL24B;_LL24A: if((int)_tmp33B != 9)goto _LL24C;_LL24B: goto _LL24D;_LL24C:
if((int)_tmp33B != 8)goto _LL24E;_LL24D: goto _LL24F;_LL24E: if((int)_tmp33B != 10)
goto _LL250;_LL24F: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2DE))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp2DE->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp33A))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp33A->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));goto _LL23D;}_LL250:;_LL251: goto _LL23D;
_LL23D:;}goto _LL1E3;}_LL1EE: if(*((int*)_tmp2D8)!= 5)goto _LL1F0;_tmp2DF=((struct
Cyc_Absyn_Increment_e_struct*)_tmp2D8)->f1;_tmp2E0=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp2D8)->f2;_LL1EF: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2DF->topt))->v;void*ptr_type=(void*)0;void*elt_type=(void*)0;int
is_dynforward=0;int is_dyneither=0;struct _dynforward_ptr incr_str=({const char*
_tmp36B="increment";_tag_dynforward(_tmp36B,sizeof(char),_get_zero_arr_size(
_tmp36B,10));});if(_tmp2E0 == (void*)2  || _tmp2E0 == (void*)3)incr_str=({const char*
_tmp360="decrement";_tag_dynforward(_tmp360,sizeof(char),_get_zero_arr_size(
_tmp360,10));});if(Cyc_Toc_is_zero_ptr_deref(_tmp2DF,& ptr_type,& is_dynforward,&
is_dyneither,& elt_type)){({struct Cyc_String_pa_struct _tmp363;_tmp363.tag=0;
_tmp363.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)incr_str);{void*
_tmp361[1]={& _tmp363};Cyc_Tcutil_terr(e->loc,({const char*_tmp362="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dynforward(_tmp362,sizeof(char),_get_zero_arr_size(_tmp362,74));}),
_tag_dynforward(_tmp361,sizeof(void*),1));}});({void*_tmp364[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp365="in-place inc on zero-term";_tag_dynforward(_tmp365,sizeof(char),
_get_zero_arr_size(_tmp365,26));}),_tag_dynforward(_tmp364,sizeof(void*),0));});}
Cyc_Toc_exp_to_c(nv,_tmp2DF);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp2E0 == (void*)3  || _tmp2E0 == (void*)2)({void*_tmp366[0]={};
Cyc_Tcutil_terr(e->loc,({const char*_tmp367="decrement not allowed on forward ? pointer";
_tag_dynforward(_tmp367,sizeof(char),_get_zero_arr_size(_tmp367,43));}),
_tag_dynforward(_tmp366,sizeof(void*),0));});fn_e=_tmp2E0 == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp2E0 == (void*)1  || _tmp2E0
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp2E0 == (void*)2  || _tmp2E0 == (void*)3)change=- 1;}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(fn_e,({struct Cyc_Absyn_Exp*_tmp368[3];_tmp368[2]=Cyc_Absyn_signed_int_exp(
change,0);_tmp368[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp368[0]=Cyc_Absyn_address_exp(_tmp2DF,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp368,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*_tmp369=_tmp2E0 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_e:
Cyc_Toc__zero_arr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp369,({struct Cyc_Absyn_Exp*_tmp36A[2];_tmp36A[1]=Cyc_Absyn_signed_int_exp(1,0);
_tmp36A[0]=_tmp2DF;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp36A,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}else{if(
elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp2DF)){Cyc_Toc_lvalue_assign(
_tmp2DF,0,Cyc_Toc_incr_lvalue,_tmp2E0);(void*)(e->r=(void*)((void*)_tmp2DF->r));}}}
goto _LL1E3;}}_LL1F0: if(*((int*)_tmp2D8)!= 4)goto _LL1F2;_tmp2E1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp2D8)->f1;_tmp2E2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp2D8)->f2;_tmp2E3=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp2D8)->f3;_LL1F1: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Toc_is_zero_ptr_deref(
_tmp2E1,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp2E1,_tmp2E2,_tmp2E3,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp2E1);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2E1->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2E3->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp2E1);
Cyc_Toc_exp_to_c(nv,_tmp2E3);{int done=0;if(_tmp2E2 != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp36C=(void*)_tmp2E2->v;_LL271:
if((int)_tmp36C != 0)goto _LL273;_LL272: change=_tmp2E3;goto _LL270;_LL273: if((int)
_tmp36C != 2)goto _LL275;_LL274: if(forward_only)({void*_tmp36D[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp36E="subtraction not allowed on forward ? pointers";
_tag_dynforward(_tmp36E,sizeof(char),_get_zero_arr_size(_tmp36E,46));}),
_tag_dynforward(_tmp36D,sizeof(void*),0));});change=Cyc_Absyn_prim1_exp((void*)2,
_tmp2E3,0);goto _LL270;_LL275:;_LL276:({void*_tmp36F[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp370="bad t ? pointer arithmetic";_tag_dynforward(_tmp370,sizeof(char),
_get_zero_arr_size(_tmp370,27));}),_tag_dynforward(_tmp36F,sizeof(void*),0));});
_LL270:;}done=1;{struct Cyc_Absyn_Exp*_tmp371=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp371,({struct Cyc_Absyn_Exp*_tmp372[3];_tmp372[2]=change;_tmp372[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp372[0]=Cyc_Absyn_address_exp(_tmp2E1,0);((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp372,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp373=(void*)_tmp2E2->v;_LL278: if((int)_tmp373 != 0)
goto _LL27A;_LL279: done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp374[2];_tmp374[1]=_tmp2E3;_tmp374[0]=_tmp2E1;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp374,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL277;_LL27A:;_LL27B:({void*_tmp375[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp376="bad zero-terminated pointer arithmetic";_tag_dynforward(_tmp376,sizeof(
char),_get_zero_arr_size(_tmp376,39));}),_tag_dynforward(_tmp375,sizeof(void*),0));});
_LL277:;}}}if(!done){if(e1_poly)(void*)(_tmp2E3->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp2E3->r,0)))->r));if(!Cyc_Absyn_is_lvalue(
_tmp2E1)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,struct _tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(
_tmp2E1,0,Cyc_Toc_assignop_lvalue,({struct _tuple9*_tmp377=_cycalloc(sizeof(
struct _tuple9)* 1);_tmp377[0]=({struct _tuple9 _tmp378;_tmp378.f1=_tmp2E2;_tmp378.f2=
_tmp2E3;_tmp378;});_tmp377;}));(void*)(e->r=(void*)((void*)_tmp2E1->r));}}goto
_LL1E3;}}}_LL1F2: if(*((int*)_tmp2D8)!= 6)goto _LL1F4;_tmp2E4=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp2D8)->f1;_tmp2E5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2D8)->f2;
_tmp2E6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp2D8)->f3;_LL1F3: Cyc_Toc_exp_to_c(
nv,_tmp2E4);Cyc_Toc_exp_to_c(nv,_tmp2E5);Cyc_Toc_exp_to_c(nv,_tmp2E6);goto _LL1E3;
_LL1F4: if(*((int*)_tmp2D8)!= 7)goto _LL1F6;_tmp2E7=((struct Cyc_Absyn_And_e_struct*)
_tmp2D8)->f1;_tmp2E8=((struct Cyc_Absyn_And_e_struct*)_tmp2D8)->f2;_LL1F5: Cyc_Toc_exp_to_c(
nv,_tmp2E7);Cyc_Toc_exp_to_c(nv,_tmp2E8);goto _LL1E3;_LL1F6: if(*((int*)_tmp2D8)!= 
8)goto _LL1F8;_tmp2E9=((struct Cyc_Absyn_Or_e_struct*)_tmp2D8)->f1;_tmp2EA=((
struct Cyc_Absyn_Or_e_struct*)_tmp2D8)->f2;_LL1F7: Cyc_Toc_exp_to_c(nv,_tmp2E9);
Cyc_Toc_exp_to_c(nv,_tmp2EA);goto _LL1E3;_LL1F8: if(*((int*)_tmp2D8)!= 9)goto
_LL1FA;_tmp2EB=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp2D8)->f1;_tmp2EC=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp2D8)->f2;_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp2EB);Cyc_Toc_exp_to_c(
nv,_tmp2EC);goto _LL1E3;_LL1FA: if(*((int*)_tmp2D8)!= 10)goto _LL1FC;_tmp2ED=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp2D8)->f1;_tmp2EE=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp2D8)->f2;_LL1FB: _tmp2EF=_tmp2ED;_tmp2F0=_tmp2EE;goto _LL1FD;_LL1FC: if(*((int*)
_tmp2D8)!= 11)goto _LL1FE;_tmp2EF=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2D8)->f1;
_tmp2F0=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2D8)->f2;_tmp2F1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2D8)->f3;if(_tmp2F1 != 0)goto _LL1FE;_LL1FD: Cyc_Toc_exp_to_c(nv,_tmp2EF);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp2F0);goto _LL1E3;
_LL1FE: if(*((int*)_tmp2D8)!= 11)goto _LL200;_tmp2F2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp2D8)->f1;_tmp2F3=((struct Cyc_Absyn_FnCall_e_struct*)_tmp2D8)->f2;_tmp2F4=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp2D8)->f3;if(_tmp2F4 == 0)goto _LL200;_tmp2F5=*
_tmp2F4;_tmp2F6=_tmp2F5.num_varargs;_tmp2F7=_tmp2F5.injectors;_tmp2F8=_tmp2F5.vai;
_LL1FF:{struct _RegionHandle _tmp379=_new_region("r");struct _RegionHandle*r=&
_tmp379;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp2F6,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp2F8->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp2F3);int num_normargs=num_args - _tmp2F6;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp2F3=_tmp2F3->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2F3))->hd);
new_args=({struct Cyc_List_List*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->hd=(
struct Cyc_Absyn_Exp*)_tmp2F3->hd;_tmp37A->tl=new_args;_tmp37A;});}}new_args=({
struct Cyc_List_List*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*_tmp37C[3];_tmp37C[2]=
num_varargs_exp;_tmp37C[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp37C[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp37C,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp37B->tl=
new_args;_tmp37B;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp2F2);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp2F2,new_args,0),0);if(_tmp2F8->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp37D=Cyc_Tcutil_compress((void*)_tmp2F8->type);
struct Cyc_Absyn_TunionInfo _tmp37E;void*_tmp37F;struct Cyc_Absyn_Tuniondecl**
_tmp380;struct Cyc_Absyn_Tuniondecl*_tmp381;_LL27D: if(_tmp37D <= (void*)4)goto
_LL27F;if(*((int*)_tmp37D)!= 2)goto _LL27F;_tmp37E=((struct Cyc_Absyn_TunionType_struct*)
_tmp37D)->f1;_tmp37F=(void*)_tmp37E.tunion_info;if(*((int*)_tmp37F)!= 1)goto
_LL27F;_tmp380=((struct Cyc_Absyn_KnownTunion_struct*)_tmp37F)->f1;_tmp381=*
_tmp380;_LL27E: tud=_tmp381;goto _LL27C;_LL27F:;_LL280:({void*_tmp382[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp383="toc: unknown tunion in vararg with inject";_tag_dynforward(_tmp383,
sizeof(char),_get_zero_arr_size(_tmp383,42));}),_tag_dynforward(_tmp382,sizeof(
void*),0));});_LL27C:;}{struct _dynforward_ptr vs=({unsigned int _tmp38B=(
unsigned int)_tmp2F6;struct _tuple1**_tmp38C=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp38B));struct _dynforward_ptr _tmp38E=
_tag_dynforward(_tmp38C,sizeof(struct _tuple1*),_tmp38B);{unsigned int _tmp38D=
_tmp38B;unsigned int i;for(i=0;i < _tmp38D;i ++){_tmp38C[i]=Cyc_Toc_temp_var();}}
_tmp38E;});struct Cyc_List_List*_tmp384=0;{int i=_tmp2F6 - 1;for(0;i >= 0;-- i){
_tmp384=({struct Cyc_List_List*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp385->tl=
_tmp384;_tmp385;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp384,0),s,0);{int i=0;for(0;_tmp2F3 != 0;(((
_tmp2F3=_tmp2F3->tl,_tmp2F7=_tmp2F7->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp2F3->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp387;struct _tuple1*
_tmp388;struct Cyc_List_List*_tmp389;struct Cyc_Absyn_Tunionfield*_tmp386=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp2F7))->hd;_tmp387=*
_tmp386;_tmp388=_tmp387.name;_tmp389=_tmp387.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp389))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp388,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp388,({const char*_tmp38A="_struct";
_tag_dynforward(_tmp38A,sizeof(char),_get_zero_arr_size(_tmp38A,8));}))),0,s,0);}}}}}
else{{int i=0;for(0;_tmp2F3 != 0;(_tmp2F3=_tmp2F3->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp2F3->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp2F3->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL1E3;_LL200: if(*((
int*)_tmp2D8)!= 12)goto _LL202;_tmp2F9=((struct Cyc_Absyn_Throw_e_struct*)_tmp2D8)->f1;
_LL201: Cyc_Toc_exp_to_c(nv,_tmp2F9);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp2F9),0))->r));goto _LL1E3;
_LL202: if(*((int*)_tmp2D8)!= 13)goto _LL204;_tmp2FA=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp2D8)->f1;_LL203: Cyc_Toc_exp_to_c(nv,_tmp2FA);goto _LL1E3;_LL204: if(*((int*)
_tmp2D8)!= 14)goto _LL206;_tmp2FB=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2D8)->f1;
_tmp2FC=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp2D8)->f2;_LL205: Cyc_Toc_exp_to_c(
nv,_tmp2FB);for(0;_tmp2FC != 0;_tmp2FC=_tmp2FC->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp2FC->hd);if(k != (void*)4  && k != (void*)3){{void*_tmp38F=Cyc_Tcutil_compress((
void*)_tmp2FC->hd);_LL282: if(_tmp38F <= (void*)4)goto _LL286;if(*((int*)_tmp38F)!= 
1)goto _LL284;_LL283: goto _LL285;_LL284: if(*((int*)_tmp38F)!= 2)goto _LL286;_LL285:
continue;_LL286:;_LL287:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp2FB,
0))->r));goto _LL281;_LL281:;}break;}}goto _LL1E3;_LL206: if(*((int*)_tmp2D8)!= 15)
goto _LL208;_tmp2FD=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2D8)->f1;_tmp2FE=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2D8)->f1);_tmp2FF=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2D8)->f2;_tmp300=((struct Cyc_Absyn_Cast_e_struct*)
_tmp2D8)->f3;_tmp301=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2D8)->f4;_LL207: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2FF->topt))->v;void*
new_typ=*_tmp2FE;*_tmp2FE=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp2FF);{
struct _tuple0 _tmp391=({struct _tuple0 _tmp390;_tmp390.f1=Cyc_Tcutil_compress(
old_t2);_tmp390.f2=Cyc_Tcutil_compress(new_typ);_tmp390;});void*_tmp392;struct
Cyc_Absyn_PtrInfo _tmp393;void*_tmp394;struct Cyc_Absyn_PtrInfo _tmp395;void*
_tmp396;struct Cyc_Absyn_PtrInfo _tmp397;void*_tmp398;_LL289: _tmp392=_tmp391.f1;
if(_tmp392 <= (void*)4)goto _LL28B;if(*((int*)_tmp392)!= 4)goto _LL28B;_tmp393=((
struct Cyc_Absyn_PointerType_struct*)_tmp392)->f1;_tmp394=_tmp391.f2;if(_tmp394 <= (
void*)4)goto _LL28B;if(*((int*)_tmp394)!= 4)goto _LL28B;_tmp395=((struct Cyc_Absyn_PointerType_struct*)
_tmp394)->f1;_LL28A: {int _tmp399=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp393.ptr_atts).nullable);int _tmp39A=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp395.ptr_atts).nullable);void*_tmp39B=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp393.ptr_atts).bounds);void*_tmp39C=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp395.ptr_atts).bounds);int _tmp39D=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp393.ptr_atts).zero_term);int
_tmp39E=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp395.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmp3A0=({struct _tuple0
_tmp39F;_tmp39F.f1=_tmp39B;_tmp39F.f2=_tmp39C;_tmp39F;});void*_tmp3A1;struct Cyc_Absyn_Exp*
_tmp3A2;void*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;void*_tmp3A5;struct Cyc_Absyn_Exp*
_tmp3A6;void*_tmp3A7;void*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3A9;void*_tmp3AA;void*
_tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AD;void*_tmp3AE;void*_tmp3AF;
struct Cyc_Absyn_Exp*_tmp3B0;void*_tmp3B1;void*_tmp3B2;void*_tmp3B3;void*_tmp3B4;
void*_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;void*_tmp3B9;void*_tmp3BA;
void*_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*_tmp3BE;_LL290: _tmp3A1=_tmp3A0.f1;
if(_tmp3A1 <= (void*)2)goto _LL292;if(*((int*)_tmp3A1)!= 0)goto _LL292;_tmp3A2=((
struct Cyc_Absyn_Upper_b_struct*)_tmp3A1)->f1;_tmp3A3=_tmp3A0.f2;if(_tmp3A3 <= (
void*)2)goto _LL292;if(*((int*)_tmp3A3)!= 0)goto _LL292;_tmp3A4=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3A3)->f1;_LL291: if(_tmp399  && !_tmp39A){if(nv->toplevel)({void*_tmp3BF[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp3C0="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp3C0,sizeof(char),_get_zero_arr_size(_tmp3C0,44));}),_tag_dynforward(_tmp3BF,
sizeof(void*),0));});if(_tmp301 != (void*)2)({struct Cyc_String_pa_struct _tmp3C3;
_tmp3C3.tag=0;_tmp3C3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp3C1[1]={& _tmp3C3};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3C2="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp3C2,sizeof(char),_get_zero_arr_size(_tmp3C2,41));}),
_tag_dynforward(_tmp3C1,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp2FF);if(do_null_check){if(!_tmp300)({void*_tmp3C4[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp3C5="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp3C5,sizeof(char),_get_zero_arr_size(_tmp3C5,58));}),
_tag_dynforward(_tmp3C4,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2FE,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp3C6=
_cycalloc(sizeof(*_tmp3C6));_tmp3C6->hd=_tmp2FF;_tmp3C6->tl=0;_tmp3C6;}),0)))->r));}}}
goto _LL28F;_LL292: _tmp3A5=_tmp3A0.f1;if(_tmp3A5 <= (void*)2)goto _LL294;if(*((int*)
_tmp3A5)!= 0)goto _LL294;_tmp3A6=((struct Cyc_Absyn_Upper_b_struct*)_tmp3A5)->f1;
_tmp3A7=_tmp3A0.f2;if((int)_tmp3A7 != 0)goto _LL294;_LL293: p2_forward_only=1;
_tmp3A9=_tmp3A6;goto _LL295;_LL294: _tmp3A8=_tmp3A0.f1;if(_tmp3A8 <= (void*)2)goto
_LL296;if(*((int*)_tmp3A8)!= 0)goto _LL296;_tmp3A9=((struct Cyc_Absyn_Upper_b_struct*)
_tmp3A8)->f1;_tmp3AA=_tmp3A0.f2;if((int)_tmp3AA != 1)goto _LL296;_LL295: if(_tmp301
== (void*)2)({struct Cyc_String_pa_struct _tmp3C9;_tmp3C9.tag=0;_tmp3C9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp3C7[
1]={& _tmp3C9};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3C8="conversion mis-classified as null-check: %s";_tag_dynforward(
_tmp3C8,sizeof(char),_get_zero_arr_size(_tmp3C8,44));}),_tag_dynforward(_tmp3C7,
sizeof(void*),1));}});if(nv->toplevel){if((_tmp39D  && !(_tmp395.elt_tq).real_const)
 && !_tmp39E)_tmp3A9=Cyc_Absyn_prim2_exp((void*)2,_tmp3A9,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3A9,
_tmp2FF,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp3CA=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp39D){struct _tuple1*
_tmp3CB=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3CC=Cyc_Absyn_var_exp(_tmp3CB,
0);struct Cyc_Absyn_Exp*_tmp3CD=Cyc_Absyn_fncall_exp(Cyc_Toc__get_zero_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp3D2[2];_tmp3D2[1]=_tmp3A9;_tmp3D2[0]=_tmp3CC;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3D2,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);if(!_tmp39E  && !(_tmp395.elt_tq).real_const)_tmp3CD=Cyc_Absyn_prim2_exp((
void*)2,_tmp3CD,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp3CE=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp395.elt_typ),0);struct Cyc_Absyn_Exp*_tmp3CF=Cyc_Absyn_fncall_exp(
_tmp3CA,({struct Cyc_Absyn_Exp*_tmp3D1[3];_tmp3D1[2]=_tmp3CD;_tmp3D1[1]=_tmp3CE;
_tmp3D1[0]=_tmp3CC;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3D1,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp3D0=Cyc_Absyn_exp_stmt(_tmp3CF,0);_tmp3D0=Cyc_Absyn_declare_stmt(_tmp3CB,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp2FF,_tmp3D0,0);(void*)(e->r=(void*)((void*)(
Cyc_Absyn_stmt_exp(_tmp3D0,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp3CA,({struct Cyc_Absyn_Exp*_tmp3D3[3];_tmp3D3[2]=_tmp3A9;_tmp3D3[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp395.elt_typ),0);_tmp3D3[0]=_tmp2FF;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3D3,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}}goto _LL28F;_LL296: _tmp3AB=_tmp3A0.f1;if((int)_tmp3AB != 0)goto
_LL298;_tmp3AC=_tmp3A0.f2;if(_tmp3AC <= (void*)2)goto _LL298;if(*((int*)_tmp3AC)!= 
0)goto _LL298;_tmp3AD=((struct Cyc_Absyn_Upper_b_struct*)_tmp3AC)->f1;_LL297:
p1_forward_only=1;_tmp3B0=_tmp3AD;goto _LL299;_LL298: _tmp3AE=_tmp3A0.f1;if((int)
_tmp3AE != 1)goto _LL29A;_tmp3AF=_tmp3A0.f2;if(_tmp3AF <= (void*)2)goto _LL29A;if(*((
int*)_tmp3AF)!= 0)goto _LL29A;_tmp3B0=((struct Cyc_Absyn_Upper_b_struct*)_tmp3AF)->f1;
_LL299: if(nv->toplevel)({void*_tmp3D4[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3D5="can't coerce t? to t* or t@ at the top-level";
_tag_dynforward(_tmp3D5,sizeof(char),_get_zero_arr_size(_tmp3D5,45));}),
_tag_dynforward(_tmp3D4,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp3D6=
_tmp3B0;if(_tmp39D  && !_tmp39E)_tmp3D6=Cyc_Absyn_add_exp(_tmp3B0,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp3D7=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e:
Cyc_Toc__untag_dyneither_ptr_e;struct Cyc_Absyn_Exp*_tmp3D8=Cyc_Absyn_fncall_exp(
_tmp3D7,({struct Cyc_Absyn_Exp*_tmp3DA[3];_tmp3DA[2]=_tmp3D6;_tmp3DA[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp393.elt_typ),0);_tmp3DA[0]=_tmp2FF;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3DA,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp39A)(void*)(_tmp3D8->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->hd=Cyc_Absyn_copy_exp(
_tmp3D8);_tmp3D9->tl=0;_tmp3D9;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2FE,_tmp3D8))->r));goto _LL28F;}}_LL29A: _tmp3B1=_tmp3A0.f1;if((int)_tmp3B1 != 
0)goto _LL29C;_tmp3B2=_tmp3A0.f2;if((int)_tmp3B2 != 0)goto _LL29C;_LL29B:
p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL29C: _tmp3B3=_tmp3A0.f1;if((
int)_tmp3B3 != 0)goto _LL29E;_tmp3B4=_tmp3A0.f2;if((int)_tmp3B4 != 1)goto _LL29E;
_LL29D: p1_forward_only=1;goto DynCast;_LL29E: _tmp3B5=_tmp3A0.f1;if((int)_tmp3B5 != 
1)goto _LL2A0;_tmp3B6=_tmp3A0.f2;if((int)_tmp3B6 != 0)goto _LL2A0;_LL29F:
p2_forward_only=1;goto DynCast;_LL2A0: _tmp3B7=_tmp3A0.f1;if((int)_tmp3B7 != 1)goto
_LL2A2;_tmp3B8=_tmp3A0.f2;if((int)_tmp3B8 != 1)goto _LL2A2;_LL2A1: DynCast: if((
_tmp39D  && !_tmp39E) && !(_tmp395.elt_tq).real_const){if(nv->toplevel)({void*
_tmp3DB[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp3DC="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dynforward(_tmp3DC,sizeof(char),_get_zero_arr_size(_tmp3DC,70));}),
_tag_dynforward(_tmp3DB,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp3DD=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;(
void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(_tmp3DD,({struct Cyc_Absyn_Exp*
_tmp3DE[3];_tmp3DE[2]=Cyc_Absyn_uint_exp(1,0);_tmp3DE[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp393.elt_typ),0);_tmp3DE[0]=_tmp2FF;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3DE,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));if(p1_forward_only != p2_forward_only){if(p1_forward_only)(void*)(
e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_to_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp3DF[1];_tmp3DF[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3DF,sizeof(struct Cyc_Absyn_Exp*),
1));}),0))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp3E0[1];_tmp3E0[0]=Cyc_Absyn_copy_exp(e);((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3E0,sizeof(struct Cyc_Absyn_Exp*),
1));}),0))->r));}}}}else{if(p1_forward_only != p2_forward_only){if(nv->toplevel)({
void*_tmp3E1[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(({const char*_tmp3E2="can't coerce between ? and ?+- at toplevel";
_tag_dynforward(_tmp3E2,sizeof(char),_get_zero_arr_size(_tmp3E2,43));}),
_tag_dynforward(_tmp3E1,sizeof(void*),0));});if(p1_forward_only)(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__dynforward_to_dyneither_e,({struct
Cyc_Absyn_Exp*_tmp3E3[1];_tmp3E3[0]=_tmp2FF;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3E3,sizeof(struct Cyc_Absyn_Exp*),
1));}),0))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_to_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp3E4[1];_tmp3E4[0]=_tmp2FF;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3E4,sizeof(struct Cyc_Absyn_Exp*),
1));}),0))->r));}}}goto _LL28F;_LL2A2: _tmp3B9=_tmp3A0.f1;if(_tmp3B9 <= (void*)2)
goto _LL2A4;if(*((int*)_tmp3B9)!= 1)goto _LL2A4;_tmp3BA=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp3B9)->f1;_tmp3BB=_tmp3A0.f2;if(_tmp3BB <= (void*)2)goto _LL2A4;if(*((int*)
_tmp3BB)!= 1)goto _LL2A4;_tmp3BC=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp3BB)->f1;_LL2A3: if(_tmp399  && !_tmp39A){if(nv->toplevel)({void*_tmp3E5[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp3E6="can't do NULL-check conversion at top-level";_tag_dynforward(
_tmp3E6,sizeof(char),_get_zero_arr_size(_tmp3E6,44));}),_tag_dynforward(_tmp3E5,
sizeof(void*),0));});if(_tmp301 != (void*)2)({struct Cyc_String_pa_struct _tmp3E9;
_tmp3E9.tag=0;_tmp3E9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp3E7[1]={& _tmp3E9};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3E8="null-check conversion mis-classified: %s";
_tag_dynforward(_tmp3E8,sizeof(char),_get_zero_arr_size(_tmp3E8,41));}),
_tag_dynforward(_tmp3E7,sizeof(void*),1));}});{int do_null_check=Cyc_Toc_need_null_check(
_tmp2FF);if(do_null_check){if(!_tmp300)({void*_tmp3EA[0]={};Cyc_Tcutil_warn(e->loc,({
const char*_tmp3EB="inserted null check due to implicit cast from * to @ type";
_tag_dynforward(_tmp3EB,sizeof(char),_get_zero_arr_size(_tmp3EB,58));}),
_tag_dynforward(_tmp3EA,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(*
_tmp2FE,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp3EC=
_cycalloc(sizeof(*_tmp3EC));_tmp3EC->hd=_tmp2FF;_tmp3EC->tl=0;_tmp3EC;}),0)))->r));}}}
goto _LL28F;_LL2A4: _tmp3BD=_tmp3A0.f1;if(_tmp3BD <= (void*)2)goto _LL2A6;if(*((int*)
_tmp3BD)!= 1)goto _LL2A6;_LL2A5:({struct Cyc_String_pa_struct _tmp3EF;_tmp3EF.tag=0;
_tmp3EF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp2FF->loc));{void*_tmp3ED[1]={& _tmp3EF};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3EE="%s: toc, cast from AbsUpper_b";
_tag_dynforward(_tmp3EE,sizeof(char),_get_zero_arr_size(_tmp3EE,30));}),
_tag_dynforward(_tmp3ED,sizeof(void*),1));}});_LL2A6: _tmp3BE=_tmp3A0.f2;if(
_tmp3BE <= (void*)2)goto _LL28F;if(*((int*)_tmp3BE)!= 1)goto _LL28F;_LL2A7: goto
_LL28F;_LL28F:;}goto _LL288;}_LL28B: _tmp396=_tmp391.f1;if(_tmp396 <= (void*)4)goto
_LL28D;if(*((int*)_tmp396)!= 4)goto _LL28D;_tmp397=((struct Cyc_Absyn_PointerType_struct*)
_tmp396)->f1;_tmp398=_tmp391.f2;if(_tmp398 <= (void*)4)goto _LL28D;if(*((int*)
_tmp398)!= 5)goto _LL28D;_LL28C:{void*_tmp3F0=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp397.ptr_atts).bounds);_LL2A9: if((int)_tmp3F0 != 0)goto _LL2AB;_LL2AA: goto
_LL2AC;_LL2AB: if((int)_tmp3F0 != 1)goto _LL2AD;_LL2AC:(void*)(_tmp2FF->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)_tmp2FF->r,_tmp2FF->loc),
Cyc_Toc_curr_sp,0))->r));goto _LL2A8;_LL2AD:;_LL2AE: goto _LL2A8;_LL2A8:;}goto
_LL288;_LL28D:;_LL28E: goto _LL288;_LL288:;}goto _LL1E3;}_LL208: if(*((int*)_tmp2D8)
!= 16)goto _LL20A;_tmp302=((struct Cyc_Absyn_Address_e_struct*)_tmp2D8)->f1;_LL209:{
void*_tmp3F1=(void*)_tmp302->r;struct _tuple1*_tmp3F2;struct Cyc_List_List*_tmp3F3;
struct Cyc_List_List*_tmp3F4;struct Cyc_List_List*_tmp3F5;_LL2B0: if(*((int*)
_tmp3F1)!= 30)goto _LL2B2;_tmp3F2=((struct Cyc_Absyn_Struct_e_struct*)_tmp3F1)->f1;
_tmp3F3=((struct Cyc_Absyn_Struct_e_struct*)_tmp3F1)->f2;_tmp3F4=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3F1)->f3;_LL2B1: if(nv->toplevel)({struct Cyc_String_pa_struct _tmp3F8;_tmp3F8.tag=
0;_tmp3F8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp302->loc));{void*_tmp3F6[1]={& _tmp3F8};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3F7="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp3F7,sizeof(char),_get_zero_arr_size(_tmp3F7,42));}),
_tag_dynforward(_tmp3F6,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp302->topt))->v,_tmp3F3 != 0,1,0,
_tmp3F4,_tmp3F2))->r));goto _LL2AF;_LL2B2: if(*((int*)_tmp3F1)!= 26)goto _LL2B4;
_tmp3F5=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3F1)->f1;_LL2B3: if(nv->toplevel)({
struct Cyc_String_pa_struct _tmp3FB;_tmp3FB.tag=0;_tmp3FB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp302->loc));{
void*_tmp3F9[1]={& _tmp3FB};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3FA="%s: & on non-identifiers at the top-level";
_tag_dynforward(_tmp3FA,sizeof(char),_get_zero_arr_size(_tmp3FA,42));}),
_tag_dynforward(_tmp3F9,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp3F5))->r));goto _LL2AF;_LL2B4:;_LL2B5: Cyc_Toc_exp_to_c(nv,_tmp302);if(
!Cyc_Absyn_is_lvalue(_tmp302)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp302,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp302))->r));}
goto _LL2AF;_LL2AF:;}goto _LL1E3;_LL20A: if(*((int*)_tmp2D8)!= 17)goto _LL20C;
_tmp303=((struct Cyc_Absyn_New_e_struct*)_tmp2D8)->f1;_tmp304=((struct Cyc_Absyn_New_e_struct*)
_tmp2D8)->f2;_LL20B: if(nv->toplevel)({struct Cyc_String_pa_struct _tmp3FE;_tmp3FE.tag=
0;_tmp3FE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp304->loc));{void*_tmp3FC[1]={& _tmp3FE};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp3FD="%s: new at top-level";
_tag_dynforward(_tmp3FD,sizeof(char),_get_zero_arr_size(_tmp3FD,21));}),
_tag_dynforward(_tmp3FC,sizeof(void*),1));}});{void*_tmp3FF=(void*)_tmp304->r;
struct Cyc_List_List*_tmp400;struct Cyc_Absyn_Vardecl*_tmp401;struct Cyc_Absyn_Exp*
_tmp402;struct Cyc_Absyn_Exp*_tmp403;int _tmp404;struct _tuple1*_tmp405;struct Cyc_List_List*
_tmp406;struct Cyc_List_List*_tmp407;struct Cyc_List_List*_tmp408;_LL2B7: if(*((int*)
_tmp3FF)!= 28)goto _LL2B9;_tmp400=((struct Cyc_Absyn_Array_e_struct*)_tmp3FF)->f1;
_LL2B8: {struct _tuple1*_tmp409=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp40A=
Cyc_Absyn_var_exp(_tmp409,0);struct Cyc_Absyn_Stmt*_tmp40B=Cyc_Toc_init_array(nv,
_tmp40A,_tmp400,Cyc_Absyn_exp_stmt(_tmp40A,0));void*old_elt_typ;{void*_tmp40C=
Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp40D;void*_tmp40E;struct
Cyc_Absyn_Tqual _tmp40F;struct Cyc_Absyn_PtrAtts _tmp410;struct Cyc_Absyn_Conref*
_tmp411;_LL2C2: if(_tmp40C <= (void*)4)goto _LL2C4;if(*((int*)_tmp40C)!= 4)goto
_LL2C4;_tmp40D=((struct Cyc_Absyn_PointerType_struct*)_tmp40C)->f1;_tmp40E=(void*)
_tmp40D.elt_typ;_tmp40F=_tmp40D.elt_tq;_tmp410=_tmp40D.ptr_atts;_tmp411=_tmp410.zero_term;
_LL2C3: old_elt_typ=_tmp40E;goto _LL2C1;_LL2C4:;_LL2C5: old_elt_typ=({void*_tmp412[
0]={};Cyc_Toc_toc_impos(({const char*_tmp413="exp_to_c:new array expression doesn't have ptr type";
_tag_dynforward(_tmp413,sizeof(char),_get_zero_arr_size(_tmp413,52));}),
_tag_dynforward(_tmp412,sizeof(void*),0));});_LL2C1:;}{void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);void*_tmp414=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp415=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp400),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp303 == 0)e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp415);else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp303;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,
_tmp415);}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp409,_tmp414,(struct Cyc_Absyn_Exp*)e1,_tmp40B,0),0))->r));goto _LL2B6;}}_LL2B9:
if(*((int*)_tmp3FF)!= 29)goto _LL2BB;_tmp401=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3FF)->f1;_tmp402=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3FF)->f2;
_tmp403=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3FF)->f3;_tmp404=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp3FF)->f4;_LL2BA: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp416=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp417;void*_tmp418;struct Cyc_Absyn_Tqual _tmp419;struct Cyc_Absyn_PtrAtts _tmp41A;
struct Cyc_Absyn_Conref*_tmp41B;struct Cyc_Absyn_Conref*_tmp41C;_LL2C7: if(_tmp416
<= (void*)4)goto _LL2C9;if(*((int*)_tmp416)!= 4)goto _LL2C9;_tmp417=((struct Cyc_Absyn_PointerType_struct*)
_tmp416)->f1;_tmp418=(void*)_tmp417.elt_typ;_tmp419=_tmp417.elt_tq;_tmp41A=
_tmp417.ptr_atts;_tmp41B=_tmp41A.bounds;_tmp41C=_tmp41A.zero_term;_LL2C8:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp41B)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp41B)== (void*)1;
goto _LL2C6;_LL2C9:;_LL2CA:({void*_tmp41D[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp41E="exp_to_c: comprehension not an array type";
_tag_dynforward(_tmp41E,sizeof(char),_get_zero_arr_size(_tmp41E,42));}),
_tag_dynforward(_tmp41D,sizeof(void*),0));});_LL2C6:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp403->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp402);{
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_var_exp(max,0);if(_tmp404)_tmp41F=Cyc_Absyn_add_exp(
_tmp41F,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp401,Cyc_Absyn_var_exp(max,0),_tmp403,_tmp404,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp420=_new_region("r");struct _RegionHandle*r=& _tmp420;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp431=
_region_malloc(r,sizeof(*_tmp431));_tmp431->hd=({struct _tuple10*_tmp432=
_region_malloc(r,sizeof(*_tmp432));_tmp432->f1=max;_tmp432->f2=Cyc_Absyn_uint_typ;
_tmp432->f3=(struct Cyc_Absyn_Exp*)_tmp402;_tmp432;});_tmp431->tl=0;_tmp431;});
struct Cyc_Absyn_Exp*ai;if(_tmp303 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp421[2];_tmp421[1]=_tmp41F;
_tmp421[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp421,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp303;Cyc_Toc_exp_to_c(
nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp422[2];_tmp422[1]=_tmp41F;_tmp422[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp422,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({struct Cyc_List_List*_tmp423=
_region_malloc(r,sizeof(*_tmp423));_tmp423->hd=({struct _tuple10*_tmp424=
_region_malloc(r,sizeof(*_tmp424));_tmp424->f1=a;_tmp424->f2=ptr_typ;_tmp424->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp424;});_tmp423->tl=decls;_tmp423;});if(
is_dynforward_ptr  || is_dyneither_ptr){struct _tuple1*_tmp425=Cyc_Toc_temp_var();
void*_tmp426=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp427=
is_dynforward_ptr?Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*
_tmp428=Cyc_Absyn_fncall_exp(_tmp427,({struct Cyc_Absyn_Exp*_tmp42B[3];_tmp42B[2]=
_tmp41F;_tmp42B[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp42B[0]=Cyc_Absyn_var_exp(
a,0);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp42B,sizeof(struct Cyc_Absyn_Exp*),3));}),0);decls=({struct Cyc_List_List*
_tmp429=_region_malloc(r,sizeof(*_tmp429));_tmp429->hd=({struct _tuple10*_tmp42A=
_region_malloc(r,sizeof(*_tmp42A));_tmp42A->f1=_tmp425;_tmp42A->f2=_tmp426;
_tmp42A->f3=(struct Cyc_Absyn_Exp*)_tmp428;_tmp42A;});_tmp429->tl=decls;_tmp429;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp425,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp42C=decls;for(0;_tmp42C != 0;_tmp42C=_tmp42C->tl){struct
_tuple1*_tmp42E;void*_tmp42F;struct Cyc_Absyn_Exp*_tmp430;struct _tuple10 _tmp42D=*((
struct _tuple10*)_tmp42C->hd);_tmp42E=_tmp42D.f1;_tmp42F=_tmp42D.f2;_tmp430=
_tmp42D.f3;s=Cyc_Absyn_declare_stmt(_tmp42E,_tmp42F,_tmp430,s,0);}}(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL2B6;}}}}
_LL2BB: if(*((int*)_tmp3FF)!= 30)goto _LL2BD;_tmp405=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3FF)->f1;_tmp406=((struct Cyc_Absyn_Struct_e_struct*)_tmp3FF)->f2;_tmp407=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3FF)->f3;_LL2BC:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp304->topt))->v,
_tmp406 != 0,1,_tmp303,_tmp407,_tmp405))->r));goto _LL2B6;_LL2BD: if(*((int*)
_tmp3FF)!= 26)goto _LL2BF;_tmp408=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3FF)->f1;
_LL2BE:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp303,_tmp408))->r));
goto _LL2B6;_LL2BF:;_LL2C0: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp304->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp433=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp434=Cyc_Absyn_var_exp(
_tmp433,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp434,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp303 == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp303;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*
_tmp435=(void*)_tmp304->r;void*_tmp436;struct Cyc_Absyn_Exp*_tmp437;_LL2CC: if(*((
int*)_tmp435)!= 15)goto _LL2CE;_tmp436=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp435)->f1;_tmp437=((struct Cyc_Absyn_Cast_e_struct*)_tmp435)->f2;_LL2CD:{
struct _tuple0 _tmp439=({struct _tuple0 _tmp438;_tmp438.f1=Cyc_Tcutil_compress(
_tmp436);_tmp438.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp437->topt))->v);_tmp438;});void*_tmp43A;struct Cyc_Absyn_PtrInfo _tmp43B;void*
_tmp43C;struct Cyc_Absyn_PtrAtts _tmp43D;struct Cyc_Absyn_Conref*_tmp43E;void*
_tmp43F;struct Cyc_Absyn_PtrInfo _tmp440;struct Cyc_Absyn_PtrAtts _tmp441;struct Cyc_Absyn_Conref*
_tmp442;_LL2D1: _tmp43A=_tmp439.f1;if(_tmp43A <= (void*)4)goto _LL2D3;if(*((int*)
_tmp43A)!= 4)goto _LL2D3;_tmp43B=((struct Cyc_Absyn_PointerType_struct*)_tmp43A)->f1;
_tmp43C=(void*)_tmp43B.elt_typ;_tmp43D=_tmp43B.ptr_atts;_tmp43E=_tmp43D.bounds;
_tmp43F=_tmp439.f2;if(_tmp43F <= (void*)4)goto _LL2D3;if(*((int*)_tmp43F)!= 4)goto
_LL2D3;_tmp440=((struct Cyc_Absyn_PointerType_struct*)_tmp43F)->f1;_tmp441=
_tmp440.ptr_atts;_tmp442=_tmp441.bounds;_LL2D2:{struct _tuple0 _tmp444=({struct
_tuple0 _tmp443;_tmp443.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp43E);
_tmp443.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp442);_tmp443;});void*
_tmp445;void*_tmp446;struct Cyc_Absyn_Exp*_tmp447;void*_tmp448;void*_tmp449;
struct Cyc_Absyn_Exp*_tmp44A;_LL2D6: _tmp445=_tmp444.f1;if((int)_tmp445 != 0)goto
_LL2D8;_tmp446=_tmp444.f2;if(_tmp446 <= (void*)2)goto _LL2D8;if(*((int*)_tmp446)!= 
0)goto _LL2D8;_tmp447=((struct Cyc_Absyn_Upper_b_struct*)_tmp446)->f1;_LL2D7:
forward_only=1;_tmp44A=_tmp447;goto _LL2D9;_LL2D8: _tmp448=_tmp444.f1;if((int)
_tmp448 != 1)goto _LL2DA;_tmp449=_tmp444.f2;if(_tmp449 <= (void*)2)goto _LL2DA;if(*((
int*)_tmp449)!= 0)goto _LL2DA;_tmp44A=((struct Cyc_Absyn_Upper_b_struct*)_tmp449)->f1;
_LL2D9: Cyc_Toc_exp_to_c(nv,_tmp437);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;{struct Cyc_Absyn_Exp*_tmp44B=forward_only?Cyc_Toc__init_dynforward_ptr_e:
Cyc_Toc__init_dyneither_ptr_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
_tmp44B,({struct Cyc_Absyn_Exp*_tmp44C[4];_tmp44C[3]=_tmp44A;_tmp44C[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp43C),0);_tmp44C[1]=_tmp437;_tmp44C[0]=mexp;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp44C,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL2D5;}_LL2DA:;_LL2DB: goto _LL2D5;_LL2D5:;}goto _LL2D0;_LL2D3:;
_LL2D4: goto _LL2D0;_LL2D0:;}goto _LL2CB;_LL2CE:;_LL2CF: goto _LL2CB;_LL2CB:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp44D=Cyc_Absyn_exp_stmt(_tmp434,0);struct Cyc_Absyn_Exp*
_tmp44E=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp304);_tmp44D=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp434,_tmp44E,0),_tmp304,0),
_tmp44D,0);{void*_tmp44F=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp433,_tmp44F,(struct
Cyc_Absyn_Exp*)mexp,_tmp44D,0),0))->r));}}goto _LL2B6;}}_LL2B6:;}goto _LL1E3;
_LL20C: if(*((int*)_tmp2D8)!= 19)goto _LL20E;_tmp305=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp2D8)->f1;_LL20D: Cyc_Toc_exp_to_c(nv,_tmp305);goto _LL1E3;_LL20E: if(*((int*)
_tmp2D8)!= 18)goto _LL210;_tmp306=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2D8)->f1;_LL20F:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp451;_tmp451.tag=18;_tmp451.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp306);_tmp451;});
_tmp450;})));goto _LL1E3;_LL210: if(*((int*)_tmp2D8)!= 21)goto _LL212;_LL211:({void*
_tmp452[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp453="__gen() in code generator";_tag_dynforward(_tmp453,sizeof(
char),_get_zero_arr_size(_tmp453,26));}),_tag_dynforward(_tmp452,sizeof(void*),0));});
_LL212: if(*((int*)_tmp2D8)!= 20)goto _LL214;_tmp307=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2D8)->f1;_tmp308=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2D8)->f2;
if(*((int*)_tmp308)!= 0)goto _LL214;_tmp309=((struct Cyc_Absyn_StructField_struct*)
_tmp308)->f1;_LL213:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp455;_tmp455.tag=20;_tmp455.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp307);_tmp455.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp456=_cycalloc(sizeof(*
_tmp456));_tmp456[0]=({struct Cyc_Absyn_StructField_struct _tmp457;_tmp457.tag=0;
_tmp457.f1=_tmp309;_tmp457;});_tmp456;}));_tmp455;});_tmp454;})));goto _LL1E3;
_LL214: if(*((int*)_tmp2D8)!= 20)goto _LL216;_tmp30A=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp2D8)->f1;_tmp30B=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp2D8)->f2;
if(*((int*)_tmp30B)!= 1)goto _LL216;_tmp30C=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp30B)->f1;_LL215:{void*_tmp458=Cyc_Tcutil_compress(_tmp30A);struct Cyc_Absyn_AggrInfo
_tmp459;void*_tmp45A;struct Cyc_List_List*_tmp45B;_LL2DD: if(_tmp458 <= (void*)4)
goto _LL2E5;if(*((int*)_tmp458)!= 10)goto _LL2DF;_tmp459=((struct Cyc_Absyn_AggrType_struct*)
_tmp458)->f1;_tmp45A=(void*)_tmp459.aggr_info;_LL2DE: {struct Cyc_Absyn_Aggrdecl*
_tmp45C=Cyc_Absyn_get_known_aggrdecl(_tmp45A);if(_tmp45C->impl == 0)({void*
_tmp45D[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp45E="struct fields must be known";_tag_dynforward(_tmp45E,sizeof(
char),_get_zero_arr_size(_tmp45E,28));}),_tag_dynforward(_tmp45D,sizeof(void*),0));});
_tmp45B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp45C->impl))->fields;goto
_LL2E0;}_LL2DF: if(*((int*)_tmp458)!= 11)goto _LL2E1;_tmp45B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp458)->f2;_LL2E0: {struct Cyc_Absyn_Aggrfield*_tmp45F=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp45B,(int)_tmp30C);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp460=_cycalloc(sizeof(*
_tmp460));_tmp460[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp461;_tmp461.tag=20;
_tmp461.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp30A);_tmp461.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462[0]=({
struct Cyc_Absyn_StructField_struct _tmp463;_tmp463.tag=0;_tmp463.f1=_tmp45F->name;
_tmp463;});_tmp462;}));_tmp461;});_tmp460;})));goto _LL2DC;}_LL2E1: if(*((int*)
_tmp458)!= 9)goto _LL2E3;_LL2E2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp465;_tmp465.tag=20;_tmp465.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp30A);_tmp465.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp466=_cycalloc(sizeof(*
_tmp466));_tmp466[0]=({struct Cyc_Absyn_StructField_struct _tmp467;_tmp467.tag=0;
_tmp467.f1=Cyc_Absyn_fieldname((int)(_tmp30C + 1));_tmp467;});_tmp466;}));_tmp465;});
_tmp464;})));goto _LL2DC;_LL2E3: if(*((int*)_tmp458)!= 3)goto _LL2E5;_LL2E4: if(
_tmp30C == 0)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp469;_tmp469.tag=20;_tmp469.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp30A);_tmp469.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp46A=_cycalloc(sizeof(*
_tmp46A));_tmp46A[0]=({struct Cyc_Absyn_StructField_struct _tmp46B;_tmp46B.tag=0;
_tmp46B.f1=Cyc_Toc_tag_sp;_tmp46B;});_tmp46A;}));_tmp469;});_tmp468;})));else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp46C=_cycalloc(
sizeof(*_tmp46C));_tmp46C[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp46D;_tmp46D.tag=
20;_tmp46D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp30A);_tmp46D.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E[0]=({
struct Cyc_Absyn_StructField_struct _tmp46F;_tmp46F.tag=0;_tmp46F.f1=Cyc_Absyn_fieldname((
int)_tmp30C);_tmp46F;});_tmp46E;}));_tmp46D;});_tmp46C;})));}goto _LL2DC;_LL2E5:;
_LL2E6:({void*_tmp470[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp471="impossible type for offsetof tuple index";
_tag_dynforward(_tmp471,sizeof(char),_get_zero_arr_size(_tmp471,41));}),
_tag_dynforward(_tmp470,sizeof(void*),0));});_LL2DC:;}goto _LL1E3;_LL216: if(*((
int*)_tmp2D8)!= 22)goto _LL218;_tmp30D=((struct Cyc_Absyn_Deref_e_struct*)_tmp2D8)->f1;
_LL217: {void*_tmp472=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp30D->topt))->v);{void*_tmp473=_tmp472;struct Cyc_Absyn_PtrInfo
_tmp474;void*_tmp475;struct Cyc_Absyn_Tqual _tmp476;struct Cyc_Absyn_PtrAtts _tmp477;
void*_tmp478;struct Cyc_Absyn_Conref*_tmp479;struct Cyc_Absyn_Conref*_tmp47A;
struct Cyc_Absyn_Conref*_tmp47B;_LL2E8: if(_tmp473 <= (void*)4)goto _LL2EA;if(*((int*)
_tmp473)!= 4)goto _LL2EA;_tmp474=((struct Cyc_Absyn_PointerType_struct*)_tmp473)->f1;
_tmp475=(void*)_tmp474.elt_typ;_tmp476=_tmp474.elt_tq;_tmp477=_tmp474.ptr_atts;
_tmp478=(void*)_tmp477.rgn;_tmp479=_tmp477.nullable;_tmp47A=_tmp477.bounds;
_tmp47B=_tmp477.zero_term;_LL2E9:{void*_tmp47C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp47A);_LL2ED: if(_tmp47C <= (void*)2)goto _LL2EF;if(*((int*)_tmp47C)!= 0)goto
_LL2EF;_LL2EE: {int do_null_check=Cyc_Toc_need_null_check(_tmp30D);Cyc_Toc_exp_to_c(
nv,_tmp30D);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp47D[0]={};
Cyc_Tcutil_warn(e->loc,({const char*_tmp47E="inserted null check due to dereference";
_tag_dynforward(_tmp47E,sizeof(char),_get_zero_arr_size(_tmp47E,39));}),
_tag_dynforward(_tmp47D,sizeof(void*),0));});(void*)(_tmp30D->r=(void*)((void*)(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(_tmp472),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->hd=Cyc_Absyn_copy_exp(
_tmp30D);_tmp47F->tl=0;_tmp47F;}),0)))->r));}goto _LL2EC;}_LL2EF: if((int)_tmp47C
!= 0)goto _LL2F1;_LL2F0: goto _LL2F2;_LL2F1: if((int)_tmp47C != 1)goto _LL2F3;_LL2F2: {
struct Cyc_Absyn_Exp*_tmp480=Cyc_Absyn_uint_exp(0,0);_tmp480->topt=({struct Cyc_Core_Opt*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->v=(void*)Cyc_Absyn_uint_typ;_tmp481;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp30D,_tmp480,0))->r));Cyc_Toc_exp_to_c(
nv,e);goto _LL2EC;}_LL2F3: if(_tmp47C <= (void*)2)goto _LL2EC;if(*((int*)_tmp47C)!= 
1)goto _LL2EC;_LL2F4:({void*_tmp482[0]={};((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp483="exp_to_c: deref w/ AbsUpper_b";
_tag_dynforward(_tmp483,sizeof(char),_get_zero_arr_size(_tmp483,30));}),
_tag_dynforward(_tmp482,sizeof(void*),0));});_LL2EC:;}goto _LL2E7;_LL2EA:;_LL2EB:({
void*_tmp484[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp485="exp_to_c: Deref: non-pointer";
_tag_dynforward(_tmp485,sizeof(char),_get_zero_arr_size(_tmp485,29));}),
_tag_dynforward(_tmp484,sizeof(void*),0));});_LL2E7:;}goto _LL1E3;}_LL218: if(*((
int*)_tmp2D8)!= 23)goto _LL21A;_tmp30E=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2D8)->f1;_tmp30F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp2D8)->f2;_LL219:
Cyc_Toc_exp_to_c(nv,_tmp30E);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E3;
_LL21A: if(*((int*)_tmp2D8)!= 24)goto _LL21C;_tmp310=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp2D8)->f1;_tmp311=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2D8)->f2;_LL21B: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp310->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp310);Cyc_Toc_exp_to_c(nv,_tmp310);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp487;struct Cyc_Absyn_Tqual
_tmp488;struct Cyc_Absyn_PtrAtts _tmp489;void*_tmp48A;struct Cyc_Absyn_Conref*
_tmp48B;struct Cyc_Absyn_Conref*_tmp48C;struct Cyc_Absyn_Conref*_tmp48D;struct Cyc_Absyn_PtrInfo
_tmp486=Cyc_Toc_get_ptr_type(e1typ);_tmp487=(void*)_tmp486.elt_typ;_tmp488=
_tmp486.elt_tq;_tmp489=_tmp486.ptr_atts;_tmp48A=(void*)_tmp489.rgn;_tmp48B=
_tmp489.nullable;_tmp48C=_tmp489.bounds;_tmp48D=_tmp489.zero_term;{void*_tmp48E=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp48C);struct Cyc_Absyn_Exp*_tmp48F;
_LL2F6: if(_tmp48E <= (void*)2)goto _LL2F8;if(*((int*)_tmp48E)!= 0)goto _LL2F8;
_tmp48F=((struct Cyc_Absyn_Upper_b_struct*)_tmp48E)->f1;_LL2F7: {unsigned int
_tmp491;int _tmp492;struct _tuple5 _tmp490=Cyc_Evexp_eval_const_uint_exp(_tmp48F);
_tmp491=_tmp490.f1;_tmp492=_tmp490.f2;if(_tmp492){if(_tmp491 < 1)({void*_tmp493[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp494="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dynforward(
_tmp494,sizeof(char),_get_zero_arr_size(_tmp494,44));}),_tag_dynforward(_tmp493,
sizeof(void*),0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*
_tmp495[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp496="inserted null check due to dereference";
_tag_dynforward(_tmp496,sizeof(char),_get_zero_arr_size(_tmp496,39));}),
_tag_dynforward(_tmp495,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp310->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->hd=_tmp310;_tmp497->tl=0;_tmp497;}),
0)),_tmp311,0))->r));}}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp310->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp498[4];_tmp498[3]=Cyc_Absyn_uint_exp(0,0);_tmp498[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp487,0);_tmp498[1]=_tmp48F;_tmp498[0]=_tmp310;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp498,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),_tmp311,0))->r));}goto _LL2F5;}_LL2F8: if((int)_tmp48E != 0)goto _LL2FA;
_LL2F9: forward_only=1;goto _LL2FB;_LL2FA: if((int)_tmp48E != 1)goto _LL2FC;_LL2FB: {
struct Cyc_Absyn_Exp*_tmp499=forward_only?Cyc_Toc__check_dynforward_subscript_e:
Cyc_Toc__check_dyneither_subscript_e;void*ta1=Cyc_Toc_typ_to_c_array(_tmp487);(
void*)(_tmp310->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp488),Cyc_Absyn_fncall_exp(_tmp499,({struct Cyc_Absyn_Exp*_tmp49A[3];_tmp49A[2]=
Cyc_Absyn_uint_exp(0,0);_tmp49A[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp49A[0]=Cyc_Absyn_copy_exp(
_tmp310);((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp49A,sizeof(struct Cyc_Absyn_Exp*),3));}),0)))->r));goto _LL2F5;}
_LL2FC: if(_tmp48E <= (void*)2)goto _LL2F5;if(*((int*)_tmp48E)!= 1)goto _LL2F5;
_LL2FD:({void*_tmp49B[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(({const char*_tmp49C="exp_to_c: AggrArrow w/ AbsUpper_b";
_tag_dynforward(_tmp49C,sizeof(char),_get_zero_arr_size(_tmp49C,34));}),
_tag_dynforward(_tmp49B,sizeof(void*),0));});_LL2F5:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E3;}}
_LL21C: if(*((int*)_tmp2D8)!= 25)goto _LL21E;_tmp312=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp2D8)->f1;_tmp313=((struct Cyc_Absyn_Subscript_e_struct*)_tmp2D8)->f2;_LL21D: {
void*_tmp49D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp312->topt))->v);{void*_tmp49E=_tmp49D;struct Cyc_List_List*_tmp49F;struct Cyc_Absyn_PtrInfo
_tmp4A0;void*_tmp4A1;struct Cyc_Absyn_Tqual _tmp4A2;struct Cyc_Absyn_PtrAtts _tmp4A3;
void*_tmp4A4;struct Cyc_Absyn_Conref*_tmp4A5;struct Cyc_Absyn_Conref*_tmp4A6;
struct Cyc_Absyn_Conref*_tmp4A7;_LL2FF: if(_tmp49E <= (void*)4)goto _LL303;if(*((int*)
_tmp49E)!= 9)goto _LL301;_tmp49F=((struct Cyc_Absyn_TupleType_struct*)_tmp49E)->f1;
_LL300: Cyc_Toc_exp_to_c(nv,_tmp312);Cyc_Toc_exp_to_c(nv,_tmp313);{unsigned int
_tmp4A9;int _tmp4AA;struct _tuple5 _tmp4A8=Cyc_Evexp_eval_const_uint_exp(_tmp313);
_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;if(!_tmp4AA)({void*_tmp4AB[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4AC="unknown tuple subscript in translation to C";_tag_dynforward(_tmp4AC,
sizeof(char),_get_zero_arr_size(_tmp4AC,44));}),_tag_dynforward(_tmp4AB,sizeof(
void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(_tmp312,Cyc_Absyn_fieldname((
int)(_tmp4A9 + 1)),0))->r));goto _LL2FE;}_LL301: if(*((int*)_tmp49E)!= 4)goto _LL303;
_tmp4A0=((struct Cyc_Absyn_PointerType_struct*)_tmp49E)->f1;_tmp4A1=(void*)
_tmp4A0.elt_typ;_tmp4A2=_tmp4A0.elt_tq;_tmp4A3=_tmp4A0.ptr_atts;_tmp4A4=(void*)
_tmp4A3.rgn;_tmp4A5=_tmp4A3.nullable;_tmp4A6=_tmp4A3.bounds;_tmp4A7=_tmp4A3.zero_term;
_LL302: {struct Cyc_List_List*_tmp4AD=Cyc_Toc_get_relns(_tmp312);int in_bnds=0;{
void*_tmp4AE=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4A6);_LL306: if(
_tmp4AE <= (void*)2)goto _LL308;if(*((int*)_tmp4AE)!= 1)goto _LL308;_LL307: goto
_LL305;_LL308:;_LL309: in_bnds=Cyc_Toc_check_bounds(_tmp4AD,_tmp312,_tmp313);if(
Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp4B1;
_tmp4B1.tag=0;_tmp4B1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp4AF[1]={& _tmp4B1};Cyc_Tcutil_warn(e->loc,({const char*_tmp4B0="bounds check necessary for %s";
_tag_dynforward(_tmp4B0,sizeof(char),_get_zero_arr_size(_tmp4B0,30));}),
_tag_dynforward(_tmp4AF,sizeof(void*),1));}});_LL305:;}Cyc_Toc_exp_to_c(nv,
_tmp312);Cyc_Toc_exp_to_c(nv,_tmp313);Cyc_Toc_total_bounds_checks ++;{int
forward_only=0;{void*_tmp4B2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4A6);
struct Cyc_Absyn_Exp*_tmp4B3;void*_tmp4B4;_LL30B: if(_tmp4B2 <= (void*)2)goto _LL30D;
if(*((int*)_tmp4B2)!= 0)goto _LL30D;_tmp4B3=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4B2)->f1;_LL30C: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp4A5);void*ta1=Cyc_Toc_typ_to_c(_tmp4A1);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp4A2);if(in_bnds)Cyc_Toc_bounds_checks_eliminated ++;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4A7))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__zero_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp4B5[3];_tmp4B5[2]=_tmp313;_tmp4B5[1]=_tmp4B3;_tmp4B5[0]=
_tmp312;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4B5,sizeof(struct Cyc_Absyn_Exp*),3));}),0)),0))->r));else{
if(possibly_null){if(Cyc_Toc_warn_all_null_deref)({void*_tmp4B6[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp4B7="inserted null check due to dereference";
_tag_dynforward(_tmp4B7,sizeof(char),_get_zero_arr_size(_tmp4B7,39));}),
_tag_dynforward(_tmp4B6,sizeof(void*),0));});(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp4B8[4];_tmp4B8[3]=_tmp313;_tmp4B8[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp4B8[1]=_tmp4B3;_tmp4B8[0]=_tmp312;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4B8,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)),0))->r));}else{(void*)(_tmp313->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp4B9[2];
_tmp4B9[1]=Cyc_Absyn_copy_exp(_tmp313);_tmp4B9[0]=_tmp4B3;((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp4B9,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));}}}goto _LL30A;}_LL30D: if((int)_tmp4B2 != 0)goto _LL30F;_LL30E:
forward_only=1;goto _LL310;_LL30F: if((int)_tmp4B2 != 1)goto _LL311;_LL310: {void*
ta1=Cyc_Toc_typ_to_c_array(_tmp4A1);if(in_bnds){Cyc_Toc_bounds_checks_eliminated
++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
ta1,_tmp4A2),Cyc_Absyn_aggrmember_exp(_tmp312,Cyc_Toc_curr_sp,0)),_tmp313,0))->r));}
else{struct Cyc_Absyn_Exp*_tmp4BA=forward_only?Cyc_Toc__check_dynforward_subscript_e:
Cyc_Toc__check_dyneither_subscript_e;(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp4A2),Cyc_Absyn_fncall_exp(_tmp4BA,({
struct Cyc_Absyn_Exp*_tmp4BB[3];_tmp4BB[2]=_tmp313;_tmp4BB[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp4BB[0]=_tmp312;((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp4BB,sizeof(struct Cyc_Absyn_Exp*),3));}),0)),0))->r));}goto
_LL30A;}_LL311: if(_tmp4B2 <= (void*)2)goto _LL30A;if(*((int*)_tmp4B2)!= 1)goto
_LL30A;_tmp4B4=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4B2)->f1;_LL312:{
void*_tmp4BC=(void*)_tmp313->annot;struct Cyc_List_List*_tmp4BD;_LL314: if(*((void**)
_tmp4BC)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL316;_tmp4BD=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)
_tmp4BC)->f1;_LL315: for(0;_tmp4BD != 0;_tmp4BD=_tmp4BD->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp4BD->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp4BD->hd)->bd,_tmp4B4)== 0){in_bnds=1;break;}}goto _LL313;_LL316:;_LL317: goto
_LL313;_LL313:;}if(!in_bnds)({void*_tmp4BE[0]={};Cyc_Tcutil_terr(e->loc,({const
char*_tmp4BF="cannot determine that subscript is in bounds";_tag_dynforward(
_tmp4BF,sizeof(char),_get_zero_arr_size(_tmp4BF,45));}),_tag_dynforward(_tmp4BE,
sizeof(void*),0));});goto _LL30A;_LL30A:;}goto _LL2FE;}}_LL303:;_LL304:({void*
_tmp4C0[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4C1="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dynforward(_tmp4C1,sizeof(char),_get_zero_arr_size(_tmp4C1,49));}),
_tag_dynforward(_tmp4C0,sizeof(void*),0));});_LL2FE:;}goto _LL1E3;}_LL21E: if(*((
int*)_tmp2D8)!= 26)goto _LL220;_tmp314=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2D8)->f1;
_LL21F: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,
_tmp314))->r));else{struct Cyc_List_List*_tmp4C2=((struct Cyc_List_List*(*)(struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp314);struct _dynforward_ptr*_tmp4C3=Cyc_Toc_add_tuple_type(_tmp4C2);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp314 != 0;(_tmp314=_tmp314->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp314->hd);dles=({struct Cyc_List_List*_tmp4C4=_cycalloc(
sizeof(*_tmp4C4));_tmp4C4->hd=({struct _tuple6*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));
_tmp4C5->f1=0;_tmp4C5->f2=(struct Cyc_Absyn_Exp*)_tmp314->hd;_tmp4C5;});_tmp4C4->tl=
dles;_tmp4C4;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL1E3;_LL220: if(*((int*)_tmp2D8)!= 28)goto _LL222;_tmp315=((struct Cyc_Absyn_Array_e_struct*)
_tmp2D8)->f1;_LL221:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp315,0))->r));{struct Cyc_List_List*_tmp4C6=_tmp315;for(0;_tmp4C6 != 0;_tmp4C6=
_tmp4C6->tl){struct _tuple6 _tmp4C8;struct Cyc_Absyn_Exp*_tmp4C9;struct _tuple6*
_tmp4C7=(struct _tuple6*)_tmp4C6->hd;_tmp4C8=*_tmp4C7;_tmp4C9=_tmp4C8.f2;Cyc_Toc_exp_to_c(
nv,_tmp4C9);}}goto _LL1E3;_LL222: if(*((int*)_tmp2D8)!= 29)goto _LL224;_tmp316=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D8)->f1;_tmp317=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2D8)->f2;_tmp318=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D8)->f3;
_tmp319=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D8)->f4;_LL223: {
unsigned int _tmp4CB;int _tmp4CC;struct _tuple5 _tmp4CA=Cyc_Evexp_eval_const_uint_exp(
_tmp317);_tmp4CB=_tmp4CA.f1;_tmp4CC=_tmp4CA.f2;{void*_tmp4CD=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp318->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp318);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp318)){if(!_tmp4CC)({
void*_tmp4CE[0]={};Cyc_Tcutil_terr(_tmp317->loc,({const char*_tmp4CF="cannot determine value of constant";
_tag_dynforward(_tmp4CF,sizeof(char),_get_zero_arr_size(_tmp4CF,35));}),
_tag_dynforward(_tmp4CE,sizeof(void*),0));});{unsigned int i=0;for(0;i < _tmp4CB;i
++){es=({struct Cyc_List_List*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->hd=({
struct _tuple6*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1->f1=0;_tmp4D1->f2=
_tmp318;_tmp4D1;});_tmp4D0->tl=es;_tmp4D0;});}}if(_tmp319){struct Cyc_Absyn_Exp*
_tmp4D2=Cyc_Toc_cast_it(_tmp4CD,Cyc_Absyn_uint_exp(0,0));es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*
_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3->hd=({struct _tuple6*_tmp4D4=
_cycalloc(sizeof(*_tmp4D4));_tmp4D4->f1=0;_tmp4D4->f2=_tmp4D2;_tmp4D4;});_tmp4D3->tl=
0;_tmp4D3;}));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));
goto _LL1E3;}}}_LL224: if(*((int*)_tmp2D8)!= 30)goto _LL226;_tmp31A=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2D8)->f1;_tmp31B=((struct Cyc_Absyn_Struct_e_struct*)_tmp2D8)->f2;_tmp31C=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2D8)->f3;_tmp31D=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2D8)->f4;_LL225: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp31B != 0,0,0,_tmp31C,_tmp31A))->r));else{if(_tmp31D == 0)({void*
_tmp4D5[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp4D6="Struct_e: missing aggrdecl pointer";_tag_dynforward(_tmp4D6,
sizeof(char),_get_zero_arr_size(_tmp4D6,35));}),_tag_dynforward(_tmp4D5,sizeof(
void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp31D;
struct _RegionHandle _tmp4D7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4D7;
_push_region(rgn);{struct Cyc_List_List*_tmp4D8=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp31C,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct Cyc_List_List*
_tmp4D9=0;struct Cyc_List_List*_tmp4DA=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp4DA != 0;_tmp4DA=_tmp4DA->tl){struct Cyc_List_List*
_tmp4DB=_tmp4D8;for(0;_tmp4DB != 0;_tmp4DB=_tmp4DB->tl){if((*((struct _tuple11*)
_tmp4DB->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp4DA->hd){struct _tuple11
_tmp4DD;struct Cyc_Absyn_Aggrfield*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DF;struct
_tuple11*_tmp4DC=(struct _tuple11*)_tmp4DB->hd;_tmp4DD=*_tmp4DC;_tmp4DE=_tmp4DD.f1;
_tmp4DF=_tmp4DD.f2;{void*_tmp4E0=(void*)_tmp4DE->type;Cyc_Toc_exp_to_c(nv,
_tmp4DF);if(Cyc_Toc_is_void_star(_tmp4E0))(void*)(_tmp4DF->r=(void*)((void*)(Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp4DF->r,0)))->r));_tmp4D9=({
struct Cyc_List_List*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->hd=({struct
_tuple6*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->f1=0;_tmp4E2->f2=_tmp4DF;
_tmp4E2;});_tmp4E1->tl=_tmp4D9;_tmp4E1;});break;}}}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp4D9),0))->r));};_pop_region(rgn);}}goto _LL1E3;_LL226: if(*((
int*)_tmp2D8)!= 31)goto _LL228;_tmp31E=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2D8)->f1;_tmp31F=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2D8)->f2;_LL227: {
struct Cyc_List_List*fs;{void*_tmp4E3=Cyc_Tcutil_compress(_tmp31E);struct Cyc_List_List*
_tmp4E4;_LL319: if(_tmp4E3 <= (void*)4)goto _LL31B;if(*((int*)_tmp4E3)!= 11)goto
_LL31B;_tmp4E4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4E3)->f2;_LL31A: fs=
_tmp4E4;goto _LL318;_LL31B:;_LL31C:({struct Cyc_String_pa_struct _tmp4E7;_tmp4E7.tag=
0;_tmp4E7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp31E));{void*_tmp4E5[1]={& _tmp4E7};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4E6="anon struct has type %s";
_tag_dynforward(_tmp4E6,sizeof(char),_get_zero_arr_size(_tmp4E6,24));}),
_tag_dynforward(_tmp4E5,sizeof(void*),1));}});_LL318:;}{struct _RegionHandle
_tmp4E8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4E8;_push_region(rgn);{
struct Cyc_List_List*_tmp4E9=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp31F,fs);for(0;
_tmp4E9 != 0;_tmp4E9=_tmp4E9->tl){struct _tuple11 _tmp4EB;struct Cyc_Absyn_Aggrfield*
_tmp4EC;struct Cyc_Absyn_Exp*_tmp4ED;struct _tuple11*_tmp4EA=(struct _tuple11*)
_tmp4E9->hd;_tmp4EB=*_tmp4EA;_tmp4EC=_tmp4EB.f1;_tmp4ED=_tmp4EB.f2;{void*_tmp4EE=(
void*)_tmp4EC->type;Cyc_Toc_exp_to_c(nv,_tmp4ED);if(Cyc_Toc_is_void_star(_tmp4EE))(
void*)(_tmp4ED->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp4ED->r,0)))->r));}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,_tmp31F,0))->r));};_pop_region(rgn);}goto _LL1E3;}_LL228: if(*((int*)_tmp2D8)!= 
32)goto _LL22A;_tmp320=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2D8)->f1;if(_tmp320
!= 0)goto _LL22A;_tmp321=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2D8)->f2;_tmp322=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp2D8)->f3;if(!(!_tmp321->is_flat))goto _LL22A;
_LL229: {struct _tuple1*qv=_tmp322->name;struct Cyc_Absyn_Exp*tag_exp=_tmp321->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp321,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1E3;}_LL22A: if(*((int*)_tmp2D8)!= 32)goto _LL22C;
_tmp323=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2D8)->f1;_tmp324=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp2D8)->f2;_tmp325=((struct Cyc_Absyn_Tunion_e_struct*)_tmp2D8)->f3;_LL22B: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp4EF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4F0=Cyc_Absyn_var_exp(_tmp4EF,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp324->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp324->name,({const char*_tmp4F1="_union";_tag_dynforward(_tmp4F1,sizeof(char),
_get_zero_arr_size(_tmp4F1,7));})));tag_exp=Cyc_Toc_tunion_tag(_tmp324,_tmp325->name,
1);{struct _tuple1 _tmp4F3;struct _dynforward_ptr*_tmp4F4;struct _tuple1*_tmp4F2=
_tmp325->name;_tmp4F3=*_tmp4F2;_tmp4F4=_tmp4F3.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp4F0,_tmp4F4,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp325->name,({const char*_tmp4F5="_struct";_tag_dynforward(_tmp4F5,sizeof(char),
_get_zero_arr_size(_tmp4F5,8));})));tag_exp=_tmp324->is_xtunion?Cyc_Absyn_var_exp(
_tmp325->name,0): Cyc_Toc_tunion_tag(_tmp324,_tmp325->name,1);member_exp=_tmp4F0;}{
struct Cyc_List_List*_tmp4F6=_tmp325->typs;if(nv->toplevel){struct Cyc_List_List*
dles=0;for(0;_tmp323 != 0;(_tmp323=_tmp323->tl,_tmp4F6=_tmp4F6->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp323->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp4F6))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->hd=({
struct _tuple6*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->f1=0;_tmp4F8->f2=
cur_e;_tmp4F8;});_tmp4F7->tl=dles;_tmp4F7;});}dles=({struct Cyc_List_List*_tmp4F9=
_cycalloc(sizeof(*_tmp4F9));_tmp4F9->hd=({struct _tuple6*_tmp4FA=_cycalloc(
sizeof(*_tmp4FA));_tmp4FA->f1=0;_tmp4FA->f2=tag_exp;_tmp4FA;});_tmp4F9->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp4F9;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));if(
_tmp324->is_flat)(void*)(e->r=(void*)((void*)(Cyc_Toc_cast_it(Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp325->name,({const char*_tmp4FB="_struct";
_tag_dynforward(_tmp4FB,sizeof(char),_get_zero_arr_size(_tmp4FB,8));}))),Cyc_Absyn_copy_exp(
e)))->r));}else{struct Cyc_List_List*_tmp4FC=({struct Cyc_List_List*_tmp502=
_cycalloc(sizeof(*_tmp502));_tmp502->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp502->tl=0;_tmp502;});{int i=1;for(0;
_tmp323 != 0;(((_tmp323=_tmp323->tl,i ++)),_tmp4F6=_tmp4F6->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp323->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp4F6))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);{struct Cyc_Absyn_Stmt*_tmp4FD=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp4FC=({struct Cyc_List_List*
_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->hd=_tmp4FD;_tmp4FE->tl=_tmp4FC;
_tmp4FE;});}}}{struct Cyc_Absyn_Stmt*_tmp4FF=Cyc_Absyn_exp_stmt(_tmp4F0,0);struct
Cyc_Absyn_Stmt*_tmp500=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp501=_cycalloc(sizeof(*_tmp501));
_tmp501->hd=_tmp4FF;_tmp501->tl=_tmp4FC;_tmp501;})),0);(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp4EF,tunion_ctype,0,_tmp500,
0),0))->r));}}goto _LL1E3;}}_LL22C: if(*((int*)_tmp2D8)!= 33)goto _LL22E;_LL22D:
goto _LL22F;_LL22E: if(*((int*)_tmp2D8)!= 34)goto _LL230;_LL22F: goto _LL1E3;_LL230:
if(*((int*)_tmp2D8)!= 35)goto _LL232;_tmp326=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp2D8)->f1;_tmp327=_tmp326.is_calloc;_tmp328=_tmp326.rgn;_tmp329=_tmp326.elt_type;
_tmp32A=_tmp326.num_elts;_tmp32B=_tmp326.fat_result;_LL231: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp329)));Cyc_Toc_exp_to_c(nv,_tmp32A);if(_tmp32B){struct
_tuple1*_tmp503=Cyc_Toc_temp_var();struct _tuple1*_tmp504=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp327){xexp=_tmp32A;if(_tmp328 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp328;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp503,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp329,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp503,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({
struct Cyc_Absyn_Exp*_tmp505[3];_tmp505[2]=Cyc_Absyn_var_exp(_tmp503,0);_tmp505[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp505[0]=Cyc_Absyn_var_exp(_tmp504,0);((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp505,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp32A,0);if(_tmp328 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp328;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp503,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp329,Cyc_Absyn_var_exp(_tmp503,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,({struct Cyc_Absyn_Exp*
_tmp506[3];_tmp506[2]=Cyc_Absyn_var_exp(_tmp503,0);_tmp506[1]=Cyc_Absyn_uint_exp(
1,0);_tmp506[0]=Cyc_Absyn_var_exp(_tmp504,0);((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp506,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp507=Cyc_Absyn_declare_stmt(_tmp503,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp504,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp507,0))->r));}}else{if(_tmp328 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp328;Cyc_Toc_exp_to_c(nv,rgn);(
void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,
0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp329,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1E3;}_LL232: if(*((int*)_tmp2D8)!= 37)goto _LL234;_tmp32C=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp2D8)->f1;_LL233: Cyc_Toc_stmt_to_c(nv,
_tmp32C);goto _LL1E3;_LL234: if(*((int*)_tmp2D8)!= 36)goto _LL236;_LL235:({void*
_tmp508[0]={};((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp509="UnresolvedMem";_tag_dynforward(_tmp509,sizeof(char),
_get_zero_arr_size(_tmp509,14));}),_tag_dynforward(_tmp508,sizeof(void*),0));});
_LL236: if(*((int*)_tmp2D8)!= 27)goto _LL1E3;_LL237:({void*_tmp50A[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp50B="compoundlit";_tag_dynforward(_tmp50B,sizeof(char),_get_zero_arr_size(
_tmp50B,12));}),_tag_dynforward(_tmp50A,sizeof(void*),0));});_LL1E3:;}}static
struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple12{struct Cyc_Toc_Env*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple13{struct _tuple1*
f1;void*f2;};struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
static struct _tuple12 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*
rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,
struct Cyc_Absyn_Stmt**tag_fail_stmt,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*
decls){struct Cyc_Absyn_Stmt*s;{void*_tmp50C=(void*)p->r;struct Cyc_Absyn_Vardecl*
_tmp50D;struct Cyc_Absyn_Vardecl _tmp50E;struct _tuple1*_tmp50F;struct Cyc_Absyn_Pat*
_tmp510;struct Cyc_Absyn_Vardecl*_tmp511;struct Cyc_Absyn_Vardecl _tmp512;struct
_tuple1*_tmp513;struct Cyc_Absyn_Vardecl*_tmp514;struct Cyc_Absyn_Pat*_tmp515;void*
_tmp516;int _tmp517;char _tmp518;struct _dynforward_ptr _tmp519;struct Cyc_Absyn_Enumdecl*
_tmp51A;struct Cyc_Absyn_Enumfield*_tmp51B;void*_tmp51C;struct Cyc_Absyn_Enumfield*
_tmp51D;struct Cyc_Absyn_Tuniondecl*_tmp51E;struct Cyc_Absyn_Tunionfield*_tmp51F;
struct Cyc_List_List*_tmp520;struct Cyc_Absyn_Pat*_tmp521;struct Cyc_Absyn_Pat
_tmp522;void*_tmp523;struct Cyc_Absyn_Tuniondecl*_tmp524;struct Cyc_Absyn_Tunionfield*
_tmp525;struct Cyc_List_List*_tmp526;struct Cyc_Absyn_Tuniondecl*_tmp527;struct Cyc_Absyn_Tunionfield*
_tmp528;struct Cyc_List_List*_tmp529;struct Cyc_List_List*_tmp52A;struct Cyc_List_List*
_tmp52B;struct Cyc_Absyn_AggrInfo _tmp52C;void*_tmp52D;struct Cyc_List_List*_tmp52E;
struct Cyc_Absyn_Pat*_tmp52F;_LL31E: if(_tmp50C <= (void*)2)goto _LL322;if(*((int*)
_tmp50C)!= 0)goto _LL320;_tmp50D=((struct Cyc_Absyn_Var_p_struct*)_tmp50C)->f1;
_tmp50E=*_tmp50D;_tmp50F=_tmp50E.name;_tmp510=((struct Cyc_Absyn_Var_p_struct*)
_tmp50C)->f2;_LL31F: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(nv,_tmp50F,r),rgn,
t,r,path,_tmp510,tag_fail_stmt,fail_stmt,decls);_LL320: if(*((int*)_tmp50C)!= 2)
goto _LL322;_tmp511=((struct Cyc_Absyn_TagInt_p_struct*)_tmp50C)->f2;_tmp512=*
_tmp511;_tmp513=_tmp512.name;_LL321: nv=Cyc_Toc_add_varmap(nv,_tmp513,r);goto
_LL323;_LL322: if((int)_tmp50C != 0)goto _LL324;_LL323: s=Cyc_Toc_skip_stmt_dl();
goto _LL31D;_LL324: if(_tmp50C <= (void*)2)goto _LL326;if(*((int*)_tmp50C)!= 1)goto
_LL326;_tmp514=((struct Cyc_Absyn_Reference_p_struct*)_tmp50C)->f1;_tmp515=((
struct Cyc_Absyn_Reference_p_struct*)_tmp50C)->f2;_LL325: {struct _tuple1*_tmp530=
Cyc_Toc_temp_var();decls=({struct Cyc_List_List*_tmp531=_region_malloc(rgn,
sizeof(*_tmp531));_tmp531->hd=({struct _tuple13*_tmp532=_region_malloc(rgn,
sizeof(*_tmp532));_tmp532->f1=_tmp530;_tmp532->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq);_tmp532;});_tmp531->tl=decls;_tmp531;});nv=Cyc_Toc_add_varmap(
nv,_tmp514->name,Cyc_Absyn_var_exp(_tmp530,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp530,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(path,0)),0);{struct _tuple12 _tmp533=Cyc_Toc_xlate_pat(nv,
rgn,t,r,path,_tmp515,tag_fail_stmt,fail_stmt,decls);_tmp533.f3=Cyc_Absyn_seq_stmt(
s,_tmp533.f3,0);return _tmp533;}}_LL326: if((int)_tmp50C != 1)goto _LL328;_LL327: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL31D;_LL328:
if(_tmp50C <= (void*)2)goto _LL32A;if(*((int*)_tmp50C)!= 7)goto _LL32A;_tmp516=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp50C)->f1;_tmp517=((struct Cyc_Absyn_Int_p_struct*)
_tmp50C)->f2;_LL329: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp516,_tmp517,0),
fail_stmt);goto _LL31D;_LL32A: if(_tmp50C <= (void*)2)goto _LL32C;if(*((int*)_tmp50C)
!= 8)goto _LL32C;_tmp518=((struct Cyc_Absyn_Char_p_struct*)_tmp50C)->f1;_LL32B: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp518,0),fail_stmt);goto _LL31D;_LL32C:
if(_tmp50C <= (void*)2)goto _LL32E;if(*((int*)_tmp50C)!= 9)goto _LL32E;_tmp519=((
struct Cyc_Absyn_Float_p_struct*)_tmp50C)->f1;_LL32D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp519,0),fail_stmt);goto _LL31D;_LL32E: if(_tmp50C <= (void*)2)goto _LL330;if(*((
int*)_tmp50C)!= 10)goto _LL330;_tmp51A=((struct Cyc_Absyn_Enum_p_struct*)_tmp50C)->f1;
_tmp51B=((struct Cyc_Absyn_Enum_p_struct*)_tmp50C)->f2;_LL32F: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp534=_cycalloc(
sizeof(*_tmp534));_tmp534[0]=({struct Cyc_Absyn_Enum_e_struct _tmp535;_tmp535.tag=
33;_tmp535.f1=_tmp51B->name;_tmp535.f2=(struct Cyc_Absyn_Enumdecl*)_tmp51A;
_tmp535.f3=(struct Cyc_Absyn_Enumfield*)_tmp51B;_tmp535;});_tmp534;}),0),
fail_stmt);goto _LL31D;_LL330: if(_tmp50C <= (void*)2)goto _LL332;if(*((int*)_tmp50C)
!= 11)goto _LL332;_tmp51C=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp50C)->f1;
_tmp51D=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp50C)->f2;_LL331: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp536=_cycalloc(
sizeof(*_tmp536));_tmp536[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp537;_tmp537.tag=
34;_tmp537.f1=_tmp51D->name;_tmp537.f2=(void*)_tmp51C;_tmp537.f3=(struct Cyc_Absyn_Enumfield*)
_tmp51D;_tmp537;});_tmp536;}),0),fail_stmt);goto _LL31D;_LL332: if(_tmp50C <= (void*)
2)goto _LL334;if(*((int*)_tmp50C)!= 6)goto _LL334;_tmp51E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp50C)->f1;_tmp51F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp50C)->f2;_tmp520=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp50C)->f3;if(_tmp520 != 0)goto _LL334;if(!(!
_tmp51E->is_flat))goto _LL334;_LL333: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp51E->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp51F->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp51E,_tmp51F->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL31D;}_LL334: if(_tmp50C <= (void*)2)goto _LL336;if(*((
int*)_tmp50C)!= 4)goto _LL336;_tmp521=((struct Cyc_Absyn_Pointer_p_struct*)_tmp50C)->f1;
_tmp522=*_tmp521;_tmp523=(void*)_tmp522.r;if(_tmp523 <= (void*)2)goto _LL336;if(*((
int*)_tmp523)!= 6)goto _LL336;_tmp524=((struct Cyc_Absyn_Tunion_p_struct*)_tmp523)->f1;
_tmp525=((struct Cyc_Absyn_Tunion_p_struct*)_tmp523)->f2;_tmp526=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp523)->f3;if(!(_tmp526 != 0  && !_tmp524->is_flat))goto _LL336;_LL335: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp525->name,({const
char*_tmp544="_struct";_tag_dynforward(_tmp544,sizeof(char),_get_zero_arr_size(
_tmp544,8));}));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*_tmp538=_tmp525->typs;
for(0;_tmp526 != 0;(((_tmp526=_tmp526->tl,_tmp538=((struct Cyc_List_List*)
_check_null(_tmp538))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp539=(struct Cyc_Absyn_Pat*)
_tmp526->hd;if((void*)_tmp539->r == (void*)0)continue;{void*_tmp53A=(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp538))->hd)).f2;struct _tuple1*
_tmp53B=Cyc_Toc_temp_var();void*_tmp53C=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp539->topt))->v;void*_tmp53D=Cyc_Toc_typ_to_c(_tmp53C);struct Cyc_Absyn_Exp*
_tmp53E=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp53A)))_tmp53E=Cyc_Toc_cast_it(_tmp53D,_tmp53E);decls=({
struct Cyc_List_List*_tmp53F=_region_malloc(rgn,sizeof(*_tmp53F));_tmp53F->hd=({
struct _tuple13*_tmp540=_region_malloc(rgn,sizeof(*_tmp540));_tmp540->f1=_tmp53B;
_tmp540->f2=_tmp53D;_tmp540;});_tmp53F->tl=decls;_tmp53F;});{struct _tuple12
_tmp541=Cyc_Toc_xlate_pat(nv,rgn,_tmp53C,Cyc_Absyn_var_exp(_tmp53B,0),_tmp53E,
_tmp539,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp541.f1;decls=
_tmp541.f2;{struct Cyc_Absyn_Stmt*_tmp542=_tmp541.f3;struct Cyc_Absyn_Stmt*_tmp543=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp53B,0),_tmp53E,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp543,_tmp542,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp524->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp525->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp524,_tmp525->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp524);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL31D;}}_LL336: if(_tmp50C <= (void*)2)goto
_LL338;if(*((int*)_tmp50C)!= 6)goto _LL338;_tmp527=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp50C)->f1;_tmp528=((struct Cyc_Absyn_Tunion_p_struct*)_tmp50C)->f2;_tmp529=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp50C)->f3;if(!_tmp527->is_flat)goto _LL338;
_LL337: {struct _tuple1 _tmp546;struct _dynforward_ptr*_tmp547;struct _tuple1*
_tmp545=_tmp528->name;_tmp546=*_tmp545;_tmp547=_tmp546.f2;r=Cyc_Absyn_aggrmember_exp(
r,_tmp547,0);path=Cyc_Absyn_aggrmember_exp(path,_tmp547,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
_tmp527,_tmp528->name,1),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;
_tmp529 != 0;(_tmp529=_tmp529->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp548=(struct Cyc_Absyn_Pat*)
_tmp529->hd;if((void*)_tmp548->r == (void*)0)continue;{struct _tuple1*_tmp549=Cyc_Toc_temp_var();
void*_tmp54A=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp548->topt))->v;decls=({
struct Cyc_List_List*_tmp54B=_region_malloc(rgn,sizeof(*_tmp54B));_tmp54B->hd=({
struct _tuple13*_tmp54C=_region_malloc(rgn,sizeof(*_tmp54C));_tmp54C->f1=_tmp549;
_tmp54C->f2=Cyc_Toc_typ_to_c(_tmp54A);_tmp54C;});_tmp54B->tl=decls;_tmp54B;});{
struct _tuple12 _tmp54D=Cyc_Toc_xlate_pat(nv,rgn,_tmp54A,Cyc_Absyn_var_exp(_tmp549,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp548,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp54D.f1;decls=_tmp54D.f2;{struct Cyc_Absyn_Stmt*
_tmp54E=_tmp54D.f3;struct Cyc_Absyn_Stmt*_tmp54F=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp549,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp54F,_tmp54E,0),0);}}}}goto _LL31D;}}_LL338: if(_tmp50C <= (
void*)2)goto _LL33A;if(*((int*)_tmp50C)!= 6)goto _LL33A;_tmp52A=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp50C)->f3;_LL339: _tmp52B=_tmp52A;goto _LL33B;_LL33A: if(_tmp50C <= (void*)2)goto
_LL33C;if(*((int*)_tmp50C)!= 3)goto _LL33C;_tmp52B=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp50C)->f1;_LL33B: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp52B != 0;(
_tmp52B=_tmp52B->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp550=(struct Cyc_Absyn_Pat*)
_tmp52B->hd;if((void*)_tmp550->r == (void*)0)continue;{struct _tuple1*_tmp551=Cyc_Toc_temp_var();
void*_tmp552=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp550->topt))->v;decls=({
struct Cyc_List_List*_tmp553=_region_malloc(rgn,sizeof(*_tmp553));_tmp553->hd=({
struct _tuple13*_tmp554=_region_malloc(rgn,sizeof(*_tmp554));_tmp554->f1=_tmp551;
_tmp554->f2=Cyc_Toc_typ_to_c(_tmp552);_tmp554;});_tmp553->tl=decls;_tmp553;});{
struct _tuple12 _tmp555=Cyc_Toc_xlate_pat(nv,rgn,_tmp552,Cyc_Absyn_var_exp(_tmp551,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp550,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp555.f1;decls=_tmp555.f2;{struct Cyc_Absyn_Stmt*
_tmp556=_tmp555.f3;struct Cyc_Absyn_Stmt*_tmp557=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp551,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp557,_tmp556,0),0);}}}}goto _LL31D;}_LL33C: if(_tmp50C <= (
void*)2)goto _LL33E;if(*((int*)_tmp50C)!= 5)goto _LL33E;_tmp52C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp50C)->f1;_tmp52D=(void*)_tmp52C.aggr_info;_tmp52E=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp50C)->f3;_LL33D: {struct Cyc_Absyn_Aggrdecl*_tmp558=Cyc_Absyn_get_known_aggrdecl(
_tmp52D);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp52E != 0;_tmp52E=_tmp52E->tl){struct
_tuple14*_tmp559=(struct _tuple14*)_tmp52E->hd;struct Cyc_Absyn_Pat*_tmp55A=(*
_tmp559).f2;if((void*)_tmp55A->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp55B=(void*)((struct Cyc_List_List*)_check_null((*_tmp559).f1))->hd;
struct _dynforward_ptr*_tmp55C;_LL347: if(*((int*)_tmp55B)!= 1)goto _LL349;_tmp55C=((
struct Cyc_Absyn_FieldName_struct*)_tmp55B)->f1;_LL348: f=_tmp55C;goto _LL346;
_LL349:;_LL34A:(int)_throw((void*)Cyc_Toc_Match_error);_LL346:;}{struct _tuple1*
_tmp55D=Cyc_Toc_temp_var();void*_tmp55E=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp55A->topt))->v;void*_tmp55F=Cyc_Toc_typ_to_c(_tmp55E);decls=({struct Cyc_List_List*
_tmp560=_region_malloc(rgn,sizeof(*_tmp560));_tmp560->hd=({struct _tuple13*
_tmp561=_region_malloc(rgn,sizeof(*_tmp561));_tmp561->f1=_tmp55D;_tmp561->f2=
_tmp55F;_tmp561;});_tmp560->tl=decls;_tmp560;});{struct _tuple12 _tmp562=Cyc_Toc_xlate_pat(
nv,rgn,_tmp55E,Cyc_Absyn_var_exp(_tmp55D,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp55A,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp562.f1;decls=
_tmp562.f2;{struct Cyc_Absyn_Exp*_tmp563=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp558->impl))->fields,f)))->type))_tmp563=
Cyc_Toc_cast_it(_tmp55F,_tmp563);{struct Cyc_Absyn_Stmt*_tmp564=_tmp562.f3;struct
Cyc_Absyn_Stmt*_tmp565=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp55D,0),
_tmp563,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp565,_tmp564,0),0);}}}}}}
goto _LL31D;}_LL33E: if(_tmp50C <= (void*)2)goto _LL340;if(*((int*)_tmp50C)!= 4)goto
_LL340;_tmp52F=((struct Cyc_Absyn_Pointer_p_struct*)_tmp50C)->f1;_LL33F: {struct
_tuple1*_tmp566=Cyc_Toc_temp_var();void*_tmp567=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp52F->topt))->v;decls=({struct Cyc_List_List*_tmp568=
_region_malloc(rgn,sizeof(*_tmp568));_tmp568->hd=({struct _tuple13*_tmp569=
_region_malloc(rgn,sizeof(*_tmp569));_tmp569->f1=_tmp566;_tmp569->f2=Cyc_Toc_typ_to_c(
_tmp567);_tmp569;});_tmp568->tl=decls;_tmp568;});{struct _tuple12 _tmp56A=Cyc_Toc_xlate_pat(
nv,rgn,_tmp567,Cyc_Absyn_var_exp(_tmp566,0),Cyc_Absyn_deref_exp(path,0),_tmp52F,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp56A.f1;decls=_tmp56A.f2;{
struct Cyc_Absyn_Stmt*_tmp56B=_tmp56A.f3;struct Cyc_Absyn_Stmt*_tmp56C=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp566,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp56B,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp56C,0);else{s=_tmp56C;}goto _LL31D;}}}_LL340: if(_tmp50C <= (void*)2)goto
_LL342;if(*((int*)_tmp50C)!= 12)goto _LL342;_LL341:({void*_tmp56D[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp56E="unknownid pat";_tag_dynforward(_tmp56E,sizeof(char),_get_zero_arr_size(
_tmp56E,14));}),_tag_dynforward(_tmp56D,sizeof(void*),0));});_LL342: if(_tmp50C <= (
void*)2)goto _LL344;if(*((int*)_tmp50C)!= 13)goto _LL344;_LL343:({void*_tmp56F[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp570="unknowncall pat";_tag_dynforward(_tmp570,sizeof(char),
_get_zero_arr_size(_tmp570,16));}),_tag_dynforward(_tmp56F,sizeof(void*),0));});
_LL344: if(_tmp50C <= (void*)2)goto _LL31D;if(*((int*)_tmp50C)!= 14)goto _LL31D;
_LL345:({void*_tmp571[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp572="exp pat";
_tag_dynforward(_tmp572,sizeof(char),_get_zero_arr_size(_tmp572,8));}),
_tag_dynforward(_tmp571,sizeof(void*),0));});_LL31D:;}return({struct _tuple12
_tmp573;_tmp573.f1=nv;_tmp573.f2=decls;_tmp573.f3=s;_tmp573;});}struct _tuple15{
struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple15*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple15*_tmp574=_region_malloc(r,sizeof(*_tmp574));_tmp574->f1=
Cyc_Toc_fresh_label();_tmp574->f2=Cyc_Toc_fresh_label();_tmp574->f3=sc;_tmp574;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp575=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp576;void*_tmp577;struct Cyc_Absyn_Tuniondecl**
_tmp578;struct Cyc_Absyn_Tuniondecl*_tmp579;_LL34C: if(_tmp575 <= (void*)4)goto
_LL34E;if(*((int*)_tmp575)!= 2)goto _LL34E;_tmp576=((struct Cyc_Absyn_TunionType_struct*)
_tmp575)->f1;_tmp577=(void*)_tmp576.tunion_info;if(*((int*)_tmp577)!= 1)goto
_LL34E;_tmp578=((struct Cyc_Absyn_KnownTunion_struct*)_tmp577)->f1;_tmp579=*
_tmp578;if(!(!_tmp579->is_flat))goto _LL34E;_LL34D: {int seen_novalue=0;int
seen_value=0;{struct Cyc_List_List*_tmp57A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp579->fields))->v;for(0;(unsigned int)_tmp57A;_tmp57A=_tmp57A->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp57A->hd)->typs == 0)seen_value=1;else{
seen_novalue=1;}if(seen_value  && seen_novalue)return 1;}}goto _LL34B;}_LL34E:;
_LL34F: goto _LL34B;_LL34B:;}return 0;}static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*
p){void*_tmp57B=(void*)p->r;struct Cyc_Absyn_Pat*_tmp57C;struct Cyc_Absyn_Pat*
_tmp57D;struct Cyc_Absyn_Pat _tmp57E;void*_tmp57F;struct Cyc_Absyn_Tuniondecl*
_tmp580;struct Cyc_Absyn_Tunionfield*_tmp581;struct Cyc_List_List*_tmp582;_LL351:
if(_tmp57B <= (void*)2)goto _LL355;if(*((int*)_tmp57B)!= 0)goto _LL353;_tmp57C=((
struct Cyc_Absyn_Var_p_struct*)_tmp57B)->f2;_LL352: return Cyc_Toc_no_tag_test(
_tmp57C);_LL353: if(*((int*)_tmp57B)!= 4)goto _LL355;_tmp57D=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp57B)->f1;_tmp57E=*_tmp57D;_tmp57F=(void*)_tmp57E.r;if(_tmp57F <= (void*)2)
goto _LL355;if(*((int*)_tmp57F)!= 6)goto _LL355;_tmp580=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp57F)->f1;_tmp581=((struct Cyc_Absyn_Tunion_p_struct*)_tmp57F)->f2;_tmp582=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp57F)->f3;if(!(_tmp582 != 0  && !_tmp580->is_flat))
goto _LL355;_LL354: return 0;_LL355:;_LL356: return 1;_LL350:;}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp583=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp584=Cyc_Tcutil_compress(
_tmp583);_LL358: if(_tmp584 <= (void*)4)goto _LL35C;if(*((int*)_tmp584)!= 5)goto
_LL35A;_LL359: goto _LL35B;_LL35A: if(*((int*)_tmp584)!= 12)goto _LL35C;_LL35B:
leave_as_switch=1;goto _LL357;_LL35C:;_LL35D: leave_as_switch=0;goto _LL357;_LL357:;}{
struct Cyc_List_List*_tmp585=scs;for(0;_tmp585 != 0;_tmp585=_tmp585->tl){if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp585->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp585->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _dynforward_ptr*
next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp586=scs;for(0;_tmp586 != 0;
_tmp586=_tmp586->tl){struct Cyc_Absyn_Stmt*_tmp587=((struct Cyc_Absyn_Switch_clause*)
_tmp586->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp586->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp587,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp587);}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp588=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp588;_push_region(rgn);{struct Cyc_List_List*
lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp583);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp589=lscs;for(0;_tmp589 != 0;_tmp589=_tmp589->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple15*)_tmp589->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp589->tl
== 0?end_l:(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp589->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp58A=_tmp589->tl;for(0;(unsigned int)
_tmp58A;_tmp58A=_tmp58A->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple15*)_tmp58A->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp58B=_region_malloc(rgn,sizeof(*_tmp58B));
_tmp58B[0]=Cyc_Absyn_goto_stmt((*((struct _tuple15*)_tmp58A->hd)).f1,0);_tmp58B;});
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp58C=_region_malloc(rgn,sizeof(*_tmp58C));_tmp58C[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp58C;});}{struct Cyc_Toc_Env*_tmp58E;struct Cyc_List_List*_tmp58F;
struct Cyc_Absyn_Stmt*_tmp590;struct _tuple12 _tmp58D=Cyc_Toc_xlate_pat(nv,rgn,
_tmp583,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);
_tmp58E=_tmp58D.f1;_tmp58F=_tmp58D.f2;_tmp590=_tmp58D.f3;if(is_tunion  && Cyc_Toc_no_tag_test(
sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*
_tmp591=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp58E,_tmp591);_tmp590=Cyc_Absyn_seq_stmt(_tmp590,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp591,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp58F;nvs=({struct Cyc_List_List*_tmp592=_region_malloc(rgn,
sizeof(*_tmp592));_tmp592->hd=_tmp58E;_tmp592->tl=nvs;_tmp592;});test_stmts=({
struct Cyc_List_List*_tmp593=_region_malloc(rgn,sizeof(*_tmp593));_tmp593->hd=
_tmp590;_tmp593->tl=test_stmts;_tmp593;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple15 _tmp595;struct _dynforward_ptr*_tmp596;struct _dynforward_ptr*
_tmp597;struct Cyc_Absyn_Switch_clause*_tmp598;struct _tuple15*_tmp594=(struct
_tuple15*)lscs->hd;_tmp595=*_tmp594;_tmp596=_tmp595.f1;_tmp597=_tmp595.f2;
_tmp598=_tmp595.f3;{struct Cyc_Toc_Env*_tmp599=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp598->body;if(lscs->tl != 0){
struct _tuple15 _tmp59B;struct _dynforward_ptr*_tmp59C;struct Cyc_Absyn_Switch_clause*
_tmp59D;struct _tuple15*_tmp59A=(struct _tuple15*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp59B=*_tmp59A;_tmp59C=_tmp59B.f2;_tmp59D=_tmp59B.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp599,end_l,_tmp59C,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp59D->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp599,end_l),s);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_label_stmt(_tmp596,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)
_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(_tmp597,s,0),0);stmts=({
struct Cyc_List_List*_tmp59E=_region_malloc(rgn,sizeof(*_tmp59E));_tmp59E->hd=s;
_tmp59E->tl=stmts;_tmp59E;});}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple13 _tmp5A0;struct _tuple1*_tmp5A1;void*_tmp5A2;struct _tuple13*_tmp59F=(
struct _tuple13*)decls->hd;_tmp5A0=*_tmp59F;_tmp5A1=_tmp5A0.f1;_tmp5A2=_tmp5A0.f2;
res=Cyc_Absyn_declare_stmt(_tmp5A1,_tmp5A2,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));
_tmp5A3->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp5A3->tl=0;_tmp5A3;}),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)(void*)(s->r=(
void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp5A4=(void*)s->r;struct Cyc_Absyn_Exp*_tmp5A5;
struct Cyc_Absyn_Stmt*_tmp5A6;struct Cyc_Absyn_Stmt*_tmp5A7;struct Cyc_Absyn_Exp*
_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A9;struct Cyc_Absyn_Stmt*_tmp5AA;struct Cyc_Absyn_Stmt*
_tmp5AB;struct _tuple3 _tmp5AC;struct Cyc_Absyn_Exp*_tmp5AD;struct Cyc_Absyn_Stmt*
_tmp5AE;struct Cyc_Absyn_Stmt*_tmp5AF;struct Cyc_Absyn_Stmt*_tmp5B0;struct Cyc_Absyn_Stmt*
_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B2;struct _tuple3 _tmp5B3;struct Cyc_Absyn_Exp*
_tmp5B4;struct _tuple3 _tmp5B5;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Stmt*
_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B8;struct Cyc_List_List*_tmp5B9;struct Cyc_List_List*
_tmp5BA;struct Cyc_Absyn_Switch_clause**_tmp5BB;struct Cyc_Absyn_Decl*_tmp5BC;
struct Cyc_Absyn_Stmt*_tmp5BD;struct _dynforward_ptr*_tmp5BE;struct Cyc_Absyn_Stmt*
_tmp5BF;struct Cyc_Absyn_Stmt*_tmp5C0;struct _tuple3 _tmp5C1;struct Cyc_Absyn_Exp*
_tmp5C2;struct Cyc_Absyn_Stmt*_tmp5C3;struct Cyc_List_List*_tmp5C4;struct Cyc_Absyn_Tvar*
_tmp5C5;struct Cyc_Absyn_Vardecl*_tmp5C6;int _tmp5C7;struct Cyc_Absyn_Stmt*_tmp5C8;
struct Cyc_Absyn_Exp*_tmp5C9;_LL35F: if((int)_tmp5A4 != 0)goto _LL361;_LL360: return;
_LL361: if(_tmp5A4 <= (void*)1)goto _LL363;if(*((int*)_tmp5A4)!= 0)goto _LL363;
_tmp5A5=((struct Cyc_Absyn_Exp_s_struct*)_tmp5A4)->f1;_LL362: Cyc_Toc_exp_to_c(nv,
_tmp5A5);return;_LL363: if(_tmp5A4 <= (void*)1)goto _LL365;if(*((int*)_tmp5A4)!= 1)
goto _LL365;_tmp5A6=((struct Cyc_Absyn_Seq_s_struct*)_tmp5A4)->f1;_tmp5A7=((struct
Cyc_Absyn_Seq_s_struct*)_tmp5A4)->f2;_LL364: Cyc_Toc_stmt_to_c(nv,_tmp5A6);s=
_tmp5A7;continue;_LL365: if(_tmp5A4 <= (void*)1)goto _LL367;if(*((int*)_tmp5A4)!= 2)
goto _LL367;_tmp5A8=((struct Cyc_Absyn_Return_s_struct*)_tmp5A4)->f1;_LL366: {
struct Cyc_Core_Opt*topt=0;if(_tmp5A8 != 0){topt=({struct Cyc_Core_Opt*_tmp5CA=
_cycalloc(sizeof(*_tmp5CA));_tmp5CA->v=(void*)Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp5A8->topt))->v);_tmp5CA;});Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp5A8);}if(s->try_depth > 0){if(topt != 0){struct _tuple1*
_tmp5CB=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp5CC=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp5CB,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp5CB,(void*)topt->v,_tmp5A8,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp5CC,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL367: if(_tmp5A4 <= (void*)1)goto _LL369;if(*((int*)_tmp5A4)!= 3)goto
_LL369;_tmp5A9=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5A4)->f1;_tmp5AA=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5A4)->f2;_tmp5AB=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp5A4)->f3;_LL368: Cyc_Toc_exp_to_c(nv,_tmp5A9);Cyc_Toc_stmt_to_c(nv,_tmp5AA);s=
_tmp5AB;continue;_LL369: if(_tmp5A4 <= (void*)1)goto _LL36B;if(*((int*)_tmp5A4)!= 4)
goto _LL36B;_tmp5AC=((struct Cyc_Absyn_While_s_struct*)_tmp5A4)->f1;_tmp5AD=
_tmp5AC.f1;_tmp5AE=((struct Cyc_Absyn_While_s_struct*)_tmp5A4)->f2;_LL36A: Cyc_Toc_exp_to_c(
nv,_tmp5AD);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp5AE);return;_LL36B: if(
_tmp5A4 <= (void*)1)goto _LL36D;if(*((int*)_tmp5A4)!= 5)goto _LL36D;_tmp5AF=((
struct Cyc_Absyn_Break_s_struct*)_tmp5A4)->f1;_LL36C: if(nv->break_lab != 0)(void*)(
s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _dynforward_ptr*)((struct Cyc_Core_Opt*)
_check_null(nv->break_lab))->v,0))->r));{int dest_depth=_tmp5AF == 0?0: _tmp5AF->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}_LL36D: if(_tmp5A4 <= (
void*)1)goto _LL36F;if(*((int*)_tmp5A4)!= 6)goto _LL36F;_tmp5B0=((struct Cyc_Absyn_Continue_s_struct*)
_tmp5A4)->f1;_LL36E: if(nv->continue_lab != 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((
struct _dynforward_ptr*)((struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));
_tmp5B1=_tmp5B0;goto _LL370;_LL36F: if(_tmp5A4 <= (void*)1)goto _LL371;if(*((int*)
_tmp5A4)!= 7)goto _LL371;_tmp5B1=((struct Cyc_Absyn_Goto_s_struct*)_tmp5A4)->f2;
_LL370: Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(
_tmp5B1))->try_depth,s);return;_LL371: if(_tmp5A4 <= (void*)1)goto _LL373;if(*((int*)
_tmp5A4)!= 8)goto _LL373;_tmp5B2=((struct Cyc_Absyn_For_s_struct*)_tmp5A4)->f1;
_tmp5B3=((struct Cyc_Absyn_For_s_struct*)_tmp5A4)->f2;_tmp5B4=_tmp5B3.f1;_tmp5B5=((
struct Cyc_Absyn_For_s_struct*)_tmp5A4)->f3;_tmp5B6=_tmp5B5.f1;_tmp5B7=((struct
Cyc_Absyn_For_s_struct*)_tmp5A4)->f4;_LL372: Cyc_Toc_exp_to_c(nv,_tmp5B2);Cyc_Toc_exp_to_c(
nv,_tmp5B4);Cyc_Toc_exp_to_c(nv,_tmp5B6);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp5B7);return;_LL373: if(_tmp5A4 <= (void*)1)goto _LL375;if(*((int*)_tmp5A4)!= 9)
goto _LL375;_tmp5B8=((struct Cyc_Absyn_Switch_s_struct*)_tmp5A4)->f1;_tmp5B9=((
struct Cyc_Absyn_Switch_s_struct*)_tmp5A4)->f2;_LL374: Cyc_Toc_xlate_switch(nv,s,
_tmp5B8,_tmp5B9);return;_LL375: if(_tmp5A4 <= (void*)1)goto _LL377;if(*((int*)
_tmp5A4)!= 10)goto _LL377;_tmp5BA=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp5A4)->f1;
_tmp5BB=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp5A4)->f2;_LL376: if(nv->fallthru_info
== 0)({void*_tmp5CD[0]={};((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5CE="fallthru in unexpected place";
_tag_dynforward(_tmp5CE,sizeof(char),_get_zero_arr_size(_tmp5CE,29));}),
_tag_dynforward(_tmp5CD,sizeof(void*),0));});{struct _tuple8 _tmp5D0;struct
_dynforward_ptr*_tmp5D1;struct Cyc_List_List*_tmp5D2;struct Cyc_Dict_Dict*_tmp5D3;
struct _tuple8*_tmp5CF=(struct _tuple8*)((struct Cyc_Core_Opt*)_check_null(nv->fallthru_info))->v;
_tmp5D0=*_tmp5CF;_tmp5D1=_tmp5D0.f1;_tmp5D2=_tmp5D0.f2;_tmp5D3=_tmp5D0.f3;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp5D1,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp5BB)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp5D4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp5D2);struct Cyc_List_List*_tmp5D5=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp5BA);for(0;_tmp5D4 != 0;(_tmp5D4=_tmp5D4->tl,
_tmp5D5=_tmp5D5->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5D5))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp5D3,(struct _tuple1*)_tmp5D4->hd),(struct Cyc_Absyn_Exp*)_tmp5D5->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL377: if(_tmp5A4 <= (void*)1)goto
_LL379;if(*((int*)_tmp5A4)!= 11)goto _LL379;_tmp5BC=((struct Cyc_Absyn_Decl_s_struct*)
_tmp5A4)->f1;_tmp5BD=((struct Cyc_Absyn_Decl_s_struct*)_tmp5A4)->f2;_LL378:{void*
_tmp5D6=(void*)_tmp5BC->r;struct Cyc_Absyn_Vardecl*_tmp5D7;struct Cyc_Absyn_Pat*
_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_List_List*_tmp5DA;struct Cyc_Absyn_Fndecl*
_tmp5DB;_LL384: if(_tmp5D6 <= (void*)2)goto _LL38C;if(*((int*)_tmp5D6)!= 0)goto
_LL386;_tmp5D7=((struct Cyc_Absyn_Var_d_struct*)_tmp5D6)->f1;_LL385: {struct Cyc_Toc_Env*
_tmp5DC=Cyc_Toc_add_varmap(nv,_tmp5D7->name,Cyc_Absyn_varb_exp(_tmp5D7->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));
_tmp5DD[0]=({struct Cyc_Absyn_Local_b_struct _tmp5DE;_tmp5DE.tag=3;_tmp5DE.f1=
_tmp5D7;_tmp5DE;});_tmp5DD;}),0));Cyc_Toc_local_decl_to_c(_tmp5DC,_tmp5DC,
_tmp5D7,_tmp5BD);goto _LL383;}_LL386: if(*((int*)_tmp5D6)!= 2)goto _LL388;_tmp5D8=((
struct Cyc_Absyn_Let_d_struct*)_tmp5D6)->f1;_tmp5D9=((struct Cyc_Absyn_Let_d_struct*)
_tmp5D6)->f3;_LL387:{void*_tmp5DF=(void*)_tmp5D8->r;struct Cyc_Absyn_Vardecl*
_tmp5E0;struct Cyc_Absyn_Pat*_tmp5E1;struct Cyc_Absyn_Pat _tmp5E2;void*_tmp5E3;
_LL38F: if(_tmp5DF <= (void*)2)goto _LL391;if(*((int*)_tmp5DF)!= 0)goto _LL391;
_tmp5E0=((struct Cyc_Absyn_Var_p_struct*)_tmp5DF)->f1;_tmp5E1=((struct Cyc_Absyn_Var_p_struct*)
_tmp5DF)->f2;_tmp5E2=*_tmp5E1;_tmp5E3=(void*)_tmp5E2.r;if((int)_tmp5E3 != 0)goto
_LL391;_LL390: {struct _tuple1*old_name=_tmp5E0->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp5E0->name=new_name;_tmp5E0->initializer=(struct Cyc_Absyn_Exp*)_tmp5D9;(void*)(
_tmp5BC->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp5E4=_cycalloc(
sizeof(*_tmp5E4));_tmp5E4[0]=({struct Cyc_Absyn_Var_d_struct _tmp5E5;_tmp5E5.tag=0;
_tmp5E5.f1=_tmp5E0;_tmp5E5;});_tmp5E4;})));{struct Cyc_Toc_Env*_tmp5E6=Cyc_Toc_add_varmap(
nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7[0]=({struct Cyc_Absyn_Local_b_struct
_tmp5E8;_tmp5E8.tag=3;_tmp5E8.f1=_tmp5E0;_tmp5E8;});_tmp5E7;}),0));Cyc_Toc_local_decl_to_c(
_tmp5E6,nv,_tmp5E0,_tmp5BD);goto _LL38E;}}_LL391:;_LL392:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp5D8,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5D9->topt))->v,_tmp5D9,_tmp5BD))->r));goto _LL38E;_LL38E:;}goto _LL383;_LL388:
if(*((int*)_tmp5D6)!= 3)goto _LL38A;_tmp5DA=((struct Cyc_Absyn_Letv_d_struct*)
_tmp5D6)->f1;_LL389: {struct Cyc_List_List*_tmp5E9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp5DA);if(_tmp5E9 == 0)({void*_tmp5EA[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp5EB="empty Letv_d";_tag_dynforward(_tmp5EB,sizeof(char),
_get_zero_arr_size(_tmp5EB,13));}),_tag_dynforward(_tmp5EA,sizeof(void*),0));});(
void*)(_tmp5BC->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp5EC=
_cycalloc(sizeof(*_tmp5EC));_tmp5EC[0]=({struct Cyc_Absyn_Var_d_struct _tmp5ED;
_tmp5ED.tag=0;_tmp5ED.f1=(struct Cyc_Absyn_Vardecl*)_tmp5E9->hd;_tmp5ED;});
_tmp5EC;})));_tmp5E9=_tmp5E9->tl;for(0;_tmp5E9 != 0;_tmp5E9=_tmp5E9->tl){struct
Cyc_Absyn_Decl*_tmp5EE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF[0]=({struct Cyc_Absyn_Var_d_struct
_tmp5F0;_tmp5F0.tag=0;_tmp5F0.f1=(struct Cyc_Absyn_Vardecl*)_tmp5E9->hd;_tmp5F0;});
_tmp5EF;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp5EE,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL383;}_LL38A: if(*((int*)
_tmp5D6)!= 1)goto _LL38C;_tmp5DB=((struct Cyc_Absyn_Fn_d_struct*)_tmp5D6)->f1;
_LL38B: {struct _tuple1*_tmp5F1=_tmp5DB->name;nv=Cyc_Toc_add_varmap(nv,_tmp5DB->name,
Cyc_Absyn_var_exp(_tmp5F1,0));Cyc_Toc_fndecl_to_c(nv,_tmp5DB,0);Cyc_Toc_stmt_to_c(
nv,_tmp5BD);goto _LL383;}_LL38C:;_LL38D:({void*_tmp5F2[0]={};((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp5F3="bad nested declaration within function";_tag_dynforward(_tmp5F3,sizeof(
char),_get_zero_arr_size(_tmp5F3,39));}),_tag_dynforward(_tmp5F2,sizeof(void*),0));});
_LL383:;}return;_LL379: if(_tmp5A4 <= (void*)1)goto _LL37B;if(*((int*)_tmp5A4)!= 12)
goto _LL37B;_tmp5BE=((struct Cyc_Absyn_Label_s_struct*)_tmp5A4)->f1;_tmp5BF=((
struct Cyc_Absyn_Label_s_struct*)_tmp5A4)->f2;_LL37A: s=_tmp5BF;continue;_LL37B:
if(_tmp5A4 <= (void*)1)goto _LL37D;if(*((int*)_tmp5A4)!= 13)goto _LL37D;_tmp5C0=((
struct Cyc_Absyn_Do_s_struct*)_tmp5A4)->f1;_tmp5C1=((struct Cyc_Absyn_Do_s_struct*)
_tmp5A4)->f2;_tmp5C2=_tmp5C1.f1;_LL37C: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp5C0);Cyc_Toc_exp_to_c(nv,_tmp5C2);return;_LL37D: if(_tmp5A4 <= (void*)1)goto
_LL37F;if(*((int*)_tmp5A4)!= 14)goto _LL37F;_tmp5C3=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp5A4)->f1;_tmp5C4=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp5A4)->f2;_LL37E: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->v=(void*)e_typ;_tmp5F4;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp5C3);{struct Cyc_Absyn_Stmt*_tmp5F5=Cyc_Absyn_seq_stmt(
_tmp5C3,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp5F6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5F7=Cyc_Absyn_var_exp(
_tmp5F6,0);struct Cyc_Absyn_Vardecl*_tmp5F8=Cyc_Absyn_new_vardecl(_tmp5F6,Cyc_Absyn_exn_typ,
0);_tmp5F7->topt=({struct Cyc_Core_Opt*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));
_tmp5F9->v=(void*)Cyc_Absyn_exn_typ;_tmp5F9;});{struct Cyc_Absyn_Pat*_tmp5FA=({
struct Cyc_Absyn_Pat*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C[0]=({
struct Cyc_Absyn_Var_p_struct _tmp60D;_tmp60D.tag=0;_tmp60D.f1=_tmp5F8;_tmp60D.f2=({
struct Cyc_Absyn_Pat*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->r=(void*)((void*)
0);_tmp60E->topt=({struct Cyc_Core_Opt*_tmp60F=_cycalloc(sizeof(*_tmp60F));
_tmp60F->v=(void*)Cyc_Absyn_exn_typ;_tmp60F;});_tmp60E->loc=0;_tmp60E;});_tmp60D;});
_tmp60C;}));_tmp60A->topt=({struct Cyc_Core_Opt*_tmp60B=_cycalloc(sizeof(*_tmp60B));
_tmp60B->v=(void*)Cyc_Absyn_exn_typ;_tmp60B;});_tmp60A->loc=0;_tmp60A;});struct
Cyc_Absyn_Exp*_tmp5FB=Cyc_Absyn_throw_exp(_tmp5F7,0);_tmp5FB->topt=({struct Cyc_Core_Opt*
_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->v=(void*)((void*)0);_tmp5FC;});{
struct Cyc_Absyn_Stmt*_tmp5FD=Cyc_Absyn_exp_stmt(_tmp5FB,0);struct Cyc_Absyn_Switch_clause*
_tmp5FE=({struct Cyc_Absyn_Switch_clause*_tmp607=_cycalloc(sizeof(*_tmp607));
_tmp607->pattern=_tmp5FA;_tmp607->pat_vars=({struct Cyc_Core_Opt*_tmp608=
_cycalloc(sizeof(*_tmp608));_tmp608->v=({struct Cyc_List_List*_tmp609=_cycalloc(
sizeof(*_tmp609));_tmp609->hd=_tmp5F8;_tmp609->tl=0;_tmp609;});_tmp608;});
_tmp607->where_clause=0;_tmp607->body=_tmp5FD;_tmp607->loc=0;_tmp607;});struct
Cyc_Absyn_Stmt*_tmp5FF=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5C4,({struct
Cyc_List_List*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->hd=_tmp5FE;_tmp606->tl=
0;_tmp606;})),0);Cyc_Toc_stmt_to_c(nv,_tmp5FF);{struct Cyc_Absyn_Exp*_tmp600=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp605=_cycalloc(sizeof(*_tmp605));
_tmp605->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp605->tl=0;
_tmp605;}),0);struct Cyc_Absyn_Stmt*_tmp601=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp604=_cycalloc(sizeof(*_tmp604));
_tmp604->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp604->tl=0;_tmp604;}),0),0);struct
Cyc_Absyn_Exp*_tmp602=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp603=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp601,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp602,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp600,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp603,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp5F5,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Toc_cast_it(
e_typ,Cyc_Toc__exn_thrown_e),_tmp5FF,0),0),0),0),0),0))->r));return;}}}}}_LL37F:
if(_tmp5A4 <= (void*)1)goto _LL381;if(*((int*)_tmp5A4)!= 15)goto _LL381;_tmp5C5=((
struct Cyc_Absyn_Region_s_struct*)_tmp5A4)->f1;_tmp5C6=((struct Cyc_Absyn_Region_s_struct*)
_tmp5A4)->f2;_tmp5C7=((struct Cyc_Absyn_Region_s_struct*)_tmp5A4)->f3;_tmp5C8=((
struct Cyc_Absyn_Region_s_struct*)_tmp5A4)->f4;_LL380: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp5C6->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp5C8);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp610->tl=0;_tmp610;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp611[1];_tmp611[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp611,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp5C8,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp612[1];_tmp612[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp612,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));return;}_LL381: if(
_tmp5A4 <= (void*)1)goto _LL35E;if(*((int*)_tmp5A4)!= 16)goto _LL35E;_tmp5C9=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmp5A4)->f1;_LL382: Cyc_Toc_exp_to_c(nv,
_tmp5C9);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp613=_cycalloc(sizeof(*_tmp613));
_tmp613->hd=_tmp5C9;_tmp613->tl=0;_tmp613;}),0),0))->r));return;_LL35E:;}}static
void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);struct _tuple16{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){f->tvs=0;f->effect=0;
f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{
struct Cyc_List_List*_tmp614=f->args;for(0;_tmp614 != 0;_tmp614=_tmp614->tl){
struct _tuple1*_tmp615=({struct _tuple1*_tmp616=_cycalloc(sizeof(*_tmp616));
_tmp616->f1=(void*)((void*)0);_tmp616->f2=(*((struct _tuple16*)_tmp614->hd)).f1;
_tmp616;});(*((struct _tuple16*)_tmp614->hd)).f3=Cyc_Toc_typ_to_c((*((struct
_tuple16*)_tmp614->hd)).f3);nv=Cyc_Toc_add_varmap(nv,_tmp615,Cyc_Absyn_var_exp(
_tmp615,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);return;}if((
unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name
!= 0){struct Cyc_Core_Opt*_tmp618;struct Cyc_Absyn_Tqual _tmp619;void*_tmp61A;int
_tmp61B;struct Cyc_Absyn_VarargInfo _tmp617=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp618=_tmp617.name;_tmp619=_tmp617.tq;_tmp61A=(
void*)_tmp617.type;_tmp61B=_tmp617.inject;{void*_tmp61C=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp61A,(void*)2,_tmp619,Cyc_Absyn_false_conref));struct _tuple1*_tmp61D=({struct
_tuple1*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->f1=(void*)((void*)0);
_tmp620->f2=(struct _dynforward_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp618))->v;
_tmp620;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp61E=_cycalloc(sizeof(*
_tmp61E));_tmp61E->hd=({struct _tuple16*_tmp61F=_cycalloc(sizeof(*_tmp61F));
_tmp61F->f1=(struct _dynforward_ptr*)_tmp618->v;_tmp61F->f2=_tmp619;_tmp61F->f3=
_tmp61C;_tmp61F;});_tmp61E->tl=0;_tmp61E;}));nv=Cyc_Toc_add_varmap(nv,_tmp61D,
Cyc_Absyn_var_exp(_tmp61D,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp621=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp621 != 0;_tmp621=_tmp621->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp621->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp621->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(nv),f->body);}
static void*Cyc_Toc_scope_to_c(void*s){void*_tmp622=s;_LL394: if((int)_tmp622 != 1)
goto _LL396;_LL395: return(void*)2;_LL396: if((int)_tmp622 != 4)goto _LL398;_LL397:
return(void*)3;_LL398:;_LL399: return s;_LL393:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=
0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp623=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*
_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp624;});{
int seen_defn_before;struct Cyc_Core_Opt*_tmp625=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp623);if(_tmp625 == 
0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(
Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,_tmp623,ad));}else{if(((
struct Cyc_Absyn_Aggrdecl*)_tmp625->v)->impl == 0){(struct Cyc_Dict_Dict*)(((struct
Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,
_tmp623,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(void*)((
void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po=
0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*_tmp626=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp626 != 0;_tmp626=_tmp626->tl){((struct
Cyc_Absyn_Aggrfield*)_tmp626->hd)->tq=Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)
_tmp626->hd)->type=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp626->hd)->type));}}}}}static struct Cyc_Core_Opt*Cyc_Toc_tunions_so_far=0;
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud){if(Cyc_Toc_tunions_so_far
== 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*_tmp627=_cycalloc(sizeof(*
_tmp627));_tmp627->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp627;});{struct _tuple1*_tmp628=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))
Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,
_tmp628))return;(struct Cyc_Set_Set*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(
void*)((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((
struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,
_tmp628));{struct Cyc_List_List*flat_structs=0;{struct Cyc_List_List*_tmp629=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;
_tmp629 != 0;_tmp629=_tmp629->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp629->hd;if(f->typs != 0  || tud->is_flat){struct Cyc_List_List*_tmp62A=0;int i=1;{
struct Cyc_List_List*_tmp62B=f->typs;for(0;_tmp62B != 0;(_tmp62B=_tmp62B->tl,i ++)){
struct _dynforward_ptr*_tmp62C=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmp62D=({struct Cyc_Absyn_Aggrfield*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->name=
_tmp62C;_tmp62F->tq=(*((struct _tuple4*)_tmp62B->hd)).f1;_tmp62F->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp62B->hd)).f2);_tmp62F->width=0;_tmp62F->attributes=0;_tmp62F;});
_tmp62A=({struct Cyc_List_List*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->hd=
_tmp62D;_tmp62E->tl=_tmp62A;_tmp62E;});}}_tmp62A=({struct Cyc_List_List*_tmp630=
_cycalloc(sizeof(*_tmp630));_tmp630->hd=({struct Cyc_Absyn_Aggrfield*_tmp631=
_cycalloc(sizeof(*_tmp631));_tmp631->name=Cyc_Toc_tag_sp;_tmp631->tq=Cyc_Toc_mt_tq;
_tmp631->type=(void*)Cyc_Absyn_sint_typ;_tmp631->width=0;_tmp631->attributes=0;
_tmp631;});_tmp630->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp62A);_tmp630;});{struct Cyc_Absyn_Aggrdecl*_tmp632=({struct Cyc_Absyn_Aggrdecl*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->kind=(void*)((void*)0);_tmp63F->sc=(
void*)((void*)2);_tmp63F->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp641="_struct";_tag_dynforward(_tmp641,sizeof(char),_get_zero_arr_size(
_tmp641,8));}));_tmp63F->tvs=0;_tmp63F->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->exist_vars=0;_tmp640->rgn_po=0;
_tmp640->fields=_tmp62A;_tmp640;});_tmp63F->attributes=0;_tmp63F;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp634=_cycalloc(sizeof(*_tmp634));
_tmp634[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp635;_tmp635.tag=4;_tmp635.f1=
_tmp632;_tmp635;});_tmp634;}),0);_tmp633->tl=Cyc_Toc_result_decls;_tmp633;});if(
tud->is_flat){struct Cyc_Absyn_Aggrfield*_tmp636=({struct Cyc_Absyn_Aggrfield*
_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->name=(*f->name).f2;_tmp638->tq=Cyc_Toc_mt_tq;
_tmp638->type=(void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp639=_cycalloc(
sizeof(*_tmp639));_tmp639[0]=({struct Cyc_Absyn_AggrType_struct _tmp63A;_tmp63A.tag=
10;_tmp63A.f1=({struct Cyc_Absyn_AggrInfo _tmp63B;_tmp63B.aggr_info=(void*)((void*)({
struct Cyc_Absyn_KnownAggr_struct*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C[0]=({
struct Cyc_Absyn_KnownAggr_struct _tmp63D;_tmp63D.tag=1;_tmp63D.f1=({struct Cyc_Absyn_Aggrdecl**
_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E[0]=_tmp632;_tmp63E;});_tmp63D;});
_tmp63C;}));_tmp63B.targs=0;_tmp63B;});_tmp63A;});_tmp639;}));_tmp638->width=0;
_tmp638->attributes=0;_tmp638;});flat_structs=({struct Cyc_List_List*_tmp637=
_cycalloc(sizeof(*_tmp637));_tmp637->hd=_tmp636;_tmp637->tl=flat_structs;_tmp637;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp642=({struct Cyc_Absyn_Aggrdecl*
_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646->kind=(void*)((void*)1);_tmp646->sc=(
void*)((void*)2);_tmp646->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp648="_union";_tag_dynforward(_tmp648,sizeof(char),_get_zero_arr_size(_tmp648,
7));}));_tmp646->tvs=0;_tmp646->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp647=
_cycalloc(sizeof(*_tmp647));_tmp647->exist_vars=0;_tmp647->rgn_po=0;_tmp647->fields=
flat_structs;_tmp647;});_tmp646->attributes=0;_tmp646;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp644=_cycalloc(sizeof(*_tmp644));
_tmp644[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp645;_tmp645.tag=4;_tmp645.f1=
_tmp642;_tmp645;});_tmp644;}),0);_tmp643->tl=Cyc_Toc_result_decls;_tmp643;});}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp649;});if(xd->fields == 0)return;{struct _tuple1*_tmp64A=xd->name;struct Cyc_List_List*
_tmp64B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp64B != 0;_tmp64B=_tmp64B->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp64B->hd;struct _dynforward_ptr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp64C=
Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(char))+ 4,0);void*_tmp64D=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp64C,Cyc_Absyn_false_conref,
0);struct Cyc_Core_Opt*_tmp64E=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,
struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);struct Cyc_Core_Opt
_tmp64F;int _tmp650;_LL39B: if(_tmp64E != 0)goto _LL39D;_LL39C: {struct Cyc_Absyn_Exp*
initopt=0;if((void*)f->sc != (void*)3){char zero='\000';initopt=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct Cyc_String_pa_struct _tmp657;
_tmp657.tag=0;_tmp657.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn);{
struct Cyc_Int_pa_struct _tmp656;_tmp656.tag=1;_tmp656.f1=(unsigned long)((int)
zero);{struct Cyc_Int_pa_struct _tmp655;_tmp655.tag=1;_tmp655.f1=(unsigned long)((
int)zero);{struct Cyc_Int_pa_struct _tmp654;_tmp654.tag=1;_tmp654.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp653;_tmp653.tag=1;_tmp653.f1=(
unsigned long)((int)zero);{void*_tmp651[5]={& _tmp653,& _tmp654,& _tmp655,& _tmp656,&
_tmp657};Cyc_aprintf(({const char*_tmp652="%c%c%c%c%s";_tag_dynforward(_tmp652,
sizeof(char),_get_zero_arr_size(_tmp652,11));}),_tag_dynforward(_tmp651,sizeof(
void*),5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*_tmp658=Cyc_Absyn_new_vardecl(f->name,
_tmp64D,initopt);(void*)(_tmp658->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A[
0]=({struct Cyc_Absyn_Var_d_struct _tmp65B;_tmp65B.tag=0;_tmp65B.f1=_tmp658;
_tmp65B;});_tmp65A;}),0);_tmp659->tl=Cyc_Toc_result_decls;_tmp659;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp65C=f->typs;for(0;_tmp65C != 0;(_tmp65C=_tmp65C->tl,
i ++)){struct _dynforward_ptr*_tmp65D=({struct _dynforward_ptr*_tmp661=_cycalloc(
sizeof(*_tmp661));_tmp661[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp664;_tmp664.tag=1;_tmp664.f1=(unsigned long)i;{void*_tmp662[1]={& _tmp664};
Cyc_aprintf(({const char*_tmp663="f%d";_tag_dynforward(_tmp663,sizeof(char),
_get_zero_arr_size(_tmp663,4));}),_tag_dynforward(_tmp662,sizeof(void*),1));}});
_tmp661;});struct Cyc_Absyn_Aggrfield*_tmp65E=({struct Cyc_Absyn_Aggrfield*_tmp660=
_cycalloc(sizeof(*_tmp660));_tmp660->name=_tmp65D;_tmp660->tq=(*((struct _tuple4*)
_tmp65C->hd)).f1;_tmp660->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp65C->hd)).f2);_tmp660->width=0;_tmp660->attributes=0;_tmp660;});fields=({
struct Cyc_List_List*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->hd=_tmp65E;
_tmp65F->tl=fields;_tmp65F;});}}fields=({struct Cyc_List_List*_tmp665=_cycalloc(
sizeof(*_tmp665));_tmp665->hd=({struct Cyc_Absyn_Aggrfield*_tmp666=_cycalloc(
sizeof(*_tmp666));_tmp666->name=Cyc_Toc_tag_sp;_tmp666->tq=Cyc_Toc_mt_tq;_tmp666->type=(
void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp666->width=0;
_tmp666->attributes=0;_tmp666;});_tmp665->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp665;});{struct Cyc_Absyn_Aggrdecl*_tmp667=({
struct Cyc_Absyn_Aggrdecl*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->kind=(void*)((
void*)0);_tmp66B->sc=(void*)((void*)2);_tmp66B->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp66D="_struct";_tag_dynforward(_tmp66D,sizeof(char),
_get_zero_arr_size(_tmp66D,8));}));_tmp66B->tvs=0;_tmp66B->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C->exist_vars=0;_tmp66C->rgn_po=0;
_tmp66C->fields=fields;_tmp66C;});_tmp66B->attributes=0;_tmp66B;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp669=_cycalloc(sizeof(*_tmp669));
_tmp669[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp66A;_tmp66A.tag=4;_tmp66A.f1=
_tmp667;_tmp66A;});_tmp669;}),0);_tmp668->tl=Cyc_Toc_result_decls;_tmp668;});}}
goto _LL39A;}}_LL39D: if(_tmp64E == 0)goto _LL39F;_tmp64F=*_tmp64E;_tmp650=(int)
_tmp64F.v;if(_tmp650 != 0)goto _LL39F;_LL39E: if((void*)f->sc != (void*)3){char zero='\000';
struct Cyc_Absyn_Exp*_tmp66E=Cyc_Absyn_string_exp((struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp679;_tmp679.tag=0;_tmp679.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*fn);{struct Cyc_Int_pa_struct _tmp678;_tmp678.tag=1;_tmp678.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp677;_tmp677.tag=1;_tmp677.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp676;_tmp676.tag=1;_tmp676.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp675;_tmp675.tag=1;_tmp675.f1=(
unsigned long)((int)zero);{void*_tmp673[5]={& _tmp675,& _tmp676,& _tmp677,& _tmp678,&
_tmp679};Cyc_aprintf(({const char*_tmp674="%c%c%c%c%s";_tag_dynforward(_tmp674,
sizeof(char),_get_zero_arr_size(_tmp674,11));}),_tag_dynforward(_tmp673,sizeof(
void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp66F=Cyc_Absyn_new_vardecl(f->name,
_tmp64D,(struct Cyc_Absyn_Exp*)_tmp66E);(void*)(_tmp66F->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp670=_cycalloc(sizeof(*_tmp670));
_tmp670->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp671=
_cycalloc(sizeof(*_tmp671));_tmp671[0]=({struct Cyc_Absyn_Var_d_struct _tmp672;
_tmp672.tag=0;_tmp672.f1=_tmp66F;_tmp672;});_tmp671;}),0);_tmp670->tl=Cyc_Toc_result_decls;
_tmp670;});(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL39A;_LL39F:;_LL3A0: goto _LL39A;_LL39A:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));{int forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp67A=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp67B;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67D;int _tmp67E;
_LL3A2: if(*((int*)_tmp67A)!= 29)goto _LL3A4;_tmp67B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp67A)->f1;_tmp67C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp67A)->f2;
_tmp67D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp67A)->f3;_tmp67E=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp67A)->f4;_LL3A3: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp67B,_tmp67C,_tmp67D,_tmp67E,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL3A1;_LL3A4:;_LL3A5: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp67F=({
struct Cyc_Toc_Env*_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=*init_nv;
_tmp680;});_tmp67F->toplevel=1;Cyc_Toc_exp_to_c(_tmp67F,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL3A1;_LL3A1:;}else{void*_tmp681=Cyc_Tcutil_compress(old_typ);
struct Cyc_Absyn_ArrayInfo _tmp682;void*_tmp683;struct Cyc_Absyn_Exp*_tmp684;struct
Cyc_Absyn_Conref*_tmp685;_LL3A7: if(_tmp681 <= (void*)4)goto _LL3A9;if(*((int*)
_tmp681)!= 7)goto _LL3A9;_tmp682=((struct Cyc_Absyn_ArrayType_struct*)_tmp681)->f1;
_tmp683=(void*)_tmp682.elt_type;_tmp684=_tmp682.num_elts;_tmp685=_tmp682.zero_term;
if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp685))goto
_LL3A9;_LL3A8: if(_tmp684 == 0)({void*_tmp686[0]={};((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp687="can't initialize zero-terminated array -- size unknown";
_tag_dynforward(_tmp687,sizeof(char),_get_zero_arr_size(_tmp687,55));}),
_tag_dynforward(_tmp686,sizeof(void*),0));});{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp684;struct Cyc_Absyn_Exp*_tmp688=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp689=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp688,_tmp689,0),0),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));goto _LL3A6;}
_LL3A9:;_LL3AA: goto _LL3A6;_LL3A6:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp68A=_cycalloc(
sizeof(*_tmp68A));_tmp68A[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp68A;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp68B=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp68B;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp68C=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp68E;struct Cyc_List_List*
_tmp68F;struct Cyc_Absyn_Stmt*_tmp690;struct _tuple12 _tmp68D=Cyc_Toc_xlate_pat(nv,
prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)&
_tmp68C,Cyc_Toc_throw_match_stmt(),0);_tmp68E=_tmp68D.f1;_tmp68F=_tmp68D.f2;
_tmp690=_tmp68D.f3;Cyc_Toc_stmt_to_c(_tmp68E,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp690,s,0),0);for(0;_tmp68F != 0;
_tmp68F=_tmp68F->tl){struct _tuple13 _tmp692;struct _tuple1*_tmp693;void*_tmp694;
struct _tuple13*_tmp691=(struct _tuple13*)_tmp68F->hd;_tmp692=*_tmp691;_tmp693=
_tmp692.f1;_tmp694=_tmp692.f2;s=Cyc_Absyn_declare_stmt(_tmp693,_tmp694,0,s,0);}};
_pop_region(prgn);}return s;}}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*
e){void*_tmp695=(void*)e->r;struct Cyc_Absyn_Exp*_tmp696;struct Cyc_Absyn_Exp*
_tmp697;struct Cyc_Absyn_Exp*_tmp698;struct Cyc_Absyn_Exp*_tmp699;struct Cyc_Absyn_Exp*
_tmp69A;struct Cyc_Absyn_Exp*_tmp69B;struct Cyc_Absyn_Exp*_tmp69C;struct Cyc_Absyn_Exp*
_tmp69D;struct Cyc_List_List*_tmp69E;struct Cyc_Absyn_Exp*_tmp69F;struct Cyc_Absyn_Exp*
_tmp6A0;struct Cyc_Absyn_Exp*_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A2;struct Cyc_Absyn_Exp*
_tmp6A3;struct Cyc_Absyn_Exp*_tmp6A4;struct Cyc_Absyn_Exp*_tmp6A5;struct Cyc_Absyn_Exp*
_tmp6A6;struct Cyc_Absyn_Exp*_tmp6A7;struct Cyc_Absyn_Exp*_tmp6A8;struct Cyc_Absyn_Exp*
_tmp6A9;struct Cyc_Absyn_Exp*_tmp6AA;struct Cyc_Absyn_Exp*_tmp6AB;struct Cyc_Absyn_Exp*
_tmp6AC;struct Cyc_List_List*_tmp6AD;struct Cyc_Absyn_Exp*_tmp6AE;struct Cyc_List_List*
_tmp6AF;void*_tmp6B0;void**_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B2;struct _tuple2*
_tmp6B3;struct _tuple2 _tmp6B4;void*_tmp6B5;void**_tmp6B6;struct Cyc_List_List*
_tmp6B7;struct Cyc_List_List*_tmp6B8;struct Cyc_List_List*_tmp6B9;void*_tmp6BA;
void**_tmp6BB;void*_tmp6BC;void**_tmp6BD;struct Cyc_Absyn_Stmt*_tmp6BE;struct Cyc_Absyn_MallocInfo
_tmp6BF;struct Cyc_Absyn_MallocInfo*_tmp6C0;_LL3AC: if(*((int*)_tmp695)!= 22)goto
_LL3AE;_tmp696=((struct Cyc_Absyn_Deref_e_struct*)_tmp695)->f1;_LL3AD: _tmp697=
_tmp696;goto _LL3AF;_LL3AE: if(*((int*)_tmp695)!= 23)goto _LL3B0;_tmp697=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp695)->f1;_LL3AF: _tmp698=_tmp697;goto _LL3B1;
_LL3B0: if(*((int*)_tmp695)!= 24)goto _LL3B2;_tmp698=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp695)->f1;_LL3B1: _tmp699=_tmp698;goto _LL3B3;_LL3B2: if(*((int*)_tmp695)!= 16)
goto _LL3B4;_tmp699=((struct Cyc_Absyn_Address_e_struct*)_tmp695)->f1;_LL3B3:
_tmp69A=_tmp699;goto _LL3B5;_LL3B4: if(*((int*)_tmp695)!= 12)goto _LL3B6;_tmp69A=((
struct Cyc_Absyn_Throw_e_struct*)_tmp695)->f1;_LL3B5: _tmp69B=_tmp69A;goto _LL3B7;
_LL3B6: if(*((int*)_tmp695)!= 13)goto _LL3B8;_tmp69B=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp695)->f1;_LL3B7: _tmp69C=_tmp69B;goto _LL3B9;_LL3B8: if(*((int*)_tmp695)!= 19)
goto _LL3BA;_tmp69C=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp695)->f1;_LL3B9:
_tmp69D=_tmp69C;goto _LL3BB;_LL3BA: if(*((int*)_tmp695)!= 5)goto _LL3BC;_tmp69D=((
struct Cyc_Absyn_Increment_e_struct*)_tmp695)->f1;_LL3BB: Cyc_Toc_exptypes_to_c(
_tmp69D);goto _LL3AB;_LL3BC: if(*((int*)_tmp695)!= 3)goto _LL3BE;_tmp69E=((struct
Cyc_Absyn_Primop_e_struct*)_tmp695)->f2;_LL3BD:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp69E);goto _LL3AB;
_LL3BE: if(*((int*)_tmp695)!= 7)goto _LL3C0;_tmp69F=((struct Cyc_Absyn_And_e_struct*)
_tmp695)->f1;_tmp6A0=((struct Cyc_Absyn_And_e_struct*)_tmp695)->f2;_LL3BF: _tmp6A1=
_tmp69F;_tmp6A2=_tmp6A0;goto _LL3C1;_LL3C0: if(*((int*)_tmp695)!= 8)goto _LL3C2;
_tmp6A1=((struct Cyc_Absyn_Or_e_struct*)_tmp695)->f1;_tmp6A2=((struct Cyc_Absyn_Or_e_struct*)
_tmp695)->f2;_LL3C1: _tmp6A3=_tmp6A1;_tmp6A4=_tmp6A2;goto _LL3C3;_LL3C2: if(*((int*)
_tmp695)!= 9)goto _LL3C4;_tmp6A3=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp695)->f1;
_tmp6A4=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp695)->f2;_LL3C3: _tmp6A5=_tmp6A3;
_tmp6A6=_tmp6A4;goto _LL3C5;_LL3C4: if(*((int*)_tmp695)!= 25)goto _LL3C6;_tmp6A5=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp695)->f1;_tmp6A6=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp695)->f2;_LL3C5: _tmp6A7=_tmp6A5;_tmp6A8=_tmp6A6;goto _LL3C7;_LL3C6: if(*((int*)
_tmp695)!= 4)goto _LL3C8;_tmp6A7=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp695)->f1;
_tmp6A8=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp695)->f3;_LL3C7: Cyc_Toc_exptypes_to_c(
_tmp6A7);Cyc_Toc_exptypes_to_c(_tmp6A8);goto _LL3AB;_LL3C8: if(*((int*)_tmp695)!= 
6)goto _LL3CA;_tmp6A9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp695)->f1;
_tmp6AA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp695)->f2;_tmp6AB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp695)->f3;_LL3C9: Cyc_Toc_exptypes_to_c(_tmp6A9);Cyc_Toc_exptypes_to_c(_tmp6AA);
Cyc_Toc_exptypes_to_c(_tmp6AB);goto _LL3AB;_LL3CA: if(*((int*)_tmp695)!= 11)goto
_LL3CC;_tmp6AC=((struct Cyc_Absyn_FnCall_e_struct*)_tmp695)->f1;_tmp6AD=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp695)->f2;_LL3CB: _tmp6AE=_tmp6AC;_tmp6AF=_tmp6AD;
goto _LL3CD;_LL3CC: if(*((int*)_tmp695)!= 10)goto _LL3CE;_tmp6AE=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp695)->f1;_tmp6AF=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp695)->f2;_LL3CD:
Cyc_Toc_exptypes_to_c(_tmp6AE);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6AF);goto _LL3AB;_LL3CE:
if(*((int*)_tmp695)!= 15)goto _LL3D0;_tmp6B0=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp695)->f1;_tmp6B1=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp695)->f1);
_tmp6B2=((struct Cyc_Absyn_Cast_e_struct*)_tmp695)->f2;_LL3CF:*_tmp6B1=Cyc_Toc_typ_to_c(*
_tmp6B1);Cyc_Toc_exptypes_to_c(_tmp6B2);goto _LL3AB;_LL3D0: if(*((int*)_tmp695)!= 
27)goto _LL3D2;_tmp6B3=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp695)->f1;
_tmp6B4=*_tmp6B3;_tmp6B5=_tmp6B4.f3;_tmp6B6=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp695)->f1).f3;_tmp6B7=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp695)->f2;
_LL3D1:*_tmp6B6=Cyc_Toc_typ_to_c(*_tmp6B6);_tmp6B8=_tmp6B7;goto _LL3D3;_LL3D2: if(*((
int*)_tmp695)!= 36)goto _LL3D4;_tmp6B8=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp695)->f2;_LL3D3: _tmp6B9=_tmp6B8;goto _LL3D5;_LL3D4: if(*((int*)_tmp695)!= 28)
goto _LL3D6;_tmp6B9=((struct Cyc_Absyn_Array_e_struct*)_tmp695)->f1;_LL3D5: for(0;
_tmp6B9 != 0;_tmp6B9=_tmp6B9->tl){struct Cyc_Absyn_Exp*_tmp6C2;struct _tuple6
_tmp6C1=*((struct _tuple6*)_tmp6B9->hd);_tmp6C2=_tmp6C1.f2;Cyc_Toc_exptypes_to_c(
_tmp6C2);}goto _LL3AB;_LL3D6: if(*((int*)_tmp695)!= 20)goto _LL3D8;_tmp6BA=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp695)->f1;_tmp6BB=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp695)->f1);_LL3D7: _tmp6BD=_tmp6BB;goto
_LL3D9;_LL3D8: if(*((int*)_tmp695)!= 18)goto _LL3DA;_tmp6BC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp695)->f1;_tmp6BD=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp695)->f1);_LL3D9:*_tmp6BD=Cyc_Toc_typ_to_c(*_tmp6BD);goto _LL3AB;_LL3DA: if(*((
int*)_tmp695)!= 37)goto _LL3DC;_tmp6BE=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp695)->f1;_LL3DB: Cyc_Toc_stmttypes_to_c(_tmp6BE);goto _LL3AB;_LL3DC: if(*((int*)
_tmp695)!= 35)goto _LL3DE;_tmp6BF=((struct Cyc_Absyn_Malloc_e_struct*)_tmp695)->f1;
_tmp6C0=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp695)->f1;
_LL3DD: if(_tmp6C0->elt_type != 0)_tmp6C0->elt_type=({void**_tmp6C3=_cycalloc(
sizeof(*_tmp6C3));_tmp6C3[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp6C0->elt_type)));
_tmp6C3;});Cyc_Toc_exptypes_to_c(_tmp6C0->num_elts);goto _LL3AB;_LL3DE: if(*((int*)
_tmp695)!= 0)goto _LL3E0;_LL3DF: goto _LL3E1;_LL3E0: if(*((int*)_tmp695)!= 1)goto
_LL3E2;_LL3E1: goto _LL3E3;_LL3E2: if(*((int*)_tmp695)!= 2)goto _LL3E4;_LL3E3: goto
_LL3E5;_LL3E4: if(*((int*)_tmp695)!= 33)goto _LL3E6;_LL3E5: goto _LL3E7;_LL3E6: if(*((
int*)_tmp695)!= 34)goto _LL3E8;_LL3E7: goto _LL3AB;_LL3E8: if(*((int*)_tmp695)!= 31)
goto _LL3EA;_LL3E9: goto _LL3EB;_LL3EA: if(*((int*)_tmp695)!= 32)goto _LL3EC;_LL3EB:
goto _LL3ED;_LL3EC: if(*((int*)_tmp695)!= 30)goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE:
if(*((int*)_tmp695)!= 29)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(*((int*)_tmp695)
!= 26)goto _LL3F2;_LL3F1: goto _LL3F3;_LL3F2: if(*((int*)_tmp695)!= 14)goto _LL3F4;
_LL3F3: goto _LL3F5;_LL3F4: if(*((int*)_tmp695)!= 17)goto _LL3F6;_LL3F5: goto _LL3F7;
_LL3F6: if(*((int*)_tmp695)!= 21)goto _LL3AB;_LL3F7:({void*_tmp6C4[0]={};Cyc_Tcutil_terr(
e->loc,({const char*_tmp6C5="Cyclone expression within C code";_tag_dynforward(
_tmp6C5,sizeof(char),_get_zero_arr_size(_tmp6C5,33));}),_tag_dynforward(_tmp6C4,
sizeof(void*),0));});goto _LL3AB;_LL3AB:;}static void Cyc_Toc_decltypes_to_c(struct
Cyc_Absyn_Decl*d){void*_tmp6C6=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp6C7;
struct Cyc_Absyn_Fndecl*_tmp6C8;struct Cyc_Absyn_Aggrdecl*_tmp6C9;struct Cyc_Absyn_Enumdecl*
_tmp6CA;struct Cyc_Absyn_Typedefdecl*_tmp6CB;_LL3F9: if(_tmp6C6 <= (void*)2)goto
_LL411;if(*((int*)_tmp6C6)!= 0)goto _LL3FB;_tmp6C7=((struct Cyc_Absyn_Var_d_struct*)
_tmp6C6)->f1;_LL3FA:(void*)(_tmp6C7->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp6C7->type));
if(_tmp6C7->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp6C7->initializer));goto _LL3F8;_LL3FB: if(*((int*)_tmp6C6)!= 1)
goto _LL3FD;_tmp6C8=((struct Cyc_Absyn_Fn_d_struct*)_tmp6C6)->f1;_LL3FC:(void*)(
_tmp6C8->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp6C8->ret_type));{struct Cyc_List_List*
_tmp6CC=_tmp6C8->args;for(0;_tmp6CC != 0;_tmp6CC=_tmp6CC->tl){(*((struct _tuple16*)
_tmp6CC->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp6CC->hd)).f3);}}goto
_LL3F8;_LL3FD: if(*((int*)_tmp6C6)!= 4)goto _LL3FF;_tmp6C9=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp6C6)->f1;_LL3FE: Cyc_Toc_aggrdecl_to_c(_tmp6C9);goto _LL3F8;_LL3FF: if(*((int*)
_tmp6C6)!= 6)goto _LL401;_tmp6CA=((struct Cyc_Absyn_Enum_d_struct*)_tmp6C6)->f1;
_LL400: if(_tmp6CA->fields != 0){struct Cyc_List_List*_tmp6CD=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp6CA->fields))->v;for(0;_tmp6CD != 0;_tmp6CD=
_tmp6CD->tl){struct Cyc_Absyn_Enumfield*_tmp6CE=(struct Cyc_Absyn_Enumfield*)
_tmp6CD->hd;if(_tmp6CE->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp6CE->tag));}}goto _LL3F8;_LL401: if(*((int*)_tmp6C6)!= 7)goto
_LL403;_tmp6CB=((struct Cyc_Absyn_Typedef_d_struct*)_tmp6C6)->f1;_LL402: _tmp6CB->defn=({
struct Cyc_Core_Opt*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6CB->defn))->v);_tmp6CF;});goto
_LL3F8;_LL403: if(*((int*)_tmp6C6)!= 2)goto _LL405;_LL404: goto _LL406;_LL405: if(*((
int*)_tmp6C6)!= 3)goto _LL407;_LL406: goto _LL408;_LL407: if(*((int*)_tmp6C6)!= 5)
goto _LL409;_LL408: goto _LL40A;_LL409: if(*((int*)_tmp6C6)!= 8)goto _LL40B;_LL40A:
goto _LL40C;_LL40B: if(*((int*)_tmp6C6)!= 9)goto _LL40D;_LL40C: goto _LL40E;_LL40D:
if(*((int*)_tmp6C6)!= 10)goto _LL40F;_LL40E: goto _LL410;_LL40F: if(*((int*)_tmp6C6)
!= 11)goto _LL411;_LL410:({void*_tmp6D0[0]={};Cyc_Tcutil_terr(d->loc,({const char*
_tmp6D1="Cyclone declaration within C code";_tag_dynforward(_tmp6D1,sizeof(char),
_get_zero_arr_size(_tmp6D1,34));}),_tag_dynforward(_tmp6D0,sizeof(void*),0));});
goto _LL3F8;_LL411: if((int)_tmp6C6 != 0)goto _LL413;_LL412: goto _LL414;_LL413: if((
int)_tmp6C6 != 1)goto _LL3F8;_LL414: goto _LL3F8;_LL3F8:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp6D2=(void*)s->r;struct Cyc_Absyn_Exp*_tmp6D3;
struct Cyc_Absyn_Stmt*_tmp6D4;struct Cyc_Absyn_Stmt*_tmp6D5;struct Cyc_Absyn_Exp*
_tmp6D6;struct Cyc_Absyn_Exp*_tmp6D7;struct Cyc_Absyn_Stmt*_tmp6D8;struct Cyc_Absyn_Stmt*
_tmp6D9;struct _tuple3 _tmp6DA;struct Cyc_Absyn_Exp*_tmp6DB;struct Cyc_Absyn_Stmt*
_tmp6DC;struct Cyc_Absyn_Exp*_tmp6DD;struct _tuple3 _tmp6DE;struct Cyc_Absyn_Exp*
_tmp6DF;struct _tuple3 _tmp6E0;struct Cyc_Absyn_Exp*_tmp6E1;struct Cyc_Absyn_Stmt*
_tmp6E2;struct Cyc_Absyn_Exp*_tmp6E3;struct Cyc_List_List*_tmp6E4;struct Cyc_Absyn_Decl*
_tmp6E5;struct Cyc_Absyn_Stmt*_tmp6E6;struct Cyc_Absyn_Stmt*_tmp6E7;struct _tuple3
_tmp6E8;struct Cyc_Absyn_Exp*_tmp6E9;_LL416: if(_tmp6D2 <= (void*)1)goto _LL428;if(*((
int*)_tmp6D2)!= 0)goto _LL418;_tmp6D3=((struct Cyc_Absyn_Exp_s_struct*)_tmp6D2)->f1;
_LL417: Cyc_Toc_exptypes_to_c(_tmp6D3);goto _LL415;_LL418: if(*((int*)_tmp6D2)!= 1)
goto _LL41A;_tmp6D4=((struct Cyc_Absyn_Seq_s_struct*)_tmp6D2)->f1;_tmp6D5=((struct
Cyc_Absyn_Seq_s_struct*)_tmp6D2)->f2;_LL419: Cyc_Toc_stmttypes_to_c(_tmp6D4);Cyc_Toc_stmttypes_to_c(
_tmp6D5);goto _LL415;_LL41A: if(*((int*)_tmp6D2)!= 2)goto _LL41C;_tmp6D6=((struct
Cyc_Absyn_Return_s_struct*)_tmp6D2)->f1;_LL41B: if(_tmp6D6 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp6D6);goto _LL415;_LL41C: if(*((int*)_tmp6D2)!= 3)goto
_LL41E;_tmp6D7=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6D2)->f1;_tmp6D8=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6D2)->f2;_tmp6D9=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp6D2)->f3;_LL41D: Cyc_Toc_exptypes_to_c(_tmp6D7);Cyc_Toc_stmttypes_to_c(
_tmp6D8);Cyc_Toc_stmttypes_to_c(_tmp6D9);goto _LL415;_LL41E: if(*((int*)_tmp6D2)!= 
4)goto _LL420;_tmp6DA=((struct Cyc_Absyn_While_s_struct*)_tmp6D2)->f1;_tmp6DB=
_tmp6DA.f1;_tmp6DC=((struct Cyc_Absyn_While_s_struct*)_tmp6D2)->f2;_LL41F: Cyc_Toc_exptypes_to_c(
_tmp6DB);Cyc_Toc_stmttypes_to_c(_tmp6DC);goto _LL415;_LL420: if(*((int*)_tmp6D2)!= 
8)goto _LL422;_tmp6DD=((struct Cyc_Absyn_For_s_struct*)_tmp6D2)->f1;_tmp6DE=((
struct Cyc_Absyn_For_s_struct*)_tmp6D2)->f2;_tmp6DF=_tmp6DE.f1;_tmp6E0=((struct
Cyc_Absyn_For_s_struct*)_tmp6D2)->f3;_tmp6E1=_tmp6E0.f1;_tmp6E2=((struct Cyc_Absyn_For_s_struct*)
_tmp6D2)->f4;_LL421: Cyc_Toc_exptypes_to_c(_tmp6DD);Cyc_Toc_exptypes_to_c(_tmp6DF);
Cyc_Toc_exptypes_to_c(_tmp6E1);Cyc_Toc_stmttypes_to_c(_tmp6E2);goto _LL415;_LL422:
if(*((int*)_tmp6D2)!= 9)goto _LL424;_tmp6E3=((struct Cyc_Absyn_Switch_s_struct*)
_tmp6D2)->f1;_tmp6E4=((struct Cyc_Absyn_Switch_s_struct*)_tmp6D2)->f2;_LL423: Cyc_Toc_exptypes_to_c(
_tmp6E3);for(0;_tmp6E4 != 0;_tmp6E4=_tmp6E4->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp6E4->hd)->body);}goto _LL415;_LL424: if(*((int*)
_tmp6D2)!= 11)goto _LL426;_tmp6E5=((struct Cyc_Absyn_Decl_s_struct*)_tmp6D2)->f1;
_tmp6E6=((struct Cyc_Absyn_Decl_s_struct*)_tmp6D2)->f2;_LL425: Cyc_Toc_decltypes_to_c(
_tmp6E5);Cyc_Toc_stmttypes_to_c(_tmp6E6);goto _LL415;_LL426: if(*((int*)_tmp6D2)!= 
13)goto _LL428;_tmp6E7=((struct Cyc_Absyn_Do_s_struct*)_tmp6D2)->f1;_tmp6E8=((
struct Cyc_Absyn_Do_s_struct*)_tmp6D2)->f2;_tmp6E9=_tmp6E8.f1;_LL427: Cyc_Toc_stmttypes_to_c(
_tmp6E7);Cyc_Toc_exptypes_to_c(_tmp6E9);goto _LL415;_LL428: if((int)_tmp6D2 != 0)
goto _LL42A;_LL429: goto _LL42B;_LL42A: if(_tmp6D2 <= (void*)1)goto _LL42C;if(*((int*)
_tmp6D2)!= 5)goto _LL42C;_LL42B: goto _LL42D;_LL42C: if(_tmp6D2 <= (void*)1)goto
_LL42E;if(*((int*)_tmp6D2)!= 6)goto _LL42E;_LL42D: goto _LL42F;_LL42E: if(_tmp6D2 <= (
void*)1)goto _LL430;if(*((int*)_tmp6D2)!= 7)goto _LL430;_LL42F: goto _LL415;_LL430:
if(_tmp6D2 <= (void*)1)goto _LL432;if(*((int*)_tmp6D2)!= 10)goto _LL432;_LL431: goto
_LL433;_LL432: if(_tmp6D2 <= (void*)1)goto _LL434;if(*((int*)_tmp6D2)!= 12)goto
_LL434;_LL433: goto _LL435;_LL434: if(_tmp6D2 <= (void*)1)goto _LL436;if(*((int*)
_tmp6D2)!= 14)goto _LL436;_LL435: goto _LL437;_LL436: if(_tmp6D2 <= (void*)1)goto
_LL438;if(*((int*)_tmp6D2)!= 15)goto _LL438;_LL437: goto _LL439;_LL438: if(_tmp6D2 <= (
void*)1)goto _LL415;if(*((int*)_tmp6D2)!= 16)goto _LL415;_LL439:({void*_tmp6EA[0]={};
Cyc_Tcutil_terr(s->loc,({const char*_tmp6EB="Cyclone statement in C code";
_tag_dynforward(_tmp6EB,sizeof(char),_get_zero_arr_size(_tmp6EB,28));}),
_tag_dynforward(_tmp6EA,sizeof(void*),0));});goto _LL415;_LL415:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int
cinclude){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp6EC[0]={};((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp6ED="decls_to_c: not at toplevel!";_tag_dynforward(_tmp6ED,sizeof(char),
_get_zero_arr_size(_tmp6ED,29));}),_tag_dynforward(_tmp6EC,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp6EE=(void*)d->r;
struct Cyc_Absyn_Vardecl*_tmp6EF;struct Cyc_Absyn_Fndecl*_tmp6F0;struct Cyc_Absyn_Aggrdecl*
_tmp6F1;struct Cyc_Absyn_Tuniondecl*_tmp6F2;struct Cyc_Absyn_Enumdecl*_tmp6F3;
struct Cyc_Absyn_Typedefdecl*_tmp6F4;struct Cyc_List_List*_tmp6F5;struct Cyc_List_List*
_tmp6F6;struct Cyc_List_List*_tmp6F7;struct Cyc_List_List*_tmp6F8;_LL43B: if(
_tmp6EE <= (void*)2)goto _LL44B;if(*((int*)_tmp6EE)!= 0)goto _LL43D;_tmp6EF=((
struct Cyc_Absyn_Var_d_struct*)_tmp6EE)->f1;_LL43C: {struct _tuple1*_tmp6F9=
_tmp6EF->name;if((void*)_tmp6EF->sc == (void*)4)_tmp6F9=({struct _tuple1*_tmp6FA=
_cycalloc(sizeof(*_tmp6FA));_tmp6FA->f1=Cyc_Absyn_rel_ns_null;_tmp6FA->f2=(*
_tmp6F9).f2;_tmp6FA;});if(_tmp6EF->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp6EF->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp6EF->initializer));}}nv=Cyc_Toc_add_varmap(
nv,_tmp6EF->name,Cyc_Absyn_varb_exp(_tmp6F9,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_Global_b_struct
_tmp6FC;_tmp6FC.tag=0;_tmp6FC.f1=_tmp6EF;_tmp6FC;});_tmp6FB;}),0));_tmp6EF->name=
_tmp6F9;(void*)(_tmp6EF->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp6EF->sc));(void*)(
_tmp6EF->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp6EF->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD->hd=d;_tmp6FD->tl=
Cyc_Toc_result_decls;_tmp6FD;});goto _LL43A;}_LL43D: if(*((int*)_tmp6EE)!= 1)goto
_LL43F;_tmp6F0=((struct Cyc_Absyn_Fn_d_struct*)_tmp6EE)->f1;_LL43E: {struct
_tuple1*_tmp6FE=_tmp6F0->name;if((void*)_tmp6F0->sc == (void*)4){_tmp6FE=({struct
_tuple1*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF->f1=Cyc_Absyn_rel_ns_null;
_tmp6FF->f2=(*_tmp6FE).f2;_tmp6FF;});(void*)(_tmp6F0->sc=(void*)((void*)2));}nv=
Cyc_Toc_add_varmap(nv,_tmp6F0->name,Cyc_Absyn_var_exp(_tmp6FE,0));_tmp6F0->name=
_tmp6FE;Cyc_Toc_fndecl_to_c(nv,_tmp6F0,cinclude);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700->hd=d;_tmp700->tl=Cyc_Toc_result_decls;
_tmp700;});goto _LL43A;}_LL43F: if(*((int*)_tmp6EE)!= 2)goto _LL441;_LL440: goto
_LL442;_LL441: if(*((int*)_tmp6EE)!= 3)goto _LL443;_LL442:({void*_tmp701[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp702="letdecl at toplevel";_tag_dynforward(_tmp702,sizeof(char),
_get_zero_arr_size(_tmp702,20));}),_tag_dynforward(_tmp701,sizeof(void*),0));});
_LL443: if(*((int*)_tmp6EE)!= 4)goto _LL445;_tmp6F1=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp6EE)->f1;_LL444: Cyc_Toc_aggrdecl_to_c(_tmp6F1);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703->hd=d;_tmp703->tl=Cyc_Toc_result_decls;
_tmp703;});goto _LL43A;_LL445: if(*((int*)_tmp6EE)!= 5)goto _LL447;_tmp6F2=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp6EE)->f1;_LL446: if(_tmp6F2->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp6F2);else{Cyc_Toc_tuniondecl_to_c(_tmp6F2);}goto _LL43A;_LL447: if(*((int*)
_tmp6EE)!= 6)goto _LL449;_tmp6F3=((struct Cyc_Absyn_Enum_d_struct*)_tmp6EE)->f1;
_LL448: Cyc_Toc_enumdecl_to_c(nv,_tmp6F3);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704->hd=d;_tmp704->tl=Cyc_Toc_result_decls;
_tmp704;});goto _LL43A;_LL449: if(*((int*)_tmp6EE)!= 7)goto _LL44B;_tmp6F4=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp6EE)->f1;_LL44A: _tmp6F4->name=_tmp6F4->name;
_tmp6F4->tvs=0;if(_tmp6F4->defn != 0)_tmp6F4->defn=({struct Cyc_Core_Opt*_tmp705=
_cycalloc(sizeof(*_tmp705));_tmp705->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp6F4->defn))->v);_tmp705;});else{void*_tmp706=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6F4->kind))->v;_LL458: if((int)
_tmp706 != 2)goto _LL45A;_LL459: _tmp6F4->defn=({struct Cyc_Core_Opt*_tmp707=
_cycalloc(sizeof(*_tmp707));_tmp707->v=(void*)Cyc_Absyn_void_star_typ();_tmp707;});
goto _LL457;_LL45A:;_LL45B: _tmp6F4->defn=({struct Cyc_Core_Opt*_tmp708=_cycalloc(
sizeof(*_tmp708));_tmp708->v=(void*)((void*)0);_tmp708;});goto _LL457;_LL457:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp709=_cycalloc(sizeof(*_tmp709));
_tmp709->hd=d;_tmp709->tl=Cyc_Toc_result_decls;_tmp709;});goto _LL43A;_LL44B: if((
int)_tmp6EE != 0)goto _LL44D;_LL44C: goto _LL44E;_LL44D: if((int)_tmp6EE != 1)goto
_LL44F;_LL44E: goto _LL43A;_LL44F: if(_tmp6EE <= (void*)2)goto _LL451;if(*((int*)
_tmp6EE)!= 8)goto _LL451;_tmp6F5=((struct Cyc_Absyn_Namespace_d_struct*)_tmp6EE)->f2;
_LL450: _tmp6F6=_tmp6F5;goto _LL452;_LL451: if(_tmp6EE <= (void*)2)goto _LL453;if(*((
int*)_tmp6EE)!= 9)goto _LL453;_tmp6F6=((struct Cyc_Absyn_Using_d_struct*)_tmp6EE)->f2;
_LL452: _tmp6F7=_tmp6F6;goto _LL454;_LL453: if(_tmp6EE <= (void*)2)goto _LL455;if(*((
int*)_tmp6EE)!= 10)goto _LL455;_tmp6F7=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp6EE)->f1;_LL454: nv=Cyc_Toc_decls_to_c(nv,_tmp6F7,top,cinclude);goto _LL43A;
_LL455: if(_tmp6EE <= (void*)2)goto _LL43A;if(*((int*)_tmp6EE)!= 11)goto _LL43A;
_tmp6F8=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp6EE)->f1;_LL456: nv=Cyc_Toc_decls_to_c(
nv,_tmp6F8,top,1);goto _LL43A;_LL43A:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1,0);return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
