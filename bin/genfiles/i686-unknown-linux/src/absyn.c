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
Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
struct _tuple0{void*f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct
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
_tuple0*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;
struct Cyc_List_List*targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0*tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*f1;
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
int tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{
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
_dynforward_ptr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual
y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(
struct _tuple0*name);void*Cyc_Absyn_union_typ(struct _dynforward_ptr*name);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dynforward_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dynforward_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dynforward_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);
struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dynforward_ptr*Cyc_Absyn_fieldname(int);
struct _tuple4{void*f1;struct _tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Absyn_is_union_type(
void*);void Cyc_Absyn_print_decls(struct Cyc_List_List*);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple5{unsigned int f1;struct _dynforward_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dynforward_ptr*f1;unsigned int f2;struct _dynforward_ptr f3;};struct
_tuple6{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dynforward_ptr f2;};struct _tuple7{unsigned int f1;struct
_dynforward_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dynforward_ptr
f1;struct _dynforward_ptr f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;unsigned int f3;struct
_dynforward_ptr f4;};struct _tuple8{struct _dynforward_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);typedef
struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*aggrdecls;
struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Tcenv_Fenv*
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void*Cyc_Tcutil_compress(void*t);void Cyc_Marshal_print_type(void*rep,void*val);
struct _tuple10{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple11{struct
_dynforward_ptr f1;int f2;};static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,
struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct
Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_strlist_cmp(
vs1,vs2);}int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){void*_tmp0=(*
q1).f1;void*_tmp1=(*q2).f1;{struct _tuple9 _tmp3=({struct _tuple9 _tmp2;_tmp2.f1=
_tmp0;_tmp2.f2=_tmp1;_tmp2;});void*_tmp4;void*_tmp5;void*_tmp6;struct Cyc_List_List*
_tmp7;void*_tmp8;struct Cyc_List_List*_tmp9;void*_tmpA;struct Cyc_List_List*_tmpB;
void*_tmpC;struct Cyc_List_List*_tmpD;void*_tmpE;void*_tmpF;void*_tmp10;void*
_tmp11;_LL1: _tmp4=_tmp3.f1;if((int)_tmp4 != 0)goto _LL3;_tmp5=_tmp3.f2;if((int)
_tmp5 != 0)goto _LL3;_LL2: goto _LL0;_LL3: _tmp6=_tmp3.f1;if(_tmp6 <= (void*)1)goto
_LL5;if(*((int*)_tmp6)!= 0)goto _LL5;_tmp7=((struct Cyc_Absyn_Rel_n_struct*)_tmp6)->f1;
_tmp8=_tmp3.f2;if(_tmp8 <= (void*)1)goto _LL5;if(*((int*)_tmp8)!= 0)goto _LL5;_tmp9=((
struct Cyc_Absyn_Rel_n_struct*)_tmp8)->f1;_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;
_LL5: _tmpA=_tmp3.f1;if(_tmpA <= (void*)1)goto _LL7;if(*((int*)_tmpA)!= 1)goto _LL7;
_tmpB=((struct Cyc_Absyn_Abs_n_struct*)_tmpA)->f1;_tmpC=_tmp3.f2;if(_tmpC <= (void*)
1)goto _LL7;if(*((int*)_tmpC)!= 1)goto _LL7;_tmpD=((struct Cyc_Absyn_Abs_n_struct*)
_tmpC)->f1;_LL6: {int i=Cyc_Absyn_strlist_cmp(_tmpB,_tmpD);if(i != 0)return i;goto
_LL0;}_LL7: _tmpE=_tmp3.f1;if((int)_tmpE != 0)goto _LL9;_LL8: return - 1;_LL9: _tmpF=
_tmp3.f2;if((int)_tmpF != 0)goto _LLB;_LLA: return 1;_LLB: _tmp10=_tmp3.f1;if(_tmp10
<= (void*)1)goto _LLD;if(*((int*)_tmp10)!= 0)goto _LLD;_LLC: return - 1;_LLD: _tmp11=
_tmp3.f2;if(_tmp11 <= (void*)1)goto _LL0;if(*((int*)_tmp11)!= 0)goto _LL0;_LLE:
return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;if(tv1->identity == tv2->identity)return 0;if(tv1->identity
!= 0  && tv2->identity != 0)return*((int*)_check_null(tv1->identity))- *((int*)
_check_null(tv2->identity));else{if(tv1->identity == 0)return - 1;else{return 1;}}}
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={0,0};void*Cyc_Absyn_rel_ns_null=(
void*)& Cyc_Absyn_rel_ns_null_value;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*
qv){void*_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;
_LL10: if(_tmp13 <= (void*)1)goto _LL14;if(*((int*)_tmp13)!= 0)goto _LL12;_tmp14=((
struct Cyc_Absyn_Rel_n_struct*)_tmp13)->f1;if(_tmp14 != 0)goto _LL12;_LL11: goto
_LL13;_LL12: if(*((int*)_tmp13)!= 1)goto _LL14;_tmp15=((struct Cyc_Absyn_Abs_n_struct*)
_tmp13)->f1;if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((int)_tmp13 != 0)
goto _LL16;_LL15: return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=
0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(
void*)({struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({
struct Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=
Cyc_Absyn_new_type_counter ++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt
Cyc_Absyn_mk={(void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({
struct Cyc_Absyn_Tqual _tmp18;_tmp18.print_const=x.print_const  || y.print_const;
_tmp18.q_volatile=x.q_volatile  || y.q_volatile;_tmp18.q_restrict=x.q_restrict
 || y.q_restrict;_tmp18.real_const=x.real_const  || y.real_const;_tmp18.loc=Cyc_Position_segment_join(
x.loc,y.loc);_tmp18;});}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.print_const=0;_tmp19.q_volatile=
0;_tmp19.q_restrict=0;_tmp19.real_const=0;_tmp19.loc=loc;_tmp19;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Tqual
_tmp1A;_tmp1A.print_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=0;_tmp1A.real_const=
1;_tmp1A.loc=loc;_tmp1A;});}static struct Cyc_Absyn_Int_c_struct Cyc_Absyn_one_intc={
2,(void*)((void*)1),1};static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={
0,(void*)((void*)& Cyc_Absyn_one_intc)};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={
0,(void*)((void*)& Cyc_Absyn_one_b_raw),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={0,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x){return({struct Cyc_Absyn_Conref*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));
_tmp1F[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(void*)
x;_tmp20;});_tmp1F;}));_tmp1E;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){
return({struct Cyc_Absyn_Conref*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->v=(void*)((
void*)0);_tmp21;});}static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_conref_constraint={
0,(void*)1};static struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(void*)((void*)&
Cyc_Absyn_true_conref_constraint)};struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=&
Cyc_Absyn_true_conref_v;static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_conref_constraint={
0,(void*)0};static struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(void*)((void*)&
Cyc_Absyn_false_conref_constraint)};struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref=&
Cyc_Absyn_false_conref_v;static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_bounds_one_conref_constraint={
0,(void*)((void*)& Cyc_Absyn_one_bt)};static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_one_conref_v={(
void*)((void*)& Cyc_Absyn_bounds_one_conref_constraint)};struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;static struct Cyc_Absyn_Eq_constr_struct
Cyc_Absyn_bounds_dynforward_conref_constraint={0,(void*)((void*)0)};static struct
Cyc_Absyn_Conref Cyc_Absyn_bounds_dynforward_conref_v={(void*)((void*)& Cyc_Absyn_bounds_dynforward_conref_constraint)};
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref=& Cyc_Absyn_bounds_dynforward_conref_v;
static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_bounds_dyneither_conref_constraint={
0,(void*)((void*)1)};static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_dyneither_conref_v={(
void*)((void*)& Cyc_Absyn_bounds_dyneither_conref_constraint)};struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x){void*_tmp27=(void*)x->v;
struct Cyc_Absyn_Conref*_tmp28;struct Cyc_Absyn_Conref**_tmp29;_LL19: if((int)
_tmp27 != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp27 <= (void*)1)goto _LL1D;if(*((
int*)_tmp27)!= 0)goto _LL1D;_LL1C: return x;_LL1D: if(_tmp27 <= (void*)1)goto _LL18;
if(*((int*)_tmp27)!= 1)goto _LL18;_tmp28=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp27)->f1;_tmp29=(struct Cyc_Absyn_Conref**)&((struct Cyc_Absyn_Forward_constr_struct*)
_tmp27)->f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp2A=Cyc_Absyn_compress_conref(*
_tmp29);*_tmp29=_tmp2A;return _tmp2A;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x){void*_tmp2B=(void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp2C;_LL20: if(
_tmp2B <= (void*)1)goto _LL22;if(*((int*)_tmp2B)!= 0)goto _LL22;_tmp2C=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp2B)->f1;_LL21: return _tmp2C;_LL22:;_LL23:({
void*_tmp2D[0]={};Cyc_Tcutil_impos(({const char*_tmp2E="conref_val";
_tag_dynforward(_tmp2E,sizeof(char),_get_zero_arr_size(_tmp2E,11));}),
_tag_dynforward(_tmp2D,sizeof(void*),0));});_LL1F:;}void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){void*_tmp2F=(void*)(Cyc_Absyn_compress_conref(x))->v;
void*_tmp30;_LL25: if(_tmp2F <= (void*)1)goto _LL27;if(*((int*)_tmp2F)!= 0)goto
_LL27;_tmp30=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2F)->f1;_LL26: return
_tmp30;_LL27:;_LL28: return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*k){void*
_tmp31=k;struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Core_Opt*
_tmp34;struct Cyc_Core_Opt _tmp35;void*_tmp36;void**_tmp37;struct Cyc_Core_Opt*
_tmp38;struct Cyc_Core_Opt _tmp39;void*_tmp3A;void**_tmp3B;_LL2A: if(*((int*)_tmp31)
!= 0)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if(*((int*)_tmp31)!= 1)goto _LL2E;_tmp32=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp31)->f1;if(_tmp32 != 0)goto _LL2E;_LL2D:
goto _LL2F;_LL2E: if(*((int*)_tmp31)!= 2)goto _LL30;_tmp33=((struct Cyc_Absyn_Less_kb_struct*)
_tmp31)->f1;if(_tmp33 != 0)goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp31)!= 1)
goto _LL32;_tmp34=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp31)->f1;if(_tmp34 == 0)
goto _LL32;_tmp35=*_tmp34;_tmp36=(void*)_tmp35.v;_tmp37=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp31)->f1).v;_LL31: _tmp3B=_tmp37;goto _LL33;_LL32: if(*((int*)_tmp31)!= 2)goto
_LL29;_tmp38=((struct Cyc_Absyn_Less_kb_struct*)_tmp31)->f1;if(_tmp38 == 0)goto
_LL29;_tmp39=*_tmp38;_tmp3A=(void*)_tmp39.v;_tmp3B=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp31)->f1).v;_LL33:*_tmp3B=Cyc_Absyn_compress_kb(*_tmp3B);return*_tmp3B;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp3C=Cyc_Absyn_compress_kb(kb);void*
_tmp3D;struct Cyc_Core_Opt*_tmp3E;struct Cyc_Core_Opt**_tmp3F;struct Cyc_Core_Opt*
_tmp40;struct Cyc_Core_Opt**_tmp41;void*_tmp42;_LL35: if(*((int*)_tmp3C)!= 0)goto
_LL37;_tmp3D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp3C)->f1;_LL36: return
_tmp3D;_LL37: if(*((int*)_tmp3C)!= 1)goto _LL39;_tmp3E=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3C)->f1;_tmp3F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3C)->f1;_LL38: _tmp41=_tmp3F;_tmp42=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp3C)!= 2)goto _LL34;_tmp40=((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f1;
_tmp41=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f1;
_tmp42=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3C)->f2;_LL3A:*_tmp41=({
struct Cyc_Core_Opt*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(void*)_tmp42;_tmp45;});
_tmp44;}));_tmp43;});return _tmp42;_LL34:;}static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_char_tt={5,(void*)((void*)2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uchar_tt={5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uint_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulong_tt={5,(void*)((void*)1),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={5,(void*)((void*)1),(void*)((void*)4)};void*Cyc_Absyn_char_typ=(
void*)& Cyc_Absyn_char_tt;void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(
void*)& Cyc_Absyn_uint_tt;void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_schar_tt={5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sint_tt={5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slong_tt={5,(void*)((void*)0),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={5,(void*)((void*)0),(void*)((void*)4)};void*Cyc_Absyn_schar_typ=(
void*)& Cyc_Absyn_schar_tt;void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)&
Cyc_Absyn_slong_tt;void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmp51=_cycalloc_atomic(sizeof(*_tmp51));
_tmp51[0]=({struct Cyc_Absyn_DoubleType_struct _tmp52;_tmp52.tag=6;_tmp52.f1=b;
_tmp52;});_tmp51;});}static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_abs_null={1,0};
static char _tmp54[4]="exn";static struct _dynforward_ptr Cyc_Absyn_exn_str={_tmp54,
_tmp54 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(void*)& Cyc_Absyn_abs_null,&
Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static
char _tmp55[15]="Null_Exception";static struct _dynforward_ptr Cyc_Absyn_Null_Exception_str={
_tmp55,_tmp55 + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(void*)& Cyc_Absyn_abs_null,&
Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Null_Exception_tuf=&
Cyc_Absyn_Null_Exception_tuf_v;static char _tmp56[13]="Array_bounds";static struct
_dynforward_ptr Cyc_Absyn_Array_bounds_str={_tmp56,_tmp56 + 13};static struct
_tuple0 Cyc_Absyn_Array_bounds_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp57[16]="Match_Exception";static struct
_dynforward_ptr Cyc_Absyn_Match_Exception_str={_tmp57,_tmp57 + 16};static struct
_tuple0 Cyc_Absyn_Match_Exception_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp58[10]="Bad_alloc";static struct
_dynforward_ptr Cyc_Absyn_Bad_alloc_str={_tmp58,_tmp58 + 10};static struct _tuple0
Cyc_Absyn_Bad_alloc_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Bad_alloc_str};
struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,(void*)((void*)3)};struct
Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;static
struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1,0};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Absyn_KnownTunion_struct Cyc_Absyn_exn_tinfou={1,& Cyc_Absyn_exn_tud};
static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)2)};static struct Cyc_Absyn_TunionType_struct
Cyc_Absyn_exn_typ_tt={2,{(void*)((void*)& Cyc_Absyn_exn_tinfou),0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_heap_opt}};void*Cyc_Absyn_exn_typ=(void*)& Cyc_Absyn_exn_typ_tt;static
char _tmp5B[9]="PrintArg";static struct _dynforward_ptr Cyc_Absyn_printarg_str={
_tmp5B,_tmp5B + 9};static char _tmp5C[9]="ScanfArg";static struct _dynforward_ptr Cyc_Absyn_scanfarg_str={
_tmp5C,_tmp5C + 9};static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_std_namespace={1,0};
static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(void*)& Cyc_Absyn_std_namespace,&
Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_tunion_scanf_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*Cyc_Absyn_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(
0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);
Cyc_Absyn_string_t_opt=({void**_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=t;
_tmp5E;});}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({
void**_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=t;_tmp5F;});}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));}void*Cyc_Absyn_starb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({
struct Cyc_Absyn_PointerType_struct _tmp61;_tmp61.tag=4;_tmp61.f1=({struct Cyc_Absyn_PtrInfo
_tmp62;_tmp62.elt_typ=(void*)t;_tmp62.elt_tq=tq;_tmp62.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp63;_tmp63.rgn=(void*)r;_tmp63.nullable=Cyc_Absyn_true_conref;_tmp63.bounds=
Cyc_Absyn_new_conref(b);_tmp63.zero_term=zeroterm;_tmp63.ptrloc=0;_tmp63;});
_tmp62;});_tmp61;});_tmp60;});}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_PointerType_struct
_tmp65;_tmp65.tag=4;_tmp65.f1=({struct Cyc_Absyn_PtrInfo _tmp66;_tmp66.elt_typ=(
void*)t;_tmp66.elt_tq=tq;_tmp66.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp67;_tmp67.rgn=(
void*)r;_tmp67.nullable=Cyc_Absyn_false_conref;_tmp67.bounds=Cyc_Absyn_new_conref(
b);_tmp67.zero_term=zeroterm;_tmp67.ptrloc=0;_tmp67;});_tmp66;});_tmp65;});
_tmp64;});}void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68[0]=({struct Cyc_Absyn_PointerType_struct
_tmp69;_tmp69.tag=4;_tmp69.f1=({struct Cyc_Absyn_PtrInfo _tmp6A;_tmp6A.elt_typ=(
void*)t;_tmp6A.elt_tq=tq;_tmp6A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp6B;_tmp6B.rgn=(
void*)r;_tmp6B.nullable=Cyc_Absyn_true_conref;_tmp6B.bounds=Cyc_Absyn_bounds_one_conref;
_tmp6B.zero_term=zeroterm;_tmp6B.ptrloc=0;_tmp6B;});_tmp6A;});_tmp69;});_tmp68;});}
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({
struct Cyc_Absyn_PointerType_struct _tmp6D;_tmp6D.tag=4;_tmp6D.f1=({struct Cyc_Absyn_PtrInfo
_tmp6E;_tmp6E.elt_typ=(void*)t;_tmp6E.elt_tq=tq;_tmp6E.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6F;_tmp6F.rgn=(void*)((void*)2);_tmp6F.nullable=Cyc_Absyn_true_conref;_tmp6F.bounds=
Cyc_Absyn_bounds_one_conref;_tmp6F.zero_term=Cyc_Absyn_false_conref;_tmp6F.ptrloc=
0;_tmp6F;});_tmp6E;});_tmp6D;});_tmp6C;});}void*Cyc_Absyn_at_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({
struct Cyc_Absyn_PointerType_struct _tmp71;_tmp71.tag=4;_tmp71.f1=({struct Cyc_Absyn_PtrInfo
_tmp72;_tmp72.elt_typ=(void*)t;_tmp72.elt_tq=tq;_tmp72.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp73;_tmp73.rgn=(void*)r;_tmp73.nullable=Cyc_Absyn_false_conref;_tmp73.bounds=
Cyc_Absyn_bounds_one_conref;_tmp73.zero_term=zeroterm;_tmp73.ptrloc=0;_tmp73;});
_tmp72;});_tmp71;});_tmp70;});}void*Cyc_Absyn_dynforward_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({
struct Cyc_Absyn_PointerType_struct _tmp75;_tmp75.tag=4;_tmp75.f1=({struct Cyc_Absyn_PtrInfo
_tmp76;_tmp76.elt_typ=(void*)t;_tmp76.elt_tq=tq;_tmp76.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp77;_tmp77.rgn=(void*)r;_tmp77.nullable=Cyc_Absyn_true_conref;_tmp77.bounds=
Cyc_Absyn_bounds_dynforward_conref;_tmp77.zero_term=zeroterm;_tmp77.ptrloc=0;
_tmp77;});_tmp76;});_tmp75;});_tmp74;});}void*Cyc_Absyn_dyneither_typ(void*t,
void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({
struct Cyc_Absyn_PointerType_struct _tmp79;_tmp79.tag=4;_tmp79.f1=({struct Cyc_Absyn_PtrInfo
_tmp7A;_tmp7A.elt_typ=(void*)t;_tmp7A.elt_tq=tq;_tmp7A.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp7B;_tmp7B.rgn=(void*)r;_tmp7B.nullable=Cyc_Absyn_true_conref;_tmp7B.bounds=
Cyc_Absyn_bounds_dyneither_conref;_tmp7B.zero_term=zeroterm;_tmp7B.ptrloc=0;
_tmp7B;});_tmp7A;});_tmp79;});_tmp78;});}void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp7D;_tmp7D.tag=7;_tmp7D.f1=({struct Cyc_Absyn_ArrayInfo _tmp7E;_tmp7E.elt_type=(
void*)elt_type;_tmp7E.tq=tq;_tmp7E.num_elts=num_elts;_tmp7E.zero_term=zero_term;
_tmp7E.zt_loc=ztloc;_tmp7E;});_tmp7D;});_tmp7C;});}static char _tmp88[8]="__sFILE";
void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;static struct _dynforward_ptr
sf_str={_tmp88,_tmp88 + 8};static struct _dynforward_ptr*sf=& sf_str;if(file_t_opt == 
0){struct _tuple0*file_t_name=({struct _tuple0*_tmp87=_cycalloc(sizeof(*_tmp87));
_tmp87->f1=(void*)& Cyc_Absyn_std_namespace;_tmp87->f2=sf;_tmp87;});struct Cyc_Absyn_Aggrdecl*
sd=({struct Cyc_Absyn_Aggrdecl*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->kind=(
void*)((void*)0);_tmp86->sc=(void*)((void*)1);_tmp86->name=file_t_name;_tmp86->tvs=
0;_tmp86->impl=0;_tmp86->attributes=0;_tmp86;});void*file_struct_typ=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({
struct Cyc_Absyn_AggrType_struct _tmp81;_tmp81.tag=10;_tmp81.f1=({struct Cyc_Absyn_AggrInfo
_tmp82;_tmp82.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp83=
_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp84;
_tmp84.tag=1;_tmp84.f1=({struct Cyc_Absyn_Aggrdecl**_tmp85=_cycalloc(sizeof(*
_tmp85));_tmp85[0]=sd;_tmp85;});_tmp84;});_tmp83;}));_tmp82.targs=0;_tmp82;});
_tmp81;});_tmp80;});file_t_opt=({void**_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[
0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmp7F;});}return*file_t_opt;}void*Cyc_Absyn_void_star_typ(){static void**
void_star_t_opt=0;if(void_star_t_opt == 0)void_star_t_opt=({void**_tmp89=
_cycalloc(sizeof(*_tmp89));_tmp89[0]=Cyc_Absyn_star_typ((void*)0,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmp89;});return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct
Cyc_Absyn_empty_eff={20,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
void*Cyc_Absyn_aggr_typ(void*k,struct _dynforward_ptr*name){return(void*)({struct
Cyc_Absyn_AggrType_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct
Cyc_Absyn_AggrType_struct _tmp8C;_tmp8C.tag=10;_tmp8C.f1=({struct Cyc_Absyn_AggrInfo
_tmp8D;_tmp8D.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp8F;_tmp8F.tag=0;_tmp8F.f1=(void*)k;_tmp8F.f2=({struct _tuple0*_tmp90=
_cycalloc(sizeof(*_tmp90));_tmp90->f1=Cyc_Absyn_rel_ns_null;_tmp90->f2=name;
_tmp90;});_tmp8F;});_tmp8E;}));_tmp8D.targs=0;_tmp8D;});_tmp8C;});_tmp8B;});}
void*Cyc_Absyn_strct(struct _dynforward_ptr*name){return Cyc_Absyn_aggr_typ((void*)
0,name);}void*Cyc_Absyn_union_typ(struct _dynforward_ptr*name){return Cyc_Absyn_aggr_typ((
void*)1,name);}void*Cyc_Absyn_strctq(struct _tuple0*name){return(void*)({struct
Cyc_Absyn_AggrType_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct
Cyc_Absyn_AggrType_struct _tmp92;_tmp92.tag=10;_tmp92.f1=({struct Cyc_Absyn_AggrInfo
_tmp93;_tmp93.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp95;_tmp95.tag=0;_tmp95.f1=(void*)((void*)0);_tmp95.f2=name;_tmp95;});_tmp94;}));
_tmp93.targs=0;_tmp93;});_tmp92;});_tmp91;});}void*Cyc_Absyn_unionq_typ(struct
_tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp96=_cycalloc(
sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_AggrType_struct _tmp97;_tmp97.tag=10;
_tmp97.f1=({struct Cyc_Absyn_AggrInfo _tmp98;_tmp98.aggr_info=(void*)((void*)({
struct Cyc_Absyn_UnknownAggr_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({
struct Cyc_Absyn_UnknownAggr_struct _tmp9A;_tmp9A.tag=0;_tmp9A.f1=(void*)((void*)1);
_tmp9A.f2=name;_tmp9A;});_tmp99;}));_tmp98.targs=0;_tmp98;});_tmp97;});_tmp96;});}
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*loc){
return({struct Cyc_Absyn_Exp*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->topt=0;
_tmp9B->r=(void*)r;_tmp9B->loc=loc;_tmp9B->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);
_tmp9B;});}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_New_e_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({
struct Cyc_Absyn_New_e_struct _tmp9D;_tmp9D.tag=17;_tmp9D.f1=rgn_handle;_tmp9D.f2=
e;_tmp9D;});_tmp9C;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*
e){return({struct Cyc_Absyn_Exp*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=*e;
_tmp9E;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*c,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*_tmp9F=
_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_Const_e_struct _tmpA0;
_tmpA0.tag=0;_tmpA0.f1=(void*)c;_tmpA0;});_tmp9F;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({
struct Cyc_Absyn_Const_e_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(void*)((void*)0);
_tmpA2;});_tmpA1;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,
struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_Int_c_struct _tmpA4;
_tmpA4.tag=2;_tmpA4.f1=(void*)s;_tmpA4.f2=i;_tmpA4;});_tmpA3;}),seg);}struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((
void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){
return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_Char_c_struct
_tmpA6;_tmpA6.tag=0;_tmpA6.f1=(void*)((void*)2);_tmpA6.f2=c;_tmpA6;});_tmpA5;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dynforward_ptr f,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Float_c_struct*_tmpA7=
_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_Float_c_struct _tmpA8;
_tmpA8.tag=4;_tmpA8.f1=f;_tmpA8;});_tmpA7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_String_c_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[
0]=({struct Cyc_Absyn_String_c_struct _tmpAA;_tmpAA.tag=5;_tmpAA.f1=s;_tmpAA;});
_tmpA9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_Var_e_struct _tmpAC;_tmpAC.tag=
1;_tmpAC.f1=q;_tmpAC.f2=(void*)((void*)0);_tmpAC;});_tmpAB;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_Var_e_struct _tmpAE;_tmpAE.tag=1;_tmpAE.f1=q;
_tmpAE.f2=(void*)b;_tmpAE;});_tmpAD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpB0;_tmpB0.tag=2;_tmpB0.f1=q;_tmpB0;});
_tmpAF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Primop_e_struct
_tmpB2;_tmpB2.tag=3;_tmpB2.f1=(void*)p;_tmpB2.f2=es;_tmpB2;});_tmpB1;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*
_tmpB3));_tmpB3->hd=e;_tmpB3->tl=0;_tmpB3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*
_tmpB4));_tmpB4->hd=e1;_tmpB4->tl=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*
_tmpB5));_tmpB5->hd=e2;_tmpB5->tl=0;_tmpB5;});_tmpB4;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpB6=
_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpB7;
_tmpB7.tag=4;_tmpB7.f1=e1;_tmpB7.f2=popt;_tmpB7.f3=e2;_tmpB7;});_tmpB6;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpB9;_tmpB9.tag=5;_tmpB9.f1=e;_tmpB9.f2=(void*)i;_tmpB9;});_tmpB8;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpBB;_tmpBB.tag=6;_tmpBB.f1=e1;_tmpBB.f2=e2;_tmpBB.f3=e3;_tmpBB;});_tmpBA;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_And_e_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_And_e_struct _tmpBD;
_tmpBD.tag=7;_tmpBD.f1=e1;_tmpBD.f2=e2;_tmpBD;});_tmpBC;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Or_e_struct*_tmpBE=
_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_Or_e_struct _tmpBF;_tmpBF.tag=
8;_tmpBF.f1=e1;_tmpBF.f2=e2;_tmpBF;});_tmpBE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpC0=
_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpC1;
_tmpC1.tag=9;_tmpC1.f1=e1;_tmpC1.f2=e2;_tmpC1;});_tmpC0;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpC3;_tmpC3.tag=10;_tmpC3.f1=e;_tmpC3.f2=es;_tmpC3;});_tmpC2;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpC5;_tmpC5.tag=11;_tmpC5.f1=e;_tmpC5.f2=es;_tmpC5.f3=0;_tmpC5;});_tmpC4;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpC7;_tmpC7.tag=13;_tmpC7.f1=e;_tmpC7;});_tmpC6;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpC9;_tmpC9.tag=14;_tmpC9.f1=e;_tmpC9.f2=ts;_tmpC9;});_tmpC8;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*
c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpCB;_tmpCB.tag=15;_tmpCB.f1=(void*)t;_tmpCB.f2=e;_tmpCB.f3=user_cast;_tmpCB.f4=(
void*)c;_tmpCB;});_tmpCA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Throw_e_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({
struct Cyc_Absyn_Throw_e_struct _tmpCD;_tmpCD.tag=12;_tmpCD.f1=e;_tmpCD;});_tmpCC;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Address_e_struct*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_Address_e_struct _tmpCF;
_tmpCF.tag=16;_tmpCF.f1=e;_tmpCF;});_tmpCE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpD1;_tmpD1.tag=18;_tmpD1.f1=(void*)t;_tmpD1;});
_tmpD0;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_Sizeofexp_e_struct
_tmpD3;_tmpD3.tag=19;_tmpD3.f1=e;_tmpD3;});_tmpD2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));
_tmpD4[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmpD5;_tmpD5.tag=20;_tmpD5.f1=(
void*)t;_tmpD5.f2=(void*)of;_tmpD5;});_tmpD4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpD7;_tmpD7.tag=21;_tmpD7.f1=tvs;_tmpD7.f2=(
void*)t;_tmpD7;});_tmpD6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpD9;_tmpD9.tag=22;_tmpD9.f1=e;_tmpD9;});_tmpD8;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpDB;_tmpDB.tag=23;_tmpDB.f1=e;_tmpDB.f2=n;
_tmpDB;});_tmpDA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _dynforward_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));
_tmpDC[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpDD;_tmpDD.tag=24;_tmpDD.f1=e;
_tmpDD.f2=n;_tmpDD;});_tmpDC;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpDE=
_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpDF;
_tmpDF.tag=25;_tmpDF.f1=e1;_tmpDF.f2=e2;_tmpDF;});_tmpDE;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpE0=_cycalloc(
sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpE1;_tmpE1.tag=26;
_tmpE1.f1=es;_tmpE1;});_tmpE0;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));
_tmpE2[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpE3;_tmpE3.tag=37;_tmpE3.f1=s;
_tmpE3;});_tmpE2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple12{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*
_tmpE4));_tmpE4->hd=({struct _tuple12*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->f1=
0;_tmpE5->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpE5;});_tmpE4->tl=dles;_tmpE4;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpE6=_cycalloc(
sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_Array_e_struct _tmpE7;_tmpE7.tag=28;
_tmpE7.f1=dles;_tmpE7;});_tmpE6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpE9;_tmpE9.tag=36;_tmpE9.f1=n;_tmpE9.f2=dles;_tmpE9;});_tmpE8;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->r=(void*)s;_tmpEA->loc=
loc;_tmpEA->non_local_preds=0;_tmpEA->try_depth=0;_tmpEA->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpEA;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpEB=_cycalloc(
sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_Exp_s_struct _tmpEC;_tmpEC.tag=0;
_tmpEC.f1=e;_tmpEC;});_tmpEB;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpED=
_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_Return_s_struct _tmpEE;
_tmpEE.tag=2;_tmpEE.f1=e;_tmpEE;});_tmpED;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpF0;_tmpF0.tag=3;_tmpF0.f1=e;_tmpF0.f2=s1;_tmpF0.f3=s2;_tmpF0;});_tmpEF;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_While_s_struct
_tmpF2;_tmpF2.tag=4;_tmpF2.f1=({struct _tuple2 _tmpF3;_tmpF3.f1=e;_tmpF3.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpF3;});_tmpF2.f2=s;_tmpF2;});_tmpF1;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_Break_s_struct
_tmpF5;_tmpF5.tag=5;_tmpF5.f1=0;_tmpF5;});_tmpF4;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));
_tmpF6[0]=({struct Cyc_Absyn_Continue_s_struct _tmpF7;_tmpF7.tag=6;_tmpF7.f1=0;
_tmpF7;});_tmpF6;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=({struct Cyc_Absyn_For_s_struct _tmpF9;
_tmpF9.tag=8;_tmpF9.f1=e1;_tmpF9.f2=({struct _tuple2 _tmpFA;_tmpFA.f1=e2;_tmpFA.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpFA;});_tmpF9.f3=({struct _tuple2 _tmpFB;_tmpFB.f1=
e3;_tmpFB.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpFB;});_tmpF9.f4=s;_tmpF9;});_tmpF8;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpFD;_tmpFD.tag=9;_tmpFD.f1=e;_tmpFD.f2=scs;_tmpFD;});_tmpFC;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmpFF=({struct _tuple9 _tmpFE;
_tmpFE.f1=(void*)s1->r;_tmpFE.f2=(void*)s2->r;_tmpFE;});void*_tmp100;void*
_tmp101;_LL3C: _tmp100=_tmpFF.f1;if((int)_tmp100 != 0)goto _LL3E;_LL3D: return s2;
_LL3E: _tmp101=_tmpFF.f2;if((int)_tmp101 != 0)goto _LL40;_LL3F: return s1;_LL40:;
_LL41: return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Seq_s_struct*_tmp102=
_cycalloc(sizeof(*_tmp102));_tmp102[0]=({struct Cyc_Absyn_Seq_s_struct _tmp103;
_tmp103.tag=1;_tmp103.f1=s1;_tmp103.f2=s2;_tmp103;});_tmp102;}),loc);_LL3B:;}
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmp105;_tmp105.tag=10;_tmp105.f1=el;_tmp105.f2=0;_tmp105;});_tmp104;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp107;_tmp107.tag=11;_tmp107.f1=d;_tmp107.f2=s;_tmp107;});_tmp106;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*
d=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp10A=_cycalloc(
sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Var_d_struct _tmp10B;_tmp10B.tag=0;
_tmp10B.f1=Cyc_Absyn_new_vardecl(x,t,init);_tmp10B;});_tmp10A;}),loc);return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Decl_s_struct*_tmp108=_cycalloc(sizeof(*_tmp108));
_tmp108[0]=({struct Cyc_Absyn_Decl_s_struct _tmp109;_tmp109.tag=11;_tmp109.f1=d;
_tmp109.f2=s;_tmp109;});_tmp108;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(
struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_Label_s_struct _tmp10D;
_tmp10D.tag=12;_tmp10D.f1=v;_tmp10D.f2=s;_tmp10D;});_tmp10C;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Do_s_struct _tmp10F;
_tmp10F.tag=13;_tmp10F.f1=s;_tmp10F.f2=({struct _tuple2 _tmp110;_tmp110.f1=e;
_tmp110.f2=Cyc_Absyn_skip_stmt(e->loc);_tmp110;});_tmp10F;});_tmp10E;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*
_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp112;_tmp112.tag=14;_tmp112.f1=s;_tmp112.f2=scs;_tmp112;});_tmp111;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp113=
_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_Goto_s_struct _tmp114;
_tmp114.tag=7;_tmp114.f1=lab;_tmp114.f2=0;_tmp114;});_tmp113;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({struct Cyc_Absyn_Pat*
_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->r=(void*)p;_tmp115->topt=0;_tmp115->loc=
s;_tmp115;});}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_struct*_tmp116=_cycalloc(
sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Absyn_Exp_p_struct _tmp117;_tmp117.tag=
14;_tmp117.f1=e;_tmp117;});_tmp116;}),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp118=
_cycalloc(sizeof(*_tmp118));_tmp118->r=(void*)r;_tmp118->loc=loc;_tmp118;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_Let_d_struct
_tmp11A;_tmp11A.tag=2;_tmp11A.f1=p;_tmp11A.f2=0;_tmp11A.f3=e;_tmp11A;});_tmp119;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp11B=
_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Absyn_Letv_d_struct _tmp11C;
_tmp11C.tag=3;_tmp11C.f1=vds;_tmp11C;});_tmp11B;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->sc=(void*)((
void*)2);_tmp11D->name=x;_tmp11D->tq=Cyc_Absyn_empty_tqual(0);_tmp11D->type=(
void*)t;_tmp11D->initializer=init;_tmp11D->rgn=0;_tmp11D->attributes=0;_tmp11D->escapes=
0;_tmp11D;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp11E=
_cycalloc(sizeof(*_tmp11E));_tmp11E->sc=(void*)((void*)0);_tmp11E->name=x;
_tmp11E->tq=Cyc_Absyn_empty_tqual(0);_tmp11E->type=(void*)t;_tmp11E->initializer=
init;_tmp11E->rgn=0;_tmp11E->attributes=0;_tmp11E->escapes=0;_tmp11E;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->exist_vars=exists;_tmp11F->rgn_po=
po;_tmp11F->fields=fs;_tmp11F;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp120=_cycalloc(sizeof(*_tmp120));
_tmp120[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp121;_tmp121.tag=4;_tmp121.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->kind=(void*)
k;_tmp122->sc=(void*)s;_tmp122->name=n;_tmp122->tvs=ts;_tmp122->impl=i;_tmp122->attributes=
atts;_tmp122;});_tmp121;});_tmp120;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp123=
_cycalloc(sizeof(*_tmp123));_tmp123[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp124;
_tmp124.tag=5;_tmp124.f1=({struct Cyc_Absyn_Tuniondecl*_tmp125=_cycalloc(sizeof(*
_tmp125));_tmp125->sc=(void*)s;_tmp125->name=n;_tmp125->tvs=ts;_tmp125->fields=
fs;_tmp125->is_xtunion=is_xtunion;_tmp125->is_flat=is_flat;_tmp125;});_tmp124;});
_tmp123;}),loc);}static struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){
return({struct _tuple1*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->f1=(*a).f1;
_tmp126->f2=(*a).f2;_tmp126->f3=Cyc_Absyn_pointer_expand((*a).f3,1);_tmp126;});}
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){return(void*)({
struct Cyc_Absyn_FnType_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({
struct Cyc_Absyn_FnType_struct _tmp128;_tmp128.tag=8;_tmp128.f1=({struct Cyc_Absyn_FnInfo
_tmp129;_tmp129.tvars=tvs;_tmp129.ret_typ=(void*)Cyc_Absyn_pointer_expand(
ret_typ,0);_tmp129.effect=eff_typ;_tmp129.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args);_tmp129.c_varargs=c_varargs;_tmp129.cyc_varargs=cyc_varargs;_tmp129.rgn_po=
rgn_po;_tmp129.attributes=atts;_tmp129;});_tmp128;});_tmp127;});}void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp12A=Cyc_Tcutil_compress(t);_LL43: if(_tmp12A <= (
void*)4)goto _LL45;if(*((int*)_tmp12A)!= 8)goto _LL45;_LL44: return Cyc_Absyn_at_typ(
t,fresh_evar?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp12B=_cycalloc(sizeof(*
_tmp12B));_tmp12B->v=(void*)((void*)3);_tmp12B;}),0):(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_LL45:;_LL46: return t;_LL42:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp12C=(void*)e->r;void*_tmp12D;void*_tmp12E;struct
Cyc_Absyn_Vardecl*_tmp12F;void*_tmp130;struct Cyc_Absyn_Vardecl*_tmp131;struct Cyc_Absyn_Exp*
_tmp132;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp134;_LL48: if(*((int*)
_tmp12C)!= 1)goto _LL4A;_tmp12D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12C)->f2;
if(_tmp12D <= (void*)1)goto _LL4A;if(*((int*)_tmp12D)!= 1)goto _LL4A;_LL49: return 0;
_LL4A: if(*((int*)_tmp12C)!= 1)goto _LL4C;_tmp12E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12C)->f2;if(_tmp12E <= (void*)1)goto _LL4C;if(*((int*)_tmp12E)!= 0)goto _LL4C;
_tmp12F=((struct Cyc_Absyn_Global_b_struct*)_tmp12E)->f1;_LL4B: _tmp131=_tmp12F;
goto _LL4D;_LL4C: if(*((int*)_tmp12C)!= 1)goto _LL4E;_tmp130=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12C)->f2;if(_tmp130 <= (void*)1)goto _LL4E;if(*((int*)_tmp130)!= 3)goto _LL4E;
_tmp131=((struct Cyc_Absyn_Local_b_struct*)_tmp130)->f1;_LL4D: {void*_tmp135=Cyc_Tcutil_compress((
void*)_tmp131->type);_LL5F: if(_tmp135 <= (void*)4)goto _LL61;if(*((int*)_tmp135)!= 
7)goto _LL61;_LL60: return 0;_LL61:;_LL62: return 1;_LL5E:;}_LL4E: if(*((int*)_tmp12C)
!= 1)goto _LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp12C)!= 24)goto _LL52;_LL51:
goto _LL53;_LL52: if(*((int*)_tmp12C)!= 22)goto _LL54;_LL53: goto _LL55;_LL54: if(*((
int*)_tmp12C)!= 25)goto _LL56;_LL55: return 1;_LL56: if(*((int*)_tmp12C)!= 23)goto
_LL58;_tmp132=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp12C)->f1;_LL57: return
Cyc_Absyn_is_lvalue(_tmp132);_LL58: if(*((int*)_tmp12C)!= 14)goto _LL5A;_tmp133=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp12C)->f1;_LL59: return Cyc_Absyn_is_lvalue(
_tmp133);_LL5A: if(*((int*)_tmp12C)!= 13)goto _LL5C;_tmp134=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp12C)->f1;_LL5B: return Cyc_Absyn_is_lvalue(_tmp134);_LL5C:;_LL5D: return 0;_LL47:;}
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,
struct _dynforward_ptr*v){{struct Cyc_List_List*_tmp136=fields;for(0;_tmp136 != 0;
_tmp136=_tmp136->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp136->hd)->name,
v)== 0)return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)_tmp136->hd);}}
return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*
ad,struct _dynforward_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0)return 0;ts=ts->tl;}if(ts
== 0)return 0;return(struct _tuple3*)((struct _tuple3*)ts->hd);}struct
_dynforward_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp137=a;int _tmp138;int
_tmp139;struct _dynforward_ptr _tmp13A;void*_tmp13B;int _tmp13C;int _tmp13D;void*
_tmp13E;int _tmp13F;int _tmp140;int _tmp141;struct _dynforward_ptr _tmp142;_LL64: if(
_tmp137 <= (void*)17)goto _LL66;if(*((int*)_tmp137)!= 0)goto _LL66;_tmp138=((struct
Cyc_Absyn_Regparm_att_struct*)_tmp137)->f1;_LL65: return(struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp145;_tmp145.tag=1;_tmp145.f1=(unsigned long)_tmp138;{
void*_tmp143[1]={& _tmp145};Cyc_aprintf(({const char*_tmp144="regparm(%d)";
_tag_dynforward(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,12));}),
_tag_dynforward(_tmp143,sizeof(void*),1));}});_LL66: if((int)_tmp137 != 0)goto
_LL68;_LL67: return({const char*_tmp146="stdcall";_tag_dynforward(_tmp146,sizeof(
char),_get_zero_arr_size(_tmp146,8));});_LL68: if((int)_tmp137 != 1)goto _LL6A;
_LL69: return({const char*_tmp147="cdecl";_tag_dynforward(_tmp147,sizeof(char),
_get_zero_arr_size(_tmp147,6));});_LL6A: if((int)_tmp137 != 2)goto _LL6C;_LL6B:
return({const char*_tmp148="fastcall";_tag_dynforward(_tmp148,sizeof(char),
_get_zero_arr_size(_tmp148,9));});_LL6C: if((int)_tmp137 != 3)goto _LL6E;_LL6D:
return({const char*_tmp149="noreturn";_tag_dynforward(_tmp149,sizeof(char),
_get_zero_arr_size(_tmp149,9));});_LL6E: if((int)_tmp137 != 4)goto _LL70;_LL6F:
return({const char*_tmp14A="const";_tag_dynforward(_tmp14A,sizeof(char),
_get_zero_arr_size(_tmp14A,6));});_LL70: if(_tmp137 <= (void*)17)goto _LL72;if(*((
int*)_tmp137)!= 1)goto _LL72;_tmp139=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp137)->f1;_LL71: if(_tmp139 == - 1)return({const char*_tmp14B="aligned";
_tag_dynforward(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,8));});else{
return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp14E;_tmp14E.tag=1;
_tmp14E.f1=(unsigned long)_tmp139;{void*_tmp14C[1]={& _tmp14E};Cyc_aprintf(({
const char*_tmp14D="aligned(%d)";_tag_dynforward(_tmp14D,sizeof(char),
_get_zero_arr_size(_tmp14D,12));}),_tag_dynforward(_tmp14C,sizeof(void*),1));}});}
_LL72: if((int)_tmp137 != 5)goto _LL74;_LL73: return({const char*_tmp14F="packed";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size(_tmp14F,7));});_LL74: if(
_tmp137 <= (void*)17)goto _LL76;if(*((int*)_tmp137)!= 2)goto _LL76;_tmp13A=((struct
Cyc_Absyn_Section_att_struct*)_tmp137)->f1;_LL75: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp152;_tmp152.tag=0;_tmp152.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp13A);{void*_tmp150[1]={& _tmp152};Cyc_aprintf(({
const char*_tmp151="section(\"%s\")";_tag_dynforward(_tmp151,sizeof(char),
_get_zero_arr_size(_tmp151,14));}),_tag_dynforward(_tmp150,sizeof(void*),1));}});
_LL76: if((int)_tmp137 != 6)goto _LL78;_LL77: return({const char*_tmp153="nocommon";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size(_tmp153,9));});_LL78: if((
int)_tmp137 != 7)goto _LL7A;_LL79: return({const char*_tmp154="shared";
_tag_dynforward(_tmp154,sizeof(char),_get_zero_arr_size(_tmp154,7));});_LL7A: if((
int)_tmp137 != 8)goto _LL7C;_LL7B: return({const char*_tmp155="unused";
_tag_dynforward(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,7));});_LL7C: if((
int)_tmp137 != 9)goto _LL7E;_LL7D: return({const char*_tmp156="weak";_tag_dynforward(
_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,5));});_LL7E: if((int)_tmp137 != 
10)goto _LL80;_LL7F: return({const char*_tmp157="dllimport";_tag_dynforward(_tmp157,
sizeof(char),_get_zero_arr_size(_tmp157,10));});_LL80: if((int)_tmp137 != 11)goto
_LL82;_LL81: return({const char*_tmp158="dllexport";_tag_dynforward(_tmp158,
sizeof(char),_get_zero_arr_size(_tmp158,10));});_LL82: if((int)_tmp137 != 12)goto
_LL84;_LL83: return({const char*_tmp159="no_instrument_function";_tag_dynforward(
_tmp159,sizeof(char),_get_zero_arr_size(_tmp159,23));});_LL84: if((int)_tmp137 != 
13)goto _LL86;_LL85: return({const char*_tmp15A="constructor";_tag_dynforward(
_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,12));});_LL86: if((int)_tmp137 != 
14)goto _LL88;_LL87: return({const char*_tmp15B="destructor";_tag_dynforward(
_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,11));});_LL88: if((int)_tmp137 != 
15)goto _LL8A;_LL89: return({const char*_tmp15C="no_check_memory_usage";
_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,22));});_LL8A:
if(_tmp137 <= (void*)17)goto _LL90;if(*((int*)_tmp137)!= 3)goto _LL8C;_tmp13B=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp137)->f1;if((int)_tmp13B != 0)goto _LL8C;
_tmp13C=((struct Cyc_Absyn_Format_att_struct*)_tmp137)->f2;_tmp13D=((struct Cyc_Absyn_Format_att_struct*)
_tmp137)->f3;_LL8B: return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp160;
_tmp160.tag=1;_tmp160.f1=(unsigned int)_tmp13D;{struct Cyc_Int_pa_struct _tmp15F;
_tmp15F.tag=1;_tmp15F.f1=(unsigned int)_tmp13C;{void*_tmp15D[2]={& _tmp15F,&
_tmp160};Cyc_aprintf(({const char*_tmp15E="format(printf,%u,%u)";_tag_dynforward(
_tmp15E,sizeof(char),_get_zero_arr_size(_tmp15E,21));}),_tag_dynforward(_tmp15D,
sizeof(void*),2));}}});_LL8C: if(*((int*)_tmp137)!= 3)goto _LL8E;_tmp13E=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp137)->f1;if((int)_tmp13E != 1)goto _LL8E;
_tmp13F=((struct Cyc_Absyn_Format_att_struct*)_tmp137)->f2;_tmp140=((struct Cyc_Absyn_Format_att_struct*)
_tmp137)->f3;_LL8D: return(struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp164;
_tmp164.tag=1;_tmp164.f1=(unsigned int)_tmp140;{struct Cyc_Int_pa_struct _tmp163;
_tmp163.tag=1;_tmp163.f1=(unsigned int)_tmp13F;{void*_tmp161[2]={& _tmp163,&
_tmp164};Cyc_aprintf(({const char*_tmp162="format(scanf,%u,%u)";_tag_dynforward(
_tmp162,sizeof(char),_get_zero_arr_size(_tmp162,20));}),_tag_dynforward(_tmp161,
sizeof(void*),2));}}});_LL8E: if(*((int*)_tmp137)!= 4)goto _LL90;_tmp141=((struct
Cyc_Absyn_Initializes_att_struct*)_tmp137)->f1;_LL8F: return(struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp167;_tmp167.tag=1;_tmp167.f1=(
unsigned long)_tmp141;{void*_tmp165[1]={& _tmp167};Cyc_aprintf(({const char*
_tmp166="initializes(%d)";_tag_dynforward(_tmp166,sizeof(char),
_get_zero_arr_size(_tmp166,16));}),_tag_dynforward(_tmp165,sizeof(void*),1));}});
_LL90: if((int)_tmp137 != 16)goto _LL92;_LL91: return({const char*_tmp168="pure";
_tag_dynforward(_tmp168,sizeof(char),_get_zero_arr_size(_tmp168,5));});_LL92: if(
_tmp137 <= (void*)17)goto _LL63;if(*((int*)_tmp137)!= 5)goto _LL63;_tmp142=((struct
Cyc_Absyn_Mode_att_struct*)_tmp137)->f1;_LL93: return(struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp16B;_tmp16B.tag=0;_tmp16B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp142);{void*_tmp169[1]={& _tmp16B};Cyc_aprintf(({
const char*_tmp16A="__mode__(\"%s\")";_tag_dynforward(_tmp16A,sizeof(char),
_get_zero_arr_size(_tmp16A,15));}),_tag_dynforward(_tmp169,sizeof(void*),1));}});
_LL63:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp16C=a;_LL95: if(_tmp16C <= (void*)
17)goto _LL97;if(*((int*)_tmp16C)!= 0)goto _LL97;_LL96: goto _LL98;_LL97: if((int)
_tmp16C != 2)goto _LL99;_LL98: goto _LL9A;_LL99: if((int)_tmp16C != 0)goto _LL9B;_LL9A:
goto _LL9C;_LL9B: if((int)_tmp16C != 1)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if((int)
_tmp16C != 3)goto _LL9F;_LL9E: goto _LLA0;_LL9F: if((int)_tmp16C != 16)goto _LLA1;_LLA0:
goto _LLA2;_LLA1: if(_tmp16C <= (void*)17)goto _LLA3;if(*((int*)_tmp16C)!= 3)goto
_LLA3;_LLA2: goto _LLA4;_LLA3: if((int)_tmp16C != 4)goto _LLA5;_LLA4: return 1;_LLA5:
if(_tmp16C <= (void*)17)goto _LLA7;if(*((int*)_tmp16C)!= 4)goto _LLA7;_LLA6: return 1;
_LLA7:;_LLA8: return 0;_LL94:;}static char _tmp16D[3]="f0";static struct
_dynforward_ptr Cyc_Absyn_f0={_tmp16D,_tmp16D + 3};static struct _dynforward_ptr*Cyc_Absyn_field_names_v[
1]={& Cyc_Absyn_f0};static struct _dynforward_ptr Cyc_Absyn_field_names={(void*)((
struct _dynforward_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dynforward_ptr**)
Cyc_Absyn_field_names_v + 1)};struct _dynforward_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dynforward_size(Cyc_Absyn_field_names,sizeof(struct
_dynforward_ptr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int _tmp16E=(
unsigned int)(i + 1);struct _dynforward_ptr**_tmp16F=(struct _dynforward_ptr**)
_cycalloc(_check_times(sizeof(struct _dynforward_ptr*),_tmp16E));struct
_dynforward_ptr _tmp175=_tag_dynforward(_tmp16F,sizeof(struct _dynforward_ptr*),
_tmp16E);{unsigned int _tmp170=_tmp16E;unsigned int j;for(j=0;j < _tmp170;j ++){
_tmp16F[j]=j < fsz?*((struct _dynforward_ptr**)_check_dynforward_subscript(Cyc_Absyn_field_names,
sizeof(struct _dynforward_ptr*),(int)j)):({struct _dynforward_ptr*_tmp171=
_cycalloc(sizeof(*_tmp171));_tmp171[0]=(struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp174;_tmp174.tag=1;_tmp174.f1=(unsigned long)((int)j);{void*_tmp172[1]={&
_tmp174};Cyc_aprintf(({const char*_tmp173="f%d";_tag_dynforward(_tmp173,sizeof(
char),_get_zero_arr_size(_tmp173,4));}),_tag_dynforward(_tmp172,sizeof(void*),1));}});
_tmp171;});}}_tmp175;});return*((struct _dynforward_ptr**)
_check_dynforward_subscript(Cyc_Absyn_field_names,sizeof(struct _dynforward_ptr*),
i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*info){void*_tmp176=info;void*
_tmp177;struct _tuple0*_tmp178;struct Cyc_Absyn_Aggrdecl**_tmp179;struct Cyc_Absyn_Aggrdecl*
_tmp17A;struct Cyc_Absyn_Aggrdecl _tmp17B;void*_tmp17C;struct _tuple0*_tmp17D;_LLAA:
if(*((int*)_tmp176)!= 0)goto _LLAC;_tmp177=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp176)->f1;_tmp178=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp176)->f2;_LLAB:
return({struct _tuple4 _tmp17E;_tmp17E.f1=_tmp177;_tmp17E.f2=_tmp178;_tmp17E;});
_LLAC: if(*((int*)_tmp176)!= 1)goto _LLA9;_tmp179=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp176)->f1;_tmp17A=*_tmp179;_tmp17B=*_tmp17A;_tmp17C=(void*)_tmp17B.kind;
_tmp17D=_tmp17B.name;_LLAD: return({struct _tuple4 _tmp17F;_tmp17F.f1=_tmp17C;
_tmp17F.f2=_tmp17D;_tmp17F;});_LLA9:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info){void*_tmp180=info;void*_tmp181;struct _tuple0*_tmp182;struct Cyc_Absyn_Aggrdecl**
_tmp183;struct Cyc_Absyn_Aggrdecl*_tmp184;_LLAF: if(*((int*)_tmp180)!= 0)goto _LLB1;
_tmp181=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp180)->f1;_tmp182=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp180)->f2;_LLB0:({void*_tmp185[0]={};((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp186="unchecked aggrdecl";_tag_dynforward(_tmp186,sizeof(char),
_get_zero_arr_size(_tmp186,19));}),_tag_dynforward(_tmp185,sizeof(void*),0));});
_LLB1: if(*((int*)_tmp180)!= 1)goto _LLAE;_tmp183=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp180)->f1;_tmp184=*_tmp183;_LLB2: return _tmp184;_LLAE:;}int Cyc_Absyn_is_union_type(
void*t){void*_tmp187=Cyc_Tcutil_compress(t);void*_tmp188;struct Cyc_Absyn_AggrInfo
_tmp189;void*_tmp18A;_LLB4: if(_tmp187 <= (void*)4)goto _LLB8;if(*((int*)_tmp187)!= 
11)goto _LLB6;_tmp188=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp187)->f1;
if((int)_tmp188 != 1)goto _LLB6;_LLB5: return 1;_LLB6: if(*((int*)_tmp187)!= 10)goto
_LLB8;_tmp189=((struct Cyc_Absyn_AggrType_struct*)_tmp187)->f1;_tmp18A=(void*)
_tmp189.aggr_info;_LLB7: return(Cyc_Absyn_aggr_kinded_name(_tmp18A)).f1 == (void*)
1;_LLB8:;_LLB9: return 0;_LLB3:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*
decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp18B[0]={};Cyc_printf(({const char*_tmp18C="\n";_tag_dynforward(
_tmp18C,sizeof(char),_get_zero_arr_size(_tmp18C,2));}),_tag_dynforward(_tmp18B,
sizeof(void*),0));});}int Cyc_Absyn_porting_c_code=0;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_810;static struct Cyc_Typerep_Int_struct
Cyc__genrep_24={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static char _tmp18E[7]="Static";
static struct _tuple7 Cyc__gentuple_167={0,(struct _dynforward_ptr){_tmp18E,_tmp18E + 
7}};static char _tmp18F[9]="Abstract";static struct _tuple7 Cyc__gentuple_168={1,(
struct _dynforward_ptr){_tmp18F,_tmp18F + 9}};static char _tmp190[7]="Public";static
struct _tuple7 Cyc__gentuple_169={2,(struct _dynforward_ptr){_tmp190,_tmp190 + 7}};
static char _tmp191[7]="Extern";static struct _tuple7 Cyc__gentuple_170={3,(struct
_dynforward_ptr){_tmp191,_tmp191 + 7}};static char _tmp192[8]="ExternC";static
struct _tuple7 Cyc__gentuple_171={4,(struct _dynforward_ptr){_tmp192,_tmp192 + 8}};
static char _tmp193[9]="Register";static struct _tuple7 Cyc__gentuple_172={5,(struct
_dynforward_ptr){_tmp193,_tmp193 + 9}};static struct _tuple7*Cyc__genarr_173[6]={&
Cyc__gentuple_167,& Cyc__gentuple_168,& Cyc__gentuple_169,& Cyc__gentuple_170,& Cyc__gentuple_171,&
Cyc__gentuple_172};static struct _tuple5*Cyc__genarr_174[0]={};static char _tmp195[6]="Scope";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={5,(struct _dynforward_ptr){
_tmp195,_tmp195 + 6},{(void*)((struct _tuple7**)Cyc__genarr_173),(void*)((struct
_tuple7**)Cyc__genarr_173 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_174),(void*)((
struct _tuple5**)Cyc__genarr_174 + 0)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_12;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_nmspace_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_18;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_13;extern struct Cyc_Typerep_FatPtr_struct Cyc__genrep_14;static struct
Cyc_Typerep_Int_struct Cyc__genrep_15={0,0,8};static struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_14={2,(void*)((void*)& Cyc__genrep_15)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_13={1,1,(void*)((void*)& Cyc__genrep_14)};static char _tmp199[5]="List";
static struct _dynforward_ptr Cyc__genname_23=(struct _dynforward_ptr){_tmp199,
_tmp199 + 5};static char _tmp19A[3]="hd";static struct _tuple5 Cyc__gentuple_20={
offsetof(struct Cyc_List_List,hd),(struct _dynforward_ptr){_tmp19A,_tmp19A + 3},(
void*)& Cyc__genrep_13};static char _tmp19B[3]="tl";static struct _tuple5 Cyc__gentuple_21={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp19B,_tmp19B + 3},(
void*)& Cyc__genrep_19};static struct _tuple5*Cyc__genarr_22[2]={& Cyc__gentuple_20,&
Cyc__gentuple_21};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_23,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_22),(void*)((struct _tuple5**)Cyc__genarr_22 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple13{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_25={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_26={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_19};static struct _tuple6*Cyc__genarr_27[
2]={& Cyc__gentuple_25,& Cyc__gentuple_26};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_18={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_27),(void*)((
struct _tuple6**)Cyc__genarr_27 + 2)}};static char _tmp19F[6]="Loc_n";static struct
_tuple7 Cyc__gentuple_16={0,(struct _dynforward_ptr){_tmp19F,_tmp19F + 6}};static
struct _tuple7*Cyc__genarr_17[1]={& Cyc__gentuple_16};static char _tmp1A0[6]="Rel_n";
static struct _tuple5 Cyc__gentuple_28={0,(struct _dynforward_ptr){_tmp1A0,_tmp1A0 + 
6},(void*)& Cyc__genrep_18};static char _tmp1A1[6]="Abs_n";static struct _tuple5 Cyc__gentuple_29={
1,(struct _dynforward_ptr){_tmp1A1,_tmp1A1 + 6},(void*)& Cyc__genrep_18};static
struct _tuple5*Cyc__genarr_30[2]={& Cyc__gentuple_28,& Cyc__gentuple_29};static char
_tmp1A3[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={5,(
struct _dynforward_ptr){_tmp1A3,_tmp1A3 + 8},{(void*)((struct _tuple7**)Cyc__genarr_17),(
void*)((struct _tuple7**)Cyc__genarr_17 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_30),(
void*)((struct _tuple5**)Cyc__genarr_30 + 2)}};static struct _tuple6 Cyc__gentuple_31={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_32={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_13};static
struct _tuple6*Cyc__genarr_33[2]={& Cyc__gentuple_31,& Cyc__gentuple_32};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_12={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_33),(void*)((struct _tuple6**)Cyc__genarr_33 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={1,1,(void*)((void*)& Cyc__genrep_12)};
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep;static struct Cyc_Typerep_Int_struct
Cyc__genrep_10={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};
static char _tmp1A8[6]="Tqual";static struct _dynforward_ptr Cyc__genname_166=(struct
_dynforward_ptr){_tmp1A8,_tmp1A8 + 6};static char _tmp1A9[12]="print_const";static
struct _tuple5 Cyc__gentuple_160={offsetof(struct Cyc_Absyn_Tqual,print_const),(
struct _dynforward_ptr){_tmp1A9,_tmp1A9 + 12},(void*)((void*)& Cyc__genrep_10)};
static char _tmp1AA[11]="q_volatile";static struct _tuple5 Cyc__gentuple_161={
offsetof(struct Cyc_Absyn_Tqual,q_volatile),(struct _dynforward_ptr){_tmp1AA,
_tmp1AA + 11},(void*)((void*)& Cyc__genrep_10)};static char _tmp1AB[11]="q_restrict";
static struct _tuple5 Cyc__gentuple_162={offsetof(struct Cyc_Absyn_Tqual,q_restrict),(
struct _dynforward_ptr){_tmp1AB,_tmp1AB + 11},(void*)((void*)& Cyc__genrep_10)};
static char _tmp1AC[11]="real_const";static struct _tuple5 Cyc__gentuple_163={
offsetof(struct Cyc_Absyn_Tqual,real_const),(struct _dynforward_ptr){_tmp1AC,
_tmp1AC + 11},(void*)((void*)& Cyc__genrep_10)};static char _tmp1AD[4]="loc";static
struct _tuple5 Cyc__gentuple_164={offsetof(struct Cyc_Absyn_Tqual,loc),(struct
_dynforward_ptr){_tmp1AD,_tmp1AD + 4},(void*)& Cyc__genrep_2};static struct _tuple5*
Cyc__genarr_165[5]={& Cyc__gentuple_160,& Cyc__gentuple_161,& Cyc__gentuple_162,&
Cyc__gentuple_163,& Cyc__gentuple_164};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_166,sizeof(struct Cyc_Absyn_Tqual),{(void*)((
struct _tuple5**)Cyc__genarr_165),(void*)((struct _tuple5**)Cyc__genarr_165 + 5)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1102;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1107;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1AF[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_188={0,(struct _dynforward_ptr){_tmp1AF,_tmp1AF + 
8}};static char _tmp1B0[8]="MemKind";static struct _tuple7 Cyc__gentuple_189={1,(
struct _dynforward_ptr){_tmp1B0,_tmp1B0 + 8}};static char _tmp1B1[8]="BoxKind";
static struct _tuple7 Cyc__gentuple_190={2,(struct _dynforward_ptr){_tmp1B1,_tmp1B1 + 
8}};static char _tmp1B2[8]="RgnKind";static struct _tuple7 Cyc__gentuple_191={3,(
struct _dynforward_ptr){_tmp1B2,_tmp1B2 + 8}};static char _tmp1B3[8]="EffKind";
static struct _tuple7 Cyc__gentuple_192={4,(struct _dynforward_ptr){_tmp1B3,_tmp1B3 + 
8}};static char _tmp1B4[8]="IntKind";static struct _tuple7 Cyc__gentuple_193={5,(
struct _dynforward_ptr){_tmp1B4,_tmp1B4 + 8}};static struct _tuple7*Cyc__genarr_194[
6]={& Cyc__gentuple_188,& Cyc__gentuple_189,& Cyc__gentuple_190,& Cyc__gentuple_191,&
Cyc__gentuple_192,& Cyc__gentuple_193};static struct _tuple5*Cyc__genarr_195[0]={};
static char _tmp1B6[5]="Kind";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep={
5,(struct _dynforward_ptr){_tmp1B6,_tmp1B6 + 5},{(void*)((struct _tuple7**)Cyc__genarr_194),(
void*)((struct _tuple7**)Cyc__genarr_194 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_195),(
void*)((struct _tuple5**)Cyc__genarr_195 + 0)}};static char _tmp1B7[4]="Opt";static
struct _dynforward_ptr Cyc__genname_1110=(struct _dynforward_ptr){_tmp1B7,_tmp1B7 + 
4};static char _tmp1B8[2]="v";static struct _tuple5 Cyc__gentuple_1108={offsetof(
struct Cyc_Core_Opt,v),(struct _dynforward_ptr){_tmp1B8,_tmp1B8 + 2},(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple5*Cyc__genarr_1109[1]={& Cyc__gentuple_1108};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1110,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1109),(void*)((
struct _tuple5**)Cyc__genarr_1109 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1107={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_110;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep;
static char _tmp1BB[4]="Opt";static struct _dynforward_ptr Cyc__genname_1142=(struct
_dynforward_ptr){_tmp1BB,_tmp1BB + 4};static char _tmp1BC[2]="v";static struct
_tuple5 Cyc__gentuple_1140={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1BC,_tmp1BC + 2},(void*)& Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1141[
1]={& Cyc__gentuple_1140};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1142,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1141),(void*)((struct _tuple5**)Cyc__genarr_1141 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1103;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_321;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_185;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tvar_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_216;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_216={1,1,(void*)((void*)& Cyc__genrep_10)};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_208;static struct _tuple6 Cyc__gentuple_209={offsetof(struct _tuple6,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_210={offsetof(struct
_tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*Cyc__genarr_211[2]={&
Cyc__gentuple_209,& Cyc__gentuple_210};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_208={
4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_211),(void*)((
struct _tuple6**)Cyc__genarr_211 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_204;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_196;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;static char _tmp1C1[4]="Opt";static
struct _dynforward_ptr Cyc__genname_199=(struct _dynforward_ptr){_tmp1C1,_tmp1C1 + 4};
static char _tmp1C2[2]="v";static struct _tuple5 Cyc__gentuple_197={offsetof(struct
Cyc_Core_Opt,v),(struct _dynforward_ptr){_tmp1C2,_tmp1C2 + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_198[1]={& Cyc__gentuple_197};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_199,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_198),(void*)((
struct _tuple5**)Cyc__genarr_198 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_196={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};struct _tuple14{
unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_205={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_206={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_196};static struct _tuple6*Cyc__genarr_207[
2]={& Cyc__gentuple_205,& Cyc__gentuple_206};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_204={4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_207),(
void*)((struct _tuple6**)Cyc__genarr_207 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_187;struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};
static struct _tuple6 Cyc__gentuple_200={offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_201={offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_196};
static struct _tuple6 Cyc__gentuple_202={offsetof(struct _tuple15,f3),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_203[3]={& Cyc__gentuple_200,& Cyc__gentuple_201,&
Cyc__gentuple_202};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_187={4,
sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_203),(void*)((
struct _tuple6**)Cyc__genarr_203 + 3)}};static struct _tuple7*Cyc__genarr_186[0]={};
static char _tmp1C7[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_212={0,(struct
_dynforward_ptr){_tmp1C7,_tmp1C7 + 6},(void*)& Cyc__genrep_208};static char _tmp1C8[
11]="Unknown_kb";static struct _tuple5 Cyc__gentuple_213={1,(struct _dynforward_ptr){
_tmp1C8,_tmp1C8 + 11},(void*)& Cyc__genrep_204};static char _tmp1C9[8]="Less_kb";
static struct _tuple5 Cyc__gentuple_214={2,(struct _dynforward_ptr){_tmp1C9,_tmp1C9 + 
8},(void*)& Cyc__genrep_187};static struct _tuple5*Cyc__genarr_215[3]={& Cyc__gentuple_212,&
Cyc__gentuple_213,& Cyc__gentuple_214};static char _tmp1CB[10]="KindBound";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={5,(struct _dynforward_ptr){
_tmp1CB,_tmp1CB + 10},{(void*)((struct _tuple7**)Cyc__genarr_186),(void*)((struct
_tuple7**)Cyc__genarr_186 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_215),(void*)((
struct _tuple5**)Cyc__genarr_215 + 3)}};static char _tmp1CC[5]="Tvar";static struct
_dynforward_ptr Cyc__genname_221=(struct _dynforward_ptr){_tmp1CC,_tmp1CC + 5};
static char _tmp1CD[5]="name";static struct _tuple5 Cyc__gentuple_217={offsetof(
struct Cyc_Absyn_Tvar,name),(struct _dynforward_ptr){_tmp1CD,_tmp1CD + 5},(void*)&
Cyc__genrep_13};static char _tmp1CE[9]="identity";static struct _tuple5 Cyc__gentuple_218={
offsetof(struct Cyc_Absyn_Tvar,identity),(struct _dynforward_ptr){_tmp1CE,_tmp1CE + 
9},(void*)& Cyc__genrep_216};static char _tmp1CF[5]="kind";static struct _tuple5 Cyc__gentuple_219={
offsetof(struct Cyc_Absyn_Tvar,kind),(struct _dynforward_ptr){_tmp1CF,_tmp1CF + 5},(
void*)& Cyc_Absyn_kindbound_t_rep};static struct _tuple5*Cyc__genarr_220[3]={& Cyc__gentuple_217,&
Cyc__gentuple_218,& Cyc__gentuple_219};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_221,sizeof(struct Cyc_Absyn_Tvar),{(void*)((
struct _tuple5**)Cyc__genarr_220),(void*)((struct _tuple5**)Cyc__genarr_220 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_185={1,1,(void*)((void*)& Cyc_struct_Absyn_Tvar_rep)};
static char _tmp1D2[5]="List";static struct _dynforward_ptr Cyc__genname_325=(struct
_dynforward_ptr){_tmp1D2,_tmp1D2 + 5};static char _tmp1D3[3]="hd";static struct
_tuple5 Cyc__gentuple_322={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp1D3,_tmp1D3 + 3},(void*)& Cyc__genrep_185};static char _tmp1D4[
3]="tl";static struct _tuple5 Cyc__gentuple_323={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp1D4,_tmp1D4 + 3},(void*)& Cyc__genrep_321};static struct
_tuple5*Cyc__genarr_324[2]={& Cyc__gentuple_322,& Cyc__gentuple_323};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_325,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_324),(void*)((
struct _tuple5**)Cyc__genarr_324 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_321={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1D7[4]="Opt";static struct _dynforward_ptr Cyc__genname_1106=(struct
_dynforward_ptr){_tmp1D7,_tmp1D7 + 4};static char _tmp1D8[2]="v";static struct
_tuple5 Cyc__gentuple_1104={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1D8,_tmp1D8 + 2},(void*)& Cyc__genrep_321};static struct _tuple5*Cyc__genarr_1105[
1]={& Cyc__gentuple_1104};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1106,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1105),(void*)((struct _tuple5**)Cyc__genarr_1105 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1103={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};
struct _tuple16{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;
struct Cyc_Core_Opt*f5;};static struct _tuple6 Cyc__gentuple_1111={offsetof(struct
_tuple16,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1112={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_1107};static struct _tuple6 Cyc__gentuple_1113={
offsetof(struct _tuple16,f3),(void*)& Cyc__genrep_110};static struct _tuple6 Cyc__gentuple_1114={
offsetof(struct _tuple16,f4),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_1115={offsetof(struct _tuple16,f5),(void*)& Cyc__genrep_1103};static
struct _tuple6*Cyc__genarr_1116[5]={& Cyc__gentuple_1111,& Cyc__gentuple_1112,& Cyc__gentuple_1113,&
Cyc__gentuple_1114,& Cyc__gentuple_1115};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1102={
4,sizeof(struct _tuple16),{(void*)((struct _tuple6**)Cyc__genarr_1116),(void*)((
struct _tuple6**)Cyc__genarr_1116 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1098;
struct _tuple17{unsigned int f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1099={
offsetof(struct _tuple17,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1100={
offsetof(struct _tuple17,f2),(void*)& Cyc__genrep_185};static struct _tuple6*Cyc__genarr_1101[
2]={& Cyc__gentuple_1099,& Cyc__gentuple_1100};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1098={4,sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1101),(
void*)((struct _tuple6**)Cyc__genarr_1101 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1071;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1078;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1DD[18]="UnknownTunionInfo";
static struct _dynforward_ptr Cyc__genname_1083=(struct _dynforward_ptr){_tmp1DD,
_tmp1DD + 18};static char _tmp1DE[5]="name";static struct _tuple5 Cyc__gentuple_1079={
offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),(struct _dynforward_ptr){_tmp1DE,
_tmp1DE + 5},(void*)& Cyc__genrep_11};static char _tmp1DF[11]="is_xtunion";static
struct _tuple5 Cyc__gentuple_1080={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),(
struct _dynforward_ptr){_tmp1DF,_tmp1DF + 11},(void*)((void*)& Cyc__genrep_10)};
static char _tmp1E0[8]="is_flat";static struct _tuple5 Cyc__gentuple_1081={offsetof(
struct Cyc_Absyn_UnknownTunionInfo,is_flat),(struct _dynforward_ptr){_tmp1E0,
_tmp1E0 + 8},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_1082[
3]={& Cyc__gentuple_1079,& Cyc__gentuple_1080,& Cyc__gentuple_1081};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1083,
sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1082),(
void*)((struct _tuple5**)Cyc__genarr_1082 + 3)}};struct _tuple18{unsigned int f1;
struct Cyc_Absyn_UnknownTunionInfo f2;};static struct _tuple6 Cyc__gentuple_1084={
offsetof(struct _tuple18,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1085={
offsetof(struct _tuple18,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1086[2]={& Cyc__gentuple_1084,& Cyc__gentuple_1085};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1078={4,sizeof(struct _tuple18),{(
void*)((struct _tuple6**)Cyc__genarr_1086),(void*)((struct _tuple6**)Cyc__genarr_1086
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1073;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1074;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_311;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_312;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_313;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_294;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_295;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_296;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_297;static struct
_tuple6 Cyc__gentuple_298={offsetof(struct _tuple3,f1),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_299={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_300[2]={& Cyc__gentuple_298,&
Cyc__gentuple_299};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_297={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_300),(void*)((struct
_tuple6**)Cyc__genarr_300 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_296={
1,1,(void*)((void*)& Cyc__genrep_297)};static char _tmp1E5[5]="List";static struct
_dynforward_ptr Cyc__genname_304=(struct _dynforward_ptr){_tmp1E5,_tmp1E5 + 5};
static char _tmp1E6[3]="hd";static struct _tuple5 Cyc__gentuple_301={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp1E6,_tmp1E6 + 3},(void*)& Cyc__genrep_296};
static char _tmp1E7[3]="tl";static struct _tuple5 Cyc__gentuple_302={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp1E7,_tmp1E7 + 3},(void*)& Cyc__genrep_295};
static struct _tuple5*Cyc__genarr_303[2]={& Cyc__gentuple_301,& Cyc__gentuple_302};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_304,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_303),(void*)((struct _tuple5**)Cyc__genarr_303 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_295={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
static char _tmp1EA[12]="Tunionfield";static struct _dynforward_ptr Cyc__genname_310=(
struct _dynforward_ptr){_tmp1EA,_tmp1EA + 12};static char _tmp1EB[5]="name";static
struct _tuple5 Cyc__gentuple_305={offsetof(struct Cyc_Absyn_Tunionfield,name),(
struct _dynforward_ptr){_tmp1EB,_tmp1EB + 5},(void*)& Cyc__genrep_11};static char
_tmp1EC[5]="typs";static struct _tuple5 Cyc__gentuple_306={offsetof(struct Cyc_Absyn_Tunionfield,typs),(
struct _dynforward_ptr){_tmp1EC,_tmp1EC + 5},(void*)& Cyc__genrep_295};static char
_tmp1ED[4]="loc";static struct _tuple5 Cyc__gentuple_307={offsetof(struct Cyc_Absyn_Tunionfield,loc),(
struct _dynforward_ptr){_tmp1ED,_tmp1ED + 4},(void*)& Cyc__genrep_2};static char
_tmp1EE[3]="sc";static struct _tuple5 Cyc__gentuple_308={offsetof(struct Cyc_Absyn_Tunionfield,sc),(
struct _dynforward_ptr){_tmp1EE,_tmp1EE + 3},(void*)& Cyc_Absyn_scope_t_rep};static
struct _tuple5*Cyc__genarr_309[4]={& Cyc__gentuple_305,& Cyc__gentuple_306,& Cyc__gentuple_307,&
Cyc__gentuple_308};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_310,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_309),(void*)((struct _tuple5**)Cyc__genarr_309
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_294={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tunionfield_rep)};static char _tmp1F1[5]="List";static struct
_dynforward_ptr Cyc__genname_317=(struct _dynforward_ptr){_tmp1F1,_tmp1F1 + 5};
static char _tmp1F2[3]="hd";static struct _tuple5 Cyc__gentuple_314={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp1F2,_tmp1F2 + 3},(void*)& Cyc__genrep_294};
static char _tmp1F3[3]="tl";static struct _tuple5 Cyc__gentuple_315={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp1F3,_tmp1F3 + 3},(void*)& Cyc__genrep_313};
static struct _tuple5*Cyc__genarr_316[2]={& Cyc__gentuple_314,& Cyc__gentuple_315};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_317,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_316),(void*)((struct _tuple5**)Cyc__genarr_316 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_313={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};
static char _tmp1F6[4]="Opt";static struct _dynforward_ptr Cyc__genname_320=(struct
_dynforward_ptr){_tmp1F6,_tmp1F6 + 4};static char _tmp1F7[2]="v";static struct
_tuple5 Cyc__gentuple_318={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp1F7,_tmp1F7 + 2},(void*)& Cyc__genrep_313};static struct _tuple5*Cyc__genarr_319[
1]={& Cyc__gentuple_318};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_320,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_319),(void*)((struct _tuple5**)Cyc__genarr_319 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_312={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp1FA[11]="Tuniondecl";static struct _dynforward_ptr Cyc__genname_333=(
struct _dynforward_ptr){_tmp1FA,_tmp1FA + 11};static char _tmp1FB[3]="sc";static
struct _tuple5 Cyc__gentuple_326={offsetof(struct Cyc_Absyn_Tuniondecl,sc),(struct
_dynforward_ptr){_tmp1FB,_tmp1FB + 3},(void*)& Cyc_Absyn_scope_t_rep};static char
_tmp1FC[5]="name";static struct _tuple5 Cyc__gentuple_327={offsetof(struct Cyc_Absyn_Tuniondecl,name),(
struct _dynforward_ptr){_tmp1FC,_tmp1FC + 5},(void*)& Cyc__genrep_11};static char
_tmp1FD[4]="tvs";static struct _tuple5 Cyc__gentuple_328={offsetof(struct Cyc_Absyn_Tuniondecl,tvs),(
struct _dynforward_ptr){_tmp1FD,_tmp1FD + 4},(void*)& Cyc__genrep_321};static char
_tmp1FE[7]="fields";static struct _tuple5 Cyc__gentuple_329={offsetof(struct Cyc_Absyn_Tuniondecl,fields),(
struct _dynforward_ptr){_tmp1FE,_tmp1FE + 7},(void*)& Cyc__genrep_312};static char
_tmp1FF[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_330={offsetof(struct
Cyc_Absyn_Tuniondecl,is_xtunion),(struct _dynforward_ptr){_tmp1FF,_tmp1FF + 11},(
void*)((void*)& Cyc__genrep_10)};static char _tmp200[8]="is_flat";static struct
_tuple5 Cyc__gentuple_331={offsetof(struct Cyc_Absyn_Tuniondecl,is_flat),(struct
_dynforward_ptr){_tmp200,_tmp200 + 8},(void*)((void*)& Cyc__genrep_10)};static
struct _tuple5*Cyc__genarr_332[6]={& Cyc__gentuple_326,& Cyc__gentuple_327,& Cyc__gentuple_328,&
Cyc__gentuple_329,& Cyc__gentuple_330,& Cyc__gentuple_331};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep={3,(struct _dynforward_ptr*)& Cyc__genname_333,
sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((struct _tuple5**)Cyc__genarr_332),(
void*)((struct _tuple5**)Cyc__genarr_332 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_311={1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1074={1,1,(void*)((void*)& Cyc__genrep_311)};
struct _tuple19{unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct
_tuple6 Cyc__gentuple_1075={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1076={offsetof(struct _tuple19,f2),(void*)& Cyc__genrep_1074};
static struct _tuple6*Cyc__genarr_1077[2]={& Cyc__gentuple_1075,& Cyc__gentuple_1076};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1073={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1077),(void*)((struct _tuple6**)Cyc__genarr_1077
+ 2)}};static struct _tuple7*Cyc__genarr_1072[0]={};static char _tmp205[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1087={0,(struct _dynforward_ptr){_tmp205,_tmp205
+ 14},(void*)& Cyc__genrep_1078};static char _tmp206[12]="KnownTunion";static struct
_tuple5 Cyc__gentuple_1088={1,(struct _dynforward_ptr){_tmp206,_tmp206 + 12},(void*)&
Cyc__genrep_1073};static struct _tuple5*Cyc__genarr_1089[2]={& Cyc__gentuple_1087,&
Cyc__gentuple_1088};static char _tmp208[12]="TunionInfoU";struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_TunionInfoU_rep={5,(struct _dynforward_ptr){_tmp208,_tmp208 + 12},{(
void*)((struct _tuple7**)Cyc__genarr_1072),(void*)((struct _tuple7**)Cyc__genarr_1072
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1089),(void*)((struct _tuple5**)Cyc__genarr_1089
+ 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_type_t46H2_rep;static char _tmp209[5]="List";static
struct _dynforward_ptr Cyc__genname_125=(struct _dynforward_ptr){_tmp209,_tmp209 + 5};
static char _tmp20A[3]="hd";static struct _tuple5 Cyc__gentuple_122={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp20A,_tmp20A + 3},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp20B[3]="tl";static struct _tuple5 Cyc__gentuple_123={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp20B,_tmp20B + 3},(
void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_124[2]={& Cyc__gentuple_122,&
Cyc__gentuple_123};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_125,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_124),(void*)((struct _tuple5**)Cyc__genarr_124 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};
static char _tmp20E[11]="TunionInfo";static struct _dynforward_ptr Cyc__genname_1094=(
struct _dynforward_ptr){_tmp20E,_tmp20E + 11};static char _tmp20F[12]="tunion_info";
static struct _tuple5 Cyc__gentuple_1090={offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),(
struct _dynforward_ptr){_tmp20F,_tmp20F + 12},(void*)& Cyc_tunion_Absyn_TunionInfoU_rep};
static char _tmp210[6]="targs";static struct _tuple5 Cyc__gentuple_1091={offsetof(
struct Cyc_Absyn_TunionInfo,targs),(struct _dynforward_ptr){_tmp210,_tmp210 + 6},(
void*)& Cyc__genrep_121};static char _tmp211[4]="rgn";static struct _tuple5 Cyc__gentuple_1092={
offsetof(struct Cyc_Absyn_TunionInfo,rgn),(struct _dynforward_ptr){_tmp211,_tmp211
+ 4},(void*)& Cyc__genrep_110};static struct _tuple5*Cyc__genarr_1093[3]={& Cyc__gentuple_1090,&
Cyc__gentuple_1091,& Cyc__gentuple_1092};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1094,sizeof(struct Cyc_Absyn_TunionInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1093),(void*)((struct _tuple5**)Cyc__genarr_1093
+ 3)}};struct _tuple20{unsigned int f1;struct Cyc_Absyn_TunionInfo f2;};static struct
_tuple6 Cyc__gentuple_1095={offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1096={offsetof(struct _tuple20,f2),(void*)& Cyc_Absyn_tunion_info_t_rep};
static struct _tuple6*Cyc__genarr_1097[2]={& Cyc__gentuple_1095,& Cyc__gentuple_1096};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1071={4,sizeof(struct _tuple20),{(
void*)((struct _tuple6**)Cyc__genarr_1097),(void*)((struct _tuple6**)Cyc__genarr_1097
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1045;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1052;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp214[23]="UnknownTunionFieldInfo";
static struct _dynforward_ptr Cyc__genname_1057=(struct _dynforward_ptr){_tmp214,
_tmp214 + 23};static char _tmp215[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1053={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),(struct
_dynforward_ptr){_tmp215,_tmp215 + 12},(void*)& Cyc__genrep_11};static char _tmp216[
11]="field_name";static struct _tuple5 Cyc__gentuple_1054={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),(
struct _dynforward_ptr){_tmp216,_tmp216 + 11},(void*)& Cyc__genrep_11};static char
_tmp217[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_1055={offsetof(struct
Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),(struct _dynforward_ptr){_tmp217,
_tmp217 + 11},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_1056[
3]={& Cyc__gentuple_1053,& Cyc__gentuple_1054,& Cyc__gentuple_1055};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1057,
sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(void*)((struct _tuple5**)Cyc__genarr_1056),(
void*)((struct _tuple5**)Cyc__genarr_1056 + 3)}};struct _tuple21{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1058={
offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1059={
offsetof(struct _tuple21,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1060[2]={& Cyc__gentuple_1058,& Cyc__gentuple_1059};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1052={4,sizeof(struct _tuple21),{(
void*)((struct _tuple6**)Cyc__genarr_1060),(void*)((struct _tuple6**)Cyc__genarr_1060
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1047;struct _tuple22{
unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;};
static struct _tuple6 Cyc__gentuple_1048={offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1049={offsetof(struct _tuple22,f2),(void*)((void*)&
Cyc__genrep_311)};static struct _tuple6 Cyc__gentuple_1050={offsetof(struct _tuple22,f3),(
void*)& Cyc__genrep_294};static struct _tuple6*Cyc__genarr_1051[3]={& Cyc__gentuple_1048,&
Cyc__gentuple_1049,& Cyc__gentuple_1050};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1047={
4,sizeof(struct _tuple22),{(void*)((struct _tuple6**)Cyc__genarr_1051),(void*)((
struct _tuple6**)Cyc__genarr_1051 + 3)}};static struct _tuple7*Cyc__genarr_1046[0]={};
static char _tmp21B[19]="UnknownTunionfield";static struct _tuple5 Cyc__gentuple_1061={
0,(struct _dynforward_ptr){_tmp21B,_tmp21B + 19},(void*)& Cyc__genrep_1052};static
char _tmp21C[17]="KnownTunionfield";static struct _tuple5 Cyc__gentuple_1062={1,(
struct _dynforward_ptr){_tmp21C,_tmp21C + 17},(void*)& Cyc__genrep_1047};static
struct _tuple5*Cyc__genarr_1063[2]={& Cyc__gentuple_1061,& Cyc__gentuple_1062};
static char _tmp21E[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={
5,(struct _dynforward_ptr){_tmp21E,_tmp21E + 17},{(void*)((struct _tuple7**)Cyc__genarr_1046),(
void*)((struct _tuple7**)Cyc__genarr_1046 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_1063),(
void*)((struct _tuple5**)Cyc__genarr_1063 + 2)}};static char _tmp21F[16]="TunionFieldInfo";
static struct _dynforward_ptr Cyc__genname_1067=(struct _dynforward_ptr){_tmp21F,
_tmp21F + 16};static char _tmp220[11]="field_info";static struct _tuple5 Cyc__gentuple_1064={
offsetof(struct Cyc_Absyn_TunionFieldInfo,field_info),(struct _dynforward_ptr){
_tmp220,_tmp220 + 11},(void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};static char
_tmp221[6]="targs";static struct _tuple5 Cyc__gentuple_1065={offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),(
struct _dynforward_ptr){_tmp221,_tmp221 + 6},(void*)& Cyc__genrep_121};static struct
_tuple5*Cyc__genarr_1066[2]={& Cyc__gentuple_1064,& Cyc__gentuple_1065};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1067,
sizeof(struct Cyc_Absyn_TunionFieldInfo),{(void*)((struct _tuple5**)Cyc__genarr_1066),(
void*)((struct _tuple5**)Cyc__genarr_1066 + 2)}};struct _tuple23{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1068={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1069={
offsetof(struct _tuple23,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1070[2]={& Cyc__gentuple_1068,& Cyc__gentuple_1069};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1045={4,sizeof(struct _tuple23),{(
void*)((struct _tuple6**)Cyc__genarr_1070),(void*)((struct _tuple6**)Cyc__genarr_1070
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1019;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_ptr_info_t_rep;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_984;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;struct _tuple24{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_86={offsetof(struct _tuple24,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_87={offsetof(struct _tuple24,f2),(
void*)((void*)& Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_88[2]={& Cyc__gentuple_86,&
Cyc__gentuple_87};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple24),{(void*)((struct _tuple6**)Cyc__genarr_88),(void*)((struct _tuple6**)
Cyc__genarr_88 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_987;struct
_tuple25{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static struct _tuple6 Cyc__gentuple_988={
offsetof(struct _tuple25,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_989={
offsetof(struct _tuple25,f2),(void*)& Cyc__genrep_984};static struct _tuple6*Cyc__genarr_990[
2]={& Cyc__gentuple_988,& Cyc__gentuple_989};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_987={4,sizeof(struct _tuple25),{(void*)((struct _tuple6**)Cyc__genarr_990),(
void*)((struct _tuple6**)Cyc__genarr_990 + 2)}};static char _tmp226[10]="No_constr";
static struct _tuple7 Cyc__gentuple_985={0,(struct _dynforward_ptr){_tmp226,_tmp226 + 
10}};static struct _tuple7*Cyc__genarr_986[1]={& Cyc__gentuple_985};static char
_tmp227[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_991={0,(struct
_dynforward_ptr){_tmp227,_tmp227 + 10},(void*)& Cyc__genrep_85};static char _tmp228[
15]="Forward_constr";static struct _tuple5 Cyc__gentuple_992={1,(struct
_dynforward_ptr){_tmp228,_tmp228 + 15},(void*)& Cyc__genrep_987};static struct
_tuple5*Cyc__genarr_993[2]={& Cyc__gentuple_991,& Cyc__gentuple_992};static char
_tmp22A[11]="Constraint";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={
5,(struct _dynforward_ptr){_tmp22A,_tmp22A + 11},{(void*)((struct _tuple7**)Cyc__genarr_986),(
void*)((struct _tuple7**)Cyc__genarr_986 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_993),(
void*)((struct _tuple5**)Cyc__genarr_993 + 2)}};static char _tmp22B[7]="Conref";
static struct _dynforward_ptr Cyc__genname_996=(struct _dynforward_ptr){_tmp22B,
_tmp22B + 7};static char _tmp22C[2]="v";static struct _tuple5 Cyc__gentuple_994={
offsetof(struct Cyc_Absyn_Conref,v),(struct _dynforward_ptr){_tmp22C,_tmp22C + 2},(
void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};static struct _tuple5*Cyc__genarr_995[
1]={& Cyc__gentuple_994};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_996,sizeof(struct Cyc_Absyn_Conref),{(
void*)((struct _tuple5**)Cyc__genarr_995),(void*)((struct _tuple5**)Cyc__genarr_995
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_984={1,1,(void*)((void*)&
Cyc_struct_Absyn_Conref0bool2_rep)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1026;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;
static char _tmp22F[7]="Conref";static struct _dynforward_ptr Cyc__genname_1029=(
struct _dynforward_ptr){_tmp22F,_tmp22F + 7};static char _tmp230[2]="v";static struct
_tuple5 Cyc__gentuple_1027={offsetof(struct Cyc_Absyn_Conref,v),(struct
_dynforward_ptr){_tmp230,_tmp230 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_1028[1]={& Cyc__gentuple_1027};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1029,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_1028),(void*)((
struct _tuple5**)Cyc__genarr_1028 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1026={
1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_1020;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_PtrLoc_rep;static char _tmp233[7]="PtrLoc";static struct
_dynforward_ptr Cyc__genname_1025=(struct _dynforward_ptr){_tmp233,_tmp233 + 7};
static char _tmp234[8]="ptr_loc";static struct _tuple5 Cyc__gentuple_1021={offsetof(
struct Cyc_Absyn_PtrLoc,ptr_loc),(struct _dynforward_ptr){_tmp234,_tmp234 + 8},(
void*)& Cyc__genrep_2};static char _tmp235[8]="rgn_loc";static struct _tuple5 Cyc__gentuple_1022={
offsetof(struct Cyc_Absyn_PtrLoc,rgn_loc),(struct _dynforward_ptr){_tmp235,_tmp235
+ 8},(void*)& Cyc__genrep_2};static char _tmp236[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1023={
offsetof(struct Cyc_Absyn_PtrLoc,zt_loc),(struct _dynforward_ptr){_tmp236,_tmp236 + 
7},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1024[3]={& Cyc__gentuple_1021,&
Cyc__gentuple_1022,& Cyc__gentuple_1023};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_PtrLoc_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1025,sizeof(struct Cyc_Absyn_PtrLoc),{(
void*)((struct _tuple5**)Cyc__genarr_1024),(void*)((struct _tuple5**)Cyc__genarr_1024
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1020={1,1,(void*)((void*)&
Cyc_struct_Absyn_PtrLoc_rep)};static char _tmp239[8]="PtrAtts";static struct
_dynforward_ptr Cyc__genname_1036=(struct _dynforward_ptr){_tmp239,_tmp239 + 8};
static char _tmp23A[4]="rgn";static struct _tuple5 Cyc__gentuple_1030={offsetof(
struct Cyc_Absyn_PtrAtts,rgn),(struct _dynforward_ptr){_tmp23A,_tmp23A + 4},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp23B[9]="nullable";static struct
_tuple5 Cyc__gentuple_1031={offsetof(struct Cyc_Absyn_PtrAtts,nullable),(struct
_dynforward_ptr){_tmp23B,_tmp23B + 9},(void*)& Cyc__genrep_984};static char _tmp23C[
7]="bounds";static struct _tuple5 Cyc__gentuple_1032={offsetof(struct Cyc_Absyn_PtrAtts,bounds),(
struct _dynforward_ptr){_tmp23C,_tmp23C + 7},(void*)& Cyc__genrep_1026};static char
_tmp23D[10]="zero_term";static struct _tuple5 Cyc__gentuple_1033={offsetof(struct
Cyc_Absyn_PtrAtts,zero_term),(struct _dynforward_ptr){_tmp23D,_tmp23D + 10},(void*)&
Cyc__genrep_984};static char _tmp23E[7]="ptrloc";static struct _tuple5 Cyc__gentuple_1034={
offsetof(struct Cyc_Absyn_PtrAtts,ptrloc),(struct _dynforward_ptr){_tmp23E,_tmp23E
+ 7},(void*)& Cyc__genrep_1020};static struct _tuple5*Cyc__genarr_1035[5]={& Cyc__gentuple_1030,&
Cyc__gentuple_1031,& Cyc__gentuple_1032,& Cyc__gentuple_1033,& Cyc__gentuple_1034};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_1036,sizeof(struct Cyc_Absyn_PtrAtts),{(void*)((
struct _tuple5**)Cyc__genarr_1035),(void*)((struct _tuple5**)Cyc__genarr_1035 + 5)}};
static char _tmp240[8]="PtrInfo";static struct _dynforward_ptr Cyc__genname_1041=(
struct _dynforward_ptr){_tmp240,_tmp240 + 8};static char _tmp241[8]="elt_typ";static
struct _tuple5 Cyc__gentuple_1037={offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),(
struct _dynforward_ptr){_tmp241,_tmp241 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp242[7]="elt_tq";static struct _tuple5 Cyc__gentuple_1038={offsetof(
struct Cyc_Absyn_PtrInfo,elt_tq),(struct _dynforward_ptr){_tmp242,_tmp242 + 7},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp243[9]="ptr_atts";static struct
_tuple5 Cyc__gentuple_1039={offsetof(struct Cyc_Absyn_PtrInfo,ptr_atts),(struct
_dynforward_ptr){_tmp243,_tmp243 + 9},(void*)& Cyc_Absyn_ptr_atts_t_rep};static
struct _tuple5*Cyc__genarr_1040[3]={& Cyc__gentuple_1037,& Cyc__gentuple_1038,& Cyc__gentuple_1039};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_1041,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1040),(void*)((struct _tuple5**)Cyc__genarr_1040 + 3)}};
struct _tuple26{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1042={
offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1043={
offsetof(struct _tuple26,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1044[2]={& Cyc__gentuple_1042,& Cyc__gentuple_1043};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1019={4,sizeof(struct _tuple26),{(void*)((
struct _tuple6**)Cyc__genarr_1044),(void*)((struct _tuple6**)Cyc__genarr_1044 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1007;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep;static char _tmp246[7]="Signed";static struct _tuple7 Cyc__gentuple_284={
0,(struct _dynforward_ptr){_tmp246,_tmp246 + 7}};static char _tmp247[9]="Unsigned";
static struct _tuple7 Cyc__gentuple_285={1,(struct _dynforward_ptr){_tmp247,_tmp247 + 
9}};static char _tmp248[5]="None";static struct _tuple7 Cyc__gentuple_286={2,(struct
_dynforward_ptr){_tmp248,_tmp248 + 5}};static struct _tuple7*Cyc__genarr_287[3]={&
Cyc__gentuple_284,& Cyc__gentuple_285,& Cyc__gentuple_286};static struct _tuple5*Cyc__genarr_288[
0]={};static char _tmp24A[5]="Sign";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep={
5,(struct _dynforward_ptr){_tmp24A,_tmp24A + 5},{(void*)((struct _tuple7**)Cyc__genarr_287),(
void*)((struct _tuple7**)Cyc__genarr_287 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_288),(
void*)((struct _tuple5**)Cyc__genarr_288 + 0)}};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_size_of_t_rep;static char _tmp24B[8]="Char_sz";static struct _tuple7 Cyc__gentuple_1008={
0,(struct _dynforward_ptr){_tmp24B,_tmp24B + 8}};static char _tmp24C[9]="Short_sz";
static struct _tuple7 Cyc__gentuple_1009={1,(struct _dynforward_ptr){_tmp24C,_tmp24C
+ 9}};static char _tmp24D[7]="Int_sz";static struct _tuple7 Cyc__gentuple_1010={2,(
struct _dynforward_ptr){_tmp24D,_tmp24D + 7}};static char _tmp24E[8]="Long_sz";
static struct _tuple7 Cyc__gentuple_1011={3,(struct _dynforward_ptr){_tmp24E,_tmp24E
+ 8}};static char _tmp24F[12]="LongLong_sz";static struct _tuple7 Cyc__gentuple_1012={
4,(struct _dynforward_ptr){_tmp24F,_tmp24F + 12}};static struct _tuple7*Cyc__genarr_1013[
5]={& Cyc__gentuple_1008,& Cyc__gentuple_1009,& Cyc__gentuple_1010,& Cyc__gentuple_1011,&
Cyc__gentuple_1012};static struct _tuple5*Cyc__genarr_1014[0]={};static char _tmp251[
8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={5,(struct
_dynforward_ptr){_tmp251,_tmp251 + 8},{(void*)((struct _tuple7**)Cyc__genarr_1013),(
void*)((struct _tuple7**)Cyc__genarr_1013 + 5)},{(void*)((struct _tuple5**)Cyc__genarr_1014),(
void*)((struct _tuple5**)Cyc__genarr_1014 + 0)}};struct _tuple27{unsigned int f1;
void*f2;void*f3;};static struct _tuple6 Cyc__gentuple_1015={offsetof(struct _tuple27,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1016={offsetof(struct
_tuple27,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_1017={
offsetof(struct _tuple27,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_1018[3]={& Cyc__gentuple_1015,& Cyc__gentuple_1016,& Cyc__gentuple_1017};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1007={4,sizeof(struct _tuple27),{(
void*)((struct _tuple6**)Cyc__genarr_1018),(void*)((struct _tuple6**)Cyc__genarr_1018
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_983;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_array_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_141;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_859;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_874;struct _tuple28{unsigned int f1;void*f2;char f3;};static struct
_tuple6 Cyc__gentuple_875={offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_876={offsetof(struct _tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_877={offsetof(struct _tuple28,f3),(void*)((void*)&
Cyc__genrep_15)};static struct _tuple6*Cyc__genarr_878[3]={& Cyc__gentuple_875,& Cyc__gentuple_876,&
Cyc__gentuple_877};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_874={4,
sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_878),(void*)((
struct _tuple6**)Cyc__genarr_878 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_868;
static struct Cyc_Typerep_Int_struct Cyc__genrep_869={0,1,16};struct _tuple29{
unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_870={offsetof(
struct _tuple29,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_871={
offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_872={
offsetof(struct _tuple29,f3),(void*)& Cyc__genrep_869};static struct _tuple6*Cyc__genarr_873[
3]={& Cyc__gentuple_870,& Cyc__gentuple_871,& Cyc__gentuple_872};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_868={4,sizeof(struct _tuple29),{(void*)((struct _tuple6**)Cyc__genarr_873),(
void*)((struct _tuple6**)Cyc__genarr_873 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_283;struct _tuple30{unsigned int f1;void*f2;int f3;};static struct
_tuple6 Cyc__gentuple_289={offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_290={offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_291={offsetof(struct _tuple30,f3),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_292[3]={& Cyc__gentuple_289,& Cyc__gentuple_290,&
Cyc__gentuple_291};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_283={4,
sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_292),(void*)((
struct _tuple6**)Cyc__genarr_292 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_862;
static struct Cyc_Typerep_Int_struct Cyc__genrep_863={0,1,64};struct _tuple31{
unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_864={
offsetof(struct _tuple31,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_865={
offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_866={
offsetof(struct _tuple31,f3),(void*)& Cyc__genrep_863};static struct _tuple6*Cyc__genarr_867[
3]={& Cyc__gentuple_864,& Cyc__gentuple_865,& Cyc__gentuple_866};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_862={4,sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_867),(
void*)((struct _tuple6**)Cyc__genarr_867 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_81;static struct _tuple6 Cyc__gentuple_82={offsetof(struct _tuple7,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_83={offsetof(struct
_tuple7,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*Cyc__genarr_84[
2]={& Cyc__gentuple_82,& Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_81={
4,sizeof(struct _tuple7),{(void*)((struct _tuple6**)Cyc__genarr_84),(void*)((
struct _tuple6**)Cyc__genarr_84 + 2)}};static char _tmp25A[7]="Null_c";static struct
_tuple7 Cyc__gentuple_860={0,(struct _dynforward_ptr){_tmp25A,_tmp25A + 7}};static
struct _tuple7*Cyc__genarr_861[1]={& Cyc__gentuple_860};static char _tmp25B[7]="Char_c";
static struct _tuple5 Cyc__gentuple_879={0,(struct _dynforward_ptr){_tmp25B,_tmp25B + 
7},(void*)& Cyc__genrep_874};static char _tmp25C[8]="Short_c";static struct _tuple5
Cyc__gentuple_880={1,(struct _dynforward_ptr){_tmp25C,_tmp25C + 8},(void*)& Cyc__genrep_868};
static char _tmp25D[6]="Int_c";static struct _tuple5 Cyc__gentuple_881={2,(struct
_dynforward_ptr){_tmp25D,_tmp25D + 6},(void*)& Cyc__genrep_283};static char _tmp25E[
11]="LongLong_c";static struct _tuple5 Cyc__gentuple_882={3,(struct _dynforward_ptr){
_tmp25E,_tmp25E + 11},(void*)& Cyc__genrep_862};static char _tmp25F[8]="Float_c";
static struct _tuple5 Cyc__gentuple_883={4,(struct _dynforward_ptr){_tmp25F,_tmp25F + 
8},(void*)& Cyc__genrep_81};static char _tmp260[9]="String_c";static struct _tuple5
Cyc__gentuple_884={5,(struct _dynforward_ptr){_tmp260,_tmp260 + 9},(void*)& Cyc__genrep_81};
static struct _tuple5*Cyc__genarr_885[6]={& Cyc__gentuple_879,& Cyc__gentuple_880,&
Cyc__gentuple_881,& Cyc__gentuple_882,& Cyc__gentuple_883,& Cyc__gentuple_884};
static char _tmp262[5]="Cnst";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={
5,(struct _dynforward_ptr){_tmp262,_tmp262 + 5},{(void*)((struct _tuple7**)Cyc__genarr_861),(
void*)((struct _tuple7**)Cyc__genarr_861 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_885),(
void*)((struct _tuple5**)Cyc__genarr_885 + 6)}};static struct _tuple6 Cyc__gentuple_886={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_887={
offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_cnst_t_rep};static struct _tuple6*Cyc__genarr_888[
2]={& Cyc__gentuple_886,& Cyc__gentuple_887};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_859={4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_888),(
void*)((struct _tuple6**)Cyc__genarr_888 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_807;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_814;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_815;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_818;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_819;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_820;struct _tuple32{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct _tuple6 Cyc__gentuple_821={offsetof(struct _tuple32,f1),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_822={offsetof(struct
_tuple32,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_823={
offsetof(struct _tuple32,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_824[3]={& Cyc__gentuple_821,& Cyc__gentuple_822,& Cyc__gentuple_823};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_820={4,sizeof(struct _tuple32),{(
void*)((struct _tuple6**)Cyc__genarr_824),(void*)((struct _tuple6**)Cyc__genarr_824
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_819={1,1,(void*)((void*)&
Cyc__genrep_820)};static char _tmp266[5]="List";static struct _dynforward_ptr Cyc__genname_828=(
struct _dynforward_ptr){_tmp266,_tmp266 + 5};static char _tmp267[3]="hd";static
struct _tuple5 Cyc__gentuple_825={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp267,_tmp267 + 3},(void*)& Cyc__genrep_819};static char _tmp268[
3]="tl";static struct _tuple5 Cyc__gentuple_826={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp268,_tmp268 + 3},(void*)& Cyc__genrep_818};static struct
_tuple5*Cyc__genarr_827[2]={& Cyc__gentuple_825,& Cyc__gentuple_826};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_828,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_827),(void*)((struct _tuple5**)Cyc__genarr_827 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_818={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_817;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_662;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp26B[4]="Opt";static struct _dynforward_ptr Cyc__genname_665=(struct
_dynforward_ptr){_tmp26B,_tmp26B + 4};static char _tmp26C[2]="v";static struct
_tuple5 Cyc__gentuple_663={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp26C,_tmp26C + 2},(void*)& Cyc__genrep_13};static struct _tuple5*Cyc__genarr_664[
1]={& Cyc__gentuple_663};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_665,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_664),(void*)((struct _tuple5**)Cyc__genarr_664 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_662={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp26F[11]="VarargInfo";static struct _dynforward_ptr Cyc__genname_737=(
struct _dynforward_ptr){_tmp26F,_tmp26F + 11};static char _tmp270[5]="name";static
struct _tuple5 Cyc__gentuple_732={offsetof(struct Cyc_Absyn_VarargInfo,name),(
struct _dynforward_ptr){_tmp270,_tmp270 + 5},(void*)& Cyc__genrep_662};static char
_tmp271[3]="tq";static struct _tuple5 Cyc__gentuple_733={offsetof(struct Cyc_Absyn_VarargInfo,tq),(
struct _dynforward_ptr){_tmp271,_tmp271 + 3},(void*)& Cyc_Absyn_tqual_t_rep};static
char _tmp272[5]="type";static struct _tuple5 Cyc__gentuple_734={offsetof(struct Cyc_Absyn_VarargInfo,type),(
struct _dynforward_ptr){_tmp272,_tmp272 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp273[7]="inject";static struct _tuple5 Cyc__gentuple_735={offsetof(
struct Cyc_Absyn_VarargInfo,inject),(struct _dynforward_ptr){_tmp273,_tmp273 + 7},(
void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_736[4]={& Cyc__gentuple_732,&
Cyc__gentuple_733,& Cyc__gentuple_734,& Cyc__gentuple_735};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_737,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_736),(void*)((
struct _tuple5**)Cyc__genarr_736 + 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_817={
1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_382;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_383;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_384;static struct _tuple6 Cyc__gentuple_385={offsetof(struct _tuple9,f1),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_386={
offsetof(struct _tuple9,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_387[2]={& Cyc__gentuple_385,& Cyc__gentuple_386};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_384={4,sizeof(struct _tuple9),{(void*)((
struct _tuple6**)Cyc__genarr_387),(void*)((struct _tuple6**)Cyc__genarr_387 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_383={1,1,(void*)((void*)& Cyc__genrep_384)};
static char _tmp278[5]="List";static struct _dynforward_ptr Cyc__genname_391=(struct
_dynforward_ptr){_tmp278,_tmp278 + 5};static char _tmp279[3]="hd";static struct
_tuple5 Cyc__gentuple_388={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp279,_tmp279 + 3},(void*)& Cyc__genrep_383};static char _tmp27A[
3]="tl";static struct _tuple5 Cyc__gentuple_389={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp27A,_tmp27A + 3},(void*)& Cyc__genrep_382};static struct
_tuple5*Cyc__genarr_390[2]={& Cyc__gentuple_388,& Cyc__gentuple_389};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_391,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_390),(void*)((struct _tuple5**)Cyc__genarr_390 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_382={1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_153;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_154;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_154={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple33{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_155={offsetof(struct _tuple33,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_156={offsetof(struct _tuple33,f2),(
void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_157[2]={& Cyc__gentuple_155,&
Cyc__gentuple_156};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_153={4,
sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_157),(void*)((
struct _tuple6**)Cyc__genarr_157 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_552;
struct _tuple34{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
static struct _tuple6 Cyc__gentuple_553={offsetof(struct _tuple34,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_554={offsetof(struct _tuple34,f2),(void*)& Cyc__genrep_145};
static struct _tuple6 Cyc__gentuple_555={offsetof(struct _tuple34,f3),(void*)& Cyc__genrep_145};
static struct _tuple6*Cyc__genarr_556[3]={& Cyc__gentuple_553,& Cyc__gentuple_554,&
Cyc__gentuple_555};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_552={4,
sizeof(struct _tuple34),{(void*)((struct _tuple6**)Cyc__genarr_556),(void*)((
struct _tuple6**)Cyc__genarr_556 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_548;
static struct _tuple6 Cyc__gentuple_549={offsetof(struct _tuple33,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_550={offsetof(struct _tuple33,f2),(void*)& Cyc__genrep_141};
static struct _tuple6*Cyc__genarr_551[2]={& Cyc__gentuple_549,& Cyc__gentuple_550};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_548={4,sizeof(struct _tuple33),{(
void*)((struct _tuple6**)Cyc__genarr_551),(void*)((struct _tuple6**)Cyc__genarr_551
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_542;struct _tuple35{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*f3;struct Cyc_Absyn_Stmt*
f4;};static struct _tuple6 Cyc__gentuple_543={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_544={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_545={offsetof(struct
_tuple35,f3),(void*)& Cyc__genrep_145};static struct _tuple6 Cyc__gentuple_546={
offsetof(struct _tuple35,f4),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_547[
4]={& Cyc__gentuple_543,& Cyc__gentuple_544,& Cyc__gentuple_545,& Cyc__gentuple_546};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_542={4,sizeof(struct _tuple35),{(
void*)((struct _tuple6**)Cyc__genarr_547),(void*)((struct _tuple6**)Cyc__genarr_547
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_537;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_486;static struct _tuple6 Cyc__gentuple_487={offsetof(struct _tuple2,f1),(
void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_488={offsetof(struct
_tuple2,f2),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_489[2]={&
Cyc__gentuple_487,& Cyc__gentuple_488};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_486={
4,sizeof(struct _tuple2),{(void*)((struct _tuple6**)Cyc__genarr_489),(void*)((
struct _tuple6**)Cyc__genarr_489 + 2)}};struct _tuple36{unsigned int f1;struct
_tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_538={
offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_539={
offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_486};static struct _tuple6 Cyc__gentuple_540={
offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_541[
3]={& Cyc__gentuple_538,& Cyc__gentuple_539,& Cyc__gentuple_540};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_537={4,sizeof(struct _tuple36),{(void*)((struct _tuple6**)Cyc__genarr_541),(
void*)((struct _tuple6**)Cyc__genarr_541 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_533;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_528;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_528={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
struct _tuple37{unsigned int f1;struct Cyc_Absyn_Stmt*f2;};static struct _tuple6 Cyc__gentuple_534={
offsetof(struct _tuple37,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_535={
offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_528};static struct _tuple6*Cyc__genarr_536[
2]={& Cyc__gentuple_534,& Cyc__gentuple_535};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_533={4,sizeof(struct _tuple37),{(void*)((struct _tuple6**)Cyc__genarr_536),(
void*)((struct _tuple6**)Cyc__genarr_536 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_527;struct _tuple38{unsigned int f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Stmt*
f3;};static struct _tuple6 Cyc__gentuple_529={offsetof(struct _tuple38,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_530={offsetof(struct _tuple38,f2),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_531={offsetof(struct
_tuple38,f3),(void*)& Cyc__genrep_528};static struct _tuple6*Cyc__genarr_532[3]={&
Cyc__gentuple_529,& Cyc__gentuple_530,& Cyc__gentuple_531};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_527={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_532),(
void*)((struct _tuple6**)Cyc__genarr_532 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_520;struct _tuple39{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct
_tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*f5;};static struct _tuple6 Cyc__gentuple_521={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_522={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_523={
offsetof(struct _tuple39,f3),(void*)& Cyc__genrep_486};static struct _tuple6 Cyc__gentuple_524={
offsetof(struct _tuple39,f4),(void*)& Cyc__genrep_486};static struct _tuple6 Cyc__gentuple_525={
offsetof(struct _tuple39,f5),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_526[
5]={& Cyc__gentuple_521,& Cyc__gentuple_522,& Cyc__gentuple_523,& Cyc__gentuple_524,&
Cyc__gentuple_525};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_520={4,
sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_526),(void*)((
struct _tuple6**)Cyc__genarr_526 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_515;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_229;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_230;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_240;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_pat_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_444;struct _tuple40{unsigned int
f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Pat*f3;};static struct _tuple6 Cyc__gentuple_445={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_446={
offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_447={
offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_240};static struct _tuple6*Cyc__genarr_448[
3]={& Cyc__gentuple_445,& Cyc__gentuple_446,& Cyc__gentuple_447};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_444={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_448),(
void*)((struct _tuple6**)Cyc__genarr_448 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_439;struct _tuple41{unsigned int f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*
f3;};static struct _tuple6 Cyc__gentuple_440={offsetof(struct _tuple41,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_441={offsetof(struct _tuple41,f2),(
void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_442={offsetof(struct
_tuple41,f3),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_443[3]={&
Cyc__gentuple_440,& Cyc__gentuple_441,& Cyc__gentuple_442};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_439={4,sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_443),(
void*)((struct _tuple6**)Cyc__genarr_443 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_434;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_245;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;static
char _tmp28A[5]="List";static struct _dynforward_ptr Cyc__genname_249=(struct
_dynforward_ptr){_tmp28A,_tmp28A + 5};static char _tmp28B[3]="hd";static struct
_tuple5 Cyc__gentuple_246={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp28B,_tmp28B + 3},(void*)& Cyc__genrep_240};static char _tmp28C[
3]="tl";static struct _tuple5 Cyc__gentuple_247={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp28C,_tmp28C + 3},(void*)& Cyc__genrep_245};static struct
_tuple5*Cyc__genarr_248[2]={& Cyc__gentuple_246,& Cyc__gentuple_247};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_249,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_248),(void*)((
struct _tuple5**)Cyc__genarr_248 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_245={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};struct _tuple42{
unsigned int f1;struct Cyc_List_List*f2;int f3;};static struct _tuple6 Cyc__gentuple_435={
offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_436={
offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_245};static struct _tuple6 Cyc__gentuple_437={
offsetof(struct _tuple42,f3),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*
Cyc__genarr_438[3]={& Cyc__gentuple_435,& Cyc__gentuple_436,& Cyc__gentuple_437};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_434={4,sizeof(struct _tuple42),{(
void*)((struct _tuple6**)Cyc__genarr_438),(void*)((struct _tuple6**)Cyc__genarr_438
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_430;struct _tuple43{
unsigned int f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_431={
offsetof(struct _tuple43,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_432={
offsetof(struct _tuple43,f2),(void*)& Cyc__genrep_240};static struct _tuple6*Cyc__genarr_433[
2]={& Cyc__gentuple_431,& Cyc__gentuple_432};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_430={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_433),(
void*)((struct _tuple6**)Cyc__genarr_433 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_340;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_412;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep;
static char _tmp291[8]="StructA";static struct _tuple7 Cyc__gentuple_397={0,(struct
_dynforward_ptr){_tmp291,_tmp291 + 8}};static char _tmp292[7]="UnionA";static struct
_tuple7 Cyc__gentuple_398={1,(struct _dynforward_ptr){_tmp292,_tmp292 + 7}};static
struct _tuple7*Cyc__genarr_399[2]={& Cyc__gentuple_397,& Cyc__gentuple_398};static
struct _tuple5*Cyc__genarr_400[0]={};static char _tmp294[9]="AggrKind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_aggr_kind_t_rep={5,(struct _dynforward_ptr){_tmp294,_tmp294 + 9},{(void*)((
struct _tuple7**)Cyc__genarr_399),(void*)((struct _tuple7**)Cyc__genarr_399 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_400),(void*)((struct _tuple5**)Cyc__genarr_400
+ 0)}};struct _tuple44{unsigned int f1;void*f2;struct _tuple0*f3;};static struct
_tuple6 Cyc__gentuple_413={offsetof(struct _tuple44,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_414={offsetof(struct _tuple44,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_415={offsetof(struct _tuple44,f3),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_416[3]={& Cyc__gentuple_413,& Cyc__gentuple_414,&
Cyc__gentuple_415};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_412={4,
sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_416),(void*)((
struct _tuple6**)Cyc__genarr_416 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_365;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_366;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_367;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_368;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_AggrdeclImpl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_369;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_370;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Aggrfield_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_89;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;
static char _tmp296[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_90={0,(struct
_dynforward_ptr){_tmp296,_tmp296 + 10}};static char _tmp297[9]="Scanf_ft";static
struct _tuple7 Cyc__gentuple_91={1,(struct _dynforward_ptr){_tmp297,_tmp297 + 9}};
static struct _tuple7*Cyc__genarr_92[2]={& Cyc__gentuple_90,& Cyc__gentuple_91};
static struct _tuple5*Cyc__genarr_93[0]={};static char _tmp299[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={5,(struct
_dynforward_ptr){_tmp299,_tmp299 + 12},{(void*)((struct _tuple7**)Cyc__genarr_92),(
void*)((struct _tuple7**)Cyc__genarr_92 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_93),(
void*)((struct _tuple5**)Cyc__genarr_93 + 0)}};struct _tuple45{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_94={offsetof(struct _tuple45,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_95={offsetof(struct
_tuple45,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_96={
offsetof(struct _tuple45,f3),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_97={offsetof(struct _tuple45,f4),(void*)((void*)& Cyc__genrep_10)};
static struct _tuple6*Cyc__genarr_98[4]={& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,&
Cyc__gentuple_97};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_89={4,sizeof(
struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_98),(void*)((struct _tuple6**)
Cyc__genarr_98 + 4)}};static char _tmp29B[12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_63={
0,(struct _dynforward_ptr){_tmp29B,_tmp29B + 12}};static char _tmp29C[10]="Cdecl_att";
static struct _tuple7 Cyc__gentuple_64={1,(struct _dynforward_ptr){_tmp29C,_tmp29C + 
10}};static char _tmp29D[13]="Fastcall_att";static struct _tuple7 Cyc__gentuple_65={2,(
struct _dynforward_ptr){_tmp29D,_tmp29D + 13}};static char _tmp29E[13]="Noreturn_att";
static struct _tuple7 Cyc__gentuple_66={3,(struct _dynforward_ptr){_tmp29E,_tmp29E + 
13}};static char _tmp29F[10]="Const_att";static struct _tuple7 Cyc__gentuple_67={4,(
struct _dynforward_ptr){_tmp29F,_tmp29F + 10}};static char _tmp2A0[11]="Packed_att";
static struct _tuple7 Cyc__gentuple_68={5,(struct _dynforward_ptr){_tmp2A0,_tmp2A0 + 
11}};static char _tmp2A1[13]="Nocommon_att";static struct _tuple7 Cyc__gentuple_69={6,(
struct _dynforward_ptr){_tmp2A1,_tmp2A1 + 13}};static char _tmp2A2[11]="Shared_att";
static struct _tuple7 Cyc__gentuple_70={7,(struct _dynforward_ptr){_tmp2A2,_tmp2A2 + 
11}};static char _tmp2A3[11]="Unused_att";static struct _tuple7 Cyc__gentuple_71={8,(
struct _dynforward_ptr){_tmp2A3,_tmp2A3 + 11}};static char _tmp2A4[9]="Weak_att";
static struct _tuple7 Cyc__gentuple_72={9,(struct _dynforward_ptr){_tmp2A4,_tmp2A4 + 
9}};static char _tmp2A5[14]="Dllimport_att";static struct _tuple7 Cyc__gentuple_73={
10,(struct _dynforward_ptr){_tmp2A5,_tmp2A5 + 14}};static char _tmp2A6[14]="Dllexport_att";
static struct _tuple7 Cyc__gentuple_74={11,(struct _dynforward_ptr){_tmp2A6,_tmp2A6 + 
14}};static char _tmp2A7[27]="No_instrument_function_att";static struct _tuple7 Cyc__gentuple_75={
12,(struct _dynforward_ptr){_tmp2A7,_tmp2A7 + 27}};static char _tmp2A8[16]="Constructor_att";
static struct _tuple7 Cyc__gentuple_76={13,(struct _dynforward_ptr){_tmp2A8,_tmp2A8 + 
16}};static char _tmp2A9[15]="Destructor_att";static struct _tuple7 Cyc__gentuple_77={
14,(struct _dynforward_ptr){_tmp2A9,_tmp2A9 + 15}};static char _tmp2AA[26]="No_check_memory_usage_att";
static struct _tuple7 Cyc__gentuple_78={15,(struct _dynforward_ptr){_tmp2AA,_tmp2AA + 
26}};static char _tmp2AB[9]="Pure_att";static struct _tuple7 Cyc__gentuple_79={16,(
struct _dynforward_ptr){_tmp2AB,_tmp2AB + 9}};static struct _tuple7*Cyc__genarr_80[
17]={& Cyc__gentuple_63,& Cyc__gentuple_64,& Cyc__gentuple_65,& Cyc__gentuple_66,&
Cyc__gentuple_67,& Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,&
Cyc__gentuple_72,& Cyc__gentuple_73,& Cyc__gentuple_74,& Cyc__gentuple_75,& Cyc__gentuple_76,&
Cyc__gentuple_77,& Cyc__gentuple_78,& Cyc__gentuple_79};static char _tmp2AC[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_99={0,(struct _dynforward_ptr){_tmp2AC,_tmp2AC + 
12},(void*)& Cyc__genrep_85};static char _tmp2AD[12]="Aligned_att";static struct
_tuple5 Cyc__gentuple_100={1,(struct _dynforward_ptr){_tmp2AD,_tmp2AD + 12},(void*)&
Cyc__genrep_85};static char _tmp2AE[12]="Section_att";static struct _tuple5 Cyc__gentuple_101={
2,(struct _dynforward_ptr){_tmp2AE,_tmp2AE + 12},(void*)& Cyc__genrep_81};static
char _tmp2AF[11]="Format_att";static struct _tuple5 Cyc__gentuple_102={3,(struct
_dynforward_ptr){_tmp2AF,_tmp2AF + 11},(void*)& Cyc__genrep_89};static char _tmp2B0[
16]="Initializes_att";static struct _tuple5 Cyc__gentuple_103={4,(struct
_dynforward_ptr){_tmp2B0,_tmp2B0 + 16},(void*)& Cyc__genrep_85};static char _tmp2B1[
9]="Mode_att";static struct _tuple5 Cyc__gentuple_104={5,(struct _dynforward_ptr){
_tmp2B1,_tmp2B1 + 9},(void*)& Cyc__genrep_81};static struct _tuple5*Cyc__genarr_105[
6]={& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,& Cyc__gentuple_102,&
Cyc__gentuple_103,& Cyc__gentuple_104};static char _tmp2B3[10]="Attribute";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,(struct _dynforward_ptr){
_tmp2B3,_tmp2B3 + 10},{(void*)((struct _tuple7**)Cyc__genarr_80),(void*)((struct
_tuple7**)Cyc__genarr_80 + 17)},{(void*)((struct _tuple5**)Cyc__genarr_105),(void*)((
struct _tuple5**)Cyc__genarr_105 + 6)}};static char _tmp2B4[5]="List";static struct
_dynforward_ptr Cyc__genname_109=(struct _dynforward_ptr){_tmp2B4,_tmp2B4 + 5};
static char _tmp2B5[3]="hd";static struct _tuple5 Cyc__gentuple_106={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2B5,_tmp2B5 + 3},(void*)& Cyc_Absyn_attribute_t_rep};
static char _tmp2B6[3]="tl";static struct _tuple5 Cyc__gentuple_107={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2B6,_tmp2B6 + 3},(void*)& Cyc__genrep_62};
static struct _tuple5*Cyc__genarr_108[2]={& Cyc__gentuple_106,& Cyc__gentuple_107};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_109,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_108),(void*)((struct _tuple5**)Cyc__genarr_108 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};
static char _tmp2B9[10]="Aggrfield";static struct _dynforward_ptr Cyc__genname_377=(
struct _dynforward_ptr){_tmp2B9,_tmp2B9 + 10};static char _tmp2BA[5]="name";static
struct _tuple5 Cyc__gentuple_371={offsetof(struct Cyc_Absyn_Aggrfield,name),(struct
_dynforward_ptr){_tmp2BA,_tmp2BA + 5},(void*)& Cyc__genrep_13};static char _tmp2BB[3]="tq";
static struct _tuple5 Cyc__gentuple_372={offsetof(struct Cyc_Absyn_Aggrfield,tq),(
struct _dynforward_ptr){_tmp2BB,_tmp2BB + 3},(void*)& Cyc_Absyn_tqual_t_rep};static
char _tmp2BC[5]="type";static struct _tuple5 Cyc__gentuple_373={offsetof(struct Cyc_Absyn_Aggrfield,type),(
struct _dynforward_ptr){_tmp2BC,_tmp2BC + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp2BD[6]="width";static struct _tuple5 Cyc__gentuple_374={offsetof(
struct Cyc_Absyn_Aggrfield,width),(struct _dynforward_ptr){_tmp2BD,_tmp2BD + 6},(
void*)& Cyc__genrep_141};static char _tmp2BE[11]="attributes";static struct _tuple5
Cyc__gentuple_375={offsetof(struct Cyc_Absyn_Aggrfield,attributes),(struct
_dynforward_ptr){_tmp2BE,_tmp2BE + 11},(void*)& Cyc__genrep_62};static struct
_tuple5*Cyc__genarr_376[5]={& Cyc__gentuple_371,& Cyc__gentuple_372,& Cyc__gentuple_373,&
Cyc__gentuple_374,& Cyc__gentuple_375};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_377,sizeof(struct Cyc_Absyn_Aggrfield),{(
void*)((struct _tuple5**)Cyc__genarr_376),(void*)((struct _tuple5**)Cyc__genarr_376
+ 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_370={1,1,(void*)((void*)&
Cyc_struct_Absyn_Aggrfield_rep)};static char _tmp2C1[5]="List";static struct
_dynforward_ptr Cyc__genname_381=(struct _dynforward_ptr){_tmp2C1,_tmp2C1 + 5};
static char _tmp2C2[3]="hd";static struct _tuple5 Cyc__gentuple_378={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2C2,_tmp2C2 + 3},(void*)& Cyc__genrep_370};
static char _tmp2C3[3]="tl";static struct _tuple5 Cyc__gentuple_379={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2C3,_tmp2C3 + 3},(void*)& Cyc__genrep_369};
static struct _tuple5*Cyc__genarr_380[2]={& Cyc__gentuple_378,& Cyc__gentuple_379};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_381,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_380),(void*)((struct _tuple5**)Cyc__genarr_380 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_369={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};
static char _tmp2C6[13]="AggrdeclImpl";static struct _dynforward_ptr Cyc__genname_396=(
struct _dynforward_ptr){_tmp2C6,_tmp2C6 + 13};static char _tmp2C7[11]="exist_vars";
static struct _tuple5 Cyc__gentuple_392={offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),(
struct _dynforward_ptr){_tmp2C7,_tmp2C7 + 11},(void*)& Cyc__genrep_321};static char
_tmp2C8[7]="rgn_po";static struct _tuple5 Cyc__gentuple_393={offsetof(struct Cyc_Absyn_AggrdeclImpl,rgn_po),(
struct _dynforward_ptr){_tmp2C8,_tmp2C8 + 7},(void*)& Cyc__genrep_382};static char
_tmp2C9[7]="fields";static struct _tuple5 Cyc__gentuple_394={offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),(
struct _dynforward_ptr){_tmp2C9,_tmp2C9 + 7},(void*)& Cyc__genrep_369};static struct
_tuple5*Cyc__genarr_395[3]={& Cyc__gentuple_392,& Cyc__gentuple_393,& Cyc__gentuple_394};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_396,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(void*)((
struct _tuple5**)Cyc__genarr_395),(void*)((struct _tuple5**)Cyc__genarr_395 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_368={1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};
static char _tmp2CC[9]="Aggrdecl";static struct _dynforward_ptr Cyc__genname_408=(
struct _dynforward_ptr){_tmp2CC,_tmp2CC + 9};static char _tmp2CD[5]="kind";static
struct _tuple5 Cyc__gentuple_401={offsetof(struct Cyc_Absyn_Aggrdecl,kind),(struct
_dynforward_ptr){_tmp2CD,_tmp2CD + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};static
char _tmp2CE[3]="sc";static struct _tuple5 Cyc__gentuple_402={offsetof(struct Cyc_Absyn_Aggrdecl,sc),(
struct _dynforward_ptr){_tmp2CE,_tmp2CE + 3},(void*)& Cyc_Absyn_scope_t_rep};static
char _tmp2CF[5]="name";static struct _tuple5 Cyc__gentuple_403={offsetof(struct Cyc_Absyn_Aggrdecl,name),(
struct _dynforward_ptr){_tmp2CF,_tmp2CF + 5},(void*)& Cyc__genrep_11};static char
_tmp2D0[4]="tvs";static struct _tuple5 Cyc__gentuple_404={offsetof(struct Cyc_Absyn_Aggrdecl,tvs),(
struct _dynforward_ptr){_tmp2D0,_tmp2D0 + 4},(void*)& Cyc__genrep_321};static char
_tmp2D1[5]="impl";static struct _tuple5 Cyc__gentuple_405={offsetof(struct Cyc_Absyn_Aggrdecl,impl),(
struct _dynforward_ptr){_tmp2D1,_tmp2D1 + 5},(void*)& Cyc__genrep_368};static char
_tmp2D2[11]="attributes";static struct _tuple5 Cyc__gentuple_406={offsetof(struct
Cyc_Absyn_Aggrdecl,attributes),(struct _dynforward_ptr){_tmp2D2,_tmp2D2 + 11},(
void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_407[6]={& Cyc__gentuple_401,&
Cyc__gentuple_402,& Cyc__gentuple_403,& Cyc__gentuple_404,& Cyc__gentuple_405,& Cyc__gentuple_406};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_408,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((
struct _tuple5**)Cyc__genarr_407),(void*)((struct _tuple5**)Cyc__genarr_407 + 6)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_367={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_366={1,1,(void*)((void*)& Cyc__genrep_367)};
struct _tuple46{unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6
Cyc__gentuple_409={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_410={offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_366};
static struct _tuple6*Cyc__genarr_411[2]={& Cyc__gentuple_409,& Cyc__gentuple_410};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_365={4,sizeof(struct _tuple46),{(
void*)((struct _tuple6**)Cyc__genarr_411),(void*)((struct _tuple6**)Cyc__genarr_411
+ 2)}};static struct _tuple7*Cyc__genarr_364[0]={};static char _tmp2D7[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_417={0,(struct _dynforward_ptr){_tmp2D7,_tmp2D7 + 
12},(void*)& Cyc__genrep_412};static char _tmp2D8[10]="KnownAggr";static struct
_tuple5 Cyc__gentuple_418={1,(struct _dynforward_ptr){_tmp2D8,_tmp2D8 + 10},(void*)&
Cyc__genrep_365};static struct _tuple5*Cyc__genarr_419[2]={& Cyc__gentuple_417,& Cyc__gentuple_418};
static char _tmp2DA[10]="AggrInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep={
5,(struct _dynforward_ptr){_tmp2DA,_tmp2DA + 10},{(void*)((struct _tuple7**)Cyc__genarr_364),(
void*)((struct _tuple7**)Cyc__genarr_364 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_419),(
void*)((struct _tuple5**)Cyc__genarr_419 + 2)}};static char _tmp2DB[9]="AggrInfo";
static struct _dynforward_ptr Cyc__genname_423=(struct _dynforward_ptr){_tmp2DB,
_tmp2DB + 9};static char _tmp2DC[10]="aggr_info";static struct _tuple5 Cyc__gentuple_420={
offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),(struct _dynforward_ptr){_tmp2DC,
_tmp2DC + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};static char _tmp2DD[6]="targs";
static struct _tuple5 Cyc__gentuple_421={offsetof(struct Cyc_Absyn_AggrInfo,targs),(
struct _dynforward_ptr){_tmp2DD,_tmp2DD + 6},(void*)& Cyc__genrep_121};static struct
_tuple5*Cyc__genarr_422[2]={& Cyc__gentuple_420,& Cyc__gentuple_421};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_aggr_info_t_rep={3,(struct _dynforward_ptr*)& Cyc__genname_423,sizeof(
struct Cyc_Absyn_AggrInfo),{(void*)((struct _tuple5**)Cyc__genarr_422),(void*)((
struct _tuple5**)Cyc__genarr_422 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_342;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_343;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_344;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_346;struct _tuple47{unsigned int f1;struct _dynforward_ptr*f2;};static
struct _tuple6 Cyc__gentuple_347={offsetof(struct _tuple47,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_348={offsetof(struct _tuple47,f2),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_349[2]={& Cyc__gentuple_347,& Cyc__gentuple_348};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_346={4,sizeof(struct _tuple47),{(
void*)((struct _tuple6**)Cyc__genarr_349),(void*)((struct _tuple6**)Cyc__genarr_349
+ 2)}};static struct _tuple7*Cyc__genarr_345[0]={};static char _tmp2E0[13]="ArrayElement";
static struct _tuple5 Cyc__gentuple_350={0,(struct _dynforward_ptr){_tmp2E0,_tmp2E0 + 
13},(void*)& Cyc__genrep_153};static char _tmp2E1[10]="FieldName";static struct
_tuple5 Cyc__gentuple_351={1,(struct _dynforward_ptr){_tmp2E1,_tmp2E1 + 10},(void*)&
Cyc__genrep_346};static struct _tuple5*Cyc__genarr_352[2]={& Cyc__gentuple_350,& Cyc__gentuple_351};
static char _tmp2E3[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,(struct _dynforward_ptr){_tmp2E3,_tmp2E3 + 11},{(void*)((struct _tuple7**)Cyc__genarr_345),(
void*)((struct _tuple7**)Cyc__genarr_345 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_352),(
void*)((struct _tuple5**)Cyc__genarr_352 + 2)}};static char _tmp2E4[5]="List";static
struct _dynforward_ptr Cyc__genname_356=(struct _dynforward_ptr){_tmp2E4,_tmp2E4 + 5};
static char _tmp2E5[3]="hd";static struct _tuple5 Cyc__gentuple_353={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp2E5,_tmp2E5 + 3},(void*)& Cyc_Absyn_designator_t_rep};
static char _tmp2E6[3]="tl";static struct _tuple5 Cyc__gentuple_354={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp2E6,_tmp2E6 + 3},(void*)& Cyc__genrep_344};
static struct _tuple5*Cyc__genarr_355[2]={& Cyc__gentuple_353,& Cyc__gentuple_354};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_356,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_355),(void*)((struct _tuple5**)Cyc__genarr_355 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_344={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};
struct _tuple48{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct
_tuple6 Cyc__gentuple_357={offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_344};
static struct _tuple6 Cyc__gentuple_358={offsetof(struct _tuple48,f2),(void*)& Cyc__genrep_240};
static struct _tuple6*Cyc__genarr_359[2]={& Cyc__gentuple_357,& Cyc__gentuple_358};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_343={4,sizeof(struct _tuple48),{(
void*)((struct _tuple6**)Cyc__genarr_359),(void*)((struct _tuple6**)Cyc__genarr_359
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_342={1,1,(void*)((void*)&
Cyc__genrep_343)};static char _tmp2EB[5]="List";static struct _dynforward_ptr Cyc__genname_363=(
struct _dynforward_ptr){_tmp2EB,_tmp2EB + 5};static char _tmp2EC[3]="hd";static
struct _tuple5 Cyc__gentuple_360={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp2EC,_tmp2EC + 3},(void*)& Cyc__genrep_342};static char _tmp2ED[
3]="tl";static struct _tuple5 Cyc__gentuple_361={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp2ED,_tmp2ED + 3},(void*)& Cyc__genrep_341};static struct
_tuple5*Cyc__genarr_362[2]={& Cyc__gentuple_360,& Cyc__gentuple_361};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_363,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_362),(void*)((struct _tuple5**)Cyc__genarr_362 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341={1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple49{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_424={offsetof(
struct _tuple49,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_425={
offsetof(struct _tuple49,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_426={offsetof(struct _tuple49,f3),(void*)& Cyc__genrep_321};
static struct _tuple6 Cyc__gentuple_427={offsetof(struct _tuple49,f4),(void*)& Cyc__genrep_341};
static struct _tuple6 Cyc__gentuple_428={offsetof(struct _tuple49,f5),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_429[5]={& Cyc__gentuple_424,& Cyc__gentuple_425,&
Cyc__gentuple_426,& Cyc__gentuple_427,& Cyc__gentuple_428};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_340={4,sizeof(struct _tuple49),{(void*)((struct _tuple6**)Cyc__genarr_429),(
void*)((struct _tuple6**)Cyc__genarr_429 + 5)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_293;struct _tuple50{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;struct Cyc_List_List*f4;int f5;};static struct
_tuple6 Cyc__gentuple_334={offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_335={offsetof(struct _tuple50,f2),(void*)((void*)&
Cyc__genrep_311)};static struct _tuple6 Cyc__gentuple_336={offsetof(struct _tuple50,f3),(
void*)& Cyc__genrep_294};static struct _tuple6 Cyc__gentuple_337={offsetof(struct
_tuple50,f4),(void*)& Cyc__genrep_245};static struct _tuple6 Cyc__gentuple_338={
offsetof(struct _tuple50,f5),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*
Cyc__genarr_339[5]={& Cyc__gentuple_334,& Cyc__gentuple_335,& Cyc__gentuple_336,&
Cyc__gentuple_337,& Cyc__gentuple_338};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_293={
4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_339),(void*)((
struct _tuple6**)Cyc__genarr_339 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_279;
struct _tuple51{unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_280={
offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_281={
offsetof(struct _tuple51,f2),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_282[2]={& Cyc__gentuple_280,& Cyc__gentuple_281};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_279={4,sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_282),(
void*)((struct _tuple6**)Cyc__genarr_282 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_264;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_266;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_139;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_140;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep;
static char _tmp2F3[10]="Enumfield";static struct _dynforward_ptr Cyc__genname_938=(
struct _dynforward_ptr){_tmp2F3,_tmp2F3 + 10};static char _tmp2F4[5]="name";static
struct _tuple5 Cyc__gentuple_934={offsetof(struct Cyc_Absyn_Enumfield,name),(struct
_dynforward_ptr){_tmp2F4,_tmp2F4 + 5},(void*)& Cyc__genrep_11};static char _tmp2F5[4]="tag";
static struct _tuple5 Cyc__gentuple_935={offsetof(struct Cyc_Absyn_Enumfield,tag),(
struct _dynforward_ptr){_tmp2F5,_tmp2F5 + 4},(void*)& Cyc__genrep_141};static char
_tmp2F6[4]="loc";static struct _tuple5 Cyc__gentuple_936={offsetof(struct Cyc_Absyn_Enumfield,loc),(
struct _dynforward_ptr){_tmp2F6,_tmp2F6 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_937[3]={& Cyc__gentuple_934,& Cyc__gentuple_935,& Cyc__gentuple_936};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_938,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_937),(void*)((struct _tuple5**)Cyc__genarr_937 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_140={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};
static char _tmp2F9[5]="List";static struct _dynforward_ptr Cyc__genname_942=(struct
_dynforward_ptr){_tmp2F9,_tmp2F9 + 5};static char _tmp2FA[3]="hd";static struct
_tuple5 Cyc__gentuple_939={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp2FA,_tmp2FA + 3},(void*)& Cyc__genrep_140};static char _tmp2FB[
3]="tl";static struct _tuple5 Cyc__gentuple_940={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp2FB,_tmp2FB + 3},(void*)& Cyc__genrep_139};static struct
_tuple5*Cyc__genarr_941[2]={& Cyc__gentuple_939,& Cyc__gentuple_940};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_942,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_941),(void*)((
struct _tuple5**)Cyc__genarr_941 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_139={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp2FE[4]="Opt";static struct _dynforward_ptr Cyc__genname_269=(struct
_dynforward_ptr){_tmp2FE,_tmp2FE + 4};static char _tmp2FF[2]="v";static struct
_tuple5 Cyc__gentuple_267={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp2FF,_tmp2FF + 2},(void*)& Cyc__genrep_139};static struct _tuple5*Cyc__genarr_268[
1]={& Cyc__gentuple_267};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_269,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_268),(void*)((struct _tuple5**)Cyc__genarr_268 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_266={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};
static char _tmp302[9]="Enumdecl";static struct _dynforward_ptr Cyc__genname_274=(
struct _dynforward_ptr){_tmp302,_tmp302 + 9};static char _tmp303[3]="sc";static
struct _tuple5 Cyc__gentuple_270={offsetof(struct Cyc_Absyn_Enumdecl,sc),(struct
_dynforward_ptr){_tmp303,_tmp303 + 3},(void*)& Cyc_Absyn_scope_t_rep};static char
_tmp304[5]="name";static struct _tuple5 Cyc__gentuple_271={offsetof(struct Cyc_Absyn_Enumdecl,name),(
struct _dynforward_ptr){_tmp304,_tmp304 + 5},(void*)& Cyc__genrep_11};static char
_tmp305[7]="fields";static struct _tuple5 Cyc__gentuple_272={offsetof(struct Cyc_Absyn_Enumdecl,fields),(
struct _dynforward_ptr){_tmp305,_tmp305 + 7},(void*)& Cyc__genrep_266};static struct
_tuple5*Cyc__genarr_273[3]={& Cyc__gentuple_270,& Cyc__gentuple_271,& Cyc__gentuple_272};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_274,sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((
struct _tuple5**)Cyc__genarr_273),(void*)((struct _tuple5**)Cyc__genarr_273 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
struct _tuple52{unsigned int f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_275={offsetof(struct _tuple52,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_276={offsetof(struct _tuple52,f2),(
void*)& Cyc__genrep_265};static struct _tuple6 Cyc__gentuple_277={offsetof(struct
_tuple52,f3),(void*)& Cyc__genrep_140};static struct _tuple6*Cyc__genarr_278[3]={&
Cyc__gentuple_275,& Cyc__gentuple_276,& Cyc__gentuple_277};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_264={4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_278),(
void*)((struct _tuple6**)Cyc__genarr_278 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_259;struct _tuple53{unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_260={offsetof(struct _tuple53,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_261={offsetof(struct _tuple53,f2),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_262={
offsetof(struct _tuple53,f3),(void*)& Cyc__genrep_140};static struct _tuple6*Cyc__genarr_263[
3]={& Cyc__gentuple_260,& Cyc__gentuple_261,& Cyc__gentuple_262};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_259={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_263),(
void*)((struct _tuple6**)Cyc__genarr_263 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_255;struct _tuple54{unsigned int f1;struct _tuple0*f2;};static struct
_tuple6 Cyc__gentuple_256={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_257={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_258[2]={& Cyc__gentuple_256,& Cyc__gentuple_257};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_255={4,sizeof(struct _tuple54),{(
void*)((struct _tuple6**)Cyc__genarr_258),(void*)((struct _tuple6**)Cyc__genarr_258
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_244;struct _tuple55{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;int f4;};static struct
_tuple6 Cyc__gentuple_250={offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_251={offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_252={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_245};
static struct _tuple6 Cyc__gentuple_253={offsetof(struct _tuple55,f4),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_254[4]={& Cyc__gentuple_250,& Cyc__gentuple_251,&
Cyc__gentuple_252,& Cyc__gentuple_253};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_244={
4,sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_254),(void*)((
struct _tuple6**)Cyc__genarr_254 + 4)}};static char _tmp30C[7]="Wild_p";static struct
_tuple7 Cyc__gentuple_241={0,(struct _dynforward_ptr){_tmp30C,_tmp30C + 7}};static
char _tmp30D[7]="Null_p";static struct _tuple7 Cyc__gentuple_242={1,(struct
_dynforward_ptr){_tmp30D,_tmp30D + 7}};static struct _tuple7*Cyc__genarr_243[2]={&
Cyc__gentuple_241,& Cyc__gentuple_242};static char _tmp30E[6]="Var_p";static struct
_tuple5 Cyc__gentuple_449={0,(struct _dynforward_ptr){_tmp30E,_tmp30E + 6},(void*)&
Cyc__genrep_444};static char _tmp30F[12]="Reference_p";static struct _tuple5 Cyc__gentuple_450={
1,(struct _dynforward_ptr){_tmp30F,_tmp30F + 12},(void*)& Cyc__genrep_444};static
char _tmp310[9]="TagInt_p";static struct _tuple5 Cyc__gentuple_451={2,(struct
_dynforward_ptr){_tmp310,_tmp310 + 9},(void*)& Cyc__genrep_439};static char _tmp311[
8]="Tuple_p";static struct _tuple5 Cyc__gentuple_452={3,(struct _dynforward_ptr){
_tmp311,_tmp311 + 8},(void*)& Cyc__genrep_434};static char _tmp312[10]="Pointer_p";
static struct _tuple5 Cyc__gentuple_453={4,(struct _dynforward_ptr){_tmp312,_tmp312 + 
10},(void*)& Cyc__genrep_430};static char _tmp313[7]="Aggr_p";static struct _tuple5
Cyc__gentuple_454={5,(struct _dynforward_ptr){_tmp313,_tmp313 + 7},(void*)& Cyc__genrep_340};
static char _tmp314[9]="Tunion_p";static struct _tuple5 Cyc__gentuple_455={6,(struct
_dynforward_ptr){_tmp314,_tmp314 + 9},(void*)& Cyc__genrep_293};static char _tmp315[
6]="Int_p";static struct _tuple5 Cyc__gentuple_456={7,(struct _dynforward_ptr){
_tmp315,_tmp315 + 6},(void*)& Cyc__genrep_283};static char _tmp316[7]="Char_p";
static struct _tuple5 Cyc__gentuple_457={8,(struct _dynforward_ptr){_tmp316,_tmp316 + 
7},(void*)& Cyc__genrep_279};static char _tmp317[8]="Float_p";static struct _tuple5
Cyc__gentuple_458={9,(struct _dynforward_ptr){_tmp317,_tmp317 + 8},(void*)& Cyc__genrep_81};
static char _tmp318[7]="Enum_p";static struct _tuple5 Cyc__gentuple_459={10,(struct
_dynforward_ptr){_tmp318,_tmp318 + 7},(void*)& Cyc__genrep_264};static char _tmp319[
11]="AnonEnum_p";static struct _tuple5 Cyc__gentuple_460={11,(struct _dynforward_ptr){
_tmp319,_tmp319 + 11},(void*)& Cyc__genrep_259};static char _tmp31A[12]="UnknownId_p";
static struct _tuple5 Cyc__gentuple_461={12,(struct _dynforward_ptr){_tmp31A,_tmp31A
+ 12},(void*)& Cyc__genrep_255};static char _tmp31B[14]="UnknownCall_p";static
struct _tuple5 Cyc__gentuple_462={13,(struct _dynforward_ptr){_tmp31B,_tmp31B + 14},(
void*)& Cyc__genrep_244};static char _tmp31C[6]="Exp_p";static struct _tuple5 Cyc__gentuple_463={
14,(struct _dynforward_ptr){_tmp31C,_tmp31C + 6},(void*)& Cyc__genrep_153};static
struct _tuple5*Cyc__genarr_464[15]={& Cyc__gentuple_449,& Cyc__gentuple_450,& Cyc__gentuple_451,&
Cyc__gentuple_452,& Cyc__gentuple_453,& Cyc__gentuple_454,& Cyc__gentuple_455,& Cyc__gentuple_456,&
Cyc__gentuple_457,& Cyc__gentuple_458,& Cyc__gentuple_459,& Cyc__gentuple_460,& Cyc__gentuple_461,&
Cyc__gentuple_462,& Cyc__gentuple_463};static char _tmp31E[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,(struct _dynforward_ptr){_tmp31E,_tmp31E + 8},{(void*)((
struct _tuple7**)Cyc__genarr_243),(void*)((struct _tuple7**)Cyc__genarr_243 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_464),(void*)((struct _tuple5**)Cyc__genarr_464
+ 15)}};static char _tmp31F[4]="Pat";static struct _dynforward_ptr Cyc__genname_469=(
struct _dynforward_ptr){_tmp31F,_tmp31F + 4};static char _tmp320[2]="r";static struct
_tuple5 Cyc__gentuple_465={offsetof(struct Cyc_Absyn_Pat,r),(struct _dynforward_ptr){
_tmp320,_tmp320 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};static char _tmp321[5]="topt";
static struct _tuple5 Cyc__gentuple_466={offsetof(struct Cyc_Absyn_Pat,topt),(struct
_dynforward_ptr){_tmp321,_tmp321 + 5},(void*)& Cyc__genrep_110};static char _tmp322[
4]="loc";static struct _tuple5 Cyc__gentuple_467={offsetof(struct Cyc_Absyn_Pat,loc),(
struct _dynforward_ptr){_tmp322,_tmp322 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_468[3]={& Cyc__gentuple_465,& Cyc__gentuple_466,& Cyc__gentuple_467};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_469,sizeof(struct Cyc_Absyn_Pat),{(void*)((struct
_tuple5**)Cyc__genarr_468),(void*)((struct _tuple5**)Cyc__genarr_468 + 3)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_240={1,1,(void*)((void*)& Cyc_struct_Absyn_Pat_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_232;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;
static char _tmp325[5]="List";static struct _dynforward_ptr Cyc__genname_236=(struct
_dynforward_ptr){_tmp325,_tmp325 + 5};static char _tmp326[3]="hd";static struct
_tuple5 Cyc__gentuple_233={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp326,_tmp326 + 3},(void*)& Cyc__genrep_159};static char _tmp327[
3]="tl";static struct _tuple5 Cyc__gentuple_234={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp327,_tmp327 + 3},(void*)& Cyc__genrep_232};static struct
_tuple5*Cyc__genarr_235[2]={& Cyc__gentuple_233,& Cyc__gentuple_234};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_236,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_235),(void*)((
struct _tuple5**)Cyc__genarr_235 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_232={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp32A[4]="Opt";static struct _dynforward_ptr Cyc__genname_239=(struct
_dynforward_ptr){_tmp32A,_tmp32A + 4};static char _tmp32B[2]="v";static struct
_tuple5 Cyc__gentuple_237={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp32B,_tmp32B + 2},(void*)& Cyc__genrep_232};static struct _tuple5*Cyc__genarr_238[
1]={& Cyc__gentuple_237};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_239,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_238),(void*)((struct _tuple5**)Cyc__genarr_238 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231={1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};
static char _tmp32E[14]="Switch_clause";static struct _dynforward_ptr Cyc__genname_476=(
struct _dynforward_ptr){_tmp32E,_tmp32E + 14};static char _tmp32F[8]="pattern";
static struct _tuple5 Cyc__gentuple_470={offsetof(struct Cyc_Absyn_Switch_clause,pattern),(
struct _dynforward_ptr){_tmp32F,_tmp32F + 8},(void*)& Cyc__genrep_240};static char
_tmp330[9]="pat_vars";static struct _tuple5 Cyc__gentuple_471={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),(
struct _dynforward_ptr){_tmp330,_tmp330 + 9},(void*)& Cyc__genrep_231};static char
_tmp331[13]="where_clause";static struct _tuple5 Cyc__gentuple_472={offsetof(struct
Cyc_Absyn_Switch_clause,where_clause),(struct _dynforward_ptr){_tmp331,_tmp331 + 
13},(void*)& Cyc__genrep_141};static char _tmp332[5]="body";static struct _tuple5 Cyc__gentuple_473={
offsetof(struct Cyc_Absyn_Switch_clause,body),(struct _dynforward_ptr){_tmp332,
_tmp332 + 5},(void*)& Cyc__genrep_145};static char _tmp333[4]="loc";static struct
_tuple5 Cyc__gentuple_474={offsetof(struct Cyc_Absyn_Switch_clause,loc),(struct
_dynforward_ptr){_tmp333,_tmp333 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*
Cyc__genarr_475[5]={& Cyc__gentuple_470,& Cyc__gentuple_471,& Cyc__gentuple_472,&
Cyc__gentuple_473,& Cyc__gentuple_474};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_476,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_475),(void*)((struct _tuple5**)Cyc__genarr_475
+ 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_230={1,1,(void*)((void*)&
Cyc_struct_Absyn_Switch_clause_rep)};static char _tmp336[5]="List";static struct
_dynforward_ptr Cyc__genname_480=(struct _dynforward_ptr){_tmp336,_tmp336 + 5};
static char _tmp337[3]="hd";static struct _tuple5 Cyc__gentuple_477={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp337,_tmp337 + 3},(void*)((void*)&
Cyc__genrep_230)};static char _tmp338[3]="tl";static struct _tuple5 Cyc__gentuple_478={
offsetof(struct Cyc_List_List,tl),(struct _dynforward_ptr){_tmp338,_tmp338 + 3},(
void*)& Cyc__genrep_229};static struct _tuple5*Cyc__genarr_479[2]={& Cyc__gentuple_477,&
Cyc__gentuple_478};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_480,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_479),(void*)((struct _tuple5**)Cyc__genarr_479 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_229={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};
struct _tuple56{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_516={offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_517={offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_518={offsetof(struct _tuple56,f3),(void*)& Cyc__genrep_229};
static struct _tuple6*Cyc__genarr_519[3]={& Cyc__gentuple_516,& Cyc__gentuple_517,&
Cyc__gentuple_518};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_515={4,
sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_519),(void*)((
struct _tuple6**)Cyc__genarr_519 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_504;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_506;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_exp_t46H2_rep;static char _tmp33C[5]="List";static
struct _dynforward_ptr Cyc__genname_510=(struct _dynforward_ptr){_tmp33C,_tmp33C + 5};
static char _tmp33D[3]="hd";static struct _tuple5 Cyc__gentuple_507={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp33D,_tmp33D + 3},(void*)& Cyc__genrep_154};
static char _tmp33E[3]="tl";static struct _tuple5 Cyc__gentuple_508={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp33E,_tmp33E + 3},(void*)& Cyc__genrep_506};
static struct _tuple5*Cyc__genarr_509[2]={& Cyc__gentuple_507,& Cyc__gentuple_508};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_510,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_509),(void*)((struct _tuple5**)Cyc__genarr_509 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_506={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_505;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_505={1,1,(void*)((void*)& Cyc__genrep_230)};struct _tuple57{
unsigned int f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Switch_clause**f3;};static
struct _tuple6 Cyc__gentuple_511={offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_512={offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_506};
static struct _tuple6 Cyc__gentuple_513={offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_505};
static struct _tuple6*Cyc__genarr_514[3]={& Cyc__gentuple_511,& Cyc__gentuple_512,&
Cyc__gentuple_513};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_504={4,
sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_514),(void*)((
struct _tuple6**)Cyc__genarr_514 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_499;
struct _tuple58{unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};
static struct _tuple6 Cyc__gentuple_500={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_501={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_502={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_145};
static struct _tuple6*Cyc__genarr_503[3]={& Cyc__gentuple_500,& Cyc__gentuple_501,&
Cyc__gentuple_502};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_499={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_503),(void*)((
struct _tuple6**)Cyc__genarr_503 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_494;
static struct _tuple6 Cyc__gentuple_495={offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_496={offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_497={offsetof(struct _tuple38,f3),(void*)& Cyc__genrep_145};
static struct _tuple6*Cyc__genarr_498[3]={& Cyc__gentuple_495,& Cyc__gentuple_496,&
Cyc__gentuple_497};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_494={4,
sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_498),(void*)((
struct _tuple6**)Cyc__genarr_498 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_485;
struct _tuple59{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static
struct _tuple6 Cyc__gentuple_490={offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_491={offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_145};
static struct _tuple6 Cyc__gentuple_492={offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_486};
static struct _tuple6*Cyc__genarr_493[3]={& Cyc__gentuple_490,& Cyc__gentuple_491,&
Cyc__gentuple_492};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_485={4,
sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_493),(void*)((
struct _tuple6**)Cyc__genarr_493 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_228;
struct _tuple60{unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_481={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_482={offsetof(struct _tuple60,f2),(void*)& Cyc__genrep_145};
static struct _tuple6 Cyc__gentuple_483={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_229};
static struct _tuple6*Cyc__genarr_484[3]={& Cyc__gentuple_481,& Cyc__gentuple_482,&
Cyc__gentuple_483};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_228={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_484),(void*)((
struct _tuple6**)Cyc__genarr_484 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_158;
struct _tuple61{unsigned int f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
int f4;struct Cyc_Absyn_Stmt*f5;};static struct _tuple6 Cyc__gentuple_222={offsetof(
struct _tuple61,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_223={
offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_185};static struct _tuple6 Cyc__gentuple_224={
offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_225={
offsetof(struct _tuple61,f4),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6
Cyc__gentuple_226={offsetof(struct _tuple61,f5),(void*)& Cyc__genrep_145};static
struct _tuple6*Cyc__genarr_227[5]={& Cyc__gentuple_222,& Cyc__gentuple_223,& Cyc__gentuple_224,&
Cyc__gentuple_225,& Cyc__gentuple_226};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_158={
4,sizeof(struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_227),(void*)((
struct _tuple6**)Cyc__genarr_227 + 5)}};static char _tmp348[7]="Skip_s";static struct
_tuple7 Cyc__gentuple_151={0,(struct _dynforward_ptr){_tmp348,_tmp348 + 7}};static
struct _tuple7*Cyc__genarr_152[1]={& Cyc__gentuple_151};static char _tmp349[6]="Exp_s";
static struct _tuple5 Cyc__gentuple_557={0,(struct _dynforward_ptr){_tmp349,_tmp349 + 
6},(void*)& Cyc__genrep_153};static char _tmp34A[6]="Seq_s";static struct _tuple5 Cyc__gentuple_558={
1,(struct _dynforward_ptr){_tmp34A,_tmp34A + 6},(void*)& Cyc__genrep_552};static
char _tmp34B[9]="Return_s";static struct _tuple5 Cyc__gentuple_559={2,(struct
_dynforward_ptr){_tmp34B,_tmp34B + 9},(void*)& Cyc__genrep_548};static char _tmp34C[
13]="IfThenElse_s";static struct _tuple5 Cyc__gentuple_560={3,(struct
_dynforward_ptr){_tmp34C,_tmp34C + 13},(void*)& Cyc__genrep_542};static char _tmp34D[
8]="While_s";static struct _tuple5 Cyc__gentuple_561={4,(struct _dynforward_ptr){
_tmp34D,_tmp34D + 8},(void*)& Cyc__genrep_537};static char _tmp34E[8]="Break_s";
static struct _tuple5 Cyc__gentuple_562={5,(struct _dynforward_ptr){_tmp34E,_tmp34E + 
8},(void*)& Cyc__genrep_533};static char _tmp34F[11]="Continue_s";static struct
_tuple5 Cyc__gentuple_563={6,(struct _dynforward_ptr){_tmp34F,_tmp34F + 11},(void*)&
Cyc__genrep_533};static char _tmp350[7]="Goto_s";static struct _tuple5 Cyc__gentuple_564={
7,(struct _dynforward_ptr){_tmp350,_tmp350 + 7},(void*)& Cyc__genrep_527};static
char _tmp351[6]="For_s";static struct _tuple5 Cyc__gentuple_565={8,(struct
_dynforward_ptr){_tmp351,_tmp351 + 6},(void*)& Cyc__genrep_520};static char _tmp352[
9]="Switch_s";static struct _tuple5 Cyc__gentuple_566={9,(struct _dynforward_ptr){
_tmp352,_tmp352 + 9},(void*)& Cyc__genrep_515};static char _tmp353[11]="Fallthru_s";
static struct _tuple5 Cyc__gentuple_567={10,(struct _dynforward_ptr){_tmp353,_tmp353
+ 11},(void*)& Cyc__genrep_504};static char _tmp354[7]="Decl_s";static struct _tuple5
Cyc__gentuple_568={11,(struct _dynforward_ptr){_tmp354,_tmp354 + 7},(void*)& Cyc__genrep_499};
static char _tmp355[8]="Label_s";static struct _tuple5 Cyc__gentuple_569={12,(struct
_dynforward_ptr){_tmp355,_tmp355 + 8},(void*)& Cyc__genrep_494};static char _tmp356[
5]="Do_s";static struct _tuple5 Cyc__gentuple_570={13,(struct _dynforward_ptr){
_tmp356,_tmp356 + 5},(void*)& Cyc__genrep_485};static char _tmp357[11]="TryCatch_s";
static struct _tuple5 Cyc__gentuple_571={14,(struct _dynforward_ptr){_tmp357,_tmp357
+ 11},(void*)& Cyc__genrep_228};static char _tmp358[9]="Region_s";static struct
_tuple5 Cyc__gentuple_572={15,(struct _dynforward_ptr){_tmp358,_tmp358 + 9},(void*)&
Cyc__genrep_158};static char _tmp359[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_573={
16,(struct _dynforward_ptr){_tmp359,_tmp359 + 14},(void*)& Cyc__genrep_153};static
struct _tuple5*Cyc__genarr_574[17]={& Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559,&
Cyc__gentuple_560,& Cyc__gentuple_561,& Cyc__gentuple_562,& Cyc__gentuple_563,& Cyc__gentuple_564,&
Cyc__gentuple_565,& Cyc__gentuple_566,& Cyc__gentuple_567,& Cyc__gentuple_568,& Cyc__gentuple_569,&
Cyc__gentuple_570,& Cyc__gentuple_571,& Cyc__gentuple_572,& Cyc__gentuple_573};
static char _tmp35B[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={
5,(struct _dynforward_ptr){_tmp35B,_tmp35B + 9},{(void*)((struct _tuple7**)Cyc__genarr_152),(
void*)((struct _tuple7**)Cyc__genarr_152 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_574),(
void*)((struct _tuple5**)Cyc__genarr_574 + 17)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_146;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static char _tmp35C[5]="List";static struct _dynforward_ptr Cyc__genname_150=(struct
_dynforward_ptr){_tmp35C,_tmp35C + 5};static char _tmp35D[3]="hd";static struct
_tuple5 Cyc__gentuple_147={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp35D,_tmp35D + 3},(void*)& Cyc__genrep_145};static char _tmp35E[
3]="tl";static struct _tuple5 Cyc__gentuple_148={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp35E,_tmp35E + 3},(void*)& Cyc__genrep_146};static struct
_tuple5*Cyc__genarr_149[2]={& Cyc__gentuple_147,& Cyc__gentuple_148};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_150,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_149),(void*)((
struct _tuple5**)Cyc__genarr_149 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_146={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_142[0]={};static char
_tmp362[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,(struct _dynforward_ptr){_tmp362,_tmp362 + 11},{(void*)((struct _tuple8**)Cyc__genarr_142),(
void*)((struct _tuple8**)Cyc__genarr_142 + 0)}};static char _tmp363[5]="Stmt";static
struct _dynforward_ptr Cyc__genname_581=(struct _dynforward_ptr){_tmp363,_tmp363 + 5};
static char _tmp364[2]="r";static struct _tuple5 Cyc__gentuple_575={offsetof(struct
Cyc_Absyn_Stmt,r),(struct _dynforward_ptr){_tmp364,_tmp364 + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp365[4]="loc";static struct _tuple5 Cyc__gentuple_576={offsetof(struct
Cyc_Absyn_Stmt,loc),(struct _dynforward_ptr){_tmp365,_tmp365 + 4},(void*)& Cyc__genrep_2};
static char _tmp366[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_577={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),(struct _dynforward_ptr){_tmp366,
_tmp366 + 16},(void*)& Cyc__genrep_146};static char _tmp367[10]="try_depth";static
struct _tuple5 Cyc__gentuple_578={offsetof(struct Cyc_Absyn_Stmt,try_depth),(struct
_dynforward_ptr){_tmp367,_tmp367 + 10},(void*)((void*)& Cyc__genrep_10)};static
char _tmp368[6]="annot";static struct _tuple5 Cyc__gentuple_579={offsetof(struct Cyc_Absyn_Stmt,annot),(
struct _dynforward_ptr){_tmp368,_tmp368 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple5*Cyc__genarr_580[5]={& Cyc__gentuple_575,& Cyc__gentuple_576,&
Cyc__gentuple_577,& Cyc__gentuple_578,& Cyc__gentuple_579};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _dynforward_ptr*)& Cyc__genname_581,sizeof(
struct Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_580),(void*)((struct
_tuple5**)Cyc__genarr_580 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145={
1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_816;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_816={1,1,(void*)((
void*)& Cyc_struct_Absyn_Vardecl_rep)};static char _tmp36C[7]="Fndecl";static struct
_dynforward_ptr Cyc__genname_845=(struct _dynforward_ptr){_tmp36C,_tmp36C + 7};
static char _tmp36D[3]="sc";static struct _tuple5 Cyc__gentuple_829={offsetof(struct
Cyc_Absyn_Fndecl,sc),(struct _dynforward_ptr){_tmp36D,_tmp36D + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp36E[10]="is_inline";static struct _tuple5 Cyc__gentuple_830={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),(struct _dynforward_ptr){_tmp36E,
_tmp36E + 10},(void*)((void*)& Cyc__genrep_10)};static char _tmp36F[5]="name";static
struct _tuple5 Cyc__gentuple_831={offsetof(struct Cyc_Absyn_Fndecl,name),(struct
_dynforward_ptr){_tmp36F,_tmp36F + 5},(void*)& Cyc__genrep_11};static char _tmp370[4]="tvs";
static struct _tuple5 Cyc__gentuple_832={offsetof(struct Cyc_Absyn_Fndecl,tvs),(
struct _dynforward_ptr){_tmp370,_tmp370 + 4},(void*)& Cyc__genrep_321};static char
_tmp371[7]="effect";static struct _tuple5 Cyc__gentuple_833={offsetof(struct Cyc_Absyn_Fndecl,effect),(
struct _dynforward_ptr){_tmp371,_tmp371 + 7},(void*)& Cyc__genrep_110};static char
_tmp372[9]="ret_type";static struct _tuple5 Cyc__gentuple_834={offsetof(struct Cyc_Absyn_Fndecl,ret_type),(
struct _dynforward_ptr){_tmp372,_tmp372 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp373[5]="args";static struct _tuple5 Cyc__gentuple_835={offsetof(
struct Cyc_Absyn_Fndecl,args),(struct _dynforward_ptr){_tmp373,_tmp373 + 5},(void*)&
Cyc__genrep_818};static char _tmp374[10]="c_varargs";static struct _tuple5 Cyc__gentuple_836={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),(struct _dynforward_ptr){_tmp374,
_tmp374 + 10},(void*)((void*)& Cyc__genrep_10)};static char _tmp375[12]="cyc_varargs";
static struct _tuple5 Cyc__gentuple_837={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),(
struct _dynforward_ptr){_tmp375,_tmp375 + 12},(void*)& Cyc__genrep_817};static char
_tmp376[7]="rgn_po";static struct _tuple5 Cyc__gentuple_838={offsetof(struct Cyc_Absyn_Fndecl,rgn_po),(
struct _dynforward_ptr){_tmp376,_tmp376 + 7},(void*)& Cyc__genrep_382};static char
_tmp377[5]="body";static struct _tuple5 Cyc__gentuple_839={offsetof(struct Cyc_Absyn_Fndecl,body),(
struct _dynforward_ptr){_tmp377,_tmp377 + 5},(void*)& Cyc__genrep_145};static char
_tmp378[11]="cached_typ";static struct _tuple5 Cyc__gentuple_840={offsetof(struct
Cyc_Absyn_Fndecl,cached_typ),(struct _dynforward_ptr){_tmp378,_tmp378 + 11},(void*)&
Cyc__genrep_110};static char _tmp379[15]="param_vardecls";static struct _tuple5 Cyc__gentuple_841={
offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),(struct _dynforward_ptr){_tmp379,
_tmp379 + 15},(void*)& Cyc__genrep_231};static char _tmp37A[11]="fn_vardecl";static
struct _tuple5 Cyc__gentuple_842={offsetof(struct Cyc_Absyn_Fndecl,fn_vardecl),(
struct _dynforward_ptr){_tmp37A,_tmp37A + 11},(void*)& Cyc__genrep_816};static char
_tmp37B[11]="attributes";static struct _tuple5 Cyc__gentuple_843={offsetof(struct
Cyc_Absyn_Fndecl,attributes),(struct _dynforward_ptr){_tmp37B,_tmp37B + 11},(void*)&
Cyc__genrep_62};static struct _tuple5*Cyc__genarr_844[15]={& Cyc__gentuple_829,& Cyc__gentuple_830,&
Cyc__gentuple_831,& Cyc__gentuple_832,& Cyc__gentuple_833,& Cyc__gentuple_834,& Cyc__gentuple_835,&
Cyc__gentuple_836,& Cyc__gentuple_837,& Cyc__gentuple_838,& Cyc__gentuple_839,& Cyc__gentuple_840,&
Cyc__gentuple_841,& Cyc__gentuple_842,& Cyc__gentuple_843};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Fndecl_rep={3,(struct _dynforward_ptr*)& Cyc__genname_845,sizeof(
struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_844),(void*)((
struct _tuple5**)Cyc__genarr_844 + 15)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_815={
1,1,(void*)((void*)& Cyc_struct_Absyn_Fndecl_rep)};struct _tuple62{unsigned int f1;
struct Cyc_Absyn_Fndecl*f2;};static struct _tuple6 Cyc__gentuple_846={offsetof(
struct _tuple62,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_847={
offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_815};static struct _tuple6*Cyc__genarr_848[
2]={& Cyc__gentuple_846,& Cyc__gentuple_847};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_814={4,sizeof(struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_848),(
void*)((struct _tuple6**)Cyc__genarr_848 + 2)}};static char _tmp37F[13]="Unresolved_b";
static struct _tuple7 Cyc__gentuple_808={0,(struct _dynforward_ptr){_tmp37F,_tmp37F + 
13}};static struct _tuple7*Cyc__genarr_809[1]={& Cyc__gentuple_808};static char
_tmp380[9]="Global_b";static struct _tuple5 Cyc__gentuple_849={0,(struct
_dynforward_ptr){_tmp380,_tmp380 + 9},(void*)& Cyc__genrep_810};static char _tmp381[
10]="Funname_b";static struct _tuple5 Cyc__gentuple_850={1,(struct _dynforward_ptr){
_tmp381,_tmp381 + 10},(void*)& Cyc__genrep_814};static char _tmp382[8]="Param_b";
static struct _tuple5 Cyc__gentuple_851={2,(struct _dynforward_ptr){_tmp382,_tmp382 + 
8},(void*)& Cyc__genrep_810};static char _tmp383[8]="Local_b";static struct _tuple5
Cyc__gentuple_852={3,(struct _dynforward_ptr){_tmp383,_tmp383 + 8},(void*)& Cyc__genrep_810};
static char _tmp384[6]="Pat_b";static struct _tuple5 Cyc__gentuple_853={4,(struct
_dynforward_ptr){_tmp384,_tmp384 + 6},(void*)& Cyc__genrep_810};static struct
_tuple5*Cyc__genarr_854[5]={& Cyc__gentuple_849,& Cyc__gentuple_850,& Cyc__gentuple_851,&
Cyc__gentuple_852,& Cyc__gentuple_853};static char _tmp386[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,(struct _dynforward_ptr){_tmp386,_tmp386 + 8},{(void*)((
struct _tuple7**)Cyc__genarr_809),(void*)((struct _tuple7**)Cyc__genarr_809 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_854),(void*)((struct _tuple5**)Cyc__genarr_854
+ 5)}};struct _tuple63{unsigned int f1;struct _tuple0*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_855={offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_856={offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_857={offsetof(struct _tuple63,f3),(void*)& Cyc_Absyn_binding_t_rep};
static struct _tuple6*Cyc__genarr_858[3]={& Cyc__gentuple_855,& Cyc__gentuple_856,&
Cyc__gentuple_857};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_807={4,
sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_858),(void*)((
struct _tuple6**)Cyc__genarr_858 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_802;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;static char _tmp388[5]="Plus";
static struct _tuple7 Cyc__gentuple_772={0,(struct _dynforward_ptr){_tmp388,_tmp388 + 
5}};static char _tmp389[6]="Times";static struct _tuple7 Cyc__gentuple_773={1,(struct
_dynforward_ptr){_tmp389,_tmp389 + 6}};static char _tmp38A[6]="Minus";static struct
_tuple7 Cyc__gentuple_774={2,(struct _dynforward_ptr){_tmp38A,_tmp38A + 6}};static
char _tmp38B[4]="Div";static struct _tuple7 Cyc__gentuple_775={3,(struct
_dynforward_ptr){_tmp38B,_tmp38B + 4}};static char _tmp38C[4]="Mod";static struct
_tuple7 Cyc__gentuple_776={4,(struct _dynforward_ptr){_tmp38C,_tmp38C + 4}};static
char _tmp38D[3]="Eq";static struct _tuple7 Cyc__gentuple_777={5,(struct
_dynforward_ptr){_tmp38D,_tmp38D + 3}};static char _tmp38E[4]="Neq";static struct
_tuple7 Cyc__gentuple_778={6,(struct _dynforward_ptr){_tmp38E,_tmp38E + 4}};static
char _tmp38F[3]="Gt";static struct _tuple7 Cyc__gentuple_779={7,(struct
_dynforward_ptr){_tmp38F,_tmp38F + 3}};static char _tmp390[3]="Lt";static struct
_tuple7 Cyc__gentuple_780={8,(struct _dynforward_ptr){_tmp390,_tmp390 + 3}};static
char _tmp391[4]="Gte";static struct _tuple7 Cyc__gentuple_781={9,(struct
_dynforward_ptr){_tmp391,_tmp391 + 4}};static char _tmp392[4]="Lte";static struct
_tuple7 Cyc__gentuple_782={10,(struct _dynforward_ptr){_tmp392,_tmp392 + 4}};static
char _tmp393[4]="Not";static struct _tuple7 Cyc__gentuple_783={11,(struct
_dynforward_ptr){_tmp393,_tmp393 + 4}};static char _tmp394[7]="Bitnot";static struct
_tuple7 Cyc__gentuple_784={12,(struct _dynforward_ptr){_tmp394,_tmp394 + 7}};static
char _tmp395[7]="Bitand";static struct _tuple7 Cyc__gentuple_785={13,(struct
_dynforward_ptr){_tmp395,_tmp395 + 7}};static char _tmp396[6]="Bitor";static struct
_tuple7 Cyc__gentuple_786={14,(struct _dynforward_ptr){_tmp396,_tmp396 + 6}};static
char _tmp397[7]="Bitxor";static struct _tuple7 Cyc__gentuple_787={15,(struct
_dynforward_ptr){_tmp397,_tmp397 + 7}};static char _tmp398[10]="Bitlshift";static
struct _tuple7 Cyc__gentuple_788={16,(struct _dynforward_ptr){_tmp398,_tmp398 + 10}};
static char _tmp399[11]="Bitlrshift";static struct _tuple7 Cyc__gentuple_789={17,(
struct _dynforward_ptr){_tmp399,_tmp399 + 11}};static char _tmp39A[11]="Bitarshift";
static struct _tuple7 Cyc__gentuple_790={18,(struct _dynforward_ptr){_tmp39A,_tmp39A
+ 11}};static char _tmp39B[5]="Size";static struct _tuple7 Cyc__gentuple_791={19,(
struct _dynforward_ptr){_tmp39B,_tmp39B + 5}};static struct _tuple7*Cyc__genarr_792[
20]={& Cyc__gentuple_772,& Cyc__gentuple_773,& Cyc__gentuple_774,& Cyc__gentuple_775,&
Cyc__gentuple_776,& Cyc__gentuple_777,& Cyc__gentuple_778,& Cyc__gentuple_779,& Cyc__gentuple_780,&
Cyc__gentuple_781,& Cyc__gentuple_782,& Cyc__gentuple_783,& Cyc__gentuple_784,& Cyc__gentuple_785,&
Cyc__gentuple_786,& Cyc__gentuple_787,& Cyc__gentuple_788,& Cyc__gentuple_789,& Cyc__gentuple_790,&
Cyc__gentuple_791};static struct _tuple5*Cyc__genarr_793[0]={};static char _tmp39D[7]="Primop";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,(struct _dynforward_ptr){
_tmp39D,_tmp39D + 7},{(void*)((struct _tuple7**)Cyc__genarr_792),(void*)((struct
_tuple7**)Cyc__genarr_792 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_793),(void*)((
struct _tuple5**)Cyc__genarr_793 + 0)}};struct _tuple64{unsigned int f1;void*f2;
struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_803={offsetof(struct
_tuple64,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_804={
offsetof(struct _tuple64,f2),(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple6
Cyc__gentuple_805={offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_506};static
struct _tuple6*Cyc__genarr_806[3]={& Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_802={4,sizeof(struct _tuple64),{(
void*)((struct _tuple6**)Cyc__genarr_806),(void*)((struct _tuple6**)Cyc__genarr_806
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_770;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_771;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp39F[4]="Opt";static struct _dynforward_ptr Cyc__genname_796=(struct
_dynforward_ptr){_tmp39F,_tmp39F + 4};static char _tmp3A0[2]="v";static struct
_tuple5 Cyc__gentuple_794={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp3A0,_tmp3A0 + 2},(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple5*Cyc__genarr_795[
1]={& Cyc__gentuple_794};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_796,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_795),(void*)((struct _tuple5**)Cyc__genarr_795 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_771={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple65{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_797={offsetof(struct
_tuple65,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_798={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_799={
offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_771};static struct _tuple6 Cyc__gentuple_800={
offsetof(struct _tuple65,f4),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_801[
4]={& Cyc__gentuple_797,& Cyc__gentuple_798,& Cyc__gentuple_799,& Cyc__gentuple_800};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_770={4,sizeof(struct _tuple65),{(
void*)((struct _tuple6**)Cyc__genarr_801),(void*)((struct _tuple6**)Cyc__genarr_801
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_759;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_incrementor_t_rep;static char _tmp3A4[7]="PreInc";static struct _tuple7 Cyc__gentuple_760={
0,(struct _dynforward_ptr){_tmp3A4,_tmp3A4 + 7}};static char _tmp3A5[8]="PostInc";
static struct _tuple7 Cyc__gentuple_761={1,(struct _dynforward_ptr){_tmp3A5,_tmp3A5 + 
8}};static char _tmp3A6[7]="PreDec";static struct _tuple7 Cyc__gentuple_762={2,(
struct _dynforward_ptr){_tmp3A6,_tmp3A6 + 7}};static char _tmp3A7[8]="PostDec";
static struct _tuple7 Cyc__gentuple_763={3,(struct _dynforward_ptr){_tmp3A7,_tmp3A7 + 
8}};static struct _tuple7*Cyc__genarr_764[4]={& Cyc__gentuple_760,& Cyc__gentuple_761,&
Cyc__gentuple_762,& Cyc__gentuple_763};static struct _tuple5*Cyc__genarr_765[0]={};
static char _tmp3A9[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,(struct _dynforward_ptr){_tmp3A9,_tmp3A9 + 12},{(void*)((struct _tuple7**)Cyc__genarr_764),(
void*)((struct _tuple7**)Cyc__genarr_764 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_765),(
void*)((struct _tuple5**)Cyc__genarr_765 + 0)}};struct _tuple66{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_766={offsetof(
struct _tuple66,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_767={
offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_768={
offsetof(struct _tuple66,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_769[3]={& Cyc__gentuple_766,& Cyc__gentuple_767,& Cyc__gentuple_768};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_759={4,sizeof(struct _tuple66),{(
void*)((struct _tuple6**)Cyc__genarr_769),(void*)((struct _tuple6**)Cyc__genarr_769
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_753;struct _tuple67{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_754={offsetof(struct _tuple67,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_755={offsetof(struct _tuple67,f2),(
void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_756={offsetof(struct
_tuple67,f3),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_757={
offsetof(struct _tuple67,f4),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_758[
4]={& Cyc__gentuple_754,& Cyc__gentuple_755,& Cyc__gentuple_756,& Cyc__gentuple_757};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_753={4,sizeof(struct _tuple67),{(
void*)((struct _tuple6**)Cyc__genarr_758),(void*)((struct _tuple6**)Cyc__genarr_758
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_678;struct _tuple68{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static struct
_tuple6 Cyc__gentuple_679={offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_680={offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_681={offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_154};
static struct _tuple6*Cyc__genarr_682[3]={& Cyc__gentuple_679,& Cyc__gentuple_680,&
Cyc__gentuple_681};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_678={4,
sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_682),(void*)((
struct _tuple6**)Cyc__genarr_682 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_748;
static struct _tuple6 Cyc__gentuple_749={offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_750={offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_751={offsetof(struct _tuple56,f3),(void*)& Cyc__genrep_506};
static struct _tuple6*Cyc__genarr_752[3]={& Cyc__gentuple_749,& Cyc__gentuple_750,&
Cyc__gentuple_751};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_748={4,
sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_752),(void*)((
struct _tuple6**)Cyc__genarr_752 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_729;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_730;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_731;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_731={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
static char _tmp3AF[15]="VarargCallInfo";static struct _dynforward_ptr Cyc__genname_742=(
struct _dynforward_ptr){_tmp3AF,_tmp3AF + 15};static char _tmp3B0[12]="num_varargs";
static struct _tuple5 Cyc__gentuple_738={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),(
struct _dynforward_ptr){_tmp3B0,_tmp3B0 + 12},(void*)((void*)& Cyc__genrep_10)};
static char _tmp3B1[10]="injectors";static struct _tuple5 Cyc__gentuple_739={
offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),(struct _dynforward_ptr){
_tmp3B1,_tmp3B1 + 10},(void*)& Cyc__genrep_313};static char _tmp3B2[4]="vai";static
struct _tuple5 Cyc__gentuple_740={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),(
struct _dynforward_ptr){_tmp3B2,_tmp3B2 + 4},(void*)& Cyc__genrep_731};static struct
_tuple5*Cyc__genarr_741[3]={& Cyc__gentuple_738,& Cyc__gentuple_739,& Cyc__gentuple_740};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_742,sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((
struct _tuple5**)Cyc__genarr_741),(void*)((struct _tuple5**)Cyc__genarr_741 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_730={1,1,(void*)((void*)& Cyc_Absyn_vararg_call_info_t_rep)};
struct _tuple69{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;
struct Cyc_Absyn_VarargCallInfo*f4;};static struct _tuple6 Cyc__gentuple_743={
offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_744={
offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_745={
offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_506};static struct _tuple6 Cyc__gentuple_746={
offsetof(struct _tuple69,f4),(void*)& Cyc__genrep_730};static struct _tuple6*Cyc__genarr_747[
4]={& Cyc__gentuple_743,& Cyc__gentuple_744,& Cyc__gentuple_745,& Cyc__gentuple_746};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_729={4,sizeof(struct _tuple69),{(
void*)((struct _tuple6**)Cyc__genarr_747),(void*)((struct _tuple6**)Cyc__genarr_747
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_724;static struct _tuple6
Cyc__gentuple_725={offsetof(struct _tuple56,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_726={offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_727={offsetof(struct _tuple56,f3),(void*)& Cyc__genrep_121};
static struct _tuple6*Cyc__genarr_728[3]={& Cyc__gentuple_725,& Cyc__gentuple_726,&
Cyc__gentuple_727};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_724={4,
sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_728),(void*)((
struct _tuple6**)Cyc__genarr_728 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_711;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep;static char _tmp3B7[
17]="Unknown_coercion";static struct _tuple7 Cyc__gentuple_712={0,(struct
_dynforward_ptr){_tmp3B7,_tmp3B7 + 17}};static char _tmp3B8[12]="No_coercion";
static struct _tuple7 Cyc__gentuple_713={1,(struct _dynforward_ptr){_tmp3B8,_tmp3B8 + 
12}};static char _tmp3B9[16]="NonNull_to_Null";static struct _tuple7 Cyc__gentuple_714={
2,(struct _dynforward_ptr){_tmp3B9,_tmp3B9 + 16}};static char _tmp3BA[15]="Other_coercion";
static struct _tuple7 Cyc__gentuple_715={3,(struct _dynforward_ptr){_tmp3BA,_tmp3BA + 
15}};static struct _tuple7*Cyc__genarr_716[4]={& Cyc__gentuple_712,& Cyc__gentuple_713,&
Cyc__gentuple_714,& Cyc__gentuple_715};static struct _tuple5*Cyc__genarr_717[0]={};
static char _tmp3BC[9]="Coercion";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep={
5,(struct _dynforward_ptr){_tmp3BC,_tmp3BC + 9},{(void*)((struct _tuple7**)Cyc__genarr_716),(
void*)((struct _tuple7**)Cyc__genarr_716 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_717),(
void*)((struct _tuple5**)Cyc__genarr_717 + 0)}};struct _tuple70{unsigned int f1;void*
f2;struct Cyc_Absyn_Exp*f3;int f4;void*f5;};static struct _tuple6 Cyc__gentuple_718={
offsetof(struct _tuple70,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_719={
offsetof(struct _tuple70,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_720={offsetof(struct _tuple70,f3),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_721={offsetof(struct _tuple70,f4),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6 Cyc__gentuple_722={offsetof(struct _tuple70,f5),(
void*)& Cyc_Absyn_coercion_t_rep};static struct _tuple6*Cyc__genarr_723[5]={& Cyc__gentuple_718,&
Cyc__gentuple_719,& Cyc__gentuple_720,& Cyc__gentuple_721,& Cyc__gentuple_722};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_711={4,sizeof(struct _tuple70),{(
void*)((struct _tuple6**)Cyc__genarr_723),(void*)((struct _tuple6**)Cyc__genarr_723
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_706;static struct _tuple6
Cyc__gentuple_707={offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_708={offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_141};
static struct _tuple6 Cyc__gentuple_709={offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_154};
static struct _tuple6*Cyc__genarr_710[3]={& Cyc__gentuple_707,& Cyc__gentuple_708,&
Cyc__gentuple_709};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_706={4,
sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_710),(void*)((
struct _tuple6**)Cyc__genarr_710 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_116;
static struct _tuple6 Cyc__gentuple_117={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_118={offsetof(struct _tuple6,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_119[2]={& Cyc__gentuple_117,&
Cyc__gentuple_118};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,
sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_119),(void*)((struct
_tuple6**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_693;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_695;struct _tuple71{unsigned int f1;
unsigned int f2;};static struct _tuple6 Cyc__gentuple_696={offsetof(struct _tuple71,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_697={offsetof(struct
_tuple71,f2),(void*)& Cyc__genrep_24};static struct _tuple6*Cyc__genarr_698[2]={&
Cyc__gentuple_696,& Cyc__gentuple_697};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_695={
4,sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_698),(void*)((
struct _tuple6**)Cyc__genarr_698 + 2)}};static struct _tuple7*Cyc__genarr_694[0]={};
static char _tmp3C1[12]="StructField";static struct _tuple5 Cyc__gentuple_699={0,(
struct _dynforward_ptr){_tmp3C1,_tmp3C1 + 12},(void*)& Cyc__genrep_346};static char
_tmp3C2[11]="TupleIndex";static struct _tuple5 Cyc__gentuple_700={1,(struct
_dynforward_ptr){_tmp3C2,_tmp3C2 + 11},(void*)& Cyc__genrep_695};static struct
_tuple5*Cyc__genarr_701[2]={& Cyc__gentuple_699,& Cyc__gentuple_700};static char
_tmp3C4[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep={
5,(struct _dynforward_ptr){_tmp3C4,_tmp3C4 + 14},{(void*)((struct _tuple7**)Cyc__genarr_694),(
void*)((struct _tuple7**)Cyc__genarr_694 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_701),(
void*)((struct _tuple5**)Cyc__genarr_701 + 2)}};static struct _tuple6 Cyc__gentuple_702={
offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_703={
offsetof(struct _tuple27,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_704={offsetof(struct _tuple27,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};
static struct _tuple6*Cyc__genarr_705[3]={& Cyc__gentuple_702,& Cyc__gentuple_703,&
Cyc__gentuple_704};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_693={4,
sizeof(struct _tuple27),{(void*)((struct _tuple6**)Cyc__genarr_705),(void*)((
struct _tuple6**)Cyc__genarr_705 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_688;
struct _tuple72{unsigned int f1;struct Cyc_List_List*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_689={offsetof(struct _tuple72,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_690={offsetof(struct _tuple72,f2),(void*)& Cyc__genrep_321};
static struct _tuple6 Cyc__gentuple_691={offsetof(struct _tuple72,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_692[3]={& Cyc__gentuple_689,&
Cyc__gentuple_690,& Cyc__gentuple_691};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_688={
4,sizeof(struct _tuple72),{(void*)((struct _tuple6**)Cyc__genarr_692),(void*)((
struct _tuple6**)Cyc__genarr_692 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_683;
struct _tuple73{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _dynforward_ptr*f3;};
static struct _tuple6 Cyc__gentuple_684={offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_685={offsetof(struct _tuple73,f2),(void*)& Cyc__genrep_154};
static struct _tuple6 Cyc__gentuple_686={offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_687[3]={& Cyc__gentuple_684,& Cyc__gentuple_685,&
Cyc__gentuple_686};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_683={4,
sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_687),(void*)((
struct _tuple6**)Cyc__genarr_687 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_674;
static struct _tuple6 Cyc__gentuple_675={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_676={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_506};
static struct _tuple6*Cyc__genarr_677[2]={& Cyc__gentuple_675,& Cyc__gentuple_676};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_674={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_677),(void*)((struct _tuple6**)Cyc__genarr_677
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_659;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_660;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_661;static struct
_tuple6 Cyc__gentuple_666={offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_662};
static struct _tuple6 Cyc__gentuple_667={offsetof(struct _tuple1,f2),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_668={offsetof(struct _tuple1,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_669[3]={& Cyc__gentuple_666,&
Cyc__gentuple_667,& Cyc__gentuple_668};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_661={
4,sizeof(struct _tuple1),{(void*)((struct _tuple6**)Cyc__genarr_669),(void*)((
struct _tuple6**)Cyc__genarr_669 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_660={
1,1,(void*)((void*)& Cyc__genrep_661)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_586;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_587;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_588;static struct _tuple6 Cyc__gentuple_589={offsetof(struct _tuple12,f1),(
void*)& Cyc__genrep_344};static struct _tuple6 Cyc__gentuple_590={offsetof(struct
_tuple12,f2),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_591[2]={&
Cyc__gentuple_589,& Cyc__gentuple_590};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_588={
4,sizeof(struct _tuple12),{(void*)((struct _tuple6**)Cyc__genarr_591),(void*)((
struct _tuple6**)Cyc__genarr_591 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_587={
1,1,(void*)((void*)& Cyc__genrep_588)};static char _tmp3CD[5]="List";static struct
_dynforward_ptr Cyc__genname_595=(struct _dynforward_ptr){_tmp3CD,_tmp3CD + 5};
static char _tmp3CE[3]="hd";static struct _tuple5 Cyc__gentuple_592={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp3CE,_tmp3CE + 3},(void*)& Cyc__genrep_587};
static char _tmp3CF[3]="tl";static struct _tuple5 Cyc__gentuple_593={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp3CF,_tmp3CF + 3},(void*)& Cyc__genrep_586};
static struct _tuple5*Cyc__genarr_594[2]={& Cyc__gentuple_592,& Cyc__gentuple_593};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_595,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_594),(void*)((struct _tuple5**)Cyc__genarr_594 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_586={1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple74{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_670={offsetof(struct _tuple74,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_671={offsetof(struct _tuple74,f2),(void*)& Cyc__genrep_660};
static struct _tuple6 Cyc__gentuple_672={offsetof(struct _tuple74,f3),(void*)& Cyc__genrep_586};
static struct _tuple6*Cyc__genarr_673[3]={& Cyc__gentuple_670,& Cyc__gentuple_671,&
Cyc__gentuple_672};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_659={4,
sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_673),(void*)((
struct _tuple6**)Cyc__genarr_673 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_655;
static struct _tuple6 Cyc__gentuple_656={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_657={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_586};
static struct _tuple6*Cyc__genarr_658[2]={& Cyc__gentuple_656,& Cyc__gentuple_657};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_655={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_658),(void*)((struct _tuple6**)Cyc__genarr_658
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_648;struct _tuple75{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_649={offsetof(struct _tuple75,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_650={offsetof(struct
_tuple75,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_651={
offsetof(struct _tuple75,f3),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_652={
offsetof(struct _tuple75,f4),(void*)& Cyc__genrep_154};static struct _tuple6 Cyc__gentuple_653={
offsetof(struct _tuple75,f5),(void*)((void*)& Cyc__genrep_10)};static struct _tuple6*
Cyc__genarr_654[5]={& Cyc__gentuple_649,& Cyc__gentuple_650,& Cyc__gentuple_651,&
Cyc__gentuple_652,& Cyc__gentuple_653};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_648={
4,sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_654),(void*)((
struct _tuple6**)Cyc__genarr_654 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_640;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_641;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_641={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct
_tuple76{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;struct Cyc_Absyn_Aggrdecl*f5;};static struct _tuple6 Cyc__gentuple_642={offsetof(
struct _tuple76,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_643={
offsetof(struct _tuple76,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_644={
offsetof(struct _tuple76,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_645={
offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_586};static struct _tuple6 Cyc__gentuple_646={
offsetof(struct _tuple76,f5),(void*)& Cyc__genrep_641};static struct _tuple6*Cyc__genarr_647[
5]={& Cyc__gentuple_642,& Cyc__gentuple_643,& Cyc__gentuple_644,& Cyc__gentuple_645,&
Cyc__gentuple_646};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_640={4,
sizeof(struct _tuple76),{(void*)((struct _tuple6**)Cyc__genarr_647),(void*)((
struct _tuple6**)Cyc__genarr_647 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_635;
static struct _tuple6 Cyc__gentuple_636={offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_637={offsetof(struct _tuple64,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_638={offsetof(struct
_tuple64,f3),(void*)& Cyc__genrep_586};static struct _tuple6*Cyc__genarr_639[3]={&
Cyc__gentuple_636,& Cyc__gentuple_637,& Cyc__gentuple_638};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_635={4,sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_639),(
void*)((struct _tuple6**)Cyc__genarr_639 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_629;struct _tuple77{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*
f3;struct Cyc_Absyn_Tunionfield*f4;};static struct _tuple6 Cyc__gentuple_630={
offsetof(struct _tuple77,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_631={
offsetof(struct _tuple77,f2),(void*)& Cyc__genrep_506};static struct _tuple6 Cyc__gentuple_632={
offsetof(struct _tuple77,f3),(void*)((void*)& Cyc__genrep_311)};static struct
_tuple6 Cyc__gentuple_633={offsetof(struct _tuple77,f4),(void*)& Cyc__genrep_294};
static struct _tuple6*Cyc__genarr_634[4]={& Cyc__gentuple_630,& Cyc__gentuple_631,&
Cyc__gentuple_632,& Cyc__gentuple_633};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_629={
4,sizeof(struct _tuple77),{(void*)((struct _tuple6**)Cyc__genarr_634),(void*)((
struct _tuple6**)Cyc__genarr_634 + 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_622;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_623;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_623={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_616;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_616={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple78{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_624={offsetof(struct _tuple78,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_625={offsetof(struct _tuple78,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_626={offsetof(struct
_tuple78,f3),(void*)& Cyc__genrep_623};static struct _tuple6 Cyc__gentuple_627={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_616};static struct _tuple6*Cyc__genarr_628[
4]={& Cyc__gentuple_624,& Cyc__gentuple_625,& Cyc__gentuple_626,& Cyc__gentuple_627};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_622={4,sizeof(struct _tuple78),{(
void*)((struct _tuple6**)Cyc__genarr_628),(void*)((struct _tuple6**)Cyc__genarr_628
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_615;struct _tuple79{
unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*f4;};static
struct _tuple6 Cyc__gentuple_617={offsetof(struct _tuple79,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_618={offsetof(struct _tuple79,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_619={offsetof(struct _tuple79,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_620={offsetof(struct
_tuple79,f4),(void*)& Cyc__genrep_616};static struct _tuple6*Cyc__genarr_621[4]={&
Cyc__gentuple_617,& Cyc__gentuple_618,& Cyc__gentuple_619,& Cyc__gentuple_620};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_615={4,sizeof(struct _tuple79),{(
void*)((struct _tuple6**)Cyc__genarr_621),(void*)((struct _tuple6**)Cyc__genarr_621
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_604;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_malloc_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp3DE[11]="MallocInfo";static struct _dynforward_ptr Cyc__genname_611=(
struct _dynforward_ptr){_tmp3DE,_tmp3DE + 11};static char _tmp3DF[10]="is_calloc";
static struct _tuple5 Cyc__gentuple_605={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),(
struct _dynforward_ptr){_tmp3DF,_tmp3DF + 10},(void*)((void*)& Cyc__genrep_10)};
static char _tmp3E0[4]="rgn";static struct _tuple5 Cyc__gentuple_606={offsetof(struct
Cyc_Absyn_MallocInfo,rgn),(struct _dynforward_ptr){_tmp3E0,_tmp3E0 + 4},(void*)&
Cyc__genrep_141};static char _tmp3E1[9]="elt_type";static struct _tuple5 Cyc__gentuple_607={
offsetof(struct Cyc_Absyn_MallocInfo,elt_type),(struct _dynforward_ptr){_tmp3E1,
_tmp3E1 + 9},(void*)& Cyc__genrep_130};static char _tmp3E2[9]="num_elts";static
struct _tuple5 Cyc__gentuple_608={offsetof(struct Cyc_Absyn_MallocInfo,num_elts),(
struct _dynforward_ptr){_tmp3E2,_tmp3E2 + 9},(void*)& Cyc__genrep_154};static char
_tmp3E3[11]="fat_result";static struct _tuple5 Cyc__gentuple_609={offsetof(struct
Cyc_Absyn_MallocInfo,fat_result),(struct _dynforward_ptr){_tmp3E3,_tmp3E3 + 11},(
void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_610[5]={& Cyc__gentuple_605,&
Cyc__gentuple_606,& Cyc__gentuple_607,& Cyc__gentuple_608,& Cyc__gentuple_609};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_611,sizeof(struct Cyc_Absyn_MallocInfo),{(void*)((
struct _tuple5**)Cyc__genarr_610),(void*)((struct _tuple5**)Cyc__genarr_610 + 5)}};
struct _tuple80{unsigned int f1;struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6
Cyc__gentuple_612={offsetof(struct _tuple80,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_613={offsetof(struct _tuple80,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};
static struct _tuple6*Cyc__genarr_614[2]={& Cyc__gentuple_612,& Cyc__gentuple_613};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_604={4,sizeof(struct _tuple80),{(
void*)((struct _tuple6**)Cyc__genarr_614),(void*)((struct _tuple6**)Cyc__genarr_614
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_585;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_596;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static char _tmp3E6[4]="Opt";static struct _dynforward_ptr Cyc__genname_599=(struct
_dynforward_ptr){_tmp3E6,_tmp3E6 + 4};static char _tmp3E7[2]="v";static struct
_tuple5 Cyc__gentuple_597={offsetof(struct Cyc_Core_Opt,v),(struct _dynforward_ptr){
_tmp3E7,_tmp3E7 + 2},(void*)& Cyc__genrep_11};static struct _tuple5*Cyc__genarr_598[
1]={& Cyc__gentuple_597};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_599,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_598),(void*)((struct _tuple5**)Cyc__genarr_598 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_596={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};
struct _tuple81{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_600={offsetof(struct _tuple81,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_601={offsetof(struct _tuple81,f2),(void*)& Cyc__genrep_596};
static struct _tuple6 Cyc__gentuple_602={offsetof(struct _tuple81,f3),(void*)& Cyc__genrep_586};
static struct _tuple6*Cyc__genarr_603[3]={& Cyc__gentuple_600,& Cyc__gentuple_601,&
Cyc__gentuple_602};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_585={4,
sizeof(struct _tuple81),{(void*)((struct _tuple6**)Cyc__genarr_603),(void*)((
struct _tuple6**)Cyc__genarr_603 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_144;
static struct _tuple6 Cyc__gentuple_582={offsetof(struct _tuple37,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_583={offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_145};
static struct _tuple6*Cyc__genarr_584[2]={& Cyc__gentuple_582,& Cyc__gentuple_583};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_144={4,sizeof(struct _tuple37),{(
void*)((struct _tuple6**)Cyc__genarr_584),(void*)((struct _tuple6**)Cyc__genarr_584
+ 2)}};static struct _tuple7*Cyc__genarr_143[0]={};static char _tmp3EC[8]="Const_e";
static struct _tuple5 Cyc__gentuple_889={0,(struct _dynforward_ptr){_tmp3EC,_tmp3EC + 
8},(void*)& Cyc__genrep_859};static char _tmp3ED[6]="Var_e";static struct _tuple5 Cyc__gentuple_890={
1,(struct _dynforward_ptr){_tmp3ED,_tmp3ED + 6},(void*)& Cyc__genrep_807};static
char _tmp3EE[12]="UnknownId_e";static struct _tuple5 Cyc__gentuple_891={2,(struct
_dynforward_ptr){_tmp3EE,_tmp3EE + 12},(void*)& Cyc__genrep_255};static char _tmp3EF[
9]="Primop_e";static struct _tuple5 Cyc__gentuple_892={3,(struct _dynforward_ptr){
_tmp3EF,_tmp3EF + 9},(void*)& Cyc__genrep_802};static char _tmp3F0[11]="AssignOp_e";
static struct _tuple5 Cyc__gentuple_893={4,(struct _dynforward_ptr){_tmp3F0,_tmp3F0 + 
11},(void*)& Cyc__genrep_770};static char _tmp3F1[12]="Increment_e";static struct
_tuple5 Cyc__gentuple_894={5,(struct _dynforward_ptr){_tmp3F1,_tmp3F1 + 12},(void*)&
Cyc__genrep_759};static char _tmp3F2[14]="Conditional_e";static struct _tuple5 Cyc__gentuple_895={
6,(struct _dynforward_ptr){_tmp3F2,_tmp3F2 + 14},(void*)& Cyc__genrep_753};static
char _tmp3F3[6]="And_e";static struct _tuple5 Cyc__gentuple_896={7,(struct
_dynforward_ptr){_tmp3F3,_tmp3F3 + 6},(void*)& Cyc__genrep_678};static char _tmp3F4[
5]="Or_e";static struct _tuple5 Cyc__gentuple_897={8,(struct _dynforward_ptr){
_tmp3F4,_tmp3F4 + 5},(void*)& Cyc__genrep_678};static char _tmp3F5[9]="SeqExp_e";
static struct _tuple5 Cyc__gentuple_898={9,(struct _dynforward_ptr){_tmp3F5,_tmp3F5 + 
9},(void*)& Cyc__genrep_678};static char _tmp3F6[14]="UnknownCall_e";static struct
_tuple5 Cyc__gentuple_899={10,(struct _dynforward_ptr){_tmp3F6,_tmp3F6 + 14},(void*)&
Cyc__genrep_748};static char _tmp3F7[9]="FnCall_e";static struct _tuple5 Cyc__gentuple_900={
11,(struct _dynforward_ptr){_tmp3F7,_tmp3F7 + 9},(void*)& Cyc__genrep_729};static
char _tmp3F8[8]="Throw_e";static struct _tuple5 Cyc__gentuple_901={12,(struct
_dynforward_ptr){_tmp3F8,_tmp3F8 + 8},(void*)& Cyc__genrep_153};static char _tmp3F9[
16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_902={13,(struct
_dynforward_ptr){_tmp3F9,_tmp3F9 + 16},(void*)& Cyc__genrep_153};static char _tmp3FA[
14]="Instantiate_e";static struct _tuple5 Cyc__gentuple_903={14,(struct
_dynforward_ptr){_tmp3FA,_tmp3FA + 14},(void*)& Cyc__genrep_724};static char _tmp3FB[
7]="Cast_e";static struct _tuple5 Cyc__gentuple_904={15,(struct _dynforward_ptr){
_tmp3FB,_tmp3FB + 7},(void*)& Cyc__genrep_711};static char _tmp3FC[10]="Address_e";
static struct _tuple5 Cyc__gentuple_905={16,(struct _dynforward_ptr){_tmp3FC,_tmp3FC
+ 10},(void*)& Cyc__genrep_153};static char _tmp3FD[6]="New_e";static struct _tuple5
Cyc__gentuple_906={17,(struct _dynforward_ptr){_tmp3FD,_tmp3FD + 6},(void*)& Cyc__genrep_706};
static char _tmp3FE[12]="Sizeoftyp_e";static struct _tuple5 Cyc__gentuple_907={18,(
struct _dynforward_ptr){_tmp3FE,_tmp3FE + 12},(void*)& Cyc__genrep_116};static char
_tmp3FF[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_908={19,(struct
_dynforward_ptr){_tmp3FF,_tmp3FF + 12},(void*)& Cyc__genrep_153};static char _tmp400[
11]="Offsetof_e";static struct _tuple5 Cyc__gentuple_909={20,(struct _dynforward_ptr){
_tmp400,_tmp400 + 11},(void*)& Cyc__genrep_693};static char _tmp401[9]="Gentyp_e";
static struct _tuple5 Cyc__gentuple_910={21,(struct _dynforward_ptr){_tmp401,_tmp401
+ 9},(void*)& Cyc__genrep_688};static char _tmp402[8]="Deref_e";static struct _tuple5
Cyc__gentuple_911={22,(struct _dynforward_ptr){_tmp402,_tmp402 + 8},(void*)& Cyc__genrep_153};
static char _tmp403[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_912={23,(
struct _dynforward_ptr){_tmp403,_tmp403 + 13},(void*)& Cyc__genrep_683};static char
_tmp404[12]="AggrArrow_e";static struct _tuple5 Cyc__gentuple_913={24,(struct
_dynforward_ptr){_tmp404,_tmp404 + 12},(void*)& Cyc__genrep_683};static char _tmp405[
12]="Subscript_e";static struct _tuple5 Cyc__gentuple_914={25,(struct
_dynforward_ptr){_tmp405,_tmp405 + 12},(void*)& Cyc__genrep_678};static char _tmp406[
8]="Tuple_e";static struct _tuple5 Cyc__gentuple_915={26,(struct _dynforward_ptr){
_tmp406,_tmp406 + 8},(void*)& Cyc__genrep_674};static char _tmp407[14]="CompoundLit_e";
static struct _tuple5 Cyc__gentuple_916={27,(struct _dynforward_ptr){_tmp407,_tmp407
+ 14},(void*)& Cyc__genrep_659};static char _tmp408[8]="Array_e";static struct
_tuple5 Cyc__gentuple_917={28,(struct _dynforward_ptr){_tmp408,_tmp408 + 8},(void*)&
Cyc__genrep_655};static char _tmp409[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_918={
29,(struct _dynforward_ptr){_tmp409,_tmp409 + 16},(void*)& Cyc__genrep_648};static
char _tmp40A[9]="Struct_e";static struct _tuple5 Cyc__gentuple_919={30,(struct
_dynforward_ptr){_tmp40A,_tmp40A + 9},(void*)& Cyc__genrep_640};static char _tmp40B[
13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_920={31,(struct
_dynforward_ptr){_tmp40B,_tmp40B + 13},(void*)& Cyc__genrep_635};static char _tmp40C[
9]="Tunion_e";static struct _tuple5 Cyc__gentuple_921={32,(struct _dynforward_ptr){
_tmp40C,_tmp40C + 9},(void*)& Cyc__genrep_629};static char _tmp40D[7]="Enum_e";
static struct _tuple5 Cyc__gentuple_922={33,(struct _dynforward_ptr){_tmp40D,_tmp40D
+ 7},(void*)& Cyc__genrep_622};static char _tmp40E[11]="AnonEnum_e";static struct
_tuple5 Cyc__gentuple_923={34,(struct _dynforward_ptr){_tmp40E,_tmp40E + 11},(void*)&
Cyc__genrep_615};static char _tmp40F[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_924={
35,(struct _dynforward_ptr){_tmp40F,_tmp40F + 9},(void*)& Cyc__genrep_604};static
char _tmp410[16]="UnresolvedMem_e";static struct _tuple5 Cyc__gentuple_925={36,(
struct _dynforward_ptr){_tmp410,_tmp410 + 16},(void*)& Cyc__genrep_585};static char
_tmp411[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_926={37,(struct
_dynforward_ptr){_tmp411,_tmp411 + 10},(void*)& Cyc__genrep_144};static struct
_tuple5*Cyc__genarr_927[38]={& Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891,&
Cyc__gentuple_892,& Cyc__gentuple_893,& Cyc__gentuple_894,& Cyc__gentuple_895,& Cyc__gentuple_896,&
Cyc__gentuple_897,& Cyc__gentuple_898,& Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,&
Cyc__gentuple_902,& Cyc__gentuple_903,& Cyc__gentuple_904,& Cyc__gentuple_905,& Cyc__gentuple_906,&
Cyc__gentuple_907,& Cyc__gentuple_908,& Cyc__gentuple_909,& Cyc__gentuple_910,& Cyc__gentuple_911,&
Cyc__gentuple_912,& Cyc__gentuple_913,& Cyc__gentuple_914,& Cyc__gentuple_915,& Cyc__gentuple_916,&
Cyc__gentuple_917,& Cyc__gentuple_918,& Cyc__gentuple_919,& Cyc__gentuple_920,& Cyc__gentuple_921,&
Cyc__gentuple_922,& Cyc__gentuple_923,& Cyc__gentuple_924,& Cyc__gentuple_925,& Cyc__gentuple_926};
static char _tmp413[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep={
5,(struct _dynforward_ptr){_tmp413,_tmp413 + 8},{(void*)((struct _tuple7**)Cyc__genarr_143),(
void*)((struct _tuple7**)Cyc__genarr_143 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_927),(
void*)((struct _tuple5**)Cyc__genarr_927 + 38)}};static char _tmp414[4]="Exp";static
struct _dynforward_ptr Cyc__genname_933=(struct _dynforward_ptr){_tmp414,_tmp414 + 4};
static char _tmp415[5]="topt";static struct _tuple5 Cyc__gentuple_928={offsetof(
struct Cyc_Absyn_Exp,topt),(struct _dynforward_ptr){_tmp415,_tmp415 + 5},(void*)&
Cyc__genrep_110};static char _tmp416[2]="r";static struct _tuple5 Cyc__gentuple_929={
offsetof(struct Cyc_Absyn_Exp,r),(struct _dynforward_ptr){_tmp416,_tmp416 + 2},(
void*)& Cyc_Absyn_raw_exp_t_rep};static char _tmp417[4]="loc";static struct _tuple5
Cyc__gentuple_930={offsetof(struct Cyc_Absyn_Exp,loc),(struct _dynforward_ptr){
_tmp417,_tmp417 + 4},(void*)& Cyc__genrep_2};static char _tmp418[6]="annot";static
struct _tuple5 Cyc__gentuple_931={offsetof(struct Cyc_Absyn_Exp,annot),(struct
_dynforward_ptr){_tmp418,_tmp418 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static
struct _tuple5*Cyc__genarr_932[4]={& Cyc__gentuple_928,& Cyc__gentuple_929,& Cyc__gentuple_930,&
Cyc__gentuple_931};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_933,sizeof(struct Cyc_Absyn_Exp),{(void*)((
struct _tuple5**)Cyc__genarr_932),(void*)((struct _tuple5**)Cyc__genarr_932 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_141={1,1,(void*)((void*)& Cyc_struct_Absyn_Exp_rep)};
static char _tmp41B[10]="ArrayInfo";static struct _dynforward_ptr Cyc__genname_1003=(
struct _dynforward_ptr){_tmp41B,_tmp41B + 10};static char _tmp41C[9]="elt_type";
static struct _tuple5 Cyc__gentuple_997={offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),(
struct _dynforward_ptr){_tmp41C,_tmp41C + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp41D[3]="tq";static struct _tuple5 Cyc__gentuple_998={offsetof(struct
Cyc_Absyn_ArrayInfo,tq),(struct _dynforward_ptr){_tmp41D,_tmp41D + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp41E[9]="num_elts";static struct _tuple5 Cyc__gentuple_999={offsetof(
struct Cyc_Absyn_ArrayInfo,num_elts),(struct _dynforward_ptr){_tmp41E,_tmp41E + 9},(
void*)& Cyc__genrep_141};static char _tmp41F[10]="zero_term";static struct _tuple5 Cyc__gentuple_1000={
offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),(struct _dynforward_ptr){_tmp41F,
_tmp41F + 10},(void*)& Cyc__genrep_984};static char _tmp420[7]="zt_loc";static struct
_tuple5 Cyc__gentuple_1001={offsetof(struct Cyc_Absyn_ArrayInfo,zt_loc),(struct
_dynforward_ptr){_tmp420,_tmp420 + 7},(void*)& Cyc__genrep_2};static struct _tuple5*
Cyc__genarr_1002[5]={& Cyc__gentuple_997,& Cyc__gentuple_998,& Cyc__gentuple_999,&
Cyc__gentuple_1000,& Cyc__gentuple_1001};struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1003,sizeof(struct Cyc_Absyn_ArrayInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1002),(void*)((struct _tuple5**)Cyc__genarr_1002
+ 5)}};struct _tuple82{unsigned int f1;struct Cyc_Absyn_ArrayInfo f2;};static struct
_tuple6 Cyc__gentuple_1004={offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1005={offsetof(struct _tuple82,f2),(void*)& Cyc_Absyn_array_info_t_rep};
static struct _tuple6*Cyc__genarr_1006[2]={& Cyc__gentuple_1004,& Cyc__gentuple_1005};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_983={4,sizeof(struct _tuple82),{(
void*)((struct _tuple6**)Cyc__genarr_1006),(void*)((struct _tuple6**)Cyc__genarr_1006
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_964;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_fn_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_965;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp423[5]="List";static struct _dynforward_ptr Cyc__genname_969=(struct
_dynforward_ptr){_tmp423,_tmp423 + 5};static char _tmp424[3]="hd";static struct
_tuple5 Cyc__gentuple_966={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp424,_tmp424 + 3},(void*)& Cyc__genrep_660};static char _tmp425[
3]="tl";static struct _tuple5 Cyc__gentuple_967={offsetof(struct Cyc_List_List,tl),(
struct _dynforward_ptr){_tmp425,_tmp425 + 3},(void*)& Cyc__genrep_965};static struct
_tuple5*Cyc__genarr_968[2]={& Cyc__gentuple_966,& Cyc__gentuple_967};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_969,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_968),(void*)((struct _tuple5**)Cyc__genarr_968 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_965={1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp428[7]="FnInfo";static struct _dynforward_ptr Cyc__genname_979=(
struct _dynforward_ptr){_tmp428,_tmp428 + 7};static char _tmp429[6]="tvars";static
struct _tuple5 Cyc__gentuple_970={offsetof(struct Cyc_Absyn_FnInfo,tvars),(struct
_dynforward_ptr){_tmp429,_tmp429 + 6},(void*)& Cyc__genrep_321};static char _tmp42A[
7]="effect";static struct _tuple5 Cyc__gentuple_971={offsetof(struct Cyc_Absyn_FnInfo,effect),(
struct _dynforward_ptr){_tmp42A,_tmp42A + 7},(void*)& Cyc__genrep_110};static char
_tmp42B[8]="ret_typ";static struct _tuple5 Cyc__gentuple_972={offsetof(struct Cyc_Absyn_FnInfo,ret_typ),(
struct _dynforward_ptr){_tmp42B,_tmp42B + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp42C[5]="args";static struct _tuple5 Cyc__gentuple_973={offsetof(
struct Cyc_Absyn_FnInfo,args),(struct _dynforward_ptr){_tmp42C,_tmp42C + 5},(void*)&
Cyc__genrep_965};static char _tmp42D[10]="c_varargs";static struct _tuple5 Cyc__gentuple_974={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),(struct _dynforward_ptr){_tmp42D,
_tmp42D + 10},(void*)((void*)& Cyc__genrep_10)};static char _tmp42E[12]="cyc_varargs";
static struct _tuple5 Cyc__gentuple_975={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),(
struct _dynforward_ptr){_tmp42E,_tmp42E + 12},(void*)& Cyc__genrep_817};static char
_tmp42F[7]="rgn_po";static struct _tuple5 Cyc__gentuple_976={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),(
struct _dynforward_ptr){_tmp42F,_tmp42F + 7},(void*)& Cyc__genrep_382};static char
_tmp430[11]="attributes";static struct _tuple5 Cyc__gentuple_977={offsetof(struct
Cyc_Absyn_FnInfo,attributes),(struct _dynforward_ptr){_tmp430,_tmp430 + 11},(void*)&
Cyc__genrep_62};static struct _tuple5*Cyc__genarr_978[8]={& Cyc__gentuple_970,& Cyc__gentuple_971,&
Cyc__gentuple_972,& Cyc__gentuple_973,& Cyc__gentuple_974,& Cyc__gentuple_975,& Cyc__gentuple_976,&
Cyc__gentuple_977};struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(
struct _dynforward_ptr*)& Cyc__genname_979,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((
struct _tuple5**)Cyc__genarr_978),(void*)((struct _tuple5**)Cyc__genarr_978 + 8)}};
struct _tuple83{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct _tuple6 Cyc__gentuple_980={
offsetof(struct _tuple83,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_981={
offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_fn_info_t_rep};static struct _tuple6*
Cyc__genarr_982[2]={& Cyc__gentuple_980,& Cyc__gentuple_981};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_964={4,sizeof(struct _tuple83),{(void*)((struct _tuple6**)Cyc__genarr_982),(
void*)((struct _tuple6**)Cyc__genarr_982 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_960;static struct _tuple6 Cyc__gentuple_961={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_962={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_295};static struct _tuple6*Cyc__genarr_963[2]={&
Cyc__gentuple_961,& Cyc__gentuple_962};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_960={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_963),(void*)((
struct _tuple6**)Cyc__genarr_963 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_956;
struct _tuple84{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6
Cyc__gentuple_957={offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_958={offsetof(struct _tuple84,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};
static struct _tuple6*Cyc__genarr_959[2]={& Cyc__gentuple_957,& Cyc__gentuple_958};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_956={4,sizeof(struct _tuple84),{(
void*)((struct _tuple6**)Cyc__genarr_959),(void*)((struct _tuple6**)Cyc__genarr_959
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_951;static struct _tuple6
Cyc__gentuple_952={offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_953={offsetof(struct _tuple64,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_954={offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_369};
static struct _tuple6*Cyc__genarr_955[3]={& Cyc__gentuple_952,& Cyc__gentuple_953,&
Cyc__gentuple_954};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_951={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_955),(void*)((
struct _tuple6**)Cyc__genarr_955 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_946;
struct _tuple85{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};
static struct _tuple6 Cyc__gentuple_947={offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_948={offsetof(struct _tuple85,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_949={offsetof(struct _tuple85,f3),(void*)& Cyc__genrep_623};
static struct _tuple6*Cyc__genarr_950[3]={& Cyc__gentuple_947,& Cyc__gentuple_948,&
Cyc__gentuple_949};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_946={4,
sizeof(struct _tuple85),{(void*)((struct _tuple6**)Cyc__genarr_950),(void*)((
struct _tuple6**)Cyc__genarr_950 + 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_138;
static struct _tuple6 Cyc__gentuple_943={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_944={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_139};
static struct _tuple6*Cyc__genarr_945[2]={& Cyc__gentuple_943,& Cyc__gentuple_944};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_138={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_945),(void*)((struct _tuple6**)Cyc__genarr_945
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_129;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_131;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;
static char _tmp438[12]="Typedefdecl";static struct _dynforward_ptr Cyc__genname_1150=(
struct _dynforward_ptr){_tmp438,_tmp438 + 12};static char _tmp439[5]="name";static
struct _tuple5 Cyc__gentuple_1143={offsetof(struct Cyc_Absyn_Typedefdecl,name),(
struct _dynforward_ptr){_tmp439,_tmp439 + 5},(void*)& Cyc__genrep_11};static char
_tmp43A[3]="tq";static struct _tuple5 Cyc__gentuple_1144={offsetof(struct Cyc_Absyn_Typedefdecl,tq),(
struct _dynforward_ptr){_tmp43A,_tmp43A + 3},(void*)& Cyc_Absyn_tqual_t_rep};static
char _tmp43B[4]="tvs";static struct _tuple5 Cyc__gentuple_1145={offsetof(struct Cyc_Absyn_Typedefdecl,tvs),(
struct _dynforward_ptr){_tmp43B,_tmp43B + 4},(void*)& Cyc__genrep_321};static char
_tmp43C[5]="kind";static struct _tuple5 Cyc__gentuple_1146={offsetof(struct Cyc_Absyn_Typedefdecl,kind),(
struct _dynforward_ptr){_tmp43C,_tmp43C + 5},(void*)& Cyc__genrep_1107};static char
_tmp43D[5]="defn";static struct _tuple5 Cyc__gentuple_1147={offsetof(struct Cyc_Absyn_Typedefdecl,defn),(
struct _dynforward_ptr){_tmp43D,_tmp43D + 5},(void*)& Cyc__genrep_110};static char
_tmp43E[5]="atts";static struct _tuple5 Cyc__gentuple_1148={offsetof(struct Cyc_Absyn_Typedefdecl,atts),(
struct _dynforward_ptr){_tmp43E,_tmp43E + 5},(void*)& Cyc__genrep_62};static struct
_tuple5*Cyc__genarr_1149[6]={& Cyc__gentuple_1143,& Cyc__gentuple_1144,& Cyc__gentuple_1145,&
Cyc__gentuple_1146,& Cyc__gentuple_1147,& Cyc__gentuple_1148};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1150,
sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((struct _tuple5**)Cyc__genarr_1149),(
void*)((struct _tuple5**)Cyc__genarr_1149 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_131={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct
_tuple86{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*
f4;void**f5;};static struct _tuple6 Cyc__gentuple_132={offsetof(struct _tuple86,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_133={offsetof(struct
_tuple86,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_134={
offsetof(struct _tuple86,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_135={
offsetof(struct _tuple86,f4),(void*)& Cyc__genrep_131};static struct _tuple6 Cyc__gentuple_136={
offsetof(struct _tuple86,f5),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_137[
5]={& Cyc__gentuple_132,& Cyc__gentuple_133,& Cyc__gentuple_134,& Cyc__gentuple_135,&
Cyc__gentuple_136};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_129={4,
sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_137),(void*)((
struct _tuple6**)Cyc__genarr_137 + 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_120;
static struct _tuple6 Cyc__gentuple_126={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_127={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_121};
static struct _tuple6*Cyc__genarr_128[2]={& Cyc__gentuple_126,& Cyc__gentuple_127};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_120={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_128),(void*)((struct _tuple6**)Cyc__genarr_128
+ 2)}};static char _tmp443[9]="VoidType";static struct _tuple7 Cyc__gentuple_111={0,(
struct _dynforward_ptr){_tmp443,_tmp443 + 9}};static char _tmp444[10]="FloatType";
static struct _tuple7 Cyc__gentuple_112={1,(struct _dynforward_ptr){_tmp444,_tmp444 + 
10}};static char _tmp445[8]="HeapRgn";static struct _tuple7 Cyc__gentuple_113={2,(
struct _dynforward_ptr){_tmp445,_tmp445 + 8}};static char _tmp446[10]="UniqueRgn";
static struct _tuple7 Cyc__gentuple_114={3,(struct _dynforward_ptr){_tmp446,_tmp446 + 
10}};static struct _tuple7*Cyc__genarr_115[4]={& Cyc__gentuple_111,& Cyc__gentuple_112,&
Cyc__gentuple_113,& Cyc__gentuple_114};static char _tmp447[5]="Evar";static struct
_tuple5 Cyc__gentuple_1117={0,(struct _dynforward_ptr){_tmp447,_tmp447 + 5},(void*)&
Cyc__genrep_1102};static char _tmp448[8]="VarType";static struct _tuple5 Cyc__gentuple_1118={
1,(struct _dynforward_ptr){_tmp448,_tmp448 + 8},(void*)& Cyc__genrep_1098};static
char _tmp449[11]="TunionType";static struct _tuple5 Cyc__gentuple_1119={2,(struct
_dynforward_ptr){_tmp449,_tmp449 + 11},(void*)& Cyc__genrep_1071};static char
_tmp44A[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1120={3,(struct
_dynforward_ptr){_tmp44A,_tmp44A + 16},(void*)& Cyc__genrep_1045};static char
_tmp44B[12]="PointerType";static struct _tuple5 Cyc__gentuple_1121={4,(struct
_dynforward_ptr){_tmp44B,_tmp44B + 12},(void*)& Cyc__genrep_1019};static char
_tmp44C[8]="IntType";static struct _tuple5 Cyc__gentuple_1122={5,(struct
_dynforward_ptr){_tmp44C,_tmp44C + 8},(void*)& Cyc__genrep_1007};static char _tmp44D[
11]="DoubleType";static struct _tuple5 Cyc__gentuple_1123={6,(struct _dynforward_ptr){
_tmp44D,_tmp44D + 11},(void*)& Cyc__genrep_85};static char _tmp44E[10]="ArrayType";
static struct _tuple5 Cyc__gentuple_1124={7,(struct _dynforward_ptr){_tmp44E,_tmp44E
+ 10},(void*)& Cyc__genrep_983};static char _tmp44F[7]="FnType";static struct _tuple5
Cyc__gentuple_1125={8,(struct _dynforward_ptr){_tmp44F,_tmp44F + 7},(void*)& Cyc__genrep_964};
static char _tmp450[10]="TupleType";static struct _tuple5 Cyc__gentuple_1126={9,(
struct _dynforward_ptr){_tmp450,_tmp450 + 10},(void*)& Cyc__genrep_960};static char
_tmp451[9]="AggrType";static struct _tuple5 Cyc__gentuple_1127={10,(struct
_dynforward_ptr){_tmp451,_tmp451 + 9},(void*)& Cyc__genrep_956};static char _tmp452[
13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1128={11,(struct
_dynforward_ptr){_tmp452,_tmp452 + 13},(void*)& Cyc__genrep_951};static char _tmp453[
9]="EnumType";static struct _tuple5 Cyc__gentuple_1129={12,(struct _dynforward_ptr){
_tmp453,_tmp453 + 9},(void*)& Cyc__genrep_946};static char _tmp454[13]="AnonEnumType";
static struct _tuple5 Cyc__gentuple_1130={13,(struct _dynforward_ptr){_tmp454,
_tmp454 + 13},(void*)& Cyc__genrep_138};static char _tmp455[11]="SizeofType";static
struct _tuple5 Cyc__gentuple_1131={14,(struct _dynforward_ptr){_tmp455,_tmp455 + 11},(
void*)& Cyc__genrep_116};static char _tmp456[14]="RgnHandleType";static struct
_tuple5 Cyc__gentuple_1132={15,(struct _dynforward_ptr){_tmp456,_tmp456 + 14},(void*)&
Cyc__genrep_116};static char _tmp457[12]="TypedefType";static struct _tuple5 Cyc__gentuple_1133={
16,(struct _dynforward_ptr){_tmp457,_tmp457 + 12},(void*)& Cyc__genrep_129};static
char _tmp458[8]="TagType";static struct _tuple5 Cyc__gentuple_1134={17,(struct
_dynforward_ptr){_tmp458,_tmp458 + 8},(void*)& Cyc__genrep_116};static char _tmp459[
8]="TypeInt";static struct _tuple5 Cyc__gentuple_1135={18,(struct _dynforward_ptr){
_tmp459,_tmp459 + 8},(void*)& Cyc__genrep_85};static char _tmp45A[10]="AccessEff";
static struct _tuple5 Cyc__gentuple_1136={19,(struct _dynforward_ptr){_tmp45A,
_tmp45A + 10},(void*)& Cyc__genrep_116};static char _tmp45B[8]="JoinEff";static
struct _tuple5 Cyc__gentuple_1137={20,(struct _dynforward_ptr){_tmp45B,_tmp45B + 8},(
void*)& Cyc__genrep_120};static char _tmp45C[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1138={
21,(struct _dynforward_ptr){_tmp45C,_tmp45C + 8},(void*)& Cyc__genrep_116};static
struct _tuple5*Cyc__genarr_1139[22]={& Cyc__gentuple_1117,& Cyc__gentuple_1118,& Cyc__gentuple_1119,&
Cyc__gentuple_1120,& Cyc__gentuple_1121,& Cyc__gentuple_1122,& Cyc__gentuple_1123,&
Cyc__gentuple_1124,& Cyc__gentuple_1125,& Cyc__gentuple_1126,& Cyc__gentuple_1127,&
Cyc__gentuple_1128,& Cyc__gentuple_1129,& Cyc__gentuple_1130,& Cyc__gentuple_1131,&
Cyc__gentuple_1132,& Cyc__gentuple_1133,& Cyc__gentuple_1134,& Cyc__gentuple_1135,&
Cyc__gentuple_1136,& Cyc__gentuple_1137,& Cyc__gentuple_1138};static char _tmp45E[5]="Type";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={5,(struct _dynforward_ptr){
_tmp45E,_tmp45E + 5},{(void*)((struct _tuple7**)Cyc__genarr_115),(void*)((struct
_tuple7**)Cyc__genarr_115 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_1139),(void*)((
struct _tuple5**)Cyc__genarr_1139 + 22)}};static char _tmp45F[8]="Vardecl";static
struct _dynforward_ptr Cyc__genname_184=(struct _dynforward_ptr){_tmp45F,_tmp45F + 8};
static char _tmp460[3]="sc";static struct _tuple5 Cyc__gentuple_175={offsetof(struct
Cyc_Absyn_Vardecl,sc),(struct _dynforward_ptr){_tmp460,_tmp460 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp461[5]="name";static struct _tuple5 Cyc__gentuple_176={offsetof(
struct Cyc_Absyn_Vardecl,name),(struct _dynforward_ptr){_tmp461,_tmp461 + 5},(void*)&
Cyc__genrep_11};static char _tmp462[3]="tq";static struct _tuple5 Cyc__gentuple_177={
offsetof(struct Cyc_Absyn_Vardecl,tq),(struct _dynforward_ptr){_tmp462,_tmp462 + 3},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp463[5]="type";static struct _tuple5 Cyc__gentuple_178={
offsetof(struct Cyc_Absyn_Vardecl,type),(struct _dynforward_ptr){_tmp463,_tmp463 + 
5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp464[12]="initializer";
static struct _tuple5 Cyc__gentuple_179={offsetof(struct Cyc_Absyn_Vardecl,initializer),(
struct _dynforward_ptr){_tmp464,_tmp464 + 12},(void*)& Cyc__genrep_141};static char
_tmp465[4]="rgn";static struct _tuple5 Cyc__gentuple_180={offsetof(struct Cyc_Absyn_Vardecl,rgn),(
struct _dynforward_ptr){_tmp465,_tmp465 + 4},(void*)& Cyc__genrep_110};static char
_tmp466[11]="attributes";static struct _tuple5 Cyc__gentuple_181={offsetof(struct
Cyc_Absyn_Vardecl,attributes),(struct _dynforward_ptr){_tmp466,_tmp466 + 11},(void*)&
Cyc__genrep_62};static char _tmp467[8]="escapes";static struct _tuple5 Cyc__gentuple_182={
offsetof(struct Cyc_Absyn_Vardecl,escapes),(struct _dynforward_ptr){_tmp467,
_tmp467 + 8},(void*)((void*)& Cyc__genrep_10)};static struct _tuple5*Cyc__genarr_183[
8]={& Cyc__gentuple_175,& Cyc__gentuple_176,& Cyc__gentuple_177,& Cyc__gentuple_178,&
Cyc__gentuple_179,& Cyc__gentuple_180,& Cyc__gentuple_181,& Cyc__gentuple_182};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={3,(struct
_dynforward_ptr*)& Cyc__genname_184,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_183),(void*)((struct _tuple5**)Cyc__genarr_183 + 8)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159={1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};
struct _tuple87{unsigned int f1;struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6
Cyc__gentuple_811={offsetof(struct _tuple87,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_812={offsetof(struct _tuple87,f2),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_813[2]={& Cyc__gentuple_811,& Cyc__gentuple_812};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_810={4,sizeof(struct _tuple87),{(
void*)((struct _tuple6**)Cyc__genarr_813),(void*)((struct _tuple6**)Cyc__genarr_813
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1170;struct _tuple88{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1171={offsetof(struct _tuple88,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1172={offsetof(struct _tuple88,f2),(
void*)& Cyc__genrep_240};static struct _tuple6 Cyc__gentuple_1173={offsetof(struct
_tuple88,f3),(void*)& Cyc__genrep_231};static struct _tuple6 Cyc__gentuple_1174={
offsetof(struct _tuple88,f4),(void*)& Cyc__genrep_154};static struct _tuple6*Cyc__genarr_1175[
4]={& Cyc__gentuple_1171,& Cyc__gentuple_1172,& Cyc__gentuple_1173,& Cyc__gentuple_1174};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1170={4,sizeof(struct _tuple88),{(
void*)((struct _tuple6**)Cyc__genarr_1175),(void*)((struct _tuple6**)Cyc__genarr_1175
+ 4)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1166;static struct _tuple6
Cyc__gentuple_1167={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_1168={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_232};
static struct _tuple6*Cyc__genarr_1169[2]={& Cyc__gentuple_1167,& Cyc__gentuple_1168};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1166={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_1169),(void*)((struct _tuple6**)Cyc__genarr_1169
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1162;struct _tuple89{
unsigned int f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1163={
offsetof(struct _tuple89,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1164={
offsetof(struct _tuple89,f2),(void*)((void*)& Cyc__genrep_367)};static struct
_tuple6*Cyc__genarr_1165[2]={& Cyc__gentuple_1163,& Cyc__gentuple_1164};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1162={4,sizeof(struct _tuple89),{(void*)((
struct _tuple6**)Cyc__genarr_1165),(void*)((struct _tuple6**)Cyc__genarr_1165 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1158;struct _tuple90{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;};static struct _tuple6 Cyc__gentuple_1159={
offsetof(struct _tuple90,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1160={
offsetof(struct _tuple90,f2),(void*)((void*)& Cyc__genrep_311)};static struct
_tuple6*Cyc__genarr_1161[2]={& Cyc__gentuple_1159,& Cyc__gentuple_1160};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1158={4,sizeof(struct _tuple90),{(void*)((
struct _tuple6**)Cyc__genarr_1161),(void*)((struct _tuple6**)Cyc__genarr_1161 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1154;struct _tuple91{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1155={
offsetof(struct _tuple91,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1156={
offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_265};static struct _tuple6*Cyc__genarr_1157[
2]={& Cyc__gentuple_1155,& Cyc__gentuple_1156};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1154={4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_1157),(
void*)((struct _tuple6**)Cyc__genarr_1157 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_60;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61;static struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};
struct _tuple92{unsigned int f1;struct Cyc_Absyn_Typedefdecl*f2;};static struct
_tuple6 Cyc__gentuple_1151={offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1152={offsetof(struct _tuple92,f2),(void*)& Cyc__genrep_61};
static struct _tuple6*Cyc__genarr_1153[2]={& Cyc__gentuple_1151,& Cyc__gentuple_1152};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_60={4,sizeof(struct _tuple92),{(
void*)((struct _tuple6**)Cyc__genarr_1153),(void*)((struct _tuple6**)Cyc__genarr_1153
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_55;struct _tuple93{
unsigned int f1;struct _dynforward_ptr*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_56={offsetof(struct _tuple93,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_57={offsetof(struct _tuple93,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_58={offsetof(struct _tuple93,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_59[3]={& Cyc__gentuple_56,& Cyc__gentuple_57,& Cyc__gentuple_58};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_55={4,sizeof(struct _tuple93),{(
void*)((struct _tuple6**)Cyc__genarr_59),(void*)((struct _tuple6**)Cyc__genarr_59 + 
3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_50;struct _tuple94{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_51={
offsetof(struct _tuple94,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_52={
offsetof(struct _tuple94,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_53={
offsetof(struct _tuple94,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_54[
3]={& Cyc__gentuple_51,& Cyc__gentuple_52,& Cyc__gentuple_53};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_50={4,sizeof(struct _tuple94),{(void*)((struct _tuple6**)Cyc__genarr_54),(
void*)((struct _tuple6**)Cyc__genarr_54 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_46;static struct _tuple6 Cyc__gentuple_47={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_48={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_49[2]={& Cyc__gentuple_47,&
Cyc__gentuple_48};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_46={4,sizeof(
struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_49),(void*)((struct _tuple6**)
Cyc__genarr_49 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_6;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep;extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_8;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_9;
struct _tuple95{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};static
struct _tuple6 Cyc__gentuple_34={offsetof(struct _tuple95,f1),(void*)& Cyc__genrep_2};
static struct _tuple6 Cyc__gentuple_35={offsetof(struct _tuple95,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_36={offsetof(struct _tuple95,f3),(void*)((void*)&
Cyc__genrep_10)};static struct _tuple6*Cyc__genarr_37[3]={& Cyc__gentuple_34,& Cyc__gentuple_35,&
Cyc__gentuple_36};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_9={4,sizeof(
struct _tuple95),{(void*)((struct _tuple6**)Cyc__genarr_37),(void*)((struct _tuple6**)
Cyc__genarr_37 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8={1,1,(
void*)((void*)& Cyc__genrep_9)};static char _tmp477[5]="List";static struct
_dynforward_ptr Cyc__genname_41=(struct _dynforward_ptr){_tmp477,_tmp477 + 5};
static char _tmp478[3]="hd";static struct _tuple5 Cyc__gentuple_38={offsetof(struct
Cyc_List_List,hd),(struct _dynforward_ptr){_tmp478,_tmp478 + 3},(void*)& Cyc__genrep_8};
static char _tmp479[3]="tl";static struct _tuple5 Cyc__gentuple_39={offsetof(struct
Cyc_List_List,tl),(struct _dynforward_ptr){_tmp479,_tmp479 + 3},(void*)& Cyc__genrep_7};
static struct _tuple5*Cyc__genarr_40[2]={& Cyc__gentuple_38,& Cyc__gentuple_39};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_41,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_40),(void*)((struct _tuple5**)Cyc__genarr_40 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7={1,1,(void*)((void*)& Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep)};
struct _tuple96{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_42={offsetof(struct _tuple96,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_43={offsetof(struct _tuple96,f2),(void*)& Cyc__genrep_0};
static struct _tuple6 Cyc__gentuple_44={offsetof(struct _tuple96,f3),(void*)& Cyc__genrep_7};
static struct _tuple6*Cyc__genarr_45[3]={& Cyc__gentuple_42,& Cyc__gentuple_43,& Cyc__gentuple_44};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_6={4,sizeof(struct _tuple96),{(
void*)((struct _tuple6**)Cyc__genarr_45),(void*)((struct _tuple6**)Cyc__genarr_45 + 
3)}};static char _tmp47D[9]="Porton_d";static struct _tuple7 Cyc__gentuple_3={0,(
struct _dynforward_ptr){_tmp47D,_tmp47D + 9}};static char _tmp47E[10]="Portoff_d";
static struct _tuple7 Cyc__gentuple_4={1,(struct _dynforward_ptr){_tmp47E,_tmp47E + 
10}};static struct _tuple7*Cyc__genarr_5[2]={& Cyc__gentuple_3,& Cyc__gentuple_4};
static char _tmp47F[6]="Var_d";static struct _tuple5 Cyc__gentuple_1176={0,(struct
_dynforward_ptr){_tmp47F,_tmp47F + 6},(void*)& Cyc__genrep_810};static char _tmp480[
5]="Fn_d";static struct _tuple5 Cyc__gentuple_1177={1,(struct _dynforward_ptr){
_tmp480,_tmp480 + 5},(void*)& Cyc__genrep_814};static char _tmp481[6]="Let_d";static
struct _tuple5 Cyc__gentuple_1178={2,(struct _dynforward_ptr){_tmp481,_tmp481 + 6},(
void*)& Cyc__genrep_1170};static char _tmp482[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1179={
3,(struct _dynforward_ptr){_tmp482,_tmp482 + 7},(void*)& Cyc__genrep_1166};static
char _tmp483[7]="Aggr_d";static struct _tuple5 Cyc__gentuple_1180={4,(struct
_dynforward_ptr){_tmp483,_tmp483 + 7},(void*)& Cyc__genrep_1162};static char _tmp484[
9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1181={5,(struct _dynforward_ptr){
_tmp484,_tmp484 + 9},(void*)& Cyc__genrep_1158};static char _tmp485[7]="Enum_d";
static struct _tuple5 Cyc__gentuple_1182={6,(struct _dynforward_ptr){_tmp485,_tmp485
+ 7},(void*)& Cyc__genrep_1154};static char _tmp486[10]="Typedef_d";static struct
_tuple5 Cyc__gentuple_1183={7,(struct _dynforward_ptr){_tmp486,_tmp486 + 10},(void*)&
Cyc__genrep_60};static char _tmp487[12]="Namespace_d";static struct _tuple5 Cyc__gentuple_1184={
8,(struct _dynforward_ptr){_tmp487,_tmp487 + 12},(void*)& Cyc__genrep_55};static
char _tmp488[8]="Using_d";static struct _tuple5 Cyc__gentuple_1185={9,(struct
_dynforward_ptr){_tmp488,_tmp488 + 8},(void*)& Cyc__genrep_50};static char _tmp489[
10]="ExternC_d";static struct _tuple5 Cyc__gentuple_1186={10,(struct _dynforward_ptr){
_tmp489,_tmp489 + 10},(void*)& Cyc__genrep_46};static char _tmp48A[17]="ExternCinclude_d";
static struct _tuple5 Cyc__gentuple_1187={11,(struct _dynforward_ptr){_tmp48A,
_tmp48A + 17},(void*)& Cyc__genrep_6};static struct _tuple5*Cyc__genarr_1188[12]={&
Cyc__gentuple_1176,& Cyc__gentuple_1177,& Cyc__gentuple_1178,& Cyc__gentuple_1179,&
Cyc__gentuple_1180,& Cyc__gentuple_1181,& Cyc__gentuple_1182,& Cyc__gentuple_1183,&
Cyc__gentuple_1184,& Cyc__gentuple_1185,& Cyc__gentuple_1186,& Cyc__gentuple_1187};
static char _tmp48C[9]="Raw_decl";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={
5,(struct _dynforward_ptr){_tmp48C,_tmp48C + 9},{(void*)((struct _tuple7**)Cyc__genarr_5),(
void*)((struct _tuple7**)Cyc__genarr_5 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_1188),(
void*)((struct _tuple5**)Cyc__genarr_1188 + 12)}};static char _tmp48D[5]="Decl";
static struct _dynforward_ptr Cyc__genname_1192=(struct _dynforward_ptr){_tmp48D,
_tmp48D + 5};static char _tmp48E[2]="r";static struct _tuple5 Cyc__gentuple_1189={
offsetof(struct Cyc_Absyn_Decl,r),(struct _dynforward_ptr){_tmp48E,_tmp48E + 2},(
void*)& Cyc_Absyn_raw_decl_t_rep};static char _tmp48F[4]="loc";static struct _tuple5
Cyc__gentuple_1190={offsetof(struct Cyc_Absyn_Decl,loc),(struct _dynforward_ptr){
_tmp48F,_tmp48F + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1191[
2]={& Cyc__gentuple_1189,& Cyc__gentuple_1190};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={
3,(struct _dynforward_ptr*)& Cyc__genname_1192,sizeof(struct Cyc_Absyn_Decl),{(void*)((
struct _tuple5**)Cyc__genarr_1191),(void*)((struct _tuple5**)Cyc__genarr_1191 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)& Cyc_struct_Absyn_Decl_rep)};
static char _tmp492[5]="List";static struct _dynforward_ptr Cyc__genname_1196=(struct
_dynforward_ptr){_tmp492,_tmp492 + 5};static char _tmp493[3]="hd";static struct
_tuple5 Cyc__gentuple_1193={offsetof(struct Cyc_List_List,hd),(struct
_dynforward_ptr){_tmp493,_tmp493 + 3},(void*)& Cyc__genrep_1};static char _tmp494[3]="tl";
static struct _tuple5 Cyc__gentuple_1194={offsetof(struct Cyc_List_List,tl),(struct
_dynforward_ptr){_tmp494,_tmp494 + 3},(void*)& Cyc__genrep_0};static struct _tuple5*
Cyc__genarr_1195[2]={& Cyc__gentuple_1193,& Cyc__gentuple_1194};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_decl_t46H2_rep={3,(struct _dynforward_ptr*)& Cyc__genname_1196,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_1195),(void*)((
struct _tuple5**)Cyc__genarr_1195 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};void*Cyc_decls_rep=(
void*)& Cyc__genrep_0;
